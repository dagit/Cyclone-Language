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
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
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
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;struct
Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};struct
Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple0{union
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
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern
struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{
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
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_const_string_typ(
void*rgn);void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_array_typ(void*elt_type,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct _dyneither_ptr Cyc_Absyn_attribute2string(
void*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(
struct Cyc_Absyn_Kind*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(
struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(
struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*
s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};
struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*
r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);int Cyc_Dict_member(struct Cyc_Dict_Dict
d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);struct
_tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*,
int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct
_RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(struct _RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct
Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*,struct Cyc_List_List*);struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple0*);int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int
Cyc_Tcutil_is_function_type(void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*
k);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,
struct Cyc_Absyn_Kind*k,int allow_evars,void*);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_supports_default(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple9*eq;int isTrue;
};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple10{unsigned int f1;int f2;};
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);void Cyc_Tc_tc(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_struct{
char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;
struct Cyc_Absyn_Datatypefield*field;};struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(
struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(
struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);static char _tmp0[1]="";static struct _dyneither_ptr
Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};static struct _dyneither_ptr*Cyc_Tc_tc_msg=(
struct _dyneither_ptr*)& Cyc_Tc_tc_msg_c;struct _tuple11{struct Cyc_Position_Segment*
f1;struct _tuple0*f2;int f3;};static int Cyc_Tc_export_member(struct _tuple0*x,struct
Cyc_List_List*exports);static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*
exports){for(0;exports != 0;exports=exports->tl){struct _tuple11*_tmp1=(struct
_tuple11*)exports->hd;if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){(*_tmp1).f3=1;
return 1;}}return 0;}struct _tuple12{void*f1;int f2;};static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Vardecl*vd,int check_var_init,int ignore_init,struct Cyc_List_List**
exports);static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,
int ignore_init,struct Cyc_List_List**exports){struct Cyc_Absyn_Vardecl _tmp3;enum 
Cyc_Absyn_Scope _tmp4;struct _tuple0*_tmp5;struct _tuple0 _tmp6;union Cyc_Absyn_Nmspace
_tmp7;struct _dyneither_ptr*_tmp8;void*_tmp9;struct Cyc_Absyn_Exp*_tmpA;struct Cyc_List_List*
_tmpB;struct Cyc_Absyn_Vardecl*_tmp2=vd;_tmp3=*_tmp2;_tmp4=_tmp3.sc;_tmp5=_tmp3.name;
_tmp6=*_tmp5;_tmp7=_tmp6.f1;_tmp8=_tmp6.f2;_tmp9=_tmp3.type;_tmpA=_tmp3.initializer;
_tmpB=_tmp3.attributes;{union Cyc_Absyn_Nmspace _tmpC=_tmp7;struct Cyc_List_List*
_tmpD;struct Cyc_List_List*_tmpE;_LL1: if((_tmpC.Rel_n).tag != 1)goto _LL3;_tmpD=(
struct Cyc_List_List*)(_tmpC.Rel_n).val;if(_tmpD != 0)goto _LL3;_LL2: goto _LL4;_LL3:
if((_tmpC.Abs_n).tag != 2)goto _LL5;_tmpE=(struct Cyc_List_List*)(_tmpC.Abs_n).val;
if(_tmpE != 0)goto _LL5;_LL4: goto _LL0;_LL5:;_LL6:{const char*_tmp29E;void*_tmp29D[1];
struct Cyc_String_pa_struct _tmp29C;(_tmp29C.tag=0,((_tmp29C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((
_tmp29D[0]=& _tmp29C,Cyc_Tcutil_terr(loc,((_tmp29E="qualified variable declarations are not implemented (%s)",
_tag_dyneither(_tmp29E,sizeof(char),57))),_tag_dyneither(_tmp29D,sizeof(void*),1)))))));}
return;_LL0:;}(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns);{void*_tmp12=Cyc_Tcutil_compress(
_tmp9);struct Cyc_Absyn_ArrayInfo _tmp14;void*_tmp15;struct Cyc_Absyn_Tqual _tmp16;
struct Cyc_Absyn_Exp*_tmp17;union Cyc_Absyn_Constraint*_tmp18;struct Cyc_Position_Segment*
_tmp19;_LL8:{struct Cyc_Absyn_ArrayType_struct*_tmp13=(struct Cyc_Absyn_ArrayType_struct*)
_tmp12;if(_tmp13->tag != 9)goto _LLA;else{_tmp14=_tmp13->f1;_tmp15=_tmp14.elt_type;
_tmp16=_tmp14.tq;_tmp17=_tmp14.num_elts;if(_tmp17 != 0)goto _LLA;_tmp18=_tmp14.zero_term;
_tmp19=_tmp14.zt_loc;}}if(!(_tmpA != 0))goto _LLA;_LL9:{void*_tmp1A=_tmpA->r;union
Cyc_Absyn_Cnst _tmp1C;struct _dyneither_ptr _tmp1D;struct Cyc_Absyn_Exp*_tmp1F;
struct Cyc_List_List*_tmp21;struct Cyc_List_List*_tmp23;_LLD: {struct Cyc_Absyn_Const_e_struct*
_tmp1B=(struct Cyc_Absyn_Const_e_struct*)_tmp1A;if(_tmp1B->tag != 0)goto _LLF;else{
_tmp1C=_tmp1B->f1;if((_tmp1C.String_c).tag != 7)goto _LLF;_tmp1D=(struct
_dyneither_ptr)(_tmp1C.String_c).val;}}_LLE: _tmp9=(vd->type=Cyc_Absyn_array_typ(
_tmp15,_tmp16,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_dyneither_size(
_tmp1D,sizeof(char)),0),_tmp18,_tmp19));goto _LLC;_LLF: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1E=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A;if(_tmp1E->tag != 28)goto
_LL11;else{_tmp1F=_tmp1E->f2;}}_LL10: _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp15,
_tmp16,(struct Cyc_Absyn_Exp*)_tmp1F,_tmp18,_tmp19));goto _LLC;_LL11: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp20=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1A;if(_tmp20->tag != 36)goto
_LL13;else{_tmp21=_tmp20->f2;}}_LL12: _tmp23=_tmp21;goto _LL14;_LL13: {struct Cyc_Absyn_Array_e_struct*
_tmp22=(struct Cyc_Absyn_Array_e_struct*)_tmp1A;if(_tmp22->tag != 27)goto _LL15;
else{_tmp23=_tmp22->f1;}}_LL14: _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp15,_tmp16,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp23),0),_tmp18,_tmp19));goto _LLC;_LL15:;_LL16: goto _LLC;
_LLC:;}goto _LL7;_LLA:;_LLB: goto _LL7;_LL7:;}Cyc_Tcutil_check_valid_toplevel_type(
loc,te,_tmp9);(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,
_tmp9);{void*_tmp24=Cyc_Tcutil_compress(_tmp9);_LL18: {struct Cyc_Absyn_ArrayType_struct*
_tmp25=(struct Cyc_Absyn_ArrayType_struct*)_tmp24;if(_tmp25->tag != 9)goto _LL1A;}
_LL19: vd->escapes=0;goto _LL17;_LL1A:;_LL1B: vd->escapes=1;goto _LL17;_LL17:;}if(
Cyc_Tcutil_is_function_type(_tmp9))_tmpB=Cyc_Tcutil_transfer_fn_type_atts(_tmp9,
_tmpB);if(_tmp4 == Cyc_Absyn_Extern  || _tmp4 == Cyc_Absyn_ExternC){if(_tmpA != 0){
const char*_tmp2A1;void*_tmp2A0;(_tmp2A0=0,Cyc_Tcutil_terr(loc,((_tmp2A1="extern declaration should not have initializer",
_tag_dyneither(_tmp2A1,sizeof(char),47))),_tag_dyneither(_tmp2A0,sizeof(void*),0)));}}
else{if(!Cyc_Tcutil_is_function_type(_tmp9)){for(0;_tmpB != 0;_tmpB=_tmpB->tl){
void*_tmp28=(void*)_tmpB->hd;_LL1D: {struct Cyc_Absyn_Aligned_att_struct*_tmp29=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp28;if(_tmp29->tag != 6)goto _LL1F;}_LL1E:
goto _LL20;_LL1F: {struct Cyc_Absyn_Section_att_struct*_tmp2A=(struct Cyc_Absyn_Section_att_struct*)
_tmp28;if(_tmp2A->tag != 8)goto _LL21;}_LL20: goto _LL22;_LL21: {struct Cyc_Absyn_Nocommon_att_struct*
_tmp2B=(struct Cyc_Absyn_Nocommon_att_struct*)_tmp28;if(_tmp2B->tag != 9)goto _LL23;}
_LL22: goto _LL24;_LL23: {struct Cyc_Absyn_Shared_att_struct*_tmp2C=(struct Cyc_Absyn_Shared_att_struct*)
_tmp28;if(_tmp2C->tag != 10)goto _LL25;}_LL24: goto _LL26;_LL25: {struct Cyc_Absyn_Unused_att_struct*
_tmp2D=(struct Cyc_Absyn_Unused_att_struct*)_tmp28;if(_tmp2D->tag != 11)goto _LL27;}
_LL26: goto _LL28;_LL27: {struct Cyc_Absyn_Weak_att_struct*_tmp2E=(struct Cyc_Absyn_Weak_att_struct*)
_tmp28;if(_tmp2E->tag != 12)goto _LL29;}_LL28: goto _LL2A;_LL29: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp2F=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp28;if(_tmp2F->tag != 13)goto
_LL2B;}_LL2A: goto _LL2C;_LL2B: {struct Cyc_Absyn_Dllexport_att_struct*_tmp30=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp28;if(_tmp30->tag != 14)goto _LL2D;}
_LL2C: continue;_LL2D:;_LL2E:{const char*_tmp2A6;void*_tmp2A5[2];struct Cyc_String_pa_struct
_tmp2A4;struct Cyc_String_pa_struct _tmp2A3;(_tmp2A3.tag=0,((_tmp2A3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((
_tmp2A4.tag=0,((_tmp2A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB->hd)),((_tmp2A5[0]=& _tmp2A4,((_tmp2A5[1]=& _tmp2A3,Cyc_Tcutil_terr(
loc,((_tmp2A6="bad attribute %s for variable %s",_tag_dyneither(_tmp2A6,sizeof(
char),33))),_tag_dyneither(_tmp2A5,sizeof(void*),2)))))))))))));}goto _LL1C;_LL1C:;}
if(_tmpA == 0  || ignore_init){if(check_var_init  && !Cyc_Tcutil_supports_default(
_tmp9)){const char*_tmp2AB;void*_tmp2AA[2];struct Cyc_String_pa_struct _tmp2A9;
struct Cyc_String_pa_struct _tmp2A8;(_tmp2A8.tag=0,((_tmp2A8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9)),((_tmp2A9.tag=
0,((_tmp2A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
vd->name)),((_tmp2AA[0]=& _tmp2A9,((_tmp2AA[1]=& _tmp2A8,Cyc_Tcutil_terr(loc,((
_tmp2AB="initializer required for variable %s of type %s",_tag_dyneither(_tmp2AB,
sizeof(char),48))),_tag_dyneither(_tmp2AA,sizeof(void*),2)))))))))))));}}else{
struct Cyc_Absyn_Exp*_tmp39=(struct Cyc_Absyn_Exp*)_tmpA;void*_tmp3A=Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp9,_tmp39);if(!Cyc_Tcutil_coerce_assign(te,_tmp39,_tmp9)){{const
char*_tmp2B1;void*_tmp2B0[3];struct Cyc_String_pa_struct _tmp2AF;struct Cyc_String_pa_struct
_tmp2AE;struct Cyc_String_pa_struct _tmp2AD;(_tmp2AD.tag=0,((_tmp2AD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3A)),((_tmp2AE.tag=
0,((_tmp2AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp9)),((_tmp2AF.tag=0,((_tmp2AF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp2B0[0]=& _tmp2AF,((
_tmp2B0[1]=& _tmp2AE,((_tmp2B0[2]=& _tmp2AD,Cyc_Tcutil_terr(loc,((_tmp2B1="%s declared with type \n%s\n but initialized with type \n%s",
_tag_dyneither(_tmp2B1,sizeof(char),57))),_tag_dyneither(_tmp2B0,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}if(!Cyc_Tcutil_is_const_exp(te,_tmp39)){const char*
_tmp2B4;void*_tmp2B3;(_tmp2B3=0,Cyc_Tcutil_terr(loc,((_tmp2B4="initializer is not a constant expression",
_tag_dyneither(_tmp2B4,sizeof(char),41))),_tag_dyneither(_tmp2B3,sizeof(void*),0)));}}}
else{for(0;_tmpB != 0;_tmpB=_tmpB->tl){void*_tmp42=(void*)_tmpB->hd;_LL30: {
struct Cyc_Absyn_Regparm_att_struct*_tmp43=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp42;if(_tmp43->tag != 0)goto _LL32;}_LL31: goto _LL33;_LL32: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp44=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp42;if(_tmp44->tag != 1)goto _LL34;}
_LL33: goto _LL35;_LL34: {struct Cyc_Absyn_Cdecl_att_struct*_tmp45=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp42;if(_tmp45->tag != 2)goto _LL36;}_LL35: goto _LL37;_LL36: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp46=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp42;if(_tmp46->tag != 3)goto _LL38;}
_LL37: goto _LL39;_LL38: {struct Cyc_Absyn_Noreturn_att_struct*_tmp47=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp42;if(_tmp47->tag != 4)goto _LL3A;}_LL39: goto _LL3B;_LL3A: {struct Cyc_Absyn_Format_att_struct*
_tmp48=(struct Cyc_Absyn_Format_att_struct*)_tmp42;if(_tmp48->tag != 19)goto _LL3C;}
_LL3B: goto _LL3D;_LL3C: {struct Cyc_Absyn_Initializes_att_struct*_tmp49=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp42;if(_tmp49->tag != 20)goto _LL3E;}_LL3D: goto _LL3F;_LL3E: {struct Cyc_Absyn_Pure_att_struct*
_tmp4A=(struct Cyc_Absyn_Pure_att_struct*)_tmp42;if(_tmp4A->tag != 22)goto _LL40;}
_LL3F: goto _LL41;_LL40: {struct Cyc_Absyn_Const_att_struct*_tmp4B=(struct Cyc_Absyn_Const_att_struct*)
_tmp42;if(_tmp4B->tag != 5)goto _LL42;}_LL41: {const char*_tmp2B7;void*_tmp2B6;(
_tmp2B6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp2B7="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp2B7,
sizeof(char),45))),_tag_dyneither(_tmp2B6,sizeof(void*),0)));}_LL42: {struct Cyc_Absyn_Aligned_att_struct*
_tmp4C=(struct Cyc_Absyn_Aligned_att_struct*)_tmp42;if(_tmp4C->tag != 6)goto _LL44;}
_LL43: goto _LL45;_LL44: {struct Cyc_Absyn_Packed_att_struct*_tmp4D=(struct Cyc_Absyn_Packed_att_struct*)
_tmp42;if(_tmp4D->tag != 7)goto _LL46;}_LL45:{const char*_tmp2BB;void*_tmp2BA[1];
struct Cyc_String_pa_struct _tmp2B9;(_tmp2B9.tag=0,((_tmp2B9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpB->hd)),((
_tmp2BA[0]=& _tmp2B9,Cyc_Tcutil_terr(loc,((_tmp2BB="bad attribute %s in function declaration",
_tag_dyneither(_tmp2BB,sizeof(char),41))),_tag_dyneither(_tmp2BA,sizeof(void*),1)))))));}
goto _LL2F;_LL46:;_LL47: continue;_LL2F:;}}}{struct _handler_cons _tmp53;
_push_handler(& _tmp53);{int _tmp55=0;if(setjmp(_tmp53.handler))_tmp55=1;if(!
_tmp55){{struct _tuple12*_tmp56=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp8);void*_tmp57=(*_tmp56).f1;
void*_tmp59;_LL49: {struct Cyc_Tcenv_VarRes_struct*_tmp58=(struct Cyc_Tcenv_VarRes_struct*)
_tmp57;if(_tmp58->tag != 0)goto _LL4B;else{_tmp59=(void*)_tmp58->f1;}}_LL4A: {
struct Cyc_Absyn_Global_b_struct _tmp2BE;struct Cyc_Absyn_Global_b_struct*_tmp2BD;
struct Cyc_Absyn_Global_b_struct*_tmp5E=(_tmp2BD=_cycalloc(sizeof(*_tmp2BD)),((
_tmp2BD[0]=((_tmp2BE.tag=1,((_tmp2BE.f1=vd,_tmp2BE)))),_tmp2BD)));void*_tmp5F=
Cyc_Tcdecl_merge_binding(_tmp59,(void*)_tmp5E,loc,Cyc_Tc_tc_msg);if(_tmp5F == (
void*)& Cyc_Absyn_Unresolved_b_val){_npop_handler(0);return;}if(_tmp5F == _tmp59
 && (*_tmp56).f2){_npop_handler(0);return;}if(exports == 0  || Cyc_Tc_export_member(
vd->name,*exports)){struct Cyc_Tcenv_VarRes_struct*_tmp2C4;struct Cyc_Tcenv_VarRes_struct
_tmp2C3;struct _tuple12*_tmp2C2;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(
struct _tuple12*)((_tmp2C2=_cycalloc(sizeof(*_tmp2C2)),((_tmp2C2->f1=(void*)((
_tmp2C4=_cycalloc(sizeof(*_tmp2C4)),((_tmp2C4[0]=((_tmp2C3.tag=0,((_tmp2C3.f1=(
void*)_tmp5F,_tmp2C3)))),_tmp2C4)))),((_tmp2C2->f2=1,_tmp2C2)))))));}
_npop_handler(0);return;}_LL4B: {struct Cyc_Tcenv_AggrRes_struct*_tmp5A=(struct
Cyc_Tcenv_AggrRes_struct*)_tmp57;if(_tmp5A->tag != 1)goto _LL4D;}_LL4C:{const char*
_tmp2C7;void*_tmp2C6;(_tmp2C6=0,Cyc_Tcutil_warn(loc,((_tmp2C7="variable declaration shadows previous struct declaration",
_tag_dyneither(_tmp2C7,sizeof(char),57))),_tag_dyneither(_tmp2C6,sizeof(void*),0)));}
goto _LL48;_LL4D: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp5B=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp57;if(_tmp5B->tag != 2)goto _LL4F;}_LL4E:{const char*_tmp2CA;void*_tmp2C9;(
_tmp2C9=0,Cyc_Tcutil_warn(loc,((_tmp2CA="variable declaration shadows previous datatype constructor",
_tag_dyneither(_tmp2CA,sizeof(char),59))),_tag_dyneither(_tmp2C9,sizeof(void*),0)));}
goto _LL48;_LL4F: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp5C=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp57;if(_tmp5C->tag != 4)goto _LL51;}_LL50: goto _LL52;_LL51: {struct Cyc_Tcenv_EnumRes_struct*
_tmp5D=(struct Cyc_Tcenv_EnumRes_struct*)_tmp57;if(_tmp5D->tag != 3)goto _LL48;}
_LL52:{const char*_tmp2CD;void*_tmp2CC;(_tmp2CC=0,Cyc_Tcutil_warn(loc,((_tmp2CD="variable declaration shadows previous enum tag",
_tag_dyneither(_tmp2CD,sizeof(char),47))),_tag_dyneither(_tmp2CC,sizeof(void*),0)));}
goto _LL48;_LL48:;};_pop_handler();}else{void*_tmp54=(void*)_exn_thrown;void*
_tmp6C=_tmp54;_LL54: {struct Cyc_Dict_Absent_struct*_tmp6D=(struct Cyc_Dict_Absent_struct*)
_tmp6C;if(_tmp6D->tag != Cyc_Dict_Absent)goto _LL56;}_LL55: goto _LL53;_LL56:;_LL57:(
void)_throw(_tmp6C);_LL53:;}};}if(exports == 0  || Cyc_Tc_export_member(vd->name,*
exports)){struct Cyc_Tcenv_VarRes_struct*_tmp2DC;struct Cyc_Absyn_Global_b_struct*
_tmp2DB;struct Cyc_Absyn_Global_b_struct _tmp2DA;struct Cyc_Tcenv_VarRes_struct
_tmp2D9;struct _tuple12*_tmp2D8;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(
struct _tuple12*)((_tmp2D8=_cycalloc(sizeof(*_tmp2D8)),((_tmp2D8->f1=(void*)((
_tmp2DC=_cycalloc(sizeof(*_tmp2DC)),((_tmp2DC[0]=((_tmp2D9.tag=0,((_tmp2D9.f1=(
void*)((void*)((_tmp2DB=_cycalloc(sizeof(*_tmp2DB)),((_tmp2DB[0]=((_tmp2DA.tag=1,((
_tmp2DA.f1=vd,_tmp2DA)))),_tmp2DB))))),_tmp2D9)))),_tmp2DC)))),((_tmp2D8->f2=0,
_tmp2D8)))))));}}static int Cyc_Tc_is_main(struct _tuple0*n);static int Cyc_Tc_is_main(
struct _tuple0*n){struct _tuple0 _tmp74;union Cyc_Absyn_Nmspace _tmp75;struct
_dyneither_ptr*_tmp76;struct _tuple0*_tmp73=n;_tmp74=*_tmp73;_tmp75=_tmp74.f1;
_tmp76=_tmp74.f2;{union Cyc_Absyn_Nmspace _tmp77=_tmp75;struct Cyc_List_List*_tmp78;
_LL59: if((_tmp77.Abs_n).tag != 2)goto _LL5B;_tmp78=(struct Cyc_List_List*)(_tmp77.Abs_n).val;
if(_tmp78 != 0)goto _LL5B;_LL5A: {const char*_tmp2DD;return Cyc_strcmp((struct
_dyneither_ptr)*_tmp76,((_tmp2DD="main",_tag_dyneither(_tmp2DD,sizeof(char),5))))
== 0;}_LL5B:;_LL5C: return 0;_LL58:;};}struct _tuple13{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*
fd,int ignore_body,struct Cyc_List_List**exports);static void Cyc_Tc_tcFndecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct
Cyc_Absyn_Fndecl*fd,int ignore_body,struct Cyc_List_List**exports){struct
_dyneither_ptr*v=(*fd->name).f2;if(fd->sc == Cyc_Absyn_ExternC  && !ignore_body){
const char*_tmp2E0;void*_tmp2DF;(_tmp2DF=0,Cyc_Tcutil_terr(loc,((_tmp2E0="extern \"C\" functions cannot be implemented in Cyclone",
_tag_dyneither(_tmp2E0,sizeof(char),54))),_tag_dyneither(_tmp2DF,sizeof(void*),0)));}{
union Cyc_Absyn_Nmspace _tmp7C=(*fd->name).f1;struct Cyc_List_List*_tmp7D;struct Cyc_List_List*
_tmp7E;_LL5E: if((_tmp7C.Rel_n).tag != 1)goto _LL60;_tmp7D=(struct Cyc_List_List*)(
_tmp7C.Rel_n).val;if(_tmp7D != 0)goto _LL60;_LL5F: goto _LL5D;_LL60: if((_tmp7C.Abs_n).tag
!= 2)goto _LL62;_tmp7E=(struct Cyc_List_List*)(_tmp7C.Abs_n).val;_LL61: {const char*
_tmp2E3;void*_tmp2E2;(_tmp2E2=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2E3="tc: Abs_n in tcFndecl",
_tag_dyneither(_tmp2E3,sizeof(char),22))),_tag_dyneither(_tmp2E2,sizeof(void*),0)));}
_LL62:;_LL63:{const char*_tmp2E7;void*_tmp2E6[1];struct Cyc_String_pa_struct
_tmp2E5;(_tmp2E5.tag=0,((_tmp2E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(fd->name)),((_tmp2E6[0]=& _tmp2E5,Cyc_Tcutil_terr(loc,((
_tmp2E7="qualified function declarations are not implemented (%s)",
_tag_dyneither(_tmp2E7,sizeof(char),57))),_tag_dyneither(_tmp2E6,sizeof(void*),1)))))));}
return;_LL5D:;}(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns);Cyc_Tcutil_check_fndecl_valid_type(
loc,te,fd);{void*t=Cyc_Tcutil_fndecl2typ(fd);fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(
t,fd->attributes);{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=
atts->tl){void*_tmp84=(void*)atts->hd;_LL65: {struct Cyc_Absyn_Packed_att_struct*
_tmp85=(struct Cyc_Absyn_Packed_att_struct*)_tmp84;if(_tmp85->tag != 7)goto _LL67;}
_LL66: goto _LL68;_LL67: {struct Cyc_Absyn_Aligned_att_struct*_tmp86=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp84;if(_tmp86->tag != 6)goto _LL69;}_LL68:{const char*_tmp2EB;void*_tmp2EA[1];
struct Cyc_String_pa_struct _tmp2E9;(_tmp2E9.tag=0,((_tmp2E9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((
_tmp2EA[0]=& _tmp2E9,Cyc_Tcutil_terr(loc,((_tmp2EB="bad attribute %s for function",
_tag_dyneither(_tmp2EB,sizeof(char),30))),_tag_dyneither(_tmp2EA,sizeof(void*),1)))))));}
goto _LL64;_LL69:;_LL6A: goto _LL64;_LL64:;}}{struct _handler_cons _tmp8A;
_push_handler(& _tmp8A);{int _tmp8C=0;if(setjmp(_tmp8A.handler))_tmp8C=1;if(!
_tmp8C){{struct _tuple12*_tmp8D=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);void*_tmp8E=(*_tmp8D).f1;
void*_tmp90;_LL6C: {struct Cyc_Tcenv_VarRes_struct*_tmp8F=(struct Cyc_Tcenv_VarRes_struct*)
_tmp8E;if(_tmp8F->tag != 0)goto _LL6E;else{_tmp90=(void*)_tmp8F->f1;}}_LL6D: {
struct Cyc_Absyn_Funname_b_struct _tmp2EE;struct Cyc_Absyn_Funname_b_struct*_tmp2ED;
struct Cyc_Absyn_Funname_b_struct*_tmp95=(_tmp2ED=_cycalloc(sizeof(*_tmp2ED)),((
_tmp2ED[0]=((_tmp2EE.tag=2,((_tmp2EE.f1=fd,_tmp2EE)))),_tmp2ED)));void*_tmp96=
Cyc_Tcdecl_merge_binding(_tmp90,(void*)_tmp95,loc,Cyc_Tc_tc_msg);if(_tmp96 == (
void*)& Cyc_Absyn_Unresolved_b_val)goto _LL6B;if(_tmp96 == _tmp90  && (*_tmp8D).f2)
goto _LL6B;if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){struct Cyc_Tcenv_VarRes_struct*
_tmp2F4;struct Cyc_Tcenv_VarRes_struct _tmp2F3;struct _tuple12*_tmp2F2;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple12*v))Cyc_Dict_insert)(ge->ordinaries,v,(struct _tuple12*)((_tmp2F2=
_cycalloc(sizeof(*_tmp2F2)),((_tmp2F2->f1=(void*)((_tmp2F4=_cycalloc(sizeof(*
_tmp2F4)),((_tmp2F4[0]=((_tmp2F3.tag=0,((_tmp2F3.f1=(void*)_tmp96,_tmp2F3)))),
_tmp2F4)))),((_tmp2F2->f2=1,_tmp2F2)))))));}goto _LL6B;}_LL6E: {struct Cyc_Tcenv_AggrRes_struct*
_tmp91=(struct Cyc_Tcenv_AggrRes_struct*)_tmp8E;if(_tmp91->tag != 1)goto _LL70;}
_LL6F:{const char*_tmp2F7;void*_tmp2F6;(_tmp2F6=0,Cyc_Tcutil_warn(loc,((_tmp2F7="function declaration shadows previous type declaration",
_tag_dyneither(_tmp2F7,sizeof(char),55))),_tag_dyneither(_tmp2F6,sizeof(void*),0)));}
goto _LL6B;_LL70: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp92=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp8E;if(_tmp92->tag != 2)goto _LL72;}_LL71:{const char*_tmp2FA;void*_tmp2F9;(
_tmp2F9=0,Cyc_Tcutil_warn(loc,((_tmp2FA="function declaration shadows previous datatype constructor",
_tag_dyneither(_tmp2FA,sizeof(char),59))),_tag_dyneither(_tmp2F9,sizeof(void*),0)));}
goto _LL6B;_LL72: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp93=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp8E;if(_tmp93->tag != 4)goto _LL74;}_LL73: goto _LL75;_LL74: {struct Cyc_Tcenv_EnumRes_struct*
_tmp94=(struct Cyc_Tcenv_EnumRes_struct*)_tmp8E;if(_tmp94->tag != 3)goto _LL6B;}
_LL75:{const char*_tmp2FD;void*_tmp2FC;(_tmp2FC=0,Cyc_Tcutil_warn(loc,((_tmp2FD="function declaration shadows previous enum tag",
_tag_dyneither(_tmp2FD,sizeof(char),47))),_tag_dyneither(_tmp2FC,sizeof(void*),0)));}
goto _LL6B;_LL6B:;};_pop_handler();}else{void*_tmp8B=(void*)_exn_thrown;void*
_tmpA3=_tmp8B;_LL77: {struct Cyc_Dict_Absent_struct*_tmpA4=(struct Cyc_Dict_Absent_struct*)
_tmpA3;if(_tmpA4->tag != Cyc_Dict_Absent)goto _LL79;}_LL78: if(exports == 0  || Cyc_Tc_export_member(
fd->name,*exports)){struct Cyc_Tcenv_VarRes_struct*_tmp30C;struct Cyc_Absyn_Funname_b_struct*
_tmp30B;struct Cyc_Absyn_Funname_b_struct _tmp30A;struct Cyc_Tcenv_VarRes_struct
_tmp309;struct _tuple12*_tmp308;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,v,(
struct _tuple12*)((_tmp308=_cycalloc(sizeof(*_tmp308)),((_tmp308->f1=(void*)((
_tmp30C=_cycalloc(sizeof(*_tmp30C)),((_tmp30C[0]=((_tmp309.tag=0,((_tmp309.f1=(
void*)((void*)((_tmp30B=_cycalloc(sizeof(*_tmp30B)),((_tmp30B[0]=((_tmp30A.tag=2,((
_tmp30A.f1=fd,_tmp30A)))),_tmp30B))))),_tmp309)))),_tmp30C)))),((_tmp308->f2=0,
_tmp308)))))));}goto _LL76;_LL79:;_LL7A:(void)_throw(_tmpA3);_LL76:;}};}if(
ignore_body)return;{struct _RegionHandle _tmpAA=_new_region("fnrgn");struct
_RegionHandle*fnrgn=& _tmpAA;_push_region(fnrgn);{struct Cyc_Tcenv_Fenv*_tmpAB=Cyc_Tcenv_new_fenv(
fnrgn,loc,fd);struct Cyc_Tcenv_Tenv*_tmp30D;struct Cyc_Tcenv_Tenv*_tmpAC=(_tmp30D=
_region_malloc(fnrgn,sizeof(*_tmp30D)),((_tmp30D->ns=te->ns,((_tmp30D->ae=te->ae,((
_tmp30D->le=(struct Cyc_Tcenv_Fenv*)_tmpAB,((_tmp30D->allow_valueof=0,_tmp30D)))))))));
Cyc_Tcstmt_tcStmt(_tmpAC,fd->body,0);Cyc_Tcenv_check_delayed_effects(_tmpAC);Cyc_Tcenv_check_delayed_constraints(
_tmpAC);if(!Cyc_Tcenv_all_labels_resolved(_tmpAC)){const char*_tmp310;void*
_tmp30F;(_tmp30F=0,Cyc_Tcutil_terr(loc,((_tmp310="function has goto statements to undefined labels",
_tag_dyneither(_tmp310,sizeof(char),49))),_tag_dyneither(_tmp30F,sizeof(void*),0)));}}
if(Cyc_Tc_is_main(fd->name)){{void*_tmpB0=Cyc_Tcutil_compress(fd->ret_type);
enum Cyc_Absyn_Size_of _tmpB3;_LL7C: {struct Cyc_Absyn_VoidType_struct*_tmpB1=(
struct Cyc_Absyn_VoidType_struct*)_tmpB0;if(_tmpB1->tag != 0)goto _LL7E;}_LL7D:{
const char*_tmp313;void*_tmp312;(_tmp312=0,Cyc_Tcutil_warn(loc,((_tmp313="main declared with return type void",
_tag_dyneither(_tmp313,sizeof(char),36))),_tag_dyneither(_tmp312,sizeof(void*),0)));}
goto _LL7B;_LL7E: {struct Cyc_Absyn_IntType_struct*_tmpB2=(struct Cyc_Absyn_IntType_struct*)
_tmpB0;if(_tmpB2->tag != 6)goto _LL80;else{_tmpB3=_tmpB2->f2;}}_LL7F: goto _LL7B;
_LL80:;_LL81:{const char*_tmp317;void*_tmp316[1];struct Cyc_String_pa_struct
_tmp315;(_tmp315.tag=0,((_tmp315.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type)),((_tmp316[0]=& _tmp315,Cyc_Tcutil_terr(loc,((
_tmp317="main declared with return type %s instead of int or void",
_tag_dyneither(_tmp317,sizeof(char),57))),_tag_dyneither(_tmp316,sizeof(void*),1)))))));}
goto _LL7B;_LL7B:;}if(fd->c_varargs  || fd->cyc_varargs != 0){const char*_tmp31A;
void*_tmp319;(_tmp319=0,Cyc_Tcutil_terr(loc,((_tmp31A="main declared with varargs",
_tag_dyneither(_tmp31A,sizeof(char),27))),_tag_dyneither(_tmp319,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpBB=fd->args;if(_tmpBB != 0){struct _tuple13 _tmpBD;void*
_tmpBE;struct _tuple13*_tmpBC=(struct _tuple13*)_tmpBB->hd;_tmpBD=*_tmpBC;_tmpBE=
_tmpBD.f3;{void*_tmpBF=Cyc_Tcutil_compress(_tmpBE);enum Cyc_Absyn_Size_of _tmpC1;
_LL83: {struct Cyc_Absyn_IntType_struct*_tmpC0=(struct Cyc_Absyn_IntType_struct*)
_tmpBF;if(_tmpC0->tag != 6)goto _LL85;else{_tmpC1=_tmpC0->f2;}}_LL84: goto _LL82;
_LL85:;_LL86:{const char*_tmp31E;void*_tmp31D[1];struct Cyc_String_pa_struct
_tmp31C;(_tmp31C.tag=0,((_tmp31C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpBE)),((_tmp31D[0]=& _tmp31C,Cyc_Tcutil_terr(loc,((
_tmp31E="main declared with first argument of type %s instead of int",
_tag_dyneither(_tmp31E,sizeof(char),60))),_tag_dyneither(_tmp31D,sizeof(void*),1)))))));}
goto _LL82;_LL82:;}_tmpBB=_tmpBB->tl;if(_tmpBB != 0){struct _tuple13 _tmpC6;void*
_tmpC7;struct _tuple13*_tmpC5=(struct _tuple13*)_tmpBB->hd;_tmpC6=*_tmpC5;_tmpC7=
_tmpC6.f3;_tmpBB=_tmpBB->tl;if(_tmpBB != 0){const char*_tmp321;void*_tmp320;(
_tmp320=0,Cyc_Tcutil_terr(loc,((_tmp321="main declared with too many arguments",
_tag_dyneither(_tmp321,sizeof(char),38))),_tag_dyneither(_tmp320,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp322;struct Cyc_Core_Opt*tvs=(_tmp322=_cycalloc(sizeof(*
_tmp322)),((_tmp322->v=fd->tvs,_tmp322)));struct Cyc_Core_Opt*_tmp32A;struct Cyc_Core_Opt*
_tmp329;struct Cyc_Core_Opt*_tmp328;struct Cyc_Core_Opt*_tmp327;struct Cyc_Core_Opt*
_tmp326;struct Cyc_Core_Opt*_tmp325;struct Cyc_Core_Opt*_tmp324;struct Cyc_Core_Opt*
_tmp323;if(((!Cyc_Tcutil_unify(_tmpC7,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(
Cyc_Absyn_new_evar(((_tmp329=_cycalloc(sizeof(*_tmp329)),((_tmp329->v=& Cyc_Tcutil_rk,
_tmp329)))),tvs)),Cyc_Absyn_new_evar(((_tmp32A=_cycalloc(sizeof(*_tmp32A)),((
_tmp32A->v=& Cyc_Tcutil_rk,_tmp32A)))),tvs),Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)())) && !Cyc_Tcutil_unify(_tmpC7,Cyc_Absyn_dyneither_typ(
Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp327=_cycalloc(sizeof(*
_tmp327)),((_tmp327->v=& Cyc_Tcutil_rk,_tmp327)))),tvs)),Cyc_Absyn_new_evar(((
_tmp328=_cycalloc(sizeof(*_tmp328)),((_tmp328->v=& Cyc_Tcutil_rk,_tmp328)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
 && !Cyc_Tcutil_unify(_tmpC7,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((
_tmp325=_cycalloc(sizeof(*_tmp325)),((_tmp325->v=& Cyc_Tcutil_rk,_tmp325)))),tvs)),
Cyc_Absyn_new_evar(((_tmp326=_cycalloc(sizeof(*_tmp326)),((_tmp326->v=& Cyc_Tcutil_rk,
_tmp326)))),tvs),Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
 && !Cyc_Tcutil_unify(_tmpC7,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(
Cyc_Absyn_new_evar(((_tmp323=_cycalloc(sizeof(*_tmp323)),((_tmp323->v=& Cyc_Tcutil_rk,
_tmp323)))),tvs)),Cyc_Absyn_new_evar(((_tmp324=_cycalloc(sizeof(*_tmp324)),((
_tmp324->v=& Cyc_Tcutil_rk,_tmp324)))),tvs),Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)()))){const char*_tmp32E;void*_tmp32D[1];struct Cyc_String_pa_struct
_tmp32C;(_tmp32C.tag=0,((_tmp32C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpC7)),((_tmp32D[0]=& _tmp32C,Cyc_Tcutil_terr(loc,((
_tmp32E="second argument of main has type %s instead of char??",_tag_dyneither(
_tmp32E,sizeof(char),54))),_tag_dyneither(_tmp32D,sizeof(void*),1)))))));}};}}};};
_pop_region(fnrgn);};};}static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*
td);static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*td){struct
_dyneither_ptr*v=(*td->name).f2;{union Cyc_Absyn_Nmspace _tmpD6=(*td->name).f1;
struct Cyc_List_List*_tmpD7;struct Cyc_List_List*_tmpD8;_LL88: if((_tmpD6.Rel_n).tag
!= 1)goto _LL8A;_tmpD7=(struct Cyc_List_List*)(_tmpD6.Rel_n).val;if(_tmpD7 != 0)
goto _LL8A;_LL89: goto _LL8B;_LL8A: if((_tmpD6.Abs_n).tag != 2)goto _LL8C;_tmpD8=(
struct Cyc_List_List*)(_tmpD6.Abs_n).val;if(_tmpD8 != 0)goto _LL8C;_LL8B: goto _LL87;
_LL8C:;_LL8D:{const char*_tmp332;void*_tmp331[1];struct Cyc_String_pa_struct
_tmp330;(_tmp330.tag=0,((_tmp330.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(td->name)),((_tmp331[0]=& _tmp330,Cyc_Tcutil_terr(loc,((
_tmp332="qualified typedef declarations are not implemented (%s)",_tag_dyneither(
_tmp332,sizeof(char),56))),_tag_dyneither(_tmp331,sizeof(void*),1)))))));}
return;_LL87:;}if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(
ge->typedefs,v)){{const char*_tmp336;void*_tmp335[1];struct Cyc_String_pa_struct
_tmp334;(_tmp334.tag=0,((_tmp334.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
v),((_tmp335[0]=& _tmp334,Cyc_Tcutil_terr(loc,((_tmp336="redeclaration of typedef %s",
_tag_dyneither(_tmp336,sizeof(char),28))),_tag_dyneither(_tmp335,sizeof(void*),1)))))));}
return;}(*td->name).f1=Cyc_Absyn_Abs_n(te->ns);Cyc_Tcutil_check_unique_tvars(loc,
td->tvs);Cyc_Tcutil_add_tvar_identities(td->tvs);if(td->defn != 0){Cyc_Tcutil_check_type(
loc,te,td->tvs,& Cyc_Tcutil_tak,0,(void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);(
td->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(
void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);}{struct Cyc_List_List*tvs=
td->tvs;for(0;tvs != 0;tvs=tvs->tl){void*_tmpDF=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmpE1;struct Cyc_Core_Opt**_tmpE2;struct Cyc_Core_Opt*
_tmpE4;struct Cyc_Core_Opt**_tmpE5;struct Cyc_Absyn_Kind*_tmpE6;_LL8F: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpE0=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpDF;if(_tmpE0->tag != 1)goto _LL91;
else{_tmpE1=_tmpE0->f1;_tmpE2=(struct Cyc_Core_Opt**)& _tmpE0->f1;}}_LL90: if(td->defn
!= 0){const char*_tmp33A;void*_tmp339[1];struct Cyc_String_pa_struct _tmp338;(
_tmp338.tag=0,((_tmp338.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp339[0]=& _tmp338,Cyc_Tcutil_warn(loc,((
_tmp33A="type variable %s is not used in typedef definition",_tag_dyneither(
_tmp33A,sizeof(char),51))),_tag_dyneither(_tmp339,sizeof(void*),1)))))));}{
struct Cyc_Core_Opt*_tmp33B;*_tmpE2=((_tmp33B=_cycalloc(sizeof(*_tmp33B)),((
_tmp33B->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk),_tmp33B))));}goto _LL8E;
_LL91: {struct Cyc_Absyn_Less_kb_struct*_tmpE3=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDF;if(_tmpE3->tag != 2)goto _LL93;else{_tmpE4=_tmpE3->f1;_tmpE5=(struct Cyc_Core_Opt**)&
_tmpE3->f1;_tmpE6=_tmpE3->f2;}}_LL92:{struct Cyc_Core_Opt*_tmp33C;*_tmpE5=((
_tmp33C=_cycalloc(sizeof(*_tmp33C)),((_tmp33C->v=Cyc_Tcutil_kind_to_bound(_tmpE6),
_tmp33C))));}goto _LL8E;_LL93:;_LL94: continue;_LL8E:;}}ge->typedefs=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(
ge->typedefs,v,td);}static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct
Cyc_List_List*rpo,struct Cyc_List_List*fields);static void Cyc_Tc_tcAggrImpl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct
Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){struct
_RegionHandle _tmpEC=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=&
_tmpEC;_push_region(uprev_rgn);for(0;rpo != 0;rpo=rpo->tl){struct _tuple9 _tmpEE;
void*_tmpEF;void*_tmpF0;struct _tuple9*_tmpED=(struct _tuple9*)rpo->hd;_tmpEE=*
_tmpED;_tmpEF=_tmpEE.f1;_tmpF0=_tmpEE.f2;Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,
0,_tmpEF);Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,_tmpF0);}{struct Cyc_List_List*
prev_fields=0;struct Cyc_List_List*_tmpF1=fields;for(0;_tmpF1 != 0;_tmpF1=_tmpF1->tl){
struct Cyc_Absyn_Aggrfield _tmpF3;struct _dyneither_ptr*_tmpF4;struct Cyc_Absyn_Tqual
_tmpF5;void*_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_List_List*_tmpF8;struct
Cyc_Absyn_Aggrfield*_tmpF2=(struct Cyc_Absyn_Aggrfield*)_tmpF1->hd;_tmpF3=*_tmpF2;
_tmpF4=_tmpF3.name;_tmpF5=_tmpF3.tq;_tmpF6=_tmpF3.type;_tmpF7=_tmpF3.width;
_tmpF8=_tmpF3.attributes;if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct
_dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,
prev_fields,_tmpF4)){const char*_tmp340;void*_tmp33F[1];struct Cyc_String_pa_struct
_tmp33E;(_tmp33E.tag=0,((_tmp33E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpF4),((_tmp33F[0]=& _tmp33E,Cyc_Tcutil_terr(loc,((_tmp340="duplicate field %s",
_tag_dyneither(_tmp340,sizeof(char),19))),_tag_dyneither(_tmp33F,sizeof(void*),1)))))));}{
const char*_tmp341;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpF4,((_tmp341="",
_tag_dyneither(_tmp341,sizeof(char),1))))!= 0){struct Cyc_List_List*_tmp342;
prev_fields=((_tmp342=_region_malloc(uprev_rgn,sizeof(*_tmp342)),((_tmp342->hd=
_tmpF4,((_tmp342->tl=prev_fields,_tmp342))))));}}Cyc_Tcutil_check_type(loc,te,
tvs,& Cyc_Tcutil_tmk,0,_tmpF6);(((struct Cyc_Absyn_Aggrfield*)_tmpF1->hd)->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmpF1->hd)->tq).print_const,
_tmpF6);Cyc_Tcutil_check_bitfield(loc,te,_tmpF6,_tmpF7,_tmpF4);}};;_pop_region(
uprev_rgn);}static void Cyc_Tc_rule_out_memkind(struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int constrain_top_kind);static void
Cyc_Tc_rule_out_memkind(struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,
struct Cyc_List_List*tvs,int constrain_top_kind){struct Cyc_List_List*tvs2=tvs;for(
0;tvs2 != 0;tvs2=tvs2->tl){void*_tmpFE=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp100;struct Cyc_Core_Opt**_tmp101;struct
Cyc_Core_Opt*_tmp103;struct Cyc_Core_Opt**_tmp104;struct Cyc_Absyn_Kind*_tmp105;
struct Cyc_Absyn_Kind _tmp106;enum Cyc_Absyn_KindQual _tmp107;enum Cyc_Absyn_AliasQual
_tmp108;struct Cyc_Core_Opt*_tmp10A;struct Cyc_Core_Opt**_tmp10B;struct Cyc_Absyn_Kind*
_tmp10C;struct Cyc_Absyn_Kind _tmp10D;enum Cyc_Absyn_KindQual _tmp10E;enum Cyc_Absyn_AliasQual
_tmp10F;struct Cyc_Core_Opt*_tmp111;struct Cyc_Core_Opt**_tmp112;struct Cyc_Absyn_Kind*
_tmp113;struct Cyc_Absyn_Kind*_tmp115;struct Cyc_Absyn_Kind _tmp116;enum Cyc_Absyn_KindQual
_tmp117;enum Cyc_Absyn_AliasQual _tmp118;_LL96: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpFF=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpFE;if(_tmpFF->tag != 1)goto _LL98;
else{_tmp100=_tmpFF->f1;_tmp101=(struct Cyc_Core_Opt**)& _tmpFF->f1;}}_LL97:{
struct Cyc_Core_Opt*_tmp343;*_tmp101=((_tmp343=_cycalloc(sizeof(*_tmp343)),((
_tmp343->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp343))));}continue;_LL98: {
struct Cyc_Absyn_Less_kb_struct*_tmp102=(struct Cyc_Absyn_Less_kb_struct*)_tmpFE;
if(_tmp102->tag != 2)goto _LL9A;else{_tmp103=_tmp102->f1;_tmp104=(struct Cyc_Core_Opt**)&
_tmp102->f1;_tmp105=_tmp102->f2;_tmp106=*_tmp105;_tmp107=_tmp106.kind;if(_tmp107
!= Cyc_Absyn_MemKind)goto _LL9A;_tmp108=_tmp106.aliasqual;}}_LL99: if(
constrain_top_kind  && _tmp108 == Cyc_Absyn_Top){struct Cyc_Absyn_Kind*_tmp346;
struct Cyc_Core_Opt*_tmp345;*_tmp104=((_tmp345=_cycalloc(sizeof(*_tmp345)),((
_tmp345->v=Cyc_Tcutil_kind_to_bound(((_tmp346=_cycalloc_atomic(sizeof(*_tmp346)),((
_tmp346->kind=Cyc_Absyn_BoxKind,((_tmp346->aliasqual=Cyc_Absyn_Aliasable,_tmp346))))))),
_tmp345))));}else{struct Cyc_Absyn_Kind*_tmp349;struct Cyc_Core_Opt*_tmp348;*
_tmp104=((_tmp348=_cycalloc(sizeof(*_tmp348)),((_tmp348->v=Cyc_Tcutil_kind_to_bound(((
_tmp349=_cycalloc_atomic(sizeof(*_tmp349)),((_tmp349->kind=Cyc_Absyn_BoxKind,((
_tmp349->aliasqual=_tmp108,_tmp349))))))),_tmp348))));}continue;_LL9A:{struct Cyc_Absyn_Less_kb_struct*
_tmp109=(struct Cyc_Absyn_Less_kb_struct*)_tmpFE;if(_tmp109->tag != 2)goto _LL9C;
else{_tmp10A=_tmp109->f1;_tmp10B=(struct Cyc_Core_Opt**)& _tmp109->f1;_tmp10C=
_tmp109->f2;_tmp10D=*_tmp10C;_tmp10E=_tmp10D.kind;if(_tmp10E != Cyc_Absyn_BoxKind)
goto _LL9C;_tmp10F=_tmp10D.aliasqual;if(_tmp10F != Cyc_Absyn_Top)goto _LL9C;}}if(!
constrain_top_kind)goto _LL9C;_LL9B:{struct Cyc_Absyn_Kind*_tmp34C;struct Cyc_Core_Opt*
_tmp34B;*_tmp10B=((_tmp34B=_cycalloc(sizeof(*_tmp34B)),((_tmp34B->v=Cyc_Tcutil_kind_to_bound(((
_tmp34C=_cycalloc_atomic(sizeof(*_tmp34C)),((_tmp34C->kind=Cyc_Absyn_BoxKind,((
_tmp34C->aliasqual=Cyc_Absyn_Aliasable,_tmp34C))))))),_tmp34B))));}continue;
_LL9C: {struct Cyc_Absyn_Less_kb_struct*_tmp110=(struct Cyc_Absyn_Less_kb_struct*)
_tmpFE;if(_tmp110->tag != 2)goto _LL9E;else{_tmp111=_tmp110->f1;_tmp112=(struct Cyc_Core_Opt**)&
_tmp110->f1;_tmp113=_tmp110->f2;}}_LL9D:{struct Cyc_Core_Opt*_tmp34D;*_tmp112=((
_tmp34D=_cycalloc(sizeof(*_tmp34D)),((_tmp34D->v=Cyc_Tcutil_kind_to_bound(
_tmp113),_tmp34D))));}continue;_LL9E: {struct Cyc_Absyn_Eq_kb_struct*_tmp114=(
struct Cyc_Absyn_Eq_kb_struct*)_tmpFE;if(_tmp114->tag != 0)goto _LLA0;else{_tmp115=
_tmp114->f1;_tmp116=*_tmp115;_tmp117=_tmp116.kind;if(_tmp117 != Cyc_Absyn_MemKind)
goto _LLA0;_tmp118=_tmp116.aliasqual;}}_LL9F:{const char*_tmp355;void*_tmp354[3];
struct Cyc_String_pa_struct _tmp353;struct Cyc_String_pa_struct _tmp352;struct Cyc_Absyn_Kind*
_tmp351;struct Cyc_String_pa_struct _tmp350;(_tmp350.tag=0,((_tmp350.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((_tmp351=
_cycalloc_atomic(sizeof(*_tmp351)),((_tmp351->kind=Cyc_Absyn_MemKind,((_tmp351->aliasqual=
_tmp118,_tmp351)))))))),((_tmp352.tag=0,((_tmp352.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp353.tag=0,((
_tmp353.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp354[0]=&
_tmp353,((_tmp354[1]=& _tmp352,((_tmp354[2]=& _tmp350,Cyc_Tcutil_terr(loc,((
_tmp355="type %s attempts to abstract type variable %s of kind %s",
_tag_dyneither(_tmp355,sizeof(char),57))),_tag_dyneither(_tmp354,sizeof(void*),3)))))))))))))))))));}
continue;_LLA0:;_LLA1: continue;_LL95:;}}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*
f1;struct Cyc_Absyn_Aggrdecl***f2;};void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl*
ad);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct
Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl*ad){struct _dyneither_ptr*
_tmp127=(*ad->name).f2;{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;
atts=atts->tl){void*_tmp128=(void*)atts->hd;_LLA3: {struct Cyc_Absyn_Packed_att_struct*
_tmp129=(struct Cyc_Absyn_Packed_att_struct*)_tmp128;if(_tmp129->tag != 7)goto
_LLA5;}_LLA4: goto _LLA6;_LLA5: {struct Cyc_Absyn_Aligned_att_struct*_tmp12A=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp128;if(_tmp12A->tag != 6)goto _LLA7;}_LLA6:
continue;_LLA7:;_LLA8:{const char*_tmp35A;void*_tmp359[2];struct Cyc_String_pa_struct
_tmp358;struct Cyc_String_pa_struct _tmp357;(_tmp357.tag=0,((_tmp357.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp127),((_tmp358.tag=0,((_tmp358.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp359[0]=& _tmp358,((_tmp359[1]=& _tmp357,Cyc_Tcutil_terr(loc,((
_tmp35A="bad attribute %s in  %s definition",_tag_dyneither(_tmp35A,sizeof(char),
35))),_tag_dyneither(_tmp359,sizeof(void*),2)))))))))))));}goto _LLA2;_LLA2:;}}{
struct Cyc_List_List*_tmp12F=ad->tvs;{union Cyc_Absyn_Nmspace _tmp130=(*ad->name).f1;
struct Cyc_List_List*_tmp131;struct Cyc_List_List*_tmp132;_LLAA: if((_tmp130.Rel_n).tag
!= 1)goto _LLAC;_tmp131=(struct Cyc_List_List*)(_tmp130.Rel_n).val;if(_tmp131 != 0)
goto _LLAC;_LLAB: goto _LLAD;_LLAC: if((_tmp130.Abs_n).tag != 2)goto _LLAE;_tmp132=(
struct Cyc_List_List*)(_tmp130.Abs_n).val;if(_tmp132 != 0)goto _LLAE;_LLAD: goto
_LLA9;_LLAE:;_LLAF:{const char*_tmp35E;void*_tmp35D[1];struct Cyc_String_pa_struct
_tmp35C;(_tmp35C.tag=0,((_tmp35C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(ad->name)),((_tmp35D[0]=& _tmp35C,Cyc_Tcutil_terr(loc,((
_tmp35E="qualified struct declarations are not implemented (%s)",_tag_dyneither(
_tmp35E,sizeof(char),55))),_tag_dyneither(_tmp35D,sizeof(void*),1)))))));}
return;_LLA9:;}(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns);Cyc_Tcutil_check_unique_tvars(
loc,ad->tvs);Cyc_Tcutil_add_tvar_identities(ad->tvs);{struct _tuple14 _tmp35F;
struct _tuple14 _tmp137=(_tmp35F.f1=ad->impl,((_tmp35F.f2=((struct Cyc_Absyn_Aggrdecl***(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,
_tmp127),_tmp35F)));struct Cyc_Absyn_AggrdeclImpl*_tmp138;struct Cyc_Absyn_Aggrdecl***
_tmp139;struct Cyc_Absyn_AggrdeclImpl*_tmp13A;struct Cyc_Absyn_AggrdeclImpl _tmp13B;
struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13D;struct Cyc_List_List*
_tmp13E;int _tmp13F;struct Cyc_Absyn_Aggrdecl***_tmp140;struct Cyc_Absyn_AggrdeclImpl*
_tmp141;struct Cyc_Absyn_AggrdeclImpl _tmp142;struct Cyc_List_List*_tmp143;struct
Cyc_List_List*_tmp144;struct Cyc_List_List*_tmp145;int _tmp146;struct Cyc_Absyn_Aggrdecl***
_tmp147;struct Cyc_Absyn_Aggrdecl**_tmp148;struct Cyc_Absyn_AggrdeclImpl*_tmp149;
struct Cyc_Absyn_Aggrdecl***_tmp14A;struct Cyc_Absyn_Aggrdecl**_tmp14B;_LLB1:
_tmp138=_tmp137.f1;if(_tmp138 != 0)goto _LLB3;_tmp139=_tmp137.f2;if(_tmp139 != 0)
goto _LLB3;_LLB2: Cyc_Tc_rule_out_memkind(loc,_tmp127,_tmp12F,0);{struct Cyc_Absyn_Aggrdecl**
_tmp360;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,
_tmp127,((_tmp360=_cycalloc(sizeof(*_tmp360)),((_tmp360[0]=ad,_tmp360)))));}goto
_LLB0;_LLB3: _tmp13A=_tmp137.f1;if(_tmp13A == 0)goto _LLB5;_tmp13B=*_tmp13A;_tmp13C=
_tmp13B.exist_vars;_tmp13D=_tmp13B.rgn_po;_tmp13E=_tmp13B.fields;_tmp13F=_tmp13B.tagged;
_tmp140=_tmp137.f2;if(_tmp140 != 0)goto _LLB5;_LLB4: {struct Cyc_Absyn_Aggrdecl*
_tmp363;struct Cyc_Absyn_Aggrdecl**_tmp362;struct Cyc_Absyn_Aggrdecl**_tmp14D=(
_tmp362=_cycalloc(sizeof(*_tmp362)),((_tmp362[0]=((_tmp363=_cycalloc(sizeof(*
_tmp363)),((_tmp363->kind=ad->kind,((_tmp363->sc=Cyc_Absyn_Extern,((_tmp363->name=
ad->name,((_tmp363->tvs=_tmp12F,((_tmp363->impl=0,((_tmp363->attributes=ad->attributes,
_tmp363)))))))))))))),_tmp362)));ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(
ge->aggrdecls,_tmp127,_tmp14D);Cyc_Tcutil_check_unique_tvars(loc,_tmp13C);Cyc_Tcutil_add_tvar_identities(
_tmp13C);if(_tmp13F  && ad->kind == Cyc_Absyn_StructA){const char*_tmp366;void*
_tmp365;(_tmp365=0,Cyc_Tcutil_terr(loc,((_tmp366="@tagged qualifier is only allowed on union declarations",
_tag_dyneither(_tmp366,sizeof(char),56))),_tag_dyneither(_tmp365,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmp12F,_tmp13C),_tmp13D,_tmp13E);Cyc_Tc_rule_out_memkind(
loc,_tmp127,_tmp12F,0);Cyc_Tc_rule_out_memkind(loc,_tmp127,_tmp13C,1);if(ad->kind
== Cyc_Absyn_UnionA  && !_tmp13F){struct Cyc_List_List*f=_tmp13E;for(0;f != 0;f=f->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)){const char*
_tmp36C;void*_tmp36B[3];struct Cyc_String_pa_struct _tmp36A;struct Cyc_String_pa_struct
_tmp369;struct Cyc_String_pa_struct _tmp368;(_tmp368.tag=0,((_tmp368.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)
f->hd)->type)),((_tmp369.tag=0,((_tmp369.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp127),((_tmp36A.tag=0,((_tmp36A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp36B[0]=&
_tmp36A,((_tmp36B[1]=& _tmp369,((_tmp36B[2]=& _tmp368,Cyc_Tcutil_warn(loc,((
_tmp36C="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp36C,sizeof(char),98))),_tag_dyneither(_tmp36B,sizeof(void*),3)))))))))))))))))));}}}*
_tmp14D=ad;goto _LLB0;}_LLB5: _tmp141=_tmp137.f1;if(_tmp141 == 0)goto _LLB7;_tmp142=*
_tmp141;_tmp143=_tmp142.exist_vars;_tmp144=_tmp142.rgn_po;_tmp145=_tmp142.fields;
_tmp146=_tmp142.tagged;_tmp147=_tmp137.f2;if(_tmp147 == 0)goto _LLB7;_tmp148=*
_tmp147;_LLB6: if(ad->kind != (*_tmp148)->kind){const char*_tmp36F;void*_tmp36E;(
_tmp36E=0,Cyc_Tcutil_terr(loc,((_tmp36F="cannot reuse struct names for unions and vice-versa",
_tag_dyneither(_tmp36F,sizeof(char),52))),_tag_dyneither(_tmp36E,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*_tmp159=*_tmp148;{struct Cyc_Absyn_Aggrdecl*_tmp370;*
_tmp148=((_tmp370=_cycalloc(sizeof(*_tmp370)),((_tmp370->kind=ad->kind,((_tmp370->sc=
Cyc_Absyn_Extern,((_tmp370->name=ad->name,((_tmp370->tvs=_tmp12F,((_tmp370->impl=
0,((_tmp370->attributes=ad->attributes,_tmp370))))))))))))));}Cyc_Tcutil_check_unique_tvars(
loc,_tmp143);Cyc_Tcutil_add_tvar_identities(_tmp143);if(_tmp146  && ad->kind == 
Cyc_Absyn_StructA){const char*_tmp373;void*_tmp372;(_tmp372=0,Cyc_Tcutil_terr(loc,((
_tmp373="@tagged qualifier is only allowed on union declarations",_tag_dyneither(
_tmp373,sizeof(char),56))),_tag_dyneither(_tmp372,sizeof(void*),0)));}Cyc_Tc_tcAggrImpl(
te,ge,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp12F,_tmp143),_tmp144,_tmp145);*_tmp148=_tmp159;_tmp14B=
_tmp148;goto _LLB8;};_LLB7: _tmp149=_tmp137.f1;if(_tmp149 != 0)goto _LLB0;_tmp14A=
_tmp137.f2;if(_tmp14A == 0)goto _LLB0;_tmp14B=*_tmp14A;_LLB8: {struct Cyc_Absyn_Aggrdecl*
_tmp15D=Cyc_Tcdecl_merge_aggrdecl(*_tmp14B,ad,loc,Cyc_Tc_tc_msg);if(_tmp15D == 0)
return;else{Cyc_Tc_rule_out_memkind(loc,_tmp127,_tmp12F,0);if(ad->impl != 0)Cyc_Tc_rule_out_memkind(
loc,_tmp127,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);*
_tmp14B=(struct Cyc_Absyn_Aggrdecl*)_tmp15D;ad=(struct Cyc_Absyn_Aggrdecl*)_tmp15D;
goto _LLB0;}}_LLB0:;}{struct Cyc_Tcenv_AggrRes_struct*_tmp379;struct Cyc_Tcenv_AggrRes_struct
_tmp378;struct _tuple12*_tmp377;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,
_tmp127,(struct _tuple12*)((_tmp377=_cycalloc(sizeof(*_tmp377)),((_tmp377->f1=(
void*)((_tmp379=_cycalloc(sizeof(*_tmp379)),((_tmp379[0]=((_tmp378.tag=1,((
_tmp378.f1=ad,_tmp378)))),_tmp379)))),((_tmp377->f2=1,_tmp377)))))));};};}static
void Cyc_Tc_rule_out_mem_and_unique(struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*v,struct Cyc_List_List*tvs);static void Cyc_Tc_rule_out_mem_and_unique(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){void*_tmp161=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp163;struct Cyc_Core_Opt**
_tmp164;struct Cyc_Core_Opt*_tmp166;struct Cyc_Core_Opt**_tmp167;struct Cyc_Absyn_Kind*
_tmp168;struct Cyc_Absyn_Kind _tmp169;enum Cyc_Absyn_KindQual _tmp16A;enum Cyc_Absyn_AliasQual
_tmp16B;struct Cyc_Core_Opt*_tmp16D;struct Cyc_Core_Opt**_tmp16E;struct Cyc_Absyn_Kind*
_tmp16F;struct Cyc_Absyn_Kind _tmp170;enum Cyc_Absyn_KindQual _tmp171;enum Cyc_Absyn_AliasQual
_tmp172;struct Cyc_Core_Opt*_tmp174;struct Cyc_Core_Opt**_tmp175;struct Cyc_Absyn_Kind*
_tmp176;struct Cyc_Absyn_Kind _tmp177;enum Cyc_Absyn_KindQual _tmp178;enum Cyc_Absyn_AliasQual
_tmp179;struct Cyc_Core_Opt*_tmp17B;struct Cyc_Core_Opt**_tmp17C;struct Cyc_Absyn_Kind*
_tmp17D;struct Cyc_Absyn_Kind _tmp17E;enum Cyc_Absyn_KindQual _tmp17F;enum Cyc_Absyn_AliasQual
_tmp180;struct Cyc_Core_Opt*_tmp182;struct Cyc_Core_Opt**_tmp183;struct Cyc_Absyn_Kind*
_tmp184;struct Cyc_Absyn_Kind _tmp185;enum Cyc_Absyn_KindQual _tmp186;enum Cyc_Absyn_AliasQual
_tmp187;struct Cyc_Absyn_Kind*_tmp189;struct Cyc_Absyn_Kind _tmp18A;enum Cyc_Absyn_KindQual
_tmp18B;enum Cyc_Absyn_AliasQual _tmp18C;struct Cyc_Core_Opt*_tmp18E;struct Cyc_Core_Opt**
_tmp18F;struct Cyc_Absyn_Kind*_tmp190;struct Cyc_Absyn_Kind _tmp191;enum Cyc_Absyn_KindQual
_tmp192;enum Cyc_Absyn_AliasQual _tmp193;struct Cyc_Absyn_Kind*_tmp195;struct Cyc_Absyn_Kind
_tmp196;enum Cyc_Absyn_KindQual _tmp197;enum Cyc_Absyn_AliasQual _tmp198;struct Cyc_Absyn_Kind*
_tmp19A;struct Cyc_Absyn_Kind _tmp19B;enum Cyc_Absyn_KindQual _tmp19C;enum Cyc_Absyn_AliasQual
_tmp19D;_LLBA: {struct Cyc_Absyn_Unknown_kb_struct*_tmp162=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp161;if(_tmp162->tag != 1)goto _LLBC;else{_tmp163=_tmp162->f1;_tmp164=(struct
Cyc_Core_Opt**)& _tmp162->f1;}}_LLBB: _tmp167=_tmp164;goto _LLBD;_LLBC: {struct Cyc_Absyn_Less_kb_struct*
_tmp165=(struct Cyc_Absyn_Less_kb_struct*)_tmp161;if(_tmp165->tag != 2)goto _LLBE;
else{_tmp166=_tmp165->f1;_tmp167=(struct Cyc_Core_Opt**)& _tmp165->f1;_tmp168=
_tmp165->f2;_tmp169=*_tmp168;_tmp16A=_tmp169.kind;if(_tmp16A != Cyc_Absyn_MemKind)
goto _LLBE;_tmp16B=_tmp169.aliasqual;if(_tmp16B != Cyc_Absyn_Top)goto _LLBE;}}_LLBD:
_tmp16E=_tmp167;goto _LLBF;_LLBE: {struct Cyc_Absyn_Less_kb_struct*_tmp16C=(struct
Cyc_Absyn_Less_kb_struct*)_tmp161;if(_tmp16C->tag != 2)goto _LLC0;else{_tmp16D=
_tmp16C->f1;_tmp16E=(struct Cyc_Core_Opt**)& _tmp16C->f1;_tmp16F=_tmp16C->f2;
_tmp170=*_tmp16F;_tmp171=_tmp170.kind;if(_tmp171 != Cyc_Absyn_MemKind)goto _LLC0;
_tmp172=_tmp170.aliasqual;if(_tmp172 != Cyc_Absyn_Aliasable)goto _LLC0;}}_LLBF:{
struct Cyc_Core_Opt*_tmp37A;*_tmp16E=((_tmp37A=_cycalloc(sizeof(*_tmp37A)),((
_tmp37A->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp37A))));}goto _LLB9;_LLC0: {
struct Cyc_Absyn_Less_kb_struct*_tmp173=(struct Cyc_Absyn_Less_kb_struct*)_tmp161;
if(_tmp173->tag != 2)goto _LLC2;else{_tmp174=_tmp173->f1;_tmp175=(struct Cyc_Core_Opt**)&
_tmp173->f1;_tmp176=_tmp173->f2;_tmp177=*_tmp176;_tmp178=_tmp177.kind;if(_tmp178
!= Cyc_Absyn_AnyKind)goto _LLC2;_tmp179=_tmp177.aliasqual;if(_tmp179 != Cyc_Absyn_Top)
goto _LLC2;}}_LLC1: _tmp17C=_tmp175;goto _LLC3;_LLC2: {struct Cyc_Absyn_Less_kb_struct*
_tmp17A=(struct Cyc_Absyn_Less_kb_struct*)_tmp161;if(_tmp17A->tag != 2)goto _LLC4;
else{_tmp17B=_tmp17A->f1;_tmp17C=(struct Cyc_Core_Opt**)& _tmp17A->f1;_tmp17D=
_tmp17A->f2;_tmp17E=*_tmp17D;_tmp17F=_tmp17E.kind;if(_tmp17F != Cyc_Absyn_AnyKind)
goto _LLC4;_tmp180=_tmp17E.aliasqual;if(_tmp180 != Cyc_Absyn_Aliasable)goto _LLC4;}}
_LLC3:{struct Cyc_Core_Opt*_tmp37B;*_tmp17C=((_tmp37B=_cycalloc(sizeof(*_tmp37B)),((
_tmp37B->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak),_tmp37B))));}goto _LLB9;_LLC4: {
struct Cyc_Absyn_Less_kb_struct*_tmp181=(struct Cyc_Absyn_Less_kb_struct*)_tmp161;
if(_tmp181->tag != 2)goto _LLC6;else{_tmp182=_tmp181->f1;_tmp183=(struct Cyc_Core_Opt**)&
_tmp181->f1;_tmp184=_tmp181->f2;_tmp185=*_tmp184;_tmp186=_tmp185.kind;if(_tmp186
!= Cyc_Absyn_RgnKind)goto _LLC6;_tmp187=_tmp185.aliasqual;if(_tmp187 != Cyc_Absyn_Top)
goto _LLC6;}}_LLC5:{struct Cyc_Core_Opt*_tmp37C;*_tmp183=((_tmp37C=_cycalloc(
sizeof(*_tmp37C)),((_tmp37C->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp37C))));}
goto _LLB9;_LLC6: {struct Cyc_Absyn_Eq_kb_struct*_tmp188=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp161;if(_tmp188->tag != 0)goto _LLC8;else{_tmp189=_tmp188->f1;_tmp18A=*_tmp189;
_tmp18B=_tmp18A.kind;if(_tmp18B != Cyc_Absyn_RgnKind)goto _LLC8;_tmp18C=_tmp18A.aliasqual;
if(_tmp18C != Cyc_Absyn_Top)goto _LLC8;}}_LLC7:{const char*_tmp381;void*_tmp380[2];
struct Cyc_String_pa_struct _tmp37F;struct Cyc_String_pa_struct _tmp37E;(_tmp37E.tag=
0,((_tmp37E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
tvs2->hd)->name),((_tmp37F.tag=0,((_tmp37F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*v),((_tmp380[0]=& _tmp37F,((_tmp380[1]=& _tmp37E,Cyc_Tcutil_terr(
loc,((_tmp381="type %s attempts to abstract type variable %s of kind TR",
_tag_dyneither(_tmp381,sizeof(char),57))),_tag_dyneither(_tmp380,sizeof(void*),2)))))))))))));}
goto _LLB9;_LLC8: {struct Cyc_Absyn_Less_kb_struct*_tmp18D=(struct Cyc_Absyn_Less_kb_struct*)
_tmp161;if(_tmp18D->tag != 2)goto _LLCA;else{_tmp18E=_tmp18D->f1;_tmp18F=(struct
Cyc_Core_Opt**)& _tmp18D->f1;_tmp190=_tmp18D->f2;_tmp191=*_tmp190;_tmp192=_tmp191.kind;
_tmp193=_tmp191.aliasqual;if(_tmp193 != Cyc_Absyn_Unique)goto _LLCA;}}_LLC9:
_tmp197=_tmp192;goto _LLCB;_LLCA: {struct Cyc_Absyn_Eq_kb_struct*_tmp194=(struct
Cyc_Absyn_Eq_kb_struct*)_tmp161;if(_tmp194->tag != 0)goto _LLCC;else{_tmp195=
_tmp194->f1;_tmp196=*_tmp195;_tmp197=_tmp196.kind;_tmp198=_tmp196.aliasqual;if(
_tmp198 != Cyc_Absyn_Unique)goto _LLCC;}}_LLCB:{const char*_tmp389;void*_tmp388[3];
struct Cyc_String_pa_struct _tmp387;struct Cyc_String_pa_struct _tmp386;struct Cyc_Absyn_Kind*
_tmp385;struct Cyc_String_pa_struct _tmp384;(_tmp384.tag=0,((_tmp384.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((_tmp385=
_cycalloc_atomic(sizeof(*_tmp385)),((_tmp385->kind=_tmp197,((_tmp385->aliasqual=
Cyc_Absyn_Unique,_tmp385)))))))),((_tmp386.tag=0,((_tmp386.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((
_tmp387.tag=0,((_tmp387.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((
_tmp388[0]=& _tmp387,((_tmp388[1]=& _tmp386,((_tmp388[2]=& _tmp384,Cyc_Tcutil_terr(
loc,((_tmp389="type %s attempts to abstract type variable %s of kind %s",
_tag_dyneither(_tmp389,sizeof(char),57))),_tag_dyneither(_tmp388,sizeof(void*),3)))))))))))))))))));}
goto _LLB9;_LLCC: {struct Cyc_Absyn_Eq_kb_struct*_tmp199=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp161;if(_tmp199->tag != 0)goto _LLCE;else{_tmp19A=_tmp199->f1;_tmp19B=*_tmp19A;
_tmp19C=_tmp19B.kind;if(_tmp19C != Cyc_Absyn_MemKind)goto _LLCE;_tmp19D=_tmp19B.aliasqual;}}
_LLCD:{const char*_tmp391;void*_tmp390[3];struct Cyc_String_pa_struct _tmp38F;
struct Cyc_String_pa_struct _tmp38E;struct Cyc_Absyn_Kind*_tmp38D;struct Cyc_String_pa_struct
_tmp38C;(_tmp38C.tag=0,((_tmp38C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp38D=_cycalloc_atomic(sizeof(*_tmp38D)),((_tmp38D->kind=
Cyc_Absyn_MemKind,((_tmp38D->aliasqual=_tmp19D,_tmp38D)))))))),((_tmp38E.tag=0,((
_tmp38E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
tvs2->hd)->name),((_tmp38F.tag=0,((_tmp38F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*v),((_tmp390[0]=& _tmp38F,((_tmp390[1]=& _tmp38E,((_tmp390[2]=&
_tmp38C,Cyc_Tcutil_terr(loc,((_tmp391="type %s attempts to abstract type variable %s of kind %s",
_tag_dyneither(_tmp391,sizeof(char),57))),_tag_dyneither(_tmp390,sizeof(void*),3)))))))))))))))))));}
goto _LLB9;_LLCE:;_LLCF: goto _LLB9;_LLB9:;}}struct _tuple15{struct Cyc_Absyn_Tqual f1;
void*f2;};static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct _dyneither_ptr
obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*
tvs,struct Cyc_Absyn_Datatypedecl*tudres);static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*
fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){{struct Cyc_List_List*
_tmp1B1=fields;for(0;_tmp1B1 != 0;_tmp1B1=_tmp1B1->tl){struct Cyc_Absyn_Datatypefield*
_tmp1B2=(struct Cyc_Absyn_Datatypefield*)_tmp1B1->hd;{struct Cyc_List_List*typs=
_tmp1B2->typs;for(0;typs != 0;typs=typs->tl){Cyc_Tcutil_check_type(_tmp1B2->loc,
te,tvs,& Cyc_Tcutil_tmk,0,(*((struct _tuple15*)typs->hd)).f2);{struct _RegionHandle*
_tmp1B3=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1B3,(*((
struct _tuple15*)typs->hd)).f2)){const char*_tmp395;void*_tmp394[1];struct Cyc_String_pa_struct
_tmp393;(_tmp393.tag=0,((_tmp393.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp1B2->name)),((_tmp394[0]=& _tmp393,Cyc_Tcutil_terr(
_tmp1B2->loc,((_tmp395="noalias pointers in datatypes are not allowed (%s)",
_tag_dyneither(_tmp395,sizeof(char),51))),_tag_dyneither(_tmp394,sizeof(void*),1)))))));}((*((
struct _tuple15*)typs->hd)).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
_tmp1B2->loc,((*((struct _tuple15*)typs->hd)).f1).print_const,(*((struct _tuple15*)
typs->hd)).f2);};}}{union Cyc_Absyn_Nmspace _tmp1B7=(*_tmp1B2->name).f1;struct Cyc_List_List*
_tmp1B8;struct Cyc_List_List*_tmp1B9;struct Cyc_List_List*_tmp1BA;int _tmp1BB;_LLD1:
if((_tmp1B7.Rel_n).tag != 1)goto _LLD3;_tmp1B8=(struct Cyc_List_List*)(_tmp1B7.Rel_n).val;
if(_tmp1B8 != 0)goto _LLD3;_LLD2: if(is_extensible)(*_tmp1B2->name).f1=Cyc_Absyn_Abs_n(
te->ns);else{(*_tmp1B2->name).f1=(*name).f1;}goto _LLD0;_LLD3: if((_tmp1B7.Rel_n).tag
!= 1)goto _LLD5;_tmp1B9=(struct Cyc_List_List*)(_tmp1B7.Rel_n).val;_LLD4:{const
char*_tmp399;void*_tmp398[1];struct Cyc_String_pa_struct _tmp397;(_tmp397.tag=0,((
_tmp397.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp1B2->name)),((_tmp398[0]=& _tmp397,Cyc_Tcutil_terr(_tmp1B2->loc,((_tmp399="qualified datatypefield declarations are not allowed (%s)",
_tag_dyneither(_tmp399,sizeof(char),58))),_tag_dyneither(_tmp398,sizeof(void*),1)))))));}
goto _LLD0;_LLD5: if((_tmp1B7.Abs_n).tag != 2)goto _LLD7;_tmp1BA=(struct Cyc_List_List*)(
_tmp1B7.Abs_n).val;_LLD6: goto _LLD0;_LLD7: if((_tmp1B7.Loc_n).tag != 3)goto _LLD0;
_tmp1BB=(int)(_tmp1B7.Loc_n).val;_LLD8: {const char*_tmp39C;void*_tmp39B;(_tmp39B=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp39C="tcDatatypeFields: Loc_n",_tag_dyneither(_tmp39C,sizeof(char),24))),
_tag_dyneither(_tmp39B,sizeof(void*),0)));}_LLD0:;};}}{struct Cyc_List_List*
fields2;if(is_extensible){int _tmp1C1=1;struct Cyc_List_List*_tmp1C2=Cyc_Tcdecl_sort_xdatatype_fields(
fields,& _tmp1C1,(*name).f2,loc,Cyc_Tc_tc_msg);if(_tmp1C1)fields2=_tmp1C2;else{
fields2=0;}}else{struct _RegionHandle _tmp1C3=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp1C3;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Datatypefield*
_tmp1C4=(struct Cyc_Absyn_Datatypefield*)fs->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1C4->name).f2)){
const char*_tmp3A1;void*_tmp3A0[2];struct Cyc_String_pa_struct _tmp39F;struct Cyc_String_pa_struct
_tmp39E;(_tmp39E.tag=0,((_tmp39E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
obj),((_tmp39F.tag=0,((_tmp39F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp1C4->name).f2),((_tmp3A0[0]=& _tmp39F,((_tmp3A0[1]=& _tmp39E,Cyc_Tcutil_terr(
_tmp1C4->loc,((_tmp3A1="duplicate field name %s in %s",_tag_dyneither(_tmp3A1,
sizeof(char),30))),_tag_dyneither(_tmp3A0,sizeof(void*),2)))))))))))));}else{
struct Cyc_List_List*_tmp3A2;prev_fields=((_tmp3A2=_region_malloc(uprev_rgn,
sizeof(*_tmp3A2)),((_tmp3A2->hd=(*_tmp1C4->name).f2,((_tmp3A2->tl=prev_fields,
_tmp3A2))))));}if(_tmp1C4->sc != Cyc_Absyn_Public){{const char*_tmp3A6;void*
_tmp3A5[1];struct Cyc_String_pa_struct _tmp3A4;(_tmp3A4.tag=0,((_tmp3A4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1C4->name).f2),((_tmp3A5[0]=&
_tmp3A4,Cyc_Tcutil_warn(loc,((_tmp3A6="ignoring scope of field %s",
_tag_dyneither(_tmp3A6,sizeof(char),27))),_tag_dyneither(_tmp3A5,sizeof(void*),1)))))));}
_tmp1C4->sc=Cyc_Absyn_Public;}}}fields2=fields;};_pop_region(uprev_rgn);}{struct
Cyc_List_List*_tmp1CD=fields;for(0;_tmp1CD != 0;_tmp1CD=_tmp1CD->tl){struct Cyc_Absyn_Datatypefield*
_tmp1CE=(struct Cyc_Absyn_Datatypefield*)_tmp1CD->hd;struct Cyc_Tcenv_DatatypeRes_struct*
_tmp3AC;struct Cyc_Tcenv_DatatypeRes_struct _tmp3AB;struct _tuple12*_tmp3AA;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple12*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1CE->name).f2,(struct _tuple12*)((
_tmp3AA=_cycalloc(sizeof(*_tmp3AA)),((_tmp3AA->f1=(void*)((_tmp3AC=_cycalloc(
sizeof(*_tmp3AC)),((_tmp3AC[0]=((_tmp3AB.tag=2,((_tmp3AB.f1=tudres,((_tmp3AB.f2=
_tmp1CE,_tmp3AB)))))),_tmp3AC)))),((_tmp3AA->f2=1,_tmp3AA)))))));}}return fields2;};}
struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};void Cyc_Tc_tcDatatypedecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Datatypedecl*tud);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Datatypedecl*
tud){struct _dyneither_ptr*v=(*tud->name).f2;const char*_tmp3AE;const char*_tmp3AD;
struct _dyneither_ptr obj=tud->is_extensible?(_tmp3AE="@extensible datatype",
_tag_dyneither(_tmp3AE,sizeof(char),21)):((_tmp3AD="datatype",_tag_dyneither(
_tmp3AD,sizeof(char),9)));struct Cyc_List_List*tvs=tud->tvs;Cyc_Tcutil_check_unique_tvars(
loc,tvs);Cyc_Tcutil_add_tvar_identities(tvs);{struct _RegionHandle _tmp1D2=
_new_region("temp");struct _RegionHandle*temp=& _tmp1D2;_push_region(temp);{struct
Cyc_Absyn_Datatypedecl***tud_opt;{struct _handler_cons _tmp1D3;_push_handler(&
_tmp1D3);{int _tmp1D5=0;if(setjmp(_tmp1D3.handler))_tmp1D5=1;if(!_tmp1D5){tud_opt=
Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);if(tud_opt == 0  && !tud->is_extensible){
struct Cyc_Dict_Absent_struct _tmp3B1;struct Cyc_Dict_Absent_struct*_tmp3B0;(int)
_throw((void*)((_tmp3B0=_cycalloc_atomic(sizeof(*_tmp3B0)),((_tmp3B0[0]=((
_tmp3B1.tag=Cyc_Dict_Absent,_tmp3B1)),_tmp3B0)))));}if(tud_opt != 0)tud->name=(*(*
tud_opt))->name;else{(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns);};_pop_handler();}
else{void*_tmp1D4=(void*)_exn_thrown;void*_tmp1D9=_tmp1D4;_LLDA: {struct Cyc_Dict_Absent_struct*
_tmp1DA=(struct Cyc_Dict_Absent_struct*)_tmp1D9;if(_tmp1DA->tag != Cyc_Dict_Absent)
goto _LLDC;}_LLDB:{union Cyc_Absyn_Nmspace _tmp1DB=(*tud->name).f1;struct Cyc_List_List*
_tmp1DC;struct Cyc_List_List*_tmp1DD;_LLDF: if((_tmp1DB.Rel_n).tag != 1)goto _LLE1;
_tmp1DC=(struct Cyc_List_List*)(_tmp1DB.Rel_n).val;if(_tmp1DC != 0)goto _LLE1;_LLE0:(*
tud->name).f1=Cyc_Absyn_Abs_n(te->ns);goto _LLDE;_LLE1: if((_tmp1DB.Abs_n).tag != 2)
goto _LLE3;_tmp1DD=(struct Cyc_List_List*)(_tmp1DB.Abs_n).val;_LLE2: goto _LLE4;
_LLE3:;_LLE4:{const char*_tmp3B5;void*_tmp3B4[1];struct Cyc_String_pa_struct
_tmp3B3;(_tmp3B3.tag=0,((_tmp3B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tud->name)),((_tmp3B4[0]=& _tmp3B3,Cyc_Tcutil_terr(loc,((
_tmp3B5="qualified (non-extensible) datatype declarations are not implemented (%s)",
_tag_dyneither(_tmp3B5,sizeof(char),74))),_tag_dyneither(_tmp3B4,sizeof(void*),1)))))));}
_npop_handler(0);return;_LLDE:;}{struct Cyc_Absyn_Datatypedecl***_tmp1E1=((struct
Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
ge->datatypedecls,v);{struct Cyc_Absyn_Datatypedecl***_tmp3B6;tud_opt=(
unsigned int)_tmp1E1?(_tmp3B6=_region_malloc(temp,sizeof(*_tmp3B6)),((_tmp3B6[0]=*
_tmp1E1,_tmp3B6))): 0;}goto _LLD9;};_LLDC:;_LLDD:(void)_throw(_tmp1D9);_LLD9:;}};}{
struct _tuple16 _tmp3B7;struct _tuple16 _tmp1E4=(_tmp3B7.f1=tud->fields,((_tmp3B7.f2=
tud_opt,_tmp3B7)));struct Cyc_Core_Opt*_tmp1E5;struct Cyc_Absyn_Datatypedecl***
_tmp1E6;struct Cyc_Core_Opt*_tmp1E7;struct Cyc_Core_Opt _tmp1E8;struct Cyc_List_List*
_tmp1E9;struct Cyc_List_List**_tmp1EA;struct Cyc_Absyn_Datatypedecl***_tmp1EB;
struct Cyc_Core_Opt*_tmp1EC;struct Cyc_Core_Opt _tmp1ED;struct Cyc_List_List*_tmp1EE;
struct Cyc_List_List**_tmp1EF;struct Cyc_Absyn_Datatypedecl***_tmp1F0;struct Cyc_Absyn_Datatypedecl**
_tmp1F1;struct Cyc_Core_Opt*_tmp1F2;struct Cyc_Absyn_Datatypedecl***_tmp1F3;struct
Cyc_Absyn_Datatypedecl**_tmp1F4;_LLE6: _tmp1E5=_tmp1E4.f1;if(_tmp1E5 != 0)goto
_LLE8;_tmp1E6=_tmp1E4.f2;if(_tmp1E6 != 0)goto _LLE8;_LLE7: Cyc_Tc_rule_out_mem_and_unique(
loc,v,tvs);{struct Cyc_Absyn_Datatypedecl**_tmp3B8;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(
ge->datatypedecls,v,((_tmp3B8=_cycalloc(sizeof(*_tmp3B8)),((_tmp3B8[0]=tud,
_tmp3B8)))));}goto _LLE5;_LLE8: _tmp1E7=_tmp1E4.f1;if(_tmp1E7 == 0)goto _LLEA;
_tmp1E8=*_tmp1E7;_tmp1E9=(struct Cyc_List_List*)_tmp1E8.v;_tmp1EA=(struct Cyc_List_List**)&(*
_tmp1E4.f1).v;_tmp1EB=_tmp1E4.f2;if(_tmp1EB != 0)goto _LLEA;_LLE9: {struct Cyc_Absyn_Datatypedecl*
_tmp3BB;struct Cyc_Absyn_Datatypedecl**_tmp3BA;struct Cyc_Absyn_Datatypedecl**
_tmp1F6=(_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA[0]=((_tmp3BB=_cycalloc(
sizeof(*_tmp3BB)),((_tmp3BB->sc=Cyc_Absyn_Extern,((_tmp3BB->name=tud->name,((
_tmp3BB->tvs=tvs,((_tmp3BB->fields=0,((_tmp3BB->is_extensible=tud->is_extensible,
_tmp3BB)))))))))))),_tmp3BA)));ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(
ge->datatypedecls,v,_tmp1F6);*_tmp1EA=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,
tud->name,*_tmp1EA,tvs,tud);Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);*_tmp1F6=
tud;goto _LLE5;}_LLEA: _tmp1EC=_tmp1E4.f1;if(_tmp1EC == 0)goto _LLEC;_tmp1ED=*
_tmp1EC;_tmp1EE=(struct Cyc_List_List*)_tmp1ED.v;_tmp1EF=(struct Cyc_List_List**)&(*
_tmp1E4.f1).v;_tmp1F0=_tmp1E4.f2;if(_tmp1F0 == 0)goto _LLEC;_tmp1F1=*_tmp1F0;_LLEB: {
struct Cyc_Absyn_Datatypedecl*_tmp1F9=*_tmp1F1;if((!tud->is_extensible  && (
unsigned int)_tmp1F9) && _tmp1F9->is_extensible)tud->is_extensible=1;{struct Cyc_Absyn_Datatypedecl*
_tmp3BC;*_tmp1F1=((_tmp3BC=_cycalloc(sizeof(*_tmp3BC)),((_tmp3BC->sc=Cyc_Absyn_Extern,((
_tmp3BC->name=tud->name,((_tmp3BC->tvs=tvs,((_tmp3BC->fields=0,((_tmp3BC->is_extensible=
tud->is_extensible,_tmp3BC))))))))))));}*_tmp1EF=Cyc_Tc_tcDatatypeFields(te,ge,
loc,obj,tud->is_extensible,tud->name,*_tmp1EF,tvs,tud);*_tmp1F1=_tmp1F9;_tmp1F4=
_tmp1F1;goto _LLED;}_LLEC: _tmp1F2=_tmp1E4.f1;if(_tmp1F2 != 0)goto _LLE5;_tmp1F3=
_tmp1E4.f2;if(_tmp1F3 == 0)goto _LLE5;_tmp1F4=*_tmp1F3;_LLED: {struct Cyc_Absyn_Datatypedecl*
_tmp1FB=Cyc_Tcdecl_merge_datatypedecl(*_tmp1F4,tud,loc,Cyc_Tc_tc_msg);Cyc_Tc_rule_out_mem_and_unique(
loc,v,tvs);if(_tmp1FB == 0){_npop_handler(0);return;}else{*_tmp1F4=(struct Cyc_Absyn_Datatypedecl*)
_tmp1FB;goto _LLE5;}}_LLE5:;};};_pop_region(temp);};}void Cyc_Tc_tcEnumdecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct
Cyc_Absyn_Enumdecl*ed);void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl*ed){struct
_dyneither_ptr*v=(*ed->name).f2;{union Cyc_Absyn_Nmspace _tmp1FE=(*ed->name).f1;
struct Cyc_List_List*_tmp1FF;struct Cyc_List_List*_tmp200;_LLEF: if((_tmp1FE.Rel_n).tag
!= 1)goto _LLF1;_tmp1FF=(struct Cyc_List_List*)(_tmp1FE.Rel_n).val;if(_tmp1FF != 0)
goto _LLF1;_LLF0: goto _LLF2;_LLF1: if((_tmp1FE.Abs_n).tag != 2)goto _LLF3;_tmp200=(
struct Cyc_List_List*)(_tmp1FE.Abs_n).val;if(_tmp200 != 0)goto _LLF3;_LLF2: goto
_LLEE;_LLF3:;_LLF4:{const char*_tmp3C0;void*_tmp3BF[1];struct Cyc_String_pa_struct
_tmp3BE;(_tmp3BE.tag=0,((_tmp3BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(ed->name)),((_tmp3BF[0]=& _tmp3BE,Cyc_Tcutil_terr(loc,((
_tmp3C0="qualified enum declarations are not implemented (%s)",_tag_dyneither(
_tmp3C0,sizeof(char),53))),_tag_dyneither(_tmp3BF,sizeof(void*),1)))))));}
return;_LLEE:;}(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns);if(ed->fields != 0){struct
_RegionHandle _tmp204=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=&
_tmp204;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int
tag_count=0;struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp205=(struct Cyc_Absyn_Enumfield*)fs->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp205->name).f2)){
const char*_tmp3C4;void*_tmp3C3[1];struct Cyc_String_pa_struct _tmp3C2;(_tmp3C2.tag=
0,((_tmp3C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp205->name).f2),((
_tmp3C3[0]=& _tmp3C2,Cyc_Tcutil_terr(_tmp205->loc,((_tmp3C4="duplicate field name %s",
_tag_dyneither(_tmp3C4,sizeof(char),24))),_tag_dyneither(_tmp3C3,sizeof(void*),1)))))));}
else{struct Cyc_List_List*_tmp3C5;prev_fields=((_tmp3C5=_region_malloc(uprev_rgn,
sizeof(*_tmp3C5)),((_tmp3C5->hd=(*_tmp205->name).f2,((_tmp3C5->tl=prev_fields,
_tmp3C5))))));}if(_tmp205->tag == 0)_tmp205->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp205->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp205->tag))){const char*_tmp3CA;void*_tmp3C9[2];struct Cyc_String_pa_struct
_tmp3C8;struct Cyc_String_pa_struct _tmp3C7;(_tmp3C7.tag=0,((_tmp3C7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp205->name).f2),((_tmp3C8.tag=0,((
_tmp3C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3C9[0]=&
_tmp3C8,((_tmp3C9[1]=& _tmp3C7,Cyc_Tcutil_terr(loc,((_tmp3CA="enum %s, field %s: expression is not constant",
_tag_dyneither(_tmp3CA,sizeof(char),46))),_tag_dyneither(_tmp3C9,sizeof(void*),2)))))))))))));}}{
unsigned int _tmp20F;int _tmp210;struct _tuple10 _tmp20E=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp205->tag));_tmp20F=_tmp20E.f1;_tmp210=
_tmp20E.f2;if(!_tmp210){const char*_tmp3CD;void*_tmp3CC;(_tmp3CC=0,Cyc_Tcutil_terr(
loc,((_tmp3CD="Cyclone enum tags cannot use sizeof or offsetof",_tag_dyneither(
_tmp3CD,sizeof(char),48))),_tag_dyneither(_tmp3CC,sizeof(void*),0)));}tag_count=
_tmp20F + 1;(*_tmp205->name).f1=Cyc_Absyn_Abs_n(te->ns);};}};_pop_region(
uprev_rgn);}{struct _handler_cons _tmp213;_push_handler(& _tmp213);{int _tmp215=0;
if(setjmp(_tmp213.handler))_tmp215=1;if(!_tmp215){{struct Cyc_Absyn_Enumdecl**
_tmp216=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);struct Cyc_Absyn_Enumdecl*
_tmp217=Cyc_Tcdecl_merge_enumdecl(*_tmp216,ed,loc,Cyc_Tc_tc_msg);if(_tmp217 == 0){
_npop_handler(0);return;}*_tmp216=(struct Cyc_Absyn_Enumdecl*)_tmp217;};
_pop_handler();}else{void*_tmp214=(void*)_exn_thrown;void*_tmp219=_tmp214;_LLF6: {
struct Cyc_Dict_Absent_struct*_tmp21A=(struct Cyc_Dict_Absent_struct*)_tmp219;if(
_tmp21A->tag != Cyc_Dict_Absent)goto _LLF8;}_LLF7: {struct Cyc_Absyn_Enumdecl**
_tmp3CE;struct Cyc_Absyn_Enumdecl**_tmp21B=(_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((
_tmp3CE[0]=ed,_tmp3CE)));ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,
v,_tmp21B);goto _LLF5;}_LLF8:;_LLF9:(void)_throw(_tmp219);_LLF5:;}};}if(ed->fields
!= 0){struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp21D=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Tcenv_EnumRes_struct*
_tmp3D4;struct Cyc_Tcenv_EnumRes_struct _tmp3D3;struct _tuple12*_tmp3D2;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple12*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp21D->name).f2,(struct _tuple12*)((
_tmp3D2=_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2->f1=(void*)((_tmp3D4=_cycalloc(
sizeof(*_tmp3D4)),((_tmp3D4[0]=((_tmp3D3.tag=3,((_tmp3D3.f1=ed,((_tmp3D3.f2=
_tmp21D,_tmp3D3)))))),_tmp3D4)))),((_tmp3D2->f2=1,_tmp3D2)))))));}}}static int Cyc_Tc_okay_externC(
struct Cyc_Position_Segment*loc,enum Cyc_Absyn_Scope sc);static int Cyc_Tc_okay_externC(
struct Cyc_Position_Segment*loc,enum Cyc_Absyn_Scope sc){switch(sc){case Cyc_Absyn_Static:
_LLFA:{const char*_tmp3D7;void*_tmp3D6;(_tmp3D6=0,Cyc_Tcutil_warn(loc,((_tmp3D7="static declaration nested within extern \"C\"",
_tag_dyneither(_tmp3D7,sizeof(char),44))),_tag_dyneither(_tmp3D6,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Abstract: _LLFB:{const char*_tmp3DA;void*_tmp3D9;(_tmp3D9=0,
Cyc_Tcutil_warn(loc,((_tmp3DA="abstract declaration nested within extern \"C\"",
_tag_dyneither(_tmp3DA,sizeof(char),46))),_tag_dyneither(_tmp3D9,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Public: _LLFC: goto _LLFD;case Cyc_Absyn_Register: _LLFD: goto
_LLFE;case Cyc_Absyn_Extern: _LLFE: return 1;case Cyc_Absyn_ExternC: _LLFF:{const char*
_tmp3DD;void*_tmp3DC;(_tmp3DC=0,Cyc_Tcutil_warn(loc,((_tmp3DD="nested extern \"C\" declaration",
_tag_dyneither(_tmp3DD,sizeof(char),30))),_tag_dyneither(_tmp3DC,sizeof(void*),0)));}
return 1;}}static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,
struct _tuple11*exp);static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*
te,struct _tuple11*exp){struct Cyc_Position_Segment*_tmp228;struct _tuple0*_tmp229;
struct _tuple11 _tmp227=*exp;_tmp228=_tmp227.f1;_tmp229=_tmp227.f2;{struct _tuple0
_tmp22B;union Cyc_Absyn_Nmspace _tmp22C;struct _dyneither_ptr*_tmp22D;struct _tuple0*
_tmp22A=_tmp229;_tmp22B=*_tmp22A;_tmp22C=_tmp22B.f1;_tmp22D=_tmp22B.f2;{union Cyc_Absyn_Nmspace
_tmp22E=_tmp22C;struct Cyc_List_List*_tmp22F;struct Cyc_List_List*_tmp230;_LL102:
if((_tmp22E.Rel_n).tag != 1)goto _LL104;_tmp22F=(struct Cyc_List_List*)(_tmp22E.Rel_n).val;
if(_tmp22F != 0)goto _LL104;_LL103: goto _LL105;_LL104: if((_tmp22E.Abs_n).tag != 2)
goto _LL106;_tmp230=(struct Cyc_List_List*)(_tmp22E.Abs_n).val;if(_tmp230 != 0)goto
_LL106;_LL105: goto _LL101;_LL106:;_LL107:{const char*_tmp3E1;void*_tmp3E0[1];
struct Cyc_String_pa_struct _tmp3DF;(_tmp3DF.tag=0,((_tmp3DF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp229)),((
_tmp3E0[0]=& _tmp3DF,Cyc_Tcutil_terr(_tmp228,((_tmp3E1="qualified export variables are not implemented (%s)",
_tag_dyneither(_tmp3E1,sizeof(char),52))),_tag_dyneither(_tmp3E0,sizeof(void*),1)))))));}
return;_LL101:;}(*_tmp229).f1=Cyc_Absyn_Abs_n(te->ns);};}static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int
in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**
exports);static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
ds0,int in_externC,int in_externCinclude,int check_var_init,struct _RegionHandle*
grgn,struct Cyc_List_List**exports){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;struct Cyc_List_List*_tmp234=ds0;for(0;_tmp234 != 0;(
last=_tmp234,_tmp234=_tmp234->tl)){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
_tmp234->hd;struct Cyc_Position_Segment*loc=d->loc;void*_tmp235=d->r;struct Cyc_Absyn_Vardecl*
_tmp23B;struct Cyc_Absyn_Fndecl*_tmp23D;struct Cyc_Absyn_Typedefdecl*_tmp23F;
struct Cyc_Absyn_Aggrdecl*_tmp241;struct Cyc_Absyn_Datatypedecl*_tmp243;struct Cyc_Absyn_Enumdecl*
_tmp245;struct _dyneither_ptr*_tmp249;struct Cyc_List_List*_tmp24A;struct _tuple0*
_tmp24C;struct _tuple0 _tmp24D;union Cyc_Absyn_Nmspace _tmp24E;struct _dyneither_ptr*
_tmp24F;struct Cyc_List_List*_tmp250;struct Cyc_List_List*_tmp252;struct Cyc_List_List*
_tmp254;struct Cyc_List_List*_tmp255;_LL109: {struct Cyc_Absyn_Let_d_struct*
_tmp236=(struct Cyc_Absyn_Let_d_struct*)_tmp235;if(_tmp236->tag != 2)goto _LL10B;}
_LL10A: goto _LL10C;_LL10B: {struct Cyc_Absyn_Letv_d_struct*_tmp237=(struct Cyc_Absyn_Letv_d_struct*)
_tmp235;if(_tmp237->tag != 3)goto _LL10D;}_LL10C:{const char*_tmp3E4;void*_tmp3E3;(
_tmp3E3=0,Cyc_Tcutil_terr(loc,((_tmp3E4="top level let-declarations are not implemented",
_tag_dyneither(_tmp3E4,sizeof(char),47))),_tag_dyneither(_tmp3E3,sizeof(void*),0)));}
goto _LL108;_LL10D: {struct Cyc_Absyn_Region_d_struct*_tmp238=(struct Cyc_Absyn_Region_d_struct*)
_tmp235;if(_tmp238->tag != 4)goto _LL10F;}_LL10E:{const char*_tmp3E7;void*_tmp3E6;(
_tmp3E6=0,Cyc_Tcutil_terr(loc,((_tmp3E7="top level region declarations are not implemented",
_tag_dyneither(_tmp3E7,sizeof(char),50))),_tag_dyneither(_tmp3E6,sizeof(void*),0)));}
goto _LL108;_LL10F: {struct Cyc_Absyn_Alias_d_struct*_tmp239=(struct Cyc_Absyn_Alias_d_struct*)
_tmp235;if(_tmp239->tag != 5)goto _LL111;}_LL110:{const char*_tmp3EA;void*_tmp3E9;(
_tmp3E9=0,Cyc_Tcutil_terr(loc,((_tmp3EA="top level alias declarations are not implemented",
_tag_dyneither(_tmp3EA,sizeof(char),49))),_tag_dyneither(_tmp3E9,sizeof(void*),0)));}
goto _LL108;_LL111: {struct Cyc_Absyn_Var_d_struct*_tmp23A=(struct Cyc_Absyn_Var_d_struct*)
_tmp235;if(_tmp23A->tag != 0)goto _LL113;else{_tmp23B=_tmp23A->f1;}}_LL112: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp23B->sc))_tmp23B->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,ge,loc,_tmp23B,check_var_init,in_externCinclude,exports);
goto _LL108;_LL113: {struct Cyc_Absyn_Fn_d_struct*_tmp23C=(struct Cyc_Absyn_Fn_d_struct*)
_tmp235;if(_tmp23C->tag != 1)goto _LL115;else{_tmp23D=_tmp23C->f1;}}_LL114: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp23D->sc))_tmp23D->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,ge,loc,_tmp23D,in_externCinclude,exports);goto _LL108;_LL115: {
struct Cyc_Absyn_Typedef_d_struct*_tmp23E=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp235;if(_tmp23E->tag != 9)goto _LL117;else{_tmp23F=_tmp23E->f1;}}_LL116: Cyc_Tc_tcTypedefdecl(
te,ge,loc,_tmp23F);goto _LL108;_LL117: {struct Cyc_Absyn_Aggr_d_struct*_tmp240=(
struct Cyc_Absyn_Aggr_d_struct*)_tmp235;if(_tmp240->tag != 6)goto _LL119;else{
_tmp241=_tmp240->f1;}}_LL118: if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp241->sc))
_tmp241->sc=Cyc_Absyn_ExternC;Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp241);goto _LL108;
_LL119: {struct Cyc_Absyn_Datatype_d_struct*_tmp242=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp235;if(_tmp242->tag != 7)goto _LL11B;else{_tmp243=_tmp242->f1;}}_LL11A: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp243->sc))_tmp243->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp243);goto _LL108;_LL11B: {struct Cyc_Absyn_Enum_d_struct*
_tmp244=(struct Cyc_Absyn_Enum_d_struct*)_tmp235;if(_tmp244->tag != 8)goto _LL11D;
else{_tmp245=_tmp244->f1;}}_LL11C: if(in_externC  && Cyc_Tc_okay_externC(d->loc,
_tmp245->sc))_tmp245->sc=Cyc_Absyn_ExternC;Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp245);
goto _LL108;_LL11D: {struct Cyc_Absyn_Porton_d_struct*_tmp246=(struct Cyc_Absyn_Porton_d_struct*)
_tmp235;if(_tmp246->tag != 14)goto _LL11F;}_LL11E:{const char*_tmp3ED;void*_tmp3EC;(
_tmp3EC=0,Cyc_Tcutil_warn(d->loc,((_tmp3ED="spurious __cyclone_port_on__",
_tag_dyneither(_tmp3ED,sizeof(char),29))),_tag_dyneither(_tmp3EC,sizeof(void*),0)));}
goto _LL108;_LL11F: {struct Cyc_Absyn_Portoff_d_struct*_tmp247=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp235;if(_tmp247->tag != 15)goto _LL121;}_LL120: goto _LL108;_LL121: {struct Cyc_Absyn_Namespace_d_struct*
_tmp248=(struct Cyc_Absyn_Namespace_d_struct*)_tmp235;if(_tmp248->tag != 10)goto
_LL123;else{_tmp249=_tmp248->f1;_tmp24A=_tmp248->f2;}}_LL122: {struct Cyc_List_List*
_tmp25E=te->ns;struct Cyc_List_List*_tmp3EE;struct Cyc_List_List*_tmp25F=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp25E,((_tmp3EE=_cycalloc(sizeof(*_tmp3EE)),((_tmp3EE->hd=_tmp249,((_tmp3EE->tl=
0,_tmp3EE)))))));if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
ge->namespaces,_tmp249)){ge->namespaces=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(
grgn,ge->namespaces,_tmp249);te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp25F,
Cyc_Tcenv_empty_genv(grgn));}te->ns=_tmp25F;Cyc_Tc_tc_decls(te,_tmp24A,
in_externC,in_externCinclude,check_var_init,grgn,exports);te->ns=_tmp25E;goto
_LL108;}_LL123: {struct Cyc_Absyn_Using_d_struct*_tmp24B=(struct Cyc_Absyn_Using_d_struct*)
_tmp235;if(_tmp24B->tag != 11)goto _LL125;else{_tmp24C=_tmp24B->f1;_tmp24D=*
_tmp24C;_tmp24E=_tmp24D.f1;_tmp24F=_tmp24D.f2;_tmp250=_tmp24B->f2;}}_LL124: {
struct _dyneither_ptr*first;struct Cyc_List_List*rest;{union Cyc_Absyn_Nmspace
_tmp261=_tmp24E;int _tmp262;struct Cyc_List_List*_tmp263;struct Cyc_List_List*
_tmp264;struct Cyc_List_List*_tmp265;struct Cyc_List_List _tmp266;struct
_dyneither_ptr*_tmp267;struct Cyc_List_List*_tmp268;struct Cyc_List_List*_tmp269;
struct Cyc_List_List _tmp26A;struct _dyneither_ptr*_tmp26B;struct Cyc_List_List*
_tmp26C;_LL12A: if((_tmp261.Loc_n).tag != 3)goto _LL12C;_tmp262=(int)(_tmp261.Loc_n).val;
_LL12B: goto _LL12D;_LL12C: if((_tmp261.Rel_n).tag != 1)goto _LL12E;_tmp263=(struct
Cyc_List_List*)(_tmp261.Rel_n).val;if(_tmp263 != 0)goto _LL12E;_LL12D: goto _LL12F;
_LL12E: if((_tmp261.Abs_n).tag != 2)goto _LL130;_tmp264=(struct Cyc_List_List*)(
_tmp261.Abs_n).val;if(_tmp264 != 0)goto _LL130;_LL12F: first=_tmp24F;rest=0;goto
_LL129;_LL130: if((_tmp261.Rel_n).tag != 1)goto _LL132;_tmp265=(struct Cyc_List_List*)(
_tmp261.Rel_n).val;if(_tmp265 == 0)goto _LL132;_tmp266=*_tmp265;_tmp267=(struct
_dyneither_ptr*)_tmp266.hd;_tmp268=_tmp266.tl;_LL131: _tmp26B=_tmp267;_tmp26C=
_tmp268;goto _LL133;_LL132: if((_tmp261.Abs_n).tag != 2)goto _LL129;_tmp269=(struct
Cyc_List_List*)(_tmp261.Abs_n).val;if(_tmp269 == 0)goto _LL129;_tmp26A=*_tmp269;
_tmp26B=(struct _dyneither_ptr*)_tmp26A.hd;_tmp26C=_tmp26A.tl;_LL133: first=
_tmp26B;{struct Cyc_List_List*_tmp3EF;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp26C,((_tmp3EF=_cycalloc(sizeof(*
_tmp3EF)),((_tmp3EF->hd=_tmp24F,((_tmp3EF->tl=0,_tmp3EF)))))));}goto _LL129;
_LL129:;}{struct Cyc_List_List*_tmp26E=Cyc_Tcenv_resolve_namespace(te,loc,first,
rest);{struct Cyc_List_List*_tmp3F0;ge->availables=(struct Cyc_List_List*)((
_tmp3F0=_cycalloc(sizeof(*_tmp3F0)),((_tmp3F0->hd=_tmp26E,((_tmp3F0->tl=ge->availables,
_tmp3F0))))));}Cyc_Tc_tc_decls(te,_tmp250,in_externC,in_externCinclude,
check_var_init,grgn,exports);ge->availables=((struct Cyc_List_List*)_check_null(
ge->availables))->tl;goto _LL108;};}_LL125: {struct Cyc_Absyn_ExternC_d_struct*
_tmp251=(struct Cyc_Absyn_ExternC_d_struct*)_tmp235;if(_tmp251->tag != 12)goto
_LL127;else{_tmp252=_tmp251->f1;}}_LL126: Cyc_Tc_tc_decls(te,_tmp252,1,
in_externCinclude,check_var_init,grgn,exports);goto _LL108;_LL127: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp253=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp235;if(_tmp253->tag != 13)
goto _LL108;else{_tmp254=_tmp253->f1;_tmp255=_tmp253->f2;}}_LL128:((void(*)(void(*
f)(struct Cyc_Tcenv_Tenv*,struct _tuple11*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp255);{struct Cyc_List_List*
newexs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp255,(unsigned int)exports?*exports: 0);Cyc_Tc_tc_decls(te,
_tmp254,1,1,check_var_init,grgn,(struct Cyc_List_List**)& newexs);for(0;_tmp255 != 
0;_tmp255=_tmp255->tl){struct _tuple11*_tmp270=(struct _tuple11*)_tmp255->hd;if(!(*
_tmp270).f3){const char*_tmp3F4;void*_tmp3F3[1];struct Cyc_String_pa_struct _tmp3F2;(
_tmp3F2.tag=0,((_tmp3F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((*
_tmp270).f2)),((_tmp3F3[0]=& _tmp3F2,Cyc_Tcutil_warn((*_tmp270).f1,((_tmp3F4="%s is exported but not defined",
_tag_dyneither(_tmp3F4,sizeof(char),31))),_tag_dyneither(_tmp3F3,sizeof(void*),1)))))));}}
goto _LL108;};_LL108:;}}void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*
te,int check_var_init,struct Cyc_List_List*ds);void Cyc_Tc_tc(struct _RegionHandle*g,
struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){Cyc_Absynpp_set_params(&
Cyc_Absynpp_tc_params_r);Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct
Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict
nsdict;};static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*
d);static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,
struct Cyc_List_List*ds);static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*
env,struct Cyc_List_List*ds){return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,
struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(
Cyc_Tc_vardecl_needed,env,ds);}struct _tuple17{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*
f2;};static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*
d);static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*
d){void*_tmp274=d->r;struct Cyc_Absyn_Vardecl*_tmp276;struct Cyc_List_List*_tmp278;
struct Cyc_List_List**_tmp279;struct Cyc_List_List*_tmp27B;struct Cyc_List_List**
_tmp27C;struct Cyc_List_List*_tmp27E;struct Cyc_List_List**_tmp27F;struct Cyc_List_List*
_tmp281;struct Cyc_List_List**_tmp282;_LL135: {struct Cyc_Absyn_Var_d_struct*
_tmp275=(struct Cyc_Absyn_Var_d_struct*)_tmp274;if(_tmp275->tag != 0)goto _LL137;
else{_tmp276=_tmp275->f1;}}_LL136: if(env->in_cinclude  || _tmp276->sc != Cyc_Absyn_Extern
 && _tmp276->sc != Cyc_Absyn_ExternC)return 1;{struct _tuple0 _tmp284;union Cyc_Absyn_Nmspace
_tmp285;struct _dyneither_ptr*_tmp286;struct _tuple0*_tmp283=_tmp276->name;_tmp284=*
_tmp283;_tmp285=_tmp284.f1;_tmp286=_tmp284.f2;{struct Cyc_List_List*ns;{union Cyc_Absyn_Nmspace
_tmp287=_tmp285;int _tmp288;struct Cyc_List_List*_tmp289;struct Cyc_List_List*
_tmp28A;_LL142: if((_tmp287.Loc_n).tag != 3)goto _LL144;_tmp288=(int)(_tmp287.Loc_n).val;
_LL143: ns=0;goto _LL141;_LL144: if((_tmp287.Rel_n).tag != 1)goto _LL146;_tmp289=(
struct Cyc_List_List*)(_tmp287.Rel_n).val;_LL145: ns=_tmp289;goto _LL141;_LL146: if((
_tmp287.Abs_n).tag != 2)goto _LL141;_tmp28A=(struct Cyc_List_List*)(_tmp287.Abs_n).val;
_LL147: ns=_tmp28A;goto _LL141;_LL141:;}{struct _tuple17*_tmp28B=((struct _tuple17*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);
struct Cyc_Tcenv_Genv*_tmp28C=(*_tmp28B).f1;int _tmp28D=(*((struct _tuple12*(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp28C->ordinaries,
_tmp286)).f2;if(!_tmp28D)(*_tmp28B).f2=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(
env->rgn,(*_tmp28B).f2,_tmp286);return _tmp28D;};};};_LL137: {struct Cyc_Absyn_ExternC_d_struct*
_tmp277=(struct Cyc_Absyn_ExternC_d_struct*)_tmp274;if(_tmp277->tag != 12)goto
_LL139;else{_tmp278=_tmp277->f1;_tmp279=(struct Cyc_List_List**)& _tmp277->f1;}}
_LL138: _tmp27C=_tmp279;goto _LL13A;_LL139: {struct Cyc_Absyn_Using_d_struct*
_tmp27A=(struct Cyc_Absyn_Using_d_struct*)_tmp274;if(_tmp27A->tag != 11)goto _LL13B;
else{_tmp27B=_tmp27A->f2;_tmp27C=(struct Cyc_List_List**)& _tmp27A->f2;}}_LL13A:
_tmp27F=_tmp27C;goto _LL13C;_LL13B: {struct Cyc_Absyn_Namespace_d_struct*_tmp27D=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp274;if(_tmp27D->tag != 10)goto _LL13D;
else{_tmp27E=_tmp27D->f2;_tmp27F=(struct Cyc_List_List**)& _tmp27D->f2;}}_LL13C:*
_tmp27F=Cyc_Tc_treeshake_f(env,*_tmp27F);return 1;_LL13D: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp280=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp274;if(_tmp280->tag != 13)
goto _LL13F;else{_tmp281=_tmp280->f1;_tmp282=(struct Cyc_List_List**)& _tmp280->f1;}}
_LL13E: {int in_cinclude=env->in_cinclude;env->in_cinclude=1;*_tmp282=Cyc_Tc_treeshake_f(
env,*_tmp282);env->in_cinclude=in_cinclude;return 1;}_LL13F:;_LL140: return 1;
_LL134:;}static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct
_dyneither_ptr*x,void*y);static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*
set,struct _dyneither_ptr*x,void*y){return !((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(set,x);}static struct _tuple17*Cyc_Tc_treeshake_make_env_f(
struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge);static struct _tuple17*Cyc_Tc_treeshake_make_env_f(
struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){struct _tuple17*_tmp3F5;return(
_tmp3F5=_region_malloc(rgn,sizeof(*_tmp3F5)),((_tmp3F5->f1=ge,((_tmp3F5->f2=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp3F5)))));}struct
_tuple18{struct Cyc_List_List*f1;struct _tuple17*f2;};struct Cyc_List_List*Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){struct _RegionHandle _tmp28F=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp28F;_push_region(rgn);{struct Cyc_Tc_TreeshakeEnv
_tmp3F6;struct Cyc_Tc_TreeshakeEnv _tmp290=(_tmp3F6.rgn=rgn,((_tmp3F6.in_cinclude=
0,((_tmp3F6.nsdict=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple17*(*
f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict
d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp3F6)))));
struct Cyc_List_List*_tmp291=Cyc_Tc_treeshake_f(& _tmp290,ds);if(((int(*)(struct
Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp290.nsdict)){struct Cyc_List_List*_tmp292=
_tmp291;_npop_handler(0);return _tmp292;}{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,
_tmp290.nsdict);struct _tuple18 _tmp293=*((struct _tuple18*(*)(struct _RegionHandle*,
struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp290.nsdict);while(((int(*)(
struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& _tmp293)){struct
_tuple17*_tmp295;struct _tuple17 _tmp296;struct Cyc_Tcenv_Genv*_tmp297;struct Cyc_Set_Set*
_tmp298;struct _tuple18 _tmp294=_tmp293;_tmp295=_tmp294.f2;_tmp296=*_tmp295;
_tmp297=_tmp296.f1;_tmp298=_tmp296.f2;_tmp297->ordinaries=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct
_tuple12*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(
_tmp297->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct
_tuple12*y))Cyc_Tc_treeshake_remove_f,_tmp298,_tmp297->ordinaries);}{struct Cyc_List_List*
_tmp299=_tmp291;_npop_handler(0);return _tmp299;};};};_pop_region(rgn);}
