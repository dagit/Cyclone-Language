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
 struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_struct{
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
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_list(struct _dyneither_ptr);extern char Cyc_List_List_mismatch[14];struct
Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*
tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];
struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{
char*tag;};struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(
void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple1{union
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
extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_VoidType_struct
Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{int tag;
struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft
 = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_struct{
int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};struct Cyc_Absyn_Fastcall_att_struct{
int tag;};struct Cyc_Absyn_Noreturn_att_struct{int tag;};struct Cyc_Absyn_Const_att_struct{
int tag;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{
int tag;};struct Cyc_Absyn_Section_att_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_Nocommon_att_struct{int tag;};struct Cyc_Absyn_Shared_att_struct{
int tag;};struct Cyc_Absyn_Unused_att_struct{int tag;};struct Cyc_Absyn_Weak_att_struct{
int tag;};struct Cyc_Absyn_Dllimport_att_struct{int tag;};struct Cyc_Absyn_Dllexport_att_struct{
int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{int tag;};struct Cyc_Absyn_Constructor_att_struct{
int tag;};struct Cyc_Absyn_Destructor_att_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{
int tag;};struct Cyc_Absyn_Format_att_struct{int tag;enum Cyc_Absyn_Format_Type f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_struct{
int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{int tag;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
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
char*tag;};int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_empty_effect;struct Cyc_RgnOrder_RgnPO;
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
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];
struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*
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
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;int Cyc_Tcutil_unify(void*,void*);void*
Cyc_Tcutil_normalize_effect(void*e);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);static struct
_dyneither_ptr Cyc_RgnOrder_tvar2string(struct Cyc_Absyn_Tvar*t);static struct
_dyneither_ptr Cyc_RgnOrder_tvar2string(struct Cyc_Absyn_Tvar*t){return*t->name;}
static int Cyc_RgnOrder_valid_constraint(void*eff,void*rgn);static void*Cyc_RgnOrder_filter_effect(
void*eff,struct Cyc_Absyn_Kind*k);struct Cyc_RgnOrder_RgnInfo{void*these_outlive;
int resetable;};struct Cyc_RgnOrder_RgnPO{struct Cyc_Dict_Dict d;void*
these_outlive_heap;void*these_outlive_unique;struct Cyc_Absyn_Tvar*
youngest_unresettable;};static int Cyc_RgnOrder_valid_constraint(void*eff,void*rgn);
static int Cyc_RgnOrder_valid_constraint(void*eff,void*rgn){struct Cyc_Absyn_Kind
_tmp1;enum Cyc_Absyn_KindQual _tmp2;enum Cyc_Absyn_AliasQual _tmp3;struct Cyc_Absyn_Kind*
_tmp0=Cyc_Tcutil_typ_kind(rgn);_tmp1=*_tmp0;_tmp2=_tmp1.kind;_tmp3=_tmp1.aliasqual;
if(_tmp2 != Cyc_Absyn_RgnKind){const char*_tmpE4;void*_tmpE3[1];struct Cyc_String_pa_struct
_tmpE2;(_tmpE2.tag=0,((_tmpE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(rgn)),((_tmpE3[0]=& _tmpE2,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE4="bad region type |%s| passed to add_outlives_constraint",
_tag_dyneither(_tmpE4,sizeof(char),55))),_tag_dyneither(_tmpE3,sizeof(void*),1)))))));}{
void*_tmp7=Cyc_Tcutil_compress(eff);struct Cyc_List_List*_tmp9;void*_tmpD;void*
_tmpF;_LL1: {struct Cyc_Absyn_JoinEff_struct*_tmp8=(struct Cyc_Absyn_JoinEff_struct*)
_tmp7;if(_tmp8->tag != 24)goto _LL3;else{_tmp9=_tmp8->f1;}}_LL2: for(0;_tmp9 != 0;
_tmp9=_tmp9->tl){if(!Cyc_RgnOrder_valid_constraint((void*)_tmp9->hd,rgn))return 0;}
return 1;_LL3: {struct Cyc_Absyn_Evar_struct*_tmpA=(struct Cyc_Absyn_Evar_struct*)
_tmp7;if(_tmpA->tag != 1)goto _LL5;}_LL4: goto _LL6;_LL5: {struct Cyc_Absyn_VarType_struct*
_tmpB=(struct Cyc_Absyn_VarType_struct*)_tmp7;if(_tmpB->tag != 2)goto _LL7;}_LL6:
return 1;_LL7: {struct Cyc_Absyn_RgnsEff_struct*_tmpC=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp7;if(_tmpC->tag != 25)goto _LL9;else{_tmpD=(void*)_tmpC->f1;}}_LL8: if(_tmp3 == 
Cyc_Absyn_Aliasable)return 1;else{return 0;}_LL9: {struct Cyc_Absyn_AccessEff_struct*
_tmpE=(struct Cyc_Absyn_AccessEff_struct*)_tmp7;if(_tmpE->tag != 23)goto _LLB;else{
_tmpF=(void*)_tmpE->f1;}}_LLA: {struct Cyc_Absyn_Kind _tmp11;enum Cyc_Absyn_KindQual
_tmp12;enum Cyc_Absyn_AliasQual _tmp13;struct Cyc_Absyn_Kind*_tmp10=Cyc_Tcutil_typ_kind(
_tmpF);_tmp11=*_tmp10;_tmp12=_tmp11.kind;_tmp13=_tmp11.aliasqual;return _tmp2 == 
Cyc_Absyn_RgnKind  && (_tmp3 == Cyc_Absyn_Aliasable  || _tmp13 == _tmp3);}_LLB:;_LLC: {
const char*_tmpE8;void*_tmpE7[1];struct Cyc_String_pa_struct _tmpE6;(_tmpE6.tag=0,((
_tmpE6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
eff)),((_tmpE7[0]=& _tmpE6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpE8="bad effect |%s| passed to add_outlives_constraint",
_tag_dyneither(_tmpE8,sizeof(char),50))),_tag_dyneither(_tmpE7,sizeof(void*),1)))))));}
_LL0:;};}static void*Cyc_RgnOrder_filter_effect(void*eff,struct Cyc_Absyn_Kind*k);
static void*Cyc_RgnOrder_filter_effect(void*eff,struct Cyc_Absyn_Kind*k){void*
_tmp17=Cyc_Tcutil_compress(eff);void*_tmp18=_tmp17;struct Cyc_List_List*_tmp1A;
void*_tmp1E;_LLE: {struct Cyc_Absyn_JoinEff_struct*_tmp19=(struct Cyc_Absyn_JoinEff_struct*)
_tmp18;if(_tmp19->tag != 24)goto _LL10;else{_tmp1A=_tmp19->f1;}}_LLF: {struct Cyc_List_List*
effs=0;for(0;_tmp1A != 0;_tmp1A=_tmp1A->tl){void*_tmp1F=Cyc_RgnOrder_filter_effect((
void*)_tmp1A->hd,k);if(_tmp1F != (void*)& Cyc_Absyn_VoidType_val){struct Cyc_List_List*
_tmpE9;effs=((_tmpE9=_cycalloc(sizeof(*_tmpE9)),((_tmpE9->hd=_tmp1F,((_tmpE9->tl=
effs,_tmpE9))))));}}{struct Cyc_Absyn_JoinEff_struct _tmpEC;struct Cyc_Absyn_JoinEff_struct*
_tmpEB;return(void*)((_tmpEB=_cycalloc(sizeof(*_tmpEB)),((_tmpEB[0]=((_tmpEC.tag=
24,((_tmpEC.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effs),_tmpEC)))),_tmpEB))));};}_LL10: {struct Cyc_Absyn_Evar_struct*_tmp1B=(
struct Cyc_Absyn_Evar_struct*)_tmp18;if(_tmp1B->tag != 1)goto _LL12;}_LL11: goto
_LL13;_LL12: {struct Cyc_Absyn_RgnsEff_struct*_tmp1C=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp18;if(_tmp1C->tag != 25)goto _LL14;}_LL13: return _tmp17;_LL14: {struct Cyc_Absyn_AccessEff_struct*
_tmp1D=(struct Cyc_Absyn_AccessEff_struct*)_tmp18;if(_tmp1D->tag != 23)goto _LL16;
else{_tmp1E=(void*)_tmp1D->f1;}}_LL15: if(Cyc_Tcutil_typ_kind(_tmp1E)!= k)return
_tmp17;else{return(void*)& Cyc_Absyn_VoidType_val;}_LL16:;_LL17: {const char*
_tmpEF;void*_tmpEE;(_tmpEE=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEF="bad effect passed to filter_effect",
_tag_dyneither(_tmpEF,sizeof(char),35))),_tag_dyneither(_tmpEE,sizeof(void*),0)));}
_LLD:;}struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*r,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*r,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
loc){eff=Cyc_Tcutil_normalize_effect(eff);{struct Cyc_RgnOrder_RgnPO*_tmpF0;
struct Cyc_RgnOrder_RgnPO*_tmp25=(_tmpF0=_region_malloc(r,sizeof(*_tmpF0)),((
_tmpF0[0]=*po,_tmpF0)));if(!Cyc_RgnOrder_valid_constraint(eff,rgn)){{const char*
_tmpF3;void*_tmpF2;(_tmpF2=0,Cyc_Tcutil_terr(loc,((_tmpF3="Invalid region ordering constraint; kind mismatch",
_tag_dyneither(_tmpF3,sizeof(char),50))),_tag_dyneither(_tmpF2,sizeof(void*),0)));}
return _tmp25;}{void*_tmp28=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp2A;
_LL19: {struct Cyc_Absyn_VarType_struct*_tmp29=(struct Cyc_Absyn_VarType_struct*)
_tmp28;if(_tmp29->tag != 2)goto _LL1B;else{_tmp2A=_tmp29->f1;}}_LL1A: {struct Cyc_Dict_Dict
_tmp2E=po->d;if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(
_tmp2E,_tmp2A)){void*_tmp2F=(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict
d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(_tmp2E,_tmp2A))->these_outlive;struct
Cyc_Absyn_JoinEff_struct*_tmpFD;void*_tmpFC[2];struct Cyc_Absyn_JoinEff_struct
_tmpFB;struct Cyc_RgnOrder_RgnInfo*_tmpFA;_tmp2E=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(
_tmp2E,_tmp2A,((_tmpFA=_region_malloc(r,sizeof(*_tmpFA)),((_tmpFA->these_outlive=(
void*)((_tmpFD=_cycalloc(sizeof(*_tmpFD)),((_tmpFD[0]=((_tmpFB.tag=24,((_tmpFB.f1=((
_tmpFC[1]=_tmp2F,((_tmpFC[0]=eff,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpFC,sizeof(void*),2)))))),_tmpFB)))),_tmpFD)))),((
_tmpFA->resetable=0,_tmpFA)))))));}else{struct Cyc_RgnOrder_RgnInfo*_tmpFE;_tmp2E=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*
v))Cyc_Dict_insert)(_tmp2E,_tmp2A,((_tmpFE=_region_malloc(r,sizeof(*_tmpFE)),((
_tmpFE->these_outlive=eff,((_tmpFE->resetable=0,_tmpFE)))))));}_tmp25->d=_tmp2E;
return _tmp25;}_LL1B: {struct Cyc_Absyn_Evar_struct*_tmp2B=(struct Cyc_Absyn_Evar_struct*)
_tmp28;if(_tmp2B->tag != 1)goto _LL1D;}_LL1C: Cyc_Tcutil_unify(rgn,(void*)& Cyc_Absyn_HeapRgn_val);
goto _LL1E;_LL1D: {struct Cyc_Absyn_HeapRgn_struct*_tmp2C=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp28;if(_tmp2C->tag != 21)goto _LL1F;}_LL1E:{struct Cyc_Absyn_JoinEff_struct
_tmp104;void*_tmp103[2];struct Cyc_Absyn_JoinEff_struct*_tmp102;_tmp25->these_outlive_heap=(
void*)((_tmp102=_cycalloc(sizeof(*_tmp102)),((_tmp102[0]=((_tmp104.tag=24,((
_tmp104.f1=((_tmp103[1]=po->these_outlive_heap,((_tmp103[0]=eff,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp103,sizeof(void*),2)))))),
_tmp104)))),_tmp102))));}return _tmp25;_LL1F: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp2D=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp28;if(_tmp2D->tag != 22)goto _LL21;}
_LL20:{struct Cyc_Absyn_JoinEff_struct _tmp10A;void*_tmp109[2];struct Cyc_Absyn_JoinEff_struct*
_tmp108;_tmp25->these_outlive_unique=(void*)((_tmp108=_cycalloc(sizeof(*_tmp108)),((
_tmp108[0]=((_tmp10A.tag=24,((_tmp10A.f1=((_tmp109[1]=po->these_outlive_unique,((
_tmp109[0]=eff,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp109,sizeof(void*),2)))))),_tmp10A)))),_tmp108))));}return
_tmp25;_LL21:;_LL22: {const char*_tmp10D;void*_tmp10C;(_tmp10C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D="RgnOrder::add_outlives_constraint passed a bad region",
_tag_dyneither(_tmp10D,sizeof(char),54))),_tag_dyneither(_tmp10C,sizeof(void*),0)));}
_LL18:;};};}struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct
_RegionHandle*r,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int
resettable,int opened);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct
_RegionHandle*r,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int
resettable,int opened){if(!opened  && ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*
k))Cyc_Dict_member)(po->d,rgn)){const char*_tmp110;void*_tmp10F;(_tmp10F=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp110="RgnOrder::add_youngest: repeated region",
_tag_dyneither(_tmp110,sizeof(char),40))),_tag_dyneither(_tmp10F,sizeof(void*),0)));}{
struct Cyc_RgnOrder_RgnPO*_tmp111;struct Cyc_RgnOrder_RgnPO*_tmp40=(_tmp111=
_region_malloc(r,sizeof(*_tmp111)),((_tmp111[0]=*po,_tmp111)));struct Cyc_Absyn_AccessEff_struct
_tmp11B;struct Cyc_Absyn_VarType_struct _tmp11A;struct Cyc_Absyn_VarType_struct*
_tmp119;struct Cyc_Absyn_AccessEff_struct*_tmp118;struct Cyc_Absyn_AccessEff_struct*
_tmp41=(_tmp118=_cycalloc(sizeof(*_tmp118)),((_tmp118[0]=((_tmp11B.tag=23,((
_tmp11B.f1=(void*)((void*)((_tmp119=_cycalloc(sizeof(*_tmp119)),((_tmp119[0]=((
_tmp11A.tag=2,((_tmp11A.f1=po->youngest_unresettable,_tmp11A)))),_tmp119))))),
_tmp11B)))),_tmp118)));{struct Cyc_RgnOrder_RgnInfo*_tmp11C;_tmp40->d=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(
po->d,rgn,((_tmp11C=_region_malloc(r,sizeof(*_tmp11C)),((_tmp11C->these_outlive=(
void*)_tmp41,((_tmp11C->resetable=resettable,_tmp11C)))))));}if(!resettable);
_tmp40->youngest_unresettable=rgn;return _tmp40;};}struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,
struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*loc);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*
loc){struct Cyc_Dict_Dict d=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*)))Cyc_Dict_rempty)(r,Cyc_Absyn_tvar_cmp);{
struct Cyc_List_List*_tmp48=tvs;for(0;_tmp48 != 0;_tmp48=_tmp48->tl){if((Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp48->hd,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_RgnKind){
struct Cyc_RgnOrder_RgnInfo*_tmp11D;d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(d,(
struct Cyc_Absyn_Tvar*)_tmp48->hd,((_tmp11D=_region_malloc(r,sizeof(*_tmp11D)),((
_tmp11D->these_outlive=Cyc_Absyn_empty_effect,((_tmp11D->resetable=0,_tmp11D)))))));}}}{
struct Cyc_Absyn_VarType_struct _tmp120;struct Cyc_Absyn_VarType_struct*_tmp11F;if(
!Cyc_RgnOrder_valid_constraint(effect,(void*)((_tmp11F=_cycalloc(sizeof(*_tmp11F)),((
_tmp11F[0]=((_tmp120.tag=2,((_tmp120.f1=fst_rgn,_tmp120)))),_tmp11F)))))){{const
char*_tmp123;void*_tmp122;(_tmp122=0,Cyc_Tcutil_terr(loc,((_tmp123="Invalid region ordering constraint; kind mismatch",
_tag_dyneither(_tmp123,sizeof(char),50))),_tag_dyneither(_tmp122,sizeof(void*),0)));}{
struct Cyc_RgnOrder_RgnPO*_tmp124;return(_tmp124=_region_malloc(r,sizeof(*_tmp124)),((
_tmp124->d=d,((_tmp124->these_outlive_heap=Cyc_Absyn_empty_effect,((_tmp124->these_outlive_unique=
Cyc_Absyn_empty_effect,((_tmp124->youngest_unresettable=fst_rgn,_tmp124)))))))));};}}{
struct Cyc_RgnOrder_RgnInfo*_tmp125;d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(d,
fst_rgn,((_tmp125=_region_malloc(r,sizeof(*_tmp125)),((_tmp125->these_outlive=
effect,((_tmp125->resetable=0,_tmp125)))))));}{struct Cyc_RgnOrder_RgnPO*_tmp126;
struct Cyc_RgnOrder_RgnPO*_tmp50=(_tmp126=_region_malloc(r,sizeof(*_tmp126)),((
_tmp126->d=d,((_tmp126->these_outlive_heap=Cyc_Absyn_empty_effect,((_tmp126->these_outlive_unique=
Cyc_Absyn_empty_effect,((_tmp126->youngest_unresettable=fst_rgn,_tmp126)))))))));
for(0;po != 0;po=po->tl){_tmp50=Cyc_RgnOrder_add_outlives_constraint(r,_tmp50,(*((
struct _tuple0*)po->hd)).f1,(*((struct _tuple0*)po->hd)).f2,loc);}return _tmp50;};}
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r){return(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*
k))Cyc_Dict_lookup)(po->d,r))->resetable;}static int Cyc_RgnOrder_contains_rgnseff(
struct Cyc_Absyn_Tvar*rgns_of_var,void*eff);static int Cyc_RgnOrder_contains_rgnseff(
struct Cyc_Absyn_Tvar*rgns_of_var,void*eff){void*_tmp52=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(eff));void*_tmp54;struct Cyc_List_List*_tmp56;_LL24: {struct
Cyc_Absyn_RgnsEff_struct*_tmp53=(struct Cyc_Absyn_RgnsEff_struct*)_tmp52;if(
_tmp53->tag != 25)goto _LL26;else{_tmp54=(void*)_tmp53->f1;}}_LL25: {void*_tmp57=
Cyc_Tcutil_compress(_tmp54);struct Cyc_Absyn_Tvar*_tmp59;_LL2B: {struct Cyc_Absyn_VarType_struct*
_tmp58=(struct Cyc_Absyn_VarType_struct*)_tmp57;if(_tmp58->tag != 2)goto _LL2D;
else{_tmp59=_tmp58->f1;}}_LL2C: return Cyc_Absyn_tvar_cmp(_tmp59,rgns_of_var)== 0;
_LL2D:;_LL2E: return 0;_LL2A:;}_LL26: {struct Cyc_Absyn_JoinEff_struct*_tmp55=(
struct Cyc_Absyn_JoinEff_struct*)_tmp52;if(_tmp55->tag != 24)goto _LL28;else{_tmp56=
_tmp55->f1;}}_LL27: for(0;_tmp56 != 0;_tmp56=_tmp56->tl){if(Cyc_RgnOrder_contains_rgnseff(
rgns_of_var,(void*)_tmp56->hd))return 1;}return 0;_LL28:;_LL29: return 0;_LL23:;}
struct Cyc_RgnOrder_OutlivesEnv{struct _RegionHandle*r;struct Cyc_List_List*seen;
struct Cyc_List_List*todo;};static void Cyc_RgnOrder_add_to_search(struct Cyc_RgnOrder_OutlivesEnv*
env,void*eff);static void Cyc_RgnOrder_add_to_search(struct Cyc_RgnOrder_OutlivesEnv*
env,void*eff){void*_tmp5A=Cyc_Tcutil_compress(eff);void*_tmp5C;struct Cyc_Absyn_Tvar*
_tmp5E;struct Cyc_List_List*_tmp60;_LL30: {struct Cyc_Absyn_AccessEff_struct*
_tmp5B=(struct Cyc_Absyn_AccessEff_struct*)_tmp5A;if(_tmp5B->tag != 23)goto _LL32;
else{_tmp5C=(void*)_tmp5B->f1;{struct Cyc_Absyn_VarType_struct*_tmp5D=(struct Cyc_Absyn_VarType_struct*)
_tmp5C;if(_tmp5D->tag != 2)goto _LL32;else{_tmp5E=_tmp5D->f1;}};}}_LL31:{struct Cyc_List_List*
_tmp61=env->seen;for(0;_tmp61 != 0;_tmp61=_tmp61->tl){if(Cyc_Absyn_tvar_cmp(
_tmp5E,(struct Cyc_Absyn_Tvar*)_tmp61->hd)== 0)return;}}{struct Cyc_List_List*
_tmp127;env->seen=((_tmp127=_region_malloc(env->r,sizeof(*_tmp127)),((_tmp127->hd=
_tmp5E,((_tmp127->tl=env->seen,_tmp127))))));}{struct Cyc_List_List*_tmp128;env->todo=((
_tmp128=_region_malloc(env->r,sizeof(*_tmp128)),((_tmp128->hd=_tmp5E,((_tmp128->tl=
env->todo,_tmp128))))));}return;_LL32: {struct Cyc_Absyn_JoinEff_struct*_tmp5F=(
struct Cyc_Absyn_JoinEff_struct*)_tmp5A;if(_tmp5F->tag != 24)goto _LL34;else{_tmp60=
_tmp5F->f1;}}_LL33: for(0;_tmp60 != 0;_tmp60=_tmp60->tl){Cyc_RgnOrder_add_to_search(
env,(void*)_tmp60->hd);}return;_LL34:;_LL35: return;_LL2F:;}static struct Cyc_RgnOrder_OutlivesEnv
Cyc_RgnOrder_initial_env(struct _RegionHandle*listrgn,struct Cyc_RgnOrder_RgnPO*po,
void*rgn);static struct Cyc_RgnOrder_OutlivesEnv Cyc_RgnOrder_initial_env(struct
_RegionHandle*listrgn,struct Cyc_RgnOrder_RgnPO*po,void*rgn){struct Cyc_RgnOrder_OutlivesEnv
_tmp129;struct Cyc_RgnOrder_OutlivesEnv _tmp64=(_tmp129.r=listrgn,((_tmp129.seen=0,((
_tmp129.todo=0,_tmp129)))));void*r=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Kind
_tmp66;enum Cyc_Absyn_KindQual _tmp67;enum Cyc_Absyn_AliasQual _tmp68;struct Cyc_Absyn_Kind*
_tmp65=Cyc_Tcutil_typ_kind(r);_tmp66=*_tmp65;_tmp67=_tmp66.kind;_tmp68=_tmp66.aliasqual;
if(_tmp67 == Cyc_Absyn_RgnKind)switch(_tmp68){case Cyc_Absyn_Aliasable: _LL36: Cyc_RgnOrder_add_to_search(&
_tmp64,po->these_outlive_heap);break;case Cyc_Absyn_Unique: _LL37: Cyc_RgnOrder_add_to_search(&
_tmp64,po->these_outlive_unique);break;case Cyc_Absyn_Top: _LL38: Cyc_RgnOrder_add_to_search(&
_tmp64,po->these_outlive_heap);Cyc_RgnOrder_add_to_search(& _tmp64,po->these_outlive_unique);
break;}else{const char*_tmp12C;void*_tmp12B;(_tmp12B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C="RgnOrder: rgn not of correct kind",
_tag_dyneither(_tmp12C,sizeof(char),34))),_tag_dyneither(_tmp12B,sizeof(void*),0)));}{
void*_tmp6B=r;struct Cyc_Absyn_Tvar*_tmp6D;_LL3B: {struct Cyc_Absyn_VarType_struct*
_tmp6C=(struct Cyc_Absyn_VarType_struct*)_tmp6B;if(_tmp6C->tag != 2)goto _LL3D;
else{_tmp6D=_tmp6C->f1;}}_LL3C:{struct Cyc_List_List*_tmp12D;_tmp64.seen=((
_tmp12D=_region_malloc(listrgn,sizeof(*_tmp12D)),((_tmp12D->hd=_tmp6D,((_tmp12D->tl=
_tmp64.seen,_tmp12D))))));}{struct Cyc_List_List*_tmp12E;_tmp64.todo=((_tmp12E=
_region_malloc(listrgn,sizeof(*_tmp12E)),((_tmp12E->hd=_tmp6D,((_tmp12E->tl=
_tmp64.todo,_tmp12E))))));}goto _LL3E;_LL3D:;_LL3E: return _tmp64;_LL3A:;};}static
int Cyc_RgnOrder_atomic_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*
rgn);static int Cyc_RgnOrder_atomic_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,
void*eff,void*rgn){struct _RegionHandle _tmp71=_new_region("listrgn");struct
_RegionHandle*listrgn=& _tmp71;_push_region(listrgn);{void*_tmp72=Cyc_Tcutil_compress(
eff);void*_tmp74;void*_tmp76;_LL40: {struct Cyc_Absyn_AccessEff_struct*_tmp73=(
struct Cyc_Absyn_AccessEff_struct*)_tmp72;if(_tmp73->tag != 23)goto _LL42;else{
_tmp74=(void*)_tmp73->f1;}}_LL41: {void*_tmp77=Cyc_Tcutil_compress(_tmp74);
struct Cyc_Absyn_Tvar*_tmp7B;_LL47: {struct Cyc_Absyn_UniqueRgn_struct*_tmp78=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp77;if(_tmp78->tag != 22)goto _LL49;}_LL48: {
int _tmp7C=1;_npop_handler(0);return _tmp7C;}_LL49: {struct Cyc_Absyn_HeapRgn_struct*
_tmp79=(struct Cyc_Absyn_HeapRgn_struct*)_tmp77;if(_tmp79->tag != 21)goto _LL4B;}
_LL4A: {void*_tmp7D=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp80;_LL50: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp7E=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp7D;if(_tmp7E->tag != 22)goto _LL52;}_LL51: {int _tmp81=0;_npop_handler(0);
return _tmp81;}_LL52: {struct Cyc_Absyn_VarType_struct*_tmp7F=(struct Cyc_Absyn_VarType_struct*)
_tmp7D;if(_tmp7F->tag != 2)goto _LL54;else{_tmp80=_tmp7F->f1;}}_LL53: {struct Cyc_Absyn_Kind
_tmp83;enum Cyc_Absyn_KindQual _tmp84;enum Cyc_Absyn_AliasQual _tmp85;struct Cyc_Absyn_Kind*
_tmp82=Cyc_Tcutil_tvar_kind(_tmp80,& Cyc_Tcutil_bk);_tmp83=*_tmp82;_tmp84=_tmp83.kind;
_tmp85=_tmp83.aliasqual;if(_tmp84 == Cyc_Absyn_RgnKind  && _tmp85 == Cyc_Absyn_Aliasable){
int _tmp86=1;_npop_handler(0);return _tmp86;}else{int _tmp87=0;_npop_handler(0);
return _tmp87;}}_LL54:;_LL55: {int _tmp88=1;_npop_handler(0);return _tmp88;}_LL4F:;}
_LL4B: {struct Cyc_Absyn_VarType_struct*_tmp7A=(struct Cyc_Absyn_VarType_struct*)
_tmp77;if(_tmp7A->tag != 2)goto _LL4D;else{_tmp7B=_tmp7A->f1;}}_LL4C: {struct Cyc_RgnOrder_OutlivesEnv
_tmp89=Cyc_RgnOrder_initial_env(listrgn,po,rgn);while(_tmp89.todo != 0){struct Cyc_Absyn_Tvar*
_tmp8A=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp89.todo))->hd;
_tmp89.todo=((struct Cyc_List_List*)_check_null(_tmp89.todo))->tl;if(Cyc_Absyn_tvar_cmp(
_tmp8A,_tmp7B)== 0){int _tmp8B=1;_npop_handler(0);return _tmp8B;}if(((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,_tmp8A))Cyc_RgnOrder_add_to_search(&
_tmp89,(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*
k))Cyc_Dict_lookup)(po->d,_tmp8A))->these_outlive);else{struct Cyc_String_pa_struct
_tmp136;void*_tmp135[1];const char*_tmp134;void*_tmp133;(_tmp133=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmp136.tag=0,((_tmp136.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp7B->name),((_tmp135[0]=& _tmp136,Cyc_aprintf(((_tmp134="RgnOrder: type variable %s not in dict",
_tag_dyneither(_tmp134,sizeof(char),39))),_tag_dyneither(_tmp135,sizeof(void*),1)))))))),
_tag_dyneither(_tmp133,sizeof(void*),0)));}}{int _tmp90=0;_npop_handler(0);return
_tmp90;};}_LL4D:;_LL4E: {int _tmp91=0;_npop_handler(0);return _tmp91;}_LL46:;}
_LL42: {struct Cyc_Absyn_RgnsEff_struct*_tmp75=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp72;if(_tmp75->tag != 25)goto _LL44;else{_tmp76=(void*)_tmp75->f1;}}_LL43: {
void*_tmp92=Cyc_Tcutil_compress(_tmp76);struct Cyc_Absyn_Tvar*_tmp94;_LL57: {
struct Cyc_Absyn_VarType_struct*_tmp93=(struct Cyc_Absyn_VarType_struct*)_tmp92;
if(_tmp93->tag != 2)goto _LL59;else{_tmp94=_tmp93->f1;}}_LL58: {struct Cyc_RgnOrder_OutlivesEnv
_tmp95=Cyc_RgnOrder_initial_env(listrgn,po,rgn);struct Cyc_Absyn_Kind _tmp97;enum 
Cyc_Absyn_KindQual _tmp98;enum Cyc_Absyn_AliasQual _tmp99;struct Cyc_Absyn_Kind*
_tmp96=Cyc_Tcutil_typ_kind(rgn);_tmp97=*_tmp96;_tmp98=_tmp97.kind;_tmp99=_tmp97.aliasqual;
if(_tmp98 == Cyc_Absyn_RgnKind  && (_tmp99 == Cyc_Absyn_Aliasable  || _tmp99 == Cyc_Absyn_Top)){
if(Cyc_RgnOrder_contains_rgnseff(_tmp94,po->these_outlive_heap)){int _tmp9A=1;
_npop_handler(0);return _tmp9A;}}if(_tmp98 == Cyc_Absyn_RgnKind  && (_tmp99 == Cyc_Absyn_Unique
 || _tmp99 == Cyc_Absyn_Top)){if(Cyc_RgnOrder_contains_rgnseff(_tmp94,po->these_outlive_unique)){
int _tmp9B=1;_npop_handler(0);return _tmp9B;}}while(_tmp95.todo != 0){struct Cyc_Absyn_Tvar*
_tmp9C=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp95.todo))->hd;
_tmp95.todo=((struct Cyc_List_List*)_check_null(_tmp95.todo))->tl;if(((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,_tmp9C)){
void*_tmp9D=(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*
k))Cyc_Dict_lookup)(po->d,_tmp9C))->these_outlive;if(Cyc_RgnOrder_contains_rgnseff(
_tmp94,_tmp9D)){int _tmp9E=1;_npop_handler(0);return _tmp9E;}Cyc_RgnOrder_add_to_search(&
_tmp95,_tmp9D);}else{const char*_tmp139;void*_tmp138;(_tmp138=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp139="RgnOrder: type variable not in dict",
_tag_dyneither(_tmp139,sizeof(char),36))),_tag_dyneither(_tmp138,sizeof(void*),0)));}}{
int _tmpA1=0;_npop_handler(0);return _tmpA1;};}_LL59:;_LL5A: {int _tmpA2=0;
_npop_handler(0);return _tmpA2;}_LL56:;}_LL44:;_LL45: {int _tmpA3=0;_npop_handler(
0);return _tmpA3;}_LL3F:;};_pop_region(listrgn);}int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){eff=Cyc_Tcutil_normalize_effect(
eff);{void*_tmpA4=Cyc_Tcutil_compress(eff);struct Cyc_List_List*_tmpA6;_LL5C: {
struct Cyc_Absyn_JoinEff_struct*_tmpA5=(struct Cyc_Absyn_JoinEff_struct*)_tmpA4;
if(_tmpA5->tag != 24)goto _LL5E;else{_tmpA6=_tmpA5->f1;}}_LL5D: for(0;_tmpA6 != 0;
_tmpA6=_tmpA6->tl){if(!Cyc_RgnOrder_effect_outlives(po,(void*)_tmpA6->hd,rgn))
return 0;}return 1;_LL5E:;_LL5F: return Cyc_RgnOrder_atomic_effect_outlives(po,eff,
rgn);_LL5B:;};}static void Cyc_RgnOrder_pin_effect(void*eff,void*bd);static void Cyc_RgnOrder_pin_effect(
void*eff,void*bd){eff=Cyc_Tcutil_normalize_effect(eff);{void*_tmpA7=Cyc_Tcutil_compress(
eff);struct Cyc_List_List*_tmpA9;void*_tmpAB;void*_tmpAD;_LL61: {struct Cyc_Absyn_JoinEff_struct*
_tmpA8=(struct Cyc_Absyn_JoinEff_struct*)_tmpA7;if(_tmpA8->tag != 24)goto _LL63;
else{_tmpA9=_tmpA8->f1;}}_LL62: for(0;_tmpA9 != 0;_tmpA9=_tmpA9->tl){Cyc_RgnOrder_pin_effect((
void*)_tmpA9->hd,bd);}return;_LL63: {struct Cyc_Absyn_RgnsEff_struct*_tmpAA=(
struct Cyc_Absyn_RgnsEff_struct*)_tmpA7;if(_tmpAA->tag != 25)goto _LL65;else{_tmpAB=(
void*)_tmpAA->f1;}}_LL64: {void*_tmpAF=Cyc_Tcutil_compress(_tmpAB);_LL6C: {
struct Cyc_Absyn_Evar_struct*_tmpB0=(struct Cyc_Absyn_Evar_struct*)_tmpAF;if(
_tmpB0->tag != 1)goto _LL6E;}_LL6D: Cyc_Tcutil_unify(_tmpAB,Cyc_Absyn_uint_typ);
return;_LL6E:;_LL6F: return;_LL6B:;}_LL65: {struct Cyc_Absyn_AccessEff_struct*
_tmpAC=(struct Cyc_Absyn_AccessEff_struct*)_tmpA7;if(_tmpAC->tag != 23)goto _LL67;
else{_tmpAD=(void*)_tmpAC->f1;}}_LL66: {void*_tmpB1=Cyc_Tcutil_compress(_tmpAD);
_LL71: {struct Cyc_Absyn_Evar_struct*_tmpB2=(struct Cyc_Absyn_Evar_struct*)_tmpB1;
if(_tmpB2->tag != 1)goto _LL73;}_LL72: Cyc_Tcutil_unify(_tmpAD,bd);return;_LL73:;
_LL74: return;_LL70:;}_LL67: {struct Cyc_Absyn_Evar_struct*_tmpAE=(struct Cyc_Absyn_Evar_struct*)
_tmpA7;if(_tmpAE->tag != 1)goto _LL69;}_LL68: Cyc_Tcutil_unify(eff,Cyc_Absyn_empty_effect);
return;_LL69:;_LL6A: return;_LL60:;};}int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_List_List*constraints,void*default_bound,int do_pin){{struct Cyc_List_List*
_tmpB3=constraints;for(0;_tmpB3 != 0;_tmpB3=_tmpB3->tl){struct _tuple0 _tmpB5;void*
_tmpB6;struct _tuple0*_tmpB4=(struct _tuple0*)_tmpB3->hd;_tmpB5=*_tmpB4;_tmpB6=
_tmpB5.f2;{void*_tmpB7=Cyc_Tcutil_compress(_tmpB6);_LL76: {struct Cyc_Absyn_Evar_struct*
_tmpB8=(struct Cyc_Absyn_Evar_struct*)_tmpB7;if(_tmpB8->tag != 1)goto _LL78;}_LL77:
if(do_pin)Cyc_Tcutil_unify(_tmpB6,default_bound);goto _LL75;_LL78:;_LL79: goto
_LL75;_LL75:;};}}{struct Cyc_List_List*_tmpB9=constraints;for(0;_tmpB9 != 0;_tmpB9=
_tmpB9->tl){struct _tuple0 _tmpBB;void*_tmpBC;void*_tmpBD;struct _tuple0*_tmpBA=(
struct _tuple0*)_tmpB9->hd;_tmpBB=*_tmpBA;_tmpBC=_tmpBB.f1;_tmpBD=_tmpBB.f2;if(
do_pin)Cyc_RgnOrder_pin_effect(_tmpBC,_tmpBD);if(!Cyc_RgnOrder_effect_outlives(
po,_tmpBC,_tmpBD))return 0;}}return 1;}static int Cyc_RgnOrder_eff_outlives_atomic_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);static int Cyc_RgnOrder_eff_outlives_atomic_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){eff2=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
eff2));{void*_tmpBE=eff2;struct Cyc_List_List*_tmpC0;void*_tmpC2;void*_tmpC4;
_LL7B: {struct Cyc_Absyn_JoinEff_struct*_tmpBF=(struct Cyc_Absyn_JoinEff_struct*)
_tmpBE;if(_tmpBF->tag != 24)goto _LL7D;else{_tmpC0=_tmpBF->f1;}}_LL7C: for(0;_tmpC0
!= 0;_tmpC0=_tmpC0->tl){if(Cyc_RgnOrder_eff_outlives_atomic_eff(po,eff1,(void*)
_tmpC0->hd))return 1;}return 0;_LL7D: {struct Cyc_Absyn_AccessEff_struct*_tmpC1=(
struct Cyc_Absyn_AccessEff_struct*)_tmpBE;if(_tmpC1->tag != 23)goto _LL7F;else{
_tmpC2=(void*)_tmpC1->f1;}}_LL7E: return Cyc_RgnOrder_effect_outlives(po,eff1,
_tmpC2);_LL7F: {struct Cyc_Absyn_RgnsEff_struct*_tmpC3=(struct Cyc_Absyn_RgnsEff_struct*)
_tmpBE;if(_tmpC3->tag != 25)goto _LL81;else{_tmpC4=(void*)_tmpC3->f1;}}_LL80: {
void*_tmpC5=eff1;void*_tmpC7;_LL84: {struct Cyc_Absyn_RgnsEff_struct*_tmpC6=(
struct Cyc_Absyn_RgnsEff_struct*)_tmpC5;if(_tmpC6->tag != 25)goto _LL86;else{_tmpC7=(
void*)_tmpC6->f1;}}_LL85:{struct _tuple0 _tmp13A;struct _tuple0 _tmpC9=(_tmp13A.f1=
Cyc_Tcutil_compress(_tmpC4),((_tmp13A.f2=Cyc_Tcutil_compress(_tmpC7),_tmp13A)));
void*_tmpCA;struct Cyc_Absyn_Tvar*_tmpCC;void*_tmpCD;struct Cyc_Absyn_Tvar*_tmpCF;
_LL89: _tmpCA=_tmpC9.f1;{struct Cyc_Absyn_VarType_struct*_tmpCB=(struct Cyc_Absyn_VarType_struct*)
_tmpCA;if(_tmpCB->tag != 2)goto _LL8B;else{_tmpCC=_tmpCB->f1;}};_tmpCD=_tmpC9.f2;{
struct Cyc_Absyn_VarType_struct*_tmpCE=(struct Cyc_Absyn_VarType_struct*)_tmpCD;
if(_tmpCE->tag != 2)goto _LL8B;else{_tmpCF=_tmpCE->f1;}};_LL8A: return Cyc_Absyn_tvar_cmp(
_tmpCC,_tmpCF)== 0;_LL8B:;_LL8C: goto _LL88;_LL88:;}goto _LL87;_LL86:;_LL87: return
eff1 == (void*)& Cyc_Absyn_HeapRgn_val;_LL83:;}_LL81:;_LL82: return eff1 == (void*)&
Cyc_Absyn_HeapRgn_val;_LL7A:;};}int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*
po,void*eff1,void*eff2);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*
po,void*eff1,void*eff2){eff1=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
eff1));{void*_tmpD0=eff1;struct Cyc_List_List*_tmpD2;_LL8E: {struct Cyc_Absyn_JoinEff_struct*
_tmpD1=(struct Cyc_Absyn_JoinEff_struct*)_tmpD0;if(_tmpD1->tag != 24)goto _LL90;
else{_tmpD2=_tmpD1->f1;}}_LL8F: for(0;_tmpD2 != 0;_tmpD2=_tmpD2->tl){if(!Cyc_RgnOrder_eff_outlives_eff(
po,(void*)_tmpD2->hd,eff2))return 0;}return 1;_LL90:;_LL91: return Cyc_RgnOrder_eff_outlives_atomic_eff(
po,eff1,eff2);_LL8D:;};}struct _tuple10{struct Cyc_Absyn_Tvar*f1;struct Cyc_RgnOrder_RgnInfo*
f2;};void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po){struct _RegionHandle _tmpD3=_new_region("r");struct
_RegionHandle*r=& _tmpD3;_push_region(r);{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,po->d);
struct _tuple10 _tmpD4=*((struct _tuple10*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict
d))Cyc_Dict_rchoose)(r,po->d);{const char*_tmp13D;void*_tmp13C;(_tmp13C=0,Cyc_fprintf(
Cyc_stderr,((_tmp13D="region po:\n",_tag_dyneither(_tmp13D,sizeof(char),12))),
_tag_dyneither(_tmp13C,sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple10*))Cyc_Iter_next)(iter,& _tmpD4)){const char*_tmp142;void*_tmp141[2];
struct Cyc_String_pa_struct _tmp140;struct Cyc_String_pa_struct _tmp13F;(_tmp13F.tag=
0,((_tmp13F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
_tmpD4.f2)->these_outlive)),((_tmp140.tag=0,((_tmp140.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(_tmpD4.f1)->name),((_tmp141[0]=& _tmp140,((_tmp141[1]=&
_tmp13F,Cyc_fprintf(Cyc_stderr,((_tmp142="  %s outlived by %s\n",_tag_dyneither(
_tmp142,sizeof(char),21))),_tag_dyneither(_tmp141,sizeof(void*),2)))))))))))));}{
const char*_tmp146;void*_tmp145[1];struct Cyc_String_pa_struct _tmp144;(_tmp144.tag=
0,((_tmp144.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
po->these_outlive_heap)),((_tmp145[0]=& _tmp144,Cyc_fprintf(Cyc_stderr,((_tmp146="  these outlive heap: %s\n",
_tag_dyneither(_tmp146,sizeof(char),26))),_tag_dyneither(_tmp145,sizeof(void*),1)))))));}{
const char*_tmp14A;void*_tmp149[1];struct Cyc_String_pa_struct _tmp148;(_tmp148.tag=
0,((_tmp148.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
po->these_outlive_unique)),((_tmp149[0]=& _tmp148,Cyc_fprintf(Cyc_stderr,((
_tmp14A="  these outlive unique: %s\n",_tag_dyneither(_tmp14A,sizeof(char),28))),
_tag_dyneither(_tmp149,sizeof(void*),1)))))));};};_pop_region(r);}
