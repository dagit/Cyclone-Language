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
struct _DynRegionHandle*);static void*Cyc_Core_arrcast(struct _dyneither_ptr dyn,
unsigned int bd,unsigned int sz);inline static void*Cyc_Core_arrcast(struct
_dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || 
dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}struct
Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
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
Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{
int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;enum Cyc_CfFlowInfo_InitLevel
f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};
struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};
struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;
struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{int tag;struct Cyc_Absyn_Exp*
f1;int f2;void*f3;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{
struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{
int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{struct
_union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
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
Cyc_stderr,fmt,ap);{const char*_tmpA26;void*_tmpA25;(_tmpA25=0,Cyc_fprintf(Cyc_stderr,((
_tmpA26="\n",_tag_dyneither(_tmpA26,sizeof(char),2))),_tag_dyneither(_tmpA25,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Unimplemented_struct
_tmpA29;struct Cyc_Toc_Toc_Unimplemented_struct*_tmpA28;(int)_throw((void*)((
_tmpA28=_cycalloc_atomic(sizeof(*_tmpA28)),((_tmpA28[0]=((_tmpA29.tag=Cyc_Toc_Toc_Unimplemented,
_tmpA29)),_tmpA28)))));};}static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA2C;void*
_tmpA2B;(_tmpA2B=0,Cyc_fprintf(Cyc_stderr,((_tmpA2C="\n",_tag_dyneither(_tmpA2C,
sizeof(char),2))),_tag_dyneither(_tmpA2B,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct _tmpA2F;struct Cyc_Toc_Toc_Impossible_struct*
_tmpA2E;(int)_throw((void*)((_tmpA2E=_cycalloc_atomic(sizeof(*_tmpA2E)),((
_tmpA2E[0]=((_tmpA2F.tag=Cyc_Toc_Toc_Impossible,_tmpA2F)),_tmpA2E)))));};}char
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
_tmp1F + 7};static struct _tuple0 Cyc_Toc__throw_pr={{.Loc_n={3,0}},& Cyc_Toc__throw_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={
0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp21[7]="setjmp";static struct
_dyneither_ptr Cyc_Toc_setjmp_str={_tmp21,_tmp21,_tmp21 + 7};static struct _tuple0
Cyc_Toc_setjmp_pr={{.Loc_n={3,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)&
Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;
static char _tmp23[14]="_push_handler";static struct _dyneither_ptr Cyc_Toc__push_handler_str={
_tmp23,_tmp23,_tmp23 + 14};static struct _tuple0 Cyc_Toc__push_handler_pr={{.Loc_n={
3,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1,& Cyc_Toc__push_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp25[13]="_pop_handler";static struct
_dyneither_ptr Cyc_Toc__pop_handler_str={_tmp25,_tmp25,_tmp25 + 13};static struct
_tuple0 Cyc_Toc__pop_handler_pr={{.Loc_n={3,0}},& Cyc_Toc__pop_handler_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char
_tmp27[12]="_exn_thrown";static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={
_tmp27,_tmp27,_tmp27 + 12};static struct _tuple0 Cyc_Toc__exn_thrown_pr={{.Loc_n={3,
0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp29[14]="_npop_handler";static struct
_dyneither_ptr Cyc_Toc__npop_handler_str={_tmp29,_tmp29,_tmp29 + 14};static struct
_tuple0 Cyc_Toc__npop_handler_pr={{.Loc_n={3,0}},& Cyc_Toc__npop_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char
_tmp2B[12]="_check_null";static struct _dyneither_ptr Cyc_Toc__check_null_str={
_tmp2B,_tmp2B,_tmp2B + 12};static struct _tuple0 Cyc_Toc__check_null_pr={{.Loc_n={3,
0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={
1,& Cyc_Toc__check_null_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp2D[28]="_check_known_subscript_null";static
struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2D,_tmp2D,
_tmp2D + 28};static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={
3,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp2F[31]="_check_known_subscript_notnull";static struct _dyneither_ptr
Cyc_Toc__check_known_subscript_notnull_str={_tmp2F,_tmp2F,_tmp2F + 31};static
struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={3,0}},& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)&
Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static char _tmp31[27]="_check_dyneither_subscript";static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp31,_tmp31,_tmp31 + 27};static struct _tuple0 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={
3,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp33[15]="_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={
_tmp33,_tmp33,_tmp33 + 15};static struct _tuple0 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=&
Cyc_Toc__dyneither_ptr_ev;static char _tmp35[15]="_tag_dyneither";static struct
_dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp35,_tmp35,_tmp35 + 15};static struct
_tuple0 Cyc_Toc__tag_dyneither_pr={{.Loc_n={3,0}},& Cyc_Toc__tag_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static
char _tmp37[20]="_init_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp37,_tmp37,_tmp37 + 20};static struct _tuple0 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={
3,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={
1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=&
Cyc_Toc__init_dyneither_ptr_ev;static char _tmp39[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp39,_tmp39,_tmp39
+ 21};static struct _tuple0 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={3,0}},& Cyc_Toc__untag_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={
0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp3B[20]="_get_dyneither_size";static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={
_tmp3B,_tmp3B,_tmp3B + 20};static struct _tuple0 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={
1,& Cyc_Toc__get_dyneither_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=&
Cyc_Toc__get_dyneither_size_ev;static char _tmp3D[19]="_get_zero_arr_size";static
struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3D,_tmp3D,_tmp3D + 19};
static struct _tuple0 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={3,0}},& Cyc_Toc__get_zero_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp3F[24]="_get_zero_arr_size_char";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={
_tmp3F,_tmp3F,_tmp3F + 24};static struct _tuple0 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;
static char _tmp41[25]="_get_zero_arr_size_short";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={
_tmp41,_tmp41,_tmp41 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;
static char _tmp43[23]="_get_zero_arr_size_int";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={
_tmp43,_tmp43,_tmp43 + 23};static struct _tuple0 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_int_re={
1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=&
Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp45[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp45,_tmp45,
_tmp45 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={3,0}},&
Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_float_re={
1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=&
Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp47[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp47,_tmp47,
_tmp47 + 26};static struct _tuple0 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={3,
0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;
static char _tmp49[30]="_get_zero_arr_size_longdouble";static struct _dyneither_ptr
Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp49,_tmp49,_tmp49 + 30};static
struct _tuple0 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={3,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,&
Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=&
Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp4B[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp4B,_tmp4B,
_tmp4B + 28};static struct _tuple0 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;
static char _tmp4D[20]="_dyneither_ptr_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp4D,_tmp4D,_tmp4D + 20};static struct _tuple0 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={
1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=&
Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp4F[15]="_zero_arr_plus";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp4F,_tmp4F,_tmp4F + 15};static
struct _tuple0 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static
char _tmp51[20]="_zero_arr_plus_char";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={
_tmp51,_tmp51,_tmp51 + 20};static struct _tuple0 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_char_re={
1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=&
Cyc_Toc__zero_arr_plus_char_ev;static char _tmp53[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp53,_tmp53,_tmp53
+ 21};static struct _tuple0 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;
static char _tmp55[19]="_zero_arr_plus_int";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={
_tmp55,_tmp55,_tmp55 + 19};static struct _tuple0 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_int_re={
1,& Cyc_Toc__zero_arr_plus_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=&
Cyc_Toc__zero_arr_plus_int_ev;static char _tmp57[21]="_zero_arr_plus_float";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp57,_tmp57,_tmp57 + 21};
static struct _tuple0 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;
static char _tmp59[22]="_zero_arr_plus_double";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={
_tmp59,_tmp59,_tmp59 + 22};static struct _tuple0 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_double_re={
1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=&
Cyc_Toc__zero_arr_plus_double_ev;static char _tmp5B[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp5B,_tmp5B,
_tmp5B + 26};static struct _tuple0 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;
static char _tmp5D[24]="_zero_arr_plus_voidstar";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={
_tmp5D,_tmp5D,_tmp5D + 24};static struct _tuple0 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;
static char _tmp5F[28]="_dyneither_ptr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={
_tmp5F,_tmp5F,_tmp5F + 28};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp61[23]="_zero_arr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp61,_tmp61,_tmp61 + 23};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={
1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=&
Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp63[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp63,_tmp63,
_tmp63 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;
static char _tmp65[29]="_zero_arr_inplace_plus_short";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp65,_tmp65,_tmp65 + 29};static struct
_tuple0 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,&
Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=&
Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp67[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp67,_tmp67,
_tmp67 + 27};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;
static char _tmp69[29]="_zero_arr_inplace_plus_float";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp69,_tmp69,_tmp69 + 29};static struct
_tuple0 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,&
Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=&
Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp6B[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp6B,
_tmp6B,_tmp6B + 30};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;
static char _tmp6D[34]="_zero_arr_inplace_plus_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp6D,_tmp6D,
_tmp6D + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;
static char _tmp6F[32]="_zero_arr_inplace_plus_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp6F,_tmp6F,_tmp6F + 
32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={3,0}},&
Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;
static char _tmp71[33]="_dyneither_ptr_inplace_plus_post";static struct
_dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp71,_tmp71,_tmp71
+ 33};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={3,
0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;
static char _tmp73[28]="_zero_arr_inplace_plus_post";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={
_tmp73,_tmp73,_tmp73 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp75[33]="_zero_arr_inplace_plus_post_char";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp75,_tmp75,_tmp75
+ 33};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;
static char _tmp77[34]="_zero_arr_inplace_plus_post_short";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp77,_tmp77,
_tmp77 + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;
static char _tmp79[32]="_zero_arr_inplace_plus_post_int";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp79,_tmp79,_tmp79 + 
32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={3,0}},&
Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;
static char _tmp7B[34]="_zero_arr_inplace_plus_post_float";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp7B,_tmp7B,
_tmp7B + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;
static char _tmp7D[35]="_zero_arr_inplace_plus_post_double";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp7D,_tmp7D,
_tmp7D + 35};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;
static char _tmp7F[39]="_zero_arr_inplace_plus_post_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp7F,_tmp7F,
_tmp7F + 39};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;
static char _tmp81[37]="_zero_arr_inplace_plus_post_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp81,_tmp81,
_tmp81 + 37};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;
static char _tmp83[10]="_cycalloc";static struct _dyneither_ptr Cyc_Toc__cycalloc_str={
_tmp83,_tmp83,_tmp83 + 10};static struct _tuple0 Cyc_Toc__cycalloc_pr={{.Loc_n={3,0}},&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct
Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char
_tmp85[11]="_cyccalloc";static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp85,
_tmp85,_tmp85 + 11};static struct _tuple0 Cyc_Toc__cyccalloc_pr={{.Loc_n={3,0}},& Cyc_Toc__cyccalloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct
Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp87[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp87,_tmp87,_tmp87 + 17};
static struct _tuple0 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={3,0}},& Cyc_Toc__cycalloc_atomic_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmp89[18]="_cyccalloc_atomic";static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmp89,_tmp89,_tmp89 + 18};static struct _tuple0 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={
3,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=&
Cyc_Toc__cyccalloc_atomic_ev;static char _tmp8B[15]="_region_malloc";static struct
_dyneither_ptr Cyc_Toc__region_malloc_str={_tmp8B,_tmp8B,_tmp8B + 15};static struct
_tuple0 Cyc_Toc__region_malloc_pr={{.Loc_n={3,0}},& Cyc_Toc__region_malloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={
0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static
char _tmp8D[15]="_region_calloc";static struct _dyneither_ptr Cyc_Toc__region_calloc_str={
_tmp8D,_tmp8D,_tmp8D + 15};static struct _tuple0 Cyc_Toc__region_calloc_pr={{.Loc_n={
3,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=&
Cyc_Toc__region_calloc_ev;static char _tmp8F[13]="_check_times";static struct
_dyneither_ptr Cyc_Toc__check_times_str={_tmp8F,_tmp8F,_tmp8F + 13};static struct
_tuple0 Cyc_Toc__check_times_pr={{.Loc_n={3,0}},& Cyc_Toc__check_times_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={
0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char
_tmp91[12]="_new_region";static struct _dyneither_ptr Cyc_Toc__new_region_str={
_tmp91,_tmp91,_tmp91 + 12};static struct _tuple0 Cyc_Toc__new_region_pr={{.Loc_n={3,
0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=&
Cyc_Toc__new_region_ev;static char _tmp93[13]="_push_region";static struct
_dyneither_ptr Cyc_Toc__push_region_str={_tmp93,_tmp93,_tmp93 + 13};static struct
_tuple0 Cyc_Toc__push_region_pr={{.Loc_n={3,0}},& Cyc_Toc__push_region_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={
0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char
_tmp95[12]="_pop_region";static struct _dyneither_ptr Cyc_Toc__pop_region_str={
_tmp95,_tmp95,_tmp95 + 12};static struct _tuple0 Cyc_Toc__pop_region_pr={{.Loc_n={3,
0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=&
Cyc_Toc__pop_region_ev;static char _tmp97[16]="_open_dynregion";static struct
_dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp97,_tmp97,_tmp97 + 16};static
struct _tuple0 Cyc_Toc__open_dynregion_pr={{.Loc_n={3,0}},& Cyc_Toc__open_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={
0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static
char _tmp99[16]="_push_dynregion";static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={
_tmp99,_tmp99,_tmp99 + 16};static struct _tuple0 Cyc_Toc__push_dynregion_pr={{.Loc_n={
3,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={
1,& Cyc_Toc__push_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=&
Cyc_Toc__push_dynregion_ev;static char _tmp9B[15]="_pop_dynregion";static struct
_dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp9B,_tmp9B,_tmp9B + 15};static struct
_tuple0 Cyc_Toc__pop_dynregion_pr={{.Loc_n={3,0}},& Cyc_Toc__pop_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={
0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static
char _tmp9D[14]="_reset_region";static struct _dyneither_ptr Cyc_Toc__reset_region_str={
_tmp9D,_tmp9D,_tmp9D + 14};static struct _tuple0 Cyc_Toc__reset_region_pr={{.Loc_n={
3,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={
1,& Cyc_Toc__reset_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static char _tmp9F[19]="_throw_arraybounds";static struct
_dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp9F,_tmp9F,_tmp9F + 19};static
struct _tuple0 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={3,0}},& Cyc_Toc__throw_arraybounds_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={
0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmpA1[29]="_dyneither_ptr_decrease_size";static struct _dyneither_ptr
Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpA1,_tmpA1,_tmpA1 + 29};static struct
_tuple0 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={3,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA3[11]="_swap_word";static
struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpA3,_tmpA3,_tmpA3 + 11};static
struct _tuple0 Cyc_Toc__swap_word_pr={{.Loc_n={3,0}},& Cyc_Toc__swap_word_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct
Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA5[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA5,_tmpA5,_tmpA5 + 16};
static struct _tuple0 Cyc_Toc__swap_dyneither_pr={{.Loc_n={3,0}},& Cyc_Toc__swap_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={
0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static
struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={12,{{.UnknownAggr={1,{
Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};static void*Cyc_Toc_dyneither_ptr_typ=(
void*)& Cyc_Toc_dyneither_ptr_typ_v;static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,
0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl();static struct Cyc_Absyn_Stmt*
Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(
skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpA30;skip_stmt_opt=((_tmpA30=
_cycalloc(sizeof(*_tmpA30)),((_tmpA30[0]=Cyc_Absyn_skip_stmt(0),_tmpA30))));}
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e);static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*
e){struct Cyc_Absyn_Cast_e_struct _tmpA33;struct Cyc_Absyn_Cast_e_struct*_tmpA32;
return(void*)((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32[0]=((_tmpA33.tag=15,((
_tmpA33.f1=(void*)t,((_tmpA33.f2=e,((_tmpA33.f3=0,((_tmpA33.f4=Cyc_Absyn_No_coercion,
_tmpA33)))))))))),_tmpA32))));}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*
e);static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA36;struct Cyc_Absyn_Deref_e_struct*_tmpA35;return(void*)((_tmpA35=_cycalloc(
sizeof(*_tmpA35)),((_tmpA35[0]=((_tmpA36.tag=21,((_tmpA36.f1=e,_tmpA36)))),
_tmpA35))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA39;struct Cyc_Absyn_Subscript_e_struct*
_tmpA38;return(void*)((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38[0]=((
_tmpA39.tag=24,((_tmpA39.f1=e1,((_tmpA39.f2=e2,_tmpA39)))))),_tmpA38))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct _tmpA3C;struct Cyc_Absyn_StmtExp_e_struct*
_tmpA3B;return(void*)((_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B[0]=((
_tmpA3C.tag=37,((_tmpA3C.f1=s,_tmpA3C)))),_tmpA3B))));}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t);static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA3F;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA3E;return(void*)((_tmpA3E=
_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E[0]=((_tmpA3F.tag=18,((_tmpA3F.f1=(void*)t,
_tmpA3F)))),_tmpA3E))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es);static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA42;struct Cyc_Absyn_FnCall_e_struct*
_tmpA41;return(void*)((_tmpA41=_cycalloc(sizeof(*_tmpA41)),((_tmpA41[0]=((
_tmpA42.tag=11,((_tmpA42.f1=e,((_tmpA42.f2=es,((_tmpA42.f3=0,_tmpA42)))))))),
_tmpA41))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_exp_stmt_r(
struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct _tmpA45;struct Cyc_Absyn_Exp_s_struct*
_tmpA44;return(void*)((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44[0]=((
_tmpA45.tag=1,((_tmpA45.f1=e,_tmpA45)))),_tmpA44))));}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2);static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Seq_s_struct
_tmpA48;struct Cyc_Absyn_Seq_s_struct*_tmpA47;return(void*)((_tmpA47=_cycalloc(
sizeof(*_tmpA47)),((_tmpA47[0]=((_tmpA48.tag=2,((_tmpA48.f1=s1,((_tmpA48.f2=s2,
_tmpA48)))))),_tmpA47))));}static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3);static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA4B;struct Cyc_Absyn_Conditional_e_struct*_tmpA4A;return(void*)((_tmpA4A=
_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A[0]=((_tmpA4B.tag=6,((_tmpA4B.f1=e1,((
_tmpA4B.f2=e2,((_tmpA4B.f3=e3,_tmpA4B)))))))),_tmpA4A))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpA4E;struct Cyc_Absyn_AggrMember_e_struct*_tmpA4D;return(void*)((_tmpA4D=
_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D[0]=((_tmpA4E.tag=22,((_tmpA4E.f1=e,((
_tmpA4E.f2=n,((_tmpA4E.f3=0,((_tmpA4E.f4=0,_tmpA4E)))))))))),_tmpA4D))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpA51;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA50;return(void*)((_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50[0]=((
_tmpA51.tag=23,((_tmpA51.f1=e,((_tmpA51.f2=n,((_tmpA51.f3=0,((_tmpA51.f4=0,
_tmpA51)))))))))),_tmpA50))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds);static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA54;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA53;return(void*)((_tmpA53=_cycalloc(
sizeof(*_tmpA53)),((_tmpA53[0]=((_tmpA54.tag=36,((_tmpA54.f1=tdopt,((_tmpA54.f2=
ds,_tmpA54)))))),_tmpA53))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA57;
struct Cyc_Absyn_Goto_s_struct*_tmpA56;return(void*)((_tmpA56=_cycalloc(sizeof(*
_tmpA56)),((_tmpA56[0]=((_tmpA57.tag=8,((_tmpA57.f1=v,((_tmpA57.f2=s,_tmpA57)))))),
_tmpA56))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
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
_tmpA5D;const char*_tmpA5C;struct Cyc_Core_Impossible_struct*_tmpA5B;(int)_throw((
void*)((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=((_tmpA5D.tag=Cyc_Core_Impossible,((
_tmpA5D.f1=((_tmpA5C="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA5C,sizeof(char),44))),_tmpA5D)))),_tmpA5B)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC7=(struct Cyc_Absyn_FloatType_struct*)
_tmpC4;if(_tmpC7->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpC8=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC4;if(_tmpC8->tag != 8)goto _LL7;else{_tmpC9=_tmpC8->f1;}}_LL6: switch(_tmpC9){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCA=(struct Cyc_Absyn_PointerType_struct*)
_tmpC4;if(_tmpCA->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpA6A;const char*_tmpA69;void*_tmpA68[1];
struct Cyc_String_pa_struct _tmpA67;struct Cyc_Core_Impossible_struct*_tmpA66;(int)
_throw((void*)((_tmpA66=_cycalloc(sizeof(*_tmpA66)),((_tmpA66[0]=((_tmpA6A.tag=
Cyc_Core_Impossible,((_tmpA6A.f1=(struct _dyneither_ptr)((_tmpA67.tag=0,((_tmpA67.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA68[
0]=& _tmpA67,Cyc_aprintf(((_tmpA69="impossible expression type %s (not int, float, double, or pointer)",
_tag_dyneither(_tmpA69,sizeof(char),67))),_tag_dyneither(_tmpA68,sizeof(void*),1)))))))),
_tmpA6A)))),_tmpA66)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
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
_tmpA70;const char*_tmpA6F;struct Cyc_Core_Impossible_struct*_tmpA6E;(int)_throw((
void*)((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E[0]=((_tmpA70.tag=Cyc_Core_Impossible,((
_tmpA70.f1=((_tmpA6F="impossible type (not pointer)",_tag_dyneither(_tmpA6F,
sizeof(char),30))),_tmpA70)))),_tmpA6E)))));}_LL13:;}struct _tuple15{struct Cyc_List_List*
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
_LL34:;_LL35: {const char*_tmpA73;void*_tmpA72;(_tmpA72=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA73="is_nullable",
_tag_dyneither(_tmpA73,sizeof(char),12))),_tag_dyneither(_tmpA72,sizeof(void*),0)));}
_LL31:;}static char _tmp10D[1]="";static char _tmp10E[8]="*bogus*";static struct
_tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct _dyneither_ptr tag);
static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp102;struct Cyc_Dict_Dict*_tmp103;
struct Cyc_Toc_TocState _tmp101=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp102=_tmp101.dyn;_tmp103=_tmp101.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp10E,_tmp10E,_tmp10E + 8};static struct _tuple0 bogus_qvar={{.Loc_n={
3,0}},& bogus_string};static struct _tuple13 pair={& bogus_qvar,{_tmp10D,_tmp10D,
_tmp10D + 1}};{struct _tuple13 _tmpA74;pair=((_tmpA74.f1=x,((_tmpA74.f2=tag,_tmpA74))));}{
struct _DynRegionFrame _tmp105;struct _RegionHandle*d=_open_dynregion(& _tmp105,
_tmp102);{struct _tuple0**_tmp106=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,
struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp103,(struct _tuple13*)& pair);if(
_tmp106 != 0){struct _tuple0*_tmp107=*_tmp106;_npop_handler(0);return _tmp107;}{
struct _tuple13*_tmpA75;struct _tuple13*_tmp108=(_tmpA75=_cycalloc(sizeof(*_tmpA75)),((
_tmpA75->f1=x,((_tmpA75->f2=tag,_tmpA75)))));struct _dyneither_ptr*_tmpA78;struct
_tuple0*_tmpA77;struct _tuple0*res=(_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77->f1=(*
x).f1,((_tmpA77->f2=((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpA78)))),_tmpA77)))));*_tmp103=((struct Cyc_Dict_Dict(*)(
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
_npop_handler(0);return _tmp119;}};}}{struct Cyc_Int_pa_struct _tmpA80;void*_tmpA7F[
1];const char*_tmpA7E;struct _dyneither_ptr*_tmpA7D;struct _dyneither_ptr*xname=(
_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D[0]=(struct _dyneither_ptr)((_tmpA80.tag=
1,((_tmpA80.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA7F[0]=& _tmpA80,
Cyc_aprintf(((_tmpA7E="_tuple%d",_tag_dyneither(_tmpA7E,sizeof(char),9))),
_tag_dyneither(_tmpA7F,sizeof(void*),1)))))))),_tmpA7D)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp11A=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpA83;struct Cyc_List_List*
_tmpA82;_tmp11A=((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82->hd=((_tmpA83=
_cycalloc(sizeof(*_tmpA83)),((_tmpA83->name=Cyc_Absyn_fieldname(i),((_tmpA83->tq=
Cyc_Toc_mt_tq,((_tmpA83->type=(void*)ts2->hd,((_tmpA83->width=0,((_tmpA83->attributes=
0,_tmpA83)))))))))))),((_tmpA82->tl=_tmp11A,_tmpA82))))));}}_tmp11A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11A);{struct Cyc_Absyn_AggrdeclImpl*
_tmpA88;struct _tuple0*_tmpA87;struct Cyc_Absyn_Aggrdecl*_tmpA86;struct Cyc_Absyn_Aggrdecl*
_tmp11D=(_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86->kind=Cyc_Absyn_StructA,((
_tmpA86->sc=Cyc_Absyn_Public,((_tmpA86->name=((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((
_tmpA87->f1=Cyc_Absyn_Rel_n(0),((_tmpA87->f2=xname,_tmpA87)))))),((_tmpA86->tvs=
0,((_tmpA86->impl=((_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88->exist_vars=0,((
_tmpA88->rgn_po=0,((_tmpA88->fields=_tmp11A,((_tmpA88->tagged=0,_tmpA88)))))))))),((
_tmpA86->attributes=0,_tmpA86)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpA8E;struct Cyc_Absyn_Aggr_d_struct _tmpA8D;struct Cyc_List_List*_tmpA8C;Cyc_Toc_result_decls=((
_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C->hd=Cyc_Absyn_new_decl((void*)((
_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E[0]=((_tmpA8D.tag=6,((_tmpA8D.f1=
_tmp11D,_tmpA8D)))),_tmpA8E)))),0),((_tmpA8C->tl=Cyc_Toc_result_decls,_tmpA8C))))));}{
struct _tuple16*_tmpA91;struct Cyc_List_List*_tmpA90;*_tmp111=((_tmpA90=
_region_malloc(d,sizeof(*_tmpA90)),((_tmpA90->hd=((_tmpA91=_region_malloc(d,
sizeof(*_tmpA91)),((_tmpA91->f1=x,((_tmpA91->f2=ts,_tmpA91)))))),((_tmpA90->tl=*
_tmp111,_tmpA90))))));}{void*_tmp123=x;_npop_handler(0);return _tmp123;};};};;
_pop_dynregion(d);};}struct _tuple0*Cyc_Toc_temp_var();struct _tuple0*Cyc_Toc_temp_var(){
int _tmp12B=Cyc_Toc_temp_var_counter ++;struct _dyneither_ptr*_tmpA9E;const char*
_tmpA9D;void*_tmpA9C[1];struct Cyc_Int_pa_struct _tmpA9B;struct _tuple0*_tmpA9A;
struct _tuple0*res=(_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A->f1=Cyc_Absyn_Loc_n,((
_tmpA9A->f2=((_tmpA9E=_cycalloc(sizeof(*_tmpA9E)),((_tmpA9E[0]=(struct
_dyneither_ptr)((_tmpA9B.tag=1,((_tmpA9B.f1=(unsigned int)_tmp12B,((_tmpA9C[0]=&
_tmpA9B,Cyc_aprintf(((_tmpA9D="_tmp%X",_tag_dyneither(_tmpA9D,sizeof(char),7))),
_tag_dyneither(_tmpA9C,sizeof(void*),1)))))))),_tmpA9E)))),_tmpA9A)))));return
res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label();static struct _dyneither_ptr*
Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp132;struct Cyc_Xarray_Xarray*
_tmp133;struct Cyc_Toc_TocState _tmp131=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp132=_tmp131.dyn;_tmp133=_tmp131.temp_labels;{struct _DynRegionFrame _tmp134;
struct _RegionHandle*d=_open_dynregion(& _tmp134,_tmp132);{int _tmp135=Cyc_Toc_fresh_label_counter
++;if(_tmp135 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp133)){
struct _dyneither_ptr*_tmp136=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp133,_tmp135);_npop_handler(0);return _tmp136;}{struct Cyc_Int_pa_struct
_tmpAA6;void*_tmpAA5[1];const char*_tmpAA4;struct _dyneither_ptr*_tmpAA3;struct
_dyneither_ptr*res=(_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3[0]=(struct
_dyneither_ptr)((_tmpAA6.tag=1,((_tmpAA6.f1=(unsigned int)_tmp135,((_tmpAA5[0]=&
_tmpAA6,Cyc_aprintf(((_tmpAA4="_LL%X",_tag_dyneither(_tmpAA4,sizeof(char),6))),
_tag_dyneither(_tmpAA5,sizeof(void*),1)))))))),_tmpAA3)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp133,res)!= _tmp135){const char*
_tmpAA9;void*_tmpAA8;(_tmpAA8=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA9="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpAA9,sizeof(char),43))),_tag_dyneither(_tmpAA8,sizeof(void*),0)));}{
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
_tmp140=_tmp13F.f1;_tmp141=_tmp13F.f2;_tmp142=_tmp13F.f3;{struct _tuple7*_tmpAAA;
return(_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA->f1=_tmp140,((_tmpAAA->f2=
_tmp141,((_tmpAAA->f3=Cyc_Toc_typ_to_c(_tmp142),_tmpAAA)))))));};}static struct
_tuple9*Cyc_Toc_typ_to_c_f(struct _tuple9*x);static struct _tuple9*Cyc_Toc_typ_to_c_f(
struct _tuple9*x){struct Cyc_Absyn_Tqual _tmp145;void*_tmp146;struct _tuple9 _tmp144=*
x;_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;{struct _tuple9*_tmpAAB;return(_tmpAAB=
_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->f1=_tmp145,((_tmpAAB->f2=Cyc_Toc_typ_to_c(
_tmp146),_tmpAAB)))));};}static void*Cyc_Toc_typ_to_c_array(void*t);static void*
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
_tmpAAC;return(_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC->name=f->name,((
_tmpAAC->tq=Cyc_Toc_mt_tq,((_tmpAAC->type=Cyc_Toc_typ_to_c(f->type),((_tmpAAC->width=
f->width,((_tmpAAC->attributes=f->attributes,_tmpAAC)))))))))));}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs);static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}static void*Cyc_Toc_char_star_typ();static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpAAD;cs=((_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((
_tmpAAD[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAAD))));}return*cs;}static void*Cyc_Toc_rgn_typ();
static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0){void**_tmpAAE;r=((_tmpAAE=
_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq),_tmpAAE))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ();static
void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){void**_tmpAAF;r=((_tmpAAF=
_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),
Cyc_Toc_mt_tq),_tmpAAF))));}return*r;}static void*Cyc_Toc_typ_to_c(void*t);static
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
const char*_tmpAB0;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp168->name,((
_tmpAB0="_struct",_tag_dyneither(_tmpAB0,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp169=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp158;if(_tmp169->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpAB3;void*_tmpAB2;(_tmpAB2=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB3="unresolved DatatypeFieldType",
_tag_dyneither(_tmpAB3,sizeof(char),29))),_tag_dyneither(_tmpAB2,sizeof(void*),0)));}
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
_LL85: continue;_LL86:;_LL87:{struct Cyc_List_List*_tmpAB4;_tmp1A1=((_tmpAB4=
_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4->hd=(void*)_tmp17F->hd,((_tmpAB4->tl=
_tmp1A1,_tmpAB4))))));}goto _LL7B;_LL7B:;}{struct Cyc_List_List*_tmp1A9=((struct
Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp17C);if(_tmp17E != 0){void*_tmp1AA=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp17E->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple7*_tmpAB5;struct _tuple7*_tmp1AB=(_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((
_tmpAB5->f1=_tmp17E->name,((_tmpAB5->f2=_tmp17E->tq,((_tmpAB5->f3=_tmp1AA,
_tmpAB5)))))));struct Cyc_List_List*_tmpAB6;_tmp1A9=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1A9,((
_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6->hd=_tmp1AB,((_tmpAB6->tl=0,
_tmpAB6)))))));}{struct Cyc_Absyn_FnType_struct _tmpABC;struct Cyc_Absyn_FnInfo
_tmpABB;struct Cyc_Absyn_FnType_struct*_tmpABA;return(void*)((_tmpABA=_cycalloc(
sizeof(*_tmpABA)),((_tmpABA[0]=((_tmpABC.tag=10,((_tmpABC.f1=((_tmpABB.tvars=0,((
_tmpABB.effect=0,((_tmpABB.ret_typ=Cyc_Toc_typ_to_c(_tmp17B),((_tmpABB.args=
_tmp1A9,((_tmpABB.c_varargs=_tmp17D,((_tmpABB.cyc_varargs=0,((_tmpABB.rgn_po=0,((
_tmpABB.attributes=_tmp1A1,_tmpABB)))))))))))))))),_tmpABC)))),_tmpABA))));};};}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp180=(struct Cyc_Absyn_TupleType_struct*)
_tmp158;if(_tmp180->tag != 11)goto _LL5A;else{_tmp181=_tmp180->f1;}}_LL59: _tmp181=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp181);return Cyc_Toc_add_tuple_type(_tmp181);
_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp182=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp158;if(_tmp182->tag != 13)goto _LL5C;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}
_LL5B: {struct Cyc_Absyn_AnonAggrType_struct _tmpABF;struct Cyc_Absyn_AnonAggrType_struct*
_tmpABE;return(void*)((_tmpABE=_cycalloc(sizeof(*_tmpABE)),((_tmpABE[0]=((
_tmpABF.tag=13,((_tmpABF.f1=_tmp183,((_tmpABF.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp184),_tmpABF)))))),_tmpABE))));}_LL5C: {
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
if(_tmp18F != 0){struct Cyc_Absyn_TypedefType_struct _tmpAC2;struct Cyc_Absyn_TypedefType_struct*
_tmpAC1;return(void*)((_tmpAC1=_cycalloc(sizeof(*_tmpAC1)),((_tmpAC1[0]=((
_tmpAC2.tag=18,((_tmpAC2.f1=_tmp18E,((_tmpAC2.f2=0,((_tmpAC2.f3=_tmp190,((
_tmpAC2.f4=0,_tmpAC2)))))))))),_tmpAC1))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpAC8;void**_tmpAC7;struct Cyc_Absyn_TypedefType_struct*_tmpAC6;return(void*)((
_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6[0]=((_tmpAC8.tag=18,((_tmpAC8.f1=
_tmp18E,((_tmpAC8.f2=0,((_tmpAC8.f3=_tmp190,((_tmpAC8.f4=((_tmpAC7=_cycalloc(
sizeof(*_tmpAC7)),((_tmpAC7[0]=Cyc_Toc_typ_to_c_array(*_tmp191),_tmpAC7)))),
_tmpAC8)))))))))),_tmpAC6))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp192=(
struct Cyc_Absyn_TagType_struct*)_tmp158;if(_tmp192->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp193=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp158;if(_tmp193->tag != 16)goto _LL68;
else{_tmp194=(void*)_tmp193->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp195=(struct Cyc_Absyn_DynRgnType_struct*)_tmp158;
if(_tmp195->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp196=(struct Cyc_Absyn_HeapRgn_struct*)_tmp158;if(
_tmp196->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpACB;void*_tmpACA;(_tmpACA=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpACB="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpACB,sizeof(char),55))),_tag_dyneither(_tmpACA,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp197=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp158;if(_tmp197->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpACE;void*_tmpACD;(
_tmpACD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpACE="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpACE,sizeof(char),57))),_tag_dyneither(_tmpACD,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp198=(struct Cyc_Absyn_AccessEff_struct*)
_tmp158;if(_tmp198->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp199=(struct Cyc_Absyn_JoinEff_struct*)_tmp158;if(_tmp199->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp19A=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp158;if(_tmp19A->tag != 25)goto _LL74;}_LL73: {const char*_tmpAD1;void*_tmpAD0;(
_tmpAD0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAD1="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpAD1,sizeof(char),49))),_tag_dyneither(_tmpAD0,sizeof(void*),0)));}_LL74: {
struct Cyc_Absyn_ValueofType_struct*_tmp19B=(struct Cyc_Absyn_ValueofType_struct*)
_tmp158;if(_tmp19B->tag != 19)goto _LL3D;}_LL75: {const char*_tmpAD4;void*_tmpAD3;(
_tmpAD3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAD4="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpAD4,sizeof(char),51))),_tag_dyneither(_tmpAD3,sizeof(void*),0)));}_LL3D:;}
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
_LLB7;}_LLB6: return 0;_LLB7:;_LLB8: {const char*_tmpAD8;void*_tmpAD7[1];struct Cyc_String_pa_struct
_tmpAD6;(_tmpAD6.tag=0,((_tmpAD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAD7[0]=& _tmpAD6,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD8="atomic_typ:  bad type %s",
_tag_dyneither(_tmpAD8,sizeof(char),25))),_tag_dyneither(_tmpAD7,sizeof(void*),1)))))));}
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
if(_tmp1FB->impl == 0){const char*_tmpADB;void*_tmpADA;(_tmpADA=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADB="is_poly_field: type missing fields",
_tag_dyneither(_tmpADB,sizeof(char),35))),_tag_dyneither(_tmpADA,sizeof(void*),0)));}
_tmp1FA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FB->impl))->fields;goto
_LLD1;}_LLD0: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1F9=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F5;if(_tmp1F9->tag != 13)goto _LLD2;else{_tmp1FA=_tmp1F9->f2;}}_LLD1: {struct
Cyc_Absyn_Aggrfield*_tmp1FE=Cyc_Absyn_lookup_field(_tmp1FA,f);if(_tmp1FE == 0){
const char*_tmpADF;void*_tmpADE[1];struct Cyc_String_pa_struct _tmpADD;(_tmpADD.tag=
0,((_tmpADD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpADE[0]=&
_tmpADD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADF="is_poly_field: bad field %s",_tag_dyneither(_tmpADF,sizeof(char),28))),
_tag_dyneither(_tmpADE,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp1FE->type);}_LLD2:;_LLD3: {const char*_tmpAE3;void*_tmpAE2[1];struct Cyc_String_pa_struct
_tmpAE1;(_tmpAE1.tag=0,((_tmpAE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAE2[0]=& _tmpAE1,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE3="is_poly_field: bad type %s",
_tag_dyneither(_tmpAE3,sizeof(char),27))),_tag_dyneither(_tmpAE2,sizeof(void*),1)))))));}
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
_tmp20B);_LLDE:;_LLDF: {const char*_tmpAE7;void*_tmpAE6[1];struct Cyc_String_pa_struct
_tmpAE5;(_tmpAE5.tag=0,((_tmpAE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp20A->topt))->v)),((
_tmpAE6[0]=& _tmpAE5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAE7="is_poly_project: bad type %s",_tag_dyneither(_tmpAE7,sizeof(char),29))),
_tag_dyneither(_tmpAE6,sizeof(void*),1)))))));}_LLDB:;}_LLD9:;_LLDA: return 0;
_LLD4:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s);
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*
_tmpAE8;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpAE8=_cycalloc(
sizeof(*_tmpAE8)),((_tmpAE8->hd=s,((_tmpAE8->tl=0,_tmpAE8)))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpAE9;return
Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpAE9=_cycalloc(sizeof(*
_tmpAE9)),((_tmpAE9->hd=s,((_tmpAE9->tl=0,_tmpAE9)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpAEA[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpAEA[1]=s,((_tmpAEA[0]=
rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAEA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n);
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpAEB[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpAEB[1]=n,((
_tmpAEB[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAEB,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpAEC[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpAEC[1]=n,((
_tmpAEC[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAEC,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n);static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_tmpAED[3];return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,((_tmpAED[2]=n,((_tmpAED[1]=s,((_tmpAED[0]=rgn,((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAED,
sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpAEE;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->hd=e,((_tmpAEE->tl=0,_tmpAEE)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt();static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){static struct Cyc_Absyn_Stmt**throw_match_stmt_opt=0;
if(throw_match_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpAEF;throw_match_stmt_opt=((
_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0)),0),_tmpAEF))));}return*throw_match_stmt_opt;}static
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
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpAF5;struct
Cyc_Absyn_Var_d_struct _tmpAF4;struct Cyc_List_List*_tmpAF3;Cyc_Toc_result_decls=((
_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3->hd=Cyc_Absyn_new_decl((void*)((
_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5[0]=((_tmpAF4.tag=0,((_tmpAF4.f1=vd,
_tmpAF4)))),_tmpAF5)))),0),((_tmpAF3->tl=Cyc_Toc_result_decls,_tmpAF3))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple15*_tmpAFC;
struct _tuple15*_tmpAFB;struct _tuple15*_tmpAFA;struct _tuple15*_tmpAF9[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpAF9[2]=((_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((
_tmpAFA->f1=0,((_tmpAFA->f2=xplussz,_tmpAFA)))))),((_tmpAF9[1]=((_tmpAFB=
_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB->f1=0,((_tmpAFB->f2=xexp,_tmpAFB)))))),((
_tmpAF9[0]=((_tmpAFC=_cycalloc(sizeof(*_tmpAFC)),((_tmpAFC->f1=0,((_tmpAFC->f2=
xexp,_tmpAFC)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF9,sizeof(struct _tuple15*),3)))))))),0);}return urm_exp;};};}
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
struct _RegionHandle*r){struct Cyc_Toc_Env*_tmpAFD;return(_tmpAFD=_region_malloc(r,
sizeof(*_tmpAFD)),((_tmpAFD->break_lab=(struct _dyneither_ptr**)0,((_tmpAFD->continue_lab=(
struct _dyneither_ptr**)0,((_tmpAFD->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpAFD->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpAFD->toplevel=(int)1,((_tmpAFD->rgn=(struct _RegionHandle*)r,_tmpAFD)))))))))))));}
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp22E;struct _dyneither_ptr**_tmp22F;struct _dyneither_ptr**
_tmp230;struct Cyc_Toc_FallthruInfo*_tmp231;struct Cyc_Dict_Dict _tmp232;int _tmp233;
struct Cyc_Toc_Env*_tmp22D=e;_tmp22E=*_tmp22D;_tmp22F=_tmp22E.break_lab;_tmp230=
_tmp22E.continue_lab;_tmp231=_tmp22E.fallthru_info;_tmp232=_tmp22E.varmap;
_tmp233=_tmp22E.toplevel;{struct Cyc_Toc_Env*_tmpAFE;return(_tmpAFE=
_region_malloc(r,sizeof(*_tmpAFE)),((_tmpAFE->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22F),((_tmpAFE->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp230),((_tmpAFE->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp231,((_tmpAFE->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp232),((_tmpAFE->toplevel=(
int)_tmp233,((_tmpAFE->rgn=(struct _RegionHandle*)r,_tmpAFE)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct
Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp236;struct _dyneither_ptr**_tmp237;struct
_dyneither_ptr**_tmp238;struct Cyc_Toc_FallthruInfo*_tmp239;struct Cyc_Dict_Dict
_tmp23A;int _tmp23B;struct Cyc_Toc_Env*_tmp235=e;_tmp236=*_tmp235;_tmp237=_tmp236.break_lab;
_tmp238=_tmp236.continue_lab;_tmp239=_tmp236.fallthru_info;_tmp23A=_tmp236.varmap;
_tmp23B=_tmp236.toplevel;{struct Cyc_Toc_Env*_tmpAFF;return(_tmpAFF=
_region_malloc(r,sizeof(*_tmpAFF)),((_tmpAFF->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp237),((_tmpAFF->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp238),((_tmpAFF->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp239,((_tmpAFF->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp23A),((_tmpAFF->toplevel=(
int)0,((_tmpAFF->rgn=(struct _RegionHandle*)r,_tmpAFF)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp23E;struct _dyneither_ptr**_tmp23F;struct _dyneither_ptr**
_tmp240;struct Cyc_Toc_FallthruInfo*_tmp241;struct Cyc_Dict_Dict _tmp242;int _tmp243;
struct Cyc_Toc_Env*_tmp23D=e;_tmp23E=*_tmp23D;_tmp23F=_tmp23E.break_lab;_tmp240=
_tmp23E.continue_lab;_tmp241=_tmp23E.fallthru_info;_tmp242=_tmp23E.varmap;
_tmp243=_tmp23E.toplevel;{struct Cyc_Toc_Env*_tmpB00;return(_tmpB00=
_region_malloc(r,sizeof(*_tmpB00)),((_tmpB00->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23F),((_tmpB00->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp240),((_tmpB00->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp241,((_tmpB00->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp242),((_tmpB00->toplevel=(
int)1,((_tmpB00->rgn=(struct _RegionHandle*)r,_tmpB00)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,
struct _tuple0*x,struct Cyc_Absyn_Exp*y);static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp245=(*x).f1;struct Cyc_List_List*_tmp246;_LLE6: if((
_tmp245.Rel_n).tag != 1)goto _LLE8;_tmp246=(struct Cyc_List_List*)(_tmp245.Rel_n).val;
_LLE7: {const char*_tmpB04;void*_tmpB03[1];struct Cyc_String_pa_struct _tmpB02;(
_tmpB02.tag=0,((_tmpB02.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpB03[0]=& _tmpB02,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB04="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpB04,sizeof(char),30))),_tag_dyneither(_tmpB03,sizeof(void*),1)))))));}_LLE8:;
_LLE9: goto _LLE5;_LLE5:;}{struct Cyc_Toc_Env _tmp24B;struct _dyneither_ptr**_tmp24C;
struct _dyneither_ptr**_tmp24D;struct Cyc_Toc_FallthruInfo*_tmp24E;struct Cyc_Dict_Dict
_tmp24F;int _tmp250;struct Cyc_Toc_Env*_tmp24A=e;_tmp24B=*_tmp24A;_tmp24C=_tmp24B.break_lab;
_tmp24D=_tmp24B.continue_lab;_tmp24E=_tmp24B.fallthru_info;_tmp24F=_tmp24B.varmap;
_tmp250=_tmp24B.toplevel;{struct Cyc_Dict_Dict _tmp251=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp24F),x,y);struct Cyc_Toc_Env*_tmpB05;return(_tmpB05=_region_malloc(r,
sizeof(*_tmpB05)),((_tmpB05->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp24C),((_tmpB05->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp24D),((_tmpB05->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24E,((_tmpB05->varmap=(struct Cyc_Dict_Dict)_tmp251,((_tmpB05->toplevel=(int)
_tmp250,((_tmpB05->rgn=(struct _RegionHandle*)r,_tmpB05)))))))))))));};};}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp254;struct _dyneither_ptr**_tmp255;struct _dyneither_ptr**
_tmp256;struct Cyc_Toc_FallthruInfo*_tmp257;struct Cyc_Dict_Dict _tmp258;int _tmp259;
struct Cyc_Toc_Env*_tmp253=e;_tmp254=*_tmp253;_tmp255=_tmp254.break_lab;_tmp256=
_tmp254.continue_lab;_tmp257=_tmp254.fallthru_info;_tmp258=_tmp254.varmap;
_tmp259=_tmp254.toplevel;{struct Cyc_Toc_Env*_tmpB06;return(_tmpB06=
_region_malloc(r,sizeof(*_tmpB06)),((_tmpB06->break_lab=(struct _dyneither_ptr**)
0,((_tmpB06->continue_lab=(struct _dyneither_ptr**)0,((_tmpB06->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp257,((_tmpB06->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp258),((_tmpB06->toplevel=(int)_tmp259,((_tmpB06->rgn=(struct _RegionHandle*)
r,_tmpB06)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env);static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpB07;fallthru_vars=((
_tmpB07=_region_malloc(r,sizeof(*_tmpB07)),((_tmpB07->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpB07->tl=fallthru_vars,_tmpB07))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp25D;struct _dyneither_ptr**_tmp25E;struct
_dyneither_ptr**_tmp25F;struct Cyc_Toc_FallthruInfo*_tmp260;struct Cyc_Dict_Dict
_tmp261;int _tmp262;struct Cyc_Toc_Env*_tmp25C=e;_tmp25D=*_tmp25C;_tmp25E=_tmp25D.break_lab;
_tmp25F=_tmp25D.continue_lab;_tmp260=_tmp25D.fallthru_info;_tmp261=_tmp25D.varmap;
_tmp262=_tmp25D.toplevel;{struct Cyc_Toc_Env _tmp264;struct Cyc_Dict_Dict _tmp265;
struct Cyc_Toc_Env*_tmp263=next_case_env;_tmp264=*_tmp263;_tmp265=_tmp264.varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB08;struct Cyc_Toc_FallthruInfo*fi=(_tmpB08=
_region_malloc(r,sizeof(*_tmpB08)),((_tmpB08->label=fallthru_l,((_tmpB08->binders=
fallthru_vars,((_tmpB08->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp265),_tmpB08)))))));
struct _dyneither_ptr**_tmpB0B;struct Cyc_Toc_Env*_tmpB0A;return(_tmpB0A=
_region_malloc(r,sizeof(*_tmpB0A)),((_tmpB0A->break_lab=(struct _dyneither_ptr**)((
_tmpB0B=_region_malloc(r,sizeof(*_tmpB0B)),((_tmpB0B[0]=break_l,_tmpB0B)))),((
_tmpB0A->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp25F),((
_tmpB0A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB0A->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp261),((_tmpB0A->toplevel=(int)_tmp262,((_tmpB0A->rgn=(
struct _RegionHandle*)r,_tmpB0A)))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l);static
struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp26A;struct _dyneither_ptr**
_tmp26B;struct _dyneither_ptr**_tmp26C;struct Cyc_Toc_FallthruInfo*_tmp26D;struct
Cyc_Dict_Dict _tmp26E;int _tmp26F;struct Cyc_Toc_Env*_tmp269=e;_tmp26A=*_tmp269;
_tmp26B=_tmp26A.break_lab;_tmp26C=_tmp26A.continue_lab;_tmp26D=_tmp26A.fallthru_info;
_tmp26E=_tmp26A.varmap;_tmp26F=_tmp26A.toplevel;{struct _dyneither_ptr**_tmpB0E;
struct Cyc_Toc_Env*_tmpB0D;return(_tmpB0D=_region_malloc(r,sizeof(*_tmpB0D)),((
_tmpB0D->break_lab=(struct _dyneither_ptr**)((_tmpB0E=_region_malloc(r,sizeof(*
_tmpB0E)),((_tmpB0E[0]=break_l,_tmpB0E)))),((_tmpB0D->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp26C),((_tmpB0D->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB0D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp26E),((_tmpB0D->toplevel=(
int)_tmp26F,((_tmpB0D->rgn=(struct _RegionHandle*)r,_tmpB0D)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l);static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp273;struct _dyneither_ptr**_tmp274;struct _dyneither_ptr**_tmp275;struct Cyc_Toc_FallthruInfo*
_tmp276;struct Cyc_Dict_Dict _tmp277;int _tmp278;struct Cyc_Toc_Env*_tmp272=e;
_tmp273=*_tmp272;_tmp274=_tmp273.break_lab;_tmp275=_tmp273.continue_lab;_tmp276=
_tmp273.fallthru_info;_tmp277=_tmp273.varmap;_tmp278=_tmp273.toplevel;{struct Cyc_Toc_FallthruInfo*
_tmpB11;struct Cyc_Toc_Env*_tmpB10;return(_tmpB10=_region_malloc(r,sizeof(*
_tmpB10)),((_tmpB10->break_lab=(struct _dyneither_ptr**)0,((_tmpB10->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp275),((_tmpB10->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)((_tmpB11=_region_malloc(r,sizeof(*_tmpB11)),((
_tmpB11->label=next_l,((_tmpB11->binders=0,((_tmpB11->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),_tmpB11)))))))),((_tmpB10->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp277),((_tmpB10->toplevel=(int)_tmp278,((_tmpB10->rgn=(struct _RegionHandle*)
r,_tmpB10)))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp27B=e->annot;_LLEB: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp27C=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp27B;if(_tmp27C->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLED;}_LLEC: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLED: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp27D=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp27B;if(_tmp27D->tag != Cyc_CfFlowInfo_NotZero)
goto _LLEF;}_LLEE: return 0;_LLEF: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp27E=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp27B;if(_tmp27E->tag != Cyc_CfFlowInfo_IsZero)
goto _LLF1;}_LLF0:{const char*_tmpB14;void*_tmpB13;(_tmpB13=0,Cyc_Tcutil_terr(e->loc,((
_tmpB14="dereference of NULL pointer",_tag_dyneither(_tmpB14,sizeof(char),28))),
_tag_dyneither(_tmpB13,sizeof(void*),0)));}return 0;_LLF1: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp27F=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp27B;if(_tmp27F->tag != Cyc_Absyn_EmptyAnnot)
goto _LLF3;}_LLF2: return 0;_LLF3:;_LLF4: {const char*_tmpB17;void*_tmpB16;(_tmpB16=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB17="need_null_check",_tag_dyneither(_tmpB17,sizeof(char),16))),
_tag_dyneither(_tmpB16,sizeof(void*),0)));}_LLEA:;}static struct Cyc_List_List*Cyc_Toc_get_relns(
struct Cyc_Absyn_Exp*e);static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*
e){void*_tmp284=e->annot;struct Cyc_List_List*_tmp286;struct Cyc_List_List*_tmp288;
_LLF6: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp285=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp284;if(_tmp285->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLF8;else{_tmp286=_tmp285->f1;}}
_LLF7: return _tmp286;_LLF8: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp287=(struct
Cyc_CfFlowInfo_NotZero_struct*)_tmp284;if(_tmp287->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFA;else{_tmp288=_tmp287->f1;}}_LLF9: return _tmp288;_LLFA: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp289=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp284;if(_tmp289->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFC;}_LLFB:{const char*_tmpB1A;void*_tmpB19;(_tmpB19=0,Cyc_Tcutil_terr(e->loc,((
_tmpB1A="dereference of NULL pointer",_tag_dyneither(_tmpB1A,sizeof(char),28))),
_tag_dyneither(_tmpB19,sizeof(void*),0)));}return 0;_LLFC: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp28A=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp284;if(_tmp28A->tag != Cyc_Absyn_EmptyAnnot)
goto _LLFE;}_LLFD: return 0;_LLFE:;_LLFF: {const char*_tmpB1D;void*_tmpB1C;(_tmpB1C=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB1D="get_relns",_tag_dyneither(_tmpB1D,sizeof(char),10))),_tag_dyneither(
_tmpB1C,sizeof(void*),0)));}_LLF5:;}static int Cyc_Toc_check_const_array(
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
bogus_qvar={{.Loc_n={3,0}},& bogus_string};static struct Cyc_Absyn_Vardecl
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
struct _tuple17 _tmpB1E;struct _tuple17 _tmp324=(_tmpB1E.f1=Cyc_Tcutil_compress(
_tmp320),((_tmpB1E.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmpB1E)));void*_tmp325;void*_tmp327;void*_tmp328;
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
_tmpB21;void*_tmpB20;(_tmpB20=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB21="Missing type in primop ",
_tag_dyneither(_tmpB21,sizeof(char),24))),_tag_dyneither(_tmpB20,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*_tmpB24;void*_tmpB23;(_tmpB23=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB24="Missing type in primop ",_tag_dyneither(_tmpB24,sizeof(char),24))),
_tag_dyneither(_tmpB23,sizeof(void*),0)));}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static struct _tuple9*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*
e);static struct _tuple9*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple9*
_tmpB25;return(_tmpB25=_cycalloc(sizeof(*_tmpB25)),((_tmpB25->f1=Cyc_Toc_mt_tq,((
_tmpB25->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmpB25)))));}static struct _tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e);static struct _tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);{struct _tuple15*_tmpB26;return(
_tmpB26=_cycalloc(sizeof(*_tmpB26)),((_tmpB26->f1=0,((_tmpB26->f2=e,_tmpB26)))));};}
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
count --,0);else{if(_tmp344->tl != 0){const char*_tmpB29;void*_tmpB28;(_tmpB28=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB29="multiple designators in array",
_tag_dyneither(_tmpB29,sizeof(char),30))),_tag_dyneither(_tmpB28,sizeof(void*),0)));}{
void*_tmp348=(void*)_tmp344->hd;void*_tmp349=_tmp348;struct Cyc_Absyn_Exp*_tmp34B;
_LL174: {struct Cyc_Absyn_ArrayElement_struct*_tmp34A=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp349;if(_tmp34A->tag != 0)goto _LL176;else{_tmp34B=_tmp34A->f1;}}_LL175: Cyc_Toc_exp_to_c(
nv,_tmp34B);e_index=_tmp34B;goto _LL173;_LL176: {struct Cyc_Absyn_FieldName_struct*
_tmp34C=(struct Cyc_Absyn_FieldName_struct*)_tmp349;if(_tmp34C->tag != 1)goto
_LL173;}_LL177: {const char*_tmpB2C;void*_tmpB2B;(_tmpB2B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB2C="field name designators in array",
_tag_dyneither(_tmpB2C,sizeof(char),32))),_tag_dyneither(_tmpB2B,sizeof(void*),0)));}
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
_tmpB2F;struct Cyc_Absyn_Local_b_struct*_tmpB2E;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp35E,nv,_tmp35A,Cyc_Absyn_varb_exp(_tmp35A,(void*)((_tmpB2E=_cycalloc(sizeof(*
_tmpB2E)),((_tmpB2E[0]=((_tmpB2F.tag=4,((_tmpB2F.f1=vd,_tmpB2F)))),_tmpB2E)))),0));
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
if(_tmp36F == 0){const char*_tmpB32;void*_tmpB31;(_tmpB31=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB32="empty designator list",
_tag_dyneither(_tmpB32,sizeof(char),22))),_tag_dyneither(_tmpB31,sizeof(void*),0)));}
if(_tmp36F->tl != 0){const char*_tmpB35;void*_tmpB34;(_tmpB34=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB35="too many designators in anonymous struct",
_tag_dyneither(_tmpB35,sizeof(char),41))),_tag_dyneither(_tmpB34,sizeof(void*),0)));}{
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
_tmpB38;void*_tmpB37;(_tmpB37=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB38="array designator in struct",
_tag_dyneither(_tmpB38,sizeof(char),27))),_tag_dyneither(_tmpB37,sizeof(void*),0)));}
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
struct Cyc_String_pa_struct _tmpB40;void*_tmpB3F[1];const char*_tmpB3E;void*_tmpB3D;(
_tmpB3D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB40.tag=0,((_tmpB40.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB3F[0]=& _tmpB40,Cyc_aprintf(((_tmpB3E="get_member_offset %s failed",
_tag_dyneither(_tmpB3E,sizeof(char),28))),_tag_dyneither(_tmpB3F,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB3D,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
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
const char*_tmpB43;void*_tmpB42;(_tmpB42=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB43="init_struct: bad struct type",
_tag_dyneither(_tmpB43,sizeof(char),29))),_tag_dyneither(_tmpB42,sizeof(void*),0)));}
_LL19F:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp3A9;struct _RegionHandle*_tmp3AA;struct Cyc_Toc_Env*_tmp3A8=
nv;_tmp3A9=*_tmp3A8;_tmp3AA=_tmp3A9.rgn;{struct Cyc_List_List*_tmp3AB=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp3AA,dles);for(0;
_tmp3AB != 0;_tmp3AB=_tmp3AB->tl){struct _tuple15 _tmp3AD;struct Cyc_List_List*
_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AF;struct _tuple15*_tmp3AC=(struct _tuple15*)
_tmp3AB->hd;_tmp3AD=*_tmp3AC;_tmp3AE=_tmp3AD.f1;_tmp3AF=_tmp3AD.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3AF->topt))->v);
if(_tmp3AE == 0){const char*_tmpB46;void*_tmpB45;(_tmpB45=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB46="empty designator list",
_tag_dyneither(_tmpB46,sizeof(char),22))),_tag_dyneither(_tmpB45,sizeof(void*),0)));}
if(_tmp3AE->tl != 0){struct _tuple0*_tmp3B2=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3B3=Cyc_Absyn_var_exp(_tmp3B2,0);for(0;_tmp3AE != 0;_tmp3AE=_tmp3AE->tl){void*
_tmp3B4=(void*)_tmp3AE->hd;struct _dyneither_ptr*_tmp3B6;_LL1A5: {struct Cyc_Absyn_FieldName_struct*
_tmp3B5=(struct Cyc_Absyn_FieldName_struct*)_tmp3B4;if(_tmp3B5->tag != 1)goto
_LL1A7;else{_tmp3B6=_tmp3B5->f1;}}_LL1A6: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3B6))_tmp3B3=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3B3);_tmp39F=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A0(_tmp39E,_tmp3B6,0),_tmp3B3,0),0),
_tmp39F,0);goto _LL1A4;_LL1A7:;_LL1A8: {const char*_tmpB49;void*_tmpB48;(_tmpB48=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB49="array designator in struct",_tag_dyneither(_tmpB49,sizeof(char),27))),
_tag_dyneither(_tmpB48,sizeof(void*),0)));}_LL1A4:;}Cyc_Toc_exp_to_c(nv,_tmp3AF);
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
const char*_tmpB4C;void*_tmpB4B;(_tmpB4B=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4C="array designator in struct",
_tag_dyneither(_tmpB4C,sizeof(char),27))),_tag_dyneither(_tmpB4B,sizeof(void*),0)));}
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
struct Cyc_Absyn_Increment_e_struct _tmpB4F;struct Cyc_Absyn_Increment_e_struct*
_tmpB4E;return Cyc_Absyn_new_exp((void*)((_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((
_tmpB4E[0]=((_tmpB4F.tag=5,((_tmpB4F.f1=e1,((_tmpB4F.f2=incr,_tmpB4F)))))),
_tmpB4E)))),0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
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
_tmpB50;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((_tmpB50=_cycalloc(
sizeof(*_tmpB50)),((_tmpB50->hd=_tmp3D4,((_tmpB50->tl=fs,_tmpB50)))))),f,f_env);}
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
_tmp3DF,fs,f,f_env);goto _LL1C0;_LL1C7:;_LL1C8: {const char*_tmpB54;void*_tmpB53[1];
struct Cyc_String_pa_struct _tmpB52;(_tmpB52.tag=0,((_tmpB52.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB53[0]=&
_tmpB52,Cyc_Toc_toc_impos(((_tmpB54="lvalue_assign_stmt: %s",_tag_dyneither(
_tmpB54,sizeof(char),23))),_tag_dyneither(_tmpB53,sizeof(void*),1)))))));}_LL1C0:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp3E3=
e->r;void*_tmp3E5;void**_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E7;struct Cyc_Absyn_Exp**
_tmp3E8;struct Cyc_Absyn_Exp*_tmp3EA;_LL1CA: {struct Cyc_Absyn_Cast_e_struct*
_tmp3E4=(struct Cyc_Absyn_Cast_e_struct*)_tmp3E3;if(_tmp3E4->tag != 15)goto _LL1CC;
else{_tmp3E5=(void**)& _tmp3E4->f1;_tmp3E6=(void**)((void**)& _tmp3E4->f1);_tmp3E7=
_tmp3E4->f2;_tmp3E8=(struct Cyc_Absyn_Exp**)& _tmp3E4->f2;}}_LL1CB:*_tmp3E8=Cyc_Toc_push_address_exp(*
_tmp3E8);*_tmp3E6=Cyc_Absyn_cstar_typ(*_tmp3E6,Cyc_Toc_mt_tq);return e;_LL1CC: {
struct Cyc_Absyn_Deref_e_struct*_tmp3E9=(struct Cyc_Absyn_Deref_e_struct*)_tmp3E3;
if(_tmp3E9->tag != 21)goto _LL1CE;else{_tmp3EA=_tmp3E9->f1;}}_LL1CD: return _tmp3EA;
_LL1CE:;_LL1CF: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{const
char*_tmpB58;void*_tmpB57[1];struct Cyc_String_pa_struct _tmpB56;(_tmpB56.tag=0,((
_tmpB56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmpB57[0]=& _tmpB56,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB58="can't take & of %s",_tag_dyneither(_tmpB58,sizeof(char),19))),
_tag_dyneither(_tmpB57,sizeof(void*),1)))))));};_LL1C9:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(
void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;struct
Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpB59;result=((
_tmpB59=_region_malloc(r2,sizeof(*_tmpB59)),((_tmpB59->hd=(void*)f((void*)x->hd,
env),((_tmpB59->tl=0,_tmpB59))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpB5A;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpB5A=_region_malloc(r2,sizeof(*_tmpB5A)),((_tmpB5A->hd=(void*)f((void*)x->hd,
env),((_tmpB5A->tl=0,_tmpB5A))))));}prev=prev->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);static
struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}static struct _tuple15*Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp*e);static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){struct _tuple15*_tmpB5B;return(_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((_tmpB5B->f1=
0,((_tmpB5B->f2=e,_tmpB5B)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(
void*t);static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp3F1=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp3F3;_LL1D1: {struct Cyc_Absyn_PointerType_struct*
_tmp3F2=(struct Cyc_Absyn_PointerType_struct*)_tmp3F1;if(_tmp3F2->tag != 5)goto
_LL1D3;else{_tmp3F3=_tmp3F2->f1;}}_LL1D2: return _tmp3F3;_LL1D3:;_LL1D4: {const
char*_tmpB5E;void*_tmpB5D;(_tmpB5D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5E="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpB5E,sizeof(char),28))),_tag_dyneither(_tmpB5D,sizeof(void*),0)));}
_LL1D0:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t);static struct Cyc_Absyn_Exp*
Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*res;{void*_tmp3F6=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Sign _tmp3F9;enum Cyc_Absyn_Size_of _tmp3FA;enum Cyc_Absyn_Sign
_tmp3FC;enum Cyc_Absyn_Size_of _tmp3FD;enum Cyc_Absyn_Sign _tmp401;enum Cyc_Absyn_Size_of
_tmp402;enum Cyc_Absyn_Sign _tmp404;enum Cyc_Absyn_Size_of _tmp405;enum Cyc_Absyn_Sign
_tmp407;enum Cyc_Absyn_Size_of _tmp408;_LL1D6: {struct Cyc_Absyn_PointerType_struct*
_tmp3F7=(struct Cyc_Absyn_PointerType_struct*)_tmp3F6;if(_tmp3F7->tag != 5)goto
_LL1D8;}_LL1D7: res=Cyc_Absyn_null_exp(0);goto _LL1D5;_LL1D8: {struct Cyc_Absyn_IntType_struct*
_tmp3F8=(struct Cyc_Absyn_IntType_struct*)_tmp3F6;if(_tmp3F8->tag != 6)goto _LL1DA;
else{_tmp3F9=_tmp3F8->f1;_tmp3FA=_tmp3F8->f2;if(_tmp3FA != Cyc_Absyn_Char_sz)goto
_LL1DA;}}_LL1D9: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3F9,'\000'),0);goto
_LL1D5;_LL1DA: {struct Cyc_Absyn_IntType_struct*_tmp3FB=(struct Cyc_Absyn_IntType_struct*)
_tmp3F6;if(_tmp3FB->tag != 6)goto _LL1DC;else{_tmp3FC=_tmp3FB->f1;_tmp3FD=_tmp3FB->f2;
if(_tmp3FD != Cyc_Absyn_Short_sz)goto _LL1DC;}}_LL1DB: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp3FC,0),0);goto _LL1D5;_LL1DC: {struct Cyc_Absyn_EnumType_struct*_tmp3FE=(
struct Cyc_Absyn_EnumType_struct*)_tmp3F6;if(_tmp3FE->tag != 14)goto _LL1DE;}_LL1DD:
goto _LL1DF;_LL1DE: {struct Cyc_Absyn_AnonEnumType_struct*_tmp3FF=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3F6;if(_tmp3FF->tag != 15)goto _LL1E0;}_LL1DF: _tmp401=Cyc_Absyn_Unsigned;goto
_LL1E1;_LL1E0: {struct Cyc_Absyn_IntType_struct*_tmp400=(struct Cyc_Absyn_IntType_struct*)
_tmp3F6;if(_tmp400->tag != 6)goto _LL1E2;else{_tmp401=_tmp400->f1;_tmp402=_tmp400->f2;
if(_tmp402 != Cyc_Absyn_Int_sz)goto _LL1E2;}}_LL1E1: _tmp404=_tmp401;goto _LL1E3;
_LL1E2: {struct Cyc_Absyn_IntType_struct*_tmp403=(struct Cyc_Absyn_IntType_struct*)
_tmp3F6;if(_tmp403->tag != 6)goto _LL1E4;else{_tmp404=_tmp403->f1;_tmp405=_tmp403->f2;
if(_tmp405 != Cyc_Absyn_Long_sz)goto _LL1E4;}}_LL1E3: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(
_tmp404,0),0);goto _LL1D5;_LL1E4: {struct Cyc_Absyn_IntType_struct*_tmp406=(struct
Cyc_Absyn_IntType_struct*)_tmp3F6;if(_tmp406->tag != 6)goto _LL1E6;else{_tmp407=
_tmp406->f1;_tmp408=_tmp406->f2;if(_tmp408 != Cyc_Absyn_LongLong_sz)goto _LL1E6;}}
_LL1E5: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp407,(long long)0),0);
goto _LL1D5;_LL1E6: {struct Cyc_Absyn_FloatType_struct*_tmp409=(struct Cyc_Absyn_FloatType_struct*)
_tmp3F6;if(_tmp409->tag != 7)goto _LL1E8;}_LL1E7: goto _LL1E9;_LL1E8: {struct Cyc_Absyn_DoubleType_struct*
_tmp40A=(struct Cyc_Absyn_DoubleType_struct*)_tmp3F6;if(_tmp40A->tag != 8)goto
_LL1EA;}_LL1E9:{const char*_tmpB5F;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpB5F="0.0",_tag_dyneither(_tmpB5F,sizeof(char),4)))),0);}goto _LL1D5;_LL1EA:;
_LL1EB: {const char*_tmpB63;void*_tmpB62[1];struct Cyc_String_pa_struct _tmpB61;(
_tmpB61.tag=0,((_tmpB61.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpB62[0]=& _tmpB61,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB63="found non-zero type %s in generate_zero",
_tag_dyneither(_tmpB63,sizeof(char),40))),_tag_dyneither(_tmpB62,sizeof(void*),1)))))));}
_LL1D5:;}{struct Cyc_Core_Opt*_tmpB64;res->topt=((_tmpB64=_cycalloc(sizeof(*
_tmpB64)),((_tmpB64->v=t,_tmpB64))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type);static
void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp410=Cyc_Toc_typ_to_c(elt_type);void*_tmp411=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp412=Cyc_Absyn_cstar_typ(_tmp410,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpB65;struct Cyc_Core_Opt*_tmp413=(_tmpB65=_cycalloc(sizeof(*_tmpB65)),((
_tmpB65->v=_tmp412,_tmpB65)));struct Cyc_Absyn_Exp*xinit;{void*_tmp414=e1->r;
struct Cyc_Absyn_Exp*_tmp416;struct Cyc_Absyn_Exp*_tmp418;struct Cyc_Absyn_Exp*
_tmp419;_LL1ED: {struct Cyc_Absyn_Deref_e_struct*_tmp415=(struct Cyc_Absyn_Deref_e_struct*)
_tmp414;if(_tmp415->tag != 21)goto _LL1EF;else{_tmp416=_tmp415->f1;}}_LL1EE: if(!
is_dyneither){_tmp416=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp416,0,Cyc_Absyn_Other_coercion,
0);{struct Cyc_Core_Opt*_tmpB66;_tmp416->topt=((_tmpB66=_cycalloc(sizeof(*_tmpB66)),((
_tmpB66->v=fat_ptr_type,_tmpB66))));};}Cyc_Toc_exp_to_c(nv,_tmp416);xinit=
_tmp416;goto _LL1EC;_LL1EF: {struct Cyc_Absyn_Subscript_e_struct*_tmp417=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp414;if(_tmp417->tag != 24)goto _LL1F1;else{
_tmp418=_tmp417->f1;_tmp419=_tmp417->f2;}}_LL1F0: if(!is_dyneither){_tmp418=Cyc_Absyn_cast_exp(
fat_ptr_type,_tmp418,0,Cyc_Absyn_Other_coercion,0);{struct Cyc_Core_Opt*_tmpB67;
_tmp418->topt=((_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->v=fat_ptr_type,
_tmpB67))));};}Cyc_Toc_exp_to_c(nv,_tmp418);Cyc_Toc_exp_to_c(nv,_tmp419);{struct
Cyc_Absyn_Exp*_tmpB68[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpB68[2]=_tmp419,((_tmpB68[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpB68[0]=_tmp418,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB68,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1EC;_LL1F1:;_LL1F2: {const char*_tmpB6B;void*_tmpB6A;(_tmpB6A=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6B="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpB6B,sizeof(char),53))),_tag_dyneither(_tmpB6A,sizeof(void*),0)));}
_LL1EC:;}{struct _tuple0*_tmp41F=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp421;
struct _RegionHandle*_tmp422;struct Cyc_Toc_Env*_tmp420=nv;_tmp421=*_tmp420;
_tmp422=_tmp421.rgn;{struct Cyc_Toc_Env*_tmp423=Cyc_Toc_add_varmap(_tmp422,nv,
_tmp41F,Cyc_Absyn_var_exp(_tmp41F,0));struct Cyc_Absyn_Vardecl*_tmpB6C;struct Cyc_Absyn_Vardecl*
_tmp424=(_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C->sc=Cyc_Absyn_Public,((
_tmpB6C->name=_tmp41F,((_tmpB6C->tq=Cyc_Toc_mt_tq,((_tmpB6C->type=_tmp411,((
_tmpB6C->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpB6C->rgn=0,((_tmpB6C->attributes=
0,((_tmpB6C->escapes=0,_tmpB6C)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpB6F;struct Cyc_Absyn_Local_b_struct*_tmpB6E;struct Cyc_Absyn_Local_b_struct*
_tmp425=(_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((_tmpB6E[0]=((_tmpB6F.tag=4,((
_tmpB6F.f1=_tmp424,_tmpB6F)))),_tmpB6E)));struct Cyc_Absyn_Exp*_tmp426=Cyc_Absyn_varb_exp(
_tmp41F,(void*)_tmp425,0);{struct Cyc_Core_Opt*_tmpB70;_tmp426->topt=((_tmpB70=
_cycalloc(sizeof(*_tmpB70)),((_tmpB70->v=fat_ptr_type,_tmpB70))));}{struct Cyc_Absyn_Exp*
_tmp428=Cyc_Absyn_deref_exp(_tmp426,0);{struct Cyc_Core_Opt*_tmpB71;_tmp428->topt=((
_tmpB71=_cycalloc(sizeof(*_tmpB71)),((_tmpB71->v=elt_type,_tmpB71))));}Cyc_Toc_exp_to_c(
_tmp423,_tmp428);{struct _tuple0*_tmp42A=Cyc_Toc_temp_var();_tmp423=Cyc_Toc_add_varmap(
_tmp422,_tmp423,_tmp42A,Cyc_Absyn_var_exp(_tmp42A,0));{struct Cyc_Absyn_Vardecl*
_tmpB72;struct Cyc_Absyn_Vardecl*_tmp42B=(_tmpB72=_cycalloc(sizeof(*_tmpB72)),((
_tmpB72->sc=Cyc_Absyn_Public,((_tmpB72->name=_tmp42A,((_tmpB72->tq=Cyc_Toc_mt_tq,((
_tmpB72->type=_tmp410,((_tmpB72->initializer=(struct Cyc_Absyn_Exp*)_tmp428,((
_tmpB72->rgn=0,((_tmpB72->attributes=0,((_tmpB72->escapes=0,_tmpB72)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB75;struct Cyc_Absyn_Local_b_struct*_tmpB74;
struct Cyc_Absyn_Local_b_struct*_tmp42C=(_tmpB74=_cycalloc(sizeof(*_tmpB74)),((
_tmpB74[0]=((_tmpB75.tag=4,((_tmpB75.f1=_tmp42B,_tmpB75)))),_tmpB74)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp42D=Cyc_Absyn_varb_exp(_tmp42A,(
void*)_tmp42C,0);_tmp42D->topt=_tmp428->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp42D,e2,0);z_init->topt=_tmp42D->topt;}Cyc_Toc_exp_to_c(
_tmp423,z_init);{struct _tuple0*_tmp42E=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpB76;struct Cyc_Absyn_Vardecl*_tmp42F=(_tmpB76=_cycalloc(sizeof(*_tmpB76)),((
_tmpB76->sc=Cyc_Absyn_Public,((_tmpB76->name=_tmp42E,((_tmpB76->tq=Cyc_Toc_mt_tq,((
_tmpB76->type=_tmp410,((_tmpB76->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpB76->rgn=0,((_tmpB76->attributes=0,((_tmpB76->escapes=0,_tmpB76)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB79;struct Cyc_Absyn_Local_b_struct*_tmpB78;
struct Cyc_Absyn_Local_b_struct*_tmp430=(_tmpB78=_cycalloc(sizeof(*_tmpB78)),((
_tmpB78[0]=((_tmpB79.tag=4,((_tmpB79.f1=_tmp42F,_tmpB79)))),_tmpB78)));_tmp423=
Cyc_Toc_add_varmap(_tmp422,_tmp423,_tmp42E,Cyc_Absyn_var_exp(_tmp42E,0));{struct
Cyc_Absyn_Exp*_tmp431=Cyc_Absyn_varb_exp(_tmp42A,(void*)_tmp42C,0);_tmp431->topt=
_tmp428->topt;{struct Cyc_Absyn_Exp*_tmp432=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp433=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp431,_tmp432,0);{
struct Cyc_Core_Opt*_tmpB7A;_tmp433->topt=((_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((
_tmpB7A->v=Cyc_Absyn_sint_typ,_tmpB7A))));}Cyc_Toc_exp_to_c(_tmp423,_tmp433);{
struct Cyc_Absyn_Exp*_tmp435=Cyc_Absyn_varb_exp(_tmp42E,(void*)_tmp430,0);_tmp435->topt=
_tmp428->topt;{struct Cyc_Absyn_Exp*_tmp436=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp437=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp435,_tmp436,0);{
struct Cyc_Core_Opt*_tmpB7B;_tmp437->topt=((_tmpB7B=_cycalloc(sizeof(*_tmpB7B)),((
_tmpB7B->v=Cyc_Absyn_sint_typ,_tmpB7B))));}Cyc_Toc_exp_to_c(_tmp423,_tmp437);{
struct Cyc_Absyn_Exp*_tmpB7C[2];struct Cyc_List_List*_tmp439=(_tmpB7C[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpB7C[0]=Cyc_Absyn_varb_exp(_tmp41F,(void*)
_tmp425,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB7C,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp43A=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp439,0),
_tmp43A,0);{struct Cyc_Absyn_Exp*_tmp43B=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp433,_tmp437,0),0);struct Cyc_Absyn_Stmt*_tmp43C=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp43D=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp41F,(void*)_tmp425,0),Cyc_Toc_curr_sp,0);_tmp43D=Cyc_Toc_cast_it(
_tmp412,_tmp43D);{struct Cyc_Absyn_Exp*_tmp43E=Cyc_Absyn_deref_exp(_tmp43D,0);
struct Cyc_Absyn_Exp*_tmp43F=Cyc_Absyn_assign_exp(_tmp43E,Cyc_Absyn_var_exp(
_tmp42E,0),0);struct Cyc_Absyn_Stmt*_tmp440=Cyc_Absyn_exp_stmt(_tmp43F,0);_tmp440=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp43B,_tmp43C,Cyc_Absyn_skip_stmt(
0),0),_tmp440,0);{struct Cyc_Absyn_Var_d_struct*_tmpB82;struct Cyc_Absyn_Var_d_struct
_tmpB81;struct Cyc_Absyn_Decl*_tmpB80;_tmp440=Cyc_Absyn_decl_stmt(((_tmpB80=
_cycalloc(sizeof(*_tmpB80)),((_tmpB80->r=(void*)((_tmpB82=_cycalloc(sizeof(*
_tmpB82)),((_tmpB82[0]=((_tmpB81.tag=0,((_tmpB81.f1=_tmp42F,_tmpB81)))),_tmpB82)))),((
_tmpB80->loc=0,_tmpB80)))))),_tmp440,0);}{struct Cyc_Absyn_Var_d_struct*_tmpB88;
struct Cyc_Absyn_Var_d_struct _tmpB87;struct Cyc_Absyn_Decl*_tmpB86;_tmp440=Cyc_Absyn_decl_stmt(((
_tmpB86=_cycalloc(sizeof(*_tmpB86)),((_tmpB86->r=(void*)((_tmpB88=_cycalloc(
sizeof(*_tmpB88)),((_tmpB88[0]=((_tmpB87.tag=0,((_tmpB87.f1=_tmp42B,_tmpB87)))),
_tmpB88)))),((_tmpB86->loc=0,_tmpB86)))))),_tmp440,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpB8E;struct Cyc_Absyn_Var_d_struct _tmpB8D;struct Cyc_Absyn_Decl*_tmpB8C;_tmp440=
Cyc_Absyn_decl_stmt(((_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C->r=(void*)((
_tmpB8E=_cycalloc(sizeof(*_tmpB8E)),((_tmpB8E[0]=((_tmpB8D.tag=0,((_tmpB8D.f1=
_tmp424,_tmpB8D)))),_tmpB8E)))),((_tmpB8C->loc=0,_tmpB8C)))))),_tmp440,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp440);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,
struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*));static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*
e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*
aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)){
struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp455=Cyc_Tcutil_compress(aggrtype);struct
Cyc_Absyn_AggrInfo _tmp457;union Cyc_Absyn_AggrInfoU _tmp458;_LL1F4: {struct Cyc_Absyn_AggrType_struct*
_tmp456=(struct Cyc_Absyn_AggrType_struct*)_tmp455;if(_tmp456->tag != 12)goto
_LL1F6;else{_tmp457=_tmp456->f1;_tmp458=_tmp457.aggr_info;}}_LL1F5: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp458);goto _LL1F3;_LL1F6:;_LL1F7: {struct Cyc_String_pa_struct _tmpB96;void*
_tmpB95[1];const char*_tmpB94;void*_tmpB93;(_tmpB93=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpB96.tag=0,((_tmpB96.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB95[0]=& _tmpB96,Cyc_aprintf(((
_tmpB94="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpB94,sizeof(char),51))),_tag_dyneither(_tmpB95,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB93,sizeof(void*),0)));}_LL1F3:;}{struct _tuple0*_tmp45D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp45E=Cyc_Absyn_var_exp(_tmp45D,0);struct Cyc_Absyn_Exp*
_tmp45F=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp45E,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp460=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
struct Cyc_Absyn_Exp*_tmp461=Cyc_Absyn_neq_exp(_tmp45F,_tmp460,0);struct Cyc_Absyn_Exp*
_tmp462=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp45E,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp463=Cyc_Absyn_exp_stmt(_tmp462,0);struct Cyc_Absyn_Stmt*
_tmp464=Cyc_Absyn_ifthenelse_stmt(_tmp461,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp465=Cyc_Absyn_declare_stmt(_tmp45D,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp464,_tmp463,0),0);return Cyc_Toc_stmt_exp_r(
_tmp465);};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read);static int Cyc_Toc_is_tagged_union_project(
struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){void*
_tmp466=e->r;struct Cyc_Absyn_Exp*_tmp468;struct Cyc_Absyn_Exp*_tmp46A;struct
_dyneither_ptr*_tmp46B;int _tmp46C;int*_tmp46D;struct Cyc_Absyn_Exp*_tmp46F;struct
_dyneither_ptr*_tmp470;int _tmp471;int*_tmp472;_LL1F9: {struct Cyc_Absyn_Cast_e_struct*
_tmp467=(struct Cyc_Absyn_Cast_e_struct*)_tmp466;if(_tmp467->tag != 15)goto _LL1FB;
else{_tmp468=_tmp467->f2;}}_LL1FA: {const char*_tmpB99;void*_tmpB98;(_tmpB98=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB99="cast on lhs!",_tag_dyneither(_tmpB99,sizeof(char),13))),_tag_dyneither(
_tmpB98,sizeof(void*),0)));}_LL1FB: {struct Cyc_Absyn_AggrMember_e_struct*_tmp469=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp466;if(_tmp469->tag != 22)goto _LL1FD;
else{_tmp46A=_tmp469->f1;_tmp46B=_tmp469->f2;_tmp46C=_tmp469->f4;_tmp46D=(int*)&
_tmp469->f4;}}_LL1FC: {void*_tmp475=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp46A->topt))->v);struct Cyc_Absyn_AggrInfo _tmp477;union Cyc_Absyn_AggrInfoU
_tmp478;_LL202: {struct Cyc_Absyn_AggrType_struct*_tmp476=(struct Cyc_Absyn_AggrType_struct*)
_tmp475;if(_tmp476->tag != 12)goto _LL204;else{_tmp477=_tmp476->f1;_tmp478=_tmp477.aggr_info;}}
_LL203: {struct Cyc_Absyn_Aggrdecl*_tmp479=Cyc_Absyn_get_known_aggrdecl(_tmp478);*
f_tag=Cyc_Toc_get_member_offset(_tmp479,_tmp46B);{const char*_tmpB9E;void*_tmpB9D[
2];struct Cyc_String_pa_struct _tmpB9C;struct Cyc_String_pa_struct _tmpB9B;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpB9B.tag=0,((_tmpB9B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp46B),((_tmpB9C.tag=0,((_tmpB9C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp479->name).f2),((_tmpB9D[0]=&
_tmpB9C,((_tmpB9D[1]=& _tmpB9B,Cyc_aprintf(((_tmpB9E="_union_%s_%s",
_tag_dyneither(_tmpB9E,sizeof(char),13))),_tag_dyneither(_tmpB9D,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpB9F;*tagged_member_type=Cyc_Absyn_strct(((_tmpB9F=
_cycalloc(sizeof(*_tmpB9F)),((_tmpB9F[0]=str,_tmpB9F)))));}if(clear_read)*
_tmp46D=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp479->impl))->tagged;};}
_LL204:;_LL205: return 0;_LL201:;}_LL1FD: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp46E=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp466;if(_tmp46E->tag != 23)goto
_LL1FF;else{_tmp46F=_tmp46E->f1;_tmp470=_tmp46E->f2;_tmp471=_tmp46E->f4;_tmp472=(
int*)& _tmp46E->f4;}}_LL1FE: {void*_tmp47F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp46F->topt))->v);struct Cyc_Absyn_PtrInfo _tmp481;void*_tmp482;
_LL207: {struct Cyc_Absyn_PointerType_struct*_tmp480=(struct Cyc_Absyn_PointerType_struct*)
_tmp47F;if(_tmp480->tag != 5)goto _LL209;else{_tmp481=_tmp480->f1;_tmp482=_tmp481.elt_typ;}}
_LL208: {void*_tmp483=Cyc_Tcutil_compress(_tmp482);struct Cyc_Absyn_AggrInfo
_tmp485;union Cyc_Absyn_AggrInfoU _tmp486;_LL20C: {struct Cyc_Absyn_AggrType_struct*
_tmp484=(struct Cyc_Absyn_AggrType_struct*)_tmp483;if(_tmp484->tag != 12)goto
_LL20E;else{_tmp485=_tmp484->f1;_tmp486=_tmp485.aggr_info;}}_LL20D: {struct Cyc_Absyn_Aggrdecl*
_tmp487=Cyc_Absyn_get_known_aggrdecl(_tmp486);*f_tag=Cyc_Toc_get_member_offset(
_tmp487,_tmp470);{const char*_tmpBA4;void*_tmpBA3[2];struct Cyc_String_pa_struct
_tmpBA2;struct Cyc_String_pa_struct _tmpBA1;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpBA1.tag=0,((_tmpBA1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp470),((_tmpBA2.tag=0,((_tmpBA2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp487->name).f2),((_tmpBA3[0]=& _tmpBA2,((_tmpBA3[1]=&
_tmpBA1,Cyc_aprintf(((_tmpBA4="_union_%s_%s",_tag_dyneither(_tmpBA4,sizeof(char),
13))),_tag_dyneither(_tmpBA3,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpBA5;*tagged_member_type=Cyc_Absyn_strct(((_tmpBA5=_cycalloc(sizeof(*_tmpBA5)),((
_tmpBA5[0]=str,_tmpBA5)))));}if(clear_read)*_tmp472=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp487->impl))->tagged;};}_LL20E:;_LL20F: return 0;_LL20B:;}_LL209:;
_LL20A: return 0;_LL206:;}_LL1FF:;_LL200: return 0;_LL1F8:;}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type);static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple0*_tmp48D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp48D,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp48E=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp48E,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp48D,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{struct _tuple0*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpBF8(unsigned int*_tmpBF7,unsigned int*_tmpBF6,struct _tuple0***
_tmpBF4){for(*_tmpBF7=0;*_tmpBF7 < *_tmpBF6;(*_tmpBF7)++){(*_tmpBF4)[*_tmpBF7]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp48F=e->r;if(e->topt == 0){const char*_tmpBA9;void*_tmpBA8[1];struct Cyc_String_pa_struct
_tmpBA7;(_tmpBA7.tag=0,((_tmpBA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBA8[0]=& _tmpBA7,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA9="exp_to_c: no type for %s",
_tag_dyneither(_tmpBA9,sizeof(char),25))),_tag_dyneither(_tmpBA8,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp493=
_tmp48F;union Cyc_Absyn_Cnst _tmp495;int _tmp496;struct _tuple0*_tmp499;void*_tmp49A;
struct _tuple0*_tmp49C;enum Cyc_Absyn_Primop _tmp49E;struct Cyc_List_List*_tmp49F;
struct Cyc_Absyn_Exp*_tmp4A1;enum Cyc_Absyn_Incrementor _tmp4A2;struct Cyc_Absyn_Exp*
_tmp4A4;struct Cyc_Core_Opt*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*
_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*
_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*
_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*
_tmp4B5;struct Cyc_List_List*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B8;struct Cyc_List_List*
_tmp4B9;struct Cyc_Absyn_VarargCallInfo*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4BC;
struct Cyc_List_List*_tmp4BD;struct Cyc_Absyn_VarargCallInfo*_tmp4BE;struct Cyc_Absyn_VarargCallInfo
_tmp4BF;int _tmp4C0;struct Cyc_List_List*_tmp4C1;struct Cyc_Absyn_VarargInfo*
_tmp4C2;struct Cyc_Absyn_Exp*_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C6;struct Cyc_Absyn_Exp*
_tmp4C8;struct Cyc_List_List*_tmp4C9;void*_tmp4CB;void**_tmp4CC;struct Cyc_Absyn_Exp*
_tmp4CD;int _tmp4CE;enum Cyc_Absyn_Coercion _tmp4CF;struct Cyc_Absyn_Exp*_tmp4D1;
struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_Absyn_Exp*
_tmp4D6;void*_tmp4D8;void*_tmp4DA;void*_tmp4DB;struct _dyneither_ptr*_tmp4DD;void*
_tmp4DF;void*_tmp4E0;unsigned int _tmp4E2;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Absyn_Exp*
_tmp4E6;struct _dyneither_ptr*_tmp4E7;int _tmp4E8;int _tmp4E9;struct Cyc_Absyn_Exp*
_tmp4EB;struct _dyneither_ptr*_tmp4EC;int _tmp4ED;int _tmp4EE;struct Cyc_Absyn_Exp*
_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_List_List*_tmp4F3;struct Cyc_List_List*
_tmp4F5;struct Cyc_Absyn_Vardecl*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_Absyn_Exp*
_tmp4F9;int _tmp4FA;struct _tuple0*_tmp4FC;struct Cyc_List_List*_tmp4FD;struct Cyc_List_List*
_tmp4FE;struct Cyc_Absyn_Aggrdecl*_tmp4FF;void*_tmp501;struct Cyc_List_List*
_tmp502;struct Cyc_List_List*_tmp504;struct Cyc_Absyn_Datatypedecl*_tmp505;struct
Cyc_Absyn_Datatypefield*_tmp506;struct Cyc_Absyn_MallocInfo _tmp50A;int _tmp50B;
struct Cyc_Absyn_Exp*_tmp50C;void**_tmp50D;struct Cyc_Absyn_Exp*_tmp50E;int _tmp50F;
struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*
_tmp514;struct _dyneither_ptr*_tmp515;struct Cyc_Absyn_Stmt*_tmp517;_LL211: {
struct Cyc_Absyn_Const_e_struct*_tmp494=(struct Cyc_Absyn_Const_e_struct*)_tmp493;
if(_tmp494->tag != 0)goto _LL213;else{_tmp495=_tmp494->f1;if((_tmp495.Null_c).tag
!= 1)goto _LL213;_tmp496=(int)(_tmp495.Null_c).val;}}_LL212: {struct Cyc_Absyn_Exp*
_tmp51B=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp51B,
_tmp51B))->r;else{struct Cyc_Absyn_Exp*_tmpBAA[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBAA[2]=_tmp51B,((_tmpBAA[1]=_tmp51B,((_tmpBAA[0]=_tmp51B,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBAA,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL210;}_LL213: {struct Cyc_Absyn_Const_e_struct*
_tmp497=(struct Cyc_Absyn_Const_e_struct*)_tmp493;if(_tmp497->tag != 0)goto _LL215;}
_LL214: goto _LL210;_LL215: {struct Cyc_Absyn_Var_e_struct*_tmp498=(struct Cyc_Absyn_Var_e_struct*)
_tmp493;if(_tmp498->tag != 1)goto _LL217;else{_tmp499=_tmp498->f1;_tmp49A=(void*)
_tmp498->f2;}}_LL216:{struct _handler_cons _tmp51D;_push_handler(& _tmp51D);{int
_tmp51F=0;if(setjmp(_tmp51D.handler))_tmp51F=1;if(!_tmp51F){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp499))->r;;_pop_handler();}else{void*_tmp51E=(void*)_exn_thrown;void*
_tmp521=_tmp51E;_LL268: {struct Cyc_Dict_Absent_struct*_tmp522=(struct Cyc_Dict_Absent_struct*)
_tmp521;if(_tmp522->tag != Cyc_Dict_Absent)goto _LL26A;}_LL269: {const char*_tmpBAE;
void*_tmpBAD[1];struct Cyc_String_pa_struct _tmpBAC;(_tmpBAC.tag=0,((_tmpBAC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp499)),((
_tmpBAD[0]=& _tmpBAC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBAE="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBAE,sizeof(char),
32))),_tag_dyneither(_tmpBAD,sizeof(void*),1)))))));}_LL26A:;_LL26B:(void)_throw(
_tmp521);_LL267:;}};}goto _LL210;_LL217: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp49B=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp493;if(_tmp49B->tag != 2)goto
_LL219;else{_tmp49C=_tmp49B->f1;}}_LL218: {const char*_tmpBB1;void*_tmpBB0;(
_tmpBB0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBB1="unknownid",_tag_dyneither(_tmpBB1,sizeof(char),10))),_tag_dyneither(
_tmpBB0,sizeof(void*),0)));}_LL219: {struct Cyc_Absyn_Primop_e_struct*_tmp49D=(
struct Cyc_Absyn_Primop_e_struct*)_tmp493;if(_tmp49D->tag != 3)goto _LL21B;else{
_tmp49E=_tmp49D->f1;_tmp49F=_tmp49D->f2;}}_LL21A: {struct Cyc_List_List*_tmp528=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp49F);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp49F);switch(_tmp49E){case Cyc_Absyn_Numelts: _LL26C: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp49F))->hd;{void*_tmp529=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp52B;struct Cyc_Absyn_Exp*
_tmp52C;struct Cyc_Absyn_PtrInfo _tmp52E;void*_tmp52F;struct Cyc_Absyn_PtrAtts
_tmp530;union Cyc_Absyn_Constraint*_tmp531;union Cyc_Absyn_Constraint*_tmp532;
union Cyc_Absyn_Constraint*_tmp533;_LL26F: {struct Cyc_Absyn_ArrayType_struct*
_tmp52A=(struct Cyc_Absyn_ArrayType_struct*)_tmp529;if(_tmp52A->tag != 9)goto
_LL271;else{_tmp52B=_tmp52A->f1;_tmp52C=_tmp52B.num_elts;}}_LL270: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp52C))){const char*_tmpBB4;void*_tmpBB3;(
_tmpBB3=0,Cyc_Tcutil_terr(e->loc,((_tmpBB4="can't calculate numelts",
_tag_dyneither(_tmpBB4,sizeof(char),24))),_tag_dyneither(_tmpBB3,sizeof(void*),0)));}
e->r=_tmp52C->r;goto _LL26E;_LL271: {struct Cyc_Absyn_PointerType_struct*_tmp52D=(
struct Cyc_Absyn_PointerType_struct*)_tmp529;if(_tmp52D->tag != 5)goto _LL273;else{
_tmp52E=_tmp52D->f1;_tmp52F=_tmp52E.elt_typ;_tmp530=_tmp52E.ptr_atts;_tmp531=
_tmp530.nullable;_tmp532=_tmp530.bounds;_tmp533=_tmp530.zero_term;}}_LL272:{void*
_tmp536=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp532);struct Cyc_Absyn_Exp*_tmp539;_LL276: {struct Cyc_Absyn_DynEither_b_struct*
_tmp537=(struct Cyc_Absyn_DynEither_b_struct*)_tmp536;if(_tmp537->tag != 0)goto
_LL278;}_LL277:{struct Cyc_Absyn_Exp*_tmpBB5[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBB5[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52F),0),((_tmpBB5[0]=(
struct Cyc_Absyn_Exp*)_tmp49F->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBB5,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL275;_LL278: {struct Cyc_Absyn_Upper_b_struct*_tmp538=(struct Cyc_Absyn_Upper_b_struct*)
_tmp536;if(_tmp538->tag != 1)goto _LL275;else{_tmp539=_tmp538->f1;}}_LL279: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp533)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp49F->hd);struct Cyc_Absyn_Exp*_tmpBB6[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBB6[1]=_tmp539,((_tmpBB6[0]=(struct Cyc_Absyn_Exp*)_tmp49F->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBB6,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp531)){if(!Cyc_Evexp_c_can_eval(_tmp539)){const
char*_tmpBB9;void*_tmpBB8;(_tmpBB8=0,Cyc_Tcutil_terr(e->loc,((_tmpBB9="can't calculate numelts",
_tag_dyneither(_tmpBB9,sizeof(char),24))),_tag_dyneither(_tmpBB8,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp539,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp539->r;goto _LL275;}}goto _LL275;_LL275:;}goto _LL26E;_LL273:;_LL274: {const
char*_tmpBBE;void*_tmpBBD[2];struct Cyc_String_pa_struct _tmpBBC;struct Cyc_String_pa_struct
_tmpBBB;(_tmpBBB.tag=0,((_tmpBBB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpBBC.tag=0,((_tmpBBC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpBBD[0]=& _tmpBBC,((
_tmpBBD[1]=& _tmpBBB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBBE="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBBE,
sizeof(char),41))),_tag_dyneither(_tmpBBD,sizeof(void*),2)))))))))))));}_LL26E:;}
break;}case Cyc_Absyn_Plus: _LL26D:{void*_tmp542=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp528))->hd);struct Cyc_Absyn_PtrInfo _tmp544;
void*_tmp545;struct Cyc_Absyn_PtrAtts _tmp546;union Cyc_Absyn_Constraint*_tmp547;
union Cyc_Absyn_Constraint*_tmp548;_LL27C: {struct Cyc_Absyn_PointerType_struct*
_tmp543=(struct Cyc_Absyn_PointerType_struct*)_tmp542;if(_tmp543->tag != 5)goto
_LL27E;else{_tmp544=_tmp543->f1;_tmp545=_tmp544.elt_typ;_tmp546=_tmp544.ptr_atts;
_tmp547=_tmp546.bounds;_tmp548=_tmp546.zero_term;}}_LL27D:{void*_tmp549=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp547);struct Cyc_Absyn_Exp*_tmp54C;_LL281: {struct Cyc_Absyn_DynEither_b_struct*
_tmp54A=(struct Cyc_Absyn_DynEither_b_struct*)_tmp549;if(_tmp54A->tag != 0)goto
_LL283;}_LL282: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp49F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp49F->tl))->hd;{struct Cyc_Absyn_Exp*_tmpBBF[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBBF[2]=e2,((_tmpBBF[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp545),0),((_tmpBBF[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBF,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL280;}_LL283: {struct Cyc_Absyn_Upper_b_struct*_tmp54B=(struct
Cyc_Absyn_Upper_b_struct*)_tmp549;if(_tmp54B->tag != 1)goto _LL280;else{_tmp54C=
_tmp54B->f1;}}_LL284: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp548)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp49F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp49F->tl))->hd;struct Cyc_Absyn_Exp*_tmpBC0[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
e1),((_tmpBC0[2]=e2,((_tmpBC0[1]=_tmp54C,((_tmpBC0[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC0,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL280;_LL280:;}goto _LL27B;_LL27E:;_LL27F: goto _LL27B;
_LL27B:;}break;case Cyc_Absyn_Minus: _LL27A: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp528))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp49F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp49F->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp528->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpBC1[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBC1[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpBC1[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpBC1[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBC1,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL285: goto _LL286;case Cyc_Absyn_Neq: _LL286: goto _LL287;case Cyc_Absyn_Gt: _LL287:
goto _LL288;case Cyc_Absyn_Gte: _LL288: goto _LL289;case Cyc_Absyn_Lt: _LL289: goto
_LL28A;case Cyc_Absyn_Lte: _LL28A: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp49F))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp49F->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp528))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp528->tl))->hd;void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& elt_typ))e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,
0));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp,0));break;}default: _LL28B: break;}goto _LL210;}_LL21B: {
struct Cyc_Absyn_Increment_e_struct*_tmp4A0=(struct Cyc_Absyn_Increment_e_struct*)
_tmp493;if(_tmp4A0->tag != 5)goto _LL21D;else{_tmp4A1=_tmp4A0->f1;_tmp4A2=_tmp4A0->f2;}}
_LL21C: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4A1->topt))->v;
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;const char*_tmpBC2;struct _dyneither_ptr incr_str=(_tmpBC2="increment",
_tag_dyneither(_tmpBC2,sizeof(char),10));if(_tmp4A2 == Cyc_Absyn_PreDec  || 
_tmp4A2 == Cyc_Absyn_PostDec){const char*_tmpBC3;incr_str=((_tmpBC3="decrement",
_tag_dyneither(_tmpBC3,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4A1,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpBC7;void*_tmpBC6[1];
struct Cyc_String_pa_struct _tmpBC5;(_tmpBC5.tag=0,((_tmpBC5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBC6[0]=& _tmpBC5,Cyc_Tcutil_terr(
e->loc,((_tmpBC7="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpBC7,sizeof(char),74))),_tag_dyneither(_tmpBC6,sizeof(void*),1)))))));}{
const char*_tmpBCA;void*_tmpBC9;(_tmpBC9=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCA="in-place inc/dec on zero-term",
_tag_dyneither(_tmpBCA,sizeof(char),30))),_tag_dyneither(_tmpBC9,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp4A1,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp556=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpBCB;_tmp556->topt=((_tmpBCB=_cycalloc(sizeof(*
_tmpBCB)),((_tmpBCB->v=Cyc_Absyn_sint_typ,_tmpBCB))));}switch(_tmp4A2){case Cyc_Absyn_PreInc:
_LL28D:{struct Cyc_Absyn_AssignOp_e_struct _tmpBD1;struct Cyc_Core_Opt*_tmpBD0;
struct Cyc_Absyn_AssignOp_e_struct*_tmpBCF;e->r=(void*)((_tmpBCF=_cycalloc(
sizeof(*_tmpBCF)),((_tmpBCF[0]=((_tmpBD1.tag=4,((_tmpBD1.f1=_tmp4A1,((_tmpBD1.f2=((
_tmpBD0=_cycalloc_atomic(sizeof(*_tmpBD0)),((_tmpBD0->v=(void*)Cyc_Absyn_Plus,
_tmpBD0)))),((_tmpBD1.f3=_tmp556,_tmpBD1)))))))),_tmpBCF))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL28E:{struct Cyc_Absyn_AssignOp_e_struct
_tmpBD7;struct Cyc_Core_Opt*_tmpBD6;struct Cyc_Absyn_AssignOp_e_struct*_tmpBD5;e->r=(
void*)((_tmpBD5=_cycalloc(sizeof(*_tmpBD5)),((_tmpBD5[0]=((_tmpBD7.tag=4,((
_tmpBD7.f1=_tmp4A1,((_tmpBD7.f2=((_tmpBD6=_cycalloc_atomic(sizeof(*_tmpBD6)),((
_tmpBD6->v=(void*)Cyc_Absyn_Minus,_tmpBD6)))),((_tmpBD7.f3=_tmp556,_tmpBD7)))))))),
_tmpBD5))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL28F:{const char*_tmpBDB;
void*_tmpBDA[1];struct Cyc_String_pa_struct _tmpBD9;(_tmpBD9.tag=0,((_tmpBD9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBDA[0]=& _tmpBD9,Cyc_Tcutil_terr(
e->loc,((_tmpBDB="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpBDB,sizeof(char),59))),_tag_dyneither(_tmpBDA,sizeof(void*),1)))))));}{
const char*_tmpBDE;void*_tmpBDD;(_tmpBDD=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDE="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpBDE,sizeof(char),34))),_tag_dyneither(_tmpBDD,sizeof(void*),0)));};}}
Cyc_Toc_exp_to_c(nv,_tmp4A1);{void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp4A2 == Cyc_Absyn_PostInc
 || _tmp4A2 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp4A2 == Cyc_Absyn_PreDec  || _tmp4A2 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpBDF[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBDF[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpBDF[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpBDF[0]=Cyc_Toc_push_address_exp(_tmp4A1),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDF,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Toc_functionSet*_tmp564=_tmp4A2 == Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:&
Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(
_tmp564,_tmp4A1);struct Cyc_Absyn_Exp*_tmpBE0[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((
_tmpBE0[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpBE0[0]=Cyc_Toc_push_address_exp(
_tmp4A1),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBE0,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4A1)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp4A1,0,Cyc_Toc_incr_lvalue,_tmp4A2);e->r=_tmp4A1->r;}}}goto _LL210;};};}_LL21D: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp4A3=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp493;if(_tmp4A3->tag != 4)goto _LL21F;else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A3->f2;
_tmp4A6=_tmp4A3->f3;}}_LL21E: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4A4->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4A6->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp4A4,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_exp_to_c(nv,_tmp4A4);Cyc_Toc_exp_to_c(nv,
_tmp4A6);e->r=Cyc_Toc_tagged_union_assignop(_tmp4A4,e1_old_typ,_tmp4A5,_tmp4A6,
e2_old_typ,f_tag,tagged_member_struct_type);return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp4A4,& ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,
_tmp4A4,_tmp4A5,_tmp4A6,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp4A4);Cyc_Toc_exp_to_c(nv,_tmp4A4);Cyc_Toc_exp_to_c(nv,_tmp4A6);{int done=0;
if(_tmp4A5 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;switch((enum Cyc_Absyn_Primop)
_tmp4A5->v){case Cyc_Absyn_Plus: _LL291: change=_tmp4A6;break;case Cyc_Absyn_Minus:
_LL292: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4A6,0);break;default:
_LL293: {const char*_tmpBE3;void*_tmpBE2;(_tmpBE2=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE3="bad t ? pointer arithmetic",
_tag_dyneither(_tmpBE3,sizeof(char),27))),_tag_dyneither(_tmpBE2,sizeof(void*),0)));}}
done=1;{struct Cyc_Absyn_Exp*_tmp569=Cyc_Toc__dyneither_ptr_inplace_plus_e;struct
Cyc_Absyn_Exp*_tmpBE4[3];e->r=Cyc_Toc_fncall_exp_r(_tmp569,((_tmpBE4[2]=change,((
_tmpBE4[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBE4[0]=Cyc_Toc_push_address_exp(
_tmp4A4),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBE4,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ))switch((enum Cyc_Absyn_Primop)_tmp4A5->v){case Cyc_Absyn_Plus:
_LL295: done=1;{struct Cyc_Absyn_Exp*_tmpBE5[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4A4),((_tmpBE5[1]=_tmp4A6,((
_tmpBE5[0]=_tmp4A4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBE5,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL296: {const char*_tmpBE8;void*_tmpBE7;(_tmpBE7=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE8="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpBE8,sizeof(char),39))),_tag_dyneither(_tmpBE7,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp4A6->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp4A6->r,0));if(!Cyc_Absyn_is_lvalue(_tmp4A4)){{struct _tuple18 _tmpBEB;struct
_tuple18*_tmpBEA;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp4A4,0,Cyc_Toc_assignop_lvalue,((_tmpBEA=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpBEA[0]=((_tmpBEB.f1=_tmp4A5,((_tmpBEB.f2=
_tmp4A6,_tmpBEB)))),_tmpBEA)))));}e->r=_tmp4A4->r;}}goto _LL210;};};};}_LL21F: {
struct Cyc_Absyn_Conditional_e_struct*_tmp4A7=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp493;if(_tmp4A7->tag != 6)goto _LL221;else{_tmp4A8=_tmp4A7->f1;_tmp4A9=_tmp4A7->f2;
_tmp4AA=_tmp4A7->f3;}}_LL220: Cyc_Toc_exp_to_c(nv,_tmp4A8);Cyc_Toc_exp_to_c(nv,
_tmp4A9);Cyc_Toc_exp_to_c(nv,_tmp4AA);goto _LL210;_LL221: {struct Cyc_Absyn_And_e_struct*
_tmp4AB=(struct Cyc_Absyn_And_e_struct*)_tmp493;if(_tmp4AB->tag != 7)goto _LL223;
else{_tmp4AC=_tmp4AB->f1;_tmp4AD=_tmp4AB->f2;}}_LL222: Cyc_Toc_exp_to_c(nv,
_tmp4AC);Cyc_Toc_exp_to_c(nv,_tmp4AD);goto _LL210;_LL223: {struct Cyc_Absyn_Or_e_struct*
_tmp4AE=(struct Cyc_Absyn_Or_e_struct*)_tmp493;if(_tmp4AE->tag != 8)goto _LL225;
else{_tmp4AF=_tmp4AE->f1;_tmp4B0=_tmp4AE->f2;}}_LL224: Cyc_Toc_exp_to_c(nv,
_tmp4AF);Cyc_Toc_exp_to_c(nv,_tmp4B0);goto _LL210;_LL225: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp4B1=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp493;if(_tmp4B1->tag != 9)goto _LL227;
else{_tmp4B2=_tmp4B1->f1;_tmp4B3=_tmp4B1->f2;}}_LL226: Cyc_Toc_exp_to_c(nv,
_tmp4B2);Cyc_Toc_exp_to_c(nv,_tmp4B3);goto _LL210;_LL227: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp4B4=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp493;if(_tmp4B4->tag != 10)goto
_LL229;else{_tmp4B5=_tmp4B4->f1;_tmp4B6=_tmp4B4->f2;}}_LL228: _tmp4B8=_tmp4B5;
_tmp4B9=_tmp4B6;goto _LL22A;_LL229: {struct Cyc_Absyn_FnCall_e_struct*_tmp4B7=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp493;if(_tmp4B7->tag != 11)goto _LL22B;else{
_tmp4B8=_tmp4B7->f1;_tmp4B9=_tmp4B7->f2;_tmp4BA=_tmp4B7->f3;if(_tmp4BA != 0)goto
_LL22B;}}_LL22A: Cyc_Toc_exp_to_c(nv,_tmp4B8);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4B9);goto _LL210;_LL22B: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4BB=(struct Cyc_Absyn_FnCall_e_struct*)_tmp493;if(_tmp4BB->tag != 11)goto
_LL22D;else{_tmp4BC=_tmp4BB->f1;_tmp4BD=_tmp4BB->f2;_tmp4BE=_tmp4BB->f3;if(
_tmp4BE == 0)goto _LL22D;_tmp4BF=*_tmp4BE;_tmp4C0=_tmp4BF.num_varargs;_tmp4C1=
_tmp4BF.injectors;_tmp4C2=_tmp4BF.vai;}}_LL22C: {struct Cyc_Toc_Env _tmp571;struct
_RegionHandle*_tmp572;struct Cyc_Toc_Env*_tmp570=nv;_tmp571=*_tmp570;_tmp572=
_tmp571.rgn;{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4C0,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp4C2->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4BD);int num_normargs=num_args - _tmp4C0;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4BD=_tmp4BD->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd);{
struct Cyc_List_List*_tmpBEC;new_args=((_tmpBEC=_cycalloc(sizeof(*_tmpBEC)),((
_tmpBEC->hd=(struct Cyc_Absyn_Exp*)_tmp4BD->hd,((_tmpBEC->tl=new_args,_tmpBEC))))));};}}{
struct Cyc_Absyn_Exp*_tmpBEF[3];struct Cyc_List_List*_tmpBEE;new_args=((_tmpBEE=
_cycalloc(sizeof(*_tmpBEE)),((_tmpBEE->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpBEF[2]=num_varargs_exp,((_tmpBEF[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpBEF[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBEF,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBEE->tl=
new_args,_tmpBEE))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4BC);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4BC,new_args,0),0);if(_tmp4C2->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp576=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4C2->type));struct Cyc_Absyn_DatatypeInfo _tmp578;union Cyc_Absyn_DatatypeInfoU
_tmp579;struct Cyc_Absyn_Datatypedecl**_tmp57A;struct Cyc_Absyn_Datatypedecl*
_tmp57B;_LL299: {struct Cyc_Absyn_DatatypeType_struct*_tmp577=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp576;if(_tmp577->tag != 3)goto _LL29B;else{_tmp578=_tmp577->f1;_tmp579=_tmp578.datatype_info;
if((_tmp579.KnownDatatype).tag != 2)goto _LL29B;_tmp57A=(struct Cyc_Absyn_Datatypedecl**)(
_tmp579.KnownDatatype).val;_tmp57B=*_tmp57A;}}_LL29A: tud=_tmp57B;goto _LL298;
_LL29B:;_LL29C: {const char*_tmpBF2;void*_tmpBF1;(_tmpBF1=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF2="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpBF2,sizeof(char),44))),_tag_dyneither(_tmpBF1,sizeof(void*),0)));}
_LL298:;}{unsigned int _tmpBF7;unsigned int _tmpBF6;struct _dyneither_ptr _tmpBF5;
struct _tuple0**_tmpBF4;unsigned int _tmpBF3;struct _dyneither_ptr vs=(_tmpBF3=(
unsigned int)_tmp4C0,((_tmpBF4=(struct _tuple0**)_region_malloc(_tmp572,
_check_times(sizeof(struct _tuple0*),_tmpBF3)),((_tmpBF5=_tag_dyneither(_tmpBF4,
sizeof(struct _tuple0*),_tmpBF3),((((_tmpBF6=_tmpBF3,_tmpBF8(& _tmpBF7,& _tmpBF6,&
_tmpBF4))),_tmpBF5)))))));if(_tmp4C0 != 0){struct Cyc_List_List*_tmp57E=0;{int i=
_tmp4C0 - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpBF9;_tmp57E=((_tmpBF9=
_cycalloc(sizeof(*_tmpBF9)),((_tmpBF9->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct
_tuple0*),i)),0),0)),((_tmpBF9->tl=_tmp57E,_tmpBF9))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp57E,0),s,0);{
int i=0;for(0;_tmp4BD != 0;(((_tmp4BD=_tmp4BD->tl,_tmp4C1=_tmp4C1->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4BD->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=*((struct
_tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp581;struct
_tuple0*_tmp582;struct Cyc_List_List*_tmp583;struct Cyc_Absyn_Datatypefield*
_tmp580=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp4C1))->hd;_tmp581=*_tmp580;_tmp582=_tmp581.name;_tmp583=_tmp581.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)_check_null(
_tmp583))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp582),0),s,0);{const char*_tmpBFA;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp582,((_tmpBFA="_struct",_tag_dyneither(_tmpBFA,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple15*_tmpBFB[3];struct Cyc_List_List*
_tmp585=(_tmpBFB[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBFB[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpBFB[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBFB,sizeof(struct _tuple15*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp4BD
!= 0;(_tmp4BD=_tmp4BD->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4BD->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4BD->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL210;}_LL22D: {struct Cyc_Absyn_Throw_e_struct*_tmp4C3=(struct Cyc_Absyn_Throw_e_struct*)
_tmp493;if(_tmp4C3->tag != 12)goto _LL22F;else{_tmp4C4=_tmp4C3->f1;}}_LL22E: Cyc_Toc_exp_to_c(
nv,_tmp4C4);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp4C4),0))->r;goto _LL210;_LL22F: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp4C5=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp493;if(_tmp4C5->tag != 13)
goto _LL231;else{_tmp4C6=_tmp4C5->f1;}}_LL230: Cyc_Toc_exp_to_c(nv,_tmp4C6);goto
_LL210;_LL231: {struct Cyc_Absyn_Instantiate_e_struct*_tmp4C7=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp493;if(_tmp4C7->tag != 14)goto _LL233;else{_tmp4C8=_tmp4C7->f1;_tmp4C9=_tmp4C7->f2;}}
_LL232: Cyc_Toc_exp_to_c(nv,_tmp4C8);for(0;_tmp4C9 != 0;_tmp4C9=_tmp4C9->tl){enum 
Cyc_Absyn_KindQual _tmp58B=(Cyc_Tcutil_typ_kind((void*)_tmp4C9->hd))->kind;if(
_tmp58B != Cyc_Absyn_EffKind  && _tmp58B != Cyc_Absyn_RgnKind){{void*_tmp58C=Cyc_Tcutil_compress((
void*)_tmp4C9->hd);_LL29E: {struct Cyc_Absyn_VarType_struct*_tmp58D=(struct Cyc_Absyn_VarType_struct*)
_tmp58C;if(_tmp58D->tag != 2)goto _LL2A0;}_LL29F: goto _LL2A1;_LL2A0: {struct Cyc_Absyn_DatatypeType_struct*
_tmp58E=(struct Cyc_Absyn_DatatypeType_struct*)_tmp58C;if(_tmp58E->tag != 3)goto
_LL2A2;}_LL2A1: continue;_LL2A2:;_LL2A3: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4C8,0))->r;goto _LL29D;
_LL29D:;}break;}}goto _LL210;_LL233: {struct Cyc_Absyn_Cast_e_struct*_tmp4CA=(
struct Cyc_Absyn_Cast_e_struct*)_tmp493;if(_tmp4CA->tag != 15)goto _LL235;else{
_tmp4CB=(void**)& _tmp4CA->f1;_tmp4CC=(void**)((void**)& _tmp4CA->f1);_tmp4CD=
_tmp4CA->f2;_tmp4CE=_tmp4CA->f3;_tmp4CF=_tmp4CA->f4;}}_LL234: {void*old_t2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4CD->topt))->v;void*new_typ=*_tmp4CC;*
_tmp4CC=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp4CD);{struct _tuple17
_tmpBFC;struct _tuple17 _tmp590=(_tmpBFC.f1=Cyc_Tcutil_compress(old_t2),((_tmpBFC.f2=
Cyc_Tcutil_compress(new_typ),_tmpBFC)));void*_tmp591;struct Cyc_Absyn_PtrInfo
_tmp593;void*_tmp594;struct Cyc_Absyn_PtrInfo _tmp596;void*_tmp597;struct Cyc_Absyn_PtrInfo
_tmp599;void*_tmp59A;_LL2A5: _tmp591=_tmp590.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp592=(struct Cyc_Absyn_PointerType_struct*)_tmp591;if(_tmp592->tag != 5)goto
_LL2A7;else{_tmp593=_tmp592->f1;}};_tmp594=_tmp590.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp595=(struct Cyc_Absyn_PointerType_struct*)_tmp594;if(_tmp595->tag != 5)goto
_LL2A7;else{_tmp596=_tmp595->f1;}};_LL2A6: {int _tmp59C=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp593.ptr_atts).nullable);int _tmp59D=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp596.ptr_atts).nullable);
void*_tmp59E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp593.ptr_atts).bounds);void*_tmp59F=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp596.ptr_atts).bounds);
int _tmp5A0=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp593.ptr_atts).zero_term);int _tmp5A1=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp596.ptr_atts).zero_term);{struct _tuple17 _tmpBFD;
struct _tuple17 _tmp5A3=(_tmpBFD.f1=_tmp59E,((_tmpBFD.f2=_tmp59F,_tmpBFD)));void*
_tmp5A4;struct Cyc_Absyn_Exp*_tmp5A6;void*_tmp5A7;struct Cyc_Absyn_Exp*_tmp5A9;
void*_tmp5AA;struct Cyc_Absyn_Exp*_tmp5AC;void*_tmp5AD;void*_tmp5AF;void*_tmp5B1;
struct Cyc_Absyn_Exp*_tmp5B3;void*_tmp5B4;void*_tmp5B6;_LL2AC: _tmp5A4=_tmp5A3.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp5A5=(struct Cyc_Absyn_Upper_b_struct*)_tmp5A4;
if(_tmp5A5->tag != 1)goto _LL2AE;else{_tmp5A6=_tmp5A5->f1;}};_tmp5A7=_tmp5A3.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5A8=(struct Cyc_Absyn_Upper_b_struct*)_tmp5A7;
if(_tmp5A8->tag != 1)goto _LL2AE;else{_tmp5A9=_tmp5A8->f1;}};_LL2AD: if((!Cyc_Evexp_c_can_eval(
_tmp5A6) || !Cyc_Evexp_c_can_eval(_tmp5A9)) && !Cyc_Evexp_same_const_exp(_tmp5A6,
_tmp5A9)){const char*_tmpC00;void*_tmpBFF;(_tmpBFF=0,Cyc_Tcutil_terr(e->loc,((
_tmpC00="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC00,sizeof(char),54))),_tag_dyneither(_tmpBFF,sizeof(void*),0)));}if(_tmp59C
 && !_tmp59D){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC03;void*_tmpC02;(
_tmpC02=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC03="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC03,
sizeof(char),44))),_tag_dyneither(_tmpC02,sizeof(void*),0)));}if(_tmp4CF != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC07;void*_tmpC06[1];struct Cyc_String_pa_struct _tmpC05;(_tmpC05.tag=
0,((_tmpC05.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC06[0]=& _tmpC05,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC07="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC07,sizeof(char),41))),_tag_dyneither(_tmpC06,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4CD);if(do_null_check){if(!_tmp4CE){
const char*_tmpC0A;void*_tmpC09;(_tmpC09=0,Cyc_Tcutil_warn(e->loc,((_tmpC0A="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC0A,sizeof(char),58))),_tag_dyneither(_tmpC09,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC0B;e->r=Cyc_Toc_cast_it_r(*_tmp4CC,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC0B=_cycalloc(sizeof(*_tmpC0B)),((_tmpC0B->hd=
_tmp4CD,((_tmpC0B->tl=0,_tmpC0B)))))),0));};}};}goto _LL2AB;_LL2AE: _tmp5AA=
_tmp5A3.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp5AB=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5AA;if(_tmp5AB->tag != 1)goto _LL2B0;else{_tmp5AC=_tmp5AB->f1;}};_tmp5AD=
_tmp5A3.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5AE=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5AD;if(_tmp5AE->tag != 0)goto _LL2B0;};_LL2AF: if(!Cyc_Evexp_c_can_eval(_tmp5AC)){
const char*_tmpC0E;void*_tmpC0D;(_tmpC0D=0,Cyc_Tcutil_terr(e->loc,((_tmpC0E="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC0E,sizeof(char),71))),_tag_dyneither(_tmpC0D,sizeof(void*),0)));}
if(_tmp4CF == Cyc_Absyn_NonNull_to_Null){const char*_tmpC12;void*_tmpC11[1];struct
Cyc_String_pa_struct _tmpC10;(_tmpC10.tag=0,((_tmpC10.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC11[0]=& _tmpC10,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC12="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC12,sizeof(char),44))),_tag_dyneither(_tmpC11,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp5A0  && !(_tmp596.elt_tq).real_const) && !
_tmp5A1)_tmp5AC=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5AC,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5AC,_tmp4CD))->r;}else{
struct Cyc_Absyn_Exp*_tmp5C7=Cyc_Toc__tag_dyneither_e;if(_tmp5A0){struct _tuple0*
_tmp5C8=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5C9=Cyc_Absyn_var_exp(_tmp5C8,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp5CA=_tmp4CD->r;union Cyc_Absyn_Cnst _tmp5CC;
struct _dyneither_ptr _tmp5CD;_LL2B5: {struct Cyc_Absyn_Const_e_struct*_tmp5CB=(
struct Cyc_Absyn_Const_e_struct*)_tmp5CA;if(_tmp5CB->tag != 0)goto _LL2B7;else{
_tmp5CC=_tmp5CB->f1;if((_tmp5CC.String_c).tag != 7)goto _LL2B7;_tmp5CD=(struct
_dyneither_ptr)(_tmp5CC.String_c).val;}}_LL2B6: arg3=_tmp5AC;goto _LL2B4;_LL2B7:;
_LL2B8:{struct Cyc_Absyn_Exp*_tmpC13[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp4CD),((_tmpC13[1]=_tmp5AC,((_tmpC13[0]=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5C9),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC13,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2B4;_LL2B4:;}if(!_tmp5A1  && !(_tmp596.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp5CF=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp596.elt_typ),0);struct Cyc_Absyn_Exp*
_tmpC14[3];struct Cyc_Absyn_Exp*_tmp5D0=Cyc_Absyn_fncall_exp(_tmp5C7,((_tmpC14[2]=
arg3,((_tmpC14[1]=_tmp5CF,((_tmpC14[0]=_tmp5C9,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC14,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp5D1=Cyc_Absyn_exp_stmt(_tmp5D0,0);_tmp5D1=
Cyc_Absyn_declare_stmt(_tmp5C8,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp4CD,_tmp5D1,0);e->r=Cyc_Toc_stmt_exp_r(_tmp5D1);};}else{struct Cyc_Absyn_Exp*
_tmpC15[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5C7,((_tmpC15[2]=_tmp5AC,((_tmpC15[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp596.elt_typ),0),((_tmpC15[0]=
_tmp4CD,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC15,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2AB;
_LL2B0: _tmp5AF=_tmp5A3.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5B0=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5AF;if(_tmp5B0->tag != 0)goto _LL2B2;};_tmp5B1=_tmp5A3.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5B2=(struct Cyc_Absyn_Upper_b_struct*)_tmp5B1;if(_tmp5B2->tag != 1)goto _LL2B2;
else{_tmp5B3=_tmp5B2->f1;}};_LL2B1: if(!Cyc_Evexp_c_can_eval(_tmp5B3)){const char*
_tmpC18;void*_tmpC17;(_tmpC17=0,Cyc_Tcutil_terr(e->loc,((_tmpC18="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC18,sizeof(char),71))),_tag_dyneither(_tmpC17,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC1B;void*_tmpC1A;(_tmpC1A=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC1B="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC1B,sizeof(char),45))),_tag_dyneither(_tmpC1A,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5D8=_tmp5B3;if(_tmp5A0  && !_tmp5A1)_tmp5D8=Cyc_Absyn_add_exp(
_tmp5B3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5D9=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC1C[3];struct Cyc_Absyn_Exp*_tmp5DA=Cyc_Absyn_fncall_exp(
_tmp5D9,((_tmpC1C[2]=_tmp5D8,((_tmpC1C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp593.elt_typ),0),((_tmpC1C[0]=_tmp4CD,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1C,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp59D){struct Cyc_List_List*_tmpC1D;_tmp5DA->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC1D=_cycalloc(sizeof(*_tmpC1D)),((_tmpC1D->hd=Cyc_Absyn_copy_exp(
_tmp5DA),((_tmpC1D->tl=0,_tmpC1D)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp4CC,_tmp5DA);
goto _LL2AB;};};_LL2B2: _tmp5B4=_tmp5A3.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5B5=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5B4;if(_tmp5B5->tag != 0)goto
_LL2AB;};_tmp5B6=_tmp5A3.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5B7=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp5B6;if(_tmp5B7->tag != 0)goto _LL2AB;};_LL2B3:
DynCast: if((_tmp5A0  && !_tmp5A1) && !(_tmp596.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC20;void*_tmpC1F;(_tmpC1F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC20="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC20,sizeof(char),70))),_tag_dyneither(_tmpC1F,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5DF=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC21[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5DF,((_tmpC21[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC21[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp593.elt_typ),0),((
_tmpC21[0]=_tmp4CD,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC21,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2AB;
_LL2AB:;}goto _LL2A4;}_LL2A7: _tmp597=_tmp590.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp598=(struct Cyc_Absyn_PointerType_struct*)_tmp597;if(_tmp598->tag != 5)goto
_LL2A9;else{_tmp599=_tmp598->f1;}};_tmp59A=_tmp590.f2;{struct Cyc_Absyn_IntType_struct*
_tmp59B=(struct Cyc_Absyn_IntType_struct*)_tmp59A;if(_tmp59B->tag != 6)goto _LL2A9;};
_LL2A8:{void*_tmp5E1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp599.ptr_atts).bounds);_LL2BA: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5E2=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5E1;if(_tmp5E2->tag != 0)goto
_LL2BC;}_LL2BB: _tmp4CD->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4CD->r,
_tmp4CD->loc),Cyc_Toc_curr_sp);goto _LL2B9;_LL2BC:;_LL2BD: goto _LL2B9;_LL2B9:;}
goto _LL2A4;_LL2A9:;_LL2AA: goto _LL2A4;_LL2A4:;}goto _LL210;}_LL235: {struct Cyc_Absyn_Address_e_struct*
_tmp4D0=(struct Cyc_Absyn_Address_e_struct*)_tmp493;if(_tmp4D0->tag != 16)goto
_LL237;else{_tmp4D1=_tmp4D0->f1;}}_LL236:{void*_tmp5E3=_tmp4D1->r;struct _tuple0*
_tmp5E5;struct Cyc_List_List*_tmp5E6;struct Cyc_List_List*_tmp5E7;struct Cyc_List_List*
_tmp5E9;_LL2BF: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5E4=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp5E3;if(_tmp5E4->tag != 29)goto _LL2C1;else{_tmp5E5=_tmp5E4->f1;_tmp5E6=_tmp5E4->f2;
_tmp5E7=_tmp5E4->f3;}}_LL2C0: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC25;void*
_tmpC24[1];struct Cyc_String_pa_struct _tmpC23;(_tmpC23.tag=0,((_tmpC23.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4D1->loc)),((
_tmpC24[0]=& _tmpC23,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC25="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC25,
sizeof(char),42))),_tag_dyneither(_tmpC24,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D1->topt))->v,_tmp5E6 != 0,1,0,
_tmp5E7,_tmp5E5))->r;goto _LL2BE;_LL2C1: {struct Cyc_Absyn_Tuple_e_struct*_tmp5E8=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5E3;if(_tmp5E8->tag != 25)goto _LL2C3;else{
_tmp5E9=_tmp5E8->f1;}}_LL2C2: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC29;void*
_tmpC28[1];struct Cyc_String_pa_struct _tmpC27;(_tmpC27.tag=0,((_tmpC27.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4D1->loc)),((
_tmpC28[0]=& _tmpC27,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC29="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC29,
sizeof(char),42))),_tag_dyneither(_tmpC28,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp5E9))->r;goto _LL2BE;_LL2C3:;_LL2C4: Cyc_Toc_exp_to_c(nv,_tmp4D1);if(!
Cyc_Absyn_is_lvalue(_tmp4D1)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp4D1,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4D1);}goto _LL2BE;
_LL2BE:;}goto _LL210;_LL237: {struct Cyc_Absyn_New_e_struct*_tmp4D2=(struct Cyc_Absyn_New_e_struct*)
_tmp493;if(_tmp4D2->tag != 17)goto _LL239;else{_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D2->f2;}}
_LL238: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC2D;void*_tmpC2C[1];struct Cyc_String_pa_struct
_tmpC2B;(_tmpC2B.tag=0,((_tmpC2B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4D4->loc)),((_tmpC2C[0]=& _tmpC2B,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC2D="%s: new at top-level",
_tag_dyneither(_tmpC2D,sizeof(char),21))),_tag_dyneither(_tmpC2C,sizeof(void*),1)))))));}{
void*_tmp5F3=_tmp4D4->r;struct Cyc_List_List*_tmp5F5;struct Cyc_Absyn_Vardecl*
_tmp5F7;struct Cyc_Absyn_Exp*_tmp5F8;struct Cyc_Absyn_Exp*_tmp5F9;int _tmp5FA;
struct _tuple0*_tmp5FC;struct Cyc_List_List*_tmp5FD;struct Cyc_List_List*_tmp5FE;
struct Cyc_Absyn_Aggrdecl*_tmp5FF;struct Cyc_List_List*_tmp601;_LL2C6: {struct Cyc_Absyn_Array_e_struct*
_tmp5F4=(struct Cyc_Absyn_Array_e_struct*)_tmp5F3;if(_tmp5F4->tag != 27)goto _LL2C8;
else{_tmp5F5=_tmp5F4->f1;}}_LL2C7: {struct _tuple0*_tmp602=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp603=Cyc_Absyn_var_exp(_tmp602,0);struct Cyc_Absyn_Stmt*
_tmp604=Cyc_Toc_init_array(nv,_tmp603,_tmp5F5,Cyc_Absyn_exp_stmt(_tmp603,0));
void*old_elt_typ;{void*_tmp605=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp607;void*_tmp608;struct Cyc_Absyn_Tqual _tmp609;struct Cyc_Absyn_PtrAtts _tmp60A;
union Cyc_Absyn_Constraint*_tmp60B;_LL2D1: {struct Cyc_Absyn_PointerType_struct*
_tmp606=(struct Cyc_Absyn_PointerType_struct*)_tmp605;if(_tmp606->tag != 5)goto
_LL2D3;else{_tmp607=_tmp606->f1;_tmp608=_tmp607.elt_typ;_tmp609=_tmp607.elt_tq;
_tmp60A=_tmp607.ptr_atts;_tmp60B=_tmp60A.zero_term;}}_LL2D2: old_elt_typ=_tmp608;
goto _LL2D0;_LL2D3:;_LL2D4: {const char*_tmpC30;void*_tmpC2F;old_elt_typ=((_tmpC2F=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC30="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC30,sizeof(char),52))),_tag_dyneither(_tmpC2F,sizeof(void*),0))));}_LL2D0:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp60E=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp60F=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5F5),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4D3 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp60F);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4D3;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp60F);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp602,_tmp60E,(struct Cyc_Absyn_Exp*)e1,_tmp604,0));goto
_LL2C5;};}_LL2C8: {struct Cyc_Absyn_Comprehension_e_struct*_tmp5F6=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp5F3;if(_tmp5F6->tag != 28)goto _LL2CA;else{_tmp5F7=_tmp5F6->f1;_tmp5F8=_tmp5F6->f2;
_tmp5F9=_tmp5F6->f3;_tmp5FA=_tmp5F6->f4;}}_LL2C9: {int is_dyneither_ptr=0;{void*
_tmp610=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp612;void*_tmp613;
struct Cyc_Absyn_Tqual _tmp614;struct Cyc_Absyn_PtrAtts _tmp615;union Cyc_Absyn_Constraint*
_tmp616;union Cyc_Absyn_Constraint*_tmp617;_LL2D6: {struct Cyc_Absyn_PointerType_struct*
_tmp611=(struct Cyc_Absyn_PointerType_struct*)_tmp610;if(_tmp611->tag != 5)goto
_LL2D8;else{_tmp612=_tmp611->f1;_tmp613=_tmp612.elt_typ;_tmp614=_tmp612.elt_tq;
_tmp615=_tmp612.ptr_atts;_tmp616=_tmp615.bounds;_tmp617=_tmp615.zero_term;}}
_LL2D7: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp616)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2D5;
_LL2D8:;_LL2D9: {const char*_tmpC33;void*_tmpC32;(_tmpC32=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC33="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC33,sizeof(char),42))),_tag_dyneither(_tmpC32,sizeof(void*),0)));}
_LL2D5:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5F9->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp5F8);{struct Cyc_Absyn_Exp*_tmp61A=
Cyc_Absyn_var_exp(max,0);if(_tmp5FA)_tmp61A=Cyc_Absyn_add_exp(_tmp61A,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp5F7,Cyc_Absyn_var_exp(max,0),_tmp5F9,_tmp5FA,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp61C;struct _RegionHandle*_tmp61D;struct Cyc_Toc_Env*_tmp61B=
nv;_tmp61C=*_tmp61B;_tmp61D=_tmp61C.rgn;{struct _tuple19*_tmpC36;struct Cyc_List_List*
_tmpC35;struct Cyc_List_List*decls=(_tmpC35=_region_malloc(_tmp61D,sizeof(*
_tmpC35)),((_tmpC35->hd=((_tmpC36=_region_malloc(_tmp61D,sizeof(*_tmpC36)),((
_tmpC36->f1=max,((_tmpC36->f2=Cyc_Absyn_uint_typ,((_tmpC36->f3=(struct Cyc_Absyn_Exp*)
_tmp5F8,_tmpC36)))))))),((_tmpC35->tl=0,_tmpC35)))));struct Cyc_Absyn_Exp*ai;if(
_tmp4D3 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC37[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC37[1]=_tmp61A,((
_tmpC37[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC37,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4D3;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC38[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC38[1]=_tmp61A,((_tmpC38[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC38,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC3B;struct Cyc_List_List*
_tmpC3A;decls=((_tmpC3A=_region_malloc(_tmp61D,sizeof(*_tmpC3A)),((_tmpC3A->hd=((
_tmpC3B=_region_malloc(_tmp61D,sizeof(*_tmpC3B)),((_tmpC3B->f1=a,((_tmpC3B->f2=
ptr_typ,((_tmpC3B->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC3B)))))))),((_tmpC3A->tl=
decls,_tmpC3A))))));}if(is_dyneither_ptr){struct _tuple0*_tmp622=Cyc_Toc_temp_var();
void*_tmp623=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp624=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC3C[3];struct Cyc_Absyn_Exp*_tmp625=Cyc_Absyn_fncall_exp(
_tmp624,((_tmpC3C[2]=_tmp61A,((_tmpC3C[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC3C[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC3C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC3F;struct Cyc_List_List*_tmpC3E;decls=((_tmpC3E=
_region_malloc(_tmp61D,sizeof(*_tmpC3E)),((_tmpC3E->hd=((_tmpC3F=_region_malloc(
_tmp61D,sizeof(*_tmpC3F)),((_tmpC3F->f1=_tmp622,((_tmpC3F->f2=_tmp623,((_tmpC3F->f3=(
struct Cyc_Absyn_Exp*)_tmp625,_tmpC3F)))))))),((_tmpC3E->tl=decls,_tmpC3E))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp622,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp629=decls;for(0;_tmp629 != 0;_tmp629=_tmp629->tl){struct
_tuple0*_tmp62B;void*_tmp62C;struct Cyc_Absyn_Exp*_tmp62D;struct _tuple19 _tmp62A=*((
struct _tuple19*)_tmp629->hd);_tmp62B=_tmp62A.f1;_tmp62C=_tmp62A.f2;_tmp62D=
_tmp62A.f3;s=Cyc_Absyn_declare_stmt(_tmp62B,_tmp62C,_tmp62D,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2C5;};};};}_LL2CA: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5FB=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp5F3;if(_tmp5FB->tag != 29)goto _LL2CC;
else{_tmp5FC=_tmp5FB->f1;_tmp5FD=_tmp5FB->f2;_tmp5FE=_tmp5FB->f3;_tmp5FF=_tmp5FB->f4;}}
_LL2CB: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4D4->topt))->v,_tmp5FD != 0,1,_tmp4D3,_tmp5FE,_tmp5FC))->r;goto _LL2C5;_LL2CC: {
struct Cyc_Absyn_Tuple_e_struct*_tmp600=(struct Cyc_Absyn_Tuple_e_struct*)_tmp5F3;
if(_tmp600->tag != 25)goto _LL2CE;else{_tmp601=_tmp600->f1;}}_LL2CD: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp4D3,_tmp601))->r;goto _LL2C5;_LL2CE:;_LL2CF: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4D4->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple0*_tmp630=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp631=Cyc_Absyn_var_exp(_tmp630,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp631,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp4D3
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4D3;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp632=_tmp4D4->r;void*_tmp634;
struct Cyc_Absyn_Exp*_tmp635;_LL2DB: {struct Cyc_Absyn_Cast_e_struct*_tmp633=(
struct Cyc_Absyn_Cast_e_struct*)_tmp632;if(_tmp633->tag != 15)goto _LL2DD;else{
_tmp634=(void*)_tmp633->f1;_tmp635=_tmp633->f2;}}_LL2DC:{struct _tuple17 _tmpC40;
struct _tuple17 _tmp637=(_tmpC40.f1=Cyc_Tcutil_compress(_tmp634),((_tmpC40.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp635->topt))->v),_tmpC40)));void*
_tmp638;struct Cyc_Absyn_PtrInfo _tmp63A;void*_tmp63B;struct Cyc_Absyn_PtrAtts
_tmp63C;union Cyc_Absyn_Constraint*_tmp63D;void*_tmp63E;struct Cyc_Absyn_PtrInfo
_tmp640;struct Cyc_Absyn_PtrAtts _tmp641;union Cyc_Absyn_Constraint*_tmp642;_LL2E0:
_tmp638=_tmp637.f1;{struct Cyc_Absyn_PointerType_struct*_tmp639=(struct Cyc_Absyn_PointerType_struct*)
_tmp638;if(_tmp639->tag != 5)goto _LL2E2;else{_tmp63A=_tmp639->f1;_tmp63B=_tmp63A.elt_typ;
_tmp63C=_tmp63A.ptr_atts;_tmp63D=_tmp63C.bounds;}};_tmp63E=_tmp637.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp63F=(struct Cyc_Absyn_PointerType_struct*)_tmp63E;if(_tmp63F->tag != 5)goto
_LL2E2;else{_tmp640=_tmp63F->f1;_tmp641=_tmp640.ptr_atts;_tmp642=_tmp641.bounds;}};
_LL2E1:{struct _tuple17 _tmpC41;struct _tuple17 _tmp644=(_tmpC41.f1=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp63D),((_tmpC41.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp642),_tmpC41)));void*_tmp645;void*_tmp647;struct Cyc_Absyn_Exp*
_tmp649;_LL2E5: _tmp645=_tmp644.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp646=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp645;if(_tmp646->tag != 0)goto _LL2E7;};
_tmp647=_tmp644.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp648=(struct Cyc_Absyn_Upper_b_struct*)
_tmp647;if(_tmp648->tag != 1)goto _LL2E7;else{_tmp649=_tmp648->f1;}};_LL2E6: Cyc_Toc_exp_to_c(
nv,_tmp635);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp64A=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC42[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp64A,((_tmpC42[3]=_tmp649,((_tmpC42[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp63B),0),((_tmpC42[1]=_tmp635,((_tmpC42[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC42,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2E4;};_LL2E7:;_LL2E8: goto _LL2E4;_LL2E4:;}goto _LL2DF;_LL2E2:;
_LL2E3: goto _LL2DF;_LL2DF:;}goto _LL2DA;_LL2DD:;_LL2DE: goto _LL2DA;_LL2DA:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp64C=Cyc_Absyn_exp_stmt(_tmp631,0);struct Cyc_Absyn_Exp*
_tmp64D=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp4D4);_tmp64C=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp631,_tmp64D,0),_tmp4D4,0),
_tmp64C,0);{void*_tmp64E=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp630,_tmp64E,(struct Cyc_Absyn_Exp*)mexp,_tmp64C,0));};}
goto _LL2C5;};}_LL2C5:;}goto _LL210;_LL239: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp4D5=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp493;if(_tmp4D5->tag != 19)goto
_LL23B;else{_tmp4D6=_tmp4D5->f1;}}_LL23A: Cyc_Toc_exp_to_c(nv,_tmp4D6);goto _LL210;
_LL23B: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4D7=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp493;if(_tmp4D7->tag != 18)goto _LL23D;else{_tmp4D8=(void*)_tmp4D7->f1;}}_LL23C:{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpC45;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpC44;e->r=(void*)((_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44[0]=((_tmpC45.tag=
18,((_tmpC45.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4D8),_tmpC45)))),_tmpC44))));}
goto _LL210;_LL23D: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4D9=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp493;if(_tmp4D9->tag != 20)goto _LL23F;else{_tmp4DA=(void*)_tmp4D9->f1;_tmp4DB=(
void*)_tmp4D9->f2;{struct Cyc_Absyn_StructField_struct*_tmp4DC=(struct Cyc_Absyn_StructField_struct*)
_tmp4DB;if(_tmp4DC->tag != 0)goto _LL23F;else{_tmp4DD=_tmp4DC->f1;}};}}_LL23E:{
struct Cyc_Absyn_Offsetof_e_struct _tmpC4F;struct Cyc_Absyn_StructField_struct
_tmpC4E;struct Cyc_Absyn_StructField_struct*_tmpC4D;struct Cyc_Absyn_Offsetof_e_struct*
_tmpC4C;e->r=(void*)((_tmpC4C=_cycalloc(sizeof(*_tmpC4C)),((_tmpC4C[0]=((_tmpC4F.tag=
20,((_tmpC4F.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4DA),((_tmpC4F.f2=(void*)((
void*)((_tmpC4D=_cycalloc(sizeof(*_tmpC4D)),((_tmpC4D[0]=((_tmpC4E.tag=0,((
_tmpC4E.f1=_tmp4DD,_tmpC4E)))),_tmpC4D))))),_tmpC4F)))))),_tmpC4C))));}goto
_LL210;_LL23F: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4DE=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp493;if(_tmp4DE->tag != 20)goto _LL241;else{_tmp4DF=(void*)_tmp4DE->f1;_tmp4E0=(
void*)_tmp4DE->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp4E1=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp4E0;if(_tmp4E1->tag != 1)goto _LL241;else{_tmp4E2=_tmp4E1->f1;}};}}_LL240:{
void*_tmp655=Cyc_Tcutil_compress(_tmp4DF);struct Cyc_Absyn_AggrInfo _tmp657;union
Cyc_Absyn_AggrInfoU _tmp658;struct Cyc_List_List*_tmp65A;_LL2EA: {struct Cyc_Absyn_AggrType_struct*
_tmp656=(struct Cyc_Absyn_AggrType_struct*)_tmp655;if(_tmp656->tag != 12)goto
_LL2EC;else{_tmp657=_tmp656->f1;_tmp658=_tmp657.aggr_info;}}_LL2EB: {struct Cyc_Absyn_Aggrdecl*
_tmp65D=Cyc_Absyn_get_known_aggrdecl(_tmp658);if(_tmp65D->impl == 0){const char*
_tmpC52;void*_tmpC51;(_tmpC51=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC52="struct fields must be known",
_tag_dyneither(_tmpC52,sizeof(char),28))),_tag_dyneither(_tmpC51,sizeof(void*),0)));}
_tmp65A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp65D->impl))->fields;goto
_LL2ED;}_LL2EC: {struct Cyc_Absyn_AnonAggrType_struct*_tmp659=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp655;if(_tmp659->tag != 13)goto _LL2EE;else{_tmp65A=_tmp659->f2;}}_LL2ED: {
struct Cyc_Absyn_Aggrfield*_tmp660=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp65A,(int)_tmp4E2);{struct Cyc_Absyn_Offsetof_e_struct
_tmpC5C;struct Cyc_Absyn_StructField_struct _tmpC5B;struct Cyc_Absyn_StructField_struct*
_tmpC5A;struct Cyc_Absyn_Offsetof_e_struct*_tmpC59;e->r=(void*)((_tmpC59=
_cycalloc(sizeof(*_tmpC59)),((_tmpC59[0]=((_tmpC5C.tag=20,((_tmpC5C.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4DF),((_tmpC5C.f2=(void*)((void*)((_tmpC5A=_cycalloc(
sizeof(*_tmpC5A)),((_tmpC5A[0]=((_tmpC5B.tag=0,((_tmpC5B.f1=_tmp660->name,
_tmpC5B)))),_tmpC5A))))),_tmpC5C)))))),_tmpC59))));}goto _LL2E9;}_LL2EE: {struct
Cyc_Absyn_TupleType_struct*_tmp65B=(struct Cyc_Absyn_TupleType_struct*)_tmp655;
if(_tmp65B->tag != 11)goto _LL2F0;}_LL2EF:{struct Cyc_Absyn_Offsetof_e_struct
_tmpC66;struct Cyc_Absyn_StructField_struct _tmpC65;struct Cyc_Absyn_StructField_struct*
_tmpC64;struct Cyc_Absyn_Offsetof_e_struct*_tmpC63;e->r=(void*)((_tmpC63=
_cycalloc(sizeof(*_tmpC63)),((_tmpC63[0]=((_tmpC66.tag=20,((_tmpC66.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4DF),((_tmpC66.f2=(void*)((void*)((_tmpC64=_cycalloc(
sizeof(*_tmpC64)),((_tmpC64[0]=((_tmpC65.tag=0,((_tmpC65.f1=Cyc_Absyn_fieldname((
int)(_tmp4E2 + 1)),_tmpC65)))),_tmpC64))))),_tmpC66)))))),_tmpC63))));}goto _LL2E9;
_LL2F0: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp65C=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp655;if(_tmp65C->tag != 4)goto _LL2F2;}_LL2F1: if(_tmp4E2 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpC70;struct Cyc_Absyn_StructField_struct _tmpC6F;struct Cyc_Absyn_StructField_struct*
_tmpC6E;struct Cyc_Absyn_Offsetof_e_struct*_tmpC6D;e->r=(void*)((_tmpC6D=
_cycalloc(sizeof(*_tmpC6D)),((_tmpC6D[0]=((_tmpC70.tag=20,((_tmpC70.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4DF),((_tmpC70.f2=(void*)((void*)((_tmpC6E=_cycalloc(
sizeof(*_tmpC6E)),((_tmpC6E[0]=((_tmpC6F.tag=0,((_tmpC6F.f1=Cyc_Toc_tag_sp,
_tmpC6F)))),_tmpC6E))))),_tmpC70)))))),_tmpC6D))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpC7A;struct Cyc_Absyn_StructField_struct _tmpC79;struct Cyc_Absyn_StructField_struct*
_tmpC78;struct Cyc_Absyn_Offsetof_e_struct*_tmpC77;e->r=(void*)((_tmpC77=
_cycalloc(sizeof(*_tmpC77)),((_tmpC77[0]=((_tmpC7A.tag=20,((_tmpC7A.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4DF),((_tmpC7A.f2=(void*)((void*)((_tmpC78=_cycalloc(
sizeof(*_tmpC78)),((_tmpC78[0]=((_tmpC79.tag=0,((_tmpC79.f1=Cyc_Absyn_fieldname((
int)_tmp4E2),_tmpC79)))),_tmpC78))))),_tmpC7A)))))),_tmpC77))));}goto _LL2E9;
_LL2F2:;_LL2F3: {const char*_tmpC7D;void*_tmpC7C;(_tmpC7C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7D="impossible type for offsetof tuple index",
_tag_dyneither(_tmpC7D,sizeof(char),41))),_tag_dyneither(_tmpC7C,sizeof(void*),0)));}
_LL2E9:;}goto _LL210;_LL241: {struct Cyc_Absyn_Deref_e_struct*_tmp4E3=(struct Cyc_Absyn_Deref_e_struct*)
_tmp493;if(_tmp4E3->tag != 21)goto _LL243;else{_tmp4E4=_tmp4E3->f1;}}_LL242: {void*
_tmp673=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E4->topt))->v);{
void*_tmp674=_tmp673;struct Cyc_Absyn_PtrInfo _tmp676;void*_tmp677;struct Cyc_Absyn_Tqual
_tmp678;struct Cyc_Absyn_PtrAtts _tmp679;void*_tmp67A;union Cyc_Absyn_Constraint*
_tmp67B;union Cyc_Absyn_Constraint*_tmp67C;union Cyc_Absyn_Constraint*_tmp67D;
_LL2F5: {struct Cyc_Absyn_PointerType_struct*_tmp675=(struct Cyc_Absyn_PointerType_struct*)
_tmp674;if(_tmp675->tag != 5)goto _LL2F7;else{_tmp676=_tmp675->f1;_tmp677=_tmp676.elt_typ;
_tmp678=_tmp676.elt_tq;_tmp679=_tmp676.ptr_atts;_tmp67A=_tmp679.rgn;_tmp67B=
_tmp679.nullable;_tmp67C=_tmp679.bounds;_tmp67D=_tmp679.zero_term;}}_LL2F6:{void*
_tmp67E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp67C);struct Cyc_Absyn_Exp*_tmp680;_LL2FA: {struct Cyc_Absyn_Upper_b_struct*
_tmp67F=(struct Cyc_Absyn_Upper_b_struct*)_tmp67E;if(_tmp67F->tag != 1)goto _LL2FC;
else{_tmp680=_tmp67F->f1;}}_LL2FB: {int do_null_check=Cyc_Toc_need_null_check(
_tmp4E4);Cyc_Toc_exp_to_c(nv,_tmp4E4);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC80;void*_tmpC7F;(_tmpC7F=0,Cyc_Tcutil_warn(e->loc,((_tmpC80="inserted null check due to dereference",
_tag_dyneither(_tmpC80,sizeof(char),39))),_tag_dyneither(_tmpC7F,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC81;_tmp4E4->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp673),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpC81=_cycalloc(sizeof(*
_tmpC81)),((_tmpC81->hd=Cyc_Absyn_copy_exp(_tmp4E4),((_tmpC81->tl=0,_tmpC81)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp67D)){unsigned int _tmp686;int _tmp687;struct _tuple10 _tmp685=Cyc_Evexp_eval_const_uint_exp(
_tmp680);_tmp686=_tmp685.f1;_tmp687=_tmp685.f2;if(!_tmp687  || _tmp686 <= 0){const
char*_tmpC84;void*_tmpC83;(_tmpC83=0,Cyc_Tcutil_terr(e->loc,((_tmpC84="cannot determine dereference is in bounds",
_tag_dyneither(_tmpC84,sizeof(char),42))),_tag_dyneither(_tmpC83,sizeof(void*),0)));}}
goto _LL2F9;}_LL2FC: {struct Cyc_Absyn_DynEither_b_struct*_tmp681=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp67E;if(_tmp681->tag != 0)goto _LL2F9;}_LL2FD: {struct Cyc_Absyn_Exp*_tmp68A=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpC85;_tmp68A->topt=((_tmpC85=_cycalloc(sizeof(*
_tmpC85)),((_tmpC85->v=Cyc_Absyn_uint_typ,_tmpC85))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp4E4,_tmp68A);Cyc_Toc_exp_to_c(nv,e);goto _LL2F9;}_LL2F9:;}goto _LL2F4;_LL2F7:;
_LL2F8: {const char*_tmpC88;void*_tmpC87;(_tmpC87=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC88="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpC88,sizeof(char),29))),_tag_dyneither(_tmpC87,sizeof(void*),0)));}
_LL2F4:;}goto _LL210;}_LL243: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4E5=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp493;if(_tmp4E5->tag != 22)goto _LL245;
else{_tmp4E6=_tmp4E5->f1;_tmp4E7=_tmp4E5->f2;_tmp4E8=_tmp4E5->f3;_tmp4E9=_tmp4E5->f4;}}
_LL244: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4E6->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp4E6);if(
_tmp4E8  && _tmp4E9)e->r=Cyc_Toc_check_tagged_union(_tmp4E6,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp4E7,Cyc_Absyn_aggrmember_exp);if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL210;}_LL245: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp4EA=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp493;if(_tmp4EA->tag != 23)goto _LL247;
else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;_tmp4ED=_tmp4EA->f3;_tmp4EE=_tmp4EA->f4;}}
_LL246: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4EB->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp4EB);Cyc_Toc_exp_to_c(
nv,_tmp4EB);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp68F;struct Cyc_Absyn_Tqual
_tmp690;struct Cyc_Absyn_PtrAtts _tmp691;void*_tmp692;union Cyc_Absyn_Constraint*
_tmp693;union Cyc_Absyn_Constraint*_tmp694;union Cyc_Absyn_Constraint*_tmp695;
struct Cyc_Absyn_PtrInfo _tmp68E=Cyc_Toc_get_ptr_type(e1typ);_tmp68F=_tmp68E.elt_typ;
_tmp690=_tmp68E.elt_tq;_tmp691=_tmp68E.ptr_atts;_tmp692=_tmp691.rgn;_tmp693=
_tmp691.nullable;_tmp694=_tmp691.bounds;_tmp695=_tmp691.zero_term;{void*_tmp696=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp694);struct Cyc_Absyn_Exp*_tmp698;_LL2FF: {struct Cyc_Absyn_Upper_b_struct*
_tmp697=(struct Cyc_Absyn_Upper_b_struct*)_tmp696;if(_tmp697->tag != 1)goto _LL301;
else{_tmp698=_tmp697->f1;}}_LL300: {unsigned int _tmp69B;int _tmp69C;struct
_tuple10 _tmp69A=Cyc_Evexp_eval_const_uint_exp(_tmp698);_tmp69B=_tmp69A.f1;
_tmp69C=_tmp69A.f2;if(_tmp69C){if(_tmp69B < 1){const char*_tmpC8B;void*_tmpC8A;(
_tmpC8A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC8B="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC8B,
sizeof(char),44))),_tag_dyneither(_tmpC8A,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpC8E;void*_tmpC8D;(_tmpC8D=0,Cyc_Tcutil_warn(
e->loc,((_tmpC8E="inserted null check due to dereference",_tag_dyneither(_tmpC8E,
sizeof(char),39))),_tag_dyneither(_tmpC8D,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpC8F[1];_tmp4EB->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC8F[0]=Cyc_Absyn_new_exp(_tmp4EB->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC8F,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp698)){const char*_tmpC92;void*
_tmpC91;(_tmpC91=0,Cyc_Tcutil_terr(e->loc,((_tmpC92="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpC92,sizeof(char),47))),_tag_dyneither(_tmpC91,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpC93[4];_tmp4EB->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpC93[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpC93[2]=Cyc_Absyn_sizeoftyp_exp(_tmp68F,0),((
_tmpC93[1]=_tmp698,((_tmpC93[0]=Cyc_Absyn_new_exp(_tmp4EB->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC93,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL2FE;}_LL301: {struct Cyc_Absyn_DynEither_b_struct*
_tmp699=(struct Cyc_Absyn_DynEither_b_struct*)_tmp696;if(_tmp699->tag != 0)goto
_LL2FE;}_LL302: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp68F);{struct Cyc_Absyn_Exp*
_tmpC94[3];_tmp4EB->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp690),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpC94[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpC94[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC94[0]=Cyc_Absyn_new_exp(_tmp4EB->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC94,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL2FE;}_LL2FE:;}if(
_tmp4ED  && _tmp4EE)e->r=Cyc_Toc_check_tagged_union(_tmp4EB,Cyc_Toc_typ_to_c(
e1typ),_tmp68F,_tmp4EC,Cyc_Absyn_aggrarrow_exp);if(is_poly  && _tmp4EE)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL210;};}_LL247: {struct Cyc_Absyn_Subscript_e_struct*_tmp4EF=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp493;if(_tmp4EF->tag != 24)goto _LL249;
else{_tmp4F0=_tmp4EF->f1;_tmp4F1=_tmp4EF->f2;}}_LL248: {void*_tmp6A6=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4F0->topt))->v);{void*_tmp6A7=
_tmp6A6;struct Cyc_List_List*_tmp6A9;struct Cyc_Absyn_PtrInfo _tmp6AB;void*_tmp6AC;
struct Cyc_Absyn_Tqual _tmp6AD;struct Cyc_Absyn_PtrAtts _tmp6AE;void*_tmp6AF;union
Cyc_Absyn_Constraint*_tmp6B0;union Cyc_Absyn_Constraint*_tmp6B1;union Cyc_Absyn_Constraint*
_tmp6B2;_LL304: {struct Cyc_Absyn_TupleType_struct*_tmp6A8=(struct Cyc_Absyn_TupleType_struct*)
_tmp6A7;if(_tmp6A8->tag != 11)goto _LL306;else{_tmp6A9=_tmp6A8->f1;}}_LL305: Cyc_Toc_exp_to_c(
nv,_tmp4F0);Cyc_Toc_exp_to_c(nv,_tmp4F1);{unsigned int _tmp6B4;int _tmp6B5;struct
_tuple10 _tmp6B3=Cyc_Evexp_eval_const_uint_exp(_tmp4F1);_tmp6B4=_tmp6B3.f1;
_tmp6B5=_tmp6B3.f2;if(!_tmp6B5){const char*_tmpC97;void*_tmpC96;(_tmpC96=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC97="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpC97,sizeof(char),44))),_tag_dyneither(_tmpC96,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4F0,Cyc_Absyn_fieldname((int)(_tmp6B4 + 1)));
goto _LL303;};_LL306: {struct Cyc_Absyn_PointerType_struct*_tmp6AA=(struct Cyc_Absyn_PointerType_struct*)
_tmp6A7;if(_tmp6AA->tag != 5)goto _LL308;else{_tmp6AB=_tmp6AA->f1;_tmp6AC=_tmp6AB.elt_typ;
_tmp6AD=_tmp6AB.elt_tq;_tmp6AE=_tmp6AB.ptr_atts;_tmp6AF=_tmp6AE.rgn;_tmp6B0=
_tmp6AE.nullable;_tmp6B1=_tmp6AE.bounds;_tmp6B2=_tmp6AE.zero_term;}}_LL307: {
struct Cyc_List_List*_tmp6B8=Cyc_Toc_get_relns(_tmp4F0);int in_bnds=0;{void*
_tmp6B9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6B1);_LL30B:;_LL30C: in_bnds=Cyc_Toc_check_bounds(_tmp6A6,_tmp6B8,_tmp4F0,
_tmp4F1);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpC9B;void*
_tmpC9A[1];struct Cyc_String_pa_struct _tmpC99;(_tmpC99.tag=0,((_tmpC99.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC9A[0]=&
_tmpC99,Cyc_Tcutil_warn(e->loc,((_tmpC9B="bounds check necessary for %s",
_tag_dyneither(_tmpC9B,sizeof(char),30))),_tag_dyneither(_tmpC9A,sizeof(void*),1)))))));}
_LL30A:;}Cyc_Toc_exp_to_c(nv,_tmp4F0);Cyc_Toc_exp_to_c(nv,_tmp4F1);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6BD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6B1);struct Cyc_Absyn_Exp*_tmp6BF;_LL30E: {struct Cyc_Absyn_Upper_b_struct*
_tmp6BE=(struct Cyc_Absyn_Upper_b_struct*)_tmp6BD;if(_tmp6BE->tag != 1)goto _LL310;
else{_tmp6BF=_tmp6BE->f1;}}_LL30F: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6B0);void*ta1=Cyc_Toc_typ_to_c(_tmp6AC);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6AD);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6B2)){if(!Cyc_Evexp_c_can_eval(_tmp6BF)){const char*_tmpC9E;void*_tmpC9D;(
_tmpC9D=0,Cyc_Tcutil_terr(e->loc,((_tmpC9E="cannot determine subscript is in bounds",
_tag_dyneither(_tmpC9E,sizeof(char),40))),_tag_dyneither(_tmpC9D,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp4F0);struct Cyc_Absyn_Exp*_tmpC9F[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpC9F[2]=_tmp4F1,((_tmpC9F[1]=_tmp6BF,((
_tmpC9F[0]=_tmp4F0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC9F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6BF)){const char*_tmpCA2;void*_tmpCA1;(
_tmpCA1=0,Cyc_Tcutil_terr(e->loc,((_tmpCA2="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCA2,sizeof(char),40))),_tag_dyneither(_tmpCA1,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCA5;void*_tmpCA4;(_tmpCA4=0,Cyc_Tcutil_warn(
e->loc,((_tmpCA5="inserted null check due to dereference",_tag_dyneither(_tmpCA5,
sizeof(char),39))),_tag_dyneither(_tmpCA4,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCA6[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCA6[3]=_tmp4F1,((_tmpCA6[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCA6[1]=
_tmp6BF,((_tmpCA6[0]=_tmp4F0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCA6,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp6BF)){const char*_tmpCA9;void*_tmpCA8;(_tmpCA8=0,Cyc_Tcutil_terr(
e->loc,((_tmpCA9="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCA9,sizeof(char),40))),_tag_dyneither(_tmpCA8,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCAA[2];_tmp4F1->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCAA[1]=Cyc_Absyn_copy_exp(_tmp4F1),((_tmpCAA[0]=_tmp6BF,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAA,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL30D;}_LL310: {struct Cyc_Absyn_DynEither_b_struct*_tmp6C0=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6BD;if(_tmp6C0->tag != 0)goto _LL30D;}
_LL311: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6AC);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6AD),
Cyc_Absyn_aggrmember_exp(_tmp4F0,Cyc_Toc_curr_sp,0)),_tmp4F1);}else{struct Cyc_Absyn_Exp*
_tmp6CC=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCAB[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6AD),Cyc_Absyn_fncall_exp(
_tmp6CC,((_tmpCAB[2]=_tmp4F1,((_tmpCAB[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCAB[0]=_tmp4F0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCAB,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL30D;}
_LL30D:;}goto _LL303;}_LL308:;_LL309: {const char*_tmpCAE;void*_tmpCAD;(_tmpCAD=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCAE="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCAE,sizeof(char),49))),_tag_dyneither(_tmpCAD,sizeof(void*),0)));}_LL303:;}
goto _LL210;}_LL249: {struct Cyc_Absyn_Tuple_e_struct*_tmp4F2=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp493;if(_tmp4F2->tag != 25)goto _LL24B;else{_tmp4F3=_tmp4F2->f1;}}_LL24A: if(!
Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4F3))->r;else{struct
Cyc_List_List*_tmp6D0=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4F3);void*_tmp6D1=Cyc_Toc_add_tuple_type(
_tmp6D0);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp4F3 != 0;(_tmp4F3=_tmp4F3->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F3->hd);{struct _tuple15*
_tmpCB1;struct Cyc_List_List*_tmpCB0;dles=((_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((
_tmpCB0->hd=((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1->f1=0,((_tmpCB1->f2=(
struct Cyc_Absyn_Exp*)_tmp4F3->hd,_tmpCB1)))))),((_tmpCB0->tl=dles,_tmpCB0))))));};}}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=
Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL210;_LL24B: {struct Cyc_Absyn_Array_e_struct*
_tmp4F4=(struct Cyc_Absyn_Array_e_struct*)_tmp493;if(_tmp4F4->tag != 27)goto _LL24D;
else{_tmp4F5=_tmp4F4->f1;}}_LL24C: e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4F5);{
struct Cyc_List_List*_tmp6D4=_tmp4F5;for(0;_tmp6D4 != 0;_tmp6D4=_tmp6D4->tl){
struct _tuple15 _tmp6D6;struct Cyc_Absyn_Exp*_tmp6D7;struct _tuple15*_tmp6D5=(struct
_tuple15*)_tmp6D4->hd;_tmp6D6=*_tmp6D5;_tmp6D7=_tmp6D6.f2;Cyc_Toc_exp_to_c(nv,
_tmp6D7);}}goto _LL210;_LL24D: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4F6=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp493;if(_tmp4F6->tag != 28)goto _LL24F;
else{_tmp4F7=_tmp4F6->f1;_tmp4F8=_tmp4F6->f2;_tmp4F9=_tmp4F6->f3;_tmp4FA=_tmp4F6->f4;}}
_LL24E: {unsigned int _tmp6D9;int _tmp6DA;struct _tuple10 _tmp6D8=Cyc_Evexp_eval_const_uint_exp(
_tmp4F8);_tmp6D9=_tmp6D8.f1;_tmp6DA=_tmp6D8.f2;{void*_tmp6DB=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4F9->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp4F9);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp4F9)){if(!_tmp6DA){
const char*_tmpCB4;void*_tmpCB3;(_tmpCB3=0,Cyc_Tcutil_terr(_tmp4F8->loc,((_tmpCB4="cannot determine value of constant",
_tag_dyneither(_tmpCB4,sizeof(char),35))),_tag_dyneither(_tmpCB3,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp6D9;++ i){struct _tuple15*_tmpCB7;struct Cyc_List_List*
_tmpCB6;es=((_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6->hd=((_tmpCB7=
_cycalloc(sizeof(*_tmpCB7)),((_tmpCB7->f1=0,((_tmpCB7->f2=_tmp4F9,_tmpCB7)))))),((
_tmpCB6->tl=es,_tmpCB6))))));}}if(_tmp4FA){struct Cyc_Absyn_Exp*_tmp6E0=Cyc_Toc_cast_it(
_tmp6DB,Cyc_Absyn_uint_exp(0,0));struct _tuple15*_tmpCBA;struct Cyc_List_List*
_tmpCB9;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9->hd=((
_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA->f1=0,((_tmpCBA->f2=_tmp6E0,
_tmpCBA)))))),((_tmpCB9->tl=0,_tmpCB9)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL210;};};}_LL24F: {struct Cyc_Absyn_Aggregate_e_struct*_tmp4FB=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp493;if(_tmp4FB->tag != 29)goto _LL251;
else{_tmp4FC=_tmp4FB->f1;_tmp4FD=_tmp4FB->f2;_tmp4FE=_tmp4FB->f3;_tmp4FF=_tmp4FB->f4;}}
_LL250: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp4FD
!= 0,0,0,_tmp4FE,_tmp4FC))->r;else{if(_tmp4FF == 0){const char*_tmpCBD;void*
_tmpCBC;(_tmpCBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCBD="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCBD,sizeof(
char),38))),_tag_dyneither(_tmpCBC,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp4FF;struct Cyc_Toc_Env _tmp6E6;struct
_RegionHandle*_tmp6E7;struct Cyc_Toc_Env*_tmp6E5=nv;_tmp6E6=*_tmp6E5;_tmp6E7=
_tmp6E6.rgn;{struct Cyc_List_List*_tmp6E8=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp6E7,e->loc,_tmp4FE,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FF->impl))->tagged){struct
_tuple20 _tmp6EA;struct Cyc_Absyn_Aggrfield*_tmp6EB;struct Cyc_Absyn_Exp*_tmp6EC;
struct _tuple20*_tmp6E9=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp6E8))->hd;_tmp6EA=*_tmp6E9;_tmp6EB=_tmp6EA.f1;_tmp6EC=_tmp6EA.f2;{void*
_tmp6ED=_tmp6EB->type;Cyc_Toc_exp_to_c(nv,_tmp6EC);if(Cyc_Toc_is_void_star_or_tvar(
_tmp6ED))_tmp6EC->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp6EC->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp4FF,_tmp6EB->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple15*_tmpCC2;struct _tuple15*_tmpCC1;struct _tuple15*_tmpCC0[2];
struct Cyc_List_List*_tmp6EE=(_tmpCC0[1]=((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((
_tmpCC1->f1=0,((_tmpCC1->f2=_tmp6EC,_tmpCC1)))))),((_tmpCC0[0]=((_tmpCC2=
_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2->f1=0,((_tmpCC2->f2=field_tag_exp,_tmpCC2)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCC0,sizeof(struct _tuple15*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp6EE,0);struct Cyc_Absyn_FieldName_struct*_tmpCC8;struct Cyc_Absyn_FieldName_struct
_tmpCC7;void*_tmpCC6[1];struct Cyc_List_List*ds=(_tmpCC6[0]=(void*)((_tmpCC8=
_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8[0]=((_tmpCC7.tag=1,((_tmpCC7.f1=_tmp6EB->name,
_tmpCC7)))),_tmpCC8)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC6,sizeof(void*),1)));struct _tuple15*_tmpCCB;struct _tuple15*
_tmpCCA[1];struct Cyc_List_List*dles=(_tmpCCA[0]=((_tmpCCB=_cycalloc(sizeof(*
_tmpCCB)),((_tmpCCB->f1=ds,((_tmpCCB->f2=umem,_tmpCCB)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCCA,sizeof(struct
_tuple15*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp6F7=0;struct Cyc_List_List*_tmp6F8=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp6F8 != 0;_tmp6F8=_tmp6F8->tl){struct Cyc_List_List*
_tmp6F9=_tmp6E8;for(0;_tmp6F9 != 0;_tmp6F9=_tmp6F9->tl){if((*((struct _tuple20*)
_tmp6F9->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6F8->hd){struct _tuple20
_tmp6FB;struct Cyc_Absyn_Aggrfield*_tmp6FC;struct Cyc_Absyn_Exp*_tmp6FD;struct
_tuple20*_tmp6FA=(struct _tuple20*)_tmp6F9->hd;_tmp6FB=*_tmp6FA;_tmp6FC=_tmp6FB.f1;
_tmp6FD=_tmp6FB.f2;{void*_tmp6FE=_tmp6FC->type;Cyc_Toc_exp_to_c(nv,_tmp6FD);if(
Cyc_Toc_is_void_star_or_tvar(_tmp6FE))_tmp6FD->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp6FD->r,0));{struct _tuple15*_tmpCCE;struct Cyc_List_List*
_tmpCCD;_tmp6F7=((_tmpCCD=_cycalloc(sizeof(*_tmpCCD)),((_tmpCCD->hd=((_tmpCCE=
_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE->f1=0,((_tmpCCE->f2=_tmp6FD,_tmpCCE)))))),((
_tmpCCD->tl=_tmp6F7,_tmpCCD))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6F7));}};};}
goto _LL210;_LL251: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp500=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp493;if(_tmp500->tag != 30)goto _LL253;else{_tmp501=(void*)_tmp500->f1;_tmp502=
_tmp500->f2;}}_LL252: {struct Cyc_List_List*fs;{void*_tmp701=Cyc_Tcutil_compress(
_tmp501);struct Cyc_List_List*_tmp703;_LL313: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp702=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp701;if(_tmp702->tag != 13)goto
_LL315;else{_tmp703=_tmp702->f2;}}_LL314: fs=_tmp703;goto _LL312;_LL315:;_LL316: {
const char*_tmpCD2;void*_tmpCD1[1];struct Cyc_String_pa_struct _tmpCD0;(_tmpCD0.tag=
0,((_tmpCD0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp501)),((_tmpCD1[0]=& _tmpCD0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD2="anon struct has type %s",
_tag_dyneither(_tmpCD2,sizeof(char),24))),_tag_dyneither(_tmpCD1,sizeof(void*),1)))))));}
_LL312:;}{struct Cyc_Toc_Env _tmp708;struct _RegionHandle*_tmp709;struct Cyc_Toc_Env*
_tmp707=nv;_tmp708=*_tmp707;_tmp709=_tmp708.rgn;{struct Cyc_List_List*_tmp70A=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp709,e->loc,_tmp502,Cyc_Absyn_StructA,fs);for(0;_tmp70A != 0;_tmp70A=_tmp70A->tl){
struct _tuple20 _tmp70C;struct Cyc_Absyn_Aggrfield*_tmp70D;struct Cyc_Absyn_Exp*
_tmp70E;struct _tuple20*_tmp70B=(struct _tuple20*)_tmp70A->hd;_tmp70C=*_tmp70B;
_tmp70D=_tmp70C.f1;_tmp70E=_tmp70C.f2;{void*_tmp70F=_tmp70D->type;Cyc_Toc_exp_to_c(
nv,_tmp70E);if(Cyc_Toc_is_void_star_or_tvar(_tmp70F))_tmp70E->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp70E->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp502);}goto _LL210;};}_LL253: {struct Cyc_Absyn_Datatype_e_struct*_tmp503=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp493;if(_tmp503->tag != 31)goto _LL255;else{
_tmp504=_tmp503->f1;_tmp505=_tmp503->f2;_tmp506=_tmp503->f3;}}_LL254: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple0*_tmp710=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp711=Cyc_Absyn_var_exp(_tmp710,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpCD3;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp506->name,((_tmpCD3="_struct",_tag_dyneither(_tmpCD3,sizeof(char),8)))));}
tag_exp=_tmp505->is_extensible?Cyc_Absyn_var_exp(_tmp506->name,0): Cyc_Toc_datatype_tag(
_tmp505,_tmp506->name);member_exp=_tmp711;{struct Cyc_List_List*_tmp713=_tmp506->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp504 != 0;(
_tmp504=_tmp504->tl,_tmp713=_tmp713->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp504->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp713))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple15*_tmpCD6;struct
Cyc_List_List*_tmpCD5;dles=((_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->hd=((
_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->f1=0,((_tmpCD6->f2=cur_e,_tmpCD6)))))),((
_tmpCD5->tl=dles,_tmpCD5))))));};}{struct _tuple15*_tmpCD9;struct Cyc_List_List*
_tmpCD8;dles=((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->hd=((_tmpCD9=
_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9->f1=0,((_tmpCD9->f2=tag_exp,_tmpCD9)))))),((
_tmpCD8->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpCD8))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpCDA;struct Cyc_List_List*_tmp718=(_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((
_tmpCDA->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpCDA->tl=0,_tmpCDA)))));{int i=1;for(0;_tmp504 != 0;(((_tmp504=
_tmp504->tl,i ++)),_tmp713=_tmp713->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp504->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp713))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp719=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpCDB;_tmp718=((_tmpCDB=_cycalloc(sizeof(*
_tmpCDB)),((_tmpCDB->hd=_tmp719,((_tmpCDB->tl=_tmp718,_tmpCDB))))));};}}{struct
Cyc_Absyn_Stmt*_tmp71B=Cyc_Absyn_exp_stmt(_tmp711,0);struct Cyc_List_List*_tmpCDC;
struct Cyc_Absyn_Stmt*_tmp71C=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((
_tmpCDC->hd=_tmp71B,((_tmpCDC->tl=_tmp718,_tmpCDC))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp710,datatype_ctype,0,_tmp71C,0));};}goto _LL210;};}
_LL255: {struct Cyc_Absyn_Enum_e_struct*_tmp507=(struct Cyc_Absyn_Enum_e_struct*)
_tmp493;if(_tmp507->tag != 32)goto _LL257;}_LL256: goto _LL258;_LL257: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp508=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp493;if(_tmp508->tag != 33)goto
_LL259;}_LL258: goto _LL210;_LL259: {struct Cyc_Absyn_Malloc_e_struct*_tmp509=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp493;if(_tmp509->tag != 34)goto _LL25B;else{
_tmp50A=_tmp509->f1;_tmp50B=_tmp50A.is_calloc;_tmp50C=_tmp50A.rgn;_tmp50D=
_tmp50A.elt_type;_tmp50E=_tmp50A.num_elts;_tmp50F=_tmp50A.fat_result;}}_LL25A: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp50D)));Cyc_Toc_exp_to_c(nv,
_tmp50E);if(_tmp50F){struct _tuple0*_tmp71F=Cyc_Toc_temp_var();struct _tuple0*
_tmp720=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp50B){xexp=_tmp50E;if(_tmp50C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp50C;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp71F,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp50D,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp71F,0));}{struct Cyc_Absyn_Exp*_tmpCDD[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCDD[2]=Cyc_Absyn_var_exp(_tmp71F,0),((_tmpCDD[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCDD[0]=Cyc_Absyn_var_exp(_tmp720,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCDD,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp50E,0);if(_tmp50C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp50C;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp71F,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp50D,Cyc_Absyn_var_exp(_tmp71F,0));}{struct Cyc_Absyn_Exp*_tmpCDE[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCDE[2]=Cyc_Absyn_var_exp(_tmp71F,0),((_tmpCDE[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpCDE[0]=Cyc_Absyn_var_exp(_tmp720,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDE,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp723=Cyc_Absyn_declare_stmt(_tmp71F,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp720,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp723);};}else{struct Cyc_Absyn_Exp*_tmp724=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp725=_tmp50E->r;union Cyc_Absyn_Cnst _tmp727;struct _tuple5 _tmp728;
int _tmp729;_LL318: {struct Cyc_Absyn_Const_e_struct*_tmp726=(struct Cyc_Absyn_Const_e_struct*)
_tmp725;if(_tmp726->tag != 0)goto _LL31A;else{_tmp727=_tmp726->f1;if((_tmp727.Int_c).tag
!= 4)goto _LL31A;_tmp728=(struct _tuple5)(_tmp727.Int_c).val;_tmp729=_tmp728.f2;
if(_tmp729 != 1)goto _LL31A;}}_LL319: goto _LL317;_LL31A:;_LL31B: _tmp724=Cyc_Absyn_times_exp(
_tmp724,_tmp50E,0);goto _LL317;_LL317:;}if(_tmp50C != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp50C;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp724))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp50D,
_tmp724))->r;}}goto _LL210;}_LL25B: {struct Cyc_Absyn_Swap_e_struct*_tmp510=(
struct Cyc_Absyn_Swap_e_struct*)_tmp493;if(_tmp510->tag != 35)goto _LL25D;else{
_tmp511=_tmp510->f1;_tmp512=_tmp510->f2;}}_LL25C: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp511->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp512->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpCE1;void*_tmpCE0;(_tmpCE0=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE1="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpCE1,sizeof(char),57))),_tag_dyneither(_tmpCE0,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}Cyc_Toc_exp_to_c(nv,_tmp511);Cyc_Toc_exp_to_c(
nv,_tmp512);{struct Cyc_Absyn_Exp*_tmpCE2[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((
_tmpCE2[1]=Cyc_Toc_push_address_exp(_tmp512),((_tmpCE2[0]=Cyc_Toc_push_address_exp(
_tmp511),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCE2,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL210;};}
_LL25D: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp513=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp493;if(_tmp513->tag != 38)goto _LL25F;else{_tmp514=_tmp513->f1;_tmp515=_tmp513->f2;}}
_LL25E: {void*_tmp72D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp514->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp514);{void*_tmp72E=
_tmp72D;struct Cyc_Absyn_AggrInfo _tmp730;union Cyc_Absyn_AggrInfoU _tmp731;struct
Cyc_Absyn_Aggrdecl**_tmp732;struct Cyc_Absyn_Aggrdecl*_tmp733;_LL31D: {struct Cyc_Absyn_AggrType_struct*
_tmp72F=(struct Cyc_Absyn_AggrType_struct*)_tmp72E;if(_tmp72F->tag != 12)goto
_LL31F;else{_tmp730=_tmp72F->f1;_tmp731=_tmp730.aggr_info;if((_tmp731.KnownAggr).tag
!= 2)goto _LL31F;_tmp732=(struct Cyc_Absyn_Aggrdecl**)(_tmp731.KnownAggr).val;
_tmp733=*_tmp732;}}_LL31E: {struct Cyc_Absyn_Exp*_tmp734=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp733,_tmp515),0);struct Cyc_Absyn_Exp*_tmp735=Cyc_Absyn_aggrmember_exp(
_tmp514,_tmp515,0);struct Cyc_Absyn_Exp*_tmp736=Cyc_Absyn_aggrmember_exp(_tmp735,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp736,_tmp734,0))->r;goto _LL31C;}
_LL31F:;_LL320: {const char*_tmpCE5;void*_tmpCE4;(_tmpCE4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE5="non-aggregate type in tagcheck",
_tag_dyneither(_tmpCE5,sizeof(char),31))),_tag_dyneither(_tmpCE4,sizeof(void*),0)));}
_LL31C:;}goto _LL210;}_LL25F: {struct Cyc_Absyn_StmtExp_e_struct*_tmp516=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp493;if(_tmp516->tag != 37)goto _LL261;else{_tmp517=
_tmp516->f1;}}_LL260: Cyc_Toc_stmt_to_c(nv,_tmp517);goto _LL210;_LL261: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp518=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp493;if(_tmp518->tag != 36)
goto _LL263;}_LL262: {const char*_tmpCE8;void*_tmpCE7;(_tmpCE7=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE8="UnresolvedMem",
_tag_dyneither(_tmpCE8,sizeof(char),14))),_tag_dyneither(_tmpCE7,sizeof(void*),0)));}
_LL263: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp519=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp493;if(_tmp519->tag != 26)goto _LL265;}_LL264: {const char*_tmpCEB;void*_tmpCEA;(
_tmpCEA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCEB="compoundlit",_tag_dyneither(_tmpCEB,sizeof(char),12))),_tag_dyneither(
_tmpCEA,sizeof(void*),0)));}_LL265: {struct Cyc_Absyn_Valueof_e_struct*_tmp51A=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp493;if(_tmp51A->tag != 39)goto _LL210;}
_LL266: {const char*_tmpCEE;void*_tmpCED;(_tmpCED=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCEE="valueof(-)",
_tag_dyneither(_tmpCEE,sizeof(char),11))),_tag_dyneither(_tmpCED,sizeof(void*),0)));}
_LL210:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,
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
struct Cyc_Absyn_Stmt*s;{void*_tmp73F=p->r;struct Cyc_Absyn_Vardecl*_tmp741;struct
Cyc_Absyn_Vardecl _tmp742;struct _tuple0*_tmp743;struct Cyc_Absyn_Pat*_tmp744;
struct Cyc_Absyn_Vardecl*_tmp746;struct Cyc_Absyn_Vardecl _tmp747;struct _tuple0*
_tmp748;struct Cyc_Absyn_Vardecl*_tmp74B;struct Cyc_Absyn_Pat*_tmp74C;enum Cyc_Absyn_Sign
_tmp74F;int _tmp750;char _tmp752;struct _dyneither_ptr _tmp754;struct Cyc_Absyn_Enumdecl*
_tmp756;struct Cyc_Absyn_Enumfield*_tmp757;void*_tmp759;struct Cyc_Absyn_Enumfield*
_tmp75A;struct Cyc_Absyn_Pat*_tmp75C;struct Cyc_Absyn_Pat _tmp75D;void*_tmp75E;
struct Cyc_Absyn_Datatypedecl*_tmp760;struct Cyc_Absyn_Datatypefield*_tmp761;
struct Cyc_List_List*_tmp762;struct Cyc_List_List*_tmp764;struct Cyc_List_List*
_tmp766;struct Cyc_Absyn_AggrInfo*_tmp768;struct Cyc_Absyn_AggrInfo*_tmp76A;struct
Cyc_Absyn_AggrInfo _tmp76B;union Cyc_Absyn_AggrInfoU _tmp76C;struct Cyc_List_List*
_tmp76D;struct Cyc_Absyn_Pat*_tmp76F;_LL322: {struct Cyc_Absyn_Var_p_struct*
_tmp740=(struct Cyc_Absyn_Var_p_struct*)_tmp73F;if(_tmp740->tag != 1)goto _LL324;
else{_tmp741=_tmp740->f1;_tmp742=*_tmp741;_tmp743=_tmp742.name;_tmp744=_tmp740->f2;}}
_LL323: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp743,r),rgn,t,r,path,
_tmp744,fail_stmt,decls);_LL324: {struct Cyc_Absyn_TagInt_p_struct*_tmp745=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp73F;if(_tmp745->tag != 3)goto _LL326;else{
_tmp746=_tmp745->f2;_tmp747=*_tmp746;_tmp748=_tmp747.name;}}_LL325: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp748,r);goto _LL327;_LL326: {struct Cyc_Absyn_Wild_p_struct*_tmp749=(
struct Cyc_Absyn_Wild_p_struct*)_tmp73F;if(_tmp749->tag != 0)goto _LL328;}_LL327: s=
Cyc_Toc_skip_stmt_dl();goto _LL321;_LL328: {struct Cyc_Absyn_Reference_p_struct*
_tmp74A=(struct Cyc_Absyn_Reference_p_struct*)_tmp73F;if(_tmp74A->tag != 2)goto
_LL32A;else{_tmp74B=_tmp74A->f1;_tmp74C=_tmp74A->f2;}}_LL329: {struct _tuple0*
_tmp773=Cyc_Toc_temp_var();{struct _tuple22*_tmpCF1;struct Cyc_List_List*_tmpCF0;
decls=((_tmpCF0=_region_malloc(rgn,sizeof(*_tmpCF0)),((_tmpCF0->hd=((_tmpCF1=
_region_malloc(rgn,sizeof(*_tmpCF1)),((_tmpCF1->f1=_tmp773,((_tmpCF1->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpCF1)))))),((_tmpCF0->tl=decls,_tmpCF0))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp74B->name,Cyc_Absyn_var_exp(_tmp773,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp773,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp776=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp74C,fail_stmt,decls);_tmp776.f3=Cyc_Absyn_seq_stmt(s,_tmp776.f3,
0);return _tmp776;};}_LL32A: {struct Cyc_Absyn_Null_p_struct*_tmp74D=(struct Cyc_Absyn_Null_p_struct*)
_tmp73F;if(_tmp74D->tag != 8)goto _LL32C;}_LL32B: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL321;_LL32C: {struct Cyc_Absyn_Int_p_struct*_tmp74E=(struct
Cyc_Absyn_Int_p_struct*)_tmp73F;if(_tmp74E->tag != 9)goto _LL32E;else{_tmp74F=
_tmp74E->f1;_tmp750=_tmp74E->f2;}}_LL32D: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp74F,_tmp750,0),fail_stmt);goto _LL321;_LL32E: {struct Cyc_Absyn_Char_p_struct*
_tmp751=(struct Cyc_Absyn_Char_p_struct*)_tmp73F;if(_tmp751->tag != 10)goto _LL330;
else{_tmp752=_tmp751->f1;}}_LL32F: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp752,0),fail_stmt);goto _LL321;_LL330: {struct Cyc_Absyn_Float_p_struct*_tmp753=(
struct Cyc_Absyn_Float_p_struct*)_tmp73F;if(_tmp753->tag != 11)goto _LL332;else{
_tmp754=_tmp753->f1;}}_LL331: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp754,
0),fail_stmt);goto _LL321;_LL332: {struct Cyc_Absyn_Enum_p_struct*_tmp755=(struct
Cyc_Absyn_Enum_p_struct*)_tmp73F;if(_tmp755->tag != 12)goto _LL334;else{_tmp756=
_tmp755->f1;_tmp757=_tmp755->f2;}}_LL333:{struct Cyc_Absyn_Enum_e_struct _tmpCF4;
struct Cyc_Absyn_Enum_e_struct*_tmpCF3;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3[0]=((_tmpCF4.tag=32,((
_tmpCF4.f1=_tmp757->name,((_tmpCF4.f2=(struct Cyc_Absyn_Enumdecl*)_tmp756,((
_tmpCF4.f3=(struct Cyc_Absyn_Enumfield*)_tmp757,_tmpCF4)))))))),_tmpCF3)))),0),
fail_stmt);}goto _LL321;_LL334: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp758=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp73F;if(_tmp758->tag != 13)goto _LL336;else{
_tmp759=(void*)_tmp758->f1;_tmp75A=_tmp758->f2;}}_LL335:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpCF7;struct Cyc_Absyn_AnonEnum_e_struct*_tmpCF6;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6[0]=((_tmpCF7.tag=33,((
_tmpCF7.f1=_tmp75A->name,((_tmpCF7.f2=(void*)_tmp759,((_tmpCF7.f3=(struct Cyc_Absyn_Enumfield*)
_tmp75A,_tmpCF7)))))))),_tmpCF6)))),0),fail_stmt);}goto _LL321;_LL336: {struct Cyc_Absyn_Pointer_p_struct*
_tmp75B=(struct Cyc_Absyn_Pointer_p_struct*)_tmp73F;if(_tmp75B->tag != 5)goto
_LL338;else{_tmp75C=_tmp75B->f1;_tmp75D=*_tmp75C;_tmp75E=_tmp75D.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp75F=(struct Cyc_Absyn_Datatype_p_struct*)_tmp75E;if(_tmp75F->tag != 7)goto
_LL338;else{_tmp760=_tmp75F->f1;_tmp761=_tmp75F->f2;_tmp762=_tmp75F->f3;}};}}
_LL337: s=Cyc_Toc_skip_stmt_dl();{struct _tuple0*_tmp77B=Cyc_Toc_temp_var();const
char*_tmpCF8;struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp761->name,((
_tmpCF8="_struct",_tag_dyneither(_tmpCF8,sizeof(char),8))));void*_tmp77C=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp77C,r);struct Cyc_List_List*_tmp77D=_tmp761->typs;for(0;_tmp762 != 0;(((
_tmp762=_tmp762->tl,_tmp77D=((struct Cyc_List_List*)_check_null(_tmp77D))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp77E=(struct Cyc_Absyn_Pat*)_tmp762->hd;if(_tmp77E->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp77F=(*((struct _tuple9*)((
struct Cyc_List_List*)_check_null(_tmp77D))->hd)).f2;struct _tuple0*_tmp780=Cyc_Toc_temp_var();
void*_tmp781=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp77E->topt))->v;void*
_tmp782=Cyc_Toc_typ_to_c(_tmp781);struct Cyc_Absyn_Exp*_tmp783=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp77B,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp77F)))_tmp783=Cyc_Toc_cast_it(_tmp782,_tmp783);{struct
_tuple22*_tmpCFB;struct Cyc_List_List*_tmpCFA;decls=((_tmpCFA=_region_malloc(rgn,
sizeof(*_tmpCFA)),((_tmpCFA->hd=((_tmpCFB=_region_malloc(rgn,sizeof(*_tmpCFB)),((
_tmpCFB->f1=_tmp780,((_tmpCFB->f2=_tmp782,_tmpCFB)))))),((_tmpCFA->tl=decls,
_tmpCFA))))));}{struct _tuple21 _tmp786=Cyc_Toc_xlate_pat(nv,rgn,_tmp781,Cyc_Absyn_var_exp(
_tmp780,0),_tmp783,_tmp77E,fail_stmt,decls);nv=_tmp786.f1;decls=_tmp786.f2;{
struct Cyc_Absyn_Stmt*_tmp787=_tmp786.f3;struct Cyc_Absyn_Stmt*_tmp788=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp780,0),_tmp783,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp788,_tmp787,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp77B,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp760->is_extensible?Cyc_Absyn_var_exp(_tmp761->name,0): Cyc_Toc_datatype_tag(
_tmp760,_tmp761->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp77B,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp77B,_tmp77C,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL321;};};_LL338: {struct Cyc_Absyn_Datatype_p_struct*_tmp763=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp73F;if(_tmp763->tag != 7)goto _LL33A;else{
_tmp764=_tmp763->f3;}}_LL339: _tmp766=_tmp764;goto _LL33B;_LL33A: {struct Cyc_Absyn_Tuple_p_struct*
_tmp765=(struct Cyc_Absyn_Tuple_p_struct*)_tmp73F;if(_tmp765->tag != 4)goto _LL33C;
else{_tmp766=_tmp765->f1;}}_LL33B: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp766 != 0;(_tmp766=_tmp766->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp78A=(struct Cyc_Absyn_Pat*)
_tmp766->hd;if(_tmp78A->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple0*
_tmp78B=Cyc_Toc_temp_var();void*_tmp78C=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp78A->topt))->v;{struct _tuple22*_tmpCFE;struct Cyc_List_List*_tmpCFD;decls=((
_tmpCFD=_region_malloc(rgn,sizeof(*_tmpCFD)),((_tmpCFD->hd=((_tmpCFE=
_region_malloc(rgn,sizeof(*_tmpCFE)),((_tmpCFE->f1=_tmp78B,((_tmpCFE->f2=Cyc_Toc_typ_to_c(
_tmp78C),_tmpCFE)))))),((_tmpCFD->tl=decls,_tmpCFD))))));}{struct _tuple21 _tmp78F=
Cyc_Toc_xlate_pat(nv,rgn,_tmp78C,Cyc_Absyn_var_exp(_tmp78B,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp78A,fail_stmt,decls);nv=_tmp78F.f1;decls=
_tmp78F.f2;{struct Cyc_Absyn_Stmt*_tmp790=_tmp78F.f3;struct Cyc_Absyn_Stmt*_tmp791=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp78B,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp791,
_tmp790,0),0);};};};}goto _LL321;};_LL33C: {struct Cyc_Absyn_Aggr_p_struct*_tmp767=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp73F;if(_tmp767->tag != 6)goto _LL33E;else{
_tmp768=_tmp767->f1;if(_tmp768 != 0)goto _LL33E;}}_LL33D: {const char*_tmpD01;void*
_tmpD00;(_tmpD00=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD01="unresolved aggregate pattern!",_tag_dyneither(_tmpD01,sizeof(char),30))),
_tag_dyneither(_tmpD00,sizeof(void*),0)));}_LL33E: {struct Cyc_Absyn_Aggr_p_struct*
_tmp769=(struct Cyc_Absyn_Aggr_p_struct*)_tmp73F;if(_tmp769->tag != 6)goto _LL340;
else{_tmp76A=_tmp769->f1;if(_tmp76A == 0)goto _LL340;_tmp76B=*_tmp76A;_tmp76C=
_tmp76B.aggr_info;_tmp76D=_tmp769->f3;}}_LL33F: {struct Cyc_Absyn_Aggrdecl*
_tmp794=Cyc_Absyn_get_known_aggrdecl(_tmp76C);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp794->impl))->tagged){struct _tuple23 _tmp796;struct Cyc_List_List*
_tmp797;struct Cyc_Absyn_Pat*_tmp798;struct _tuple23*_tmp795=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp76D))->hd;_tmp796=*_tmp795;_tmp797=_tmp796.f1;
_tmp798=_tmp796.f2;{struct _dyneither_ptr*f;{void*_tmp799=(void*)((struct Cyc_List_List*)
_check_null(_tmp797))->hd;struct _dyneither_ptr*_tmp79B;_LL349: {struct Cyc_Absyn_FieldName_struct*
_tmp79A=(struct Cyc_Absyn_FieldName_struct*)_tmp799;if(_tmp79A->tag != 1)goto
_LL34B;else{_tmp79B=_tmp79A->f1;}}_LL34A: f=_tmp79B;goto _LL348;_LL34B:;_LL34C: {
const char*_tmpD04;void*_tmpD03;(_tmpD03=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD04="no field name in tagged union pattern",
_tag_dyneither(_tmpD04,sizeof(char),38))),_tag_dyneither(_tmpD03,sizeof(void*),0)));}
_LL348:;}{struct _tuple0*_tmp79E=Cyc_Toc_temp_var();void*_tmp79F=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp798->topt))->v;void*_tmp7A0=Cyc_Toc_typ_to_c(
_tmp79F);{struct _tuple22*_tmpD07;struct Cyc_List_List*_tmpD06;decls=((_tmpD06=
_region_malloc(rgn,sizeof(*_tmpD06)),((_tmpD06->hd=((_tmpD07=_region_malloc(rgn,
sizeof(*_tmpD07)),((_tmpD07->f1=_tmp79E,((_tmpD07->f2=_tmp7A0,_tmpD07)))))),((
_tmpD06->tl=decls,_tmpD06))))));}{struct Cyc_Absyn_Exp*_tmp7A3=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp7A4=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7A4=Cyc_Toc_cast_it(_tmp7A0,_tmp7A4);_tmp7A3=Cyc_Toc_cast_it(_tmp7A0,_tmp7A3);{
struct _tuple21 _tmp7A5=Cyc_Toc_xlate_pat(nv,rgn,_tmp79F,Cyc_Absyn_var_exp(_tmp79E,
0),_tmp7A3,_tmp798,fail_stmt,decls);nv=_tmp7A5.f1;decls=_tmp7A5.f2;{struct Cyc_Absyn_Stmt*
_tmp7A6=_tmp7A5.f3;struct Cyc_Absyn_Stmt*_tmp7A7=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp794,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7A8=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp79E,0),_tmp7A4,0);s=Cyc_Absyn_seq_stmt(_tmp7A7,Cyc_Absyn_seq_stmt(_tmp7A8,
_tmp7A6,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp76D != 0;_tmp76D=
_tmp76D->tl){struct _tuple23*_tmp7A9=(struct _tuple23*)_tmp76D->hd;struct Cyc_Absyn_Pat*
_tmp7AA=(*_tmp7A9).f2;if(_tmp7AA->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7AB=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7A9).f1))->hd;struct _dyneither_ptr*_tmp7AD;_LL34E: {struct Cyc_Absyn_FieldName_struct*
_tmp7AC=(struct Cyc_Absyn_FieldName_struct*)_tmp7AB;if(_tmp7AC->tag != 1)goto
_LL350;else{_tmp7AD=_tmp7AC->f1;}}_LL34F: f=_tmp7AD;goto _LL34D;_LL350:;_LL351: {
struct Cyc_Toc_Match_error_struct _tmpD0A;struct Cyc_Toc_Match_error_struct*_tmpD09;(
int)_throw((void*)((_tmpD09=_cycalloc_atomic(sizeof(*_tmpD09)),((_tmpD09[0]=((
_tmpD0A.tag=Cyc_Toc_Match_error,_tmpD0A)),_tmpD09)))));}_LL34D:;}{struct _tuple0*
_tmp7B0=Cyc_Toc_temp_var();void*_tmp7B1=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7AA->topt))->v;void*_tmp7B2=Cyc_Toc_typ_to_c(_tmp7B1);{struct _tuple22*
_tmpD0D;struct Cyc_List_List*_tmpD0C;decls=((_tmpD0C=_region_malloc(rgn,sizeof(*
_tmpD0C)),((_tmpD0C->hd=((_tmpD0D=_region_malloc(rgn,sizeof(*_tmpD0D)),((_tmpD0D->f1=
_tmp7B0,((_tmpD0D->f2=_tmp7B2,_tmpD0D)))))),((_tmpD0C->tl=decls,_tmpD0C))))));}{
struct _tuple21 _tmp7B5=Cyc_Toc_xlate_pat(nv,rgn,_tmp7B1,Cyc_Absyn_var_exp(_tmp7B0,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7AA,fail_stmt,decls);nv=_tmp7B5.f1;
decls=_tmp7B5.f2;{struct Cyc_Absyn_Exp*_tmp7B6=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp794->impl))->fields,f)))->type))
_tmp7B6=Cyc_Toc_cast_it(_tmp7B2,_tmp7B6);{struct Cyc_Absyn_Stmt*_tmp7B7=_tmp7B5.f3;
struct Cyc_Absyn_Stmt*_tmp7B8=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7B0,0),
_tmp7B6,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7B8,_tmp7B7,0),0);};};};};};}}
goto _LL321;}_LL340: {struct Cyc_Absyn_Pointer_p_struct*_tmp76E=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp73F;if(_tmp76E->tag != 5)goto _LL342;else{_tmp76F=_tmp76E->f1;}}_LL341: {
struct _tuple0*_tmp7B9=Cyc_Toc_temp_var();void*_tmp7BA=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp76F->topt))->v;{struct _tuple22*_tmpD10;struct Cyc_List_List*
_tmpD0F;decls=((_tmpD0F=_region_malloc(rgn,sizeof(*_tmpD0F)),((_tmpD0F->hd=((
_tmpD10=_region_malloc(rgn,sizeof(*_tmpD10)),((_tmpD10->f1=_tmp7B9,((_tmpD10->f2=
Cyc_Toc_typ_to_c(_tmp7BA),_tmpD10)))))),((_tmpD0F->tl=decls,_tmpD0F))))));}{
struct _tuple21 _tmp7BD=Cyc_Toc_xlate_pat(nv,rgn,_tmp7BA,Cyc_Absyn_var_exp(_tmp7B9,
0),Cyc_Absyn_deref_exp(path,0),_tmp76F,fail_stmt,decls);nv=_tmp7BD.f1;decls=
_tmp7BD.f2;{struct Cyc_Absyn_Stmt*_tmp7BE=_tmp7BD.f3;struct Cyc_Absyn_Stmt*_tmp7BF=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7B9,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7BE,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp7BF,0);else{s=_tmp7BF;}goto _LL321;};};}_LL342: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp770=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp73F;if(_tmp770->tag != 14)goto
_LL344;}_LL343: {const char*_tmpD13;void*_tmpD12;(_tmpD12=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD13="unknownid pat",
_tag_dyneither(_tmpD13,sizeof(char),14))),_tag_dyneither(_tmpD12,sizeof(void*),0)));}
_LL344: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp771=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp73F;if(_tmp771->tag != 15)goto _LL346;}_LL345: {const char*_tmpD16;void*_tmpD15;(
_tmpD15=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD16="unknowncall pat",_tag_dyneither(_tmpD16,sizeof(char),16))),
_tag_dyneither(_tmpD15,sizeof(void*),0)));}_LL346: {struct Cyc_Absyn_Exp_p_struct*
_tmp772=(struct Cyc_Absyn_Exp_p_struct*)_tmp73F;if(_tmp772->tag != 16)goto _LL321;}
_LL347: {const char*_tmpD19;void*_tmpD18;(_tmpD18=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD19="exp pat",
_tag_dyneither(_tmpD19,sizeof(char),8))),_tag_dyneither(_tmpD18,sizeof(void*),0)));}
_LL321:;}{struct _tuple21 _tmpD1A;return(_tmpD1A.f1=nv,((_tmpD1A.f2=decls,((
_tmpD1A.f3=s,_tmpD1A)))));};}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc);static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpD1B;
return(_tmpD1B=_region_malloc(r,sizeof(*_tmpD1B)),((_tmpD1B->f1=Cyc_Toc_fresh_label(),((
_tmpD1B->f2=Cyc_Toc_fresh_label(),((_tmpD1B->f3=sc,_tmpD1B)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs);static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7C8=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp7C9=Cyc_Tcutil_compress(_tmp7C8);_LL353: {struct Cyc_Absyn_IntType_struct*
_tmp7CA=(struct Cyc_Absyn_IntType_struct*)_tmp7C9;if(_tmp7CA->tag != 6)goto _LL355;}
_LL354: goto _LL356;_LL355: {struct Cyc_Absyn_EnumType_struct*_tmp7CB=(struct Cyc_Absyn_EnumType_struct*)
_tmp7C9;if(_tmp7CB->tag != 14)goto _LL357;}_LL356: leave_as_switch=1;goto _LL352;
_LL357:;_LL358: leave_as_switch=0;goto _LL352;_LL352:;}{struct Cyc_List_List*
_tmp7CC=scs;for(0;_tmp7CC != 0;_tmp7CC=_tmp7CC->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7CC->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7CC->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp7CD=scs;for(0;_tmp7CD != 0;_tmp7CD=_tmp7CD->tl){struct Cyc_Absyn_Stmt*
_tmp7CE=((struct Cyc_Absyn_Switch_clause*)_tmp7CD->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp7CD->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7CE,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp7D0;struct _RegionHandle*_tmp7D1;struct Cyc_Toc_Env*_tmp7CF=
nv;_tmp7D0=*_tmp7CF;_tmp7D1=_tmp7D0.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp7D1,nv,next_l),_tmp7CE);};}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp7D3;
struct _RegionHandle*_tmp7D4;struct Cyc_Toc_Env*_tmp7D2=nv;_tmp7D3=*_tmp7D2;
_tmp7D4=_tmp7D3.rgn;{struct Cyc_Toc_Env*_tmp7D5=Cyc_Toc_share_env(_tmp7D4,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp7D4,Cyc_Toc_gen_label,
_tmp7D4,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp7D6=lscs;for(0;_tmp7D6 != 0;
_tmp7D6=_tmp7D6->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp7D6->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp7D6->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp7D6->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp7D8;struct Cyc_List_List*_tmp7D9;struct Cyc_Absyn_Stmt*_tmp7DA;struct _tuple21
_tmp7D7=Cyc_Toc_xlate_pat(_tmp7D5,_tmp7D4,_tmp7C8,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp7D8=_tmp7D7.f1;_tmp7D9=_tmp7D7.f2;_tmp7DA=_tmp7D7.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp7DB=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp7D8,_tmp7DB);_tmp7DA=Cyc_Absyn_seq_stmt(
_tmp7DA,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7DB,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp7D9;{
struct Cyc_List_List*_tmpD1C;nvs=((_tmpD1C=_region_malloc(_tmp7D4,sizeof(*_tmpD1C)),((
_tmpD1C->hd=_tmp7D8,((_tmpD1C->tl=nvs,_tmpD1C))))));}{struct Cyc_List_List*
_tmpD1D;test_stmts=((_tmpD1D=_region_malloc(_tmp7D4,sizeof(*_tmpD1D)),((_tmpD1D->hd=
_tmp7DA,((_tmpD1D->tl=test_stmts,_tmpD1D))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp7DF;struct _dyneither_ptr*_tmp7E0;struct _dyneither_ptr*_tmp7E1;
struct Cyc_Absyn_Switch_clause*_tmp7E2;struct _tuple24*_tmp7DE=(struct _tuple24*)
lscs->hd;_tmp7DF=*_tmp7DE;_tmp7E0=_tmp7DF.f1;_tmp7E1=_tmp7DF.f2;_tmp7E2=_tmp7DF.f3;{
struct Cyc_Toc_Env*_tmp7E3=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp7E2->body;struct Cyc_Toc_Env _tmp7E5;struct
_RegionHandle*_tmp7E6;struct Cyc_Toc_Env*_tmp7E4=_tmp7D5;_tmp7E5=*_tmp7E4;_tmp7E6=
_tmp7E5.rgn;if(lscs->tl != 0){struct _tuple24 _tmp7E8;struct _dyneither_ptr*_tmp7E9;
struct Cyc_Absyn_Switch_clause*_tmp7EA;struct _tuple24*_tmp7E7=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7E8=*_tmp7E7;_tmp7E9=_tmp7E8.f2;
_tmp7EA=_tmp7E8.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp7E6,
_tmp7E3,end_l,_tmp7E9,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp7EA->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp7E6,
_tmp7E3,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7E0,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp7E1,s,0),0);{struct Cyc_List_List*_tmpD1E;stmts=((_tmpD1E=_region_malloc(
_tmp7D4,sizeof(*_tmpD1E)),((_tmpD1E->hd=s,((_tmpD1E->tl=stmts,_tmpD1E))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp7ED;struct _tuple0*_tmp7EE;void*_tmp7EF;struct _tuple22*_tmp7EC=(struct
_tuple22*)decls->hd;_tmp7ED=*_tmp7EC;_tmp7EE=_tmp7ED.f1;_tmp7EF=_tmp7ED.f2;res=
Cyc_Absyn_declare_stmt(_tmp7EE,_tmp7EF,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*_tmpD1F;return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((
_tmpD1F->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD1F->tl=0,_tmpD1F)))))),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s);void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp7F1=s->r;struct Cyc_Absyn_Exp*_tmp7F4;struct
Cyc_Absyn_Stmt*_tmp7F6;struct Cyc_Absyn_Stmt*_tmp7F7;struct Cyc_Absyn_Exp*_tmp7F9;
struct Cyc_Absyn_Exp*_tmp7FB;struct Cyc_Absyn_Stmt*_tmp7FC;struct Cyc_Absyn_Stmt*
_tmp7FD;struct _tuple8 _tmp7FF;struct Cyc_Absyn_Exp*_tmp800;struct Cyc_Absyn_Stmt*
_tmp801;struct Cyc_Absyn_Stmt*_tmp803;struct Cyc_Absyn_Stmt*_tmp805;struct Cyc_Absyn_Stmt*
_tmp807;struct Cyc_Absyn_Exp*_tmp809;struct _tuple8 _tmp80A;struct Cyc_Absyn_Exp*
_tmp80B;struct _tuple8 _tmp80C;struct Cyc_Absyn_Exp*_tmp80D;struct Cyc_Absyn_Stmt*
_tmp80E;struct Cyc_Absyn_Exp*_tmp810;struct Cyc_List_List*_tmp811;struct Cyc_List_List*
_tmp813;struct Cyc_Absyn_Switch_clause**_tmp814;struct Cyc_Absyn_Decl*_tmp816;
struct Cyc_Absyn_Stmt*_tmp817;struct _dyneither_ptr*_tmp819;struct Cyc_Absyn_Stmt*
_tmp81A;struct Cyc_Absyn_Stmt*_tmp81C;struct _tuple8 _tmp81D;struct Cyc_Absyn_Exp*
_tmp81E;struct Cyc_Absyn_Stmt*_tmp820;struct Cyc_List_List*_tmp821;struct Cyc_Absyn_Exp*
_tmp823;_LL35A: {struct Cyc_Absyn_Skip_s_struct*_tmp7F2=(struct Cyc_Absyn_Skip_s_struct*)
_tmp7F1;if(_tmp7F2->tag != 0)goto _LL35C;}_LL35B: return;_LL35C: {struct Cyc_Absyn_Exp_s_struct*
_tmp7F3=(struct Cyc_Absyn_Exp_s_struct*)_tmp7F1;if(_tmp7F3->tag != 1)goto _LL35E;
else{_tmp7F4=_tmp7F3->f1;}}_LL35D: Cyc_Toc_exp_to_c(nv,_tmp7F4);return;_LL35E: {
struct Cyc_Absyn_Seq_s_struct*_tmp7F5=(struct Cyc_Absyn_Seq_s_struct*)_tmp7F1;if(
_tmp7F5->tag != 2)goto _LL360;else{_tmp7F6=_tmp7F5->f1;_tmp7F7=_tmp7F5->f2;}}
_LL35F: Cyc_Toc_stmt_to_c(nv,_tmp7F6);s=_tmp7F7;continue;_LL360: {struct Cyc_Absyn_Return_s_struct*
_tmp7F8=(struct Cyc_Absyn_Return_s_struct*)_tmp7F1;if(_tmp7F8->tag != 3)goto _LL362;
else{_tmp7F9=_tmp7F8->f1;}}_LL361: {struct Cyc_Core_Opt*topt=0;if(_tmp7F9 != 0){{
struct Cyc_Core_Opt*_tmpD20;topt=((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F9->topt))->v),
_tmpD20))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp7F9);}if(s->try_depth
> 0){if(topt != 0){struct _tuple0*_tmp825=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp826=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp825,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp825,(void*)topt->v,_tmp7F9,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp826,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL362: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp7FA=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7F1;if(_tmp7FA->tag != 4)goto _LL364;
else{_tmp7FB=_tmp7FA->f1;_tmp7FC=_tmp7FA->f2;_tmp7FD=_tmp7FA->f3;}}_LL363: Cyc_Toc_exp_to_c(
nv,_tmp7FB);Cyc_Toc_stmt_to_c(nv,_tmp7FC);s=_tmp7FD;continue;_LL364: {struct Cyc_Absyn_While_s_struct*
_tmp7FE=(struct Cyc_Absyn_While_s_struct*)_tmp7F1;if(_tmp7FE->tag != 5)goto _LL366;
else{_tmp7FF=_tmp7FE->f1;_tmp800=_tmp7FF.f1;_tmp801=_tmp7FE->f2;}}_LL365: Cyc_Toc_exp_to_c(
nv,_tmp800);{struct Cyc_Toc_Env _tmp828;struct _RegionHandle*_tmp829;struct Cyc_Toc_Env*
_tmp827=nv;_tmp828=*_tmp827;_tmp829=_tmp828.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp829,nv),_tmp801);return;};_LL366: {struct Cyc_Absyn_Break_s_struct*_tmp802=(
struct Cyc_Absyn_Break_s_struct*)_tmp7F1;if(_tmp802->tag != 6)goto _LL368;else{
_tmp803=_tmp802->f1;}}_LL367: {struct Cyc_Toc_Env _tmp82B;struct _dyneither_ptr**
_tmp82C;struct Cyc_Toc_Env*_tmp82A=nv;_tmp82B=*_tmp82A;_tmp82C=_tmp82B.break_lab;
if(_tmp82C != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp82C,0);{int dest_depth=_tmp803 == 0?0:
_tmp803->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL368: {struct Cyc_Absyn_Continue_s_struct*_tmp804=(struct Cyc_Absyn_Continue_s_struct*)
_tmp7F1;if(_tmp804->tag != 7)goto _LL36A;else{_tmp805=_tmp804->f1;}}_LL369: {
struct Cyc_Toc_Env _tmp82E;struct _dyneither_ptr**_tmp82F;struct Cyc_Toc_Env*_tmp82D=
nv;_tmp82E=*_tmp82D;_tmp82F=_tmp82E.continue_lab;if(_tmp82F != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp82F,0);_tmp807=_tmp805;goto _LL36B;}_LL36A: {struct Cyc_Absyn_Goto_s_struct*
_tmp806=(struct Cyc_Absyn_Goto_s_struct*)_tmp7F1;if(_tmp806->tag != 8)goto _LL36C;
else{_tmp807=_tmp806->f2;}}_LL36B: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp807))->try_depth,s);return;_LL36C: {struct Cyc_Absyn_For_s_struct*
_tmp808=(struct Cyc_Absyn_For_s_struct*)_tmp7F1;if(_tmp808->tag != 9)goto _LL36E;
else{_tmp809=_tmp808->f1;_tmp80A=_tmp808->f2;_tmp80B=_tmp80A.f1;_tmp80C=_tmp808->f3;
_tmp80D=_tmp80C.f1;_tmp80E=_tmp808->f4;}}_LL36D: Cyc_Toc_exp_to_c(nv,_tmp809);Cyc_Toc_exp_to_c(
nv,_tmp80B);Cyc_Toc_exp_to_c(nv,_tmp80D);{struct Cyc_Toc_Env _tmp831;struct
_RegionHandle*_tmp832;struct Cyc_Toc_Env*_tmp830=nv;_tmp831=*_tmp830;_tmp832=
_tmp831.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp832,nv),_tmp80E);return;};
_LL36E: {struct Cyc_Absyn_Switch_s_struct*_tmp80F=(struct Cyc_Absyn_Switch_s_struct*)
_tmp7F1;if(_tmp80F->tag != 10)goto _LL370;else{_tmp810=_tmp80F->f1;_tmp811=_tmp80F->f2;}}
_LL36F: Cyc_Toc_xlate_switch(nv,s,_tmp810,_tmp811);return;_LL370: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp812=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp7F1;if(_tmp812->tag != 11)goto
_LL372;else{_tmp813=_tmp812->f1;_tmp814=_tmp812->f2;}}_LL371: {struct Cyc_Toc_Env
_tmp834;struct Cyc_Toc_FallthruInfo*_tmp835;struct Cyc_Toc_Env*_tmp833=nv;_tmp834=*
_tmp833;_tmp835=_tmp834.fallthru_info;if(_tmp835 == 0){const char*_tmpD23;void*
_tmpD22;(_tmpD22=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD23="fallthru in unexpected place",_tag_dyneither(_tmpD23,sizeof(char),29))),
_tag_dyneither(_tmpD22,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp839;struct
Cyc_List_List*_tmp83A;struct Cyc_Dict_Dict _tmp83B;struct Cyc_Toc_FallthruInfo
_tmp838=*_tmp835;_tmp839=_tmp838.label;_tmp83A=_tmp838.binders;_tmp83B=_tmp838.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp839,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp814)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp83C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp83A);struct Cyc_List_List*_tmp83D=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp813);for(0;_tmp83C != 0;(_tmp83C=_tmp83C->tl,
_tmp83D=_tmp83D->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp83D))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp83B,(
struct _tuple0*)_tmp83C->hd),(struct Cyc_Absyn_Exp*)_tmp83D->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL372: {struct Cyc_Absyn_Decl_s_struct*_tmp815=(struct Cyc_Absyn_Decl_s_struct*)
_tmp7F1;if(_tmp815->tag != 12)goto _LL374;else{_tmp816=_tmp815->f1;_tmp817=_tmp815->f2;}}
_LL373:{void*_tmp83E=_tmp816->r;struct Cyc_Absyn_Vardecl*_tmp840;struct Cyc_Absyn_Pat*
_tmp842;struct Cyc_Absyn_Exp*_tmp843;struct Cyc_List_List*_tmp845;struct Cyc_Absyn_Fndecl*
_tmp847;struct Cyc_Absyn_Tvar*_tmp849;struct Cyc_Absyn_Vardecl*_tmp84A;int _tmp84B;
struct Cyc_Absyn_Exp*_tmp84C;struct Cyc_Absyn_Exp*_tmp84E;struct Cyc_Absyn_Tvar*
_tmp84F;struct Cyc_Absyn_Vardecl*_tmp850;_LL37D: {struct Cyc_Absyn_Var_d_struct*
_tmp83F=(struct Cyc_Absyn_Var_d_struct*)_tmp83E;if(_tmp83F->tag != 0)goto _LL37F;
else{_tmp840=_tmp83F->f1;}}_LL37E: {struct Cyc_Toc_Env _tmp852;struct _RegionHandle*
_tmp853;struct Cyc_Toc_Env*_tmp851=nv;_tmp852=*_tmp851;_tmp853=_tmp852.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD26;struct Cyc_Absyn_Local_b_struct*_tmpD25;
struct Cyc_Toc_Env*_tmp854=Cyc_Toc_add_varmap(_tmp853,nv,_tmp840->name,Cyc_Absyn_varb_exp(
_tmp840->name,(void*)((_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25[0]=((
_tmpD26.tag=4,((_tmpD26.f1=_tmp840,_tmpD26)))),_tmpD25)))),0));Cyc_Toc_local_decl_to_c(
_tmp854,_tmp854,_tmp840,_tmp817);}goto _LL37C;}_LL37F: {struct Cyc_Absyn_Let_d_struct*
_tmp841=(struct Cyc_Absyn_Let_d_struct*)_tmp83E;if(_tmp841->tag != 2)goto _LL381;
else{_tmp842=_tmp841->f1;_tmp843=_tmp841->f3;}}_LL380:{void*_tmp857=_tmp842->r;
struct Cyc_Absyn_Vardecl*_tmp859;struct Cyc_Absyn_Pat*_tmp85A;struct Cyc_Absyn_Pat
_tmp85B;void*_tmp85C;_LL38C: {struct Cyc_Absyn_Var_p_struct*_tmp858=(struct Cyc_Absyn_Var_p_struct*)
_tmp857;if(_tmp858->tag != 1)goto _LL38E;else{_tmp859=_tmp858->f1;_tmp85A=_tmp858->f2;
_tmp85B=*_tmp85A;_tmp85C=_tmp85B.r;{struct Cyc_Absyn_Wild_p_struct*_tmp85D=(
struct Cyc_Absyn_Wild_p_struct*)_tmp85C;if(_tmp85D->tag != 0)goto _LL38E;};}}_LL38D: {
struct _tuple0*old_name=_tmp859->name;struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp859->name=new_name;_tmp859->initializer=(struct Cyc_Absyn_Exp*)_tmp843;{
struct Cyc_Absyn_Var_d_struct _tmpD29;struct Cyc_Absyn_Var_d_struct*_tmpD28;_tmp816->r=(
void*)((_tmpD28=_cycalloc(sizeof(*_tmpD28)),((_tmpD28[0]=((_tmpD29.tag=0,((
_tmpD29.f1=_tmp859,_tmpD29)))),_tmpD28))));}{struct Cyc_Toc_Env _tmp861;struct
_RegionHandle*_tmp862;struct Cyc_Toc_Env*_tmp860=nv;_tmp861=*_tmp860;_tmp862=
_tmp861.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD2C;struct Cyc_Absyn_Local_b_struct*
_tmpD2B;struct Cyc_Toc_Env*_tmp863=Cyc_Toc_add_varmap(_tmp862,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B[0]=((_tmpD2C.tag=
4,((_tmpD2C.f1=_tmp859,_tmpD2C)))),_tmpD2B)))),0));Cyc_Toc_local_decl_to_c(
_tmp863,nv,_tmp859,_tmp817);}goto _LL38B;};}_LL38E:;_LL38F: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp842,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp843->topt))->v,_tmp843,
_tmp817))->r;goto _LL38B;_LL38B:;}goto _LL37C;_LL381: {struct Cyc_Absyn_Letv_d_struct*
_tmp844=(struct Cyc_Absyn_Letv_d_struct*)_tmp83E;if(_tmp844->tag != 3)goto _LL383;
else{_tmp845=_tmp844->f1;}}_LL382: {struct Cyc_List_List*_tmp866=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp845);if(_tmp866 == 0){const char*_tmpD2F;
void*_tmpD2E;(_tmpD2E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD2F="empty Letv_d",_tag_dyneither(_tmpD2F,sizeof(char),
13))),_tag_dyneither(_tmpD2E,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD32;struct Cyc_Absyn_Var_d_struct*_tmpD31;_tmp816->r=(void*)((_tmpD31=
_cycalloc(sizeof(*_tmpD31)),((_tmpD31[0]=((_tmpD32.tag=0,((_tmpD32.f1=(struct Cyc_Absyn_Vardecl*)
_tmp866->hd,_tmpD32)))),_tmpD31))));}_tmp866=_tmp866->tl;for(0;_tmp866 != 0;
_tmp866=_tmp866->tl){struct Cyc_Absyn_Var_d_struct _tmpD35;struct Cyc_Absyn_Var_d_struct*
_tmpD34;struct Cyc_Absyn_Decl*_tmp86B=Cyc_Absyn_new_decl((void*)((_tmpD34=
_cycalloc(sizeof(*_tmpD34)),((_tmpD34[0]=((_tmpD35.tag=0,((_tmpD35.f1=(struct Cyc_Absyn_Vardecl*)
_tmp866->hd,_tmpD35)))),_tmpD34)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp86B,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL37C;}_LL383: {struct Cyc_Absyn_Fn_d_struct*
_tmp846=(struct Cyc_Absyn_Fn_d_struct*)_tmp83E;if(_tmp846->tag != 1)goto _LL385;
else{_tmp847=_tmp846->f1;}}_LL384: {struct _tuple0*_tmp86E=_tmp847->name;struct
Cyc_Toc_Env _tmp870;struct _RegionHandle*_tmp871;struct Cyc_Toc_Env*_tmp86F=nv;
_tmp870=*_tmp86F;_tmp871=_tmp870.rgn;{struct Cyc_Toc_Env*_tmp872=Cyc_Toc_add_varmap(
_tmp871,nv,_tmp847->name,Cyc_Absyn_var_exp(_tmp86E,0));Cyc_Toc_fndecl_to_c(
_tmp872,_tmp847,0);Cyc_Toc_stmt_to_c(_tmp872,_tmp817);}goto _LL37C;}_LL385: {
struct Cyc_Absyn_Region_d_struct*_tmp848=(struct Cyc_Absyn_Region_d_struct*)
_tmp83E;if(_tmp848->tag != 4)goto _LL387;else{_tmp849=_tmp848->f1;_tmp84A=_tmp848->f2;
_tmp84B=_tmp848->f3;_tmp84C=_tmp848->f4;}}_LL386: {struct Cyc_Absyn_Stmt*_tmp873=
_tmp817;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp84A->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp875;struct _RegionHandle*_tmp876;struct Cyc_Toc_Env*
_tmp874=nv;_tmp875=*_tmp874;_tmp876=_tmp875.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp876,nv,x_var,x_exp),_tmp873);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp873,0))->r;
else{if(_tmp84C == 0){struct Cyc_Absyn_Exp*_tmpD38[1];struct Cyc_Absyn_Exp*_tmpD37[
1];struct Cyc_List_List*_tmpD36;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD36=
_cycalloc(sizeof(*_tmpD36)),((_tmpD36->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD36->tl=0,_tmpD36)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD37[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD37,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp873,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD38[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD38,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp84C);{struct Cyc_Absyn_Exp*
_tmpD3A[1];struct Cyc_Absyn_Exp*_tmpD39[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD39[1]=(struct Cyc_Absyn_Exp*)_tmp84C,((_tmpD39[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD39,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp873,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD3A[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD3A,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL387: {struct Cyc_Absyn_Alias_d_struct*_tmp84D=(struct Cyc_Absyn_Alias_d_struct*)
_tmp83E;if(_tmp84D->tag != 5)goto _LL389;else{_tmp84E=_tmp84D->f1;_tmp84F=_tmp84D->f2;
_tmp850=_tmp84D->f3;}}_LL388: {struct _tuple0*old_name=_tmp850->name;struct
_tuple0*new_name=Cyc_Toc_temp_var();_tmp850->name=new_name;_tmp850->initializer=(
struct Cyc_Absyn_Exp*)_tmp84E;{struct Cyc_Absyn_Decl_s_struct _tmpD49;struct Cyc_Absyn_Var_d_struct*
_tmpD48;struct Cyc_Absyn_Var_d_struct _tmpD47;struct Cyc_Absyn_Decl*_tmpD46;struct
Cyc_Absyn_Decl_s_struct*_tmpD45;s->r=(void*)((_tmpD45=_cycalloc(sizeof(*_tmpD45)),((
_tmpD45[0]=((_tmpD49.tag=12,((_tmpD49.f1=((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((
_tmpD46->r=(void*)((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48[0]=((_tmpD47.tag=
0,((_tmpD47.f1=_tmp850,_tmpD47)))),_tmpD48)))),((_tmpD46->loc=0,_tmpD46)))))),((
_tmpD49.f2=_tmp817,_tmpD49)))))),_tmpD45))));}{struct Cyc_Toc_Env _tmp882;struct
_RegionHandle*_tmp883;struct Cyc_Toc_Env*_tmp881=nv;_tmp882=*_tmp881;_tmp883=
_tmp882.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD4C;struct Cyc_Absyn_Local_b_struct*
_tmpD4B;struct Cyc_Toc_Env*_tmp884=Cyc_Toc_add_varmap(_tmp883,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B[0]=((_tmpD4C.tag=
4,((_tmpD4C.f1=_tmp850,_tmpD4C)))),_tmpD4B)))),0));Cyc_Toc_local_decl_to_c(
_tmp884,nv,_tmp850,_tmp817);}return;};}_LL389:;_LL38A: {const char*_tmpD4F;void*
_tmpD4E;(_tmpD4E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD4F="bad nested declaration within function",_tag_dyneither(_tmpD4F,sizeof(
char),39))),_tag_dyneither(_tmpD4E,sizeof(void*),0)));}_LL37C:;}return;_LL374: {
struct Cyc_Absyn_Label_s_struct*_tmp818=(struct Cyc_Absyn_Label_s_struct*)_tmp7F1;
if(_tmp818->tag != 13)goto _LL376;else{_tmp819=_tmp818->f1;_tmp81A=_tmp818->f2;}}
_LL375: s=_tmp81A;continue;_LL376: {struct Cyc_Absyn_Do_s_struct*_tmp81B=(struct
Cyc_Absyn_Do_s_struct*)_tmp7F1;if(_tmp81B->tag != 14)goto _LL378;else{_tmp81C=
_tmp81B->f1;_tmp81D=_tmp81B->f2;_tmp81E=_tmp81D.f1;}}_LL377: {struct Cyc_Toc_Env
_tmp88A;struct _RegionHandle*_tmp88B;struct Cyc_Toc_Env*_tmp889=nv;_tmp88A=*
_tmp889;_tmp88B=_tmp88A.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp88B,nv),
_tmp81C);Cyc_Toc_exp_to_c(nv,_tmp81E);return;}_LL378: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp81F=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp7F1;if(_tmp81F->tag != 15)goto
_LL37A;else{_tmp820=_tmp81F->f1;_tmp821=_tmp81F->f2;}}_LL379: {struct _tuple0*
h_var=Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpD50;
e_exp->topt=((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50->v=e_typ,_tmpD50))));}{
struct Cyc_Toc_Env _tmp88E;struct _RegionHandle*_tmp88F;struct Cyc_Toc_Env*_tmp88D=
nv;_tmp88E=*_tmp88D;_tmp88F=_tmp88E.rgn;{struct Cyc_Toc_Env*_tmp890=Cyc_Toc_add_varmap(
_tmp88F,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp890,_tmp820);{struct Cyc_Absyn_Stmt*
_tmp891=Cyc_Absyn_seq_stmt(_tmp820,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp892=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp893=
Cyc_Absyn_var_exp(_tmp892,0);struct Cyc_Absyn_Vardecl*_tmp894=Cyc_Absyn_new_vardecl(
_tmp892,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpD51;_tmp893->topt=((
_tmpD51=_cycalloc(sizeof(*_tmpD51)),((_tmpD51->v=Cyc_Absyn_exn_typ(),_tmpD51))));}{
struct Cyc_Core_Opt*_tmpD62;struct Cyc_Absyn_Var_p_struct*_tmpD61;struct Cyc_Absyn_Pat*
_tmpD60;struct Cyc_Core_Opt*_tmpD5F;struct Cyc_Absyn_Var_p_struct _tmpD5E;struct Cyc_Absyn_Pat*
_tmpD5D;struct Cyc_Absyn_Pat*_tmp896=(_tmpD5D=_cycalloc(sizeof(*_tmpD5D)),((
_tmpD5D->r=(void*)((_tmpD61=_cycalloc(sizeof(*_tmpD61)),((_tmpD61[0]=((_tmpD5E.tag=
1,((_tmpD5E.f1=_tmp894,((_tmpD5E.f2=((_tmpD60=_cycalloc(sizeof(*_tmpD60)),((
_tmpD60->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD60->topt=((_tmpD5F=_cycalloc(
sizeof(*_tmpD5F)),((_tmpD5F->v=Cyc_Absyn_exn_typ(),_tmpD5F)))),((_tmpD60->loc=0,
_tmpD60)))))))),_tmpD5E)))))),_tmpD61)))),((_tmpD5D->topt=((_tmpD62=_cycalloc(
sizeof(*_tmpD62)),((_tmpD62->v=Cyc_Absyn_exn_typ(),_tmpD62)))),((_tmpD5D->loc=0,
_tmpD5D)))))));struct Cyc_Absyn_Exp*_tmp897=Cyc_Absyn_throw_exp(_tmp893,0);{
struct Cyc_Core_Opt*_tmpD63;_tmp897->topt=((_tmpD63=_cycalloc(sizeof(*_tmpD63)),((
_tmpD63->v=(void*)& Cyc_Absyn_VoidType_val,_tmpD63))));}{struct Cyc_Absyn_Stmt*
_tmp899=Cyc_Absyn_exp_stmt(_tmp897,0);struct Cyc_Core_Opt*_tmpD69;struct Cyc_List_List*
_tmpD68;struct Cyc_Absyn_Switch_clause*_tmpD67;struct Cyc_Absyn_Switch_clause*
_tmp89A=(_tmpD67=_cycalloc(sizeof(*_tmpD67)),((_tmpD67->pattern=_tmp896,((
_tmpD67->pat_vars=((_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69->v=((_tmpD68=
_cycalloc(sizeof(*_tmpD68)),((_tmpD68->hd=_tmp894,((_tmpD68->tl=0,_tmpD68)))))),
_tmpD69)))),((_tmpD67->where_clause=0,((_tmpD67->body=_tmp899,((_tmpD67->loc=0,
_tmpD67)))))))))));struct Cyc_List_List*_tmpD6A;struct Cyc_Absyn_Stmt*_tmp89B=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp821,((_tmpD6A=_cycalloc(sizeof(*_tmpD6A)),((_tmpD6A->hd=_tmp89A,((_tmpD6A->tl=
0,_tmpD6A))))))),0);Cyc_Toc_stmt_to_c(_tmp890,_tmp89B);{struct Cyc_List_List*
_tmpD6B;struct Cyc_Absyn_Exp*_tmp89C=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpD6B->tl=0,_tmpD6B)))))),0);struct Cyc_List_List*
_tmpD6C;struct Cyc_Absyn_Stmt*_tmp89D=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpD6C->tl=0,_tmpD6C)))))),0),0);struct Cyc_Absyn_Exp*_tmp89E=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp89F=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp89D,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp89E,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp89C,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp89F,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp891,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp89B,0),0),0),0),0),0))->r;};};};};}
return;};}_LL37A: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp822=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp7F1;if(_tmp822->tag != 16)goto _LL359;else{_tmp823=_tmp822->f1;}}_LL37B: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp823);{struct Cyc_List_List*
_tmpD6D;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpD6D=_cycalloc(sizeof(*_tmpD6D)),((_tmpD6D->hd=_tmp823,((_tmpD6D->tl=0,
_tmpD6D)))))),0));};}return;_LL359:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8AD=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8AD;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8AE=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8AF=f->args;for(0;_tmp8AF != 0;_tmp8AF=_tmp8AF->tl){struct
_tuple0*_tmpD6E;struct _tuple0*_tmp8B0=(_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((
_tmpD6E->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD6E->f2=(*((struct
_tuple25*)_tmp8AF->hd)).f1,_tmpD6E)))));(*((struct _tuple25*)_tmp8AF->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8AF->hd)).f3);_tmp8AE=Cyc_Toc_add_varmap(frgn,_tmp8AE,
_tmp8B0,Cyc_Absyn_var_exp(_tmp8B0,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8B3;struct Cyc_Absyn_Tqual
_tmp8B4;void*_tmp8B5;int _tmp8B6;struct Cyc_Absyn_VarargInfo _tmp8B2=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8B3=_tmp8B2.name;_tmp8B4=_tmp8B2.tq;_tmp8B5=
_tmp8B2.type;_tmp8B6=_tmp8B2.inject;{void*_tmp8B7=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8B5,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8B4,Cyc_Absyn_false_conref));struct
_tuple0*_tmpD6F;struct _tuple0*_tmp8B8=(_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((
_tmpD6F->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD6F->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8B3))->v,_tmpD6F)))));{
struct _tuple25*_tmpD72;struct Cyc_List_List*_tmpD71;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD71=
_cycalloc(sizeof(*_tmpD71)),((_tmpD71->hd=((_tmpD72=_cycalloc(sizeof(*_tmpD72)),((
_tmpD72->f1=(struct _dyneither_ptr*)_tmp8B3->v,((_tmpD72->f2=_tmp8B4,((_tmpD72->f3=
_tmp8B7,_tmpD72)))))))),((_tmpD71->tl=0,_tmpD71)))))));}_tmp8AE=Cyc_Toc_add_varmap(
frgn,_tmp8AE,_tmp8B8,Cyc_Absyn_var_exp(_tmp8B8,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp8BC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8BC != 0;_tmp8BC=_tmp8BC->tl){((struct Cyc_Absyn_Vardecl*)_tmp8BC->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8BC->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8AE),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s);static enum Cyc_Absyn_Scope
Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL390: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL391: return Cyc_Absyn_Extern;
default: _LL392: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad);static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple0*
_tmp8BD=ad->name;struct _DynRegionHandle*_tmp8BF;struct Cyc_Dict_Dict*_tmp8C0;
struct Cyc_Toc_TocState _tmp8BE=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8BF=_tmp8BE.dyn;_tmp8C0=_tmp8BE.aggrs_so_far;{struct _DynRegionFrame _tmp8C1;
struct _RegionHandle*d=_open_dynregion(& _tmp8C1,_tmp8BF);{int seen_defn_before;
struct _tuple14**_tmp8C2=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*
k))Cyc_Dict_lookup_opt)(*_tmp8C0,_tmp8BD);if(_tmp8C2 == 0){seen_defn_before=0;{
struct _tuple14*v;if(ad->kind == Cyc_Absyn_StructA){struct _tuple14*_tmpD73;v=((
_tmpD73=_region_malloc(d,sizeof(*_tmpD73)),((_tmpD73->f1=ad,((_tmpD73->f2=Cyc_Absyn_strctq(
_tmp8BD),_tmpD73))))));}else{struct _tuple14*_tmpD74;v=((_tmpD74=_region_malloc(d,
sizeof(*_tmpD74)),((_tmpD74->f1=ad,((_tmpD74->f2=Cyc_Absyn_unionq_typ(_tmp8BD),
_tmpD74))))));}*_tmp8C0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8C0,_tmp8BD,v);};}else{struct
_tuple14 _tmp8C6;struct Cyc_Absyn_Aggrdecl*_tmp8C7;void*_tmp8C8;struct _tuple14*
_tmp8C5=*_tmp8C2;_tmp8C6=*_tmp8C5;_tmp8C7=_tmp8C6.f1;_tmp8C8=_tmp8C6.f2;if(
_tmp8C7->impl == 0){{struct _tuple14*_tmpD75;*_tmp8C0=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp8C0,
_tmp8BD,((_tmpD75=_region_malloc(d,sizeof(*_tmpD75)),((_tmpD75->f1=ad,((_tmpD75->f2=
_tmp8C8,_tmpD75)))))));}seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*
_tmpD76;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD76=_cycalloc(sizeof(*_tmpD76)),((
_tmpD76->kind=ad->kind,((_tmpD76->sc=Cyc_Absyn_Public,((_tmpD76->name=ad->name,((
_tmpD76->tvs=0,((_tmpD76->impl=0,((_tmpD76->attributes=ad->attributes,_tmpD76)))))))))))));
if(ad->impl != 0  && !seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpD77;
new_ad->impl=((_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77->exist_vars=0,((
_tmpD77->rgn_po=0,((_tmpD77->fields=0,((_tmpD77->tagged=0,_tmpD77))))))))));}{
struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*_tmp8CB=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp8CB != 0;_tmp8CB=_tmp8CB->tl){struct Cyc_Absyn_Aggrfield*
_tmp8CC=(struct Cyc_Absyn_Aggrfield*)_tmp8CB->hd;struct Cyc_Absyn_Aggrfield*
_tmpD78;struct Cyc_Absyn_Aggrfield*_tmp8CD=(_tmpD78=_cycalloc(sizeof(*_tmpD78)),((
_tmpD78->name=_tmp8CC->name,((_tmpD78->tq=Cyc_Toc_mt_tq,((_tmpD78->type=Cyc_Toc_typ_to_c_array(
_tmp8CC->type),((_tmpD78->width=_tmp8CC->width,((_tmpD78->attributes=_tmp8CC->attributes,
_tmpD78)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp8CE=_tmp8CD->type;struct _dyneither_ptr*_tmp8CF=_tmp8CD->name;const char*
_tmpD7D;void*_tmpD7C[2];struct Cyc_String_pa_struct _tmpD7B;struct Cyc_String_pa_struct
_tmpD7A;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD7A.tag=0,((_tmpD7A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8CF),((_tmpD7B.tag=0,((_tmpD7B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD7C[0]=&
_tmpD7B,((_tmpD7C[1]=& _tmpD7A,Cyc_aprintf(((_tmpD7D="_union_%s_%s",
_tag_dyneither(_tmpD7D,sizeof(char),13))),_tag_dyneither(_tmpD7C,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD7E;struct _dyneither_ptr*str=(_tmpD7E=_cycalloc(sizeof(*
_tmpD7E)),((_tmpD7E[0]=s,_tmpD7E)));struct Cyc_Absyn_Aggrfield*_tmpD7F;struct Cyc_Absyn_Aggrfield*
_tmp8D0=(_tmpD7F=_cycalloc(sizeof(*_tmpD7F)),((_tmpD7F->name=Cyc_Toc_val_sp,((
_tmpD7F->tq=Cyc_Toc_mt_tq,((_tmpD7F->type=_tmp8CE,((_tmpD7F->width=0,((_tmpD7F->attributes=
0,_tmpD7F)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD80;struct Cyc_Absyn_Aggrfield*
_tmp8D1=(_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80->name=Cyc_Toc_tag_sp,((
_tmpD80->tq=Cyc_Toc_mt_tq,((_tmpD80->type=Cyc_Absyn_sint_typ,((_tmpD80->width=0,((
_tmpD80->attributes=0,_tmpD80)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD81[2];
struct Cyc_List_List*_tmp8D2=(_tmpD81[1]=_tmp8D0,((_tmpD81[0]=_tmp8D1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD81,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpD86;struct _tuple0*_tmpD85;struct Cyc_Absyn_Aggrdecl*
_tmpD84;struct Cyc_Absyn_Aggrdecl*_tmp8D3=(_tmpD84=_cycalloc(sizeof(*_tmpD84)),((
_tmpD84->kind=Cyc_Absyn_StructA,((_tmpD84->sc=Cyc_Absyn_Public,((_tmpD84->name=((
_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85->f1=Cyc_Absyn_Loc_n,((_tmpD85->f2=
str,_tmpD85)))))),((_tmpD84->tvs=0,((_tmpD84->impl=((_tmpD86=_cycalloc(sizeof(*
_tmpD86)),((_tmpD86->exist_vars=0,((_tmpD86->rgn_po=0,((_tmpD86->fields=_tmp8D2,((
_tmpD86->tagged=0,_tmpD86)))))))))),((_tmpD84->attributes=0,_tmpD84)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpD8C;struct Cyc_Absyn_Aggr_d_struct _tmpD8B;
struct Cyc_List_List*_tmpD8A;Cyc_Toc_result_decls=((_tmpD8A=_cycalloc(sizeof(*
_tmpD8A)),((_tmpD8A->hd=Cyc_Absyn_new_decl((void*)((_tmpD8C=_cycalloc(sizeof(*
_tmpD8C)),((_tmpD8C[0]=((_tmpD8B.tag=6,((_tmpD8B.f1=_tmp8D3,_tmpD8B)))),_tmpD8C)))),
0),((_tmpD8A->tl=Cyc_Toc_result_decls,_tmpD8A))))));}_tmp8CD->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpD8D;new_fields=((_tmpD8D=_cycalloc(sizeof(*
_tmpD8D)),((_tmpD8D->hd=_tmp8CD,((_tmpD8D->tl=new_fields,_tmpD8D))))));};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}{struct Cyc_Absyn_Decl*_tmpD97;struct Cyc_Absyn_Aggr_d_struct
_tmpD96;struct Cyc_Absyn_Aggr_d_struct*_tmpD95;struct Cyc_List_List*_tmpD94;Cyc_Toc_result_decls=((
_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94->hd=((_tmpD97=_cycalloc(sizeof(*
_tmpD97)),((_tmpD97->r=(void*)((_tmpD95=_cycalloc(sizeof(*_tmpD95)),((_tmpD95[0]=((
_tmpD96.tag=6,((_tmpD96.f1=new_ad,_tmpD96)))),_tmpD95)))),((_tmpD97->loc=0,
_tmpD97)))))),((_tmpD94->tl=Cyc_Toc_result_decls,_tmpD94))))));};};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud);static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp8EA;struct Cyc_Set_Set**_tmp8EB;struct Cyc_Toc_TocState
_tmp8E9=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8EA=
_tmp8E9.dyn;_tmp8EB=_tmp8E9.datatypes_so_far;{struct _DynRegionFrame _tmp8EC;
struct _RegionHandle*d=_open_dynregion(& _tmp8EC,_tmp8EA);{struct _tuple0*_tmp8ED=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))
Cyc_Set_member)(*_tmp8EB,_tmp8ED)){_npop_handler(0);return;}*_tmp8EB=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*
_tmp8EB,_tmp8ED);}{struct Cyc_List_List*_tmp8EE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp8EE != 0;_tmp8EE=_tmp8EE->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8EE->hd;struct Cyc_List_List*_tmp8EF=0;int i=
1;{struct Cyc_List_List*_tmp8F0=f->typs;for(0;_tmp8F0 != 0;(_tmp8F0=_tmp8F0->tl,i
++)){struct _dyneither_ptr*_tmp8F1=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpD98;struct Cyc_Absyn_Aggrfield*_tmp8F2=(_tmpD98=_cycalloc(sizeof(*_tmpD98)),((
_tmpD98->name=_tmp8F1,((_tmpD98->tq=(*((struct _tuple9*)_tmp8F0->hd)).f1,((
_tmpD98->type=Cyc_Toc_typ_to_c_array((*((struct _tuple9*)_tmp8F0->hd)).f2),((
_tmpD98->width=0,((_tmpD98->attributes=0,_tmpD98)))))))))));struct Cyc_List_List*
_tmpD99;_tmp8EF=((_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99->hd=_tmp8F2,((
_tmpD99->tl=_tmp8EF,_tmpD99))))));}}{struct Cyc_Absyn_Aggrfield*_tmpD9C;struct Cyc_List_List*
_tmpD9B;_tmp8EF=((_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B->hd=((_tmpD9C=
_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C->name=Cyc_Toc_tag_sp,((_tmpD9C->tq=Cyc_Toc_mt_tq,((
_tmpD9C->type=Cyc_Absyn_sint_typ,((_tmpD9C->width=0,((_tmpD9C->attributes=0,
_tmpD9C)))))))))))),((_tmpD9B->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp8EF),_tmpD9B))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDA1;const char*_tmpDA0;struct Cyc_Absyn_Aggrdecl*_tmpD9F;struct Cyc_Absyn_Aggrdecl*
_tmp8F7=(_tmpD9F=_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F->kind=Cyc_Absyn_StructA,((
_tmpD9F->sc=Cyc_Absyn_Public,((_tmpD9F->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDA0="_struct",_tag_dyneither(_tmpDA0,sizeof(char),8)))),((_tmpD9F->tvs=0,((
_tmpD9F->impl=((_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1->exist_vars=0,((
_tmpDA1->rgn_po=0,((_tmpDA1->fields=_tmp8EF,((_tmpDA1->tagged=0,_tmpDA1)))))))))),((
_tmpD9F->attributes=0,_tmpD9F)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDA7;
struct Cyc_Absyn_Aggr_d_struct _tmpDA6;struct Cyc_List_List*_tmpDA5;Cyc_Toc_result_decls=((
_tmpDA5=_cycalloc(sizeof(*_tmpDA5)),((_tmpDA5->hd=Cyc_Absyn_new_decl((void*)((
_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7[0]=((_tmpDA6.tag=6,((_tmpDA6.f1=
_tmp8F7,_tmpDA6)))),_tmpDA7)))),0),((_tmpDA5->tl=Cyc_Toc_result_decls,_tmpDA5))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd);static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp8FF;struct Cyc_Dict_Dict*_tmp900;struct
Cyc_Toc_TocState _tmp8FE=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8FF=_tmp8FE.dyn;_tmp900=_tmp8FE.xdatatypes_so_far;{struct _DynRegionFrame
_tmp901;struct _RegionHandle*d=_open_dynregion(& _tmp901,_tmp8FF);{struct _tuple0*
_tmp902=xd->name;struct Cyc_List_List*_tmp903=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp903 != 0;_tmp903=_tmp903->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp903->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp904=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp905=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp904,Cyc_Absyn_false_conref,0);int*_tmp906=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp900,f->name);int
_tmp907;_LL395: if(_tmp906 != 0)goto _LL397;_LL396: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp908=Cyc_Absyn_new_vardecl(f->name,_tmp905,
initopt);_tmp908->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDAD;struct Cyc_Absyn_Var_d_struct
_tmpDAC;struct Cyc_List_List*_tmpDAB;Cyc_Toc_result_decls=((_tmpDAB=_cycalloc(
sizeof(*_tmpDAB)),((_tmpDAB->hd=Cyc_Absyn_new_decl((void*)((_tmpDAD=_cycalloc(
sizeof(*_tmpDAD)),((_tmpDAD[0]=((_tmpDAC.tag=0,((_tmpDAC.f1=_tmp908,_tmpDAC)))),
_tmpDAD)))),0),((_tmpDAB->tl=Cyc_Toc_result_decls,_tmpDAB))))));}*_tmp900=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp900,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp90C=f->typs;for(0;_tmp90C != 0;(_tmp90C=_tmp90C->tl,i ++)){
struct Cyc_Int_pa_struct _tmpDB5;void*_tmpDB4[1];const char*_tmpDB3;struct
_dyneither_ptr*_tmpDB2;struct _dyneither_ptr*_tmp90D=(_tmpDB2=_cycalloc(sizeof(*
_tmpDB2)),((_tmpDB2[0]=(struct _dyneither_ptr)((_tmpDB5.tag=1,((_tmpDB5.f1=(
unsigned long)i,((_tmpDB4[0]=& _tmpDB5,Cyc_aprintf(((_tmpDB3="f%d",_tag_dyneither(
_tmpDB3,sizeof(char),4))),_tag_dyneither(_tmpDB4,sizeof(void*),1)))))))),_tmpDB2)));
struct Cyc_Absyn_Aggrfield*_tmpDB6;struct Cyc_Absyn_Aggrfield*_tmp90E=(_tmpDB6=
_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->name=_tmp90D,((_tmpDB6->tq=(*((struct
_tuple9*)_tmp90C->hd)).f1,((_tmpDB6->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple9*)_tmp90C->hd)).f2),((_tmpDB6->width=0,((_tmpDB6->attributes=0,_tmpDB6)))))))))));
struct Cyc_List_List*_tmpDB7;fields=((_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((
_tmpDB7->hd=_tmp90E,((_tmpDB7->tl=fields,_tmpDB7))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpDBA;struct Cyc_List_List*_tmpDB9;fields=((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((
_tmpDB9->hd=((_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->name=Cyc_Toc_tag_sp,((
_tmpDBA->tq=Cyc_Toc_mt_tq,((_tmpDBA->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpDBA->width=0,((_tmpDBA->attributes=0,_tmpDBA)))))))))))),((
_tmpDB9->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpDB9))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpDBF;const char*_tmpDBE;
struct Cyc_Absyn_Aggrdecl*_tmpDBD;struct Cyc_Absyn_Aggrdecl*_tmp917=(_tmpDBD=
_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->kind=Cyc_Absyn_StructA,((_tmpDBD->sc=Cyc_Absyn_Public,((
_tmpDBD->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpDBE="_struct",
_tag_dyneither(_tmpDBE,sizeof(char),8)))),((_tmpDBD->tvs=0,((_tmpDBD->impl=((
_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF->exist_vars=0,((_tmpDBF->rgn_po=0,((
_tmpDBF->fields=fields,((_tmpDBF->tagged=0,_tmpDBF)))))))))),((_tmpDBD->attributes=
0,_tmpDBD)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpDC5;struct Cyc_Absyn_Aggr_d_struct
_tmpDC4;struct Cyc_List_List*_tmpDC3;Cyc_Toc_result_decls=((_tmpDC3=_cycalloc(
sizeof(*_tmpDC3)),((_tmpDC3->hd=Cyc_Absyn_new_decl((void*)((_tmpDC5=_cycalloc(
sizeof(*_tmpDC5)),((_tmpDC5[0]=((_tmpDC4.tag=6,((_tmpDC4.f1=_tmp917,_tmpDC4)))),
_tmpDC5)))),0),((_tmpDC3->tl=Cyc_Toc_result_decls,_tmpDC3))))));}goto _LL394;};};};}
_LL397: if(_tmp906 == 0)goto _LL399;_tmp907=*_tmp906;if(_tmp907 != 0)goto _LL399;
_LL398: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp91E=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp91F=Cyc_Absyn_new_vardecl(f->name,_tmp905,(
struct Cyc_Absyn_Exp*)_tmp91E);_tmp91F->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpDCB;struct Cyc_Absyn_Var_d_struct _tmpDCA;struct Cyc_List_List*_tmpDC9;Cyc_Toc_result_decls=((
_tmpDC9=_cycalloc(sizeof(*_tmpDC9)),((_tmpDC9->hd=Cyc_Absyn_new_decl((void*)((
_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB[0]=((_tmpDCA.tag=0,((_tmpDCA.f1=
_tmp91F,_tmpDCA)))),_tmpDCB)))),0),((_tmpDC9->tl=Cyc_Toc_result_decls,_tmpDC9))))));}*
_tmp900=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*
_tmp900,f->name,1);}goto _LL394;_LL399:;_LL39A: goto _LL394;_LL394:;}};
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
void*_tmp923=init->r;struct Cyc_Absyn_Vardecl*_tmp925;struct Cyc_Absyn_Exp*_tmp926;
struct Cyc_Absyn_Exp*_tmp927;int _tmp928;_LL39C: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp924=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp923;if(_tmp924->tag != 28)
goto _LL39E;else{_tmp925=_tmp924->f1;_tmp926=_tmp924->f2;_tmp927=_tmp924->f3;
_tmp928=_tmp924->f4;}}_LL39D: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp925,_tmp926,_tmp927,_tmp928,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL39B;_LL39E:;_LL39F: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp92A;struct _RegionHandle*_tmp92B;struct Cyc_Toc_Env*_tmp929=init_nv;_tmp92A=*
_tmp929;_tmp92B=_tmp92A.rgn;{struct Cyc_Toc_Env*_tmp92C=Cyc_Toc_set_toplevel(
_tmp92B,init_nv);Cyc_Toc_exp_to_c(_tmp92C,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL39B;_LL39B:;}else{void*_tmp92D=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp92F;void*_tmp930;struct Cyc_Absyn_Exp*_tmp931;union Cyc_Absyn_Constraint*
_tmp932;_LL3A1:{struct Cyc_Absyn_ArrayType_struct*_tmp92E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp92D;if(_tmp92E->tag != 9)goto _LL3A3;else{_tmp92F=_tmp92E->f1;_tmp930=_tmp92F.elt_type;
_tmp931=_tmp92F.num_elts;_tmp932=_tmp92F.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp932))goto _LL3A3;_LL3A2: if(_tmp931 == 0){const char*
_tmpDCE;void*_tmpDCD;(_tmpDCD=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDCE="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpDCE,sizeof(char),55))),_tag_dyneither(_tmpDCD,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp931;struct Cyc_Absyn_Exp*
_tmp935=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp936=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp935,
_tmp936,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3A0;};_LL3A3:;_LL3A4: goto _LL3A0;
_LL3A0:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp938;struct _RegionHandle*
_tmp939;struct Cyc_Toc_Env*_tmp937=nv;_tmp938=*_tmp937;_tmp939=_tmp938.rgn;{
struct Cyc_Absyn_Stmt*_tmp93A=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp93B=Cyc_Toc_share_env(_tmp939,nv);struct Cyc_Toc_Env*_tmp93D;struct Cyc_List_List*
_tmp93E;struct Cyc_Absyn_Stmt*_tmp93F;struct _tuple21 _tmp93C=Cyc_Toc_xlate_pat(
_tmp93B,_tmp939,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp93D=_tmp93C.f1;_tmp93E=_tmp93C.f2;_tmp93F=_tmp93C.f3;Cyc_Toc_stmt_to_c(
_tmp93D,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp93F,s,0),0);for(0;_tmp93E != 0;_tmp93E=_tmp93E->tl){
struct _tuple22 _tmp941;struct _tuple0*_tmp942;void*_tmp943;struct _tuple22*_tmp940=(
struct _tuple22*)_tmp93E->hd;_tmp941=*_tmp940;_tmp942=_tmp941.f1;_tmp943=_tmp941.f2;
s=Cyc_Absyn_declare_stmt(_tmp942,_tmp943,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e);static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp944=e->r;struct Cyc_Absyn_Exp*_tmp946;struct Cyc_Absyn_Exp*_tmp948;struct
Cyc_Absyn_Exp*_tmp94A;struct Cyc_Absyn_Exp*_tmp94C;struct Cyc_Absyn_Exp*_tmp94E;
struct Cyc_Absyn_Exp*_tmp950;struct Cyc_Absyn_Exp*_tmp952;struct Cyc_Absyn_Exp*
_tmp954;struct Cyc_List_List*_tmp956;struct Cyc_Absyn_Exp*_tmp958;struct Cyc_Absyn_Exp*
_tmp959;struct Cyc_Absyn_Exp*_tmp95B;struct Cyc_Absyn_Exp*_tmp95C;struct Cyc_Absyn_Exp*
_tmp95E;struct Cyc_Absyn_Exp*_tmp95F;struct Cyc_Absyn_Exp*_tmp961;struct Cyc_Absyn_Exp*
_tmp962;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*_tmp965;struct Cyc_Absyn_Exp*
_tmp967;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_Absyn_Exp*
_tmp96B;struct Cyc_Absyn_Exp*_tmp96C;struct Cyc_Absyn_Exp*_tmp96E;struct Cyc_List_List*
_tmp96F;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_List_List*_tmp972;void*_tmp974;
void**_tmp975;struct Cyc_Absyn_Exp*_tmp976;struct _tuple7*_tmp978;struct _tuple7
_tmp979;void*_tmp97A;void**_tmp97B;struct Cyc_List_List*_tmp97C;struct Cyc_List_List*
_tmp97E;struct Cyc_List_List*_tmp980;void*_tmp982;void**_tmp983;void*_tmp985;void**
_tmp986;struct Cyc_Absyn_Stmt*_tmp988;struct Cyc_Absyn_MallocInfo _tmp98A;struct Cyc_Absyn_MallocInfo*
_tmp98B;_LL3A6: {struct Cyc_Absyn_Deref_e_struct*_tmp945=(struct Cyc_Absyn_Deref_e_struct*)
_tmp944;if(_tmp945->tag != 21)goto _LL3A8;else{_tmp946=_tmp945->f1;}}_LL3A7:
_tmp948=_tmp946;goto _LL3A9;_LL3A8: {struct Cyc_Absyn_AggrMember_e_struct*_tmp947=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp944;if(_tmp947->tag != 22)goto _LL3AA;
else{_tmp948=_tmp947->f1;}}_LL3A9: _tmp94A=_tmp948;goto _LL3AB;_LL3AA: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp949=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp944;if(_tmp949->tag != 23)goto
_LL3AC;else{_tmp94A=_tmp949->f1;}}_LL3AB: _tmp94C=_tmp94A;goto _LL3AD;_LL3AC: {
struct Cyc_Absyn_Address_e_struct*_tmp94B=(struct Cyc_Absyn_Address_e_struct*)
_tmp944;if(_tmp94B->tag != 16)goto _LL3AE;else{_tmp94C=_tmp94B->f1;}}_LL3AD:
_tmp94E=_tmp94C;goto _LL3AF;_LL3AE: {struct Cyc_Absyn_Throw_e_struct*_tmp94D=(
struct Cyc_Absyn_Throw_e_struct*)_tmp944;if(_tmp94D->tag != 12)goto _LL3B0;else{
_tmp94E=_tmp94D->f1;}}_LL3AF: _tmp950=_tmp94E;goto _LL3B1;_LL3B0: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp94F=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp944;if(_tmp94F->tag != 13)
goto _LL3B2;else{_tmp950=_tmp94F->f1;}}_LL3B1: _tmp952=_tmp950;goto _LL3B3;_LL3B2: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp951=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp944;if(_tmp951->tag != 19)goto _LL3B4;else{_tmp952=_tmp951->f1;}}_LL3B3:
_tmp954=_tmp952;goto _LL3B5;_LL3B4: {struct Cyc_Absyn_Increment_e_struct*_tmp953=(
struct Cyc_Absyn_Increment_e_struct*)_tmp944;if(_tmp953->tag != 5)goto _LL3B6;else{
_tmp954=_tmp953->f1;}}_LL3B5: Cyc_Toc_exptypes_to_c(_tmp954);goto _LL3A5;_LL3B6: {
struct Cyc_Absyn_Primop_e_struct*_tmp955=(struct Cyc_Absyn_Primop_e_struct*)
_tmp944;if(_tmp955->tag != 3)goto _LL3B8;else{_tmp956=_tmp955->f2;}}_LL3B7:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp956);goto _LL3A5;_LL3B8: {struct Cyc_Absyn_And_e_struct*_tmp957=(struct Cyc_Absyn_And_e_struct*)
_tmp944;if(_tmp957->tag != 7)goto _LL3BA;else{_tmp958=_tmp957->f1;_tmp959=_tmp957->f2;}}
_LL3B9: _tmp95B=_tmp958;_tmp95C=_tmp959;goto _LL3BB;_LL3BA: {struct Cyc_Absyn_Or_e_struct*
_tmp95A=(struct Cyc_Absyn_Or_e_struct*)_tmp944;if(_tmp95A->tag != 8)goto _LL3BC;
else{_tmp95B=_tmp95A->f1;_tmp95C=_tmp95A->f2;}}_LL3BB: _tmp95E=_tmp95B;_tmp95F=
_tmp95C;goto _LL3BD;_LL3BC: {struct Cyc_Absyn_SeqExp_e_struct*_tmp95D=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp944;if(_tmp95D->tag != 9)goto _LL3BE;else{_tmp95E=_tmp95D->f1;_tmp95F=_tmp95D->f2;}}
_LL3BD: _tmp961=_tmp95E;_tmp962=_tmp95F;goto _LL3BF;_LL3BE: {struct Cyc_Absyn_Subscript_e_struct*
_tmp960=(struct Cyc_Absyn_Subscript_e_struct*)_tmp944;if(_tmp960->tag != 24)goto
_LL3C0;else{_tmp961=_tmp960->f1;_tmp962=_tmp960->f2;}}_LL3BF: _tmp964=_tmp961;
_tmp965=_tmp962;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_Swap_e_struct*_tmp963=(
struct Cyc_Absyn_Swap_e_struct*)_tmp944;if(_tmp963->tag != 35)goto _LL3C2;else{
_tmp964=_tmp963->f1;_tmp965=_tmp963->f2;}}_LL3C1: _tmp967=_tmp964;_tmp968=_tmp965;
goto _LL3C3;_LL3C2: {struct Cyc_Absyn_AssignOp_e_struct*_tmp966=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp944;if(_tmp966->tag != 4)goto _LL3C4;else{_tmp967=_tmp966->f1;_tmp968=_tmp966->f3;}}
_LL3C3: Cyc_Toc_exptypes_to_c(_tmp967);Cyc_Toc_exptypes_to_c(_tmp968);goto _LL3A5;
_LL3C4: {struct Cyc_Absyn_Conditional_e_struct*_tmp969=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp944;if(_tmp969->tag != 6)goto _LL3C6;else{_tmp96A=_tmp969->f1;_tmp96B=_tmp969->f2;
_tmp96C=_tmp969->f3;}}_LL3C5: Cyc_Toc_exptypes_to_c(_tmp96A);Cyc_Toc_exptypes_to_c(
_tmp96B);Cyc_Toc_exptypes_to_c(_tmp96C);goto _LL3A5;_LL3C6: {struct Cyc_Absyn_FnCall_e_struct*
_tmp96D=(struct Cyc_Absyn_FnCall_e_struct*)_tmp944;if(_tmp96D->tag != 11)goto
_LL3C8;else{_tmp96E=_tmp96D->f1;_tmp96F=_tmp96D->f2;}}_LL3C7: _tmp971=_tmp96E;
_tmp972=_tmp96F;goto _LL3C9;_LL3C8: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp970=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp944;if(_tmp970->tag != 10)goto _LL3CA;
else{_tmp971=_tmp970->f1;_tmp972=_tmp970->f2;}}_LL3C9: Cyc_Toc_exptypes_to_c(
_tmp971);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp972);goto _LL3A5;_LL3CA: {struct Cyc_Absyn_Cast_e_struct*
_tmp973=(struct Cyc_Absyn_Cast_e_struct*)_tmp944;if(_tmp973->tag != 15)goto _LL3CC;
else{_tmp974=(void**)& _tmp973->f1;_tmp975=(void**)((void**)& _tmp973->f1);_tmp976=
_tmp973->f2;}}_LL3CB:*_tmp975=Cyc_Toc_typ_to_c(*_tmp975);Cyc_Toc_exptypes_to_c(
_tmp976);goto _LL3A5;_LL3CC: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp977=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp944;if(_tmp977->tag != 26)goto _LL3CE;
else{_tmp978=_tmp977->f1;_tmp979=*_tmp978;_tmp97A=_tmp979.f3;_tmp97B=(void**)&(*
_tmp977->f1).f3;_tmp97C=_tmp977->f2;}}_LL3CD:*_tmp97B=Cyc_Toc_typ_to_c(*_tmp97B);
_tmp97E=_tmp97C;goto _LL3CF;_LL3CE: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp97D=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp944;if(_tmp97D->tag != 36)
goto _LL3D0;else{_tmp97E=_tmp97D->f2;}}_LL3CF: _tmp980=_tmp97E;goto _LL3D1;_LL3D0: {
struct Cyc_Absyn_Array_e_struct*_tmp97F=(struct Cyc_Absyn_Array_e_struct*)_tmp944;
if(_tmp97F->tag != 27)goto _LL3D2;else{_tmp980=_tmp97F->f1;}}_LL3D1: for(0;_tmp980
!= 0;_tmp980=_tmp980->tl){struct Cyc_Absyn_Exp*_tmp99B;struct _tuple15 _tmp99A=*((
struct _tuple15*)_tmp980->hd);_tmp99B=_tmp99A.f2;Cyc_Toc_exptypes_to_c(_tmp99B);}
goto _LL3A5;_LL3D2: {struct Cyc_Absyn_Offsetof_e_struct*_tmp981=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp944;if(_tmp981->tag != 20)goto _LL3D4;else{_tmp982=(void**)& _tmp981->f1;
_tmp983=(void**)((void**)& _tmp981->f1);}}_LL3D3: _tmp986=_tmp983;goto _LL3D5;
_LL3D4: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp984=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp944;if(_tmp984->tag != 18)goto _LL3D6;else{_tmp985=(void**)& _tmp984->f1;
_tmp986=(void**)((void**)& _tmp984->f1);}}_LL3D5:*_tmp986=Cyc_Toc_typ_to_c(*
_tmp986);goto _LL3A5;_LL3D6: {struct Cyc_Absyn_StmtExp_e_struct*_tmp987=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp944;if(_tmp987->tag != 37)goto _LL3D8;else{_tmp988=_tmp987->f1;}}_LL3D7: Cyc_Toc_stmttypes_to_c(
_tmp988);goto _LL3A5;_LL3D8: {struct Cyc_Absyn_Malloc_e_struct*_tmp989=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp944;if(_tmp989->tag != 34)goto _LL3DA;else{_tmp98A=_tmp989->f1;_tmp98B=(struct
Cyc_Absyn_MallocInfo*)& _tmp989->f1;}}_LL3D9: if(_tmp98B->elt_type != 0){void**
_tmpDCF;_tmp98B->elt_type=((_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp98B->elt_type))),_tmpDCF))));}Cyc_Toc_exptypes_to_c(
_tmp98B->num_elts);goto _LL3A5;_LL3DA: {struct Cyc_Absyn_Const_e_struct*_tmp98C=(
struct Cyc_Absyn_Const_e_struct*)_tmp944;if(_tmp98C->tag != 0)goto _LL3DC;}_LL3DB:
goto _LL3DD;_LL3DC: {struct Cyc_Absyn_Var_e_struct*_tmp98D=(struct Cyc_Absyn_Var_e_struct*)
_tmp944;if(_tmp98D->tag != 1)goto _LL3DE;}_LL3DD: goto _LL3DF;_LL3DE: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp98E=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp944;if(_tmp98E->tag != 2)goto
_LL3E0;}_LL3DF: goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Enum_e_struct*_tmp98F=(struct
Cyc_Absyn_Enum_e_struct*)_tmp944;if(_tmp98F->tag != 32)goto _LL3E2;}_LL3E1: goto
_LL3E3;_LL3E2: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp990=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp944;if(_tmp990->tag != 33)goto _LL3E4;}_LL3E3: goto _LL3A5;_LL3E4: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp991=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp944;if(_tmp991->tag != 30)goto
_LL3E6;}_LL3E5: goto _LL3E7;_LL3E6: {struct Cyc_Absyn_Datatype_e_struct*_tmp992=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp944;if(_tmp992->tag != 31)goto _LL3E8;}
_LL3E7: goto _LL3E9;_LL3E8: {struct Cyc_Absyn_Aggregate_e_struct*_tmp993=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp944;if(_tmp993->tag != 29)goto _LL3EA;}_LL3E9: goto _LL3EB;_LL3EA: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp994=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp944;if(_tmp994->tag != 28)
goto _LL3EC;}_LL3EB: goto _LL3ED;_LL3EC: {struct Cyc_Absyn_Tuple_e_struct*_tmp995=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp944;if(_tmp995->tag != 25)goto _LL3EE;}_LL3ED:
goto _LL3EF;_LL3EE: {struct Cyc_Absyn_Instantiate_e_struct*_tmp996=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp944;if(_tmp996->tag != 14)goto _LL3F0;}_LL3EF: goto _LL3F1;_LL3F0: {struct Cyc_Absyn_New_e_struct*
_tmp997=(struct Cyc_Absyn_New_e_struct*)_tmp944;if(_tmp997->tag != 17)goto _LL3F2;}
_LL3F1: goto _LL3F3;_LL3F2: {struct Cyc_Absyn_Valueof_e_struct*_tmp998=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp944;if(_tmp998->tag != 39)goto _LL3F4;}_LL3F3: goto _LL3F5;_LL3F4: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp999=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp944;if(_tmp999->tag != 38)goto
_LL3A5;}_LL3F5:{const char*_tmpDD2;void*_tmpDD1;(_tmpDD1=0,Cyc_Tcutil_terr(e->loc,((
_tmpDD2="Cyclone expression within C code",_tag_dyneither(_tmpDD2,sizeof(char),
33))),_tag_dyneither(_tmpDD1,sizeof(void*),0)));}goto _LL3A5;_LL3A5:;}static void
Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d);static void Cyc_Toc_decltypes_to_c(
struct Cyc_Absyn_Decl*d){void*_tmp99F=d->r;struct Cyc_Absyn_Vardecl*_tmp9A1;struct
Cyc_Absyn_Fndecl*_tmp9A3;struct Cyc_Absyn_Aggrdecl*_tmp9A5;struct Cyc_Absyn_Enumdecl*
_tmp9A7;struct Cyc_Absyn_Typedefdecl*_tmp9A9;_LL3F7: {struct Cyc_Absyn_Var_d_struct*
_tmp9A0=(struct Cyc_Absyn_Var_d_struct*)_tmp99F;if(_tmp9A0->tag != 0)goto _LL3F9;
else{_tmp9A1=_tmp9A0->f1;}}_LL3F8: _tmp9A1->type=Cyc_Toc_typ_to_c(_tmp9A1->type);
if(_tmp9A1->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp9A1->initializer));goto _LL3F6;_LL3F9: {struct Cyc_Absyn_Fn_d_struct*
_tmp9A2=(struct Cyc_Absyn_Fn_d_struct*)_tmp99F;if(_tmp9A2->tag != 1)goto _LL3FB;
else{_tmp9A3=_tmp9A2->f1;}}_LL3FA: _tmp9A3->ret_type=Cyc_Toc_typ_to_c(_tmp9A3->ret_type);{
struct Cyc_List_List*_tmp9B5=_tmp9A3->args;for(0;_tmp9B5 != 0;_tmp9B5=_tmp9B5->tl){(*((
struct _tuple25*)_tmp9B5->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9B5->hd)).f3);}}
goto _LL3F6;_LL3FB: {struct Cyc_Absyn_Aggr_d_struct*_tmp9A4=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp99F;if(_tmp9A4->tag != 6)goto _LL3FD;else{_tmp9A5=_tmp9A4->f1;}}_LL3FC: Cyc_Toc_aggrdecl_to_c(
_tmp9A5);goto _LL3F6;_LL3FD: {struct Cyc_Absyn_Enum_d_struct*_tmp9A6=(struct Cyc_Absyn_Enum_d_struct*)
_tmp99F;if(_tmp9A6->tag != 8)goto _LL3FF;else{_tmp9A7=_tmp9A6->f1;}}_LL3FE: if(
_tmp9A7->fields != 0){struct Cyc_List_List*_tmp9B6=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9A7->fields))->v;for(0;_tmp9B6 != 0;_tmp9B6=_tmp9B6->tl){
struct Cyc_Absyn_Enumfield*_tmp9B7=(struct Cyc_Absyn_Enumfield*)_tmp9B6->hd;if(
_tmp9B7->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9B7->tag));}}
goto _LL3F6;_LL3FF: {struct Cyc_Absyn_Typedef_d_struct*_tmp9A8=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp99F;if(_tmp9A8->tag != 9)goto _LL401;else{_tmp9A9=_tmp9A8->f1;}}_LL400:{struct
Cyc_Core_Opt*_tmpDD3;_tmp9A9->defn=((_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((
_tmpDD3->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9A9->defn))->v),_tmpDD3))));}goto _LL3F6;_LL401: {struct Cyc_Absyn_Let_d_struct*
_tmp9AA=(struct Cyc_Absyn_Let_d_struct*)_tmp99F;if(_tmp9AA->tag != 2)goto _LL403;}
_LL402: goto _LL404;_LL403: {struct Cyc_Absyn_Letv_d_struct*_tmp9AB=(struct Cyc_Absyn_Letv_d_struct*)
_tmp99F;if(_tmp9AB->tag != 3)goto _LL405;}_LL404: goto _LL406;_LL405: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9AC=(struct Cyc_Absyn_Datatype_d_struct*)_tmp99F;if(_tmp9AC->tag != 7)goto
_LL407;}_LL406: goto _LL408;_LL407: {struct Cyc_Absyn_Namespace_d_struct*_tmp9AD=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp99F;if(_tmp9AD->tag != 10)goto _LL409;}
_LL408: goto _LL40A;_LL409: {struct Cyc_Absyn_Using_d_struct*_tmp9AE=(struct Cyc_Absyn_Using_d_struct*)
_tmp99F;if(_tmp9AE->tag != 11)goto _LL40B;}_LL40A: goto _LL40C;_LL40B: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9AF=(struct Cyc_Absyn_ExternC_d_struct*)_tmp99F;if(_tmp9AF->tag != 12)goto
_LL40D;}_LL40C: goto _LL40E;_LL40D: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9B0=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp99F;if(_tmp9B0->tag != 13)
goto _LL40F;}_LL40E: goto _LL410;_LL40F: {struct Cyc_Absyn_Region_d_struct*_tmp9B1=(
struct Cyc_Absyn_Region_d_struct*)_tmp99F;if(_tmp9B1->tag != 4)goto _LL411;}_LL410:
goto _LL412;_LL411: {struct Cyc_Absyn_Alias_d_struct*_tmp9B2=(struct Cyc_Absyn_Alias_d_struct*)
_tmp99F;if(_tmp9B2->tag != 5)goto _LL413;}_LL412:{const char*_tmpDD6;void*_tmpDD5;(
_tmpDD5=0,Cyc_Tcutil_terr(d->loc,((_tmpDD6="Cyclone declaration within C code",
_tag_dyneither(_tmpDD6,sizeof(char),34))),_tag_dyneither(_tmpDD5,sizeof(void*),0)));}
goto _LL3F6;_LL413: {struct Cyc_Absyn_Porton_d_struct*_tmp9B3=(struct Cyc_Absyn_Porton_d_struct*)
_tmp99F;if(_tmp9B3->tag != 14)goto _LL415;}_LL414: goto _LL416;_LL415: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9B4=(struct Cyc_Absyn_Portoff_d_struct*)_tmp99F;if(_tmp9B4->tag != 15)goto
_LL3F6;}_LL416: goto _LL3F6;_LL3F6:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){void*_tmp9BB=s->r;
struct Cyc_Absyn_Exp*_tmp9BD;struct Cyc_Absyn_Stmt*_tmp9BF;struct Cyc_Absyn_Stmt*
_tmp9C0;struct Cyc_Absyn_Exp*_tmp9C2;struct Cyc_Absyn_Exp*_tmp9C4;struct Cyc_Absyn_Stmt*
_tmp9C5;struct Cyc_Absyn_Stmt*_tmp9C6;struct _tuple8 _tmp9C8;struct Cyc_Absyn_Exp*
_tmp9C9;struct Cyc_Absyn_Stmt*_tmp9CA;struct Cyc_Absyn_Exp*_tmp9CC;struct _tuple8
_tmp9CD;struct Cyc_Absyn_Exp*_tmp9CE;struct _tuple8 _tmp9CF;struct Cyc_Absyn_Exp*
_tmp9D0;struct Cyc_Absyn_Stmt*_tmp9D1;struct Cyc_Absyn_Exp*_tmp9D3;struct Cyc_List_List*
_tmp9D4;struct Cyc_Absyn_Decl*_tmp9D6;struct Cyc_Absyn_Stmt*_tmp9D7;struct Cyc_Absyn_Stmt*
_tmp9D9;struct _tuple8 _tmp9DA;struct Cyc_Absyn_Exp*_tmp9DB;_LL418: {struct Cyc_Absyn_Exp_s_struct*
_tmp9BC=(struct Cyc_Absyn_Exp_s_struct*)_tmp9BB;if(_tmp9BC->tag != 1)goto _LL41A;
else{_tmp9BD=_tmp9BC->f1;}}_LL419: Cyc_Toc_exptypes_to_c(_tmp9BD);goto _LL417;
_LL41A: {struct Cyc_Absyn_Seq_s_struct*_tmp9BE=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9BB;if(_tmp9BE->tag != 2)goto _LL41C;else{_tmp9BF=_tmp9BE->f1;_tmp9C0=_tmp9BE->f2;}}
_LL41B: Cyc_Toc_stmttypes_to_c(_tmp9BF);Cyc_Toc_stmttypes_to_c(_tmp9C0);goto
_LL417;_LL41C: {struct Cyc_Absyn_Return_s_struct*_tmp9C1=(struct Cyc_Absyn_Return_s_struct*)
_tmp9BB;if(_tmp9C1->tag != 3)goto _LL41E;else{_tmp9C2=_tmp9C1->f1;}}_LL41D: if(
_tmp9C2 != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9C2);goto _LL417;
_LL41E: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9C3=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9BB;if(_tmp9C3->tag != 4)goto _LL420;else{_tmp9C4=_tmp9C3->f1;_tmp9C5=_tmp9C3->f2;
_tmp9C6=_tmp9C3->f3;}}_LL41F: Cyc_Toc_exptypes_to_c(_tmp9C4);Cyc_Toc_stmttypes_to_c(
_tmp9C5);Cyc_Toc_stmttypes_to_c(_tmp9C6);goto _LL417;_LL420: {struct Cyc_Absyn_While_s_struct*
_tmp9C7=(struct Cyc_Absyn_While_s_struct*)_tmp9BB;if(_tmp9C7->tag != 5)goto _LL422;
else{_tmp9C8=_tmp9C7->f1;_tmp9C9=_tmp9C8.f1;_tmp9CA=_tmp9C7->f2;}}_LL421: Cyc_Toc_exptypes_to_c(
_tmp9C9);Cyc_Toc_stmttypes_to_c(_tmp9CA);goto _LL417;_LL422: {struct Cyc_Absyn_For_s_struct*
_tmp9CB=(struct Cyc_Absyn_For_s_struct*)_tmp9BB;if(_tmp9CB->tag != 9)goto _LL424;
else{_tmp9CC=_tmp9CB->f1;_tmp9CD=_tmp9CB->f2;_tmp9CE=_tmp9CD.f1;_tmp9CF=_tmp9CB->f3;
_tmp9D0=_tmp9CF.f1;_tmp9D1=_tmp9CB->f4;}}_LL423: Cyc_Toc_exptypes_to_c(_tmp9CC);
Cyc_Toc_exptypes_to_c(_tmp9CE);Cyc_Toc_exptypes_to_c(_tmp9D0);Cyc_Toc_stmttypes_to_c(
_tmp9D1);goto _LL417;_LL424: {struct Cyc_Absyn_Switch_s_struct*_tmp9D2=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9BB;if(_tmp9D2->tag != 10)goto _LL426;else{_tmp9D3=_tmp9D2->f1;_tmp9D4=_tmp9D2->f2;}}
_LL425: Cyc_Toc_exptypes_to_c(_tmp9D3);for(0;_tmp9D4 != 0;_tmp9D4=_tmp9D4->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmp9D4->hd)->body);}goto _LL417;_LL426: {struct
Cyc_Absyn_Decl_s_struct*_tmp9D5=(struct Cyc_Absyn_Decl_s_struct*)_tmp9BB;if(
_tmp9D5->tag != 12)goto _LL428;else{_tmp9D6=_tmp9D5->f1;_tmp9D7=_tmp9D5->f2;}}
_LL427: Cyc_Toc_decltypes_to_c(_tmp9D6);Cyc_Toc_stmttypes_to_c(_tmp9D7);goto
_LL417;_LL428: {struct Cyc_Absyn_Do_s_struct*_tmp9D8=(struct Cyc_Absyn_Do_s_struct*)
_tmp9BB;if(_tmp9D8->tag != 14)goto _LL42A;else{_tmp9D9=_tmp9D8->f1;_tmp9DA=_tmp9D8->f2;
_tmp9DB=_tmp9DA.f1;}}_LL429: Cyc_Toc_stmttypes_to_c(_tmp9D9);Cyc_Toc_exptypes_to_c(
_tmp9DB);goto _LL417;_LL42A: {struct Cyc_Absyn_Skip_s_struct*_tmp9DC=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9BB;if(_tmp9DC->tag != 0)goto _LL42C;}_LL42B: goto _LL42D;_LL42C: {struct Cyc_Absyn_Break_s_struct*
_tmp9DD=(struct Cyc_Absyn_Break_s_struct*)_tmp9BB;if(_tmp9DD->tag != 6)goto _LL42E;}
_LL42D: goto _LL42F;_LL42E: {struct Cyc_Absyn_Continue_s_struct*_tmp9DE=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9BB;if(_tmp9DE->tag != 7)goto _LL430;}_LL42F: goto _LL431;_LL430: {struct Cyc_Absyn_Goto_s_struct*
_tmp9DF=(struct Cyc_Absyn_Goto_s_struct*)_tmp9BB;if(_tmp9DF->tag != 8)goto _LL432;}
_LL431: goto _LL417;_LL432: {struct Cyc_Absyn_Fallthru_s_struct*_tmp9E0=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9BB;if(_tmp9E0->tag != 11)goto _LL434;}_LL433: goto _LL435;_LL434: {struct Cyc_Absyn_Label_s_struct*
_tmp9E1=(struct Cyc_Absyn_Label_s_struct*)_tmp9BB;if(_tmp9E1->tag != 13)goto _LL436;}
_LL435: goto _LL437;_LL436: {struct Cyc_Absyn_TryCatch_s_struct*_tmp9E2=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9BB;if(_tmp9E2->tag != 15)goto _LL438;}_LL437: goto _LL439;_LL438: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp9E3=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9BB;if(_tmp9E3->tag != 16)goto
_LL417;}_LL439:{const char*_tmpDD9;void*_tmpDD8;(_tmpDD8=0,Cyc_Tcutil_terr(s->loc,((
_tmpDD9="Cyclone statement in C code",_tag_dyneither(_tmpDD9,sizeof(char),28))),
_tag_dyneither(_tmpDD8,sizeof(void*),0)));}goto _LL417;_LL417:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude);static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){const char*_tmpDDC;void*
_tmpDDB;(_tmpDDB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDDC="decls_to_c: not at toplevel!",_tag_dyneither(_tmpDDC,sizeof(char),29))),
_tag_dyneither(_tmpDDB,sizeof(void*),0)));}{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp9E8=d->r;struct Cyc_Absyn_Vardecl*_tmp9EA;struct Cyc_Absyn_Fndecl*
_tmp9EC;struct Cyc_Absyn_Aggrdecl*_tmp9F2;struct Cyc_Absyn_Datatypedecl*_tmp9F4;
struct Cyc_Absyn_Enumdecl*_tmp9F6;struct Cyc_Absyn_Typedefdecl*_tmp9F8;struct Cyc_List_List*
_tmp9FC;struct Cyc_List_List*_tmp9FE;struct Cyc_List_List*_tmpA00;struct Cyc_List_List*
_tmpA02;_LL43B: {struct Cyc_Absyn_Var_d_struct*_tmp9E9=(struct Cyc_Absyn_Var_d_struct*)
_tmp9E8;if(_tmp9E9->tag != 0)goto _LL43D;else{_tmp9EA=_tmp9E9->f1;}}_LL43C: {
struct _tuple0*_tmpA03=_tmp9EA->name;if(_tmp9EA->sc == Cyc_Absyn_ExternC){struct
_tuple0*_tmpDDD;_tmpA03=((_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD->f1=Cyc_Absyn_Rel_n(
0),((_tmpDDD->f2=(*_tmpA03).f2,_tmpDDD))))));}if(_tmp9EA->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9EA->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9EA->initializer));}}{
struct Cyc_Absyn_Global_b_struct _tmpDE0;struct Cyc_Absyn_Global_b_struct*_tmpDDF;
nv=Cyc_Toc_add_varmap(r,nv,_tmp9EA->name,Cyc_Absyn_varb_exp(_tmpA03,(void*)((
_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF[0]=((_tmpDE0.tag=1,((_tmpDE0.f1=
_tmp9EA,_tmpDE0)))),_tmpDDF)))),0));}_tmp9EA->name=_tmpA03;_tmp9EA->sc=Cyc_Toc_scope_to_c(
_tmp9EA->sc);_tmp9EA->type=Cyc_Toc_typ_to_c_array(_tmp9EA->type);{struct Cyc_List_List*
_tmpDE1;Cyc_Toc_result_decls=((_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1->hd=
d,((_tmpDE1->tl=Cyc_Toc_result_decls,_tmpDE1))))));}goto _LL43A;}_LL43D: {struct
Cyc_Absyn_Fn_d_struct*_tmp9EB=(struct Cyc_Absyn_Fn_d_struct*)_tmp9E8;if(_tmp9EB->tag
!= 1)goto _LL43F;else{_tmp9EC=_tmp9EB->f1;}}_LL43E: {struct _tuple0*_tmpA08=
_tmp9EC->name;if(_tmp9EC->sc == Cyc_Absyn_ExternC){{struct _tuple0*_tmpDE2;_tmpA08=((
_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2->f1=Cyc_Absyn_Rel_n(0),((_tmpDE2->f2=(*
_tmpA08).f2,_tmpDE2))))));}_tmp9EC->sc=Cyc_Absyn_Public;}nv=Cyc_Toc_add_varmap(r,
nv,_tmp9EC->name,Cyc_Absyn_var_exp(_tmpA08,0));_tmp9EC->name=_tmpA08;Cyc_Toc_fndecl_to_c(
nv,_tmp9EC,cinclude);{struct Cyc_List_List*_tmpDE3;Cyc_Toc_result_decls=((_tmpDE3=
_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3->hd=d,((_tmpDE3->tl=Cyc_Toc_result_decls,
_tmpDE3))))));}goto _LL43A;}_LL43F: {struct Cyc_Absyn_Let_d_struct*_tmp9ED=(struct
Cyc_Absyn_Let_d_struct*)_tmp9E8;if(_tmp9ED->tag != 2)goto _LL441;}_LL440: goto
_LL442;_LL441: {struct Cyc_Absyn_Letv_d_struct*_tmp9EE=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9E8;if(_tmp9EE->tag != 3)goto _LL443;}_LL442: {const char*_tmpDE6;void*_tmpDE5;(
_tmpDE5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDE6="letdecl at toplevel",_tag_dyneither(_tmpDE6,sizeof(char),20))),
_tag_dyneither(_tmpDE5,sizeof(void*),0)));}_LL443: {struct Cyc_Absyn_Region_d_struct*
_tmp9EF=(struct Cyc_Absyn_Region_d_struct*)_tmp9E8;if(_tmp9EF->tag != 4)goto _LL445;}
_LL444: {const char*_tmpDE9;void*_tmpDE8;(_tmpDE8=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDE9="region decl at toplevel",
_tag_dyneither(_tmpDE9,sizeof(char),24))),_tag_dyneither(_tmpDE8,sizeof(void*),0)));}
_LL445: {struct Cyc_Absyn_Alias_d_struct*_tmp9F0=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9E8;if(_tmp9F0->tag != 5)goto _LL447;}_LL446: {const char*_tmpDEC;void*_tmpDEB;(
_tmpDEB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDEC="alias decl at toplevel",_tag_dyneither(_tmpDEC,sizeof(char),23))),
_tag_dyneither(_tmpDEB,sizeof(void*),0)));}_LL447: {struct Cyc_Absyn_Aggr_d_struct*
_tmp9F1=(struct Cyc_Absyn_Aggr_d_struct*)_tmp9E8;if(_tmp9F1->tag != 6)goto _LL449;
else{_tmp9F2=_tmp9F1->f1;}}_LL448: Cyc_Toc_aggrdecl_to_c(_tmp9F2);goto _LL43A;
_LL449: {struct Cyc_Absyn_Datatype_d_struct*_tmp9F3=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp9E8;if(_tmp9F3->tag != 7)goto _LL44B;else{_tmp9F4=_tmp9F3->f1;}}_LL44A: if(
_tmp9F4->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmp9F4);else{Cyc_Toc_datatypedecl_to_c(
_tmp9F4);}goto _LL43A;_LL44B: {struct Cyc_Absyn_Enum_d_struct*_tmp9F5=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9E8;if(_tmp9F5->tag != 8)goto _LL44D;else{_tmp9F6=_tmp9F5->f1;}}_LL44C: Cyc_Toc_enumdecl_to_c(
nv,_tmp9F6);{struct Cyc_List_List*_tmpDED;Cyc_Toc_result_decls=((_tmpDED=
_cycalloc(sizeof(*_tmpDED)),((_tmpDED->hd=d,((_tmpDED->tl=Cyc_Toc_result_decls,
_tmpDED))))));}goto _LL43A;_LL44D: {struct Cyc_Absyn_Typedef_d_struct*_tmp9F7=(
struct Cyc_Absyn_Typedef_d_struct*)_tmp9E8;if(_tmp9F7->tag != 9)goto _LL44F;else{
_tmp9F8=_tmp9F7->f1;}}_LL44E: _tmp9F8->name=_tmp9F8->name;_tmp9F8->tvs=0;if(
_tmp9F8->defn != 0){struct Cyc_Core_Opt*_tmpDEE;_tmp9F8->defn=((_tmpDEE=_cycalloc(
sizeof(*_tmpDEE)),((_tmpDEE->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9F8->defn))->v),_tmpDEE))));}else{switch(((struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(_tmp9F8->kind))->v)->kind){case Cyc_Absyn_BoxKind:
_LL45B:{struct Cyc_Core_Opt*_tmpDEF;_tmp9F8->defn=((_tmpDEF=_cycalloc(sizeof(*
_tmpDEF)),((_tmpDEF->v=Cyc_Absyn_void_star_typ(),_tmpDEF))));}break;default:
_LL45C:{struct Cyc_Core_Opt*_tmpDF0;_tmp9F8->defn=((_tmpDF0=_cycalloc(sizeof(*
_tmpDF0)),((_tmpDF0->v=(void*)& Cyc_Absyn_VoidType_val,_tmpDF0))));}break;}}{
struct Cyc_List_List*_tmpDF1;Cyc_Toc_result_decls=((_tmpDF1=_cycalloc(sizeof(*
_tmpDF1)),((_tmpDF1->hd=d,((_tmpDF1->tl=Cyc_Toc_result_decls,_tmpDF1))))));}goto
_LL43A;_LL44F: {struct Cyc_Absyn_Porton_d_struct*_tmp9F9=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9E8;if(_tmp9F9->tag != 14)goto _LL451;}_LL450: goto _LL452;_LL451: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9FA=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9E8;if(_tmp9FA->tag != 15)goto
_LL453;}_LL452: goto _LL43A;_LL453: {struct Cyc_Absyn_Namespace_d_struct*_tmp9FB=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9E8;if(_tmp9FB->tag != 10)goto _LL455;
else{_tmp9FC=_tmp9FB->f2;}}_LL454: _tmp9FE=_tmp9FC;goto _LL456;_LL455: {struct Cyc_Absyn_Using_d_struct*
_tmp9FD=(struct Cyc_Absyn_Using_d_struct*)_tmp9E8;if(_tmp9FD->tag != 11)goto _LL457;
else{_tmp9FE=_tmp9FD->f2;}}_LL456: _tmpA00=_tmp9FE;goto _LL458;_LL457: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9FF=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9E8;if(_tmp9FF->tag != 12)goto
_LL459;else{_tmpA00=_tmp9FF->f1;}}_LL458: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA00,top,
cinclude);goto _LL43A;_LL459: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA01=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9E8;if(_tmpA01->tag != 13)goto _LL43A;
else{_tmpA02=_tmpA01->f1;}}_LL45A: nv=Cyc_Toc_decls_to_c(r,nv,_tmpA02,top,1);goto
_LL43A;_LL43A:;};}return nv;}static void Cyc_Toc_init();static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmpA17;struct Cyc_Core_NewRegion _tmpA16=Cyc_Core_new_dynregion();
_tmpA17=_tmpA16.dynregion;{struct _DynRegionFrame _tmpA18;struct _RegionHandle*d=
_open_dynregion(& _tmpA18,_tmpA17);{struct Cyc_Dict_Dict*_tmpDFC;struct Cyc_Dict_Dict*
_tmpDFB;struct Cyc_Set_Set**_tmpDFA;struct Cyc_Dict_Dict*_tmpDF9;struct Cyc_List_List**
_tmpDF8;struct Cyc_Toc_TocState*_tmpDF7;Cyc_Toc_toc_state=((_tmpDF7=_cycalloc(
sizeof(*_tmpDF7)),((_tmpDF7->dyn=(struct _DynRegionHandle*)_tmpA17,((_tmpDF7->tuple_types=(
struct Cyc_List_List**)((_tmpDF8=_region_malloc(d,sizeof(*_tmpDF8)),((_tmpDF8[0]=
0,_tmpDF8)))),((_tmpDF7->aggrs_so_far=(struct Cyc_Dict_Dict*)((_tmpDF9=
_region_malloc(d,sizeof(*_tmpDF9)),((_tmpDF9[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpDF9)))),((_tmpDF7->datatypes_so_far=(struct Cyc_Set_Set**)((_tmpDFA=
_region_malloc(d,sizeof(*_tmpDFA)),((_tmpDFA[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpDFA)))),((_tmpDF7->xdatatypes_so_far=(struct Cyc_Dict_Dict*)((_tmpDFB=
_region_malloc(d,sizeof(*_tmpDFB)),((_tmpDFB[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpDFB)))),((_tmpDF7->qvar_tags=(struct Cyc_Dict_Dict*)((_tmpDFC=_region_malloc(
d,sizeof(*_tmpDFC)),((_tmpDFC[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _tuple13*,struct _tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),
_tmpDFC)))),((_tmpDF7->temp_labels=(struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpDF7))))))))))))))));}Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpDFD;Cyc_Toc_globals=_tag_dyneither(((_tmpDFD=_cycalloc(
sizeof(struct _dyneither_ptr*)* 38),((_tmpDFD[0]=& Cyc_Toc__throw_str,((_tmpDFD[1]=&
Cyc_Toc_setjmp_str,((_tmpDFD[2]=& Cyc_Toc__push_handler_str,((_tmpDFD[3]=& Cyc_Toc__pop_handler_str,((
_tmpDFD[4]=& Cyc_Toc__exn_thrown_str,((_tmpDFD[5]=& Cyc_Toc__npop_handler_str,((
_tmpDFD[6]=& Cyc_Toc__check_null_str,((_tmpDFD[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpDFD[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpDFD[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpDFD[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpDFD[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpDFD[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpDFD[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpDFD[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpDFD[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpDFD[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpDFD[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpDFD[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpDFD[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpDFD[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpDFD[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpDFD[22]=& Cyc_Toc__cycalloc_str,((_tmpDFD[23]=& Cyc_Toc__cyccalloc_str,((
_tmpDFD[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpDFD[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpDFD[26]=& Cyc_Toc__region_malloc_str,((_tmpDFD[27]=& Cyc_Toc__region_calloc_str,((
_tmpDFD[28]=& Cyc_Toc__check_times_str,((_tmpDFD[29]=& Cyc_Toc__new_region_str,((
_tmpDFD[30]=& Cyc_Toc__push_region_str,((_tmpDFD[31]=& Cyc_Toc__pop_region_str,((
_tmpDFD[32]=& Cyc_Toc__open_dynregion_str,((_tmpDFD[33]=& Cyc_Toc__push_dynregion_str,((
_tmpDFD[34]=& Cyc_Toc__pop_dynregion_str,((_tmpDFD[35]=& Cyc_Toc__reset_region_str,((
_tmpDFD[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpDFD[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,
_tmpDFD)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),38);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*
ds){Cyc_Toc_init();{struct _RegionHandle _tmpA20=_new_region("start");struct
_RegionHandle*start=& _tmpA20;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmpA22;struct Cyc_Toc_TocState _tmpA21=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA22=_tmpA21.dyn;Cyc_Core_free_dynregion(
_tmpA22);};{struct Cyc_List_List*_tmpA23=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA23;};;
_pop_region(start);};}
