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
extern int _throw_match();
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
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,
unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * 
sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern
char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*tag;};
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_list_cmp(int(*cmp)(void*,
void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_join(struct
Cyc_Position_Segment*,struct Cyc_Position_Segment*);struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};struct _union_Nmspace_Rel_n{int tag;
struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);union
Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,
struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU
field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;
struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int
tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);union Cyc_Absyn_AggrInfoU
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
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
int tag;struct _dyneither_ptr f1;};extern struct Cyc_Absyn_Stdcall_att_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_struct Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_struct
Cyc_Absyn_Fastcall_att_val;extern struct Cyc_Absyn_Noreturn_att_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_struct Cyc_Absyn_Const_att_val;extern struct Cyc_Absyn_Packed_att_struct
Cyc_Absyn_Packed_att_val;extern struct Cyc_Absyn_Nocommon_att_struct Cyc_Absyn_Nocommon_att_val;
extern struct Cyc_Absyn_Shared_att_struct Cyc_Absyn_Shared_att_val;extern struct Cyc_Absyn_Unused_att_struct
Cyc_Absyn_Unused_att_val;extern struct Cyc_Absyn_Weak_att_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_struct Cyc_Absyn_Dllimport_att_val;extern
struct Cyc_Absyn_Dllexport_att_struct Cyc_Absyn_Dllexport_att_val;extern struct Cyc_Absyn_No_instrument_function_att_struct
Cyc_Absyn_No_instrument_function_att_val;extern struct Cyc_Absyn_Constructor_att_struct
Cyc_Absyn_Constructor_att_val;extern struct Cyc_Absyn_Destructor_att_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_struct{
int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
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
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;union
Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(
enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,
int);union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr);union Cyc_Absyn_Cnst
Cyc_Absyn_String_c(struct _dyneither_ptr);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 
0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,
Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte
 = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand
 = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift
 = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{
Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec
 = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*
injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion
 = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_struct{int tag;union Cyc_Absyn_Cnst f1;};
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop
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
_dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_struct{
int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*
topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Skip_s_struct{
int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;};struct Cyc_Absyn_TryCatch_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Skip_s_struct Cyc_Absyn_Skip_s_val;
struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
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
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*
r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};extern struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;
struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
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
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};extern struct Cyc_Absyn_Porton_d_struct
Cyc_Absyn_Porton_d_val;extern struct Cyc_Absyn_Portoff_d_struct Cyc_Absyn_Portoff_d_val;
struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};extern struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val;int
Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*
x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*
Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);extern union
Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);struct Cyc_Absyn_Kind*
Cyc_Absyn_force_kb(void*kb);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct
Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);void*Cyc_Absyn_int_typ(
enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);extern void*Cyc_Absyn_char_typ;extern
void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;
extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(
int);extern void*Cyc_Absyn_empty_effect;extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;void*Cyc_Absyn_exn_typ();
extern struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar;extern struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar;
void*Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);
void*Cyc_Absyn_file_typ();extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_atb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union
Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_cstar_typ(void*t,struct
Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(
struct _dyneither_ptr*name);void*Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(
struct _tuple0*name);void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct
Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*
es,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct
Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
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
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int
volatile_kw,struct _dyneither_ptr body,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(
struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct
Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*
Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,
int,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,
struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int
tagged);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum 
Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct
_tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(
enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(void*,int
fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple9*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*,int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
int Cyc_Absyn_fntype_att(void*a);struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};struct _tuple10 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_nontagged_union_type(
void*);int Cyc_Absyn_is_aggr_type(void*t);extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);}
;int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*
k);extern int Cyc_Cyclone_tovc_r;enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 
0,Cyc_Cyclone_Vc_c  = 1};extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
int Cyc_Cyclone_tovc_r=0;enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
return((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,ss1,ss2);}int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){if((int)vs1 == (int)vs2)return 0;
return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple11{union Cyc_Absyn_Nmspace f1;
union Cyc_Absyn_Nmspace f2;};int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*
q2){union Cyc_Absyn_Nmspace _tmp0=(*q1).f1;union Cyc_Absyn_Nmspace _tmp1=(*q2).f1;{
struct _tuple11 _tmp223;struct _tuple11 _tmp3=(_tmp223.f1=_tmp0,((_tmp223.f2=_tmp1,
_tmp223)));union Cyc_Absyn_Nmspace _tmp4;int _tmp5;union Cyc_Absyn_Nmspace _tmp6;int
_tmp7;union Cyc_Absyn_Nmspace _tmp8;struct Cyc_List_List*_tmp9;union Cyc_Absyn_Nmspace
_tmpA;struct Cyc_List_List*_tmpB;union Cyc_Absyn_Nmspace _tmpC;struct Cyc_List_List*
_tmpD;union Cyc_Absyn_Nmspace _tmpE;struct Cyc_List_List*_tmpF;union Cyc_Absyn_Nmspace
_tmp10;struct Cyc_List_List*_tmp11;union Cyc_Absyn_Nmspace _tmp12;struct Cyc_List_List*
_tmp13;union Cyc_Absyn_Nmspace _tmp14;int _tmp15;union Cyc_Absyn_Nmspace _tmp16;int
_tmp17;union Cyc_Absyn_Nmspace _tmp18;struct Cyc_List_List*_tmp19;union Cyc_Absyn_Nmspace
_tmp1A;struct Cyc_List_List*_tmp1B;union Cyc_Absyn_Nmspace _tmp1C;struct Cyc_List_List*
_tmp1D;union Cyc_Absyn_Nmspace _tmp1E;struct Cyc_List_List*_tmp1F;_LL1: _tmp4=_tmp3.f1;
if((_tmp4.Loc_n).tag != 4)goto _LL3;_tmp5=(int)(_tmp4.Loc_n).val;_tmp6=_tmp3.f2;
if((_tmp6.Loc_n).tag != 4)goto _LL3;_tmp7=(int)(_tmp6.Loc_n).val;_LL2: goto _LL0;
_LL3: _tmp8=_tmp3.f1;if((_tmp8.Rel_n).tag != 1)goto _LL5;_tmp9=(struct Cyc_List_List*)(
_tmp8.Rel_n).val;_tmpA=_tmp3.f2;if((_tmpA.Rel_n).tag != 1)goto _LL5;_tmpB=(struct
Cyc_List_List*)(_tmpA.Rel_n).val;_LL4: _tmpD=_tmp9;_tmpF=_tmpB;goto _LL6;_LL5:
_tmpC=_tmp3.f1;if((_tmpC.Abs_n).tag != 2)goto _LL7;_tmpD=(struct Cyc_List_List*)(
_tmpC.Abs_n).val;_tmpE=_tmp3.f2;if((_tmpE.Abs_n).tag != 2)goto _LL7;_tmpF=(struct
Cyc_List_List*)(_tmpE.Abs_n).val;_LL6: _tmp11=_tmpD;_tmp13=_tmpF;goto _LL8;_LL7:
_tmp10=_tmp3.f1;if((_tmp10.C_n).tag != 3)goto _LL9;_tmp11=(struct Cyc_List_List*)(
_tmp10.C_n).val;_tmp12=_tmp3.f2;if((_tmp12.C_n).tag != 3)goto _LL9;_tmp13=(struct
Cyc_List_List*)(_tmp12.C_n).val;_LL8: {int i=Cyc_Absyn_strlist_cmp(_tmp11,_tmp13);
if(i != 0)return i;goto _LL0;}_LL9: _tmp14=_tmp3.f1;if((_tmp14.Loc_n).tag != 4)goto
_LLB;_tmp15=(int)(_tmp14.Loc_n).val;_LLA: return - 1;_LLB: _tmp16=_tmp3.f2;if((
_tmp16.Loc_n).tag != 4)goto _LLD;_tmp17=(int)(_tmp16.Loc_n).val;_LLC: return 1;_LLD:
_tmp18=_tmp3.f1;if((_tmp18.Rel_n).tag != 1)goto _LLF;_tmp19=(struct Cyc_List_List*)(
_tmp18.Rel_n).val;_LLE: return - 1;_LLF: _tmp1A=_tmp3.f2;if((_tmp1A.Rel_n).tag != 1)
goto _LL11;_tmp1B=(struct Cyc_List_List*)(_tmp1A.Rel_n).val;_LL10: return 1;_LL11:
_tmp1C=_tmp3.f1;if((_tmp1C.Abs_n).tag != 2)goto _LL13;_tmp1D=(struct Cyc_List_List*)(
_tmp1C.Abs_n).val;_LL12: return - 1;_LL13: _tmp1E=_tmp3.f2;if((_tmp1E.Abs_n).tag != 2)
goto _LL0;_tmp1F=(struct Cyc_List_List*)(_tmp1E.Abs_n).val;_LL14: return 1;_LL0:;}
return Cyc_strptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*
tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,tv2->name);if(i != 0)
return i;return tv1->identity - tv2->identity;}union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={
4,0}};union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x,int C_scope){
if(C_scope){union Cyc_Absyn_Nmspace _tmp224;return((_tmp224.C_n).val=x,(((_tmp224.C_n).tag=
3,_tmp224)));}else{union Cyc_Absyn_Nmspace _tmp225;return((_tmp225.Abs_n).val=x,(((
_tmp225.Abs_n).tag=2,_tmp225)));}}union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct
Cyc_List_List*x){union Cyc_Absyn_Nmspace _tmp226;return((_tmp226.Rel_n).val=x,(((
_tmp226.Rel_n).tag=1,_tmp226)));}union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={
1,0}};int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){union Cyc_Absyn_Nmspace
_tmp23=(*qv).f1;struct Cyc_List_List*_tmp24;struct Cyc_List_List*_tmp25;int _tmp26;
_LL16: if((_tmp23.Rel_n).tag != 1)goto _LL18;_tmp24=(struct Cyc_List_List*)(_tmp23.Rel_n).val;
if(_tmp24 != 0)goto _LL18;_LL17: goto _LL19;_LL18: if((_tmp23.Abs_n).tag != 2)goto
_LL1A;_tmp25=(struct Cyc_List_List*)(_tmp23.Abs_n).val;if(_tmp25 != 0)goto _LL1A;
_LL19: goto _LL1B;_LL1A: if((_tmp23.Loc_n).tag != 4)goto _LL1C;_tmp26=(int)(_tmp23.Loc_n).val;
_LL1B: return 0;_LL1C:;_LL1D: return 1;_LL15:;}static int Cyc_Absyn_new_type_counter=0;
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){struct Cyc_Absyn_Evar_struct
_tmp229;struct Cyc_Absyn_Evar_struct*_tmp228;return(void*)((_tmp228=_cycalloc(
sizeof(*_tmp228)),((_tmp228[0]=((_tmp229.tag=1,((_tmp229.f1=k,((_tmp229.f2=0,((
_tmp229.f3=Cyc_Absyn_new_type_counter ++,((_tmp229.f4=env,_tmp229)))))))))),
_tmp228))));}void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,tenv);}struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){struct Cyc_Absyn_Tqual _tmp22A;
return(_tmp22A.print_const=x.print_const  || y.print_const,((_tmp22A.q_volatile=x.q_volatile
 || y.q_volatile,((_tmp22A.q_restrict=x.q_restrict  || y.q_restrict,((_tmp22A.real_const=
x.real_const  || y.real_const,((_tmp22A.loc=Cyc_Position_segment_join(x.loc,y.loc),
_tmp22A)))))))));}struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Tqual _tmp22B;return(_tmp22B.print_const=0,((_tmp22B.q_volatile=
0,((_tmp22B.q_restrict=0,((_tmp22B.real_const=0,((_tmp22B.loc=loc,_tmp22B)))))))));}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Tqual _tmp22C;return(_tmp22C.print_const=1,((_tmp22C.q_volatile=0,((
_tmp22C.q_restrict=0,((_tmp22C.real_const=1,((_tmp22C.loc=loc,_tmp22C)))))))));}
struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={0,{.Int_c={4,{Cyc_Absyn_Unsigned,
1}}}};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(void*)& Cyc_Absyn_one_b_raw,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=&
Cyc_Absyn_exp_unsigned_one_v;static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={
1,& Cyc_Absyn_exp_unsigned_one_v};void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo
udi){union Cyc_Absyn_DatatypeInfoU _tmp22D;return((_tmp22D.UnknownDatatype).val=
udi,(((_tmp22D.UnknownDatatype).tag=1,_tmp22D)));}extern union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){union Cyc_Absyn_DatatypeInfoU
_tmp22E;return((_tmp22E.KnownDatatype).val=d,(((_tmp22E.KnownDatatype).tag=2,
_tmp22E)));}union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(
struct Cyc_Absyn_UnknownDatatypeFieldInfo s){union Cyc_Absyn_DatatypeFieldInfoU
_tmp22F;return((_tmp22F.UnknownDatatypefield).val=s,(((_tmp22F.UnknownDatatypefield).tag=
1,_tmp22F)));}union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(
struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){struct _tuple1
_tmp232;union Cyc_Absyn_DatatypeFieldInfoU _tmp231;return((_tmp231.KnownDatatypefield).val=((
_tmp232.f1=dd,((_tmp232.f2=df,_tmp232)))),(((_tmp231.KnownDatatypefield).tag=2,
_tmp231)));}union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind
ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){struct _tuple2 _tmp235;union Cyc_Absyn_AggrInfoU
_tmp234;return((_tmp234.UnknownAggr).val=((_tmp235.f1=ak,((_tmp235.f2=n,((
_tmp235.f3=tagged,_tmp235)))))),(((_tmp234.UnknownAggr).tag=1,_tmp234)));}union
Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){union Cyc_Absyn_AggrInfoU
_tmp236;return((_tmp236.KnownAggr).val=ad,(((_tmp236.KnownAggr).tag=2,_tmp236)));}
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x){union Cyc_Absyn_Constraint*
_tmp237;return(_tmp237=_cycalloc(sizeof(*_tmp237)),(((_tmp237->Eq_constr).val=(
void*)x,(((_tmp237->Eq_constr).tag=1,_tmp237)))));}union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref(){
union Cyc_Absyn_Constraint*_tmp238;return(_tmp238=_cycalloc_atomic(sizeof(*
_tmp238)),(((_tmp238->No_constr).val=0,(((_tmp238->No_constr).tag=3,_tmp238)))));}
struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val={0};static union Cyc_Absyn_Constraint
Cyc_Absyn_true_conref_v={.Eq_constr={1,(void*)1}};union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref=&
Cyc_Absyn_true_conref_v;static union Cyc_Absyn_Constraint Cyc_Absyn_false_conref_v={.Eq_constr={
1,(void*)0}};union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_one_conref_v={.Eq_constr={1,(
void*)((void*)& Cyc_Absyn_one_bt)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref=&
Cyc_Absyn_bounds_one_conref_v;static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_dyneither_conref_v={.Eq_constr={
1,(void*)((void*)& Cyc_Absyn_DynEither_b_val)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref=&
Cyc_Absyn_bounds_dyneither_conref_v;union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x){union Cyc_Absyn_Constraint*_tmp3A=x;union Cyc_Absyn_Constraint
_tmp3B;int _tmp3C;union Cyc_Absyn_Constraint _tmp3D;void*_tmp3E;union Cyc_Absyn_Constraint
_tmp3F;union Cyc_Absyn_Constraint*_tmp40;_LL1F: _tmp3B=*_tmp3A;if((_tmp3B.No_constr).tag
!= 3)goto _LL21;_tmp3C=(int)(_tmp3B.No_constr).val;_LL20: goto _LL22;_LL21: _tmp3D=*
_tmp3A;if((_tmp3D.Eq_constr).tag != 1)goto _LL23;_tmp3E=(void*)(_tmp3D.Eq_constr).val;
_LL22: return x;_LL23: _tmp3F=*_tmp3A;if((_tmp3F.Forward_constr).tag != 2)goto _LL1E;
_tmp40=(union Cyc_Absyn_Constraint*)(_tmp3F.Forward_constr).val;_LL24: {union Cyc_Absyn_Constraint*
_tmp41=Cyc_Absyn_compress_conref(_tmp40);{struct _union_Constraint_Forward_constr*
_tmp239;(_tmp239=& x->Forward_constr,((_tmp239->tag=2,_tmp239->val=_tmp41)));}
return _tmp41;}_LL1E:;}void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp43=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint
_tmp44;void*_tmp45;_LL26: _tmp44=*_tmp43;if((_tmp44.Eq_constr).tag != 1)goto _LL28;
_tmp45=(void*)(_tmp44.Eq_constr).val;_LL27: return _tmp45;_LL28:;_LL29: {const char*
_tmp23C;void*_tmp23B;(_tmp23B=0,Cyc_Tcutil_impos(((_tmp23C="conref_val",
_tag_dyneither(_tmp23C,sizeof(char),11))),_tag_dyneither(_tmp23B,sizeof(void*),0)));}
_LL25:;}void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x){union Cyc_Absyn_Constraint*
_tmp48=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint _tmp49;void*_tmp4A;
_LL2B: _tmp49=*_tmp48;if((_tmp49.Eq_constr).tag != 1)goto _LL2D;_tmp4A=(void*)(
_tmp49.Eq_constr).val;_LL2C: return _tmp4A;_LL2D:;_LL2E: return y;_LL2A:;}void*Cyc_Absyn_conref_constr(
void*y,union Cyc_Absyn_Constraint*x){x=Cyc_Absyn_compress_conref(x);{union Cyc_Absyn_Constraint*
_tmp4B=x;union Cyc_Absyn_Constraint _tmp4C;void*_tmp4D;_LL30: _tmp4C=*_tmp4B;if((
_tmp4C.Eq_constr).tag != 1)goto _LL32;_tmp4D=(void*)(_tmp4C.Eq_constr).val;_LL31:
return _tmp4D;_LL32:;_LL33:{struct _union_Constraint_Eq_constr*_tmp23D;(_tmp23D=& x->Eq_constr,((
_tmp23D->tag=1,_tmp23D->val=y)));}return y;_LL2F:;};}void*Cyc_Absyn_compress_kb(
void*k){void*_tmp4F=k;struct Cyc_Core_Opt*_tmp52;struct Cyc_Core_Opt*_tmp54;struct
Cyc_Core_Opt*_tmp56;struct Cyc_Core_Opt _tmp57;void*_tmp58;void**_tmp59;struct Cyc_Core_Opt*
_tmp5B;struct Cyc_Core_Opt _tmp5C;void*_tmp5D;void**_tmp5E;_LL35: {struct Cyc_Absyn_Eq_kb_struct*
_tmp50=(struct Cyc_Absyn_Eq_kb_struct*)_tmp4F;if(_tmp50->tag != 0)goto _LL37;}_LL36:
goto _LL38;_LL37: {struct Cyc_Absyn_Unknown_kb_struct*_tmp51=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp4F;if(_tmp51->tag != 1)goto _LL39;else{_tmp52=_tmp51->f1;if(_tmp52 != 0)goto
_LL39;}}_LL38: goto _LL3A;_LL39: {struct Cyc_Absyn_Less_kb_struct*_tmp53=(struct Cyc_Absyn_Less_kb_struct*)
_tmp4F;if(_tmp53->tag != 2)goto _LL3B;else{_tmp54=_tmp53->f1;if(_tmp54 != 0)goto
_LL3B;}}_LL3A: return k;_LL3B: {struct Cyc_Absyn_Unknown_kb_struct*_tmp55=(struct
Cyc_Absyn_Unknown_kb_struct*)_tmp4F;if(_tmp55->tag != 1)goto _LL3D;else{_tmp56=
_tmp55->f1;if(_tmp56 == 0)goto _LL3D;_tmp57=*_tmp56;_tmp58=(void*)_tmp57.v;_tmp59=(
void**)&(*_tmp55->f1).v;}}_LL3C: _tmp5E=_tmp59;goto _LL3E;_LL3D: {struct Cyc_Absyn_Less_kb_struct*
_tmp5A=(struct Cyc_Absyn_Less_kb_struct*)_tmp4F;if(_tmp5A->tag != 2)goto _LL34;
else{_tmp5B=_tmp5A->f1;if(_tmp5B == 0)goto _LL34;_tmp5C=*_tmp5B;_tmp5D=(void*)
_tmp5C.v;_tmp5E=(void**)&(*_tmp5A->f1).v;}}_LL3E:*_tmp5E=Cyc_Absyn_compress_kb(*
_tmp5E);return*_tmp5E;_LL34:;}struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_tmp5F=Cyc_Absyn_compress_kb(kb);struct Cyc_Absyn_Kind*_tmp61;struct Cyc_Core_Opt*
_tmp63;struct Cyc_Core_Opt**_tmp64;struct Cyc_Core_Opt*_tmp66;struct Cyc_Core_Opt**
_tmp67;struct Cyc_Absyn_Kind*_tmp68;_LL40: {struct Cyc_Absyn_Eq_kb_struct*_tmp60=(
struct Cyc_Absyn_Eq_kb_struct*)_tmp5F;if(_tmp60->tag != 0)goto _LL42;else{_tmp61=
_tmp60->f1;}}_LL41: return _tmp61;_LL42: {struct Cyc_Absyn_Unknown_kb_struct*_tmp62=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmp5F;if(_tmp62->tag != 1)goto _LL44;else{
_tmp63=_tmp62->f1;_tmp64=(struct Cyc_Core_Opt**)& _tmp62->f1;}}_LL43: _tmp67=_tmp64;
_tmp68=& Cyc_Tcutil_bk;goto _LL45;_LL44: {struct Cyc_Absyn_Less_kb_struct*_tmp65=(
struct Cyc_Absyn_Less_kb_struct*)_tmp5F;if(_tmp65->tag != 2)goto _LL3F;else{_tmp66=
_tmp65->f1;_tmp67=(struct Cyc_Core_Opt**)& _tmp65->f1;_tmp68=_tmp65->f2;}}_LL45:{
struct Cyc_Core_Opt*_tmp23E;*_tmp67=((_tmp23E=_cycalloc(sizeof(*_tmp23E)),((
_tmp23E->v=Cyc_Tcutil_kind_to_bound(_tmp68),_tmp23E))));}return _tmp68;_LL3F:;}
struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val={21};struct Cyc_Absyn_UniqueRgn_struct
Cyc_Absyn_UniqueRgn_val={22};struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val={
0};struct Cyc_Absyn_FloatType_struct Cyc_Absyn_FloatType_val={7};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_char_tt={6,Cyc_Absyn_None,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uchar_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ushort_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uint_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulong_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulonglong_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_LongLong_sz};void*Cyc_Absyn_char_typ=(
void*)& Cyc_Absyn_char_tt;void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;
void*Cyc_Absyn_ushort_typ=(void*)& Cyc_Absyn_ushort_tt;void*Cyc_Absyn_uint_typ=(
void*)& Cyc_Absyn_uint_tt;void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;
void*Cyc_Absyn_ulonglong_typ=(void*)& Cyc_Absyn_ulonglong_tt;static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_schar_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sshort_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sint_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slong_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slonglong_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_LongLong_sz};void*Cyc_Absyn_schar_typ=(
void*)& Cyc_Absyn_schar_tt;void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;
void*Cyc_Absyn_sint_typ=(void*)& Cyc_Absyn_sint_tt;void*Cyc_Absyn_slong_typ=(void*)&
Cyc_Absyn_slong_tt;void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;
static struct Cyc_Absyn_IntType_struct Cyc_Absyn_nshort_tt={6,Cyc_Absyn_None,Cyc_Absyn_Short_sz};
static struct Cyc_Absyn_IntType_struct Cyc_Absyn_nint_tt={6,Cyc_Absyn_None,Cyc_Absyn_Int_sz};
static struct Cyc_Absyn_IntType_struct Cyc_Absyn_nlong_tt={6,Cyc_Absyn_None,Cyc_Absyn_Long_sz};
static struct Cyc_Absyn_IntType_struct Cyc_Absyn_nlonglong_tt={6,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};
void*Cyc_Absyn_nshort_typ=(void*)& Cyc_Absyn_nshort_tt;void*Cyc_Absyn_nint_typ=(
void*)& Cyc_Absyn_nint_tt;void*Cyc_Absyn_nlong_typ=(void*)& Cyc_Absyn_nlong_tt;
void*Cyc_Absyn_nlonglong_typ=(void*)& Cyc_Absyn_nlonglong_tt;void*Cyc_Absyn_int_typ(
enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){switch(sn){case Cyc_Absyn_Signed:
_LL46: switch(sz){case Cyc_Absyn_Char_sz: _LL48: return Cyc_Absyn_schar_typ;case Cyc_Absyn_Short_sz:
_LL49: return Cyc_Absyn_sshort_typ;case Cyc_Absyn_Int_sz: _LL4A: return Cyc_Absyn_sint_typ;
case Cyc_Absyn_Long_sz: _LL4B: return Cyc_Absyn_slong_typ;case Cyc_Absyn_LongLong_sz:
_LL4C: return Cyc_Absyn_slonglong_typ;}case Cyc_Absyn_Unsigned: _LL47: switch(sz){
case Cyc_Absyn_Char_sz: _LL4F: return Cyc_Absyn_uchar_typ;case Cyc_Absyn_Short_sz:
_LL50: return Cyc_Absyn_ushort_typ;case Cyc_Absyn_Int_sz: _LL51: return Cyc_Absyn_uint_typ;
case Cyc_Absyn_Long_sz: _LL52: return Cyc_Absyn_ulong_typ;case Cyc_Absyn_LongLong_sz:
_LL53: return Cyc_Absyn_ulonglong_typ;}case Cyc_Absyn_None: _LL4E: switch(sz){case Cyc_Absyn_Char_sz:
_LL56: return Cyc_Absyn_char_typ;case Cyc_Absyn_Short_sz: _LL57: return Cyc_Absyn_nshort_typ;
case Cyc_Absyn_Int_sz: _LL58: return Cyc_Absyn_nint_typ;case Cyc_Absyn_Long_sz: _LL59:
return Cyc_Absyn_nlong_typ;case Cyc_Absyn_LongLong_sz: _LL5A: return Cyc_Absyn_nlonglong_typ;}}}
void*Cyc_Absyn_float_typ=(void*)& Cyc_Absyn_FloatType_val;void*Cyc_Absyn_double_typ(
int b){static struct Cyc_Absyn_DoubleType_struct dt={8,1};static struct Cyc_Absyn_DoubleType_struct
df={8,0};return(void*)(b?& dt:& df);}static char _tmp7F[4]="exn";static struct
_dyneither_ptr Cyc_Absyn_exn_str={_tmp7F,_tmp7F,_tmp7F + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={
2,0}},& Cyc_Absyn_exn_str};struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;
static char _tmp80[15]="Null_Exception";static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={
_tmp80,_tmp80,_tmp80 + 15};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={{.Abs_n={
2,0}},& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=&
Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Null_Exception_tuf_v={&
Cyc_Absyn_Null_Exception_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*
Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp81[13]="Array_bounds";
static struct _dyneither_ptr Cyc_Absyn_Array_bounds_str={_tmp81,_tmp81,_tmp81 + 13};
static struct _tuple0 Cyc_Absyn_Array_bounds_pr={{.Abs_n={2,0}},& Cyc_Absyn_Array_bounds_str};
struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static
struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,
0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=&
Cyc_Absyn_Array_bounds_tuf_v;static char _tmp82[16]="Match_Exception";static struct
_dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp82,_tmp82,_tmp82 + 16};static
struct _tuple0 Cyc_Absyn_Match_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Match_Exception_str};
struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;
static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,
0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp83[10]="Bad_alloc";static struct
_dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp83,_tmp83,_tmp83 + 10};static struct
_tuple0 Cyc_Absyn_Bad_alloc_pr={{.Abs_n={2,0}},& Cyc_Absyn_Bad_alloc_str};struct
_tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Datatypefield
Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,Cyc_Absyn_Extern};struct
Cyc_Absyn_Datatypefield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,
0};static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l0};static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};static struct Cyc_Absyn_Datatypedecl
Cyc_Absyn_exn_tud_v={Cyc_Absyn_Extern,& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_exn_ol,1};struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
static struct Cyc_Core_Opt Cyc_Absyn_heap_opt={(void*)((void*)& Cyc_Absyn_HeapRgn_val)};
static struct Cyc_Absyn_DatatypeType_struct Cyc_Absyn_exn_typ_tt={3,{{.KnownDatatype={
2,& Cyc_Absyn_exn_tud}},0}};void*Cyc_Absyn_exn_typ(){static struct Cyc_Core_Opt*
exn_type_val=0;if(exn_type_val == 0){struct Cyc_Core_Opt*_tmp23F;exn_type_val=((
_tmp23F=_cycalloc(sizeof(*_tmp23F)),((_tmp23F->v=Cyc_Absyn_at_typ((void*)& Cyc_Absyn_exn_typ_tt,(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp23F))));}return(void*)exn_type_val->v;}static char _tmp86[9]="PrintArg";static
struct _dyneither_ptr Cyc_Absyn_printarg_str={_tmp86,_tmp86,_tmp86 + 9};static char
_tmp87[9]="ScanfArg";static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp87,
_tmp87,_tmp87 + 9};static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={
2,0}},& Cyc_Absyn_printarg_str};static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={
2,0}},& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=&
Cyc_Absyn_datatype_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=&
Cyc_Absyn_datatype_scanf_arg_qvar_p;static void**Cyc_Absyn_string_t_opt=0;void*
Cyc_Absyn_string_typ(void*rgn){void*_tmp88=Cyc_Tcutil_compress(rgn);_LL5D: {
struct Cyc_Absyn_HeapRgn_struct*_tmp89=(struct Cyc_Absyn_HeapRgn_struct*)_tmp88;
if(_tmp89->tag != 21)goto _LL5F;}_LL5E: if(Cyc_Absyn_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),(void*)&
Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);void**_tmp240;Cyc_Absyn_string_t_opt=((
_tmp240=_cycalloc(sizeof(*_tmp240)),((_tmp240[0]=t,_tmp240))));}return*((void**)
_check_null(Cyc_Absyn_string_t_opt));_LL5F:;_LL60: return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,
rgn,Cyc_Absyn_empty_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
_LL5C:;}static void**Cyc_Absyn_const_string_t_opt=0;void*Cyc_Absyn_const_string_typ(
void*rgn){void*_tmp8B=Cyc_Tcutil_compress(rgn);_LL62: {struct Cyc_Absyn_HeapRgn_struct*
_tmp8C=(struct Cyc_Absyn_HeapRgn_struct*)_tmp8B;if(_tmp8C->tag != 21)goto _LL64;}
_LL63: if(Cyc_Absyn_const_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void**_tmp241;Cyc_Absyn_const_string_t_opt=((_tmp241=
_cycalloc(sizeof(*_tmp241)),((_tmp241[0]=t,_tmp241))));}return*((void**)
_check_null(Cyc_Absyn_const_string_t_opt));_LL64:;_LL65: return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);_LL61:;}void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,void*b,union Cyc_Absyn_Constraint*zeroterm){struct Cyc_Absyn_PointerType_struct
_tmp24B;struct Cyc_Absyn_PtrAtts _tmp24A;struct Cyc_Absyn_PtrInfo _tmp249;struct Cyc_Absyn_PointerType_struct*
_tmp248;return(void*)((_tmp248=_cycalloc(sizeof(*_tmp248)),((_tmp248[0]=((
_tmp24B.tag=5,((_tmp24B.f1=((_tmp249.elt_typ=t,((_tmp249.elt_tq=tq,((_tmp249.ptr_atts=((
_tmp24A.rgn=r,((_tmp24A.nullable=Cyc_Absyn_true_conref,((_tmp24A.bounds=((union
Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp24A.zero_term=
zeroterm,((_tmp24A.ptrloc=0,_tmp24A)))))))))),_tmp249)))))),_tmp24B)))),_tmp248))));}
void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zeroterm){struct Cyc_Absyn_PointerType_struct _tmp255;struct Cyc_Absyn_PtrAtts
_tmp254;struct Cyc_Absyn_PtrInfo _tmp253;struct Cyc_Absyn_PointerType_struct*
_tmp252;return(void*)((_tmp252=_cycalloc(sizeof(*_tmp252)),((_tmp252[0]=((
_tmp255.tag=5,((_tmp255.f1=((_tmp253.elt_typ=t,((_tmp253.elt_tq=tq,((_tmp253.ptr_atts=((
_tmp254.rgn=r,((_tmp254.nullable=Cyc_Absyn_false_conref,((_tmp254.bounds=((union
Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp254.zero_term=
zeroterm,((_tmp254.ptrloc=0,_tmp254)))))))))),_tmp253)))))),_tmp255)))),_tmp252))));}
void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zeroterm){struct Cyc_Absyn_PointerType_struct _tmp25F;struct Cyc_Absyn_PtrAtts
_tmp25E;struct Cyc_Absyn_PtrInfo _tmp25D;struct Cyc_Absyn_PointerType_struct*
_tmp25C;return(void*)((_tmp25C=_cycalloc(sizeof(*_tmp25C)),((_tmp25C[0]=((
_tmp25F.tag=5,((_tmp25F.f1=((_tmp25D.elt_typ=t,((_tmp25D.elt_tq=tq,((_tmp25D.ptr_atts=((
_tmp25E.rgn=r,((_tmp25E.nullable=Cyc_Absyn_true_conref,((_tmp25E.bounds=Cyc_Absyn_bounds_one_conref,((
_tmp25E.zero_term=zeroterm,((_tmp25E.ptrloc=0,_tmp25E)))))))))),_tmp25D)))))),
_tmp25F)))),_tmp25C))));}void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){
struct Cyc_Absyn_PointerType_struct _tmp269;struct Cyc_Absyn_PtrAtts _tmp268;struct
Cyc_Absyn_PtrInfo _tmp267;struct Cyc_Absyn_PointerType_struct*_tmp266;return(void*)((
_tmp266=_cycalloc(sizeof(*_tmp266)),((_tmp266[0]=((_tmp269.tag=5,((_tmp269.f1=((
_tmp267.elt_typ=t,((_tmp267.elt_tq=tq,((_tmp267.ptr_atts=((_tmp268.rgn=(void*)&
Cyc_Absyn_HeapRgn_val,((_tmp268.nullable=Cyc_Absyn_true_conref,((_tmp268.bounds=
Cyc_Absyn_bounds_one_conref,((_tmp268.zero_term=Cyc_Absyn_false_conref,((_tmp268.ptrloc=
0,_tmp268)))))))))),_tmp267)))))),_tmp269)))),_tmp266))));}void*Cyc_Absyn_at_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){struct
Cyc_Absyn_PointerType_struct _tmp273;struct Cyc_Absyn_PtrAtts _tmp272;struct Cyc_Absyn_PtrInfo
_tmp271;struct Cyc_Absyn_PointerType_struct*_tmp270;return(void*)((_tmp270=
_cycalloc(sizeof(*_tmp270)),((_tmp270[0]=((_tmp273.tag=5,((_tmp273.f1=((_tmp271.elt_typ=
t,((_tmp271.elt_tq=tq,((_tmp271.ptr_atts=((_tmp272.rgn=r,((_tmp272.nullable=Cyc_Absyn_false_conref,((
_tmp272.bounds=Cyc_Absyn_bounds_one_conref,((_tmp272.zero_term=zeroterm,((
_tmp272.ptrloc=0,_tmp272)))))))))),_tmp271)))))),_tmp273)))),_tmp270))));}void*
Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zeroterm){struct Cyc_Absyn_PointerType_struct _tmp27D;struct Cyc_Absyn_PtrAtts
_tmp27C;struct Cyc_Absyn_PtrInfo _tmp27B;struct Cyc_Absyn_PointerType_struct*
_tmp27A;return(void*)((_tmp27A=_cycalloc(sizeof(*_tmp27A)),((_tmp27A[0]=((
_tmp27D.tag=5,((_tmp27D.f1=((_tmp27B.elt_typ=t,((_tmp27B.elt_tq=tq,((_tmp27B.ptr_atts=((
_tmp27C.rgn=r,((_tmp27C.nullable=Cyc_Absyn_true_conref,((_tmp27C.bounds=Cyc_Absyn_bounds_dyneither_conref,((
_tmp27C.zero_term=zeroterm,((_tmp27C.ptrloc=0,_tmp27C)))))))))),_tmp27B)))))),
_tmp27D)))),_tmp27A))));}void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc){struct Cyc_Absyn_ArrayType_struct _tmp283;struct Cyc_Absyn_ArrayInfo _tmp282;
struct Cyc_Absyn_ArrayType_struct*_tmp281;return(void*)((_tmp281=_cycalloc(
sizeof(*_tmp281)),((_tmp281[0]=((_tmp283.tag=9,((_tmp283.f1=((_tmp282.elt_type=
elt_type,((_tmp282.tq=tq,((_tmp282.num_elts=num_elts,((_tmp282.zero_term=
zero_term,((_tmp282.zt_loc=ztloc,_tmp282)))))))))),_tmp283)))),_tmp281))));}
static char _tmpB0[8]="__sFILE";void*Cyc_Absyn_file_typ(){static void**file_t_opt=0;
static struct _dyneither_ptr sf_str={_tmpB0,_tmpB0,_tmpB0 + 8};static struct
_dyneither_ptr*sf=& sf_str;if(file_t_opt == 0){struct _tuple0*_tmp284;struct _tuple0*
file_t_name=(_tmp284=_cycalloc(sizeof(*_tmp284)),((_tmp284->f1=Cyc_Absyn_Abs_n(0,
0),((_tmp284->f2=sf,_tmp284)))));struct Cyc_Absyn_Aggrdecl*_tmp285;struct Cyc_Absyn_Aggrdecl*
sd=(_tmp285=_cycalloc(sizeof(*_tmp285)),((_tmp285->kind=Cyc_Absyn_StructA,((
_tmp285->sc=Cyc_Absyn_Abstract,((_tmp285->name=file_t_name,((_tmp285->tvs=0,((
_tmp285->impl=0,((_tmp285->attributes=0,_tmp285)))))))))))));struct Cyc_Absyn_AggrType_struct
_tmp28F;struct Cyc_Absyn_Aggrdecl**_tmp28E;struct Cyc_Absyn_AggrInfo _tmp28D;struct
Cyc_Absyn_AggrType_struct*_tmp28C;void*file_struct_typ=(void*)((_tmp28C=
_cycalloc(sizeof(*_tmp28C)),((_tmp28C[0]=((_tmp28F.tag=12,((_tmp28F.f1=((_tmp28D.aggr_info=
Cyc_Absyn_KnownAggr(((_tmp28E=_cycalloc(sizeof(*_tmp28E)),((_tmp28E[0]=sd,
_tmp28E))))),((_tmp28D.targs=0,_tmp28D)))),_tmp28F)))),_tmp28C))));void**_tmp290;
file_t_opt=((_tmp290=_cycalloc(sizeof(*_tmp290)),((_tmp290[0]=Cyc_Absyn_at_typ(
file_struct_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp290))));}return*file_t_opt;}void*Cyc_Absyn_void_star_typ(){static void**
void_star_t_opt=0;if(void_star_t_opt == 0){void**_tmp291;void_star_t_opt=((
_tmp291=_cycalloc(sizeof(*_tmp291)),((_tmp291[0]=Cyc_Absyn_star_typ((void*)& Cyc_Absyn_VoidType_val,(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp291))));}return*void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={
24,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_aggr_typ(
enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name){struct Cyc_Absyn_AggrType_struct
_tmp29B;struct _tuple0*_tmp29A;struct Cyc_Absyn_AggrInfo _tmp299;struct Cyc_Absyn_AggrType_struct*
_tmp298;return(void*)((_tmp298=_cycalloc(sizeof(*_tmp298)),((_tmp298[0]=((
_tmp29B.tag=12,((_tmp29B.f1=((_tmp299.aggr_info=Cyc_Absyn_UnknownAggr(k,((
_tmp29A=_cycalloc(sizeof(*_tmp29A)),((_tmp29A->f1=Cyc_Absyn_rel_ns_null,((
_tmp29A->f2=name,_tmp29A)))))),0),((_tmp299.targs=0,_tmp299)))),_tmp29B)))),
_tmp298))));}void*Cyc_Absyn_strct(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(
Cyc_Absyn_StructA,name);}void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){
return Cyc_Absyn_aggr_typ(Cyc_Absyn_UnionA,name);}void*Cyc_Absyn_strctq(struct
_tuple0*name){struct Cyc_Absyn_AggrType_struct _tmp2A1;struct Cyc_Absyn_AggrInfo
_tmp2A0;struct Cyc_Absyn_AggrType_struct*_tmp29F;return(void*)((_tmp29F=_cycalloc(
sizeof(*_tmp29F)),((_tmp29F[0]=((_tmp2A1.tag=12,((_tmp2A1.f1=((_tmp2A0.aggr_info=
Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0),((_tmp2A0.targs=0,_tmp2A0)))),
_tmp2A1)))),_tmp29F))));}void*Cyc_Absyn_unionq_typ(struct _tuple0*name){struct Cyc_Absyn_AggrType_struct
_tmp2A7;struct Cyc_Absyn_AggrInfo _tmp2A6;struct Cyc_Absyn_AggrType_struct*_tmp2A5;
return(void*)((_tmp2A5=_cycalloc(sizeof(*_tmp2A5)),((_tmp2A5[0]=((_tmp2A7.tag=12,((
_tmp2A7.f1=((_tmp2A6.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),((
_tmp2A6.targs=0,_tmp2A6)))),_tmp2A7)))),_tmp2A5))));}union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={
1,0}};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){struct
_tuple3 _tmp2AA;union Cyc_Absyn_Cnst _tmp2A9;return((_tmp2A9.Char_c).val=((_tmp2AA.f1=
sn,((_tmp2AA.f2=c,_tmp2AA)))),(((_tmp2A9.Char_c).tag=2,_tmp2A9)));}union Cyc_Absyn_Cnst
Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){struct _tuple4 _tmp2AD;union Cyc_Absyn_Cnst
_tmp2AC;return((_tmp2AC.Short_c).val=((_tmp2AD.f1=sn,((_tmp2AD.f2=s,_tmp2AD)))),(((
_tmp2AC.Short_c).tag=3,_tmp2AC)));}union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign
sn,int i){struct _tuple5 _tmp2B0;union Cyc_Absyn_Cnst _tmp2AF;return((_tmp2AF.Int_c).val=((
_tmp2B0.f1=sn,((_tmp2B0.f2=i,_tmp2B0)))),(((_tmp2AF.Int_c).tag=4,_tmp2AF)));}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){struct
_tuple6 _tmp2B3;union Cyc_Absyn_Cnst _tmp2B2;return((_tmp2B2.LongLong_c).val=((
_tmp2B3.f1=sn,((_tmp2B3.f2=l,_tmp2B3)))),(((_tmp2B2.LongLong_c).tag=5,_tmp2B2)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst
_tmp2B4;return((_tmp2B4.Float_c).val=s,(((_tmp2B4.Float_c).tag=6,_tmp2B4)));}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst
_tmp2B5;return((_tmp2B5.String_c).val=s,(((_tmp2B5.String_c).tag=7,_tmp2B5)));}
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Exp*_tmp2B6;return(_tmp2B6=_cycalloc(sizeof(*_tmp2B6)),((_tmp2B6->topt=
0,((_tmp2B6->r=r,((_tmp2B6->loc=loc,((_tmp2B6->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,
_tmp2B6)))))))));}struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_New_e_struct
_tmp2B9;struct Cyc_Absyn_New_e_struct*_tmp2B8;return Cyc_Absyn_new_exp((void*)((
_tmp2B8=_cycalloc(sizeof(*_tmp2B8)),((_tmp2B8[0]=((_tmp2B9.tag=17,((_tmp2B9.f1=
rgn_handle,((_tmp2B9.f2=e,_tmp2B9)))))),_tmp2B8)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_tmp2BA;return(
_tmp2BA=_cycalloc(sizeof(*_tmp2BA)),((_tmp2BA[0]=*e,_tmp2BA)));}struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_Const_e_struct _tmp2BD;struct Cyc_Absyn_Const_e_struct*_tmp2BC;return Cyc_Absyn_new_exp((
void*)((_tmp2BC=_cycalloc(sizeof(*_tmp2BC)),((_tmp2BC[0]=((_tmp2BD.tag=0,((
_tmp2BD.f1=c,_tmp2BD)))),_tmp2BC)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Const_e_struct _tmp2C0;struct Cyc_Absyn_Const_e_struct*
_tmp2BF;return Cyc_Absyn_new_exp((void*)((_tmp2BF=_cycalloc(sizeof(*_tmp2BF)),((
_tmp2BF[0]=((_tmp2C0.tag=0,((_tmp2C0.f1=Cyc_Absyn_Null_c,_tmp2C0)))),_tmp2BF)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,struct Cyc_Position_Segment*
seg){return Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(s,i),seg);}struct Cyc_Absyn_Exp*
Cyc_Absyn_signed_int_exp(int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)
i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(1,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(0,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(Cyc_Absyn_None,c),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(f),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_string_exp(struct _dyneither_ptr s,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_const_exp(Cyc_Absyn_String_c(s),loc);}struct Cyc_Absyn_Unresolved_b_struct
Cyc_Absyn_Unresolved_b_val={0};struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct
_tuple0*q,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Var_e_struct _tmp2C3;
struct Cyc_Absyn_Var_e_struct*_tmp2C2;return Cyc_Absyn_new_exp((void*)((_tmp2C2=
_cycalloc(sizeof(*_tmp2C2)),((_tmp2C2[0]=((_tmp2C3.tag=1,((_tmp2C3.f1=q,((
_tmp2C3.f2=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),_tmp2C3)))))),_tmp2C2)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Var_e_struct _tmp2C6;struct Cyc_Absyn_Var_e_struct*_tmp2C5;
return Cyc_Absyn_new_exp((void*)((_tmp2C5=_cycalloc(sizeof(*_tmp2C5)),((_tmp2C5[0]=((
_tmp2C6.tag=1,((_tmp2C6.f1=q,((_tmp2C6.f2=(void*)b,_tmp2C6)))))),_tmp2C5)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_UnknownId_e_struct _tmp2C9;struct Cyc_Absyn_UnknownId_e_struct*
_tmp2C8;return Cyc_Absyn_new_exp((void*)((_tmp2C8=_cycalloc(sizeof(*_tmp2C8)),((
_tmp2C8[0]=((_tmp2C9.tag=2,((_tmp2C9.f1=q,_tmp2C9)))),_tmp2C8)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Primop_e_struct _tmp2CC;struct Cyc_Absyn_Primop_e_struct*
_tmp2CB;return Cyc_Absyn_new_exp((void*)((_tmp2CB=_cycalloc(sizeof(*_tmp2CB)),((
_tmp2CB[0]=((_tmp2CC.tag=3,((_tmp2CC.f1=p,((_tmp2CC.f2=es,_tmp2CC)))))),_tmp2CB)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp2CD;return Cyc_Absyn_primop_exp(
p,((_tmp2CD=_cycalloc(sizeof(*_tmp2CD)),((_tmp2CD->hd=e,((_tmp2CD->tl=0,_tmp2CD)))))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){struct Cyc_List_List*
_tmp2D0;struct Cyc_List_List*_tmp2CF;return Cyc_Absyn_primop_exp(p,((_tmp2CF=
_cycalloc(sizeof(*_tmp2CF)),((_tmp2CF->hd=e1,((_tmp2CF->tl=((_tmp2D0=_cycalloc(
sizeof(*_tmp2D0)),((_tmp2D0->hd=e2,((_tmp2D0->tl=0,_tmp2D0)))))),_tmp2CF)))))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Swap_e_struct _tmp2D3;struct
Cyc_Absyn_Swap_e_struct*_tmp2D2;return Cyc_Absyn_new_exp((void*)((_tmp2D2=
_cycalloc(sizeof(*_tmp2D2)),((_tmp2D2[0]=((_tmp2D3.tag=35,((_tmp2D3.f1=e1,((
_tmp2D3.f2=e2,_tmp2D3)))))),_tmp2D2)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_AssignOp_e_struct _tmp2D6;struct Cyc_Absyn_AssignOp_e_struct*
_tmp2D5;return Cyc_Absyn_new_exp((void*)((_tmp2D5=_cycalloc(sizeof(*_tmp2D5)),((
_tmp2D5[0]=((_tmp2D6.tag=4,((_tmp2D6.f1=e1,((_tmp2D6.f2=popt,((_tmp2D6.f3=e2,
_tmp2D6)))))))),_tmp2D5)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Increment_e_struct _tmp2D9;struct Cyc_Absyn_Increment_e_struct*
_tmp2D8;return Cyc_Absyn_new_exp((void*)((_tmp2D8=_cycalloc(sizeof(*_tmp2D8)),((
_tmp2D8[0]=((_tmp2D9.tag=5,((_tmp2D9.f1=e,((_tmp2D9.f2=i,_tmp2D9)))))),_tmp2D8)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostInc,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreInc,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,Cyc_Absyn_PreDec,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostDec,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Conditional_e_struct _tmp2DC;struct Cyc_Absyn_Conditional_e_struct*
_tmp2DB;return Cyc_Absyn_new_exp((void*)((_tmp2DB=_cycalloc(sizeof(*_tmp2DB)),((
_tmp2DB[0]=((_tmp2DC.tag=6,((_tmp2DC.f1=e1,((_tmp2DC.f2=e2,((_tmp2DC.f3=e3,
_tmp2DC)))))))),_tmp2DB)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_And_e_struct _tmp2DF;struct Cyc_Absyn_And_e_struct*_tmp2DE;return Cyc_Absyn_new_exp((
void*)((_tmp2DE=_cycalloc(sizeof(*_tmp2DE)),((_tmp2DE[0]=((_tmp2DF.tag=7,((
_tmp2DF.f1=e1,((_tmp2DF.f2=e2,_tmp2DF)))))),_tmp2DE)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Or_e_struct _tmp2E2;struct Cyc_Absyn_Or_e_struct*_tmp2E1;
return Cyc_Absyn_new_exp((void*)((_tmp2E1=_cycalloc(sizeof(*_tmp2E1)),((_tmp2E1[0]=((
_tmp2E2.tag=8,((_tmp2E2.f1=e1,((_tmp2E2.f2=e2,_tmp2E2)))))),_tmp2E1)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_SeqExp_e_struct _tmp2E5;struct
Cyc_Absyn_SeqExp_e_struct*_tmp2E4;return Cyc_Absyn_new_exp((void*)((_tmp2E4=
_cycalloc(sizeof(*_tmp2E4)),((_tmp2E4[0]=((_tmp2E5.tag=9,((_tmp2E5.f1=e1,((
_tmp2E5.f2=e2,_tmp2E5)))))),_tmp2E4)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_UnknownCall_e_struct _tmp2E8;struct Cyc_Absyn_UnknownCall_e_struct*
_tmp2E7;return Cyc_Absyn_new_exp((void*)((_tmp2E7=_cycalloc(sizeof(*_tmp2E7)),((
_tmp2E7[0]=((_tmp2E8.tag=10,((_tmp2E8.f1=e,((_tmp2E8.f2=es,_tmp2E8)))))),_tmp2E7)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_FnCall_e_struct _tmp2EB;struct
Cyc_Absyn_FnCall_e_struct*_tmp2EA;return Cyc_Absyn_new_exp((void*)((_tmp2EA=
_cycalloc(sizeof(*_tmp2EA)),((_tmp2EA[0]=((_tmp2EB.tag=11,((_tmp2EB.f1=e,((
_tmp2EB.f2=es,((_tmp2EB.f3=0,_tmp2EB)))))))),_tmp2EA)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_NoInstantiate_e_struct _tmp2EE;struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp2ED;return Cyc_Absyn_new_exp((void*)((_tmp2ED=_cycalloc(sizeof(*_tmp2ED)),((
_tmp2ED[0]=((_tmp2EE.tag=13,((_tmp2EE.f1=e,_tmp2EE)))),_tmp2ED)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
ts,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Instantiate_e_struct _tmp2F1;
struct Cyc_Absyn_Instantiate_e_struct*_tmp2F0;return Cyc_Absyn_new_exp((void*)((
_tmp2F0=_cycalloc(sizeof(*_tmp2F0)),((_tmp2F0[0]=((_tmp2F1.tag=14,((_tmp2F1.f1=e,((
_tmp2F1.f2=ts,_tmp2F1)))))),_tmp2F0)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Cast_e_struct _tmp2F4;struct Cyc_Absyn_Cast_e_struct*_tmp2F3;
return Cyc_Absyn_new_exp((void*)((_tmp2F3=_cycalloc(sizeof(*_tmp2F3)),((_tmp2F3[0]=((
_tmp2F4.tag=15,((_tmp2F4.f1=(void*)t,((_tmp2F4.f2=e,((_tmp2F4.f3=user_cast,((
_tmp2F4.f4=c,_tmp2F4)))))))))),_tmp2F3)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Throw_e_struct
_tmp2F7;struct Cyc_Absyn_Throw_e_struct*_tmp2F6;return Cyc_Absyn_new_exp((void*)((
_tmp2F6=_cycalloc(sizeof(*_tmp2F6)),((_tmp2F6[0]=((_tmp2F7.tag=12,((_tmp2F7.f1=e,
_tmp2F7)))),_tmp2F6)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Address_e_struct
_tmp2FA;struct Cyc_Absyn_Address_e_struct*_tmp2F9;return Cyc_Absyn_new_exp((void*)((
_tmp2F9=_cycalloc(sizeof(*_tmp2F9)),((_tmp2F9[0]=((_tmp2FA.tag=16,((_tmp2FA.f1=e,
_tmp2FA)))),_tmp2F9)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Sizeoftyp_e_struct _tmp2FD;struct
Cyc_Absyn_Sizeoftyp_e_struct*_tmp2FC;return Cyc_Absyn_new_exp((void*)((_tmp2FC=
_cycalloc(sizeof(*_tmp2FC)),((_tmp2FC[0]=((_tmp2FD.tag=18,((_tmp2FD.f1=(void*)t,
_tmp2FD)))),_tmp2FC)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Sizeofexp_e_struct
_tmp300;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp2FF;return Cyc_Absyn_new_exp((void*)((
_tmp2FF=_cycalloc(sizeof(*_tmp2FF)),((_tmp2FF[0]=((_tmp300.tag=19,((_tmp300.f1=e,
_tmp300)))),_tmp2FF)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,
void*of,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Offsetof_e_struct _tmp303;
struct Cyc_Absyn_Offsetof_e_struct*_tmp302;return Cyc_Absyn_new_exp((void*)((
_tmp302=_cycalloc(sizeof(*_tmp302)),((_tmp302[0]=((_tmp303.tag=20,((_tmp303.f1=(
void*)t,((_tmp303.f2=(void*)of,_tmp303)))))),_tmp302)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_Deref_e_struct _tmp306;struct Cyc_Absyn_Deref_e_struct*_tmp305;return Cyc_Absyn_new_exp((
void*)((_tmp305=_cycalloc(sizeof(*_tmp305)),((_tmp305[0]=((_tmp306.tag=21,((
_tmp306.f1=e,_tmp306)))),_tmp305)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_AggrMember_e_struct _tmp309;struct Cyc_Absyn_AggrMember_e_struct*
_tmp308;return Cyc_Absyn_new_exp((void*)((_tmp308=_cycalloc(sizeof(*_tmp308)),((
_tmp308[0]=((_tmp309.tag=22,((_tmp309.f1=e,((_tmp309.f2=n,((_tmp309.f3=0,((
_tmp309.f4=0,_tmp309)))))))))),_tmp308)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_AggrArrow_e_struct _tmp30C;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp30B;return Cyc_Absyn_new_exp((void*)((_tmp30B=_cycalloc(sizeof(*_tmp30B)),((
_tmp30B[0]=((_tmp30C.tag=23,((_tmp30C.f1=e,((_tmp30C.f2=n,((_tmp30C.f3=0,((
_tmp30C.f4=0,_tmp30C)))))))))),_tmp30B)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Subscript_e_struct _tmp30F;struct Cyc_Absyn_Subscript_e_struct*
_tmp30E;return Cyc_Absyn_new_exp((void*)((_tmp30E=_cycalloc(sizeof(*_tmp30E)),((
_tmp30E[0]=((_tmp30F.tag=24,((_tmp30F.f1=e1,((_tmp30F.f2=e2,_tmp30F)))))),
_tmp30E)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Tuple_e_struct _tmp312;struct Cyc_Absyn_Tuple_e_struct*
_tmp311;return Cyc_Absyn_new_exp((void*)((_tmp311=_cycalloc(sizeof(*_tmp311)),((
_tmp311[0]=((_tmp312.tag=25,((_tmp312.f1=es,_tmp312)))),_tmp311)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_StmtExp_e_struct _tmp315;struct Cyc_Absyn_StmtExp_e_struct*
_tmp314;return Cyc_Absyn_new_exp((void*)((_tmp314=_cycalloc(sizeof(*_tmp314)),((
_tmp314[0]=((_tmp315.tag=37,((_tmp315.f1=s,_tmp315)))),_tmp314)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_var_exp(
Cyc_Absyn_Match_Exception_name,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(
void*t,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Valueof_e_struct _tmp318;
struct Cyc_Absyn_Valueof_e_struct*_tmp317;return Cyc_Absyn_new_exp((void*)((
_tmp317=_cycalloc(sizeof(*_tmp317)),((_tmp317[0]=((_tmp318.tag=39,((_tmp318.f1=(
void*)t,_tmp318)))),_tmp317)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int
volatile_kw,struct _dyneither_ptr body,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Asm_e_struct
_tmp31B;struct Cyc_Absyn_Asm_e_struct*_tmp31A;return Cyc_Absyn_new_exp((void*)((
_tmp31A=_cycalloc(sizeof(*_tmp31A)),((_tmp31A[0]=((_tmp31B.tag=40,((_tmp31B.f1=
volatile_kw,((_tmp31B.f2=body,_tmp31B)))))),_tmp31A)))),loc);}struct _tuple12{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){struct _tuple12*_tmp31E;struct Cyc_List_List*_tmp31D;
dles=((_tmp31D=_cycalloc(sizeof(*_tmp31D)),((_tmp31D->hd=((_tmp31E=_cycalloc(
sizeof(*_tmp31E)),((_tmp31E->f1=0,((_tmp31E->f2=(struct Cyc_Absyn_Exp*)es->hd,
_tmp31E)))))),((_tmp31D->tl=dles,_tmp31D))))));}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);{struct Cyc_Absyn_Array_e_struct
_tmp321;struct Cyc_Absyn_Array_e_struct*_tmp320;return Cyc_Absyn_new_exp((void*)((
_tmp320=_cycalloc(sizeof(*_tmp320)),((_tmp320[0]=((_tmp321.tag=27,((_tmp321.f1=
dles,_tmp321)))),_tmp320)))),loc);};}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp324;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp323;return Cyc_Absyn_new_exp((void*)((_tmp323=_cycalloc(sizeof(*_tmp323)),((
_tmp323[0]=((_tmp324.tag=36,((_tmp324.f1=n,((_tmp324.f2=dles,_tmp324)))))),
_tmp323)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Stmt*_tmp325;return(_tmp325=_cycalloc(sizeof(*_tmp325)),((
_tmp325->r=s,((_tmp325->loc=loc,((_tmp325->non_local_preds=0,((_tmp325->try_depth=
0,((_tmp325->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,_tmp325)))))))))));}struct
Cyc_Absyn_Skip_s_struct Cyc_Absyn_Skip_s_val={0};struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Exp_s_struct _tmp328;struct Cyc_Absyn_Exp_s_struct*_tmp327;
return Cyc_Absyn_new_stmt((void*)((_tmp327=_cycalloc(sizeof(*_tmp327)),((_tmp327[
0]=((_tmp328.tag=1,((_tmp328.f1=e,_tmp328)))),_tmp327)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss
== 0)return Cyc_Absyn_skip_stmt(loc);else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)
ss->hd;else{return Cyc_Absyn_seq_stmt((struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(
ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Return_s_struct _tmp32B;struct
Cyc_Absyn_Return_s_struct*_tmp32A;return Cyc_Absyn_new_stmt((void*)((_tmp32A=
_cycalloc(sizeof(*_tmp32A)),((_tmp32A[0]=((_tmp32B.tag=3,((_tmp32B.f1=e,_tmp32B)))),
_tmp32A)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_IfThenElse_s_struct _tmp32E;struct Cyc_Absyn_IfThenElse_s_struct*
_tmp32D;return Cyc_Absyn_new_stmt((void*)((_tmp32D=_cycalloc(sizeof(*_tmp32D)),((
_tmp32D[0]=((_tmp32E.tag=4,((_tmp32E.f1=e,((_tmp32E.f2=s1,((_tmp32E.f3=s2,
_tmp32E)))))))),_tmp32D)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_While_s_struct _tmp334;struct _tuple8 _tmp333;struct Cyc_Absyn_While_s_struct*
_tmp332;return Cyc_Absyn_new_stmt((void*)((_tmp332=_cycalloc(sizeof(*_tmp332)),((
_tmp332[0]=((_tmp334.tag=5,((_tmp334.f1=((_tmp333.f1=e,((_tmp333.f2=Cyc_Absyn_skip_stmt(
e->loc),_tmp333)))),((_tmp334.f2=s,_tmp334)))))),_tmp332)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_break_stmt(struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Break_s_struct
_tmp337;struct Cyc_Absyn_Break_s_struct*_tmp336;return Cyc_Absyn_new_stmt((void*)((
_tmp336=_cycalloc(sizeof(*_tmp336)),((_tmp336[0]=((_tmp337.tag=6,((_tmp337.f1=0,
_tmp337)))),_tmp336)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Continue_s_struct _tmp33A;struct
Cyc_Absyn_Continue_s_struct*_tmp339;return Cyc_Absyn_new_stmt((void*)((_tmp339=
_cycalloc(sizeof(*_tmp339)),((_tmp339[0]=((_tmp33A.tag=7,((_tmp33A.f1=0,_tmp33A)))),
_tmp339)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_For_s_struct _tmp343;struct _tuple8 _tmp342;struct _tuple8
_tmp341;struct Cyc_Absyn_For_s_struct*_tmp340;return Cyc_Absyn_new_stmt((void*)((
_tmp340=_cycalloc(sizeof(*_tmp340)),((_tmp340[0]=((_tmp343.tag=9,((_tmp343.f1=e1,((
_tmp343.f2=((_tmp342.f1=e2,((_tmp342.f2=Cyc_Absyn_skip_stmt(e3->loc),_tmp342)))),((
_tmp343.f3=((_tmp341.f1=e3,((_tmp341.f2=Cyc_Absyn_skip_stmt(e3->loc),_tmp341)))),((
_tmp343.f4=s,_tmp343)))))))))),_tmp340)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Switch_s_struct _tmp346;struct Cyc_Absyn_Switch_s_struct*_tmp345;
return Cyc_Absyn_new_stmt((void*)((_tmp345=_cycalloc(sizeof(*_tmp345)),((_tmp345[
0]=((_tmp346.tag=10,((_tmp346.f1=e,((_tmp346.f2=scs,_tmp346)))))),_tmp345)))),
loc);}struct _tuple13{void*f1;void*f2;};struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc){
struct _tuple13 _tmp347;struct _tuple13 _tmp129=(_tmp347.f1=s1->r,((_tmp347.f2=s2->r,
_tmp347)));void*_tmp12A;void*_tmp12C;_LL67: _tmp12A=_tmp129.f1;{struct Cyc_Absyn_Skip_s_struct*
_tmp12B=(struct Cyc_Absyn_Skip_s_struct*)_tmp12A;if(_tmp12B->tag != 0)goto _LL69;};
_LL68: return s2;_LL69: _tmp12C=_tmp129.f2;{struct Cyc_Absyn_Skip_s_struct*_tmp12D=(
struct Cyc_Absyn_Skip_s_struct*)_tmp12C;if(_tmp12D->tag != 0)goto _LL6B;};_LL6A:
return s1;_LL6B:;_LL6C: {struct Cyc_Absyn_Seq_s_struct _tmp34A;struct Cyc_Absyn_Seq_s_struct*
_tmp349;return Cyc_Absyn_new_stmt((void*)((_tmp349=_cycalloc(sizeof(*_tmp349)),((
_tmp349[0]=((_tmp34A.tag=2,((_tmp34A.f1=s1,((_tmp34A.f2=s2,_tmp34A)))))),_tmp349)))),
loc);}_LL66:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*
el,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Fallthru_s_struct _tmp34D;
struct Cyc_Absyn_Fallthru_s_struct*_tmp34C;return Cyc_Absyn_new_stmt((void*)((
_tmp34C=_cycalloc(sizeof(*_tmp34C)),((_tmp34C[0]=((_tmp34D.tag=11,((_tmp34D.f1=
el,((_tmp34D.f2=0,_tmp34D)))))),_tmp34C)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Decl_s_struct _tmp350;struct Cyc_Absyn_Decl_s_struct*_tmp34F;
return Cyc_Absyn_new_stmt((void*)((_tmp34F=_cycalloc(sizeof(*_tmp34F)),((_tmp34F[
0]=((_tmp350.tag=12,((_tmp350.f1=d,((_tmp350.f2=s,_tmp350)))))),_tmp34F)))),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Var_d_struct
_tmp353;struct Cyc_Absyn_Var_d_struct*_tmp352;struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((
void*)((_tmp352=_cycalloc(sizeof(*_tmp352)),((_tmp352[0]=((_tmp353.tag=0,((
_tmp353.f1=Cyc_Absyn_new_vardecl(x,t,init),_tmp353)))),_tmp352)))),loc);struct
Cyc_Absyn_Decl_s_struct _tmp356;struct Cyc_Absyn_Decl_s_struct*_tmp355;return Cyc_Absyn_new_stmt((
void*)((_tmp355=_cycalloc(sizeof(*_tmp355)),((_tmp355[0]=((_tmp356.tag=12,((
_tmp356.f1=d,((_tmp356.f2=s,_tmp356)))))),_tmp355)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Label_s_struct _tmp359;struct Cyc_Absyn_Label_s_struct*
_tmp358;return Cyc_Absyn_new_stmt((void*)((_tmp358=_cycalloc(sizeof(*_tmp358)),((
_tmp358[0]=((_tmp359.tag=13,((_tmp359.f1=v,((_tmp359.f2=s,_tmp359)))))),_tmp358)))),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Do_s_struct _tmp35F;struct
_tuple8 _tmp35E;struct Cyc_Absyn_Do_s_struct*_tmp35D;return Cyc_Absyn_new_stmt((
void*)((_tmp35D=_cycalloc(sizeof(*_tmp35D)),((_tmp35D[0]=((_tmp35F.tag=14,((
_tmp35F.f1=s,((_tmp35F.f2=((_tmp35E.f1=e,((_tmp35E.f2=Cyc_Absyn_skip_stmt(e->loc),
_tmp35E)))),_tmp35F)))))),_tmp35D)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_TryCatch_s_struct _tmp362;struct Cyc_Absyn_TryCatch_s_struct*
_tmp361;return Cyc_Absyn_new_stmt((void*)((_tmp361=_cycalloc(sizeof(*_tmp361)),((
_tmp361[0]=((_tmp362.tag=15,((_tmp362.f1=s,((_tmp362.f2=scs,_tmp362)))))),
_tmp361)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*
lab,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Goto_s_struct _tmp365;struct
Cyc_Absyn_Goto_s_struct*_tmp364;return Cyc_Absyn_new_stmt((void*)((_tmp364=
_cycalloc(sizeof(*_tmp364)),((_tmp364[0]=((_tmp365.tag=8,((_tmp365.f1=lab,((
_tmp365.f2=0,_tmp365)))))),_tmp364)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s){struct Cyc_Absyn_Pat*
_tmp366;return(_tmp366=_cycalloc(sizeof(*_tmp366)),((_tmp366->r=p,((_tmp366->topt=
0,((_tmp366->loc=s,_tmp366)))))));}struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct
Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_p_struct _tmp369;struct Cyc_Absyn_Exp_p_struct*
_tmp368;return Cyc_Absyn_new_pat((void*)((_tmp368=_cycalloc(sizeof(*_tmp368)),((
_tmp368[0]=((_tmp369.tag=16,((_tmp369.f1=e,_tmp369)))),_tmp368)))),e->loc);}
struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val={0};struct Cyc_Absyn_Null_p_struct
Cyc_Absyn_Null_p_val={8};struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct
Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*_tmp36A;return(_tmp36A=_cycalloc(
sizeof(*_tmp36A)),((_tmp36A->r=r,((_tmp36A->loc=loc,_tmp36A)))));}struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Let_d_struct _tmp36D;struct Cyc_Absyn_Let_d_struct*_tmp36C;
return Cyc_Absyn_new_decl((void*)((_tmp36C=_cycalloc(sizeof(*_tmp36C)),((_tmp36C[
0]=((_tmp36D.tag=2,((_tmp36D.f1=p,((_tmp36D.f2=0,((_tmp36D.f3=e,_tmp36D)))))))),
_tmp36C)))),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*
vds,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Letv_d_struct _tmp370;struct
Cyc_Absyn_Letv_d_struct*_tmp36F;return Cyc_Absyn_new_decl((void*)((_tmp36F=
_cycalloc(sizeof(*_tmp36F)),((_tmp36F[0]=((_tmp370.tag=3,((_tmp370.f1=vds,
_tmp370)))),_tmp36F)))),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct
Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int resetable,struct Cyc_Absyn_Exp*eo,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Region_d_struct _tmp373;struct Cyc_Absyn_Region_d_struct*
_tmp372;return Cyc_Absyn_new_decl((void*)((_tmp372=_cycalloc(sizeof(*_tmp372)),((
_tmp372[0]=((_tmp373.tag=4,((_tmp373.f1=tv,((_tmp373.f2=vd,((_tmp373.f3=
resetable,((_tmp373.f4=eo,_tmp373)))))))))),_tmp372)))),loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Alias_d_struct _tmp376;struct
Cyc_Absyn_Alias_d_struct*_tmp375;return Cyc_Absyn_new_decl((void*)((_tmp375=
_cycalloc(sizeof(*_tmp375)),((_tmp375[0]=((_tmp376.tag=5,((_tmp376.f1=e,((
_tmp376.f2=tv,((_tmp376.f3=vd,_tmp376)))))))),_tmp375)))),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){struct Cyc_Absyn_Vardecl*
_tmp377;return(_tmp377=_cycalloc(sizeof(*_tmp377)),((_tmp377->sc=Cyc_Absyn_Public,((
_tmp377->name=x,((_tmp377->tq=Cyc_Absyn_empty_tqual(0),((_tmp377->type=t,((
_tmp377->initializer=init,((_tmp377->rgn=0,((_tmp377->attributes=0,((_tmp377->escapes=
0,_tmp377)))))))))))))))));}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){struct Cyc_Absyn_Vardecl*_tmp378;
return(_tmp378=_cycalloc(sizeof(*_tmp378)),((_tmp378->sc=Cyc_Absyn_Static,((
_tmp378->name=x,((_tmp378->tq=Cyc_Absyn_empty_tqual(0),((_tmp378->type=t,((
_tmp378->initializer=init,((_tmp378->rgn=0,((_tmp378->attributes=0,((_tmp378->escapes=
0,_tmp378)))))))))))))))));}struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int
tagged){struct Cyc_Absyn_AggrdeclImpl*_tmp379;return(_tmp379=_cycalloc(sizeof(*
_tmp379)),((_tmp379->exist_vars=exists,((_tmp379->rgn_po=po,((_tmp379->fields=fs,((
_tmp379->tagged=tagged,_tmp379)))))))));}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(
enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Aggr_d_struct _tmp37F;struct Cyc_Absyn_Aggrdecl*_tmp37E;
struct Cyc_Absyn_Aggr_d_struct*_tmp37D;return Cyc_Absyn_new_decl((void*)((_tmp37D=
_cycalloc(sizeof(*_tmp37D)),((_tmp37D[0]=((_tmp37F.tag=6,((_tmp37F.f1=((_tmp37E=
_cycalloc(sizeof(*_tmp37E)),((_tmp37E->kind=k,((_tmp37E->sc=s,((_tmp37E->name=n,((
_tmp37E->tvs=ts,((_tmp37E->impl=i,((_tmp37E->attributes=atts,_tmp37E)))))))))))))),
_tmp37F)))),_tmp37D)))),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum 
Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl(
Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(
enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl(
Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(
enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Datatype_d_struct
_tmp385;struct Cyc_Absyn_Datatypedecl*_tmp384;struct Cyc_Absyn_Datatype_d_struct*
_tmp383;return Cyc_Absyn_new_decl((void*)((_tmp383=_cycalloc(sizeof(*_tmp383)),((
_tmp383[0]=((_tmp385.tag=7,((_tmp385.f1=((_tmp384=_cycalloc(sizeof(*_tmp384)),((
_tmp384->sc=s,((_tmp384->name=n,((_tmp384->tvs=ts,((_tmp384->fields=fs,((_tmp384->is_extensible=
is_extensible,_tmp384)))))))))))),_tmp385)))),_tmp383)))),loc);}static struct
_tuple7*Cyc_Absyn_expand_arg(struct _tuple7*a){struct _tuple7*_tmp386;return(
_tmp386=_cycalloc(sizeof(*_tmp386)),((_tmp386->f1=(*a).f1,((_tmp386->f2=(*a).f2,((
_tmp386->f3=Cyc_Absyn_pointer_expand((*a).f3,1),_tmp386)))))));}void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts){struct Cyc_Absyn_FnType_struct _tmp38C;struct Cyc_Absyn_FnInfo
_tmp38B;struct Cyc_Absyn_FnType_struct*_tmp38A;return(void*)((_tmp38A=_cycalloc(
sizeof(*_tmp38A)),((_tmp38A[0]=((_tmp38C.tag=10,((_tmp38C.f1=((_tmp38B.tvars=tvs,((
_tmp38B.ret_typ=Cyc_Absyn_pointer_expand(ret_typ,0),((_tmp38B.effect=eff_typ,((
_tmp38B.args=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,args),((_tmp38B.c_varargs=
c_varargs,((_tmp38B.cyc_varargs=cyc_varargs,((_tmp38B.rgn_po=rgn_po,((_tmp38B.attributes=
atts,_tmp38B)))))))))))))))),_tmp38C)))),_tmp38A))));}void*Cyc_Absyn_pointer_expand(
void*t,int fresh_evar){void*_tmp15C=Cyc_Tcutil_compress(t);_LL6E: {struct Cyc_Absyn_FnType_struct*
_tmp15D=(struct Cyc_Absyn_FnType_struct*)_tmp15C;if(_tmp15D->tag != 10)goto _LL70;}
_LL6F: {struct Cyc_Core_Opt*_tmp38D;return Cyc_Absyn_at_typ(t,fresh_evar?Cyc_Absyn_new_evar(((
_tmp38D=_cycalloc(sizeof(*_tmp38D)),((_tmp38D->v=& Cyc_Tcutil_rk,_tmp38D)))),0):(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);}
_LL70:;_LL71: return t;_LL6D:;}int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){void*
_tmp15F=e->r;void*_tmp161;void*_tmp164;struct Cyc_Absyn_Vardecl*_tmp166;void*
_tmp168;struct Cyc_Absyn_Vardecl*_tmp16A;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Exp*
_tmp172;struct Cyc_Absyn_Exp*_tmp174;_LL73: {struct Cyc_Absyn_Var_e_struct*_tmp160=(
struct Cyc_Absyn_Var_e_struct*)_tmp15F;if(_tmp160->tag != 1)goto _LL75;else{_tmp161=(
void*)_tmp160->f2;{struct Cyc_Absyn_Funname_b_struct*_tmp162=(struct Cyc_Absyn_Funname_b_struct*)
_tmp161;if(_tmp162->tag != 2)goto _LL75;};}}_LL74: return 0;_LL75: {struct Cyc_Absyn_Var_e_struct*
_tmp163=(struct Cyc_Absyn_Var_e_struct*)_tmp15F;if(_tmp163->tag != 1)goto _LL77;
else{_tmp164=(void*)_tmp163->f2;{struct Cyc_Absyn_Global_b_struct*_tmp165=(struct
Cyc_Absyn_Global_b_struct*)_tmp164;if(_tmp165->tag != 1)goto _LL77;else{_tmp166=
_tmp165->f1;}};}}_LL76: _tmp16A=_tmp166;goto _LL78;_LL77: {struct Cyc_Absyn_Var_e_struct*
_tmp167=(struct Cyc_Absyn_Var_e_struct*)_tmp15F;if(_tmp167->tag != 1)goto _LL79;
else{_tmp168=(void*)_tmp167->f2;{struct Cyc_Absyn_Local_b_struct*_tmp169=(struct
Cyc_Absyn_Local_b_struct*)_tmp168;if(_tmp169->tag != 4)goto _LL79;else{_tmp16A=
_tmp169->f1;}};}}_LL78: {void*_tmp175=Cyc_Tcutil_compress(_tmp16A->type);_LL8A: {
struct Cyc_Absyn_ArrayType_struct*_tmp176=(struct Cyc_Absyn_ArrayType_struct*)
_tmp175;if(_tmp176->tag != 9)goto _LL8C;}_LL8B: return 0;_LL8C:;_LL8D: return 1;_LL89:;}
_LL79: {struct Cyc_Absyn_Var_e_struct*_tmp16B=(struct Cyc_Absyn_Var_e_struct*)
_tmp15F;if(_tmp16B->tag != 1)goto _LL7B;}_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp16C=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp15F;if(_tmp16C->tag != 23)goto
_LL7D;}_LL7C: goto _LL7E;_LL7D: {struct Cyc_Absyn_Deref_e_struct*_tmp16D=(struct Cyc_Absyn_Deref_e_struct*)
_tmp15F;if(_tmp16D->tag != 21)goto _LL7F;}_LL7E: goto _LL80;_LL7F: {struct Cyc_Absyn_Subscript_e_struct*
_tmp16E=(struct Cyc_Absyn_Subscript_e_struct*)_tmp15F;if(_tmp16E->tag != 24)goto
_LL81;}_LL80: return 1;_LL81: {struct Cyc_Absyn_AggrMember_e_struct*_tmp16F=(struct
Cyc_Absyn_AggrMember_e_struct*)_tmp15F;if(_tmp16F->tag != 22)goto _LL83;else{
_tmp170=_tmp16F->f1;}}_LL82: return Cyc_Absyn_is_lvalue(_tmp170);_LL83: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp171=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp15F;if(_tmp171->tag != 14)goto
_LL85;else{_tmp172=_tmp171->f1;}}_LL84: return Cyc_Absyn_is_lvalue(_tmp172);_LL85: {
struct Cyc_Absyn_NoInstantiate_e_struct*_tmp173=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp15F;if(_tmp173->tag != 13)goto _LL87;else{_tmp174=_tmp173->f1;}}_LL86: return
Cyc_Absyn_is_lvalue(_tmp174);_LL87:;_LL88: return 0;_LL72:;}struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v){{
struct Cyc_List_List*_tmp177=fields;for(0;_tmp177 != 0;_tmp177=_tmp177->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Aggrfield*)_tmp177->hd)->name,v)== 0)return(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_Absyn_Aggrfield*)_tmp177->hd);}}return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple9*
Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts
== 0)return 0;ts=ts->tl;}if(ts == 0)return 0;return(struct _tuple9*)((struct _tuple9*)
ts->hd);}struct Cyc_Absyn_Stdcall_att_struct Cyc_Absyn_Stdcall_att_val={1};struct
Cyc_Absyn_Cdecl_att_struct Cyc_Absyn_Cdecl_att_val={2};struct Cyc_Absyn_Fastcall_att_struct
Cyc_Absyn_Fastcall_att_val={3};struct Cyc_Absyn_Noreturn_att_struct Cyc_Absyn_Noreturn_att_val={
4};struct Cyc_Absyn_Const_att_struct Cyc_Absyn_Const_att_val={5};struct Cyc_Absyn_Packed_att_struct
Cyc_Absyn_Packed_att_val={7};struct Cyc_Absyn_Nocommon_att_struct Cyc_Absyn_Nocommon_att_val={
9};struct Cyc_Absyn_Shared_att_struct Cyc_Absyn_Shared_att_val={10};struct Cyc_Absyn_Unused_att_struct
Cyc_Absyn_Unused_att_val={11};struct Cyc_Absyn_Weak_att_struct Cyc_Absyn_Weak_att_val={
12};struct Cyc_Absyn_Dllimport_att_struct Cyc_Absyn_Dllimport_att_val={13};struct
Cyc_Absyn_Dllexport_att_struct Cyc_Absyn_Dllexport_att_val={14};struct Cyc_Absyn_No_instrument_function_att_struct
Cyc_Absyn_No_instrument_function_att_val={15};struct Cyc_Absyn_Constructor_att_struct
Cyc_Absyn_Constructor_att_val={16};struct Cyc_Absyn_Destructor_att_struct Cyc_Absyn_Destructor_att_val={
17};struct Cyc_Absyn_No_check_memory_usage_att_struct Cyc_Absyn_No_check_memory_usage_att_val={
18};struct Cyc_Absyn_Pure_att_struct Cyc_Absyn_Pure_att_val={22};struct
_dyneither_ptr Cyc_Absyn_attribute2string(void*a){void*_tmp189=a;int _tmp18B;int
_tmp192;struct _dyneither_ptr _tmp195;enum Cyc_Absyn_Format_Type _tmp1A1;int _tmp1A2;
int _tmp1A3;enum Cyc_Absyn_Format_Type _tmp1A5;int _tmp1A6;int _tmp1A7;int _tmp1A9;
int _tmp1AB;struct _dyneither_ptr _tmp1AE;_LL8F: {struct Cyc_Absyn_Regparm_att_struct*
_tmp18A=(struct Cyc_Absyn_Regparm_att_struct*)_tmp189;if(_tmp18A->tag != 0)goto
_LL91;else{_tmp18B=_tmp18A->f1;}}_LL90: {const char*_tmp391;void*_tmp390[1];
struct Cyc_Int_pa_struct _tmp38F;return(struct _dyneither_ptr)((_tmp38F.tag=1,((
_tmp38F.f1=(unsigned long)_tmp18B,((_tmp390[0]=& _tmp38F,Cyc_aprintf(((_tmp391="regparm(%d)",
_tag_dyneither(_tmp391,sizeof(char),12))),_tag_dyneither(_tmp390,sizeof(void*),1))))))));}
_LL91: {struct Cyc_Absyn_Stdcall_att_struct*_tmp18C=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp189;if(_tmp18C->tag != 1)goto _LL93;}_LL92: {const char*_tmp392;return(_tmp392="stdcall",
_tag_dyneither(_tmp392,sizeof(char),8));}_LL93: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp18D=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp189;if(_tmp18D->tag != 2)goto _LL95;}
_LL94: {const char*_tmp393;return(_tmp393="cdecl",_tag_dyneither(_tmp393,sizeof(
char),6));}_LL95: {struct Cyc_Absyn_Fastcall_att_struct*_tmp18E=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp189;if(_tmp18E->tag != 3)goto _LL97;}_LL96: {const char*_tmp394;return(_tmp394="fastcall",
_tag_dyneither(_tmp394,sizeof(char),9));}_LL97: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp18F=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp189;if(_tmp18F->tag != 4)goto
_LL99;}_LL98: {const char*_tmp395;return(_tmp395="noreturn",_tag_dyneither(
_tmp395,sizeof(char),9));}_LL99: {struct Cyc_Absyn_Const_att_struct*_tmp190=(
struct Cyc_Absyn_Const_att_struct*)_tmp189;if(_tmp190->tag != 5)goto _LL9B;}_LL9A: {
const char*_tmp396;return(_tmp396="const",_tag_dyneither(_tmp396,sizeof(char),6));}
_LL9B: {struct Cyc_Absyn_Aligned_att_struct*_tmp191=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp189;if(_tmp191->tag != 6)goto _LL9D;else{_tmp192=_tmp191->f1;}}_LL9C: if(
_tmp192 == - 1){const char*_tmp397;return(_tmp397="aligned",_tag_dyneither(_tmp397,
sizeof(char),8));}else{switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LLC1: {const char*_tmp39B;void*_tmp39A[1];struct Cyc_Int_pa_struct _tmp399;return(
struct _dyneither_ptr)((_tmp399.tag=1,((_tmp399.f1=(unsigned long)_tmp192,((
_tmp39A[0]=& _tmp399,Cyc_aprintf(((_tmp39B="aligned(%d)",_tag_dyneither(_tmp39B,
sizeof(char),12))),_tag_dyneither(_tmp39A,sizeof(void*),1))))))));}case Cyc_Cyclone_Vc_c:
_LLC2: {const char*_tmp39F;void*_tmp39E[1];struct Cyc_Int_pa_struct _tmp39D;return(
struct _dyneither_ptr)((_tmp39D.tag=1,((_tmp39D.f1=(unsigned long)_tmp192,((
_tmp39E[0]=& _tmp39D,Cyc_aprintf(((_tmp39F="align(%d)",_tag_dyneither(_tmp39F,
sizeof(char),10))),_tag_dyneither(_tmp39E,sizeof(void*),1))))))));}}}_LL9D: {
struct Cyc_Absyn_Packed_att_struct*_tmp193=(struct Cyc_Absyn_Packed_att_struct*)
_tmp189;if(_tmp193->tag != 7)goto _LL9F;}_LL9E: {const char*_tmp3A0;return(_tmp3A0="packed",
_tag_dyneither(_tmp3A0,sizeof(char),7));}_LL9F: {struct Cyc_Absyn_Section_att_struct*
_tmp194=(struct Cyc_Absyn_Section_att_struct*)_tmp189;if(_tmp194->tag != 8)goto
_LLA1;else{_tmp195=_tmp194->f1;}}_LLA0: {const char*_tmp3A4;void*_tmp3A3[1];
struct Cyc_String_pa_struct _tmp3A2;return(struct _dyneither_ptr)((_tmp3A2.tag=0,((
_tmp3A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp195),((_tmp3A3[0]=&
_tmp3A2,Cyc_aprintf(((_tmp3A4="section(\"%s\")",_tag_dyneither(_tmp3A4,sizeof(
char),14))),_tag_dyneither(_tmp3A3,sizeof(void*),1))))))));}_LLA1: {struct Cyc_Absyn_Nocommon_att_struct*
_tmp196=(struct Cyc_Absyn_Nocommon_att_struct*)_tmp189;if(_tmp196->tag != 9)goto
_LLA3;}_LLA2: {const char*_tmp3A5;return(_tmp3A5="nocommon",_tag_dyneither(
_tmp3A5,sizeof(char),9));}_LLA3: {struct Cyc_Absyn_Shared_att_struct*_tmp197=(
struct Cyc_Absyn_Shared_att_struct*)_tmp189;if(_tmp197->tag != 10)goto _LLA5;}_LLA4: {
const char*_tmp3A6;return(_tmp3A6="shared",_tag_dyneither(_tmp3A6,sizeof(char),7));}
_LLA5: {struct Cyc_Absyn_Unused_att_struct*_tmp198=(struct Cyc_Absyn_Unused_att_struct*)
_tmp189;if(_tmp198->tag != 11)goto _LLA7;}_LLA6: {const char*_tmp3A7;return(_tmp3A7="unused",
_tag_dyneither(_tmp3A7,sizeof(char),7));}_LLA7: {struct Cyc_Absyn_Weak_att_struct*
_tmp199=(struct Cyc_Absyn_Weak_att_struct*)_tmp189;if(_tmp199->tag != 12)goto _LLA9;}
_LLA8: {const char*_tmp3A8;return(_tmp3A8="weak",_tag_dyneither(_tmp3A8,sizeof(
char),5));}_LLA9: {struct Cyc_Absyn_Dllimport_att_struct*_tmp19A=(struct Cyc_Absyn_Dllimport_att_struct*)
_tmp189;if(_tmp19A->tag != 13)goto _LLAB;}_LLAA: {const char*_tmp3A9;return(_tmp3A9="dllimport",
_tag_dyneither(_tmp3A9,sizeof(char),10));}_LLAB: {struct Cyc_Absyn_Dllexport_att_struct*
_tmp19B=(struct Cyc_Absyn_Dllexport_att_struct*)_tmp189;if(_tmp19B->tag != 14)goto
_LLAD;}_LLAC: {const char*_tmp3AA;return(_tmp3AA="dllexport",_tag_dyneither(
_tmp3AA,sizeof(char),10));}_LLAD: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp19C=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp189;if(_tmp19C->tag
!= 15)goto _LLAF;}_LLAE: {const char*_tmp3AB;return(_tmp3AB="no_instrument_function",
_tag_dyneither(_tmp3AB,sizeof(char),23));}_LLAF: {struct Cyc_Absyn_Constructor_att_struct*
_tmp19D=(struct Cyc_Absyn_Constructor_att_struct*)_tmp189;if(_tmp19D->tag != 16)
goto _LLB1;}_LLB0: {const char*_tmp3AC;return(_tmp3AC="constructor",_tag_dyneither(
_tmp3AC,sizeof(char),12));}_LLB1: {struct Cyc_Absyn_Destructor_att_struct*_tmp19E=(
struct Cyc_Absyn_Destructor_att_struct*)_tmp189;if(_tmp19E->tag != 17)goto _LLB3;}
_LLB2: {const char*_tmp3AD;return(_tmp3AD="destructor",_tag_dyneither(_tmp3AD,
sizeof(char),11));}_LLB3: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp19F=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp189;if(_tmp19F->tag
!= 18)goto _LLB5;}_LLB4: {const char*_tmp3AE;return(_tmp3AE="no_check_memory_usage",
_tag_dyneither(_tmp3AE,sizeof(char),22));}_LLB5: {struct Cyc_Absyn_Format_att_struct*
_tmp1A0=(struct Cyc_Absyn_Format_att_struct*)_tmp189;if(_tmp1A0->tag != 19)goto
_LLB7;else{_tmp1A1=_tmp1A0->f1;if(_tmp1A1 != Cyc_Absyn_Printf_ft)goto _LLB7;
_tmp1A2=_tmp1A0->f2;_tmp1A3=_tmp1A0->f3;}}_LLB6: {const char*_tmp3B3;void*_tmp3B2[
2];struct Cyc_Int_pa_struct _tmp3B1;struct Cyc_Int_pa_struct _tmp3B0;return(struct
_dyneither_ptr)((_tmp3B0.tag=1,((_tmp3B0.f1=(unsigned int)_tmp1A3,((_tmp3B1.tag=
1,((_tmp3B1.f1=(unsigned int)_tmp1A2,((_tmp3B2[0]=& _tmp3B1,((_tmp3B2[1]=& _tmp3B0,
Cyc_aprintf(((_tmp3B3="format(printf,%u,%u)",_tag_dyneither(_tmp3B3,sizeof(char),
21))),_tag_dyneither(_tmp3B2,sizeof(void*),2))))))))))))));}_LLB7: {struct Cyc_Absyn_Format_att_struct*
_tmp1A4=(struct Cyc_Absyn_Format_att_struct*)_tmp189;if(_tmp1A4->tag != 19)goto
_LLB9;else{_tmp1A5=_tmp1A4->f1;if(_tmp1A5 != Cyc_Absyn_Scanf_ft)goto _LLB9;_tmp1A6=
_tmp1A4->f2;_tmp1A7=_tmp1A4->f3;}}_LLB8: {const char*_tmp3B8;void*_tmp3B7[2];
struct Cyc_Int_pa_struct _tmp3B6;struct Cyc_Int_pa_struct _tmp3B5;return(struct
_dyneither_ptr)((_tmp3B5.tag=1,((_tmp3B5.f1=(unsigned int)_tmp1A7,((_tmp3B6.tag=
1,((_tmp3B6.f1=(unsigned int)_tmp1A6,((_tmp3B7[0]=& _tmp3B6,((_tmp3B7[1]=& _tmp3B5,
Cyc_aprintf(((_tmp3B8="format(scanf,%u,%u)",_tag_dyneither(_tmp3B8,sizeof(char),
20))),_tag_dyneither(_tmp3B7,sizeof(void*),2))))))))))))));}_LLB9: {struct Cyc_Absyn_Initializes_att_struct*
_tmp1A8=(struct Cyc_Absyn_Initializes_att_struct*)_tmp189;if(_tmp1A8->tag != 20)
goto _LLBB;else{_tmp1A9=_tmp1A8->f1;}}_LLBA: {const char*_tmp3BC;void*_tmp3BB[1];
struct Cyc_Int_pa_struct _tmp3BA;return(struct _dyneither_ptr)((_tmp3BA.tag=1,((
_tmp3BA.f1=(unsigned long)_tmp1A9,((_tmp3BB[0]=& _tmp3BA,Cyc_aprintf(((_tmp3BC="initializes(%d)",
_tag_dyneither(_tmp3BC,sizeof(char),16))),_tag_dyneither(_tmp3BB,sizeof(void*),1))))))));}
_LLBB: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp1AA=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp189;if(_tmp1AA->tag != 21)goto _LLBD;else{_tmp1AB=_tmp1AA->f1;}}_LLBC: {const
char*_tmp3C0;void*_tmp3BF[1];struct Cyc_Int_pa_struct _tmp3BE;return(struct
_dyneither_ptr)((_tmp3BE.tag=1,((_tmp3BE.f1=(unsigned long)_tmp1AB,((_tmp3BF[0]=&
_tmp3BE,Cyc_aprintf(((_tmp3C0="noliveunique(%d)",_tag_dyneither(_tmp3C0,sizeof(
char),17))),_tag_dyneither(_tmp3BF,sizeof(void*),1))))))));}_LLBD: {struct Cyc_Absyn_Pure_att_struct*
_tmp1AC=(struct Cyc_Absyn_Pure_att_struct*)_tmp189;if(_tmp1AC->tag != 22)goto _LLBF;}
_LLBE: {const char*_tmp3C1;return(_tmp3C1="pure",_tag_dyneither(_tmp3C1,sizeof(
char),5));}_LLBF: {struct Cyc_Absyn_Mode_att_struct*_tmp1AD=(struct Cyc_Absyn_Mode_att_struct*)
_tmp189;if(_tmp1AD->tag != 23)goto _LL8E;else{_tmp1AE=_tmp1AD->f1;}}_LLC0: {const
char*_tmp3C5;void*_tmp3C4[1];struct Cyc_String_pa_struct _tmp3C3;return(struct
_dyneither_ptr)((_tmp3C3.tag=0,((_tmp3C3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp1AE),((_tmp3C4[0]=& _tmp3C3,Cyc_aprintf(((_tmp3C5="__mode__(\"%s\")",
_tag_dyneither(_tmp3C5,sizeof(char),15))),_tag_dyneither(_tmp3C4,sizeof(void*),1))))))));}
_LL8E:;}int Cyc_Absyn_fntype_att(void*a){void*_tmp1DE=a;int _tmp1E0;_LLC5: {struct
Cyc_Absyn_Regparm_att_struct*_tmp1DF=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp1DE;if(_tmp1DF->tag != 0)goto _LLC7;else{_tmp1E0=_tmp1DF->f1;}}_LLC6: goto _LLC8;
_LLC7: {struct Cyc_Absyn_Fastcall_att_struct*_tmp1E1=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp1DE;if(_tmp1E1->tag != 3)goto _LLC9;}_LLC8: goto _LLCA;_LLC9: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp1E2=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp1DE;if(_tmp1E2->tag != 1)goto
_LLCB;}_LLCA: goto _LLCC;_LLCB: {struct Cyc_Absyn_Cdecl_att_struct*_tmp1E3=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp1DE;if(_tmp1E3->tag != 2)goto _LLCD;}_LLCC: goto
_LLCE;_LLCD: {struct Cyc_Absyn_Noreturn_att_struct*_tmp1E4=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp1DE;if(_tmp1E4->tag != 4)goto _LLCF;}_LLCE: goto _LLD0;_LLCF: {struct Cyc_Absyn_Pure_att_struct*
_tmp1E5=(struct Cyc_Absyn_Pure_att_struct*)_tmp1DE;if(_tmp1E5->tag != 22)goto _LLD1;}
_LLD0: goto _LLD2;_LLD1: {struct Cyc_Absyn_Format_att_struct*_tmp1E6=(struct Cyc_Absyn_Format_att_struct*)
_tmp1DE;if(_tmp1E6->tag != 19)goto _LLD3;}_LLD2: goto _LLD4;_LLD3: {struct Cyc_Absyn_Const_att_struct*
_tmp1E7=(struct Cyc_Absyn_Const_att_struct*)_tmp1DE;if(_tmp1E7->tag != 5)goto _LLD5;}
_LLD4: return 1;_LLD5: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp1E8=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp1DE;if(_tmp1E8->tag != 21)goto _LLD7;}_LLD6: return 1;_LLD7: {struct Cyc_Absyn_Initializes_att_struct*
_tmp1E9=(struct Cyc_Absyn_Initializes_att_struct*)_tmp1DE;if(_tmp1E9->tag != 20)
goto _LLD9;}_LLD8: return 1;_LLD9:;_LLDA: return 0;_LLC4:;}static char _tmp1EA[3]="f0";
static struct _dyneither_ptr Cyc_Absyn_f0={_tmp1EA,_tmp1EA,_tmp1EA + 3};static struct
_dyneither_ptr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};static struct
_dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(
void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct
_dyneither_ptr**)Cyc_Absyn_field_names_v + 1)};struct _dyneither_ptr*Cyc_Absyn_fieldname(
int i);static void _tmp3D3(unsigned int*fsz,unsigned int*_tmp3D2,unsigned int*
_tmp3D1,struct _dyneither_ptr***_tmp3CF){for(*_tmp3D2=0;*_tmp3D2 < *_tmp3D1;(*
_tmp3D2)++){struct Cyc_Int_pa_struct _tmp3CD;void*_tmp3CC[1];const char*_tmp3CB;
struct _dyneither_ptr*_tmp3CA;(*_tmp3CF)[*_tmp3D2]=*_tmp3D2 < *fsz?*((struct
_dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct
_dyneither_ptr*),(int)*_tmp3D2)):((_tmp3CA=_cycalloc(sizeof(*_tmp3CA)),((_tmp3CA[
0]=(struct _dyneither_ptr)((_tmp3CD.tag=1,((_tmp3CD.f1=(unsigned long)((int)*
_tmp3D2),((_tmp3CC[0]=& _tmp3CD,Cyc_aprintf(((_tmp3CB="f%d",_tag_dyneither(
_tmp3CB,sizeof(char),4))),_tag_dyneither(_tmp3CC,sizeof(void*),1)))))))),_tmp3CA))));}}
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){unsigned int fsz=
_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));if(i >= 
fsz){unsigned int _tmp3D2;unsigned int _tmp3D1;struct _dyneither_ptr _tmp3D0;struct
_dyneither_ptr**_tmp3CF;unsigned int _tmp3CE;Cyc_Absyn_field_names=((_tmp3CE=(
unsigned int)(i + 1),((_tmp3CF=(struct _dyneither_ptr**)_cycalloc(_check_times(
sizeof(struct _dyneither_ptr*),_tmp3CE)),((_tmp3D0=_tag_dyneither(_tmp3CF,sizeof(
struct _dyneither_ptr*),_tmp3CE),((((_tmp3D1=_tmp3CE,_tmp3D3(& fsz,& _tmp3D2,&
_tmp3D1,& _tmp3CF))),_tmp3D0))))))));}return*((struct _dyneither_ptr**)
_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU info){union Cyc_Absyn_AggrInfoU
_tmp1F3=info;struct _tuple2 _tmp1F4;enum Cyc_Absyn_AggrKind _tmp1F5;struct _tuple0*
_tmp1F6;struct Cyc_Absyn_Aggrdecl**_tmp1F7;struct Cyc_Absyn_Aggrdecl*_tmp1F8;
struct Cyc_Absyn_Aggrdecl _tmp1F9;enum Cyc_Absyn_AggrKind _tmp1FA;struct _tuple0*
_tmp1FB;_LLDC: if((_tmp1F3.UnknownAggr).tag != 1)goto _LLDE;_tmp1F4=(struct _tuple2)(
_tmp1F3.UnknownAggr).val;_tmp1F5=_tmp1F4.f1;_tmp1F6=_tmp1F4.f2;_LLDD: {struct
_tuple10 _tmp3D4;return(_tmp3D4.f1=_tmp1F5,((_tmp3D4.f2=_tmp1F6,_tmp3D4)));}_LLDE:
if((_tmp1F3.KnownAggr).tag != 2)goto _LLDB;_tmp1F7=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1F3.KnownAggr).val;_tmp1F8=*_tmp1F7;_tmp1F9=*_tmp1F8;_tmp1FA=_tmp1F9.kind;
_tmp1FB=_tmp1F9.name;_LLDF: {struct _tuple10 _tmp3D5;return(_tmp3D5.f1=_tmp1FA,((
_tmp3D5.f2=_tmp1FB,_tmp3D5)));}_LLDB:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info){union Cyc_Absyn_AggrInfoU _tmp1FE=info;struct _tuple2
_tmp1FF;struct Cyc_Absyn_Aggrdecl**_tmp200;struct Cyc_Absyn_Aggrdecl*_tmp201;_LLE1:
if((_tmp1FE.UnknownAggr).tag != 1)goto _LLE3;_tmp1FF=(struct _tuple2)(_tmp1FE.UnknownAggr).val;
_LLE2: {const char*_tmp3D8;void*_tmp3D7;(_tmp3D7=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3D8="unchecked aggrdecl",
_tag_dyneither(_tmp3D8,sizeof(char),19))),_tag_dyneither(_tmp3D7,sizeof(void*),0)));}
_LLE3: if((_tmp1FE.KnownAggr).tag != 2)goto _LLE0;_tmp200=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1FE.KnownAggr).val;_tmp201=*_tmp200;_LLE4: return _tmp201;_LLE0:;}int Cyc_Absyn_is_union_type(
void*t){void*_tmp204=Cyc_Tcutil_compress(t);enum Cyc_Absyn_AggrKind _tmp206;
struct Cyc_Absyn_AggrInfo _tmp208;union Cyc_Absyn_AggrInfoU _tmp209;_LLE6: {struct
Cyc_Absyn_AnonAggrType_struct*_tmp205=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp204;if(_tmp205->tag != 13)goto _LLE8;else{_tmp206=_tmp205->f1;if(_tmp206 != Cyc_Absyn_UnionA)
goto _LLE8;}}_LLE7: return 1;_LLE8: {struct Cyc_Absyn_AggrType_struct*_tmp207=(
struct Cyc_Absyn_AggrType_struct*)_tmp204;if(_tmp207->tag != 12)goto _LLEA;else{
_tmp208=_tmp207->f1;_tmp209=_tmp208.aggr_info;}}_LLE9: return(Cyc_Absyn_aggr_kinded_name(
_tmp209)).f1 == Cyc_Absyn_UnionA;_LLEA:;_LLEB: return 0;_LLE5:;}int Cyc_Absyn_is_nontagged_union_type(
void*t){void*_tmp20A=Cyc_Tcutil_compress(t);enum Cyc_Absyn_AggrKind _tmp20C;
struct Cyc_Absyn_AggrInfo _tmp20E;union Cyc_Absyn_AggrInfoU _tmp20F;_LLED: {struct
Cyc_Absyn_AnonAggrType_struct*_tmp20B=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp20A;if(_tmp20B->tag != 13)goto _LLEF;else{_tmp20C=_tmp20B->f1;if(_tmp20C != Cyc_Absyn_UnionA)
goto _LLEF;}}_LLEE: return 1;_LLEF: {struct Cyc_Absyn_AggrType_struct*_tmp20D=(
struct Cyc_Absyn_AggrType_struct*)_tmp20A;if(_tmp20D->tag != 12)goto _LLF1;else{
_tmp20E=_tmp20D->f1;_tmp20F=_tmp20E.aggr_info;}}_LLF0: {union Cyc_Absyn_AggrInfoU
_tmp210=_tmp20F;struct Cyc_Absyn_Aggrdecl**_tmp211;struct Cyc_Absyn_Aggrdecl*
_tmp212;struct _tuple2 _tmp213;enum Cyc_Absyn_AggrKind _tmp214;struct Cyc_Core_Opt*
_tmp215;struct _tuple2 _tmp216;enum Cyc_Absyn_AggrKind _tmp217;struct Cyc_Core_Opt*
_tmp218;struct Cyc_Core_Opt _tmp219;int _tmp21A;_LLF4: if((_tmp210.KnownAggr).tag != 
2)goto _LLF6;_tmp211=(struct Cyc_Absyn_Aggrdecl**)(_tmp210.KnownAggr).val;_tmp212=*
_tmp211;_LLF5: return _tmp212->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp212->impl))->tagged == 0;_LLF6: if((_tmp210.UnknownAggr).tag != 1)
goto _LLF8;_tmp213=(struct _tuple2)(_tmp210.UnknownAggr).val;_tmp214=_tmp213.f1;
_tmp215=_tmp213.f3;if(_tmp215 != 0)goto _LLF8;_LLF7: return _tmp214 == Cyc_Absyn_UnionA;
_LLF8: if((_tmp210.UnknownAggr).tag != 1)goto _LLF3;_tmp216=(struct _tuple2)(_tmp210.UnknownAggr).val;
_tmp217=_tmp216.f1;_tmp218=_tmp216.f3;if(_tmp218 == 0)goto _LLF3;_tmp219=*_tmp218;
_tmp21A=(int)_tmp219.v;_LLF9: return _tmp217 == Cyc_Absyn_UnionA  && !_tmp21A;_LLF3:;}
_LLF1:;_LLF2: return 0;_LLEC:;}int Cyc_Absyn_is_aggr_type(void*t){void*_tmp21B=Cyc_Tcutil_compress(
t);_LLFB: {struct Cyc_Absyn_DatatypeType_struct*_tmp21C=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp21B;if(_tmp21C->tag != 3)goto _LLFD;}_LLFC: goto _LLFE;_LLFD: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp21D=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp21B;if(_tmp21D->tag != 4)
goto _LLFF;}_LLFE: goto _LL100;_LLFF: {struct Cyc_Absyn_TupleType_struct*_tmp21E=(
struct Cyc_Absyn_TupleType_struct*)_tmp21B;if(_tmp21E->tag != 11)goto _LL101;}
_LL100: goto _LL102;_LL101: {struct Cyc_Absyn_AggrType_struct*_tmp21F=(struct Cyc_Absyn_AggrType_struct*)
_tmp21B;if(_tmp21F->tag != 12)goto _LL103;}_LL102: goto _LL104;_LL103: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp220=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp21B;if(_tmp220->tag != 13)goto
_LL105;}_LL104: return 1;_LL105:;_LL106: return 0;_LLFA:;}struct Cyc_Absyn_Porton_d_struct
Cyc_Absyn_Porton_d_val={14};struct Cyc_Absyn_Portoff_d_struct Cyc_Absyn_Portoff_d_val={
15};int Cyc_Absyn_porting_c_code=0;int Cyc_Absyn_no_regions=0;
