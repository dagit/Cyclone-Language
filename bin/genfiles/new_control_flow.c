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
fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);int Cyc_CfFlowInfo_is_unique_consumed(
struct Cyc_Absyn_Exp*e,int env_iteration,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
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
struct Cyc_Absyn_Stmt*encloser){struct Cyc_NewControlFlow_CFAnnot_struct _tmp74D;
struct Cyc_NewControlFlow_CFStmtAnnot _tmp74C;struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp74B;enclosee->annot=(void*)((_tmp74B=_cycalloc(sizeof(*_tmp74B)),((_tmp74B[0]=((
_tmp74D.tag=Cyc_NewControlFlow_CFAnnot,((_tmp74D.f1=((_tmp74C.encloser=encloser,((
_tmp74C.visited=0,_tmp74C)))),_tmp74D)))),_tmp74B))));}struct Cyc_NewControlFlow_AnalEnv{
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
_tmp753;const char*_tmp752;struct Cyc_Core_Impossible_struct*_tmp751;(int)_throw((
void*)((_tmp751=_cycalloc(sizeof(*_tmp751)),((_tmp751[0]=((_tmp753.tag=Cyc_Core_Impossible,((
_tmp753.f1=((_tmp752="ControlFlow -- wrong stmt annotation",_tag_dyneither(
_tmp752,sizeof(char),37))),_tmp753)))),_tmp751)))));}_LL0:;}static union Cyc_CfFlowInfo_FlowInfo*
Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s);static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*_tmp754;union Cyc_CfFlowInfo_FlowInfo*
res=(_tmp754=_region_malloc(env->r,sizeof(*_tmp754)),((_tmp754[0]=Cyc_CfFlowInfo_BottomFL(),
_tmp754)));((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union
Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);return
res;}return*sflow;}struct _tuple16{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union
Cyc_CfFlowInfo_FlowInfo*f2;};static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s);static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*
s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*
_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);_tmpB->visited=
env->iteration_num;{struct _tuple16 _tmp755;return(_tmp755.f1=_tmpB,((_tmp755.f2=
_tmpC,_tmp755)));};}static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*
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
_tmp759;void*_tmp758[1];struct Cyc_String_pa_struct _tmp757;(_tmp757.tag=0,((
_tmp757.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp15->name)),((_tmp758[0]=& _tmp757,Cyc_Tcutil_warn(_tmp11,((_tmp759="unique pointer(s) reachable from %s may become unreachable.",
_tag_dyneither(_tmp759,sizeof(char),60))),_tag_dyneither(_tmp758,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}goto _LL5;}goto _LL5;_LL8:;_LL9: goto _LL5;_LL5:;};}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow);static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp1E=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo*_tmp1F=
Cyc_NewControlFlow_get_stmt_flow(env,s);union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,flow,*_tmp1F);if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple15 _tmp75A;struct _tuple15 _tmp22=(_tmp75A.f1=flow,((_tmp75A.f2=_tmp20,
_tmp75A)));union Cyc_CfFlowInfo_FlowInfo _tmp23;struct _tuple12 _tmp24;struct Cyc_Dict_Dict
_tmp25;union Cyc_CfFlowInfo_FlowInfo _tmp26;struct _tuple12 _tmp27;struct Cyc_Dict_Dict
_tmp28;_LL14: _tmp23=_tmp22.f1;if((_tmp23.ReachableFL).tag != 2)goto _LL16;_tmp24=(
struct _tuple12)(_tmp23.ReachableFL).val;_tmp25=_tmp24.f1;_tmp26=_tmp22.f2;if((
_tmp26.ReachableFL).tag != 2)goto _LL16;_tmp27=(struct _tuple12)(_tmp26.ReachableFL).val;
_tmp28=_tmp27.f1;_LL15: {struct _tuple17 _tmp75B;struct _tuple17 _tmp29=(_tmp75B.f1=
env,((_tmp75B.f2=s->loc,((_tmp75B.f3=_tmp28,_tmp75B)))));((void(*)(void(*f)(
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
_tmp75E;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp75D;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp30=(_tmp75D=_region_malloc(fenv->r,sizeof(*_tmp75D)),((_tmp75D[0]=((_tmp75E.tag=
0,((_tmp75E.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp75E)))),_tmp75D)));_tmp2E=((
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
_tmp36,r)!= Cyc_CfFlowInfo_AllIL){const char*_tmp761;void*_tmp760;(_tmp760=0,Cyc_Tcutil_terr(
loc,((_tmp761="expression may not be fully initialized",_tag_dyneither(_tmp761,
sizeof(char),40))),_tag_dyneither(_tmp760,sizeof(void*),0)));}{struct Cyc_Dict_Dict
_tmp3A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp36,env->all_changed,r);if(
_tmp36.t == _tmp3A.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp3B=Cyc_CfFlowInfo_ReachableFL(
_tmp3A,_tmp37);Cyc_NewControlFlow_update_tryflow(env,_tmp3B);return _tmp3B;};};
_LL1D:;}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_NewControlFlow_check_nounique(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,void*
r);static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,void*t,void*r){struct _tuple0 _tmp762;struct
_tuple0 _tmp3D=(_tmp762.f1=Cyc_Tcutil_compress(t),((_tmp762.f2=r,_tmp762)));void*
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
if(!Cyc_Tcutil_is_noalias_pointer(t))goto _LL33;_LL32:{const char*_tmp765;void*
_tmp764;(_tmp764=0,Cyc_Tcutil_warn(loc,((_tmp765="argument may still contain unique pointers",
_tag_dyneither(_tmp765,sizeof(char),43))),_tag_dyneither(_tmp764,sizeof(void*),0)));}
return;_LL33:;_LL34: return;_LL22:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp6B=inflow;
int _tmp6C;struct _tuple12 _tmp6D;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*
_tmp6F;_LL36: if((_tmp6B.BottomFL).tag != 1)goto _LL38;_tmp6C=(int)(_tmp6B.BottomFL).val;
_LL37: return Cyc_CfFlowInfo_BottomFL();_LL38: if((_tmp6B.ReachableFL).tag != 2)goto
_LL35;_tmp6D=(struct _tuple12)(_tmp6B.ReachableFL).val;_tmp6E=_tmp6D.f1;_tmp6F=
_tmp6D.f2;_LL39: if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp768;void*
_tmp767;(_tmp767=0,Cyc_Tcutil_terr(loc,((_tmp768="noliveunique attribute requires unique pointer",
_tag_dyneither(_tmp768,sizeof(char),47))),_tag_dyneither(_tmp767,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{void*_tmp72=Cyc_Tcutil_pointer_elt_type(t);{
void*_tmp73=r;struct Cyc_CfFlowInfo_Place*_tmp75;_LL3B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp74=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp73;if(_tmp74->tag != 5)goto
_LL3D;else{_tmp75=_tmp74->f1;}}_LL3C: Cyc_NewControlFlow_check_nounique(env,loc,
_tmp72,Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp75));goto _LL3A;_LL3D:;_LL3E: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,_tmp72)){const char*_tmp76B;void*_tmp76A;(_tmp76A=0,Cyc_Tcutil_warn(
loc,((_tmp76B="argument may contain live unique pointers",_tag_dyneither(_tmp76B,
sizeof(char),42))),_tag_dyneither(_tmp76A,sizeof(void*),0)));}return Cyc_NewControlFlow_use_Rval(
env,loc,inflow,r);_LL3A:;}{struct Cyc_Dict_Dict _tmp78=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp6E,env->all_changed,r);if(_tmp6E.t == _tmp78.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo _tmp79=Cyc_CfFlowInfo_ReachableFL(_tmp78,_tmp6F);Cyc_NewControlFlow_update_tryflow(
env,_tmp79);return _tmp79;};};};_LL35:;}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo
f1;struct Cyc_List_List*f2;};static struct _tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy);static struct
_tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*
es,int first_is_copy,int others_are_copy){if(es == 0){struct _tuple19 _tmp76C;return(
_tmp76C.f1=inflow,((_tmp76C.f2=0,_tmp76C)));}if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo
_tmp7C;void*_tmp7D;struct _tuple13 _tmp7B=Cyc_NewControlFlow_anal_Rexp(env,
first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp7C=_tmp7B.f1;_tmp7D=
_tmp7B.f2;{struct Cyc_List_List*_tmp76F;struct _tuple19 _tmp76E;return(_tmp76E.f1=
_tmp7C,((_tmp76E.f2=((_tmp76F=_region_malloc(rgn,sizeof(*_tmp76F)),((_tmp76F->hd=
_tmp7D,((_tmp76F->tl=0,_tmp76F)))))),_tmp76E)));};}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo
old_inflow;union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_List_List*rvals;do{
this_all_changed=(env->fenv)->mt_place_set;{struct Cyc_Dict_Dict*_tmp770;env->all_changed=((
_tmp770=_region_malloc(env->r,sizeof(*_tmp770)),((_tmp770[0]=(env->fenv)->mt_place_set,
_tmp770))));}{union Cyc_CfFlowInfo_FlowInfo _tmp82;void*_tmp83;struct _tuple13
_tmp81=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;outflow=_tmp82;{struct Cyc_List_List*
_tmp771;rvals=((_tmp771=_region_malloc(rgn,sizeof(*_tmp771)),((_tmp771->hd=
_tmp83,((_tmp771->tl=0,_tmp771))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*
_tmp772;env->all_changed=((_tmp772=_region_malloc(env->r,sizeof(*_tmp772)),((
_tmp772[0]=(env->fenv)->mt_place_set,_tmp772))));}{union Cyc_CfFlowInfo_FlowInfo
_tmp87;void*_tmp88;struct _tuple13 _tmp86=Cyc_NewControlFlow_anal_Rexp(env,
others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp87=_tmp86.f1;_tmp88=
_tmp86.f2;{struct Cyc_List_List*_tmp773;rvals=((_tmp773=_region_malloc(rgn,
sizeof(*_tmp773)),((_tmp773->hd=_tmp88,((_tmp773->tl=rvals,_tmp773))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp87,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
old_inflow=inflow;inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,
inflow,outflow);};}while(!Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*_tmp774;env->all_changed=((
_tmp774=_region_malloc(env->r,sizeof(*_tmp774)),((_tmp774[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0),_tmp774))));}Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple19 _tmp775;return(_tmp775.f1=outflow,((_tmp775.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp775)));};};}
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
const char*_tmp778;void*_tmp777;(_tmp777=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp778="expression may not be initialized",
_tag_dyneither(_tmp778,sizeof(char),34))),_tag_dyneither(_tmp777,sizeof(void*),0)));}}
goto _LL3F;_LL42: if((_tmp90.BottomFL).tag != 1)goto _LL3F;_tmp93=(int)(_tmp90.BottomFL).val;
_LL43: goto _LL3F;_LL3F:;}{struct _tuple13 _tmp779;return(_tmp779.f1=_tmp8E,((
_tmp779.f2=(env->fenv)->unknown_all,_tmp779)));};}static union Cyc_CfFlowInfo_FlowInfo
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
_tmpA0.BottomFL).val;_LL50: {struct _tuple15 _tmp77A;return(_tmp77A.f1=outflow,((
_tmp77A.f2=outflow,_tmp77A)));}_LL51: if((_tmpA0.ReachableFL).tag != 2)goto _LL4E;
_tmpA2=(struct _tuple12)(_tmpA0.ReachableFL).val;_tmpA3=_tmpA2.f1;_tmpA4=_tmpA2.f2;
_LL52: {union Cyc_CfFlowInfo_AbsLVal _tmpA6=(Cyc_NewControlFlow_anal_Lexp(env,
inflow,0,e)).f2;int _tmpA7;struct Cyc_CfFlowInfo_Place*_tmpA8;_LL54: if((_tmpA6.UnknownL).tag
!= 2)goto _LL56;_tmpA7=(int)(_tmpA6.UnknownL).val;_LL55: {struct _tuple15 _tmp77B;
return(_tmp77B.f1=outflow,((_tmp77B.f2=outflow,_tmp77B)));}_LL56: if((_tmpA6.PlaceL).tag
!= 1)goto _LL53;_tmpA8=(struct Cyc_CfFlowInfo_Place*)(_tmpA6.PlaceL).val;_LL57: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
struct _tuple15 _tmp77C;return(_tmp77C.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,nzval),_tmpA4),((_tmp77C.f2=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,(env->fenv)->zero),
_tmpA4),_tmp77C)));}_LL53:;}_LL4E:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
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
_tmpB5=(int)(_tmpB4.BottomFL).val;_LL5F: {struct _tuple13 _tmp77D;return(_tmp77D.f1=
f,((_tmp77D.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_all),
_tmp77D)));}_LL60: if((_tmpB4.ReachableFL).tag != 2)goto _LL5D;_tmpB6=(struct
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
_LL76:{struct Cyc_CfFlowInfo_NotZero_struct _tmp780;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp77F;e->annot=(void*)((_tmp77F=_cycalloc(sizeof(*_tmp77F)),((_tmp77F[0]=((
_tmp780.tag=Cyc_CfFlowInfo_NotZero,((_tmp780.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp780)))),_tmp77F))));}goto _LL74;_LL77:;_LL78: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL74;_LL74:;}goto _LL6F;_LL6F:;}goto _LL62;_LL67: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpBD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpBA;if(_tmpBD->tag != 5)goto
_LL69;else{_tmpBE=_tmpBD->f1;}}_LL68:{void*_tmpC9=e->annot;struct Cyc_List_List*
_tmpCB;_LL7A: {struct Cyc_CfFlowInfo_NotZero_struct*_tmpCA=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpC9;if(_tmpCA->tag != Cyc_CfFlowInfo_NotZero)goto _LL7C;else{_tmpCB=_tmpCA->f1;}}
_LL7B: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpCB))goto _LL7D;goto _LL79;_LL7C:;
_LL7D:{void*_tmpCC=e->r;_LL7F: {struct Cyc_Absyn_Subscript_e_struct*_tmpCD=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpCC;if(_tmpCD->tag != 24)goto _LL81;}_LL80:{
struct Cyc_CfFlowInfo_NotZero_struct _tmp783;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp782;e->annot=(void*)((_tmp782=_cycalloc(sizeof(*_tmp782)),((_tmp782[0]=((
_tmp783.tag=Cyc_CfFlowInfo_NotZero,((_tmp783.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp783)))),_tmp782))));}goto _LL7E;_LL81:;_LL82: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL7E;_LL7E:;}goto _LL79;_LL79:;}{struct _tuple13 _tmp784;return(_tmp784.f1=f,((
_tmp784.f2=Cyc_CfFlowInfo_lookup_place(_tmpB7,_tmpBE),_tmp784)));};_LL69: {
struct Cyc_CfFlowInfo_Zero_struct*_tmpBF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmpBA;if(_tmpBF->tag != 0)goto _LL6B;}_LL6A: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple13 _tmp785;return(_tmp785.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp785.f2=
Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_all),_tmp785)));};
_LL6B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpC0=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpBA;if(_tmpC0->tag != 3)goto _LL6D;else{_tmpC1=_tmpC0->f1;}}_LL6C: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmpC1);goto _LL6E;_LL6D:;_LL6E:{void*_tmpD2=e->r;_LL84: {struct
Cyc_Absyn_Subscript_e_struct*_tmpD3=(struct Cyc_Absyn_Subscript_e_struct*)_tmpD2;
if(_tmpD3->tag != 24)goto _LL86;}_LL85:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp788;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp787;e->annot=(void*)((_tmp787=
_cycalloc(sizeof(*_tmp787)),((_tmp787[0]=((_tmp788.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp788.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpB8),_tmp788)))),
_tmp787))));}goto _LL83;_LL86:;_LL87: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL83;_LL83:;}goto _LL62;_LL62:;}else{void*_tmpD6=e->annot;struct Cyc_List_List*
_tmpD8;_LL89: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpD7=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmpD6;if(_tmpD7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL8B;else{_tmpD8=_tmpD7->f1;}}
_LL8A: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpD8))goto _LL8C;goto _LL88;_LL8B:;
_LL8C:{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp78B;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp78A;e->annot=(void*)((_tmp78A=_cycalloc(sizeof(*_tmp78A)),((_tmp78A[0]=((
_tmp78B.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp78B.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp78B)))),_tmp78A))));}goto _LL88;_LL88:;}switch(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmpB7,r)){case Cyc_CfFlowInfo_NoneIL: _LL8D:{void*_tmpDB=r;_LL90: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmpDC=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpDB;if(_tmpDC->tag != 7)goto _LL92;}_LL91:{const char*_tmp78E;void*_tmp78D;(
_tmp78D=0,Cyc_Tcutil_terr(e->loc,((_tmp78E="attempt to use a unique pointer that has already been copied",
_tag_dyneither(_tmp78E,sizeof(char),61))),_tag_dyneither(_tmp78D,sizeof(void*),0)));}
goto _LL8F;_LL92:;_LL93: {const char*_tmp791;void*_tmp790;(_tmp790=0,Cyc_Tcutil_terr(
e->loc,((_tmp791="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp791,sizeof(char),46))),_tag_dyneither(_tmp790,sizeof(void*),0)));}_LL8F:;}
goto _LL8E;case Cyc_CfFlowInfo_AllIL: _LL8E: {struct _tuple13 _tmp792;return(_tmp792.f1=
f,((_tmp792.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_all),
_tmp792)));}case Cyc_CfFlowInfo_ThisIL: _LL94: {struct _tuple13 _tmp793;return(
_tmp793.f1=f,((_tmp793.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,(env->fenv)->unknown_none),
_tmp793)));}}_LL5D:;}_LL5B:;_LL5C: {struct Cyc_Core_Impossible_struct _tmp799;
const char*_tmp798;struct Cyc_Core_Impossible_struct*_tmp797;(int)_throw((void*)((
_tmp797=_cycalloc(sizeof(*_tmp797)),((_tmp797[0]=((_tmp799.tag=Cyc_Core_Impossible,((
_tmp799.f1=((_tmp798="right deref of non-pointer-type",_tag_dyneither(_tmp798,
sizeof(char),32))),_tmp799)))),_tmp797)))));}_LL58:;}static struct Cyc_List_List*
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
_LLAF:;_LLB0: continue;_LLAC:;}}}if(!found){struct Cyc_CfFlowInfo_Reln*_tmp79C;
struct Cyc_List_List*_tmp79B;return(_tmp79B=_region_malloc(rgn,sizeof(*_tmp79B)),((
_tmp79B->hd=((_tmp79C=_region_malloc(rgn,sizeof(*_tmp79C)),((_tmp79C->vd=_tmp107,((
_tmp79C->rop=Cyc_CfFlowInfo_LessNumelts(_tmpF6),_tmp79C)))))),((_tmp79B->tl=
relns,_tmp79B)))));}}return relns;_LLAA:;_LLAB: return relns;_LLA1:;}else{return
relns;}_LL9F:;_LLA0: return relns;_LL96:;}static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc);static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo
_tmp10E=outflow;int _tmp10F;struct _tuple12 _tmp110;struct Cyc_Dict_Dict _tmp111;
struct Cyc_List_List*_tmp112;_LLB2: if((_tmp10E.BottomFL).tag != 1)goto _LLB4;
_tmp10F=(int)(_tmp10E.BottomFL).val;_LLB3: {struct _tuple13 _tmp79D;return(_tmp79D.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp79D.f2=rval,_tmp79D)));}_LLB4: if((_tmp10E.ReachableFL).tag
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
_tmp7A0;void*_tmp79F;(_tmp79F=0,Cyc_Tcutil_warn(lexp->loc,((_tmp7A0="assignment may overwrite unique pointer(s)",
_tag_dyneither(_tmp7A0,sizeof(char),43))),_tag_dyneither(_tmp79F,sizeof(void*),0)));}
goto _LLBB;_LLBB:;}Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple13
_tmp7A1;return(_tmp7A1.f1=outflow,((_tmp7A1.f2=rval,_tmp7A1)));};}_LLB9: if((
_tmp114.UnknownL).tag != 2)goto _LLB6;_tmp116=(int)(_tmp114.UnknownL).val;_LLBA: {
struct _tuple13 _tmp7A2;return(_tmp7A2.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,
outflow,rval),((_tmp7A2.f2=rval,_tmp7A2)));}_LLB6:;}_LLB1:;}static int Cyc_NewControlFlow_is_local_var_rooted_path(
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
_tmp14D=_tmp14C.f1;_tmp14E=_tmp14C.f2;{struct _tuple14 _tmp7A3;struct _tuple14
_tmp150=(_tmp7A3.f1=_tmp14D,((_tmp7A3.f2=_tmp14E,_tmp7A3)));union Cyc_CfFlowInfo_FlowInfo
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
struct Cyc_List_List _tmp7A4;struct Cyc_List_List _tmp15F=(_tmp7A4.hd=_tmp145,((
_tmp7A4.tl=0,_tmp7A4)));_tmp14D=Cyc_NewControlFlow_add_vars(_tmp137,_tmp14D,(
struct Cyc_List_List*)& _tmp15F,_tmp137->unknown_all,s->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp161;void*_tmp162;struct _tuple13 _tmp160=Cyc_NewControlFlow_anal_stmt_exp(env,
copy_ctxt,_tmp14D,_tmp146);_tmp161=_tmp160.f1;_tmp162=_tmp160.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp163=_tmp161;struct _tuple12 _tmp164;struct Cyc_Dict_Dict _tmp165;struct Cyc_List_List*
_tmp166;_LLF8: if((_tmp163.ReachableFL).tag != 2)goto _LLFA;_tmp164=(struct _tuple12)(
_tmp163.ReachableFL).val;_tmp165=_tmp164.f1;_tmp166=_tmp164.f2;_LLF9: _tmp165=Cyc_CfFlowInfo_assign_place(
_tmp137,s->loc,_tmp165,env->all_changed,_tmp156,_tmp15E);{union Cyc_CfFlowInfo_FlowInfo
_tmp167=Cyc_CfFlowInfo_ReachableFL(_tmp165,_tmp166);struct _tuple13 _tmp7A5;return(
_tmp7A5.f1=_tmp167,((_tmp7A5.f2=_tmp162,_tmp7A5)));};_LLFA:;_LLFB: {struct
_tuple13 _tmp7A6;return(_tmp7A6.f1=_tmp161,((_tmp7A6.f2=_tmp162,_tmp7A6)));}_LLF7:;};};};};};};}
_LLEA: _tmp157=_tmp150.f1;if((_tmp157.BottomFL).tag != 1)goto _LLEC;_tmp158=(int)(
_tmp157.BottomFL).val;_LLEB: {struct _tuple13 _tmp7A7;return(_tmp7A7.f1=_tmp14D,((
_tmp7A7.f2=_tmp137->unknown_all,_tmp7A7)));}_LLEC:;_LLED:{const char*_tmp7AA;void*
_tmp7A9;(_tmp7A9=0,Cyc_Tcutil_terr(_tmp143->loc,((_tmp7AA="bad alias expression--no unique path found",
_tag_dyneither(_tmp7AA,sizeof(char),43))),_tag_dyneither(_tmp7A9,sizeof(void*),0)));}{
struct _tuple13 _tmp7AB;return(_tmp7AB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7AB.f2=
_tmp137->unknown_all,_tmp7AB)));};_LLE7:;};}_LLE1: {struct Cyc_Absyn_Decl_s_struct*
_tmp147=(struct Cyc_Absyn_Decl_s_struct*)_tmp13A;if(_tmp147->tag != 12)goto _LLE3;
else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}_LLE2: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp148);s=_tmp149;goto _LLDC;_LLE3: {struct Cyc_Absyn_Exp_s_struct*
_tmp14A=(struct Cyc_Absyn_Exp_s_struct*)_tmp13A;if(_tmp14A->tag != 1)goto _LLE5;
else{_tmp14B=_tmp14A->f1;}}_LLE4: return Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp14B);_LLE5:;_LLE6: {struct Cyc_Core_Impossible_struct _tmp7B1;
const char*_tmp7B0;struct Cyc_Core_Impossible_struct*_tmp7AF;(int)_throw((void*)((
_tmp7AF=_cycalloc(sizeof(*_tmp7AF)),((_tmp7AF[0]=((_tmp7B1.tag=Cyc_Core_Impossible,((
_tmp7B1.f1=((_tmp7B0="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp7B0,
sizeof(char),33))),_tmp7B1)))),_tmp7AF)))));}_LLDC:;};}}struct _tuple20{union Cyc_CfFlowInfo_AbsLVal
f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple21{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);
static void _tmp844(unsigned int*_tmp843,unsigned int*_tmp842,void***_tmp840,
struct Cyc_List_List**_tmp34F){for(*_tmp843=0;*_tmp843 < *_tmp842;(*_tmp843)++){
void*_tmp83E;(*_tmp840)[*_tmp843]=((_tmp83E=(void*)((struct Cyc_List_List*)
_check_null(*_tmp34F))->hd,((*_tmp34F=(*_tmp34F)->tl,_tmp83E))));}}static struct
_tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp172=env->fenv;struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;{union Cyc_CfFlowInfo_FlowInfo
_tmp173=inflow;int _tmp174;struct _tuple12 _tmp175;struct Cyc_Dict_Dict _tmp176;
struct Cyc_List_List*_tmp177;_LLFD: if((_tmp173.BottomFL).tag != 1)goto _LLFF;
_tmp174=(int)(_tmp173.BottomFL).val;_LLFE: {struct _tuple13 _tmp7B2;return(_tmp7B2.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp7B2.f2=_tmp172->unknown_all,_tmp7B2)));}_LLFF:
if((_tmp173.ReachableFL).tag != 2)goto _LLFC;_tmp175=(struct _tuple12)(_tmp173.ReachableFL).val;
_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;_LL100: d=_tmp176;relns=_tmp177;_LLFC:;}if((
copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){union Cyc_CfFlowInfo_FlowInfo
_tmp17A;union Cyc_CfFlowInfo_AbsLVal _tmp17B;struct _tuple14 _tmp179=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,e);_tmp17A=_tmp179.f1;_tmp17B=_tmp179.f2;{struct _tuple14 _tmp7B3;
struct _tuple14 _tmp17D=(_tmp7B3.f1=_tmp17A,((_tmp7B3.f2=_tmp17B,_tmp7B3)));union
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
struct _tuple13 _tmp7B4;return(_tmp7B4.f1=_tmp17A,((_tmp7B4.f2=_tmp189,_tmp7B4)));}{
const char*_tmp7B7;void*_tmp7B6;(_tmp7B6=0,Cyc_Tcutil_terr(e->loc,((_tmp7B7="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp7B7,sizeof(char),71))),_tag_dyneither(_tmp7B6,sizeof(void*),0)));}{
struct _tuple13 _tmp7B8;return(_tmp7B8.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7B8.f2=
_tmp172->unknown_all,_tmp7B8)));};_LL109:;_LL10A: if(Cyc_CfFlowInfo_is_unique_consumed(
e,env->iteration_num,_tmp184)){{const char*_tmp7BB;void*_tmp7BA;(_tmp7BA=0,Cyc_Tcutil_terr(
e->loc,((_tmp7BB="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp7BB,sizeof(char),71))),_tag_dyneither(_tmp7BA,sizeof(void*),0)));}{
struct _tuple13 _tmp7BC;return(_tmp7BC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7BC.f2=
_tmp172->unknown_all,_tmp7BC)));};}goto _LL106;_LL106:;}{void*_tmp191=Cyc_CfFlowInfo_make_unique_consumed(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,e,env->iteration_num,
_tmp184);struct Cyc_Dict_Dict _tmp192=Cyc_CfFlowInfo_assign_place(_tmp172,e->loc,
_tmp180,env->all_changed,_tmp183,_tmp191);struct _tuple13 _tmp7BD;return(_tmp7BD.f1=
Cyc_CfFlowInfo_ReachableFL(_tmp192,_tmp181),((_tmp7BD.f2=_tmp184,_tmp7BD)));};}
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
_tuple13 _tmp7BE;return(_tmp7BE.f1=_tmp224,((_tmp7BE.f2=_tmp222,_tmp7BE)));};};}
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
_LL117: {struct _tuple13 _tmp7BF;return(_tmp7BF.f1=inflow,((_tmp7BF.f2=_tmp172->zero,
_tmp7BF)));}_LL118: {struct Cyc_Absyn_Const_e_struct*_tmp1A5=(struct Cyc_Absyn_Const_e_struct*)
_tmp194;if(_tmp1A5->tag != 0)goto _LL11A;else{_tmp1A6=_tmp1A5->f1;if((_tmp1A6.Int_c).tag
!= 4)goto _LL11A;_tmp1A7=(struct _tuple6)(_tmp1A6.Int_c).val;}}_LL119: goto _LL11B;
_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp1A8=(struct Cyc_Absyn_Var_e_struct*)
_tmp194;if(_tmp1A8->tag != 1)goto _LL11C;else{_tmp1A9=(void*)_tmp1A8->f2;{struct
Cyc_Absyn_Funname_b_struct*_tmp1AA=(struct Cyc_Absyn_Funname_b_struct*)_tmp1A9;
if(_tmp1AA->tag != 2)goto _LL11C;};}}_LL11B: {struct _tuple13 _tmp7C0;return(_tmp7C0.f1=
inflow,((_tmp7C0.f2=_tmp172->notzeroall,_tmp7C0)));}_LL11C: {struct Cyc_Absyn_Datatype_e_struct*
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
struct _tuple13 _tmp7C1;return(_tmp7C1.f1=inflow,((_tmp7C1.f2=_tmp172->unknown_all,
_tmp7C1)));}_LL12A: {struct Cyc_Absyn_Var_e_struct*_tmp1B3=(struct Cyc_Absyn_Var_e_struct*)
_tmp194;if(_tmp1B3->tag != 1)goto _LL12C;else{_tmp1B4=(void*)_tmp1B3->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp1B5=(struct Cyc_Absyn_Global_b_struct*)_tmp1B4;if(
_tmp1B5->tag != 1)goto _LL12C;};}}_LL12B: {struct _tuple13 _tmp7C2;return(_tmp7C2.f1=
inflow,((_tmp7C2.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp172->unknown_all),_tmp7C2)));}_LL12C: {struct Cyc_Absyn_Var_e_struct*
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
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp7C8;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp7C7;struct _tuple13 _tmp7C6;return(_tmp7C6.f1=inflow,((_tmp7C6.f2=((void*(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,(void*)((_tmp7C8=_region_malloc(
env->r,sizeof(*_tmp7C8)),((_tmp7C8[0]=((_tmp7C7.tag=0,((_tmp7C7.f1=_tmp1C1,
_tmp7C7)))),_tmp7C8))))),_tmp7C6)));}_LL132: {struct Cyc_Absyn_Primop_e_struct*
_tmp1C2=(struct Cyc_Absyn_Primop_e_struct*)_tmp194;if(_tmp1C2->tag != 3)goto _LL134;
else{_tmp1C3=_tmp1C2->f1;_tmp1C4=_tmp1C2->f2;}}_LL133: {union Cyc_CfFlowInfo_FlowInfo
_tmp22F;void*_tmp230;struct _tuple13 _tmp22E=Cyc_NewControlFlow_anal_use_ints(env,
inflow,_tmp1C4);_tmp22F=_tmp22E.f1;_tmp230=_tmp22E.f2;{struct _tuple13 _tmp7C9;
return(_tmp7C9.f1=_tmp22F,((_tmp7C9.f2=_tmp230,_tmp7C9)));};}_LL134: {struct Cyc_Absyn_Increment_e_struct*
_tmp1C5=(struct Cyc_Absyn_Increment_e_struct*)_tmp194;if(_tmp1C5->tag != 5)goto
_LL136;else{_tmp1C6=_tmp1C5->f1;}}_LL135: {struct Cyc_List_List _tmp7CA;struct Cyc_List_List
_tmp232=(_tmp7CA.hd=_tmp1C6,((_tmp7CA.tl=0,_tmp7CA)));union Cyc_CfFlowInfo_FlowInfo
_tmp234;struct _tuple13 _tmp233=Cyc_NewControlFlow_anal_use_ints(env,inflow,(
struct Cyc_List_List*)& _tmp232);_tmp234=_tmp233.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp236;struct _tuple14 _tmp235=Cyc_NewControlFlow_anal_Lexp(env,_tmp234,0,_tmp1C6);
_tmp236=_tmp235.f2;{struct _tuple20 _tmp7CB;struct _tuple20 _tmp238=(_tmp7CB.f1=
_tmp236,((_tmp7CB.f2=_tmp234,_tmp7CB)));union Cyc_CfFlowInfo_AbsLVal _tmp239;
struct Cyc_CfFlowInfo_Place*_tmp23A;union Cyc_CfFlowInfo_FlowInfo _tmp23B;struct
_tuple12 _tmp23C;struct Cyc_Dict_Dict _tmp23D;struct Cyc_List_List*_tmp23E;_LL175:
_tmp239=_tmp238.f1;if((_tmp239.PlaceL).tag != 1)goto _LL177;_tmp23A=(struct Cyc_CfFlowInfo_Place*)(
_tmp239.PlaceL).val;_tmp23B=_tmp238.f2;if((_tmp23B.ReachableFL).tag != 2)goto
_LL177;_tmp23C=(struct _tuple12)(_tmp23B.ReachableFL).val;_tmp23D=_tmp23C.f1;
_tmp23E=_tmp23C.f2;_LL176: _tmp23E=Cyc_CfFlowInfo_reln_kill_exp(_tmp172->r,
_tmp23E,_tmp1C6);_tmp234=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
_tmp172,_tmp1C6->loc,_tmp23D,env->all_changed,_tmp23A,_tmp172->unknown_all),
_tmp23E);goto _LL174;_LL177:;_LL178: goto _LL174;_LL174:;}{struct _tuple13 _tmp7CC;
return(_tmp7CC.f1=_tmp234,((_tmp7CC.f2=_tmp172->unknown_all,_tmp7CC)));};};}
_LL136: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1C7=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp194;if(_tmp1C7->tag != 4)goto _LL138;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C7->f2;
if(_tmp1C9 == 0)goto _LL138;_tmp1CA=*_tmp1C9;_tmp1CB=_tmp1C7->f3;}}_LL137: if(
copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){{const char*_tmp7CF;void*_tmp7CE;(_tmp7CE=0,Cyc_Tcutil_terr(
e->loc,((_tmp7CF="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7CF,sizeof(char),58))),_tag_dyneither(_tmp7CE,sizeof(void*),0)));}{
struct _tuple13 _tmp7D0;return(_tmp7D0.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7D0.f2=
_tmp172->unknown_all,_tmp7D0)));};}{struct Cyc_List_List _tmp7D1;struct Cyc_List_List
_tmp244=(_tmp7D1.hd=_tmp1CB,((_tmp7D1.tl=0,_tmp7D1)));struct Cyc_List_List _tmp7D2;
struct Cyc_List_List _tmp245=(_tmp7D2.hd=_tmp1C8,((_tmp7D2.tl=(struct Cyc_List_List*)&
_tmp244,_tmp7D2)));union Cyc_CfFlowInfo_FlowInfo _tmp247;struct _tuple13 _tmp246=Cyc_NewControlFlow_anal_use_ints(
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
_tuple13 _tmp7D3;return(_tmp7D3.f1=_tmp247,((_tmp7D3.f2=_tmp172->unknown_all,
_tmp7D3)));};};};_LL138: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1CC=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp194;if(_tmp1CC->tag != 4)goto _LL13A;else{_tmp1CD=_tmp1CC->f1;_tmp1CE=_tmp1CC->f2;
if(_tmp1CE != 0)goto _LL13A;_tmp1CF=_tmp1CC->f3;}}_LL139: if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){{const char*
_tmp7D6;void*_tmp7D5;(_tmp7D5=0,Cyc_Tcutil_terr(e->loc,((_tmp7D6="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7D6,sizeof(char),58))),_tag_dyneither(_tmp7D5,sizeof(void*),0)));}{
struct _tuple13 _tmp7D7;return(_tmp7D7.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7D7.f2=
_tmp172->unknown_all,_tmp7D7)));};}{struct Cyc_Dict_Dict*_tmp257=env->all_changed;
while(1){{struct Cyc_Dict_Dict*_tmp7D8;env->all_changed=((_tmp7D8=_region_malloc(
env->r,sizeof(*_tmp7D8)),((_tmp7D8[0]=_tmp172->mt_place_set,_tmp7D8))));}{union
Cyc_CfFlowInfo_FlowInfo _tmp25A;union Cyc_CfFlowInfo_AbsLVal _tmp25B;struct _tuple14
_tmp259=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1CD);_tmp25A=_tmp259.f1;
_tmp25B=_tmp259.f2;{struct Cyc_Dict_Dict _tmp25C=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp7D9;env->all_changed=((
_tmp7D9=_region_malloc(env->r,sizeof(*_tmp7D9)),((_tmp7D9[0]=_tmp172->mt_place_set,
_tmp7D9))));}{union Cyc_CfFlowInfo_FlowInfo _tmp25F;void*_tmp260;struct _tuple13
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
env,_tmp1D4->loc,_tmp268,_tmp269);{struct _tuple13 _tmp7DA;return(_tmp7DA.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp7DA.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp172->unknown_all),_tmp7DA)));};}_LL13E: {struct Cyc_Absyn_FnCall_e_struct*
_tmp1D5=(struct Cyc_Absyn_FnCall_e_struct*)_tmp194;if(_tmp1D5->tag != 11)goto
_LL140;else{_tmp1D6=_tmp1D5->f1;_tmp1D7=_tmp1D5->f2;}}_LL13F: {struct
_RegionHandle*_tmp26B=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp26E;struct Cyc_List_List*
_tmp26F;struct Cyc_List_List*_tmp7DB;struct _tuple19 _tmp26D=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp26B,env,inflow,((_tmp7DB=_region_malloc(_tmp26B,sizeof(*_tmp7DB)),((_tmp7DB->hd=
_tmp1D6,((_tmp7DB->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(_tmp26B,_tmp1D7),_tmp7DB)))))),0,1);_tmp26E=_tmp26D.f1;
_tmp26F=_tmp26D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp270=Cyc_NewControlFlow_use_Rval(
env,_tmp1D6->loc,_tmp26E,(void*)((struct Cyc_List_List*)_check_null(_tmp26F))->hd);
_tmp26F=_tmp26F->tl;{struct Cyc_List_List*init_params=0;struct Cyc_List_List*
nolive_unique_params=0;{void*_tmp271=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1D6->topt))->v);struct Cyc_Absyn_PtrInfo _tmp273;void*_tmp274;
_LL184: {struct Cyc_Absyn_PointerType_struct*_tmp272=(struct Cyc_Absyn_PointerType_struct*)
_tmp271;if(_tmp272->tag != 5)goto _LL186;else{_tmp273=_tmp272->f1;_tmp274=_tmp273.elt_typ;}}
_LL185:{void*_tmp275=Cyc_Tcutil_compress(_tmp274);struct Cyc_Absyn_FnInfo _tmp277;
struct Cyc_List_List*_tmp278;_LL189: {struct Cyc_Absyn_FnType_struct*_tmp276=(
struct Cyc_Absyn_FnType_struct*)_tmp275;if(_tmp276->tag != 10)goto _LL18B;else{
_tmp277=_tmp276->f1;_tmp278=_tmp277.attributes;}}_LL18A: for(0;_tmp278 != 0;
_tmp278=_tmp278->tl){void*_tmp279=(void*)_tmp278->hd;int _tmp27B;int _tmp27D;
_LL18E: {struct Cyc_Absyn_Initializes_att_struct*_tmp27A=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp279;if(_tmp27A->tag != 20)goto _LL190;else{_tmp27B=_tmp27A->f1;}}_LL18F:{
struct Cyc_List_List*_tmp7DC;init_params=((_tmp7DC=_region_malloc(_tmp26B,sizeof(*
_tmp7DC)),((_tmp7DC->hd=(void*)_tmp27B,((_tmp7DC->tl=init_params,_tmp7DC))))));}
goto _LL18D;_LL190: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp27C=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp279;if(_tmp27C->tag != 21)goto _LL192;else{_tmp27D=_tmp27C->f1;}}_LL191:{
struct Cyc_List_List*_tmp7DD;nolive_unique_params=((_tmp7DD=_region_malloc(
_tmp26B,sizeof(*_tmp7DD)),((_tmp7DD->hd=(void*)_tmp27D,((_tmp7DD->tl=
nolive_unique_params,_tmp7DD))))));}goto _LL18D;_LL192:;_LL193: goto _LL18D;_LL18D:;}
goto _LL188;_LL18B:;_LL18C: {const char*_tmp7E0;void*_tmp7DF;(_tmp7DF=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7E0="anal_Rexp: bad function type",
_tag_dyneither(_tmp7E0,sizeof(char),29))),_tag_dyneither(_tmp7DF,sizeof(void*),0)));}
_LL188:;}goto _LL183;_LL186:;_LL187: {const char*_tmp7E3;void*_tmp7E2;(_tmp7E2=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp7E3="anal_Rexp: bad function type",_tag_dyneither(_tmp7E3,sizeof(char),29))),
_tag_dyneither(_tmp7E2,sizeof(void*),0)));}_LL183:;}{int i=1;for(0;_tmp26F != 0;(((
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
== Cyc_CfFlowInfo_NoneIL){const char*_tmp7E6;void*_tmp7E5;(_tmp7E5=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1D7))->hd)->loc,((
_tmp7E6="expression may not be initialized",_tag_dyneither(_tmp7E6,sizeof(char),
34))),_tag_dyneither(_tmp7E5,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo
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
_tmp172,_tmp296,_tmp172->esc_all));goto _LL1A3;_LL1A6:;_LL1A7: {const char*_tmp7E9;
void*_tmp7E8;(_tmp7E8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp7E9="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp7E9,sizeof(char),39))),_tag_dyneither(_tmp7E8,sizeof(void*),0)));}
_LL1A3:;}goto _LL19E;_LL1A1:;_LL1A2: goto _LL19E;_LL19E:;}_tmp270=Cyc_CfFlowInfo_ReachableFL(
_tmp28F,_tmp28E);goto _LL199;}_LL199:;}goto _LL194;_LL194:;};}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1D6->topt))->v)){struct _tuple13
_tmp7EA;return(_tmp7EA.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7EA.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp172->unknown_all),
_tmp7EA)));}else{struct _tuple13 _tmp7EB;return(_tmp7EB.f1=_tmp270,((_tmp7EB.f2=
Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp172->unknown_all),_tmp7EB)));}};};}_LL140: {struct Cyc_Absyn_Malloc_e_struct*
_tmp1D8=(struct Cyc_Absyn_Malloc_e_struct*)_tmp194;if(_tmp1D8->tag != 34)goto
_LL142;else{_tmp1D9=_tmp1D8->f1;_tmp1DA=_tmp1D9.is_calloc;_tmp1DB=_tmp1D9.rgn;
_tmp1DC=_tmp1D9.elt_type;_tmp1DD=_tmp1D9.num_elts;_tmp1DE=_tmp1D9.fat_result;}}
_LL141: {struct Cyc_CfFlowInfo_MallocPt_struct _tmp7EE;struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp7ED;void*root=(void*)((_tmp7ED=_region_malloc(_tmp172->r,sizeof(*_tmp7ED)),((
_tmp7ED[0]=((_tmp7EE.tag=1,((_tmp7EE.f1=_tmp1DD,((_tmp7EE.f2=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp7EE)))))),_tmp7ED))));struct
Cyc_CfFlowInfo_Place*_tmp7EF;struct Cyc_CfFlowInfo_Place*place=(_tmp7EF=
_region_malloc(_tmp172->r,sizeof(*_tmp7EF)),((_tmp7EF->root=root,((_tmp7EF->fields=
0,_tmp7EF)))));struct Cyc_CfFlowInfo_AddressOf_struct _tmp7F2;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp7F1;void*rval=(void*)((_tmp7F1=_region_malloc(_tmp172->r,sizeof(*_tmp7F1)),((
_tmp7F1[0]=((_tmp7F2.tag=5,((_tmp7F2.f1=place,_tmp7F2)))),_tmp7F1))));void*
place_val;if(_tmp1DE)place_val=_tmp172->notzeroall;else{if(_tmp1DA)place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,*((void**)_check_null(_tmp1DC)),_tmp172->zero);else{place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,*((void**)_check_null(_tmp1DC)),_tmp172->unknown_none);}}{union Cyc_CfFlowInfo_FlowInfo
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);if(_tmp1DB != 0){struct _RegionHandle*_tmp29B=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp29E;struct Cyc_List_List*_tmp29F;struct Cyc_Absyn_Exp*_tmp7F3[2];struct
_tuple19 _tmp29D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp29B,env,inflow,((
_tmp7F3[1]=_tmp1DD,((_tmp7F3[0]=(struct Cyc_Absyn_Exp*)_tmp1DB,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp29B,
_tag_dyneither(_tmp7F3,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp29E=_tmp29D.f1;
_tmp29F=_tmp29D.f2;outflow=_tmp29E;}else{union Cyc_CfFlowInfo_FlowInfo _tmp2A1;
struct _tuple13 _tmp2A0=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1DD);_tmp2A1=
_tmp2A0.f1;outflow=_tmp2A1;}{union Cyc_CfFlowInfo_FlowInfo _tmp2A2=outflow;int
_tmp2A3;struct _tuple12 _tmp2A4;struct Cyc_Dict_Dict _tmp2A5;struct Cyc_List_List*
_tmp2A6;_LL1A9: if((_tmp2A2.BottomFL).tag != 1)goto _LL1AB;_tmp2A3=(int)(_tmp2A2.BottomFL).val;
_LL1AA: {struct _tuple13 _tmp7F4;return(_tmp7F4.f1=outflow,((_tmp7F4.f2=rval,
_tmp7F4)));}_LL1AB: if((_tmp2A2.ReachableFL).tag != 2)goto _LL1A8;_tmp2A4=(struct
_tuple12)(_tmp2A2.ReachableFL).val;_tmp2A5=_tmp2A4.f1;_tmp2A6=_tmp2A4.f2;_LL1AC: {
struct _tuple13 _tmp7F5;return(_tmp7F5.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2A5,root,place_val),
_tmp2A6),((_tmp7F5.f2=rval,_tmp7F5)));}_LL1A8:;};};}_LL142: {struct Cyc_Absyn_Swap_e_struct*
_tmp1DF=(struct Cyc_Absyn_Swap_e_struct*)_tmp194;if(_tmp1DF->tag != 35)goto _LL144;
else{_tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}}_LL143: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle*_tmp2AE=env->r;{
union Cyc_CfFlowInfo_FlowInfo _tmp2B1;struct Cyc_List_List*_tmp2B2;struct Cyc_Absyn_Exp*
_tmp7F6[2];struct _tuple19 _tmp2B0=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2AE,
env,inflow,((_tmp7F6[1]=_tmp1E1,((_tmp7F6[0]=_tmp1E0,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2AE,
_tag_dyneither(_tmp7F6,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp2B1=_tmp2B0.f1;
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
_LL1B0:;_LL1B1: goto _LL1AD;_LL1AD:;}{struct _tuple13 _tmp7F7;return(_tmp7F7.f1=
outflow,((_tmp7F7.f2=_tmp172->unknown_all,_tmp7F7)));};};};}_LL144: {struct Cyc_Absyn_New_e_struct*
_tmp1E2=(struct Cyc_Absyn_New_e_struct*)_tmp194;if(_tmp1E2->tag != 17)goto _LL146;
else{_tmp1E3=_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;}}_LL145: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp7FA;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp7F9;void*root=(void*)((_tmp7F9=
_region_malloc(_tmp172->r,sizeof(*_tmp7F9)),((_tmp7F9[0]=((_tmp7FA.tag=1,((
_tmp7FA.f1=_tmp1E4,((_tmp7FA.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp7FA)))))),_tmp7F9))));struct Cyc_CfFlowInfo_Place*_tmp7FB;
struct Cyc_CfFlowInfo_Place*place=(_tmp7FB=_region_malloc(_tmp172->r,sizeof(*
_tmp7FB)),((_tmp7FB->root=root,((_tmp7FB->fields=0,_tmp7FB)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp7FE;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp7FD;void*rval=(void*)((_tmp7FD=
_region_malloc(_tmp172->r,sizeof(*_tmp7FD)),((_tmp7FD[0]=((_tmp7FE.tag=5,((
_tmp7FE.f1=place,_tmp7FE)))),_tmp7FD))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;
if(_tmp1E3 != 0){struct _RegionHandle*_tmp2C2=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp2C5;struct Cyc_List_List*_tmp2C6;struct Cyc_Absyn_Exp*_tmp7FF[2];struct
_tuple19 _tmp2C4=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2C2,env,inflow,((
_tmp7FF[1]=_tmp1E4,((_tmp7FF[0]=(struct Cyc_Absyn_Exp*)_tmp1E3,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2C2,
_tag_dyneither(_tmp7FF,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp2C5=_tmp2C4.f1;
_tmp2C6=_tmp2C4.f2;outflow=_tmp2C5;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2C6))->tl))->hd;}else{union
Cyc_CfFlowInfo_FlowInfo _tmp2C8;void*_tmp2C9;struct _tuple13 _tmp2C7=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1E4);_tmp2C8=_tmp2C7.f1;_tmp2C9=_tmp2C7.f2;outflow=_tmp2C8;
place_val=_tmp2C9;}{union Cyc_CfFlowInfo_FlowInfo _tmp2CA=outflow;int _tmp2CB;
struct _tuple12 _tmp2CC;struct Cyc_Dict_Dict _tmp2CD;struct Cyc_List_List*_tmp2CE;
_LL1BD: if((_tmp2CA.BottomFL).tag != 1)goto _LL1BF;_tmp2CB=(int)(_tmp2CA.BottomFL).val;
_LL1BE: {struct _tuple13 _tmp800;return(_tmp800.f1=outflow,((_tmp800.f2=rval,
_tmp800)));}_LL1BF: if((_tmp2CA.ReachableFL).tag != 2)goto _LL1BC;_tmp2CC=(struct
_tuple12)(_tmp2CA.ReachableFL).val;_tmp2CD=_tmp2CC.f1;_tmp2CE=_tmp2CC.f2;_LL1C0: {
struct _tuple13 _tmp801;return(_tmp801.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2CD,root,place_val),
_tmp2CE),((_tmp801.f2=rval,_tmp801)));}_LL1BC:;};};}_LL146: {struct Cyc_Absyn_Address_e_struct*
_tmp1E5=(struct Cyc_Absyn_Address_e_struct*)_tmp194;if(_tmp1E5->tag != 16)goto
_LL148;else{_tmp1E6=_tmp1E5->f1;}}_LL147: {union Cyc_CfFlowInfo_FlowInfo _tmp2D7;
union Cyc_CfFlowInfo_AbsLVal _tmp2D8;struct _tuple14 _tmp2D6=Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,_tmp1E6);_tmp2D7=_tmp2D6.f1;_tmp2D8=_tmp2D6.f2;{union Cyc_CfFlowInfo_AbsLVal
_tmp2D9=_tmp2D8;int _tmp2DA;struct Cyc_CfFlowInfo_Place*_tmp2DB;_LL1C2: if((_tmp2D9.UnknownL).tag
!= 2)goto _LL1C4;_tmp2DA=(int)(_tmp2D9.UnknownL).val;_LL1C3: {struct _tuple13
_tmp802;return(_tmp802.f1=_tmp2D7,((_tmp802.f2=_tmp172->notzeroall,_tmp802)));}
_LL1C4: if((_tmp2D9.PlaceL).tag != 1)goto _LL1C1;_tmp2DB=(struct Cyc_CfFlowInfo_Place*)(
_tmp2D9.PlaceL).val;_LL1C5: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp808;
struct Cyc_CfFlowInfo_AddressOf_struct _tmp807;struct _tuple13 _tmp806;return(
_tmp806.f1=_tmp2D7,((_tmp806.f2=(void*)((_tmp808=_region_malloc(env->r,sizeof(*
_tmp808)),((_tmp808[0]=((_tmp807.tag=5,((_tmp807.f1=_tmp2DB,_tmp807)))),_tmp808)))),
_tmp806)));}_LL1C1:;};}_LL148: {struct Cyc_Absyn_Deref_e_struct*_tmp1E7=(struct
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
_tmp809;return(_tmp809.f1=_tmp2E4,((_tmp809.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp172->unknown_all),
_tmp809)));}{void*_tmp2E7=_tmp2E5;int _tmp2E9;struct _dyneither_ptr _tmp2EA;_LL1C7: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2E8=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2E7;if(_tmp2E8->tag != 6)goto _LL1C9;else{_tmp2E9=_tmp2E8->f1;_tmp2EA=_tmp2E8->f2;}}
_LL1C8: {int _tmp2EB=Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1EA->topt))->v,_tmp1EB);struct _tuple13 _tmp80A;return(_tmp80A.f1=
_tmp2E4,((_tmp80A.f2=*((void**)_check_dyneither_subscript(_tmp2EA,sizeof(void*),
_tmp2EB)),_tmp80A)));}_LL1C9:;_LL1CA: {struct Cyc_Core_Impossible_struct _tmp817;
const char*_tmp816;void*_tmp815[1];struct Cyc_String_pa_struct _tmp814;struct Cyc_Core_Impossible_struct*
_tmp813;(int)_throw((void*)((_tmp813=_cycalloc(sizeof(*_tmp813)),((_tmp813[0]=((
_tmp817.tag=Cyc_Core_Impossible,((_tmp817.f1=(struct _dyneither_ptr)((_tmp814.tag=
0,((_tmp814.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp815[0]=& _tmp814,Cyc_aprintf(((_tmp816="anal_Rexp: AggrMember: %s",
_tag_dyneither(_tmp816,sizeof(char),26))),_tag_dyneither(_tmp815,sizeof(void*),1)))))))),
_tmp817)))),_tmp813)))));}_LL1C6:;};}_LL14C: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1EC=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp194;if(_tmp1EC->tag != 38)goto
_LL14E;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=_tmp1EC->f2;}}_LL14D: {union Cyc_CfFlowInfo_FlowInfo
_tmp2F3;void*_tmp2F4;struct _tuple13 _tmp2F2=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1ED);_tmp2F3=_tmp2F2.f1;_tmp2F4=_tmp2F2.f2;{struct _tuple13 _tmp818;
return(_tmp818.f1=_tmp2F3,((_tmp818.f2=_tmp172->unknown_all,_tmp818)));};}_LL14E: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp1EF=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp194;if(_tmp1EF->tag != 23)goto _LL150;else{_tmp1F0=_tmp1EF->f1;_tmp1F1=_tmp1EF->f2;}}
_LL14F: {union Cyc_CfFlowInfo_FlowInfo _tmp2F7;void*_tmp2F8;struct _tuple13 _tmp2F6=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1F0);_tmp2F7=_tmp2F6.f1;_tmp2F8=
_tmp2F6.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2FA;void*_tmp2FB;struct _tuple13
_tmp2F9=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2F7,_tmp1F0,_tmp2F8);
_tmp2FA=_tmp2F9.f1;_tmp2FB=_tmp2F9.f2;{void*_tmp2FC=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1F0->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp2FE;void*_tmp2FF;_LL1CC: {struct Cyc_Absyn_PointerType_struct*_tmp2FD=(struct
Cyc_Absyn_PointerType_struct*)_tmp2FC;if(_tmp2FD->tag != 5)goto _LL1CE;else{
_tmp2FE=_tmp2FD->f1;_tmp2FF=_tmp2FE.elt_typ;}}_LL1CD: if(Cyc_Absyn_is_nontagged_union_type(
_tmp2FF)){struct _tuple13 _tmp819;return(_tmp819.f1=_tmp2FA,((_tmp819.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp172->unknown_all),
_tmp819)));}{void*_tmp301=_tmp2FB;int _tmp303;struct _dyneither_ptr _tmp304;_LL1D1: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp302=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp301;if(_tmp302->tag != 6)goto _LL1D3;else{_tmp303=_tmp302->f1;_tmp304=_tmp302->f2;}}
_LL1D2: {int _tmp305=Cyc_CfFlowInfo_get_field_index(_tmp2FF,_tmp1F1);struct
_tuple13 _tmp81A;return(_tmp81A.f1=_tmp2FA,((_tmp81A.f2=*((void**)
_check_dyneither_subscript(_tmp304,sizeof(void*),_tmp305)),_tmp81A)));}_LL1D3:;
_LL1D4: {struct Cyc_Core_Impossible_struct _tmp820;const char*_tmp81F;struct Cyc_Core_Impossible_struct*
_tmp81E;(int)_throw((void*)((_tmp81E=_cycalloc(sizeof(*_tmp81E)),((_tmp81E[0]=((
_tmp820.tag=Cyc_Core_Impossible,((_tmp820.f1=((_tmp81F="anal_Rexp: AggrArrow",
_tag_dyneither(_tmp81F,sizeof(char),21))),_tmp820)))),_tmp81E)))));}_LL1D0:;};
_LL1CE:;_LL1CF: {struct Cyc_Core_Impossible_struct _tmp826;const char*_tmp825;
struct Cyc_Core_Impossible_struct*_tmp824;(int)_throw((void*)((_tmp824=_cycalloc(
sizeof(*_tmp824)),((_tmp824[0]=((_tmp826.tag=Cyc_Core_Impossible,((_tmp826.f1=((
_tmp825="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp825,sizeof(char),25))),
_tmp826)))),_tmp824)))));}_LL1CB:;};};}_LL150: {struct Cyc_Absyn_Conditional_e_struct*
_tmp1F2=(struct Cyc_Absyn_Conditional_e_struct*)_tmp194;if(_tmp1F2->tag != 6)goto
_LL152;else{_tmp1F3=_tmp1F2->f1;_tmp1F4=_tmp1F2->f2;_tmp1F5=_tmp1F2->f3;}}_LL151: {
union Cyc_CfFlowInfo_FlowInfo _tmp30E;union Cyc_CfFlowInfo_FlowInfo _tmp30F;struct
_tuple15 _tmp30D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1F3);_tmp30E=_tmp30D.f1;
_tmp30F=_tmp30D.f2;{struct _tuple13 _tmp310=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp30E,_tmp1F4);struct _tuple13 _tmp311=Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,_tmp30F,_tmp1F5);return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,
env->all_changed,_tmp310,_tmp311);};}_LL152: {struct Cyc_Absyn_And_e_struct*
_tmp1F6=(struct Cyc_Absyn_And_e_struct*)_tmp194;if(_tmp1F6->tag != 7)goto _LL154;
else{_tmp1F7=_tmp1F6->f1;_tmp1F8=_tmp1F6->f2;}}_LL153: {union Cyc_CfFlowInfo_FlowInfo
_tmp313;union Cyc_CfFlowInfo_FlowInfo _tmp314;struct _tuple15 _tmp312=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1F7);_tmp313=_tmp312.f1;_tmp314=_tmp312.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp316;void*_tmp317;struct _tuple13 _tmp315=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp313,_tmp1F8);_tmp316=_tmp315.f1;_tmp317=_tmp315.f2;{struct _tuple13
_tmp827;struct _tuple13 _tmp318=(_tmp827.f1=_tmp316,((_tmp827.f2=_tmp317,_tmp827)));
struct _tuple13 _tmp828;struct _tuple13 _tmp319=(_tmp828.f1=_tmp314,((_tmp828.f2=
_tmp172->zero,_tmp828)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,env->all_changed,
_tmp318,_tmp319);};};}_LL154: {struct Cyc_Absyn_Or_e_struct*_tmp1F9=(struct Cyc_Absyn_Or_e_struct*)
_tmp194;if(_tmp1F9->tag != 8)goto _LL156;else{_tmp1FA=_tmp1F9->f1;_tmp1FB=_tmp1F9->f2;}}
_LL155: {union Cyc_CfFlowInfo_FlowInfo _tmp31D;union Cyc_CfFlowInfo_FlowInfo _tmp31E;
struct _tuple15 _tmp31C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1FA);_tmp31D=
_tmp31C.f1;_tmp31E=_tmp31C.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp320;void*_tmp321;
struct _tuple13 _tmp31F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp31E,_tmp1FB);
_tmp320=_tmp31F.f1;_tmp321=_tmp31F.f2;{struct _tuple13 _tmp829;struct _tuple13
_tmp322=(_tmp829.f1=_tmp320,((_tmp829.f2=_tmp321,_tmp829)));struct _tuple13
_tmp82A;struct _tuple13 _tmp323=(_tmp82A.f1=_tmp31D,((_tmp82A.f2=_tmp172->notzeroall,
_tmp82A)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,env->all_changed,
_tmp322,_tmp323);};};}_LL156: {struct Cyc_Absyn_Subscript_e_struct*_tmp1FC=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp194;if(_tmp1FC->tag != 24)goto _LL158;
else{_tmp1FD=_tmp1FC->f1;_tmp1FE=_tmp1FC->f2;}}_LL157: {struct _RegionHandle*
_tmp326=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp329;struct Cyc_List_List*_tmp32A;
struct Cyc_Absyn_Exp*_tmp82B[2];struct _tuple19 _tmp328=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp326,env,inflow,((_tmp82B[1]=_tmp1FE,((_tmp82B[0]=_tmp1FD,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp326,
_tag_dyneither(_tmp82B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp329=_tmp328.f1;
_tmp32A=_tmp328.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp32B=_tmp329;{union Cyc_CfFlowInfo_FlowInfo
_tmp32C=_tmp329;struct _tuple12 _tmp32D;struct Cyc_Dict_Dict _tmp32E;struct Cyc_List_List*
_tmp32F;_LL1D6: if((_tmp32C.ReachableFL).tag != 2)goto _LL1D8;_tmp32D=(struct
_tuple12)(_tmp32C.ReachableFL).val;_tmp32E=_tmp32D.f1;_tmp32F=_tmp32D.f2;_LL1D7:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp32E,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp32A))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp82E;void*_tmp82D;(_tmp82D=0,Cyc_Tcutil_terr(_tmp1FE->loc,((_tmp82E="expression may not be initialized",
_tag_dyneither(_tmp82E,sizeof(char),34))),_tag_dyneither(_tmp82D,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp332=Cyc_NewControlFlow_add_subscript_reln(_tmp172->r,
_tmp32F,_tmp1FD,_tmp1FE);if(_tmp32F != _tmp332)_tmp32B=Cyc_CfFlowInfo_ReachableFL(
_tmp32E,_tmp332);goto _LL1D5;};_LL1D8:;_LL1D9: goto _LL1D5;_LL1D5:;}{void*_tmp333=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1FD->topt))->v);
struct Cyc_List_List*_tmp335;struct Cyc_Absyn_PtrInfo _tmp337;struct Cyc_Absyn_PtrAtts
_tmp338;union Cyc_Absyn_Constraint*_tmp339;_LL1DB: {struct Cyc_Absyn_TupleType_struct*
_tmp334=(struct Cyc_Absyn_TupleType_struct*)_tmp333;if(_tmp334->tag != 11)goto
_LL1DD;else{_tmp335=_tmp334->f1;}}_LL1DC: {void*_tmp33A=(void*)((struct Cyc_List_List*)
_check_null(_tmp32A))->hd;int _tmp33C;struct _dyneither_ptr _tmp33D;_LL1E2: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp33B=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp33A;if(_tmp33B->tag != 6)goto _LL1E4;else{_tmp33C=_tmp33B->f1;_tmp33D=_tmp33B->f2;}}
_LL1E3: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1FE)).f1;struct
_tuple13 _tmp82F;return(_tmp82F.f1=_tmp32B,((_tmp82F.f2=*((void**)
_check_dyneither_subscript(_tmp33D,sizeof(void*),(int)i)),_tmp82F)));}_LL1E4:;
_LL1E5: {struct Cyc_Core_Impossible_struct _tmp835;const char*_tmp834;struct Cyc_Core_Impossible_struct*
_tmp833;(int)_throw((void*)((_tmp833=_cycalloc(sizeof(*_tmp833)),((_tmp833[0]=((
_tmp835.tag=Cyc_Core_Impossible,((_tmp835.f1=((_tmp834="anal_Rexp: Subscript",
_tag_dyneither(_tmp834,sizeof(char),21))),_tmp835)))),_tmp833)))));}_LL1E1:;}
_LL1DD: {struct Cyc_Absyn_PointerType_struct*_tmp336=(struct Cyc_Absyn_PointerType_struct*)
_tmp333;if(_tmp336->tag != 5)goto _LL1DF;else{_tmp337=_tmp336->f1;_tmp338=_tmp337.ptr_atts;
_tmp339=_tmp338.bounds;}}_LL1DE: {union Cyc_CfFlowInfo_FlowInfo _tmp343;void*
_tmp344;struct _tuple13 _tmp342=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp329,
_tmp1FD,(void*)((struct Cyc_List_List*)_check_null(_tmp32A))->hd);_tmp343=_tmp342.f1;
_tmp344=_tmp342.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp345=_tmp343;int _tmp346;
_LL1E7: if((_tmp345.BottomFL).tag != 1)goto _LL1E9;_tmp346=(int)(_tmp345.BottomFL).val;
_LL1E8: {struct _tuple13 _tmp836;return(_tmp836.f1=_tmp343,((_tmp836.f2=_tmp344,
_tmp836)));}_LL1E9:;_LL1EA: {struct _tuple13 _tmp837;return(_tmp837.f1=_tmp32B,((
_tmp837.f2=_tmp344,_tmp837)));}_LL1E6:;};}_LL1DF:;_LL1E0: {struct Cyc_Core_Impossible_struct
_tmp83D;const char*_tmp83C;struct Cyc_Core_Impossible_struct*_tmp83B;(int)_throw((
void*)((_tmp83B=_cycalloc(sizeof(*_tmp83B)),((_tmp83B[0]=((_tmp83D.tag=Cyc_Core_Impossible,((
_tmp83D.f1=((_tmp83C="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp83C,
sizeof(char),33))),_tmp83D)))),_tmp83B)))));}_LL1DA:;};};}_LL158: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1FF=(struct Cyc_Absyn_Datatype_e_struct*)_tmp194;if(_tmp1FF->tag != 31)goto
_LL15A;else{_tmp200=_tmp1FF->f1;_tmp201=_tmp1FF->f2;}}_LL159: _tmp203=_tmp200;
goto _LL15B;_LL15A: {struct Cyc_Absyn_Tuple_e_struct*_tmp202=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp194;if(_tmp202->tag != 25)goto _LL15C;else{_tmp203=_tmp202->f1;}}_LL15B: {
struct _RegionHandle*_tmp34C=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp34E;struct Cyc_List_List*
_tmp34F;struct _tuple19 _tmp34D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp34C,
env,inflow,_tmp203,1,1);_tmp34E=_tmp34D.f1;_tmp34F=_tmp34D.f2;{unsigned int
_tmp843;unsigned int _tmp842;struct _dyneither_ptr _tmp841;void**_tmp840;
unsigned int _tmp83F;struct _dyneither_ptr aggrdict=(_tmp83F=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp203),((_tmp840=(void**)
_region_malloc(env->r,_check_times(sizeof(void*),_tmp83F)),((_tmp841=
_tag_dyneither(_tmp840,sizeof(void*),_tmp83F),((((_tmp842=_tmp83F,_tmp844(&
_tmp843,& _tmp842,& _tmp840,& _tmp34F))),_tmp841)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp84A;struct Cyc_CfFlowInfo_Aggregate_struct _tmp849;struct _tuple13 _tmp848;
return(_tmp848.f1=_tmp34E,((_tmp848.f2=(void*)((_tmp84A=_region_malloc(env->r,
sizeof(*_tmp84A)),((_tmp84A[0]=((_tmp849.tag=6,((_tmp849.f1=0,((_tmp849.f2=
aggrdict,_tmp849)))))),_tmp84A)))),_tmp848)));};}_LL15C: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp204=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp194;if(_tmp204->tag != 30)goto
_LL15E;else{_tmp205=_tmp204->f2;}}_LL15D: {struct Cyc_List_List*fs;{void*_tmp358=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_List_List*_tmp35A;_LL1EC: {struct Cyc_Absyn_AnonAggrType_struct*_tmp359=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp358;if(_tmp359->tag != 13)goto _LL1EE;
else{_tmp35A=_tmp359->f2;}}_LL1ED: fs=_tmp35A;goto _LL1EB;_LL1EE:;_LL1EF: {struct
Cyc_Core_Impossible_struct _tmp850;const char*_tmp84F;struct Cyc_Core_Impossible_struct*
_tmp84E;(int)_throw((void*)((_tmp84E=_cycalloc(sizeof(*_tmp84E)),((_tmp84E[0]=((
_tmp850.tag=Cyc_Core_Impossible,((_tmp850.f1=((_tmp84F="anal_Rexp:anon struct has bad type",
_tag_dyneither(_tmp84F,sizeof(char),35))),_tmp850)))),_tmp84E)))));}_LL1EB:;}
_tmp207=_tmp205;_tmp20A=Cyc_Absyn_StructA;_tmp20D=fs;goto _LL15F;}_LL15E: {struct
Cyc_Absyn_Aggregate_e_struct*_tmp206=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp194;if(_tmp206->tag != 29)goto _LL160;else{_tmp207=_tmp206->f3;_tmp208=_tmp206->f4;
if(_tmp208 == 0)goto _LL160;_tmp209=*_tmp208;_tmp20A=_tmp209.kind;_tmp20B=_tmp209.impl;
if(_tmp20B == 0)goto _LL160;_tmp20C=*_tmp20B;_tmp20D=_tmp20C.fields;}}_LL15F: {
struct _RegionHandle*_tmp35E=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp360;struct Cyc_List_List*
_tmp361;struct _tuple19 _tmp35F=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp35E,
env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp35E,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp207),1,1);_tmp360=_tmp35F.f1;_tmp361=_tmp35F.f2;{
struct _dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp172,
_tmp20D,_tmp172->unknown_all);{int i=0;for(0;_tmp361 != 0;(((_tmp361=_tmp361->tl,
_tmp207=_tmp207->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple21*)((struct
Cyc_List_List*)_check_null(_tmp207))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp362=(void*)ds->hd;struct _dyneither_ptr*_tmp365;_LL1F1: {struct Cyc_Absyn_ArrayElement_struct*
_tmp363=(struct Cyc_Absyn_ArrayElement_struct*)_tmp362;if(_tmp363->tag != 0)goto
_LL1F3;}_LL1F2: {struct Cyc_Core_Impossible_struct _tmp856;const char*_tmp855;
struct Cyc_Core_Impossible_struct*_tmp854;(int)_throw((void*)((_tmp854=_cycalloc(
sizeof(*_tmp854)),((_tmp854[0]=((_tmp856.tag=Cyc_Core_Impossible,((_tmp856.f1=((
_tmp855="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp855,sizeof(char),22))),
_tmp856)))),_tmp854)))));}_LL1F3: {struct Cyc_Absyn_FieldName_struct*_tmp364=(
struct Cyc_Absyn_FieldName_struct*)_tmp362;if(_tmp364->tag != 1)goto _LL1F0;else{
_tmp365=_tmp364->f1;}}_LL1F4: {int _tmp369=Cyc_CfFlowInfo_get_field_index_fs(
_tmp20D,_tmp365);*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),
_tmp369))=(void*)_tmp361->hd;if(_tmp20A == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*
_tmp36A=(*((struct _tuple21*)_tmp207->hd)).f2;_tmp360=Cyc_NewControlFlow_use_Rval(
env,_tmp36A->loc,_tmp360,(void*)_tmp361->hd);}}_LL1F0:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp85C;struct Cyc_CfFlowInfo_Aggregate_struct _tmp85B;struct _tuple13 _tmp85A;
return(_tmp85A.f1=_tmp360,((_tmp85A.f2=(void*)((_tmp85C=_region_malloc(env->r,
sizeof(*_tmp85C)),((_tmp85C[0]=((_tmp85B.tag=6,((_tmp85B.f1=_tmp20A == Cyc_Absyn_UnionA,((
_tmp85B.f2=aggrdict,_tmp85B)))))),_tmp85C)))),_tmp85A)));};};}_LL160: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp20E=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp194;if(_tmp20E->tag != 29)goto
_LL162;}_LL161: {struct Cyc_Core_Impossible_struct _tmp862;const char*_tmp861;
struct Cyc_Core_Impossible_struct*_tmp860;(int)_throw((void*)((_tmp860=_cycalloc(
sizeof(*_tmp860)),((_tmp860[0]=((_tmp862.tag=Cyc_Core_Impossible,((_tmp862.f1=((
_tmp861="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp861,sizeof(char),31))),
_tmp862)))),_tmp860)))));}_LL162: {struct Cyc_Absyn_Array_e_struct*_tmp20F=(
struct Cyc_Absyn_Array_e_struct*)_tmp194;if(_tmp20F->tag != 27)goto _LL164;else{
_tmp210=_tmp20F->f1;}}_LL163: {struct _RegionHandle*_tmp371=env->r;struct Cyc_List_List*
_tmp372=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp371,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp210);union Cyc_CfFlowInfo_FlowInfo _tmp374;
struct Cyc_List_List*_tmp375;struct _tuple19 _tmp373=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp371,env,inflow,_tmp372,1,1);_tmp374=_tmp373.f1;_tmp375=_tmp373.f2;for(0;
_tmp375 != 0;(_tmp375=_tmp375->tl,_tmp372=_tmp372->tl)){_tmp374=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp372))->hd)->loc,
_tmp374,(void*)_tmp375->hd);}{struct _tuple13 _tmp863;return(_tmp863.f1=_tmp374,((
_tmp863.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp172->unknown_all),_tmp863)));};}_LL164: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp211=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp194;if(_tmp211->tag != 28)
goto _LL166;else{_tmp212=_tmp211->f1;_tmp213=_tmp211->f2;_tmp214=_tmp211->f3;
_tmp215=_tmp211->f4;}}_LL165: {union Cyc_CfFlowInfo_FlowInfo _tmp378;void*_tmp379;
struct _tuple13 _tmp377=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp213);_tmp378=
_tmp377.f1;_tmp379=_tmp377.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp37A=_tmp378;int
_tmp37B;struct _tuple12 _tmp37C;struct Cyc_Dict_Dict _tmp37D;struct Cyc_List_List*
_tmp37E;_LL1F6: if((_tmp37A.BottomFL).tag != 1)goto _LL1F8;_tmp37B=(int)(_tmp37A.BottomFL).val;
_LL1F7: {struct _tuple13 _tmp864;return(_tmp864.f1=_tmp378,((_tmp864.f2=_tmp172->unknown_all,
_tmp864)));}_LL1F8: if((_tmp37A.ReachableFL).tag != 2)goto _LL1F5;_tmp37C=(struct
_tuple12)(_tmp37A.ReachableFL).val;_tmp37D=_tmp37C.f1;_tmp37E=_tmp37C.f2;_LL1F9:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp37D,_tmp379)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp867;void*_tmp866;(_tmp866=0,Cyc_Tcutil_terr(_tmp213->loc,((_tmp867="expression may not be initialized",
_tag_dyneither(_tmp867,sizeof(char),34))),_tag_dyneither(_tmp866,sizeof(void*),0)));}{
struct Cyc_List_List*new_relns=_tmp37E;comp_loop: {void*_tmp382=_tmp213->r;struct
Cyc_Absyn_Exp*_tmp384;void*_tmp386;struct Cyc_Absyn_Vardecl*_tmp388;void*_tmp38A;
struct Cyc_Absyn_Vardecl*_tmp38C;void*_tmp38E;struct Cyc_Absyn_Vardecl*_tmp390;
void*_tmp392;struct Cyc_Absyn_Vardecl*_tmp394;union Cyc_Absyn_Cnst _tmp396;struct
_tuple6 _tmp397;int _tmp398;enum Cyc_Absyn_Primop _tmp39A;struct Cyc_List_List*
_tmp39B;struct Cyc_List_List _tmp39C;struct Cyc_Absyn_Exp*_tmp39D;_LL1FB: {struct
Cyc_Absyn_Cast_e_struct*_tmp383=(struct Cyc_Absyn_Cast_e_struct*)_tmp382;if(
_tmp383->tag != 15)goto _LL1FD;else{_tmp384=_tmp383->f2;}}_LL1FC: _tmp213=_tmp384;
goto comp_loop;_LL1FD:{struct Cyc_Absyn_Var_e_struct*_tmp385=(struct Cyc_Absyn_Var_e_struct*)
_tmp382;if(_tmp385->tag != 1)goto _LL1FF;else{_tmp386=(void*)_tmp385->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp387=(struct Cyc_Absyn_Global_b_struct*)_tmp386;if(
_tmp387->tag != 1)goto _LL1FF;else{_tmp388=_tmp387->f1;}};}}if(!(!_tmp388->escapes))
goto _LL1FF;_LL1FE: _tmp38C=_tmp388;goto _LL200;_LL1FF:{struct Cyc_Absyn_Var_e_struct*
_tmp389=(struct Cyc_Absyn_Var_e_struct*)_tmp382;if(_tmp389->tag != 1)goto _LL201;
else{_tmp38A=(void*)_tmp389->f2;{struct Cyc_Absyn_Local_b_struct*_tmp38B=(struct
Cyc_Absyn_Local_b_struct*)_tmp38A;if(_tmp38B->tag != 4)goto _LL201;else{_tmp38C=
_tmp38B->f1;}};}}if(!(!_tmp38C->escapes))goto _LL201;_LL200: _tmp390=_tmp38C;goto
_LL202;_LL201:{struct Cyc_Absyn_Var_e_struct*_tmp38D=(struct Cyc_Absyn_Var_e_struct*)
_tmp382;if(_tmp38D->tag != 1)goto _LL203;else{_tmp38E=(void*)_tmp38D->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp38F=(struct Cyc_Absyn_Pat_b_struct*)_tmp38E;if(_tmp38F->tag
!= 5)goto _LL203;else{_tmp390=_tmp38F->f1;}};}}if(!(!_tmp390->escapes))goto _LL203;
_LL202: _tmp394=_tmp390;goto _LL204;_LL203:{struct Cyc_Absyn_Var_e_struct*_tmp391=(
struct Cyc_Absyn_Var_e_struct*)_tmp382;if(_tmp391->tag != 1)goto _LL205;else{
_tmp392=(void*)_tmp391->f2;{struct Cyc_Absyn_Param_b_struct*_tmp393=(struct Cyc_Absyn_Param_b_struct*)
_tmp392;if(_tmp393->tag != 3)goto _LL205;else{_tmp394=_tmp393->f1;}};}}if(!(!
_tmp394->escapes))goto _LL205;_LL204:{struct Cyc_CfFlowInfo_Reln*_tmp86A;struct Cyc_List_List*
_tmp869;new_relns=((_tmp869=_region_malloc(env->r,sizeof(*_tmp869)),((_tmp869->hd=((
_tmp86A=_region_malloc(env->r,sizeof(*_tmp86A)),((_tmp86A->vd=_tmp212,((_tmp86A->rop=
Cyc_CfFlowInfo_LessVar(_tmp394,_tmp394->type),_tmp86A)))))),((_tmp869->tl=
_tmp37E,_tmp869))))));}goto _LL1FA;_LL205: {struct Cyc_Absyn_Const_e_struct*
_tmp395=(struct Cyc_Absyn_Const_e_struct*)_tmp382;if(_tmp395->tag != 0)goto _LL207;
else{_tmp396=_tmp395->f1;if((_tmp396.Int_c).tag != 4)goto _LL207;_tmp397=(struct
_tuple6)(_tmp396.Int_c).val;_tmp398=_tmp397.f2;}}_LL206:{struct Cyc_CfFlowInfo_Reln*
_tmp86D;struct Cyc_List_List*_tmp86C;new_relns=((_tmp86C=_region_malloc(env->r,
sizeof(*_tmp86C)),((_tmp86C->hd=((_tmp86D=_region_malloc(env->r,sizeof(*_tmp86D)),((
_tmp86D->vd=_tmp212,((_tmp86D->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp398),_tmp86D)))))),((_tmp86C->tl=_tmp37E,_tmp86C))))));}goto _LL1FA;_LL207: {
struct Cyc_Absyn_Primop_e_struct*_tmp399=(struct Cyc_Absyn_Primop_e_struct*)
_tmp382;if(_tmp399->tag != 3)goto _LL209;else{_tmp39A=_tmp399->f1;_tmp39B=_tmp399->f2;
if(_tmp39B == 0)goto _LL209;_tmp39C=*_tmp39B;_tmp39D=(struct Cyc_Absyn_Exp*)_tmp39C.hd;}}
_LL208:{void*_tmp3A2=_tmp39D->r;void*_tmp3A4;struct Cyc_Absyn_Vardecl*_tmp3A6;
void*_tmp3A8;struct Cyc_Absyn_Vardecl*_tmp3AA;void*_tmp3AC;struct Cyc_Absyn_Vardecl*
_tmp3AE;void*_tmp3B0;struct Cyc_Absyn_Vardecl*_tmp3B2;_LL20C:{struct Cyc_Absyn_Var_e_struct*
_tmp3A3=(struct Cyc_Absyn_Var_e_struct*)_tmp3A2;if(_tmp3A3->tag != 1)goto _LL20E;
else{_tmp3A4=(void*)_tmp3A3->f2;{struct Cyc_Absyn_Global_b_struct*_tmp3A5=(struct
Cyc_Absyn_Global_b_struct*)_tmp3A4;if(_tmp3A5->tag != 1)goto _LL20E;else{_tmp3A6=
_tmp3A5->f1;}};}}if(!(!_tmp3A6->escapes))goto _LL20E;_LL20D: _tmp3AA=_tmp3A6;goto
_LL20F;_LL20E:{struct Cyc_Absyn_Var_e_struct*_tmp3A7=(struct Cyc_Absyn_Var_e_struct*)
_tmp3A2;if(_tmp3A7->tag != 1)goto _LL210;else{_tmp3A8=(void*)_tmp3A7->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp3A9=(struct Cyc_Absyn_Local_b_struct*)_tmp3A8;if(
_tmp3A9->tag != 4)goto _LL210;else{_tmp3AA=_tmp3A9->f1;}};}}if(!(!_tmp3AA->escapes))
goto _LL210;_LL20F: _tmp3AE=_tmp3AA;goto _LL211;_LL210:{struct Cyc_Absyn_Var_e_struct*
_tmp3AB=(struct Cyc_Absyn_Var_e_struct*)_tmp3A2;if(_tmp3AB->tag != 1)goto _LL212;
else{_tmp3AC=(void*)_tmp3AB->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp3AD=(struct Cyc_Absyn_Pat_b_struct*)
_tmp3AC;if(_tmp3AD->tag != 5)goto _LL212;else{_tmp3AE=_tmp3AD->f1;}};}}if(!(!
_tmp3AE->escapes))goto _LL212;_LL211: _tmp3B2=_tmp3AE;goto _LL213;_LL212:{struct Cyc_Absyn_Var_e_struct*
_tmp3AF=(struct Cyc_Absyn_Var_e_struct*)_tmp3A2;if(_tmp3AF->tag != 1)goto _LL214;
else{_tmp3B0=(void*)_tmp3AF->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3B1=(struct
Cyc_Absyn_Param_b_struct*)_tmp3B0;if(_tmp3B1->tag != 3)goto _LL214;else{_tmp3B2=
_tmp3B1->f1;}};}}if(!(!_tmp3B2->escapes))goto _LL214;_LL213:{struct Cyc_CfFlowInfo_Reln*
_tmp870;struct Cyc_List_List*_tmp86F;new_relns=((_tmp86F=_region_malloc(env->r,
sizeof(*_tmp86F)),((_tmp86F->hd=((_tmp870=_region_malloc(env->r,sizeof(*_tmp870)),((
_tmp870->vd=_tmp212,((_tmp870->rop=Cyc_CfFlowInfo_LessNumelts(_tmp3B2),_tmp870)))))),((
_tmp86F->tl=_tmp37E,_tmp86F))))));}goto _LL20B;_LL214:;_LL215: goto _LL20B;_LL20B:;}
goto _LL1FA;_LL209:;_LL20A: goto _LL1FA;_LL1FA:;}if(_tmp37E != new_relns)_tmp378=Cyc_CfFlowInfo_ReachableFL(
_tmp37D,new_relns);{void*_tmp3B5=_tmp379;_LL217: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3B6=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3B5;if(_tmp3B6->tag != 0)goto
_LL219;}_LL218: {struct _tuple13 _tmp871;return(_tmp871.f1=_tmp378,((_tmp871.f2=
_tmp172->unknown_all,_tmp871)));}_LL219: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3B7=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3B5;if(_tmp3B7->tag != 2)
goto _LL21B;}_LL21A: goto _LL21C;_LL21B: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3B8=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3B5;if(_tmp3B8->tag != 1)
goto _LL21D;}_LL21C: goto _LL21E;_LL21D: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3B9=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3B5;if(_tmp3B9->tag != 5)goto
_LL21F;}_LL21E: {struct Cyc_List_List _tmp872;struct Cyc_List_List _tmp3BB=(_tmp872.hd=
_tmp212,((_tmp872.tl=0,_tmp872)));_tmp378=Cyc_NewControlFlow_add_vars(_tmp172,
_tmp378,(struct Cyc_List_List*)& _tmp3BB,_tmp172->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3BD;void*_tmp3BE;struct _tuple13 _tmp3BC=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp378,_tmp214);_tmp3BD=_tmp3BC.f1;_tmp3BE=_tmp3BC.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3BF=_tmp3BD;int _tmp3C0;struct _tuple12 _tmp3C1;struct Cyc_Dict_Dict _tmp3C2;
_LL222: if((_tmp3BF.BottomFL).tag != 1)goto _LL224;_tmp3C0=(int)(_tmp3BF.BottomFL).val;
_LL223: {struct _tuple13 _tmp873;return(_tmp873.f1=_tmp3BD,((_tmp873.f2=_tmp172->unknown_all,
_tmp873)));}_LL224: if((_tmp3BF.ReachableFL).tag != 2)goto _LL221;_tmp3C1=(struct
_tuple12)(_tmp3BF.ReachableFL).val;_tmp3C2=_tmp3C1.f1;_LL225: if(Cyc_CfFlowInfo_initlevel(
_tmp172,_tmp3C2,_tmp3BE)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp876;void*_tmp875;(
_tmp875=0,Cyc_Tcutil_terr(_tmp213->loc,((_tmp876="expression may not be initialized",
_tag_dyneither(_tmp876,sizeof(char),34))),_tag_dyneither(_tmp875,sizeof(void*),0)));}{
struct _tuple13 _tmp877;return(_tmp877.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp877.f2=
_tmp172->unknown_all,_tmp877)));};}_LL221:;}_tmp378=_tmp3BD;goto _LL220;};}_LL21F:;
_LL220: while(1){struct Cyc_List_List _tmp878;struct Cyc_List_List _tmp3C8=(_tmp878.hd=
_tmp212,((_tmp878.tl=0,_tmp878)));_tmp378=Cyc_NewControlFlow_add_vars(_tmp172,
_tmp378,(struct Cyc_List_List*)& _tmp3C8,_tmp172->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3CA;void*_tmp3CB;struct _tuple13 _tmp3C9=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp378,_tmp214);_tmp3CA=_tmp3C9.f1;_tmp3CB=_tmp3C9.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3CC=_tmp3CA;int _tmp3CD;struct _tuple12 _tmp3CE;struct Cyc_Dict_Dict _tmp3CF;
_LL227: if((_tmp3CC.BottomFL).tag != 1)goto _LL229;_tmp3CD=(int)(_tmp3CC.BottomFL).val;
_LL228: goto _LL226;_LL229: if((_tmp3CC.ReachableFL).tag != 2)goto _LL226;_tmp3CE=(
struct _tuple12)(_tmp3CC.ReachableFL).val;_tmp3CF=_tmp3CE.f1;_LL22A: if(Cyc_CfFlowInfo_initlevel(
_tmp172,_tmp3CF,_tmp3CB)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp87B;void*_tmp87A;(
_tmp87A=0,Cyc_Tcutil_terr(_tmp213->loc,((_tmp87B="expression may not be initialized",
_tag_dyneither(_tmp87B,sizeof(char),34))),_tag_dyneither(_tmp87A,sizeof(void*),0)));}{
struct _tuple13 _tmp87C;return(_tmp87C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp87C.f2=
_tmp172->unknown_all,_tmp87C)));};}_LL226:;}{union Cyc_CfFlowInfo_FlowInfo _tmp3D3=
Cyc_CfFlowInfo_join_flow(_tmp172,env->all_changed,_tmp378,_tmp3CA);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3D3,_tmp378))break;_tmp378=_tmp3D3;};};}{struct _tuple13 _tmp87D;return(
_tmp87D.f1=_tmp378,((_tmp87D.f2=_tmp172->unknown_all,_tmp87D)));};_LL216:;};};
_LL1F5:;};}_LL166: {struct Cyc_Absyn_StmtExp_e_struct*_tmp216=(struct Cyc_Absyn_StmtExp_e_struct*)
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
_LL173: {struct Cyc_Core_Impossible_struct _tmp883;const char*_tmp882;struct Cyc_Core_Impossible_struct*
_tmp881;(int)_throw((void*)((_tmp881=_cycalloc(sizeof(*_tmp881)),((_tmp881[0]=((
_tmp883.tag=Cyc_Core_Impossible,((_tmp883.f1=((_tmp882="anal_Rexp, unexpected exp form",
_tag_dyneither(_tmp882,sizeof(char),31))),_tmp883)))),_tmp881)))));}_LL10B:;};}
static struct _tuple14 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo
f,void*r,struct Cyc_List_List*flds);static struct _tuple14 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3D9=env->fenv;void*_tmp3DA=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp3DC;void*_tmp3DD;struct Cyc_Absyn_PtrAtts _tmp3DE;union Cyc_Absyn_Constraint*
_tmp3DF;union Cyc_Absyn_Constraint*_tmp3E0;_LL22C: {struct Cyc_Absyn_PointerType_struct*
_tmp3DB=(struct Cyc_Absyn_PointerType_struct*)_tmp3DA;if(_tmp3DB->tag != 5)goto
_LL22E;else{_tmp3DC=_tmp3DB->f1;_tmp3DD=_tmp3DC.elt_typ;_tmp3DE=_tmp3DC.ptr_atts;
_tmp3DF=_tmp3DE.bounds;_tmp3E0=_tmp3DE.zero_term;}}_LL22D: {union Cyc_CfFlowInfo_FlowInfo
_tmp3E1=f;int _tmp3E2;struct _tuple12 _tmp3E3;struct Cyc_Dict_Dict _tmp3E4;struct Cyc_List_List*
_tmp3E5;_LL231: if((_tmp3E1.BottomFL).tag != 1)goto _LL233;_tmp3E2=(int)(_tmp3E1.BottomFL).val;
_LL232: {struct _tuple14 _tmp884;return(_tmp884.f1=f,((_tmp884.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp884)));}_LL233: if((_tmp3E1.ReachableFL).tag != 2)goto _LL230;_tmp3E3=(struct
_tuple12)(_tmp3E1.ReachableFL).val;_tmp3E4=_tmp3E3.f1;_tmp3E5=_tmp3E3.f2;_LL234:
if(Cyc_Tcutil_is_bound_one(_tmp3DF)){void*_tmp3E7=r;struct Cyc_CfFlowInfo_Place*
_tmp3EB;struct Cyc_CfFlowInfo_Place _tmp3EC;void*_tmp3ED;struct Cyc_List_List*
_tmp3EE;enum Cyc_CfFlowInfo_InitLevel _tmp3F1;_LL236: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3E8=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3E7;if(_tmp3E8->tag != 1)
goto _LL238;}_LL237: goto _LL239;_LL238: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3E9=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3E7;if(_tmp3E9->tag != 2)
goto _LL23A;}_LL239:{struct Cyc_CfFlowInfo_NotZero_struct _tmp887;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp886;e->annot=(void*)((_tmp886=_cycalloc(sizeof(*_tmp886)),((_tmp886[0]=((
_tmp887.tag=Cyc_CfFlowInfo_NotZero,((_tmp887.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3E5),_tmp887)))),_tmp886))));}goto _LL235;_LL23A: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3EA=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3E7;if(_tmp3EA->tag != 5)goto
_LL23C;else{_tmp3EB=_tmp3EA->f1;_tmp3EC=*_tmp3EB;_tmp3ED=_tmp3EC.root;_tmp3EE=
_tmp3EC.fields;}}_LL23B:{struct Cyc_CfFlowInfo_NotZero_struct _tmp88A;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp889;e->annot=(void*)((_tmp889=_cycalloc(sizeof(*_tmp889)),((_tmp889[0]=((
_tmp88A.tag=Cyc_CfFlowInfo_NotZero,((_tmp88A.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3E5),_tmp88A)))),_tmp889))));}{struct Cyc_CfFlowInfo_Place*_tmp88D;struct
_tuple14 _tmp88C;return(_tmp88C.f1=f,((_tmp88C.f2=Cyc_CfFlowInfo_PlaceL(((_tmp88D=
_region_malloc(_tmp3D9->r,sizeof(*_tmp88D)),((_tmp88D->root=_tmp3ED,((_tmp88D->fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(_tmp3D9->r,_tmp3EE,flds),_tmp88D))))))),_tmp88C)));};_LL23C: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp3EF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp3E7;if(_tmp3EF->tag != 0)goto _LL23E;}_LL23D: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp88E;return(_tmp88E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp88E.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp88E)));};_LL23E: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3F0=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3E7;if(_tmp3F0->tag != 3)goto
_LL240;else{_tmp3F1=_tmp3F0->f1;}}_LL23F: f=Cyc_NewControlFlow_notzero(env,inflow,
f,e,_tmp3F1);goto _LL241;_LL240:;_LL241: {struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp891;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp890;e->annot=(void*)((_tmp890=
_cycalloc(sizeof(*_tmp890)),((_tmp890[0]=((_tmp891.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp891.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3E5),_tmp891)))),
_tmp890))));}_LL235:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp894;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp893;e->annot=(void*)((_tmp893=_cycalloc(sizeof(*_tmp893)),((_tmp893[0]=((
_tmp894.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp894.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3E5),_tmp894)))),_tmp893))));}if(Cyc_CfFlowInfo_initlevel(_tmp3D9,_tmp3E4,r)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp897;void*_tmp896;(_tmp896=0,Cyc_Tcutil_terr(
e->loc,((_tmp897="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp897,sizeof(char),46))),_tag_dyneither(_tmp896,sizeof(void*),0)));}{struct
_tuple14 _tmp898;return(_tmp898.f1=f,((_tmp898.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp898)));};_LL230:;}_LL22E:;_LL22F: {struct Cyc_Core_Impossible_struct _tmp89E;
const char*_tmp89D;struct Cyc_Core_Impossible_struct*_tmp89C;(int)_throw((void*)((
_tmp89C=_cycalloc(sizeof(*_tmp89C)),((_tmp89C[0]=((_tmp89E.tag=Cyc_Core_Impossible,((
_tmp89E.f1=((_tmp89D="left deref of non-pointer-type",_tag_dyneither(_tmp89D,
sizeof(char),31))),_tmp89E)))),_tmp89C)))));}_LL22B:;}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int
expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds);static struct
_tuple14 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp403=
env->fenv;{union Cyc_CfFlowInfo_FlowInfo _tmp404=inflow;int _tmp405;struct _tuple12
_tmp406;struct Cyc_Dict_Dict _tmp407;struct Cyc_List_List*_tmp408;_LL243: if((
_tmp404.BottomFL).tag != 1)goto _LL245;_tmp405=(int)(_tmp404.BottomFL).val;_LL244: {
struct _tuple14 _tmp89F;return(_tmp89F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp89F.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp89F)));}_LL245: if((_tmp404.ReachableFL).tag != 2)
goto _LL242;_tmp406=(struct _tuple12)(_tmp404.ReachableFL).val;_tmp407=_tmp406.f1;
_tmp408=_tmp406.f2;_LL246: d=_tmp407;_LL242:;}{void*_tmp40A=e->r;struct Cyc_Absyn_Exp*
_tmp40C;struct Cyc_Absyn_Exp*_tmp40E;struct Cyc_Absyn_Exp*_tmp410;void*_tmp412;
struct Cyc_Absyn_Vardecl*_tmp414;void*_tmp416;struct Cyc_Absyn_Vardecl*_tmp418;
void*_tmp41A;struct Cyc_Absyn_Vardecl*_tmp41C;void*_tmp41E;struct Cyc_Absyn_Vardecl*
_tmp420;struct Cyc_Absyn_Exp*_tmp422;struct _dyneither_ptr*_tmp423;struct Cyc_Absyn_Exp*
_tmp425;struct Cyc_Absyn_Exp*_tmp427;struct Cyc_Absyn_Exp*_tmp428;struct Cyc_Absyn_Exp*
_tmp42A;struct _dyneither_ptr*_tmp42B;_LL248: {struct Cyc_Absyn_Cast_e_struct*
_tmp40B=(struct Cyc_Absyn_Cast_e_struct*)_tmp40A;if(_tmp40B->tag != 15)goto _LL24A;
else{_tmp40C=_tmp40B->f2;}}_LL249: _tmp40E=_tmp40C;goto _LL24B;_LL24A: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp40D=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp40A;if(_tmp40D->tag != 13)
goto _LL24C;else{_tmp40E=_tmp40D->f1;}}_LL24B: _tmp410=_tmp40E;goto _LL24D;_LL24C: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp40F=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp40A;if(_tmp40F->tag != 14)goto _LL24E;else{_tmp410=_tmp40F->f1;}}_LL24D: return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp410,flds);_LL24E: {
struct Cyc_Absyn_Var_e_struct*_tmp411=(struct Cyc_Absyn_Var_e_struct*)_tmp40A;if(
_tmp411->tag != 1)goto _LL250;else{_tmp412=(void*)_tmp411->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp413=(struct Cyc_Absyn_Param_b_struct*)_tmp412;if(_tmp413->tag != 3)goto _LL250;
else{_tmp414=_tmp413->f1;}};}}_LL24F: _tmp418=_tmp414;goto _LL251;_LL250: {struct
Cyc_Absyn_Var_e_struct*_tmp415=(struct Cyc_Absyn_Var_e_struct*)_tmp40A;if(_tmp415->tag
!= 1)goto _LL252;else{_tmp416=(void*)_tmp415->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp417=(struct Cyc_Absyn_Local_b_struct*)_tmp416;if(_tmp417->tag != 4)goto _LL252;
else{_tmp418=_tmp417->f1;}};}}_LL251: _tmp41C=_tmp418;goto _LL253;_LL252: {struct
Cyc_Absyn_Var_e_struct*_tmp419=(struct Cyc_Absyn_Var_e_struct*)_tmp40A;if(_tmp419->tag
!= 1)goto _LL254;else{_tmp41A=(void*)_tmp419->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp41B=(struct Cyc_Absyn_Pat_b_struct*)_tmp41A;if(_tmp41B->tag != 5)goto _LL254;
else{_tmp41C=_tmp41B->f1;}};}}_LL253: {struct Cyc_CfFlowInfo_Place*_tmp8A9;struct
Cyc_CfFlowInfo_VarRoot_struct _tmp8A8;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp8A7;
struct _tuple14 _tmp8A6;return(_tmp8A6.f1=inflow,((_tmp8A6.f2=Cyc_CfFlowInfo_PlaceL(((
_tmp8A9=_region_malloc(env->r,sizeof(*_tmp8A9)),((_tmp8A9->root=(void*)((_tmp8A7=
_region_malloc(env->r,sizeof(*_tmp8A7)),((_tmp8A7[0]=((_tmp8A8.tag=0,((_tmp8A8.f1=
_tmp41C,_tmp8A8)))),_tmp8A7)))),((_tmp8A9->fields=flds,_tmp8A9))))))),_tmp8A6)));}
_LL254: {struct Cyc_Absyn_Var_e_struct*_tmp41D=(struct Cyc_Absyn_Var_e_struct*)
_tmp40A;if(_tmp41D->tag != 1)goto _LL256;else{_tmp41E=(void*)_tmp41D->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp41F=(struct Cyc_Absyn_Global_b_struct*)_tmp41E;if(
_tmp41F->tag != 1)goto _LL256;else{_tmp420=_tmp41F->f1;}};}}_LL255: {struct
_tuple14 _tmp8AA;return(_tmp8AA.f1=inflow,((_tmp8AA.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8AA)));}_LL256: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp421=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp40A;if(_tmp421->tag != 23)goto _LL258;else{_tmp422=_tmp421->f1;_tmp423=_tmp421->f2;}}
_LL257:{void*_tmp431=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp422->topt))->v);struct Cyc_Absyn_PtrInfo _tmp433;void*_tmp434;
_LL261: {struct Cyc_Absyn_PointerType_struct*_tmp432=(struct Cyc_Absyn_PointerType_struct*)
_tmp431;if(_tmp432->tag != 5)goto _LL263;else{_tmp433=_tmp432->f1;_tmp434=_tmp433.elt_typ;}}
_LL262: if(!Cyc_Absyn_is_nontagged_union_type(_tmp434)){struct Cyc_List_List*
_tmp8AB;flds=((_tmp8AB=_region_malloc(env->r,sizeof(*_tmp8AB)),((_tmp8AB->hd=(
void*)Cyc_CfFlowInfo_get_field_index(_tmp434,_tmp423),((_tmp8AB->tl=flds,_tmp8AB))))));}
goto _LL260;_LL263:;_LL264: {struct Cyc_Core_Impossible_struct _tmp8B1;const char*
_tmp8B0;struct Cyc_Core_Impossible_struct*_tmp8AF;(int)_throw((void*)((_tmp8AF=
_cycalloc(sizeof(*_tmp8AF)),((_tmp8AF[0]=((_tmp8B1.tag=Cyc_Core_Impossible,((
_tmp8B1.f1=((_tmp8B0="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp8B0,sizeof(
char),25))),_tmp8B1)))),_tmp8AF)))));}_LL260:;}_tmp425=_tmp422;goto _LL259;_LL258: {
struct Cyc_Absyn_Deref_e_struct*_tmp424=(struct Cyc_Absyn_Deref_e_struct*)_tmp40A;
if(_tmp424->tag != 21)goto _LL25A;else{_tmp425=_tmp424->f1;}}_LL259: if(
expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp425->topt))->v)){union Cyc_CfFlowInfo_FlowInfo _tmp43A;union Cyc_CfFlowInfo_AbsLVal
_tmp43B;struct _tuple14 _tmp439=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp425);
_tmp43A=_tmp439.f1;_tmp43B=_tmp439.f2;{struct _tuple14 _tmp8B2;struct _tuple14
_tmp43D=(_tmp8B2.f1=_tmp43A,((_tmp8B2.f2=_tmp43B,_tmp8B2)));union Cyc_CfFlowInfo_FlowInfo
_tmp43E;struct _tuple12 _tmp43F;struct Cyc_Dict_Dict _tmp440;struct Cyc_List_List*
_tmp441;union Cyc_CfFlowInfo_AbsLVal _tmp442;struct Cyc_CfFlowInfo_Place*_tmp443;
_LL266: _tmp43E=_tmp43D.f1;if((_tmp43E.ReachableFL).tag != 2)goto _LL268;_tmp43F=(
struct _tuple12)(_tmp43E.ReachableFL).val;_tmp440=_tmp43F.f1;_tmp441=_tmp43F.f2;
_tmp442=_tmp43D.f2;if((_tmp442.PlaceL).tag != 1)goto _LL268;_tmp443=(struct Cyc_CfFlowInfo_Place*)(
_tmp442.PlaceL).val;_LL267: {void*_tmp444=Cyc_CfFlowInfo_lookup_place(_tmp440,
_tmp443);void*_tmp445=_tmp444;_LL26B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp446=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp445;if(_tmp446->tag != 5)goto
_LL26D;}_LL26C: return Cyc_NewControlFlow_anal_derefL(env,_tmp43A,_tmp425,_tmp43A,
_tmp444,flds);_LL26D:;_LL26E: {enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(
_tmp403,_tmp440,_tmp444);void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp403->unknown_all:
_tmp403->unknown_none;void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp403,Cyc_Tcutil_pointer_elt_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp425->topt))->v),leaf);struct Cyc_CfFlowInfo_MallocPt_struct
_tmp8B5;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp8B4;void*new_root=(void*)((
_tmp8B4=_region_malloc(_tmp403->r,sizeof(*_tmp8B4)),((_tmp8B4[0]=((_tmp8B5.tag=1,((
_tmp8B5.f1=e,((_tmp8B5.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp8B5)))))),_tmp8B4))));struct Cyc_CfFlowInfo_Place*_tmp8B6;struct Cyc_CfFlowInfo_Place*
place=(_tmp8B6=_region_malloc(_tmp403->r,sizeof(*_tmp8B6)),((_tmp8B6->root=
new_root,((_tmp8B6->fields=0,_tmp8B6)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp8B9;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp8B8;void*res=(void*)((_tmp8B8=
_region_malloc(_tmp403->r,sizeof(*_tmp8B8)),((_tmp8B8[0]=((_tmp8B9.tag=5,((
_tmp8B9.f1=place,_tmp8B9)))),_tmp8B8))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);_tmp440=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(_tmp440,new_root,new_rval);_tmp440=Cyc_CfFlowInfo_assign_place(
_tmp403,e->loc,_tmp440,env->all_changed,_tmp443,res);{union Cyc_CfFlowInfo_FlowInfo
_tmp447=Cyc_CfFlowInfo_ReachableFL(_tmp440,_tmp441);return Cyc_NewControlFlow_anal_derefL(
env,_tmp447,_tmp425,_tmp447,res,flds);};}_LL26A:;}_LL268:;_LL269: goto _LL265;
_LL265:;};}{union Cyc_CfFlowInfo_FlowInfo _tmp44E;void*_tmp44F;struct _tuple13
_tmp44D=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp425);_tmp44E=_tmp44D.f1;
_tmp44F=_tmp44D.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp425,
_tmp44E,_tmp44F,flds);};_LL25A: {struct Cyc_Absyn_Subscript_e_struct*_tmp426=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp40A;if(_tmp426->tag != 24)goto _LL25C;
else{_tmp427=_tmp426->f1;_tmp428=_tmp426->f2;}}_LL25B: {void*_tmp450=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp427->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp453;struct Cyc_Absyn_PtrAtts _tmp454;union Cyc_Absyn_Constraint*_tmp455;_LL270: {
struct Cyc_Absyn_TupleType_struct*_tmp451=(struct Cyc_Absyn_TupleType_struct*)
_tmp450;if(_tmp451->tag != 11)goto _LL272;}_LL271: {unsigned int _tmp456=(Cyc_Evexp_eval_const_uint_exp(
_tmp428)).f1;struct Cyc_List_List*_tmp8BA;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,_tmp427,((_tmp8BA=_region_malloc(env->r,sizeof(*_tmp8BA)),((
_tmp8BA->hd=(void*)((int)_tmp456),((_tmp8BA->tl=flds,_tmp8BA)))))));}_LL272: {
struct Cyc_Absyn_PointerType_struct*_tmp452=(struct Cyc_Absyn_PointerType_struct*)
_tmp450;if(_tmp452->tag != 5)goto _LL274;else{_tmp453=_tmp452->f1;_tmp454=_tmp453.ptr_atts;
_tmp455=_tmp454.bounds;}}_LL273: {struct _RegionHandle*_tmp458=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp45B;struct Cyc_List_List*_tmp45C;struct Cyc_Absyn_Exp*_tmp8BB[2];struct
_tuple19 _tmp45A=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp458,env,inflow,((
_tmp8BB[1]=_tmp428,((_tmp8BB[0]=_tmp427,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp458,_tag_dyneither(
_tmp8BB,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp45B=_tmp45A.f1;_tmp45C=
_tmp45A.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp45D=_tmp45B;{union Cyc_CfFlowInfo_FlowInfo
_tmp45E=_tmp45B;struct _tuple12 _tmp45F;struct Cyc_Dict_Dict _tmp460;struct Cyc_List_List*
_tmp461;_LL277: if((_tmp45E.ReachableFL).tag != 2)goto _LL279;_tmp45F=(struct
_tuple12)(_tmp45E.ReachableFL).val;_tmp460=_tmp45F.f1;_tmp461=_tmp45F.f2;_LL278:
if(Cyc_CfFlowInfo_initlevel(_tmp403,_tmp460,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp45C))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8BE;void*_tmp8BD;(_tmp8BD=0,Cyc_Tcutil_terr(_tmp428->loc,((_tmp8BE="expression may not be initialized",
_tag_dyneither(_tmp8BE,sizeof(char),34))),_tag_dyneither(_tmp8BD,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp464=Cyc_NewControlFlow_add_subscript_reln(_tmp403->r,
_tmp461,_tmp427,_tmp428);if(_tmp461 != _tmp464)_tmp45D=Cyc_CfFlowInfo_ReachableFL(
_tmp460,_tmp464);goto _LL276;};_LL279:;_LL27A: goto _LL276;_LL276:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp466;union Cyc_CfFlowInfo_AbsLVal _tmp467;struct _tuple14 _tmp465=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp427,_tmp45B,(void*)((struct Cyc_List_List*)_check_null(_tmp45C))->hd,
flds);_tmp466=_tmp465.f1;_tmp467=_tmp465.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp468=
_tmp466;int _tmp469;_LL27C: if((_tmp468.BottomFL).tag != 1)goto _LL27E;_tmp469=(int)(
_tmp468.BottomFL).val;_LL27D: {struct _tuple14 _tmp8BF;return(_tmp8BF.f1=_tmp466,((
_tmp8BF.f2=_tmp467,_tmp8BF)));}_LL27E:;_LL27F: {struct _tuple14 _tmp8C0;return(
_tmp8C0.f1=_tmp45D,((_tmp8C0.f2=_tmp467,_tmp8C0)));}_LL27B:;};};};}_LL274:;
_LL275: {struct Cyc_Core_Impossible_struct _tmp8C6;const char*_tmp8C5;struct Cyc_Core_Impossible_struct*
_tmp8C4;(int)_throw((void*)((_tmp8C4=_cycalloc(sizeof(*_tmp8C4)),((_tmp8C4[0]=((
_tmp8C6.tag=Cyc_Core_Impossible,((_tmp8C6.f1=((_tmp8C5="anal_Lexp: Subscript -- bad type",
_tag_dyneither(_tmp8C5,sizeof(char),33))),_tmp8C6)))),_tmp8C4)))));}_LL26F:;}
_LL25C: {struct Cyc_Absyn_AggrMember_e_struct*_tmp429=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp40A;if(_tmp429->tag != 22)goto _LL25E;else{_tmp42A=_tmp429->f1;_tmp42B=_tmp429->f2;}}
_LL25D: if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp42A->topt))->v)){struct _tuple14 _tmp8C7;return(_tmp8C7.f1=inflow,((
_tmp8C7.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8C7)));}{struct Cyc_List_List*_tmp8C8;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp42A,((
_tmp8C8=_region_malloc(env->r,sizeof(*_tmp8C8)),((_tmp8C8->hd=(void*)Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp42A->topt))->v,_tmp42B),((_tmp8C8->tl=
flds,_tmp8C8)))))));};_LL25E:;_LL25F: {struct _tuple14 _tmp8C9;return(_tmp8C9.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp8C9.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8C9)));}
_LL247:;};}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e);
static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
union Cyc_CfFlowInfo_FlowInfo _tmp473;union Cyc_CfFlowInfo_AbsLVal _tmp474;struct
_tuple14 _tmp472=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);
_tmp473=_tmp472.f1;_tmp474=_tmp472.f2;{struct _tuple14 _tmp8CA;return(_tmp8CA.f1=
_tmp473,((_tmp8CA.f2=_tmp474,_tmp8CA)));};}static struct _tuple15 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e);static struct _tuple15 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp476=env->fenv;{void*_tmp477=e->r;struct Cyc_Absyn_Exp*_tmp479;struct Cyc_Absyn_Exp*
_tmp47A;struct Cyc_Absyn_Exp*_tmp47B;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_Absyn_Exp*
_tmp47E;struct Cyc_Absyn_Exp*_tmp480;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Exp*
_tmp483;struct Cyc_Absyn_Exp*_tmp484;enum Cyc_Absyn_Primop _tmp486;struct Cyc_List_List*
_tmp487;struct Cyc_List_List _tmp488;struct Cyc_Absyn_Exp*_tmp489;struct Cyc_List_List*
_tmp48A;enum Cyc_Absyn_Primop _tmp48C;struct Cyc_List_List*_tmp48D;_LL281: {struct
Cyc_Absyn_Conditional_e_struct*_tmp478=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp477;if(_tmp478->tag != 6)goto _LL283;else{_tmp479=_tmp478->f1;_tmp47A=_tmp478->f2;
_tmp47B=_tmp478->f3;}}_LL282: {union Cyc_CfFlowInfo_FlowInfo _tmp48F;union Cyc_CfFlowInfo_FlowInfo
_tmp490;struct _tuple15 _tmp48E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp479);
_tmp48F=_tmp48E.f1;_tmp490=_tmp48E.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp492;union
Cyc_CfFlowInfo_FlowInfo _tmp493;struct _tuple15 _tmp491=Cyc_NewControlFlow_anal_test(
env,_tmp48F,_tmp47A);_tmp492=_tmp491.f1;_tmp493=_tmp491.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp495;union Cyc_CfFlowInfo_FlowInfo _tmp496;struct _tuple15 _tmp494=Cyc_NewControlFlow_anal_test(
env,_tmp490,_tmp47B);_tmp495=_tmp494.f1;_tmp496=_tmp494.f2;{struct _tuple15
_tmp8CB;return(_tmp8CB.f1=Cyc_CfFlowInfo_join_flow(_tmp476,env->all_changed,
_tmp492,_tmp495),((_tmp8CB.f2=Cyc_CfFlowInfo_join_flow(_tmp476,env->all_changed,
_tmp493,_tmp496),_tmp8CB)));};};};}_LL283: {struct Cyc_Absyn_And_e_struct*_tmp47C=(
struct Cyc_Absyn_And_e_struct*)_tmp477;if(_tmp47C->tag != 7)goto _LL285;else{
_tmp47D=_tmp47C->f1;_tmp47E=_tmp47C->f2;}}_LL284: {union Cyc_CfFlowInfo_FlowInfo
_tmp499;union Cyc_CfFlowInfo_FlowInfo _tmp49A;struct _tuple15 _tmp498=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp47D);_tmp499=_tmp498.f1;_tmp49A=_tmp498.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp49C;union Cyc_CfFlowInfo_FlowInfo _tmp49D;struct _tuple15 _tmp49B=Cyc_NewControlFlow_anal_test(
env,_tmp499,_tmp47E);_tmp49C=_tmp49B.f1;_tmp49D=_tmp49B.f2;{struct _tuple15
_tmp8CC;return(_tmp8CC.f1=_tmp49C,((_tmp8CC.f2=Cyc_CfFlowInfo_join_flow(_tmp476,
env->all_changed,_tmp49A,_tmp49D),_tmp8CC)));};};}_LL285: {struct Cyc_Absyn_Or_e_struct*
_tmp47F=(struct Cyc_Absyn_Or_e_struct*)_tmp477;if(_tmp47F->tag != 8)goto _LL287;
else{_tmp480=_tmp47F->f1;_tmp481=_tmp47F->f2;}}_LL286: {union Cyc_CfFlowInfo_FlowInfo
_tmp4A0;union Cyc_CfFlowInfo_FlowInfo _tmp4A1;struct _tuple15 _tmp49F=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp480);_tmp4A0=_tmp49F.f1;_tmp4A1=_tmp49F.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4A3;union Cyc_CfFlowInfo_FlowInfo _tmp4A4;struct _tuple15 _tmp4A2=Cyc_NewControlFlow_anal_test(
env,_tmp4A1,_tmp481);_tmp4A3=_tmp4A2.f1;_tmp4A4=_tmp4A2.f2;{struct _tuple15
_tmp8CD;return(_tmp8CD.f1=Cyc_CfFlowInfo_join_flow(_tmp476,env->all_changed,
_tmp4A0,_tmp4A3),((_tmp8CD.f2=_tmp4A4,_tmp8CD)));};};}_LL287: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp482=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp477;if(_tmp482->tag != 9)goto _LL289;
else{_tmp483=_tmp482->f1;_tmp484=_tmp482->f2;}}_LL288: {union Cyc_CfFlowInfo_FlowInfo
_tmp4A7;void*_tmp4A8;struct _tuple13 _tmp4A6=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp483);_tmp4A7=_tmp4A6.f1;_tmp4A8=_tmp4A6.f2;return Cyc_NewControlFlow_anal_test(
env,_tmp4A7,_tmp484);}_LL289: {struct Cyc_Absyn_Primop_e_struct*_tmp485=(struct
Cyc_Absyn_Primop_e_struct*)_tmp477;if(_tmp485->tag != 3)goto _LL28B;else{_tmp486=
_tmp485->f1;if(_tmp486 != Cyc_Absyn_Not)goto _LL28B;_tmp487=_tmp485->f2;if(_tmp487
== 0)goto _LL28B;_tmp488=*_tmp487;_tmp489=(struct Cyc_Absyn_Exp*)_tmp488.hd;
_tmp48A=_tmp488.tl;if(_tmp48A != 0)goto _LL28B;}}_LL28A: {union Cyc_CfFlowInfo_FlowInfo
_tmp4AA;union Cyc_CfFlowInfo_FlowInfo _tmp4AB;struct _tuple15 _tmp4A9=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp489);_tmp4AA=_tmp4A9.f1;_tmp4AB=_tmp4A9.f2;{struct _tuple15 _tmp8CE;
return(_tmp8CE.f1=_tmp4AB,((_tmp8CE.f2=_tmp4AA,_tmp8CE)));};}_LL28B: {struct Cyc_Absyn_Primop_e_struct*
_tmp48B=(struct Cyc_Absyn_Primop_e_struct*)_tmp477;if(_tmp48B->tag != 3)goto _LL28D;
else{_tmp48C=_tmp48B->f1;_tmp48D=_tmp48B->f2;}}_LL28C: {void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo
f;struct _RegionHandle*_tmp4AD=env->r;{union Cyc_CfFlowInfo_FlowInfo _tmp4AF;struct
Cyc_List_List*_tmp4B0;struct _tuple19 _tmp4AE=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp4AD,env,inflow,_tmp48D,0,0);_tmp4AF=_tmp4AE.f1;_tmp4B0=_tmp4AE.f2;r1=(void*)((
struct Cyc_List_List*)_check_null(_tmp4B0))->hd;r2=(void*)((struct Cyc_List_List*)
_check_null(_tmp4B0->tl))->hd;f=_tmp4AF;}{union Cyc_CfFlowInfo_FlowInfo _tmp4B1=f;
int _tmp4B2;struct _tuple12 _tmp4B3;struct Cyc_Dict_Dict _tmp4B4;struct Cyc_List_List*
_tmp4B5;_LL290: if((_tmp4B1.BottomFL).tag != 1)goto _LL292;_tmp4B2=(int)(_tmp4B1.BottomFL).val;
_LL291: {struct _tuple15 _tmp8CF;return(_tmp8CF.f1=f,((_tmp8CF.f2=f,_tmp8CF)));}
_LL292: if((_tmp4B1.ReachableFL).tag != 2)goto _LL28F;_tmp4B3=(struct _tuple12)(
_tmp4B1.ReachableFL).val;_tmp4B4=_tmp4B3.f1;_tmp4B5=_tmp4B3.f2;_LL293: {struct
Cyc_Absyn_Exp*_tmp4B7=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp48D))->hd;struct Cyc_Absyn_Exp*_tmp4B8=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp48D->tl))->hd;if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4B4,r1)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp8D2;void*_tmp8D1;(_tmp8D1=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp48D->hd)->loc,((_tmp8D2="expression may not be initialized",
_tag_dyneither(_tmp8D2,sizeof(char),34))),_tag_dyneither(_tmp8D1,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4B4,r2)== Cyc_CfFlowInfo_NoneIL){const
char*_tmp8D5;void*_tmp8D4;(_tmp8D4=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48D->tl))->hd)->loc,((_tmp8D5="expression may not be initialized",
_tag_dyneither(_tmp8D5,sizeof(char),34))),_tag_dyneither(_tmp8D4,sizeof(void*),0)));}
if(_tmp48C == Cyc_Absyn_Eq  || _tmp48C == Cyc_Absyn_Neq){struct _tuple0 _tmp8D6;
struct _tuple0 _tmp4BE=(_tmp8D6.f1=r1,((_tmp8D6.f2=r2,_tmp8D6)));void*_tmp4BF;
enum Cyc_CfFlowInfo_InitLevel _tmp4C1;void*_tmp4C2;void*_tmp4C4;void*_tmp4C6;
enum Cyc_CfFlowInfo_InitLevel _tmp4C8;void*_tmp4C9;void*_tmp4CB;void*_tmp4CD;void*
_tmp4CF;void*_tmp4D1;void*_tmp4D3;void*_tmp4D5;void*_tmp4D7;void*_tmp4D9;void*
_tmp4DB;void*_tmp4DD;void*_tmp4DF;void*_tmp4E1;void*_tmp4E3;_LL295: _tmp4BF=
_tmp4BE.f1;{struct Cyc_CfFlowInfo_UnknownR_struct*_tmp4C0=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4BF;if(_tmp4C0->tag != 3)goto _LL297;else{_tmp4C1=_tmp4C0->f1;}};_tmp4C2=
_tmp4BE.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4C3=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4C2;if(_tmp4C3->tag != 0)goto _LL297;};_LL296: {union Cyc_CfFlowInfo_FlowInfo
_tmp4E6;union Cyc_CfFlowInfo_FlowInfo _tmp4E7;struct _tuple15 _tmp4E5=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4B7,_tmp4C1);_tmp4E6=_tmp4E5.f1;_tmp4E7=_tmp4E5.f2;switch(_tmp48C){
case Cyc_Absyn_Eq: _LL2A9: {struct _tuple15 _tmp8D7;return(_tmp8D7.f1=_tmp4E7,((
_tmp8D7.f2=_tmp4E6,_tmp8D7)));}case Cyc_Absyn_Neq: _LL2AA: {struct _tuple15 _tmp8D8;
return(_tmp8D8.f1=_tmp4E6,((_tmp8D8.f2=_tmp4E7,_tmp8D8)));}default: _LL2AB: {
struct Cyc_Core_Impossible_struct _tmp8DE;const char*_tmp8DD;struct Cyc_Core_Impossible_struct*
_tmp8DC;(int)_throw((void*)((_tmp8DC=_cycalloc(sizeof(*_tmp8DC)),((_tmp8DC[0]=((
_tmp8DE.tag=Cyc_Core_Impossible,((_tmp8DE.f1=((_tmp8DD="anal_test, zero-split",
_tag_dyneither(_tmp8DD,sizeof(char),22))),_tmp8DE)))),_tmp8DC)))));}}}_LL297:
_tmp4C4=_tmp4BE.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4C5=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4C4;if(_tmp4C5->tag != 0)goto _LL299;};_tmp4C6=_tmp4BE.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4C7=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4C6;if(_tmp4C7->tag != 3)goto
_LL299;else{_tmp4C8=_tmp4C7->f1;}};_LL298: {union Cyc_CfFlowInfo_FlowInfo _tmp4EE;
union Cyc_CfFlowInfo_FlowInfo _tmp4EF;struct _tuple15 _tmp4ED=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4B8,_tmp4C8);_tmp4EE=_tmp4ED.f1;_tmp4EF=_tmp4ED.f2;switch(_tmp48C){
case Cyc_Absyn_Eq: _LL2AD: {struct _tuple15 _tmp8DF;return(_tmp8DF.f1=_tmp4EF,((
_tmp8DF.f2=_tmp4EE,_tmp8DF)));}case Cyc_Absyn_Neq: _LL2AE: {struct _tuple15 _tmp8E0;
return(_tmp8E0.f1=_tmp4EE,((_tmp8E0.f2=_tmp4EF,_tmp8E0)));}default: _LL2AF: {
struct Cyc_Core_Impossible_struct _tmp8E6;const char*_tmp8E5;struct Cyc_Core_Impossible_struct*
_tmp8E4;(int)_throw((void*)((_tmp8E4=_cycalloc(sizeof(*_tmp8E4)),((_tmp8E4[0]=((
_tmp8E6.tag=Cyc_Core_Impossible,((_tmp8E6.f1=((_tmp8E5="anal_test, zero-split",
_tag_dyneither(_tmp8E5,sizeof(char),22))),_tmp8E6)))),_tmp8E4)))));}}}_LL299:
_tmp4C9=_tmp4BE.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4CA=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4C9;if(_tmp4CA->tag != 0)goto _LL29B;};_tmp4CB=_tmp4BE.f2;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4CC=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4CB;if(_tmp4CC->tag != 0)goto
_LL29B;};_LL29A: if(_tmp48C == Cyc_Absyn_Eq){struct _tuple15 _tmp8E7;return(_tmp8E7.f1=
f,((_tmp8E7.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8E7)));}else{struct _tuple15 _tmp8E8;
return(_tmp8E8.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8E8.f2=f,_tmp8E8)));}_LL29B:
_tmp4CD=_tmp4BE.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4CE=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4CD;if(_tmp4CE->tag != 0)goto _LL29D;};_tmp4CF=_tmp4BE.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4D0=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4CF;if(_tmp4D0->tag != 1)
goto _LL29D;};_LL29C: goto _LL29E;_LL29D: _tmp4D1=_tmp4BE.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4D2=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4D1;if(_tmp4D2->tag != 0)goto
_LL29F;};_tmp4D3=_tmp4BE.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp4D4=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4D3;if(_tmp4D4->tag != 2)goto _LL29F;};
_LL29E: goto _LL2A0;_LL29F: _tmp4D5=_tmp4BE.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4D6=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4D5;if(_tmp4D6->tag != 0)goto
_LL2A1;};_tmp4D7=_tmp4BE.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp4D8=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4D7;if(_tmp4D8->tag != 5)goto _LL2A1;};
_LL2A0: goto _LL2A2;_LL2A1: _tmp4D9=_tmp4BE.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4DA=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4D9;if(_tmp4DA->tag != 1)
goto _LL2A3;};_tmp4DB=_tmp4BE.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4DC=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4DB;if(_tmp4DC->tag != 0)goto _LL2A3;};
_LL2A2: goto _LL2A4;_LL2A3: _tmp4DD=_tmp4BE.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4DE=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4DD;if(_tmp4DE->tag != 2)
goto _LL2A5;};_tmp4DF=_tmp4BE.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4E0=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4DF;if(_tmp4E0->tag != 0)goto _LL2A5;};
_LL2A4: goto _LL2A6;_LL2A5: _tmp4E1=_tmp4BE.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp4E2=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4E1;if(_tmp4E2->tag != 5)goto
_LL2A7;};_tmp4E3=_tmp4BE.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4E4=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4E3;if(_tmp4E4->tag != 0)goto _LL2A7;};_LL2A6: if(_tmp48C == Cyc_Absyn_Neq){
struct _tuple15 _tmp8E9;return(_tmp8E9.f1=f,((_tmp8E9.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp8E9)));}else{struct _tuple15 _tmp8EA;return(_tmp8EA.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp8EA.f2=f,_tmp8EA)));}_LL2A7:;_LL2A8: goto _LL294;_LL294:;}{struct _tuple0
_tmp8EB;struct _tuple0 _tmp4FA=(_tmp8EB.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4B7->topt))->v),((_tmp8EB.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4B8->topt))->v),_tmp8EB)));void*_tmp4FB;enum Cyc_Absyn_Sign
_tmp4FD;void*_tmp4FE;enum Cyc_Absyn_Sign _tmp500;void*_tmp501;void*_tmp503;_LL2B2:
_tmp4FB=_tmp4FA.f1;{struct Cyc_Absyn_IntType_struct*_tmp4FC=(struct Cyc_Absyn_IntType_struct*)
_tmp4FB;if(_tmp4FC->tag != 6)goto _LL2B4;else{_tmp4FD=_tmp4FC->f1;if(_tmp4FD != Cyc_Absyn_Unsigned)
goto _LL2B4;}};_LL2B3: goto _LL2B5;_LL2B4: _tmp4FE=_tmp4FA.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4FF=(struct Cyc_Absyn_IntType_struct*)_tmp4FE;if(_tmp4FF->tag != 6)goto _LL2B6;
else{_tmp500=_tmp4FF->f1;if(_tmp500 != Cyc_Absyn_Unsigned)goto _LL2B6;}};_LL2B5:
goto _LL2B7;_LL2B6: _tmp501=_tmp4FA.f1;{struct Cyc_Absyn_TagType_struct*_tmp502=(
struct Cyc_Absyn_TagType_struct*)_tmp501;if(_tmp502->tag != 20)goto _LL2B8;};_LL2B7:
goto _LL2B9;_LL2B8: _tmp503=_tmp4FA.f2;{struct Cyc_Absyn_TagType_struct*_tmp504=(
struct Cyc_Absyn_TagType_struct*)_tmp503;if(_tmp504->tag != 20)goto _LL2BA;};_LL2B9:
goto _LL2B1;_LL2BA:;_LL2BB: {struct _tuple15 _tmp8EC;return(_tmp8EC.f1=f,((_tmp8EC.f2=
f,_tmp8EC)));}_LL2B1:;}switch(_tmp48C){case Cyc_Absyn_Eq: _LL2BC: goto _LL2BD;case
Cyc_Absyn_Neq: _LL2BD: goto _LL2BE;case Cyc_Absyn_Gt: _LL2BE: goto _LL2BF;case Cyc_Absyn_Gte:
_LL2BF: {struct _tuple15 _tmp8ED;return(_tmp8ED.f1=f,((_tmp8ED.f2=f,_tmp8ED)));}
case Cyc_Absyn_Lt: _LL2C0: {union Cyc_CfFlowInfo_FlowInfo _tmp507=f;union Cyc_CfFlowInfo_FlowInfo
_tmp508=f;{union Cyc_CfFlowInfo_FlowInfo _tmp509=_tmp507;int _tmp50A;struct _tuple12
_tmp50B;struct Cyc_Dict_Dict _tmp50C;_LL2C3: if((_tmp509.BottomFL).tag != 1)goto
_LL2C5;_tmp50A=(int)(_tmp509.BottomFL).val;_LL2C4: {struct Cyc_Core_Impossible_struct
_tmp8F3;const char*_tmp8F2;struct Cyc_Core_Impossible_struct*_tmp8F1;(int)_throw((
void*)((_tmp8F1=_cycalloc(sizeof(*_tmp8F1)),((_tmp8F1[0]=((_tmp8F3.tag=Cyc_Core_Impossible,((
_tmp8F3.f1=((_tmp8F2="anal_test, Lt",_tag_dyneither(_tmp8F2,sizeof(char),14))),
_tmp8F3)))),_tmp8F1)))));}_LL2C5: if((_tmp509.ReachableFL).tag != 2)goto _LL2C2;
_tmp50B=(struct _tuple12)(_tmp509.ReachableFL).val;_tmp50C=_tmp50B.f1;_LL2C6:
_tmp4B4=_tmp50C;_LL2C2:;}{void*_tmp510=_tmp4B7->r;void*_tmp512;struct Cyc_Absyn_Vardecl*
_tmp514;void*_tmp516;struct Cyc_Absyn_Vardecl*_tmp518;void*_tmp51A;struct Cyc_Absyn_Vardecl*
_tmp51C;void*_tmp51E;struct Cyc_Absyn_Vardecl*_tmp520;_LL2C8:{struct Cyc_Absyn_Var_e_struct*
_tmp511=(struct Cyc_Absyn_Var_e_struct*)_tmp510;if(_tmp511->tag != 1)goto _LL2CA;
else{_tmp512=(void*)_tmp511->f2;{struct Cyc_Absyn_Global_b_struct*_tmp513=(struct
Cyc_Absyn_Global_b_struct*)_tmp512;if(_tmp513->tag != 1)goto _LL2CA;else{_tmp514=
_tmp513->f1;}};}}if(!(!_tmp514->escapes))goto _LL2CA;_LL2C9: _tmp518=_tmp514;goto
_LL2CB;_LL2CA:{struct Cyc_Absyn_Var_e_struct*_tmp515=(struct Cyc_Absyn_Var_e_struct*)
_tmp510;if(_tmp515->tag != 1)goto _LL2CC;else{_tmp516=(void*)_tmp515->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp517=(struct Cyc_Absyn_Local_b_struct*)_tmp516;if(
_tmp517->tag != 4)goto _LL2CC;else{_tmp518=_tmp517->f1;}};}}if(!(!_tmp518->escapes))
goto _LL2CC;_LL2CB: _tmp51C=_tmp518;goto _LL2CD;_LL2CC:{struct Cyc_Absyn_Var_e_struct*
_tmp519=(struct Cyc_Absyn_Var_e_struct*)_tmp510;if(_tmp519->tag != 1)goto _LL2CE;
else{_tmp51A=(void*)_tmp519->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp51B=(struct Cyc_Absyn_Pat_b_struct*)
_tmp51A;if(_tmp51B->tag != 5)goto _LL2CE;else{_tmp51C=_tmp51B->f1;}};}}if(!(!
_tmp51C->escapes))goto _LL2CE;_LL2CD: _tmp520=_tmp51C;goto _LL2CF;_LL2CE:{struct Cyc_Absyn_Var_e_struct*
_tmp51D=(struct Cyc_Absyn_Var_e_struct*)_tmp510;if(_tmp51D->tag != 1)goto _LL2D0;
else{_tmp51E=(void*)_tmp51D->f2;{struct Cyc_Absyn_Param_b_struct*_tmp51F=(struct
Cyc_Absyn_Param_b_struct*)_tmp51E;if(_tmp51F->tag != 3)goto _LL2D0;else{_tmp520=
_tmp51F->f1;}};}}if(!(!_tmp520->escapes))goto _LL2D0;_LL2CF: {void*_tmp521=
_tmp4B8->r;void*_tmp523;struct Cyc_Absyn_Vardecl*_tmp525;void*_tmp527;struct Cyc_Absyn_Vardecl*
_tmp529;void*_tmp52B;struct Cyc_Absyn_Vardecl*_tmp52D;void*_tmp52F;struct Cyc_Absyn_Vardecl*
_tmp531;union Cyc_Absyn_Cnst _tmp533;struct _tuple6 _tmp534;int _tmp535;struct Cyc_Absyn_Exp*
_tmp537;struct Cyc_Absyn_Exp _tmp538;void*_tmp539;union Cyc_Absyn_Cnst _tmp53B;
struct _tuple6 _tmp53C;int _tmp53D;enum Cyc_Absyn_Primop _tmp53F;struct Cyc_List_List*
_tmp540;struct Cyc_List_List _tmp541;struct Cyc_Absyn_Exp*_tmp542;_LL2D3:{struct Cyc_Absyn_Var_e_struct*
_tmp522=(struct Cyc_Absyn_Var_e_struct*)_tmp521;if(_tmp522->tag != 1)goto _LL2D5;
else{_tmp523=(void*)_tmp522->f2;{struct Cyc_Absyn_Global_b_struct*_tmp524=(struct
Cyc_Absyn_Global_b_struct*)_tmp523;if(_tmp524->tag != 1)goto _LL2D5;else{_tmp525=
_tmp524->f1;}};}}if(!(!_tmp525->escapes))goto _LL2D5;_LL2D4: _tmp529=_tmp525;goto
_LL2D6;_LL2D5:{struct Cyc_Absyn_Var_e_struct*_tmp526=(struct Cyc_Absyn_Var_e_struct*)
_tmp521;if(_tmp526->tag != 1)goto _LL2D7;else{_tmp527=(void*)_tmp526->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp528=(struct Cyc_Absyn_Local_b_struct*)_tmp527;if(
_tmp528->tag != 4)goto _LL2D7;else{_tmp529=_tmp528->f1;}};}}if(!(!_tmp529->escapes))
goto _LL2D7;_LL2D6: _tmp52D=_tmp529;goto _LL2D8;_LL2D7:{struct Cyc_Absyn_Var_e_struct*
_tmp52A=(struct Cyc_Absyn_Var_e_struct*)_tmp521;if(_tmp52A->tag != 1)goto _LL2D9;
else{_tmp52B=(void*)_tmp52A->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp52C=(struct Cyc_Absyn_Pat_b_struct*)
_tmp52B;if(_tmp52C->tag != 5)goto _LL2D9;else{_tmp52D=_tmp52C->f1;}};}}if(!(!
_tmp52D->escapes))goto _LL2D9;_LL2D8: _tmp531=_tmp52D;goto _LL2DA;_LL2D9:{struct Cyc_Absyn_Var_e_struct*
_tmp52E=(struct Cyc_Absyn_Var_e_struct*)_tmp521;if(_tmp52E->tag != 1)goto _LL2DB;
else{_tmp52F=(void*)_tmp52E->f2;{struct Cyc_Absyn_Param_b_struct*_tmp530=(struct
Cyc_Absyn_Param_b_struct*)_tmp52F;if(_tmp530->tag != 3)goto _LL2DB;else{_tmp531=
_tmp530->f1;}};}}if(!(!_tmp531->escapes))goto _LL2DB;_LL2DA: {struct _RegionHandle*
_tmp543=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8F6;struct Cyc_List_List*
_tmp8F5;struct Cyc_List_List*_tmp544=(_tmp8F5=_region_malloc(_tmp543,sizeof(*
_tmp8F5)),((_tmp8F5->hd=((_tmp8F6=_region_malloc(_tmp543,sizeof(*_tmp8F6)),((
_tmp8F6->vd=_tmp520,((_tmp8F6->rop=Cyc_CfFlowInfo_LessVar(_tmp531,_tmp531->type),
_tmp8F6)))))),((_tmp8F5->tl=_tmp4B5,_tmp8F5)))));struct _tuple15 _tmp8F7;return(
_tmp8F7.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4B4,_tmp544),((_tmp8F7.f2=_tmp508,
_tmp8F7)));}_LL2DB: {struct Cyc_Absyn_Const_e_struct*_tmp532=(struct Cyc_Absyn_Const_e_struct*)
_tmp521;if(_tmp532->tag != 0)goto _LL2DD;else{_tmp533=_tmp532->f1;if((_tmp533.Int_c).tag
!= 4)goto _LL2DD;_tmp534=(struct _tuple6)(_tmp533.Int_c).val;_tmp535=_tmp534.f2;}}
_LL2DC: _tmp53D=_tmp535;goto _LL2DE;_LL2DD: {struct Cyc_Absyn_Cast_e_struct*_tmp536=(
struct Cyc_Absyn_Cast_e_struct*)_tmp521;if(_tmp536->tag != 15)goto _LL2DF;else{
_tmp537=_tmp536->f2;_tmp538=*_tmp537;_tmp539=_tmp538.r;{struct Cyc_Absyn_Const_e_struct*
_tmp53A=(struct Cyc_Absyn_Const_e_struct*)_tmp539;if(_tmp53A->tag != 0)goto _LL2DF;
else{_tmp53B=_tmp53A->f1;if((_tmp53B.Int_c).tag != 4)goto _LL2DF;_tmp53C=(struct
_tuple6)(_tmp53B.Int_c).val;_tmp53D=_tmp53C.f2;}};}}_LL2DE: {struct _RegionHandle*
_tmp548=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8FA;struct Cyc_List_List*
_tmp8F9;struct Cyc_List_List*_tmp549=(_tmp8F9=_region_malloc(_tmp548,sizeof(*
_tmp8F9)),((_tmp8F9->hd=((_tmp8FA=_region_malloc(_tmp548,sizeof(*_tmp8FA)),((
_tmp8FA->vd=_tmp520,((_tmp8FA->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp53D),_tmp8FA)))))),((_tmp8F9->tl=_tmp4B5,_tmp8F9)))));struct _tuple15 _tmp8FB;
return(_tmp8FB.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4B4,_tmp549),((_tmp8FB.f2=
_tmp508,_tmp8FB)));}_LL2DF: {struct Cyc_Absyn_Primop_e_struct*_tmp53E=(struct Cyc_Absyn_Primop_e_struct*)
_tmp521;if(_tmp53E->tag != 3)goto _LL2E1;else{_tmp53F=_tmp53E->f1;_tmp540=_tmp53E->f2;
if(_tmp540 == 0)goto _LL2E1;_tmp541=*_tmp540;_tmp542=(struct Cyc_Absyn_Exp*)_tmp541.hd;}}
_LL2E0: {void*_tmp54D=_tmp542->r;void*_tmp54F;struct Cyc_Absyn_Vardecl*_tmp551;
void*_tmp553;struct Cyc_Absyn_Vardecl*_tmp555;void*_tmp557;struct Cyc_Absyn_Vardecl*
_tmp559;void*_tmp55B;struct Cyc_Absyn_Vardecl*_tmp55D;_LL2E4:{struct Cyc_Absyn_Var_e_struct*
_tmp54E=(struct Cyc_Absyn_Var_e_struct*)_tmp54D;if(_tmp54E->tag != 1)goto _LL2E6;
else{_tmp54F=(void*)_tmp54E->f2;{struct Cyc_Absyn_Global_b_struct*_tmp550=(struct
Cyc_Absyn_Global_b_struct*)_tmp54F;if(_tmp550->tag != 1)goto _LL2E6;else{_tmp551=
_tmp550->f1;}};}}if(!(!_tmp551->escapes))goto _LL2E6;_LL2E5: _tmp555=_tmp551;goto
_LL2E7;_LL2E6:{struct Cyc_Absyn_Var_e_struct*_tmp552=(struct Cyc_Absyn_Var_e_struct*)
_tmp54D;if(_tmp552->tag != 1)goto _LL2E8;else{_tmp553=(void*)_tmp552->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp554=(struct Cyc_Absyn_Local_b_struct*)_tmp553;if(
_tmp554->tag != 4)goto _LL2E8;else{_tmp555=_tmp554->f1;}};}}if(!(!_tmp555->escapes))
goto _LL2E8;_LL2E7: _tmp559=_tmp555;goto _LL2E9;_LL2E8:{struct Cyc_Absyn_Var_e_struct*
_tmp556=(struct Cyc_Absyn_Var_e_struct*)_tmp54D;if(_tmp556->tag != 1)goto _LL2EA;
else{_tmp557=(void*)_tmp556->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp558=(struct Cyc_Absyn_Pat_b_struct*)
_tmp557;if(_tmp558->tag != 5)goto _LL2EA;else{_tmp559=_tmp558->f1;}};}}if(!(!
_tmp559->escapes))goto _LL2EA;_LL2E9: _tmp55D=_tmp559;goto _LL2EB;_LL2EA:{struct Cyc_Absyn_Var_e_struct*
_tmp55A=(struct Cyc_Absyn_Var_e_struct*)_tmp54D;if(_tmp55A->tag != 1)goto _LL2EC;
else{_tmp55B=(void*)_tmp55A->f2;{struct Cyc_Absyn_Param_b_struct*_tmp55C=(struct
Cyc_Absyn_Param_b_struct*)_tmp55B;if(_tmp55C->tag != 3)goto _LL2EC;else{_tmp55D=
_tmp55C->f1;}};}}if(!(!_tmp55D->escapes))goto _LL2EC;_LL2EB: {struct _RegionHandle*
_tmp55E=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8FE;struct Cyc_List_List*
_tmp8FD;struct Cyc_List_List*_tmp55F=(_tmp8FD=_region_malloc(_tmp55E,sizeof(*
_tmp8FD)),((_tmp8FD->hd=((_tmp8FE=_region_malloc(_tmp55E,sizeof(*_tmp8FE)),((
_tmp8FE->vd=_tmp520,((_tmp8FE->rop=Cyc_CfFlowInfo_LessNumelts(_tmp55D),_tmp8FE)))))),((
_tmp8FD->tl=_tmp4B5,_tmp8FD)))));struct _tuple15 _tmp8FF;return(_tmp8FF.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4B4,_tmp55F),((_tmp8FF.f2=_tmp508,_tmp8FF)));}_LL2EC:;_LL2ED: {struct
_tuple15 _tmp900;return(_tmp900.f1=_tmp507,((_tmp900.f2=_tmp508,_tmp900)));}
_LL2E3:;}_LL2E1:;_LL2E2: {struct _tuple15 _tmp901;return(_tmp901.f1=_tmp507,((
_tmp901.f2=_tmp508,_tmp901)));}_LL2D2:;}_LL2D0:;_LL2D1: {struct _tuple15 _tmp902;
return(_tmp902.f1=_tmp507,((_tmp902.f2=_tmp508,_tmp902)));}_LL2C7:;};}case Cyc_Absyn_Lte:
_LL2C1: {union Cyc_CfFlowInfo_FlowInfo _tmp566=f;union Cyc_CfFlowInfo_FlowInfo
_tmp567=f;{union Cyc_CfFlowInfo_FlowInfo _tmp568=_tmp566;int _tmp569;struct _tuple12
_tmp56A;struct Cyc_Dict_Dict _tmp56B;_LL2F0: if((_tmp568.BottomFL).tag != 1)goto
_LL2F2;_tmp569=(int)(_tmp568.BottomFL).val;_LL2F1: {struct Cyc_Core_Impossible_struct
_tmp908;const char*_tmp907;struct Cyc_Core_Impossible_struct*_tmp906;(int)_throw((
void*)((_tmp906=_cycalloc(sizeof(*_tmp906)),((_tmp906[0]=((_tmp908.tag=Cyc_Core_Impossible,((
_tmp908.f1=((_tmp907="anal_test, Lte",_tag_dyneither(_tmp907,sizeof(char),15))),
_tmp908)))),_tmp906)))));}_LL2F2: if((_tmp568.ReachableFL).tag != 2)goto _LL2EF;
_tmp56A=(struct _tuple12)(_tmp568.ReachableFL).val;_tmp56B=_tmp56A.f1;_LL2F3:
_tmp4B4=_tmp56B;_LL2EF:;}{void*_tmp56F=_tmp4B7->r;void*_tmp571;struct Cyc_Absyn_Vardecl*
_tmp573;void*_tmp575;struct Cyc_Absyn_Vardecl*_tmp577;void*_tmp579;struct Cyc_Absyn_Vardecl*
_tmp57B;void*_tmp57D;struct Cyc_Absyn_Vardecl*_tmp57F;_LL2F5:{struct Cyc_Absyn_Var_e_struct*
_tmp570=(struct Cyc_Absyn_Var_e_struct*)_tmp56F;if(_tmp570->tag != 1)goto _LL2F7;
else{_tmp571=(void*)_tmp570->f2;{struct Cyc_Absyn_Global_b_struct*_tmp572=(struct
Cyc_Absyn_Global_b_struct*)_tmp571;if(_tmp572->tag != 1)goto _LL2F7;else{_tmp573=
_tmp572->f1;}};}}if(!(!_tmp573->escapes))goto _LL2F7;_LL2F6: _tmp577=_tmp573;goto
_LL2F8;_LL2F7:{struct Cyc_Absyn_Var_e_struct*_tmp574=(struct Cyc_Absyn_Var_e_struct*)
_tmp56F;if(_tmp574->tag != 1)goto _LL2F9;else{_tmp575=(void*)_tmp574->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp576=(struct Cyc_Absyn_Local_b_struct*)_tmp575;if(
_tmp576->tag != 4)goto _LL2F9;else{_tmp577=_tmp576->f1;}};}}if(!(!_tmp577->escapes))
goto _LL2F9;_LL2F8: _tmp57B=_tmp577;goto _LL2FA;_LL2F9:{struct Cyc_Absyn_Var_e_struct*
_tmp578=(struct Cyc_Absyn_Var_e_struct*)_tmp56F;if(_tmp578->tag != 1)goto _LL2FB;
else{_tmp579=(void*)_tmp578->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp57A=(struct Cyc_Absyn_Pat_b_struct*)
_tmp579;if(_tmp57A->tag != 5)goto _LL2FB;else{_tmp57B=_tmp57A->f1;}};}}if(!(!
_tmp57B->escapes))goto _LL2FB;_LL2FA: _tmp57F=_tmp57B;goto _LL2FC;_LL2FB:{struct Cyc_Absyn_Var_e_struct*
_tmp57C=(struct Cyc_Absyn_Var_e_struct*)_tmp56F;if(_tmp57C->tag != 1)goto _LL2FD;
else{_tmp57D=(void*)_tmp57C->f2;{struct Cyc_Absyn_Param_b_struct*_tmp57E=(struct
Cyc_Absyn_Param_b_struct*)_tmp57D;if(_tmp57E->tag != 3)goto _LL2FD;else{_tmp57F=
_tmp57E->f1;}};}}if(!(!_tmp57F->escapes))goto _LL2FD;_LL2FC: {void*_tmp580=
_tmp4B8->r;union Cyc_Absyn_Cnst _tmp582;struct _tuple6 _tmp583;int _tmp584;struct Cyc_Absyn_Exp*
_tmp586;struct Cyc_Absyn_Exp _tmp587;void*_tmp588;union Cyc_Absyn_Cnst _tmp58A;
struct _tuple6 _tmp58B;int _tmp58C;enum Cyc_Absyn_Primop _tmp58E;struct Cyc_List_List*
_tmp58F;struct Cyc_List_List _tmp590;struct Cyc_Absyn_Exp*_tmp591;_LL300: {struct
Cyc_Absyn_Const_e_struct*_tmp581=(struct Cyc_Absyn_Const_e_struct*)_tmp580;if(
_tmp581->tag != 0)goto _LL302;else{_tmp582=_tmp581->f1;if((_tmp582.Int_c).tag != 4)
goto _LL302;_tmp583=(struct _tuple6)(_tmp582.Int_c).val;_tmp584=_tmp583.f2;}}
_LL301: _tmp58C=_tmp584;goto _LL303;_LL302: {struct Cyc_Absyn_Cast_e_struct*_tmp585=(
struct Cyc_Absyn_Cast_e_struct*)_tmp580;if(_tmp585->tag != 15)goto _LL304;else{
_tmp586=_tmp585->f2;_tmp587=*_tmp586;_tmp588=_tmp587.r;{struct Cyc_Absyn_Const_e_struct*
_tmp589=(struct Cyc_Absyn_Const_e_struct*)_tmp588;if(_tmp589->tag != 0)goto _LL304;
else{_tmp58A=_tmp589->f1;if((_tmp58A.Int_c).tag != 4)goto _LL304;_tmp58B=(struct
_tuple6)(_tmp58A.Int_c).val;_tmp58C=_tmp58B.f2;}};}}_LL303: {struct _RegionHandle*
_tmp592=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp90B;struct Cyc_List_List*
_tmp90A;struct Cyc_List_List*_tmp593=(_tmp90A=_region_malloc(_tmp592,sizeof(*
_tmp90A)),((_tmp90A->hd=((_tmp90B=_region_malloc(_tmp592,sizeof(*_tmp90B)),((
_tmp90B->vd=_tmp57F,((_tmp90B->rop=Cyc_CfFlowInfo_LessEqConst((unsigned int)
_tmp58C),_tmp90B)))))),((_tmp90A->tl=_tmp4B5,_tmp90A)))));struct _tuple15 _tmp90C;
return(_tmp90C.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4B4,_tmp593),((_tmp90C.f2=
_tmp567,_tmp90C)));}_LL304: {struct Cyc_Absyn_Primop_e_struct*_tmp58D=(struct Cyc_Absyn_Primop_e_struct*)
_tmp580;if(_tmp58D->tag != 3)goto _LL306;else{_tmp58E=_tmp58D->f1;_tmp58F=_tmp58D->f2;
if(_tmp58F == 0)goto _LL306;_tmp590=*_tmp58F;_tmp591=(struct Cyc_Absyn_Exp*)_tmp590.hd;}}
_LL305: {void*_tmp597=_tmp591->r;void*_tmp599;struct Cyc_Absyn_Vardecl*_tmp59B;
void*_tmp59D;struct Cyc_Absyn_Vardecl*_tmp59F;void*_tmp5A1;struct Cyc_Absyn_Vardecl*
_tmp5A3;void*_tmp5A5;struct Cyc_Absyn_Vardecl*_tmp5A7;_LL309:{struct Cyc_Absyn_Var_e_struct*
_tmp598=(struct Cyc_Absyn_Var_e_struct*)_tmp597;if(_tmp598->tag != 1)goto _LL30B;
else{_tmp599=(void*)_tmp598->f2;{struct Cyc_Absyn_Global_b_struct*_tmp59A=(struct
Cyc_Absyn_Global_b_struct*)_tmp599;if(_tmp59A->tag != 1)goto _LL30B;else{_tmp59B=
_tmp59A->f1;}};}}if(!(!_tmp59B->escapes))goto _LL30B;_LL30A: _tmp59F=_tmp59B;goto
_LL30C;_LL30B:{struct Cyc_Absyn_Var_e_struct*_tmp59C=(struct Cyc_Absyn_Var_e_struct*)
_tmp597;if(_tmp59C->tag != 1)goto _LL30D;else{_tmp59D=(void*)_tmp59C->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp59E=(struct Cyc_Absyn_Local_b_struct*)_tmp59D;if(
_tmp59E->tag != 4)goto _LL30D;else{_tmp59F=_tmp59E->f1;}};}}if(!(!_tmp59F->escapes))
goto _LL30D;_LL30C: _tmp5A3=_tmp59F;goto _LL30E;_LL30D:{struct Cyc_Absyn_Var_e_struct*
_tmp5A0=(struct Cyc_Absyn_Var_e_struct*)_tmp597;if(_tmp5A0->tag != 1)goto _LL30F;
else{_tmp5A1=(void*)_tmp5A0->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp5A2=(struct Cyc_Absyn_Pat_b_struct*)
_tmp5A1;if(_tmp5A2->tag != 5)goto _LL30F;else{_tmp5A3=_tmp5A2->f1;}};}}if(!(!
_tmp5A3->escapes))goto _LL30F;_LL30E: _tmp5A7=_tmp5A3;goto _LL310;_LL30F:{struct Cyc_Absyn_Var_e_struct*
_tmp5A4=(struct Cyc_Absyn_Var_e_struct*)_tmp597;if(_tmp5A4->tag != 1)goto _LL311;
else{_tmp5A5=(void*)_tmp5A4->f2;{struct Cyc_Absyn_Param_b_struct*_tmp5A6=(struct
Cyc_Absyn_Param_b_struct*)_tmp5A5;if(_tmp5A6->tag != 3)goto _LL311;else{_tmp5A7=
_tmp5A6->f1;}};}}if(!(!_tmp5A7->escapes))goto _LL311;_LL310: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp5A8=env->fenv;struct Cyc_CfFlowInfo_Reln*_tmp90F;struct Cyc_List_List*_tmp90E;
struct Cyc_List_List*_tmp5A9=(_tmp90E=_region_malloc(_tmp5A8->r,sizeof(*_tmp90E)),((
_tmp90E->hd=((_tmp90F=_region_malloc(_tmp5A8->r,sizeof(*_tmp90F)),((_tmp90F->vd=
_tmp57F,((_tmp90F->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp5A7),_tmp90F)))))),((
_tmp90E->tl=_tmp4B5,_tmp90E)))));struct _tuple15 _tmp910;return(_tmp910.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4B4,_tmp5A9),((_tmp910.f2=_tmp567,_tmp910)));}_LL311:;_LL312: {struct
_tuple15 _tmp911;return(_tmp911.f1=_tmp566,((_tmp911.f2=_tmp567,_tmp911)));}
_LL308:;}_LL306:;_LL307: {struct _tuple15 _tmp912;return(_tmp912.f1=_tmp566,((
_tmp912.f2=_tmp567,_tmp912)));}_LL2FF:;}_LL2FD:;_LL2FE: {struct _tuple15 _tmp913;
return(_tmp913.f1=_tmp566,((_tmp913.f2=_tmp567,_tmp913)));}_LL2F4:;};}default:
_LL2EE: {struct _tuple15 _tmp914;return(_tmp914.f1=f,((_tmp914.f2=f,_tmp914)));}}}
_LL28F:;};}_LL28D:;_LL28E: goto _LL280;_LL280:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp5B2;void*_tmp5B3;struct _tuple13 _tmp5B1=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,e);_tmp5B2=_tmp5B1.f1;_tmp5B3=_tmp5B1.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp5B4=_tmp5B2;int _tmp5B5;struct _tuple12 _tmp5B6;struct Cyc_Dict_Dict _tmp5B7;
_LL315: if((_tmp5B4.BottomFL).tag != 1)goto _LL317;_tmp5B5=(int)(_tmp5B4.BottomFL).val;
_LL316: {struct _tuple15 _tmp915;return(_tmp915.f1=_tmp5B2,((_tmp915.f2=_tmp5B2,
_tmp915)));}_LL317: if((_tmp5B4.ReachableFL).tag != 2)goto _LL314;_tmp5B6=(struct
_tuple12)(_tmp5B4.ReachableFL).val;_tmp5B7=_tmp5B6.f1;_LL318: {void*_tmp5B9=
_tmp5B3;enum Cyc_CfFlowInfo_InitLevel _tmp5BF;enum Cyc_CfFlowInfo_InitLevel
_tmp5C1;enum Cyc_CfFlowInfo_InitLevel _tmp5C4;_LL31A: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5BA=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5B9;if(_tmp5BA->tag != 0)goto
_LL31C;}_LL31B: {struct _tuple15 _tmp916;return(_tmp916.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp916.f2=_tmp5B2,_tmp916)));}_LL31C: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp5BB=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp5B9;if(_tmp5BB->tag != 2)
goto _LL31E;}_LL31D: goto _LL31F;_LL31E: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp5BC=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp5B9;if(_tmp5BC->tag != 1)
goto _LL320;}_LL31F: goto _LL321;_LL320: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp5BD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp5B9;if(_tmp5BD->tag != 5)goto
_LL322;}_LL321: {struct _tuple15 _tmp917;return(_tmp917.f1=_tmp5B2,((_tmp917.f2=
Cyc_CfFlowInfo_BottomFL(),_tmp917)));}_LL322: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp5BE=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5B9;if(_tmp5BE->tag != 3)goto
_LL324;else{_tmp5BF=_tmp5BE->f1;if(_tmp5BF != Cyc_CfFlowInfo_NoneIL)goto _LL324;}}
_LL323: goto _LL325;_LL324: {struct Cyc_CfFlowInfo_Esc_struct*_tmp5C0=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp5B9;if(_tmp5C0->tag != 4)goto _LL326;else{_tmp5C1=_tmp5C0->f1;if(_tmp5C1 != Cyc_CfFlowInfo_NoneIL)
goto _LL326;}}_LL325: goto _LL327;_LL326: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp5C2=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp5B9;if(_tmp5C2->tag != 7)goto
_LL328;}_LL327:{const char*_tmp91A;void*_tmp919;(_tmp919=0,Cyc_Tcutil_terr(e->loc,((
_tmp91A="expression may not be initialized",_tag_dyneither(_tmp91A,sizeof(char),
34))),_tag_dyneither(_tmp919,sizeof(void*),0)));}{struct _tuple15 _tmp91B;return(
_tmp91B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp91B.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp91B)));};_LL328: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5C3=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp5B9;if(_tmp5C3->tag != 3)goto _LL32A;else{_tmp5C4=_tmp5C3->f1;}}_LL329: return
Cyc_NewControlFlow_splitzero(env,inflow,_tmp5B2,e,_tmp5C4);_LL32A: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp5C5=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp5B9;if(_tmp5C5->tag != 4)goto _LL32C;}
_LL32B: {struct _tuple15 _tmp91C;return(_tmp91C.f1=_tmp5B2,((_tmp91C.f2=_tmp5B2,
_tmp91C)));}_LL32C: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5C6=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp5B9;if(_tmp5C6->tag != 6)goto _LL319;}_LL32D: {struct Cyc_Core_Impossible_struct
_tmp922;const char*_tmp921;struct Cyc_Core_Impossible_struct*_tmp920;(int)_throw((
void*)((_tmp920=_cycalloc(sizeof(*_tmp920)),((_tmp920[0]=((_tmp922.tag=Cyc_Core_Impossible,((
_tmp922.f1=((_tmp921="anal_test",_tag_dyneither(_tmp921,sizeof(char),10))),
_tmp922)))),_tmp920)))));}_LL319:;}_LL314:;};};}struct _tuple22{struct Cyc_Position_Segment*
f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple22*ckenv,void*root,void*rval);static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple22*ckenv,void*root,void*rval){struct _tuple22 _tmp5D1;struct Cyc_Position_Segment*
_tmp5D2;struct Cyc_NewControlFlow_AnalEnv*_tmp5D3;struct Cyc_Dict_Dict _tmp5D4;
struct _tuple22*_tmp5D0=ckenv;_tmp5D1=*_tmp5D0;_tmp5D2=_tmp5D1.f1;_tmp5D3=_tmp5D1.f2;
_tmp5D4=_tmp5D1.f3;{void*_tmp5D5=root;struct Cyc_Absyn_Vardecl*_tmp5D7;_LL32F: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp5D6=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp5D5;if(_tmp5D6->tag != 0)goto _LL331;else{_tmp5D7=_tmp5D6->f1;}}_LL330: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp5D3->fenv)->r,_tmp5D7->type)){void*_tmp5D8=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp5DC;_LL334: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp5D9=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp5D8;if(_tmp5D9->tag != 7)goto _LL336;}_LL335: goto _LL337;_LL336: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5DA=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5D8;if(_tmp5DA->tag != 0)goto
_LL338;}_LL337: goto _LL339;_LL338: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5DB=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5D8;if(_tmp5DB->tag != 3)goto _LL33A;
else{_tmp5DC=_tmp5DB->f1;if(_tmp5DC != Cyc_CfFlowInfo_NoneIL)goto _LL33A;}}_LL339:
goto _LL333;_LL33A:;_LL33B:{const char*_tmp926;void*_tmp925[1];struct Cyc_String_pa_struct
_tmp924;(_tmp924.tag=0,((_tmp924.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp5D7->name)),((_tmp925[0]=& _tmp924,Cyc_Tcutil_warn(
_tmp5D2,((_tmp926="unique pointers reachable from %s may become unreachable",
_tag_dyneither(_tmp926,sizeof(char),57))),_tag_dyneither(_tmp925,sizeof(void*),1)))))));}
goto _LL333;_LL333:;}goto _LL32E;_LL331:;_LL332: goto _LL32E;_LL32E:;};}static void
Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo flow);static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
flow){union Cyc_CfFlowInfo_FlowInfo _tmp5E0=flow;int _tmp5E1;struct _tuple12 _tmp5E2;
struct Cyc_Dict_Dict _tmp5E3;_LL33D: if((_tmp5E0.BottomFL).tag != 1)goto _LL33F;
_tmp5E1=(int)(_tmp5E0.BottomFL).val;_LL33E: return;_LL33F: if((_tmp5E0.ReachableFL).tag
!= 2)goto _LL33C;_tmp5E2=(struct _tuple12)(_tmp5E0.ReachableFL).val;_tmp5E3=
_tmp5E2.f1;_LL340:{struct Cyc_List_List*_tmp5E4=env->param_roots;for(0;_tmp5E4 != 
0;_tmp5E4=_tmp5E4->tl){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp5E3,Cyc_CfFlowInfo_lookup_place(
_tmp5E3,(struct Cyc_CfFlowInfo_Place*)_tmp5E4->hd))!= Cyc_CfFlowInfo_AllIL){const
char*_tmp929;void*_tmp928;(_tmp928=0,Cyc_Tcutil_terr(loc,((_tmp929="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp929,sizeof(char),76))),_tag_dyneither(_tmp928,sizeof(void*),0)));}}}
if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple22 _tmp92A;struct _tuple22
_tmp5E7=(_tmp92A.f1=loc,((_tmp92A.f2=env,((_tmp92A.f3=_tmp5E3,_tmp92A)))));((
void(*)(void(*f)(struct _tuple22*,void*,void*),struct _tuple22*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp5E7,_tmp5E3);}
return;_LL33C:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp5E9=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp5EB;struct Cyc_Core_Opt*_tmp5EC;
struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_Absyn_Stmt*_tmp5EE;struct Cyc_Position_Segment*
_tmp5EF;struct Cyc_Absyn_Switch_clause*_tmp5EA=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp5EB=*_tmp5EA;_tmp5EC=_tmp5EB.pat_vars;_tmp5ED=_tmp5EB.where_clause;
_tmp5EE=_tmp5EB.body;_tmp5EF=_tmp5EB.loc;{union Cyc_CfFlowInfo_FlowInfo
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp5E9,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp5EC))->v,_tmp5E9->unknown_all,_tmp5EF);union
Cyc_CfFlowInfo_FlowInfo clause_outflow;if(_tmp5ED != 0){struct Cyc_Absyn_Exp*wexp=(
struct Cyc_Absyn_Exp*)_tmp5ED;union Cyc_CfFlowInfo_FlowInfo _tmp5F1;union Cyc_CfFlowInfo_FlowInfo
_tmp5F2;struct _tuple15 _tmp5F0=Cyc_NewControlFlow_anal_test(env,clause_inflow,
wexp);_tmp5F1=_tmp5F0.f1;_tmp5F2=_tmp5F0.f2;inflow=_tmp5F2;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp5F1,_tmp5EE);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp5EE);}{union Cyc_CfFlowInfo_FlowInfo _tmp5F3=clause_outflow;int
_tmp5F4;_LL342: if((_tmp5F3.BottomFL).tag != 1)goto _LL344;_tmp5F4=(int)(_tmp5F3.BottomFL).val;
_LL343: goto _LL341;_LL344:;_LL345: if(scs->tl == 0)return clause_outflow;else{if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){const char*
_tmp92D;void*_tmp92C;(_tmp92C=0,Cyc_Tcutil_terr(_tmp5EE->loc,((_tmp92D="switch clause may implicitly fallthru",
_tag_dyneither(_tmp92D,sizeof(char),38))),_tag_dyneither(_tmp92C,sizeof(void*),0)));}
else{const char*_tmp930;void*_tmp92F;(_tmp92F=0,Cyc_Tcutil_warn(_tmp5EE->loc,((
_tmp930="switch clause may implicitly fallthru",_tag_dyneither(_tmp930,sizeof(
char),38))),_tag_dyneither(_tmp92F,sizeof(void*),0)));}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL341;_LL341:;};};}return Cyc_CfFlowInfo_BottomFL();}struct
_tuple23{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;struct Cyc_Position_Segment*
f3;};static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple23*vdenv,
struct Cyc_Absyn_Vardecl*vd);static void Cyc_NewControlFlow_check_dropped_unique_vd(
struct _tuple23*vdenv,struct Cyc_Absyn_Vardecl*vd){struct _tuple23 _tmp5FA;struct Cyc_NewControlFlow_AnalEnv*
_tmp5FB;struct Cyc_Dict_Dict _tmp5FC;struct Cyc_Position_Segment*_tmp5FD;struct
_tuple23*_tmp5F9=vdenv;_tmp5FA=*_tmp5F9;_tmp5FB=_tmp5FA.f1;_tmp5FC=_tmp5FA.f2;
_tmp5FD=_tmp5FA.f3;if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp5FB->fenv)->r,
vd->type)){struct Cyc_CfFlowInfo_VarRoot_struct _tmp933;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp932;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp5FE=(_tmp932=_region_malloc((
_tmp5FB->fenv)->r,sizeof(*_tmp932)),((_tmp932[0]=((_tmp933.tag=0,((_tmp933.f1=vd,
_tmp933)))),_tmp932)));void*_tmp5FF=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
_tmp5FC,(void*)_tmp5FE);void*_tmp600=_tmp5FF;enum Cyc_CfFlowInfo_InitLevel
_tmp604;_LL347: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp601=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp600;if(_tmp601->tag != 7)goto _LL349;}_LL348: goto _LL34A;_LL349: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp602=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp600;if(_tmp602->tag != 0)goto
_LL34B;}_LL34A: goto _LL34C;_LL34B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp603=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp600;if(_tmp603->tag != 3)goto _LL34D;
else{_tmp604=_tmp603->f1;if(_tmp604 != Cyc_CfFlowInfo_NoneIL)goto _LL34D;}}_LL34C:
goto _LL346;_LL34D:;_LL34E:{const char*_tmp937;void*_tmp936[1];struct Cyc_String_pa_struct
_tmp935;(_tmp935.tag=0,((_tmp935.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp936[0]=& _tmp935,Cyc_Tcutil_warn(_tmp5FD,((
_tmp937="unique pointers may still exist after variable %s goes out of scope",
_tag_dyneither(_tmp937,sizeof(char),68))),_tag_dyneither(_tmp936,sizeof(void*),1)))))));}
goto _LL346;_LL346:;}}static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl);static void Cyc_NewControlFlow_check_dropped_unique(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Decl*decl){{union Cyc_CfFlowInfo_FlowInfo _tmp60A=inflow;struct _tuple12
_tmp60B;struct Cyc_Dict_Dict _tmp60C;_LL350: if((_tmp60A.ReachableFL).tag != 2)goto
_LL352;_tmp60B=(struct _tuple12)(_tmp60A.ReachableFL).val;_tmp60C=_tmp60B.f1;
_LL351: {struct _tuple23 _tmp938;struct _tuple23 _tmp60D=(_tmp938.f1=env,((_tmp938.f2=
_tmp60C,((_tmp938.f3=decl->loc,_tmp938)))));struct Cyc_CfFlowInfo_FlowEnv*_tmp60E=
env->fenv;{void*_tmp60F=decl->r;struct Cyc_Absyn_Vardecl*_tmp611;struct Cyc_Core_Opt*
_tmp613;struct Cyc_Core_Opt _tmp614;struct Cyc_List_List*_tmp615;struct Cyc_List_List*
_tmp617;_LL355: {struct Cyc_Absyn_Var_d_struct*_tmp610=(struct Cyc_Absyn_Var_d_struct*)
_tmp60F;if(_tmp610->tag != 0)goto _LL357;else{_tmp611=_tmp610->f1;}}_LL356: Cyc_NewControlFlow_check_dropped_unique_vd(&
_tmp60D,_tmp611);goto _LL354;_LL357: {struct Cyc_Absyn_Let_d_struct*_tmp612=(
struct Cyc_Absyn_Let_d_struct*)_tmp60F;if(_tmp612->tag != 2)goto _LL359;else{
_tmp613=_tmp612->f2;if(_tmp613 == 0)goto _LL359;_tmp614=*_tmp613;_tmp615=(struct
Cyc_List_List*)_tmp614.v;}}_LL358: _tmp617=_tmp615;goto _LL35A;_LL359: {struct Cyc_Absyn_Letv_d_struct*
_tmp616=(struct Cyc_Absyn_Letv_d_struct*)_tmp60F;if(_tmp616->tag != 3)goto _LL35B;
else{_tmp617=_tmp616->f1;}}_LL35A:((void(*)(void(*f)(struct _tuple23*,struct Cyc_Absyn_Vardecl*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,&
_tmp60D,_tmp617);goto _LL354;_LL35B:;_LL35C: goto _LL354;_LL354:;}goto _LL34F;}
_LL352:;_LL353: goto _LL34F;_LL34F:;}return;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp61A;union Cyc_CfFlowInfo_FlowInfo*_tmp61B;struct _tuple16 _tmp619=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp61A=_tmp619.f1;_tmp61B=_tmp619.f2;inflow=*_tmp61B;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp61C=env->fenv;void*_tmp61D=s->r;struct Cyc_Absyn_Exp*_tmp620;struct Cyc_Absyn_Exp*
_tmp622;struct Cyc_Absyn_Exp*_tmp624;struct Cyc_Absyn_Stmt*_tmp626;struct Cyc_Absyn_Stmt*
_tmp627;struct Cyc_Absyn_Exp*_tmp629;struct Cyc_Absyn_Stmt*_tmp62A;struct Cyc_Absyn_Stmt*
_tmp62B;struct _tuple9 _tmp62D;struct Cyc_Absyn_Exp*_tmp62E;struct Cyc_Absyn_Stmt*
_tmp62F;struct Cyc_Absyn_Stmt*_tmp630;struct Cyc_Absyn_Stmt*_tmp632;struct _tuple9
_tmp633;struct Cyc_Absyn_Exp*_tmp634;struct Cyc_Absyn_Stmt*_tmp635;struct Cyc_Absyn_Exp*
_tmp637;struct _tuple9 _tmp638;struct Cyc_Absyn_Exp*_tmp639;struct Cyc_Absyn_Stmt*
_tmp63A;struct _tuple9 _tmp63B;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_Absyn_Stmt*
_tmp63D;struct Cyc_Absyn_Stmt*_tmp63E;struct Cyc_Absyn_Stmt*_tmp640;struct Cyc_List_List*
_tmp642;struct Cyc_Absyn_Switch_clause**_tmp643;struct Cyc_Absyn_Switch_clause*
_tmp644;struct Cyc_Absyn_Stmt*_tmp646;struct Cyc_Absyn_Stmt*_tmp648;struct Cyc_Absyn_Stmt*
_tmp64A;struct Cyc_Absyn_Exp*_tmp64C;struct Cyc_List_List*_tmp64D;struct Cyc_Absyn_Stmt*
_tmp64F;struct Cyc_List_List*_tmp650;struct Cyc_Absyn_Decl*_tmp652;struct Cyc_Absyn_Decl
_tmp653;void*_tmp654;struct Cyc_Absyn_Exp*_tmp656;struct Cyc_Absyn_Tvar*_tmp657;
struct Cyc_Absyn_Vardecl*_tmp658;struct Cyc_Absyn_Stmt*_tmp659;struct Cyc_Absyn_Decl*
_tmp65B;struct Cyc_Absyn_Stmt*_tmp65C;struct Cyc_Absyn_Stmt*_tmp65E;struct Cyc_Absyn_Exp*
_tmp660;_LL35E: {struct Cyc_Absyn_Skip_s_struct*_tmp61E=(struct Cyc_Absyn_Skip_s_struct*)
_tmp61D;if(_tmp61E->tag != 0)goto _LL360;}_LL35F: return inflow;_LL360: {struct Cyc_Absyn_Return_s_struct*
_tmp61F=(struct Cyc_Absyn_Return_s_struct*)_tmp61D;if(_tmp61F->tag != 3)goto _LL362;
else{_tmp620=_tmp61F->f1;if(_tmp620 != 0)goto _LL362;}}_LL361: if(env->noreturn){
const char*_tmp93B;void*_tmp93A;(_tmp93A=0,Cyc_Tcutil_terr(s->loc,((_tmp93B="`noreturn' function might return",
_tag_dyneither(_tmp93B,sizeof(char),33))),_tag_dyneither(_tmp93A,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL362: {struct Cyc_Absyn_Return_s_struct*_tmp621=(struct Cyc_Absyn_Return_s_struct*)
_tmp61D;if(_tmp621->tag != 3)goto _LL364;else{_tmp622=_tmp621->f1;}}_LL363: if(env->noreturn){
const char*_tmp93E;void*_tmp93D;(_tmp93D=0,Cyc_Tcutil_terr(s->loc,((_tmp93E="`noreturn' function might return",
_tag_dyneither(_tmp93E,sizeof(char),33))),_tag_dyneither(_tmp93D,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp666;void*_tmp667;struct _tuple13 _tmp665=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp622));_tmp666=_tmp665.f1;
_tmp667=_tmp665.f2;_tmp666=Cyc_NewControlFlow_use_Rval(env,_tmp622->loc,_tmp666,
_tmp667);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp666);return Cyc_CfFlowInfo_BottomFL();};
_LL364: {struct Cyc_Absyn_Exp_s_struct*_tmp623=(struct Cyc_Absyn_Exp_s_struct*)
_tmp61D;if(_tmp623->tag != 1)goto _LL366;else{_tmp624=_tmp623->f1;}}_LL365: return(
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp624)).f1;_LL366: {struct Cyc_Absyn_Seq_s_struct*
_tmp625=(struct Cyc_Absyn_Seq_s_struct*)_tmp61D;if(_tmp625->tag != 2)goto _LL368;
else{_tmp626=_tmp625->f1;_tmp627=_tmp625->f2;}}_LL367: return Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp626),_tmp627);_LL368: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp628=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp61D;if(_tmp628->tag != 4)goto
_LL36A;else{_tmp629=_tmp628->f1;_tmp62A=_tmp628->f2;_tmp62B=_tmp628->f3;}}_LL369: {
union Cyc_CfFlowInfo_FlowInfo _tmp669;union Cyc_CfFlowInfo_FlowInfo _tmp66A;struct
_tuple15 _tmp668=Cyc_NewControlFlow_anal_test(env,inflow,_tmp629);_tmp669=_tmp668.f1;
_tmp66A=_tmp668.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp66B=Cyc_NewControlFlow_anal_stmt(
env,_tmp66A,_tmp62B);union Cyc_CfFlowInfo_FlowInfo _tmp66C=Cyc_NewControlFlow_anal_stmt(
env,_tmp669,_tmp62A);return Cyc_CfFlowInfo_join_flow(_tmp61C,env->all_changed,
_tmp66C,_tmp66B);};}_LL36A: {struct Cyc_Absyn_While_s_struct*_tmp62C=(struct Cyc_Absyn_While_s_struct*)
_tmp61D;if(_tmp62C->tag != 5)goto _LL36C;else{_tmp62D=_tmp62C->f1;_tmp62E=_tmp62D.f1;
_tmp62F=_tmp62D.f2;_tmp630=_tmp62C->f2;}}_LL36B: {union Cyc_CfFlowInfo_FlowInfo*
_tmp66E;struct _tuple16 _tmp66D=Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp62F);_tmp66E=_tmp66D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp66F=*_tmp66E;union
Cyc_CfFlowInfo_FlowInfo _tmp671;union Cyc_CfFlowInfo_FlowInfo _tmp672;struct
_tuple15 _tmp670=Cyc_NewControlFlow_anal_test(env,_tmp66F,_tmp62E);_tmp671=
_tmp670.f1;_tmp672=_tmp670.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp673=Cyc_NewControlFlow_anal_stmt(
env,_tmp671,_tmp630);Cyc_NewControlFlow_update_flow(env,_tmp62F,_tmp673);return
_tmp672;};};}_LL36C: {struct Cyc_Absyn_Do_s_struct*_tmp631=(struct Cyc_Absyn_Do_s_struct*)
_tmp61D;if(_tmp631->tag != 14)goto _LL36E;else{_tmp632=_tmp631->f1;_tmp633=_tmp631->f2;
_tmp634=_tmp633.f1;_tmp635=_tmp633.f2;}}_LL36D: {union Cyc_CfFlowInfo_FlowInfo
_tmp674=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp632);union Cyc_CfFlowInfo_FlowInfo*
_tmp676;struct _tuple16 _tmp675=Cyc_NewControlFlow_pre_stmt_check(env,_tmp674,
_tmp635);_tmp676=_tmp675.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp677=*_tmp676;union
Cyc_CfFlowInfo_FlowInfo _tmp679;union Cyc_CfFlowInfo_FlowInfo _tmp67A;struct
_tuple15 _tmp678=Cyc_NewControlFlow_anal_test(env,_tmp677,_tmp634);_tmp679=
_tmp678.f1;_tmp67A=_tmp678.f2;Cyc_NewControlFlow_update_flow(env,_tmp632,_tmp679);
return _tmp67A;};}_LL36E: {struct Cyc_Absyn_For_s_struct*_tmp636=(struct Cyc_Absyn_For_s_struct*)
_tmp61D;if(_tmp636->tag != 9)goto _LL370;else{_tmp637=_tmp636->f1;_tmp638=_tmp636->f2;
_tmp639=_tmp638.f1;_tmp63A=_tmp638.f2;_tmp63B=_tmp636->f3;_tmp63C=_tmp63B.f1;
_tmp63D=_tmp63B.f2;_tmp63E=_tmp636->f4;}}_LL36F: {union Cyc_CfFlowInfo_FlowInfo
_tmp67B=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp637)).f1;union Cyc_CfFlowInfo_FlowInfo*
_tmp67D;struct _tuple16 _tmp67C=Cyc_NewControlFlow_pre_stmt_check(env,_tmp67B,
_tmp63A);_tmp67D=_tmp67C.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp67E=*_tmp67D;union
Cyc_CfFlowInfo_FlowInfo _tmp680;union Cyc_CfFlowInfo_FlowInfo _tmp681;struct
_tuple15 _tmp67F=Cyc_NewControlFlow_anal_test(env,_tmp67E,_tmp639);_tmp680=
_tmp67F.f1;_tmp681=_tmp67F.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp682=Cyc_NewControlFlow_anal_stmt(
env,_tmp680,_tmp63E);union Cyc_CfFlowInfo_FlowInfo*_tmp684;struct _tuple16 _tmp683=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp682,_tmp63D);_tmp684=_tmp683.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp685=*_tmp684;union Cyc_CfFlowInfo_FlowInfo _tmp686=(Cyc_NewControlFlow_anal_Rexp(
env,0,_tmp685,_tmp63C)).f1;Cyc_NewControlFlow_update_flow(env,_tmp63A,_tmp686);
return _tmp681;};};};}_LL370: {struct Cyc_Absyn_Break_s_struct*_tmp63F=(struct Cyc_Absyn_Break_s_struct*)
_tmp61D;if(_tmp63F->tag != 6)goto _LL372;else{_tmp640=_tmp63F->f1;if(_tmp640 != 0)
goto _LL372;}}_LL371: return Cyc_CfFlowInfo_BottomFL();_LL372: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp641=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp61D;if(_tmp641->tag != 11)goto
_LL374;else{_tmp642=_tmp641->f1;_tmp643=_tmp641->f2;if(_tmp643 == 0)goto _LL374;
_tmp644=*_tmp643;}}_LL373: {struct _RegionHandle*_tmp687=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp689;struct Cyc_List_List*_tmp68A;struct _tuple19 _tmp688=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp687,env,inflow,_tmp642,1,1);_tmp689=_tmp688.f1;_tmp68A=_tmp688.f2;for(0;
_tmp68A != 0;(_tmp68A=_tmp68A->tl,_tmp642=_tmp642->tl)){_tmp689=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp642))->hd)->loc,
_tmp689,(void*)_tmp68A->hd);}_tmp689=Cyc_NewControlFlow_add_vars(_tmp61C,_tmp689,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp644->pat_vars))->v,
_tmp61C->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp644->body,_tmp689);return Cyc_CfFlowInfo_BottomFL();}_LL374: {struct Cyc_Absyn_Break_s_struct*
_tmp645=(struct Cyc_Absyn_Break_s_struct*)_tmp61D;if(_tmp645->tag != 6)goto _LL376;
else{_tmp646=_tmp645->f1;}}_LL375: _tmp648=_tmp646;goto _LL377;_LL376: {struct Cyc_Absyn_Continue_s_struct*
_tmp647=(struct Cyc_Absyn_Continue_s_struct*)_tmp61D;if(_tmp647->tag != 7)goto
_LL378;else{_tmp648=_tmp647->f1;}}_LL377: _tmp64A=_tmp648;goto _LL379;_LL378: {
struct Cyc_Absyn_Goto_s_struct*_tmp649=(struct Cyc_Absyn_Goto_s_struct*)_tmp61D;
if(_tmp649->tag != 8)goto _LL37A;else{_tmp64A=_tmp649->f2;}}_LL379: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp68B=_tmp61A->encloser;struct Cyc_Absyn_Stmt*
_tmp68C=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp64A)))->encloser;while(_tmp68C != _tmp68B){struct Cyc_Absyn_Stmt*_tmp68D=(Cyc_NewControlFlow_get_stmt_annot(
_tmp68B))->encloser;if(_tmp68D == _tmp68B){{const char*_tmp941;void*_tmp940;(
_tmp940=0,Cyc_Tcutil_terr(s->loc,((_tmp941="goto enters local scope or exception handler",
_tag_dyneither(_tmp941,sizeof(char),45))),_tag_dyneither(_tmp940,sizeof(void*),0)));}
break;}_tmp68B=_tmp68D;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp64A),inflow);return Cyc_CfFlowInfo_BottomFL();_LL37A: {struct Cyc_Absyn_Switch_s_struct*
_tmp64B=(struct Cyc_Absyn_Switch_s_struct*)_tmp61D;if(_tmp64B->tag != 10)goto
_LL37C;else{_tmp64C=_tmp64B->f1;_tmp64D=_tmp64B->f2;}}_LL37B: {union Cyc_CfFlowInfo_FlowInfo
_tmp691;void*_tmp692;struct _tuple13 _tmp690=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp64C);_tmp691=_tmp690.f1;_tmp692=_tmp690.f2;_tmp691=Cyc_NewControlFlow_use_Rval(
env,_tmp64C->loc,_tmp691,_tmp692);return Cyc_NewControlFlow_anal_scs(env,_tmp691,
_tmp64D);}_LL37C: {struct Cyc_Absyn_TryCatch_s_struct*_tmp64E=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp61D;if(_tmp64E->tag != 15)goto _LL37E;else{_tmp64F=_tmp64E->f1;_tmp650=_tmp64E->f2;}}
_LL37D: {int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp64F);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp650);{union Cyc_CfFlowInfo_FlowInfo _tmp693=scs_outflow;int
_tmp694;_LL389: if((_tmp693.BottomFL).tag != 1)goto _LL38B;_tmp694=(int)(_tmp693.BottomFL).val;
_LL38A: goto _LL388;_LL38B:;_LL38C: {const char*_tmp944;void*_tmp943;(_tmp943=0,Cyc_Tcutil_terr(
s->loc,((_tmp944="last catch clause may implicitly fallthru",_tag_dyneither(
_tmp944,sizeof(char),42))),_tag_dyneither(_tmp943,sizeof(void*),0)));}_LL388:;}
outflow=s1_outflow;return outflow;};};}_LL37E: {struct Cyc_Absyn_Decl_s_struct*
_tmp651=(struct Cyc_Absyn_Decl_s_struct*)_tmp61D;if(_tmp651->tag != 12)goto _LL380;
else{_tmp652=_tmp651->f1;_tmp653=*_tmp652;_tmp654=_tmp653.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp655=(struct Cyc_Absyn_Alias_d_struct*)_tmp654;if(_tmp655->tag != 5)goto _LL380;
else{_tmp656=_tmp655->f1;_tmp657=_tmp655->f2;_tmp658=_tmp655->f3;}};_tmp659=
_tmp651->f2;}}_LL37F: {union Cyc_CfFlowInfo_FlowInfo _tmp698;union Cyc_CfFlowInfo_AbsLVal
_tmp699;struct _tuple14 _tmp697=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp656);
_tmp698=_tmp697.f1;_tmp699=_tmp697.f2;{struct _tuple14 _tmp945;struct _tuple14
_tmp69B=(_tmp945.f1=_tmp698,((_tmp945.f2=_tmp699,_tmp945)));union Cyc_CfFlowInfo_FlowInfo
_tmp69C;struct _tuple12 _tmp69D;struct Cyc_Dict_Dict _tmp69E;struct Cyc_List_List*
_tmp69F;union Cyc_CfFlowInfo_AbsLVal _tmp6A0;struct Cyc_CfFlowInfo_Place*_tmp6A1;
union Cyc_CfFlowInfo_FlowInfo _tmp6A2;int _tmp6A3;_LL38E: _tmp69C=_tmp69B.f1;if((
_tmp69C.ReachableFL).tag != 2)goto _LL390;_tmp69D=(struct _tuple12)(_tmp69C.ReachableFL).val;
_tmp69E=_tmp69D.f1;_tmp69F=_tmp69D.f2;_tmp6A0=_tmp69B.f2;if((_tmp6A0.PlaceL).tag
!= 1)goto _LL390;_tmp6A1=(struct Cyc_CfFlowInfo_Place*)(_tmp6A0.PlaceL).val;_LL38F: {
void*_tmp6A4=Cyc_CfFlowInfo_lookup_place(_tmp69E,_tmp6A1);void*t=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp656->topt))->v;{void*_tmp6A5=_tmp656->r;struct Cyc_Absyn_Exp*
_tmp6A7;_LL395: {struct Cyc_Absyn_Cast_e_struct*_tmp6A6=(struct Cyc_Absyn_Cast_e_struct*)
_tmp6A5;if(_tmp6A6->tag != 15)goto _LL397;else{_tmp6A7=_tmp6A6->f2;}}_LL396: t=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6A7->topt))->v;goto _LL394;_LL397:;
_LL398: goto _LL394;_LL394:;}{void*_tmp6A8=Cyc_CfFlowInfo_make_unique_consumed(
_tmp61C,t,_tmp656,env->iteration_num,_tmp6A4);_tmp69E=Cyc_CfFlowInfo_assign_place(
_tmp61C,_tmp656->loc,_tmp69E,env->all_changed,_tmp6A1,_tmp6A8);_tmp698=Cyc_CfFlowInfo_ReachableFL(
_tmp69E,_tmp69F);{void*leaf;switch(Cyc_CfFlowInfo_initlevel(_tmp61C,_tmp69E,
_tmp6A4)){case Cyc_CfFlowInfo_AllIL: _LL399: leaf=_tmp61C->unknown_all;break;case
Cyc_CfFlowInfo_NoneIL: _LL39A: leaf=_tmp61C->unknown_none;break;case Cyc_CfFlowInfo_ThisIL:
_LL39B: leaf=_tmp61C->unknown_this;break;}{void*_tmp6A9=Cyc_CfFlowInfo_typ_to_absrval(
_tmp61C,t,leaf);_tmp698=Cyc_NewControlFlow_use_Rval(env,_tmp656->loc,_tmp698,
_tmp6A4);{struct Cyc_List_List _tmp946;struct Cyc_List_List _tmp6AA=(_tmp946.hd=
_tmp658,((_tmp946.tl=0,_tmp946)));_tmp698=Cyc_NewControlFlow_add_vars(_tmp61C,
_tmp698,(struct Cyc_List_List*)& _tmp6AA,_tmp61C->unknown_all,s->loc);_tmp698=Cyc_NewControlFlow_anal_stmt(
env,_tmp698,_tmp659);{union Cyc_CfFlowInfo_FlowInfo _tmp6AB=_tmp698;struct _tuple12
_tmp6AC;struct Cyc_Dict_Dict _tmp6AD;struct Cyc_List_List*_tmp6AE;_LL39E: if((
_tmp6AB.ReachableFL).tag != 2)goto _LL3A0;_tmp6AC=(struct _tuple12)(_tmp6AB.ReachableFL).val;
_tmp6AD=_tmp6AC.f1;_tmp6AE=_tmp6AC.f2;_LL39F: _tmp6AD=Cyc_CfFlowInfo_assign_place(
_tmp61C,s->loc,_tmp6AD,env->all_changed,_tmp6A1,_tmp6A9);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp6AD,_tmp6AE);return outflow;_LL3A0:;_LL3A1: return _tmp698;_LL39D:;};};};};};}
_LL390: _tmp6A2=_tmp69B.f1;if((_tmp6A2.BottomFL).tag != 1)goto _LL392;_tmp6A3=(int)(
_tmp6A2.BottomFL).val;_LL391: return _tmp698;_LL392:;_LL393:{const char*_tmp949;
void*_tmp948;(_tmp948=0,Cyc_Tcutil_terr(_tmp656->loc,((_tmp949="bad alias expression--no unique path found",
_tag_dyneither(_tmp949,sizeof(char),43))),_tag_dyneither(_tmp948,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL38D:;};}_LL380: {struct Cyc_Absyn_Decl_s_struct*
_tmp65A=(struct Cyc_Absyn_Decl_s_struct*)_tmp61D;if(_tmp65A->tag != 12)goto _LL382;
else{_tmp65B=_tmp65A->f1;_tmp65C=_tmp65A->f2;}}_LL381: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp65B),_tmp65C);if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp65B);return outflow;
_LL382: {struct Cyc_Absyn_Label_s_struct*_tmp65D=(struct Cyc_Absyn_Label_s_struct*)
_tmp61D;if(_tmp65D->tag != 13)goto _LL384;else{_tmp65E=_tmp65D->f2;}}_LL383: return
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp65E);_LL384: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp65F=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp61D;if(_tmp65F->tag != 16)goto
_LL386;else{_tmp660=_tmp65F->f1;}}_LL385: {union Cyc_CfFlowInfo_FlowInfo _tmp6B3;
void*_tmp6B4;struct _tuple13 _tmp6B2=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,
_tmp660);_tmp6B3=_tmp6B2.f1;_tmp6B4=_tmp6B2.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp6B5=Cyc_NewControlFlow_use_Rval(env,_tmp660->loc,_tmp6B3,_tmp6B4);void*
_tmp6B6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp660->topt))->v);
void*_tmp6B8;_LL3A3: {struct Cyc_Absyn_RgnHandleType_struct*_tmp6B7=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6B6;if(_tmp6B7->tag != 16)goto _LL3A5;else{_tmp6B8=(void*)_tmp6B7->f1;}}_LL3A4:
return Cyc_CfFlowInfo_kill_flow_region(_tmp61C,_tmp6B3,_tmp6B8);_LL3A5:;_LL3A6: {
struct Cyc_Core_Impossible_struct _tmp94F;const char*_tmp94E;struct Cyc_Core_Impossible_struct*
_tmp94D;(int)_throw((void*)((_tmp94D=_cycalloc(sizeof(*_tmp94D)),((_tmp94D[0]=((
_tmp94F.tag=Cyc_Core_Impossible,((_tmp94F.f1=((_tmp94E="anal_stmt -- reset_region",
_tag_dyneither(_tmp94E,sizeof(char),26))),_tmp94F)))),_tmp94D)))));}_LL3A2:;};}
_LL386:;_LL387: {struct Cyc_Core_Impossible_struct _tmp955;const char*_tmp954;
struct Cyc_Core_Impossible_struct*_tmp953;(int)_throw((void*)((_tmp953=_cycalloc(
sizeof(*_tmp953)),((_tmp953[0]=((_tmp955.tag=Cyc_Core_Impossible,((_tmp955.f1=((
_tmp954="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(
_tmp954,sizeof(char),56))),_tmp955)))),_tmp953)))));}_LL35D:;};}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl);static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*_tmp6BF=env->fenv;
void*_tmp6C0=decl->r;struct Cyc_Absyn_Vardecl*_tmp6C2;struct Cyc_Core_Opt*_tmp6C4;
struct Cyc_Core_Opt _tmp6C5;struct Cyc_List_List*_tmp6C6;struct Cyc_Absyn_Exp*
_tmp6C7;struct Cyc_List_List*_tmp6C9;struct Cyc_Absyn_Fndecl*_tmp6CB;struct Cyc_Absyn_Tvar*
_tmp6CD;struct Cyc_Absyn_Vardecl*_tmp6CE;int _tmp6CF;struct Cyc_Absyn_Exp*_tmp6D0;
_LL3A8: {struct Cyc_Absyn_Var_d_struct*_tmp6C1=(struct Cyc_Absyn_Var_d_struct*)
_tmp6C0;if(_tmp6C1->tag != 0)goto _LL3AA;else{_tmp6C2=_tmp6C1->f1;}}_LL3A9: {
struct Cyc_List_List _tmp956;struct Cyc_List_List _tmp6D1=(_tmp956.hd=_tmp6C2,((
_tmp956.tl=0,_tmp956)));inflow=Cyc_NewControlFlow_add_vars(_tmp6BF,inflow,(
struct Cyc_List_List*)& _tmp6D1,_tmp6BF->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*
_tmp6D2=_tmp6C2->initializer;if(_tmp6D2 == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo
_tmp6D4;void*_tmp6D5;struct _tuple13 _tmp6D3=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,(struct Cyc_Absyn_Exp*)_tmp6D2);_tmp6D4=_tmp6D3.f1;_tmp6D5=_tmp6D3.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6D6=_tmp6D4;int _tmp6D7;struct _tuple12 _tmp6D8;
struct Cyc_Dict_Dict _tmp6D9;struct Cyc_List_List*_tmp6DA;_LL3B5: if((_tmp6D6.BottomFL).tag
!= 1)goto _LL3B7;_tmp6D7=(int)(_tmp6D6.BottomFL).val;_LL3B6: return Cyc_CfFlowInfo_BottomFL();
_LL3B7: if((_tmp6D6.ReachableFL).tag != 2)goto _LL3B4;_tmp6D8=(struct _tuple12)(
_tmp6D6.ReachableFL).val;_tmp6D9=_tmp6D8.f1;_tmp6DA=_tmp6D8.f2;_LL3B8:{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp95C;struct Cyc_CfFlowInfo_VarRoot_struct _tmp95B;struct Cyc_CfFlowInfo_Place*
_tmp95A;_tmp6D9=Cyc_CfFlowInfo_assign_place(_tmp6BF,decl->loc,_tmp6D9,env->all_changed,((
_tmp95A=_region_malloc(env->r,sizeof(*_tmp95A)),((_tmp95A->root=(void*)((_tmp95C=
_region_malloc(env->r,sizeof(*_tmp95C)),((_tmp95C[0]=((_tmp95B.tag=0,((_tmp95B.f1=
_tmp6C2,_tmp95B)))),_tmp95C)))),((_tmp95A->fields=0,_tmp95A)))))),_tmp6D5);}
_tmp6DA=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp6DA,_tmp6C2,(struct Cyc_Absyn_Exp*)
_check_null(_tmp6C2->initializer));{union Cyc_CfFlowInfo_FlowInfo _tmp6DE=Cyc_CfFlowInfo_ReachableFL(
_tmp6D9,_tmp6DA);Cyc_NewControlFlow_update_tryflow(env,_tmp6DE);return _tmp6DE;};
_LL3B4:;};};};}_LL3AA: {struct Cyc_Absyn_Let_d_struct*_tmp6C3=(struct Cyc_Absyn_Let_d_struct*)
_tmp6C0;if(_tmp6C3->tag != 2)goto _LL3AC;else{_tmp6C4=_tmp6C3->f2;if(_tmp6C4 == 0)
goto _LL3AC;_tmp6C5=*_tmp6C4;_tmp6C6=(struct Cyc_List_List*)_tmp6C5.v;_tmp6C7=
_tmp6C3->f3;}}_LL3AB: {union Cyc_CfFlowInfo_FlowInfo _tmp6E1;void*_tmp6E2;struct
_tuple13 _tmp6E0=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6C7);_tmp6E1=
_tmp6E0.f1;_tmp6E2=_tmp6E0.f2;_tmp6E1=Cyc_NewControlFlow_use_Rval(env,_tmp6C7->loc,
_tmp6E1,_tmp6E2);return Cyc_NewControlFlow_add_vars(_tmp6BF,_tmp6E1,_tmp6C6,
_tmp6BF->unknown_all,decl->loc);}_LL3AC: {struct Cyc_Absyn_Letv_d_struct*_tmp6C8=(
struct Cyc_Absyn_Letv_d_struct*)_tmp6C0;if(_tmp6C8->tag != 3)goto _LL3AE;else{
_tmp6C9=_tmp6C8->f1;}}_LL3AD: return Cyc_NewControlFlow_add_vars(_tmp6BF,inflow,
_tmp6C9,_tmp6BF->unknown_none,decl->loc);_LL3AE: {struct Cyc_Absyn_Fn_d_struct*
_tmp6CA=(struct Cyc_Absyn_Fn_d_struct*)_tmp6C0;if(_tmp6CA->tag != 1)goto _LL3B0;
else{_tmp6CB=_tmp6CA->f1;}}_LL3AF: Cyc_NewControlFlow_check_nested_fun(_tmp6BF,
inflow,_tmp6CB);{void*_tmp6E3=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp6CB->cached_typ))->v;
struct Cyc_Absyn_Vardecl*_tmp6E4=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp6CB->fn_vardecl);
struct Cyc_List_List*_tmp95D;return Cyc_NewControlFlow_add_vars(_tmp6BF,inflow,((
_tmp95D=_region_malloc(env->r,sizeof(*_tmp95D)),((_tmp95D->hd=_tmp6E4,((_tmp95D->tl=
0,_tmp95D)))))),_tmp6BF->unknown_all,decl->loc);};_LL3B0: {struct Cyc_Absyn_Region_d_struct*
_tmp6CC=(struct Cyc_Absyn_Region_d_struct*)_tmp6C0;if(_tmp6CC->tag != 4)goto _LL3B2;
else{_tmp6CD=_tmp6CC->f1;_tmp6CE=_tmp6CC->f2;_tmp6CF=_tmp6CC->f3;_tmp6D0=_tmp6CC->f4;}}
_LL3B1: if(_tmp6D0 != 0){struct Cyc_Absyn_Exp*_tmp6E6=(struct Cyc_Absyn_Exp*)_tmp6D0;
union Cyc_CfFlowInfo_FlowInfo _tmp6E8;void*_tmp6E9;struct _tuple13 _tmp6E7=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp6E6);_tmp6E8=_tmp6E7.f1;_tmp6E9=_tmp6E7.f2;inflow=Cyc_NewControlFlow_use_Rval(
env,_tmp6E6->loc,_tmp6E8,_tmp6E9);}{struct Cyc_List_List _tmp95E;struct Cyc_List_List
_tmp6EA=(_tmp95E.hd=_tmp6CE,((_tmp95E.tl=0,_tmp95E)));return Cyc_NewControlFlow_add_vars(
_tmp6BF,inflow,(struct Cyc_List_List*)& _tmp6EA,_tmp6BF->unknown_all,decl->loc);};
_LL3B2:;_LL3B3: {struct Cyc_Core_Impossible_struct _tmp964;const char*_tmp963;
struct Cyc_Core_Impossible_struct*_tmp962;(int)_throw((void*)((_tmp962=_cycalloc(
sizeof(*_tmp962)),((_tmp962[0]=((_tmp964.tag=Cyc_Core_Impossible,((_tmp964.f1=((
_tmp963="anal_decl: unexpected decl variant",_tag_dyneither(_tmp963,sizeof(char),
35))),_tmp964)))),_tmp962)))));}_LL3A7:;}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd);static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp6EF=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp6EF;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,
0),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(void*s);static
int Cyc_NewControlFlow_hash_ptr(void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp6F0=fenv->r;struct Cyc_Position_Segment*_tmp6F1=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp6F1);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp6F2=inflow;int _tmp6F3;struct
_tuple12 _tmp6F4;struct Cyc_Dict_Dict _tmp6F5;struct Cyc_List_List*_tmp6F6;_LL3BA:
if((_tmp6F2.BottomFL).tag != 1)goto _LL3BC;_tmp6F3=(int)(_tmp6F2.BottomFL).val;
_LL3BB: {const char*_tmp967;void*_tmp966;(_tmp966=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp967="check_fun",
_tag_dyneither(_tmp967,sizeof(char),10))),_tag_dyneither(_tmp966,sizeof(void*),0)));}
_LL3BC: if((_tmp6F2.ReachableFL).tag != 2)goto _LL3B9;_tmp6F4=(struct _tuple12)(
_tmp6F2.ReachableFL).val;_tmp6F5=_tmp6F4.f1;_tmp6F6=_tmp6F4.f2;_LL3BD: {struct
Cyc_List_List*atts;{void*_tmp6F9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp6FB;struct Cyc_List_List*
_tmp6FC;_LL3BF: {struct Cyc_Absyn_FnType_struct*_tmp6FA=(struct Cyc_Absyn_FnType_struct*)
_tmp6F9;if(_tmp6FA->tag != 10)goto _LL3C1;else{_tmp6FB=_tmp6FA->f1;_tmp6FC=_tmp6FB.attributes;}}
_LL3C0: atts=_tmp6FC;goto _LL3BE;_LL3C1:;_LL3C2: {const char*_tmp96A;void*_tmp969;(
_tmp969=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp96A="check_fun: non-function type cached with fndecl_t",_tag_dyneither(
_tmp96A,sizeof(char),50))),_tag_dyneither(_tmp969,sizeof(void*),0)));}_LL3BE:;}
for(0;atts != 0;atts=atts->tl){void*_tmp6FF=(void*)atts->hd;int _tmp701;int _tmp703;
_LL3C4: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp700=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp6FF;if(_tmp700->tag != 21)goto _LL3C6;else{_tmp701=_tmp700->f1;}}_LL3C5: {
unsigned int j=(unsigned int)_tmp701;struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(
- 1,0);if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{const char*_tmp96D;
void*_tmp96C;(_tmp96C=0,Cyc_Tcutil_terr(_tmp6F1,((_tmp96D="noliveunique attribute exceeds number of parameters",
_tag_dyneither(_tmp96D,sizeof(char),52))),_tag_dyneither(_tmp96C,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp706=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));void*t=Cyc_Tcutil_compress(
_tmp706->type);if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp970;void*
_tmp96F;(_tmp96F=0,Cyc_Tcutil_terr(_tmp6F1,((_tmp970="noliveunique attribute requires unique pointer type",
_tag_dyneither(_tmp970,sizeof(char),52))),_tag_dyneither(_tmp96F,sizeof(void*),0)));}
continue;}{void*elttype=Cyc_Tcutil_pointer_elt_type(t);void*_tmp709=Cyc_CfFlowInfo_make_unique_consumed(
fenv,elttype,bogus_exp,- 1,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_struct _tmp973;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp972;struct Cyc_CfFlowInfo_InitParam_struct*_tmp70A=(_tmp972=_region_malloc(
_tmp6F0,sizeof(*_tmp972)),((_tmp972[0]=((_tmp973.tag=2,((_tmp973.f1=(int)j,((
_tmp973.f2=(void*)t,_tmp973)))))),_tmp972)));struct Cyc_CfFlowInfo_Place*_tmp974;
struct Cyc_CfFlowInfo_Place*_tmp70B=(_tmp974=_region_malloc(_tmp6F0,sizeof(*
_tmp974)),((_tmp974->root=(void*)_tmp70A,((_tmp974->fields=0,_tmp974)))));
_tmp6F5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6F5,(void*)_tmp70A,_tmp709);{struct Cyc_CfFlowInfo_AddressOf_struct _tmp97A;
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp979;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp977;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp976;_tmp6F5=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6F5,(void*)((_tmp976=
_region_malloc(_tmp6F0,sizeof(*_tmp976)),((_tmp976[0]=((_tmp977.tag=0,((_tmp977.f1=
_tmp706,_tmp977)))),_tmp976)))),(void*)((_tmp979=_region_malloc(_tmp6F0,sizeof(*
_tmp979)),((_tmp979[0]=((_tmp97A.tag=5,((_tmp97A.f1=_tmp70B,_tmp97A)))),_tmp979)))));}
goto _LL3C3;};};}_LL3C6: {struct Cyc_Absyn_Initializes_att_struct*_tmp702=(struct
Cyc_Absyn_Initializes_att_struct*)_tmp6FF;if(_tmp702->tag != 20)goto _LL3C8;else{
_tmp703=_tmp702->f1;}}_LL3C7: {unsigned int j=(unsigned int)_tmp703;if(j > ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){{const char*_tmp97D;void*_tmp97C;(_tmp97C=0,
Cyc_Tcutil_terr(_tmp6F1,((_tmp97D="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp97D,sizeof(char),51))),_tag_dyneither(_tmp97C,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp715=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp716=Cyc_Tcutil_compress(
_tmp715->type);struct Cyc_Absyn_PtrInfo _tmp718;void*_tmp719;struct Cyc_Absyn_PtrAtts
_tmp71A;union Cyc_Absyn_Constraint*_tmp71B;union Cyc_Absyn_Constraint*_tmp71C;
union Cyc_Absyn_Constraint*_tmp71D;_LL3CB: {struct Cyc_Absyn_PointerType_struct*
_tmp717=(struct Cyc_Absyn_PointerType_struct*)_tmp716;if(_tmp717->tag != 5)goto
_LL3CD;else{_tmp718=_tmp717->f1;_tmp719=_tmp718.elt_typ;_tmp71A=_tmp718.ptr_atts;
_tmp71B=_tmp71A.nullable;_tmp71C=_tmp71A.bounds;_tmp71D=_tmp71A.zero_term;}}
_LL3CC: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp71B)){
const char*_tmp980;void*_tmp97F;(_tmp97F=0,Cyc_Tcutil_terr(_tmp6F1,((_tmp980="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp980,sizeof(char),55))),_tag_dyneither(_tmp97F,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp71C)){const char*_tmp983;void*_tmp982;(_tmp982=0,
Cyc_Tcutil_terr(_tmp6F1,((_tmp983="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp983,sizeof(char),57))),_tag_dyneither(_tmp982,sizeof(void*),0)));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp71D)){
const char*_tmp986;void*_tmp985;(_tmp985=0,Cyc_Tcutil_terr(_tmp6F1,((_tmp986="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp986,sizeof(char),77))),_tag_dyneither(_tmp985,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_struct _tmp989;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp988;struct Cyc_CfFlowInfo_InitParam_struct*_tmp724=(_tmp988=_region_malloc(
_tmp6F0,sizeof(*_tmp988)),((_tmp988[0]=((_tmp989.tag=2,((_tmp989.f1=(int)j,((
_tmp989.f2=(void*)_tmp719,_tmp989)))))),_tmp988)));struct Cyc_CfFlowInfo_Place*
_tmp98A;struct Cyc_CfFlowInfo_Place*_tmp725=(_tmp98A=_region_malloc(_tmp6F0,
sizeof(*_tmp98A)),((_tmp98A->root=(void*)_tmp724,((_tmp98A->fields=0,_tmp98A)))));
_tmp6F5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6F5,(void*)_tmp724,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp719,fenv->esc_none));{
struct Cyc_CfFlowInfo_AddressOf_struct _tmp990;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp98F;struct Cyc_CfFlowInfo_VarRoot_struct _tmp98D;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp98C;_tmp6F5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6F5,(void*)((_tmp98C=_region_malloc(_tmp6F0,sizeof(*_tmp98C)),((_tmp98C[0]=((
_tmp98D.tag=0,((_tmp98D.f1=_tmp715,_tmp98D)))),_tmp98C)))),(void*)((_tmp98F=
_region_malloc(_tmp6F0,sizeof(*_tmp98F)),((_tmp98F[0]=((_tmp990.tag=5,((_tmp990.f1=
_tmp725,_tmp990)))),_tmp98F)))));}{struct Cyc_List_List*_tmp991;param_roots=((
_tmp991=_region_malloc(_tmp6F0,sizeof(*_tmp991)),((_tmp991->hd=_tmp725,((_tmp991->tl=
param_roots,_tmp991))))));}goto _LL3CA;};_LL3CD:;_LL3CE: {const char*_tmp994;void*
_tmp993;(_tmp993=0,Cyc_Tcutil_terr(_tmp6F1,((_tmp994="initializes attribute on non-pointer",
_tag_dyneither(_tmp994,sizeof(char),37))),_tag_dyneither(_tmp993,sizeof(void*),0)));}
_LL3CA:;}goto _LL3C3;};}_LL3C8:;_LL3C9: goto _LL3C3;_LL3C3:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp6F5,_tmp6F6);}_LL3B9:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp6F0,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp995;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp995=_region_malloc(_tmp6F0,sizeof(*_tmp995)),((
_tmp995->r=_tmp6F0,((_tmp995->fenv=fenv,((_tmp995->iterate_again=1,((_tmp995->iteration_num=
0,((_tmp995->in_try=0,((_tmp995->tryflow=inflow,((_tmp995->all_changed=0,((
_tmp995->noreturn=noreturn,((_tmp995->param_roots=param_roots,((_tmp995->flow_table=
flow_table,_tmp995)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
union Cyc_CfFlowInfo_FlowInfo _tmp730=outflow;int _tmp731;_LL3D0: if((_tmp730.BottomFL).tag
!= 1)goto _LL3D2;_tmp731=(int)(_tmp730.BottomFL).val;_LL3D1: goto _LL3CF;_LL3D2:;
_LL3D3: Cyc_NewControlFlow_check_init_params(_tmp6F1,env,outflow);if(noreturn){
const char*_tmp998;void*_tmp997;(_tmp997=0,Cyc_Tcutil_terr(_tmp6F1,((_tmp998="`noreturn' function might (implicitly) return",
_tag_dyneither(_tmp998,sizeof(char),46))),_tag_dyneither(_tmp997,sizeof(void*),0)));}
else{void*_tmp734=Cyc_Tcutil_compress(fd->ret_type);_LL3D5: {struct Cyc_Absyn_VoidType_struct*
_tmp735=(struct Cyc_Absyn_VoidType_struct*)_tmp734;if(_tmp735->tag != 0)goto _LL3D7;}
_LL3D6: goto _LL3D4;_LL3D7: {struct Cyc_Absyn_DoubleType_struct*_tmp736=(struct Cyc_Absyn_DoubleType_struct*)
_tmp734;if(_tmp736->tag != 8)goto _LL3D9;}_LL3D8: goto _LL3DA;_LL3D9: {struct Cyc_Absyn_FloatType_struct*
_tmp737=(struct Cyc_Absyn_FloatType_struct*)_tmp734;if(_tmp737->tag != 7)goto
_LL3DB;}_LL3DA: goto _LL3DC;_LL3DB: {struct Cyc_Absyn_IntType_struct*_tmp738=(
struct Cyc_Absyn_IntType_struct*)_tmp734;if(_tmp738->tag != 6)goto _LL3DD;}_LL3DC:{
const char*_tmp99B;void*_tmp99A;(_tmp99A=0,Cyc_Tcutil_warn(_tmp6F1,((_tmp99B="function may not return a value",
_tag_dyneither(_tmp99B,sizeof(char),32))),_tag_dyneither(_tmp99A,sizeof(void*),0)));}
goto _LL3D4;_LL3DD:;_LL3DE:{const char*_tmp99E;void*_tmp99D;(_tmp99D=0,Cyc_Tcutil_terr(
_tmp6F1,((_tmp99E="function may not return a value",_tag_dyneither(_tmp99E,
sizeof(char),32))),_tag_dyneither(_tmp99D,sizeof(void*),0)));}goto _LL3D4;_LL3D4:;}
goto _LL3CF;_LL3CF:;};};};}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){
void*_tmp73E=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp740;
struct Cyc_List_List*_tmp742;struct Cyc_List_List*_tmp744;struct Cyc_List_List*
_tmp746;_LL3E0: {struct Cyc_Absyn_Fn_d_struct*_tmp73F=(struct Cyc_Absyn_Fn_d_struct*)
_tmp73E;if(_tmp73F->tag != 1)goto _LL3E2;else{_tmp740=_tmp73F->f1;}}_LL3E1: Cyc_NewControlFlow_check_fun(
_tmp740);goto _LL3DF;_LL3E2: {struct Cyc_Absyn_ExternC_d_struct*_tmp741=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp73E;if(_tmp741->tag != 12)goto _LL3E4;else{_tmp742=_tmp741->f1;}}_LL3E3:
_tmp744=_tmp742;goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Using_d_struct*_tmp743=(
struct Cyc_Absyn_Using_d_struct*)_tmp73E;if(_tmp743->tag != 11)goto _LL3E6;else{
_tmp744=_tmp743->f2;}}_LL3E5: _tmp746=_tmp744;goto _LL3E7;_LL3E6: {struct Cyc_Absyn_Namespace_d_struct*
_tmp745=(struct Cyc_Absyn_Namespace_d_struct*)_tmp73E;if(_tmp745->tag != 10)goto
_LL3E8;else{_tmp746=_tmp745->f2;}}_LL3E7: Cyc_NewControlFlow_cf_check(_tmp746);
goto _LL3DF;_LL3E8: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp747=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp73E;if(_tmp747->tag != 13)goto _LL3EA;}_LL3E9: goto _LL3DF;_LL3EA:;_LL3EB: goto
_LL3DF;_LL3DF:;}}
