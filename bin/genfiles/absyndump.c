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
extern void* _profile_GC_malloc(int,const char *file,const char *func,
                                int lineno);
extern void* _profile_GC_malloc_atomic(int,const char *file,
                                       const char *func,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
                                    const char *file,
                                    const char *func,
                                    int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						const char *file,
						const char *func,
                                                int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 const char *file,
                                 const char *func,
                                 int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
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
 typedef char*Cyc_Cstring;typedef char*Cyc_CstringNN;typedef struct _dyneither_ptr Cyc_string_t;
typedef struct _dyneither_ptr Cyc_mstring_t;typedef struct _dyneither_ptr*Cyc_stringptr_t;
typedef struct _dyneither_ptr*Cyc_mstringptr_t;typedef char*Cyc_Cbuffer_t;typedef
char*Cyc_CbufferNN_t;typedef struct _dyneither_ptr Cyc_buffer_t;typedef struct
_dyneither_ptr Cyc_mbuffer_t;typedef int Cyc_bool;struct Cyc_Core_NewRegion{struct
_DynRegionHandle*dynregion;};typedef unsigned long Cyc_size_t;typedef
unsigned short Cyc_mode_t;struct Cyc___cycFILE;typedef struct Cyc___cycFILE Cyc_FILE;
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};typedef void*Cyc_parg_t;struct _dyneither_ptr Cyc_aprintf(struct
_dyneither_ptr,struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);int Cyc_fputc(int,struct Cyc___cycFILE*);
struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int
tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};int Cyc_file_string_write(struct Cyc___cycFILE*,
struct _dyneither_ptr src,int src_offset,int max_count);typedef unsigned int Cyc_Core_sizeof_t;
struct Cyc_Core_Opt{void*v;};typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char
Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{char*tag;
struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_unique_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < 
dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
typedef struct Cyc_List_List*Cyc_List_list_t;typedef struct Cyc_List_List*Cyc_List_List_t;
int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*
f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_exists(int(*pred)(void*),
struct Cyc_List_List*x);unsigned long Cyc_strlen(struct _dyneither_ptr s);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];
struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;typedef
struct Cyc_Position_Segment*Cyc_Position_seg_t;struct Cyc_Position_Lex_struct{int
tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern
char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*tag;};
struct _dyneither_ptr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*
loc);typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;typedef struct
_dyneither_ptr*Cyc_Absyn_var_t;typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n
Loc_n;};typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};typedef struct _tuple0*
Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;typedef struct _tuple0*
Cyc_Absyn_typedef_name_t;typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
typedef void*Cyc_Absyn_kindbound_t;typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
typedef void*Cyc_Absyn_bounds_t;typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;typedef struct Cyc_Absyn_VarargInfo
Cyc_Absyn_vararg_info_t;typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;typedef
struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;typedef struct Cyc_Absyn_DatatypeFieldInfo
Cyc_Absyn_datatype_field_info_t;typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;typedef void*Cyc_Absyn_type_t;
typedef void*Cyc_Absyn_rgntype_t;typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;typedef enum Cyc_Absyn_Incrementor
Cyc_Absyn_incrementor_t;typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void*Cyc_Absyn_raw_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;typedef void*Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
typedef void*Cyc_Absyn_raw_pat_t;typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
typedef void*Cyc_Absyn_binding_t;typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;typedef struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_aggrdecl_t;typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;typedef struct Cyc_Absyn_Typedefdecl*
Cyc_Absyn_typedefdecl_t;typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;typedef struct Cyc_Absyn_Vardecl*
Cyc_Absyn_vardecl_t;typedef void*Cyc_Absyn_raw_decl_t;typedef struct Cyc_Absyn_Decl*
Cyc_Absyn_decl_t;typedef void*Cyc_Absyn_designator_t;typedef void*Cyc_Absyn_absyn_annot_t;
typedef void*Cyc_Absyn_attribute_t;typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;typedef void*Cyc_Absyn_offsetof_field_t;
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;typedef enum Cyc_Absyn_Coercion
Cyc_Absyn_coercion_t;typedef struct Cyc_Absyn_PtrLoc*Cyc_Absyn_ptrloc_t;enum Cyc_Absyn_Scope{
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
No_constr;};typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_struct{
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
int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{int tag;};typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;struct Cyc_Absyn_Tqual
ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
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
zt_loc;};struct Cyc_Absyn_Aggr_td_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};
struct Cyc_Absyn_Enum_td_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;};typedef void*Cyc_Absyn_raw_type_decl_t;
struct Cyc_Absyn_TypeDecl{void*r;struct Cyc_Position_Segment*loc;};typedef struct
Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_struct{int tag;
};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;
int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};
struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
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
};struct Cyc_Absyn_RefCntRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{int
tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};typedef void*Cyc_Absyn_funcparams_t;enum Cyc_Absyn_Format_Type{
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
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};extern
struct Cyc_Absyn_Stdcall_att_struct Cyc_Absyn_Stdcall_att_val;extern struct Cyc_Absyn_Cdecl_att_struct
Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Unused_att_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_struct{
int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};typedef void*Cyc_Absyn_type_modifier_t;struct
_union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int
tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};
struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6
val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int
tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;
};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct
_union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct
_union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct
_union_Cnst_Wstring_c Wstring_c;};enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times
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
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple8*f1;struct
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
_tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{
int tag;};struct Cyc_Absyn_Var_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct
Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};
struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*
f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_struct{
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
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
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
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct _dyneither_ptr
Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;
struct _tuple0*f2;};struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
struct Cyc_PP_Ppstate;typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int
add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;
int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop
p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple11{struct
_dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple8*Cyc_Absynpp_arg_mk_opt(
struct _tuple11*arg);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual
tq,void*t);struct _tuple13{unsigned int f1;int f2;};struct _tuple13 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[
7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;typedef const struct Cyc_Dict_T*
Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char
Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[
7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;typedef
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};typedef const struct
Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;
struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};typedef void*Cyc_Tcenv_resolved_t;
struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;
struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict
enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*
availables;};typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_NotLoop_j_struct{
int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{int tag;};struct Cyc_Tcenv_FnEnd_j_struct{
int tag;};struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};typedef
void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict
ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_check_no_qual(struct Cyc_Position_Segment*loc,void*t);
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};extern
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;static int Cyc_Absyndump_expand_typedefs;
static int Cyc_Absyndump_qvar_to_Cids;static int Cyc_Absyndump_add_cyc_prefix;static
int Cyc_Absyndump_generate_line_directives;static int Cyc_Absyndump_to_VC;void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs){Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_set_params(fs);}void Cyc_Absyndump_dumptyp(void*);void Cyc_Absyndump_dumpntyp(
void*t);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumpexp_prec(
int,struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);void
Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt);void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*);void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(
void*),void*a);void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(
void*),void*);void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);void
Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);typedef struct
_dyneither_ptr Cyc_Absyndump_dump_string_t;void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*);
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;void Cyc_Absyndump_ignore(
void*x){return;}static unsigned int Cyc_Absyndump_pos=0;static char Cyc_Absyndump_prev_char='x';
int Cyc_Absyndump_need_space_before(){switch(Cyc_Absyndump_prev_char){case '{':
_LL0: goto _LL1;case '}': _LL1: goto _LL2;case '(': _LL2: goto _LL3;case ')': _LL3: goto _LL4;
case '[': _LL4: goto _LL5;case ']': _LL5: goto _LL6;case ';': _LL6: goto _LL7;case ',': _LL7:
goto _LL8;case '=': _LL8: goto _LL9;case '?': _LL9: goto _LLA;case '!': _LLA: goto _LLB;case ' ':
_LLB: goto _LLC;case '\n': _LLC: goto _LLD;case '*': _LLD: return 0;default: _LLE: return 1;}}
void Cyc_Absyndump_dump(struct _dyneither_ptr s){int sz=(int)Cyc_strlen((struct
_dyneither_ptr)s);Cyc_Absyndump_pos +=sz;if(Cyc_Absyndump_pos > 80){Cyc_Absyndump_pos=(
unsigned int)sz;Cyc_fputc((int)'\n',*Cyc_Absyndump_dump_file);}else{if(Cyc_Absyndump_need_space_before())
Cyc_fputc((int)' ',*Cyc_Absyndump_dump_file);}if(sz >= 1){Cyc_Absyndump_prev_char=*((
const char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));Cyc_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);}}void Cyc_Absyndump_dump_nospace(struct
_dyneither_ptr s){int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);Cyc_Absyndump_pos
+=sz;if(sz >= 1){Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);Cyc_Absyndump_prev_char=*((
const char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));}}void Cyc_Absyndump_dump_char(
int c){++ Cyc_Absyndump_pos;Cyc_fputc(c,*Cyc_Absyndump_dump_file);Cyc_Absyndump_prev_char=(
char)c;}void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*loc){if(loc == 0)
return;if(!Cyc_Absyndump_generate_line_directives)return;{struct _dyneither_ptr
_tmp0=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(_tmp0);};}void Cyc_Absyndump_dump_str(
struct _dyneither_ptr*s){Cyc_Absyndump_dump(*s);}void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}void Cyc_Absyndump_dump_sep(void(*f)(void*),
struct Cyc_List_List*l,struct _dyneither_ptr sep){if(l == 0)return;for(0;l->tl != 0;l=
l->tl){f((void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f((void*)l->hd);}void Cyc_Absyndump_dump_sep_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)return;for(0;l->tl != 0;l=l->tl){f(env,(void*)l->hd);Cyc_Absyndump_dump_nospace(
sep);}f(env,(void*)l->hd);}void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){Cyc_Absyndump_dump_nospace(
start);Cyc_Absyndump_dump_sep(f,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,
struct _dyneither_ptr end,struct _dyneither_ptr sep){Cyc_Absyndump_dump_nospace(
start);Cyc_Absyndump_dump_sep_c(f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}
void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}void Cyc_Absyndump_dumpqvar(struct _tuple0*
v){struct Cyc_List_List*_tmp1=0;struct _dyneither_ptr**prefix=0;{union Cyc_Absyn_Nmspace
_tmp2=(*v).f1;int _tmp3;struct Cyc_List_List*_tmp4;struct Cyc_List_List*_tmp5;
struct Cyc_List_List*_tmp6;_LL11: if((_tmp2.Loc_n).tag != 4)goto _LL13;_tmp3=(int)(
_tmp2.Loc_n).val;_LL12: _tmp4=0;goto _LL14;_LL13: if((_tmp2.Rel_n).tag != 1)goto
_LL15;_tmp4=(struct Cyc_List_List*)(_tmp2.Rel_n).val;_LL14: _tmp1=_tmp4;goto _LL10;
_LL15: if((_tmp2.C_n).tag != 3)goto _LL17;_tmp5=(struct Cyc_List_List*)(_tmp2.C_n).val;
_LL16: Cyc_Absyndump_dump_char((int)' ');goto _LL10;_LL17: if((_tmp2.Abs_n).tag != 2)
goto _LL10;_tmp6=(struct Cyc_List_List*)(_tmp2.Abs_n).val;_LL18: if(Cyc_Absyndump_qvar_to_Cids
 && Cyc_Absyndump_add_cyc_prefix)prefix=(struct _dyneither_ptr**)& Cyc_Absynpp_cyc_stringptr;
_tmp1=_tmp6;goto _LL10;_LL10:;}if(prefix != 0){Cyc_Absyndump_dump_str(*prefix);if(
Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{const char*
_tmp3CA;Cyc_Absyndump_dump_nospace(((_tmp3CA="::",_tag_dyneither(_tmp3CA,sizeof(
char),3))));}}if(_tmp1 != 0){Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)
_tmp1->hd));for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){if(Cyc_Absyndump_qvar_to_Cids)
Cyc_Absyndump_dump_char((int)'_');else{const char*_tmp3CB;Cyc_Absyndump_dump_nospace(((
_tmp3CB="::",_tag_dyneither(_tmp3CB,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*((
struct _dyneither_ptr*)_tmp1->hd));}if(Cyc_Absyndump_qvar_to_Cids){const char*
_tmp3CC;Cyc_Absyndump_dump_nospace(((_tmp3CC="_",_tag_dyneither(_tmp3CC,sizeof(
char),2))));}else{const char*_tmp3CD;Cyc_Absyndump_dump_nospace(((_tmp3CD="::",
_tag_dyneither(_tmp3CD,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*(*v).f2);}
else{if(prefix != 0)Cyc_Absyndump_dump_nospace(*(*v).f2);else{Cyc_Absyndump_dump_str((*
v).f2);}}}void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict){
const char*_tmp3CE;Cyc_Absyndump_dump(((_tmp3CE="restrict",_tag_dyneither(_tmp3CE,
sizeof(char),9))));}if(tq.q_volatile){const char*_tmp3CF;Cyc_Absyndump_dump(((
_tmp3CF="volatile",_tag_dyneither(_tmp3CF,sizeof(char),9))));}if(tq.print_const){
const char*_tmp3D0;Cyc_Absyndump_dump(((_tmp3D0="const",_tag_dyneither(_tmp3D0,
sizeof(char),6))));}}void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){switch(
sc){case Cyc_Absyn_Static: _LL19:{const char*_tmp3D1;Cyc_Absyndump_dump(((_tmp3D1="static",
_tag_dyneither(_tmp3D1,sizeof(char),7))));}return;case Cyc_Absyn_Public: _LL1A:
return;case Cyc_Absyn_Extern: _LL1B:{const char*_tmp3D2;Cyc_Absyndump_dump(((
_tmp3D2="extern",_tag_dyneither(_tmp3D2,sizeof(char),7))));}return;case Cyc_Absyn_ExternC:
_LL1C:{const char*_tmp3D3;Cyc_Absyndump_dump(((_tmp3D3="extern \"C\"",
_tag_dyneither(_tmp3D3,sizeof(char),11))));}return;case Cyc_Absyn_Abstract: _LL1D:{
const char*_tmp3D4;Cyc_Absyndump_dump(((_tmp3D4="abstract",_tag_dyneither(_tmp3D4,
sizeof(char),9))));}return;case Cyc_Absyn_Register: _LL1E:{const char*_tmp3D5;Cyc_Absyndump_dump(((
_tmp3D5="register",_tag_dyneither(_tmp3D5,sizeof(char),9))));}return;}}void Cyc_Absyndump_dumpkind(
struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp14;enum Cyc_Absyn_KindQual
_tmp15;enum Cyc_Absyn_AliasQual _tmp16;struct Cyc_Absyn_Kind*_tmp13=ka;_tmp14=*
_tmp13;_tmp15=_tmp14.kind;_tmp16=_tmp14.aliasqual;switch(_tmp15){case Cyc_Absyn_AnyKind:
_LL20: switch(_tmp16){case Cyc_Absyn_Aliasable: _LL22:{const char*_tmp3D6;Cyc_Absyndump_dump(((
_tmp3D6="A",_tag_dyneither(_tmp3D6,sizeof(char),2))));}return;case Cyc_Absyn_Unique:
_LL23:{const char*_tmp3D7;Cyc_Absyndump_dump(((_tmp3D7="UA",_tag_dyneither(
_tmp3D7,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL24:{const char*_tmp3D8;
Cyc_Absyndump_dump(((_tmp3D8="TA",_tag_dyneither(_tmp3D8,sizeof(char),3))));}
return;}case Cyc_Absyn_MemKind: _LL21: switch(_tmp16){case Cyc_Absyn_Aliasable: _LL27:{
const char*_tmp3D9;Cyc_Absyndump_dump(((_tmp3D9="M",_tag_dyneither(_tmp3D9,
sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL28:{const char*_tmp3DA;Cyc_Absyndump_dump(((
_tmp3DA="UM",_tag_dyneither(_tmp3DA,sizeof(char),3))));}return;case Cyc_Absyn_Top:
_LL29:{const char*_tmp3DB;Cyc_Absyndump_dump(((_tmp3DB="TM",_tag_dyneither(
_tmp3DB,sizeof(char),3))));}return;}case Cyc_Absyn_BoxKind: _LL26: switch(_tmp16){
case Cyc_Absyn_Aliasable: _LL2C:{const char*_tmp3DC;Cyc_Absyndump_dump(((_tmp3DC="B",
_tag_dyneither(_tmp3DC,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL2D:{
const char*_tmp3DD;Cyc_Absyndump_dump(((_tmp3DD="UB",_tag_dyneither(_tmp3DD,
sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL2E:{const char*_tmp3DE;Cyc_Absyndump_dump(((
_tmp3DE="TB",_tag_dyneither(_tmp3DE,sizeof(char),3))));}return;}case Cyc_Absyn_RgnKind:
_LL2B: switch(_tmp16){case Cyc_Absyn_Aliasable: _LL31:{const char*_tmp3DF;Cyc_Absyndump_dump(((
_tmp3DF="R",_tag_dyneither(_tmp3DF,sizeof(char),2))));}return;case Cyc_Absyn_Unique:
_LL32:{const char*_tmp3E0;Cyc_Absyndump_dump(((_tmp3E0="UR",_tag_dyneither(
_tmp3E0,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL33:{const char*_tmp3E1;
Cyc_Absyndump_dump(((_tmp3E1="TR",_tag_dyneither(_tmp3E1,sizeof(char),3))));}
return;}case Cyc_Absyn_EffKind: _LL30:{const char*_tmp3E2;Cyc_Absyndump_dump(((
_tmp3E2="E",_tag_dyneither(_tmp3E2,sizeof(char),2))));}return;case Cyc_Absyn_IntKind:
_LL35:{const char*_tmp3E3;Cyc_Absyndump_dump(((_tmp3E3="I",_tag_dyneither(_tmp3E3,
sizeof(char),2))));}return;}}void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind
k){switch(k){case Cyc_Absyn_StructA: _LL37:{const char*_tmp3E4;Cyc_Absyndump_dump(((
_tmp3E4="struct",_tag_dyneither(_tmp3E4,sizeof(char),7))));}return;case Cyc_Absyn_UnionA:
_LL38:{const char*_tmp3E5;Cyc_Absyndump_dump(((_tmp3E5="union",_tag_dyneither(
_tmp3E5,sizeof(char),6))));}return;}}void Cyc_Absyndump_dumptps(struct Cyc_List_List*
ts){const char*_tmp3E8;const char*_tmp3E7;const char*_tmp3E6;((void(*)(void(*f)(
void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,
struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,ts,((
_tmp3E6="<",_tag_dyneither(_tmp3E6,sizeof(char),2))),((_tmp3E7=">",
_tag_dyneither(_tmp3E7,sizeof(char),2))),((_tmp3E8=",",_tag_dyneither(_tmp3E8,
sizeof(char),2))));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);}void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);{void*_tmp2A=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*
_tmp2C;struct Cyc_Absyn_Kind _tmp2D;enum Cyc_Absyn_KindQual _tmp2E;enum Cyc_Absyn_AliasQual
_tmp2F;struct Cyc_Absyn_Kind*_tmp33;_LL3B: {struct Cyc_Absyn_Eq_kb_struct*_tmp2B=(
struct Cyc_Absyn_Eq_kb_struct*)_tmp2A;if(_tmp2B->tag != 0)goto _LL3D;else{_tmp2C=
_tmp2B->f1;_tmp2D=*_tmp2C;_tmp2E=_tmp2D.kind;if(_tmp2E != Cyc_Absyn_BoxKind)goto
_LL3D;_tmp2F=_tmp2D.aliasqual;if(_tmp2F != Cyc_Absyn_Aliasable)goto _LL3D;}}_LL3C:
goto _LL3E;_LL3D: {struct Cyc_Absyn_Less_kb_struct*_tmp30=(struct Cyc_Absyn_Less_kb_struct*)
_tmp2A;if(_tmp30->tag != 2)goto _LL3F;}_LL3E: goto _LL40;_LL3F: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp31=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp2A;if(_tmp31->tag != 1)goto _LL41;}
_LL40:{const char*_tmp3E9;Cyc_Absyndump_dump(((_tmp3E9="::?",_tag_dyneither(
_tmp3E9,sizeof(char),4))));}goto _LL3A;_LL41: {struct Cyc_Absyn_Eq_kb_struct*
_tmp32=(struct Cyc_Absyn_Eq_kb_struct*)_tmp2A;if(_tmp32->tag != 0)goto _LL3A;else{
_tmp33=_tmp32->f1;}}_LL42:{const char*_tmp3EA;Cyc_Absyndump_dump(((_tmp3EA="::",
_tag_dyneither(_tmp3EA,sizeof(char),3))));}Cyc_Absyndump_dumpkind(_tmp33);goto
_LL3A;_LL3A:;};}void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){const char*
_tmp3ED;const char*_tmp3EC;const char*_tmp3EB;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,tvs,((_tmp3EB="<",
_tag_dyneither(_tmp3EB,sizeof(char),2))),((_tmp3EC=">",_tag_dyneither(_tmp3EC,
sizeof(char),2))),((_tmp3ED=",",_tag_dyneither(_tmp3ED,sizeof(char),2))));}void
Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){const char*_tmp3F0;const
char*_tmp3EF;const char*_tmp3EE;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,tvs,((
_tmp3EE="<",_tag_dyneither(_tmp3EE,sizeof(char),2))),((_tmp3EF=">",
_tag_dyneither(_tmp3EF,sizeof(char),2))),((_tmp3F0=",",_tag_dyneither(_tmp3F0,
sizeof(char),2))));}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(
struct _tuple14*pr){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){const char*_tmp3F3;const char*_tmp3F2;const char*_tmp3F1;((
void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _dyneither_ptr
start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,
ts,((_tmp3F1="(",_tag_dyneither(_tmp3F1,sizeof(char),2))),((_tmp3F2=")",
_tag_dyneither(_tmp3F2,sizeof(char),2))),((_tmp3F3=",",_tag_dyneither(_tmp3F3,
sizeof(char),2))));}void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp3F=(void*)atts->hd;_LL44: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp40=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp3F;if(_tmp40->tag != 1)goto _LL46;}
_LL45:{const char*_tmp3F4;Cyc_Absyndump_dump(((_tmp3F4="_stdcall",_tag_dyneither(
_tmp3F4,sizeof(char),9))));}return;_LL46: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp41=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp3F;if(_tmp41->tag != 2)goto _LL48;}
_LL47:{const char*_tmp3F5;Cyc_Absyndump_dump(((_tmp3F5="_cdecl",_tag_dyneither(
_tmp3F5,sizeof(char),7))));}return;_LL48: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp42=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp3F;if(_tmp42->tag != 3)goto _LL4A;}
_LL49:{const char*_tmp3F6;Cyc_Absyndump_dump(((_tmp3F6="_fastcall",_tag_dyneither(
_tmp3F6,sizeof(char),10))));}return;_LL4A:;_LL4B: goto _LL43;_LL43:;}}void Cyc_Absyndump_dump_noncallconv(
struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;
atts2 != 0;atts2=atts2->tl){void*_tmp46=(void*)((struct Cyc_List_List*)_check_null(
atts))->hd;_LL4D: {struct Cyc_Absyn_Stdcall_att_struct*_tmp47=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp46;if(_tmp47->tag != 1)goto _LL4F;}_LL4E: goto _LL50;_LL4F: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp48=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp46;if(_tmp48->tag != 2)goto _LL51;}
_LL50: goto _LL52;_LL51: {struct Cyc_Absyn_Fastcall_att_struct*_tmp49=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp46;if(_tmp49->tag != 3)goto _LL53;}_LL52: goto _LL4C;_LL53:;_LL54: hasatt=1;goto
_LL4C;_LL4C:;}}if(!hasatt)return;{const char*_tmp3F7;Cyc_Absyndump_dump(((_tmp3F7="__declspec(",
_tag_dyneither(_tmp3F7,sizeof(char),12))));}for(0;atts != 0;atts=atts->tl){void*
_tmp4B=(void*)atts->hd;_LL56: {struct Cyc_Absyn_Stdcall_att_struct*_tmp4C=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmp4B;if(_tmp4C->tag != 1)goto _LL58;}_LL57: goto
_LL59;_LL58: {struct Cyc_Absyn_Cdecl_att_struct*_tmp4D=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp4B;if(_tmp4D->tag != 2)goto _LL5A;}_LL59: goto _LL5B;_LL5A: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp4E=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp4B;if(_tmp4E->tag != 3)goto _LL5C;}
_LL5B: goto _LL55;_LL5C:;_LL5D: Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)
atts->hd));goto _LL55;_LL55:;}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpatts(
struct Cyc_List_List*atts){if(atts == 0)return;switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL5E:{const char*_tmp3F8;Cyc_Absyndump_dump(((_tmp3F8=" __attribute__((",
_tag_dyneither(_tmp3F8,sizeof(char),17))));}for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));if(atts->tl != 0){const char*_tmp3F9;
Cyc_Absyndump_dump(((_tmp3F9=",",_tag_dyneither(_tmp3F9,sizeof(char),2))));}}{
const char*_tmp3FA;Cyc_Absyndump_dump(((_tmp3FA=")) ",_tag_dyneither(_tmp3FA,
sizeof(char),4))));}return;case Cyc_Cyclone_Vc_c: _LL5F: Cyc_Absyndump_dump_noncallconv(
atts);return;}}int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){if(tms
== 0)return 0;{void*_tmp52=(void*)tms->hd;_LL62: {struct Cyc_Absyn_Pointer_mod_struct*
_tmp53=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp52;if(_tmp53->tag != 2)goto _LL64;}
_LL63: return 1;_LL64:;_LL65: return 0;_LL61:;};}static void Cyc_Absyndump_dumprgn(
void*t){void*_tmp54=Cyc_Tcutil_compress(t);_LL67: {struct Cyc_Absyn_HeapRgn_struct*
_tmp55=(struct Cyc_Absyn_HeapRgn_struct*)_tmp54;if(_tmp55->tag != 20)goto _LL69;}
_LL68:{const char*_tmp3FB;Cyc_Absyndump_dump(((_tmp3FB="`H",_tag_dyneither(
_tmp3FB,sizeof(char),3))));}goto _LL66;_LL69:;_LL6A: Cyc_Absyndump_dumpntyp(t);
goto _LL66;_LL66:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
static struct _tuple15 Cyc_Absyndump_effects_split(void*t){struct Cyc_List_List*
rgions=0;struct Cyc_List_List*effects=0;{void*_tmp57=Cyc_Tcutil_compress(t);void*
_tmp59;struct Cyc_List_List*_tmp5B;_LL6C: {struct Cyc_Absyn_AccessEff_struct*
_tmp58=(struct Cyc_Absyn_AccessEff_struct*)_tmp57;if(_tmp58->tag != 23)goto _LL6E;
else{_tmp59=(void*)_tmp58->f1;}}_LL6D:{struct Cyc_List_List*_tmp3FC;rgions=((
_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC->hd=_tmp59,((_tmp3FC->tl=rgions,
_tmp3FC))))));}goto _LL6B;_LL6E: {struct Cyc_Absyn_JoinEff_struct*_tmp5A=(struct
Cyc_Absyn_JoinEff_struct*)_tmp57;if(_tmp5A->tag != 24)goto _LL70;else{_tmp5B=
_tmp5A->f1;}}_LL6F: for(0;_tmp5B != 0;_tmp5B=_tmp5B->tl){struct Cyc_List_List*
_tmp5E;struct Cyc_List_List*_tmp5F;struct _tuple15 _tmp5D=Cyc_Absyndump_effects_split((
void*)_tmp5B->hd);_tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;rgions=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5E,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp5F,effects);}goto _LL6B;_LL70:;_LL71:{struct Cyc_List_List*
_tmp3FD;effects=((_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((_tmp3FD->hd=t,((_tmp3FD->tl=
effects,_tmp3FD))))));}goto _LL6B;_LL6B:;}{struct _tuple15 _tmp3FE;return(_tmp3FE.f1=
rgions,((_tmp3FE.f2=effects,_tmp3FE)));};}static void Cyc_Absyndump_dumpeff(void*t){
struct Cyc_List_List*_tmp63;struct Cyc_List_List*_tmp64;struct _tuple15 _tmp62=Cyc_Absyndump_effects_split(
t);_tmp63=_tmp62.f1;_tmp64=_tmp62.f2;_tmp63=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp63);_tmp64=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp64);for(0;_tmp64 != 0;_tmp64=_tmp64->tl){Cyc_Absyndump_dumpntyp((
void*)_tmp64->hd);Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp63 != 0;_tmp63=_tmp63->tl){Cyc_Absyndump_dumprgn((void*)_tmp63->hd);
if(_tmp63->tl != 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump_char((
int)'}');}static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);static void Cyc_Absyndump_dump_datatypedecl(
struct Cyc_Absyn_Datatypedecl*);void Cyc_Absyndump_dumpntyp(void*t){void*_tmp65=t;
struct Cyc_Absyn_Tvar*_tmp6B;struct Cyc_Core_Opt*_tmp6D;struct Cyc_Core_Opt*_tmp6E;
int _tmp6F;struct Cyc_Core_Opt*_tmp71;struct Cyc_Core_Opt*_tmp72;struct Cyc_Core_Opt
_tmp73;void*_tmp74;int _tmp75;struct Cyc_Absyn_DatatypeInfo _tmp77;union Cyc_Absyn_DatatypeInfoU
_tmp78;struct Cyc_List_List*_tmp79;struct Cyc_Absyn_DatatypeFieldInfo _tmp7B;union
Cyc_Absyn_DatatypeFieldInfoU _tmp7C;struct Cyc_List_List*_tmp7D;enum Cyc_Absyn_Sign
_tmp7F;enum Cyc_Absyn_Size_of _tmp80;enum Cyc_Absyn_Sign _tmp82;enum Cyc_Absyn_Size_of
_tmp83;enum Cyc_Absyn_Sign _tmp85;enum Cyc_Absyn_Size_of _tmp86;enum Cyc_Absyn_Sign
_tmp88;enum Cyc_Absyn_Size_of _tmp89;enum Cyc_Absyn_Sign _tmp8B;enum Cyc_Absyn_Size_of
_tmp8C;enum Cyc_Absyn_Sign _tmp8E;enum Cyc_Absyn_Size_of _tmp8F;enum Cyc_Absyn_Sign
_tmp91;enum Cyc_Absyn_Size_of _tmp92;enum Cyc_Absyn_Sign _tmp94;enum Cyc_Absyn_Size_of
_tmp95;enum Cyc_Absyn_Sign _tmp97;enum Cyc_Absyn_Size_of _tmp98;enum Cyc_Absyn_Sign
_tmp9A;enum Cyc_Absyn_Size_of _tmp9B;enum Cyc_Absyn_Sign _tmp9D;enum Cyc_Absyn_Size_of
_tmp9E;enum Cyc_Absyn_Sign _tmpA0;enum Cyc_Absyn_Size_of _tmpA1;enum Cyc_Absyn_Sign
_tmpA3;enum Cyc_Absyn_Size_of _tmpA4;enum Cyc_Absyn_Sign _tmpA6;enum Cyc_Absyn_Size_of
_tmpA7;enum Cyc_Absyn_Sign _tmpA9;enum Cyc_Absyn_Size_of _tmpAA;int _tmpAC;struct
Cyc_List_List*_tmpAE;struct Cyc_Absyn_AggrInfo _tmpB0;union Cyc_Absyn_AggrInfoU
_tmpB1;struct Cyc_List_List*_tmpB2;enum Cyc_Absyn_AggrKind _tmpB4;struct Cyc_List_List*
_tmpB5;struct _tuple0*_tmpB7;struct Cyc_List_List*_tmpB9;struct _tuple0*_tmpBB;
struct Cyc_List_List*_tmpBC;struct Cyc_Absyn_Exp*_tmpBE;void*_tmpC0;void*_tmpC2;
void*_tmpC3;void*_tmpC5;struct Cyc_Absyn_TypeDecl*_tmpC9;struct Cyc_Absyn_TypeDecl
_tmpCA;void*_tmpCB;struct Cyc_Absyn_Aggrdecl*_tmpCD;struct Cyc_Absyn_TypeDecl*
_tmpCF;struct Cyc_Absyn_TypeDecl _tmpD0;void*_tmpD1;struct Cyc_Absyn_Enumdecl*
_tmpD3;struct Cyc_Absyn_TypeDecl*_tmpD5;struct Cyc_Absyn_TypeDecl _tmpD6;void*
_tmpD7;struct Cyc_Absyn_Datatypedecl*_tmpD9;_LL73: {struct Cyc_Absyn_ArrayType_struct*
_tmp66=(struct Cyc_Absyn_ArrayType_struct*)_tmp65;if(_tmp66->tag != 8)goto _LL75;}
_LL74: goto _LL76;_LL75: {struct Cyc_Absyn_FnType_struct*_tmp67=(struct Cyc_Absyn_FnType_struct*)
_tmp65;if(_tmp67->tag != 9)goto _LL77;}_LL76: goto _LL78;_LL77: {struct Cyc_Absyn_PointerType_struct*
_tmp68=(struct Cyc_Absyn_PointerType_struct*)_tmp65;if(_tmp68->tag != 5)goto _LL79;}
_LL78: return;_LL79: {struct Cyc_Absyn_VoidType_struct*_tmp69=(struct Cyc_Absyn_VoidType_struct*)
_tmp65;if(_tmp69->tag != 0)goto _LL7B;}_LL7A:{const char*_tmp3FF;Cyc_Absyndump_dump(((
_tmp3FF="void",_tag_dyneither(_tmp3FF,sizeof(char),5))));}return;_LL7B: {struct
Cyc_Absyn_VarType_struct*_tmp6A=(struct Cyc_Absyn_VarType_struct*)_tmp65;if(
_tmp6A->tag != 2)goto _LL7D;else{_tmp6B=_tmp6A->f1;}}_LL7C: Cyc_Absyndump_dump_str(
_tmp6B->name);return;_LL7D: {struct Cyc_Absyn_Evar_struct*_tmp6C=(struct Cyc_Absyn_Evar_struct*)
_tmp65;if(_tmp6C->tag != 1)goto _LL7F;else{_tmp6D=_tmp6C->f1;_tmp6E=_tmp6C->f2;if(
_tmp6E != 0)goto _LL7F;_tmp6F=_tmp6C->f3;}}_LL7E:{const char*_tmp400;Cyc_Absyndump_dump(((
_tmp400="%",_tag_dyneither(_tmp400,sizeof(char),2))));}if(_tmp6D == 0){const char*
_tmp401;Cyc_Absyndump_dump(((_tmp401="?",_tag_dyneither(_tmp401,sizeof(char),2))));}
else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmp6D->v);}{const char*
_tmp405;void*_tmp404[1];struct Cyc_Int_pa_struct _tmp403;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp403.tag=1,((_tmp403.f1=(unsigned long)_tmp6F,((
_tmp404[0]=& _tmp403,Cyc_aprintf(((_tmp405="(%d)",_tag_dyneither(_tmp405,sizeof(
char),5))),_tag_dyneither(_tmp404,sizeof(void*),1)))))))));}return;_LL7F: {
struct Cyc_Absyn_Evar_struct*_tmp70=(struct Cyc_Absyn_Evar_struct*)_tmp65;if(
_tmp70->tag != 1)goto _LL81;else{_tmp71=_tmp70->f1;_tmp72=_tmp70->f2;if(_tmp72 == 0)
goto _LL81;_tmp73=*_tmp72;_tmp74=(void*)_tmp73.v;_tmp75=_tmp70->f3;}}_LL80: Cyc_Absyndump_dumpntyp(
_tmp74);return;_LL81: {struct Cyc_Absyn_DatatypeType_struct*_tmp76=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp65;if(_tmp76->tag != 3)goto _LL83;else{_tmp77=_tmp76->f1;_tmp78=_tmp77.datatype_info;
_tmp79=_tmp77.targs;}}_LL82:{union Cyc_Absyn_DatatypeInfoU _tmpE4=_tmp78;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpE5;struct _tuple0*_tmpE6;int _tmpE7;struct Cyc_Absyn_Datatypedecl**_tmpE8;
struct Cyc_Absyn_Datatypedecl*_tmpE9;struct Cyc_Absyn_Datatypedecl _tmpEA;struct
_tuple0*_tmpEB;int _tmpEC;_LLCC: if((_tmpE4.UnknownDatatype).tag != 1)goto _LLCE;
_tmpE5=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpE4.UnknownDatatype).val;_tmpE6=
_tmpE5.name;_tmpE7=_tmpE5.is_extensible;_LLCD: _tmpEB=_tmpE6;_tmpEC=_tmpE7;goto
_LLCF;_LLCE: if((_tmpE4.KnownDatatype).tag != 2)goto _LLCB;_tmpE8=(struct Cyc_Absyn_Datatypedecl**)(
_tmpE4.KnownDatatype).val;_tmpE9=*_tmpE8;_tmpEA=*_tmpE9;_tmpEB=_tmpEA.name;
_tmpEC=_tmpEA.is_extensible;_LLCF: if(_tmpEC){const char*_tmp406;Cyc_Absyndump_dump(((
_tmp406="@extensible ",_tag_dyneither(_tmp406,sizeof(char),13))));}{const char*
_tmp407;Cyc_Absyndump_dump(((_tmp407="datatype ",_tag_dyneither(_tmp407,sizeof(
char),10))));}Cyc_Absyndump_dumpqvar(_tmpEB);Cyc_Absyndump_dumptps(_tmp79);goto
_LLCB;_LLCB:;}goto _LL72;_LL83: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp7A=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp65;if(_tmp7A->tag != 4)goto _LL85;
else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.field_info;_tmp7D=_tmp7B.targs;}}_LL84:{
union Cyc_Absyn_DatatypeFieldInfoU _tmpEF=_tmp7C;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpF0;struct _tuple0*_tmpF1;struct _tuple0*_tmpF2;int _tmpF3;struct _tuple1 _tmpF4;
struct Cyc_Absyn_Datatypedecl*_tmpF5;struct Cyc_Absyn_Datatypedecl _tmpF6;struct
_tuple0*_tmpF7;int _tmpF8;struct Cyc_Absyn_Datatypefield*_tmpF9;struct Cyc_Absyn_Datatypefield
_tmpFA;struct _tuple0*_tmpFB;_LLD1: if((_tmpEF.UnknownDatatypefield).tag != 1)goto
_LLD3;_tmpF0=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpEF.UnknownDatatypefield).val;
_tmpF1=_tmpF0.datatype_name;_tmpF2=_tmpF0.field_name;_tmpF3=_tmpF0.is_extensible;
_LLD2: _tmpF7=_tmpF1;_tmpF8=_tmpF3;_tmpFB=_tmpF2;goto _LLD4;_LLD3: if((_tmpEF.KnownDatatypefield).tag
!= 2)goto _LLD0;_tmpF4=(struct _tuple1)(_tmpEF.KnownDatatypefield).val;_tmpF5=
_tmpF4.f1;_tmpF6=*_tmpF5;_tmpF7=_tmpF6.name;_tmpF8=_tmpF6.is_extensible;_tmpF9=
_tmpF4.f2;_tmpFA=*_tmpF9;_tmpFB=_tmpFA.name;_LLD4: if(_tmpF8){const char*_tmp408;
Cyc_Absyndump_dump(((_tmp408="@extensible ",_tag_dyneither(_tmp408,sizeof(char),
13))));}{const char*_tmp409;Cyc_Absyndump_dump(((_tmp409="datatype ",
_tag_dyneither(_tmp409,sizeof(char),10))));}Cyc_Absyndump_dumpqvar(_tmpF7);{
const char*_tmp40A;Cyc_Absyndump_dump(((_tmp40A=".",_tag_dyneither(_tmp40A,
sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmpFB);Cyc_Absyndump_dumptps(_tmp7D);
goto _LLD0;_LLD0:;}goto _LL72;_LL85: {struct Cyc_Absyn_IntType_struct*_tmp7E=(
struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp7E->tag != 6)goto _LL87;else{_tmp7F=
_tmp7E->f1;if(_tmp7F != Cyc_Absyn_None)goto _LL87;_tmp80=_tmp7E->f2;if(_tmp80 != 
Cyc_Absyn_Int_sz)goto _LL87;}}_LL86: goto _LL88;_LL87: {struct Cyc_Absyn_IntType_struct*
_tmp81=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp81->tag != 6)goto _LL89;
else{_tmp82=_tmp81->f1;if(_tmp82 != Cyc_Absyn_Signed)goto _LL89;_tmp83=_tmp81->f2;
if(_tmp83 != Cyc_Absyn_Int_sz)goto _LL89;}}_LL88:{const char*_tmp40B;Cyc_Absyndump_dump(((
_tmp40B="int",_tag_dyneither(_tmp40B,sizeof(char),4))));}return;_LL89: {struct
Cyc_Absyn_IntType_struct*_tmp84=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(
_tmp84->tag != 6)goto _LL8B;else{_tmp85=_tmp84->f1;if(_tmp85 != Cyc_Absyn_None)goto
_LL8B;_tmp86=_tmp84->f2;if(_tmp86 != Cyc_Absyn_Long_sz)goto _LL8B;}}_LL8A: goto
_LL8C;_LL8B: {struct Cyc_Absyn_IntType_struct*_tmp87=(struct Cyc_Absyn_IntType_struct*)
_tmp65;if(_tmp87->tag != 6)goto _LL8D;else{_tmp88=_tmp87->f1;if(_tmp88 != Cyc_Absyn_Signed)
goto _LL8D;_tmp89=_tmp87->f2;if(_tmp89 != Cyc_Absyn_Long_sz)goto _LL8D;}}_LL8C:{
const char*_tmp40C;Cyc_Absyndump_dump(((_tmp40C="long",_tag_dyneither(_tmp40C,
sizeof(char),5))));}return;_LL8D: {struct Cyc_Absyn_IntType_struct*_tmp8A=(struct
Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp8A->tag != 6)goto _LL8F;else{_tmp8B=_tmp8A->f1;
if(_tmp8B != Cyc_Absyn_None)goto _LL8F;_tmp8C=_tmp8A->f2;if(_tmp8C != Cyc_Absyn_Char_sz)
goto _LL8F;}}_LL8E:{const char*_tmp40D;Cyc_Absyndump_dump(((_tmp40D="char",
_tag_dyneither(_tmp40D,sizeof(char),5))));}return;_LL8F: {struct Cyc_Absyn_IntType_struct*
_tmp8D=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp8D->tag != 6)goto _LL91;
else{_tmp8E=_tmp8D->f1;if(_tmp8E != Cyc_Absyn_Signed)goto _LL91;_tmp8F=_tmp8D->f2;
if(_tmp8F != Cyc_Absyn_Char_sz)goto _LL91;}}_LL90:{const char*_tmp40E;Cyc_Absyndump_dump(((
_tmp40E="signed char",_tag_dyneither(_tmp40E,sizeof(char),12))));}return;_LL91: {
struct Cyc_Absyn_IntType_struct*_tmp90=(struct Cyc_Absyn_IntType_struct*)_tmp65;
if(_tmp90->tag != 6)goto _LL93;else{_tmp91=_tmp90->f1;if(_tmp91 != Cyc_Absyn_Unsigned)
goto _LL93;_tmp92=_tmp90->f2;if(_tmp92 != Cyc_Absyn_Char_sz)goto _LL93;}}_LL92:{
const char*_tmp40F;Cyc_Absyndump_dump(((_tmp40F="unsigned char",_tag_dyneither(
_tmp40F,sizeof(char),14))));}return;_LL93: {struct Cyc_Absyn_IntType_struct*
_tmp93=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp93->tag != 6)goto _LL95;
else{_tmp94=_tmp93->f1;if(_tmp94 != Cyc_Absyn_None)goto _LL95;_tmp95=_tmp93->f2;
if(_tmp95 != Cyc_Absyn_Short_sz)goto _LL95;}}_LL94: goto _LL96;_LL95: {struct Cyc_Absyn_IntType_struct*
_tmp96=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp96->tag != 6)goto _LL97;
else{_tmp97=_tmp96->f1;if(_tmp97 != Cyc_Absyn_Signed)goto _LL97;_tmp98=_tmp96->f2;
if(_tmp98 != Cyc_Absyn_Short_sz)goto _LL97;}}_LL96:{const char*_tmp410;Cyc_Absyndump_dump(((
_tmp410="short",_tag_dyneither(_tmp410,sizeof(char),6))));}return;_LL97: {struct
Cyc_Absyn_IntType_struct*_tmp99=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(
_tmp99->tag != 6)goto _LL99;else{_tmp9A=_tmp99->f1;if(_tmp9A != Cyc_Absyn_Unsigned)
goto _LL99;_tmp9B=_tmp99->f2;if(_tmp9B != Cyc_Absyn_Short_sz)goto _LL99;}}_LL98:{
const char*_tmp411;Cyc_Absyndump_dump(((_tmp411="unsigned short",_tag_dyneither(
_tmp411,sizeof(char),15))));}return;_LL99: {struct Cyc_Absyn_IntType_struct*
_tmp9C=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp9C->tag != 6)goto _LL9B;
else{_tmp9D=_tmp9C->f1;if(_tmp9D != Cyc_Absyn_Unsigned)goto _LL9B;_tmp9E=_tmp9C->f2;
if(_tmp9E != Cyc_Absyn_Int_sz)goto _LL9B;}}_LL9A:{const char*_tmp412;Cyc_Absyndump_dump(((
_tmp412="unsigned int",_tag_dyneither(_tmp412,sizeof(char),13))));}return;_LL9B: {
struct Cyc_Absyn_IntType_struct*_tmp9F=(struct Cyc_Absyn_IntType_struct*)_tmp65;
if(_tmp9F->tag != 6)goto _LL9D;else{_tmpA0=_tmp9F->f1;if(_tmpA0 != Cyc_Absyn_Unsigned)
goto _LL9D;_tmpA1=_tmp9F->f2;if(_tmpA1 != Cyc_Absyn_Long_sz)goto _LL9D;}}_LL9C:{
const char*_tmp413;Cyc_Absyndump_dump(((_tmp413="unsigned long",_tag_dyneither(
_tmp413,sizeof(char),14))));}return;_LL9D: {struct Cyc_Absyn_IntType_struct*
_tmpA2=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmpA2->tag != 6)goto _LL9F;
else{_tmpA3=_tmpA2->f1;if(_tmpA3 != Cyc_Absyn_None)goto _LL9F;_tmpA4=_tmpA2->f2;
if(_tmpA4 != Cyc_Absyn_LongLong_sz)goto _LL9F;}}_LL9E: goto _LLA0;_LL9F: {struct Cyc_Absyn_IntType_struct*
_tmpA5=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmpA5->tag != 6)goto _LLA1;
else{_tmpA6=_tmpA5->f1;if(_tmpA6 != Cyc_Absyn_Signed)goto _LLA1;_tmpA7=_tmpA5->f2;
if(_tmpA7 != Cyc_Absyn_LongLong_sz)goto _LLA1;}}_LLA0: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLD5:{const char*_tmp414;Cyc_Absyndump_dump(((_tmp414="long long",
_tag_dyneither(_tmp414,sizeof(char),10))));}return;case Cyc_Cyclone_Vc_c: _LLD6:{
const char*_tmp415;Cyc_Absyndump_dump(((_tmp415="__int64",_tag_dyneither(_tmp415,
sizeof(char),8))));}return;}_LLA1: {struct Cyc_Absyn_IntType_struct*_tmpA8=(
struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmpA8->tag != 6)goto _LLA3;else{_tmpA9=
_tmpA8->f1;if(_tmpA9 != Cyc_Absyn_Unsigned)goto _LLA3;_tmpAA=_tmpA8->f2;if(_tmpAA
!= Cyc_Absyn_LongLong_sz)goto _LLA3;}}_LLA2: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Vc_c: _LLD8:{const char*_tmp416;Cyc_Absyndump_dump(((_tmp416="unsigned __int64",
_tag_dyneither(_tmp416,sizeof(char),17))));}return;case Cyc_Cyclone_Gcc_c: _LLD9:{
const char*_tmp417;Cyc_Absyndump_dump(((_tmp417="unsigned long long",
_tag_dyneither(_tmp417,sizeof(char),19))));}return;}_LLA3: {struct Cyc_Absyn_FloatType_struct*
_tmpAB=(struct Cyc_Absyn_FloatType_struct*)_tmp65;if(_tmpAB->tag != 7)goto _LLA5;
else{_tmpAC=_tmpAB->f1;}}_LLA4: if(_tmpAC == 0){const char*_tmp418;Cyc_Absyndump_dump(((
_tmp418="float",_tag_dyneither(_tmp418,sizeof(char),6))));}else{if(_tmpAC == 1){
const char*_tmp419;Cyc_Absyndump_dump(((_tmp419="double",_tag_dyneither(_tmp419,
sizeof(char),7))));}else{const char*_tmp41A;Cyc_Absyndump_dump(((_tmp41A="long double",
_tag_dyneither(_tmp41A,sizeof(char),12))));}}return;_LLA5: {struct Cyc_Absyn_TupleType_struct*
_tmpAD=(struct Cyc_Absyn_TupleType_struct*)_tmp65;if(_tmpAD->tag != 10)goto _LLA7;
else{_tmpAE=_tmpAD->f1;}}_LLA6: Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(
_tmpAE);return;_LLA7: {struct Cyc_Absyn_AggrType_struct*_tmpAF=(struct Cyc_Absyn_AggrType_struct*)
_tmp65;if(_tmpAF->tag != 11)goto _LLA9;else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpB0.aggr_info;
_tmpB2=_tmpB0.targs;}}_LLA8: {enum Cyc_Absyn_AggrKind _tmp110;struct _tuple0*
_tmp111;struct _tuple10 _tmp10F=Cyc_Absyn_aggr_kinded_name(_tmpB1);_tmp110=_tmp10F.f1;
_tmp111=_tmp10F.f2;Cyc_Absyndump_dumpaggr_kind(_tmp110);Cyc_Absyndump_dumpqvar(
_tmp111);Cyc_Absyndump_dumptps(_tmpB2);return;}_LLA9: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB3=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp65;if(_tmpB3->tag != 12)goto
_LLAB;else{_tmpB4=_tmpB3->f1;_tmpB5=_tmpB3->f2;}}_LLAA: Cyc_Absyndump_dumpaggr_kind(
_tmpB4);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmpB5);
Cyc_Absyndump_dump_char((int)'}');return;_LLAB: {struct Cyc_Absyn_EnumType_struct*
_tmpB6=(struct Cyc_Absyn_EnumType_struct*)_tmp65;if(_tmpB6->tag != 13)goto _LLAD;
else{_tmpB7=_tmpB6->f1;}}_LLAC:{const char*_tmp41B;Cyc_Absyndump_dump(((_tmp41B="enum ",
_tag_dyneither(_tmp41B,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmpB7);
return;_LLAD: {struct Cyc_Absyn_AnonEnumType_struct*_tmpB8=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp65;if(_tmpB8->tag != 14)goto _LLAF;else{_tmpB9=_tmpB8->f1;}}_LLAE:{const char*
_tmp41C;Cyc_Absyndump_dump(((_tmp41C="enum {",_tag_dyneither(_tmp41C,sizeof(char),
7))));}Cyc_Absyndump_dumpenumfields(_tmpB9);{const char*_tmp41D;Cyc_Absyndump_dump(((
_tmp41D="}",_tag_dyneither(_tmp41D,sizeof(char),2))));}return;_LLAF: {struct Cyc_Absyn_TypedefType_struct*
_tmpBA=(struct Cyc_Absyn_TypedefType_struct*)_tmp65;if(_tmpBA->tag != 17)goto _LLB1;
else{_tmpBB=_tmpBA->f1;_tmpBC=_tmpBA->f2;}}_LLB0:(Cyc_Absyndump_dumpqvar(_tmpBB),
Cyc_Absyndump_dumptps(_tmpBC));return;_LLB1: {struct Cyc_Absyn_ValueofType_struct*
_tmpBD=(struct Cyc_Absyn_ValueofType_struct*)_tmp65;if(_tmpBD->tag != 18)goto _LLB3;
else{_tmpBE=_tmpBD->f1;}}_LLB2:{const char*_tmp41E;Cyc_Absyndump_dump(((_tmp41E="valueof_t(",
_tag_dyneither(_tmp41E,sizeof(char),11))));}Cyc_Absyndump_dumpexp(_tmpBE);{const
char*_tmp41F;Cyc_Absyndump_dump(((_tmp41F=")",_tag_dyneither(_tmp41F,sizeof(char),
2))));}return;_LLB3: {struct Cyc_Absyn_RgnHandleType_struct*_tmpBF=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp65;if(_tmpBF->tag != 15)goto _LLB5;else{_tmpC0=(void*)_tmpBF->f1;}}_LLB4:{
const char*_tmp420;Cyc_Absyndump_dump(((_tmp420="region_t<",_tag_dyneither(
_tmp420,sizeof(char),10))));}Cyc_Absyndump_dumprgn(_tmpC0);{const char*_tmp421;
Cyc_Absyndump_dump(((_tmp421=">",_tag_dyneither(_tmp421,sizeof(char),2))));}
return;_LLB5: {struct Cyc_Absyn_DynRgnType_struct*_tmpC1=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp65;if(_tmpC1->tag != 16)goto _LLB7;else{_tmpC2=(void*)_tmpC1->f1;_tmpC3=(void*)
_tmpC1->f2;}}_LLB6:{const char*_tmp422;Cyc_Absyndump_dump(((_tmp422="dynregion_t<",
_tag_dyneither(_tmp422,sizeof(char),13))));}Cyc_Absyndump_dumprgn(_tmpC2);{const
char*_tmp423;Cyc_Absyndump_dump(((_tmp423=",",_tag_dyneither(_tmp423,sizeof(char),
2))));}Cyc_Absyndump_dumprgn(_tmpC3);{const char*_tmp424;Cyc_Absyndump_dump(((
_tmp424=">",_tag_dyneither(_tmp424,sizeof(char),2))));}return;_LLB7: {struct Cyc_Absyn_TagType_struct*
_tmpC4=(struct Cyc_Absyn_TagType_struct*)_tmp65;if(_tmpC4->tag != 19)goto _LLB9;
else{_tmpC5=(void*)_tmpC4->f1;}}_LLB8:{const char*_tmp425;Cyc_Absyndump_dump(((
_tmp425="tag_t<",_tag_dyneither(_tmp425,sizeof(char),7))));}Cyc_Absyndump_dumpntyp(
_tmpC5);{const char*_tmp426;Cyc_Absyndump_dump(((_tmp426=">",_tag_dyneither(
_tmp426,sizeof(char),2))));}return;_LLB9: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC6=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp65;if(_tmpC6->tag != 21)goto _LLBB;}
_LLBA:{const char*_tmp427;Cyc_Absyndump_dump(((_tmp427="`U",_tag_dyneither(
_tmp427,sizeof(char),3))));}goto _LL72;_LLBB: {struct Cyc_Absyn_RefCntRgn_struct*
_tmpC7=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp65;if(_tmpC7->tag != 22)goto _LLBD;}
_LLBC:{const char*_tmp428;Cyc_Absyndump_dump(((_tmp428="`RC",_tag_dyneither(
_tmp428,sizeof(char),4))));}goto _LL72;_LLBD: {struct Cyc_Absyn_TypeDeclType_struct*
_tmpC8=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp65;if(_tmpC8->tag != 26)goto
_LLBF;else{_tmpC9=_tmpC8->f1;_tmpCA=*_tmpC9;_tmpCB=_tmpCA.r;{struct Cyc_Absyn_Aggr_td_struct*
_tmpCC=(struct Cyc_Absyn_Aggr_td_struct*)_tmpCB;if(_tmpCC->tag != 0)goto _LLBF;
else{_tmpCD=_tmpCC->f1;}};}}_LLBE: Cyc_Absyndump_dump_aggrdecl(_tmpCD);return;
_LLBF: {struct Cyc_Absyn_TypeDeclType_struct*_tmpCE=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp65;if(_tmpCE->tag != 26)goto _LLC1;else{_tmpCF=_tmpCE->f1;_tmpD0=*_tmpCF;
_tmpD1=_tmpD0.r;{struct Cyc_Absyn_Enum_td_struct*_tmpD2=(struct Cyc_Absyn_Enum_td_struct*)
_tmpD1;if(_tmpD2->tag != 1)goto _LLC1;else{_tmpD3=_tmpD2->f1;}};}}_LLC0: Cyc_Absyndump_dump_enumdecl(
_tmpD3);return;_LLC1: {struct Cyc_Absyn_TypeDeclType_struct*_tmpD4=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp65;if(_tmpD4->tag != 26)goto _LLC3;else{_tmpD5=_tmpD4->f1;_tmpD6=*_tmpD5;
_tmpD7=_tmpD6.r;{struct Cyc_Absyn_Datatype_td_struct*_tmpD8=(struct Cyc_Absyn_Datatype_td_struct*)
_tmpD7;if(_tmpD8->tag != 2)goto _LLC3;else{_tmpD9=_tmpD8->f1;}};}}_LLC2: Cyc_Absyndump_dump_datatypedecl(
_tmpD9);return;_LLC3: {struct Cyc_Absyn_HeapRgn_struct*_tmpDA=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp65;if(_tmpDA->tag != 20)goto _LLC5;}_LLC4: goto _LLC6;_LLC5: {struct Cyc_Absyn_AccessEff_struct*
_tmpDB=(struct Cyc_Absyn_AccessEff_struct*)_tmp65;if(_tmpDB->tag != 23)goto _LLC7;}
_LLC6: goto _LLC8;_LLC7: {struct Cyc_Absyn_RgnsEff_struct*_tmpDC=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp65;if(_tmpDC->tag != 25)goto _LLC9;}_LLC8: goto _LLCA;_LLC9: {struct Cyc_Absyn_JoinEff_struct*
_tmpDD=(struct Cyc_Absyn_JoinEff_struct*)_tmp65;if(_tmpDD->tag != 24)goto _LL72;}
_LLCA: return;_LL72:;}void Cyc_Absyndump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo != 
0)Cyc_Absyndump_dump_str((struct _dyneither_ptr*)vo->v);}void Cyc_Absyndump_dumpfunarg(
struct _tuple8*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),
struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*
t).f1);}struct _tuple16{void*f1;void*f2;};void Cyc_Absyndump_dump_rgncmp(struct
_tuple16*cmp){struct _tuple16 _tmp121;void*_tmp122;void*_tmp123;struct _tuple16*
_tmp120=cmp;_tmp121=*_tmp120;_tmp122=_tmp121.f1;_tmp123=_tmp121.f2;Cyc_Absyndump_dumpeff(
_tmp122);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmp123);}void
Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){const char*_tmp429;((void(*)(
void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dump_rgncmp,rgn_po,((_tmp429=",",_tag_dyneither(_tmp429,sizeof(
char),2))));}void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,
struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,struct Cyc_List_List*
rgn_po){Cyc_Absyndump_dump_char((int)'(');for(0;args != 0;args=args->tl){Cyc_Absyndump_dumpfunarg((
struct _tuple8*)args->hd);if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((
int)',');}if(c_varargs){const char*_tmp42A;Cyc_Absyndump_dump(((_tmp42A="...",
_tag_dyneither(_tmp42A,sizeof(char),4))));}else{if(cyc_varargs != 0){struct
_tuple8*_tmp42B;struct _tuple8*_tmp126=(_tmp42B=_cycalloc(sizeof(*_tmp42B)),((
_tmp42B->f1=cyc_varargs->name,((_tmp42B->f2=cyc_varargs->tq,((_tmp42B->f3=
cyc_varargs->type,_tmp42B)))))));{const char*_tmp42C;Cyc_Absyndump_dump(((_tmp42C="...",
_tag_dyneither(_tmp42C,sizeof(char),4))));}if(cyc_varargs->inject){const char*
_tmp42D;Cyc_Absyndump_dump(((_tmp42D=" inject ",_tag_dyneither(_tmp42D,sizeof(
char),9))));}Cyc_Absyndump_dumpfunarg(_tmp126);}}if(effopt != 0){Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff((void*)effopt->v);}if(rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}Cyc_Absyndump_dump_char((int)')');}
void Cyc_Absyndump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(int x))Cyc_Absyndump_ignore,
0);}void Cyc_Absyndump_dumpdesignator(void*d){void*_tmp12A=d;struct Cyc_Absyn_Exp*
_tmp12C;struct _dyneither_ptr*_tmp12E;_LLDC: {struct Cyc_Absyn_ArrayElement_struct*
_tmp12B=(struct Cyc_Absyn_ArrayElement_struct*)_tmp12A;if(_tmp12B->tag != 0)goto
_LLDE;else{_tmp12C=_tmp12B->f1;}}_LLDD:{const char*_tmp42E;Cyc_Absyndump_dump(((
_tmp42E=".[",_tag_dyneither(_tmp42E,sizeof(char),3))));}Cyc_Absyndump_dumpexp(
_tmp12C);Cyc_Absyndump_dump_char((int)']');goto _LLDB;_LLDE: {struct Cyc_Absyn_FieldName_struct*
_tmp12D=(struct Cyc_Absyn_FieldName_struct*)_tmp12A;if(_tmp12D->tag != 1)goto _LLDB;
else{_tmp12E=_tmp12D->f1;}}_LLDF: Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp12E);goto _LLDB;_LLDB:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};void Cyc_Absyndump_dumpde(struct _tuple17*de){{const char*_tmp431;const char*
_tmp430;const char*_tmp42F;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(*de).f1,((_tmp42F="",_tag_dyneither(_tmp42F,
sizeof(char),1))),((_tmp430="=",_tag_dyneither(_tmp430,sizeof(char),2))),((
_tmp431="=",_tag_dyneither(_tmp431,sizeof(char),2))));}Cyc_Absyndump_dumpexp((*
de).f2);}void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){const
char*_tmp434;const char*_tmp433;const char*_tmp432;((void(*)(void(*f)(int,struct
Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,
inprec,es,((_tmp432="",_tag_dyneither(_tmp432,sizeof(char),1))),((_tmp433="",
_tag_dyneither(_tmp433,sizeof(char),1))),((_tmp434=",",_tag_dyneither(_tmp434,
sizeof(char),2))));}void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*
e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= myprec){const char*_tmp435;Cyc_Absyndump_dump_nospace(((
_tmp435="(",_tag_dyneither(_tmp435,sizeof(char),2))));}{void*_tmp137=e->r;union
Cyc_Absyn_Cnst _tmp139;struct _tuple3 _tmp13A;enum Cyc_Absyn_Sign _tmp13B;char
_tmp13C;union Cyc_Absyn_Cnst _tmp13E;struct _dyneither_ptr _tmp13F;union Cyc_Absyn_Cnst
_tmp141;struct _tuple4 _tmp142;enum Cyc_Absyn_Sign _tmp143;short _tmp144;union Cyc_Absyn_Cnst
_tmp146;struct _tuple5 _tmp147;enum Cyc_Absyn_Sign _tmp148;int _tmp149;union Cyc_Absyn_Cnst
_tmp14B;struct _tuple5 _tmp14C;enum Cyc_Absyn_Sign _tmp14D;int _tmp14E;union Cyc_Absyn_Cnst
_tmp150;struct _tuple5 _tmp151;enum Cyc_Absyn_Sign _tmp152;int _tmp153;union Cyc_Absyn_Cnst
_tmp155;struct _tuple6 _tmp156;enum Cyc_Absyn_Sign _tmp157;long long _tmp158;union
Cyc_Absyn_Cnst _tmp15A;struct _tuple7 _tmp15B;struct _dyneither_ptr _tmp15C;union Cyc_Absyn_Cnst
_tmp15E;int _tmp15F;union Cyc_Absyn_Cnst _tmp161;struct _dyneither_ptr _tmp162;union
Cyc_Absyn_Cnst _tmp164;struct _dyneither_ptr _tmp165;struct _tuple0*_tmp167;struct
_tuple0*_tmp169;enum Cyc_Absyn_Primop _tmp16B;struct Cyc_List_List*_tmp16C;struct
Cyc_Absyn_Exp*_tmp16E;struct Cyc_Core_Opt*_tmp16F;struct Cyc_Absyn_Exp*_tmp170;
struct Cyc_Absyn_Exp*_tmp172;enum Cyc_Absyn_Incrementor _tmp173;struct Cyc_Absyn_Exp*
_tmp175;enum Cyc_Absyn_Incrementor _tmp176;struct Cyc_Absyn_Exp*_tmp178;enum Cyc_Absyn_Incrementor
_tmp179;struct Cyc_Absyn_Exp*_tmp17B;enum Cyc_Absyn_Incrementor _tmp17C;struct Cyc_Absyn_Exp*
_tmp17E;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*
_tmp182;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp185;struct Cyc_Absyn_Exp*
_tmp186;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Exp*
_tmp18B;struct Cyc_List_List*_tmp18C;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_List_List*
_tmp18F;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*
_tmp195;void*_tmp197;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Exp*_tmp19A;
struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Exp*_tmp19D;void*_tmp19F;struct Cyc_Absyn_Exp*
_tmp1A1;void*_tmp1A3;int _tmp1A5;struct _dyneither_ptr _tmp1A6;struct Cyc_Absyn_Exp*
_tmp1A8;struct _dyneither_ptr*_tmp1A9;void*_tmp1AB;void*_tmp1AC;struct
_dyneither_ptr*_tmp1AE;void*_tmp1B0;void*_tmp1B1;unsigned int _tmp1B3;struct Cyc_Absyn_Exp*
_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B7;struct _dyneither_ptr*_tmp1B8;struct Cyc_Absyn_Exp*
_tmp1BA;struct _dyneither_ptr*_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Exp*
_tmp1BE;struct Cyc_List_List*_tmp1C0;struct _tuple8*_tmp1C2;struct Cyc_List_List*
_tmp1C3;struct Cyc_List_List*_tmp1C5;struct Cyc_Absyn_Vardecl*_tmp1C7;struct Cyc_Absyn_Exp*
_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C9;struct _tuple0*_tmp1CB;struct Cyc_List_List*
_tmp1CC;struct Cyc_List_List*_tmp1CD;struct Cyc_List_List*_tmp1CF;struct Cyc_List_List*
_tmp1D1;struct Cyc_Absyn_Datatypefield*_tmp1D2;struct _tuple0*_tmp1D4;struct
_tuple0*_tmp1D6;struct Cyc_Absyn_MallocInfo _tmp1D8;int _tmp1D9;struct Cyc_Absyn_Exp*
_tmp1DA;void**_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DE;
struct Cyc_Absyn_Exp*_tmp1DF;struct Cyc_Core_Opt*_tmp1E1;struct Cyc_List_List*
_tmp1E2;struct Cyc_Absyn_Stmt*_tmp1E4;_LLE1: {struct Cyc_Absyn_Const_e_struct*
_tmp138=(struct Cyc_Absyn_Const_e_struct*)_tmp137;if(_tmp138->tag != 0)goto _LLE3;
else{_tmp139=_tmp138->f1;if((_tmp139.Char_c).tag != 2)goto _LLE3;_tmp13A=(struct
_tuple3)(_tmp139.Char_c).val;_tmp13B=_tmp13A.f1;_tmp13C=_tmp13A.f2;}}_LLE2: Cyc_Absyndump_dump_char((
int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp13C));Cyc_Absyndump_dump_char((
int)'\'');goto _LLE0;_LLE3: {struct Cyc_Absyn_Const_e_struct*_tmp13D=(struct Cyc_Absyn_Const_e_struct*)
_tmp137;if(_tmp13D->tag != 0)goto _LLE5;else{_tmp13E=_tmp13D->f1;if((_tmp13E.Wchar_c).tag
!= 3)goto _LLE5;_tmp13F=(struct _dyneither_ptr)(_tmp13E.Wchar_c).val;}}_LLE4:{
const char*_tmp439;void*_tmp438[1];struct Cyc_String_pa_struct _tmp437;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp437.tag=0,((_tmp437.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp13F),((_tmp438[0]=& _tmp437,Cyc_aprintf(((_tmp439="L'%s'",
_tag_dyneither(_tmp439,sizeof(char),6))),_tag_dyneither(_tmp438,sizeof(void*),1)))))))));}
goto _LLE0;_LLE5: {struct Cyc_Absyn_Const_e_struct*_tmp140=(struct Cyc_Absyn_Const_e_struct*)
_tmp137;if(_tmp140->tag != 0)goto _LLE7;else{_tmp141=_tmp140->f1;if((_tmp141.Short_c).tag
!= 4)goto _LLE7;_tmp142=(struct _tuple4)(_tmp141.Short_c).val;_tmp143=_tmp142.f1;
_tmp144=_tmp142.f2;}}_LLE6:{const char*_tmp43D;void*_tmp43C[1];struct Cyc_Int_pa_struct
_tmp43B;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp43B.tag=1,((_tmp43B.f1=(
unsigned long)((int)_tmp144),((_tmp43C[0]=& _tmp43B,Cyc_aprintf(((_tmp43D="%d",
_tag_dyneither(_tmp43D,sizeof(char),3))),_tag_dyneither(_tmp43C,sizeof(void*),1)))))))));}
goto _LLE0;_LLE7: {struct Cyc_Absyn_Const_e_struct*_tmp145=(struct Cyc_Absyn_Const_e_struct*)
_tmp137;if(_tmp145->tag != 0)goto _LLE9;else{_tmp146=_tmp145->f1;if((_tmp146.Int_c).tag
!= 5)goto _LLE9;_tmp147=(struct _tuple5)(_tmp146.Int_c).val;_tmp148=_tmp147.f1;if(
_tmp148 != Cyc_Absyn_None)goto _LLE9;_tmp149=_tmp147.f2;}}_LLE8: _tmp14E=_tmp149;
goto _LLEA;_LLE9: {struct Cyc_Absyn_Const_e_struct*_tmp14A=(struct Cyc_Absyn_Const_e_struct*)
_tmp137;if(_tmp14A->tag != 0)goto _LLEB;else{_tmp14B=_tmp14A->f1;if((_tmp14B.Int_c).tag
!= 5)goto _LLEB;_tmp14C=(struct _tuple5)(_tmp14B.Int_c).val;_tmp14D=_tmp14C.f1;if(
_tmp14D != Cyc_Absyn_Signed)goto _LLEB;_tmp14E=_tmp14C.f2;}}_LLEA:{const char*
_tmp441;void*_tmp440[1];struct Cyc_Int_pa_struct _tmp43F;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp43F.tag=1,((_tmp43F.f1=(unsigned long)_tmp14E,((
_tmp440[0]=& _tmp43F,Cyc_aprintf(((_tmp441="%d",_tag_dyneither(_tmp441,sizeof(
char),3))),_tag_dyneither(_tmp440,sizeof(void*),1)))))))));}goto _LLE0;_LLEB: {
struct Cyc_Absyn_Const_e_struct*_tmp14F=(struct Cyc_Absyn_Const_e_struct*)_tmp137;
if(_tmp14F->tag != 0)goto _LLED;else{_tmp150=_tmp14F->f1;if((_tmp150.Int_c).tag != 
5)goto _LLED;_tmp151=(struct _tuple5)(_tmp150.Int_c).val;_tmp152=_tmp151.f1;if(
_tmp152 != Cyc_Absyn_Unsigned)goto _LLED;_tmp153=_tmp151.f2;}}_LLEC:{const char*
_tmp445;void*_tmp444[1];struct Cyc_Int_pa_struct _tmp443;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp443.tag=1,((_tmp443.f1=(unsigned int)_tmp153,((
_tmp444[0]=& _tmp443,Cyc_aprintf(((_tmp445="%u",_tag_dyneither(_tmp445,sizeof(
char),3))),_tag_dyneither(_tmp444,sizeof(void*),1)))))))));}goto _LLE0;_LLED: {
struct Cyc_Absyn_Const_e_struct*_tmp154=(struct Cyc_Absyn_Const_e_struct*)_tmp137;
if(_tmp154->tag != 0)goto _LLEF;else{_tmp155=_tmp154->f1;if((_tmp155.LongLong_c).tag
!= 6)goto _LLEF;_tmp156=(struct _tuple6)(_tmp155.LongLong_c).val;_tmp157=_tmp156.f1;
_tmp158=_tmp156.f2;}}_LLEE:{const char*_tmp446;Cyc_Absyndump_dump(((_tmp446="<<FIX LONG LONG CONSTANT>>",
_tag_dyneither(_tmp446,sizeof(char),27))));}goto _LLE0;_LLEF: {struct Cyc_Absyn_Const_e_struct*
_tmp159=(struct Cyc_Absyn_Const_e_struct*)_tmp137;if(_tmp159->tag != 0)goto _LLF1;
else{_tmp15A=_tmp159->f1;if((_tmp15A.Float_c).tag != 7)goto _LLF1;_tmp15B=(struct
_tuple7)(_tmp15A.Float_c).val;_tmp15C=_tmp15B.f1;}}_LLF0: Cyc_Absyndump_dump(
_tmp15C);goto _LLE0;_LLF1: {struct Cyc_Absyn_Const_e_struct*_tmp15D=(struct Cyc_Absyn_Const_e_struct*)
_tmp137;if(_tmp15D->tag != 0)goto _LLF3;else{_tmp15E=_tmp15D->f1;if((_tmp15E.Null_c).tag
!= 1)goto _LLF3;_tmp15F=(int)(_tmp15E.Null_c).val;}}_LLF2:{const char*_tmp447;Cyc_Absyndump_dump(((
_tmp447="NULL",_tag_dyneither(_tmp447,sizeof(char),5))));}goto _LLE0;_LLF3: {
struct Cyc_Absyn_Const_e_struct*_tmp160=(struct Cyc_Absyn_Const_e_struct*)_tmp137;
if(_tmp160->tag != 0)goto _LLF5;else{_tmp161=_tmp160->f1;if((_tmp161.String_c).tag
!= 8)goto _LLF5;_tmp162=(struct _dyneither_ptr)(_tmp161.String_c).val;}}_LLF4: Cyc_Absyndump_dump_char((
int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp162));Cyc_Absyndump_dump_char((
int)'"');goto _LLE0;_LLF5: {struct Cyc_Absyn_Const_e_struct*_tmp163=(struct Cyc_Absyn_Const_e_struct*)
_tmp137;if(_tmp163->tag != 0)goto _LLF7;else{_tmp164=_tmp163->f1;if((_tmp164.Wstring_c).tag
!= 9)goto _LLF7;_tmp165=(struct _dyneither_ptr)(_tmp164.Wstring_c).val;}}_LLF6:{
const char*_tmp448;Cyc_Absyndump_dump(((_tmp448="L\"",_tag_dyneither(_tmp448,
sizeof(char),3))));}Cyc_Absyndump_dump_nospace(_tmp165);Cyc_Absyndump_dump_char((
int)'"');goto _LLE0;_LLF7: {struct Cyc_Absyn_UnknownId_e_struct*_tmp166=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp137;if(_tmp166->tag != 2)goto _LLF9;else{_tmp167=_tmp166->f1;}}_LLF8: _tmp169=
_tmp167;goto _LLFA;_LLF9: {struct Cyc_Absyn_Var_e_struct*_tmp168=(struct Cyc_Absyn_Var_e_struct*)
_tmp137;if(_tmp168->tag != 1)goto _LLFB;else{_tmp169=_tmp168->f1;}}_LLFA: Cyc_Absyndump_dumpqvar(
_tmp169);goto _LLE0;_LLFB: {struct Cyc_Absyn_Primop_e_struct*_tmp16A=(struct Cyc_Absyn_Primop_e_struct*)
_tmp137;if(_tmp16A->tag != 3)goto _LLFD;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16A->f2;}}
_LLFC: {struct _dyneither_ptr _tmp1F4=Cyc_Absynpp_prim2str(_tmp16B);switch(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp16C)){case 1: _LL14F: if(_tmp16B == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){{const char*_tmp449;Cyc_Absyndump_dump(((
_tmp449="numelts(",_tag_dyneither(_tmp449,sizeof(char),9))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp16C))->hd);{const
char*_tmp44A;Cyc_Absyndump_dump(((_tmp44A=")",_tag_dyneither(_tmp44A,sizeof(char),
2))));};}else{Cyc_Absyndump_dump(_tmp1F4);Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp16C))->hd);}break;
case 2: _LL150: Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp16C))->hd);Cyc_Absyndump_dump(_tmp1F4);Cyc_Absyndump_dump_char((
int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp16C->tl))->hd);break;default: _LL151: {struct Cyc_Core_Failure_struct
_tmp450;const char*_tmp44F;struct Cyc_Core_Failure_struct*_tmp44E;(int)_throw((
void*)((_tmp44E=_cycalloc(sizeof(*_tmp44E)),((_tmp44E[0]=((_tmp450.tag=Cyc_Core_Failure,((
_tmp450.f1=((_tmp44F="Absyndump -- Bad number of arguments to primop",
_tag_dyneither(_tmp44F,sizeof(char),47))),_tmp450)))),_tmp44E)))));}}goto _LLE0;}
_LLFD: {struct Cyc_Absyn_AssignOp_e_struct*_tmp16D=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp137;if(_tmp16D->tag != 4)goto _LLFF;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;
_tmp170=_tmp16D->f3;}}_LLFE: Cyc_Absyndump_dumpexp_prec(myprec,_tmp16E);if(
_tmp16F != 0)Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)
_tmp16F->v));{const char*_tmp451;Cyc_Absyndump_dump_nospace(((_tmp451="=",
_tag_dyneither(_tmp451,sizeof(char),2))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp170);goto _LLE0;_LLFF: {struct Cyc_Absyn_Increment_e_struct*_tmp171=(struct Cyc_Absyn_Increment_e_struct*)
_tmp137;if(_tmp171->tag != 5)goto _LL101;else{_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;
if(_tmp173 != Cyc_Absyn_PreInc)goto _LL101;}}_LL100:{const char*_tmp452;Cyc_Absyndump_dump(((
_tmp452="++",_tag_dyneither(_tmp452,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp172);goto _LLE0;_LL101: {struct Cyc_Absyn_Increment_e_struct*_tmp174=(
struct Cyc_Absyn_Increment_e_struct*)_tmp137;if(_tmp174->tag != 5)goto _LL103;else{
_tmp175=_tmp174->f1;_tmp176=_tmp174->f2;if(_tmp176 != Cyc_Absyn_PreDec)goto _LL103;}}
_LL102:{const char*_tmp453;Cyc_Absyndump_dump(((_tmp453="--",_tag_dyneither(
_tmp453,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp175);goto _LLE0;
_LL103: {struct Cyc_Absyn_Increment_e_struct*_tmp177=(struct Cyc_Absyn_Increment_e_struct*)
_tmp137;if(_tmp177->tag != 5)goto _LL105;else{_tmp178=_tmp177->f1;_tmp179=_tmp177->f2;
if(_tmp179 != Cyc_Absyn_PostInc)goto _LL105;}}_LL104: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp178);{const char*_tmp454;Cyc_Absyndump_dump(((_tmp454="++",
_tag_dyneither(_tmp454,sizeof(char),3))));}goto _LLE0;_LL105: {struct Cyc_Absyn_Increment_e_struct*
_tmp17A=(struct Cyc_Absyn_Increment_e_struct*)_tmp137;if(_tmp17A->tag != 5)goto
_LL107;else{_tmp17B=_tmp17A->f1;_tmp17C=_tmp17A->f2;if(_tmp17C != Cyc_Absyn_PostDec)
goto _LL107;}}_LL106: Cyc_Absyndump_dumpexp_prec(myprec,_tmp17B);{const char*
_tmp455;Cyc_Absyndump_dump(((_tmp455="--",_tag_dyneither(_tmp455,sizeof(char),3))));}
goto _LLE0;_LL107: {struct Cyc_Absyn_Conditional_e_struct*_tmp17D=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp137;if(_tmp17D->tag != 6)goto _LL109;else{_tmp17E=_tmp17D->f1;_tmp17F=_tmp17D->f2;
_tmp180=_tmp17D->f3;}}_LL108: Cyc_Absyndump_dumpexp_prec(myprec,_tmp17E);Cyc_Absyndump_dump_char((
int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp17F);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp180);goto _LLE0;_LL109: {struct Cyc_Absyn_And_e_struct*
_tmp181=(struct Cyc_Absyn_And_e_struct*)_tmp137;if(_tmp181->tag != 7)goto _LL10B;
else{_tmp182=_tmp181->f1;_tmp183=_tmp181->f2;}}_LL10A: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp182);{const char*_tmp456;Cyc_Absyndump_dump(((_tmp456=" && ",
_tag_dyneither(_tmp456,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp183);goto _LLE0;_LL10B: {struct Cyc_Absyn_Or_e_struct*_tmp184=(struct Cyc_Absyn_Or_e_struct*)
_tmp137;if(_tmp184->tag != 8)goto _LL10D;else{_tmp185=_tmp184->f1;_tmp186=_tmp184->f2;}}
_LL10C: Cyc_Absyndump_dumpexp_prec(myprec,_tmp185);{const char*_tmp457;Cyc_Absyndump_dump(((
_tmp457=" || ",_tag_dyneither(_tmp457,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp186);goto _LLE0;_LL10D: {struct Cyc_Absyn_SeqExp_e_struct*_tmp187=(
struct Cyc_Absyn_SeqExp_e_struct*)_tmp137;if(_tmp187->tag != 9)goto _LL10F;else{
_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;}}_LL10E: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp188);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp189);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL10F: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp18A=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp137;if(_tmp18A->tag != 10)goto
_LL111;else{_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;}}_LL110: _tmp18E=_tmp18B;
_tmp18F=_tmp18C;goto _LL112;_LL111: {struct Cyc_Absyn_FnCall_e_struct*_tmp18D=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp137;if(_tmp18D->tag != 11)goto _LL113;else{
_tmp18E=_tmp18D->f1;_tmp18F=_tmp18D->f2;}}_LL112: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp18E);{const char*_tmp458;Cyc_Absyndump_dump_nospace(((_tmp458="(",
_tag_dyneither(_tmp458,sizeof(char),2))));}Cyc_Absyndump_dumpexps_prec(20,
_tmp18F);{const char*_tmp459;Cyc_Absyndump_dump_nospace(((_tmp459=")",
_tag_dyneither(_tmp459,sizeof(char),2))));}goto _LLE0;_LL113: {struct Cyc_Absyn_Throw_e_struct*
_tmp190=(struct Cyc_Absyn_Throw_e_struct*)_tmp137;if(_tmp190->tag != 12)goto _LL115;
else{_tmp191=_tmp190->f1;}}_LL114:{const char*_tmp45A;Cyc_Absyndump_dump(((
_tmp45A="throw",_tag_dyneither(_tmp45A,sizeof(char),6))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp191);goto _LLE0;_LL115: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp192=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp137;if(_tmp192->tag != 13)
goto _LL117;else{_tmp193=_tmp192->f1;}}_LL116: _tmp195=_tmp193;goto _LL118;_LL117: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp194=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp137;if(_tmp194->tag != 14)goto _LL119;else{_tmp195=_tmp194->f1;}}_LL118: Cyc_Absyndump_dumpexp_prec(
inprec,_tmp195);goto _LLE0;_LL119: {struct Cyc_Absyn_Cast_e_struct*_tmp196=(struct
Cyc_Absyn_Cast_e_struct*)_tmp137;if(_tmp196->tag != 15)goto _LL11B;else{_tmp197=(
void*)_tmp196->f1;_tmp198=_tmp196->f2;}}_LL11A: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp(_tmp197);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp198);goto _LLE0;_LL11B: {struct Cyc_Absyn_Address_e_struct*_tmp199=(
struct Cyc_Absyn_Address_e_struct*)_tmp137;if(_tmp199->tag != 16)goto _LL11D;else{
_tmp19A=_tmp199->f1;}}_LL11C: Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp19A);goto _LLE0;_LL11D: {struct Cyc_Absyn_New_e_struct*_tmp19B=(struct
Cyc_Absyn_New_e_struct*)_tmp137;if(_tmp19B->tag != 17)goto _LL11F;else{_tmp19C=
_tmp19B->f1;_tmp19D=_tmp19B->f2;}}_LL11E:{const char*_tmp45B;Cyc_Absyndump_dump(((
_tmp45B="new ",_tag_dyneither(_tmp45B,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp19D);goto _LLE0;_LL11F: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp19E=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp137;if(_tmp19E->tag != 18)goto _LL121;
else{_tmp19F=(void*)_tmp19E->f1;}}_LL120:{const char*_tmp45C;Cyc_Absyndump_dump(((
_tmp45C="sizeof(",_tag_dyneither(_tmp45C,sizeof(char),8))));}Cyc_Absyndump_dumptyp(
_tmp19F);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL121: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp1A0=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp137;if(_tmp1A0->tag != 19)goto
_LL123;else{_tmp1A1=_tmp1A0->f1;}}_LL122:{const char*_tmp45D;Cyc_Absyndump_dump(((
_tmp45D="sizeof(",_tag_dyneither(_tmp45D,sizeof(char),8))));}Cyc_Absyndump_dumpexp(
_tmp1A1);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL123: {struct Cyc_Absyn_Valueof_e_struct*
_tmp1A2=(struct Cyc_Absyn_Valueof_e_struct*)_tmp137;if(_tmp1A2->tag != 39)goto
_LL125;else{_tmp1A3=(void*)_tmp1A2->f1;}}_LL124:{const char*_tmp45E;Cyc_Absyndump_dump(((
_tmp45E="valueof(",_tag_dyneither(_tmp45E,sizeof(char),9))));}Cyc_Absyndump_dumptyp(
_tmp1A3);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL125: {struct Cyc_Absyn_Asm_e_struct*
_tmp1A4=(struct Cyc_Absyn_Asm_e_struct*)_tmp137;if(_tmp1A4->tag != 40)goto _LL127;
else{_tmp1A5=_tmp1A4->f1;_tmp1A6=_tmp1A4->f2;}}_LL126:{const char*_tmp45F;Cyc_Absyndump_dump(((
_tmp45F="__asm__",_tag_dyneither(_tmp45F,sizeof(char),8))));}if(_tmp1A5){const
char*_tmp460;Cyc_Absyndump_dump(((_tmp460=" volatile ",_tag_dyneither(_tmp460,
sizeof(char),11))));}Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dump_nospace(
_tmp1A6);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL127: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1A7=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp137;if(_tmp1A7->tag != 38)goto
_LL129;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;}}_LL128:{const char*_tmp461;
Cyc_Absyndump_dump(((_tmp461="tagcheck(",_tag_dyneither(_tmp461,sizeof(char),10))));}
Cyc_Absyndump_dumpexp(_tmp1A8);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp1A9);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL129: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp1AA=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp137;if(_tmp1AA->tag != 20)goto
_LL12B;else{_tmp1AB=(void*)_tmp1AA->f1;_tmp1AC=(void*)_tmp1AA->f2;{struct Cyc_Absyn_StructField_struct*
_tmp1AD=(struct Cyc_Absyn_StructField_struct*)_tmp1AC;if(_tmp1AD->tag != 0)goto
_LL12B;else{_tmp1AE=_tmp1AD->f1;}};}}_LL12A:{const char*_tmp462;Cyc_Absyndump_dump(((
_tmp462="offsetof(",_tag_dyneither(_tmp462,sizeof(char),10))));}Cyc_Absyndump_dumptyp(
_tmp1AB);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmp1AE);
Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL12B: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp1AF=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp137;if(_tmp1AF->tag != 20)goto
_LL12D;else{_tmp1B0=(void*)_tmp1AF->f1;_tmp1B1=(void*)_tmp1AF->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp1B2=(struct Cyc_Absyn_TupleIndex_struct*)_tmp1B1;if(_tmp1B2->tag != 1)goto
_LL12D;else{_tmp1B3=_tmp1B2->f1;}};}}_LL12C:{const char*_tmp463;Cyc_Absyndump_dump(((
_tmp463="offsetof(",_tag_dyneither(_tmp463,sizeof(char),10))));}Cyc_Absyndump_dumptyp(
_tmp1B0);Cyc_Absyndump_dump_char((int)',');{const char*_tmp467;void*_tmp466[1];
struct Cyc_Int_pa_struct _tmp465;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp465.tag=1,((_tmp465.f1=(unsigned long)((int)_tmp1B3),((_tmp466[0]=& _tmp465,
Cyc_aprintf(((_tmp467="%d",_tag_dyneither(_tmp467,sizeof(char),3))),
_tag_dyneither(_tmp466,sizeof(void*),1)))))))));}Cyc_Absyndump_dump_char((int)')');
goto _LLE0;_LL12D: {struct Cyc_Absyn_Deref_e_struct*_tmp1B4=(struct Cyc_Absyn_Deref_e_struct*)
_tmp137;if(_tmp1B4->tag != 21)goto _LL12F;else{_tmp1B5=_tmp1B4->f1;}}_LL12E: Cyc_Absyndump_dump_char((
int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp1B5);goto _LLE0;_LL12F: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp1B6=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp137;if(_tmp1B6->tag != 22)goto
_LL131;else{_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;}}_LL130: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1B7);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp1B8);goto _LLE0;_LL131: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp1B9=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp137;if(_tmp1B9->tag != 23)goto _LL133;else{
_tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1B9->f2;}}_LL132: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1BA);{const char*_tmp468;Cyc_Absyndump_dump_nospace(((_tmp468="->",
_tag_dyneither(_tmp468,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*_tmp1BB);
goto _LLE0;_LL133: {struct Cyc_Absyn_Subscript_e_struct*_tmp1BC=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp137;if(_tmp1BC->tag != 24)goto _LL135;else{_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BC->f2;}}
_LL134: Cyc_Absyndump_dumpexp_prec(myprec,_tmp1BD);Cyc_Absyndump_dump_char((int)'[');
Cyc_Absyndump_dumpexp(_tmp1BE);Cyc_Absyndump_dump_char((int)']');goto _LLE0;
_LL135: {struct Cyc_Absyn_Tuple_e_struct*_tmp1BF=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp137;if(_tmp1BF->tag != 25)goto _LL137;else{_tmp1C0=_tmp1BF->f1;}}_LL136:{const
char*_tmp469;Cyc_Absyndump_dump(((_tmp469="$(",_tag_dyneither(_tmp469,sizeof(
char),3))));}Cyc_Absyndump_dumpexps_prec(20,_tmp1C0);Cyc_Absyndump_dump_char((
int)')');goto _LLE0;_LL137: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp1C1=(struct
Cyc_Absyn_CompoundLit_e_struct*)_tmp137;if(_tmp1C1->tag != 26)goto _LL139;else{
_tmp1C2=_tmp1C1->f1;_tmp1C3=_tmp1C1->f2;}}_LL138: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp1C2).f3);Cyc_Absyndump_dump_char((int)')');{const
char*_tmp46C;const char*_tmp46B;const char*_tmp46A;((void(*)(void(*f)(struct
_tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr
end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1C3,((
_tmp46A="{",_tag_dyneither(_tmp46A,sizeof(char),2))),((_tmp46B="}",
_tag_dyneither(_tmp46B,sizeof(char),2))),((_tmp46C=",",_tag_dyneither(_tmp46C,
sizeof(char),2))));}goto _LLE0;_LL139: {struct Cyc_Absyn_Array_e_struct*_tmp1C4=(
struct Cyc_Absyn_Array_e_struct*)_tmp137;if(_tmp1C4->tag != 27)goto _LL13B;else{
_tmp1C5=_tmp1C4->f1;}}_LL13A:{const char*_tmp46F;const char*_tmp46E;const char*
_tmp46D;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1C5,((_tmp46D="{",_tag_dyneither(_tmp46D,sizeof(char),2))),((
_tmp46E="}",_tag_dyneither(_tmp46E,sizeof(char),2))),((_tmp46F=",",
_tag_dyneither(_tmp46F,sizeof(char),2))));}goto _LLE0;_LL13B: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1C6=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp137;if(_tmp1C6->tag != 28)
goto _LL13D;else{_tmp1C7=_tmp1C6->f1;_tmp1C8=_tmp1C6->f2;_tmp1C9=_tmp1C6->f3;}}
_LL13C:{const char*_tmp470;Cyc_Absyndump_dump(((_tmp470="new {for",_tag_dyneither(
_tmp470,sizeof(char),9))));}Cyc_Absyndump_dump_str((*_tmp1C7->name).f2);Cyc_Absyndump_dump_char((
int)'<');Cyc_Absyndump_dumpexp(_tmp1C8);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(
_tmp1C9);Cyc_Absyndump_dump_char((int)'}');goto _LLE0;_LL13D: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1CA=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp137;if(_tmp1CA->tag != 29)goto
_LL13F;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=_tmp1CA->f2;_tmp1CD=_tmp1CA->f3;}}_LL13E:
Cyc_Absyndump_dumpqvar(_tmp1CB);Cyc_Absyndump_dump_char((int)'{');if(_tmp1CC != 0)
Cyc_Absyndump_dumptps(_tmp1CC);{const char*_tmp473;const char*_tmp472;const char*
_tmp471;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1CD,((_tmp471="",_tag_dyneither(_tmp471,sizeof(char),1))),((
_tmp472="}",_tag_dyneither(_tmp472,sizeof(char),2))),((_tmp473=",",
_tag_dyneither(_tmp473,sizeof(char),2))));}goto _LLE0;_LL13F: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1CE=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp137;if(_tmp1CE->tag != 30)goto
_LL141;else{_tmp1CF=_tmp1CE->f2;}}_LL140:{const char*_tmp476;const char*_tmp475;
const char*_tmp474;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,
struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1CF,((_tmp474="{",_tag_dyneither(_tmp474,sizeof(char),2))),((
_tmp475="}",_tag_dyneither(_tmp475,sizeof(char),2))),((_tmp476=",",
_tag_dyneither(_tmp476,sizeof(char),2))));}goto _LLE0;_LL141: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1D0=(struct Cyc_Absyn_Datatype_e_struct*)_tmp137;if(_tmp1D0->tag != 31)goto
_LL143;else{_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D0->f3;}}_LL142: Cyc_Absyndump_dumpqvar(
_tmp1D2->name);if(_tmp1D1 != 0){const char*_tmp479;const char*_tmp478;const char*
_tmp477;((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp,_tmp1D1,((_tmp477="(",_tag_dyneither(_tmp477,sizeof(char),
2))),((_tmp478=")",_tag_dyneither(_tmp478,sizeof(char),2))),((_tmp479=",",
_tag_dyneither(_tmp479,sizeof(char),2))));}goto _LLE0;_LL143: {struct Cyc_Absyn_Enum_e_struct*
_tmp1D3=(struct Cyc_Absyn_Enum_e_struct*)_tmp137;if(_tmp1D3->tag != 32)goto _LL145;
else{_tmp1D4=_tmp1D3->f1;}}_LL144: Cyc_Absyndump_dumpqvar(_tmp1D4);goto _LLE0;
_LL145: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp1D5=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp137;if(_tmp1D5->tag != 33)goto _LL147;else{_tmp1D6=_tmp1D5->f1;}}_LL146: Cyc_Absyndump_dumpqvar(
_tmp1D6);goto _LLE0;_LL147: {struct Cyc_Absyn_Malloc_e_struct*_tmp1D7=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp137;if(_tmp1D7->tag != 34)goto _LL149;else{_tmp1D8=_tmp1D7->f1;_tmp1D9=_tmp1D8.is_calloc;
_tmp1DA=_tmp1D8.rgn;_tmp1DB=_tmp1D8.elt_type;_tmp1DC=_tmp1D8.num_elts;}}_LL148:
if(_tmp1D9){if(_tmp1DA != 0){{const char*_tmp47A;Cyc_Absyndump_dump(((_tmp47A="rcalloc(",
_tag_dyneither(_tmp47A,sizeof(char),9))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_tmp1DA);{const char*_tmp47B;Cyc_Absyndump_dump(((_tmp47B=",",_tag_dyneither(
_tmp47B,sizeof(char),2))));};}else{const char*_tmp47C;Cyc_Absyndump_dump(((
_tmp47C="calloc",_tag_dyneither(_tmp47C,sizeof(char),7))));}Cyc_Absyndump_dumpexp(
_tmp1DC);{const char*_tmp47D;Cyc_Absyndump_dump(((_tmp47D=",",_tag_dyneither(
_tmp47D,sizeof(char),2))));}Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmp1DB)),0));{const char*_tmp47E;Cyc_Absyndump_dump(((_tmp47E=")",
_tag_dyneither(_tmp47E,sizeof(char),2))));};}else{if(_tmp1DA != 0){{const char*
_tmp47F;Cyc_Absyndump_dump(((_tmp47F="rmalloc(",_tag_dyneither(_tmp47F,sizeof(
char),9))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp1DA);{const char*
_tmp480;Cyc_Absyndump_dump(((_tmp480=",",_tag_dyneither(_tmp480,sizeof(char),2))));};}
else{const char*_tmp481;Cyc_Absyndump_dump(((_tmp481="malloc(",_tag_dyneither(
_tmp481,sizeof(char),8))));}if(_tmp1DB != 0)Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp1DB,0),_tmp1DC,0));else{Cyc_Absyndump_dumpexp(
_tmp1DC);}{const char*_tmp482;Cyc_Absyndump_dump(((_tmp482=")",_tag_dyneither(
_tmp482,sizeof(char),2))));};}goto _LLE0;_LL149: {struct Cyc_Absyn_Swap_e_struct*
_tmp1DD=(struct Cyc_Absyn_Swap_e_struct*)_tmp137;if(_tmp1DD->tag != 35)goto _LL14B;
else{_tmp1DE=_tmp1DD->f1;_tmp1DF=_tmp1DD->f2;}}_LL14A: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1DE);{const char*_tmp483;Cyc_Absyndump_dump_nospace(((_tmp483=":=:",
_tag_dyneither(_tmp483,sizeof(char),4))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp1DF);goto _LLE0;_LL14B: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp1E0=(
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp137;if(_tmp1E0->tag != 36)goto _LL14D;
else{_tmp1E1=_tmp1E0->f1;_tmp1E2=_tmp1E0->f2;}}_LL14C:{const char*_tmp486;const
char*_tmp485;const char*_tmp484;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1E2,((_tmp484="{",_tag_dyneither(_tmp484,sizeof(char),2))),((
_tmp485="}",_tag_dyneither(_tmp485,sizeof(char),2))),((_tmp486=",",
_tag_dyneither(_tmp486,sizeof(char),2))));}goto _LLE0;_LL14D: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp1E3=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp137;if(_tmp1E3->tag != 37)goto
_LLE0;else{_tmp1E4=_tmp1E3->f1;}}_LL14E:{const char*_tmp487;Cyc_Absyndump_dump_nospace(((
_tmp487="({",_tag_dyneither(_tmp487,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(
_tmp1E4,1);{const char*_tmp488;Cyc_Absyndump_dump_nospace(((_tmp488="})",
_tag_dyneither(_tmp488,sizeof(char),3))));}goto _LLE0;_LLE0:;}if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*
e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(struct
Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmp231=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmp231->where_clause == 0  && (
_tmp231->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){const char*_tmp489;Cyc_Absyndump_dump(((
_tmp489="default:",_tag_dyneither(_tmp489,sizeof(char),9))));}else{{const char*
_tmp48A;Cyc_Absyndump_dump(((_tmp48A="case",_tag_dyneither(_tmp48A,sizeof(char),
5))));}Cyc_Absyndump_dumppat(_tmp231->pattern);if(_tmp231->where_clause != 0){{
const char*_tmp48B;Cyc_Absyndump_dump(((_tmp48B="&&",_tag_dyneither(_tmp48B,
sizeof(char),3))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmp231->where_clause));}{const char*_tmp48C;Cyc_Absyndump_dump_nospace(((_tmp48C=":",
_tag_dyneither(_tmp48C,sizeof(char),2))));};}Cyc_Absyndump_dumpstmt(_tmp231->body,
0);}}void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){void*_tmp236=
s->r;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_Absyn_Stmt*_tmp23B;struct Cyc_Absyn_Stmt*
_tmp23C;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_Absyn_Exp*_tmp240;struct Cyc_Absyn_Exp*
_tmp242;struct Cyc_Absyn_Stmt*_tmp243;struct Cyc_Absyn_Stmt*_tmp244;struct _tuple9
_tmp246;struct Cyc_Absyn_Exp*_tmp247;struct Cyc_Absyn_Stmt*_tmp248;struct
_dyneither_ptr*_tmp24C;struct Cyc_Absyn_Exp*_tmp24E;struct _tuple9 _tmp24F;struct
Cyc_Absyn_Exp*_tmp250;struct _tuple9 _tmp251;struct Cyc_Absyn_Exp*_tmp252;struct Cyc_Absyn_Stmt*
_tmp253;struct Cyc_Absyn_Exp*_tmp255;struct Cyc_List_List*_tmp256;struct Cyc_Absyn_Decl*
_tmp258;struct Cyc_Absyn_Stmt*_tmp259;struct _dyneither_ptr*_tmp25B;struct Cyc_Absyn_Stmt*
_tmp25C;struct Cyc_Absyn_Stmt*_tmp25E;struct _tuple9 _tmp25F;struct Cyc_Absyn_Exp*
_tmp260;struct Cyc_List_List*_tmp262;struct Cyc_List_List*_tmp264;struct Cyc_Absyn_Stmt*
_tmp266;struct Cyc_List_List*_tmp267;struct Cyc_Absyn_Exp*_tmp269;_LL154: {struct
Cyc_Absyn_Skip_s_struct*_tmp237=(struct Cyc_Absyn_Skip_s_struct*)_tmp236;if(
_tmp237->tag != 0)goto _LL156;}_LL155: Cyc_Absyndump_dump_semi();goto _LL153;_LL156: {
struct Cyc_Absyn_Exp_s_struct*_tmp238=(struct Cyc_Absyn_Exp_s_struct*)_tmp236;if(
_tmp238->tag != 1)goto _LL158;else{_tmp239=_tmp238->f1;}}_LL157: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dumpexp(_tmp239);Cyc_Absyndump_dump_semi();goto _LL153;
_LL158: {struct Cyc_Absyn_Seq_s_struct*_tmp23A=(struct Cyc_Absyn_Seq_s_struct*)
_tmp236;if(_tmp23A->tag != 2)goto _LL15A;else{_tmp23B=_tmp23A->f1;_tmp23C=_tmp23A->f2;}}
_LL159: if(Cyc_Absynpp_is_declaration(_tmp23B)){Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp23B,0);Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dumpstmt(
_tmp23B,0);}if(Cyc_Absynpp_is_declaration(_tmp23C)){if(expstmt)Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp23C,
expstmt);Cyc_Absyndump_dump_char((int)'}');if(expstmt)Cyc_Absyndump_dump_char((
int)')');Cyc_Absyndump_dump_semi();}else{Cyc_Absyndump_dumpstmt(_tmp23C,expstmt);}
goto _LL153;_LL15A: {struct Cyc_Absyn_Return_s_struct*_tmp23D=(struct Cyc_Absyn_Return_s_struct*)
_tmp236;if(_tmp23D->tag != 3)goto _LL15C;else{_tmp23E=_tmp23D->f1;if(_tmp23E != 0)
goto _LL15C;}}_LL15B: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp48D;Cyc_Absyndump_dump(((
_tmp48D="return;",_tag_dyneither(_tmp48D,sizeof(char),8))));}goto _LL153;_LL15C: {
struct Cyc_Absyn_Return_s_struct*_tmp23F=(struct Cyc_Absyn_Return_s_struct*)
_tmp236;if(_tmp23F->tag != 3)goto _LL15E;else{_tmp240=_tmp23F->f1;}}_LL15D: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp48E;Cyc_Absyndump_dump(((_tmp48E="return",_tag_dyneither(
_tmp48E,sizeof(char),7))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmp240));Cyc_Absyndump_dump_semi();goto _LL153;_LL15E: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp241=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp236;if(_tmp241->tag != 4)goto
_LL160;else{_tmp242=_tmp241->f1;_tmp243=_tmp241->f2;_tmp244=_tmp241->f3;}}_LL15F:
Cyc_Absyndump_dumploc(s->loc);{const char*_tmp48F;Cyc_Absyndump_dump(((_tmp48F="if(",
_tag_dyneither(_tmp48F,sizeof(char),4))));}Cyc_Absyndump_dumpexp(_tmp242);{void*
_tmp26D=_tmp243->r;_LL17B: {struct Cyc_Absyn_Seq_s_struct*_tmp26E=(struct Cyc_Absyn_Seq_s_struct*)
_tmp26D;if(_tmp26E->tag != 2)goto _LL17D;}_LL17C: goto _LL17E;_LL17D: {struct Cyc_Absyn_Decl_s_struct*
_tmp26F=(struct Cyc_Absyn_Decl_s_struct*)_tmp26D;if(_tmp26F->tag != 12)goto _LL17F;}
_LL17E: goto _LL180;_LL17F: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp270=(struct
Cyc_Absyn_IfThenElse_s_struct*)_tmp26D;if(_tmp270->tag != 4)goto _LL181;}_LL180:
goto _LL182;_LL181: {struct Cyc_Absyn_Label_s_struct*_tmp271=(struct Cyc_Absyn_Label_s_struct*)
_tmp26D;if(_tmp271->tag != 13)goto _LL183;}_LL182:{const char*_tmp490;Cyc_Absyndump_dump_nospace(((
_tmp490="){",_tag_dyneither(_tmp490,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(
_tmp243,0);Cyc_Absyndump_dump_char((int)'}');goto _LL17A;_LL183:;_LL184: Cyc_Absyndump_dump_char((
int)')');Cyc_Absyndump_dumpstmt(_tmp243,0);_LL17A:;}{void*_tmp273=_tmp244->r;
_LL186: {struct Cyc_Absyn_Skip_s_struct*_tmp274=(struct Cyc_Absyn_Skip_s_struct*)
_tmp273;if(_tmp274->tag != 0)goto _LL188;}_LL187: goto _LL185;_LL188:;_LL189:{const
char*_tmp491;Cyc_Absyndump_dump(((_tmp491="else{",_tag_dyneither(_tmp491,sizeof(
char),6))));}Cyc_Absyndump_dumpstmt(_tmp244,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL185;_LL185:;}goto _LL153;_LL160: {struct Cyc_Absyn_While_s_struct*_tmp245=(
struct Cyc_Absyn_While_s_struct*)_tmp236;if(_tmp245->tag != 5)goto _LL162;else{
_tmp246=_tmp245->f1;_tmp247=_tmp246.f1;_tmp248=_tmp245->f2;}}_LL161: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp492;Cyc_Absyndump_dump(((_tmp492="while(",_tag_dyneither(
_tmp492,sizeof(char),7))));}Cyc_Absyndump_dumpexp(_tmp247);{const char*_tmp493;
Cyc_Absyndump_dump_nospace(((_tmp493="){",_tag_dyneither(_tmp493,sizeof(char),3))));}
Cyc_Absyndump_dumpstmt(_tmp248,0);Cyc_Absyndump_dump_char((int)'}');goto _LL153;
_LL162: {struct Cyc_Absyn_Break_s_struct*_tmp249=(struct Cyc_Absyn_Break_s_struct*)
_tmp236;if(_tmp249->tag != 6)goto _LL164;}_LL163: Cyc_Absyndump_dumploc(s->loc);{
const char*_tmp494;Cyc_Absyndump_dump(((_tmp494="break;",_tag_dyneither(_tmp494,
sizeof(char),7))));}goto _LL153;_LL164: {struct Cyc_Absyn_Continue_s_struct*
_tmp24A=(struct Cyc_Absyn_Continue_s_struct*)_tmp236;if(_tmp24A->tag != 7)goto
_LL166;}_LL165: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp495;Cyc_Absyndump_dump(((
_tmp495="continue;",_tag_dyneither(_tmp495,sizeof(char),10))));}goto _LL153;
_LL166: {struct Cyc_Absyn_Goto_s_struct*_tmp24B=(struct Cyc_Absyn_Goto_s_struct*)
_tmp236;if(_tmp24B->tag != 8)goto _LL168;else{_tmp24C=_tmp24B->f1;}}_LL167: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp496;Cyc_Absyndump_dump(((_tmp496="goto",_tag_dyneither(
_tmp496,sizeof(char),5))));}Cyc_Absyndump_dump_str(_tmp24C);Cyc_Absyndump_dump_semi();
goto _LL153;_LL168: {struct Cyc_Absyn_For_s_struct*_tmp24D=(struct Cyc_Absyn_For_s_struct*)
_tmp236;if(_tmp24D->tag != 9)goto _LL16A;else{_tmp24E=_tmp24D->f1;_tmp24F=_tmp24D->f2;
_tmp250=_tmp24F.f1;_tmp251=_tmp24D->f3;_tmp252=_tmp251.f1;_tmp253=_tmp24D->f4;}}
_LL169: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp497;Cyc_Absyndump_dump(((
_tmp497="for(",_tag_dyneither(_tmp497,sizeof(char),5))));}Cyc_Absyndump_dumpexp(
_tmp24E);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp250);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp252);{const char*_tmp498;Cyc_Absyndump_dump_nospace(((
_tmp498="){",_tag_dyneither(_tmp498,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(
_tmp253,0);Cyc_Absyndump_dump_char((int)'}');goto _LL153;_LL16A: {struct Cyc_Absyn_Switch_s_struct*
_tmp254=(struct Cyc_Absyn_Switch_s_struct*)_tmp236;if(_tmp254->tag != 10)goto
_LL16C;else{_tmp255=_tmp254->f1;_tmp256=_tmp254->f2;}}_LL16B: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp499;Cyc_Absyndump_dump(((_tmp499="switch(",_tag_dyneither(
_tmp499,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp255);{const char*_tmp49A;
Cyc_Absyndump_dump_nospace(((_tmp49A="){",_tag_dyneither(_tmp49A,sizeof(char),3))));}
Cyc_Absyndump_dumpswitchclauses(_tmp256);Cyc_Absyndump_dump_char((int)'}');goto
_LL153;_LL16C: {struct Cyc_Absyn_Decl_s_struct*_tmp257=(struct Cyc_Absyn_Decl_s_struct*)
_tmp236;if(_tmp257->tag != 12)goto _LL16E;else{_tmp258=_tmp257->f1;_tmp259=_tmp257->f2;}}
_LL16D: Cyc_Absyndump_dumpdecl(_tmp258);Cyc_Absyndump_dumpstmt(_tmp259,expstmt);
goto _LL153;_LL16E: {struct Cyc_Absyn_Label_s_struct*_tmp25A=(struct Cyc_Absyn_Label_s_struct*)
_tmp236;if(_tmp25A->tag != 13)goto _LL170;else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25A->f2;}}
_LL16F: if(Cyc_Absynpp_is_declaration(_tmp25C)){Cyc_Absyndump_dump_str(_tmp25B);
if(expstmt){const char*_tmp49B;Cyc_Absyndump_dump_nospace(((_tmp49B=": ({",
_tag_dyneither(_tmp49B,sizeof(char),5))));}else{const char*_tmp49C;Cyc_Absyndump_dump_nospace(((
_tmp49C=": {",_tag_dyneither(_tmp49C,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(
_tmp25C,expstmt);if(expstmt){const char*_tmp49D;Cyc_Absyndump_dump_nospace(((
_tmp49D="});}",_tag_dyneither(_tmp49D,sizeof(char),5))));}else{Cyc_Absyndump_dump_char((
int)'}');}}else{Cyc_Absyndump_dump_str(_tmp25B);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpstmt(_tmp25C,expstmt);}goto _LL153;_LL170: {struct Cyc_Absyn_Do_s_struct*
_tmp25D=(struct Cyc_Absyn_Do_s_struct*)_tmp236;if(_tmp25D->tag != 14)goto _LL172;
else{_tmp25E=_tmp25D->f1;_tmp25F=_tmp25D->f2;_tmp260=_tmp25F.f1;}}_LL171: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp49E;Cyc_Absyndump_dump(((_tmp49E="do{",_tag_dyneither(
_tmp49E,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp25E,0);{const char*_tmp49F;
Cyc_Absyndump_dump_nospace(((_tmp49F="}while(",_tag_dyneither(_tmp49F,sizeof(
char),8))));}Cyc_Absyndump_dumpexp(_tmp260);{const char*_tmp4A0;Cyc_Absyndump_dump_nospace(((
_tmp4A0=");",_tag_dyneither(_tmp4A0,sizeof(char),3))));}goto _LL153;_LL172: {
struct Cyc_Absyn_Fallthru_s_struct*_tmp261=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp236;if(_tmp261->tag != 11)goto _LL174;else{_tmp262=_tmp261->f1;if(_tmp262 != 0)
goto _LL174;}}_LL173:{const char*_tmp4A1;Cyc_Absyndump_dump(((_tmp4A1="fallthru;",
_tag_dyneither(_tmp4A1,sizeof(char),10))));}goto _LL153;_LL174: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp263=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp236;if(_tmp263->tag != 11)goto
_LL176;else{_tmp264=_tmp263->f1;}}_LL175:{const char*_tmp4A2;Cyc_Absyndump_dump(((
_tmp4A2="fallthru(",_tag_dyneither(_tmp4A2,sizeof(char),10))));}Cyc_Absyndump_dumpexps_prec(
20,_tmp264);{const char*_tmp4A3;Cyc_Absyndump_dump_nospace(((_tmp4A3=");",
_tag_dyneither(_tmp4A3,sizeof(char),3))));}goto _LL153;_LL176: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp265=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp236;if(_tmp265->tag != 15)goto
_LL178;else{_tmp266=_tmp265->f1;_tmp267=_tmp265->f2;}}_LL177: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp4A4;Cyc_Absyndump_dump(((_tmp4A4="try",_tag_dyneither(
_tmp4A4,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp266,0);{const char*_tmp4A5;
Cyc_Absyndump_dump(((_tmp4A5="catch{",_tag_dyneither(_tmp4A5,sizeof(char),7))));}
Cyc_Absyndump_dumpswitchclauses(_tmp267);Cyc_Absyndump_dump_char((int)'}');goto
_LL153;_LL178: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp268=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp236;if(_tmp268->tag != 16)goto _LL153;else{_tmp269=_tmp268->f1;}}_LL179: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp4A6;Cyc_Absyndump_dump(((_tmp4A6="reset_region(",
_tag_dyneither(_tmp4A6,sizeof(char),14))));}Cyc_Absyndump_dumpexp(_tmp269);{
const char*_tmp4A7;Cyc_Absyndump_dump(((_tmp4A7=");",_tag_dyneither(_tmp4A7,
sizeof(char),3))));}goto _LL153;_LL153:;}struct _tuple18{struct Cyc_List_List*f1;
struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(struct _tuple18*dp){{const char*
_tmp4AA;const char*_tmp4A9;const char*_tmp4A8;((void(*)(void(*f)(void*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(*dp).f1,((_tmp4A8="",_tag_dyneither(_tmp4A8,
sizeof(char),1))),((_tmp4A9="=",_tag_dyneither(_tmp4A9,sizeof(char),2))),((
_tmp4AA="=",_tag_dyneither(_tmp4AA,sizeof(char),2))));}Cyc_Absyndump_dumppat((*
dp).f2);}void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){void*_tmp28F=p->r;
enum Cyc_Absyn_Sign _tmp293;int _tmp294;enum Cyc_Absyn_Sign _tmp296;int _tmp297;
enum Cyc_Absyn_Sign _tmp299;int _tmp29A;char _tmp29C;struct _dyneither_ptr _tmp29E;
struct Cyc_Absyn_Vardecl*_tmp2A0;struct Cyc_Absyn_Pat*_tmp2A1;struct Cyc_Absyn_Pat
_tmp2A2;void*_tmp2A3;struct Cyc_Absyn_Vardecl*_tmp2A6;struct Cyc_Absyn_Pat*_tmp2A7;
struct Cyc_List_List*_tmp2A9;int _tmp2AA;struct Cyc_Absyn_Pat*_tmp2AC;struct Cyc_Absyn_Vardecl*
_tmp2AE;struct Cyc_Absyn_Pat*_tmp2AF;struct Cyc_Absyn_Pat _tmp2B0;void*_tmp2B1;
struct Cyc_Absyn_Vardecl*_tmp2B4;struct Cyc_Absyn_Pat*_tmp2B5;struct Cyc_Absyn_Tvar*
_tmp2B7;struct Cyc_Absyn_Vardecl*_tmp2B8;struct _tuple0*_tmp2BA;struct _tuple0*
_tmp2BC;struct Cyc_List_List*_tmp2BD;int _tmp2BE;struct Cyc_Absyn_AggrInfo*_tmp2C0;
struct Cyc_Absyn_AggrInfo _tmp2C1;union Cyc_Absyn_AggrInfoU _tmp2C2;struct Cyc_List_List*
_tmp2C3;struct Cyc_List_List*_tmp2C4;int _tmp2C5;struct Cyc_Absyn_AggrInfo*_tmp2C7;
struct Cyc_List_List*_tmp2C8;struct Cyc_List_List*_tmp2C9;int _tmp2CA;struct Cyc_Absyn_Datatypefield*
_tmp2CC;struct Cyc_List_List*_tmp2CD;int _tmp2CE;struct Cyc_Absyn_Enumfield*_tmp2D0;
struct Cyc_Absyn_Enumfield*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D4;_LL18B: {struct Cyc_Absyn_Wild_p_struct*
_tmp290=(struct Cyc_Absyn_Wild_p_struct*)_tmp28F;if(_tmp290->tag != 0)goto _LL18D;}
_LL18C: Cyc_Absyndump_dump_char((int)'_');goto _LL18A;_LL18D: {struct Cyc_Absyn_Null_p_struct*
_tmp291=(struct Cyc_Absyn_Null_p_struct*)_tmp28F;if(_tmp291->tag != 8)goto _LL18F;}
_LL18E:{const char*_tmp4AB;Cyc_Absyndump_dump(((_tmp4AB="NULL",_tag_dyneither(
_tmp4AB,sizeof(char),5))));}goto _LL18A;_LL18F: {struct Cyc_Absyn_Int_p_struct*
_tmp292=(struct Cyc_Absyn_Int_p_struct*)_tmp28F;if(_tmp292->tag != 9)goto _LL191;
else{_tmp293=_tmp292->f1;if(_tmp293 != Cyc_Absyn_None)goto _LL191;_tmp294=_tmp292->f2;}}
_LL190: _tmp297=_tmp294;goto _LL192;_LL191: {struct Cyc_Absyn_Int_p_struct*_tmp295=(
struct Cyc_Absyn_Int_p_struct*)_tmp28F;if(_tmp295->tag != 9)goto _LL193;else{
_tmp296=_tmp295->f1;if(_tmp296 != Cyc_Absyn_Signed)goto _LL193;_tmp297=_tmp295->f2;}}
_LL192:{const char*_tmp4AF;void*_tmp4AE[1];struct Cyc_Int_pa_struct _tmp4AD;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp4AD.tag=1,((_tmp4AD.f1=(unsigned long)_tmp297,((
_tmp4AE[0]=& _tmp4AD,Cyc_aprintf(((_tmp4AF="%d",_tag_dyneither(_tmp4AF,sizeof(
char),3))),_tag_dyneither(_tmp4AE,sizeof(void*),1)))))))));}goto _LL18A;_LL193: {
struct Cyc_Absyn_Int_p_struct*_tmp298=(struct Cyc_Absyn_Int_p_struct*)_tmp28F;if(
_tmp298->tag != 9)goto _LL195;else{_tmp299=_tmp298->f1;if(_tmp299 != Cyc_Absyn_Unsigned)
goto _LL195;_tmp29A=_tmp298->f2;}}_LL194:{const char*_tmp4B3;void*_tmp4B2[1];
struct Cyc_Int_pa_struct _tmp4B1;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp4B1.tag=1,((_tmp4B1.f1=(unsigned int)_tmp29A,((_tmp4B2[0]=& _tmp4B1,Cyc_aprintf(((
_tmp4B3="%u",_tag_dyneither(_tmp4B3,sizeof(char),3))),_tag_dyneither(_tmp4B2,
sizeof(void*),1)))))))));}goto _LL18A;_LL195: {struct Cyc_Absyn_Char_p_struct*
_tmp29B=(struct Cyc_Absyn_Char_p_struct*)_tmp28F;if(_tmp29B->tag != 10)goto _LL197;
else{_tmp29C=_tmp29B->f1;}}_LL196:{const char*_tmp4B4;Cyc_Absyndump_dump(((
_tmp4B4="'",_tag_dyneither(_tmp4B4,sizeof(char),2))));}Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape(_tmp29C));{const char*_tmp4B5;Cyc_Absyndump_dump_nospace(((
_tmp4B5="'",_tag_dyneither(_tmp4B5,sizeof(char),2))));}goto _LL18A;_LL197: {
struct Cyc_Absyn_Float_p_struct*_tmp29D=(struct Cyc_Absyn_Float_p_struct*)_tmp28F;
if(_tmp29D->tag != 11)goto _LL199;else{_tmp29E=_tmp29D->f1;}}_LL198: Cyc_Absyndump_dump(
_tmp29E);goto _LL18A;_LL199: {struct Cyc_Absyn_Var_p_struct*_tmp29F=(struct Cyc_Absyn_Var_p_struct*)
_tmp28F;if(_tmp29F->tag != 1)goto _LL19B;else{_tmp2A0=_tmp29F->f1;_tmp2A1=_tmp29F->f2;
_tmp2A2=*_tmp2A1;_tmp2A3=_tmp2A2.r;{struct Cyc_Absyn_Wild_p_struct*_tmp2A4=(
struct Cyc_Absyn_Wild_p_struct*)_tmp2A3;if(_tmp2A4->tag != 0)goto _LL19B;};}}_LL19A:
Cyc_Absyndump_dumpqvar(_tmp2A0->name);goto _LL18A;_LL19B: {struct Cyc_Absyn_Var_p_struct*
_tmp2A5=(struct Cyc_Absyn_Var_p_struct*)_tmp28F;if(_tmp2A5->tag != 1)goto _LL19D;
else{_tmp2A6=_tmp2A5->f1;_tmp2A7=_tmp2A5->f2;}}_LL19C: Cyc_Absyndump_dumpqvar(
_tmp2A6->name);{const char*_tmp4B6;Cyc_Absyndump_dump(((_tmp4B6=" as ",
_tag_dyneither(_tmp4B6,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp2A7);goto
_LL18A;_LL19D: {struct Cyc_Absyn_Tuple_p_struct*_tmp2A8=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp28F;if(_tmp2A8->tag != 4)goto _LL19F;else{_tmp2A9=_tmp2A8->f1;_tmp2AA=_tmp2A8->f2;}}
_LL19E: {const char*_tmp4B8;const char*_tmp4B7;struct _dyneither_ptr term=_tmp2AA?(
_tmp4B8=", ...)",_tag_dyneither(_tmp4B8,sizeof(char),7)):((_tmp4B7=")",
_tag_dyneither(_tmp4B7,sizeof(char),2)));{const char*_tmp4BA;const char*_tmp4B9;((
void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp2A9,((_tmp4B9="$(",_tag_dyneither(_tmp4B9,sizeof(char),
3))),term,((_tmp4BA=",",_tag_dyneither(_tmp4BA,sizeof(char),2))));}goto _LL18A;}
_LL19F: {struct Cyc_Absyn_Pointer_p_struct*_tmp2AB=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp28F;if(_tmp2AB->tag != 5)goto _LL1A1;else{_tmp2AC=_tmp2AB->f1;}}_LL1A0:{const
char*_tmp4BB;Cyc_Absyndump_dump(((_tmp4BB="&",_tag_dyneither(_tmp4BB,sizeof(char),
2))));}Cyc_Absyndump_dumppat(_tmp2AC);goto _LL18A;_LL1A1: {struct Cyc_Absyn_Reference_p_struct*
_tmp2AD=(struct Cyc_Absyn_Reference_p_struct*)_tmp28F;if(_tmp2AD->tag != 2)goto
_LL1A3;else{_tmp2AE=_tmp2AD->f1;_tmp2AF=_tmp2AD->f2;_tmp2B0=*_tmp2AF;_tmp2B1=
_tmp2B0.r;{struct Cyc_Absyn_Wild_p_struct*_tmp2B2=(struct Cyc_Absyn_Wild_p_struct*)
_tmp2B1;if(_tmp2B2->tag != 0)goto _LL1A3;};}}_LL1A2:{const char*_tmp4BC;Cyc_Absyndump_dump(((
_tmp4BC="*",_tag_dyneither(_tmp4BC,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(
_tmp2AE->name);goto _LL18A;_LL1A3: {struct Cyc_Absyn_Reference_p_struct*_tmp2B3=(
struct Cyc_Absyn_Reference_p_struct*)_tmp28F;if(_tmp2B3->tag != 2)goto _LL1A5;else{
_tmp2B4=_tmp2B3->f1;_tmp2B5=_tmp2B3->f2;}}_LL1A4:{const char*_tmp4BD;Cyc_Absyndump_dump(((
_tmp4BD="*",_tag_dyneither(_tmp4BD,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(
_tmp2B4->name);{const char*_tmp4BE;Cyc_Absyndump_dump(((_tmp4BE=" as ",
_tag_dyneither(_tmp4BE,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp2B5);goto
_LL18A;_LL1A5: {struct Cyc_Absyn_TagInt_p_struct*_tmp2B6=(struct Cyc_Absyn_TagInt_p_struct*)
_tmp28F;if(_tmp2B6->tag != 3)goto _LL1A7;else{_tmp2B7=_tmp2B6->f1;_tmp2B8=_tmp2B6->f2;}}
_LL1A6: Cyc_Absyndump_dumpqvar(_tmp2B8->name);Cyc_Absyndump_dump_char((int)'<');
Cyc_Absyndump_dumptvar(_tmp2B7);Cyc_Absyndump_dump_char((int)'>');goto _LL18A;
_LL1A7: {struct Cyc_Absyn_UnknownId_p_struct*_tmp2B9=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp28F;if(_tmp2B9->tag != 14)goto _LL1A9;else{_tmp2BA=_tmp2B9->f1;}}_LL1A8: Cyc_Absyndump_dumpqvar(
_tmp2BA);goto _LL18A;_LL1A9: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp2BB=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp28F;if(_tmp2BB->tag != 15)goto _LL1AB;
else{_tmp2BC=_tmp2BB->f1;_tmp2BD=_tmp2BB->f2;_tmp2BE=_tmp2BB->f3;}}_LL1AA: {
const char*_tmp4C0;const char*_tmp4BF;struct _dyneither_ptr term=_tmp2BE?(_tmp4C0=", ...)",
_tag_dyneither(_tmp4C0,sizeof(char),7)):((_tmp4BF=")",_tag_dyneither(_tmp4BF,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp2BC);{const char*_tmp4C2;const char*
_tmp4C1;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp2BD,((_tmp4C1="(",_tag_dyneither(_tmp4C1,sizeof(char),
2))),term,((_tmp4C2=",",_tag_dyneither(_tmp4C2,sizeof(char),2))));}goto _LL18A;}
_LL1AB: {struct Cyc_Absyn_Aggr_p_struct*_tmp2BF=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp28F;if(_tmp2BF->tag != 6)goto _LL1AD;else{_tmp2C0=_tmp2BF->f1;if(_tmp2C0 == 0)
goto _LL1AD;_tmp2C1=*_tmp2C0;_tmp2C2=_tmp2C1.aggr_info;_tmp2C3=_tmp2BF->f2;
_tmp2C4=_tmp2BF->f3;_tmp2C5=_tmp2BF->f4;}}_LL1AC: {struct _tuple0*_tmp2EC;struct
_tuple10 _tmp2EB=Cyc_Absyn_aggr_kinded_name(_tmp2C2);_tmp2EC=_tmp2EB.f2;{const
char*_tmp4C4;const char*_tmp4C3;struct _dyneither_ptr term=_tmp2C5?(_tmp4C4=", ...)",
_tag_dyneither(_tmp4C4,sizeof(char),7)):((_tmp4C3=")",_tag_dyneither(_tmp4C3,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp2EC);Cyc_Absyndump_dump_char((int)'{');{
const char*_tmp4C7;const char*_tmp4C6;const char*_tmp4C5;((void(*)(void(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp2C3,((_tmp4C5="[",_tag_dyneither(_tmp4C5,sizeof(char),2))),((_tmp4C6="]",
_tag_dyneither(_tmp4C6,sizeof(char),2))),((_tmp4C7=",",_tag_dyneither(_tmp4C7,
sizeof(char),2))));}{const char*_tmp4C9;const char*_tmp4C8;((void(*)(void(*f)(
struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp2C4,((_tmp4C8="",_tag_dyneither(_tmp4C8,sizeof(char),1))),term,((_tmp4C9=",",
_tag_dyneither(_tmp4C9,sizeof(char),2))));}goto _LL18A;};}_LL1AD: {struct Cyc_Absyn_Aggr_p_struct*
_tmp2C6=(struct Cyc_Absyn_Aggr_p_struct*)_tmp28F;if(_tmp2C6->tag != 6)goto _LL1AF;
else{_tmp2C7=_tmp2C6->f1;if(_tmp2C7 != 0)goto _LL1AF;_tmp2C8=_tmp2C6->f2;_tmp2C9=
_tmp2C6->f3;_tmp2CA=_tmp2C6->f4;}}_LL1AE: {const char*_tmp4CB;const char*_tmp4CA;
struct _dyneither_ptr term=_tmp2CA?(_tmp4CB=", ...)",_tag_dyneither(_tmp4CB,
sizeof(char),7)):((_tmp4CA=")",_tag_dyneither(_tmp4CA,sizeof(char),2)));Cyc_Absyndump_dump_char((
int)'{');{const char*_tmp4CE;const char*_tmp4CD;const char*_tmp4CC;((void(*)(void(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp2C8,((_tmp4CC="[",_tag_dyneither(_tmp4CC,sizeof(char),2))),((_tmp4CD="]",
_tag_dyneither(_tmp4CD,sizeof(char),2))),((_tmp4CE=",",_tag_dyneither(_tmp4CE,
sizeof(char),2))));}{const char*_tmp4D0;const char*_tmp4CF;((void(*)(void(*f)(
struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp2C9,((_tmp4CF="",_tag_dyneither(_tmp4CF,sizeof(char),1))),term,((_tmp4D0=",",
_tag_dyneither(_tmp4D0,sizeof(char),2))));}goto _LL18A;}_LL1AF: {struct Cyc_Absyn_Datatype_p_struct*
_tmp2CB=(struct Cyc_Absyn_Datatype_p_struct*)_tmp28F;if(_tmp2CB->tag != 7)goto
_LL1B1;else{_tmp2CC=_tmp2CB->f2;_tmp2CD=_tmp2CB->f3;_tmp2CE=_tmp2CB->f4;}}_LL1B0: {
const char*_tmp4D2;const char*_tmp4D1;struct _dyneither_ptr term=_tmp2CE?(_tmp4D2=", ...)",
_tag_dyneither(_tmp4D2,sizeof(char),7)):((_tmp4D1=")",_tag_dyneither(_tmp4D1,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp2CC->name);if(_tmp2CD != 0){const
char*_tmp4D4;const char*_tmp4D3;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct
Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp2CD,((_tmp4D3="(",
_tag_dyneither(_tmp4D3,sizeof(char),2))),term,((_tmp4D4=",",_tag_dyneither(
_tmp4D4,sizeof(char),2))));}goto _LL18A;}_LL1B1: {struct Cyc_Absyn_Enum_p_struct*
_tmp2CF=(struct Cyc_Absyn_Enum_p_struct*)_tmp28F;if(_tmp2CF->tag != 12)goto _LL1B3;
else{_tmp2D0=_tmp2CF->f2;}}_LL1B2: _tmp2D2=_tmp2D0;goto _LL1B4;_LL1B3: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmp2D1=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp28F;if(_tmp2D1->tag != 13)goto
_LL1B5;else{_tmp2D2=_tmp2D1->f2;}}_LL1B4: Cyc_Absyndump_dumpqvar(_tmp2D2->name);
goto _LL18A;_LL1B5: {struct Cyc_Absyn_Exp_p_struct*_tmp2D3=(struct Cyc_Absyn_Exp_p_struct*)
_tmp28F;if(_tmp2D3->tag != 16)goto _LL18A;else{_tmp2D4=_tmp2D3->f1;}}_LL1B6: Cyc_Absyndump_dumpexp(
_tmp2D4);goto _LL18A;_LL18A:;}void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*
ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->typs != 0)Cyc_Absyndump_dumpargs(ef->typs);}
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){const char*
_tmp4D5;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,
struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,
fields,((_tmp4D5=",",_tag_dyneither(_tmp4D5,sizeof(char),2))));}void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield*ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->tag != 0){{
const char*_tmp4D6;Cyc_Absyndump_dump(((_tmp4D6=" = ",_tag_dyneither(_tmp4D6,
sizeof(char),4))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){const char*_tmp4D7;((
void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct
_dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,fields,((
_tmp4D7=",",_tag_dyneither(_tmp4D7,sizeof(char),2))));}void Cyc_Absyndump_dumpaggrfields(
struct Cyc_List_List*fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield
_tmp303;struct _dyneither_ptr*_tmp304;struct Cyc_Absyn_Tqual _tmp305;void*_tmp306;
struct Cyc_Absyn_Exp*_tmp307;struct Cyc_List_List*_tmp308;struct Cyc_Absyn_Aggrfield*
_tmp302=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp303=*_tmp302;_tmp304=_tmp303.name;
_tmp305=_tmp303.tq;_tmp306=_tmp303.type;_tmp307=_tmp303.width;_tmp308=_tmp303.attributes;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1B7:((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(
_tmp305,_tmp306,Cyc_Absyndump_dump_str,_tmp304);Cyc_Absyndump_dumpatts(_tmp308);
break;case Cyc_Cyclone_Vc_c: _LL1B8: Cyc_Absyndump_dumpatts(_tmp308);((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))
Cyc_Absyndump_dumptqtd)(_tmp305,_tmp306,Cyc_Absyndump_dump_str,_tmp304);break;}
if(_tmp307 != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp((struct
Cyc_Absyn_Exp*)_tmp307);}Cyc_Absyndump_dump_semi();}}void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(td->name);Cyc_Absyndump_dumptvars(
td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple19{void*f1;struct
_tuple0*f2;};static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple19*pr){{void*
_tmp309=(*pr).f1;_LL1BB: {struct Cyc_Absyn_Unused_att_struct*_tmp30A=(struct Cyc_Absyn_Unused_att_struct*)
_tmp309;if(_tmp30A->tag != 11)goto _LL1BD;}_LL1BC: goto _LL1BA;_LL1BD: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp30B=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp309;if(_tmp30B->tag != 1)goto
_LL1BF;}_LL1BE:{const char*_tmp4D8;Cyc_Absyndump_dump(((_tmp4D8="_stdcall",
_tag_dyneither(_tmp4D8,sizeof(char),9))));}goto _LL1BA;_LL1BF: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp30C=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp309;if(_tmp30C->tag != 2)goto
_LL1C1;}_LL1C0:{const char*_tmp4D9;Cyc_Absyndump_dump(((_tmp4D9="_cdecl",
_tag_dyneither(_tmp4D9,sizeof(char),7))));}goto _LL1BA;_LL1C1: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp30D=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp309;if(_tmp30D->tag != 3)goto
_LL1C3;}_LL1C2:{const char*_tmp4DA;Cyc_Absyndump_dump(((_tmp4DA="_fastcall",
_tag_dyneither(_tmp4DA,sizeof(char),10))));}goto _LL1BA;_LL1C3:;_LL1C4: goto _LL1BA;
_LL1BA:;}Cyc_Absyndump_dumpqvar((*pr).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 
0;vds=vds->tl){Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0)Cyc_Absyndump_dump_char((int)',');}}void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Vardecl
_tmp312;enum Cyc_Absyn_Scope _tmp313;struct _tuple0*_tmp314;struct Cyc_Absyn_Tqual
_tmp315;void*_tmp316;struct Cyc_Absyn_Exp*_tmp317;struct Cyc_List_List*_tmp318;
struct Cyc_Absyn_Vardecl*_tmp311=vd;_tmp312=*_tmp311;_tmp313=_tmp312.sc;_tmp314=
_tmp312.name;_tmp315=_tmp312.tq;_tmp316=_tmp312.type;_tmp317=_tmp312.initializer;
_tmp318=_tmp312.attributes;Cyc_Absyndump_dumploc(loc);switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL1C5: if(_tmp313 == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){
void*_tmp319=Cyc_Tcutil_compress(_tmp316);_LL1C8: {struct Cyc_Absyn_FnType_struct*
_tmp31A=(struct Cyc_Absyn_FnType_struct*)_tmp319;if(_tmp31A->tag != 9)goto _LL1CA;}
_LL1C9: goto _LL1C7;_LL1CA:;_LL1CB: Cyc_Absyndump_dumpscope(_tmp313);_LL1C7:;}else{
Cyc_Absyndump_dumpscope(_tmp313);}((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp315,_tmp316,Cyc_Absyndump_dumpqvar,
_tmp314);Cyc_Absyndump_dumpatts(_tmp318);break;case Cyc_Cyclone_Vc_c: _LL1C6: Cyc_Absyndump_dumpatts(
_tmp318);Cyc_Absyndump_dumpscope(_tmp313);{struct _RegionHandle _tmp31B=
_new_region("temp");struct _RegionHandle*temp=& _tmp31B;_push_region(temp);{struct
Cyc_Absyn_Tqual _tmp31D;void*_tmp31E;struct Cyc_List_List*_tmp31F;struct _tuple12
_tmp31C=Cyc_Absynpp_to_tms(temp,_tmp315,_tmp316);_tmp31D=_tmp31C.f1;_tmp31E=
_tmp31C.f2;_tmp31F=_tmp31C.f3;{void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;{
struct Cyc_List_List*tms2=_tmp31F;for(0;tms2 != 0;tms2=tms2->tl){void*_tmp320=(
void*)tms2->hd;struct Cyc_List_List*_tmp322;_LL1CE: {struct Cyc_Absyn_Attributes_mod_struct*
_tmp321=(struct Cyc_Absyn_Attributes_mod_struct*)_tmp320;if(_tmp321->tag != 5)goto
_LL1D0;else{_tmp322=_tmp321->f2;}}_LL1CF: for(0;_tmp322 != 0;_tmp322=_tmp322->tl){
void*_tmp323=(void*)_tmp322->hd;_LL1D3: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp324=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp323;if(_tmp324->tag != 1)goto
_LL1D5;}_LL1D4: call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL1D2;_LL1D5: {
struct Cyc_Absyn_Cdecl_att_struct*_tmp325=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp323;if(_tmp325->tag != 2)goto _LL1D7;}_LL1D6: call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;
goto _LL1D2;_LL1D7: {struct Cyc_Absyn_Fastcall_att_struct*_tmp326=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp323;if(_tmp326->tag != 3)goto _LL1D9;}_LL1D8: call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;
goto _LL1D2;_LL1D9:;_LL1DA: goto _LL1D2;_LL1D2:;}goto _LL1CD;_LL1D0:;_LL1D1: goto
_LL1CD;_LL1CD:;}}Cyc_Absyndump_dumptq(_tmp31D);Cyc_Absyndump_dumpntyp(_tmp31E);{
struct _tuple19 _tmp4DB;struct _tuple19 _tmp327=(_tmp4DB.f1=call_conv,((_tmp4DB.f2=
_tmp314,_tmp4DB)));((void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple19*),
struct _tuple19*a))Cyc_Absyndump_dumptms)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp31F),Cyc_Absyndump_dump_callconv_qvar,& _tmp327);};};}
_npop_handler(0);break;;_pop_region(temp);};}if(_tmp317 != 0){Cyc_Absyndump_dump_char((
int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp317);}Cyc_Absyndump_dump_semi();}
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){Cyc_Absyndump_dumpscope(
ad->sc);Cyc_Absyndump_dumpaggr_kind(ad->kind);Cyc_Absyndump_dumpqvar(ad->name);
Cyc_Absyndump_dumptvars(ad->tvs);if(ad->impl == 0)return;else{Cyc_Absyndump_dump_char((
int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 
0){const char*_tmp4DE;const char*_tmp4DD;const char*_tmp4DC;((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmp4DC="<",
_tag_dyneither(_tmp4DC,sizeof(char),2))),((_tmp4DD=">",_tag_dyneither(_tmp4DD,
sizeof(char),2))),((_tmp4DE=",",_tag_dyneither(_tmp4DE,sizeof(char),2))));}if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);{
const char*_tmp4DF;Cyc_Absyndump_dump(((_tmp4DF="}",_tag_dyneither(_tmp4DF,
sizeof(char),2))));}Cyc_Absyndump_dumpatts(ad->attributes);}}static void Cyc_Absyndump_dump_enumdecl(
struct Cyc_Absyn_Enumdecl*ed){struct Cyc_Absyn_Enumdecl _tmp32E;enum Cyc_Absyn_Scope
_tmp32F;struct _tuple0*_tmp330;struct Cyc_Core_Opt*_tmp331;struct Cyc_Absyn_Enumdecl*
_tmp32D=ed;_tmp32E=*_tmp32D;_tmp32F=_tmp32E.sc;_tmp330=_tmp32E.name;_tmp331=
_tmp32E.fields;Cyc_Absyndump_dumpscope(_tmp32F);{const char*_tmp4E0;Cyc_Absyndump_dump(((
_tmp4E0="enum ",_tag_dyneither(_tmp4E0,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(
_tmp330);if(_tmp331 != 0){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpenumfields((
struct Cyc_List_List*)_tmp331->v);{const char*_tmp4E1;Cyc_Absyndump_dump_nospace(((
_tmp4E1="}",_tag_dyneither(_tmp4E1,sizeof(char),2))));};}}static void Cyc_Absyndump_dump_datatypedecl(
struct Cyc_Absyn_Datatypedecl*dd){struct Cyc_Absyn_Datatypedecl _tmp335;enum Cyc_Absyn_Scope
_tmp336;struct _tuple0*_tmp337;struct Cyc_List_List*_tmp338;struct Cyc_Core_Opt*
_tmp339;int _tmp33A;struct Cyc_Absyn_Datatypedecl*_tmp334=dd;_tmp335=*_tmp334;
_tmp336=_tmp335.sc;_tmp337=_tmp335.name;_tmp338=_tmp335.tvs;_tmp339=_tmp335.fields;
_tmp33A=_tmp335.is_extensible;Cyc_Absyndump_dumpscope(_tmp336);if(_tmp33A){const
char*_tmp4E2;Cyc_Absyndump_dump(((_tmp4E2="@extensible ",_tag_dyneither(_tmp4E2,
sizeof(char),13))));}{const char*_tmp4E3;Cyc_Absyndump_dump(((_tmp4E3="datatype ",
_tag_dyneither(_tmp4E3,sizeof(char),10))));}Cyc_Absyndump_dumpqvar(_tmp337);Cyc_Absyndump_dumptvars(
_tmp338);if(_tmp339 != 0){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpdatatypefields((
struct Cyc_List_List*)_tmp339->v);{const char*_tmp4E4;Cyc_Absyndump_dump_nospace(((
_tmp4E4="}",_tag_dyneither(_tmp4E4,sizeof(char),2))));};}}struct _tuple20{struct
Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*d){void*_tmp33E=d->r;struct Cyc_Absyn_Vardecl*_tmp340;struct
Cyc_Absyn_Fndecl*_tmp342;struct Cyc_Absyn_Aggrdecl*_tmp344;struct Cyc_Absyn_Datatypedecl*
_tmp346;struct Cyc_Absyn_Enumdecl*_tmp348;struct Cyc_Absyn_Pat*_tmp34A;struct Cyc_Absyn_Exp*
_tmp34B;struct Cyc_List_List*_tmp34D;struct Cyc_Absyn_Tvar*_tmp34F;struct Cyc_Absyn_Vardecl*
_tmp350;int _tmp351;struct Cyc_Absyn_Exp*_tmp352;struct Cyc_Absyn_Exp*_tmp354;
struct Cyc_Absyn_Tvar*_tmp355;struct Cyc_Absyn_Vardecl*_tmp356;struct Cyc_Absyn_Typedefdecl*
_tmp358;struct _dyneither_ptr*_tmp35A;struct Cyc_List_List*_tmp35B;struct _tuple0*
_tmp35D;struct Cyc_List_List*_tmp35E;struct Cyc_List_List*_tmp360;struct Cyc_List_List*
_tmp362;struct Cyc_List_List*_tmp363;_LL1DC: {struct Cyc_Absyn_Var_d_struct*
_tmp33F=(struct Cyc_Absyn_Var_d_struct*)_tmp33E;if(_tmp33F->tag != 0)goto _LL1DE;
else{_tmp340=_tmp33F->f1;}}_LL1DD: Cyc_Absyndump_dumpvardecl(_tmp340,d->loc);goto
_LL1DB;_LL1DE: {struct Cyc_Absyn_Fn_d_struct*_tmp341=(struct Cyc_Absyn_Fn_d_struct*)
_tmp33E;if(_tmp341->tag != 1)goto _LL1E0;else{_tmp342=_tmp341->f1;}}_LL1DF: Cyc_Absyndump_dumploc(
d->loc);switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL1FC: Cyc_Absyndump_dumpatts(
_tmp342->attributes);break;case Cyc_Cyclone_Gcc_c: _LL1FD: break;}if(_tmp342->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp366=Cyc_Cyclone_c_compiler;_LL200: if(_tmp366 != 
Cyc_Cyclone_Vc_c)goto _LL202;_LL201:{const char*_tmp4E5;Cyc_Absyndump_dump(((
_tmp4E5="__inline",_tag_dyneither(_tmp4E5,sizeof(char),9))));}goto _LL1FF;_LL202:;
_LL203:{const char*_tmp4E6;Cyc_Absyndump_dump(((_tmp4E6="inline",_tag_dyneither(
_tmp4E6,sizeof(char),7))));}goto _LL1FF;_LL1FF:;}Cyc_Absyndump_dumpscope(_tmp342->sc);{
struct Cyc_Absyn_FnType_struct _tmp4EC;struct Cyc_Absyn_FnInfo _tmp4EB;struct Cyc_Absyn_FnType_struct*
_tmp4EA;void*t=(void*)((_tmp4EA=_cycalloc(sizeof(*_tmp4EA)),((_tmp4EA[0]=((
_tmp4EC.tag=9,((_tmp4EC.f1=((_tmp4EB.tvars=_tmp342->tvs,((_tmp4EB.effect=_tmp342->effect,((
_tmp4EB.ret_tqual=_tmp342->ret_tqual,((_tmp4EB.ret_typ=_tmp342->ret_type,((
_tmp4EB.args=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp342->args),((
_tmp4EB.c_varargs=_tmp342->c_varargs,((_tmp4EB.cyc_varargs=_tmp342->cyc_varargs,((
_tmp4EB.rgn_po=_tmp342->rgn_po,((_tmp4EB.attributes=0,_tmp4EB)))))))))))))))))),
_tmp4EC)))),_tmp4EA))));switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL204:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,
_tmp342);break;case Cyc_Cyclone_Vc_c: _LL205:((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(
Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,_tmp342);break;}
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp342->body,0);Cyc_Absyndump_dump_char((
int)'}');goto _LL1DB;};_LL1E0: {struct Cyc_Absyn_Aggr_d_struct*_tmp343=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp33E;if(_tmp343->tag != 6)goto _LL1E2;else{_tmp344=_tmp343->f1;}}_LL1E1: Cyc_Absyndump_dump_aggrdecl(
_tmp344);Cyc_Absyndump_dump_semi();goto _LL1DB;_LL1E2: {struct Cyc_Absyn_Datatype_d_struct*
_tmp345=(struct Cyc_Absyn_Datatype_d_struct*)_tmp33E;if(_tmp345->tag != 7)goto
_LL1E4;else{_tmp346=_tmp345->f1;}}_LL1E3: Cyc_Absyndump_dump_datatypedecl(_tmp346);
Cyc_Absyndump_dump_semi();goto _LL1DB;_LL1E4: {struct Cyc_Absyn_Enum_d_struct*
_tmp347=(struct Cyc_Absyn_Enum_d_struct*)_tmp33E;if(_tmp347->tag != 8)goto _LL1E6;
else{_tmp348=_tmp347->f1;}}_LL1E5: Cyc_Absyndump_dump_enumdecl(_tmp348);Cyc_Absyndump_dump_semi();
goto _LL1DB;_LL1E6: {struct Cyc_Absyn_Let_d_struct*_tmp349=(struct Cyc_Absyn_Let_d_struct*)
_tmp33E;if(_tmp349->tag != 2)goto _LL1E8;else{_tmp34A=_tmp349->f1;_tmp34B=_tmp349->f3;}}
_LL1E7:{const char*_tmp4ED;Cyc_Absyndump_dump(((_tmp4ED="let",_tag_dyneither(
_tmp4ED,sizeof(char),4))));}Cyc_Absyndump_dumppat(_tmp34A);Cyc_Absyndump_dump_char((
int)'=');Cyc_Absyndump_dumpexp(_tmp34B);Cyc_Absyndump_dump_semi();goto _LL1DB;
_LL1E8: {struct Cyc_Absyn_Letv_d_struct*_tmp34C=(struct Cyc_Absyn_Letv_d_struct*)
_tmp33E;if(_tmp34C->tag != 3)goto _LL1EA;else{_tmp34D=_tmp34C->f1;}}_LL1E9:{const
char*_tmp4EE;Cyc_Absyndump_dump(((_tmp4EE="let ",_tag_dyneither(_tmp4EE,sizeof(
char),5))));}Cyc_Absyndump_dumpids(_tmp34D);Cyc_Absyndump_dump_semi();goto _LL1DB;
_LL1EA: {struct Cyc_Absyn_Region_d_struct*_tmp34E=(struct Cyc_Absyn_Region_d_struct*)
_tmp33E;if(_tmp34E->tag != 4)goto _LL1EC;else{_tmp34F=_tmp34E->f1;_tmp350=_tmp34E->f2;
_tmp351=_tmp34E->f3;_tmp352=_tmp34E->f4;}}_LL1EB:{const char*_tmp4EF;Cyc_Absyndump_dump(((
_tmp4EF="region",_tag_dyneither(_tmp4EF,sizeof(char),7))));}{const char*_tmp4F0;
Cyc_Absyndump_dump(((_tmp4F0="<",_tag_dyneither(_tmp4F0,sizeof(char),2))));}Cyc_Absyndump_dumptvar(
_tmp34F);{const char*_tmp4F1;Cyc_Absyndump_dump(((_tmp4F1="> ",_tag_dyneither(
_tmp4F1,sizeof(char),3))));}Cyc_Absyndump_dumpqvar(_tmp350->name);if(_tmp351){
const char*_tmp4F2;Cyc_Absyndump_dump(((_tmp4F2="@resetable",_tag_dyneither(
_tmp4F2,sizeof(char),11))));}if(_tmp352 != 0){{const char*_tmp4F3;Cyc_Absyndump_dump(((
_tmp4F3=" = open(",_tag_dyneither(_tmp4F3,sizeof(char),9))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp352);{const char*_tmp4F4;Cyc_Absyndump_dump(((_tmp4F4=")",
_tag_dyneither(_tmp4F4,sizeof(char),2))));};}Cyc_Absyndump_dump_semi();goto
_LL1DB;_LL1EC: {struct Cyc_Absyn_Alias_d_struct*_tmp353=(struct Cyc_Absyn_Alias_d_struct*)
_tmp33E;if(_tmp353->tag != 5)goto _LL1EE;else{_tmp354=_tmp353->f1;_tmp355=_tmp353->f2;
_tmp356=_tmp353->f3;}}_LL1ED:{const char*_tmp4F5;Cyc_Absyndump_dump(((_tmp4F5="alias ",
_tag_dyneither(_tmp4F5,sizeof(char),7))));}{const char*_tmp4F6;Cyc_Absyndump_dump(((
_tmp4F6="<",_tag_dyneither(_tmp4F6,sizeof(char),2))));}Cyc_Absyndump_dumptvar(
_tmp355);{const char*_tmp4F7;Cyc_Absyndump_dump(((_tmp4F7=">",_tag_dyneither(
_tmp4F7,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp356->name);{const char*
_tmp4F8;Cyc_Absyndump_dump(((_tmp4F8=" = ",_tag_dyneither(_tmp4F8,sizeof(char),4))));}
Cyc_Absyndump_dumpexp(_tmp354);Cyc_Absyndump_dump_semi();goto _LL1DB;_LL1EE: {
struct Cyc_Absyn_Typedef_d_struct*_tmp357=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp33E;if(_tmp357->tag != 9)goto _LL1F0;else{_tmp358=_tmp357->f1;}}_LL1EF:{const
char*_tmp4F9;Cyc_Absyndump_dump(((_tmp4F9="typedef",_tag_dyneither(_tmp4F9,
sizeof(char),8))));}{void*t;if(_tmp358->defn == 0)t=Cyc_Absyn_new_evar(_tmp358->kind,
0);else{t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp358->defn))->v;}((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))
Cyc_Absyndump_dumptqtd)(_tmp358->tq,t,Cyc_Absyndump_dumptypedefname,_tmp358);Cyc_Absyndump_dumpatts(
_tmp358->atts);Cyc_Absyndump_dump_semi();goto _LL1DB;};_LL1F0: {struct Cyc_Absyn_Namespace_d_struct*
_tmp359=(struct Cyc_Absyn_Namespace_d_struct*)_tmp33E;if(_tmp359->tag != 10)goto
_LL1F2;else{_tmp35A=_tmp359->f1;_tmp35B=_tmp359->f2;}}_LL1F1:{const char*_tmp4FA;
Cyc_Absyndump_dump(((_tmp4FA="namespace",_tag_dyneither(_tmp4FA,sizeof(char),10))));}
Cyc_Absyndump_dump_str(_tmp35A);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp35B
!= 0;_tmp35B=_tmp35B->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp35B->hd);}
Cyc_Absyndump_dump_char((int)'}');goto _LL1DB;_LL1F2: {struct Cyc_Absyn_Using_d_struct*
_tmp35C=(struct Cyc_Absyn_Using_d_struct*)_tmp33E;if(_tmp35C->tag != 11)goto _LL1F4;
else{_tmp35D=_tmp35C->f1;_tmp35E=_tmp35C->f2;}}_LL1F3:{const char*_tmp4FB;Cyc_Absyndump_dump(((
_tmp4FB="using",_tag_dyneither(_tmp4FB,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(
_tmp35D);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp35E != 0;_tmp35E=_tmp35E->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp35E->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1DB;_LL1F4: {struct Cyc_Absyn_ExternC_d_struct*_tmp35F=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp33E;if(_tmp35F->tag != 12)goto _LL1F6;else{_tmp360=_tmp35F->f1;}}_LL1F5:{const
char*_tmp4FC;Cyc_Absyndump_dump(((_tmp4FC="extern \"C\" {",_tag_dyneither(
_tmp4FC,sizeof(char),13))));}for(0;_tmp360 != 0;_tmp360=_tmp360->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp360->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1DB;
_LL1F6: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp361=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp33E;if(_tmp361->tag != 13)goto _LL1F8;else{_tmp362=_tmp361->f1;_tmp363=_tmp361->f2;}}
_LL1F7:{const char*_tmp4FD;Cyc_Absyndump_dump(((_tmp4FD="extern \"C include\" {",
_tag_dyneither(_tmp4FD,sizeof(char),21))));}for(0;_tmp362 != 0;_tmp362=_tmp362->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp362->hd);}Cyc_Absyndump_dump_char((
int)'}');if(_tmp363 != 0){{const char*_tmp4FE;Cyc_Absyndump_dump(((_tmp4FE=" export {",
_tag_dyneither(_tmp4FE,sizeof(char),10))));}for(0;_tmp363 != 0;_tmp363=_tmp363->tl){
struct _tuple0*_tmp37F;struct _tuple20 _tmp37E=*((struct _tuple20*)_tmp363->hd);
_tmp37F=_tmp37E.f2;Cyc_Absyndump_dumpqvar(_tmp37F);if(_tmp363->tl != 0)Cyc_Absyndump_dump_char((
int)',');}{const char*_tmp4FF;Cyc_Absyndump_dump(((_tmp4FF="}",_tag_dyneither(
_tmp4FF,sizeof(char),2))));};}goto _LL1DB;_LL1F8: {struct Cyc_Absyn_Porton_d_struct*
_tmp364=(struct Cyc_Absyn_Porton_d_struct*)_tmp33E;if(_tmp364->tag != 14)goto
_LL1FA;}_LL1F9:{const char*_tmp500;Cyc_Absyndump_dump(((_tmp500=" __cyclone_port_on__; ",
_tag_dyneither(_tmp500,sizeof(char),23))));}goto _LL1DB;_LL1FA: {struct Cyc_Absyn_Portoff_d_struct*
_tmp365=(struct Cyc_Absyn_Portoff_d_struct*)_tmp33E;if(_tmp365->tag != 15)goto
_LL1DB;}_LL1FB:{const char*_tmp501;Cyc_Absyndump_dump(((_tmp501=" __cyclone_port_off__; ",
_tag_dyneither(_tmp501,sizeof(char),24))));}goto _LL1DB;_LL1DB:;}static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp*e){struct _tuple13 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1
!= 1  || !pr.f2){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((
int)'}');}}void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(void*),
void*a){if(tms == 0){f(a);return;}{void*_tmp383=(void*)tms->hd;struct Cyc_Absyn_PtrAtts
_tmp385;void*_tmp386;union Cyc_Absyn_Constraint*_tmp387;union Cyc_Absyn_Constraint*
_tmp388;union Cyc_Absyn_Constraint*_tmp389;struct Cyc_Absyn_Tqual _tmp38A;_LL208: {
struct Cyc_Absyn_Pointer_mod_struct*_tmp384=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp383;if(_tmp384->tag != 2)goto _LL20A;else{_tmp385=_tmp384->f1;_tmp386=_tmp385.rgn;
_tmp387=_tmp385.nullable;_tmp388=_tmp385.bounds;_tmp389=_tmp385.zero_term;
_tmp38A=_tmp384->f2;}}_LL209:{void*_tmp38B=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp388);struct Cyc_Absyn_Exp*
_tmp38E;_LL20D: {struct Cyc_Absyn_DynEither_b_struct*_tmp38C=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp38B;if(_tmp38C->tag != 0)goto _LL20F;}_LL20E: Cyc_Absyndump_dump_char((int)'?');
goto _LL20C;_LL20F: {struct Cyc_Absyn_Upper_b_struct*_tmp38D=(struct Cyc_Absyn_Upper_b_struct*)
_tmp38B;if(_tmp38D->tag != 1)goto _LL20C;else{_tmp38E=_tmp38D->f1;}}_LL210: Cyc_Absyndump_dump_char((
int)(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp387)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp38E);goto _LL20C;_LL20C:;}if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp389)){const char*_tmp502;
Cyc_Absyndump_dump(((_tmp502="@zeroterm",_tag_dyneither(_tmp502,sizeof(char),10))));}{
void*_tmp390=Cyc_Tcutil_compress(_tmp386);struct Cyc_Absyn_Tvar*_tmp395;struct Cyc_Core_Opt*
_tmp397;_LL212: {struct Cyc_Absyn_HeapRgn_struct*_tmp391=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp390;if(_tmp391->tag != 20)goto _LL214;}_LL213: goto _LL211;_LL214: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp392=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp390;if(_tmp392->tag != 21)goto
_LL216;}_LL215:{const char*_tmp503;Cyc_Absyndump_dump(((_tmp503="`U",
_tag_dyneither(_tmp503,sizeof(char),3))));}goto _LL211;_LL216: {struct Cyc_Absyn_RefCntRgn_struct*
_tmp393=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp390;if(_tmp393->tag != 22)goto
_LL218;}_LL217:{const char*_tmp504;Cyc_Absyndump_dump(((_tmp504="`RC",
_tag_dyneither(_tmp504,sizeof(char),4))));}goto _LL211;_LL218: {struct Cyc_Absyn_VarType_struct*
_tmp394=(struct Cyc_Absyn_VarType_struct*)_tmp390;if(_tmp394->tag != 2)goto _LL21A;
else{_tmp395=_tmp394->f1;}}_LL219: Cyc_Absyndump_dump_str(_tmp395->name);goto
_LL211;_LL21A: {struct Cyc_Absyn_Evar_struct*_tmp396=(struct Cyc_Absyn_Evar_struct*)
_tmp390;if(_tmp396->tag != 1)goto _LL21C;else{_tmp397=_tmp396->f2;if(_tmp397 != 0)
goto _LL21C;}}_LL21B: Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp386));goto
_LL211;_LL21C: {struct Cyc_Absyn_TypedefType_struct*_tmp398=(struct Cyc_Absyn_TypedefType_struct*)
_tmp390;if(_tmp398->tag != 17)goto _LL21E;}_LL21D:{const char*_tmp505;Cyc_Absyndump_dump(((
_tmp505="@region(",_tag_dyneither(_tmp505,sizeof(char),9))));}Cyc_Absyndump_dumptyp(
_tmp386);{const char*_tmp506;Cyc_Absyndump_dump(((_tmp506=")",_tag_dyneither(
_tmp506,sizeof(char),2))));}goto _LL211;_LL21E:;_LL21F: {const char*_tmp509;void*
_tmp508;(_tmp508=0,Cyc_Tcutil_impos(((_tmp509="dumptms: bad rgn type in Pointer_mod",
_tag_dyneither(_tmp509,sizeof(char),37))),_tag_dyneither(_tmp508,sizeof(void*),0)));}
_LL211:;}Cyc_Absyndump_dumptq(_tmp38A);Cyc_Absyndump_dumptms(tms->tl,f,a);
return;_LL20A:;_LL20B: {int next_is_pointer=0;if(tms->tl != 0){void*_tmp39F=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;_LL221: {struct Cyc_Absyn_Pointer_mod_struct*
_tmp3A0=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp39F;if(_tmp3A0->tag != 2)goto
_LL223;}_LL222: next_is_pointer=1;goto _LL220;_LL223:;_LL224: goto _LL220;_LL220:;}
if(next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp3A1=(void*)
tms->hd;union Cyc_Absyn_Constraint*_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A5;union Cyc_Absyn_Constraint*
_tmp3A6;void*_tmp3A8;struct Cyc_List_List*_tmp3AA;int _tmp3AB;struct Cyc_Absyn_VarargInfo*
_tmp3AC;struct Cyc_Core_Opt*_tmp3AD;struct Cyc_List_List*_tmp3AE;void*_tmp3B0;
struct Cyc_List_List*_tmp3B2;struct Cyc_Position_Segment*_tmp3B3;struct Cyc_List_List*
_tmp3B5;struct Cyc_Position_Segment*_tmp3B6;int _tmp3B7;struct Cyc_List_List*
_tmp3B9;_LL226: {struct Cyc_Absyn_Carray_mod_struct*_tmp3A2=(struct Cyc_Absyn_Carray_mod_struct*)
_tmp3A1;if(_tmp3A2->tag != 0)goto _LL228;else{_tmp3A3=_tmp3A2->f1;}}_LL227:{const
char*_tmp50A;Cyc_Absyndump_dump(((_tmp50A="[]",_tag_dyneither(_tmp50A,sizeof(
char),3))));}if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp3A3)){const char*_tmp50B;Cyc_Absyndump_dump(((_tmp50B="@zeroterm",
_tag_dyneither(_tmp50B,sizeof(char),10))));}goto _LL225;_LL228: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmp3A4=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmp3A1;if(_tmp3A4->tag != 1)goto
_LL22A;else{_tmp3A5=_tmp3A4->f1;_tmp3A6=_tmp3A4->f2;}}_LL229: Cyc_Absyndump_dump_char((
int)'[');Cyc_Absyndump_dumpexp(_tmp3A5);Cyc_Absyndump_dump_char((int)']');if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3A6)){const
char*_tmp50C;Cyc_Absyndump_dump(((_tmp50C="@zeroterm",_tag_dyneither(_tmp50C,
sizeof(char),10))));}goto _LL225;_LL22A: {struct Cyc_Absyn_Function_mod_struct*
_tmp3A7=(struct Cyc_Absyn_Function_mod_struct*)_tmp3A1;if(_tmp3A7->tag != 3)goto
_LL22C;else{_tmp3A8=(void*)_tmp3A7->f1;{struct Cyc_Absyn_WithTypes_struct*_tmp3A9=(
struct Cyc_Absyn_WithTypes_struct*)_tmp3A8;if(_tmp3A9->tag != 1)goto _LL22C;else{
_tmp3AA=_tmp3A9->f1;_tmp3AB=_tmp3A9->f2;_tmp3AC=_tmp3A9->f3;_tmp3AD=_tmp3A9->f4;
_tmp3AE=_tmp3A9->f5;}};}}_LL22B: Cyc_Absyndump_dumpfunargs(_tmp3AA,_tmp3AB,
_tmp3AC,_tmp3AD,_tmp3AE);goto _LL225;_LL22C: {struct Cyc_Absyn_Function_mod_struct*
_tmp3AF=(struct Cyc_Absyn_Function_mod_struct*)_tmp3A1;if(_tmp3AF->tag != 3)goto
_LL22E;else{_tmp3B0=(void*)_tmp3AF->f1;{struct Cyc_Absyn_NoTypes_struct*_tmp3B1=(
struct Cyc_Absyn_NoTypes_struct*)_tmp3B0;if(_tmp3B1->tag != 0)goto _LL22E;else{
_tmp3B2=_tmp3B1->f1;_tmp3B3=_tmp3B1->f2;}};}}_LL22D:{const char*_tmp50F;const char*
_tmp50E;const char*_tmp50D;((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dump_str,_tmp3B2,((_tmp50D="(",_tag_dyneither(_tmp50D,sizeof(char),
2))),((_tmp50E=")",_tag_dyneither(_tmp50E,sizeof(char),2))),((_tmp50F=",",
_tag_dyneither(_tmp50F,sizeof(char),2))));}goto _LL225;_LL22E: {struct Cyc_Absyn_TypeParams_mod_struct*
_tmp3B4=(struct Cyc_Absyn_TypeParams_mod_struct*)_tmp3A1;if(_tmp3B4->tag != 4)goto
_LL230;else{_tmp3B5=_tmp3B4->f1;_tmp3B6=_tmp3B4->f2;_tmp3B7=_tmp3B4->f3;}}_LL22F:
if(_tmp3B7)Cyc_Absyndump_dumpkindedtvars(_tmp3B5);else{Cyc_Absyndump_dumptvars(
_tmp3B5);}goto _LL225;_LL230: {struct Cyc_Absyn_Attributes_mod_struct*_tmp3B8=(
struct Cyc_Absyn_Attributes_mod_struct*)_tmp3A1;if(_tmp3B8->tag != 5)goto _LL232;
else{_tmp3B9=_tmp3B8->f2;}}_LL231: Cyc_Absyndump_dumpatts(_tmp3B9);goto _LL225;
_LL232: {struct Cyc_Absyn_Pointer_mod_struct*_tmp3BA=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp3A1;if(_tmp3BA->tag != 2)goto _LL225;}_LL233: {const char*_tmp512;void*_tmp511;(
_tmp511=0,Cyc_Tcutil_impos(((_tmp512="dumptms",_tag_dyneither(_tmp512,sizeof(
char),8))),_tag_dyneither(_tmp511,sizeof(void*),0)));}_LL225:;}return;}_LL207:;};}
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
struct _RegionHandle _tmp3C3=_new_region("temp");struct _RegionHandle*temp=& _tmp3C3;
_push_region(temp);{struct Cyc_Absyn_Tqual _tmp3C5;void*_tmp3C6;struct Cyc_List_List*
_tmp3C7;struct _tuple12 _tmp3C4=Cyc_Absynpp_to_tms(temp,tq,t);_tmp3C5=_tmp3C4.f1;
_tmp3C6=_tmp3C4.f2;_tmp3C7=_tmp3C4.f3;Cyc_Absyndump_dumptq(_tmp3C5);Cyc_Absyndump_dumpntyp(
_tmp3C6);Cyc_Absyndump_dumptms(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp3C7),f,a);};_pop_region(temp);}void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=
f;for(0;tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}{
const char*_tmp515;void*_tmp514;(_tmp514=0,Cyc_fprintf(f,((_tmp515="\n",
_tag_dyneither(_tmp515,sizeof(char),2))),_tag_dyneither(_tmp514,sizeof(void*),0)));};}
