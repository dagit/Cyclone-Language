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
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};double modf(double,
double*);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_putc(int,struct
Cyc___cycFILE*);struct _dyneither_ptr Cyc_rprintf(struct _RegionHandle*,struct
_dyneither_ptr,struct _dyneither_ptr);int Cyc_vfprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);int Cyc_vprintf(struct _dyneither_ptr,struct
_dyneither_ptr);struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct
_dyneither_ptr,struct _dyneither_ptr);int Cyc_vsnprintf(struct _dyneither_ptr,
unsigned long,struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_vsprintf(struct
_dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{
void*v;};extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz);
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < 
dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};static
struct _dyneither_ptr Cyc_parg2string(void*x);static struct _dyneither_ptr Cyc_parg2string(
void*x){void*_tmp0=x;_LL1: {struct Cyc_String_pa_struct*_tmp1=(struct Cyc_String_pa_struct*)
_tmp0;if(_tmp1->tag != 0)goto _LL3;}_LL2: {const char*_tmpCD;return(_tmpCD="string",
_tag_dyneither(_tmpCD,sizeof(char),7));}_LL3: {struct Cyc_Int_pa_struct*_tmp2=(
struct Cyc_Int_pa_struct*)_tmp0;if(_tmp2->tag != 1)goto _LL5;}_LL4: {const char*
_tmpCE;return(_tmpCE="int",_tag_dyneither(_tmpCE,sizeof(char),4));}_LL5: {struct
Cyc_Double_pa_struct*_tmp3=(struct Cyc_Double_pa_struct*)_tmp0;if(_tmp3->tag != 2)
goto _LL7;}_LL6: {const char*_tmpCF;return(_tmpCF="double",_tag_dyneither(_tmpCF,
sizeof(char),7));}_LL7: {struct Cyc_LongDouble_pa_struct*_tmp4=(struct Cyc_LongDouble_pa_struct*)
_tmp0;if(_tmp4->tag != 3)goto _LL9;}_LL8: {const char*_tmpD0;return(_tmpD0="long double",
_tag_dyneither(_tmpD0,sizeof(char),12));}_LL9: {struct Cyc_ShortPtr_pa_struct*
_tmp5=(struct Cyc_ShortPtr_pa_struct*)_tmp0;if(_tmp5->tag != 4)goto _LLB;}_LLA: {
const char*_tmpD1;return(_tmpD1="short *",_tag_dyneither(_tmpD1,sizeof(char),8));}
_LLB: {struct Cyc_IntPtr_pa_struct*_tmp6=(struct Cyc_IntPtr_pa_struct*)_tmp0;if(
_tmp6->tag != 5)goto _LL0;}_LLC: {const char*_tmpD2;return(_tmpD2="unsigned long *",
_tag_dyneither(_tmpD2,sizeof(char),16));}_LL0:;}static void*Cyc_badarg(struct
_dyneither_ptr s);static void*Cyc_badarg(struct _dyneither_ptr s){struct Cyc_Core_Invalid_argument_struct
_tmpD5;struct Cyc_Core_Invalid_argument_struct*_tmpD4;return(void*)_throw((void*)((
_tmpD4=_cycalloc(sizeof(*_tmpD4)),((_tmpD4[0]=((_tmpD5.tag=Cyc_Core_Invalid_argument,((
_tmpD5.f1=s,_tmpD5)))),_tmpD4)))));}static int Cyc_va_arg_int(struct _dyneither_ptr
ap);static int Cyc_va_arg_int(struct _dyneither_ptr ap){void*_tmpF=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0));unsigned long _tmp11;_LLE: {
struct Cyc_Int_pa_struct*_tmp10=(struct Cyc_Int_pa_struct*)_tmpF;if(_tmp10->tag != 
1)goto _LL10;else{_tmp11=_tmp10->f1;}}_LLF: return(int)_tmp11;_LL10:;_LL11: {const
char*_tmpD6;return((int(*)(struct _dyneither_ptr s))Cyc_badarg)(((_tmpD6="printf expected int",
_tag_dyneither(_tmpD6,sizeof(char),20))));}_LLD:;}static long Cyc_va_arg_long(
struct _dyneither_ptr ap);static long Cyc_va_arg_long(struct _dyneither_ptr ap){void*
_tmp13=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));unsigned long
_tmp15;_LL13: {struct Cyc_Int_pa_struct*_tmp14=(struct Cyc_Int_pa_struct*)_tmp13;
if(_tmp14->tag != 1)goto _LL15;else{_tmp15=_tmp14->f1;}}_LL14: return(long)_tmp15;
_LL15:;_LL16: {const char*_tmpD7;return((long(*)(struct _dyneither_ptr s))Cyc_badarg)(((
_tmpD7="printf expected int",_tag_dyneither(_tmpD7,sizeof(char),20))));}_LL12:;}
static unsigned long Cyc_va_arg_ulong(struct _dyneither_ptr ap);static unsigned long
Cyc_va_arg_ulong(struct _dyneither_ptr ap){void*_tmp17=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0));unsigned long _tmp19;_LL18: {
struct Cyc_Int_pa_struct*_tmp18=(struct Cyc_Int_pa_struct*)_tmp17;if(_tmp18->tag != 
1)goto _LL1A;else{_tmp19=_tmp18->f1;}}_LL19: return(unsigned long)_tmp19;_LL1A:;
_LL1B: {const char*_tmpD8;return((unsigned long(*)(struct _dyneither_ptr s))Cyc_badarg)(((
_tmpD8="printf expected int",_tag_dyneither(_tmpD8,sizeof(char),20))));}_LL17:;}
static unsigned long Cyc_va_arg_uint(struct _dyneither_ptr ap);static unsigned long
Cyc_va_arg_uint(struct _dyneither_ptr ap){void*_tmp1B=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0));unsigned long _tmp1D;_LL1D: {
struct Cyc_Int_pa_struct*_tmp1C=(struct Cyc_Int_pa_struct*)_tmp1B;if(_tmp1C->tag != 
1)goto _LL1F;else{_tmp1D=_tmp1C->f1;}}_LL1E: return(unsigned int)_tmp1D;_LL1F:;
_LL20: {const char*_tmpD9;return((unsigned long(*)(struct _dyneither_ptr s))Cyc_badarg)(((
_tmpD9="printf expected int",_tag_dyneither(_tmpD9,sizeof(char),20))));}_LL1C:;}
static double Cyc_va_arg_double(struct _dyneither_ptr ap);static double Cyc_va_arg_double(
struct _dyneither_ptr ap){void*_tmp1F=*((void**)_check_dyneither_subscript(ap,
sizeof(void*),0));double _tmp21;long double _tmp23;_LL22: {struct Cyc_Double_pa_struct*
_tmp20=(struct Cyc_Double_pa_struct*)_tmp1F;if(_tmp20->tag != 2)goto _LL24;else{
_tmp21=_tmp20->f1;}}_LL23: return _tmp21;_LL24: {struct Cyc_LongDouble_pa_struct*
_tmp22=(struct Cyc_LongDouble_pa_struct*)_tmp1F;if(_tmp22->tag != 3)goto _LL26;
else{_tmp23=_tmp22->f1;}}_LL25: return(double)_tmp23;_LL26:;_LL27: {const char*
_tmpDD;void*_tmpDC[1];struct Cyc_String_pa_struct _tmpDB;(int)_throw(((void*(*)(
struct _dyneither_ptr s))Cyc_badarg)((struct _dyneither_ptr)((_tmpDB.tag=0,((_tmpDB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_parg2string(*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0)))),((_tmpDC[0]=& _tmpDB,Cyc_aprintf(((
_tmpDD="printf expected double but found %s",_tag_dyneither(_tmpDD,sizeof(char),
36))),_tag_dyneither(_tmpDC,sizeof(void*),1))))))))));}_LL21:;}static short*Cyc_va_arg_short_ptr(
struct _dyneither_ptr ap);static short*Cyc_va_arg_short_ptr(struct _dyneither_ptr ap){
void*_tmp27=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));short*
_tmp29;_LL29: {struct Cyc_ShortPtr_pa_struct*_tmp28=(struct Cyc_ShortPtr_pa_struct*)
_tmp27;if(_tmp28->tag != 4)goto _LL2B;else{_tmp29=_tmp28->f1;}}_LL2A: return _tmp29;
_LL2B:;_LL2C: {const char*_tmpDE;(int)_throw(((void*(*)(struct _dyneither_ptr s))
Cyc_badarg)(((_tmpDE="printf expected short pointer",_tag_dyneither(_tmpDE,
sizeof(char),30)))));}_LL28:;}static unsigned long*Cyc_va_arg_int_ptr(struct
_dyneither_ptr ap);static unsigned long*Cyc_va_arg_int_ptr(struct _dyneither_ptr ap){
void*_tmp2B=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));
unsigned long*_tmp2D;_LL2E: {struct Cyc_IntPtr_pa_struct*_tmp2C=(struct Cyc_IntPtr_pa_struct*)
_tmp2B;if(_tmp2C->tag != 5)goto _LL30;else{_tmp2D=_tmp2C->f1;}}_LL2F: return _tmp2D;
_LL30:;_LL31: {const char*_tmpDF;(int)_throw(((void*(*)(struct _dyneither_ptr s))
Cyc_badarg)(((_tmpDF="printf expected int pointer",_tag_dyneither(_tmpDF,sizeof(
char),28)))));}_LL2D:;}static struct _dyneither_ptr Cyc_va_arg_string(struct
_dyneither_ptr ap);static struct _dyneither_ptr Cyc_va_arg_string(struct
_dyneither_ptr ap){void*_tmp2F=*((void**)_check_dyneither_subscript(ap,sizeof(
void*),0));struct _dyneither_ptr _tmp31;_LL33: {struct Cyc_String_pa_struct*_tmp30=(
struct Cyc_String_pa_struct*)_tmp2F;if(_tmp30->tag != 0)goto _LL35;else{_tmp31=
_tmp30->f1;}}_LL34: return _tmp31;_LL35:;_LL36: {const char*_tmpE0;(int)_throw(((
void*(*)(struct _dyneither_ptr s))Cyc_badarg)(((_tmpE0="printf expected string",
_tag_dyneither(_tmpE0,sizeof(char),23)))));}_LL32:;}int Cyc___cvt_double(double
number,int prec,int flags,int*signp,int fmtch,struct _dyneither_ptr startp,struct
_dyneither_ptr endp);enum Cyc_BASE{Cyc_OCT  = 0,Cyc_DEC  = 1,Cyc_HEX  = 2};static int
Cyc__IO_sputn(int(*ioputc)(int,void*),void*ioputc_env,struct _dyneither_ptr s,int
howmany);inline static int Cyc__IO_sputn(int(*ioputc)(int,void*),void*ioputc_env,
struct _dyneither_ptr s,int howmany){int i=0;while(i < howmany){if(ioputc((int)*((
const char*)_check_dyneither_subscript(s,sizeof(char),0)),ioputc_env)== - 1)return
i;_dyneither_ptr_inplace_plus(& s,sizeof(char),1);++ i;}return i;}static int Cyc__IO_nzsputn(
int(*ioputc)(int,void*),void*ioputc_env,struct _dyneither_ptr s,int howmany);static
int Cyc__IO_nzsputn(int(*ioputc)(int,void*),void*ioputc_env,struct _dyneither_ptr s,
int howmany){int i=0;while(i < howmany){char c;if((c=*((const char*)
_check_dyneither_subscript(s,sizeof(char),0)))== 0  || ioputc((int)c,ioputc_env)
== - 1)return i;_dyneither_ptr_inplace_plus(& s,sizeof(char),1);++ i;}return i;}
static int Cyc__IO_padn(int(*ioputc)(int,void*),void*ioputc_env,char c,int howmany);
static int Cyc__IO_padn(int(*ioputc)(int,void*),void*ioputc_env,char c,int howmany){
int i=0;while(i < howmany){if(ioputc((int)c,ioputc_env)== - 1)return i;++ i;}return i;}
static struct _dyneither_ptr Cyc_my_memchr(struct _dyneither_ptr s,char c,int n);static
struct _dyneither_ptr Cyc_my_memchr(struct _dyneither_ptr s,char c,int n){int sz=(int)
_get_dyneither_size(s,sizeof(char));n=n < sz?n: sz;for(0;n != 0;(n --,
_dyneither_ptr_inplace_plus_post(& s,sizeof(char),1))){if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),0))== c)return s;}return(struct
_dyneither_ptr)_tag_dyneither(0,0,0);}static struct _dyneither_ptr Cyc_my_nzmemchr(
struct _dyneither_ptr s,char c,int n);static struct _dyneither_ptr Cyc_my_nzmemchr(
struct _dyneither_ptr s,char c,int n){int sz=(int)_get_dyneither_size(s,sizeof(char));
n=n < sz?n: sz;for(0;n != 0;(n --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),1))){
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),0))== c)return s;}
return(struct _dyneither_ptr)_tag_dyneither(0,0,0);}static unsigned long Cyc_my_strlen(
struct _dyneither_ptr s);inline static unsigned long Cyc_my_strlen(struct
_dyneither_ptr s){unsigned int sz=_get_dyneither_size(s,sizeof(char));unsigned int
i=0;while(i < sz  && ((const char*)s.curr)[(int)i]!= 0){++ i;}return i;}int Cyc__IO_vfprintf(
int(*ioputc)(int,void*),void*ioputc_env,struct _dyneither_ptr fmt0,struct
_dyneither_ptr ap);int Cyc__IO_vfprintf(int(*ioputc)(int,void*),void*ioputc_env,
struct _dyneither_ptr fmt0,struct _dyneither_ptr ap){struct _dyneither_ptr fmt;
register int ch;register int n;struct _dyneither_ptr cp=_tag_dyneither(0,0,0);struct
_dyneither_ptr cp2=(struct _dyneither_ptr)_tag_dyneither(0,0,0);struct
_dyneither_ptr cp3=(struct _dyneither_ptr)_tag_dyneither(0,0,0);int which_cp;struct
_dyneither_ptr fmark;register int flags;int ret;int width;int prec;char sign;char
sign_string[2]={'\000','\000'};int softsign=0;double _double;int fpprec;
unsigned long _ulong;int dprec;int dpad;int fieldsz;int size=0;char buf[349];{
unsigned int _tmp58=348;unsigned int i;for(i=0;i < _tmp58;i ++){buf[i]='\000';}buf[
_tmp58]=(char)0;}{char ox[2]={'\000','\000'};enum Cyc_BASE base;fmt=fmt0;ret=0;
for(0;1;0){fmark=fmt;{unsigned int fmt_sz=_get_dyneither_size(fmt,sizeof(char));
for(n=0;(n < fmt_sz  && (ch=(int)((const char*)fmt.curr)[n])!= '\000') && ch != '%';
++ n){;}fmt=_dyneither_ptr_plus(fmt,sizeof(char),n);if((n=(fmt.curr - fmark.curr)/ 
sizeof(char))!= 0){do{if(Cyc__IO_sputn(ioputc,ioputc_env,(struct _dyneither_ptr)
fmark,n)!= n)goto error;}while(0);ret +=n;}if(ch == '\000')goto done;
_dyneither_ptr_inplace_plus(& fmt,sizeof(char),1);flags=0;dprec=0;fpprec=0;width=
0;prec=- 1;sign='\000';rflag: ch=(int)*((const char*)_check_dyneither_subscript(
_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));reswitch:
which_cp=0;switch(ch){case ' ': _LL37: if(!((int)sign))sign=' ';goto rflag;case '#':
_LL38: flags |=8;goto rflag;case '*': _LL39: width=Cyc_va_arg_int(ap);
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);if(width >= 0)goto rflag;width=-
width;goto _LL3A;case '-': _LL3A: flags |=16;flags &=~ 32;goto rflag;case '+': _LL3B: sign='+';
goto rflag;case '.': _LL3C: if((ch=(int)*((const char*)_check_dyneither_subscript(
_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0)))== '*'){n=
Cyc_va_arg_int(ap);_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);prec=n < 0?- 1:
n;goto rflag;}n=0;while((unsigned int)(ch - '0')<= 9){n=10 * n + (ch - '0');ch=(int)*((
const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,
sizeof(char),1),sizeof(char),0));}prec=n < 0?- 1: n;goto reswitch;case '0': _LL3D: if(!(
flags & 16))flags |=32;goto rflag;case '1': _LL3E: goto _LL3F;case '2': _LL3F: goto _LL40;
case '3': _LL40: goto _LL41;case '4': _LL41: goto _LL42;case '5': _LL42: goto _LL43;case '6':
_LL43: goto _LL44;case '7': _LL44: goto _LL45;case '8': _LL45: goto _LL46;case '9': _LL46: n=
0;do{n=10 * n + (ch - '0');ch=(int)*((const char*)_check_dyneither_subscript(
_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));}while((
unsigned int)(ch - '0')<= 9);width=n;goto reswitch;case 'L': _LL47: flags |=2;goto
rflag;case 'h': _LL48: flags |=4;goto rflag;case 'l': _LL49: flags |=1;goto rflag;case 'c':
_LL4A:{char*_tmpE1;cp=((_tmpE1=buf,_tag_dyneither(_tmpE1,sizeof(char),
_get_zero_arr_size_char((void*)_tmpE1,349))));}{char _tmpE4;char _tmpE3;struct
_dyneither_ptr _tmpE2;(_tmpE2=cp,((_tmpE3=*((char*)_check_dyneither_subscript(
_tmpE2,sizeof(char),0)),((_tmpE4=(char)Cyc_va_arg_int(ap),((_get_dyneither_size(
_tmpE2,sizeof(char))== 1  && (_tmpE3 == '\000'  && _tmpE4 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpE2.curr)=_tmpE4)))))));}
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);size=1;sign='\000';break;case 'D':
_LL4B: flags |=1;goto _LL4C;case 'd': _LL4C: goto _LL4D;case 'i': _LL4D: _ulong=(
unsigned long)(flags & 1?Cyc_va_arg_long(ap):(flags & 4?(long)((short)Cyc_va_arg_int(
ap)):(long)Cyc_va_arg_int(ap)));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
if((long)_ulong < 0){_ulong=- _ulong;sign='-';}base=Cyc_DEC;goto number;case 'e':
_LL4E: goto _LL4F;case 'E': _LL4F: goto _LL50;case 'f': _LL50: goto _LL51;case 'F': _LL51:
goto _LL52;case 'g': _LL52: goto _LL53;case 'G': _LL53: _double=Cyc_va_arg_double(ap);
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);if(prec > 39){if(ch != 'g'  && ch
!= 'G'  || flags & 8)fpprec=prec - 39;prec=39;}else{if(prec == - 1)prec=6;}{char*
_tmpE5;cp=((_tmpE5=buf,_tag_dyneither(_tmpE5,sizeof(char),
_get_zero_arr_size_char((void*)_tmpE5,349))));}{char _tmpE8;char _tmpE7;struct
_dyneither_ptr _tmpE6;(_tmpE6=cp,((_tmpE7=*((char*)_check_dyneither_subscript(
_tmpE6,sizeof(char),0)),((_tmpE8='\000',((_get_dyneither_size(_tmpE6,sizeof(char))
== 1  && (_tmpE7 == '\000'  && _tmpE8 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmpE6.curr)=_tmpE8)))))));}{char*_tmpE9;size=Cyc___cvt_double(_double,prec,
flags,& softsign,ch,cp,_dyneither_ptr_plus(((_tmpE9=buf,_tag_dyneither(_tmpE9,
sizeof(char),_get_zero_arr_size_char((void*)_tmpE9,349)))),sizeof(char),(int)(
sizeof(buf)- 1)));}if(softsign)sign='-';if(*((char*)_check_dyneither_subscript(
cp,sizeof(char),0))== '\000')_dyneither_ptr_inplace_plus(& cp,sizeof(char),1);
break;case 'n': _LL54: if(flags & 1)*Cyc_va_arg_int_ptr(ap)=(unsigned long)ret;else{
if(flags & 4)*Cyc_va_arg_short_ptr(ap)=(short)ret;else{*Cyc_va_arg_int_ptr(ap)=(
unsigned long)ret;}}_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);continue;
case 'O': _LL55: flags |=1;goto _LL56;case 'o': _LL56: _ulong=flags & 1?Cyc_va_arg_ulong(
ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)):(unsigned long)
Cyc_va_arg_uint(ap));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);base=Cyc_OCT;
goto nosign;case 'p': _LL57: _ulong=(unsigned long)Cyc_va_arg_long(ap);
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);base=Cyc_HEX;flags |=64;ch=(int)'x';
goto nosign;case 's': _LL58: {struct _dyneither_ptr b=Cyc_va_arg_string(ap);
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);which_cp=3;cp3=b;if(prec >= 0){
struct _dyneither_ptr p=Cyc_my_nzmemchr(cp3,'\000',prec);if((char*)p.curr != (char*)(
_tag_dyneither(0,0,0)).curr){size=(p.curr - cp3.curr)/ sizeof(char);if(size > prec)
size=prec;}else{size=prec;}}else{size=(int)Cyc_my_strlen(cp3);}sign='\000';
break;}case 'U': _LL59: flags |=1;goto _LL5A;case 'u': _LL5A: _ulong=flags & 1?Cyc_va_arg_ulong(
ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)):(unsigned long)
Cyc_va_arg_uint(ap));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);base=Cyc_DEC;
goto nosign;case 'X': _LL5B: goto _LL5C;case 'x': _LL5C: _ulong=flags & 1?Cyc_va_arg_ulong(
ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)):(unsigned long)
Cyc_va_arg_uint(ap));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);base=Cyc_HEX;
if(flags & 8  && _ulong != 0)flags |=64;nosign: sign='\000';number: if((dprec=prec)>= 
0)flags &=~ 32;{char*_tmpEA;cp=_dyneither_ptr_plus(((_tmpEA=buf,_tag_dyneither(
_tmpEA,sizeof(char),_get_zero_arr_size_char((void*)_tmpEA,349)))),sizeof(char),(
308 + 39)+ 1);}if(_ulong != 0  || prec != 0){struct _dyneither_ptr xdigs;switch(base){
case Cyc_OCT: _LL5E: do{{char _tmpED;char _tmpEC;struct _dyneither_ptr _tmpEB;(_tmpEB=
_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1),((_tmpEC=*((char*)
_check_dyneither_subscript(_tmpEB,sizeof(char),0)),((_tmpED=(char)((_ulong & 7)+ '0'),((
_get_dyneither_size(_tmpEB,sizeof(char))== 1  && (_tmpEC == '\000'  && _tmpED != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpEB.curr)=_tmpED)))))));}_ulong >>=3;}while((
int)_ulong);if(flags & 8  && *((char*)_check_dyneither_subscript(cp,sizeof(char),0))
!= '0'){char _tmpF0;char _tmpEF;struct _dyneither_ptr _tmpEE;(_tmpEE=
_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1),((_tmpEF=*((char*)
_check_dyneither_subscript(_tmpEE,sizeof(char),0)),((_tmpF0='0',((
_get_dyneither_size(_tmpEE,sizeof(char))== 1  && (_tmpEF == '\000'  && _tmpF0 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpEE.curr)=_tmpF0)))))));}break;case Cyc_DEC:
_LL5F: while(_ulong >= 10){{char _tmpF3;char _tmpF2;struct _dyneither_ptr _tmpF1;(
_tmpF1=_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1),((_tmpF2=*((char*)
_check_dyneither_subscript(_tmpF1,sizeof(char),0)),((_tmpF3=(char)(_ulong % 10 + '0'),((
_get_dyneither_size(_tmpF1,sizeof(char))== 1  && (_tmpF2 == '\000'  && _tmpF3 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpF1.curr)=_tmpF3)))))));}_ulong /=10;}{char
_tmpF6;char _tmpF5;struct _dyneither_ptr _tmpF4;(_tmpF4=_dyneither_ptr_inplace_plus(&
cp,sizeof(char),-1),((_tmpF5=*((char*)_check_dyneither_subscript(_tmpF4,sizeof(
char),0)),((_tmpF6=(char)(_ulong + '0'),((_get_dyneither_size(_tmpF4,sizeof(char))
== 1  && (_tmpF5 == '\000'  && _tmpF6 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmpF4.curr)=_tmpF6)))))));}break;case Cyc_HEX: _LL60: if(ch == 'X'){const char*
_tmpF7;xdigs=((_tmpF7="0123456789ABCDEF",_tag_dyneither(_tmpF7,sizeof(char),17)));}
else{const char*_tmpF8;xdigs=((_tmpF8="0123456789abcdef",_tag_dyneither(_tmpF8,
sizeof(char),17)));}do{{char _tmpFB;char _tmpFA;struct _dyneither_ptr _tmpF9;(_tmpF9=
_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1),((_tmpFA=*((char*)
_check_dyneither_subscript(_tmpF9,sizeof(char),0)),((_tmpFB=*((const char*)
_check_dyneither_subscript(xdigs,sizeof(char),(int)(_ulong & 15))),((
_get_dyneither_size(_tmpF9,sizeof(char))== 1  && (_tmpFA == '\000'  && _tmpFB != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpF9.curr)=_tmpFB)))))));}_ulong >>=4;}while((
int)_ulong);break;}}{char*_tmpFC;size=((_dyneither_ptr_plus(((_tmpFC=buf,
_tag_dyneither(_tmpFC,sizeof(char),_get_zero_arr_size_char((void*)_tmpFC,349)))),
sizeof(char),(308 + 39)+ 1)).curr - cp.curr)/ sizeof(char);}skipsize: break;
default: _LL5D: if(ch == '\000')goto done;{char*_tmpFD;cp=((_tmpFD=buf,
_tag_dyneither(_tmpFD,sizeof(char),_get_zero_arr_size_char((void*)_tmpFD,349))));}{
char _tmp100;char _tmpFF;struct _dyneither_ptr _tmpFE;(_tmpFE=cp,((_tmpFF=*((char*)
_check_dyneither_subscript(_tmpFE,sizeof(char),0)),((_tmp100=(char)ch,((
_get_dyneither_size(_tmpFE,sizeof(char))== 1  && (_tmpFF == '\000'  && _tmp100 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpFE.curr)=_tmp100)))))));}size=1;sign='\000';
break;}fieldsz=size + fpprec;dpad=dprec - size;if(dpad < 0)dpad=0;if((int)sign)++
fieldsz;else{if(flags & 64)fieldsz +=2;}fieldsz +=dpad;if((flags & (16 | 32))== 0){
if(Cyc__IO_padn(ioputc,ioputc_env,' ',width - fieldsz)< width - fieldsz)goto error;}
if((int)sign){{char _tmp105;char _tmp104;struct _dyneither_ptr _tmp103;char*_tmp102;(
_tmp103=_dyneither_ptr_plus(((_tmp102=sign_string,_tag_dyneither(_tmp102,sizeof(
char),_get_zero_arr_size_char((void*)_tmp102,2)))),sizeof(char),0),((_tmp104=*((
char*)_check_dyneither_subscript(_tmp103,sizeof(char),0)),((_tmp105=sign,((
_get_dyneither_size(_tmp103,sizeof(char))== 1  && (_tmp104 == '\000'  && _tmp105 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp103.curr)=_tmp105)))))));}do{char*_tmp106;
if(Cyc__IO_sputn(ioputc,ioputc_env,(struct _dyneither_ptr)((_tmp106=sign_string,
_tag_dyneither(_tmp106,sizeof(char),_get_zero_arr_size_char((void*)_tmp106,2)))),
1)!= 1)goto error;}while(0);}else{if(flags & 64){ox[0]='0';ox[1]=(char)ch;do{if(
Cyc__IO_nzsputn(ioputc,ioputc_env,_tag_dyneither(ox,sizeof(char),2),2)!= 2)goto
error;}while(0);}}if((flags & (16 | 32))== 32){if(Cyc__IO_padn(ioputc,ioputc_env,'0',
width - fieldsz)< width - fieldsz)goto error;}if(Cyc__IO_padn(ioputc,ioputc_env,'0',
dpad)< dpad)goto error;if(which_cp == 0)do{if(Cyc__IO_sputn(ioputc,ioputc_env,(
struct _dyneither_ptr)cp,size)!= size)goto error;}while(0);else{if(which_cp == 2)
do{if(Cyc__IO_sputn(ioputc,ioputc_env,(struct _dyneither_ptr)cp2,size)!= size)
goto error;}while(0);else{if(which_cp == 3)do{if(Cyc__IO_nzsputn(ioputc,ioputc_env,(
struct _dyneither_ptr)cp3,size)!= size)goto error;}while(0);}}if(Cyc__IO_padn(
ioputc,ioputc_env,'0',fpprec)< fpprec)goto error;if(flags & 16){if(Cyc__IO_padn(
ioputc,ioputc_env,' ',width - fieldsz)< width - fieldsz)goto error;}ret +=width > 
fieldsz?width: fieldsz;};}done: return ret;error: return - 1;};}static struct
_dyneither_ptr Cyc_exponent(struct _dyneither_ptr p,int exp,int fmtch);static struct
_dyneither_ptr Cyc_exponent(struct _dyneither_ptr p,int exp,int fmtch){struct
_dyneither_ptr t;char expbuffer[309];{unsigned int _tmp72=308;unsigned int i;for(i=0;
i < _tmp72;i ++){expbuffer[i]='0';}expbuffer[_tmp72]=(char)0;}{char*_tmp107;struct
_dyneither_ptr expbuf=(_tmp107=(char*)expbuffer,_tag_dyneither(_tmp107,sizeof(
char),_get_zero_arr_size_char((void*)_tmp107,309)));{char _tmp10A;char _tmp109;
struct _dyneither_ptr _tmp108;(_tmp108=_dyneither_ptr_inplace_plus_post(& p,sizeof(
char),1),((_tmp109=*((char*)_check_dyneither_subscript(_tmp108,sizeof(char),0)),((
_tmp10A=(char)fmtch,((_get_dyneither_size(_tmp108,sizeof(char))== 1  && (_tmp109
== '\000'  && _tmp10A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp108.curr)=
_tmp10A)))))));}if(exp < 0){exp=- exp;{char _tmp10D;char _tmp10C;struct
_dyneither_ptr _tmp10B;(_tmp10B=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),
1),((_tmp10C=*((char*)_check_dyneither_subscript(_tmp10B,sizeof(char),0)),((
_tmp10D='-',((_get_dyneither_size(_tmp10B,sizeof(char))== 1  && (_tmp10C == '\000'
 && _tmp10D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp10B.curr)=_tmp10D)))))));};}
else{char _tmp110;char _tmp10F;struct _dyneither_ptr _tmp10E;(_tmp10E=
_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmp10F=*((char*)
_check_dyneither_subscript(_tmp10E,sizeof(char),0)),((_tmp110='+',((
_get_dyneither_size(_tmp10E,sizeof(char))== 1  && (_tmp10F == '\000'  && _tmp110 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp10E.curr)=_tmp110)))))));}t=
_dyneither_ptr_plus(expbuf,sizeof(char),308);if(exp > 9){do{char _tmp113;char
_tmp112;struct _dyneither_ptr _tmp111;(_tmp111=_dyneither_ptr_inplace_plus(& t,
sizeof(char),-1),((_tmp112=*((char*)_check_dyneither_subscript(_tmp111,sizeof(
char),0)),((_tmp113=(char)(exp % 10 + '0'),((_get_dyneither_size(_tmp111,sizeof(
char))== 1  && (_tmp112 == '\000'  && _tmp113 != '\000')?_throw_arraybounds(): 1,*((
char*)_tmp111.curr)=_tmp113)))))));}while((exp /=10)> 9);{char _tmp116;char _tmp115;
struct _dyneither_ptr _tmp114;(_tmp114=_dyneither_ptr_inplace_plus(& t,sizeof(char),
-1),((_tmp115=*((char*)_check_dyneither_subscript(_tmp114,sizeof(char),0)),((
_tmp116=(char)(exp + '0'),((_get_dyneither_size(_tmp114,sizeof(char))== 1  && (
_tmp115 == '\000'  && _tmp116 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp114.curr)=
_tmp116)))))));}{char _tmp119;char _tmp118;struct _dyneither_ptr _tmp117;for(0;(char*)
t.curr < (char*)(_dyneither_ptr_plus(expbuf,sizeof(char),308)).curr;(_tmp117=
_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmp118=*((char*)
_check_dyneither_subscript(_tmp117,sizeof(char),0)),((_tmp119=*((char*)
_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),
sizeof(char),0)),((_get_dyneither_size(_tmp117,sizeof(char))== 1  && (_tmp118 == '\000'
 && _tmp119 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp117.curr)=_tmp119)))))))){;}};}
else{{char _tmp11C;char _tmp11B;struct _dyneither_ptr _tmp11A;(_tmp11A=
_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmp11B=*((char*)
_check_dyneither_subscript(_tmp11A,sizeof(char),0)),((_tmp11C='0',((
_get_dyneither_size(_tmp11A,sizeof(char))== 1  && (_tmp11B == '\000'  && _tmp11C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp11A.curr)=_tmp11C)))))));}{char _tmp11F;char
_tmp11E;struct _dyneither_ptr _tmp11D;(_tmp11D=_dyneither_ptr_inplace_plus_post(& p,
sizeof(char),1),((_tmp11E=*((char*)_check_dyneither_subscript(_tmp11D,sizeof(
char),0)),((_tmp11F=(char)(exp + '0'),((_get_dyneither_size(_tmp11D,sizeof(char))
== 1  && (_tmp11E == '\000'  && _tmp11F != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp11D.curr)=_tmp11F)))))));};}return p;};}static struct _dyneither_ptr Cyc_sround(
double fract,int*exp,struct _dyneither_ptr start,struct _dyneither_ptr end,char ch,int*
signp);static struct _dyneither_ptr Cyc_sround(double fract,int*exp,struct
_dyneither_ptr start,struct _dyneither_ptr end,char ch,int*signp){double tmp=(double)
0.0;if(fract != 0.0)modf(fract * 10,& tmp);else{tmp=(double)(ch - '0');}if(tmp > 4)
for(0;1;_dyneither_ptr_inplace_plus(& end,sizeof(char),-1)){if(*((char*)
_check_dyneither_subscript(end,sizeof(char),0))== '.')
_dyneither_ptr_inplace_plus(& end,sizeof(char),-1);{char _tmp122;char _tmp121;
struct _dyneither_ptr _tmp120;if(((_tmp120=end,((_tmp121=*((char*)
_check_dyneither_subscript(_tmp120,sizeof(char),0)),((_tmp122=_tmp121 + '\001',((
_get_dyneither_size(_tmp120,sizeof(char))== 1  && (_tmp121 == '\000'  && _tmp122 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp120.curr)=_tmp122))))))))<= '9')break;}{char
_tmp125;char _tmp124;struct _dyneither_ptr _tmp123;(_tmp123=end,((_tmp124=*((char*)
_check_dyneither_subscript(_tmp123,sizeof(char),0)),((_tmp125='0',((
_get_dyneither_size(_tmp123,sizeof(char))== 1  && (_tmp124 == '\000'  && _tmp125 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp123.curr)=_tmp125)))))));}if((char*)end.curr
== (char*)start.curr){if((unsigned int)exp){{char _tmp128;char _tmp127;struct
_dyneither_ptr _tmp126;(_tmp126=end,((_tmp127=*((char*)_check_dyneither_subscript(
_tmp126,sizeof(char),0)),((_tmp128='1',((_get_dyneither_size(_tmp126,sizeof(char))
== 1  && (_tmp127 == '\000'  && _tmp128 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp126.curr)=_tmp128)))))));}++(*exp);}else{{char _tmp12B;char _tmp12A;struct
_dyneither_ptr _tmp129;(_tmp129=_dyneither_ptr_inplace_plus(& end,sizeof(char),-1),((
_tmp12A=*((char*)_check_dyneither_subscript(_tmp129,sizeof(char),0)),((_tmp12B='1',((
_get_dyneither_size(_tmp129,sizeof(char))== 1  && (_tmp12A == '\000'  && _tmp12B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp129.curr)=_tmp12B)))))));}
_dyneither_ptr_inplace_plus(& start,sizeof(char),-1);}break;}}else{if(*signp == '-')
for(0;1;_dyneither_ptr_inplace_plus(& end,sizeof(char),-1)){if(*((char*)
_check_dyneither_subscript(end,sizeof(char),0))== '.')
_dyneither_ptr_inplace_plus(& end,sizeof(char),-1);if(*((char*)
_check_dyneither_subscript(end,sizeof(char),0))!= '0')break;if((char*)end.curr == (
char*)start.curr)*signp=0;}}return start;}int Cyc___cvt_double(double number,int
prec,int flags,int*signp,int fmtch,struct _dyneither_ptr startp,struct _dyneither_ptr
endp);int Cyc___cvt_double(double number,int prec,int flags,int*signp,int fmtch,
struct _dyneither_ptr startp,struct _dyneither_ptr endp){struct _dyneither_ptr p;
struct _dyneither_ptr t;register double fract;int dotrim=0;int expcnt;int gformat=0;
double integer=(double)0.0;double tmp=(double)0.0;expcnt=0;if(number < 0){number=-
number;*signp=(int)'-';}else{*signp=0;}fract=modf(number,& integer);t=
_dyneither_ptr_inplace_plus(& startp,sizeof(char),1);for(p=_dyneither_ptr_plus(
endp,sizeof(char),- 1);(char*)p.curr >= (char*)startp.curr  && integer != 0.0;++
expcnt){tmp=modf(integer / 10,& integer);{char _tmp12E;char _tmp12D;struct
_dyneither_ptr _tmp12C;(_tmp12C=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),
-1),((_tmp12D=*((char*)_check_dyneither_subscript(_tmp12C,sizeof(char),0)),((
_tmp12E=(char)((int)((tmp + .01)* 10)+ '0'),((_get_dyneither_size(_tmp12C,sizeof(
char))== 1  && (_tmp12D == '\000'  && _tmp12E != '\000')?_throw_arraybounds(): 1,*((
char*)_tmp12C.curr)=_tmp12E)))))));};}switch(fmtch){case 'f': _LL63: goto _LL64;case
'F': _LL64: if(expcnt){char _tmp131;char _tmp130;struct _dyneither_ptr _tmp12F;for(0;(
char*)(_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr < (char*)endp.curr;(
_tmp12F=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp130=*((char*)
_check_dyneither_subscript(_tmp12F,sizeof(char),0)),((_tmp131=*((char*)
_check_dyneither_subscript(p,sizeof(char),0)),((_get_dyneither_size(_tmp12F,
sizeof(char))== 1  && (_tmp130 == '\000'  && _tmp131 != '\000')?_throw_arraybounds():
1,*((char*)_tmp12F.curr)=_tmp131)))))))){;}}else{char _tmp134;char _tmp133;struct
_dyneither_ptr _tmp132;(_tmp132=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),
1),((_tmp133=*((char*)_check_dyneither_subscript(_tmp132,sizeof(char),0)),((
_tmp134='0',((_get_dyneither_size(_tmp132,sizeof(char))== 1  && (_tmp133 == '\000'
 && _tmp134 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp132.curr)=_tmp134)))))));}
if(prec  || flags & 8){char _tmp137;char _tmp136;struct _dyneither_ptr _tmp135;(
_tmp135=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp136=*((char*)
_check_dyneither_subscript(_tmp135,sizeof(char),0)),((_tmp137='.',((
_get_dyneither_size(_tmp135,sizeof(char))== 1  && (_tmp136 == '\000'  && _tmp137 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp135.curr)=_tmp137)))))));}if(fract != 0.0){
if(prec)do{fract=modf(fract * 10,& tmp);{char _tmp13A;char _tmp139;struct
_dyneither_ptr _tmp138;(_tmp138=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),
1),((_tmp139=*((char*)_check_dyneither_subscript(_tmp138,sizeof(char),0)),((
_tmp13A=(char)((int)tmp + '0'),((_get_dyneither_size(_tmp138,sizeof(char))== 1
 && (_tmp139 == '\000'  && _tmp13A != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp138.curr)=_tmp13A)))))));};}while(-- prec  && fract != 0.0);if(fract != 0.0)
startp=Cyc_sround(fract,0,startp,_dyneither_ptr_plus(t,sizeof(char),- 1),(char)'\000',
signp);}{char _tmp13D;char _tmp13C;struct _dyneither_ptr _tmp13B;for(0;prec --;(
_tmp13B=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp13C=*((char*)
_check_dyneither_subscript(_tmp13B,sizeof(char),0)),((_tmp13D='0',((
_get_dyneither_size(_tmp13B,sizeof(char))== 1  && (_tmp13C == '\000'  && _tmp13D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp13B.curr)=_tmp13D)))))))){;}}break;case 'e':
_LL65: goto _LL66;case 'E': _LL66: eformat: if(expcnt){{char _tmp140;char _tmp13F;struct
_dyneither_ptr _tmp13E;(_tmp13E=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),
1),((_tmp13F=*((char*)_check_dyneither_subscript(_tmp13E,sizeof(char),0)),((
_tmp140=*((char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus(& p,
sizeof(char),1),sizeof(char),0)),((_get_dyneither_size(_tmp13E,sizeof(char))== 1
 && (_tmp13F == '\000'  && _tmp140 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp13E.curr)=_tmp140)))))));}if(prec  || flags & 8){char _tmp143;char _tmp142;
struct _dyneither_ptr _tmp141;(_tmp141=_dyneither_ptr_inplace_plus_post(& t,sizeof(
char),1),((_tmp142=*((char*)_check_dyneither_subscript(_tmp141,sizeof(char),0)),((
_tmp143='.',((_get_dyneither_size(_tmp141,sizeof(char))== 1  && (_tmp142 == '\000'
 && _tmp143 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp141.curr)=_tmp143)))))));}
for(0;prec  && (char*)(_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr < (
char*)endp.curr;-- prec){char _tmp146;char _tmp145;struct _dyneither_ptr _tmp144;(
_tmp144=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp145=*((char*)
_check_dyneither_subscript(_tmp144,sizeof(char),0)),((_tmp146=*((char*)
_check_dyneither_subscript(p,sizeof(char),0)),((_get_dyneither_size(_tmp144,
sizeof(char))== 1  && (_tmp145 == '\000'  && _tmp146 != '\000')?_throw_arraybounds():
1,*((char*)_tmp144.curr)=_tmp146)))))));}if(!prec  && (char*)(
_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr < (char*)endp.curr){fract=(
double)0;startp=Cyc_sround((double)0,(int*)& expcnt,startp,_dyneither_ptr_plus(t,
sizeof(char),- 1),*((char*)_check_dyneither_subscript(p,sizeof(char),0)),signp);}
-- expcnt;}else{if(fract != 0.0){for(expcnt=- 1;1;-- expcnt){fract=modf(fract * 10,&
tmp);if(tmp != 0.0)break;}{char _tmp149;char _tmp148;struct _dyneither_ptr _tmp147;(
_tmp147=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp148=*((char*)
_check_dyneither_subscript(_tmp147,sizeof(char),0)),((_tmp149=(char)((int)tmp + '0'),((
_get_dyneither_size(_tmp147,sizeof(char))== 1  && (_tmp148 == '\000'  && _tmp149 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp147.curr)=_tmp149)))))));}if(prec  || flags & 
8){char _tmp14C;char _tmp14B;struct _dyneither_ptr _tmp14A;(_tmp14A=
_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp14B=*((char*)
_check_dyneither_subscript(_tmp14A,sizeof(char),0)),((_tmp14C='.',((
_get_dyneither_size(_tmp14A,sizeof(char))== 1  && (_tmp14B == '\000'  && _tmp14C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp14A.curr)=_tmp14C)))))));}}else{{char _tmp14F;
char _tmp14E;struct _dyneither_ptr _tmp14D;(_tmp14D=
_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp14E=*((char*)
_check_dyneither_subscript(_tmp14D,sizeof(char),0)),((_tmp14F='0',((
_get_dyneither_size(_tmp14D,sizeof(char))== 1  && (_tmp14E == '\000'  && _tmp14F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp14D.curr)=_tmp14F)))))));}if(prec  || flags & 
8){char _tmp152;char _tmp151;struct _dyneither_ptr _tmp150;(_tmp150=
_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp151=*((char*)
_check_dyneither_subscript(_tmp150,sizeof(char),0)),((_tmp152='.',((
_get_dyneither_size(_tmp150,sizeof(char))== 1  && (_tmp151 == '\000'  && _tmp152 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp150.curr)=_tmp152)))))));}}}if(fract != 0.0){
if(prec)do{fract=modf(fract * 10,& tmp);{char _tmp155;char _tmp154;struct
_dyneither_ptr _tmp153;(_tmp153=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),
1),((_tmp154=*((char*)_check_dyneither_subscript(_tmp153,sizeof(char),0)),((
_tmp155=(char)((int)tmp + '0'),((_get_dyneither_size(_tmp153,sizeof(char))== 1
 && (_tmp154 == '\000'  && _tmp155 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp153.curr)=_tmp155)))))));};}while(-- prec  && fract != 0.0);if(fract != 0.0)
startp=Cyc_sround(fract,(int*)& expcnt,startp,_dyneither_ptr_plus(t,sizeof(char),
- 1),(char)'\000',signp);}{char _tmp158;char _tmp157;struct _dyneither_ptr _tmp156;
for(0;prec --;(_tmp156=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((
_tmp157=*((char*)_check_dyneither_subscript(_tmp156,sizeof(char),0)),((_tmp158='0',((
_get_dyneither_size(_tmp156,sizeof(char))== 1  && (_tmp157 == '\000'  && _tmp158 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp156.curr)=_tmp158)))))))){;}}if(gformat  && !(
flags & 8)){while((char*)t.curr > (char*)startp.curr  && *((char*)
_check_dyneither_subscript(_dyneither_ptr_inplace_plus(& t,sizeof(char),-1),
sizeof(char),0))== '0'){;}if(*((char*)_check_dyneither_subscript(t,sizeof(char),
0))== '.')_dyneither_ptr_inplace_plus(& t,sizeof(char),-1);
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}t=Cyc_exponent(t,expcnt,fmtch);
break;case 'g': _LL67: goto _LL68;case 'G': _LL68: if(!prec)++ prec;if(expcnt > prec  || (
!expcnt  && fract != 0.0) && fract < .0001){-- prec;fmtch -=2;gformat=1;goto eformat;}
if(expcnt){char _tmp15B;char _tmp15A;struct _dyneither_ptr _tmp159;for(0;(char*)(
_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr < (char*)endp.curr;(((
_tmp159=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp15A=*((char*)
_check_dyneither_subscript(_tmp159,sizeof(char),0)),((_tmp15B=*((char*)
_check_dyneither_subscript(p,sizeof(char),0)),((_get_dyneither_size(_tmp159,
sizeof(char))== 1  && (_tmp15A == '\000'  && _tmp15B != '\000')?_throw_arraybounds():
1,*((char*)_tmp159.curr)=_tmp15B)))))))),-- prec)){;}}else{char _tmp15E;char
_tmp15D;struct _dyneither_ptr _tmp15C;(_tmp15C=_dyneither_ptr_inplace_plus_post(& t,
sizeof(char),1),((_tmp15D=*((char*)_check_dyneither_subscript(_tmp15C,sizeof(
char),0)),((_tmp15E='0',((_get_dyneither_size(_tmp15C,sizeof(char))== 1  && (
_tmp15D == '\000'  && _tmp15E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp15C.curr)=
_tmp15E)))))));}if(prec  || flags & 8){dotrim=1;{char _tmp161;char _tmp160;struct
_dyneither_ptr _tmp15F;(_tmp15F=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),
1),((_tmp160=*((char*)_check_dyneither_subscript(_tmp15F,sizeof(char),0)),((
_tmp161='.',((_get_dyneither_size(_tmp15F,sizeof(char))== 1  && (_tmp160 == '\000'
 && _tmp161 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp15F.curr)=_tmp161)))))));};}
else{dotrim=0;}if(fract != 0.0){if(prec){do{fract=modf(fract * 10,& tmp);{char
_tmp164;char _tmp163;struct _dyneither_ptr _tmp162;(_tmp162=
_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp163=*((char*)
_check_dyneither_subscript(_tmp162,sizeof(char),0)),((_tmp164=(char)((int)tmp + '0'),((
_get_dyneither_size(_tmp162,sizeof(char))== 1  && (_tmp163 == '\000'  && _tmp164 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp162.curr)=_tmp164)))))));};}while(tmp == 0.0
 && !expcnt);while(-- prec  && fract != 0.0){fract=modf(fract * 10,& tmp);{char
_tmp167;char _tmp166;struct _dyneither_ptr _tmp165;(_tmp165=
_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp166=*((char*)
_check_dyneither_subscript(_tmp165,sizeof(char),0)),((_tmp167=(char)((int)tmp + '0'),((
_get_dyneither_size(_tmp165,sizeof(char))== 1  && (_tmp166 == '\000'  && _tmp167 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp165.curr)=_tmp167)))))));};}}if(fract != 0.0)
startp=Cyc_sround(fract,0,startp,_dyneither_ptr_plus(t,sizeof(char),- 1),(char)'\000',
signp);}if(flags & 8){char _tmp16A;char _tmp169;struct _dyneither_ptr _tmp168;for(0;
prec --;(_tmp168=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp169=*((
char*)_check_dyneither_subscript(_tmp168,sizeof(char),0)),((_tmp16A='0',((
_get_dyneither_size(_tmp168,sizeof(char))== 1  && (_tmp169 == '\000'  && _tmp16A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp168.curr)=_tmp16A)))))))){;}}else{if(dotrim){
while((char*)t.curr > (char*)startp.curr  && *((char*)_check_dyneither_subscript(
_dyneither_ptr_inplace_plus(& t,sizeof(char),-1),sizeof(char),0))== '0'){;}if(*((
char*)_check_dyneither_subscript(t,sizeof(char),0))!= '.')
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}}break;default: _LL69: {struct Cyc_Core_Impossible_struct
_tmp170;const char*_tmp16F;struct Cyc_Core_Impossible_struct*_tmp16E;(int)_throw((
void*)((_tmp16E=_cycalloc(sizeof(*_tmp16E)),((_tmp16E[0]=((_tmp170.tag=Cyc_Core_Impossible,((
_tmp170.f1=((_tmp16F="__cvt_double",_tag_dyneither(_tmp16F,sizeof(char),13))),
_tmp170)))),_tmp16E)))));}}return(t.curr - startp.curr)/ sizeof(char);}int Cyc_vfprintf(
struct Cyc___cycFILE*f,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_vfprintf(
struct Cyc___cycFILE*f,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){int ans;ans=((
int(*)(int(*ioputc)(int,struct Cyc___cycFILE*),struct Cyc___cycFILE*ioputc_env,
struct _dyneither_ptr fmt0,struct _dyneither_ptr ap))Cyc__IO_vfprintf)(Cyc_putc,f,
fmt,ap);return ans;}int Cyc_fprintf(struct Cyc___cycFILE*f,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);int Cyc_fprintf(struct Cyc___cycFILE*f,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap){return Cyc_vfprintf(f,fmt,ap);}int Cyc_vprintf(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_vprintf(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap){int ans;ans=((int(*)(int(*ioputc)(int,
struct Cyc___cycFILE*),struct Cyc___cycFILE*ioputc_env,struct _dyneither_ptr fmt0,
struct _dyneither_ptr ap))Cyc__IO_vfprintf)(Cyc_putc,Cyc_stdout,fmt,ap);return ans;}
int Cyc_printf(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_printf(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){int ans;ans=Cyc_vprintf(fmt,ap);
return ans;}struct _tuple0{struct _dyneither_ptr*f1;unsigned long*f2;};static int Cyc_putc_string(
int c,struct _tuple0*sptr_n);static int Cyc_putc_string(int c,struct _tuple0*sptr_n){
struct _tuple0 _tmpC2;struct _dyneither_ptr*_tmpC3;unsigned long*_tmpC4;struct
_tuple0*_tmpC1=sptr_n;_tmpC2=*_tmpC1;_tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;{struct
_dyneither_ptr s=*_tmpC3;unsigned long n=*_tmpC4;if(n == 0)return - 1;*((char*)
_check_dyneither_subscript(s,sizeof(char),0))=(char)c;
_dyneither_ptr_inplace_plus(_tmpC3,sizeof(char),1);*_tmpC4=n - 1;return 1;};}int
Cyc_vsnprintf(struct _dyneither_ptr s,unsigned long n,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);int Cyc_vsnprintf(struct _dyneither_ptr s,unsigned long n,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){int ans;struct _dyneither_ptr
_tmpC5=s;unsigned long _tmpC6=n;struct _tuple0 _tmp171;struct _tuple0 _tmpC7=(_tmp171.f1=&
_tmpC5,((_tmp171.f2=& _tmpC6,_tmp171)));ans=((int(*)(int(*ioputc)(int,struct
_tuple0*),struct _tuple0*ioputc_env,struct _dyneither_ptr fmt0,struct _dyneither_ptr
ap))Cyc__IO_vfprintf)(Cyc_putc_string,& _tmpC7,fmt,ap);if(0 <= ans)*((char*)
_check_dyneither_subscript(s,sizeof(char),ans))='\000';return ans;}int Cyc_snprintf(
struct _dyneither_ptr s,unsigned long n,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);int Cyc_snprintf(struct _dyneither_ptr s,unsigned long n,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap){return Cyc_vsnprintf(s,n,fmt,ap);}int
Cyc_vsprintf(struct _dyneither_ptr s,struct _dyneither_ptr fmt,struct _dyneither_ptr
ap);int Cyc_vsprintf(struct _dyneither_ptr s,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap){return Cyc_vsnprintf(s,_get_dyneither_size(s,sizeof(char)),fmt,
ap);}int Cyc_sprintf(struct _dyneither_ptr s,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);int Cyc_sprintf(struct _dyneither_ptr s,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){return Cyc_vsnprintf(s,_get_dyneither_size(s,sizeof(char)),
fmt,ap);}static int Cyc_putc_void(int c,int dummy);static int Cyc_putc_void(int c,int
dummy){return 1;}struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*r1,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);static void _tmp177(unsigned int*_tmp176,
unsigned int*_tmp175,char**_tmp173){for(*_tmp176=0;*_tmp176 < *_tmp175;(*_tmp176)
++){(*_tmp173)[*_tmp176]='\000';}}struct _dyneither_ptr Cyc_vrprintf(struct
_RegionHandle*r1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){int size=((int(*)(
int(*ioputc)(int,int),int ioputc_env,struct _dyneither_ptr fmt0,struct
_dyneither_ptr ap))Cyc__IO_vfprintf)(Cyc_putc_void,0,fmt,ap)+ 1;unsigned int
_tmp176;unsigned int _tmp175;struct _dyneither_ptr _tmp174;char*_tmp173;
unsigned int _tmp172;struct _dyneither_ptr s=(_tmp172=(unsigned int)size,((_tmp173=(
char*)_region_malloc(r1,_check_times(sizeof(char),_tmp172 + 1)),((_tmp174=
_tag_dyneither(_tmp173,sizeof(char),_tmp172 + 1),((((_tmp175=_tmp172,((_tmp177(&
_tmp176,& _tmp175,& _tmp173),_tmp173[_tmp175]=(char)0)))),_tmp174)))))));Cyc_vsprintf(
_dyneither_ptr_decrease_size(s,sizeof(char),1),fmt,ap);return s;}struct
_dyneither_ptr Cyc_rprintf(struct _RegionHandle*r1,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);struct _dyneither_ptr Cyc_rprintf(struct _RegionHandle*r1,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap){return Cyc_vrprintf(r1,fmt,ap);}struct
_dyneither_ptr Cyc_aprintf(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct
_dyneither_ptr Cyc_aprintf(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){return
Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);}
