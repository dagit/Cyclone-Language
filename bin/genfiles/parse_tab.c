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
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
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
return 0;return dyn.curr;};}extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*x);void*Cyc_List_fold_right(void*(*f)(
void*,void*),struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0
Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);int Cyc_zstrcmp(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;
int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct _dyneither_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct
Cyc_Position_Segment*,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
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
int tag;struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;
struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct
Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct
Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;
struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3
val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};
union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct
_union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{
union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;}
;struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
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
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};extern
struct Cyc_Absyn_Stdcall_att_struct Cyc_Absyn_Stdcall_att_val;extern struct Cyc_Absyn_Cdecl_att_struct
Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_struct Cyc_Absyn_Noreturn_att_val;extern struct
Cyc_Absyn_Const_att_struct Cyc_Absyn_Const_att_val;extern struct Cyc_Absyn_Packed_att_struct
Cyc_Absyn_Packed_att_val;extern struct Cyc_Absyn_Shared_att_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_struct Cyc_Absyn_Unused_att_val;extern struct Cyc_Absyn_Weak_att_struct
Cyc_Absyn_Weak_att_val;extern struct Cyc_Absyn_Dllimport_att_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_struct Cyc_Absyn_Dllexport_att_val;extern
struct Cyc_Absyn_No_instrument_function_att_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_struct Cyc_Absyn_Constructor_att_val;extern
struct Cyc_Absyn_Destructor_att_struct Cyc_Absyn_Destructor_att_val;extern struct
Cyc_Absyn_No_check_memory_usage_att_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_struct{
int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
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
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*
r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope
sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum Cyc_Absyn_Scope
sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*
fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;struct
Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*
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
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};extern struct Cyc_Absyn_Porton_d_struct
Cyc_Absyn_Porton_d_val;extern struct Cyc_Absyn_Portoff_d_struct Cyc_Absyn_Portoff_d_val;
struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_is_qvar_qualified(struct _tuple1*);struct Cyc_Absyn_Tqual
Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(
void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();extern union Cyc_Absyn_Constraint*
Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_starb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
enum Cyc_Absyn_Sign,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(
enum Cyc_Absyn_Primop,struct Cyc_List_List*es,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*
exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple1*
n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(
enum Cyc_Absyn_Scope s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*
Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_compress(void*t);extern struct Cyc_Absyn_Kind
Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct Cyc_Absyn_Kind
Cyc_Tcutil_bk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;extern struct Cyc_Absyn_Kind
Cyc_Tcutil_ek;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;extern struct Cyc_Absyn_Kind
Cyc_Tcutil_trk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;extern struct Cyc_Absyn_Kind
Cyc_Tcutil_tbk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;extern struct Cyc_Absyn_Kind
Cyc_Tcutil_urk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;extern struct Cyc_Absyn_Kind
Cyc_Tcutil_ubk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;extern struct Cyc_Core_Opt
Cyc_Tcutil_rko;extern struct Cyc_Core_Opt Cyc_Tcutil_bko;extern struct Cyc_Core_Opt
Cyc_Tcutil_iko;extern struct Cyc_Core_Opt Cyc_Tcutil_trko;struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(
struct Cyc_Absyn_Kind*k);void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Lex_register_typedef(struct _tuple1*s);void Cyc_Lex_enter_namespace(
struct _dyneither_ptr*);void Cyc_Lex_leave_namespace();void Cyc_Lex_enter_using(
struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=0;struct Cyc_Parse_Signed_spec_struct{
int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Unsigned_spec_struct{int
tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Short_spec_struct{int tag;
struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Long_spec_struct{int tag;struct
Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{int tag;void*f1;struct
Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{int tag;struct Cyc_Absyn_Decl*
f1;};enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0,Cyc_Parse_Extern_sc
 = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc  = 3,Cyc_Parse_Auto_sc  = 4,Cyc_Parse_Register_sc
 = 5,Cyc_Parse_Abstract_sc  = 6};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple10{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple10 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple11{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple11
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg);static void Cyc_Parse_err(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);static void*Cyc_Parse_abort(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
Cyc_Parse_err((struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);{
struct Cyc_Position_Exit_struct _tmp859;struct Cyc_Position_Exit_struct*_tmp858;(
int)_throw((void*)((_tmp858=_cycalloc_atomic(sizeof(*_tmp858)),((_tmp858[0]=((
_tmp859.tag=Cyc_Position_Exit,_tmp859)),_tmp858)))));};}static void Cyc_Parse_unimp(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg);static void Cyc_Parse_unimp(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){{const char*_tmp85E;void*
_tmp85D[2];struct Cyc_String_pa_struct _tmp85C;struct Cyc_String_pa_struct _tmp85B;(
_tmp85B.tag=0,((_tmp85B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((
_tmp85C.tag=0,((_tmp85C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
sg)),((_tmp85D[0]=& _tmp85C,((_tmp85D[1]=& _tmp85B,Cyc_fprintf(Cyc_stderr,((
_tmp85E="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp85E,
sizeof(char),46))),_tag_dyneither(_tmp85D,sizeof(void*),2)))))))))))));}return;}
struct _tuple12{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union
Cyc_Absyn_Constraint*f3;void*f4;};static struct _tuple12 Cyc_Parse_collapse_pointer_quals(
struct Cyc_Position_Segment*loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*
bound,void*rgn,struct Cyc_List_List*pqs);static struct _tuple12 Cyc_Parse_collapse_pointer_quals(
struct Cyc_Position_Segment*loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*
bound,void*rgn,struct Cyc_List_List*pqs){union Cyc_Absyn_Constraint*zeroterm=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();for(0;pqs != 0;pqs=pqs->tl){
void*_tmp6=(void*)pqs->hd;struct Cyc_Absyn_Exp*_tmpE;void*_tmp10;_LL1: {struct Cyc_Absyn_Zeroterm_ptrqual_struct*
_tmp7=(struct Cyc_Absyn_Zeroterm_ptrqual_struct*)_tmp6;if(_tmp7->tag != 4)goto _LL3;}
_LL2: zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: {struct Cyc_Absyn_Nozeroterm_ptrqual_struct*
_tmp8=(struct Cyc_Absyn_Nozeroterm_ptrqual_struct*)_tmp6;if(_tmp8->tag != 5)goto
_LL5;}_LL4: zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: {struct Cyc_Absyn_Nullable_ptrqual_struct*
_tmp9=(struct Cyc_Absyn_Nullable_ptrqual_struct*)_tmp6;if(_tmp9->tag != 7)goto _LL7;}
_LL6: nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: {struct Cyc_Absyn_Notnull_ptrqual_struct*
_tmpA=(struct Cyc_Absyn_Notnull_ptrqual_struct*)_tmp6;if(_tmpA->tag != 6)goto _LL9;}
_LL8: nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: {struct Cyc_Absyn_Fat_ptrqual_struct*
_tmpB=(struct Cyc_Absyn_Fat_ptrqual_struct*)_tmp6;if(_tmpB->tag != 3)goto _LLB;}
_LLA: bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB: {struct Cyc_Absyn_Thin_ptrqual_struct*
_tmpC=(struct Cyc_Absyn_Thin_ptrqual_struct*)_tmp6;if(_tmpC->tag != 2)goto _LLD;}
_LLC: bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD: {struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmpD=(struct Cyc_Absyn_Numelts_ptrqual_struct*)_tmp6;if(_tmpD->tag != 0)goto _LLF;
else{_tmpE=_tmpD->f1;}}_LLE:{struct Cyc_Absyn_Upper_b_struct _tmp861;struct Cyc_Absyn_Upper_b_struct*
_tmp860;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmp860=_cycalloc(sizeof(*_tmp860)),((_tmp860[0]=((_tmp861.tag=1,((
_tmp861.f1=_tmpE,_tmp861)))),_tmp860)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_struct*
_tmpF=(struct Cyc_Absyn_Region_ptrqual_struct*)_tmp6;if(_tmpF->tag != 1)goto _LL0;
else{_tmp10=(void*)_tmpF->f1;}}_LL10: rgn=_tmp10;goto _LL0;_LL0:;}{struct _tuple12
_tmp862;return(_tmp862.f1=nullable,((_tmp862.f2=bound,((_tmp862.f3=zeroterm,((
_tmp862.f4=rgn,_tmp862)))))));};}struct _tuple1*Cyc_Parse_gensym_enum();struct
_tuple1*Cyc_Parse_gensym_enum(){static int enum_counter=0;struct _dyneither_ptr*
_tmp86F;const char*_tmp86E;void*_tmp86D[1];struct Cyc_Int_pa_struct _tmp86C;struct
_tuple1*_tmp86B;return(_tmp86B=_cycalloc(sizeof(*_tmp86B)),((_tmp86B->f1=Cyc_Absyn_Rel_n(
0),((_tmp86B->f2=((_tmp86F=_cycalloc(sizeof(*_tmp86F)),((_tmp86F[0]=(struct
_dyneither_ptr)((_tmp86C.tag=1,((_tmp86C.f1=(unsigned long)enum_counter ++,((
_tmp86D[0]=& _tmp86C,Cyc_aprintf(((_tmp86E="__anonymous_enum_%d__",_tag_dyneither(
_tmp86E,sizeof(char),22))),_tag_dyneither(_tmp86D,sizeof(void*),1)))))))),
_tmp86F)))),_tmp86B)))));}struct _tuple13{struct _tuple1*f1;struct Cyc_Absyn_Tqual
f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple14{struct
_tuple13*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple14*field_info);static struct Cyc_Absyn_Aggrfield*
Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*loc,struct _tuple14*
field_info){struct _tuple14 _tmp1A;struct _tuple13*_tmp1B;struct _tuple13 _tmp1C;
struct _tuple1*_tmp1D;struct Cyc_Absyn_Tqual _tmp1E;void*_tmp1F;struct Cyc_List_List*
_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_Absyn_Exp*_tmp22;struct _tuple14*
_tmp19=field_info;_tmp1A=*_tmp19;_tmp1B=_tmp1A.f1;_tmp1C=*_tmp1B;_tmp1D=_tmp1C.f1;
_tmp1E=_tmp1C.f2;_tmp1F=_tmp1C.f3;_tmp20=_tmp1C.f4;_tmp21=_tmp1C.f5;_tmp22=
_tmp1A.f2;if(_tmp20 != 0){const char*_tmp870;Cyc_Parse_err(((_tmp870="bad type params in struct field",
_tag_dyneither(_tmp870,sizeof(char),32))),loc);}if(Cyc_Absyn_is_qvar_qualified(
_tmp1D)){const char*_tmp871;Cyc_Parse_err(((_tmp871="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp871,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp872;return(_tmp872=_cycalloc(sizeof(*_tmp872)),((_tmp872->name=(*_tmp1D).f2,((
_tmp872->tq=_tmp1E,((_tmp872->type=_tmp1F,((_tmp872->width=_tmp22,((_tmp872->attributes=
_tmp21,_tmp872)))))))))));};}static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*
loc);static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*loc){
struct Cyc_Parse_Type_spec_struct _tmp875;struct Cyc_Parse_Type_spec_struct*_tmp874;
return(void*)((_tmp874=_cycalloc(sizeof(*_tmp874)),((_tmp874[0]=((_tmp875.tag=4,((
_tmp875.f1=(void*)t,((_tmp875.f2=loc,_tmp875)))))),_tmp874))));}static void*Cyc_Parse_array2ptr(
void*t,int argposn);static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp28=
t;struct Cyc_Absyn_ArrayInfo _tmp2A;void*_tmp2B;struct Cyc_Absyn_Tqual _tmp2C;struct
Cyc_Absyn_Exp*_tmp2D;union Cyc_Absyn_Constraint*_tmp2E;struct Cyc_Position_Segment*
_tmp2F;_LL12: {struct Cyc_Absyn_ArrayType_struct*_tmp29=(struct Cyc_Absyn_ArrayType_struct*)
_tmp28;if(_tmp29->tag != 9)goto _LL14;else{_tmp2A=_tmp29->f1;_tmp2B=_tmp2A.elt_type;
_tmp2C=_tmp2A.tq;_tmp2D=_tmp2A.num_elts;_tmp2E=_tmp2A.zero_term;_tmp2F=_tmp2A.zt_loc;}}
_LL13: {struct Cyc_Absyn_Upper_b_struct _tmp878;struct Cyc_Absyn_Upper_b_struct*
_tmp877;return Cyc_Absyn_starb_typ(_tmp2B,argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,_tmp2C,_tmp2D == 0?(void*)& Cyc_Absyn_DynEither_b_val:(
void*)((void*)((_tmp877=_cycalloc(sizeof(*_tmp877)),((_tmp877[0]=((_tmp878.tag=1,((
_tmp878.f1=(struct Cyc_Absyn_Exp*)_tmp2D,_tmp878)))),_tmp877))))),_tmp2E);}_LL14:;
_LL15: return t;_LL11:;}struct _tuple15{struct _dyneither_ptr*f1;void*f2;};static
struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x);static struct
Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){struct Cyc_List_List*
_tmp32=0;for(0;x != 0;x=x->tl){struct _tuple8*_tmp33=(struct _tuple8*)x->hd;struct
_tuple8 _tmp34;struct Cyc_Core_Opt*_tmp35;struct Cyc_Core_Opt _tmp36;struct
_dyneither_ptr*_tmp37;void*_tmp38;void*_tmp3A;struct _tuple8 _tmp3B;struct Cyc_Core_Opt*
_tmp3C;struct Cyc_Core_Opt _tmp3D;struct _dyneither_ptr*_tmp3E;void*_tmp3F;void*
_tmp41;struct Cyc_Core_Opt*_tmp43;struct Cyc_Core_Opt**_tmp44;_LL17: _tmp34=*_tmp33;
_tmp35=_tmp34.f1;if(_tmp35 == 0)goto _LL19;_tmp36=*_tmp35;_tmp37=(struct
_dyneither_ptr*)_tmp36.v;_tmp38=_tmp34.f3;{struct Cyc_Absyn_TagType_struct*_tmp39=(
struct Cyc_Absyn_TagType_struct*)_tmp38;if(_tmp39->tag != 20)goto _LL19;else{_tmp3A=(
void*)_tmp39->f1;}};_LL18:{void*_tmp45=_tmp3A;struct Cyc_Core_Opt*_tmp47;struct
Cyc_Core_Opt**_tmp48;_LL1E: {struct Cyc_Absyn_Evar_struct*_tmp46=(struct Cyc_Absyn_Evar_struct*)
_tmp45;if(_tmp46->tag != 1)goto _LL20;else{_tmp47=_tmp46->f2;_tmp48=(struct Cyc_Core_Opt**)&
_tmp46->f2;}}_LL1F: {struct Cyc_String_pa_struct _tmp880;void*_tmp87F[1];const char*
_tmp87E;struct _dyneither_ptr*_tmp87D;struct _dyneither_ptr*nm=(struct
_dyneither_ptr*)((_tmp87D=_cycalloc(sizeof(*_tmp87D)),((_tmp87D[0]=((_tmp880.tag=
0,((_tmp880.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp37),((_tmp87F[0]=&
_tmp880,Cyc_aprintf(((_tmp87E="`%s",_tag_dyneither(_tmp87E,sizeof(char),4))),
_tag_dyneither(_tmp87F,sizeof(void*),1)))))))),_tmp87D))));{struct Cyc_Absyn_VarType_struct*
_tmp895;struct Cyc_Absyn_Tvar*_tmp894;struct Cyc_Absyn_Eq_kb_struct _tmp893;struct
Cyc_Absyn_Eq_kb_struct*_tmp892;struct Cyc_Absyn_VarType_struct _tmp891;struct Cyc_Core_Opt*
_tmp890;*_tmp48=((_tmp890=_cycalloc(sizeof(*_tmp890)),((_tmp890->v=(void*)((
_tmp895=_cycalloc(sizeof(*_tmp895)),((_tmp895[0]=((_tmp891.tag=2,((_tmp891.f1=((
_tmp894=_cycalloc(sizeof(*_tmp894)),((_tmp894->name=nm,((_tmp894->identity=- 1,((
_tmp894->kind=(void*)((_tmp892=_cycalloc(sizeof(*_tmp892)),((_tmp892[0]=((
_tmp893.tag=0,((_tmp893.f1=& Cyc_Tcutil_ik,_tmp893)))),_tmp892)))),_tmp894)))))))),
_tmp891)))),_tmp895)))),_tmp890))));}goto _LL1D;}_LL20:;_LL21: goto _LL1D;_LL1D:;}{
struct _tuple15*_tmp898;struct Cyc_List_List*_tmp897;_tmp32=((_tmp897=_cycalloc(
sizeof(*_tmp897)),((_tmp897->hd=((_tmp898=_cycalloc(sizeof(*_tmp898)),((_tmp898->f1=
_tmp37,((_tmp898->f2=_tmp3A,_tmp898)))))),((_tmp897->tl=_tmp32,_tmp897))))));}
goto _LL16;_LL19: _tmp3B=*_tmp33;_tmp3C=_tmp3B.f1;if(_tmp3C == 0)goto _LL1B;_tmp3D=*
_tmp3C;_tmp3E=(struct _dyneither_ptr*)_tmp3D.v;_tmp3F=_tmp3B.f3;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp40=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp3F;if(_tmp40->tag != 16)goto
_LL1B;else{_tmp41=(void*)_tmp40->f1;{struct Cyc_Absyn_Evar_struct*_tmp42=(struct
Cyc_Absyn_Evar_struct*)_tmp41;if(_tmp42->tag != 1)goto _LL1B;else{_tmp43=_tmp42->f2;
_tmp44=(struct Cyc_Core_Opt**)& _tmp42->f2;}};}};_LL1A: {struct Cyc_String_pa_struct
_tmp8A0;void*_tmp89F[1];const char*_tmp89E;struct _dyneither_ptr*_tmp89D;struct
_dyneither_ptr*nm=(struct _dyneither_ptr*)((_tmp89D=_cycalloc(sizeof(*_tmp89D)),((
_tmp89D[0]=((_tmp8A0.tag=0,((_tmp8A0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp3E),((_tmp89F[0]=& _tmp8A0,Cyc_aprintf(((_tmp89E="`%s",
_tag_dyneither(_tmp89E,sizeof(char),4))),_tag_dyneither(_tmp89F,sizeof(void*),1)))))))),
_tmp89D))));{struct Cyc_Absyn_VarType_struct*_tmp8B5;struct Cyc_Absyn_Tvar*_tmp8B4;
struct Cyc_Absyn_Eq_kb_struct _tmp8B3;struct Cyc_Absyn_Eq_kb_struct*_tmp8B2;struct
Cyc_Absyn_VarType_struct _tmp8B1;struct Cyc_Core_Opt*_tmp8B0;*_tmp44=((_tmp8B0=
_cycalloc(sizeof(*_tmp8B0)),((_tmp8B0->v=(void*)((_tmp8B5=_cycalloc(sizeof(*
_tmp8B5)),((_tmp8B5[0]=((_tmp8B1.tag=2,((_tmp8B1.f1=((_tmp8B4=_cycalloc(sizeof(*
_tmp8B4)),((_tmp8B4->name=nm,((_tmp8B4->identity=- 1,((_tmp8B4->kind=(void*)((
_tmp8B2=_cycalloc(sizeof(*_tmp8B2)),((_tmp8B2[0]=((_tmp8B3.tag=0,((_tmp8B3.f1=&
Cyc_Tcutil_rk,_tmp8B3)))),_tmp8B2)))),_tmp8B4)))))))),_tmp8B1)))),_tmp8B5)))),
_tmp8B0))));}goto _LL16;}_LL1B:;_LL1C: goto _LL16;_LL16:;}return _tmp32;}static
struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x);static
struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){struct
Cyc_List_List*_tmp5F=0;for(0;x != 0;x=x->tl){void*_tmp60=((struct Cyc_Absyn_Aggrfield*)
x->hd)->type;void*_tmp62;_LL23: {struct Cyc_Absyn_TagType_struct*_tmp61=(struct
Cyc_Absyn_TagType_struct*)_tmp60;if(_tmp61->tag != 20)goto _LL25;else{_tmp62=(void*)
_tmp61->f1;}}_LL24:{struct _tuple15*_tmp8B8;struct Cyc_List_List*_tmp8B7;_tmp5F=((
_tmp8B7=_cycalloc(sizeof(*_tmp8B7)),((_tmp8B7->hd=((_tmp8B8=_cycalloc(sizeof(*
_tmp8B8)),((_tmp8B8->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8B8->f2=
_tmp62,_tmp8B8)))))),((_tmp8B7->tl=_tmp5F,_tmp8B7))))));}goto _LL22;_LL25:;_LL26:
goto _LL22;_LL22:;}return _tmp5F;}static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(
struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(
struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){{void*_tmp65=e->r;struct _tuple1*
_tmp67;struct _tuple1 _tmp68;union Cyc_Absyn_Nmspace _tmp69;struct Cyc_List_List*
_tmp6A;struct _dyneither_ptr*_tmp6B;struct _tuple1*_tmp6D;struct _tuple1 _tmp6E;
union Cyc_Absyn_Nmspace _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr*
_tmp71;_LL28: {struct Cyc_Absyn_Var_e_struct*_tmp66=(struct Cyc_Absyn_Var_e_struct*)
_tmp65;if(_tmp66->tag != 1)goto _LL2A;else{_tmp67=_tmp66->f1;_tmp68=*_tmp67;_tmp69=
_tmp68.f1;if((_tmp69.Rel_n).tag != 1)goto _LL2A;_tmp6A=(struct Cyc_List_List*)(
_tmp69.Rel_n).val;if(_tmp6A != 0)goto _LL2A;_tmp6B=_tmp68.f2;}}_LL29: _tmp71=_tmp6B;
goto _LL2B;_LL2A: {struct Cyc_Absyn_UnknownId_e_struct*_tmp6C=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp65;if(_tmp6C->tag != 2)goto _LL2C;else{_tmp6D=_tmp6C->f1;_tmp6E=*_tmp6D;_tmp6F=
_tmp6E.f1;if((_tmp6F.Rel_n).tag != 1)goto _LL2C;_tmp70=(struct Cyc_List_List*)(
_tmp6F.Rel_n).val;if(_tmp70 != 0)goto _LL2C;_tmp71=_tmp6E.f2;}}_LL2B:{struct Cyc_List_List*
_tmp72=tags;for(0;_tmp72 != 0;_tmp72=_tmp72->tl){struct _tuple15 _tmp74;struct
_dyneither_ptr*_tmp75;void*_tmp76;struct _tuple15*_tmp73=(struct _tuple15*)_tmp72->hd;
_tmp74=*_tmp73;_tmp75=_tmp74.f1;_tmp76=_tmp74.f2;if(Cyc_strptrcmp(_tmp75,_tmp71)
== 0){struct Cyc_Absyn_Valueof_e_struct _tmp8BB;struct Cyc_Absyn_Valueof_e_struct*
_tmp8BA;return Cyc_Absyn_new_exp((void*)((_tmp8BA=_cycalloc(sizeof(*_tmp8BA)),((
_tmp8BA[0]=((_tmp8BB.tag=39,((_tmp8BB.f1=(void*)Cyc_Tcutil_copy_type(_tmp76),
_tmp8BB)))),_tmp8BA)))),e->loc);}}}goto _LL27;_LL2C:;_LL2D: goto _LL27;_LL27:;}
return e;}static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t);
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){{void*
_tmp79=t;struct Cyc_Absyn_ArrayInfo _tmp7B;void*_tmp7C;struct Cyc_Absyn_Tqual _tmp7D;
struct Cyc_Absyn_Exp*_tmp7E;union Cyc_Absyn_Constraint*_tmp7F;struct Cyc_Position_Segment*
_tmp80;struct Cyc_Absyn_PtrInfo _tmp82;void*_tmp83;struct Cyc_Absyn_Tqual _tmp84;
struct Cyc_Absyn_PtrAtts _tmp85;void*_tmp86;union Cyc_Absyn_Constraint*_tmp87;union
Cyc_Absyn_Constraint*_tmp88;union Cyc_Absyn_Constraint*_tmp89;struct Cyc_Absyn_PtrLoc*
_tmp8A;_LL2F: {struct Cyc_Absyn_ArrayType_struct*_tmp7A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp79;if(_tmp7A->tag != 9)goto _LL31;else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.elt_type;
_tmp7D=_tmp7B.tq;_tmp7E=_tmp7B.num_elts;_tmp7F=_tmp7B.zero_term;_tmp80=_tmp7B.zt_loc;}}
_LL30: {struct Cyc_Absyn_Exp*nelts2=_tmp7E;if(_tmp7E != 0)nelts2=(struct Cyc_Absyn_Exp*)
Cyc_Parse_substitute_tags_exp(tags,(struct Cyc_Absyn_Exp*)_tmp7E);{void*_tmp8B=
Cyc_Parse_substitute_tags(tags,_tmp7C);if(_tmp7E != nelts2  || _tmp7C != _tmp8B){
struct Cyc_Absyn_ArrayType_struct _tmp8C1;struct Cyc_Absyn_ArrayInfo _tmp8C0;struct
Cyc_Absyn_ArrayType_struct*_tmp8BF;return(void*)((_tmp8BF=_cycalloc(sizeof(*
_tmp8BF)),((_tmp8BF[0]=((_tmp8C1.tag=9,((_tmp8C1.f1=((_tmp8C0.elt_type=_tmp8B,((
_tmp8C0.tq=_tmp7D,((_tmp8C0.num_elts=nelts2,((_tmp8C0.zero_term=_tmp7F,((_tmp8C0.zt_loc=
_tmp80,_tmp8C0)))))))))),_tmp8C1)))),_tmp8BF))));}goto _LL2E;};}_LL31: {struct Cyc_Absyn_PointerType_struct*
_tmp81=(struct Cyc_Absyn_PointerType_struct*)_tmp79;if(_tmp81->tag != 5)goto _LL33;
else{_tmp82=_tmp81->f1;_tmp83=_tmp82.elt_typ;_tmp84=_tmp82.elt_tq;_tmp85=_tmp82.ptr_atts;
_tmp86=_tmp85.rgn;_tmp87=_tmp85.nullable;_tmp88=_tmp85.bounds;_tmp89=_tmp85.zero_term;
_tmp8A=_tmp85.ptrloc;}}_LL32: {void*_tmp8F=Cyc_Parse_substitute_tags(tags,_tmp83);
union Cyc_Absyn_Constraint*_tmp90=_tmp88;{union Cyc_Absyn_Constraint _tmp91=*_tmp88;
void*_tmp92;struct Cyc_Absyn_Exp*_tmp94;_LL36: if((_tmp91.Eq_constr).tag != 1)goto
_LL38;_tmp92=(void*)(_tmp91.Eq_constr).val;{struct Cyc_Absyn_Upper_b_struct*
_tmp93=(struct Cyc_Absyn_Upper_b_struct*)_tmp92;if(_tmp93->tag != 1)goto _LL38;
else{_tmp94=_tmp93->f1;}};_LL37: _tmp94=Cyc_Parse_substitute_tags_exp(tags,_tmp94);{
struct Cyc_Absyn_Upper_b_struct*_tmp8C7;struct Cyc_Absyn_Upper_b_struct _tmp8C6;
union Cyc_Absyn_Constraint*_tmp8C5;_tmp90=((_tmp8C5=_cycalloc(sizeof(*_tmp8C5)),(((
_tmp8C5->Eq_constr).val=(void*)((_tmp8C7=_cycalloc(sizeof(*_tmp8C7)),((_tmp8C7[0]=((
_tmp8C6.tag=1,((_tmp8C6.f1=_tmp94,_tmp8C6)))),_tmp8C7)))),(((_tmp8C5->Eq_constr).tag=
1,_tmp8C5))))));}goto _LL35;_LL38:;_LL39: goto _LL35;_LL35:;}if(_tmp8F != _tmp83  || 
_tmp90 != _tmp88){struct Cyc_Absyn_PointerType_struct _tmp8D1;struct Cyc_Absyn_PtrAtts
_tmp8D0;struct Cyc_Absyn_PtrInfo _tmp8CF;struct Cyc_Absyn_PointerType_struct*
_tmp8CE;return(void*)((_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE[0]=((
_tmp8D1.tag=5,((_tmp8D1.f1=((_tmp8CF.elt_typ=_tmp8F,((_tmp8CF.elt_tq=_tmp84,((
_tmp8CF.ptr_atts=((_tmp8D0.rgn=_tmp86,((_tmp8D0.nullable=_tmp87,((_tmp8D0.bounds=
_tmp90,((_tmp8D0.zero_term=_tmp89,((_tmp8D0.ptrloc=_tmp8A,_tmp8D0)))))))))),
_tmp8CF)))))),_tmp8D1)))),_tmp8CE))));}goto _LL2E;}_LL33:;_LL34: goto _LL2E;_LL2E:;}
return t;}static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,
struct Cyc_Absyn_Aggrfield*x);static void Cyc_Parse_substitute_aggrfield_tags(
struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){x->type=Cyc_Parse_substitute_tags(
tags,x->type);}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};static struct
_tuple16*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple8*t);
static struct _tuple16*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,
struct _tuple8*t){struct _tuple16*_tmp8D2;return(_tmp8D2=_cycalloc(sizeof(*_tmp8D2)),((
_tmp8D2->f1=(*t).f2,((_tmp8D2->f2=(*t).f3,_tmp8D2)))));}static struct Cyc_Absyn_Vardecl*
Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d);static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(
struct Cyc_Absyn_Decl*d){void*_tmp9D=d->r;struct Cyc_Absyn_Vardecl*_tmp9F;_LL3B: {
struct Cyc_Absyn_Var_d_struct*_tmp9E=(struct Cyc_Absyn_Var_d_struct*)_tmp9D;if(
_tmp9E->tag != 0)goto _LL3D;else{_tmp9F=_tmp9E->f1;}}_LL3C: return _tmp9F;_LL3D:;
_LL3E: {const char*_tmp8D5;void*_tmp8D4;(_tmp8D4=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((
_tmp8D5="bad declaration in `forarray' statement",_tag_dyneither(_tmp8D5,sizeof(
char),40))),_tag_dyneither(_tmp8D4,sizeof(void*),0)));}_LL3A:;}static int Cyc_Parse_is_typeparam(
void*tm);static int Cyc_Parse_is_typeparam(void*tm){void*_tmpA2=tm;_LL40: {struct
Cyc_Absyn_TypeParams_mod_struct*_tmpA3=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpA2;if(_tmpA3->tag != 4)goto _LL42;}_LL41: return 1;_LL42:;_LL43: return 0;_LL3F:;}
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k);static void*Cyc_Parse_id2type(
struct _dyneither_ptr s,void*k){const char*_tmp8D6;if(Cyc_zstrcmp((struct
_dyneither_ptr)s,((_tmp8D6="`H",_tag_dyneither(_tmp8D6,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{const char*_tmp8D7;if(Cyc_zstrcmp((
struct _dyneither_ptr)s,((_tmp8D7="`U",_tag_dyneither(_tmp8D7,sizeof(char),3))))
== 0)return(void*)& Cyc_Absyn_UniqueRgn_val;else{struct Cyc_Absyn_VarType_struct
_tmp8E1;struct _dyneither_ptr*_tmp8E0;struct Cyc_Absyn_Tvar*_tmp8DF;struct Cyc_Absyn_VarType_struct*
_tmp8DE;return(void*)((_tmp8DE=_cycalloc(sizeof(*_tmp8DE)),((_tmp8DE[0]=((
_tmp8E1.tag=2,((_tmp8E1.f1=((_tmp8DF=_cycalloc(sizeof(*_tmp8DF)),((_tmp8DF->name=((
_tmp8E0=_cycalloc(sizeof(*_tmp8E0)),((_tmp8E0[0]=s,_tmp8E0)))),((_tmp8DF->identity=
- 1,((_tmp8DF->kind=k,_tmp8DF)))))))),_tmp8E1)))),_tmp8DE))));}}}static struct Cyc_Absyn_Tvar*
Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t);static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar*t){void*k;{void*_tmpAA=Cyc_Absyn_compress_kb(t->kind);
struct Cyc_Absyn_Kind*_tmpAD;_LL45: {struct Cyc_Absyn_Unknown_kb_struct*_tmpAB=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpAA;if(_tmpAB->tag != 1)goto _LL47;}_LL46:{
struct Cyc_Absyn_Unknown_kb_struct _tmp8E4;struct Cyc_Absyn_Unknown_kb_struct*
_tmp8E3;k=(void*)((_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E4.tag=
1,((_tmp8E4.f1=0,_tmp8E4)))),_tmp8E3))));}goto _LL44;_LL47: {struct Cyc_Absyn_Less_kb_struct*
_tmpAC=(struct Cyc_Absyn_Less_kb_struct*)_tmpAA;if(_tmpAC->tag != 2)goto _LL49;
else{_tmpAD=_tmpAC->f2;}}_LL48:{struct Cyc_Absyn_Less_kb_struct _tmp8E7;struct Cyc_Absyn_Less_kb_struct*
_tmp8E6;k=(void*)((_tmp8E6=_cycalloc(sizeof(*_tmp8E6)),((_tmp8E6[0]=((_tmp8E7.tag=
2,((_tmp8E7.f1=0,((_tmp8E7.f2=_tmpAD,_tmp8E7)))))),_tmp8E6))));}goto _LL44;_LL49:;
_LL4A: k=_tmpAA;goto _LL44;_LL44:;}{struct Cyc_Absyn_Tvar*_tmp8E8;return(_tmp8E8=
_cycalloc(sizeof(*_tmp8E8)),((_tmp8E8->name=t->name,((_tmp8E8->identity=- 1,((
_tmp8E8->kind=k,_tmp8E8)))))));};}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(
struct Cyc_Position_Segment*loc,void*t);static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(
struct Cyc_Position_Segment*loc,void*t){void*_tmpB3=t;struct Cyc_Absyn_Tvar*_tmpB5;
_LL4C: {struct Cyc_Absyn_VarType_struct*_tmpB4=(struct Cyc_Absyn_VarType_struct*)
_tmpB3;if(_tmpB4->tag != 2)goto _LL4E;else{_tmpB5=_tmpB4->f1;}}_LL4D: return _tmpB5;
_LL4E:;_LL4F: {const char*_tmp8EB;void*_tmp8EA;(_tmp8EA=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp8EB="expecting a list of type variables, not types",_tag_dyneither(_tmp8EB,
sizeof(char),46))),_tag_dyneither(_tmp8EA,sizeof(void*),0)));}_LL4B:;}static void*
Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr);static void*Cyc_Parse_tvar2typ(struct
Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct _tmp8EE;struct Cyc_Absyn_VarType_struct*
_tmp8ED;return(void*)((_tmp8ED=_cycalloc(sizeof(*_tmp8ED)),((_tmp8ED[0]=((
_tmp8EE.tag=2,((_tmp8EE.f1=pr,_tmp8EE)))),_tmp8ED))));}static void Cyc_Parse_set_vartyp_kind(
void*t,struct Cyc_Absyn_Kind*k,int leq);static void Cyc_Parse_set_vartyp_kind(void*t,
struct Cyc_Absyn_Kind*k,int leq){void*_tmpBA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmpBC;struct Cyc_Absyn_Tvar _tmpBD;void*_tmpBE;void**_tmpBF;_LL51: {struct Cyc_Absyn_VarType_struct*
_tmpBB=(struct Cyc_Absyn_VarType_struct*)_tmpBA;if(_tmpBB->tag != 2)goto _LL53;
else{_tmpBC=_tmpBB->f1;_tmpBD=*_tmpBC;_tmpBE=_tmpBD.kind;_tmpBF=(void**)&(*
_tmpBB->f1).kind;}}_LL52: {void*_tmpC0=Cyc_Absyn_compress_kb(*_tmpBF);_LL56: {
struct Cyc_Absyn_Unknown_kb_struct*_tmpC1=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC0;if(_tmpC1->tag != 1)goto _LL58;}_LL57: if(!leq)*_tmpBF=Cyc_Tcutil_kind_to_bound(
k);else{struct Cyc_Absyn_Less_kb_struct _tmp8F1;struct Cyc_Absyn_Less_kb_struct*
_tmp8F0;*_tmpBF=(void*)((_tmp8F0=_cycalloc(sizeof(*_tmp8F0)),((_tmp8F0[0]=((
_tmp8F1.tag=2,((_tmp8F1.f1=0,((_tmp8F1.f2=k,_tmp8F1)))))),_tmp8F0))));}return;
_LL58:;_LL59: return;_LL55:;}_LL53:;_LL54: return;_LL50:;}static struct Cyc_List_List*
Cyc_Parse_oldstyle2newstyle(struct Cyc_List_List*tms,struct Cyc_List_List*tds,
struct Cyc_Position_Segment*loc);static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmpC4=(void*)tms->hd;void*_tmpC6;
_LL5B: {struct Cyc_Absyn_Function_mod_struct*_tmpC5=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC4;if(_tmpC5->tag != 3)goto _LL5D;else{_tmpC6=(void*)_tmpC5->f1;}}_LL5C: if(tms->tl
== 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)
 && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){void*_tmpC7=_tmpC6;
struct Cyc_List_List*_tmpCA;_LL60: {struct Cyc_Absyn_WithTypes_struct*_tmpC8=(
struct Cyc_Absyn_WithTypes_struct*)_tmpC7;if(_tmpC8->tag != 1)goto _LL62;}_LL61:{
const char*_tmp8F4;void*_tmp8F3;(_tmp8F3=0,Cyc_Tcutil_warn(loc,((_tmp8F4="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp8F4,sizeof(char),93))),_tag_dyneither(_tmp8F3,sizeof(void*),0)));}
return tms;_LL62: {struct Cyc_Absyn_NoTypes_struct*_tmpC9=(struct Cyc_Absyn_NoTypes_struct*)
_tmpC7;if(_tmpC9->tag != 0)goto _LL5F;else{_tmpCA=_tmpC9->f1;}}_LL63: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpCA)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds)){const char*_tmp8F7;void*_tmp8F6;(_tmp8F6=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
loc,((_tmp8F7="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp8F7,sizeof(char),73))),_tag_dyneither(_tmp8F6,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpCF=0;for(0;_tmpCA != 0;_tmpCA=_tmpCA->tl){struct Cyc_List_List*
_tmpD0=tds;for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){struct Cyc_Absyn_Decl*_tmpD1=(
struct Cyc_Absyn_Decl*)_tmpD0->hd;void*_tmpD2=_tmpD1->r;struct Cyc_Absyn_Vardecl*
_tmpD4;_LL65: {struct Cyc_Absyn_Var_d_struct*_tmpD3=(struct Cyc_Absyn_Var_d_struct*)
_tmpD2;if(_tmpD3->tag != 0)goto _LL67;else{_tmpD4=_tmpD3->f1;}}_LL66: if(Cyc_zstrptrcmp((*
_tmpD4->name).f2,(struct _dyneither_ptr*)_tmpCA->hd)!= 0)continue;if(_tmpD4->initializer
!= 0){const char*_tmp8FA;void*_tmp8F9;(_tmp8F9=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,((
_tmp8FA="initializer found in parameter declaration",_tag_dyneither(_tmp8FA,
sizeof(char),43))),_tag_dyneither(_tmp8F9,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmpD4->name)){const char*_tmp8FD;void*_tmp8FC;(_tmp8FC=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,((
_tmp8FD="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8FD,
sizeof(char),47))),_tag_dyneither(_tmp8FC,sizeof(void*),0)));}{struct _tuple8*
_tmp903;struct Cyc_Core_Opt*_tmp902;struct Cyc_List_List*_tmp901;_tmpCF=((_tmp901=
_cycalloc(sizeof(*_tmp901)),((_tmp901->hd=((_tmp903=_cycalloc(sizeof(*_tmp903)),((
_tmp903->f1=((_tmp902=_cycalloc(sizeof(*_tmp902)),((_tmp902->v=(*_tmpD4->name).f2,
_tmp902)))),((_tmp903->f2=_tmpD4->tq,((_tmp903->f3=_tmpD4->type,_tmp903)))))))),((
_tmp901->tl=_tmpCF,_tmp901))))));}goto L;_LL67:;_LL68: {const char*_tmp906;void*
_tmp905;(_tmp905=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,((_tmp906="nonvariable declaration in parameter type",
_tag_dyneither(_tmp906,sizeof(char),42))),_tag_dyneither(_tmp905,sizeof(void*),0)));}
_LL64:;}L: if(_tmpD0 == 0){const char*_tmp90A;void*_tmp909[1];struct Cyc_String_pa_struct
_tmp908;(_tmp908.tag=0,((_tmp908.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmpCA->hd)),((_tmp909[0]=& _tmp908,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp90A="%s is not given a type",_tag_dyneither(_tmp90A,sizeof(char),23))),
_tag_dyneither(_tmp909,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp919;struct Cyc_Absyn_WithTypes_struct*_tmp918;struct Cyc_Absyn_WithTypes_struct
_tmp917;struct Cyc_Absyn_Function_mod_struct _tmp916;struct Cyc_List_List*_tmp915;
return(_tmp915=_cycalloc(sizeof(*_tmp915)),((_tmp915->hd=(void*)((_tmp919=
_cycalloc(sizeof(*_tmp919)),((_tmp919[0]=((_tmp916.tag=3,((_tmp916.f1=(void*)((
void*)((_tmp918=_cycalloc(sizeof(*_tmp918)),((_tmp918[0]=((_tmp917.tag=1,((
_tmp917.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpCF),((_tmp917.f2=0,((_tmp917.f3=0,((_tmp917.f4=0,((_tmp917.f5=0,_tmp917)))))))))))),
_tmp918))))),_tmp916)))),_tmp919)))),((_tmp915->tl=0,_tmp915)))));};};_LL5F:;}
goto _LL5E;_LL5D:;_LL5E: {struct Cyc_List_List*_tmp91A;return(_tmp91A=_cycalloc(
sizeof(*_tmp91A)),((_tmp91A->hd=(void*)tms->hd,((_tmp91A->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp91A)))));}_LL5A:;};}struct _tuple17{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc);static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,
struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){
if(tds != 0){struct Cyc_Parse_Declarator*_tmp91B;d=((_tmp91B=_cycalloc(sizeof(*
_tmp91B)),((_tmp91B->id=d->id,((_tmp91B->tms=Cyc_Parse_oldstyle2newstyle(d->tms,
tds,loc),_tmp91B))))));}{enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;struct Cyc_List_List*
tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*
atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;
tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)
dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;
if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0)switch((enum Cyc_Parse_Storage_class)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc))->v){
case Cyc_Parse_Extern_sc: _LL69: sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc:
_LL6A: sc=Cyc_Absyn_Static;break;default: _LL6B:{const char*_tmp91C;Cyc_Parse_err(((
_tmp91C="bad storage class on function",_tag_dyneither(_tmp91C,sizeof(char),30))),
loc);}break;}}{void*_tmpEA;struct Cyc_Core_Opt*_tmpEB;struct _tuple10 _tmpE9=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpEA=_tmpE9.f1;_tmpEB=_tmpE9.f2;{struct Cyc_Absyn_Tqual _tmpED;void*
_tmpEE;struct Cyc_List_List*_tmpEF;struct Cyc_List_List*_tmpF0;struct _tuple11
_tmpEC=Cyc_Parse_apply_tms(tq,_tmpEA,atts,d->tms);_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;
_tmpEF=_tmpEC.f3;_tmpF0=_tmpEC.f4;if(_tmpEB != 0){const char*_tmp91F;void*_tmp91E;(
_tmp91E=0,Cyc_Tcutil_warn(loc,((_tmp91F="nested type declaration within function prototype",
_tag_dyneither(_tmp91F,sizeof(char),50))),_tag_dyneither(_tmp91E,sizeof(void*),0)));}
if(_tmpEF != 0){const char*_tmp922;void*_tmp921;(_tmp921=0,Cyc_Tcutil_warn(loc,((
_tmp922="bad type params, ignoring",_tag_dyneither(_tmp922,sizeof(char),26))),
_tag_dyneither(_tmp921,sizeof(void*),0)));}{void*_tmpF5=_tmpEE;struct Cyc_Absyn_FnInfo
_tmpF7;struct Cyc_List_List*_tmpF8;struct Cyc_Core_Opt*_tmpF9;void*_tmpFA;struct
Cyc_List_List*_tmpFB;int _tmpFC;struct Cyc_Absyn_VarargInfo*_tmpFD;struct Cyc_List_List*
_tmpFE;struct Cyc_List_List*_tmpFF;_LL6E: {struct Cyc_Absyn_FnType_struct*_tmpF6=(
struct Cyc_Absyn_FnType_struct*)_tmpF5;if(_tmpF6->tag != 10)goto _LL70;else{_tmpF7=
_tmpF6->f1;_tmpF8=_tmpF7.tvars;_tmpF9=_tmpF7.effect;_tmpFA=_tmpF7.ret_typ;_tmpFB=
_tmpF7.args;_tmpFC=_tmpF7.c_varargs;_tmpFD=_tmpF7.cyc_varargs;_tmpFE=_tmpF7.rgn_po;
_tmpFF=_tmpF7.attributes;}}_LL6F: {struct Cyc_List_List*_tmp100=0;{struct Cyc_List_List*
_tmp101=_tmpFB;for(0;_tmp101 != 0;_tmp101=_tmp101->tl){struct _tuple8 _tmp103;
struct Cyc_Core_Opt*_tmp104;struct Cyc_Absyn_Tqual _tmp105;void*_tmp106;struct
_tuple8*_tmp102=(struct _tuple8*)_tmp101->hd;_tmp103=*_tmp102;_tmp104=_tmp103.f1;
_tmp105=_tmp103.f2;_tmp106=_tmp103.f3;if(_tmp104 == 0){{const char*_tmp923;Cyc_Parse_err(((
_tmp923="missing argument variable in function prototype",_tag_dyneither(_tmp923,
sizeof(char),48))),loc);}{struct _tuple17*_tmp926;struct Cyc_List_List*_tmp925;
_tmp100=((_tmp925=_cycalloc(sizeof(*_tmp925)),((_tmp925->hd=((_tmp926=_cycalloc(
sizeof(*_tmp926)),((_tmp926->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"?",sizeof(char),2),((_tmp926->f2=_tmp105,((_tmp926->f3=_tmp106,
_tmp926)))))))),((_tmp925->tl=_tmp100,_tmp925))))));};}else{struct _tuple17*
_tmp929;struct Cyc_List_List*_tmp928;_tmp100=((_tmp928=_cycalloc(sizeof(*_tmp928)),((
_tmp928->hd=((_tmp929=_cycalloc(sizeof(*_tmp929)),((_tmp929->f1=(struct
_dyneither_ptr*)_tmp104->v,((_tmp929->f2=_tmp105,((_tmp929->f3=_tmp106,_tmp929)))))))),((
_tmp928->tl=_tmp100,_tmp928))))));}}}{struct Cyc_Absyn_Fndecl*_tmp92A;return(
_tmp92A=_cycalloc(sizeof(*_tmp92A)),((_tmp92A->sc=sc,((_tmp92A->name=d->id,((
_tmp92A->tvs=_tmpF8,((_tmp92A->is_inline=is_inline,((_tmp92A->effect=_tmpF9,((
_tmp92A->ret_type=_tmpFA,((_tmp92A->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp100),((_tmp92A->c_varargs=_tmpFC,((_tmp92A->cyc_varargs=
_tmpFD,((_tmp92A->rgn_po=_tmpFE,((_tmp92A->body=body,((_tmp92A->cached_typ=0,((
_tmp92A->param_vardecls=0,((_tmp92A->fn_vardecl=0,((_tmp92A->attributes=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmpFF,_tmpF0),_tmp92A)))))))))))))))))))))))))))))));};}_LL70:;_LL71: {const
char*_tmp92D;void*_tmp92C;(_tmp92C=0,((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp92D="declarator is not a function prototype",
_tag_dyneither(_tmp92D,sizeof(char),39))),_tag_dyneither(_tmp92C,sizeof(void*),0)));}
_LL6D:;};};};};}static char _tmp110[52]="at most one type may appear within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg1={_tmp110,_tmp110,_tmp110 + 52};static
char _tmp111[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp111,_tmp111,_tmp111 + 63};static
char _tmp112[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp112,_tmp112,_tmp112 + 50};static char _tmp113[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp113,_tmp113,_tmp113 + 60};static
struct _tuple10 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct
Cyc_Position_Segment*loc);static struct _tuple10 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*
declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;void*t=(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp114=(
void*)ts->hd;void*_tmp116;struct Cyc_Position_Segment*_tmp117;struct Cyc_Position_Segment*
_tmp119;struct Cyc_Position_Segment*_tmp11B;struct Cyc_Position_Segment*_tmp11D;
struct Cyc_Position_Segment*_tmp11F;struct Cyc_Absyn_Decl*_tmp121;_LL73: {struct
Cyc_Parse_Type_spec_struct*_tmp115=(struct Cyc_Parse_Type_spec_struct*)_tmp114;
if(_tmp115->tag != 4)goto _LL75;else{_tmp116=(void*)_tmp115->f1;_tmp117=_tmp115->f2;}}
_LL74: if(seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp117);last_loc=_tmp117;
seen_type=1;t=_tmp116;goto _LL72;_LL75: {struct Cyc_Parse_Signed_spec_struct*
_tmp118=(struct Cyc_Parse_Signed_spec_struct*)_tmp114;if(_tmp118->tag != 0)goto
_LL77;else{_tmp119=_tmp118->f1;}}_LL76: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,
_tmp119);if(seen_type){const char*_tmp92E;Cyc_Parse_err(((_tmp92E="signed qualifier must come before type",
_tag_dyneither(_tmp92E,sizeof(char),39))),_tmp119);}last_loc=_tmp119;seen_sign=1;
sgn=Cyc_Absyn_Signed;goto _LL72;_LL77: {struct Cyc_Parse_Unsigned_spec_struct*
_tmp11A=(struct Cyc_Parse_Unsigned_spec_struct*)_tmp114;if(_tmp11A->tag != 1)goto
_LL79;else{_tmp11B=_tmp11A->f1;}}_LL78: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,
_tmp11B);if(seen_type){const char*_tmp92F;Cyc_Parse_err(((_tmp92F="signed qualifier must come before type",
_tag_dyneither(_tmp92F,sizeof(char),39))),_tmp11B);}last_loc=_tmp11B;seen_sign=1;
sgn=Cyc_Absyn_Unsigned;goto _LL72;_LL79: {struct Cyc_Parse_Short_spec_struct*
_tmp11C=(struct Cyc_Parse_Short_spec_struct*)_tmp114;if(_tmp11C->tag != 2)goto
_LL7B;else{_tmp11D=_tmp11C->f1;}}_LL7A: if(seen_size){const char*_tmp930;Cyc_Parse_err(((
_tmp930="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp930,sizeof(char),59))),_tmp11D);}if(seen_type){const char*
_tmp931;Cyc_Parse_err(((_tmp931="short modifier must come before base type",
_tag_dyneither(_tmp931,sizeof(char),42))),_tmp11D);}last_loc=_tmp11D;seen_size=1;
sz=Cyc_Absyn_Short_sz;goto _LL72;_LL7B: {struct Cyc_Parse_Long_spec_struct*_tmp11E=(
struct Cyc_Parse_Long_spec_struct*)_tmp114;if(_tmp11E->tag != 3)goto _LL7D;else{
_tmp11F=_tmp11E->f1;}}_LL7C: if(seen_type){const char*_tmp932;Cyc_Parse_err(((
_tmp932="long modifier must come before base type",_tag_dyneither(_tmp932,
sizeof(char),41))),_tmp11F);}if(seen_size)switch(sz){case Cyc_Absyn_Long_sz: _LL7F:
sz=Cyc_Absyn_LongLong_sz;break;default: _LL80:{const char*_tmp933;Cyc_Parse_err(((
_tmp933="extra long in type specifier",_tag_dyneither(_tmp933,sizeof(char),29))),
_tmp11F);}break;}else{sz=Cyc_Absyn_Long_sz;}last_loc=_tmp11F;seen_size=1;goto
_LL72;_LL7D: {struct Cyc_Parse_Decl_spec_struct*_tmp120=(struct Cyc_Parse_Decl_spec_struct*)
_tmp114;if(_tmp120->tag != 5)goto _LL72;else{_tmp121=_tmp120->f1;}}_LL7E: last_loc=
_tmp121->loc;if(declopt == 0  && !seen_type){seen_type=1;{void*_tmp128=_tmp121->r;
struct Cyc_Absyn_Aggrdecl*_tmp12A;struct Cyc_Absyn_Datatypedecl*_tmp12C;struct Cyc_Absyn_Enumdecl*
_tmp12E;_LL83: {struct Cyc_Absyn_Aggr_d_struct*_tmp129=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp128;if(_tmp129->tag != 6)goto _LL85;else{_tmp12A=_tmp129->f1;}}_LL84: {struct
Cyc_List_List*_tmp12F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmp12A->tvs));{struct Cyc_Absyn_AggrType_struct _tmp939;
struct Cyc_Absyn_AggrInfo _tmp938;struct Cyc_Absyn_AggrType_struct*_tmp937;t=(void*)((
_tmp937=_cycalloc(sizeof(*_tmp937)),((_tmp937[0]=((_tmp939.tag=12,((_tmp939.f1=((
_tmp938.aggr_info=Cyc_Absyn_UnknownAggr(_tmp12A->kind,_tmp12A->name,0),((_tmp938.targs=
_tmp12F,_tmp938)))),_tmp939)))),_tmp937))));}if(_tmp12A->impl != 0){struct Cyc_Core_Opt*
_tmp93A;declopt=((_tmp93A=_cycalloc(sizeof(*_tmp93A)),((_tmp93A->v=_tmp121,
_tmp93A))));}goto _LL82;}_LL85: {struct Cyc_Absyn_Datatype_d_struct*_tmp12B=(
struct Cyc_Absyn_Datatype_d_struct*)_tmp128;if(_tmp12B->tag != 7)goto _LL87;else{
_tmp12C=_tmp12B->f1;}}_LL86: {struct Cyc_List_List*_tmp134=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmp12C->tvs));{struct Cyc_Absyn_DatatypeType_struct
_tmp944;struct Cyc_Absyn_Datatypedecl**_tmp943;struct Cyc_Absyn_DatatypeInfo
_tmp942;struct Cyc_Absyn_DatatypeType_struct*_tmp941;t=(void*)((_tmp941=_cycalloc(
sizeof(*_tmp941)),((_tmp941[0]=((_tmp944.tag=3,((_tmp944.f1=((_tmp942.datatype_info=
Cyc_Absyn_KnownDatatype(((_tmp943=_cycalloc(sizeof(*_tmp943)),((_tmp943[0]=
_tmp12C,_tmp943))))),((_tmp942.targs=_tmp134,_tmp942)))),_tmp944)))),_tmp941))));}
if(_tmp12C->fields != 0){struct Cyc_Core_Opt*_tmp945;declopt=((_tmp945=_cycalloc(
sizeof(*_tmp945)),((_tmp945->v=_tmp121,_tmp945))));}goto _LL82;}_LL87: {struct Cyc_Absyn_Enum_d_struct*
_tmp12D=(struct Cyc_Absyn_Enum_d_struct*)_tmp128;if(_tmp12D->tag != 8)goto _LL89;
else{_tmp12E=_tmp12D->f1;}}_LL88:{struct Cyc_Absyn_EnumType_struct _tmp948;struct
Cyc_Absyn_EnumType_struct*_tmp947;t=(void*)((_tmp947=_cycalloc(sizeof(*_tmp947)),((
_tmp947[0]=((_tmp948.tag=14,((_tmp948.f1=_tmp12E->name,((_tmp948.f2=0,_tmp948)))))),
_tmp947))));}{struct Cyc_Core_Opt*_tmp949;declopt=((_tmp949=_cycalloc(sizeof(*
_tmp949)),((_tmp949->v=_tmp121,_tmp949))));}goto _LL82;_LL89:;_LL8A: {const char*
_tmp94C;void*_tmp94B;(_tmp94B=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp121->loc,((
_tmp94C="bad declaration within type specifier",_tag_dyneither(_tmp94C,sizeof(
char),38))),_tag_dyneither(_tmp94B,sizeof(void*),0)));}_LL82:;};}else{Cyc_Parse_err(
Cyc_Parse_msg3,_tmp121->loc);}goto _LL72;_LL72:;}if(!seen_type){if(!seen_sign  && 
!seen_size){const char*_tmp94F;void*_tmp94E;(_tmp94E=0,Cyc_Tcutil_warn(last_loc,((
_tmp94F="missing type within specifier",_tag_dyneither(_tmp94F,sizeof(char),30))),
_tag_dyneither(_tmp94E,sizeof(void*),0)));}t=Cyc_Absyn_int_typ(sgn,sz);}else{if(
seen_sign){void*_tmp141=t;enum Cyc_Absyn_Sign _tmp143;enum Cyc_Absyn_Size_of
_tmp144;_LL8C: {struct Cyc_Absyn_IntType_struct*_tmp142=(struct Cyc_Absyn_IntType_struct*)
_tmp141;if(_tmp142->tag != 6)goto _LL8E;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;}}
_LL8D: if(_tmp143 != sgn)t=Cyc_Absyn_int_typ(sgn,_tmp144);goto _LL8B;_LL8E:;_LL8F:{
const char*_tmp950;Cyc_Parse_err(((_tmp950="sign specification on non-integral type",
_tag_dyneither(_tmp950,sizeof(char),40))),last_loc);}goto _LL8B;_LL8B:;}if(
seen_size){void*_tmp146=t;enum Cyc_Absyn_Sign _tmp148;enum Cyc_Absyn_Size_of
_tmp149;_LL91: {struct Cyc_Absyn_IntType_struct*_tmp147=(struct Cyc_Absyn_IntType_struct*)
_tmp146;if(_tmp147->tag != 6)goto _LL93;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}
_LL92: if(_tmp149 != sz)t=Cyc_Absyn_int_typ(_tmp148,sz);goto _LL90;_LL93: {struct
Cyc_Absyn_DoubleType_struct*_tmp14A=(struct Cyc_Absyn_DoubleType_struct*)_tmp146;
if(_tmp14A->tag != 8)goto _LL95;}_LL94: t=Cyc_Absyn_double_typ(1);goto _LL90;_LL95:;
_LL96:{const char*_tmp951;Cyc_Parse_err(((_tmp951="size qualifier on non-integral type",
_tag_dyneither(_tmp951,sizeof(char),36))),last_loc);}goto _LL90;_LL90:;}}{struct
_tuple10 _tmp952;return(_tmp952.f1=t,((_tmp952.f2=declopt,_tmp952)));};}static
struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts);static struct
Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,
void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{
struct Cyc_Parse_Declarator*_tmp14D=(struct Cyc_Parse_Declarator*)ds->hd;struct
_tuple1*_tmp14E=_tmp14D->id;struct Cyc_Absyn_Tqual _tmp150;void*_tmp151;struct Cyc_List_List*
_tmp152;struct Cyc_List_List*_tmp153;struct _tuple11 _tmp14F=Cyc_Parse_apply_tms(tq,
t,shared_atts,_tmp14D->tms);_tmp150=_tmp14F.f1;_tmp151=_tmp14F.f2;_tmp152=
_tmp14F.f3;_tmp153=_tmp14F.f4;if(ds->tl == 0){struct _tuple13*_tmp955;struct Cyc_List_List*
_tmp954;return(_tmp954=_region_malloc(r,sizeof(*_tmp954)),((_tmp954->hd=((
_tmp955=_region_malloc(r,sizeof(*_tmp955)),((_tmp955->f1=_tmp14E,((_tmp955->f2=
_tmp150,((_tmp955->f3=_tmp151,((_tmp955->f4=_tmp152,((_tmp955->f5=_tmp153,
_tmp955)))))))))))),((_tmp954->tl=0,_tmp954)))));}else{struct _tuple13*_tmp958;
struct Cyc_List_List*_tmp957;return(_tmp957=_region_malloc(r,sizeof(*_tmp957)),((
_tmp957->hd=((_tmp958=_region_malloc(r,sizeof(*_tmp958)),((_tmp958->f1=_tmp14E,((
_tmp958->f2=_tmp150,((_tmp958->f3=_tmp151,((_tmp958->f4=_tmp152,((_tmp958->f5=
_tmp153,_tmp958)))))))))))),((_tmp957->tl=Cyc_Parse_apply_tmss(r,_tmp150,Cyc_Tcutil_copy_type(
t),ds->tl,shared_atts),_tmp957)))));}};}static struct _tuple11 Cyc_Parse_apply_tms(
struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms);
static struct _tuple11 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0){struct _tuple11 _tmp959;return(_tmp959.f1=
tq,((_tmp959.f2=t,((_tmp959.f3=0,((_tmp959.f4=atts,_tmp959)))))));}{void*_tmp159=(
void*)tms->hd;union Cyc_Absyn_Constraint*_tmp15B;struct Cyc_Position_Segment*
_tmp15C;struct Cyc_Absyn_Exp*_tmp15E;union Cyc_Absyn_Constraint*_tmp15F;struct Cyc_Position_Segment*
_tmp160;void*_tmp162;struct Cyc_List_List*_tmp164;struct Cyc_Position_Segment*
_tmp165;struct Cyc_Absyn_PtrAtts _tmp167;struct Cyc_Absyn_Tqual _tmp168;struct Cyc_Position_Segment*
_tmp16A;struct Cyc_List_List*_tmp16B;_LL98: {struct Cyc_Absyn_Carray_mod_struct*
_tmp15A=(struct Cyc_Absyn_Carray_mod_struct*)_tmp159;if(_tmp15A->tag != 0)goto
_LL9A;else{_tmp15B=_tmp15A->f1;_tmp15C=_tmp15A->f2;}}_LL99: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmp15B,_tmp15C),atts,tms->tl);
_LL9A: {struct Cyc_Absyn_ConstArray_mod_struct*_tmp15D=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp159;if(_tmp15D->tag != 1)goto _LL9C;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15D->f2;
_tmp160=_tmp15D->f3;}}_LL9B: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmp15E,_tmp15F,_tmp160),atts,tms->tl);
_LL9C: {struct Cyc_Absyn_Function_mod_struct*_tmp161=(struct Cyc_Absyn_Function_mod_struct*)
_tmp159;if(_tmp161->tag != 3)goto _LL9E;else{_tmp162=(void*)_tmp161->f1;}}_LL9D: {
void*_tmp16C=_tmp162;struct Cyc_List_List*_tmp16E;int _tmp16F;struct Cyc_Absyn_VarargInfo*
_tmp170;struct Cyc_Core_Opt*_tmp171;struct Cyc_List_List*_tmp172;struct Cyc_Position_Segment*
_tmp174;_LLA5: {struct Cyc_Absyn_WithTypes_struct*_tmp16D=(struct Cyc_Absyn_WithTypes_struct*)
_tmp16C;if(_tmp16D->tag != 1)goto _LLA7;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;
_tmp170=_tmp16D->f3;_tmp171=_tmp16D->f4;_tmp172=_tmp16D->f5;}}_LLA6: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp95A;fn_atts=((_tmp95A=_cycalloc(sizeof(*_tmp95A)),((
_tmp95A->hd=(void*)as->hd,((_tmp95A->tl=fn_atts,_tmp95A))))));}else{struct Cyc_List_List*
_tmp95B;new_atts=((_tmp95B=_cycalloc(sizeof(*_tmp95B)),((_tmp95B->hd=(void*)as->hd,((
_tmp95B->tl=new_atts,_tmp95B))))));}}}if(tms->tl != 0){void*_tmp177=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;struct Cyc_List_List*_tmp179;_LLAA: {
struct Cyc_Absyn_TypeParams_mod_struct*_tmp178=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp177;if(_tmp178->tag != 4)goto _LLAC;else{_tmp179=_tmp178->f1;}}_LLAB: typvars=
_tmp179;tms=tms->tl;goto _LLA9;_LLAC:;_LLAD: goto _LLA9;_LLA9:;}if(((((!_tmp16F  && 
_tmp170 == 0) && _tmp16E != 0) && _tmp16E->tl == 0) && (*((struct _tuple8*)_tmp16E->hd)).f1
== 0) && (*((struct _tuple8*)_tmp16E->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
_tmp16E=0;{struct Cyc_List_List*_tmp17A=Cyc_Parse_get_arg_tags(_tmp16E);if(
_tmp17A != 0)t=Cyc_Parse_substitute_tags(_tmp17A,t);t=Cyc_Parse_array2ptr(t,0);{
struct Cyc_List_List*_tmp17B=_tmp16E;for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){
struct _tuple8 _tmp17D;struct Cyc_Core_Opt*_tmp17E;struct Cyc_Absyn_Tqual _tmp17F;
void*_tmp180;void**_tmp181;struct _tuple8*_tmp17C=(struct _tuple8*)_tmp17B->hd;
_tmp17D=*_tmp17C;_tmp17E=_tmp17D.f1;_tmp17F=_tmp17D.f2;_tmp180=_tmp17D.f3;
_tmp181=(void**)&(*_tmp17C).f3;if(_tmp17A != 0)*_tmp181=Cyc_Parse_substitute_tags(
_tmp17A,*_tmp181);*_tmp181=Cyc_Parse_array2ptr(*_tmp181,1);}}return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(tq.loc),Cyc_Absyn_function_typ(typvars,_tmp171,t,_tmp16E,
_tmp16F,_tmp170,_tmp172,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}
_LLA7: {struct Cyc_Absyn_NoTypes_struct*_tmp173=(struct Cyc_Absyn_NoTypes_struct*)
_tmp16C;if(_tmp173->tag != 0)goto _LLA4;else{_tmp174=_tmp173->f2;}}_LLA8: {const
char*_tmp95E;void*_tmp95D;(_tmp95D=0,((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp174,((
_tmp95E="function declaration without parameter types",_tag_dyneither(_tmp95E,
sizeof(char),45))),_tag_dyneither(_tmp95D,sizeof(void*),0)));}_LLA4:;}_LL9E: {
struct Cyc_Absyn_TypeParams_mod_struct*_tmp163=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp159;if(_tmp163->tag != 4)goto _LLA0;else{_tmp164=_tmp163->f1;_tmp165=_tmp163->f2;}}
_LL9F: if(tms->tl == 0){struct _tuple11 _tmp95F;return(_tmp95F.f1=tq,((_tmp95F.f2=t,((
_tmp95F.f3=_tmp164,((_tmp95F.f4=atts,_tmp95F)))))));}{const char*_tmp962;void*
_tmp961;(_tmp961=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp165,((_tmp962="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp962,sizeof(char),68))),_tag_dyneither(_tmp961,sizeof(void*),0)));};
_LLA0: {struct Cyc_Absyn_Pointer_mod_struct*_tmp166=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp159;if(_tmp166->tag != 2)goto _LLA2;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}
_LLA1: {struct Cyc_Absyn_PointerType_struct _tmp968;struct Cyc_Absyn_PtrInfo _tmp967;
struct Cyc_Absyn_PointerType_struct*_tmp966;return Cyc_Parse_apply_tms(_tmp168,(
void*)((_tmp966=_cycalloc(sizeof(*_tmp966)),((_tmp966[0]=((_tmp968.tag=5,((
_tmp968.f1=((_tmp967.elt_typ=t,((_tmp967.elt_tq=tq,((_tmp967.ptr_atts=_tmp167,
_tmp967)))))),_tmp968)))),_tmp966)))),atts,tms->tl);}_LLA2: {struct Cyc_Absyn_Attributes_mod_struct*
_tmp169=(struct Cyc_Absyn_Attributes_mod_struct*)_tmp159;if(_tmp169->tag != 5)goto
_LL97;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;}}_LLA3: return Cyc_Parse_apply_tms(
tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
atts,_tmp16B),tms->tl);_LL97:;};}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*
tss,struct Cyc_Position_Segment*loc);void*Cyc_Parse_speclist2typ(struct Cyc_List_List*
tss,struct Cyc_Position_Segment*loc){void*_tmp18B;struct Cyc_Core_Opt*_tmp18C;
struct _tuple10 _tmp18A=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmp18B=
_tmp18A.f1;_tmp18C=_tmp18A.f2;if(_tmp18C != 0){const char*_tmp96B;void*_tmp96A;(
_tmp96A=0,Cyc_Tcutil_warn(loc,((_tmp96B="ignoring nested type declaration(s) in specifier list",
_tag_dyneither(_tmp96B,sizeof(char),54))),_tag_dyneither(_tmp96A,sizeof(void*),0)));}
return _tmp18B;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple13*t);static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment*loc,struct _tuple13*t){struct _tuple13 _tmp190;struct
_tuple1*_tmp191;struct Cyc_Absyn_Tqual _tmp192;void*_tmp193;struct Cyc_List_List*
_tmp194;struct Cyc_List_List*_tmp195;struct _tuple13*_tmp18F=t;_tmp190=*_tmp18F;
_tmp191=_tmp190.f1;_tmp192=_tmp190.f2;_tmp193=_tmp190.f3;_tmp194=_tmp190.f4;
_tmp195=_tmp190.f5;Cyc_Lex_register_typedef(_tmp191);{struct Cyc_Core_Opt*kind;
struct Cyc_Core_Opt*type;{void*_tmp196=_tmp193;struct Cyc_Core_Opt*_tmp198;_LLAF: {
struct Cyc_Absyn_Evar_struct*_tmp197=(struct Cyc_Absyn_Evar_struct*)_tmp196;if(
_tmp197->tag != 1)goto _LLB1;else{_tmp198=_tmp197->f1;}}_LLB0: type=0;if(_tmp198 == 
0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;else{kind=_tmp198;}goto _LLAE;_LLB1:;
_LLB2: kind=0;{struct Cyc_Core_Opt*_tmp96C;type=((_tmp96C=_cycalloc(sizeof(*
_tmp96C)),((_tmp96C->v=_tmp193,_tmp96C))));}goto _LLAE;_LLAE:;}{struct Cyc_Absyn_Typedef_d_struct
_tmp972;struct Cyc_Absyn_Typedefdecl*_tmp971;struct Cyc_Absyn_Typedef_d_struct*
_tmp970;return Cyc_Absyn_new_decl((void*)((_tmp970=_cycalloc(sizeof(*_tmp970)),((
_tmp970[0]=((_tmp972.tag=9,((_tmp972.f1=((_tmp971=_cycalloc(sizeof(*_tmp971)),((
_tmp971->name=_tmp191,((_tmp971->tvs=_tmp194,((_tmp971->kind=kind,((_tmp971->defn=
type,((_tmp971->atts=_tmp195,((_tmp971->tq=_tmp192,_tmp971)))))))))))))),_tmp972)))),
_tmp970)))),loc);};};}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Decl_s_struct
_tmp975;struct Cyc_Absyn_Decl_s_struct*_tmp974;return Cyc_Absyn_new_stmt((void*)((
_tmp974=_cycalloc(sizeof(*_tmp974)),((_tmp974[0]=((_tmp975.tag=12,((_tmp975.f1=d,((
_tmp975.f2=s,_tmp975)))))),_tmp974)))),d->loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc);static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc){struct _RegionHandle _tmp19F=_new_region("mkrgn");struct _RegionHandle*mkrgn=&
_tmp19F;_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp1A1;struct Cyc_Absyn_Tqual
_tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_List_List*_tmp1A4;struct Cyc_Parse_Declaration_spec*
_tmp1A0=ds;_tmp1A1=*_tmp1A0;_tmp1A2=_tmp1A1.tq;_tmp1A3=_tmp1A1.type_specs;
_tmp1A4=_tmp1A1.attributes;if(_tmp1A2.loc == 0)_tmp1A2.loc=tqual_loc;if(ds->is_inline){
const char*_tmp976;Cyc_Parse_err(((_tmp976="inline is allowed only on function definitions",
_tag_dyneither(_tmp976,sizeof(char),47))),loc);}if(_tmp1A3 == 0){{const char*
_tmp977;Cyc_Parse_err(((_tmp977="missing type specifiers in declaration",
_tag_dyneither(_tmp977,sizeof(char),39))),loc);}{struct Cyc_List_List*_tmp1A7=0;
_npop_handler(0);return _tmp1A7;};}{enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;int
istypedef=0;if(ds->sc != 0)switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v){case Cyc_Parse_Typedef_sc: _LLB3: istypedef=1;break;case
Cyc_Parse_Extern_sc: _LLB4: s=Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc:
_LLB5: s=Cyc_Absyn_ExternC;break;case Cyc_Parse_Static_sc: _LLB6: s=Cyc_Absyn_Static;
break;case Cyc_Parse_Auto_sc: _LLB7: s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc:
_LLB8: if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}
break;case Cyc_Parse_Abstract_sc: _LLB9: s=Cyc_Absyn_Abstract;break;}{struct Cyc_List_List*
_tmp1A9;struct Cyc_List_List*_tmp1AA;struct _tuple0 _tmp1A8=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
mkrgn,mkrgn,ids);_tmp1A9=_tmp1A8.f1;_tmp1AA=_tmp1A8.f2;{int exps_empty=1;{struct
Cyc_List_List*es=_tmp1AA;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd
!= 0){exps_empty=0;break;}}}{struct _tuple10 _tmp1AB=Cyc_Parse_collapse_type_specifiers(
_tmp1A3,loc);if(_tmp1A9 == 0){void*_tmp1AD;struct Cyc_Core_Opt*_tmp1AE;struct
_tuple10 _tmp1AC=_tmp1AB;_tmp1AD=_tmp1AC.f1;_tmp1AE=_tmp1AC.f2;if(_tmp1AE != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1AE->v;{void*_tmp1AF=d->r;
struct Cyc_Absyn_Enumdecl*_tmp1B1;struct Cyc_Absyn_Aggrdecl*_tmp1B3;struct Cyc_Absyn_Datatypedecl*
_tmp1B5;_LLBC: {struct Cyc_Absyn_Enum_d_struct*_tmp1B0=(struct Cyc_Absyn_Enum_d_struct*)
_tmp1AF;if(_tmp1B0->tag != 8)goto _LLBE;else{_tmp1B1=_tmp1B0->f1;}}_LLBD: _tmp1B1->sc=
s;if(_tmp1A4 != 0){const char*_tmp978;Cyc_Parse_err(((_tmp978="bad attributes on enum",
_tag_dyneither(_tmp978,sizeof(char),23))),loc);}goto _LLBB;_LLBE: {struct Cyc_Absyn_Aggr_d_struct*
_tmp1B2=(struct Cyc_Absyn_Aggr_d_struct*)_tmp1AF;if(_tmp1B2->tag != 6)goto _LLC0;
else{_tmp1B3=_tmp1B2->f1;}}_LLBF: _tmp1B3->sc=s;_tmp1B3->attributes=_tmp1A4;goto
_LLBB;_LLC0: {struct Cyc_Absyn_Datatype_d_struct*_tmp1B4=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp1AF;if(_tmp1B4->tag != 7)goto _LLC2;else{_tmp1B5=_tmp1B4->f1;}}_LLC1: _tmp1B5->sc=
s;if(_tmp1A4 != 0){const char*_tmp979;Cyc_Parse_err(((_tmp979="bad attributes on datatype",
_tag_dyneither(_tmp979,sizeof(char),27))),loc);}goto _LLBB;_LLC2:;_LLC3:{const
char*_tmp97A;Cyc_Parse_err(((_tmp97A="bad declaration",_tag_dyneither(_tmp97A,
sizeof(char),16))),loc);}{struct Cyc_List_List*_tmp1B9=0;_npop_handler(0);return
_tmp1B9;};_LLBB:;}{struct Cyc_List_List*_tmp97B;struct Cyc_List_List*_tmp1BB=(
_tmp97B=_cycalloc(sizeof(*_tmp97B)),((_tmp97B->hd=d,((_tmp97B->tl=0,_tmp97B)))));
_npop_handler(0);return _tmp1BB;};}else{void*_tmp1BC=_tmp1AD;struct Cyc_Absyn_AggrInfo
_tmp1BE;union Cyc_Absyn_AggrInfoU _tmp1BF;struct _tuple3 _tmp1C0;enum Cyc_Absyn_AggrKind
_tmp1C1;struct _tuple1*_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_Absyn_DatatypeInfo
_tmp1C5;union Cyc_Absyn_DatatypeInfoU _tmp1C6;struct Cyc_Absyn_Datatypedecl**
_tmp1C7;struct Cyc_Absyn_DatatypeInfo _tmp1C9;union Cyc_Absyn_DatatypeInfoU _tmp1CA;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp1CB;struct _tuple1*_tmp1CC;int _tmp1CD;
struct Cyc_List_List*_tmp1CE;struct _tuple1*_tmp1D0;struct Cyc_List_List*_tmp1D2;
_LLC5: {struct Cyc_Absyn_AggrType_struct*_tmp1BD=(struct Cyc_Absyn_AggrType_struct*)
_tmp1BC;if(_tmp1BD->tag != 12)goto _LLC7;else{_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BE.aggr_info;
if((_tmp1BF.UnknownAggr).tag != 1)goto _LLC7;_tmp1C0=(struct _tuple3)(_tmp1BF.UnknownAggr).val;
_tmp1C1=_tmp1C0.f1;_tmp1C2=_tmp1C0.f2;_tmp1C3=_tmp1BE.targs;}}_LLC6: {struct Cyc_List_List*
_tmp1D3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp1C3);struct Cyc_Absyn_Aggrdecl*_tmp97C;struct Cyc_Absyn_Aggrdecl*_tmp1D4=(
_tmp97C=_cycalloc(sizeof(*_tmp97C)),((_tmp97C->kind=_tmp1C1,((_tmp97C->sc=s,((
_tmp97C->name=_tmp1C2,((_tmp97C->tvs=_tmp1D3,((_tmp97C->impl=0,((_tmp97C->attributes=
0,_tmp97C)))))))))))));if(_tmp1A4 != 0){const char*_tmp97D;Cyc_Parse_err(((_tmp97D="bad attributes on type declaration",
_tag_dyneither(_tmp97D,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp983;struct Cyc_Absyn_Aggr_d_struct _tmp982;struct Cyc_List_List*_tmp981;struct
Cyc_List_List*_tmp1D9=(_tmp981=_cycalloc(sizeof(*_tmp981)),((_tmp981->hd=Cyc_Absyn_new_decl((
void*)((_tmp983=_cycalloc(sizeof(*_tmp983)),((_tmp983[0]=((_tmp982.tag=6,((
_tmp982.f1=_tmp1D4,_tmp982)))),_tmp983)))),loc),((_tmp981->tl=0,_tmp981)))));
_npop_handler(0);return _tmp1D9;};}_LLC7: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1C4=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1BC;if(_tmp1C4->tag != 3)goto
_LLC9;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C5.datatype_info;if((_tmp1C6.KnownDatatype).tag
!= 2)goto _LLC9;_tmp1C7=(struct Cyc_Absyn_Datatypedecl**)(_tmp1C6.KnownDatatype).val;}}
_LLC8: if(_tmp1A4 != 0){const char*_tmp984;Cyc_Parse_err(((_tmp984="bad attributes on datatype",
_tag_dyneither(_tmp984,sizeof(char),27))),loc);}{struct Cyc_Absyn_Datatype_d_struct*
_tmp98A;struct Cyc_Absyn_Datatype_d_struct _tmp989;struct Cyc_List_List*_tmp988;
struct Cyc_List_List*_tmp1DF=(_tmp988=_cycalloc(sizeof(*_tmp988)),((_tmp988->hd=
Cyc_Absyn_new_decl((void*)((_tmp98A=_cycalloc(sizeof(*_tmp98A)),((_tmp98A[0]=((
_tmp989.tag=7,((_tmp989.f1=*_tmp1C7,_tmp989)))),_tmp98A)))),loc),((_tmp988->tl=0,
_tmp988)))));_npop_handler(0);return _tmp1DF;};_LLC9: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1C8=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1BC;if(_tmp1C8->tag != 3)goto
_LLCB;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=_tmp1C9.datatype_info;if((_tmp1CA.UnknownDatatype).tag
!= 1)goto _LLCB;_tmp1CB=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1CA.UnknownDatatype).val;
_tmp1CC=_tmp1CB.name;_tmp1CD=_tmp1CB.is_extensible;_tmp1CE=_tmp1C9.targs;}}_LLCA: {
struct Cyc_List_List*_tmp1E0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp1CE);struct Cyc_Absyn_Decl*_tmp1E1=
Cyc_Absyn_datatype_decl(s,_tmp1CC,_tmp1E0,0,_tmp1CD,loc);if(_tmp1A4 != 0){const
char*_tmp98B;Cyc_Parse_err(((_tmp98B="bad attributes on datatype",_tag_dyneither(
_tmp98B,sizeof(char),27))),loc);}{struct Cyc_List_List*_tmp98C;struct Cyc_List_List*
_tmp1E4=(_tmp98C=_cycalloc(sizeof(*_tmp98C)),((_tmp98C->hd=_tmp1E1,((_tmp98C->tl=
0,_tmp98C)))));_npop_handler(0);return _tmp1E4;};}_LLCB: {struct Cyc_Absyn_EnumType_struct*
_tmp1CF=(struct Cyc_Absyn_EnumType_struct*)_tmp1BC;if(_tmp1CF->tag != 14)goto _LLCD;
else{_tmp1D0=_tmp1CF->f1;}}_LLCC: {struct Cyc_Absyn_Enumdecl*_tmp98D;struct Cyc_Absyn_Enumdecl*
_tmp1E5=(_tmp98D=_cycalloc(sizeof(*_tmp98D)),((_tmp98D->sc=s,((_tmp98D->name=
_tmp1D0,((_tmp98D->fields=0,_tmp98D)))))));if(_tmp1A4 != 0){const char*_tmp98E;Cyc_Parse_err(((
_tmp98E="bad attributes on enum",_tag_dyneither(_tmp98E,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp998;struct Cyc_Absyn_Enum_d_struct _tmp997;struct Cyc_Absyn_Enum_d_struct*
_tmp996;struct Cyc_List_List*_tmp995;struct Cyc_List_List*_tmp1EB=(_tmp995=
_cycalloc(sizeof(*_tmp995)),((_tmp995->hd=((_tmp998=_cycalloc(sizeof(*_tmp998)),((
_tmp998->r=(void*)((_tmp996=_cycalloc(sizeof(*_tmp996)),((_tmp996[0]=((_tmp997.tag=
8,((_tmp997.f1=_tmp1E5,_tmp997)))),_tmp996)))),((_tmp998->loc=loc,_tmp998)))))),((
_tmp995->tl=0,_tmp995)))));_npop_handler(0);return _tmp1EB;};}_LLCD: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1D1=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp1BC;if(_tmp1D1->tag != 15)goto
_LLCF;else{_tmp1D2=_tmp1D1->f1;}}_LLCE: {struct Cyc_Core_Opt*_tmp99B;struct Cyc_Absyn_Enumdecl*
_tmp99A;struct Cyc_Absyn_Enumdecl*_tmp1ED=(_tmp99A=_cycalloc(sizeof(*_tmp99A)),((
_tmp99A->sc=s,((_tmp99A->name=Cyc_Parse_gensym_enum(),((_tmp99A->fields=((
_tmp99B=_cycalloc(sizeof(*_tmp99B)),((_tmp99B->v=_tmp1D2,_tmp99B)))),_tmp99A)))))));
if(_tmp1A4 != 0){const char*_tmp99C;Cyc_Parse_err(((_tmp99C="bad attributes on enum",
_tag_dyneither(_tmp99C,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp9A6;
struct Cyc_Absyn_Enum_d_struct _tmp9A5;struct Cyc_Absyn_Enum_d_struct*_tmp9A4;
struct Cyc_List_List*_tmp9A3;struct Cyc_List_List*_tmp1F3=(_tmp9A3=_cycalloc(
sizeof(*_tmp9A3)),((_tmp9A3->hd=((_tmp9A6=_cycalloc(sizeof(*_tmp9A6)),((_tmp9A6->r=(
void*)((_tmp9A4=_cycalloc(sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=8,((
_tmp9A5.f1=_tmp1ED,_tmp9A5)))),_tmp9A4)))),((_tmp9A6->loc=loc,_tmp9A6)))))),((
_tmp9A3->tl=0,_tmp9A3)))));_npop_handler(0);return _tmp1F3;};}_LLCF:;_LLD0:{const
char*_tmp9A7;Cyc_Parse_err(((_tmp9A7="missing declarator",_tag_dyneither(_tmp9A7,
sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1F7=0;_npop_handler(0);return
_tmp1F7;};_LLC4:;}}else{void*t=_tmp1AB.f1;struct Cyc_List_List*_tmp1F8=Cyc_Parse_apply_tmss(
mkrgn,_tmp1A2,t,_tmp1A9,_tmp1A4);if(istypedef){if(!exps_empty){const char*_tmp9A8;
Cyc_Parse_err(((_tmp9A8="initializer in typedef declaration",_tag_dyneither(
_tmp9A8,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple13*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp1F8);if(_tmp1AB.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp1AB.f2)->v;{void*_tmp1FA=d->r;struct Cyc_Absyn_Aggrdecl*_tmp1FC;struct Cyc_Absyn_Datatypedecl*
_tmp1FE;struct Cyc_Absyn_Enumdecl*_tmp200;_LLD2: {struct Cyc_Absyn_Aggr_d_struct*
_tmp1FB=(struct Cyc_Absyn_Aggr_d_struct*)_tmp1FA;if(_tmp1FB->tag != 6)goto _LLD4;
else{_tmp1FC=_tmp1FB->f1;}}_LLD3: _tmp1FC->sc=s;_tmp1FC->attributes=_tmp1A4;
_tmp1A4=0;goto _LLD1;_LLD4: {struct Cyc_Absyn_Datatype_d_struct*_tmp1FD=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp1FA;if(_tmp1FD->tag != 7)goto _LLD6;else{_tmp1FE=_tmp1FD->f1;}}_LLD5: _tmp1FE->sc=
s;goto _LLD1;_LLD6: {struct Cyc_Absyn_Enum_d_struct*_tmp1FF=(struct Cyc_Absyn_Enum_d_struct*)
_tmp1FA;if(_tmp1FF->tag != 8)goto _LLD8;else{_tmp200=_tmp1FF->f1;}}_LLD7: _tmp200->sc=
s;goto _LLD1;_LLD8:;_LLD9:{const char*_tmp9A9;Cyc_Parse_err(((_tmp9A9="declaration within typedef is not a struct, union, or datatype",
_tag_dyneither(_tmp9A9,sizeof(char),63))),loc);}goto _LLD1;_LLD1:;}{struct Cyc_List_List*
_tmp9AA;decls=((_tmp9AA=_cycalloc(sizeof(*_tmp9AA)),((_tmp9AA->hd=d,((_tmp9AA->tl=
decls,_tmp9AA))))));};}{struct Cyc_List_List*_tmp203=decls;_npop_handler(0);
return _tmp203;};};}else{if(_tmp1AB.f2 != 0){const char*_tmp9AB;Cyc_Parse_unimp(((
_tmp9AB="nested type declaration within declarator",_tag_dyneither(_tmp9AB,
sizeof(char),42))),loc);}{struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp205=_tmp1F8;for(0;_tmp205 != 0;(_tmp205=_tmp205->tl,_tmp1AA=_tmp1AA->tl)){
struct _tuple13 _tmp207;struct _tuple1*_tmp208;struct Cyc_Absyn_Tqual _tmp209;void*
_tmp20A;struct Cyc_List_List*_tmp20B;struct Cyc_List_List*_tmp20C;struct _tuple13*
_tmp206=(struct _tuple13*)_tmp205->hd;_tmp207=*_tmp206;_tmp208=_tmp207.f1;_tmp209=
_tmp207.f2;_tmp20A=_tmp207.f3;_tmp20B=_tmp207.f4;_tmp20C=_tmp207.f5;if(_tmp20B != 
0){const char*_tmp9AE;void*_tmp9AD;(_tmp9AD=0,Cyc_Tcutil_warn(loc,((_tmp9AE="bad type params, ignoring",
_tag_dyneither(_tmp9AE,sizeof(char),26))),_tag_dyneither(_tmp9AD,sizeof(void*),0)));}
if(_tmp1AA == 0){const char*_tmp9B1;void*_tmp9B0;(_tmp9B0=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp9B1="unexpected NULL in parse!",_tag_dyneither(_tmp9B1,sizeof(char),26))),
_tag_dyneither(_tmp9B0,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*_tmp211=(struct
Cyc_Absyn_Exp*)_tmp1AA->hd;struct Cyc_Absyn_Vardecl*_tmp212=Cyc_Absyn_new_vardecl(
_tmp208,_tmp20A,_tmp211);_tmp212->tq=_tmp209;_tmp212->sc=s;_tmp212->attributes=
_tmp20C;{struct Cyc_Absyn_Var_d_struct*_tmp9B7;struct Cyc_Absyn_Var_d_struct
_tmp9B6;struct Cyc_Absyn_Decl*_tmp9B5;struct Cyc_Absyn_Decl*_tmp213=(_tmp9B5=
_cycalloc(sizeof(*_tmp9B5)),((_tmp9B5->r=(void*)((_tmp9B7=_cycalloc(sizeof(*
_tmp9B7)),((_tmp9B7[0]=((_tmp9B6.tag=0,((_tmp9B6.f1=_tmp212,_tmp9B6)))),_tmp9B7)))),((
_tmp9B5->loc=loc,_tmp9B5)))));struct Cyc_List_List*_tmp9B8;decls=((_tmp9B8=
_cycalloc(sizeof(*_tmp9B8)),((_tmp9B8->hd=_tmp213,((_tmp9B8->tl=decls,_tmp9B8))))));};};}}{
struct Cyc_List_List*_tmp218=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
decls);_npop_handler(0);return _tmp218;};};}}};};};};};_pop_region(mkrgn);}static
struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,struct Cyc_Position_Segment*
loc);static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,
struct Cyc_Position_Segment*loc){if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || 
Cyc_strlen((struct _dyneither_ptr)s)== 2)switch(*((const char*)
_check_dyneither_subscript(s,sizeof(char),0))){case 'A': _LLDA: return& Cyc_Tcutil_ak;
case 'M': _LLDB: return& Cyc_Tcutil_mk;case 'B': _LLDC: return& Cyc_Tcutil_bk;case 'R':
_LLDD: return& Cyc_Tcutil_rk;case 'E': _LLDE: return& Cyc_Tcutil_ek;case 'I': _LLDF:
return& Cyc_Tcutil_ik;case 'U': _LLE0: switch(*((const char*)
_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLE2: return& Cyc_Tcutil_urk;
case 'A': _LLE3: return& Cyc_Tcutil_uak;case 'M': _LLE4: return& Cyc_Tcutil_umk;case 'B':
_LLE5: return& Cyc_Tcutil_ubk;default: _LLE6: break;}break;case 'T': _LLE1: switch(*((
const char*)_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLE9: return&
Cyc_Tcutil_trk;case 'A': _LLEA: return& Cyc_Tcutil_tak;case 'M': _LLEB: return& Cyc_Tcutil_tmk;
case 'B': _LLEC: return& Cyc_Tcutil_tbk;default: _LLED: break;}break;default: _LLE8:
break;}{const char*_tmp9BD;void*_tmp9BC[2];struct Cyc_String_pa_struct _tmp9BB;
struct Cyc_Int_pa_struct _tmp9BA;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9BA.tag=
1,((_tmp9BA.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((
_tmp9BB.tag=0,((_tmp9BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp9BC[0]=& _tmp9BB,((_tmp9BC[1]=& _tmp9BA,Cyc_aprintf(((_tmp9BD="bad kind: %s; strlen=%d",
_tag_dyneither(_tmp9BD,sizeof(char),24))),_tag_dyneither(_tmp9BC,sizeof(void*),2)))))))))))))),
loc);}return& Cyc_Tcutil_bk;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct
Cyc_Absyn_Pat*p);static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*
p){void*_tmp21D=p->r;struct _tuple1*_tmp21F;struct Cyc_Absyn_Vardecl*_tmp221;
struct Cyc_Absyn_Pat*_tmp222;struct Cyc_Absyn_Pat _tmp223;void*_tmp224;struct Cyc_Absyn_Pat*
_tmp227;enum Cyc_Absyn_Sign _tmp22A;int _tmp22B;char _tmp22D;struct _dyneither_ptr
_tmp22F;struct _tuple1*_tmp231;struct Cyc_List_List*_tmp232;int _tmp233;struct Cyc_Absyn_Exp*
_tmp235;_LLF1: {struct Cyc_Absyn_UnknownId_p_struct*_tmp21E=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp21D;if(_tmp21E->tag != 14)goto _LLF3;else{_tmp21F=_tmp21E->f1;}}_LLF2: {struct
Cyc_Absyn_UnknownId_e_struct _tmp9C0;struct Cyc_Absyn_UnknownId_e_struct*_tmp9BF;
return Cyc_Absyn_new_exp((void*)((_tmp9BF=_cycalloc(sizeof(*_tmp9BF)),((_tmp9BF[0]=((
_tmp9C0.tag=2,((_tmp9C0.f1=_tmp21F,_tmp9C0)))),_tmp9BF)))),p->loc);}_LLF3: {
struct Cyc_Absyn_Reference_p_struct*_tmp220=(struct Cyc_Absyn_Reference_p_struct*)
_tmp21D;if(_tmp220->tag != 2)goto _LLF5;else{_tmp221=_tmp220->f1;_tmp222=_tmp220->f2;
_tmp223=*_tmp222;_tmp224=_tmp223.r;{struct Cyc_Absyn_Wild_p_struct*_tmp225=(
struct Cyc_Absyn_Wild_p_struct*)_tmp224;if(_tmp225->tag != 0)goto _LLF5;};}}_LLF4: {
struct Cyc_Absyn_UnknownId_e_struct _tmp9C3;struct Cyc_Absyn_UnknownId_e_struct*
_tmp9C2;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmp9C2=_cycalloc(
sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C3.tag=2,((_tmp9C3.f1=_tmp221->name,
_tmp9C3)))),_tmp9C2)))),p->loc),p->loc);}_LLF5: {struct Cyc_Absyn_Pointer_p_struct*
_tmp226=(struct Cyc_Absyn_Pointer_p_struct*)_tmp21D;if(_tmp226->tag != 5)goto _LLF7;
else{_tmp227=_tmp226->f1;}}_LLF6: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(
_tmp227),p->loc);_LLF7: {struct Cyc_Absyn_Null_p_struct*_tmp228=(struct Cyc_Absyn_Null_p_struct*)
_tmp21D;if(_tmp228->tag != 8)goto _LLF9;}_LLF8: return Cyc_Absyn_null_exp(p->loc);
_LLF9: {struct Cyc_Absyn_Int_p_struct*_tmp229=(struct Cyc_Absyn_Int_p_struct*)
_tmp21D;if(_tmp229->tag != 9)goto _LLFB;else{_tmp22A=_tmp229->f1;_tmp22B=_tmp229->f2;}}
_LLFA: return Cyc_Absyn_int_exp(_tmp22A,_tmp22B,p->loc);_LLFB: {struct Cyc_Absyn_Char_p_struct*
_tmp22C=(struct Cyc_Absyn_Char_p_struct*)_tmp21D;if(_tmp22C->tag != 10)goto _LLFD;
else{_tmp22D=_tmp22C->f1;}}_LLFC: return Cyc_Absyn_char_exp(_tmp22D,p->loc);_LLFD: {
struct Cyc_Absyn_Float_p_struct*_tmp22E=(struct Cyc_Absyn_Float_p_struct*)_tmp21D;
if(_tmp22E->tag != 11)goto _LLFF;else{_tmp22F=_tmp22E->f1;}}_LLFE: return Cyc_Absyn_float_exp(
_tmp22F,p->loc);_LLFF: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp230=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp21D;if(_tmp230->tag != 15)goto _LL101;else{_tmp231=_tmp230->f1;_tmp232=_tmp230->f2;
_tmp233=_tmp230->f3;if(_tmp233 != 0)goto _LL101;}}_LL100: {struct Cyc_Absyn_UnknownId_e_struct
_tmp9C6;struct Cyc_Absyn_UnknownId_e_struct*_tmp9C5;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmp9C5=_cycalloc(sizeof(*_tmp9C5)),((_tmp9C5[0]=((_tmp9C6.tag=2,((
_tmp9C6.f1=_tmp231,_tmp9C6)))),_tmp9C5)))),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp232);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LL101: {struct Cyc_Absyn_Exp_p_struct*_tmp234=(struct Cyc_Absyn_Exp_p_struct*)
_tmp21D;if(_tmp234->tag != 16)goto _LL103;else{_tmp235=_tmp234->f1;}}_LL102: return
_tmp235;_LL103:;_LL104:{const char*_tmp9C7;Cyc_Parse_err(((_tmp9C7="cannot mix patterns and expressions in case",
_tag_dyneither(_tmp9C7,sizeof(char),44))),p->loc);}return Cyc_Absyn_null_exp(p->loc);
_LLF0:;}union Cyc_YYSTYPE;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple6 val;};
struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{
int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;
struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*
val;};struct _tuple18{struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;
union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple18*
val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple19{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple19*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple20*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Parse_Declaration_spec*val;};
struct _tuple21{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Parse_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Parse_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator*val;};
struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum 
Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*
val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple22{
struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple23{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple23*val;};
struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{
int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};
struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct
_union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct
_union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{
int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};
struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct
_union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct
_union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok
Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_YY1
YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct
_union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;
struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9
YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct
_union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14
YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct
_union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19
YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct
_union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24
YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct
_union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29
YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct
_union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34
YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct
_union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39
YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct
_union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44
YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct
_union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49
YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct
_union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54
YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp23E[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp23E,
_tmp23E,_tmp23E + 14}};struct _tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1);struct
_tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){struct _tuple6 yyzzz;{union Cyc_YYSTYPE
_tmp23F=yy1;struct _tuple6 _tmp240;_LL106: if((_tmp23F.Int_tok).tag != 1)goto _LL108;
_tmp240=(struct _tuple6)(_tmp23F.Int_tok).val;_LL107: yyzzz=_tmp240;goto _LL105;
_LL108:;_LL109:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LL105:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1);union Cyc_YYSTYPE Cyc_Int_tok(struct
_tuple6 yy1){union Cyc_YYSTYPE _tmp9C8;return((_tmp9C8.Int_tok).val=yy1,(((_tmp9C8.Int_tok).tag=
1,_tmp9C8)));}static char _tmp243[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE
yy1);char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){char yyzzz;{union Cyc_YYSTYPE
_tmp244=yy1;char _tmp245;_LL10B: if((_tmp244.Char_tok).tag != 2)goto _LL10D;_tmp245=(
char)(_tmp244.Char_tok).val;_LL10C: yyzzz=_tmp245;goto _LL10A;_LL10D:;_LL10E:(int)
_throw((void*)& Cyc_yyfail_Char_tok);_LL10A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_Char_tok(
char yy1);union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9C9;return((
_tmp9C9.Char_tok).val=yy1,(((_tmp9C9.Char_tok).tag=2,_tmp9C9)));}static char
_tmp248[13]="string_t<`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp248,_tmp248,_tmp248 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(
union Cyc_YYSTYPE yy1);struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp249=yy1;struct _dyneither_ptr
_tmp24A;_LL110: if((_tmp249.String_tok).tag != 3)goto _LL112;_tmp24A=(struct
_dyneither_ptr)(_tmp249.String_tok).val;_LL111: yyzzz=_tmp24A;goto _LL10F;_LL112:;
_LL113:(int)_throw((void*)& Cyc_yyfail_String_tok);_LL10F:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1);union Cyc_YYSTYPE Cyc_String_tok(struct
_dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9CA;return((_tmp9CA.String_tok).val=yy1,(((
_tmp9CA.String_tok).tag=3,_tmp9CA)));}static char _tmp24D[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp24D,
_tmp24D,_tmp24D + 56}};struct _tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1);struct
_tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct _tuple18*yyzzz;{union Cyc_YYSTYPE
_tmp24E=yy1;struct _tuple18*_tmp24F;_LL115: if((_tmp24E.YY1).tag != 6)goto _LL117;
_tmp24F=(struct _tuple18*)(_tmp24E.YY1).val;_LL116: yyzzz=_tmp24F;goto _LL114;
_LL117:;_LL118:(int)_throw((void*)& Cyc_yyfail_YY1);_LL114:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY1(struct _tuple18*yy1);union Cyc_YYSTYPE Cyc_YY1(struct _tuple18*
yy1){union Cyc_YYSTYPE _tmp9CB;return((_tmp9CB.YY1).val=yy1,(((_tmp9CB.YY1).tag=6,
_tmp9CB)));}static char _tmp252[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp252,_tmp252,_tmp252 + 19}};union Cyc_Absyn_Constraint*
Cyc_yyget_YY2(union Cyc_YYSTYPE yy1);union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union
Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp253=yy1;
union Cyc_Absyn_Constraint*_tmp254;_LL11A: if((_tmp253.YY2).tag != 7)goto _LL11C;
_tmp254=(union Cyc_Absyn_Constraint*)(_tmp253.YY2).val;_LL11B: yyzzz=_tmp254;goto
_LL119;_LL11C:;_LL11D:(int)_throw((void*)& Cyc_yyfail_YY2);_LL119:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1);union Cyc_YYSTYPE Cyc_YY2(
union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9CC;return((_tmp9CC.YY2).val=
yy1,(((_tmp9CC.YY2).tag=7,_tmp9CC)));}static char _tmp257[6]="exp_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp257,_tmp257,_tmp257
+ 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Exp*
Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE
_tmp258=yy1;struct Cyc_Absyn_Exp*_tmp259;_LL11F: if((_tmp258.YY3).tag != 8)goto
_LL121;_tmp259=(struct Cyc_Absyn_Exp*)(_tmp258.YY3).val;_LL120: yyzzz=_tmp259;goto
_LL11E;_LL121:;_LL122:(int)_throw((void*)& Cyc_yyfail_YY3);_LL11E:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_Absyn_Exp*yy1);union Cyc_YYSTYPE Cyc_YY3(struct
Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9CD;return((_tmp9CD.YY3).val=yy1,(((
_tmp9CD.YY3).tag=8,_tmp9CD)));}static char _tmp25C[10]="exp_opt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp25C,_tmp25C,_tmp25C + 10}};struct Cyc_Absyn_Exp*
Cyc_yyget_YY4(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE _tmp25D=yy1;struct Cyc_Absyn_Exp*
_tmp25E;_LL124: if((_tmp25D.YY4).tag != 9)goto _LL126;_tmp25E=(struct Cyc_Absyn_Exp*)(
_tmp25D.YY4).val;_LL125: yyzzz=_tmp25E;goto _LL123;_LL126:;_LL127:(int)_throw((
void*)& Cyc_yyfail_YY4);_LL123:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*
yy1);union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9CE;
return((_tmp9CE.YY4).val=yy1,(((_tmp9CE.YY4).tag=9,_tmp9CE)));}static char _tmp261[
17]="list_t<exp_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{
_tmp261,_tmp261,_tmp261 + 17}};struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE
yy1);struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp262=yy1;struct Cyc_List_List*_tmp263;_LL129: if((
_tmp262.YY5).tag != 10)goto _LL12B;_tmp263=(struct Cyc_List_List*)(_tmp262.YY5).val;
_LL12A: yyzzz=_tmp263;goto _LL128;_LL12B:;_LL12C:(int)_throw((void*)& Cyc_yyfail_YY5);
_LL128:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CF;return((_tmp9CF.YY5).val=
yy1,(((_tmp9CF.YY5).tag=10,_tmp9CF)));}static char _tmp266[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp266,
_tmp266,_tmp266 + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{
union Cyc_YYSTYPE _tmp267=yy1;struct Cyc_List_List*_tmp268;_LL12E: if((_tmp267.YY6).tag
!= 11)goto _LL130;_tmp268=(struct Cyc_List_List*)(_tmp267.YY6).val;_LL12F: yyzzz=
_tmp268;goto _LL12D;_LL130:;_LL131:(int)_throw((void*)& Cyc_yyfail_YY6);_LL12D:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D0;return((_tmp9D0.YY6).val=
yy1,(((_tmp9D0.YY6).tag=11,_tmp9D0)));}static char _tmp26B[9]="primop_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp26B,_tmp26B,
_tmp26B + 9}};enum Cyc_Absyn_Primop Cyc_yyget_YY7(union Cyc_YYSTYPE yy1);enum Cyc_Absyn_Primop
Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Primop yyzzz;{union Cyc_YYSTYPE
_tmp26C=yy1;enum Cyc_Absyn_Primop _tmp26D;_LL133: if((_tmp26C.YY7).tag != 12)goto
_LL135;_tmp26D=(enum Cyc_Absyn_Primop)(_tmp26C.YY7).val;_LL134: yyzzz=_tmp26D;
goto _LL132;_LL135:;_LL136:(int)_throw((void*)& Cyc_yyfail_YY7);_LL132:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1);union Cyc_YYSTYPE Cyc_YY7(
enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9D1;return((_tmp9D1.YY7).val=yy1,(((
_tmp9D1.YY7).tag=12,_tmp9D1)));}static char _tmp270[19]="opt_t<primop_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp270,
_tmp270,_tmp270 + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1);
struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{
union Cyc_YYSTYPE _tmp271=yy1;struct Cyc_Core_Opt*_tmp272;_LL138: if((_tmp271.YY8).tag
!= 13)goto _LL13A;_tmp272=(struct Cyc_Core_Opt*)(_tmp271.YY8).val;_LL139: yyzzz=
_tmp272;goto _LL137;_LL13A:;_LL13B:(int)_throw((void*)& Cyc_yyfail_YY8);_LL137:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY8(struct Cyc_Core_Opt*yy1);union Cyc_YYSTYPE Cyc_YY8(
struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9D2;return((_tmp9D2.YY8).val=yy1,(((
_tmp9D2.YY8).tag=13,_tmp9D2)));}static char _tmp275[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp275,_tmp275,_tmp275 + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1);struct _tuple1*Cyc_yyget_QualId_tok(
union Cyc_YYSTYPE yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp276=yy1;struct
_tuple1*_tmp277;_LL13D: if((_tmp276.QualId_tok).tag != 5)goto _LL13F;_tmp277=(
struct _tuple1*)(_tmp276.QualId_tok).val;_LL13E: yyzzz=_tmp277;goto _LL13C;_LL13F:;
_LL140:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL13C:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_QualId_tok(struct _tuple1*yy1);union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple1*
yy1){union Cyc_YYSTYPE _tmp9D3;return((_tmp9D3.QualId_tok).val=yy1,(((_tmp9D3.QualId_tok).tag=
5,_tmp9D3)));}static char _tmp27A[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp27A,_tmp27A,_tmp27A + 7}};struct Cyc_Absyn_Stmt*
Cyc_yyget_YY9(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE _tmp27B=yy1;struct Cyc_Absyn_Stmt*
_tmp27C;_LL142: if((_tmp27B.YY9).tag != 14)goto _LL144;_tmp27C=(struct Cyc_Absyn_Stmt*)(
_tmp27B.YY9).val;_LL143: yyzzz=_tmp27C;goto _LL141;_LL144:;_LL145:(int)_throw((
void*)& Cyc_yyfail_YY9);_LL141:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*
yy1);union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9D4;
return((_tmp9D4.YY9).val=yy1,(((_tmp9D4.YY9).tag=14,_tmp9D4)));}static char
_tmp27F[27]="list_t<switch_clause_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={
Cyc_Core_Failure,{_tmp27F,_tmp27F,_tmp27F + 27}};struct Cyc_List_List*Cyc_yyget_YY10(
union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp280=yy1;struct Cyc_List_List*
_tmp281;_LL147: if((_tmp280.YY10).tag != 15)goto _LL149;_tmp281=(struct Cyc_List_List*)(
_tmp280.YY10).val;_LL148: yyzzz=_tmp281;goto _LL146;_LL149:;_LL14A:(int)_throw((
void*)& Cyc_yyfail_YY10);_LL146:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D5;
return((_tmp9D5.YY10).val=yy1,(((_tmp9D5.YY10).tag=15,_tmp9D5)));}static char
_tmp284[6]="pat_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{
_tmp284,_tmp284,_tmp284 + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE
yy1);struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Pat*
yyzzz;{union Cyc_YYSTYPE _tmp285=yy1;struct Cyc_Absyn_Pat*_tmp286;_LL14C: if((
_tmp285.YY11).tag != 16)goto _LL14E;_tmp286=(struct Cyc_Absyn_Pat*)(_tmp285.YY11).val;
_LL14D: yyzzz=_tmp286;goto _LL14B;_LL14E:;_LL14F:(int)_throw((void*)& Cyc_yyfail_YY11);
_LL14B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1);union Cyc_YYSTYPE
Cyc_YY11(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9D6;return((_tmp9D6.YY11).val=
yy1,(((_tmp9D6.YY11).tag=16,_tmp9D6)));}static char _tmp289[28]="$(list_t<pat_t,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp289,
_tmp289,_tmp289 + 28}};struct _tuple19*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1);struct
_tuple19*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){struct _tuple19*yyzzz;{union Cyc_YYSTYPE
_tmp28A=yy1;struct _tuple19*_tmp28B;_LL151: if((_tmp28A.YY12).tag != 17)goto _LL153;
_tmp28B=(struct _tuple19*)(_tmp28A.YY12).val;_LL152: yyzzz=_tmp28B;goto _LL150;
_LL153:;_LL154:(int)_throw((void*)& Cyc_yyfail_YY12);_LL150:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY12(struct _tuple19*yy1);union Cyc_YYSTYPE Cyc_YY12(struct _tuple19*
yy1){union Cyc_YYSTYPE _tmp9D7;return((_tmp9D7.YY12).val=yy1,(((_tmp9D7.YY12).tag=
17,_tmp9D7)));}static char _tmp28E[17]="list_t<pat_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp28E,_tmp28E,_tmp28E + 17}};struct Cyc_List_List*
Cyc_yyget_YY13(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp28F=yy1;struct Cyc_List_List*
_tmp290;_LL156: if((_tmp28F.YY13).tag != 18)goto _LL158;_tmp290=(struct Cyc_List_List*)(
_tmp28F.YY13).val;_LL157: yyzzz=_tmp290;goto _LL155;_LL158:;_LL159:(int)_throw((
void*)& Cyc_yyfail_YY13);_LL155:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D8;
return((_tmp9D8.YY13).val=yy1,(((_tmp9D8.YY13).tag=18,_tmp9D8)));}static char
_tmp293[36]="$(list_t<designator_t,`H>,pat_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp293,_tmp293,_tmp293 + 36}};struct _tuple20*
Cyc_yyget_YY14(union Cyc_YYSTYPE yy1);struct _tuple20*Cyc_yyget_YY14(union Cyc_YYSTYPE
yy1){struct _tuple20*yyzzz;{union Cyc_YYSTYPE _tmp294=yy1;struct _tuple20*_tmp295;
_LL15B: if((_tmp294.YY14).tag != 19)goto _LL15D;_tmp295=(struct _tuple20*)(_tmp294.YY14).val;
_LL15C: yyzzz=_tmp295;goto _LL15A;_LL15D:;_LL15E:(int)_throw((void*)& Cyc_yyfail_YY14);
_LL15A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY14(struct _tuple20*yy1);union Cyc_YYSTYPE
Cyc_YY14(struct _tuple20*yy1){union Cyc_YYSTYPE _tmp9D9;return((_tmp9D9.YY14).val=
yy1,(((_tmp9D9.YY14).tag=19,_tmp9D9)));}static char _tmp298[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp298,
_tmp298,_tmp298 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp299=yy1;struct Cyc_List_List*_tmp29A;_LL160: if((
_tmp299.YY15).tag != 20)goto _LL162;_tmp29A=(struct Cyc_List_List*)(_tmp299.YY15).val;
_LL161: yyzzz=_tmp29A;goto _LL15F;_LL162:;_LL163:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL15F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY15(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DA;return((_tmp9DA.YY15).val=
yy1,(((_tmp9DA.YY15).tag=20,_tmp9DA)));}static char _tmp29D[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp29D,
_tmp29D,_tmp29D + 58}};struct _tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1);struct
_tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct _tuple19*yyzzz;{union Cyc_YYSTYPE
_tmp29E=yy1;struct _tuple19*_tmp29F;_LL165: if((_tmp29E.YY16).tag != 21)goto _LL167;
_tmp29F=(struct _tuple19*)(_tmp29E.YY16).val;_LL166: yyzzz=_tmp29F;goto _LL164;
_LL167:;_LL168:(int)_throw((void*)& Cyc_yyfail_YY16);_LL164:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY16(struct _tuple19*yy1);union Cyc_YYSTYPE Cyc_YY16(struct _tuple19*
yy1){union Cyc_YYSTYPE _tmp9DB;return((_tmp9DB.YY16).val=yy1,(((_tmp9DB.YY16).tag=
21,_tmp9DB)));}static char _tmp2A2[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp2A2,_tmp2A2,_tmp2A2 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_YY17(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(union
Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp2A3=yy1;struct
Cyc_Absyn_Fndecl*_tmp2A4;_LL16A: if((_tmp2A3.YY17).tag != 22)goto _LL16C;_tmp2A4=(
struct Cyc_Absyn_Fndecl*)(_tmp2A3.YY17).val;_LL16B: yyzzz=_tmp2A4;goto _LL169;
_LL16C:;_LL16D:(int)_throw((void*)& Cyc_yyfail_YY17);_LL169:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1);union Cyc_YYSTYPE Cyc_YY17(struct
Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9DC;return((_tmp9DC.YY17).val=yy1,(((
_tmp9DC.YY17).tag=22,_tmp9DC)));}static char _tmp2A7[18]="list_t<decl_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp2A7,
_tmp2A7,_tmp2A7 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2A8=yy1;struct Cyc_List_List*_tmp2A9;_LL16F: if((
_tmp2A8.YY18).tag != 23)goto _LL171;_tmp2A9=(struct Cyc_List_List*)(_tmp2A8.YY18).val;
_LL170: yyzzz=_tmp2A9;goto _LL16E;_LL171:;_LL172:(int)_throw((void*)& Cyc_yyfail_YY18);
_LL16E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY18(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DD;return((_tmp9DD.YY18).val=
yy1,(((_tmp9DD.YY18).tag=23,_tmp9DD)));}static char _tmp2AC[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp2AC,
_tmp2AC,_tmp2AC + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1);struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){struct
Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp2AD=yy1;struct Cyc_Parse_Declaration_spec*
_tmp2AE;_LL174: if((_tmp2AD.YY19).tag != 24)goto _LL176;_tmp2AE=(struct Cyc_Parse_Declaration_spec*)(
_tmp2AD.YY19).val;_LL175: yyzzz=_tmp2AE;goto _LL173;_LL176:;_LL177:(int)_throw((
void*)& Cyc_yyfail_YY19);_LL173:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*
yy1);union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){union Cyc_YYSTYPE
_tmp9DE;return((_tmp9DE.YY19).val=yy1,(((_tmp9DE.YY19).tag=24,_tmp9DE)));}static
char _tmp2B1[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp2B1,_tmp2B1,_tmp2B1 + 29}};struct _tuple21*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1);struct _tuple21*Cyc_yyget_YY20(union Cyc_YYSTYPE
yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE _tmp2B2=yy1;struct _tuple21*_tmp2B3;
_LL179: if((_tmp2B2.YY20).tag != 25)goto _LL17B;_tmp2B3=(struct _tuple21*)(_tmp2B2.YY20).val;
_LL17A: yyzzz=_tmp2B3;goto _LL178;_LL17B:;_LL17C:(int)_throw((void*)& Cyc_yyfail_YY20);
_LL178:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY20(struct _tuple21*yy1);union Cyc_YYSTYPE
Cyc_YY20(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9DF;return((_tmp9DF.YY20).val=
yy1,(((_tmp9DF.YY20).tag=25,_tmp9DF)));}static char _tmp2B6[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp2B6,
_tmp2B6,_tmp2B6 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2B7=yy1;struct Cyc_List_List*_tmp2B8;_LL17E: if((
_tmp2B7.YY21).tag != 26)goto _LL180;_tmp2B8=(struct Cyc_List_List*)(_tmp2B7.YY21).val;
_LL17F: yyzzz=_tmp2B8;goto _LL17D;_LL180:;_LL181:(int)_throw((void*)& Cyc_yyfail_YY21);
_LL17D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY21(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E0;return((_tmp9E0.YY21).val=
yy1,(((_tmp9E0.YY21).tag=26,_tmp9E0)));}static char _tmp2BB[16]="storage_class_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp2BB,
_tmp2BB,_tmp2BB + 16}};enum Cyc_Parse_Storage_class Cyc_yyget_YY22(union Cyc_YYSTYPE
yy1);enum Cyc_Parse_Storage_class Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){enum Cyc_Parse_Storage_class
yyzzz;{union Cyc_YYSTYPE _tmp2BC=yy1;enum Cyc_Parse_Storage_class _tmp2BD;_LL183:
if((_tmp2BC.YY22).tag != 27)goto _LL185;_tmp2BD=(enum Cyc_Parse_Storage_class)(
_tmp2BC.YY22).val;_LL184: yyzzz=_tmp2BD;goto _LL182;_LL185:;_LL186:(int)_throw((
void*)& Cyc_yyfail_YY22);_LL182:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class
yy1);union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class yy1){union Cyc_YYSTYPE
_tmp9E1;return((_tmp9E1.YY22).val=yy1,(((_tmp9E1.YY22).tag=27,_tmp9E1)));}static
char _tmp2C0[17]="type_specifier_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY23={
Cyc_Core_Failure,{_tmp2C0,_tmp2C0,_tmp2C0 + 17}};void*Cyc_yyget_YY23(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp2C1=yy1;void*_tmp2C2;_LL188: if((_tmp2C1.YY23).tag != 28)goto _LL18A;_tmp2C2=(
void*)(_tmp2C1.YY23).val;_LL189: yyzzz=_tmp2C2;goto _LL187;_LL18A:;_LL18B:(int)
_throw((void*)& Cyc_yyfail_YY23);_LL187:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(
void*yy1);union Cyc_YYSTYPE Cyc_YY23(void*yy1){union Cyc_YYSTYPE _tmp9E2;return((
_tmp9E2.YY23).val=yy1,(((_tmp9E2.YY23).tag=28,_tmp9E2)));}static char _tmp2C5[12]="aggr_kind_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp2C5,
_tmp2C5,_tmp2C5 + 12}};enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(union Cyc_YYSTYPE yy1);
enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_AggrKind
yyzzz;{union Cyc_YYSTYPE _tmp2C6=yy1;enum Cyc_Absyn_AggrKind _tmp2C7;_LL18D: if((
_tmp2C6.YY24).tag != 29)goto _LL18F;_tmp2C7=(enum Cyc_Absyn_AggrKind)(_tmp2C6.YY24).val;
_LL18E: yyzzz=_tmp2C7;goto _LL18C;_LL18F:;_LL190:(int)_throw((void*)& Cyc_yyfail_YY24);
_LL18C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1);union
Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9E3;return((
_tmp9E3.YY24).val=yy1,(((_tmp9E3.YY24).tag=29,_tmp9E3)));}static char _tmp2CA[8]="tqual_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp2CA,
_tmp2CA,_tmp2CA + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE yy1);
struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Tqual
yyzzz;{union Cyc_YYSTYPE _tmp2CB=yy1;struct Cyc_Absyn_Tqual _tmp2CC;_LL192: if((
_tmp2CB.YY25).tag != 30)goto _LL194;_tmp2CC=(struct Cyc_Absyn_Tqual)(_tmp2CB.YY25).val;
_LL193: yyzzz=_tmp2CC;goto _LL191;_LL194:;_LL195:(int)_throw((void*)& Cyc_yyfail_YY25);
_LL191:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual yy1);union
Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp9E4;return((
_tmp9E4.YY25).val=yy1,(((_tmp9E4.YY25).tag=30,_tmp9E4)));}static char _tmp2CF[23]="list_t<aggrfield_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp2CF,
_tmp2CF,_tmp2CF + 23}};struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2D0=yy1;struct Cyc_List_List*_tmp2D1;_LL197: if((
_tmp2D0.YY26).tag != 31)goto _LL199;_tmp2D1=(struct Cyc_List_List*)(_tmp2D0.YY26).val;
_LL198: yyzzz=_tmp2D1;goto _LL196;_LL199:;_LL19A:(int)_throw((void*)& Cyc_yyfail_YY26);
_LL196:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E5;return((_tmp9E5.YY26).val=
yy1,(((_tmp9E5.YY26).tag=31,_tmp9E5)));}static char _tmp2D4[34]="list_t<list_t<aggrfield_t,`H>,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2D4,
_tmp2D4,_tmp2D4 + 34}};struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2D5=yy1;struct Cyc_List_List*_tmp2D6;_LL19C: if((
_tmp2D5.YY27).tag != 32)goto _LL19E;_tmp2D6=(struct Cyc_List_List*)(_tmp2D5.YY27).val;
_LL19D: yyzzz=_tmp2D6;goto _LL19B;_LL19E:;_LL19F:(int)_throw((void*)& Cyc_yyfail_YY27);
_LL19B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E6;return((_tmp9E6.YY27).val=
yy1,(((_tmp9E6.YY27).tag=32,_tmp9E6)));}static char _tmp2D9[31]="list_t<type_modifier_t<`H>,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2D9,
_tmp2D9,_tmp2D9 + 31}};struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2DA=yy1;struct Cyc_List_List*_tmp2DB;_LL1A1: if((
_tmp2DA.YY28).tag != 33)goto _LL1A3;_tmp2DB=(struct Cyc_List_List*)(_tmp2DA.YY28).val;
_LL1A2: yyzzz=_tmp2DB;goto _LL1A0;_LL1A3:;_LL1A4:(int)_throw((void*)& Cyc_yyfail_YY28);
_LL1A0:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E7;return((_tmp9E7.YY28).val=
yy1,(((_tmp9E7.YY28).tag=33,_tmp9E7)));}static char _tmp2DE[13]="declarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp2DE,
_tmp2DE,_tmp2DE + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(union Cyc_YYSTYPE
yy1);struct Cyc_Parse_Declarator*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*
yyzzz;{union Cyc_YYSTYPE _tmp2DF=yy1;struct Cyc_Parse_Declarator*_tmp2E0;_LL1A6: if((
_tmp2DF.YY29).tag != 34)goto _LL1A8;_tmp2E0=(struct Cyc_Parse_Declarator*)(_tmp2DF.YY29).val;
_LL1A7: yyzzz=_tmp2E0;goto _LL1A5;_LL1A8:;_LL1A9:(int)_throw((void*)& Cyc_yyfail_YY29);
_LL1A5:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1);
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){union Cyc_YYSTYPE _tmp9E8;
return((_tmp9E8.YY29).val=yy1,(((_tmp9E8.YY29).tag=34,_tmp9E8)));}static char
_tmp2E3[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={
Cyc_Core_Failure,{_tmp2E3,_tmp2E3,_tmp2E3 + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1);struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_YY30(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{union Cyc_YYSTYPE
_tmp2E4=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp2E5;_LL1AB: if((_tmp2E4.YY30).tag
!= 35)goto _LL1AD;_tmp2E5=(struct Cyc_Parse_Abstractdeclarator*)(_tmp2E4.YY30).val;
_LL1AC: yyzzz=_tmp2E5;goto _LL1AA;_LL1AD:;_LL1AE:(int)_throw((void*)& Cyc_yyfail_YY30);
_LL1AA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1);union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*yy1){union Cyc_YYSTYPE
_tmp9E9;return((_tmp9E9.YY30).val=yy1,(((_tmp9E9.YY30).tag=35,_tmp9E9)));}static
char _tmp2E8[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{
_tmp2E8,_tmp2E8,_tmp2E8 + 5}};int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1);int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp2E9=yy1;int _tmp2EA;_LL1B0: if((
_tmp2E9.YY31).tag != 36)goto _LL1B2;_tmp2EA=(int)(_tmp2E9.YY31).val;_LL1B1: yyzzz=
_tmp2EA;goto _LL1AF;_LL1B2:;_LL1B3:(int)_throw((void*)& Cyc_yyfail_YY31);_LL1AF:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1);union Cyc_YYSTYPE Cyc_YY31(int yy1){
union Cyc_YYSTYPE _tmp9EA;return((_tmp9EA.YY31).val=yy1,(((_tmp9EA.YY31).tag=36,
_tmp9EA)));}static char _tmp2ED[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2ED,_tmp2ED,_tmp2ED + 8}};enum Cyc_Absyn_Scope
Cyc_yyget_YY32(union Cyc_YYSTYPE yy1);enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE
yy1){enum Cyc_Absyn_Scope yyzzz;{union Cyc_YYSTYPE _tmp2EE=yy1;enum Cyc_Absyn_Scope
_tmp2EF;_LL1B5: if((_tmp2EE.YY32).tag != 37)goto _LL1B7;_tmp2EF=(enum Cyc_Absyn_Scope)(
_tmp2EE.YY32).val;_LL1B6: yyzzz=_tmp2EF;goto _LL1B4;_LL1B7:;_LL1B8:(int)_throw((
void*)& Cyc_yyfail_YY32);_LL1B4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope
yy1);union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp9EB;
return((_tmp9EB.YY32).val=yy1,(((_tmp9EB.YY32).tag=37,_tmp9EB)));}static char
_tmp2F2[16]="datatypefield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY33={
Cyc_Core_Failure,{_tmp2F2,_tmp2F2,_tmp2F2 + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union Cyc_YYSTYPE
_tmp2F3=yy1;struct Cyc_Absyn_Datatypefield*_tmp2F4;_LL1BA: if((_tmp2F3.YY33).tag != 
38)goto _LL1BC;_tmp2F4=(struct Cyc_Absyn_Datatypefield*)(_tmp2F3.YY33).val;_LL1BB:
yyzzz=_tmp2F4;goto _LL1B9;_LL1BC:;_LL1BD:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL1B9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1);
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE
_tmp9EC;return((_tmp9EC.YY33).val=yy1,(((_tmp9EC.YY33).tag=38,_tmp9EC)));}static
char _tmp2F7[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2F7,_tmp2F7,_tmp2F7 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2F8=yy1;struct Cyc_List_List*
_tmp2F9;_LL1BF: if((_tmp2F8.YY34).tag != 39)goto _LL1C1;_tmp2F9=(struct Cyc_List_List*)(
_tmp2F8.YY34).val;_LL1C0: yyzzz=_tmp2F9;goto _LL1BE;_LL1C1:;_LL1C2:(int)_throw((
void*)& Cyc_yyfail_YY34);_LL1BE:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9ED;
return((_tmp9ED.YY34).val=yy1,(((_tmp9ED.YY34).tag=39,_tmp9ED)));}static char
_tmp2FC[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2FC,_tmp2FC,
_tmp2FC + 55}};struct _tuple22*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1);struct _tuple22*
Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple22*yyzzz;{union Cyc_YYSTYPE
_tmp2FD=yy1;struct _tuple22*_tmp2FE;_LL1C4: if((_tmp2FD.YY35).tag != 40)goto _LL1C6;
_tmp2FE=(struct _tuple22*)(_tmp2FD.YY35).val;_LL1C5: yyzzz=_tmp2FE;goto _LL1C3;
_LL1C6:;_LL1C7:(int)_throw((void*)& Cyc_yyfail_YY35);_LL1C3:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY35(struct _tuple22*yy1);union Cyc_YYSTYPE Cyc_YY35(struct _tuple22*
yy1){union Cyc_YYSTYPE _tmp9EE;return((_tmp9EE.YY35).val=yy1,(((_tmp9EE.YY35).tag=
40,_tmp9EE)));}static char _tmp301[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp301,_tmp301,_tmp301 + 17}};struct Cyc_List_List*
Cyc_yyget_YY36(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp302=yy1;struct Cyc_List_List*
_tmp303;_LL1C9: if((_tmp302.YY36).tag != 41)goto _LL1CB;_tmp303=(struct Cyc_List_List*)(
_tmp302.YY36).val;_LL1CA: yyzzz=_tmp303;goto _LL1C8;_LL1CB:;_LL1CC:(int)_throw((
void*)& Cyc_yyfail_YY36);_LL1C8:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EF;
return((_tmp9EF.YY36).val=yy1,(((_tmp9EF.YY36).tag=41,_tmp9EF)));}static char
_tmp306[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp306,_tmp306,_tmp306 + 37}};struct _tuple8*
Cyc_yyget_YY37(union Cyc_YYSTYPE yy1);struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE
yy1){struct _tuple8*yyzzz;{union Cyc_YYSTYPE _tmp307=yy1;struct _tuple8*_tmp308;
_LL1CE: if((_tmp307.YY37).tag != 42)goto _LL1D0;_tmp308=(struct _tuple8*)(_tmp307.YY37).val;
_LL1CF: yyzzz=_tmp308;goto _LL1CD;_LL1D0:;_LL1D1:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL1CD:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1);union Cyc_YYSTYPE
Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp9F0;return((_tmp9F0.YY37).val=
yy1,(((_tmp9F0.YY37).tag=42,_tmp9F0)));}static char _tmp30B[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp30B,
_tmp30B,_tmp30B + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp30C=yy1;struct Cyc_List_List*_tmp30D;_LL1D3: if((
_tmp30C.YY38).tag != 43)goto _LL1D5;_tmp30D=(struct Cyc_List_List*)(_tmp30C.YY38).val;
_LL1D4: yyzzz=_tmp30D;goto _LL1D2;_LL1D5:;_LL1D6:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1D2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F1;return((_tmp9F1.YY38).val=
yy1,(((_tmp9F1.YY38).tag=43,_tmp9F1)));}static char _tmp310[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp310,
_tmp310,_tmp310 + 127}};struct _tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1);struct
_tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct _tuple23*yyzzz;{union Cyc_YYSTYPE
_tmp311=yy1;struct _tuple23*_tmp312;_LL1D8: if((_tmp311.YY39).tag != 44)goto _LL1DA;
_tmp312=(struct _tuple23*)(_tmp311.YY39).val;_LL1D9: yyzzz=_tmp312;goto _LL1D7;
_LL1DA:;_LL1DB:(int)_throw((void*)& Cyc_yyfail_YY39);_LL1D7:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY39(struct _tuple23*yy1);union Cyc_YYSTYPE Cyc_YY39(struct _tuple23*
yy1){union Cyc_YYSTYPE _tmp9F2;return((_tmp9F2.YY39).val=yy1,(((_tmp9F2.YY39).tag=
44,_tmp9F2)));}static char _tmp315[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp315,_tmp315,_tmp315 + 18}};struct Cyc_List_List*
Cyc_yyget_YY40(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp316=yy1;struct Cyc_List_List*
_tmp317;_LL1DD: if((_tmp316.YY40).tag != 45)goto _LL1DF;_tmp317=(struct Cyc_List_List*)(
_tmp316.YY40).val;_LL1DE: yyzzz=_tmp317;goto _LL1DC;_LL1DF:;_LL1E0:(int)_throw((
void*)& Cyc_yyfail_YY40);_LL1DC:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F3;
return((_tmp9F3.YY40).val=yy1,(((_tmp9F3.YY40).tag=45,_tmp9F3)));}static char
_tmp31A[24]="list_t<designator_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={
Cyc_Core_Failure,{_tmp31A,_tmp31A,_tmp31A + 24}};struct Cyc_List_List*Cyc_yyget_YY41(
union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp31B=yy1;struct Cyc_List_List*
_tmp31C;_LL1E2: if((_tmp31B.YY41).tag != 46)goto _LL1E4;_tmp31C=(struct Cyc_List_List*)(
_tmp31B.YY41).val;_LL1E3: yyzzz=_tmp31C;goto _LL1E1;_LL1E4:;_LL1E5:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1E1:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F4;
return((_tmp9F4.YY41).val=yy1,(((_tmp9F4.YY41).tag=46,_tmp9F4)));}static char
_tmp31F[13]="designator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY42={
Cyc_Core_Failure,{_tmp31F,_tmp31F,_tmp31F + 13}};void*Cyc_yyget_YY42(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp320=yy1;void*_tmp321;_LL1E7: if((_tmp320.YY42).tag != 47)goto _LL1E9;_tmp321=(
void*)(_tmp320.YY42).val;_LL1E8: yyzzz=_tmp321;goto _LL1E6;_LL1E9:;_LL1EA:(int)
_throw((void*)& Cyc_yyfail_YY42);_LL1E6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(
void*yy1);union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp9F5;return((
_tmp9F5.YY42).val=yy1,(((_tmp9F5.YY42).tag=47,_tmp9F5)));}static char _tmp324[7]="kind_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp324,
_tmp324,_tmp324 + 7}};struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1);
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Kind*
yyzzz;{union Cyc_YYSTYPE _tmp325=yy1;struct Cyc_Absyn_Kind*_tmp326;_LL1EC: if((
_tmp325.YY43).tag != 48)goto _LL1EE;_tmp326=(struct Cyc_Absyn_Kind*)(_tmp325.YY43).val;
_LL1ED: yyzzz=_tmp326;goto _LL1EB;_LL1EE:;_LL1EF:(int)_throw((void*)& Cyc_yyfail_YY43);
_LL1EB:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1);union
Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp9F6;return((
_tmp9F6.YY43).val=yy1,(((_tmp9F6.YY43).tag=48,_tmp9F6)));}static char _tmp329[7]="type_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp329,
_tmp329,_tmp329 + 7}};void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1);void*Cyc_yyget_YY44(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp32A=yy1;void*_tmp32B;_LL1F1:
if((_tmp32A.YY44).tag != 49)goto _LL1F3;_tmp32B=(void*)(_tmp32A.YY44).val;_LL1F2:
yyzzz=_tmp32B;goto _LL1F0;_LL1F3:;_LL1F4:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1F0:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1);union Cyc_YYSTYPE Cyc_YY44(
void*yy1){union Cyc_YYSTYPE _tmp9F7;return((_tmp9F7.YY44).val=yy1,(((_tmp9F7.YY44).tag=
49,_tmp9F7)));}static char _tmp32E[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp32E,_tmp32E,_tmp32E + 23}};struct Cyc_List_List*
Cyc_yyget_YY45(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp32F=yy1;struct Cyc_List_List*
_tmp330;_LL1F6: if((_tmp32F.YY45).tag != 50)goto _LL1F8;_tmp330=(struct Cyc_List_List*)(
_tmp32F.YY45).val;_LL1F7: yyzzz=_tmp330;goto _LL1F5;_LL1F8:;_LL1F9:(int)_throw((
void*)& Cyc_yyfail_YY45);_LL1F5:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F8;
return((_tmp9F8.YY45).val=yy1,(((_tmp9F8.YY45).tag=50,_tmp9F8)));}static char
_tmp333[12]="attribute_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={
Cyc_Core_Failure,{_tmp333,_tmp333,_tmp333 + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp334=yy1;void*_tmp335;_LL1FB: if((_tmp334.YY46).tag != 51)goto _LL1FD;_tmp335=(
void*)(_tmp334.YY46).val;_LL1FC: yyzzz=_tmp335;goto _LL1FA;_LL1FD:;_LL1FE:(int)
_throw((void*)& Cyc_yyfail_YY46);_LL1FA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1);union Cyc_YYSTYPE Cyc_YY46(void*yy1){union Cyc_YYSTYPE _tmp9F9;return((
_tmp9F9.YY46).val=yy1,(((_tmp9F9.YY46).tag=51,_tmp9F9)));}static char _tmp338[12]="enumfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp338,
_tmp338,_tmp338 + 12}};struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE
yy1);struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{union Cyc_YYSTYPE _tmp339=yy1;struct Cyc_Absyn_Enumfield*_tmp33A;_LL200: if((
_tmp339.YY47).tag != 52)goto _LL202;_tmp33A=(struct Cyc_Absyn_Enumfield*)(_tmp339.YY47).val;
_LL201: yyzzz=_tmp33A;goto _LL1FF;_LL202:;_LL203:(int)_throw((void*)& Cyc_yyfail_YY47);
_LL1FF:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1);
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp9FA;
return((_tmp9FA.YY47).val=yy1,(((_tmp9FA.YY47).tag=52,_tmp9FA)));}static char
_tmp33D[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={
Cyc_Core_Failure,{_tmp33D,_tmp33D,_tmp33D + 23}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp33E=yy1;struct Cyc_List_List*
_tmp33F;_LL205: if((_tmp33E.YY48).tag != 53)goto _LL207;_tmp33F=(struct Cyc_List_List*)(
_tmp33E.YY48).val;_LL206: yyzzz=_tmp33F;goto _LL204;_LL207:;_LL208:(int)_throw((
void*)& Cyc_yyfail_YY48);_LL204:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FB;
return((_tmp9FB.YY48).val=yy1,(((_tmp9FB.YY48).tag=53,_tmp9FB)));}static char
_tmp342[17]="opt_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={
Cyc_Core_Failure,{_tmp342,_tmp342,_tmp342 + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(
union Cyc_YYSTYPE yy1);struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp343=yy1;struct Cyc_Core_Opt*_tmp344;
_LL20A: if((_tmp343.YY49).tag != 54)goto _LL20C;_tmp344=(struct Cyc_Core_Opt*)(
_tmp343.YY49).val;_LL20B: yyzzz=_tmp344;goto _LL209;_LL20C:;_LL20D:(int)_throw((
void*)& Cyc_yyfail_YY49);_LL209:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*
yy1);union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9FC;
return((_tmp9FC.YY49).val=yy1,(((_tmp9FC.YY49).tag=54,_tmp9FC)));}static char
_tmp347[31]="list_t<$(type_t,type_t)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp347,_tmp347,_tmp347 + 31}};struct Cyc_List_List*
Cyc_yyget_YY50(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp348=yy1;struct Cyc_List_List*
_tmp349;_LL20F: if((_tmp348.YY50).tag != 55)goto _LL211;_tmp349=(struct Cyc_List_List*)(
_tmp348.YY50).val;_LL210: yyzzz=_tmp349;goto _LL20E;_LL211:;_LL212:(int)_throw((
void*)& Cyc_yyfail_YY50);_LL20E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FD;
return((_tmp9FD.YY50).val=yy1,(((_tmp9FD.YY50).tag=55,_tmp9FD)));}static char
_tmp34C[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY51={
Cyc_Core_Failure,{_tmp34C,_tmp34C,_tmp34C + 15}};union Cyc_Absyn_Constraint*Cyc_yyget_YY51(
union Cyc_YYSTYPE yy1);union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE
yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp34D=yy1;union Cyc_Absyn_Constraint*
_tmp34E;_LL214: if((_tmp34D.YY51).tag != 56)goto _LL216;_tmp34E=(union Cyc_Absyn_Constraint*)(
_tmp34D.YY51).val;_LL215: yyzzz=_tmp34E;goto _LL213;_LL216:;_LL217:(int)_throw((
void*)& Cyc_yyfail_YY51);_LL213:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*
yy1);union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE
_tmp9FE;return((_tmp9FE.YY51).val=yy1,(((_tmp9FE.YY51).tag=56,_tmp9FE)));}static
char _tmp351[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp351,_tmp351,_tmp351 + 45}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp352=yy1;struct Cyc_List_List*
_tmp353;_LL219: if((_tmp352.YY52).tag != 57)goto _LL21B;_tmp353=(struct Cyc_List_List*)(
_tmp352.YY52).val;_LL21A: yyzzz=_tmp353;goto _LL218;_LL21B:;_LL21C:(int)_throw((
void*)& Cyc_yyfail_YY52);_LL218:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FF;
return((_tmp9FF.YY52).val=yy1,(((_tmp9FF.YY52).tag=57,_tmp9FF)));}static char
_tmp356[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp356,_tmp356,_tmp356 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp357=yy1;void*_tmp358;_LL21E: if((_tmp357.YY53).tag != 58)goto _LL220;_tmp358=(
void*)(_tmp357.YY53).val;_LL21F: yyzzz=_tmp358;goto _LL21D;_LL220:;_LL221:(int)
_throw((void*)& Cyc_yyfail_YY53);_LL21D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY53(
void*yy1);union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmpA00;return((
_tmpA00.YY53).val=yy1,(((_tmpA00.YY53).tag=58,_tmpA00)));}static char _tmp35B[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp35B,
_tmp35B,_tmp35B + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp35C=yy1;struct Cyc_List_List*_tmp35D;_LL223: if((
_tmp35C.YY54).tag != 59)goto _LL225;_tmp35D=(struct Cyc_List_List*)(_tmp35C.YY54).val;
_LL224: yyzzz=_tmp35D;goto _LL222;_LL225:;_LL226:(int)_throw((void*)& Cyc_yyfail_YY54);
_LL222:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY54(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA01;return((_tmpA01.YY54).val=
yy1,(((_tmpA01.YY54).tag=59,_tmpA01)));}struct Cyc_Yyltype{int timestamp;int
first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc();
struct Cyc_Yyltype Cyc_yynewloc(){struct Cyc_Yyltype _tmpA02;return(_tmpA02.timestamp=
0,((_tmpA02.first_line=0,((_tmpA02.first_column=0,((_tmpA02.last_line=0,((
_tmpA02.last_column=0,_tmpA02)))))))));}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
static short Cyc_yytranslate[371]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,142,2,2,126,140,137,2,123,124,131,134,119,138,128,139,2,2,2,2,
2,2,2,2,2,2,127,116,121,120,122,133,132,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,129,2,130,136,125,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,117,135,118,141,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,
45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,
99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115};static char
_tmp360[2]="$";static char _tmp361[6]="error";static char _tmp362[12]="$undefined.";
static char _tmp363[5]="AUTO";static char _tmp364[9]="REGISTER";static char _tmp365[7]="STATIC";
static char _tmp366[7]="EXTERN";static char _tmp367[8]="TYPEDEF";static char _tmp368[5]="VOID";
static char _tmp369[5]="CHAR";static char _tmp36A[6]="SHORT";static char _tmp36B[4]="INT";
static char _tmp36C[5]="LONG";static char _tmp36D[6]="FLOAT";static char _tmp36E[7]="DOUBLE";
static char _tmp36F[7]="SIGNED";static char _tmp370[9]="UNSIGNED";static char _tmp371[
6]="CONST";static char _tmp372[9]="VOLATILE";static char _tmp373[9]="RESTRICT";
static char _tmp374[7]="STRUCT";static char _tmp375[6]="UNION";static char _tmp376[5]="CASE";
static char _tmp377[8]="DEFAULT";static char _tmp378[7]="INLINE";static char _tmp379[7]="SIZEOF";
static char _tmp37A[9]="OFFSETOF";static char _tmp37B[3]="IF";static char _tmp37C[5]="ELSE";
static char _tmp37D[7]="SWITCH";static char _tmp37E[6]="WHILE";static char _tmp37F[3]="DO";
static char _tmp380[4]="FOR";static char _tmp381[5]="GOTO";static char _tmp382[9]="CONTINUE";
static char _tmp383[6]="BREAK";static char _tmp384[7]="RETURN";static char _tmp385[5]="ENUM";
static char _tmp386[8]="NULL_kw";static char _tmp387[4]="LET";static char _tmp388[6]="THROW";
static char _tmp389[4]="TRY";static char _tmp38A[6]="CATCH";static char _tmp38B[7]="EXPORT";
static char _tmp38C[4]="NEW";static char _tmp38D[9]="ABSTRACT";static char _tmp38E[9]="FALLTHRU";
static char _tmp38F[6]="USING";static char _tmp390[10]="NAMESPACE";static char _tmp391[
9]="DATATYPE";static char _tmp392[8]="XTUNION";static char _tmp393[7]="TUNION";
static char _tmp394[7]="MALLOC";static char _tmp395[8]="RMALLOC";static char _tmp396[7]="CALLOC";
static char _tmp397[8]="RCALLOC";static char _tmp398[5]="SWAP";static char _tmp399[9]="REGION_T";
static char _tmp39A[6]="TAG_T";static char _tmp39B[7]="REGION";static char _tmp39C[5]="RNEW";
static char _tmp39D[8]="REGIONS";static char _tmp39E[13]="RESET_REGION";static char
_tmp39F[16]="NOZEROTERM_QUAL";static char _tmp3A0[14]="ZEROTERM_QUAL";static char
_tmp3A1[12]="REGION_QUAL";static char _tmp3A2[7]="PORTON";static char _tmp3A3[8]="PORTOFF";
static char _tmp3A4[12]="DYNREGION_T";static char _tmp3A5[6]="ALIAS";static char
_tmp3A6[8]="NUMELTS";static char _tmp3A7[8]="VALUEOF";static char _tmp3A8[10]="VALUEOF_T";
static char _tmp3A9[9]="TAGCHECK";static char _tmp3AA[13]="NUMELTS_QUAL";static char
_tmp3AB[10]="THIN_QUAL";static char _tmp3AC[9]="FAT_QUAL";static char _tmp3AD[13]="NOTNULL_QUAL";
static char _tmp3AE[14]="NULLABLE_QUAL";static char _tmp3AF[12]="TAGGED_QUAL";static
char _tmp3B0[16]="EXTENSIBLE_QUAL";static char _tmp3B1[15]="RESETABLE_QUAL";static
char _tmp3B2[7]="PTR_OP";static char _tmp3B3[7]="INC_OP";static char _tmp3B4[7]="DEC_OP";
static char _tmp3B5[8]="LEFT_OP";static char _tmp3B6[9]="RIGHT_OP";static char _tmp3B7[
6]="LE_OP";static char _tmp3B8[6]="GE_OP";static char _tmp3B9[6]="EQ_OP";static char
_tmp3BA[6]="NE_OP";static char _tmp3BB[7]="AND_OP";static char _tmp3BC[6]="OR_OP";
static char _tmp3BD[11]="MUL_ASSIGN";static char _tmp3BE[11]="DIV_ASSIGN";static char
_tmp3BF[11]="MOD_ASSIGN";static char _tmp3C0[11]="ADD_ASSIGN";static char _tmp3C1[11]="SUB_ASSIGN";
static char _tmp3C2[12]="LEFT_ASSIGN";static char _tmp3C3[13]="RIGHT_ASSIGN";static
char _tmp3C4[11]="AND_ASSIGN";static char _tmp3C5[11]="XOR_ASSIGN";static char
_tmp3C6[10]="OR_ASSIGN";static char _tmp3C7[9]="ELLIPSIS";static char _tmp3C8[11]="LEFT_RIGHT";
static char _tmp3C9[12]="COLON_COLON";static char _tmp3CA[11]="IDENTIFIER";static
char _tmp3CB[17]="INTEGER_CONSTANT";static char _tmp3CC[7]="STRING";static char
_tmp3CD[19]="CHARACTER_CONSTANT";static char _tmp3CE[18]="FLOATING_CONSTANT";
static char _tmp3CF[9]="TYPE_VAR";static char _tmp3D0[13]="TYPEDEF_NAME";static char
_tmp3D1[16]="QUAL_IDENTIFIER";static char _tmp3D2[18]="QUAL_TYPEDEF_NAME";static
char _tmp3D3[10]="ATTRIBUTE";static char _tmp3D4[4]="';'";static char _tmp3D5[4]="'{'";
static char _tmp3D6[4]="'}'";static char _tmp3D7[4]="','";static char _tmp3D8[4]="'='";
static char _tmp3D9[4]="'<'";static char _tmp3DA[4]="'>'";static char _tmp3DB[4]="'('";
static char _tmp3DC[4]="')'";static char _tmp3DD[4]="'_'";static char _tmp3DE[4]="'$'";
static char _tmp3DF[4]="':'";static char _tmp3E0[4]="'.'";static char _tmp3E1[4]="'['";
static char _tmp3E2[4]="']'";static char _tmp3E3[4]="'*'";static char _tmp3E4[4]="'@'";
static char _tmp3E5[4]="'?'";static char _tmp3E6[4]="'+'";static char _tmp3E7[4]="'|'";
static char _tmp3E8[4]="'^'";static char _tmp3E9[4]="'&'";static char _tmp3EA[4]="'-'";
static char _tmp3EB[4]="'/'";static char _tmp3EC[4]="'%'";static char _tmp3ED[4]="'~'";
static char _tmp3EE[4]="'!'";static char _tmp3EF[5]="prog";static char _tmp3F0[17]="translation_unit";
static char _tmp3F1[12]="export_list";static char _tmp3F2[19]="export_list_values";
static char _tmp3F3[21]="external_declaration";static char _tmp3F4[20]="function_definition";
static char _tmp3F5[21]="function_definition2";static char _tmp3F6[13]="using_action";
static char _tmp3F7[15]="unusing_action";static char _tmp3F8[17]="namespace_action";
static char _tmp3F9[19]="unnamespace_action";static char _tmp3FA[12]="declaration";
static char _tmp3FB[19]="resetable_qual_opt";static char _tmp3FC[17]="declaration_list";
static char _tmp3FD[23]="declaration_specifiers";static char _tmp3FE[24]="storage_class_specifier";
static char _tmp3FF[15]="attributes_opt";static char _tmp400[11]="attributes";static
char _tmp401[15]="attribute_list";static char _tmp402[10]="attribute";static char
_tmp403[15]="type_specifier";static char _tmp404[25]="type_specifier_notypedef";
static char _tmp405[5]="kind";static char _tmp406[15]="type_qualifier";static char
_tmp407[15]="enum_specifier";static char _tmp408[11]="enum_field";static char
_tmp409[22]="enum_declaration_list";static char _tmp40A[26]="struct_or_union_specifier";
static char _tmp40B[16]="type_params_opt";static char _tmp40C[16]="struct_or_union";
static char _tmp40D[24]="struct_declaration_list";static char _tmp40E[25]="struct_declaration_list0";
static char _tmp40F[21]="init_declarator_list";static char _tmp410[22]="init_declarator_list0";
static char _tmp411[16]="init_declarator";static char _tmp412[19]="struct_declaration";
static char _tmp413[25]="specifier_qualifier_list";static char _tmp414[35]="notypedef_specifier_qualifier_list";
static char _tmp415[23]="struct_declarator_list";static char _tmp416[24]="struct_declarator_list0";
static char _tmp417[18]="struct_declarator";static char _tmp418[19]="datatype_specifier";
static char _tmp419[14]="qual_datatype";static char _tmp41A[19]="datatypefield_list";
static char _tmp41B[20]="datatypefield_scope";static char _tmp41C[14]="datatypefield";
static char _tmp41D[11]="declarator";static char _tmp41E[23]="declarator_withtypedef";
static char _tmp41F[18]="direct_declarator";static char _tmp420[30]="direct_declarator_withtypedef";
static char _tmp421[8]="pointer";static char _tmp422[12]="one_pointer";static char
_tmp423[14]="pointer_quals";static char _tmp424[13]="pointer_qual";static char
_tmp425[23]="pointer_null_and_bound";static char _tmp426[14]="pointer_bound";
static char _tmp427[18]="zeroterm_qual_opt";static char _tmp428[8]="rgn_opt";static
char _tmp429[11]="tqual_list";static char _tmp42A[20]="parameter_type_list";static
char _tmp42B[9]="type_var";static char _tmp42C[16]="optional_effect";static char
_tmp42D[19]="optional_rgn_order";static char _tmp42E[10]="rgn_order";static char
_tmp42F[16]="optional_inject";static char _tmp430[11]="effect_set";static char
_tmp431[14]="atomic_effect";static char _tmp432[11]="region_set";static char _tmp433[
15]="parameter_list";static char _tmp434[22]="parameter_declaration";static char
_tmp435[16]="identifier_list";static char _tmp436[17]="identifier_list0";static
char _tmp437[12]="initializer";static char _tmp438[18]="array_initializer";static
char _tmp439[17]="initializer_list";static char _tmp43A[12]="designation";static
char _tmp43B[16]="designator_list";static char _tmp43C[11]="designator";static char
_tmp43D[10]="type_name";static char _tmp43E[14]="any_type_name";static char _tmp43F[
15]="type_name_list";static char _tmp440[20]="abstract_declarator";static char
_tmp441[27]="direct_abstract_declarator";static char _tmp442[10]="statement";
static char _tmp443[13]="open_exp_opt";static char _tmp444[18]="labeled_statement";
static char _tmp445[21]="expression_statement";static char _tmp446[19]="compound_statement";
static char _tmp447[16]="block_item_list";static char _tmp448[20]="selection_statement";
static char _tmp449[15]="switch_clauses";static char _tmp44A[20]="iteration_statement";
static char _tmp44B[15]="jump_statement";static char _tmp44C[12]="exp_pattern";
static char _tmp44D[20]="conditional_pattern";static char _tmp44E[19]="logical_or_pattern";
static char _tmp44F[20]="logical_and_pattern";static char _tmp450[21]="inclusive_or_pattern";
static char _tmp451[21]="exclusive_or_pattern";static char _tmp452[12]="and_pattern";
static char _tmp453[17]="equality_pattern";static char _tmp454[19]="relational_pattern";
static char _tmp455[14]="shift_pattern";static char _tmp456[17]="additive_pattern";
static char _tmp457[23]="multiplicative_pattern";static char _tmp458[13]="cast_pattern";
static char _tmp459[14]="unary_pattern";static char _tmp45A[16]="postfix_pattern";
static char _tmp45B[16]="primary_pattern";static char _tmp45C[8]="pattern";static
char _tmp45D[19]="tuple_pattern_list";static char _tmp45E[20]="tuple_pattern_list0";
static char _tmp45F[14]="field_pattern";static char _tmp460[19]="field_pattern_list";
static char _tmp461[20]="field_pattern_list0";static char _tmp462[11]="expression";
static char _tmp463[22]="assignment_expression";static char _tmp464[20]="assignment_operator";
static char _tmp465[23]="conditional_expression";static char _tmp466[20]="constant_expression";
static char _tmp467[22]="logical_or_expression";static char _tmp468[23]="logical_and_expression";
static char _tmp469[24]="inclusive_or_expression";static char _tmp46A[24]="exclusive_or_expression";
static char _tmp46B[15]="and_expression";static char _tmp46C[20]="equality_expression";
static char _tmp46D[22]="relational_expression";static char _tmp46E[17]="shift_expression";
static char _tmp46F[20]="additive_expression";static char _tmp470[26]="multiplicative_expression";
static char _tmp471[16]="cast_expression";static char _tmp472[17]="unary_expression";
static char _tmp473[15]="unary_operator";static char _tmp474[19]="postfix_expression";
static char _tmp475[19]="primary_expression";static char _tmp476[25]="argument_expression_list";
static char _tmp477[26]="argument_expression_list0";static char _tmp478[9]="constant";
static char _tmp479[20]="qual_opt_identifier";static char _tmp47A[17]="qual_opt_typedef";
static char _tmp47B[18]="struct_union_name";static char _tmp47C[11]="field_name";
static char _tmp47D[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[286]={{
_tmp360,_tmp360,_tmp360 + 2},{_tmp361,_tmp361,_tmp361 + 6},{_tmp362,_tmp362,
_tmp362 + 12},{_tmp363,_tmp363,_tmp363 + 5},{_tmp364,_tmp364,_tmp364 + 9},{_tmp365,
_tmp365,_tmp365 + 7},{_tmp366,_tmp366,_tmp366 + 7},{_tmp367,_tmp367,_tmp367 + 8},{
_tmp368,_tmp368,_tmp368 + 5},{_tmp369,_tmp369,_tmp369 + 5},{_tmp36A,_tmp36A,
_tmp36A + 6},{_tmp36B,_tmp36B,_tmp36B + 4},{_tmp36C,_tmp36C,_tmp36C + 5},{_tmp36D,
_tmp36D,_tmp36D + 6},{_tmp36E,_tmp36E,_tmp36E + 7},{_tmp36F,_tmp36F,_tmp36F + 7},{
_tmp370,_tmp370,_tmp370 + 9},{_tmp371,_tmp371,_tmp371 + 6},{_tmp372,_tmp372,
_tmp372 + 9},{_tmp373,_tmp373,_tmp373 + 9},{_tmp374,_tmp374,_tmp374 + 7},{_tmp375,
_tmp375,_tmp375 + 6},{_tmp376,_tmp376,_tmp376 + 5},{_tmp377,_tmp377,_tmp377 + 8},{
_tmp378,_tmp378,_tmp378 + 7},{_tmp379,_tmp379,_tmp379 + 7},{_tmp37A,_tmp37A,
_tmp37A + 9},{_tmp37B,_tmp37B,_tmp37B + 3},{_tmp37C,_tmp37C,_tmp37C + 5},{_tmp37D,
_tmp37D,_tmp37D + 7},{_tmp37E,_tmp37E,_tmp37E + 6},{_tmp37F,_tmp37F,_tmp37F + 3},{
_tmp380,_tmp380,_tmp380 + 4},{_tmp381,_tmp381,_tmp381 + 5},{_tmp382,_tmp382,
_tmp382 + 9},{_tmp383,_tmp383,_tmp383 + 6},{_tmp384,_tmp384,_tmp384 + 7},{_tmp385,
_tmp385,_tmp385 + 5},{_tmp386,_tmp386,_tmp386 + 8},{_tmp387,_tmp387,_tmp387 + 4},{
_tmp388,_tmp388,_tmp388 + 6},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,
_tmp38A + 6},{_tmp38B,_tmp38B,_tmp38B + 7},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,
_tmp38D,_tmp38D + 9},{_tmp38E,_tmp38E,_tmp38E + 9},{_tmp38F,_tmp38F,_tmp38F + 6},{
_tmp390,_tmp390,_tmp390 + 10},{_tmp391,_tmp391,_tmp391 + 9},{_tmp392,_tmp392,
_tmp392 + 8},{_tmp393,_tmp393,_tmp393 + 7},{_tmp394,_tmp394,_tmp394 + 7},{_tmp395,
_tmp395,_tmp395 + 8},{_tmp396,_tmp396,_tmp396 + 7},{_tmp397,_tmp397,_tmp397 + 8},{
_tmp398,_tmp398,_tmp398 + 5},{_tmp399,_tmp399,_tmp399 + 9},{_tmp39A,_tmp39A,
_tmp39A + 6},{_tmp39B,_tmp39B,_tmp39B + 7},{_tmp39C,_tmp39C,_tmp39C + 5},{_tmp39D,
_tmp39D,_tmp39D + 8},{_tmp39E,_tmp39E,_tmp39E + 13},{_tmp39F,_tmp39F,_tmp39F + 16},{
_tmp3A0,_tmp3A0,_tmp3A0 + 14},{_tmp3A1,_tmp3A1,_tmp3A1 + 12},{_tmp3A2,_tmp3A2,
_tmp3A2 + 7},{_tmp3A3,_tmp3A3,_tmp3A3 + 8},{_tmp3A4,_tmp3A4,_tmp3A4 + 12},{_tmp3A5,
_tmp3A5,_tmp3A5 + 6},{_tmp3A6,_tmp3A6,_tmp3A6 + 8},{_tmp3A7,_tmp3A7,_tmp3A7 + 8},{
_tmp3A8,_tmp3A8,_tmp3A8 + 10},{_tmp3A9,_tmp3A9,_tmp3A9 + 9},{_tmp3AA,_tmp3AA,
_tmp3AA + 13},{_tmp3AB,_tmp3AB,_tmp3AB + 10},{_tmp3AC,_tmp3AC,_tmp3AC + 9},{_tmp3AD,
_tmp3AD,_tmp3AD + 13},{_tmp3AE,_tmp3AE,_tmp3AE + 14},{_tmp3AF,_tmp3AF,_tmp3AF + 12},{
_tmp3B0,_tmp3B0,_tmp3B0 + 16},{_tmp3B1,_tmp3B1,_tmp3B1 + 15},{_tmp3B2,_tmp3B2,
_tmp3B2 + 7},{_tmp3B3,_tmp3B3,_tmp3B3 + 7},{_tmp3B4,_tmp3B4,_tmp3B4 + 7},{_tmp3B5,
_tmp3B5,_tmp3B5 + 8},{_tmp3B6,_tmp3B6,_tmp3B6 + 9},{_tmp3B7,_tmp3B7,_tmp3B7 + 6},{
_tmp3B8,_tmp3B8,_tmp3B8 + 6},{_tmp3B9,_tmp3B9,_tmp3B9 + 6},{_tmp3BA,_tmp3BA,
_tmp3BA + 6},{_tmp3BB,_tmp3BB,_tmp3BB + 7},{_tmp3BC,_tmp3BC,_tmp3BC + 6},{_tmp3BD,
_tmp3BD,_tmp3BD + 11},{_tmp3BE,_tmp3BE,_tmp3BE + 11},{_tmp3BF,_tmp3BF,_tmp3BF + 11},{
_tmp3C0,_tmp3C0,_tmp3C0 + 11},{_tmp3C1,_tmp3C1,_tmp3C1 + 11},{_tmp3C2,_tmp3C2,
_tmp3C2 + 12},{_tmp3C3,_tmp3C3,_tmp3C3 + 13},{_tmp3C4,_tmp3C4,_tmp3C4 + 11},{
_tmp3C5,_tmp3C5,_tmp3C5 + 11},{_tmp3C6,_tmp3C6,_tmp3C6 + 10},{_tmp3C7,_tmp3C7,
_tmp3C7 + 9},{_tmp3C8,_tmp3C8,_tmp3C8 + 11},{_tmp3C9,_tmp3C9,_tmp3C9 + 12},{_tmp3CA,
_tmp3CA,_tmp3CA + 11},{_tmp3CB,_tmp3CB,_tmp3CB + 17},{_tmp3CC,_tmp3CC,_tmp3CC + 7},{
_tmp3CD,_tmp3CD,_tmp3CD + 19},{_tmp3CE,_tmp3CE,_tmp3CE + 18},{_tmp3CF,_tmp3CF,
_tmp3CF + 9},{_tmp3D0,_tmp3D0,_tmp3D0 + 13},{_tmp3D1,_tmp3D1,_tmp3D1 + 16},{_tmp3D2,
_tmp3D2,_tmp3D2 + 18},{_tmp3D3,_tmp3D3,_tmp3D3 + 10},{_tmp3D4,_tmp3D4,_tmp3D4 + 4},{
_tmp3D5,_tmp3D5,_tmp3D5 + 4},{_tmp3D6,_tmp3D6,_tmp3D6 + 4},{_tmp3D7,_tmp3D7,
_tmp3D7 + 4},{_tmp3D8,_tmp3D8,_tmp3D8 + 4},{_tmp3D9,_tmp3D9,_tmp3D9 + 4},{_tmp3DA,
_tmp3DA,_tmp3DA + 4},{_tmp3DB,_tmp3DB,_tmp3DB + 4},{_tmp3DC,_tmp3DC,_tmp3DC + 4},{
_tmp3DD,_tmp3DD,_tmp3DD + 4},{_tmp3DE,_tmp3DE,_tmp3DE + 4},{_tmp3DF,_tmp3DF,
_tmp3DF + 4},{_tmp3E0,_tmp3E0,_tmp3E0 + 4},{_tmp3E1,_tmp3E1,_tmp3E1 + 4},{_tmp3E2,
_tmp3E2,_tmp3E2 + 4},{_tmp3E3,_tmp3E3,_tmp3E3 + 4},{_tmp3E4,_tmp3E4,_tmp3E4 + 4},{
_tmp3E5,_tmp3E5,_tmp3E5 + 4},{_tmp3E6,_tmp3E6,_tmp3E6 + 4},{_tmp3E7,_tmp3E7,
_tmp3E7 + 4},{_tmp3E8,_tmp3E8,_tmp3E8 + 4},{_tmp3E9,_tmp3E9,_tmp3E9 + 4},{_tmp3EA,
_tmp3EA,_tmp3EA + 4},{_tmp3EB,_tmp3EB,_tmp3EB + 4},{_tmp3EC,_tmp3EC,_tmp3EC + 4},{
_tmp3ED,_tmp3ED,_tmp3ED + 4},{_tmp3EE,_tmp3EE,_tmp3EE + 4},{_tmp3EF,_tmp3EF,
_tmp3EF + 5},{_tmp3F0,_tmp3F0,_tmp3F0 + 17},{_tmp3F1,_tmp3F1,_tmp3F1 + 12},{_tmp3F2,
_tmp3F2,_tmp3F2 + 19},{_tmp3F3,_tmp3F3,_tmp3F3 + 21},{_tmp3F4,_tmp3F4,_tmp3F4 + 20},{
_tmp3F5,_tmp3F5,_tmp3F5 + 21},{_tmp3F6,_tmp3F6,_tmp3F6 + 13},{_tmp3F7,_tmp3F7,
_tmp3F7 + 15},{_tmp3F8,_tmp3F8,_tmp3F8 + 17},{_tmp3F9,_tmp3F9,_tmp3F9 + 19},{
_tmp3FA,_tmp3FA,_tmp3FA + 12},{_tmp3FB,_tmp3FB,_tmp3FB + 19},{_tmp3FC,_tmp3FC,
_tmp3FC + 17},{_tmp3FD,_tmp3FD,_tmp3FD + 23},{_tmp3FE,_tmp3FE,_tmp3FE + 24},{
_tmp3FF,_tmp3FF,_tmp3FF + 15},{_tmp400,_tmp400,_tmp400 + 11},{_tmp401,_tmp401,
_tmp401 + 15},{_tmp402,_tmp402,_tmp402 + 10},{_tmp403,_tmp403,_tmp403 + 15},{
_tmp404,_tmp404,_tmp404 + 25},{_tmp405,_tmp405,_tmp405 + 5},{_tmp406,_tmp406,
_tmp406 + 15},{_tmp407,_tmp407,_tmp407 + 15},{_tmp408,_tmp408,_tmp408 + 11},{
_tmp409,_tmp409,_tmp409 + 22},{_tmp40A,_tmp40A,_tmp40A + 26},{_tmp40B,_tmp40B,
_tmp40B + 16},{_tmp40C,_tmp40C,_tmp40C + 16},{_tmp40D,_tmp40D,_tmp40D + 24},{
_tmp40E,_tmp40E,_tmp40E + 25},{_tmp40F,_tmp40F,_tmp40F + 21},{_tmp410,_tmp410,
_tmp410 + 22},{_tmp411,_tmp411,_tmp411 + 16},{_tmp412,_tmp412,_tmp412 + 19},{
_tmp413,_tmp413,_tmp413 + 25},{_tmp414,_tmp414,_tmp414 + 35},{_tmp415,_tmp415,
_tmp415 + 23},{_tmp416,_tmp416,_tmp416 + 24},{_tmp417,_tmp417,_tmp417 + 18},{
_tmp418,_tmp418,_tmp418 + 19},{_tmp419,_tmp419,_tmp419 + 14},{_tmp41A,_tmp41A,
_tmp41A + 19},{_tmp41B,_tmp41B,_tmp41B + 20},{_tmp41C,_tmp41C,_tmp41C + 14},{
_tmp41D,_tmp41D,_tmp41D + 11},{_tmp41E,_tmp41E,_tmp41E + 23},{_tmp41F,_tmp41F,
_tmp41F + 18},{_tmp420,_tmp420,_tmp420 + 30},{_tmp421,_tmp421,_tmp421 + 8},{_tmp422,
_tmp422,_tmp422 + 12},{_tmp423,_tmp423,_tmp423 + 14},{_tmp424,_tmp424,_tmp424 + 13},{
_tmp425,_tmp425,_tmp425 + 23},{_tmp426,_tmp426,_tmp426 + 14},{_tmp427,_tmp427,
_tmp427 + 18},{_tmp428,_tmp428,_tmp428 + 8},{_tmp429,_tmp429,_tmp429 + 11},{_tmp42A,
_tmp42A,_tmp42A + 20},{_tmp42B,_tmp42B,_tmp42B + 9},{_tmp42C,_tmp42C,_tmp42C + 16},{
_tmp42D,_tmp42D,_tmp42D + 19},{_tmp42E,_tmp42E,_tmp42E + 10},{_tmp42F,_tmp42F,
_tmp42F + 16},{_tmp430,_tmp430,_tmp430 + 11},{_tmp431,_tmp431,_tmp431 + 14},{
_tmp432,_tmp432,_tmp432 + 11},{_tmp433,_tmp433,_tmp433 + 15},{_tmp434,_tmp434,
_tmp434 + 22},{_tmp435,_tmp435,_tmp435 + 16},{_tmp436,_tmp436,_tmp436 + 17},{
_tmp437,_tmp437,_tmp437 + 12},{_tmp438,_tmp438,_tmp438 + 18},{_tmp439,_tmp439,
_tmp439 + 17},{_tmp43A,_tmp43A,_tmp43A + 12},{_tmp43B,_tmp43B,_tmp43B + 16},{
_tmp43C,_tmp43C,_tmp43C + 11},{_tmp43D,_tmp43D,_tmp43D + 10},{_tmp43E,_tmp43E,
_tmp43E + 14},{_tmp43F,_tmp43F,_tmp43F + 15},{_tmp440,_tmp440,_tmp440 + 20},{
_tmp441,_tmp441,_tmp441 + 27},{_tmp442,_tmp442,_tmp442 + 10},{_tmp443,_tmp443,
_tmp443 + 13},{_tmp444,_tmp444,_tmp444 + 18},{_tmp445,_tmp445,_tmp445 + 21},{
_tmp446,_tmp446,_tmp446 + 19},{_tmp447,_tmp447,_tmp447 + 16},{_tmp448,_tmp448,
_tmp448 + 20},{_tmp449,_tmp449,_tmp449 + 15},{_tmp44A,_tmp44A,_tmp44A + 20},{
_tmp44B,_tmp44B,_tmp44B + 15},{_tmp44C,_tmp44C,_tmp44C + 12},{_tmp44D,_tmp44D,
_tmp44D + 20},{_tmp44E,_tmp44E,_tmp44E + 19},{_tmp44F,_tmp44F,_tmp44F + 20},{
_tmp450,_tmp450,_tmp450 + 21},{_tmp451,_tmp451,_tmp451 + 21},{_tmp452,_tmp452,
_tmp452 + 12},{_tmp453,_tmp453,_tmp453 + 17},{_tmp454,_tmp454,_tmp454 + 19},{
_tmp455,_tmp455,_tmp455 + 14},{_tmp456,_tmp456,_tmp456 + 17},{_tmp457,_tmp457,
_tmp457 + 23},{_tmp458,_tmp458,_tmp458 + 13},{_tmp459,_tmp459,_tmp459 + 14},{
_tmp45A,_tmp45A,_tmp45A + 16},{_tmp45B,_tmp45B,_tmp45B + 16},{_tmp45C,_tmp45C,
_tmp45C + 8},{_tmp45D,_tmp45D,_tmp45D + 19},{_tmp45E,_tmp45E,_tmp45E + 20},{_tmp45F,
_tmp45F,_tmp45F + 14},{_tmp460,_tmp460,_tmp460 + 19},{_tmp461,_tmp461,_tmp461 + 20},{
_tmp462,_tmp462,_tmp462 + 11},{_tmp463,_tmp463,_tmp463 + 22},{_tmp464,_tmp464,
_tmp464 + 20},{_tmp465,_tmp465,_tmp465 + 23},{_tmp466,_tmp466,_tmp466 + 20},{
_tmp467,_tmp467,_tmp467 + 22},{_tmp468,_tmp468,_tmp468 + 23},{_tmp469,_tmp469,
_tmp469 + 24},{_tmp46A,_tmp46A,_tmp46A + 24},{_tmp46B,_tmp46B,_tmp46B + 15},{
_tmp46C,_tmp46C,_tmp46C + 20},{_tmp46D,_tmp46D,_tmp46D + 22},{_tmp46E,_tmp46E,
_tmp46E + 17},{_tmp46F,_tmp46F,_tmp46F + 20},{_tmp470,_tmp470,_tmp470 + 26},{
_tmp471,_tmp471,_tmp471 + 16},{_tmp472,_tmp472,_tmp472 + 17},{_tmp473,_tmp473,
_tmp473 + 15},{_tmp474,_tmp474,_tmp474 + 19},{_tmp475,_tmp475,_tmp475 + 19},{
_tmp476,_tmp476,_tmp476 + 25},{_tmp477,_tmp477,_tmp477 + 26},{_tmp478,_tmp478,
_tmp478 + 9},{_tmp479,_tmp479,_tmp479 + 20},{_tmp47A,_tmp47A,_tmp47A + 17},{_tmp47B,
_tmp47B,_tmp47B + 18},{_tmp47C,_tmp47C,_tmp47C + 11},{_tmp47D,_tmp47D,_tmp47D + 12}};
static short Cyc_yyr1[510]={0,143,144,144,144,144,144,144,144,144,144,144,145,145,
146,146,147,147,147,148,148,148,148,149,149,150,151,152,153,154,154,154,154,154,
154,154,155,155,156,156,157,157,157,157,157,157,157,157,157,157,158,158,158,158,
158,158,158,159,159,160,161,161,162,162,162,162,162,162,163,163,164,164,164,164,
164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,165,
166,166,166,167,167,167,168,168,169,169,169,170,170,170,170,170,171,171,172,172,
173,173,174,174,175,176,176,177,177,178,179,179,179,179,179,179,180,180,180,180,
180,180,181,182,182,183,183,183,184,184,184,185,185,186,186,186,186,187,187,187,
188,188,189,189,190,190,191,191,191,191,191,191,191,191,191,191,192,192,192,192,
192,192,192,192,192,192,192,193,193,194,195,195,196,196,196,196,196,196,196,196,
197,197,197,198,198,199,199,199,200,200,200,201,201,202,202,202,202,203,203,204,
204,205,205,206,206,207,207,208,208,209,209,209,209,210,210,211,211,212,212,212,
213,214,214,215,215,216,216,216,216,217,217,217,217,218,219,219,220,220,221,221,
222,222,222,222,222,223,223,224,224,224,225,225,225,225,225,225,225,225,225,225,
225,226,226,226,226,226,226,226,227,227,228,229,229,230,230,231,231,231,231,231,
231,232,232,232,232,232,232,233,233,233,233,233,233,234,234,234,234,234,234,234,
234,234,234,234,234,234,234,235,235,235,235,235,235,235,235,236,237,237,238,238,
239,239,240,240,241,241,242,242,243,243,243,244,244,244,244,244,245,245,245,246,
246,246,247,247,247,247,248,248,249,249,249,249,249,249,249,250,251,252,252,252,
252,252,252,252,252,252,252,252,252,252,252,252,253,253,253,254,254,255,255,256,
256,256,257,257,258,258,259,259,259,260,260,260,260,260,260,260,260,260,260,260,
261,261,261,261,261,261,261,262,263,263,264,264,265,265,266,266,267,267,268,268,
268,269,269,269,269,269,270,270,270,271,271,271,272,272,272,272,273,273,274,274,
274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,275,275,275,
276,276,276,276,276,276,276,276,276,276,277,277,277,277,277,277,277,277,277,278,
279,279,280,280,280,280,281,281,282,282,283,283,284,284,285,285};static short Cyc_yyr2[
510]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,8,0,
1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,
1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,
2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,
2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,
0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,
1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,
1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,
6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,
3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,
3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,
3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,4,6,9,11,4,6,6,4,1,1,1,1,4,3,
4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short
Cyc_yydefact[1027]={0,18,50,51,52,53,55,70,73,74,75,76,77,78,79,80,94,95,96,112,
113,46,0,0,56,0,0,145,87,91,0,0,0,0,0,0,0,37,500,211,502,501,503,0,0,71,0,197,
197,196,1,0,16,0,0,17,0,0,40,48,42,68,44,81,82,0,83,0,36,156,0,181,184,84,160,
110,54,53,47,0,98,499,0,500,496,497,498,110,0,371,0,0,0,0,234,0,373,374,25,27,0,
0,0,0,0,0,0,0,146,0,0,0,0,0,0,0,194,195,2,0,0,0,0,0,29,0,118,119,121,41,49,43,45,
114,504,505,110,110,36,38,36,0,19,0,213,0,169,157,182,191,190,0,0,188,189,192,
193,202,184,0,69,0,54,102,0,100,0,500,381,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,486,
0,0,0,0,0,473,471,472,0,398,400,414,422,424,426,428,430,432,435,440,443,446,450,
0,452,474,485,484,0,382,380,32,0,0,110,0,0,0,128,124,126,252,254,0,0,9,10,0,0,0,
110,506,507,212,93,0,0,161,72,232,0,229,0,3,0,5,0,282,0,30,0,0,36,20,0,115,116,0,
109,0,143,0,0,0,0,0,0,0,0,0,0,0,0,500,285,287,0,293,289,0,291,275,276,277,0,278,
279,280,0,39,21,121,259,0,219,235,0,0,215,213,0,199,0,0,0,204,57,203,185,0,0,103,
99,0,0,375,0,0,395,0,0,0,0,248,391,396,0,393,0,460,0,416,450,0,417,418,0,0,0,0,0,
0,0,0,0,453,454,36,0,0,0,456,457,455,0,372,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,404,405,406,407,408,409,410,411,412,413,403,0,458,0,480,481,0,0,0,488,0,110,
388,389,0,386,0,236,0,0,0,0,255,227,0,129,134,130,132,125,127,213,0,261,253,262,
509,508,0,86,90,0,88,0,92,108,63,62,0,60,162,213,231,158,261,233,170,171,0,85,
198,26,0,28,0,0,0,0,120,122,238,237,22,105,117,0,0,0,136,137,139,0,110,110,151,0,
0,0,0,0,0,36,0,322,323,324,0,0,326,0,0,0,0,294,290,121,292,288,286,0,168,220,0,0,
0,226,214,221,165,0,0,0,215,167,201,200,163,199,0,0,205,58,111,0,104,421,101,97,
384,385,251,0,392,247,249,379,0,0,0,0,239,243,0,0,0,0,0,0,0,0,0,0,0,0,0,487,494,
0,493,399,423,0,425,427,429,431,433,434,438,439,436,437,441,442,444,445,447,448,
449,402,401,479,476,0,478,0,0,0,376,0,383,31,0,377,0,0,256,135,131,133,0,215,0,
199,0,263,0,213,0,274,258,0,0,110,0,0,0,128,0,110,0,213,0,180,159,230,4,6,0,34,0,
0,140,123,0,0,215,144,153,152,0,0,147,0,0,0,301,0,0,0,0,0,321,325,0,0,0,0,284,36,
23,260,213,0,223,0,0,216,0,166,219,207,164,187,186,205,183,0,7,0,250,394,397,459,
0,0,240,0,244,463,0,0,0,0,467,470,0,0,0,0,0,451,490,0,0,477,475,0,0,387,390,378,
257,228,270,0,264,265,199,0,0,215,199,0,89,0,215,0,0,0,59,61,0,172,0,0,215,0,199,
0,0,0,138,141,114,142,154,151,151,0,0,0,0,0,0,0,0,0,0,0,0,0,301,327,0,0,24,215,0,
224,222,0,215,0,206,0,8,0,0,0,241,245,0,0,0,0,419,420,479,478,492,0,495,415,489,
491,0,269,267,273,272,0,266,199,0,114,0,66,64,65,173,179,176,0,178,174,199,0,33,
0,0,150,149,295,301,0,0,0,0,0,0,329,330,332,334,336,338,340,342,345,350,353,356,
360,362,369,370,0,36,298,307,0,0,0,0,0,0,0,0,0,0,328,281,209,225,217,208,213,13,
0,0,462,461,0,246,464,0,0,469,468,482,0,271,268,35,0,0,177,175,283,107,0,0,0,301,
0,366,0,0,363,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,364,302,0,309,0,0,0,317,
0,0,0,0,0,0,300,0,215,12,14,0,0,0,0,483,106,0,155,296,297,0,0,0,0,301,303,333,0,
335,337,339,341,343,344,348,349,346,347,351,352,354,355,357,358,359,0,308,310,
311,0,319,318,0,313,0,0,0,218,210,15,0,0,0,0,299,365,0,361,304,0,36,312,320,314,
315,0,242,465,0,67,0,0,331,301,305,316,0,368,367,306,466,0,0,0};static short Cyc_yydefgoto[
143]={1024,50,682,872,51,52,284,53,455,54,457,55,56,140,57,58,523,225,441,442,
226,61,240,227,63,163,164,64,160,65,261,262,125,126,127,263,228,422,470,471,472,
66,67,643,644,645,68,473,69,446,70,71,157,158,72,116,519,313,679,603,73,604,513,
670,505,509,510,417,306,248,93,94,544,463,545,328,329,330,229,299,300,605,428,
287,459,288,289,290,291,292,757,293,294,833,834,835,836,837,838,839,840,841,842,
843,844,845,846,847,848,331,407,408,332,333,334,295,196,394,197,529,198,199,200,
201,202,203,204,205,206,207,208,209,210,211,212,560,561,213,214,75,873,241,432};
static short Cyc_yypact[1027]={1197,- -32768,- -32768,- -32768,- -32768,29,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,3236,215,3637,- -32768,154,62,- -32768,137,153,166,
175,172,212,223,15,306,- -32768,- -32768,276,- -32768,- -32768,- -32768,266,889,305,
294,318,318,- -32768,- -32768,2625,- -32768,493,597,- -32768,391,731,3236,3236,3236,
- -32768,3236,- -32768,- -32768,643,- -32768,154,3157,241,131,737,670,- -32768,-
-32768,327,371,387,- -32768,154,398,- -32768,4675,272,- -32768,- -32768,- -32768,327,
5300,- -32768,394,413,4675,421,437,440,- -32768,193,- -32768,- -32768,3309,3309,2625,
2625,3309,475,5300,747,- -32768,280,448,267,468,280,3528,5300,- -32768,- -32768,-
-32768,2625,2758,2625,2758,50,- -32768,486,506,- -32768,3078,- -32768,- -32768,-
-32768,- -32768,3528,- -32768,- -32768,327,191,1658,- -32768,3157,731,- -32768,3309,
1506,4036,- -32768,241,- -32768,- -32768,- -32768,525,532,- -32768,- -32768,- -32768,-
-32768,113,670,3309,- -32768,2758,- -32768,520,543,549,154,114,- -32768,4675,130,
1235,5476,571,5300,5341,578,601,603,619,631,649,652,660,5567,5567,- -32768,2213,
662,5608,5608,5608,- -32768,- -32768,- -32768,72,- -32768,- -32768,- 35,594,659,674,
661,629,140,678,81,155,- -32768,1075,5608,194,40,- -32768,697,874,711,- -32768,-
-32768,720,5300,327,874,713,226,3528,3601,3528,702,- -32768,- 68,- 68,- -32768,-
-32768,95,717,124,327,- -32768,- -32768,- -32768,- -32768,58,718,- -32768,- -32768,664,
319,- -32768,739,- -32768,772,- -32768,783,735,754,- -32768,267,4077,3157,- -32768,
795,3528,- -32768,598,805,154,806,803,99,804,3669,811,823,819,821,4172,3669,261,
815,812,- -32768,- -32768,820,1798,1798,731,1798,- -32768,- -32768,- -32768,826,-
-32768,- -32768,- -32768,20,- -32768,- -32768,825,813,32,840,- -32768,24,824,827,299,
829,716,830,3309,5300,- -32768,841,- -32768,- -32768,32,837,154,- -32768,5300,843,-
-32768,845,846,- -32768,280,5300,4675,240,- -32768,- -32768,- -32768,851,852,2213,-
-32768,3528,- -32768,- -32768,3764,- -32768,870,5300,5300,5300,5300,5300,5300,3528,
491,2213,- -32768,- -32768,1938,839,337,5300,- -32768,- -32768,- -32768,5300,- -32768,
5608,5300,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,
5608,5608,5608,5300,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,5300,- -32768,280,- -32768,- -32768,4213,280,5300,-
-32768,854,327,- -32768,- -32768,848,860,4675,- -32768,389,1235,858,3309,- -32768,
866,871,- -32768,3601,3601,3601,- -32768,- -32768,2918,4305,285,- -32768,419,- -32768,
- -32768,24,- -32768,- -32768,3309,- -32768,864,- -32768,873,- -32768,865,877,875,-
-32768,2837,- -32768,574,699,- -32768,- -32768,- -32768,3528,- -32768,- -32768,- -32768,
2625,- -32768,2625,892,891,888,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,676,5300,897,895,- -32768,890,531,327,327,844,5300,5300,896,898,5300,988,
2078,907,- -32768,- -32768,- -32768,428,982,- -32768,4346,5300,3669,2348,- -32768,-
-32768,3078,- -32768,- -32768,- -32768,3309,- -32768,- -32768,3528,904,293,- -32768,-
-32768,894,- -32768,24,906,3455,827,- -32768,- -32768,- -32768,- -32768,716,89,908,
784,- -32768,- -32768,2491,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
901,- -32768,- -32768,- -32768,- -32768,5725,909,915,929,- -32768,- -32768,705,4077,
339,917,919,920,361,363,916,537,918,923,5435,- -32768,- -32768,922,924,- -32768,594,
177,659,674,661,629,140,140,678,678,678,678,81,81,155,155,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,927,- -32768,149,3309,3941,- -32768,5390,-
-32768,- -32768,926,- -32768,115,941,- -32768,- -32768,- -32768,- -32768,931,827,936,
716,932,419,3309,3382,4438,- -32768,- -32768,- 68,933,327,769,939,58,2999,940,327,
3309,1506,4479,- -32768,574,- -32768,- -32768,- -32768,942,- -32768,960,818,- -32768,-
-32768,598,5300,827,- -32768,- -32768,- -32768,949,154,487,377,380,5300,849,383,945,
4574,4615,561,- -32768,- -32768,952,954,948,397,- -32768,3157,- -32768,813,957,3309,
- -32768,958,24,- -32768,955,- -32768,202,- -32768,- -32768,- -32768,- -32768,784,-
-32768,961,- -32768,2625,- -32768,- -32768,- -32768,962,364,963,- -32768,3805,- -32768,
- -32768,5300,1056,5300,5341,- -32768,- -32768,280,280,962,959,3941,- -32768,- -32768,
5300,5300,- -32768,- -32768,32,767,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
964,- -32768,- -32768,716,32,965,827,716,956,- -32768,5300,827,399,966,967,- -32768,
- -32768,968,- -32768,32,970,827,989,716,985,5300,969,- -32768,- -32768,3528,- -32768,
993,91,844,3669,1000,994,1023,995,1002,3669,5300,4707,562,4799,583,4891,849,-
-32768,971,1005,- -32768,827,116,- -32768,- -32768,1012,827,3528,- -32768,510,-
-32768,1001,1004,5300,- -32768,- -32768,4077,408,1003,1015,- -32768,870,1011,1014,-
-32768,774,- -32768,- -32768,- -32768,- -32768,3941,- -32768,- -32768,- -32768,- -32768,
1017,- -32768,716,589,3528,975,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
1018,- -32768,- -32768,716,427,- -32768,1021,3528,- -32768,- -32768,1115,849,1027,
5699,1024,2348,5608,1026,- -32768,77,- -32768,1064,1028,751,807,178,814,197,291,-
-32768,- -32768,- -32768,- -32768,1065,5608,1938,- -32768,- -32768,431,3669,460,4932,
3669,507,5024,5116,590,1049,- -32768,- -32768,- -32768,- -32768,1059,- -32768,957,-
-32768,1061,634,- -32768,- -32768,211,- -32768,- -32768,3528,1155,- -32768,- -32768,-
-32768,3900,- -32768,- -32768,- -32768,1063,1066,- -32768,- -32768,- -32768,- -32768,
516,3669,1068,849,2213,- -32768,3528,1060,- -32768,1370,5608,5300,5608,5608,5608,
5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5300,-
-32768,- -32768,1067,- -32768,3669,3669,523,- -32768,3669,3669,527,3669,533,5208,-
-32768,24,827,- -32768,- -32768,747,5300,1069,1071,- -32768,- -32768,1080,- -32768,-
-32768,- -32768,1070,1072,1073,5608,849,- -32768,594,234,659,674,674,629,140,140,
678,678,678,678,81,81,155,155,- -32768,- -32768,- -32768,239,- -32768,- -32768,-
-32768,3669,- -32768,- -32768,3669,- -32768,3669,3669,540,- -32768,- -32768,- -32768,
791,1095,3528,1096,- -32768,962,366,- -32768,- -32768,5300,1370,- -32768,- -32768,-
-32768,- -32768,3669,- -32768,- -32768,1098,- -32768,1099,1101,- -32768,849,- -32768,-
-32768,1102,- -32768,- -32768,- -32768,- -32768,1189,1190,- -32768};static short Cyc_yypgoto[
143]={- -32768,150,- -32768,248,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- 51,- -32768,- 96,22,- -32768,- -32768,0,608,- -32768,55,- 174,1117,5,- -32768,-
-32768,- 128,- -32768,120,1193,- 707,- -32768,- -32768,- -32768,974,973,542,463,-
-32768,- -32768,595,- -32768,- -32768,167,- -32768,- -32768,48,- 193,1163,- 274,- 61,-
-32768,1081,- -32768,- -32768,1195,- 374,- -32768,560,- 124,- 133,- 143,119,302,575,580,
- 370,- 412,- 108,- 405,- 110,- -32768,- 231,- 171,- 497,- 254,- -32768,921,- 157,- 89,- 112,-
94,- 377,- 167,- -32768,- -32768,- -32768,- 29,- 271,- -32768,- 715,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 4,1025,- -32768,712,847,- -32768,
214,534,- -32768,- 172,- 290,- 144,- 330,- 336,- 341,885,- 320,- 347,- 242,- 334,- 306,- 182,
730,498,910,- -32768,- 305,- -32768,181,372,- 62,9,- 247,49};static short Cyc_yytable[
5863]={59,305,338,135,341,62,247,358,359,360,148,230,231,496,497,234,499,139,429,
95,304,59,569,570,314,566,62,462,395,565,355,342,259,563,307,19,20,534,321,142,
822,575,576,78,111,135,628,316,568,608,863,59,420,445,430,60,62,363,59,59,59,613,
59,62,62,62,431,62,59,146,608,577,578,62,136,439,60,139,168,533,129,130,131,556,
132,506,546,285,217,296,141,416,112,711,586,668,641,642,364,260,665,888,59,59,
483,128,60,62,62,628,490,297,896,60,60,60,237,60,429,59,59,59,59,60,62,62,62,62,
59,571,572,573,574,62,427,39,501,76,59,361,59,507,671,62,402,62,675,146,528,584,
141,502,448,587,430,528,254,60,60,243,286,59,141,515,440,322,62,426,99,904,508,
255,403,627,60,60,60,60,540,635,541,429,953,60,450,447,716,659,958,298,527,361,
553,60,555,60,362,579,580,581,627,118,450,474,96,38,794,171,296,- 148,905,406,41,
676,434,377,60,430,406,378,169,521,479,431,39,480,419,371,372,431,465,421,720,
285,285,170,285,38,312,715,867,323,1001,361,41,420,420,420,437,431,431,621,232,
233,44,324,265,267,100,59,38,373,374,96,62,912,913,41,361,250,251,252,253,96,101,
621,396,397,398,709,433,141,102,435,59,59,379,59,1017,62,62,103,62,104,380,381,
361,528,508,914,915,1022,195,285,707,298,286,286,504,286,221,317,159,524,60,691,
222,399,303,266,236,38,400,401,535,597,- 213,661,41,- 213,361,918,79,105,498,919,
546,39,944,60,60,412,60,754,415,614,106,801,746,503,96,805,96,361,59,108,43,438,
361,62,536,1002,143,426,144,525,1003,818,326,327,145,870,74,38,416,704,286,491,
169,508,41,109,622,447,492,450,238,247,- 235,110,44,- 235,239,170,80,97,96,98,47,
48,49,356,662,96,39,593,450,474,424,60,113,667,450,664,425,303,74,114,514,419,
419,419,920,74,421,421,421,735,612,74,921,922,886,653,411,115,785,134,451,137,
781,735,74,452,620,891,626,139,159,546,123,165,791,792,97,59,361,59,361,784,62,
558,62,692,97,416,528,724,634,663,238,780,238,1013,74,74,239,710,239,134,361,739,
361,74,59,696,723,697,161,62,489,74,74,74,74,162,361,722,59,361,738,752,361,62,
753,594,141,758,361,96,60,737,60,74,740,166,361,215,809,216,141,768,770,810,590,
789,59,361,678,81,775,62,878,74,43,796,508,218,165,60,609,97,610,97,656,785,361,
361,611,356,361,892,790,60,877,926,219,547,622,474,220,551,552,965,966,356,962,
963,784,671,961,242,450,474,959,450,474,771,564,361,925,60,971,972,928,826,235,
97,713,964,96,853,244,96,97,639,640,38,84,186,85,86,256,750,41,629,751,630,612,
119,120,296,973,974,351,588,38,188,449,699,397,398,40,41,42,257,361,626,871,74,
785,932,957,769,674,451,449,38,476,318,950,481,361,40,41,42,361,982,310,249,902,
985,361,784,468,311,246,987,74,361,399,319,59,727,1008,700,401,62,924,320,967,
968,969,970,901,264,681,764,857,309,361,361,59,678,141,365,246,62,927,43,165,931,
646,647,337,623,650,624,654,860,97,343,361,625,38,887,937,660,361,361,40,41,42,
121,122,894,135,60,369,370,96,468,945,718,344,469,345,940,951,47,48,49,1016,149,
150,151,729,60,975,976,977,954,346,955,152,153,154,155,156,38,942,849,1014,943,
347,40,41,42,747,797,133,980,981,375,376,983,984,418,986,423,38,802,348,96,1000,
349,40,41,42,517,518,97,38,350,97,357,814,444,40,41,42,43,464,425,366,47,48,49,
368,468,285,16,17,18,264,38,508,47,48,49,367,40,41,42,404,1004,449,409,1005,449,
1006,1007,444,689,690,424,410,74,425,74,1015,425,779,47,48,49,414,38,1011,436,
449,1018,443,804,41,522,449,124,808,641,642,59,285,38,44,458,62,453,816,40,41,42,
47,48,49,460,761,763,47,48,49,755,756,286,464,730,731,732,548,549,550,135,600,
601,602,798,799,908,909,866,454,559,883,884,869,562,910,911,74,916,917,456,336,
59,339,339,60,786,62,1009,361,97,81,466,352,353,582,824,825,339,339,339,475,477,
38,286,478,482,583,485,40,41,42,559,484,486,96,487,493,494,339,468,807,495,500,
258,504,431,511,47,48,49,285,516,512,526,43,820,60,607,520,530,363,557,97,82,246,
531,532,538,615,539,591,854,856,589,859,405,862,592,167,84,596,85,86,598,246,41,
617,599,616,87,449,246,619,38,876,88,631,89,90,618,41,59,43,91,449,632,62,449,
633,92,44,636,637,649,749,638,651,648,47,48,49,655,657,286,559,666,669,559,672,
683,677,686,687,688,693,246,694,695,698,702,701,706,714,195,705,246,829,830,339,
708,39,728,74,717,246,339,60,991,719,81,721,733,736,743,744,748,759,765,766,930,
767,303,934,936,772,774,778,703,464,787,889,793,782,821,806,864,800,803,811,812,
813,339,815,339,339,339,339,339,339,339,339,339,339,339,339,339,339,339,339,339,
356,817,82,819,823,827,828,960,852,865,851,868,464,874,879,97,875,167,84,382,85,
86,880,881,41,978,882,893,87,885,890,895,897,726,831,900,89,90,134,989,246,903,
91,906,923,832,993,742,92,192,418,907,193,194,246,938,383,384,385,386,387,388,
389,390,391,392,939,941,946,948,559,979,956,949,952,996,997,1025,1026,992,999,
994,995,393,998,- 11,1,339,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,1010,
1012,21,1019,1020,464,1021,1023,734,107,788,245,461,745,147,22,467,23,464,777,
315,795,990,24,117,25,26,27,413,776,773,537,685,567,850,28,29,- 36,0,0,595,554,0,
0,30,31,32,33,0,0,34,0,0,0,81,0,0,35,36,37,0,0,0,0,0,0,0,0,339,0,264,0,0,0,0,0,0,
0,0,0,0,0,0,0,38,0,0,0,0,39,40,41,42,43,0,0,134,0,0,246,464,44,0,45,46,0,0,82,0,
47,48,49,0,0,464,0,0,0,0,325,0,0,167,84,0,85,86,0,0,41,0,264,0,87,0,0,0,0,0,88,0,
89,90,0,326,327,246,91,0,339,0,0,0,92,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,755,756,21,172,173,268,0,269,270,271,272,273,274,275,276,22,81,23,174,
277,0,0,175,24,278,0,464,27,0,0,176,177,178,179,339,28,29,0,180,0,279,0,0,0,0,
339,32,33,181,182,34,183,0,0,0,0,0,35,36,37,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,280,84,186,85,86,39,40,41,42,43,281,138,- 301,0,0,0,0,187,0,45,
283,0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,27,0,0,0,899,0,0,
339,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,339,0,0,0,0,35,36,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,301,0,0,302,0,0,0,0,39,40,0,42,43,303,0,0,0,0,0,0,0,0,45,
46,0,339,0,339,339,339,339,339,339,339,339,339,339,339,339,339,339,339,339,339,0,
0,0,0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,
268,339,269,270,271,272,273,274,275,276,22,81,23,174,277,0,0,175,24,278,0,0,27,0,
0,176,177,178,179,0,28,29,0,180,0,279,0,0,0,0,0,32,33,181,182,34,183,339,0,0,0,0,
35,36,37,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,84,186,85,86,39,
40,41,42,43,281,138,282,0,0,0,0,187,0,45,283,0,0,0,0,189,0,0,190,0,0,191,192,0,0,
193,194,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,268,0,
269,270,271,272,273,274,275,276,22,81,23,174,277,0,0,175,24,278,0,0,27,0,0,176,
177,178,179,0,28,29,- 36,180,0,279,0,0,0,0,0,32,33,181,182,34,183,0,0,0,0,0,35,36,
37,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,84,186,85,86,39,40,41,
42,43,281,138,0,0,0,0,0,187,0,45,283,0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,
2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,268,0,269,270,
271,272,273,274,275,276,22,81,23,174,277,0,0,175,24,278,0,0,27,0,0,176,177,178,
179,0,28,29,0,180,0,279,0,0,0,0,0,32,33,181,182,34,183,0,0,0,0,0,35,36,37,0,184,
185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,84,186,85,86,39,40,41,42,43,
281,138,0,0,0,0,0,187,0,45,283,0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,2,3,4,
77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,0,0,0,0,0,0,0,0,0,0,
22,81,23,174,0,0,0,175,24,0,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,0,0,0,0,0,
0,32,33,181,182,34,183,0,0,0,0,0,35,36,37,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,38,84,186,85,86,39,40,41,42,43,652,0,0,0,0,0,0,187,0,45,283,0,0,0,
0,189,0,0,190,0,0,191,192,0,0,193,194,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
0,172,173,0,0,0,0,0,0,0,0,0,0,22,81,0,174,0,0,0,175,0,0,0,0,27,0,0,176,177,178,
179,0,28,29,0,180,0,0,0,0,0,0,0,32,0,181,182,34,183,0,0,0,0,0,35,36,0,0,184,185,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,39,40,41,42,43,0,354,0,
0,0,0,0,187,0,45,283,0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,0,172,173,0,0,0,0,0,0,0,0,0,0,22,81,0,174,0,0,0,175,
0,0,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,0,0,0,0,0,0,32,0,181,182,34,183,0,
0,0,0,0,35,36,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,
86,39,40,41,42,43,0,0,0,0,0,0,0,187,0,45,283,0,0,0,0,189,0,0,190,0,0,191,192,0,0,
193,194,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,
0,0,0,0,0,0,0,0,22,0,23,0,0,0,680,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,
0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,38,0,0,0,0,39,40,41,42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,
49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,
0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,30,
31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,38,0,0,0,0,39,40,41,42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,
0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,30,31,32,33,0,0,
34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,
0,38,0,0,0,0,39,40,41,42,43,22,0,- 11,0,0,0,0,44,0,45,46,0,27,0,0,47,48,49,0,0,28,
29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,0,0,0,0,0,0,0,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,301,0,0,38,0,0,0,0,39,40,41,42,43,303,0,22,0,0,0,0,
444,0,45,46,0,0,425,27,47,48,49,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,
0,0,0,0,35,36,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,301,0,0,0,0,
0,0,0,39,40,0,42,43,303,0,22,0,0,0,0,424,0,45,46,0,0,425,27,47,48,49,0,0,0,0,28,
29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,2,3,4,77,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,0,0,38,0,0,0,0,39,40,41,42,43,22,0,23,0,0,0,0,468,
24,45,46,0,27,0,0,47,48,49,0,0,28,29,- 36,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,
0,35,36,37,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,
0,39,40,0,42,43,22,138,23,0,258,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,
0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,2,3,4,77,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,138,0,0,0,0,0,0,24,45,46,0,
27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,39,40,0,42,43,0,0,
0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,223,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,
36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,39,40,
0,42,43,0,224,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,
0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,301,0,0,0,0,
0,0,22,39,40,0,42,43,303,0,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,
0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,
0,673,0,0,0,0,0,0,22,39,40,0,42,43,0,0,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,0,0,
0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,39,40,0,42,43,0,0,0,0,0,0,27,0,0,45,46,0,0,0,28,
29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,81,0,0,0,0,35,36,0,0,0,0,0,0,0,0,0,0,0,0,172,
173,268,0,269,270,271,272,273,274,275,276,0,81,0,174,277,0,39,175,0,278,43,0,0,0,
0,176,177,178,179,0,45,46,82,180,0,279,0,0,0,0,0,0,0,181,182,0,183,83,84,0,85,86,
0,0,41,0,184,185,87,0,0,0,0,0,88,0,89,90,0,0,0,0,91,0,0,0,0,0,92,280,84,186,85,
86,0,0,41,0,0,281,138,0,0,172,173,0,187,0,0,188,542,0,0,0,189,0,81,190,174,0,191,
192,175,0,193,194,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,172,173,0,0,181,
182,0,183,0,0,0,0,0,81,0,174,0,184,185,175,0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,
180,0,0,0,0,38,84,186,85,86,181,182,41,183,0,0,340,543,0,0,0,0,187,184,185,188,0,
326,327,0,189,0,0,190,0,0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,
0,340,783,0,172,173,0,187,0,0,188,0,326,327,0,189,0,81,190,174,0,191,192,175,0,
193,194,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,172,173,0,0,181,182,0,183,
0,0,0,0,0,81,0,174,0,184,185,175,0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,
0,38,84,186,85,86,181,182,41,183,0,0,340,947,0,0,0,0,187,184,185,188,0,326,327,0,
189,0,0,190,0,0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,0,340,0,0,
172,173,0,187,0,0,188,0,326,327,0,189,0,81,190,174,0,191,192,175,0,193,194,0,0,0,
0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,172,173,0,0,181,182,0,183,0,0,0,0,0,81,0,
174,0,184,185,175,0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,38,84,186,85,
86,181,182,41,183,0,0,0,0,0,0,0,0,187,184,185,188,0,0,0,308,189,0,0,190,0,0,191,
192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,0,340,0,0,172,173,0,187,0,0,
188,0,0,0,0,189,0,81,190,174,0,191,192,175,0,193,194,0,0,0,0,176,177,178,179,0,0,
0,0,180,0,0,0,0,0,172,173,0,0,181,182,0,183,0,0,0,0,0,81,0,174,0,184,185,175,0,0,
0,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,38,84,186,85,86,181,182,41,183,0,
488,0,0,0,0,0,0,187,184,185,188,0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,0,0,
0,0,38,84,186,85,86,0,0,41,0,0,0,172,173,0,0,0,0,187,585,0,188,0,0,0,81,189,174,
0,190,0,175,191,192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,172,173,
0,0,181,182,0,183,0,0,0,0,0,81,0,174,0,184,185,175,0,0,0,0,0,0,0,176,177,178,179,
0,0,0,0,180,0,0,0,0,38,84,186,85,86,181,182,41,183,0,0,0,0,0,0,0,0,187,184,185,
188,0,0,0,606,189,0,0,190,0,0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,
0,0,0,172,173,0,0,0,0,187,658,0,188,0,0,0,81,189,174,0,190,0,175,191,192,0,0,193,
194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,172,173,0,0,181,182,0,183,0,0,0,0,0,
81,0,174,0,184,185,175,0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,38,84,
186,85,86,181,182,41,183,0,0,0,0,0,0,0,0,187,184,185,188,0,0,0,725,189,0,0,190,0,
0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,0,0,0,0,172,173,0,187,0,
0,188,0,0,0,741,189,0,81,190,174,0,191,192,175,0,193,194,0,0,0,0,176,177,178,179,
0,0,0,0,180,0,0,0,0,0,172,173,0,0,181,182,0,183,0,0,0,0,0,81,0,174,0,184,185,175,
0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,38,84,186,85,86,181,182,41,183,
0,760,0,0,0,0,0,0,187,184,185,188,0,0,0,0,189,0,0,190,0,0,191,192,81,0,193,194,0,
0,0,0,38,84,186,85,86,0,0,41,0,0,762,172,173,0,0,0,0,187,0,0,188,0,0,0,81,189,
174,0,190,0,175,191,192,0,0,193,194,0,176,177,178,179,0,0,0,82,180,0,0,0,0,0,0,0,
0,0,181,182,0,183,167,84,0,85,86,0,0,41,0,184,185,87,0,0,0,0,0,88,0,89,90,0,0,0,
0,91,0,0,0,0,0,92,38,84,186,85,86,0,0,41,0,0,0,172,173,0,0,0,0,187,855,0,188,0,0,
0,81,189,174,0,190,0,175,191,192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,
0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,0,172,173,0,0,0,0,187,858,0,188,0,0,0,
81,189,174,0,190,0,175,191,192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,
0,172,173,0,0,181,182,0,183,0,0,0,0,0,81,0,174,0,184,185,175,0,0,0,0,0,0,0,176,
177,178,179,0,0,0,0,180,0,0,0,0,38,84,186,85,86,181,182,41,183,0,861,0,0,0,0,0,0,
187,184,185,188,0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,
86,0,0,41,0,0,0,172,173,0,0,0,0,187,929,0,188,0,0,0,81,189,174,0,190,0,175,191,
192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,
0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,
86,0,0,41,0,0,0,172,173,0,0,0,0,187,933,0,188,0,0,0,81,189,174,0,190,0,175,191,
192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,
0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,
86,0,0,41,0,0,0,172,173,0,0,0,0,187,935,0,188,0,0,0,81,189,174,0,190,0,175,191,
192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,
0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,
86,0,0,41,0,0,0,172,173,0,0,0,0,187,988,0,188,0,0,0,81,189,174,0,190,0,175,191,
192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,172,173,0,0,181,182,0,
183,0,0,0,0,0,81,0,0,0,184,185,0,0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,0,0,0,0,0,
38,84,186,85,86,181,182,41,183,0,0,0,0,0,0,0,0,187,184,185,188,0,81,0,0,189,0,0,
190,0,0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,0,340,0,172,173,0,
0,187,0,0,188,0,0,0,0,189,81,0,190,0,0,191,192,0,82,193,194,0,0,0,176,177,178,
179,0,0,712,0,0,167,84,0,85,86,172,173,41,0,181,182,87,183,0,0,0,0,88,81,89,90,0,
184,185,0,91,0,0,0,0,0,92,176,177,178,179,0,0,0,0,0,0,0,0,0,38,84,186,85,86,181,
182,41,183,0,0,703,0,0,0,0,0,187,184,185,188,0,0,0,0,189,0,0,190,0,0,191,192,0,0,
193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,172,173,0,0,0,0,0,335,0,0,188,0,0,81,
0,189,0,0,190,0,0,191,192,0,0,193,194,176,177,178,179,0,0,0,0,0,0,0,0,0,0,172,
173,0,0,181,182,0,183,0,0,0,0,0,81,0,0,0,184,185,0,0,0,0,0,0,0,0,176,177,178,179,
0,0,0,0,0,0,0,0,0,38,84,186,85,86,181,182,41,183,0,0,0,0,0,0,0,0,351,184,185,188,
0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,
172,173,0,0,0,0,0,187,0,0,188,0,0,81,0,189,0,0,190,0,0,191,192,0,0,193,194,176,
177,178,179,0,0,0,0,0,0,0,0,81,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,0,82,0,0,0,
0,0,898,0,0,188,0,0,684,0,189,167,84,190,85,86,191,192,41,0,193,194,87,0,0,0,0,0,
88,0,89,90,0,326,327,0,91,0,0,0,0,0,92};static short Cyc_yycheck[5863]={0,144,174,
65,175,0,114,189,190,191,71,100,101,284,285,104,287,68,86,23,144,21,369,370,157,
366,21,258,210,365,187,175,128,363,144,20,21,327,166,68,747,375,376,21,44,107,
451,159,368,426,765,51,226,246,122,0,51,92,58,59,60,431,62,58,59,60,134,62,68,69,
447,377,378,68,65,17,21,128,82,326,58,59,60,354,62,61,340,138,92,140,68,224,44,
590,399,507,5,6,133,128,505,808,102,103,271,57,51,102,103,514,277,140,827,58,59,
60,107,62,86,119,120,121,122,68,119,120,121,122,128,371,372,373,374,128,228,111,
116,108,138,119,140,117,512,138,104,140,520,147,320,396,128,119,246,400,122,327,
106,102,103,111,138,161,140,306,106,169,161,228,106,92,303,121,132,447,119,120,
121,122,335,469,337,86,897,128,246,246,598,492,903,141,318,119,349,138,351,140,
124,379,380,381,474,51,264,264,23,106,703,87,259,118,133,215,113,124,119,134,161,
122,222,138,106,310,123,134,111,126,226,87,88,134,259,226,606,284,285,121,287,
106,125,124,124,111,957,119,113,419,420,421,124,134,134,444,102,103,123,125,136,
137,121,259,106,121,122,82,259,87,88,113,119,119,120,121,122,92,121,468,82,83,84,
130,231,259,116,234,284,285,131,287,1003,284,285,116,287,121,139,140,119,469,431,
121,122,1016,88,354,127,257,284,285,106,287,117,161,121,313,259,546,123,123,116,
128,106,106,128,129,328,414,124,494,113,127,119,134,117,121,286,138,590,111,127,
284,285,221,287,648,118,434,123,721,638,300,169,725,171,119,354,49,115,237,119,
354,120,127,121,424,123,316,127,741,128,129,129,776,0,106,507,557,354,116,106,
512,113,105,444,444,123,447,106,495,116,123,123,119,112,121,22,23,215,25,131,132,
133,187,498,222,111,409,468,468,123,354,105,118,474,502,129,116,44,123,119,419,
420,421,131,51,419,420,421,620,428,57,139,140,806,484,220,117,690,65,119,67,687,
634,70,124,444,819,446,498,121,703,59,79,699,700,82,455,119,457,119,690,455,124,
457,124,92,598,638,610,468,498,106,107,106,107,102,103,112,589,112,107,119,624,
119,111,484,124,610,124,117,484,276,119,120,121,122,108,119,609,498,119,624,124,
119,498,124,116,484,124,119,328,455,623,457,141,624,117,119,123,119,106,498,124,
665,124,404,696,526,119,523,38,673,526,124,161,115,707,669,116,166,484,121,169,
123,171,116,799,119,119,129,335,119,124,696,498,785,124,119,343,620,620,120,347,
348,910,911,351,907,908,799,939,906,123,634,634,904,637,637,666,364,119,851,526,
916,917,124,752,111,215,592,909,409,758,124,412,222,475,476,106,107,108,109,110,
116,116,113,455,119,457,608,116,117,662,918,919,123,401,106,126,246,82,83,84,112,
113,114,119,119,627,118,257,884,124,903,662,515,119,264,106,266,119,124,269,119,
112,113,114,119,124,123,115,832,124,119,884,123,123,114,124,286,119,123,118,662,
614,124,128,129,662,850,120,912,913,914,915,831,133,526,116,116,145,119,119,682,
678,662,91,144,682,855,115,318,858,478,479,123,121,482,123,484,116,328,123,119,
129,106,116,116,493,119,119,112,113,114,116,117,823,778,662,89,90,539,123,879,
604,123,127,123,870,895,131,132,133,1003,63,64,65,616,682,920,921,922,898,123,
900,74,75,76,77,78,106,116,755,999,119,123,112,113,114,639,710,117,928,929,85,86,
932,933,225,935,227,106,722,123,592,956,123,112,113,114,63,64,409,106,123,412,
123,737,123,112,113,114,115,258,129,135,131,132,133,137,123,851,17,18,19,262,106,
939,131,132,133,136,112,113,114,117,982,444,106,985,447,987,988,123,118,119,123,
106,455,129,457,1002,129,682,131,132,133,123,106,995,122,468,1008,124,724,113,
311,474,116,729,5,6,851,903,106,123,120,851,118,739,112,113,114,131,132,133,111,
652,653,131,132,133,22,23,851,340,106,107,108,344,345,346,943,419,420,421,118,
119,136,137,770,118,357,118,119,775,361,89,90,526,85,86,118,172,903,174,175,851,
693,903,118,119,539,38,118,184,185,382,750,751,189,190,191,117,117,106,903,123,
123,394,106,112,113,114,399,123,116,755,116,123,127,210,123,728,123,118,120,106,
134,124,131,132,133,1003,124,127,118,115,743,903,425,130,118,92,124,592,91,424,
122,122,118,106,119,124,759,760,121,762,103,764,119,106,107,124,109,110,119,444,
113,123,118,117,117,620,451,119,106,782,123,106,125,126,124,113,1003,115,131,634,
116,1003,637,122,137,123,116,119,117,644,127,30,123,131,132,133,116,42,1003,492,
123,134,495,124,130,124,124,119,106,119,495,119,119,124,118,124,119,118,831,124,
505,25,26,320,124,111,120,682,124,514,327,1003,940,124,38,130,124,124,123,106,
118,123,117,116,857,124,116,860,861,118,122,117,117,546,25,107,124,121,116,130,
116,124,124,124,124,124,363,124,365,366,367,368,369,370,371,372,373,374,375,376,
377,378,379,380,381,898,124,91,130,123,117,124,905,118,116,127,111,590,124,123,
755,124,106,107,56,109,110,119,124,113,923,124,118,117,124,124,28,117,611,123,
123,125,126,778,937,610,127,131,91,91,134,944,625,137,138,620,135,141,142,624,
118,93,94,95,96,97,98,99,100,101,102,119,118,25,118,648,116,124,119,118,107,118,
0,0,943,119,124,123,120,124,0,1,469,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,124,124,24,124,124,690,124,124,619,35,695,113,257,637,70,37,262,39,703,678,
158,706,939,45,48,47,48,49,222,673,669,329,539,367,755,57,58,59,- 1,- 1,412,350,- 1,
- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,38,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,557,-
1,747,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,112,113,114,115,
- 1,- 1,943,- 1,- 1,776,785,123,- 1,125,126,- 1,- 1,91,- 1,131,132,133,- 1,- 1,799,- 1,- 1,-
1,- 1,103,- 1,- 1,106,107,- 1,109,110,- 1,- 1,113,- 1,808,- 1,117,- 1,- 1,- 1,- 1,- 1,123,- 1,
125,126,- 1,128,129,823,131,- 1,638,- 1,- 1,- 1,137,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,
- 1,44,45,46,- 1,884,49,- 1,- 1,52,53,54,55,696,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,707,68,
69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,
118,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,
142,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,829,- 1,- 1,832,57,58,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,850,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,- 1,
111,112,- 1,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,- 1,904,- 1,906,907,908,909,
910,911,912,913,914,915,916,917,918,919,920,921,922,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,
5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,956,29,30,31,32,
33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,
60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,1002,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,
111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,
134,- 1,- 1,137,138,- 1,- 1,141,142,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,
- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,
- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,123,-
1,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,
38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,
- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,
115,116,117,- 1,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,
138,- 1,- 1,141,142,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,
26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,
53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,
80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
106,107,108,109,110,111,112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,
- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,
- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,
72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,- 1,117,- 1,- 1,- 1,- 1,
- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,
115,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,
- 1,- 1,141,142,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,
79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
106,- 1,- 1,- 1,- 1,111,112,113,114,115,- 1,- 1,118,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,-
1,- 1,131,132,133,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,
- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,106,- 1,- 1,- 1,- 1,111,112,113,114,115,- 1,- 1,118,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,-
1,- 1,- 1,131,132,133,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,
- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,-
1,106,- 1,- 1,- 1,- 1,111,112,113,114,115,37,- 1,118,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,49,
- 1,- 1,131,132,133,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,
- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
103,- 1,- 1,106,- 1,- 1,- 1,- 1,111,112,113,114,115,116,- 1,37,- 1,- 1,- 1,- 1,123,- 1,125,
126,- 1,- 1,129,49,131,132,133,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,
- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,115,116,- 1,37,- 1,- 1,- 1,
- 1,123,- 1,125,126,- 1,- 1,129,49,131,132,133,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,106,- 1,- 1,- 1,- 1,111,112,113,114,115,37,- 1,39,
- 1,- 1,- 1,- 1,123,45,125,126,- 1,49,- 1,- 1,131,132,133,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,115,37,117,39,
- 1,120,- 1,- 1,- 1,45,125,126,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,115,37,117,- 1,- 1,
- 1,- 1,- 1,- 1,45,125,126,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,111,112,- 1,114,115,- 1,- 1,- 1,-
1,- 1,- 1,49,- 1,- 1,125,126,- 1,- 1,- 1,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,
72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,111,112,- 1,114,115,- 1,117,- 1,- 1,- 1,- 1,49,- 1,-
1,125,126,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,-
1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,-
1,- 1,- 1,- 1,- 1,- 1,37,111,112,- 1,114,115,116,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,125,126,- 1,- 1,
- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,-
1,37,111,112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,125,126,- 1,- 1,- 1,57,58,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,111,112,- 1,
114,115,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,125,126,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,68,- 1,- 1,- 1,72,- 1,38,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,111,44,- 1,46,115,- 1,- 1,- 1,
- 1,52,53,54,55,- 1,125,126,91,60,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,106,107,-
1,109,110,- 1,- 1,113,- 1,83,84,117,- 1,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,-
1,- 1,- 1,- 1,- 1,137,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,117,- 1,- 1,25,26,- 1,123,
- 1,- 1,126,32,- 1,- 1,- 1,131,- 1,38,134,40,- 1,137,138,44,- 1,141,142,- 1,- 1,- 1,- 1,52,
53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,
- 1,40,- 1,83,84,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,
106,107,108,109,110,70,71,113,73,- 1,- 1,117,118,- 1,- 1,- 1,- 1,123,83,84,126,- 1,128,
129,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,
- 1,- 1,113,- 1,- 1,- 1,117,118,- 1,25,26,- 1,123,- 1,- 1,126,- 1,128,129,- 1,131,- 1,38,134,
40,- 1,137,138,44,- 1,141,142,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,-
1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,83,84,44,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,113,73,- 1,- 1,
117,118,- 1,- 1,- 1,- 1,123,83,84,126,- 1,128,129,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,-
1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,117,- 1,- 1,25,26,- 1,
123,- 1,- 1,126,- 1,128,129,- 1,131,- 1,38,134,40,- 1,137,138,44,- 1,141,142,- 1,- 1,- 1,-
1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,
- 1,38,- 1,40,- 1,83,84,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,
- 1,106,107,108,109,110,70,71,113,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,- 1,
- 1,130,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,
- 1,- 1,113,- 1,- 1,- 1,117,- 1,- 1,25,26,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,38,134,40,
- 1,137,138,44,- 1,141,142,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,
25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,83,84,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,113,73,- 1,116,-
1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,
142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,
- 1,126,- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,- 1,141,142,- 1,52,53,54,55,- 1,-
1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,83,
84,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,106,107,108,
109,110,70,71,113,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,- 1,- 1,130,131,- 1,-
1,134,- 1,- 1,137,138,- 1,- 1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,-
1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,- 1,126,- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,
- 1,141,142,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,
- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,83,84,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,
60,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,113,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,83,
84,126,- 1,- 1,- 1,130,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,- 1,- 1,- 1,- 1,106,
107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,123,- 1,- 1,126,- 1,- 1,- 1,130,
131,- 1,38,134,40,- 1,137,138,44,- 1,141,142,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,83,84,44,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,
113,73,- 1,116,- 1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,
137,138,38,- 1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,25,26,
- 1,- 1,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,- 1,141,142,
- 1,52,53,54,55,- 1,- 1,- 1,91,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,106,107,- 1,
109,110,- 1,- 1,113,- 1,83,84,117,- 1,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,- 1,
- 1,- 1,- 1,- 1,137,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,
- 1,126,- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,- 1,141,142,- 1,52,53,54,55,- 1,-
1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,
84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,
109,110,- 1,- 1,113,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,- 1,126,- 1,- 1,- 1,38,131,40,-
1,134,- 1,44,137,138,- 1,- 1,141,142,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,
25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,83,84,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,113,73,- 1,116,-
1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,
142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,
- 1,126,- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,- 1,141,142,- 1,52,53,54,55,- 1,-
1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,
84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,
109,110,- 1,- 1,113,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,- 1,126,- 1,- 1,- 1,38,131,40,-
1,134,- 1,44,137,138,- 1,- 1,141,142,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,25,
26,- 1,- 1,- 1,- 1,123,124,- 1,126,- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,- 1,141,
142,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,- 1,126,
- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,- 1,141,142,- 1,52,53,54,55,- 1,- 1,- 1,- 1,
60,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,83,84,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,70,
71,113,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,38,- 1,- 1,131,- 1,- 1,134,- 1,- 1,
137,138,- 1,- 1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,117,- 1,
25,26,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,38,- 1,134,- 1,- 1,137,138,- 1,91,141,142,
- 1,- 1,- 1,52,53,54,55,- 1,- 1,103,- 1,- 1,106,107,- 1,109,110,25,26,113,- 1,70,71,117,
73,- 1,- 1,- 1,- 1,123,38,125,126,- 1,83,84,- 1,131,- 1,- 1,- 1,- 1,- 1,137,52,53,54,55,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,113,73,- 1,- 1,117,- 1,- 1,- 1,- 1,-
1,123,83,84,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,- 1,- 1,- 1,-
1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,126,- 1,- 1,
38,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,113,73,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,
- 1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,
123,- 1,- 1,126,- 1,- 1,38,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,52,53,54,55,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,
109,110,- 1,- 1,113,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,126,- 1,- 1,103,- 1,131,106,
107,134,109,110,137,138,113,- 1,141,142,117,- 1,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,128,
129,- 1,131,- 1,- 1,- 1,- 1,- 1,137};char Cyc_Yystack_overflow[17]="Yystack_overflow";
struct Cyc_Yystack_overflow_struct{char*tag;int f1;};struct Cyc_Yystack_overflow_struct
Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};void Cyc_yyerror(struct
_dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(int)'\000';
union Cyc_YYSTYPE Cyc_yylval={.YYINITIALSVAL={60,0}};static int Cyc_yynerrs=0;struct
_tuple24{struct Cyc_Position_Segment*f1;struct _tuple1*f2;int f3;};struct _tuple25{
struct _dyneither_ptr f1;void*f2;};static char _tmp519[8]="stdcall";static char
_tmp51A[6]="cdecl";static char _tmp51B[9]="fastcall";static char _tmp51C[9]="noreturn";
static char _tmp51D[6]="const";static char _tmp51E[8]="aligned";static char _tmp51F[7]="packed";
static char _tmp520[7]="shared";static char _tmp521[7]="unused";static char _tmp522[5]="weak";
static char _tmp523[10]="dllimport";static char _tmp524[10]="dllexport";static char
_tmp525[23]="no_instrument_function";static char _tmp526[12]="constructor";static
char _tmp527[11]="destructor";static char _tmp528[22]="no_check_memory_usage";
static char _tmp529[5]="pure";struct _tuple26{void*f1;void*f2;};struct _tuple27{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*
yylex_buf);static void _tmpA07(unsigned int*_tmpA06,unsigned int*_tmpA05,short**
_tmpA04){for(*_tmpA06=0;*_tmpA06 < *_tmpA05;(*_tmpA06)++){(*_tmpA04)[*_tmpA06]=(
short)0;}}static void _tmpA0C(unsigned int*_tmpA0B,unsigned int*_tmpA0A,union Cyc_YYSTYPE**
_tmpA09){for(*_tmpA0B=0;*_tmpA0B < *_tmpA0A;(*_tmpA0B)++){(*_tmpA09)[*_tmpA0B]=
Cyc_yylval;}}static void _tmpA11(unsigned int*_tmpA10,unsigned int*_tmpA0F,struct
Cyc_Yyltype**_tmpA0E){for(*_tmpA10=0;*_tmpA10 < *_tmpA0F;(*_tmpA10)++){(*_tmpA0E)[*
_tmpA10]=Cyc_yynewloc();}}static void _tmpF0B(unsigned int*_tmpF0A,unsigned int*
_tmpF09,char**_tmpF07){for(*_tmpF0A=0;*_tmpF0A < *_tmpF09;(*_tmpF0A)++){(*_tmpF07)[*
_tmpF0A]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct
_RegionHandle _tmp47F=_new_region("yyregion");struct _RegionHandle*yyregion=&
_tmp47F;_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;
int yyssp_offset;unsigned int _tmpA06;unsigned int _tmpA05;short*_tmpA04;
unsigned int _tmpA03;short*yyss=(short*)((_tmpA03=10000,((_tmpA04=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmpA03)),((((_tmpA05=_tmpA03,
_tmpA07(& _tmpA06,& _tmpA05,& _tmpA04))),_tmpA04))))));int yyvsp_offset;unsigned int
_tmpA0B;unsigned int _tmpA0A;union Cyc_YYSTYPE*_tmpA09;unsigned int _tmpA08;union
Cyc_YYSTYPE*yyvs=(union Cyc_YYSTYPE*)((_tmpA08=10000,((_tmpA09=(union Cyc_YYSTYPE*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA08)),((((
_tmpA0A=_tmpA08,_tmpA0C(& _tmpA0B,& _tmpA0A,& _tmpA09))),_tmpA09))))));int
yylsp_offset;unsigned int _tmpA10;unsigned int _tmpA0F;struct Cyc_Yyltype*_tmpA0E;
unsigned int _tmpA0D;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)((_tmpA0D=10000,((
_tmpA0E=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct
Cyc_Yyltype),_tmpA0D)),((((_tmpA0F=_tmpA0D,_tmpA11(& _tmpA10,& _tmpA0F,& _tmpA0E))),
_tmpA0E))))));int yystacksize=10000;union Cyc_YYSTYPE yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){{const char*_tmpA12;Cyc_yyerror(((_tmpA12="parser stack overflow",
_tag_dyneither(_tmpA12,sizeof(char),22))));}(int)_throw((void*)& Cyc_Yystack_overflow_val);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1027,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 370?(int)Cyc_yytranslate[_check_known_subscript_notnull(371,Cyc_yychar)]:
286;}yyn +=yychar1;if((yyn < 0  || yyn > 5862) || Cyc_yycheck[
_check_known_subscript_notnull(5863,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5863,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1026){int
_tmp481=0;_npop_handler(0);return _tmp481;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1027,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(510,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL227: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL228: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp482;_push_handler(& _tmp482);{int _tmp484=0;if(setjmp(_tmp482.handler))_tmp484=
1;if(!_tmp484){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp483=(void*)_exn_thrown;void*
_tmp486=_tmp483;_LL22B: {struct Cyc_Core_Failure_struct*_tmp487=(struct Cyc_Core_Failure_struct*)
_tmp486;if(_tmp487->tag != Cyc_Core_Failure)goto _LL22D;}_LL22C: x=0;goto _LL22A;
_LL22D:;_LL22E:(void)_throw(_tmp486);_LL22A:;}};}{struct _handler_cons _tmp488;
_push_handler(& _tmp488);{int _tmp48A=0;if(setjmp(_tmp488.handler))_tmp48A=1;if(!
_tmp48A){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;
_pop_handler();}else{void*_tmp489=(void*)_exn_thrown;void*_tmp48C=_tmp489;_LL230: {
struct Cyc_Core_Failure_struct*_tmp48D=(struct Cyc_Core_Failure_struct*)_tmp48C;
if(_tmp48D->tag != Cyc_Core_Failure)goto _LL232;}_LL231: y=0;goto _LL22F;_LL232:;
_LL233:(void)_throw(_tmp48C);_LL22F:;}};}yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));break;}
case 3: _LL229:{struct Cyc_Absyn_Decl*_tmpA1C;struct Cyc_Absyn_Using_d_struct _tmpA1B;
struct Cyc_Absyn_Using_d_struct*_tmpA1A;struct Cyc_List_List*_tmpA19;yyval=Cyc_YY18(((
_tmpA19=_cycalloc(sizeof(*_tmpA19)),((_tmpA19->hd=((_tmpA1C=_cycalloc(sizeof(*
_tmpA1C)),((_tmpA1C->r=(void*)((_tmpA1A=_cycalloc(sizeof(*_tmpA1A)),((_tmpA1A[0]=((
_tmpA1B.tag=11,((_tmpA1B.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA1B.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA1B)))))),_tmpA1A)))),((
_tmpA1C->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA1C)))))),((_tmpA19->tl=0,_tmpA19)))))));}Cyc_Lex_leave_using();
break;case 4: _LL234:{struct Cyc_Absyn_Decl*_tmpA26;struct Cyc_Absyn_Using_d_struct
_tmpA25;struct Cyc_Absyn_Using_d_struct*_tmpA24;struct Cyc_List_List*_tmpA23;yyval=
Cyc_YY18(((_tmpA23=_cycalloc(sizeof(*_tmpA23)),((_tmpA23->hd=((_tmpA26=_cycalloc(
sizeof(*_tmpA26)),((_tmpA26->r=(void*)((_tmpA24=_cycalloc(sizeof(*_tmpA24)),((
_tmpA24[0]=((_tmpA25.tag=11,((_tmpA25.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpA25.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA25)))))),
_tmpA24)))),((_tmpA26->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA26)))))),((
_tmpA23->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA23)))))));}break;case 5: _LL235:{struct Cyc_Absyn_Decl*_tmpA35;
struct Cyc_Absyn_Namespace_d_struct _tmpA34;struct _dyneither_ptr*_tmpA33;struct Cyc_Absyn_Namespace_d_struct*
_tmpA32;struct Cyc_List_List*_tmpA31;yyval=Cyc_YY18(((_tmpA31=_cycalloc(sizeof(*
_tmpA31)),((_tmpA31->hd=((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35->r=(void*)((
_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32[0]=((_tmpA34.tag=10,((_tmpA34.f1=((
_tmpA33=_cycalloc(sizeof(*_tmpA33)),((_tmpA33[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA33)))),((_tmpA34.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA34)))))),
_tmpA32)))),((_tmpA35->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA35)))))),((
_tmpA31->tl=0,_tmpA31)))))));}Cyc_Lex_leave_namespace();break;case 6: _LL236: {
struct _dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp49B;_push_handler(& _tmp49B);{int _tmp49D=0;if(setjmp(_tmp49B.handler))
_tmp49D=1;if(!_tmp49D){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp49C=(void*)_exn_thrown;void*_tmp49F=_tmp49C;_LL239: {struct Cyc_Core_Failure_struct*
_tmp4A0=(struct Cyc_Core_Failure_struct*)_tmp49F;if(_tmp4A0->tag != Cyc_Core_Failure)
goto _LL23B;}_LL23A:{const char*_tmpA36;nspace=((_tmpA36="",_tag_dyneither(_tmpA36,
sizeof(char),1)));}goto _LL238;_LL23B:;_LL23C:(void)_throw(_tmp49F);_LL238:;}};}{
struct _handler_cons _tmp4A2;_push_handler(& _tmp4A2);{int _tmp4A4=0;if(setjmp(
_tmp4A2.handler))_tmp4A4=1;if(!_tmp4A4){x=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp4A3=(void*)_exn_thrown;void*_tmp4A6=_tmp4A3;_LL23E: {struct Cyc_Core_Failure_struct*
_tmp4A7=(struct Cyc_Core_Failure_struct*)_tmp4A6;if(_tmp4A7->tag != Cyc_Core_Failure)
goto _LL240;}_LL23F: x=0;goto _LL23D;_LL240:;_LL241:(void)_throw(_tmp4A6);_LL23D:;}};}{
struct _handler_cons _tmp4A8;_push_handler(& _tmp4A8);{int _tmp4AA=0;if(setjmp(
_tmp4A8.handler))_tmp4AA=1;if(!_tmp4AA){y=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{void*
_tmp4A9=(void*)_exn_thrown;void*_tmp4AC=_tmp4A9;_LL243: {struct Cyc_Core_Failure_struct*
_tmp4AD=(struct Cyc_Core_Failure_struct*)_tmp4AC;if(_tmp4AD->tag != Cyc_Core_Failure)
goto _LL245;}_LL244: y=0;goto _LL242;_LL245:;_LL246:(void)_throw(_tmp4AC);_LL242:;}};}{
struct Cyc_Absyn_Decl*_tmpA45;struct Cyc_Absyn_Namespace_d_struct _tmpA44;struct
_dyneither_ptr*_tmpA43;struct Cyc_Absyn_Namespace_d_struct*_tmpA42;struct Cyc_List_List*
_tmpA41;yyval=Cyc_YY18(((_tmpA41=_cycalloc(sizeof(*_tmpA41)),((_tmpA41->hd=((
_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45->r=(void*)((_tmpA42=_cycalloc(
sizeof(*_tmpA42)),((_tmpA42[0]=((_tmpA44.tag=10,((_tmpA44.f1=((_tmpA43=_cycalloc(
sizeof(*_tmpA43)),((_tmpA43[0]=nspace,_tmpA43)))),((_tmpA44.f2=x,_tmpA44)))))),
_tmpA42)))),((_tmpA45->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA45)))))),((
_tmpA41->tl=y,_tmpA41)))))));}break;}case 7: _LL237:{const char*_tmpA46;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),((_tmpA46="C",_tag_dyneither(_tmpA46,sizeof(char),2))))
== 0){struct Cyc_Absyn_Decl*_tmpA50;struct Cyc_Absyn_ExternC_d_struct _tmpA4F;
struct Cyc_Absyn_ExternC_d_struct*_tmpA4E;struct Cyc_List_List*_tmpA4D;yyval=Cyc_YY18(((
_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D->hd=((_tmpA50=_cycalloc(sizeof(*
_tmpA50)),((_tmpA50->r=(void*)((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E[0]=((
_tmpA4F.tag=12,((_tmpA4F.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpA4F)))),_tmpA4E)))),((_tmpA50->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA50)))))),((
_tmpA4D->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA4D)))))));}else{{const char*_tmpA51;if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),((_tmpA51="C include",_tag_dyneither(_tmpA51,sizeof(char),10))))
!= 0){const char*_tmpA52;Cyc_Parse_err(((_tmpA52="expecting \"C\" or \"C include\"",
_tag_dyneither(_tmpA52,sizeof(char),29))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}}{struct Cyc_Absyn_Decl*
_tmpA5C;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA5B;struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpA5A;struct Cyc_List_List*_tmpA59;yyval=Cyc_YY18(((_tmpA59=_cycalloc(sizeof(*
_tmpA59)),((_tmpA59->hd=((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C->r=(void*)((
_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A[0]=((_tmpA5B.tag=13,((_tmpA5B.f1=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpA5B.f2=0,_tmpA5B)))))),_tmpA5A)))),((_tmpA5C->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA5C)))))),((
_tmpA59->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA59)))))));};}}break;case 8: _LL247:{const char*_tmpA5D;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]),((_tmpA5D="C include",_tag_dyneither(_tmpA5D,sizeof(
char),10))))!= 0){const char*_tmpA5E;Cyc_Parse_err(((_tmpA5E="expecting \"C include\"",
_tag_dyneither(_tmpA5E,sizeof(char),22))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_Absyn_Decl*_tmpA68;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA67;
struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA66;struct Cyc_List_List*_tmpA65;
yyval=Cyc_YY18(((_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65->hd=((_tmpA68=
_cycalloc(sizeof(*_tmpA68)),((_tmpA68->r=(void*)((_tmpA66=_cycalloc(sizeof(*
_tmpA66)),((_tmpA66[0]=((_tmpA67.tag=13,((_tmpA67.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA67.f2=exs,_tmpA67)))))),
_tmpA66)))),((_tmpA68->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA68)))))),((
_tmpA65->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA65)))))));}break;};case 9: _LL248:{struct Cyc_Absyn_Decl*
_tmpA6B;struct Cyc_List_List*_tmpA6A;yyval=Cyc_YY18(((_tmpA6A=_cycalloc(sizeof(*
_tmpA6A)),((_tmpA6A->hd=((_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B->r=(void*)&
Cyc_Absyn_Porton_d_val,((_tmpA6B->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA6B)))))),((
_tmpA6A->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA6A)))))));}break;case 10: _LL249:{struct Cyc_Absyn_Decl*
_tmpA6E;struct Cyc_List_List*_tmpA6D;yyval=Cyc_YY18(((_tmpA6D=_cycalloc(sizeof(*
_tmpA6D)),((_tmpA6D->hd=((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E->r=(void*)&
Cyc_Absyn_Portoff_d_val,((_tmpA6E->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA6E)))))),((
_tmpA6D->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA6D)))))));}break;case 11: _LL24A: yyval=Cyc_YY18(0);break;case
12: _LL24B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 13: _LL24C: yyval=Cyc_YY52(0);break;case 14: _LL24D:{struct _tuple24*
_tmpA71;struct Cyc_List_List*_tmpA70;yyval=Cyc_YY52(((_tmpA70=_cycalloc(sizeof(*
_tmpA70)),((_tmpA70->hd=((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpA71->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpA71->f3=0,_tmpA71)))))))),((_tmpA70->tl=0,_tmpA70)))))));}break;case 15:
_LL24E:{struct _tuple24*_tmpA74;struct Cyc_List_List*_tmpA73;yyval=Cyc_YY52(((
_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73->hd=((_tmpA74=_cycalloc(sizeof(*
_tmpA74)),((_tmpA74->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpA74->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpA74->f3=0,_tmpA74)))))))),((_tmpA73->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA73)))))));}break;case
16: _LL24F:{struct Cyc_Absyn_Fn_d_struct*_tmpA7A;struct Cyc_Absyn_Fn_d_struct
_tmpA79;struct Cyc_List_List*_tmpA78;yyval=Cyc_YY18(((_tmpA78=_cycalloc(sizeof(*
_tmpA78)),((_tmpA78->hd=Cyc_Absyn_new_decl((void*)((_tmpA7A=_cycalloc(sizeof(*
_tmpA7A)),((_tmpA7A[0]=((_tmpA79.tag=1,((_tmpA79.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA79)))),_tmpA7A)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA78->tl=0,
_tmpA78)))))));}break;case 17: _LL250: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 18: _LL251: yyval=Cyc_YY18(0);break;case 19: _LL252:
yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 20:
_LL253:{struct Cyc_Core_Opt*_tmpA7B;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA7B)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 21:
_LL254: yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 22:
_LL255:{struct Cyc_Core_Opt*_tmpA7C;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA7C)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 23:
_LL256:{struct Cyc_Core_Opt*_tmpA7D;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA7D)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 24:
_LL257:{struct Cyc_Core_Opt*_tmpA7E;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA7E)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 25:
_LL258: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL259: Cyc_Lex_leave_using();
break;case 27: _LL25A:{struct _dyneither_ptr*_tmpA7F;Cyc_Lex_enter_namespace(((
_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA7F)))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL25B: Cyc_Lex_leave_namespace();
break;case 29: _LL25C: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
30: _LL25D: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 31:
_LL25E:{struct Cyc_List_List*_tmpA80;yyval=Cyc_YY18(((_tmpA80=_cycalloc(sizeof(*
_tmpA80)),((_tmpA80->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA80->tl=0,
_tmpA80)))))));}break;case 32: _LL25F: {struct Cyc_List_List*_tmp4D5=0;{struct Cyc_List_List*
_tmp4D6=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
for(0;_tmp4D6 != 0;_tmp4D6=_tmp4D6->tl){struct _dyneither_ptr*_tmp4D7=(struct
_dyneither_ptr*)_tmp4D6->hd;struct _tuple1*_tmpA81;struct _tuple1*qv=(_tmpA81=
_cycalloc(sizeof(*_tmpA81)),((_tmpA81->f1=Cyc_Absyn_Rel_n(0),((_tmpA81->f2=
_tmp4D7,_tmpA81)))));struct Cyc_Absyn_Vardecl*_tmp4D8=Cyc_Absyn_new_vardecl(qv,
Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*_tmpA82;_tmp4D5=((_tmpA82=_cycalloc(
sizeof(*_tmpA82)),((_tmpA82->hd=_tmp4D8,((_tmpA82->tl=_tmp4D5,_tmpA82))))));}}
_tmp4D5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4D5);{struct Cyc_List_List*_tmpA83;yyval=Cyc_YY18(((_tmpA83=_cycalloc(sizeof(*
_tmpA83)),((_tmpA83->hd=Cyc_Absyn_letv_decl(_tmp4D5,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA83->tl=0,
_tmpA83)))))));}break;}case 33: _LL260:{const char*_tmpA84;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpA84="`H",_tag_dyneither(_tmpA84,sizeof(char),3))))== 0){
const char*_tmpA87;void*_tmpA86;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA86=0,
Cyc_aprintf(((_tmpA87="bad occurrence of heap region",_tag_dyneither(_tmpA87,
sizeof(char),30))),_tag_dyneither(_tmpA86,sizeof(void*),0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{const char*
_tmpA88;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA88="`U",
_tag_dyneither(_tmpA88,sizeof(char),3))))== 0){const char*_tmpA8B;void*_tmpA8A;
Cyc_Parse_err((struct _dyneither_ptr)((_tmpA8A=0,Cyc_aprintf(((_tmpA8B="bad occurrence of unique region",
_tag_dyneither(_tmpA8B,sizeof(char),32))),_tag_dyneither(_tmpA8A,sizeof(void*),0)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));}}{struct _dyneither_ptr*_tmpA8E;struct Cyc_Absyn_Tvar*
_tmpA8D;struct Cyc_Absyn_Tvar*tv=(_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D->name=((
_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA8E)))),((_tmpA8D->identity=
- 1,((_tmpA8D->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA8D)))))));
struct Cyc_Absyn_VarType_struct _tmpA91;struct Cyc_Absyn_VarType_struct*_tmpA90;
void*t=(void*)((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90[0]=((_tmpA91.tag=2,((
_tmpA91.f1=tv,_tmpA91)))),_tmpA90))));struct Cyc_Absyn_RgnHandleType_struct
_tmpA97;struct Cyc_Absyn_RgnHandleType_struct*_tmpA96;struct _dyneither_ptr*
_tmpA94;struct _tuple1*_tmpA93;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93->f1=Cyc_Absyn_Loc_n,((_tmpA93->f2=((
_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA94)))),_tmpA93)))))),(
void*)((_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96[0]=((_tmpA97.tag=16,((
_tmpA97.f1=(void*)t,_tmpA97)))),_tmpA96)))),0);{struct Cyc_List_List*_tmpA98;
yyval=Cyc_YY18(((_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpA98->tl=0,_tmpA98)))))));}break;};case 34:
_LL261:{const char*_tmpA99;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA99="H",
_tag_dyneither(_tmpA99,sizeof(char),2))))== 0){const char*_tmpA9A;Cyc_Parse_err(((
_tmpA9A="bad occurrence of heap region `H",_tag_dyneither(_tmpA9A,sizeof(char),
33))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));}}{const char*_tmpA9B;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmpA9B="U",_tag_dyneither(_tmpA9B,sizeof(char),2))))== 0){
const char*_tmpA9C;Cyc_Parse_err(((_tmpA9C="bad occurrence of unique region `U",
_tag_dyneither(_tmpA9C,sizeof(char),35))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));}}if(Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){const char*
_tmpA9D;Cyc_Parse_err(((_tmpA9D="open regions cannot be @resetable",
_tag_dyneither(_tmpA9D,sizeof(char),34))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}{struct
_dyneither_ptr*_tmpAAA;const char*_tmpAA9;void*_tmpAA8[1];struct Cyc_String_pa_struct
_tmpAA7;struct Cyc_Absyn_Tvar*_tmpAA6;struct Cyc_Absyn_Tvar*tv=(_tmpAA6=_cycalloc(
sizeof(*_tmpAA6)),((_tmpAA6->name=((_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((
_tmpAAA[0]=(struct _dyneither_ptr)((_tmpAA7.tag=0,((_tmpAA7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpAA8[0]=& _tmpAA7,
Cyc_aprintf(((_tmpAA9="`%s",_tag_dyneither(_tmpAA9,sizeof(char),4))),
_tag_dyneither(_tmpAA8,sizeof(void*),1)))))))),_tmpAAA)))),((_tmpAA6->identity=-
1,((_tmpAA6->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAA6)))))));struct
Cyc_Absyn_VarType_struct _tmpAAD;struct Cyc_Absyn_VarType_struct*_tmpAAC;void*t=(
void*)((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC[0]=((_tmpAAD.tag=2,((
_tmpAAD.f1=tv,_tmpAAD)))),_tmpAAC))));struct Cyc_Absyn_RgnHandleType_struct
_tmpAB3;struct Cyc_Absyn_RgnHandleType_struct*_tmpAB2;struct _dyneither_ptr*
_tmpAB0;struct _tuple1*_tmpAAF;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpAAF=_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF->f1=Cyc_Absyn_Loc_n,((_tmpAAF->f2=((
_tmpAB0=_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAB0)))),_tmpAAF)))))),(
void*)((_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2[0]=((_tmpAB3.tag=16,((
_tmpAB3.f1=(void*)t,_tmpAB3)))),_tmpAB2)))),0);{struct Cyc_List_List*_tmpAB4;
yyval=Cyc_YY18(((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAB4->tl=0,
_tmpAB4)))))));}break;};case 35: _LL262: {struct Cyc_Absyn_Eq_kb_struct*_tmpABC;
struct Cyc_Absyn_Eq_kb_struct _tmpABB;struct _dyneither_ptr*_tmpABA;struct Cyc_Absyn_Tvar*
_tmpAB9;struct Cyc_Absyn_Tvar*tv=(_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->name=((
_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpABA)))),((_tmpAB9->identity=
- 1,((_tmpAB9->kind=(void*)((_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC[0]=((
_tmpABB.tag=0,((_tmpABB.f1=& Cyc_Tcutil_rk,_tmpABB)))),_tmpABC)))),_tmpAB9)))))));
struct _dyneither_ptr*_tmpABF;struct _tuple1*_tmpABE;struct Cyc_Absyn_Vardecl*vd=
Cyc_Absyn_new_vardecl(((_tmpABE=_cycalloc(sizeof(*_tmpABE)),((_tmpABE->f1=Cyc_Absyn_Loc_n,((
_tmpABE->f2=((_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpABF)))),_tmpABE)))))),(
void*)& Cyc_Absyn_VoidType_val,0);{struct Cyc_List_List*_tmpAC0;yyval=Cyc_YY18(((
_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAC0->tl=0,
_tmpAC0)))))));}break;}case 36: _LL263: yyval=Cyc_YY31(0);break;case 37: _LL264: yyval=
Cyc_YY31(1);break;case 38: _LL265: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 39: _LL266: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 40: _LL267:{
struct Cyc_Core_Opt*_tmpAC3;struct Cyc_Parse_Declaration_spec*_tmpAC2;yyval=Cyc_YY19(((
_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2->sc=((_tmpAC3=_cycalloc_atomic(
sizeof(*_tmpAC3)),((_tmpAC3->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAC3)))),((_tmpAC2->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAC2->type_specs=
0,((_tmpAC2->is_inline=0,((_tmpAC2->attributes=0,_tmpAC2)))))))))))));}break;
case 41: _LL268: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc != 0){const char*_tmpAC6;void*_tmpAC5;(_tmpAC5=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpAC6="Only one storage class is allowed in a declaration",
_tag_dyneither(_tmpAC6,sizeof(char),51))),_tag_dyneither(_tmpAC5,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmpAC9;struct Cyc_Parse_Declaration_spec*_tmpAC8;yyval=Cyc_YY19(((
_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((_tmpAC8->sc=((_tmpAC9=_cycalloc_atomic(
sizeof(*_tmpAC9)),((_tmpAC9->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpAC9)))),((_tmpAC8->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAC8->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAC8->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpAC8->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,
_tmpAC8)))))))))))));}break;case 42: _LL269:{struct Cyc_List_List*_tmpACC;struct Cyc_Parse_Declaration_spec*
_tmpACB;yyval=Cyc_YY19(((_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB->sc=0,((
_tmpACB->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpACB->type_specs=((
_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpACC->tl=0,_tmpACC)))))),((
_tmpACB->is_inline=0,((_tmpACB->attributes=0,_tmpACB)))))))))))));}break;case 43:
_LL26A:{struct Cyc_List_List*_tmpACF;struct Cyc_Parse_Declaration_spec*_tmpACE;
yyval=Cyc_YY19(((_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpACE->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpACE->type_specs=((_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpACF->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,_tmpACF)))))),((
_tmpACE->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpACE->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpACE)))))))))))));}
break;case 44: _LL26B:{struct Cyc_Parse_Declaration_spec*_tmpAD0;yyval=Cyc_YY19(((
_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0->sc=0,((_tmpAD0->tq=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAD0->type_specs=
0,((_tmpAD0->is_inline=0,((_tmpAD0->attributes=0,_tmpAD0)))))))))))));}break;
case 45: _LL26C:{struct Cyc_Parse_Declaration_spec*_tmpAD1;yyval=Cyc_YY19(((_tmpAD1=
_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAD1->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq),((_tmpAD1->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpAD1->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpAD1->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAD1)))))))))))));}
break;case 46: _LL26D:{struct Cyc_Parse_Declaration_spec*_tmpAD2;yyval=Cyc_YY19(((
_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2->sc=0,((_tmpAD2->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAD2->type_specs=0,((_tmpAD2->is_inline=1,((
_tmpAD2->attributes=0,_tmpAD2)))))))))))));}break;case 47: _LL26E:{struct Cyc_Parse_Declaration_spec*
_tmpAD3;yyval=Cyc_YY19(((_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAD3->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAD3->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAD3->is_inline=1,((_tmpAD3->attributes=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAD3)))))))))))));}
break;case 48: _LL26F:{struct Cyc_Parse_Declaration_spec*_tmpAD4;yyval=Cyc_YY19(((
_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4->sc=0,((_tmpAD4->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAD4->type_specs=0,((_tmpAD4->is_inline=0,((
_tmpAD4->attributes=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpAD4)))))))))))));}break;case 49: _LL270:{struct Cyc_Parse_Declaration_spec*
_tmpAD5;yyval=Cyc_YY19(((_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAD5->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAD5->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAD5->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpAD5->attributes=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes),_tmpAD5)))))))))))));}
break;case 50: _LL271: yyval=Cyc_YY22(Cyc_Parse_Auto_sc);break;case 51: _LL272: yyval=
Cyc_YY22(Cyc_Parse_Register_sc);break;case 52: _LL273: yyval=Cyc_YY22(Cyc_Parse_Static_sc);
break;case 53: _LL274: yyval=Cyc_YY22(Cyc_Parse_Extern_sc);break;case 54: _LL275:{
const char*_tmpAD6;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAD6="C",
_tag_dyneither(_tmpAD6,sizeof(char),2))))!= 0){const char*_tmpAD7;Cyc_Parse_err(((
_tmpAD7="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAD7,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}}yyval=Cyc_YY22(Cyc_Parse_ExternC_sc);break;case 55:
_LL276: yyval=Cyc_YY22(Cyc_Parse_Typedef_sc);break;case 56: _LL277: yyval=Cyc_YY22(
Cyc_Parse_Abstract_sc);break;case 57: _LL278: yyval=Cyc_YY45(0);break;case 58: _LL279:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 59:
_LL27A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 60: _LL27B:{struct Cyc_List_List*_tmpAD8;yyval=Cyc_YY45(((_tmpAD8=_cycalloc(
sizeof(*_tmpAD8)),((_tmpAD8->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAD8->tl=0,_tmpAD8)))))));}
break;case 61: _LL27C:{struct Cyc_List_List*_tmpAD9;yyval=Cyc_YY45(((_tmpAD9=
_cycalloc(sizeof(*_tmpAD9)),((_tmpAD9->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpAD9->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAD9)))))));}break;
case 62: _LL27D: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={6,- 1};
static struct _tuple25 att_map[17]={{{_tmp519,_tmp519,_tmp519 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{
_tmp51A,_tmp51A,_tmp51A + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp51B,_tmp51B,
_tmp51B + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp51C,_tmp51C,_tmp51C + 9},(
void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp51D,_tmp51D,_tmp51D + 6},(void*)& Cyc_Absyn_Const_att_val},{{
_tmp51E,_tmp51E,_tmp51E + 8},(void*)& att_aligned},{{_tmp51F,_tmp51F,_tmp51F + 7},(
void*)& Cyc_Absyn_Packed_att_val},{{_tmp520,_tmp520,_tmp520 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{
_tmp521,_tmp521,_tmp521 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp522,_tmp522,
_tmp522 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp523,_tmp523,_tmp523 + 10},(void*)&
Cyc_Absyn_Dllimport_att_val},{{_tmp524,_tmp524,_tmp524 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{
_tmp525,_tmp525,_tmp525 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{
_tmp526,_tmp526,_tmp526 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp527,
_tmp527,_tmp527 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp528,_tmp528,
_tmp528 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp529,_tmp529,
_tmp529 + 5},(void*)& Cyc_Absyn_Pure_att_val}};struct _dyneither_ptr _tmp517=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp517,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp517,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp517,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp517,sizeof(char),(int)(_get_dyneither_size(
_tmp517,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp517,sizeof(char),(int)(_get_dyneither_size(_tmp517,sizeof(char))- 3)))== '_')
_tmp517=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp517,2,
_get_dyneither_size(_tmp517,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp517,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){{const char*_tmpADA;Cyc_Parse_err(((
_tmpADA="unrecognized attribute",_tag_dyneither(_tmpADA,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=Cyc_YY46((
void*)& Cyc_Absyn_Cdecl_att_val);}break;};}case 63: _LL27E: yyval=Cyc_YY46((void*)&
Cyc_Absyn_Const_att_val);break;case 64: _LL27F: {struct _dyneither_ptr _tmp52B=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp52D;struct
_tuple6 _tmp52C=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp52D=_tmp52C.f2;{void*a;{const char*_tmpADC;const char*
_tmpADB;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpADC="regparm",
_tag_dyneither(_tmpADC,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp52B,((_tmpADB="__regparm__",_tag_dyneither(_tmpADB,sizeof(
char),12))))== 0){if(_tmp52D < 0  || _tmp52D > 3){const char*_tmpADD;Cyc_Parse_err(((
_tmpADD="regparm requires value between 0 and 3",_tag_dyneither(_tmpADD,sizeof(
char),39))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Regparm_att_struct _tmpAE0;
struct Cyc_Absyn_Regparm_att_struct*_tmpADF;a=(void*)((_tmpADF=_cycalloc_atomic(
sizeof(*_tmpADF)),((_tmpADF[0]=((_tmpAE0.tag=0,((_tmpAE0.f1=_tmp52D,_tmpAE0)))),
_tmpADF))));};}else{const char*_tmpAE2;const char*_tmpAE1;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp52B,((_tmpAE2="aligned",_tag_dyneither(_tmpAE2,sizeof(char),8))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAE1="__aligned__",
_tag_dyneither(_tmpAE1,sizeof(char),12))))== 0){if(_tmp52D < 0){const char*_tmpAE3;
Cyc_Parse_err(((_tmpAE3="aligned requires positive power of two",_tag_dyneither(
_tmpAE3,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp52D;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
const char*_tmpAE4;Cyc_Parse_err(((_tmpAE4="aligned requires positive power of two",
_tag_dyneither(_tmpAE4,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmpAE7;struct Cyc_Absyn_Aligned_att_struct*_tmpAE6;a=(void*)((_tmpAE6=
_cycalloc_atomic(sizeof(*_tmpAE6)),((_tmpAE6[0]=((_tmpAE7.tag=6,((_tmpAE7.f1=
_tmp52D,_tmpAE7)))),_tmpAE6))));};};}else{const char*_tmpAE9;const char*_tmpAE8;
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAE9="initializes",
_tag_dyneither(_tmpAE9,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp52B,((_tmpAE8="__initializes__",_tag_dyneither(_tmpAE8,
sizeof(char),16))))== 0){struct Cyc_Absyn_Initializes_att_struct _tmpAEC;struct Cyc_Absyn_Initializes_att_struct*
_tmpAEB;a=(void*)((_tmpAEB=_cycalloc_atomic(sizeof(*_tmpAEB)),((_tmpAEB[0]=((
_tmpAEC.tag=20,((_tmpAEC.f1=_tmp52D,_tmpAEC)))),_tmpAEB))));}else{const char*
_tmpAEE;const char*_tmpAED;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAEE="noliveunique",
_tag_dyneither(_tmpAEE,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp52B,((_tmpAED="__noliveunique__",_tag_dyneither(_tmpAED,
sizeof(char),17))))== 0){struct Cyc_Absyn_Noliveunique_att_struct _tmpAF1;struct
Cyc_Absyn_Noliveunique_att_struct*_tmpAF0;a=(void*)((_tmpAF0=_cycalloc_atomic(
sizeof(*_tmpAF0)),((_tmpAF0[0]=((_tmpAF1.tag=21,((_tmpAF1.f1=_tmp52D,_tmpAF1)))),
_tmpAF0))));}else{{const char*_tmpAF2;Cyc_Parse_err(((_tmpAF2="unrecognized attribute",
_tag_dyneither(_tmpAF2,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}}}}yyval=Cyc_YY46(a);break;};}case 65: _LL280: {struct
_dyneither_ptr _tmp542=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp543=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpAF4;const char*_tmpAF3;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp542,((_tmpAF4="section",
_tag_dyneither(_tmpAF4,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp542,((_tmpAF3="__section__",_tag_dyneither(_tmpAF3,sizeof(
char),12))))== 0){struct Cyc_Absyn_Section_att_struct _tmpAF7;struct Cyc_Absyn_Section_att_struct*
_tmpAF6;a=(void*)((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6[0]=((_tmpAF7.tag=
8,((_tmpAF7.f1=_tmp543,_tmpAF7)))),_tmpAF6))));}else{{const char*_tmpAF8;Cyc_Parse_err(((
_tmpAF8="unrecognized attribute",_tag_dyneither(_tmpAF8,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 66: _LL281: {struct
_dyneither_ptr _tmp549=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp54A=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpAF9;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp549,((_tmpAF9="__mode__",
_tag_dyneither(_tmpAF9,sizeof(char),9))))== 0){struct Cyc_Absyn_Mode_att_struct
_tmpAFC;struct Cyc_Absyn_Mode_att_struct*_tmpAFB;a=(void*)((_tmpAFB=_cycalloc(
sizeof(*_tmpAFB)),((_tmpAFB[0]=((_tmpAFC.tag=23,((_tmpAFC.f1=_tmp54A,_tmpAFC)))),
_tmpAFB))));}else{{const char*_tmpAFD;Cyc_Parse_err(((_tmpAFD="unrecognized attribute",
_tag_dyneither(_tmpAFD,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 67: _LL282: {struct
_dyneither_ptr _tmp54F=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp550=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp552;struct _tuple6
_tmp551=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp552=_tmp551.f2;{int _tmp554;struct _tuple6 _tmp553=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp554=_tmp553.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;{const char*_tmpAFF;const char*_tmpAFE;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp54F,((_tmpAFF="format",_tag_dyneither(
_tmpAFF,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp54F,((
_tmpAFE="__format__",_tag_dyneither(_tmpAFE,sizeof(char),11))))== 0){const char*
_tmpB00;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp550,((_tmpB00="printf",
_tag_dyneither(_tmpB00,sizeof(char),7))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpB03;struct Cyc_Absyn_Format_att_struct*_tmpB02;a=(void*)((_tmpB02=
_cycalloc_atomic(sizeof(*_tmpB02)),((_tmpB02[0]=((_tmpB03.tag=19,((_tmpB03.f1=
Cyc_Absyn_Printf_ft,((_tmpB03.f2=_tmp552,((_tmpB03.f3=_tmp554,_tmpB03)))))))),
_tmpB02))));}else{const char*_tmpB04;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp550,((
_tmpB04="scanf",_tag_dyneither(_tmpB04,sizeof(char),6))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpB07;struct Cyc_Absyn_Format_att_struct*_tmpB06;a=(void*)((_tmpB06=
_cycalloc_atomic(sizeof(*_tmpB06)),((_tmpB06[0]=((_tmpB07.tag=19,((_tmpB07.f1=
Cyc_Absyn_Scanf_ft,((_tmpB07.f2=_tmp552,((_tmpB07.f3=_tmp554,_tmpB07)))))))),
_tmpB06))));}else{const char*_tmpB08;Cyc_Parse_err(((_tmpB08="unrecognized format type",
_tag_dyneither(_tmpB08,sizeof(char),25))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{
const char*_tmpB09;Cyc_Parse_err(((_tmpB09="unrecognized attribute",
_tag_dyneither(_tmpB09,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY46(
a);break;};};}case 68: _LL283: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 69: _LL284:{struct Cyc_Absyn_TypedefType_struct _tmpB0C;
struct Cyc_Absyn_TypedefType_struct*_tmpB0B;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B[0]=((_tmpB0C.tag=18,((
_tmpB0C.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB0C.f2=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB0C.f3=0,((_tmpB0C.f4=
0,_tmpB0C)))))))))),_tmpB0B)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 70:
_LL285: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 71:
_LL286: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 72:
_LL287: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 73:
_LL288: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 74:
_LL289:{struct Cyc_Parse_Short_spec_struct _tmpB0F;struct Cyc_Parse_Short_spec_struct*
_tmpB0E;yyval=Cyc_YY23((void*)((_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E[0]=((
_tmpB0F.tag=2,((_tmpB0F.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB0F)))),
_tmpB0E)))));}break;case 75: _LL28A: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 76: _LL28B:{struct Cyc_Parse_Long_spec_struct
_tmpB12;struct Cyc_Parse_Long_spec_struct*_tmpB11;yyval=Cyc_YY23((void*)((_tmpB11=
_cycalloc(sizeof(*_tmpB11)),((_tmpB11[0]=((_tmpB12.tag=3,((_tmpB12.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB12)))),
_tmpB11)))));}break;case 77: _LL28C: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 78: _LL28D: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 79:
_LL28E:{struct Cyc_Parse_Signed_spec_struct _tmpB15;struct Cyc_Parse_Signed_spec_struct*
_tmpB14;yyval=Cyc_YY23((void*)((_tmpB14=_cycalloc(sizeof(*_tmpB14)),((_tmpB14[0]=((
_tmpB15.tag=0,((_tmpB15.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB15)))),
_tmpB14)))));}break;case 80: _LL28F:{struct Cyc_Parse_Unsigned_spec_struct _tmpB18;
struct Cyc_Parse_Unsigned_spec_struct*_tmpB17;yyval=Cyc_YY23((void*)((_tmpB17=
_cycalloc(sizeof(*_tmpB17)),((_tmpB17[0]=((_tmpB18.tag=1,((_tmpB18.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB18)))),
_tmpB17)))));}break;case 81: _LL290: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 82: _LL291: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 83: _LL292: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84: _LL293:
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 85:
_LL294:{struct Cyc_Absyn_TupleType_struct _tmpB1B;struct Cyc_Absyn_TupleType_struct*
_tmpB1A;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB1A=_cycalloc(sizeof(*
_tmpB1A)),((_tmpB1A[0]=((_tmpB1B.tag=11,((_tmpB1B.f1=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))),_tmpB1B)))),_tmpB1A)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 86: _LL295:{struct Cyc_Absyn_RgnHandleType_struct
_tmpB1E;struct Cyc_Absyn_RgnHandleType_struct*_tmpB1D;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D[0]=((_tmpB1E.tag=16,((
_tmpB1E.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpB1E)))),_tmpB1D)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 87:
_LL296:{struct Cyc_Absyn_RgnHandleType_struct _tmpB21;struct Cyc_Absyn_RgnHandleType_struct*
_tmpB20;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB20=_cycalloc(sizeof(*
_tmpB20)),((_tmpB20[0]=((_tmpB21.tag=16,((_tmpB21.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0),_tmpB21)))),_tmpB20)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 88:
_LL297: {void*_tmp56F=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0);{
struct Cyc_Absyn_DynRgnType_struct _tmpB24;struct Cyc_Absyn_DynRgnType_struct*
_tmpB23;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB23=_cycalloc(sizeof(*
_tmpB23)),((_tmpB23[0]=((_tmpB24.tag=17,((_tmpB24.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB24.f2=(void*)
_tmp56F,_tmpB24)))))),_tmpB23)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case 89:
_LL298:{struct Cyc_Absyn_DynRgnType_struct _tmpB27;struct Cyc_Absyn_DynRgnType_struct*
_tmpB26;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB26=_cycalloc(sizeof(*
_tmpB26)),((_tmpB26[0]=((_tmpB27.tag=17,((_tmpB27.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB27.f2=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB27)))))),
_tmpB26)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 90: _LL299:{struct Cyc_Absyn_TagType_struct
_tmpB2A;struct Cyc_Absyn_TagType_struct*_tmpB29;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB29=_cycalloc(sizeof(*_tmpB29)),((_tmpB29[0]=((_tmpB2A.tag=20,((
_tmpB2A.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpB2A)))),_tmpB29)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 91:
_LL29A:{struct Cyc_Absyn_TagType_struct _tmpB2D;struct Cyc_Absyn_TagType_struct*
_tmpB2C;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB2C=_cycalloc(sizeof(*
_tmpB2C)),((_tmpB2C[0]=((_tmpB2D.tag=20,((_tmpB2D.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_iko,0),_tmpB2D)))),_tmpB2C)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 92:
_LL29B:{struct Cyc_Absyn_ValueofType_struct _tmpB30;struct Cyc_Absyn_ValueofType_struct*
_tmpB2F;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB2F=_cycalloc(sizeof(*
_tmpB2F)),((_tmpB2F[0]=((_tmpB30.tag=19,((_tmpB30.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB30)))),_tmpB2F)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 93: _LL29C: yyval=Cyc_YY43(Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 94: _LL29D: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;{struct Cyc_Absyn_Tqual
_tmpB31;yyval=Cyc_YY25(((_tmpB31.print_const=1,((_tmpB31.q_volatile=0,((_tmpB31.q_restrict=
0,((_tmpB31.real_const=1,((_tmpB31.loc=loc,_tmpB31)))))))))));}break;}case 95:
_LL29E:{struct Cyc_Absyn_Tqual _tmpB32;yyval=Cyc_YY25(((_tmpB32.print_const=0,((
_tmpB32.q_volatile=1,((_tmpB32.q_restrict=0,((_tmpB32.real_const=0,((_tmpB32.loc=
0,_tmpB32)))))))))));}break;case 96: _LL29F:{struct Cyc_Absyn_Tqual _tmpB33;yyval=
Cyc_YY25(((_tmpB33.print_const=0,((_tmpB33.q_volatile=0,((_tmpB33.q_restrict=1,((
_tmpB33.real_const=0,((_tmpB33.loc=0,_tmpB33)))))))))));}break;case 97: _LL2A0:{
struct Cyc_Parse_Decl_spec_struct _tmpB4F;struct Cyc_Absyn_Enum_d_struct*_tmpB4E;
struct Cyc_Absyn_Enumdecl*_tmpB4D;struct Cyc_Core_Opt*_tmpB4C;struct Cyc_Absyn_Enum_d_struct
_tmpB4B;struct Cyc_Absyn_Decl*_tmpB4A;struct Cyc_Parse_Decl_spec_struct*_tmpB49;
yyval=Cyc_YY23((void*)((_tmpB49=_cycalloc(sizeof(*_tmpB49)),((_tmpB49[0]=((
_tmpB4F.tag=5,((_tmpB4F.f1=((_tmpB4A=_cycalloc(sizeof(*_tmpB4A)),((_tmpB4A->r=(
void*)((_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E[0]=((_tmpB4B.tag=8,((
_tmpB4B.f1=((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D->sc=Cyc_Absyn_Public,((
_tmpB4D->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpB4D->fields=((_tmpB4C=_cycalloc(sizeof(*_tmpB4C)),((
_tmpB4C->v=Cyc_yyget_YY48(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),_tmpB4C)))),_tmpB4D)))))))),_tmpB4B)))),_tmpB4E)))),((_tmpB4A->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB4A)))))),
_tmpB4F)))),_tmpB49)))));}break;case 98: _LL2A1:{struct Cyc_Absyn_EnumType_struct
_tmpB52;struct Cyc_Absyn_EnumType_struct*_tmpB51;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB51=_cycalloc(sizeof(*_tmpB51)),((_tmpB51[0]=((_tmpB52.tag=14,((
_tmpB52.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB52.f2=0,_tmpB52)))))),_tmpB51)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 99:
_LL2A2:{struct Cyc_Parse_Type_spec_struct _tmpB5C;struct Cyc_Absyn_AnonEnumType_struct
_tmpB5B;struct Cyc_Absyn_AnonEnumType_struct*_tmpB5A;struct Cyc_Parse_Type_spec_struct*
_tmpB59;yyval=Cyc_YY23((void*)((_tmpB59=_cycalloc(sizeof(*_tmpB59)),((_tmpB59[0]=((
_tmpB5C.tag=4,((_tmpB5C.f1=(void*)((void*)((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((
_tmpB5A[0]=((_tmpB5B.tag=15,((_tmpB5B.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB5B)))),_tmpB5A))))),((
_tmpB5C.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB5C)))))),_tmpB59)))));}break;case 100: _LL2A3:{
struct Cyc_Absyn_Enumfield*_tmpB5D;yyval=Cyc_YY47(((_tmpB5D=_cycalloc(sizeof(*
_tmpB5D)),((_tmpB5D->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB5D->tag=0,((_tmpB5D->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB5D)))))))));}break;case 101: _LL2A4:{struct Cyc_Absyn_Enumfield*
_tmpB5E;yyval=Cyc_YY47(((_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((_tmpB5E->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB5E->tag=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB5E->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB5E)))))))));}
break;case 102: _LL2A5:{struct Cyc_List_List*_tmpB5F;yyval=Cyc_YY48(((_tmpB5F=
_cycalloc(sizeof(*_tmpB5F)),((_tmpB5F->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB5F->tl=0,_tmpB5F)))))));}
break;case 103: _LL2A6:{struct Cyc_List_List*_tmpB60;yyval=Cyc_YY48(((_tmpB60=
_cycalloc(sizeof(*_tmpB60)),((_tmpB60->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB60->tl=0,_tmpB60)))))));}
break;case 104: _LL2A7:{struct Cyc_List_List*_tmpB61;yyval=Cyc_YY48(((_tmpB61=
_cycalloc(sizeof(*_tmpB61)),((_tmpB61->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB61->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB61)))))));}break;
case 105: _LL2A8:{struct Cyc_Absyn_AnonAggrType_struct _tmpB64;struct Cyc_Absyn_AnonAggrType_struct*
_tmpB63;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB63=_cycalloc(sizeof(*
_tmpB63)),((_tmpB63[0]=((_tmpB64.tag=13,((_tmpB64.f1=Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB64.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB64)))))),
_tmpB63)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 106: _LL2A9: {struct Cyc_List_List*_tmp591=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp592=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct
_tmpB67;struct Cyc_Parse_Decl_spec_struct*_tmpB66;yyval=Cyc_YY23((void*)((_tmpB66=
_cycalloc(sizeof(*_tmpB66)),((_tmpB66[0]=((_tmpB67.tag=5,((_tmpB67.f1=Cyc_Absyn_aggr_decl(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tmp591,(struct Cyc_Absyn_AggrdeclImpl*)Cyc_Absyn_aggrdecl_impl(_tmp592,Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB67)))),
_tmpB66)))));}break;}case 107: _LL2AA: {struct Cyc_List_List*_tmp595=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp596=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct _tmpB6A;struct Cyc_Parse_Decl_spec_struct*
_tmpB69;yyval=Cyc_YY23((void*)((_tmpB69=_cycalloc(sizeof(*_tmpB69)),((_tmpB69[0]=((
_tmpB6A.tag=5,((_tmpB6A.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp595,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp596,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB6A)))),
_tmpB69)))));}break;}case 108: _LL2AB:{struct Cyc_Absyn_AggrType_struct _tmpB74;
struct Cyc_Core_Opt*_tmpB73;struct Cyc_Absyn_AggrInfo _tmpB72;struct Cyc_Absyn_AggrType_struct*
_tmpB71;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB71=_cycalloc(sizeof(*
_tmpB71)),((_tmpB71[0]=((_tmpB74.tag=12,((_tmpB74.f1=((_tmpB72.aggr_info=Cyc_Absyn_UnknownAggr(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB73=
_cycalloc_atomic(sizeof(*_tmpB73)),((_tmpB73->v=(void*)1,_tmpB73))))),((_tmpB72.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB72)))),
_tmpB74)))),_tmpB71)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 109:
_LL2AC:{struct Cyc_Absyn_AggrType_struct _tmpB7A;struct Cyc_Absyn_AggrInfo _tmpB79;
struct Cyc_Absyn_AggrType_struct*_tmpB78;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((
_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78[0]=((_tmpB7A.tag=12,((_tmpB7A.f1=((
_tmpB79.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),((_tmpB79.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB79)))),
_tmpB7A)))),_tmpB78)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 110:
_LL2AD: yyval=Cyc_YY40(0);break;case 111: _LL2AE: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 112: _LL2AF:
yyval=Cyc_YY24(Cyc_Absyn_StructA);break;case 113: _LL2B0: yyval=Cyc_YY24(Cyc_Absyn_UnionA);
break;case 114: _LL2B1: yyval=Cyc_YY26(0);break;case 115: _LL2B2: {struct Cyc_List_List*
_tmp5A0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));struct Cyc_List_List*
_tmp5A1=Cyc_Parse_get_argrfield_tags(_tmp5A0);if(_tmp5A1 != 0)((void(*)(void(*f)(
struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct
Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp5A1,
_tmp5A0);yyval=Cyc_YY26(_tmp5A0);break;}case 116: _LL2B3:{struct Cyc_List_List*
_tmpB7B;yyval=Cyc_YY27(((_tmpB7B=_cycalloc(sizeof(*_tmpB7B)),((_tmpB7B->hd=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB7B->tl=0,
_tmpB7B)))))));}break;case 117: _LL2B4:{struct Cyc_List_List*_tmpB7C;yyval=Cyc_YY27(((
_tmpB7C=_cycalloc(sizeof(*_tmpB7C)),((_tmpB7C->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB7C->tl=Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB7C)))))));}
break;case 118: _LL2B5: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 119: _LL2B6:{struct Cyc_List_List*_tmpB7D;yyval=Cyc_YY21(((
_tmpB7D=_cycalloc(sizeof(*_tmpB7D)),((_tmpB7D->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB7D->tl=0,_tmpB7D)))))));}
break;case 120: _LL2B7:{struct Cyc_List_List*_tmpB7E;yyval=Cyc_YY21(((_tmpB7E=
_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB7E->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB7E)))))));}
break;case 121: _LL2B8:{struct _tuple21*_tmpB7F;yyval=Cyc_YY20(((_tmpB7F=_cycalloc(
sizeof(*_tmpB7F)),((_tmpB7F->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB7F->f2=0,_tmpB7F)))))));}
break;case 122: _LL2B9:{struct _tuple21*_tmpB80;yyval=Cyc_YY20(((_tmpB80=_cycalloc(
sizeof(*_tmpB80)),((_tmpB80->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB80->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB80)))))));}
break;case 123: _LL2BA: {struct _RegionHandle _tmp5A8=_new_region("temp");struct
_RegionHandle*temp=& _tmp5A8;_push_region(temp);{struct _tuple22 _tmp5AA;struct Cyc_Absyn_Tqual
_tmp5AB;struct Cyc_List_List*_tmp5AC;struct Cyc_List_List*_tmp5AD;struct _tuple22*
_tmp5A9=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5AA=*_tmp5A9;_tmp5AB=_tmp5AA.f1;_tmp5AC=_tmp5AA.f2;_tmp5AD=_tmp5AA.f3;if(
_tmp5AB.loc == 0)_tmp5AB.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);{struct Cyc_List_List*
_tmp5AF;struct Cyc_List_List*_tmp5B0;struct _tuple0 _tmp5AE=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
temp,temp,Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]));_tmp5AF=_tmp5AE.f1;_tmp5B0=_tmp5AE.f2;{void*_tmp5B1=Cyc_Parse_speclist2typ(
_tmp5AC,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp5B2=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(temp,_tmp5AB,_tmp5B1,_tmp5AF,
_tmp5AD),_tmp5B0);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple14*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp5B2));};};}
_npop_handler(0);break;;_pop_region(temp);}case 124: _LL2BB:{struct Cyc_List_List*
_tmpB83;struct _tuple22*_tmpB82;yyval=Cyc_YY35(((_tmpB82=_cycalloc(sizeof(*
_tmpB82)),((_tmpB82->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB82->f2=((
_tmpB83=_cycalloc(sizeof(*_tmpB83)),((_tmpB83->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB83->tl=0,_tmpB83)))))),((
_tmpB82->f3=0,_tmpB82)))))))));}break;case 125: _LL2BC:{struct Cyc_List_List*
_tmpB86;struct _tuple22*_tmpB85;yyval=Cyc_YY35(((_tmpB85=_cycalloc(sizeof(*
_tmpB85)),((_tmpB85->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpB85->f2=((_tmpB86=_cycalloc(sizeof(*_tmpB86)),((
_tmpB86->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB86->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpB86)))))),((_tmpB85->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB85)))))))));}break;case 126: _LL2BD:{struct _tuple22*_tmpB87;yyval=Cyc_YY35(((
_tmpB87=_cycalloc(sizeof(*_tmpB87)),((_tmpB87->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB87->f2=0,((_tmpB87->f3=
0,_tmpB87)))))))));}break;case 127: _LL2BE:{struct _tuple22*_tmpB88;yyval=Cyc_YY35(((
_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpB88->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpB88->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpB88)))))))));}break;case 128: _LL2BF:{struct _tuple22*
_tmpB89;yyval=Cyc_YY35(((_tmpB89=_cycalloc(sizeof(*_tmpB89)),((_tmpB89->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB89->f2=0,((_tmpB89->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB89)))))))));}break;
case 129: _LL2C0:{struct _tuple22*_tmpB8A;yyval=Cyc_YY35(((_tmpB8A=_cycalloc(
sizeof(*_tmpB8A)),((_tmpB8A->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpB8A->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpB8A->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpB8A)))))))));}break;case 130: _LL2C1:{struct Cyc_List_List*_tmpB8D;struct
_tuple22*_tmpB8C;yyval=Cyc_YY35(((_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C->f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB8C->f2=((
_tmpB8D=_cycalloc(sizeof(*_tmpB8D)),((_tmpB8D->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB8D->tl=0,_tmpB8D)))))),((
_tmpB8C->f3=0,_tmpB8C)))))))));}break;case 131: _LL2C2:{struct Cyc_List_List*
_tmpB90;struct _tuple22*_tmpB8F;yyval=Cyc_YY35(((_tmpB8F=_cycalloc(sizeof(*
_tmpB8F)),((_tmpB8F->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpB8F->f2=((_tmpB90=_cycalloc(sizeof(*_tmpB90)),((
_tmpB90->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB90->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpB90)))))),((_tmpB8F->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB8F)))))))));}break;case 132: _LL2C3:{struct _tuple22*_tmpB91;yyval=Cyc_YY35(((
_tmpB91=_cycalloc(sizeof(*_tmpB91)),((_tmpB91->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB91->f2=0,((_tmpB91->f3=
0,_tmpB91)))))))));}break;case 133: _LL2C4:{struct _tuple22*_tmpB92;yyval=Cyc_YY35(((
_tmpB92=_cycalloc(sizeof(*_tmpB92)),((_tmpB92->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpB92->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpB92->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpB92)))))))));}break;case 134: _LL2C5:{struct _tuple22*
_tmpB93;yyval=Cyc_YY35(((_tmpB93=_cycalloc(sizeof(*_tmpB93)),((_tmpB93->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB93->f2=0,((_tmpB93->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB93)))))))));}break;
case 135: _LL2C6:{struct _tuple22*_tmpB94;yyval=Cyc_YY35(((_tmpB94=_cycalloc(
sizeof(*_tmpB94)),((_tmpB94->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpB94->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpB94->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpB94)))))))));}break;case 136: _LL2C7: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 137: _LL2C8:{
struct Cyc_List_List*_tmpB95;yyval=Cyc_YY21(((_tmpB95=_cycalloc(sizeof(*_tmpB95)),((
_tmpB95->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB95->tl=0,_tmpB95)))))));}break;case 138: _LL2C9:{struct Cyc_List_List*
_tmpB96;yyval=Cyc_YY21(((_tmpB96=_cycalloc(sizeof(*_tmpB96)),((_tmpB96->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB96->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB96)))))));}
break;case 139: _LL2CA:{struct _tuple21*_tmpB97;yyval=Cyc_YY20(((_tmpB97=_cycalloc(
sizeof(*_tmpB97)),((_tmpB97->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB97->f2=0,_tmpB97)))))));}
break;case 140: _LL2CB:{struct Cyc_Parse_Declarator*_tmpB9D;struct _tuple1*_tmpB9C;
struct _tuple21*_tmpB9B;yyval=Cyc_YY20(((_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((
_tmpB9B->f1=((_tmpB9D=_cycalloc(sizeof(*_tmpB9D)),((_tmpB9D->id=((_tmpB9C=
_cycalloc(sizeof(*_tmpB9C)),((_tmpB9C->f1=Cyc_Absyn_Rel_n(0),((_tmpB9C->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),
_tmpB9C)))))),((_tmpB9D->tms=0,_tmpB9D)))))),((_tmpB9B->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB9B)))))));}
break;case 141: _LL2CC:{struct _tuple21*_tmpB9E;yyval=Cyc_YY20(((_tmpB9E=_cycalloc(
sizeof(*_tmpB9E)),((_tmpB9E->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB9E->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB9E)))))));}
break;case 142: _LL2CD: {int _tmp5CB=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);struct Cyc_List_List*
_tmp5CC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct _tmpBA4;struct Cyc_Core_Opt*
_tmpBA3;struct Cyc_Parse_Decl_spec_struct*_tmpBA2;yyval=Cyc_YY23((void*)((_tmpBA2=
_cycalloc(sizeof(*_tmpBA2)),((_tmpBA2[0]=((_tmpBA4.tag=5,((_tmpBA4.f1=Cyc_Absyn_datatype_decl(
Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),_tmp5CC,((_tmpBA3=_cycalloc(sizeof(*_tmpBA3)),((_tmpBA3->v=
Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpBA3)))),_tmp5CB,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBA4)))),
_tmpBA2)))));}break;}case 143: _LL2CE: {int _tmp5D0=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);{struct Cyc_Absyn_DatatypeType_struct
_tmpBAE;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBAD;struct Cyc_Absyn_DatatypeInfo
_tmpBAC;struct Cyc_Absyn_DatatypeType_struct*_tmpBAB;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpBAB=_cycalloc(sizeof(*_tmpBAB)),((_tmpBAB[0]=((_tmpBAE.tag=3,((
_tmpBAE.f1=((_tmpBAC.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpBAD.name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBAD.is_extensible=
_tmp5D0,_tmpBAD))))),((_tmpBAC.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBAC)))),_tmpBAE)))),
_tmpBAB)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 144: _LL2CF: {int _tmp5D5=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);{struct Cyc_Absyn_DatatypeFieldType_struct
_tmpBB8;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBB7;struct Cyc_Absyn_DatatypeFieldInfo
_tmpBB6;struct Cyc_Absyn_DatatypeFieldType_struct*_tmpBB5;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpBB5=_cycalloc(sizeof(*_tmpBB5)),((_tmpBB5[0]=((_tmpBB8.tag=4,((
_tmpBB8.f1=((_tmpBB6.field_info=Cyc_Absyn_UnknownDatatypefield(((_tmpBB7.datatype_name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpBB7.field_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpBB7.is_extensible=_tmp5D5,_tmpBB7))))))),((
_tmpBB6.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpBB6)))),_tmpBB8)))),_tmpBB5)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
145: _LL2D0: yyval=Cyc_YY31(0);break;case 146: _LL2D1: yyval=Cyc_YY31(1);break;case
147: _LL2D2:{struct Cyc_List_List*_tmpBB9;yyval=Cyc_YY34(((_tmpBB9=_cycalloc(
sizeof(*_tmpBB9)),((_tmpBB9->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBB9->tl=0,_tmpBB9)))))));}
break;case 148: _LL2D3:{struct Cyc_List_List*_tmpBBA;yyval=Cyc_YY34(((_tmpBBA=
_cycalloc(sizeof(*_tmpBBA)),((_tmpBBA->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBBA->tl=0,_tmpBBA)))))));}
break;case 149: _LL2D4:{struct Cyc_List_List*_tmpBBB;yyval=Cyc_YY34(((_tmpBBB=
_cycalloc(sizeof(*_tmpBBB)),((_tmpBBB->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBBB->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBBB)))))));}break;
case 150: _LL2D5:{struct Cyc_List_List*_tmpBBC;yyval=Cyc_YY34(((_tmpBBC=_cycalloc(
sizeof(*_tmpBBC)),((_tmpBBC->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBBC->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBBC)))))));}break;
case 151: _LL2D6: yyval=Cyc_YY32(Cyc_Absyn_Public);break;case 152: _LL2D7: yyval=Cyc_YY32(
Cyc_Absyn_Extern);break;case 153: _LL2D8: yyval=Cyc_YY32(Cyc_Absyn_Static);break;
case 154: _LL2D9:{struct Cyc_Absyn_Datatypefield*_tmpBBD;yyval=Cyc_YY33(((_tmpBBD=
_cycalloc(sizeof(*_tmpBBD)),((_tmpBBD->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBBD->typs=0,((_tmpBBD->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpBBD->sc=Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBBD)))))))))));}
break;case 155: _LL2DA: {struct Cyc_List_List*_tmp5DF=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Absyn_Datatypefield*
_tmpBBE;yyval=Cyc_YY33(((_tmpBBE=_cycalloc(sizeof(*_tmpBBE)),((_tmpBBE->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpBBE->typs=
_tmp5DF,((_tmpBBE->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpBBE->sc=
Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmpBBE)))))))))));}break;}case 156: _LL2DB: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 157: _LL2DC:{struct
Cyc_Parse_Declarator*_tmpBBF;yyval=Cyc_YY29(((_tmpBBF=_cycalloc(sizeof(*_tmpBBF)),((
_tmpBBF->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id,((_tmpBBF->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpBBF)))))));}
break;case 158: _LL2DD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 159: _LL2DE:{struct Cyc_Parse_Declarator*_tmpBC0;yyval=
Cyc_YY29(((_tmpBC0=_cycalloc(sizeof(*_tmpBC0)),((_tmpBC0->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->id,((_tmpBC0->tms=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpBC0)))))));}
break;case 160: _LL2DF:{struct Cyc_Parse_Declarator*_tmpBC1;yyval=Cyc_YY29(((
_tmpBC1=_cycalloc(sizeof(*_tmpBC1)),((_tmpBC1->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBC1->tms=0,_tmpBC1)))))));}
break;case 161: _LL2E0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 162: _LL2E1: {struct Cyc_Parse_Declarator*_tmp5E4=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpBC7;struct Cyc_Absyn_Attributes_mod_struct _tmpBC6;struct Cyc_List_List*_tmpBC5;
_tmp5E4->tms=((_tmpBC5=_cycalloc(sizeof(*_tmpBC5)),((_tmpBC5->hd=(void*)((
_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7[0]=((_tmpBC6.tag=5,((_tmpBC6.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpBC6.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpBC6)))))),_tmpBC7)))),((
_tmpBC5->tl=_tmp5E4->tms,_tmpBC5))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 163: _LL2E2:{
struct Cyc_List_List*_tmpBD1;struct Cyc_Absyn_Carray_mod_struct _tmpBD0;struct Cyc_Absyn_Carray_mod_struct*
_tmpBCF;struct Cyc_Parse_Declarator*_tmpBCE;yyval=Cyc_YY29(((_tmpBCE=_cycalloc(
sizeof(*_tmpBCE)),((_tmpBCE->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBCE->tms=((
_tmpBD1=_cycalloc(sizeof(*_tmpBD1)),((_tmpBD1->hd=(void*)((_tmpBCF=_cycalloc(
sizeof(*_tmpBCF)),((_tmpBCF[0]=((_tmpBD0.tag=0,((_tmpBD0.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBD0.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBD0)))))),
_tmpBCF)))),((_tmpBD1->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpBD1)))))),_tmpBCE)))))));}break;case 164:
_LL2E3:{struct Cyc_List_List*_tmpBDB;struct Cyc_Absyn_ConstArray_mod_struct _tmpBDA;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpBD9;struct Cyc_Parse_Declarator*_tmpBD8;
yyval=Cyc_YY29(((_tmpBD8=_cycalloc(sizeof(*_tmpBD8)),((_tmpBD8->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpBD8->tms=((
_tmpBDB=_cycalloc(sizeof(*_tmpBDB)),((_tmpBDB->hd=(void*)((_tmpBD9=_cycalloc(
sizeof(*_tmpBD9)),((_tmpBD9[0]=((_tmpBDA.tag=1,((_tmpBDA.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBDA.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBDA.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBDA)))))))),
_tmpBD9)))),((_tmpBDB->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpBDB)))))),_tmpBD8)))))));}break;case 165:
_LL2E4: {struct _tuple23 _tmp5F1;struct Cyc_List_List*_tmp5F2;int _tmp5F3;struct Cyc_Absyn_VarargInfo*
_tmp5F4;struct Cyc_Core_Opt*_tmp5F5;struct Cyc_List_List*_tmp5F6;struct _tuple23*
_tmp5F0=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5F1=*_tmp5F0;_tmp5F2=_tmp5F1.f1;_tmp5F3=_tmp5F1.f2;_tmp5F4=_tmp5F1.f3;
_tmp5F5=_tmp5F1.f4;_tmp5F6=_tmp5F1.f5;{struct Cyc_List_List*_tmpBF0;struct Cyc_Absyn_Function_mod_struct
_tmpBEF;struct Cyc_Absyn_WithTypes_struct _tmpBEE;struct Cyc_Absyn_WithTypes_struct*
_tmpBED;struct Cyc_Absyn_Function_mod_struct*_tmpBEC;struct Cyc_Parse_Declarator*
_tmpBEB;yyval=Cyc_YY29(((_tmpBEB=_cycalloc(sizeof(*_tmpBEB)),((_tmpBEB->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBEB->tms=((
_tmpBF0=_cycalloc(sizeof(*_tmpBF0)),((_tmpBF0->hd=(void*)((_tmpBEC=_cycalloc(
sizeof(*_tmpBEC)),((_tmpBEC[0]=((_tmpBEF.tag=3,((_tmpBEF.f1=(void*)((void*)((
_tmpBED=_cycalloc(sizeof(*_tmpBED)),((_tmpBED[0]=((_tmpBEE.tag=1,((_tmpBEE.f1=
_tmp5F2,((_tmpBEE.f2=_tmp5F3,((_tmpBEE.f3=_tmp5F4,((_tmpBEE.f4=_tmp5F5,((_tmpBEE.f5=
_tmp5F6,_tmpBEE)))))))))))),_tmpBED))))),_tmpBEF)))),_tmpBEC)))),((_tmpBF0->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpBF0)))))),_tmpBEB)))))));}break;}case 166: _LL2E5:{struct Cyc_List_List*_tmpC05;
struct Cyc_Absyn_Function_mod_struct _tmpC04;struct Cyc_Absyn_WithTypes_struct
_tmpC03;struct Cyc_Absyn_WithTypes_struct*_tmpC02;struct Cyc_Absyn_Function_mod_struct*
_tmpC01;struct Cyc_Parse_Declarator*_tmpC00;yyval=Cyc_YY29(((_tmpC00=_cycalloc(
sizeof(*_tmpC00)),((_tmpC00->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpC00->tms=((
_tmpC05=_cycalloc(sizeof(*_tmpC05)),((_tmpC05->hd=(void*)((_tmpC01=_cycalloc(
sizeof(*_tmpC01)),((_tmpC01[0]=((_tmpC04.tag=3,((_tmpC04.f1=(void*)((void*)((
_tmpC02=_cycalloc(sizeof(*_tmpC02)),((_tmpC02[0]=((_tmpC03.tag=1,((_tmpC03.f1=0,((
_tmpC03.f2=0,((_tmpC03.f3=0,((_tmpC03.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC03.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC03)))))))))))),
_tmpC02))))),_tmpC04)))),_tmpC01)))),((_tmpC05->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpC05)))))),
_tmpC00)))))));}break;case 167: _LL2E6:{struct Cyc_List_List*_tmpC1A;struct Cyc_Absyn_Function_mod_struct
_tmpC19;struct Cyc_Absyn_NoTypes_struct _tmpC18;struct Cyc_Absyn_NoTypes_struct*
_tmpC17;struct Cyc_Absyn_Function_mod_struct*_tmpC16;struct Cyc_Parse_Declarator*
_tmpC15;yyval=Cyc_YY29(((_tmpC15=_cycalloc(sizeof(*_tmpC15)),((_tmpC15->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC15->tms=((
_tmpC1A=_cycalloc(sizeof(*_tmpC1A)),((_tmpC1A->hd=(void*)((_tmpC16=_cycalloc(
sizeof(*_tmpC16)),((_tmpC16[0]=((_tmpC19.tag=3,((_tmpC19.f1=(void*)((void*)((
_tmpC17=_cycalloc(sizeof(*_tmpC17)),((_tmpC17[0]=((_tmpC18.tag=0,((_tmpC18.f1=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpC18.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpC18)))))),_tmpC17))))),_tmpC19)))),_tmpC16)))),((
_tmpC1A->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms,_tmpC1A)))))),_tmpC15)))))));}break;case 168: _LL2E7: {
struct Cyc_List_List*_tmp609=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpC24;struct Cyc_Absyn_TypeParams_mod_struct _tmpC23;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpC22;struct Cyc_Parse_Declarator*_tmpC21;yyval=Cyc_YY29(((_tmpC21=_cycalloc(
sizeof(*_tmpC21)),((_tmpC21->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC21->tms=((
_tmpC24=_cycalloc(sizeof(*_tmpC24)),((_tmpC24->hd=(void*)((_tmpC22=_cycalloc(
sizeof(*_tmpC22)),((_tmpC22[0]=((_tmpC23.tag=4,((_tmpC23.f1=_tmp609,((_tmpC23.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpC23.f3=0,_tmpC23)))))))),_tmpC22)))),((_tmpC24->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC24)))))),_tmpC21)))))));}break;}case 169: _LL2E8:{struct Cyc_List_List*_tmpC2E;
struct Cyc_Absyn_Attributes_mod_struct _tmpC2D;struct Cyc_Absyn_Attributes_mod_struct*
_tmpC2C;struct Cyc_Parse_Declarator*_tmpC2B;yyval=Cyc_YY29(((_tmpC2B=_cycalloc(
sizeof(*_tmpC2B)),((_tmpC2B->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpC2B->tms=((
_tmpC2E=_cycalloc(sizeof(*_tmpC2E)),((_tmpC2E->hd=(void*)((_tmpC2C=_cycalloc(
sizeof(*_tmpC2C)),((_tmpC2C[0]=((_tmpC2D.tag=5,((_tmpC2D.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC2D.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC2D)))))),_tmpC2C)))),((
_tmpC2E->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpC2E)))))),_tmpC2B)))))));}break;case 170: _LL2E9:{
struct Cyc_Parse_Declarator*_tmpC2F;yyval=Cyc_YY29(((_tmpC2F=_cycalloc(sizeof(*
_tmpC2F)),((_tmpC2F->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC2F->tms=0,_tmpC2F)))))));}break;case 171: _LL2EA:{
struct Cyc_Parse_Declarator*_tmpC30;yyval=Cyc_YY29(((_tmpC30=_cycalloc(sizeof(*
_tmpC30)),((_tmpC30->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC30->tms=0,_tmpC30)))))));}break;case 172: _LL2EB:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 173:
_LL2EC: {struct Cyc_Parse_Declarator*_tmp614=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpC36;struct Cyc_Absyn_Attributes_mod_struct _tmpC35;struct Cyc_List_List*_tmpC34;
_tmp614->tms=((_tmpC34=_cycalloc(sizeof(*_tmpC34)),((_tmpC34->hd=(void*)((
_tmpC36=_cycalloc(sizeof(*_tmpC36)),((_tmpC36[0]=((_tmpC35.tag=5,((_tmpC35.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpC35.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC35)))))),_tmpC36)))),((
_tmpC34->tl=_tmp614->tms,_tmpC34))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 174: _LL2ED:{
struct Cyc_List_List*_tmpC40;struct Cyc_Absyn_Carray_mod_struct _tmpC3F;struct Cyc_Absyn_Carray_mod_struct*
_tmpC3E;struct Cyc_Parse_Declarator*_tmpC3D;yyval=Cyc_YY29(((_tmpC3D=_cycalloc(
sizeof(*_tmpC3D)),((_tmpC3D->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC3D->tms=((
_tmpC40=_cycalloc(sizeof(*_tmpC40)),((_tmpC40->hd=(void*)((_tmpC3E=_cycalloc(
sizeof(*_tmpC3E)),((_tmpC3E[0]=((_tmpC3F.tag=0,((_tmpC3F.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC3F.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC3F)))))),
_tmpC3E)))),((_tmpC40->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpC40)))))),_tmpC3D)))))));}break;case 175:
_LL2EE:{struct Cyc_List_List*_tmpC4A;struct Cyc_Absyn_ConstArray_mod_struct _tmpC49;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpC48;struct Cyc_Parse_Declarator*_tmpC47;
yyval=Cyc_YY29(((_tmpC47=_cycalloc(sizeof(*_tmpC47)),((_tmpC47->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpC47->tms=((
_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A->hd=(void*)((_tmpC48=_cycalloc(
sizeof(*_tmpC48)),((_tmpC48[0]=((_tmpC49.tag=1,((_tmpC49.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC49.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC49.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC49)))))))),
_tmpC48)))),((_tmpC4A->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpC4A)))))),_tmpC47)))))));}break;case 176:
_LL2EF: {struct _tuple23 _tmp621;struct Cyc_List_List*_tmp622;int _tmp623;struct Cyc_Absyn_VarargInfo*
_tmp624;struct Cyc_Core_Opt*_tmp625;struct Cyc_List_List*_tmp626;struct _tuple23*
_tmp620=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp621=*_tmp620;_tmp622=_tmp621.f1;_tmp623=_tmp621.f2;_tmp624=_tmp621.f3;
_tmp625=_tmp621.f4;_tmp626=_tmp621.f5;{struct Cyc_List_List*_tmpC5F;struct Cyc_Absyn_Function_mod_struct
_tmpC5E;struct Cyc_Absyn_WithTypes_struct _tmpC5D;struct Cyc_Absyn_WithTypes_struct*
_tmpC5C;struct Cyc_Absyn_Function_mod_struct*_tmpC5B;struct Cyc_Parse_Declarator*
_tmpC5A;yyval=Cyc_YY29(((_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((_tmpC5A->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC5A->tms=((
_tmpC5F=_cycalloc(sizeof(*_tmpC5F)),((_tmpC5F->hd=(void*)((_tmpC5B=_cycalloc(
sizeof(*_tmpC5B)),((_tmpC5B[0]=((_tmpC5E.tag=3,((_tmpC5E.f1=(void*)((void*)((
_tmpC5C=_cycalloc(sizeof(*_tmpC5C)),((_tmpC5C[0]=((_tmpC5D.tag=1,((_tmpC5D.f1=
_tmp622,((_tmpC5D.f2=_tmp623,((_tmpC5D.f3=_tmp624,((_tmpC5D.f4=_tmp625,((_tmpC5D.f5=
_tmp626,_tmpC5D)))))))))))),_tmpC5C))))),_tmpC5E)))),_tmpC5B)))),((_tmpC5F->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC5F)))))),_tmpC5A)))))));}break;}case 177: _LL2F0:{struct Cyc_List_List*_tmpC74;
struct Cyc_Absyn_Function_mod_struct _tmpC73;struct Cyc_Absyn_WithTypes_struct
_tmpC72;struct Cyc_Absyn_WithTypes_struct*_tmpC71;struct Cyc_Absyn_Function_mod_struct*
_tmpC70;struct Cyc_Parse_Declarator*_tmpC6F;yyval=Cyc_YY29(((_tmpC6F=_cycalloc(
sizeof(*_tmpC6F)),((_tmpC6F->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpC6F->tms=((
_tmpC74=_cycalloc(sizeof(*_tmpC74)),((_tmpC74->hd=(void*)((_tmpC70=_cycalloc(
sizeof(*_tmpC70)),((_tmpC70[0]=((_tmpC73.tag=3,((_tmpC73.f1=(void*)((void*)((
_tmpC71=_cycalloc(sizeof(*_tmpC71)),((_tmpC71[0]=((_tmpC72.tag=1,((_tmpC72.f1=0,((
_tmpC72.f2=0,((_tmpC72.f3=0,((_tmpC72.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC72.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC72)))))))))))),
_tmpC71))))),_tmpC73)))),_tmpC70)))),((_tmpC74->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpC74)))))),
_tmpC6F)))))));}break;case 178: _LL2F1:{struct Cyc_List_List*_tmpC89;struct Cyc_Absyn_Function_mod_struct
_tmpC88;struct Cyc_Absyn_NoTypes_struct _tmpC87;struct Cyc_Absyn_NoTypes_struct*
_tmpC86;struct Cyc_Absyn_Function_mod_struct*_tmpC85;struct Cyc_Parse_Declarator*
_tmpC84;yyval=Cyc_YY29(((_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC84->tms=((
_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89->hd=(void*)((_tmpC85=_cycalloc(
sizeof(*_tmpC85)),((_tmpC85[0]=((_tmpC88.tag=3,((_tmpC88.f1=(void*)((void*)((
_tmpC86=_cycalloc(sizeof(*_tmpC86)),((_tmpC86[0]=((_tmpC87.tag=0,((_tmpC87.f1=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpC87.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpC87)))))),_tmpC86))))),_tmpC88)))),_tmpC85)))),((
_tmpC89->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms,_tmpC89)))))),_tmpC84)))))));}break;case 179: _LL2F2: {
struct Cyc_List_List*_tmp639=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpC93;struct Cyc_Absyn_TypeParams_mod_struct _tmpC92;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpC91;struct Cyc_Parse_Declarator*_tmpC90;yyval=Cyc_YY29(((_tmpC90=_cycalloc(
sizeof(*_tmpC90)),((_tmpC90->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC90->tms=((
_tmpC93=_cycalloc(sizeof(*_tmpC93)),((_tmpC93->hd=(void*)((_tmpC91=_cycalloc(
sizeof(*_tmpC91)),((_tmpC91[0]=((_tmpC92.tag=4,((_tmpC92.f1=_tmp639,((_tmpC92.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpC92.f3=0,_tmpC92)))))))),_tmpC91)))),((_tmpC93->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC93)))))),_tmpC90)))))));}break;}case 180: _LL2F3:{struct Cyc_List_List*_tmpC9D;
struct Cyc_Absyn_Attributes_mod_struct _tmpC9C;struct Cyc_Absyn_Attributes_mod_struct*
_tmpC9B;struct Cyc_Parse_Declarator*_tmpC9A;yyval=Cyc_YY29(((_tmpC9A=_cycalloc(
sizeof(*_tmpC9A)),((_tmpC9A->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpC9A->tms=((
_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->hd=(void*)((_tmpC9B=_cycalloc(
sizeof(*_tmpC9B)),((_tmpC9B[0]=((_tmpC9C.tag=5,((_tmpC9C.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC9C.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC9C)))))),_tmpC9B)))),((
_tmpC9D->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpC9D)))))),_tmpC9A)))))));}break;case 181: _LL2F4:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 182:
_LL2F5: yyval=Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 183: _LL2F6: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){struct Cyc_Absyn_Attributes_mod_struct*
_tmpCA3;struct Cyc_Absyn_Attributes_mod_struct _tmpCA2;struct Cyc_List_List*_tmpCA1;
ans=((_tmpCA1=_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1->hd=(void*)((_tmpCA3=
_cycalloc(sizeof(*_tmpCA3)),((_tmpCA3[0]=((_tmpCA2.tag=5,((_tmpCA2.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpCA2.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpCA2)))))),_tmpCA3)))),((_tmpCA1->tl=ans,_tmpCA1))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;struct Cyc_Position_Segment*_tmp646;union Cyc_Absyn_Constraint*_tmp647;
union Cyc_Absyn_Constraint*_tmp648;struct _tuple18 _tmp645=*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp646=_tmp645.f1;
_tmp647=_tmp645.f2;_tmp648=_tmp645.f3;if(Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*
_tmpCA4;ptrloc=((_tmpCA4=_cycalloc(sizeof(*_tmpCA4)),((_tmpCA4->ptr_loc=_tmp646,((
_tmpCA4->rgn_loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpCA4->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),_tmpCA4))))))));}{union Cyc_Absyn_Constraint*
_tmp64B;union Cyc_Absyn_Constraint*_tmp64C;union Cyc_Absyn_Constraint*_tmp64D;void*
_tmp64E;struct _tuple12 _tmp64A=Cyc_Parse_collapse_pointer_quals(_tmp646,_tmp647,
_tmp648,Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY54(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
_tmp64B=_tmp64A.f1;_tmp64C=_tmp64A.f2;_tmp64D=_tmp64A.f3;_tmp64E=_tmp64A.f4;{
struct Cyc_Absyn_Pointer_mod_struct*_tmpCAE;struct Cyc_Absyn_PtrAtts _tmpCAD;struct
Cyc_Absyn_Pointer_mod_struct _tmpCAC;struct Cyc_List_List*_tmpCAB;ans=((_tmpCAB=
_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->hd=(void*)((_tmpCAE=_cycalloc(sizeof(*
_tmpCAE)),((_tmpCAE[0]=((_tmpCAC.tag=2,((_tmpCAC.f1=((_tmpCAD.rgn=_tmp64E,((
_tmpCAD.nullable=_tmp64B,((_tmpCAD.bounds=_tmp64C,((_tmpCAD.zero_term=_tmp64D,((
_tmpCAD.ptrloc=ptrloc,_tmpCAD)))))))))),((_tmpCAC.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCAC)))))),_tmpCAE)))),((
_tmpCAB->tl=ans,_tmpCAB))))));}yyval=Cyc_YY28(ans);break;};};}case 184: _LL2F7:
yyval=Cyc_YY54(0);break;case 185: _LL2F8:{struct Cyc_List_List*_tmpCAF;yyval=Cyc_YY54(((
_tmpCAF=_cycalloc(sizeof(*_tmpCAF)),((_tmpCAF->hd=Cyc_yyget_YY53(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCAF->tl=Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCAF)))))));}break;
case 186: _LL2F9:{struct Cyc_Absyn_Numelts_ptrqual_struct _tmpCB2;struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmpCB1;yyval=Cyc_YY53((void*)((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1[0]=((
_tmpCB2.tag=0,((_tmpCB2.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpCB2)))),_tmpCB1)))));}break;case 187: _LL2FA:{struct
Cyc_Absyn_Region_ptrqual_struct _tmpCB5;struct Cyc_Absyn_Region_ptrqual_struct*
_tmpCB4;yyval=Cyc_YY53((void*)((_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4[0]=((
_tmpCB5.tag=1,((_tmpCB5.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCB5)))),_tmpCB4)))));}
break;case 188: _LL2FB:{struct Cyc_Absyn_Thin_ptrqual_struct _tmpCB8;struct Cyc_Absyn_Thin_ptrqual_struct*
_tmpCB7;yyval=Cyc_YY53((void*)((_tmpCB7=_cycalloc_atomic(sizeof(*_tmpCB7)),((
_tmpCB7[0]=((_tmpCB8.tag=2,_tmpCB8)),_tmpCB7)))));}break;case 189: _LL2FC:{struct
Cyc_Absyn_Fat_ptrqual_struct _tmpCBB;struct Cyc_Absyn_Fat_ptrqual_struct*_tmpCBA;
yyval=Cyc_YY53((void*)((_tmpCBA=_cycalloc_atomic(sizeof(*_tmpCBA)),((_tmpCBA[0]=((
_tmpCBB.tag=3,_tmpCBB)),_tmpCBA)))));}break;case 190: _LL2FD:{struct Cyc_Absyn_Zeroterm_ptrqual_struct
_tmpCBE;struct Cyc_Absyn_Zeroterm_ptrqual_struct*_tmpCBD;yyval=Cyc_YY53((void*)((
_tmpCBD=_cycalloc_atomic(sizeof(*_tmpCBD)),((_tmpCBD[0]=((_tmpCBE.tag=4,_tmpCBE)),
_tmpCBD)))));}break;case 191: _LL2FE:{struct Cyc_Absyn_Nozeroterm_ptrqual_struct
_tmpCC1;struct Cyc_Absyn_Nozeroterm_ptrqual_struct*_tmpCC0;yyval=Cyc_YY53((void*)((
_tmpCC0=_cycalloc_atomic(sizeof(*_tmpCC0)),((_tmpCC0[0]=((_tmpCC1.tag=5,_tmpCC1)),
_tmpCC0)))));}break;case 192: _LL2FF:{struct Cyc_Absyn_Notnull_ptrqual_struct
_tmpCC4;struct Cyc_Absyn_Notnull_ptrqual_struct*_tmpCC3;yyval=Cyc_YY53((void*)((
_tmpCC3=_cycalloc_atomic(sizeof(*_tmpCC3)),((_tmpCC3[0]=((_tmpCC4.tag=6,_tmpCC4)),
_tmpCC3)))));}break;case 193: _LL300:{struct Cyc_Absyn_Nullable_ptrqual_struct
_tmpCC7;struct Cyc_Absyn_Nullable_ptrqual_struct*_tmpCC6;yyval=Cyc_YY53((void*)((
_tmpCC6=_cycalloc_atomic(sizeof(*_tmpCC6)),((_tmpCC6[0]=((_tmpCC7.tag=7,_tmpCC7)),
_tmpCC6)))));}break;case 194: _LL301: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple18*_tmpCC8;yyval=Cyc_YY1(((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8->f1=
loc,((_tmpCC8->f2=Cyc_Absyn_true_conref,((_tmpCC8->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCC8)))))))));}break;}
case 195: _LL302: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple18*_tmpCC9;yyval=Cyc_YY1(((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9->f1=
loc,((_tmpCC9->f2=Cyc_Absyn_false_conref,((_tmpCC9->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCC9)))))))));}break;}
case 196: _LL303: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{struct _tuple18*
_tmpCCA;yyval=Cyc_YY1(((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA->f1=loc,((
_tmpCCA->f2=Cyc_Absyn_true_conref,((_tmpCCA->f3=Cyc_Absyn_bounds_dyneither_conref,
_tmpCCA)))))))));}break;}case 197: _LL304: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 198: _LL305:{struct Cyc_Absyn_Upper_b_struct _tmpCCD;struct Cyc_Absyn_Upper_b_struct*
_tmpCCC;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC[0]=((_tmpCCD.tag=1,((
_tmpCCD.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpCCD)))),_tmpCCC))))));}break;case 199: _LL306: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)());break;case 200: _LL307: yyval=Cyc_YY51(Cyc_Absyn_true_conref);
break;case 201: _LL308: yyval=Cyc_YY51(Cyc_Absyn_false_conref);break;case 202: _LL309:
yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));
break;case 203: _LL30A: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_trk,1);yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 204: _LL30B:
yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));
break;case 205: _LL30C: yyval=Cyc_YY25(Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)));break;case
206: _LL30D: yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 207: _LL30E:{
struct _tuple23*_tmpCCE;yyval=Cyc_YY39(((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((
_tmpCCE->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((
_tmpCCE->f2=0,((_tmpCCE->f3=0,((_tmpCCE->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCCE->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCCE)))))))))))));}
break;case 208: _LL30F:{struct _tuple23*_tmpCCF;yyval=Cyc_YY39(((_tmpCCF=_cycalloc(
sizeof(*_tmpCCF)),((_tmpCCF->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),((_tmpCCF->f2=1,((_tmpCCF->f3=0,((_tmpCCF->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCCF->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCCF)))))))))))));}
break;case 209: _LL310: {struct _tuple8 _tmp66C;struct Cyc_Core_Opt*_tmp66D;struct Cyc_Absyn_Tqual
_tmp66E;void*_tmp66F;struct _tuple8*_tmp66B=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp66C=*_tmp66B;_tmp66D=
_tmp66C.f1;_tmp66E=_tmp66C.f2;_tmp66F=_tmp66C.f3;{struct Cyc_Absyn_VarargInfo*
_tmpCD0;struct Cyc_Absyn_VarargInfo*_tmp670=(_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((
_tmpCD0->name=_tmp66D,((_tmpCD0->tq=_tmp66E,((_tmpCD0->type=_tmp66F,((_tmpCD0->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpCD0)))))))));{struct _tuple23*_tmpCD1;yyval=Cyc_YY39(((_tmpCD1=_cycalloc(
sizeof(*_tmpCD1)),((_tmpCD1->f1=0,((_tmpCD1->f2=0,((_tmpCD1->f3=_tmp670,((
_tmpCD1->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpCD1->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCD1)))))))))))));}
break;};}case 210: _LL311: {struct _tuple8 _tmp674;struct Cyc_Core_Opt*_tmp675;struct
Cyc_Absyn_Tqual _tmp676;void*_tmp677;struct _tuple8*_tmp673=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp674=*_tmp673;_tmp675=
_tmp674.f1;_tmp676=_tmp674.f2;_tmp677=_tmp674.f3;{struct Cyc_Absyn_VarargInfo*
_tmpCD2;struct Cyc_Absyn_VarargInfo*_tmp678=(_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((
_tmpCD2->name=_tmp675,((_tmpCD2->tq=_tmp676,((_tmpCD2->type=_tmp677,((_tmpCD2->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpCD2)))))))));{struct _tuple23*_tmpCD3;yyval=Cyc_YY39(((_tmpCD3=_cycalloc(
sizeof(*_tmpCD3)),((_tmpCD3->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)])),((_tmpCD3->f2=0,((_tmpCD3->f3=_tmp678,((_tmpCD3->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCD3->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCD3)))))))))))));}
break;};}case 211: _LL312:{struct Cyc_Absyn_Unknown_kb_struct _tmpCD6;struct Cyc_Absyn_Unknown_kb_struct*
_tmpCD5;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)((_tmpCD5=_cycalloc(
sizeof(*_tmpCD5)),((_tmpCD5[0]=((_tmpCD6.tag=1,((_tmpCD6.f1=0,_tmpCD6)))),
_tmpCD5))))));}break;case 212: _LL313: yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))));
break;case 213: _LL314: yyval=Cyc_YY49(0);break;case 214: _LL315:{struct Cyc_Absyn_JoinEff_struct*
_tmpCDC;struct Cyc_Absyn_JoinEff_struct _tmpCDB;struct Cyc_Core_Opt*_tmpCDA;yyval=
Cyc_YY49(((_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA->v=(void*)((_tmpCDC=
_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC[0]=((_tmpCDB.tag=24,((_tmpCDB.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCDB)))),_tmpCDC)))),
_tmpCDA)))));}break;case 215: _LL316: yyval=Cyc_YY50(0);break;case 216: _LL317: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 217: _LL318: {
struct Cyc_Absyn_Less_kb_struct _tmpCDF;struct Cyc_Absyn_Less_kb_struct*_tmpCDE;
struct Cyc_Absyn_Less_kb_struct*_tmp680=(_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((
_tmpCDE[0]=((_tmpCDF.tag=2,((_tmpCDF.f1=0,((_tmpCDF.f2=& Cyc_Tcutil_trk,_tmpCDF)))))),
_tmpCDE)));struct _dyneither_ptr _tmp681=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp682=Cyc_Parse_id2type(
_tmp681,(void*)_tmp680);{struct _tuple26*_tmpCE9;struct Cyc_Absyn_JoinEff_struct
_tmpCE8;struct Cyc_Absyn_JoinEff_struct*_tmpCE7;struct Cyc_List_List*_tmpCE6;yyval=
Cyc_YY50(((_tmpCE6=_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6->hd=((_tmpCE9=_cycalloc(
sizeof(*_tmpCE9)),((_tmpCE9->f1=(void*)((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((
_tmpCE7[0]=((_tmpCE8.tag=24,((_tmpCE8.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCE8)))),_tmpCE7)))),((
_tmpCE9->f2=_tmp682,_tmpCE9)))))),((_tmpCE6->tl=0,_tmpCE6)))))));}break;}case 218:
_LL319: {struct Cyc_Absyn_Less_kb_struct _tmpCEC;struct Cyc_Absyn_Less_kb_struct*
_tmpCEB;struct Cyc_Absyn_Less_kb_struct*_tmp689=(_tmpCEB=_cycalloc(sizeof(*
_tmpCEB)),((_tmpCEB[0]=((_tmpCEC.tag=2,((_tmpCEC.f1=0,((_tmpCEC.f2=& Cyc_Tcutil_trk,
_tmpCEC)))))),_tmpCEB)));struct _dyneither_ptr _tmp68A=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp68B=Cyc_Parse_id2type(
_tmp68A,(void*)_tmp689);{struct _tuple26*_tmpCF6;struct Cyc_Absyn_JoinEff_struct
_tmpCF5;struct Cyc_Absyn_JoinEff_struct*_tmpCF4;struct Cyc_List_List*_tmpCF3;yyval=
Cyc_YY50(((_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3->hd=((_tmpCF6=_cycalloc(
sizeof(*_tmpCF6)),((_tmpCF6->f1=(void*)((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((
_tmpCF4[0]=((_tmpCF5.tag=24,((_tmpCF5.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmpCF5)))),_tmpCF4)))),((
_tmpCF6->f2=_tmp68B,_tmpCF6)))))),((_tmpCF3->tl=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCF3)))))));}break;}case
219: _LL31A: yyval=Cyc_YY31(0);break;case 220: _LL31B:{const char*_tmpCF7;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpCF7="inject",_tag_dyneither(_tmpCF7,sizeof(char),7))))
!= 0){const char*_tmpCF8;Cyc_Parse_err(((_tmpCF8="missing type in function declaration",
_tag_dyneither(_tmpCF8,sizeof(char),37))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY31(
1);break;case 221: _LL31C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 222: _LL31D: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 223: _LL31E:
yyval=Cyc_YY40(0);break;case 224: _LL31F: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 225: _LL320:{struct Cyc_Absyn_RgnsEff_struct*
_tmpCFE;struct Cyc_Absyn_RgnsEff_struct _tmpCFD;struct Cyc_List_List*_tmpCFC;yyval=
Cyc_YY40(((_tmpCFC=_cycalloc(sizeof(*_tmpCFC)),((_tmpCFC->hd=(void*)((_tmpCFE=
_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE[0]=((_tmpCFD.tag=25,((_tmpCFD.f1=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpCFD)))),_tmpCFE)))),((_tmpCFC->tl=0,_tmpCFC)))))));}break;case 226: _LL321: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_ek,
0);{struct Cyc_List_List*_tmpCFF;yyval=Cyc_YY40(((_tmpCFF=_cycalloc(sizeof(*
_tmpCFF)),((_tmpCFF->hd=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpCFF->tl=0,_tmpCFF)))))));}break;case 227: _LL322: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_trk,
1);{struct Cyc_Absyn_AccessEff_struct*_tmpD05;struct Cyc_Absyn_AccessEff_struct
_tmpD04;struct Cyc_List_List*_tmpD03;yyval=Cyc_YY40(((_tmpD03=_cycalloc(sizeof(*
_tmpD03)),((_tmpD03->hd=(void*)((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05[0]=((
_tmpD04.tag=23,((_tmpD04.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD04)))),_tmpD05)))),((
_tmpD03->tl=0,_tmpD03)))))));}break;case 228: _LL323: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),& Cyc_Tcutil_trk,1);{
struct Cyc_Absyn_AccessEff_struct*_tmpD0B;struct Cyc_Absyn_AccessEff_struct _tmpD0A;
struct Cyc_List_List*_tmpD09;yyval=Cyc_YY40(((_tmpD09=_cycalloc(sizeof(*_tmpD09)),((
_tmpD09->hd=(void*)((_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B[0]=((_tmpD0A.tag=
23,((_tmpD0A.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),_tmpD0A)))),_tmpD0B)))),((_tmpD09->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD09)))))));}break;case
229: _LL324:{struct Cyc_List_List*_tmpD0C;yyval=Cyc_YY38(((_tmpD0C=_cycalloc(
sizeof(*_tmpD0C)),((_tmpD0C->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD0C->tl=0,_tmpD0C)))))));}
break;case 230: _LL325:{struct Cyc_List_List*_tmpD0D;yyval=Cyc_YY38(((_tmpD0D=
_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD0D->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD0D)))))));}
break;case 231: _LL326: {struct _tuple22 _tmp6A1;struct Cyc_Absyn_Tqual _tmp6A2;struct
Cyc_List_List*_tmp6A3;struct Cyc_List_List*_tmp6A4;struct _tuple22*_tmp6A0=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6A1=*_tmp6A0;
_tmp6A2=_tmp6A1.f1;_tmp6A3=_tmp6A1.f2;_tmp6A4=_tmp6A1.f3;if(_tmp6A2.loc == 0)
_tmp6A2.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp6A6;struct _tuple1*
_tmp6A7;struct Cyc_List_List*_tmp6A8;struct Cyc_Parse_Declarator*_tmp6A5=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A6=*_tmp6A5;
_tmp6A7=_tmp6A6.id;_tmp6A8=_tmp6A6.tms;{void*_tmp6A9=Cyc_Parse_speclist2typ(
_tmp6A3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp6AB;void*_tmp6AC;struct
Cyc_List_List*_tmp6AD;struct Cyc_List_List*_tmp6AE;struct _tuple11 _tmp6AA=Cyc_Parse_apply_tms(
_tmp6A2,_tmp6A9,_tmp6A4,_tmp6A8);_tmp6AB=_tmp6AA.f1;_tmp6AC=_tmp6AA.f2;_tmp6AD=
_tmp6AA.f3;_tmp6AE=_tmp6AA.f4;if(_tmp6AD != 0){const char*_tmpD0E;Cyc_Parse_err(((
_tmpD0E="parameter with bad type params",_tag_dyneither(_tmpD0E,sizeof(char),31))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp6A7)){const char*
_tmpD0F;Cyc_Parse_err(((_tmpD0F="parameter cannot be qualified with a namespace",
_tag_dyneither(_tmpD0F,sizeof(char),47))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*
_tmpD10;struct Cyc_Core_Opt*_tmp6B1=(struct Cyc_Core_Opt*)((_tmpD10=_cycalloc(
sizeof(*_tmpD10)),((_tmpD10->v=(*_tmp6A7).f2,_tmpD10))));if(_tmp6AE != 0){const
char*_tmpD13;void*_tmpD12;(_tmpD12=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD13="extra attributes on parameter, ignoring",
_tag_dyneither(_tmpD13,sizeof(char),40))),_tag_dyneither(_tmpD12,sizeof(void*),0)));}{
struct _tuple8*_tmpD14;yyval=Cyc_YY37(((_tmpD14=_cycalloc(sizeof(*_tmpD14)),((
_tmpD14->f1=_tmp6B1,((_tmpD14->f2=_tmp6AB,((_tmpD14->f3=_tmp6AC,_tmpD14)))))))));}
break;};};};}case 232: _LL327: {struct _tuple22 _tmp6B7;struct Cyc_Absyn_Tqual _tmp6B8;
struct Cyc_List_List*_tmp6B9;struct Cyc_List_List*_tmp6BA;struct _tuple22*_tmp6B6=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B7=*
_tmp6B6;_tmp6B8=_tmp6B7.f1;_tmp6B9=_tmp6B7.f2;_tmp6BA=_tmp6B7.f3;if(_tmp6B8.loc
== 0)_tmp6B8.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{void*_tmp6BB=
Cyc_Parse_speclist2typ(_tmp6B9,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6BA != 0){
const char*_tmpD17;void*_tmpD16;(_tmpD16=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD17="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpD17,sizeof(char),38))),_tag_dyneither(_tmpD16,sizeof(void*),0)));}{
struct _tuple8*_tmpD18;yyval=Cyc_YY37(((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((
_tmpD18->f1=0,((_tmpD18->f2=_tmp6B8,((_tmpD18->f3=_tmp6BB,_tmpD18)))))))));}
break;};}case 233: _LL328: {struct _tuple22 _tmp6C0;struct Cyc_Absyn_Tqual _tmp6C1;
struct Cyc_List_List*_tmp6C2;struct Cyc_List_List*_tmp6C3;struct _tuple22*_tmp6BF=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6C0=*_tmp6BF;_tmp6C1=_tmp6C0.f1;_tmp6C2=_tmp6C0.f2;_tmp6C3=_tmp6C0.f3;if(
_tmp6C1.loc == 0)_tmp6C1.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{void*_tmp6C4=
Cyc_Parse_speclist2typ(_tmp6C2,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6C5=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp6C7;void*_tmp6C8;struct Cyc_List_List*_tmp6C9;struct Cyc_List_List*
_tmp6CA;struct _tuple11 _tmp6C6=Cyc_Parse_apply_tms(_tmp6C1,_tmp6C4,_tmp6C3,
_tmp6C5);_tmp6C7=_tmp6C6.f1;_tmp6C8=_tmp6C6.f2;_tmp6C9=_tmp6C6.f3;_tmp6CA=
_tmp6C6.f4;if(_tmp6C9 != 0){const char*_tmpD1B;void*_tmpD1A;(_tmpD1A=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpD1B="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpD1B,sizeof(char),49))),_tag_dyneither(_tmpD1A,sizeof(void*),0)));}
if(_tmp6CA != 0){const char*_tmpD1E;void*_tmpD1D;(_tmpD1D=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD1E="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpD1E,sizeof(char),38))),_tag_dyneither(_tmpD1D,sizeof(void*),0)));}{
struct _tuple8*_tmpD1F;yyval=Cyc_YY37(((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((
_tmpD1F->f1=0,((_tmpD1F->f2=_tmp6C7,((_tmpD1F->f3=_tmp6C8,_tmpD1F)))))))));}
break;};}case 234: _LL329: yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 235: _LL32A:{struct _dyneither_ptr*_tmpD22;struct Cyc_List_List*
_tmpD21;yyval=Cyc_YY36(((_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21->hd=((
_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD22)))),((_tmpD21->tl=0,
_tmpD21)))))));}break;case 236: _LL32B:{struct _dyneither_ptr*_tmpD25;struct Cyc_List_List*
_tmpD24;yyval=Cyc_YY36(((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24->hd=((
_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD25)))),((_tmpD24->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpD24)))))));}break;case 237: _LL32C: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 238: _LL32D: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 239: _LL32E:{struct
Cyc_Absyn_UnresolvedMem_e_struct _tmpD28;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpD27;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD27=_cycalloc(sizeof(*
_tmpD27)),((_tmpD27[0]=((_tmpD28.tag=36,((_tmpD28.f1=0,((_tmpD28.f2=0,_tmpD28)))))),
_tmpD27)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 240: _LL32F:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD2B;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD2A;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((_tmpD2A[0]=((_tmpD2B.tag=36,((
_tmpD2B.f1=0,((_tmpD2B.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),
_tmpD2B)))))),_tmpD2A)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 241:
_LL330:{struct Cyc_Absyn_UnresolvedMem_e_struct _tmpD2E;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpD2D;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD2D=_cycalloc(sizeof(*
_tmpD2D)),((_tmpD2D[0]=((_tmpD2E.tag=36,((_tmpD2E.f1=0,((_tmpD2E.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),_tmpD2E)))))),_tmpD2D)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 242: _LL331: {struct _dyneither_ptr*
_tmpD31;struct _tuple1*_tmpD30;struct Cyc_Absyn_Vardecl*_tmp6DA=Cyc_Absyn_new_vardecl(((
_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30->f1=Cyc_Absyn_Loc_n,((_tmpD30->f2=((
_tmpD31=_cycalloc(sizeof(*_tmpD31)),((_tmpD31[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpD31)))),_tmpD30)))))),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp6DA->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct _tmpD34;struct Cyc_Absyn_Comprehension_e_struct*
_tmpD33;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD33=_cycalloc(sizeof(*
_tmpD33)),((_tmpD33[0]=((_tmpD34.tag=28,((_tmpD34.f1=_tmp6DA,((_tmpD34.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpD34.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD34.f4=0,
_tmpD34)))))))))),_tmpD33)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
243: _LL332:{struct _tuple27*_tmpD37;struct Cyc_List_List*_tmpD36;yyval=Cyc_YY6(((
_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36->hd=((_tmpD37=_cycalloc(sizeof(*
_tmpD37)),((_tmpD37->f1=0,((_tmpD37->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD37)))))),((_tmpD36->tl=
0,_tmpD36)))))));}break;case 244: _LL333:{struct _tuple27*_tmpD3A;struct Cyc_List_List*
_tmpD39;yyval=Cyc_YY6(((_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39->hd=((
_tmpD3A=_cycalloc(sizeof(*_tmpD3A)),((_tmpD3A->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD3A->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD3A)))))),((
_tmpD39->tl=0,_tmpD39)))))));}break;case 245: _LL334:{struct _tuple27*_tmpD3D;
struct Cyc_List_List*_tmpD3C;yyval=Cyc_YY6(((_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((
_tmpD3C->hd=((_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D->f1=0,((_tmpD3D->f2=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD3D)))))),((
_tmpD3C->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),_tmpD3C)))))));}break;case 246: _LL335:{struct _tuple27*_tmpD40;struct Cyc_List_List*
_tmpD3F;yyval=Cyc_YY6(((_tmpD3F=_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F->hd=((
_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD40->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD40)))))),((
_tmpD3F->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]),_tmpD3F)))))));}break;case 247: _LL336: yyval=Cyc_YY41(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 248: _LL337:{
struct Cyc_List_List*_tmpD41;yyval=Cyc_YY41(((_tmpD41=_cycalloc(sizeof(*_tmpD41)),((
_tmpD41->hd=Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD41->tl=0,_tmpD41)))))));}break;case 249: _LL338:{struct Cyc_List_List*
_tmpD42;yyval=Cyc_YY41(((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->hd=Cyc_yyget_YY42(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD42->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD42)))))));}
break;case 250: _LL339:{struct Cyc_Absyn_ArrayElement_struct _tmpD45;struct Cyc_Absyn_ArrayElement_struct*
_tmpD44;yyval=Cyc_YY42((void*)((_tmpD44=_cycalloc(sizeof(*_tmpD44)),((_tmpD44[0]=((
_tmpD45.tag=0,((_tmpD45.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpD45)))),_tmpD44)))));}break;case 251: _LL33A:{struct
Cyc_Absyn_FieldName_struct _tmpD4B;struct _dyneither_ptr*_tmpD4A;struct Cyc_Absyn_FieldName_struct*
_tmpD49;yyval=Cyc_YY42((void*)((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49[0]=((
_tmpD4B.tag=1,((_tmpD4B.f1=((_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpD4A)))),_tmpD4B)))),_tmpD49)))));}break;case 252: _LL33B: {struct _tuple22
_tmp6EF;struct Cyc_Absyn_Tqual _tmp6F0;struct Cyc_List_List*_tmp6F1;struct Cyc_List_List*
_tmp6F2;struct _tuple22*_tmp6EE=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6EF=*_tmp6EE;_tmp6F0=
_tmp6EF.f1;_tmp6F1=_tmp6EF.f2;_tmp6F2=_tmp6EF.f3;{void*_tmp6F3=Cyc_Parse_speclist2typ(
_tmp6F1,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp6F2 != 0){const char*_tmpD4E;void*_tmpD4D;(
_tmpD4D=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD4E="ignoring attributes in type",
_tag_dyneither(_tmpD4E,sizeof(char),28))),_tag_dyneither(_tmpD4D,sizeof(void*),0)));}{
struct _tuple8*_tmpD4F;yyval=Cyc_YY37(((_tmpD4F=_cycalloc(sizeof(*_tmpD4F)),((
_tmpD4F->f1=0,((_tmpD4F->f2=_tmp6F0,((_tmpD4F->f3=_tmp6F3,_tmpD4F)))))))));}
break;};}case 253: _LL33C: {struct _tuple22 _tmp6F8;struct Cyc_Absyn_Tqual _tmp6F9;
struct Cyc_List_List*_tmp6FA;struct Cyc_List_List*_tmp6FB;struct _tuple22*_tmp6F7=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6F8=*_tmp6F7;_tmp6F9=_tmp6F8.f1;_tmp6FA=_tmp6F8.f2;_tmp6FB=_tmp6F8.f3;{void*
_tmp6FC=Cyc_Parse_speclist2typ(_tmp6FA,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6FD=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple11 _tmp6FE=Cyc_Parse_apply_tms(_tmp6F9,_tmp6FC,_tmp6FB,_tmp6FD);if(
_tmp6FE.f3 != 0){const char*_tmpD52;void*_tmpD51;(_tmpD51=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD52="bad type params, ignoring",
_tag_dyneither(_tmpD52,sizeof(char),26))),_tag_dyneither(_tmpD51,sizeof(void*),0)));}
if(_tmp6FE.f4 != 0){const char*_tmpD55;void*_tmpD54;(_tmpD54=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD55="bad specifiers, ignoring",
_tag_dyneither(_tmpD55,sizeof(char),25))),_tag_dyneither(_tmpD54,sizeof(void*),0)));}{
struct _tuple8*_tmpD56;yyval=Cyc_YY37(((_tmpD56=_cycalloc(sizeof(*_tmpD56)),((
_tmpD56->f1=0,((_tmpD56->f2=_tmp6FE.f1,((_tmpD56->f3=_tmp6FE.f2,_tmpD56)))))))));}
break;};}case 254: _LL33D: yyval=Cyc_YY44((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);break;case 255: _LL33E:{
struct Cyc_Absyn_JoinEff_struct _tmpD59;struct Cyc_Absyn_JoinEff_struct*_tmpD58;
yyval=Cyc_YY44((void*)((_tmpD58=_cycalloc(sizeof(*_tmpD58)),((_tmpD58[0]=((
_tmpD59.tag=24,((_tmpD59.f1=0,_tmpD59)))),_tmpD58)))));}break;case 256: _LL33F:{
struct Cyc_Absyn_JoinEff_struct _tmpD5C;struct Cyc_Absyn_JoinEff_struct*_tmpD5B;
yyval=Cyc_YY44((void*)((_tmpD5B=_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B[0]=((
_tmpD5C.tag=24,((_tmpD5C.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpD5C)))),_tmpD5B)))));}break;case 257: _LL340:{struct
Cyc_Absyn_RgnsEff_struct _tmpD5F;struct Cyc_Absyn_RgnsEff_struct*_tmpD5E;yyval=Cyc_YY44((
void*)((_tmpD5E=_cycalloc(sizeof(*_tmpD5E)),((_tmpD5E[0]=((_tmpD5F.tag=25,((
_tmpD5F.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpD5F)))),_tmpD5E)))));}break;case 258: _LL341:{struct Cyc_Absyn_JoinEff_struct
_tmpD65;struct Cyc_List_List*_tmpD64;struct Cyc_Absyn_JoinEff_struct*_tmpD63;yyval=
Cyc_YY44((void*)((_tmpD63=_cycalloc(sizeof(*_tmpD63)),((_tmpD63[0]=((_tmpD65.tag=
24,((_tmpD65.f1=((_tmpD64=_cycalloc(sizeof(*_tmpD64)),((_tmpD64->hd=Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD64->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD64)))))),_tmpD65)))),
_tmpD63)))));}break;case 259: _LL342:{struct Cyc_List_List*_tmpD66;yyval=Cyc_YY40(((
_tmpD66=_cycalloc(sizeof(*_tmpD66)),((_tmpD66->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD66->tl=0,_tmpD66)))))));}
break;case 260: _LL343:{struct Cyc_List_List*_tmpD67;yyval=Cyc_YY40(((_tmpD67=
_cycalloc(sizeof(*_tmpD67)),((_tmpD67->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD67->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD67)))))));}
break;case 261: _LL344:{struct Cyc_Parse_Abstractdeclarator*_tmpD68;yyval=Cyc_YY30(((
_tmpD68=_cycalloc(sizeof(*_tmpD68)),((_tmpD68->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD68)))));}break;case 262:
_LL345: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
263: _LL346:{struct Cyc_Parse_Abstractdeclarator*_tmpD69;yyval=Cyc_YY30(((_tmpD69=
_cycalloc(sizeof(*_tmpD69)),((_tmpD69->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpD69)))));}break;
case 264: _LL347: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 265: _LL348:{struct Cyc_List_List*_tmpD73;struct Cyc_Absyn_Carray_mod_struct
_tmpD72;struct Cyc_Absyn_Carray_mod_struct*_tmpD71;struct Cyc_Parse_Abstractdeclarator*
_tmpD70;yyval=Cyc_YY30(((_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70->tms=((
_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73->hd=(void*)((_tmpD71=_cycalloc(
sizeof(*_tmpD71)),((_tmpD71[0]=((_tmpD72.tag=0,((_tmpD72.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD72.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD72)))))),
_tmpD71)))),((_tmpD73->tl=0,_tmpD73)))))),_tmpD70)))));}break;case 266: _LL349:{
struct Cyc_List_List*_tmpD7D;struct Cyc_Absyn_Carray_mod_struct _tmpD7C;struct Cyc_Absyn_Carray_mod_struct*
_tmpD7B;struct Cyc_Parse_Abstractdeclarator*_tmpD7A;yyval=Cyc_YY30(((_tmpD7A=
_cycalloc(sizeof(*_tmpD7A)),((_tmpD7A->tms=((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((
_tmpD7D->hd=(void*)((_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((_tmpD7B[0]=((_tmpD7C.tag=
0,((_tmpD7C.f1=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD7C.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD7C)))))),
_tmpD7B)))),((_tmpD7D->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD7D)))))),_tmpD7A)))));}break;case 267: _LL34A:{
struct Cyc_List_List*_tmpD87;struct Cyc_Absyn_ConstArray_mod_struct _tmpD86;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD85;struct Cyc_Parse_Abstractdeclarator*
_tmpD84;yyval=Cyc_YY30(((_tmpD84=_cycalloc(sizeof(*_tmpD84)),((_tmpD84->tms=((
_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->hd=(void*)((_tmpD85=_cycalloc(
sizeof(*_tmpD85)),((_tmpD85[0]=((_tmpD86.tag=1,((_tmpD86.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD86.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD86.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD86)))))))),
_tmpD85)))),((_tmpD87->tl=0,_tmpD87)))))),_tmpD84)))));}break;case 268: _LL34B:{
struct Cyc_List_List*_tmpD91;struct Cyc_Absyn_ConstArray_mod_struct _tmpD90;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD8F;struct Cyc_Parse_Abstractdeclarator*
_tmpD8E;yyval=Cyc_YY30(((_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E->tms=((
_tmpD91=_cycalloc(sizeof(*_tmpD91)),((_tmpD91->hd=(void*)((_tmpD8F=_cycalloc(
sizeof(*_tmpD8F)),((_tmpD8F[0]=((_tmpD90.tag=1,((_tmpD90.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD90.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD90.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD90)))))))),
_tmpD8F)))),((_tmpD91->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpD91)))))),_tmpD8E)))));}break;case 269: _LL34C:{
struct Cyc_List_List*_tmpDA6;struct Cyc_Absyn_Function_mod_struct _tmpDA5;struct Cyc_Absyn_WithTypes_struct
_tmpDA4;struct Cyc_Absyn_WithTypes_struct*_tmpDA3;struct Cyc_Absyn_Function_mod_struct*
_tmpDA2;struct Cyc_Parse_Abstractdeclarator*_tmpDA1;yyval=Cyc_YY30(((_tmpDA1=
_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1->tms=((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((
_tmpDA6->hd=(void*)((_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2[0]=((_tmpDA5.tag=
3,((_tmpDA5.f1=(void*)((void*)((_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((_tmpDA3[0]=((
_tmpDA4.tag=1,((_tmpDA4.f1=0,((_tmpDA4.f2=0,((_tmpDA4.f3=0,((_tmpDA4.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDA4.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDA4)))))))))))),
_tmpDA3))))),_tmpDA5)))),_tmpDA2)))),((_tmpDA6->tl=0,_tmpDA6)))))),_tmpDA1)))));}
break;case 270: _LL34D: {struct _tuple23 _tmp728;struct Cyc_List_List*_tmp729;int
_tmp72A;struct Cyc_Absyn_VarargInfo*_tmp72B;struct Cyc_Core_Opt*_tmp72C;struct Cyc_List_List*
_tmp72D;struct _tuple23*_tmp727=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp728=*_tmp727;_tmp729=
_tmp728.f1;_tmp72A=_tmp728.f2;_tmp72B=_tmp728.f3;_tmp72C=_tmp728.f4;_tmp72D=
_tmp728.f5;{struct Cyc_List_List*_tmpDBB;struct Cyc_Absyn_Function_mod_struct
_tmpDBA;struct Cyc_Absyn_WithTypes_struct _tmpDB9;struct Cyc_Absyn_WithTypes_struct*
_tmpDB8;struct Cyc_Absyn_Function_mod_struct*_tmpDB7;struct Cyc_Parse_Abstractdeclarator*
_tmpDB6;yyval=Cyc_YY30(((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->tms=((
_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB->hd=(void*)((_tmpDB7=_cycalloc(
sizeof(*_tmpDB7)),((_tmpDB7[0]=((_tmpDBA.tag=3,((_tmpDBA.f1=(void*)((void*)((
_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDB9.tag=1,((_tmpDB9.f1=
_tmp729,((_tmpDB9.f2=_tmp72A,((_tmpDB9.f3=_tmp72B,((_tmpDB9.f4=_tmp72C,((_tmpDB9.f5=
_tmp72D,_tmpDB9)))))))))))),_tmpDB8))))),_tmpDBA)))),_tmpDB7)))),((_tmpDBB->tl=0,
_tmpDBB)))))),_tmpDB6)))));}break;}case 271: _LL34E:{struct Cyc_List_List*_tmpDD0;
struct Cyc_Absyn_Function_mod_struct _tmpDCF;struct Cyc_Absyn_WithTypes_struct
_tmpDCE;struct Cyc_Absyn_WithTypes_struct*_tmpDCD;struct Cyc_Absyn_Function_mod_struct*
_tmpDCC;struct Cyc_Parse_Abstractdeclarator*_tmpDCB;yyval=Cyc_YY30(((_tmpDCB=
_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB->tms=((_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((
_tmpDD0->hd=(void*)((_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC[0]=((_tmpDCF.tag=
3,((_tmpDCF.f1=(void*)((void*)((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD[0]=((
_tmpDCE.tag=1,((_tmpDCE.f1=0,((_tmpDCE.f2=0,((_tmpDCE.f3=0,((_tmpDCE.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDCE.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDCE)))))))))))),
_tmpDCD))))),_tmpDCF)))),_tmpDCC)))),((_tmpDD0->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpDD0)))))),
_tmpDCB)))));}break;case 272: _LL34F: {struct _tuple23 _tmp73B;struct Cyc_List_List*
_tmp73C;int _tmp73D;struct Cyc_Absyn_VarargInfo*_tmp73E;struct Cyc_Core_Opt*_tmp73F;
struct Cyc_List_List*_tmp740;struct _tuple23*_tmp73A=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp73B=*_tmp73A;_tmp73C=
_tmp73B.f1;_tmp73D=_tmp73B.f2;_tmp73E=_tmp73B.f3;_tmp73F=_tmp73B.f4;_tmp740=
_tmp73B.f5;{struct Cyc_List_List*_tmpDE5;struct Cyc_Absyn_Function_mod_struct
_tmpDE4;struct Cyc_Absyn_WithTypes_struct _tmpDE3;struct Cyc_Absyn_WithTypes_struct*
_tmpDE2;struct Cyc_Absyn_Function_mod_struct*_tmpDE1;struct Cyc_Parse_Abstractdeclarator*
_tmpDE0;yyval=Cyc_YY30(((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0->tms=((
_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->hd=(void*)((_tmpDE1=_cycalloc(
sizeof(*_tmpDE1)),((_tmpDE1[0]=((_tmpDE4.tag=3,((_tmpDE4.f1=(void*)((void*)((
_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE3.tag=1,((_tmpDE3.f1=
_tmp73C,((_tmpDE3.f2=_tmp73D,((_tmpDE3.f3=_tmp73E,((_tmpDE3.f4=_tmp73F,((_tmpDE3.f5=
_tmp740,_tmpDE3)))))))))))),_tmpDE2))))),_tmpDE4)))),_tmpDE1)))),((_tmpDE5->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpDE5)))))),_tmpDE0)))));}break;}case 273: _LL350: {struct Cyc_List_List*_tmp747=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{
struct Cyc_List_List*_tmpDEF;struct Cyc_Absyn_TypeParams_mod_struct _tmpDEE;struct
Cyc_Absyn_TypeParams_mod_struct*_tmpDED;struct Cyc_Parse_Abstractdeclarator*
_tmpDEC;yyval=Cyc_YY30(((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC->tms=((
_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF->hd=(void*)((_tmpDED=_cycalloc(
sizeof(*_tmpDED)),((_tmpDED[0]=((_tmpDEE.tag=4,((_tmpDEE.f1=_tmp747,((_tmpDEE.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpDEE.f3=0,_tmpDEE)))))))),_tmpDED)))),((_tmpDEF->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpDEF)))))),_tmpDEC)))));}break;}case 274: _LL351:{struct Cyc_List_List*_tmpDF9;
struct Cyc_Absyn_Attributes_mod_struct _tmpDF8;struct Cyc_Absyn_Attributes_mod_struct*
_tmpDF7;struct Cyc_Parse_Abstractdeclarator*_tmpDF6;yyval=Cyc_YY30(((_tmpDF6=
_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6->tms=((_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((
_tmpDF9->hd=(void*)((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7[0]=((_tmpDF8.tag=
5,((_tmpDF8.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpDF8.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDF8)))))),_tmpDF7)))),((
_tmpDF9->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpDF9)))))),_tmpDF6)))));}break;case 275: _LL352: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 276: _LL353:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 277:
_LL354: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
278: _LL355: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 279: _LL356: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 280: _LL357: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 281: _LL358:{struct Cyc_Absyn_ResetRegion_s_struct _tmpDFC;
struct Cyc_Absyn_ResetRegion_s_struct*_tmpDFB;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((
void*)((_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB[0]=((_tmpDFC.tag=16,((
_tmpDFC.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),_tmpDFC)))),_tmpDFB)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 282:
_LL359: yyval=Cyc_YY4(0);break;case 283: _LL35A:{const char*_tmpDFD;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmpDFD="open",_tag_dyneither(_tmpDFD,sizeof(char),5))))
!= 0){const char*_tmpDFE;Cyc_Parse_err(((_tmpDFE="expecting `open'",
_tag_dyneither(_tmpDFE,sizeof(char),17))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}yyval=Cyc_YY4((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));break;case 284: _LL35B:{struct Cyc_Absyn_Label_s_struct _tmpE04;
struct _dyneither_ptr*_tmpE03;struct Cyc_Absyn_Label_s_struct*_tmpE02;yyval=Cyc_YY9(
Cyc_Absyn_new_stmt((void*)((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02[0]=((
_tmpE04.tag=13,((_tmpE04.f1=((_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpE03)))),((_tmpE04.f2=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE04)))))),_tmpE02)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 285:
_LL35C: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 286:
_LL35D: yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 287:
_LL35E: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 288:
_LL35F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 289: _LL360: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0)));
break;case 290: _LL361: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 291: _LL362:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 292:
_LL363: yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 293:
_LL364:{struct Cyc_Absyn_Fn_d_struct _tmpE07;struct Cyc_Absyn_Fn_d_struct*_tmpE06;
yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE06=
_cycalloc(sizeof(*_tmpE06)),((_tmpE06[0]=((_tmpE07.tag=1,((_tmpE07.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE07)))),_tmpE06)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0)));}break;case 294: _LL365:{
struct Cyc_Absyn_Fn_d_struct _tmpE0A;struct Cyc_Absyn_Fn_d_struct*_tmpE09;yyval=Cyc_YY9(
Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE09=_cycalloc(sizeof(*
_tmpE09)),((_tmpE09[0]=((_tmpE0A.tag=1,((_tmpE0A.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE0A)))),_tmpE09)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));}break;case 295: _LL366: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 296: _LL367: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 297:
_LL368: yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 298:
_LL369: {struct Cyc_Absyn_UnknownId_e_struct _tmpE0D;struct Cyc_Absyn_UnknownId_e_struct*
_tmpE0C;struct Cyc_Absyn_Exp*_tmp75B=Cyc_Absyn_new_exp((void*)((_tmpE0C=_cycalloc(
sizeof(*_tmpE0C)),((_tmpE0C[0]=((_tmpE0D.tag=2,((_tmpE0D.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpE0D)))),_tmpE0C)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp75B,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 299:
_LL36A: {struct Cyc_Absyn_Exp*_tmp75E=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(
Cyc_Absyn_switch_stmt(_tmp75E,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 300:
_LL36B: yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 301:
_LL36C: yyval=Cyc_YY10(0);break;case 302: _LL36D:{struct Cyc_Absyn_Switch_clause*
_tmpE10;struct Cyc_List_List*_tmpE0F;yyval=Cyc_YY10(((_tmpE0F=_cycalloc(sizeof(*
_tmpE0F)),((_tmpE0F->hd=((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10->pattern=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)),((_tmpE10->pat_vars=
0,((_tmpE10->where_clause=0,((_tmpE10->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpE10->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE10)))))))))))),((
_tmpE0F->tl=0,_tmpE0F)))))));}break;case 303: _LL36E:{struct Cyc_Absyn_Switch_clause*
_tmpE13;struct Cyc_List_List*_tmpE12;yyval=Cyc_YY10(((_tmpE12=_cycalloc(sizeof(*
_tmpE12)),((_tmpE12->hd=((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpE13->pat_vars=0,((_tmpE13->where_clause=0,((_tmpE13->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpE13->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE13)))))))))))),((
_tmpE12->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE12)))))));}break;case 304: _LL36F:{struct Cyc_Absyn_Switch_clause*
_tmpE16;struct Cyc_List_List*_tmpE15;yyval=Cyc_YY10(((_tmpE15=_cycalloc(sizeof(*
_tmpE15)),((_tmpE15->hd=((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpE16->pat_vars=0,((_tmpE16->where_clause=0,((_tmpE16->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE16->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpE16)))))))))))),((
_tmpE15->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE15)))))));}break;case 305: _LL370:{struct Cyc_Absyn_Switch_clause*
_tmpE19;struct Cyc_List_List*_tmpE18;yyval=Cyc_YY10(((_tmpE18=_cycalloc(sizeof(*
_tmpE18)),((_tmpE18->hd=((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((
_tmpE19->pat_vars=0,((_tmpE19->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpE19->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpE19->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE19)))))))))))),((
_tmpE18->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE18)))))));}break;case 306: _LL371:{struct Cyc_Absyn_Switch_clause*
_tmpE1C;struct Cyc_List_List*_tmpE1B;yyval=Cyc_YY10(((_tmpE1B=_cycalloc(sizeof(*
_tmpE1B)),((_tmpE1B->hd=((_tmpE1C=_cycalloc(sizeof(*_tmpE1C)),((_tmpE1C->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((
_tmpE1C->pat_vars=0,((_tmpE1C->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpE1C->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE1C->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE1C)))))))))))),((
_tmpE1B->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE1B)))))));}break;case 307: _LL372: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 308:
_LL373: yyval=Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 309:
_LL374: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 310:
_LL375: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 311:
_LL376: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 312:
_LL377: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
313: _LL378: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 314: _LL379: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 315:
_LL37A: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 316:
_LL37B: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 317:
_LL37C: {struct Cyc_List_List*_tmp769=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp76A=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp769,_tmp76A));break;}case 318: _LL37D: {struct
Cyc_List_List*_tmp76B=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp76C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp76B,
_tmp76C));break;}case 319: _LL37E: {struct Cyc_List_List*_tmp76D=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp76E=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp76D,_tmp76E));break;}case 320: _LL37F: {struct
Cyc_List_List*_tmp76F=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp770=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp76F,_tmp770));break;}case 321: _LL380:{struct
_dyneither_ptr*_tmpE1D;yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((_tmpE1D=_cycalloc(
sizeof(*_tmpE1D)),((_tmpE1D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE1D)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));}break;case
322: _LL381: yyval=Cyc_YY9(Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
323: _LL382: yyval=Cyc_YY9(Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
324: _LL383: yyval=Cyc_YY9(Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
325: _LL384: yyval=Cyc_YY9(Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
326: _LL385: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
327: _LL386: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)));break;case
328: _LL387: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)));break;case
329: _LL388: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 330: _LL389: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 331: _LL38A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 332: _LL38B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 333: _LL38C: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 334:
_LL38D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
335: _LL38E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 336:
_LL38F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
337: _LL390: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 338: _LL391: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 339: _LL392: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 340:
_LL393: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
341: _LL394: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 342: _LL395: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 343: _LL396: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 344:
_LL397: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 345:
_LL398: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
346: _LL399: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 347:
_LL39A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 348:
_LL39B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 349:
_LL39C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 350:
_LL39D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
351: _LL39E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 352: _LL39F: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 353:
_LL3A0: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
354: _LL3A1: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 355: _LL3A2: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 356:
_LL3A3: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
357: _LL3A4: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 358: _LL3A5: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 359:
_LL3A6: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 360:
_LL3A7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
361: _LL3A8: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 362: _LL3A9: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 363: _LL3AA: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));break;case 364: _LL3AB: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 365: _LL3AC: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 366:
_LL3AD: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 367:
_LL3AE:{struct Cyc_Absyn_StructField_struct _tmpE23;struct _dyneither_ptr*_tmpE22;
struct Cyc_Absyn_StructField_struct*_tmpE21;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpE21=_cycalloc(sizeof(*_tmpE21)),((_tmpE21[0]=((_tmpE23.tag=0,((
_tmpE23.f1=((_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE22)))),_tmpE23)))),
_tmpE21)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 368: _LL3AF:{struct Cyc_Absyn_TupleIndex_struct
_tmpE26;struct Cyc_Absyn_TupleIndex_struct*_tmpE25;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)((_tmpE25=_cycalloc_atomic(sizeof(*_tmpE25)),((
_tmpE25[0]=((_tmpE26.tag=1,((_tmpE26.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,_tmpE26)))),_tmpE25)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 369: _LL3B0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 370: _LL3B1: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 371: _LL3B2:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 372:
_LL3B3: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 373: _LL3B4: {
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);{void*_tmp777=e->r;union Cyc_Absyn_Cnst _tmp779;struct _tuple4
_tmp77A;enum Cyc_Absyn_Sign _tmp77B;char _tmp77C;union Cyc_Absyn_Cnst _tmp77E;struct
_tuple5 _tmp77F;enum Cyc_Absyn_Sign _tmp780;short _tmp781;union Cyc_Absyn_Cnst
_tmp783;struct _tuple6 _tmp784;enum Cyc_Absyn_Sign _tmp785;int _tmp786;union Cyc_Absyn_Cnst
_tmp788;struct _dyneither_ptr _tmp789;union Cyc_Absyn_Cnst _tmp78B;int _tmp78C;union
Cyc_Absyn_Cnst _tmp78E;struct _dyneither_ptr _tmp78F;union Cyc_Absyn_Cnst _tmp791;
struct _tuple7 _tmp792;_LL3B7: {struct Cyc_Absyn_Const_e_struct*_tmp778=(struct Cyc_Absyn_Const_e_struct*)
_tmp777;if(_tmp778->tag != 0)goto _LL3B9;else{_tmp779=_tmp778->f1;if((_tmp779.Char_c).tag
!= 2)goto _LL3B9;_tmp77A=(struct _tuple4)(_tmp779.Char_c).val;_tmp77B=_tmp77A.f1;
_tmp77C=_tmp77A.f2;}}_LL3B8:{struct Cyc_Absyn_Char_p_struct _tmpE29;struct Cyc_Absyn_Char_p_struct*
_tmpE28;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE28=_cycalloc_atomic(
sizeof(*_tmpE28)),((_tmpE28[0]=((_tmpE29.tag=10,((_tmpE29.f1=_tmp77C,_tmpE29)))),
_tmpE28)))),e->loc));}goto _LL3B6;_LL3B9: {struct Cyc_Absyn_Const_e_struct*_tmp77D=(
struct Cyc_Absyn_Const_e_struct*)_tmp777;if(_tmp77D->tag != 0)goto _LL3BB;else{
_tmp77E=_tmp77D->f1;if((_tmp77E.Short_c).tag != 3)goto _LL3BB;_tmp77F=(struct
_tuple5)(_tmp77E.Short_c).val;_tmp780=_tmp77F.f1;_tmp781=_tmp77F.f2;}}_LL3BA:{
struct Cyc_Absyn_Int_p_struct _tmpE2C;struct Cyc_Absyn_Int_p_struct*_tmpE2B;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE2B=_cycalloc_atomic(sizeof(*_tmpE2B)),((
_tmpE2B[0]=((_tmpE2C.tag=9,((_tmpE2C.f1=_tmp780,((_tmpE2C.f2=(int)_tmp781,
_tmpE2C)))))),_tmpE2B)))),e->loc));}goto _LL3B6;_LL3BB: {struct Cyc_Absyn_Const_e_struct*
_tmp782=(struct Cyc_Absyn_Const_e_struct*)_tmp777;if(_tmp782->tag != 0)goto _LL3BD;
else{_tmp783=_tmp782->f1;if((_tmp783.Int_c).tag != 4)goto _LL3BD;_tmp784=(struct
_tuple6)(_tmp783.Int_c).val;_tmp785=_tmp784.f1;_tmp786=_tmp784.f2;}}_LL3BC:{
struct Cyc_Absyn_Int_p_struct _tmpE2F;struct Cyc_Absyn_Int_p_struct*_tmpE2E;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE2E=_cycalloc_atomic(sizeof(*_tmpE2E)),((
_tmpE2E[0]=((_tmpE2F.tag=9,((_tmpE2F.f1=_tmp785,((_tmpE2F.f2=_tmp786,_tmpE2F)))))),
_tmpE2E)))),e->loc));}goto _LL3B6;_LL3BD: {struct Cyc_Absyn_Const_e_struct*_tmp787=(
struct Cyc_Absyn_Const_e_struct*)_tmp777;if(_tmp787->tag != 0)goto _LL3BF;else{
_tmp788=_tmp787->f1;if((_tmp788.Float_c).tag != 6)goto _LL3BF;_tmp789=(struct
_dyneither_ptr)(_tmp788.Float_c).val;}}_LL3BE:{struct Cyc_Absyn_Float_p_struct
_tmpE32;struct Cyc_Absyn_Float_p_struct*_tmpE31;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31[0]=((_tmpE32.tag=11,((
_tmpE32.f1=_tmp789,_tmpE32)))),_tmpE31)))),e->loc));}goto _LL3B6;_LL3BF: {struct
Cyc_Absyn_Const_e_struct*_tmp78A=(struct Cyc_Absyn_Const_e_struct*)_tmp777;if(
_tmp78A->tag != 0)goto _LL3C1;else{_tmp78B=_tmp78A->f1;if((_tmp78B.Null_c).tag != 1)
goto _LL3C1;_tmp78C=(int)(_tmp78B.Null_c).val;}}_LL3C0: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3B6;_LL3C1: {struct Cyc_Absyn_Const_e_struct*
_tmp78D=(struct Cyc_Absyn_Const_e_struct*)_tmp777;if(_tmp78D->tag != 0)goto _LL3C3;
else{_tmp78E=_tmp78D->f1;if((_tmp78E.String_c).tag != 7)goto _LL3C3;_tmp78F=(
struct _dyneither_ptr)(_tmp78E.String_c).val;}}_LL3C2:{const char*_tmpE33;Cyc_Parse_err(((
_tmpE33="strings cannot occur within patterns",_tag_dyneither(_tmpE33,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL3B6;_LL3C3: {struct Cyc_Absyn_Const_e_struct*
_tmp790=(struct Cyc_Absyn_Const_e_struct*)_tmp777;if(_tmp790->tag != 0)goto _LL3C5;
else{_tmp791=_tmp790->f1;if((_tmp791.LongLong_c).tag != 5)goto _LL3C5;_tmp792=(
struct _tuple7)(_tmp791.LongLong_c).val;}}_LL3C4:{const char*_tmpE34;Cyc_Parse_unimp(((
_tmpE34="long long's in patterns",_tag_dyneither(_tmpE34,sizeof(char),24))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}goto _LL3B6;
_LL3C5:;_LL3C6: {const char*_tmpE35;Cyc_Parse_err(((_tmpE35="bad constant in case",
_tag_dyneither(_tmpE35,sizeof(char),21))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}_LL3B6:;}
break;}case 374: _LL3B5:{struct Cyc_Absyn_UnknownId_p_struct _tmpE38;struct Cyc_Absyn_UnknownId_p_struct*
_tmpE37;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE37=_cycalloc(sizeof(*
_tmpE37)),((_tmpE37[0]=((_tmpE38.tag=14,((_tmpE38.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE38)))),_tmpE37)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 375:
_LL3C7:{const char*_tmpE39;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE39="as",
_tag_dyneither(_tmpE39,sizeof(char),3))))!= 0){const char*_tmpE3A;Cyc_Parse_err(((
_tmpE3A="expecting `as'",_tag_dyneither(_tmpE3A,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Var_p_struct
_tmpE44;struct _dyneither_ptr*_tmpE43;struct _tuple1*_tmpE42;struct Cyc_Absyn_Var_p_struct*
_tmpE41;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE41=_cycalloc(sizeof(*
_tmpE41)),((_tmpE41[0]=((_tmpE44.tag=1,((_tmpE44.f1=Cyc_Absyn_new_vardecl(((
_tmpE42=_cycalloc(sizeof(*_tmpE42)),((_tmpE42->f1=Cyc_Absyn_Loc_n,((_tmpE42->f2=((
_tmpE43=_cycalloc(sizeof(*_tmpE43)),((_tmpE43[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE43)))),_tmpE42)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE44.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE44)))))),_tmpE41)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 376: _LL3C8: {struct Cyc_List_List*
_tmp7A7;int _tmp7A8;struct _tuple19 _tmp7A6=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A7=_tmp7A6.f1;
_tmp7A8=_tmp7A6.f2;{struct Cyc_Absyn_Tuple_p_struct _tmpE47;struct Cyc_Absyn_Tuple_p_struct*
_tmpE46;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE46=_cycalloc(sizeof(*
_tmpE46)),((_tmpE46[0]=((_tmpE47.tag=4,((_tmpE47.f1=_tmp7A7,((_tmpE47.f2=_tmp7A8,
_tmpE47)))))),_tmpE46)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
377: _LL3C9: {struct Cyc_List_List*_tmp7AC;int _tmp7AD;struct _tuple19 _tmp7AB=*Cyc_yyget_YY12(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7AC=_tmp7AB.f1;
_tmp7AD=_tmp7AB.f2;{struct Cyc_Absyn_UnknownCall_p_struct _tmpE4A;struct Cyc_Absyn_UnknownCall_p_struct*
_tmpE49;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE49=_cycalloc(sizeof(*
_tmpE49)),((_tmpE49[0]=((_tmpE4A.tag=15,((_tmpE4A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpE4A.f2=_tmp7AC,((
_tmpE4A.f3=_tmp7AD,_tmpE4A)))))))),_tmpE49)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
378: _LL3CA: {struct Cyc_List_List*_tmp7B1;int _tmp7B2;struct _tuple19 _tmp7B0=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7B1=_tmp7B0.f1;
_tmp7B2=_tmp7B0.f2;{struct Cyc_List_List*_tmp7B3=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpE50;struct Cyc_Absyn_AggrInfo*_tmpE4F;struct Cyc_Absyn_Aggr_p_struct*_tmpE4E;
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((
_tmpE4E[0]=((_tmpE50.tag=6,((_tmpE50.f1=((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((
_tmpE4F->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),0),((_tmpE4F->targs=
0,_tmpE4F)))))),((_tmpE50.f2=_tmp7B3,((_tmpE50.f3=_tmp7B1,((_tmpE50.f4=_tmp7B2,
_tmpE50)))))))))),_tmpE4E)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;};}case
379: _LL3CB: {struct Cyc_List_List*_tmp7B8;int _tmp7B9;struct _tuple19 _tmp7B7=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7B8=_tmp7B7.f1;
_tmp7B9=_tmp7B7.f2;{struct Cyc_List_List*_tmp7BA=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpE53;struct Cyc_Absyn_Aggr_p_struct*_tmpE52;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE52=_cycalloc(sizeof(*_tmpE52)),((_tmpE52[0]=((_tmpE53.tag=6,((
_tmpE53.f1=0,((_tmpE53.f2=_tmp7BA,((_tmpE53.f3=_tmp7B8,((_tmpE53.f4=_tmp7B9,
_tmpE53)))))))))),_tmpE52)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;};}case
380: _LL3CC:{struct Cyc_Absyn_Pointer_p_struct _tmpE56;struct Cyc_Absyn_Pointer_p_struct*
_tmpE55;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE55=_cycalloc(sizeof(*
_tmpE55)),((_tmpE55[0]=((_tmpE56.tag=5,((_tmpE56.f1=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE56)))),_tmpE55)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 381:
_LL3CD:{struct Cyc_Absyn_Pointer_p_struct _tmpE60;struct Cyc_Absyn_Pointer_p_struct
_tmpE5F;struct Cyc_Absyn_Pointer_p_struct*_tmpE5E;struct Cyc_Absyn_Pointer_p_struct*
_tmpE5D;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE5D=_cycalloc(sizeof(*
_tmpE5D)),((_tmpE5D[0]=((_tmpE60.tag=5,((_tmpE60.f1=Cyc_Absyn_new_pat((void*)((
_tmpE5E=_cycalloc(sizeof(*_tmpE5E)),((_tmpE5E[0]=((_tmpE5F.tag=5,((_tmpE5F.f1=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE5F)))),
_tmpE5E)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpE60)))),_tmpE5D)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 382:
_LL3CE:{struct Cyc_Absyn_Reference_p_struct _tmpE6A;struct _dyneither_ptr*_tmpE69;
struct _tuple1*_tmpE68;struct Cyc_Absyn_Reference_p_struct*_tmpE67;yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)((_tmpE67=_cycalloc(sizeof(*_tmpE67)),((_tmpE67[0]=((
_tmpE6A.tag=2,((_tmpE6A.f1=Cyc_Absyn_new_vardecl(((_tmpE68=_cycalloc(sizeof(*
_tmpE68)),((_tmpE68->f1=Cyc_Absyn_Loc_n,((_tmpE68->f2=((_tmpE69=_cycalloc(
sizeof(*_tmpE69)),((_tmpE69[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE69)))),_tmpE68)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE6A.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpE6A)))))),_tmpE67)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 383:
_LL3CF:{const char*_tmpE6B;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE6B="as",
_tag_dyneither(_tmpE6B,sizeof(char),3))))!= 0){const char*_tmpE6C;Cyc_Parse_err(((
_tmpE6C="expecting `as'",_tag_dyneither(_tmpE6C,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Reference_p_struct
_tmpE76;struct _dyneither_ptr*_tmpE75;struct _tuple1*_tmpE74;struct Cyc_Absyn_Reference_p_struct*
_tmpE73;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE73=_cycalloc(sizeof(*
_tmpE73)),((_tmpE73[0]=((_tmpE76.tag=2,((_tmpE76.f1=Cyc_Absyn_new_vardecl(((
_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74->f1=Cyc_Absyn_Loc_n,((_tmpE74->f2=((
_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE75)))),_tmpE74)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE76.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE76)))))),_tmpE73)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 384: _LL3D0: {void*_tmp7CD=Cyc_Parse_id2type(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct
_tmpE87;struct Cyc_Absyn_TagType_struct _tmpE86;struct Cyc_Absyn_TagType_struct*
_tmpE85;struct _dyneither_ptr*_tmpE84;struct _tuple1*_tmpE83;struct Cyc_Absyn_TagInt_p_struct*
_tmpE82;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE82=_cycalloc(sizeof(*
_tmpE82)),((_tmpE82[0]=((_tmpE87.tag=3,((_tmpE87.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp7CD),((
_tmpE87.f2=Cyc_Absyn_new_vardecl(((_tmpE83=_cycalloc(sizeof(*_tmpE83)),((_tmpE83->f1=
Cyc_Absyn_Loc_n,((_tmpE83->f2=((_tmpE84=_cycalloc(sizeof(*_tmpE84)),((_tmpE84[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpE84)))),_tmpE83)))))),(void*)((_tmpE85=_cycalloc(sizeof(*_tmpE85)),((_tmpE85[
0]=((_tmpE86.tag=20,((_tmpE86.f1=(void*)_tmp7CD,_tmpE86)))),_tmpE85)))),0),
_tmpE87)))))),_tmpE82)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
385: _LL3D1: {struct Cyc_Absyn_Tvar*_tmp7D4=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(&
Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct _tmpEA3;struct Cyc_Absyn_TagType_struct
_tmpEA2;struct Cyc_Absyn_VarType_struct _tmpEA1;struct Cyc_Absyn_VarType_struct*
_tmpEA0;struct Cyc_Absyn_TagType_struct*_tmpE9F;struct _dyneither_ptr*_tmpE9E;
struct _tuple1*_tmpE9D;struct Cyc_Absyn_TagInt_p_struct*_tmpE9C;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE9C=_cycalloc(sizeof(*_tmpE9C)),((_tmpE9C[0]=((_tmpEA3.tag=3,((
_tmpEA3.f1=_tmp7D4,((_tmpEA3.f2=Cyc_Absyn_new_vardecl(((_tmpE9D=_cycalloc(
sizeof(*_tmpE9D)),((_tmpE9D->f1=Cyc_Absyn_Loc_n,((_tmpE9D->f2=((_tmpE9E=
_cycalloc(sizeof(*_tmpE9E)),((_tmpE9E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpE9E)))),_tmpE9D)))))),(
void*)((_tmpE9F=_cycalloc(sizeof(*_tmpE9F)),((_tmpE9F[0]=((_tmpEA2.tag=20,((
_tmpEA2.f1=(void*)((void*)((_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0[0]=((
_tmpEA1.tag=2,((_tmpEA1.f1=_tmp7D4,_tmpEA1)))),_tmpEA0))))),_tmpEA2)))),_tmpE9F)))),
0),_tmpEA3)))))),_tmpE9C)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
386: _LL3D2:{struct _tuple19*_tmpEA4;yyval=Cyc_YY12(((_tmpEA4=_cycalloc(sizeof(*
_tmpEA4)),((_tmpEA4->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpEA4->f2=0,_tmpEA4)))))));}break;case 387: _LL3D3:{struct _tuple19*_tmpEA5;yyval=
Cyc_YY12(((_tmpEA5=_cycalloc(sizeof(*_tmpEA5)),((_tmpEA5->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpEA5->f2=1,_tmpEA5)))))));}
break;case 388: _LL3D4:{struct _tuple19*_tmpEA6;yyval=Cyc_YY12(((_tmpEA6=_cycalloc(
sizeof(*_tmpEA6)),((_tmpEA6->f1=0,((_tmpEA6->f2=1,_tmpEA6)))))));}break;case 389:
_LL3D5:{struct Cyc_List_List*_tmpEA7;yyval=Cyc_YY13(((_tmpEA7=_cycalloc(sizeof(*
_tmpEA7)),((_tmpEA7->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpEA7->tl=0,_tmpEA7)))))));}break;case 390: _LL3D6:{struct Cyc_List_List*
_tmpEA8;yyval=Cyc_YY13(((_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8->hd=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEA8->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpEA8)))))));}
break;case 391: _LL3D7:{struct _tuple20*_tmpEA9;yyval=Cyc_YY14(((_tmpEA9=_cycalloc(
sizeof(*_tmpEA9)),((_tmpEA9->f1=0,((_tmpEA9->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEA9)))))));}break;case
392: _LL3D8:{struct _tuple20*_tmpEAA;yyval=Cyc_YY14(((_tmpEAA=_cycalloc(sizeof(*
_tmpEAA)),((_tmpEAA->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpEAA->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEAA)))))));}break;case
393: _LL3D9:{struct _tuple19*_tmpEAB;yyval=Cyc_YY16(((_tmpEAB=_cycalloc(sizeof(*
_tmpEAB)),((_tmpEAB->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpEAB->f2=0,_tmpEAB)))))));}break;case 394: _LL3DA:{struct _tuple19*_tmpEAC;yyval=
Cyc_YY16(((_tmpEAC=_cycalloc(sizeof(*_tmpEAC)),((_tmpEAC->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpEAC->f2=1,_tmpEAC)))))));}
break;case 395: _LL3DB:{struct _tuple19*_tmpEAD;yyval=Cyc_YY16(((_tmpEAD=_cycalloc(
sizeof(*_tmpEAD)),((_tmpEAD->f1=0,((_tmpEAD->f2=1,_tmpEAD)))))));}break;case 396:
_LL3DC:{struct Cyc_List_List*_tmpEAE;yyval=Cyc_YY15(((_tmpEAE=_cycalloc(sizeof(*
_tmpEAE)),((_tmpEAE->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpEAE->tl=0,_tmpEAE)))))));}break;case 397: _LL3DD:{struct Cyc_List_List*
_tmpEAF;yyval=Cyc_YY15(((_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((_tmpEAF->hd=Cyc_yyget_YY14(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEAF->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpEAF)))))));}
break;case 398: _LL3DE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 399: _LL3DF: yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 400:
_LL3E0: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
401: _LL3E1: yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 402:
_LL3E2: yyval=Cyc_YY3(Cyc_Absyn_swap_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 403:
_LL3E3: yyval=Cyc_YY8(0);break;case 404: _LL3E4:{struct Cyc_Core_Opt*_tmpEB0;yyval=
Cyc_YY8(((_tmpEB0=_cycalloc_atomic(sizeof(*_tmpEB0)),((_tmpEB0->v=(void*)Cyc_Absyn_Times,
_tmpEB0)))));}break;case 405: _LL3E5:{struct Cyc_Core_Opt*_tmpEB1;yyval=Cyc_YY8(((
_tmpEB1=_cycalloc_atomic(sizeof(*_tmpEB1)),((_tmpEB1->v=(void*)Cyc_Absyn_Div,
_tmpEB1)))));}break;case 406: _LL3E6:{struct Cyc_Core_Opt*_tmpEB2;yyval=Cyc_YY8(((
_tmpEB2=_cycalloc_atomic(sizeof(*_tmpEB2)),((_tmpEB2->v=(void*)Cyc_Absyn_Mod,
_tmpEB2)))));}break;case 407: _LL3E7:{struct Cyc_Core_Opt*_tmpEB3;yyval=Cyc_YY8(((
_tmpEB3=_cycalloc_atomic(sizeof(*_tmpEB3)),((_tmpEB3->v=(void*)Cyc_Absyn_Plus,
_tmpEB3)))));}break;case 408: _LL3E8:{struct Cyc_Core_Opt*_tmpEB4;yyval=Cyc_YY8(((
_tmpEB4=_cycalloc_atomic(sizeof(*_tmpEB4)),((_tmpEB4->v=(void*)Cyc_Absyn_Minus,
_tmpEB4)))));}break;case 409: _LL3E9:{struct Cyc_Core_Opt*_tmpEB5;yyval=Cyc_YY8(((
_tmpEB5=_cycalloc_atomic(sizeof(*_tmpEB5)),((_tmpEB5->v=(void*)Cyc_Absyn_Bitlshift,
_tmpEB5)))));}break;case 410: _LL3EA:{struct Cyc_Core_Opt*_tmpEB6;yyval=Cyc_YY8(((
_tmpEB6=_cycalloc_atomic(sizeof(*_tmpEB6)),((_tmpEB6->v=(void*)Cyc_Absyn_Bitlrshift,
_tmpEB6)))));}break;case 411: _LL3EB:{struct Cyc_Core_Opt*_tmpEB7;yyval=Cyc_YY8(((
_tmpEB7=_cycalloc_atomic(sizeof(*_tmpEB7)),((_tmpEB7->v=(void*)Cyc_Absyn_Bitand,
_tmpEB7)))));}break;case 412: _LL3EC:{struct Cyc_Core_Opt*_tmpEB8;yyval=Cyc_YY8(((
_tmpEB8=_cycalloc_atomic(sizeof(*_tmpEB8)),((_tmpEB8->v=(void*)Cyc_Absyn_Bitxor,
_tmpEB8)))));}break;case 413: _LL3ED:{struct Cyc_Core_Opt*_tmpEB9;yyval=Cyc_YY8(((
_tmpEB9=_cycalloc_atomic(sizeof(*_tmpEB9)),((_tmpEB9->v=(void*)Cyc_Absyn_Bitor,
_tmpEB9)))));}break;case 414: _LL3EE: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 415: _LL3EF: yyval=Cyc_YY3(Cyc_Absyn_conditional_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 416:
_LL3F0: yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 417:
_LL3F1: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 418:
_LL3F2: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 419:
_LL3F3: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 420:
_LL3F4: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 421:
_LL3F5: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
422: _LL3F6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 423: _LL3F7: yyval=Cyc_YY3(Cyc_Absyn_or_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 424:
_LL3F8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
425: _LL3F9: yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 426:
_LL3FA: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
427: _LL3FB: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 428:
_LL3FC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
429: _LL3FD: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 430:
_LL3FE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
431: _LL3FF: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 432:
_LL400: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
433: _LL401: yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 434:
_LL402: yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 435:
_LL403: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
436: _LL404: yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 437:
_LL405: yyval=Cyc_YY3(Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 438:
_LL406: yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 439:
_LL407: yyval=Cyc_YY3(Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 440:
_LL408: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
441: _LL409: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 442:
_LL40A: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 443:
_LL40B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
444: _LL40C: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 445:
_LL40D: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 446:
_LL40E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
447: _LL40F: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 448:
_LL410: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 449:
_LL411: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 450:
_LL412: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
451: _LL413: yyval=Cyc_YY3(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 452: _LL414: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 453: _LL415: yyval=
Cyc_YY3(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 454:
_LL416: yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 455:
_LL417: yyval=Cyc_YY3(Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 456:
_LL418: yyval=Cyc_YY3(Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 457:
_LL419: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
458: _LL41A: yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 459:
_LL41B: yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 460:
_LL41C: yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 461:
_LL41D:{struct Cyc_Absyn_StructField_struct _tmpEBF;struct _dyneither_ptr*_tmpEBE;
struct Cyc_Absyn_StructField_struct*_tmpEBD;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpEBD=_cycalloc(sizeof(*_tmpEBD)),((_tmpEBD[0]=((_tmpEBF.tag=0,((
_tmpEBF.f1=((_tmpEBE=_cycalloc(sizeof(*_tmpEBE)),((_tmpEBE[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpEBE)))),_tmpEBF)))),
_tmpEBD)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 462: _LL41E:{struct Cyc_Absyn_TupleIndex_struct
_tmpEC2;struct Cyc_Absyn_TupleIndex_struct*_tmpEC1;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpEC1=_cycalloc_atomic(sizeof(*_tmpEC1)),((_tmpEC1[0]=((_tmpEC2.tag=1,((
_tmpEC2.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2,_tmpEC2)))),_tmpEC1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 463:
_LL41F:{struct Cyc_Absyn_Malloc_e_struct _tmpEC8;struct Cyc_Absyn_MallocInfo _tmpEC7;
struct Cyc_Absyn_Malloc_e_struct*_tmpEC6;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpEC6=_cycalloc(sizeof(*_tmpEC6)),((_tmpEC6[0]=((_tmpEC8.tag=34,((_tmpEC8.f1=((
_tmpEC7.is_calloc=0,((_tmpEC7.rgn=0,((_tmpEC7.elt_type=0,((_tmpEC7.num_elts=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpEC7.fat_result=
0,_tmpEC7)))))))))),_tmpEC8)))),_tmpEC6)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 464:
_LL420:{struct Cyc_Absyn_Malloc_e_struct _tmpECE;struct Cyc_Absyn_MallocInfo _tmpECD;
struct Cyc_Absyn_Malloc_e_struct*_tmpECC;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpECC=_cycalloc(sizeof(*_tmpECC)),((_tmpECC[0]=((_tmpECE.tag=34,((_tmpECE.f1=((
_tmpECD.is_calloc=0,((_tmpECD.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpECD.elt_type=0,((
_tmpECD.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpECD.fat_result=0,_tmpECD)))))))))),_tmpECE)))),_tmpECC)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 465: _LL421: {void*_tmp7FF;struct _tuple8
_tmp7FE=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp7FF=_tmp7FE.f3;{struct Cyc_Absyn_Malloc_e_struct _tmpED8;void**_tmpED7;
struct Cyc_Absyn_MallocInfo _tmpED6;struct Cyc_Absyn_Malloc_e_struct*_tmpED5;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpED5=_cycalloc(sizeof(*_tmpED5)),((_tmpED5[
0]=((_tmpED8.tag=34,((_tmpED8.f1=((_tmpED6.is_calloc=1,((_tmpED6.rgn=0,((_tmpED6.elt_type=((
_tmpED7=_cycalloc(sizeof(*_tmpED7)),((_tmpED7[0]=_tmp7FF,_tmpED7)))),((_tmpED6.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((
_tmpED6.fat_result=0,_tmpED6)))))))))),_tmpED8)))),_tmpED5)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
466: _LL422: {void*_tmp805;struct _tuple8 _tmp804=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp805=_tmp804.f3;{
struct Cyc_Absyn_Malloc_e_struct _tmpEE2;void**_tmpEE1;struct Cyc_Absyn_MallocInfo
_tmpEE0;struct Cyc_Absyn_Malloc_e_struct*_tmpEDF;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpEDF=_cycalloc(sizeof(*_tmpEDF)),((_tmpEDF[0]=((_tmpEE2.tag=34,((
_tmpEE2.f1=((_tmpEE0.is_calloc=1,((_tmpEE0.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),((_tmpEE0.elt_type=((
_tmpEE1=_cycalloc(sizeof(*_tmpEE1)),((_tmpEE1[0]=_tmp805,_tmpEE1)))),((_tmpEE0.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((
_tmpEE0.fat_result=0,_tmpEE0)))))))))),_tmpEE2)))),_tmpEDF)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
467: _LL423:{struct Cyc_Absyn_Exp*_tmpEE3[1];yyval=Cyc_YY3(Cyc_Absyn_primop_exp(
Cyc_Absyn_Numelts,((_tmpEE3[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpEE3,sizeof(struct Cyc_Absyn_Exp*),1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 468:
_LL424:{struct Cyc_Absyn_Tagcheck_e_struct _tmpEE9;struct _dyneither_ptr*_tmpEE8;
struct Cyc_Absyn_Tagcheck_e_struct*_tmpEE7;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpEE7=_cycalloc(sizeof(*_tmpEE7)),((_tmpEE7[0]=((_tmpEE9.tag=38,((_tmpEE9.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpEE9.f2=((_tmpEE8=_cycalloc(sizeof(*_tmpEE8)),((_tmpEE8[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpEE8)))),_tmpEE9)))))),
_tmpEE7)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 469: _LL425:{struct Cyc_Absyn_Tagcheck_e_struct
_tmpEEF;struct _dyneither_ptr*_tmpEEE;struct Cyc_Absyn_Tagcheck_e_struct*_tmpEED;
yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEED=_cycalloc(sizeof(*_tmpEED)),((
_tmpEED[0]=((_tmpEEF.tag=38,((_tmpEEF.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)),((_tmpEEF.f2=((
_tmpEEE=_cycalloc(sizeof(*_tmpEEE)),((_tmpEEE[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpEEE)))),_tmpEEF)))))),
_tmpEED)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 470: _LL426: {void*_tmp812;struct _tuple8
_tmp811=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp812=_tmp811.f3;yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(_tmp812,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 471:
_LL427: yyval=Cyc_YY7(Cyc_Absyn_Bitnot);break;case 472: _LL428: yyval=Cyc_YY7(Cyc_Absyn_Not);
break;case 473: _LL429: yyval=Cyc_YY7(Cyc_Absyn_Minus);break;case 474: _LL42A: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 475: _LL42B:
yyval=Cyc_YY3(Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 476:
_LL42C: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 477:
_LL42D: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 478:
_LL42E:{struct _dyneither_ptr*_tmpEF0;yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpEF0=_cycalloc(
sizeof(*_tmpEF0)),((_tmpEF0[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEF0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 479:
_LL42F:{struct _dyneither_ptr*_tmpEF1;yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpEF1=_cycalloc(
sizeof(*_tmpEF1)),((_tmpEF1[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEF1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 480:
_LL430: yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 481:
_LL431: yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 482:
_LL432:{struct Cyc_Absyn_CompoundLit_e_struct _tmpEF4;struct Cyc_Absyn_CompoundLit_e_struct*
_tmpEF3;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEF3=_cycalloc(sizeof(*
_tmpEF3)),((_tmpEF3[0]=((_tmpEF4.tag=26,((_tmpEF4.f1=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpEF4.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmpEF4)))))),_tmpEF3)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 483: _LL433:{struct Cyc_Absyn_CompoundLit_e_struct
_tmpEF7;struct Cyc_Absyn_CompoundLit_e_struct*_tmpEF6;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpEF6=_cycalloc(sizeof(*_tmpEF6)),((_tmpEF6[0]=((_tmpEF7.tag=26,((
_tmpEF7.f1=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 5)]),((_tmpEF7.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),
_tmpEF7)))))),_tmpEF6)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 484:
_LL434:{struct Cyc_Absyn_UnknownId_e_struct _tmpEFA;struct Cyc_Absyn_UnknownId_e_struct*
_tmpEF9;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEF9=_cycalloc(sizeof(*
_tmpEF9)),((_tmpEF9[0]=((_tmpEFA.tag=2,((_tmpEFA.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEFA)))),_tmpEF9)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 485:
_LL435: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
486: _LL436: yyval=Cyc_YY3(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 487:
_LL437: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 488: _LL438: yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 489:
_LL439: yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 490:
_LL43A: yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 491:
_LL43B:{struct Cyc_Absyn_Aggregate_e_struct _tmpEFD;struct Cyc_Absyn_Aggregate_e_struct*
_tmpEFC;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEFC=_cycalloc(sizeof(*
_tmpEFC)),((_tmpEFC[0]=((_tmpEFD.tag=29,((_tmpEFD.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpEFD.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpEFD.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmpEFD.f4=0,
_tmpEFD)))))))))),_tmpEFC)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 492:
_LL43C: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 493:
_LL43D: yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));break;
case 494: _LL43E:{struct Cyc_List_List*_tmpEFE;yyval=Cyc_YY5(((_tmpEFE=_cycalloc(
sizeof(*_tmpEFE)),((_tmpEFE->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEFE->tl=0,_tmpEFE)))))));}
break;case 495: _LL43F:{struct Cyc_List_List*_tmpEFF;yyval=Cyc_YY5(((_tmpEFF=
_cycalloc(sizeof(*_tmpEFF)),((_tmpEFF->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEFF->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpEFF)))))));}
break;case 496: _LL440: yyval=Cyc_YY3(Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 497:
_LL441: yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 498:
_LL442: yyval=Cyc_YY3(Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 499:
_LL443: yyval=Cyc_YY3(Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 500:
_LL444:{struct _dyneither_ptr*_tmpF02;struct _tuple1*_tmpF01;yyval=Cyc_QualId_tok(((
_tmpF01=_cycalloc(sizeof(*_tmpF01)),((_tmpF01->f1=Cyc_Absyn_Rel_n(0),((_tmpF01->f2=((
_tmpF02=_cycalloc(sizeof(*_tmpF02)),((_tmpF02[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF02)))),_tmpF01)))))));}
break;case 501: _LL445: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 502: _LL446:{struct _dyneither_ptr*_tmpF05;struct _tuple1*
_tmpF04;yyval=Cyc_QualId_tok(((_tmpF04=_cycalloc(sizeof(*_tmpF04)),((_tmpF04->f1=
Cyc_Absyn_Rel_n(0),((_tmpF04->f2=((_tmpF05=_cycalloc(sizeof(*_tmpF05)),((_tmpF05[
0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpF05)))),_tmpF04)))))));}break;case 503: _LL447: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 504: _LL448: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 505: _LL449:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 506:
_LL44A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
507: _LL44B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 508: _LL44C: break;case 509: _LL44D: yylex_buf->lex_curr_pos -=1;break;default:
_LL44E: break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;++ yylsp_offset;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(510,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(143,yyn - 143)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
5862) && Cyc_yycheck[_check_known_subscript_notnull(5863,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(5863,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(143,yyn - 143)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1027,yystate)];if(yyn > - 32768  && yyn < 5862){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 286 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(5863,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(286,x)])+ 15,count ++);}{unsigned int _tmpF0A;
unsigned int _tmpF09;struct _dyneither_ptr _tmpF08;char*_tmpF07;unsigned int _tmpF06;
msg=((_tmpF06=(unsigned int)(sze + 15),((_tmpF07=(char*)_region_malloc(yyregion,
_check_times(sizeof(char),_tmpF06 + 1)),((_tmpF08=_tag_dyneither(_tmpF07,sizeof(
char),_tmpF06 + 1),((((_tmpF09=_tmpF06,((_tmpF0B(& _tmpF0A,& _tmpF09,& _tmpF07),
_tmpF07[_tmpF09]=(char)0)))),_tmpF08))))))));}{const char*_tmpF0C;Cyc_strcpy(msg,((
_tmpF0C="parse error",_tag_dyneither(_tmpF0C,sizeof(char),12))));}if(count < 5){
count=0;for(x=yyn < 0?- yyn: 0;x < 286 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(5863,x + yyn)]== x){{const char*_tmpF0E;const char*
_tmpF0D;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)((
_tmpF0E=", expecting `",_tag_dyneither(_tmpF0E,sizeof(char),14))):(struct
_dyneither_ptr)((_tmpF0D=" or `",_tag_dyneither(_tmpF0D,sizeof(char),6)))));}Cyc_strcat(
msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(286,x)]);{
const char*_tmpF0F;Cyc_strcat(msg,((_tmpF0F="'",_tag_dyneither(_tmpF0F,sizeof(
char),2))));}++ count;}}}Cyc_yyerror((struct _dyneither_ptr)msg);}else{const char*
_tmpF10;Cyc_yyerror(((_tmpF10="parse error",_tag_dyneither(_tmpF10,sizeof(char),
12))));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int
_tmp82C=1;_npop_handler(0);return _tmp82C;}Cyc_yychar=- 2;}yyerrstatus=3;goto
yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp82D=1;
_npop_handler(0);return _tmp82D;}-- yyvsp_offset;yystate=(int)yyss[
_check_known_subscript_notnull(10000,-- yyssp_offset)];-- yylsp_offset;yyerrhandle:
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1027,yystate)];if(yyn == -
32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 5862) || Cyc_yycheck[
_check_known_subscript_notnull(5863,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5863,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 1026){int
_tmp82E=0;_npop_handler(0);return _tmp82E;}yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE v);void Cyc_yyprint(int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE
_tmp838=v;struct _tuple6 _tmp839;int _tmp83A;char _tmp83B;struct _dyneither_ptr
_tmp83C;struct _tuple1*_tmp83D;struct _tuple1 _tmp83E;union Cyc_Absyn_Nmspace _tmp83F;
struct _dyneither_ptr*_tmp840;_LL451: if((_tmp838.Int_tok).tag != 1)goto _LL453;
_tmp839=(struct _tuple6)(_tmp838.Int_tok).val;_tmp83A=_tmp839.f2;_LL452:{const
char*_tmpF14;void*_tmpF13[1];struct Cyc_Int_pa_struct _tmpF12;(_tmpF12.tag=1,((
_tmpF12.f1=(unsigned long)_tmp83A,((_tmpF13[0]=& _tmpF12,Cyc_fprintf(Cyc_stderr,((
_tmpF14="%d",_tag_dyneither(_tmpF14,sizeof(char),3))),_tag_dyneither(_tmpF13,
sizeof(void*),1)))))));}goto _LL450;_LL453: if((_tmp838.Char_tok).tag != 2)goto
_LL455;_tmp83B=(char)(_tmp838.Char_tok).val;_LL454:{const char*_tmpF18;void*
_tmpF17[1];struct Cyc_Int_pa_struct _tmpF16;(_tmpF16.tag=1,((_tmpF16.f1=(
unsigned long)((int)_tmp83B),((_tmpF17[0]=& _tmpF16,Cyc_fprintf(Cyc_stderr,((
_tmpF18="%c",_tag_dyneither(_tmpF18,sizeof(char),3))),_tag_dyneither(_tmpF17,
sizeof(void*),1)))))));}goto _LL450;_LL455: if((_tmp838.String_tok).tag != 3)goto
_LL457;_tmp83C=(struct _dyneither_ptr)(_tmp838.String_tok).val;_LL456:{const char*
_tmpF1C;void*_tmpF1B[1];struct Cyc_String_pa_struct _tmpF1A;(_tmpF1A.tag=0,((
_tmpF1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp83C),((_tmpF1B[0]=&
_tmpF1A,Cyc_fprintf(Cyc_stderr,((_tmpF1C="\"%s\"",_tag_dyneither(_tmpF1C,sizeof(
char),5))),_tag_dyneither(_tmpF1B,sizeof(void*),1)))))));}goto _LL450;_LL457: if((
_tmp838.QualId_tok).tag != 5)goto _LL459;_tmp83D=(struct _tuple1*)(_tmp838.QualId_tok).val;
_tmp83E=*_tmp83D;_tmp83F=_tmp83E.f1;_tmp840=_tmp83E.f2;_LL458: {struct Cyc_List_List*
_tmp84A=0;{union Cyc_Absyn_Nmspace _tmp84B=_tmp83F;struct Cyc_List_List*_tmp84C;
struct Cyc_List_List*_tmp84D;int _tmp84E;_LL45C: if((_tmp84B.Rel_n).tag != 1)goto
_LL45E;_tmp84C=(struct Cyc_List_List*)(_tmp84B.Rel_n).val;_LL45D: _tmp84A=_tmp84C;
goto _LL45B;_LL45E: if((_tmp84B.Abs_n).tag != 2)goto _LL460;_tmp84D=(struct Cyc_List_List*)(
_tmp84B.Abs_n).val;_LL45F: _tmp84A=_tmp84D;goto _LL45B;_LL460: if((_tmp84B.Loc_n).tag
!= 3)goto _LL45B;_tmp84E=(int)(_tmp84B.Loc_n).val;_LL461: goto _LL45B;_LL45B:;}for(
0;_tmp84A != 0;_tmp84A=_tmp84A->tl){const char*_tmpF20;void*_tmpF1F[1];struct Cyc_String_pa_struct
_tmpF1E;(_tmpF1E.tag=0,((_tmpF1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp84A->hd)),((_tmpF1F[0]=& _tmpF1E,Cyc_fprintf(Cyc_stderr,((
_tmpF20="%s::",_tag_dyneither(_tmpF20,sizeof(char),5))),_tag_dyneither(_tmpF1F,
sizeof(void*),1)))))));}{const char*_tmpF24;void*_tmpF23[1];struct Cyc_String_pa_struct
_tmpF22;(_tmpF22.tag=0,((_tmpF22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp840),((_tmpF23[0]=& _tmpF22,Cyc_fprintf(Cyc_stderr,((_tmpF24="%s::",
_tag_dyneither(_tmpF24,sizeof(char),5))),_tag_dyneither(_tmpF23,sizeof(void*),1)))))));}
goto _LL450;}_LL459:;_LL45A:{const char*_tmpF27;void*_tmpF26;(_tmpF26=0,Cyc_fprintf(
Cyc_stderr,((_tmpF27="?",_tag_dyneither(_tmpF27,sizeof(char),2))),_tag_dyneither(
_tmpF26,sizeof(void*),0)));}goto _LL450;_LL450:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*
f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(f));return Cyc_Parse_parse_result;}
