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
_dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
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
static void*Cyc_Core_arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int
sz);inline static void*Cyc_Core_arrcast(struct _dyneither_ptr dyn,unsigned int bd,
unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * 
sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];
struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{
char*tag;};struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(
void*,void*));int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);int Cyc_Dict_member(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct
_RegionHandle*,struct Cyc_Dict_Dict,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rempty(struct
_RegionHandle*r,int(*cmp)(void*,void*));int Cyc_Set_member(struct Cyc_Set_Set*s,
void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};
struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[8];struct Cyc_SlowDict_Present_struct{
char*tag;};extern char Cyc_SlowDict_Absent[7];struct Cyc_SlowDict_Absent_struct{
char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;}
;struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};int Cyc_strptrcmp(struct
_dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};
struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct
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
extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct
Cyc_Absyn_UniqueRgn_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
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
_tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple3 val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6
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
char*tag;};int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);extern
union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct Cyc_RgnOrder_RgnPO;
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
int tag;struct Cyc_Absyn_Stmt*f1;};extern struct Cyc_Tcenv_NotLoop_j_struct Cyc_Tcenv_NotLoop_j_val;
extern struct Cyc_Tcenv_CaseEnd_j_struct Cyc_Tcenv_CaseEnd_j_val;extern struct Cyc_Tcenv_FnEnd_j_struct
Cyc_Tcenv_FnEnd_j_val;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict
ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcenv_env_err(struct
_dyneither_ptr msg);struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(struct _RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct
Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Fenv*
old_fenv,struct Cyc_Absyn_Fndecl*new_fn);struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _dyneither_ptr*,struct
Cyc_List_List*);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple0*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple0*);struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple0*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple0*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);
struct _tuple9{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_Tcenv_CList*
f3;};struct _tuple9*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc);struct _tuple10{struct Cyc_Absyn_Tvar*f1;void*f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple10**
oldtv,struct Cyc_Position_Segment*loc);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,void*r,int resetable,int opened);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);struct
Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*
def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);struct _tuple10 Cyc_Tcutil_swap_kind(
void*t,void*kb);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*
e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);char Cyc_Tcenv_Env_error[
10]="Env_error";struct Cyc_Tcenv_Env_error_struct{char*tag;};struct Cyc_Tcenv_Env_error_struct
Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};void*Cyc_Tcenv_env_err(struct
_dyneither_ptr msg);void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){{const char*
_tmp320;void*_tmp31F[1];struct Cyc_String_pa_struct _tmp31E;(_tmp31E.tag=0,((
_tmp31E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp31F[0]=&
_tmp31E,Cyc_fprintf(Cyc_stderr,((_tmp320="Internal error in tcenv: %s\n",
_tag_dyneither(_tmp320,sizeof(char),29))),_tag_dyneither(_tmp31F,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}
struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;struct Cyc_Tcenv_NotLoop_j_struct Cyc_Tcenv_NotLoop_j_val={
0};struct Cyc_Tcenv_CaseEnd_j_struct Cyc_Tcenv_CaseEnd_j_val={1};struct Cyc_Tcenv_FnEnd_j_struct
Cyc_Tcenv_FnEnd_j_val={2};struct Cyc_Tcenv_CtrlEnv{struct _RegionHandle*ctrl_rgn;
void*continue_stmt;void*break_stmt;struct _tuple9*fallthru_clause;void*next_stmt;
int try_depth;};struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*
return_typ;struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict needed_labels;
struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*
delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*
b;struct Cyc_Tcenv_Bindings*tl;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*
shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;
struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*
ctrl_env;void*capability;void*curr_rgn;int in_notreadctxt;struct _RegionHandle*
fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_struct{
char*tag;};struct Cyc_Tcenv_NoBinding_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
void*Cyc_Tcenv_lookup_binding(struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v);
void*Cyc_Tcenv_lookup_binding(struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){if(Cyc_strptrcmp(v,bs->v)== 0)return bs->b;}(
int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(
struct _RegionHandle*r);struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct
_RegionHandle*r){struct Cyc_Tcenv_Genv*_tmp321;return(_tmp321=_region_malloc(r,
sizeof(*_tmp321)),((_tmp321->grgn=r,((_tmp321->namespaces=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp321->aggrdecls=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp321->datatypedecls=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp321->enumdecls=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp321->typedefs=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp321->ordinaries=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp321->availables=0,_tmp321)))))))))))))))));}struct
_tuple11{void*f1;int f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct
_RegionHandle*r);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){{
struct Cyc_Core_Opt*_tmp322;Cyc_Tcutil_empty_var_set=((_tmp322=_cycalloc(sizeof(*
_tmp322)),((_tmp322->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp322))));}{struct Cyc_Tcenv_Genv*
_tmpA=Cyc_Tcenv_empty_genv(r);{struct Cyc_Absyn_Datatypedecl**_tmp323;_tmpA->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**
v))Cyc_Dict_insert)(_tmpA->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp323=
_cycalloc(sizeof(*_tmp323)),((_tmp323[0]=Cyc_Absyn_exn_tud,_tmp323)))));}{struct
Cyc_List_List*_tmpC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmpC != 0;_tmpC=_tmpC->tl){struct Cyc_Tcenv_DatatypeRes_struct*_tmp329;
struct Cyc_Tcenv_DatatypeRes_struct _tmp328;struct _tuple11*_tmp327;_tmpA->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple11*v))Cyc_Dict_insert)(_tmpA->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)
_tmpC->hd)->name).f2,((_tmp327=_region_malloc(r,sizeof(*_tmp327)),((_tmp327->f1=(
void*)((_tmp329=_cycalloc(sizeof(*_tmp329)),((_tmp329[0]=((_tmp328.tag=2,((
_tmp328.f1=Cyc_Absyn_exn_tud,((_tmp328.f2=(struct Cyc_Absyn_Datatypefield*)_tmpC->hd,
_tmp328)))))),_tmp329)))),((_tmp327->f2=1,_tmp327)))))));}}{struct Cyc_Dict_Dict
ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),
0,_tmpA);struct Cyc_Tcenv_Tenv*_tmp32A;return(_tmp32A=_region_malloc(r,sizeof(*
_tmp32A)),((_tmp32A->ns=0,((_tmp32A->ae=ae,((_tmp32A->le=0,((_tmp32A->allow_valueof=
0,_tmp32A)))))))));};};}static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,
struct Cyc_List_List*ns);static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,
struct Cyc_List_List*ns){return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,
loc,n,ns));}static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*
ns);static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){const char*_tmp32B;return((struct Cyc_List_List*(*)(struct
_dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp32B="outer_namespace",_tag_dyneither(
_tmp32B,sizeof(char),16))));}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2);static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2){if(n1 == n2)return 1;for(0;n1 != 0;
n1=n1->tl){if(n2 == 0)return 0;if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(
struct _dyneither_ptr*)n2->hd)!= 0)return 0;n2=n2->tl;}if(n2 != 0)return 0;return 1;}
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct
_dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss);static void Cyc_Tcenv_check_repeat(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dyneither_ptr*),struct _dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*
nss){for(0;nss != 0;nss=nss->tl){if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)
nss->hd)){struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);
struct _handler_cons _tmp12;_push_handler(& _tmp12);{int _tmp14=0;if(setjmp(_tmp12.handler))
_tmp14=1;if(!_tmp14){lookup(ge2,v);{const char*_tmp32F;void*_tmp32E[1];struct Cyc_String_pa_struct
_tmp32D;(_tmp32D.tag=0,((_tmp32D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
v),((_tmp32E[0]=& _tmp32D,Cyc_Tcutil_terr(loc,((_tmp32F="%s is ambiguous",
_tag_dyneither(_tmp32F,sizeof(char),16))),_tag_dyneither(_tmp32E,sizeof(void*),1)))))));};;
_pop_handler();}else{void*_tmp13=(void*)_exn_thrown;void*_tmp19=_tmp13;_LL1: {
struct Cyc_Dict_Absent_struct*_tmp1A=(struct Cyc_Dict_Absent_struct*)_tmp19;if(
_tmp1A->tag != Cyc_Dict_Absent)goto _LL3;}_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(
_tmp19);_LL0:;}};}}return;}static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v);static void*Cyc_Tcenv_scoped_lookup(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dyneither_ptr*),struct _dyneither_ptr*v){struct Cyc_List_List*cns=te->ns;
while(1){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);{struct _handler_cons _tmp1B;
_push_handler(& _tmp1B);{int _tmp1D=0;if(setjmp(_tmp1B.handler))_tmp1D=1;if(!
_tmp1D){{void*result=lookup(ge,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1E=result;_npop_handler(0);return _tmp1E;};};_pop_handler();}else{void*
_tmp1C=(void*)_exn_thrown;void*_tmp20=_tmp1C;_LL6: {struct Cyc_Dict_Absent_struct*
_tmp21=(struct Cyc_Dict_Absent_struct*)_tmp20;if(_tmp21->tag != Cyc_Dict_Absent)
goto _LL8;}_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp20);_LL5:;}};}{struct Cyc_List_List*
_tmp22=ge->availables;for(0;_tmp22 != 0;_tmp22=_tmp22->tl){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)_tmp22->hd);struct _handler_cons _tmp23;_push_handler(&
_tmp23);{int _tmp25=0;if(setjmp(_tmp23.handler))_tmp25=1;if(!_tmp25){{void*result=
lookup(ge2,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)
_tmp22->hd,_tmp22->tl);{void*_tmp26=result;_npop_handler(0);return _tmp26;};};
_pop_handler();}else{void*_tmp24=(void*)_exn_thrown;void*_tmp28=_tmp24;_LLB: {
struct Cyc_Dict_Absent_struct*_tmp29=(struct Cyc_Dict_Absent_struct*)_tmp28;if(
_tmp29->tag != Cyc_Dict_Absent)goto _LLD;}_LLC: goto _LLA;_LLD:;_LLE:(void)_throw(
_tmp28);_LLA:;}};}}if(cns == 0){struct Cyc_Dict_Absent_struct _tmp332;struct Cyc_Dict_Absent_struct*
_tmp331;(int)_throw((void*)((_tmp331=_cycalloc_atomic(sizeof(*_tmp331)),((
_tmp331[0]=((_tmp332.tag=Cyc_Dict_Absent,_tmp332)),_tmp331)))));}cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _dyneither_ptr*v);static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*
ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp2C=ge->ordinaries;struct
_tuple11*ans=((struct _tuple11*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup)(_tmp2C,v);(*ans).f2=1;return(*ans).f1;}static void*Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q);static
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple0*q){struct _tuple0 _tmp2E;union Cyc_Absyn_Nmspace _tmp2F;struct
_dyneither_ptr*_tmp30;struct _tuple0*_tmp2D=q;_tmp2E=*_tmp2D;_tmp2F=_tmp2E.f1;
_tmp30=_tmp2E.f2;{union Cyc_Absyn_Nmspace _tmp31=_tmp2F;int _tmp32;struct Cyc_List_List*
_tmp33;struct Cyc_List_List*_tmp34;struct Cyc_List_List _tmp35;struct _dyneither_ptr*
_tmp36;struct Cyc_List_List*_tmp37;struct Cyc_List_List*_tmp38;_LL10: if((_tmp31.Loc_n).tag
!= 3)goto _LL12;_tmp32=(int)(_tmp31.Loc_n).val;_LL11: goto _LL13;_LL12: if((_tmp31.Rel_n).tag
!= 1)goto _LL14;_tmp33=(struct Cyc_List_List*)(_tmp31.Rel_n).val;if(_tmp33 != 0)
goto _LL14;_LL13: return((void*(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct
_dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,
_tmp30);_LL14: if((_tmp31.Rel_n).tag != 1)goto _LL16;_tmp34=(struct Cyc_List_List*)(
_tmp31.Rel_n).val;if(_tmp34 == 0)goto _LL16;_tmp35=*_tmp34;_tmp36=(struct
_dyneither_ptr*)_tmp35.hd;_tmp37=_tmp35.tl;_LL15: {struct Cyc_Tcenv_Genv*_tmp39=
Cyc_Tcenv_lookup_namespace(te,loc,_tmp36,_tmp37);return Cyc_Tcenv_lookup_ordinary_global_f(
_tmp39,_tmp30);}_LL16: if((_tmp31.Abs_n).tag != 2)goto _LLF;_tmp38=(struct Cyc_List_List*)(
_tmp31.Abs_n).val;_LL17: return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp38),
_tmp30);_LLF:;};}struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns);
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){struct
Cyc_List_List*_tmp3A=te->ns;struct _RegionHandle*_tmp3B=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp3C=0;while(1){struct Cyc_Tcenv_Genv*_tmp3D=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp3A);
struct Cyc_List_List*_tmp3E=_tmp3D->availables;struct Cyc_Set_Set*_tmp3F=_tmp3D->namespaces;{
struct Cyc_List_List*_tmp40=_tmp3E;for(0;_tmp40 != 0;_tmp40=_tmp40->tl){struct Cyc_Set_Set*
_tmp41=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp40->hd))->namespaces;if(((int(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp41,n)){struct
Cyc_List_List*_tmp335;struct Cyc_List_List*_tmp334;_tmp3C=((_tmp334=
_region_malloc(_tmp3B,sizeof(*_tmp334)),((_tmp334->hd=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp3A,((_tmp335=
_cycalloc(sizeof(*_tmp335)),((_tmp335->hd=n,((_tmp335->tl=ns,_tmp335))))))),((
_tmp334->tl=_tmp3C,_tmp334))))));}}}if(((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(_tmp3F,n)){struct Cyc_List_List*_tmp338;
struct Cyc_List_List*_tmp337;_tmp3C=((_tmp337=_region_malloc(_tmp3B,sizeof(*
_tmp337)),((_tmp337->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp3A,((_tmp338=_cycalloc(sizeof(*_tmp338)),((
_tmp338->hd=n,((_tmp338->tl=ns,_tmp338))))))),((_tmp337->tl=_tmp3C,_tmp337))))));}
if(_tmp3C != 0){if(_tmp3C->tl != 0){const char*_tmp33C;void*_tmp33B[1];struct Cyc_String_pa_struct
_tmp33A;(_tmp33A.tag=0,((_tmp33A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
n),((_tmp33B[0]=& _tmp33A,Cyc_Tcutil_terr(loc,((_tmp33C="%s is ambiguous",
_tag_dyneither(_tmp33C,sizeof(char),16))),_tag_dyneither(_tmp33B,sizeof(void*),1)))))));}
return(struct Cyc_List_List*)_tmp3C->hd;}if(_tmp3A == 0){struct Cyc_Dict_Absent_struct
_tmp33F;struct Cyc_Dict_Absent_struct*_tmp33E;(int)_throw((void*)((_tmp33E=
_cycalloc_atomic(sizeof(*_tmp33E)),((_tmp33E[0]=((_tmp33F.tag=Cyc_Dict_Absent,
_tmp33F)),_tmp33E)))));}_tmp3A=Cyc_Tcenv_outer_namespace(_tmp3A);}}static struct
Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct
_dyneither_ptr*v);static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp4B=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*
k))Cyc_Dict_lookup)(_tmp4B,v);}struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q);struct
Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple0*q){struct _tuple0 _tmp4D;union Cyc_Absyn_Nmspace _tmp4E;struct
_dyneither_ptr*_tmp4F;struct _tuple0*_tmp4C=q;_tmp4D=*_tmp4C;_tmp4E=_tmp4D.f1;
_tmp4F=_tmp4D.f2;{union Cyc_Absyn_Nmspace _tmp50=_tmp4E;int _tmp51;struct Cyc_List_List*
_tmp52;struct Cyc_List_List*_tmp53;struct Cyc_List_List*_tmp54;struct Cyc_List_List
_tmp55;struct _dyneither_ptr*_tmp56;struct Cyc_List_List*_tmp57;_LL19: if((_tmp50.Loc_n).tag
!= 3)goto _LL1B;_tmp51=(int)(_tmp50.Loc_n).val;_LL1A: goto _LL1C;_LL1B: if((_tmp50.Rel_n).tag
!= 1)goto _LL1D;_tmp52=(struct Cyc_List_List*)(_tmp50.Rel_n).val;if(_tmp52 != 0)
goto _LL1D;_LL1C: return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,
Cyc_Tcenv_lookup_aggrdecl_f,_tmp4F);_LL1D: if((_tmp50.Abs_n).tag != 2)goto _LL1F;
_tmp53=(struct Cyc_List_List*)(_tmp50.Abs_n).val;_LL1E: {struct Cyc_Dict_Dict
_tmp58=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp53))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp58,_tmp4F);}
_LL1F: if((_tmp50.Rel_n).tag != 1)goto _LL18;_tmp54=(struct Cyc_List_List*)(_tmp50.Rel_n).val;
if(_tmp54 == 0)goto _LL18;_tmp55=*_tmp54;_tmp56=(struct _dyneither_ptr*)_tmp55.hd;
_tmp57=_tmp55.tl;_LL20: {struct Cyc_Dict_Dict _tmp59=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp56,_tmp57))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp59,_tmp4F);}_LL18:;};}
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*
ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp5A=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5A,v);}struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q);struct
Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0 _tmp5C;union Cyc_Absyn_Nmspace
_tmp5D;struct _dyneither_ptr*_tmp5E;struct _tuple0*_tmp5B=q;_tmp5C=*_tmp5B;_tmp5D=
_tmp5C.f1;_tmp5E=_tmp5C.f2;{union Cyc_Absyn_Nmspace _tmp5F=_tmp5D;int _tmp60;struct
Cyc_List_List*_tmp61;struct Cyc_List_List*_tmp62;struct Cyc_List_List _tmp63;struct
_dyneither_ptr*_tmp64;struct Cyc_List_List*_tmp65;struct Cyc_List_List*_tmp66;
_LL22: if((_tmp5F.Loc_n).tag != 3)goto _LL24;_tmp60=(int)(_tmp5F.Loc_n).val;_LL23:
goto _LL25;_LL24: if((_tmp5F.Rel_n).tag != 1)goto _LL26;_tmp61=(struct Cyc_List_List*)(
_tmp5F.Rel_n).val;if(_tmp61 != 0)goto _LL26;_LL25: return((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Datatypedecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp5E);_LL26: if((_tmp5F.Rel_n).tag != 1)
goto _LL28;_tmp62=(struct Cyc_List_List*)(_tmp5F.Rel_n).val;if(_tmp62 == 0)goto
_LL28;_tmp63=*_tmp62;_tmp64=(struct _dyneither_ptr*)_tmp63.hd;_tmp65=_tmp63.tl;
_LL27: {struct Cyc_Dict_Dict _tmp67=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp64,
_tmp65))->datatypedecls;return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp67,_tmp5E);}_LL28: if((_tmp5F.Abs_n).tag
!= 2)goto _LL21;_tmp66=(struct Cyc_List_List*)(_tmp5F.Abs_n).val;_LL29: {struct Cyc_Dict_Dict
_tmp68=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp66))->datatypedecls;return((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp68,_tmp5E);}
_LL21:;};}static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}struct Cyc_Absyn_Datatypedecl***
Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple0*q);struct Cyc_Absyn_Datatypedecl***
Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0 _tmp6A;union Cyc_Absyn_Nmspace
_tmp6B;struct _dyneither_ptr*_tmp6C;struct _tuple0*_tmp69=q;_tmp6A=*_tmp69;_tmp6B=
_tmp6A.f1;_tmp6C=_tmp6A.f2;{union Cyc_Absyn_Nmspace _tmp6D=_tmp6B;struct Cyc_List_List*
_tmp6E;int _tmp6F;struct Cyc_List_List*_tmp70;struct Cyc_List_List _tmp71;struct
_dyneither_ptr*_tmp72;struct Cyc_List_List*_tmp73;struct Cyc_List_List*_tmp74;
_LL2B: if((_tmp6D.Rel_n).tag != 1)goto _LL2D;_tmp6E=(struct Cyc_List_List*)(_tmp6D.Rel_n).val;
if(_tmp6E != 0)goto _LL2D;_LL2C: {struct _handler_cons _tmp75;_push_handler(& _tmp75);{
int _tmp77=0;if(setjmp(_tmp75.handler))_tmp77=1;if(!_tmp77){{struct Cyc_Absyn_Datatypedecl***
_tmp340;struct Cyc_Absyn_Datatypedecl***_tmp79=(_tmp340=_region_malloc(r,sizeof(*
_tmp340)),((_tmp340[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct
Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp6C),_tmp340)));_npop_handler(0);
return _tmp79;};_pop_handler();}else{void*_tmp76=(void*)_exn_thrown;void*_tmp7B=
_tmp76;_LL34: {struct Cyc_Dict_Absent_struct*_tmp7C=(struct Cyc_Dict_Absent_struct*)
_tmp7B;if(_tmp7C->tag != Cyc_Dict_Absent)goto _LL36;}_LL35: return 0;_LL36:;_LL37:(
void)_throw(_tmp7B);_LL33:;}};}_LL2D: if((_tmp6D.Loc_n).tag != 3)goto _LL2F;_tmp6F=(
int)(_tmp6D.Loc_n).val;_LL2E:{const char*_tmp343;void*_tmp342;(_tmp342=0,Cyc_Tcutil_terr(
loc,((_tmp343="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp343,sizeof(
char),33))),_tag_dyneither(_tmp342,sizeof(void*),0)));}return 0;_LL2F: if((_tmp6D.Rel_n).tag
!= 1)goto _LL31;_tmp70=(struct Cyc_List_List*)(_tmp6D.Rel_n).val;if(_tmp70 == 0)
goto _LL31;_tmp71=*_tmp70;_tmp72=(struct _dyneither_ptr*)_tmp71.hd;_tmp73=_tmp71.tl;
_LL30: {struct Cyc_Tcenv_Genv*ge;{struct _handler_cons _tmp7F;_push_handler(& _tmp7F);{
int _tmp81=0;if(setjmp(_tmp7F.handler))_tmp81=1;if(!_tmp81){ge=Cyc_Tcenv_lookup_namespace(
te,loc,_tmp72,_tmp73);;_pop_handler();}else{void*_tmp80=(void*)_exn_thrown;void*
_tmp83=_tmp80;_LL39: {struct Cyc_Dict_Absent_struct*_tmp84=(struct Cyc_Dict_Absent_struct*)
_tmp83;if(_tmp84->tag != Cyc_Dict_Absent)goto _LL3B;}_LL3A:{const char*_tmp346;void*
_tmp345;(_tmp345=0,Cyc_Tcutil_terr(loc,((_tmp346="bad qualified name for @extensible datatype",
_tag_dyneither(_tmp346,sizeof(char),44))),_tag_dyneither(_tmp345,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_struct _tmp349;struct Cyc_Dict_Absent_struct*_tmp348;(int)
_throw((void*)((_tmp348=_cycalloc_atomic(sizeof(*_tmp348)),((_tmp348[0]=((
_tmp349.tag=Cyc_Dict_Absent,_tmp349)),_tmp348)))));};_LL3B:;_LL3C:(void)_throw(
_tmp83);_LL38:;}};}{struct Cyc_Dict_Dict _tmp89=ge->datatypedecls;struct Cyc_Absyn_Datatypedecl***
_tmp34A;return(_tmp34A=_region_malloc(r,sizeof(*_tmp34A)),((_tmp34A[0]=((struct
Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp89,_tmp6C),_tmp34A)));};}_LL31: if((_tmp6D.Abs_n).tag != 2)goto _LL2A;_tmp74=(
struct Cyc_List_List*)(_tmp6D.Abs_n).val;_LL32: {struct Cyc_Dict_Dict _tmp8B=(((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp74))->datatypedecls;struct Cyc_Absyn_Datatypedecl***_tmp34B;return(
_tmp34B=_region_malloc(r,sizeof(*_tmp34B)),((_tmp34B[0]=((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8B,_tmp6C),
_tmp34B)));}_LL2A:;};}static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
struct Cyc_Dict_Dict _tmp8D=ge->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8D,v);}struct
Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple0*q);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0
_tmp8F;union Cyc_Absyn_Nmspace _tmp90;struct _dyneither_ptr*_tmp91;struct _tuple0*
_tmp8E=q;_tmp8F=*_tmp8E;_tmp90=_tmp8F.f1;_tmp91=_tmp8F.f2;{union Cyc_Absyn_Nmspace
_tmp92=_tmp90;int _tmp93;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp95;
struct Cyc_List_List _tmp96;struct _dyneither_ptr*_tmp97;struct Cyc_List_List*_tmp98;
struct Cyc_List_List*_tmp99;_LL3E: if((_tmp92.Loc_n).tag != 3)goto _LL40;_tmp93=(int)(
_tmp92.Loc_n).val;_LL3F: goto _LL41;_LL40: if((_tmp92.Rel_n).tag != 1)goto _LL42;
_tmp94=(struct Cyc_List_List*)(_tmp92.Rel_n).val;if(_tmp94 != 0)goto _LL42;_LL41:
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,
_tmp91);_LL42: if((_tmp92.Rel_n).tag != 1)goto _LL44;_tmp95=(struct Cyc_List_List*)(
_tmp92.Rel_n).val;if(_tmp95 == 0)goto _LL44;_tmp96=*_tmp95;_tmp97=(struct
_dyneither_ptr*)_tmp96.hd;_tmp98=_tmp96.tl;_LL43: {struct Cyc_Dict_Dict _tmp9A=(
Cyc_Tcenv_lookup_namespace(te,loc,_tmp97,_tmp98))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp9A,_tmp91);}
_LL44: if((_tmp92.Abs_n).tag != 2)goto _LL3D;_tmp99=(struct Cyc_List_List*)(_tmp92.Abs_n).val;
_LL45: {struct Cyc_Dict_Dict _tmp9B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp99))->enumdecls;return((
struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp9B,_tmp91);}_LL3D:;};}static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
struct Cyc_Dict_Dict _tmp9C=ge->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp9C,v);}struct
Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple0*q);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct
_tuple0 _tmp9E;union Cyc_Absyn_Nmspace _tmp9F;struct _dyneither_ptr*_tmpA0;struct
_tuple0*_tmp9D=q;_tmp9E=*_tmp9D;_tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;{union Cyc_Absyn_Nmspace
_tmpA1=_tmp9F;int _tmpA2;struct Cyc_List_List*_tmpA3;struct Cyc_List_List*_tmpA4;
struct Cyc_List_List _tmpA5;struct _dyneither_ptr*_tmpA6;struct Cyc_List_List*_tmpA7;
struct Cyc_List_List*_tmpA8;_LL47: if((_tmpA1.Loc_n).tag != 3)goto _LL49;_tmpA2=(int)(
_tmpA1.Loc_n).val;_LL48: goto _LL4A;_LL49: if((_tmpA1.Rel_n).tag != 1)goto _LL4B;
_tmpA3=(struct Cyc_List_List*)(_tmpA1.Rel_n).val;if(_tmpA3 != 0)goto _LL4B;_LL4A:
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,
_tmpA0);_LL4B: if((_tmpA1.Rel_n).tag != 1)goto _LL4D;_tmpA4=(struct Cyc_List_List*)(
_tmpA1.Rel_n).val;if(_tmpA4 == 0)goto _LL4D;_tmpA5=*_tmpA4;_tmpA6=(struct
_dyneither_ptr*)_tmpA5.hd;_tmpA7=_tmpA5.tl;_LL4C: {struct Cyc_Dict_Dict _tmpA9=(
Cyc_Tcenv_lookup_namespace(te,loc,_tmpA6,_tmpA7))->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmpA9,_tmpA0);}
_LL4D: if((_tmpA1.Abs_n).tag != 2)goto _LL46;_tmpA8=(struct Cyc_List_List*)(_tmpA1.Abs_n).val;
_LL4E: {struct Cyc_Dict_Dict _tmpAA=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmpA8))->typedefs;return((
struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmpAA,_tmpA0);}_LL46:;};}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct
Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(
struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){struct Cyc_Tcenv_Fenv*_tmpAB=
te->le;if((struct Cyc_Tcenv_Tenv*)te == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(
err_msg);return(struct Cyc_Tcenv_Fenv*)_check_null(_tmpAB);}struct _RegionHandle*
Cyc_Tcenv_coerce_heap_region();struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return(struct _RegionHandle*)Cyc_Core_heap_region;}struct _RegionHandle*Cyc_Tcenv_get_fnrgn(
struct Cyc_Tcenv_Tenv*te);struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmpAC=te->le;if(_tmpAC != 0){struct Cyc_Tcenv_Fenv _tmpAE;
struct _RegionHandle*_tmpAF;struct Cyc_Tcenv_Fenv*_tmpAD=(struct Cyc_Tcenv_Fenv*)
_tmpAC;_tmpAE=*_tmpAD;_tmpAF=_tmpAE.fnrgn;return _tmpAF;}return Cyc_Tcenv_coerce_heap_region();}
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Fenv*fe);static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct
_RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){if(te->le == 0){
const char*_tmp34C;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp34C="put_fenv",
_tag_dyneither(_tmp34C,sizeof(char),9))));}{struct Cyc_Tcenv_Tenv*_tmp34D;return(
_tmp34D=_region_malloc(l,sizeof(*_tmp34D)),((_tmp34D->ns=te->ns,((_tmp34D->ae=te->ae,((
_tmp34D->le=(struct Cyc_Tcenv_Fenv*)fe,((_tmp34D->allow_valueof=te->allow_valueof,
_tmp34D)))))))));};}static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct
_RegionHandle*l,struct Cyc_Tcenv_Tenv*te);static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(
struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_tmp34E;
return(_tmp34E=_region_malloc(l,sizeof(*_tmp34E)),((_tmp34E->ns=te->ns,((_tmp34E->ae=
te->ae,((_tmp34E->le=0,((_tmp34E->allow_valueof=te->allow_valueof,_tmp34E)))))))));}
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,
struct Cyc_Tcenv_Fenv*f);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpB4;
struct Cyc_List_List*_tmpB5;struct Cyc_RgnOrder_RgnPO*_tmpB6;struct Cyc_Tcenv_Bindings*
_tmpB7;struct Cyc_Absyn_Stmt*_tmpB8;struct Cyc_Tcenv_CtrlEnv*_tmpB9;void*_tmpBA;
void*_tmpBB;int _tmpBC;struct _RegionHandle*_tmpBD;struct Cyc_Tcenv_Fenv _tmpB3=*f;
_tmpB4=_tmpB3.shared;_tmpB5=_tmpB3.type_vars;_tmpB6=_tmpB3.region_order;_tmpB7=
_tmpB3.locals;_tmpB8=_tmpB3.encloser;_tmpB9=_tmpB3.ctrl_env;_tmpBA=_tmpB3.capability;
_tmpBB=_tmpB3.curr_rgn;_tmpBC=_tmpB3.in_notreadctxt;_tmpBD=_tmpB3.fnrgn;{struct
Cyc_Tcenv_Fenv*_tmp34F;return(_tmp34F=_region_malloc(l,sizeof(*_tmp34F)),((
_tmp34F->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpB4,((_tmp34F->type_vars=(struct
Cyc_List_List*)_tmpB5,((_tmp34F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpB6,((
_tmp34F->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpB7),((
_tmp34F->encloser=(struct Cyc_Absyn_Stmt*)_tmpB8,((_tmp34F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmpB9),((_tmp34F->capability=(void*)_tmpBA,((_tmp34F->curr_rgn=(
void*)_tmpBB,((_tmp34F->in_notreadctxt=(int)_tmpBC,((_tmp34F->fnrgn=(struct
_RegionHandle*)l,_tmp34F)))))))))))))))))))));};}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpC0;
struct Cyc_List_List*_tmpC1;struct Cyc_RgnOrder_RgnPO*_tmpC2;struct Cyc_Tcenv_Bindings*
_tmpC3;struct Cyc_Absyn_Stmt*_tmpC4;struct Cyc_Tcenv_CtrlEnv*_tmpC5;void*_tmpC6;
void*_tmpC7;int _tmpC8;struct Cyc_Tcenv_Fenv _tmpBF=*f;_tmpC0=_tmpBF.shared;_tmpC1=
_tmpBF.type_vars;_tmpC2=_tmpBF.region_order;_tmpC3=_tmpBF.locals;_tmpC4=_tmpBF.encloser;
_tmpC5=_tmpBF.ctrl_env;_tmpC6=_tmpBF.capability;_tmpC7=_tmpBF.curr_rgn;_tmpC8=
_tmpBF.in_notreadctxt;{struct _RegionHandle*_tmpCA;void*_tmpCB;void*_tmpCC;struct
_tuple9*_tmpCD;void*_tmpCE;int _tmpCF;struct Cyc_Tcenv_CtrlEnv _tmpC9=*_tmpC5;
_tmpCA=_tmpC9.ctrl_rgn;_tmpCB=_tmpC9.continue_stmt;_tmpCC=_tmpC9.break_stmt;
_tmpCD=_tmpC9.fallthru_clause;_tmpCE=_tmpC9.next_stmt;_tmpCF=_tmpC9.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp350;struct Cyc_Tcenv_CtrlEnv*_tmpD0=(_tmp350=
_region_malloc(l,sizeof(*_tmp350)),((_tmp350->ctrl_rgn=_tmpCA,((_tmp350->continue_stmt=
_tmpCB,((_tmp350->break_stmt=_tmpCC,((_tmp350->fallthru_clause=_tmpCD,((_tmp350->next_stmt=
_tmpCE,((_tmp350->try_depth=_tmpCF,_tmp350)))))))))))));struct Cyc_Tcenv_Fenv*
_tmp351;return(_tmp351=_region_malloc(l,sizeof(*_tmp351)),((_tmp351->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmpC0,((_tmp351->type_vars=(struct Cyc_List_List*)
_tmpC1,((_tmp351->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC2,((_tmp351->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpC3),((_tmp351->encloser=(
struct Cyc_Absyn_Stmt*)_tmpC4,((_tmp351->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)
_tmpD0,((_tmp351->capability=(void*)_tmpC6,((_tmp351->curr_rgn=(void*)_tmpC7,((
_tmp351->in_notreadctxt=(int)_tmpC8,((_tmp351->fnrgn=(struct _RegionHandle*)l,
_tmp351)))))))))))))))))))));};};}void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv
_tmpD5;struct Cyc_Tcenv_SharedFenv*_tmpD6;const char*_tmp352;struct Cyc_Tcenv_Fenv*
_tmpD4=Cyc_Tcenv_get_fenv(te,((_tmp352="return_typ",_tag_dyneither(_tmp352,
sizeof(char),11))));_tmpD5=*_tmpD4;_tmpD6=_tmpD5.shared;{void*_tmpD8;struct Cyc_Tcenv_SharedFenv
_tmpD7=*_tmpD6;_tmpD8=_tmpD7.return_typ;return _tmpD8;};}struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*te);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct
Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpD9=te->le;if(te->le == 0)return 0;{
struct Cyc_List_List*_tmpDB;struct Cyc_Tcenv_Fenv _tmpDA=*((struct Cyc_Tcenv_Fenv*)
_check_null(_tmpD9));_tmpDB=_tmpDA.type_vars;return _tmpDB;};}struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Core_Opt*_tmp353;return(_tmp353=_cycalloc(
sizeof(*_tmp353)),((_tmp353->v=Cyc_Tcenv_lookup_type_vars(te),_tmp353)));}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*tvs){struct Cyc_Tcenv_Fenv _tmpDF;struct Cyc_Tcenv_SharedFenv*
_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_RgnOrder_RgnPO*_tmpE2;struct Cyc_Tcenv_Bindings*
_tmpE3;struct Cyc_Absyn_Stmt*_tmpE4;struct Cyc_Tcenv_CtrlEnv*_tmpE5;void*_tmpE6;
void*_tmpE7;int _tmpE8;const char*_tmp354;struct Cyc_Tcenv_Fenv*_tmpDE=Cyc_Tcenv_get_fenv(
te,((_tmp354="add_type_vars",_tag_dyneither(_tmp354,sizeof(char),14))));_tmpDF=*
_tmpDE;_tmpE0=_tmpDF.shared;_tmpE1=_tmpDF.type_vars;_tmpE2=_tmpDF.region_order;
_tmpE3=_tmpDF.locals;_tmpE4=_tmpDF.encloser;_tmpE5=_tmpDF.ctrl_env;_tmpE6=_tmpDF.capability;
_tmpE7=_tmpDF.curr_rgn;_tmpE8=_tmpDF.in_notreadctxt;Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_List_List*_tmpE9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpE1);Cyc_Tcutil_check_unique_tvars(
loc,_tmpE9);{struct Cyc_Tcenv_Fenv*_tmp355;struct Cyc_Tcenv_Fenv*_tmpEA=(_tmp355=
_region_malloc(r,sizeof(*_tmp355)),((_tmp355->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpE0,((_tmp355->type_vars=(struct Cyc_List_List*)_tmpE9,((_tmp355->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpE2,((_tmp355->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmpE3),((_tmp355->encloser=(struct Cyc_Absyn_Stmt*)
_tmpE4,((_tmp355->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmpE5),((_tmp355->capability=(void*)_tmpE6,((_tmp355->curr_rgn=(void*)_tmpE7,((
_tmp355->in_notreadctxt=(int)_tmpE8,((_tmp355->fnrgn=(struct _RegionHandle*)r,
_tmp355)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmpEA);};};}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmpEC=te->le;if(_tmpEC == 0)return Cyc_Tcenv_put_emptyfenv(
r,te);else{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,(struct Cyc_Tcenv_Fenv*)
_tmpEC);return Cyc_Tcenv_put_fenv(r,te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Vardecl*vd);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*
vd){struct _dyneither_ptr*_tmpED=(*vd->name).f2;{union Cyc_Absyn_Nmspace _tmpEE=(*
vd->name).f1;int _tmpEF;_LL50: if((_tmpEE.Loc_n).tag != 3)goto _LL52;_tmpEF=(int)(
_tmpEE.Loc_n).val;_LL51: goto _LL4F;_LL52:;_LL53: {struct Cyc_Core_Impossible_struct
_tmp35B;const char*_tmp35A;struct Cyc_Core_Impossible_struct*_tmp359;(int)_throw((
void*)((_tmp359=_cycalloc(sizeof(*_tmp359)),((_tmp359[0]=((_tmp35B.tag=Cyc_Core_Impossible,((
_tmp35B.f1=((_tmp35A="add_local_var: called with Rel_n",_tag_dyneither(_tmp35A,
sizeof(char),33))),_tmp35B)))),_tmp359)))));}_LL4F:;}{struct Cyc_Tcenv_Fenv _tmpF5;
struct Cyc_Tcenv_SharedFenv*_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_RgnOrder_RgnPO*
_tmpF8;struct Cyc_Tcenv_Bindings*_tmpF9;struct Cyc_Absyn_Stmt*_tmpFA;struct Cyc_Tcenv_CtrlEnv*
_tmpFB;void*_tmpFC;void*_tmpFD;int _tmpFE;const char*_tmp35C;struct Cyc_Tcenv_Fenv*
_tmpF4=Cyc_Tcenv_get_fenv(te,((_tmp35C="add_local_var",_tag_dyneither(_tmp35C,
sizeof(char),14))));_tmpF5=*_tmpF4;_tmpF6=_tmpF5.shared;_tmpF7=_tmpF5.type_vars;
_tmpF8=_tmpF5.region_order;_tmpF9=_tmpF5.locals;_tmpFA=_tmpF5.encloser;_tmpFB=
_tmpF5.ctrl_env;_tmpFC=_tmpF5.capability;_tmpFD=_tmpF5.curr_rgn;_tmpFE=_tmpF5.in_notreadctxt;{
struct Cyc_Absyn_Local_b_struct*_tmp362;struct Cyc_Absyn_Local_b_struct _tmp361;
struct Cyc_Tcenv_Bindings*_tmp360;struct Cyc_Tcenv_Bindings*_tmpFF=(_tmp360=
_region_malloc(r,sizeof(*_tmp360)),((_tmp360->v=_tmpED,((_tmp360->b=(void*)((
_tmp362=_cycalloc(sizeof(*_tmp362)),((_tmp362[0]=((_tmp361.tag=4,((_tmp361.f1=vd,
_tmp361)))),_tmp362)))),((_tmp360->tl=(struct Cyc_Tcenv_Bindings*)_tmpF9,_tmp360)))))));
struct Cyc_Tcenv_Fenv*_tmp363;struct Cyc_Tcenv_Fenv*_tmp100=(_tmp363=
_region_malloc(r,sizeof(*_tmp363)),((_tmp363->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpF6,((_tmp363->type_vars=(struct Cyc_List_List*)_tmpF7,((_tmp363->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpF8,((_tmp363->locals=(struct Cyc_Tcenv_Bindings*)
_tmpFF,((_tmp363->encloser=(struct Cyc_Absyn_Stmt*)_tmpFA,((_tmp363->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpFB),((_tmp363->capability=(
void*)_tmpFC,((_tmp363->curr_rgn=(void*)_tmpFD,((_tmp363->in_notreadctxt=(int)
_tmpFE,((_tmp363->fnrgn=(struct _RegionHandle*)r,_tmp363)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp100);};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp105=te->le;
if(_tmp105 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmp107;struct Cyc_List_List*_tmp108;struct Cyc_RgnOrder_RgnPO*_tmp109;struct Cyc_Tcenv_Bindings*
_tmp10A;struct Cyc_Absyn_Stmt*_tmp10B;struct Cyc_Tcenv_CtrlEnv*_tmp10C;void*
_tmp10D;void*_tmp10E;int _tmp10F;struct Cyc_Tcenv_Fenv _tmp106=*_tmp105;_tmp107=
_tmp106.shared;_tmp108=_tmp106.type_vars;_tmp109=_tmp106.region_order;_tmp10A=
_tmp106.locals;_tmp10B=_tmp106.encloser;_tmp10C=_tmp106.ctrl_env;_tmp10D=_tmp106.capability;
_tmp10E=_tmp106.curr_rgn;_tmp10F=_tmp106.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmp364;struct Cyc_Tcenv_Fenv*_tmp110=(_tmp364=_region_malloc(r,sizeof(*_tmp364)),((
_tmp364->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp107,((_tmp364->type_vars=(
struct Cyc_List_List*)_tmp108,((_tmp364->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp109,((_tmp364->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp10A),((_tmp364->encloser=(struct Cyc_Absyn_Stmt*)_tmp10B,((_tmp364->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp10C),((_tmp364->capability=(
void*)_tmp10D,((_tmp364->curr_rgn=(void*)_tmp10E,((_tmp364->in_notreadctxt=(int)
1,((_tmp364->fnrgn=(struct _RegionHandle*)r,_tmp364)))))))))))))))))))));return
Cyc_Tcenv_put_fenv(r,te,_tmp110);};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp112=te->le;
if(_tmp112 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmp114;struct Cyc_List_List*_tmp115;struct Cyc_RgnOrder_RgnPO*_tmp116;struct Cyc_Tcenv_Bindings*
_tmp117;struct Cyc_Absyn_Stmt*_tmp118;struct Cyc_Tcenv_CtrlEnv*_tmp119;void*
_tmp11A;void*_tmp11B;int _tmp11C;struct Cyc_Tcenv_Fenv _tmp113=*_tmp112;_tmp114=
_tmp113.shared;_tmp115=_tmp113.type_vars;_tmp116=_tmp113.region_order;_tmp117=
_tmp113.locals;_tmp118=_tmp113.encloser;_tmp119=_tmp113.ctrl_env;_tmp11A=_tmp113.capability;
_tmp11B=_tmp113.curr_rgn;_tmp11C=_tmp113.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmp365;struct Cyc_Tcenv_Fenv*_tmp11D=(_tmp365=_region_malloc(r,sizeof(*_tmp365)),((
_tmp365->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp114,((_tmp365->type_vars=(
struct Cyc_List_List*)_tmp115,((_tmp365->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp116,((_tmp365->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp117),((_tmp365->encloser=(struct Cyc_Absyn_Stmt*)_tmp118,((_tmp365->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp119),((_tmp365->capability=(
void*)_tmp11A,((_tmp365->curr_rgn=(void*)_tmp11B,((_tmp365->in_notreadctxt=(int)
0,((_tmp365->fnrgn=(struct _RegionHandle*)r,_tmp365)))))))))))))))))))));return
Cyc_Tcenv_put_fenv(r,te,_tmp11D);};};}int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*
te);int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
_tmp11F=te->le;if(_tmp11F == 0)return 0;{struct Cyc_Tcenv_Fenv _tmp121;int _tmp122;
struct Cyc_Tcenv_Fenv*_tmp120=(struct Cyc_Tcenv_Fenv*)_tmp11F;_tmp121=*_tmp120;
_tmp122=_tmp121.in_notreadctxt;return _tmp122;};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Vardecl*vd);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*
vd){struct _dyneither_ptr*_tmp123=(*vd->name).f2;struct Cyc_Tcenv_Fenv _tmp126;
struct Cyc_Tcenv_SharedFenv*_tmp127;struct Cyc_List_List*_tmp128;struct Cyc_RgnOrder_RgnPO*
_tmp129;struct Cyc_Tcenv_Bindings*_tmp12A;struct Cyc_Absyn_Stmt*_tmp12B;struct Cyc_Tcenv_CtrlEnv*
_tmp12C;void*_tmp12D;void*_tmp12E;int _tmp12F;const char*_tmp366;struct Cyc_Tcenv_Fenv*
_tmp125=Cyc_Tcenv_get_fenv(te,((_tmp366="add_pat_var",_tag_dyneither(_tmp366,
sizeof(char),12))));_tmp126=*_tmp125;_tmp127=_tmp126.shared;_tmp128=_tmp126.type_vars;
_tmp129=_tmp126.region_order;_tmp12A=_tmp126.locals;_tmp12B=_tmp126.encloser;
_tmp12C=_tmp126.ctrl_env;_tmp12D=_tmp126.capability;_tmp12E=_tmp126.curr_rgn;
_tmp12F=_tmp126.in_notreadctxt;{struct Cyc_Absyn_Pat_b_struct*_tmp36C;struct Cyc_Absyn_Pat_b_struct
_tmp36B;struct Cyc_Tcenv_Bindings*_tmp36A;struct Cyc_Tcenv_Bindings*_tmp130=(
_tmp36A=_region_malloc(r,sizeof(*_tmp36A)),((_tmp36A->v=_tmp123,((_tmp36A->b=(
void*)((_tmp36C=_cycalloc(sizeof(*_tmp36C)),((_tmp36C[0]=((_tmp36B.tag=5,((
_tmp36B.f1=vd,_tmp36B)))),_tmp36C)))),((_tmp36A->tl=(struct Cyc_Tcenv_Bindings*)
_tmp12A,_tmp36A)))))));struct Cyc_Tcenv_Fenv*_tmp36D;struct Cyc_Tcenv_Fenv*_tmp131=(
_tmp36D=_region_malloc(r,sizeof(*_tmp36D)),((_tmp36D->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp127,((_tmp36D->type_vars=(struct Cyc_List_List*)_tmp128,((_tmp36D->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp129,((_tmp36D->locals=(struct Cyc_Tcenv_Bindings*)
_tmp130,((_tmp36D->encloser=(struct Cyc_Absyn_Stmt*)_tmp12B,((_tmp36D->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp12C),((_tmp36D->capability=(
void*)_tmp12D,((_tmp36D->curr_rgn=(void*)_tmp12E,((_tmp36D->in_notreadctxt=(int)
_tmp12F,((_tmp36D->fnrgn=(struct _RegionHandle*)r,_tmp36D)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp131);};}void*Cyc_Tcenv_lookup_ordinary(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct
_tuple0*q);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct Cyc_Tcenv_Fenv*_tmp136=
te->le;struct _tuple0 _tmp138;union Cyc_Absyn_Nmspace _tmp139;struct _dyneither_ptr*
_tmp13A;struct _tuple0*_tmp137=q;_tmp138=*_tmp137;_tmp139=_tmp138.f1;_tmp13A=
_tmp138.f2;{union Cyc_Absyn_Nmspace _tmp13B=_tmp139;int _tmp13C;struct Cyc_List_List*
_tmp13D;_LL55: if((_tmp13B.Loc_n).tag != 3)goto _LL57;_tmp13C=(int)(_tmp13B.Loc_n).val;
_LL56: if(_tmp136 == 0){struct Cyc_Dict_Absent_struct _tmp370;struct Cyc_Dict_Absent_struct*
_tmp36F;(int)_throw((void*)((_tmp36F=_cycalloc_atomic(sizeof(*_tmp36F)),((
_tmp36F[0]=((_tmp370.tag=Cyc_Dict_Absent,_tmp370)),_tmp36F)))));}goto _LL58;_LL57:
if((_tmp13B.Rel_n).tag != 1)goto _LL59;_tmp13D=(struct Cyc_List_List*)(_tmp13B.Rel_n).val;
if(_tmp13D != 0)goto _LL59;if(!(_tmp136 != 0))goto _LL59;_LL58: {struct Cyc_Tcenv_Fenv
_tmp141;struct Cyc_Tcenv_Bindings*_tmp142;struct Cyc_Tcenv_Fenv*_tmp140=(struct Cyc_Tcenv_Fenv*)
_tmp136;_tmp141=*_tmp140;_tmp142=_tmp141.locals;{struct _handler_cons _tmp143;
_push_handler(& _tmp143);{int _tmp145=0;if(setjmp(_tmp143.handler))_tmp145=1;if(!
_tmp145){{struct Cyc_Tcenv_VarRes_struct _tmp373;struct Cyc_Tcenv_VarRes_struct*
_tmp372;void*_tmp148=(void*)((_tmp372=_region_malloc(r,sizeof(*_tmp372)),((
_tmp372[0]=((_tmp373.tag=0,((_tmp373.f1=(void*)Cyc_Tcenv_lookup_binding(_tmp142,
_tmp13A),_tmp373)))),_tmp372))));_npop_handler(0);return _tmp148;};_pop_handler();}
else{void*_tmp144=(void*)_exn_thrown;void*_tmp14A=_tmp144;_LL5C: {struct Cyc_Tcenv_NoBinding_struct*
_tmp14B=(struct Cyc_Tcenv_NoBinding_struct*)_tmp14A;if(_tmp14B->tag != Cyc_Tcenv_NoBinding)
goto _LL5E;}_LL5D: return(void*)Cyc_Tcenv_lookup_ordinary_global(te,loc,q);_LL5E:;
_LL5F:(void)_throw(_tmp14A);_LL5B:;}};};}_LL59:;_LL5A: {struct _handler_cons
_tmp14C;_push_handler(& _tmp14C);{int _tmp14E=0;if(setjmp(_tmp14C.handler))_tmp14E=
1;if(!_tmp14E){{void*_tmp14F=(void*)Cyc_Tcenv_lookup_ordinary_global(te,loc,q);
_npop_handler(0);return _tmp14F;};_pop_handler();}else{void*_tmp14D=(void*)
_exn_thrown;void*_tmp151=_tmp14D;_LL61: {struct Cyc_Dict_Absent_struct*_tmp152=(
struct Cyc_Dict_Absent_struct*)_tmp151;if(_tmp152->tag != Cyc_Dict_Absent)goto
_LL63;}_LL62: {struct Cyc_Tcenv_VarRes_struct _tmp376;struct Cyc_Tcenv_VarRes_struct*
_tmp375;return(void*)((_tmp375=_region_malloc(r,sizeof(*_tmp375)),((_tmp375[0]=((
_tmp376.tag=0,((_tmp376.f1=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),_tmp376)))),
_tmp375))));}_LL63:;_LL64:(void)_throw(_tmp151);_LL60:;}};}_LL54:;};}void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt);void
Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct
Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv _tmp157;struct Cyc_Tcenv_CtrlEnv*
_tmp158;const char*_tmp377;struct Cyc_Tcenv_Fenv*_tmp156=Cyc_Tcenv_get_fenv(te,((
_tmp377="process_continue",_tag_dyneither(_tmp377,sizeof(char),17))));_tmp157=*
_tmp156;_tmp158=_tmp157.ctrl_env;{void*_tmp159=_tmp158->continue_stmt;struct Cyc_Absyn_Stmt*
_tmp15B;_LL66: {struct Cyc_Tcenv_Stmt_j_struct*_tmp15A=(struct Cyc_Tcenv_Stmt_j_struct*)
_tmp159;if(_tmp15A->tag != 3)goto _LL68;else{_tmp15B=_tmp15A->f1;}}_LL67:{struct
Cyc_List_List*_tmp378;_tmp15B->non_local_preds=((_tmp378=_cycalloc(sizeof(*
_tmp378)),((_tmp378->hd=s,((_tmp378->tl=_tmp15B->non_local_preds,_tmp378))))));}*
sopt=(struct Cyc_Absyn_Stmt*)_tmp15B;return;_LL68: {struct Cyc_Tcenv_NotLoop_j_struct*
_tmp15C=(struct Cyc_Tcenv_NotLoop_j_struct*)_tmp159;if(_tmp15C->tag != 0)goto _LL6A;}
_LL69:{const char*_tmp37B;void*_tmp37A;(_tmp37A=0,Cyc_Tcutil_terr(s->loc,((
_tmp37B="continue not in a loop",_tag_dyneither(_tmp37B,sizeof(char),23))),
_tag_dyneither(_tmp37A,sizeof(void*),0)));}return;_LL6A: {struct Cyc_Tcenv_CaseEnd_j_struct*
_tmp15D=(struct Cyc_Tcenv_CaseEnd_j_struct*)_tmp159;if(_tmp15D->tag != 1)goto _LL6C;}
_LL6B: goto _LL6D;_LL6C: {struct Cyc_Tcenv_FnEnd_j_struct*_tmp15E=(struct Cyc_Tcenv_FnEnd_j_struct*)
_tmp159;if(_tmp15E->tag != 2)goto _LL65;}_LL6D: {const char*_tmp37C;((int(*)(struct
_dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp37C="bad continue destination",
_tag_dyneither(_tmp37C,sizeof(char),25))));}_LL65:;};}void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt);void
Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**
sopt){struct Cyc_Tcenv_Fenv _tmp165;struct Cyc_Tcenv_CtrlEnv*_tmp166;struct Cyc_Tcenv_SharedFenv*
_tmp167;const char*_tmp37D;struct Cyc_Tcenv_Fenv*_tmp164=Cyc_Tcenv_get_fenv(te,((
_tmp37D="process_break",_tag_dyneither(_tmp37D,sizeof(char),14))));_tmp165=*
_tmp164;_tmp166=_tmp165.ctrl_env;_tmp167=_tmp165.shared;{void*_tmp168=_tmp166->break_stmt;
struct Cyc_Absyn_Stmt*_tmp16A;_LL6F: {struct Cyc_Tcenv_Stmt_j_struct*_tmp169=(
struct Cyc_Tcenv_Stmt_j_struct*)_tmp168;if(_tmp169->tag != 3)goto _LL71;else{
_tmp16A=_tmp169->f1;}}_LL70:{struct Cyc_List_List*_tmp37E;_tmp16A->non_local_preds=((
_tmp37E=_cycalloc(sizeof(*_tmp37E)),((_tmp37E->hd=s,((_tmp37E->tl=_tmp16A->non_local_preds,
_tmp37E))))));}*sopt=(struct Cyc_Absyn_Stmt*)_tmp16A;return;_LL71: {struct Cyc_Tcenv_NotLoop_j_struct*
_tmp16B=(struct Cyc_Tcenv_NotLoop_j_struct*)_tmp168;if(_tmp16B->tag != 0)goto _LL73;}
_LL72:{const char*_tmp381;void*_tmp380;(_tmp380=0,Cyc_Tcutil_terr(s->loc,((
_tmp381="break not in a loop or switch",_tag_dyneither(_tmp381,sizeof(char),30))),
_tag_dyneither(_tmp380,sizeof(void*),0)));}return;_LL73: {struct Cyc_Tcenv_FnEnd_j_struct*
_tmp16C=(struct Cyc_Tcenv_FnEnd_j_struct*)_tmp168;if(_tmp16C->tag != 2)goto _LL75;}
_LL74:{void*_tmp171=Cyc_Tcutil_compress(_tmp167->return_typ);_LL78: {struct Cyc_Absyn_VoidType_struct*
_tmp172=(struct Cyc_Absyn_VoidType_struct*)_tmp171;if(_tmp172->tag != 0)goto _LL7A;}
_LL79: goto _LL77;_LL7A: {struct Cyc_Absyn_DoubleType_struct*_tmp173=(struct Cyc_Absyn_DoubleType_struct*)
_tmp171;if(_tmp173->tag != 8)goto _LL7C;}_LL7B: goto _LL7D;_LL7C: {struct Cyc_Absyn_FloatType_struct*
_tmp174=(struct Cyc_Absyn_FloatType_struct*)_tmp171;if(_tmp174->tag != 7)goto _LL7E;}
_LL7D: goto _LL7F;_LL7E: {struct Cyc_Absyn_IntType_struct*_tmp175=(struct Cyc_Absyn_IntType_struct*)
_tmp171;if(_tmp175->tag != 6)goto _LL80;}_LL7F:{const char*_tmp384;void*_tmp383;(
_tmp383=0,Cyc_Tcutil_warn(s->loc,((_tmp384="break may cause function not to return a value",
_tag_dyneither(_tmp384,sizeof(char),47))),_tag_dyneither(_tmp383,sizeof(void*),0)));}
goto _LL77;_LL80:;_LL81:{const char*_tmp387;void*_tmp386;(_tmp386=0,Cyc_Tcutil_terr(
s->loc,((_tmp387="break may cause function not to return a value",_tag_dyneither(
_tmp387,sizeof(char),47))),_tag_dyneither(_tmp386,sizeof(void*),0)));}goto _LL77;
_LL77:;}return;_LL75: {struct Cyc_Tcenv_CaseEnd_j_struct*_tmp16D=(struct Cyc_Tcenv_CaseEnd_j_struct*)
_tmp168;if(_tmp16D->tag != 1)goto _LL6E;}_LL76:{const char*_tmp38A;void*_tmp389;(
_tmp389=0,Cyc_Tcutil_terr(s->loc,((_tmp38A="break causes outer switch clause to implicitly fallthru",
_tag_dyneither(_tmp38A,sizeof(char),56))),_tag_dyneither(_tmp389,sizeof(void*),0)));}
return;_LL6E:;};}void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt);void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**
sopt){struct Cyc_Tcenv_Fenv _tmp17E;struct Cyc_Tcenv_SharedFenv*_tmp17F;const char*
_tmp38B;struct Cyc_Tcenv_Fenv*_tmp17D=Cyc_Tcenv_get_fenv(te,((_tmp38B="process_goto",
_tag_dyneither(_tmp38B,sizeof(char),13))));_tmp17E=*_tmp17D;_tmp17F=_tmp17E.shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp17F->seen_labels,l);if(sopt2 == 
0){struct Cyc_Dict_Dict _tmp180=_tmp17F->needed_labels;struct Cyc_List_List**slopt=((
struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
_tmp180,l);struct _RegionHandle*frgn=_tmp17F->frgn;if(slopt == 0){struct Cyc_List_List**
_tmp38C;slopt=((_tmp38C=_region_malloc(frgn,sizeof(*_tmp38C)),((_tmp38C[0]=0,
_tmp38C))));}{struct Cyc_List_List*_tmp38D;struct Cyc_List_List*new_needed=(
_tmp38D=_cycalloc(sizeof(*_tmp38D)),((_tmp38D->hd=s,((_tmp38D->tl=*slopt,_tmp38D)))));
_tmp17F->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp180,l,new_needed);};}
else{struct Cyc_Absyn_Stmt*s=*sopt2;{struct Cyc_List_List*_tmp38E;s->non_local_preds=((
_tmp38E=_cycalloc(sizeof(*_tmp38E)),((_tmp38E->hd=s,((_tmp38E->tl=s->non_local_preds,
_tmp38E))))));}*sopt=(struct Cyc_Absyn_Stmt*)s;}};}struct _tuple9*Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***
clauseopt);struct _tuple9*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){struct Cyc_Tcenv_Fenv
_tmp186;struct Cyc_Tcenv_CtrlEnv*_tmp187;const char*_tmp38F;struct Cyc_Tcenv_Fenv*
_tmp185=Cyc_Tcenv_get_fenv(te,((_tmp38F="process_fallthru",_tag_dyneither(
_tmp38F,sizeof(char),17))));_tmp186=*_tmp185;_tmp187=_tmp186.ctrl_env;{struct
_tuple9*_tmp188=(struct _tuple9*)_tmp187->fallthru_clause;if(_tmp188 != 0){{struct
Cyc_List_List*_tmp390;(((*_tmp188).f1)->body)->non_local_preds=((_tmp390=
_cycalloc(sizeof(*_tmp390)),((_tmp390->hd=s,((_tmp390->tl=(((*_tmp188).f1)->body)->non_local_preds,
_tmp390))))));}{struct Cyc_Absyn_Switch_clause**_tmp391;*clauseopt=(struct Cyc_Absyn_Switch_clause**)((
_tmp391=_cycalloc(sizeof(*_tmp391)),((_tmp391[0]=(*_tmp188).f1,_tmp391))));};}
return _tmp188;};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,
struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){struct Cyc_Tcenv_Fenv
_tmp18D;struct Cyc_Tcenv_SharedFenv*_tmp18E;struct Cyc_List_List*_tmp18F;struct Cyc_RgnOrder_RgnPO*
_tmp190;struct Cyc_Tcenv_Bindings*_tmp191;struct Cyc_Absyn_Stmt*_tmp192;struct Cyc_Tcenv_CtrlEnv*
_tmp193;void*_tmp194;void*_tmp195;int _tmp196;const char*_tmp392;struct Cyc_Tcenv_Fenv*
_tmp18C=Cyc_Tcenv_get_fenv(te,((_tmp392="set_fallthru",_tag_dyneither(_tmp392,
sizeof(char),13))));_tmp18D=*_tmp18C;_tmp18E=_tmp18D.shared;_tmp18F=_tmp18D.type_vars;
_tmp190=_tmp18D.region_order;_tmp191=_tmp18D.locals;_tmp192=_tmp18D.encloser;
_tmp193=_tmp18D.ctrl_env;_tmp194=_tmp18D.capability;_tmp195=_tmp18D.curr_rgn;
_tmp196=_tmp18D.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv _tmp198;struct
_RegionHandle*_tmp199;void*_tmp19A;void*_tmp19B;struct _tuple9*_tmp19C;void*
_tmp19D;int _tmp19E;struct Cyc_Tcenv_CtrlEnv*_tmp197=_tmp193;_tmp198=*_tmp197;
_tmp199=_tmp198.ctrl_rgn;_tmp19A=_tmp198.continue_stmt;_tmp19B=_tmp198.break_stmt;
_tmp19C=_tmp198.fallthru_clause;_tmp19D=_tmp198.next_stmt;_tmp19E=_tmp198.try_depth;{
struct Cyc_List_List*ft_typ=0;for(0;vds != 0;vds=vds->tl){struct Cyc_List_List*
_tmp393;ft_typ=((_tmp393=_region_malloc(_tmp199,sizeof(*_tmp393)),((_tmp393->hd=((
struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp393->tl=ft_typ,_tmp393))))));}{
struct Cyc_Tcenv_CList*_tmp1A0=(struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);struct _tuple9*_tmp396;struct Cyc_Tcenv_CtrlEnv*
_tmp395;struct Cyc_Tcenv_CtrlEnv*_tmp1A1=(_tmp395=_region_malloc(r,sizeof(*
_tmp395)),((_tmp395->ctrl_rgn=_tmp199,((_tmp395->continue_stmt=_tmp19A,((_tmp395->break_stmt=
_tmp19B,((_tmp395->fallthru_clause=((_tmp396=_region_malloc(_tmp199,sizeof(*
_tmp396)),((_tmp396->f1=clause,((_tmp396->f2=new_tvs,((_tmp396->f3=_tmp1A0,
_tmp396)))))))),((_tmp395->next_stmt=_tmp19D,((_tmp395->try_depth=_tmp19E,
_tmp395)))))))))))));struct Cyc_Tcenv_Fenv*_tmp397;struct Cyc_Tcenv_Fenv*_tmp1A2=(
_tmp397=_region_malloc(r,sizeof(*_tmp397)),((_tmp397->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp18E,((_tmp397->type_vars=(struct Cyc_List_List*)_tmp18F,((_tmp397->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp190,((_tmp397->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp191),((_tmp397->encloser=(struct Cyc_Absyn_Stmt*)
_tmp192,((_tmp397->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1A1,((_tmp397->capability=(
void*)_tmp194,((_tmp397->curr_rgn=(void*)_tmp195,((_tmp397->in_notreadctxt=(int)
_tmp196,((_tmp397->fnrgn=(struct _RegionHandle*)r,_tmp397)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp1A2);};};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*_tmp398;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp398="clear_fallthru",
_tag_dyneither(_tmp398,sizeof(char),15)))));struct Cyc_Tcenv_Fenv _tmp1A7;struct
Cyc_Tcenv_CtrlEnv*_tmp1A8;struct Cyc_Tcenv_Fenv*_tmp1A6=fe;_tmp1A7=*_tmp1A6;
_tmp1A8=_tmp1A7.ctrl_env;_tmp1A8->fallthru_clause=0;return Cyc_Tcenv_put_fenv(r,
te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
struct Cyc_Tcenv_Fenv _tmp1AC;struct Cyc_Tcenv_SharedFenv*_tmp1AD;struct Cyc_List_List*
_tmp1AE;struct Cyc_RgnOrder_RgnPO*_tmp1AF;struct Cyc_Tcenv_Bindings*_tmp1B0;struct
Cyc_Absyn_Stmt*_tmp1B1;struct Cyc_Tcenv_CtrlEnv*_tmp1B2;void*_tmp1B3;void*_tmp1B4;
int _tmp1B5;const char*_tmp399;struct Cyc_Tcenv_Fenv*_tmp1AB=Cyc_Tcenv_get_fenv(te,((
_tmp399="set_in_loop",_tag_dyneither(_tmp399,sizeof(char),12))));_tmp1AC=*
_tmp1AB;_tmp1AD=_tmp1AC.shared;_tmp1AE=_tmp1AC.type_vars;_tmp1AF=_tmp1AC.region_order;
_tmp1B0=_tmp1AC.locals;_tmp1B1=_tmp1AC.encloser;_tmp1B2=_tmp1AC.ctrl_env;_tmp1B3=
_tmp1AC.capability;_tmp1B4=_tmp1AC.curr_rgn;_tmp1B5=_tmp1AC.in_notreadctxt;{
struct Cyc_Tcenv_Stmt_j_struct*_tmp3A4;struct Cyc_Tcenv_Stmt_j_struct _tmp3A3;
struct Cyc_Tcenv_Stmt_j_struct*_tmp3A2;struct Cyc_Tcenv_Stmt_j_struct _tmp3A1;
struct Cyc_Tcenv_CtrlEnv*_tmp3A0;struct Cyc_Tcenv_CtrlEnv*new_cenv=(_tmp3A0=
_region_malloc(r,sizeof(*_tmp3A0)),((_tmp3A0->ctrl_rgn=r,((_tmp3A0->continue_stmt=(
void*)((_tmp3A2=_region_malloc(r,sizeof(*_tmp3A2)),((_tmp3A2[0]=((_tmp3A1.tag=3,((
_tmp3A1.f1=continue_dest,_tmp3A1)))),_tmp3A2)))),((_tmp3A0->break_stmt=(void*)
_tmp1B2->next_stmt,((_tmp3A0->next_stmt=(void*)((_tmp3A4=_region_malloc(r,
sizeof(*_tmp3A4)),((_tmp3A4[0]=((_tmp3A3.tag=3,((_tmp3A3.f1=continue_dest,
_tmp3A3)))),_tmp3A4)))),((_tmp3A0->fallthru_clause=(struct _tuple9*)_tmp1B2->fallthru_clause,((
_tmp3A0->try_depth=_tmp1B2->try_depth,_tmp3A0)))))))))))));struct Cyc_Tcenv_Fenv*
_tmp3A5;struct Cyc_Tcenv_Fenv*new_fenv=(_tmp3A5=_region_malloc(r,sizeof(*_tmp3A5)),((
_tmp3A5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1AD,((_tmp3A5->type_vars=(
struct Cyc_List_List*)_tmp1AE,((_tmp3A5->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp1AF,((_tmp3A5->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp1B0),((_tmp3A5->encloser=(struct Cyc_Absyn_Stmt*)_tmp1B1,((_tmp3A5->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3A5->capability=(void*)_tmp1B3,((_tmp3A5->curr_rgn=(
void*)_tmp1B4,((_tmp3A5->in_notreadctxt=(int)_tmp1B5,((_tmp3A5->fnrgn=(struct
_RegionHandle*)r,_tmp3A5)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,
new_fenv);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*_tmp3A6;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3A6="enter_try",
_tag_dyneither(_tmp3A6,sizeof(char),10)))));struct Cyc_Tcenv_Fenv _tmp1BD;struct
Cyc_Tcenv_CtrlEnv*_tmp1BE;struct Cyc_Tcenv_Fenv*_tmp1BC=fe;_tmp1BD=*_tmp1BC;
_tmp1BE=_tmp1BD.ctrl_env;++ _tmp1BE->try_depth;return Cyc_Tcenv_put_fenv(r,te,fe);}
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp1C2;struct Cyc_Tcenv_CtrlEnv*
_tmp1C3;const char*_tmp3A7;struct Cyc_Tcenv_Fenv*_tmp1C1=Cyc_Tcenv_get_fenv(te,((
_tmp3A7="get_try_depth",_tag_dyneither(_tmp3A7,sizeof(char),14))));_tmp1C2=*
_tmp1C1;_tmp1C3=_tmp1C2.ctrl_env;return _tmp1C3->try_depth;}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*
_tmp3A8;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(
te,((_tmp3A8="set_in_switch",_tag_dyneither(_tmp3A8,sizeof(char),14)))));struct
Cyc_Tcenv_Fenv _tmp1C5;struct Cyc_Tcenv_CtrlEnv*_tmp1C6;struct Cyc_Tcenv_Fenv*
_tmp1C4=fe;_tmp1C5=*_tmp1C4;_tmp1C6=_tmp1C5.ctrl_env;_tmp1C6->break_stmt=_tmp1C6->next_stmt;
_tmp1C6->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;return Cyc_Tcenv_put_fenv(r,te,
fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,void*j);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te,void*j){struct Cyc_Tcenv_SharedFenv*_tmp1CA;struct Cyc_List_List*
_tmp1CB;struct Cyc_RgnOrder_RgnPO*_tmp1CC;struct Cyc_Tcenv_Bindings*_tmp1CD;struct
Cyc_Absyn_Stmt*_tmp1CE;struct Cyc_Tcenv_CtrlEnv*_tmp1CF;void*_tmp1D0;void*_tmp1D1;
int _tmp1D2;const char*_tmp3A9;struct Cyc_Tcenv_Fenv _tmp1C9=*Cyc_Tcenv_get_fenv(te,((
_tmp3A9="set_next",_tag_dyneither(_tmp3A9,sizeof(char),9))));_tmp1CA=_tmp1C9.shared;
_tmp1CB=_tmp1C9.type_vars;_tmp1CC=_tmp1C9.region_order;_tmp1CD=_tmp1C9.locals;
_tmp1CE=_tmp1C9.encloser;_tmp1CF=_tmp1C9.ctrl_env;_tmp1D0=_tmp1C9.capability;
_tmp1D1=_tmp1C9.curr_rgn;_tmp1D2=_tmp1C9.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv*
_tmp3AA;struct Cyc_Tcenv_CtrlEnv*new_cenv=(_tmp3AA=_region_malloc(r,sizeof(*
_tmp3AA)),((_tmp3AA->ctrl_rgn=r,((_tmp3AA->continue_stmt=(void*)_tmp1CF->continue_stmt,((
_tmp3AA->break_stmt=(void*)_tmp1CF->break_stmt,((_tmp3AA->next_stmt=j,((_tmp3AA->fallthru_clause=(
struct _tuple9*)_tmp1CF->fallthru_clause,((_tmp3AA->try_depth=_tmp1CF->try_depth,
_tmp3AA)))))))))))));struct Cyc_Tcenv_Fenv*_tmp3AB;struct Cyc_Tcenv_Fenv*new_fenv=(
_tmp3AB=_region_malloc(r,sizeof(*_tmp3AB)),((_tmp3AB->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1CA,((_tmp3AB->type_vars=(struct Cyc_List_List*)_tmp1CB,((_tmp3AB->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1CC,((_tmp3AB->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1CD),((_tmp3AB->encloser=(struct Cyc_Absyn_Stmt*)
_tmp1CE,((_tmp3AB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3AB->capability=(
void*)_tmp1D0,((_tmp3AB->curr_rgn=(void*)_tmp1D1,((_tmp3AB->in_notreadctxt=(int)
_tmp1D2,((_tmp3AB->fnrgn=(struct _RegionHandle*)r,_tmp3AB)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*
s){struct Cyc_Tcenv_Fenv _tmp1D7;struct Cyc_Tcenv_SharedFenv*_tmp1D8;const char*
_tmp3AC;struct Cyc_Tcenv_Fenv*_tmp1D6=Cyc_Tcenv_get_fenv(te,((_tmp3AC="add_label",
_tag_dyneither(_tmp3AC,sizeof(char),10))));_tmp1D7=*_tmp1D6;_tmp1D8=_tmp1D7.shared;{
struct Cyc_Dict_Dict needed=_tmp1D8->needed_labels;struct Cyc_List_List**sl_opt=((
struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
needed,v);struct _RegionHandle*frgn=_tmp1D8->frgn;if(sl_opt != 0){_tmp1D8->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct
_dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{struct Cyc_List_List*_tmp1D9=*
sl_opt;s->non_local_preds=_tmp1D9;for(0;_tmp1D9 != 0;_tmp1D9=_tmp1D9->tl){void*
_tmp1DA=((struct Cyc_Absyn_Stmt*)_tmp1D9->hd)->r;struct Cyc_Absyn_Stmt*_tmp1DC;
struct Cyc_Absyn_Stmt**_tmp1DD;_LL83: {struct Cyc_Absyn_Goto_s_struct*_tmp1DB=(
struct Cyc_Absyn_Goto_s_struct*)_tmp1DA;if(_tmp1DB->tag != 8)goto _LL85;else{
_tmp1DC=_tmp1DB->f2;_tmp1DD=(struct Cyc_Absyn_Stmt**)& _tmp1DB->f2;}}_LL84:*
_tmp1DD=(struct Cyc_Absyn_Stmt*)s;goto _LL82;_LL85:;_LL86:{const char*_tmp3AD;((int(*)(
struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3AD="Tcenv: add_label backpatching of non-goto",
_tag_dyneither(_tmp3AD,sizeof(char),42))));}goto _LL82;_LL82:;}};}if(((int(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp1D8->seen_labels,
v)){const char*_tmp3B1;void*_tmp3B0[1];struct Cyc_String_pa_struct _tmp3AF;(_tmp3AF.tag=
0,((_tmp3AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3B0[0]=&
_tmp3AF,Cyc_Tcutil_terr(s->loc,((_tmp3B1="Repeated label: %s",_tag_dyneither(
_tmp3B1,sizeof(char),19))),_tag_dyneither(_tmp3B0,sizeof(void*),1)))))));}
_tmp1D8->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp1D8->seen_labels,v,
s);return te;};}int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp1E4;struct Cyc_Tcenv_SharedFenv*
_tmp1E5;const char*_tmp3B2;struct Cyc_Tcenv_Fenv*_tmp1E3=Cyc_Tcenv_get_fenv(te,((
_tmp3B2="all_labels_resolved",_tag_dyneither(_tmp3B2,sizeof(char),20))));_tmp1E4=*
_tmp1E3;_tmp1E5=_tmp1E4.shared;return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(
_tmp1E5->needed_labels);}struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*
te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){struct
Cyc_Tcenv_Fenv _tmp1E8;struct Cyc_Absyn_Stmt*_tmp1E9;const char*_tmp3B3;struct Cyc_Tcenv_Fenv*
_tmp1E7=Cyc_Tcenv_get_fenv(te,((_tmp3B3="get_encloser",_tag_dyneither(_tmp3B3,
sizeof(char),13))));_tmp1E8=*_tmp1E7;_tmp1E9=_tmp1E8.encloser;return _tmp1E9;}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*
te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct
Cyc_Tcenv_Tenv*te){if(te->le == 0){struct Cyc_Tcenv_Tenv*_tmp3B4;return(_tmp3B4=
_region_malloc(r2,sizeof(*_tmp3B4)),((_tmp3B4->ns=te->ns,((_tmp3B4->ae=te->ae,((
_tmp3B4->le=0,((_tmp3B4->allow_valueof=1,_tmp3B4)))))))));}{struct Cyc_Tcenv_SharedFenv*
_tmp1EC;struct Cyc_List_List*_tmp1ED;struct Cyc_RgnOrder_RgnPO*_tmp1EE;struct Cyc_Tcenv_Bindings*
_tmp1EF;struct Cyc_Absyn_Stmt*_tmp1F0;struct Cyc_Tcenv_CtrlEnv*_tmp1F1;void*
_tmp1F2;void*_tmp1F3;int _tmp1F4;struct Cyc_Tcenv_Fenv _tmp1EB=*((struct Cyc_Tcenv_Fenv*)
_check_null(te->le));_tmp1EC=_tmp1EB.shared;_tmp1ED=_tmp1EB.type_vars;_tmp1EE=
_tmp1EB.region_order;_tmp1EF=_tmp1EB.locals;_tmp1F0=_tmp1EB.encloser;_tmp1F1=
_tmp1EB.ctrl_env;_tmp1F2=_tmp1EB.capability;_tmp1F3=_tmp1EB.curr_rgn;_tmp1F4=
_tmp1EB.in_notreadctxt;{struct Cyc_Tcenv_Fenv*_tmp3B5;struct Cyc_Tcenv_Fenv*
_tmp1F5=(_tmp3B5=_region_malloc(r2,sizeof(*_tmp3B5)),((_tmp3B5->shared=(struct
Cyc_Tcenv_SharedFenv*)_tmp1EC,((_tmp3B5->type_vars=(struct Cyc_List_List*)_tmp1ED,((
_tmp3B5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1EE,((_tmp3B5->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp1EF),((_tmp3B5->encloser=(
struct Cyc_Absyn_Stmt*)_tmp1F0,((_tmp3B5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp1F1),((_tmp3B5->capability=(void*)_tmp1F2,((_tmp3B5->curr_rgn=(
void*)_tmp1F3,((_tmp3B5->in_notreadctxt=(int)_tmp1F4,((_tmp3B5->fnrgn=(struct
_RegionHandle*)r2,_tmp3B5)))))))))))))))))))));struct Cyc_Tcenv_Tenv*_tmp3B6;
return(_tmp3B6=_region_malloc(r2,sizeof(*_tmp3B6)),((_tmp3B6->ns=te->ns,((
_tmp3B6->ae=te->ae,((_tmp3B6->le=_tmp1F5,((_tmp3B6->allow_valueof=1,_tmp3B6)))))))));};};}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_SharedFenv*
_tmp1FA;struct Cyc_List_List*_tmp1FB;struct Cyc_RgnOrder_RgnPO*_tmp1FC;struct Cyc_Tcenv_Bindings*
_tmp1FD;struct Cyc_Absyn_Stmt*_tmp1FE;struct Cyc_Tcenv_CtrlEnv*_tmp1FF;void*
_tmp200;void*_tmp201;int _tmp202;const char*_tmp3B7;struct Cyc_Tcenv_Fenv _tmp1F9=*
Cyc_Tcenv_get_fenv(te,((_tmp3B7="set_encloser",_tag_dyneither(_tmp3B7,sizeof(
char),13))));_tmp1FA=_tmp1F9.shared;_tmp1FB=_tmp1F9.type_vars;_tmp1FC=_tmp1F9.region_order;
_tmp1FD=_tmp1F9.locals;_tmp1FE=_tmp1F9.encloser;_tmp1FF=_tmp1F9.ctrl_env;_tmp200=
_tmp1F9.capability;_tmp201=_tmp1F9.curr_rgn;_tmp202=_tmp1F9.in_notreadctxt;{
struct Cyc_Tcenv_Fenv*_tmp3B8;struct Cyc_Tcenv_Fenv*_tmp203=(_tmp3B8=
_region_malloc(r,sizeof(*_tmp3B8)),((_tmp3B8->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1FA,((_tmp3B8->type_vars=(struct Cyc_List_List*)_tmp1FB,((_tmp3B8->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1FC,((_tmp3B8->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1FD),((_tmp3B8->encloser=(struct Cyc_Absyn_Stmt*)s,((
_tmp3B8->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp1FF),((
_tmp3B8->capability=(void*)_tmp200,((_tmp3B8->curr_rgn=(void*)_tmp201,((_tmp3B8->in_notreadctxt=(
int)_tmp202,((_tmp3B8->fnrgn=(struct _RegionHandle*)r,_tmp3B8)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp203);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,void*rgn,int resetable,int opened){struct Cyc_Absyn_Tvar*tv;{void*_tmp205=Cyc_Tcutil_compress(
rgn);struct Cyc_Absyn_Tvar*_tmp207;_LL88: {struct Cyc_Absyn_VarType_struct*_tmp206=(
struct Cyc_Absyn_VarType_struct*)_tmp205;if(_tmp206->tag != 2)goto _LL8A;else{
_tmp207=_tmp206->f1;}}_LL89: tv=_tmp207;goto _LL87;_LL8A:;_LL8B:{const char*_tmp3B9;
tv=((struct Cyc_Absyn_Tvar*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((
_tmp3B9="bad add region",_tag_dyneither(_tmp3B9,sizeof(char),15))));}goto _LL87;
_LL87:;}{struct Cyc_Tcenv_SharedFenv*_tmp20B;struct Cyc_List_List*_tmp20C;struct
Cyc_RgnOrder_RgnPO*_tmp20D;struct Cyc_Tcenv_Bindings*_tmp20E;struct Cyc_Absyn_Stmt*
_tmp20F;struct Cyc_Tcenv_CtrlEnv*_tmp210;void*_tmp211;void*_tmp212;int _tmp213;
const char*_tmp3BA;struct Cyc_Tcenv_Fenv _tmp20A=*Cyc_Tcenv_get_fenv(te,((_tmp3BA="add_region",
_tag_dyneither(_tmp3BA,sizeof(char),11))));_tmp20B=_tmp20A.shared;_tmp20C=
_tmp20A.type_vars;_tmp20D=_tmp20A.region_order;_tmp20E=_tmp20A.locals;_tmp20F=
_tmp20A.encloser;_tmp210=_tmp20A.ctrl_env;_tmp211=_tmp20A.capability;_tmp212=
_tmp20A.curr_rgn;_tmp213=_tmp20A.in_notreadctxt;_tmp20D=Cyc_RgnOrder_add_youngest(
_tmp20B->frgn,_tmp20D,tv,resetable,opened);{struct Cyc_Absyn_JoinEff_struct
_tmp3CD;struct Cyc_List_List*_tmp3CC;struct Cyc_Absyn_AccessEff_struct*_tmp3CB;
struct Cyc_Absyn_AccessEff_struct _tmp3CA;struct Cyc_List_List*_tmp3C9;struct Cyc_Absyn_JoinEff_struct*
_tmp3C8;_tmp211=(void*)((_tmp3C8=_cycalloc(sizeof(*_tmp3C8)),((_tmp3C8[0]=((
_tmp3CD.tag=24,((_tmp3CD.f1=((_tmp3C9=_cycalloc(sizeof(*_tmp3C9)),((_tmp3C9->hd=(
void*)((_tmp3CB=_cycalloc(sizeof(*_tmp3CB)),((_tmp3CB[0]=((_tmp3CA.tag=23,((
_tmp3CA.f1=(void*)rgn,_tmp3CA)))),_tmp3CB)))),((_tmp3C9->tl=((_tmp3CC=_cycalloc(
sizeof(*_tmp3CC)),((_tmp3CC->hd=_tmp211,((_tmp3CC->tl=0,_tmp3CC)))))),_tmp3C9)))))),
_tmp3CD)))),_tmp3C8))));}{struct Cyc_Tcenv_Fenv*_tmp3CE;struct Cyc_Tcenv_Fenv*
_tmp21A=(_tmp3CE=_region_malloc(r,sizeof(*_tmp3CE)),((_tmp3CE->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp20B,((_tmp3CE->type_vars=(struct Cyc_List_List*)_tmp20C,((_tmp3CE->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp20D,((_tmp3CE->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp20E),((_tmp3CE->encloser=(struct Cyc_Absyn_Stmt*)
_tmp20F,((_tmp3CE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp210),((_tmp3CE->capability=(void*)_tmp211,((_tmp3CE->curr_rgn=(void*)_tmp212,((
_tmp3CE->in_notreadctxt=(int)_tmp213,((_tmp3CE->fnrgn=(struct _RegionHandle*)r,
_tmp3CE)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmp21A);};};}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_named_block(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){struct Cyc_Tcenv_SharedFenv*
_tmp21E;struct Cyc_List_List*_tmp21F;struct Cyc_RgnOrder_RgnPO*_tmp220;struct Cyc_Tcenv_Bindings*
_tmp221;struct Cyc_Absyn_Stmt*_tmp222;struct Cyc_Tcenv_CtrlEnv*_tmp223;void*
_tmp224;void*_tmp225;int _tmp226;const char*_tmp3CF;struct Cyc_Tcenv_Fenv _tmp21D=*
Cyc_Tcenv_get_fenv(te,((_tmp3CF="new_named_block",_tag_dyneither(_tmp3CF,sizeof(
char),16))));_tmp21E=_tmp21D.shared;_tmp21F=_tmp21D.type_vars;_tmp220=_tmp21D.region_order;
_tmp221=_tmp21D.locals;_tmp222=_tmp21D.encloser;_tmp223=_tmp21D.ctrl_env;_tmp224=
_tmp21D.capability;_tmp225=_tmp21D.curr_rgn;_tmp226=_tmp21D.in_notreadctxt;{
const char*_tmp3D0;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(
te,((_tmp3D0="new_block",_tag_dyneither(_tmp3D0,sizeof(char),10)))));struct Cyc_Absyn_VarType_struct
_tmp3D3;struct Cyc_Absyn_VarType_struct*_tmp3D2;void*block_typ=(void*)((_tmp3D2=
_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2[0]=((_tmp3D3.tag=2,((_tmp3D3.f1=block_rgn,
_tmp3D3)))),_tmp3D2))));{struct Cyc_List_List*_tmp3D4;_tmp21F=((_tmp3D4=_cycalloc(
sizeof(*_tmp3D4)),((_tmp3D4->hd=block_rgn,((_tmp3D4->tl=_tmp21F,_tmp3D4))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp21F);_tmp220=Cyc_RgnOrder_add_youngest(
_tmp21E->frgn,_tmp220,block_rgn,0,0);{struct Cyc_Absyn_JoinEff_struct _tmp3E7;
struct Cyc_List_List*_tmp3E6;struct Cyc_Absyn_AccessEff_struct*_tmp3E5;struct Cyc_Absyn_AccessEff_struct
_tmp3E4;struct Cyc_List_List*_tmp3E3;struct Cyc_Absyn_JoinEff_struct*_tmp3E2;
_tmp224=(void*)((_tmp3E2=_cycalloc(sizeof(*_tmp3E2)),((_tmp3E2[0]=((_tmp3E7.tag=
24,((_tmp3E7.f1=((_tmp3E3=_cycalloc(sizeof(*_tmp3E3)),((_tmp3E3->hd=(void*)((
_tmp3E5=_cycalloc(sizeof(*_tmp3E5)),((_tmp3E5[0]=((_tmp3E4.tag=23,((_tmp3E4.f1=(
void*)block_typ,_tmp3E4)))),_tmp3E5)))),((_tmp3E3->tl=((_tmp3E6=_cycalloc(
sizeof(*_tmp3E6)),((_tmp3E6->hd=_tmp224,((_tmp3E6->tl=0,_tmp3E6)))))),_tmp3E3)))))),
_tmp3E7)))),_tmp3E2))));}_tmp225=block_typ;{struct Cyc_Tcenv_Fenv*_tmp3E8;struct
Cyc_Tcenv_Fenv*_tmp22E=(_tmp3E8=_region_malloc(r,sizeof(*_tmp3E8)),((_tmp3E8->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmp21E,((_tmp3E8->type_vars=(struct Cyc_List_List*)
_tmp21F,((_tmp3E8->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp220,((_tmp3E8->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp221),((_tmp3E8->encloser=(
struct Cyc_Absyn_Stmt*)_tmp222,((_tmp3E8->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp223),((_tmp3E8->capability=(void*)_tmp224,((_tmp3E8->curr_rgn=(
void*)_tmp225,((_tmp3E8->in_notreadctxt=(int)_tmp226,((_tmp3E8->fnrgn=(struct
_RegionHandle*)r,_tmp3E8)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,
_tmp22E);};};}static struct Cyc_Absyn_Eq_kb_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te){struct
Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);Cyc_Tcutil_add_tvar_identity(
t);return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple12{void*f1;void*f2;}
;struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*loc);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*loc){
struct Cyc_Tcenv_SharedFenv*_tmp236;struct Cyc_List_List*_tmp237;struct Cyc_RgnOrder_RgnPO*
_tmp238;struct Cyc_Tcenv_Bindings*_tmp239;struct Cyc_Absyn_Stmt*_tmp23A;struct Cyc_Tcenv_CtrlEnv*
_tmp23B;void*_tmp23C;void*_tmp23D;int _tmp23E;const char*_tmp3E9;struct Cyc_Tcenv_Fenv
_tmp235=*Cyc_Tcenv_get_fenv(te,((_tmp3E9="new_outlives_constraints",
_tag_dyneither(_tmp3E9,sizeof(char),25))));_tmp236=_tmp235.shared;_tmp237=
_tmp235.type_vars;_tmp238=_tmp235.region_order;_tmp239=_tmp235.locals;_tmp23A=
_tmp235.encloser;_tmp23B=_tmp235.ctrl_env;_tmp23C=_tmp235.capability;_tmp23D=
_tmp235.curr_rgn;_tmp23E=_tmp235.in_notreadctxt;for(0;cs != 0;cs=cs->tl){_tmp238=
Cyc_RgnOrder_add_outlives_constraint(_tmp236->frgn,_tmp238,(*((struct _tuple12*)
cs->hd)).f1,(*((struct _tuple12*)cs->hd)).f2,loc);}{struct Cyc_Tcenv_Fenv*_tmp3EA;
struct Cyc_Tcenv_Fenv*_tmp23F=(_tmp3EA=_region_malloc(r,sizeof(*_tmp3EA)),((
_tmp3EA->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp236,((_tmp3EA->type_vars=(
struct Cyc_List_List*)_tmp237,((_tmp3EA->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp238,((_tmp3EA->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp239),((_tmp3EA->encloser=(struct Cyc_Absyn_Stmt*)_tmp23A,((_tmp3EA->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp23B),((_tmp3EA->capability=(
void*)_tmp23C,((_tmp3EA->curr_rgn=(void*)_tmp23D,((_tmp3EA->in_notreadctxt=(int)
_tmp23E,((_tmp3EA->fnrgn=(struct _RegionHandle*)r,_tmp3EA)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp23F);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple10**
oldtv,struct Cyc_Position_Segment*loc);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple10**
oldtv,struct Cyc_Position_Segment*loc){void*_tmp241=Cyc_Tcutil_compress(r1);void*
_tmp242=Cyc_Tcutil_compress(r2);struct Cyc_Absyn_Kind*_tmp243=Cyc_Tcutil_typ_kind(
_tmp241);struct Cyc_Absyn_Kind*_tmp244=Cyc_Tcutil_typ_kind(_tmp242);int r1_le_r2=
Cyc_Tcutil_kind_leq(_tmp243,_tmp244);int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp244,
_tmp243);if(!r1_le_r2  && !r2_le_r1){{const char*_tmp3F1;void*_tmp3F0[4];struct Cyc_String_pa_struct
_tmp3EF;struct Cyc_String_pa_struct _tmp3EE;struct Cyc_String_pa_struct _tmp3ED;
struct Cyc_String_pa_struct _tmp3EC;(_tmp3EC.tag=0,((_tmp3EC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp244)),((
_tmp3ED.tag=0,((_tmp3ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp243)),((_tmp3EE.tag=0,((_tmp3EE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp242)),((_tmp3EF.tag=0,((_tmp3EF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp241)),((
_tmp3F0[0]=& _tmp3EF,((_tmp3F0[1]=& _tmp3EE,((_tmp3F0[2]=& _tmp3ED,((_tmp3F0[3]=&
_tmp3EC,Cyc_Tcutil_terr(loc,((_tmp3F1="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",
_tag_dyneither(_tmp3F1,sizeof(char),82))),_tag_dyneither(_tmp3F0,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{if(r1_le_r2  && !
r2_le_r1){struct Cyc_Absyn_Tvar*_tmp24C;void*_tmp24D;struct _tuple10 _tmp24B=Cyc_Tcutil_swap_kind(
_tmp242,Cyc_Tcutil_kind_to_bound(_tmp243));_tmp24C=_tmp24B.f1;_tmp24D=_tmp24B.f2;{
struct _tuple10*_tmp3F2;*oldtv=((_tmp3F2=_region_malloc(r,sizeof(*_tmp3F2)),((
_tmp3F2->f1=_tmp24C,((_tmp3F2->f2=_tmp24D,_tmp3F2))))));};}else{if(!r1_le_r2  && 
r2_le_r1){struct Cyc_Absyn_Tvar*_tmp250;void*_tmp251;struct _tuple10 _tmp24F=Cyc_Tcutil_swap_kind(
_tmp241,Cyc_Tcutil_kind_to_bound(_tmp244));_tmp250=_tmp24F.f1;_tmp251=_tmp24F.f2;{
struct _tuple10*_tmp3F3;*oldtv=((_tmp3F3=_region_malloc(r,sizeof(*_tmp3F3)),((
_tmp3F3->f1=_tmp250,((_tmp3F3->f2=_tmp251,_tmp3F3))))));};}}}{struct
_RegionHandle*_tmp253=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp254=0;if(
_tmp241 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp241 != (void*)& Cyc_Absyn_UniqueRgn_val){
struct Cyc_Absyn_AccessEff_struct _tmp3F6;struct Cyc_Absyn_AccessEff_struct*_tmp3F5;
void*eff1=(void*)((_tmp3F5=_cycalloc(sizeof(*_tmp3F5)),((_tmp3F5[0]=((_tmp3F6.tag=
23,((_tmp3F6.f1=(void*)_tmp241,_tmp3F6)))),_tmp3F5))));struct _tuple12*_tmp3F9;
struct Cyc_List_List*_tmp3F8;_tmp254=((_tmp3F8=_region_malloc(_tmp253,sizeof(*
_tmp3F8)),((_tmp3F8->hd=((_tmp3F9=_region_malloc(_tmp253,sizeof(*_tmp3F9)),((
_tmp3F9->f1=eff1,((_tmp3F9->f2=_tmp242,_tmp3F9)))))),((_tmp3F8->tl=_tmp254,
_tmp3F8))))));}if(_tmp242 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp242 != (void*)&
Cyc_Absyn_UniqueRgn_val){struct Cyc_Absyn_AccessEff_struct _tmp3FC;struct Cyc_Absyn_AccessEff_struct*
_tmp3FB;void*eff2=(void*)((_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB[0]=((
_tmp3FC.tag=23,((_tmp3FC.f1=(void*)_tmp242,_tmp3FC)))),_tmp3FB))));struct
_tuple12*_tmp3FF;struct Cyc_List_List*_tmp3FE;_tmp254=((_tmp3FE=_region_malloc(
_tmp253,sizeof(*_tmp3FE)),((_tmp3FE->hd=((_tmp3FF=_region_malloc(_tmp253,sizeof(*
_tmp3FF)),((_tmp3FF->f1=eff2,((_tmp3FF->f2=_tmp241,_tmp3FF)))))),((_tmp3FE->tl=
_tmp254,_tmp3FE))))));}return Cyc_Tcenv_new_outlives_constraints((struct
_RegionHandle*)_tmp253,te,_tmp254,loc);};}void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
_tmp25D=te->le;if(_tmp25D == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_Tcenv_Fenv*
fe=(struct Cyc_Tcenv_Fenv*)_tmp25D;struct Cyc_Tcenv_Fenv _tmp25F;void*_tmp260;
struct Cyc_Tcenv_Fenv*_tmp25E=fe;_tmp25F=*_tmp25E;_tmp260=_tmp25F.curr_rgn;return
_tmp260;};}void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*rgn);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void*rgn){const char*_tmp400;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(
te,((_tmp400="check_rgn_accessible",_tag_dyneither(_tmp400,sizeof(char),21))));
struct Cyc_Tcenv_Fenv _tmp262;void*_tmp263;struct Cyc_RgnOrder_RgnPO*_tmp264;struct
Cyc_Tcenv_Fenv*_tmp261=fe;_tmp262=*_tmp261;_tmp263=_tmp262.capability;_tmp264=
_tmp262.region_order;if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp263) || Cyc_Tcutil_region_in_effect(
1,rgn,_tmp263))return;{struct Cyc_Absyn_AccessEff_struct _tmp403;struct Cyc_Absyn_AccessEff_struct*
_tmp402;if(Cyc_RgnOrder_eff_outlives_eff(_tmp264,(void*)((_tmp402=_cycalloc(
sizeof(*_tmp402)),((_tmp402[0]=((_tmp403.tag=23,((_tmp403.f1=(void*)rgn,_tmp403)))),
_tmp402)))),_tmp263))return;}{const char*_tmp407;void*_tmp406[1];struct Cyc_String_pa_struct
_tmp405;(_tmp405.tag=0,((_tmp405.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(rgn)),((_tmp406[0]=& _tmp405,Cyc_Tcutil_terr(loc,((_tmp407="Expression accesses unavailable region %s",
_tag_dyneither(_tmp407,sizeof(char),42))),_tag_dyneither(_tmp406,sizeof(void*),1)))))));};}
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void*rgn){Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv _tmp26D;struct Cyc_RgnOrder_RgnPO*_tmp26E;const char*_tmp408;
struct Cyc_Tcenv_Fenv*_tmp26C=Cyc_Tcenv_get_fenv(te,((_tmp408="check_rgn_resetable",
_tag_dyneither(_tmp408,sizeof(char),20))));_tmp26D=*_tmp26C;_tmp26E=_tmp26D.region_order;{
void*_tmp26F=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp271;_LL8D: {
struct Cyc_Absyn_VarType_struct*_tmp270=(struct Cyc_Absyn_VarType_struct*)_tmp26F;
if(_tmp270->tag != 2)goto _LL8F;else{_tmp271=_tmp270->f1;}}_LL8E: if(!Cyc_RgnOrder_is_region_resetable(
_tmp26E,_tmp271)){const char*_tmp40C;void*_tmp40B[1];struct Cyc_String_pa_struct
_tmp40A;(_tmp40A.tag=0,((_tmp40A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(rgn)),((_tmp40B[0]=& _tmp40A,Cyc_Tcutil_terr(loc,((_tmp40C="Region %s is not resetable",
_tag_dyneither(_tmp40C,sizeof(char),27))),_tag_dyneither(_tmp40B,sizeof(void*),1)))))));}
return;_LL8F:;_LL90: {const char*_tmp40F;void*_tmp40E;(_tmp40E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp40F="check_rgn_resetable",
_tag_dyneither(_tmp40F,sizeof(char),20))),_tag_dyneither(_tmp40E,sizeof(void*),0)));}
_LL8C:;};};}int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*
rt_b);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp277=te->le;rt_a=Cyc_Tcutil_compress(rt_a);rt_b=Cyc_Tcutil_compress(
rt_b);if(_tmp277 == 0)return rt_a == (void*)& Cyc_Absyn_UniqueRgn_val  || rt_a == (
void*)& Cyc_Absyn_HeapRgn_val  && rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;{struct
Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)_tmp277;struct Cyc_Tcenv_Fenv _tmp279;
struct Cyc_RgnOrder_RgnPO*_tmp27A;struct Cyc_Tcenv_Fenv*_tmp278=fe;_tmp279=*
_tmp278;_tmp27A=_tmp279.region_order;{struct Cyc_Absyn_AccessEff_struct _tmp412;
struct Cyc_Absyn_AccessEff_struct*_tmp411;int _tmp27B=Cyc_RgnOrder_effect_outlives(
_tmp27A,(void*)((_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411[0]=((_tmp412.tag=
23,((_tmp412.f1=(void*)rt_a,_tmp412)))),_tmp411)))),rt_b);return _tmp27B;};};}
struct _tuple13{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;struct Cyc_Position_Segment*
f4;};void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*eff);void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*eff){struct Cyc_Tcenv_Fenv _tmp280;void*
_tmp281;struct Cyc_RgnOrder_RgnPO*_tmp282;struct Cyc_Tcenv_SharedFenv*_tmp283;
const char*_tmp413;struct Cyc_Tcenv_Fenv*_tmp27F=Cyc_Tcenv_get_fenv(te,((_tmp413="check_effect_accessible",
_tag_dyneither(_tmp413,sizeof(char),24))));_tmp280=*_tmp27F;_tmp281=_tmp280.capability;
_tmp282=_tmp280.region_order;_tmp283=_tmp280.shared;if(Cyc_Tcutil_subset_effect(
0,eff,_tmp281))return;if(Cyc_RgnOrder_eff_outlives_eff(_tmp282,eff,_tmp281))
return;{struct _RegionHandle*frgn=_tmp283->frgn;struct _tuple13*_tmp416;struct Cyc_List_List*
_tmp415;_tmp283->delayed_effect_checks=((_tmp415=_region_malloc(frgn,sizeof(*
_tmp415)),((_tmp415->hd=((_tmp416=_region_malloc(frgn,sizeof(*_tmp416)),((
_tmp416->f1=_tmp281,((_tmp416->f2=eff,((_tmp416->f3=_tmp282,((_tmp416->f4=loc,
_tmp416)))))))))),((_tmp415->tl=_tmp283->delayed_effect_checks,_tmp415))))));};}
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp288;struct Cyc_Tcenv_SharedFenv*
_tmp289;const char*_tmp417;struct Cyc_Tcenv_Fenv*_tmp287=Cyc_Tcenv_get_fenv(te,((
_tmp417="check_delayed_effects",_tag_dyneither(_tmp417,sizeof(char),22))));
_tmp288=*_tmp287;_tmp289=_tmp288.shared;{struct Cyc_List_List*_tmp28A=_tmp289->delayed_effect_checks;
for(0;_tmp28A != 0;_tmp28A=_tmp28A->tl){struct _tuple13 _tmp28C;void*_tmp28D;void*
_tmp28E;struct Cyc_RgnOrder_RgnPO*_tmp28F;struct Cyc_Position_Segment*_tmp290;
struct _tuple13*_tmp28B=(struct _tuple13*)_tmp28A->hd;_tmp28C=*_tmp28B;_tmp28D=
_tmp28C.f1;_tmp28E=_tmp28C.f2;_tmp28F=_tmp28C.f3;_tmp290=_tmp28C.f4;if(Cyc_Tcutil_subset_effect(
1,_tmp28E,_tmp28D))continue;if(Cyc_RgnOrder_eff_outlives_eff(_tmp28F,_tmp28E,
_tmp28D))continue;{const char*_tmp41C;void*_tmp41B[2];struct Cyc_String_pa_struct
_tmp41A;struct Cyc_String_pa_struct _tmp419;(_tmp419.tag=0,((_tmp419.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp28E)),((_tmp41A.tag=
0,((_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp28D)),((_tmp41B[0]=& _tmp41A,((_tmp41B[1]=& _tmp419,Cyc_Tcutil_terr(_tmp290,((
_tmp41C="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(
_tmp41C,sizeof(char),51))),_tag_dyneither(_tmp41B,sizeof(void*),2)))))))))))));};}};}
struct _tuple14{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;struct Cyc_Position_Segment*
f3;};void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*po);void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po){struct Cyc_Tcenv_Fenv*
_tmp295=te->le;if(_tmp295 == 0){for(0;po != 0;po=po->tl){struct Cyc_Absyn_AccessEff_struct
_tmp41F;struct Cyc_Absyn_AccessEff_struct*_tmp41E;if(!Cyc_Tcutil_subset_effect(1,(*((
struct _tuple12*)po->hd)).f1,Cyc_Absyn_empty_effect) || !Cyc_Tcutil_subset_effect(
1,(void*)((_tmp41E=_cycalloc(sizeof(*_tmp41E)),((_tmp41E[0]=((_tmp41F.tag=23,((
_tmp41F.f1=(void*)(*((struct _tuple12*)po->hd)).f2,_tmp41F)))),_tmp41E)))),Cyc_Absyn_empty_effect)){
const char*_tmp422;void*_tmp421;(_tmp421=0,Cyc_Tcutil_terr(loc,((_tmp422="the required region ordering is not satisfied here",
_tag_dyneither(_tmp422,sizeof(char),51))),_tag_dyneither(_tmp421,sizeof(void*),0)));}}
return;}{struct Cyc_Tcenv_Fenv _tmp29B;struct Cyc_RgnOrder_RgnPO*_tmp29C;struct Cyc_Tcenv_SharedFenv*
_tmp29D;struct Cyc_Tcenv_Fenv*_tmp29A=(struct Cyc_Tcenv_Fenv*)_tmp295;_tmp29B=*
_tmp29A;_tmp29C=_tmp29B.region_order;_tmp29D=_tmp29B.shared;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp29C,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){struct _tuple14*_tmp425;struct Cyc_List_List*
_tmp424;_tmp29D->delayed_constraint_checks=((_tmp424=_region_malloc(_tmp29D->frgn,
sizeof(*_tmp424)),((_tmp424->hd=((_tmp425=_region_malloc(_tmp29D->frgn,sizeof(*
_tmp425)),((_tmp425->f1=_tmp29C,((_tmp425->f2=po,((_tmp425->f3=loc,_tmp425)))))))),((
_tmp424->tl=_tmp29D->delayed_constraint_checks,_tmp424))))));}};}void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv _tmp2A2;struct Cyc_Tcenv_SharedFenv*_tmp2A3;const char*
_tmp426;struct Cyc_Tcenv_Fenv*_tmp2A1=Cyc_Tcenv_get_fenv(te,((_tmp426="check_delayed_constraints",
_tag_dyneither(_tmp426,sizeof(char),26))));_tmp2A2=*_tmp2A1;_tmp2A3=_tmp2A2.shared;{
struct Cyc_List_List*_tmp2A4=_tmp2A3->delayed_constraint_checks;for(0;_tmp2A4 != 0;
_tmp2A4=_tmp2A4->tl){struct _tuple14 _tmp2A6;struct Cyc_RgnOrder_RgnPO*_tmp2A7;
struct Cyc_List_List*_tmp2A8;struct Cyc_Position_Segment*_tmp2A9;struct _tuple14*
_tmp2A5=(struct _tuple14*)_tmp2A4->hd;_tmp2A6=*_tmp2A5;_tmp2A7=_tmp2A6.f1;_tmp2A8=
_tmp2A6.f2;_tmp2A9=_tmp2A6.f3;if(!Cyc_RgnOrder_satisfies_constraints(_tmp2A7,
_tmp2A8,(void*)& Cyc_Absyn_HeapRgn_val,1)){const char*_tmp429;void*_tmp428;(
_tmp428=0,Cyc_Tcutil_terr(_tmp2A9,((_tmp429="the required region ordering is not satisfied here",
_tag_dyneither(_tmp429,sizeof(char),51))),_tag_dyneither(_tmp428,sizeof(void*),0)));}}};}
struct _tuple15{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct
Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Fndecl*fd);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*fd){struct
Cyc_Tcenv_Bindings*locals=0;struct _dyneither_ptr*_tmp436;const char*_tmp435;void*
_tmp434[1];struct Cyc_String_pa_struct _tmp433;struct Cyc_Absyn_Tvar*_tmp432;struct
Cyc_Absyn_Tvar*rgn0=(_tmp432=_cycalloc(sizeof(*_tmp432)),((_tmp432->name=((
_tmp436=_cycalloc(sizeof(*_tmp436)),((_tmp436[0]=(struct _dyneither_ptr)((_tmp433.tag=
0,((_tmp433.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((
_tmp434[0]=& _tmp433,Cyc_aprintf(((_tmp435="`%s",_tag_dyneither(_tmp435,sizeof(
char),4))),_tag_dyneither(_tmp434,sizeof(void*),1)))))))),_tmp436)))),((_tmp432->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp432->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp432)))))));
struct Cyc_List_List*_tmp437;struct Cyc_List_List*_tmp2AC=(_tmp437=_cycalloc(
sizeof(*_tmp437)),((_tmp437->hd=rgn0,((_tmp437->tl=fd->tvs,_tmp437)))));Cyc_Tcutil_check_unique_tvars(
loc,_tmp2AC);{struct Cyc_RgnOrder_RgnPO*_tmp2AD=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,
fd->rgn_po,(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,rgn0,loc);
struct Cyc_Absyn_VarType_struct _tmp43A;struct Cyc_Absyn_VarType_struct*_tmp439;
void*param_rgn=(void*)((_tmp439=_cycalloc(sizeof(*_tmp439)),((_tmp439[0]=((
_tmp43A.tag=2,((_tmp43A.f1=rgn0,_tmp43A)))),_tmp439))));struct Cyc_Core_Opt*
_tmp43B;struct Cyc_Core_Opt*param_rgn_opt=(_tmp43B=_cycalloc(sizeof(*_tmp43B)),((
_tmp43B->v=param_rgn,_tmp43B)));struct Cyc_List_List*_tmp2AE=0;{struct Cyc_List_List*
_tmp2AF=fd->args;for(0;_tmp2AF != 0;_tmp2AF=_tmp2AF->tl){struct Cyc_Absyn_Vardecl
_tmp441;struct _tuple0*_tmp440;struct Cyc_Absyn_Vardecl*_tmp43F;struct Cyc_Absyn_Vardecl*
_tmp2B0=(_tmp43F=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp43F[0]=((
_tmp441.sc=Cyc_Absyn_Public,((_tmp441.name=((_tmp440=_cycalloc(sizeof(*_tmp440)),((
_tmp440->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp440->f2=(*((struct
_tuple15*)_tmp2AF->hd)).f1,_tmp440)))))),((_tmp441.tq=(*((struct _tuple15*)
_tmp2AF->hd)).f2,((_tmp441.type=(*((struct _tuple15*)_tmp2AF->hd)).f3,((_tmp441.initializer=
0,((_tmp441.rgn=param_rgn_opt,((_tmp441.attributes=0,((_tmp441.escapes=0,_tmp441)))))))))))))))),
_tmp43F)));{struct Cyc_List_List _tmp444;struct Cyc_List_List*_tmp443;_tmp2AE=((
_tmp443=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp443[0]=((_tmp444.hd=
_tmp2B0,((_tmp444.tl=_tmp2AE,_tmp444)))),_tmp443))));}{struct Cyc_Absyn_Param_b_struct*
_tmp44A;struct Cyc_Absyn_Param_b_struct _tmp449;struct Cyc_Tcenv_Bindings*_tmp448;
locals=((_tmp448=_region_malloc(r,sizeof(*_tmp448)),((_tmp448->v=(*((struct
_tuple15*)_tmp2AF->hd)).f1,((_tmp448->b=(void*)((_tmp44A=_cycalloc(sizeof(*
_tmp44A)),((_tmp44A[0]=((_tmp449.tag=3,((_tmp449.f1=_tmp2B0,_tmp449)))),_tmp44A)))),((
_tmp448->tl=locals,_tmp448))))))));};}}if(fd->cyc_varargs != 0){struct Cyc_Core_Opt*
_tmp2BA;struct Cyc_Absyn_Tqual _tmp2BB;void*_tmp2BC;int _tmp2BD;struct Cyc_Absyn_VarargInfo
_tmp2B9=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));_tmp2BA=
_tmp2B9.name;_tmp2BB=_tmp2B9.tq;_tmp2BC=_tmp2B9.type;_tmp2BD=_tmp2B9.inject;if(
_tmp2BA != 0){void*_tmp2BE=Cyc_Absyn_dyneither_typ(_tmp2BC,param_rgn,_tmp2BB,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp450;struct _tuple0*_tmp44F;struct Cyc_Absyn_Vardecl*
_tmp44E;struct Cyc_Absyn_Vardecl*_tmp2BF=(_tmp44E=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1),((_tmp44E[0]=((_tmp450.sc=Cyc_Absyn_Public,((_tmp450.name=((_tmp44F=
_cycalloc(sizeof(*_tmp44F)),((_tmp44F->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((
_tmp44F->f2=(struct _dyneither_ptr*)_tmp2BA->v,_tmp44F)))))),((_tmp450.tq=Cyc_Absyn_empty_tqual(
0),((_tmp450.type=_tmp2BE,((_tmp450.initializer=0,((_tmp450.rgn=param_rgn_opt,((
_tmp450.attributes=0,((_tmp450.escapes=0,_tmp450)))))))))))))))),_tmp44E)));{
struct Cyc_List_List*_tmp451;_tmp2AE=((_tmp451=_cycalloc(sizeof(*_tmp451)),((
_tmp451->hd=_tmp2BF,((_tmp451->tl=_tmp2AE,_tmp451))))));}{struct Cyc_Absyn_Param_b_struct*
_tmp457;struct Cyc_Absyn_Param_b_struct _tmp456;struct Cyc_Tcenv_Bindings*_tmp455;
locals=((_tmp455=_region_malloc(r,sizeof(*_tmp455)),((_tmp455->v=(struct
_dyneither_ptr*)_tmp2BA->v,((_tmp455->b=(void*)((_tmp457=_cycalloc(sizeof(*
_tmp457)),((_tmp457[0]=((_tmp456.tag=3,((_tmp456.f1=_tmp2BF,_tmp456)))),_tmp457)))),((
_tmp455->tl=locals,_tmp455))))))));};}else{const char*_tmp45A;void*_tmp459;(
_tmp459=0,Cyc_Tcutil_terr(loc,((_tmp45A="missing name for varargs",
_tag_dyneither(_tmp45A,sizeof(char),25))),_tag_dyneither(_tmp459,sizeof(void*),0)));}}{
struct Cyc_Core_Opt _tmp45D;struct Cyc_Core_Opt*_tmp45C;fd->param_vardecls=((
_tmp45C=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp45C[0]=((_tmp45D.v=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2AE),
_tmp45D)),_tmp45C))));}{struct Cyc_Absyn_JoinEff_struct*_tmp47B;struct Cyc_List_List*
_tmp47A;struct Cyc_Absyn_AccessEff_struct _tmp479;struct Cyc_Absyn_AccessEff_struct*
_tmp478;struct Cyc_List_List*_tmp477;struct Cyc_Absyn_JoinEff_struct _tmp476;struct
Cyc_Tcenv_CtrlEnv*_tmp475;struct Cyc_Tcenv_SharedFenv*_tmp474;struct Cyc_Tcenv_Fenv*
_tmp473;return(_tmp473=_region_malloc(r,sizeof(*_tmp473)),((_tmp473->shared=(
struct Cyc_Tcenv_SharedFenv*)((_tmp474=_region_malloc(r,sizeof(*_tmp474)),((
_tmp474->frgn=r,((_tmp474->return_typ=fd->ret_type,((_tmp474->seen_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp474->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp474->delayed_effect_checks=
0,((_tmp474->delayed_constraint_checks=0,_tmp474)))))))))))))),((_tmp473->type_vars=(
struct Cyc_List_List*)_tmp2AC,((_tmp473->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp2AD,((_tmp473->locals=(struct Cyc_Tcenv_Bindings*)locals,((_tmp473->encloser=(
struct Cyc_Absyn_Stmt*)fd->body,((_tmp473->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
_tmp475=_region_malloc(r,sizeof(*_tmp475)),((_tmp475->ctrl_rgn=r,((_tmp475->continue_stmt=(
void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp475->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((
_tmp475->fallthru_clause=0,((_tmp475->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((
_tmp475->try_depth=0,_tmp475)))))))))))))),((_tmp473->capability=(void*)((void*)((
_tmp47B=_cycalloc(sizeof(*_tmp47B)),((_tmp47B[0]=((_tmp476.tag=24,((_tmp476.f1=((
_tmp47A=_cycalloc(sizeof(*_tmp47A)),((_tmp47A->hd=(void*)((_tmp478=_cycalloc(
sizeof(*_tmp478)),((_tmp478[0]=((_tmp479.tag=23,((_tmp479.f1=(void*)param_rgn,
_tmp479)))),_tmp478)))),((_tmp47A->tl=((_tmp477=_cycalloc(sizeof(*_tmp477)),((
_tmp477->hd=(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,((_tmp477->tl=
0,_tmp477)))))),_tmp47A)))))),_tmp476)))),_tmp47B))))),((_tmp473->curr_rgn=(void*)
param_rgn,((_tmp473->in_notreadctxt=(int)0,((_tmp473->fnrgn=(struct _RegionHandle*)
r,_tmp473)))))))))))))))))))));};};}struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*
fd);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Tcenv_Fenv
_tmp2DE;struct Cyc_Tcenv_Bindings*_tmp2DF;struct Cyc_RgnOrder_RgnPO*_tmp2E0;struct
Cyc_List_List*_tmp2E1;struct Cyc_Tcenv_SharedFenv*_tmp2E2;struct _RegionHandle*
_tmp2E3;struct Cyc_Tcenv_Fenv*_tmp2DD=old_fenv;_tmp2DE=*_tmp2DD;_tmp2DF=_tmp2DE.locals;
_tmp2E0=_tmp2DE.region_order;_tmp2E1=_tmp2DE.type_vars;_tmp2E2=_tmp2DE.shared;
_tmp2E3=_tmp2DE.fnrgn;{struct _RegionHandle*_tmp2E4=_tmp2E2->frgn;struct Cyc_Tcenv_Bindings*
_tmp2E5=(struct Cyc_Tcenv_Bindings*)_tmp2DF;struct _dyneither_ptr*_tmp488;const
char*_tmp487;void*_tmp486[1];struct Cyc_String_pa_struct _tmp485;struct Cyc_Absyn_Tvar*
_tmp484;struct Cyc_Absyn_Tvar*rgn0=(_tmp484=_cycalloc(sizeof(*_tmp484)),((_tmp484->name=((
_tmp488=_cycalloc(sizeof(*_tmp488)),((_tmp488[0]=(struct _dyneither_ptr)((_tmp485.tag=
0,((_tmp485.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((
_tmp486[0]=& _tmp485,Cyc_aprintf(((_tmp487="`%s",_tag_dyneither(_tmp487,sizeof(
char),4))),_tag_dyneither(_tmp486,sizeof(void*),1)))))))),_tmp488)))),((_tmp484->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp484->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp484)))))));{
struct Cyc_List_List*_tmp2E6=fd->tvs;for(0;_tmp2E6 != 0;_tmp2E6=_tmp2E6->tl){
struct Cyc_Absyn_Kind _tmp2E8;enum Cyc_Absyn_KindQual _tmp2E9;enum Cyc_Absyn_AliasQual
_tmp2EA;struct Cyc_Absyn_Kind*_tmp2E7=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp2E6->hd,& Cyc_Tcutil_bk);_tmp2E8=*_tmp2E7;_tmp2E9=_tmp2E8.kind;_tmp2EA=
_tmp2E8.aliasqual;if(_tmp2E9 == Cyc_Absyn_RgnKind){if(_tmp2EA == Cyc_Absyn_Aliasable)
_tmp2E0=Cyc_RgnOrder_add_youngest(_tmp2E4,_tmp2E0,(struct Cyc_Absyn_Tvar*)_tmp2E6->hd,
0,0);else{const char*_tmp48B;void*_tmp48A;(_tmp48A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp48B="non-intuitionistic tvar in nested_fenv",
_tag_dyneither(_tmp48B,sizeof(char),39))),_tag_dyneither(_tmp48A,sizeof(void*),0)));}}}}
_tmp2E0=Cyc_RgnOrder_add_youngest(_tmp2E4,_tmp2E0,rgn0,0,0);{struct Cyc_List_List*
_tmp48C;struct Cyc_List_List*_tmp2ED=(_tmp48C=_cycalloc(sizeof(*_tmp48C)),((
_tmp48C->hd=rgn0,((_tmp48C->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp2E1),_tmp48C)))));Cyc_Tcutil_check_unique_tvars(
loc,_tmp2ED);{struct Cyc_Absyn_VarType_struct _tmp48F;struct Cyc_Absyn_VarType_struct*
_tmp48E;void*param_rgn=(void*)((_tmp48E=_cycalloc(sizeof(*_tmp48E)),((_tmp48E[0]=((
_tmp48F.tag=2,((_tmp48F.f1=rgn0,_tmp48F)))),_tmp48E))));struct Cyc_Core_Opt*
_tmp490;struct Cyc_Core_Opt*param_rgn_opt=(_tmp490=_cycalloc(sizeof(*_tmp490)),((
_tmp490->v=param_rgn,_tmp490)));struct Cyc_List_List*_tmp2EE=0;{struct Cyc_List_List*
_tmp2EF=fd->args;for(0;_tmp2EF != 0;_tmp2EF=_tmp2EF->tl){struct Cyc_Absyn_Vardecl
_tmp496;struct _tuple0*_tmp495;struct Cyc_Absyn_Vardecl*_tmp494;struct Cyc_Absyn_Vardecl*
_tmp2F0=(_tmp494=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp494[0]=((
_tmp496.sc=Cyc_Absyn_Public,((_tmp496.name=((_tmp495=_cycalloc(sizeof(*_tmp495)),((
_tmp495->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp495->f2=(*((struct
_tuple15*)_tmp2EF->hd)).f1,_tmp495)))))),((_tmp496.tq=(*((struct _tuple15*)
_tmp2EF->hd)).f2,((_tmp496.type=(*((struct _tuple15*)_tmp2EF->hd)).f3,((_tmp496.initializer=
0,((_tmp496.rgn=param_rgn_opt,((_tmp496.attributes=0,((_tmp496.escapes=0,_tmp496)))))))))))))))),
_tmp494)));{struct Cyc_List_List _tmp499;struct Cyc_List_List*_tmp498;_tmp2EE=((
_tmp498=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp498[0]=((_tmp499.hd=
_tmp2F0,((_tmp499.tl=_tmp2EE,_tmp499)))),_tmp498))));}{struct Cyc_Absyn_Param_b_struct*
_tmp49F;struct Cyc_Absyn_Param_b_struct _tmp49E;struct Cyc_Tcenv_Bindings*_tmp49D;
_tmp2E5=(struct Cyc_Tcenv_Bindings*)((_tmp49D=_region_malloc(_tmp2E4,sizeof(*
_tmp49D)),((_tmp49D->v=(*((struct _tuple15*)_tmp2EF->hd)).f1,((_tmp49D->b=(void*)((
_tmp49F=_cycalloc(sizeof(*_tmp49F)),((_tmp49F[0]=((_tmp49E.tag=3,((_tmp49E.f1=
_tmp2F0,_tmp49E)))),_tmp49F)))),((_tmp49D->tl=_tmp2E5,_tmp49D))))))));};}}if(fd->cyc_varargs
!= 0){struct Cyc_Core_Opt*_tmp2FA;struct Cyc_Absyn_Tqual _tmp2FB;void*_tmp2FC;int
_tmp2FD;struct Cyc_Absyn_VarargInfo _tmp2F9=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp2FA=_tmp2F9.name;_tmp2FB=_tmp2F9.tq;_tmp2FC=
_tmp2F9.type;_tmp2FD=_tmp2F9.inject;if(_tmp2FA != 0){void*_tmp2FE=Cyc_Absyn_dyneither_typ(
_tmp2FC,param_rgn,_tmp2FB,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl _tmp4A5;
struct _tuple0*_tmp4A4;struct Cyc_Absyn_Vardecl*_tmp4A3;struct Cyc_Absyn_Vardecl*
_tmp2FF=(_tmp4A3=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4A3[0]=((
_tmp4A5.sc=Cyc_Absyn_Public,((_tmp4A5.name=((_tmp4A4=_cycalloc(sizeof(*_tmp4A4)),((
_tmp4A4->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp4A4->f2=(struct
_dyneither_ptr*)_tmp2FA->v,_tmp4A4)))))),((_tmp4A5.tq=Cyc_Absyn_empty_tqual(0),((
_tmp4A5.type=_tmp2FE,((_tmp4A5.initializer=0,((_tmp4A5.rgn=param_rgn_opt,((
_tmp4A5.attributes=0,((_tmp4A5.escapes=0,_tmp4A5)))))))))))))))),_tmp4A3)));{
struct Cyc_List_List*_tmp4A6;_tmp2EE=((_tmp4A6=_cycalloc(sizeof(*_tmp4A6)),((
_tmp4A6->hd=_tmp2FF,((_tmp4A6->tl=_tmp2EE,_tmp4A6))))));}{struct Cyc_Absyn_Param_b_struct*
_tmp4AC;struct Cyc_Absyn_Param_b_struct _tmp4AB;struct Cyc_Tcenv_Bindings*_tmp4AA;
_tmp2E5=(struct Cyc_Tcenv_Bindings*)((_tmp4AA=_region_malloc(_tmp2E4,sizeof(*
_tmp4AA)),((_tmp4AA->v=(struct _dyneither_ptr*)_tmp2FA->v,((_tmp4AA->b=(void*)((
_tmp4AC=_cycalloc(sizeof(*_tmp4AC)),((_tmp4AC[0]=((_tmp4AB.tag=3,((_tmp4AB.f1=
_tmp2FF,_tmp4AB)))),_tmp4AC)))),((_tmp4AA->tl=_tmp2E5,_tmp4AA))))))));};}else{
const char*_tmp4AF;void*_tmp4AE;(_tmp4AE=0,Cyc_Tcutil_terr(loc,((_tmp4AF="missing name for varargs",
_tag_dyneither(_tmp4AF,sizeof(char),25))),_tag_dyneither(_tmp4AE,sizeof(void*),0)));}}{
struct Cyc_Core_Opt _tmp4B2;struct Cyc_Core_Opt*_tmp4B1;fd->param_vardecls=((
_tmp4B1=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4B1[0]=((_tmp4B2.v=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2EE),
_tmp4B2)),_tmp4B1))));}{struct Cyc_Absyn_JoinEff_struct*_tmp4D0;struct Cyc_List_List*
_tmp4CF;struct Cyc_Absyn_AccessEff_struct _tmp4CE;struct Cyc_Absyn_AccessEff_struct*
_tmp4CD;struct Cyc_List_List*_tmp4CC;struct Cyc_Absyn_JoinEff_struct _tmp4CB;struct
Cyc_Tcenv_CtrlEnv*_tmp4CA;struct Cyc_Tcenv_SharedFenv*_tmp4C9;struct Cyc_Tcenv_Fenv*
_tmp4C8;return(struct Cyc_Tcenv_Fenv*)((_tmp4C8=_region_malloc(_tmp2E4,sizeof(*
_tmp4C8)),((_tmp4C8->shared=(struct Cyc_Tcenv_SharedFenv*)((_tmp4C9=
_region_malloc(_tmp2E4,sizeof(*_tmp4C9)),((_tmp4C9->frgn=_tmp2E4,((_tmp4C9->return_typ=
fd->ret_type,((_tmp4C9->seen_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp2E4,
Cyc_strptrcmp),((_tmp4C9->needed_labels=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
_tmp2E4,Cyc_strptrcmp),((_tmp4C9->delayed_effect_checks=0,((_tmp4C9->delayed_constraint_checks=
0,_tmp4C9)))))))))))))),((_tmp4C8->type_vars=(struct Cyc_List_List*)_tmp2ED,((
_tmp4C8->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2E0,((_tmp4C8->locals=(
struct Cyc_Tcenv_Bindings*)_tmp2E5,((_tmp4C8->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((
_tmp4C8->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)((_tmp4CA=
_region_malloc(_tmp2E4,sizeof(*_tmp4CA)),((_tmp4CA->ctrl_rgn=_tmp2E4,((_tmp4CA->continue_stmt=(
void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4CA->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((
_tmp4CA->fallthru_clause=0,((_tmp4CA->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((
_tmp4CA->try_depth=0,_tmp4CA))))))))))))))),((_tmp4C8->capability=(void*)((void*)((
_tmp4D0=_cycalloc(sizeof(*_tmp4D0)),((_tmp4D0[0]=((_tmp4CB.tag=24,((_tmp4CB.f1=((
_tmp4CF=_cycalloc(sizeof(*_tmp4CF)),((_tmp4CF->hd=(void*)((_tmp4CD=_cycalloc(
sizeof(*_tmp4CD)),((_tmp4CD[0]=((_tmp4CE.tag=23,((_tmp4CE.f1=(void*)param_rgn,
_tmp4CE)))),_tmp4CD)))),((_tmp4CF->tl=((_tmp4CC=_cycalloc(sizeof(*_tmp4CC)),((
_tmp4CC->hd=(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,((_tmp4CC->tl=
0,_tmp4CC)))))),_tmp4CF)))))),_tmp4CB)))),_tmp4D0))))),((_tmp4C8->curr_rgn=(void*)
param_rgn,((_tmp4C8->in_notreadctxt=(int)0,((_tmp4C8->fnrgn=(struct _RegionHandle*)
_tmp2E3,_tmp4C8))))))))))))))))))))));};};};};}
