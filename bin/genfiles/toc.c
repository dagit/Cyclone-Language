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
tag;struct _dyneither_ptr f1;};int Cyc_vfprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct Cyc_Core_NewRegion Cyc_Core_new_dynregion();extern char Cyc_Core_Open_Region[
12];struct Cyc_Core_Open_Region_struct{char*tag;};extern char Cyc_Core_Free_Region[
12];struct Cyc_Core_Free_Region_struct{char*tag;};void Cyc_Core_free_dynregion(
struct _DynRegionHandle*);static void* arrcast(struct _dyneither_ptr dyn,
unsigned int bd,unsigned int sz);inline static void* arrcast(struct _dyneither_ptr dyn,
unsigned int bd,unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{unsigned char*
ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base)
 || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}struct Cyc_List_List{void*
hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(
struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct
_dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,
int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,
struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict
Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));struct Cyc_Dict_Dict
Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(
struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,
void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct _dyneither_ptr Cyc_Position_string_of_segment(struct
Cyc_Position_Segment*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct Cyc_Xarray_Xarray{struct _RegionHandle*r;struct _dyneither_ptr
elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);void*Cyc_Xarray_get(
struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(
struct _RegionHandle*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n
Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(
struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*
ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public
 = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct
Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;struct
Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz
 = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};enum 
Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
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
_union_Cnst_String_c String_c;};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,
char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst
Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(
enum Cyc_Absyn_Sign,long long);union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct
_dyneither_ptr);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus
 = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt
 = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,
Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor
 = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 
18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc
 = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
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
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_Skip_s_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;
struct Cyc_Position_Segment*loc;struct Cyc_List_List*non_local_preds;int try_depth;
void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;
struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct
Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern struct Cyc_Absyn_Unresolved_b_struct
Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;
struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*
body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*
fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct
_dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*
exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};
struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum Cyc_Absyn_Scope
sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*
fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};extern struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val;int
Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_sint_typ;void*Cyc_Absyn_exn_typ();extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct
_tuple0*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
enum Cyc_Absyn_Sign,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple0*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple0*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*
r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _dyneither_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU
info);extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;int Cyc_Tcutil_unify(void*,void*);
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dyneither,void**elt_type);struct _tuple9{struct Cyc_Absyn_Tqual f1;
void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple9*);struct _tuple10{unsigned int f1;
int f2;};struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int
Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL
 = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};struct
_union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;union Cyc_CfFlowInfo_RelnOp rop;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct
Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{int
tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int
f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{int tag;struct
Cyc_Absyn_Exp*f1;int f2;void*f3;};struct _union_FlowInfo_BottomFL{int tag;int val;};
struct _tuple12{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct
_union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*
notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;
void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct
Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple0*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=
0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**
tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_Set_Set**datatypes_so_far;
struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*
temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple13{
struct _tuple0*f1;struct _dyneither_ptr f2;};int Cyc_Toc_qvar_tag_cmp(struct _tuple13*
x,struct _tuple13*y);int Cyc_Toc_qvar_tag_cmp(struct _tuple13*x,struct _tuple13*y){
struct _tuple0*_tmp1;struct _dyneither_ptr _tmp2;struct _tuple13 _tmp0=*x;_tmp1=_tmp0.f1;
_tmp2=_tmp0.f2;{struct _tuple0*_tmp4;struct _dyneither_ptr _tmp5;struct _tuple13
_tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);if(
i != 0)return i;return Cyc_strcmp((struct _dyneither_ptr)_tmp2,(struct _dyneither_ptr)
_tmp5);};};}struct _tuple14{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};void*Cyc_Toc_aggrdecl_type(
struct _tuple0*q,void*(*type_maker)(struct _tuple0*));void*Cyc_Toc_aggrdecl_type(
struct _tuple0*q,void*(*type_maker)(struct _tuple0*)){struct _DynRegionHandle*_tmp7;
struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{struct
_DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);{struct
_tuple14**v=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*
_tmp8,q);if(v == 0){void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
struct _tuple14 _tmpC;void*_tmpD;struct _tuple14*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{
void*_tmpE=_tmpD;_npop_handler(0);return _tmpE;};}};_pop_dynregion(d);};}static
int Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=
0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_struct{
char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_struct{
char*tag;};static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap);static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpA33;void*_tmpA32;(_tmpA32=0,Cyc_fprintf(Cyc_stderr,((
_tmpA33="\n",_tag_dyneither(_tmpA33,sizeof(char),2))),_tag_dyneither(_tmpA32,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Unimplemented_struct
_tmpA36;struct Cyc_Toc_Toc_Unimplemented_struct*_tmpA35;(int)_throw((void*)((
_tmpA35=_cycalloc_atomic(sizeof(*_tmpA35)),((_tmpA35[0]=((_tmpA36.tag=Cyc_Toc_Toc_Unimplemented,
_tmpA36)),_tmpA35)))));};}static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA39;void*
_tmpA38;(_tmpA38=0,Cyc_fprintf(Cyc_stderr,((_tmpA39="\n",_tag_dyneither(_tmpA39,
sizeof(char),2))),_tag_dyneither(_tmpA38,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct _tmpA3C;struct Cyc_Toc_Toc_Impossible_struct*
_tmpA3B;(int)_throw((void*)((_tmpA3B=_cycalloc_atomic(sizeof(*_tmpA3B)),((
_tmpA3B[0]=((_tmpA3C.tag=Cyc_Toc_Toc_Impossible,_tmpA3C)),_tmpA3B)))));};}char
Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{char*tag;
};static char _tmp17[5]="curr";static struct _dyneither_ptr Cyc_Toc_curr_string={
_tmp17,_tmp17,_tmp17 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _tmp18[4]="tag";static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp18,
_tmp18,_tmp18 + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static char _tmp19[4]="val";static struct _dyneither_ptr Cyc_Toc_val_string={_tmp19,
_tmp19,_tmp19 + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;
static char _tmp1A[14]="_handler_cons";static struct _dyneither_ptr Cyc_Toc__handler_cons_string={
_tmp1A,_tmp1A,_tmp1A + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string;static char _tmp1B[8]="handler";static struct
_dyneither_ptr Cyc_Toc_handler_string={_tmp1B,_tmp1B,_tmp1B + 8};static struct
_dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp1C[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp1C,_tmp1C,_tmp1C + 14};
static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp1D[17]="_DynRegionHandle";static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={
_tmp1D,_tmp1D,_tmp1D + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=&
Cyc_Toc__DynRegionHandle_string;static char _tmp1E[16]="_DynRegionFrame";static
struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp1E,_tmp1E,_tmp1E + 16};
static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char
_tmp1F[7]="_throw";static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp1F,_tmp1F,
_tmp1F + 7};static struct _tuple0 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={
0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp21[7]="setjmp";static struct
_dyneither_ptr Cyc_Toc_setjmp_str={_tmp21,_tmp21,_tmp21 + 7};static struct _tuple0
Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)&
Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;
static char _tmp23[14]="_push_handler";static struct _dyneither_ptr Cyc_Toc__push_handler_str={
_tmp23,_tmp23,_tmp23 + 14};static struct _tuple0 Cyc_Toc__push_handler_pr={{.Loc_n={
4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1,& Cyc_Toc__push_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp25[13]="_pop_handler";static struct
_dyneither_ptr Cyc_Toc__pop_handler_str={_tmp25,_tmp25,_tmp25 + 13};static struct
_tuple0 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char
_tmp27[12]="_exn_thrown";static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={
_tmp27,_tmp27,_tmp27 + 12};static struct _tuple0 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,
0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp29[14]="_npop_handler";static struct
_dyneither_ptr Cyc_Toc__npop_handler_str={_tmp29,_tmp29,_tmp29 + 14};static struct
_tuple0 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char
_tmp2B[12]="_check_null";static struct _dyneither_ptr Cyc_Toc__check_null_str={
_tmp2B,_tmp2B,_tmp2B + 12};static struct _tuple0 Cyc_Toc__check_null_pr={{.Loc_n={4,
0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={
1,& Cyc_Toc__check_null_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp2D[28]="_check_known_subscript_null";static
struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2D,_tmp2D,
_tmp2D + 28};static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={
4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp2F[31]="_check_known_subscript_notnull";static struct _dyneither_ptr
Cyc_Toc__check_known_subscript_notnull_str={_tmp2F,_tmp2F,_tmp2F + 31};static
struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)&
Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static char _tmp31[27]="_check_dyneither_subscript";static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp31,_tmp31,_tmp31 + 27};static struct _tuple0 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={
4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp33[15]="_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={
_tmp33,_tmp33,_tmp33 + 15};static struct _tuple0 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={
4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=&
Cyc_Toc__dyneither_ptr_ev;static char _tmp35[15]="_tag_dyneither";static struct
_dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp35,_tmp35,_tmp35 + 15};static struct
_tuple0 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static
char _tmp37[20]="_init_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp37,_tmp37,_tmp37 + 20};static struct _tuple0 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={
4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={
1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=&
Cyc_Toc__init_dyneither_ptr_ev;static char _tmp39[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp39,_tmp39,_tmp39
+ 21};static struct _tuple0 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={
0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp3B[20]="_get_dyneither_size";static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={
_tmp3B,_tmp3B,_tmp3B + 20};static struct _tuple0 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={
1,& Cyc_Toc__get_dyneither_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=&
Cyc_Toc__get_dyneither_size_ev;static char _tmp3D[19]="_get_zero_arr_size";static
struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3D,_tmp3D,_tmp3D + 19};
static struct _tuple0 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp3F[24]="_get_zero_arr_size_char";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={
_tmp3F,_tmp3F,_tmp3F + 24};static struct _tuple0 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;
static char _tmp41[25]="_get_zero_arr_size_short";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={
_tmp41,_tmp41,_tmp41 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;
static char _tmp43[23]="_get_zero_arr_size_int";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={
_tmp43,_tmp43,_tmp43 + 23};static struct _tuple0 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_int_re={
1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=&
Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp45[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp45,_tmp45,
_tmp45 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},&
Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_float_re={
1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=&
Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp47[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp47,_tmp47,
_tmp47 + 26};static struct _tuple0 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,
0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;
static char _tmp49[30]="_get_zero_arr_size_longdouble";static struct _dyneither_ptr
Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp49,_tmp49,_tmp49 + 30};static
struct _tuple0 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,&
Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=&
Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp4B[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp4B,_tmp4B,
_tmp4B + 28};static struct _tuple0 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={
4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;
static char _tmp4D[20]="_dyneither_ptr_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp4D,_tmp4D,_tmp4D + 20};static struct _tuple0 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={
4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={
1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=&
Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp4F[15]="_zero_arr_plus";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp4F,_tmp4F,_tmp4F + 15};static
struct _tuple0 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static
char _tmp51[20]="_zero_arr_plus_char";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={
_tmp51,_tmp51,_tmp51 + 20};static struct _tuple0 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_char_re={
1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=&
Cyc_Toc__zero_arr_plus_char_ev;static char _tmp53[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp53,_tmp53,_tmp53
+ 21};static struct _tuple0 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;
static char _tmp55[19]="_zero_arr_plus_int";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={
_tmp55,_tmp55,_tmp55 + 19};static struct _tuple0 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_int_re={
1,& Cyc_Toc__zero_arr_plus_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=&
Cyc_Toc__zero_arr_plus_int_ev;static char _tmp57[21]="_zero_arr_plus_float";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp57,_tmp57,_tmp57 + 21};
static struct _tuple0 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;
static char _tmp59[22]="_zero_arr_plus_double";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={
_tmp59,_tmp59,_tmp59 + 22};static struct _tuple0 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_double_re={
1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=&
Cyc_Toc__zero_arr_plus_double_ev;static char _tmp5B[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp5B,_tmp5B,
_tmp5B + 26};static struct _tuple0 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,
0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;
static char _tmp5D[24]="_zero_arr_plus_voidstar";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={
_tmp5D,_tmp5D,_tmp5D + 24};static struct _tuple0 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;
static char _tmp5F[28]="_dyneither_ptr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={
_tmp5F,_tmp5F,_tmp5F + 28};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={
4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp61[23]="_zero_arr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp61,_tmp61,_tmp61 + 23};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={
1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=&
Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp63[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp63,_tmp63,
_tmp63 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;
static char _tmp65[29]="_zero_arr_inplace_plus_short";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp65,_tmp65,_tmp65 + 29};static struct
_tuple0 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,&
Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=&
Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp67[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp67,_tmp67,
_tmp67 + 27};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,
0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;
static char _tmp69[29]="_zero_arr_inplace_plus_float";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp69,_tmp69,_tmp69 + 29};static struct
_tuple0 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,&
Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=&
Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp6B[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp6B,
_tmp6B,_tmp6B + 30};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;
static char _tmp6D[34]="_zero_arr_inplace_plus_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp6D,_tmp6D,
_tmp6D + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;
static char _tmp6F[32]="_zero_arr_inplace_plus_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp6F,_tmp6F,_tmp6F + 
32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},&
Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;
static char _tmp71[33]="_dyneither_ptr_inplace_plus_post";static struct
_dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp71,_tmp71,_tmp71
+ 33};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,
0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;
static char _tmp73[28]="_zero_arr_inplace_plus_post";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={
_tmp73,_tmp73,_tmp73 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp75[33]="_zero_arr_inplace_plus_post_char";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp75,_tmp75,_tmp75
+ 33};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,
0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;
static char _tmp77[34]="_zero_arr_inplace_plus_post_short";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp77,_tmp77,
_tmp77 + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;
static char _tmp79[32]="_zero_arr_inplace_plus_post_int";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp79,_tmp79,_tmp79 + 
32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},&
Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;
static char _tmp7B[34]="_zero_arr_inplace_plus_post_float";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp7B,_tmp7B,
_tmp7B + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;
static char _tmp7D[35]="_zero_arr_inplace_plus_post_double";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp7D,_tmp7D,
_tmp7D + 35};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;
static char _tmp7F[39]="_zero_arr_inplace_plus_post_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp7F,_tmp7F,
_tmp7F + 39};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;
static char _tmp81[37]="_zero_arr_inplace_plus_post_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp81,_tmp81,
_tmp81 + 37};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={
4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;
static char _tmp83[10]="_cycalloc";static struct _dyneither_ptr Cyc_Toc__cycalloc_str={
_tmp83,_tmp83,_tmp83 + 10};static struct _tuple0 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct
Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char
_tmp85[11]="_cyccalloc";static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp85,
_tmp85,_tmp85 + 11};static struct _tuple0 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct
Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp87[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp87,_tmp87,_tmp87 + 17};
static struct _tuple0 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmp89[18]="_cyccalloc_atomic";static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmp89,_tmp89,_tmp89 + 18};static struct _tuple0 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={
4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=&
Cyc_Toc__cyccalloc_atomic_ev;static char _tmp8B[15]="_region_malloc";static struct
_dyneither_ptr Cyc_Toc__region_malloc_str={_tmp8B,_tmp8B,_tmp8B + 15};static struct
_tuple0 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={
0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static
char _tmp8D[15]="_region_calloc";static struct _dyneither_ptr Cyc_Toc__region_calloc_str={
_tmp8D,_tmp8D,_tmp8D + 15};static struct _tuple0 Cyc_Toc__region_calloc_pr={{.Loc_n={
4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=&
Cyc_Toc__region_calloc_ev;static char _tmp8F[13]="_check_times";static struct
_dyneither_ptr Cyc_Toc__check_times_str={_tmp8F,_tmp8F,_tmp8F + 13};static struct
_tuple0 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={
0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char
_tmp91[12]="_new_region";static struct _dyneither_ptr Cyc_Toc__new_region_str={
_tmp91,_tmp91,_tmp91 + 12};static struct _tuple0 Cyc_Toc__new_region_pr={{.Loc_n={4,
0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=&
Cyc_Toc__new_region_ev;static char _tmp93[13]="_push_region";static struct
_dyneither_ptr Cyc_Toc__push_region_str={_tmp93,_tmp93,_tmp93 + 13};static struct
_tuple0 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={
0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char
_tmp95[12]="_pop_region";static struct _dyneither_ptr Cyc_Toc__pop_region_str={
_tmp95,_tmp95,_tmp95 + 12};static struct _tuple0 Cyc_Toc__pop_region_pr={{.Loc_n={4,
0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=&
Cyc_Toc__pop_region_ev;static char _tmp97[16]="_open_dynregion";static struct
_dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp97,_tmp97,_tmp97 + 16};static
struct _tuple0 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={
0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static
char _tmp99[16]="_push_dynregion";static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={
_tmp99,_tmp99,_tmp99 + 16};static struct _tuple0 Cyc_Toc__push_dynregion_pr={{.Loc_n={
4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={
1,& Cyc_Toc__push_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=&
Cyc_Toc__push_dynregion_ev;static char _tmp9B[15]="_pop_dynregion";static struct
_dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp9B,_tmp9B,_tmp9B + 15};static struct
_tuple0 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={
0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static
char _tmp9D[14]="_reset_region";static struct _dyneither_ptr Cyc_Toc__reset_region_str={
_tmp9D,_tmp9D,_tmp9D + 14};static struct _tuple0 Cyc_Toc__reset_region_pr={{.Loc_n={
4,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={
1,& Cyc_Toc__reset_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static char _tmp9F[19]="_throw_arraybounds";static struct
_dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp9F,_tmp9F,_tmp9F + 19};static
struct _tuple0 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={
0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmpA1[29]="_dyneither_ptr_decrease_size";static struct _dyneither_ptr
Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpA1,_tmpA1,_tmpA1 + 29};static struct
_tuple0 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA3[11]="_swap_word";static
struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpA3,_tmpA3,_tmpA3 + 11};static
struct _tuple0 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct
Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA5[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA5,_tmpA5,_tmpA5 + 16};
static struct _tuple0 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={
0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static
struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={12,{{.UnknownAggr={1,{
Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};static void*Cyc_Toc_dyneither_ptr_typ=(
void*)& Cyc_Toc_dyneither_ptr_typ_v;static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,
0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl();static struct Cyc_Absyn_Stmt*
Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(
skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpA3D;skip_stmt_opt=((_tmpA3D=
_cycalloc(sizeof(*_tmpA3D)),((_tmpA3D[0]=Cyc_Absyn_skip_stmt(0),_tmpA3D))));}
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e);static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*
e){struct Cyc_Absyn_Cast_e_struct _tmpA40;struct Cyc_Absyn_Cast_e_struct*_tmpA3F;
return(void*)((_tmpA3F=_cycalloc(sizeof(*_tmpA3F)),((_tmpA3F[0]=((_tmpA40.tag=15,((
_tmpA40.f1=(void*)t,((_tmpA40.f2=e,((_tmpA40.f3=0,((_tmpA40.f4=Cyc_Absyn_No_coercion,
_tmpA40)))))))))),_tmpA3F))));}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*
e);static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA43;struct Cyc_Absyn_Deref_e_struct*_tmpA42;return(void*)((_tmpA42=_cycalloc(
sizeof(*_tmpA42)),((_tmpA42[0]=((_tmpA43.tag=21,((_tmpA43.f1=e,_tmpA43)))),
_tmpA42))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA46;struct Cyc_Absyn_Subscript_e_struct*
_tmpA45;return(void*)((_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45[0]=((
_tmpA46.tag=24,((_tmpA46.f1=e1,((_tmpA46.f2=e2,_tmpA46)))))),_tmpA45))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct _tmpA49;struct Cyc_Absyn_StmtExp_e_struct*
_tmpA48;return(void*)((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48[0]=((
_tmpA49.tag=37,((_tmpA49.f1=s,_tmpA49)))),_tmpA48))));}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t);static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA4C;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA4B;return(void*)((_tmpA4B=
_cycalloc(sizeof(*_tmpA4B)),((_tmpA4B[0]=((_tmpA4C.tag=18,((_tmpA4C.f1=(void*)t,
_tmpA4C)))),_tmpA4B))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es);static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA4F;struct Cyc_Absyn_FnCall_e_struct*
_tmpA4E;return(void*)((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E[0]=((
_tmpA4F.tag=11,((_tmpA4F.f1=e,((_tmpA4F.f2=es,((_tmpA4F.f3=0,_tmpA4F)))))))),
_tmpA4E))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_exp_stmt_r(
struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct _tmpA52;struct Cyc_Absyn_Exp_s_struct*
_tmpA51;return(void*)((_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51[0]=((
_tmpA52.tag=1,((_tmpA52.f1=e,_tmpA52)))),_tmpA51))));}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2);static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Seq_s_struct
_tmpA55;struct Cyc_Absyn_Seq_s_struct*_tmpA54;return(void*)((_tmpA54=_cycalloc(
sizeof(*_tmpA54)),((_tmpA54[0]=((_tmpA55.tag=2,((_tmpA55.f1=s1,((_tmpA55.f2=s2,
_tmpA55)))))),_tmpA54))));}static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3);static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA58;struct Cyc_Absyn_Conditional_e_struct*_tmpA57;return(void*)((_tmpA57=
_cycalloc(sizeof(*_tmpA57)),((_tmpA57[0]=((_tmpA58.tag=6,((_tmpA58.f1=e1,((
_tmpA58.f2=e2,((_tmpA58.f3=e3,_tmpA58)))))))),_tmpA57))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpA5B;struct Cyc_Absyn_AggrMember_e_struct*_tmpA5A;return(void*)((_tmpA5A=
_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A[0]=((_tmpA5B.tag=22,((_tmpA5B.f1=e,((
_tmpA5B.f2=n,((_tmpA5B.f3=0,((_tmpA5B.f4=0,_tmpA5B)))))))))),_tmpA5A))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpA5E;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA5D;return(void*)((_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D[0]=((
_tmpA5E.tag=23,((_tmpA5E.f1=e,((_tmpA5E.f2=n,((_tmpA5E.f3=0,((_tmpA5E.f4=0,
_tmpA5E)))))))))),_tmpA5D))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds);static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA61;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA60;return(void*)((_tmpA60=_cycalloc(
sizeof(*_tmpA60)),((_tmpA60[0]=((_tmpA61.tag=36,((_tmpA61.f1=tdopt,((_tmpA61.f2=
ds,_tmpA61)))))),_tmpA60))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA64;
struct Cyc_Absyn_Goto_s_struct*_tmpA63;return(void*)((_tmpA63=_cycalloc(sizeof(*
_tmpA63)),((_tmpA63[0]=((_tmpA64.tag=8,((_tmpA64.f1=v,((_tmpA64.f2=s,_tmpA64)))))),
_tmpA63))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
Cyc_Absyn_Signed,0}}}};struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;
struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;
struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*
fvoidstar;};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,&
Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,&
Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,&
Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,&
Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,&
Cyc_Toc__get_zero_arr_size_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t);static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmpC4=
Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmpC6;int _tmpC9;_LL1: {struct Cyc_Absyn_IntType_struct*
_tmpC5=(struct Cyc_Absyn_IntType_struct*)_tmpC4;if(_tmpC5->tag != 6)goto _LL3;else{
_tmpC6=_tmpC5->f2;}}_LL2: switch(_tmpC6){case Cyc_Absyn_Char_sz: _LLB: function=fS->fchar;
break;case Cyc_Absyn_Short_sz: _LLC: function=fS->fshort;break;case Cyc_Absyn_Int_sz:
_LLD: function=fS->fint;break;default: _LLE: {struct Cyc_Core_Impossible_struct
_tmpA6A;const char*_tmpA69;struct Cyc_Core_Impossible_struct*_tmpA68;(int)_throw((
void*)((_tmpA68=_cycalloc(sizeof(*_tmpA68)),((_tmpA68[0]=((_tmpA6A.tag=Cyc_Core_Impossible,((
_tmpA6A.f1=((_tmpA69="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA69,sizeof(char),44))),_tmpA6A)))),_tmpA68)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC7=(struct Cyc_Absyn_FloatType_struct*)
_tmpC4;if(_tmpC7->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpC8=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC4;if(_tmpC8->tag != 8)goto _LL7;else{_tmpC9=_tmpC8->f1;}}_LL6: switch(_tmpC9){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCA=(struct Cyc_Absyn_PointerType_struct*)
_tmpC4;if(_tmpCA->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpA77;const char*_tmpA76;void*_tmpA75[1];
struct Cyc_String_pa_struct _tmpA74;struct Cyc_Core_Impossible_struct*_tmpA73;(int)
_throw((void*)((_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73[0]=((_tmpA77.tag=
Cyc_Core_Impossible,((_tmpA77.f1=(struct _dyneither_ptr)((_tmpA74.tag=0,((_tmpA74.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA75[
0]=& _tmpA74,Cyc_aprintf(((_tmpA76="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpA76,sizeof(char),67))),_tag_dyneither(_tmpA75,sizeof(void*),1)))))))),
_tmpA77)))),_tmpA73)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,(void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v);}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr){void*_tmpD3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD5;void*_tmpD6;_LL14: {
struct Cyc_Absyn_PointerType_struct*_tmpD4=(struct Cyc_Absyn_PointerType_struct*)
_tmpD3;if(_tmpD4->tag != 5)goto _LL16;else{_tmpD5=_tmpD4->f1;_tmpD6=_tmpD5.elt_typ;}}
_LL15: return Cyc_Toc_getFunctionType(fS,_tmpD6);_LL16:;_LL17: {struct Cyc_Core_Impossible_struct
_tmpA7D;const char*_tmpA7C;struct Cyc_Core_Impossible_struct*_tmpA7B;(int)_throw((
void*)((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B[0]=((_tmpA7D.tag=Cyc_Core_Impossible,((
_tmpA7D.f1=((_tmpA7C="impossible type (not pointer)",_tag_dyneither(_tmpA7C,
sizeof(char),30))),_tmpA7D)))),_tmpA7B)))));}_LL13:;}struct _tuple15{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e);
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*_tmpDA=e->r;union Cyc_Absyn_Cnst
_tmpDC;struct _tuple3 _tmpDD;char _tmpDE;union Cyc_Absyn_Cnst _tmpE0;struct _tuple4
_tmpE1;short _tmpE2;union Cyc_Absyn_Cnst _tmpE4;struct _tuple5 _tmpE5;int _tmpE6;union
Cyc_Absyn_Cnst _tmpE8;struct _tuple6 _tmpE9;long long _tmpEA;union Cyc_Absyn_Cnst
_tmpEC;int _tmpED;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_List_List*_tmpF1;struct Cyc_List_List*
_tmpF3;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*
_tmpF9;_LL19: {struct Cyc_Absyn_Const_e_struct*_tmpDB=(struct Cyc_Absyn_Const_e_struct*)
_tmpDA;if(_tmpDB->tag != 0)goto _LL1B;else{_tmpDC=_tmpDB->f1;if((_tmpDC.Char_c).tag
!= 2)goto _LL1B;_tmpDD=(struct _tuple3)(_tmpDC.Char_c).val;_tmpDE=_tmpDD.f2;}}
_LL1A: return _tmpDE == '\000';_LL1B: {struct Cyc_Absyn_Const_e_struct*_tmpDF=(
struct Cyc_Absyn_Const_e_struct*)_tmpDA;if(_tmpDF->tag != 0)goto _LL1D;else{_tmpE0=
_tmpDF->f1;if((_tmpE0.Short_c).tag != 3)goto _LL1D;_tmpE1=(struct _tuple4)(_tmpE0.Short_c).val;
_tmpE2=_tmpE1.f2;}}_LL1C: return _tmpE2 == 0;_LL1D: {struct Cyc_Absyn_Const_e_struct*
_tmpE3=(struct Cyc_Absyn_Const_e_struct*)_tmpDA;if(_tmpE3->tag != 0)goto _LL1F;
else{_tmpE4=_tmpE3->f1;if((_tmpE4.Int_c).tag != 4)goto _LL1F;_tmpE5=(struct _tuple5)(
_tmpE4.Int_c).val;_tmpE6=_tmpE5.f2;}}_LL1E: return _tmpE6 == 0;_LL1F: {struct Cyc_Absyn_Const_e_struct*
_tmpE7=(struct Cyc_Absyn_Const_e_struct*)_tmpDA;if(_tmpE7->tag != 0)goto _LL21;
else{_tmpE8=_tmpE7->f1;if((_tmpE8.LongLong_c).tag != 5)goto _LL21;_tmpE9=(struct
_tuple6)(_tmpE8.LongLong_c).val;_tmpEA=_tmpE9.f2;}}_LL20: return _tmpEA == 0;_LL21: {
struct Cyc_Absyn_Const_e_struct*_tmpEB=(struct Cyc_Absyn_Const_e_struct*)_tmpDA;
if(_tmpEB->tag != 0)goto _LL23;else{_tmpEC=_tmpEB->f1;if((_tmpEC.Null_c).tag != 1)
goto _LL23;_tmpED=(int)(_tmpEC.Null_c).val;}}_LL22: return 1;_LL23: {struct Cyc_Absyn_Cast_e_struct*
_tmpEE=(struct Cyc_Absyn_Cast_e_struct*)_tmpDA;if(_tmpEE->tag != 15)goto _LL25;
else{_tmpEF=_tmpEE->f2;}}_LL24: return Cyc_Toc_is_zero(_tmpEF);_LL25: {struct Cyc_Absyn_Tuple_e_struct*
_tmpF0=(struct Cyc_Absyn_Tuple_e_struct*)_tmpDA;if(_tmpF0->tag != 25)goto _LL27;
else{_tmpF1=_tmpF0->f1;}}_LL26: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpF1);_LL27: {struct
Cyc_Absyn_Array_e_struct*_tmpF2=(struct Cyc_Absyn_Array_e_struct*)_tmpDA;if(
_tmpF2->tag != 27)goto _LL29;else{_tmpF3=_tmpF2->f1;}}_LL28: _tmpF5=_tmpF3;goto
_LL2A;_LL29: {struct Cyc_Absyn_Aggregate_e_struct*_tmpF4=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpDA;if(_tmpF4->tag != 29)goto _LL2B;else{_tmpF5=_tmpF4->f3;}}_LL2A: _tmpF7=
_tmpF5;goto _LL2C;_LL2B: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpF6=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpDA;if(_tmpF6->tag != 26)goto _LL2D;else{_tmpF7=_tmpF6->f2;}}_LL2C: _tmpF9=
_tmpF7;goto _LL2E;_LL2D: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpF8=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmpDA;if(_tmpF8->tag != 36)goto _LL2F;else{
_tmpF9=_tmpF8->f2;}}_LL2E: for(0;_tmpF9 != 0;_tmpF9=_tmpF9->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple15*)_tmpF9->hd)).f2))return 0;}return 1;_LL2F:;_LL30: return 0;_LL18:;}
static int Cyc_Toc_is_nullable(void*t);static int Cyc_Toc_is_nullable(void*t){void*
_tmpFA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpFC;struct Cyc_Absyn_PtrAtts
_tmpFD;union Cyc_Absyn_Constraint*_tmpFE;_LL32: {struct Cyc_Absyn_PointerType_struct*
_tmpFB=(struct Cyc_Absyn_PointerType_struct*)_tmpFA;if(_tmpFB->tag != 5)goto _LL34;
else{_tmpFC=_tmpFB->f1;_tmpFD=_tmpFC.ptr_atts;_tmpFE=_tmpFD.nullable;}}_LL33:
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFE);
_LL34:;_LL35: {const char*_tmpA80;void*_tmpA7F;(_tmpA7F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA80="is_nullable",
_tag_dyneither(_tmpA80,sizeof(char),12))),_tag_dyneither(_tmpA7F,sizeof(void*),0)));}
_LL31:;}static char _tmp10D[1]="";static char _tmp10E[8]="*bogus*";static struct
_tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct _dyneither_ptr tag);
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp102;struct Cyc_Dict_Dict*_tmp103;
struct Cyc_Toc_TocState _tmp101=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp102=_tmp101.dyn;_tmp103=_tmp101.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp10E,_tmp10E,_tmp10E + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
4,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp10D,_tmp10D,
_tmp10D + 1}};{struct _tuple13 _tmpA81;pair=((_tmpA81.f1=x,((_tmpA81.f2=tag,_tmpA81))));}{
struct _DynRegionFrame _tmp105;struct _RegionHandle*d=_open_dynregion(& _tmp105,
_tmp102);{struct _tuple0**_tmp106=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp103,(struct _tuple13*)& pair);if(
_tmp106 != 0){struct _tuple0*_tmp107=*_tmp106;_npop_handler(0);return _tmp107;}{
struct _tuple13*_tmpA82;struct _tuple13*_tmp108=(_tmpA82=_cycalloc(sizeof(*_tmpA82)),((
_tmpA82->f1=x,((_tmpA82->f2=tag,_tmpA82)))));struct _dyneither_ptr*_tmpA85;struct
_tuple0*_tmpA84;struct _tuple0*res=(_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->f1=(*
x).f1,((_tmpA84->f2=((_tmpA85=_cycalloc(sizeof(*_tmpA85)),((_tmpA85[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpA85)))),_tmpA84)))));*_tmp103=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple13*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp103,(
struct _tuple13*)_tmp108,res);{struct _tuple0*_tmp109=res;_npop_handler(0);return
_tmp109;};};};_pop_dynregion(d);};};}struct _tuple16{void*f1;struct Cyc_List_List*
f2;};static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0);static void*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){struct _DynRegionHandle*_tmp110;struct Cyc_List_List**
_tmp111;struct Cyc_Toc_TocState _tmp10F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp110=_tmp10F.dyn;_tmp111=_tmp10F.tuple_types;{struct _DynRegionFrame _tmp112;
struct _RegionHandle*d=_open_dynregion(& _tmp112,_tmp110);{struct Cyc_List_List*
_tmp113=*_tmp111;for(0;_tmp113 != 0;_tmp113=_tmp113->tl){struct _tuple16 _tmp115;
void*_tmp116;struct Cyc_List_List*_tmp117;struct _tuple16*_tmp114=(struct _tuple16*)
_tmp113->hd;_tmp115=*_tmp114;_tmp116=_tmp115.f1;_tmp117=_tmp115.f2;{struct Cyc_List_List*
_tmp118=tqs0;for(0;_tmp118 != 0  && _tmp117 != 0;(_tmp118=_tmp118->tl,_tmp117=
_tmp117->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple9*)_tmp118->hd)).f2,(void*)
_tmp117->hd))break;}if(_tmp118 == 0  && _tmp117 == 0){void*_tmp119=_tmp116;
_npop_handler(0);return _tmp119;}};}}{struct Cyc_Int_pa_struct _tmpA8D;void*_tmpA8C[
1];const char*_tmpA8B;struct _dyneither_ptr*_tmpA8A;struct _dyneither_ptr*xname=(
_tmpA8A=_cycalloc(sizeof(*_tmpA8A)),((_tmpA8A[0]=(struct _dyneither_ptr)((_tmpA8D.tag=
1,((_tmpA8D.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA8C[0]=& _tmpA8D,
Cyc_aprintf(((_tmpA8B="_tuple%d",_tag_dyneither(_tmpA8B,sizeof(char),9))),
_tag_dyneither(_tmpA8C,sizeof(void*),1)))))))),_tmpA8A)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp11A=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpA90;struct Cyc_List_List*
_tmpA8F;_tmp11A=((_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F->hd=((_tmpA90=
_cycalloc(sizeof(*_tmpA90)),((_tmpA90->name=Cyc_Absyn_fieldname(i),((_tmpA90->tq=
Cyc_Toc_mt_tq,((_tmpA90->type=(void*)ts2->hd,((_tmpA90->width=0,((_tmpA90->attributes=
0,_tmpA90)))))))))))),((_tmpA8F->tl=_tmp11A,_tmpA8F))))));}}_tmp11A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11A);{struct Cyc_Absyn_AggrdeclImpl*
_tmpA95;struct _tuple0*_tmpA94;struct Cyc_Absyn_Aggrdecl*_tmpA93;struct Cyc_Absyn_Aggrdecl*
_tmp11D=(_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93->kind=Cyc_Absyn_StructA,((
_tmpA93->sc=Cyc_Absyn_Public,((_tmpA93->name=((_tmpA94=_cycalloc(sizeof(*_tmpA94)),((
_tmpA94->f1=Cyc_Absyn_Rel_n(0),((_tmpA94->f2=xname,_tmpA94)))))),((_tmpA93->tvs=
0,((_tmpA93->impl=((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95->exist_vars=0,((
_tmpA95->rgn_po=0,((_tmpA95->fields=_tmp11A,((_tmpA95->tagged=0,_tmpA95)))))))))),((
_tmpA93->attributes=0,_tmpA93)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpA9B;struct Cyc_Absyn_Aggr_d_struct _tmpA9A;struct Cyc_List_List*_tmpA99;Cyc_Toc_result_decls=((
_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99->hd=Cyc_Absyn_new_decl((void*)((
_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((_tmpA9B[0]=((_tmpA9A.tag=6,((_tmpA9A.f1=
_tmp11D,_tmpA9A)))),_tmpA9B)))),0),((_tmpA99->tl=Cyc_Toc_result_decls,_tmpA99))))));}{
struct _tuple16*_tmpA9E;struct Cyc_List_List*_tmpA9D;*_tmp111=((_tmpA9D=
_region_malloc(d,sizeof(*_tmpA9D)),((_tmpA9D->hd=((_tmpA9E=_region_malloc(d,
sizeof(*_tmpA9E)),((_tmpA9E->f1=x,((_tmpA9E->f2=ts,_tmpA9E)))))),((_tmpA9D->tl=*
_tmp111,_tmpA9D))))));}{void*_tmp123=x;_npop_handler(0);return _tmp123;};};};;
_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var();struct _tuple0*Cyc_Toc_temp_var(){
int _tmp12B=Cyc_Toc_temp_var_counter ++;struct _dyneither_ptr*_tmpAAB;const char*
_tmpAAA;void*_tmpAA9[1];struct Cyc_Int_pa_struct _tmpAA8;struct _tuple0*_tmpAA7;
struct _tuple0*res=(_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->f1=Cyc_Absyn_Loc_n,((
_tmpAA7->f2=((_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB[0]=(struct
_dyneither_ptr)((_tmpAA8.tag=1,((_tmpAA8.f1=(unsigned int)_tmp12B,((_tmpAA9[0]=&
_tmpAA8,Cyc_aprintf(((_tmpAAA="_tmp%X",_tag_dyneither(_tmpAAA,sizeof(char),7))),
_tag_dyneither(_tmpAA9,sizeof(void*),1)))))))),_tmpAAB)))),_tmpAA7)))));return
res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label();static struct _dyneither_ptr*
Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp132;struct Cyc_Xarray_Xarray*
_tmp133;struct Cyc_Toc_TocState _tmp131=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp132=_tmp131.dyn;_tmp133=_tmp131.temp_labels;{struct _DynRegionFrame _tmp134;
struct _RegionHandle*d=_open_dynregion(& _tmp134,_tmp132);{int _tmp135=Cyc_Toc_fresh_label_counter
++;if(_tmp135 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp133)){
struct _dyneither_ptr*_tmp136=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp133,_tmp135);_npop_handler(0);return _tmp136;}{struct Cyc_Int_pa_struct
_tmpAB3;void*_tmpAB2[1];const char*_tmpAB1;struct _dyneither_ptr*_tmpAB0;struct
_dyneither_ptr*res=(_tmpAB0=_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0[0]=(struct
_dyneither_ptr)((_tmpAB3.tag=1,((_tmpAB3.f1=(unsigned int)_tmp135,((_tmpAB2[0]=&
_tmpAB3,Cyc_aprintf(((_tmpAB1="_LL%X",_tag_dyneither(_tmpAB1,sizeof(char),6))),
_tag_dyneither(_tmpAB2,sizeof(void*),1)))))))),_tmpAB0)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp133,res)!= _tmp135){const char*
_tmpAB6;void*_tmpAB5;(_tmpAB5=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB6="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpAB6,sizeof(char),43))),_tag_dyneither(_tmpAB5,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp139=res;_npop_handler(0);return _tmp139;};};};
_pop_dynregion(d);};}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple0*name);static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple0*name){int ans=0;struct Cyc_List_List*_tmp13E=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp13E))->hd)->name)
!= 0){++ ans;_tmp13E=_tmp13E->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple7*Cyc_Toc_arg_to_c(struct
_tuple7*a);static struct _tuple7*Cyc_Toc_arg_to_c(struct _tuple7*a){struct Cyc_Core_Opt*
_tmp140;struct Cyc_Absyn_Tqual _tmp141;void*_tmp142;struct _tuple7 _tmp13F=*a;
_tmp140=_tmp13F.f1;_tmp141=_tmp13F.f2;_tmp142=_tmp13F.f3;{struct _tuple7*_tmpAB7;
return(_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->f1=_tmp140,((_tmpAB7->f2=
_tmp141,((_tmpAB7->f3=Cyc_Toc_typ_to_c(_tmp142),_tmpAB7)))))));};}static struct
_tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x);static struct _tuple9*Cyc_Toc_typ_to_c_f(
struct _tuple9*x){struct Cyc_Absyn_Tqual _tmp145;void*_tmp146;struct _tuple9 _tmp144=*
x;_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;{struct _tuple9*_tmpAB8;return(_tmpAB8=
_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8->f1=_tmp145,((_tmpAB8->f2=Cyc_Toc_typ_to_c(
_tmp146),_tmpAB8)))));};}static void*Cyc_Toc_typ_to_c_array(void*t);static void*
Cyc_Toc_typ_to_c_array(void*t){void*_tmp148=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo
_tmp14A;void*_tmp14B;struct Cyc_Absyn_Tqual _tmp14C;struct Cyc_Absyn_Exp*_tmp14D;
union Cyc_Absyn_Constraint*_tmp14E;struct Cyc_Position_Segment*_tmp14F;struct Cyc_Core_Opt*
_tmp151;struct Cyc_Core_Opt _tmp152;void*_tmp153;_LL37: {struct Cyc_Absyn_ArrayType_struct*
_tmp149=(struct Cyc_Absyn_ArrayType_struct*)_tmp148;if(_tmp149->tag != 9)goto _LL39;
else{_tmp14A=_tmp149->f1;_tmp14B=_tmp14A.elt_type;_tmp14C=_tmp14A.tq;_tmp14D=
_tmp14A.num_elts;_tmp14E=_tmp14A.zero_term;_tmp14F=_tmp14A.zt_loc;}}_LL38: return
Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp14B),_tmp14C,_tmp14D,Cyc_Absyn_false_conref,
_tmp14F);_LL39: {struct Cyc_Absyn_Evar_struct*_tmp150=(struct Cyc_Absyn_Evar_struct*)
_tmp148;if(_tmp150->tag != 1)goto _LL3B;else{_tmp151=_tmp150->f2;if(_tmp151 == 0)
goto _LL3B;_tmp152=*_tmp151;_tmp153=(void*)_tmp152.v;}}_LL3A: return Cyc_Toc_typ_to_c_array(
_tmp153);_LL3B:;_LL3C: return Cyc_Toc_typ_to_c(t);_LL36:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmpAB9;return(_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->name=f->name,((
_tmpAB9->tq=Cyc_Toc_mt_tq,((_tmpAB9->type=Cyc_Toc_typ_to_c(f->type),((_tmpAB9->width=
f->width,((_tmpAB9->attributes=f->attributes,_tmpAB9)))))))))));}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs);static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}static void*Cyc_Toc_char_star_typ();static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpABA;cs=((_tmpABA=_cycalloc(sizeof(*_tmpABA)),((
_tmpABA[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpABA))));}return*cs;}static void*Cyc_Toc_rgn_typ();
static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0){void**_tmpABB;r=((_tmpABB=
_cycalloc(sizeof(*_tmpABB)),((_tmpABB[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq),_tmpABB))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ();static
void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){void**_tmpABC;r=((_tmpABC=
_cycalloc(sizeof(*_tmpABC)),((_tmpABC[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),
Cyc_Toc_mt_tq),_tmpABC))));}return*r;}static void*Cyc_Toc_typ_to_c(void*t);static
void*Cyc_Toc_typ_to_c(void*t){void*_tmp158=t;struct Cyc_Core_Opt*_tmp15B;struct
Cyc_Core_Opt*_tmp15D;struct Cyc_Core_Opt _tmp15E;void*_tmp15F;struct Cyc_Absyn_Tvar*
_tmp161;struct Cyc_Absyn_DatatypeFieldInfo _tmp164;union Cyc_Absyn_DatatypeFieldInfoU
_tmp165;struct _tuple1 _tmp166;struct Cyc_Absyn_Datatypedecl*_tmp167;struct Cyc_Absyn_Datatypefield*
_tmp168;struct Cyc_Absyn_PtrInfo _tmp16B;void*_tmp16C;struct Cyc_Absyn_Tqual _tmp16D;
struct Cyc_Absyn_PtrAtts _tmp16E;union Cyc_Absyn_Constraint*_tmp16F;struct Cyc_Absyn_ArrayInfo
_tmp174;void*_tmp175;struct Cyc_Absyn_Tqual _tmp176;struct Cyc_Absyn_Exp*_tmp177;
struct Cyc_Position_Segment*_tmp178;struct Cyc_Absyn_FnInfo _tmp17A;void*_tmp17B;
struct Cyc_List_List*_tmp17C;int _tmp17D;struct Cyc_Absyn_VarargInfo*_tmp17E;struct
Cyc_List_List*_tmp17F;struct Cyc_List_List*_tmp181;enum Cyc_Absyn_AggrKind _tmp183;
struct Cyc_List_List*_tmp184;struct Cyc_Absyn_AggrInfo _tmp186;union Cyc_Absyn_AggrInfoU
_tmp187;struct Cyc_List_List*_tmp188;struct _tuple0*_tmp18A;struct Cyc_List_List*
_tmp18C;struct _tuple0*_tmp18E;struct Cyc_List_List*_tmp18F;struct Cyc_Absyn_Typedefdecl*
_tmp190;void**_tmp191;void*_tmp194;_LL3E: {struct Cyc_Absyn_VoidType_struct*
_tmp159=(struct Cyc_Absyn_VoidType_struct*)_tmp158;if(_tmp159->tag != 0)goto _LL40;}
_LL3F: return t;_LL40: {struct Cyc_Absyn_Evar_struct*_tmp15A=(struct Cyc_Absyn_Evar_struct*)
_tmp158;if(_tmp15A->tag != 1)goto _LL42;else{_tmp15B=_tmp15A->f2;if(_tmp15B != 0)
goto _LL42;}}_LL41: return Cyc_Absyn_sint_typ;_LL42: {struct Cyc_Absyn_Evar_struct*
_tmp15C=(struct Cyc_Absyn_Evar_struct*)_tmp158;if(_tmp15C->tag != 1)goto _LL44;
else{_tmp15D=_tmp15C->f2;if(_tmp15D == 0)goto _LL44;_tmp15E=*_tmp15D;_tmp15F=(void*)
_tmp15E.v;}}_LL43: return Cyc_Toc_typ_to_c(_tmp15F);_LL44: {struct Cyc_Absyn_VarType_struct*
_tmp160=(struct Cyc_Absyn_VarType_struct*)_tmp158;if(_tmp160->tag != 2)goto _LL46;
else{_tmp161=_tmp160->f1;}}_LL45: if((Cyc_Tcutil_tvar_kind(_tmp161,& Cyc_Tcutil_bk))->kind
== Cyc_Absyn_AnyKind)return(void*)& Cyc_Absyn_VoidType_val;else{return Cyc_Absyn_void_star_typ();}
_LL46: {struct Cyc_Absyn_DatatypeType_struct*_tmp162=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp158;if(_tmp162->tag != 3)goto _LL48;}_LL47: return(void*)& Cyc_Absyn_VoidType_val;
_LL48: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp163=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp158;if(_tmp163->tag != 4)goto _LL4A;else{_tmp164=_tmp163->f1;_tmp165=_tmp164.field_info;
if((_tmp165.KnownDatatypefield).tag != 2)goto _LL4A;_tmp166=(struct _tuple1)(
_tmp165.KnownDatatypefield).val;_tmp167=_tmp166.f1;_tmp168=_tmp166.f2;}}_LL49: {
const char*_tmpABD;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp168->name,((
_tmpABD="_struct",_tag_dyneither(_tmpABD,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp169=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp158;if(_tmp169->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpAC0;void*_tmpABF;(_tmpABF=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAC0="unresolved DatatypeFieldType",
_tag_dyneither(_tmpAC0,sizeof(char),29))),_tag_dyneither(_tmpABF,sizeof(void*),0)));}
_LL4C: {struct Cyc_Absyn_PointerType_struct*_tmp16A=(struct Cyc_Absyn_PointerType_struct*)
_tmp158;if(_tmp16A->tag != 5)goto _LL4E;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16B.elt_typ;
_tmp16D=_tmp16B.elt_tq;_tmp16E=_tmp16B.ptr_atts;_tmp16F=_tmp16E.bounds;}}_LL4D:
_tmp16C=Cyc_Toc_typ_to_c_array(_tmp16C);{void*_tmp19F=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp16F);_LL77: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1A0=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp19F;if(_tmp1A0->tag != 0)goto _LL79;}_LL78: return Cyc_Toc_dyneither_ptr_typ;
_LL79:;_LL7A: return Cyc_Absyn_star_typ(_tmp16C,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp16D,Cyc_Absyn_false_conref);_LL76:;};_LL4E: {struct Cyc_Absyn_IntType_struct*
_tmp170=(struct Cyc_Absyn_IntType_struct*)_tmp158;if(_tmp170->tag != 6)goto _LL50;}
_LL4F: goto _LL51;_LL50: {struct Cyc_Absyn_FloatType_struct*_tmp171=(struct Cyc_Absyn_FloatType_struct*)
_tmp158;if(_tmp171->tag != 7)goto _LL52;}_LL51: goto _LL53;_LL52: {struct Cyc_Absyn_DoubleType_struct*
_tmp172=(struct Cyc_Absyn_DoubleType_struct*)_tmp158;if(_tmp172->tag != 8)goto
_LL54;}_LL53: return t;_LL54: {struct Cyc_Absyn_ArrayType_struct*_tmp173=(struct Cyc_Absyn_ArrayType_struct*)
_tmp158;if(_tmp173->tag != 9)goto _LL56;else{_tmp174=_tmp173->f1;_tmp175=_tmp174.elt_type;
_tmp176=_tmp174.tq;_tmp177=_tmp174.num_elts;_tmp178=_tmp174.zt_loc;}}_LL55:
return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp175),_tmp176,_tmp177,Cyc_Absyn_false_conref,
_tmp178);_LL56: {struct Cyc_Absyn_FnType_struct*_tmp179=(struct Cyc_Absyn_FnType_struct*)
_tmp158;if(_tmp179->tag != 10)goto _LL58;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp17A.ret_typ;
_tmp17C=_tmp17A.args;_tmp17D=_tmp17A.c_varargs;_tmp17E=_tmp17A.cyc_varargs;
_tmp17F=_tmp17A.attributes;}}_LL57: {struct Cyc_List_List*_tmp1A1=0;for(0;_tmp17F
!= 0;_tmp17F=_tmp17F->tl){void*_tmp1A2=(void*)_tmp17F->hd;_LL7C: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp1A3=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp1A2;if(_tmp1A3->tag != 4)goto
_LL7E;}_LL7D: goto _LL7F;_LL7E: {struct Cyc_Absyn_Const_att_struct*_tmp1A4=(struct
Cyc_Absyn_Const_att_struct*)_tmp1A2;if(_tmp1A4->tag != 5)goto _LL80;}_LL7F: goto
_LL81;_LL80: {struct Cyc_Absyn_Format_att_struct*_tmp1A5=(struct Cyc_Absyn_Format_att_struct*)
_tmp1A2;if(_tmp1A5->tag != 19)goto _LL82;}_LL81: continue;_LL82: {struct Cyc_Absyn_Noliveunique_att_struct*
_tmp1A6=(struct Cyc_Absyn_Noliveunique_att_struct*)_tmp1A2;if(_tmp1A6->tag != 21)
goto _LL84;}_LL83: continue;_LL84: {struct Cyc_Absyn_Initializes_att_struct*_tmp1A7=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp1A2;if(_tmp1A7->tag != 20)goto _LL86;}
_LL85: continue;_LL86:;_LL87:{struct Cyc_List_List*_tmpAC1;_tmp1A1=((_tmpAC1=
_cycalloc(sizeof(*_tmpAC1)),((_tmpAC1->hd=(void*)_tmp17F->hd,((_tmpAC1->tl=
_tmp1A1,_tmpAC1))))));}goto _LL7B;_LL7B:;}{struct Cyc_List_List*_tmp1A9=((struct
Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp17C);if(_tmp17E != 0){void*_tmp1AA=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp17E->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmpAC2;struct _tuple7*_tmp1AB=(_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((
_tmpAC2->f1=_tmp17E->name,((_tmpAC2->f2=_tmp17E->tq,((_tmpAC2->f3=_tmp1AA,
_tmpAC2)))))));struct Cyc_List_List*_tmpAC3;_tmp1A9=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1A9,((
_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3->hd=_tmp1AB,((_tmpAC3->tl=0,
_tmpAC3)))))));}{struct Cyc_Absyn_FnType_struct _tmpAC9;struct Cyc_Absyn_FnInfo
_tmpAC8;struct Cyc_Absyn_FnType_struct*_tmpAC7;return(void*)((_tmpAC7=_cycalloc(
sizeof(*_tmpAC7)),((_tmpAC7[0]=((_tmpAC9.tag=10,((_tmpAC9.f1=((_tmpAC8.tvars=0,((
_tmpAC8.effect=0,((_tmpAC8.ret_typ=Cyc_Toc_typ_to_c(_tmp17B),((_tmpAC8.args=
_tmp1A9,((_tmpAC8.c_varargs=_tmp17D,((_tmpAC8.cyc_varargs=0,((_tmpAC8.rgn_po=0,((
_tmpAC8.attributes=_tmp1A1,_tmpAC8)))))))))))))))),_tmpAC9)))),_tmpAC7))));};};}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp180=(struct Cyc_Absyn_TupleType_struct*)
_tmp158;if(_tmp180->tag != 11)goto _LL5A;else{_tmp181=_tmp180->f1;}}_LL59: _tmp181=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp181);return Cyc_Toc_add_tuple_type(_tmp181);
_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp182=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp158;if(_tmp182->tag != 13)goto _LL5C;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}
_LL5B: {struct Cyc_Absyn_AnonAggrType_struct _tmpACC;struct Cyc_Absyn_AnonAggrType_struct*
_tmpACB;return(void*)((_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB[0]=((
_tmpACC.tag=13,((_tmpACC.f1=_tmp183,((_tmpACC.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp184),_tmpACC)))))),_tmpACB))));}_LL5C: {
struct Cyc_Absyn_AggrType_struct*_tmp185=(struct Cyc_Absyn_AggrType_struct*)
_tmp158;if(_tmp185->tag != 12)goto _LL5E;else{_tmp186=_tmp185->f1;_tmp187=_tmp186.aggr_info;
_tmp188=_tmp186.targs;}}_LL5D:{union Cyc_Absyn_AggrInfoU _tmp1B3=_tmp187;struct
_tuple2 _tmp1B4;_LL89: if((_tmp1B3.UnknownAggr).tag != 1)goto _LL8B;_tmp1B4=(struct
_tuple2)(_tmp1B3.UnknownAggr).val;_LL8A: return t;_LL8B:;_LL8C: goto _LL88;_LL88:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1B5=Cyc_Absyn_get_known_aggrdecl(_tmp187);if(
_tmp1B5->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1B5->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp1B5->name,Cyc_Absyn_strctq);}};_LL5E: {
struct Cyc_Absyn_EnumType_struct*_tmp189=(struct Cyc_Absyn_EnumType_struct*)
_tmp158;if(_tmp189->tag != 14)goto _LL60;else{_tmp18A=_tmp189->f1;}}_LL5F: return t;
_LL60: {struct Cyc_Absyn_AnonEnumType_struct*_tmp18B=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp158;if(_tmp18B->tag != 15)goto _LL62;else{_tmp18C=_tmp18B->f1;}}_LL61: Cyc_Toc_enumfields_to_c(
_tmp18C);return t;_LL62: {struct Cyc_Absyn_TypedefType_struct*_tmp18D=(struct Cyc_Absyn_TypedefType_struct*)
_tmp158;if(_tmp18D->tag != 18)goto _LL64;else{_tmp18E=_tmp18D->f1;_tmp18F=_tmp18D->f2;
_tmp190=_tmp18D->f3;_tmp191=_tmp18D->f4;}}_LL63: if(_tmp191 == 0  || Cyc_noexpand_r){
if(_tmp18F != 0){struct Cyc_Absyn_TypedefType_struct _tmpACF;struct Cyc_Absyn_TypedefType_struct*
_tmpACE;return(void*)((_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE[0]=((
_tmpACF.tag=18,((_tmpACF.f1=_tmp18E,((_tmpACF.f2=0,((_tmpACF.f3=_tmp190,((
_tmpACF.f4=0,_tmpACF)))))))))),_tmpACE))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpAD5;void**_tmpAD4;struct Cyc_Absyn_TypedefType_struct*_tmpAD3;return(void*)((
_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3[0]=((_tmpAD5.tag=18,((_tmpAD5.f1=
_tmp18E,((_tmpAD5.f2=0,((_tmpAD5.f3=_tmp190,((_tmpAD5.f4=((_tmpAD4=_cycalloc(
sizeof(*_tmpAD4)),((_tmpAD4[0]=Cyc_Toc_typ_to_c_array(*_tmp191),_tmpAD4)))),
_tmpAD5)))))))))),_tmpAD3))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp192=(
struct Cyc_Absyn_TagType_struct*)_tmp158;if(_tmp192->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp193=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp158;if(_tmp193->tag != 16)goto _LL68;
else{_tmp194=(void*)_tmp193->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp195=(struct Cyc_Absyn_DynRgnType_struct*)_tmp158;
if(_tmp195->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp196=(struct Cyc_Absyn_HeapRgn_struct*)_tmp158;if(
_tmp196->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpAD8;void*_tmpAD7;(_tmpAD7=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAD8="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpAD8,sizeof(char),55))),_tag_dyneither(_tmpAD7,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp197=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp158;if(_tmp197->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpADB;void*_tmpADA;(
_tmpADA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADB="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpADB,sizeof(char),57))),_tag_dyneither(_tmpADA,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp198=(struct Cyc_Absyn_AccessEff_struct*)
_tmp158;if(_tmp198->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp199=(struct Cyc_Absyn_JoinEff_struct*)_tmp158;if(_tmp199->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp19A=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp158;if(_tmp19A->tag != 25)goto _LL74;}_LL73: {const char*_tmpADE;void*_tmpADD;(
_tmpADD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADE="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpADE,sizeof(char),49))),_tag_dyneither(_tmpADD,sizeof(void*),0)));}_LL74: {
struct Cyc_Absyn_ValueofType_struct*_tmp19B=(struct Cyc_Absyn_ValueofType_struct*)
_tmp158;if(_tmp19B->tag != 19)goto _LL3D;}_LL75: {const char*_tmpAE1;void*_tmpAE0;(
_tmpAE0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAE1="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpAE1,sizeof(char),51))),_tag_dyneither(_tmpAE0,sizeof(void*),0)));}_LL3D:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*l);static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp1C3=t;struct
Cyc_Absyn_ArrayInfo _tmp1C5;void*_tmp1C6;struct Cyc_Absyn_Tqual _tmp1C7;_LL8E: {
struct Cyc_Absyn_ArrayType_struct*_tmp1C4=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1C3;if(_tmp1C4->tag != 9)goto _LL90;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C5.elt_type;
_tmp1C7=_tmp1C5.tq;}}_LL8F: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1C6,(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1C7,Cyc_Absyn_false_conref),e);_LL90:;_LL91:
return Cyc_Toc_cast_it(t,e);_LL8D:;}static int Cyc_Toc_atomic_typ(void*t);static int
Cyc_Toc_atomic_typ(void*t){void*_tmp1C8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo
_tmp1D3;void*_tmp1D4;struct Cyc_Absyn_AggrInfo _tmp1D6;union Cyc_Absyn_AggrInfoU
_tmp1D7;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_DatatypeFieldInfo _tmp1DB;
union Cyc_Absyn_DatatypeFieldInfoU _tmp1DC;struct _tuple1 _tmp1DD;struct Cyc_Absyn_Datatypedecl*
_tmp1DE;struct Cyc_Absyn_Datatypefield*_tmp1DF;struct Cyc_List_List*_tmp1E1;_LL93: {
struct Cyc_Absyn_VoidType_struct*_tmp1C9=(struct Cyc_Absyn_VoidType_struct*)
_tmp1C8;if(_tmp1C9->tag != 0)goto _LL95;}_LL94: return 1;_LL95: {struct Cyc_Absyn_VarType_struct*
_tmp1CA=(struct Cyc_Absyn_VarType_struct*)_tmp1C8;if(_tmp1CA->tag != 2)goto _LL97;}
_LL96: return 0;_LL97: {struct Cyc_Absyn_IntType_struct*_tmp1CB=(struct Cyc_Absyn_IntType_struct*)
_tmp1C8;if(_tmp1CB->tag != 6)goto _LL99;}_LL98: goto _LL9A;_LL99: {struct Cyc_Absyn_EnumType_struct*
_tmp1CC=(struct Cyc_Absyn_EnumType_struct*)_tmp1C8;if(_tmp1CC->tag != 14)goto _LL9B;}
_LL9A: goto _LL9C;_LL9B: {struct Cyc_Absyn_AnonEnumType_struct*_tmp1CD=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1C8;if(_tmp1CD->tag != 15)goto _LL9D;}_LL9C: goto _LL9E;_LL9D: {struct Cyc_Absyn_FloatType_struct*
_tmp1CE=(struct Cyc_Absyn_FloatType_struct*)_tmp1C8;if(_tmp1CE->tag != 7)goto _LL9F;}
_LL9E: goto _LLA0;_LL9F: {struct Cyc_Absyn_DoubleType_struct*_tmp1CF=(struct Cyc_Absyn_DoubleType_struct*)
_tmp1C8;if(_tmp1CF->tag != 8)goto _LLA1;}_LLA0: goto _LLA2;_LLA1: {struct Cyc_Absyn_FnType_struct*
_tmp1D0=(struct Cyc_Absyn_FnType_struct*)_tmp1C8;if(_tmp1D0->tag != 10)goto _LLA3;}
_LLA2: goto _LLA4;_LLA3: {struct Cyc_Absyn_TagType_struct*_tmp1D1=(struct Cyc_Absyn_TagType_struct*)
_tmp1C8;if(_tmp1D1->tag != 20)goto _LLA5;}_LLA4: return 1;_LLA5: {struct Cyc_Absyn_ArrayType_struct*
_tmp1D2=(struct Cyc_Absyn_ArrayType_struct*)_tmp1C8;if(_tmp1D2->tag != 9)goto _LLA7;
else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D3.elt_type;}}_LLA6: return Cyc_Toc_atomic_typ(
_tmp1D4);_LLA7: {struct Cyc_Absyn_AggrType_struct*_tmp1D5=(struct Cyc_Absyn_AggrType_struct*)
_tmp1C8;if(_tmp1D5->tag != 12)goto _LLA9;else{_tmp1D6=_tmp1D5->f1;_tmp1D7=_tmp1D6.aggr_info;}}
_LLA8:{union Cyc_Absyn_AggrInfoU _tmp1E6=_tmp1D7;struct _tuple2 _tmp1E7;_LLBA: if((
_tmp1E6.UnknownAggr).tag != 1)goto _LLBC;_tmp1E7=(struct _tuple2)(_tmp1E6.UnknownAggr).val;
_LLBB: return 0;_LLBC:;_LLBD: goto _LLB9;_LLB9:;}{struct Cyc_Absyn_Aggrdecl*_tmp1E8=
Cyc_Absyn_get_known_aggrdecl(_tmp1D7);if(_tmp1E8->impl == 0)return 0;{struct Cyc_List_List*
_tmp1E9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E8->impl))->fields;for(
0;_tmp1E9 != 0;_tmp1E9=_tmp1E9->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp1E9->hd)->type))return 0;}}return 1;};_LLA9: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1D8=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C8;if(_tmp1D8->tag != 13)goto
_LLAB;else{_tmp1D9=_tmp1D8->f2;}}_LLAA: for(0;_tmp1D9 != 0;_tmp1D9=_tmp1D9->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1D9->hd)->type))return 0;}
return 1;_LLAB: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1DA=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp1C8;if(_tmp1DA->tag != 4)goto _LLAD;else{_tmp1DB=_tmp1DA->f1;_tmp1DC=_tmp1DB.field_info;
if((_tmp1DC.KnownDatatypefield).tag != 2)goto _LLAD;_tmp1DD=(struct _tuple1)(
_tmp1DC.KnownDatatypefield).val;_tmp1DE=_tmp1DD.f1;_tmp1DF=_tmp1DD.f2;}}_LLAC:
_tmp1E1=_tmp1DF->typs;goto _LLAE;_LLAD: {struct Cyc_Absyn_TupleType_struct*_tmp1E0=(
struct Cyc_Absyn_TupleType_struct*)_tmp1C8;if(_tmp1E0->tag != 11)goto _LLAF;else{
_tmp1E1=_tmp1E0->f1;}}_LLAE: for(0;_tmp1E1 != 0;_tmp1E1=_tmp1E1->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple9*)_tmp1E1->hd)).f2))return 0;}return 1;_LLAF: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1E2=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1C8;if(_tmp1E2->tag != 3)goto
_LLB1;}_LLB0: goto _LLB2;_LLB1: {struct Cyc_Absyn_PointerType_struct*_tmp1E3=(
struct Cyc_Absyn_PointerType_struct*)_tmp1C8;if(_tmp1E3->tag != 5)goto _LLB3;}_LLB2:
goto _LLB4;_LLB3: {struct Cyc_Absyn_DynRgnType_struct*_tmp1E4=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp1C8;if(_tmp1E4->tag != 17)goto _LLB5;}_LLB4: goto _LLB6;_LLB5: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp1E5=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp1C8;if(_tmp1E5->tag != 16)goto
_LLB7;}_LLB6: return 0;_LLB7:;_LLB8: {const char*_tmpAE5;void*_tmpAE4[1];struct Cyc_String_pa_struct
_tmpAE3;(_tmpAE3.tag=0,((_tmpAE3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAE4[0]=& _tmpAE3,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE5="atomic_typ:  bad type %s",
_tag_dyneither(_tmpAE5,sizeof(char),25))),_tag_dyneither(_tmpAE4,sizeof(void*),1)))))));}
_LL92:;}static int Cyc_Toc_is_void_star(void*t);static int Cyc_Toc_is_void_star(void*
t){void*_tmp1ED=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1EF;void*
_tmp1F0;_LLBF: {struct Cyc_Absyn_PointerType_struct*_tmp1EE=(struct Cyc_Absyn_PointerType_struct*)
_tmp1ED;if(_tmp1EE->tag != 5)goto _LLC1;else{_tmp1EF=_tmp1EE->f1;_tmp1F0=_tmp1EF.elt_typ;}}
_LLC0: {void*_tmp1F1=Cyc_Tcutil_compress(_tmp1F0);_LLC4: {struct Cyc_Absyn_VoidType_struct*
_tmp1F2=(struct Cyc_Absyn_VoidType_struct*)_tmp1F1;if(_tmp1F2->tag != 0)goto _LLC6;}
_LLC5: return 1;_LLC6:;_LLC7: return 0;_LLC3:;}_LLC1:;_LLC2: return 0;_LLBE:;}static
int Cyc_Toc_is_tvar(void*t);static int Cyc_Toc_is_tvar(void*t){void*_tmp1F3=Cyc_Tcutil_compress(
t);_LLC9: {struct Cyc_Absyn_VarType_struct*_tmp1F4=(struct Cyc_Absyn_VarType_struct*)
_tmp1F3;if(_tmp1F4->tag != 2)goto _LLCB;}_LLCA: return 1;_LLCB:;_LLCC: return 0;_LLC8:;}
static int Cyc_Toc_is_void_star_or_tvar(void*t);static int Cyc_Toc_is_void_star_or_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f);static int Cyc_Toc_is_poly_field(void*t,struct
_dyneither_ptr*f){void*_tmp1F5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1F7;union Cyc_Absyn_AggrInfoU _tmp1F8;struct Cyc_List_List*_tmp1FA;_LLCE: {
struct Cyc_Absyn_AggrType_struct*_tmp1F6=(struct Cyc_Absyn_AggrType_struct*)
_tmp1F5;if(_tmp1F6->tag != 12)goto _LLD0;else{_tmp1F7=_tmp1F6->f1;_tmp1F8=_tmp1F7.aggr_info;}}
_LLCF: {struct Cyc_Absyn_Aggrdecl*_tmp1FB=Cyc_Absyn_get_known_aggrdecl(_tmp1F8);
if(_tmp1FB->impl == 0){const char*_tmpAE8;void*_tmpAE7;(_tmpAE7=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE8="is_poly_field: type missing fields",
_tag_dyneither(_tmpAE8,sizeof(char),35))),_tag_dyneither(_tmpAE7,sizeof(void*),0)));}
_tmp1FA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FB->impl))->fields;goto
_LLD1;}_LLD0: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1F9=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F5;if(_tmp1F9->tag != 13)goto _LLD2;else{_tmp1FA=_tmp1F9->f2;}}_LLD1: {struct
Cyc_Absyn_Aggrfield*_tmp1FE=Cyc_Absyn_lookup_field(_tmp1FA,f);if(_tmp1FE == 0){
const char*_tmpAEC;void*_tmpAEB[1];struct Cyc_String_pa_struct _tmpAEA;(_tmpAEA.tag=
0,((_tmpAEA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAEB[0]=&
_tmpAEA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAEC="is_poly_field: bad field %s",_tag_dyneither(_tmpAEC,sizeof(char),28))),
_tag_dyneither(_tmpAEB,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp1FE->type);}_LLD2:;_LLD3: {const char*_tmpAF0;void*_tmpAEF[1];struct Cyc_String_pa_struct
_tmpAEE;(_tmpAEE.tag=0,((_tmpAEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAEF[0]=& _tmpAEE,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF0="is_poly_field: bad type %s",
_tag_dyneither(_tmpAF0,sizeof(char),27))),_tag_dyneither(_tmpAEF,sizeof(void*),1)))))));}
_LLCD:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp*e){void*_tmp205=e->r;struct Cyc_Absyn_Exp*_tmp207;struct
_dyneither_ptr*_tmp208;struct Cyc_Absyn_Exp*_tmp20A;struct _dyneither_ptr*_tmp20B;
_LLD5: {struct Cyc_Absyn_AggrMember_e_struct*_tmp206=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp205;if(_tmp206->tag != 22)goto _LLD7;else{_tmp207=_tmp206->f1;_tmp208=_tmp206->f2;}}
_LLD6: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp207->topt))->v,_tmp208);_LLD7: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp209=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp205;if(_tmp209->tag != 23)goto _LLD9;else{
_tmp20A=_tmp209->f1;_tmp20B=_tmp209->f2;}}_LLD8: {void*_tmp20C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp20A->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp20E;void*_tmp20F;_LLDC: {struct Cyc_Absyn_PointerType_struct*_tmp20D=(struct
Cyc_Absyn_PointerType_struct*)_tmp20C;if(_tmp20D->tag != 5)goto _LLDE;else{_tmp20E=
_tmp20D->f1;_tmp20F=_tmp20E.elt_typ;}}_LLDD: return Cyc_Toc_is_poly_field(_tmp20F,
_tmp20B);_LLDE:;_LLDF: {const char*_tmpAF4;void*_tmpAF3[1];struct Cyc_String_pa_struct
_tmpAF2;(_tmpAF2.tag=0,((_tmpAF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp20A->topt))->v)),((
_tmpAF3[0]=& _tmpAF2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAF4="is_poly_project: bad type %s",_tag_dyneither(_tmpAF4,sizeof(char),29))),
_tag_dyneither(_tmpAF3,sizeof(void*),1)))))));}_LLDB:;}_LLD9:;_LLDA: return 0;
_LLD4:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s);
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*
_tmpAF5;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpAF5=_cycalloc(
sizeof(*_tmpAF5)),((_tmpAF5->hd=s,((_tmpAF5->tl=0,_tmpAF5)))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpAF6;return
Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpAF6=_cycalloc(sizeof(*
_tmpAF6)),((_tmpAF6->hd=s,((_tmpAF6->tl=0,_tmpAF6)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpAF7[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpAF7[1]=s,((_tmpAF7[0]=
rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF7,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n);
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpAF8[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpAF8[1]=n,((
_tmpAF8[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF8,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpAF9[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpAF9[1]=n,((
_tmpAF9[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF9,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n);static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_tmpAFA[3];return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,((_tmpAFA[2]=n,((_tmpAFA[1]=s,((_tmpAFA[0]=rgn,((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAFA,
sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpAFB;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB->hd=e,((_tmpAFB->tl=0,_tmpAFB)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt();static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){static struct Cyc_Absyn_Stmt**throw_match_stmt_opt=0;
if(throw_match_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpAFC;throw_match_stmt_opt=((
_tmpAFC=_cycalloc(sizeof(*_tmpAFC)),((_tmpAFC[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0)),0),_tmpAFC))));}return*throw_match_stmt_opt;}static
struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp21B=e->r;union Cyc_Absyn_Cnst _tmp21D;struct _dyneither_ptr _tmp21E;_LLE1: {
struct Cyc_Absyn_Const_e_struct*_tmp21C=(struct Cyc_Absyn_Const_e_struct*)_tmp21B;
if(_tmp21C->tag != 0)goto _LLE3;else{_tmp21D=_tmp21C->f1;if((_tmp21D.String_c).tag
!= 7)goto _LLE3;_tmp21E=(struct _dyneither_ptr)(_tmp21D.String_c).val;}}_LLE2:
is_string=1;goto _LLE0;_LLE3:;_LLE4: goto _LLE0;_LLE0:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpB02;struct
Cyc_Absyn_Var_d_struct _tmpB01;struct Cyc_List_List*_tmpB00;Cyc_Toc_result_decls=((
_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00->hd=Cyc_Absyn_new_decl((void*)((
_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02[0]=((_tmpB01.tag=0,((_tmpB01.f1=vd,
_tmpB01)))),_tmpB02)))),0),((_tmpB00->tl=Cyc_Toc_result_decls,_tmpB00))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpB09;
struct _tuple15*_tmpB08;struct _tuple15*_tmpB07;struct _tuple15*_tmpB06[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpB06[2]=((_tmpB07=_cycalloc(sizeof(*_tmpB07)),((
_tmpB07->f1=0,((_tmpB07->f2=xplussz,_tmpB07)))))),((_tmpB06[1]=((_tmpB08=
_cycalloc(sizeof(*_tmpB08)),((_tmpB08->f1=0,((_tmpB08->f2=xexp,_tmpB08)))))),((
_tmpB06[0]=((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09->f1=0,((_tmpB09->f2=
xexp,_tmpB09)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB06,sizeof(struct _tuple15*),3)))))))),0);}return urm_exp;};};}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;struct _RegionHandle*rgn;};
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv);static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp227;int _tmp228;struct Cyc_Toc_Env*
_tmp226=nv;_tmp227=*_tmp226;_tmp228=_tmp227.toplevel;return _tmp228;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x);static
struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){
struct Cyc_Toc_Env _tmp22A;struct Cyc_Dict_Dict _tmp22B;struct Cyc_Toc_Env*_tmp229=nv;
_tmp22A=*_tmp229;_tmp22B=_tmp22A.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp22B,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r);static struct Cyc_Toc_Env*Cyc_Toc_empty_env(
struct _RegionHandle*r){struct Cyc_Toc_Env*_tmpB0A;return(_tmpB0A=_region_malloc(r,
sizeof(*_tmpB0A)),((_tmpB0A->break_lab=(struct _dyneither_ptr**)0,((_tmpB0A->continue_lab=(
struct _dyneither_ptr**)0,((_tmpB0A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpB0A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpB0A->toplevel=(int)1,((_tmpB0A->rgn=(struct _RegionHandle*)r,_tmpB0A)))))))))))));}
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp22E;struct _dyneither_ptr**_tmp22F;struct _dyneither_ptr**
_tmp230;struct Cyc_Toc_FallthruInfo*_tmp231;struct Cyc_Dict_Dict _tmp232;int _tmp233;
struct Cyc_Toc_Env*_tmp22D=e;_tmp22E=*_tmp22D;_tmp22F=_tmp22E.break_lab;_tmp230=
_tmp22E.continue_lab;_tmp231=_tmp22E.fallthru_info;_tmp232=_tmp22E.varmap;
_tmp233=_tmp22E.toplevel;{struct Cyc_Toc_Env*_tmpB0B;return(_tmpB0B=
_region_malloc(r,sizeof(*_tmpB0B)),((_tmpB0B->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22F),((_tmpB0B->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp230),((_tmpB0B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp231,((_tmpB0B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp232),((_tmpB0B->toplevel=(
int)_tmp233,((_tmpB0B->rgn=(struct _RegionHandle*)r,_tmpB0B)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct
Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp236;struct _dyneither_ptr**_tmp237;struct
_dyneither_ptr**_tmp238;struct Cyc_Toc_FallthruInfo*_tmp239;struct Cyc_Dict_Dict
_tmp23A;int _tmp23B;struct Cyc_Toc_Env*_tmp235=e;_tmp236=*_tmp235;_tmp237=_tmp236.break_lab;
_tmp238=_tmp236.continue_lab;_tmp239=_tmp236.fallthru_info;_tmp23A=_tmp236.varmap;
_tmp23B=_tmp236.toplevel;{struct Cyc_Toc_Env*_tmpB0C;return(_tmpB0C=
_region_malloc(r,sizeof(*_tmpB0C)),((_tmpB0C->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp237),((_tmpB0C->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp238),((_tmpB0C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp239,((_tmpB0C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp23A),((_tmpB0C->toplevel=(
int)0,((_tmpB0C->rgn=(struct _RegionHandle*)r,_tmpB0C)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp23E;struct _dyneither_ptr**_tmp23F;struct _dyneither_ptr**
_tmp240;struct Cyc_Toc_FallthruInfo*_tmp241;struct Cyc_Dict_Dict _tmp242;int _tmp243;
struct Cyc_Toc_Env*_tmp23D=e;_tmp23E=*_tmp23D;_tmp23F=_tmp23E.break_lab;_tmp240=
_tmp23E.continue_lab;_tmp241=_tmp23E.fallthru_info;_tmp242=_tmp23E.varmap;
_tmp243=_tmp23E.toplevel;{struct Cyc_Toc_Env*_tmpB0D;return(_tmpB0D=
_region_malloc(r,sizeof(*_tmpB0D)),((_tmpB0D->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23F),((_tmpB0D->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp240),((_tmpB0D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp241,((_tmpB0D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp242),((_tmpB0D->toplevel=(
int)1,((_tmpB0D->rgn=(struct _RegionHandle*)r,_tmpB0D)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,
struct _tuple0*x,struct Cyc_Absyn_Exp*y);static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp245=(*x).f1;struct Cyc_List_List*_tmp246;_LLE6: if((
_tmp245.Rel_n).tag != 1)goto _LLE8;_tmp246=(struct Cyc_List_List*)(_tmp245.Rel_n).val;
_LLE7: {const char*_tmpB11;void*_tmpB10[1];struct Cyc_String_pa_struct _tmpB0F;(
_tmpB0F.tag=0,((_tmpB0F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpB10[0]=& _tmpB0F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB11="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB11,sizeof(char),30))),_tag_dyneither(_tmpB10,sizeof(void*),1)))))));}_LLE8:;
_LLE9: goto _LLE5;_LLE5:;}{struct Cyc_Toc_Env _tmp24B;struct _dyneither_ptr**_tmp24C;
struct _dyneither_ptr**_tmp24D;struct Cyc_Toc_FallthruInfo*_tmp24E;struct Cyc_Dict_Dict
_tmp24F;int _tmp250;struct Cyc_Toc_Env*_tmp24A=e;_tmp24B=*_tmp24A;_tmp24C=_tmp24B.break_lab;
_tmp24D=_tmp24B.continue_lab;_tmp24E=_tmp24B.fallthru_info;_tmp24F=_tmp24B.varmap;
_tmp250=_tmp24B.toplevel;{struct Cyc_Dict_Dict _tmp251=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp24F),x,y);struct Cyc_Toc_Env*_tmpB12;return(_tmpB12=_region_malloc(r,
sizeof(*_tmpB12)),((_tmpB12->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp24C),((_tmpB12->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp24D),((_tmpB12->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24E,((_tmpB12->varmap=(struct Cyc_Dict_Dict)_tmp251,((_tmpB12->toplevel=(int)
_tmp250,((_tmpB12->rgn=(struct _RegionHandle*)r,_tmpB12)))))))))))));};};}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp254;struct _dyneither_ptr**_tmp255;struct _dyneither_ptr**
_tmp256;struct Cyc_Toc_FallthruInfo*_tmp257;struct Cyc_Dict_Dict _tmp258;int _tmp259;
struct Cyc_Toc_Env*_tmp253=e;_tmp254=*_tmp253;_tmp255=_tmp254.break_lab;_tmp256=
_tmp254.continue_lab;_tmp257=_tmp254.fallthru_info;_tmp258=_tmp254.varmap;
_tmp259=_tmp254.toplevel;{struct Cyc_Toc_Env*_tmpB13;return(_tmpB13=
_region_malloc(r,sizeof(*_tmpB13)),((_tmpB13->break_lab=(struct _dyneither_ptr**)
0,((_tmpB13->continue_lab=(struct _dyneither_ptr**)0,((_tmpB13->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp257,((_tmpB13->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp258),((_tmpB13->toplevel=(int)_tmp259,((_tmpB13->rgn=(struct _RegionHandle*)
r,_tmpB13)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env);static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB14;fallthru_vars=((
_tmpB14=_region_malloc(r,sizeof(*_tmpB14)),((_tmpB14->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB14->tl=fallthru_vars,_tmpB14))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp25D;struct _dyneither_ptr**_tmp25E;struct
_dyneither_ptr**_tmp25F;struct Cyc_Toc_FallthruInfo*_tmp260;struct Cyc_Dict_Dict
_tmp261;int _tmp262;struct Cyc_Toc_Env*_tmp25C=e;_tmp25D=*_tmp25C;_tmp25E=_tmp25D.break_lab;
_tmp25F=_tmp25D.continue_lab;_tmp260=_tmp25D.fallthru_info;_tmp261=_tmp25D.varmap;
_tmp262=_tmp25D.toplevel;{struct Cyc_Toc_Env _tmp264;struct Cyc_Dict_Dict _tmp265;
struct Cyc_Toc_Env*_tmp263=next_case_env;_tmp264=*_tmp263;_tmp265=_tmp264.varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB15;struct Cyc_Toc_FallthruInfo*fi=(_tmpB15=
_region_malloc(r,sizeof(*_tmpB15)),((_tmpB15->label=fallthru_l,((_tmpB15->binders=
fallthru_vars,((_tmpB15->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp265),_tmpB15)))))));
struct _dyneither_ptr**_tmpB18;struct Cyc_Toc_Env*_tmpB17;return(_tmpB17=
_region_malloc(r,sizeof(*_tmpB17)),((_tmpB17->break_lab=(struct _dyneither_ptr**)((
_tmpB18=_region_malloc(r,sizeof(*_tmpB18)),((_tmpB18[0]=break_l,_tmpB18)))),((
_tmpB17->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp25F),((
_tmpB17->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB17->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp261),((_tmpB17->toplevel=(int)_tmp262,((_tmpB17->rgn=(
struct _RegionHandle*)r,_tmpB17)))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l);static
struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp26A;struct _dyneither_ptr**
_tmp26B;struct _dyneither_ptr**_tmp26C;struct Cyc_Toc_FallthruInfo*_tmp26D;struct
Cyc_Dict_Dict _tmp26E;int _tmp26F;struct Cyc_Toc_Env*_tmp269=e;_tmp26A=*_tmp269;
_tmp26B=_tmp26A.break_lab;_tmp26C=_tmp26A.continue_lab;_tmp26D=_tmp26A.fallthru_info;
_tmp26E=_tmp26A.varmap;_tmp26F=_tmp26A.toplevel;{struct _dyneither_ptr**_tmpB1B;
struct Cyc_Toc_Env*_tmpB1A;return(_tmpB1A=_region_malloc(r,sizeof(*_tmpB1A)),((
_tmpB1A->break_lab=(struct _dyneither_ptr**)((_tmpB1B=_region_malloc(r,sizeof(*
_tmpB1B)),((_tmpB1B[0]=break_l,_tmpB1B)))),((_tmpB1A->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp26C),((_tmpB1A->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB1A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp26E),((_tmpB1A->toplevel=(
int)_tmp26F,((_tmpB1A->rgn=(struct _RegionHandle*)r,_tmpB1A)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l);static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp273;struct _dyneither_ptr**_tmp274;struct _dyneither_ptr**_tmp275;struct Cyc_Toc_FallthruInfo*
_tmp276;struct Cyc_Dict_Dict _tmp277;int _tmp278;struct Cyc_Toc_Env*_tmp272=e;
_tmp273=*_tmp272;_tmp274=_tmp273.break_lab;_tmp275=_tmp273.continue_lab;_tmp276=
_tmp273.fallthru_info;_tmp277=_tmp273.varmap;_tmp278=_tmp273.toplevel;{struct Cyc_Toc_FallthruInfo*
_tmpB1E;struct Cyc_Toc_Env*_tmpB1D;return(_tmpB1D=_region_malloc(r,sizeof(*
_tmpB1D)),((_tmpB1D->break_lab=(struct _dyneither_ptr**)0,((_tmpB1D->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp275),((_tmpB1D->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)((_tmpB1E=_region_malloc(r,sizeof(*_tmpB1E)),((
_tmpB1E->label=next_l,((_tmpB1E->binders=0,((_tmpB1E->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),_tmpB1E)))))))),((_tmpB1D->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp277),((_tmpB1D->toplevel=(int)_tmp278,((_tmpB1D->rgn=(struct _RegionHandle*)
r,_tmpB1D)))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp27B=e->annot;_LLEB: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp27C=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp27B;if(_tmp27C->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLED;}_LLEC: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLED: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp27D=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp27B;if(_tmp27D->tag != Cyc_CfFlowInfo_NotZero)
goto _LLEF;}_LLEE: return 0;_LLEF: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp27E=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp27B;if(_tmp27E->tag != Cyc_CfFlowInfo_IsZero)
goto _LLF1;}_LLF0:{const char*_tmpB21;void*_tmpB20;(_tmpB20=0,Cyc_Tcutil_terr(e->loc,((
_tmpB21="dereference of NULL pointer",_tag_dyneither(_tmpB21,sizeof(char),28))),
_tag_dyneither(_tmpB20,sizeof(void*),0)));}return 0;_LLF1: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp27F=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp27B;if(_tmp27F->tag != Cyc_Absyn_EmptyAnnot)
goto _LLF3;}_LLF2: return 0;_LLF3:;_LLF4: {const char*_tmpB24;void*_tmpB23;(_tmpB23=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB24="need_null_check",_tag_dyneither(_tmpB24,sizeof(char),16))),
_tag_dyneither(_tmpB23,sizeof(void*),0)));}_LLEA:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e);static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*
e){void*_tmp284=e->annot;struct Cyc_List_List*_tmp286;struct Cyc_List_List*_tmp288;
_LLF6: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp285=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp284;if(_tmp285->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLF8;else{_tmp286=_tmp285->f1;}}
_LLF7: return _tmp286;_LLF8: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp287=(struct
Cyc_CfFlowInfo_NotZero_struct*)_tmp284;if(_tmp287->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFA;else{_tmp288=_tmp287->f1;}}_LLF9: return _tmp288;_LLFA: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp289=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp284;if(_tmp289->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFC;}_LLFB:{const char*_tmpB27;void*_tmpB26;(_tmpB26=0,Cyc_Tcutil_terr(e->loc,((
_tmpB27="dereference of NULL pointer",_tag_dyneither(_tmpB27,sizeof(char),28))),
_tag_dyneither(_tmpB26,sizeof(void*),0)));}return 0;_LLFC: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp28A=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp284;if(_tmp28A->tag != Cyc_Absyn_EmptyAnnot)
goto _LLFE;}_LLFD: return 0;_LLFE:;_LLFF: {const char*_tmpB2A;void*_tmpB29;(_tmpB29=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB2A="get_relns",_tag_dyneither(_tmpB2A,sizeof(char),10))),_tag_dyneither(
_tmpB29,sizeof(void*),0)));}_LLF5:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t);static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp28F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp291;struct Cyc_Absyn_PtrAtts
_tmp292;union Cyc_Absyn_Constraint*_tmp293;union Cyc_Absyn_Constraint*_tmp294;
struct Cyc_Absyn_ArrayInfo _tmp296;struct Cyc_Absyn_Exp*_tmp297;_LL101: {struct Cyc_Absyn_PointerType_struct*
_tmp290=(struct Cyc_Absyn_PointerType_struct*)_tmp28F;if(_tmp290->tag != 5)goto
_LL103;else{_tmp291=_tmp290->f1;_tmp292=_tmp291.ptr_atts;_tmp293=_tmp292.bounds;
_tmp294=_tmp292.zero_term;}}_LL102: {void*_tmp298=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp293);struct Cyc_Absyn_Exp*
_tmp29B;_LL108: {struct Cyc_Absyn_DynEither_b_struct*_tmp299=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp298;if(_tmp299->tag != 0)goto _LL10A;}_LL109: return 0;_LL10A: {struct Cyc_Absyn_Upper_b_struct*
_tmp29A=(struct Cyc_Absyn_Upper_b_struct*)_tmp298;if(_tmp29A->tag != 1)goto _LL107;
else{_tmp29B=_tmp29A->f1;}}_LL10B: {unsigned int _tmp29D;int _tmp29E;struct
_tuple10 _tmp29C=Cyc_Evexp_eval_const_uint_exp(_tmp29B);_tmp29D=_tmp29C.f1;
_tmp29E=_tmp29C.f2;return _tmp29E  && i <= _tmp29D;}_LL107:;}_LL103: {struct Cyc_Absyn_ArrayType_struct*
_tmp295=(struct Cyc_Absyn_ArrayType_struct*)_tmp28F;if(_tmp295->tag != 9)goto
_LL105;else{_tmp296=_tmp295->f1;_tmp297=_tmp296.num_elts;}}_LL104: if(_tmp297 == 0)
return 0;{unsigned int _tmp2A0;int _tmp2A1;struct _tuple10 _tmp29F=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp297);_tmp2A0=_tmp29F.f1;_tmp2A1=_tmp29F.f2;return
_tmp2A1  && i <= _tmp2A0;};_LL105:;_LL106: return 0;_LL100:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y);
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*
_tmp2A2=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(_tmp2A2->vd != y)continue;{union
Cyc_CfFlowInfo_RelnOp _tmp2A3=_tmp2A2->rop;struct Cyc_Absyn_Vardecl*_tmp2A4;struct
Cyc_Absyn_Vardecl*_tmp2A5;_LL10D: if((_tmp2A3.LessNumelts).tag != 3)goto _LL10F;
_tmp2A4=(struct Cyc_Absyn_Vardecl*)(_tmp2A3.LessNumelts).val;_LL10E: _tmp2A5=
_tmp2A4;goto _LL110;_LL10F: if((_tmp2A3.LessEqNumelts).tag != 5)goto _LL111;_tmp2A5=(
struct Cyc_Absyn_Vardecl*)(_tmp2A3.LessEqNumelts).val;_LL110: if(_tmp2A5 == v)
return 1;else{goto _LL10C;}_LL111:;_LL112: continue;_LL10C:;};}return 0;}static int
Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,
struct Cyc_Absyn_Exp*e,void*vtype);static int Cyc_Toc_check_leq_size(struct Cyc_List_List*
relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e,void*vtype){{void*_tmp2A6=
e->r;void*_tmp2A8;struct Cyc_Absyn_Vardecl*_tmp2AA;void*_tmp2AC;struct Cyc_Absyn_Vardecl*
_tmp2AE;void*_tmp2B0;struct Cyc_Absyn_Vardecl*_tmp2B2;void*_tmp2B4;struct Cyc_Absyn_Vardecl*
_tmp2B6;union Cyc_Absyn_Cnst _tmp2B8;struct _tuple5 _tmp2B9;enum Cyc_Absyn_Sign
_tmp2BA;int _tmp2BB;union Cyc_Absyn_Cnst _tmp2BD;struct _tuple5 _tmp2BE;enum Cyc_Absyn_Sign
_tmp2BF;int _tmp2C0;union Cyc_Absyn_Cnst _tmp2C2;struct _tuple5 _tmp2C3;enum Cyc_Absyn_Sign
_tmp2C4;int _tmp2C5;enum Cyc_Absyn_Primop _tmp2C7;struct Cyc_List_List*_tmp2C8;
struct Cyc_List_List _tmp2C9;struct Cyc_Absyn_Exp*_tmp2CA;_LL114: {struct Cyc_Absyn_Var_e_struct*
_tmp2A7=(struct Cyc_Absyn_Var_e_struct*)_tmp2A6;if(_tmp2A7->tag != 1)goto _LL116;
else{_tmp2A8=(void*)_tmp2A7->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2A9=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2A8;if(_tmp2A9->tag != 5)goto _LL116;else{_tmp2AA=_tmp2A9->f1;}};}}_LL115:
_tmp2AE=_tmp2AA;goto _LL117;_LL116: {struct Cyc_Absyn_Var_e_struct*_tmp2AB=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A6;if(_tmp2AB->tag != 1)goto _LL118;else{_tmp2AC=(
void*)_tmp2AB->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2AD=(struct Cyc_Absyn_Local_b_struct*)
_tmp2AC;if(_tmp2AD->tag != 4)goto _LL118;else{_tmp2AE=_tmp2AD->f1;}};}}_LL117:
_tmp2B2=_tmp2AE;goto _LL119;_LL118: {struct Cyc_Absyn_Var_e_struct*_tmp2AF=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A6;if(_tmp2AF->tag != 1)goto _LL11A;else{_tmp2B0=(
void*)_tmp2AF->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2B1=(struct Cyc_Absyn_Global_b_struct*)
_tmp2B0;if(_tmp2B1->tag != 1)goto _LL11A;else{_tmp2B2=_tmp2B1->f1;}};}}_LL119:
_tmp2B6=_tmp2B2;goto _LL11B;_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp2B3=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A6;if(_tmp2B3->tag != 1)goto _LL11C;else{_tmp2B4=(
void*)_tmp2B3->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2B5=(struct Cyc_Absyn_Param_b_struct*)
_tmp2B4;if(_tmp2B5->tag != 3)goto _LL11C;else{_tmp2B6=_tmp2B5->f1;}};}}_LL11B: if(
_tmp2B6->escapes)return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2B6))return 1;
goto _LL113;_LL11C: {struct Cyc_Absyn_Const_e_struct*_tmp2B7=(struct Cyc_Absyn_Const_e_struct*)
_tmp2A6;if(_tmp2B7->tag != 0)goto _LL11E;else{_tmp2B8=_tmp2B7->f1;if((_tmp2B8.Int_c).tag
!= 4)goto _LL11E;_tmp2B9=(struct _tuple5)(_tmp2B8.Int_c).val;_tmp2BA=_tmp2B9.f1;
if(_tmp2BA != Cyc_Absyn_None)goto _LL11E;_tmp2BB=_tmp2B9.f2;}}_LL11D: _tmp2C0=
_tmp2BB;goto _LL11F;_LL11E: {struct Cyc_Absyn_Const_e_struct*_tmp2BC=(struct Cyc_Absyn_Const_e_struct*)
_tmp2A6;if(_tmp2BC->tag != 0)goto _LL120;else{_tmp2BD=_tmp2BC->f1;if((_tmp2BD.Int_c).tag
!= 4)goto _LL120;_tmp2BE=(struct _tuple5)(_tmp2BD.Int_c).val;_tmp2BF=_tmp2BE.f1;
if(_tmp2BF != Cyc_Absyn_Signed)goto _LL120;_tmp2C0=_tmp2BE.f2;}}_LL11F: return
_tmp2C0 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp2C0,vtype);_LL120: {
struct Cyc_Absyn_Const_e_struct*_tmp2C1=(struct Cyc_Absyn_Const_e_struct*)_tmp2A6;
if(_tmp2C1->tag != 0)goto _LL122;else{_tmp2C2=_tmp2C1->f1;if((_tmp2C2.Int_c).tag != 
4)goto _LL122;_tmp2C3=(struct _tuple5)(_tmp2C2.Int_c).val;_tmp2C4=_tmp2C3.f1;if(
_tmp2C4 != Cyc_Absyn_Unsigned)goto _LL122;_tmp2C5=_tmp2C3.f2;}}_LL121: return Cyc_Toc_check_const_array((
unsigned int)_tmp2C5,vtype);_LL122: {struct Cyc_Absyn_Primop_e_struct*_tmp2C6=(
struct Cyc_Absyn_Primop_e_struct*)_tmp2A6;if(_tmp2C6->tag != 3)goto _LL124;else{
_tmp2C7=_tmp2C6->f1;if(_tmp2C7 != Cyc_Absyn_Numelts)goto _LL124;_tmp2C8=_tmp2C6->f2;
if(_tmp2C8 == 0)goto _LL124;_tmp2C9=*_tmp2C8;_tmp2CA=(struct Cyc_Absyn_Exp*)_tmp2C9.hd;}}
_LL123:{void*_tmp2CB=_tmp2CA->r;void*_tmp2CD;struct Cyc_Absyn_Vardecl*_tmp2CF;
void*_tmp2D1;struct Cyc_Absyn_Vardecl*_tmp2D3;void*_tmp2D5;struct Cyc_Absyn_Vardecl*
_tmp2D7;void*_tmp2D9;struct Cyc_Absyn_Vardecl*_tmp2DB;_LL127: {struct Cyc_Absyn_Var_e_struct*
_tmp2CC=(struct Cyc_Absyn_Var_e_struct*)_tmp2CB;if(_tmp2CC->tag != 1)goto _LL129;
else{_tmp2CD=(void*)_tmp2CC->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2CE=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2CD;if(_tmp2CE->tag != 5)goto _LL129;else{_tmp2CF=_tmp2CE->f1;}};}}_LL128:
_tmp2D3=_tmp2CF;goto _LL12A;_LL129: {struct Cyc_Absyn_Var_e_struct*_tmp2D0=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CB;if(_tmp2D0->tag != 1)goto _LL12B;else{_tmp2D1=(
void*)_tmp2D0->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2D2=(struct Cyc_Absyn_Local_b_struct*)
_tmp2D1;if(_tmp2D2->tag != 4)goto _LL12B;else{_tmp2D3=_tmp2D2->f1;}};}}_LL12A:
_tmp2D7=_tmp2D3;goto _LL12C;_LL12B: {struct Cyc_Absyn_Var_e_struct*_tmp2D4=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CB;if(_tmp2D4->tag != 1)goto _LL12D;else{_tmp2D5=(
void*)_tmp2D4->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2D6=(struct Cyc_Absyn_Global_b_struct*)
_tmp2D5;if(_tmp2D6->tag != 1)goto _LL12D;else{_tmp2D7=_tmp2D6->f1;}};}}_LL12C:
_tmp2DB=_tmp2D7;goto _LL12E;_LL12D: {struct Cyc_Absyn_Var_e_struct*_tmp2D8=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CB;if(_tmp2D8->tag != 1)goto _LL12F;else{_tmp2D9=(
void*)_tmp2D8->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2DA=(struct Cyc_Absyn_Param_b_struct*)
_tmp2D9;if(_tmp2DA->tag != 3)goto _LL12F;else{_tmp2DB=_tmp2DA->f1;}};}}_LL12E:
return _tmp2DB == v;_LL12F:;_LL130: goto _LL126;_LL126:;}goto _LL113;_LL124:;_LL125:
goto _LL113;_LL113:;}return 0;}static char _tmp2ED[8]="*bogus*";struct _tuple17{void*
f1;void*f2;};static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i);static int Cyc_Toc_check_bounds(void*
a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
struct Cyc_Absyn_Vardecl*x;{void*_tmp2DC=a->r;void*_tmp2DE;struct Cyc_Absyn_Vardecl*
_tmp2E0;void*_tmp2E2;struct Cyc_Absyn_Vardecl*_tmp2E4;void*_tmp2E6;struct Cyc_Absyn_Vardecl*
_tmp2E8;void*_tmp2EA;struct Cyc_Absyn_Vardecl*_tmp2EC;_LL132: {struct Cyc_Absyn_Var_e_struct*
_tmp2DD=(struct Cyc_Absyn_Var_e_struct*)_tmp2DC;if(_tmp2DD->tag != 1)goto _LL134;
else{_tmp2DE=(void*)_tmp2DD->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2DF=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2DE;if(_tmp2DF->tag != 5)goto _LL134;else{_tmp2E0=_tmp2DF->f1;}};}}_LL133:
_tmp2E4=_tmp2E0;goto _LL135;_LL134: {struct Cyc_Absyn_Var_e_struct*_tmp2E1=(struct
Cyc_Absyn_Var_e_struct*)_tmp2DC;if(_tmp2E1->tag != 1)goto _LL136;else{_tmp2E2=(
void*)_tmp2E1->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2E3=(struct Cyc_Absyn_Local_b_struct*)
_tmp2E2;if(_tmp2E3->tag != 4)goto _LL136;else{_tmp2E4=_tmp2E3->f1;}};}}_LL135:
_tmp2E8=_tmp2E4;goto _LL137;_LL136: {struct Cyc_Absyn_Var_e_struct*_tmp2E5=(struct
Cyc_Absyn_Var_e_struct*)_tmp2DC;if(_tmp2E5->tag != 1)goto _LL138;else{_tmp2E6=(
void*)_tmp2E5->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2E7=(struct Cyc_Absyn_Global_b_struct*)
_tmp2E6;if(_tmp2E7->tag != 1)goto _LL138;else{_tmp2E8=_tmp2E7->f1;}};}}_LL137:
_tmp2EC=_tmp2E8;goto _LL139;_LL138: {struct Cyc_Absyn_Var_e_struct*_tmp2E9=(struct
Cyc_Absyn_Var_e_struct*)_tmp2DC;if(_tmp2E9->tag != 1)goto _LL13A;else{_tmp2EA=(
void*)_tmp2E9->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2EB=(struct Cyc_Absyn_Param_b_struct*)
_tmp2EA;if(_tmp2EB->tag != 3)goto _LL13A;else{_tmp2EC=_tmp2EB->f1;}};}}_LL139: if(
_tmp2EC->escapes)goto _LL13B;x=_tmp2EC;goto _LL131;_LL13A:;_LL13B: {static struct
_dyneither_ptr bogus_string={_tmp2ED,_tmp2ED,_tmp2ED + 8};static struct _tuple0
bogus_qvar={{.Loc_n={4,0}},& bogus_string};static struct Cyc_Absyn_Vardecl
bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,
0,0,0,0};x=& bogus_vardecl;x->type=a_typ;}_LL131:;}{void*_tmp2EE=a_typ;inner_loop: {
void*_tmp2EF=i->r;void*_tmp2F1;struct Cyc_Absyn_Exp*_tmp2F2;union Cyc_Absyn_Cnst
_tmp2F4;struct _tuple5 _tmp2F5;enum Cyc_Absyn_Sign _tmp2F6;int _tmp2F7;union Cyc_Absyn_Cnst
_tmp2F9;struct _tuple5 _tmp2FA;enum Cyc_Absyn_Sign _tmp2FB;int _tmp2FC;union Cyc_Absyn_Cnst
_tmp2FE;struct _tuple5 _tmp2FF;enum Cyc_Absyn_Sign _tmp300;int _tmp301;enum Cyc_Absyn_Primop
_tmp303;struct Cyc_List_List*_tmp304;struct Cyc_List_List _tmp305;struct Cyc_Absyn_Exp*
_tmp306;struct Cyc_List_List*_tmp307;struct Cyc_List_List _tmp308;struct Cyc_Absyn_Exp*
_tmp309;void*_tmp30B;struct Cyc_Absyn_Vardecl*_tmp30D;void*_tmp30F;struct Cyc_Absyn_Vardecl*
_tmp311;void*_tmp313;struct Cyc_Absyn_Vardecl*_tmp315;void*_tmp317;struct Cyc_Absyn_Vardecl*
_tmp319;_LL13D: {struct Cyc_Absyn_Cast_e_struct*_tmp2F0=(struct Cyc_Absyn_Cast_e_struct*)
_tmp2EF;if(_tmp2F0->tag != 15)goto _LL13F;else{_tmp2F1=(void*)_tmp2F0->f1;_tmp2F2=
_tmp2F0->f2;}}_LL13E: i=_tmp2F2;goto inner_loop;_LL13F: {struct Cyc_Absyn_Const_e_struct*
_tmp2F3=(struct Cyc_Absyn_Const_e_struct*)_tmp2EF;if(_tmp2F3->tag != 0)goto _LL141;
else{_tmp2F4=_tmp2F3->f1;if((_tmp2F4.Int_c).tag != 4)goto _LL141;_tmp2F5=(struct
_tuple5)(_tmp2F4.Int_c).val;_tmp2F6=_tmp2F5.f1;if(_tmp2F6 != Cyc_Absyn_None)goto
_LL141;_tmp2F7=_tmp2F5.f2;}}_LL140: _tmp2FC=_tmp2F7;goto _LL142;_LL141: {struct Cyc_Absyn_Const_e_struct*
_tmp2F8=(struct Cyc_Absyn_Const_e_struct*)_tmp2EF;if(_tmp2F8->tag != 0)goto _LL143;
else{_tmp2F9=_tmp2F8->f1;if((_tmp2F9.Int_c).tag != 4)goto _LL143;_tmp2FA=(struct
_tuple5)(_tmp2F9.Int_c).val;_tmp2FB=_tmp2FA.f1;if(_tmp2FB != Cyc_Absyn_Signed)
goto _LL143;_tmp2FC=_tmp2FA.f2;}}_LL142: return _tmp2FC >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp2FC + 1),_tmp2EE);_LL143: {struct Cyc_Absyn_Const_e_struct*
_tmp2FD=(struct Cyc_Absyn_Const_e_struct*)_tmp2EF;if(_tmp2FD->tag != 0)goto _LL145;
else{_tmp2FE=_tmp2FD->f1;if((_tmp2FE.Int_c).tag != 4)goto _LL145;_tmp2FF=(struct
_tuple5)(_tmp2FE.Int_c).val;_tmp300=_tmp2FF.f1;if(_tmp300 != Cyc_Absyn_Unsigned)
goto _LL145;_tmp301=_tmp2FF.f2;}}_LL144: return Cyc_Toc_check_const_array((
unsigned int)(_tmp301 + 1),_tmp2EE);_LL145: {struct Cyc_Absyn_Primop_e_struct*
_tmp302=(struct Cyc_Absyn_Primop_e_struct*)_tmp2EF;if(_tmp302->tag != 3)goto _LL147;
else{_tmp303=_tmp302->f1;if(_tmp303 != Cyc_Absyn_Mod)goto _LL147;_tmp304=_tmp302->f2;
if(_tmp304 == 0)goto _LL147;_tmp305=*_tmp304;_tmp306=(struct Cyc_Absyn_Exp*)_tmp305.hd;
_tmp307=_tmp305.tl;if(_tmp307 == 0)goto _LL147;_tmp308=*_tmp307;_tmp309=(struct Cyc_Absyn_Exp*)
_tmp308.hd;}}_LL146: return Cyc_Toc_check_leq_size(relns,x,_tmp309,_tmp2EE);_LL147: {
struct Cyc_Absyn_Var_e_struct*_tmp30A=(struct Cyc_Absyn_Var_e_struct*)_tmp2EF;if(
_tmp30A->tag != 1)goto _LL149;else{_tmp30B=(void*)_tmp30A->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp30C=(struct Cyc_Absyn_Pat_b_struct*)_tmp30B;if(_tmp30C->tag != 5)goto _LL149;
else{_tmp30D=_tmp30C->f1;}};}}_LL148: _tmp311=_tmp30D;goto _LL14A;_LL149: {struct
Cyc_Absyn_Var_e_struct*_tmp30E=(struct Cyc_Absyn_Var_e_struct*)_tmp2EF;if(_tmp30E->tag
!= 1)goto _LL14B;else{_tmp30F=(void*)_tmp30E->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp310=(struct Cyc_Absyn_Local_b_struct*)_tmp30F;if(_tmp310->tag != 4)goto _LL14B;
else{_tmp311=_tmp310->f1;}};}}_LL14A: _tmp315=_tmp311;goto _LL14C;_LL14B: {struct
Cyc_Absyn_Var_e_struct*_tmp312=(struct Cyc_Absyn_Var_e_struct*)_tmp2EF;if(_tmp312->tag
!= 1)goto _LL14D;else{_tmp313=(void*)_tmp312->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp314=(struct Cyc_Absyn_Global_b_struct*)_tmp313;if(_tmp314->tag != 1)goto _LL14D;
else{_tmp315=_tmp314->f1;}};}}_LL14C: _tmp319=_tmp315;goto _LL14E;_LL14D: {struct
Cyc_Absyn_Var_e_struct*_tmp316=(struct Cyc_Absyn_Var_e_struct*)_tmp2EF;if(_tmp316->tag
!= 1)goto _LL14F;else{_tmp317=(void*)_tmp316->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp318=(struct Cyc_Absyn_Param_b_struct*)_tmp317;if(_tmp318->tag != 3)goto _LL14F;
else{_tmp319=_tmp318->f1;}};}}_LL14E: if(_tmp319->escapes)return 0;{struct Cyc_List_List*
_tmp31A=relns;for(0;_tmp31A != 0;_tmp31A=_tmp31A->tl){struct Cyc_CfFlowInfo_Reln*
_tmp31B=(struct Cyc_CfFlowInfo_Reln*)_tmp31A->hd;if(_tmp31B->vd == _tmp319){union
Cyc_CfFlowInfo_RelnOp _tmp31C=_tmp31B->rop;struct Cyc_Absyn_Vardecl*_tmp31D;struct
_tuple11 _tmp31E;struct Cyc_Absyn_Vardecl*_tmp31F;void*_tmp320;unsigned int _tmp321;
unsigned int _tmp322;_LL152: if((_tmp31C.LessNumelts).tag != 3)goto _LL154;_tmp31D=(
struct Cyc_Absyn_Vardecl*)(_tmp31C.LessNumelts).val;_LL153: if(x == _tmp31D)return 1;
else{goto _LL151;}_LL154: if((_tmp31C.LessVar).tag != 2)goto _LL156;_tmp31E=(struct
_tuple11)(_tmp31C.LessVar).val;_tmp31F=_tmp31E.f1;_tmp320=_tmp31E.f2;_LL155:{
struct _tuple17 _tmpB2B;struct _tuple17 _tmp324=(_tmpB2B.f1=Cyc_Tcutil_compress(
_tmp320),((_tmpB2B.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpB2B)));void*_tmp325;void*_tmp327;void*_tmp328;
struct Cyc_Absyn_PtrInfo _tmp32A;struct Cyc_Absyn_PtrAtts _tmp32B;union Cyc_Absyn_Constraint*
_tmp32C;_LL15D: _tmp325=_tmp324.f1;{struct Cyc_Absyn_TagType_struct*_tmp326=(
struct Cyc_Absyn_TagType_struct*)_tmp325;if(_tmp326->tag != 20)goto _LL15F;else{
_tmp327=(void*)_tmp326->f1;}};_tmp328=_tmp324.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp329=(struct Cyc_Absyn_PointerType_struct*)_tmp328;if(_tmp329->tag != 5)goto
_LL15F;else{_tmp32A=_tmp329->f1;_tmp32B=_tmp32A.ptr_atts;_tmp32C=_tmp32B.bounds;}};
_LL15E:{void*_tmp32D=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp32C);struct Cyc_Absyn_Exp*_tmp32F;_LL162: {struct Cyc_Absyn_Upper_b_struct*
_tmp32E=(struct Cyc_Absyn_Upper_b_struct*)_tmp32D;if(_tmp32E->tag != 1)goto _LL164;
else{_tmp32F=_tmp32E->f1;}}_LL163: {struct Cyc_Absyn_Exp*_tmp330=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp327,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp330,_tmp32F))return 1;goto _LL161;}_LL164:;_LL165:
goto _LL161;_LL161:;}goto _LL15C;_LL15F:;_LL160: goto _LL15C;_LL15C:;}{struct Cyc_List_List*
_tmp331=relns;for(0;_tmp331 != 0;_tmp331=_tmp331->tl){struct Cyc_CfFlowInfo_Reln*
_tmp332=(struct Cyc_CfFlowInfo_Reln*)_tmp331->hd;if(_tmp332->vd == _tmp31F){union
Cyc_CfFlowInfo_RelnOp _tmp333=_tmp332->rop;struct Cyc_Absyn_Vardecl*_tmp334;struct
Cyc_Absyn_Vardecl*_tmp335;unsigned int _tmp336;unsigned int _tmp337;struct _tuple11
_tmp338;struct Cyc_Absyn_Vardecl*_tmp339;_LL167: if((_tmp333.LessEqNumelts).tag != 
5)goto _LL169;_tmp334=(struct Cyc_Absyn_Vardecl*)(_tmp333.LessEqNumelts).val;
_LL168: _tmp335=_tmp334;goto _LL16A;_LL169: if((_tmp333.LessNumelts).tag != 3)goto
_LL16B;_tmp335=(struct Cyc_Absyn_Vardecl*)(_tmp333.LessNumelts).val;_LL16A: if(x == 
_tmp335)return 1;goto _LL166;_LL16B: if((_tmp333.EqualConst).tag != 1)goto _LL16D;
_tmp336=(unsigned int)(_tmp333.EqualConst).val;_LL16C: return Cyc_Toc_check_const_array(
_tmp336,_tmp2EE);_LL16D: if((_tmp333.LessEqConst).tag != 6)goto _LL16F;_tmp337=(
unsigned int)(_tmp333.LessEqConst).val;if(!(_tmp337 > 0))goto _LL16F;_LL16E: return
Cyc_Toc_check_const_array(_tmp337,_tmp2EE);_LL16F: if((_tmp333.LessVar).tag != 2)
goto _LL171;_tmp338=(struct _tuple11)(_tmp333.LessVar).val;_tmp339=_tmp338.f1;
_LL170: if(Cyc_Toc_check_leq_size_var(relns,x,_tmp339))return 1;goto _LL166;_LL171:;
_LL172: goto _LL166;_LL166:;}}}goto _LL151;_LL156: if((_tmp31C.LessConst).tag != 4)
goto _LL158;_tmp321=(unsigned int)(_tmp31C.LessConst).val;_LL157: return Cyc_Toc_check_const_array(
_tmp321,_tmp2EE);_LL158: if((_tmp31C.LessEqConst).tag != 6)goto _LL15A;_tmp322=(
unsigned int)(_tmp31C.LessEqConst).val;_LL159: return Cyc_Toc_check_const_array(
_tmp322 + 1,_tmp2EE);_LL15A:;_LL15B: goto _LL151;_LL151:;}}}goto _LL13C;_LL14F:;
_LL150: goto _LL13C;_LL13C:;}return 0;};}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*
e);static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*
_tmpB2E;void*_tmpB2D;(_tmpB2D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2E="Missing type in primop ",
_tag_dyneither(_tmpB2E,sizeof(char),24))),_tag_dyneither(_tmpB2D,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*_tmpB31;void*_tmpB30;(_tmpB30=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB31="Missing type in primop ",_tag_dyneither(_tmpB31,sizeof(char),24))),
_tag_dyneither(_tmpB30,sizeof(void*),0)));}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static struct _tuple9*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*
e);static struct _tuple9*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple9*
_tmpB32;return(_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32->f1=Cyc_Toc_mt_tq,((
_tmpB32->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmpB32)))));}static struct _tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e);static struct _tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);{struct _tuple15*_tmpB33;return(
_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33->f1=0,((_tmpB33->f2=e,_tmpB33)))));};}
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple0*
x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,
int is_atomic);static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,
struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*
rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp340=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp340): Cyc_Toc_malloc_ptr(
_tmp340));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)rgnopt;Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp340);}};}else{t=
struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*
dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
dles0)- 1;{struct Cyc_List_List*_tmp341=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(dles0);for(0;_tmp341 != 0;_tmp341=_tmp341->tl){struct _tuple15
_tmp343;struct Cyc_List_List*_tmp344;struct Cyc_Absyn_Exp*_tmp345;struct _tuple15*
_tmp342=(struct _tuple15*)_tmp341->hd;_tmp343=*_tmp342;_tmp344=_tmp343.f1;_tmp345=
_tmp343.f2;{struct Cyc_Absyn_Exp*e_index;if(_tmp344 == 0)e_index=Cyc_Absyn_signed_int_exp(
count --,0);else{if(_tmp344->tl != 0){const char*_tmpB36;void*_tmpB35;(_tmpB35=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB36="multiple designators in array",
_tag_dyneither(_tmpB36,sizeof(char),30))),_tag_dyneither(_tmpB35,sizeof(void*),0)));}{
void*_tmp348=(void*)_tmp344->hd;void*_tmp349=_tmp348;struct Cyc_Absyn_Exp*_tmp34B;
_LL174: {struct Cyc_Absyn_ArrayElement_struct*_tmp34A=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp349;if(_tmp34A->tag != 0)goto _LL176;else{_tmp34B=_tmp34A->f1;}}_LL175: Cyc_Toc_exp_to_c(
nv,_tmp34B);e_index=_tmp34B;goto _LL173;_LL176: {struct Cyc_Absyn_FieldName_struct*
_tmp34C=(struct Cyc_Absyn_FieldName_struct*)_tmp349;if(_tmp34C->tag != 1)goto
_LL173;}_LL177: {const char*_tmpB39;void*_tmpB38;(_tmpB38=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB39="field name designators in array",
_tag_dyneither(_tmpB39,sizeof(char),32))),_tag_dyneither(_tmpB38,sizeof(void*),0)));}
_LL173:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp34F=_tmp345->r;struct Cyc_List_List*_tmp351;struct Cyc_Absyn_Vardecl*_tmp353;
struct Cyc_Absyn_Exp*_tmp354;struct Cyc_Absyn_Exp*_tmp355;int _tmp356;void*_tmp358;
struct Cyc_List_List*_tmp359;_LL179: {struct Cyc_Absyn_Array_e_struct*_tmp350=(
struct Cyc_Absyn_Array_e_struct*)_tmp34F;if(_tmp350->tag != 27)goto _LL17B;else{
_tmp351=_tmp350->f1;}}_LL17A: s=Cyc_Toc_init_array(nv,lval,_tmp351,s);goto _LL178;
_LL17B: {struct Cyc_Absyn_Comprehension_e_struct*_tmp352=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp34F;if(_tmp352->tag != 28)goto _LL17D;else{_tmp353=_tmp352->f1;_tmp354=_tmp352->f2;
_tmp355=_tmp352->f3;_tmp356=_tmp352->f4;}}_LL17C: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp353,_tmp354,_tmp355,_tmp356,s,0);goto _LL178;_LL17D: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp357=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp34F;if(_tmp357->tag != 30)goto
_LL17F;else{_tmp358=(void*)_tmp357->f1;_tmp359=_tmp357->f2;}}_LL17E: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp358,_tmp359,s);goto _LL178;_LL17F:;_LL180: Cyc_Toc_exp_to_c(nv,_tmp345);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp345,0),s,0);goto _LL178;_LL178:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple0*_tmp35A=vd->name;void*_tmp35B=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp35D;struct _RegionHandle*_tmp35E;
struct Cyc_Toc_Env*_tmp35C=nv;_tmp35D=*_tmp35C;_tmp35E=_tmp35D.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpB3C;struct Cyc_Absyn_Local_b_struct*_tmpB3B;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp35E,nv,_tmp35A,Cyc_Absyn_varb_exp(_tmp35A,(void*)((_tmpB3B=_cycalloc(sizeof(*
_tmpB3B)),((_tmpB3B[0]=((_tmpB3C.tag=4,((_tmpB3C.f1=vd,_tmpB3C)))),_tmpB3B)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp35A,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp35A,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp35A,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp35A,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp35F=e2->r;struct Cyc_List_List*_tmp361;
struct Cyc_Absyn_Vardecl*_tmp363;struct Cyc_Absyn_Exp*_tmp364;struct Cyc_Absyn_Exp*
_tmp365;int _tmp366;void*_tmp368;struct Cyc_List_List*_tmp369;_LL182: {struct Cyc_Absyn_Array_e_struct*
_tmp360=(struct Cyc_Absyn_Array_e_struct*)_tmp35F;if(_tmp360->tag != 27)goto _LL184;
else{_tmp361=_tmp360->f1;}}_LL183: body=Cyc_Toc_init_array(nv2,lval,_tmp361,Cyc_Toc_skip_stmt_dl());
goto _LL181;_LL184: {struct Cyc_Absyn_Comprehension_e_struct*_tmp362=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp35F;if(_tmp362->tag != 28)goto _LL186;else{_tmp363=_tmp362->f1;_tmp364=_tmp362->f2;
_tmp365=_tmp362->f3;_tmp366=_tmp362->f4;}}_LL185: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp363,_tmp364,_tmp365,_tmp366,Cyc_Toc_skip_stmt_dl(),0);goto _LL181;
_LL186: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp367=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp35F;if(_tmp367->tag != 30)goto _LL188;else{_tmp368=(void*)_tmp367->f1;_tmp369=
_tmp367->f2;}}_LL187: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp368,_tmp369,Cyc_Toc_skip_stmt_dl());
goto _LL181;_LL188:;_LL189: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL181;_LL181:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp35B,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp35A,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp36C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp36C != 0;_tmp36C=_tmp36C->tl){struct _tuple15 _tmp36E;
struct Cyc_List_List*_tmp36F;struct Cyc_Absyn_Exp*_tmp370;struct _tuple15*_tmp36D=(
struct _tuple15*)_tmp36C->hd;_tmp36E=*_tmp36D;_tmp36F=_tmp36E.f1;_tmp370=_tmp36E.f2;
if(_tmp36F == 0){const char*_tmpB3F;void*_tmpB3E;(_tmpB3E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3F="empty designator list",
_tag_dyneither(_tmpB3F,sizeof(char),22))),_tag_dyneither(_tmpB3E,sizeof(void*),0)));}
if(_tmp36F->tl != 0){const char*_tmpB42;void*_tmpB41;(_tmpB41=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB42="too many designators in anonymous struct",
_tag_dyneither(_tmpB42,sizeof(char),41))),_tag_dyneither(_tmpB41,sizeof(void*),0)));}{
void*_tmp375=(void*)_tmp36F->hd;struct _dyneither_ptr*_tmp377;_LL18B: {struct Cyc_Absyn_FieldName_struct*
_tmp376=(struct Cyc_Absyn_FieldName_struct*)_tmp375;if(_tmp376->tag != 1)goto
_LL18D;else{_tmp377=_tmp376->f1;}}_LL18C: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp377,0);{void*_tmp378=_tmp370->r;struct Cyc_List_List*_tmp37A;struct Cyc_Absyn_Vardecl*
_tmp37C;struct Cyc_Absyn_Exp*_tmp37D;struct Cyc_Absyn_Exp*_tmp37E;int _tmp37F;void*
_tmp381;struct Cyc_List_List*_tmp382;_LL190: {struct Cyc_Absyn_Array_e_struct*
_tmp379=(struct Cyc_Absyn_Array_e_struct*)_tmp378;if(_tmp379->tag != 27)goto _LL192;
else{_tmp37A=_tmp379->f1;}}_LL191: s=Cyc_Toc_init_array(nv,lval,_tmp37A,s);goto
_LL18F;_LL192: {struct Cyc_Absyn_Comprehension_e_struct*_tmp37B=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp378;if(_tmp37B->tag != 28)goto _LL194;else{_tmp37C=_tmp37B->f1;_tmp37D=_tmp37B->f2;
_tmp37E=_tmp37B->f3;_tmp37F=_tmp37B->f4;}}_LL193: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp37C,_tmp37D,_tmp37E,_tmp37F,s,0);goto _LL18F;_LL194: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp380=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp378;if(_tmp380->tag != 30)goto
_LL196;else{_tmp381=(void*)_tmp380->f1;_tmp382=_tmp380->f2;}}_LL195: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp381,_tmp382,s);goto _LL18F;_LL196:;_LL197: Cyc_Toc_exp_to_c(nv,_tmp370);
if(Cyc_Toc_is_poly_field(struct_type,_tmp377))_tmp370=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp370);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp370,0),0),s,0);goto _LL18F;_LL18F:;}goto _LL18A;}_LL18D:;_LL18E: {const char*
_tmpB45;void*_tmpB44;(_tmpB44=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB45="array designator in struct",
_tag_dyneither(_tmpB45,sizeof(char),27))),_tag_dyneither(_tmpB44,sizeof(void*),0)));}
_LL18A:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es);static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,
struct Cyc_List_List*es){struct Cyc_Toc_Env _tmp386;struct _RegionHandle*_tmp387;
struct Cyc_Toc_Env*_tmp385=nv;_tmp386=*_tmp385;_tmp387=_tmp386.rgn;{struct Cyc_List_List*
_tmp388=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple9*(*f)(struct
Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp387,Cyc_Toc_tup_to_c,
es);void*_tmp389=Cyc_Toc_add_tuple_type(_tmp388);struct _tuple0*_tmp38A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp38B=Cyc_Absyn_var_exp(_tmp38A,0);struct Cyc_Absyn_Stmt*
_tmp38C=Cyc_Absyn_exp_stmt(_tmp38B,0);struct Cyc_Absyn_Exp*(*_tmp38D)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp38E=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp387,es);{int i=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp38E);for(0;_tmp38E != 0;(
_tmp38E=_tmp38E->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp38E->hd;
struct Cyc_Absyn_Exp*lval=_tmp38D(_tmp38B,Cyc_Absyn_fieldname(i),0);is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp38F=e->r;struct Cyc_List_List*_tmp391;struct Cyc_Absyn_Vardecl*_tmp393;
struct Cyc_Absyn_Exp*_tmp394;struct Cyc_Absyn_Exp*_tmp395;int _tmp396;_LL199: {
struct Cyc_Absyn_Array_e_struct*_tmp390=(struct Cyc_Absyn_Array_e_struct*)_tmp38F;
if(_tmp390->tag != 27)goto _LL19B;else{_tmp391=_tmp390->f1;}}_LL19A: _tmp38C=Cyc_Toc_init_array(
nv,lval,_tmp391,_tmp38C);goto _LL198;_LL19B: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp392=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp38F;if(_tmp392->tag != 28)
goto _LL19D;else{_tmp393=_tmp392->f1;_tmp394=_tmp392->f2;_tmp395=_tmp392->f3;
_tmp396=_tmp392->f4;}}_LL19C: _tmp38C=Cyc_Toc_init_comprehension(nv,lval,_tmp393,
_tmp394,_tmp395,_tmp396,_tmp38C,0);goto _LL198;_LL19D:;_LL19E: Cyc_Toc_exp_to_c(nv,
e);_tmp38C=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp38D(
_tmp38B,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp38C,0);goto _LL198;_LL198:;};}}
return Cyc_Toc_make_struct(nv,_tmp38A,_tmp389,_tmp38C,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f);static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,
struct _dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp397=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp397 != 0;_tmp397=_tmp397->tl){struct Cyc_Absyn_Aggrfield*
_tmp398=(struct Cyc_Absyn_Aggrfield*)_tmp397->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp398->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpB4D;void*_tmpB4C[1];const char*_tmpB4B;void*_tmpB4A;(
_tmpB4A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB4D.tag=0,((_tmpB4D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB4C[0]=& _tmpB4D,Cyc_aprintf(((_tmpB4B="get_member_offset %s failed",
_tag_dyneither(_tmpB4B,sizeof(char),28))),_tag_dyneither(_tmpB4C,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB4A,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn);static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp39D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp39E=Cyc_Absyn_var_exp(_tmp39D,0);struct Cyc_Absyn_Stmt*
_tmp39F=Cyc_Absyn_exp_stmt(_tmp39E,0);struct Cyc_Absyn_Exp*(*_tmp3A0)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp3A1=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp3A2=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp3A4;union Cyc_Absyn_AggrInfoU _tmp3A5;
_LL1A0: {struct Cyc_Absyn_AggrType_struct*_tmp3A3=(struct Cyc_Absyn_AggrType_struct*)
_tmp3A2;if(_tmp3A3->tag != 12)goto _LL1A2;else{_tmp3A4=_tmp3A3->f1;_tmp3A5=_tmp3A4.aggr_info;}}
_LL1A1: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3A5);goto _LL19F;_LL1A2:;_LL1A3: {
const char*_tmpB50;void*_tmpB4F;(_tmpB4F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB50="init_struct: bad struct type",
_tag_dyneither(_tmpB50,sizeof(char),29))),_tag_dyneither(_tmpB4F,sizeof(void*),0)));}
_LL19F:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp3A9;struct _RegionHandle*_tmp3AA;struct Cyc_Toc_Env*_tmp3A8=
nv;_tmp3A9=*_tmp3A8;_tmp3AA=_tmp3A9.rgn;{struct Cyc_List_List*_tmp3AB=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3AA,dles);for(0;
_tmp3AB != 0;_tmp3AB=_tmp3AB->tl){struct _tuple15 _tmp3AD;struct Cyc_List_List*
_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AF;struct _tuple15*_tmp3AC=(struct _tuple15*)
_tmp3AB->hd;_tmp3AD=*_tmp3AC;_tmp3AE=_tmp3AD.f1;_tmp3AF=_tmp3AD.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3AF->topt))->v);
if(_tmp3AE == 0){const char*_tmpB53;void*_tmpB52;(_tmpB52=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB53="empty designator list",
_tag_dyneither(_tmpB53,sizeof(char),22))),_tag_dyneither(_tmpB52,sizeof(void*),0)));}
if(_tmp3AE->tl != 0){struct _tuple0*_tmp3B2=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3B3=Cyc_Absyn_var_exp(_tmp3B2,0);for(0;_tmp3AE != 0;_tmp3AE=_tmp3AE->tl){void*
_tmp3B4=(void*)_tmp3AE->hd;struct _dyneither_ptr*_tmp3B6;_LL1A5: {struct Cyc_Absyn_FieldName_struct*
_tmp3B5=(struct Cyc_Absyn_FieldName_struct*)_tmp3B4;if(_tmp3B5->tag != 1)goto
_LL1A7;else{_tmp3B6=_tmp3B5->f1;}}_LL1A6: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3B6))_tmp3B3=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3B3);_tmp39F=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A0(_tmp39E,_tmp3B6,0),_tmp3B3,0),0),
_tmp39F,0);goto _LL1A4;_LL1A7:;_LL1A8: {const char*_tmpB56;void*_tmpB55;(_tmpB55=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB56="array designator in struct",_tag_dyneither(_tmpB56,sizeof(char),27))),
_tag_dyneither(_tmpB55,sizeof(void*),0)));}_LL1A4:;}Cyc_Toc_exp_to_c(nv,_tmp3AF);
_tmp39F=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3B3,
_tmp3AF,0),0),_tmp39F,0);}else{void*_tmp3B9=(void*)_tmp3AE->hd;struct
_dyneither_ptr*_tmp3BB;_LL1AA: {struct Cyc_Absyn_FieldName_struct*_tmp3BA=(struct
Cyc_Absyn_FieldName_struct*)_tmp3B9;if(_tmp3BA->tag != 1)goto _LL1AC;else{_tmp3BB=
_tmp3BA->f1;}}_LL1AB: {struct Cyc_Absyn_Exp*lval=_tmp3A0(_tmp39E,_tmp3BB,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp3BB);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp39F=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp39F,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp3BC=_tmp3AF->r;
struct Cyc_List_List*_tmp3BE;struct Cyc_Absyn_Vardecl*_tmp3C0;struct Cyc_Absyn_Exp*
_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;int _tmp3C3;void*_tmp3C5;struct Cyc_List_List*
_tmp3C6;_LL1AF: {struct Cyc_Absyn_Array_e_struct*_tmp3BD=(struct Cyc_Absyn_Array_e_struct*)
_tmp3BC;if(_tmp3BD->tag != 27)goto _LL1B1;else{_tmp3BE=_tmp3BD->f1;}}_LL1B0:
_tmp39F=Cyc_Toc_init_array(nv,lval,_tmp3BE,_tmp39F);goto _LL1AE;_LL1B1: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp3BF=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3BC;if(_tmp3BF->tag != 28)goto _LL1B3;else{_tmp3C0=_tmp3BF->f1;_tmp3C1=_tmp3BF->f2;
_tmp3C2=_tmp3BF->f3;_tmp3C3=_tmp3BF->f4;}}_LL1B2: _tmp39F=Cyc_Toc_init_comprehension(
nv,lval,_tmp3C0,_tmp3C1,_tmp3C2,_tmp3C3,_tmp39F,0);goto _LL1AE;_LL1B3: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3C4=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3BC;if(_tmp3C4->tag != 30)goto
_LL1B5;else{_tmp3C5=(void*)_tmp3C4->f1;_tmp3C6=_tmp3C4->f2;}}_LL1B4: _tmp39F=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3C5,_tmp3C6,_tmp39F);goto _LL1AE;_LL1B5:;_LL1B6: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3AF->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3AF);{struct Cyc_Absyn_Aggrfield*_tmp3C7=Cyc_Absyn_lookup_decl_field(ad,
_tmp3BB);if(Cyc_Toc_is_poly_field(struct_type,_tmp3BB) && !was_ptr_type)_tmp3AF=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3AF);if(has_exists)_tmp3AF=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3C7))->type),
_tmp3AF);_tmp39F=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp3AF,0),0),_tmp39F,0);goto _LL1AE;};}_LL1AE:;}goto _LL1A9;}_LL1AC:;_LL1AD: {
const char*_tmpB59;void*_tmpB58;(_tmpB58=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB59="array designator in struct",
_tag_dyneither(_tmpB59,sizeof(char),27))),_tag_dyneither(_tmpB58,sizeof(void*),0)));}
_LL1A9:;}}}return Cyc_Toc_make_struct(nv,_tmp39D,_tmp3A1,_tmp39F,pointer,rgnopt,
is_atomic);};}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple18*pr);static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*
el,struct _tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int
ignore);static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,
int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr);static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_struct _tmpB5C;struct Cyc_Absyn_Increment_e_struct*
_tmpB5B;return Cyc_Absyn_new_exp((void*)((_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((
_tmpB5B[0]=((_tmpB5C.tag=5,((_tmpB5C.f1=e1,((_tmpB5C.f2=incr,_tmpB5C)))))),
_tmpB5B)))),0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*
f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp3CC=e1->r;struct Cyc_Absyn_Stmt*
_tmp3CE;void*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D1;struct Cyc_Absyn_Exp*_tmp3D3;
struct _dyneither_ptr*_tmp3D4;int _tmp3D5;int _tmp3D6;_LL1B8: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp3CD=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp3CC;if(_tmp3CD->tag != 37)goto
_LL1BA;else{_tmp3CE=_tmp3CD->f1;}}_LL1B9: Cyc_Toc_lvalue_assign_stmt(_tmp3CE,fs,f,
f_env);goto _LL1B7;_LL1BA: {struct Cyc_Absyn_Cast_e_struct*_tmp3CF=(struct Cyc_Absyn_Cast_e_struct*)
_tmp3CC;if(_tmp3CF->tag != 15)goto _LL1BC;else{_tmp3D0=(void*)_tmp3CF->f1;_tmp3D1=
_tmp3CF->f2;}}_LL1BB: Cyc_Toc_lvalue_assign(_tmp3D1,fs,f,f_env);goto _LL1B7;_LL1BC: {
struct Cyc_Absyn_AggrMember_e_struct*_tmp3D2=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3CC;if(_tmp3D2->tag != 22)goto _LL1BE;else{_tmp3D3=_tmp3D2->f1;_tmp3D4=_tmp3D2->f2;
_tmp3D5=_tmp3D2->f3;_tmp3D6=_tmp3D2->f4;}}_LL1BD: e1->r=_tmp3D3->r;{struct Cyc_List_List*
_tmpB5D;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((_tmpB5D=_cycalloc(
sizeof(*_tmpB5D)),((_tmpB5D->hd=_tmp3D4,((_tmpB5D->tl=fs,_tmpB5D)))))),f,f_env);}
goto _LL1B7;_LL1BE:;_LL1BF: {struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct
_dyneither_ptr*)fs->hd,e1_copy->loc);}e1->r=(f(e1_copy,f_env))->r;goto _LL1B7;}
_LL1B7:;}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp3D8=s->r;
struct Cyc_Absyn_Exp*_tmp3DA;struct Cyc_Absyn_Decl*_tmp3DC;struct Cyc_Absyn_Stmt*
_tmp3DD;struct Cyc_Absyn_Stmt*_tmp3DF;_LL1C1: {struct Cyc_Absyn_Exp_s_struct*
_tmp3D9=(struct Cyc_Absyn_Exp_s_struct*)_tmp3D8;if(_tmp3D9->tag != 1)goto _LL1C3;
else{_tmp3DA=_tmp3D9->f1;}}_LL1C2: Cyc_Toc_lvalue_assign(_tmp3DA,fs,f,f_env);goto
_LL1C0;_LL1C3: {struct Cyc_Absyn_Decl_s_struct*_tmp3DB=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3D8;if(_tmp3DB->tag != 12)goto _LL1C5;else{_tmp3DC=_tmp3DB->f1;_tmp3DD=_tmp3DB->f2;}}
_LL1C4: Cyc_Toc_lvalue_assign_stmt(_tmp3DD,fs,f,f_env);goto _LL1C0;_LL1C5: {struct
Cyc_Absyn_Seq_s_struct*_tmp3DE=(struct Cyc_Absyn_Seq_s_struct*)_tmp3D8;if(_tmp3DE->tag
!= 2)goto _LL1C7;else{_tmp3DF=_tmp3DE->f2;}}_LL1C6: Cyc_Toc_lvalue_assign_stmt(
_tmp3DF,fs,f,f_env);goto _LL1C0;_LL1C7:;_LL1C8: {const char*_tmpB61;void*_tmpB60[1];
struct Cyc_String_pa_struct _tmpB5F;(_tmpB5F.tag=0,((_tmpB5F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB60[0]=&
_tmpB5F,Cyc_Toc_toc_impos(((_tmpB61="lvalue_assign_stmt: %s",_tag_dyneither(
_tmpB61,sizeof(char),23))),_tag_dyneither(_tmpB60,sizeof(void*),1)))))));}_LL1C0:;}
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(
struct Cyc_Absyn_Exp*e){void*_tmp3E3=e->r;void*_tmp3E5;void**_tmp3E6;struct Cyc_Absyn_Exp*
_tmp3E7;struct Cyc_Absyn_Exp**_tmp3E8;struct Cyc_Absyn_Exp*_tmp3EA;struct Cyc_Absyn_Stmt*
_tmp3EC;_LL1CA: {struct Cyc_Absyn_Cast_e_struct*_tmp3E4=(struct Cyc_Absyn_Cast_e_struct*)
_tmp3E3;if(_tmp3E4->tag != 15)goto _LL1CC;else{_tmp3E5=(void**)& _tmp3E4->f1;
_tmp3E6=(void**)((void**)& _tmp3E4->f1);_tmp3E7=_tmp3E4->f2;_tmp3E8=(struct Cyc_Absyn_Exp**)&
_tmp3E4->f2;}}_LL1CB:*_tmp3E8=Cyc_Toc_push_address_exp(*_tmp3E8);*_tmp3E6=Cyc_Absyn_cstar_typ(*
_tmp3E6,Cyc_Toc_mt_tq);return e;_LL1CC: {struct Cyc_Absyn_Deref_e_struct*_tmp3E9=(
struct Cyc_Absyn_Deref_e_struct*)_tmp3E3;if(_tmp3E9->tag != 21)goto _LL1CE;else{
_tmp3EA=_tmp3E9->f1;}}_LL1CD: return _tmp3EA;_LL1CE: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp3EB=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp3E3;if(_tmp3EB->tag != 37)goto
_LL1D0;else{_tmp3EC=_tmp3EB->f1;}}_LL1CF: Cyc_Toc_push_address_stmt(_tmp3EC);
return e;_LL1D0:;_LL1D1: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpB65;void*_tmpB64[1];struct Cyc_String_pa_struct _tmpB63;(_tmpB63.tag=
0,((_tmpB63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpB64[0]=& _tmpB63,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB65="can't take & of %s",_tag_dyneither(_tmpB65,
sizeof(char),19))),_tag_dyneither(_tmpB64,sizeof(void*),1)))))));};_LL1C9:;}
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_push_address_stmt(
struct Cyc_Absyn_Stmt*s){void*_tmp3F0=s->r;struct Cyc_Absyn_Exp*_tmp3F2;struct Cyc_Absyn_Exp**
_tmp3F3;struct Cyc_Absyn_Stmt*_tmp3F5;struct Cyc_Absyn_Stmt*_tmp3F7;_LL1D3: {
struct Cyc_Absyn_Exp_s_struct*_tmp3F1=(struct Cyc_Absyn_Exp_s_struct*)_tmp3F0;if(
_tmp3F1->tag != 1)goto _LL1D5;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=(struct Cyc_Absyn_Exp**)&
_tmp3F1->f1;}}_LL1D4:*_tmp3F3=Cyc_Toc_push_address_exp(*_tmp3F3);return;_LL1D5: {
struct Cyc_Absyn_Decl_s_struct*_tmp3F4=(struct Cyc_Absyn_Decl_s_struct*)_tmp3F0;
if(_tmp3F4->tag != 12)goto _LL1D7;else{_tmp3F5=_tmp3F4->f2;}}_LL1D6: _tmp3F7=
_tmp3F5;goto _LL1D8;_LL1D7: {struct Cyc_Absyn_Seq_s_struct*_tmp3F6=(struct Cyc_Absyn_Seq_s_struct*)
_tmp3F0;if(_tmp3F6->tag != 2)goto _LL1D9;else{_tmp3F7=_tmp3F6->f2;}}_LL1D8: Cyc_Toc_push_address_stmt(
_tmp3F7);return;_LL1D9:;_LL1DA: {const char*_tmpB69;void*_tmpB68[1];struct Cyc_String_pa_struct
_tmpB67;(_tmpB67.tag=0,((_tmpB67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpB68[0]=& _tmpB67,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB69="can't take & of statement-expression %s",
_tag_dyneither(_tmpB69,sizeof(char),40))),_tag_dyneither(_tmpB68,sizeof(void*),1)))))));}
_LL1D2:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_Toc_rmap_2c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*
_tmpB6A;result=((_tmpB6A=_region_malloc(r2,sizeof(*_tmpB6A)),((_tmpB6A->hd=(void*)
f((void*)x->hd,env),((_tmpB6A->tl=0,_tmpB6A))))));}prev=result;for(x=x->tl;x != 0;
x=x->tl){{struct Cyc_List_List*_tmpB6B;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpB6B=_region_malloc(r2,sizeof(*_tmpB6B)),((_tmpB6B->hd=(void*)f((void*)x->hd,
env),((_tmpB6B->tl=0,_tmpB6B))))));}prev=prev->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);static
struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}static struct _tuple15*Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp*e);static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){struct _tuple15*_tmpB6C;return(_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C->f1=
0,((_tmpB6C->f2=e,_tmpB6C)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void*t);static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp3FE=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp400;_LL1DC: {struct Cyc_Absyn_PointerType_struct*
_tmp3FF=(struct Cyc_Absyn_PointerType_struct*)_tmp3FE;if(_tmp3FF->tag != 5)goto
_LL1DE;else{_tmp400=_tmp3FF->f1;}}_LL1DD: return _tmp400;_LL1DE:;_LL1DF: {const
char*_tmpB6F;void*_tmpB6E;(_tmpB6E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6F="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpB6F,sizeof(char),28))),_tag_dyneither(_tmpB6E,sizeof(void*),0)));}
_LL1DB:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t);static struct Cyc_Absyn_Exp*
Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*res;{void*_tmp403=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Sign _tmp406;enum Cyc_Absyn_Size_of _tmp407;enum Cyc_Absyn_Sign
_tmp409;enum Cyc_Absyn_Size_of _tmp40A;enum Cyc_Absyn_Sign _tmp40E;enum Cyc_Absyn_Size_of
_tmp40F;enum Cyc_Absyn_Sign _tmp411;enum Cyc_Absyn_Size_of _tmp412;enum Cyc_Absyn_Sign
_tmp414;enum Cyc_Absyn_Size_of _tmp415;_LL1E1: {struct Cyc_Absyn_PointerType_struct*
_tmp404=(struct Cyc_Absyn_PointerType_struct*)_tmp403;if(_tmp404->tag != 5)goto
_LL1E3;}_LL1E2: res=Cyc_Absyn_null_exp(0);goto _LL1E0;_LL1E3: {struct Cyc_Absyn_IntType_struct*
_tmp405=(struct Cyc_Absyn_IntType_struct*)_tmp403;if(_tmp405->tag != 6)goto _LL1E5;
else{_tmp406=_tmp405->f1;_tmp407=_tmp405->f2;if(_tmp407 != Cyc_Absyn_Char_sz)goto
_LL1E5;}}_LL1E4: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp406,'\000'),0);goto
_LL1E0;_LL1E5: {struct Cyc_Absyn_IntType_struct*_tmp408=(struct Cyc_Absyn_IntType_struct*)
_tmp403;if(_tmp408->tag != 6)goto _LL1E7;else{_tmp409=_tmp408->f1;_tmp40A=_tmp408->f2;
if(_tmp40A != Cyc_Absyn_Short_sz)goto _LL1E7;}}_LL1E6: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp409,0),0);goto _LL1E0;_LL1E7: {struct Cyc_Absyn_EnumType_struct*_tmp40B=(
struct Cyc_Absyn_EnumType_struct*)_tmp403;if(_tmp40B->tag != 14)goto _LL1E9;}_LL1E8:
goto _LL1EA;_LL1E9: {struct Cyc_Absyn_AnonEnumType_struct*_tmp40C=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp403;if(_tmp40C->tag != 15)goto _LL1EB;}_LL1EA: _tmp40E=Cyc_Absyn_Unsigned;goto
_LL1EC;_LL1EB: {struct Cyc_Absyn_IntType_struct*_tmp40D=(struct Cyc_Absyn_IntType_struct*)
_tmp403;if(_tmp40D->tag != 6)goto _LL1ED;else{_tmp40E=_tmp40D->f1;_tmp40F=_tmp40D->f2;
if(_tmp40F != Cyc_Absyn_Int_sz)goto _LL1ED;}}_LL1EC: _tmp411=_tmp40E;goto _LL1EE;
_LL1ED: {struct Cyc_Absyn_IntType_struct*_tmp410=(struct Cyc_Absyn_IntType_struct*)
_tmp403;if(_tmp410->tag != 6)goto _LL1EF;else{_tmp411=_tmp410->f1;_tmp412=_tmp410->f2;
if(_tmp412 != Cyc_Absyn_Long_sz)goto _LL1EF;}}_LL1EE: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(
_tmp411,0),0);goto _LL1E0;_LL1EF: {struct Cyc_Absyn_IntType_struct*_tmp413=(struct
Cyc_Absyn_IntType_struct*)_tmp403;if(_tmp413->tag != 6)goto _LL1F1;else{_tmp414=
_tmp413->f1;_tmp415=_tmp413->f2;if(_tmp415 != Cyc_Absyn_LongLong_sz)goto _LL1F1;}}
_LL1F0: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp414,(long long)0),0);
goto _LL1E0;_LL1F1: {struct Cyc_Absyn_FloatType_struct*_tmp416=(struct Cyc_Absyn_FloatType_struct*)
_tmp403;if(_tmp416->tag != 7)goto _LL1F3;}_LL1F2: goto _LL1F4;_LL1F3: {struct Cyc_Absyn_DoubleType_struct*
_tmp417=(struct Cyc_Absyn_DoubleType_struct*)_tmp403;if(_tmp417->tag != 8)goto
_LL1F5;}_LL1F4:{const char*_tmpB70;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpB70="0.0",_tag_dyneither(_tmpB70,sizeof(char),4)))),0);}goto _LL1E0;_LL1F5:;
_LL1F6: {const char*_tmpB74;void*_tmpB73[1];struct Cyc_String_pa_struct _tmpB72;(
_tmpB72.tag=0,((_tmpB72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpB73[0]=& _tmpB72,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB74="found non-zero type %s in generate_zero",
_tag_dyneither(_tmpB74,sizeof(char),40))),_tag_dyneither(_tmpB73,sizeof(void*),1)))))));}
_LL1E0:;}{struct Cyc_Core_Opt*_tmpB75;res->topt=((_tmpB75=_cycalloc(sizeof(*
_tmpB75)),((_tmpB75->v=t,_tmpB75))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type);static
void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp41D=Cyc_Toc_typ_to_c(elt_type);void*_tmp41E=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp41F=Cyc_Absyn_cstar_typ(_tmp41D,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpB76;struct Cyc_Core_Opt*_tmp420=(_tmpB76=_cycalloc(sizeof(*_tmpB76)),((
_tmpB76->v=_tmp41F,_tmpB76)));struct Cyc_Absyn_Exp*xinit;{void*_tmp421=e1->r;
struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Exp*_tmp425;struct Cyc_Absyn_Exp*
_tmp426;_LL1F8: {struct Cyc_Absyn_Deref_e_struct*_tmp422=(struct Cyc_Absyn_Deref_e_struct*)
_tmp421;if(_tmp422->tag != 21)goto _LL1FA;else{_tmp423=_tmp422->f1;}}_LL1F9: if(!
is_dyneither){_tmp423=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp423,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpB77;_tmp423->topt=((_tmpB77=_cycalloc(sizeof(*_tmpB77)),((
_tmpB77->v=fat_ptr_type,_tmpB77))));};}Cyc_Toc_exp_to_c(nv,_tmp423);xinit=
_tmp423;goto _LL1F7;_LL1FA: {struct Cyc_Absyn_Subscript_e_struct*_tmp424=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp421;if(_tmp424->tag != 24)goto _LL1FC;else{
_tmp425=_tmp424->f1;_tmp426=_tmp424->f2;}}_LL1FB: if(!is_dyneither){_tmp425=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp425,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpB78;
_tmp425->topt=((_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78->v=fat_ptr_type,
_tmpB78))));};}Cyc_Toc_exp_to_c(nv,_tmp425);Cyc_Toc_exp_to_c(nv,_tmp426);{struct
Cyc_Absyn_Exp*_tmpB79[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpB79[2]=_tmp426,((_tmpB79[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpB79[0]=_tmp425,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB79,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1F7;_LL1FC:;_LL1FD: {const char*_tmpB7C;void*_tmpB7B;(_tmpB7B=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7C="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpB7C,sizeof(char),53))),_tag_dyneither(_tmpB7B,sizeof(void*),0)));}
_LL1F7:;}{struct _tuple0*_tmp42C=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp42E;
struct _RegionHandle*_tmp42F;struct Cyc_Toc_Env*_tmp42D=nv;_tmp42E=*_tmp42D;
_tmp42F=_tmp42E.rgn;{struct Cyc_Toc_Env*_tmp430=Cyc_Toc_add_varmap(_tmp42F,nv,
_tmp42C,Cyc_Absyn_var_exp(_tmp42C,0));struct Cyc_Absyn_Vardecl*_tmpB7D;struct Cyc_Absyn_Vardecl*
_tmp431=(_tmpB7D=_cycalloc(sizeof(*_tmpB7D)),((_tmpB7D->sc=Cyc_Absyn_Public,((
_tmpB7D->name=_tmp42C,((_tmpB7D->tq=Cyc_Toc_mt_tq,((_tmpB7D->type=_tmp41E,((
_tmpB7D->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpB7D->rgn=0,((_tmpB7D->attributes=
0,((_tmpB7D->escapes=0,_tmpB7D)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpB80;struct Cyc_Absyn_Local_b_struct*_tmpB7F;struct Cyc_Absyn_Local_b_struct*
_tmp432=(_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F[0]=((_tmpB80.tag=4,((
_tmpB80.f1=_tmp431,_tmpB80)))),_tmpB7F)));struct Cyc_Absyn_Exp*_tmp433=Cyc_Absyn_varb_exp(
_tmp42C,(void*)_tmp432,0);{struct Cyc_Core_Opt*_tmpB81;_tmp433->topt=((_tmpB81=
_cycalloc(sizeof(*_tmpB81)),((_tmpB81->v=fat_ptr_type,_tmpB81))));}{struct Cyc_Absyn_Exp*
_tmp435=Cyc_Absyn_deref_exp(_tmp433,0);{struct Cyc_Core_Opt*_tmpB82;_tmp435->topt=((
_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82->v=elt_type,_tmpB82))));}Cyc_Toc_exp_to_c(
_tmp430,_tmp435);{struct _tuple0*_tmp437=Cyc_Toc_temp_var();_tmp430=Cyc_Toc_add_varmap(
_tmp42F,_tmp430,_tmp437,Cyc_Absyn_var_exp(_tmp437,0));{struct Cyc_Absyn_Vardecl*
_tmpB83;struct Cyc_Absyn_Vardecl*_tmp438=(_tmpB83=_cycalloc(sizeof(*_tmpB83)),((
_tmpB83->sc=Cyc_Absyn_Public,((_tmpB83->name=_tmp437,((_tmpB83->tq=Cyc_Toc_mt_tq,((
_tmpB83->type=_tmp41D,((_tmpB83->initializer=(struct Cyc_Absyn_Exp*)_tmp435,((
_tmpB83->rgn=0,((_tmpB83->attributes=0,((_tmpB83->escapes=0,_tmpB83)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB86;struct Cyc_Absyn_Local_b_struct*_tmpB85;
struct Cyc_Absyn_Local_b_struct*_tmp439=(_tmpB85=_cycalloc(sizeof(*_tmpB85)),((
_tmpB85[0]=((_tmpB86.tag=4,((_tmpB86.f1=_tmp438,_tmpB86)))),_tmpB85)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp43A=Cyc_Absyn_varb_exp(_tmp437,(
void*)_tmp439,0);_tmp43A->topt=_tmp435->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp43A,e2,0);z_init->topt=_tmp43A->topt;}Cyc_Toc_exp_to_c(
_tmp430,z_init);{struct _tuple0*_tmp43B=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpB87;struct Cyc_Absyn_Vardecl*_tmp43C=(_tmpB87=_cycalloc(sizeof(*_tmpB87)),((
_tmpB87->sc=Cyc_Absyn_Public,((_tmpB87->name=_tmp43B,((_tmpB87->tq=Cyc_Toc_mt_tq,((
_tmpB87->type=_tmp41D,((_tmpB87->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpB87->rgn=0,((_tmpB87->attributes=0,((_tmpB87->escapes=0,_tmpB87)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB8A;struct Cyc_Absyn_Local_b_struct*_tmpB89;
struct Cyc_Absyn_Local_b_struct*_tmp43D=(_tmpB89=_cycalloc(sizeof(*_tmpB89)),((
_tmpB89[0]=((_tmpB8A.tag=4,((_tmpB8A.f1=_tmp43C,_tmpB8A)))),_tmpB89)));_tmp430=
Cyc_Toc_add_varmap(_tmp42F,_tmp430,_tmp43B,Cyc_Absyn_var_exp(_tmp43B,0));{struct
Cyc_Absyn_Exp*_tmp43E=Cyc_Absyn_varb_exp(_tmp437,(void*)_tmp439,0);_tmp43E->topt=
_tmp435->topt;{struct Cyc_Absyn_Exp*_tmp43F=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp440=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp43E,_tmp43F,0);{
struct Cyc_Core_Opt*_tmpB8B;_tmp440->topt=((_tmpB8B=_cycalloc(sizeof(*_tmpB8B)),((
_tmpB8B->v=Cyc_Absyn_sint_typ,_tmpB8B))));}Cyc_Toc_exp_to_c(_tmp430,_tmp440);{
struct Cyc_Absyn_Exp*_tmp442=Cyc_Absyn_varb_exp(_tmp43B,(void*)_tmp43D,0);_tmp442->topt=
_tmp435->topt;{struct Cyc_Absyn_Exp*_tmp443=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp444=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp442,_tmp443,0);{
struct Cyc_Core_Opt*_tmpB8C;_tmp444->topt=((_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((
_tmpB8C->v=Cyc_Absyn_sint_typ,_tmpB8C))));}Cyc_Toc_exp_to_c(_tmp430,_tmp444);{
struct Cyc_Absyn_Exp*_tmpB8D[2];struct Cyc_List_List*_tmp446=(_tmpB8D[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpB8D[0]=Cyc_Absyn_varb_exp(_tmp42C,(void*)
_tmp432,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB8D,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp447=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp446,0),
_tmp447,0);{struct Cyc_Absyn_Exp*_tmp448=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp440,_tmp444,0),0);struct Cyc_Absyn_Stmt*_tmp449=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp44A=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp42C,(void*)_tmp432,0),Cyc_Toc_curr_sp,0);_tmp44A=Cyc_Toc_cast_it(
_tmp41F,_tmp44A);{struct Cyc_Absyn_Exp*_tmp44B=Cyc_Absyn_deref_exp(_tmp44A,0);
struct Cyc_Absyn_Exp*_tmp44C=Cyc_Absyn_assign_exp(_tmp44B,Cyc_Absyn_var_exp(
_tmp43B,0),0);struct Cyc_Absyn_Stmt*_tmp44D=Cyc_Absyn_exp_stmt(_tmp44C,0);_tmp44D=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp448,_tmp449,Cyc_Absyn_skip_stmt(
0),0),_tmp44D,0);{struct Cyc_Absyn_Var_d_struct*_tmpB93;struct Cyc_Absyn_Var_d_struct
_tmpB92;struct Cyc_Absyn_Decl*_tmpB91;_tmp44D=Cyc_Absyn_decl_stmt(((_tmpB91=
_cycalloc(sizeof(*_tmpB91)),((_tmpB91->r=(void*)((_tmpB93=_cycalloc(sizeof(*
_tmpB93)),((_tmpB93[0]=((_tmpB92.tag=0,((_tmpB92.f1=_tmp43C,_tmpB92)))),_tmpB93)))),((
_tmpB91->loc=0,_tmpB91)))))),_tmp44D,0);}{struct Cyc_Absyn_Var_d_struct*_tmpB99;
struct Cyc_Absyn_Var_d_struct _tmpB98;struct Cyc_Absyn_Decl*_tmpB97;_tmp44D=Cyc_Absyn_decl_stmt(((
_tmpB97=_cycalloc(sizeof(*_tmpB97)),((_tmpB97->r=(void*)((_tmpB99=_cycalloc(
sizeof(*_tmpB99)),((_tmpB99[0]=((_tmpB98.tag=0,((_tmpB98.f1=_tmp438,_tmpB98)))),
_tmpB99)))),((_tmpB97->loc=0,_tmpB97)))))),_tmp44D,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpB9F;struct Cyc_Absyn_Var_d_struct _tmpB9E;struct Cyc_Absyn_Decl*_tmpB9D;_tmp44D=
Cyc_Absyn_decl_stmt(((_tmpB9D=_cycalloc(sizeof(*_tmpB9D)),((_tmpB9D->r=(void*)((
_tmpB9F=_cycalloc(sizeof(*_tmpB9F)),((_tmpB9F[0]=((_tmpB9E.tag=0,((_tmpB9E.f1=
_tmp431,_tmpB9E)))),_tmpB9F)))),((_tmpB9D->loc=0,_tmpB9D)))))),_tmp44D,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp44D);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,
struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*));static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*
e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*
aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)){
struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp462=Cyc_Tcutil_compress(aggrtype);struct
Cyc_Absyn_AggrInfo _tmp464;union Cyc_Absyn_AggrInfoU _tmp465;_LL1FF: {struct Cyc_Absyn_AggrType_struct*
_tmp463=(struct Cyc_Absyn_AggrType_struct*)_tmp462;if(_tmp463->tag != 12)goto
_LL201;else{_tmp464=_tmp463->f1;_tmp465=_tmp464.aggr_info;}}_LL200: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp465);goto _LL1FE;_LL201:;_LL202: {struct Cyc_String_pa_struct _tmpBA7;void*
_tmpBA6[1];const char*_tmpBA5;void*_tmpBA4;(_tmpBA4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpBA7.tag=0,((_tmpBA7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpBA6[0]=& _tmpBA7,Cyc_aprintf(((
_tmpBA5="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpBA5,sizeof(char),51))),_tag_dyneither(_tmpBA6,sizeof(void*),1)))))))),
_tag_dyneither(_tmpBA4,sizeof(void*),0)));}_LL1FE:;}{struct _tuple0*_tmp46A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp46B=Cyc_Absyn_var_exp(_tmp46A,0);struct Cyc_Absyn_Exp*
_tmp46C=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp46B,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp46D=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
struct Cyc_Absyn_Exp*_tmp46E=Cyc_Absyn_neq_exp(_tmp46C,_tmp46D,0);struct Cyc_Absyn_Exp*
_tmp46F=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp46B,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp470=Cyc_Absyn_exp_stmt(_tmp46F,0);struct Cyc_Absyn_Stmt*
_tmp471=Cyc_Absyn_ifthenelse_stmt(_tmp46E,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp472=Cyc_Absyn_declare_stmt(_tmp46A,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp471,_tmp470,0),0);return Cyc_Toc_stmt_exp_r(
_tmp472);};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read);static int Cyc_Toc_is_tagged_union_project(
struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){void*
_tmp473=e->r;struct Cyc_Absyn_Exp*_tmp475;struct Cyc_Absyn_Exp*_tmp477;struct
_dyneither_ptr*_tmp478;int _tmp479;int*_tmp47A;struct Cyc_Absyn_Exp*_tmp47C;struct
_dyneither_ptr*_tmp47D;int _tmp47E;int*_tmp47F;_LL204: {struct Cyc_Absyn_Cast_e_struct*
_tmp474=(struct Cyc_Absyn_Cast_e_struct*)_tmp473;if(_tmp474->tag != 15)goto _LL206;
else{_tmp475=_tmp474->f2;}}_LL205: {const char*_tmpBAA;void*_tmpBA9;(_tmpBA9=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBAA="cast on lhs!",_tag_dyneither(_tmpBAA,sizeof(char),13))),_tag_dyneither(
_tmpBA9,sizeof(void*),0)));}_LL206: {struct Cyc_Absyn_AggrMember_e_struct*_tmp476=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp473;if(_tmp476->tag != 22)goto _LL208;
else{_tmp477=_tmp476->f1;_tmp478=_tmp476->f2;_tmp479=_tmp476->f4;_tmp47A=(int*)&
_tmp476->f4;}}_LL207: {void*_tmp482=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp477->topt))->v);struct Cyc_Absyn_AggrInfo _tmp484;union Cyc_Absyn_AggrInfoU
_tmp485;_LL20D: {struct Cyc_Absyn_AggrType_struct*_tmp483=(struct Cyc_Absyn_AggrType_struct*)
_tmp482;if(_tmp483->tag != 12)goto _LL20F;else{_tmp484=_tmp483->f1;_tmp485=_tmp484.aggr_info;}}
_LL20E: {struct Cyc_Absyn_Aggrdecl*_tmp486=Cyc_Absyn_get_known_aggrdecl(_tmp485);*
f_tag=Cyc_Toc_get_member_offset(_tmp486,_tmp478);{const char*_tmpBAF;void*_tmpBAE[
2];struct Cyc_String_pa_struct _tmpBAD;struct Cyc_String_pa_struct _tmpBAC;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpBAC.tag=0,((_tmpBAC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp478),((_tmpBAD.tag=0,((_tmpBAD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp486->name).f2),((_tmpBAE[0]=&
_tmpBAD,((_tmpBAE[1]=& _tmpBAC,Cyc_aprintf(((_tmpBAF="_union_%s_%s",
_tag_dyneither(_tmpBAF,sizeof(char),13))),_tag_dyneither(_tmpBAE,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpBB0;*tagged_member_type=Cyc_Absyn_strct(((_tmpBB0=
_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0[0]=str,_tmpBB0)))));}if(clear_read)*
_tmp47A=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp486->impl))->tagged;};}
_LL20F:;_LL210: return 0;_LL20C:;}_LL208: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp47B=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp473;if(_tmp47B->tag != 23)goto
_LL20A;else{_tmp47C=_tmp47B->f1;_tmp47D=_tmp47B->f2;_tmp47E=_tmp47B->f4;_tmp47F=(
int*)& _tmp47B->f4;}}_LL209: {void*_tmp48C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp47C->topt))->v);struct Cyc_Absyn_PtrInfo _tmp48E;void*_tmp48F;
_LL212: {struct Cyc_Absyn_PointerType_struct*_tmp48D=(struct Cyc_Absyn_PointerType_struct*)
_tmp48C;if(_tmp48D->tag != 5)goto _LL214;else{_tmp48E=_tmp48D->f1;_tmp48F=_tmp48E.elt_typ;}}
_LL213: {void*_tmp490=Cyc_Tcutil_compress(_tmp48F);struct Cyc_Absyn_AggrInfo
_tmp492;union Cyc_Absyn_AggrInfoU _tmp493;_LL217: {struct Cyc_Absyn_AggrType_struct*
_tmp491=(struct Cyc_Absyn_AggrType_struct*)_tmp490;if(_tmp491->tag != 12)goto
_LL219;else{_tmp492=_tmp491->f1;_tmp493=_tmp492.aggr_info;}}_LL218: {struct Cyc_Absyn_Aggrdecl*
_tmp494=Cyc_Absyn_get_known_aggrdecl(_tmp493);*f_tag=Cyc_Toc_get_member_offset(
_tmp494,_tmp47D);{const char*_tmpBB5;void*_tmpBB4[2];struct Cyc_String_pa_struct
_tmpBB3;struct Cyc_String_pa_struct _tmpBB2;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpBB2.tag=0,((_tmpBB2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp47D),((_tmpBB3.tag=0,((_tmpBB3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp494->name).f2),((_tmpBB4[0]=& _tmpBB3,((_tmpBB4[1]=&
_tmpBB2,Cyc_aprintf(((_tmpBB5="_union_%s_%s",_tag_dyneither(_tmpBB5,sizeof(char),
13))),_tag_dyneither(_tmpBB4,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpBB6;*tagged_member_type=Cyc_Absyn_strct(((_tmpBB6=_cycalloc(sizeof(*_tmpBB6)),((
_tmpBB6[0]=str,_tmpBB6)))));}if(clear_read)*_tmp47F=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp494->impl))->tagged;};}_LL219:;_LL21A: return 0;_LL216:;}_LL214:;
_LL215: return 0;_LL211:;}_LL20A:;_LL20B: return 0;_LL203:;}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type);static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp49A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp49A,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp49B=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp49B,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp49A,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpC09(unsigned int*_tmpC08,unsigned int*_tmpC07,struct _tuple0***
_tmpC05){for(*_tmpC08=0;*_tmpC08 < *_tmpC07;(*_tmpC08)++){(*_tmpC05)[*_tmpC08]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp49C=e->r;if(e->topt == 0){const char*_tmpBBA;void*_tmpBB9[1];struct Cyc_String_pa_struct
_tmpBB8;(_tmpBB8.tag=0,((_tmpBB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBB9[0]=& _tmpBB8,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBA="exp_to_c: no type for %s",
_tag_dyneither(_tmpBBA,sizeof(char),25))),_tag_dyneither(_tmpBB9,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp4A0=
_tmp49C;union Cyc_Absyn_Cnst _tmp4A2;int _tmp4A3;struct _tuple0*_tmp4A6;void*_tmp4A7;
struct _tuple0*_tmp4A9;enum Cyc_Absyn_Primop _tmp4AB;struct Cyc_List_List*_tmp4AC;
struct Cyc_Absyn_Exp*_tmp4AE;enum Cyc_Absyn_Incrementor _tmp4AF;struct Cyc_Absyn_Exp*
_tmp4B1;struct Cyc_Core_Opt*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*
_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Absyn_Exp*
_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4BC;struct Cyc_Absyn_Exp*
_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BF;struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_Absyn_Exp*
_tmp4C2;struct Cyc_List_List*_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_List_List*
_tmp4C6;struct Cyc_Absyn_VarargCallInfo*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C9;
struct Cyc_List_List*_tmp4CA;struct Cyc_Absyn_VarargCallInfo*_tmp4CB;struct Cyc_Absyn_VarargCallInfo
_tmp4CC;int _tmp4CD;struct Cyc_List_List*_tmp4CE;struct Cyc_Absyn_VarargInfo*
_tmp4CF;struct Cyc_Absyn_Exp*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*
_tmp4D5;struct Cyc_List_List*_tmp4D6;void*_tmp4D8;void**_tmp4D9;struct Cyc_Absyn_Exp*
_tmp4DA;int _tmp4DB;enum Cyc_Absyn_Coercion _tmp4DC;struct Cyc_Absyn_Exp*_tmp4DE;
struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_Absyn_Exp*
_tmp4E3;void*_tmp4E5;void*_tmp4E7;void*_tmp4E8;struct _dyneither_ptr*_tmp4EA;void*
_tmp4EC;void*_tmp4ED;unsigned int _tmp4EF;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_Absyn_Exp*
_tmp4F3;struct _dyneither_ptr*_tmp4F4;int _tmp4F5;int _tmp4F6;struct Cyc_Absyn_Exp*
_tmp4F8;struct _dyneither_ptr*_tmp4F9;int _tmp4FA;int _tmp4FB;struct Cyc_Absyn_Exp*
_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FE;struct Cyc_List_List*_tmp500;struct Cyc_List_List*
_tmp502;struct Cyc_Absyn_Vardecl*_tmp504;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Absyn_Exp*
_tmp506;int _tmp507;struct _tuple0*_tmp509;struct Cyc_List_List*_tmp50A;struct Cyc_List_List*
_tmp50B;struct Cyc_Absyn_Aggrdecl*_tmp50C;void*_tmp50E;struct Cyc_List_List*
_tmp50F;struct Cyc_List_List*_tmp511;struct Cyc_Absyn_Datatypedecl*_tmp512;struct
Cyc_Absyn_Datatypefield*_tmp513;struct Cyc_Absyn_MallocInfo _tmp517;int _tmp518;
struct Cyc_Absyn_Exp*_tmp519;void**_tmp51A;struct Cyc_Absyn_Exp*_tmp51B;int _tmp51C;
struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Exp*_tmp51F;struct Cyc_Absyn_Exp*
_tmp521;struct _dyneither_ptr*_tmp522;struct Cyc_Absyn_Stmt*_tmp524;_LL21C: {
struct Cyc_Absyn_Const_e_struct*_tmp4A1=(struct Cyc_Absyn_Const_e_struct*)_tmp4A0;
if(_tmp4A1->tag != 0)goto _LL21E;else{_tmp4A2=_tmp4A1->f1;if((_tmp4A2.Null_c).tag
!= 1)goto _LL21E;_tmp4A3=(int)(_tmp4A2.Null_c).val;}}_LL21D: {struct Cyc_Absyn_Exp*
_tmp528=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp528,
_tmp528))->r;else{struct Cyc_Absyn_Exp*_tmpBBB[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBBB[2]=_tmp528,((_tmpBBB[1]=_tmp528,((_tmpBBB[0]=_tmp528,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBB,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL21B;}_LL21E: {struct Cyc_Absyn_Const_e_struct*
_tmp4A4=(struct Cyc_Absyn_Const_e_struct*)_tmp4A0;if(_tmp4A4->tag != 0)goto _LL220;}
_LL21F: goto _LL21B;_LL220: {struct Cyc_Absyn_Var_e_struct*_tmp4A5=(struct Cyc_Absyn_Var_e_struct*)
_tmp4A0;if(_tmp4A5->tag != 1)goto _LL222;else{_tmp4A6=_tmp4A5->f1;_tmp4A7=(void*)
_tmp4A5->f2;}}_LL221:{struct _handler_cons _tmp52A;_push_handler(& _tmp52A);{int
_tmp52C=0;if(setjmp(_tmp52A.handler))_tmp52C=1;if(!_tmp52C){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp4A6))->r;;_pop_handler();}else{void*_tmp52B=(void*)_exn_thrown;void*
_tmp52E=_tmp52B;_LL273: {struct Cyc_Dict_Absent_struct*_tmp52F=(struct Cyc_Dict_Absent_struct*)
_tmp52E;if(_tmp52F->tag != Cyc_Dict_Absent)goto _LL275;}_LL274: {const char*_tmpBBF;
void*_tmpBBE[1];struct Cyc_String_pa_struct _tmpBBD;(_tmpBBD.tag=0,((_tmpBBD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4A6)),((
_tmpBBE[0]=& _tmpBBD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBBF="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBBF,sizeof(char),
32))),_tag_dyneither(_tmpBBE,sizeof(void*),1)))))));}_LL275:;_LL276:(void)_throw(
_tmp52E);_LL272:;}};}goto _LL21B;_LL222: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp4A8=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp4A0;if(_tmp4A8->tag != 2)goto
_LL224;else{_tmp4A9=_tmp4A8->f1;}}_LL223: {const char*_tmpBC2;void*_tmpBC1;(
_tmpBC1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBC2="unknownid",_tag_dyneither(_tmpBC2,sizeof(char),10))),_tag_dyneither(
_tmpBC1,sizeof(void*),0)));}_LL224: {struct Cyc_Absyn_Primop_e_struct*_tmp4AA=(
struct Cyc_Absyn_Primop_e_struct*)_tmp4A0;if(_tmp4AA->tag != 3)goto _LL226;else{
_tmp4AB=_tmp4AA->f1;_tmp4AC=_tmp4AA->f2;}}_LL225: {struct Cyc_List_List*_tmp535=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4AC);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4AC);switch(_tmp4AB){case Cyc_Absyn_Numelts: _LL277: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp4AC))->hd;{void*_tmp536=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp538;struct Cyc_Absyn_Exp*
_tmp539;struct Cyc_Absyn_PtrInfo _tmp53B;void*_tmp53C;struct Cyc_Absyn_PtrAtts
_tmp53D;union Cyc_Absyn_Constraint*_tmp53E;union Cyc_Absyn_Constraint*_tmp53F;
union Cyc_Absyn_Constraint*_tmp540;_LL27A: {struct Cyc_Absyn_ArrayType_struct*
_tmp537=(struct Cyc_Absyn_ArrayType_struct*)_tmp536;if(_tmp537->tag != 9)goto
_LL27C;else{_tmp538=_tmp537->f1;_tmp539=_tmp538.num_elts;}}_LL27B: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp539))){const char*_tmpBC5;void*_tmpBC4;(
_tmpBC4=0,Cyc_Tcutil_terr(e->loc,((_tmpBC5="can't calculate numelts",
_tag_dyneither(_tmpBC5,sizeof(char),24))),_tag_dyneither(_tmpBC4,sizeof(void*),0)));}
e->r=_tmp539->r;goto _LL279;_LL27C: {struct Cyc_Absyn_PointerType_struct*_tmp53A=(
struct Cyc_Absyn_PointerType_struct*)_tmp536;if(_tmp53A->tag != 5)goto _LL27E;else{
_tmp53B=_tmp53A->f1;_tmp53C=_tmp53B.elt_typ;_tmp53D=_tmp53B.ptr_atts;_tmp53E=
_tmp53D.nullable;_tmp53F=_tmp53D.bounds;_tmp540=_tmp53D.zero_term;}}_LL27D:{void*
_tmp543=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp53F);struct Cyc_Absyn_Exp*_tmp546;_LL281: {struct Cyc_Absyn_DynEither_b_struct*
_tmp544=(struct Cyc_Absyn_DynEither_b_struct*)_tmp543;if(_tmp544->tag != 0)goto
_LL283;}_LL282:{struct Cyc_Absyn_Exp*_tmpBC6[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBC6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp53C),0),((_tmpBC6[0]=(
struct Cyc_Absyn_Exp*)_tmp4AC->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBC6,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL280;_LL283: {struct Cyc_Absyn_Upper_b_struct*_tmp545=(struct Cyc_Absyn_Upper_b_struct*)
_tmp543;if(_tmp545->tag != 1)goto _LL280;else{_tmp546=_tmp545->f1;}}_LL284: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp540)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp4AC->hd);struct Cyc_Absyn_Exp*_tmpBC7[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBC7[1]=_tmp546,((_tmpBC7[0]=(struct Cyc_Absyn_Exp*)_tmp4AC->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBC7,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp53E)){if(!Cyc_Evexp_c_can_eval(_tmp546)){const
char*_tmpBCA;void*_tmpBC9;(_tmpBC9=0,Cyc_Tcutil_terr(e->loc,((_tmpBCA="can't calculate numelts",
_tag_dyneither(_tmpBCA,sizeof(char),24))),_tag_dyneither(_tmpBC9,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp546,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp546->r;goto _LL280;}}goto _LL280;_LL280:;}goto _LL279;_LL27E:;_LL27F: {const
char*_tmpBCF;void*_tmpBCE[2];struct Cyc_String_pa_struct _tmpBCD;struct Cyc_String_pa_struct
_tmpBCC;(_tmpBCC.tag=0,((_tmpBCC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpBCD.tag=0,((_tmpBCD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpBCE[0]=& _tmpBCD,((
_tmpBCE[1]=& _tmpBCC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBCF="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBCF,
sizeof(char),41))),_tag_dyneither(_tmpBCE,sizeof(void*),2)))))))))))));}_LL279:;}
break;}case Cyc_Absyn_Plus: _LL278:{void*_tmp54F=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp535))->hd);struct Cyc_Absyn_PtrInfo _tmp551;
void*_tmp552;struct Cyc_Absyn_PtrAtts _tmp553;union Cyc_Absyn_Constraint*_tmp554;
union Cyc_Absyn_Constraint*_tmp555;_LL287: {struct Cyc_Absyn_PointerType_struct*
_tmp550=(struct Cyc_Absyn_PointerType_struct*)_tmp54F;if(_tmp550->tag != 5)goto
_LL289;else{_tmp551=_tmp550->f1;_tmp552=_tmp551.elt_typ;_tmp553=_tmp551.ptr_atts;
_tmp554=_tmp553.bounds;_tmp555=_tmp553.zero_term;}}_LL288:{void*_tmp556=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp554);struct Cyc_Absyn_Exp*_tmp559;_LL28C: {struct Cyc_Absyn_DynEither_b_struct*
_tmp557=(struct Cyc_Absyn_DynEither_b_struct*)_tmp556;if(_tmp557->tag != 0)goto
_LL28E;}_LL28D: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4AC))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4AC->tl))->hd;{struct Cyc_Absyn_Exp*_tmpBD0[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBD0[2]=e2,((_tmpBD0[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp552),0),((_tmpBD0[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD0,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL28B;}_LL28E: {struct Cyc_Absyn_Upper_b_struct*_tmp558=(struct
Cyc_Absyn_Upper_b_struct*)_tmp556;if(_tmp558->tag != 1)goto _LL28B;else{_tmp559=
_tmp558->f1;}}_LL28F: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp555)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4AC))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp4AC->tl))->hd;struct Cyc_Absyn_Exp*_tmpBD1[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpBD1[2]=e2,((_tmpBD1[1]=_tmp559,((_tmpBD1[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD1,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL28B;_LL28B:;}goto _LL286;_LL289:;_LL28A: goto _LL286;
_LL286:;}break;case Cyc_Absyn_Minus: _LL285: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp535))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4AC))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4AC->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp535->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpBD2[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBD2[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpBD2[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpBD2[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBD2,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL290: goto _LL291;case Cyc_Absyn_Neq: _LL291: goto _LL292;case Cyc_Absyn_Gt: _LL292:
goto _LL293;case Cyc_Absyn_Gte: _LL293: goto _LL294;case Cyc_Absyn_Lt: _LL294: goto
_LL295;case Cyc_Absyn_Lte: _LL295: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4AC))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4AC->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp535))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp535->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL296: break;}goto _LL21B;}_LL226: {
struct Cyc_Absyn_Increment_e_struct*_tmp4AD=(struct Cyc_Absyn_Increment_e_struct*)
_tmp4A0;if(_tmp4AD->tag != 5)goto _LL228;else{_tmp4AE=_tmp4AD->f1;_tmp4AF=_tmp4AD->f2;}}
_LL227: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4AE->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpBD3;struct _dyneither_ptr incr_str=(_tmpBD3="increment",
_tag_dyneither(_tmpBD3,sizeof(char),10));if(_tmp4AF == Cyc_Absyn_PreDec  || 
_tmp4AF == Cyc_Absyn_PostDec){const char*_tmpBD4;incr_str=((_tmpBD4="decrement",
_tag_dyneither(_tmpBD4,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4AE,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpBD8;void*_tmpBD7[1];
struct Cyc_String_pa_struct _tmpBD6;(_tmpBD6.tag=0,((_tmpBD6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBD7[0]=& _tmpBD6,Cyc_Tcutil_terr(
e->loc,((_tmpBD8="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpBD8,sizeof(char),74))),_tag_dyneither(_tmpBD7,sizeof(void*),1)))))));}{
const char*_tmpBDB;void*_tmpBDA;(_tmpBDA=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDB="in-place inc/dec on zero-term",
_tag_dyneither(_tmpBDB,sizeof(char),30))),_tag_dyneither(_tmpBDA,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp4AE,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp563=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpBDC;_tmp563->topt=((_tmpBDC=_cycalloc(sizeof(*
_tmpBDC)),((_tmpBDC->v=Cyc_Absyn_sint_typ,_tmpBDC))));}switch(_tmp4AF){case Cyc_Absyn_PreInc:
_LL298:{struct Cyc_Absyn_AssignOp_e_struct _tmpBE2;struct Cyc_Core_Opt*_tmpBE1;
struct Cyc_Absyn_AssignOp_e_struct*_tmpBE0;e->r=(void*)((_tmpBE0=_cycalloc(
sizeof(*_tmpBE0)),((_tmpBE0[0]=((_tmpBE2.tag=4,((_tmpBE2.f1=_tmp4AE,((_tmpBE2.f2=((
_tmpBE1=_cycalloc_atomic(sizeof(*_tmpBE1)),((_tmpBE1->v=(void*)Cyc_Absyn_Plus,
_tmpBE1)))),((_tmpBE2.f3=_tmp563,_tmpBE2)))))))),_tmpBE0))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL299:{struct Cyc_Absyn_AssignOp_e_struct
_tmpBE8;struct Cyc_Core_Opt*_tmpBE7;struct Cyc_Absyn_AssignOp_e_struct*_tmpBE6;e->r=(
void*)((_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6[0]=((_tmpBE8.tag=4,((
_tmpBE8.f1=_tmp4AE,((_tmpBE8.f2=((_tmpBE7=_cycalloc_atomic(sizeof(*_tmpBE7)),((
_tmpBE7->v=(void*)Cyc_Absyn_Minus,_tmpBE7)))),((_tmpBE8.f3=_tmp563,_tmpBE8)))))))),
_tmpBE6))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL29A:{const char*_tmpBEC;
void*_tmpBEB[1];struct Cyc_String_pa_struct _tmpBEA;(_tmpBEA.tag=0,((_tmpBEA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBEB[0]=& _tmpBEA,Cyc_Tcutil_terr(
e->loc,((_tmpBEC="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpBEC,sizeof(char),59))),_tag_dyneither(_tmpBEB,sizeof(void*),1)))))));}{
const char*_tmpBEF;void*_tmpBEE;(_tmpBEE=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEF="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpBEF,sizeof(char),34))),_tag_dyneither(_tmpBEE,sizeof(void*),0)));};}}
Cyc_Toc_exp_to_c(nv,_tmp4AE);{void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp4AF == Cyc_Absyn_PostInc
 || _tmp4AF == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp4AF == Cyc_Absyn_PreDec  || _tmp4AF == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpBF0[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBF0[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpBF0[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpBF0[0]=Cyc_Toc_push_address_exp(_tmp4AE),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF0,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp571=_tmp4AF == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp571,_tmp4AE);struct Cyc_Absyn_Exp*_tmpBF1[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpBF1[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpBF1[0]=Cyc_Toc_push_address_exp(
_tmp4AE),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBF1,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4AE)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp4AE,0,Cyc_Toc_incr_lvalue,_tmp4AF);e->r=_tmp4AE->r;}}}goto _LL21B;};};}_LL228: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp4B0=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4A0;if(_tmp4B0->tag != 4)goto _LL22A;else{_tmp4B1=_tmp4B0->f1;_tmp4B2=_tmp4B0->f2;
_tmp4B3=_tmp4B0->f3;}}_LL229: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4B1->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4B3->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp4B1,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_exp_to_c(nv,_tmp4B1);Cyc_Toc_exp_to_c(nv,
_tmp4B3);e->r=Cyc_Toc_tagged_union_assignop(_tmp4B1,e1_old_typ,_tmp4B2,_tmp4B3,
e2_old_typ,f_tag,tagged_member_struct_type);return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4B1,& ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,
_tmp4B1,_tmp4B2,_tmp4B3,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp4B1);Cyc_Toc_exp_to_c(nv,_tmp4B1);Cyc_Toc_exp_to_c(nv,_tmp4B3);{int done=0;
if(_tmp4B2 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;switch((enum Cyc_Absyn_Primop)
_tmp4B2->v){case Cyc_Absyn_Plus: _LL29C: change=_tmp4B3;break;case Cyc_Absyn_Minus:
_LL29D: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4B3,0);break;default:
_LL29E: {const char*_tmpBF4;void*_tmpBF3;(_tmpBF3=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF4="bad t ? pointer arithmetic",
_tag_dyneither(_tmpBF4,sizeof(char),27))),_tag_dyneither(_tmpBF3,sizeof(void*),0)));}}
done=1;{struct Cyc_Absyn_Exp*_tmp576=Cyc_Toc__dyneither_ptr_inplace_plus_e;struct
Cyc_Absyn_Exp*_tmpBF5[3];e->r=Cyc_Toc_fncall_exp_r(_tmp576,((_tmpBF5[2]=change,((
_tmpBF5[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBF5[0]=Cyc_Toc_push_address_exp(
_tmp4B1),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBF5,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ))switch((enum Cyc_Absyn_Primop)_tmp4B2->v){case Cyc_Absyn_Plus:
_LL2A0: done=1;{struct Cyc_Absyn_Exp*_tmpBF6[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4B1),((_tmpBF6[1]=_tmp4B3,((
_tmpBF6[0]=_tmp4B1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBF6,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2A1: {const char*_tmpBF9;void*_tmpBF8;(_tmpBF8=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF9="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpBF9,sizeof(char),39))),_tag_dyneither(_tmpBF8,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp4B3->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp4B3->r,0));if(!Cyc_Absyn_is_lvalue(_tmp4B1)){{struct _tuple18 _tmpBFC;struct
_tuple18*_tmpBFB;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp4B1,0,Cyc_Toc_assignop_lvalue,((_tmpBFB=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpBFB[0]=((_tmpBFC.f1=_tmp4B2,((_tmpBFC.f2=
_tmp4B3,_tmpBFC)))),_tmpBFB)))));}e->r=_tmp4B1->r;}}goto _LL21B;};};};}_LL22A: {
struct Cyc_Absyn_Conditional_e_struct*_tmp4B4=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp4A0;if(_tmp4B4->tag != 6)goto _LL22C;else{_tmp4B5=_tmp4B4->f1;_tmp4B6=_tmp4B4->f2;
_tmp4B7=_tmp4B4->f3;}}_LL22B: Cyc_Toc_exp_to_c(nv,_tmp4B5);Cyc_Toc_exp_to_c(nv,
_tmp4B6);Cyc_Toc_exp_to_c(nv,_tmp4B7);goto _LL21B;_LL22C: {struct Cyc_Absyn_And_e_struct*
_tmp4B8=(struct Cyc_Absyn_And_e_struct*)_tmp4A0;if(_tmp4B8->tag != 7)goto _LL22E;
else{_tmp4B9=_tmp4B8->f1;_tmp4BA=_tmp4B8->f2;}}_LL22D: Cyc_Toc_exp_to_c(nv,
_tmp4B9);Cyc_Toc_exp_to_c(nv,_tmp4BA);goto _LL21B;_LL22E: {struct Cyc_Absyn_Or_e_struct*
_tmp4BB=(struct Cyc_Absyn_Or_e_struct*)_tmp4A0;if(_tmp4BB->tag != 8)goto _LL230;
else{_tmp4BC=_tmp4BB->f1;_tmp4BD=_tmp4BB->f2;}}_LL22F: Cyc_Toc_exp_to_c(nv,
_tmp4BC);Cyc_Toc_exp_to_c(nv,_tmp4BD);goto _LL21B;_LL230: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp4BE=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp4A0;if(_tmp4BE->tag != 9)goto _LL232;
else{_tmp4BF=_tmp4BE->f1;_tmp4C0=_tmp4BE->f2;}}_LL231: Cyc_Toc_exp_to_c(nv,
_tmp4BF);Cyc_Toc_exp_to_c(nv,_tmp4C0);goto _LL21B;_LL232: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp4C1=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp4A0;if(_tmp4C1->tag != 10)goto
_LL234;else{_tmp4C2=_tmp4C1->f1;_tmp4C3=_tmp4C1->f2;}}_LL233: _tmp4C5=_tmp4C2;
_tmp4C6=_tmp4C3;goto _LL235;_LL234: {struct Cyc_Absyn_FnCall_e_struct*_tmp4C4=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp4A0;if(_tmp4C4->tag != 11)goto _LL236;else{
_tmp4C5=_tmp4C4->f1;_tmp4C6=_tmp4C4->f2;_tmp4C7=_tmp4C4->f3;if(_tmp4C7 != 0)goto
_LL236;}}_LL235: Cyc_Toc_exp_to_c(nv,_tmp4C5);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4C6);goto _LL21B;_LL236: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4C8=(struct Cyc_Absyn_FnCall_e_struct*)_tmp4A0;if(_tmp4C8->tag != 11)goto
_LL238;else{_tmp4C9=_tmp4C8->f1;_tmp4CA=_tmp4C8->f2;_tmp4CB=_tmp4C8->f3;if(
_tmp4CB == 0)goto _LL238;_tmp4CC=*_tmp4CB;_tmp4CD=_tmp4CC.num_varargs;_tmp4CE=
_tmp4CC.injectors;_tmp4CF=_tmp4CC.vai;}}_LL237: {struct Cyc_Toc_Env _tmp57E;struct
_RegionHandle*_tmp57F;struct Cyc_Toc_Env*_tmp57D=nv;_tmp57E=*_tmp57D;_tmp57F=
_tmp57E.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4CD,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp4CF->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4CA);int num_normargs=num_args - _tmp4CD;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4CA=_tmp4CA->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CA))->hd);{
struct Cyc_List_List*_tmpBFD;new_args=((_tmpBFD=_cycalloc(sizeof(*_tmpBFD)),((
_tmpBFD->hd=(struct Cyc_Absyn_Exp*)_tmp4CA->hd,((_tmpBFD->tl=new_args,_tmpBFD))))));};}}{
struct Cyc_Absyn_Exp*_tmpC00[3];struct Cyc_List_List*_tmpBFF;new_args=((_tmpBFF=
_cycalloc(sizeof(*_tmpBFF)),((_tmpBFF->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpC00[2]=num_varargs_exp,((_tmpC00[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpC00[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC00,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBFF->tl=
new_args,_tmpBFF))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4C9);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4C9,new_args,0),0);if(_tmp4CF->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp583=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4CF->type));struct Cyc_Absyn_DatatypeInfo _tmp585;union Cyc_Absyn_DatatypeInfoU
_tmp586;struct Cyc_Absyn_Datatypedecl**_tmp587;struct Cyc_Absyn_Datatypedecl*
_tmp588;_LL2A4: {struct Cyc_Absyn_DatatypeType_struct*_tmp584=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp583;if(_tmp584->tag != 3)goto _LL2A6;else{_tmp585=_tmp584->f1;_tmp586=_tmp585.datatype_info;
if((_tmp586.KnownDatatype).tag != 2)goto _LL2A6;_tmp587=(struct Cyc_Absyn_Datatypedecl**)(
_tmp586.KnownDatatype).val;_tmp588=*_tmp587;}}_LL2A5: tud=_tmp588;goto _LL2A3;
_LL2A6:;_LL2A7: {const char*_tmpC03;void*_tmpC02;(_tmpC02=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC03="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpC03,sizeof(char),44))),_tag_dyneither(_tmpC02,sizeof(void*),0)));}
_LL2A3:;}{unsigned int _tmpC08;unsigned int _tmpC07;struct _dyneither_ptr _tmpC06;
struct _tuple0**_tmpC05;unsigned int _tmpC04;struct _dyneither_ptr vs=(_tmpC04=(
unsigned int)_tmp4CD,((_tmpC05=(struct _tuple0**)_region_malloc(_tmp57F,
_check_times(sizeof(struct _tuple0*),_tmpC04)),((_tmpC06=_tag_dyneither(_tmpC05,
sizeof(struct _tuple0*),_tmpC04),((((_tmpC07=_tmpC04,_tmpC09(& _tmpC08,& _tmpC07,&
_tmpC05))),_tmpC06)))))));if(_tmp4CD != 0){struct Cyc_List_List*_tmp58B=0;{int i=
_tmp4CD - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpC0A;_tmp58B=((_tmpC0A=
_cycalloc(sizeof(*_tmpC0A)),((_tmpC0A->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpC0A->tl=_tmp58B,_tmpC0A))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp58B,0),s,0);{
int i=0;for(0;_tmp4CA != 0;(((_tmp4CA=_tmp4CA->tl,_tmp4CE=_tmp4CE->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4CA->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp58E;struct
_tuple0*_tmp58F;struct Cyc_List_List*_tmp590;struct Cyc_Absyn_Datatypefield*
_tmp58D=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp4CE))->hd;_tmp58E=*_tmp58D;_tmp58F=_tmp58E.name;_tmp590=_tmp58E.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp590))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp58F),0),s,0);{const char*_tmpC0B;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp58F,((_tmpC0B="_struct",_tag_dyneither(_tmpC0B,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpC0C[3];struct Cyc_List_List*
_tmp592=(_tmpC0C[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC0C[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC0C[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC0C,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp4CA
!= 0;(_tmp4CA=_tmp4CA->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4CA->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4CA->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL21B;}_LL238: {struct Cyc_Absyn_Throw_e_struct*_tmp4D0=(struct Cyc_Absyn_Throw_e_struct*)
_tmp4A0;if(_tmp4D0->tag != 12)goto _LL23A;else{_tmp4D1=_tmp4D0->f1;}}_LL239: Cyc_Toc_exp_to_c(
nv,_tmp4D1);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp4D1),0))->r;goto _LL21B;_LL23A: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp4D2=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp4A0;if(_tmp4D2->tag != 13)
goto _LL23C;else{_tmp4D3=_tmp4D2->f1;}}_LL23B: Cyc_Toc_exp_to_c(nv,_tmp4D3);goto
_LL21B;_LL23C: {struct Cyc_Absyn_Instantiate_e_struct*_tmp4D4=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp4A0;if(_tmp4D4->tag != 14)goto _LL23E;else{_tmp4D5=_tmp4D4->f1;_tmp4D6=_tmp4D4->f2;}}
_LL23D: Cyc_Toc_exp_to_c(nv,_tmp4D5);for(0;_tmp4D6 != 0;_tmp4D6=_tmp4D6->tl){enum 
Cyc_Absyn_KindQual _tmp598=(Cyc_Tcutil_typ_kind((void*)_tmp4D6->hd))->kind;if(
_tmp598 != Cyc_Absyn_EffKind  && _tmp598 != Cyc_Absyn_RgnKind){{void*_tmp599=Cyc_Tcutil_compress((
void*)_tmp4D6->hd);_LL2A9: {struct Cyc_Absyn_VarType_struct*_tmp59A=(struct Cyc_Absyn_VarType_struct*)
_tmp599;if(_tmp59A->tag != 2)goto _LL2AB;}_LL2AA: goto _LL2AC;_LL2AB: {struct Cyc_Absyn_DatatypeType_struct*
_tmp59B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp599;if(_tmp59B->tag != 3)goto
_LL2AD;}_LL2AC: continue;_LL2AD:;_LL2AE: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4D5,0))->r;goto _LL2A8;
_LL2A8:;}break;}}goto _LL21B;_LL23E: {struct Cyc_Absyn_Cast_e_struct*_tmp4D7=(
struct Cyc_Absyn_Cast_e_struct*)_tmp4A0;if(_tmp4D7->tag != 15)goto _LL240;else{
_tmp4D8=(void**)& _tmp4D7->f1;_tmp4D9=(void**)((void**)& _tmp4D7->f1);_tmp4DA=
_tmp4D7->f2;_tmp4DB=_tmp4D7->f3;_tmp4DC=_tmp4D7->f4;}}_LL23F: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4DA->topt))->v;void*new_typ=*_tmp4D9;*
_tmp4D9=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp4DA);{struct _tuple17
_tmpC0D;struct _tuple17 _tmp59D=(_tmpC0D.f1=Cyc_Tcutil_compress(old_t2),((_tmpC0D.f2=
Cyc_Tcutil_compress(new_typ),_tmpC0D)));void*_tmp59E;struct Cyc_Absyn_PtrInfo
_tmp5A0;void*_tmp5A1;struct Cyc_Absyn_PtrInfo _tmp5A3;void*_tmp5A4;struct Cyc_Absyn_PtrInfo
_tmp5A6;void*_tmp5A7;_LL2B0: _tmp59E=_tmp59D.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp59F=(struct Cyc_Absyn_PointerType_struct*)_tmp59E;if(_tmp59F->tag != 5)goto
_LL2B2;else{_tmp5A0=_tmp59F->f1;}};_tmp5A1=_tmp59D.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp5A2=(struct Cyc_Absyn_PointerType_struct*)_tmp5A1;if(_tmp5A2->tag != 5)goto
_LL2B2;else{_tmp5A3=_tmp5A2->f1;}};_LL2B1: {int _tmp5A9=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5A0.ptr_atts).nullable);int _tmp5AA=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5A3.ptr_atts).nullable);
void*_tmp5AB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5A0.ptr_atts).bounds);void*_tmp5AC=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5A3.ptr_atts).bounds);
int _tmp5AD=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp5A0.ptr_atts).zero_term);int _tmp5AE=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp5A3.ptr_atts).zero_term);{struct _tuple17 _tmpC0E;
struct _tuple17 _tmp5B0=(_tmpC0E.f1=_tmp5AB,((_tmpC0E.f2=_tmp5AC,_tmpC0E)));void*
_tmp5B1;struct Cyc_Absyn_Exp*_tmp5B3;void*_tmp5B4;struct Cyc_Absyn_Exp*_tmp5B6;
void*_tmp5B7;struct Cyc_Absyn_Exp*_tmp5B9;void*_tmp5BA;void*_tmp5BC;void*_tmp5BE;
struct Cyc_Absyn_Exp*_tmp5C0;void*_tmp5C1;void*_tmp5C3;_LL2B7: _tmp5B1=_tmp5B0.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp5B2=(struct Cyc_Absyn_Upper_b_struct*)_tmp5B1;
if(_tmp5B2->tag != 1)goto _LL2B9;else{_tmp5B3=_tmp5B2->f1;}};_tmp5B4=_tmp5B0.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5B5=(struct Cyc_Absyn_Upper_b_struct*)_tmp5B4;
if(_tmp5B5->tag != 1)goto _LL2B9;else{_tmp5B6=_tmp5B5->f1;}};_LL2B8: if((!Cyc_Evexp_c_can_eval(
_tmp5B3) || !Cyc_Evexp_c_can_eval(_tmp5B6)) && !Cyc_Evexp_same_const_exp(_tmp5B3,
_tmp5B6)){const char*_tmpC11;void*_tmpC10;(_tmpC10=0,Cyc_Tcutil_terr(e->loc,((
_tmpC11="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC11,sizeof(char),54))),_tag_dyneither(_tmpC10,sizeof(void*),0)));}if(_tmp5A9
 && !_tmp5AA){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC14;void*_tmpC13;(
_tmpC13=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC14="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC14,
sizeof(char),44))),_tag_dyneither(_tmpC13,sizeof(void*),0)));}if(_tmp4DC != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC18;void*_tmpC17[1];struct Cyc_String_pa_struct _tmpC16;(_tmpC16.tag=
0,((_tmpC16.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC17[0]=& _tmpC16,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC18="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC18,sizeof(char),41))),_tag_dyneither(_tmpC17,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4DA);if(do_null_check){if(!_tmp4DB){
const char*_tmpC1B;void*_tmpC1A;(_tmpC1A=0,Cyc_Tcutil_warn(e->loc,((_tmpC1B="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC1B,sizeof(char),58))),_tag_dyneither(_tmpC1A,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC1C;e->r=Cyc_Toc_cast_it_r(*_tmp4D9,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC1C=_cycalloc(sizeof(*_tmpC1C)),((_tmpC1C->hd=
_tmp4DA,((_tmpC1C->tl=0,_tmpC1C)))))),0));};}};}goto _LL2B6;_LL2B9: _tmp5B7=
_tmp5B0.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp5B8=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5B7;if(_tmp5B8->tag != 1)goto _LL2BB;else{_tmp5B9=_tmp5B8->f1;}};_tmp5BA=
_tmp5B0.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5BB=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5BA;if(_tmp5BB->tag != 0)goto _LL2BB;};_LL2BA: if(!Cyc_Evexp_c_can_eval(_tmp5B9)){
const char*_tmpC1F;void*_tmpC1E;(_tmpC1E=0,Cyc_Tcutil_terr(e->loc,((_tmpC1F="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC1F,sizeof(char),71))),_tag_dyneither(_tmpC1E,sizeof(void*),0)));}
if(_tmp4DC == Cyc_Absyn_NonNull_to_Null){const char*_tmpC23;void*_tmpC22[1];struct
Cyc_String_pa_struct _tmpC21;(_tmpC21.tag=0,((_tmpC21.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC22[0]=& _tmpC21,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC23="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC23,sizeof(char),44))),_tag_dyneither(_tmpC22,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp5AD  && !(_tmp5A3.elt_tq).real_const) && !
_tmp5AE)_tmp5B9=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5B9,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5B9,_tmp4DA))->r;}else{
struct Cyc_Absyn_Exp*_tmp5D4=Cyc_Toc__tag_dyneither_e;if(_tmp5AD){struct _tuple0*
_tmp5D5=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5D6=Cyc_Absyn_var_exp(_tmp5D5,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp5D7=_tmp4DA->r;union Cyc_Absyn_Cnst _tmp5D9;
struct _dyneither_ptr _tmp5DA;_LL2C0: {struct Cyc_Absyn_Const_e_struct*_tmp5D8=(
struct Cyc_Absyn_Const_e_struct*)_tmp5D7;if(_tmp5D8->tag != 0)goto _LL2C2;else{
_tmp5D9=_tmp5D8->f1;if((_tmp5D9.String_c).tag != 7)goto _LL2C2;_tmp5DA=(struct
_dyneither_ptr)(_tmp5D9.String_c).val;}}_LL2C1: arg3=_tmp5B9;goto _LL2BF;_LL2C2:;
_LL2C3:{struct Cyc_Absyn_Exp*_tmpC24[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp4DA),((_tmpC24[1]=_tmp5B9,((_tmpC24[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5D6),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC24,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2BF;_LL2BF:;}if(!_tmp5AE  && !(_tmp5A3.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp5DC=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A3.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpC25[3];struct Cyc_Absyn_Exp*_tmp5DD=Cyc_Absyn_fncall_exp(_tmp5D4,((_tmpC25[2]=
arg3,((_tmpC25[1]=_tmp5DC,((_tmpC25[0]=_tmp5D6,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC25,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp5DE=Cyc_Absyn_exp_stmt(_tmp5DD,0);_tmp5DE=
Cyc_Absyn_declare_stmt(_tmp5D5,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp4DA,_tmp5DE,0);e->r=Cyc_Toc_stmt_exp_r(_tmp5DE);};}else{struct Cyc_Absyn_Exp*
_tmpC26[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5D4,((_tmpC26[2]=_tmp5B9,((_tmpC26[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A3.elt_typ),0),((_tmpC26[0]=
_tmp4DA,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC26,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2B6;
_LL2BB: _tmp5BC=_tmp5B0.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5BD=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5BC;if(_tmp5BD->tag != 0)goto _LL2BD;};_tmp5BE=_tmp5B0.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5BF=(struct Cyc_Absyn_Upper_b_struct*)_tmp5BE;if(_tmp5BF->tag != 1)goto _LL2BD;
else{_tmp5C0=_tmp5BF->f1;}};_LL2BC: if(!Cyc_Evexp_c_can_eval(_tmp5C0)){const char*
_tmpC29;void*_tmpC28;(_tmpC28=0,Cyc_Tcutil_terr(e->loc,((_tmpC29="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC29,sizeof(char),71))),_tag_dyneither(_tmpC28,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC2C;void*_tmpC2B;(_tmpC2B=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC2C="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC2C,sizeof(char),45))),_tag_dyneither(_tmpC2B,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5E5=_tmp5C0;if(_tmp5AD  && !_tmp5AE)_tmp5E5=Cyc_Absyn_add_exp(
_tmp5C0,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5E6=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC2D[3];struct Cyc_Absyn_Exp*_tmp5E7=Cyc_Absyn_fncall_exp(
_tmp5E6,((_tmpC2D[2]=_tmp5E5,((_tmpC2D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp5A0.elt_typ),0),((_tmpC2D[0]=_tmp4DA,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2D,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp5AA){struct Cyc_List_List*_tmpC2E;_tmp5E7->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC2E=_cycalloc(sizeof(*_tmpC2E)),((_tmpC2E->hd=Cyc_Absyn_copy_exp(
_tmp5E7),((_tmpC2E->tl=0,_tmpC2E)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp4D9,_tmp5E7);
goto _LL2B6;};};_LL2BD: _tmp5C1=_tmp5B0.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5C2=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5C1;if(_tmp5C2->tag != 0)goto
_LL2B6;};_tmp5C3=_tmp5B0.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5C4=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp5C3;if(_tmp5C4->tag != 0)goto _LL2B6;};_LL2BE:
DynCast: if((_tmp5AD  && !_tmp5AE) && !(_tmp5A3.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC31;void*_tmpC30;(_tmpC30=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC31="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC31,sizeof(char),70))),_tag_dyneither(_tmpC30,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5EC=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC32[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5EC,((_tmpC32[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC32[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A0.elt_typ),0),((
_tmpC32[0]=_tmp4DA,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC32,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2B6;
_LL2B6:;}goto _LL2AF;}_LL2B2: _tmp5A4=_tmp59D.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp5A5=(struct Cyc_Absyn_PointerType_struct*)_tmp5A4;if(_tmp5A5->tag != 5)goto
_LL2B4;else{_tmp5A6=_tmp5A5->f1;}};_tmp5A7=_tmp59D.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5A8=(struct Cyc_Absyn_IntType_struct*)_tmp5A7;if(_tmp5A8->tag != 6)goto _LL2B4;};
_LL2B3:{void*_tmp5EE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp5A6.ptr_atts).bounds);_LL2C5: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5EF=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5EE;if(_tmp5EF->tag != 0)goto
_LL2C7;}_LL2C6: _tmp4DA->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4DA->r,
_tmp4DA->loc),Cyc_Toc_curr_sp);goto _LL2C4;_LL2C7:;_LL2C8: goto _LL2C4;_LL2C4:;}
goto _LL2AF;_LL2B4:;_LL2B5: goto _LL2AF;_LL2AF:;}goto _LL21B;}_LL240: {struct Cyc_Absyn_Address_e_struct*
_tmp4DD=(struct Cyc_Absyn_Address_e_struct*)_tmp4A0;if(_tmp4DD->tag != 16)goto
_LL242;else{_tmp4DE=_tmp4DD->f1;}}_LL241:{void*_tmp5F0=_tmp4DE->r;struct _tuple0*
_tmp5F2;struct Cyc_List_List*_tmp5F3;struct Cyc_List_List*_tmp5F4;struct Cyc_List_List*
_tmp5F6;_LL2CA: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5F1=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp5F0;if(_tmp5F1->tag != 29)goto _LL2CC;else{_tmp5F2=_tmp5F1->f1;_tmp5F3=_tmp5F1->f2;
_tmp5F4=_tmp5F1->f3;}}_LL2CB: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC36;void*
_tmpC35[1];struct Cyc_String_pa_struct _tmpC34;(_tmpC34.tag=0,((_tmpC34.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4DE->loc)),((
_tmpC35[0]=& _tmpC34,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC36="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC36,
sizeof(char),42))),_tag_dyneither(_tmpC35,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4DE->topt))->v,_tmp5F3 != 0,1,0,
_tmp5F4,_tmp5F2))->r;goto _LL2C9;_LL2CC: {struct Cyc_Absyn_Tuple_e_struct*_tmp5F5=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5F0;if(_tmp5F5->tag != 25)goto _LL2CE;else{
_tmp5F6=_tmp5F5->f1;}}_LL2CD: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC3A;void*
_tmpC39[1];struct Cyc_String_pa_struct _tmpC38;(_tmpC38.tag=0,((_tmpC38.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4DE->loc)),((
_tmpC39[0]=& _tmpC38,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC3A="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC3A,
sizeof(char),42))),_tag_dyneither(_tmpC39,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp5F6))->r;goto _LL2C9;_LL2CE:;_LL2CF: Cyc_Toc_exp_to_c(nv,_tmp4DE);if(!
Cyc_Absyn_is_lvalue(_tmp4DE)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp4DE,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4DE);}goto _LL2C9;
_LL2C9:;}goto _LL21B;_LL242: {struct Cyc_Absyn_New_e_struct*_tmp4DF=(struct Cyc_Absyn_New_e_struct*)
_tmp4A0;if(_tmp4DF->tag != 17)goto _LL244;else{_tmp4E0=_tmp4DF->f1;_tmp4E1=_tmp4DF->f2;}}
_LL243: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC3E;void*_tmpC3D[1];struct Cyc_String_pa_struct
_tmpC3C;(_tmpC3C.tag=0,((_tmpC3C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4E1->loc)),((_tmpC3D[0]=& _tmpC3C,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC3E="%s: new at top-level",
_tag_dyneither(_tmpC3E,sizeof(char),21))),_tag_dyneither(_tmpC3D,sizeof(void*),1)))))));}{
void*_tmp600=_tmp4E1->r;struct Cyc_List_List*_tmp602;struct Cyc_Absyn_Vardecl*
_tmp604;struct Cyc_Absyn_Exp*_tmp605;struct Cyc_Absyn_Exp*_tmp606;int _tmp607;
struct _tuple0*_tmp609;struct Cyc_List_List*_tmp60A;struct Cyc_List_List*_tmp60B;
struct Cyc_Absyn_Aggrdecl*_tmp60C;struct Cyc_List_List*_tmp60E;_LL2D1: {struct Cyc_Absyn_Array_e_struct*
_tmp601=(struct Cyc_Absyn_Array_e_struct*)_tmp600;if(_tmp601->tag != 27)goto _LL2D3;
else{_tmp602=_tmp601->f1;}}_LL2D2: {struct _tuple0*_tmp60F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp610=Cyc_Absyn_var_exp(_tmp60F,0);struct Cyc_Absyn_Stmt*
_tmp611=Cyc_Toc_init_array(nv,_tmp610,_tmp602,Cyc_Absyn_exp_stmt(_tmp610,0));
void*old_elt_typ;{void*_tmp612=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp614;void*_tmp615;struct Cyc_Absyn_Tqual _tmp616;struct Cyc_Absyn_PtrAtts _tmp617;
union Cyc_Absyn_Constraint*_tmp618;_LL2DC: {struct Cyc_Absyn_PointerType_struct*
_tmp613=(struct Cyc_Absyn_PointerType_struct*)_tmp612;if(_tmp613->tag != 5)goto
_LL2DE;else{_tmp614=_tmp613->f1;_tmp615=_tmp614.elt_typ;_tmp616=_tmp614.elt_tq;
_tmp617=_tmp614.ptr_atts;_tmp618=_tmp617.zero_term;}}_LL2DD: old_elt_typ=_tmp615;
goto _LL2DB;_LL2DE:;_LL2DF: {const char*_tmpC41;void*_tmpC40;old_elt_typ=((_tmpC40=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC41="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC41,sizeof(char),52))),_tag_dyneither(_tmpC40,sizeof(void*),0))));}_LL2DB:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp61B=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp61C=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp602),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4E0 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp61C);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4E0;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp61C);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp60F,_tmp61B,(struct Cyc_Absyn_Exp*)e1,_tmp611,0));goto
_LL2D0;};}_LL2D3: {struct Cyc_Absyn_Comprehension_e_struct*_tmp603=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp600;if(_tmp603->tag != 28)goto _LL2D5;else{_tmp604=_tmp603->f1;_tmp605=_tmp603->f2;
_tmp606=_tmp603->f3;_tmp607=_tmp603->f4;}}_LL2D4: {int is_dyneither_ptr=0;{void*
_tmp61D=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp61F;void*_tmp620;
struct Cyc_Absyn_Tqual _tmp621;struct Cyc_Absyn_PtrAtts _tmp622;union Cyc_Absyn_Constraint*
_tmp623;union Cyc_Absyn_Constraint*_tmp624;_LL2E1: {struct Cyc_Absyn_PointerType_struct*
_tmp61E=(struct Cyc_Absyn_PointerType_struct*)_tmp61D;if(_tmp61E->tag != 5)goto
_LL2E3;else{_tmp61F=_tmp61E->f1;_tmp620=_tmp61F.elt_typ;_tmp621=_tmp61F.elt_tq;
_tmp622=_tmp61F.ptr_atts;_tmp623=_tmp622.bounds;_tmp624=_tmp622.zero_term;}}
_LL2E2: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp623)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2E0;
_LL2E3:;_LL2E4: {const char*_tmpC44;void*_tmpC43;(_tmpC43=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC44="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC44,sizeof(char),42))),_tag_dyneither(_tmpC43,sizeof(void*),0)));}
_LL2E0:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp606->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp605);{struct Cyc_Absyn_Exp*_tmp627=
Cyc_Absyn_var_exp(max,0);if(_tmp607)_tmp627=Cyc_Absyn_add_exp(_tmp627,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp604,Cyc_Absyn_var_exp(max,0),_tmp606,_tmp607,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp629;struct _RegionHandle*_tmp62A;struct Cyc_Toc_Env*_tmp628=
nv;_tmp629=*_tmp628;_tmp62A=_tmp629.rgn;{struct _tuple19*_tmpC47;struct Cyc_List_List*
_tmpC46;struct Cyc_List_List*decls=(_tmpC46=_region_malloc(_tmp62A,sizeof(*
_tmpC46)),((_tmpC46->hd=((_tmpC47=_region_malloc(_tmp62A,sizeof(*_tmpC47)),((
_tmpC47->f1=max,((_tmpC47->f2=Cyc_Absyn_uint_typ,((_tmpC47->f3=(struct Cyc_Absyn_Exp*)
_tmp605,_tmpC47)))))))),((_tmpC46->tl=0,_tmpC46)))));struct Cyc_Absyn_Exp*ai;if(
_tmp4E0 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC48[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC48[1]=_tmp627,((
_tmpC48[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC48,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4E0;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC49[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC49[1]=_tmp627,((_tmpC49[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC49,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC4C;struct Cyc_List_List*
_tmpC4B;decls=((_tmpC4B=_region_malloc(_tmp62A,sizeof(*_tmpC4B)),((_tmpC4B->hd=((
_tmpC4C=_region_malloc(_tmp62A,sizeof(*_tmpC4C)),((_tmpC4C->f1=a,((_tmpC4C->f2=
ptr_typ,((_tmpC4C->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC4C)))))))),((_tmpC4B->tl=
decls,_tmpC4B))))));}if(is_dyneither_ptr){struct _tuple0*_tmp62F=Cyc_Toc_temp_var();
void*_tmp630=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp631=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC4D[3];struct Cyc_Absyn_Exp*_tmp632=Cyc_Absyn_fncall_exp(
_tmp631,((_tmpC4D[2]=_tmp627,((_tmpC4D[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC4D[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC4D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC50;struct Cyc_List_List*_tmpC4F;decls=((_tmpC4F=
_region_malloc(_tmp62A,sizeof(*_tmpC4F)),((_tmpC4F->hd=((_tmpC50=_region_malloc(
_tmp62A,sizeof(*_tmpC50)),((_tmpC50->f1=_tmp62F,((_tmpC50->f2=_tmp630,((_tmpC50->f3=(
struct Cyc_Absyn_Exp*)_tmp632,_tmpC50)))))))),((_tmpC4F->tl=decls,_tmpC4F))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp62F,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp636=decls;for(0;_tmp636 != 0;_tmp636=_tmp636->tl){struct
_tuple0*_tmp638;void*_tmp639;struct Cyc_Absyn_Exp*_tmp63A;struct _tuple19 _tmp637=*((
struct _tuple19*)_tmp636->hd);_tmp638=_tmp637.f1;_tmp639=_tmp637.f2;_tmp63A=
_tmp637.f3;s=Cyc_Absyn_declare_stmt(_tmp638,_tmp639,_tmp63A,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2D0;};};};}_LL2D5: {struct Cyc_Absyn_Aggregate_e_struct*_tmp608=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp600;if(_tmp608->tag != 29)goto _LL2D7;
else{_tmp609=_tmp608->f1;_tmp60A=_tmp608->f2;_tmp60B=_tmp608->f3;_tmp60C=_tmp608->f4;}}
_LL2D6: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4E1->topt))->v,_tmp60A != 0,1,_tmp4E0,_tmp60B,_tmp609))->r;goto _LL2D0;_LL2D7: {
struct Cyc_Absyn_Tuple_e_struct*_tmp60D=(struct Cyc_Absyn_Tuple_e_struct*)_tmp600;
if(_tmp60D->tag != 25)goto _LL2D9;else{_tmp60E=_tmp60D->f1;}}_LL2D8: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp4E0,_tmp60E))->r;goto _LL2D0;_LL2D9:;_LL2DA: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4E1->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp63D=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp63E=Cyc_Absyn_var_exp(_tmp63D,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp63E,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp4E0
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4E0;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp63F=_tmp4E1->r;void*_tmp641;
struct Cyc_Absyn_Exp*_tmp642;_LL2E6: {struct Cyc_Absyn_Cast_e_struct*_tmp640=(
struct Cyc_Absyn_Cast_e_struct*)_tmp63F;if(_tmp640->tag != 15)goto _LL2E8;else{
_tmp641=(void*)_tmp640->f1;_tmp642=_tmp640->f2;}}_LL2E7:{struct _tuple17 _tmpC51;
struct _tuple17 _tmp644=(_tmpC51.f1=Cyc_Tcutil_compress(_tmp641),((_tmpC51.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp642->topt))->v),_tmpC51)));void*
_tmp645;struct Cyc_Absyn_PtrInfo _tmp647;void*_tmp648;struct Cyc_Absyn_PtrAtts
_tmp649;union Cyc_Absyn_Constraint*_tmp64A;void*_tmp64B;struct Cyc_Absyn_PtrInfo
_tmp64D;struct Cyc_Absyn_PtrAtts _tmp64E;union Cyc_Absyn_Constraint*_tmp64F;_LL2EB:
_tmp645=_tmp644.f1;{struct Cyc_Absyn_PointerType_struct*_tmp646=(struct Cyc_Absyn_PointerType_struct*)
_tmp645;if(_tmp646->tag != 5)goto _LL2ED;else{_tmp647=_tmp646->f1;_tmp648=_tmp647.elt_typ;
_tmp649=_tmp647.ptr_atts;_tmp64A=_tmp649.bounds;}};_tmp64B=_tmp644.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp64C=(struct Cyc_Absyn_PointerType_struct*)_tmp64B;if(_tmp64C->tag != 5)goto
_LL2ED;else{_tmp64D=_tmp64C->f1;_tmp64E=_tmp64D.ptr_atts;_tmp64F=_tmp64E.bounds;}};
_LL2EC:{struct _tuple17 _tmpC52;struct _tuple17 _tmp651=(_tmpC52.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp64A),((_tmpC52.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp64F),_tmpC52)));void*_tmp652;void*_tmp654;struct Cyc_Absyn_Exp*
_tmp656;_LL2F0: _tmp652=_tmp651.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp653=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp652;if(_tmp653->tag != 0)goto _LL2F2;};
_tmp654=_tmp651.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp655=(struct Cyc_Absyn_Upper_b_struct*)
_tmp654;if(_tmp655->tag != 1)goto _LL2F2;else{_tmp656=_tmp655->f1;}};_LL2F1: Cyc_Toc_exp_to_c(
nv,_tmp642);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp657=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC53[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp657,((_tmpC53[3]=_tmp656,((_tmpC53[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp648),0),((_tmpC53[1]=_tmp642,((_tmpC53[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC53,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2EF;};_LL2F2:;_LL2F3: goto _LL2EF;_LL2EF:;}goto _LL2EA;_LL2ED:;
_LL2EE: goto _LL2EA;_LL2EA:;}goto _LL2E5;_LL2E8:;_LL2E9: goto _LL2E5;_LL2E5:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp659=Cyc_Absyn_exp_stmt(_tmp63E,0);struct Cyc_Absyn_Exp*
_tmp65A=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp4E1);_tmp659=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp63E,_tmp65A,0),_tmp4E1,0),
_tmp659,0);{void*_tmp65B=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp63D,_tmp65B,(struct Cyc_Absyn_Exp*)mexp,_tmp659,0));};}
goto _LL2D0;};}_LL2D0:;}goto _LL21B;_LL244: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp4E2=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp4A0;if(_tmp4E2->tag != 19)goto
_LL246;else{_tmp4E3=_tmp4E2->f1;}}_LL245: Cyc_Toc_exp_to_c(nv,_tmp4E3);goto _LL21B;
_LL246: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4E4=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4A0;if(_tmp4E4->tag != 18)goto _LL248;else{_tmp4E5=(void*)_tmp4E4->f1;}}_LL247:{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpC56;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpC55;e->r=(void*)((_tmpC55=_cycalloc(sizeof(*_tmpC55)),((_tmpC55[0]=((_tmpC56.tag=
18,((_tmpC56.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4E5),_tmpC56)))),_tmpC55))));}
goto _LL21B;_LL248: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4E6=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp4A0;if(_tmp4E6->tag != 20)goto _LL24A;else{_tmp4E7=(void*)_tmp4E6->f1;_tmp4E8=(
void*)_tmp4E6->f2;{struct Cyc_Absyn_StructField_struct*_tmp4E9=(struct Cyc_Absyn_StructField_struct*)
_tmp4E8;if(_tmp4E9->tag != 0)goto _LL24A;else{_tmp4EA=_tmp4E9->f1;}};}}_LL249:{
struct Cyc_Absyn_Offsetof_e_struct _tmpC60;struct Cyc_Absyn_StructField_struct
_tmpC5F;struct Cyc_Absyn_StructField_struct*_tmpC5E;struct Cyc_Absyn_Offsetof_e_struct*
_tmpC5D;e->r=(void*)((_tmpC5D=_cycalloc(sizeof(*_tmpC5D)),((_tmpC5D[0]=((_tmpC60.tag=
20,((_tmpC60.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4E7),((_tmpC60.f2=(void*)((
void*)((_tmpC5E=_cycalloc(sizeof(*_tmpC5E)),((_tmpC5E[0]=((_tmpC5F.tag=0,((
_tmpC5F.f1=_tmp4EA,_tmpC5F)))),_tmpC5E))))),_tmpC60)))))),_tmpC5D))));}goto
_LL21B;_LL24A: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4EB=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp4A0;if(_tmp4EB->tag != 20)goto _LL24C;else{_tmp4EC=(void*)_tmp4EB->f1;_tmp4ED=(
void*)_tmp4EB->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp4EE=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp4ED;if(_tmp4EE->tag != 1)goto _LL24C;else{_tmp4EF=_tmp4EE->f1;}};}}_LL24B:{
void*_tmp662=Cyc_Tcutil_compress(_tmp4EC);struct Cyc_Absyn_AggrInfo _tmp664;union
Cyc_Absyn_AggrInfoU _tmp665;struct Cyc_List_List*_tmp667;_LL2F5: {struct Cyc_Absyn_AggrType_struct*
_tmp663=(struct Cyc_Absyn_AggrType_struct*)_tmp662;if(_tmp663->tag != 12)goto
_LL2F7;else{_tmp664=_tmp663->f1;_tmp665=_tmp664.aggr_info;}}_LL2F6: {struct Cyc_Absyn_Aggrdecl*
_tmp66A=Cyc_Absyn_get_known_aggrdecl(_tmp665);if(_tmp66A->impl == 0){const char*
_tmpC63;void*_tmpC62;(_tmpC62=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC63="struct fields must be known",
_tag_dyneither(_tmpC63,sizeof(char),28))),_tag_dyneither(_tmpC62,sizeof(void*),0)));}
_tmp667=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp66A->impl))->fields;goto
_LL2F8;}_LL2F7: {struct Cyc_Absyn_AnonAggrType_struct*_tmp666=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp662;if(_tmp666->tag != 13)goto _LL2F9;else{_tmp667=_tmp666->f2;}}_LL2F8: {
struct Cyc_Absyn_Aggrfield*_tmp66D=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp667,(int)_tmp4EF);{struct Cyc_Absyn_Offsetof_e_struct
_tmpC6D;struct Cyc_Absyn_StructField_struct _tmpC6C;struct Cyc_Absyn_StructField_struct*
_tmpC6B;struct Cyc_Absyn_Offsetof_e_struct*_tmpC6A;e->r=(void*)((_tmpC6A=
_cycalloc(sizeof(*_tmpC6A)),((_tmpC6A[0]=((_tmpC6D.tag=20,((_tmpC6D.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4EC),((_tmpC6D.f2=(void*)((void*)((_tmpC6B=_cycalloc(
sizeof(*_tmpC6B)),((_tmpC6B[0]=((_tmpC6C.tag=0,((_tmpC6C.f1=_tmp66D->name,
_tmpC6C)))),_tmpC6B))))),_tmpC6D)))))),_tmpC6A))));}goto _LL2F4;}_LL2F9: {struct
Cyc_Absyn_TupleType_struct*_tmp668=(struct Cyc_Absyn_TupleType_struct*)_tmp662;
if(_tmp668->tag != 11)goto _LL2FB;}_LL2FA:{struct Cyc_Absyn_Offsetof_e_struct
_tmpC77;struct Cyc_Absyn_StructField_struct _tmpC76;struct Cyc_Absyn_StructField_struct*
_tmpC75;struct Cyc_Absyn_Offsetof_e_struct*_tmpC74;e->r=(void*)((_tmpC74=
_cycalloc(sizeof(*_tmpC74)),((_tmpC74[0]=((_tmpC77.tag=20,((_tmpC77.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4EC),((_tmpC77.f2=(void*)((void*)((_tmpC75=_cycalloc(
sizeof(*_tmpC75)),((_tmpC75[0]=((_tmpC76.tag=0,((_tmpC76.f1=Cyc_Absyn_fieldname((
int)(_tmp4EF + 1)),_tmpC76)))),_tmpC75))))),_tmpC77)))))),_tmpC74))));}goto _LL2F4;
_LL2FB: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp669=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp662;if(_tmp669->tag != 4)goto _LL2FD;}_LL2FC: if(_tmp4EF == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpC81;struct Cyc_Absyn_StructField_struct _tmpC80;struct Cyc_Absyn_StructField_struct*
_tmpC7F;struct Cyc_Absyn_Offsetof_e_struct*_tmpC7E;e->r=(void*)((_tmpC7E=
_cycalloc(sizeof(*_tmpC7E)),((_tmpC7E[0]=((_tmpC81.tag=20,((_tmpC81.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4EC),((_tmpC81.f2=(void*)((void*)((_tmpC7F=_cycalloc(
sizeof(*_tmpC7F)),((_tmpC7F[0]=((_tmpC80.tag=0,((_tmpC80.f1=Cyc_Toc_tag_sp,
_tmpC80)))),_tmpC7F))))),_tmpC81)))))),_tmpC7E))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpC8B;struct Cyc_Absyn_StructField_struct _tmpC8A;struct Cyc_Absyn_StructField_struct*
_tmpC89;struct Cyc_Absyn_Offsetof_e_struct*_tmpC88;e->r=(void*)((_tmpC88=
_cycalloc(sizeof(*_tmpC88)),((_tmpC88[0]=((_tmpC8B.tag=20,((_tmpC8B.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4EC),((_tmpC8B.f2=(void*)((void*)((_tmpC89=_cycalloc(
sizeof(*_tmpC89)),((_tmpC89[0]=((_tmpC8A.tag=0,((_tmpC8A.f1=Cyc_Absyn_fieldname((
int)_tmp4EF),_tmpC8A)))),_tmpC89))))),_tmpC8B)))))),_tmpC88))));}goto _LL2F4;
_LL2FD:;_LL2FE: {const char*_tmpC8E;void*_tmpC8D;(_tmpC8D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8E="impossible type for offsetof tuple index",
_tag_dyneither(_tmpC8E,sizeof(char),41))),_tag_dyneither(_tmpC8D,sizeof(void*),0)));}
_LL2F4:;}goto _LL21B;_LL24C: {struct Cyc_Absyn_Deref_e_struct*_tmp4F0=(struct Cyc_Absyn_Deref_e_struct*)
_tmp4A0;if(_tmp4F0->tag != 21)goto _LL24E;else{_tmp4F1=_tmp4F0->f1;}}_LL24D: {void*
_tmp680=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4F1->topt))->v);{
void*_tmp681=_tmp680;struct Cyc_Absyn_PtrInfo _tmp683;void*_tmp684;struct Cyc_Absyn_Tqual
_tmp685;struct Cyc_Absyn_PtrAtts _tmp686;void*_tmp687;union Cyc_Absyn_Constraint*
_tmp688;union Cyc_Absyn_Constraint*_tmp689;union Cyc_Absyn_Constraint*_tmp68A;
_LL300: {struct Cyc_Absyn_PointerType_struct*_tmp682=(struct Cyc_Absyn_PointerType_struct*)
_tmp681;if(_tmp682->tag != 5)goto _LL302;else{_tmp683=_tmp682->f1;_tmp684=_tmp683.elt_typ;
_tmp685=_tmp683.elt_tq;_tmp686=_tmp683.ptr_atts;_tmp687=_tmp686.rgn;_tmp688=
_tmp686.nullable;_tmp689=_tmp686.bounds;_tmp68A=_tmp686.zero_term;}}_LL301:{void*
_tmp68B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp689);struct Cyc_Absyn_Exp*_tmp68D;_LL305: {struct Cyc_Absyn_Upper_b_struct*
_tmp68C=(struct Cyc_Absyn_Upper_b_struct*)_tmp68B;if(_tmp68C->tag != 1)goto _LL307;
else{_tmp68D=_tmp68C->f1;}}_LL306: {int do_null_check=Cyc_Toc_need_null_check(
_tmp4F1);Cyc_Toc_exp_to_c(nv,_tmp4F1);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC91;void*_tmpC90;(_tmpC90=0,Cyc_Tcutil_warn(e->loc,((_tmpC91="inserted null check due to dereference",
_tag_dyneither(_tmpC91,sizeof(char),39))),_tag_dyneither(_tmpC90,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC92;_tmp4F1->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp680),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpC92=_cycalloc(sizeof(*
_tmpC92)),((_tmpC92->hd=Cyc_Absyn_copy_exp(_tmp4F1),((_tmpC92->tl=0,_tmpC92)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp68A)){unsigned int _tmp693;int _tmp694;struct _tuple10 _tmp692=Cyc_Evexp_eval_const_uint_exp(
_tmp68D);_tmp693=_tmp692.f1;_tmp694=_tmp692.f2;if(!_tmp694  || _tmp693 <= 0){const
char*_tmpC95;void*_tmpC94;(_tmpC94=0,Cyc_Tcutil_terr(e->loc,((_tmpC95="cannot determine dereference is in bounds",
_tag_dyneither(_tmpC95,sizeof(char),42))),_tag_dyneither(_tmpC94,sizeof(void*),0)));}}
goto _LL304;}_LL307: {struct Cyc_Absyn_DynEither_b_struct*_tmp68E=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp68B;if(_tmp68E->tag != 0)goto _LL304;}_LL308: {struct Cyc_Absyn_Exp*_tmp697=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpC96;_tmp697->topt=((_tmpC96=_cycalloc(sizeof(*
_tmpC96)),((_tmpC96->v=Cyc_Absyn_uint_typ,_tmpC96))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp4F1,_tmp697);Cyc_Toc_exp_to_c(nv,e);goto _LL304;}_LL304:;}goto _LL2FF;_LL302:;
_LL303: {const char*_tmpC99;void*_tmpC98;(_tmpC98=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC99="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpC99,sizeof(char),29))),_tag_dyneither(_tmpC98,sizeof(void*),0)));}
_LL2FF:;}goto _LL21B;}_LL24E: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4F2=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp4A0;if(_tmp4F2->tag != 22)goto _LL250;
else{_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;_tmp4F5=_tmp4F2->f3;_tmp4F6=_tmp4F2->f4;}}
_LL24F: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4F3->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp4F3);if(
_tmp4F5  && _tmp4F6)e->r=Cyc_Toc_check_tagged_union(_tmp4F3,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp4F4,Cyc_Absyn_aggrmember_exp);if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL21B;}_LL250: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp4F7=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4A0;if(_tmp4F7->tag != 23)goto _LL252;
else{_tmp4F8=_tmp4F7->f1;_tmp4F9=_tmp4F7->f2;_tmp4FA=_tmp4F7->f3;_tmp4FB=_tmp4F7->f4;}}
_LL251: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4F8->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp4F8);Cyc_Toc_exp_to_c(
nv,_tmp4F8);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp69C;struct Cyc_Absyn_Tqual
_tmp69D;struct Cyc_Absyn_PtrAtts _tmp69E;void*_tmp69F;union Cyc_Absyn_Constraint*
_tmp6A0;union Cyc_Absyn_Constraint*_tmp6A1;union Cyc_Absyn_Constraint*_tmp6A2;
struct Cyc_Absyn_PtrInfo _tmp69B=Cyc_Toc_get_ptr_type(e1typ);_tmp69C=_tmp69B.elt_typ;
_tmp69D=_tmp69B.elt_tq;_tmp69E=_tmp69B.ptr_atts;_tmp69F=_tmp69E.rgn;_tmp6A0=
_tmp69E.nullable;_tmp6A1=_tmp69E.bounds;_tmp6A2=_tmp69E.zero_term;{void*_tmp6A3=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6A1);struct Cyc_Absyn_Exp*_tmp6A5;_LL30A: {struct Cyc_Absyn_Upper_b_struct*
_tmp6A4=(struct Cyc_Absyn_Upper_b_struct*)_tmp6A3;if(_tmp6A4->tag != 1)goto _LL30C;
else{_tmp6A5=_tmp6A4->f1;}}_LL30B: {unsigned int _tmp6A8;int _tmp6A9;struct
_tuple10 _tmp6A7=Cyc_Evexp_eval_const_uint_exp(_tmp6A5);_tmp6A8=_tmp6A7.f1;
_tmp6A9=_tmp6A7.f2;if(_tmp6A9){if(_tmp6A8 < 1){const char*_tmpC9C;void*_tmpC9B;(
_tmpC9B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC9C="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC9C,
sizeof(char),44))),_tag_dyneither(_tmpC9B,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpC9F;void*_tmpC9E;(_tmpC9E=0,Cyc_Tcutil_warn(
e->loc,((_tmpC9F="inserted null check due to dereference",_tag_dyneither(_tmpC9F,
sizeof(char),39))),_tag_dyneither(_tmpC9E,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCA0[1];_tmp4F8->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpCA0[0]=Cyc_Absyn_new_exp(_tmp4F8->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA0,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp6A5)){const char*_tmpCA3;void*
_tmpCA2;(_tmpCA2=0,Cyc_Tcutil_terr(e->loc,((_tmpCA3="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpCA3,sizeof(char),47))),_tag_dyneither(_tmpCA2,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCA4[4];_tmp4F8->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpCA4[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpCA4[2]=Cyc_Absyn_sizeoftyp_exp(_tmp69C,0),((
_tmpCA4[1]=_tmp6A5,((_tmpCA4[0]=Cyc_Absyn_new_exp(_tmp4F8->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA4,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL309;}_LL30C: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6A6=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6A3;if(_tmp6A6->tag != 0)goto
_LL309;}_LL30D: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp69C);{struct Cyc_Absyn_Exp*
_tmpCA5[3];_tmp4F8->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp69D),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpCA5[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpCA5[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCA5[0]=Cyc_Absyn_new_exp(_tmp4F8->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCA5,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL309;}_LL309:;}if(
_tmp4FA  && _tmp4FB)e->r=Cyc_Toc_check_tagged_union(_tmp4F8,Cyc_Toc_typ_to_c(
e1typ),_tmp69C,_tmp4F9,Cyc_Absyn_aggrarrow_exp);if(is_poly  && _tmp4FB)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL21B;};}_LL252: {struct Cyc_Absyn_Subscript_e_struct*_tmp4FC=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp4A0;if(_tmp4FC->tag != 24)goto _LL254;
else{_tmp4FD=_tmp4FC->f1;_tmp4FE=_tmp4FC->f2;}}_LL253: {void*_tmp6B3=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4FD->topt))->v);{void*_tmp6B4=
_tmp6B3;struct Cyc_List_List*_tmp6B6;struct Cyc_Absyn_PtrInfo _tmp6B8;void*_tmp6B9;
struct Cyc_Absyn_Tqual _tmp6BA;struct Cyc_Absyn_PtrAtts _tmp6BB;void*_tmp6BC;union
Cyc_Absyn_Constraint*_tmp6BD;union Cyc_Absyn_Constraint*_tmp6BE;union Cyc_Absyn_Constraint*
_tmp6BF;_LL30F: {struct Cyc_Absyn_TupleType_struct*_tmp6B5=(struct Cyc_Absyn_TupleType_struct*)
_tmp6B4;if(_tmp6B5->tag != 11)goto _LL311;else{_tmp6B6=_tmp6B5->f1;}}_LL310: Cyc_Toc_exp_to_c(
nv,_tmp4FD);Cyc_Toc_exp_to_c(nv,_tmp4FE);{unsigned int _tmp6C1;int _tmp6C2;struct
_tuple10 _tmp6C0=Cyc_Evexp_eval_const_uint_exp(_tmp4FE);_tmp6C1=_tmp6C0.f1;
_tmp6C2=_tmp6C0.f2;if(!_tmp6C2){const char*_tmpCA8;void*_tmpCA7;(_tmpCA7=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCA8="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpCA8,sizeof(char),44))),_tag_dyneither(_tmpCA7,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4FD,Cyc_Absyn_fieldname((int)(_tmp6C1 + 1)));
goto _LL30E;};_LL311: {struct Cyc_Absyn_PointerType_struct*_tmp6B7=(struct Cyc_Absyn_PointerType_struct*)
_tmp6B4;if(_tmp6B7->tag != 5)goto _LL313;else{_tmp6B8=_tmp6B7->f1;_tmp6B9=_tmp6B8.elt_typ;
_tmp6BA=_tmp6B8.elt_tq;_tmp6BB=_tmp6B8.ptr_atts;_tmp6BC=_tmp6BB.rgn;_tmp6BD=
_tmp6BB.nullable;_tmp6BE=_tmp6BB.bounds;_tmp6BF=_tmp6BB.zero_term;}}_LL312: {
struct Cyc_List_List*_tmp6C5=Cyc_Toc_get_relns(_tmp4FD);int in_bnds=0;{void*
_tmp6C6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6BE);_LL316:;_LL317: in_bnds=Cyc_Toc_check_bounds(_tmp6B3,_tmp6C5,_tmp4FD,
_tmp4FE);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpCAC;void*
_tmpCAB[1];struct Cyc_String_pa_struct _tmpCAA;(_tmpCAA.tag=0,((_tmpCAA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCAB[0]=&
_tmpCAA,Cyc_Tcutil_warn(e->loc,((_tmpCAC="bounds check necessary for %s",
_tag_dyneither(_tmpCAC,sizeof(char),30))),_tag_dyneither(_tmpCAB,sizeof(void*),1)))))));}
_LL315:;}Cyc_Toc_exp_to_c(nv,_tmp4FD);Cyc_Toc_exp_to_c(nv,_tmp4FE);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6CA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6BE);struct Cyc_Absyn_Exp*_tmp6CC;_LL319: {struct Cyc_Absyn_Upper_b_struct*
_tmp6CB=(struct Cyc_Absyn_Upper_b_struct*)_tmp6CA;if(_tmp6CB->tag != 1)goto _LL31B;
else{_tmp6CC=_tmp6CB->f1;}}_LL31A: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6BD);void*ta1=Cyc_Toc_typ_to_c(_tmp6B9);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6BA);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6BF)){if(!Cyc_Evexp_c_can_eval(_tmp6CC)){const char*_tmpCAF;void*_tmpCAE;(
_tmpCAE=0,Cyc_Tcutil_terr(e->loc,((_tmpCAF="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCAF,sizeof(char),40))),_tag_dyneither(_tmpCAE,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp4FD);struct Cyc_Absyn_Exp*_tmpCB0[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCB0[2]=_tmp4FE,((_tmpCB0[1]=_tmp6CC,((
_tmpCB0[0]=_tmp4FD,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCB0,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6CC)){const char*_tmpCB3;void*_tmpCB2;(
_tmpCB2=0,Cyc_Tcutil_terr(e->loc,((_tmpCB3="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCB3,sizeof(char),40))),_tag_dyneither(_tmpCB2,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCB6;void*_tmpCB5;(_tmpCB5=0,Cyc_Tcutil_warn(
e->loc,((_tmpCB6="inserted null check due to dereference",_tag_dyneither(_tmpCB6,
sizeof(char),39))),_tag_dyneither(_tmpCB5,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCB7[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCB7[3]=_tmp4FE,((_tmpCB7[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCB7[1]=
_tmp6CC,((_tmpCB7[0]=_tmp4FD,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCB7,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp6CC)){const char*_tmpCBA;void*_tmpCB9;(_tmpCB9=0,Cyc_Tcutil_terr(
e->loc,((_tmpCBA="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCBA,sizeof(char),40))),_tag_dyneither(_tmpCB9,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCBB[2];_tmp4FE->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCBB[1]=Cyc_Absyn_copy_exp(_tmp4FE),((_tmpCBB[0]=_tmp6CC,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCBB,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL318;}_LL31B: {struct Cyc_Absyn_DynEither_b_struct*_tmp6CD=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6CA;if(_tmp6CD->tag != 0)goto _LL318;}
_LL31C: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6B9);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6BA),
Cyc_Absyn_aggrmember_exp(_tmp4FD,Cyc_Toc_curr_sp,0)),_tmp4FE);}else{struct Cyc_Absyn_Exp*
_tmp6D9=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCBC[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6BA),Cyc_Absyn_fncall_exp(
_tmp6D9,((_tmpCBC[2]=_tmp4FE,((_tmpCBC[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCBC[0]=_tmp4FD,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCBC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL318;}
_LL318:;}goto _LL30E;}_LL313:;_LL314: {const char*_tmpCBF;void*_tmpCBE;(_tmpCBE=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCBF="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCBF,sizeof(char),49))),_tag_dyneither(_tmpCBE,sizeof(void*),0)));}_LL30E:;}
goto _LL21B;}_LL254: {struct Cyc_Absyn_Tuple_e_struct*_tmp4FF=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp4A0;if(_tmp4FF->tag != 25)goto _LL256;else{_tmp500=_tmp4FF->f1;}}_LL255: if(!
Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp500))->r;else{struct
Cyc_List_List*_tmp6DD=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp500);void*_tmp6DE=Cyc_Toc_add_tuple_type(
_tmp6DD);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp500 != 0;(_tmp500=_tmp500->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp500->hd);{struct _tuple15*
_tmpCC2;struct Cyc_List_List*_tmpCC1;dles=((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((
_tmpCC1->hd=((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2->f1=0,((_tmpCC2->f2=(
struct Cyc_Absyn_Exp*)_tmp500->hd,_tmpCC2)))))),((_tmpCC1->tl=dles,_tmpCC1))))));};}}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=
Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL21B;_LL256: {struct Cyc_Absyn_Array_e_struct*
_tmp501=(struct Cyc_Absyn_Array_e_struct*)_tmp4A0;if(_tmp501->tag != 27)goto _LL258;
else{_tmp502=_tmp501->f1;}}_LL257: e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp502);{
struct Cyc_List_List*_tmp6E1=_tmp502;for(0;_tmp6E1 != 0;_tmp6E1=_tmp6E1->tl){
struct _tuple15 _tmp6E3;struct Cyc_Absyn_Exp*_tmp6E4;struct _tuple15*_tmp6E2=(struct
_tuple15*)_tmp6E1->hd;_tmp6E3=*_tmp6E2;_tmp6E4=_tmp6E3.f2;Cyc_Toc_exp_to_c(nv,
_tmp6E4);}}goto _LL21B;_LL258: {struct Cyc_Absyn_Comprehension_e_struct*_tmp503=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp4A0;if(_tmp503->tag != 28)goto _LL25A;
else{_tmp504=_tmp503->f1;_tmp505=_tmp503->f2;_tmp506=_tmp503->f3;_tmp507=_tmp503->f4;}}
_LL259: {unsigned int _tmp6E6;int _tmp6E7;struct _tuple10 _tmp6E5=Cyc_Evexp_eval_const_uint_exp(
_tmp505);_tmp6E6=_tmp6E5.f1;_tmp6E7=_tmp6E5.f2;{void*_tmp6E8=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp506->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp506);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp506)){if(!_tmp6E7){
const char*_tmpCC5;void*_tmpCC4;(_tmpCC4=0,Cyc_Tcutil_terr(_tmp505->loc,((_tmpCC5="cannot determine value of constant",
_tag_dyneither(_tmpCC5,sizeof(char),35))),_tag_dyneither(_tmpCC4,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp6E6;++ i){struct _tuple15*_tmpCC8;struct Cyc_List_List*
_tmpCC7;es=((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7->hd=((_tmpCC8=
_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8->f1=0,((_tmpCC8->f2=_tmp506,_tmpCC8)))))),((
_tmpCC7->tl=es,_tmpCC7))))));}}if(_tmp507){struct Cyc_Absyn_Exp*_tmp6ED=Cyc_Toc_cast_it(
_tmp6E8,Cyc_Absyn_uint_exp(0,0));struct _tuple15*_tmpCCB;struct Cyc_List_List*
_tmpCCA;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA->hd=((
_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB->f1=0,((_tmpCCB->f2=_tmp6ED,
_tmpCCB)))))),((_tmpCCA->tl=0,_tmpCCA)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL21B;};};}_LL25A: {struct Cyc_Absyn_Aggregate_e_struct*_tmp508=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp4A0;if(_tmp508->tag != 29)goto _LL25C;
else{_tmp509=_tmp508->f1;_tmp50A=_tmp508->f2;_tmp50B=_tmp508->f3;_tmp50C=_tmp508->f4;}}
_LL25B: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp50A
!= 0,0,0,_tmp50B,_tmp509))->r;else{if(_tmp50C == 0){const char*_tmpCCE;void*
_tmpCCD;(_tmpCCD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCCE="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCCE,sizeof(
char),38))),_tag_dyneither(_tmpCCD,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp50C;struct Cyc_Toc_Env _tmp6F3;struct
_RegionHandle*_tmp6F4;struct Cyc_Toc_Env*_tmp6F2=nv;_tmp6F3=*_tmp6F2;_tmp6F4=
_tmp6F3.rgn;{struct Cyc_List_List*_tmp6F5=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp6F4,e->loc,_tmp50B,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50C->impl))->tagged){struct
_tuple20 _tmp6F7;struct Cyc_Absyn_Aggrfield*_tmp6F8;struct Cyc_Absyn_Exp*_tmp6F9;
struct _tuple20*_tmp6F6=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp6F5))->hd;_tmp6F7=*_tmp6F6;_tmp6F8=_tmp6F7.f1;_tmp6F9=_tmp6F7.f2;{void*
_tmp6FA=_tmp6F8->type;Cyc_Toc_exp_to_c(nv,_tmp6F9);if(Cyc_Toc_is_void_star_or_tvar(
_tmp6FA))_tmp6F9->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp6F9->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp50C,_tmp6F8->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpCD3;struct _tuple15*_tmpCD2;struct _tuple15*_tmpCD1[2];
struct Cyc_List_List*_tmp6FB=(_tmpCD1[1]=((_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((
_tmpCD2->f1=0,((_tmpCD2->f2=_tmp6F9,_tmpCD2)))))),((_tmpCD1[0]=((_tmpCD3=
_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3->f1=0,((_tmpCD3->f2=field_tag_exp,_tmpCD3)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCD1,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp6FB,0);struct Cyc_Absyn_FieldName_struct*_tmpCD9;struct Cyc_Absyn_FieldName_struct
_tmpCD8;void*_tmpCD7[1];struct Cyc_List_List*ds=(_tmpCD7[0]=(void*)((_tmpCD9=
_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9[0]=((_tmpCD8.tag=1,((_tmpCD8.f1=_tmp6F8->name,
_tmpCD8)))),_tmpCD9)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCD7,sizeof(void*),1)));struct _tuple15*_tmpCDC;struct _tuple15*
_tmpCDB[1];struct Cyc_List_List*dles=(_tmpCDB[0]=((_tmpCDC=_cycalloc(sizeof(*
_tmpCDC)),((_tmpCDC->f1=ds,((_tmpCDC->f2=umem,_tmpCDC)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDB,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp704=0;struct Cyc_List_List*_tmp705=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp705 != 0;_tmp705=_tmp705->tl){struct Cyc_List_List*
_tmp706=_tmp6F5;for(0;_tmp706 != 0;_tmp706=_tmp706->tl){if((*((struct _tuple20*)
_tmp706->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp705->hd){struct _tuple20
_tmp708;struct Cyc_Absyn_Aggrfield*_tmp709;struct Cyc_Absyn_Exp*_tmp70A;struct
_tuple20*_tmp707=(struct _tuple20*)_tmp706->hd;_tmp708=*_tmp707;_tmp709=_tmp708.f1;
_tmp70A=_tmp708.f2;{void*_tmp70B=_tmp709->type;Cyc_Toc_exp_to_c(nv,_tmp70A);if(
Cyc_Toc_is_void_star_or_tvar(_tmp70B))_tmp70A->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp70A->r,0));{struct _tuple15*_tmpCDF;struct Cyc_List_List*
_tmpCDE;_tmp704=((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE->hd=((_tmpCDF=
_cycalloc(sizeof(*_tmpCDF)),((_tmpCDF->f1=0,((_tmpCDF->f2=_tmp70A,_tmpCDF)))))),((
_tmpCDE->tl=_tmp704,_tmpCDE))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp704));}};};}
goto _LL21B;_LL25C: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp50D=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp4A0;if(_tmp50D->tag != 30)goto _LL25E;else{_tmp50E=(void*)_tmp50D->f1;_tmp50F=
_tmp50D->f2;}}_LL25D: {struct Cyc_List_List*fs;{void*_tmp70E=Cyc_Tcutil_compress(
_tmp50E);struct Cyc_List_List*_tmp710;_LL31E: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp70F=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp70E;if(_tmp70F->tag != 13)goto
_LL320;else{_tmp710=_tmp70F->f2;}}_LL31F: fs=_tmp710;goto _LL31D;_LL320:;_LL321: {
const char*_tmpCE3;void*_tmpCE2[1];struct Cyc_String_pa_struct _tmpCE1;(_tmpCE1.tag=
0,((_tmpCE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp50E)),((_tmpCE2[0]=& _tmpCE1,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE3="anon struct has type %s",
_tag_dyneither(_tmpCE3,sizeof(char),24))),_tag_dyneither(_tmpCE2,sizeof(void*),1)))))));}
_LL31D:;}{struct Cyc_Toc_Env _tmp715;struct _RegionHandle*_tmp716;struct Cyc_Toc_Env*
_tmp714=nv;_tmp715=*_tmp714;_tmp716=_tmp715.rgn;{struct Cyc_List_List*_tmp717=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp716,e->loc,_tmp50F,Cyc_Absyn_StructA,fs);for(0;_tmp717 != 0;_tmp717=_tmp717->tl){
struct _tuple20 _tmp719;struct Cyc_Absyn_Aggrfield*_tmp71A;struct Cyc_Absyn_Exp*
_tmp71B;struct _tuple20*_tmp718=(struct _tuple20*)_tmp717->hd;_tmp719=*_tmp718;
_tmp71A=_tmp719.f1;_tmp71B=_tmp719.f2;{void*_tmp71C=_tmp71A->type;Cyc_Toc_exp_to_c(
nv,_tmp71B);if(Cyc_Toc_is_void_star_or_tvar(_tmp71C))_tmp71B->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp71B->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp50F);}goto _LL21B;};}_LL25E: {struct Cyc_Absyn_Datatype_e_struct*_tmp510=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp4A0;if(_tmp510->tag != 31)goto _LL260;else{
_tmp511=_tmp510->f1;_tmp512=_tmp510->f2;_tmp513=_tmp510->f3;}}_LL25F: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp71D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp71E=Cyc_Absyn_var_exp(_tmp71D,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpCE4;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp513->name,((_tmpCE4="_struct",_tag_dyneither(_tmpCE4,sizeof(char),8)))));}
tag_exp=_tmp512->is_extensible?Cyc_Absyn_var_exp(_tmp513->name,0): Cyc_Toc_datatype_tag(
_tmp512,_tmp513->name);member_exp=_tmp71E;{struct Cyc_List_List*_tmp720=_tmp513->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp511 != 0;(
_tmp511=_tmp511->tl,_tmp720=_tmp720->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp511->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp720))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpCE7;struct
Cyc_List_List*_tmpCE6;dles=((_tmpCE6=_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6->hd=((
_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7->f1=0,((_tmpCE7->f2=cur_e,_tmpCE7)))))),((
_tmpCE6->tl=dles,_tmpCE6))))));};}{struct _tuple15*_tmpCEA;struct Cyc_List_List*
_tmpCE9;dles=((_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9->hd=((_tmpCEA=
_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->f1=0,((_tmpCEA->f2=tag_exp,_tmpCEA)))))),((
_tmpCE9->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpCE9))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpCEB;struct Cyc_List_List*_tmp725=(_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((
_tmpCEB->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpCEB->tl=0,_tmpCEB)))));{int i=1;for(0;_tmp511 != 0;(((_tmp511=
_tmp511->tl,i ++)),_tmp720=_tmp720->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp511->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp720))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp726=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpCEC;_tmp725=((_tmpCEC=_cycalloc(sizeof(*
_tmpCEC)),((_tmpCEC->hd=_tmp726,((_tmpCEC->tl=_tmp725,_tmpCEC))))));};}}{struct
Cyc_Absyn_Stmt*_tmp728=Cyc_Absyn_exp_stmt(_tmp71E,0);struct Cyc_List_List*_tmpCED;
struct Cyc_Absyn_Stmt*_tmp729=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((
_tmpCED->hd=_tmp728,((_tmpCED->tl=_tmp725,_tmpCED))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp71D,datatype_ctype,0,_tmp729,0));};}goto _LL21B;};}
_LL260: {struct Cyc_Absyn_Enum_e_struct*_tmp514=(struct Cyc_Absyn_Enum_e_struct*)
_tmp4A0;if(_tmp514->tag != 32)goto _LL262;}_LL261: goto _LL263;_LL262: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp515=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp4A0;if(_tmp515->tag != 33)goto
_LL264;}_LL263: goto _LL21B;_LL264: {struct Cyc_Absyn_Malloc_e_struct*_tmp516=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp4A0;if(_tmp516->tag != 34)goto _LL266;else{
_tmp517=_tmp516->f1;_tmp518=_tmp517.is_calloc;_tmp519=_tmp517.rgn;_tmp51A=
_tmp517.elt_type;_tmp51B=_tmp517.num_elts;_tmp51C=_tmp517.fat_result;}}_LL265: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp51A)));Cyc_Toc_exp_to_c(nv,
_tmp51B);if(_tmp51C){struct _tuple0*_tmp72C=Cyc_Toc_temp_var();struct _tuple0*
_tmp72D=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp518){xexp=_tmp51B;if(_tmp519 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp519;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp72C,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp51A,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp72C,0));}{struct Cyc_Absyn_Exp*_tmpCEE[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCEE[2]=Cyc_Absyn_var_exp(_tmp72C,0),((_tmpCEE[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCEE[0]=Cyc_Absyn_var_exp(_tmp72D,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCEE,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp51B,0);if(_tmp519 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp519;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp72C,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp51A,Cyc_Absyn_var_exp(_tmp72C,0));}{struct Cyc_Absyn_Exp*_tmpCEF[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCEF[2]=Cyc_Absyn_var_exp(_tmp72C,0),((_tmpCEF[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpCEF[0]=Cyc_Absyn_var_exp(_tmp72D,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCEF,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp730=Cyc_Absyn_declare_stmt(_tmp72C,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp72D,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp730);};}else{struct Cyc_Absyn_Exp*_tmp731=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp732=_tmp51B->r;union Cyc_Absyn_Cnst _tmp734;struct _tuple5 _tmp735;
int _tmp736;_LL323: {struct Cyc_Absyn_Const_e_struct*_tmp733=(struct Cyc_Absyn_Const_e_struct*)
_tmp732;if(_tmp733->tag != 0)goto _LL325;else{_tmp734=_tmp733->f1;if((_tmp734.Int_c).tag
!= 4)goto _LL325;_tmp735=(struct _tuple5)(_tmp734.Int_c).val;_tmp736=_tmp735.f2;
if(_tmp736 != 1)goto _LL325;}}_LL324: goto _LL322;_LL325:;_LL326: _tmp731=Cyc_Absyn_times_exp(
_tmp731,_tmp51B,0);goto _LL322;_LL322:;}if(_tmp519 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp519;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp731))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp51A,
_tmp731))->r;}}goto _LL21B;}_LL266: {struct Cyc_Absyn_Swap_e_struct*_tmp51D=(
struct Cyc_Absyn_Swap_e_struct*)_tmp4A0;if(_tmp51D->tag != 35)goto _LL268;else{
_tmp51E=_tmp51D->f1;_tmp51F=_tmp51D->f2;}}_LL267: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp51E->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp51F->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpCF2;void*_tmpCF1;(_tmpCF1=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF2="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpCF2,sizeof(char),57))),_tag_dyneither(_tmpCF1,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}Cyc_Toc_exp_to_c(nv,_tmp51E);Cyc_Toc_exp_to_c(
nv,_tmp51F);{struct Cyc_Absyn_Exp*_tmpCF3[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((
_tmpCF3[1]=Cyc_Toc_push_address_exp(_tmp51F),((_tmpCF3[0]=Cyc_Toc_push_address_exp(
_tmp51E),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCF3,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL21B;};}
_LL268: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp520=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp4A0;if(_tmp520->tag != 38)goto _LL26A;else{_tmp521=_tmp520->f1;_tmp522=_tmp520->f2;}}
_LL269: {void*_tmp73A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp521->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp521);{void*_tmp73B=
_tmp73A;struct Cyc_Absyn_AggrInfo _tmp73D;union Cyc_Absyn_AggrInfoU _tmp73E;struct
Cyc_Absyn_Aggrdecl**_tmp73F;struct Cyc_Absyn_Aggrdecl*_tmp740;_LL328: {struct Cyc_Absyn_AggrType_struct*
_tmp73C=(struct Cyc_Absyn_AggrType_struct*)_tmp73B;if(_tmp73C->tag != 12)goto
_LL32A;else{_tmp73D=_tmp73C->f1;_tmp73E=_tmp73D.aggr_info;if((_tmp73E.KnownAggr).tag
!= 2)goto _LL32A;_tmp73F=(struct Cyc_Absyn_Aggrdecl**)(_tmp73E.KnownAggr).val;
_tmp740=*_tmp73F;}}_LL329: {struct Cyc_Absyn_Exp*_tmp741=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp740,_tmp522),0);struct Cyc_Absyn_Exp*_tmp742=Cyc_Absyn_aggrmember_exp(
_tmp521,_tmp522,0);struct Cyc_Absyn_Exp*_tmp743=Cyc_Absyn_aggrmember_exp(_tmp742,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp743,_tmp741,0))->r;goto _LL327;}
_LL32A:;_LL32B: {const char*_tmpCF6;void*_tmpCF5;(_tmpCF5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF6="non-aggregate type in tagcheck",
_tag_dyneither(_tmpCF6,sizeof(char),31))),_tag_dyneither(_tmpCF5,sizeof(void*),0)));}
_LL327:;}goto _LL21B;}_LL26A: {struct Cyc_Absyn_StmtExp_e_struct*_tmp523=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp4A0;if(_tmp523->tag != 37)goto _LL26C;else{_tmp524=
_tmp523->f1;}}_LL26B: Cyc_Toc_stmt_to_c(nv,_tmp524);goto _LL21B;_LL26C: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp525=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp4A0;if(_tmp525->tag != 36)
goto _LL26E;}_LL26D: {const char*_tmpCF9;void*_tmpCF8;(_tmpCF8=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF9="UnresolvedMem",
_tag_dyneither(_tmpCF9,sizeof(char),14))),_tag_dyneither(_tmpCF8,sizeof(void*),0)));}
_LL26E: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp526=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp4A0;if(_tmp526->tag != 26)goto _LL270;}_LL26F: {const char*_tmpCFC;void*_tmpCFB;(
_tmpCFB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCFC="compoundlit",_tag_dyneither(_tmpCFC,sizeof(char),12))),_tag_dyneither(
_tmpCFB,sizeof(void*),0)));}_LL270: {struct Cyc_Absyn_Valueof_e_struct*_tmp527=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp4A0;if(_tmp527->tag != 39)goto _LL21B;}
_LL271: {const char*_tmpCFF;void*_tmpCFE;(_tmpCFE=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCFF="valueof(-)",
_tag_dyneither(_tmpCFF,sizeof(char),11))),_tag_dyneither(_tmpCFE,sizeof(void*),0)));}
_LL21B:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt);static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple21{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple22{struct _tuple0*f1;void*f2;};struct
_tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple21 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct
Cyc_List_List*decls);static struct _tuple21 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,
struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp74C=p->r;struct Cyc_Absyn_Vardecl*_tmp74E;struct
Cyc_Absyn_Vardecl _tmp74F;struct _tuple0*_tmp750;struct Cyc_Absyn_Pat*_tmp751;
struct Cyc_Absyn_Vardecl*_tmp753;struct Cyc_Absyn_Vardecl _tmp754;struct _tuple0*
_tmp755;struct Cyc_Absyn_Vardecl*_tmp758;struct Cyc_Absyn_Pat*_tmp759;enum Cyc_Absyn_Sign
_tmp75C;int _tmp75D;char _tmp75F;struct _dyneither_ptr _tmp761;struct Cyc_Absyn_Enumdecl*
_tmp763;struct Cyc_Absyn_Enumfield*_tmp764;void*_tmp766;struct Cyc_Absyn_Enumfield*
_tmp767;struct Cyc_Absyn_Pat*_tmp769;struct Cyc_Absyn_Pat _tmp76A;void*_tmp76B;
struct Cyc_Absyn_Datatypedecl*_tmp76D;struct Cyc_Absyn_Datatypefield*_tmp76E;
struct Cyc_List_List*_tmp76F;struct Cyc_List_List*_tmp771;struct Cyc_List_List*
_tmp773;struct Cyc_Absyn_AggrInfo*_tmp775;struct Cyc_Absyn_AggrInfo*_tmp777;struct
Cyc_Absyn_AggrInfo _tmp778;union Cyc_Absyn_AggrInfoU _tmp779;struct Cyc_List_List*
_tmp77A;struct Cyc_Absyn_Pat*_tmp77C;_LL32D: {struct Cyc_Absyn_Var_p_struct*
_tmp74D=(struct Cyc_Absyn_Var_p_struct*)_tmp74C;if(_tmp74D->tag != 1)goto _LL32F;
else{_tmp74E=_tmp74D->f1;_tmp74F=*_tmp74E;_tmp750=_tmp74F.name;_tmp751=_tmp74D->f2;}}
_LL32E: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp750,r),rgn,t,r,path,
_tmp751,fail_stmt,decls);_LL32F: {struct Cyc_Absyn_TagInt_p_struct*_tmp752=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp74C;if(_tmp752->tag != 3)goto _LL331;else{
_tmp753=_tmp752->f2;_tmp754=*_tmp753;_tmp755=_tmp754.name;}}_LL330: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp755,r);goto _LL332;_LL331: {struct Cyc_Absyn_Wild_p_struct*_tmp756=(
struct Cyc_Absyn_Wild_p_struct*)_tmp74C;if(_tmp756->tag != 0)goto _LL333;}_LL332: s=
Cyc_Toc_skip_stmt_dl();goto _LL32C;_LL333: {struct Cyc_Absyn_Reference_p_struct*
_tmp757=(struct Cyc_Absyn_Reference_p_struct*)_tmp74C;if(_tmp757->tag != 2)goto
_LL335;else{_tmp758=_tmp757->f1;_tmp759=_tmp757->f2;}}_LL334: {struct _tuple0*
_tmp780=Cyc_Toc_temp_var();{struct _tuple22*_tmpD02;struct Cyc_List_List*_tmpD01;
decls=((_tmpD01=_region_malloc(rgn,sizeof(*_tmpD01)),((_tmpD01->hd=((_tmpD02=
_region_malloc(rgn,sizeof(*_tmpD02)),((_tmpD02->f1=_tmp780,((_tmpD02->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD02)))))),((_tmpD01->tl=decls,_tmpD01))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp758->name,Cyc_Absyn_var_exp(_tmp780,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp780,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp783=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp759,fail_stmt,decls);_tmp783.f3=Cyc_Absyn_seq_stmt(s,_tmp783.f3,
0);return _tmp783;};}_LL335: {struct Cyc_Absyn_Null_p_struct*_tmp75A=(struct Cyc_Absyn_Null_p_struct*)
_tmp74C;if(_tmp75A->tag != 8)goto _LL337;}_LL336: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL32C;_LL337: {struct Cyc_Absyn_Int_p_struct*_tmp75B=(struct
Cyc_Absyn_Int_p_struct*)_tmp74C;if(_tmp75B->tag != 9)goto _LL339;else{_tmp75C=
_tmp75B->f1;_tmp75D=_tmp75B->f2;}}_LL338: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp75C,_tmp75D,0),fail_stmt);goto _LL32C;_LL339: {struct Cyc_Absyn_Char_p_struct*
_tmp75E=(struct Cyc_Absyn_Char_p_struct*)_tmp74C;if(_tmp75E->tag != 10)goto _LL33B;
else{_tmp75F=_tmp75E->f1;}}_LL33A: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp75F,0),fail_stmt);goto _LL32C;_LL33B: {struct Cyc_Absyn_Float_p_struct*_tmp760=(
struct Cyc_Absyn_Float_p_struct*)_tmp74C;if(_tmp760->tag != 11)goto _LL33D;else{
_tmp761=_tmp760->f1;}}_LL33C: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp761,
0),fail_stmt);goto _LL32C;_LL33D: {struct Cyc_Absyn_Enum_p_struct*_tmp762=(struct
Cyc_Absyn_Enum_p_struct*)_tmp74C;if(_tmp762->tag != 12)goto _LL33F;else{_tmp763=
_tmp762->f1;_tmp764=_tmp762->f2;}}_LL33E:{struct Cyc_Absyn_Enum_e_struct _tmpD05;
struct Cyc_Absyn_Enum_e_struct*_tmpD04;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04[0]=((_tmpD05.tag=32,((
_tmpD05.f1=_tmp764->name,((_tmpD05.f2=(struct Cyc_Absyn_Enumdecl*)_tmp763,((
_tmpD05.f3=(struct Cyc_Absyn_Enumfield*)_tmp764,_tmpD05)))))))),_tmpD04)))),0),
fail_stmt);}goto _LL32C;_LL33F: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp765=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp74C;if(_tmp765->tag != 13)goto _LL341;else{
_tmp766=(void*)_tmp765->f1;_tmp767=_tmp765->f2;}}_LL340:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpD08;struct Cyc_Absyn_AnonEnum_e_struct*_tmpD07;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07[0]=((_tmpD08.tag=33,((
_tmpD08.f1=_tmp767->name,((_tmpD08.f2=(void*)_tmp766,((_tmpD08.f3=(struct Cyc_Absyn_Enumfield*)
_tmp767,_tmpD08)))))))),_tmpD07)))),0),fail_stmt);}goto _LL32C;_LL341: {struct Cyc_Absyn_Pointer_p_struct*
_tmp768=(struct Cyc_Absyn_Pointer_p_struct*)_tmp74C;if(_tmp768->tag != 5)goto
_LL343;else{_tmp769=_tmp768->f1;_tmp76A=*_tmp769;_tmp76B=_tmp76A.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp76C=(struct Cyc_Absyn_Datatype_p_struct*)_tmp76B;if(_tmp76C->tag != 7)goto
_LL343;else{_tmp76D=_tmp76C->f1;_tmp76E=_tmp76C->f2;_tmp76F=_tmp76C->f3;}};}}
_LL342: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp788=Cyc_Toc_temp_var();const
char*_tmpD09;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp76E->name,((
_tmpD09="_struct",_tag_dyneither(_tmpD09,sizeof(char),8))));void*_tmp789=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp789,r);struct Cyc_List_List*_tmp78A=_tmp76E->typs;for(0;_tmp76F != 0;(((
_tmp76F=_tmp76F->tl,_tmp78A=((struct Cyc_List_List*)_check_null(_tmp78A))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp78B=(struct Cyc_Absyn_Pat*)_tmp76F->hd;if(_tmp78B->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp78C=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp78A))->hd)).f2;struct _tuple0*_tmp78D=Cyc_Toc_temp_var();
void*_tmp78E=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp78B->topt))->v;void*
_tmp78F=Cyc_Toc_typ_to_c(_tmp78E);struct Cyc_Absyn_Exp*_tmp790=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp788,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp78C)))_tmp790=Cyc_Toc_cast_it(_tmp78F,_tmp790);{struct
_tuple22*_tmpD0C;struct Cyc_List_List*_tmpD0B;decls=((_tmpD0B=_region_malloc(rgn,
sizeof(*_tmpD0B)),((_tmpD0B->hd=((_tmpD0C=_region_malloc(rgn,sizeof(*_tmpD0C)),((
_tmpD0C->f1=_tmp78D,((_tmpD0C->f2=_tmp78F,_tmpD0C)))))),((_tmpD0B->tl=decls,
_tmpD0B))))));}{struct _tuple21 _tmp793=Cyc_Toc_xlate_pat(nv,rgn,_tmp78E,Cyc_Absyn_var_exp(
_tmp78D,0),_tmp790,_tmp78B,fail_stmt,decls);nv=_tmp793.f1;decls=_tmp793.f2;{
struct Cyc_Absyn_Stmt*_tmp794=_tmp793.f3;struct Cyc_Absyn_Stmt*_tmp795=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp78D,0),_tmp790,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp795,_tmp794,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp788,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp76D->is_extensible?Cyc_Absyn_var_exp(_tmp76E->name,0): Cyc_Toc_datatype_tag(
_tmp76D,_tmp76E->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp788,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp788,_tmp789,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL32C;};};_LL343: {struct Cyc_Absyn_Datatype_p_struct*_tmp770=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp74C;if(_tmp770->tag != 7)goto _LL345;else{
_tmp771=_tmp770->f3;}}_LL344: _tmp773=_tmp771;goto _LL346;_LL345: {struct Cyc_Absyn_Tuple_p_struct*
_tmp772=(struct Cyc_Absyn_Tuple_p_struct*)_tmp74C;if(_tmp772->tag != 4)goto _LL347;
else{_tmp773=_tmp772->f1;}}_LL346: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp773 != 0;(_tmp773=_tmp773->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp797=(struct Cyc_Absyn_Pat*)
_tmp773->hd;if(_tmp797->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp798=Cyc_Toc_temp_var();void*_tmp799=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp797->topt))->v;{struct _tuple22*_tmpD0F;struct Cyc_List_List*_tmpD0E;decls=((
_tmpD0E=_region_malloc(rgn,sizeof(*_tmpD0E)),((_tmpD0E->hd=((_tmpD0F=
_region_malloc(rgn,sizeof(*_tmpD0F)),((_tmpD0F->f1=_tmp798,((_tmpD0F->f2=Cyc_Toc_typ_to_c(
_tmp799),_tmpD0F)))))),((_tmpD0E->tl=decls,_tmpD0E))))));}{struct _tuple21 _tmp79C=
Cyc_Toc_xlate_pat(nv,rgn,_tmp799,Cyc_Absyn_var_exp(_tmp798,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp797,fail_stmt,decls);nv=_tmp79C.f1;decls=
_tmp79C.f2;{struct Cyc_Absyn_Stmt*_tmp79D=_tmp79C.f3;struct Cyc_Absyn_Stmt*_tmp79E=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp798,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp79E,
_tmp79D,0),0);};};};}goto _LL32C;};_LL347: {struct Cyc_Absyn_Aggr_p_struct*_tmp774=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp74C;if(_tmp774->tag != 6)goto _LL349;else{
_tmp775=_tmp774->f1;if(_tmp775 != 0)goto _LL349;}}_LL348: {const char*_tmpD12;void*
_tmpD11;(_tmpD11=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD12="unresolved aggregate pattern!",_tag_dyneither(_tmpD12,sizeof(char),30))),
_tag_dyneither(_tmpD11,sizeof(void*),0)));}_LL349: {struct Cyc_Absyn_Aggr_p_struct*
_tmp776=(struct Cyc_Absyn_Aggr_p_struct*)_tmp74C;if(_tmp776->tag != 6)goto _LL34B;
else{_tmp777=_tmp776->f1;if(_tmp777 == 0)goto _LL34B;_tmp778=*_tmp777;_tmp779=
_tmp778.aggr_info;_tmp77A=_tmp776->f3;}}_LL34A: {struct Cyc_Absyn_Aggrdecl*
_tmp7A1=Cyc_Absyn_get_known_aggrdecl(_tmp779);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7A1->impl))->tagged){struct _tuple23 _tmp7A3;struct Cyc_List_List*
_tmp7A4;struct Cyc_Absyn_Pat*_tmp7A5;struct _tuple23*_tmp7A2=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp77A))->hd;_tmp7A3=*_tmp7A2;_tmp7A4=_tmp7A3.f1;
_tmp7A5=_tmp7A3.f2;{struct _dyneither_ptr*f;{void*_tmp7A6=(void*)((struct Cyc_List_List*)
_check_null(_tmp7A4))->hd;struct _dyneither_ptr*_tmp7A8;_LL354: {struct Cyc_Absyn_FieldName_struct*
_tmp7A7=(struct Cyc_Absyn_FieldName_struct*)_tmp7A6;if(_tmp7A7->tag != 1)goto
_LL356;else{_tmp7A8=_tmp7A7->f1;}}_LL355: f=_tmp7A8;goto _LL353;_LL356:;_LL357: {
const char*_tmpD15;void*_tmpD14;(_tmpD14=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD15="no field name in tagged union pattern",
_tag_dyneither(_tmpD15,sizeof(char),38))),_tag_dyneither(_tmpD14,sizeof(void*),0)));}
_LL353:;}{struct _tuple0*_tmp7AB=Cyc_Toc_temp_var();void*_tmp7AC=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp7A5->topt))->v;void*_tmp7AD=Cyc_Toc_typ_to_c(
_tmp7AC);{struct _tuple22*_tmpD18;struct Cyc_List_List*_tmpD17;decls=((_tmpD17=
_region_malloc(rgn,sizeof(*_tmpD17)),((_tmpD17->hd=((_tmpD18=_region_malloc(rgn,
sizeof(*_tmpD18)),((_tmpD18->f1=_tmp7AB,((_tmpD18->f2=_tmp7AD,_tmpD18)))))),((
_tmpD17->tl=decls,_tmpD17))))));}{struct Cyc_Absyn_Exp*_tmp7B0=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp7B1=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7B1=Cyc_Toc_cast_it(_tmp7AD,_tmp7B1);_tmp7B0=Cyc_Toc_cast_it(_tmp7AD,_tmp7B0);{
struct _tuple21 _tmp7B2=Cyc_Toc_xlate_pat(nv,rgn,_tmp7AC,Cyc_Absyn_var_exp(_tmp7AB,
0),_tmp7B0,_tmp7A5,fail_stmt,decls);nv=_tmp7B2.f1;decls=_tmp7B2.f2;{struct Cyc_Absyn_Stmt*
_tmp7B3=_tmp7B2.f3;struct Cyc_Absyn_Stmt*_tmp7B4=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp7A1,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7B5=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp7AB,0),_tmp7B1,0);s=Cyc_Absyn_seq_stmt(_tmp7B4,Cyc_Absyn_seq_stmt(_tmp7B5,
_tmp7B3,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp77A != 0;_tmp77A=
_tmp77A->tl){struct _tuple23*_tmp7B6=(struct _tuple23*)_tmp77A->hd;struct Cyc_Absyn_Pat*
_tmp7B7=(*_tmp7B6).f2;if(_tmp7B7->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7B8=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7B6).f1))->hd;struct _dyneither_ptr*_tmp7BA;_LL359: {struct Cyc_Absyn_FieldName_struct*
_tmp7B9=(struct Cyc_Absyn_FieldName_struct*)_tmp7B8;if(_tmp7B9->tag != 1)goto
_LL35B;else{_tmp7BA=_tmp7B9->f1;}}_LL35A: f=_tmp7BA;goto _LL358;_LL35B:;_LL35C: {
struct Cyc_Toc_Match_error_struct _tmpD1B;struct Cyc_Toc_Match_error_struct*_tmpD1A;(
int)_throw((void*)((_tmpD1A=_cycalloc_atomic(sizeof(*_tmpD1A)),((_tmpD1A[0]=((
_tmpD1B.tag=Cyc_Toc_Match_error,_tmpD1B)),_tmpD1A)))));}_LL358:;}{struct _tuple0*
_tmp7BD=Cyc_Toc_temp_var();void*_tmp7BE=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7B7->topt))->v;void*_tmp7BF=Cyc_Toc_typ_to_c(_tmp7BE);{struct _tuple22*
_tmpD1E;struct Cyc_List_List*_tmpD1D;decls=((_tmpD1D=_region_malloc(rgn,sizeof(*
_tmpD1D)),((_tmpD1D->hd=((_tmpD1E=_region_malloc(rgn,sizeof(*_tmpD1E)),((_tmpD1E->f1=
_tmp7BD,((_tmpD1E->f2=_tmp7BF,_tmpD1E)))))),((_tmpD1D->tl=decls,_tmpD1D))))));}{
struct _tuple21 _tmp7C2=Cyc_Toc_xlate_pat(nv,rgn,_tmp7BE,Cyc_Absyn_var_exp(_tmp7BD,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7B7,fail_stmt,decls);nv=_tmp7C2.f1;
decls=_tmp7C2.f2;{struct Cyc_Absyn_Exp*_tmp7C3=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7A1->impl))->fields,f)))->type))
_tmp7C3=Cyc_Toc_cast_it(_tmp7BF,_tmp7C3);{struct Cyc_Absyn_Stmt*_tmp7C4=_tmp7C2.f3;
struct Cyc_Absyn_Stmt*_tmp7C5=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7BD,0),
_tmp7C3,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7C5,_tmp7C4,0),0);};};};};};}}
goto _LL32C;}_LL34B: {struct Cyc_Absyn_Pointer_p_struct*_tmp77B=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp74C;if(_tmp77B->tag != 5)goto _LL34D;else{_tmp77C=_tmp77B->f1;}}_LL34C: {
struct _tuple0*_tmp7C6=Cyc_Toc_temp_var();void*_tmp7C7=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp77C->topt))->v;{struct _tuple22*_tmpD21;struct Cyc_List_List*
_tmpD20;decls=((_tmpD20=_region_malloc(rgn,sizeof(*_tmpD20)),((_tmpD20->hd=((
_tmpD21=_region_malloc(rgn,sizeof(*_tmpD21)),((_tmpD21->f1=_tmp7C6,((_tmpD21->f2=
Cyc_Toc_typ_to_c(_tmp7C7),_tmpD21)))))),((_tmpD20->tl=decls,_tmpD20))))));}{
struct _tuple21 _tmp7CA=Cyc_Toc_xlate_pat(nv,rgn,_tmp7C7,Cyc_Absyn_var_exp(_tmp7C6,
0),Cyc_Absyn_deref_exp(path,0),_tmp77C,fail_stmt,decls);nv=_tmp7CA.f1;decls=
_tmp7CA.f2;{struct Cyc_Absyn_Stmt*_tmp7CB=_tmp7CA.f3;struct Cyc_Absyn_Stmt*_tmp7CC=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7C6,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7CB,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp7CC,0);else{s=_tmp7CC;}goto _LL32C;};};}_LL34D: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp77D=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp74C;if(_tmp77D->tag != 14)goto
_LL34F;}_LL34E: {const char*_tmpD24;void*_tmpD23;(_tmpD23=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD24="unknownid pat",
_tag_dyneither(_tmpD24,sizeof(char),14))),_tag_dyneither(_tmpD23,sizeof(void*),0)));}
_LL34F: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp77E=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp74C;if(_tmp77E->tag != 15)goto _LL351;}_LL350: {const char*_tmpD27;void*_tmpD26;(
_tmpD26=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD27="unknowncall pat",_tag_dyneither(_tmpD27,sizeof(char),16))),
_tag_dyneither(_tmpD26,sizeof(void*),0)));}_LL351: {struct Cyc_Absyn_Exp_p_struct*
_tmp77F=(struct Cyc_Absyn_Exp_p_struct*)_tmp74C;if(_tmp77F->tag != 16)goto _LL32C;}
_LL352: {const char*_tmpD2A;void*_tmpD29;(_tmpD29=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD2A="exp pat",
_tag_dyneither(_tmpD2A,sizeof(char),8))),_tag_dyneither(_tmpD29,sizeof(void*),0)));}
_LL32C:;}{struct _tuple21 _tmpD2B;return(_tmpD2B.f1=nv,((_tmpD2B.f2=decls,((
_tmpD2B.f3=s,_tmpD2B)))));};}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc);static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpD2C;
return(_tmpD2C=_region_malloc(r,sizeof(*_tmpD2C)),((_tmpD2C->f1=Cyc_Toc_fresh_label(),((
_tmpD2C->f2=Cyc_Toc_fresh_label(),((_tmpD2C->f3=sc,_tmpD2C)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs);static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7D5=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp7D6=Cyc_Tcutil_compress(_tmp7D5);_LL35E: {struct Cyc_Absyn_IntType_struct*
_tmp7D7=(struct Cyc_Absyn_IntType_struct*)_tmp7D6;if(_tmp7D7->tag != 6)goto _LL360;}
_LL35F: goto _LL361;_LL360: {struct Cyc_Absyn_EnumType_struct*_tmp7D8=(struct Cyc_Absyn_EnumType_struct*)
_tmp7D6;if(_tmp7D8->tag != 14)goto _LL362;}_LL361: leave_as_switch=1;goto _LL35D;
_LL362:;_LL363: leave_as_switch=0;goto _LL35D;_LL35D:;}{struct Cyc_List_List*
_tmp7D9=scs;for(0;_tmp7D9 != 0;_tmp7D9=_tmp7D9->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7D9->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7D9->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp7DA=scs;for(0;_tmp7DA != 0;_tmp7DA=_tmp7DA->tl){struct Cyc_Absyn_Stmt*
_tmp7DB=((struct Cyc_Absyn_Switch_clause*)_tmp7DA->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp7DA->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7DB,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp7DD;struct _RegionHandle*_tmp7DE;struct Cyc_Toc_Env*_tmp7DC=
nv;_tmp7DD=*_tmp7DC;_tmp7DE=_tmp7DD.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp7DE,nv,next_l),_tmp7DB);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp7E0;
struct _RegionHandle*_tmp7E1;struct Cyc_Toc_Env*_tmp7DF=nv;_tmp7E0=*_tmp7DF;
_tmp7E1=_tmp7E0.rgn;{struct Cyc_Toc_Env*_tmp7E2=Cyc_Toc_share_env(_tmp7E1,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp7E1,Cyc_Toc_gen_label,
_tmp7E1,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp7E3=lscs;for(0;_tmp7E3 != 0;
_tmp7E3=_tmp7E3->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp7E3->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp7E3->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp7E3->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp7E5;struct Cyc_List_List*_tmp7E6;struct Cyc_Absyn_Stmt*_tmp7E7;struct _tuple21
_tmp7E4=Cyc_Toc_xlate_pat(_tmp7E2,_tmp7E1,_tmp7D5,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp7E5=_tmp7E4.f1;_tmp7E6=_tmp7E4.f2;_tmp7E7=_tmp7E4.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp7E8=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp7E5,_tmp7E8);_tmp7E7=Cyc_Absyn_seq_stmt(
_tmp7E7,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7E8,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp7E6;{
struct Cyc_List_List*_tmpD2D;nvs=((_tmpD2D=_region_malloc(_tmp7E1,sizeof(*_tmpD2D)),((
_tmpD2D->hd=_tmp7E5,((_tmpD2D->tl=nvs,_tmpD2D))))));}{struct Cyc_List_List*
_tmpD2E;test_stmts=((_tmpD2E=_region_malloc(_tmp7E1,sizeof(*_tmpD2E)),((_tmpD2E->hd=
_tmp7E7,((_tmpD2E->tl=test_stmts,_tmpD2E))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp7EC;struct _dyneither_ptr*_tmp7ED;struct _dyneither_ptr*_tmp7EE;
struct Cyc_Absyn_Switch_clause*_tmp7EF;struct _tuple24*_tmp7EB=(struct _tuple24*)
lscs->hd;_tmp7EC=*_tmp7EB;_tmp7ED=_tmp7EC.f1;_tmp7EE=_tmp7EC.f2;_tmp7EF=_tmp7EC.f3;{
struct Cyc_Toc_Env*_tmp7F0=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp7EF->body;struct Cyc_Toc_Env _tmp7F2;struct
_RegionHandle*_tmp7F3;struct Cyc_Toc_Env*_tmp7F1=_tmp7E2;_tmp7F2=*_tmp7F1;_tmp7F3=
_tmp7F2.rgn;if(lscs->tl != 0){struct _tuple24 _tmp7F5;struct _dyneither_ptr*_tmp7F6;
struct Cyc_Absyn_Switch_clause*_tmp7F7;struct _tuple24*_tmp7F4=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7F5=*_tmp7F4;_tmp7F6=_tmp7F5.f2;
_tmp7F7=_tmp7F5.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp7F3,
_tmp7F0,end_l,_tmp7F6,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp7F7->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp7F3,
_tmp7F0,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7ED,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp7EE,s,0),0);{struct Cyc_List_List*_tmpD2F;stmts=((_tmpD2F=_region_malloc(
_tmp7E1,sizeof(*_tmpD2F)),((_tmpD2F->hd=s,((_tmpD2F->tl=stmts,_tmpD2F))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp7FA;struct _tuple0*_tmp7FB;void*_tmp7FC;struct _tuple22*_tmp7F9=(struct
_tuple22*)decls->hd;_tmp7FA=*_tmp7F9;_tmp7FB=_tmp7FA.f1;_tmp7FC=_tmp7FA.f2;res=
Cyc_Absyn_declare_stmt(_tmp7FB,_tmp7FC,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*_tmpD30;return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((
_tmpD30->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD30->tl=0,_tmpD30)))))),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s);void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp7FE=s->r;struct Cyc_Absyn_Exp*_tmp801;struct
Cyc_Absyn_Stmt*_tmp803;struct Cyc_Absyn_Stmt*_tmp804;struct Cyc_Absyn_Exp*_tmp806;
struct Cyc_Absyn_Exp*_tmp808;struct Cyc_Absyn_Stmt*_tmp809;struct Cyc_Absyn_Stmt*
_tmp80A;struct _tuple8 _tmp80C;struct Cyc_Absyn_Exp*_tmp80D;struct Cyc_Absyn_Stmt*
_tmp80E;struct Cyc_Absyn_Stmt*_tmp810;struct Cyc_Absyn_Stmt*_tmp812;struct Cyc_Absyn_Stmt*
_tmp814;struct Cyc_Absyn_Exp*_tmp816;struct _tuple8 _tmp817;struct Cyc_Absyn_Exp*
_tmp818;struct _tuple8 _tmp819;struct Cyc_Absyn_Exp*_tmp81A;struct Cyc_Absyn_Stmt*
_tmp81B;struct Cyc_Absyn_Exp*_tmp81D;struct Cyc_List_List*_tmp81E;struct Cyc_List_List*
_tmp820;struct Cyc_Absyn_Switch_clause**_tmp821;struct Cyc_Absyn_Decl*_tmp823;
struct Cyc_Absyn_Stmt*_tmp824;struct _dyneither_ptr*_tmp826;struct Cyc_Absyn_Stmt*
_tmp827;struct Cyc_Absyn_Stmt*_tmp829;struct _tuple8 _tmp82A;struct Cyc_Absyn_Exp*
_tmp82B;struct Cyc_Absyn_Stmt*_tmp82D;struct Cyc_List_List*_tmp82E;struct Cyc_Absyn_Exp*
_tmp830;_LL365: {struct Cyc_Absyn_Skip_s_struct*_tmp7FF=(struct Cyc_Absyn_Skip_s_struct*)
_tmp7FE;if(_tmp7FF->tag != 0)goto _LL367;}_LL366: return;_LL367: {struct Cyc_Absyn_Exp_s_struct*
_tmp800=(struct Cyc_Absyn_Exp_s_struct*)_tmp7FE;if(_tmp800->tag != 1)goto _LL369;
else{_tmp801=_tmp800->f1;}}_LL368: Cyc_Toc_exp_to_c(nv,_tmp801);return;_LL369: {
struct Cyc_Absyn_Seq_s_struct*_tmp802=(struct Cyc_Absyn_Seq_s_struct*)_tmp7FE;if(
_tmp802->tag != 2)goto _LL36B;else{_tmp803=_tmp802->f1;_tmp804=_tmp802->f2;}}
_LL36A: Cyc_Toc_stmt_to_c(nv,_tmp803);s=_tmp804;continue;_LL36B: {struct Cyc_Absyn_Return_s_struct*
_tmp805=(struct Cyc_Absyn_Return_s_struct*)_tmp7FE;if(_tmp805->tag != 3)goto _LL36D;
else{_tmp806=_tmp805->f1;}}_LL36C: {struct Cyc_Core_Opt*topt=0;if(_tmp806 != 0){{
struct Cyc_Core_Opt*_tmpD31;topt=((_tmpD31=_cycalloc(sizeof(*_tmpD31)),((_tmpD31->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp806->topt))->v),
_tmpD31))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp806);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp832=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp833=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp832,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp832,(void*)topt->v,_tmp806,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp833,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL36D: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp807=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7FE;if(_tmp807->tag != 4)goto _LL36F;
else{_tmp808=_tmp807->f1;_tmp809=_tmp807->f2;_tmp80A=_tmp807->f3;}}_LL36E: Cyc_Toc_exp_to_c(
nv,_tmp808);Cyc_Toc_stmt_to_c(nv,_tmp809);s=_tmp80A;continue;_LL36F: {struct Cyc_Absyn_While_s_struct*
_tmp80B=(struct Cyc_Absyn_While_s_struct*)_tmp7FE;if(_tmp80B->tag != 5)goto _LL371;
else{_tmp80C=_tmp80B->f1;_tmp80D=_tmp80C.f1;_tmp80E=_tmp80B->f2;}}_LL370: Cyc_Toc_exp_to_c(
nv,_tmp80D);{struct Cyc_Toc_Env _tmp835;struct _RegionHandle*_tmp836;struct Cyc_Toc_Env*
_tmp834=nv;_tmp835=*_tmp834;_tmp836=_tmp835.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp836,nv),_tmp80E);return;};_LL371: {struct Cyc_Absyn_Break_s_struct*_tmp80F=(
struct Cyc_Absyn_Break_s_struct*)_tmp7FE;if(_tmp80F->tag != 6)goto _LL373;else{
_tmp810=_tmp80F->f1;}}_LL372: {struct Cyc_Toc_Env _tmp838;struct _dyneither_ptr**
_tmp839;struct Cyc_Toc_Env*_tmp837=nv;_tmp838=*_tmp837;_tmp839=_tmp838.break_lab;
if(_tmp839 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp839,0);{int dest_depth=_tmp810 == 0?0:
_tmp810->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL373: {struct Cyc_Absyn_Continue_s_struct*_tmp811=(struct Cyc_Absyn_Continue_s_struct*)
_tmp7FE;if(_tmp811->tag != 7)goto _LL375;else{_tmp812=_tmp811->f1;}}_LL374: {
struct Cyc_Toc_Env _tmp83B;struct _dyneither_ptr**_tmp83C;struct Cyc_Toc_Env*_tmp83A=
nv;_tmp83B=*_tmp83A;_tmp83C=_tmp83B.continue_lab;if(_tmp83C != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp83C,0);_tmp814=_tmp812;goto _LL376;}_LL375: {struct Cyc_Absyn_Goto_s_struct*
_tmp813=(struct Cyc_Absyn_Goto_s_struct*)_tmp7FE;if(_tmp813->tag != 8)goto _LL377;
else{_tmp814=_tmp813->f2;}}_LL376: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp814))->try_depth,s);return;_LL377: {struct Cyc_Absyn_For_s_struct*
_tmp815=(struct Cyc_Absyn_For_s_struct*)_tmp7FE;if(_tmp815->tag != 9)goto _LL379;
else{_tmp816=_tmp815->f1;_tmp817=_tmp815->f2;_tmp818=_tmp817.f1;_tmp819=_tmp815->f3;
_tmp81A=_tmp819.f1;_tmp81B=_tmp815->f4;}}_LL378: Cyc_Toc_exp_to_c(nv,_tmp816);Cyc_Toc_exp_to_c(
nv,_tmp818);Cyc_Toc_exp_to_c(nv,_tmp81A);{struct Cyc_Toc_Env _tmp83E;struct
_RegionHandle*_tmp83F;struct Cyc_Toc_Env*_tmp83D=nv;_tmp83E=*_tmp83D;_tmp83F=
_tmp83E.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp83F,nv),_tmp81B);return;};
_LL379: {struct Cyc_Absyn_Switch_s_struct*_tmp81C=(struct Cyc_Absyn_Switch_s_struct*)
_tmp7FE;if(_tmp81C->tag != 10)goto _LL37B;else{_tmp81D=_tmp81C->f1;_tmp81E=_tmp81C->f2;}}
_LL37A: Cyc_Toc_xlate_switch(nv,s,_tmp81D,_tmp81E);return;_LL37B: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp81F=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp7FE;if(_tmp81F->tag != 11)goto
_LL37D;else{_tmp820=_tmp81F->f1;_tmp821=_tmp81F->f2;}}_LL37C: {struct Cyc_Toc_Env
_tmp841;struct Cyc_Toc_FallthruInfo*_tmp842;struct Cyc_Toc_Env*_tmp840=nv;_tmp841=*
_tmp840;_tmp842=_tmp841.fallthru_info;if(_tmp842 == 0){const char*_tmpD34;void*
_tmpD33;(_tmpD33=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD34="fallthru in unexpected place",_tag_dyneither(_tmpD34,sizeof(char),29))),
_tag_dyneither(_tmpD33,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp846;struct
Cyc_List_List*_tmp847;struct Cyc_Dict_Dict _tmp848;struct Cyc_Toc_FallthruInfo
_tmp845=*_tmp842;_tmp846=_tmp845.label;_tmp847=_tmp845.binders;_tmp848=_tmp845.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp846,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp821)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp849=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp847);struct Cyc_List_List*_tmp84A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp820);for(0;_tmp849 != 0;(_tmp849=_tmp849->tl,
_tmp84A=_tmp84A->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp84A))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp848,(
struct _tuple0*)_tmp849->hd),(struct Cyc_Absyn_Exp*)_tmp84A->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL37D: {struct Cyc_Absyn_Decl_s_struct*_tmp822=(struct Cyc_Absyn_Decl_s_struct*)
_tmp7FE;if(_tmp822->tag != 12)goto _LL37F;else{_tmp823=_tmp822->f1;_tmp824=_tmp822->f2;}}
_LL37E:{void*_tmp84B=_tmp823->r;struct Cyc_Absyn_Vardecl*_tmp84D;struct Cyc_Absyn_Pat*
_tmp84F;struct Cyc_Absyn_Exp*_tmp850;struct Cyc_List_List*_tmp852;struct Cyc_Absyn_Fndecl*
_tmp854;struct Cyc_Absyn_Tvar*_tmp856;struct Cyc_Absyn_Vardecl*_tmp857;int _tmp858;
struct Cyc_Absyn_Exp*_tmp859;struct Cyc_Absyn_Exp*_tmp85B;struct Cyc_Absyn_Tvar*
_tmp85C;struct Cyc_Absyn_Vardecl*_tmp85D;_LL388: {struct Cyc_Absyn_Var_d_struct*
_tmp84C=(struct Cyc_Absyn_Var_d_struct*)_tmp84B;if(_tmp84C->tag != 0)goto _LL38A;
else{_tmp84D=_tmp84C->f1;}}_LL389: {struct Cyc_Toc_Env _tmp85F;struct _RegionHandle*
_tmp860;struct Cyc_Toc_Env*_tmp85E=nv;_tmp85F=*_tmp85E;_tmp860=_tmp85F.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD37;struct Cyc_Absyn_Local_b_struct*_tmpD36;
struct Cyc_Toc_Env*_tmp861=Cyc_Toc_add_varmap(_tmp860,nv,_tmp84D->name,Cyc_Absyn_varb_exp(
_tmp84D->name,(void*)((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36[0]=((
_tmpD37.tag=4,((_tmpD37.f1=_tmp84D,_tmpD37)))),_tmpD36)))),0));Cyc_Toc_local_decl_to_c(
_tmp861,_tmp861,_tmp84D,_tmp824);}goto _LL387;}_LL38A: {struct Cyc_Absyn_Let_d_struct*
_tmp84E=(struct Cyc_Absyn_Let_d_struct*)_tmp84B;if(_tmp84E->tag != 2)goto _LL38C;
else{_tmp84F=_tmp84E->f1;_tmp850=_tmp84E->f3;}}_LL38B:{void*_tmp864=_tmp84F->r;
struct Cyc_Absyn_Vardecl*_tmp866;struct Cyc_Absyn_Pat*_tmp867;struct Cyc_Absyn_Pat
_tmp868;void*_tmp869;_LL397: {struct Cyc_Absyn_Var_p_struct*_tmp865=(struct Cyc_Absyn_Var_p_struct*)
_tmp864;if(_tmp865->tag != 1)goto _LL399;else{_tmp866=_tmp865->f1;_tmp867=_tmp865->f2;
_tmp868=*_tmp867;_tmp869=_tmp868.r;{struct Cyc_Absyn_Wild_p_struct*_tmp86A=(
struct Cyc_Absyn_Wild_p_struct*)_tmp869;if(_tmp86A->tag != 0)goto _LL399;};}}_LL398: {
struct _tuple0*old_name=_tmp866->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp866->name=new_name;_tmp866->initializer=(struct Cyc_Absyn_Exp*)_tmp850;{
struct Cyc_Absyn_Var_d_struct _tmpD3A;struct Cyc_Absyn_Var_d_struct*_tmpD39;_tmp823->r=(
void*)((_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39[0]=((_tmpD3A.tag=0,((
_tmpD3A.f1=_tmp866,_tmpD3A)))),_tmpD39))));}{struct Cyc_Toc_Env _tmp86E;struct
_RegionHandle*_tmp86F;struct Cyc_Toc_Env*_tmp86D=nv;_tmp86E=*_tmp86D;_tmp86F=
_tmp86E.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD3D;struct Cyc_Absyn_Local_b_struct*
_tmpD3C;struct Cyc_Toc_Env*_tmp870=Cyc_Toc_add_varmap(_tmp86F,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C[0]=((_tmpD3D.tag=
4,((_tmpD3D.f1=_tmp866,_tmpD3D)))),_tmpD3C)))),0));Cyc_Toc_local_decl_to_c(
_tmp870,nv,_tmp866,_tmp824);}goto _LL396;};}_LL399:;_LL39A: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp84F,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp850->topt))->v,_tmp850,
_tmp824))->r;goto _LL396;_LL396:;}goto _LL387;_LL38C: {struct Cyc_Absyn_Letv_d_struct*
_tmp851=(struct Cyc_Absyn_Letv_d_struct*)_tmp84B;if(_tmp851->tag != 3)goto _LL38E;
else{_tmp852=_tmp851->f1;}}_LL38D: {struct Cyc_List_List*_tmp873=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp852);if(_tmp873 == 0){const char*_tmpD40;
void*_tmpD3F;(_tmpD3F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD40="empty Letv_d",_tag_dyneither(_tmpD40,sizeof(char),
13))),_tag_dyneither(_tmpD3F,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD43;struct Cyc_Absyn_Var_d_struct*_tmpD42;_tmp823->r=(void*)((_tmpD42=
_cycalloc(sizeof(*_tmpD42)),((_tmpD42[0]=((_tmpD43.tag=0,((_tmpD43.f1=(struct Cyc_Absyn_Vardecl*)
_tmp873->hd,_tmpD43)))),_tmpD42))));}_tmp873=_tmp873->tl;for(0;_tmp873 != 0;
_tmp873=_tmp873->tl){struct Cyc_Absyn_Var_d_struct _tmpD46;struct Cyc_Absyn_Var_d_struct*
_tmpD45;struct Cyc_Absyn_Decl*_tmp878=Cyc_Absyn_new_decl((void*)((_tmpD45=
_cycalloc(sizeof(*_tmpD45)),((_tmpD45[0]=((_tmpD46.tag=0,((_tmpD46.f1=(struct Cyc_Absyn_Vardecl*)
_tmp873->hd,_tmpD46)))),_tmpD45)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp878,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL387;}_LL38E: {struct Cyc_Absyn_Fn_d_struct*
_tmp853=(struct Cyc_Absyn_Fn_d_struct*)_tmp84B;if(_tmp853->tag != 1)goto _LL390;
else{_tmp854=_tmp853->f1;}}_LL38F: {struct _tuple0*_tmp87B=_tmp854->name;struct
Cyc_Toc_Env _tmp87D;struct _RegionHandle*_tmp87E;struct Cyc_Toc_Env*_tmp87C=nv;
_tmp87D=*_tmp87C;_tmp87E=_tmp87D.rgn;{struct Cyc_Toc_Env*_tmp87F=Cyc_Toc_add_varmap(
_tmp87E,nv,_tmp854->name,Cyc_Absyn_var_exp(_tmp87B,0));Cyc_Toc_fndecl_to_c(
_tmp87F,_tmp854,0);Cyc_Toc_stmt_to_c(_tmp87F,_tmp824);}goto _LL387;}_LL390: {
struct Cyc_Absyn_Region_d_struct*_tmp855=(struct Cyc_Absyn_Region_d_struct*)
_tmp84B;if(_tmp855->tag != 4)goto _LL392;else{_tmp856=_tmp855->f1;_tmp857=_tmp855->f2;
_tmp858=_tmp855->f3;_tmp859=_tmp855->f4;}}_LL391: {struct Cyc_Absyn_Stmt*_tmp880=
_tmp824;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp857->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp882;struct _RegionHandle*_tmp883;struct Cyc_Toc_Env*
_tmp881=nv;_tmp882=*_tmp881;_tmp883=_tmp882.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp883,nv,x_var,x_exp),_tmp880);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp880,0))->r;
else{if(_tmp859 == 0){struct Cyc_Absyn_Exp*_tmpD49[1];struct Cyc_Absyn_Exp*_tmpD48[
1];struct Cyc_List_List*_tmpD47;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD47=
_cycalloc(sizeof(*_tmpD47)),((_tmpD47->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD47->tl=0,_tmpD47)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD48[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD48,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp880,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD49[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD49,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp859);{struct Cyc_Absyn_Exp*
_tmpD4B[1];struct Cyc_Absyn_Exp*_tmpD4A[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD4A[1]=(struct Cyc_Absyn_Exp*)_tmp859,((_tmpD4A[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD4A,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp880,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD4B[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD4B,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL392: {struct Cyc_Absyn_Alias_d_struct*_tmp85A=(struct Cyc_Absyn_Alias_d_struct*)
_tmp84B;if(_tmp85A->tag != 5)goto _LL394;else{_tmp85B=_tmp85A->f1;_tmp85C=_tmp85A->f2;
_tmp85D=_tmp85A->f3;}}_LL393: {struct _tuple0*old_name=_tmp85D->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp85D->name=new_name;_tmp85D->initializer=(
struct Cyc_Absyn_Exp*)_tmp85B;{struct Cyc_Absyn_Decl_s_struct _tmpD5A;struct Cyc_Absyn_Var_d_struct*
_tmpD59;struct Cyc_Absyn_Var_d_struct _tmpD58;struct Cyc_Absyn_Decl*_tmpD57;struct
Cyc_Absyn_Decl_s_struct*_tmpD56;s->r=(void*)((_tmpD56=_cycalloc(sizeof(*_tmpD56)),((
_tmpD56[0]=((_tmpD5A.tag=12,((_tmpD5A.f1=((_tmpD57=_cycalloc(sizeof(*_tmpD57)),((
_tmpD57->r=(void*)((_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59[0]=((_tmpD58.tag=
0,((_tmpD58.f1=_tmp85D,_tmpD58)))),_tmpD59)))),((_tmpD57->loc=0,_tmpD57)))))),((
_tmpD5A.f2=_tmp824,_tmpD5A)))))),_tmpD56))));}{struct Cyc_Toc_Env _tmp88F;struct
_RegionHandle*_tmp890;struct Cyc_Toc_Env*_tmp88E=nv;_tmp88F=*_tmp88E;_tmp890=
_tmp88F.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD5D;struct Cyc_Absyn_Local_b_struct*
_tmpD5C;struct Cyc_Toc_Env*_tmp891=Cyc_Toc_add_varmap(_tmp890,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C[0]=((_tmpD5D.tag=
4,((_tmpD5D.f1=_tmp85D,_tmpD5D)))),_tmpD5C)))),0));Cyc_Toc_local_decl_to_c(
_tmp891,nv,_tmp85D,_tmp824);}return;};}_LL394:;_LL395: {const char*_tmpD60;void*
_tmpD5F;(_tmpD5F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD60="bad nested declaration within function",_tag_dyneither(_tmpD60,sizeof(
char),39))),_tag_dyneither(_tmpD5F,sizeof(void*),0)));}_LL387:;}return;_LL37F: {
struct Cyc_Absyn_Label_s_struct*_tmp825=(struct Cyc_Absyn_Label_s_struct*)_tmp7FE;
if(_tmp825->tag != 13)goto _LL381;else{_tmp826=_tmp825->f1;_tmp827=_tmp825->f2;}}
_LL380: s=_tmp827;continue;_LL381: {struct Cyc_Absyn_Do_s_struct*_tmp828=(struct
Cyc_Absyn_Do_s_struct*)_tmp7FE;if(_tmp828->tag != 14)goto _LL383;else{_tmp829=
_tmp828->f1;_tmp82A=_tmp828->f2;_tmp82B=_tmp82A.f1;}}_LL382: {struct Cyc_Toc_Env
_tmp897;struct _RegionHandle*_tmp898;struct Cyc_Toc_Env*_tmp896=nv;_tmp897=*
_tmp896;_tmp898=_tmp897.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp898,nv),
_tmp829);Cyc_Toc_exp_to_c(nv,_tmp82B);return;}_LL383: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp82C=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp7FE;if(_tmp82C->tag != 15)goto
_LL385;else{_tmp82D=_tmp82C->f1;_tmp82E=_tmp82C->f2;}}_LL384: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpD61;
e_exp->topt=((_tmpD61=_cycalloc(sizeof(*_tmpD61)),((_tmpD61->v=e_typ,_tmpD61))));}{
struct Cyc_Toc_Env _tmp89B;struct _RegionHandle*_tmp89C;struct Cyc_Toc_Env*_tmp89A=
nv;_tmp89B=*_tmp89A;_tmp89C=_tmp89B.rgn;{struct Cyc_Toc_Env*_tmp89D=Cyc_Toc_add_varmap(
_tmp89C,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp89D,_tmp82D);{struct Cyc_Absyn_Stmt*
_tmp89E=Cyc_Absyn_seq_stmt(_tmp82D,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp89F=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp8A0=
Cyc_Absyn_var_exp(_tmp89F,0);struct Cyc_Absyn_Vardecl*_tmp8A1=Cyc_Absyn_new_vardecl(
_tmp89F,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpD62;_tmp8A0->topt=((
_tmpD62=_cycalloc(sizeof(*_tmpD62)),((_tmpD62->v=Cyc_Absyn_exn_typ(),_tmpD62))));}{
struct Cyc_Core_Opt*_tmpD73;struct Cyc_Absyn_Var_p_struct*_tmpD72;struct Cyc_Absyn_Pat*
_tmpD71;struct Cyc_Core_Opt*_tmpD70;struct Cyc_Absyn_Var_p_struct _tmpD6F;struct Cyc_Absyn_Pat*
_tmpD6E;struct Cyc_Absyn_Pat*_tmp8A3=(_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((
_tmpD6E->r=(void*)((_tmpD72=_cycalloc(sizeof(*_tmpD72)),((_tmpD72[0]=((_tmpD6F.tag=
1,((_tmpD6F.f1=_tmp8A1,((_tmpD6F.f2=((_tmpD71=_cycalloc(sizeof(*_tmpD71)),((
_tmpD71->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD71->topt=((_tmpD70=_cycalloc(
sizeof(*_tmpD70)),((_tmpD70->v=Cyc_Absyn_exn_typ(),_tmpD70)))),((_tmpD71->loc=0,
_tmpD71)))))))),_tmpD6F)))))),_tmpD72)))),((_tmpD6E->topt=((_tmpD73=_cycalloc(
sizeof(*_tmpD73)),((_tmpD73->v=Cyc_Absyn_exn_typ(),_tmpD73)))),((_tmpD6E->loc=0,
_tmpD6E)))))));struct Cyc_Absyn_Exp*_tmp8A4=Cyc_Absyn_throw_exp(_tmp8A0,0);{
struct Cyc_Core_Opt*_tmpD74;_tmp8A4->topt=((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((
_tmpD74->v=(void*)& Cyc_Absyn_VoidType_val,_tmpD74))));}{struct Cyc_Absyn_Stmt*
_tmp8A6=Cyc_Absyn_exp_stmt(_tmp8A4,0);struct Cyc_Core_Opt*_tmpD7A;struct Cyc_List_List*
_tmpD79;struct Cyc_Absyn_Switch_clause*_tmpD78;struct Cyc_Absyn_Switch_clause*
_tmp8A7=(_tmpD78=_cycalloc(sizeof(*_tmpD78)),((_tmpD78->pattern=_tmp8A3,((
_tmpD78->pat_vars=((_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((_tmpD7A->v=((_tmpD79=
_cycalloc(sizeof(*_tmpD79)),((_tmpD79->hd=_tmp8A1,((_tmpD79->tl=0,_tmpD79)))))),
_tmpD7A)))),((_tmpD78->where_clause=0,((_tmpD78->body=_tmp8A6,((_tmpD78->loc=0,
_tmpD78)))))))))));struct Cyc_List_List*_tmpD7B;struct Cyc_Absyn_Stmt*_tmp8A8=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp82E,((_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((_tmpD7B->hd=_tmp8A7,((_tmpD7B->tl=
0,_tmpD7B))))))),0);Cyc_Toc_stmt_to_c(_tmp89D,_tmp8A8);{struct Cyc_List_List*
_tmpD7C;struct Cyc_Absyn_Exp*_tmp8A9=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpD7C=_cycalloc(sizeof(*_tmpD7C)),((_tmpD7C->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpD7C->tl=0,_tmpD7C)))))),0);struct Cyc_List_List*
_tmpD7D;struct Cyc_Absyn_Stmt*_tmp8AA=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpD7D->tl=0,_tmpD7D)))))),0),0);struct Cyc_Absyn_Exp*_tmp8AB=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp8AC=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp8AA,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp8AB,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp8A9,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8AC,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp89E,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8A8,0),0),0),0),0),0))->r;};};};};}
return;};}_LL385: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp82F=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp7FE;if(_tmp82F->tag != 16)goto _LL364;else{_tmp830=_tmp82F->f1;}}_LL386: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp830);{struct Cyc_List_List*
_tmpD7E;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((_tmpD7E->hd=_tmp830,((_tmpD7E->tl=0,
_tmpD7E)))))),0));};}return;_LL364:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8BA=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8BA;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8BB=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8BC=f->args;for(0;_tmp8BC != 0;_tmp8BC=_tmp8BC->tl){struct
_tuple0*_tmpD7F;struct _tuple0*_tmp8BD=(_tmpD7F=_cycalloc(sizeof(*_tmpD7F)),((
_tmpD7F->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD7F->f2=(*((struct
_tuple25*)_tmp8BC->hd)).f1,_tmpD7F)))));(*((struct _tuple25*)_tmp8BC->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8BC->hd)).f3);_tmp8BB=Cyc_Toc_add_varmap(frgn,_tmp8BB,
_tmp8BD,Cyc_Absyn_var_exp(_tmp8BD,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8C0;struct Cyc_Absyn_Tqual
_tmp8C1;void*_tmp8C2;int _tmp8C3;struct Cyc_Absyn_VarargInfo _tmp8BF=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8C0=_tmp8BF.name;_tmp8C1=_tmp8BF.tq;_tmp8C2=
_tmp8BF.type;_tmp8C3=_tmp8BF.inject;{void*_tmp8C4=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8C2,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8C1,Cyc_Absyn_false_conref));struct
_tuple0*_tmpD80;struct _tuple0*_tmp8C5=(_tmpD80=_cycalloc(sizeof(*_tmpD80)),((
_tmpD80->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD80->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8C0))->v,_tmpD80)))));{
struct _tuple25*_tmpD83;struct Cyc_List_List*_tmpD82;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD82=
_cycalloc(sizeof(*_tmpD82)),((_tmpD82->hd=((_tmpD83=_cycalloc(sizeof(*_tmpD83)),((
_tmpD83->f1=(struct _dyneither_ptr*)_tmp8C0->v,((_tmpD83->f2=_tmp8C1,((_tmpD83->f3=
_tmp8C4,_tmpD83)))))))),((_tmpD82->tl=0,_tmpD82)))))));}_tmp8BB=Cyc_Toc_add_varmap(
frgn,_tmp8BB,_tmp8C5,Cyc_Absyn_var_exp(_tmp8C5,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp8C9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8C9 != 0;_tmp8C9=_tmp8C9->tl){((struct Cyc_Absyn_Vardecl*)_tmp8C9->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8C9->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8BB),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s);static enum Cyc_Absyn_Scope
Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL39B: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL39C: return Cyc_Absyn_Extern;
default: _LL39D: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad);static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple0*
_tmp8CA=ad->name;struct _DynRegionHandle*_tmp8CC;struct Cyc_Dict_Dict*_tmp8CD;
struct Cyc_Toc_TocState _tmp8CB=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8CC=_tmp8CB.dyn;_tmp8CD=_tmp8CB.aggrs_so_far;{struct _DynRegionFrame _tmp8CE;
struct _RegionHandle*d=_open_dynregion(& _tmp8CE,_tmp8CC);{int seen_defn_before;
struct _tuple14**_tmp8CF=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k))Cyc_Dict_lookup_opt)(*_tmp8CD,_tmp8CA);if(_tmp8CF == 0){seen_defn_before=0;{
struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){struct _tuple14*_tmpD84;v=((
_tmpD84=_region_malloc(d,sizeof(*_tmpD84)),((_tmpD84->f1=ad,((_tmpD84->f2=Cyc_Absyn_strctq(
_tmp8CA),_tmpD84))))));}else{struct _tuple14*_tmpD85;v=((_tmpD85=_region_malloc(d,
sizeof(*_tmpD85)),((_tmpD85->f1=ad,((_tmpD85->f2=Cyc_Absyn_unionq_typ(_tmp8CA),
_tmpD85))))));}*_tmp8CD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8CD,_tmp8CA,v);};}else{struct
_tuple14 _tmp8D3;struct Cyc_Absyn_Aggrdecl*_tmp8D4;void*_tmp8D5;struct _tuple14*
_tmp8D2=*_tmp8CF;_tmp8D3=*_tmp8D2;_tmp8D4=_tmp8D3.f1;_tmp8D5=_tmp8D3.f2;if(
_tmp8D4->impl == 0){{struct _tuple14*_tmpD86;*_tmp8CD=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8CD,
_tmp8CA,((_tmpD86=_region_malloc(d,sizeof(*_tmpD86)),((_tmpD86->f1=ad,((_tmpD86->f2=
_tmp8D5,_tmpD86)))))));}seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*
_tmpD87;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD87=_cycalloc(sizeof(*_tmpD87)),((
_tmpD87->kind=ad->kind,((_tmpD87->sc=Cyc_Absyn_Public,((_tmpD87->name=ad->name,((
_tmpD87->tvs=0,((_tmpD87->impl=0,((_tmpD87->attributes=ad->attributes,_tmpD87)))))))))))));
if(ad->impl != 0  && !seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpD88;
new_ad->impl=((_tmpD88=_cycalloc(sizeof(*_tmpD88)),((_tmpD88->exist_vars=0,((
_tmpD88->rgn_po=0,((_tmpD88->fields=0,((_tmpD88->tagged=0,_tmpD88))))))))));}{
struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*_tmp8D8=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp8D8 != 0;_tmp8D8=_tmp8D8->tl){struct Cyc_Absyn_Aggrfield*
_tmp8D9=(struct Cyc_Absyn_Aggrfield*)_tmp8D8->hd;struct Cyc_Absyn_Aggrfield*
_tmpD89;struct Cyc_Absyn_Aggrfield*_tmp8DA=(_tmpD89=_cycalloc(sizeof(*_tmpD89)),((
_tmpD89->name=_tmp8D9->name,((_tmpD89->tq=Cyc_Toc_mt_tq,((_tmpD89->type=Cyc_Toc_typ_to_c_array(
_tmp8D9->type),((_tmpD89->width=_tmp8D9->width,((_tmpD89->attributes=_tmp8D9->attributes,
_tmpD89)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp8DB=_tmp8DA->type;struct _dyneither_ptr*_tmp8DC=_tmp8DA->name;const char*
_tmpD8E;void*_tmpD8D[2];struct Cyc_String_pa_struct _tmpD8C;struct Cyc_String_pa_struct
_tmpD8B;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD8B.tag=0,((_tmpD8B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8DC),((_tmpD8C.tag=0,((_tmpD8C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD8D[0]=&
_tmpD8C,((_tmpD8D[1]=& _tmpD8B,Cyc_aprintf(((_tmpD8E="_union_%s_%s",
_tag_dyneither(_tmpD8E,sizeof(char),13))),_tag_dyneither(_tmpD8D,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD8F;struct _dyneither_ptr*str=(_tmpD8F=_cycalloc(sizeof(*
_tmpD8F)),((_tmpD8F[0]=s,_tmpD8F)));struct Cyc_Absyn_Aggrfield*_tmpD90;struct Cyc_Absyn_Aggrfield*
_tmp8DD=(_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90->name=Cyc_Toc_val_sp,((
_tmpD90->tq=Cyc_Toc_mt_tq,((_tmpD90->type=_tmp8DB,((_tmpD90->width=0,((_tmpD90->attributes=
0,_tmpD90)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD91;struct Cyc_Absyn_Aggrfield*
_tmp8DE=(_tmpD91=_cycalloc(sizeof(*_tmpD91)),((_tmpD91->name=Cyc_Toc_tag_sp,((
_tmpD91->tq=Cyc_Toc_mt_tq,((_tmpD91->type=Cyc_Absyn_sint_typ,((_tmpD91->width=0,((
_tmpD91->attributes=0,_tmpD91)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD92[2];
struct Cyc_List_List*_tmp8DF=(_tmpD92[1]=_tmp8DD,((_tmpD92[0]=_tmp8DE,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD92,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpD97;struct _tuple0*_tmpD96;struct Cyc_Absyn_Aggrdecl*
_tmpD95;struct Cyc_Absyn_Aggrdecl*_tmp8E0=(_tmpD95=_cycalloc(sizeof(*_tmpD95)),((
_tmpD95->kind=Cyc_Absyn_StructA,((_tmpD95->sc=Cyc_Absyn_Public,((_tmpD95->name=((
_tmpD96=_cycalloc(sizeof(*_tmpD96)),((_tmpD96->f1=Cyc_Absyn_Loc_n,((_tmpD96->f2=
str,_tmpD96)))))),((_tmpD95->tvs=0,((_tmpD95->impl=((_tmpD97=_cycalloc(sizeof(*
_tmpD97)),((_tmpD97->exist_vars=0,((_tmpD97->rgn_po=0,((_tmpD97->fields=_tmp8DF,((
_tmpD97->tagged=0,_tmpD97)))))))))),((_tmpD95->attributes=0,_tmpD95)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpD9D;struct Cyc_Absyn_Aggr_d_struct _tmpD9C;
struct Cyc_List_List*_tmpD9B;Cyc_Toc_result_decls=((_tmpD9B=_cycalloc(sizeof(*
_tmpD9B)),((_tmpD9B->hd=Cyc_Absyn_new_decl((void*)((_tmpD9D=_cycalloc(sizeof(*
_tmpD9D)),((_tmpD9D[0]=((_tmpD9C.tag=6,((_tmpD9C.f1=_tmp8E0,_tmpD9C)))),_tmpD9D)))),
0),((_tmpD9B->tl=Cyc_Toc_result_decls,_tmpD9B))))));}_tmp8DA->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpD9E;new_fields=((_tmpD9E=_cycalloc(sizeof(*
_tmpD9E)),((_tmpD9E->hd=_tmp8DA,((_tmpD9E->tl=new_fields,_tmpD9E))))));};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}{struct Cyc_Absyn_Decl*_tmpDA8;struct Cyc_Absyn_Aggr_d_struct
_tmpDA7;struct Cyc_Absyn_Aggr_d_struct*_tmpDA6;struct Cyc_List_List*_tmpDA5;Cyc_Toc_result_decls=((
_tmpDA5=_cycalloc(sizeof(*_tmpDA5)),((_tmpDA5->hd=((_tmpDA8=_cycalloc(sizeof(*
_tmpDA8)),((_tmpDA8->r=(void*)((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((_tmpDA6[0]=((
_tmpDA7.tag=6,((_tmpDA7.f1=new_ad,_tmpDA7)))),_tmpDA6)))),((_tmpDA8->loc=0,
_tmpDA8)))))),((_tmpDA5->tl=Cyc_Toc_result_decls,_tmpDA5))))));};};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud);static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp8F7;struct Cyc_Set_Set**_tmp8F8;struct Cyc_Toc_TocState
_tmp8F6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8F7=
_tmp8F6.dyn;_tmp8F8=_tmp8F6.datatypes_so_far;{struct _DynRegionFrame _tmp8F9;
struct _RegionHandle*d=_open_dynregion(& _tmp8F9,_tmp8F7);{struct _tuple0*_tmp8FA=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp8F8,_tmp8FA)){_npop_handler(0);return;}*_tmp8F8=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp8F8,_tmp8FA);}{struct Cyc_List_List*_tmp8FB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp8FB != 0;_tmp8FB=_tmp8FB->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8FB->hd;struct Cyc_List_List*_tmp8FC=0;int i=
1;{struct Cyc_List_List*_tmp8FD=f->typs;for(0;_tmp8FD != 0;(_tmp8FD=_tmp8FD->tl,i
++)){struct _dyneither_ptr*_tmp8FE=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpDA9;struct Cyc_Absyn_Aggrfield*_tmp8FF=(_tmpDA9=_cycalloc(sizeof(*_tmpDA9)),((
_tmpDA9->name=_tmp8FE,((_tmpDA9->tq=(*((struct _tuple9*)_tmp8FD->hd)).f1,((
_tmpDA9->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp8FD->hd)).f2),((
_tmpDA9->width=0,((_tmpDA9->attributes=0,_tmpDA9)))))))))));struct Cyc_List_List*
_tmpDAA;_tmp8FC=((_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA->hd=_tmp8FF,((
_tmpDAA->tl=_tmp8FC,_tmpDAA))))));}}{struct Cyc_Absyn_Aggrfield*_tmpDAD;struct Cyc_List_List*
_tmpDAC;_tmp8FC=((_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->hd=((_tmpDAD=
_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD->name=Cyc_Toc_tag_sp,((_tmpDAD->tq=Cyc_Toc_mt_tq,((
_tmpDAD->type=Cyc_Absyn_sint_typ,((_tmpDAD->width=0,((_tmpDAD->attributes=0,
_tmpDAD)))))))))))),((_tmpDAC->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp8FC),_tmpDAC))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDB2;const char*_tmpDB1;struct Cyc_Absyn_Aggrdecl*_tmpDB0;struct Cyc_Absyn_Aggrdecl*
_tmp904=(_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0->kind=Cyc_Absyn_StructA,((
_tmpDB0->sc=Cyc_Absyn_Public,((_tmpDB0->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDB1="_struct",_tag_dyneither(_tmpDB1,sizeof(char),8)))),((_tmpDB0->tvs=0,((
_tmpDB0->impl=((_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2->exist_vars=0,((
_tmpDB2->rgn_po=0,((_tmpDB2->fields=_tmp8FC,((_tmpDB2->tagged=0,_tmpDB2)))))))))),((
_tmpDB0->attributes=0,_tmpDB0)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDB8;
struct Cyc_Absyn_Aggr_d_struct _tmpDB7;struct Cyc_List_List*_tmpDB6;Cyc_Toc_result_decls=((
_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->hd=Cyc_Absyn_new_decl((void*)((
_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDB7.tag=6,((_tmpDB7.f1=
_tmp904,_tmpDB7)))),_tmpDB8)))),0),((_tmpDB6->tl=Cyc_Toc_result_decls,_tmpDB6))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd);static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp90C;struct Cyc_Dict_Dict*_tmp90D;struct
Cyc_Toc_TocState _tmp90B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp90C=_tmp90B.dyn;_tmp90D=_tmp90B.xdatatypes_so_far;{struct _DynRegionFrame
_tmp90E;struct _RegionHandle*d=_open_dynregion(& _tmp90E,_tmp90C);{struct _tuple0*
_tmp90F=xd->name;struct Cyc_List_List*_tmp910=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp910 != 0;_tmp910=_tmp910->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp910->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp911=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp912=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp911,Cyc_Absyn_false_conref,0);int*_tmp913=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp90D,f->name);int
_tmp914;_LL3A0: if(_tmp913 != 0)goto _LL3A2;_LL3A1: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp915=Cyc_Absyn_new_vardecl(f->name,_tmp912,
initopt);_tmp915->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDBE;struct Cyc_Absyn_Var_d_struct
_tmpDBD;struct Cyc_List_List*_tmpDBC;Cyc_Toc_result_decls=((_tmpDBC=_cycalloc(
sizeof(*_tmpDBC)),((_tmpDBC->hd=Cyc_Absyn_new_decl((void*)((_tmpDBE=_cycalloc(
sizeof(*_tmpDBE)),((_tmpDBE[0]=((_tmpDBD.tag=0,((_tmpDBD.f1=_tmp915,_tmpDBD)))),
_tmpDBE)))),0),((_tmpDBC->tl=Cyc_Toc_result_decls,_tmpDBC))))));}*_tmp90D=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp90D,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp919=f->typs;for(0;_tmp919 != 0;(_tmp919=_tmp919->tl,i ++)){
struct Cyc_Int_pa_struct _tmpDC6;void*_tmpDC5[1];const char*_tmpDC4;struct
_dyneither_ptr*_tmpDC3;struct _dyneither_ptr*_tmp91A=(_tmpDC3=_cycalloc(sizeof(*
_tmpDC3)),((_tmpDC3[0]=(struct _dyneither_ptr)((_tmpDC6.tag=1,((_tmpDC6.f1=(
unsigned long)i,((_tmpDC5[0]=& _tmpDC6,Cyc_aprintf(((_tmpDC4="f%d",_tag_dyneither(
_tmpDC4,sizeof(char),4))),_tag_dyneither(_tmpDC5,sizeof(void*),1)))))))),_tmpDC3)));
struct Cyc_Absyn_Aggrfield*_tmpDC7;struct Cyc_Absyn_Aggrfield*_tmp91B=(_tmpDC7=
_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7->name=_tmp91A,((_tmpDC7->tq=(*((struct
_tuple9*)_tmp919->hd)).f1,((_tmpDC7->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple9*)_tmp919->hd)).f2),((_tmpDC7->width=0,((_tmpDC7->attributes=0,_tmpDC7)))))))))));
struct Cyc_List_List*_tmpDC8;fields=((_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((
_tmpDC8->hd=_tmp91B,((_tmpDC8->tl=fields,_tmpDC8))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpDCB;struct Cyc_List_List*_tmpDCA;fields=((_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((
_tmpDCA->hd=((_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB->name=Cyc_Toc_tag_sp,((
_tmpDCB->tq=Cyc_Toc_mt_tq,((_tmpDCB->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpDCB->width=0,((_tmpDCB->attributes=0,_tmpDCB)))))))))))),((
_tmpDCA->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpDCA))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpDD0;const char*_tmpDCF;
struct Cyc_Absyn_Aggrdecl*_tmpDCE;struct Cyc_Absyn_Aggrdecl*_tmp924=(_tmpDCE=
_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE->kind=Cyc_Absyn_StructA,((_tmpDCE->sc=Cyc_Absyn_Public,((
_tmpDCE->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpDCF="_struct",
_tag_dyneither(_tmpDCF,sizeof(char),8)))),((_tmpDCE->tvs=0,((_tmpDCE->impl=((
_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0->exist_vars=0,((_tmpDD0->rgn_po=0,((
_tmpDD0->fields=fields,((_tmpDD0->tagged=0,_tmpDD0)))))))))),((_tmpDCE->attributes=
0,_tmpDCE)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpDD6;struct Cyc_Absyn_Aggr_d_struct
_tmpDD5;struct Cyc_List_List*_tmpDD4;Cyc_Toc_result_decls=((_tmpDD4=_cycalloc(
sizeof(*_tmpDD4)),((_tmpDD4->hd=Cyc_Absyn_new_decl((void*)((_tmpDD6=_cycalloc(
sizeof(*_tmpDD6)),((_tmpDD6[0]=((_tmpDD5.tag=6,((_tmpDD5.f1=_tmp924,_tmpDD5)))),
_tmpDD6)))),0),((_tmpDD4->tl=Cyc_Toc_result_decls,_tmpDD4))))));}goto _LL39F;};};};}
_LL3A2: if(_tmp913 == 0)goto _LL3A4;_tmp914=*_tmp913;if(_tmp914 != 0)goto _LL3A4;
_LL3A3: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp92B=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp92C=Cyc_Absyn_new_vardecl(f->name,_tmp912,(
struct Cyc_Absyn_Exp*)_tmp92B);_tmp92C->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpDDC;struct Cyc_Absyn_Var_d_struct _tmpDDB;struct Cyc_List_List*_tmpDDA;Cyc_Toc_result_decls=((
_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA->hd=Cyc_Absyn_new_decl((void*)((
_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC[0]=((_tmpDDB.tag=0,((_tmpDDB.f1=
_tmp92C,_tmpDDB)))),_tmpDDC)))),0),((_tmpDDA->tl=Cyc_Toc_result_decls,_tmpDDA))))));}*
_tmp90D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp90D,f->name,1);}goto _LL39F;_LL3A4:;_LL3A5: goto _LL39F;_LL39F:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Enumdecl*ed);static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Enumdecl*ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static
void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp930=init->r;struct Cyc_Absyn_Vardecl*_tmp932;struct Cyc_Absyn_Exp*_tmp933;
struct Cyc_Absyn_Exp*_tmp934;int _tmp935;_LL3A7: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp931=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp930;if(_tmp931->tag != 28)
goto _LL3A9;else{_tmp932=_tmp931->f1;_tmp933=_tmp931->f2;_tmp934=_tmp931->f3;
_tmp935=_tmp931->f4;}}_LL3A8: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp932,_tmp933,_tmp934,_tmp935,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3A6;_LL3A9:;_LL3AA: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp937;struct _RegionHandle*_tmp938;struct Cyc_Toc_Env*_tmp936=init_nv;_tmp937=*
_tmp936;_tmp938=_tmp937.rgn;{struct Cyc_Toc_Env*_tmp939=Cyc_Toc_set_toplevel(
_tmp938,init_nv);Cyc_Toc_exp_to_c(_tmp939,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3A6;_LL3A6:;}else{void*_tmp93A=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp93C;void*_tmp93D;struct Cyc_Absyn_Exp*_tmp93E;union Cyc_Absyn_Constraint*
_tmp93F;_LL3AC:{struct Cyc_Absyn_ArrayType_struct*_tmp93B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp93A;if(_tmp93B->tag != 9)goto _LL3AE;else{_tmp93C=_tmp93B->f1;_tmp93D=_tmp93C.elt_type;
_tmp93E=_tmp93C.num_elts;_tmp93F=_tmp93C.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp93F))goto _LL3AE;_LL3AD: if(_tmp93E == 0){const char*
_tmpDDF;void*_tmpDDE;(_tmpDDE=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDDF="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpDDF,sizeof(char),55))),_tag_dyneither(_tmpDDE,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp93E;struct Cyc_Absyn_Exp*
_tmp942=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp943=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp942,
_tmp943,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3AB;};_LL3AE:;_LL3AF: goto _LL3AB;
_LL3AB:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp945;struct _RegionHandle*
_tmp946;struct Cyc_Toc_Env*_tmp944=nv;_tmp945=*_tmp944;_tmp946=_tmp945.rgn;{
struct Cyc_Absyn_Stmt*_tmp947=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp948=Cyc_Toc_share_env(_tmp946,nv);struct Cyc_Toc_Env*_tmp94A;struct Cyc_List_List*
_tmp94B;struct Cyc_Absyn_Stmt*_tmp94C;struct _tuple21 _tmp949=Cyc_Toc_xlate_pat(
_tmp948,_tmp946,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp94A=_tmp949.f1;_tmp94B=_tmp949.f2;_tmp94C=_tmp949.f3;Cyc_Toc_stmt_to_c(
_tmp94A,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp94C,s,0),0);for(0;_tmp94B != 0;_tmp94B=_tmp94B->tl){
struct _tuple22 _tmp94E;struct _tuple0*_tmp94F;void*_tmp950;struct _tuple22*_tmp94D=(
struct _tuple22*)_tmp94B->hd;_tmp94E=*_tmp94D;_tmp94F=_tmp94E.f1;_tmp950=_tmp94E.f2;
s=Cyc_Absyn_declare_stmt(_tmp94F,_tmp950,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e);static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp951=e->r;struct Cyc_Absyn_Exp*_tmp953;struct Cyc_Absyn_Exp*_tmp955;struct
Cyc_Absyn_Exp*_tmp957;struct Cyc_Absyn_Exp*_tmp959;struct Cyc_Absyn_Exp*_tmp95B;
struct Cyc_Absyn_Exp*_tmp95D;struct Cyc_Absyn_Exp*_tmp95F;struct Cyc_Absyn_Exp*
_tmp961;struct Cyc_List_List*_tmp963;struct Cyc_Absyn_Exp*_tmp965;struct Cyc_Absyn_Exp*
_tmp966;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_Absyn_Exp*_tmp969;struct Cyc_Absyn_Exp*
_tmp96B;struct Cyc_Absyn_Exp*_tmp96C;struct Cyc_Absyn_Exp*_tmp96E;struct Cyc_Absyn_Exp*
_tmp96F;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_Absyn_Exp*_tmp972;struct Cyc_Absyn_Exp*
_tmp974;struct Cyc_Absyn_Exp*_tmp975;struct Cyc_Absyn_Exp*_tmp977;struct Cyc_Absyn_Exp*
_tmp978;struct Cyc_Absyn_Exp*_tmp979;struct Cyc_Absyn_Exp*_tmp97B;struct Cyc_List_List*
_tmp97C;struct Cyc_Absyn_Exp*_tmp97E;struct Cyc_List_List*_tmp97F;void*_tmp981;
void**_tmp982;struct Cyc_Absyn_Exp*_tmp983;struct _tuple7*_tmp985;struct _tuple7
_tmp986;void*_tmp987;void**_tmp988;struct Cyc_List_List*_tmp989;struct Cyc_List_List*
_tmp98B;struct Cyc_List_List*_tmp98D;void*_tmp98F;void**_tmp990;void*_tmp992;void**
_tmp993;struct Cyc_Absyn_Stmt*_tmp995;struct Cyc_Absyn_MallocInfo _tmp997;struct Cyc_Absyn_MallocInfo*
_tmp998;_LL3B1: {struct Cyc_Absyn_Deref_e_struct*_tmp952=(struct Cyc_Absyn_Deref_e_struct*)
_tmp951;if(_tmp952->tag != 21)goto _LL3B3;else{_tmp953=_tmp952->f1;}}_LL3B2:
_tmp955=_tmp953;goto _LL3B4;_LL3B3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp954=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp951;if(_tmp954->tag != 22)goto _LL3B5;
else{_tmp955=_tmp954->f1;}}_LL3B4: _tmp957=_tmp955;goto _LL3B6;_LL3B5: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp956=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp951;if(_tmp956->tag != 23)goto
_LL3B7;else{_tmp957=_tmp956->f1;}}_LL3B6: _tmp959=_tmp957;goto _LL3B8;_LL3B7: {
struct Cyc_Absyn_Address_e_struct*_tmp958=(struct Cyc_Absyn_Address_e_struct*)
_tmp951;if(_tmp958->tag != 16)goto _LL3B9;else{_tmp959=_tmp958->f1;}}_LL3B8:
_tmp95B=_tmp959;goto _LL3BA;_LL3B9: {struct Cyc_Absyn_Throw_e_struct*_tmp95A=(
struct Cyc_Absyn_Throw_e_struct*)_tmp951;if(_tmp95A->tag != 12)goto _LL3BB;else{
_tmp95B=_tmp95A->f1;}}_LL3BA: _tmp95D=_tmp95B;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp95C=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp951;if(_tmp95C->tag != 13)
goto _LL3BD;else{_tmp95D=_tmp95C->f1;}}_LL3BC: _tmp95F=_tmp95D;goto _LL3BE;_LL3BD: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp95E=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp951;if(_tmp95E->tag != 19)goto _LL3BF;else{_tmp95F=_tmp95E->f1;}}_LL3BE:
_tmp961=_tmp95F;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_Increment_e_struct*_tmp960=(
struct Cyc_Absyn_Increment_e_struct*)_tmp951;if(_tmp960->tag != 5)goto _LL3C1;else{
_tmp961=_tmp960->f1;}}_LL3C0: Cyc_Toc_exptypes_to_c(_tmp961);goto _LL3B0;_LL3C1: {
struct Cyc_Absyn_Primop_e_struct*_tmp962=(struct Cyc_Absyn_Primop_e_struct*)
_tmp951;if(_tmp962->tag != 3)goto _LL3C3;else{_tmp963=_tmp962->f2;}}_LL3C2:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp963);goto _LL3B0;_LL3C3: {struct Cyc_Absyn_And_e_struct*_tmp964=(struct Cyc_Absyn_And_e_struct*)
_tmp951;if(_tmp964->tag != 7)goto _LL3C5;else{_tmp965=_tmp964->f1;_tmp966=_tmp964->f2;}}
_LL3C4: _tmp968=_tmp965;_tmp969=_tmp966;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_Or_e_struct*
_tmp967=(struct Cyc_Absyn_Or_e_struct*)_tmp951;if(_tmp967->tag != 8)goto _LL3C7;
else{_tmp968=_tmp967->f1;_tmp969=_tmp967->f2;}}_LL3C6: _tmp96B=_tmp968;_tmp96C=
_tmp969;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_SeqExp_e_struct*_tmp96A=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp951;if(_tmp96A->tag != 9)goto _LL3C9;else{_tmp96B=_tmp96A->f1;_tmp96C=_tmp96A->f2;}}
_LL3C8: _tmp96E=_tmp96B;_tmp96F=_tmp96C;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Subscript_e_struct*
_tmp96D=(struct Cyc_Absyn_Subscript_e_struct*)_tmp951;if(_tmp96D->tag != 24)goto
_LL3CB;else{_tmp96E=_tmp96D->f1;_tmp96F=_tmp96D->f2;}}_LL3CA: _tmp971=_tmp96E;
_tmp972=_tmp96F;goto _LL3CC;_LL3CB: {struct Cyc_Absyn_Swap_e_struct*_tmp970=(
struct Cyc_Absyn_Swap_e_struct*)_tmp951;if(_tmp970->tag != 35)goto _LL3CD;else{
_tmp971=_tmp970->f1;_tmp972=_tmp970->f2;}}_LL3CC: _tmp974=_tmp971;_tmp975=_tmp972;
goto _LL3CE;_LL3CD: {struct Cyc_Absyn_AssignOp_e_struct*_tmp973=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp951;if(_tmp973->tag != 4)goto _LL3CF;else{_tmp974=_tmp973->f1;_tmp975=_tmp973->f3;}}
_LL3CE: Cyc_Toc_exptypes_to_c(_tmp974);Cyc_Toc_exptypes_to_c(_tmp975);goto _LL3B0;
_LL3CF: {struct Cyc_Absyn_Conditional_e_struct*_tmp976=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp951;if(_tmp976->tag != 6)goto _LL3D1;else{_tmp977=_tmp976->f1;_tmp978=_tmp976->f2;
_tmp979=_tmp976->f3;}}_LL3D0: Cyc_Toc_exptypes_to_c(_tmp977);Cyc_Toc_exptypes_to_c(
_tmp978);Cyc_Toc_exptypes_to_c(_tmp979);goto _LL3B0;_LL3D1: {struct Cyc_Absyn_FnCall_e_struct*
_tmp97A=(struct Cyc_Absyn_FnCall_e_struct*)_tmp951;if(_tmp97A->tag != 11)goto
_LL3D3;else{_tmp97B=_tmp97A->f1;_tmp97C=_tmp97A->f2;}}_LL3D2: _tmp97E=_tmp97B;
_tmp97F=_tmp97C;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp97D=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp951;if(_tmp97D->tag != 10)goto _LL3D5;
else{_tmp97E=_tmp97D->f1;_tmp97F=_tmp97D->f2;}}_LL3D4: Cyc_Toc_exptypes_to_c(
_tmp97E);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp97F);goto _LL3B0;_LL3D5: {struct Cyc_Absyn_Cast_e_struct*
_tmp980=(struct Cyc_Absyn_Cast_e_struct*)_tmp951;if(_tmp980->tag != 15)goto _LL3D7;
else{_tmp981=(void**)& _tmp980->f1;_tmp982=(void**)((void**)& _tmp980->f1);_tmp983=
_tmp980->f2;}}_LL3D6:*_tmp982=Cyc_Toc_typ_to_c(*_tmp982);Cyc_Toc_exptypes_to_c(
_tmp983);goto _LL3B0;_LL3D7: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp984=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp951;if(_tmp984->tag != 26)goto _LL3D9;
else{_tmp985=_tmp984->f1;_tmp986=*_tmp985;_tmp987=_tmp986.f3;_tmp988=(void**)&(*
_tmp984->f1).f3;_tmp989=_tmp984->f2;}}_LL3D8:*_tmp988=Cyc_Toc_typ_to_c(*_tmp988);
_tmp98B=_tmp989;goto _LL3DA;_LL3D9: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp98A=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp951;if(_tmp98A->tag != 36)
goto _LL3DB;else{_tmp98B=_tmp98A->f2;}}_LL3DA: _tmp98D=_tmp98B;goto _LL3DC;_LL3DB: {
struct Cyc_Absyn_Array_e_struct*_tmp98C=(struct Cyc_Absyn_Array_e_struct*)_tmp951;
if(_tmp98C->tag != 27)goto _LL3DD;else{_tmp98D=_tmp98C->f1;}}_LL3DC: for(0;_tmp98D
!= 0;_tmp98D=_tmp98D->tl){struct Cyc_Absyn_Exp*_tmp9A8;struct _tuple15 _tmp9A7=*((
struct _tuple15*)_tmp98D->hd);_tmp9A8=_tmp9A7.f2;Cyc_Toc_exptypes_to_c(_tmp9A8);}
goto _LL3B0;_LL3DD: {struct Cyc_Absyn_Offsetof_e_struct*_tmp98E=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp951;if(_tmp98E->tag != 20)goto _LL3DF;else{_tmp98F=(void**)& _tmp98E->f1;
_tmp990=(void**)((void**)& _tmp98E->f1);}}_LL3DE: _tmp993=_tmp990;goto _LL3E0;
_LL3DF: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp991=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp951;if(_tmp991->tag != 18)goto _LL3E1;else{_tmp992=(void**)& _tmp991->f1;
_tmp993=(void**)((void**)& _tmp991->f1);}}_LL3E0:*_tmp993=Cyc_Toc_typ_to_c(*
_tmp993);goto _LL3B0;_LL3E1: {struct Cyc_Absyn_StmtExp_e_struct*_tmp994=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp951;if(_tmp994->tag != 37)goto _LL3E3;else{_tmp995=_tmp994->f1;}}_LL3E2: Cyc_Toc_stmttypes_to_c(
_tmp995);goto _LL3B0;_LL3E3: {struct Cyc_Absyn_Malloc_e_struct*_tmp996=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp951;if(_tmp996->tag != 34)goto _LL3E5;else{_tmp997=_tmp996->f1;_tmp998=(struct
Cyc_Absyn_MallocInfo*)& _tmp996->f1;}}_LL3E4: if(_tmp998->elt_type != 0){void**
_tmpDE0;_tmp998->elt_type=((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp998->elt_type))),_tmpDE0))));}Cyc_Toc_exptypes_to_c(
_tmp998->num_elts);goto _LL3B0;_LL3E5: {struct Cyc_Absyn_Const_e_struct*_tmp999=(
struct Cyc_Absyn_Const_e_struct*)_tmp951;if(_tmp999->tag != 0)goto _LL3E7;}_LL3E6:
goto _LL3E8;_LL3E7: {struct Cyc_Absyn_Var_e_struct*_tmp99A=(struct Cyc_Absyn_Var_e_struct*)
_tmp951;if(_tmp99A->tag != 1)goto _LL3E9;}_LL3E8: goto _LL3EA;_LL3E9: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp99B=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp951;if(_tmp99B->tag != 2)goto
_LL3EB;}_LL3EA: goto _LL3EC;_LL3EB: {struct Cyc_Absyn_Enum_e_struct*_tmp99C=(struct
Cyc_Absyn_Enum_e_struct*)_tmp951;if(_tmp99C->tag != 32)goto _LL3ED;}_LL3EC: goto
_LL3EE;_LL3ED: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp99D=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp951;if(_tmp99D->tag != 33)goto _LL3EF;}_LL3EE: goto _LL3B0;_LL3EF: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp99E=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp951;if(_tmp99E->tag != 30)goto
_LL3F1;}_LL3F0: goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Datatype_e_struct*_tmp99F=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp951;if(_tmp99F->tag != 31)goto _LL3F3;}
_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Aggregate_e_struct*_tmp9A0=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp951;if(_tmp9A0->tag != 29)goto _LL3F5;}_LL3F4: goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp9A1=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp951;if(_tmp9A1->tag != 28)
goto _LL3F7;}_LL3F6: goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Tuple_e_struct*_tmp9A2=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp951;if(_tmp9A2->tag != 25)goto _LL3F9;}_LL3F8:
goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Instantiate_e_struct*_tmp9A3=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp951;if(_tmp9A3->tag != 14)goto _LL3FB;}_LL3FA: goto _LL3FC;_LL3FB: {struct Cyc_Absyn_New_e_struct*
_tmp9A4=(struct Cyc_Absyn_New_e_struct*)_tmp951;if(_tmp9A4->tag != 17)goto _LL3FD;}
_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Valueof_e_struct*_tmp9A5=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp951;if(_tmp9A5->tag != 39)goto _LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp9A6=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp951;if(_tmp9A6->tag != 38)goto
_LL3B0;}_LL400:{const char*_tmpDE3;void*_tmpDE2;(_tmpDE2=0,Cyc_Tcutil_terr(e->loc,((
_tmpDE3="Cyclone expression within C code",_tag_dyneither(_tmpDE3,sizeof(char),
33))),_tag_dyneither(_tmpDE2,sizeof(void*),0)));}goto _LL3B0;_LL3B0:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d);static void Cyc_Toc_decltypes_to_c(
struct Cyc_Absyn_Decl*d){void*_tmp9AC=d->r;struct Cyc_Absyn_Vardecl*_tmp9AE;struct
Cyc_Absyn_Fndecl*_tmp9B0;struct Cyc_Absyn_Aggrdecl*_tmp9B2;struct Cyc_Absyn_Enumdecl*
_tmp9B4;struct Cyc_Absyn_Typedefdecl*_tmp9B6;_LL402: {struct Cyc_Absyn_Var_d_struct*
_tmp9AD=(struct Cyc_Absyn_Var_d_struct*)_tmp9AC;if(_tmp9AD->tag != 0)goto _LL404;
else{_tmp9AE=_tmp9AD->f1;}}_LL403: _tmp9AE->type=Cyc_Toc_typ_to_c(_tmp9AE->type);
if(_tmp9AE->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp9AE->initializer));goto _LL401;_LL404: {struct Cyc_Absyn_Fn_d_struct*
_tmp9AF=(struct Cyc_Absyn_Fn_d_struct*)_tmp9AC;if(_tmp9AF->tag != 1)goto _LL406;
else{_tmp9B0=_tmp9AF->f1;}}_LL405: _tmp9B0->ret_type=Cyc_Toc_typ_to_c(_tmp9B0->ret_type);{
struct Cyc_List_List*_tmp9C2=_tmp9B0->args;for(0;_tmp9C2 != 0;_tmp9C2=_tmp9C2->tl){(*((
struct _tuple25*)_tmp9C2->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9C2->hd)).f3);}}
goto _LL401;_LL406: {struct Cyc_Absyn_Aggr_d_struct*_tmp9B1=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp9AC;if(_tmp9B1->tag != 6)goto _LL408;else{_tmp9B2=_tmp9B1->f1;}}_LL407: Cyc_Toc_aggrdecl_to_c(
_tmp9B2);goto _LL401;_LL408: {struct Cyc_Absyn_Enum_d_struct*_tmp9B3=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9AC;if(_tmp9B3->tag != 8)goto _LL40A;else{_tmp9B4=_tmp9B3->f1;}}_LL409: if(
_tmp9B4->fields != 0){struct Cyc_List_List*_tmp9C3=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9B4->fields))->v;for(0;_tmp9C3 != 0;_tmp9C3=_tmp9C3->tl){
struct Cyc_Absyn_Enumfield*_tmp9C4=(struct Cyc_Absyn_Enumfield*)_tmp9C3->hd;if(
_tmp9C4->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9C4->tag));}}
goto _LL401;_LL40A: {struct Cyc_Absyn_Typedef_d_struct*_tmp9B5=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp9AC;if(_tmp9B5->tag != 9)goto _LL40C;else{_tmp9B6=_tmp9B5->f1;}}_LL40B:{struct
Cyc_Core_Opt*_tmpDE4;_tmp9B6->defn=((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((
_tmpDE4->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9B6->defn))->v),_tmpDE4))));}goto _LL401;_LL40C: {struct Cyc_Absyn_Let_d_struct*
_tmp9B7=(struct Cyc_Absyn_Let_d_struct*)_tmp9AC;if(_tmp9B7->tag != 2)goto _LL40E;}
_LL40D: goto _LL40F;_LL40E: {struct Cyc_Absyn_Letv_d_struct*_tmp9B8=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9AC;if(_tmp9B8->tag != 3)goto _LL410;}_LL40F: goto _LL411;_LL410: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9B9=(struct Cyc_Absyn_Datatype_d_struct*)_tmp9AC;if(_tmp9B9->tag != 7)goto
_LL412;}_LL411: goto _LL413;_LL412: {struct Cyc_Absyn_Namespace_d_struct*_tmp9BA=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9AC;if(_tmp9BA->tag != 10)goto _LL414;}
_LL413: goto _LL415;_LL414: {struct Cyc_Absyn_Using_d_struct*_tmp9BB=(struct Cyc_Absyn_Using_d_struct*)
_tmp9AC;if(_tmp9BB->tag != 11)goto _LL416;}_LL415: goto _LL417;_LL416: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9BC=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9AC;if(_tmp9BC->tag != 12)goto
_LL418;}_LL417: goto _LL419;_LL418: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9BD=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9AC;if(_tmp9BD->tag != 13)
goto _LL41A;}_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_Region_d_struct*_tmp9BE=(
struct Cyc_Absyn_Region_d_struct*)_tmp9AC;if(_tmp9BE->tag != 4)goto _LL41C;}_LL41B:
goto _LL41D;_LL41C: {struct Cyc_Absyn_Alias_d_struct*_tmp9BF=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9AC;if(_tmp9BF->tag != 5)goto _LL41E;}_LL41D:{const char*_tmpDE7;void*_tmpDE6;(
_tmpDE6=0,Cyc_Tcutil_terr(d->loc,((_tmpDE7="Cyclone declaration within C code",
_tag_dyneither(_tmpDE7,sizeof(char),34))),_tag_dyneither(_tmpDE6,sizeof(void*),0)));}
goto _LL401;_LL41E: {struct Cyc_Absyn_Porton_d_struct*_tmp9C0=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9AC;if(_tmp9C0->tag != 14)goto _LL420;}_LL41F: goto _LL421;_LL420: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9C1=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9AC;if(_tmp9C1->tag != 15)goto
_LL401;}_LL421: goto _LL401;_LL401:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){void*_tmp9C8=s->r;
struct Cyc_Absyn_Exp*_tmp9CA;struct Cyc_Absyn_Stmt*_tmp9CC;struct Cyc_Absyn_Stmt*
_tmp9CD;struct Cyc_Absyn_Exp*_tmp9CF;struct Cyc_Absyn_Exp*_tmp9D1;struct Cyc_Absyn_Stmt*
_tmp9D2;struct Cyc_Absyn_Stmt*_tmp9D3;struct _tuple8 _tmp9D5;struct Cyc_Absyn_Exp*
_tmp9D6;struct Cyc_Absyn_Stmt*_tmp9D7;struct Cyc_Absyn_Exp*_tmp9D9;struct _tuple8
_tmp9DA;struct Cyc_Absyn_Exp*_tmp9DB;struct _tuple8 _tmp9DC;struct Cyc_Absyn_Exp*
_tmp9DD;struct Cyc_Absyn_Stmt*_tmp9DE;struct Cyc_Absyn_Exp*_tmp9E0;struct Cyc_List_List*
_tmp9E1;struct Cyc_Absyn_Decl*_tmp9E3;struct Cyc_Absyn_Stmt*_tmp9E4;struct Cyc_Absyn_Stmt*
_tmp9E6;struct _tuple8 _tmp9E7;struct Cyc_Absyn_Exp*_tmp9E8;_LL423: {struct Cyc_Absyn_Exp_s_struct*
_tmp9C9=(struct Cyc_Absyn_Exp_s_struct*)_tmp9C8;if(_tmp9C9->tag != 1)goto _LL425;
else{_tmp9CA=_tmp9C9->f1;}}_LL424: Cyc_Toc_exptypes_to_c(_tmp9CA);goto _LL422;
_LL425: {struct Cyc_Absyn_Seq_s_struct*_tmp9CB=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9C8;if(_tmp9CB->tag != 2)goto _LL427;else{_tmp9CC=_tmp9CB->f1;_tmp9CD=_tmp9CB->f2;}}
_LL426: Cyc_Toc_stmttypes_to_c(_tmp9CC);Cyc_Toc_stmttypes_to_c(_tmp9CD);goto
_LL422;_LL427: {struct Cyc_Absyn_Return_s_struct*_tmp9CE=(struct Cyc_Absyn_Return_s_struct*)
_tmp9C8;if(_tmp9CE->tag != 3)goto _LL429;else{_tmp9CF=_tmp9CE->f1;}}_LL428: if(
_tmp9CF != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9CF);goto _LL422;
_LL429: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9D0=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9C8;if(_tmp9D0->tag != 4)goto _LL42B;else{_tmp9D1=_tmp9D0->f1;_tmp9D2=_tmp9D0->f2;
_tmp9D3=_tmp9D0->f3;}}_LL42A: Cyc_Toc_exptypes_to_c(_tmp9D1);Cyc_Toc_stmttypes_to_c(
_tmp9D2);Cyc_Toc_stmttypes_to_c(_tmp9D3);goto _LL422;_LL42B: {struct Cyc_Absyn_While_s_struct*
_tmp9D4=(struct Cyc_Absyn_While_s_struct*)_tmp9C8;if(_tmp9D4->tag != 5)goto _LL42D;
else{_tmp9D5=_tmp9D4->f1;_tmp9D6=_tmp9D5.f1;_tmp9D7=_tmp9D4->f2;}}_LL42C: Cyc_Toc_exptypes_to_c(
_tmp9D6);Cyc_Toc_stmttypes_to_c(_tmp9D7);goto _LL422;_LL42D: {struct Cyc_Absyn_For_s_struct*
_tmp9D8=(struct Cyc_Absyn_For_s_struct*)_tmp9C8;if(_tmp9D8->tag != 9)goto _LL42F;
else{_tmp9D9=_tmp9D8->f1;_tmp9DA=_tmp9D8->f2;_tmp9DB=_tmp9DA.f1;_tmp9DC=_tmp9D8->f3;
_tmp9DD=_tmp9DC.f1;_tmp9DE=_tmp9D8->f4;}}_LL42E: Cyc_Toc_exptypes_to_c(_tmp9D9);
Cyc_Toc_exptypes_to_c(_tmp9DB);Cyc_Toc_exptypes_to_c(_tmp9DD);Cyc_Toc_stmttypes_to_c(
_tmp9DE);goto _LL422;_LL42F: {struct Cyc_Absyn_Switch_s_struct*_tmp9DF=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9C8;if(_tmp9DF->tag != 10)goto _LL431;else{_tmp9E0=_tmp9DF->f1;_tmp9E1=_tmp9DF->f2;}}
_LL430: Cyc_Toc_exptypes_to_c(_tmp9E0);for(0;_tmp9E1 != 0;_tmp9E1=_tmp9E1->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmp9E1->hd)->body);}goto _LL422;_LL431: {struct
Cyc_Absyn_Decl_s_struct*_tmp9E2=(struct Cyc_Absyn_Decl_s_struct*)_tmp9C8;if(
_tmp9E2->tag != 12)goto _LL433;else{_tmp9E3=_tmp9E2->f1;_tmp9E4=_tmp9E2->f2;}}
_LL432: Cyc_Toc_decltypes_to_c(_tmp9E3);Cyc_Toc_stmttypes_to_c(_tmp9E4);goto
_LL422;_LL433: {struct Cyc_Absyn_Do_s_struct*_tmp9E5=(struct Cyc_Absyn_Do_s_struct*)
_tmp9C8;if(_tmp9E5->tag != 14)goto _LL435;else{_tmp9E6=_tmp9E5->f1;_tmp9E7=_tmp9E5->f2;
_tmp9E8=_tmp9E7.f1;}}_LL434: Cyc_Toc_stmttypes_to_c(_tmp9E6);Cyc_Toc_exptypes_to_c(
_tmp9E8);goto _LL422;_LL435: {struct Cyc_Absyn_Skip_s_struct*_tmp9E9=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9C8;if(_tmp9E9->tag != 0)goto _LL437;}_LL436: goto _LL438;_LL437: {struct Cyc_Absyn_Break_s_struct*
_tmp9EA=(struct Cyc_Absyn_Break_s_struct*)_tmp9C8;if(_tmp9EA->tag != 6)goto _LL439;}
_LL438: goto _LL43A;_LL439: {struct Cyc_Absyn_Continue_s_struct*_tmp9EB=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9C8;if(_tmp9EB->tag != 7)goto _LL43B;}_LL43A: goto _LL43C;_LL43B: {struct Cyc_Absyn_Goto_s_struct*
_tmp9EC=(struct Cyc_Absyn_Goto_s_struct*)_tmp9C8;if(_tmp9EC->tag != 8)goto _LL43D;}
_LL43C: goto _LL422;_LL43D: {struct Cyc_Absyn_Fallthru_s_struct*_tmp9ED=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9C8;if(_tmp9ED->tag != 11)goto _LL43F;}_LL43E: goto _LL440;_LL43F: {struct Cyc_Absyn_Label_s_struct*
_tmp9EE=(struct Cyc_Absyn_Label_s_struct*)_tmp9C8;if(_tmp9EE->tag != 13)goto _LL441;}
_LL440: goto _LL442;_LL441: {struct Cyc_Absyn_TryCatch_s_struct*_tmp9EF=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9C8;if(_tmp9EF->tag != 15)goto _LL443;}_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp9F0=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9C8;if(_tmp9F0->tag != 16)goto
_LL422;}_LL444:{const char*_tmpDEA;void*_tmpDE9;(_tmpDE9=0,Cyc_Tcutil_terr(s->loc,((
_tmpDEA="Cyclone statement in C code",_tag_dyneither(_tmpDEA,sizeof(char),28))),
_tag_dyneither(_tmpDE9,sizeof(void*),0)));}goto _LL422;_LL422:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude);static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){const char*_tmpDED;void*
_tmpDEC;(_tmpDEC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDED="decls_to_c: not at toplevel!",_tag_dyneither(_tmpDED,sizeof(char),29))),
_tag_dyneither(_tmpDEC,sizeof(void*),0)));}{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp9F5=d->r;struct Cyc_Absyn_Vardecl*_tmp9F7;struct Cyc_Absyn_Fndecl*
_tmp9F9;struct Cyc_Absyn_Aggrdecl*_tmp9FF;struct Cyc_Absyn_Datatypedecl*_tmpA01;
struct Cyc_Absyn_Enumdecl*_tmpA03;struct Cyc_Absyn_Typedefdecl*_tmpA05;struct Cyc_List_List*
_tmpA09;struct Cyc_List_List*_tmpA0B;struct Cyc_List_List*_tmpA0D;struct Cyc_List_List*
_tmpA0F;_LL446: {struct Cyc_Absyn_Var_d_struct*_tmp9F6=(struct Cyc_Absyn_Var_d_struct*)
_tmp9F5;if(_tmp9F6->tag != 0)goto _LL448;else{_tmp9F7=_tmp9F6->f1;}}_LL447: {
struct _tuple0*_tmpA10=_tmp9F7->name;if(_tmp9F7->sc == Cyc_Absyn_ExternC){struct
_tuple0*_tmpDEE;_tmpA10=((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE->f1=Cyc_Absyn_Rel_n(
0),((_tmpDEE->f2=(*_tmpA10).f2,_tmpDEE))))));}if(_tmp9F7->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9F7->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9F7->initializer));}}{
struct Cyc_Absyn_Global_b_struct _tmpDF1;struct Cyc_Absyn_Global_b_struct*_tmpDF0;
nv=Cyc_Toc_add_varmap(r,nv,_tmp9F7->name,Cyc_Absyn_varb_exp(_tmpA10,(void*)((
_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((_tmpDF0[0]=((_tmpDF1.tag=1,((_tmpDF1.f1=
_tmp9F7,_tmpDF1)))),_tmpDF0)))),0));}_tmp9F7->name=_tmpA10;_tmp9F7->sc=Cyc_Toc_scope_to_c(
_tmp9F7->sc);_tmp9F7->type=Cyc_Toc_typ_to_c_array(_tmp9F7->type);{struct Cyc_List_List*
_tmpDF2;Cyc_Toc_result_decls=((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2->hd=
d,((_tmpDF2->tl=Cyc_Toc_result_decls,_tmpDF2))))));}goto _LL445;}_LL448: {struct
Cyc_Absyn_Fn_d_struct*_tmp9F8=(struct Cyc_Absyn_Fn_d_struct*)_tmp9F5;if(_tmp9F8->tag
!= 1)goto _LL44A;else{_tmp9F9=_tmp9F8->f1;}}_LL449: {struct _tuple0*_tmpA15=
_tmp9F9->name;if(_tmp9F9->sc == Cyc_Absyn_ExternC){{struct _tuple0*_tmpDF3;_tmpA15=((
_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3->f1=Cyc_Absyn_Rel_n(0),((_tmpDF3->f2=(*
_tmpA15).f2,_tmpDF3))))));}_tmp9F9->sc=Cyc_Absyn_Public;}nv=Cyc_Toc_add_varmap(r,
nv,_tmp9F9->name,Cyc_Absyn_var_exp(_tmpA15,0));_tmp9F9->name=_tmpA15;Cyc_Toc_fndecl_to_c(
nv,_tmp9F9,cinclude);{struct Cyc_List_List*_tmpDF4;Cyc_Toc_result_decls=((_tmpDF4=
_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4->hd=d,((_tmpDF4->tl=Cyc_Toc_result_decls,
_tmpDF4))))));}goto _LL445;}_LL44A: {struct Cyc_Absyn_Let_d_struct*_tmp9FA=(struct
Cyc_Absyn_Let_d_struct*)_tmp9F5;if(_tmp9FA->tag != 2)goto _LL44C;}_LL44B: goto
_LL44D;_LL44C: {struct Cyc_Absyn_Letv_d_struct*_tmp9FB=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9F5;if(_tmp9FB->tag != 3)goto _LL44E;}_LL44D: {const char*_tmpDF7;void*_tmpDF6;(
_tmpDF6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDF7="letdecl at toplevel",_tag_dyneither(_tmpDF7,sizeof(char),20))),
_tag_dyneither(_tmpDF6,sizeof(void*),0)));}_LL44E: {struct Cyc_Absyn_Region_d_struct*
_tmp9FC=(struct Cyc_Absyn_Region_d_struct*)_tmp9F5;if(_tmp9FC->tag != 4)goto _LL450;}
_LL44F: {const char*_tmpDFA;void*_tmpDF9;(_tmpDF9=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDFA="region decl at toplevel",
_tag_dyneither(_tmpDFA,sizeof(char),24))),_tag_dyneither(_tmpDF9,sizeof(void*),0)));}
_LL450: {struct Cyc_Absyn_Alias_d_struct*_tmp9FD=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9F5;if(_tmp9FD->tag != 5)goto _LL452;}_LL451: {const char*_tmpDFD;void*_tmpDFC;(
_tmpDFC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDFD="alias decl at toplevel",_tag_dyneither(_tmpDFD,sizeof(char),23))),
_tag_dyneither(_tmpDFC,sizeof(void*),0)));}_LL452: {struct Cyc_Absyn_Aggr_d_struct*
_tmp9FE=(struct Cyc_Absyn_Aggr_d_struct*)_tmp9F5;if(_tmp9FE->tag != 6)goto _LL454;
else{_tmp9FF=_tmp9FE->f1;}}_LL453: Cyc_Toc_aggrdecl_to_c(_tmp9FF);goto _LL445;
_LL454: {struct Cyc_Absyn_Datatype_d_struct*_tmpA00=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp9F5;if(_tmpA00->tag != 7)goto _LL456;else{_tmpA01=_tmpA00->f1;}}_LL455: if(
_tmpA01->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmpA01);else{Cyc_Toc_datatypedecl_to_c(
_tmpA01);}goto _LL445;_LL456: {struct Cyc_Absyn_Enum_d_struct*_tmpA02=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9F5;if(_tmpA02->tag != 8)goto _LL458;else{_tmpA03=_tmpA02->f1;}}_LL457: Cyc_Toc_enumdecl_to_c(
nv,_tmpA03);{struct Cyc_List_List*_tmpDFE;Cyc_Toc_result_decls=((_tmpDFE=
_cycalloc(sizeof(*_tmpDFE)),((_tmpDFE->hd=d,((_tmpDFE->tl=Cyc_Toc_result_decls,
_tmpDFE))))));}goto _LL445;_LL458: {struct Cyc_Absyn_Typedef_d_struct*_tmpA04=(
struct Cyc_Absyn_Typedef_d_struct*)_tmp9F5;if(_tmpA04->tag != 9)goto _LL45A;else{
_tmpA05=_tmpA04->f1;}}_LL459: _tmpA05->name=_tmpA05->name;_tmpA05->tvs=0;if(
_tmpA05->defn != 0){struct Cyc_Core_Opt*_tmpDFF;_tmpA05->defn=((_tmpDFF=_cycalloc(
sizeof(*_tmpDFF)),((_tmpDFF->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA05->defn))->v),_tmpDFF))));}else{switch(((struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(_tmpA05->kind))->v)->kind){case Cyc_Absyn_BoxKind:
_LL466:{struct Cyc_Core_Opt*_tmpE00;_tmpA05->defn=((_tmpE00=_cycalloc(sizeof(*
_tmpE00)),((_tmpE00->v=Cyc_Absyn_void_star_typ(),_tmpE00))));}break;default:
_LL467:{struct Cyc_Core_Opt*_tmpE01;_tmpA05->defn=((_tmpE01=_cycalloc(sizeof(*
_tmpE01)),((_tmpE01->v=(void*)& Cyc_Absyn_VoidType_val,_tmpE01))));}break;}}{
struct Cyc_List_List*_tmpE02;Cyc_Toc_result_decls=((_tmpE02=_cycalloc(sizeof(*
_tmpE02)),((_tmpE02->hd=d,((_tmpE02->tl=Cyc_Toc_result_decls,_tmpE02))))));}goto
_LL445;_LL45A: {struct Cyc_Absyn_Porton_d_struct*_tmpA06=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9F5;if(_tmpA06->tag != 14)goto _LL45C;}_LL45B: goto _LL45D;_LL45C: {struct Cyc_Absyn_Portoff_d_struct*
_tmpA07=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9F5;if(_tmpA07->tag != 15)goto
_LL45E;}_LL45D: goto _LL445;_LL45E: {struct Cyc_Absyn_Namespace_d_struct*_tmpA08=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9F5;if(_tmpA08->tag != 10)goto _LL460;
else{_tmpA09=_tmpA08->f2;}}_LL45F: _tmpA0B=_tmpA09;goto _LL461;_LL460: {struct Cyc_Absyn_Using_d_struct*
_tmpA0A=(struct Cyc_Absyn_Using_d_struct*)_tmp9F5;if(_tmpA0A->tag != 11)goto _LL462;
else{_tmpA0B=_tmpA0A->f2;}}_LL461: _tmpA0D=_tmpA0B;goto _LL463;_LL462: {struct Cyc_Absyn_ExternC_d_struct*
_tmpA0C=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9F5;if(_tmpA0C->tag != 12)goto
_LL464;else{_tmpA0D=_tmpA0C->f1;}}_LL463: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA0D,top,
cinclude);goto _LL445;_LL464: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA0E=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9F5;if(_tmpA0E->tag != 13)goto _LL445;
else{_tmpA0F=_tmpA0E->f1;}}_LL465: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA0F,top,1);goto
_LL445;_LL445:;};}return nv;}static void Cyc_Toc_init();static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmpA24;struct Cyc_Core_NewRegion _tmpA23=Cyc_Core_new_dynregion();
_tmpA24=_tmpA23.dynregion;{struct _DynRegionFrame _tmpA25;struct _RegionHandle*d=
_open_dynregion(& _tmpA25,_tmpA24);{struct Cyc_Dict_Dict*_tmpE0D;struct Cyc_Dict_Dict*
_tmpE0C;struct Cyc_Set_Set**_tmpE0B;struct Cyc_Dict_Dict*_tmpE0A;struct Cyc_List_List**
_tmpE09;struct Cyc_Toc_TocState*_tmpE08;Cyc_Toc_toc_state=((_tmpE08=_cycalloc(
sizeof(*_tmpE08)),((_tmpE08->dyn=(struct _DynRegionHandle*)_tmpA24,((_tmpE08->tuple_types=(
struct Cyc_List_List**)((_tmpE09=_region_malloc(d,sizeof(*_tmpE09)),((_tmpE09[0]=
0,_tmpE09)))),((_tmpE08->aggrs_so_far=(struct Cyc_Dict_Dict*)((_tmpE0A=
_region_malloc(d,sizeof(*_tmpE0A)),((_tmpE0A[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE0A)))),((_tmpE08->datatypes_so_far=(struct Cyc_Set_Set**)((_tmpE0B=
_region_malloc(d,sizeof(*_tmpE0B)),((_tmpE0B[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE0B)))),((_tmpE08->xdatatypes_so_far=(struct Cyc_Dict_Dict*)((_tmpE0C=
_region_malloc(d,sizeof(*_tmpE0C)),((_tmpE0C[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE0C)))),((_tmpE08->qvar_tags=(struct Cyc_Dict_Dict*)((_tmpE0D=_region_malloc(
d,sizeof(*_tmpE0D)),((_tmpE0D[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _tuple13*,struct _tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),
_tmpE0D)))),((_tmpE08->temp_labels=(struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpE08))))))))))))))));}Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpE0E;Cyc_Toc_globals=_tag_dyneither(((_tmpE0E=_cycalloc(
sizeof(struct _dyneither_ptr*)* 38),((_tmpE0E[0]=& Cyc_Toc__throw_str,((_tmpE0E[1]=&
Cyc_Toc_setjmp_str,((_tmpE0E[2]=& Cyc_Toc__push_handler_str,((_tmpE0E[3]=& Cyc_Toc__pop_handler_str,((
_tmpE0E[4]=& Cyc_Toc__exn_thrown_str,((_tmpE0E[5]=& Cyc_Toc__npop_handler_str,((
_tmpE0E[6]=& Cyc_Toc__check_null_str,((_tmpE0E[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpE0E[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE0E[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpE0E[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE0E[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpE0E[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE0E[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpE0E[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE0E[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpE0E[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE0E[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpE0E[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE0E[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpE0E[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE0E[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpE0E[22]=& Cyc_Toc__cycalloc_str,((_tmpE0E[23]=& Cyc_Toc__cyccalloc_str,((
_tmpE0E[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE0E[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpE0E[26]=& Cyc_Toc__region_malloc_str,((_tmpE0E[27]=& Cyc_Toc__region_calloc_str,((
_tmpE0E[28]=& Cyc_Toc__check_times_str,((_tmpE0E[29]=& Cyc_Toc__new_region_str,((
_tmpE0E[30]=& Cyc_Toc__push_region_str,((_tmpE0E[31]=& Cyc_Toc__pop_region_str,((
_tmpE0E[32]=& Cyc_Toc__open_dynregion_str,((_tmpE0E[33]=& Cyc_Toc__push_dynregion_str,((
_tmpE0E[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE0E[35]=& Cyc_Toc__reset_region_str,((
_tmpE0E[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE0E[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,
_tmpE0E)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),38);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*
ds){Cyc_Toc_init();{struct _RegionHandle _tmpA2D=_new_region("start");struct
_RegionHandle*start=& _tmpA2D;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmpA2F;struct Cyc_Toc_TocState _tmpA2E=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA2F=_tmpA2E.dyn;Cyc_Core_free_dynregion(
_tmpA2F);};{struct Cyc_List_List*_tmpA30=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA30;};;
_pop_region(start);};}
