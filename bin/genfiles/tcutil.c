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
tag;struct _dyneither_ptr f1;};int Cyc_printf(struct _dyneither_ptr,struct
_dyneither_ptr);struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct
_dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;
void*f2;};void*Cyc_Core_fst(struct _tuple0*);int Cyc_Core_intcmp(int,int);extern
char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
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
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple1 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct _dyneither_ptr Cyc_Position_string_of_segment(struct
Cyc_Position_Segment*);struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{
Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern
 = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz
 = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};enum Cyc_Absyn_AliasQual{
Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};enum Cyc_Absyn_KindQual{
Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind
 = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual
kind;enum Cyc_Absyn_AliasQual aliasqual;};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 
0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA
 = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};
struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};
struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{
struct _union_Constraint_Eq_constr Eq_constr;struct
_union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr
No_constr;};struct Cyc_Absyn_Eq_kb_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct
Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{
struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{
struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;
union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct
Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{int tag;
};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct
_tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;
struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);union Cyc_Absyn_AggrInfoU
Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union
Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
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
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple2*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;extern struct Cyc_Absyn_FloatType_struct
Cyc_Absyn_FloatType_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
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
_tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct
_union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;enum 
Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div
 = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt
 = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 
12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift
 = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec
 = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct
Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion
 = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_struct{int tag;union Cyc_Absyn_Cnst f1;};
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple2*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple9*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;
};struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;
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
int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple2*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(
void*y,union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple2*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*
es,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple11{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple11*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,
int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};struct
_tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(struct
Cyc_Absyn_Kind*);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,
struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn,struct Cyc_Position_Segment*loc);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);
struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;
struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct
Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct
_RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple2*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*
r1,void*r2);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct
_dyneither_ptr,struct _dyneither_ptr);struct _tuple13{unsigned int f1;int f2;};
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*
t);struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_kind_leq(
struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(
void*t);int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);
struct _tuple14{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple14
Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);extern int Cyc_Tcutil_warn_alias_coerce;
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_region(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k);int Cyc_Tcutil_unify_kindbound(void*,void*);struct
_tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple15 Cyc_Tcutil_swap_kind(
void*t,void*kb);int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct
Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple15*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple16{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct
_tuple16*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int
allow_evars,void*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,union Cyc_Absyn_Constraint*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,union Cyc_Absyn_Constraint*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,
int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple17{int f1;void*f2;
};struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_normalize_effect(void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Tcutil_bits_only(
void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcutil_snd_tqt(struct _tuple11*);int Cyc_Tcutil_supports_default(void*);
int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct
Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);void
Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;enum Cyc_Cyclone_C_Compilers{
Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";
struct Cyc_Tcutil_Unify_struct{char*tag;};struct Cyc_Tcutil_Unify_struct Cyc_Tcutil_Unify_val={
Cyc_Tcutil_Unify};void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(
void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(
void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq2_const=0;struct _dyneither_ptr Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure();
void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors >= Cyc_Position_max_errors)
return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{const char*_tmpFD9;void*
_tmpFD8[2];const char*_tmpFD7;const char*_tmpFD6;struct Cyc_String_pa_struct _tmpFD5;
struct Cyc_String_pa_struct _tmpFD4;struct _dyneither_ptr s1=(struct _dyneither_ptr)((
_tmpFD4.tag=0,((_tmpFD4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmpFD5.tag=0,((_tmpFD5.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(
_tmpFD6="const ",_tag_dyneither(_tmpFD6,sizeof(char),7)):((_tmpFD7="",
_tag_dyneither(_tmpFD7,sizeof(char),1)))),((_tmpFD8[0]=& _tmpFD5,((_tmpFD8[1]=&
_tmpFD4,Cyc_aprintf(((_tmpFD9="%s%s",_tag_dyneither(_tmpFD9,sizeof(char),5))),
_tag_dyneither(_tmpFD8,sizeof(void*),2))))))))))))));const char*_tmpFE2;void*
_tmpFE1[2];const char*_tmpFE0;const char*_tmpFDF;struct Cyc_String_pa_struct _tmpFDE;
struct Cyc_String_pa_struct _tmpFDD;struct _dyneither_ptr s2=(struct _dyneither_ptr)((
_tmpFDD.tag=0,((_tmpFDD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t2_failure)),((_tmpFDE.tag=0,((_tmpFDE.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(
_tmpFDF="const ",_tag_dyneither(_tmpFDF,sizeof(char),7)):((_tmpFE0="",
_tag_dyneither(_tmpFE0,sizeof(char),1)))),((_tmpFE1[0]=& _tmpFDE,((_tmpFE1[1]=&
_tmpFDD,Cyc_aprintf(((_tmpFE2="%s%s",_tag_dyneither(_tmpFE2,sizeof(char),5))),
_tag_dyneither(_tmpFE1,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmpFE6;
void*_tmpFE5[1];struct Cyc_String_pa_struct _tmpFE4;(_tmpFE4.tag=0,((_tmpFE4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpFE5[0]=& _tmpFE4,Cyc_fprintf(
Cyc_stderr,((_tmpFE6="  %s",_tag_dyneither(_tmpFE6,sizeof(char),5))),
_tag_dyneither(_tmpFE5,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmpFE9;void*_tmpFE8;(_tmpFE8=0,Cyc_fprintf(
Cyc_stderr,((_tmpFE9="\n\t",_tag_dyneither(_tmpFE9,sizeof(char),3))),
_tag_dyneither(_tmpFE8,sizeof(void*),0)));}pos=8;}else{{const char*_tmpFEC;void*
_tmpFEB;(_tmpFEB=0,Cyc_fprintf(Cyc_stderr,((_tmpFEC=" ",_tag_dyneither(_tmpFEC,
sizeof(char),2))),_tag_dyneither(_tmpFEB,sizeof(void*),0)));}++ pos;}{const char*
_tmpFEF;void*_tmpFEE;(_tmpFEE=0,Cyc_fprintf(Cyc_stderr,((_tmpFEF="and ",
_tag_dyneither(_tmpFEF,sizeof(char),5))),_tag_dyneither(_tmpFEE,sizeof(void*),0)));}
pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*_tmpFF2;
void*_tmpFF1;(_tmpFF1=0,Cyc_fprintf(Cyc_stderr,((_tmpFF2="\n\t",_tag_dyneither(
_tmpFF2,sizeof(char),3))),_tag_dyneither(_tmpFF1,sizeof(void*),0)));}pos=8;}{
const char*_tmpFF6;void*_tmpFF5[1];struct Cyc_String_pa_struct _tmpFF4;(_tmpFF4.tag=
0,((_tmpFF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpFF5[0]=&
_tmpFF4,Cyc_fprintf(Cyc_stderr,((_tmpFF6="%s ",_tag_dyneither(_tmpFF6,sizeof(
char),4))),_tag_dyneither(_tmpFF5,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmpFF9;void*
_tmpFF8;(_tmpFF8=0,Cyc_fprintf(Cyc_stderr,((_tmpFF9="\n\t",_tag_dyneither(
_tmpFF9,sizeof(char),3))),_tag_dyneither(_tmpFF8,sizeof(void*),0)));}pos=8;}{
const char*_tmpFFC;void*_tmpFFB;(_tmpFFB=0,Cyc_fprintf(Cyc_stderr,((_tmpFFC="are not compatible. ",
_tag_dyneither(_tmpFFC,sizeof(char),21))),_tag_dyneither(_tmpFFB,sizeof(void*),0)));}
pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){const
char*_tmpFFF;void*_tmpFFE;(_tmpFFE=0,Cyc_fprintf(Cyc_stderr,((_tmpFFF="\n\t",
_tag_dyneither(_tmpFFF,sizeof(char),3))),_tag_dyneither(_tmpFFE,sizeof(void*),0)));}{
const char*_tmp1003;void*_tmp1002[1];struct Cyc_String_pa_struct _tmp1001;(_tmp1001.tag=
0,((_tmp1001.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((
_tmp1002[0]=& _tmp1001,Cyc_fprintf(Cyc_stderr,((_tmp1003="%s",_tag_dyneither(
_tmp1003,sizeof(char),3))),_tag_dyneither(_tmp1002,sizeof(void*),1)))))));};}{
const char*_tmp1006;void*_tmp1005;(_tmp1005=0,Cyc_fprintf(Cyc_stderr,((_tmp1006="\n",
_tag_dyneither(_tmp1006,sizeof(char),2))),_tag_dyneither(_tmp1005,sizeof(void*),
0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(
Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap){struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{const char*_tmp100A;void*_tmp1009[1];struct Cyc_String_pa_struct _tmp1008;(
_tmp1008.tag=0,((_tmp1008.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((
_tmp1009[0]=& _tmp1008,Cyc_fprintf(Cyc_stderr,((_tmp100A="Compiler Error (Tcutil::impos): %s\n",
_tag_dyneither(_tmp100A,sizeof(char),36))),_tag_dyneither(_tmp1009,sizeof(void*),
1)))))));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct
_tmp100D;struct Cyc_Core_Impossible_struct*_tmp100C;(int)_throw((void*)((_tmp100C=
_cycalloc(sizeof(*_tmp100C)),((_tmp100C[0]=((_tmp100D.tag=Cyc_Core_Impossible,((
_tmp100D.f1=msg,_tmp100D)))),_tmp100C)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct
Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs);void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){
const char*_tmp1012;void*_tmp1011[2];struct Cyc_String_pa_struct _tmp1010;struct Cyc_String_pa_struct
_tmp100F;(_tmp100F.tag=0,((_tmp100F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((
_tmp1010.tag=0,((_tmp1010.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmp1011[0]=& _tmp1010,((_tmp1011[1]=& _tmp100F,
Cyc_fprintf(Cyc_stderr,((_tmp1012="%s::%s ",_tag_dyneither(_tmp1012,sizeof(char),
8))),_tag_dyneither(_tmp1011,sizeof(void*),2)))))))))))));}{const char*_tmp1015;
void*_tmp1014;(_tmp1014=0,Cyc_fprintf(Cyc_stderr,((_tmp1015="\n",_tag_dyneither(
_tmp1015,sizeof(char),2))),_tag_dyneither(_tmp1014,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=
0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{struct Cyc_List_List*_tmp1016;Cyc_Tcutil_warning_segs=((_tmp1016=
_cycalloc(sizeof(*_tmp1016)),((_tmp1016->hd=sg,((_tmp1016->tl=Cyc_Tcutil_warning_segs,
_tmp1016))))));}{struct _dyneither_ptr*_tmp1019;struct Cyc_List_List*_tmp1018;Cyc_Tcutil_warning_msgs=((
_tmp1018=_cycalloc(sizeof(*_tmp1018)),((_tmp1018->hd=((_tmp1019=_cycalloc(
sizeof(*_tmp1019)),((_tmp1019[0]=msg,_tmp1019)))),((_tmp1018->tl=Cyc_Tcutil_warning_msgs,
_tmp1018))))));};}void Cyc_Tcutil_flush_warnings();void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)return;{const char*_tmp101C;void*_tmp101B;(
_tmp101B=0,Cyc_fprintf(Cyc_stderr,((_tmp101C="***Warnings***\n",_tag_dyneither(
_tmp101C,sizeof(char),16))),_tag_dyneither(_tmp101B,sizeof(void*),0)));}{struct
Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs
!= 0){{const char*_tmp1021;void*_tmp1020[2];struct Cyc_String_pa_struct _tmp101F;
struct Cyc_String_pa_struct _tmp101E;(_tmp101E.tag=0,((_tmp101E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp101F.tag=0,((_tmp101F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)
_check_null(_tmp36))->hd)),((_tmp1020[0]=& _tmp101F,((_tmp1020[1]=& _tmp101E,Cyc_fprintf(
Cyc_stderr,((_tmp1021="%s: %s\n",_tag_dyneither(_tmp1021,sizeof(char),8))),
_tag_dyneither(_tmp1020,sizeof(void*),2)))))))))))));}_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{const char*
_tmp1024;void*_tmp1023;(_tmp1023=0,Cyc_fprintf(Cyc_stderr,((_tmp1024="**************\n",
_tag_dyneither(_tmp1024,sizeof(char),16))),_tag_dyneither(_tmp1023,sizeof(void*),
0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=
0;static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2);static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return tv1->identity - tv2->identity;}void*Cyc_Tcutil_compress(void*t);void*
Cyc_Tcutil_compress(void*t){void*_tmp3D=t;struct Cyc_Core_Opt*_tmp3F;void**_tmp41;
void**_tmp43;void***_tmp44;struct Cyc_Core_Opt*_tmp46;struct Cyc_Core_Opt**_tmp47;
struct Cyc_Absyn_Exp*_tmp49;_LL1: {struct Cyc_Absyn_Evar_struct*_tmp3E=(struct Cyc_Absyn_Evar_struct*)
_tmp3D;if(_tmp3E->tag != 1)goto _LL3;else{_tmp3F=_tmp3E->f2;if(_tmp3F != 0)goto _LL3;}}
_LL2: goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_struct*_tmp40=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp40->tag != 18)goto _LL5;else{_tmp41=_tmp40->f4;if(_tmp41 != 0)goto
_LL5;}}_LL4: return t;_LL5: {struct Cyc_Absyn_TypedefType_struct*_tmp42=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp42->tag != 18)goto _LL7;else{_tmp43=_tmp42->f4;_tmp44=(void***)&
_tmp42->f4;}}_LL6: {void*t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp44)));
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmp1025;*_tmp44=((_tmp1025=
_cycalloc(sizeof(*_tmp1025)),((_tmp1025[0]=t2,_tmp1025))));}return t2;}_LL7: {
struct Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(
_tmp45->tag != 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)&
_tmp45->f2;}}_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v){struct Cyc_Core_Opt*_tmp1026;*_tmp47=((_tmp1026=_cycalloc(sizeof(*
_tmp1026)),((_tmp1026->v=t2,_tmp1026))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
_tmp48=(struct Cyc_Absyn_ValueofType_struct*)_tmp3D;if(_tmp48->tag != 19)goto _LLB;
else{_tmp49=_tmp48->f1;}}_LLA: Cyc_Evexp_eval_const_uint_exp(_tmp49);{void*_tmp4C=
_tmp49->r;void*_tmp4E;_LLE: {struct Cyc_Absyn_Valueof_e_struct*_tmp4D=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp4C;if(_tmp4D->tag != 39)goto _LL10;else{_tmp4E=(void*)_tmp4D->f1;}}_LLF: return
_tmp4E;_LL10:;_LL11: return t;_LLD:;};_LLB:;_LLC: return t;_LL0:;}void*Cyc_Tcutil_copy_type(
void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){return((
struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_type,ts);}static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(
union Cyc_Absyn_Constraint*cptr);static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(
union Cyc_Absyn_Constraint*cptr){union Cyc_Absyn_Constraint*_tmp4F=cptr;union Cyc_Absyn_Constraint
_tmp50;int _tmp51;union Cyc_Absyn_Constraint _tmp52;void*_tmp53;union Cyc_Absyn_Constraint
_tmp54;union Cyc_Absyn_Constraint*_tmp55;_LL13: _tmp50=*_tmp4F;if((_tmp50.No_constr).tag
!= 3)goto _LL15;_tmp51=(int)(_tmp50.No_constr).val;_LL14: return Cyc_Absyn_empty_conref();
_LL15: _tmp52=*_tmp4F;if((_tmp52.Eq_constr).tag != 1)goto _LL17;_tmp53=(void*)(
_tmp52.Eq_constr).val;_LL16: return Cyc_Absyn_new_conref(_tmp53);_LL17: _tmp54=*
_tmp4F;if((_tmp54.Forward_constr).tag != 2)goto _LL12;_tmp55=(union Cyc_Absyn_Constraint*)(
_tmp54.Forward_constr).val;_LL18: return Cyc_Tcutil_copy_conref(_tmp55);_LL12:;}
static void*Cyc_Tcutil_copy_kindbound(void*kb);static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp56=Cyc_Absyn_compress_kb(kb);struct Cyc_Absyn_Kind*_tmp59;_LL1A: {
struct Cyc_Absyn_Unknown_kb_struct*_tmp57=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp56;if(_tmp57->tag != 1)goto _LL1C;}_LL1B: {struct Cyc_Absyn_Unknown_kb_struct
_tmp1029;struct Cyc_Absyn_Unknown_kb_struct*_tmp1028;return(void*)((_tmp1028=
_cycalloc(sizeof(*_tmp1028)),((_tmp1028[0]=((_tmp1029.tag=1,((_tmp1029.f1=0,
_tmp1029)))),_tmp1028))));}_LL1C: {struct Cyc_Absyn_Less_kb_struct*_tmp58=(struct
Cyc_Absyn_Less_kb_struct*)_tmp56;if(_tmp58->tag != 2)goto _LL1E;else{_tmp59=_tmp58->f2;}}
_LL1D: {struct Cyc_Absyn_Less_kb_struct _tmp102C;struct Cyc_Absyn_Less_kb_struct*
_tmp102B;return(void*)((_tmp102B=_cycalloc(sizeof(*_tmp102B)),((_tmp102B[0]=((
_tmp102C.tag=2,((_tmp102C.f1=0,((_tmp102C.f2=_tmp59,_tmp102C)))))),_tmp102B))));}
_LL1E:;_LL1F: return kb;_LL19:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(
struct Cyc_Absyn_Tvar*tv);static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct
Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmp102D;return(_tmp102D=_cycalloc(
sizeof(*_tmp102D)),((_tmp102D->name=tv->name,((_tmp102D->identity=- 1,((_tmp102D->kind=
Cyc_Tcutil_copy_kindbound(tv->kind),_tmp102D)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(
struct _tuple9*arg);static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp60;struct Cyc_Core_Opt*_tmp61;struct Cyc_Absyn_Tqual _tmp62;void*
_tmp63;struct _tuple9*_tmp5F=arg;_tmp60=*_tmp5F;_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;
_tmp63=_tmp60.f3;{struct _tuple9*_tmp102E;return(_tmp102E=_cycalloc(sizeof(*
_tmp102E)),((_tmp102E->f1=_tmp61,((_tmp102E->f2=_tmp62,((_tmp102E->f3=Cyc_Tcutil_copy_type(
_tmp63),_tmp102E)))))));};}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct
_tuple11*arg);static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*arg){
struct _tuple11 _tmp66;struct Cyc_Absyn_Tqual _tmp67;void*_tmp68;struct _tuple11*
_tmp65=arg;_tmp66=*_tmp65;_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{struct _tuple11*
_tmp102F;return(_tmp102F=_cycalloc(sizeof(*_tmp102F)),((_tmp102F->f1=_tmp67,((
_tmp102F->f2=Cyc_Tcutil_copy_type(_tmp68),_tmp102F)))));};}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp1030;return(_tmp1030=_cycalloc(sizeof(*_tmp1030)),((_tmp1030->name=f->name,((
_tmp1030->tq=f->tq,((_tmp1030->type=Cyc_Tcutil_copy_type(f->type),((_tmp1030->width=
f->width,((_tmp1030->attributes=f->attributes,_tmp1030)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x);static struct _tuple0*Cyc_Tcutil_copy_rgncmp(
struct _tuple0*x){struct _tuple0 _tmp6C;void*_tmp6D;void*_tmp6E;struct _tuple0*
_tmp6B=x;_tmp6C=*_tmp6B;_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;{struct _tuple0*
_tmp1031;return(_tmp1031=_cycalloc(sizeof(*_tmp1031)),((_tmp1031->f1=Cyc_Tcutil_copy_type(
_tmp6D),((_tmp1031->f2=Cyc_Tcutil_copy_type(_tmp6E),_tmp1031)))));};}static
struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f);
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*
f){struct Cyc_Absyn_Enumfield*_tmp1032;return(_tmp1032=_cycalloc(sizeof(*_tmp1032)),((
_tmp1032->name=f->name,((_tmp1032->tag=f->tag,((_tmp1032->loc=f->loc,_tmp1032)))))));}
void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_copy_type(void*t){void*_tmp71=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp75;struct Cyc_Absyn_DatatypeInfo
_tmp77;union Cyc_Absyn_DatatypeInfoU _tmp78;struct Cyc_List_List*_tmp79;struct Cyc_Absyn_DatatypeFieldInfo
_tmp7B;union Cyc_Absyn_DatatypeFieldInfoU _tmp7C;struct Cyc_List_List*_tmp7D;struct
Cyc_Absyn_PtrInfo _tmp7F;void*_tmp80;struct Cyc_Absyn_Tqual _tmp81;struct Cyc_Absyn_PtrAtts
_tmp82;void*_tmp83;union Cyc_Absyn_Constraint*_tmp84;union Cyc_Absyn_Constraint*
_tmp85;union Cyc_Absyn_Constraint*_tmp86;struct Cyc_Absyn_PtrLoc*_tmp87;struct Cyc_Absyn_ArrayInfo
_tmp8C;void*_tmp8D;struct Cyc_Absyn_Tqual _tmp8E;struct Cyc_Absyn_Exp*_tmp8F;union
Cyc_Absyn_Constraint*_tmp90;struct Cyc_Position_Segment*_tmp91;struct Cyc_Absyn_FnInfo
_tmp93;struct Cyc_List_List*_tmp94;struct Cyc_Core_Opt*_tmp95;void*_tmp96;struct
Cyc_List_List*_tmp97;int _tmp98;struct Cyc_Absyn_VarargInfo*_tmp99;struct Cyc_List_List*
_tmp9A;struct Cyc_List_List*_tmp9B;struct Cyc_List_List*_tmp9D;struct Cyc_Absyn_AggrInfo
_tmp9F;union Cyc_Absyn_AggrInfoU _tmpA0;struct _tuple4 _tmpA1;enum Cyc_Absyn_AggrKind
_tmpA2;struct _tuple2*_tmpA3;struct Cyc_Core_Opt*_tmpA4;struct Cyc_List_List*_tmpA5;
struct Cyc_Absyn_AggrInfo _tmpA7;union Cyc_Absyn_AggrInfoU _tmpA8;struct Cyc_Absyn_Aggrdecl**
_tmpA9;struct Cyc_List_List*_tmpAA;enum Cyc_Absyn_AggrKind _tmpAC;struct Cyc_List_List*
_tmpAD;struct _tuple2*_tmpAF;struct Cyc_Absyn_Enumdecl*_tmpB0;struct Cyc_List_List*
_tmpB2;void*_tmpB4;struct Cyc_Absyn_Exp*_tmpB6;void*_tmpB8;void*_tmpBA;void*
_tmpBB;struct _tuple2*_tmpBD;struct Cyc_List_List*_tmpBE;struct Cyc_Absyn_Typedefdecl*
_tmpBF;void*_tmpC3;struct Cyc_List_List*_tmpC5;void*_tmpC7;_LL21: {struct Cyc_Absyn_VoidType_struct*
_tmp72=(struct Cyc_Absyn_VoidType_struct*)_tmp71;if(_tmp72->tag != 0)goto _LL23;}
_LL22: goto _LL24;_LL23: {struct Cyc_Absyn_Evar_struct*_tmp73=(struct Cyc_Absyn_Evar_struct*)
_tmp71;if(_tmp73->tag != 1)goto _LL25;}_LL24: return t;_LL25: {struct Cyc_Absyn_VarType_struct*
_tmp74=(struct Cyc_Absyn_VarType_struct*)_tmp71;if(_tmp74->tag != 2)goto _LL27;
else{_tmp75=_tmp74->f1;}}_LL26: {struct Cyc_Absyn_VarType_struct _tmp1035;struct
Cyc_Absyn_VarType_struct*_tmp1034;return(void*)((_tmp1034=_cycalloc(sizeof(*
_tmp1034)),((_tmp1034[0]=((_tmp1035.tag=2,((_tmp1035.f1=Cyc_Tcutil_copy_tvar(
_tmp75),_tmp1035)))),_tmp1034))));}_LL27: {struct Cyc_Absyn_DatatypeType_struct*
_tmp76=(struct Cyc_Absyn_DatatypeType_struct*)_tmp71;if(_tmp76->tag != 3)goto _LL29;
else{_tmp77=_tmp76->f1;_tmp78=_tmp77.datatype_info;_tmp79=_tmp77.targs;}}_LL28: {
struct Cyc_Absyn_DatatypeType_struct _tmp103B;struct Cyc_Absyn_DatatypeInfo _tmp103A;
struct Cyc_Absyn_DatatypeType_struct*_tmp1039;return(void*)((_tmp1039=_cycalloc(
sizeof(*_tmp1039)),((_tmp1039[0]=((_tmp103B.tag=3,((_tmp103B.f1=((_tmp103A.datatype_info=
_tmp78,((_tmp103A.targs=Cyc_Tcutil_copy_types(_tmp79),_tmp103A)))),_tmp103B)))),
_tmp1039))));}_LL29: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp7A=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp71;if(_tmp7A->tag != 4)goto _LL2B;else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.field_info;
_tmp7D=_tmp7B.targs;}}_LL2A: {struct Cyc_Absyn_DatatypeFieldType_struct _tmp1041;
struct Cyc_Absyn_DatatypeFieldInfo _tmp1040;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp103F;return(void*)((_tmp103F=_cycalloc(sizeof(*_tmp103F)),((_tmp103F[0]=((
_tmp1041.tag=4,((_tmp1041.f1=((_tmp1040.field_info=_tmp7C,((_tmp1040.targs=Cyc_Tcutil_copy_types(
_tmp7D),_tmp1040)))),_tmp1041)))),_tmp103F))));}_LL2B: {struct Cyc_Absyn_PointerType_struct*
_tmp7E=(struct Cyc_Absyn_PointerType_struct*)_tmp71;if(_tmp7E->tag != 5)goto _LL2D;
else{_tmp7F=_tmp7E->f1;_tmp80=_tmp7F.elt_typ;_tmp81=_tmp7F.elt_tq;_tmp82=_tmp7F.ptr_atts;
_tmp83=_tmp82.rgn;_tmp84=_tmp82.nullable;_tmp85=_tmp82.bounds;_tmp86=_tmp82.zero_term;
_tmp87=_tmp82.ptrloc;}}_LL2C: {void*_tmpD0=Cyc_Tcutil_copy_type(_tmp80);void*
_tmpD1=Cyc_Tcutil_copy_type(_tmp83);union Cyc_Absyn_Constraint*_tmpD2=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp84);struct Cyc_Absyn_Tqual
_tmpD3=_tmp81;union Cyc_Absyn_Constraint*_tmpD4=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp85);union Cyc_Absyn_Constraint*
_tmpD5=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp86);struct Cyc_Absyn_PointerType_struct _tmp104B;struct Cyc_Absyn_PtrAtts
_tmp104A;struct Cyc_Absyn_PtrInfo _tmp1049;struct Cyc_Absyn_PointerType_struct*
_tmp1048;return(void*)((_tmp1048=_cycalloc(sizeof(*_tmp1048)),((_tmp1048[0]=((
_tmp104B.tag=5,((_tmp104B.f1=((_tmp1049.elt_typ=_tmpD0,((_tmp1049.elt_tq=_tmpD3,((
_tmp1049.ptr_atts=((_tmp104A.rgn=_tmpD1,((_tmp104A.nullable=_tmpD2,((_tmp104A.bounds=
_tmpD4,((_tmp104A.zero_term=_tmpD5,((_tmp104A.ptrloc=_tmp87,_tmp104A)))))))))),
_tmp1049)))))),_tmp104B)))),_tmp1048))));}_LL2D: {struct Cyc_Absyn_IntType_struct*
_tmp88=(struct Cyc_Absyn_IntType_struct*)_tmp71;if(_tmp88->tag != 6)goto _LL2F;}
_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_FloatType_struct*_tmp89=(struct Cyc_Absyn_FloatType_struct*)
_tmp71;if(_tmp89->tag != 7)goto _LL31;}_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_DoubleType_struct*
_tmp8A=(struct Cyc_Absyn_DoubleType_struct*)_tmp71;if(_tmp8A->tag != 8)goto _LL33;}
_LL32: return t;_LL33: {struct Cyc_Absyn_ArrayType_struct*_tmp8B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp71;if(_tmp8B->tag != 9)goto _LL35;else{_tmp8C=_tmp8B->f1;_tmp8D=_tmp8C.elt_type;
_tmp8E=_tmp8C.tq;_tmp8F=_tmp8C.num_elts;_tmp90=_tmp8C.zero_term;_tmp91=_tmp8C.zt_loc;}}
_LL34: {struct Cyc_Absyn_ArrayType_struct _tmp1051;struct Cyc_Absyn_ArrayInfo
_tmp1050;struct Cyc_Absyn_ArrayType_struct*_tmp104F;return(void*)((_tmp104F=
_cycalloc(sizeof(*_tmp104F)),((_tmp104F[0]=((_tmp1051.tag=9,((_tmp1051.f1=((
_tmp1050.elt_type=Cyc_Tcutil_copy_type(_tmp8D),((_tmp1050.tq=_tmp8E,((_tmp1050.num_elts=
_tmp8F,((_tmp1050.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp90),((_tmp1050.zt_loc=_tmp91,_tmp1050)))))))))),
_tmp1051)))),_tmp104F))));}_LL35: {struct Cyc_Absyn_FnType_struct*_tmp92=(struct
Cyc_Absyn_FnType_struct*)_tmp71;if(_tmp92->tag != 10)goto _LL37;else{_tmp93=_tmp92->f1;
_tmp94=_tmp93.tvars;_tmp95=_tmp93.effect;_tmp96=_tmp93.ret_typ;_tmp97=_tmp93.args;
_tmp98=_tmp93.c_varargs;_tmp99=_tmp93.cyc_varargs;_tmp9A=_tmp93.rgn_po;_tmp9B=
_tmp93.attributes;}}_LL36: {struct Cyc_List_List*_tmpDD=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmp94);struct Cyc_Core_Opt*_tmp1052;struct Cyc_Core_Opt*
_tmpDE=_tmp95 == 0?0:((_tmp1052=_cycalloc(sizeof(*_tmp1052)),((_tmp1052->v=Cyc_Tcutil_copy_type((
void*)_tmp95->v),_tmp1052))));void*_tmpDF=Cyc_Tcutil_copy_type(_tmp96);struct Cyc_List_List*
_tmpE0=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp97);int _tmpE1=_tmp98;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp99 != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp99;struct Cyc_Absyn_VarargInfo*_tmp1053;cyc_varargs2=((_tmp1053=_cycalloc(
sizeof(*_tmp1053)),((_tmp1053->name=cv->name,((_tmp1053->tq=cv->tq,((_tmp1053->type=
Cyc_Tcutil_copy_type(cv->type),((_tmp1053->inject=cv->inject,_tmp1053))))))))));}{
struct Cyc_List_List*_tmpE3=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct
_tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9A);
struct Cyc_List_List*_tmpE4=_tmp9B;struct Cyc_Absyn_FnType_struct _tmp1059;struct
Cyc_Absyn_FnInfo _tmp1058;struct Cyc_Absyn_FnType_struct*_tmp1057;return(void*)((
_tmp1057=_cycalloc(sizeof(*_tmp1057)),((_tmp1057[0]=((_tmp1059.tag=10,((_tmp1059.f1=((
_tmp1058.tvars=_tmpDD,((_tmp1058.effect=_tmpDE,((_tmp1058.ret_typ=_tmpDF,((
_tmp1058.args=_tmpE0,((_tmp1058.c_varargs=_tmpE1,((_tmp1058.cyc_varargs=
cyc_varargs2,((_tmp1058.rgn_po=_tmpE3,((_tmp1058.attributes=_tmpE4,_tmp1058)))))))))))))))),
_tmp1059)))),_tmp1057))));};}_LL37: {struct Cyc_Absyn_TupleType_struct*_tmp9C=(
struct Cyc_Absyn_TupleType_struct*)_tmp71;if(_tmp9C->tag != 11)goto _LL39;else{
_tmp9D=_tmp9C->f1;}}_LL38: {struct Cyc_Absyn_TupleType_struct _tmp105C;struct Cyc_Absyn_TupleType_struct*
_tmp105B;return(void*)((_tmp105B=_cycalloc(sizeof(*_tmp105B)),((_tmp105B[0]=((
_tmp105C.tag=11,((_tmp105C.f1=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp9D),
_tmp105C)))),_tmp105B))));}_LL39: {struct Cyc_Absyn_AggrType_struct*_tmp9E=(
struct Cyc_Absyn_AggrType_struct*)_tmp71;if(_tmp9E->tag != 12)goto _LL3B;else{
_tmp9F=_tmp9E->f1;_tmpA0=_tmp9F.aggr_info;if((_tmpA0.UnknownAggr).tag != 1)goto
_LL3B;_tmpA1=(struct _tuple4)(_tmpA0.UnknownAggr).val;_tmpA2=_tmpA1.f1;_tmpA3=
_tmpA1.f2;_tmpA4=_tmpA1.f3;_tmpA5=_tmp9F.targs;}}_LL3A: {struct Cyc_Absyn_AggrType_struct
_tmp1062;struct Cyc_Absyn_AggrInfo _tmp1061;struct Cyc_Absyn_AggrType_struct*
_tmp1060;return(void*)((_tmp1060=_cycalloc(sizeof(*_tmp1060)),((_tmp1060[0]=((
_tmp1062.tag=12,((_tmp1062.f1=((_tmp1061.aggr_info=Cyc_Absyn_UnknownAggr(_tmpA2,
_tmpA3,_tmpA4),((_tmp1061.targs=Cyc_Tcutil_copy_types(_tmpA5),_tmp1061)))),
_tmp1062)))),_tmp1060))));}_LL3B: {struct Cyc_Absyn_AggrType_struct*_tmpA6=(
struct Cyc_Absyn_AggrType_struct*)_tmp71;if(_tmpA6->tag != 12)goto _LL3D;else{
_tmpA7=_tmpA6->f1;_tmpA8=_tmpA7.aggr_info;if((_tmpA8.KnownAggr).tag != 2)goto
_LL3D;_tmpA9=(struct Cyc_Absyn_Aggrdecl**)(_tmpA8.KnownAggr).val;_tmpAA=_tmpA7.targs;}}
_LL3C: {struct Cyc_Absyn_AggrType_struct _tmp1068;struct Cyc_Absyn_AggrInfo _tmp1067;
struct Cyc_Absyn_AggrType_struct*_tmp1066;return(void*)((_tmp1066=_cycalloc(
sizeof(*_tmp1066)),((_tmp1066[0]=((_tmp1068.tag=12,((_tmp1068.f1=((_tmp1067.aggr_info=
Cyc_Absyn_KnownAggr(_tmpA9),((_tmp1067.targs=Cyc_Tcutil_copy_types(_tmpAA),
_tmp1067)))),_tmp1068)))),_tmp1066))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpAB=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp71;if(_tmpAB->tag != 13)goto
_LL3F;else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL3E: {struct Cyc_Absyn_AnonAggrType_struct
_tmp106B;struct Cyc_Absyn_AnonAggrType_struct*_tmp106A;return(void*)((_tmp106A=
_cycalloc(sizeof(*_tmp106A)),((_tmp106A[0]=((_tmp106B.tag=13,((_tmp106B.f1=
_tmpAC,((_tmp106B.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmpAD),_tmp106B)))))),_tmp106A))));}_LL3F: {struct Cyc_Absyn_EnumType_struct*
_tmpAE=(struct Cyc_Absyn_EnumType_struct*)_tmp71;if(_tmpAE->tag != 14)goto _LL41;
else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL40: {struct Cyc_Absyn_EnumType_struct
_tmp106E;struct Cyc_Absyn_EnumType_struct*_tmp106D;return(void*)((_tmp106D=
_cycalloc(sizeof(*_tmp106D)),((_tmp106D[0]=((_tmp106E.tag=14,((_tmp106E.f1=
_tmpAF,((_tmp106E.f2=_tmpB0,_tmp106E)))))),_tmp106D))));}_LL41: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB1=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp71;if(_tmpB1->tag != 15)goto
_LL43;else{_tmpB2=_tmpB1->f1;}}_LL42: {struct Cyc_Absyn_AnonEnumType_struct
_tmp1071;struct Cyc_Absyn_AnonEnumType_struct*_tmp1070;return(void*)((_tmp1070=
_cycalloc(sizeof(*_tmp1070)),((_tmp1070[0]=((_tmp1071.tag=15,((_tmp1071.f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB2),_tmp1071)))),
_tmp1070))));}_LL43: {struct Cyc_Absyn_TagType_struct*_tmpB3=(struct Cyc_Absyn_TagType_struct*)
_tmp71;if(_tmpB3->tag != 20)goto _LL45;else{_tmpB4=(void*)_tmpB3->f1;}}_LL44: {
struct Cyc_Absyn_TagType_struct _tmp1074;struct Cyc_Absyn_TagType_struct*_tmp1073;
return(void*)((_tmp1073=_cycalloc(sizeof(*_tmp1073)),((_tmp1073[0]=((_tmp1074.tag=
20,((_tmp1074.f1=(void*)Cyc_Tcutil_copy_type(_tmpB4),_tmp1074)))),_tmp1073))));}
_LL45: {struct Cyc_Absyn_ValueofType_struct*_tmpB5=(struct Cyc_Absyn_ValueofType_struct*)
_tmp71;if(_tmpB5->tag != 19)goto _LL47;else{_tmpB6=_tmpB5->f1;}}_LL46: {struct Cyc_Absyn_ValueofType_struct
_tmp1077;struct Cyc_Absyn_ValueofType_struct*_tmp1076;return(void*)((_tmp1076=
_cycalloc(sizeof(*_tmp1076)),((_tmp1076[0]=((_tmp1077.tag=19,((_tmp1077.f1=
_tmpB6,_tmp1077)))),_tmp1076))));}_LL47: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpB7=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp71;if(_tmpB7->tag != 16)goto
_LL49;else{_tmpB8=(void*)_tmpB7->f1;}}_LL48: {struct Cyc_Absyn_RgnHandleType_struct
_tmp107A;struct Cyc_Absyn_RgnHandleType_struct*_tmp1079;return(void*)((_tmp1079=
_cycalloc(sizeof(*_tmp1079)),((_tmp1079[0]=((_tmp107A.tag=16,((_tmp107A.f1=(void*)
Cyc_Tcutil_copy_type(_tmpB8),_tmp107A)))),_tmp1079))));}_LL49: {struct Cyc_Absyn_DynRgnType_struct*
_tmpB9=(struct Cyc_Absyn_DynRgnType_struct*)_tmp71;if(_tmpB9->tag != 17)goto _LL4B;
else{_tmpBA=(void*)_tmpB9->f1;_tmpBB=(void*)_tmpB9->f2;}}_LL4A: {struct Cyc_Absyn_DynRgnType_struct
_tmp107D;struct Cyc_Absyn_DynRgnType_struct*_tmp107C;return(void*)((_tmp107C=
_cycalloc(sizeof(*_tmp107C)),((_tmp107C[0]=((_tmp107D.tag=17,((_tmp107D.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBA),((_tmp107D.f2=(void*)Cyc_Tcutil_copy_type(_tmpBB),
_tmp107D)))))),_tmp107C))));}_LL4B: {struct Cyc_Absyn_TypedefType_struct*_tmpBC=(
struct Cyc_Absyn_TypedefType_struct*)_tmp71;if(_tmpBC->tag != 18)goto _LL4D;else{
_tmpBD=_tmpBC->f1;_tmpBE=_tmpBC->f2;_tmpBF=_tmpBC->f3;}}_LL4C: {struct Cyc_Absyn_TypedefType_struct
_tmp1080;struct Cyc_Absyn_TypedefType_struct*_tmp107F;return(void*)((_tmp107F=
_cycalloc(sizeof(*_tmp107F)),((_tmp107F[0]=((_tmp1080.tag=18,((_tmp1080.f1=
_tmpBD,((_tmp1080.f2=Cyc_Tcutil_copy_types(_tmpBE),((_tmp1080.f3=_tmpBF,((
_tmp1080.f4=0,_tmp1080)))))))))),_tmp107F))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC0=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp71;if(_tmpC0->tag != 22)goto _LL4F;}
_LL4E: goto _LL50;_LL4F: {struct Cyc_Absyn_HeapRgn_struct*_tmpC1=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp71;if(_tmpC1->tag != 21)goto _LL51;}_LL50: return t;_LL51: {struct Cyc_Absyn_AccessEff_struct*
_tmpC2=(struct Cyc_Absyn_AccessEff_struct*)_tmp71;if(_tmpC2->tag != 23)goto _LL53;
else{_tmpC3=(void*)_tmpC2->f1;}}_LL52: {struct Cyc_Absyn_AccessEff_struct _tmp1083;
struct Cyc_Absyn_AccessEff_struct*_tmp1082;return(void*)((_tmp1082=_cycalloc(
sizeof(*_tmp1082)),((_tmp1082[0]=((_tmp1083.tag=23,((_tmp1083.f1=(void*)Cyc_Tcutil_copy_type(
_tmpC3),_tmp1083)))),_tmp1082))));}_LL53: {struct Cyc_Absyn_JoinEff_struct*_tmpC4=(
struct Cyc_Absyn_JoinEff_struct*)_tmp71;if(_tmpC4->tag != 24)goto _LL55;else{_tmpC5=
_tmpC4->f1;}}_LL54: {struct Cyc_Absyn_JoinEff_struct _tmp1086;struct Cyc_Absyn_JoinEff_struct*
_tmp1085;return(void*)((_tmp1085=_cycalloc(sizeof(*_tmp1085)),((_tmp1085[0]=((
_tmp1086.tag=24,((_tmp1086.f1=Cyc_Tcutil_copy_types(_tmpC5),_tmp1086)))),
_tmp1085))));}_LL55: {struct Cyc_Absyn_RgnsEff_struct*_tmpC6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp71;if(_tmpC6->tag != 25)goto _LL20;else{_tmpC7=(void*)_tmpC6->f1;}}_LL56: {
struct Cyc_Absyn_RgnsEff_struct _tmp1089;struct Cyc_Absyn_RgnsEff_struct*_tmp1088;
return(void*)((_tmp1088=_cycalloc(sizeof(*_tmp1088)),((_tmp1088[0]=((_tmp1089.tag=
25,((_tmp1089.f1=(void*)Cyc_Tcutil_copy_type(_tmpC7),_tmp1089)))),_tmp1088))));}
_LL20:;}static void*Cyc_Tcutil_copy_designator(void*d);static void*Cyc_Tcutil_copy_designator(
void*d){void*_tmp107=d;struct Cyc_Absyn_Exp*_tmp109;struct _dyneither_ptr*_tmp10B;
_LL58: {struct Cyc_Absyn_ArrayElement_struct*_tmp108=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp107;if(_tmp108->tag != 0)goto _LL5A;else{_tmp109=_tmp108->f1;}}_LL59: {struct
Cyc_Absyn_ArrayElement_struct _tmp108C;struct Cyc_Absyn_ArrayElement_struct*
_tmp108B;return(void*)((_tmp108B=_cycalloc(sizeof(*_tmp108B)),((_tmp108B[0]=((
_tmp108C.tag=0,((_tmp108C.f1=Cyc_Tcutil_deep_copy_exp(_tmp109),_tmp108C)))),
_tmp108B))));}_LL5A: {struct Cyc_Absyn_FieldName_struct*_tmp10A=(struct Cyc_Absyn_FieldName_struct*)
_tmp107;if(_tmp10A->tag != 1)goto _LL57;else{_tmp10B=_tmp10A->f1;}}_LL5B: return d;
_LL57:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static
struct _tuple18*Cyc_Tcutil_copy_eds(struct _tuple18*e);static struct _tuple18*Cyc_Tcutil_copy_eds(
struct _tuple18*e){struct _tuple18*_tmp108D;return(_tmp108D=_cycalloc(sizeof(*
_tmp108D)),((_tmp108D->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmp108D->f2=Cyc_Tcutil_deep_copy_exp((
e[0]).f2),_tmp108D)))));}struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*
e);struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*e){void*
_tmp10F=e->r;union Cyc_Absyn_Cnst _tmp111;struct _tuple2*_tmp113;void*_tmp114;
struct _tuple2*_tmp116;enum Cyc_Absyn_Primop _tmp118;struct Cyc_List_List*_tmp119;
struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Core_Opt*_tmp11C;struct Cyc_Absyn_Exp*
_tmp11D;struct Cyc_Absyn_Exp*_tmp11F;enum Cyc_Absyn_Incrementor _tmp120;struct Cyc_Absyn_Exp*
_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*
_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*
_tmp12A;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*
_tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_List_List*
_tmp133;struct Cyc_Absyn_VarargCallInfo*_tmp134;struct Cyc_Absyn_Exp*_tmp136;
struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_List_List*
_tmp13B;void*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;int _tmp13F;enum Cyc_Absyn_Coercion
_tmp140;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*
_tmp145;void*_tmp147;struct Cyc_Absyn_Exp*_tmp149;void*_tmp14B;void*_tmp14C;
struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_Absyn_Exp*_tmp150;struct _dyneither_ptr*
_tmp151;int _tmp152;int _tmp153;struct Cyc_Absyn_Exp*_tmp155;struct _dyneither_ptr*
_tmp156;int _tmp157;int _tmp158;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*
_tmp15B;struct Cyc_List_List*_tmp15D;struct _tuple9*_tmp15F;struct _tuple9 _tmp160;
struct Cyc_Core_Opt*_tmp161;struct Cyc_Absyn_Tqual _tmp162;void*_tmp163;struct Cyc_List_List*
_tmp164;struct Cyc_List_List*_tmp166;struct Cyc_Absyn_Vardecl*_tmp168;struct Cyc_Absyn_Exp*
_tmp169;struct Cyc_Absyn_Exp*_tmp16A;int _tmp16B;struct _tuple2*_tmp16D;struct Cyc_List_List*
_tmp16E;struct Cyc_List_List*_tmp16F;struct Cyc_Absyn_Aggrdecl*_tmp170;void*
_tmp172;struct Cyc_List_List*_tmp173;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_Datatypedecl*
_tmp176;struct Cyc_Absyn_Datatypefield*_tmp177;struct _tuple2*_tmp179;struct Cyc_Absyn_Enumdecl*
_tmp17A;struct Cyc_Absyn_Enumfield*_tmp17B;struct _tuple2*_tmp17D;void*_tmp17E;
struct Cyc_Absyn_Enumfield*_tmp17F;struct Cyc_Absyn_MallocInfo _tmp181;int _tmp182;
struct Cyc_Absyn_Exp*_tmp183;void**_tmp184;struct Cyc_Absyn_Exp*_tmp185;int _tmp186;
struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Core_Opt*
_tmp18B;struct Cyc_List_List*_tmp18C;struct Cyc_Absyn_Exp*_tmp18F;struct
_dyneither_ptr*_tmp190;void*_tmp192;_LL5D: {struct Cyc_Absyn_Const_e_struct*
_tmp110=(struct Cyc_Absyn_Const_e_struct*)_tmp10F;if(_tmp110->tag != 0)goto _LL5F;
else{_tmp111=_tmp110->f1;}}_LL5E: return Cyc_Absyn_const_exp(_tmp111,e->loc);_LL5F: {
struct Cyc_Absyn_Var_e_struct*_tmp112=(struct Cyc_Absyn_Var_e_struct*)_tmp10F;if(
_tmp112->tag != 1)goto _LL61;else{_tmp113=_tmp112->f1;_tmp114=(void*)_tmp112->f2;}}
_LL60: return Cyc_Absyn_varb_exp(_tmp113,_tmp114,e->loc);_LL61: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp115=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp10F;if(_tmp115->tag != 2)goto
_LL63;else{_tmp116=_tmp115->f1;}}_LL62: return Cyc_Absyn_unknownid_exp(_tmp116,e->loc);
_LL63: {struct Cyc_Absyn_Primop_e_struct*_tmp117=(struct Cyc_Absyn_Primop_e_struct*)
_tmp10F;if(_tmp117->tag != 3)goto _LL65;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}
_LL64: return Cyc_Absyn_primop_exp(_tmp118,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp119),e->loc);_LL65: {struct Cyc_Absyn_AssignOp_e_struct*_tmp11A=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp10F;if(_tmp11A->tag != 4)goto _LL67;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;
_tmp11D=_tmp11A->f3;}}_LL66: {struct Cyc_Core_Opt*_tmp108E;return Cyc_Absyn_assignop_exp(
Cyc_Tcutil_deep_copy_exp(_tmp11B),(unsigned int)_tmp11C?(_tmp108E=
_cycalloc_atomic(sizeof(*_tmp108E)),((_tmp108E->v=(void*)((enum Cyc_Absyn_Primop)
_tmp11C->v),_tmp108E))): 0,Cyc_Tcutil_deep_copy_exp(_tmp11D),e->loc);}_LL67: {
struct Cyc_Absyn_Increment_e_struct*_tmp11E=(struct Cyc_Absyn_Increment_e_struct*)
_tmp10F;if(_tmp11E->tag != 5)goto _LL69;else{_tmp11F=_tmp11E->f1;_tmp120=_tmp11E->f2;}}
_LL68: return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp11F),_tmp120,e->loc);
_LL69: {struct Cyc_Absyn_Conditional_e_struct*_tmp121=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp10F;if(_tmp121->tag != 6)goto _LL6B;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;
_tmp124=_tmp121->f3;}}_LL6A: return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(
_tmp122),Cyc_Tcutil_deep_copy_exp(_tmp123),Cyc_Tcutil_deep_copy_exp(_tmp124),e->loc);
_LL6B: {struct Cyc_Absyn_And_e_struct*_tmp125=(struct Cyc_Absyn_And_e_struct*)
_tmp10F;if(_tmp125->tag != 7)goto _LL6D;else{_tmp126=_tmp125->f1;_tmp127=_tmp125->f2;}}
_LL6C: return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp126),Cyc_Tcutil_deep_copy_exp(
_tmp127),e->loc);_LL6D: {struct Cyc_Absyn_Or_e_struct*_tmp128=(struct Cyc_Absyn_Or_e_struct*)
_tmp10F;if(_tmp128->tag != 8)goto _LL6F;else{_tmp129=_tmp128->f1;_tmp12A=_tmp128->f2;}}
_LL6E: return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp129),Cyc_Tcutil_deep_copy_exp(
_tmp12A),e->loc);_LL6F: {struct Cyc_Absyn_SeqExp_e_struct*_tmp12B=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp10F;if(_tmp12B->tag != 9)goto _LL71;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;}}
_LL70: return Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp12C),Cyc_Tcutil_deep_copy_exp(
_tmp12D),e->loc);_LL71: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp12E=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp10F;if(_tmp12E->tag != 10)goto _LL73;else{_tmp12F=_tmp12E->f1;_tmp130=_tmp12E->f2;}}
_LL72: return Cyc_Absyn_unknowncall_exp(Cyc_Tcutil_deep_copy_exp(_tmp12F),((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp130),e->loc);_LL73: {struct Cyc_Absyn_FnCall_e_struct*
_tmp131=(struct Cyc_Absyn_FnCall_e_struct*)_tmp10F;if(_tmp131->tag != 11)goto _LL75;
else{_tmp132=_tmp131->f1;_tmp133=_tmp131->f2;_tmp134=_tmp131->f3;}}_LL74: {
struct Cyc_Absyn_VarargCallInfo*_tmp194=_tmp134;struct Cyc_Absyn_VarargCallInfo
_tmp195;int _tmp196;struct Cyc_List_List*_tmp197;struct Cyc_Absyn_VarargInfo*
_tmp198;_LLAE: if(_tmp194 == 0)goto _LLB0;_tmp195=*_tmp194;_tmp196=_tmp195.num_varargs;
_tmp197=_tmp195.injectors;_tmp198=_tmp195.vai;_LLAF: {struct Cyc_Absyn_VarargInfo
_tmp19A;struct Cyc_Core_Opt*_tmp19B;struct Cyc_Absyn_Tqual _tmp19C;void*_tmp19D;int
_tmp19E;struct Cyc_Absyn_VarargInfo*_tmp199=_tmp198;_tmp19A=*_tmp199;_tmp19B=
_tmp19A.name;_tmp19C=_tmp19A.tq;_tmp19D=_tmp19A.type;_tmp19E=_tmp19A.inject;{
struct Cyc_Absyn_FnCall_e_struct _tmp1098;struct Cyc_Absyn_VarargInfo*_tmp1097;
struct Cyc_Absyn_VarargCallInfo*_tmp1096;struct Cyc_Absyn_FnCall_e_struct*_tmp1095;
return Cyc_Absyn_new_exp((void*)((_tmp1095=_cycalloc(sizeof(*_tmp1095)),((
_tmp1095[0]=((_tmp1098.tag=11,((_tmp1098.f1=Cyc_Tcutil_deep_copy_exp(_tmp132),((
_tmp1098.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp133),((
_tmp1098.f3=((_tmp1096=_cycalloc(sizeof(*_tmp1096)),((_tmp1096->num_varargs=
_tmp196,((_tmp1096->injectors=_tmp197,((_tmp1096->vai=((_tmp1097=_cycalloc(
sizeof(*_tmp1097)),((_tmp1097->name=_tmp19B,((_tmp1097->tq=_tmp19C,((_tmp1097->type=
Cyc_Tcutil_copy_type(_tmp19D),((_tmp1097->inject=_tmp19E,_tmp1097)))))))))),
_tmp1096)))))))),_tmp1098)))))))),_tmp1095)))),e->loc);};}_LLB0:;_LLB1: {struct
Cyc_Absyn_FnCall_e_struct _tmp109B;struct Cyc_Absyn_FnCall_e_struct*_tmp109A;
return Cyc_Absyn_new_exp((void*)((_tmp109A=_cycalloc(sizeof(*_tmp109A)),((
_tmp109A[0]=((_tmp109B.tag=11,((_tmp109B.f1=Cyc_Tcutil_deep_copy_exp(_tmp132),((
_tmp109B.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp133),((
_tmp109B.f3=_tmp134,_tmp109B)))))))),_tmp109A)))),e->loc);}_LLAD:;}_LL75: {
struct Cyc_Absyn_Throw_e_struct*_tmp135=(struct Cyc_Absyn_Throw_e_struct*)_tmp10F;
if(_tmp135->tag != 12)goto _LL77;else{_tmp136=_tmp135->f1;}}_LL76: return Cyc_Absyn_throw_exp(
Cyc_Tcutil_deep_copy_exp(_tmp136),e->loc);_LL77: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp137=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp10F;if(_tmp137->tag != 13)
goto _LL79;else{_tmp138=_tmp137->f1;}}_LL78: return Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(
_tmp138),e->loc);_LL79: {struct Cyc_Absyn_Instantiate_e_struct*_tmp139=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp10F;if(_tmp139->tag != 14)goto _LL7B;else{_tmp13A=_tmp139->f1;_tmp13B=_tmp139->f2;}}
_LL7A: return Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp13A),((struct
Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp13B),e->loc);_LL7B: {struct Cyc_Absyn_Cast_e_struct*_tmp13C=(struct Cyc_Absyn_Cast_e_struct*)
_tmp10F;if(_tmp13C->tag != 15)goto _LL7D;else{_tmp13D=(void*)_tmp13C->f1;_tmp13E=
_tmp13C->f2;_tmp13F=_tmp13C->f3;_tmp140=_tmp13C->f4;}}_LL7C: return Cyc_Absyn_cast_exp(
Cyc_Tcutil_copy_type(_tmp13D),Cyc_Tcutil_deep_copy_exp(_tmp13E),_tmp13F,_tmp140,
e->loc);_LL7D: {struct Cyc_Absyn_Address_e_struct*_tmp141=(struct Cyc_Absyn_Address_e_struct*)
_tmp10F;if(_tmp141->tag != 16)goto _LL7F;else{_tmp142=_tmp141->f1;}}_LL7E: return
Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp142),e->loc);_LL7F: {struct
Cyc_Absyn_New_e_struct*_tmp143=(struct Cyc_Absyn_New_e_struct*)_tmp10F;if(_tmp143->tag
!= 17)goto _LL81;else{_tmp144=_tmp143->f1;_tmp145=_tmp143->f2;}}_LL80: {struct Cyc_Absyn_Exp*
eo1=_tmp144;if(_tmp144 != 0)eo1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp144);return Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(
_tmp145),e->loc);}_LL81: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp146=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp10F;if(_tmp146->tag != 18)goto _LL83;else{_tmp147=(void*)_tmp146->f1;}}_LL82:
return Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp147),e->loc);_LL83: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp148=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp10F;if(_tmp148->tag != 19)goto _LL85;else{_tmp149=_tmp148->f1;}}_LL84: return
Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp149),e->loc);_LL85: {struct
Cyc_Absyn_Offsetof_e_struct*_tmp14A=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp10F;
if(_tmp14A->tag != 20)goto _LL87;else{_tmp14B=(void*)_tmp14A->f1;_tmp14C=(void*)
_tmp14A->f2;}}_LL86: return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp14B),
_tmp14C,e->loc);_LL87: {struct Cyc_Absyn_Deref_e_struct*_tmp14D=(struct Cyc_Absyn_Deref_e_struct*)
_tmp10F;if(_tmp14D->tag != 21)goto _LL89;else{_tmp14E=_tmp14D->f1;}}_LL88: return
Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp14E),e->loc);_LL89: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp14F=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp10F;if(_tmp14F->tag != 22)goto
_LL8B;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;_tmp152=_tmp14F->f3;_tmp153=
_tmp14F->f4;}}_LL8A: {struct Cyc_Absyn_AggrMember_e_struct _tmp109E;struct Cyc_Absyn_AggrMember_e_struct*
_tmp109D;return Cyc_Absyn_new_exp((void*)((_tmp109D=_cycalloc(sizeof(*_tmp109D)),((
_tmp109D[0]=((_tmp109E.tag=22,((_tmp109E.f1=Cyc_Tcutil_deep_copy_exp(_tmp150),((
_tmp109E.f2=_tmp151,((_tmp109E.f3=_tmp152,((_tmp109E.f4=_tmp153,_tmp109E)))))))))),
_tmp109D)))),e->loc);}_LL8B: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp154=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp10F;if(_tmp154->tag != 23)goto _LL8D;else{
_tmp155=_tmp154->f1;_tmp156=_tmp154->f2;_tmp157=_tmp154->f3;_tmp158=_tmp154->f4;}}
_LL8C: {struct Cyc_Absyn_AggrArrow_e_struct _tmp10A1;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp10A0;return Cyc_Absyn_new_exp((void*)((_tmp10A0=_cycalloc(sizeof(*_tmp10A0)),((
_tmp10A0[0]=((_tmp10A1.tag=23,((_tmp10A1.f1=Cyc_Tcutil_deep_copy_exp(_tmp155),((
_tmp10A1.f2=_tmp156,((_tmp10A1.f3=_tmp157,((_tmp10A1.f4=_tmp158,_tmp10A1)))))))))),
_tmp10A0)))),e->loc);}_LL8D: {struct Cyc_Absyn_Subscript_e_struct*_tmp159=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp10F;if(_tmp159->tag != 24)goto _LL8F;else{
_tmp15A=_tmp159->f1;_tmp15B=_tmp159->f2;}}_LL8E: return Cyc_Absyn_subscript_exp(
Cyc_Tcutil_deep_copy_exp(_tmp15A),Cyc_Tcutil_deep_copy_exp(_tmp15B),e->loc);
_LL8F: {struct Cyc_Absyn_Tuple_e_struct*_tmp15C=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp10F;if(_tmp15C->tag != 25)goto _LL91;else{_tmp15D=_tmp15C->f1;}}_LL90: return
Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp15D),e->loc);
_LL91: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp15E=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp10F;if(_tmp15E->tag != 26)goto _LL93;else{_tmp15F=_tmp15E->f1;_tmp160=*_tmp15F;
_tmp161=_tmp160.f1;_tmp162=_tmp160.f2;_tmp163=_tmp160.f3;_tmp164=_tmp15E->f2;}}
_LL92: {struct Cyc_Core_Opt*vopt1=_tmp161;if(_tmp161 != 0){struct Cyc_Core_Opt*
_tmp10A2;vopt1=((_tmp10A2=_cycalloc(sizeof(*_tmp10A2)),((_tmp10A2->v=(struct
_dyneither_ptr*)_tmp161->v,_tmp10A2))));}{struct Cyc_Absyn_CompoundLit_e_struct
_tmp10A8;struct _tuple9*_tmp10A7;struct Cyc_Absyn_CompoundLit_e_struct*_tmp10A6;
return Cyc_Absyn_new_exp((void*)((_tmp10A6=_cycalloc(sizeof(*_tmp10A6)),((
_tmp10A6[0]=((_tmp10A8.tag=26,((_tmp10A8.f1=((_tmp10A7=_cycalloc(sizeof(*
_tmp10A7)),((_tmp10A7->f1=vopt1,((_tmp10A7->f2=_tmp162,((_tmp10A7->f3=Cyc_Tcutil_copy_type(
_tmp163),_tmp10A7)))))))),((_tmp10A8.f2=((struct Cyc_List_List*(*)(struct _tuple18*(*
f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp164),_tmp10A8)))))),_tmp10A6)))),e->loc);};}_LL93: {struct Cyc_Absyn_Array_e_struct*
_tmp165=(struct Cyc_Absyn_Array_e_struct*)_tmp10F;if(_tmp165->tag != 27)goto _LL95;
else{_tmp166=_tmp165->f1;}}_LL94: {struct Cyc_Absyn_Array_e_struct _tmp10AB;struct
Cyc_Absyn_Array_e_struct*_tmp10AA;return Cyc_Absyn_new_exp((void*)((_tmp10AA=
_cycalloc(sizeof(*_tmp10AA)),((_tmp10AA[0]=((_tmp10AB.tag=27,((_tmp10AB.f1=((
struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp166),_tmp10AB)))),_tmp10AA)))),e->loc);}
_LL95: {struct Cyc_Absyn_Comprehension_e_struct*_tmp167=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp10F;if(_tmp167->tag != 28)goto _LL97;else{_tmp168=_tmp167->f1;_tmp169=_tmp167->f2;
_tmp16A=_tmp167->f3;_tmp16B=_tmp167->f4;}}_LL96: {struct Cyc_Absyn_Comprehension_e_struct
_tmp10AE;struct Cyc_Absyn_Comprehension_e_struct*_tmp10AD;return Cyc_Absyn_new_exp((
void*)((_tmp10AD=_cycalloc(sizeof(*_tmp10AD)),((_tmp10AD[0]=((_tmp10AE.tag=28,((
_tmp10AE.f1=_tmp168,((_tmp10AE.f2=Cyc_Tcutil_deep_copy_exp(_tmp169),((_tmp10AE.f3=
Cyc_Tcutil_deep_copy_exp(_tmp16A),((_tmp10AE.f4=_tmp16B,_tmp10AE)))))))))),
_tmp10AD)))),e->loc);}_LL97: {struct Cyc_Absyn_Aggregate_e_struct*_tmp16C=(struct
Cyc_Absyn_Aggregate_e_struct*)_tmp10F;if(_tmp16C->tag != 29)goto _LL99;else{
_tmp16D=_tmp16C->f1;_tmp16E=_tmp16C->f2;_tmp16F=_tmp16C->f3;_tmp170=_tmp16C->f4;}}
_LL98: {struct Cyc_Absyn_Aggregate_e_struct _tmp10B1;struct Cyc_Absyn_Aggregate_e_struct*
_tmp10B0;return Cyc_Absyn_new_exp((void*)((_tmp10B0=_cycalloc(sizeof(*_tmp10B0)),((
_tmp10B0[0]=((_tmp10B1.tag=29,((_tmp10B1.f1=_tmp16D,((_tmp10B1.f2=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp16E),((_tmp10B1.f3=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct
_tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp16F),((
_tmp10B1.f4=_tmp170,_tmp10B1)))))))))),_tmp10B0)))),e->loc);}_LL99: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp171=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp10F;if(_tmp171->tag != 30)goto
_LL9B;else{_tmp172=(void*)_tmp171->f1;_tmp173=_tmp171->f2;}}_LL9A: {struct Cyc_Absyn_AnonStruct_e_struct
_tmp10B4;struct Cyc_Absyn_AnonStruct_e_struct*_tmp10B3;return Cyc_Absyn_new_exp((
void*)((_tmp10B3=_cycalloc(sizeof(*_tmp10B3)),((_tmp10B3[0]=((_tmp10B4.tag=30,((
_tmp10B4.f1=(void*)Cyc_Tcutil_copy_type(_tmp172),((_tmp10B4.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp173),_tmp10B4)))))),_tmp10B3)))),e->loc);}_LL9B: {struct Cyc_Absyn_Datatype_e_struct*
_tmp174=(struct Cyc_Absyn_Datatype_e_struct*)_tmp10F;if(_tmp174->tag != 31)goto
_LL9D;else{_tmp175=_tmp174->f1;_tmp176=_tmp174->f2;_tmp177=_tmp174->f3;}}_LL9C: {
struct Cyc_Absyn_Datatype_e_struct _tmp10B7;struct Cyc_Absyn_Datatype_e_struct*
_tmp10B6;return Cyc_Absyn_new_exp((void*)((_tmp10B6=_cycalloc(sizeof(*_tmp10B6)),((
_tmp10B6[0]=((_tmp10B7.tag=31,((_tmp10B7.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp175),((_tmp10B7.f2=_tmp176,((_tmp10B7.f3=_tmp177,_tmp10B7)))))))),_tmp10B6)))),
e->loc);}_LL9D: {struct Cyc_Absyn_Enum_e_struct*_tmp178=(struct Cyc_Absyn_Enum_e_struct*)
_tmp10F;if(_tmp178->tag != 32)goto _LL9F;else{_tmp179=_tmp178->f1;_tmp17A=_tmp178->f2;
_tmp17B=_tmp178->f3;}}_LL9E: return e;_LL9F: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp17C=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp10F;if(_tmp17C->tag != 33)goto
_LLA1;else{_tmp17D=_tmp17C->f1;_tmp17E=(void*)_tmp17C->f2;_tmp17F=_tmp17C->f3;}}
_LLA0: {struct Cyc_Absyn_AnonEnum_e_struct _tmp10BA;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp10B9;return Cyc_Absyn_new_exp((void*)((_tmp10B9=_cycalloc(sizeof(*_tmp10B9)),((
_tmp10B9[0]=((_tmp10BA.tag=33,((_tmp10BA.f1=_tmp17D,((_tmp10BA.f2=(void*)Cyc_Tcutil_copy_type(
_tmp17E),((_tmp10BA.f3=_tmp17F,_tmp10BA)))))))),_tmp10B9)))),e->loc);}_LLA1: {
struct Cyc_Absyn_Malloc_e_struct*_tmp180=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp10F;if(_tmp180->tag != 34)goto _LLA3;else{_tmp181=_tmp180->f1;_tmp182=_tmp181.is_calloc;
_tmp183=_tmp181.rgn;_tmp184=_tmp181.elt_type;_tmp185=_tmp181.num_elts;_tmp186=
_tmp181.fat_result;}}_LLA2: {struct Cyc_Absyn_Exp*_tmp1B9=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp183;if(_tmp183 != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp183);{void**t1=_tmp184;if(_tmp184 != 0){void**_tmp10BB;
t1=((_tmp10BB=_cycalloc(sizeof(*_tmp10BB)),((_tmp10BB[0]=Cyc_Tcutil_copy_type(*
_tmp184),_tmp10BB))));}{struct Cyc_Absyn_Malloc_e_struct _tmp10C1;struct Cyc_Absyn_MallocInfo
_tmp10C0;struct Cyc_Absyn_Malloc_e_struct*_tmp10BF;_tmp1B9->r=(void*)((_tmp10BF=
_cycalloc(sizeof(*_tmp10BF)),((_tmp10BF[0]=((_tmp10C1.tag=34,((_tmp10C1.f1=((
_tmp10C0.is_calloc=_tmp182,((_tmp10C0.rgn=r1,((_tmp10C0.elt_type=t1,((_tmp10C0.num_elts=
_tmp185,((_tmp10C0.fat_result=_tmp186,_tmp10C0)))))))))),_tmp10C1)))),_tmp10BF))));}
return _tmp1B9;};}_LLA3: {struct Cyc_Absyn_Swap_e_struct*_tmp187=(struct Cyc_Absyn_Swap_e_struct*)
_tmp10F;if(_tmp187->tag != 35)goto _LLA5;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;}}
_LLA4: return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp188),Cyc_Tcutil_deep_copy_exp(
_tmp189),e->loc);_LLA5: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp18A=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp10F;if(_tmp18A->tag != 36)goto _LLA7;else{
_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;}}_LLA6: {struct Cyc_Core_Opt*nopt1=
_tmp18B;if(_tmp18B != 0){struct Cyc_Core_Opt*_tmp10C2;nopt1=((_tmp10C2=_cycalloc(
sizeof(*_tmp10C2)),((_tmp10C2->v=(struct _tuple2*)_tmp18B->v,_tmp10C2))));}{
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp10C5;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp10C4;return Cyc_Absyn_new_exp((void*)((_tmp10C4=_cycalloc(sizeof(*_tmp10C4)),((
_tmp10C4[0]=((_tmp10C5.tag=36,((_tmp10C5.f1=nopt1,((_tmp10C5.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp18C),_tmp10C5)))))),_tmp10C4)))),e->loc);};}_LLA7: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp18D=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp10F;if(_tmp18D->tag != 37)goto
_LLA9;}_LLA8: {struct Cyc_Core_Failure_struct _tmp10CB;const char*_tmp10CA;struct
Cyc_Core_Failure_struct*_tmp10C9;(int)_throw((void*)((_tmp10C9=_cycalloc(sizeof(*
_tmp10C9)),((_tmp10C9[0]=((_tmp10CB.tag=Cyc_Core_Failure,((_tmp10CB.f1=((
_tmp10CA="deep_copy: statement expressions unsupported",_tag_dyneither(_tmp10CA,
sizeof(char),45))),_tmp10CB)))),_tmp10C9)))));}_LLA9: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp18E=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp10F;if(_tmp18E->tag != 38)goto
_LLAB;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;}}_LLAA: {struct Cyc_Absyn_Tagcheck_e_struct
_tmp10CE;struct Cyc_Absyn_Tagcheck_e_struct*_tmp10CD;return Cyc_Absyn_new_exp((
void*)((_tmp10CD=_cycalloc(sizeof(*_tmp10CD)),((_tmp10CD[0]=((_tmp10CE.tag=38,((
_tmp10CE.f1=Cyc_Tcutil_deep_copy_exp(_tmp18F),((_tmp10CE.f2=_tmp190,_tmp10CE)))))),
_tmp10CD)))),e->loc);}_LLAB: {struct Cyc_Absyn_Valueof_e_struct*_tmp191=(struct
Cyc_Absyn_Valueof_e_struct*)_tmp10F;if(_tmp191->tag != 39)goto _LL5C;else{_tmp192=(
void*)_tmp191->f1;}}_LLAC: return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(
_tmp192),e->loc);_LL5C:;}struct _tuple19{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual
f2;};struct _tuple20{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};int
Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2);int Cyc_Tcutil_kind_leq(
struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){struct Cyc_Absyn_Kind _tmp1C7;
enum Cyc_Absyn_KindQual _tmp1C8;enum Cyc_Absyn_AliasQual _tmp1C9;struct Cyc_Absyn_Kind*
_tmp1C6=ka1;_tmp1C7=*_tmp1C6;_tmp1C8=_tmp1C7.kind;_tmp1C9=_tmp1C7.aliasqual;{
struct Cyc_Absyn_Kind _tmp1CB;enum Cyc_Absyn_KindQual _tmp1CC;enum Cyc_Absyn_AliasQual
_tmp1CD;struct Cyc_Absyn_Kind*_tmp1CA=ka2;_tmp1CB=*_tmp1CA;_tmp1CC=_tmp1CB.kind;
_tmp1CD=_tmp1CB.aliasqual;if(_tmp1C8 != _tmp1CC){struct _tuple19 _tmp10CF;struct
_tuple19 _tmp1CF=(_tmp10CF.f1=_tmp1C8,((_tmp10CF.f2=_tmp1CC,_tmp10CF)));enum Cyc_Absyn_KindQual
_tmp1D0;enum Cyc_Absyn_KindQual _tmp1D1;enum Cyc_Absyn_KindQual _tmp1D2;enum Cyc_Absyn_KindQual
_tmp1D3;enum Cyc_Absyn_KindQual _tmp1D4;enum Cyc_Absyn_KindQual _tmp1D5;_LLB3:
_tmp1D0=_tmp1CF.f1;if(_tmp1D0 != Cyc_Absyn_BoxKind)goto _LLB5;_tmp1D1=_tmp1CF.f2;
if(_tmp1D1 != Cyc_Absyn_MemKind)goto _LLB5;_LLB4: goto _LLB6;_LLB5: _tmp1D2=_tmp1CF.f1;
if(_tmp1D2 != Cyc_Absyn_BoxKind)goto _LLB7;_tmp1D3=_tmp1CF.f2;if(_tmp1D3 != Cyc_Absyn_AnyKind)
goto _LLB7;_LLB6: goto _LLB8;_LLB7: _tmp1D4=_tmp1CF.f1;if(_tmp1D4 != Cyc_Absyn_MemKind)
goto _LLB9;_tmp1D5=_tmp1CF.f2;if(_tmp1D5 != Cyc_Absyn_AnyKind)goto _LLB9;_LLB8: goto
_LLB2;_LLB9:;_LLBA: return 0;_LLB2:;}if(_tmp1C9 != _tmp1CD){struct _tuple20 _tmp10D0;
struct _tuple20 _tmp1D7=(_tmp10D0.f1=_tmp1C9,((_tmp10D0.f2=_tmp1CD,_tmp10D0)));
enum Cyc_Absyn_AliasQual _tmp1D8;enum Cyc_Absyn_AliasQual _tmp1D9;enum Cyc_Absyn_AliasQual
_tmp1DA;enum Cyc_Absyn_AliasQual _tmp1DB;_LLBC: _tmp1D8=_tmp1D7.f1;if(_tmp1D8 != 
Cyc_Absyn_Aliasable)goto _LLBE;_tmp1D9=_tmp1D7.f2;if(_tmp1D9 != Cyc_Absyn_Top)goto
_LLBE;_LLBD: goto _LLBF;_LLBE: _tmp1DA=_tmp1D7.f1;if(_tmp1DA != Cyc_Absyn_Unique)
goto _LLC0;_tmp1DB=_tmp1D7.f2;if(_tmp1DB != Cyc_Absyn_Top)goto _LLC0;_LLBF: return 1;
_LLC0:;_LLC1: return 0;_LLBB:;}return 1;};}struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){void*_tmp1DC=Cyc_Absyn_compress_kb(
tv->kind);struct Cyc_Absyn_Kind*_tmp1DE;struct Cyc_Absyn_Kind*_tmp1E0;_LLC3: {
struct Cyc_Absyn_Eq_kb_struct*_tmp1DD=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1DC;if(
_tmp1DD->tag != 0)goto _LLC5;else{_tmp1DE=_tmp1DD->f1;}}_LLC4: return _tmp1DE;_LLC5: {
struct Cyc_Absyn_Less_kb_struct*_tmp1DF=(struct Cyc_Absyn_Less_kb_struct*)_tmp1DC;
if(_tmp1DF->tag != 2)goto _LLC7;else{_tmp1E0=_tmp1DF->f2;}}_LLC6: return _tmp1E0;
_LLC7:;_LLC8:{struct Cyc_Absyn_Less_kb_struct _tmp10D3;struct Cyc_Absyn_Less_kb_struct*
_tmp10D2;tv->kind=(void*)((_tmp10D2=_cycalloc(sizeof(*_tmp10D2)),((_tmp10D2[0]=((
_tmp10D3.tag=2,((_tmp10D3.f1=0,((_tmp10D3.f2=def,_tmp10D3)))))),_tmp10D2))));}
return def;_LLC2:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2);int Cyc_Tcutil_unify_kindbound(
void*kb1,void*kb2){struct _tuple0 _tmp10D4;struct _tuple0 _tmp1E4=(_tmp10D4.f1=Cyc_Absyn_compress_kb(
kb1),((_tmp10D4.f2=Cyc_Absyn_compress_kb(kb2),_tmp10D4)));void*_tmp1E5;struct Cyc_Absyn_Kind*
_tmp1E7;void*_tmp1E8;struct Cyc_Absyn_Kind*_tmp1EA;void*_tmp1EB;struct Cyc_Core_Opt*
_tmp1ED;struct Cyc_Core_Opt**_tmp1EE;struct Cyc_Absyn_Kind*_tmp1EF;void*_tmp1F0;
struct Cyc_Absyn_Kind*_tmp1F2;void*_tmp1F3;struct Cyc_Absyn_Kind*_tmp1F5;void*
_tmp1F6;struct Cyc_Core_Opt*_tmp1F8;struct Cyc_Core_Opt**_tmp1F9;struct Cyc_Absyn_Kind*
_tmp1FA;void*_tmp1FB;struct Cyc_Core_Opt*_tmp1FD;struct Cyc_Core_Opt**_tmp1FE;
struct Cyc_Absyn_Kind*_tmp1FF;void*_tmp200;struct Cyc_Core_Opt*_tmp202;struct Cyc_Core_Opt**
_tmp203;struct Cyc_Absyn_Kind*_tmp204;void*_tmp205;struct Cyc_Core_Opt*_tmp207;
struct Cyc_Core_Opt**_tmp208;void*_tmp209;void*_tmp20A;void*_tmp20B;struct Cyc_Core_Opt*
_tmp20D;struct Cyc_Core_Opt**_tmp20E;_LLCA: _tmp1E5=_tmp1E4.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp1E6=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1E5;if(_tmp1E6->tag != 0)goto _LLCC;
else{_tmp1E7=_tmp1E6->f1;}};_tmp1E8=_tmp1E4.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp1E9=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1E8;if(_tmp1E9->tag != 0)goto _LLCC;
else{_tmp1EA=_tmp1E9->f1;}};_LLCB: return _tmp1E7 == _tmp1EA;_LLCC: _tmp1EB=_tmp1E4.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp1EC=(struct Cyc_Absyn_Less_kb_struct*)_tmp1EB;
if(_tmp1EC->tag != 2)goto _LLCE;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=(struct Cyc_Core_Opt**)&
_tmp1EC->f1;_tmp1EF=_tmp1EC->f2;}};_tmp1F0=_tmp1E4.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp1F1=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1F0;if(_tmp1F1->tag != 0)goto _LLCE;
else{_tmp1F2=_tmp1F1->f1;}};_LLCD: if(Cyc_Tcutil_kind_leq(_tmp1F2,_tmp1EF)){{void**
_tmp10D5;*_tmp1EE=(struct Cyc_Core_Opt*)((_tmp10D5=_cycalloc(sizeof(*_tmp10D5)),((
_tmp10D5[0]=kb2,_tmp10D5))));}return 1;}else{return 0;}_LLCE: _tmp1F3=_tmp1E4.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp1F4=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1F3;if(
_tmp1F4->tag != 0)goto _LLD0;else{_tmp1F5=_tmp1F4->f1;}};_tmp1F6=_tmp1E4.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp1F7=(struct Cyc_Absyn_Less_kb_struct*)_tmp1F6;
if(_tmp1F7->tag != 2)goto _LLD0;else{_tmp1F8=_tmp1F7->f1;_tmp1F9=(struct Cyc_Core_Opt**)&
_tmp1F7->f1;_tmp1FA=_tmp1F7->f2;}};_LLCF: if(Cyc_Tcutil_kind_leq(_tmp1F5,_tmp1FA)){{
void**_tmp10D6;*_tmp1F9=(struct Cyc_Core_Opt*)((_tmp10D6=_cycalloc(sizeof(*
_tmp10D6)),((_tmp10D6[0]=kb1,_tmp10D6))));}return 1;}else{return 0;}_LLD0: _tmp1FB=
_tmp1E4.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp1FC=(struct Cyc_Absyn_Less_kb_struct*)
_tmp1FB;if(_tmp1FC->tag != 2)goto _LLD2;else{_tmp1FD=_tmp1FC->f1;_tmp1FE=(struct
Cyc_Core_Opt**)& _tmp1FC->f1;_tmp1FF=_tmp1FC->f2;}};_tmp200=_tmp1E4.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp201=(struct Cyc_Absyn_Less_kb_struct*)_tmp200;if(_tmp201->tag != 2)goto _LLD2;
else{_tmp202=_tmp201->f1;_tmp203=(struct Cyc_Core_Opt**)& _tmp201->f1;_tmp204=
_tmp201->f2;}};_LLD1: if(Cyc_Tcutil_kind_leq(_tmp1FF,_tmp204)){{void**_tmp10D7;*
_tmp203=(struct Cyc_Core_Opt*)((_tmp10D7=_cycalloc(sizeof(*_tmp10D7)),((_tmp10D7[
0]=kb1,_tmp10D7))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp204,_tmp1FF)){{
void**_tmp10D8;*_tmp1FE=(struct Cyc_Core_Opt*)((_tmp10D8=_cycalloc(sizeof(*
_tmp10D8)),((_tmp10D8[0]=kb2,_tmp10D8))));}return 1;}else{return 0;}}_LLD2: _tmp205=
_tmp1E4.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp206=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp205;if(_tmp206->tag != 1)goto _LLD4;else{_tmp207=_tmp206->f1;_tmp208=(struct
Cyc_Core_Opt**)& _tmp206->f1;}};_tmp209=_tmp1E4.f2;_LLD3: _tmp20A=_tmp209;_tmp20E=
_tmp208;goto _LLD5;_LLD4: _tmp20A=_tmp1E4.f1;_tmp20B=_tmp1E4.f2;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp20C=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp20B;if(_tmp20C->tag != 1)goto
_LLC9;else{_tmp20D=_tmp20C->f1;_tmp20E=(struct Cyc_Core_Opt**)& _tmp20C->f1;}};
_LLD5:{void**_tmp10D9;*_tmp20E=(struct Cyc_Core_Opt*)((_tmp10D9=_cycalloc(sizeof(*
_tmp10D9)),((_tmp10D9[0]=_tmp20A,_tmp10D9))));}return 1;_LLC9:;}struct _tuple15 Cyc_Tcutil_swap_kind(
void*t,void*kb);struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb){void*_tmp214=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp216;_LLD7: {struct Cyc_Absyn_VarType_struct*
_tmp215=(struct Cyc_Absyn_VarType_struct*)_tmp214;if(_tmp215->tag != 2)goto _LLD9;
else{_tmp216=_tmp215->f1;}}_LLD8: {void*_tmp217=_tmp216->kind;_tmp216->kind=kb;{
struct _tuple15 _tmp10DA;return(_tmp10DA.f1=_tmp216,((_tmp10DA.f2=_tmp217,_tmp10DA)));};}
_LLD9:;_LLDA: {const char*_tmp10DE;void*_tmp10DD[1];struct Cyc_String_pa_struct
_tmp10DC;(_tmp10DC.tag=0,((_tmp10DC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp10DD[0]=& _tmp10DC,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10DE="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmp10DE,sizeof(char),40))),_tag_dyneither(_tmp10DD,sizeof(void*),
1)))))));}_LLD6:;}struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);struct Cyc_Absyn_Kind*
Cyc_Tcutil_typ_kind(void*t){void*_tmp21C=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*
_tmp21E;struct Cyc_Absyn_Tvar*_tmp220;enum Cyc_Absyn_Size_of _tmp223;struct Cyc_Absyn_DatatypeFieldInfo
_tmp22F;union Cyc_Absyn_DatatypeFieldInfoU _tmp230;struct _tuple3 _tmp231;struct Cyc_Absyn_Datatypedecl*
_tmp232;struct Cyc_Absyn_Datatypefield*_tmp233;struct Cyc_Absyn_DatatypeFieldInfo
_tmp235;union Cyc_Absyn_DatatypeFieldInfoU _tmp236;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmp237;struct Cyc_Absyn_AggrInfo _tmp239;union Cyc_Absyn_AggrInfoU _tmp23A;struct
_tuple4 _tmp23B;struct Cyc_Absyn_AggrInfo _tmp23D;union Cyc_Absyn_AggrInfoU _tmp23E;
struct Cyc_Absyn_Aggrdecl**_tmp23F;struct Cyc_Absyn_Aggrdecl*_tmp240;struct Cyc_Absyn_Aggrdecl
_tmp241;struct Cyc_Absyn_AggrdeclImpl*_tmp242;struct Cyc_Absyn_PtrInfo _tmp245;
struct Cyc_Absyn_Typedefdecl*_tmp24B;_LLDC: {struct Cyc_Absyn_Evar_struct*_tmp21D=(
struct Cyc_Absyn_Evar_struct*)_tmp21C;if(_tmp21D->tag != 1)goto _LLDE;else{_tmp21E=
_tmp21D->f1;}}_LLDD: return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(_tmp21E))->v;_LLDE: {struct Cyc_Absyn_VarType_struct*_tmp21F=(struct
Cyc_Absyn_VarType_struct*)_tmp21C;if(_tmp21F->tag != 2)goto _LLE0;else{_tmp220=
_tmp21F->f1;}}_LLDF: return Cyc_Tcutil_tvar_kind(_tmp220,& Cyc_Tcutil_bk);_LLE0: {
struct Cyc_Absyn_VoidType_struct*_tmp221=(struct Cyc_Absyn_VoidType_struct*)
_tmp21C;if(_tmp221->tag != 0)goto _LLE2;}_LLE1: return& Cyc_Tcutil_mk;_LLE2: {struct
Cyc_Absyn_IntType_struct*_tmp222=(struct Cyc_Absyn_IntType_struct*)_tmp21C;if(
_tmp222->tag != 6)goto _LLE4;else{_tmp223=_tmp222->f2;}}_LLE3: return(_tmp223 == (
enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz  || _tmp223 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Long_sz)?&
Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLE4: {struct Cyc_Absyn_FloatType_struct*_tmp224=(
struct Cyc_Absyn_FloatType_struct*)_tmp21C;if(_tmp224->tag != 7)goto _LLE6;}_LLE5:
goto _LLE7;_LLE6: {struct Cyc_Absyn_DoubleType_struct*_tmp225=(struct Cyc_Absyn_DoubleType_struct*)
_tmp21C;if(_tmp225->tag != 8)goto _LLE8;}_LLE7: goto _LLE9;_LLE8: {struct Cyc_Absyn_FnType_struct*
_tmp226=(struct Cyc_Absyn_FnType_struct*)_tmp21C;if(_tmp226->tag != 10)goto _LLEA;}
_LLE9: return& Cyc_Tcutil_mk;_LLEA: {struct Cyc_Absyn_DynRgnType_struct*_tmp227=(
struct Cyc_Absyn_DynRgnType_struct*)_tmp21C;if(_tmp227->tag != 17)goto _LLEC;}_LLEB:
goto _LLED;_LLEC: {struct Cyc_Absyn_EnumType_struct*_tmp228=(struct Cyc_Absyn_EnumType_struct*)
_tmp21C;if(_tmp228->tag != 14)goto _LLEE;}_LLED: goto _LLEF;_LLEE: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp229=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp21C;if(_tmp229->tag != 15)goto
_LLF0;}_LLEF: goto _LLF1;_LLF0: {struct Cyc_Absyn_RgnHandleType_struct*_tmp22A=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp21C;if(_tmp22A->tag != 16)goto _LLF2;}
_LLF1: return& Cyc_Tcutil_bk;_LLF2: {struct Cyc_Absyn_UniqueRgn_struct*_tmp22B=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp21C;if(_tmp22B->tag != 22)goto _LLF4;}_LLF3:
return& Cyc_Tcutil_urk;_LLF4: {struct Cyc_Absyn_HeapRgn_struct*_tmp22C=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp21C;if(_tmp22C->tag != 21)goto _LLF6;}_LLF5: return& Cyc_Tcutil_rk;_LLF6: {
struct Cyc_Absyn_DatatypeType_struct*_tmp22D=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp21C;if(_tmp22D->tag != 3)goto _LLF8;}_LLF7: return& Cyc_Tcutil_ak;_LLF8: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp22E=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp21C;if(_tmp22E->tag != 4)goto _LLFA;else{_tmp22F=_tmp22E->f1;_tmp230=_tmp22F.field_info;
if((_tmp230.KnownDatatypefield).tag != 2)goto _LLFA;_tmp231=(struct _tuple3)(
_tmp230.KnownDatatypefield).val;_tmp232=_tmp231.f1;_tmp233=_tmp231.f2;}}_LLF9:
return& Cyc_Tcutil_mk;_LLFA: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp234=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp21C;if(_tmp234->tag != 4)goto _LLFC;
else{_tmp235=_tmp234->f1;_tmp236=_tmp235.field_info;if((_tmp236.UnknownDatatypefield).tag
!= 1)goto _LLFC;_tmp237=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp236.UnknownDatatypefield).val;}}
_LLFB: {const char*_tmp10E1;void*_tmp10E0;(_tmp10E0=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E1="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmp10E1,sizeof(char),39))),_tag_dyneither(_tmp10E0,sizeof(void*),
0)));}_LLFC: {struct Cyc_Absyn_AggrType_struct*_tmp238=(struct Cyc_Absyn_AggrType_struct*)
_tmp21C;if(_tmp238->tag != 12)goto _LLFE;else{_tmp239=_tmp238->f1;_tmp23A=_tmp239.aggr_info;
if((_tmp23A.UnknownAggr).tag != 1)goto _LLFE;_tmp23B=(struct _tuple4)(_tmp23A.UnknownAggr).val;}}
_LLFD: return& Cyc_Tcutil_ak;_LLFE: {struct Cyc_Absyn_AggrType_struct*_tmp23C=(
struct Cyc_Absyn_AggrType_struct*)_tmp21C;if(_tmp23C->tag != 12)goto _LL100;else{
_tmp23D=_tmp23C->f1;_tmp23E=_tmp23D.aggr_info;if((_tmp23E.KnownAggr).tag != 2)
goto _LL100;_tmp23F=(struct Cyc_Absyn_Aggrdecl**)(_tmp23E.KnownAggr).val;_tmp240=*
_tmp23F;_tmp241=*_tmp240;_tmp242=_tmp241.impl;}}_LLFF: return _tmp242 == 0?& Cyc_Tcutil_ak:&
Cyc_Tcutil_mk;_LL100: {struct Cyc_Absyn_AnonAggrType_struct*_tmp243=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp21C;if(_tmp243->tag != 13)goto _LL102;}_LL101: return& Cyc_Tcutil_mk;_LL102: {
struct Cyc_Absyn_PointerType_struct*_tmp244=(struct Cyc_Absyn_PointerType_struct*)
_tmp21C;if(_tmp244->tag != 5)goto _LL104;else{_tmp245=_tmp244->f1;}}_LL103: {void*
_tmp251=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,(_tmp245.ptr_atts).bounds);_LL115: {struct Cyc_Absyn_DynEither_b_struct*
_tmp252=(struct Cyc_Absyn_DynEither_b_struct*)_tmp251;if(_tmp252->tag != 0)goto
_LL117;}_LL116: {struct Cyc_Absyn_Kind*_tmp10E2;return(_tmp10E2=_cycalloc_atomic(
sizeof(*_tmp10E2)),((_tmp10E2->kind=Cyc_Absyn_MemKind,((_tmp10E2->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp245.ptr_atts).rgn))->aliasqual,_tmp10E2)))));}_LL117: {struct Cyc_Absyn_Upper_b_struct*
_tmp253=(struct Cyc_Absyn_Upper_b_struct*)_tmp251;if(_tmp253->tag != 1)goto _LL114;}
_LL118: {struct Cyc_Absyn_Kind*_tmp10E3;return(_tmp10E3=_cycalloc_atomic(sizeof(*
_tmp10E3)),((_tmp10E3->kind=Cyc_Absyn_BoxKind,((_tmp10E3->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp245.ptr_atts).rgn))->aliasqual,_tmp10E3)))));}_LL114:;}_LL104: {struct Cyc_Absyn_ValueofType_struct*
_tmp246=(struct Cyc_Absyn_ValueofType_struct*)_tmp21C;if(_tmp246->tag != 19)goto
_LL106;}_LL105: return& Cyc_Tcutil_ik;_LL106: {struct Cyc_Absyn_TagType_struct*
_tmp247=(struct Cyc_Absyn_TagType_struct*)_tmp21C;if(_tmp247->tag != 20)goto _LL108;}
_LL107: return& Cyc_Tcutil_bk;_LL108: {struct Cyc_Absyn_ArrayType_struct*_tmp248=(
struct Cyc_Absyn_ArrayType_struct*)_tmp21C;if(_tmp248->tag != 9)goto _LL10A;}_LL109:
goto _LL10B;_LL10A: {struct Cyc_Absyn_TupleType_struct*_tmp249=(struct Cyc_Absyn_TupleType_struct*)
_tmp21C;if(_tmp249->tag != 11)goto _LL10C;}_LL10B: return& Cyc_Tcutil_mk;_LL10C: {
struct Cyc_Absyn_TypedefType_struct*_tmp24A=(struct Cyc_Absyn_TypedefType_struct*)
_tmp21C;if(_tmp24A->tag != 18)goto _LL10E;else{_tmp24B=_tmp24A->f3;}}_LL10D: if(
_tmp24B == 0  || _tmp24B->kind == 0){const char*_tmp10E7;void*_tmp10E6[1];struct Cyc_String_pa_struct
_tmp10E5;(_tmp10E5.tag=0,((_tmp10E5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp10E6[0]=& _tmp10E5,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E7="typ_kind: typedef found: %s",
_tag_dyneither(_tmp10E7,sizeof(char),28))),_tag_dyneither(_tmp10E6,sizeof(void*),
1)))))));}return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp24B->kind))->v;
_LL10E: {struct Cyc_Absyn_AccessEff_struct*_tmp24C=(struct Cyc_Absyn_AccessEff_struct*)
_tmp21C;if(_tmp24C->tag != 23)goto _LL110;}_LL10F: goto _LL111;_LL110: {struct Cyc_Absyn_JoinEff_struct*
_tmp24D=(struct Cyc_Absyn_JoinEff_struct*)_tmp21C;if(_tmp24D->tag != 24)goto _LL112;}
_LL111: goto _LL113;_LL112: {struct Cyc_Absyn_RgnsEff_struct*_tmp24E=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp21C;if(_tmp24E->tag != 25)goto _LLDB;}_LL113: return& Cyc_Tcutil_ek;_LLDB:;}int
Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);int Cyc_Tcutil_kind_eq(
struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){return k1 == k2  || k1->kind == k2->kind
 && k1->aliasqual == k2->aliasqual;}int Cyc_Tcutil_unify(void*t1,void*t2);int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp259;_push_handler(& _tmp259);{int _tmp25B=
0;if(setjmp(_tmp259.handler))_tmp25B=1;if(!_tmp25B){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp25C=1;_npop_handler(0);return _tmp25C;};;_pop_handler();}else{void*_tmp25A=(
void*)_exn_thrown;void*_tmp25E=_tmp25A;_LL11A: {struct Cyc_Tcutil_Unify_struct*
_tmp25F=(struct Cyc_Tcutil_Unify_struct*)_tmp25E;if(_tmp25F->tag != Cyc_Tcutil_Unify)
goto _LL11C;}_LL11B: return 0;_LL11C:;_LL11D:(void)_throw(_tmp25E);_LL119:;}};}
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,void*t);static void Cyc_Tcutil_occurs(void*
evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(
t);{void*_tmp260=t;struct Cyc_Absyn_Tvar*_tmp262;struct Cyc_Core_Opt*_tmp264;
struct Cyc_Core_Opt*_tmp265;struct Cyc_Core_Opt**_tmp266;struct Cyc_Absyn_PtrInfo
_tmp268;struct Cyc_Absyn_ArrayInfo _tmp26A;void*_tmp26B;struct Cyc_Absyn_FnInfo
_tmp26D;struct Cyc_List_List*_tmp26E;struct Cyc_Core_Opt*_tmp26F;void*_tmp270;
struct Cyc_List_List*_tmp271;int _tmp272;struct Cyc_Absyn_VarargInfo*_tmp273;struct
Cyc_List_List*_tmp274;struct Cyc_List_List*_tmp275;struct Cyc_List_List*_tmp277;
struct Cyc_Absyn_DatatypeInfo _tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_List_List*
_tmp27C;struct Cyc_Absyn_DatatypeFieldInfo _tmp27E;struct Cyc_List_List*_tmp27F;
struct Cyc_Absyn_AggrInfo _tmp281;struct Cyc_List_List*_tmp282;struct Cyc_List_List*
_tmp284;void*_tmp286;void*_tmp288;void*_tmp28A;void*_tmp28C;struct Cyc_List_List*
_tmp28E;_LL11F: {struct Cyc_Absyn_VarType_struct*_tmp261=(struct Cyc_Absyn_VarType_struct*)
_tmp260;if(_tmp261->tag != 2)goto _LL121;else{_tmp262=_tmp261->f1;}}_LL120: if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp262)){{
const char*_tmp10E8;Cyc_Tcutil_failure_reason=((_tmp10E8="(type variable would escape scope)",
_tag_dyneither(_tmp10E8,sizeof(char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
goto _LL11E;_LL121: {struct Cyc_Absyn_Evar_struct*_tmp263=(struct Cyc_Absyn_Evar_struct*)
_tmp260;if(_tmp263->tag != 1)goto _LL123;else{_tmp264=_tmp263->f2;_tmp265=_tmp263->f4;
_tmp266=(struct Cyc_Core_Opt**)& _tmp263->f4;}}_LL122: if(t == evar){{const char*
_tmp10E9;Cyc_Tcutil_failure_reason=((_tmp10E9="(occurs check)",_tag_dyneither(
_tmp10E9,sizeof(char),15)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(
_tmp264 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp264->v);else{int problem=0;{
struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*
_tmp266))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}
if(problem){struct Cyc_List_List*_tmp291=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp266))->v;for(0;s != 0;s=s->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){struct Cyc_List_List*_tmp10EA;_tmp291=((_tmp10EA=_cycalloc(sizeof(*
_tmp10EA)),((_tmp10EA->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmp10EA->tl=_tmp291,
_tmp10EA))))));}}}{struct Cyc_Core_Opt*_tmp10EB;*_tmp266=((_tmp10EB=_cycalloc(
sizeof(*_tmp10EB)),((_tmp10EB->v=_tmp291,_tmp10EB))));};}}}goto _LL11E;_LL123: {
struct Cyc_Absyn_PointerType_struct*_tmp267=(struct Cyc_Absyn_PointerType_struct*)
_tmp260;if(_tmp267->tag != 5)goto _LL125;else{_tmp268=_tmp267->f1;}}_LL124: Cyc_Tcutil_occurs(
evar,r,env,_tmp268.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(_tmp268.ptr_atts).rgn);
goto _LL11E;_LL125: {struct Cyc_Absyn_ArrayType_struct*_tmp269=(struct Cyc_Absyn_ArrayType_struct*)
_tmp260;if(_tmp269->tag != 9)goto _LL127;else{_tmp26A=_tmp269->f1;_tmp26B=_tmp26A.elt_type;}}
_LL126: Cyc_Tcutil_occurs(evar,r,env,_tmp26B);goto _LL11E;_LL127: {struct Cyc_Absyn_FnType_struct*
_tmp26C=(struct Cyc_Absyn_FnType_struct*)_tmp260;if(_tmp26C->tag != 10)goto _LL129;
else{_tmp26D=_tmp26C->f1;_tmp26E=_tmp26D.tvars;_tmp26F=_tmp26D.effect;_tmp270=
_tmp26D.ret_typ;_tmp271=_tmp26D.args;_tmp272=_tmp26D.c_varargs;_tmp273=_tmp26D.cyc_varargs;
_tmp274=_tmp26D.rgn_po;_tmp275=_tmp26D.attributes;}}_LL128: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp26E,env);if(_tmp26F != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp26F->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp270);for(0;_tmp271 != 0;_tmp271=_tmp271->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple9*)_tmp271->hd)).f3);}if(_tmp273 != 0)Cyc_Tcutil_occurs(
evar,r,env,_tmp273->type);for(0;_tmp274 != 0;_tmp274=_tmp274->tl){struct _tuple0
_tmp295;void*_tmp296;void*_tmp297;struct _tuple0*_tmp294=(struct _tuple0*)_tmp274->hd;
_tmp295=*_tmp294;_tmp296=_tmp295.f1;_tmp297=_tmp295.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp296);Cyc_Tcutil_occurs(evar,r,env,_tmp297);}goto _LL11E;_LL129: {struct
Cyc_Absyn_TupleType_struct*_tmp276=(struct Cyc_Absyn_TupleType_struct*)_tmp260;
if(_tmp276->tag != 11)goto _LL12B;else{_tmp277=_tmp276->f1;}}_LL12A: for(0;_tmp277
!= 0;_tmp277=_tmp277->tl){Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple11*)
_tmp277->hd)).f2);}goto _LL11E;_LL12B: {struct Cyc_Absyn_DatatypeType_struct*
_tmp278=(struct Cyc_Absyn_DatatypeType_struct*)_tmp260;if(_tmp278->tag != 3)goto
_LL12D;else{_tmp279=_tmp278->f1;_tmp27A=_tmp279.targs;}}_LL12C: Cyc_Tcutil_occurslist(
evar,r,env,_tmp27A);goto _LL11E;_LL12D: {struct Cyc_Absyn_TypedefType_struct*
_tmp27B=(struct Cyc_Absyn_TypedefType_struct*)_tmp260;if(_tmp27B->tag != 18)goto
_LL12F;else{_tmp27C=_tmp27B->f2;}}_LL12E: _tmp27F=_tmp27C;goto _LL130;_LL12F: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp27D=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp260;if(_tmp27D->tag != 4)goto _LL131;else{_tmp27E=_tmp27D->f1;_tmp27F=_tmp27E.targs;}}
_LL130: _tmp282=_tmp27F;goto _LL132;_LL131: {struct Cyc_Absyn_AggrType_struct*
_tmp280=(struct Cyc_Absyn_AggrType_struct*)_tmp260;if(_tmp280->tag != 12)goto
_LL133;else{_tmp281=_tmp280->f1;_tmp282=_tmp281.targs;}}_LL132: Cyc_Tcutil_occurslist(
evar,r,env,_tmp282);goto _LL11E;_LL133: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp283=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp260;if(_tmp283->tag != 13)goto
_LL135;else{_tmp284=_tmp283->f2;}}_LL134: for(0;_tmp284 != 0;_tmp284=_tmp284->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp284->hd)->type);}
goto _LL11E;_LL135: {struct Cyc_Absyn_TagType_struct*_tmp285=(struct Cyc_Absyn_TagType_struct*)
_tmp260;if(_tmp285->tag != 20)goto _LL137;else{_tmp286=(void*)_tmp285->f1;}}_LL136:
_tmp288=_tmp286;goto _LL138;_LL137: {struct Cyc_Absyn_AccessEff_struct*_tmp287=(
struct Cyc_Absyn_AccessEff_struct*)_tmp260;if(_tmp287->tag != 23)goto _LL139;else{
_tmp288=(void*)_tmp287->f1;}}_LL138: _tmp28A=_tmp288;goto _LL13A;_LL139: {struct
Cyc_Absyn_RgnHandleType_struct*_tmp289=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp260;if(_tmp289->tag != 16)goto _LL13B;else{_tmp28A=(void*)_tmp289->f1;}}_LL13A:
_tmp28C=_tmp28A;goto _LL13C;_LL13B: {struct Cyc_Absyn_RgnsEff_struct*_tmp28B=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp260;if(_tmp28B->tag != 25)goto _LL13D;else{
_tmp28C=(void*)_tmp28B->f1;}}_LL13C: Cyc_Tcutil_occurs(evar,r,env,_tmp28C);goto
_LL11E;_LL13D: {struct Cyc_Absyn_JoinEff_struct*_tmp28D=(struct Cyc_Absyn_JoinEff_struct*)
_tmp260;if(_tmp28D->tag != 24)goto _LL13F;else{_tmp28E=_tmp28D->f1;}}_LL13E: Cyc_Tcutil_occurslist(
evar,r,env,_tmp28E);goto _LL11E;_LL13F:;_LL140: goto _LL11E;_LL11E:;};}static void
Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,
struct Cyc_List_List*ts);static void Cyc_Tcutil_occurslist(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=
ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2);static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,
t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}if(t1 != 0  || t2 != 
0)(int)_throw((void*)& Cyc_Tcutil_Unify_val);}static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2);static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const){const char*_tmp10EE;void*_tmp10ED;(_tmp10ED=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10EE="tq1 real_const not set.",
_tag_dyneither(_tmp10EE,sizeof(char),24))),_tag_dyneither(_tmp10ED,sizeof(void*),
0)));}if(tq2.print_const  && !tq2.real_const){const char*_tmp10F1;void*_tmp10F0;(
_tmp10F0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp10F1="tq2 real_const not set.",_tag_dyneither(_tmp10F1,sizeof(char),24))),
_tag_dyneither(_tmp10F0,sizeof(void*),0)));}if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;{const char*_tmp10F2;Cyc_Tcutil_failure_reason=((_tmp10F2="(qualifiers don't match)",
_tag_dyneither(_tmp10F2,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,
struct _dyneither_ptr reason);static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct
_dyneither_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{union Cyc_Absyn_Constraint*_tmp29D=x;union Cyc_Absyn_Constraint
_tmp29E;int _tmp29F;union Cyc_Absyn_Constraint _tmp2A0;void*_tmp2A1;union Cyc_Absyn_Constraint
_tmp2A2;union Cyc_Absyn_Constraint*_tmp2A3;_LL142: _tmp29E=*_tmp29D;if((_tmp29E.No_constr).tag
!= 3)goto _LL144;_tmp29F=(int)(_tmp29E.No_constr).val;_LL143:{union Cyc_Absyn_Constraint
_tmp10F3;*x=(((_tmp10F3.Forward_constr).val=y,(((_tmp10F3.Forward_constr).tag=2,
_tmp10F3))));}return;_LL144: _tmp2A0=*_tmp29D;if((_tmp2A0.Eq_constr).tag != 1)goto
_LL146;_tmp2A1=(void*)(_tmp2A0.Eq_constr).val;_LL145: {union Cyc_Absyn_Constraint*
_tmp2A5=y;union Cyc_Absyn_Constraint _tmp2A6;int _tmp2A7;union Cyc_Absyn_Constraint
_tmp2A8;void*_tmp2A9;union Cyc_Absyn_Constraint _tmp2AA;union Cyc_Absyn_Constraint*
_tmp2AB;_LL149: _tmp2A6=*_tmp2A5;if((_tmp2A6.No_constr).tag != 3)goto _LL14B;
_tmp2A7=(int)(_tmp2A6.No_constr).val;_LL14A:*y=*x;return;_LL14B: _tmp2A8=*_tmp2A5;
if((_tmp2A8.Eq_constr).tag != 1)goto _LL14D;_tmp2A9=(void*)(_tmp2A8.Eq_constr).val;
_LL14C: if(cmp(_tmp2A1,_tmp2A9)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)& Cyc_Tcutil_Unify_val);}return;_LL14D: _tmp2AA=*_tmp2A5;if((_tmp2AA.Forward_constr).tag
!= 2)goto _LL148;_tmp2AB=(union Cyc_Absyn_Constraint*)(_tmp2AA.Forward_constr).val;
_LL14E: {const char*_tmp10F6;void*_tmp10F5;(_tmp10F5=0,Cyc_Tcutil_impos(((
_tmp10F6="unify_conref: forward after compress(2)",_tag_dyneither(_tmp10F6,
sizeof(char),40))),_tag_dyneither(_tmp10F5,sizeof(void*),0)));}_LL148:;}_LL146:
_tmp2A2=*_tmp29D;if((_tmp2A2.Forward_constr).tag != 2)goto _LL141;_tmp2A3=(union
Cyc_Absyn_Constraint*)(_tmp2A2.Forward_constr).val;_LL147: {const char*_tmp10F9;
void*_tmp10F8;(_tmp10F8=0,Cyc_Tcutil_impos(((_tmp10F9="unify_conref: forward after compress",
_tag_dyneither(_tmp10F9,sizeof(char),37))),_tag_dyneither(_tmp10F8,sizeof(void*),
0)));}_LL141:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp2B0;_push_handler(& _tmp2B0);{int _tmp2B2=0;if(setjmp(
_tmp2B0.handler))_tmp2B2=1;if(!_tmp2B2){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(
struct _dyneither_ptr)_tag_dyneither(0,0,0));{int _tmp2B3=1;_npop_handler(0);
return _tmp2B3;};;_pop_handler();}else{void*_tmp2B1=(void*)_exn_thrown;void*
_tmp2B5=_tmp2B1;_LL150: {struct Cyc_Tcutil_Unify_struct*_tmp2B6=(struct Cyc_Tcutil_Unify_struct*)
_tmp2B5;if(_tmp2B6->tag != Cyc_Tcutil_Unify)goto _LL152;}_LL151: return 0;_LL152:;
_LL153:(void)_throw(_tmp2B5);_LL14F:;}};}static int Cyc_Tcutil_boundscmp(void*b1,
void*b2);static int Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple0 _tmp10FA;
struct _tuple0 _tmp2B8=(_tmp10FA.f1=b1,((_tmp10FA.f2=b2,_tmp10FA)));void*_tmp2B9;
void*_tmp2BB;void*_tmp2BD;void*_tmp2BF;void*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C3;
void*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C6;_LL155: _tmp2B9=_tmp2B8.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2BA=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2B9;if(_tmp2BA->tag != 0)goto
_LL157;};_tmp2BB=_tmp2B8.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp2BC=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp2BB;if(_tmp2BC->tag != 0)goto _LL157;};_LL156:
return 0;_LL157: _tmp2BD=_tmp2B8.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp2BE=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp2BD;if(_tmp2BE->tag != 0)goto _LL159;};
_LL158: return - 1;_LL159: _tmp2BF=_tmp2B8.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2C0=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2BF;if(_tmp2C0->tag != 0)goto
_LL15B;};_LL15A: return 1;_LL15B: _tmp2C1=_tmp2B8.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp2C2=(struct Cyc_Absyn_Upper_b_struct*)_tmp2C1;if(_tmp2C2->tag != 1)goto _LL154;
else{_tmp2C3=_tmp2C2->f1;}};_tmp2C4=_tmp2B8.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp2C5=(struct Cyc_Absyn_Upper_b_struct*)_tmp2C4;if(_tmp2C5->tag != 1)goto _LL154;
else{_tmp2C6=_tmp2C5->f1;}};_LL15C: return Cyc_Evexp_const_exp_cmp(_tmp2C3,_tmp2C6);
_LL154:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2);static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2){struct _tuple0 _tmp10FB;struct _tuple0 _tmp2C8=(_tmp10FB.f1=b1,((
_tmp10FB.f2=b2,_tmp10FB)));void*_tmp2C9;void*_tmp2CB;void*_tmp2CD;void*_tmp2CF;
void*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D3;void*_tmp2D4;struct Cyc_Absyn_Exp*
_tmp2D6;_LL15E: _tmp2C9=_tmp2C8.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp2CA=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp2C9;if(_tmp2CA->tag != 0)goto _LL160;};
_tmp2CB=_tmp2C8.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp2CC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2CB;if(_tmp2CC->tag != 0)goto _LL160;};_LL15F: return 0;_LL160: _tmp2CD=_tmp2C8.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp2CE=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2CD;if(_tmp2CE->tag != 0)goto _LL162;};_LL161: return - 1;_LL162: _tmp2CF=_tmp2C8.f2;{
struct Cyc_Absyn_DynEither_b_struct*_tmp2D0=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2CF;if(_tmp2D0->tag != 0)goto _LL164;};_LL163: return 1;_LL164: _tmp2D1=_tmp2C8.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp2D2=(struct Cyc_Absyn_Upper_b_struct*)_tmp2D1;
if(_tmp2D2->tag != 1)goto _LL15D;else{_tmp2D3=_tmp2D2->f1;}};_tmp2D4=_tmp2C8.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp2D5=(struct Cyc_Absyn_Upper_b_struct*)_tmp2D4;
if(_tmp2D5->tag != 1)goto _LL15D;else{_tmp2D6=_tmp2D5->f1;}};_LL165: return Cyc_Evexp_const_exp_cmp(
_tmp2D3,_tmp2D6);_LL15D:;}static int Cyc_Tcutil_attribute_case_number(void*att);
static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp2D7=att;_LL167: {
struct Cyc_Absyn_Regparm_att_struct*_tmp2D8=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp2D7;if(_tmp2D8->tag != 0)goto _LL169;}_LL168: return 0;_LL169: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp2D9=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp2D7;if(_tmp2D9->tag != 1)goto
_LL16B;}_LL16A: return 1;_LL16B: {struct Cyc_Absyn_Cdecl_att_struct*_tmp2DA=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp2D7;if(_tmp2DA->tag != 2)goto _LL16D;}_LL16C:
return 2;_LL16D: {struct Cyc_Absyn_Fastcall_att_struct*_tmp2DB=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp2D7;if(_tmp2DB->tag != 3)goto _LL16F;}_LL16E: return 3;_LL16F: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp2DC=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp2D7;if(_tmp2DC->tag != 4)goto
_LL171;}_LL170: return 4;_LL171: {struct Cyc_Absyn_Const_att_struct*_tmp2DD=(struct
Cyc_Absyn_Const_att_struct*)_tmp2D7;if(_tmp2DD->tag != 5)goto _LL173;}_LL172:
return 5;_LL173: {struct Cyc_Absyn_Aligned_att_struct*_tmp2DE=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp2D7;if(_tmp2DE->tag != 6)goto _LL175;}_LL174: return 6;_LL175: {struct Cyc_Absyn_Packed_att_struct*
_tmp2DF=(struct Cyc_Absyn_Packed_att_struct*)_tmp2D7;if(_tmp2DF->tag != 7)goto
_LL177;}_LL176: return 7;_LL177: {struct Cyc_Absyn_Section_att_struct*_tmp2E0=(
struct Cyc_Absyn_Section_att_struct*)_tmp2D7;if(_tmp2E0->tag != 8)goto _LL179;}
_LL178: return 8;_LL179: {struct Cyc_Absyn_Nocommon_att_struct*_tmp2E1=(struct Cyc_Absyn_Nocommon_att_struct*)
_tmp2D7;if(_tmp2E1->tag != 9)goto _LL17B;}_LL17A: return 9;_LL17B: {struct Cyc_Absyn_Shared_att_struct*
_tmp2E2=(struct Cyc_Absyn_Shared_att_struct*)_tmp2D7;if(_tmp2E2->tag != 10)goto
_LL17D;}_LL17C: return 10;_LL17D: {struct Cyc_Absyn_Unused_att_struct*_tmp2E3=(
struct Cyc_Absyn_Unused_att_struct*)_tmp2D7;if(_tmp2E3->tag != 11)goto _LL17F;}
_LL17E: return 11;_LL17F: {struct Cyc_Absyn_Weak_att_struct*_tmp2E4=(struct Cyc_Absyn_Weak_att_struct*)
_tmp2D7;if(_tmp2E4->tag != 12)goto _LL181;}_LL180: return 12;_LL181: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp2E5=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp2D7;if(_tmp2E5->tag != 13)goto
_LL183;}_LL182: return 13;_LL183: {struct Cyc_Absyn_Dllexport_att_struct*_tmp2E6=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp2D7;if(_tmp2E6->tag != 14)goto _LL185;}
_LL184: return 14;_LL185: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp2E7=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp2D7;if(_tmp2E7->tag
!= 15)goto _LL187;}_LL186: return 15;_LL187: {struct Cyc_Absyn_Constructor_att_struct*
_tmp2E8=(struct Cyc_Absyn_Constructor_att_struct*)_tmp2D7;if(_tmp2E8->tag != 16)
goto _LL189;}_LL188: return 16;_LL189: {struct Cyc_Absyn_Destructor_att_struct*
_tmp2E9=(struct Cyc_Absyn_Destructor_att_struct*)_tmp2D7;if(_tmp2E9->tag != 17)
goto _LL18B;}_LL18A: return 17;_LL18B: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp2EA=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp2D7;if(_tmp2EA->tag
!= 18)goto _LL18D;}_LL18C: return 18;_LL18D: {struct Cyc_Absyn_Format_att_struct*
_tmp2EB=(struct Cyc_Absyn_Format_att_struct*)_tmp2D7;if(_tmp2EB->tag != 19)goto
_LL18F;}_LL18E: return 19;_LL18F: {struct Cyc_Absyn_Initializes_att_struct*_tmp2EC=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp2D7;if(_tmp2EC->tag != 20)goto _LL191;}
_LL190: return 20;_LL191:;_LL192: return 21;_LL166:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2);static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp10FC;struct _tuple0 _tmp2EE=(_tmp10FC.f1=att1,((_tmp10FC.f2=att2,
_tmp10FC)));void*_tmp2EF;int _tmp2F1;void*_tmp2F2;int _tmp2F4;void*_tmp2F5;int
_tmp2F7;void*_tmp2F8;int _tmp2FA;void*_tmp2FB;int _tmp2FD;void*_tmp2FE;int _tmp300;
void*_tmp301;struct _dyneither_ptr _tmp303;void*_tmp304;struct _dyneither_ptr
_tmp306;void*_tmp307;enum Cyc_Absyn_Format_Type _tmp309;int _tmp30A;int _tmp30B;
void*_tmp30C;enum Cyc_Absyn_Format_Type _tmp30E;int _tmp30F;int _tmp310;_LL194:
_tmp2EF=_tmp2EE.f1;{struct Cyc_Absyn_Regparm_att_struct*_tmp2F0=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp2EF;if(_tmp2F0->tag != 0)goto _LL196;else{_tmp2F1=_tmp2F0->f1;}};_tmp2F2=
_tmp2EE.f2;{struct Cyc_Absyn_Regparm_att_struct*_tmp2F3=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp2F2;if(_tmp2F3->tag != 0)goto _LL196;else{_tmp2F4=_tmp2F3->f1;}};_LL195:
_tmp2F7=_tmp2F1;_tmp2FA=_tmp2F4;goto _LL197;_LL196: _tmp2F5=_tmp2EE.f1;{struct Cyc_Absyn_Initializes_att_struct*
_tmp2F6=(struct Cyc_Absyn_Initializes_att_struct*)_tmp2F5;if(_tmp2F6->tag != 20)
goto _LL198;else{_tmp2F7=_tmp2F6->f1;}};_tmp2F8=_tmp2EE.f2;{struct Cyc_Absyn_Initializes_att_struct*
_tmp2F9=(struct Cyc_Absyn_Initializes_att_struct*)_tmp2F8;if(_tmp2F9->tag != 20)
goto _LL198;else{_tmp2FA=_tmp2F9->f1;}};_LL197: _tmp2FD=_tmp2F7;_tmp300=_tmp2FA;
goto _LL199;_LL198: _tmp2FB=_tmp2EE.f1;{struct Cyc_Absyn_Aligned_att_struct*_tmp2FC=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp2FB;if(_tmp2FC->tag != 6)goto _LL19A;else{
_tmp2FD=_tmp2FC->f1;}};_tmp2FE=_tmp2EE.f2;{struct Cyc_Absyn_Aligned_att_struct*
_tmp2FF=(struct Cyc_Absyn_Aligned_att_struct*)_tmp2FE;if(_tmp2FF->tag != 6)goto
_LL19A;else{_tmp300=_tmp2FF->f1;}};_LL199: return Cyc_Core_intcmp(_tmp2FD,_tmp300);
_LL19A: _tmp301=_tmp2EE.f1;{struct Cyc_Absyn_Section_att_struct*_tmp302=(struct Cyc_Absyn_Section_att_struct*)
_tmp301;if(_tmp302->tag != 8)goto _LL19C;else{_tmp303=_tmp302->f1;}};_tmp304=
_tmp2EE.f2;{struct Cyc_Absyn_Section_att_struct*_tmp305=(struct Cyc_Absyn_Section_att_struct*)
_tmp304;if(_tmp305->tag != 8)goto _LL19C;else{_tmp306=_tmp305->f1;}};_LL19B: return
Cyc_strcmp((struct _dyneither_ptr)_tmp303,(struct _dyneither_ptr)_tmp306);_LL19C:
_tmp307=_tmp2EE.f1;{struct Cyc_Absyn_Format_att_struct*_tmp308=(struct Cyc_Absyn_Format_att_struct*)
_tmp307;if(_tmp308->tag != 19)goto _LL19E;else{_tmp309=_tmp308->f1;_tmp30A=_tmp308->f2;
_tmp30B=_tmp308->f3;}};_tmp30C=_tmp2EE.f2;{struct Cyc_Absyn_Format_att_struct*
_tmp30D=(struct Cyc_Absyn_Format_att_struct*)_tmp30C;if(_tmp30D->tag != 19)goto
_LL19E;else{_tmp30E=_tmp30D->f1;_tmp30F=_tmp30D->f2;_tmp310=_tmp30D->f3;}};
_LL19D: {int _tmp311=Cyc_Core_intcmp((int)((unsigned int)_tmp309),(int)((
unsigned int)_tmp30E));if(_tmp311 != 0)return _tmp311;{int _tmp312=Cyc_Core_intcmp(
_tmp30A,_tmp30F);if(_tmp312 != 0)return _tmp312;return Cyc_Core_intcmp(_tmp30B,
_tmp310);};}_LL19E:;_LL19F: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL193:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2);static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2);int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){{
struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,
a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!((int(*)(int(*
pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of(af->type);}static struct
_tuple15*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv);static struct
_tuple15*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;{struct Cyc_Absyn_Kind*
_tmp313=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp314;
enum Cyc_Absyn_KindQual _tmp315;enum Cyc_Absyn_AliasQual _tmp316;struct Cyc_Absyn_Kind
_tmp317;enum Cyc_Absyn_KindQual _tmp318;enum Cyc_Absyn_AliasQual _tmp319;struct Cyc_Absyn_Kind
_tmp31A;enum Cyc_Absyn_KindQual _tmp31B;struct Cyc_Absyn_Kind _tmp31C;enum Cyc_Absyn_KindQual
_tmp31D;_LL1A1: _tmp314=*_tmp313;_tmp315=_tmp314.kind;if(_tmp315 != Cyc_Absyn_RgnKind)
goto _LL1A3;_tmp316=_tmp314.aliasqual;if(_tmp316 != Cyc_Absyn_Unique)goto _LL1A3;
_LL1A2: t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1A0;_LL1A3: _tmp317=*_tmp313;
_tmp318=_tmp317.kind;if(_tmp318 != Cyc_Absyn_RgnKind)goto _LL1A5;_tmp319=_tmp317.aliasqual;
if(_tmp319 != Cyc_Absyn_Aliasable)goto _LL1A5;_LL1A4: t=(void*)& Cyc_Absyn_HeapRgn_val;
goto _LL1A0;_LL1A5: _tmp31A=*_tmp313;_tmp31B=_tmp31A.kind;if(_tmp31B != Cyc_Absyn_EffKind)
goto _LL1A7;_LL1A6: t=Cyc_Absyn_empty_effect;goto _LL1A0;_LL1A7: _tmp31C=*_tmp313;
_tmp31D=_tmp31C.kind;if(_tmp31D != Cyc_Absyn_IntKind)goto _LL1A9;_LL1A8:{struct Cyc_Absyn_ValueofType_struct
_tmp10FF;struct Cyc_Absyn_ValueofType_struct*_tmp10FE;t=(void*)((_tmp10FE=
_cycalloc(sizeof(*_tmp10FE)),((_tmp10FE[0]=((_tmp10FF.tag=19,((_tmp10FF.f1=Cyc_Absyn_uint_exp(
0,0),_tmp10FF)))),_tmp10FE))));}goto _LL1A0;_LL1A9:;_LL1AA: t=Cyc_Absyn_sint_typ;
goto _LL1A0;_LL1A0:;}{struct _tuple15*_tmp1100;return(_tmp1100=_cycalloc(sizeof(*
_tmp1100)),((_tmp1100->f1=tv,((_tmp1100->f2=t,_tmp1100)))));};}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp321=Cyc_Tcutil_compress(t);
void*_tmp32C;void*_tmp32E;void*_tmp32F;struct Cyc_Absyn_DatatypeInfo _tmp331;
struct Cyc_List_List*_tmp332;struct Cyc_Absyn_PtrInfo _tmp334;void*_tmp335;struct
Cyc_Absyn_PtrAtts _tmp336;void*_tmp337;struct Cyc_Absyn_ArrayInfo _tmp339;void*
_tmp33A;struct Cyc_List_List*_tmp33C;struct Cyc_Absyn_DatatypeFieldInfo _tmp33E;
struct Cyc_List_List*_tmp33F;struct Cyc_Absyn_AggrInfo _tmp341;struct Cyc_List_List*
_tmp342;struct Cyc_List_List*_tmp344;struct Cyc_Absyn_FnInfo _tmp347;struct Cyc_List_List*
_tmp348;struct Cyc_Core_Opt*_tmp349;void*_tmp34A;struct Cyc_List_List*_tmp34B;
struct Cyc_Absyn_VarargInfo*_tmp34C;struct Cyc_List_List*_tmp34D;void*_tmp353;
struct Cyc_List_List*_tmp355;_LL1AC: {struct Cyc_Absyn_VoidType_struct*_tmp322=(
struct Cyc_Absyn_VoidType_struct*)_tmp321;if(_tmp322->tag != 0)goto _LL1AE;}_LL1AD:
goto _LL1AF;_LL1AE: {struct Cyc_Absyn_FloatType_struct*_tmp323=(struct Cyc_Absyn_FloatType_struct*)
_tmp321;if(_tmp323->tag != 7)goto _LL1B0;}_LL1AF: goto _LL1B1;_LL1B0: {struct Cyc_Absyn_DoubleType_struct*
_tmp324=(struct Cyc_Absyn_DoubleType_struct*)_tmp321;if(_tmp324->tag != 8)goto
_LL1B2;}_LL1B1: goto _LL1B3;_LL1B2: {struct Cyc_Absyn_EnumType_struct*_tmp325=(
struct Cyc_Absyn_EnumType_struct*)_tmp321;if(_tmp325->tag != 14)goto _LL1B4;}_LL1B3:
goto _LL1B5;_LL1B4: {struct Cyc_Absyn_AnonEnumType_struct*_tmp326=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp321;if(_tmp326->tag != 15)goto _LL1B6;}_LL1B5: goto _LL1B7;_LL1B6: {struct Cyc_Absyn_ValueofType_struct*
_tmp327=(struct Cyc_Absyn_ValueofType_struct*)_tmp321;if(_tmp327->tag != 19)goto
_LL1B8;}_LL1B7: goto _LL1B9;_LL1B8: {struct Cyc_Absyn_IntType_struct*_tmp328=(
struct Cyc_Absyn_IntType_struct*)_tmp321;if(_tmp328->tag != 6)goto _LL1BA;}_LL1B9:
return Cyc_Absyn_empty_effect;_LL1BA: {struct Cyc_Absyn_Evar_struct*_tmp329=(
struct Cyc_Absyn_Evar_struct*)_tmp321;if(_tmp329->tag != 1)goto _LL1BC;}_LL1BB: goto
_LL1BD;_LL1BC: {struct Cyc_Absyn_VarType_struct*_tmp32A=(struct Cyc_Absyn_VarType_struct*)
_tmp321;if(_tmp32A->tag != 2)goto _LL1BE;}_LL1BD: {struct Cyc_Absyn_Kind*_tmp356=
Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind _tmp357;enum Cyc_Absyn_KindQual
_tmp358;struct Cyc_Absyn_Kind _tmp359;enum Cyc_Absyn_KindQual _tmp35A;struct Cyc_Absyn_Kind
_tmp35B;enum Cyc_Absyn_KindQual _tmp35C;_LL1E1: _tmp357=*_tmp356;_tmp358=_tmp357.kind;
if(_tmp358 != Cyc_Absyn_RgnKind)goto _LL1E3;_LL1E2: {struct Cyc_Absyn_AccessEff_struct
_tmp1103;struct Cyc_Absyn_AccessEff_struct*_tmp1102;return(void*)((_tmp1102=
_cycalloc(sizeof(*_tmp1102)),((_tmp1102[0]=((_tmp1103.tag=23,((_tmp1103.f1=(void*)
t,_tmp1103)))),_tmp1102))));}_LL1E3: _tmp359=*_tmp356;_tmp35A=_tmp359.kind;if(
_tmp35A != Cyc_Absyn_EffKind)goto _LL1E5;_LL1E4: return t;_LL1E5: _tmp35B=*_tmp356;
_tmp35C=_tmp35B.kind;if(_tmp35C != Cyc_Absyn_IntKind)goto _LL1E7;_LL1E6: return Cyc_Absyn_empty_effect;
_LL1E7:;_LL1E8: {struct Cyc_Absyn_RgnsEff_struct _tmp1106;struct Cyc_Absyn_RgnsEff_struct*
_tmp1105;return(void*)((_tmp1105=_cycalloc(sizeof(*_tmp1105)),((_tmp1105[0]=((
_tmp1106.tag=25,((_tmp1106.f1=(void*)t,_tmp1106)))),_tmp1105))));}_LL1E0:;}
_LL1BE: {struct Cyc_Absyn_RgnHandleType_struct*_tmp32B=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp321;if(_tmp32B->tag != 16)goto _LL1C0;else{_tmp32C=(void*)_tmp32B->f1;}}_LL1BF: {
struct Cyc_Absyn_AccessEff_struct _tmp1109;struct Cyc_Absyn_AccessEff_struct*
_tmp1108;return(void*)((_tmp1108=_cycalloc(sizeof(*_tmp1108)),((_tmp1108[0]=((
_tmp1109.tag=23,((_tmp1109.f1=(void*)_tmp32C,_tmp1109)))),_tmp1108))));}_LL1C0: {
struct Cyc_Absyn_DynRgnType_struct*_tmp32D=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp321;if(_tmp32D->tag != 17)goto _LL1C2;else{_tmp32E=(void*)_tmp32D->f1;_tmp32F=(
void*)_tmp32D->f2;}}_LL1C1: {struct Cyc_Absyn_AccessEff_struct _tmp110C;struct Cyc_Absyn_AccessEff_struct*
_tmp110B;return(void*)((_tmp110B=_cycalloc(sizeof(*_tmp110B)),((_tmp110B[0]=((
_tmp110C.tag=23,((_tmp110C.f1=(void*)_tmp32F,_tmp110C)))),_tmp110B))));}_LL1C2: {
struct Cyc_Absyn_DatatypeType_struct*_tmp330=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp321;if(_tmp330->tag != 3)goto _LL1C4;else{_tmp331=_tmp330->f1;_tmp332=_tmp331.targs;}}
_LL1C3: {struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp332);struct Cyc_Absyn_JoinEff_struct
_tmp110F;struct Cyc_Absyn_JoinEff_struct*_tmp110E;return Cyc_Tcutil_normalize_effect((
void*)((_tmp110E=_cycalloc(sizeof(*_tmp110E)),((_tmp110E[0]=((_tmp110F.tag=24,((
_tmp110F.f1=ts,_tmp110F)))),_tmp110E)))));}_LL1C4: {struct Cyc_Absyn_PointerType_struct*
_tmp333=(struct Cyc_Absyn_PointerType_struct*)_tmp321;if(_tmp333->tag != 5)goto
_LL1C6;else{_tmp334=_tmp333->f1;_tmp335=_tmp334.elt_typ;_tmp336=_tmp334.ptr_atts;
_tmp337=_tmp336.rgn;}}_LL1C5: {struct Cyc_Absyn_JoinEff_struct _tmp111E;struct Cyc_Absyn_AccessEff_struct*
_tmp111D;struct Cyc_Absyn_AccessEff_struct _tmp111C;void*_tmp111B[2];struct Cyc_Absyn_JoinEff_struct*
_tmp111A;return Cyc_Tcutil_normalize_effect((void*)((_tmp111A=_cycalloc(sizeof(*
_tmp111A)),((_tmp111A[0]=((_tmp111E.tag=24,((_tmp111E.f1=((_tmp111B[1]=Cyc_Tcutil_rgns_of(
_tmp335),((_tmp111B[0]=(void*)((_tmp111D=_cycalloc(sizeof(*_tmp111D)),((_tmp111D[
0]=((_tmp111C.tag=23,((_tmp111C.f1=(void*)_tmp337,_tmp111C)))),_tmp111D)))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp111B,sizeof(void*),2)))))),_tmp111E)))),_tmp111A)))));}_LL1C6: {struct Cyc_Absyn_ArrayType_struct*
_tmp338=(struct Cyc_Absyn_ArrayType_struct*)_tmp321;if(_tmp338->tag != 9)goto
_LL1C8;else{_tmp339=_tmp338->f1;_tmp33A=_tmp339.elt_type;}}_LL1C7: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp33A));_LL1C8: {struct Cyc_Absyn_TupleType_struct*_tmp33B=(
struct Cyc_Absyn_TupleType_struct*)_tmp321;if(_tmp33B->tag != 11)goto _LL1CA;else{
_tmp33C=_tmp33B->f1;}}_LL1C9: {struct Cyc_List_List*_tmp36C=0;for(0;_tmp33C != 0;
_tmp33C=_tmp33C->tl){struct Cyc_List_List*_tmp111F;_tmp36C=((_tmp111F=_cycalloc(
sizeof(*_tmp111F)),((_tmp111F->hd=(*((struct _tuple11*)_tmp33C->hd)).f2,((
_tmp111F->tl=_tmp36C,_tmp111F))))));}_tmp33F=_tmp36C;goto _LL1CB;}_LL1CA: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp33D=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp321;if(_tmp33D->tag != 4)goto _LL1CC;else{_tmp33E=_tmp33D->f1;_tmp33F=_tmp33E.targs;}}
_LL1CB: _tmp342=_tmp33F;goto _LL1CD;_LL1CC: {struct Cyc_Absyn_AggrType_struct*
_tmp340=(struct Cyc_Absyn_AggrType_struct*)_tmp321;if(_tmp340->tag != 12)goto
_LL1CE;else{_tmp341=_tmp340->f1;_tmp342=_tmp341.targs;}}_LL1CD: {struct Cyc_Absyn_JoinEff_struct
_tmp1122;struct Cyc_Absyn_JoinEff_struct*_tmp1121;return Cyc_Tcutil_normalize_effect((
void*)((_tmp1121=_cycalloc(sizeof(*_tmp1121)),((_tmp1121[0]=((_tmp1122.tag=24,((
_tmp1122.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp342),_tmp1122)))),_tmp1121)))));}_LL1CE: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp343=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp321;if(_tmp343->tag != 13)goto _LL1D0;else{_tmp344=_tmp343->f2;}}_LL1CF: {
struct Cyc_Absyn_JoinEff_struct _tmp1125;struct Cyc_Absyn_JoinEff_struct*_tmp1124;
return Cyc_Tcutil_normalize_effect((void*)((_tmp1124=_cycalloc(sizeof(*_tmp1124)),((
_tmp1124[0]=((_tmp1125.tag=24,((_tmp1125.f1=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,
_tmp344),_tmp1125)))),_tmp1124)))));}_LL1D0: {struct Cyc_Absyn_TagType_struct*
_tmp345=(struct Cyc_Absyn_TagType_struct*)_tmp321;if(_tmp345->tag != 20)goto _LL1D2;}
_LL1D1: return Cyc_Absyn_empty_effect;_LL1D2: {struct Cyc_Absyn_FnType_struct*
_tmp346=(struct Cyc_Absyn_FnType_struct*)_tmp321;if(_tmp346->tag != 10)goto _LL1D4;
else{_tmp347=_tmp346->f1;_tmp348=_tmp347.tvars;_tmp349=_tmp347.effect;_tmp34A=
_tmp347.ret_typ;_tmp34B=_tmp347.args;_tmp34C=_tmp347.cyc_varargs;_tmp34D=_tmp347.rgn_po;}}
_LL1D3: {void*_tmp372=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct
_tuple15*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp348),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp349))->v);return Cyc_Tcutil_normalize_effect(
_tmp372);}_LL1D4: {struct Cyc_Absyn_UniqueRgn_struct*_tmp34E=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp321;if(_tmp34E->tag != 22)goto _LL1D6;}_LL1D5: goto _LL1D7;_LL1D6: {struct Cyc_Absyn_HeapRgn_struct*
_tmp34F=(struct Cyc_Absyn_HeapRgn_struct*)_tmp321;if(_tmp34F->tag != 21)goto _LL1D8;}
_LL1D7: return Cyc_Absyn_empty_effect;_LL1D8: {struct Cyc_Absyn_AccessEff_struct*
_tmp350=(struct Cyc_Absyn_AccessEff_struct*)_tmp321;if(_tmp350->tag != 23)goto
_LL1DA;}_LL1D9: goto _LL1DB;_LL1DA: {struct Cyc_Absyn_JoinEff_struct*_tmp351=(
struct Cyc_Absyn_JoinEff_struct*)_tmp321;if(_tmp351->tag != 24)goto _LL1DC;}_LL1DB:
return t;_LL1DC: {struct Cyc_Absyn_RgnsEff_struct*_tmp352=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp321;if(_tmp352->tag != 25)goto _LL1DE;else{_tmp353=(void*)_tmp352->f1;}}_LL1DD:
return Cyc_Tcutil_rgns_of(_tmp353);_LL1DE: {struct Cyc_Absyn_TypedefType_struct*
_tmp354=(struct Cyc_Absyn_TypedefType_struct*)_tmp321;if(_tmp354->tag != 18)goto
_LL1AB;else{_tmp355=_tmp354->f2;}}_LL1DF: {struct Cyc_Absyn_JoinEff_struct
_tmp1128;struct Cyc_Absyn_JoinEff_struct*_tmp1127;return Cyc_Tcutil_normalize_effect((
void*)((_tmp1127=_cycalloc(sizeof(*_tmp1127)),((_tmp1127[0]=((_tmp1128.tag=24,((
_tmp1128.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp355),_tmp1128)))),_tmp1127)))));}_LL1AB:;}
void*Cyc_Tcutil_normalize_effect(void*e);void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{void*_tmp375=e;struct Cyc_List_List*_tmp377;struct Cyc_List_List**
_tmp378;void*_tmp37A;_LL1EA: {struct Cyc_Absyn_JoinEff_struct*_tmp376=(struct Cyc_Absyn_JoinEff_struct*)
_tmp375;if(_tmp376->tag != 24)goto _LL1EC;else{_tmp377=_tmp376->f1;_tmp378=(struct
Cyc_List_List**)& _tmp376->f1;}}_LL1EB: {int redo_join=0;{struct Cyc_List_List*effs=*
_tmp378;for(0;effs != 0;effs=effs->tl){void*_tmp37B=(void*)effs->hd;effs->hd=(
void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp37B));{void*_tmp37C=(
void*)effs->hd;void*_tmp37F;_LL1F1: {struct Cyc_Absyn_JoinEff_struct*_tmp37D=(
struct Cyc_Absyn_JoinEff_struct*)_tmp37C;if(_tmp37D->tag != 24)goto _LL1F3;}_LL1F2:
goto _LL1F4;_LL1F3: {struct Cyc_Absyn_AccessEff_struct*_tmp37E=(struct Cyc_Absyn_AccessEff_struct*)
_tmp37C;if(_tmp37E->tag != 23)goto _LL1F5;else{_tmp37F=(void*)_tmp37E->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp380=(struct Cyc_Absyn_HeapRgn_struct*)_tmp37F;if(
_tmp380->tag != 21)goto _LL1F5;};}}_LL1F4: redo_join=1;goto _LL1F0;_LL1F5:;_LL1F6:
goto _LL1F0;_LL1F0:;};}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{
struct Cyc_List_List*effs=*_tmp378;for(0;effs != 0;effs=effs->tl){void*_tmp381=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp383;void*_tmp385;_LL1F8: {struct Cyc_Absyn_JoinEff_struct*
_tmp382=(struct Cyc_Absyn_JoinEff_struct*)_tmp381;if(_tmp382->tag != 24)goto _LL1FA;
else{_tmp383=_tmp382->f1;}}_LL1F9: effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp383,effects);goto _LL1F7;_LL1FA: {
struct Cyc_Absyn_AccessEff_struct*_tmp384=(struct Cyc_Absyn_AccessEff_struct*)
_tmp381;if(_tmp384->tag != 23)goto _LL1FC;else{_tmp385=(void*)_tmp384->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp386=(struct Cyc_Absyn_HeapRgn_struct*)_tmp385;if(
_tmp386->tag != 21)goto _LL1FC;};}}_LL1FB: goto _LL1F7;_LL1FC:;_LL1FD:{struct Cyc_List_List*
_tmp1129;effects=((_tmp1129=_cycalloc(sizeof(*_tmp1129)),((_tmp1129->hd=_tmp381,((
_tmp1129->tl=effects,_tmp1129))))));}goto _LL1F7;_LL1F7:;}}*_tmp378=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL1EC: {struct Cyc_Absyn_RgnsEff_struct*
_tmp379=(struct Cyc_Absyn_RgnsEff_struct*)_tmp375;if(_tmp379->tag != 25)goto _LL1EE;
else{_tmp37A=(void*)_tmp379->f1;}}_LL1ED: {void*_tmp388=Cyc_Tcutil_compress(
_tmp37A);_LL1FF: {struct Cyc_Absyn_Evar_struct*_tmp389=(struct Cyc_Absyn_Evar_struct*)
_tmp388;if(_tmp389->tag != 1)goto _LL201;}_LL200: goto _LL202;_LL201: {struct Cyc_Absyn_VarType_struct*
_tmp38A=(struct Cyc_Absyn_VarType_struct*)_tmp388;if(_tmp38A->tag != 2)goto _LL203;}
_LL202: return e;_LL203:;_LL204: return Cyc_Tcutil_rgns_of(_tmp37A);_LL1FE:;}_LL1EE:;
_LL1EF: return e;_LL1E9:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff);static
void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct _tmp1133;
struct Cyc_Core_Opt*_tmp1132;struct Cyc_Absyn_FnInfo _tmp1131;struct Cyc_Absyn_FnType_struct*
_tmp1130;struct Cyc_Absyn_FnType_struct*_tmp38B=(_tmp1130=_cycalloc(sizeof(*
_tmp1130)),((_tmp1130[0]=((_tmp1133.tag=10,((_tmp1133.f1=((_tmp1131.tvars=0,((
_tmp1131.effect=((_tmp1132=_cycalloc(sizeof(*_tmp1132)),((_tmp1132->v=eff,
_tmp1132)))),((_tmp1131.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp1131.args=0,((
_tmp1131.c_varargs=0,((_tmp1131.cyc_varargs=0,((_tmp1131.rgn_po=0,((_tmp1131.attributes=
0,_tmp1131)))))))))))))))),_tmp1133)))),_tmp1130)));return Cyc_Absyn_atb_typ((
void*)_tmp38B,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,
Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*
e);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val)
return 1;{void*_tmp390=Cyc_Tcutil_compress(e);void*_tmp392;struct Cyc_List_List*
_tmp394;void*_tmp396;struct Cyc_Core_Opt*_tmp398;struct Cyc_Core_Opt*_tmp399;
struct Cyc_Core_Opt**_tmp39A;struct Cyc_Core_Opt*_tmp39B;_LL206: {struct Cyc_Absyn_AccessEff_struct*
_tmp391=(struct Cyc_Absyn_AccessEff_struct*)_tmp390;if(_tmp391->tag != 23)goto
_LL208;else{_tmp392=(void*)_tmp391->f1;}}_LL207: if(constrain)return Cyc_Tcutil_unify(
r,_tmp392);_tmp392=Cyc_Tcutil_compress(_tmp392);if(r == _tmp392)return 1;{struct
_tuple0 _tmp1134;struct _tuple0 _tmp39D=(_tmp1134.f1=r,((_tmp1134.f2=_tmp392,
_tmp1134)));void*_tmp39E;struct Cyc_Absyn_Tvar*_tmp3A0;void*_tmp3A1;struct Cyc_Absyn_Tvar*
_tmp3A3;_LL211: _tmp39E=_tmp39D.f1;{struct Cyc_Absyn_VarType_struct*_tmp39F=(
struct Cyc_Absyn_VarType_struct*)_tmp39E;if(_tmp39F->tag != 2)goto _LL213;else{
_tmp3A0=_tmp39F->f1;}};_tmp3A1=_tmp39D.f2;{struct Cyc_Absyn_VarType_struct*
_tmp3A2=(struct Cyc_Absyn_VarType_struct*)_tmp3A1;if(_tmp3A2->tag != 2)goto _LL213;
else{_tmp3A3=_tmp3A2->f1;}};_LL212: return Cyc_Absyn_tvar_cmp(_tmp3A0,_tmp3A3)== 0;
_LL213:;_LL214: return 0;_LL210:;};_LL208: {struct Cyc_Absyn_JoinEff_struct*_tmp393=(
struct Cyc_Absyn_JoinEff_struct*)_tmp390;if(_tmp393->tag != 24)goto _LL20A;else{
_tmp394=_tmp393->f1;}}_LL209: for(0;_tmp394 != 0;_tmp394=_tmp394->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp394->hd))return 1;}return 0;_LL20A: {struct Cyc_Absyn_RgnsEff_struct*
_tmp395=(struct Cyc_Absyn_RgnsEff_struct*)_tmp390;if(_tmp395->tag != 25)goto _LL20C;
else{_tmp396=(void*)_tmp395->f1;}}_LL20B: {void*_tmp3A4=Cyc_Tcutil_rgns_of(
_tmp396);void*_tmp3A6;_LL216: {struct Cyc_Absyn_RgnsEff_struct*_tmp3A5=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3A4;if(_tmp3A5->tag != 25)goto _LL218;else{_tmp3A6=(void*)_tmp3A5->f1;}}_LL217:
if(!constrain)return 0;{void*_tmp3A7=Cyc_Tcutil_compress(_tmp3A6);struct Cyc_Core_Opt*
_tmp3A9;struct Cyc_Core_Opt*_tmp3AA;struct Cyc_Core_Opt**_tmp3AB;struct Cyc_Core_Opt*
_tmp3AC;_LL21B: {struct Cyc_Absyn_Evar_struct*_tmp3A8=(struct Cyc_Absyn_Evar_struct*)
_tmp3A7;if(_tmp3A8->tag != 1)goto _LL21D;else{_tmp3A9=_tmp3A8->f1;_tmp3AA=_tmp3A8->f2;
_tmp3AB=(struct Cyc_Core_Opt**)& _tmp3A8->f2;_tmp3AC=_tmp3A8->f4;}}_LL21C: {void*
_tmp3AD=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3AC);Cyc_Tcutil_occurs(
_tmp3AD,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3AC))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp1143;struct Cyc_Absyn_AccessEff_struct*
_tmp1142;struct Cyc_Absyn_AccessEff_struct _tmp1141;void*_tmp1140[2];struct Cyc_Absyn_JoinEff_struct*
_tmp113F;void*_tmp3AE=Cyc_Tcutil_dummy_fntype((void*)((_tmp113F=_cycalloc(
sizeof(*_tmp113F)),((_tmp113F[0]=((_tmp1143.tag=24,((_tmp1143.f1=((_tmp1140[1]=(
void*)((_tmp1142=_cycalloc(sizeof(*_tmp1142)),((_tmp1142[0]=((_tmp1141.tag=23,((
_tmp1141.f1=(void*)r,_tmp1141)))),_tmp1142)))),((_tmp1140[0]=_tmp3AD,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1140,sizeof(void*),2)))))),
_tmp1143)))),_tmp113F)))));{struct Cyc_Core_Opt*_tmp1144;*_tmp3AB=((_tmp1144=
_cycalloc(sizeof(*_tmp1144)),((_tmp1144->v=_tmp3AE,_tmp1144))));}return 1;};}
_LL21D:;_LL21E: return 0;_LL21A:;};_LL218:;_LL219: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp3A4);_LL215:;}_LL20C: {struct Cyc_Absyn_Evar_struct*_tmp397=(
struct Cyc_Absyn_Evar_struct*)_tmp390;if(_tmp397->tag != 1)goto _LL20E;else{_tmp398=
_tmp397->f1;_tmp399=_tmp397->f2;_tmp39A=(struct Cyc_Core_Opt**)& _tmp397->f2;
_tmp39B=_tmp397->f4;}}_LL20D: if(_tmp398 == 0  || ((struct Cyc_Absyn_Kind*)_tmp398->v)->kind
!= Cyc_Absyn_EffKind){const char*_tmp1147;void*_tmp1146;(_tmp1146=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1147="effect evar has wrong kind",
_tag_dyneither(_tmp1147,sizeof(char),27))),_tag_dyneither(_tmp1146,sizeof(void*),
0)));}if(!constrain)return 0;{void*_tmp3B7=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp39B);Cyc_Tcutil_occurs(_tmp3B7,Cyc_Core_heap_region,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp39B))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmp115C;struct Cyc_List_List*_tmp115B;struct Cyc_Absyn_AccessEff_struct _tmp115A;
struct Cyc_Absyn_AccessEff_struct*_tmp1159;struct Cyc_List_List*_tmp1158;struct Cyc_Absyn_JoinEff_struct*
_tmp1157;struct Cyc_Absyn_JoinEff_struct*_tmp3B8=(_tmp1157=_cycalloc(sizeof(*
_tmp1157)),((_tmp1157[0]=((_tmp115C.tag=24,((_tmp115C.f1=((_tmp1158=_cycalloc(
sizeof(*_tmp1158)),((_tmp1158->hd=_tmp3B7,((_tmp1158->tl=((_tmp115B=_cycalloc(
sizeof(*_tmp115B)),((_tmp115B->hd=(void*)((_tmp1159=_cycalloc(sizeof(*_tmp1159)),((
_tmp1159[0]=((_tmp115A.tag=23,((_tmp115A.f1=(void*)r,_tmp115A)))),_tmp1159)))),((
_tmp115B->tl=0,_tmp115B)))))),_tmp1158)))))),_tmp115C)))),_tmp1157)));{struct Cyc_Core_Opt*
_tmp115D;*_tmp39A=((_tmp115D=_cycalloc(sizeof(*_tmp115D)),((_tmp115D->v=(void*)
_tmp3B8,_tmp115D))));}return 1;};};_LL20E:;_LL20F: return 0;_LL205:;};}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e);static int Cyc_Tcutil_type_in_effect(int
may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp3C0=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp3C3;void*_tmp3C5;struct Cyc_Core_Opt*
_tmp3C7;struct Cyc_Core_Opt*_tmp3C8;struct Cyc_Core_Opt**_tmp3C9;struct Cyc_Core_Opt*
_tmp3CA;_LL220: {struct Cyc_Absyn_AccessEff_struct*_tmp3C1=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3C0;if(_tmp3C1->tag != 23)goto _LL222;}_LL221: return 0;_LL222: {struct Cyc_Absyn_JoinEff_struct*
_tmp3C2=(struct Cyc_Absyn_JoinEff_struct*)_tmp3C0;if(_tmp3C2->tag != 24)goto _LL224;
else{_tmp3C3=_tmp3C2->f1;}}_LL223: for(0;_tmp3C3 != 0;_tmp3C3=_tmp3C3->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp3C3->hd))return 1;}return 0;_LL224: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3C4=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3C0;if(_tmp3C4->tag != 25)goto _LL226;
else{_tmp3C5=(void*)_tmp3C4->f1;}}_LL225: _tmp3C5=Cyc_Tcutil_compress(_tmp3C5);
if(t == _tmp3C5)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp3C5);{
void*_tmp3CB=Cyc_Tcutil_rgns_of(t);void*_tmp3CD;_LL22B: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3CC=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3CB;if(_tmp3CC->tag != 25)goto _LL22D;
else{_tmp3CD=(void*)_tmp3CC->f1;}}_LL22C: {struct _tuple0 _tmp115E;struct _tuple0
_tmp3CF=(_tmp115E.f1=Cyc_Tcutil_compress(_tmp3CD),((_tmp115E.f2=_tmp3C5,_tmp115E)));
void*_tmp3D0;struct Cyc_Absyn_Tvar*_tmp3D2;void*_tmp3D3;struct Cyc_Absyn_Tvar*
_tmp3D5;_LL230: _tmp3D0=_tmp3CF.f1;{struct Cyc_Absyn_VarType_struct*_tmp3D1=(
struct Cyc_Absyn_VarType_struct*)_tmp3D0;if(_tmp3D1->tag != 2)goto _LL232;else{
_tmp3D2=_tmp3D1->f1;}};_tmp3D3=_tmp3CF.f2;{struct Cyc_Absyn_VarType_struct*
_tmp3D4=(struct Cyc_Absyn_VarType_struct*)_tmp3D3;if(_tmp3D4->tag != 2)goto _LL232;
else{_tmp3D5=_tmp3D4->f1;}};_LL231: return Cyc_Tcutil_unify(t,_tmp3C5);_LL232:;
_LL233: return _tmp3CD == _tmp3C5;_LL22F:;}_LL22D:;_LL22E: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp3CB);_LL22A:;};_LL226: {struct Cyc_Absyn_Evar_struct*
_tmp3C6=(struct Cyc_Absyn_Evar_struct*)_tmp3C0;if(_tmp3C6->tag != 1)goto _LL228;
else{_tmp3C7=_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;_tmp3C9=(struct Cyc_Core_Opt**)&
_tmp3C6->f2;_tmp3CA=_tmp3C6->f4;}}_LL227: if(_tmp3C7 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp3C7->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1161;void*_tmp1160;(
_tmp1160=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1161="effect evar has wrong kind",_tag_dyneither(_tmp1161,sizeof(char),27))),
_tag_dyneither(_tmp1160,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp3D8=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3CA);
Cyc_Tcutil_occurs(_tmp3D8,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3CA))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp1176;struct Cyc_List_List*
_tmp1175;struct Cyc_Absyn_RgnsEff_struct _tmp1174;struct Cyc_Absyn_RgnsEff_struct*
_tmp1173;struct Cyc_List_List*_tmp1172;struct Cyc_Absyn_JoinEff_struct*_tmp1171;
struct Cyc_Absyn_JoinEff_struct*_tmp3D9=(_tmp1171=_cycalloc(sizeof(*_tmp1171)),((
_tmp1171[0]=((_tmp1176.tag=24,((_tmp1176.f1=((_tmp1172=_cycalloc(sizeof(*
_tmp1172)),((_tmp1172->hd=_tmp3D8,((_tmp1172->tl=((_tmp1175=_cycalloc(sizeof(*
_tmp1175)),((_tmp1175->hd=(void*)((_tmp1173=_cycalloc(sizeof(*_tmp1173)),((
_tmp1173[0]=((_tmp1174.tag=25,((_tmp1174.f1=(void*)t,_tmp1174)))),_tmp1173)))),((
_tmp1175->tl=0,_tmp1175)))))),_tmp1172)))))),_tmp1176)))),_tmp1171)));{struct Cyc_Core_Opt*
_tmp1177;*_tmp3C9=((_tmp1177=_cycalloc(sizeof(*_tmp1177)),((_tmp1177->v=(void*)
_tmp3D9,_tmp1177))));}return 1;};};_LL228:;_LL229: return 0;_LL21F:;};}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e);static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp3E1=e;struct Cyc_Absyn_Tvar*_tmp3E3;struct Cyc_List_List*_tmp3E5;void*
_tmp3E7;struct Cyc_Core_Opt*_tmp3E9;struct Cyc_Core_Opt*_tmp3EA;struct Cyc_Core_Opt**
_tmp3EB;struct Cyc_Core_Opt*_tmp3EC;_LL235: {struct Cyc_Absyn_VarType_struct*
_tmp3E2=(struct Cyc_Absyn_VarType_struct*)_tmp3E1;if(_tmp3E2->tag != 2)goto _LL237;
else{_tmp3E3=_tmp3E2->f1;}}_LL236: return Cyc_Absyn_tvar_cmp(v,_tmp3E3)== 0;_LL237: {
struct Cyc_Absyn_JoinEff_struct*_tmp3E4=(struct Cyc_Absyn_JoinEff_struct*)_tmp3E1;
if(_tmp3E4->tag != 24)goto _LL239;else{_tmp3E5=_tmp3E4->f1;}}_LL238: for(0;_tmp3E5
!= 0;_tmp3E5=_tmp3E5->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp3E5->hd))return 1;}return 0;_LL239: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3E6=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3E1;if(_tmp3E6->tag != 25)goto _LL23B;
else{_tmp3E7=(void*)_tmp3E6->f1;}}_LL23A: {void*_tmp3ED=Cyc_Tcutil_rgns_of(
_tmp3E7);void*_tmp3EF;_LL240: {struct Cyc_Absyn_RgnsEff_struct*_tmp3EE=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3ED;if(_tmp3EE->tag != 25)goto _LL242;else{_tmp3EF=(void*)_tmp3EE->f1;}}_LL241:
if(!may_constrain_evars)return 0;{void*_tmp3F0=Cyc_Tcutil_compress(_tmp3EF);
struct Cyc_Core_Opt*_tmp3F2;struct Cyc_Core_Opt*_tmp3F3;struct Cyc_Core_Opt**
_tmp3F4;struct Cyc_Core_Opt*_tmp3F5;_LL245: {struct Cyc_Absyn_Evar_struct*_tmp3F1=(
struct Cyc_Absyn_Evar_struct*)_tmp3F0;if(_tmp3F1->tag != 1)goto _LL247;else{_tmp3F2=
_tmp3F1->f1;_tmp3F3=_tmp3F1->f2;_tmp3F4=(struct Cyc_Core_Opt**)& _tmp3F1->f2;
_tmp3F5=_tmp3F1->f4;}}_LL246: {void*_tmp3F6=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp3F5);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3F5))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmp1186;struct Cyc_Absyn_VarType_struct*_tmp1185;
struct Cyc_Absyn_VarType_struct _tmp1184;void*_tmp1183[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1182;void*_tmp3F7=Cyc_Tcutil_dummy_fntype((void*)((_tmp1182=_cycalloc(
sizeof(*_tmp1182)),((_tmp1182[0]=((_tmp1186.tag=24,((_tmp1186.f1=((_tmp1183[1]=(
void*)((_tmp1185=_cycalloc(sizeof(*_tmp1185)),((_tmp1185[0]=((_tmp1184.tag=2,((
_tmp1184.f1=v,_tmp1184)))),_tmp1185)))),((_tmp1183[0]=_tmp3F6,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1183,sizeof(void*),2)))))),
_tmp1186)))),_tmp1182)))));{struct Cyc_Core_Opt*_tmp1187;*_tmp3F4=((_tmp1187=
_cycalloc(sizeof(*_tmp1187)),((_tmp1187->v=_tmp3F7,_tmp1187))));}return 1;};}
_LL247:;_LL248: return 0;_LL244:;};_LL242:;_LL243: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp3ED);_LL23F:;}_LL23B: {struct Cyc_Absyn_Evar_struct*
_tmp3E8=(struct Cyc_Absyn_Evar_struct*)_tmp3E1;if(_tmp3E8->tag != 1)goto _LL23D;
else{_tmp3E9=_tmp3E8->f1;_tmp3EA=_tmp3E8->f2;_tmp3EB=(struct Cyc_Core_Opt**)&
_tmp3E8->f2;_tmp3EC=_tmp3E8->f4;}}_LL23C: if(_tmp3E9 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp3E9->v)->kind != Cyc_Absyn_EffKind){const char*_tmp118A;void*_tmp1189;(
_tmp1189=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp118A="effect evar has wrong kind",_tag_dyneither(_tmp118A,sizeof(char),27))),
_tag_dyneither(_tmp1189,sizeof(void*),0)));}{void*_tmp400=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3EC);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3EC))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmp119F;
struct Cyc_List_List*_tmp119E;struct Cyc_Absyn_VarType_struct _tmp119D;struct Cyc_Absyn_VarType_struct*
_tmp119C;struct Cyc_List_List*_tmp119B;struct Cyc_Absyn_JoinEff_struct*_tmp119A;
struct Cyc_Absyn_JoinEff_struct*_tmp401=(_tmp119A=_cycalloc(sizeof(*_tmp119A)),((
_tmp119A[0]=((_tmp119F.tag=24,((_tmp119F.f1=((_tmp119B=_cycalloc(sizeof(*
_tmp119B)),((_tmp119B->hd=_tmp400,((_tmp119B->tl=((_tmp119E=_cycalloc(sizeof(*
_tmp119E)),((_tmp119E->hd=(void*)((_tmp119C=_cycalloc(sizeof(*_tmp119C)),((
_tmp119C[0]=((_tmp119D.tag=2,((_tmp119D.f1=v,_tmp119D)))),_tmp119C)))),((
_tmp119E->tl=0,_tmp119E)))))),_tmp119B)))))),_tmp119F)))),_tmp119A)));{struct Cyc_Core_Opt*
_tmp11A0;*_tmp3EB=((_tmp11A0=_cycalloc(sizeof(*_tmp11A0)),((_tmp11A0->v=(void*)
_tmp401,_tmp11A0))));}return 1;};};_LL23D:;_LL23E: return 0;_LL234:;};}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e);static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp409=e;struct Cyc_List_List*_tmp40B;void*_tmp40D;_LL24A: {struct Cyc_Absyn_JoinEff_struct*
_tmp40A=(struct Cyc_Absyn_JoinEff_struct*)_tmp409;if(_tmp40A->tag != 24)goto _LL24C;
else{_tmp40B=_tmp40A->f1;}}_LL24B: for(0;_tmp40B != 0;_tmp40B=_tmp40B->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp40B->hd))return 1;}return 0;_LL24C: {struct Cyc_Absyn_RgnsEff_struct*
_tmp40C=(struct Cyc_Absyn_RgnsEff_struct*)_tmp409;if(_tmp40C->tag != 25)goto _LL24E;
else{_tmp40D=(void*)_tmp40C->f1;}}_LL24D: {void*_tmp40F=Cyc_Tcutil_rgns_of(
_tmp40D);void*_tmp411;_LL253: {struct Cyc_Absyn_RgnsEff_struct*_tmp410=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp40F;if(_tmp410->tag != 25)goto _LL255;else{_tmp411=(void*)_tmp410->f1;}}_LL254:
return 0;_LL255:;_LL256: return Cyc_Tcutil_evar_in_effect(evar,_tmp40F);_LL252:;}
_LL24E: {struct Cyc_Absyn_Evar_struct*_tmp40E=(struct Cyc_Absyn_Evar_struct*)
_tmp409;if(_tmp40E->tag != 1)goto _LL250;}_LL24F: return evar == e;_LL250:;_LL251:
return 0;_LL249:;};}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,
void*e2);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp412=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp414;void*_tmp416;
struct Cyc_Absyn_Tvar*_tmp418;void*_tmp41A;struct Cyc_Core_Opt*_tmp41C;struct Cyc_Core_Opt**
_tmp41D;struct Cyc_Core_Opt*_tmp41E;_LL258: {struct Cyc_Absyn_JoinEff_struct*
_tmp413=(struct Cyc_Absyn_JoinEff_struct*)_tmp412;if(_tmp413->tag != 24)goto _LL25A;
else{_tmp414=_tmp413->f1;}}_LL259: for(0;_tmp414 != 0;_tmp414=_tmp414->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp414->hd,e2))return 0;}return 1;_LL25A: {struct Cyc_Absyn_AccessEff_struct*
_tmp415=(struct Cyc_Absyn_AccessEff_struct*)_tmp412;if(_tmp415->tag != 23)goto
_LL25C;else{_tmp416=(void*)_tmp415->f1;}}_LL25B: return Cyc_Tcutil_region_in_effect(
0,_tmp416,e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp416,(void*)& Cyc_Absyn_HeapRgn_val);
_LL25C: {struct Cyc_Absyn_VarType_struct*_tmp417=(struct Cyc_Absyn_VarType_struct*)
_tmp412;if(_tmp417->tag != 2)goto _LL25E;else{_tmp418=_tmp417->f1;}}_LL25D: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp418,e2);_LL25E: {struct Cyc_Absyn_RgnsEff_struct*
_tmp419=(struct Cyc_Absyn_RgnsEff_struct*)_tmp412;if(_tmp419->tag != 25)goto _LL260;
else{_tmp41A=(void*)_tmp419->f1;}}_LL25F: {void*_tmp41F=Cyc_Tcutil_rgns_of(
_tmp41A);void*_tmp421;_LL265: {struct Cyc_Absyn_RgnsEff_struct*_tmp420=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp41F;if(_tmp420->tag != 25)goto _LL267;else{_tmp421=(void*)_tmp420->f1;}}_LL266:
return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp421,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp421,Cyc_Absyn_sint_typ);_LL267:;
_LL268: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp41F,e2);_LL264:;}
_LL260: {struct Cyc_Absyn_Evar_struct*_tmp41B=(struct Cyc_Absyn_Evar_struct*)
_tmp412;if(_tmp41B->tag != 1)goto _LL262;else{_tmp41C=_tmp41B->f2;_tmp41D=(struct
Cyc_Core_Opt**)& _tmp41B->f2;_tmp41E=_tmp41B->f4;}}_LL261: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){struct Cyc_Core_Opt*_tmp11A1;*_tmp41D=((_tmp11A1=_cycalloc(sizeof(*
_tmp11A1)),((_tmp11A1->v=Cyc_Absyn_empty_effect,_tmp11A1))));}return 1;_LL262:;
_LL263: {const char*_tmp11A5;void*_tmp11A4[1];struct Cyc_String_pa_struct _tmp11A3;(
_tmp11A3.tag=0,((_tmp11A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmp11A4[0]=& _tmp11A3,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A5="subset_effect: bad effect: %s",
_tag_dyneither(_tmp11A5,sizeof(char),30))),_tag_dyneither(_tmp11A4,sizeof(void*),
1)))))));}_LL257:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2);static int
Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=
Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(
0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(
1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2);static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0 _tmp427;void*_tmp428;void*_tmp429;struct _tuple0*_tmp426=(struct
_tuple0*)r1->hd;_tmp427=*_tmp426;_tmp428=_tmp427.f1;_tmp429=_tmp427.f2;{int found=
_tmp428 == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 
0  && !found;r2=r2->tl){struct _tuple0 _tmp42B;void*_tmp42C;void*_tmp42D;struct
_tuple0*_tmp42A=(struct _tuple0*)r2->hd;_tmp42B=*_tmp42A;_tmp42C=_tmp42B.f1;
_tmp42D=_tmp42B.f2;if(Cyc_Tcutil_unify(_tmp428,_tmp42C) && Cyc_Tcutil_unify(
_tmp429,_tmp42D)){found=1;break;}}}if(!found)return 0;};}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2);static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple21{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple22{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2);void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp42E=t1;struct Cyc_Core_Opt*_tmp430;struct Cyc_Core_Opt*
_tmp431;struct Cyc_Core_Opt**_tmp432;struct Cyc_Core_Opt*_tmp433;_LL26A: {struct
Cyc_Absyn_Evar_struct*_tmp42F=(struct Cyc_Absyn_Evar_struct*)_tmp42E;if(_tmp42F->tag
!= 1)goto _LL26C;else{_tmp430=_tmp42F->f1;_tmp431=_tmp42F->f2;_tmp432=(struct Cyc_Core_Opt**)&
_tmp42F->f2;_tmp433=_tmp42F->f4;}}_LL26B: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp433))->v,t2);{struct
Cyc_Absyn_Kind*_tmp434=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp434,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp430))->v)){{struct
Cyc_Core_Opt*_tmp11A6;*_tmp432=((_tmp11A6=_cycalloc(sizeof(*_tmp11A6)),((
_tmp11A6->v=t2,_tmp11A6))));}return;}else{{void*_tmp436=t2;struct Cyc_Core_Opt*
_tmp438;struct Cyc_Core_Opt**_tmp439;struct Cyc_Core_Opt*_tmp43A;struct Cyc_Absyn_PtrInfo
_tmp43C;_LL26F: {struct Cyc_Absyn_Evar_struct*_tmp437=(struct Cyc_Absyn_Evar_struct*)
_tmp436;if(_tmp437->tag != 1)goto _LL271;else{_tmp438=_tmp437->f2;_tmp439=(struct
Cyc_Core_Opt**)& _tmp437->f2;_tmp43A=_tmp437->f4;}}_LL270: {struct Cyc_List_List*
_tmp43D=(struct Cyc_List_List*)_tmp433->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp43A))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp43D,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp11A7;Cyc_Tcutil_failure_reason=((
_tmp11A7="(type variable would escape scope)",_tag_dyneither(_tmp11A7,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp430->v,_tmp434)){{struct Cyc_Core_Opt*_tmp11A8;*_tmp439=((
_tmp11A8=_cycalloc(sizeof(*_tmp11A8)),((_tmp11A8->v=t1,_tmp11A8))));}return;}{
const char*_tmp11A9;Cyc_Tcutil_failure_reason=((_tmp11A9="(kinds are incompatible)",
_tag_dyneither(_tmp11A9,sizeof(char),25)));}goto _LL26E;}_LL271:{struct Cyc_Absyn_PointerType_struct*
_tmp43B=(struct Cyc_Absyn_PointerType_struct*)_tmp436;if(_tmp43B->tag != 5)goto
_LL273;else{_tmp43C=_tmp43B->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp430->v)->kind
== Cyc_Absyn_BoxKind))goto _LL273;_LL272: {union Cyc_Absyn_Constraint*_tmp441=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp43C.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp442=_tmp441;union Cyc_Absyn_Constraint
_tmp443;int _tmp444;_LL276: _tmp443=*_tmp442;if((_tmp443.No_constr).tag != 3)goto
_LL278;_tmp444=(int)(_tmp443.No_constr).val;_LL277:{struct
_union_Constraint_Eq_constr*_tmp11AA;(_tmp11AA=& _tmp441->Eq_constr,((_tmp11AA->tag=
1,_tmp11AA->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmp11AB;*_tmp432=((
_tmp11AB=_cycalloc(sizeof(*_tmp11AB)),((_tmp11AB->v=t2,_tmp11AB))));}return;
_LL278:;_LL279: goto _LL275;_LL275:;}goto _LL26E;}_LL273:;_LL274: goto _LL26E;_LL26E:;}{
const char*_tmp11AC;Cyc_Tcutil_failure_reason=((_tmp11AC="(kinds are incompatible)",
_tag_dyneither(_tmp11AC,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL26C:;_LL26D: goto _LL269;_LL269:;}{struct _tuple0 _tmp11AD;struct _tuple0 _tmp449=(
_tmp11AD.f1=t2,((_tmp11AD.f2=t1,_tmp11AD)));void*_tmp44A;void*_tmp44C;void*
_tmp44E;void*_tmp450;struct Cyc_Absyn_Tvar*_tmp452;void*_tmp453;struct Cyc_Absyn_Tvar*
_tmp455;void*_tmp456;struct Cyc_Absyn_AggrInfo _tmp458;union Cyc_Absyn_AggrInfoU
_tmp459;struct Cyc_List_List*_tmp45A;void*_tmp45B;struct Cyc_Absyn_AggrInfo _tmp45D;
union Cyc_Absyn_AggrInfoU _tmp45E;struct Cyc_List_List*_tmp45F;void*_tmp460;struct
_tuple2*_tmp462;void*_tmp463;struct _tuple2*_tmp465;void*_tmp466;struct Cyc_List_List*
_tmp468;void*_tmp469;struct Cyc_List_List*_tmp46B;void*_tmp46C;struct Cyc_Absyn_DatatypeInfo
_tmp46E;union Cyc_Absyn_DatatypeInfoU _tmp46F;struct Cyc_Absyn_Datatypedecl**
_tmp470;struct Cyc_Absyn_Datatypedecl*_tmp471;struct Cyc_List_List*_tmp472;void*
_tmp473;struct Cyc_Absyn_DatatypeInfo _tmp475;union Cyc_Absyn_DatatypeInfoU _tmp476;
struct Cyc_Absyn_Datatypedecl**_tmp477;struct Cyc_Absyn_Datatypedecl*_tmp478;
struct Cyc_List_List*_tmp479;void*_tmp47A;struct Cyc_Absyn_DatatypeFieldInfo
_tmp47C;union Cyc_Absyn_DatatypeFieldInfoU _tmp47D;struct _tuple3 _tmp47E;struct Cyc_Absyn_Datatypedecl*
_tmp47F;struct Cyc_Absyn_Datatypefield*_tmp480;struct Cyc_List_List*_tmp481;void*
_tmp482;struct Cyc_Absyn_DatatypeFieldInfo _tmp484;union Cyc_Absyn_DatatypeFieldInfoU
_tmp485;struct _tuple3 _tmp486;struct Cyc_Absyn_Datatypedecl*_tmp487;struct Cyc_Absyn_Datatypefield*
_tmp488;struct Cyc_List_List*_tmp489;void*_tmp48A;struct Cyc_Absyn_PtrInfo _tmp48C;
void*_tmp48D;struct Cyc_Absyn_Tqual _tmp48E;struct Cyc_Absyn_PtrAtts _tmp48F;void*
_tmp490;union Cyc_Absyn_Constraint*_tmp491;union Cyc_Absyn_Constraint*_tmp492;
union Cyc_Absyn_Constraint*_tmp493;void*_tmp494;struct Cyc_Absyn_PtrInfo _tmp496;
void*_tmp497;struct Cyc_Absyn_Tqual _tmp498;struct Cyc_Absyn_PtrAtts _tmp499;void*
_tmp49A;union Cyc_Absyn_Constraint*_tmp49B;union Cyc_Absyn_Constraint*_tmp49C;
union Cyc_Absyn_Constraint*_tmp49D;void*_tmp49E;enum Cyc_Absyn_Sign _tmp4A0;enum 
Cyc_Absyn_Size_of _tmp4A1;void*_tmp4A2;enum Cyc_Absyn_Sign _tmp4A4;enum Cyc_Absyn_Size_of
_tmp4A5;void*_tmp4A6;void*_tmp4A8;void*_tmp4AA;int _tmp4AC;void*_tmp4AD;int
_tmp4AF;void*_tmp4B0;void*_tmp4B2;void*_tmp4B3;void*_tmp4B5;void*_tmp4B6;struct
Cyc_Absyn_Exp*_tmp4B8;void*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BB;void*_tmp4BC;
struct Cyc_Absyn_ArrayInfo _tmp4BE;void*_tmp4BF;struct Cyc_Absyn_Tqual _tmp4C0;
struct Cyc_Absyn_Exp*_tmp4C1;union Cyc_Absyn_Constraint*_tmp4C2;void*_tmp4C3;
struct Cyc_Absyn_ArrayInfo _tmp4C5;void*_tmp4C6;struct Cyc_Absyn_Tqual _tmp4C7;
struct Cyc_Absyn_Exp*_tmp4C8;union Cyc_Absyn_Constraint*_tmp4C9;void*_tmp4CA;
struct Cyc_Absyn_FnInfo _tmp4CC;struct Cyc_List_List*_tmp4CD;struct Cyc_Core_Opt*
_tmp4CE;void*_tmp4CF;struct Cyc_List_List*_tmp4D0;int _tmp4D1;struct Cyc_Absyn_VarargInfo*
_tmp4D2;struct Cyc_List_List*_tmp4D3;struct Cyc_List_List*_tmp4D4;void*_tmp4D5;
struct Cyc_Absyn_FnInfo _tmp4D7;struct Cyc_List_List*_tmp4D8;struct Cyc_Core_Opt*
_tmp4D9;void*_tmp4DA;struct Cyc_List_List*_tmp4DB;int _tmp4DC;struct Cyc_Absyn_VarargInfo*
_tmp4DD;struct Cyc_List_List*_tmp4DE;struct Cyc_List_List*_tmp4DF;void*_tmp4E0;
struct Cyc_List_List*_tmp4E2;void*_tmp4E3;struct Cyc_List_List*_tmp4E5;void*
_tmp4E6;enum Cyc_Absyn_AggrKind _tmp4E8;struct Cyc_List_List*_tmp4E9;void*_tmp4EA;
enum Cyc_Absyn_AggrKind _tmp4EC;struct Cyc_List_List*_tmp4ED;void*_tmp4EE;void*
_tmp4F0;void*_tmp4F2;void*_tmp4F4;void*_tmp4F6;void*_tmp4F8;void*_tmp4F9;void*
_tmp4FB;void*_tmp4FC;void*_tmp4FE;void*_tmp4FF;void*_tmp500;void*_tmp502;void*
_tmp503;void*_tmp504;void*_tmp506;void*_tmp508;void*_tmp50A;void*_tmp50C;void*
_tmp50E;_LL27B: _tmp44A=_tmp449.f1;{struct Cyc_Absyn_Evar_struct*_tmp44B=(struct
Cyc_Absyn_Evar_struct*)_tmp44A;if(_tmp44B->tag != 1)goto _LL27D;};_LL27C: Cyc_Tcutil_unify_it(
t2,t1);return;_LL27D: _tmp44C=_tmp449.f1;{struct Cyc_Absyn_VoidType_struct*_tmp44D=(
struct Cyc_Absyn_VoidType_struct*)_tmp44C;if(_tmp44D->tag != 0)goto _LL27F;};
_tmp44E=_tmp449.f2;{struct Cyc_Absyn_VoidType_struct*_tmp44F=(struct Cyc_Absyn_VoidType_struct*)
_tmp44E;if(_tmp44F->tag != 0)goto _LL27F;};_LL27E: return;_LL27F: _tmp450=_tmp449.f1;{
struct Cyc_Absyn_VarType_struct*_tmp451=(struct Cyc_Absyn_VarType_struct*)_tmp450;
if(_tmp451->tag != 2)goto _LL281;else{_tmp452=_tmp451->f1;}};_tmp453=_tmp449.f2;{
struct Cyc_Absyn_VarType_struct*_tmp454=(struct Cyc_Absyn_VarType_struct*)_tmp453;
if(_tmp454->tag != 2)goto _LL281;else{_tmp455=_tmp454->f1;}};_LL280: {struct
_dyneither_ptr*_tmp510=_tmp452->name;struct _dyneither_ptr*_tmp511=_tmp455->name;
int _tmp512=_tmp452->identity;int _tmp513=_tmp455->identity;if(_tmp513 == _tmp512)
return;{const char*_tmp11AE;Cyc_Tcutil_failure_reason=((_tmp11AE="(variable types are not the same)",
_tag_dyneither(_tmp11AE,sizeof(char),34)));}goto _LL27A;}_LL281: _tmp456=_tmp449.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp457=(struct Cyc_Absyn_AggrType_struct*)
_tmp456;if(_tmp457->tag != 12)goto _LL283;else{_tmp458=_tmp457->f1;_tmp459=_tmp458.aggr_info;
_tmp45A=_tmp458.targs;}};_tmp45B=_tmp449.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp45C=(struct Cyc_Absyn_AggrType_struct*)_tmp45B;if(_tmp45C->tag != 12)goto
_LL283;else{_tmp45D=_tmp45C->f1;_tmp45E=_tmp45D.aggr_info;_tmp45F=_tmp45D.targs;}};
_LL282: {enum Cyc_Absyn_AggrKind _tmp516;struct _tuple2*_tmp517;struct _tuple12
_tmp515=Cyc_Absyn_aggr_kinded_name(_tmp45E);_tmp516=_tmp515.f1;_tmp517=_tmp515.f2;{
enum Cyc_Absyn_AggrKind _tmp519;struct _tuple2*_tmp51A;struct _tuple12 _tmp518=Cyc_Absyn_aggr_kinded_name(
_tmp459);_tmp519=_tmp518.f1;_tmp51A=_tmp518.f2;if(_tmp516 != _tmp519){{const char*
_tmp11AF;Cyc_Tcutil_failure_reason=((_tmp11AF="(struct and union type)",
_tag_dyneither(_tmp11AF,sizeof(char),24)));}goto _LL27A;}if(Cyc_Absyn_qvar_cmp(
_tmp517,_tmp51A)!= 0){{const char*_tmp11B0;Cyc_Tcutil_failure_reason=((_tmp11B0="(different type name)",
_tag_dyneither(_tmp11B0,sizeof(char),22)));}goto _LL27A;}Cyc_Tcutil_unify_list(
_tmp45F,_tmp45A);return;};}_LL283: _tmp460=_tmp449.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp461=(struct Cyc_Absyn_EnumType_struct*)_tmp460;if(_tmp461->tag != 14)goto
_LL285;else{_tmp462=_tmp461->f1;}};_tmp463=_tmp449.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp464=(struct Cyc_Absyn_EnumType_struct*)_tmp463;if(_tmp464->tag != 14)goto
_LL285;else{_tmp465=_tmp464->f1;}};_LL284: if(Cyc_Absyn_qvar_cmp(_tmp462,_tmp465)
== 0)return;{const char*_tmp11B1;Cyc_Tcutil_failure_reason=((_tmp11B1="(different enum types)",
_tag_dyneither(_tmp11B1,sizeof(char),23)));}goto _LL27A;_LL285: _tmp466=_tmp449.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp467=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp466;if(_tmp467->tag != 15)goto _LL287;else{_tmp468=_tmp467->f1;}};_tmp469=
_tmp449.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp46A=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp469;if(_tmp46A->tag != 15)goto _LL287;else{_tmp46B=_tmp46A->f1;}};_LL286: {int
bad=0;for(0;_tmp468 != 0  && _tmp46B != 0;(_tmp468=_tmp468->tl,_tmp46B=_tmp46B->tl)){
struct Cyc_Absyn_Enumfield*_tmp51E=(struct Cyc_Absyn_Enumfield*)_tmp468->hd;struct
Cyc_Absyn_Enumfield*_tmp51F=(struct Cyc_Absyn_Enumfield*)_tmp46B->hd;if(Cyc_Absyn_qvar_cmp(
_tmp51E->name,_tmp51F->name)!= 0){{const char*_tmp11B2;Cyc_Tcutil_failure_reason=((
_tmp11B2="(different names for enum fields)",_tag_dyneither(_tmp11B2,sizeof(char),
34)));}bad=1;break;}if(_tmp51E->tag == _tmp51F->tag)continue;if(_tmp51E->tag == 0
 || _tmp51F->tag == 0){{const char*_tmp11B3;Cyc_Tcutil_failure_reason=((_tmp11B3="(different tag values for enum fields)",
_tag_dyneither(_tmp11B3,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp51E->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp51F->tag))){{const char*_tmp11B4;Cyc_Tcutil_failure_reason=((_tmp11B4="(different tag values for enum fields)",
_tag_dyneither(_tmp11B4,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL27A;if(
_tmp468 == 0  && _tmp46B == 0)return;{const char*_tmp11B5;Cyc_Tcutil_failure_reason=((
_tmp11B5="(different number of fields for enums)",_tag_dyneither(_tmp11B5,
sizeof(char),39)));}goto _LL27A;}_LL287: _tmp46C=_tmp449.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp46D=(struct Cyc_Absyn_DatatypeType_struct*)_tmp46C;if(_tmp46D->tag != 3)goto
_LL289;else{_tmp46E=_tmp46D->f1;_tmp46F=_tmp46E.datatype_info;if((_tmp46F.KnownDatatype).tag
!= 2)goto _LL289;_tmp470=(struct Cyc_Absyn_Datatypedecl**)(_tmp46F.KnownDatatype).val;
_tmp471=*_tmp470;_tmp472=_tmp46E.targs;}};_tmp473=_tmp449.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp474=(struct Cyc_Absyn_DatatypeType_struct*)_tmp473;if(_tmp474->tag != 3)goto
_LL289;else{_tmp475=_tmp474->f1;_tmp476=_tmp475.datatype_info;if((_tmp476.KnownDatatype).tag
!= 2)goto _LL289;_tmp477=(struct Cyc_Absyn_Datatypedecl**)(_tmp476.KnownDatatype).val;
_tmp478=*_tmp477;_tmp479=_tmp475.targs;}};_LL288: if(_tmp471 == _tmp478  || Cyc_Absyn_qvar_cmp(
_tmp471->name,_tmp478->name)== 0){Cyc_Tcutil_unify_list(_tmp479,_tmp472);return;}
goto _LL27A;_LL289: _tmp47A=_tmp449.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp47B=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp47A;if(_tmp47B->tag != 4)
goto _LL28B;else{_tmp47C=_tmp47B->f1;_tmp47D=_tmp47C.field_info;if((_tmp47D.KnownDatatypefield).tag
!= 2)goto _LL28B;_tmp47E=(struct _tuple3)(_tmp47D.KnownDatatypefield).val;_tmp47F=
_tmp47E.f1;_tmp480=_tmp47E.f2;_tmp481=_tmp47C.targs;}};_tmp482=_tmp449.f2;{
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp483=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp482;if(_tmp483->tag != 4)goto _LL28B;else{_tmp484=_tmp483->f1;_tmp485=_tmp484.field_info;
if((_tmp485.KnownDatatypefield).tag != 2)goto _LL28B;_tmp486=(struct _tuple3)(
_tmp485.KnownDatatypefield).val;_tmp487=_tmp486.f1;_tmp488=_tmp486.f2;_tmp489=
_tmp484.targs;}};_LL28A: if((_tmp47F == _tmp487  || Cyc_Absyn_qvar_cmp(_tmp47F->name,
_tmp487->name)== 0) && (_tmp480 == _tmp488  || Cyc_Absyn_qvar_cmp(_tmp480->name,
_tmp488->name)== 0)){Cyc_Tcutil_unify_list(_tmp489,_tmp481);return;}{const char*
_tmp11B6;Cyc_Tcutil_failure_reason=((_tmp11B6="(different datatype field types)",
_tag_dyneither(_tmp11B6,sizeof(char),33)));}goto _LL27A;_LL28B: _tmp48A=_tmp449.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp48B=(struct Cyc_Absyn_PointerType_struct*)
_tmp48A;if(_tmp48B->tag != 5)goto _LL28D;else{_tmp48C=_tmp48B->f1;_tmp48D=_tmp48C.elt_typ;
_tmp48E=_tmp48C.elt_tq;_tmp48F=_tmp48C.ptr_atts;_tmp490=_tmp48F.rgn;_tmp491=
_tmp48F.nullable;_tmp492=_tmp48F.bounds;_tmp493=_tmp48F.zero_term;}};_tmp494=
_tmp449.f2;{struct Cyc_Absyn_PointerType_struct*_tmp495=(struct Cyc_Absyn_PointerType_struct*)
_tmp494;if(_tmp495->tag != 5)goto _LL28D;else{_tmp496=_tmp495->f1;_tmp497=_tmp496.elt_typ;
_tmp498=_tmp496.elt_tq;_tmp499=_tmp496.ptr_atts;_tmp49A=_tmp499.rgn;_tmp49B=
_tmp499.nullable;_tmp49C=_tmp499.bounds;_tmp49D=_tmp499.zero_term;}};_LL28C: Cyc_Tcutil_unify_it(
_tmp497,_tmp48D);Cyc_Tcutil_unify_it(_tmp490,_tmp49A);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp11B7;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp49D,_tmp493,((_tmp11B7="(not both zero terminated)",
_tag_dyneither(_tmp11B7,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp498,
_tmp497,_tmp48E,_tmp48D);{const char*_tmp11B8;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp49C,_tmp492,((
_tmp11B8="(different pointer bounds)",_tag_dyneither(_tmp11B8,sizeof(char),27))));}{
void*_tmp527=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp49C);_LL2B6: {struct Cyc_Absyn_DynEither_b_struct*
_tmp528=(struct Cyc_Absyn_DynEither_b_struct*)_tmp527;if(_tmp528->tag != 0)goto
_LL2B8;}_LL2B7: return;_LL2B8:;_LL2B9: goto _LL2B5;_LL2B5:;}{const char*_tmp11B9;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp49B,_tmp491,((_tmp11B9="(incompatible pointer types)",_tag_dyneither(
_tmp11B9,sizeof(char),29))));}return;_LL28D: _tmp49E=_tmp449.f1;{struct Cyc_Absyn_IntType_struct*
_tmp49F=(struct Cyc_Absyn_IntType_struct*)_tmp49E;if(_tmp49F->tag != 6)goto _LL28F;
else{_tmp4A0=_tmp49F->f1;_tmp4A1=_tmp49F->f2;}};_tmp4A2=_tmp449.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4A3=(struct Cyc_Absyn_IntType_struct*)_tmp4A2;if(_tmp4A3->tag != 6)goto _LL28F;
else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A3->f2;}};_LL28E: if(_tmp4A4 == _tmp4A0  && ((
_tmp4A5 == _tmp4A1  || _tmp4A5 == Cyc_Absyn_Int_sz  && _tmp4A1 == Cyc_Absyn_Long_sz)
 || _tmp4A5 == Cyc_Absyn_Long_sz  && _tmp4A1 == Cyc_Absyn_Int_sz))return;{const char*
_tmp11BA;Cyc_Tcutil_failure_reason=((_tmp11BA="(different integral types)",
_tag_dyneither(_tmp11BA,sizeof(char),27)));}goto _LL27A;_LL28F: _tmp4A6=_tmp449.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp4A7=(struct Cyc_Absyn_FloatType_struct*)
_tmp4A6;if(_tmp4A7->tag != 7)goto _LL291;};_tmp4A8=_tmp449.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp4A9=(struct Cyc_Absyn_FloatType_struct*)_tmp4A8;if(_tmp4A9->tag != 7)goto
_LL291;};_LL290: return;_LL291: _tmp4AA=_tmp449.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp4AB=(struct Cyc_Absyn_DoubleType_struct*)_tmp4AA;if(_tmp4AB->tag != 8)goto
_LL293;else{_tmp4AC=_tmp4AB->f1;}};_tmp4AD=_tmp449.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp4AE=(struct Cyc_Absyn_DoubleType_struct*)_tmp4AD;if(_tmp4AE->tag != 8)goto
_LL293;else{_tmp4AF=_tmp4AE->f1;}};_LL292: if(_tmp4AC == _tmp4AF)return;goto _LL27A;
_LL293: _tmp4B0=_tmp449.f1;{struct Cyc_Absyn_TagType_struct*_tmp4B1=(struct Cyc_Absyn_TagType_struct*)
_tmp4B0;if(_tmp4B1->tag != 20)goto _LL295;else{_tmp4B2=(void*)_tmp4B1->f1;}};
_tmp4B3=_tmp449.f2;{struct Cyc_Absyn_TagType_struct*_tmp4B4=(struct Cyc_Absyn_TagType_struct*)
_tmp4B3;if(_tmp4B4->tag != 20)goto _LL295;else{_tmp4B5=(void*)_tmp4B4->f1;}};
_LL294: Cyc_Tcutil_unify_it(_tmp4B2,_tmp4B5);return;_LL295: _tmp4B6=_tmp449.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp4B7=(struct Cyc_Absyn_ValueofType_struct*)
_tmp4B6;if(_tmp4B7->tag != 19)goto _LL297;else{_tmp4B8=_tmp4B7->f1;}};_tmp4B9=
_tmp449.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp4BA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp4B9;if(_tmp4BA->tag != 19)goto _LL297;else{_tmp4BB=_tmp4BA->f1;}};_LL296: if(!
Cyc_Evexp_same_const_exp(_tmp4B8,_tmp4BB)){{const char*_tmp11BB;Cyc_Tcutil_failure_reason=((
_tmp11BB="(cannot prove expressions are the same)",_tag_dyneither(_tmp11BB,
sizeof(char),40)));}goto _LL27A;}return;_LL297: _tmp4BC=_tmp449.f1;{struct Cyc_Absyn_ArrayType_struct*
_tmp4BD=(struct Cyc_Absyn_ArrayType_struct*)_tmp4BC;if(_tmp4BD->tag != 9)goto
_LL299;else{_tmp4BE=_tmp4BD->f1;_tmp4BF=_tmp4BE.elt_type;_tmp4C0=_tmp4BE.tq;
_tmp4C1=_tmp4BE.num_elts;_tmp4C2=_tmp4BE.zero_term;}};_tmp4C3=_tmp449.f2;{struct
Cyc_Absyn_ArrayType_struct*_tmp4C4=(struct Cyc_Absyn_ArrayType_struct*)_tmp4C3;
if(_tmp4C4->tag != 9)goto _LL299;else{_tmp4C5=_tmp4C4->f1;_tmp4C6=_tmp4C5.elt_type;
_tmp4C7=_tmp4C5.tq;_tmp4C8=_tmp4C5.num_elts;_tmp4C9=_tmp4C5.zero_term;}};_LL298:
Cyc_Tcutil_unify_it(_tmp4C6,_tmp4BF);Cyc_Tcutil_unify_tqual(_tmp4C7,_tmp4C6,
_tmp4C0,_tmp4BF);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp11BC;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4C2,_tmp4C9,((_tmp11BC="(not both zero terminated)",_tag_dyneither(_tmp11BC,
sizeof(char),27))));}if(_tmp4C1 == _tmp4C8)return;if(_tmp4C1 == 0  || _tmp4C8 == 0)
goto _LL27A;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4C1,(struct Cyc_Absyn_Exp*)
_tmp4C8))return;{const char*_tmp11BD;Cyc_Tcutil_failure_reason=((_tmp11BD="(different array sizes)",
_tag_dyneither(_tmp11BD,sizeof(char),24)));}goto _LL27A;_LL299: _tmp4CA=_tmp449.f1;{
struct Cyc_Absyn_FnType_struct*_tmp4CB=(struct Cyc_Absyn_FnType_struct*)_tmp4CA;
if(_tmp4CB->tag != 10)goto _LL29B;else{_tmp4CC=_tmp4CB->f1;_tmp4CD=_tmp4CC.tvars;
_tmp4CE=_tmp4CC.effect;_tmp4CF=_tmp4CC.ret_typ;_tmp4D0=_tmp4CC.args;_tmp4D1=
_tmp4CC.c_varargs;_tmp4D2=_tmp4CC.cyc_varargs;_tmp4D3=_tmp4CC.rgn_po;_tmp4D4=
_tmp4CC.attributes;}};_tmp4D5=_tmp449.f2;{struct Cyc_Absyn_FnType_struct*_tmp4D6=(
struct Cyc_Absyn_FnType_struct*)_tmp4D5;if(_tmp4D6->tag != 10)goto _LL29B;else{
_tmp4D7=_tmp4D6->f1;_tmp4D8=_tmp4D7.tvars;_tmp4D9=_tmp4D7.effect;_tmp4DA=_tmp4D7.ret_typ;
_tmp4DB=_tmp4D7.args;_tmp4DC=_tmp4D7.c_varargs;_tmp4DD=_tmp4D7.cyc_varargs;
_tmp4DE=_tmp4D7.rgn_po;_tmp4DF=_tmp4D7.attributes;}};_LL29A: {int done=0;struct
_RegionHandle _tmp52E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp52E;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp4D8 != 0){if(_tmp4CD == 0){{
const char*_tmp11BE;Cyc_Tcutil_failure_reason=((_tmp11BE="(second function type has too few type variables)",
_tag_dyneither(_tmp11BE,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
void*_tmp530=((struct Cyc_Absyn_Tvar*)_tmp4D8->hd)->kind;void*_tmp531=((struct Cyc_Absyn_Tvar*)
_tmp4CD->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp530,_tmp531)){{const char*
_tmp11C4;void*_tmp11C3[3];struct Cyc_String_pa_struct _tmp11C2;struct Cyc_String_pa_struct
_tmp11C1;struct Cyc_String_pa_struct _tmp11C0;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp11C0.tag=0,((_tmp11C0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp4CD->hd,& Cyc_Tcutil_bk))),((_tmp11C1.tag=0,((_tmp11C1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp4D8->hd,& Cyc_Tcutil_bk))),((_tmp11C2.tag=0,((_tmp11C2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp4D8->hd)),((_tmp11C3[0]=& _tmp11C2,((_tmp11C3[1]=& _tmp11C1,((_tmp11C3[2]=&
_tmp11C0,Cyc_aprintf(((_tmp11C4="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp11C4,sizeof(char),44))),_tag_dyneither(_tmp11C3,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple15*_tmp11CE;struct Cyc_Absyn_VarType_struct _tmp11CD;struct Cyc_Absyn_VarType_struct*
_tmp11CC;struct Cyc_List_List*_tmp11CB;inst=((_tmp11CB=_region_malloc(rgn,sizeof(*
_tmp11CB)),((_tmp11CB->hd=((_tmp11CE=_region_malloc(rgn,sizeof(*_tmp11CE)),((
_tmp11CE->f1=(struct Cyc_Absyn_Tvar*)_tmp4CD->hd,((_tmp11CE->f2=(void*)((_tmp11CC=
_cycalloc(sizeof(*_tmp11CC)),((_tmp11CC[0]=((_tmp11CD.tag=2,((_tmp11CD.f1=(
struct Cyc_Absyn_Tvar*)_tmp4D8->hd,_tmp11CD)))),_tmp11CC)))),_tmp11CE)))))),((
_tmp11CB->tl=inst,_tmp11CB))))));}_tmp4D8=_tmp4D8->tl;_tmp4CD=_tmp4CD->tl;};}if(
_tmp4CD != 0){{const char*_tmp11CF;Cyc_Tcutil_failure_reason=((_tmp11CF="(second function type has too many type variables)",
_tag_dyneither(_tmp11CF,sizeof(char),51)));}_npop_handler(0);goto _LL27A;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp11DB;struct Cyc_Absyn_FnInfo _tmp11DA;
struct Cyc_Absyn_FnType_struct*_tmp11D9;struct Cyc_Absyn_FnType_struct _tmp11D5;
struct Cyc_Absyn_FnInfo _tmp11D4;struct Cyc_Absyn_FnType_struct*_tmp11D3;Cyc_Tcutil_unify_it((
void*)((_tmp11D3=_cycalloc(sizeof(*_tmp11D3)),((_tmp11D3[0]=((_tmp11D5.tag=10,((
_tmp11D5.f1=((_tmp11D4.tvars=0,((_tmp11D4.effect=_tmp4D9,((_tmp11D4.ret_typ=
_tmp4DA,((_tmp11D4.args=_tmp4DB,((_tmp11D4.c_varargs=_tmp4DC,((_tmp11D4.cyc_varargs=
_tmp4DD,((_tmp11D4.rgn_po=_tmp4DE,((_tmp11D4.attributes=_tmp4DF,_tmp11D4)))))))))))))))),
_tmp11D5)))),_tmp11D3)))),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)((_tmp11D9=
_cycalloc(sizeof(*_tmp11D9)),((_tmp11D9[0]=((_tmp11DB.tag=10,((_tmp11DB.f1=((
_tmp11DA.tvars=0,((_tmp11DA.effect=_tmp4CE,((_tmp11DA.ret_typ=_tmp4CF,((_tmp11DA.args=
_tmp4D0,((_tmp11DA.c_varargs=_tmp4D1,((_tmp11DA.cyc_varargs=_tmp4D2,((_tmp11DA.rgn_po=
_tmp4D3,((_tmp11DA.attributes=_tmp4D4,_tmp11DA)))))))))))))))),_tmp11DB)))),
_tmp11D9))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp4DA,_tmp4CF);for(0;_tmp4DB != 0  && _tmp4D0 != 0;(_tmp4DB=_tmp4DB->tl,_tmp4D0=
_tmp4D0->tl)){struct Cyc_Absyn_Tqual _tmp543;void*_tmp544;struct _tuple9 _tmp542=*((
struct _tuple9*)_tmp4DB->hd);_tmp543=_tmp542.f2;_tmp544=_tmp542.f3;{struct Cyc_Absyn_Tqual
_tmp546;void*_tmp547;struct _tuple9 _tmp545=*((struct _tuple9*)_tmp4D0->hd);_tmp546=
_tmp545.f2;_tmp547=_tmp545.f3;Cyc_Tcutil_unify_it(_tmp544,_tmp547);Cyc_Tcutil_unify_tqual(
_tmp543,_tmp544,_tmp546,_tmp547);};}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp4DB != 0  || _tmp4D0 != 0){{const char*_tmp11DC;Cyc_Tcutil_failure_reason=((
_tmp11DC="(function types have different number of arguments)",_tag_dyneither(
_tmp11DC,sizeof(char),52)));}_npop_handler(0);goto _LL27A;}if(_tmp4DC != _tmp4D1){{
const char*_tmp11DD;Cyc_Tcutil_failure_reason=((_tmp11DD="(only one function type takes C varargs)",
_tag_dyneither(_tmp11DD,sizeof(char),41)));}_npop_handler(0);goto _LL27A;}{int
bad_cyc_vararg=0;{struct _tuple21 _tmp11DE;struct _tuple21 _tmp54B=(_tmp11DE.f1=
_tmp4DD,((_tmp11DE.f2=_tmp4D2,_tmp11DE)));struct Cyc_Absyn_VarargInfo*_tmp54C;
struct Cyc_Absyn_VarargInfo*_tmp54D;struct Cyc_Absyn_VarargInfo*_tmp54E;struct Cyc_Absyn_VarargInfo*
_tmp54F;struct Cyc_Absyn_VarargInfo*_tmp550;struct Cyc_Absyn_VarargInfo _tmp551;
struct Cyc_Core_Opt*_tmp552;struct Cyc_Absyn_Tqual _tmp553;void*_tmp554;int _tmp555;
struct Cyc_Absyn_VarargInfo*_tmp556;struct Cyc_Absyn_VarargInfo _tmp557;struct Cyc_Core_Opt*
_tmp558;struct Cyc_Absyn_Tqual _tmp559;void*_tmp55A;int _tmp55B;_LL2BB: _tmp54C=
_tmp54B.f1;if(_tmp54C != 0)goto _LL2BD;_tmp54D=_tmp54B.f2;if(_tmp54D != 0)goto
_LL2BD;_LL2BC: goto _LL2BA;_LL2BD: _tmp54E=_tmp54B.f1;if(_tmp54E != 0)goto _LL2BF;
_LL2BE: goto _LL2C0;_LL2BF: _tmp54F=_tmp54B.f2;if(_tmp54F != 0)goto _LL2C1;_LL2C0:
bad_cyc_vararg=1;{const char*_tmp11DF;Cyc_Tcutil_failure_reason=((_tmp11DF="(only one function type takes varargs)",
_tag_dyneither(_tmp11DF,sizeof(char),39)));}goto _LL2BA;_LL2C1: _tmp550=_tmp54B.f1;
if(_tmp550 == 0)goto _LL2BA;_tmp551=*_tmp550;_tmp552=_tmp551.name;_tmp553=_tmp551.tq;
_tmp554=_tmp551.type;_tmp555=_tmp551.inject;_tmp556=_tmp54B.f2;if(_tmp556 == 0)
goto _LL2BA;_tmp557=*_tmp556;_tmp558=_tmp557.name;_tmp559=_tmp557.tq;_tmp55A=
_tmp557.type;_tmp55B=_tmp557.inject;_LL2C2: Cyc_Tcutil_unify_it(_tmp554,_tmp55A);
Cyc_Tcutil_unify_tqual(_tmp553,_tmp554,_tmp559,_tmp55A);if(_tmp555 != _tmp55B){
bad_cyc_vararg=1;{const char*_tmp11E0;Cyc_Tcutil_failure_reason=((_tmp11E0="(only one function type injects varargs)",
_tag_dyneither(_tmp11E0,sizeof(char),41)));};}goto _LL2BA;_LL2BA:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL27A;}{int bad_effect=0;{struct _tuple22
_tmp11E1;struct _tuple22 _tmp55F=(_tmp11E1.f1=_tmp4D9,((_tmp11E1.f2=_tmp4CE,
_tmp11E1)));struct Cyc_Core_Opt*_tmp560;struct Cyc_Core_Opt*_tmp561;struct Cyc_Core_Opt*
_tmp562;struct Cyc_Core_Opt*_tmp563;_LL2C4: _tmp560=_tmp55F.f1;if(_tmp560 != 0)goto
_LL2C6;_tmp561=_tmp55F.f2;if(_tmp561 != 0)goto _LL2C6;_LL2C5: goto _LL2C3;_LL2C6:
_tmp562=_tmp55F.f1;if(_tmp562 != 0)goto _LL2C8;_LL2C7: goto _LL2C9;_LL2C8: _tmp563=
_tmp55F.f2;if(_tmp563 != 0)goto _LL2CA;_LL2C9: bad_effect=1;goto _LL2C3;_LL2CA:;
_LL2CB: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4D9))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4CE))->v);
goto _LL2C3;_LL2C3:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp11E2;Cyc_Tcutil_failure_reason=((_tmp11E2="(function type effects do not unify)",
_tag_dyneither(_tmp11E2,sizeof(char),37)));}_npop_handler(0);goto _LL27A;}if(!Cyc_Tcutil_same_atts(
_tmp4D4,_tmp4DF)){{const char*_tmp11E3;Cyc_Tcutil_failure_reason=((_tmp11E3="(function types have different attributes)",
_tag_dyneither(_tmp11E3,sizeof(char),43)));}_npop_handler(0);goto _LL27A;}if(!Cyc_Tcutil_same_rgn_po(
_tmp4D3,_tmp4DE)){{const char*_tmp11E4;Cyc_Tcutil_failure_reason=((_tmp11E4="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp11E4,sizeof(char),58)));}_npop_handler(0);goto _LL27A;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL29B: _tmp4E0=_tmp449.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp4E1=(struct Cyc_Absyn_TupleType_struct*)_tmp4E0;
if(_tmp4E1->tag != 11)goto _LL29D;else{_tmp4E2=_tmp4E1->f1;}};_tmp4E3=_tmp449.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp4E4=(struct Cyc_Absyn_TupleType_struct*)
_tmp4E3;if(_tmp4E4->tag != 11)goto _LL29D;else{_tmp4E5=_tmp4E4->f1;}};_LL29C: for(0;
_tmp4E5 != 0  && _tmp4E2 != 0;(_tmp4E5=_tmp4E5->tl,_tmp4E2=_tmp4E2->tl)){struct Cyc_Absyn_Tqual
_tmp568;void*_tmp569;struct _tuple11 _tmp567=*((struct _tuple11*)_tmp4E5->hd);
_tmp568=_tmp567.f1;_tmp569=_tmp567.f2;{struct Cyc_Absyn_Tqual _tmp56B;void*_tmp56C;
struct _tuple11 _tmp56A=*((struct _tuple11*)_tmp4E2->hd);_tmp56B=_tmp56A.f1;_tmp56C=
_tmp56A.f2;Cyc_Tcutil_unify_it(_tmp569,_tmp56C);Cyc_Tcutil_unify_tqual(_tmp568,
_tmp569,_tmp56B,_tmp56C);};}if(_tmp4E5 == 0  && _tmp4E2 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp11E5;Cyc_Tcutil_failure_reason=((
_tmp11E5="(tuple types have different numbers of components)",_tag_dyneither(
_tmp11E5,sizeof(char),51)));}goto _LL27A;_LL29D: _tmp4E6=_tmp449.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp4E7=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4E6;if(_tmp4E7->tag != 13)goto
_LL29F;else{_tmp4E8=_tmp4E7->f1;_tmp4E9=_tmp4E7->f2;}};_tmp4EA=_tmp449.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp4EB=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4EA;if(_tmp4EB->tag != 13)goto _LL29F;else{_tmp4EC=_tmp4EB->f1;_tmp4ED=_tmp4EB->f2;}};
_LL29E: if(_tmp4EC != _tmp4E8){{const char*_tmp11E6;Cyc_Tcutil_failure_reason=((
_tmp11E6="(struct and union type)",_tag_dyneither(_tmp11E6,sizeof(char),24)));}
goto _LL27A;}for(0;_tmp4ED != 0  && _tmp4E9 != 0;(_tmp4ED=_tmp4ED->tl,_tmp4E9=
_tmp4E9->tl)){struct Cyc_Absyn_Aggrfield*_tmp56F=(struct Cyc_Absyn_Aggrfield*)
_tmp4ED->hd;struct Cyc_Absyn_Aggrfield*_tmp570=(struct Cyc_Absyn_Aggrfield*)
_tmp4E9->hd;if(Cyc_strptrcmp(_tmp56F->name,_tmp570->name)!= 0){{const char*
_tmp11E7;Cyc_Tcutil_failure_reason=((_tmp11E7="(different member names)",
_tag_dyneither(_tmp11E7,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp56F->type,_tmp570->type);Cyc_Tcutil_unify_tqual(_tmp56F->tq,
_tmp56F->type,_tmp570->tq,_tmp570->type);if(!Cyc_Tcutil_same_atts(_tmp56F->attributes,
_tmp570->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp11E8;Cyc_Tcutil_failure_reason=((_tmp11E8="(different attributes on member)",
_tag_dyneither(_tmp11E8,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp56F->width != 0  && _tmp570->width == 0  || _tmp570->width != 0  && _tmp56F->width
== 0) || (_tmp56F->width != 0  && _tmp570->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp56F->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp570->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp11E9;Cyc_Tcutil_failure_reason=((_tmp11E9="(different bitfield widths on member)",
_tag_dyneither(_tmp11E9,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp4ED == 0  && _tmp4E9 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp11EA;Cyc_Tcutil_failure_reason=((_tmp11EA="(different number of members)",
_tag_dyneither(_tmp11EA,sizeof(char),30)));}goto _LL27A;_LL29F: _tmp4EE=_tmp449.f1;{
struct Cyc_Absyn_HeapRgn_struct*_tmp4EF=(struct Cyc_Absyn_HeapRgn_struct*)_tmp4EE;
if(_tmp4EF->tag != 21)goto _LL2A1;};_tmp4F0=_tmp449.f2;{struct Cyc_Absyn_HeapRgn_struct*
_tmp4F1=(struct Cyc_Absyn_HeapRgn_struct*)_tmp4F0;if(_tmp4F1->tag != 21)goto _LL2A1;};
_LL2A0: return;_LL2A1: _tmp4F2=_tmp449.f1;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp4F3=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp4F2;if(_tmp4F3->tag != 22)goto
_LL2A3;};_tmp4F4=_tmp449.f2;{struct Cyc_Absyn_UniqueRgn_struct*_tmp4F5=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp4F4;if(_tmp4F5->tag != 22)goto _LL2A3;};_LL2A2: return;_LL2A3: _tmp4F6=_tmp449.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp4F7=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4F6;if(_tmp4F7->tag != 16)goto _LL2A5;else{_tmp4F8=(void*)_tmp4F7->f1;}};
_tmp4F9=_tmp449.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp4FA=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4F9;if(_tmp4FA->tag != 16)goto _LL2A5;else{_tmp4FB=(void*)_tmp4FA->f1;}};
_LL2A4: Cyc_Tcutil_unify_it(_tmp4F8,_tmp4FB);return;_LL2A5: _tmp4FC=_tmp449.f1;{
struct Cyc_Absyn_DynRgnType_struct*_tmp4FD=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp4FC;if(_tmp4FD->tag != 17)goto _LL2A7;else{_tmp4FE=(void*)_tmp4FD->f1;_tmp4FF=(
void*)_tmp4FD->f2;}};_tmp500=_tmp449.f2;{struct Cyc_Absyn_DynRgnType_struct*
_tmp501=(struct Cyc_Absyn_DynRgnType_struct*)_tmp500;if(_tmp501->tag != 17)goto
_LL2A7;else{_tmp502=(void*)_tmp501->f1;_tmp503=(void*)_tmp501->f2;}};_LL2A6: Cyc_Tcutil_unify_it(
_tmp4FE,_tmp502);Cyc_Tcutil_unify_it(_tmp4FF,_tmp503);return;_LL2A7: _tmp504=
_tmp449.f1;{struct Cyc_Absyn_JoinEff_struct*_tmp505=(struct Cyc_Absyn_JoinEff_struct*)
_tmp504;if(_tmp505->tag != 24)goto _LL2A9;};_LL2A8: goto _LL2AA;_LL2A9: _tmp506=
_tmp449.f2;{struct Cyc_Absyn_JoinEff_struct*_tmp507=(struct Cyc_Absyn_JoinEff_struct*)
_tmp506;if(_tmp507->tag != 24)goto _LL2AB;};_LL2AA: goto _LL2AC;_LL2AB: _tmp508=
_tmp449.f1;{struct Cyc_Absyn_AccessEff_struct*_tmp509=(struct Cyc_Absyn_AccessEff_struct*)
_tmp508;if(_tmp509->tag != 23)goto _LL2AD;};_LL2AC: goto _LL2AE;_LL2AD: _tmp50A=
_tmp449.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp50B=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp50A;if(_tmp50B->tag != 25)goto _LL2AF;};_LL2AE: goto _LL2B0;_LL2AF: _tmp50C=
_tmp449.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp50D=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp50C;if(_tmp50D->tag != 25)goto _LL2B1;};_LL2B0: goto _LL2B2;_LL2B1: _tmp50E=
_tmp449.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp50F=(struct Cyc_Absyn_AccessEff_struct*)
_tmp50E;if(_tmp50F->tag != 23)goto _LL2B3;};_LL2B2: if(Cyc_Tcutil_unify_effect(t1,
t2))return;{const char*_tmp11EB;Cyc_Tcutil_failure_reason=((_tmp11EB="(effects don't unify)",
_tag_dyneither(_tmp11EB,sizeof(char),22)));}goto _LL27A;_LL2B3:;_LL2B4: goto _LL27A;
_LL27A:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}int Cyc_Tcutil_star_cmp(int(*
cmp)(void*,void*),void*a1,void*a2);int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2
== 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int
Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);static
int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int
_tmp576=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp577=(
tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp576,_tmp577);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp578=x;union Cyc_Absyn_Constraint _tmp579;int _tmp57A;
union Cyc_Absyn_Constraint _tmp57B;void*_tmp57C;union Cyc_Absyn_Constraint _tmp57D;
union Cyc_Absyn_Constraint*_tmp57E;_LL2CD: _tmp579=*_tmp578;if((_tmp579.No_constr).tag
!= 3)goto _LL2CF;_tmp57A=(int)(_tmp579.No_constr).val;_LL2CE: return - 1;_LL2CF:
_tmp57B=*_tmp578;if((_tmp57B.Eq_constr).tag != 1)goto _LL2D1;_tmp57C=(void*)(
_tmp57B.Eq_constr).val;_LL2D0: {union Cyc_Absyn_Constraint*_tmp57F=y;union Cyc_Absyn_Constraint
_tmp580;int _tmp581;union Cyc_Absyn_Constraint _tmp582;void*_tmp583;union Cyc_Absyn_Constraint
_tmp584;union Cyc_Absyn_Constraint*_tmp585;_LL2D4: _tmp580=*_tmp57F;if((_tmp580.No_constr).tag
!= 3)goto _LL2D6;_tmp581=(int)(_tmp580.No_constr).val;_LL2D5: return 1;_LL2D6:
_tmp582=*_tmp57F;if((_tmp582.Eq_constr).tag != 1)goto _LL2D8;_tmp583=(void*)(
_tmp582.Eq_constr).val;_LL2D7: return cmp(_tmp57C,_tmp583);_LL2D8: _tmp584=*_tmp57F;
if((_tmp584.Forward_constr).tag != 2)goto _LL2D3;_tmp585=(union Cyc_Absyn_Constraint*)(
_tmp584.Forward_constr).val;_LL2D9: {const char*_tmp11EE;void*_tmp11ED;(_tmp11ED=
0,Cyc_Tcutil_impos(((_tmp11EE="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp11EE,sizeof(char),40))),_tag_dyneither(_tmp11ED,sizeof(void*),
0)));}_LL2D3:;}_LL2D1: _tmp57D=*_tmp578;if((_tmp57D.Forward_constr).tag != 2)goto
_LL2CC;_tmp57E=(union Cyc_Absyn_Constraint*)(_tmp57D.Forward_constr).val;_LL2D2: {
const char*_tmp11F1;void*_tmp11F0;(_tmp11F0=0,Cyc_Tcutil_impos(((_tmp11F1="unify_conref: forward after compress",
_tag_dyneither(_tmp11F1,sizeof(char),37))),_tag_dyneither(_tmp11F0,sizeof(void*),
0)));}_LL2CC:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2);static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2){struct _tuple11 _tmp58B;struct Cyc_Absyn_Tqual _tmp58C;void*_tmp58D;
struct _tuple11*_tmp58A=tqt1;_tmp58B=*_tmp58A;_tmp58C=_tmp58B.f1;_tmp58D=_tmp58B.f2;{
struct _tuple11 _tmp58F;struct Cyc_Absyn_Tqual _tmp590;void*_tmp591;struct _tuple11*
_tmp58E=tqt2;_tmp58F=*_tmp58E;_tmp590=_tmp58F.f1;_tmp591=_tmp58F.f2;{int _tmp592=
Cyc_Tcutil_tqual_cmp(_tmp58C,_tmp590);if(_tmp592 != 0)return _tmp592;return Cyc_Tcutil_typecmp(
_tmp58D,_tmp591);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2);static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp593=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp593 != 0)return _tmp593;{int _tmp594=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp594 != 0)return _tmp594;{int _tmp595=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp595 != 0)return _tmp595;{int _tmp596=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp596 != 0)return _tmp596;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2);static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp597=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp597 != 0)return _tmp597;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t);static int Cyc_Tcutil_type_case_number(void*t){void*_tmp598=t;_LL2DB: {
struct Cyc_Absyn_VoidType_struct*_tmp599=(struct Cyc_Absyn_VoidType_struct*)
_tmp598;if(_tmp599->tag != 0)goto _LL2DD;}_LL2DC: return 0;_LL2DD: {struct Cyc_Absyn_Evar_struct*
_tmp59A=(struct Cyc_Absyn_Evar_struct*)_tmp598;if(_tmp59A->tag != 1)goto _LL2DF;}
_LL2DE: return 1;_LL2DF: {struct Cyc_Absyn_VarType_struct*_tmp59B=(struct Cyc_Absyn_VarType_struct*)
_tmp598;if(_tmp59B->tag != 2)goto _LL2E1;}_LL2E0: return 2;_LL2E1: {struct Cyc_Absyn_DatatypeType_struct*
_tmp59C=(struct Cyc_Absyn_DatatypeType_struct*)_tmp598;if(_tmp59C->tag != 3)goto
_LL2E3;}_LL2E2: return 3;_LL2E3: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp59D=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp598;if(_tmp59D->tag != 4)goto _LL2E5;}
_LL2E4: return 4;_LL2E5: {struct Cyc_Absyn_PointerType_struct*_tmp59E=(struct Cyc_Absyn_PointerType_struct*)
_tmp598;if(_tmp59E->tag != 5)goto _LL2E7;}_LL2E6: return 5;_LL2E7: {struct Cyc_Absyn_IntType_struct*
_tmp59F=(struct Cyc_Absyn_IntType_struct*)_tmp598;if(_tmp59F->tag != 6)goto _LL2E9;}
_LL2E8: return 6;_LL2E9: {struct Cyc_Absyn_FloatType_struct*_tmp5A0=(struct Cyc_Absyn_FloatType_struct*)
_tmp598;if(_tmp5A0->tag != 7)goto _LL2EB;}_LL2EA: return 7;_LL2EB: {struct Cyc_Absyn_DoubleType_struct*
_tmp5A1=(struct Cyc_Absyn_DoubleType_struct*)_tmp598;if(_tmp5A1->tag != 8)goto
_LL2ED;}_LL2EC: return 8;_LL2ED: {struct Cyc_Absyn_ArrayType_struct*_tmp5A2=(struct
Cyc_Absyn_ArrayType_struct*)_tmp598;if(_tmp5A2->tag != 9)goto _LL2EF;}_LL2EE:
return 9;_LL2EF: {struct Cyc_Absyn_FnType_struct*_tmp5A3=(struct Cyc_Absyn_FnType_struct*)
_tmp598;if(_tmp5A3->tag != 10)goto _LL2F1;}_LL2F0: return 10;_LL2F1: {struct Cyc_Absyn_TupleType_struct*
_tmp5A4=(struct Cyc_Absyn_TupleType_struct*)_tmp598;if(_tmp5A4->tag != 11)goto
_LL2F3;}_LL2F2: return 11;_LL2F3: {struct Cyc_Absyn_AggrType_struct*_tmp5A5=(struct
Cyc_Absyn_AggrType_struct*)_tmp598;if(_tmp5A5->tag != 12)goto _LL2F5;}_LL2F4:
return 12;_LL2F5: {struct Cyc_Absyn_AnonAggrType_struct*_tmp5A6=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp598;if(_tmp5A6->tag != 13)goto _LL2F7;}_LL2F6: return 14;_LL2F7: {struct Cyc_Absyn_EnumType_struct*
_tmp5A7=(struct Cyc_Absyn_EnumType_struct*)_tmp598;if(_tmp5A7->tag != 14)goto
_LL2F9;}_LL2F8: return 16;_LL2F9: {struct Cyc_Absyn_AnonEnumType_struct*_tmp5A8=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp598;if(_tmp5A8->tag != 15)goto _LL2FB;}
_LL2FA: return 17;_LL2FB: {struct Cyc_Absyn_RgnHandleType_struct*_tmp5A9=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp598;if(_tmp5A9->tag != 16)goto _LL2FD;}_LL2FC: return 18;_LL2FD: {struct Cyc_Absyn_TypedefType_struct*
_tmp5AA=(struct Cyc_Absyn_TypedefType_struct*)_tmp598;if(_tmp5AA->tag != 18)goto
_LL2FF;}_LL2FE: return 19;_LL2FF: {struct Cyc_Absyn_UniqueRgn_struct*_tmp5AB=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp598;if(_tmp5AB->tag != 22)goto _LL301;}
_LL300: return 20;_LL301: {struct Cyc_Absyn_HeapRgn_struct*_tmp5AC=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp598;if(_tmp5AC->tag != 21)goto _LL303;}_LL302: return 21;_LL303: {struct Cyc_Absyn_AccessEff_struct*
_tmp5AD=(struct Cyc_Absyn_AccessEff_struct*)_tmp598;if(_tmp5AD->tag != 23)goto
_LL305;}_LL304: return 22;_LL305: {struct Cyc_Absyn_JoinEff_struct*_tmp5AE=(struct
Cyc_Absyn_JoinEff_struct*)_tmp598;if(_tmp5AE->tag != 24)goto _LL307;}_LL306: return
23;_LL307: {struct Cyc_Absyn_RgnsEff_struct*_tmp5AF=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp598;if(_tmp5AF->tag != 25)goto _LL309;}_LL308: return 24;_LL309: {struct Cyc_Absyn_TagType_struct*
_tmp5B0=(struct Cyc_Absyn_TagType_struct*)_tmp598;if(_tmp5B0->tag != 20)goto _LL30B;}
_LL30A: return 26;_LL30B: {struct Cyc_Absyn_DynRgnType_struct*_tmp5B1=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp598;if(_tmp5B1->tag != 17)goto _LL30D;}_LL30C: return 27;_LL30D: {struct Cyc_Absyn_ValueofType_struct*
_tmp5B2=(struct Cyc_Absyn_ValueofType_struct*)_tmp598;if(_tmp5B2->tag != 19)goto
_LL2DA;}_LL30E: return 28;_LL2DA:;}int Cyc_Tcutil_typecmp(void*t1,void*t2);int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp5B3=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp5B3 != 0)return _tmp5B3;{struct _tuple0 _tmp11F2;struct _tuple0 _tmp5B5=(
_tmp11F2.f1=t2,((_tmp11F2.f2=t1,_tmp11F2)));void*_tmp5B6;void*_tmp5B8;void*
_tmp5BA;struct Cyc_Absyn_Tvar*_tmp5BC;void*_tmp5BD;struct Cyc_Absyn_Tvar*_tmp5BF;
void*_tmp5C0;struct Cyc_Absyn_AggrInfo _tmp5C2;union Cyc_Absyn_AggrInfoU _tmp5C3;
struct Cyc_List_List*_tmp5C4;void*_tmp5C5;struct Cyc_Absyn_AggrInfo _tmp5C7;union
Cyc_Absyn_AggrInfoU _tmp5C8;struct Cyc_List_List*_tmp5C9;void*_tmp5CA;struct
_tuple2*_tmp5CC;void*_tmp5CD;struct _tuple2*_tmp5CF;void*_tmp5D0;struct Cyc_List_List*
_tmp5D2;void*_tmp5D3;struct Cyc_List_List*_tmp5D5;void*_tmp5D6;struct Cyc_Absyn_DatatypeInfo
_tmp5D8;union Cyc_Absyn_DatatypeInfoU _tmp5D9;struct Cyc_Absyn_Datatypedecl**
_tmp5DA;struct Cyc_Absyn_Datatypedecl*_tmp5DB;struct Cyc_List_List*_tmp5DC;void*
_tmp5DD;struct Cyc_Absyn_DatatypeInfo _tmp5DF;union Cyc_Absyn_DatatypeInfoU _tmp5E0;
struct Cyc_Absyn_Datatypedecl**_tmp5E1;struct Cyc_Absyn_Datatypedecl*_tmp5E2;
struct Cyc_List_List*_tmp5E3;void*_tmp5E4;struct Cyc_Absyn_DatatypeFieldInfo
_tmp5E6;union Cyc_Absyn_DatatypeFieldInfoU _tmp5E7;struct _tuple3 _tmp5E8;struct Cyc_Absyn_Datatypedecl*
_tmp5E9;struct Cyc_Absyn_Datatypefield*_tmp5EA;struct Cyc_List_List*_tmp5EB;void*
_tmp5EC;struct Cyc_Absyn_DatatypeFieldInfo _tmp5EE;union Cyc_Absyn_DatatypeFieldInfoU
_tmp5EF;struct _tuple3 _tmp5F0;struct Cyc_Absyn_Datatypedecl*_tmp5F1;struct Cyc_Absyn_Datatypefield*
_tmp5F2;struct Cyc_List_List*_tmp5F3;void*_tmp5F4;struct Cyc_Absyn_PtrInfo _tmp5F6;
void*_tmp5F7;struct Cyc_Absyn_Tqual _tmp5F8;struct Cyc_Absyn_PtrAtts _tmp5F9;void*
_tmp5FA;union Cyc_Absyn_Constraint*_tmp5FB;union Cyc_Absyn_Constraint*_tmp5FC;
union Cyc_Absyn_Constraint*_tmp5FD;void*_tmp5FE;struct Cyc_Absyn_PtrInfo _tmp600;
void*_tmp601;struct Cyc_Absyn_Tqual _tmp602;struct Cyc_Absyn_PtrAtts _tmp603;void*
_tmp604;union Cyc_Absyn_Constraint*_tmp605;union Cyc_Absyn_Constraint*_tmp606;
union Cyc_Absyn_Constraint*_tmp607;void*_tmp608;enum Cyc_Absyn_Sign _tmp60A;enum 
Cyc_Absyn_Size_of _tmp60B;void*_tmp60C;enum Cyc_Absyn_Sign _tmp60E;enum Cyc_Absyn_Size_of
_tmp60F;void*_tmp610;void*_tmp612;void*_tmp614;int _tmp616;void*_tmp617;int
_tmp619;void*_tmp61A;struct Cyc_Absyn_ArrayInfo _tmp61C;void*_tmp61D;struct Cyc_Absyn_Tqual
_tmp61E;struct Cyc_Absyn_Exp*_tmp61F;union Cyc_Absyn_Constraint*_tmp620;void*
_tmp621;struct Cyc_Absyn_ArrayInfo _tmp623;void*_tmp624;struct Cyc_Absyn_Tqual
_tmp625;struct Cyc_Absyn_Exp*_tmp626;union Cyc_Absyn_Constraint*_tmp627;void*
_tmp628;struct Cyc_Absyn_FnInfo _tmp62A;struct Cyc_List_List*_tmp62B;struct Cyc_Core_Opt*
_tmp62C;void*_tmp62D;struct Cyc_List_List*_tmp62E;int _tmp62F;struct Cyc_Absyn_VarargInfo*
_tmp630;struct Cyc_List_List*_tmp631;struct Cyc_List_List*_tmp632;void*_tmp633;
struct Cyc_Absyn_FnInfo _tmp635;struct Cyc_List_List*_tmp636;struct Cyc_Core_Opt*
_tmp637;void*_tmp638;struct Cyc_List_List*_tmp639;int _tmp63A;struct Cyc_Absyn_VarargInfo*
_tmp63B;struct Cyc_List_List*_tmp63C;struct Cyc_List_List*_tmp63D;void*_tmp63E;
struct Cyc_List_List*_tmp640;void*_tmp641;struct Cyc_List_List*_tmp643;void*
_tmp644;enum Cyc_Absyn_AggrKind _tmp646;struct Cyc_List_List*_tmp647;void*_tmp648;
enum Cyc_Absyn_AggrKind _tmp64A;struct Cyc_List_List*_tmp64B;void*_tmp64C;void*
_tmp64E;void*_tmp64F;void*_tmp651;void*_tmp652;void*_tmp654;void*_tmp655;void*
_tmp656;void*_tmp658;void*_tmp659;void*_tmp65A;void*_tmp65C;void*_tmp65D;void*
_tmp65F;void*_tmp660;struct Cyc_Absyn_Exp*_tmp662;void*_tmp663;struct Cyc_Absyn_Exp*
_tmp665;void*_tmp666;void*_tmp668;void*_tmp66A;void*_tmp66C;void*_tmp66E;void*
_tmp670;_LL310: _tmp5B6=_tmp5B5.f1;{struct Cyc_Absyn_Evar_struct*_tmp5B7=(struct
Cyc_Absyn_Evar_struct*)_tmp5B6;if(_tmp5B7->tag != 1)goto _LL312;};_tmp5B8=_tmp5B5.f2;{
struct Cyc_Absyn_Evar_struct*_tmp5B9=(struct Cyc_Absyn_Evar_struct*)_tmp5B8;if(
_tmp5B9->tag != 1)goto _LL312;};_LL311: {const char*_tmp11F5;void*_tmp11F4;(
_tmp11F4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp11F5="typecmp: can only compare closed types",_tag_dyneither(_tmp11F5,
sizeof(char),39))),_tag_dyneither(_tmp11F4,sizeof(void*),0)));}_LL312: _tmp5BA=
_tmp5B5.f1;{struct Cyc_Absyn_VarType_struct*_tmp5BB=(struct Cyc_Absyn_VarType_struct*)
_tmp5BA;if(_tmp5BB->tag != 2)goto _LL314;else{_tmp5BC=_tmp5BB->f1;}};_tmp5BD=
_tmp5B5.f2;{struct Cyc_Absyn_VarType_struct*_tmp5BE=(struct Cyc_Absyn_VarType_struct*)
_tmp5BD;if(_tmp5BE->tag != 2)goto _LL314;else{_tmp5BF=_tmp5BE->f1;}};_LL313: return
Cyc_Core_intcmp(_tmp5BF->identity,_tmp5BC->identity);_LL314: _tmp5C0=_tmp5B5.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp5C1=(struct Cyc_Absyn_AggrType_struct*)
_tmp5C0;if(_tmp5C1->tag != 12)goto _LL316;else{_tmp5C2=_tmp5C1->f1;_tmp5C3=_tmp5C2.aggr_info;
_tmp5C4=_tmp5C2.targs;}};_tmp5C5=_tmp5B5.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp5C6=(struct Cyc_Absyn_AggrType_struct*)_tmp5C5;if(_tmp5C6->tag != 12)goto
_LL316;else{_tmp5C7=_tmp5C6->f1;_tmp5C8=_tmp5C7.aggr_info;_tmp5C9=_tmp5C7.targs;}};
_LL315: {struct _tuple2*_tmp675;struct _tuple12 _tmp674=Cyc_Absyn_aggr_kinded_name(
_tmp5C3);_tmp675=_tmp674.f2;{struct _tuple2*_tmp677;struct _tuple12 _tmp676=Cyc_Absyn_aggr_kinded_name(
_tmp5C8);_tmp677=_tmp676.f2;{int _tmp678=Cyc_Absyn_qvar_cmp(_tmp675,_tmp677);if(
_tmp678 != 0)return _tmp678;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp5C4,_tmp5C9);}};};}
_LL316: _tmp5CA=_tmp5B5.f1;{struct Cyc_Absyn_EnumType_struct*_tmp5CB=(struct Cyc_Absyn_EnumType_struct*)
_tmp5CA;if(_tmp5CB->tag != 14)goto _LL318;else{_tmp5CC=_tmp5CB->f1;}};_tmp5CD=
_tmp5B5.f2;{struct Cyc_Absyn_EnumType_struct*_tmp5CE=(struct Cyc_Absyn_EnumType_struct*)
_tmp5CD;if(_tmp5CE->tag != 14)goto _LL318;else{_tmp5CF=_tmp5CE->f1;}};_LL317:
return Cyc_Absyn_qvar_cmp(_tmp5CC,_tmp5CF);_LL318: _tmp5D0=_tmp5B5.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp5D1=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp5D0;if(_tmp5D1->tag != 15)goto
_LL31A;else{_tmp5D2=_tmp5D1->f1;}};_tmp5D3=_tmp5B5.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp5D4=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp5D3;if(_tmp5D4->tag != 15)goto
_LL31A;else{_tmp5D5=_tmp5D4->f1;}};_LL319: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp5D2,_tmp5D5);_LL31A: _tmp5D6=_tmp5B5.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp5D7=(struct Cyc_Absyn_DatatypeType_struct*)_tmp5D6;if(_tmp5D7->tag != 3)goto
_LL31C;else{_tmp5D8=_tmp5D7->f1;_tmp5D9=_tmp5D8.datatype_info;if((_tmp5D9.KnownDatatype).tag
!= 2)goto _LL31C;_tmp5DA=(struct Cyc_Absyn_Datatypedecl**)(_tmp5D9.KnownDatatype).val;
_tmp5DB=*_tmp5DA;_tmp5DC=_tmp5D8.targs;}};_tmp5DD=_tmp5B5.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp5DE=(struct Cyc_Absyn_DatatypeType_struct*)_tmp5DD;if(_tmp5DE->tag != 3)goto
_LL31C;else{_tmp5DF=_tmp5DE->f1;_tmp5E0=_tmp5DF.datatype_info;if((_tmp5E0.KnownDatatype).tag
!= 2)goto _LL31C;_tmp5E1=(struct Cyc_Absyn_Datatypedecl**)(_tmp5E0.KnownDatatype).val;
_tmp5E2=*_tmp5E1;_tmp5E3=_tmp5DF.targs;}};_LL31B: if(_tmp5E2 == _tmp5DB)return 0;{
int _tmp679=Cyc_Absyn_qvar_cmp(_tmp5E2->name,_tmp5DB->name);if(_tmp679 != 0)return
_tmp679;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp5E3,_tmp5DC);};_LL31C: _tmp5E4=
_tmp5B5.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp5E5=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp5E4;if(_tmp5E5->tag != 4)goto _LL31E;else{_tmp5E6=_tmp5E5->f1;_tmp5E7=_tmp5E6.field_info;
if((_tmp5E7.KnownDatatypefield).tag != 2)goto _LL31E;_tmp5E8=(struct _tuple3)(
_tmp5E7.KnownDatatypefield).val;_tmp5E9=_tmp5E8.f1;_tmp5EA=_tmp5E8.f2;_tmp5EB=
_tmp5E6.targs;}};_tmp5EC=_tmp5B5.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp5ED=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp5EC;if(_tmp5ED->tag != 4)
goto _LL31E;else{_tmp5EE=_tmp5ED->f1;_tmp5EF=_tmp5EE.field_info;if((_tmp5EF.KnownDatatypefield).tag
!= 2)goto _LL31E;_tmp5F0=(struct _tuple3)(_tmp5EF.KnownDatatypefield).val;_tmp5F1=
_tmp5F0.f1;_tmp5F2=_tmp5F0.f2;_tmp5F3=_tmp5EE.targs;}};_LL31D: if(_tmp5F1 == 
_tmp5E9)return 0;{int _tmp67A=Cyc_Absyn_qvar_cmp(_tmp5E9->name,_tmp5F1->name);if(
_tmp67A != 0)return _tmp67A;{int _tmp67B=Cyc_Absyn_qvar_cmp(_tmp5EA->name,_tmp5F2->name);
if(_tmp67B != 0)return _tmp67B;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp5F3,_tmp5EB);};};
_LL31E: _tmp5F4=_tmp5B5.f1;{struct Cyc_Absyn_PointerType_struct*_tmp5F5=(struct Cyc_Absyn_PointerType_struct*)
_tmp5F4;if(_tmp5F5->tag != 5)goto _LL320;else{_tmp5F6=_tmp5F5->f1;_tmp5F7=_tmp5F6.elt_typ;
_tmp5F8=_tmp5F6.elt_tq;_tmp5F9=_tmp5F6.ptr_atts;_tmp5FA=_tmp5F9.rgn;_tmp5FB=
_tmp5F9.nullable;_tmp5FC=_tmp5F9.bounds;_tmp5FD=_tmp5F9.zero_term;}};_tmp5FE=
_tmp5B5.f2;{struct Cyc_Absyn_PointerType_struct*_tmp5FF=(struct Cyc_Absyn_PointerType_struct*)
_tmp5FE;if(_tmp5FF->tag != 5)goto _LL320;else{_tmp600=_tmp5FF->f1;_tmp601=_tmp600.elt_typ;
_tmp602=_tmp600.elt_tq;_tmp603=_tmp600.ptr_atts;_tmp604=_tmp603.rgn;_tmp605=
_tmp603.nullable;_tmp606=_tmp603.bounds;_tmp607=_tmp603.zero_term;}};_LL31F: {
int _tmp67C=Cyc_Tcutil_typecmp(_tmp601,_tmp5F7);if(_tmp67C != 0)return _tmp67C;{int
_tmp67D=Cyc_Tcutil_typecmp(_tmp604,_tmp5FA);if(_tmp67D != 0)return _tmp67D;{int
_tmp67E=Cyc_Tcutil_tqual_cmp(_tmp602,_tmp5F8);if(_tmp67E != 0)return _tmp67E;{int
_tmp67F=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp606,_tmp5FC);if(_tmp67F != 0)
return _tmp67F;{int _tmp680=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp607,
_tmp5FD);if(_tmp680 != 0)return _tmp680;{void*_tmp681=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp606);_LL345: {struct Cyc_Absyn_DynEither_b_struct*
_tmp682=(struct Cyc_Absyn_DynEither_b_struct*)_tmp681;if(_tmp682->tag != 0)goto
_LL347;}_LL346: return 0;_LL347:;_LL348: goto _LL344;_LL344:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp605,_tmp5FB);};};};};}_LL320: _tmp608=_tmp5B5.f1;{struct Cyc_Absyn_IntType_struct*
_tmp609=(struct Cyc_Absyn_IntType_struct*)_tmp608;if(_tmp609->tag != 6)goto _LL322;
else{_tmp60A=_tmp609->f1;_tmp60B=_tmp609->f2;}};_tmp60C=_tmp5B5.f2;{struct Cyc_Absyn_IntType_struct*
_tmp60D=(struct Cyc_Absyn_IntType_struct*)_tmp60C;if(_tmp60D->tag != 6)goto _LL322;
else{_tmp60E=_tmp60D->f1;_tmp60F=_tmp60D->f2;}};_LL321: if(_tmp60E != _tmp60A)
return Cyc_Core_intcmp((int)((unsigned int)_tmp60E),(int)((unsigned int)_tmp60A));
if(_tmp60F != _tmp60B)return Cyc_Core_intcmp((int)((unsigned int)_tmp60F),(int)((
unsigned int)_tmp60B));return 0;_LL322: _tmp610=_tmp5B5.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp611=(struct Cyc_Absyn_FloatType_struct*)_tmp610;if(_tmp611->tag != 7)goto
_LL324;};_tmp612=_tmp5B5.f2;{struct Cyc_Absyn_FloatType_struct*_tmp613=(struct Cyc_Absyn_FloatType_struct*)
_tmp612;if(_tmp613->tag != 7)goto _LL324;};_LL323: return 0;_LL324: _tmp614=_tmp5B5.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp615=(struct Cyc_Absyn_DoubleType_struct*)
_tmp614;if(_tmp615->tag != 8)goto _LL326;else{_tmp616=_tmp615->f1;}};_tmp617=
_tmp5B5.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp618=(struct Cyc_Absyn_DoubleType_struct*)
_tmp617;if(_tmp618->tag != 8)goto _LL326;else{_tmp619=_tmp618->f1;}};_LL325: if(
_tmp616 == _tmp619)return 0;else{if(_tmp616)return - 1;else{return 1;}}_LL326: _tmp61A=
_tmp5B5.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp61B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp61A;if(_tmp61B->tag != 9)goto _LL328;else{_tmp61C=_tmp61B->f1;_tmp61D=_tmp61C.elt_type;
_tmp61E=_tmp61C.tq;_tmp61F=_tmp61C.num_elts;_tmp620=_tmp61C.zero_term;}};_tmp621=
_tmp5B5.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp622=(struct Cyc_Absyn_ArrayType_struct*)
_tmp621;if(_tmp622->tag != 9)goto _LL328;else{_tmp623=_tmp622->f1;_tmp624=_tmp623.elt_type;
_tmp625=_tmp623.tq;_tmp626=_tmp623.num_elts;_tmp627=_tmp623.zero_term;}};_LL327: {
int _tmp683=Cyc_Tcutil_tqual_cmp(_tmp625,_tmp61E);if(_tmp683 != 0)return _tmp683;{
int _tmp684=Cyc_Tcutil_typecmp(_tmp624,_tmp61D);if(_tmp684 != 0)return _tmp684;{int
_tmp685=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp620,_tmp627);if(_tmp685 != 0)
return _tmp685;if(_tmp61F == _tmp626)return 0;if(_tmp61F == 0  || _tmp626 == 0){const
char*_tmp11F8;void*_tmp11F7;(_tmp11F7=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11F8="missing expression in array index",
_tag_dyneither(_tmp11F8,sizeof(char),34))),_tag_dyneither(_tmp11F7,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp61F,_tmp626);};};}_LL328: _tmp628=_tmp5B5.f1;{struct Cyc_Absyn_FnType_struct*
_tmp629=(struct Cyc_Absyn_FnType_struct*)_tmp628;if(_tmp629->tag != 10)goto _LL32A;
else{_tmp62A=_tmp629->f1;_tmp62B=_tmp62A.tvars;_tmp62C=_tmp62A.effect;_tmp62D=
_tmp62A.ret_typ;_tmp62E=_tmp62A.args;_tmp62F=_tmp62A.c_varargs;_tmp630=_tmp62A.cyc_varargs;
_tmp631=_tmp62A.rgn_po;_tmp632=_tmp62A.attributes;}};_tmp633=_tmp5B5.f2;{struct
Cyc_Absyn_FnType_struct*_tmp634=(struct Cyc_Absyn_FnType_struct*)_tmp633;if(
_tmp634->tag != 10)goto _LL32A;else{_tmp635=_tmp634->f1;_tmp636=_tmp635.tvars;
_tmp637=_tmp635.effect;_tmp638=_tmp635.ret_typ;_tmp639=_tmp635.args;_tmp63A=
_tmp635.c_varargs;_tmp63B=_tmp635.cyc_varargs;_tmp63C=_tmp635.rgn_po;_tmp63D=
_tmp635.attributes;}};_LL329: {const char*_tmp11FB;void*_tmp11FA;(_tmp11FA=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp11FB="typecmp: function types not handled",_tag_dyneither(_tmp11FB,sizeof(
char),36))),_tag_dyneither(_tmp11FA,sizeof(void*),0)));}_LL32A: _tmp63E=_tmp5B5.f1;{
struct Cyc_Absyn_TupleType_struct*_tmp63F=(struct Cyc_Absyn_TupleType_struct*)
_tmp63E;if(_tmp63F->tag != 11)goto _LL32C;else{_tmp640=_tmp63F->f1;}};_tmp641=
_tmp5B5.f2;{struct Cyc_Absyn_TupleType_struct*_tmp642=(struct Cyc_Absyn_TupleType_struct*)
_tmp641;if(_tmp642->tag != 11)goto _LL32C;else{_tmp643=_tmp642->f1;}};_LL32B:
return((int(*)(int(*cmp)(struct _tuple11*,struct _tuple11*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp643,
_tmp640);_LL32C: _tmp644=_tmp5B5.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmp645=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp644;if(_tmp645->tag != 13)goto _LL32E;
else{_tmp646=_tmp645->f1;_tmp647=_tmp645->f2;}};_tmp648=_tmp5B5.f2;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp649=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp648;if(_tmp649->tag != 13)goto
_LL32E;else{_tmp64A=_tmp649->f1;_tmp64B=_tmp649->f2;}};_LL32D: if(_tmp64A != 
_tmp646){if(_tmp64A == Cyc_Absyn_StructA)return - 1;else{return 1;}}return((int(*)(
int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp64B,
_tmp647);_LL32E: _tmp64C=_tmp5B5.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp64D=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp64C;if(_tmp64D->tag != 16)goto _LL330;
else{_tmp64E=(void*)_tmp64D->f1;}};_tmp64F=_tmp5B5.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp650=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp64F;if(_tmp650->tag != 16)goto
_LL330;else{_tmp651=(void*)_tmp650->f1;}};_LL32F: return Cyc_Tcutil_typecmp(
_tmp64E,_tmp651);_LL330: _tmp652=_tmp5B5.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp653=(struct Cyc_Absyn_DynRgnType_struct*)_tmp652;if(_tmp653->tag != 17)goto
_LL332;else{_tmp654=(void*)_tmp653->f1;_tmp655=(void*)_tmp653->f2;}};_tmp656=
_tmp5B5.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp657=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp656;if(_tmp657->tag != 17)goto _LL332;else{_tmp658=(void*)_tmp657->f1;_tmp659=(
void*)_tmp657->f2;}};_LL331: {int _tmp68A=Cyc_Tcutil_typecmp(_tmp654,_tmp658);if(
_tmp68A != 0)return _tmp68A;else{return Cyc_Tcutil_typecmp(_tmp655,_tmp659);}}
_LL332: _tmp65A=_tmp5B5.f1;{struct Cyc_Absyn_TagType_struct*_tmp65B=(struct Cyc_Absyn_TagType_struct*)
_tmp65A;if(_tmp65B->tag != 20)goto _LL334;else{_tmp65C=(void*)_tmp65B->f1;}};
_tmp65D=_tmp5B5.f2;{struct Cyc_Absyn_TagType_struct*_tmp65E=(struct Cyc_Absyn_TagType_struct*)
_tmp65D;if(_tmp65E->tag != 20)goto _LL334;else{_tmp65F=(void*)_tmp65E->f1;}};
_LL333: return Cyc_Tcutil_typecmp(_tmp65C,_tmp65F);_LL334: _tmp660=_tmp5B5.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp661=(struct Cyc_Absyn_ValueofType_struct*)
_tmp660;if(_tmp661->tag != 19)goto _LL336;else{_tmp662=_tmp661->f1;}};_tmp663=
_tmp5B5.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp664=(struct Cyc_Absyn_ValueofType_struct*)
_tmp663;if(_tmp664->tag != 19)goto _LL336;else{_tmp665=_tmp664->f1;}};_LL335:
return Cyc_Evexp_const_exp_cmp(_tmp662,_tmp665);_LL336: _tmp666=_tmp5B5.f1;{struct
Cyc_Absyn_JoinEff_struct*_tmp667=(struct Cyc_Absyn_JoinEff_struct*)_tmp666;if(
_tmp667->tag != 24)goto _LL338;};_LL337: goto _LL339;_LL338: _tmp668=_tmp5B5.f2;{
struct Cyc_Absyn_JoinEff_struct*_tmp669=(struct Cyc_Absyn_JoinEff_struct*)_tmp668;
if(_tmp669->tag != 24)goto _LL33A;};_LL339: goto _LL33B;_LL33A: _tmp66A=_tmp5B5.f1;{
struct Cyc_Absyn_AccessEff_struct*_tmp66B=(struct Cyc_Absyn_AccessEff_struct*)
_tmp66A;if(_tmp66B->tag != 23)goto _LL33C;};_LL33B: goto _LL33D;_LL33C: _tmp66C=
_tmp5B5.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp66D=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp66C;if(_tmp66D->tag != 25)goto _LL33E;};_LL33D: goto _LL33F;_LL33E: _tmp66E=
_tmp5B5.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp66F=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp66E;if(_tmp66F->tag != 25)goto _LL340;};_LL33F: goto _LL341;_LL340: _tmp670=
_tmp5B5.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp671=(struct Cyc_Absyn_AccessEff_struct*)
_tmp670;if(_tmp671->tag != 23)goto _LL342;};_LL341: {const char*_tmp11FE;void*
_tmp11FD;(_tmp11FD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11FE="typecmp: effects not handled",_tag_dyneither(
_tmp11FE,sizeof(char),29))),_tag_dyneither(_tmp11FD,sizeof(void*),0)));}_LL342:;
_LL343: {const char*_tmp1201;void*_tmp1200;(_tmp1200=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1201="Unmatched case in typecmp",
_tag_dyneither(_tmp1201,sizeof(char),26))),_tag_dyneither(_tmp1200,sizeof(void*),
0)));}_LL30F:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t);int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp68F=Cyc_Tcutil_compress(t);_LL34A: {struct Cyc_Absyn_IntType_struct*
_tmp690=(struct Cyc_Absyn_IntType_struct*)_tmp68F;if(_tmp690->tag != 6)goto _LL34C;}
_LL34B: goto _LL34D;_LL34C: {struct Cyc_Absyn_FloatType_struct*_tmp691=(struct Cyc_Absyn_FloatType_struct*)
_tmp68F;if(_tmp691->tag != 7)goto _LL34E;}_LL34D: goto _LL34F;_LL34E: {struct Cyc_Absyn_DoubleType_struct*
_tmp692=(struct Cyc_Absyn_DoubleType_struct*)_tmp68F;if(_tmp692->tag != 8)goto
_LL350;}_LL34F: goto _LL351;_LL350: {struct Cyc_Absyn_EnumType_struct*_tmp693=(
struct Cyc_Absyn_EnumType_struct*)_tmp68F;if(_tmp693->tag != 14)goto _LL352;}_LL351:
goto _LL353;_LL352: {struct Cyc_Absyn_AnonEnumType_struct*_tmp694=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp68F;if(_tmp694->tag != 15)goto _LL354;}_LL353: return 1;_LL354:;_LL355: return 0;
_LL349:;}int Cyc_Tcutil_will_lose_precision(void*t1,void*t2);int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp1202;struct _tuple0 _tmp696=(_tmp1202.f1=t1,((_tmp1202.f2=t2,_tmp1202)));
void*_tmp697;int _tmp699;void*_tmp69A;int _tmp69C;void*_tmp69D;void*_tmp69F;void*
_tmp6A1;void*_tmp6A3;void*_tmp6A5;void*_tmp6A7;void*_tmp6A9;void*_tmp6AB;void*
_tmp6AD;void*_tmp6AF;void*_tmp6B1;enum Cyc_Absyn_Size_of _tmp6B3;void*_tmp6B4;
enum Cyc_Absyn_Size_of _tmp6B6;void*_tmp6B7;enum Cyc_Absyn_Size_of _tmp6B9;void*
_tmp6BA;enum Cyc_Absyn_Size_of _tmp6BC;void*_tmp6BD;enum Cyc_Absyn_Size_of _tmp6BF;
void*_tmp6C0;enum Cyc_Absyn_Size_of _tmp6C2;void*_tmp6C3;enum Cyc_Absyn_Size_of
_tmp6C5;void*_tmp6C6;enum Cyc_Absyn_Size_of _tmp6C8;void*_tmp6C9;void*_tmp6CB;
enum Cyc_Absyn_Size_of _tmp6CD;void*_tmp6CE;void*_tmp6D0;enum Cyc_Absyn_Size_of
_tmp6D2;void*_tmp6D3;enum Cyc_Absyn_Size_of _tmp6D5;void*_tmp6D6;enum Cyc_Absyn_Size_of
_tmp6D8;void*_tmp6D9;enum Cyc_Absyn_Size_of _tmp6DB;void*_tmp6DC;void*_tmp6DE;
enum Cyc_Absyn_Size_of _tmp6E0;void*_tmp6E1;enum Cyc_Absyn_Size_of _tmp6E3;void*
_tmp6E4;enum Cyc_Absyn_Size_of _tmp6E6;void*_tmp6E7;enum Cyc_Absyn_Size_of _tmp6E9;
void*_tmp6EA;enum Cyc_Absyn_Size_of _tmp6EC;void*_tmp6ED;enum Cyc_Absyn_Size_of
_tmp6EF;void*_tmp6F0;enum Cyc_Absyn_Size_of _tmp6F2;void*_tmp6F3;void*_tmp6F5;
enum Cyc_Absyn_Size_of _tmp6F7;_LL357: _tmp697=_tmp696.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp698=(struct Cyc_Absyn_DoubleType_struct*)_tmp697;if(_tmp698->tag != 8)goto
_LL359;else{_tmp699=_tmp698->f1;}};_tmp69A=_tmp696.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp69B=(struct Cyc_Absyn_DoubleType_struct*)_tmp69A;if(_tmp69B->tag != 8)goto
_LL359;else{_tmp69C=_tmp69B->f1;}};_LL358: return !_tmp69C  && _tmp699;_LL359:
_tmp69D=_tmp696.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp69E=(struct Cyc_Absyn_DoubleType_struct*)
_tmp69D;if(_tmp69E->tag != 8)goto _LL35B;};_tmp69F=_tmp696.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp6A0=(struct Cyc_Absyn_FloatType_struct*)_tmp69F;if(_tmp6A0->tag != 7)goto
_LL35B;};_LL35A: goto _LL35C;_LL35B: _tmp6A1=_tmp696.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp6A2=(struct Cyc_Absyn_DoubleType_struct*)_tmp6A1;if(_tmp6A2->tag != 8)goto
_LL35D;};_tmp6A3=_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6A4=(struct Cyc_Absyn_IntType_struct*)
_tmp6A3;if(_tmp6A4->tag != 6)goto _LL35D;};_LL35C: goto _LL35E;_LL35D: _tmp6A5=
_tmp696.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp6A6=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6A5;if(_tmp6A6->tag != 8)goto _LL35F;};_tmp6A7=_tmp696.f2;{struct Cyc_Absyn_TagType_struct*
_tmp6A8=(struct Cyc_Absyn_TagType_struct*)_tmp6A7;if(_tmp6A8->tag != 20)goto _LL35F;};
_LL35E: goto _LL360;_LL35F: _tmp6A9=_tmp696.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp6AA=(struct Cyc_Absyn_FloatType_struct*)_tmp6A9;if(_tmp6AA->tag != 7)goto
_LL361;};_tmp6AB=_tmp696.f2;{struct Cyc_Absyn_TagType_struct*_tmp6AC=(struct Cyc_Absyn_TagType_struct*)
_tmp6AB;if(_tmp6AC->tag != 20)goto _LL361;};_LL360: goto _LL362;_LL361: _tmp6AD=
_tmp696.f1;{struct Cyc_Absyn_FloatType_struct*_tmp6AE=(struct Cyc_Absyn_FloatType_struct*)
_tmp6AD;if(_tmp6AE->tag != 7)goto _LL363;};_tmp6AF=_tmp696.f2;{struct Cyc_Absyn_IntType_struct*
_tmp6B0=(struct Cyc_Absyn_IntType_struct*)_tmp6AF;if(_tmp6B0->tag != 6)goto _LL363;};
_LL362: return 1;_LL363: _tmp6B1=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*_tmp6B2=(
struct Cyc_Absyn_IntType_struct*)_tmp6B1;if(_tmp6B2->tag != 6)goto _LL365;else{
_tmp6B3=_tmp6B2->f2;if(_tmp6B3 != Cyc_Absyn_LongLong_sz)goto _LL365;}};_tmp6B4=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6B5=(struct Cyc_Absyn_IntType_struct*)
_tmp6B4;if(_tmp6B5->tag != 6)goto _LL365;else{_tmp6B6=_tmp6B5->f2;if(_tmp6B6 != Cyc_Absyn_LongLong_sz)
goto _LL365;}};_LL364: return 0;_LL365: _tmp6B7=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6B8=(struct Cyc_Absyn_IntType_struct*)_tmp6B7;if(_tmp6B8->tag != 6)goto _LL367;
else{_tmp6B9=_tmp6B8->f2;if(_tmp6B9 != Cyc_Absyn_LongLong_sz)goto _LL367;}};_LL366:
return 1;_LL367: _tmp6BA=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*_tmp6BB=(
struct Cyc_Absyn_IntType_struct*)_tmp6BA;if(_tmp6BB->tag != 6)goto _LL369;else{
_tmp6BC=_tmp6BB->f2;if(_tmp6BC != Cyc_Absyn_Long_sz)goto _LL369;}};_tmp6BD=_tmp696.f2;{
struct Cyc_Absyn_IntType_struct*_tmp6BE=(struct Cyc_Absyn_IntType_struct*)_tmp6BD;
if(_tmp6BE->tag != 6)goto _LL369;else{_tmp6BF=_tmp6BE->f2;if(_tmp6BF != Cyc_Absyn_Int_sz)
goto _LL369;}};_LL368: goto _LL36A;_LL369: _tmp6C0=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6C1=(struct Cyc_Absyn_IntType_struct*)_tmp6C0;if(_tmp6C1->tag != 6)goto _LL36B;
else{_tmp6C2=_tmp6C1->f2;if(_tmp6C2 != Cyc_Absyn_Int_sz)goto _LL36B;}};_tmp6C3=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6C4=(struct Cyc_Absyn_IntType_struct*)
_tmp6C3;if(_tmp6C4->tag != 6)goto _LL36B;else{_tmp6C5=_tmp6C4->f2;if(_tmp6C5 != Cyc_Absyn_Long_sz)
goto _LL36B;}};_LL36A: return 0;_LL36B: _tmp6C6=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6C7=(struct Cyc_Absyn_IntType_struct*)_tmp6C6;if(_tmp6C7->tag != 6)goto _LL36D;
else{_tmp6C8=_tmp6C7->f2;if(_tmp6C8 != Cyc_Absyn_Long_sz)goto _LL36D;}};_tmp6C9=
_tmp696.f2;{struct Cyc_Absyn_FloatType_struct*_tmp6CA=(struct Cyc_Absyn_FloatType_struct*)
_tmp6C9;if(_tmp6CA->tag != 7)goto _LL36D;};_LL36C: goto _LL36E;_LL36D: _tmp6CB=
_tmp696.f1;{struct Cyc_Absyn_IntType_struct*_tmp6CC=(struct Cyc_Absyn_IntType_struct*)
_tmp6CB;if(_tmp6CC->tag != 6)goto _LL36F;else{_tmp6CD=_tmp6CC->f2;if(_tmp6CD != Cyc_Absyn_Int_sz)
goto _LL36F;}};_tmp6CE=_tmp696.f2;{struct Cyc_Absyn_FloatType_struct*_tmp6CF=(
struct Cyc_Absyn_FloatType_struct*)_tmp6CE;if(_tmp6CF->tag != 7)goto _LL36F;};
_LL36E: goto _LL370;_LL36F: _tmp6D0=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6D1=(struct Cyc_Absyn_IntType_struct*)_tmp6D0;if(_tmp6D1->tag != 6)goto _LL371;
else{_tmp6D2=_tmp6D1->f2;if(_tmp6D2 != Cyc_Absyn_Long_sz)goto _LL371;}};_tmp6D3=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6D4=(struct Cyc_Absyn_IntType_struct*)
_tmp6D3;if(_tmp6D4->tag != 6)goto _LL371;else{_tmp6D5=_tmp6D4->f2;if(_tmp6D5 != Cyc_Absyn_Short_sz)
goto _LL371;}};_LL370: goto _LL372;_LL371: _tmp6D6=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6D7=(struct Cyc_Absyn_IntType_struct*)_tmp6D6;if(_tmp6D7->tag != 6)goto _LL373;
else{_tmp6D8=_tmp6D7->f2;if(_tmp6D8 != Cyc_Absyn_Int_sz)goto _LL373;}};_tmp6D9=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6DA=(struct Cyc_Absyn_IntType_struct*)
_tmp6D9;if(_tmp6DA->tag != 6)goto _LL373;else{_tmp6DB=_tmp6DA->f2;if(_tmp6DB != Cyc_Absyn_Short_sz)
goto _LL373;}};_LL372: goto _LL374;_LL373: _tmp6DC=_tmp696.f1;{struct Cyc_Absyn_TagType_struct*
_tmp6DD=(struct Cyc_Absyn_TagType_struct*)_tmp6DC;if(_tmp6DD->tag != 20)goto _LL375;};
_tmp6DE=_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6DF=(struct Cyc_Absyn_IntType_struct*)
_tmp6DE;if(_tmp6DF->tag != 6)goto _LL375;else{_tmp6E0=_tmp6DF->f2;if(_tmp6E0 != Cyc_Absyn_Short_sz)
goto _LL375;}};_LL374: goto _LL376;_LL375: _tmp6E1=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6E2=(struct Cyc_Absyn_IntType_struct*)_tmp6E1;if(_tmp6E2->tag != 6)goto _LL377;
else{_tmp6E3=_tmp6E2->f2;if(_tmp6E3 != Cyc_Absyn_Long_sz)goto _LL377;}};_tmp6E4=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6E5=(struct Cyc_Absyn_IntType_struct*)
_tmp6E4;if(_tmp6E5->tag != 6)goto _LL377;else{_tmp6E6=_tmp6E5->f2;if(_tmp6E6 != Cyc_Absyn_Char_sz)
goto _LL377;}};_LL376: goto _LL378;_LL377: _tmp6E7=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6E8=(struct Cyc_Absyn_IntType_struct*)_tmp6E7;if(_tmp6E8->tag != 6)goto _LL379;
else{_tmp6E9=_tmp6E8->f2;if(_tmp6E9 != Cyc_Absyn_Int_sz)goto _LL379;}};_tmp6EA=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6EB=(struct Cyc_Absyn_IntType_struct*)
_tmp6EA;if(_tmp6EB->tag != 6)goto _LL379;else{_tmp6EC=_tmp6EB->f2;if(_tmp6EC != Cyc_Absyn_Char_sz)
goto _LL379;}};_LL378: goto _LL37A;_LL379: _tmp6ED=_tmp696.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6EE=(struct Cyc_Absyn_IntType_struct*)_tmp6ED;if(_tmp6EE->tag != 6)goto _LL37B;
else{_tmp6EF=_tmp6EE->f2;if(_tmp6EF != Cyc_Absyn_Short_sz)goto _LL37B;}};_tmp6F0=
_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6F1=(struct Cyc_Absyn_IntType_struct*)
_tmp6F0;if(_tmp6F1->tag != 6)goto _LL37B;else{_tmp6F2=_tmp6F1->f2;if(_tmp6F2 != Cyc_Absyn_Char_sz)
goto _LL37B;}};_LL37A: goto _LL37C;_LL37B: _tmp6F3=_tmp696.f1;{struct Cyc_Absyn_TagType_struct*
_tmp6F4=(struct Cyc_Absyn_TagType_struct*)_tmp6F3;if(_tmp6F4->tag != 20)goto _LL37D;};
_tmp6F5=_tmp696.f2;{struct Cyc_Absyn_IntType_struct*_tmp6F6=(struct Cyc_Absyn_IntType_struct*)
_tmp6F5;if(_tmp6F6->tag != 6)goto _LL37D;else{_tmp6F7=_tmp6F6->f2;if(_tmp6F7 != Cyc_Absyn_Char_sz)
goto _LL37D;}};_LL37C: return 1;_LL37D:;_LL37E: return 0;_LL356:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es);int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp6F8=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp1203;max_arith_type=((_tmp1203=
_region_malloc(_tmp6F8,sizeof(*_tmp1203)),((_tmp1203->v=t1,_tmp1203))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp1208;void*_tmp1207[2];struct
Cyc_String_pa_struct _tmp1206;struct Cyc_String_pa_struct _tmp1205;(_tmp1205.tag=0,((
_tmp1205.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp1206.tag=0,((_tmp1206.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1207[0]=& _tmp1206,((_tmp1207[1]=& _tmp1205,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp1208="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp1208,sizeof(char),41))),_tag_dyneither(_tmp1207,sizeof(void*),
2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp6FE=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL380: {struct Cyc_Absyn_PointerType_struct*
_tmp6FF=(struct Cyc_Absyn_PointerType_struct*)_tmp6FE;if(_tmp6FF->tag != 5)goto
_LL382;}_LL381: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL37F;_LL382:;_LL383: return 0;_LL37F:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t);int Cyc_Tcutil_is_integral_type(void*t){void*_tmp700=Cyc_Tcutil_compress(
t);_LL385: {struct Cyc_Absyn_IntType_struct*_tmp701=(struct Cyc_Absyn_IntType_struct*)
_tmp700;if(_tmp701->tag != 6)goto _LL387;}_LL386: goto _LL388;_LL387: {struct Cyc_Absyn_TagType_struct*
_tmp702=(struct Cyc_Absyn_TagType_struct*)_tmp700;if(_tmp702->tag != 20)goto _LL389;}
_LL388: goto _LL38A;_LL389: {struct Cyc_Absyn_EnumType_struct*_tmp703=(struct Cyc_Absyn_EnumType_struct*)
_tmp700;if(_tmp703->tag != 14)goto _LL38B;}_LL38A: goto _LL38C;_LL38B: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp704=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp700;if(_tmp704->tag != 15)goto
_LL38D;}_LL38C: return 1;_LL38D:;_LL38E: return 0;_LL384:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp120B;void*_tmp120A;(_tmp120A=0,Cyc_Tcutil_warn(e->loc,((_tmp120B="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp120B,sizeof(char),44))),_tag_dyneither(_tmp120A,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp120E;void*_tmp120D;(
_tmp120D=0,Cyc_Tcutil_warn(e->loc,((_tmp120E="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp120E,sizeof(char),44))),_tag_dyneither(_tmp120D,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp120F;struct _tuple0 _tmp70A=(
_tmp120F.f1=t1,((_tmp120F.f2=t2,_tmp120F)));void*_tmp70B;struct Cyc_Absyn_PtrInfo
_tmp70D;void*_tmp70E;struct Cyc_Absyn_PtrInfo _tmp710;void*_tmp711;struct Cyc_Absyn_ArrayInfo
_tmp713;void*_tmp714;struct Cyc_Absyn_Tqual _tmp715;struct Cyc_Absyn_Exp*_tmp716;
union Cyc_Absyn_Constraint*_tmp717;void*_tmp718;struct Cyc_Absyn_ArrayInfo _tmp71A;
void*_tmp71B;struct Cyc_Absyn_Tqual _tmp71C;struct Cyc_Absyn_Exp*_tmp71D;union Cyc_Absyn_Constraint*
_tmp71E;void*_tmp71F;void*_tmp721;_LL390: _tmp70B=_tmp70A.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp70C=(struct Cyc_Absyn_PointerType_struct*)_tmp70B;if(_tmp70C->tag != 5)goto
_LL392;else{_tmp70D=_tmp70C->f1;}};_tmp70E=_tmp70A.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp70F=(struct Cyc_Absyn_PointerType_struct*)_tmp70E;if(_tmp70F->tag != 5)goto
_LL392;else{_tmp710=_tmp70F->f1;}};_LL391: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp70D.ptr_atts).nullable,(_tmp710.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp70D.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp70D.ptr_atts).bounds,(
_tmp710.ptr_atts).bounds)){struct _tuple0 _tmp1210;struct _tuple0 _tmp724=(_tmp1210.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp70D.ptr_atts).bounds),((_tmp1210.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp710.ptr_atts).bounds),
_tmp1210)));void*_tmp725;void*_tmp727;void*_tmp729;void*_tmp72B;void*_tmp72D;
struct Cyc_Absyn_Exp*_tmp72F;void*_tmp730;struct Cyc_Absyn_Exp*_tmp732;void*
_tmp733;void*_tmp735;struct Cyc_Absyn_Exp*_tmp737;_LL399: _tmp725=_tmp724.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp726=(struct Cyc_Absyn_Upper_b_struct*)_tmp725;
if(_tmp726->tag != 1)goto _LL39B;};_tmp727=_tmp724.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp728=(struct Cyc_Absyn_DynEither_b_struct*)_tmp727;if(_tmp728->tag != 0)goto
_LL39B;};_LL39A: goto _LL39C;_LL39B: _tmp729=_tmp724.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp72A=(struct Cyc_Absyn_DynEither_b_struct*)_tmp729;if(_tmp72A->tag != 0)goto
_LL39D;};_tmp72B=_tmp724.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp72C=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp72B;if(_tmp72C->tag != 0)goto _LL39D;};_LL39C:
okay=1;goto _LL398;_LL39D: _tmp72D=_tmp724.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp72E=(struct Cyc_Absyn_Upper_b_struct*)_tmp72D;if(_tmp72E->tag != 1)goto _LL39F;
else{_tmp72F=_tmp72E->f1;}};_tmp730=_tmp724.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp731=(struct Cyc_Absyn_Upper_b_struct*)_tmp730;if(_tmp731->tag != 1)goto _LL39F;
else{_tmp732=_tmp731->f1;}};_LL39E: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp732,
_tmp72F);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp710.ptr_atts).zero_term)){const char*_tmp1213;void*_tmp1212;(_tmp1212=0,
Cyc_Tcutil_warn(loc,((_tmp1213="implicit cast to shorter array",_tag_dyneither(
_tmp1213,sizeof(char),31))),_tag_dyneither(_tmp1212,sizeof(void*),0)));}goto
_LL398;_LL39F: _tmp733=_tmp724.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp734=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp733;if(_tmp734->tag != 0)goto _LL398;};
_tmp735=_tmp724.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp736=(struct Cyc_Absyn_Upper_b_struct*)
_tmp735;if(_tmp736->tag != 1)goto _LL398;else{_tmp737=_tmp736->f1;}};_LL3A0: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp70D.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp710.ptr_atts).bounds))goto _LL398;okay=0;goto
_LL398;_LL398:;}okay=okay  && (!(_tmp70D.elt_tq).real_const  || (_tmp710.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp70D.ptr_atts).rgn,(_tmp710.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp70D.ptr_atts).rgn,(_tmp710.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp70D.ptr_atts).zero_term,(
_tmp710.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp70D.ptr_atts).zero_term) && (_tmp710.elt_tq).real_const);okay=okay  && (
Cyc_Tcutil_unify(_tmp70D.elt_typ,_tmp710.elt_typ) || Cyc_Tcutil_ptrsubtype(te,0,
_tmp70D.elt_typ,_tmp710.elt_typ));return okay;}_LL392: _tmp711=_tmp70A.f1;{struct
Cyc_Absyn_ArrayType_struct*_tmp712=(struct Cyc_Absyn_ArrayType_struct*)_tmp711;
if(_tmp712->tag != 9)goto _LL394;else{_tmp713=_tmp712->f1;_tmp714=_tmp713.elt_type;
_tmp715=_tmp713.tq;_tmp716=_tmp713.num_elts;_tmp717=_tmp713.zero_term;}};_tmp718=
_tmp70A.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp719=(struct Cyc_Absyn_ArrayType_struct*)
_tmp718;if(_tmp719->tag != 9)goto _LL394;else{_tmp71A=_tmp719->f1;_tmp71B=_tmp71A.elt_type;
_tmp71C=_tmp71A.tq;_tmp71D=_tmp71A.num_elts;_tmp71E=_tmp71A.zero_term;}};_LL393: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp717,_tmp71E) && ((_tmp716 != 0
 && _tmp71D != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp716,(
struct Cyc_Absyn_Exp*)_tmp71D));return(okay  && Cyc_Tcutil_unify(_tmp714,_tmp71B))
 && (!_tmp715.real_const  || _tmp71C.real_const);}_LL394: _tmp71F=_tmp70A.f1;{
struct Cyc_Absyn_TagType_struct*_tmp720=(struct Cyc_Absyn_TagType_struct*)_tmp71F;
if(_tmp720->tag != 20)goto _LL396;};_tmp721=_tmp70A.f2;{struct Cyc_Absyn_IntType_struct*
_tmp722=(struct Cyc_Absyn_IntType_struct*)_tmp721;if(_tmp722->tag != 6)goto _LL396;};
_LL395: return 0;_LL396:;_LL397: return Cyc_Tcutil_unify(t1,t2);_LL38F:;};}int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp73A=Cyc_Tcutil_compress(t);
_LL3A2: {struct Cyc_Absyn_PointerType_struct*_tmp73B=(struct Cyc_Absyn_PointerType_struct*)
_tmp73A;if(_tmp73B->tag != 5)goto _LL3A4;}_LL3A3: return 1;_LL3A4:;_LL3A5: return 0;
_LL3A1:;}void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp73C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp73E;void*
_tmp73F;_LL3A7: {struct Cyc_Absyn_PointerType_struct*_tmp73D=(struct Cyc_Absyn_PointerType_struct*)
_tmp73C;if(_tmp73D->tag != 5)goto _LL3A9;else{_tmp73E=_tmp73D->f1;_tmp73F=_tmp73E.elt_typ;}}
_LL3A8: return _tmp73F;_LL3A9:;_LL3AA: {const char*_tmp1216;void*_tmp1215;(_tmp1215=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1216="pointer_elt_type",_tag_dyneither(_tmp1216,sizeof(char),17))),
_tag_dyneither(_tmp1215,sizeof(void*),0)));}_LL3A6:;}void*Cyc_Tcutil_pointer_region(
void*t);void*Cyc_Tcutil_pointer_region(void*t){void*_tmp742=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp744;struct Cyc_Absyn_PtrAtts _tmp745;struct Cyc_Absyn_PtrAtts*
_tmp746;_LL3AC: {struct Cyc_Absyn_PointerType_struct*_tmp743=(struct Cyc_Absyn_PointerType_struct*)
_tmp742;if(_tmp743->tag != 5)goto _LL3AE;else{_tmp744=_tmp743->f1;_tmp745=_tmp744.ptr_atts;
_tmp746=(struct Cyc_Absyn_PtrAtts*)&(_tmp743->f1).ptr_atts;}}_LL3AD: return _tmp746->rgn;
_LL3AE:;_LL3AF: {const char*_tmp1219;void*_tmp1218;(_tmp1218=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1219="pointer_elt_type",
_tag_dyneither(_tmp1219,sizeof(char),17))),_tag_dyneither(_tmp1218,sizeof(void*),
0)));}_LL3AB:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);int Cyc_Tcutil_rgn_of_pointer(
void*t,void**rgn){void*_tmp749=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp74B;struct Cyc_Absyn_PtrAtts _tmp74C;void*_tmp74D;_LL3B1: {struct Cyc_Absyn_PointerType_struct*
_tmp74A=(struct Cyc_Absyn_PointerType_struct*)_tmp749;if(_tmp74A->tag != 5)goto
_LL3B3;else{_tmp74B=_tmp74A->f1;_tmp74C=_tmp74B.ptr_atts;_tmp74D=_tmp74C.rgn;}}
_LL3B2:*rgn=_tmp74D;return 1;_LL3B3:;_LL3B4: return 0;_LL3B0:;}int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr){void*_tmp74E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp750;struct Cyc_Absyn_PtrAtts _tmp751;union Cyc_Absyn_Constraint*_tmp752;_LL3B6: {
struct Cyc_Absyn_PointerType_struct*_tmp74F=(struct Cyc_Absyn_PointerType_struct*)
_tmp74E;if(_tmp74F->tag != 5)goto _LL3B8;else{_tmp750=_tmp74F->f1;_tmp751=_tmp750.ptr_atts;
_tmp752=_tmp751.bounds;}}_LL3B7:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp752)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3B8:;_LL3B9: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;
_LL3B5:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp753=e->r;union Cyc_Absyn_Cnst _tmp755;struct
_tuple7 _tmp756;int _tmp757;union Cyc_Absyn_Cnst _tmp759;struct _tuple5 _tmp75A;char
_tmp75B;union Cyc_Absyn_Cnst _tmp75D;struct _tuple6 _tmp75E;short _tmp75F;union Cyc_Absyn_Cnst
_tmp761;struct _tuple8 _tmp762;long long _tmp763;void*_tmp765;struct Cyc_Absyn_Exp*
_tmp766;_LL3BB: {struct Cyc_Absyn_Const_e_struct*_tmp754=(struct Cyc_Absyn_Const_e_struct*)
_tmp753;if(_tmp754->tag != 0)goto _LL3BD;else{_tmp755=_tmp754->f1;if((_tmp755.Int_c).tag
!= 4)goto _LL3BD;_tmp756=(struct _tuple7)(_tmp755.Int_c).val;_tmp757=_tmp756.f2;
if(_tmp757 != 0)goto _LL3BD;}}_LL3BC: goto _LL3BE;_LL3BD: {struct Cyc_Absyn_Const_e_struct*
_tmp758=(struct Cyc_Absyn_Const_e_struct*)_tmp753;if(_tmp758->tag != 0)goto _LL3BF;
else{_tmp759=_tmp758->f1;if((_tmp759.Char_c).tag != 2)goto _LL3BF;_tmp75A=(struct
_tuple5)(_tmp759.Char_c).val;_tmp75B=_tmp75A.f2;if(_tmp75B != 0)goto _LL3BF;}}
_LL3BE: goto _LL3C0;_LL3BF: {struct Cyc_Absyn_Const_e_struct*_tmp75C=(struct Cyc_Absyn_Const_e_struct*)
_tmp753;if(_tmp75C->tag != 0)goto _LL3C1;else{_tmp75D=_tmp75C->f1;if((_tmp75D.Short_c).tag
!= 3)goto _LL3C1;_tmp75E=(struct _tuple6)(_tmp75D.Short_c).val;_tmp75F=_tmp75E.f2;
if(_tmp75F != 0)goto _LL3C1;}}_LL3C0: goto _LL3C2;_LL3C1: {struct Cyc_Absyn_Const_e_struct*
_tmp760=(struct Cyc_Absyn_Const_e_struct*)_tmp753;if(_tmp760->tag != 0)goto _LL3C3;
else{_tmp761=_tmp760->f1;if((_tmp761.LongLong_c).tag != 5)goto _LL3C3;_tmp762=(
struct _tuple8)(_tmp761.LongLong_c).val;_tmp763=_tmp762.f2;if(_tmp763 != 0)goto
_LL3C3;}}_LL3C2: return 1;_LL3C3: {struct Cyc_Absyn_Cast_e_struct*_tmp764=(struct
Cyc_Absyn_Cast_e_struct*)_tmp753;if(_tmp764->tag != 15)goto _LL3C5;else{_tmp765=(
void*)_tmp764->f1;_tmp766=_tmp764->f2;}}_LL3C4: return Cyc_Tcutil_is_zero(_tmp766)
 && Cyc_Tcutil_admits_zero(_tmp765);_LL3C5:;_LL3C6: return 0;_LL3BA:;}struct Cyc_Absyn_Kind
Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_ak={
Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,
Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};struct
Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind
Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_tak={
Cyc_Absyn_AnyKind,Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,
Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};struct
Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};struct Cyc_Absyn_Kind
Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};struct Cyc_Absyn_Kind Cyc_Tcutil_umk={
Cyc_Absyn_MemKind,Cyc_Absyn_Unique};struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)&
Cyc_Tcutil_rk};struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};struct
Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};struct Cyc_Core_Opt Cyc_Tcutil_mko={(
void*)& Cyc_Tcutil_mk};struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};struct Cyc_Core_Opt Cyc_Tcutil_trko={(
void*)& Cyc_Tcutil_trk};struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};struct Cyc_Core_Opt Cyc_Tcutil_tmko={(
void*)& Cyc_Tcutil_tmk};struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};struct Cyc_Core_Opt Cyc_Tcutil_ubko={(
void*)& Cyc_Tcutil_ubk};struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka);struct Cyc_Core_Opt*
Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp768;
enum Cyc_Absyn_KindQual _tmp769;enum Cyc_Absyn_AliasQual _tmp76A;struct Cyc_Absyn_Kind*
_tmp767=ka;_tmp768=*_tmp767;_tmp769=_tmp768.kind;_tmp76A=_tmp768.aliasqual;
switch(_tmp76A){case Cyc_Absyn_Aliasable: _LL3C7: switch(_tmp769){case Cyc_Absyn_AnyKind:
_LL3C9: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3CA:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3CB: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3CC: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3CD: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3CE: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_iko;}
case Cyc_Absyn_Unique: _LL3C8: switch(_tmp769){case Cyc_Absyn_AnyKind: _LL3D1: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3D2: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3D3: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3D4: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;
default: _LL3D5: break;}break;case Cyc_Absyn_Top: _LL3D0: switch(_tmp769){case Cyc_Absyn_AnyKind:
_LL3D8: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3D9:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3DA: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL3DB: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL3DC: break;}break;}{const char*_tmp121D;
void*_tmp121C[1];struct Cyc_String_pa_struct _tmp121B;(_tmp121B.tag=0,((_tmp121B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((
_tmp121C[0]=& _tmp121B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp121D="kind_to_opt: bad kind %s\n",_tag_dyneither(
_tmp121D,sizeof(char),26))),_tag_dyneither(_tmp121C,sizeof(void*),1)))))));};}
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka);static void**Cyc_Tcutil_kind_to_b(
struct Cyc_Absyn_Kind*ka){static struct Cyc_Absyn_Eq_kb_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_struct mb_v={0,& Cyc_Tcutil_mk};static struct Cyc_Absyn_Eq_kb_struct
bb_v={0,& Cyc_Tcutil_bk};static struct Cyc_Absyn_Eq_kb_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_struct eb_v={0,& Cyc_Tcutil_ek};static struct Cyc_Absyn_Eq_kb_struct
ib_v={0,& Cyc_Tcutil_ik};static void*ab=(void*)& ab_v;static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;static void*rb=(void*)& rb_v;static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;static struct Cyc_Absyn_Eq_kb_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_struct tmb_v={0,& Cyc_Tcutil_tmk};static struct Cyc_Absyn_Eq_kb_struct
tbb_v={0,& Cyc_Tcutil_tbk};static struct Cyc_Absyn_Eq_kb_struct trb_v={0,& Cyc_Tcutil_trk};
static void*tab=(void*)& tab_v;static void*tmb=(void*)& tmb_v;static void*tbb=(void*)&
tbb_v;static void*trb=(void*)& trb_v;static struct Cyc_Absyn_Eq_kb_struct uab_v={0,&
Cyc_Tcutil_uak};static struct Cyc_Absyn_Eq_kb_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_struct ubb_v={0,& Cyc_Tcutil_ubk};static struct Cyc_Absyn_Eq_kb_struct
urb_v={0,& Cyc_Tcutil_urk};static void*uab=(void*)& uab_v;static void*umb=(void*)&
umb_v;static void*ubb=(void*)& ubb_v;static void*urb=(void*)& urb_v;struct Cyc_Absyn_Kind
_tmp76F;enum Cyc_Absyn_KindQual _tmp770;enum Cyc_Absyn_AliasQual _tmp771;struct Cyc_Absyn_Kind*
_tmp76E=ka;_tmp76F=*_tmp76E;_tmp770=_tmp76F.kind;_tmp771=_tmp76F.aliasqual;
switch(_tmp771){case Cyc_Absyn_Aliasable: _LL3DE: switch(_tmp770){case Cyc_Absyn_AnyKind:
_LL3E0: return& ab;case Cyc_Absyn_MemKind: _LL3E1: return& mb;case Cyc_Absyn_BoxKind:
_LL3E2: return& bb;case Cyc_Absyn_RgnKind: _LL3E3: return& rb;case Cyc_Absyn_EffKind:
_LL3E4: return& eb;case Cyc_Absyn_IntKind: _LL3E5: return& ib;}case Cyc_Absyn_Unique:
_LL3DF: switch(_tmp770){case Cyc_Absyn_AnyKind: _LL3E8: return& uab;case Cyc_Absyn_MemKind:
_LL3E9: return& umb;case Cyc_Absyn_BoxKind: _LL3EA: return& ubb;case Cyc_Absyn_RgnKind:
_LL3EB: return& urb;default: _LL3EC: break;}break;case Cyc_Absyn_Top: _LL3E7: switch(
_tmp770){case Cyc_Absyn_AnyKind: _LL3EF: return& tab;case Cyc_Absyn_MemKind: _LL3F0:
return& tmb;case Cyc_Absyn_BoxKind: _LL3F1: return& tbb;case Cyc_Absyn_RgnKind: _LL3F2:
return& trb;default: _LL3F3: break;}break;}{const char*_tmp1221;void*_tmp1220[1];
struct Cyc_String_pa_struct _tmp121F;(_tmp121F.tag=0,((_tmp121F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp1220[0]=&
_tmp121F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1221="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp1221,sizeof(char),24))),
_tag_dyneither(_tmp1220,sizeof(void*),1)))))));};}void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k);void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
struct Cyc_Absyn_Kind*k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct
Cyc_Absyn_Kind*k){return(struct Cyc_Core_Opt*)Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp1224;struct Cyc_Absyn_Const_e_struct*
_tmp1223;e1->r=(void*)((_tmp1223=_cycalloc(sizeof(*_tmp1223)),((_tmp1223[0]=((
_tmp1224.tag=0,((_tmp1224.f1=Cyc_Absyn_Null_c,_tmp1224)))),_tmp1223))));}{struct
Cyc_Core_Opt*_tmp785=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp122E;struct Cyc_Absyn_PtrAtts _tmp122D;struct Cyc_Absyn_PtrInfo _tmp122C;struct
Cyc_Absyn_PointerType_struct*_tmp122B;struct Cyc_Absyn_PointerType_struct*_tmp786=(
_tmp122B=_cycalloc(sizeof(*_tmp122B)),((_tmp122B[0]=((_tmp122E.tag=5,((_tmp122E.f1=((
_tmp122C.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp785),((
_tmp122C.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp122C.ptr_atts=((_tmp122D.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp785),((_tmp122D.nullable=Cyc_Absyn_true_conref,((
_tmp122D.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp122D.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp122D.ptrloc=0,_tmp122D)))))))))),_tmp122C)))))),_tmp122E)))),_tmp122B)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp786);{int bogus=
0;int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);if(bogus != 0){const char*
_tmp1233;void*_tmp1232[2];struct Cyc_String_pa_struct _tmp1231;struct Cyc_String_pa_struct
_tmp1230;(_tmp1230.tag=0,((_tmp1230.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(e1->loc)),((_tmp1231.tag=0,((
_tmp1231.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1232[0]=& _tmp1231,((_tmp1232[1]=& _tmp1230,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1233="zero_to_null resulted in an alias coercion on %s at %s\n",
_tag_dyneither(_tmp1233,sizeof(char),56))),_tag_dyneither(_tmp1232,sizeof(void*),
2)))))))))))));}return retv;};};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(
enum Cyc_Absyn_Coercion c);struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum 
Cyc_Absyn_Coercion c){switch(c){case Cyc_Absyn_Unknown_coercion: _LL3F5: {const char*
_tmp1234;return(_tmp1234="unknown",_tag_dyneither(_tmp1234,sizeof(char),8));}
case Cyc_Absyn_No_coercion: _LL3F6: {const char*_tmp1235;return(_tmp1235="no coercion",
_tag_dyneither(_tmp1235,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL3F7: {
const char*_tmp1236;return(_tmp1236="null check",_tag_dyneither(_tmp1236,sizeof(
char),11));}case Cyc_Absyn_Other_coercion: _LL3F8: {const char*_tmp1237;return(
_tmp1237="other coercion",_tag_dyneither(_tmp1237,sizeof(char),15));}}}int Cyc_Tcutil_warn_alias_coerce=
0;struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);struct
_tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){static struct
Cyc_Absyn_Eq_kb_struct rgn_kb={0,& Cyc_Tcutil_rk};static int counter=0;struct
_dyneither_ptr*_tmp1244;const char*_tmp1243;void*_tmp1242[1];struct Cyc_Int_pa_struct
_tmp1241;struct _tuple2*_tmp1240;struct _tuple2*v=(_tmp1240=_cycalloc(sizeof(*
_tmp1240)),((_tmp1240->f1=Cyc_Absyn_Loc_n,((_tmp1240->f2=(struct _dyneither_ptr*)((
_tmp1244=_cycalloc(sizeof(*_tmp1244)),((_tmp1244[0]=((_tmp1241.tag=1,((_tmp1241.f1=(
unsigned long)counter ++,((_tmp1242[0]=& _tmp1241,Cyc_aprintf(((_tmp1243="__aliasvar%d",
_tag_dyneither(_tmp1243,sizeof(char),13))),_tag_dyneither(_tmp1242,sizeof(void*),
1)))))))),_tmp1244)))),_tmp1240)))));struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(
v,(void*)& Cyc_Absyn_VoidType_val,0);struct Cyc_Absyn_Local_b_struct _tmp1247;
struct Cyc_Absyn_Local_b_struct*_tmp1246;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(
v,(void*)((_tmp1246=_cycalloc(sizeof(*_tmp1246)),((_tmp1246[0]=((_tmp1247.tag=4,((
_tmp1247.f1=vd,_tmp1247)))),_tmp1246)))),e->loc);struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((
void*)& rgn_kb);{void*_tmp793=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo
_tmp795;struct Cyc_Absyn_PtrAtts _tmp796;void*_tmp797;void**_tmp798;_LL3FB: {
struct Cyc_Absyn_PointerType_struct*_tmp794=(struct Cyc_Absyn_PointerType_struct*)
_tmp793;if(_tmp794->tag != 5)goto _LL3FD;else{_tmp795=_tmp794->f1;_tmp796=_tmp795.ptr_atts;
_tmp797=_tmp796.rgn;_tmp798=(void**)&((_tmp794->f1).ptr_atts).rgn;}}_LL3FC:{
struct Cyc_Absyn_VarType_struct _tmp124A;struct Cyc_Absyn_VarType_struct*_tmp1249;*
_tmp798=(void*)((_tmp1249=_cycalloc(sizeof(*_tmp1249)),((_tmp1249[0]=((_tmp124A.tag=
2,((_tmp124A.f1=tv,_tmp124A)))),_tmp1249))));}goto _LL3FA;_LL3FD:;_LL3FE: goto
_LL3FA;_LL3FA:;}e->topt=0;e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,
e->loc);{struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);struct
_tuple14 _tmp124B;return(_tmp124B.f1=d,((_tmp124B.f2=ve,_tmp124B)));};}static int
Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,
struct Cyc_Position_Segment*loc);static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ,void*wants_typ,struct Cyc_Position_Segment*loc){struct _RegionHandle
_tmp7A4=_new_region("r");struct _RegionHandle*r=& _tmp7A4;_push_region(r);if((Cyc_Tcutil_is_noalias_path(
r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && Cyc_Tcutil_is_pointer_type(
e_typ)){void*_tmp7A5=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo
_tmp7A7;struct Cyc_Absyn_PtrAtts _tmp7A8;void*_tmp7A9;_LL400: {struct Cyc_Absyn_PointerType_struct*
_tmp7A6=(struct Cyc_Absyn_PointerType_struct*)_tmp7A5;if(_tmp7A6->tag != 5)goto
_LL402;else{_tmp7A7=_tmp7A6->f1;_tmp7A8=_tmp7A7.ptr_atts;_tmp7A9=_tmp7A8.rgn;}}
_LL401: {void*_tmp7AB=Cyc_Tcutil_compress(_tmp7A9);_LL407: {struct Cyc_Absyn_HeapRgn_struct*
_tmp7AC=(struct Cyc_Absyn_HeapRgn_struct*)_tmp7AB;if(_tmp7AC->tag != 21)goto _LL409;}
_LL408: {int _tmp7AD=0;_npop_handler(0);return _tmp7AD;}_LL409:;_LL40A: {struct Cyc_Absyn_Kind*
_tmp7AE=Cyc_Tcutil_typ_kind(_tmp7A9);int _tmp7AF=_tmp7AE->kind == Cyc_Absyn_RgnKind
 && _tmp7AE->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp7AF;}
_LL406:;}_LL402: {struct Cyc_Absyn_TypedefType_struct*_tmp7AA=(struct Cyc_Absyn_TypedefType_struct*)
_tmp7A5;if(_tmp7AA->tag != 18)goto _LL404;}_LL403: {const char*_tmp1250;void*
_tmp124F[2];struct Cyc_String_pa_struct _tmp124E;struct Cyc_String_pa_struct
_tmp124D;(_tmp124D.tag=0,((_tmp124D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(loc)),((_tmp124E.tag=0,((_tmp124E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(
wants_typ))),((_tmp124F[0]=& _tmp124E,((_tmp124F[1]=& _tmp124D,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1250="got typedef %s in can_insert_alias at %s\n",
_tag_dyneither(_tmp1250,sizeof(char),42))),_tag_dyneither(_tmp124F,sizeof(void*),
2)))))))))))));}_LL404:;_LL405: {int _tmp7B4=0;_npop_handler(0);return _tmp7B4;}
_LL3FF:;}{int _tmp7B5=0;_npop_handler(0);return _tmp7B5;};;_pop_region(r);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){void*
t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
enum Cyc_Absyn_Coercion c;int do_alias_coercion=0;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp1255;void*_tmp1254[2];
struct Cyc_String_pa_struct _tmp1253;struct Cyc_String_pa_struct _tmp1252;(_tmp1252.tag=
0,((_tmp1252.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1253.tag=0,((_tmp1253.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp1254[0]=& _tmp1253,((_tmp1254[1]=& _tmp1252,Cyc_Tcutil_warn(
e->loc,((_tmp1255="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp1255,sizeof(char),53))),_tag_dyneither(_tmp1254,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp125B;void*_tmp125A[3];struct Cyc_String_pa_struct _tmp1259;struct Cyc_String_pa_struct
_tmp1258;struct Cyc_String_pa_struct _tmp1257;(_tmp1257.tag=0,((_tmp1257.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp1258.tag=0,((_tmp1258.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp1259.tag=0,((_tmp1259.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp125A[0]=& _tmp1259,((_tmp125A[1]=& _tmp1258,((
_tmp125A[2]=& _tmp1257,Cyc_Tcutil_warn(e->loc,((_tmp125B="implicit alias coercion for %s:%s to %s",
_tag_dyneither(_tmp125B,sizeof(char),40))),_tag_dyneither(_tmp125A,sizeof(void*),
3)))))))))))))))))));}*alias_coercion=1;}if(Cyc_Tcutil_silent_castable(te,e->loc,
t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);return 1;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(
te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){Cyc_Tcutil_unchecked_cast(te,e,
t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*_tmp1260;void*_tmp125F[2];
struct Cyc_String_pa_struct _tmp125E;struct Cyc_String_pa_struct _tmp125D;(_tmp125D.tag=
0,((_tmp125D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp125E.tag=0,((_tmp125E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp125F[0]=& _tmp125E,((_tmp125F[1]=& _tmp125D,Cyc_Tcutil_warn(
e->loc,((_tmp1260="implicit cast from %s to %s",_tag_dyneither(_tmp1260,sizeof(
char),28))),_tag_dyneither(_tmp125F,sizeof(void*),2)))))))))))));}return 1;}else{
return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t){int bogus=0;return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}int Cyc_Tcutil_coerceable(
void*t);int Cyc_Tcutil_coerceable(void*t){void*_tmp7C3=Cyc_Tcutil_compress(t);
_LL40C: {struct Cyc_Absyn_IntType_struct*_tmp7C4=(struct Cyc_Absyn_IntType_struct*)
_tmp7C3;if(_tmp7C4->tag != 6)goto _LL40E;}_LL40D: goto _LL40F;_LL40E: {struct Cyc_Absyn_FloatType_struct*
_tmp7C5=(struct Cyc_Absyn_FloatType_struct*)_tmp7C3;if(_tmp7C5->tag != 7)goto
_LL410;}_LL40F: goto _LL411;_LL410: {struct Cyc_Absyn_DoubleType_struct*_tmp7C6=(
struct Cyc_Absyn_DoubleType_struct*)_tmp7C3;if(_tmp7C6->tag != 8)goto _LL412;}
_LL411: return 1;_LL412:;_LL413: return 0;_LL40B:;}static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple16*env,struct Cyc_Absyn_Aggrfield*x);static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple16*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp7C8;
struct _RegionHandle*_tmp7C9;struct _tuple16 _tmp7C7=*env;_tmp7C8=_tmp7C7.f1;
_tmp7C9=_tmp7C7.f2;{struct _tuple11*_tmp1261;return(_tmp1261=_region_malloc(
_tmp7C9,sizeof(*_tmp1261)),((_tmp1261->f1=x->tq,((_tmp1261->f2=Cyc_Tcutil_rsubstitute(
_tmp7C9,_tmp7C8,x->type),_tmp1261)))));};}static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x);static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x){struct Cyc_Absyn_Tqual _tmp7CC;void*
_tmp7CD;struct _tuple11 _tmp7CB=*x;_tmp7CC=_tmp7CB.f1;_tmp7CD=_tmp7CB.f2;{struct
_tuple11*_tmp1262;return(_tmp1262=_region_malloc(r,sizeof(*_tmp1262)),((_tmp1262->f1=
_tmp7CC,((_tmp1262->f2=_tmp7CD,_tmp1262)))));};}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1);static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=
Cyc_Tcutil_compress(t1);{void*_tmp7CF=t1;struct Cyc_List_List*_tmp7D2;struct Cyc_Absyn_AggrInfo
_tmp7D4;union Cyc_Absyn_AggrInfoU _tmp7D5;struct Cyc_Absyn_Aggrdecl**_tmp7D6;struct
Cyc_Absyn_Aggrdecl*_tmp7D7;struct Cyc_List_List*_tmp7D8;enum Cyc_Absyn_AggrKind
_tmp7DA;struct Cyc_List_List*_tmp7DB;_LL415: {struct Cyc_Absyn_VoidType_struct*
_tmp7D0=(struct Cyc_Absyn_VoidType_struct*)_tmp7CF;if(_tmp7D0->tag != 0)goto _LL417;}
_LL416: return 0;_LL417: {struct Cyc_Absyn_TupleType_struct*_tmp7D1=(struct Cyc_Absyn_TupleType_struct*)
_tmp7CF;if(_tmp7D1->tag != 11)goto _LL419;else{_tmp7D2=_tmp7D1->f1;}}_LL418: return((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_RegionHandle*,struct _tuple11*),struct _RegionHandle*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp7D2);_LL419: {struct Cyc_Absyn_AggrType_struct*
_tmp7D3=(struct Cyc_Absyn_AggrType_struct*)_tmp7CF;if(_tmp7D3->tag != 12)goto
_LL41B;else{_tmp7D4=_tmp7D3->f1;_tmp7D5=_tmp7D4.aggr_info;if((_tmp7D5.KnownAggr).tag
!= 2)goto _LL41B;_tmp7D6=(struct Cyc_Absyn_Aggrdecl**)(_tmp7D5.KnownAggr).val;
_tmp7D7=*_tmp7D6;_tmp7D8=_tmp7D4.targs;}}_LL41A: if(((_tmp7D7->kind == Cyc_Absyn_UnionA
 || _tmp7D7->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7D7->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7D7->impl))->rgn_po != 0){
struct _tuple11*_tmp1265;struct Cyc_List_List*_tmp1264;return(_tmp1264=
_region_malloc(r,sizeof(*_tmp1264)),((_tmp1264->hd=((_tmp1265=_region_malloc(r,
sizeof(*_tmp1265)),((_tmp1265->f1=Cyc_Absyn_const_tqual(0),((_tmp1265->f2=t1,
_tmp1265)))))),((_tmp1264->tl=0,_tmp1264)))));}{struct Cyc_List_List*_tmp7DE=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp7D7->tvs,_tmp7D8);struct _tuple16
_tmp1266;struct _tuple16 env=(_tmp1266.f1=_tmp7DE,((_tmp1266.f2=r,_tmp1266)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_tuple16*,struct Cyc_Absyn_Aggrfield*),struct _tuple16*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7D7->impl))->fields);};_LL41B: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp7D9=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp7CF;if(_tmp7D9->tag != 13)goto
_LL41D;else{_tmp7DA=_tmp7D9->f1;if(_tmp7DA != Cyc_Absyn_StructA)goto _LL41D;
_tmp7DB=_tmp7D9->f2;}}_LL41C: {struct _tuple16 _tmp1267;struct _tuple16 env=(
_tmp1267.f1=0,((_tmp1267.f2=r,_tmp1267)));return((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple16*,struct Cyc_Absyn_Aggrfield*),
struct _tuple16*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp7DB);}_LL41D:;_LL41E: {struct _tuple11*_tmp126A;struct Cyc_List_List*
_tmp1269;return(_tmp1269=_region_malloc(r,sizeof(*_tmp1269)),((_tmp1269->hd=((
_tmp126A=_region_malloc(r,sizeof(*_tmp126A)),((_tmp126A->f1=Cyc_Absyn_const_tqual(
0),((_tmp126A->f2=t1,_tmp126A)))))),((_tmp1269->tl=0,_tmp1269)))));}_LL414:;};}
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2);
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){{
struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*_tmp7E3=(void*)t->hd;_LL420: {
struct Cyc_Absyn_Pure_att_struct*_tmp7E4=(struct Cyc_Absyn_Pure_att_struct*)
_tmp7E3;if(_tmp7E4->tag != 22)goto _LL422;}_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp7E5=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp7E3;if(_tmp7E5->tag != 4)goto
_LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_Initializes_att_struct*
_tmp7E6=(struct Cyc_Absyn_Initializes_att_struct*)_tmp7E3;if(_tmp7E6->tag != 20)
goto _LL426;}_LL425: continue;_LL426:;_LL427: if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,
a2))return 0;_LL41F:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmp126B;struct _tuple0 _tmp7E8=(_tmp126B.f1=t1,((_tmp126B.f2=t2,_tmp126B)));void*
_tmp7E9;struct Cyc_Absyn_PtrInfo _tmp7EB;void*_tmp7EC;struct Cyc_Absyn_Tqual _tmp7ED;
struct Cyc_Absyn_PtrAtts _tmp7EE;void*_tmp7EF;union Cyc_Absyn_Constraint*_tmp7F0;
union Cyc_Absyn_Constraint*_tmp7F1;union Cyc_Absyn_Constraint*_tmp7F2;void*_tmp7F3;
struct Cyc_Absyn_PtrInfo _tmp7F5;void*_tmp7F6;struct Cyc_Absyn_Tqual _tmp7F7;struct
Cyc_Absyn_PtrAtts _tmp7F8;void*_tmp7F9;union Cyc_Absyn_Constraint*_tmp7FA;union Cyc_Absyn_Constraint*
_tmp7FB;union Cyc_Absyn_Constraint*_tmp7FC;void*_tmp7FD;struct Cyc_Absyn_DatatypeFieldInfo
_tmp7FF;union Cyc_Absyn_DatatypeFieldInfoU _tmp800;struct _tuple3 _tmp801;struct Cyc_Absyn_Datatypedecl*
_tmp802;struct Cyc_Absyn_Datatypefield*_tmp803;struct Cyc_List_List*_tmp804;void*
_tmp805;struct Cyc_Absyn_DatatypeInfo _tmp807;union Cyc_Absyn_DatatypeInfoU _tmp808;
struct Cyc_Absyn_Datatypedecl**_tmp809;struct Cyc_Absyn_Datatypedecl*_tmp80A;
struct Cyc_List_List*_tmp80B;void*_tmp80C;struct Cyc_Absyn_FnInfo _tmp80E;void*
_tmp80F;struct Cyc_Absyn_FnInfo _tmp811;_LL429: _tmp7E9=_tmp7E8.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp7EA=(struct Cyc_Absyn_PointerType_struct*)_tmp7E9;if(_tmp7EA->tag != 5)goto
_LL42B;else{_tmp7EB=_tmp7EA->f1;_tmp7EC=_tmp7EB.elt_typ;_tmp7ED=_tmp7EB.elt_tq;
_tmp7EE=_tmp7EB.ptr_atts;_tmp7EF=_tmp7EE.rgn;_tmp7F0=_tmp7EE.nullable;_tmp7F1=
_tmp7EE.bounds;_tmp7F2=_tmp7EE.zero_term;}};_tmp7F3=_tmp7E8.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp7F4=(struct Cyc_Absyn_PointerType_struct*)_tmp7F3;if(_tmp7F4->tag != 5)goto
_LL42B;else{_tmp7F5=_tmp7F4->f1;_tmp7F6=_tmp7F5.elt_typ;_tmp7F7=_tmp7F5.elt_tq;
_tmp7F8=_tmp7F5.ptr_atts;_tmp7F9=_tmp7F8.rgn;_tmp7FA=_tmp7F8.nullable;_tmp7FB=
_tmp7F8.bounds;_tmp7FC=_tmp7F8.zero_term;}};_LL42A: if(_tmp7ED.real_const  && !
_tmp7F7.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7F0,
_tmp7FA) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp7F0)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp7FA))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7F2,
_tmp7FC) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp7F2)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp7FC))return 0;if(!Cyc_Tcutil_unify(_tmp7EF,_tmp7F9) && !Cyc_Tcenv_region_outlives(
te,_tmp7EF,_tmp7F9))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp7F1,_tmp7FB)){struct _tuple0 _tmp126C;struct _tuple0 _tmp813=(_tmp126C.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp7F1),((_tmp126C.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp7FB),_tmp126C)));void*
_tmp814;void*_tmp816;void*_tmp818;struct Cyc_Absyn_Exp*_tmp81A;void*_tmp81B;
struct Cyc_Absyn_Exp*_tmp81D;_LL432: _tmp814=_tmp813.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp815=(struct Cyc_Absyn_Upper_b_struct*)_tmp814;if(_tmp815->tag != 1)goto _LL434;};
_tmp816=_tmp813.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp817=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp816;if(_tmp817->tag != 0)goto _LL434;};_LL433: goto _LL431;_LL434: _tmp818=
_tmp813.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp819=(struct Cyc_Absyn_Upper_b_struct*)
_tmp818;if(_tmp819->tag != 1)goto _LL436;else{_tmp81A=_tmp819->f1;}};_tmp81B=
_tmp813.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp81C=(struct Cyc_Absyn_Upper_b_struct*)
_tmp81B;if(_tmp81C->tag != 1)goto _LL436;else{_tmp81D=_tmp81C->f1;}};_LL435: if(!
Cyc_Evexp_lte_const_exp(_tmp81D,_tmp81A))return 0;goto _LL431;_LL436:;_LL437:
return 0;_LL431:;}{struct _tuple0*_tmp126F;struct Cyc_List_List*_tmp126E;return Cyc_Tcutil_ptrsubtype(
te,((_tmp126E=_cycalloc(sizeof(*_tmp126E)),((_tmp126E->hd=((_tmp126F=_cycalloc(
sizeof(*_tmp126F)),((_tmp126F->f1=t1,((_tmp126F->f2=t2,_tmp126F)))))),((_tmp126E->tl=
assume,_tmp126E)))))),_tmp7EC,_tmp7F6);};_LL42B: _tmp7FD=_tmp7E8.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp7FE=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp7FD;if(_tmp7FE->tag != 4)
goto _LL42D;else{_tmp7FF=_tmp7FE->f1;_tmp800=_tmp7FF.field_info;if((_tmp800.KnownDatatypefield).tag
!= 2)goto _LL42D;_tmp801=(struct _tuple3)(_tmp800.KnownDatatypefield).val;_tmp802=
_tmp801.f1;_tmp803=_tmp801.f2;_tmp804=_tmp7FF.targs;}};_tmp805=_tmp7E8.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp806=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp805;if(_tmp806->tag != 3)goto _LL42D;else{_tmp807=_tmp806->f1;_tmp808=_tmp807.datatype_info;
if((_tmp808.KnownDatatype).tag != 2)goto _LL42D;_tmp809=(struct Cyc_Absyn_Datatypedecl**)(
_tmp808.KnownDatatype).val;_tmp80A=*_tmp809;_tmp80B=_tmp807.targs;}};_LL42C: if(
_tmp802 != _tmp80A  && Cyc_Absyn_qvar_cmp(_tmp802->name,_tmp80A->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp804)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp80B))return 0;for(0;_tmp804 != 0;(_tmp804=
_tmp804->tl,_tmp80B=_tmp80B->tl)){if(!Cyc_Tcutil_unify((void*)_tmp804->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp80B))->hd))return 0;}return 1;_LL42D: _tmp80C=
_tmp7E8.f1;{struct Cyc_Absyn_FnType_struct*_tmp80D=(struct Cyc_Absyn_FnType_struct*)
_tmp80C;if(_tmp80D->tag != 10)goto _LL42F;else{_tmp80E=_tmp80D->f1;}};_tmp80F=
_tmp7E8.f2;{struct Cyc_Absyn_FnType_struct*_tmp810=(struct Cyc_Absyn_FnType_struct*)
_tmp80F;if(_tmp810->tag != 10)goto _LL42F;else{_tmp811=_tmp810->f1;}};_LL42E: if(
_tmp80E.tvars != 0  || _tmp811.tvars != 0){struct Cyc_List_List*_tmp820=_tmp80E.tvars;
struct Cyc_List_List*_tmp821=_tmp811.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp820)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp821))return 0;{
struct _RegionHandle*_tmp822=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp820 != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp820->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp821))->hd)->kind))return 0;{struct _tuple15*_tmp1279;struct Cyc_Absyn_VarType_struct
_tmp1278;struct Cyc_Absyn_VarType_struct*_tmp1277;struct Cyc_List_List*_tmp1276;
inst=((_tmp1276=_region_malloc(_tmp822,sizeof(*_tmp1276)),((_tmp1276->hd=((
_tmp1279=_region_malloc(_tmp822,sizeof(*_tmp1279)),((_tmp1279->f1=(struct Cyc_Absyn_Tvar*)
_tmp821->hd,((_tmp1279->f2=(void*)((_tmp1277=_cycalloc(sizeof(*_tmp1277)),((
_tmp1277[0]=((_tmp1278.tag=2,((_tmp1278.f1=(struct Cyc_Absyn_Tvar*)_tmp820->hd,
_tmp1278)))),_tmp1277)))),_tmp1279)))))),((_tmp1276->tl=inst,_tmp1276))))));}
_tmp820=_tmp820->tl;_tmp821=_tmp821->tl;}if(inst != 0){_tmp80E.tvars=0;_tmp811.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp127F;struct Cyc_Absyn_FnType_struct*_tmp127E;
struct Cyc_Absyn_FnType_struct _tmp127C;struct Cyc_Absyn_FnType_struct*_tmp127B;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp127B=_cycalloc(sizeof(*_tmp127B)),((
_tmp127B[0]=((_tmp127C.tag=10,((_tmp127C.f1=_tmp80E,_tmp127C)))),_tmp127B)))),(
void*)((_tmp127E=_cycalloc(sizeof(*_tmp127E)),((_tmp127E[0]=((_tmp127F.tag=10,((
_tmp127F.f1=_tmp811,_tmp127F)))),_tmp127E)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp80E.ret_typ,_tmp811.ret_typ))return 0;{struct Cyc_List_List*_tmp82B=
_tmp80E.args;struct Cyc_List_List*_tmp82C=_tmp811.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp82B)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp82C))return 0;for(0;_tmp82B != 0;(_tmp82B=_tmp82B->tl,_tmp82C=_tmp82C->tl)){
struct Cyc_Absyn_Tqual _tmp82E;void*_tmp82F;struct _tuple9 _tmp82D=*((struct _tuple9*)
_tmp82B->hd);_tmp82E=_tmp82D.f2;_tmp82F=_tmp82D.f3;{struct Cyc_Absyn_Tqual _tmp831;
void*_tmp832;struct _tuple9 _tmp830=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp82C))->hd);_tmp831=_tmp830.f2;_tmp832=_tmp830.f3;if(_tmp831.real_const
 && !_tmp82E.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp832,_tmp82F))return
0;};}if(_tmp80E.c_varargs != _tmp811.c_varargs)return 0;if(_tmp80E.cyc_varargs != 0
 && _tmp811.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp833=*_tmp80E.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp834=*_tmp811.cyc_varargs;if((_tmp834.tq).real_const
 && !(_tmp833.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp834.type,
_tmp833.type))return 0;}else{if(_tmp80E.cyc_varargs != 0  || _tmp811.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp80E.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp811.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp80E.rgn_po,_tmp811.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp80E.attributes,_tmp811.attributes))return 0;return 1;};_LL42F:;_LL430: return 0;
_LL428:;};}static int Cyc_Tcutil_isomorphic(void*t1,void*t2);static int Cyc_Tcutil_isomorphic(
void*t1,void*t2){struct _tuple0 _tmp1280;struct _tuple0 _tmp836=(_tmp1280.f1=Cyc_Tcutil_compress(
t1),((_tmp1280.f2=Cyc_Tcutil_compress(t2),_tmp1280)));void*_tmp837;enum Cyc_Absyn_Size_of
_tmp839;void*_tmp83A;enum Cyc_Absyn_Size_of _tmp83C;_LL439: _tmp837=_tmp836.f1;{
struct Cyc_Absyn_IntType_struct*_tmp838=(struct Cyc_Absyn_IntType_struct*)_tmp837;
if(_tmp838->tag != 6)goto _LL43B;else{_tmp839=_tmp838->f2;}};_tmp83A=_tmp836.f2;{
struct Cyc_Absyn_IntType_struct*_tmp83B=(struct Cyc_Absyn_IntType_struct*)_tmp83A;
if(_tmp83B->tag != 6)goto _LL43B;else{_tmp83C=_tmp83B->f2;}};_LL43A: return(_tmp839
== _tmp83C  || _tmp839 == Cyc_Absyn_Int_sz  && _tmp83C == Cyc_Absyn_Long_sz) || 
_tmp839 == Cyc_Absyn_Long_sz  && _tmp83C == Cyc_Absyn_Int_sz;_LL43B:;_LL43C: return 0;
_LL438:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2);static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct _RegionHandle*_tmp83D=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp83D,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp83D,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple11 _tmp83F;struct Cyc_Absyn_Tqual
_tmp840;void*_tmp841;struct _tuple11*_tmp83E=(struct _tuple11*)tqs1->hd;_tmp83F=*
_tmp83E;_tmp840=_tmp83F.f1;_tmp841=_tmp83F.f2;{struct _tuple11 _tmp843;struct Cyc_Absyn_Tqual
_tmp844;void*_tmp845;struct _tuple11*_tmp842=(struct _tuple11*)tqs2->hd;_tmp843=*
_tmp842;_tmp844=_tmp843.f1;_tmp845=_tmp843.f2;if(_tmp844.real_const  && Cyc_Tcutil_subtype(
te,assume,_tmp841,_tmp845))continue;else{if(Cyc_Tcutil_unify(_tmp841,_tmp845))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp841,_tmp845))continue;else{return 0;}}}};};}
return 1;}static int Cyc_Tcutil_is_char_type(void*t);static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp846=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmp848;_LL43E: {
struct Cyc_Absyn_IntType_struct*_tmp847=(struct Cyc_Absyn_IntType_struct*)_tmp846;
if(_tmp847->tag != 6)goto _LL440;else{_tmp848=_tmp847->f2;if(_tmp848 != Cyc_Absyn_Char_sz)
goto _LL440;}}_LL43F: return 1;_LL440:;_LL441: return 0;_LL43D:;}enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*
t1,void*t2);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);
if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*_tmp849=
t2;enum Cyc_Absyn_Size_of _tmp84B;enum Cyc_Absyn_Size_of _tmp84D;_LL443: {struct
Cyc_Absyn_IntType_struct*_tmp84A=(struct Cyc_Absyn_IntType_struct*)_tmp849;if(
_tmp84A->tag != 6)goto _LL445;else{_tmp84B=_tmp84A->f2;if(_tmp84B != Cyc_Absyn_Int_sz)
goto _LL445;}}_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_IntType_struct*_tmp84C=(
struct Cyc_Absyn_IntType_struct*)_tmp849;if(_tmp84C->tag != 6)goto _LL447;else{
_tmp84D=_tmp84C->f2;if(_tmp84D != Cyc_Absyn_Long_sz)goto _LL447;}}_LL446: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL442;_LL447:;
_LL448: goto _LL442;_LL442:;}{void*_tmp84E=t1;struct Cyc_Absyn_PtrInfo _tmp850;void*
_tmp851;struct Cyc_Absyn_Tqual _tmp852;struct Cyc_Absyn_PtrAtts _tmp853;void*_tmp854;
union Cyc_Absyn_Constraint*_tmp855;union Cyc_Absyn_Constraint*_tmp856;union Cyc_Absyn_Constraint*
_tmp857;struct Cyc_Absyn_ArrayInfo _tmp859;void*_tmp85A;struct Cyc_Absyn_Tqual
_tmp85B;struct Cyc_Absyn_Exp*_tmp85C;union Cyc_Absyn_Constraint*_tmp85D;struct Cyc_Absyn_Enumdecl*
_tmp85F;void*_tmp864;_LL44A: {struct Cyc_Absyn_PointerType_struct*_tmp84F=(struct
Cyc_Absyn_PointerType_struct*)_tmp84E;if(_tmp84F->tag != 5)goto _LL44C;else{
_tmp850=_tmp84F->f1;_tmp851=_tmp850.elt_typ;_tmp852=_tmp850.elt_tq;_tmp853=
_tmp850.ptr_atts;_tmp854=_tmp853.rgn;_tmp855=_tmp853.nullable;_tmp856=_tmp853.bounds;
_tmp857=_tmp853.zero_term;}}_LL44B:{void*_tmp865=t2;struct Cyc_Absyn_PtrInfo
_tmp867;void*_tmp868;struct Cyc_Absyn_Tqual _tmp869;struct Cyc_Absyn_PtrAtts _tmp86A;
void*_tmp86B;union Cyc_Absyn_Constraint*_tmp86C;union Cyc_Absyn_Constraint*_tmp86D;
union Cyc_Absyn_Constraint*_tmp86E;_LL45B: {struct Cyc_Absyn_PointerType_struct*
_tmp866=(struct Cyc_Absyn_PointerType_struct*)_tmp865;if(_tmp866->tag != 5)goto
_LL45D;else{_tmp867=_tmp866->f1;_tmp868=_tmp867.elt_typ;_tmp869=_tmp867.elt_tq;
_tmp86A=_tmp867.ptr_atts;_tmp86B=_tmp86A.rgn;_tmp86C=_tmp86A.nullable;_tmp86D=
_tmp86A.bounds;_tmp86E=_tmp86A.zero_term;}}_LL45C: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1283;struct Cyc_List_List*
_tmp1282;struct Cyc_List_List*_tmp86F=(_tmp1282=_cycalloc(sizeof(*_tmp1282)),((
_tmp1282->hd=((_tmp1283=_cycalloc(sizeof(*_tmp1283)),((_tmp1283->f1=t1,((
_tmp1283->f2=t2,_tmp1283)))))),((_tmp1282->tl=0,_tmp1282)))));int _tmp870=Cyc_Tcutil_ptrsubtype(
te,_tmp86F,_tmp851,_tmp868) && (!_tmp852.real_const  || _tmp869.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp857,
_tmp86E) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp86E);
int _tmp871=_tmp870?0:((Cyc_Tcutil_bits_only(_tmp851) && Cyc_Tcutil_is_char_type(
_tmp868)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp86E)) && (_tmp869.real_const  || !_tmp852.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp856,_tmp86D);if(!
bounds_ok  && !_tmp871){struct _tuple0 _tmp1284;struct _tuple0 _tmp873=(_tmp1284.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp856),((_tmp1284.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp86D),_tmp1284)));
void*_tmp874;struct Cyc_Absyn_Exp*_tmp876;void*_tmp877;struct Cyc_Absyn_Exp*
_tmp879;_LL460: _tmp874=_tmp873.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp875=(
struct Cyc_Absyn_Upper_b_struct*)_tmp874;if(_tmp875->tag != 1)goto _LL462;else{
_tmp876=_tmp875->f1;}};_tmp877=_tmp873.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp878=(struct Cyc_Absyn_Upper_b_struct*)_tmp877;if(_tmp878->tag != 1)goto _LL462;
else{_tmp879=_tmp878->f1;}};_LL461: if(Cyc_Evexp_lte_const_exp(_tmp879,_tmp876))
bounds_ok=1;goto _LL45F;_LL462:;_LL463: bounds_ok=1;goto _LL45F;_LL45F:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp855) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp86C))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp870  || _tmp871)) && (Cyc_Tcutil_unify(
_tmp854,_tmp86B) || Cyc_Tcenv_region_outlives(te,_tmp854,_tmp86B)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL45D:;_LL45E: goto _LL45A;
_LL45A:;}return Cyc_Absyn_Unknown_coercion;_LL44C: {struct Cyc_Absyn_ArrayType_struct*
_tmp858=(struct Cyc_Absyn_ArrayType_struct*)_tmp84E;if(_tmp858->tag != 9)goto
_LL44E;else{_tmp859=_tmp858->f1;_tmp85A=_tmp859.elt_type;_tmp85B=_tmp859.tq;
_tmp85C=_tmp859.num_elts;_tmp85D=_tmp859.zero_term;}}_LL44D:{void*_tmp87C=t2;
struct Cyc_Absyn_ArrayInfo _tmp87E;void*_tmp87F;struct Cyc_Absyn_Tqual _tmp880;
struct Cyc_Absyn_Exp*_tmp881;union Cyc_Absyn_Constraint*_tmp882;_LL465: {struct Cyc_Absyn_ArrayType_struct*
_tmp87D=(struct Cyc_Absyn_ArrayType_struct*)_tmp87C;if(_tmp87D->tag != 9)goto
_LL467;else{_tmp87E=_tmp87D->f1;_tmp87F=_tmp87E.elt_type;_tmp880=_tmp87E.tq;
_tmp881=_tmp87E.num_elts;_tmp882=_tmp87E.zero_term;}}_LL466: {int okay;okay=((
_tmp85C != 0  && _tmp881 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp85D,
_tmp882)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp881,(struct Cyc_Absyn_Exp*)
_tmp85C);return(okay  && Cyc_Tcutil_unify(_tmp85A,_tmp87F)) && (!_tmp85B.real_const
 || _tmp880.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL467:;_LL468: return Cyc_Absyn_Unknown_coercion;_LL464:;}return Cyc_Absyn_Unknown_coercion;
_LL44E: {struct Cyc_Absyn_EnumType_struct*_tmp85E=(struct Cyc_Absyn_EnumType_struct*)
_tmp84E;if(_tmp85E->tag != 14)goto _LL450;else{_tmp85F=_tmp85E->f2;}}_LL44F:{void*
_tmp883=t2;struct Cyc_Absyn_Enumdecl*_tmp885;_LL46A: {struct Cyc_Absyn_EnumType_struct*
_tmp884=(struct Cyc_Absyn_EnumType_struct*)_tmp883;if(_tmp884->tag != 14)goto
_LL46C;else{_tmp885=_tmp884->f2;}}_LL46B: if((_tmp85F->fields != 0  && _tmp885->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp85F->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp885->fields))->v))return Cyc_Absyn_No_coercion;goto _LL469;_LL46C:;_LL46D: goto
_LL469;_LL469:;}goto _LL451;_LL450: {struct Cyc_Absyn_IntType_struct*_tmp860=(
struct Cyc_Absyn_IntType_struct*)_tmp84E;if(_tmp860->tag != 6)goto _LL452;}_LL451:
goto _LL453;_LL452: {struct Cyc_Absyn_FloatType_struct*_tmp861=(struct Cyc_Absyn_FloatType_struct*)
_tmp84E;if(_tmp861->tag != 7)goto _LL454;}_LL453: goto _LL455;_LL454: {struct Cyc_Absyn_DoubleType_struct*
_tmp862=(struct Cyc_Absyn_DoubleType_struct*)_tmp84E;if(_tmp862->tag != 8)goto
_LL456;}_LL455: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL456: {struct Cyc_Absyn_RgnHandleType_struct*_tmp863=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp84E;if(_tmp863->tag != 16)goto _LL458;else{_tmp864=(void*)_tmp863->f1;}}_LL457:{
void*_tmp886=t2;void*_tmp888;_LL46F: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp887=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp886;if(_tmp887->tag != 16)goto
_LL471;else{_tmp888=(void*)_tmp887->f1;}}_LL470: if(Cyc_Tcenv_region_outlives(te,
_tmp864,_tmp888))return Cyc_Absyn_No_coercion;goto _LL46E;_LL471:;_LL472: goto
_LL46E;_LL46E:;}return Cyc_Absyn_Unknown_coercion;_LL458:;_LL459: return Cyc_Absyn_Unknown_coercion;
_LL449:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*
_tmp889=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Cast_e_struct _tmp1287;struct Cyc_Absyn_Cast_e_struct*
_tmp1286;e->r=(void*)((_tmp1286=_cycalloc(sizeof(*_tmp1286)),((_tmp1286[0]=((
_tmp1287.tag=15,((_tmp1287.f1=(void*)t,((_tmp1287.f2=_tmp889,((_tmp1287.f3=0,((
_tmp1287.f4=c,_tmp1287)))))))))),_tmp1286))));}{struct Cyc_Core_Opt*_tmp1288;e->topt=((
_tmp1288=_cycalloc(sizeof(*_tmp1288)),((_tmp1288->v=t,_tmp1288))));};}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*
_tmp88D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL474: {struct Cyc_Absyn_IntType_struct*_tmp88E=(struct Cyc_Absyn_IntType_struct*)
_tmp88D;if(_tmp88E->tag != 6)goto _LL476;}_LL475: goto _LL477;_LL476: {struct Cyc_Absyn_EnumType_struct*
_tmp88F=(struct Cyc_Absyn_EnumType_struct*)_tmp88D;if(_tmp88F->tag != 14)goto
_LL478;}_LL477: goto _LL479;_LL478: {struct Cyc_Absyn_AnonEnumType_struct*_tmp890=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp88D;if(_tmp890->tag != 15)goto _LL47A;}
_LL479: goto _LL47B;_LL47A: {struct Cyc_Absyn_TagType_struct*_tmp891=(struct Cyc_Absyn_TagType_struct*)
_tmp88D;if(_tmp891->tag != 20)goto _LL47C;}_LL47B: return 1;_LL47C: {struct Cyc_Absyn_Evar_struct*
_tmp892=(struct Cyc_Absyn_Evar_struct*)_tmp88D;if(_tmp892->tag != 1)goto _LL47E;}
_LL47D: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL47E:;_LL47F: return 0;_LL473:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp893=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL481: {struct Cyc_Absyn_FloatType_struct*_tmp894=(
struct Cyc_Absyn_FloatType_struct*)_tmp893;if(_tmp894->tag != 7)goto _LL483;}_LL482:
goto _LL484;_LL483: {struct Cyc_Absyn_DoubleType_struct*_tmp895=(struct Cyc_Absyn_DoubleType_struct*)
_tmp893;if(_tmp895->tag != 8)goto _LL485;}_LL484: return 1;_LL485:;_LL486: return 0;
_LL480:;};}int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_function_type(
void*t){void*_tmp896=Cyc_Tcutil_compress(t);_LL488: {struct Cyc_Absyn_FnType_struct*
_tmp897=(struct Cyc_Absyn_FnType_struct*)_tmp896;if(_tmp897->tag != 10)goto _LL48A;}
_LL489: return 1;_LL48A:;_LL48B: return 0;_LL487:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2);void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct
_tuple0 _tmp1289;struct _tuple0 _tmp899=(_tmp1289.f1=t1,((_tmp1289.f2=t2,_tmp1289)));
void*_tmp89A;int _tmp89C;void*_tmp89D;int _tmp89F;void*_tmp8A0;void*_tmp8A2;void*
_tmp8A4;void*_tmp8A6;void*_tmp8A8;enum Cyc_Absyn_Sign _tmp8AA;enum Cyc_Absyn_Size_of
_tmp8AB;void*_tmp8AC;enum Cyc_Absyn_Sign _tmp8AE;enum Cyc_Absyn_Size_of _tmp8AF;
void*_tmp8B0;enum Cyc_Absyn_Size_of _tmp8B2;void*_tmp8B3;enum Cyc_Absyn_Size_of
_tmp8B5;void*_tmp8B6;enum Cyc_Absyn_Sign _tmp8B8;enum Cyc_Absyn_Size_of _tmp8B9;
void*_tmp8BA;enum Cyc_Absyn_Sign _tmp8BC;enum Cyc_Absyn_Size_of _tmp8BD;void*
_tmp8BE;enum Cyc_Absyn_Sign _tmp8C0;enum Cyc_Absyn_Size_of _tmp8C1;void*_tmp8C2;
enum Cyc_Absyn_Sign _tmp8C4;enum Cyc_Absyn_Size_of _tmp8C5;void*_tmp8C6;enum Cyc_Absyn_Size_of
_tmp8C8;void*_tmp8C9;enum Cyc_Absyn_Size_of _tmp8CB;_LL48D: _tmp89A=_tmp899.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp89B=(struct Cyc_Absyn_DoubleType_struct*)
_tmp89A;if(_tmp89B->tag != 8)goto _LL48F;else{_tmp89C=_tmp89B->f1;}};_tmp89D=
_tmp899.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp89E=(struct Cyc_Absyn_DoubleType_struct*)
_tmp89D;if(_tmp89E->tag != 8)goto _LL48F;else{_tmp89F=_tmp89E->f1;}};_LL48E: if(
_tmp89C)return t1;else{return t2;}_LL48F: _tmp8A0=_tmp899.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp8A1=(struct Cyc_Absyn_DoubleType_struct*)_tmp8A0;if(_tmp8A1->tag != 8)goto
_LL491;};_LL490: return t1;_LL491: _tmp8A2=_tmp899.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp8A3=(struct Cyc_Absyn_DoubleType_struct*)_tmp8A2;if(_tmp8A3->tag != 8)goto
_LL493;};_LL492: return t2;_LL493: _tmp8A4=_tmp899.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp8A5=(struct Cyc_Absyn_FloatType_struct*)_tmp8A4;if(_tmp8A5->tag != 7)goto
_LL495;};_LL494: goto _LL496;_LL495: _tmp8A6=_tmp899.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp8A7=(struct Cyc_Absyn_FloatType_struct*)_tmp8A6;if(_tmp8A7->tag != 7)goto
_LL497;};_LL496: return(void*)& Cyc_Absyn_FloatType_val;_LL497: _tmp8A8=_tmp899.f1;{
struct Cyc_Absyn_IntType_struct*_tmp8A9=(struct Cyc_Absyn_IntType_struct*)_tmp8A8;
if(_tmp8A9->tag != 6)goto _LL499;else{_tmp8AA=_tmp8A9->f1;if(_tmp8AA != Cyc_Absyn_Unsigned)
goto _LL499;_tmp8AB=_tmp8A9->f2;if(_tmp8AB != Cyc_Absyn_LongLong_sz)goto _LL499;}};
_LL498: goto _LL49A;_LL499: _tmp8AC=_tmp899.f2;{struct Cyc_Absyn_IntType_struct*
_tmp8AD=(struct Cyc_Absyn_IntType_struct*)_tmp8AC;if(_tmp8AD->tag != 6)goto _LL49B;
else{_tmp8AE=_tmp8AD->f1;if(_tmp8AE != Cyc_Absyn_Unsigned)goto _LL49B;_tmp8AF=
_tmp8AD->f2;if(_tmp8AF != Cyc_Absyn_LongLong_sz)goto _LL49B;}};_LL49A: return Cyc_Absyn_ulonglong_typ;
_LL49B: _tmp8B0=_tmp899.f1;{struct Cyc_Absyn_IntType_struct*_tmp8B1=(struct Cyc_Absyn_IntType_struct*)
_tmp8B0;if(_tmp8B1->tag != 6)goto _LL49D;else{_tmp8B2=_tmp8B1->f2;if(_tmp8B2 != Cyc_Absyn_LongLong_sz)
goto _LL49D;}};_LL49C: goto _LL49E;_LL49D: _tmp8B3=_tmp899.f2;{struct Cyc_Absyn_IntType_struct*
_tmp8B4=(struct Cyc_Absyn_IntType_struct*)_tmp8B3;if(_tmp8B4->tag != 6)goto _LL49F;
else{_tmp8B5=_tmp8B4->f2;if(_tmp8B5 != Cyc_Absyn_LongLong_sz)goto _LL49F;}};_LL49E:
return Cyc_Absyn_slonglong_typ;_LL49F: _tmp8B6=_tmp899.f1;{struct Cyc_Absyn_IntType_struct*
_tmp8B7=(struct Cyc_Absyn_IntType_struct*)_tmp8B6;if(_tmp8B7->tag != 6)goto _LL4A1;
else{_tmp8B8=_tmp8B7->f1;if(_tmp8B8 != Cyc_Absyn_Unsigned)goto _LL4A1;_tmp8B9=
_tmp8B7->f2;if(_tmp8B9 != Cyc_Absyn_Long_sz)goto _LL4A1;}};_LL4A0: goto _LL4A2;
_LL4A1: _tmp8BA=_tmp899.f2;{struct Cyc_Absyn_IntType_struct*_tmp8BB=(struct Cyc_Absyn_IntType_struct*)
_tmp8BA;if(_tmp8BB->tag != 6)goto _LL4A3;else{_tmp8BC=_tmp8BB->f1;if(_tmp8BC != Cyc_Absyn_Unsigned)
goto _LL4A3;_tmp8BD=_tmp8BB->f2;if(_tmp8BD != Cyc_Absyn_Long_sz)goto _LL4A3;}};
_LL4A2: return Cyc_Absyn_ulong_typ;_LL4A3: _tmp8BE=_tmp899.f1;{struct Cyc_Absyn_IntType_struct*
_tmp8BF=(struct Cyc_Absyn_IntType_struct*)_tmp8BE;if(_tmp8BF->tag != 6)goto _LL4A5;
else{_tmp8C0=_tmp8BF->f1;if(_tmp8C0 != Cyc_Absyn_Unsigned)goto _LL4A5;_tmp8C1=
_tmp8BF->f2;if(_tmp8C1 != Cyc_Absyn_Int_sz)goto _LL4A5;}};_LL4A4: goto _LL4A6;_LL4A5:
_tmp8C2=_tmp899.f2;{struct Cyc_Absyn_IntType_struct*_tmp8C3=(struct Cyc_Absyn_IntType_struct*)
_tmp8C2;if(_tmp8C3->tag != 6)goto _LL4A7;else{_tmp8C4=_tmp8C3->f1;if(_tmp8C4 != Cyc_Absyn_Unsigned)
goto _LL4A7;_tmp8C5=_tmp8C3->f2;if(_tmp8C5 != Cyc_Absyn_Int_sz)goto _LL4A7;}};
_LL4A6: return Cyc_Absyn_uint_typ;_LL4A7: _tmp8C6=_tmp899.f1;{struct Cyc_Absyn_IntType_struct*
_tmp8C7=(struct Cyc_Absyn_IntType_struct*)_tmp8C6;if(_tmp8C7->tag != 6)goto _LL4A9;
else{_tmp8C8=_tmp8C7->f2;if(_tmp8C8 != Cyc_Absyn_Long_sz)goto _LL4A9;}};_LL4A8:
goto _LL4AA;_LL4A9: _tmp8C9=_tmp899.f2;{struct Cyc_Absyn_IntType_struct*_tmp8CA=(
struct Cyc_Absyn_IntType_struct*)_tmp8C9;if(_tmp8CA->tag != 6)goto _LL4AB;else{
_tmp8CB=_tmp8CA->f2;if(_tmp8CB != Cyc_Absyn_Long_sz)goto _LL4AB;}};_LL4AA: return
Cyc_Absyn_slong_typ;_LL4AB:;_LL4AC: return Cyc_Absyn_sint_typ;_LL48C:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*
e){void*_tmp8CC=e->r;struct Cyc_Core_Opt*_tmp8CE;_LL4AE: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp8CD=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp8CC;if(_tmp8CD->tag != 4)goto
_LL4B0;else{_tmp8CE=_tmp8CD->f2;if(_tmp8CE != 0)goto _LL4B0;}}_LL4AF:{const char*
_tmp128C;void*_tmp128B;(_tmp128B=0,Cyc_Tcutil_warn(e->loc,((_tmp128C="assignment in test",
_tag_dyneither(_tmp128C,sizeof(char),19))),_tag_dyneither(_tmp128B,sizeof(void*),
0)));}goto _LL4AD;_LL4B0:;_LL4B1: goto _LL4AD;_LL4AD:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2);static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp128D;struct _tuple0 _tmp8D2=(
_tmp128D.f1=c1,((_tmp128D.f2=c2,_tmp128D)));void*_tmp8D3;struct Cyc_Absyn_Kind*
_tmp8D5;void*_tmp8D6;struct Cyc_Absyn_Kind*_tmp8D8;void*_tmp8D9;struct Cyc_Core_Opt*
_tmp8DB;struct Cyc_Core_Opt**_tmp8DC;void*_tmp8DD;struct Cyc_Core_Opt*_tmp8DF;
struct Cyc_Core_Opt**_tmp8E0;void*_tmp8E1;struct Cyc_Core_Opt*_tmp8E3;struct Cyc_Core_Opt**
_tmp8E4;struct Cyc_Absyn_Kind*_tmp8E5;void*_tmp8E6;struct Cyc_Absyn_Kind*_tmp8E8;
void*_tmp8E9;struct Cyc_Absyn_Kind*_tmp8EB;void*_tmp8EC;struct Cyc_Core_Opt*
_tmp8EE;struct Cyc_Core_Opt**_tmp8EF;struct Cyc_Absyn_Kind*_tmp8F0;void*_tmp8F1;
struct Cyc_Core_Opt*_tmp8F3;struct Cyc_Core_Opt**_tmp8F4;struct Cyc_Absyn_Kind*
_tmp8F5;void*_tmp8F6;struct Cyc_Core_Opt*_tmp8F8;struct Cyc_Core_Opt**_tmp8F9;
struct Cyc_Absyn_Kind*_tmp8FA;_LL4B3: _tmp8D3=_tmp8D2.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp8D4=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8D3;if(_tmp8D4->tag != 0)goto _LL4B5;
else{_tmp8D5=_tmp8D4->f1;}};_tmp8D6=_tmp8D2.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp8D7=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8D6;if(_tmp8D7->tag != 0)goto _LL4B5;
else{_tmp8D8=_tmp8D7->f1;}};_LL4B4: return _tmp8D5 == _tmp8D8;_LL4B5: _tmp8D9=
_tmp8D2.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp8DA=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8D9;if(_tmp8DA->tag != 1)goto _LL4B7;else{_tmp8DB=_tmp8DA->f1;_tmp8DC=(struct
Cyc_Core_Opt**)& _tmp8DA->f1;}};_LL4B6:{struct Cyc_Core_Opt*_tmp128E;*_tmp8DC=((
_tmp128E=_cycalloc(sizeof(*_tmp128E)),((_tmp128E->v=c1,_tmp128E))));}return 1;
_LL4B7: _tmp8DD=_tmp8D2.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp8DE=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8DD;if(_tmp8DE->tag != 1)goto _LL4B9;else{_tmp8DF=_tmp8DE->f1;_tmp8E0=(struct
Cyc_Core_Opt**)& _tmp8DE->f1;}};_LL4B8:{struct Cyc_Core_Opt*_tmp128F;*_tmp8E0=((
_tmp128F=_cycalloc(sizeof(*_tmp128F)),((_tmp128F->v=c2,_tmp128F))));}return 1;
_LL4B9: _tmp8E1=_tmp8D2.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp8E2=(struct Cyc_Absyn_Less_kb_struct*)
_tmp8E1;if(_tmp8E2->tag != 2)goto _LL4BB;else{_tmp8E3=_tmp8E2->f1;_tmp8E4=(struct
Cyc_Core_Opt**)& _tmp8E2->f1;_tmp8E5=_tmp8E2->f2;}};_tmp8E6=_tmp8D2.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp8E7=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8E6;if(_tmp8E7->tag != 0)goto _LL4BB;
else{_tmp8E8=_tmp8E7->f1;}};_LL4BA: if(Cyc_Tcutil_kind_leq(_tmp8E8,_tmp8E5)){{
struct Cyc_Core_Opt*_tmp1290;*_tmp8E4=((_tmp1290=_cycalloc(sizeof(*_tmp1290)),((
_tmp1290->v=c2,_tmp1290))));}return 1;}else{return 0;}_LL4BB: _tmp8E9=_tmp8D2.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp8EA=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8E9;if(
_tmp8EA->tag != 0)goto _LL4BD;else{_tmp8EB=_tmp8EA->f1;}};_tmp8EC=_tmp8D2.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp8ED=(struct Cyc_Absyn_Less_kb_struct*)_tmp8EC;
if(_tmp8ED->tag != 2)goto _LL4BD;else{_tmp8EE=_tmp8ED->f1;_tmp8EF=(struct Cyc_Core_Opt**)&
_tmp8ED->f1;_tmp8F0=_tmp8ED->f2;}};_LL4BC: if(Cyc_Tcutil_kind_leq(_tmp8EB,_tmp8F0)){{
struct Cyc_Core_Opt*_tmp1291;*_tmp8EF=((_tmp1291=_cycalloc(sizeof(*_tmp1291)),((
_tmp1291->v=c1,_tmp1291))));}return 1;}else{return 0;}_LL4BD: _tmp8F1=_tmp8D2.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp8F2=(struct Cyc_Absyn_Less_kb_struct*)_tmp8F1;
if(_tmp8F2->tag != 2)goto _LL4B2;else{_tmp8F3=_tmp8F2->f1;_tmp8F4=(struct Cyc_Core_Opt**)&
_tmp8F2->f1;_tmp8F5=_tmp8F2->f2;}};_tmp8F6=_tmp8D2.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp8F7=(struct Cyc_Absyn_Less_kb_struct*)_tmp8F6;if(_tmp8F7->tag != 2)goto _LL4B2;
else{_tmp8F8=_tmp8F7->f1;_tmp8F9=(struct Cyc_Core_Opt**)& _tmp8F7->f1;_tmp8FA=
_tmp8F7->f2;}};_LL4BE: if(Cyc_Tcutil_kind_leq(_tmp8F5,_tmp8FA)){{struct Cyc_Core_Opt*
_tmp1292;*_tmp8F9=((_tmp1292=_cycalloc(sizeof(*_tmp1292)),((_tmp1292->v=c1,
_tmp1292))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp8FA,_tmp8F5)){{struct Cyc_Core_Opt*
_tmp1293;*_tmp8F4=((_tmp1293=_cycalloc(sizeof(*_tmp1293)),((_tmp1293->v=c2,
_tmp1293))));}return 1;}else{return 0;}}_LL4B2:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp1297;void*_tmp1296[1];struct Cyc_Int_pa_struct _tmp1295;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp1295.tag=1,((_tmp1295.f1=(
unsigned long)i,((_tmp1296[0]=& _tmp1295,Cyc_aprintf(((_tmp1297="#%d",
_tag_dyneither(_tmp1297,sizeof(char),4))),_tag_dyneither(_tmp1296,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp129A;struct Cyc_Absyn_Tvar*_tmp1299;return(
_tmp1299=_cycalloc(sizeof(*_tmp1299)),((_tmp1299->name=((_tmp129A=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp129A[0]=s,_tmp129A)))),((_tmp1299->identity=
- 1,((_tmp1299->kind=k,_tmp1299)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr
_tmp906=*t->name;return*((const char*)_check_dyneither_subscript(_tmp906,sizeof(
char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t);void
Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){{const char*_tmp129E;void*
_tmp129D[1];struct Cyc_String_pa_struct _tmp129C;(_tmp129C.tag=0,((_tmp129C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp129D[0]=& _tmp129C,
Cyc_printf(((_tmp129E="%s",_tag_dyneither(_tmp129E,sizeof(char),3))),
_tag_dyneither(_tmp129D,sizeof(void*),1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))
return;{const char*_tmp129F;struct _dyneither_ptr _tmp90A=Cyc_strconcat(((_tmp129F="`",
_tag_dyneither(_tmp129F,sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char
_tmp12A2;char _tmp12A1;struct _dyneither_ptr _tmp12A0;(_tmp12A0=_dyneither_ptr_plus(
_tmp90A,sizeof(char),1),((_tmp12A1=*((char*)_check_dyneither_subscript(_tmp12A0,
sizeof(char),0)),((_tmp12A2='t',((_get_dyneither_size(_tmp12A0,sizeof(char))== 1
 && (_tmp12A1 == '\000'  && _tmp12A2 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp12A0.curr)=_tmp12A2)))))));}{struct _dyneither_ptr*_tmp12A3;t->name=((
_tmp12A3=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp12A3[0]=(struct
_dyneither_ptr)_tmp90A,_tmp12A3))));};};}struct _tuple23{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(
struct _tuple23*x);static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple23*x){
struct Cyc_Core_Opt*_tmp12A6;struct _tuple9*_tmp12A5;return(_tmp12A5=_cycalloc(
sizeof(*_tmp12A5)),((_tmp12A5->f1=(struct Cyc_Core_Opt*)((_tmp12A6=_cycalloc(
sizeof(*_tmp12A6)),((_tmp12A6->v=(*x).f1,_tmp12A6)))),((_tmp12A5->f2=(*x).f2,((
_tmp12A5->f3=(*x).f3,_tmp12A5)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*
fd);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){
struct Cyc_List_List*_tmp912=0;{struct Cyc_List_List*atts=fd->attributes;for(0;
atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct Cyc_List_List*
_tmp12A7;_tmp912=((_tmp12A7=_cycalloc(sizeof(*_tmp12A7)),((_tmp12A7->hd=(void*)
atts->hd,((_tmp12A7->tl=_tmp912,_tmp12A7))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp12AD;struct Cyc_Absyn_FnInfo _tmp12AC;struct Cyc_Absyn_FnType_struct*_tmp12AB;
return(void*)((_tmp12AB=_cycalloc(sizeof(*_tmp12AB)),((_tmp12AB[0]=((_tmp12AD.tag=
10,((_tmp12AD.f1=((_tmp12AC.tvars=fd->tvs,((_tmp12AC.effect=fd->effect,((
_tmp12AC.ret_typ=fd->ret_type,((_tmp12AC.args=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple23*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmp12AC.c_varargs=fd->c_varargs,((_tmp12AC.cyc_varargs=fd->cyc_varargs,((
_tmp12AC.rgn_po=fd->rgn_po,((_tmp12AC.attributes=_tmp912,_tmp12AC)))))))))))))))),
_tmp12AD)))),_tmp12AB))));};}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple24{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple24*t);static void*Cyc_Tcutil_fst_fdarg(struct _tuple24*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple11*t);void*Cyc_Tcutil_snd_tqt(struct _tuple11*
t){return(*t).f2;}static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*
t);static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){struct
_tuple11*_tmp12AE;return(_tmp12AE=_cycalloc(sizeof(*_tmp12AE)),((_tmp12AE->f1=(*
pr).f1,((_tmp12AE->f2=t,_tmp12AE)))));}struct _tuple25{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple26{struct _tuple25*f1;void*f2;};static
struct _tuple26*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y);
static struct _tuple26*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct
_tuple9*y){struct _tuple25*_tmp12B1;struct _tuple26*_tmp12B0;return(_tmp12B0=
_region_malloc(rgn,sizeof(*_tmp12B0)),((_tmp12B0->f1=((_tmp12B1=_region_malloc(
rgn,sizeof(*_tmp12B1)),((_tmp12B1->f1=(*y).f1,((_tmp12B1->f2=(*y).f2,_tmp12B1)))))),((
_tmp12B0->f2=(*y).f3,_tmp12B0)))));}static struct _tuple9*Cyc_Tcutil_substitute_f2(
struct _tuple26*w);static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple26*w){
struct _tuple25*_tmp91B;void*_tmp91C;struct _tuple26 _tmp91A=*w;_tmp91B=_tmp91A.f1;
_tmp91C=_tmp91A.f2;{struct Cyc_Core_Opt*_tmp91E;struct Cyc_Absyn_Tqual _tmp91F;
struct _tuple25 _tmp91D=*_tmp91B;_tmp91E=_tmp91D.f1;_tmp91F=_tmp91D.f2;{struct
_tuple9*_tmp12B2;return(_tmp12B2=_cycalloc(sizeof(*_tmp12B2)),((_tmp12B2->f1=
_tmp91E,((_tmp12B2->f2=_tmp91F,((_tmp12B2->f3=_tmp91C,_tmp12B2)))))));};};}
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f);static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmp12B3;return(
_tmp12B3=_cycalloc(sizeof(*_tmp12B3)),((_tmp12B3->name=f->name,((_tmp12B3->tq=f->tq,((
_tmp12B3->type=t,((_tmp12B3->width=f->width,((_tmp12B3->attributes=f->attributes,
_tmp12B3)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmp12B4;
return(_tmp12B4=_cycalloc(sizeof(*_tmp12B4)),((_tmp12B4->topt=old->topt,((
_tmp12B4->r=r,((_tmp12B4->loc=old->loc,((_tmp12B4->annot=old->annot,_tmp12B4)))))))));}
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct
_RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){void*_tmp923=e->r;
enum Cyc_Absyn_Primop _tmp929;struct Cyc_List_List*_tmp92A;struct Cyc_Absyn_Exp*
_tmp92C;struct Cyc_Absyn_Exp*_tmp92D;struct Cyc_Absyn_Exp*_tmp92E;struct Cyc_Absyn_Exp*
_tmp930;struct Cyc_Absyn_Exp*_tmp931;struct Cyc_Absyn_Exp*_tmp933;struct Cyc_Absyn_Exp*
_tmp934;struct Cyc_Absyn_Exp*_tmp936;struct Cyc_Absyn_Exp*_tmp937;void*_tmp939;
struct Cyc_Absyn_Exp*_tmp93A;int _tmp93B;enum Cyc_Absyn_Coercion _tmp93C;void*
_tmp93E;struct Cyc_Absyn_Exp*_tmp940;void*_tmp942;void*_tmp943;void*_tmp945;
_LL4C0: {struct Cyc_Absyn_Const_e_struct*_tmp924=(struct Cyc_Absyn_Const_e_struct*)
_tmp923;if(_tmp924->tag != 0)goto _LL4C2;}_LL4C1: goto _LL4C3;_LL4C2: {struct Cyc_Absyn_Enum_e_struct*
_tmp925=(struct Cyc_Absyn_Enum_e_struct*)_tmp923;if(_tmp925->tag != 32)goto _LL4C4;}
_LL4C3: goto _LL4C5;_LL4C4: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp926=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp923;if(_tmp926->tag != 33)goto _LL4C6;}_LL4C5: goto _LL4C7;_LL4C6: {struct Cyc_Absyn_Var_e_struct*
_tmp927=(struct Cyc_Absyn_Var_e_struct*)_tmp923;if(_tmp927->tag != 1)goto _LL4C8;}
_LL4C7: return e;_LL4C8: {struct Cyc_Absyn_Primop_e_struct*_tmp928=(struct Cyc_Absyn_Primop_e_struct*)
_tmp923;if(_tmp928->tag != 3)goto _LL4CA;else{_tmp929=_tmp928->f1;_tmp92A=_tmp928->f2;}}
_LL4C9: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp92A)== 1){struct
Cyc_Absyn_Exp*_tmp946=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp92A))->hd;struct Cyc_Absyn_Exp*_tmp947=Cyc_Tcutil_rsubsexp(r,inst,_tmp946);
if(_tmp947 == _tmp946)return e;{struct Cyc_Absyn_Primop_e_struct _tmp12BA;struct Cyc_Absyn_Exp*
_tmp12B9[1];struct Cyc_Absyn_Primop_e_struct*_tmp12B8;return Cyc_Tcutil_copye(e,(
void*)((_tmp12B8=_cycalloc(sizeof(*_tmp12B8)),((_tmp12B8[0]=((_tmp12BA.tag=3,((
_tmp12BA.f1=_tmp929,((_tmp12BA.f2=((_tmp12B9[0]=_tmp947,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp12B9,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp12BA)))))),_tmp12B8)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp92A)== 2){struct Cyc_Absyn_Exp*_tmp94B=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp92A))->hd;struct Cyc_Absyn_Exp*_tmp94C=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp92A->tl))->hd;struct
Cyc_Absyn_Exp*_tmp94D=Cyc_Tcutil_rsubsexp(r,inst,_tmp94B);struct Cyc_Absyn_Exp*
_tmp94E=Cyc_Tcutil_rsubsexp(r,inst,_tmp94C);if(_tmp94D == _tmp94B  && _tmp94E == 
_tmp94C)return e;{struct Cyc_Absyn_Primop_e_struct _tmp12C0;struct Cyc_Absyn_Exp*
_tmp12BF[2];struct Cyc_Absyn_Primop_e_struct*_tmp12BE;return Cyc_Tcutil_copye(e,(
void*)((_tmp12BE=_cycalloc(sizeof(*_tmp12BE)),((_tmp12BE[0]=((_tmp12C0.tag=3,((
_tmp12C0.f1=_tmp929,((_tmp12C0.f2=((_tmp12BF[1]=_tmp94E,((_tmp12BF[0]=_tmp94D,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp12BF,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp12C0)))))),_tmp12BE)))));};}
else{const char*_tmp12C3;void*_tmp12C2;return(_tmp12C2=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C3="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp12C3,sizeof(char),34))),_tag_dyneither(_tmp12C2,sizeof(void*),
0)));}}_LL4CA: {struct Cyc_Absyn_Conditional_e_struct*_tmp92B=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp923;if(_tmp92B->tag != 6)goto _LL4CC;else{_tmp92C=_tmp92B->f1;_tmp92D=_tmp92B->f2;
_tmp92E=_tmp92B->f3;}}_LL4CB: {struct Cyc_Absyn_Exp*_tmp954=Cyc_Tcutil_rsubsexp(r,
inst,_tmp92C);struct Cyc_Absyn_Exp*_tmp955=Cyc_Tcutil_rsubsexp(r,inst,_tmp92D);
struct Cyc_Absyn_Exp*_tmp956=Cyc_Tcutil_rsubsexp(r,inst,_tmp92E);if((_tmp954 == 
_tmp92C  && _tmp955 == _tmp92D) && _tmp956 == _tmp92E)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp12C6;struct Cyc_Absyn_Conditional_e_struct*_tmp12C5;return Cyc_Tcutil_copye(e,(
void*)((_tmp12C5=_cycalloc(sizeof(*_tmp12C5)),((_tmp12C5[0]=((_tmp12C6.tag=6,((
_tmp12C6.f1=_tmp954,((_tmp12C6.f2=_tmp955,((_tmp12C6.f3=_tmp956,_tmp12C6)))))))),
_tmp12C5)))));};}_LL4CC: {struct Cyc_Absyn_And_e_struct*_tmp92F=(struct Cyc_Absyn_And_e_struct*)
_tmp923;if(_tmp92F->tag != 7)goto _LL4CE;else{_tmp930=_tmp92F->f1;_tmp931=_tmp92F->f2;}}
_LL4CD: {struct Cyc_Absyn_Exp*_tmp959=Cyc_Tcutil_rsubsexp(r,inst,_tmp930);struct
Cyc_Absyn_Exp*_tmp95A=Cyc_Tcutil_rsubsexp(r,inst,_tmp931);if(_tmp959 == _tmp930
 && _tmp95A == _tmp931)return e;{struct Cyc_Absyn_And_e_struct _tmp12C9;struct Cyc_Absyn_And_e_struct*
_tmp12C8;return Cyc_Tcutil_copye(e,(void*)((_tmp12C8=_cycalloc(sizeof(*_tmp12C8)),((
_tmp12C8[0]=((_tmp12C9.tag=7,((_tmp12C9.f1=_tmp959,((_tmp12C9.f2=_tmp95A,
_tmp12C9)))))),_tmp12C8)))));};}_LL4CE: {struct Cyc_Absyn_Or_e_struct*_tmp932=(
struct Cyc_Absyn_Or_e_struct*)_tmp923;if(_tmp932->tag != 8)goto _LL4D0;else{_tmp933=
_tmp932->f1;_tmp934=_tmp932->f2;}}_LL4CF: {struct Cyc_Absyn_Exp*_tmp95D=Cyc_Tcutil_rsubsexp(
r,inst,_tmp933);struct Cyc_Absyn_Exp*_tmp95E=Cyc_Tcutil_rsubsexp(r,inst,_tmp934);
if(_tmp95D == _tmp933  && _tmp95E == _tmp934)return e;{struct Cyc_Absyn_Or_e_struct
_tmp12CC;struct Cyc_Absyn_Or_e_struct*_tmp12CB;return Cyc_Tcutil_copye(e,(void*)((
_tmp12CB=_cycalloc(sizeof(*_tmp12CB)),((_tmp12CB[0]=((_tmp12CC.tag=8,((_tmp12CC.f1=
_tmp95D,((_tmp12CC.f2=_tmp95E,_tmp12CC)))))),_tmp12CB)))));};}_LL4D0: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp935=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp923;if(_tmp935->tag != 9)goto _LL4D2;
else{_tmp936=_tmp935->f1;_tmp937=_tmp935->f2;}}_LL4D1: {struct Cyc_Absyn_Exp*
_tmp961=Cyc_Tcutil_rsubsexp(r,inst,_tmp936);struct Cyc_Absyn_Exp*_tmp962=Cyc_Tcutil_rsubsexp(
r,inst,_tmp937);if(_tmp961 == _tmp936  && _tmp962 == _tmp937)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp12CF;struct Cyc_Absyn_SeqExp_e_struct*_tmp12CE;return Cyc_Tcutil_copye(e,(void*)((
_tmp12CE=_cycalloc(sizeof(*_tmp12CE)),((_tmp12CE[0]=((_tmp12CF.tag=9,((_tmp12CF.f1=
_tmp961,((_tmp12CF.f2=_tmp962,_tmp12CF)))))),_tmp12CE)))));};}_LL4D2: {struct Cyc_Absyn_Cast_e_struct*
_tmp938=(struct Cyc_Absyn_Cast_e_struct*)_tmp923;if(_tmp938->tag != 15)goto _LL4D4;
else{_tmp939=(void*)_tmp938->f1;_tmp93A=_tmp938->f2;_tmp93B=_tmp938->f3;_tmp93C=
_tmp938->f4;}}_LL4D3: {struct Cyc_Absyn_Exp*_tmp965=Cyc_Tcutil_rsubsexp(r,inst,
_tmp93A);void*_tmp966=Cyc_Tcutil_rsubstitute(r,inst,_tmp939);if(_tmp965 == 
_tmp93A  && _tmp966 == _tmp939)return e;{struct Cyc_Absyn_Cast_e_struct _tmp12D2;
struct Cyc_Absyn_Cast_e_struct*_tmp12D1;return Cyc_Tcutil_copye(e,(void*)((
_tmp12D1=_cycalloc(sizeof(*_tmp12D1)),((_tmp12D1[0]=((_tmp12D2.tag=15,((_tmp12D2.f1=(
void*)_tmp966,((_tmp12D2.f2=_tmp965,((_tmp12D2.f3=_tmp93B,((_tmp12D2.f4=_tmp93C,
_tmp12D2)))))))))),_tmp12D1)))));};}_LL4D4: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp93D=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp923;if(_tmp93D->tag != 18)goto
_LL4D6;else{_tmp93E=(void*)_tmp93D->f1;}}_LL4D5: {void*_tmp969=Cyc_Tcutil_rsubstitute(
r,inst,_tmp93E);if(_tmp969 == _tmp93E)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp12D5;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp12D4;return Cyc_Tcutil_copye(e,(
void*)((_tmp12D4=_cycalloc(sizeof(*_tmp12D4)),((_tmp12D4[0]=((_tmp12D5.tag=18,((
_tmp12D5.f1=(void*)_tmp969,_tmp12D5)))),_tmp12D4)))));};}_LL4D6: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp93F=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp923;if(_tmp93F->tag != 19)goto
_LL4D8;else{_tmp940=_tmp93F->f1;}}_LL4D7: {struct Cyc_Absyn_Exp*_tmp96C=Cyc_Tcutil_rsubsexp(
r,inst,_tmp940);if(_tmp96C == _tmp940)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp12D8;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp12D7;return Cyc_Tcutil_copye(e,(
void*)((_tmp12D7=_cycalloc(sizeof(*_tmp12D7)),((_tmp12D7[0]=((_tmp12D8.tag=19,((
_tmp12D8.f1=_tmp96C,_tmp12D8)))),_tmp12D7)))));};}_LL4D8: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp941=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp923;if(_tmp941->tag != 20)goto
_LL4DA;else{_tmp942=(void*)_tmp941->f1;_tmp943=(void*)_tmp941->f2;}}_LL4D9: {
void*_tmp96F=Cyc_Tcutil_rsubstitute(r,inst,_tmp942);if(_tmp96F == _tmp942)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp12DB;struct Cyc_Absyn_Offsetof_e_struct*
_tmp12DA;return Cyc_Tcutil_copye(e,(void*)((_tmp12DA=_cycalloc(sizeof(*_tmp12DA)),((
_tmp12DA[0]=((_tmp12DB.tag=20,((_tmp12DB.f1=(void*)_tmp96F,((_tmp12DB.f2=(void*)
_tmp943,_tmp12DB)))))),_tmp12DA)))));};}_LL4DA: {struct Cyc_Absyn_Valueof_e_struct*
_tmp944=(struct Cyc_Absyn_Valueof_e_struct*)_tmp923;if(_tmp944->tag != 39)goto
_LL4DC;else{_tmp945=(void*)_tmp944->f1;}}_LL4DB: {void*_tmp972=Cyc_Tcutil_rsubstitute(
r,inst,_tmp945);if(_tmp972 == _tmp945)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp12DE;struct Cyc_Absyn_Valueof_e_struct*_tmp12DD;return Cyc_Tcutil_copye(e,(
void*)((_tmp12DD=_cycalloc(sizeof(*_tmp12DD)),((_tmp12DD[0]=((_tmp12DE.tag=39,((
_tmp12DE.f1=(void*)_tmp972,_tmp12DE)))),_tmp12DD)))));};}_LL4DC:;_LL4DD: {const
char*_tmp12E1;void*_tmp12E0;return(_tmp12E0=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E1="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp12E1,sizeof(char),46))),_tag_dyneither(_tmp12E0,sizeof(void*),
0)));}_LL4BF:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp977=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp979;
struct Cyc_Absyn_AggrInfo _tmp97B;union Cyc_Absyn_AggrInfoU _tmp97C;struct Cyc_List_List*
_tmp97D;struct Cyc_Absyn_DatatypeInfo _tmp97F;union Cyc_Absyn_DatatypeInfoU _tmp980;
struct Cyc_List_List*_tmp981;struct Cyc_Absyn_DatatypeFieldInfo _tmp983;union Cyc_Absyn_DatatypeFieldInfoU
_tmp984;struct Cyc_List_List*_tmp985;struct _tuple2*_tmp987;struct Cyc_List_List*
_tmp988;struct Cyc_Absyn_Typedefdecl*_tmp989;void**_tmp98A;struct Cyc_Absyn_ArrayInfo
_tmp98C;void*_tmp98D;struct Cyc_Absyn_Tqual _tmp98E;struct Cyc_Absyn_Exp*_tmp98F;
union Cyc_Absyn_Constraint*_tmp990;struct Cyc_Position_Segment*_tmp991;struct Cyc_Absyn_PtrInfo
_tmp993;void*_tmp994;struct Cyc_Absyn_Tqual _tmp995;struct Cyc_Absyn_PtrAtts _tmp996;
void*_tmp997;union Cyc_Absyn_Constraint*_tmp998;union Cyc_Absyn_Constraint*_tmp999;
union Cyc_Absyn_Constraint*_tmp99A;struct Cyc_Absyn_FnInfo _tmp99C;struct Cyc_List_List*
_tmp99D;struct Cyc_Core_Opt*_tmp99E;void*_tmp99F;struct Cyc_List_List*_tmp9A0;int
_tmp9A1;struct Cyc_Absyn_VarargInfo*_tmp9A2;struct Cyc_List_List*_tmp9A3;struct Cyc_List_List*
_tmp9A4;struct Cyc_List_List*_tmp9A6;enum Cyc_Absyn_AggrKind _tmp9A8;struct Cyc_List_List*
_tmp9A9;struct Cyc_Core_Opt*_tmp9AB;void*_tmp9AD;void*_tmp9AF;void*_tmp9B0;void*
_tmp9B2;struct Cyc_Absyn_Exp*_tmp9B4;void*_tmp9BE;void*_tmp9C0;struct Cyc_List_List*
_tmp9C2;_LL4DF: {struct Cyc_Absyn_VarType_struct*_tmp978=(struct Cyc_Absyn_VarType_struct*)
_tmp977;if(_tmp978->tag != 2)goto _LL4E1;else{_tmp979=_tmp978->f1;}}_LL4E0: {
struct _handler_cons _tmp9C3;_push_handler(& _tmp9C3);{int _tmp9C5=0;if(setjmp(
_tmp9C3.handler))_tmp9C5=1;if(!_tmp9C5){{void*_tmp9C6=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp979);_npop_handler(0);return
_tmp9C6;};_pop_handler();}else{void*_tmp9C4=(void*)_exn_thrown;void*_tmp9C8=
_tmp9C4;_LL514: {struct Cyc_Core_Not_found_struct*_tmp9C9=(struct Cyc_Core_Not_found_struct*)
_tmp9C8;if(_tmp9C9->tag != Cyc_Core_Not_found)goto _LL516;}_LL515: return t;_LL516:;
_LL517:(void)_throw(_tmp9C8);_LL513:;}};}_LL4E1: {struct Cyc_Absyn_AggrType_struct*
_tmp97A=(struct Cyc_Absyn_AggrType_struct*)_tmp977;if(_tmp97A->tag != 12)goto
_LL4E3;else{_tmp97B=_tmp97A->f1;_tmp97C=_tmp97B.aggr_info;_tmp97D=_tmp97B.targs;}}
_LL4E2: {struct Cyc_List_List*_tmp9CA=Cyc_Tcutil_substs(rgn,inst,_tmp97D);struct
Cyc_Absyn_AggrType_struct _tmp12E7;struct Cyc_Absyn_AggrInfo _tmp12E6;struct Cyc_Absyn_AggrType_struct*
_tmp12E5;return _tmp9CA == _tmp97D?t:(void*)((_tmp12E5=_cycalloc(sizeof(*_tmp12E5)),((
_tmp12E5[0]=((_tmp12E7.tag=12,((_tmp12E7.f1=((_tmp12E6.aggr_info=_tmp97C,((
_tmp12E6.targs=_tmp9CA,_tmp12E6)))),_tmp12E7)))),_tmp12E5))));}_LL4E3: {struct
Cyc_Absyn_DatatypeType_struct*_tmp97E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp977;if(_tmp97E->tag != 3)goto _LL4E5;else{_tmp97F=_tmp97E->f1;_tmp980=_tmp97F.datatype_info;
_tmp981=_tmp97F.targs;}}_LL4E4: {struct Cyc_List_List*_tmp9CE=Cyc_Tcutil_substs(
rgn,inst,_tmp981);struct Cyc_Absyn_DatatypeType_struct _tmp12ED;struct Cyc_Absyn_DatatypeInfo
_tmp12EC;struct Cyc_Absyn_DatatypeType_struct*_tmp12EB;return _tmp9CE == _tmp981?t:(
void*)((_tmp12EB=_cycalloc(sizeof(*_tmp12EB)),((_tmp12EB[0]=((_tmp12ED.tag=3,((
_tmp12ED.f1=((_tmp12EC.datatype_info=_tmp980,((_tmp12EC.targs=_tmp9CE,_tmp12EC)))),
_tmp12ED)))),_tmp12EB))));}_LL4E5: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp982=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp977;if(_tmp982->tag != 4)
goto _LL4E7;else{_tmp983=_tmp982->f1;_tmp984=_tmp983.field_info;_tmp985=_tmp983.targs;}}
_LL4E6: {struct Cyc_List_List*_tmp9D2=Cyc_Tcutil_substs(rgn,inst,_tmp985);struct
Cyc_Absyn_DatatypeFieldType_struct _tmp12F3;struct Cyc_Absyn_DatatypeFieldInfo
_tmp12F2;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp12F1;return _tmp9D2 == 
_tmp985?t:(void*)((_tmp12F1=_cycalloc(sizeof(*_tmp12F1)),((_tmp12F1[0]=((
_tmp12F3.tag=4,((_tmp12F3.f1=((_tmp12F2.field_info=_tmp984,((_tmp12F2.targs=
_tmp9D2,_tmp12F2)))),_tmp12F3)))),_tmp12F1))));}_LL4E7: {struct Cyc_Absyn_TypedefType_struct*
_tmp986=(struct Cyc_Absyn_TypedefType_struct*)_tmp977;if(_tmp986->tag != 18)goto
_LL4E9;else{_tmp987=_tmp986->f1;_tmp988=_tmp986->f2;_tmp989=_tmp986->f3;_tmp98A=
_tmp986->f4;}}_LL4E8: {struct Cyc_List_List*_tmp9D6=Cyc_Tcutil_substs(rgn,inst,
_tmp988);struct Cyc_Absyn_TypedefType_struct _tmp12F6;struct Cyc_Absyn_TypedefType_struct*
_tmp12F5;return _tmp9D6 == _tmp988?t:(void*)((_tmp12F5=_cycalloc(sizeof(*_tmp12F5)),((
_tmp12F5[0]=((_tmp12F6.tag=18,((_tmp12F6.f1=_tmp987,((_tmp12F6.f2=_tmp9D6,((
_tmp12F6.f3=_tmp989,((_tmp12F6.f4=_tmp98A,_tmp12F6)))))))))),_tmp12F5))));}
_LL4E9: {struct Cyc_Absyn_ArrayType_struct*_tmp98B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp977;if(_tmp98B->tag != 9)goto _LL4EB;else{_tmp98C=_tmp98B->f1;_tmp98D=_tmp98C.elt_type;
_tmp98E=_tmp98C.tq;_tmp98F=_tmp98C.num_elts;_tmp990=_tmp98C.zero_term;_tmp991=
_tmp98C.zt_loc;}}_LL4EA: {void*_tmp9D9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp98D);
struct Cyc_Absyn_ArrayType_struct _tmp12FC;struct Cyc_Absyn_ArrayInfo _tmp12FB;
struct Cyc_Absyn_ArrayType_struct*_tmp12FA;return _tmp9D9 == _tmp98D?t:(void*)((
_tmp12FA=_cycalloc(sizeof(*_tmp12FA)),((_tmp12FA[0]=((_tmp12FC.tag=9,((_tmp12FC.f1=((
_tmp12FB.elt_type=_tmp9D9,((_tmp12FB.tq=_tmp98E,((_tmp12FB.num_elts=_tmp98F,((
_tmp12FB.zero_term=_tmp990,((_tmp12FB.zt_loc=_tmp991,_tmp12FB)))))))))),_tmp12FC)))),
_tmp12FA))));}_LL4EB: {struct Cyc_Absyn_PointerType_struct*_tmp992=(struct Cyc_Absyn_PointerType_struct*)
_tmp977;if(_tmp992->tag != 5)goto _LL4ED;else{_tmp993=_tmp992->f1;_tmp994=_tmp993.elt_typ;
_tmp995=_tmp993.elt_tq;_tmp996=_tmp993.ptr_atts;_tmp997=_tmp996.rgn;_tmp998=
_tmp996.nullable;_tmp999=_tmp996.bounds;_tmp99A=_tmp996.zero_term;}}_LL4EC: {
void*_tmp9DD=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp994);void*_tmp9DE=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp997);union Cyc_Absyn_Constraint*_tmp9DF=_tmp999;{void*_tmp9E0=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp999);struct Cyc_Absyn_Exp*_tmp9E2;_LL519: {struct Cyc_Absyn_Upper_b_struct*
_tmp9E1=(struct Cyc_Absyn_Upper_b_struct*)_tmp9E0;if(_tmp9E1->tag != 1)goto _LL51B;
else{_tmp9E2=_tmp9E1->f1;}}_LL51A: {struct Cyc_Absyn_Exp*_tmp9E3=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmp9E2);if(_tmp9E3 != _tmp9E2){struct Cyc_Absyn_Upper_b_struct _tmp12FF;
struct Cyc_Absyn_Upper_b_struct*_tmp12FE;_tmp9DF=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp12FE=_cycalloc(sizeof(*_tmp12FE)),((
_tmp12FE[0]=((_tmp12FF.tag=1,((_tmp12FF.f1=_tmp9E3,_tmp12FF)))),_tmp12FE)))));}
goto _LL518;}_LL51B:;_LL51C: goto _LL518;_LL518:;}if((_tmp9DD == _tmp994  && _tmp9DE
== _tmp997) && _tmp9DF == _tmp999)return t;{struct Cyc_Absyn_PointerType_struct
_tmp1309;struct Cyc_Absyn_PtrAtts _tmp1308;struct Cyc_Absyn_PtrInfo _tmp1307;struct
Cyc_Absyn_PointerType_struct*_tmp1306;return(void*)((_tmp1306=_cycalloc(sizeof(*
_tmp1306)),((_tmp1306[0]=((_tmp1309.tag=5,((_tmp1309.f1=((_tmp1307.elt_typ=
_tmp9DD,((_tmp1307.elt_tq=_tmp995,((_tmp1307.ptr_atts=((_tmp1308.rgn=_tmp9DE,((
_tmp1308.nullable=_tmp998,((_tmp1308.bounds=_tmp9DF,((_tmp1308.zero_term=_tmp99A,((
_tmp1308.ptrloc=0,_tmp1308)))))))))),_tmp1307)))))),_tmp1309)))),_tmp1306))));};}
_LL4ED: {struct Cyc_Absyn_FnType_struct*_tmp99B=(struct Cyc_Absyn_FnType_struct*)
_tmp977;if(_tmp99B->tag != 10)goto _LL4EF;else{_tmp99C=_tmp99B->f1;_tmp99D=_tmp99C.tvars;
_tmp99E=_tmp99C.effect;_tmp99F=_tmp99C.ret_typ;_tmp9A0=_tmp99C.args;_tmp9A1=
_tmp99C.c_varargs;_tmp9A2=_tmp99C.cyc_varargs;_tmp9A3=_tmp99C.rgn_po;_tmp9A4=
_tmp99C.attributes;}}_LL4EE:{struct Cyc_List_List*_tmp9EA=_tmp99D;for(0;_tmp9EA != 
0;_tmp9EA=_tmp9EA->tl){struct _tuple15*_tmp1313;struct Cyc_Absyn_VarType_struct
_tmp1312;struct Cyc_Absyn_VarType_struct*_tmp1311;struct Cyc_List_List*_tmp1310;
inst=((_tmp1310=_region_malloc(rgn,sizeof(*_tmp1310)),((_tmp1310->hd=((_tmp1313=
_region_malloc(rgn,sizeof(*_tmp1313)),((_tmp1313->f1=(struct Cyc_Absyn_Tvar*)
_tmp9EA->hd,((_tmp1313->f2=(void*)((_tmp1311=_cycalloc(sizeof(*_tmp1311)),((
_tmp1311[0]=((_tmp1312.tag=2,((_tmp1312.f1=(struct Cyc_Absyn_Tvar*)_tmp9EA->hd,
_tmp1312)))),_tmp1311)))),_tmp1313)))))),((_tmp1310->tl=inst,_tmp1310))))));}}{
struct Cyc_List_List*_tmp9F0;struct Cyc_List_List*_tmp9F1;struct _tuple1 _tmp9EF=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple26*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp9A0));
_tmp9F0=_tmp9EF.f1;_tmp9F1=_tmp9EF.f2;{struct Cyc_List_List*_tmp9F2=Cyc_Tcutil_substs(
rgn,inst,_tmp9F1);struct Cyc_List_List*_tmp9F3=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp9F0,_tmp9F2));struct Cyc_Core_Opt*
eff2;if(_tmp99E == 0)eff2=0;else{void*_tmp9F4=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp99E->v);if(_tmp9F4 == (void*)_tmp99E->v)eff2=_tmp99E;else{struct Cyc_Core_Opt*
_tmp1314;eff2=((_tmp1314=_cycalloc(sizeof(*_tmp1314)),((_tmp1314->v=_tmp9F4,
_tmp1314))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp9A2 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp9F7;struct Cyc_Absyn_Tqual _tmp9F8;void*
_tmp9F9;int _tmp9FA;struct Cyc_Absyn_VarargInfo _tmp9F6=*_tmp9A2;_tmp9F7=_tmp9F6.name;
_tmp9F8=_tmp9F6.tq;_tmp9F9=_tmp9F6.type;_tmp9FA=_tmp9F6.inject;{void*_tmp9FB=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9F9);if(_tmp9FB == _tmp9F9)cyc_varargs2=_tmp9A2;else{struct Cyc_Absyn_VarargInfo*
_tmp1315;cyc_varargs2=((_tmp1315=_cycalloc(sizeof(*_tmp1315)),((_tmp1315->name=
_tmp9F7,((_tmp1315->tq=_tmp9F8,((_tmp1315->type=_tmp9FB,((_tmp1315->inject=
_tmp9FA,_tmp1315))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmp9FE;struct Cyc_List_List*_tmp9FF;struct _tuple1 _tmp9FD=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmp9A3);_tmp9FE=_tmp9FD.f1;_tmp9FF=_tmp9FD.f2;{struct Cyc_List_List*
_tmpA00=Cyc_Tcutil_substs(rgn,inst,_tmp9FE);struct Cyc_List_List*_tmpA01=Cyc_Tcutil_substs(
rgn,inst,_tmp9FF);if(_tmpA00 == _tmp9FE  && _tmpA01 == _tmp9FF)rgn_po2=_tmp9A3;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmpA00,_tmpA01);}{struct Cyc_Absyn_FnType_struct _tmp131B;struct
Cyc_Absyn_FnInfo _tmp131A;struct Cyc_Absyn_FnType_struct*_tmp1319;return(void*)((
_tmp1319=_cycalloc(sizeof(*_tmp1319)),((_tmp1319[0]=((_tmp131B.tag=10,((_tmp131B.f1=((
_tmp131A.tvars=_tmp99D,((_tmp131A.effect=eff2,((_tmp131A.ret_typ=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp99F),((_tmp131A.args=_tmp9F3,((_tmp131A.c_varargs=_tmp9A1,((
_tmp131A.cyc_varargs=cyc_varargs2,((_tmp131A.rgn_po=rgn_po2,((_tmp131A.attributes=
_tmp9A4,_tmp131A)))))))))))))))),_tmp131B)))),_tmp1319))));};};};};};};_LL4EF: {
struct Cyc_Absyn_TupleType_struct*_tmp9A5=(struct Cyc_Absyn_TupleType_struct*)
_tmp977;if(_tmp9A5->tag != 11)goto _LL4F1;else{_tmp9A6=_tmp9A5->f1;}}_LL4F0: {
struct Cyc_List_List*_tmpA05=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp9A6);struct Cyc_List_List*_tmpA06=Cyc_Tcutil_substs(rgn,inst,_tmpA05);if(
_tmpA06 == _tmpA05)return t;{struct Cyc_List_List*_tmpA07=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp9A6,_tmpA06);struct Cyc_Absyn_TupleType_struct
_tmp131E;struct Cyc_Absyn_TupleType_struct*_tmp131D;return(void*)((_tmp131D=
_cycalloc(sizeof(*_tmp131D)),((_tmp131D[0]=((_tmp131E.tag=11,((_tmp131E.f1=
_tmpA07,_tmp131E)))),_tmp131D))));};}_LL4F1: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp9A7=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp977;if(_tmp9A7->tag != 13)goto
_LL4F3;else{_tmp9A8=_tmp9A7->f1;_tmp9A9=_tmp9A7->f2;}}_LL4F2: {struct Cyc_List_List*
_tmpA0A=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp9A9);struct
Cyc_List_List*_tmpA0B=Cyc_Tcutil_substs(rgn,inst,_tmpA0A);if(_tmpA0B == _tmpA0A)
return t;{struct Cyc_List_List*_tmpA0C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp9A9,_tmpA0B);struct Cyc_Absyn_AnonAggrType_struct
_tmp1321;struct Cyc_Absyn_AnonAggrType_struct*_tmp1320;return(void*)((_tmp1320=
_cycalloc(sizeof(*_tmp1320)),((_tmp1320[0]=((_tmp1321.tag=13,((_tmp1321.f1=
_tmp9A8,((_tmp1321.f2=_tmpA0C,_tmp1321)))))),_tmp1320))));};}_LL4F3: {struct Cyc_Absyn_Evar_struct*
_tmp9AA=(struct Cyc_Absyn_Evar_struct*)_tmp977;if(_tmp9AA->tag != 1)goto _LL4F5;
else{_tmp9AB=_tmp9AA->f2;}}_LL4F4: if(_tmp9AB != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp9AB->v);else{return t;}_LL4F5: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp9AC=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp977;if(_tmp9AC->tag != 16)goto
_LL4F7;else{_tmp9AD=(void*)_tmp9AC->f1;}}_LL4F6: {void*_tmpA0F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9AD);struct Cyc_Absyn_RgnHandleType_struct _tmp1324;struct Cyc_Absyn_RgnHandleType_struct*
_tmp1323;return _tmpA0F == _tmp9AD?t:(void*)((_tmp1323=_cycalloc(sizeof(*_tmp1323)),((
_tmp1323[0]=((_tmp1324.tag=16,((_tmp1324.f1=(void*)_tmpA0F,_tmp1324)))),_tmp1323))));}
_LL4F7: {struct Cyc_Absyn_DynRgnType_struct*_tmp9AE=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp977;if(_tmp9AE->tag != 17)goto _LL4F9;else{_tmp9AF=(void*)_tmp9AE->f1;_tmp9B0=(
void*)_tmp9AE->f2;}}_LL4F8: {void*_tmpA12=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp9AF);void*_tmpA13=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9B0);struct Cyc_Absyn_DynRgnType_struct
_tmp1327;struct Cyc_Absyn_DynRgnType_struct*_tmp1326;return _tmpA12 == _tmp9AF  && 
_tmpA13 == _tmp9B0?t:(void*)((_tmp1326=_cycalloc(sizeof(*_tmp1326)),((_tmp1326[0]=((
_tmp1327.tag=17,((_tmp1327.f1=(void*)_tmpA12,((_tmp1327.f2=(void*)_tmpA13,
_tmp1327)))))),_tmp1326))));}_LL4F9: {struct Cyc_Absyn_TagType_struct*_tmp9B1=(
struct Cyc_Absyn_TagType_struct*)_tmp977;if(_tmp9B1->tag != 20)goto _LL4FB;else{
_tmp9B2=(void*)_tmp9B1->f1;}}_LL4FA: {void*_tmpA16=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp9B2);struct Cyc_Absyn_TagType_struct _tmp132A;struct Cyc_Absyn_TagType_struct*
_tmp1329;return _tmpA16 == _tmp9B2?t:(void*)((_tmp1329=_cycalloc(sizeof(*_tmp1329)),((
_tmp1329[0]=((_tmp132A.tag=20,((_tmp132A.f1=(void*)_tmpA16,_tmp132A)))),_tmp1329))));}
_LL4FB: {struct Cyc_Absyn_ValueofType_struct*_tmp9B3=(struct Cyc_Absyn_ValueofType_struct*)
_tmp977;if(_tmp9B3->tag != 19)goto _LL4FD;else{_tmp9B4=_tmp9B3->f1;}}_LL4FC: {
struct Cyc_Absyn_Exp*_tmpA19=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp9B4);struct Cyc_Absyn_ValueofType_struct
_tmp132D;struct Cyc_Absyn_ValueofType_struct*_tmp132C;return _tmpA19 == _tmp9B4?t:(
void*)((_tmp132C=_cycalloc(sizeof(*_tmp132C)),((_tmp132C[0]=((_tmp132D.tag=19,((
_tmp132D.f1=_tmpA19,_tmp132D)))),_tmp132C))));}_LL4FD: {struct Cyc_Absyn_EnumType_struct*
_tmp9B5=(struct Cyc_Absyn_EnumType_struct*)_tmp977;if(_tmp9B5->tag != 14)goto
_LL4FF;}_LL4FE: goto _LL500;_LL4FF: {struct Cyc_Absyn_AnonEnumType_struct*_tmp9B6=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp977;if(_tmp9B6->tag != 15)goto _LL501;}
_LL500: goto _LL502;_LL501: {struct Cyc_Absyn_VoidType_struct*_tmp9B7=(struct Cyc_Absyn_VoidType_struct*)
_tmp977;if(_tmp9B7->tag != 0)goto _LL503;}_LL502: goto _LL504;_LL503: {struct Cyc_Absyn_IntType_struct*
_tmp9B8=(struct Cyc_Absyn_IntType_struct*)_tmp977;if(_tmp9B8->tag != 6)goto _LL505;}
_LL504: goto _LL506;_LL505: {struct Cyc_Absyn_FloatType_struct*_tmp9B9=(struct Cyc_Absyn_FloatType_struct*)
_tmp977;if(_tmp9B9->tag != 7)goto _LL507;}_LL506: goto _LL508;_LL507: {struct Cyc_Absyn_DoubleType_struct*
_tmp9BA=(struct Cyc_Absyn_DoubleType_struct*)_tmp977;if(_tmp9BA->tag != 8)goto
_LL509;}_LL508: goto _LL50A;_LL509: {struct Cyc_Absyn_UniqueRgn_struct*_tmp9BB=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp977;if(_tmp9BB->tag != 22)goto _LL50B;}
_LL50A: goto _LL50C;_LL50B: {struct Cyc_Absyn_HeapRgn_struct*_tmp9BC=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp977;if(_tmp9BC->tag != 21)goto _LL50D;}_LL50C: return t;_LL50D: {struct Cyc_Absyn_RgnsEff_struct*
_tmp9BD=(struct Cyc_Absyn_RgnsEff_struct*)_tmp977;if(_tmp9BD->tag != 25)goto _LL50F;
else{_tmp9BE=(void*)_tmp9BD->f1;}}_LL50E: {void*_tmpA1C=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9BE);struct Cyc_Absyn_RgnsEff_struct _tmp1330;struct Cyc_Absyn_RgnsEff_struct*
_tmp132F;return _tmpA1C == _tmp9BE?t:(void*)((_tmp132F=_cycalloc(sizeof(*_tmp132F)),((
_tmp132F[0]=((_tmp1330.tag=25,((_tmp1330.f1=(void*)_tmpA1C,_tmp1330)))),_tmp132F))));}
_LL50F: {struct Cyc_Absyn_AccessEff_struct*_tmp9BF=(struct Cyc_Absyn_AccessEff_struct*)
_tmp977;if(_tmp9BF->tag != 23)goto _LL511;else{_tmp9C0=(void*)_tmp9BF->f1;}}_LL510: {
void*_tmpA1F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9C0);struct Cyc_Absyn_AccessEff_struct
_tmp1333;struct Cyc_Absyn_AccessEff_struct*_tmp1332;return _tmpA1F == _tmp9C0?t:(
void*)((_tmp1332=_cycalloc(sizeof(*_tmp1332)),((_tmp1332[0]=((_tmp1333.tag=23,((
_tmp1333.f1=(void*)_tmpA1F,_tmp1333)))),_tmp1332))));}_LL511: {struct Cyc_Absyn_JoinEff_struct*
_tmp9C1=(struct Cyc_Absyn_JoinEff_struct*)_tmp977;if(_tmp9C1->tag != 24)goto _LL4DE;
else{_tmp9C2=_tmp9C1->f1;}}_LL512: {struct Cyc_List_List*_tmpA22=Cyc_Tcutil_substs(
rgn,inst,_tmp9C2);struct Cyc_Absyn_JoinEff_struct _tmp1336;struct Cyc_Absyn_JoinEff_struct*
_tmp1335;return _tmpA22 == _tmp9C2?t:(void*)((_tmp1335=_cycalloc(sizeof(*_tmp1335)),((
_tmp1335[0]=((_tmp1336.tag=24,((_tmp1336.f1=_tmpA22,_tmp1336)))),_tmp1335))));}
_LL4DE:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 
0)return 0;{void*_tmpA25=(void*)ts->hd;struct Cyc_List_List*_tmpA26=ts->tl;void*
_tmpA27=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA25);struct Cyc_List_List*_tmpA28=Cyc_Tcutil_substs(
rgn,inst,_tmpA26);if(_tmpA25 == _tmpA27  && _tmpA26 == _tmpA28)return ts;{struct Cyc_List_List*
_tmp1337;return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp1337=_cycalloc(
sizeof(*_tmp1337)),((_tmp1337->hd=_tmpA27,((_tmp1337->tl=_tmpA28,_tmp1337)))))));};};}
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv);struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){struct Cyc_Core_Opt*_tmpA2A=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,&
Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp133A;struct _tuple15*_tmp1339;return(
_tmp1339=_cycalloc(sizeof(*_tmp1339)),((_tmp1339->f1=tv,((_tmp1339->f2=Cyc_Absyn_new_evar(
_tmpA2A,((_tmp133A=_cycalloc(sizeof(*_tmp133A)),((_tmp133A->v=s,_tmp133A))))),
_tmp1339)))));}struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,
struct Cyc_Absyn_Tvar*tv);struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct
_tuple16*env,struct Cyc_Absyn_Tvar*tv){struct _tuple16 _tmpA2E;struct Cyc_List_List*
_tmpA2F;struct _RegionHandle*_tmpA30;struct _tuple16*_tmpA2D=env;_tmpA2E=*_tmpA2D;
_tmpA2F=_tmpA2E.f1;_tmpA30=_tmpA2E.f2;{struct Cyc_Core_Opt*_tmpA31=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp133D;struct
_tuple15*_tmp133C;return(_tmp133C=_region_malloc(_tmpA30,sizeof(*_tmp133C)),((
_tmp133C->f1=tv,((_tmp133C->f2=Cyc_Absyn_new_evar(_tmpA31,((_tmp133D=_cycalloc(
sizeof(*_tmp133D)),((_tmp133D->v=_tmpA2F,_tmp133D))))),_tmp133C)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const
char*_tmp1343;void*_tmp1342[3];struct Cyc_String_pa_struct _tmp1341;struct Cyc_String_pa_struct
_tmp1340;struct Cyc_String_pa_struct _tmp133F;(_tmp133F.tag=0,((_tmp133F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((
_tmp1340.tag=0,((_tmp1340.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k1)),((_tmp1341.tag=0,((_tmp1341.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
tv->name),((_tmp1342[0]=& _tmp1341,((_tmp1342[1]=& _tmp1340,((_tmp1342[2]=&
_tmp133F,Cyc_Tcutil_terr(loc,((_tmp1343="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp1343,sizeof(char),59))),_tag_dyneither(_tmp1342,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp1346;void*_tmp1345;(_tmp1345=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1346="same type variable has different identity!",
_tag_dyneither(_tmp1346,sizeof(char),43))),_tag_dyneither(_tmp1345,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp1347;return(_tmp1347=_cycalloc(sizeof(*_tmp1347)),((_tmp1347->hd=tv,((
_tmp1347->tl=tvs,_tmp1347)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp134A;void*_tmp1349;(_tmp1349=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp134A="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp134A,sizeof(char),39))),_tag_dyneither(_tmp1349,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmpA3E=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmpA3E->identity == - 1){const char*
_tmp134D;void*_tmp134C;(_tmp134C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp134D="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp134D,sizeof(char),41))),_tag_dyneither(_tmp134C,sizeof(void*),
0)));}if(_tmpA3E->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp134E;return(_tmp134E=_cycalloc(sizeof(*_tmp134E)),((_tmp134E->hd=tv,((
_tmp134E->tl=tvs,_tmp134E)))));};}struct _tuple27{struct Cyc_Absyn_Tvar*f1;int f2;}
;static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct
_RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b);static
struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp1351;void*_tmp1350;(_tmp1350=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1351="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp1351,sizeof(char),44))),_tag_dyneither(_tmp1350,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple27
_tmpA45;struct Cyc_Absyn_Tvar*_tmpA46;int _tmpA47;int*_tmpA48;struct _tuple27*
_tmpA44=(struct _tuple27*)tvs2->hd;_tmpA45=*_tmpA44;_tmpA46=_tmpA45.f1;_tmpA47=
_tmpA45.f2;_tmpA48=(int*)&(*_tmpA44).f2;if(_tmpA46->identity == - 1){const char*
_tmp1354;void*_tmp1353;(_tmp1353=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1354="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp1354,sizeof(char),46))),_tag_dyneither(_tmp1353,sizeof(void*),
0)));}if(_tmpA46->identity == tv->identity){*_tmpA48=*_tmpA48  || b;return tvs;}}}{
struct _tuple27*_tmp1357;struct Cyc_List_List*_tmp1356;return(_tmp1356=
_region_malloc(r,sizeof(*_tmp1356)),((_tmp1356->hd=((_tmp1357=_region_malloc(r,
sizeof(*_tmp1357)),((_tmp1357->f1=tv,((_tmp1357->f2=b,_tmp1357)))))),((_tmp1356->tl=
tvs,_tmp1356)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp135B;void*_tmp135A[1];struct Cyc_String_pa_struct _tmp1359;(_tmp1359.tag=0,((
_tmp1359.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp135A[0]=& _tmp1359,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp135B="bound tvar id for %s is NULL",
_tag_dyneither(_tmp135B,sizeof(char),29))),_tag_dyneither(_tmp135A,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp135C;return(_tmp135C=_cycalloc(sizeof(*
_tmp135C)),((_tmp135C->hd=tv,((_tmp135C->tl=tvs,_tmp135C)))));};}struct _tuple28{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b);static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmpA51=Cyc_Tcutil_compress(e);int _tmpA53;_LL51E: {struct Cyc_Absyn_Evar_struct*
_tmpA52=(struct Cyc_Absyn_Evar_struct*)_tmpA51;if(_tmpA52->tag != 1)goto _LL520;
else{_tmpA53=_tmpA52->f3;}}_LL51F:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=
es2->tl){struct _tuple28 _tmpA55;void*_tmpA56;int _tmpA57;int*_tmpA58;struct
_tuple28*_tmpA54=(struct _tuple28*)es2->hd;_tmpA55=*_tmpA54;_tmpA56=_tmpA55.f1;
_tmpA57=_tmpA55.f2;_tmpA58=(int*)&(*_tmpA54).f2;{void*_tmpA59=Cyc_Tcutil_compress(
_tmpA56);int _tmpA5B;_LL523: {struct Cyc_Absyn_Evar_struct*_tmpA5A=(struct Cyc_Absyn_Evar_struct*)
_tmpA59;if(_tmpA5A->tag != 1)goto _LL525;else{_tmpA5B=_tmpA5A->f3;}}_LL524: if(
_tmpA53 == _tmpA5B){if(b != *_tmpA58)*_tmpA58=1;return es;}goto _LL522;_LL525:;
_LL526: goto _LL522;_LL522:;};}}{struct _tuple28*_tmp135F;struct Cyc_List_List*
_tmp135E;return(_tmp135E=_region_malloc(r,sizeof(*_tmp135E)),((_tmp135E->hd=((
_tmp135F=_region_malloc(r,sizeof(*_tmp135F)),((_tmp135F->f1=e,((_tmp135F->f2=b,
_tmp135F)))))),((_tmp135E->tl=es,_tmp135E)))));};_LL520:;_LL521: return es;_LL51D:;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp1360;r=((_tmp1360=_region_malloc(rgn,sizeof(*_tmp1360)),((
_tmp1360->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1360->tl=r,_tmp1360))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpA60;int _tmpA61;struct _tuple27 _tmpA5F=*((struct _tuple27*)
tvs->hd);_tmpA60=_tmpA5F.f1;_tmpA61=_tmpA5F.f2;{int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(_tmpA60->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;break;}}}if(!present){struct Cyc_List_List*_tmp1361;res=((_tmp1361=
_region_malloc(r,sizeof(*_tmp1361)),((_tmp1361->hd=(struct _tuple27*)tvs->hd,((
_tmp1361->tl=res,_tmp1361))))));}};}res=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(res);return res;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width)){const char*_tmp1365;void*_tmp1364[1];struct Cyc_String_pa_struct
_tmp1363;(_tmp1363.tag=0,((_tmp1363.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp1364[0]=& _tmp1363,Cyc_Tcutil_terr(loc,((_tmp1365="bitfield %s does not have constant width",
_tag_dyneither(_tmp1365,sizeof(char),41))),_tag_dyneither(_tmp1364,sizeof(void*),
1)))))));}else{unsigned int _tmpA67;int _tmpA68;struct _tuple13 _tmpA66=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmpA67=_tmpA66.f1;_tmpA68=_tmpA66.f2;if(!_tmpA68){
const char*_tmp1368;void*_tmp1367;(_tmp1367=0,Cyc_Tcutil_terr(loc,((_tmp1368="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp1368,sizeof(char),45))),_tag_dyneither(_tmp1367,sizeof(void*),
0)));}w=_tmpA67;}{void*_tmpA6B=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmpA6D;_LL528: {struct Cyc_Absyn_IntType_struct*_tmpA6C=(struct Cyc_Absyn_IntType_struct*)
_tmpA6B;if(_tmpA6C->tag != 6)goto _LL52A;else{_tmpA6D=_tmpA6C->f2;}}_LL529: switch(
_tmpA6D){case Cyc_Absyn_Char_sz: _LL52C: if(w > 8){const char*_tmp136B;void*_tmp136A;(
_tmp136A=0,Cyc_Tcutil_terr(loc,((_tmp136B="bitfield larger than type",
_tag_dyneither(_tmp136B,sizeof(char),26))),_tag_dyneither(_tmp136A,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL52D: if(w > 16){const char*_tmp136E;void*
_tmp136D;(_tmp136D=0,Cyc_Tcutil_terr(loc,((_tmp136E="bitfield larger than type",
_tag_dyneither(_tmp136E,sizeof(char),26))),_tag_dyneither(_tmp136D,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL52E: goto _LL52F;case Cyc_Absyn_Int_sz: _LL52F:
if(w > 32){const char*_tmp1371;void*_tmp1370;(_tmp1370=0,Cyc_Tcutil_terr(loc,((
_tmp1371="bitfield larger than type",_tag_dyneither(_tmp1371,sizeof(char),26))),
_tag_dyneither(_tmp1370,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL530: if(w > 64){const char*_tmp1374;void*_tmp1373;(_tmp1373=0,Cyc_Tcutil_terr(
loc,((_tmp1374="bitfield larger than type",_tag_dyneither(_tmp1374,sizeof(char),
26))),_tag_dyneither(_tmp1373,sizeof(void*),0)));}break;}goto _LL527;_LL52A:;
_LL52B:{const char*_tmp1379;void*_tmp1378[2];struct Cyc_String_pa_struct _tmp1377;
struct Cyc_String_pa_struct _tmp1376;(_tmp1376.tag=0,((_tmp1376.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp1377.tag=0,((_tmp1377.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp1378[0]=& _tmp1377,((_tmp1378[1]=& _tmp1376,Cyc_Tcutil_terr(loc,((_tmp1379="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp1379,sizeof(char),52))),_tag_dyneither(_tmp1378,sizeof(void*),
2)))))))))))));}goto _LL527;_LL527:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts);
static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*
_tmpA7A=(void*)atts->hd;_LL533: {struct Cyc_Absyn_Packed_att_struct*_tmpA7B=(
struct Cyc_Absyn_Packed_att_struct*)_tmpA7A;if(_tmpA7B->tag != 7)goto _LL535;}
_LL534: continue;_LL535: {struct Cyc_Absyn_Aligned_att_struct*_tmpA7C=(struct Cyc_Absyn_Aligned_att_struct*)
_tmpA7A;if(_tmpA7C->tag != 6)goto _LL537;}_LL536: continue;_LL537:;_LL538: {const
char*_tmp137E;void*_tmp137D[2];struct Cyc_String_pa_struct _tmp137C;struct Cyc_String_pa_struct
_tmp137B;(_tmp137B.tag=0,((_tmp137B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp137C.tag=0,((_tmp137C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp137D[0]=&
_tmp137C,((_tmp137D[1]=& _tmp137B,Cyc_Tcutil_terr(loc,((_tmp137E="bad attribute %s on member %s",
_tag_dyneither(_tmp137E,sizeof(char),30))),_tag_dyneither(_tmp137D,sizeof(void*),
2)))))))))))));}_LL532:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct
Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*
free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmpA81=t;struct
Cyc_Absyn_Typedefdecl*_tmpA83;void**_tmpA84;_LL53A: {struct Cyc_Absyn_TypedefType_struct*
_tmpA82=(struct Cyc_Absyn_TypedefType_struct*)_tmpA81;if(_tmpA82->tag != 18)goto
_LL53C;else{_tmpA83=_tmpA82->f3;_tmpA84=_tmpA82->f4;}}_LL53B: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmpA83))->tq).real_const  || (_tmpA83->tq).print_const){if(
declared_const){const char*_tmp1381;void*_tmp1380;(_tmp1380=0,Cyc_Tcutil_warn(loc,((
_tmp1381="extra const",_tag_dyneither(_tmp1381,sizeof(char),12))),_tag_dyneither(
_tmp1380,sizeof(void*),0)));}return 1;}if((unsigned int)_tmpA84)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmpA84);else{return declared_const;}_LL53C:;_LL53D: return
declared_const;_LL539:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td);static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(
struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn
!= 0){void*_tmpA87=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
td->defn))->v);struct Cyc_Absyn_PtrInfo _tmpA89;struct Cyc_Absyn_PtrAtts _tmpA8A;
void*_tmpA8B;_LL53F: {struct Cyc_Absyn_PointerType_struct*_tmpA88=(struct Cyc_Absyn_PointerType_struct*)
_tmpA87;if(_tmpA88->tag != 5)goto _LL541;else{_tmpA89=_tmpA88->f1;_tmpA8A=_tmpA89.ptr_atts;
_tmpA8B=_tmpA8A.rgn;}}_LL540:{void*_tmpA8C=Cyc_Tcutil_compress(_tmpA8B);struct
Cyc_Absyn_Tvar*_tmpA8E;_LL544: {struct Cyc_Absyn_VarType_struct*_tmpA8D=(struct
Cyc_Absyn_VarType_struct*)_tmpA8C;if(_tmpA8D->tag != 2)goto _LL546;else{_tmpA8E=
_tmpA8D->f1;}}_LL545: return Cyc_Absyn_tvar_cmp(tvar,_tmpA8E)== 0;_LL546:;_LL547:
goto _LL543;_LL543:;}goto _LL53E;_LL541:;_LL542: goto _LL53E;_LL53E:;}}else{return 1;}
return 0;}static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td);static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td){void*_tmpA8F=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpA91;
struct Cyc_Absyn_Kind _tmpA92;enum Cyc_Absyn_KindQual _tmpA93;enum Cyc_Absyn_AliasQual
_tmpA94;struct Cyc_Absyn_Kind*_tmpA96;struct Cyc_Absyn_Kind _tmpA97;enum Cyc_Absyn_KindQual
_tmpA98;enum Cyc_Absyn_AliasQual _tmpA99;_LL549: {struct Cyc_Absyn_Less_kb_struct*
_tmpA90=(struct Cyc_Absyn_Less_kb_struct*)_tmpA8F;if(_tmpA90->tag != 2)goto _LL54B;
else{_tmpA91=_tmpA90->f2;_tmpA92=*_tmpA91;_tmpA93=_tmpA92.kind;if(_tmpA93 != Cyc_Absyn_RgnKind)
goto _LL54B;_tmpA94=_tmpA92.aliasqual;if(_tmpA94 != Cyc_Absyn_Top)goto _LL54B;}}
_LL54A: goto _LL54C;_LL54B: {struct Cyc_Absyn_Eq_kb_struct*_tmpA95=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpA8F;if(_tmpA95->tag != 0)goto _LL54D;else{_tmpA96=_tmpA95->f1;_tmpA97=*_tmpA96;
_tmpA98=_tmpA97.kind;if(_tmpA98 != Cyc_Absyn_RgnKind)goto _LL54D;_tmpA99=_tmpA97.aliasqual;
if(_tmpA99 != Cyc_Absyn_Top)goto _LL54D;}}_LL54C: if(((expected_kind->kind == Cyc_Absyn_BoxKind
 || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind)
 && Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){if(expected_kind->aliasqual == 
Cyc_Absyn_Aliasable)return& Cyc_Tcutil_rk;else{if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}return& Cyc_Tcutil_trk;_LL54D:;_LL54E: return Cyc_Tcutil_tvar_kind(
tvar,def_kind);_LL548:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple29{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int
put_in_effect);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(struct
Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,
struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){static struct Cyc_Core_Opt
urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};static struct Cyc_Core_Opt hrgn={(
void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*_tmpA9A=Cyc_Tcutil_compress(t);
struct Cyc_Core_Opt*_tmpA9D;struct Cyc_Core_Opt**_tmpA9E;struct Cyc_Core_Opt*
_tmpA9F;struct Cyc_Core_Opt**_tmpAA0;struct Cyc_Absyn_Tvar*_tmpAA2;struct Cyc_List_List*
_tmpAA4;struct _tuple2*_tmpAA6;struct Cyc_Absyn_Enumdecl*_tmpAA7;struct Cyc_Absyn_Enumdecl**
_tmpAA8;struct Cyc_Absyn_DatatypeInfo _tmpAAA;union Cyc_Absyn_DatatypeInfoU _tmpAAB;
union Cyc_Absyn_DatatypeInfoU*_tmpAAC;struct Cyc_List_List*_tmpAAD;struct Cyc_List_List**
_tmpAAE;struct Cyc_Absyn_DatatypeFieldInfo _tmpAB0;union Cyc_Absyn_DatatypeFieldInfoU
_tmpAB1;union Cyc_Absyn_DatatypeFieldInfoU*_tmpAB2;struct Cyc_List_List*_tmpAB3;
struct Cyc_Absyn_PtrInfo _tmpAB5;void*_tmpAB6;struct Cyc_Absyn_Tqual _tmpAB7;struct
Cyc_Absyn_Tqual*_tmpAB8;struct Cyc_Absyn_PtrAtts _tmpAB9;void*_tmpABA;union Cyc_Absyn_Constraint*
_tmpABB;union Cyc_Absyn_Constraint*_tmpABC;union Cyc_Absyn_Constraint*_tmpABD;void*
_tmpABF;struct Cyc_Absyn_Exp*_tmpAC1;struct Cyc_Absyn_ArrayInfo _tmpAC6;void*
_tmpAC7;struct Cyc_Absyn_Tqual _tmpAC8;struct Cyc_Absyn_Tqual*_tmpAC9;struct Cyc_Absyn_Exp*
_tmpACA;struct Cyc_Absyn_Exp**_tmpACB;union Cyc_Absyn_Constraint*_tmpACC;struct Cyc_Position_Segment*
_tmpACD;struct Cyc_Absyn_FnInfo _tmpACF;struct Cyc_List_List*_tmpAD0;struct Cyc_List_List**
_tmpAD1;struct Cyc_Core_Opt*_tmpAD2;struct Cyc_Core_Opt**_tmpAD3;void*_tmpAD4;
struct Cyc_List_List*_tmpAD5;int _tmpAD6;struct Cyc_Absyn_VarargInfo*_tmpAD7;struct
Cyc_List_List*_tmpAD8;struct Cyc_List_List*_tmpAD9;struct Cyc_List_List*_tmpADB;
enum Cyc_Absyn_AggrKind _tmpADD;struct Cyc_List_List*_tmpADE;struct Cyc_Absyn_AggrInfo
_tmpAE0;union Cyc_Absyn_AggrInfoU _tmpAE1;union Cyc_Absyn_AggrInfoU*_tmpAE2;struct
Cyc_List_List*_tmpAE3;struct Cyc_List_List**_tmpAE4;struct _tuple2*_tmpAE6;struct
Cyc_List_List*_tmpAE7;struct Cyc_List_List**_tmpAE8;struct Cyc_Absyn_Typedefdecl*
_tmpAE9;struct Cyc_Absyn_Typedefdecl**_tmpAEA;void**_tmpAEB;void***_tmpAEC;void*
_tmpAF0;void*_tmpAF2;void*_tmpAF3;void*_tmpAF5;void*_tmpAF7;struct Cyc_List_List*
_tmpAF9;_LL550: {struct Cyc_Absyn_VoidType_struct*_tmpA9B=(struct Cyc_Absyn_VoidType_struct*)
_tmpA9A;if(_tmpA9B->tag != 0)goto _LL552;}_LL551: goto _LL54F;_LL552: {struct Cyc_Absyn_Evar_struct*
_tmpA9C=(struct Cyc_Absyn_Evar_struct*)_tmpA9A;if(_tmpA9C->tag != 1)goto _LL554;
else{_tmpA9D=_tmpA9C->f1;_tmpA9E=(struct Cyc_Core_Opt**)& _tmpA9C->f1;_tmpA9F=
_tmpA9C->f2;_tmpAA0=(struct Cyc_Core_Opt**)& _tmpA9C->f2;}}_LL553: if(*_tmpA9E == 0
 || Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(*_tmpA9E))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(*_tmpA9E))->v,expected_kind))*_tmpA9E=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind
== Cyc_Absyn_RgnKind){if(expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmpAA0=(
struct Cyc_Core_Opt*)& urgn;else{*_tmpAA0=(struct Cyc_Core_Opt*)& hrgn;}}else{if(
cvtenv.generalize_evars){struct Cyc_Absyn_Less_kb_struct _tmp1384;struct Cyc_Absyn_Less_kb_struct*
_tmp1383;struct Cyc_Absyn_Tvar*_tmpAFA=Cyc_Tcutil_new_tvar((void*)((_tmp1383=
_cycalloc(sizeof(*_tmp1383)),((_tmp1383[0]=((_tmp1384.tag=2,((_tmp1384.f1=0,((
_tmp1384.f2=expected_kind,_tmp1384)))))),_tmp1383)))));{struct Cyc_Absyn_VarType_struct*
_tmp138A;struct Cyc_Absyn_VarType_struct _tmp1389;struct Cyc_Core_Opt*_tmp1388;*
_tmpAA0=((_tmp1388=_cycalloc(sizeof(*_tmp1388)),((_tmp1388->v=(void*)((_tmp138A=
_cycalloc(sizeof(*_tmp138A)),((_tmp138A[0]=((_tmp1389.tag=2,((_tmp1389.f1=
_tmpAFA,_tmp1389)))),_tmp138A)))),_tmp1388))));}_tmpAA2=_tmpAFA;goto _LL555;}
else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL54F;_LL554: {struct Cyc_Absyn_VarType_struct*_tmpAA1=(
struct Cyc_Absyn_VarType_struct*)_tmpA9A;if(_tmpAA1->tag != 2)goto _LL556;else{
_tmpAA2=_tmpAA1->f1;}}_LL555:{void*_tmpB00=Cyc_Absyn_compress_kb(_tmpAA2->kind);
struct Cyc_Core_Opt*_tmpB02;struct Cyc_Core_Opt**_tmpB03;_LL585: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpB01=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpB00;if(_tmpB01->tag != 1)goto
_LL587;else{_tmpB02=_tmpB01->f1;_tmpB03=(struct Cyc_Core_Opt**)& _tmpB01->f1;}}
_LL586:{struct Cyc_Absyn_Less_kb_struct*_tmp1390;struct Cyc_Absyn_Less_kb_struct
_tmp138F;struct Cyc_Core_Opt*_tmp138E;*_tmpB03=((_tmp138E=_cycalloc(sizeof(*
_tmp138E)),((_tmp138E->v=(void*)((_tmp1390=_cycalloc(sizeof(*_tmp1390)),((
_tmp1390[0]=((_tmp138F.tag=2,((_tmp138F.f1=0,((_tmp138F.f2=expected_kind,
_tmp138F)))))),_tmp1390)))),_tmp138E))));}goto _LL584;_LL587:;_LL588: goto _LL584;
_LL584:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpAA2);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpAA2,put_in_effect);{void*_tmpB07=Cyc_Absyn_compress_kb(_tmpAA2->kind);struct
Cyc_Core_Opt*_tmpB09;struct Cyc_Core_Opt**_tmpB0A;struct Cyc_Absyn_Kind*_tmpB0B;
_LL58A: {struct Cyc_Absyn_Less_kb_struct*_tmpB08=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB07;if(_tmpB08->tag != 2)goto _LL58C;else{_tmpB09=_tmpB08->f1;_tmpB0A=(struct
Cyc_Core_Opt**)& _tmpB08->f1;_tmpB0B=_tmpB08->f2;}}_LL58B: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpB0B)){struct Cyc_Absyn_Less_kb_struct*_tmp1396;struct Cyc_Absyn_Less_kb_struct
_tmp1395;struct Cyc_Core_Opt*_tmp1394;*_tmpB0A=((_tmp1394=_cycalloc(sizeof(*
_tmp1394)),((_tmp1394->v=(void*)((_tmp1396=_cycalloc(sizeof(*_tmp1396)),((
_tmp1396[0]=((_tmp1395.tag=2,((_tmp1395.f1=0,((_tmp1395.f2=expected_kind,
_tmp1395)))))),_tmp1396)))),_tmp1394))));}goto _LL589;_LL58C:;_LL58D: goto _LL589;
_LL589:;}goto _LL54F;_LL556: {struct Cyc_Absyn_AnonEnumType_struct*_tmpAA3=(struct
Cyc_Absyn_AnonEnumType_struct*)_tmpA9A;if(_tmpAA3->tag != 15)goto _LL558;else{
_tmpAA4=_tmpAA3->f1;}}_LL557: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmpB0F=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmpAA4 != 0;_tmpAA4=_tmpAA4->tl){
struct Cyc_Absyn_Enumfield*_tmpB10=(struct Cyc_Absyn_Enumfield*)_tmpAA4->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpB10->name).f2)){
const char*_tmp139A;void*_tmp1399[1];struct Cyc_String_pa_struct _tmp1398;(_tmp1398.tag=
0,((_tmp1398.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB10->name).f2),((
_tmp1399[0]=& _tmp1398,Cyc_Tcutil_terr(_tmpB10->loc,((_tmp139A="duplicate enum field name %s",
_tag_dyneither(_tmp139A,sizeof(char),29))),_tag_dyneither(_tmp1399,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp139B;prev_fields=((_tmp139B=
_region_malloc(_tmpB0F,sizeof(*_tmp139B)),((_tmp139B->hd=(*_tmpB10->name).f2,((
_tmp139B->tl=prev_fields,_tmp139B))))));}if(_tmpB10->tag == 0)_tmpB10->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpB10->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpB10->tag))){const char*_tmp139F;void*
_tmp139E[1];struct Cyc_String_pa_struct _tmp139D;(_tmp139D.tag=0,((_tmp139D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB10->name).f2),((_tmp139E[0]=&
_tmp139D,Cyc_Tcutil_terr(loc,((_tmp139F="enum field %s: expression is not constant",
_tag_dyneither(_tmp139F,sizeof(char),42))),_tag_dyneither(_tmp139E,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmpB10->tag))).f1;tag_count=t1 + 1;(*_tmpB10->name).f1=Cyc_Absyn_Abs_n(
te->ns,0);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp13A5;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp13A4;struct _tuple28*_tmp13A3;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple28*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpB10->name).f2,(struct _tuple28*)((_tmp13A3=_cycalloc(sizeof(*_tmp13A3)),((
_tmp13A3->f1=(void*)((_tmp13A5=_cycalloc(sizeof(*_tmp13A5)),((_tmp13A5[0]=((
_tmp13A4.tag=4,((_tmp13A4.f1=(void*)t,((_tmp13A4.f2=_tmpB10,_tmp13A4)))))),
_tmp13A5)))),((_tmp13A3->f2=1,_tmp13A3)))))));};};}}goto _LL54F;}_LL558: {struct
Cyc_Absyn_EnumType_struct*_tmpAA5=(struct Cyc_Absyn_EnumType_struct*)_tmpA9A;if(
_tmpAA5->tag != 14)goto _LL55A;else{_tmpAA6=_tmpAA5->f1;_tmpAA7=_tmpAA5->f2;
_tmpAA8=(struct Cyc_Absyn_Enumdecl**)& _tmpAA5->f2;}}_LL559: if(*_tmpAA8 == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpAA8))->fields == 0){struct
_handler_cons _tmpB1B;_push_handler(& _tmpB1B);{int _tmpB1D=0;if(setjmp(_tmpB1B.handler))
_tmpB1D=1;if(!_tmpB1D){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpAA6);*_tmpAA8=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpB1C=(void*)_exn_thrown;void*_tmpB1F=_tmpB1C;_LL58F: {struct Cyc_Dict_Absent_struct*
_tmpB20=(struct Cyc_Dict_Absent_struct*)_tmpB1F;if(_tmpB20->tag != Cyc_Dict_Absent)
goto _LL591;}_LL590: {struct Cyc_Tcenv_Genv*_tmpB21=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp13A6;struct Cyc_Absyn_Enumdecl*_tmpB22=(_tmp13A6=
_cycalloc(sizeof(*_tmp13A6)),((_tmp13A6->sc=Cyc_Absyn_Extern,((_tmp13A6->name=
_tmpAA6,((_tmp13A6->fields=0,_tmp13A6)))))));Cyc_Tc_tcEnumdecl(te,_tmpB21,loc,
_tmpB22);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpAA6);*
_tmpAA8=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL58E;};}_LL591:;_LL592:(void)_throw(
_tmpB1F);_LL58E:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpAA8);*_tmpAA6=(ed->name)[0];goto _LL54F;};_LL55A: {struct Cyc_Absyn_DatatypeType_struct*
_tmpAA9=(struct Cyc_Absyn_DatatypeType_struct*)_tmpA9A;if(_tmpAA9->tag != 3)goto
_LL55C;else{_tmpAAA=_tmpAA9->f1;_tmpAAB=_tmpAAA.datatype_info;_tmpAAC=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmpAA9->f1).datatype_info;_tmpAAD=_tmpAAA.targs;_tmpAAE=(struct Cyc_List_List**)&(
_tmpAA9->f1).targs;}}_LL55B: {struct Cyc_List_List*_tmpB24=*_tmpAAE;{union Cyc_Absyn_DatatypeInfoU
_tmpB25=*_tmpAAC;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB26;struct _tuple2*
_tmpB27;int _tmpB28;struct Cyc_Absyn_Datatypedecl**_tmpB29;struct Cyc_Absyn_Datatypedecl*
_tmpB2A;_LL594: if((_tmpB25.UnknownDatatype).tag != 1)goto _LL596;_tmpB26=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpB25.UnknownDatatype).val;_tmpB27=_tmpB26.name;
_tmpB28=_tmpB26.is_extensible;_LL595: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpB2B;_push_handler(& _tmpB2B);{int _tmpB2D=0;if(setjmp(
_tmpB2B.handler))_tmpB2D=1;if(!_tmpB2D){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpB27);;_pop_handler();}else{void*_tmpB2C=(void*)_exn_thrown;void*_tmpB2F=
_tmpB2C;_LL599: {struct Cyc_Dict_Absent_struct*_tmpB30=(struct Cyc_Dict_Absent_struct*)
_tmpB2F;if(_tmpB30->tag != Cyc_Dict_Absent)goto _LL59B;}_LL59A: {struct Cyc_Tcenv_Genv*
_tmpB31=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp13A7;struct Cyc_Absyn_Datatypedecl*
_tmpB32=(_tmp13A7=_cycalloc(sizeof(*_tmp13A7)),((_tmp13A7->sc=Cyc_Absyn_Extern,((
_tmp13A7->name=_tmpB27,((_tmp13A7->tvs=0,((_tmp13A7->fields=0,((_tmp13A7->is_extensible=
_tmpB28,_tmp13A7)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpB31,loc,_tmpB32);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpB27);if(_tmpB24 != 0){{const char*
_tmp13AB;void*_tmp13AA[1];struct Cyc_String_pa_struct _tmp13A9;(_tmp13A9.tag=0,((
_tmp13A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB27)),((_tmp13AA[0]=& _tmp13A9,Cyc_Tcutil_terr(loc,((_tmp13AB="declare parameterized datatype %s before using",
_tag_dyneither(_tmp13AB,sizeof(char),47))),_tag_dyneither(_tmp13AA,sizeof(void*),
1)))))));}return cvtenv;}goto _LL598;}_LL59B:;_LL59C:(void)_throw(_tmpB2F);_LL598:;}};}
if(_tmpB28  && !(*tudp)->is_extensible){const char*_tmp13AF;void*_tmp13AE[1];
struct Cyc_String_pa_struct _tmp13AD;(_tmp13AD.tag=0,((_tmp13AD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB27)),((
_tmp13AE[0]=& _tmp13AD,Cyc_Tcutil_terr(loc,((_tmp13AF="datatype %s was not declared @extensible",
_tag_dyneither(_tmp13AF,sizeof(char),41))),_tag_dyneither(_tmp13AE,sizeof(void*),
1)))))));}*_tmpAAC=Cyc_Absyn_KnownDatatype(tudp);_tmpB2A=*tudp;goto _LL597;}
_LL596: if((_tmpB25.KnownDatatype).tag != 2)goto _LL593;_tmpB29=(struct Cyc_Absyn_Datatypedecl**)(
_tmpB25.KnownDatatype).val;_tmpB2A=*_tmpB29;_LL597: {struct Cyc_List_List*tvs=
_tmpB2A->tvs;for(0;_tmpB24 != 0  && tvs != 0;(_tmpB24=_tmpB24->tl,tvs=tvs->tl)){
void*t=(void*)_tmpB24->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp13B0;struct _tuple0 _tmpB3B=(_tmp13B0.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp13B0.f2=t,_tmp13B0)));void*_tmpB3C;void*_tmpB3E;struct Cyc_Absyn_Tvar*_tmpB40;
_LL59E: _tmpB3C=_tmpB3B.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpB3D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB3C;if(_tmpB3D->tag != 1)goto _LL5A0;};_tmpB3E=_tmpB3B.f2;{struct Cyc_Absyn_VarType_struct*
_tmpB3F=(struct Cyc_Absyn_VarType_struct*)_tmpB3E;if(_tmpB3F->tag != 2)goto _LL5A0;
else{_tmpB40=_tmpB3F->f1;}};_LL59F: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpB40);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpB40,1);continue;_LL5A0:;_LL5A1: goto _LL59D;_LL59D:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmpB24 != 0){const char*_tmp13B4;void*_tmp13B3[1];
struct Cyc_String_pa_struct _tmp13B2;(_tmp13B2.tag=0,((_tmp13B2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB2A->name)),((
_tmp13B3[0]=& _tmp13B2,Cyc_Tcutil_terr(loc,((_tmp13B4="too many type arguments for datatype %s",
_tag_dyneither(_tmp13B4,sizeof(char),40))),_tag_dyneither(_tmp13B3,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp13B5;hidden_ts=((_tmp13B5=_cycalloc(sizeof(*_tmp13B5)),((_tmp13B5->hd=e,((
_tmp13B5->tl=hidden_ts,_tmp13B5))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k1,e,1);}*_tmpAAE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpAAE,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL593;}_LL593:;}goto
_LL54F;}_LL55C: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpAAF=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpA9A;if(_tmpAAF->tag != 4)goto _LL55E;else{_tmpAB0=_tmpAAF->f1;_tmpAB1=_tmpAB0.field_info;
_tmpAB2=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpAAF->f1).field_info;_tmpAB3=
_tmpAB0.targs;}}_LL55D:{union Cyc_Absyn_DatatypeFieldInfoU _tmpB45=*_tmpAB2;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB46;struct _tuple2*_tmpB47;struct _tuple2*
_tmpB48;int _tmpB49;struct _tuple3 _tmpB4A;struct Cyc_Absyn_Datatypedecl*_tmpB4B;
struct Cyc_Absyn_Datatypefield*_tmpB4C;_LL5A3: if((_tmpB45.UnknownDatatypefield).tag
!= 1)goto _LL5A5;_tmpB46=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpB45.UnknownDatatypefield).val;
_tmpB47=_tmpB46.datatype_name;_tmpB48=_tmpB46.field_name;_tmpB49=_tmpB46.is_extensible;
_LL5A4: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpB4D;_push_handler(& _tmpB4D);{int _tmpB4F=0;if(setjmp(
_tmpB4D.handler))_tmpB4F=1;if(!_tmpB4F){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpB47);;_pop_handler();}else{void*_tmpB4E=(void*)_exn_thrown;void*_tmpB51=
_tmpB4E;_LL5A8: {struct Cyc_Dict_Absent_struct*_tmpB52=(struct Cyc_Dict_Absent_struct*)
_tmpB51;if(_tmpB52->tag != Cyc_Dict_Absent)goto _LL5AA;}_LL5A9:{const char*_tmp13B9;
void*_tmp13B8[1];struct Cyc_String_pa_struct _tmp13B7;(_tmp13B7.tag=0,((_tmp13B7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB47)),((
_tmp13B8[0]=& _tmp13B7,Cyc_Tcutil_terr(loc,((_tmp13B9="unbound datatype %s",
_tag_dyneither(_tmp13B9,sizeof(char),20))),_tag_dyneither(_tmp13B8,sizeof(void*),
1)))))));}return cvtenv;_LL5AA:;_LL5AB:(void)_throw(_tmpB51);_LL5A7:;}};}{struct
_handler_cons _tmpB56;_push_handler(& _tmpB56);{int _tmpB58=0;if(setjmp(_tmpB56.handler))
_tmpB58=1;if(!_tmpB58){{struct _RegionHandle*_tmpB59=Cyc_Tcenv_get_fnrgn(te);void*
_tmpB5A=Cyc_Tcenv_lookup_ordinary(_tmpB59,te,loc,_tmpB48);struct Cyc_Absyn_Datatypedecl*
_tmpB5C;struct Cyc_Absyn_Datatypefield*_tmpB5D;_LL5AD: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpB5B=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpB5A;if(_tmpB5B->tag != 2)goto
_LL5AF;else{_tmpB5C=_tmpB5B->f1;_tmpB5D=_tmpB5B->f2;}}_LL5AE: tuf=_tmpB5D;tud=
_tmpB5C;if(_tmpB49  && !tud->is_extensible){const char*_tmp13BD;void*_tmp13BC[1];
struct Cyc_String_pa_struct _tmp13BB;(_tmp13BB.tag=0,((_tmp13BB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB47)),((
_tmp13BC[0]=& _tmp13BB,Cyc_Tcutil_terr(loc,((_tmp13BD="datatype %s was not declared @extensible",
_tag_dyneither(_tmp13BD,sizeof(char),41))),_tag_dyneither(_tmp13BC,sizeof(void*),
1)))))));}goto _LL5AC;_LL5AF:;_LL5B0:{const char*_tmp13C2;void*_tmp13C1[2];struct
Cyc_String_pa_struct _tmp13C0;struct Cyc_String_pa_struct _tmp13BF;(_tmp13BF.tag=0,((
_tmp13BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB47)),((_tmp13C0.tag=0,((_tmp13C0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB48)),((_tmp13C1[0]=& _tmp13C0,((
_tmp13C1[1]=& _tmp13BF,Cyc_Tcutil_terr(loc,((_tmp13C2="unbound field %s in type datatype %s",
_tag_dyneither(_tmp13C2,sizeof(char),37))),_tag_dyneither(_tmp13C1,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpB65=cvtenv;_npop_handler(0);return
_tmpB65;};_LL5AC:;};_pop_handler();}else{void*_tmpB57=(void*)_exn_thrown;void*
_tmpB67=_tmpB57;_LL5B2: {struct Cyc_Dict_Absent_struct*_tmpB68=(struct Cyc_Dict_Absent_struct*)
_tmpB67;if(_tmpB68->tag != Cyc_Dict_Absent)goto _LL5B4;}_LL5B3:{const char*_tmp13C7;
void*_tmp13C6[2];struct Cyc_String_pa_struct _tmp13C5;struct Cyc_String_pa_struct
_tmp13C4;(_tmp13C4.tag=0,((_tmp13C4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB47)),((_tmp13C5.tag=0,((_tmp13C5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB48)),((
_tmp13C6[0]=& _tmp13C5,((_tmp13C6[1]=& _tmp13C4,Cyc_Tcutil_terr(loc,((_tmp13C7="unbound field %s in type datatype %s",
_tag_dyneither(_tmp13C7,sizeof(char),37))),_tag_dyneither(_tmp13C6,sizeof(void*),
2)))))))))))));}return cvtenv;_LL5B4:;_LL5B5:(void)_throw(_tmpB67);_LL5B1:;}};}*
_tmpAB2=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpB4B=tud;_tmpB4C=tuf;goto _LL5A6;}
_LL5A5: if((_tmpB45.KnownDatatypefield).tag != 2)goto _LL5A2;_tmpB4A=(struct _tuple3)(
_tmpB45.KnownDatatypefield).val;_tmpB4B=_tmpB4A.f1;_tmpB4C=_tmpB4A.f2;_LL5A6: {
struct Cyc_List_List*tvs=_tmpB4B->tvs;for(0;_tmpAB3 != 0  && tvs != 0;(_tmpAB3=
_tmpAB3->tl,tvs=tvs->tl)){void*t=(void*)_tmpAB3->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp13C8;struct _tuple0 _tmpB6E=(
_tmp13C8.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp13C8.f2=t,_tmp13C8)));void*
_tmpB6F;void*_tmpB71;struct Cyc_Absyn_Tvar*_tmpB73;_LL5B7: _tmpB6F=_tmpB6E.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpB70=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB6F;if(_tmpB70->tag != 1)goto _LL5B9;};_tmpB71=_tmpB6E.f2;{struct Cyc_Absyn_VarType_struct*
_tmpB72=(struct Cyc_Absyn_VarType_struct*)_tmpB71;if(_tmpB72->tag != 2)goto _LL5B9;
else{_tmpB73=_tmpB72->f1;}};_LL5B8: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpB73);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpB73,1);continue;_LL5B9:;_LL5BA: goto _LL5B6;_LL5B6:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmpAB3 != 0){const char*_tmp13CD;void*_tmp13CC[2];
struct Cyc_String_pa_struct _tmp13CB;struct Cyc_String_pa_struct _tmp13CA;(_tmp13CA.tag=
0,((_tmp13CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4C->name)),((_tmp13CB.tag=0,((_tmp13CB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4B->name)),((_tmp13CC[0]=& _tmp13CB,((
_tmp13CC[1]=& _tmp13CA,Cyc_Tcutil_terr(loc,((_tmp13CD="too many type arguments for datatype %s.%s",
_tag_dyneither(_tmp13CD,sizeof(char),43))),_tag_dyneither(_tmp13CC,sizeof(void*),
2)))))))))))));}if(tvs != 0){const char*_tmp13D2;void*_tmp13D1[2];struct Cyc_String_pa_struct
_tmp13D0;struct Cyc_String_pa_struct _tmp13CF;(_tmp13CF.tag=0,((_tmp13CF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4C->name)),((
_tmp13D0.tag=0,((_tmp13D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4B->name)),((_tmp13D1[0]=& _tmp13D0,((_tmp13D1[1]=& _tmp13CF,Cyc_Tcutil_terr(
loc,((_tmp13D2="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp13D2,sizeof(char),42))),_tag_dyneither(_tmp13D1,sizeof(void*),2)))))))))))));}
goto _LL5A2;}_LL5A2:;}goto _LL54F;_LL55E: {struct Cyc_Absyn_PointerType_struct*
_tmpAB4=(struct Cyc_Absyn_PointerType_struct*)_tmpA9A;if(_tmpAB4->tag != 5)goto
_LL560;else{_tmpAB5=_tmpAB4->f1;_tmpAB6=_tmpAB5.elt_typ;_tmpAB7=_tmpAB5.elt_tq;
_tmpAB8=(struct Cyc_Absyn_Tqual*)&(_tmpAB4->f1).elt_tq;_tmpAB9=_tmpAB5.ptr_atts;
_tmpABA=_tmpAB9.rgn;_tmpABB=_tmpAB9.nullable;_tmpABC=_tmpAB9.bounds;_tmpABD=
_tmpAB9.zero_term;}}_LL55F: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpAB6,1);_tmpAB8->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpAB8->print_const,_tmpAB6);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL5BB: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL5BC: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5BD: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpABA,1);{union Cyc_Absyn_Constraint*
_tmpB7C=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpABD);union Cyc_Absyn_Constraint _tmpB7D;int _tmpB7E;union Cyc_Absyn_Constraint
_tmpB7F;int _tmpB80;_LL5C0: _tmpB7D=*_tmpB7C;if((_tmpB7D.No_constr).tag != 3)goto
_LL5C2;_tmpB7E=(int)(_tmpB7D.No_constr).val;_LL5C1:{void*_tmpB81=Cyc_Tcutil_compress(
_tmpAB6);enum Cyc_Absyn_Size_of _tmpB83;_LL5C7: {struct Cyc_Absyn_IntType_struct*
_tmpB82=(struct Cyc_Absyn_IntType_struct*)_tmpB81;if(_tmpB82->tag != 6)goto _LL5C9;
else{_tmpB83=_tmpB82->f2;if(_tmpB83 != Cyc_Absyn_Char_sz)goto _LL5C9;}}_LL5C8:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpABD,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL5C6;_LL5C9:;_LL5CA:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpABD,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL5C6;
_LL5C6:;}goto _LL5BF;_LL5C2: _tmpB7F=*_tmpB7C;if((_tmpB7F.Eq_constr).tag != 1)goto
_LL5C4;_tmpB80=(int)(_tmpB7F.Eq_constr).val;if(_tmpB80 != 1)goto _LL5C4;_LL5C3: if(
!Cyc_Tcutil_admits_zero(_tmpAB6)){const char*_tmp13D6;void*_tmp13D5[1];struct Cyc_String_pa_struct
_tmp13D4;(_tmp13D4.tag=0,((_tmp13D4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmpAB6)),((_tmp13D5[0]=& _tmp13D4,Cyc_Tcutil_terr(
loc,((_tmp13D6="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp13D6,sizeof(char),53))),_tag_dyneither(_tmp13D5,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5BF;_LL5C4:;_LL5C5: is_zero_terminated=0;
goto _LL5BF;_LL5BF:;}{void*_tmpB87=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpABC);struct Cyc_Absyn_Exp*
_tmpB8A;_LL5CC: {struct Cyc_Absyn_DynEither_b_struct*_tmpB88=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpB87;if(_tmpB88->tag != 0)goto _LL5CE;}_LL5CD: goto _LL5CB;_LL5CE: {struct Cyc_Absyn_Upper_b_struct*
_tmpB89=(struct Cyc_Absyn_Upper_b_struct*)_tmpB87;if(_tmpB89->tag != 1)goto _LL5CB;
else{_tmpB8A=_tmpB89->f1;}}_LL5CF: {struct _RegionHandle*_tmpB8B=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpB8C=Cyc_Tcenv_allow_valueof(_tmpB8B,te);Cyc_Tcexp_tcExp(
_tmpB8C,0,_tmpB8A);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB8A,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB8A)){const char*_tmp13D9;void*
_tmp13D8;(_tmp13D8=0,Cyc_Tcutil_terr(loc,((_tmp13D9="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp13D9,sizeof(char),49))),_tag_dyneither(_tmp13D8,sizeof(void*),
0)));}{unsigned int _tmpB90;int _tmpB91;struct _tuple13 _tmpB8F=Cyc_Evexp_eval_const_uint_exp(
_tmpB8A);_tmpB90=_tmpB8F.f1;_tmpB91=_tmpB8F.f2;if(is_zero_terminated  && (!
_tmpB91  || _tmpB90 < 1)){const char*_tmp13DC;void*_tmp13DB;(_tmp13DB=0,Cyc_Tcutil_terr(
loc,((_tmp13DC="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp13DC,sizeof(char),55))),_tag_dyneither(_tmp13DB,sizeof(void*),
0)));}goto _LL5CB;};}_LL5CB:;}goto _LL54F;};}_LL560: {struct Cyc_Absyn_TagType_struct*
_tmpABE=(struct Cyc_Absyn_TagType_struct*)_tmpA9A;if(_tmpABE->tag != 20)goto _LL562;
else{_tmpABF=(void*)_tmpABE->f1;}}_LL561: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpABF,1);goto _LL54F;_LL562: {struct Cyc_Absyn_ValueofType_struct*
_tmpAC0=(struct Cyc_Absyn_ValueofType_struct*)_tmpA9A;if(_tmpAC0->tag != 19)goto
_LL564;else{_tmpAC1=_tmpAC0->f1;}}_LL563: {struct _RegionHandle*_tmpB94=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpB95=Cyc_Tcenv_allow_valueof(_tmpB94,te);Cyc_Tcexp_tcExp(
_tmpB95,0,_tmpAC1);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpAC1)){const char*
_tmp13DF;void*_tmp13DE;(_tmp13DE=0,Cyc_Tcutil_terr(loc,((_tmp13DF="valueof_t requires an int expression",
_tag_dyneither(_tmp13DF,sizeof(char),37))),_tag_dyneither(_tmp13DE,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAC1,te,cvtenv);goto
_LL54F;}_LL564: {struct Cyc_Absyn_IntType_struct*_tmpAC2=(struct Cyc_Absyn_IntType_struct*)
_tmpA9A;if(_tmpAC2->tag != 6)goto _LL566;}_LL565: goto _LL567;_LL566: {struct Cyc_Absyn_FloatType_struct*
_tmpAC3=(struct Cyc_Absyn_FloatType_struct*)_tmpA9A;if(_tmpAC3->tag != 7)goto
_LL568;}_LL567: goto _LL569;_LL568: {struct Cyc_Absyn_DoubleType_struct*_tmpAC4=(
struct Cyc_Absyn_DoubleType_struct*)_tmpA9A;if(_tmpAC4->tag != 8)goto _LL56A;}
_LL569: goto _LL54F;_LL56A: {struct Cyc_Absyn_ArrayType_struct*_tmpAC5=(struct Cyc_Absyn_ArrayType_struct*)
_tmpA9A;if(_tmpAC5->tag != 9)goto _LL56C;else{_tmpAC6=_tmpAC5->f1;_tmpAC7=_tmpAC6.elt_type;
_tmpAC8=_tmpAC6.tq;_tmpAC9=(struct Cyc_Absyn_Tqual*)&(_tmpAC5->f1).tq;_tmpACA=
_tmpAC6.num_elts;_tmpACB=(struct Cyc_Absyn_Exp**)&(_tmpAC5->f1).num_elts;_tmpACC=
_tmpAC6.zero_term;_tmpACD=_tmpAC6.zt_loc;}}_LL56B: {struct Cyc_Absyn_Exp*_tmpB98=*
_tmpACB;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,
_tmpAC7,1);_tmpAC9->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpAC9->print_const,
_tmpAC7);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpB99=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpACC);union Cyc_Absyn_Constraint
_tmpB9A;int _tmpB9B;union Cyc_Absyn_Constraint _tmpB9C;int _tmpB9D;_LL5D1: _tmpB9A=*
_tmpB99;if((_tmpB9A.No_constr).tag != 3)goto _LL5D3;_tmpB9B=(int)(_tmpB9A.No_constr).val;
_LL5D2:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpACC,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL5D0;_LL5D3: _tmpB9C=*_tmpB99;if((_tmpB9C.Eq_constr).tag
!= 1)goto _LL5D5;_tmpB9D=(int)(_tmpB9C.Eq_constr).val;if(_tmpB9D != 1)goto _LL5D5;
_LL5D4: if(!Cyc_Tcutil_admits_zero(_tmpAC7)){const char*_tmp13E3;void*_tmp13E2[1];
struct Cyc_String_pa_struct _tmp13E1;(_tmp13E1.tag=0,((_tmp13E1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpAC7)),((
_tmp13E2[0]=& _tmp13E1,Cyc_Tcutil_terr(loc,((_tmp13E3="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp13E3,sizeof(char),51))),_tag_dyneither(_tmp13E2,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5D0;_LL5D5:;_LL5D6: is_zero_terminated=0;
goto _LL5D0;_LL5D0:;}if(_tmpB98 == 0){if(is_zero_terminated)*_tmpACB=(_tmpB98=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp13E6;void*
_tmp13E5;(_tmp13E5=0,Cyc_Tcutil_warn(loc,((_tmp13E6="array bound defaults to 1 here",
_tag_dyneither(_tmp13E6,sizeof(char),31))),_tag_dyneither(_tmp13E5,sizeof(void*),
0)));}*_tmpACB=(_tmpB98=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpB98);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpB98)){const char*_tmp13E9;void*_tmp13E8;(_tmp13E8=0,Cyc_Tcutil_terr(loc,((
_tmp13E9="array bounds expression is not constant",_tag_dyneither(_tmp13E9,
sizeof(char),40))),_tag_dyneither(_tmp13E8,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpB98)){const char*_tmp13EC;void*_tmp13EB;(_tmp13EB=0,
Cyc_Tcutil_terr(loc,((_tmp13EC="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp13EC,sizeof(char),47))),_tag_dyneither(_tmp13EB,sizeof(void*),
0)));}{unsigned int _tmpBA8;int _tmpBA9;struct _tuple13 _tmpBA7=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpB98);_tmpBA8=_tmpBA7.f1;_tmpBA9=_tmpBA7.f2;if((
is_zero_terminated  && _tmpBA9) && _tmpBA8 < 1){const char*_tmp13EF;void*_tmp13EE;(
_tmp13EE=0,Cyc_Tcutil_warn(loc,((_tmp13EF="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp13EF,sizeof(char),48))),_tag_dyneither(_tmp13EE,sizeof(void*),
0)));}if((_tmpBA9  && _tmpBA8 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp13F2;void*
_tmp13F1;(_tmp13F1=0,Cyc_Tcutil_warn(loc,((_tmp13F2="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp13F2,sizeof(char),75))),_tag_dyneither(_tmp13F1,sizeof(void*),
0)));}*_tmpACB=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL54F;};};}
_LL56C: {struct Cyc_Absyn_FnType_struct*_tmpACE=(struct Cyc_Absyn_FnType_struct*)
_tmpA9A;if(_tmpACE->tag != 10)goto _LL56E;else{_tmpACF=_tmpACE->f1;_tmpAD0=_tmpACF.tvars;
_tmpAD1=(struct Cyc_List_List**)&(_tmpACE->f1).tvars;_tmpAD2=_tmpACF.effect;
_tmpAD3=(struct Cyc_Core_Opt**)&(_tmpACE->f1).effect;_tmpAD4=_tmpACF.ret_typ;
_tmpAD5=_tmpACF.args;_tmpAD6=_tmpACF.c_varargs;_tmpAD7=_tmpACF.cyc_varargs;
_tmpAD8=_tmpACF.rgn_po;_tmpAD9=_tmpACF.attributes;}}_LL56D: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpAD9 != 0;
_tmpAD9=_tmpAD9->tl){if(!Cyc_Absyn_fntype_att((void*)_tmpAD9->hd)){const char*
_tmp13F6;void*_tmp13F5[1];struct Cyc_String_pa_struct _tmp13F4;(_tmp13F4.tag=0,((
_tmp13F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpAD9->hd)),((_tmp13F5[0]=& _tmp13F4,Cyc_Tcutil_terr(loc,((_tmp13F6="bad function type attribute %s",
_tag_dyneither(_tmp13F6,sizeof(char),31))),_tag_dyneither(_tmp13F5,sizeof(void*),
1)))))));}{void*_tmpBB1=(void*)_tmpAD9->hd;enum Cyc_Absyn_Format_Type _tmpBB6;int
_tmpBB7;int _tmpBB8;_LL5D8: {struct Cyc_Absyn_Stdcall_att_struct*_tmpBB2=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpBB1;if(_tmpBB2->tag != 1)goto _LL5DA;}_LL5D9: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL5D7;_LL5DA: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpBB3=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpBB1;if(_tmpBB3->tag != 2)goto
_LL5DC;}_LL5DB: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL5D7;_LL5DC: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpBB4=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpBB1;if(_tmpBB4->tag != 3)goto _LL5DE;}_LL5DD: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL5D7;_LL5DE: {struct Cyc_Absyn_Format_att_struct*_tmpBB5=(
struct Cyc_Absyn_Format_att_struct*)_tmpBB1;if(_tmpBB5->tag != 19)goto _LL5E0;else{
_tmpBB6=_tmpBB5->f1;_tmpBB7=_tmpBB5->f2;_tmpBB8=_tmpBB5->f3;}}_LL5DF: if(!
seen_format){seen_format=1;ft=_tmpBB6;fmt_desc_arg=_tmpBB7;fmt_arg_start=_tmpBB8;}
else{const char*_tmp13F9;void*_tmp13F8;(_tmp13F8=0,Cyc_Tcutil_terr(loc,((_tmp13F9="function can't have multiple format attributes",
_tag_dyneither(_tmp13F9,sizeof(char),47))),_tag_dyneither(_tmp13F8,sizeof(void*),
0)));}goto _LL5D7;_LL5E0:;_LL5E1: goto _LL5D7;_LL5D7:;};}if(num_convs > 1){const char*
_tmp13FC;void*_tmp13FB;(_tmp13FB=0,Cyc_Tcutil_terr(loc,((_tmp13FC="function can't have multiple calling conventions",
_tag_dyneither(_tmp13FC,sizeof(char),49))),_tag_dyneither(_tmp13FB,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmpAD1);{struct Cyc_List_List*b=*
_tmpAD1;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpBBD=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpBBF;struct Cyc_Absyn_Kind _tmpBC0;enum Cyc_Absyn_KindQual
_tmpBC1;_LL5E3: {struct Cyc_Absyn_Eq_kb_struct*_tmpBBE=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpBBD;if(_tmpBBE->tag != 0)goto _LL5E5;else{_tmpBBF=_tmpBBE->f1;_tmpBC0=*_tmpBBF;
_tmpBC1=_tmpBC0.kind;if(_tmpBC1 != Cyc_Absyn_MemKind)goto _LL5E5;}}_LL5E4:{const
char*_tmp1400;void*_tmp13FF[1];struct Cyc_String_pa_struct _tmp13FE;(_tmp13FE.tag=
0,((_tmp13FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp13FF[0]=& _tmp13FE,Cyc_Tcutil_terr(loc,((_tmp1400="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp1400,sizeof(char),51))),_tag_dyneither(_tmp13FF,sizeof(void*),
1)))))));}goto _LL5E2;_LL5E5:;_LL5E6: goto _LL5E2;_LL5E2:;};}}{struct _RegionHandle*
_tmpBC5=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp1401;struct Cyc_Tcutil_CVTEnv
_tmpBC6=(_tmp1401.r=_tmpBC5,((_tmp1401.kind_env=cvtenv.kind_env,((_tmp1401.free_vars=
0,((_tmp1401.free_evars=0,((_tmp1401.generalize_evars=cvtenv.generalize_evars,((
_tmp1401.fn_result=1,_tmp1401)))))))))));_tmpBC6=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC6,& Cyc_Tcutil_tmk,_tmpAD4,1);_tmpBC6.fn_result=0;{struct Cyc_List_List*
a=_tmpAD5;for(0;a != 0;a=a->tl){struct _tuple9*_tmpBC7=(struct _tuple9*)a->hd;void*
_tmpBC8=(*_tmpBC7).f3;_tmpBC6=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpBC6,& Cyc_Tcutil_tmk,
_tmpBC8,1);((*_tmpBC7).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmpBC7).f2).print_const,_tmpBC8);}}if(_tmpAD7 != 0){if(_tmpAD6){const char*
_tmp1404;void*_tmp1403;(_tmp1403=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1404="both c_vararg and cyc_vararg",
_tag_dyneither(_tmp1404,sizeof(char),29))),_tag_dyneither(_tmp1403,sizeof(void*),
0)));}{void*_tmpBCC;int _tmpBCD;struct Cyc_Absyn_VarargInfo _tmpBCB=*_tmpAD7;
_tmpBCC=_tmpBCB.type;_tmpBCD=_tmpBCB.inject;_tmpBC6=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC6,& Cyc_Tcutil_tmk,_tmpBCC,1);(_tmpAD7->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmpAD7->tq).print_const,_tmpBCC);if(_tmpBCD){void*_tmpBCE=Cyc_Tcutil_compress(
_tmpBCC);struct Cyc_Absyn_PtrInfo _tmpBD0;void*_tmpBD1;struct Cyc_Absyn_PtrAtts
_tmpBD2;union Cyc_Absyn_Constraint*_tmpBD3;union Cyc_Absyn_Constraint*_tmpBD4;
_LL5E8: {struct Cyc_Absyn_PointerType_struct*_tmpBCF=(struct Cyc_Absyn_PointerType_struct*)
_tmpBCE;if(_tmpBCF->tag != 5)goto _LL5EA;else{_tmpBD0=_tmpBCF->f1;_tmpBD1=_tmpBD0.elt_typ;
_tmpBD2=_tmpBD0.ptr_atts;_tmpBD3=_tmpBD2.bounds;_tmpBD4=_tmpBD2.zero_term;}}
_LL5E9:{void*_tmpBD5=Cyc_Tcutil_compress(_tmpBD1);_LL5ED: {struct Cyc_Absyn_DatatypeType_struct*
_tmpBD6=(struct Cyc_Absyn_DatatypeType_struct*)_tmpBD5;if(_tmpBD6->tag != 3)goto
_LL5EF;}_LL5EE: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpBD4)){const char*_tmp1407;void*_tmp1406;(_tmp1406=0,Cyc_Tcutil_terr(loc,((
_tmp1407="can't inject into a zeroterm pointer",_tag_dyneither(_tmp1407,sizeof(
char),37))),_tag_dyneither(_tmp1406,sizeof(void*),0)));}{void*_tmpBD9=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpBD3);_LL5F2: {struct Cyc_Absyn_DynEither_b_struct*_tmpBDA=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpBD9;if(_tmpBDA->tag != 0)goto _LL5F4;}_LL5F3:{const char*_tmp140A;void*_tmp1409;(
_tmp1409=0,Cyc_Tcutil_terr(loc,((_tmp140A="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp140A,sizeof(char),44))),_tag_dyneither(_tmp1409,sizeof(void*),
0)));}goto _LL5F1;_LL5F4:;_LL5F5: goto _LL5F1;_LL5F1:;}goto _LL5EC;_LL5EF:;_LL5F0:{
const char*_tmp140D;void*_tmp140C;(_tmp140C=0,Cyc_Tcutil_terr(loc,((_tmp140D="can't inject a non-datatype type",
_tag_dyneither(_tmp140D,sizeof(char),33))),_tag_dyneither(_tmp140C,sizeof(void*),
0)));}goto _LL5EC;_LL5EC:;}goto _LL5E7;_LL5EA:;_LL5EB:{const char*_tmp1410;void*
_tmp140F;(_tmp140F=0,Cyc_Tcutil_terr(loc,((_tmp1410="expecting a datatype pointer type",
_tag_dyneither(_tmp1410,sizeof(char),34))),_tag_dyneither(_tmp140F,sizeof(void*),
0)));}goto _LL5E7;_LL5E7:;}};}if(seen_format){int _tmpBE1=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpAD5);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpBE1) || 
fmt_arg_start < 0) || _tmpAD7 == 0  && fmt_arg_start != 0) || _tmpAD7 != 0  && 
fmt_arg_start != _tmpBE1 + 1){const char*_tmp1413;void*_tmp1412;(_tmp1412=0,Cyc_Tcutil_terr(
loc,((_tmp1413="bad format descriptor",_tag_dyneither(_tmp1413,sizeof(char),22))),
_tag_dyneither(_tmp1412,sizeof(void*),0)));}else{void*_tmpBE5;struct _tuple9
_tmpBE4=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpAD5,
fmt_desc_arg - 1);_tmpBE5=_tmpBE4.f3;{void*_tmpBE6=Cyc_Tcutil_compress(_tmpBE5);
struct Cyc_Absyn_PtrInfo _tmpBE8;void*_tmpBE9;struct Cyc_Absyn_PtrAtts _tmpBEA;union
Cyc_Absyn_Constraint*_tmpBEB;union Cyc_Absyn_Constraint*_tmpBEC;_LL5F7: {struct
Cyc_Absyn_PointerType_struct*_tmpBE7=(struct Cyc_Absyn_PointerType_struct*)
_tmpBE6;if(_tmpBE7->tag != 5)goto _LL5F9;else{_tmpBE8=_tmpBE7->f1;_tmpBE9=_tmpBE8.elt_typ;
_tmpBEA=_tmpBE8.ptr_atts;_tmpBEB=_tmpBEA.bounds;_tmpBEC=_tmpBEA.zero_term;}}
_LL5F8:{struct _tuple0 _tmp1414;struct _tuple0 _tmpBEE=(_tmp1414.f1=Cyc_Tcutil_compress(
_tmpBE9),((_tmp1414.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpBEB),_tmp1414)));void*_tmpBEF;enum Cyc_Absyn_Sign
_tmpBF1;enum Cyc_Absyn_Size_of _tmpBF2;void*_tmpBF3;_LL5FC: _tmpBEF=_tmpBEE.f1;{
struct Cyc_Absyn_IntType_struct*_tmpBF0=(struct Cyc_Absyn_IntType_struct*)_tmpBEF;
if(_tmpBF0->tag != 6)goto _LL5FE;else{_tmpBF1=_tmpBF0->f1;if(_tmpBF1 != Cyc_Absyn_None)
goto _LL5FE;_tmpBF2=_tmpBF0->f2;if(_tmpBF2 != Cyc_Absyn_Char_sz)goto _LL5FE;}};
_tmpBF3=_tmpBEE.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpBF4=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpBF3;if(_tmpBF4->tag != 0)goto _LL5FE;};_LL5FD: goto _LL5FB;_LL5FE:;_LL5FF:{const
char*_tmp1417;void*_tmp1416;(_tmp1416=0,Cyc_Tcutil_terr(loc,((_tmp1417="format descriptor is not a char ? type",
_tag_dyneither(_tmp1417,sizeof(char),39))),_tag_dyneither(_tmp1416,sizeof(void*),
0)));}goto _LL5FB;_LL5FB:;}goto _LL5F6;_LL5F9:;_LL5FA:{const char*_tmp141A;void*
_tmp1419;(_tmp1419=0,Cyc_Tcutil_terr(loc,((_tmp141A="format descriptor is not a char ? type",
_tag_dyneither(_tmp141A,sizeof(char),39))),_tag_dyneither(_tmp1419,sizeof(void*),
0)));}goto _LL5F6;_LL5F6:;}if(fmt_arg_start != 0){int problem;{struct _tuple29
_tmp141B;struct _tuple29 _tmpBFA=(_tmp141B.f1=ft,((_tmp141B.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpAD7))->type)),
_tmp141B)));enum Cyc_Absyn_Format_Type _tmpBFB;void*_tmpBFC;struct Cyc_Absyn_DatatypeInfo
_tmpBFE;union Cyc_Absyn_DatatypeInfoU _tmpBFF;struct Cyc_Absyn_Datatypedecl**
_tmpC00;struct Cyc_Absyn_Datatypedecl*_tmpC01;enum Cyc_Absyn_Format_Type _tmpC02;
void*_tmpC03;struct Cyc_Absyn_DatatypeInfo _tmpC05;union Cyc_Absyn_DatatypeInfoU
_tmpC06;struct Cyc_Absyn_Datatypedecl**_tmpC07;struct Cyc_Absyn_Datatypedecl*
_tmpC08;_LL601: _tmpBFB=_tmpBFA.f1;if(_tmpBFB != Cyc_Absyn_Printf_ft)goto _LL603;
_tmpBFC=_tmpBFA.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpBFD=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpBFC;if(_tmpBFD->tag != 3)goto _LL603;else{_tmpBFE=_tmpBFD->f1;_tmpBFF=_tmpBFE.datatype_info;
if((_tmpBFF.KnownDatatype).tag != 2)goto _LL603;_tmpC00=(struct Cyc_Absyn_Datatypedecl**)(
_tmpBFF.KnownDatatype).val;_tmpC01=*_tmpC00;}};_LL602: problem=Cyc_Absyn_qvar_cmp(
_tmpC01->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL600;_LL603: _tmpC02=
_tmpBFA.f1;if(_tmpC02 != Cyc_Absyn_Scanf_ft)goto _LL605;_tmpC03=_tmpBFA.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpC04=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC03;if(_tmpC04->tag != 3)goto _LL605;else{_tmpC05=_tmpC04->f1;_tmpC06=_tmpC05.datatype_info;
if((_tmpC06.KnownDatatype).tag != 2)goto _LL605;_tmpC07=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC06.KnownDatatype).val;_tmpC08=*_tmpC07;}};_LL604: problem=Cyc_Absyn_qvar_cmp(
_tmpC08->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL600;_LL605:;_LL606:
problem=1;goto _LL600;_LL600:;}if(problem){const char*_tmp141E;void*_tmp141D;(
_tmp141D=0,Cyc_Tcutil_terr(loc,((_tmp141E="format attribute and vararg types don't match",
_tag_dyneither(_tmp141E,sizeof(char),46))),_tag_dyneither(_tmp141D,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmpAD8;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpC0C;void*_tmpC0D;void*_tmpC0E;struct _tuple0*_tmpC0B=(struct _tuple0*)
rpo->hd;_tmpC0C=*_tmpC0B;_tmpC0D=_tmpC0C.f1;_tmpC0E=_tmpC0C.f2;_tmpBC6=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC6,& Cyc_Tcutil_ek,_tmpC0D,1);_tmpBC6=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC6,& Cyc_Tcutil_trk,_tmpC0E,1);}}if(*_tmpAD3 != 0)_tmpBC6=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC6,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmpAD3))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpBC6.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpC10;int _tmpC11;struct
_tuple27 _tmpC0F=*((struct _tuple27*)tvs->hd);_tmpC10=_tmpC0F.f1;_tmpC11=_tmpC0F.f2;
if(!_tmpC11)continue;{void*_tmpC12=Cyc_Absyn_compress_kb(_tmpC10->kind);struct
Cyc_Core_Opt*_tmpC14;struct Cyc_Core_Opt**_tmpC15;struct Cyc_Absyn_Kind*_tmpC16;
struct Cyc_Absyn_Kind*_tmpC18;struct Cyc_Absyn_Kind*_tmpC1A;struct Cyc_Absyn_Kind
_tmpC1B;enum Cyc_Absyn_KindQual _tmpC1C;struct Cyc_Absyn_Kind*_tmpC1E;struct Cyc_Absyn_Kind
_tmpC1F;enum Cyc_Absyn_KindQual _tmpC20;struct Cyc_Core_Opt*_tmpC22;struct Cyc_Core_Opt**
_tmpC23;struct Cyc_Absyn_Kind*_tmpC24;struct Cyc_Absyn_Kind _tmpC25;enum Cyc_Absyn_KindQual
_tmpC26;struct Cyc_Absyn_Kind*_tmpC28;struct Cyc_Absyn_Kind _tmpC29;enum Cyc_Absyn_KindQual
_tmpC2A;struct Cyc_Core_Opt*_tmpC2C;struct Cyc_Core_Opt**_tmpC2D;_LL608:{struct Cyc_Absyn_Less_kb_struct*
_tmpC13=(struct Cyc_Absyn_Less_kb_struct*)_tmpC12;if(_tmpC13->tag != 2)goto _LL60A;
else{_tmpC14=_tmpC13->f1;_tmpC15=(struct Cyc_Core_Opt**)& _tmpC13->f1;_tmpC16=
_tmpC13->f2;}}if(!(_tmpC16->kind == Cyc_Absyn_RgnKind))goto _LL60A;_LL609: if(
_tmpC16->aliasqual == Cyc_Absyn_Top){*_tmpC15=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpC18=_tmpC16;goto _LL60B;}*_tmpC15=Cyc_Tcutil_kind_to_bound_opt(_tmpC16);
_tmpC18=_tmpC16;goto _LL60B;_LL60A:{struct Cyc_Absyn_Eq_kb_struct*_tmpC17=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC12;if(_tmpC17->tag != 0)goto _LL60C;else{_tmpC18=
_tmpC17->f1;}}if(!(_tmpC18->kind == Cyc_Absyn_RgnKind))goto _LL60C;_LL60B:{struct
Cyc_Absyn_AccessEff_struct*_tmp142D;struct Cyc_Absyn_VarType_struct*_tmp142C;
struct Cyc_Absyn_VarType_struct _tmp142B;struct Cyc_Absyn_AccessEff_struct _tmp142A;
struct Cyc_List_List*_tmp1429;effect=((_tmp1429=_cycalloc(sizeof(*_tmp1429)),((
_tmp1429->hd=(void*)((_tmp142D=_cycalloc(sizeof(*_tmp142D)),((_tmp142D[0]=((
_tmp142A.tag=23,((_tmp142A.f1=(void*)((void*)((_tmp142C=_cycalloc(sizeof(*
_tmp142C)),((_tmp142C[0]=((_tmp142B.tag=2,((_tmp142B.f1=_tmpC10,_tmp142B)))),
_tmp142C))))),_tmp142A)))),_tmp142D)))),((_tmp1429->tl=effect,_tmp1429))))));}
goto _LL607;_LL60C: {struct Cyc_Absyn_Less_kb_struct*_tmpC19=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC12;if(_tmpC19->tag != 2)goto _LL60E;else{_tmpC1A=_tmpC19->f2;_tmpC1B=*_tmpC1A;
_tmpC1C=_tmpC1B.kind;if(_tmpC1C != Cyc_Absyn_IntKind)goto _LL60E;}}_LL60D: goto
_LL60F;_LL60E: {struct Cyc_Absyn_Eq_kb_struct*_tmpC1D=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC12;if(_tmpC1D->tag != 0)goto _LL610;else{_tmpC1E=_tmpC1D->f1;_tmpC1F=*_tmpC1E;
_tmpC20=_tmpC1F.kind;if(_tmpC20 != Cyc_Absyn_IntKind)goto _LL610;}}_LL60F: goto
_LL607;_LL610: {struct Cyc_Absyn_Less_kb_struct*_tmpC21=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC12;if(_tmpC21->tag != 2)goto _LL612;else{_tmpC22=_tmpC21->f1;_tmpC23=(struct
Cyc_Core_Opt**)& _tmpC21->f1;_tmpC24=_tmpC21->f2;_tmpC25=*_tmpC24;_tmpC26=_tmpC25.kind;
if(_tmpC26 != Cyc_Absyn_EffKind)goto _LL612;}}_LL611:*_tmpC23=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_ek);goto _LL613;_LL612: {struct Cyc_Absyn_Eq_kb_struct*_tmpC27=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC12;if(_tmpC27->tag != 0)goto _LL614;else{_tmpC28=
_tmpC27->f1;_tmpC29=*_tmpC28;_tmpC2A=_tmpC29.kind;if(_tmpC2A != Cyc_Absyn_EffKind)
goto _LL614;}}_LL613:{struct Cyc_Absyn_VarType_struct*_tmp1433;struct Cyc_Absyn_VarType_struct
_tmp1432;struct Cyc_List_List*_tmp1431;effect=((_tmp1431=_cycalloc(sizeof(*
_tmp1431)),((_tmp1431->hd=(void*)((_tmp1433=_cycalloc(sizeof(*_tmp1433)),((
_tmp1433[0]=((_tmp1432.tag=2,((_tmp1432.f1=_tmpC10,_tmp1432)))),_tmp1433)))),((
_tmp1431->tl=effect,_tmp1431))))));}goto _LL607;_LL614: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpC2B=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC12;if(_tmpC2B->tag != 1)goto
_LL616;else{_tmpC2C=_tmpC2B->f1;_tmpC2D=(struct Cyc_Core_Opt**)& _tmpC2B->f1;}}
_LL615:{struct Cyc_Absyn_Less_kb_struct*_tmp1439;struct Cyc_Absyn_Less_kb_struct
_tmp1438;struct Cyc_Core_Opt*_tmp1437;*_tmpC2D=((_tmp1437=_cycalloc(sizeof(*
_tmp1437)),((_tmp1437->v=(void*)((_tmp1439=_cycalloc(sizeof(*_tmp1439)),((
_tmp1439[0]=((_tmp1438.tag=2,((_tmp1438.f1=0,((_tmp1438.f2=& Cyc_Tcutil_ak,
_tmp1438)))))),_tmp1439)))),_tmp1437))));}goto _LL617;_LL616:;_LL617:{struct Cyc_Absyn_RgnsEff_struct*
_tmp1448;struct Cyc_Absyn_VarType_struct*_tmp1447;struct Cyc_Absyn_VarType_struct
_tmp1446;struct Cyc_Absyn_RgnsEff_struct _tmp1445;struct Cyc_List_List*_tmp1444;
effect=((_tmp1444=_cycalloc(sizeof(*_tmp1444)),((_tmp1444->hd=(void*)((_tmp1448=
_cycalloc(sizeof(*_tmp1448)),((_tmp1448[0]=((_tmp1445.tag=25,((_tmp1445.f1=(void*)((
void*)((_tmp1447=_cycalloc(sizeof(*_tmp1447)),((_tmp1447[0]=((_tmp1446.tag=2,((
_tmp1446.f1=_tmpC10,_tmp1446)))),_tmp1447))))),_tmp1445)))),_tmp1448)))),((
_tmp1444->tl=effect,_tmp1444))))));}goto _LL607;_LL607:;};}}{struct Cyc_List_List*
ts=_tmpBC6.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpC3F;int _tmpC40;struct
_tuple28 _tmpC3E=*((struct _tuple28*)ts->hd);_tmpC3F=_tmpC3E.f1;_tmpC40=_tmpC3E.f2;
if(!_tmpC40)continue;{struct Cyc_Absyn_Kind*_tmpC41=Cyc_Tcutil_typ_kind(_tmpC3F);
struct Cyc_Absyn_Kind _tmpC42;enum Cyc_Absyn_KindQual _tmpC43;struct Cyc_Absyn_Kind
_tmpC44;enum Cyc_Absyn_KindQual _tmpC45;struct Cyc_Absyn_Kind _tmpC46;enum Cyc_Absyn_KindQual
_tmpC47;_LL619: _tmpC42=*_tmpC41;_tmpC43=_tmpC42.kind;if(_tmpC43 != Cyc_Absyn_RgnKind)
goto _LL61B;_LL61A:{struct Cyc_Absyn_AccessEff_struct*_tmp144E;struct Cyc_Absyn_AccessEff_struct
_tmp144D;struct Cyc_List_List*_tmp144C;effect=((_tmp144C=_cycalloc(sizeof(*
_tmp144C)),((_tmp144C->hd=(void*)((_tmp144E=_cycalloc(sizeof(*_tmp144E)),((
_tmp144E[0]=((_tmp144D.tag=23,((_tmp144D.f1=(void*)_tmpC3F,_tmp144D)))),_tmp144E)))),((
_tmp144C->tl=effect,_tmp144C))))));}goto _LL618;_LL61B: _tmpC44=*_tmpC41;_tmpC45=
_tmpC44.kind;if(_tmpC45 != Cyc_Absyn_EffKind)goto _LL61D;_LL61C:{struct Cyc_List_List*
_tmp144F;effect=((_tmp144F=_cycalloc(sizeof(*_tmp144F)),((_tmp144F->hd=_tmpC3F,((
_tmp144F->tl=effect,_tmp144F))))));}goto _LL618;_LL61D: _tmpC46=*_tmpC41;_tmpC47=
_tmpC46.kind;if(_tmpC47 != Cyc_Absyn_IntKind)goto _LL61F;_LL61E: goto _LL618;_LL61F:;
_LL620:{struct Cyc_Absyn_RgnsEff_struct*_tmp1455;struct Cyc_Absyn_RgnsEff_struct
_tmp1454;struct Cyc_List_List*_tmp1453;effect=((_tmp1453=_cycalloc(sizeof(*
_tmp1453)),((_tmp1453->hd=(void*)((_tmp1455=_cycalloc(sizeof(*_tmp1455)),((
_tmp1455[0]=((_tmp1454.tag=25,((_tmp1454.f1=(void*)_tmpC3F,_tmp1454)))),_tmp1455)))),((
_tmp1453->tl=effect,_tmp1453))))));}goto _LL618;_LL618:;};}}{struct Cyc_Absyn_JoinEff_struct*
_tmp145B;struct Cyc_Absyn_JoinEff_struct _tmp145A;struct Cyc_Core_Opt*_tmp1459;*
_tmpAD3=((_tmp1459=_cycalloc(sizeof(*_tmp1459)),((_tmp1459->v=(void*)((_tmp145B=
_cycalloc(sizeof(*_tmp145B)),((_tmp145B[0]=((_tmp145A.tag=24,((_tmp145A.f1=
effect,_tmp145A)))),_tmp145B)))),_tmp1459))));};}if(*_tmpAD1 != 0){struct Cyc_List_List*
bs=*_tmpAD1;for(0;bs != 0;bs=bs->tl){void*_tmpC52=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpC54;struct Cyc_Core_Opt**
_tmpC55;struct Cyc_Core_Opt*_tmpC57;struct Cyc_Core_Opt**_tmpC58;struct Cyc_Absyn_Kind*
_tmpC59;struct Cyc_Absyn_Kind _tmpC5A;enum Cyc_Absyn_KindQual _tmpC5B;enum Cyc_Absyn_AliasQual
_tmpC5C;struct Cyc_Core_Opt*_tmpC5E;struct Cyc_Core_Opt**_tmpC5F;struct Cyc_Absyn_Kind*
_tmpC60;struct Cyc_Absyn_Kind _tmpC61;enum Cyc_Absyn_KindQual _tmpC62;enum Cyc_Absyn_AliasQual
_tmpC63;struct Cyc_Core_Opt*_tmpC65;struct Cyc_Core_Opt**_tmpC66;struct Cyc_Absyn_Kind*
_tmpC67;struct Cyc_Absyn_Kind _tmpC68;enum Cyc_Absyn_KindQual _tmpC69;enum Cyc_Absyn_AliasQual
_tmpC6A;struct Cyc_Core_Opt*_tmpC6C;struct Cyc_Core_Opt**_tmpC6D;struct Cyc_Absyn_Kind*
_tmpC6E;struct Cyc_Absyn_Kind _tmpC6F;enum Cyc_Absyn_KindQual _tmpC70;enum Cyc_Absyn_AliasQual
_tmpC71;struct Cyc_Core_Opt*_tmpC73;struct Cyc_Core_Opt**_tmpC74;struct Cyc_Absyn_Kind*
_tmpC75;struct Cyc_Absyn_Kind _tmpC76;enum Cyc_Absyn_KindQual _tmpC77;enum Cyc_Absyn_AliasQual
_tmpC78;struct Cyc_Core_Opt*_tmpC7A;struct Cyc_Core_Opt**_tmpC7B;struct Cyc_Absyn_Kind*
_tmpC7C;struct Cyc_Absyn_Kind _tmpC7D;enum Cyc_Absyn_KindQual _tmpC7E;enum Cyc_Absyn_AliasQual
_tmpC7F;struct Cyc_Core_Opt*_tmpC81;struct Cyc_Core_Opt**_tmpC82;struct Cyc_Absyn_Kind*
_tmpC83;struct Cyc_Absyn_Kind _tmpC84;enum Cyc_Absyn_KindQual _tmpC85;enum Cyc_Absyn_AliasQual
_tmpC86;struct Cyc_Core_Opt*_tmpC88;struct Cyc_Core_Opt**_tmpC89;struct Cyc_Absyn_Kind*
_tmpC8A;struct Cyc_Absyn_Kind _tmpC8B;enum Cyc_Absyn_KindQual _tmpC8C;enum Cyc_Absyn_AliasQual
_tmpC8D;struct Cyc_Core_Opt*_tmpC8F;struct Cyc_Core_Opt**_tmpC90;struct Cyc_Absyn_Kind*
_tmpC91;struct Cyc_Absyn_Kind*_tmpC93;struct Cyc_Absyn_Kind _tmpC94;enum Cyc_Absyn_KindQual
_tmpC95;_LL622: {struct Cyc_Absyn_Unknown_kb_struct*_tmpC53=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC52;if(_tmpC53->tag != 1)goto _LL624;else{_tmpC54=_tmpC53->f1;_tmpC55=(struct
Cyc_Core_Opt**)& _tmpC53->f1;}}_LL623:{const char*_tmp145F;void*_tmp145E[1];struct
Cyc_String_pa_struct _tmp145D;(_tmp145D.tag=0,((_tmp145D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp145E[0]=&
_tmp145D,Cyc_Tcutil_warn(loc,((_tmp145F="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp145F,sizeof(char),47))),_tag_dyneither(_tmp145E,sizeof(void*),
1)))))));}_tmpC58=_tmpC55;goto _LL625;_LL624: {struct Cyc_Absyn_Less_kb_struct*
_tmpC56=(struct Cyc_Absyn_Less_kb_struct*)_tmpC52;if(_tmpC56->tag != 2)goto _LL626;
else{_tmpC57=_tmpC56->f1;_tmpC58=(struct Cyc_Core_Opt**)& _tmpC56->f1;_tmpC59=
_tmpC56->f2;_tmpC5A=*_tmpC59;_tmpC5B=_tmpC5A.kind;if(_tmpC5B != Cyc_Absyn_BoxKind)
goto _LL626;_tmpC5C=_tmpC5A.aliasqual;if(_tmpC5C != Cyc_Absyn_Top)goto _LL626;}}
_LL625: _tmpC5F=_tmpC58;goto _LL627;_LL626: {struct Cyc_Absyn_Less_kb_struct*
_tmpC5D=(struct Cyc_Absyn_Less_kb_struct*)_tmpC52;if(_tmpC5D->tag != 2)goto _LL628;
else{_tmpC5E=_tmpC5D->f1;_tmpC5F=(struct Cyc_Core_Opt**)& _tmpC5D->f1;_tmpC60=
_tmpC5D->f2;_tmpC61=*_tmpC60;_tmpC62=_tmpC61.kind;if(_tmpC62 != Cyc_Absyn_MemKind)
goto _LL628;_tmpC63=_tmpC61.aliasqual;if(_tmpC63 != Cyc_Absyn_Top)goto _LL628;}}
_LL627: _tmpC66=_tmpC5F;goto _LL629;_LL628: {struct Cyc_Absyn_Less_kb_struct*
_tmpC64=(struct Cyc_Absyn_Less_kb_struct*)_tmpC52;if(_tmpC64->tag != 2)goto _LL62A;
else{_tmpC65=_tmpC64->f1;_tmpC66=(struct Cyc_Core_Opt**)& _tmpC64->f1;_tmpC67=
_tmpC64->f2;_tmpC68=*_tmpC67;_tmpC69=_tmpC68.kind;if(_tmpC69 != Cyc_Absyn_MemKind)
goto _LL62A;_tmpC6A=_tmpC68.aliasqual;if(_tmpC6A != Cyc_Absyn_Aliasable)goto _LL62A;}}
_LL629: _tmpC6D=_tmpC66;goto _LL62B;_LL62A: {struct Cyc_Absyn_Less_kb_struct*
_tmpC6B=(struct Cyc_Absyn_Less_kb_struct*)_tmpC52;if(_tmpC6B->tag != 2)goto _LL62C;
else{_tmpC6C=_tmpC6B->f1;_tmpC6D=(struct Cyc_Core_Opt**)& _tmpC6B->f1;_tmpC6E=
_tmpC6B->f2;_tmpC6F=*_tmpC6E;_tmpC70=_tmpC6F.kind;if(_tmpC70 != Cyc_Absyn_AnyKind)
goto _LL62C;_tmpC71=_tmpC6F.aliasqual;if(_tmpC71 != Cyc_Absyn_Top)goto _LL62C;}}
_LL62B: _tmpC74=_tmpC6D;goto _LL62D;_LL62C: {struct Cyc_Absyn_Less_kb_struct*
_tmpC72=(struct Cyc_Absyn_Less_kb_struct*)_tmpC52;if(_tmpC72->tag != 2)goto _LL62E;
else{_tmpC73=_tmpC72->f1;_tmpC74=(struct Cyc_Core_Opt**)& _tmpC72->f1;_tmpC75=
_tmpC72->f2;_tmpC76=*_tmpC75;_tmpC77=_tmpC76.kind;if(_tmpC77 != Cyc_Absyn_AnyKind)
goto _LL62E;_tmpC78=_tmpC76.aliasqual;if(_tmpC78 != Cyc_Absyn_Aliasable)goto _LL62E;}}
_LL62D:*_tmpC74=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL621;_LL62E: {
struct Cyc_Absyn_Less_kb_struct*_tmpC79=(struct Cyc_Absyn_Less_kb_struct*)_tmpC52;
if(_tmpC79->tag != 2)goto _LL630;else{_tmpC7A=_tmpC79->f1;_tmpC7B=(struct Cyc_Core_Opt**)&
_tmpC79->f1;_tmpC7C=_tmpC79->f2;_tmpC7D=*_tmpC7C;_tmpC7E=_tmpC7D.kind;if(_tmpC7E
!= Cyc_Absyn_MemKind)goto _LL630;_tmpC7F=_tmpC7D.aliasqual;if(_tmpC7F != Cyc_Absyn_Unique)
goto _LL630;}}_LL62F: _tmpC82=_tmpC7B;goto _LL631;_LL630: {struct Cyc_Absyn_Less_kb_struct*
_tmpC80=(struct Cyc_Absyn_Less_kb_struct*)_tmpC52;if(_tmpC80->tag != 2)goto _LL632;
else{_tmpC81=_tmpC80->f1;_tmpC82=(struct Cyc_Core_Opt**)& _tmpC80->f1;_tmpC83=
_tmpC80->f2;_tmpC84=*_tmpC83;_tmpC85=_tmpC84.kind;if(_tmpC85 != Cyc_Absyn_AnyKind)
goto _LL632;_tmpC86=_tmpC84.aliasqual;if(_tmpC86 != Cyc_Absyn_Unique)goto _LL632;}}
_LL631:*_tmpC82=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL621;_LL632: {
struct Cyc_Absyn_Less_kb_struct*_tmpC87=(struct Cyc_Absyn_Less_kb_struct*)_tmpC52;
if(_tmpC87->tag != 2)goto _LL634;else{_tmpC88=_tmpC87->f1;_tmpC89=(struct Cyc_Core_Opt**)&
_tmpC87->f1;_tmpC8A=_tmpC87->f2;_tmpC8B=*_tmpC8A;_tmpC8C=_tmpC8B.kind;if(_tmpC8C
!= Cyc_Absyn_RgnKind)goto _LL634;_tmpC8D=_tmpC8B.aliasqual;if(_tmpC8D != Cyc_Absyn_Top)
goto _LL634;}}_LL633:*_tmpC89=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto
_LL621;_LL634: {struct Cyc_Absyn_Less_kb_struct*_tmpC8E=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC52;if(_tmpC8E->tag != 2)goto _LL636;else{_tmpC8F=_tmpC8E->f1;_tmpC90=(struct
Cyc_Core_Opt**)& _tmpC8E->f1;_tmpC91=_tmpC8E->f2;}}_LL635:*_tmpC90=Cyc_Tcutil_kind_to_bound_opt(
_tmpC91);goto _LL621;_LL636: {struct Cyc_Absyn_Eq_kb_struct*_tmpC92=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC52;if(_tmpC92->tag != 0)goto _LL638;else{_tmpC93=_tmpC92->f1;_tmpC94=*_tmpC93;
_tmpC95=_tmpC94.kind;if(_tmpC95 != Cyc_Absyn_MemKind)goto _LL638;}}_LL637:{const
char*_tmp1462;void*_tmp1461;(_tmp1461=0,Cyc_Tcutil_terr(loc,((_tmp1462="functions can't abstract M types",
_tag_dyneither(_tmp1462,sizeof(char),33))),_tag_dyneither(_tmp1461,sizeof(void*),
0)));}goto _LL621;_LL638:;_LL639: goto _LL621;_LL621:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpBC6.kind_env,*_tmpAD1);_tmpBC6.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpBC6.r,_tmpBC6.free_vars,*_tmpAD1);{struct Cyc_List_List*tvs=_tmpBC6.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpC9C;int _tmpC9D;struct
_tuple27 _tmpC9B=*((struct _tuple27*)tvs->hd);_tmpC9C=_tmpC9B.f1;_tmpC9D=_tmpC9B.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpC9C,_tmpC9D);}}{struct Cyc_List_List*evs=_tmpBC6.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpC9F;int _tmpCA0;struct _tuple28 _tmpC9E=*((struct _tuple28*)evs->hd);
_tmpC9F=_tmpC9E.f1;_tmpCA0=_tmpC9E.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpC9F,_tmpCA0);}};}goto _LL54F;};}_LL56E: {struct Cyc_Absyn_TupleType_struct*
_tmpADA=(struct Cyc_Absyn_TupleType_struct*)_tmpA9A;if(_tmpADA->tag != 11)goto
_LL570;else{_tmpADB=_tmpADA->f1;}}_LL56F: for(0;_tmpADB != 0;_tmpADB=_tmpADB->tl){
struct _tuple11*_tmpCA2=(struct _tuple11*)_tmpADB->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpCA2).f2,1);((*_tmpCA2).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpCA2).f1).print_const,(*_tmpCA2).f2);}goto _LL54F;_LL570: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpADC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpA9A;if(_tmpADC->tag != 13)goto
_LL572;else{_tmpADD=_tmpADC->f1;_tmpADE=_tmpADC->f2;}}_LL571: {struct
_RegionHandle*_tmpCA3=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmpADE != 0;_tmpADE=_tmpADE->tl){struct Cyc_Absyn_Aggrfield _tmpCA5;struct
_dyneither_ptr*_tmpCA6;struct Cyc_Absyn_Tqual _tmpCA7;struct Cyc_Absyn_Tqual*
_tmpCA8;void*_tmpCA9;struct Cyc_Absyn_Exp*_tmpCAA;struct Cyc_List_List*_tmpCAB;
struct Cyc_Absyn_Aggrfield*_tmpCA4=(struct Cyc_Absyn_Aggrfield*)_tmpADE->hd;
_tmpCA5=*_tmpCA4;_tmpCA6=_tmpCA5.name;_tmpCA7=_tmpCA5.tq;_tmpCA8=(struct Cyc_Absyn_Tqual*)&(*
_tmpCA4).tq;_tmpCA9=_tmpCA5.type;_tmpCAA=_tmpCA5.width;_tmpCAB=_tmpCA5.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpCA6)){
const char*_tmp1466;void*_tmp1465[1];struct Cyc_String_pa_struct _tmp1464;(_tmp1464.tag=
0,((_tmp1464.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCA6),((
_tmp1465[0]=& _tmp1464,Cyc_Tcutil_terr(loc,((_tmp1466="duplicate field %s",
_tag_dyneither(_tmp1466,sizeof(char),19))),_tag_dyneither(_tmp1465,sizeof(void*),
1)))))));}{const char*_tmp1467;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpCA6,((
_tmp1467="",_tag_dyneither(_tmp1467,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp1468;prev_fields=((_tmp1468=_region_malloc(_tmpCA3,sizeof(*_tmp1468)),((
_tmp1468->hd=_tmpCA6,((_tmp1468->tl=prev_fields,_tmp1468))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpCA9,1);_tmpCA8->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpCA8->print_const,_tmpCA9);if(_tmpADD == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpCA9)){const char*_tmp146C;void*_tmp146B[1];struct Cyc_String_pa_struct _tmp146A;(
_tmp146A.tag=0,((_tmp146A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpCA6),((_tmp146B[0]=& _tmp146A,Cyc_Tcutil_warn(loc,((_tmp146C="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp146C,sizeof(char),74))),_tag_dyneither(_tmp146B,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpCA9,_tmpCAA,_tmpCA6);Cyc_Tcutil_check_field_atts(
loc,_tmpCA6,_tmpCAB);}}goto _LL54F;}_LL572: {struct Cyc_Absyn_AggrType_struct*
_tmpADF=(struct Cyc_Absyn_AggrType_struct*)_tmpA9A;if(_tmpADF->tag != 12)goto
_LL574;else{_tmpAE0=_tmpADF->f1;_tmpAE1=_tmpAE0.aggr_info;_tmpAE2=(union Cyc_Absyn_AggrInfoU*)&(
_tmpADF->f1).aggr_info;_tmpAE3=_tmpAE0.targs;_tmpAE4=(struct Cyc_List_List**)&(
_tmpADF->f1).targs;}}_LL573:{union Cyc_Absyn_AggrInfoU _tmpCB4=*_tmpAE2;struct
_tuple4 _tmpCB5;enum Cyc_Absyn_AggrKind _tmpCB6;struct _tuple2*_tmpCB7;struct Cyc_Core_Opt*
_tmpCB8;struct Cyc_Absyn_Aggrdecl**_tmpCB9;struct Cyc_Absyn_Aggrdecl*_tmpCBA;
_LL63B: if((_tmpCB4.UnknownAggr).tag != 1)goto _LL63D;_tmpCB5=(struct _tuple4)(
_tmpCB4.UnknownAggr).val;_tmpCB6=_tmpCB5.f1;_tmpCB7=_tmpCB5.f2;_tmpCB8=_tmpCB5.f3;
_LL63C: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpCBB;_push_handler(&
_tmpCBB);{int _tmpCBD=0;if(setjmp(_tmpCBB.handler))_tmpCBD=1;if(!_tmpCBD){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpCB7);{struct Cyc_Absyn_Aggrdecl*_tmpCBE=*adp;if(_tmpCBE->kind != 
_tmpCB6){if(_tmpCBE->kind == Cyc_Absyn_StructA){const char*_tmp1471;void*_tmp1470[
2];struct Cyc_String_pa_struct _tmp146F;struct Cyc_String_pa_struct _tmp146E;(
_tmp146E.tag=0,((_tmp146E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCB7)),((_tmp146F.tag=0,((_tmp146F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpCB7)),((_tmp1470[0]=& _tmp146F,((
_tmp1470[1]=& _tmp146E,Cyc_Tcutil_terr(loc,((_tmp1471="expecting struct %s instead of union %s",
_tag_dyneither(_tmp1471,sizeof(char),40))),_tag_dyneither(_tmp1470,sizeof(void*),
2)))))))))))));}else{const char*_tmp1476;void*_tmp1475[2];struct Cyc_String_pa_struct
_tmp1474;struct Cyc_String_pa_struct _tmp1473;(_tmp1473.tag=0,((_tmp1473.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpCB7)),((
_tmp1474.tag=0,((_tmp1474.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCB7)),((_tmp1475[0]=& _tmp1474,((_tmp1475[1]=& _tmp1473,Cyc_Tcutil_terr(loc,((
_tmp1476="expecting union %s instead of struct %s",_tag_dyneither(_tmp1476,
sizeof(char),40))),_tag_dyneither(_tmp1475,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpCB8  && (int)_tmpCB8->v){if(!((unsigned int)_tmpCBE->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCBE->impl))->tagged){const char*
_tmp147A;void*_tmp1479[1];struct Cyc_String_pa_struct _tmp1478;(_tmp1478.tag=0,((
_tmp1478.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCB7)),((_tmp1479[0]=& _tmp1478,Cyc_Tcutil_terr(loc,((_tmp147A="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp147A,sizeof(char),42))),_tag_dyneither(_tmp1479,sizeof(void*),
1)))))));}}*_tmpAE2=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpCBC=(void*)_exn_thrown;void*_tmpCCB=_tmpCBC;_LL640: {struct Cyc_Dict_Absent_struct*
_tmpCCC=(struct Cyc_Dict_Absent_struct*)_tmpCCB;if(_tmpCCC->tag != Cyc_Dict_Absent)
goto _LL642;}_LL641: {struct Cyc_Tcenv_Genv*_tmpCCD=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp147B;struct Cyc_Absyn_Aggrdecl*_tmpCCE=(_tmp147B=
_cycalloc(sizeof(*_tmp147B)),((_tmp147B->kind=_tmpCB6,((_tmp147B->sc=Cyc_Absyn_Extern,((
_tmp147B->name=_tmpCB7,((_tmp147B->tvs=0,((_tmp147B->impl=0,((_tmp147B->attributes=
0,_tmp147B)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpCCD,loc,_tmpCCE);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpCB7);*_tmpAE2=Cyc_Absyn_KnownAggr(adp);if(*_tmpAE4 != 0){{const char*
_tmp147F;void*_tmp147E[1];struct Cyc_String_pa_struct _tmp147D;(_tmp147D.tag=0,((
_tmp147D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCB7)),((_tmp147E[0]=& _tmp147D,Cyc_Tcutil_terr(loc,((_tmp147F="declare parameterized type %s before using",
_tag_dyneither(_tmp147F,sizeof(char),43))),_tag_dyneither(_tmp147E,sizeof(void*),
1)))))));}return cvtenv;}goto _LL63F;}_LL642:;_LL643:(void)_throw(_tmpCCB);_LL63F:;}};}
_tmpCBA=*adp;goto _LL63E;}_LL63D: if((_tmpCB4.KnownAggr).tag != 2)goto _LL63A;
_tmpCB9=(struct Cyc_Absyn_Aggrdecl**)(_tmpCB4.KnownAggr).val;_tmpCBA=*_tmpCB9;
_LL63E: {struct Cyc_List_List*tvs=_tmpCBA->tvs;struct Cyc_List_List*ts=*_tmpAE4;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpCD3=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpCD4=(void*)ts->hd;{struct _tuple0 _tmp1480;
struct _tuple0 _tmpCD6=(_tmp1480.f1=Cyc_Absyn_compress_kb(_tmpCD3->kind),((
_tmp1480.f2=_tmpCD4,_tmp1480)));void*_tmpCD7;void*_tmpCD9;struct Cyc_Absyn_Tvar*
_tmpCDB;_LL645: _tmpCD7=_tmpCD6.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpCD8=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpCD7;if(_tmpCD8->tag != 1)goto _LL647;};
_tmpCD9=_tmpCD6.f2;{struct Cyc_Absyn_VarType_struct*_tmpCDA=(struct Cyc_Absyn_VarType_struct*)
_tmpCD9;if(_tmpCDA->tag != 2)goto _LL647;else{_tmpCDB=_tmpCDA->f1;}};_LL646: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpCDB);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpCDB,1);continue;_LL647:;_LL648: goto _LL644;_LL644:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);};}if(ts != 
0){const char*_tmp1484;void*_tmp1483[1];struct Cyc_String_pa_struct _tmp1482;(
_tmp1482.tag=0,((_tmp1482.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCBA->name)),((_tmp1483[0]=& _tmp1482,Cyc_Tcutil_terr(loc,((_tmp1484="too many parameters for type %s",
_tag_dyneither(_tmp1484,sizeof(char),32))),_tag_dyneither(_tmp1483,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp1485;hidden_ts=((_tmp1485=_cycalloc(sizeof(*_tmp1485)),((_tmp1485->hd=e,((
_tmp1485->tl=hidden_ts,_tmp1485))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k,e,1);}*_tmpAE4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpAE4,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL63A:;}goto _LL54F;
_LL574: {struct Cyc_Absyn_TypedefType_struct*_tmpAE5=(struct Cyc_Absyn_TypedefType_struct*)
_tmpA9A;if(_tmpAE5->tag != 18)goto _LL576;else{_tmpAE6=_tmpAE5->f1;_tmpAE7=_tmpAE5->f2;
_tmpAE8=(struct Cyc_List_List**)& _tmpAE5->f2;_tmpAE9=_tmpAE5->f3;_tmpAEA=(struct
Cyc_Absyn_Typedefdecl**)& _tmpAE5->f3;_tmpAEB=_tmpAE5->f4;_tmpAEC=(void***)&
_tmpAE5->f4;}}_LL575: {struct Cyc_List_List*targs=*_tmpAE8;struct Cyc_Absyn_Typedefdecl*
td;{struct _handler_cons _tmpCE0;_push_handler(& _tmpCE0);{int _tmpCE2=0;if(setjmp(
_tmpCE0.handler))_tmpCE2=1;if(!_tmpCE2){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmpAE6);;_pop_handler();}else{void*_tmpCE1=(void*)_exn_thrown;void*_tmpCE4=
_tmpCE1;_LL64A: {struct Cyc_Dict_Absent_struct*_tmpCE5=(struct Cyc_Dict_Absent_struct*)
_tmpCE4;if(_tmpCE5->tag != Cyc_Dict_Absent)goto _LL64C;}_LL64B:{const char*_tmp1489;
void*_tmp1488[1];struct Cyc_String_pa_struct _tmp1487;(_tmp1487.tag=0,((_tmp1487.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAE6)),((
_tmp1488[0]=& _tmp1487,Cyc_Tcutil_terr(loc,((_tmp1489="unbound typedef name %s",
_tag_dyneither(_tmp1489,sizeof(char),24))),_tag_dyneither(_tmp1488,sizeof(void*),
1)))))));}return cvtenv;_LL64C:;_LL64D:(void)_throw(_tmpCE4);_LL649:;}};}*_tmpAEA=(
struct Cyc_Absyn_Typedefdecl*)td;_tmpAE6[0]=(td->name)[0];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct _RegionHandle*_tmpCE9=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_tbk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);{struct _tuple15*_tmp148C;struct Cyc_List_List*
_tmp148B;inst=((_tmp148B=_region_malloc(_tmpCE9,sizeof(*_tmp148B)),((_tmp148B->hd=((
_tmp148C=_region_malloc(_tmpCE9,sizeof(*_tmp148C)),((_tmp148C->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp148C->f2=(void*)ts->hd,_tmp148C)))))),((_tmp148B->tl=inst,_tmp148B))))));};}
if(ts != 0){const char*_tmp1490;void*_tmp148F[1];struct Cyc_String_pa_struct
_tmp148E;(_tmp148E.tag=0,((_tmp148E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAE6)),((_tmp148F[0]=& _tmp148E,Cyc_Tcutil_terr(
loc,((_tmp1490="too many parameters for typedef %s",_tag_dyneither(_tmp1490,
sizeof(char),35))),_tag_dyneither(_tmp148F,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp1491;hidden_ts=((_tmp1491=_cycalloc(sizeof(*_tmp1491)),((
_tmp1491->hd=e,((_tmp1491->tl=hidden_ts,_tmp1491))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple15*_tmp1494;struct Cyc_List_List*_tmp1493;inst=(
struct Cyc_List_List*)((_tmp1493=_cycalloc(sizeof(*_tmp1493)),((_tmp1493->hd=(
struct _tuple15*)((_tmp1494=_cycalloc(sizeof(*_tmp1494)),((_tmp1494->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp1494->f2=e,_tmp1494)))))),((_tmp1493->tl=inst,
_tmp1493))))));};}*_tmpAE8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(_tmpCE9,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp1495;*_tmpAEC=((_tmp1495=_cycalloc(sizeof(*
_tmp1495)),((_tmp1495[0]=new_typ,_tmp1495))));}}goto _LL54F;};}_LL576: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpAED=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpA9A;if(_tmpAED->tag != 22)goto
_LL578;}_LL577: goto _LL579;_LL578: {struct Cyc_Absyn_HeapRgn_struct*_tmpAEE=(
struct Cyc_Absyn_HeapRgn_struct*)_tmpA9A;if(_tmpAEE->tag != 21)goto _LL57A;}_LL579:
goto _LL54F;_LL57A: {struct Cyc_Absyn_RgnHandleType_struct*_tmpAEF=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmpA9A;if(_tmpAEF->tag != 16)goto _LL57C;else{_tmpAF0=(void*)_tmpAEF->f1;}}_LL57B:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpAF0,1);
goto _LL54F;_LL57C: {struct Cyc_Absyn_DynRgnType_struct*_tmpAF1=(struct Cyc_Absyn_DynRgnType_struct*)
_tmpA9A;if(_tmpAF1->tag != 17)goto _LL57E;else{_tmpAF2=(void*)_tmpAF1->f1;_tmpAF3=(
void*)_tmpAF1->f2;}}_LL57D: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_rk,_tmpAF2,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,
_tmpAF3,1);goto _LL54F;_LL57E: {struct Cyc_Absyn_AccessEff_struct*_tmpAF4=(struct
Cyc_Absyn_AccessEff_struct*)_tmpA9A;if(_tmpAF4->tag != 23)goto _LL580;else{_tmpAF5=(
void*)_tmpAF4->f1;}}_LL57F: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_trk,_tmpAF5,1);goto _LL54F;_LL580: {struct Cyc_Absyn_RgnsEff_struct*
_tmpAF6=(struct Cyc_Absyn_RgnsEff_struct*)_tmpA9A;if(_tmpAF6->tag != 25)goto _LL582;
else{_tmpAF7=(void*)_tmpAF6->f1;}}_LL581: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpAF7,1);goto _LL54F;_LL582: {struct Cyc_Absyn_JoinEff_struct*
_tmpAF8=(struct Cyc_Absyn_JoinEff_struct*)_tmpA9A;if(_tmpAF8->tag != 24)goto _LL54F;
else{_tmpAF9=_tmpAF8->f1;}}_LL583: for(0;_tmpAF9 != 0;_tmpAF9=_tmpAF9->tl){cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpAF9->hd,1);}
goto _LL54F;_LL54F:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){{
void*_tmpCF3=t;struct Cyc_Core_Opt*_tmpCF5;struct Cyc_Core_Opt*_tmpCF6;_LL64F: {
struct Cyc_Absyn_Evar_struct*_tmpCF4=(struct Cyc_Absyn_Evar_struct*)_tmpCF3;if(
_tmpCF4->tag != 1)goto _LL651;else{_tmpCF5=_tmpCF4->f1;_tmpCF6=_tmpCF4->f2;}}
_LL650: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*_tmpCF7=_tmpCF5;struct Cyc_Core_Opt
_tmpCF8;struct Cyc_Absyn_Kind*_tmpCF9;_LL654: if(_tmpCF7 != 0)goto _LL656;_LL655:{
const char*_tmp1496;s=((_tmp1496="kind=NULL ",_tag_dyneither(_tmp1496,sizeof(char),
11)));}goto _LL653;_LL656: if(_tmpCF7 == 0)goto _LL653;_tmpCF8=*_tmpCF7;_tmpCF9=(
struct Cyc_Absyn_Kind*)_tmpCF8.v;_LL657:{const char*_tmp149A;void*_tmp1499[1];
struct Cyc_String_pa_struct _tmp1498;s=(struct _dyneither_ptr)((_tmp1498.tag=0,((
_tmp1498.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmpCF9)),((_tmp1499[0]=& _tmp1498,Cyc_aprintf(((_tmp149A="kind=%s ",
_tag_dyneither(_tmp149A,sizeof(char),9))),_tag_dyneither(_tmp1499,sizeof(void*),
1))))))));}goto _LL653;_LL653:;}{struct Cyc_Core_Opt*_tmpCFE=_tmpCF6;struct Cyc_Core_Opt
_tmpCFF;void*_tmpD00;_LL659: if(_tmpCFE != 0)goto _LL65B;_LL65A:{const char*_tmp149E;
void*_tmp149D[1];struct Cyc_String_pa_struct _tmp149C;s=(struct _dyneither_ptr)((
_tmp149C.tag=0,((_tmp149C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp149D[0]=& _tmp149C,Cyc_aprintf(((_tmp149E="%s ref=NULL",_tag_dyneither(
_tmp149E,sizeof(char),12))),_tag_dyneither(_tmp149D,sizeof(void*),1))))))));}
goto _LL658;_LL65B: if(_tmpCFE == 0)goto _LL658;_tmpCFF=*_tmpCFE;_tmpD00=(void*)
_tmpCFF.v;_LL65C:{const char*_tmp14A3;void*_tmp14A2[2];struct Cyc_String_pa_struct
_tmp14A1;struct Cyc_String_pa_struct _tmp14A0;s=(struct _dyneither_ptr)((_tmp14A0.tag=
0,((_tmp14A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmpD00)),((_tmp14A1.tag=0,((_tmp14A1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp14A2[0]=& _tmp14A1,((_tmp14A2[1]=& _tmp14A0,Cyc_aprintf(((
_tmp14A3="%s ref=%s",_tag_dyneither(_tmp14A3,sizeof(char),10))),_tag_dyneither(
_tmp14A2,sizeof(void*),2))))))))))))));}goto _LL658;_LL658:;}{const char*_tmp14A7;
void*_tmp14A6[1];struct Cyc_String_pa_struct _tmp14A5;(_tmp14A5.tag=0,((_tmp14A5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp14A6[0]=& _tmp14A5,Cyc_fprintf(
Cyc_stderr,((_tmp14A7="evar info: %s\n",_tag_dyneither(_tmp14A7,sizeof(char),15))),
_tag_dyneither(_tmp14A6,sizeof(void*),1)))))));}goto _LL64E;}_LL651:;_LL652: goto
_LL64E;_LL64E:;}{const char*_tmp14AD;void*_tmp14AC[3];struct Cyc_String_pa_struct
_tmp14AB;struct Cyc_String_pa_struct _tmp14AA;struct Cyc_String_pa_struct _tmp14A9;(
_tmp14A9.tag=0,((_tmp14A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp14AA.tag=0,((_tmp14AA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp14AB.tag=0,((
_tmp14AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp14AC[0]=& _tmp14AB,((_tmp14AC[1]=& _tmp14AA,((_tmp14AC[2]=& _tmp14A9,Cyc_Tcutil_terr(
loc,((_tmp14AD="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp14AD,sizeof(char),51))),_tag_dyneither(_tmp14AC,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{void*_tmpD10=e->r;
struct Cyc_List_List*_tmpD16;struct Cyc_Absyn_Exp*_tmpD18;struct Cyc_Absyn_Exp*
_tmpD19;struct Cyc_Absyn_Exp*_tmpD1A;struct Cyc_Absyn_Exp*_tmpD1C;struct Cyc_Absyn_Exp*
_tmpD1D;struct Cyc_Absyn_Exp*_tmpD1F;struct Cyc_Absyn_Exp*_tmpD20;struct Cyc_Absyn_Exp*
_tmpD22;struct Cyc_Absyn_Exp*_tmpD23;void*_tmpD25;struct Cyc_Absyn_Exp*_tmpD26;
void*_tmpD28;void*_tmpD2A;void*_tmpD2C;struct Cyc_Absyn_Exp*_tmpD2E;_LL65E: {
struct Cyc_Absyn_Const_e_struct*_tmpD11=(struct Cyc_Absyn_Const_e_struct*)_tmpD10;
if(_tmpD11->tag != 0)goto _LL660;}_LL65F: goto _LL661;_LL660: {struct Cyc_Absyn_Enum_e_struct*
_tmpD12=(struct Cyc_Absyn_Enum_e_struct*)_tmpD10;if(_tmpD12->tag != 32)goto _LL662;}
_LL661: goto _LL663;_LL662: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpD13=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpD10;if(_tmpD13->tag != 33)goto _LL664;}_LL663: goto _LL665;_LL664: {struct Cyc_Absyn_Var_e_struct*
_tmpD14=(struct Cyc_Absyn_Var_e_struct*)_tmpD10;if(_tmpD14->tag != 1)goto _LL666;}
_LL665: goto _LL65D;_LL666: {struct Cyc_Absyn_Primop_e_struct*_tmpD15=(struct Cyc_Absyn_Primop_e_struct*)
_tmpD10;if(_tmpD15->tag != 3)goto _LL668;else{_tmpD16=_tmpD15->f2;}}_LL667: for(0;
_tmpD16 != 0;_tmpD16=_tmpD16->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpD16->hd,te,cvtenv);}goto _LL65D;_LL668: {struct Cyc_Absyn_Conditional_e_struct*
_tmpD17=(struct Cyc_Absyn_Conditional_e_struct*)_tmpD10;if(_tmpD17->tag != 6)goto
_LL66A;else{_tmpD18=_tmpD17->f1;_tmpD19=_tmpD17->f2;_tmpD1A=_tmpD17->f3;}}_LL669:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD18,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpD19,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD1A,te,
cvtenv);goto _LL65D;_LL66A: {struct Cyc_Absyn_And_e_struct*_tmpD1B=(struct Cyc_Absyn_And_e_struct*)
_tmpD10;if(_tmpD1B->tag != 7)goto _LL66C;else{_tmpD1C=_tmpD1B->f1;_tmpD1D=_tmpD1B->f2;}}
_LL66B: _tmpD1F=_tmpD1C;_tmpD20=_tmpD1D;goto _LL66D;_LL66C: {struct Cyc_Absyn_Or_e_struct*
_tmpD1E=(struct Cyc_Absyn_Or_e_struct*)_tmpD10;if(_tmpD1E->tag != 8)goto _LL66E;
else{_tmpD1F=_tmpD1E->f1;_tmpD20=_tmpD1E->f2;}}_LL66D: _tmpD22=_tmpD1F;_tmpD23=
_tmpD20;goto _LL66F;_LL66E: {struct Cyc_Absyn_SeqExp_e_struct*_tmpD21=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpD10;if(_tmpD21->tag != 9)goto _LL670;else{_tmpD22=_tmpD21->f1;_tmpD23=_tmpD21->f2;}}
_LL66F: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD22,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD23,te,cvtenv);goto _LL65D;_LL670: {
struct Cyc_Absyn_Cast_e_struct*_tmpD24=(struct Cyc_Absyn_Cast_e_struct*)_tmpD10;
if(_tmpD24->tag != 15)goto _LL672;else{_tmpD25=(void*)_tmpD24->f1;_tmpD26=_tmpD24->f2;}}
_LL671: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD26,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpD25,1);goto
_LL65D;_LL672: {struct Cyc_Absyn_Offsetof_e_struct*_tmpD27=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpD10;if(_tmpD27->tag != 20)goto _LL674;else{_tmpD28=(void*)_tmpD27->f1;}}_LL673:
_tmpD2A=_tmpD28;goto _LL675;_LL674: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpD29=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpD10;if(_tmpD29->tag != 18)goto _LL676;
else{_tmpD2A=(void*)_tmpD29->f1;}}_LL675: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_tak,_tmpD2A,1);goto _LL65D;_LL676: {struct Cyc_Absyn_Valueof_e_struct*
_tmpD2B=(struct Cyc_Absyn_Valueof_e_struct*)_tmpD10;if(_tmpD2B->tag != 39)goto
_LL678;else{_tmpD2C=(void*)_tmpD2B->f1;}}_LL677: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpD2C,1);goto _LL65D;_LL678: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpD2D=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpD10;if(_tmpD2D->tag != 19)goto
_LL67A;else{_tmpD2E=_tmpD2D->f1;}}_LL679: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpD2E,te,cvtenv);goto _LL65D;_LL67A:;_LL67B: {const char*_tmp14B0;void*_tmp14AF;(
_tmp14AF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp14B0="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp14B0,sizeof(char),66))),_tag_dyneither(_tmp14AF,sizeof(void*),
0)));}_LL65D:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){struct Cyc_List_List*_tmpD31=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpD33;struct
_tuple27 _tmpD32=*((struct _tuple27*)vs->hd);_tmpD33=_tmpD32.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpD31,_tmpD33);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpD35;struct _tuple28 _tmpD34=*((struct _tuple28*)evs->hd);_tmpD35=_tmpD34.f1;{
void*_tmpD36=Cyc_Tcutil_compress(_tmpD35);struct Cyc_Core_Opt*_tmpD38;struct Cyc_Core_Opt**
_tmpD39;_LL67D: {struct Cyc_Absyn_Evar_struct*_tmpD37=(struct Cyc_Absyn_Evar_struct*)
_tmpD36;if(_tmpD37->tag != 1)goto _LL67F;else{_tmpD38=_tmpD37->f4;_tmpD39=(struct
Cyc_Core_Opt**)& _tmpD37->f4;}}_LL67E: if(*_tmpD39 == 0){struct Cyc_Core_Opt*
_tmp14B1;*_tmpD39=((_tmp14B1=_cycalloc(sizeof(*_tmp14B1)),((_tmp14B1->v=_tmpD31,
_tmp14B1))));}else{struct Cyc_List_List*_tmpD3B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpD39))->v;struct Cyc_List_List*_tmpD3C=0;for(0;_tmpD3B != 0;
_tmpD3B=_tmpD3B->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpD31,(struct Cyc_Absyn_Tvar*)_tmpD3B->hd)){struct Cyc_List_List*_tmp14B2;
_tmpD3C=((_tmp14B2=_cycalloc(sizeof(*_tmp14B2)),((_tmp14B2->hd=(struct Cyc_Absyn_Tvar*)
_tmpD3B->hd,((_tmp14B2->tl=_tmpD3C,_tmp14B2))))));}}{struct Cyc_Core_Opt*_tmp14B3;*
_tmpD39=((_tmp14B3=_cycalloc(sizeof(*_tmp14B3)),((_tmp14B3->v=_tmpD3C,_tmp14B3))));};}
goto _LL67C;_LL67F:;_LL680: goto _LL67C;_LL67C:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpD3F=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpD40=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp14B4;struct Cyc_Tcutil_CVTEnv _tmpD41=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp14B4.r=_tmpD40,((_tmp14B4.kind_env=_tmpD3F,((_tmp14B4.free_vars=0,((_tmp14B4.free_evars=
0,((_tmp14B4.generalize_evars=generalize_evars,((_tmp14B4.fn_result=0,_tmp14B4)))))))))))),&
Cyc_Tcutil_tmk,t);struct Cyc_List_List*_tmpD42=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple27*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpD40,(struct Cyc_Absyn_Tvar*(*)(struct _tuple27*))Cyc_Core_fst,
_tmpD41.free_vars);struct Cyc_List_List*_tmpD43=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpD40,(void*(*)(struct _tuple28*))Cyc_Core_fst,_tmpD41.free_evars);if(_tmpD3F != 
0){struct Cyc_List_List*_tmpD44=0;{struct Cyc_List_List*_tmpD45=_tmpD42;for(0;(
unsigned int)_tmpD45;_tmpD45=_tmpD45->tl){struct Cyc_Absyn_Tvar*_tmpD46=(struct
Cyc_Absyn_Tvar*)_tmpD45->hd;int found=0;{struct Cyc_List_List*_tmpD47=_tmpD3F;for(
0;(unsigned int)_tmpD47;_tmpD47=_tmpD47->tl){if(Cyc_Absyn_tvar_cmp(_tmpD46,(
struct Cyc_Absyn_Tvar*)_tmpD47->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp14B5;_tmpD44=((_tmp14B5=_region_malloc(_tmpD40,sizeof(*_tmp14B5)),((_tmp14B5->hd=(
struct Cyc_Absyn_Tvar*)_tmpD45->hd,((_tmp14B5->tl=_tmpD44,_tmp14B5))))));}}}
_tmpD42=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpD44);}{struct Cyc_List_List*x=_tmpD42;for(0;x != 0;x=x->tl){void*_tmpD49=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpD4B;struct Cyc_Core_Opt**
_tmpD4C;struct Cyc_Core_Opt*_tmpD4E;struct Cyc_Core_Opt**_tmpD4F;struct Cyc_Absyn_Kind*
_tmpD50;struct Cyc_Absyn_Kind _tmpD51;enum Cyc_Absyn_KindQual _tmpD52;enum Cyc_Absyn_AliasQual
_tmpD53;struct Cyc_Core_Opt*_tmpD55;struct Cyc_Core_Opt**_tmpD56;struct Cyc_Absyn_Kind*
_tmpD57;struct Cyc_Absyn_Kind _tmpD58;enum Cyc_Absyn_KindQual _tmpD59;enum Cyc_Absyn_AliasQual
_tmpD5A;struct Cyc_Core_Opt*_tmpD5C;struct Cyc_Core_Opt**_tmpD5D;struct Cyc_Absyn_Kind*
_tmpD5E;struct Cyc_Absyn_Kind _tmpD5F;enum Cyc_Absyn_KindQual _tmpD60;enum Cyc_Absyn_AliasQual
_tmpD61;struct Cyc_Core_Opt*_tmpD63;struct Cyc_Core_Opt**_tmpD64;struct Cyc_Absyn_Kind*
_tmpD65;struct Cyc_Absyn_Kind _tmpD66;enum Cyc_Absyn_KindQual _tmpD67;enum Cyc_Absyn_AliasQual
_tmpD68;struct Cyc_Core_Opt*_tmpD6A;struct Cyc_Core_Opt**_tmpD6B;struct Cyc_Absyn_Kind*
_tmpD6C;struct Cyc_Absyn_Kind _tmpD6D;enum Cyc_Absyn_KindQual _tmpD6E;enum Cyc_Absyn_AliasQual
_tmpD6F;struct Cyc_Core_Opt*_tmpD71;struct Cyc_Core_Opt**_tmpD72;struct Cyc_Absyn_Kind*
_tmpD73;struct Cyc_Absyn_Kind*_tmpD75;struct Cyc_Absyn_Kind _tmpD76;enum Cyc_Absyn_KindQual
_tmpD77;enum Cyc_Absyn_AliasQual _tmpD78;_LL682: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpD4A=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpD49;if(_tmpD4A->tag != 1)goto
_LL684;else{_tmpD4B=_tmpD4A->f1;_tmpD4C=(struct Cyc_Core_Opt**)& _tmpD4A->f1;}}
_LL683: _tmpD4F=_tmpD4C;goto _LL685;_LL684: {struct Cyc_Absyn_Less_kb_struct*
_tmpD4D=(struct Cyc_Absyn_Less_kb_struct*)_tmpD49;if(_tmpD4D->tag != 2)goto _LL686;
else{_tmpD4E=_tmpD4D->f1;_tmpD4F=(struct Cyc_Core_Opt**)& _tmpD4D->f1;_tmpD50=
_tmpD4D->f2;_tmpD51=*_tmpD50;_tmpD52=_tmpD51.kind;if(_tmpD52 != Cyc_Absyn_BoxKind)
goto _LL686;_tmpD53=_tmpD51.aliasqual;if(_tmpD53 != Cyc_Absyn_Top)goto _LL686;}}
_LL685: _tmpD56=_tmpD4F;goto _LL687;_LL686: {struct Cyc_Absyn_Less_kb_struct*
_tmpD54=(struct Cyc_Absyn_Less_kb_struct*)_tmpD49;if(_tmpD54->tag != 2)goto _LL688;
else{_tmpD55=_tmpD54->f1;_tmpD56=(struct Cyc_Core_Opt**)& _tmpD54->f1;_tmpD57=
_tmpD54->f2;_tmpD58=*_tmpD57;_tmpD59=_tmpD58.kind;if(_tmpD59 != Cyc_Absyn_MemKind)
goto _LL688;_tmpD5A=_tmpD58.aliasqual;if(_tmpD5A != Cyc_Absyn_Top)goto _LL688;}}
_LL687: _tmpD5D=_tmpD56;goto _LL689;_LL688: {struct Cyc_Absyn_Less_kb_struct*
_tmpD5B=(struct Cyc_Absyn_Less_kb_struct*)_tmpD49;if(_tmpD5B->tag != 2)goto _LL68A;
else{_tmpD5C=_tmpD5B->f1;_tmpD5D=(struct Cyc_Core_Opt**)& _tmpD5B->f1;_tmpD5E=
_tmpD5B->f2;_tmpD5F=*_tmpD5E;_tmpD60=_tmpD5F.kind;if(_tmpD60 != Cyc_Absyn_MemKind)
goto _LL68A;_tmpD61=_tmpD5F.aliasqual;if(_tmpD61 != Cyc_Absyn_Aliasable)goto _LL68A;}}
_LL689:*_tmpD5D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL681;_LL68A: {
struct Cyc_Absyn_Less_kb_struct*_tmpD62=(struct Cyc_Absyn_Less_kb_struct*)_tmpD49;
if(_tmpD62->tag != 2)goto _LL68C;else{_tmpD63=_tmpD62->f1;_tmpD64=(struct Cyc_Core_Opt**)&
_tmpD62->f1;_tmpD65=_tmpD62->f2;_tmpD66=*_tmpD65;_tmpD67=_tmpD66.kind;if(_tmpD67
!= Cyc_Absyn_MemKind)goto _LL68C;_tmpD68=_tmpD66.aliasqual;if(_tmpD68 != Cyc_Absyn_Unique)
goto _LL68C;}}_LL68B:*_tmpD64=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto
_LL681;_LL68C: {struct Cyc_Absyn_Less_kb_struct*_tmpD69=(struct Cyc_Absyn_Less_kb_struct*)
_tmpD49;if(_tmpD69->tag != 2)goto _LL68E;else{_tmpD6A=_tmpD69->f1;_tmpD6B=(struct
Cyc_Core_Opt**)& _tmpD69->f1;_tmpD6C=_tmpD69->f2;_tmpD6D=*_tmpD6C;_tmpD6E=_tmpD6D.kind;
if(_tmpD6E != Cyc_Absyn_RgnKind)goto _LL68E;_tmpD6F=_tmpD6D.aliasqual;if(_tmpD6F != 
Cyc_Absyn_Top)goto _LL68E;}}_LL68D:*_tmpD6B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
goto _LL681;_LL68E: {struct Cyc_Absyn_Less_kb_struct*_tmpD70=(struct Cyc_Absyn_Less_kb_struct*)
_tmpD49;if(_tmpD70->tag != 2)goto _LL690;else{_tmpD71=_tmpD70->f1;_tmpD72=(struct
Cyc_Core_Opt**)& _tmpD70->f1;_tmpD73=_tmpD70->f2;}}_LL68F:*_tmpD72=Cyc_Tcutil_kind_to_bound_opt(
_tmpD73);goto _LL681;_LL690: {struct Cyc_Absyn_Eq_kb_struct*_tmpD74=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpD49;if(_tmpD74->tag != 0)goto _LL692;else{_tmpD75=_tmpD74->f1;_tmpD76=*_tmpD75;
_tmpD77=_tmpD76.kind;if(_tmpD77 != Cyc_Absyn_MemKind)goto _LL692;_tmpD78=_tmpD76.aliasqual;}}
_LL691:{const char*_tmp14BC;void*_tmp14BB[2];struct Cyc_String_pa_struct _tmp14BA;
struct Cyc_Absyn_Kind*_tmp14B9;struct Cyc_String_pa_struct _tmp14B8;(_tmp14B8.tag=0,((
_tmp14B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp14B9=_cycalloc_atomic(sizeof(*_tmp14B9)),((_tmp14B9->kind=Cyc_Absyn_MemKind,((
_tmp14B9->aliasqual=_tmpD78,_tmp14B9)))))))),((_tmp14BA.tag=0,((_tmp14BA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmp14BB[0]=& _tmp14BA,((_tmp14BB[1]=& _tmp14B8,Cyc_Tcutil_terr(loc,((
_tmp14BC="type variable %s cannot have kind %s",_tag_dyneither(_tmp14BC,sizeof(
char),37))),_tag_dyneither(_tmp14BB,sizeof(void*),2)))))))))))));}goto _LL681;
_LL692:;_LL693: goto _LL681;_LL681:;}}if(_tmpD42 != 0  || _tmpD43 != 0){{void*_tmpD7E=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpD80;struct Cyc_List_List*_tmpD81;
struct Cyc_List_List**_tmpD82;struct Cyc_Core_Opt*_tmpD83;void*_tmpD84;struct Cyc_List_List*
_tmpD85;int _tmpD86;struct Cyc_Absyn_VarargInfo*_tmpD87;struct Cyc_List_List*
_tmpD88;struct Cyc_List_List*_tmpD89;_LL695: {struct Cyc_Absyn_FnType_struct*
_tmpD7F=(struct Cyc_Absyn_FnType_struct*)_tmpD7E;if(_tmpD7F->tag != 10)goto _LL697;
else{_tmpD80=_tmpD7F->f1;_tmpD81=_tmpD80.tvars;_tmpD82=(struct Cyc_List_List**)&(
_tmpD7F->f1).tvars;_tmpD83=_tmpD80.effect;_tmpD84=_tmpD80.ret_typ;_tmpD85=
_tmpD80.args;_tmpD86=_tmpD80.c_varargs;_tmpD87=_tmpD80.cyc_varargs;_tmpD88=
_tmpD80.rgn_po;_tmpD89=_tmpD80.attributes;}}_LL696: if(*_tmpD82 == 0){*_tmpD82=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpD42);_tmpD42=0;}
goto _LL694;_LL697:;_LL698: goto _LL694;_LL694:;}if(_tmpD42 != 0){const char*_tmp14C0;
void*_tmp14BF[1];struct Cyc_String_pa_struct _tmp14BE;(_tmp14BE.tag=0,((_tmp14BE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpD42->hd)->name),((
_tmp14BF[0]=& _tmp14BE,Cyc_Tcutil_terr(loc,((_tmp14C0="unbound type variable %s",
_tag_dyneither(_tmp14C0,sizeof(char),25))),_tag_dyneither(_tmp14BF,sizeof(void*),
1)))))));}if(_tmpD43 != 0)for(0;_tmpD43 != 0;_tmpD43=_tmpD43->tl){void*e=(void*)
_tmpD43->hd;struct Cyc_Absyn_Kind*_tmpD8D=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpD8E;enum Cyc_Absyn_KindQual _tmpD8F;enum Cyc_Absyn_AliasQual _tmpD90;struct Cyc_Absyn_Kind
_tmpD91;enum Cyc_Absyn_KindQual _tmpD92;enum Cyc_Absyn_AliasQual _tmpD93;struct Cyc_Absyn_Kind
_tmpD94;enum Cyc_Absyn_KindQual _tmpD95;enum Cyc_Absyn_AliasQual _tmpD96;struct Cyc_Absyn_Kind
_tmpD97;enum Cyc_Absyn_KindQual _tmpD98;_LL69A: _tmpD8E=*_tmpD8D;_tmpD8F=_tmpD8E.kind;
if(_tmpD8F != Cyc_Absyn_RgnKind)goto _LL69C;_tmpD90=_tmpD8E.aliasqual;if(_tmpD90 != 
Cyc_Absyn_Unique)goto _LL69C;_LL69B: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp14C3;void*_tmp14C2;(_tmp14C2=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14C3="can't unify evar with unique region!",
_tag_dyneither(_tmp14C3,sizeof(char),37))),_tag_dyneither(_tmp14C2,sizeof(void*),
0)));}goto _LL699;_LL69C: _tmpD91=*_tmpD8D;_tmpD92=_tmpD91.kind;if(_tmpD92 != Cyc_Absyn_RgnKind)
goto _LL69E;_tmpD93=_tmpD91.aliasqual;if(_tmpD93 != Cyc_Absyn_Aliasable)goto _LL69E;
_LL69D: goto _LL69F;_LL69E: _tmpD94=*_tmpD8D;_tmpD95=_tmpD94.kind;if(_tmpD95 != Cyc_Absyn_RgnKind)
goto _LL6A0;_tmpD96=_tmpD94.aliasqual;if(_tmpD96 != Cyc_Absyn_Top)goto _LL6A0;
_LL69F: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp14C6;
void*_tmp14C5;(_tmp14C5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp14C6="can't unify evar with heap!",_tag_dyneither(
_tmp14C6,sizeof(char),28))),_tag_dyneither(_tmp14C5,sizeof(void*),0)));}goto
_LL699;_LL6A0: _tmpD97=*_tmpD8D;_tmpD98=_tmpD97.kind;if(_tmpD98 != Cyc_Absyn_EffKind)
goto _LL6A2;_LL6A1: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp14C9;void*_tmp14C8;(_tmp14C8=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14C9="can't unify evar with {}!",
_tag_dyneither(_tmp14C9,sizeof(char),26))),_tag_dyneither(_tmp14C8,sizeof(void*),
0)));}goto _LL699;_LL6A2:;_LL6A3:{const char*_tmp14CE;void*_tmp14CD[2];struct Cyc_String_pa_struct
_tmp14CC;struct Cyc_String_pa_struct _tmp14CB;(_tmp14CB.tag=0,((_tmp14CB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp14CC.tag=0,((_tmp14CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp14CD[0]=& _tmp14CC,((_tmp14CD[1]=& _tmp14CB,Cyc_Tcutil_terr(loc,((
_tmp14CE="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp14CE,sizeof(char),52))),_tag_dyneither(_tmp14CD,sizeof(void*),2)))))))))))));}
goto _LL699;_LL699:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmpDA4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpDA6;struct Cyc_List_List*
_tmpDA7;struct Cyc_Core_Opt*_tmpDA8;void*_tmpDA9;_LL6A5: {struct Cyc_Absyn_FnType_struct*
_tmpDA5=(struct Cyc_Absyn_FnType_struct*)_tmpDA4;if(_tmpDA5->tag != 10)goto _LL6A7;
else{_tmpDA6=_tmpDA5->f1;_tmpDA7=_tmpDA6.tvars;_tmpDA8=_tmpDA6.effect;_tmpDA9=
_tmpDA6.ret_typ;}}_LL6A6: fd->tvs=_tmpDA7;fd->effect=_tmpDA8;goto _LL6A4;_LL6A7:;
_LL6A8: {const char*_tmp14D1;void*_tmp14D0;(_tmp14D0=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14D1="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp14D1,sizeof(char),38))),_tag_dyneither(_tmp14D0,sizeof(void*),
0)));}_LL6A4:;}{struct _RegionHandle*_tmpDAC=Cyc_Tcenv_get_fnrgn(te);{const char*
_tmp14D2;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple23*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpDAC,(struct _dyneither_ptr*(*)(struct _tuple23*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmp14D2="function declaration has repeated parameter",
_tag_dyneither(_tmp14D2,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp14D3;fd->cached_typ=((
_tmp14D3=_cycalloc(sizeof(*_tmp14D3)),((_tmp14D3->v=t,_tmp14D3))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpDAF=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp14D4;
struct Cyc_Tcutil_CVTEnv _tmpDB0=Cyc_Tcutil_check_valid_type(loc,te,((_tmp14D4.r=
_tmpDAF,((_tmp14D4.kind_env=bound_tvars,((_tmp14D4.free_vars=0,((_tmp14D4.free_evars=
0,((_tmp14D4.generalize_evars=0,((_tmp14D4.fn_result=0,_tmp14D4)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpDB1=Cyc_Tcutil_remove_bound_tvars(
_tmpDAF,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple27*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpDAF,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple27*))Cyc_Core_fst,_tmpDB0.free_vars),bound_tvars);struct Cyc_List_List*
_tmpDB2=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple28*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpDAF,(void*(*)(struct _tuple28*))Cyc_Core_fst,
_tmpDB0.free_evars);{struct Cyc_List_List*fs=_tmpDB1;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpDB3=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp14D9;void*_tmp14D8[2];struct Cyc_String_pa_struct _tmp14D7;struct Cyc_String_pa_struct
_tmp14D6;(_tmp14D6.tag=0,((_tmp14D6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp14D7.tag=0,((_tmp14D7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpDB3),((_tmp14D8[0]=& _tmp14D7,((
_tmp14D8[1]=& _tmp14D6,Cyc_Tcutil_terr(loc,((_tmp14D9="unbound type variable %s in type %s",
_tag_dyneither(_tmp14D9,sizeof(char),36))),_tag_dyneither(_tmp14D8,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpDB2 != 0)for(0;_tmpDB2 != 0;_tmpDB2=
_tmpDB2->tl){void*e=(void*)_tmpDB2->hd;struct Cyc_Absyn_Kind*_tmpDB8=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpDB9;enum Cyc_Absyn_KindQual _tmpDBA;enum Cyc_Absyn_AliasQual
_tmpDBB;struct Cyc_Absyn_Kind _tmpDBC;enum Cyc_Absyn_KindQual _tmpDBD;enum Cyc_Absyn_AliasQual
_tmpDBE;struct Cyc_Absyn_Kind _tmpDBF;enum Cyc_Absyn_KindQual _tmpDC0;enum Cyc_Absyn_AliasQual
_tmpDC1;struct Cyc_Absyn_Kind _tmpDC2;enum Cyc_Absyn_KindQual _tmpDC3;_LL6AA:
_tmpDB9=*_tmpDB8;_tmpDBA=_tmpDB9.kind;if(_tmpDBA != Cyc_Absyn_RgnKind)goto _LL6AC;
_tmpDBB=_tmpDB9.aliasqual;if(_tmpDBB != Cyc_Absyn_Unique)goto _LL6AC;_LL6AB: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp14DC;void*
_tmp14DB;(_tmp14DB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp14DC="can't unify evar with unique region!",
_tag_dyneither(_tmp14DC,sizeof(char),37))),_tag_dyneither(_tmp14DB,sizeof(void*),
0)));}goto _LL6A9;_LL6AC: _tmpDBC=*_tmpDB8;_tmpDBD=_tmpDBC.kind;if(_tmpDBD != Cyc_Absyn_RgnKind)
goto _LL6AE;_tmpDBE=_tmpDBC.aliasqual;if(_tmpDBE != Cyc_Absyn_Aliasable)goto _LL6AE;
_LL6AD: goto _LL6AF;_LL6AE: _tmpDBF=*_tmpDB8;_tmpDC0=_tmpDBF.kind;if(_tmpDC0 != Cyc_Absyn_RgnKind)
goto _LL6B0;_tmpDC1=_tmpDBF.aliasqual;if(_tmpDC1 != Cyc_Absyn_Top)goto _LL6B0;
_LL6AF: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp14DF;
void*_tmp14DE;(_tmp14DE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp14DF="can't unify evar with heap!",_tag_dyneither(
_tmp14DF,sizeof(char),28))),_tag_dyneither(_tmp14DE,sizeof(void*),0)));}goto
_LL6A9;_LL6B0: _tmpDC2=*_tmpDB8;_tmpDC3=_tmpDC2.kind;if(_tmpDC3 != Cyc_Absyn_EffKind)
goto _LL6B2;_LL6B1: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp14E2;void*_tmp14E1;(_tmp14E1=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14E2="can't unify evar with {}!",
_tag_dyneither(_tmp14E2,sizeof(char),26))),_tag_dyneither(_tmp14E1,sizeof(void*),
0)));}goto _LL6A9;_LL6B2:;_LL6B3:{const char*_tmp14E7;void*_tmp14E6[2];struct Cyc_String_pa_struct
_tmp14E5;struct Cyc_String_pa_struct _tmp14E4;(_tmp14E4.tag=0,((_tmp14E4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp14E5.tag=0,((_tmp14E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp14E6[0]=& _tmp14E5,((_tmp14E6[1]=& _tmp14E4,Cyc_Tcutil_terr(loc,((
_tmp14E7="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp14E7,sizeof(char),52))),_tag_dyneither(_tmp14E6,sizeof(void*),2)))))))))))));}
goto _LL6A9;_LL6A9:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv);
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(struct
Cyc_List_List*tvs);void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg);static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp14EC;void*_tmp14EB[2];struct Cyc_String_pa_struct
_tmp14EA;struct Cyc_String_pa_struct _tmp14E9;(_tmp14E9.tag=0,((_tmp14E9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp14EA.tag=
0,((_tmp14EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp14EB[0]=&
_tmp14EA,((_tmp14EB[1]=& _tmp14E9,Cyc_Tcutil_terr(loc,((_tmp14EC="%s: %s",
_tag_dyneither(_tmp14EC,sizeof(char),7))),_tag_dyneither(_tmp14EB,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s);static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg);void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((
void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct
_dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,
vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(struct
Cyc_Position_Segment*loc,struct Cyc_List_List*tvs);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){const char*_tmp14ED;((
void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,((_tmp14ED="duplicate type variable",
_tag_dyneither(_tmp14ED,sizeof(char),24))));}struct _tuple30{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple31{struct Cyc_List_List*f1;void*f2;};struct _tuple32{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields);struct Cyc_List_List*
Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*
sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*sd_fields=sdfields;
for(0;sd_fields != 0;sd_fields=sd_fields->tl){const char*_tmp14EE;if(Cyc_strcmp((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((
_tmp14EE="",_tag_dyneither(_tmp14EE,sizeof(char),1))))!= 0){struct _tuple30*
_tmp14F1;struct Cyc_List_List*_tmp14F0;fields=((_tmp14F0=_cycalloc(sizeof(*
_tmp14F0)),((_tmp14F0->hd=((_tmp14F1=_cycalloc(sizeof(*_tmp14F1)),((_tmp14F1->f1=(
struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp14F1->f2=0,_tmp14F1)))))),((
_tmp14F0->tl=fields,_tmp14F0))))));}}}fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);{const char*_tmp14F3;const char*_tmp14F2;struct
_dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp14F3="struct",
_tag_dyneither(_tmp14F3,sizeof(char),7)):((_tmp14F2="union",_tag_dyneither(
_tmp14F2,sizeof(char),6)));struct Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){
struct _tuple31 _tmpDD8;struct Cyc_List_List*_tmpDD9;void*_tmpDDA;struct _tuple31*
_tmpDD7=(struct _tuple31*)des->hd;_tmpDD8=*_tmpDD7;_tmpDD9=_tmpDD8.f1;_tmpDDA=
_tmpDD8.f2;if(_tmpDD9 == 0){struct Cyc_List_List*_tmpDDB=fields;for(0;_tmpDDB != 0;
_tmpDDB=_tmpDDB->tl){if(!(*((struct _tuple30*)_tmpDDB->hd)).f2){(*((struct
_tuple30*)_tmpDDB->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*_tmp14F9;struct
Cyc_Absyn_FieldName_struct _tmp14F8;struct Cyc_List_List*_tmp14F7;(*((struct
_tuple31*)des->hd)).f1=(struct Cyc_List_List*)((_tmp14F7=_cycalloc(sizeof(*
_tmp14F7)),((_tmp14F7->hd=(void*)((_tmp14F9=_cycalloc(sizeof(*_tmp14F9)),((
_tmp14F9[0]=((_tmp14F8.tag=1,((_tmp14F8.f1=((*((struct _tuple30*)_tmpDDB->hd)).f1)->name,
_tmp14F8)))),_tmp14F9)))),((_tmp14F7->tl=0,_tmp14F7))))));}{struct _tuple32*
_tmp14FC;struct Cyc_List_List*_tmp14FB;ans=((_tmp14FB=_region_malloc(rgn,sizeof(*
_tmp14FB)),((_tmp14FB->hd=((_tmp14FC=_region_malloc(rgn,sizeof(*_tmp14FC)),((
_tmp14FC->f1=(*((struct _tuple30*)_tmpDDB->hd)).f1,((_tmp14FC->f2=_tmpDDA,
_tmp14FC)))))),((_tmp14FB->tl=ans,_tmp14FB))))));}break;}}if(_tmpDDB == 0){const
char*_tmp1500;void*_tmp14FF[1];struct Cyc_String_pa_struct _tmp14FE;(_tmp14FE.tag=
0,((_tmp14FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp14FF[0]=& _tmp14FE,Cyc_Tcutil_terr(loc,((_tmp1500="too many arguments to %s",
_tag_dyneither(_tmp1500,sizeof(char),25))),_tag_dyneither(_tmp14FF,sizeof(void*),
1)))))));}}else{if(_tmpDD9->tl != 0){const char*_tmp1503;void*_tmp1502;(_tmp1502=0,
Cyc_Tcutil_terr(loc,((_tmp1503="multiple designators are not yet supported",
_tag_dyneither(_tmp1503,sizeof(char),43))),_tag_dyneither(_tmp1502,sizeof(void*),
0)));}else{void*_tmpDE6=(void*)_tmpDD9->hd;struct _dyneither_ptr*_tmpDE9;_LL6B5: {
struct Cyc_Absyn_ArrayElement_struct*_tmpDE7=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpDE6;if(_tmpDE7->tag != 0)goto _LL6B7;}_LL6B6:{const char*_tmp1507;void*_tmp1506[
1];struct Cyc_String_pa_struct _tmp1505;(_tmp1505.tag=0,((_tmp1505.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1506[0]=& _tmp1505,Cyc_Tcutil_terr(
loc,((_tmp1507="array designator used in argument to %s",_tag_dyneither(_tmp1507,
sizeof(char),40))),_tag_dyneither(_tmp1506,sizeof(void*),1)))))));}goto _LL6B4;
_LL6B7: {struct Cyc_Absyn_FieldName_struct*_tmpDE8=(struct Cyc_Absyn_FieldName_struct*)
_tmpDE6;if(_tmpDE8->tag != 1)goto _LL6B4;else{_tmpDE9=_tmpDE8->f1;}}_LL6B8: {
struct Cyc_List_List*_tmpDED=fields;for(0;_tmpDED != 0;_tmpDED=_tmpDED->tl){if(Cyc_strptrcmp(
_tmpDE9,((*((struct _tuple30*)_tmpDED->hd)).f1)->name)== 0){if((*((struct _tuple30*)
_tmpDED->hd)).f2){const char*_tmp150B;void*_tmp150A[1];struct Cyc_String_pa_struct
_tmp1509;(_tmp1509.tag=0,((_tmp1509.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpDE9),((_tmp150A[0]=& _tmp1509,Cyc_Tcutil_terr(loc,((_tmp150B="member %s has already been used as an argument",
_tag_dyneither(_tmp150B,sizeof(char),47))),_tag_dyneither(_tmp150A,sizeof(void*),
1)))))));}(*((struct _tuple30*)_tmpDED->hd)).f2=1;{struct _tuple32*_tmp150E;struct
Cyc_List_List*_tmp150D;ans=((_tmp150D=_region_malloc(rgn,sizeof(*_tmp150D)),((
_tmp150D->hd=((_tmp150E=_region_malloc(rgn,sizeof(*_tmp150E)),((_tmp150E->f1=(*((
struct _tuple30*)_tmpDED->hd)).f1,((_tmp150E->f2=_tmpDDA,_tmp150E)))))),((
_tmp150D->tl=ans,_tmp150D))))));}break;}}if(_tmpDED == 0){const char*_tmp1512;void*
_tmp1511[1];struct Cyc_String_pa_struct _tmp1510;(_tmp1510.tag=0,((_tmp1510.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpDE9),((_tmp1511[0]=& _tmp1510,
Cyc_Tcutil_terr(loc,((_tmp1512="bad field designator %s",_tag_dyneither(_tmp1512,
sizeof(char),24))),_tag_dyneither(_tmp1511,sizeof(void*),1)))))));}goto _LL6B4;}
_LL6B4:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple30*)fields->hd)).f2){{const char*_tmp1515;void*_tmp1514;(
_tmp1514=0,Cyc_Tcutil_terr(loc,((_tmp1515="too few arguments to struct",
_tag_dyneither(_tmp1515,sizeof(char),28))),_tag_dyneither(_tmp1514,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple30*)fields->hd)).f2){if(found){const char*_tmp1518;void*_tmp1517;(_tmp1517=
0,Cyc_Tcutil_terr(loc,((_tmp1518="only one member of a union is allowed",
_tag_dyneither(_tmp1518,sizeof(char),38))),_tag_dyneither(_tmp1517,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp151B;void*_tmp151A;(_tmp151A=0,Cyc_Tcutil_terr(
loc,((_tmp151B="missing member for union",_tag_dyneither(_tmp151B,sizeof(char),
25))),_tag_dyneither(_tmp151A,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpDFE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpE00;
void*_tmpE01;struct Cyc_Absyn_PtrAtts _tmpE02;union Cyc_Absyn_Constraint*_tmpE03;
_LL6BA: {struct Cyc_Absyn_PointerType_struct*_tmpDFF=(struct Cyc_Absyn_PointerType_struct*)
_tmpDFE;if(_tmpDFF->tag != 5)goto _LL6BC;else{_tmpE00=_tmpDFF->f1;_tmpE01=_tmpE00.elt_typ;
_tmpE02=_tmpE00.ptr_atts;_tmpE03=_tmpE02.bounds;}}_LL6BB: {void*_tmpE04=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpE03);_LL6BF: {struct Cyc_Absyn_DynEither_b_struct*_tmpE05=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE04;if(_tmpE05->tag != 0)goto _LL6C1;}_LL6C0:*elt_typ_dest=_tmpE01;return 1;
_LL6C1:;_LL6C2: return 0;_LL6BE:;}_LL6BC:;_LL6BD: return 0;_LL6B9:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpE06=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpE08;
void*_tmpE09;struct Cyc_Absyn_PtrAtts _tmpE0A;union Cyc_Absyn_Constraint*_tmpE0B;
_LL6C4: {struct Cyc_Absyn_PointerType_struct*_tmpE07=(struct Cyc_Absyn_PointerType_struct*)
_tmpE06;if(_tmpE07->tag != 5)goto _LL6C6;else{_tmpE08=_tmpE07->f1;_tmpE09=_tmpE08.elt_typ;
_tmpE0A=_tmpE08.ptr_atts;_tmpE0B=_tmpE0A.zero_term;}}_LL6C5:*elt_typ_dest=
_tmpE09;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpE0B);_LL6C6:;_LL6C7: return 0;_LL6C3:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpE0C=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpE0E;void*_tmpE0F;struct Cyc_Absyn_PtrAtts _tmpE10;
union Cyc_Absyn_Constraint*_tmpE11;union Cyc_Absyn_Constraint*_tmpE12;struct Cyc_Absyn_ArrayInfo
_tmpE14;void*_tmpE15;struct Cyc_Absyn_Tqual _tmpE16;struct Cyc_Absyn_Exp*_tmpE17;
union Cyc_Absyn_Constraint*_tmpE18;_LL6C9: {struct Cyc_Absyn_PointerType_struct*
_tmpE0D=(struct Cyc_Absyn_PointerType_struct*)_tmpE0C;if(_tmpE0D->tag != 5)goto
_LL6CB;else{_tmpE0E=_tmpE0D->f1;_tmpE0F=_tmpE0E.elt_typ;_tmpE10=_tmpE0E.ptr_atts;
_tmpE11=_tmpE10.bounds;_tmpE12=_tmpE10.zero_term;}}_LL6CA: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE12)){*ptr_type=t;*elt_type=
_tmpE0F;{void*_tmpE19=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpE11);_LL6D0: {struct Cyc_Absyn_DynEither_b_struct*
_tmpE1A=(struct Cyc_Absyn_DynEither_b_struct*)_tmpE19;if(_tmpE1A->tag != 0)goto
_LL6D2;}_LL6D1:*is_dyneither=1;goto _LL6CF;_LL6D2:;_LL6D3:*is_dyneither=0;goto
_LL6CF;_LL6CF:;}return 1;}else{return 0;}_LL6CB: {struct Cyc_Absyn_ArrayType_struct*
_tmpE13=(struct Cyc_Absyn_ArrayType_struct*)_tmpE0C;if(_tmpE13->tag != 9)goto
_LL6CD;else{_tmpE14=_tmpE13->f1;_tmpE15=_tmpE14.elt_type;_tmpE16=_tmpE14.tq;
_tmpE17=_tmpE14.num_elts;_tmpE18=_tmpE14.zero_term;}}_LL6CC: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE18)){*elt_type=_tmpE15;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp1530;struct Cyc_Absyn_PtrAtts
_tmp152F;struct Cyc_Absyn_Upper_b_struct _tmp152E;struct Cyc_Absyn_Upper_b_struct*
_tmp152D;struct Cyc_Absyn_PtrInfo _tmp152C;struct Cyc_Absyn_PointerType_struct*
_tmp152B;*ptr_type=(void*)((_tmp152B=_cycalloc(sizeof(*_tmp152B)),((_tmp152B[0]=((
_tmp1530.tag=5,((_tmp1530.f1=((_tmp152C.elt_typ=_tmpE15,((_tmp152C.elt_tq=
_tmpE16,((_tmp152C.ptr_atts=((_tmp152F.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp152F.nullable=Cyc_Absyn_false_conref,((_tmp152F.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp152D=_cycalloc(sizeof(*_tmp152D)),((
_tmp152D[0]=((_tmp152E.tag=1,((_tmp152E.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpE17),_tmp152E)))),_tmp152D))))),((_tmp152F.zero_term=_tmpE18,((_tmp152F.ptrloc=
0,_tmp152F)))))))))),_tmp152C)))))),_tmp1530)))),_tmp152B))));}return 1;}else{
return 0;}_LL6CD:;_LL6CE: return 0;_LL6C8:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*
_tmpE21=e1->r;struct Cyc_Absyn_Exp*_tmpE24;struct Cyc_Absyn_Exp*_tmpE26;struct Cyc_Absyn_Exp*
_tmpE28;struct Cyc_Absyn_Exp*_tmpE2A;struct Cyc_Absyn_Exp*_tmpE2C;struct Cyc_Absyn_Exp*
_tmpE2E;_LL6D5: {struct Cyc_Absyn_Cast_e_struct*_tmpE22=(struct Cyc_Absyn_Cast_e_struct*)
_tmpE21;if(_tmpE22->tag != 15)goto _LL6D7;}_LL6D6: {const char*_tmp1534;void*
_tmp1533[1];struct Cyc_String_pa_struct _tmp1532;(_tmp1532.tag=0,((_tmp1532.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp1533[0]=& _tmp1532,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1534="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp1534,sizeof(char),29))),_tag_dyneither(_tmp1533,sizeof(void*),1)))))));}
_LL6D7: {struct Cyc_Absyn_Deref_e_struct*_tmpE23=(struct Cyc_Absyn_Deref_e_struct*)
_tmpE21;if(_tmpE23->tag != 21)goto _LL6D9;else{_tmpE24=_tmpE23->f1;}}_LL6D8:
_tmpE26=_tmpE24;goto _LL6DA;_LL6D9: {struct Cyc_Absyn_Subscript_e_struct*_tmpE25=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpE21;if(_tmpE25->tag != 24)goto _LL6DB;
else{_tmpE26=_tmpE25->f1;}}_LL6DA: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpE26->topt))->v,ptr_type,is_dyneither,
elt_type);_LL6DB: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpE27=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpE21;if(_tmpE27->tag != 23)goto _LL6DD;else{_tmpE28=_tmpE27->f1;}}_LL6DC:
_tmpE2A=_tmpE28;goto _LL6DE;_LL6DD: {struct Cyc_Absyn_AggrMember_e_struct*_tmpE29=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpE21;if(_tmpE29->tag != 22)goto _LL6DF;
else{_tmpE2A=_tmpE29->f1;}}_LL6DE: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpE2A->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp1538;void*_tmp1537[1];struct Cyc_String_pa_struct _tmp1536;(_tmp1536.tag=
0,((_tmp1536.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1537[0]=& _tmp1536,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1538="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp1538,sizeof(char),51))),_tag_dyneither(_tmp1537,sizeof(void*),
1)))))));}return 0;_LL6DF: {struct Cyc_Absyn_Instantiate_e_struct*_tmpE2B=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpE21;if(_tmpE2B->tag != 14)goto _LL6E1;else{
_tmpE2C=_tmpE2B->f1;}}_LL6E0: _tmpE2E=_tmpE2C;goto _LL6E2;_LL6E1: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpE2D=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpE21;if(_tmpE2D->tag != 13)
goto _LL6E3;else{_tmpE2E=_tmpE2D->f1;}}_LL6E2: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE2E->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp153C;void*_tmp153B[1];struct Cyc_String_pa_struct
_tmp153A;(_tmp153A.tag=0,((_tmp153A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp153B[0]=& _tmp153A,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp153C="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp153C,sizeof(char),49))),_tag_dyneither(_tmp153B,sizeof(void*),
1)))))));}return 0;_LL6E3: {struct Cyc_Absyn_Var_e_struct*_tmpE2F=(struct Cyc_Absyn_Var_e_struct*)
_tmpE21;if(_tmpE2F->tag != 1)goto _LL6E5;}_LL6E4: return 0;_LL6E5:;_LL6E6: {const
char*_tmp1540;void*_tmp153F[1];struct Cyc_String_pa_struct _tmp153E;(_tmp153E.tag=
0,((_tmp153E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp153F[0]=& _tmp153E,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1540="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp1540,sizeof(char),39))),_tag_dyneither(_tmp153F,sizeof(void*),
1)))))));}_LL6D4:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t);int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)& Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore);}static int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
static int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){void*_tmpE3C=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpE3F;_LL6E8: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpE3D=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpE3C;if(_tmpE3D->tag != 22)goto
_LL6EA;}_LL6E9: return 1;_LL6EA: {struct Cyc_Absyn_VarType_struct*_tmpE3E=(struct
Cyc_Absyn_VarType_struct*)_tmpE3C;if(_tmpE3E->tag != 2)goto _LL6EC;else{_tmpE3F=
_tmpE3E->f1;}}_LL6EB: {struct Cyc_Absyn_Kind _tmpE41;enum Cyc_Absyn_KindQual
_tmpE42;enum Cyc_Absyn_AliasQual _tmpE43;struct Cyc_Absyn_Kind*_tmpE40=Cyc_Tcutil_tvar_kind(
_tmpE3F,& Cyc_Tcutil_rk);_tmpE41=*_tmpE40;_tmpE42=_tmpE41.kind;_tmpE43=_tmpE41.aliasqual;
if(_tmpE42 == Cyc_Absyn_RgnKind  && (_tmpE43 == Cyc_Absyn_Unique  || _tmpE43 == Cyc_Absyn_Top
 && !must_be_unique)){void*_tmpE44=Cyc_Absyn_compress_kb(_tmpE3F->kind);struct
Cyc_Core_Opt*_tmpE46;struct Cyc_Core_Opt**_tmpE47;struct Cyc_Absyn_Kind*_tmpE48;
struct Cyc_Absyn_Kind _tmpE49;enum Cyc_Absyn_KindQual _tmpE4A;enum Cyc_Absyn_AliasQual
_tmpE4B;_LL6EF: {struct Cyc_Absyn_Less_kb_struct*_tmpE45=(struct Cyc_Absyn_Less_kb_struct*)
_tmpE44;if(_tmpE45->tag != 2)goto _LL6F1;else{_tmpE46=_tmpE45->f1;_tmpE47=(struct
Cyc_Core_Opt**)& _tmpE45->f1;_tmpE48=_tmpE45->f2;_tmpE49=*_tmpE48;_tmpE4A=_tmpE49.kind;
if(_tmpE4A != Cyc_Absyn_RgnKind)goto _LL6F1;_tmpE4B=_tmpE49.aliasqual;if(_tmpE4B != 
Cyc_Absyn_Top)goto _LL6F1;}}_LL6F0:{struct Cyc_Absyn_Less_kb_struct*_tmp1546;
struct Cyc_Absyn_Less_kb_struct _tmp1545;struct Cyc_Core_Opt*_tmp1544;*_tmpE47=((
_tmp1544=_cycalloc(sizeof(*_tmp1544)),((_tmp1544->v=(void*)((_tmp1546=_cycalloc(
sizeof(*_tmp1546)),((_tmp1546[0]=((_tmp1545.tag=2,((_tmp1545.f1=0,((_tmp1545.f2=&
Cyc_Tcutil_rk,_tmp1545)))))),_tmp1546)))),_tmp1544))));}return 0;_LL6F1:;_LL6F2:
return 1;_LL6EE:;}return 0;}_LL6EC:;_LL6ED: return 0;_LL6E7:;}static int Cyc_Tcutil_is_noalias_pointer_aux(
void*t,int must_be_unique);static int Cyc_Tcutil_is_noalias_pointer_aux(void*t,int
must_be_unique){void*_tmpE4F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE51;struct Cyc_Absyn_PtrAtts _tmpE52;void*_tmpE53;struct Cyc_Absyn_Tvar*_tmpE55;
_LL6F4: {struct Cyc_Absyn_PointerType_struct*_tmpE50=(struct Cyc_Absyn_PointerType_struct*)
_tmpE4F;if(_tmpE50->tag != 5)goto _LL6F6;else{_tmpE51=_tmpE50->f1;_tmpE52=_tmpE51.ptr_atts;
_tmpE53=_tmpE52.rgn;}}_LL6F5: return Cyc_Tcutil_is_noalias_region(_tmpE53,
must_be_unique);_LL6F6: {struct Cyc_Absyn_VarType_struct*_tmpE54=(struct Cyc_Absyn_VarType_struct*)
_tmpE4F;if(_tmpE54->tag != 2)goto _LL6F8;else{_tmpE55=_tmpE54->f1;}}_LL6F7: {
struct Cyc_Absyn_Kind _tmpE57;enum Cyc_Absyn_KindQual _tmpE58;enum Cyc_Absyn_AliasQual
_tmpE59;struct Cyc_Absyn_Kind*_tmpE56=Cyc_Tcutil_tvar_kind(_tmpE55,& Cyc_Tcutil_bk);
_tmpE57=*_tmpE56;_tmpE58=_tmpE57.kind;_tmpE59=_tmpE57.aliasqual;switch(_tmpE58){
case Cyc_Absyn_BoxKind: _LL6FA: goto _LL6FB;case Cyc_Absyn_AnyKind: _LL6FB: goto _LL6FC;
case Cyc_Absyn_MemKind: _LL6FC: if(_tmpE59 == Cyc_Absyn_Unique  || _tmpE59 == Cyc_Absyn_Top){
void*_tmpE5A=Cyc_Absyn_compress_kb(_tmpE55->kind);struct Cyc_Core_Opt*_tmpE5C;
struct Cyc_Core_Opt**_tmpE5D;struct Cyc_Absyn_Kind*_tmpE5E;struct Cyc_Absyn_Kind
_tmpE5F;enum Cyc_Absyn_KindQual _tmpE60;enum Cyc_Absyn_AliasQual _tmpE61;_LL6FF: {
struct Cyc_Absyn_Less_kb_struct*_tmpE5B=(struct Cyc_Absyn_Less_kb_struct*)_tmpE5A;
if(_tmpE5B->tag != 2)goto _LL701;else{_tmpE5C=_tmpE5B->f1;_tmpE5D=(struct Cyc_Core_Opt**)&
_tmpE5B->f1;_tmpE5E=_tmpE5B->f2;_tmpE5F=*_tmpE5E;_tmpE60=_tmpE5F.kind;_tmpE61=
_tmpE5F.aliasqual;if(_tmpE61 != Cyc_Absyn_Top)goto _LL701;}}_LL700:{struct Cyc_Absyn_Less_kb_struct*
_tmp1550;struct Cyc_Absyn_Kind*_tmp154F;struct Cyc_Absyn_Less_kb_struct _tmp154E;
struct Cyc_Core_Opt*_tmp154D;*_tmpE5D=((_tmp154D=_cycalloc(sizeof(*_tmp154D)),((
_tmp154D->v=(void*)((_tmp1550=_cycalloc(sizeof(*_tmp1550)),((_tmp1550[0]=((
_tmp154E.tag=2,((_tmp154E.f1=0,((_tmp154E.f2=((_tmp154F=_cycalloc_atomic(sizeof(*
_tmp154F)),((_tmp154F->kind=_tmpE60,((_tmp154F->aliasqual=Cyc_Absyn_Aliasable,
_tmp154F)))))),_tmp154E)))))),_tmp1550)))),_tmp154D))));}return 0;_LL701:;_LL702:
return 1;_LL6FE:;}return 0;default: _LL6FD: return 0;}}_LL6F8:;_LL6F9: return 0;_LL6F3:;}
int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_pointer(void*t){
return Cyc_Tcutil_is_noalias_pointer_aux(t,0);}int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*rgn,void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t){void*_tmpE66=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpE66))return 1;{void*_tmpE67=_tmpE66;struct Cyc_List_List*_tmpE69;struct Cyc_Absyn_AggrInfo
_tmpE6B;union Cyc_Absyn_AggrInfoU _tmpE6C;struct Cyc_Absyn_Aggrdecl**_tmpE6D;struct
Cyc_List_List*_tmpE6E;struct Cyc_List_List*_tmpE70;struct Cyc_Absyn_AggrInfo
_tmpE72;union Cyc_Absyn_AggrInfoU _tmpE73;struct _tuple4 _tmpE74;struct Cyc_Absyn_DatatypeInfo
_tmpE76;union Cyc_Absyn_DatatypeInfoU _tmpE77;struct Cyc_List_List*_tmpE78;struct
Cyc_Absyn_DatatypeFieldInfo _tmpE7A;union Cyc_Absyn_DatatypeFieldInfoU _tmpE7B;
struct Cyc_List_List*_tmpE7C;_LL705: {struct Cyc_Absyn_TupleType_struct*_tmpE68=(
struct Cyc_Absyn_TupleType_struct*)_tmpE67;if(_tmpE68->tag != 11)goto _LL707;else{
_tmpE69=_tmpE68->f1;}}_LL706: while(_tmpE69 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpE69->hd)).f2))return 1;_tmpE69=_tmpE69->tl;}return 0;
_LL707: {struct Cyc_Absyn_AggrType_struct*_tmpE6A=(struct Cyc_Absyn_AggrType_struct*)
_tmpE67;if(_tmpE6A->tag != 12)goto _LL709;else{_tmpE6B=_tmpE6A->f1;_tmpE6C=_tmpE6B.aggr_info;
if((_tmpE6C.KnownAggr).tag != 2)goto _LL709;_tmpE6D=(struct Cyc_Absyn_Aggrdecl**)(
_tmpE6C.KnownAggr).val;_tmpE6E=_tmpE6B.targs;}}_LL708: if((*_tmpE6D)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpE7D=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpE6D)->tvs,_tmpE6E);struct Cyc_List_List*_tmpE7E=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpE6D)->impl))->fields;void*t;
while(_tmpE7E != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpE7D,((struct Cyc_Absyn_Aggrfield*)
_tmpE7E->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpE7E=_tmpE7E->tl;}return 0;}_LL709: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpE6F=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpE67;if(_tmpE6F->tag != 13)goto
_LL70B;else{_tmpE70=_tmpE6F->f2;}}_LL70A: while(_tmpE70 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpE70->hd)->type))return 1;_tmpE70=_tmpE70->tl;}
return 0;_LL70B: {struct Cyc_Absyn_AggrType_struct*_tmpE71=(struct Cyc_Absyn_AggrType_struct*)
_tmpE67;if(_tmpE71->tag != 12)goto _LL70D;else{_tmpE72=_tmpE71->f1;_tmpE73=_tmpE72.aggr_info;
if((_tmpE73.UnknownAggr).tag != 1)goto _LL70D;_tmpE74=(struct _tuple4)(_tmpE73.UnknownAggr).val;}}
_LL70C: {const char*_tmp1553;void*_tmp1552;(_tmp1552=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1553="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp1553,sizeof(char),36))),_tag_dyneither(_tmp1552,sizeof(void*),
0)));}_LL70D: {struct Cyc_Absyn_DatatypeType_struct*_tmpE75=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpE67;if(_tmpE75->tag != 3)goto _LL70F;else{_tmpE76=_tmpE75->f1;_tmpE77=_tmpE76.datatype_info;
_tmpE78=_tmpE76.targs;}}_LL70E: {union Cyc_Absyn_DatatypeInfoU _tmpE81=_tmpE77;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpE82;struct _tuple2*_tmpE83;int _tmpE84;
struct Cyc_Absyn_Datatypedecl**_tmpE85;struct Cyc_Absyn_Datatypedecl*_tmpE86;
struct Cyc_Absyn_Datatypedecl _tmpE87;struct Cyc_List_List*_tmpE88;struct Cyc_Core_Opt*
_tmpE89;_LL714: if((_tmpE81.UnknownDatatype).tag != 1)goto _LL716;_tmpE82=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpE81.UnknownDatatype).val;_tmpE83=_tmpE82.name;
_tmpE84=_tmpE82.is_extensible;_LL715: {const char*_tmp1556;void*_tmp1555;(
_tmp1555=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1556="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp1556,
sizeof(char),40))),_tag_dyneither(_tmp1555,sizeof(void*),0)));}_LL716: if((
_tmpE81.KnownDatatype).tag != 2)goto _LL713;_tmpE85=(struct Cyc_Absyn_Datatypedecl**)(
_tmpE81.KnownDatatype).val;_tmpE86=*_tmpE85;_tmpE87=*_tmpE86;_tmpE88=_tmpE87.tvs;
_tmpE89=_tmpE87.fields;_LL717: return 0;_LL713:;}_LL70F: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpE79=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpE67;if(_tmpE79->tag != 4)
goto _LL711;else{_tmpE7A=_tmpE79->f1;_tmpE7B=_tmpE7A.field_info;_tmpE7C=_tmpE7A.targs;}}
_LL710: {union Cyc_Absyn_DatatypeFieldInfoU _tmpE8C=_tmpE7B;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpE8D;struct _tuple3 _tmpE8E;struct Cyc_Absyn_Datatypedecl*_tmpE8F;struct Cyc_Absyn_Datatypefield*
_tmpE90;_LL719: if((_tmpE8C.UnknownDatatypefield).tag != 1)goto _LL71B;_tmpE8D=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpE8C.UnknownDatatypefield).val;
_LL71A: {const char*_tmp1559;void*_tmp1558;(_tmp1558=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1559="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp1559,sizeof(char),46))),_tag_dyneither(_tmp1558,sizeof(void*),
0)));}_LL71B: if((_tmpE8C.KnownDatatypefield).tag != 2)goto _LL718;_tmpE8E=(struct
_tuple3)(_tmpE8C.KnownDatatypefield).val;_tmpE8F=_tmpE8E.f1;_tmpE90=_tmpE8E.f2;
_LL71C: {struct Cyc_List_List*_tmpE93=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpE8F->tvs,_tmpE7C);struct Cyc_List_List*_tmpE94=
_tmpE90->typs;while(_tmpE94 != 0){_tmpE66=Cyc_Tcutil_rsubstitute(rgn,_tmpE93,(*((
struct _tuple11*)_tmpE94->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpE66))return 1;_tmpE94=_tmpE94->tl;}return 0;}_LL718:;}_LL711:;_LL712: return 0;
_LL704:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f);static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*
r,void*t,struct _dyneither_ptr*f){void*_tmpE95=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpE97;union Cyc_Absyn_AggrInfoU _tmpE98;struct Cyc_Absyn_Aggrdecl**_tmpE99;struct
Cyc_Absyn_Aggrdecl*_tmpE9A;struct Cyc_List_List*_tmpE9B;struct Cyc_List_List*
_tmpE9D;_LL71E: {struct Cyc_Absyn_AggrType_struct*_tmpE96=(struct Cyc_Absyn_AggrType_struct*)
_tmpE95;if(_tmpE96->tag != 12)goto _LL720;else{_tmpE97=_tmpE96->f1;_tmpE98=_tmpE97.aggr_info;
if((_tmpE98.KnownAggr).tag != 2)goto _LL720;_tmpE99=(struct Cyc_Absyn_Aggrdecl**)(
_tmpE98.KnownAggr).val;_tmpE9A=*_tmpE99;_tmpE9B=_tmpE97.targs;}}_LL71F: _tmpE9D=
_tmpE9A->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE9A->impl))->fields;
goto _LL721;_LL720: {struct Cyc_Absyn_AnonAggrType_struct*_tmpE9C=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE95;if(_tmpE9C->tag != 13)goto _LL722;else{_tmpE9D=_tmpE9C->f2;}}_LL721: {
struct Cyc_Absyn_Aggrfield*_tmpE9E=Cyc_Absyn_lookup_field(_tmpE9D,f);{struct Cyc_Absyn_Aggrfield*
_tmpE9F=_tmpE9E;struct Cyc_Absyn_Aggrfield _tmpEA0;void*_tmpEA1;_LL725: if(_tmpE9F
!= 0)goto _LL727;_LL726: {const char*_tmp155C;void*_tmp155B;(_tmp155B=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp155C="is_noalias_field: missing field",
_tag_dyneither(_tmp155C,sizeof(char),32))),_tag_dyneither(_tmp155B,sizeof(void*),
0)));}_LL727: if(_tmpE9F == 0)goto _LL724;_tmpEA0=*_tmpE9F;_tmpEA1=_tmpEA0.type;
_LL728: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpEA1);_LL724:;}return 0;}
_LL722:;_LL723: {const char*_tmp1560;void*_tmp155F[1];struct Cyc_String_pa_struct
_tmp155E;(_tmp155E.tag=0,((_tmp155E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp155F[0]=& _tmp155E,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1560="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1560,sizeof(char),36))),_tag_dyneither(_tmp155F,sizeof(void*),
1)))))));}_LL71D:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e);static int Cyc_Tcutil_is_noalias_path_aux(struct
_RegionHandle*r,struct Cyc_Absyn_Exp*e){void*_tmpEA7=e->r;void*_tmpEA9;struct Cyc_Absyn_Exp*
_tmpEAC;struct Cyc_Absyn_Exp*_tmpEAE;struct Cyc_Absyn_Exp*_tmpEB0;struct
_dyneither_ptr*_tmpEB1;struct Cyc_Absyn_Exp*_tmpEB3;struct Cyc_Absyn_Exp*_tmpEB4;
struct Cyc_Absyn_Exp*_tmpEB6;struct Cyc_Absyn_Exp*_tmpEB7;struct Cyc_Absyn_Exp*
_tmpEB9;struct Cyc_Absyn_Exp*_tmpEBB;struct Cyc_Absyn_Stmt*_tmpEBD;_LL72A: {struct
Cyc_Absyn_Var_e_struct*_tmpEA8=(struct Cyc_Absyn_Var_e_struct*)_tmpEA7;if(_tmpEA8->tag
!= 1)goto _LL72C;else{_tmpEA9=(void*)_tmpEA8->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpEAA=(struct Cyc_Absyn_Global_b_struct*)_tmpEA9;if(_tmpEAA->tag != 1)goto _LL72C;};}}
_LL72B: return 0;_LL72C: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpEAB=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpEA7;if(_tmpEAB->tag != 23)goto _LL72E;else{_tmpEAC=_tmpEAB->f1;}}_LL72D:
_tmpEAE=_tmpEAC;goto _LL72F;_LL72E: {struct Cyc_Absyn_Deref_e_struct*_tmpEAD=(
struct Cyc_Absyn_Deref_e_struct*)_tmpEA7;if(_tmpEAD->tag != 21)goto _LL730;else{
_tmpEAE=_tmpEAD->f1;}}_LL72F: return Cyc_Tcutil_is_noalias_pointer_aux((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpEAE->topt))->v,1) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpEAE);_LL730: {struct Cyc_Absyn_AggrMember_e_struct*_tmpEAF=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpEA7;if(_tmpEAF->tag != 22)goto _LL732;else{_tmpEB0=_tmpEAF->f1;_tmpEB1=_tmpEAF->f2;}}
_LL731: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpEB0);_LL732: {struct Cyc_Absyn_Subscript_e_struct*
_tmpEB2=(struct Cyc_Absyn_Subscript_e_struct*)_tmpEA7;if(_tmpEB2->tag != 24)goto
_LL734;else{_tmpEB3=_tmpEB2->f1;_tmpEB4=_tmpEB2->f2;}}_LL733: {void*_tmpEBE=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEB3->topt))->v);_LL73F: {struct Cyc_Absyn_TupleType_struct*
_tmpEBF=(struct Cyc_Absyn_TupleType_struct*)_tmpEBE;if(_tmpEBF->tag != 11)goto
_LL741;}_LL740: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpEB3);_LL741:;_LL742:
return 0;_LL73E:;}_LL734: {struct Cyc_Absyn_Conditional_e_struct*_tmpEB5=(struct
Cyc_Absyn_Conditional_e_struct*)_tmpEA7;if(_tmpEB5->tag != 6)goto _LL736;else{
_tmpEB6=_tmpEB5->f2;_tmpEB7=_tmpEB5->f3;}}_LL735: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpEB6) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpEB7);_LL736: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpEB8=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpEA7;if(_tmpEB8->tag != 9)goto _LL738;
else{_tmpEB9=_tmpEB8->f2;}}_LL737: _tmpEBB=_tmpEB9;goto _LL739;_LL738: {struct Cyc_Absyn_Cast_e_struct*
_tmpEBA=(struct Cyc_Absyn_Cast_e_struct*)_tmpEA7;if(_tmpEBA->tag != 15)goto _LL73A;
else{_tmpEBB=_tmpEBA->f2;}}_LL739: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpEBB);
_LL73A: {struct Cyc_Absyn_StmtExp_e_struct*_tmpEBC=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpEA7;if(_tmpEBC->tag != 37)goto _LL73C;else{_tmpEBD=_tmpEBC->f1;}}_LL73B: while(
1){void*_tmpEC0=_tmpEBD->r;struct Cyc_Absyn_Stmt*_tmpEC2;struct Cyc_Absyn_Stmt*
_tmpEC3;struct Cyc_Absyn_Decl*_tmpEC5;struct Cyc_Absyn_Stmt*_tmpEC6;struct Cyc_Absyn_Exp*
_tmpEC8;_LL744: {struct Cyc_Absyn_Seq_s_struct*_tmpEC1=(struct Cyc_Absyn_Seq_s_struct*)
_tmpEC0;if(_tmpEC1->tag != 2)goto _LL746;else{_tmpEC2=_tmpEC1->f1;_tmpEC3=_tmpEC1->f2;}}
_LL745: _tmpEBD=_tmpEC3;goto _LL743;_LL746: {struct Cyc_Absyn_Decl_s_struct*_tmpEC4=(
struct Cyc_Absyn_Decl_s_struct*)_tmpEC0;if(_tmpEC4->tag != 12)goto _LL748;else{
_tmpEC5=_tmpEC4->f1;_tmpEC6=_tmpEC4->f2;}}_LL747: _tmpEBD=_tmpEC6;goto _LL743;
_LL748: {struct Cyc_Absyn_Exp_s_struct*_tmpEC7=(struct Cyc_Absyn_Exp_s_struct*)
_tmpEC0;if(_tmpEC7->tag != 1)goto _LL74A;else{_tmpEC8=_tmpEC7->f1;}}_LL749: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpEC8);_LL74A:;_LL74B: {const char*_tmp1563;
void*_tmp1562;(_tmp1562=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1563="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp1563,sizeof(char),40))),_tag_dyneither(_tmp1562,sizeof(void*),
0)));}_LL743:;}_LL73C:;_LL73D: return 1;_LL729:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple17 _tmp1564;struct _tuple17 bogus_ans=(_tmp1564.f1=0,((_tmp1564.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1564)));void*_tmpECB=e->r;struct _tuple2*_tmpECD;
void*_tmpECE;struct Cyc_Absyn_Exp*_tmpED0;struct _dyneither_ptr*_tmpED1;int _tmpED2;
struct Cyc_Absyn_Exp*_tmpED4;struct _dyneither_ptr*_tmpED5;int _tmpED6;struct Cyc_Absyn_Exp*
_tmpED8;struct Cyc_Absyn_Exp*_tmpEDA;struct Cyc_Absyn_Exp*_tmpEDB;_LL74D: {struct
Cyc_Absyn_Var_e_struct*_tmpECC=(struct Cyc_Absyn_Var_e_struct*)_tmpECB;if(_tmpECC->tag
!= 1)goto _LL74F;else{_tmpECD=_tmpECC->f1;_tmpECE=(void*)_tmpECC->f2;}}_LL74E: {
void*_tmpEDC=_tmpECE;struct Cyc_Absyn_Vardecl*_tmpEE0;struct Cyc_Absyn_Vardecl*
_tmpEE2;struct Cyc_Absyn_Vardecl*_tmpEE4;struct Cyc_Absyn_Vardecl*_tmpEE6;_LL75A: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpEDD=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpEDC;if(_tmpEDD->tag != 0)goto _LL75C;}_LL75B: goto _LL75D;_LL75C: {struct Cyc_Absyn_Funname_b_struct*
_tmpEDE=(struct Cyc_Absyn_Funname_b_struct*)_tmpEDC;if(_tmpEDE->tag != 2)goto
_LL75E;}_LL75D: return bogus_ans;_LL75E: {struct Cyc_Absyn_Global_b_struct*_tmpEDF=(
struct Cyc_Absyn_Global_b_struct*)_tmpEDC;if(_tmpEDF->tag != 1)goto _LL760;else{
_tmpEE0=_tmpEDF->f1;}}_LL75F: {void*_tmpEE7=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL767: {struct Cyc_Absyn_ArrayType_struct*
_tmpEE8=(struct Cyc_Absyn_ArrayType_struct*)_tmpEE7;if(_tmpEE8->tag != 9)goto
_LL769;}_LL768: {struct _tuple17 _tmp1565;return(_tmp1565.f1=1,((_tmp1565.f2=(void*)&
Cyc_Absyn_HeapRgn_val,_tmp1565)));}_LL769:;_LL76A: {struct _tuple17 _tmp1566;
return(_tmp1566.f1=(_tmpEE0->tq).real_const,((_tmp1566.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp1566)));}_LL766:;}_LL760: {struct Cyc_Absyn_Local_b_struct*_tmpEE1=(struct Cyc_Absyn_Local_b_struct*)
_tmpEDC;if(_tmpEE1->tag != 4)goto _LL762;else{_tmpEE2=_tmpEE1->f1;}}_LL761: {void*
_tmpEEB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL76C: {struct Cyc_Absyn_ArrayType_struct*_tmpEEC=(struct Cyc_Absyn_ArrayType_struct*)
_tmpEEB;if(_tmpEEC->tag != 9)goto _LL76E;}_LL76D: {struct _tuple17 _tmp1567;return(
_tmp1567.f1=1,((_tmp1567.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpEE2->rgn))->v,
_tmp1567)));}_LL76E:;_LL76F: _tmpEE2->escapes=1;{struct _tuple17 _tmp1568;return(
_tmp1568.f1=(_tmpEE2->tq).real_const,((_tmp1568.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEE2->rgn))->v,_tmp1568)));};_LL76B:;}_LL762: {struct Cyc_Absyn_Pat_b_struct*
_tmpEE3=(struct Cyc_Absyn_Pat_b_struct*)_tmpEDC;if(_tmpEE3->tag != 5)goto _LL764;
else{_tmpEE4=_tmpEE3->f1;}}_LL763: _tmpEE6=_tmpEE4;goto _LL765;_LL764: {struct Cyc_Absyn_Param_b_struct*
_tmpEE5=(struct Cyc_Absyn_Param_b_struct*)_tmpEDC;if(_tmpEE5->tag != 3)goto _LL759;
else{_tmpEE6=_tmpEE5->f1;}}_LL765: _tmpEE6->escapes=1;{struct _tuple17 _tmp1569;
return(_tmp1569.f1=(_tmpEE6->tq).real_const,((_tmp1569.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEE6->rgn))->v,_tmp1569)));};_LL759:;}_LL74F: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpECF=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpECB;if(_tmpECF->tag != 22)goto
_LL751;else{_tmpED0=_tmpECF->f1;_tmpED1=_tmpECF->f2;_tmpED2=_tmpECF->f3;}}_LL750:
if(_tmpED2)return bogus_ans;{void*_tmpEF0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpED0->topt))->v);struct Cyc_List_List*_tmpEF2;struct Cyc_Absyn_AggrInfo
_tmpEF4;union Cyc_Absyn_AggrInfoU _tmpEF5;struct Cyc_Absyn_Aggrdecl**_tmpEF6;struct
Cyc_Absyn_Aggrdecl*_tmpEF7;_LL771: {struct Cyc_Absyn_AnonAggrType_struct*_tmpEF1=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmpEF0;if(_tmpEF1->tag != 13)goto _LL773;
else{_tmpEF2=_tmpEF1->f2;}}_LL772: {struct Cyc_Absyn_Aggrfield*_tmpEF8=Cyc_Absyn_lookup_field(
_tmpEF2,_tmpED1);if(_tmpEF8 != 0  && _tmpEF8->width != 0){struct _tuple17 _tmp156A;
return(_tmp156A.f1=(_tmpEF8->tq).real_const,((_tmp156A.f2=(Cyc_Tcutil_addressof_props(
te,_tmpED0)).f2,_tmp156A)));}return bogus_ans;}_LL773: {struct Cyc_Absyn_AggrType_struct*
_tmpEF3=(struct Cyc_Absyn_AggrType_struct*)_tmpEF0;if(_tmpEF3->tag != 12)goto
_LL775;else{_tmpEF4=_tmpEF3->f1;_tmpEF5=_tmpEF4.aggr_info;if((_tmpEF5.KnownAggr).tag
!= 2)goto _LL775;_tmpEF6=(struct Cyc_Absyn_Aggrdecl**)(_tmpEF5.KnownAggr).val;
_tmpEF7=*_tmpEF6;}}_LL774: {struct Cyc_Absyn_Aggrfield*_tmpEFA=Cyc_Absyn_lookup_decl_field(
_tmpEF7,_tmpED1);if(_tmpEFA != 0  && _tmpEFA->width != 0){struct _tuple17 _tmp156B;
return(_tmp156B.f1=(_tmpEFA->tq).real_const,((_tmp156B.f2=(Cyc_Tcutil_addressof_props(
te,_tmpED0)).f2,_tmp156B)));}return bogus_ans;}_LL775:;_LL776: return bogus_ans;
_LL770:;};_LL751: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpED3=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpECB;if(_tmpED3->tag != 23)goto _LL753;else{_tmpED4=_tmpED3->f1;_tmpED5=_tmpED3->f2;
_tmpED6=_tmpED3->f3;}}_LL752: if(_tmpED6)return bogus_ans;{void*_tmpEFC=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpED4->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpEFE;void*_tmpEFF;struct Cyc_Absyn_PtrAtts _tmpF00;void*_tmpF01;_LL778: {struct
Cyc_Absyn_PointerType_struct*_tmpEFD=(struct Cyc_Absyn_PointerType_struct*)
_tmpEFC;if(_tmpEFD->tag != 5)goto _LL77A;else{_tmpEFE=_tmpEFD->f1;_tmpEFF=_tmpEFE.elt_typ;
_tmpF00=_tmpEFE.ptr_atts;_tmpF01=_tmpF00.rgn;}}_LL779: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpF02=Cyc_Tcutil_compress(_tmpEFF);struct Cyc_List_List*_tmpF04;
struct Cyc_Absyn_AggrInfo _tmpF06;union Cyc_Absyn_AggrInfoU _tmpF07;struct Cyc_Absyn_Aggrdecl**
_tmpF08;struct Cyc_Absyn_Aggrdecl*_tmpF09;_LL77D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpF03=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpF02;if(_tmpF03->tag != 13)goto
_LL77F;else{_tmpF04=_tmpF03->f2;}}_LL77E: finfo=Cyc_Absyn_lookup_field(_tmpF04,
_tmpED5);goto _LL77C;_LL77F: {struct Cyc_Absyn_AggrType_struct*_tmpF05=(struct Cyc_Absyn_AggrType_struct*)
_tmpF02;if(_tmpF05->tag != 12)goto _LL781;else{_tmpF06=_tmpF05->f1;_tmpF07=_tmpF06.aggr_info;
if((_tmpF07.KnownAggr).tag != 2)goto _LL781;_tmpF08=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF07.KnownAggr).val;_tmpF09=*_tmpF08;}}_LL780: finfo=Cyc_Absyn_lookup_decl_field(
_tmpF09,_tmpED5);goto _LL77C;_LL781:;_LL782: return bogus_ans;_LL77C:;}if(finfo != 0
 && finfo->width != 0){struct _tuple17 _tmp156C;return(_tmp156C.f1=(finfo->tq).real_const,((
_tmp156C.f2=_tmpF01,_tmp156C)));}return bogus_ans;}_LL77A:;_LL77B: return bogus_ans;
_LL777:;};_LL753: {struct Cyc_Absyn_Deref_e_struct*_tmpED7=(struct Cyc_Absyn_Deref_e_struct*)
_tmpECB;if(_tmpED7->tag != 21)goto _LL755;else{_tmpED8=_tmpED7->f1;}}_LL754: {void*
_tmpF0B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpED8->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpF0D;struct Cyc_Absyn_Tqual _tmpF0E;struct Cyc_Absyn_PtrAtts
_tmpF0F;void*_tmpF10;_LL784: {struct Cyc_Absyn_PointerType_struct*_tmpF0C=(struct
Cyc_Absyn_PointerType_struct*)_tmpF0B;if(_tmpF0C->tag != 5)goto _LL786;else{
_tmpF0D=_tmpF0C->f1;_tmpF0E=_tmpF0D.elt_tq;_tmpF0F=_tmpF0D.ptr_atts;_tmpF10=
_tmpF0F.rgn;}}_LL785: {struct _tuple17 _tmp156D;return(_tmp156D.f1=_tmpF0E.real_const,((
_tmp156D.f2=_tmpF10,_tmp156D)));}_LL786:;_LL787: return bogus_ans;_LL783:;}_LL755: {
struct Cyc_Absyn_Subscript_e_struct*_tmpED9=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpECB;if(_tmpED9->tag != 24)goto _LL757;else{_tmpEDA=_tmpED9->f1;_tmpEDB=_tmpED9->f2;}}
_LL756: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpEDA->topt))->v);void*_tmpF12=t;struct Cyc_List_List*_tmpF14;struct Cyc_Absyn_PtrInfo
_tmpF16;struct Cyc_Absyn_Tqual _tmpF17;struct Cyc_Absyn_PtrAtts _tmpF18;void*_tmpF19;
struct Cyc_Absyn_ArrayInfo _tmpF1B;struct Cyc_Absyn_Tqual _tmpF1C;_LL789: {struct Cyc_Absyn_TupleType_struct*
_tmpF13=(struct Cyc_Absyn_TupleType_struct*)_tmpF12;if(_tmpF13->tag != 11)goto
_LL78B;else{_tmpF14=_tmpF13->f1;}}_LL78A: {unsigned int _tmpF1E;int _tmpF1F;struct
_tuple13 _tmpF1D=Cyc_Evexp_eval_const_uint_exp(_tmpEDB);_tmpF1E=_tmpF1D.f1;
_tmpF1F=_tmpF1D.f2;if(!_tmpF1F)return bogus_ans;{struct _tuple11*_tmpF20=Cyc_Absyn_lookup_tuple_field(
_tmpF14,(int)_tmpF1E);if(_tmpF20 != 0){struct _tuple17 _tmp156E;return(_tmp156E.f1=((*
_tmpF20).f1).real_const,((_tmp156E.f2=(Cyc_Tcutil_addressof_props(te,_tmpEDA)).f2,
_tmp156E)));}return bogus_ans;};}_LL78B: {struct Cyc_Absyn_PointerType_struct*
_tmpF15=(struct Cyc_Absyn_PointerType_struct*)_tmpF12;if(_tmpF15->tag != 5)goto
_LL78D;else{_tmpF16=_tmpF15->f1;_tmpF17=_tmpF16.elt_tq;_tmpF18=_tmpF16.ptr_atts;
_tmpF19=_tmpF18.rgn;}}_LL78C: {struct _tuple17 _tmp156F;return(_tmp156F.f1=_tmpF17.real_const,((
_tmp156F.f2=_tmpF19,_tmp156F)));}_LL78D: {struct Cyc_Absyn_ArrayType_struct*
_tmpF1A=(struct Cyc_Absyn_ArrayType_struct*)_tmpF12;if(_tmpF1A->tag != 9)goto
_LL78F;else{_tmpF1B=_tmpF1A->f1;_tmpF1C=_tmpF1B.tq;}}_LL78E: {struct _tuple17
_tmp1570;return(_tmp1570.f1=_tmpF1C.real_const,((_tmp1570.f2=(Cyc_Tcutil_addressof_props(
te,_tmpEDA)).f2,_tmp1570)));}_LL78F:;_LL790: return bogus_ans;_LL788:;}_LL757:;
_LL758:{const char*_tmp1573;void*_tmp1572;(_tmp1572=0,Cyc_Tcutil_terr(e->loc,((
_tmp1573="unary & applied to non-lvalue",_tag_dyneither(_tmp1573,sizeof(char),30))),
_tag_dyneither(_tmp1572,sizeof(void*),0)));}return bogus_ans;_LL74C:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpF27=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpF29;void*_tmpF2A;struct Cyc_Absyn_Tqual
_tmpF2B;union Cyc_Absyn_Constraint*_tmpF2C;_LL792: {struct Cyc_Absyn_ArrayType_struct*
_tmpF28=(struct Cyc_Absyn_ArrayType_struct*)_tmpF27;if(_tmpF28->tag != 9)goto
_LL794;else{_tmpF29=_tmpF28->f1;_tmpF2A=_tmpF29.elt_type;_tmpF2B=_tmpF29.tq;
_tmpF2C=_tmpF29.zero_term;}}_LL793: {void*_tmpF2E;struct _tuple17 _tmpF2D=Cyc_Tcutil_addressof_props(
te,e);_tmpF2E=_tmpF2D.f2;{struct Cyc_Absyn_Upper_b_struct _tmp1576;struct Cyc_Absyn_Upper_b_struct*
_tmp1575;return Cyc_Absyn_atb_typ(_tmpF2A,_tmpF2E,_tmpF2B,(void*)((_tmp1575=
_cycalloc(sizeof(*_tmp1575)),((_tmp1575[0]=((_tmp1576.tag=1,((_tmp1576.f1=e,
_tmp1576)))),_tmp1575)))),_tmpF2C);};}_LL794:;_LL795: return e_typ;_LL791:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b);void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union
Cyc_Absyn_Constraint*b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
x))Cyc_Absyn_compress_conref)(b);{void*_tmpF31=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpF34;
_LL797: {struct Cyc_Absyn_DynEither_b_struct*_tmpF32=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpF31;if(_tmpF32->tag != 0)goto _LL799;}_LL798: return;_LL799: {struct Cyc_Absyn_Upper_b_struct*
_tmpF33=(struct Cyc_Absyn_Upper_b_struct*)_tmpF31;if(_tmpF33->tag != 1)goto _LL796;
else{_tmpF34=_tmpF33->f1;}}_LL79A: {unsigned int _tmpF36;int _tmpF37;struct
_tuple13 _tmpF35=Cyc_Evexp_eval_const_uint_exp(_tmpF34);_tmpF36=_tmpF35.f1;
_tmpF37=_tmpF35.f2;if(_tmpF37  && _tmpF36 <= i){const char*_tmp157B;void*_tmp157A[2];
struct Cyc_Int_pa_struct _tmp1579;struct Cyc_Int_pa_struct _tmp1578;(_tmp1578.tag=1,((
_tmp1578.f1=(unsigned long)((int)i),((_tmp1579.tag=1,((_tmp1579.f1=(
unsigned long)((int)_tmpF36),((_tmp157A[0]=& _tmp1579,((_tmp157A[1]=& _tmp1578,Cyc_Tcutil_terr(
loc,((_tmp157B="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp157B,
sizeof(char),39))),_tag_dyneither(_tmp157A,sizeof(void*),2)))))))))))));}return;}
_LL796:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,
union Cyc_Absyn_Constraint*b);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*
b){void*_tmpF3C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpF3E;_LL79C: {struct
Cyc_Absyn_Upper_b_struct*_tmpF3D=(struct Cyc_Absyn_Upper_b_struct*)_tmpF3C;if(
_tmpF3D->tag != 1)goto _LL79E;else{_tmpF3E=_tmpF3D->f1;}}_LL79D: {unsigned int
_tmpF40;int _tmpF41;struct _tuple13 _tmpF3F=Cyc_Evexp_eval_const_uint_exp(_tmpF3E);
_tmpF40=_tmpF3F.f1;_tmpF41=_tmpF3F.f2;return _tmpF41  && _tmpF40 == 1;}_LL79E:;
_LL79F: return 0;_LL79B:;}int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_bits_only(
void*t){void*_tmpF42=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpF4A;
void*_tmpF4B;union Cyc_Absyn_Constraint*_tmpF4C;struct Cyc_List_List*_tmpF4E;
struct Cyc_Absyn_AggrInfo _tmpF50;union Cyc_Absyn_AggrInfoU _tmpF51;struct _tuple4
_tmpF52;struct Cyc_Absyn_AggrInfo _tmpF54;union Cyc_Absyn_AggrInfoU _tmpF55;struct
Cyc_Absyn_Aggrdecl**_tmpF56;struct Cyc_Absyn_Aggrdecl*_tmpF57;struct Cyc_List_List*
_tmpF58;struct Cyc_List_List*_tmpF5A;_LL7A1: {struct Cyc_Absyn_VoidType_struct*
_tmpF43=(struct Cyc_Absyn_VoidType_struct*)_tmpF42;if(_tmpF43->tag != 0)goto _LL7A3;}
_LL7A2: goto _LL7A4;_LL7A3: {struct Cyc_Absyn_IntType_struct*_tmpF44=(struct Cyc_Absyn_IntType_struct*)
_tmpF42;if(_tmpF44->tag != 6)goto _LL7A5;}_LL7A4: goto _LL7A6;_LL7A5: {struct Cyc_Absyn_FloatType_struct*
_tmpF45=(struct Cyc_Absyn_FloatType_struct*)_tmpF42;if(_tmpF45->tag != 7)goto
_LL7A7;}_LL7A6: goto _LL7A8;_LL7A7: {struct Cyc_Absyn_DoubleType_struct*_tmpF46=(
struct Cyc_Absyn_DoubleType_struct*)_tmpF42;if(_tmpF46->tag != 8)goto _LL7A9;}
_LL7A8: return 1;_LL7A9: {struct Cyc_Absyn_EnumType_struct*_tmpF47=(struct Cyc_Absyn_EnumType_struct*)
_tmpF42;if(_tmpF47->tag != 14)goto _LL7AB;}_LL7AA: goto _LL7AC;_LL7AB: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpF48=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpF42;if(_tmpF48->tag != 15)goto
_LL7AD;}_LL7AC: return 0;_LL7AD: {struct Cyc_Absyn_ArrayType_struct*_tmpF49=(struct
Cyc_Absyn_ArrayType_struct*)_tmpF42;if(_tmpF49->tag != 9)goto _LL7AF;else{_tmpF4A=
_tmpF49->f1;_tmpF4B=_tmpF4A.elt_type;_tmpF4C=_tmpF4A.zero_term;}}_LL7AE: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF4C) && Cyc_Tcutil_bits_only(
_tmpF4B);_LL7AF: {struct Cyc_Absyn_TupleType_struct*_tmpF4D=(struct Cyc_Absyn_TupleType_struct*)
_tmpF42;if(_tmpF4D->tag != 11)goto _LL7B1;else{_tmpF4E=_tmpF4D->f1;}}_LL7B0: for(0;
_tmpF4E != 0;_tmpF4E=_tmpF4E->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpF4E->hd)).f2))return 0;}return 1;_LL7B1: {struct Cyc_Absyn_AggrType_struct*
_tmpF4F=(struct Cyc_Absyn_AggrType_struct*)_tmpF42;if(_tmpF4F->tag != 12)goto
_LL7B3;else{_tmpF50=_tmpF4F->f1;_tmpF51=_tmpF50.aggr_info;if((_tmpF51.UnknownAggr).tag
!= 1)goto _LL7B3;_tmpF52=(struct _tuple4)(_tmpF51.UnknownAggr).val;}}_LL7B2: return
0;_LL7B3: {struct Cyc_Absyn_AggrType_struct*_tmpF53=(struct Cyc_Absyn_AggrType_struct*)
_tmpF42;if(_tmpF53->tag != 12)goto _LL7B5;else{_tmpF54=_tmpF53->f1;_tmpF55=_tmpF54.aggr_info;
if((_tmpF55.KnownAggr).tag != 2)goto _LL7B5;_tmpF56=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF55.KnownAggr).val;_tmpF57=*_tmpF56;_tmpF58=_tmpF54.targs;}}_LL7B4: if(
_tmpF57->impl == 0)return 0;{struct _RegionHandle _tmpF5B=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpF5B;_push_region(rgn);{struct Cyc_List_List*_tmpF5C=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpF57->tvs,_tmpF58);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF57->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpF5C,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpF5D=0;_npop_handler(0);return
_tmpF5D;}}}{int _tmpF5E=1;_npop_handler(0);return _tmpF5E;};};_pop_region(rgn);};
_LL7B5: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF59=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF42;if(_tmpF59->tag != 13)goto _LL7B7;else{_tmpF5A=_tmpF59->f2;}}_LL7B6: for(0;
_tmpF5A != 0;_tmpF5A=_tmpF5A->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpF5A->hd)->type))return 0;}return 1;_LL7B7:;_LL7B8: return 0;_LL7A0:;}static int
Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e);
static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmpF5F=e->r;struct _tuple2*_tmpF67;void*_tmpF68;struct Cyc_Absyn_Exp*
_tmpF6A;struct Cyc_Absyn_Exp*_tmpF6B;struct Cyc_Absyn_Exp*_tmpF6C;struct Cyc_Absyn_Exp*
_tmpF6E;struct Cyc_Absyn_Exp*_tmpF6F;struct Cyc_Absyn_Exp*_tmpF71;struct Cyc_Absyn_Exp*
_tmpF73;void*_tmpF75;struct Cyc_Absyn_Exp*_tmpF76;enum Cyc_Absyn_Coercion _tmpF77;
void*_tmpF79;struct Cyc_Absyn_Exp*_tmpF7A;struct Cyc_Absyn_Exp*_tmpF7C;struct Cyc_Absyn_Exp*
_tmpF7E;struct Cyc_Absyn_Exp*_tmpF7F;struct Cyc_List_List*_tmpF81;struct Cyc_List_List*
_tmpF83;struct Cyc_List_List*_tmpF85;enum Cyc_Absyn_Primop _tmpF87;struct Cyc_List_List*
_tmpF88;struct Cyc_List_List*_tmpF8A;struct Cyc_List_List*_tmpF8C;_LL7BA: {struct
Cyc_Absyn_Const_e_struct*_tmpF60=(struct Cyc_Absyn_Const_e_struct*)_tmpF5F;if(
_tmpF60->tag != 0)goto _LL7BC;}_LL7BB: goto _LL7BD;_LL7BC: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpF61=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpF5F;if(_tmpF61->tag != 18)goto
_LL7BE;}_LL7BD: goto _LL7BF;_LL7BE: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpF62=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpF5F;if(_tmpF62->tag != 19)goto _LL7C0;}
_LL7BF: goto _LL7C1;_LL7C0: {struct Cyc_Absyn_Offsetof_e_struct*_tmpF63=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpF5F;if(_tmpF63->tag != 20)goto _LL7C2;}_LL7C1: goto _LL7C3;_LL7C2: {struct Cyc_Absyn_Enum_e_struct*
_tmpF64=(struct Cyc_Absyn_Enum_e_struct*)_tmpF5F;if(_tmpF64->tag != 32)goto _LL7C4;}
_LL7C3: goto _LL7C5;_LL7C4: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpF65=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpF5F;if(_tmpF65->tag != 33)goto _LL7C6;}_LL7C5: return 1;_LL7C6: {struct Cyc_Absyn_Var_e_struct*
_tmpF66=(struct Cyc_Absyn_Var_e_struct*)_tmpF5F;if(_tmpF66->tag != 1)goto _LL7C8;
else{_tmpF67=_tmpF66->f1;_tmpF68=(void*)_tmpF66->f2;}}_LL7C7: {void*_tmpF8D=
_tmpF68;struct Cyc_Absyn_Vardecl*_tmpF90;_LL7E7: {struct Cyc_Absyn_Funname_b_struct*
_tmpF8E=(struct Cyc_Absyn_Funname_b_struct*)_tmpF8D;if(_tmpF8E->tag != 2)goto
_LL7E9;}_LL7E8: return 1;_LL7E9: {struct Cyc_Absyn_Global_b_struct*_tmpF8F=(struct
Cyc_Absyn_Global_b_struct*)_tmpF8D;if(_tmpF8F->tag != 1)goto _LL7EB;else{_tmpF90=
_tmpF8F->f1;}}_LL7EA: {void*_tmpF92=Cyc_Tcutil_compress(_tmpF90->type);_LL7F0: {
struct Cyc_Absyn_ArrayType_struct*_tmpF93=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF92;if(_tmpF93->tag != 9)goto _LL7F2;}_LL7F1: goto _LL7F3;_LL7F2: {struct Cyc_Absyn_FnType_struct*
_tmpF94=(struct Cyc_Absyn_FnType_struct*)_tmpF92;if(_tmpF94->tag != 10)goto _LL7F4;}
_LL7F3: return 1;_LL7F4:;_LL7F5: return var_okay;_LL7EF:;}_LL7EB: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpF91=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpF8D;if(_tmpF91->tag != 0)goto
_LL7ED;}_LL7EC: return 0;_LL7ED:;_LL7EE: return var_okay;_LL7E6:;}_LL7C8: {struct Cyc_Absyn_Conditional_e_struct*
_tmpF69=(struct Cyc_Absyn_Conditional_e_struct*)_tmpF5F;if(_tmpF69->tag != 6)goto
_LL7CA;else{_tmpF6A=_tmpF69->f1;_tmpF6B=_tmpF69->f2;_tmpF6C=_tmpF69->f3;}}_LL7C9:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpF6A) && Cyc_Tcutil_cnst_exp(te,0,_tmpF6B))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpF6C);_LL7CA: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpF6D=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpF5F;if(_tmpF6D->tag != 9)goto _LL7CC;
else{_tmpF6E=_tmpF6D->f1;_tmpF6F=_tmpF6D->f2;}}_LL7CB: return Cyc_Tcutil_cnst_exp(
te,0,_tmpF6E) && Cyc_Tcutil_cnst_exp(te,0,_tmpF6F);_LL7CC: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpF70=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpF5F;if(_tmpF70->tag != 13)
goto _LL7CE;else{_tmpF71=_tmpF70->f1;}}_LL7CD: _tmpF73=_tmpF71;goto _LL7CF;_LL7CE: {
struct Cyc_Absyn_Instantiate_e_struct*_tmpF72=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpF5F;if(_tmpF72->tag != 14)goto _LL7D0;else{_tmpF73=_tmpF72->f1;}}_LL7CF: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmpF73);_LL7D0: {struct Cyc_Absyn_Cast_e_struct*
_tmpF74=(struct Cyc_Absyn_Cast_e_struct*)_tmpF5F;if(_tmpF74->tag != 15)goto _LL7D2;
else{_tmpF75=(void*)_tmpF74->f1;_tmpF76=_tmpF74->f2;_tmpF77=_tmpF74->f4;if(
_tmpF77 != Cyc_Absyn_No_coercion)goto _LL7D2;}}_LL7D1: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpF76);_LL7D2: {struct Cyc_Absyn_Cast_e_struct*_tmpF78=(struct Cyc_Absyn_Cast_e_struct*)
_tmpF5F;if(_tmpF78->tag != 15)goto _LL7D4;else{_tmpF79=(void*)_tmpF78->f1;_tmpF7A=
_tmpF78->f2;}}_LL7D3: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpF7A);_LL7D4: {
struct Cyc_Absyn_Address_e_struct*_tmpF7B=(struct Cyc_Absyn_Address_e_struct*)
_tmpF5F;if(_tmpF7B->tag != 16)goto _LL7D6;else{_tmpF7C=_tmpF7B->f1;}}_LL7D5: return
Cyc_Tcutil_cnst_exp(te,1,_tmpF7C);_LL7D6: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpF7D=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpF5F;if(_tmpF7D->tag != 28)
goto _LL7D8;else{_tmpF7E=_tmpF7D->f2;_tmpF7F=_tmpF7D->f3;}}_LL7D7: return Cyc_Tcutil_cnst_exp(
te,0,_tmpF7E) && Cyc_Tcutil_cnst_exp(te,0,_tmpF7F);_LL7D8: {struct Cyc_Absyn_Array_e_struct*
_tmpF80=(struct Cyc_Absyn_Array_e_struct*)_tmpF5F;if(_tmpF80->tag != 27)goto _LL7DA;
else{_tmpF81=_tmpF80->f1;}}_LL7D9: _tmpF83=_tmpF81;goto _LL7DB;_LL7DA: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpF82=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpF5F;if(_tmpF82->tag != 30)goto
_LL7DC;else{_tmpF83=_tmpF82->f2;}}_LL7DB: _tmpF85=_tmpF83;goto _LL7DD;_LL7DC: {
struct Cyc_Absyn_Aggregate_e_struct*_tmpF84=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpF5F;if(_tmpF84->tag != 29)goto _LL7DE;else{_tmpF85=_tmpF84->f3;}}_LL7DD: for(0;
_tmpF85 != 0;_tmpF85=_tmpF85->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple18*)
_tmpF85->hd)).f2))return 0;}return 1;_LL7DE: {struct Cyc_Absyn_Primop_e_struct*
_tmpF86=(struct Cyc_Absyn_Primop_e_struct*)_tmpF5F;if(_tmpF86->tag != 3)goto _LL7E0;
else{_tmpF87=_tmpF86->f1;_tmpF88=_tmpF86->f2;}}_LL7DF: _tmpF8A=_tmpF88;goto _LL7E1;
_LL7E0: {struct Cyc_Absyn_Tuple_e_struct*_tmpF89=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpF5F;if(_tmpF89->tag != 25)goto _LL7E2;else{_tmpF8A=_tmpF89->f1;}}_LL7E1:
_tmpF8C=_tmpF8A;goto _LL7E3;_LL7E2: {struct Cyc_Absyn_Datatype_e_struct*_tmpF8B=(
struct Cyc_Absyn_Datatype_e_struct*)_tmpF5F;if(_tmpF8B->tag != 31)goto _LL7E4;else{
_tmpF8C=_tmpF8B->f1;}}_LL7E3: for(0;_tmpF8C != 0;_tmpF8C=_tmpF8C->tl){if(!Cyc_Tcutil_cnst_exp(
te,0,(struct Cyc_Absyn_Exp*)_tmpF8C->hd))return 0;}return 1;_LL7E4:;_LL7E5: return 0;
_LL7B9:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t);int Cyc_Tcutil_supports_default(void*t){void*_tmpF95=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpF9B;struct Cyc_Absyn_PtrAtts _tmpF9C;union Cyc_Absyn_Constraint*
_tmpF9D;union Cyc_Absyn_Constraint*_tmpF9E;struct Cyc_Absyn_ArrayInfo _tmpFA0;void*
_tmpFA1;struct Cyc_List_List*_tmpFA3;struct Cyc_Absyn_AggrInfo _tmpFA5;union Cyc_Absyn_AggrInfoU
_tmpFA6;struct Cyc_List_List*_tmpFA7;struct Cyc_List_List*_tmpFA9;_LL7F7: {struct
Cyc_Absyn_VoidType_struct*_tmpF96=(struct Cyc_Absyn_VoidType_struct*)_tmpF95;if(
_tmpF96->tag != 0)goto _LL7F9;}_LL7F8: goto _LL7FA;_LL7F9: {struct Cyc_Absyn_IntType_struct*
_tmpF97=(struct Cyc_Absyn_IntType_struct*)_tmpF95;if(_tmpF97->tag != 6)goto _LL7FB;}
_LL7FA: goto _LL7FC;_LL7FB: {struct Cyc_Absyn_FloatType_struct*_tmpF98=(struct Cyc_Absyn_FloatType_struct*)
_tmpF95;if(_tmpF98->tag != 7)goto _LL7FD;}_LL7FC: goto _LL7FE;_LL7FD: {struct Cyc_Absyn_DoubleType_struct*
_tmpF99=(struct Cyc_Absyn_DoubleType_struct*)_tmpF95;if(_tmpF99->tag != 8)goto
_LL7FF;}_LL7FE: return 1;_LL7FF: {struct Cyc_Absyn_PointerType_struct*_tmpF9A=(
struct Cyc_Absyn_PointerType_struct*)_tmpF95;if(_tmpF9A->tag != 5)goto _LL801;else{
_tmpF9B=_tmpF9A->f1;_tmpF9C=_tmpF9B.ptr_atts;_tmpF9D=_tmpF9C.nullable;_tmpF9E=
_tmpF9C.bounds;}}_LL800: {void*_tmpFAC=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpF9E);_LL810: {
struct Cyc_Absyn_DynEither_b_struct*_tmpFAD=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpFAC;if(_tmpFAD->tag != 0)goto _LL812;}_LL811: return 1;_LL812:;_LL813: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpF9D);_LL80F:;}
_LL801: {struct Cyc_Absyn_ArrayType_struct*_tmpF9F=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF95;if(_tmpF9F->tag != 9)goto _LL803;else{_tmpFA0=_tmpF9F->f1;_tmpFA1=_tmpFA0.elt_type;}}
_LL802: return Cyc_Tcutil_supports_default(_tmpFA1);_LL803: {struct Cyc_Absyn_TupleType_struct*
_tmpFA2=(struct Cyc_Absyn_TupleType_struct*)_tmpF95;if(_tmpFA2->tag != 11)goto
_LL805;else{_tmpFA3=_tmpFA2->f1;}}_LL804: for(0;_tmpFA3 != 0;_tmpFA3=_tmpFA3->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple11*)_tmpFA3->hd)).f2))return 0;}
return 1;_LL805: {struct Cyc_Absyn_AggrType_struct*_tmpFA4=(struct Cyc_Absyn_AggrType_struct*)
_tmpF95;if(_tmpFA4->tag != 12)goto _LL807;else{_tmpFA5=_tmpFA4->f1;_tmpFA6=_tmpFA5.aggr_info;
_tmpFA7=_tmpFA5.targs;}}_LL806: {struct Cyc_Absyn_Aggrdecl*_tmpFAE=Cyc_Absyn_get_known_aggrdecl(
_tmpFA6);if(_tmpFAE->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpFAE->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpFAE->tvs,_tmpFA7,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFAE->impl))->fields);}
_LL807: {struct Cyc_Absyn_AnonAggrType_struct*_tmpFA8=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF95;if(_tmpFA8->tag != 13)goto _LL809;else{_tmpFA9=_tmpFA8->f2;}}_LL808: return
Cyc_Tcutil_fields_support_default(0,0,_tmpFA9);_LL809: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpFAA=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpF95;if(_tmpFAA->tag != 15)goto
_LL80B;}_LL80A: goto _LL80C;_LL80B: {struct Cyc_Absyn_EnumType_struct*_tmpFAB=(
struct Cyc_Absyn_EnumType_struct*)_tmpF95;if(_tmpFAB->tag != 14)goto _LL80D;}_LL80C:
return 1;_LL80D:;_LL80E: return 0;_LL7F6:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);static int
Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,
struct Cyc_List_List*fs){struct _RegionHandle _tmpFAF=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpFAF;_push_region(rgn);{struct Cyc_List_List*_tmpFB0=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpFB0,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpFB1=0;_npop_handler(0);return _tmpFB1;}}}{
int _tmpFB2=1;_npop_handler(0);return _tmpFB2;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t);int Cyc_Tcutil_admits_zero(void*t){void*_tmpFB3=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpFB8;struct Cyc_Absyn_PtrAtts _tmpFB9;union Cyc_Absyn_Constraint*
_tmpFBA;union Cyc_Absyn_Constraint*_tmpFBB;_LL815: {struct Cyc_Absyn_IntType_struct*
_tmpFB4=(struct Cyc_Absyn_IntType_struct*)_tmpFB3;if(_tmpFB4->tag != 6)goto _LL817;}
_LL816: goto _LL818;_LL817: {struct Cyc_Absyn_FloatType_struct*_tmpFB5=(struct Cyc_Absyn_FloatType_struct*)
_tmpFB3;if(_tmpFB5->tag != 7)goto _LL819;}_LL818: goto _LL81A;_LL819: {struct Cyc_Absyn_DoubleType_struct*
_tmpFB6=(struct Cyc_Absyn_DoubleType_struct*)_tmpFB3;if(_tmpFB6->tag != 8)goto
_LL81B;}_LL81A: return 1;_LL81B: {struct Cyc_Absyn_PointerType_struct*_tmpFB7=(
struct Cyc_Absyn_PointerType_struct*)_tmpFB3;if(_tmpFB7->tag != 5)goto _LL81D;else{
_tmpFB8=_tmpFB7->f1;_tmpFB9=_tmpFB8.ptr_atts;_tmpFBA=_tmpFB9.nullable;_tmpFBB=
_tmpFB9.bounds;}}_LL81C: {void*_tmpFBC=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpFBB);_LL820: {
struct Cyc_Absyn_DynEither_b_struct*_tmpFBD=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpFBC;if(_tmpFBD->tag != 0)goto _LL822;}_LL821: return 0;_LL822:;_LL823: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFBA);_LL81F:;}
_LL81D:;_LL81E: return 0;_LL814:;}int Cyc_Tcutil_is_noreturn(void*t);int Cyc_Tcutil_is_noreturn(
void*t){{void*_tmpFBE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpFC0;void*
_tmpFC1;struct Cyc_Absyn_FnInfo _tmpFC3;struct Cyc_List_List*_tmpFC4;_LL825: {
struct Cyc_Absyn_PointerType_struct*_tmpFBF=(struct Cyc_Absyn_PointerType_struct*)
_tmpFBE;if(_tmpFBF->tag != 5)goto _LL827;else{_tmpFC0=_tmpFBF->f1;_tmpFC1=_tmpFC0.elt_typ;}}
_LL826: return Cyc_Tcutil_is_noreturn(_tmpFC1);_LL827: {struct Cyc_Absyn_FnType_struct*
_tmpFC2=(struct Cyc_Absyn_FnType_struct*)_tmpFBE;if(_tmpFC2->tag != 10)goto _LL829;
else{_tmpFC3=_tmpFC2->f1;_tmpFC4=_tmpFC3.attributes;}}_LL828: for(0;_tmpFC4 != 0;
_tmpFC4=_tmpFC4->tl){void*_tmpFC5=(void*)_tmpFC4->hd;_LL82C: {struct Cyc_Absyn_Noreturn_att_struct*
_tmpFC6=(struct Cyc_Absyn_Noreturn_att_struct*)_tmpFC5;if(_tmpFC6->tag != 4)goto
_LL82E;}_LL82D: return 1;_LL82E:;_LL82F: continue;_LL82B:;}goto _LL824;_LL829:;
_LL82A: goto _LL824;_LL824:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmpFC7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpFC9;struct Cyc_List_List*_tmpFCA;struct Cyc_List_List**_tmpFCB;_LL831: {struct
Cyc_Absyn_FnType_struct*_tmpFC8=(struct Cyc_Absyn_FnType_struct*)_tmpFC7;if(
_tmpFC8->tag != 10)goto _LL833;else{_tmpFC9=_tmpFC8->f1;_tmpFCA=_tmpFC9.attributes;
_tmpFCB=(struct Cyc_List_List**)&(_tmpFC8->f1).attributes;}}_LL832: {struct Cyc_List_List*
_tmpFCC=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp157C;*_tmpFCB=((_tmp157C=_cycalloc(sizeof(*_tmp157C)),((
_tmp157C->hd=(void*)atts->hd,((_tmp157C->tl=*_tmpFCB,_tmp157C))))));}else{struct
Cyc_List_List*_tmp157D;_tmpFCC=((_tmp157D=_cycalloc(sizeof(*_tmp157D)),((
_tmp157D->hd=(void*)atts->hd,((_tmp157D->tl=_tmpFCC,_tmp157D))))));}}return
_tmpFCC;}_LL833:;_LL834: {const char*_tmp1580;void*_tmp157F;(_tmp157F=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1580="transfer_fn_type_atts",
_tag_dyneither(_tmp1580,sizeof(char),22))),_tag_dyneither(_tmp157F,sizeof(void*),
0)));}_LL830:;}
