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
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_Aggr_td_struct{int
tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_struct{int tag;struct
Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;};struct Cyc_Absyn_TypeDecl{void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};
struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
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
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};extern struct Cyc_Absyn_HeapRgn_struct
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
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
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
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;};
struct Cyc_Absyn_Var_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*
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
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
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
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_TypeDecl*
Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct
_tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct
_tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(
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
_dyneither_ptr*s2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern
char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[
7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*
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
struct _tuple11 _tmp230;struct _tuple11 _tmp3=(_tmp230.f1=_tmp0,((_tmp230.f2=_tmp1,
_tmp230)));union Cyc_Absyn_Nmspace _tmp4;int _tmp5;union Cyc_Absyn_Nmspace _tmp6;int
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
if(C_scope){union Cyc_Absyn_Nmspace _tmp231;return((_tmp231.C_n).val=x,(((_tmp231.C_n).tag=
3,_tmp231)));}else{union Cyc_Absyn_Nmspace _tmp232;return((_tmp232.Abs_n).val=x,(((
_tmp232.Abs_n).tag=2,_tmp232)));}}union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct
Cyc_List_List*x){union Cyc_Absyn_Nmspace _tmp233;return((_tmp233.Rel_n).val=x,(((
_tmp233.Rel_n).tag=1,_tmp233)));}union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={
1,0}};int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){union Cyc_Absyn_Nmspace
_tmp23=(*qv).f1;struct Cyc_List_List*_tmp24;struct Cyc_List_List*_tmp25;int _tmp26;
_LL16: if((_tmp23.Rel_n).tag != 1)goto _LL18;_tmp24=(struct Cyc_List_List*)(_tmp23.Rel_n).val;
if(_tmp24 != 0)goto _LL18;_LL17: goto _LL19;_LL18: if((_tmp23.Abs_n).tag != 2)goto
_LL1A;_tmp25=(struct Cyc_List_List*)(_tmp23.Abs_n).val;if(_tmp25 != 0)goto _LL1A;
_LL19: goto _LL1B;_LL1A: if((_tmp23.Loc_n).tag != 4)goto _LL1C;_tmp26=(int)(_tmp23.Loc_n).val;
_LL1B: return 0;_LL1C:;_LL1D: return 1;_LL15:;}static int Cyc_Absyn_new_type_counter=0;
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){struct Cyc_Absyn_Evar_struct
_tmp236;struct Cyc_Absyn_Evar_struct*_tmp235;return(void*)((_tmp235=_cycalloc(
sizeof(*_tmp235)),((_tmp235[0]=((_tmp236.tag=1,((_tmp236.f1=k,((_tmp236.f2=0,((
_tmp236.f3=Cyc_Absyn_new_type_counter ++,((_tmp236.f4=env,_tmp236)))))))))),
_tmp235))));}void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,tenv);}struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){struct Cyc_Absyn_Tqual _tmp237;
return(_tmp237.print_const=x.print_const  || y.print_const,((_tmp237.q_volatile=x.q_volatile
 || y.q_volatile,((_tmp237.q_restrict=x.q_restrict  || y.q_restrict,((_tmp237.real_const=
x.real_const  || y.real_const,((_tmp237.loc=Cyc_Position_segment_join(x.loc,y.loc),
_tmp237)))))))));}struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Tqual _tmp238;return(_tmp238.print_const=0,((_tmp238.q_volatile=
0,((_tmp238.q_restrict=0,((_tmp238.real_const=0,((_tmp238.loc=loc,_tmp238)))))))));}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Tqual _tmp239;return(_tmp239.print_const=1,((_tmp239.q_volatile=0,((
_tmp239.q_restrict=0,((_tmp239.real_const=1,((_tmp239.loc=loc,_tmp239)))))))));}
struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={0,{.Int_c={4,{Cyc_Absyn_Unsigned,
1}}}};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(void*)& Cyc_Absyn_one_b_raw,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=&
Cyc_Absyn_exp_unsigned_one_v;static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={
1,& Cyc_Absyn_exp_unsigned_one_v};void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo
udi){union Cyc_Absyn_DatatypeInfoU _tmp23A;return((_tmp23A.UnknownDatatype).val=
udi,(((_tmp23A.UnknownDatatype).tag=1,_tmp23A)));}extern union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){union Cyc_Absyn_DatatypeInfoU
_tmp23B;return((_tmp23B.KnownDatatype).val=d,(((_tmp23B.KnownDatatype).tag=2,
_tmp23B)));}union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(
struct Cyc_Absyn_UnknownDatatypeFieldInfo s){union Cyc_Absyn_DatatypeFieldInfoU
_tmp23C;return((_tmp23C.UnknownDatatypefield).val=s,(((_tmp23C.UnknownDatatypefield).tag=
1,_tmp23C)));}union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(
struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){struct _tuple1
_tmp23F;union Cyc_Absyn_DatatypeFieldInfoU _tmp23E;return((_tmp23E.KnownDatatypefield).val=((
_tmp23F.f1=dd,((_tmp23F.f2=df,_tmp23F)))),(((_tmp23E.KnownDatatypefield).tag=2,
_tmp23E)));}union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind
ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){struct _tuple2 _tmp242;union Cyc_Absyn_AggrInfoU
_tmp241;return((_tmp241.UnknownAggr).val=((_tmp242.f1=ak,((_tmp242.f2=n,((
_tmp242.f3=tagged,_tmp242)))))),(((_tmp241.UnknownAggr).tag=1,_tmp241)));}union
Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){union Cyc_Absyn_AggrInfoU
_tmp243;return((_tmp243.KnownAggr).val=ad,(((_tmp243.KnownAggr).tag=2,_tmp243)));}
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x){union Cyc_Absyn_Constraint*
_tmp244;return(_tmp244=_cycalloc(sizeof(*_tmp244)),(((_tmp244->Eq_constr).val=(
void*)x,(((_tmp244->Eq_constr).tag=1,_tmp244)))));}union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref(){
union Cyc_Absyn_Constraint*_tmp245;return(_tmp245=_cycalloc_atomic(sizeof(*
_tmp245)),(((_tmp245->No_constr).val=0,(((_tmp245->No_constr).tag=3,_tmp245)))));}
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
_tmp246;(_tmp246=& x->Forward_constr,((_tmp246->tag=2,_tmp246->val=_tmp41)));}
return _tmp41;}_LL1E:;}void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp43=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint
_tmp44;void*_tmp45;_LL26: _tmp44=*_tmp43;if((_tmp44.Eq_constr).tag != 1)goto _LL28;
_tmp45=(void*)(_tmp44.Eq_constr).val;_LL27: return _tmp45;_LL28:;_LL29: {const char*
_tmp249;void*_tmp248;(_tmp248=0,Cyc_Tcutil_impos(((_tmp249="conref_val",
_tag_dyneither(_tmp249,sizeof(char),11))),_tag_dyneither(_tmp248,sizeof(void*),0)));}
_LL25:;}void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x){union Cyc_Absyn_Constraint*
_tmp48=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint _tmp49;void*_tmp4A;
_LL2B: _tmp49=*_tmp48;if((_tmp49.Eq_constr).tag != 1)goto _LL2D;_tmp4A=(void*)(
_tmp49.Eq_constr).val;_LL2C: return _tmp4A;_LL2D:;_LL2E: return y;_LL2A:;}void*Cyc_Absyn_conref_constr(
void*y,union Cyc_Absyn_Constraint*x){x=Cyc_Absyn_compress_conref(x);{union Cyc_Absyn_Constraint*
_tmp4B=x;union Cyc_Absyn_Constraint _tmp4C;void*_tmp4D;_LL30: _tmp4C=*_tmp4B;if((
_tmp4C.Eq_constr).tag != 1)goto _LL32;_tmp4D=(void*)(_tmp4C.Eq_constr).val;_LL31:
return _tmp4D;_LL32:;_LL33:{struct _union_Constraint_Eq_constr*_tmp24A;(_tmp24A=& x->Eq_constr,((
_tmp24A->tag=1,_tmp24A->val=y)));}return y;_LL2F:;};}void*Cyc_Absyn_compress_kb(
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
struct Cyc_Core_Opt*_tmp24B;*_tmp67=((_tmp24B=_cycalloc(sizeof(*_tmp24B)),((
_tmp24B->v=Cyc_Tcutil_kind_to_bound(_tmp68),_tmp24B))));}return _tmp68;_LL3F:;}
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
static struct Cyc_Absyn_DatatypeType_struct Cyc_Absyn_exn_typ_tt={3,{{.KnownDatatype={
2,& Cyc_Absyn_exn_tud}},0}};void*Cyc_Absyn_exn_typ(){static struct Cyc_Core_Opt*
exn_type_val=0;if(exn_type_val == 0){struct Cyc_Core_Opt*_tmp24C;exn_type_val=((
_tmp24C=_cycalloc(sizeof(*_tmp24C)),((_tmp24C->v=Cyc_Absyn_at_typ((void*)& Cyc_Absyn_exn_typ_tt,(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp24C))));}return(void*)exn_type_val->v;}static char _tmp86[9]="PrintArg";static
struct _dyneither_ptr Cyc_Absyn_printarg_str={_tmp86,_tmp86,_tmp86 + 9};static char
_tmp87[9]="ScanfArg";static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp87,
_tmp87,_tmp87 + 9};static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={
2,0}},& Cyc_Absyn_printarg_str};static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={
2,0}},& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=&
Cyc_Absyn_datatype_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=&
Cyc_Absyn_datatype_scanf_arg_qvar_p;static char _tmp88[14]="unique_region";static
struct _dyneither_ptr Cyc_Absyn_uniquergn_str={_tmp88,_tmp88,_tmp88 + 14};static
char _tmp89[5]="Core";static struct _dyneither_ptr Cyc_Absyn_Core_str={_tmp89,_tmp89,
_tmp89 + 5};static struct Cyc_List_List Cyc_Absyn_uniquergn_nmspace={(void*)((struct
_dyneither_ptr*)& Cyc_Absyn_Core_str),(struct Cyc_List_List*)0};static struct
_tuple0 Cyc_Absyn_uniquergn_qvar_p={{.Abs_n={2,(struct Cyc_List_List*)& Cyc_Absyn_uniquergn_nmspace}},&
Cyc_Absyn_uniquergn_str};struct _tuple0*Cyc_Absyn_uniquergn_qvar=& Cyc_Absyn_uniquergn_qvar_p;
static struct Cyc_Absyn_RgnHandleType_struct Cyc_Absyn_uniquergn_typ={16,(void*)((
void*)& Cyc_Absyn_UniqueRgn_val)};static struct Cyc_Absyn_Vardecl Cyc_Absyn_uniquergn_var_d={
Cyc_Absyn_Extern,& Cyc_Absyn_uniquergn_qvar_p,{0,0,0,0,0},(void*)& Cyc_Absyn_uniquergn_typ,
0,0,0,0};static struct Cyc_Absyn_Global_b_struct Cyc_Absyn_uniquergn_bind_p={1,& Cyc_Absyn_uniquergn_var_d};
static struct Cyc_Absyn_Var_e_struct Cyc_Absyn_uniquergn_exp_r={1,& Cyc_Absyn_uniquergn_qvar_p,(
void*)((void*)& Cyc_Absyn_uniquergn_bind_p)};static struct Cyc_Core_Opt Cyc_Absyn_uniquergn_typ_opt={(
void*)((void*)& Cyc_Absyn_uniquergn_typ)};static struct Cyc_Absyn_Exp Cyc_Absyn_uniquergn_exp_p={(
struct Cyc_Core_Opt*)& Cyc_Absyn_uniquergn_typ_opt,(void*)& Cyc_Absyn_uniquergn_exp_r,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp=&
Cyc_Absyn_uniquergn_exp_p;static void**Cyc_Absyn_string_t_opt=0;void*Cyc_Absyn_string_typ(
void*rgn){void*_tmp8D=Cyc_Tcutil_compress(rgn);_LL5D: {struct Cyc_Absyn_HeapRgn_struct*
_tmp8E=(struct Cyc_Absyn_HeapRgn_struct*)_tmp8D;if(_tmp8E->tag != 21)goto _LL5F;}
_LL5E: if(Cyc_Absyn_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void**_tmp24D;Cyc_Absyn_string_t_opt=((_tmp24D=_cycalloc(
sizeof(*_tmp24D)),((_tmp24D[0]=t,_tmp24D))));}return*((void**)_check_null(Cyc_Absyn_string_t_opt));
_LL5F:;_LL60: return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(
0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);_LL5C:;}static void**
Cyc_Absyn_const_string_t_opt=0;void*Cyc_Absyn_const_string_typ(void*rgn){void*
_tmp90=Cyc_Tcutil_compress(rgn);_LL62: {struct Cyc_Absyn_HeapRgn_struct*_tmp91=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp90;if(_tmp91->tag != 21)goto _LL64;}_LL63: if(
Cyc_Absyn_const_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void**_tmp24E;Cyc_Absyn_const_string_t_opt=((_tmp24E=
_cycalloc(sizeof(*_tmp24E)),((_tmp24E[0]=t,_tmp24E))));}return*((void**)
_check_null(Cyc_Absyn_const_string_t_opt));_LL64:;_LL65: return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);_LL61:;}void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,void*b,union Cyc_Absyn_Constraint*zeroterm){struct Cyc_Absyn_PointerType_struct
_tmp258;struct Cyc_Absyn_PtrAtts _tmp257;struct Cyc_Absyn_PtrInfo _tmp256;struct Cyc_Absyn_PointerType_struct*
_tmp255;return(void*)((_tmp255=_cycalloc(sizeof(*_tmp255)),((_tmp255[0]=((
_tmp258.tag=5,((_tmp258.f1=((_tmp256.elt_typ=t,((_tmp256.elt_tq=tq,((_tmp256.ptr_atts=((
_tmp257.rgn=r,((_tmp257.nullable=Cyc_Absyn_true_conref,((_tmp257.bounds=((union
Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp257.zero_term=
zeroterm,((_tmp257.ptrloc=0,_tmp257)))))))))),_tmp256)))))),_tmp258)))),_tmp255))));}
void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zeroterm){struct Cyc_Absyn_PointerType_struct _tmp262;struct Cyc_Absyn_PtrAtts
_tmp261;struct Cyc_Absyn_PtrInfo _tmp260;struct Cyc_Absyn_PointerType_struct*
_tmp25F;return(void*)((_tmp25F=_cycalloc(sizeof(*_tmp25F)),((_tmp25F[0]=((
_tmp262.tag=5,((_tmp262.f1=((_tmp260.elt_typ=t,((_tmp260.elt_tq=tq,((_tmp260.ptr_atts=((
_tmp261.rgn=r,((_tmp261.nullable=Cyc_Absyn_false_conref,((_tmp261.bounds=((union
Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp261.zero_term=
zeroterm,((_tmp261.ptrloc=0,_tmp261)))))))))),_tmp260)))))),_tmp262)))),_tmp25F))));}
void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zeroterm){struct Cyc_Absyn_PointerType_struct _tmp26C;struct Cyc_Absyn_PtrAtts
_tmp26B;struct Cyc_Absyn_PtrInfo _tmp26A;struct Cyc_Absyn_PointerType_struct*
_tmp269;return(void*)((_tmp269=_cycalloc(sizeof(*_tmp269)),((_tmp269[0]=((
_tmp26C.tag=5,((_tmp26C.f1=((_tmp26A.elt_typ=t,((_tmp26A.elt_tq=tq,((_tmp26A.ptr_atts=((
_tmp26B.rgn=r,((_tmp26B.nullable=Cyc_Absyn_true_conref,((_tmp26B.bounds=Cyc_Absyn_bounds_one_conref,((
_tmp26B.zero_term=zeroterm,((_tmp26B.ptrloc=0,_tmp26B)))))))))),_tmp26A)))))),
_tmp26C)))),_tmp269))));}void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){
struct Cyc_Absyn_PointerType_struct _tmp276;struct Cyc_Absyn_PtrAtts _tmp275;struct
Cyc_Absyn_PtrInfo _tmp274;struct Cyc_Absyn_PointerType_struct*_tmp273;return(void*)((
_tmp273=_cycalloc(sizeof(*_tmp273)),((_tmp273[0]=((_tmp276.tag=5,((_tmp276.f1=((
_tmp274.elt_typ=t,((_tmp274.elt_tq=tq,((_tmp274.ptr_atts=((_tmp275.rgn=(void*)&
Cyc_Absyn_HeapRgn_val,((_tmp275.nullable=Cyc_Absyn_true_conref,((_tmp275.bounds=
Cyc_Absyn_bounds_one_conref,((_tmp275.zero_term=Cyc_Absyn_false_conref,((_tmp275.ptrloc=
0,_tmp275)))))))))),_tmp274)))))),_tmp276)))),_tmp273))));}void*Cyc_Absyn_at_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){struct
Cyc_Absyn_PointerType_struct _tmp280;struct Cyc_Absyn_PtrAtts _tmp27F;struct Cyc_Absyn_PtrInfo
_tmp27E;struct Cyc_Absyn_PointerType_struct*_tmp27D;return(void*)((_tmp27D=
_cycalloc(sizeof(*_tmp27D)),((_tmp27D[0]=((_tmp280.tag=5,((_tmp280.f1=((_tmp27E.elt_typ=
t,((_tmp27E.elt_tq=tq,((_tmp27E.ptr_atts=((_tmp27F.rgn=r,((_tmp27F.nullable=Cyc_Absyn_false_conref,((
_tmp27F.bounds=Cyc_Absyn_bounds_one_conref,((_tmp27F.zero_term=zeroterm,((
_tmp27F.ptrloc=0,_tmp27F)))))))))),_tmp27E)))))),_tmp280)))),_tmp27D))));}void*
Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zeroterm){struct Cyc_Absyn_PointerType_struct _tmp28A;struct Cyc_Absyn_PtrAtts
_tmp289;struct Cyc_Absyn_PtrInfo _tmp288;struct Cyc_Absyn_PointerType_struct*
_tmp287;return(void*)((_tmp287=_cycalloc(sizeof(*_tmp287)),((_tmp287[0]=((
_tmp28A.tag=5,((_tmp28A.f1=((_tmp288.elt_typ=t,((_tmp288.elt_tq=tq,((_tmp288.ptr_atts=((
_tmp289.rgn=r,((_tmp289.nullable=Cyc_Absyn_true_conref,((_tmp289.bounds=Cyc_Absyn_bounds_dyneither_conref,((
_tmp289.zero_term=zeroterm,((_tmp289.ptrloc=0,_tmp289)))))))))),_tmp288)))))),
_tmp28A)))),_tmp287))));}void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc){struct Cyc_Absyn_ArrayType_struct _tmp290;struct Cyc_Absyn_ArrayInfo _tmp28F;
struct Cyc_Absyn_ArrayType_struct*_tmp28E;return(void*)((_tmp28E=_cycalloc(
sizeof(*_tmp28E)),((_tmp28E[0]=((_tmp290.tag=9,((_tmp290.f1=((_tmp28F.elt_type=
elt_type,((_tmp28F.tq=tq,((_tmp28F.num_elts=num_elts,((_tmp28F.zero_term=
zero_term,((_tmp28F.zt_loc=ztloc,_tmp28F)))))))))),_tmp290)))),_tmp28E))));}
static char _tmpB5[8]="__sFILE";void*Cyc_Absyn_file_typ(){static void**file_t_opt=0;
static struct _dyneither_ptr sf_str={_tmpB5,_tmpB5,_tmpB5 + 8};static struct
_dyneither_ptr*sf=& sf_str;if(file_t_opt == 0){struct _tuple0*_tmp291;struct _tuple0*
file_t_name=(_tmp291=_cycalloc(sizeof(*_tmp291)),((_tmp291->f1=Cyc_Absyn_Abs_n(0,
0),((_tmp291->f2=sf,_tmp291)))));struct Cyc_Absyn_Aggrdecl*_tmp292;struct Cyc_Absyn_Aggrdecl*
sd=(_tmp292=_cycalloc(sizeof(*_tmp292)),((_tmp292->kind=Cyc_Absyn_StructA,((
_tmp292->sc=Cyc_Absyn_Abstract,((_tmp292->name=file_t_name,((_tmp292->tvs=0,((
_tmp292->impl=0,((_tmp292->attributes=0,_tmp292)))))))))))));struct Cyc_Absyn_AggrType_struct
_tmp29C;struct Cyc_Absyn_Aggrdecl**_tmp29B;struct Cyc_Absyn_AggrInfo _tmp29A;struct
Cyc_Absyn_AggrType_struct*_tmp299;void*file_struct_typ=(void*)((_tmp299=
_cycalloc(sizeof(*_tmp299)),((_tmp299[0]=((_tmp29C.tag=12,((_tmp29C.f1=((_tmp29A.aggr_info=
Cyc_Absyn_KnownAggr(((_tmp29B=_cycalloc(sizeof(*_tmp29B)),((_tmp29B[0]=sd,
_tmp29B))))),((_tmp29A.targs=0,_tmp29A)))),_tmp29C)))),_tmp299))));void**_tmp29D;
file_t_opt=((_tmp29D=_cycalloc(sizeof(*_tmp29D)),((_tmp29D[0]=Cyc_Absyn_at_typ(
file_struct_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp29D))));}return*file_t_opt;}void*Cyc_Absyn_void_star_typ(){static void**
void_star_t_opt=0;if(void_star_t_opt == 0){void**_tmp29E;void_star_t_opt=((
_tmp29E=_cycalloc(sizeof(*_tmp29E)),((_tmp29E[0]=Cyc_Absyn_star_typ((void*)& Cyc_Absyn_VoidType_val,(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp29E))));}return*void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={
24,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_aggr_typ(
enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name){struct Cyc_Absyn_AggrType_struct
_tmp2A8;struct _tuple0*_tmp2A7;struct Cyc_Absyn_AggrInfo _tmp2A6;struct Cyc_Absyn_AggrType_struct*
_tmp2A5;return(void*)((_tmp2A5=_cycalloc(sizeof(*_tmp2A5)),((_tmp2A5[0]=((
_tmp2A8.tag=12,((_tmp2A8.f1=((_tmp2A6.aggr_info=Cyc_Absyn_UnknownAggr(k,((
_tmp2A7=_cycalloc(sizeof(*_tmp2A7)),((_tmp2A7->f1=Cyc_Absyn_rel_ns_null,((
_tmp2A7->f2=name,_tmp2A7)))))),0),((_tmp2A6.targs=0,_tmp2A6)))),_tmp2A8)))),
_tmp2A5))));}void*Cyc_Absyn_strct(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(
Cyc_Absyn_StructA,name);}void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){
return Cyc_Absyn_aggr_typ(Cyc_Absyn_UnionA,name);}void*Cyc_Absyn_strctq(struct
_tuple0*name){struct Cyc_Absyn_AggrType_struct _tmp2AE;struct Cyc_Absyn_AggrInfo
_tmp2AD;struct Cyc_Absyn_AggrType_struct*_tmp2AC;return(void*)((_tmp2AC=_cycalloc(
sizeof(*_tmp2AC)),((_tmp2AC[0]=((_tmp2AE.tag=12,((_tmp2AE.f1=((_tmp2AD.aggr_info=
Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0),((_tmp2AD.targs=0,_tmp2AD)))),
_tmp2AE)))),_tmp2AC))));}void*Cyc_Absyn_unionq_typ(struct _tuple0*name){struct Cyc_Absyn_AggrType_struct
_tmp2B4;struct Cyc_Absyn_AggrInfo _tmp2B3;struct Cyc_Absyn_AggrType_struct*_tmp2B2;
return(void*)((_tmp2B2=_cycalloc(sizeof(*_tmp2B2)),((_tmp2B2[0]=((_tmp2B4.tag=12,((
_tmp2B4.f1=((_tmp2B3.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),((
_tmp2B3.targs=0,_tmp2B3)))),_tmp2B4)))),_tmp2B2))));}union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={
1,0}};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){struct
_tuple3 _tmp2B7;union Cyc_Absyn_Cnst _tmp2B6;return((_tmp2B6.Char_c).val=((_tmp2B7.f1=
sn,((_tmp2B7.f2=c,_tmp2B7)))),(((_tmp2B6.Char_c).tag=2,_tmp2B6)));}union Cyc_Absyn_Cnst
Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){struct _tuple4 _tmp2BA;union Cyc_Absyn_Cnst
_tmp2B9;return((_tmp2B9.Short_c).val=((_tmp2BA.f1=sn,((_tmp2BA.f2=s,_tmp2BA)))),(((
_tmp2B9.Short_c).tag=3,_tmp2B9)));}union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign
sn,int i){struct _tuple5 _tmp2BD;union Cyc_Absyn_Cnst _tmp2BC;return((_tmp2BC.Int_c).val=((
_tmp2BD.f1=sn,((_tmp2BD.f2=i,_tmp2BD)))),(((_tmp2BC.Int_c).tag=4,_tmp2BC)));}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){struct
_tuple6 _tmp2C0;union Cyc_Absyn_Cnst _tmp2BF;return((_tmp2BF.LongLong_c).val=((
_tmp2C0.f1=sn,((_tmp2C0.f2=l,_tmp2C0)))),(((_tmp2BF.LongLong_c).tag=5,_tmp2BF)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst
_tmp2C1;return((_tmp2C1.Float_c).val=s,(((_tmp2C1.Float_c).tag=6,_tmp2C1)));}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst
_tmp2C2;return((_tmp2C2.String_c).val=s,(((_tmp2C2.String_c).tag=7,_tmp2C2)));}
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Exp*_tmp2C3;return(_tmp2C3=_cycalloc(sizeof(*_tmp2C3)),((_tmp2C3->topt=
0,((_tmp2C3->r=r,((_tmp2C3->loc=loc,((_tmp2C3->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,
_tmp2C3)))))))));}struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_New_e_struct
_tmp2C6;struct Cyc_Absyn_New_e_struct*_tmp2C5;return Cyc_Absyn_new_exp((void*)((
_tmp2C5=_cycalloc(sizeof(*_tmp2C5)),((_tmp2C5[0]=((_tmp2C6.tag=17,((_tmp2C6.f1=
rgn_handle,((_tmp2C6.f2=e,_tmp2C6)))))),_tmp2C5)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_tmp2C7;return(
_tmp2C7=_cycalloc(sizeof(*_tmp2C7)),((_tmp2C7[0]=*e,_tmp2C7)));}struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_Const_e_struct _tmp2CA;struct Cyc_Absyn_Const_e_struct*_tmp2C9;return Cyc_Absyn_new_exp((
void*)((_tmp2C9=_cycalloc(sizeof(*_tmp2C9)),((_tmp2C9[0]=((_tmp2CA.tag=0,((
_tmp2CA.f1=c,_tmp2CA)))),_tmp2C9)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Const_e_struct _tmp2CD;struct Cyc_Absyn_Const_e_struct*
_tmp2CC;return Cyc_Absyn_new_exp((void*)((_tmp2CC=_cycalloc(sizeof(*_tmp2CC)),((
_tmp2CC[0]=((_tmp2CD.tag=0,((_tmp2CD.f1=Cyc_Absyn_Null_c,_tmp2CD)))),_tmp2CC)))),
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
_tuple0*q,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Var_e_struct _tmp2D0;
struct Cyc_Absyn_Var_e_struct*_tmp2CF;return Cyc_Absyn_new_exp((void*)((_tmp2CF=
_cycalloc(sizeof(*_tmp2CF)),((_tmp2CF[0]=((_tmp2D0.tag=1,((_tmp2D0.f1=q,((
_tmp2D0.f2=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),_tmp2D0)))))),_tmp2CF)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Var_e_struct _tmp2D3;struct Cyc_Absyn_Var_e_struct*_tmp2D2;
return Cyc_Absyn_new_exp((void*)((_tmp2D2=_cycalloc(sizeof(*_tmp2D2)),((_tmp2D2[0]=((
_tmp2D3.tag=1,((_tmp2D3.f1=q,((_tmp2D3.f2=(void*)b,_tmp2D3)))))),_tmp2D2)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_UnknownId_e_struct _tmp2D6;struct Cyc_Absyn_UnknownId_e_struct*
_tmp2D5;return Cyc_Absyn_new_exp((void*)((_tmp2D5=_cycalloc(sizeof(*_tmp2D5)),((
_tmp2D5[0]=((_tmp2D6.tag=2,((_tmp2D6.f1=q,_tmp2D6)))),_tmp2D5)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Primop_e_struct _tmp2D9;struct Cyc_Absyn_Primop_e_struct*
_tmp2D8;return Cyc_Absyn_new_exp((void*)((_tmp2D8=_cycalloc(sizeof(*_tmp2D8)),((
_tmp2D8[0]=((_tmp2D9.tag=3,((_tmp2D9.f1=p,((_tmp2D9.f2=es,_tmp2D9)))))),_tmp2D8)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp2DA;return Cyc_Absyn_primop_exp(
p,((_tmp2DA=_cycalloc(sizeof(*_tmp2DA)),((_tmp2DA->hd=e,((_tmp2DA->tl=0,_tmp2DA)))))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){struct Cyc_List_List*
_tmp2DD;struct Cyc_List_List*_tmp2DC;return Cyc_Absyn_primop_exp(p,((_tmp2DC=
_cycalloc(sizeof(*_tmp2DC)),((_tmp2DC->hd=e1,((_tmp2DC->tl=((_tmp2DD=_cycalloc(
sizeof(*_tmp2DD)),((_tmp2DD->hd=e2,((_tmp2DD->tl=0,_tmp2DD)))))),_tmp2DC)))))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Swap_e_struct _tmp2E0;struct
Cyc_Absyn_Swap_e_struct*_tmp2DF;return Cyc_Absyn_new_exp((void*)((_tmp2DF=
_cycalloc(sizeof(*_tmp2DF)),((_tmp2DF[0]=((_tmp2E0.tag=35,((_tmp2E0.f1=e1,((
_tmp2E0.f2=e2,_tmp2E0)))))),_tmp2DF)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
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
loc){struct Cyc_Absyn_AssignOp_e_struct _tmp2E3;struct Cyc_Absyn_AssignOp_e_struct*
_tmp2E2;return Cyc_Absyn_new_exp((void*)((_tmp2E2=_cycalloc(sizeof(*_tmp2E2)),((
_tmp2E2[0]=((_tmp2E3.tag=4,((_tmp2E3.f1=e1,((_tmp2E3.f2=popt,((_tmp2E3.f3=e2,
_tmp2E3)))))))),_tmp2E2)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Increment_e_struct _tmp2E6;struct Cyc_Absyn_Increment_e_struct*
_tmp2E5;return Cyc_Absyn_new_exp((void*)((_tmp2E5=_cycalloc(sizeof(*_tmp2E5)),((
_tmp2E5[0]=((_tmp2E6.tag=5,((_tmp2E6.f1=e,((_tmp2E6.f2=i,_tmp2E6)))))),_tmp2E5)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostInc,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreInc,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,Cyc_Absyn_PreDec,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostDec,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Conditional_e_struct _tmp2E9;struct Cyc_Absyn_Conditional_e_struct*
_tmp2E8;return Cyc_Absyn_new_exp((void*)((_tmp2E8=_cycalloc(sizeof(*_tmp2E8)),((
_tmp2E8[0]=((_tmp2E9.tag=6,((_tmp2E9.f1=e1,((_tmp2E9.f2=e2,((_tmp2E9.f3=e3,
_tmp2E9)))))))),_tmp2E8)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_And_e_struct _tmp2EC;struct Cyc_Absyn_And_e_struct*_tmp2EB;return Cyc_Absyn_new_exp((
void*)((_tmp2EB=_cycalloc(sizeof(*_tmp2EB)),((_tmp2EB[0]=((_tmp2EC.tag=7,((
_tmp2EC.f1=e1,((_tmp2EC.f2=e2,_tmp2EC)))))),_tmp2EB)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Or_e_struct _tmp2EF;struct Cyc_Absyn_Or_e_struct*_tmp2EE;
return Cyc_Absyn_new_exp((void*)((_tmp2EE=_cycalloc(sizeof(*_tmp2EE)),((_tmp2EE[0]=((
_tmp2EF.tag=8,((_tmp2EF.f1=e1,((_tmp2EF.f2=e2,_tmp2EF)))))),_tmp2EE)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_SeqExp_e_struct _tmp2F2;struct
Cyc_Absyn_SeqExp_e_struct*_tmp2F1;return Cyc_Absyn_new_exp((void*)((_tmp2F1=
_cycalloc(sizeof(*_tmp2F1)),((_tmp2F1[0]=((_tmp2F2.tag=9,((_tmp2F2.f1=e1,((
_tmp2F2.f2=e2,_tmp2F2)))))),_tmp2F1)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_UnknownCall_e_struct _tmp2F5;struct Cyc_Absyn_UnknownCall_e_struct*
_tmp2F4;return Cyc_Absyn_new_exp((void*)((_tmp2F4=_cycalloc(sizeof(*_tmp2F4)),((
_tmp2F4[0]=((_tmp2F5.tag=10,((_tmp2F5.f1=e,((_tmp2F5.f2=es,_tmp2F5)))))),_tmp2F4)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_FnCall_e_struct _tmp2F8;struct
Cyc_Absyn_FnCall_e_struct*_tmp2F7;return Cyc_Absyn_new_exp((void*)((_tmp2F7=
_cycalloc(sizeof(*_tmp2F7)),((_tmp2F7[0]=((_tmp2F8.tag=11,((_tmp2F8.f1=e,((
_tmp2F8.f2=es,((_tmp2F8.f3=0,_tmp2F8)))))))),_tmp2F7)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_NoInstantiate_e_struct _tmp2FB;struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp2FA;return Cyc_Absyn_new_exp((void*)((_tmp2FA=_cycalloc(sizeof(*_tmp2FA)),((
_tmp2FA[0]=((_tmp2FB.tag=13,((_tmp2FB.f1=e,_tmp2FB)))),_tmp2FA)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
ts,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Instantiate_e_struct _tmp2FE;
struct Cyc_Absyn_Instantiate_e_struct*_tmp2FD;return Cyc_Absyn_new_exp((void*)((
_tmp2FD=_cycalloc(sizeof(*_tmp2FD)),((_tmp2FD[0]=((_tmp2FE.tag=14,((_tmp2FE.f1=e,((
_tmp2FE.f2=ts,_tmp2FE)))))),_tmp2FD)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Cast_e_struct _tmp301;struct Cyc_Absyn_Cast_e_struct*_tmp300;
return Cyc_Absyn_new_exp((void*)((_tmp300=_cycalloc(sizeof(*_tmp300)),((_tmp300[0]=((
_tmp301.tag=15,((_tmp301.f1=(void*)t,((_tmp301.f2=e,((_tmp301.f3=user_cast,((
_tmp301.f4=c,_tmp301)))))))))),_tmp300)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Throw_e_struct
_tmp304;struct Cyc_Absyn_Throw_e_struct*_tmp303;return Cyc_Absyn_new_exp((void*)((
_tmp303=_cycalloc(sizeof(*_tmp303)),((_tmp303[0]=((_tmp304.tag=12,((_tmp304.f1=e,
_tmp304)))),_tmp303)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Address_e_struct
_tmp307;struct Cyc_Absyn_Address_e_struct*_tmp306;return Cyc_Absyn_new_exp((void*)((
_tmp306=_cycalloc(sizeof(*_tmp306)),((_tmp306[0]=((_tmp307.tag=16,((_tmp307.f1=e,
_tmp307)))),_tmp306)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Sizeoftyp_e_struct _tmp30A;struct
Cyc_Absyn_Sizeoftyp_e_struct*_tmp309;return Cyc_Absyn_new_exp((void*)((_tmp309=
_cycalloc(sizeof(*_tmp309)),((_tmp309[0]=((_tmp30A.tag=18,((_tmp30A.f1=(void*)t,
_tmp30A)))),_tmp309)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Sizeofexp_e_struct
_tmp30D;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp30C;return Cyc_Absyn_new_exp((void*)((
_tmp30C=_cycalloc(sizeof(*_tmp30C)),((_tmp30C[0]=((_tmp30D.tag=19,((_tmp30D.f1=e,
_tmp30D)))),_tmp30C)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,
void*of,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Offsetof_e_struct _tmp310;
struct Cyc_Absyn_Offsetof_e_struct*_tmp30F;return Cyc_Absyn_new_exp((void*)((
_tmp30F=_cycalloc(sizeof(*_tmp30F)),((_tmp30F[0]=((_tmp310.tag=20,((_tmp310.f1=(
void*)t,((_tmp310.f2=(void*)of,_tmp310)))))),_tmp30F)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_Deref_e_struct _tmp313;struct Cyc_Absyn_Deref_e_struct*_tmp312;return Cyc_Absyn_new_exp((
void*)((_tmp312=_cycalloc(sizeof(*_tmp312)),((_tmp312[0]=((_tmp313.tag=21,((
_tmp313.f1=e,_tmp313)))),_tmp312)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_AggrMember_e_struct _tmp316;struct Cyc_Absyn_AggrMember_e_struct*
_tmp315;return Cyc_Absyn_new_exp((void*)((_tmp315=_cycalloc(sizeof(*_tmp315)),((
_tmp315[0]=((_tmp316.tag=22,((_tmp316.f1=e,((_tmp316.f2=n,((_tmp316.f3=0,((
_tmp316.f4=0,_tmp316)))))))))),_tmp315)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_AggrArrow_e_struct _tmp319;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp318;return Cyc_Absyn_new_exp((void*)((_tmp318=_cycalloc(sizeof(*_tmp318)),((
_tmp318[0]=((_tmp319.tag=23,((_tmp319.f1=e,((_tmp319.f2=n,((_tmp319.f3=0,((
_tmp319.f4=0,_tmp319)))))))))),_tmp318)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Subscript_e_struct _tmp31C;struct Cyc_Absyn_Subscript_e_struct*
_tmp31B;return Cyc_Absyn_new_exp((void*)((_tmp31B=_cycalloc(sizeof(*_tmp31B)),((
_tmp31B[0]=((_tmp31C.tag=24,((_tmp31C.f1=e1,((_tmp31C.f2=e2,_tmp31C)))))),
_tmp31B)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Tuple_e_struct _tmp31F;struct Cyc_Absyn_Tuple_e_struct*
_tmp31E;return Cyc_Absyn_new_exp((void*)((_tmp31E=_cycalloc(sizeof(*_tmp31E)),((
_tmp31E[0]=((_tmp31F.tag=25,((_tmp31F.f1=es,_tmp31F)))),_tmp31E)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_StmtExp_e_struct _tmp322;struct Cyc_Absyn_StmtExp_e_struct*
_tmp321;return Cyc_Absyn_new_exp((void*)((_tmp321=_cycalloc(sizeof(*_tmp321)),((
_tmp321[0]=((_tmp322.tag=37,((_tmp322.f1=s,_tmp322)))),_tmp321)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_var_exp(
Cyc_Absyn_Match_Exception_name,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(
void*t,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Valueof_e_struct _tmp325;
struct Cyc_Absyn_Valueof_e_struct*_tmp324;return Cyc_Absyn_new_exp((void*)((
_tmp324=_cycalloc(sizeof(*_tmp324)),((_tmp324[0]=((_tmp325.tag=39,((_tmp325.f1=(
void*)t,_tmp325)))),_tmp324)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int
volatile_kw,struct _dyneither_ptr body,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Asm_e_struct
_tmp328;struct Cyc_Absyn_Asm_e_struct*_tmp327;return Cyc_Absyn_new_exp((void*)((
_tmp327=_cycalloc(sizeof(*_tmp327)),((_tmp327[0]=((_tmp328.tag=40,((_tmp328.f1=
volatile_kw,((_tmp328.f2=body,_tmp328)))))),_tmp327)))),loc);}struct _tuple12{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){struct _tuple12*_tmp32B;struct Cyc_List_List*_tmp32A;
dles=((_tmp32A=_cycalloc(sizeof(*_tmp32A)),((_tmp32A->hd=((_tmp32B=_cycalloc(
sizeof(*_tmp32B)),((_tmp32B->f1=0,((_tmp32B->f2=(struct Cyc_Absyn_Exp*)es->hd,
_tmp32B)))))),((_tmp32A->tl=dles,_tmp32A))))));}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);{struct Cyc_Absyn_Array_e_struct
_tmp32E;struct Cyc_Absyn_Array_e_struct*_tmp32D;return Cyc_Absyn_new_exp((void*)((
_tmp32D=_cycalloc(sizeof(*_tmp32D)),((_tmp32D[0]=((_tmp32E.tag=27,((_tmp32E.f1=
dles,_tmp32E)))),_tmp32D)))),loc);};}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp331;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp330;return Cyc_Absyn_new_exp((void*)((_tmp330=_cycalloc(sizeof(*_tmp330)),((
_tmp330[0]=((_tmp331.tag=36,((_tmp331.f1=n,((_tmp331.f2=dles,_tmp331)))))),
_tmp330)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Stmt*_tmp332;return(_tmp332=_cycalloc(sizeof(*_tmp332)),((
_tmp332->r=s,((_tmp332->loc=loc,((_tmp332->non_local_preds=0,((_tmp332->try_depth=
0,((_tmp332->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,_tmp332)))))))))));}struct
Cyc_Absyn_Skip_s_struct Cyc_Absyn_Skip_s_val={0};struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Exp_s_struct _tmp335;struct Cyc_Absyn_Exp_s_struct*_tmp334;
return Cyc_Absyn_new_stmt((void*)((_tmp334=_cycalloc(sizeof(*_tmp334)),((_tmp334[
0]=((_tmp335.tag=1,((_tmp335.f1=e,_tmp335)))),_tmp334)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss
== 0)return Cyc_Absyn_skip_stmt(loc);else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)
ss->hd;else{return Cyc_Absyn_seq_stmt((struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(
ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Return_s_struct _tmp338;struct
Cyc_Absyn_Return_s_struct*_tmp337;return Cyc_Absyn_new_stmt((void*)((_tmp337=
_cycalloc(sizeof(*_tmp337)),((_tmp337[0]=((_tmp338.tag=3,((_tmp338.f1=e,_tmp338)))),
_tmp337)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_IfThenElse_s_struct _tmp33B;struct Cyc_Absyn_IfThenElse_s_struct*
_tmp33A;return Cyc_Absyn_new_stmt((void*)((_tmp33A=_cycalloc(sizeof(*_tmp33A)),((
_tmp33A[0]=((_tmp33B.tag=4,((_tmp33B.f1=e,((_tmp33B.f2=s1,((_tmp33B.f3=s2,
_tmp33B)))))))),_tmp33A)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_While_s_struct _tmp341;struct _tuple8 _tmp340;struct Cyc_Absyn_While_s_struct*
_tmp33F;return Cyc_Absyn_new_stmt((void*)((_tmp33F=_cycalloc(sizeof(*_tmp33F)),((
_tmp33F[0]=((_tmp341.tag=5,((_tmp341.f1=((_tmp340.f1=e,((_tmp340.f2=Cyc_Absyn_skip_stmt(
e->loc),_tmp340)))),((_tmp341.f2=s,_tmp341)))))),_tmp33F)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_break_stmt(struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Break_s_struct
_tmp344;struct Cyc_Absyn_Break_s_struct*_tmp343;return Cyc_Absyn_new_stmt((void*)((
_tmp343=_cycalloc(sizeof(*_tmp343)),((_tmp343[0]=((_tmp344.tag=6,((_tmp344.f1=0,
_tmp344)))),_tmp343)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Continue_s_struct _tmp347;struct
Cyc_Absyn_Continue_s_struct*_tmp346;return Cyc_Absyn_new_stmt((void*)((_tmp346=
_cycalloc(sizeof(*_tmp346)),((_tmp346[0]=((_tmp347.tag=7,((_tmp347.f1=0,_tmp347)))),
_tmp346)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_For_s_struct _tmp350;struct _tuple8 _tmp34F;struct _tuple8
_tmp34E;struct Cyc_Absyn_For_s_struct*_tmp34D;return Cyc_Absyn_new_stmt((void*)((
_tmp34D=_cycalloc(sizeof(*_tmp34D)),((_tmp34D[0]=((_tmp350.tag=9,((_tmp350.f1=e1,((
_tmp350.f2=((_tmp34F.f1=e2,((_tmp34F.f2=Cyc_Absyn_skip_stmt(e3->loc),_tmp34F)))),((
_tmp350.f3=((_tmp34E.f1=e3,((_tmp34E.f2=Cyc_Absyn_skip_stmt(e3->loc),_tmp34E)))),((
_tmp350.f4=s,_tmp350)))))))))),_tmp34D)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Switch_s_struct _tmp353;struct Cyc_Absyn_Switch_s_struct*_tmp352;
return Cyc_Absyn_new_stmt((void*)((_tmp352=_cycalloc(sizeof(*_tmp352)),((_tmp352[
0]=((_tmp353.tag=10,((_tmp353.f1=e,((_tmp353.f2=scs,_tmp353)))))),_tmp352)))),
loc);}struct _tuple13{void*f1;void*f2;};struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc){
struct _tuple13 _tmp354;struct _tuple13 _tmp12E=(_tmp354.f1=s1->r,((_tmp354.f2=s2->r,
_tmp354)));void*_tmp12F;void*_tmp131;_LL67: _tmp12F=_tmp12E.f1;{struct Cyc_Absyn_Skip_s_struct*
_tmp130=(struct Cyc_Absyn_Skip_s_struct*)_tmp12F;if(_tmp130->tag != 0)goto _LL69;};
_LL68: return s2;_LL69: _tmp131=_tmp12E.f2;{struct Cyc_Absyn_Skip_s_struct*_tmp132=(
struct Cyc_Absyn_Skip_s_struct*)_tmp131;if(_tmp132->tag != 0)goto _LL6B;};_LL6A:
return s1;_LL6B:;_LL6C: {struct Cyc_Absyn_Seq_s_struct _tmp357;struct Cyc_Absyn_Seq_s_struct*
_tmp356;return Cyc_Absyn_new_stmt((void*)((_tmp356=_cycalloc(sizeof(*_tmp356)),((
_tmp356[0]=((_tmp357.tag=2,((_tmp357.f1=s1,((_tmp357.f2=s2,_tmp357)))))),_tmp356)))),
loc);}_LL66:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*
el,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Fallthru_s_struct _tmp35A;
struct Cyc_Absyn_Fallthru_s_struct*_tmp359;return Cyc_Absyn_new_stmt((void*)((
_tmp359=_cycalloc(sizeof(*_tmp359)),((_tmp359[0]=((_tmp35A.tag=11,((_tmp35A.f1=
el,((_tmp35A.f2=0,_tmp35A)))))),_tmp359)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Decl_s_struct _tmp35D;struct Cyc_Absyn_Decl_s_struct*_tmp35C;
return Cyc_Absyn_new_stmt((void*)((_tmp35C=_cycalloc(sizeof(*_tmp35C)),((_tmp35C[
0]=((_tmp35D.tag=12,((_tmp35D.f1=d,((_tmp35D.f2=s,_tmp35D)))))),_tmp35C)))),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Var_d_struct
_tmp360;struct Cyc_Absyn_Var_d_struct*_tmp35F;struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((
void*)((_tmp35F=_cycalloc(sizeof(*_tmp35F)),((_tmp35F[0]=((_tmp360.tag=0,((
_tmp360.f1=Cyc_Absyn_new_vardecl(x,t,init),_tmp360)))),_tmp35F)))),loc);struct
Cyc_Absyn_Decl_s_struct _tmp363;struct Cyc_Absyn_Decl_s_struct*_tmp362;return Cyc_Absyn_new_stmt((
void*)((_tmp362=_cycalloc(sizeof(*_tmp362)),((_tmp362[0]=((_tmp363.tag=12,((
_tmp363.f1=d,((_tmp363.f2=s,_tmp363)))))),_tmp362)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Label_s_struct _tmp366;struct Cyc_Absyn_Label_s_struct*
_tmp365;return Cyc_Absyn_new_stmt((void*)((_tmp365=_cycalloc(sizeof(*_tmp365)),((
_tmp365[0]=((_tmp366.tag=13,((_tmp366.f1=v,((_tmp366.f2=s,_tmp366)))))),_tmp365)))),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Do_s_struct _tmp36C;struct
_tuple8 _tmp36B;struct Cyc_Absyn_Do_s_struct*_tmp36A;return Cyc_Absyn_new_stmt((
void*)((_tmp36A=_cycalloc(sizeof(*_tmp36A)),((_tmp36A[0]=((_tmp36C.tag=14,((
_tmp36C.f1=s,((_tmp36C.f2=((_tmp36B.f1=e,((_tmp36B.f2=Cyc_Absyn_skip_stmt(e->loc),
_tmp36B)))),_tmp36C)))))),_tmp36A)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_TryCatch_s_struct _tmp36F;struct Cyc_Absyn_TryCatch_s_struct*
_tmp36E;return Cyc_Absyn_new_stmt((void*)((_tmp36E=_cycalloc(sizeof(*_tmp36E)),((
_tmp36E[0]=((_tmp36F.tag=15,((_tmp36F.f1=s,((_tmp36F.f2=scs,_tmp36F)))))),
_tmp36E)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*
lab,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Goto_s_struct _tmp372;struct
Cyc_Absyn_Goto_s_struct*_tmp371;return Cyc_Absyn_new_stmt((void*)((_tmp371=
_cycalloc(sizeof(*_tmp371)),((_tmp371[0]=((_tmp372.tag=8,((_tmp372.f1=lab,((
_tmp372.f2=0,_tmp372)))))),_tmp371)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s){struct Cyc_Absyn_Pat*
_tmp373;return(_tmp373=_cycalloc(sizeof(*_tmp373)),((_tmp373->r=p,((_tmp373->topt=
0,((_tmp373->loc=s,_tmp373)))))));}struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct
Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_p_struct _tmp376;struct Cyc_Absyn_Exp_p_struct*
_tmp375;return Cyc_Absyn_new_pat((void*)((_tmp375=_cycalloc(sizeof(*_tmp375)),((
_tmp375[0]=((_tmp376.tag=16,((_tmp376.f1=e,_tmp376)))),_tmp375)))),e->loc);}
struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val={0};struct Cyc_Absyn_Null_p_struct
Cyc_Absyn_Null_p_val={8};struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct
Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*_tmp377;return(_tmp377=_cycalloc(
sizeof(*_tmp377)),((_tmp377->r=r,((_tmp377->loc=loc,_tmp377)))));}struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Let_d_struct _tmp37A;struct Cyc_Absyn_Let_d_struct*_tmp379;
return Cyc_Absyn_new_decl((void*)((_tmp379=_cycalloc(sizeof(*_tmp379)),((_tmp379[
0]=((_tmp37A.tag=2,((_tmp37A.f1=p,((_tmp37A.f2=0,((_tmp37A.f3=e,_tmp37A)))))))),
_tmp379)))),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*
vds,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Letv_d_struct _tmp37D;struct
Cyc_Absyn_Letv_d_struct*_tmp37C;return Cyc_Absyn_new_decl((void*)((_tmp37C=
_cycalloc(sizeof(*_tmp37C)),((_tmp37C[0]=((_tmp37D.tag=3,((_tmp37D.f1=vds,
_tmp37D)))),_tmp37C)))),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct
Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int resetable,struct Cyc_Absyn_Exp*eo,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Region_d_struct _tmp380;struct Cyc_Absyn_Region_d_struct*
_tmp37F;return Cyc_Absyn_new_decl((void*)((_tmp37F=_cycalloc(sizeof(*_tmp37F)),((
_tmp37F[0]=((_tmp380.tag=4,((_tmp380.f1=tv,((_tmp380.f2=vd,((_tmp380.f3=
resetable,((_tmp380.f4=eo,_tmp380)))))))))),_tmp37F)))),loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Alias_d_struct _tmp383;struct
Cyc_Absyn_Alias_d_struct*_tmp382;return Cyc_Absyn_new_decl((void*)((_tmp382=
_cycalloc(sizeof(*_tmp382)),((_tmp382[0]=((_tmp383.tag=5,((_tmp383.f1=e,((
_tmp383.f2=tv,((_tmp383.f3=vd,_tmp383)))))))),_tmp382)))),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){struct Cyc_Absyn_Vardecl*
_tmp384;return(_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384->sc=Cyc_Absyn_Public,((
_tmp384->name=x,((_tmp384->tq=Cyc_Absyn_empty_tqual(0),((_tmp384->type=t,((
_tmp384->initializer=init,((_tmp384->rgn=0,((_tmp384->attributes=0,((_tmp384->escapes=
0,_tmp384)))))))))))))))));}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){struct Cyc_Absyn_Vardecl*_tmp385;
return(_tmp385=_cycalloc(sizeof(*_tmp385)),((_tmp385->sc=Cyc_Absyn_Static,((
_tmp385->name=x,((_tmp385->tq=Cyc_Absyn_empty_tqual(0),((_tmp385->type=t,((
_tmp385->initializer=init,((_tmp385->rgn=0,((_tmp385->attributes=0,((_tmp385->escapes=
0,_tmp385)))))))))))))))));}struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int
tagged){struct Cyc_Absyn_AggrdeclImpl*_tmp386;return(_tmp386=_cycalloc(sizeof(*
_tmp386)),((_tmp386->exist_vars=exists,((_tmp386->rgn_po=po,((_tmp386->fields=fs,((
_tmp386->tagged=tagged,_tmp386)))))))));}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(
enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Aggr_d_struct _tmp38C;struct Cyc_Absyn_Aggrdecl*_tmp38B;
struct Cyc_Absyn_Aggr_d_struct*_tmp38A;return Cyc_Absyn_new_decl((void*)((_tmp38A=
_cycalloc(sizeof(*_tmp38A)),((_tmp38A[0]=((_tmp38C.tag=6,((_tmp38C.f1=((_tmp38B=
_cycalloc(sizeof(*_tmp38B)),((_tmp38B->kind=k,((_tmp38B->sc=s,((_tmp38B->name=n,((
_tmp38B->tvs=ts,((_tmp38B->impl=i,((_tmp38B->attributes=atts,_tmp38B)))))))))))))),
_tmp38C)))),_tmp38A)))),loc);}struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(
enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Aggr_td_struct*_tmp396;struct Cyc_Absyn_Aggrdecl*_tmp395;
struct Cyc_Absyn_Aggr_td_struct _tmp394;struct Cyc_Absyn_TypeDecl*_tmp393;return(
_tmp393=_cycalloc(sizeof(*_tmp393)),((_tmp393->r=(void*)((_tmp396=_cycalloc(
sizeof(*_tmp396)),((_tmp396[0]=((_tmp394.tag=0,((_tmp394.f1=((_tmp395=_cycalloc(
sizeof(*_tmp395)),((_tmp395->kind=k,((_tmp395->sc=s,((_tmp395->name=n,((_tmp395->tvs=
ts,((_tmp395->impl=i,((_tmp395->attributes=atts,_tmp395)))))))))))))),_tmp394)))),
_tmp396)))),((_tmp393->loc=loc,_tmp393)))));}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl(
Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(
enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl(
Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(
enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Datatype_d_struct
_tmp39C;struct Cyc_Absyn_Datatypedecl*_tmp39B;struct Cyc_Absyn_Datatype_d_struct*
_tmp39A;return Cyc_Absyn_new_decl((void*)((_tmp39A=_cycalloc(sizeof(*_tmp39A)),((
_tmp39A[0]=((_tmp39C.tag=7,((_tmp39C.f1=((_tmp39B=_cycalloc(sizeof(*_tmp39B)),((
_tmp39B->sc=s,((_tmp39B->name=n,((_tmp39B->tvs=ts,((_tmp39B->fields=fs,((_tmp39B->is_extensible=
is_extensible,_tmp39B)))))))))))),_tmp39C)))),_tmp39A)))),loc);}struct Cyc_Absyn_TypeDecl*
Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_extensible,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_Datatype_td_struct*_tmp3A6;struct Cyc_Absyn_Datatypedecl*_tmp3A5;struct
Cyc_Absyn_Datatype_td_struct _tmp3A4;struct Cyc_Absyn_TypeDecl*_tmp3A3;return(
_tmp3A3=_cycalloc(sizeof(*_tmp3A3)),((_tmp3A3->r=(void*)((_tmp3A6=_cycalloc(
sizeof(*_tmp3A6)),((_tmp3A6[0]=((_tmp3A4.tag=2,((_tmp3A4.f1=((_tmp3A5=_cycalloc(
sizeof(*_tmp3A5)),((_tmp3A5->sc=s,((_tmp3A5->name=n,((_tmp3A5->tvs=ts,((_tmp3A5->fields=
fs,((_tmp3A5->is_extensible=is_extensible,_tmp3A5)))))))))))),_tmp3A4)))),
_tmp3A6)))),((_tmp3A3->loc=loc,_tmp3A3)))));}static struct _tuple7*Cyc_Absyn_expand_arg(
struct _tuple7*a){struct _tuple7*_tmp3A7;return(_tmp3A7=_cycalloc(sizeof(*_tmp3A7)),((
_tmp3A7->f1=(*a).f1,((_tmp3A7->f2=(*a).f2,((_tmp3A7->f3=Cyc_Absyn_pointer_expand((*
a).f3,1),_tmp3A7)))))));}void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,
struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*args,int c_varargs,
struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*
atts){struct Cyc_Absyn_FnType_struct _tmp3AD;struct Cyc_Absyn_FnInfo _tmp3AC;struct
Cyc_Absyn_FnType_struct*_tmp3AB;return(void*)((_tmp3AB=_cycalloc(sizeof(*_tmp3AB)),((
_tmp3AB[0]=((_tmp3AD.tag=10,((_tmp3AD.f1=((_tmp3AC.tvars=tvs,((_tmp3AC.ret_typ=
Cyc_Absyn_pointer_expand(ret_typ,0),((_tmp3AC.effect=eff_typ,((_tmp3AC.args=((
struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absyn_expand_arg,args),((_tmp3AC.c_varargs=c_varargs,((_tmp3AC.cyc_varargs=
cyc_varargs,((_tmp3AC.rgn_po=rgn_po,((_tmp3AC.attributes=atts,_tmp3AC)))))))))))))))),
_tmp3AD)))),_tmp3AB))));}void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmp169=Cyc_Tcutil_compress(t);_LL6E: {struct Cyc_Absyn_FnType_struct*
_tmp16A=(struct Cyc_Absyn_FnType_struct*)_tmp169;if(_tmp16A->tag != 10)goto _LL70;}
_LL6F: {struct Cyc_Core_Opt*_tmp3AE;return Cyc_Absyn_at_typ(t,fresh_evar?Cyc_Absyn_new_evar(((
_tmp3AE=_cycalloc(sizeof(*_tmp3AE)),((_tmp3AE->v=& Cyc_Tcutil_rk,_tmp3AE)))),0):(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);}
_LL70:;_LL71: return t;_LL6D:;}int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){void*
_tmp16C=e->r;void*_tmp16E;void*_tmp171;struct Cyc_Absyn_Vardecl*_tmp173;void*
_tmp175;struct Cyc_Absyn_Vardecl*_tmp177;struct Cyc_Absyn_Exp*_tmp17D;struct Cyc_Absyn_Exp*
_tmp17F;struct Cyc_Absyn_Exp*_tmp181;_LL73: {struct Cyc_Absyn_Var_e_struct*_tmp16D=(
struct Cyc_Absyn_Var_e_struct*)_tmp16C;if(_tmp16D->tag != 1)goto _LL75;else{_tmp16E=(
void*)_tmp16D->f2;{struct Cyc_Absyn_Funname_b_struct*_tmp16F=(struct Cyc_Absyn_Funname_b_struct*)
_tmp16E;if(_tmp16F->tag != 2)goto _LL75;};}}_LL74: return 0;_LL75: {struct Cyc_Absyn_Var_e_struct*
_tmp170=(struct Cyc_Absyn_Var_e_struct*)_tmp16C;if(_tmp170->tag != 1)goto _LL77;
else{_tmp171=(void*)_tmp170->f2;{struct Cyc_Absyn_Global_b_struct*_tmp172=(struct
Cyc_Absyn_Global_b_struct*)_tmp171;if(_tmp172->tag != 1)goto _LL77;else{_tmp173=
_tmp172->f1;}};}}_LL76: _tmp177=_tmp173;goto _LL78;_LL77: {struct Cyc_Absyn_Var_e_struct*
_tmp174=(struct Cyc_Absyn_Var_e_struct*)_tmp16C;if(_tmp174->tag != 1)goto _LL79;
else{_tmp175=(void*)_tmp174->f2;{struct Cyc_Absyn_Local_b_struct*_tmp176=(struct
Cyc_Absyn_Local_b_struct*)_tmp175;if(_tmp176->tag != 4)goto _LL79;else{_tmp177=
_tmp176->f1;}};}}_LL78: {void*_tmp182=Cyc_Tcutil_compress(_tmp177->type);_LL8A: {
struct Cyc_Absyn_ArrayType_struct*_tmp183=(struct Cyc_Absyn_ArrayType_struct*)
_tmp182;if(_tmp183->tag != 9)goto _LL8C;}_LL8B: return 0;_LL8C:;_LL8D: return 1;_LL89:;}
_LL79: {struct Cyc_Absyn_Var_e_struct*_tmp178=(struct Cyc_Absyn_Var_e_struct*)
_tmp16C;if(_tmp178->tag != 1)goto _LL7B;}_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp179=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp16C;if(_tmp179->tag != 23)goto
_LL7D;}_LL7C: goto _LL7E;_LL7D: {struct Cyc_Absyn_Deref_e_struct*_tmp17A=(struct Cyc_Absyn_Deref_e_struct*)
_tmp16C;if(_tmp17A->tag != 21)goto _LL7F;}_LL7E: goto _LL80;_LL7F: {struct Cyc_Absyn_Subscript_e_struct*
_tmp17B=(struct Cyc_Absyn_Subscript_e_struct*)_tmp16C;if(_tmp17B->tag != 24)goto
_LL81;}_LL80: return 1;_LL81: {struct Cyc_Absyn_AggrMember_e_struct*_tmp17C=(struct
Cyc_Absyn_AggrMember_e_struct*)_tmp16C;if(_tmp17C->tag != 22)goto _LL83;else{
_tmp17D=_tmp17C->f1;}}_LL82: return Cyc_Absyn_is_lvalue(_tmp17D);_LL83: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp17E=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp16C;if(_tmp17E->tag != 14)goto
_LL85;else{_tmp17F=_tmp17E->f1;}}_LL84: return Cyc_Absyn_is_lvalue(_tmp17F);_LL85: {
struct Cyc_Absyn_NoInstantiate_e_struct*_tmp180=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp16C;if(_tmp180->tag != 13)goto _LL87;else{_tmp181=_tmp180->f1;}}_LL86: return
Cyc_Absyn_is_lvalue(_tmp181);_LL87:;_LL88: return 0;_LL72:;}struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v){{
struct Cyc_List_List*_tmp184=fields;for(0;_tmp184 != 0;_tmp184=_tmp184->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Aggrfield*)_tmp184->hd)->name,v)== 0)return(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_Absyn_Aggrfield*)_tmp184->hd);}}return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
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
_dyneither_ptr Cyc_Absyn_attribute2string(void*a){void*_tmp196=a;int _tmp198;int
_tmp19F;struct _dyneither_ptr _tmp1A2;enum Cyc_Absyn_Format_Type _tmp1AE;int _tmp1AF;
int _tmp1B0;enum Cyc_Absyn_Format_Type _tmp1B2;int _tmp1B3;int _tmp1B4;int _tmp1B6;
int _tmp1B8;struct _dyneither_ptr _tmp1BB;_LL8F: {struct Cyc_Absyn_Regparm_att_struct*
_tmp197=(struct Cyc_Absyn_Regparm_att_struct*)_tmp196;if(_tmp197->tag != 0)goto
_LL91;else{_tmp198=_tmp197->f1;}}_LL90: {const char*_tmp3B2;void*_tmp3B1[1];
struct Cyc_Int_pa_struct _tmp3B0;return(struct _dyneither_ptr)((_tmp3B0.tag=1,((
_tmp3B0.f1=(unsigned long)_tmp198,((_tmp3B1[0]=& _tmp3B0,Cyc_aprintf(((_tmp3B2="regparm(%d)",
_tag_dyneither(_tmp3B2,sizeof(char),12))),_tag_dyneither(_tmp3B1,sizeof(void*),1))))))));}
_LL91: {struct Cyc_Absyn_Stdcall_att_struct*_tmp199=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp196;if(_tmp199->tag != 1)goto _LL93;}_LL92: {const char*_tmp3B3;return(_tmp3B3="stdcall",
_tag_dyneither(_tmp3B3,sizeof(char),8));}_LL93: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp19A=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp196;if(_tmp19A->tag != 2)goto _LL95;}
_LL94: {const char*_tmp3B4;return(_tmp3B4="cdecl",_tag_dyneither(_tmp3B4,sizeof(
char),6));}_LL95: {struct Cyc_Absyn_Fastcall_att_struct*_tmp19B=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp196;if(_tmp19B->tag != 3)goto _LL97;}_LL96: {const char*_tmp3B5;return(_tmp3B5="fastcall",
_tag_dyneither(_tmp3B5,sizeof(char),9));}_LL97: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp19C=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp196;if(_tmp19C->tag != 4)goto
_LL99;}_LL98: {const char*_tmp3B6;return(_tmp3B6="noreturn",_tag_dyneither(
_tmp3B6,sizeof(char),9));}_LL99: {struct Cyc_Absyn_Const_att_struct*_tmp19D=(
struct Cyc_Absyn_Const_att_struct*)_tmp196;if(_tmp19D->tag != 5)goto _LL9B;}_LL9A: {
const char*_tmp3B7;return(_tmp3B7="const",_tag_dyneither(_tmp3B7,sizeof(char),6));}
_LL9B: {struct Cyc_Absyn_Aligned_att_struct*_tmp19E=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp196;if(_tmp19E->tag != 6)goto _LL9D;else{_tmp19F=_tmp19E->f1;}}_LL9C: if(
_tmp19F == - 1){const char*_tmp3B8;return(_tmp3B8="aligned",_tag_dyneither(_tmp3B8,
sizeof(char),8));}else{switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LLC1: {const char*_tmp3BC;void*_tmp3BB[1];struct Cyc_Int_pa_struct _tmp3BA;return(
struct _dyneither_ptr)((_tmp3BA.tag=1,((_tmp3BA.f1=(unsigned long)_tmp19F,((
_tmp3BB[0]=& _tmp3BA,Cyc_aprintf(((_tmp3BC="aligned(%d)",_tag_dyneither(_tmp3BC,
sizeof(char),12))),_tag_dyneither(_tmp3BB,sizeof(void*),1))))))));}case Cyc_Cyclone_Vc_c:
_LLC2: {const char*_tmp3C0;void*_tmp3BF[1];struct Cyc_Int_pa_struct _tmp3BE;return(
struct _dyneither_ptr)((_tmp3BE.tag=1,((_tmp3BE.f1=(unsigned long)_tmp19F,((
_tmp3BF[0]=& _tmp3BE,Cyc_aprintf(((_tmp3C0="align(%d)",_tag_dyneither(_tmp3C0,
sizeof(char),10))),_tag_dyneither(_tmp3BF,sizeof(void*),1))))))));}}}_LL9D: {
struct Cyc_Absyn_Packed_att_struct*_tmp1A0=(struct Cyc_Absyn_Packed_att_struct*)
_tmp196;if(_tmp1A0->tag != 7)goto _LL9F;}_LL9E: {const char*_tmp3C1;return(_tmp3C1="packed",
_tag_dyneither(_tmp3C1,sizeof(char),7));}_LL9F: {struct Cyc_Absyn_Section_att_struct*
_tmp1A1=(struct Cyc_Absyn_Section_att_struct*)_tmp196;if(_tmp1A1->tag != 8)goto
_LLA1;else{_tmp1A2=_tmp1A1->f1;}}_LLA0: {const char*_tmp3C5;void*_tmp3C4[1];
struct Cyc_String_pa_struct _tmp3C3;return(struct _dyneither_ptr)((_tmp3C3.tag=0,((
_tmp3C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1A2),((_tmp3C4[0]=&
_tmp3C3,Cyc_aprintf(((_tmp3C5="section(\"%s\")",_tag_dyneither(_tmp3C5,sizeof(
char),14))),_tag_dyneither(_tmp3C4,sizeof(void*),1))))))));}_LLA1: {struct Cyc_Absyn_Nocommon_att_struct*
_tmp1A3=(struct Cyc_Absyn_Nocommon_att_struct*)_tmp196;if(_tmp1A3->tag != 9)goto
_LLA3;}_LLA2: {const char*_tmp3C6;return(_tmp3C6="nocommon",_tag_dyneither(
_tmp3C6,sizeof(char),9));}_LLA3: {struct Cyc_Absyn_Shared_att_struct*_tmp1A4=(
struct Cyc_Absyn_Shared_att_struct*)_tmp196;if(_tmp1A4->tag != 10)goto _LLA5;}_LLA4: {
const char*_tmp3C7;return(_tmp3C7="shared",_tag_dyneither(_tmp3C7,sizeof(char),7));}
_LLA5: {struct Cyc_Absyn_Unused_att_struct*_tmp1A5=(struct Cyc_Absyn_Unused_att_struct*)
_tmp196;if(_tmp1A5->tag != 11)goto _LLA7;}_LLA6: {const char*_tmp3C8;return(_tmp3C8="unused",
_tag_dyneither(_tmp3C8,sizeof(char),7));}_LLA7: {struct Cyc_Absyn_Weak_att_struct*
_tmp1A6=(struct Cyc_Absyn_Weak_att_struct*)_tmp196;if(_tmp1A6->tag != 12)goto _LLA9;}
_LLA8: {const char*_tmp3C9;return(_tmp3C9="weak",_tag_dyneither(_tmp3C9,sizeof(
char),5));}_LLA9: {struct Cyc_Absyn_Dllimport_att_struct*_tmp1A7=(struct Cyc_Absyn_Dllimport_att_struct*)
_tmp196;if(_tmp1A7->tag != 13)goto _LLAB;}_LLAA: {const char*_tmp3CA;return(_tmp3CA="dllimport",
_tag_dyneither(_tmp3CA,sizeof(char),10));}_LLAB: {struct Cyc_Absyn_Dllexport_att_struct*
_tmp1A8=(struct Cyc_Absyn_Dllexport_att_struct*)_tmp196;if(_tmp1A8->tag != 14)goto
_LLAD;}_LLAC: {const char*_tmp3CB;return(_tmp3CB="dllexport",_tag_dyneither(
_tmp3CB,sizeof(char),10));}_LLAD: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp1A9=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp196;if(_tmp1A9->tag
!= 15)goto _LLAF;}_LLAE: {const char*_tmp3CC;return(_tmp3CC="no_instrument_function",
_tag_dyneither(_tmp3CC,sizeof(char),23));}_LLAF: {struct Cyc_Absyn_Constructor_att_struct*
_tmp1AA=(struct Cyc_Absyn_Constructor_att_struct*)_tmp196;if(_tmp1AA->tag != 16)
goto _LLB1;}_LLB0: {const char*_tmp3CD;return(_tmp3CD="constructor",_tag_dyneither(
_tmp3CD,sizeof(char),12));}_LLB1: {struct Cyc_Absyn_Destructor_att_struct*_tmp1AB=(
struct Cyc_Absyn_Destructor_att_struct*)_tmp196;if(_tmp1AB->tag != 17)goto _LLB3;}
_LLB2: {const char*_tmp3CE;return(_tmp3CE="destructor",_tag_dyneither(_tmp3CE,
sizeof(char),11));}_LLB3: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp1AC=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp196;if(_tmp1AC->tag
!= 18)goto _LLB5;}_LLB4: {const char*_tmp3CF;return(_tmp3CF="no_check_memory_usage",
_tag_dyneither(_tmp3CF,sizeof(char),22));}_LLB5: {struct Cyc_Absyn_Format_att_struct*
_tmp1AD=(struct Cyc_Absyn_Format_att_struct*)_tmp196;if(_tmp1AD->tag != 19)goto
_LLB7;else{_tmp1AE=_tmp1AD->f1;if(_tmp1AE != Cyc_Absyn_Printf_ft)goto _LLB7;
_tmp1AF=_tmp1AD->f2;_tmp1B0=_tmp1AD->f3;}}_LLB6: {const char*_tmp3D4;void*_tmp3D3[
2];struct Cyc_Int_pa_struct _tmp3D2;struct Cyc_Int_pa_struct _tmp3D1;return(struct
_dyneither_ptr)((_tmp3D1.tag=1,((_tmp3D1.f1=(unsigned int)_tmp1B0,((_tmp3D2.tag=
1,((_tmp3D2.f1=(unsigned int)_tmp1AF,((_tmp3D3[0]=& _tmp3D2,((_tmp3D3[1]=& _tmp3D1,
Cyc_aprintf(((_tmp3D4="format(printf,%u,%u)",_tag_dyneither(_tmp3D4,sizeof(char),
21))),_tag_dyneither(_tmp3D3,sizeof(void*),2))))))))))))));}_LLB7: {struct Cyc_Absyn_Format_att_struct*
_tmp1B1=(struct Cyc_Absyn_Format_att_struct*)_tmp196;if(_tmp1B1->tag != 19)goto
_LLB9;else{_tmp1B2=_tmp1B1->f1;if(_tmp1B2 != Cyc_Absyn_Scanf_ft)goto _LLB9;_tmp1B3=
_tmp1B1->f2;_tmp1B4=_tmp1B1->f3;}}_LLB8: {const char*_tmp3D9;void*_tmp3D8[2];
struct Cyc_Int_pa_struct _tmp3D7;struct Cyc_Int_pa_struct _tmp3D6;return(struct
_dyneither_ptr)((_tmp3D6.tag=1,((_tmp3D6.f1=(unsigned int)_tmp1B4,((_tmp3D7.tag=
1,((_tmp3D7.f1=(unsigned int)_tmp1B3,((_tmp3D8[0]=& _tmp3D7,((_tmp3D8[1]=& _tmp3D6,
Cyc_aprintf(((_tmp3D9="format(scanf,%u,%u)",_tag_dyneither(_tmp3D9,sizeof(char),
20))),_tag_dyneither(_tmp3D8,sizeof(void*),2))))))))))))));}_LLB9: {struct Cyc_Absyn_Initializes_att_struct*
_tmp1B5=(struct Cyc_Absyn_Initializes_att_struct*)_tmp196;if(_tmp1B5->tag != 20)
goto _LLBB;else{_tmp1B6=_tmp1B5->f1;}}_LLBA: {const char*_tmp3DD;void*_tmp3DC[1];
struct Cyc_Int_pa_struct _tmp3DB;return(struct _dyneither_ptr)((_tmp3DB.tag=1,((
_tmp3DB.f1=(unsigned long)_tmp1B6,((_tmp3DC[0]=& _tmp3DB,Cyc_aprintf(((_tmp3DD="initializes(%d)",
_tag_dyneither(_tmp3DD,sizeof(char),16))),_tag_dyneither(_tmp3DC,sizeof(void*),1))))))));}
_LLBB: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp1B7=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp196;if(_tmp1B7->tag != 21)goto _LLBD;else{_tmp1B8=_tmp1B7->f1;}}_LLBC: {const
char*_tmp3E1;void*_tmp3E0[1];struct Cyc_Int_pa_struct _tmp3DF;return(struct
_dyneither_ptr)((_tmp3DF.tag=1,((_tmp3DF.f1=(unsigned long)_tmp1B8,((_tmp3E0[0]=&
_tmp3DF,Cyc_aprintf(((_tmp3E1="noliveunique(%d)",_tag_dyneither(_tmp3E1,sizeof(
char),17))),_tag_dyneither(_tmp3E0,sizeof(void*),1))))))));}_LLBD: {struct Cyc_Absyn_Pure_att_struct*
_tmp1B9=(struct Cyc_Absyn_Pure_att_struct*)_tmp196;if(_tmp1B9->tag != 22)goto _LLBF;}
_LLBE: {const char*_tmp3E2;return(_tmp3E2="pure",_tag_dyneither(_tmp3E2,sizeof(
char),5));}_LLBF: {struct Cyc_Absyn_Mode_att_struct*_tmp1BA=(struct Cyc_Absyn_Mode_att_struct*)
_tmp196;if(_tmp1BA->tag != 23)goto _LL8E;else{_tmp1BB=_tmp1BA->f1;}}_LLC0: {const
char*_tmp3E6;void*_tmp3E5[1];struct Cyc_String_pa_struct _tmp3E4;return(struct
_dyneither_ptr)((_tmp3E4.tag=0,((_tmp3E4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp1BB),((_tmp3E5[0]=& _tmp3E4,Cyc_aprintf(((_tmp3E6="__mode__(\"%s\")",
_tag_dyneither(_tmp3E6,sizeof(char),15))),_tag_dyneither(_tmp3E5,sizeof(void*),1))))))));}
_LL8E:;}int Cyc_Absyn_fntype_att(void*a){void*_tmp1EB=a;int _tmp1ED;_LLC5: {struct
Cyc_Absyn_Regparm_att_struct*_tmp1EC=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp1EB;if(_tmp1EC->tag != 0)goto _LLC7;else{_tmp1ED=_tmp1EC->f1;}}_LLC6: goto _LLC8;
_LLC7: {struct Cyc_Absyn_Fastcall_att_struct*_tmp1EE=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp1EB;if(_tmp1EE->tag != 3)goto _LLC9;}_LLC8: goto _LLCA;_LLC9: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp1EF=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp1EB;if(_tmp1EF->tag != 1)goto
_LLCB;}_LLCA: goto _LLCC;_LLCB: {struct Cyc_Absyn_Cdecl_att_struct*_tmp1F0=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp1EB;if(_tmp1F0->tag != 2)goto _LLCD;}_LLCC: goto
_LLCE;_LLCD: {struct Cyc_Absyn_Noreturn_att_struct*_tmp1F1=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp1EB;if(_tmp1F1->tag != 4)goto _LLCF;}_LLCE: goto _LLD0;_LLCF: {struct Cyc_Absyn_Pure_att_struct*
_tmp1F2=(struct Cyc_Absyn_Pure_att_struct*)_tmp1EB;if(_tmp1F2->tag != 22)goto _LLD1;}
_LLD0: goto _LLD2;_LLD1: {struct Cyc_Absyn_Format_att_struct*_tmp1F3=(struct Cyc_Absyn_Format_att_struct*)
_tmp1EB;if(_tmp1F3->tag != 19)goto _LLD3;}_LLD2: goto _LLD4;_LLD3: {struct Cyc_Absyn_Const_att_struct*
_tmp1F4=(struct Cyc_Absyn_Const_att_struct*)_tmp1EB;if(_tmp1F4->tag != 5)goto _LLD5;}
_LLD4: return 1;_LLD5: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp1F5=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp1EB;if(_tmp1F5->tag != 21)goto _LLD7;}_LLD6: return 1;_LLD7: {struct Cyc_Absyn_Initializes_att_struct*
_tmp1F6=(struct Cyc_Absyn_Initializes_att_struct*)_tmp1EB;if(_tmp1F6->tag != 20)
goto _LLD9;}_LLD8: return 1;_LLD9:;_LLDA: return 0;_LLC4:;}static char _tmp1F7[3]="f0";
static struct _dyneither_ptr Cyc_Absyn_f0={_tmp1F7,_tmp1F7,_tmp1F7 + 3};static struct
_dyneither_ptr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};static struct
_dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(
void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct
_dyneither_ptr**)Cyc_Absyn_field_names_v + 1)};struct _dyneither_ptr*Cyc_Absyn_fieldname(
int i);static void _tmp3F4(unsigned int*fsz,unsigned int*_tmp3F3,unsigned int*
_tmp3F2,struct _dyneither_ptr***_tmp3F0){for(*_tmp3F3=0;*_tmp3F3 < *_tmp3F2;(*
_tmp3F3)++){struct Cyc_Int_pa_struct _tmp3EE;void*_tmp3ED[1];const char*_tmp3EC;
struct _dyneither_ptr*_tmp3EB;(*_tmp3F0)[*_tmp3F3]=*_tmp3F3 < *fsz?*((struct
_dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct
_dyneither_ptr*),(int)*_tmp3F3)):((_tmp3EB=_cycalloc(sizeof(*_tmp3EB)),((_tmp3EB[
0]=(struct _dyneither_ptr)((_tmp3EE.tag=1,((_tmp3EE.f1=(unsigned long)((int)*
_tmp3F3),((_tmp3ED[0]=& _tmp3EE,Cyc_aprintf(((_tmp3EC="f%d",_tag_dyneither(
_tmp3EC,sizeof(char),4))),_tag_dyneither(_tmp3ED,sizeof(void*),1)))))))),_tmp3EB))));}}
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){unsigned int fsz=
_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));if(i >= 
fsz){unsigned int _tmp3F3;unsigned int _tmp3F2;struct _dyneither_ptr _tmp3F1;struct
_dyneither_ptr**_tmp3F0;unsigned int _tmp3EF;Cyc_Absyn_field_names=((_tmp3EF=(
unsigned int)(i + 1),((_tmp3F0=(struct _dyneither_ptr**)_cycalloc(_check_times(
sizeof(struct _dyneither_ptr*),_tmp3EF)),((_tmp3F1=_tag_dyneither(_tmp3F0,sizeof(
struct _dyneither_ptr*),_tmp3EF),((((_tmp3F2=_tmp3EF,_tmp3F4(& fsz,& _tmp3F3,&
_tmp3F2,& _tmp3F0))),_tmp3F1))))))));}return*((struct _dyneither_ptr**)
_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU info){union Cyc_Absyn_AggrInfoU
_tmp200=info;struct _tuple2 _tmp201;enum Cyc_Absyn_AggrKind _tmp202;struct _tuple0*
_tmp203;struct Cyc_Absyn_Aggrdecl**_tmp204;struct Cyc_Absyn_Aggrdecl*_tmp205;
struct Cyc_Absyn_Aggrdecl _tmp206;enum Cyc_Absyn_AggrKind _tmp207;struct _tuple0*
_tmp208;_LLDC: if((_tmp200.UnknownAggr).tag != 1)goto _LLDE;_tmp201=(struct _tuple2)(
_tmp200.UnknownAggr).val;_tmp202=_tmp201.f1;_tmp203=_tmp201.f2;_LLDD: {struct
_tuple10 _tmp3F5;return(_tmp3F5.f1=_tmp202,((_tmp3F5.f2=_tmp203,_tmp3F5)));}_LLDE:
if((_tmp200.KnownAggr).tag != 2)goto _LLDB;_tmp204=(struct Cyc_Absyn_Aggrdecl**)(
_tmp200.KnownAggr).val;_tmp205=*_tmp204;_tmp206=*_tmp205;_tmp207=_tmp206.kind;
_tmp208=_tmp206.name;_LLDF: {struct _tuple10 _tmp3F6;return(_tmp3F6.f1=_tmp207,((
_tmp3F6.f2=_tmp208,_tmp3F6)));}_LLDB:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info){union Cyc_Absyn_AggrInfoU _tmp20B=info;struct _tuple2
_tmp20C;struct Cyc_Absyn_Aggrdecl**_tmp20D;struct Cyc_Absyn_Aggrdecl*_tmp20E;_LLE1:
if((_tmp20B.UnknownAggr).tag != 1)goto _LLE3;_tmp20C=(struct _tuple2)(_tmp20B.UnknownAggr).val;
_LLE2: {const char*_tmp3F9;void*_tmp3F8;(_tmp3F8=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3F9="unchecked aggrdecl",
_tag_dyneither(_tmp3F9,sizeof(char),19))),_tag_dyneither(_tmp3F8,sizeof(void*),0)));}
_LLE3: if((_tmp20B.KnownAggr).tag != 2)goto _LLE0;_tmp20D=(struct Cyc_Absyn_Aggrdecl**)(
_tmp20B.KnownAggr).val;_tmp20E=*_tmp20D;_LLE4: return _tmp20E;_LLE0:;}int Cyc_Absyn_is_union_type(
void*t){void*_tmp211=Cyc_Tcutil_compress(t);enum Cyc_Absyn_AggrKind _tmp213;
struct Cyc_Absyn_AggrInfo _tmp215;union Cyc_Absyn_AggrInfoU _tmp216;_LLE6: {struct
Cyc_Absyn_AnonAggrType_struct*_tmp212=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp211;if(_tmp212->tag != 13)goto _LLE8;else{_tmp213=_tmp212->f1;if(_tmp213 != Cyc_Absyn_UnionA)
goto _LLE8;}}_LLE7: return 1;_LLE8: {struct Cyc_Absyn_AggrType_struct*_tmp214=(
struct Cyc_Absyn_AggrType_struct*)_tmp211;if(_tmp214->tag != 12)goto _LLEA;else{
_tmp215=_tmp214->f1;_tmp216=_tmp215.aggr_info;}}_LLE9: return(Cyc_Absyn_aggr_kinded_name(
_tmp216)).f1 == Cyc_Absyn_UnionA;_LLEA:;_LLEB: return 0;_LLE5:;}int Cyc_Absyn_is_nontagged_union_type(
void*t){void*_tmp217=Cyc_Tcutil_compress(t);enum Cyc_Absyn_AggrKind _tmp219;
struct Cyc_Absyn_AggrInfo _tmp21B;union Cyc_Absyn_AggrInfoU _tmp21C;_LLED: {struct
Cyc_Absyn_AnonAggrType_struct*_tmp218=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp217;if(_tmp218->tag != 13)goto _LLEF;else{_tmp219=_tmp218->f1;if(_tmp219 != Cyc_Absyn_UnionA)
goto _LLEF;}}_LLEE: return 1;_LLEF: {struct Cyc_Absyn_AggrType_struct*_tmp21A=(
struct Cyc_Absyn_AggrType_struct*)_tmp217;if(_tmp21A->tag != 12)goto _LLF1;else{
_tmp21B=_tmp21A->f1;_tmp21C=_tmp21B.aggr_info;}}_LLF0: {union Cyc_Absyn_AggrInfoU
_tmp21D=_tmp21C;struct Cyc_Absyn_Aggrdecl**_tmp21E;struct Cyc_Absyn_Aggrdecl*
_tmp21F;struct _tuple2 _tmp220;enum Cyc_Absyn_AggrKind _tmp221;struct Cyc_Core_Opt*
_tmp222;struct _tuple2 _tmp223;enum Cyc_Absyn_AggrKind _tmp224;struct Cyc_Core_Opt*
_tmp225;struct Cyc_Core_Opt _tmp226;int _tmp227;_LLF4: if((_tmp21D.KnownAggr).tag != 
2)goto _LLF6;_tmp21E=(struct Cyc_Absyn_Aggrdecl**)(_tmp21D.KnownAggr).val;_tmp21F=*
_tmp21E;_LLF5: return _tmp21F->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp21F->impl))->tagged == 0;_LLF6: if((_tmp21D.UnknownAggr).tag != 1)
goto _LLF8;_tmp220=(struct _tuple2)(_tmp21D.UnknownAggr).val;_tmp221=_tmp220.f1;
_tmp222=_tmp220.f3;if(_tmp222 != 0)goto _LLF8;_LLF7: return _tmp221 == Cyc_Absyn_UnionA;
_LLF8: if((_tmp21D.UnknownAggr).tag != 1)goto _LLF3;_tmp223=(struct _tuple2)(_tmp21D.UnknownAggr).val;
_tmp224=_tmp223.f1;_tmp225=_tmp223.f3;if(_tmp225 == 0)goto _LLF3;_tmp226=*_tmp225;
_tmp227=(int)_tmp226.v;_LLF9: return _tmp224 == Cyc_Absyn_UnionA  && !_tmp227;_LLF3:;}
_LLF1:;_LLF2: return 0;_LLEC:;}int Cyc_Absyn_is_aggr_type(void*t){void*_tmp228=Cyc_Tcutil_compress(
t);_LLFB: {struct Cyc_Absyn_DatatypeType_struct*_tmp229=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp228;if(_tmp229->tag != 3)goto _LLFD;}_LLFC: goto _LLFE;_LLFD: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp22A=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp228;if(_tmp22A->tag != 4)
goto _LLFF;}_LLFE: goto _LL100;_LLFF: {struct Cyc_Absyn_TupleType_struct*_tmp22B=(
struct Cyc_Absyn_TupleType_struct*)_tmp228;if(_tmp22B->tag != 11)goto _LL101;}
_LL100: goto _LL102;_LL101: {struct Cyc_Absyn_AggrType_struct*_tmp22C=(struct Cyc_Absyn_AggrType_struct*)
_tmp228;if(_tmp22C->tag != 12)goto _LL103;}_LL102: goto _LL104;_LL103: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp22D=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp228;if(_tmp22D->tag != 13)goto
_LL105;}_LL104: return 1;_LL105:;_LL106: return 0;_LLFA:;}struct Cyc_Absyn_Porton_d_struct
Cyc_Absyn_Porton_d_val={14};struct Cyc_Absyn_Portoff_d_struct Cyc_Absyn_Portoff_d_val={
15};int Cyc_Absyn_porting_c_code=0;int Cyc_Absyn_no_regions=0;
