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
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct
_dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _dyneither_ptr Cyc_Core_new_string(
unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
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
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);int Cyc_List_list_prefix(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);extern int
Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(
struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);struct _dyneither_ptr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();
struct Cyc_PP_Doc*Cyc_PP_line_doc();struct Cyc_PP_Doc*Cyc_PP_text(struct
_dyneither_ptr s);struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);struct
Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);struct Cyc_PP_Doc*Cyc_PP_nest(
int k,struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr
sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,
struct Cyc_List_List*l0);struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(
void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(
struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,
struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,
struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct
Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;};
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
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
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
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
Cyc_Core_Opt*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*
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
zt_loc;};struct Cyc_Absyn_Aggr_td_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};
struct Cyc_Absyn_Enum_td_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;struct
Cyc_Position_Segment*loc;};struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};
struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;enum 
Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_struct{int
tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;
};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_RefCntRgn_struct{
int tag;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_TypeDeclType_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;
};struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
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
struct Cyc_Buffer_t;unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dyneither_ptr);struct _tuple11{unsigned int f1;int f2;};
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct
Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{
char*tag;};struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_no_qual(struct Cyc_Position_Segment*
loc,void*t);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int
add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;
int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};extern int Cyc_Absynpp_print_for_cycdoc;void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct
_dyneither_ptr Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _dyneither_ptr
Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct _dyneither_ptr Cyc_Absynpp_prim2string(
enum Cyc_Absyn_Primop p);struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*
p);struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);int Cyc_Absynpp_is_anon_aggrtype(
void*t);extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;extern struct
_dyneither_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop
p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple12{struct
_dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple8*Cyc_Absynpp_arg_mk_opt(
struct _tuple12*arg);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual
tq,void*t);enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c
 = 1};extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple14{
struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(
struct _tuple14*dp);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*
cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt);struct
Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int
inprec,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct
_tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
enum Cyc_Absyn_Primop);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum 
Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple15{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple15*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct
Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd);struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ad);struct Cyc_PP_Doc*
Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*ad);static int Cyc_Absynpp_expand_typedefs;
static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[4]="Cyc";struct _dyneither_ptr
Cyc_Absynpp_cyc_string={_tmp0,_tmp0,_tmp0 + 4};struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=&
Cyc_Absynpp_cyc_string;static int Cyc_Absynpp_add_cyc_prefix;static int Cyc_Absynpp_to_VC;
static int Cyc_Absynpp_decls_first;static int Cyc_Absynpp_rewrite_temp_tvars;static
int Cyc_Absynpp_print_all_tvars;static int Cyc_Absynpp_print_all_kinds;static int Cyc_Absynpp_print_all_effects;
static int Cyc_Absynpp_print_using_stmts;static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars;static int Cyc_Absynpp_generate_line_directives;
static int Cyc_Absynpp_use_curr_namespace;static int Cyc_Absynpp_print_zeroterm;
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;int Cyc_Absynpp_print_for_cycdoc=
0;struct Cyc_Absynpp_Params;void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*
fs){Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=
fs->decls_first;Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_all_effects=
fs->print_all_effects;Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=
fs->print_externC_stmts;Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=
fs->print_zeroterm;Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=
fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,
1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,0,
0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,
0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,
0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_dyneither_ptr*v){struct Cyc_List_List*_tmp615;Cyc_Absynpp_curr_namespace=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_Absynpp_curr_namespace,((_tmp615=_cycalloc(sizeof(*_tmp615)),((_tmp615->hd=v,((
_tmp615->tl=0,_tmp615)))))));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**
l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((
struct Cyc_List_List*)_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0: {
const char*_tmp616;return(_tmp616="\\a",_tag_dyneither(_tmp616,sizeof(char),3));}
case '\b': _LL1: {const char*_tmp617;return(_tmp617="\\b",_tag_dyneither(_tmp617,
sizeof(char),3));}case '\f': _LL2: {const char*_tmp618;return(_tmp618="\\f",
_tag_dyneither(_tmp618,sizeof(char),3));}case '\n': _LL3: {const char*_tmp619;
return(_tmp619="\\n",_tag_dyneither(_tmp619,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp61A;return(_tmp61A="\\r",_tag_dyneither(_tmp61A,sizeof(char),3));}
case '\t': _LL5: {const char*_tmp61B;return(_tmp61B="\\t",_tag_dyneither(_tmp61B,
sizeof(char),3));}case '\v': _LL6: {const char*_tmp61C;return(_tmp61C="\\v",
_tag_dyneither(_tmp61C,sizeof(char),3));}case '\\': _LL7: {const char*_tmp61D;
return(_tmp61D="\\\\",_tag_dyneither(_tmp61D,sizeof(char),3));}case '"': _LL8: {
const char*_tmp61E;return(_tmp61E="\"",_tag_dyneither(_tmp61E,sizeof(char),2));}
case '\'': _LL9: {const char*_tmp61F;return(_tmp61F="\\'",_tag_dyneither(_tmp61F,
sizeof(char),3));}default: _LLA: if(c >= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=
Cyc_Core_new_string(2);{char _tmp622;char _tmp621;struct _dyneither_ptr _tmp620;(
_tmp620=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp621=*((char*)
_check_dyneither_subscript(_tmp620,sizeof(char),0)),((_tmp622=c,((
_get_dyneither_size(_tmp620,sizeof(char))== 1  && (_tmp621 == '\000'  && _tmp622 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp620.curr)=_tmp622)))))));}return(struct
_dyneither_ptr)_tmpC;}else{struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);int
j=0;{char _tmp625;char _tmp624;struct _dyneither_ptr _tmp623;(_tmp623=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp624=*((char*)
_check_dyneither_subscript(_tmp623,sizeof(char),0)),((_tmp625='\\',((
_get_dyneither_size(_tmp623,sizeof(char))== 1  && (_tmp624 == '\000'  && _tmp625 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp623.curr)=_tmp625)))))));}{char _tmp628;char
_tmp627;struct _dyneither_ptr _tmp626;(_tmp626=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp627=*((char*)_check_dyneither_subscript(_tmp626,sizeof(char),0)),((
_tmp628=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp626,
sizeof(char))== 1  && (_tmp627 == '\000'  && _tmp628 != '\000')?_throw_arraybounds():
1,*((char*)_tmp626.curr)=_tmp628)))))));}{char _tmp62B;char _tmp62A;struct
_dyneither_ptr _tmp629;(_tmp629=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp62A=*((char*)_check_dyneither_subscript(_tmp629,sizeof(char),0)),((_tmp62B=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp629,sizeof(char))== 1  && (
_tmp62A == '\000'  && _tmp62B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp629.curr)=
_tmp62B)))))));}{char _tmp62E;char _tmp62D;struct _dyneither_ptr _tmp62C;(_tmp62C=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp62D=*((char*)
_check_dyneither_subscript(_tmp62C,sizeof(char),0)),((_tmp62E=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp62C,sizeof(char))== 1  && (_tmp62D == '\000'  && _tmp62E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp62C.curr)=_tmp62E)))))));}return(struct
_dyneither_ptr)_tmp10;}}}static int Cyc_Absynpp_special(struct _dyneither_ptr s){int
sz=(int)(_get_dyneither_size(s,sizeof(char))- 1);{int i=0;for(0;i < sz;++ i){char c=*((
const char*)_check_dyneither_subscript(s,sizeof(char),i));if(((c <= ' '  || c >= '~')
 || c == '"') || c == '\\')return 1;}}return 0;}struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr s){if(!Cyc_Absynpp_special(s))return s;{int n=(int)(
_get_dyneither_size(s,sizeof(char))- 1);if(n > 0  && *((const char*)
_check_dyneither_subscript(s,sizeof(char),n))== '\000')-- n;{int len=0;{int i=0;
for(0;i <= n;++ i){char _tmp1D=*((const char*)_check_dyneither_subscript(s,sizeof(
char),i));_LLD: if(_tmp1D != '\a')goto _LLF;_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')
goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')goto _LL13;_LL12: goto _LL14;_LL13:
if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15: if(_tmp1D != '\r')goto _LL17;
_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;_LL18: goto _LL1A;_LL19: if(_tmp1D
!= '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D != '\\')goto _LL1D;_LL1C: goto
_LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;goto _LLC;_LL1F:;_LL20: if(
_tmp1D >= ' '  && _tmp1D <= '~')++ len;else{len +=4;}goto _LLC;_LLC:;}}{struct
_dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));int j=0;{int i=0;for(0;
i <= n;++ i){char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp631;char _tmp630;struct
_dyneither_ptr _tmp62F;(_tmp62F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp630=*((
char*)_check_dyneither_subscript(_tmp62F,sizeof(char),0)),((_tmp631='\\',((
_get_dyneither_size(_tmp62F,sizeof(char))== 1  && (_tmp630 == '\000'  && _tmp631 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp62F.curr)=_tmp631)))))));}{char _tmp634;char
_tmp633;struct _dyneither_ptr _tmp632;(_tmp632=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp633=*((char*)_check_dyneither_subscript(_tmp632,sizeof(char),0)),((
_tmp634='a',((_get_dyneither_size(_tmp632,sizeof(char))== 1  && (_tmp633 == '\000'
 && _tmp634 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp632.curr)=_tmp634)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp637;char _tmp636;struct
_dyneither_ptr _tmp635;(_tmp635=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp636=*((
char*)_check_dyneither_subscript(_tmp635,sizeof(char),0)),((_tmp637='\\',((
_get_dyneither_size(_tmp635,sizeof(char))== 1  && (_tmp636 == '\000'  && _tmp637 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp635.curr)=_tmp637)))))));}{char _tmp63A;char
_tmp639;struct _dyneither_ptr _tmp638;(_tmp638=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp639=*((char*)_check_dyneither_subscript(_tmp638,sizeof(char),0)),((
_tmp63A='b',((_get_dyneither_size(_tmp638,sizeof(char))== 1  && (_tmp639 == '\000'
 && _tmp63A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp638.curr)=_tmp63A)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp63D;char _tmp63C;struct
_dyneither_ptr _tmp63B;(_tmp63B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63C=*((
char*)_check_dyneither_subscript(_tmp63B,sizeof(char),0)),((_tmp63D='\\',((
_get_dyneither_size(_tmp63B,sizeof(char))== 1  && (_tmp63C == '\000'  && _tmp63D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp63B.curr)=_tmp63D)))))));}{char _tmp640;char
_tmp63F;struct _dyneither_ptr _tmp63E;(_tmp63E=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp63F=*((char*)_check_dyneither_subscript(_tmp63E,sizeof(char),0)),((
_tmp640='f',((_get_dyneither_size(_tmp63E,sizeof(char))== 1  && (_tmp63F == '\000'
 && _tmp640 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63E.curr)=_tmp640)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp643;char _tmp642;struct
_dyneither_ptr _tmp641;(_tmp641=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp642=*((
char*)_check_dyneither_subscript(_tmp641,sizeof(char),0)),((_tmp643='\\',((
_get_dyneither_size(_tmp641,sizeof(char))== 1  && (_tmp642 == '\000'  && _tmp643 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp641.curr)=_tmp643)))))));}{char _tmp646;char
_tmp645;struct _dyneither_ptr _tmp644;(_tmp644=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp645=*((char*)_check_dyneither_subscript(_tmp644,sizeof(char),0)),((
_tmp646='n',((_get_dyneither_size(_tmp644,sizeof(char))== 1  && (_tmp645 == '\000'
 && _tmp646 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp644.curr)=_tmp646)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp649;char _tmp648;struct
_dyneither_ptr _tmp647;(_tmp647=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp648=*((
char*)_check_dyneither_subscript(_tmp647,sizeof(char),0)),((_tmp649='\\',((
_get_dyneither_size(_tmp647,sizeof(char))== 1  && (_tmp648 == '\000'  && _tmp649 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp647.curr)=_tmp649)))))));}{char _tmp64C;char
_tmp64B;struct _dyneither_ptr _tmp64A;(_tmp64A=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp64B=*((char*)_check_dyneither_subscript(_tmp64A,sizeof(char),0)),((
_tmp64C='r',((_get_dyneither_size(_tmp64A,sizeof(char))== 1  && (_tmp64B == '\000'
 && _tmp64C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64A.curr)=_tmp64C)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp64F;char _tmp64E;struct
_dyneither_ptr _tmp64D;(_tmp64D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64E=*((
char*)_check_dyneither_subscript(_tmp64D,sizeof(char),0)),((_tmp64F='\\',((
_get_dyneither_size(_tmp64D,sizeof(char))== 1  && (_tmp64E == '\000'  && _tmp64F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp64D.curr)=_tmp64F)))))));}{char _tmp652;char
_tmp651;struct _dyneither_ptr _tmp650;(_tmp650=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp651=*((char*)_check_dyneither_subscript(_tmp650,sizeof(char),0)),((
_tmp652='t',((_get_dyneither_size(_tmp650,sizeof(char))== 1  && (_tmp651 == '\000'
 && _tmp652 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp650.curr)=_tmp652)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp655;char _tmp654;struct
_dyneither_ptr _tmp653;(_tmp653=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp654=*((
char*)_check_dyneither_subscript(_tmp653,sizeof(char),0)),((_tmp655='\\',((
_get_dyneither_size(_tmp653,sizeof(char))== 1  && (_tmp654 == '\000'  && _tmp655 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp653.curr)=_tmp655)))))));}{char _tmp658;char
_tmp657;struct _dyneither_ptr _tmp656;(_tmp656=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp657=*((char*)_check_dyneither_subscript(_tmp656,sizeof(char),0)),((
_tmp658='v',((_get_dyneither_size(_tmp656,sizeof(char))== 1  && (_tmp657 == '\000'
 && _tmp658 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp656.curr)=_tmp658)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp65B;char _tmp65A;struct
_dyneither_ptr _tmp659;(_tmp659=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp65A=*((
char*)_check_dyneither_subscript(_tmp659,sizeof(char),0)),((_tmp65B='\\',((
_get_dyneither_size(_tmp659,sizeof(char))== 1  && (_tmp65A == '\000'  && _tmp65B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp659.curr)=_tmp65B)))))));}{char _tmp65E;char
_tmp65D;struct _dyneither_ptr _tmp65C;(_tmp65C=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp65D=*((char*)_check_dyneither_subscript(_tmp65C,sizeof(char),0)),((
_tmp65E='\\',((_get_dyneither_size(_tmp65C,sizeof(char))== 1  && (_tmp65D == '\000'
 && _tmp65E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65C.curr)=_tmp65E)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp661;char _tmp660;struct
_dyneither_ptr _tmp65F;(_tmp65F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp660=*((
char*)_check_dyneither_subscript(_tmp65F,sizeof(char),0)),((_tmp661='\\',((
_get_dyneither_size(_tmp65F,sizeof(char))== 1  && (_tmp660 == '\000'  && _tmp661 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp65F.curr)=_tmp661)))))));}{char _tmp664;char
_tmp663;struct _dyneither_ptr _tmp662;(_tmp662=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp663=*((char*)_check_dyneither_subscript(_tmp662,sizeof(char),0)),((
_tmp664='"',((_get_dyneither_size(_tmp662,sizeof(char))== 1  && (_tmp663 == '\000'
 && _tmp664 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp662.curr)=_tmp664)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp667;char _tmp666;
struct _dyneither_ptr _tmp665;(_tmp665=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp666=*((char*)_check_dyneither_subscript(_tmp665,sizeof(char),0)),((_tmp667=
_tmp1E,((_get_dyneither_size(_tmp665,sizeof(char))== 1  && (_tmp666 == '\000'  && 
_tmp667 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp665.curr)=_tmp667)))))));}
else{{char _tmp66A;char _tmp669;struct _dyneither_ptr _tmp668;(_tmp668=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp669=*((char*)
_check_dyneither_subscript(_tmp668,sizeof(char),0)),((_tmp66A='\\',((
_get_dyneither_size(_tmp668,sizeof(char))== 1  && (_tmp669 == '\000'  && _tmp66A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp668.curr)=_tmp66A)))))));}{char _tmp66D;char
_tmp66C;struct _dyneither_ptr _tmp66B;(_tmp66B=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp66C=*((char*)_check_dyneither_subscript(_tmp66B,sizeof(char),0)),((
_tmp66D=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp66B,sizeof(char))
== 1  && (_tmp66C == '\000'  && _tmp66D != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp66B.curr)=_tmp66D)))))));}{char _tmp670;char _tmp66F;struct _dyneither_ptr
_tmp66E;(_tmp66E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp66F=*((char*)
_check_dyneither_subscript(_tmp66E,sizeof(char),0)),((_tmp670=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp66E,sizeof(char))== 1  && (_tmp66F == '\000'
 && _tmp670 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp66E.curr)=_tmp670)))))));}{
char _tmp673;char _tmp672;struct _dyneither_ptr _tmp671;(_tmp671=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp672=*((char*)_check_dyneither_subscript(_tmp671,
sizeof(char),0)),((_tmp673=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp671,sizeof(char))== 1  && (_tmp672 == '\000'  && _tmp673 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp671.curr)=_tmp673)))))));};}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;};};};}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*
l=0;if(tq.q_restrict){struct Cyc_List_List*_tmp674;l=((_tmp674=_cycalloc(sizeof(*
_tmp674)),((_tmp674->hd=Cyc_Absynpp_restrict_sp,((_tmp674->tl=l,_tmp674))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp675;l=((_tmp675=_cycalloc(sizeof(*
_tmp675)),((_tmp675->hd=Cyc_Absynpp_volatile_sp,((_tmp675->tl=l,_tmp675))))));}
if(tq.print_const){struct Cyc_List_List*_tmp676;l=((_tmp676=_cycalloc(sizeof(*
_tmp676)),((_tmp676->hd=Cyc_Absynpp_const_sp,((_tmp676->tl=l,_tmp676))))));}{
const char*_tmp679;const char*_tmp678;const char*_tmp677;return Cyc_PP_egroup(((
_tmp677="",_tag_dyneither(_tmp677,sizeof(char),1))),((_tmp678=" ",_tag_dyneither(
_tmp678,sizeof(char),2))),((_tmp679=" ",_tag_dyneither(_tmp679,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,l));};}struct _dyneither_ptr Cyc_Absynpp_kind2string(
struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp6E;enum Cyc_Absyn_KindQual
_tmp6F;enum Cyc_Absyn_AliasQual _tmp70;struct Cyc_Absyn_Kind*_tmp6D=ka;_tmp6E=*
_tmp6D;_tmp6F=_tmp6E.kind;_tmp70=_tmp6E.aliasqual;switch(_tmp6F){case Cyc_Absyn_AnyKind:
_LL36: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL38: {const char*_tmp67A;return(
_tmp67A="A",_tag_dyneither(_tmp67A,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp67B;return(_tmp67B="UA",_tag_dyneither(_tmp67B,sizeof(char),3));}
case Cyc_Absyn_Top: _LL3A: {const char*_tmp67C;return(_tmp67C="TA",_tag_dyneither(
_tmp67C,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37: switch(_tmp70){case Cyc_Absyn_Aliasable:
_LL3D: {const char*_tmp67D;return(_tmp67D="M",_tag_dyneither(_tmp67D,sizeof(char),
2));}case Cyc_Absyn_Unique: _LL3E: {const char*_tmp67E;return(_tmp67E="UM",
_tag_dyneither(_tmp67E,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {const char*
_tmp67F;return(_tmp67F="TM",_tag_dyneither(_tmp67F,sizeof(char),3));}}case Cyc_Absyn_BoxKind:
_LL3C: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL42: {const char*_tmp680;return(
_tmp680="B",_tag_dyneither(_tmp680,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp681;return(_tmp681="UB",_tag_dyneither(_tmp681,sizeof(char),3));}
case Cyc_Absyn_Top: _LL44: {const char*_tmp682;return(_tmp682="TB",_tag_dyneither(
_tmp682,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41: switch(_tmp70){case Cyc_Absyn_Aliasable:
_LL47: {const char*_tmp683;return(_tmp683="R",_tag_dyneither(_tmp683,sizeof(char),
2));}case Cyc_Absyn_Unique: _LL48: {const char*_tmp684;return(_tmp684="UR",
_tag_dyneither(_tmp684,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {const char*
_tmp685;return(_tmp685="TR",_tag_dyneither(_tmp685,sizeof(char),3));}}case Cyc_Absyn_EffKind:
_LL46: {const char*_tmp686;return(_tmp686="E",_tag_dyneither(_tmp686,sizeof(char),
2));}case Cyc_Absyn_IntKind: _LL4B: {const char*_tmp687;return(_tmp687="I",
_tag_dyneither(_tmp687,sizeof(char),2));}}}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp7F=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp81;struct Cyc_Absyn_Kind*_tmp84;_LL4E: {struct Cyc_Absyn_Eq_kb_struct*
_tmp80=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7F;if(_tmp80->tag != 0)goto _LL50;else{
_tmp81=_tmp80->f1;}}_LL4F: return Cyc_Absynpp_kind2string(_tmp81);_LL50: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp82=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp7F;
if(_tmp82->tag != 1)goto _LL52;}_LL51: if(Cyc_PP_tex_output){const char*_tmp688;
return(_tmp688="{?}",_tag_dyneither(_tmp688,sizeof(char),4));}else{const char*
_tmp689;return(_tmp689="?",_tag_dyneither(_tmp689,sizeof(char),2));}_LL52: {
struct Cyc_Absyn_Less_kb_struct*_tmp83=(struct Cyc_Absyn_Less_kb_struct*)_tmp7F;
if(_tmp83->tag != 2)goto _LL4D;else{_tmp84=_tmp83->f2;}}_LL53: {const char*_tmp68D;
void*_tmp68C[1];struct Cyc_String_pa_struct _tmp68B;return(struct _dyneither_ptr)((
_tmp68B.tag=0,((_tmp68B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp84)),((_tmp68C[0]=& _tmp68B,Cyc_aprintf(((_tmp68D="<=%s",_tag_dyneither(
_tmp68D,sizeof(char),5))),_tag_dyneither(_tmp68C,sizeof(void*),1))))))));}_LL4D:;}
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){void*_tmp8A=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp8C;struct Cyc_Absyn_Kind*_tmp8F;_LL55: {struct Cyc_Absyn_Eq_kb_struct*
_tmp8B=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL57;else{
_tmp8C=_tmp8B->f1;}}_LL56: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8C));
_LL57: {struct Cyc_Absyn_Unknown_kb_struct*_tmp8D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8A;if(_tmp8D->tag != 1)goto _LL59;}_LL58: if(Cyc_PP_tex_output){const char*
_tmp68E;return Cyc_PP_text_width(((_tmp68E="{?}",_tag_dyneither(_tmp68E,sizeof(
char),4))),1);}else{const char*_tmp68F;return Cyc_PP_text(((_tmp68F="?",
_tag_dyneither(_tmp68F,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_struct*
_tmp8E=(struct Cyc_Absyn_Less_kb_struct*)_tmp8A;if(_tmp8E->tag != 2)goto _LL54;
else{_tmp8F=_tmp8E->f2;}}_LL5A: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8F));
_LL54:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*
_tmp692;const char*_tmp691;const char*_tmp690;return Cyc_PP_egroup(((_tmp690="<",
_tag_dyneither(_tmp690,sizeof(char),2))),((_tmp691=">",_tag_dyneither(_tmp691,
sizeof(char),2))),((_tmp692=",",_tag_dyneither(_tmp692,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_typ2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*
tv){void*_tmp95=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp98;
struct Cyc_Absyn_Kind _tmp99;enum Cyc_Absyn_KindQual _tmp9A;enum Cyc_Absyn_AliasQual
_tmp9B;struct Cyc_Absyn_Kind*_tmp9D;struct Cyc_Absyn_Kind*_tmp9F;_LL5C: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp96=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp95;if(_tmp96->tag != 1)goto _LL5E;}
_LL5D: goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_struct*_tmp97=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp95;if(_tmp97->tag != 0)goto _LL60;else{_tmp98=_tmp97->f1;_tmp99=*_tmp98;_tmp9A=
_tmp99.kind;if(_tmp9A != Cyc_Absyn_BoxKind)goto _LL60;_tmp9B=_tmp99.aliasqual;if(
_tmp9B != Cyc_Absyn_Aliasable)goto _LL60;}}_LL5F: return Cyc_PP_textptr(tv->name);
_LL60: {struct Cyc_Absyn_Less_kb_struct*_tmp9C=(struct Cyc_Absyn_Less_kb_struct*)
_tmp95;if(_tmp9C->tag != 2)goto _LL62;else{_tmp9D=_tmp9C->f2;}}_LL61: _tmp9F=_tmp9D;
goto _LL63;_LL62: {struct Cyc_Absyn_Eq_kb_struct*_tmp9E=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp95;if(_tmp9E->tag != 0)goto _LL5B;else{_tmp9F=_tmp9E->f1;}}_LL63: {const char*
_tmp695;struct Cyc_PP_Doc*_tmp694[3];return(_tmp694[2]=Cyc_Absynpp_kind2doc(
_tmp9F),((_tmp694[1]=Cyc_PP_text(((_tmp695="::",_tag_dyneither(_tmp695,sizeof(
char),3)))),((_tmp694[0]=Cyc_PP_textptr(tv->name),Cyc_PP_cat(_tag_dyneither(
_tmp694,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs){const char*_tmp698;const char*_tmp697;const char*_tmp696;
return Cyc_PP_egroup(((_tmp696="<",_tag_dyneither(_tmp696,sizeof(char),2))),((
_tmp697=">",_tag_dyneither(_tmp697,sizeof(char),2))),((_tmp698=",",
_tag_dyneither(_tmp698,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,
tvs));}static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);{const char*
_tmp69B;const char*_tmp69A;const char*_tmp699;return Cyc_PP_egroup(((_tmp699="<",
_tag_dyneither(_tmp699,sizeof(char),2))),((_tmp69A=">",_tag_dyneither(_tmp69A,
sizeof(char),2))),((_tmp69B=",",_tag_dyneither(_tmp69B,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));};}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple16*t){return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*
Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){const char*_tmp69E;const char*_tmp69D;
const char*_tmp69C;return Cyc_PP_group(((_tmp69C="(",_tag_dyneither(_tmp69C,
sizeof(char),2))),((_tmp69D=")",_tag_dyneither(_tmp69D,sizeof(char),2))),((
_tmp69E=",",_tag_dyneither(_tmp69E,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmpAB=att;
_LL65: {struct Cyc_Absyn_Stdcall_att_struct*_tmpAC=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmpAB;if(_tmpAC->tag != 1)goto _LL67;}_LL66: goto _LL68;_LL67: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpAD=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpAB;if(_tmpAD->tag != 2)goto _LL69;}
_LL68: goto _LL6A;_LL69: {struct Cyc_Absyn_Fastcall_att_struct*_tmpAE=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpAB;if(_tmpAE->tag != 3)goto _LL6B;}_LL6A: return Cyc_PP_nil_doc();_LL6B:;_LL6C:
return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL64:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(
struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmpAF=(void*)atts->hd;
_LL6E: {struct Cyc_Absyn_Stdcall_att_struct*_tmpB0=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmpAF;if(_tmpB0->tag != 1)goto _LL70;}_LL6F: {const char*_tmp69F;return Cyc_PP_text(((
_tmp69F=" _stdcall ",_tag_dyneither(_tmp69F,sizeof(char),11))));}_LL70: {struct
Cyc_Absyn_Cdecl_att_struct*_tmpB1=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpAF;if(
_tmpB1->tag != 2)goto _LL72;}_LL71: {const char*_tmp6A0;return Cyc_PP_text(((_tmp6A0=" _cdecl ",
_tag_dyneither(_tmp6A0,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpB2=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpAF;if(_tmpB2->tag != 3)goto _LL74;}
_LL73: {const char*_tmp6A1;return Cyc_PP_text(((_tmp6A1=" _fastcall ",
_tag_dyneither(_tmp6A1,sizeof(char),12))));}_LL74:;_LL75: goto _LL6D;_LL6D:;}
return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpB6=(void*)atts2->hd;_LL77: {struct Cyc_Absyn_Stdcall_att_struct*_tmpB7=(
struct Cyc_Absyn_Stdcall_att_struct*)_tmpB6;if(_tmpB7->tag != 1)goto _LL79;}_LL78:
goto _LL7A;_LL79: {struct Cyc_Absyn_Cdecl_att_struct*_tmpB8=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmpB6;if(_tmpB8->tag != 2)goto _LL7B;}_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpB9=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpB6;if(_tmpB9->tag != 3)goto _LL7D;}
_LL7C: goto _LL76;_LL7D:;_LL7E: hasatt=1;goto _LL76;_LL76:;}}if(!hasatt)return Cyc_PP_nil_doc();{
const char*_tmp6AC;const char*_tmp6AB;const char*_tmp6AA;const char*_tmp6A9;const
char*_tmp6A8;struct Cyc_PP_Doc*_tmp6A7[3];return(_tmp6A7[2]=Cyc_PP_text(((_tmp6A8=")",
_tag_dyneither(_tmp6A8,sizeof(char),2)))),((_tmp6A7[1]=Cyc_PP_group(((_tmp6AB="",
_tag_dyneither(_tmp6AB,sizeof(char),1))),((_tmp6AA="",_tag_dyneither(_tmp6AA,
sizeof(char),1))),((_tmp6A9=" ",_tag_dyneither(_tmp6A9,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_noncallatt2doc,atts)),((_tmp6A7[0]=Cyc_PP_text(((_tmp6AC=" __declspec(",
_tag_dyneither(_tmp6AC,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp6A7,
sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*
a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Vc_c: _LL7F: return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c:
_LL80: {const char*_tmp6B5;const char*_tmp6B4;const char*_tmp6B3;const char*_tmp6B2;
struct Cyc_PP_Doc*_tmp6B1[2];return(_tmp6B1[1]=Cyc_PP_group(((_tmp6B4="((",
_tag_dyneither(_tmp6B4,sizeof(char),3))),((_tmp6B3="))",_tag_dyneither(_tmp6B3,
sizeof(char),3))),((_tmp6B2=",",_tag_dyneither(_tmp6B2,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_att2doc,atts)),((_tmp6B1[0]=Cyc_PP_text(((_tmp6B5=" __attribute__",
_tag_dyneither(_tmp6B5,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp6B1,
sizeof(struct Cyc_PP_Doc*),2)))));}}}int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*
tms){if(tms == 0)return 0;{void*_tmpC5=(void*)tms->hd;_LL83: {struct Cyc_Absyn_Pointer_mod_struct*
_tmpC6=(struct Cyc_Absyn_Pointer_mod_struct*)_tmpC5;if(_tmpC6->tag != 2)goto _LL85;}
_LL84: return 1;_LL85: {struct Cyc_Absyn_Attributes_mod_struct*_tmpC7=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC5;if(_tmpC7->tag != 5)goto _LL87;}_LL86: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL89: return 0;default: _LL8A: return Cyc_Absynpp_next_is_pointer(
tms->tl);}_LL87:;_LL88: return 0;_LL82:;};}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*
Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output){
const char*_tmp6B6;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp6B6="{?}",_tag_dyneither(_tmp6B6,sizeof(char),4))),1);}else{const char*
_tmp6B7;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp6B7="?",
_tag_dyneither(_tmp6B7,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){const char*_tmp6B8;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp6B8="{\\lb}",_tag_dyneither(_tmp6B8,sizeof(char),6))),1);}
else{const char*_tmp6B9;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp6B9="{",_tag_dyneither(_tmp6B9,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_lb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){const char*_tmp6BA;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp6BA="{\\rb}",_tag_dyneither(_tmp6BA,sizeof(char),6))),1);}
else{const char*_tmp6BB;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp6BB="}",_tag_dyneither(_tmp6BB,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){const char*_tmp6BC;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp6BC="\\$",_tag_dyneither(_tmp6BC,sizeof(char),3))),1);}
else{const char*_tmp6BD;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp6BD="$",_tag_dyneither(_tmp6BD,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_dollar);}struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(
struct _dyneither_ptr sep,struct Cyc_List_List*ss){struct Cyc_PP_Doc*_tmp6BE[3];
return(_tmp6BE[2]=Cyc_Absynpp_rb(),((_tmp6BE[1]=Cyc_PP_seq(sep,ss),((_tmp6BE[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp6BE,sizeof(struct Cyc_PP_Doc*),3)))))));}
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){for(0;tms != 0;tms=tms->tl){
void*_tmpD1=(void*)tms->hd;void*_tmpD5;struct Cyc_List_List*_tmpD7;_LL8D: {struct
Cyc_Absyn_Carray_mod_struct*_tmpD2=(struct Cyc_Absyn_Carray_mod_struct*)_tmpD1;
if(_tmpD2->tag != 0)goto _LL8F;}_LL8E:{const char*_tmp6C1;void*_tmp6C0;(_tmp6C0=0,
Cyc_fprintf(Cyc_stderr,((_tmp6C1="Carray_mod ",_tag_dyneither(_tmp6C1,sizeof(
char),12))),_tag_dyneither(_tmp6C0,sizeof(void*),0)));}goto _LL8C;_LL8F: {struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD3=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD1;if(_tmpD3->tag != 1)goto _LL91;}_LL90:{const char*_tmp6C4;void*_tmp6C3;(
_tmp6C3=0,Cyc_fprintf(Cyc_stderr,((_tmp6C4="ConstArray_mod ",_tag_dyneither(
_tmp6C4,sizeof(char),16))),_tag_dyneither(_tmp6C3,sizeof(void*),0)));}goto _LL8C;
_LL91: {struct Cyc_Absyn_Function_mod_struct*_tmpD4=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD4->tag != 3)goto _LL93;else{_tmpD5=(void*)_tmpD4->f1;{struct Cyc_Absyn_WithTypes_struct*
_tmpD6=(struct Cyc_Absyn_WithTypes_struct*)_tmpD5;if(_tmpD6->tag != 1)goto _LL93;
else{_tmpD7=_tmpD6->f1;}};}}_LL92:{const char*_tmp6C7;void*_tmp6C6;(_tmp6C6=0,Cyc_fprintf(
Cyc_stderr,((_tmp6C7="Function_mod(",_tag_dyneither(_tmp6C7,sizeof(char),14))),
_tag_dyneither(_tmp6C6,sizeof(void*),0)));}for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){
struct Cyc_Core_Opt*_tmpE2=(*((struct _tuple8*)_tmpD7->hd)).f1;if(_tmpE2 == 0){
const char*_tmp6CA;void*_tmp6C9;(_tmp6C9=0,Cyc_fprintf(Cyc_stderr,((_tmp6CA="?",
_tag_dyneither(_tmp6CA,sizeof(char),2))),_tag_dyneither(_tmp6C9,sizeof(void*),0)));}
else{void*_tmp6CB;(_tmp6CB=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)
_tmpE2->v),_tag_dyneither(_tmp6CB,sizeof(void*),0)));}if(_tmpD7->tl != 0){const
char*_tmp6CE;void*_tmp6CD;(_tmp6CD=0,Cyc_fprintf(Cyc_stderr,((_tmp6CE=",",
_tag_dyneither(_tmp6CE,sizeof(char),2))),_tag_dyneither(_tmp6CD,sizeof(void*),0)));}}{
const char*_tmp6D1;void*_tmp6D0;(_tmp6D0=0,Cyc_fprintf(Cyc_stderr,((_tmp6D1=") ",
_tag_dyneither(_tmp6D1,sizeof(char),3))),_tag_dyneither(_tmp6D0,sizeof(void*),0)));}
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_struct*_tmpD8=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD8->tag != 3)goto _LL95;}_LL94:{const char*_tmp6D4;void*_tmp6D3;(
_tmp6D3=0,Cyc_fprintf(Cyc_stderr,((_tmp6D4="Function_mod()",_tag_dyneither(
_tmp6D4,sizeof(char),15))),_tag_dyneither(_tmp6D3,sizeof(void*),0)));}goto _LL8C;
_LL95: {struct Cyc_Absyn_Attributes_mod_struct*_tmpD9=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpD1;if(_tmpD9->tag != 5)goto _LL97;}_LL96:{const char*_tmp6D7;void*_tmp6D6;(
_tmp6D6=0,Cyc_fprintf(Cyc_stderr,((_tmp6D7="Attributes_mod ",_tag_dyneither(
_tmp6D7,sizeof(char),16))),_tag_dyneither(_tmp6D6,sizeof(void*),0)));}goto _LL8C;
_LL97: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpDA=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpD1;if(_tmpDA->tag != 4)goto _LL99;}_LL98:{const char*_tmp6DA;void*_tmp6D9;(
_tmp6D9=0,Cyc_fprintf(Cyc_stderr,((_tmp6DA="TypeParams_mod ",_tag_dyneither(
_tmp6DA,sizeof(char),16))),_tag_dyneither(_tmp6D9,sizeof(void*),0)));}goto _LL8C;
_LL99: {struct Cyc_Absyn_Pointer_mod_struct*_tmpDB=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpD1;if(_tmpDB->tag != 2)goto _LL8C;}_LL9A:{const char*_tmp6DD;void*_tmp6DC;(
_tmp6DC=0,Cyc_fprintf(Cyc_stderr,((_tmp6DD="Pointer_mod ",_tag_dyneither(_tmp6DD,
sizeof(char),13))),_tag_dyneither(_tmp6DC,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
const char*_tmp6E0;void*_tmp6DF;(_tmp6DF=0,Cyc_fprintf(Cyc_stderr,((_tmp6E0="\n",
_tag_dyneither(_tmp6E0,sizeof(char),2))),_tag_dyneither(_tmp6DF,sizeof(void*),0)));};}
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct
Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(
0,d,tms->tl);const char*_tmp6E5;const char*_tmp6E4;struct Cyc_PP_Doc*_tmp6E3[3];
struct Cyc_PP_Doc*p_rest=(_tmp6E3[2]=Cyc_PP_text(((_tmp6E4=")",_tag_dyneither(
_tmp6E4,sizeof(char),2)))),((_tmp6E3[1]=rest,((_tmp6E3[0]=Cyc_PP_text(((_tmp6E5="(",
_tag_dyneither(_tmp6E5,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6E3,
sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpF4=(void*)tms->hd;union Cyc_Absyn_Constraint*
_tmpF6;struct Cyc_Absyn_Exp*_tmpF8;union Cyc_Absyn_Constraint*_tmpF9;void*_tmpFB;
struct Cyc_List_List*_tmpFD;struct Cyc_List_List*_tmpFF;struct Cyc_Position_Segment*
_tmp100;int _tmp101;struct Cyc_Absyn_PtrAtts _tmp103;void*_tmp104;union Cyc_Absyn_Constraint*
_tmp105;union Cyc_Absyn_Constraint*_tmp106;union Cyc_Absyn_Constraint*_tmp107;
struct Cyc_Absyn_Tqual _tmp108;_LL9C: {struct Cyc_Absyn_Carray_mod_struct*_tmpF5=(
struct Cyc_Absyn_Carray_mod_struct*)_tmpF4;if(_tmpF5->tag != 0)goto _LL9E;else{
_tmpF6=_tmpF5->f1;}}_LL9D: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6EA;const char*_tmp6E9;struct Cyc_PP_Doc*_tmp6E8[2];return(_tmp6E8[1]=((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF6)?Cyc_PP_text(((
_tmp6E9="[]@zeroterm",_tag_dyneither(_tmp6E9,sizeof(char),12)))): Cyc_PP_text(((
_tmp6EA="[]",_tag_dyneither(_tmp6EA,sizeof(char),3)))),((_tmp6E8[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp6E8,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpF7=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpF4;if(_tmpF7->tag != 1)goto
_LLA0;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;}}_LL9F: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp6F1;const char*_tmp6F0;const char*_tmp6EF;
struct Cyc_PP_Doc*_tmp6EE[4];return(_tmp6EE[3]=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpF9)?Cyc_PP_text(((_tmp6EF="]@zeroterm",
_tag_dyneither(_tmp6EF,sizeof(char),11)))): Cyc_PP_text(((_tmp6F0="]",
_tag_dyneither(_tmp6F0,sizeof(char),2)))),((_tmp6EE[2]=Cyc_Absynpp_exp2doc(
_tmpF8),((_tmp6EE[1]=Cyc_PP_text(((_tmp6F1="[",_tag_dyneither(_tmp6F1,sizeof(
char),2)))),((_tmp6EE[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6EE,sizeof(struct Cyc_PP_Doc*),
4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_struct*_tmpFA=(struct Cyc_Absyn_Function_mod_struct*)
_tmpF4;if(_tmpFA->tag != 3)goto _LLA2;else{_tmpFB=(void*)_tmpFA->f1;}}_LLA1: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmp110=_tmpFB;struct Cyc_List_List*_tmp112;int _tmp113;
struct Cyc_Absyn_VarargInfo*_tmp114;struct Cyc_Core_Opt*_tmp115;struct Cyc_List_List*
_tmp116;struct Cyc_List_List*_tmp118;struct Cyc_Position_Segment*_tmp119;_LLA9: {
struct Cyc_Absyn_WithTypes_struct*_tmp111=(struct Cyc_Absyn_WithTypes_struct*)
_tmp110;if(_tmp111->tag != 1)goto _LLAB;else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;
_tmp114=_tmp111->f3;_tmp115=_tmp111->f4;_tmp116=_tmp111->f5;}}_LLAA: {struct Cyc_PP_Doc*
_tmp6F2[2];return(_tmp6F2[1]=Cyc_Absynpp_funargs2doc(_tmp112,_tmp113,_tmp114,
_tmp115,_tmp116),((_tmp6F2[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F2,sizeof(
struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_struct*_tmp117=(
struct Cyc_Absyn_NoTypes_struct*)_tmp110;if(_tmp117->tag != 0)goto _LLA8;else{
_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LLAC: {const char*_tmp6F9;const char*
_tmp6F8;const char*_tmp6F7;struct Cyc_PP_Doc*_tmp6F6[2];return(_tmp6F6[1]=Cyc_PP_group(((
_tmp6F9="(",_tag_dyneither(_tmp6F9,sizeof(char),2))),((_tmp6F8=")",
_tag_dyneither(_tmp6F8,sizeof(char),2))),((_tmp6F7=",",_tag_dyneither(_tmp6F7,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp118)),((
_tmp6F6[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F6,sizeof(struct Cyc_PP_Doc*),2)))));}
_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_struct*_tmpFC=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpF4;if(_tmpFC->tag != 5)goto _LLA4;else{_tmpFD=_tmpFC->f2;}}_LLA3: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6FA[2];return(_tmp6FA[1]=Cyc_Absynpp_atts2doc(_tmpFD),((
_tmp6FA[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6FA,sizeof(struct Cyc_PP_Doc*),2)))));};
case Cyc_Cyclone_Vc_c: _LLAE: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp6FB[2];return(_tmp6FB[1]=rest,((_tmp6FB[0]=Cyc_Absynpp_callconv2doc(_tmpFD),
Cyc_PP_cat(_tag_dyneither(_tmp6FB,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;}
_LLA4: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpFE=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpF4;if(_tmpFE->tag != 4)goto _LLA6;else{_tmpFF=_tmpFE->f1;_tmp100=_tmpFE->f2;
_tmp101=_tmpFE->f3;}}_LLA5: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp101){struct Cyc_PP_Doc*_tmp6FC[2];return(_tmp6FC[1]=Cyc_Absynpp_ktvars2doc(
_tmpFF),((_tmp6FC[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6FC,sizeof(struct Cyc_PP_Doc*),
2)))));}else{struct Cyc_PP_Doc*_tmp6FD[2];return(_tmp6FD[1]=Cyc_Absynpp_tvars2doc(
_tmpFF),((_tmp6FD[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6FD,sizeof(struct Cyc_PP_Doc*),
2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_struct*_tmp102=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpF4;if(_tmp102->tag != 2)goto _LL9B;else{_tmp103=_tmp102->f1;_tmp104=_tmp103.rgn;
_tmp105=_tmp103.nullable;_tmp106=_tmp103.bounds;_tmp107=_tmp103.zero_term;
_tmp108=_tmp102->f2;}}_LLA7: {struct Cyc_PP_Doc*ptr;{void*_tmp123=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp106);struct Cyc_Absyn_Exp*_tmp126;_LLB1: {struct Cyc_Absyn_DynEither_b_struct*
_tmp124=(struct Cyc_Absyn_DynEither_b_struct*)_tmp123;if(_tmp124->tag != 0)goto
_LLB3;}_LLB2: ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_struct*
_tmp125=(struct Cyc_Absyn_Upper_b_struct*)_tmp123;if(_tmp125->tag != 1)goto _LLB0;
else{_tmp126=_tmp125->f1;}}_LLB4:{const char*_tmp6FF;const char*_tmp6FE;ptr=Cyc_PP_text(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp105)?(
_tmp6FF="*",_tag_dyneither(_tmp6FF,sizeof(char),2)):((_tmp6FE="@",_tag_dyneither(
_tmp6FE,sizeof(char),2))));}{unsigned int _tmp12A;int _tmp12B;struct _tuple11
_tmp129=Cyc_Evexp_eval_const_uint_exp(_tmp126);_tmp12A=_tmp129.f1;_tmp12B=
_tmp129.f2;if(!_tmp12B  || _tmp12A != 1){struct Cyc_PP_Doc*_tmp700[4];ptr=((_tmp700[
3]=Cyc_Absynpp_rb(),((_tmp700[2]=Cyc_Absynpp_exp2doc(_tmp126),((_tmp700[1]=Cyc_Absynpp_lb(),((
_tmp700[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp700,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;};_LLB0:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){const char*
_tmp703;struct Cyc_PP_Doc*_tmp702[2];ptr=((_tmp702[1]=Cyc_PP_text(((_tmp703="@zeroterm ",
_tag_dyneither(_tmp703,sizeof(char),11)))),((_tmp702[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp702,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){
const char*_tmp706;struct Cyc_PP_Doc*_tmp705[2];ptr=((_tmp705[1]=Cyc_PP_text(((
_tmp706="@nozeroterm ",_tag_dyneither(_tmp706,sizeof(char),13)))),((_tmp705[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp705,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmp131=Cyc_Tcutil_compress(_tmp104);_LLB6: {struct Cyc_Absyn_HeapRgn_struct*
_tmp132=(struct Cyc_Absyn_HeapRgn_struct*)_tmp131;if(_tmp132->tag != 20)goto _LLB8;}
_LLB7: goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_struct*_tmp133=(struct Cyc_Absyn_Evar_struct*)
_tmp131;if(_tmp133->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto
_LLBA;_LLB9: goto _LLB5;_LLBA:;_LLBB: {const char*_tmp709;struct Cyc_PP_Doc*_tmp708[
3];ptr=((_tmp708[2]=Cyc_PP_text(((_tmp709=" ",_tag_dyneither(_tmp709,sizeof(char),
2)))),((_tmp708[1]=Cyc_Absynpp_typ2doc(_tmp104),((_tmp708[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp708,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}{struct Cyc_PP_Doc*
_tmp70A[2];ptr=((_tmp70A[1]=Cyc_Absynpp_tqual2doc(_tmp108),((_tmp70A[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp70A,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp70B[2];return(_tmp70B[1]=rest,((_tmp70B[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp70B,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t){void*_tmp13B=Cyc_Tcutil_compress(t);_LLBD: {struct Cyc_Absyn_HeapRgn_struct*
_tmp13C=(struct Cyc_Absyn_HeapRgn_struct*)_tmp13B;if(_tmp13C->tag != 20)goto _LLBF;}
_LLBE: {const char*_tmp70C;return Cyc_PP_text(((_tmp70C="`H",_tag_dyneither(
_tmp70C,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_struct*_tmp13D=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp13B;if(_tmp13D->tag != 21)goto _LLC1;}_LLC0: {
const char*_tmp70D;return Cyc_PP_text(((_tmp70D="`U",_tag_dyneither(_tmp70D,
sizeof(char),3))));}_LLC1: {struct Cyc_Absyn_RefCntRgn_struct*_tmp13E=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp13B;if(_tmp13E->tag != 22)goto _LLC3;}_LLC2: {const char*_tmp70E;return Cyc_PP_text(((
_tmp70E="`RC",_tag_dyneither(_tmp70E,sizeof(char),4))));}_LLC3:;_LLC4: return Cyc_Absynpp_ntyp2doc(
t);_LLBC:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct
Cyc_List_List**effects,void*t){void*_tmp142=Cyc_Tcutil_compress(t);void*_tmp144;
struct Cyc_List_List*_tmp146;_LLC6: {struct Cyc_Absyn_AccessEff_struct*_tmp143=(
struct Cyc_Absyn_AccessEff_struct*)_tmp142;if(_tmp143->tag != 23)goto _LLC8;else{
_tmp144=(void*)_tmp143->f1;}}_LLC7:{struct Cyc_List_List*_tmp70F;*rgions=((
_tmp70F=_cycalloc(sizeof(*_tmp70F)),((_tmp70F->hd=Cyc_Absynpp_rgn2doc(_tmp144),((
_tmp70F->tl=*rgions,_tmp70F))))));}goto _LLC5;_LLC8: {struct Cyc_Absyn_JoinEff_struct*
_tmp145=(struct Cyc_Absyn_JoinEff_struct*)_tmp142;if(_tmp145->tag != 24)goto _LLCA;
else{_tmp146=_tmp145->f1;}}_LLC9: for(0;_tmp146 != 0;_tmp146=_tmp146->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp146->hd);}goto _LLC5;_LLCA:;_LLCB:{struct Cyc_List_List*
_tmp710;*effects=((_tmp710=_cycalloc(sizeof(*_tmp710)),((_tmp710->hd=Cyc_Absynpp_typ2doc(
t),((_tmp710->tl=*effects,_tmp710))))));}goto _LLC5;_LLC5:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(
void*t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0){const char*_tmp713;const char*_tmp712;const
char*_tmp711;return Cyc_PP_group(((_tmp711="",_tag_dyneither(_tmp711,sizeof(char),
1))),((_tmp712="",_tag_dyneither(_tmp712,sizeof(char),1))),((_tmp713="+",
_tag_dyneither(_tmp713,sizeof(char),2))),effects);}else{const char*_tmp714;struct
Cyc_PP_Doc*_tmp14C=Cyc_Absynpp_group_braces(((_tmp714=",",_tag_dyneither(_tmp714,
sizeof(char),2))),rgions);struct Cyc_List_List*_tmp718;const char*_tmp717;const
char*_tmp716;const char*_tmp715;return Cyc_PP_group(((_tmp715="",_tag_dyneither(
_tmp715,sizeof(char),1))),((_tmp716="",_tag_dyneither(_tmp716,sizeof(char),1))),((
_tmp717="+",_tag_dyneither(_tmp717,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp718=_cycalloc(sizeof(*_tmp718)),((_tmp718->hd=_tmp14C,((_tmp718->tl=0,
_tmp718))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind
k){switch(k){case Cyc_Absyn_StructA: _LLCC: {const char*_tmp719;return Cyc_PP_text(((
_tmp719="struct ",_tag_dyneither(_tmp719,sizeof(char),8))));}case Cyc_Absyn_UnionA:
_LLCD: {const char*_tmp71A;return Cyc_PP_text(((_tmp71A="union ",_tag_dyneither(
_tmp71A,sizeof(char),7))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;{void*_tmp154=t;struct Cyc_Core_Opt*_tmp15A;struct Cyc_Core_Opt*
_tmp15B;int _tmp15C;struct Cyc_Core_Opt*_tmp15D;struct Cyc_Absyn_Tvar*_tmp15F;
struct Cyc_Absyn_DatatypeInfo _tmp161;union Cyc_Absyn_DatatypeInfoU _tmp162;struct
Cyc_List_List*_tmp163;struct Cyc_Absyn_DatatypeFieldInfo _tmp165;union Cyc_Absyn_DatatypeFieldInfoU
_tmp166;struct Cyc_List_List*_tmp167;enum Cyc_Absyn_Sign _tmp169;enum Cyc_Absyn_Size_of
_tmp16A;int _tmp16C;struct Cyc_List_List*_tmp16E;struct Cyc_Absyn_AggrInfo _tmp170;
union Cyc_Absyn_AggrInfoU _tmp171;struct Cyc_List_List*_tmp172;enum Cyc_Absyn_AggrKind
_tmp174;struct Cyc_List_List*_tmp175;struct Cyc_List_List*_tmp177;struct _tuple0*
_tmp179;struct Cyc_Absyn_Exp*_tmp17B;struct _tuple0*_tmp17D;struct Cyc_List_List*
_tmp17E;struct Cyc_Absyn_Typedefdecl*_tmp17F;void*_tmp181;void*_tmp183;void*
_tmp184;void*_tmp186;void*_tmp18B;struct Cyc_Absyn_TypeDecl*_tmp18F;struct Cyc_Absyn_TypeDecl
_tmp190;void*_tmp191;struct Cyc_Absyn_Aggrdecl*_tmp193;struct Cyc_Absyn_TypeDecl*
_tmp195;struct Cyc_Absyn_TypeDecl _tmp196;void*_tmp197;struct Cyc_Absyn_Enumdecl*
_tmp199;struct Cyc_Absyn_TypeDecl*_tmp19B;struct Cyc_Absyn_TypeDecl _tmp19C;void*
_tmp19D;struct Cyc_Absyn_Datatypedecl*_tmp19F;_LLD0: {struct Cyc_Absyn_ArrayType_struct*
_tmp155=(struct Cyc_Absyn_ArrayType_struct*)_tmp154;if(_tmp155->tag != 8)goto _LLD2;}
_LLD1: {const char*_tmp71B;return Cyc_PP_text(((_tmp71B="[[[array]]]",
_tag_dyneither(_tmp71B,sizeof(char),12))));}_LLD2: {struct Cyc_Absyn_FnType_struct*
_tmp156=(struct Cyc_Absyn_FnType_struct*)_tmp154;if(_tmp156->tag != 9)goto _LLD4;}
_LLD3: return Cyc_PP_nil_doc();_LLD4: {struct Cyc_Absyn_PointerType_struct*_tmp157=(
struct Cyc_Absyn_PointerType_struct*)_tmp154;if(_tmp157->tag != 5)goto _LLD6;}_LLD5:
return Cyc_PP_nil_doc();_LLD6: {struct Cyc_Absyn_VoidType_struct*_tmp158=(struct
Cyc_Absyn_VoidType_struct*)_tmp154;if(_tmp158->tag != 0)goto _LLD8;}_LLD7:{const
char*_tmp71C;s=Cyc_PP_text(((_tmp71C="void",_tag_dyneither(_tmp71C,sizeof(char),
5))));}goto _LLCF;_LLD8: {struct Cyc_Absyn_Evar_struct*_tmp159=(struct Cyc_Absyn_Evar_struct*)
_tmp154;if(_tmp159->tag != 1)goto _LLDA;else{_tmp15A=_tmp159->f1;_tmp15B=_tmp159->f2;
_tmp15C=_tmp159->f3;_tmp15D=_tmp159->f4;}}_LLD9: if(_tmp15B != 0)return Cyc_Absynpp_ntyp2doc((
void*)_tmp15B->v);else{const char*_tmp72C;const char*_tmp72B;const char*_tmp72A;
struct Cyc_Int_pa_struct _tmp729;void*_tmp728[1];const char*_tmp727;const char*
_tmp726;struct Cyc_PP_Doc*_tmp725[6];s=((_tmp725[5]=_tmp15A == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp15A->v),((_tmp725[4]=Cyc_PP_text(((
_tmp726=")::",_tag_dyneither(_tmp726,sizeof(char),4)))),((_tmp725[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp15D == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp15D->v),((_tmp725[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp729.tag=1,((
_tmp729.f1=(unsigned long)_tmp15C,((_tmp728[0]=& _tmp729,Cyc_aprintf(((_tmp727="%d",
_tag_dyneither(_tmp727,sizeof(char),3))),_tag_dyneither(_tmp728,sizeof(void*),1))))))))),((
_tmp725[1]=Cyc_PP_text(((_tmp72A="(",_tag_dyneither(_tmp72A,sizeof(char),2)))),((
_tmp725[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp72B="\\%",
_tag_dyneither(_tmp72B,sizeof(char),3)))): Cyc_PP_text(((_tmp72C="%",
_tag_dyneither(_tmp72C,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp725,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLCF;_LLDA: {struct Cyc_Absyn_VarType_struct*
_tmp15E=(struct Cyc_Absyn_VarType_struct*)_tmp154;if(_tmp15E->tag != 2)goto _LLDC;
else{_tmp15F=_tmp15E->f1;}}_LLDB: s=Cyc_PP_textptr(_tmp15F->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp72F;struct Cyc_PP_Doc*_tmp72E[3];s=((_tmp72E[2]=Cyc_Absynpp_kindbound2doc(
_tmp15F->kind),((_tmp72E[1]=Cyc_PP_text(((_tmp72F="::",_tag_dyneither(_tmp72F,
sizeof(char),3)))),((_tmp72E[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp72E,sizeof(
struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp15F)){const char*_tmp734;const char*_tmp733;struct Cyc_PP_Doc*_tmp732[3];s=((
_tmp732[2]=Cyc_PP_text(((_tmp733=" */",_tag_dyneither(_tmp733,sizeof(char),4)))),((
_tmp732[1]=s,((_tmp732[0]=Cyc_PP_text(((_tmp734="_ /* ",_tag_dyneither(_tmp734,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp732,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCF;_LLDC: {struct Cyc_Absyn_DatatypeType_struct*_tmp160=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp154;if(_tmp160->tag != 3)goto _LLDE;else{_tmp161=_tmp160->f1;_tmp162=_tmp161.datatype_info;
_tmp163=_tmp161.targs;}}_LLDD:{union Cyc_Absyn_DatatypeInfoU _tmp1AF=_tmp162;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp1B0;struct _tuple0*_tmp1B1;int _tmp1B2;
struct Cyc_Absyn_Datatypedecl**_tmp1B3;struct Cyc_Absyn_Datatypedecl*_tmp1B4;
struct Cyc_Absyn_Datatypedecl _tmp1B5;struct _tuple0*_tmp1B6;int _tmp1B7;_LL10B: if((
_tmp1AF.UnknownDatatype).tag != 1)goto _LL10D;_tmp1B0=(struct Cyc_Absyn_UnknownDatatypeInfo)(
_tmp1AF.UnknownDatatype).val;_tmp1B1=_tmp1B0.name;_tmp1B2=_tmp1B0.is_extensible;
_LL10C: _tmp1B6=_tmp1B1;_tmp1B7=_tmp1B2;goto _LL10E;_LL10D: if((_tmp1AF.KnownDatatype).tag
!= 2)goto _LL10A;_tmp1B3=(struct Cyc_Absyn_Datatypedecl**)(_tmp1AF.KnownDatatype).val;
_tmp1B4=*_tmp1B3;_tmp1B5=*_tmp1B4;_tmp1B6=_tmp1B5.name;_tmp1B7=_tmp1B5.is_extensible;
_LL10E: {const char*_tmp735;struct Cyc_PP_Doc*_tmp1B8=Cyc_PP_text(((_tmp735="datatype ",
_tag_dyneither(_tmp735,sizeof(char),10))));const char*_tmp736;struct Cyc_PP_Doc*
_tmp1B9=_tmp1B7?Cyc_PP_text(((_tmp736="@extensible ",_tag_dyneither(_tmp736,
sizeof(char),13)))): Cyc_PP_nil_doc();{struct Cyc_PP_Doc*_tmp737[4];s=((_tmp737[3]=
Cyc_Absynpp_tps2doc(_tmp163),((_tmp737[2]=Cyc_Absynpp_qvar2doc(_tmp1B6),((
_tmp737[1]=_tmp1B8,((_tmp737[0]=_tmp1B9,Cyc_PP_cat(_tag_dyneither(_tmp737,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL10A;}_LL10A:;}goto _LLCF;_LLDE: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp164=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp154;if(_tmp164->tag != 4)goto _LLE0;else{_tmp165=_tmp164->f1;_tmp166=_tmp165.field_info;
_tmp167=_tmp165.targs;}}_LLDF:{union Cyc_Absyn_DatatypeFieldInfoU _tmp1BD=_tmp166;
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp1BE;struct _tuple0*_tmp1BF;struct
_tuple0*_tmp1C0;int _tmp1C1;struct _tuple1 _tmp1C2;struct Cyc_Absyn_Datatypedecl*
_tmp1C3;struct Cyc_Absyn_Datatypedecl _tmp1C4;struct _tuple0*_tmp1C5;int _tmp1C6;
struct Cyc_Absyn_Datatypefield*_tmp1C7;struct Cyc_Absyn_Datatypefield _tmp1C8;
struct _tuple0*_tmp1C9;_LL110: if((_tmp1BD.UnknownDatatypefield).tag != 1)goto
_LL112;_tmp1BE=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1BD.UnknownDatatypefield).val;
_tmp1BF=_tmp1BE.datatype_name;_tmp1C0=_tmp1BE.field_name;_tmp1C1=_tmp1BE.is_extensible;
_LL111: _tmp1C5=_tmp1BF;_tmp1C6=_tmp1C1;_tmp1C9=_tmp1C0;goto _LL113;_LL112: if((
_tmp1BD.KnownDatatypefield).tag != 2)goto _LL10F;_tmp1C2=(struct _tuple1)(_tmp1BD.KnownDatatypefield).val;
_tmp1C3=_tmp1C2.f1;_tmp1C4=*_tmp1C3;_tmp1C5=_tmp1C4.name;_tmp1C6=_tmp1C4.is_extensible;
_tmp1C7=_tmp1C2.f2;_tmp1C8=*_tmp1C7;_tmp1C9=_tmp1C8.name;_LL113: {const char*
_tmp739;const char*_tmp738;struct Cyc_PP_Doc*_tmp1CA=Cyc_PP_text(_tmp1C6?(_tmp739="@extensible datatype ",
_tag_dyneither(_tmp739,sizeof(char),22)):((_tmp738="datatype ",_tag_dyneither(
_tmp738,sizeof(char),10))));{const char*_tmp73C;struct Cyc_PP_Doc*_tmp73B[4];s=((
_tmp73B[3]=Cyc_Absynpp_qvar2doc(_tmp1C9),((_tmp73B[2]=Cyc_PP_text(((_tmp73C=".",
_tag_dyneither(_tmp73C,sizeof(char),2)))),((_tmp73B[1]=Cyc_Absynpp_qvar2doc(
_tmp1C5),((_tmp73B[0]=_tmp1CA,Cyc_PP_cat(_tag_dyneither(_tmp73B,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL10F;}_LL10F:;}goto _LLCF;_LLE0: {struct Cyc_Absyn_IntType_struct*
_tmp168=(struct Cyc_Absyn_IntType_struct*)_tmp154;if(_tmp168->tag != 6)goto _LLE2;
else{_tmp169=_tmp168->f1;_tmp16A=_tmp168->f2;}}_LLE1: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;switch(_tmp169){case Cyc_Absyn_None: _LL114: goto _LL115;case
Cyc_Absyn_Signed: _LL115:{const char*_tmp73D;sns=((_tmp73D="",_tag_dyneither(
_tmp73D,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL116:{const char*
_tmp73E;sns=((_tmp73E="unsigned ",_tag_dyneither(_tmp73E,sizeof(char),10)));}
break;}switch(_tmp16A){case Cyc_Absyn_Char_sz: _LL118: switch(_tmp169){case Cyc_Absyn_None:
_LL11A:{const char*_tmp73F;sns=((_tmp73F="",_tag_dyneither(_tmp73F,sizeof(char),1)));}
break;case Cyc_Absyn_Signed: _LL11B:{const char*_tmp740;sns=((_tmp740="signed ",
_tag_dyneither(_tmp740,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL11C:{
const char*_tmp741;sns=((_tmp741="unsigned ",_tag_dyneither(_tmp741,sizeof(char),
10)));}break;}{const char*_tmp742;ts=((_tmp742="char",_tag_dyneither(_tmp742,
sizeof(char),5)));}break;case Cyc_Absyn_Short_sz: _LL119:{const char*_tmp743;ts=((
_tmp743="short",_tag_dyneither(_tmp743,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz:
_LL11E:{const char*_tmp744;ts=((_tmp744="int",_tag_dyneither(_tmp744,sizeof(char),
4)));}break;case Cyc_Absyn_Long_sz: _LL11F:{const char*_tmp745;ts=((_tmp745="long",
_tag_dyneither(_tmp745,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL120:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL122:{const char*_tmp746;
ts=((_tmp746="long long",_tag_dyneither(_tmp746,sizeof(char),10)));}break;case
Cyc_Cyclone_Vc_c: _LL123:{const char*_tmp747;ts=((_tmp747="__int64",_tag_dyneither(
_tmp747,sizeof(char),8)));}break;}break;}{const char*_tmp74C;void*_tmp74B[2];
struct Cyc_String_pa_struct _tmp74A;struct Cyc_String_pa_struct _tmp749;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp749.tag=0,((_tmp749.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ts),((_tmp74A.tag=0,((_tmp74A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sns),((_tmp74B[0]=& _tmp74A,((_tmp74B[1]=& _tmp749,Cyc_aprintf(((
_tmp74C="%s%s",_tag_dyneither(_tmp74C,sizeof(char),5))),_tag_dyneither(_tmp74B,
sizeof(void*),2)))))))))))))));}goto _LLCF;}_LLE2: {struct Cyc_Absyn_FloatType_struct*
_tmp16B=(struct Cyc_Absyn_FloatType_struct*)_tmp154;if(_tmp16B->tag != 7)goto _LLE4;
else{_tmp16C=_tmp16B->f1;}}_LLE3: if(_tmp16C == 0){const char*_tmp74D;s=Cyc_PP_text(((
_tmp74D="float",_tag_dyneither(_tmp74D,sizeof(char),6))));}else{if(_tmp16C == 1){
const char*_tmp74E;s=Cyc_PP_text(((_tmp74E="double",_tag_dyneither(_tmp74E,
sizeof(char),7))));}else{const char*_tmp74F;s=Cyc_PP_text(((_tmp74F="long double",
_tag_dyneither(_tmp74F,sizeof(char),12))));}}goto _LLCF;_LLE4: {struct Cyc_Absyn_TupleType_struct*
_tmp16D=(struct Cyc_Absyn_TupleType_struct*)_tmp154;if(_tmp16D->tag != 10)goto
_LLE6;else{_tmp16E=_tmp16D->f1;}}_LLE5:{struct Cyc_PP_Doc*_tmp750[2];s=((_tmp750[
1]=Cyc_Absynpp_args2doc(_tmp16E),((_tmp750[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp750,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCF;_LLE6: {
struct Cyc_Absyn_AggrType_struct*_tmp16F=(struct Cyc_Absyn_AggrType_struct*)
_tmp154;if(_tmp16F->tag != 11)goto _LLE8;else{_tmp170=_tmp16F->f1;_tmp171=_tmp170.aggr_info;
_tmp172=_tmp170.targs;}}_LLE7: {enum Cyc_Absyn_AggrKind _tmp1E3;struct _tuple0*
_tmp1E4;struct _tuple10 _tmp1E2=Cyc_Absyn_aggr_kinded_name(_tmp171);_tmp1E3=
_tmp1E2.f1;_tmp1E4=_tmp1E2.f2;{struct Cyc_PP_Doc*_tmp751[3];s=((_tmp751[2]=Cyc_Absynpp_tps2doc(
_tmp172),((_tmp751[1]=Cyc_Absynpp_qvar2doc(_tmp1E4),((_tmp751[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp1E3),Cyc_PP_cat(_tag_dyneither(_tmp751,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCF;}_LLE8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp173=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp154;if(_tmp173->tag != 12)goto _LLEA;else{_tmp174=_tmp173->f1;_tmp175=_tmp173->f2;}}
_LLE9:{struct Cyc_PP_Doc*_tmp752[4];s=((_tmp752[3]=Cyc_Absynpp_rb(),((_tmp752[2]=
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp175)),((_tmp752[1]=Cyc_Absynpp_lb(),((
_tmp752[0]=Cyc_Absynpp_aggr_kind2doc(_tmp174),Cyc_PP_cat(_tag_dyneither(_tmp752,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCF;_LLEA: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp176=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp154;if(_tmp176->tag != 14)goto
_LLEC;else{_tmp177=_tmp176->f1;}}_LLEB:{const char*_tmp755;struct Cyc_PP_Doc*
_tmp754[4];s=((_tmp754[3]=Cyc_Absynpp_rb(),((_tmp754[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(
_tmp177)),((_tmp754[1]=Cyc_Absynpp_lb(),((_tmp754[0]=Cyc_PP_text(((_tmp755="enum ",
_tag_dyneither(_tmp755,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp754,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCF;_LLEC: {struct Cyc_Absyn_EnumType_struct*
_tmp178=(struct Cyc_Absyn_EnumType_struct*)_tmp154;if(_tmp178->tag != 13)goto _LLEE;
else{_tmp179=_tmp178->f1;}}_LLED:{const char*_tmp758;struct Cyc_PP_Doc*_tmp757[2];
s=((_tmp757[1]=Cyc_Absynpp_qvar2doc(_tmp179),((_tmp757[0]=Cyc_PP_text(((_tmp758="enum ",
_tag_dyneither(_tmp758,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp757,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCF;_LLEE: {struct Cyc_Absyn_ValueofType_struct*
_tmp17A=(struct Cyc_Absyn_ValueofType_struct*)_tmp154;if(_tmp17A->tag != 18)goto
_LLF0;else{_tmp17B=_tmp17A->f1;}}_LLEF:{const char*_tmp75D;const char*_tmp75C;
struct Cyc_PP_Doc*_tmp75B[3];s=((_tmp75B[2]=Cyc_PP_text(((_tmp75C=")",
_tag_dyneither(_tmp75C,sizeof(char),2)))),((_tmp75B[1]=Cyc_Absynpp_exp2doc(
_tmp17B),((_tmp75B[0]=Cyc_PP_text(((_tmp75D="valueof_t(",_tag_dyneither(_tmp75D,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp75B,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLCF;_LLF0: {struct Cyc_Absyn_TypedefType_struct*_tmp17C=(struct
Cyc_Absyn_TypedefType_struct*)_tmp154;if(_tmp17C->tag != 17)goto _LLF2;else{
_tmp17D=_tmp17C->f1;_tmp17E=_tmp17C->f2;_tmp17F=_tmp17C->f3;}}_LLF1:{struct Cyc_PP_Doc*
_tmp75E[2];s=((_tmp75E[1]=Cyc_Absynpp_tps2doc(_tmp17E),((_tmp75E[0]=Cyc_Absynpp_qvar2doc(
_tmp17D),Cyc_PP_cat(_tag_dyneither(_tmp75E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLCF;_LLF2: {struct Cyc_Absyn_RgnHandleType_struct*_tmp180=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp154;if(_tmp180->tag != 15)goto _LLF4;else{_tmp181=(void*)_tmp180->f1;}}_LLF3:{
const char*_tmp763;const char*_tmp762;struct Cyc_PP_Doc*_tmp761[3];s=((_tmp761[2]=
Cyc_PP_text(((_tmp762=">",_tag_dyneither(_tmp762,sizeof(char),2)))),((_tmp761[1]=
Cyc_Absynpp_rgn2doc(_tmp181),((_tmp761[0]=Cyc_PP_text(((_tmp763="region_t<",
_tag_dyneither(_tmp763,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp761,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCF;_LLF4: {struct Cyc_Absyn_DynRgnType_struct*
_tmp182=(struct Cyc_Absyn_DynRgnType_struct*)_tmp154;if(_tmp182->tag != 16)goto
_LLF6;else{_tmp183=(void*)_tmp182->f1;_tmp184=(void*)_tmp182->f2;}}_LLF5:{const
char*_tmp76A;const char*_tmp769;const char*_tmp768;struct Cyc_PP_Doc*_tmp767[5];s=((
_tmp767[4]=Cyc_PP_text(((_tmp768=">",_tag_dyneither(_tmp768,sizeof(char),2)))),((
_tmp767[3]=Cyc_Absynpp_rgn2doc(_tmp184),((_tmp767[2]=Cyc_PP_text(((_tmp769=",",
_tag_dyneither(_tmp769,sizeof(char),2)))),((_tmp767[1]=Cyc_Absynpp_rgn2doc(
_tmp183),((_tmp767[0]=Cyc_PP_text(((_tmp76A="dynregion_t<",_tag_dyneither(
_tmp76A,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp767,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLCF;_LLF6: {struct Cyc_Absyn_TagType_struct*_tmp185=(struct
Cyc_Absyn_TagType_struct*)_tmp154;if(_tmp185->tag != 19)goto _LLF8;else{_tmp186=(
void*)_tmp185->f1;}}_LLF7:{const char*_tmp76F;const char*_tmp76E;struct Cyc_PP_Doc*
_tmp76D[3];s=((_tmp76D[2]=Cyc_PP_text(((_tmp76E=">",_tag_dyneither(_tmp76E,
sizeof(char),2)))),((_tmp76D[1]=Cyc_Absynpp_typ2doc(_tmp186),((_tmp76D[0]=Cyc_PP_text(((
_tmp76F="tag_t<",_tag_dyneither(_tmp76F,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp76D,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCF;_LLF8: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp187=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp154;if(_tmp187->tag != 21)goto _LLFA;}_LLF9: goto _LLFB;_LLFA: {struct Cyc_Absyn_HeapRgn_struct*
_tmp188=(struct Cyc_Absyn_HeapRgn_struct*)_tmp154;if(_tmp188->tag != 20)goto _LLFC;}
_LLFB: goto _LLFD;_LLFC: {struct Cyc_Absyn_RefCntRgn_struct*_tmp189=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp154;if(_tmp189->tag != 22)goto _LLFE;}_LLFD: s=Cyc_Absynpp_rgn2doc(t);goto _LLCF;
_LLFE: {struct Cyc_Absyn_RgnsEff_struct*_tmp18A=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp154;if(_tmp18A->tag != 25)goto _LL100;else{_tmp18B=(void*)_tmp18A->f1;}}_LLFF:{
const char*_tmp774;const char*_tmp773;struct Cyc_PP_Doc*_tmp772[3];s=((_tmp772[2]=
Cyc_PP_text(((_tmp773=")",_tag_dyneither(_tmp773,sizeof(char),2)))),((_tmp772[1]=
Cyc_Absynpp_typ2doc(_tmp18B),((_tmp772[0]=Cyc_PP_text(((_tmp774="regions(",
_tag_dyneither(_tmp774,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp772,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCF;_LL100: {struct Cyc_Absyn_AccessEff_struct*
_tmp18C=(struct Cyc_Absyn_AccessEff_struct*)_tmp154;if(_tmp18C->tag != 23)goto
_LL102;}_LL101: goto _LL103;_LL102: {struct Cyc_Absyn_JoinEff_struct*_tmp18D=(
struct Cyc_Absyn_JoinEff_struct*)_tmp154;if(_tmp18D->tag != 24)goto _LL104;}_LL103:
s=Cyc_Absynpp_eff2doc(t);goto _LLCF;_LL104: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp18E=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp154;if(_tmp18E->tag != 26)goto
_LL106;else{_tmp18F=_tmp18E->f1;_tmp190=*_tmp18F;_tmp191=_tmp190.r;{struct Cyc_Absyn_Aggr_td_struct*
_tmp192=(struct Cyc_Absyn_Aggr_td_struct*)_tmp191;if(_tmp192->tag != 0)goto _LL106;
else{_tmp193=_tmp192->f1;}};}}_LL105: s=Cyc_Absynpp_aggrdecl2doc(_tmp193);goto
_LLCF;_LL106: {struct Cyc_Absyn_TypeDeclType_struct*_tmp194=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp154;if(_tmp194->tag != 26)goto _LL108;else{_tmp195=_tmp194->f1;_tmp196=*
_tmp195;_tmp197=_tmp196.r;{struct Cyc_Absyn_Enum_td_struct*_tmp198=(struct Cyc_Absyn_Enum_td_struct*)
_tmp197;if(_tmp198->tag != 1)goto _LL108;else{_tmp199=_tmp198->f1;}};}}_LL107: s=
Cyc_Absynpp_enumdecl2doc(_tmp199);goto _LLCF;_LL108: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp19A=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp154;if(_tmp19A->tag != 26)goto
_LLCF;else{_tmp19B=_tmp19A->f1;_tmp19C=*_tmp19B;_tmp19D=_tmp19C.r;{struct Cyc_Absyn_Datatype_td_struct*
_tmp19E=(struct Cyc_Absyn_Datatype_td_struct*)_tmp19D;if(_tmp19E->tag != 2)goto
_LLCF;else{_tmp19F=_tmp19E->f1;}};}}_LL109: s=Cyc_Absynpp_datatypedecl2doc(
_tmp19F);goto _LLCF;_LLCF:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct
Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct
_dyneither_ptr*)vo->v));}struct _tuple17{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(
struct _tuple17*cmp){struct _tuple17 _tmp1FD;void*_tmp1FE;void*_tmp1FF;struct
_tuple17*_tmp1FC=cmp;_tmp1FD=*_tmp1FC;_tmp1FE=_tmp1FD.f1;_tmp1FF=_tmp1FD.f2;{
const char*_tmp777;struct Cyc_PP_Doc*_tmp776[3];return(_tmp776[2]=Cyc_Absynpp_rgn2doc(
_tmp1FF),((_tmp776[1]=Cyc_PP_text(((_tmp777=" > ",_tag_dyneither(_tmp777,sizeof(
char),4)))),((_tmp776[0]=Cyc_Absynpp_rgn2doc(_tmp1FE),Cyc_PP_cat(_tag_dyneither(
_tmp776,sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List*po){const char*_tmp77A;const char*_tmp779;const char*_tmp778;
return Cyc_PP_group(((_tmp778="",_tag_dyneither(_tmp778,sizeof(char),1))),((
_tmp779="",_tag_dyneither(_tmp779,sizeof(char),1))),((_tmp77A=",",_tag_dyneither(
_tmp77A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){struct Cyc_Core_Opt*
_tmp77B;struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:((_tmp77B=_cycalloc(sizeof(*
_tmp77B)),((_tmp77B->v=Cyc_PP_text(*((struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v)),_tmp77B))));return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp206=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs){const char*_tmp77E;struct Cyc_List_List*
_tmp77D;_tmp206=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp206,((_tmp77D=_cycalloc(sizeof(*_tmp77D)),((_tmp77D->hd=
Cyc_PP_text(((_tmp77E="...",_tag_dyneither(_tmp77E,sizeof(char),4)))),((_tmp77D->tl=
0,_tmp77D)))))));}else{if(cyc_varargs != 0){const char*_tmp787;const char*_tmp786;
const char*_tmp785;struct _tuple8*_tmp784;struct Cyc_PP_Doc*_tmp783[3];struct Cyc_PP_Doc*
_tmp209=(_tmp783[2]=Cyc_Absynpp_funarg2doc(((_tmp784=_cycalloc(sizeof(*_tmp784)),((
_tmp784->f1=cyc_varargs->name,((_tmp784->f2=cyc_varargs->tq,((_tmp784->f3=
cyc_varargs->type,_tmp784))))))))),((_tmp783[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp785=" inject ",_tag_dyneither(_tmp785,sizeof(char),9)))): Cyc_PP_text(((
_tmp786=" ",_tag_dyneither(_tmp786,sizeof(char),2)))),((_tmp783[0]=Cyc_PP_text(((
_tmp787="...",_tag_dyneither(_tmp787,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp783,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp788;_tmp206=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp206,((_tmp788=_cycalloc(sizeof(*_tmp788)),((_tmp788->hd=
_tmp209,((_tmp788->tl=0,_tmp788)))))));}}{const char*_tmp78B;const char*_tmp78A;
const char*_tmp789;struct Cyc_PP_Doc*_tmp210=Cyc_PP_group(((_tmp789="",
_tag_dyneither(_tmp789,sizeof(char),1))),((_tmp78A="",_tag_dyneither(_tmp78A,
sizeof(char),1))),((_tmp78B=",",_tag_dyneither(_tmp78B,sizeof(char),2))),_tmp206);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp78E;struct Cyc_PP_Doc*
_tmp78D[3];_tmp210=((_tmp78D[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp78D[
1]=Cyc_PP_text(((_tmp78E=";",_tag_dyneither(_tmp78E,sizeof(char),2)))),((_tmp78D[
0]=_tmp210,Cyc_PP_cat(_tag_dyneither(_tmp78D,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp791;struct Cyc_PP_Doc*_tmp790[3];_tmp210=((_tmp790[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp790[1]=Cyc_PP_text(((_tmp791=":",
_tag_dyneither(_tmp791,sizeof(char),2)))),((_tmp790[0]=_tmp210,Cyc_PP_cat(
_tag_dyneither(_tmp790,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp796;
const char*_tmp795;struct Cyc_PP_Doc*_tmp794[3];return(_tmp794[2]=Cyc_PP_text(((
_tmp795=")",_tag_dyneither(_tmp795,sizeof(char),2)))),((_tmp794[1]=_tmp210,((
_tmp794[0]=Cyc_PP_text(((_tmp796="(",_tag_dyneither(_tmp796,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp794,sizeof(struct Cyc_PP_Doc*),3)))))));};};}struct
_tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple12*arg){struct Cyc_Core_Opt*_tmp799;
struct _tuple8*_tmp798;return(_tmp798=_cycalloc(sizeof(*_tmp798)),((_tmp798->f1=((
_tmp799=_cycalloc(sizeof(*_tmp799)),((_tmp799->v=(*arg).f1,_tmp799)))),((_tmp798->f2=(*
arg).f2,((_tmp798->f3=(*arg).f3,_tmp798)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(
struct _dyneither_ptr*v){return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*q){struct Cyc_List_List*_tmp21D=0;int match;{union Cyc_Absyn_Nmspace
_tmp21E=(*q).f1;int _tmp21F;struct Cyc_List_List*_tmp220;struct Cyc_List_List*
_tmp221;struct Cyc_List_List*_tmp222;_LL126: if((_tmp21E.Loc_n).tag != 4)goto _LL128;
_tmp21F=(int)(_tmp21E.Loc_n).val;_LL127: _tmp220=0;goto _LL129;_LL128: if((_tmp21E.Rel_n).tag
!= 1)goto _LL12A;_tmp220=(struct Cyc_List_List*)(_tmp21E.Rel_n).val;_LL129: match=0;
_tmp21D=_tmp220;goto _LL125;_LL12A: if((_tmp21E.C_n).tag != 3)goto _LL12C;_tmp221=(
struct Cyc_List_List*)(_tmp21E.C_n).val;_LL12B: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp221,Cyc_Absynpp_curr_namespace);
goto _LL125;_LL12C: if((_tmp21E.Abs_n).tag != 2)goto _LL125;_tmp222=(struct Cyc_List_List*)(
_tmp21E.Abs_n).val;_LL12D: match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp222,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp79A;_tmp21D=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp79A=_cycalloc(sizeof(*_tmp79A)),((_tmp79A->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp79A->tl=_tmp222,_tmp79A))))): _tmp222;}goto _LL125;_LL125:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp79C;struct Cyc_List_List*_tmp79B;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp21D,((_tmp79B=_cycalloc(sizeof(*_tmp79B)),((_tmp79B->hd=(*q).f2,((_tmp79B->tl=
0,_tmp79B))))))),((_tmp79C="_",_tag_dyneither(_tmp79C,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp79E;struct Cyc_List_List*_tmp79D;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp21D,((_tmp79D=_cycalloc(sizeof(*
_tmp79D)),((_tmp79D->hd=(*q).f2,((_tmp79D->tl=0,_tmp79D))))))),((_tmp79E="::",
_tag_dyneither(_tmp79E,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*
Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct _dyneither_ptr _tmp228=Cyc_Absynpp_qvar2string(
q);if(Cyc_PP_tex_output){const char*_tmp7A0;const char*_tmp79F;return Cyc_PP_text_width((
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp79F="\\textbf{",
_tag_dyneither(_tmp79F,sizeof(char),9))),(struct _dyneither_ptr)_tmp228),((
_tmp7A0="}",_tag_dyneither(_tmp7A0,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp228));}else{return Cyc_PP_text(_tmp228);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)
return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace
_tmp22B=(*v).f1;int _tmp22C;struct Cyc_List_List*_tmp22D;struct Cyc_List_List*
_tmp22E;struct Cyc_List_List*_tmp22F;_LL12F: if((_tmp22B.Loc_n).tag != 4)goto _LL131;
_tmp22C=(int)(_tmp22B.Loc_n).val;_LL130: goto _LL132;_LL131: if((_tmp22B.Rel_n).tag
!= 1)goto _LL133;_tmp22D=(struct Cyc_List_List*)(_tmp22B.Rel_n).val;if(_tmp22D != 0)
goto _LL133;_LL132: return*(*v).f2;_LL133: if((_tmp22B.C_n).tag != 3)goto _LL135;
_tmp22E=(struct Cyc_List_List*)(_tmp22B.C_n).val;_LL134: _tmp22F=_tmp22E;goto
_LL136;_LL135: if((_tmp22B.Abs_n).tag != 2)goto _LL137;_tmp22F=(struct Cyc_List_List*)(
_tmp22B.Abs_n).val;_LL136: if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_strptrcmp,_tmp22F,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto
_LL138;}_LL137:;_LL138: {const char*_tmp7A1;return(struct _dyneither_ptr)Cyc_strconcat(((
_tmp7A1="/* bad namespace : */ ",_tag_dyneither(_tmp7A1,sizeof(char),23))),(
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL12E:;}else{return*(*v).f2;}}
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){return Cyc_PP_text(
Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v){struct _dyneither_ptr _tmp231=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){const char*_tmp7A3;const char*_tmp7A2;return Cyc_PP_text_width((
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp7A2="\\textbf{",
_tag_dyneither(_tmp7A2,sizeof(char),9))),(struct _dyneither_ptr)_tmp231),((
_tmp7A3="}",_tag_dyneither(_tmp7A3,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp231));}else{return Cyc_PP_text(_tmp231);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmp234=e->r;enum Cyc_Absyn_Primop _tmp239;struct Cyc_Absyn_Exp*
_tmp244;struct Cyc_Absyn_Exp*_tmp246;_LL13A: {struct Cyc_Absyn_Const_e_struct*
_tmp235=(struct Cyc_Absyn_Const_e_struct*)_tmp234;if(_tmp235->tag != 0)goto _LL13C;}
_LL13B: goto _LL13D;_LL13C: {struct Cyc_Absyn_Var_e_struct*_tmp236=(struct Cyc_Absyn_Var_e_struct*)
_tmp234;if(_tmp236->tag != 1)goto _LL13E;}_LL13D: goto _LL13F;_LL13E: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp237=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp234;if(_tmp237->tag != 2)goto
_LL140;}_LL13F: return 10000;_LL140: {struct Cyc_Absyn_Primop_e_struct*_tmp238=(
struct Cyc_Absyn_Primop_e_struct*)_tmp234;if(_tmp238->tag != 3)goto _LL142;else{
_tmp239=_tmp238->f1;}}_LL141: switch(_tmp239){case Cyc_Absyn_Plus: _LL18C: return 100;
case Cyc_Absyn_Times: _LL18D: return 110;case Cyc_Absyn_Minus: _LL18E: return 100;case
Cyc_Absyn_Div: _LL18F: goto _LL190;case Cyc_Absyn_Mod: _LL190: return 110;case Cyc_Absyn_Eq:
_LL191: goto _LL192;case Cyc_Absyn_Neq: _LL192: return 70;case Cyc_Absyn_Gt: _LL193: goto
_LL194;case Cyc_Absyn_Lt: _LL194: goto _LL195;case Cyc_Absyn_Gte: _LL195: goto _LL196;
case Cyc_Absyn_Lte: _LL196: return 80;case Cyc_Absyn_Not: _LL197: goto _LL198;case Cyc_Absyn_Bitnot:
_LL198: return 130;case Cyc_Absyn_Bitand: _LL199: return 60;case Cyc_Absyn_Bitor: _LL19A:
return 40;case Cyc_Absyn_Bitxor: _LL19B: return 50;case Cyc_Absyn_Bitlshift: _LL19C:
return 90;case Cyc_Absyn_Bitlrshift: _LL19D: return 80;case Cyc_Absyn_Bitarshift:
_LL19E: return 80;case Cyc_Absyn_Numelts: _LL19F: return 140;}_LL142: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp23A=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp234;if(_tmp23A->tag != 4)goto
_LL144;}_LL143: return 20;_LL144: {struct Cyc_Absyn_Increment_e_struct*_tmp23B=(
struct Cyc_Absyn_Increment_e_struct*)_tmp234;if(_tmp23B->tag != 5)goto _LL146;}
_LL145: return 130;_LL146: {struct Cyc_Absyn_Conditional_e_struct*_tmp23C=(struct
Cyc_Absyn_Conditional_e_struct*)_tmp234;if(_tmp23C->tag != 6)goto _LL148;}_LL147:
return 30;_LL148: {struct Cyc_Absyn_And_e_struct*_tmp23D=(struct Cyc_Absyn_And_e_struct*)
_tmp234;if(_tmp23D->tag != 7)goto _LL14A;}_LL149: return 35;_LL14A: {struct Cyc_Absyn_Or_e_struct*
_tmp23E=(struct Cyc_Absyn_Or_e_struct*)_tmp234;if(_tmp23E->tag != 8)goto _LL14C;}
_LL14B: return 30;_LL14C: {struct Cyc_Absyn_SeqExp_e_struct*_tmp23F=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp234;if(_tmp23F->tag != 9)goto _LL14E;}_LL14D: return 10;_LL14E: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp240=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp234;if(_tmp240->tag != 10)goto
_LL150;}_LL14F: goto _LL151;_LL150: {struct Cyc_Absyn_FnCall_e_struct*_tmp241=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp234;if(_tmp241->tag != 11)goto _LL152;}_LL151:
return 140;_LL152: {struct Cyc_Absyn_Throw_e_struct*_tmp242=(struct Cyc_Absyn_Throw_e_struct*)
_tmp234;if(_tmp242->tag != 12)goto _LL154;}_LL153: return 130;_LL154: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp243=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp234;if(_tmp243->tag != 13)
goto _LL156;else{_tmp244=_tmp243->f1;}}_LL155: return Cyc_Absynpp_exp_prec(_tmp244);
_LL156: {struct Cyc_Absyn_Instantiate_e_struct*_tmp245=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp234;if(_tmp245->tag != 14)goto _LL158;else{_tmp246=_tmp245->f1;}}_LL157: return
Cyc_Absynpp_exp_prec(_tmp246);_LL158: {struct Cyc_Absyn_Cast_e_struct*_tmp247=(
struct Cyc_Absyn_Cast_e_struct*)_tmp234;if(_tmp247->tag != 15)goto _LL15A;}_LL159:
return 120;_LL15A: {struct Cyc_Absyn_New_e_struct*_tmp248=(struct Cyc_Absyn_New_e_struct*)
_tmp234;if(_tmp248->tag != 17)goto _LL15C;}_LL15B: return 15;_LL15C: {struct Cyc_Absyn_Address_e_struct*
_tmp249=(struct Cyc_Absyn_Address_e_struct*)_tmp234;if(_tmp249->tag != 16)goto
_LL15E;}_LL15D: goto _LL15F;_LL15E: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp24A=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp234;if(_tmp24A->tag != 18)goto _LL160;}
_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp24B=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp234;if(_tmp24B->tag != 19)goto _LL162;}_LL161: goto _LL163;_LL162: {struct Cyc_Absyn_Valueof_e_struct*
_tmp24C=(struct Cyc_Absyn_Valueof_e_struct*)_tmp234;if(_tmp24C->tag != 39)goto
_LL164;}_LL163: goto _LL165;_LL164: {struct Cyc_Absyn_Asm_e_struct*_tmp24D=(struct
Cyc_Absyn_Asm_e_struct*)_tmp234;if(_tmp24D->tag != 40)goto _LL166;}_LL165: goto
_LL167;_LL166: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp24E=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp234;if(_tmp24E->tag != 38)goto _LL168;}_LL167: goto _LL169;_LL168: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp24F=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp234;if(_tmp24F->tag != 20)goto
_LL16A;}_LL169: goto _LL16B;_LL16A: {struct Cyc_Absyn_Deref_e_struct*_tmp250=(
struct Cyc_Absyn_Deref_e_struct*)_tmp234;if(_tmp250->tag != 21)goto _LL16C;}_LL16B:
return 130;_LL16C: {struct Cyc_Absyn_AggrMember_e_struct*_tmp251=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp234;if(_tmp251->tag != 22)goto _LL16E;}_LL16D: goto _LL16F;_LL16E: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp252=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp234;if(_tmp252->tag != 23)goto
_LL170;}_LL16F: goto _LL171;_LL170: {struct Cyc_Absyn_Subscript_e_struct*_tmp253=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp234;if(_tmp253->tag != 24)goto _LL172;}
_LL171: return 140;_LL172: {struct Cyc_Absyn_Tuple_e_struct*_tmp254=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp234;if(_tmp254->tag != 25)goto _LL174;}_LL173: return 150;_LL174: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp255=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp234;if(_tmp255->tag != 26)goto
_LL176;}_LL175: goto _LL177;_LL176: {struct Cyc_Absyn_Array_e_struct*_tmp256=(
struct Cyc_Absyn_Array_e_struct*)_tmp234;if(_tmp256->tag != 27)goto _LL178;}_LL177:
goto _LL179;_LL178: {struct Cyc_Absyn_Comprehension_e_struct*_tmp257=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp234;if(_tmp257->tag != 28)goto _LL17A;}_LL179: goto _LL17B;_LL17A: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp258=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp234;if(_tmp258->tag != 29)goto
_LL17C;}_LL17B: goto _LL17D;_LL17C: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp259=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp234;if(_tmp259->tag != 30)goto _LL17E;}
_LL17D: goto _LL17F;_LL17E: {struct Cyc_Absyn_Datatype_e_struct*_tmp25A=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp234;if(_tmp25A->tag != 31)goto _LL180;}_LL17F: goto _LL181;_LL180: {struct Cyc_Absyn_Enum_e_struct*
_tmp25B=(struct Cyc_Absyn_Enum_e_struct*)_tmp234;if(_tmp25B->tag != 32)goto _LL182;}
_LL181: goto _LL183;_LL182: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp25C=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp234;if(_tmp25C->tag != 33)goto _LL184;}_LL183: goto _LL185;_LL184: {struct Cyc_Absyn_Malloc_e_struct*
_tmp25D=(struct Cyc_Absyn_Malloc_e_struct*)_tmp234;if(_tmp25D->tag != 34)goto
_LL186;}_LL185: goto _LL187;_LL186: {struct Cyc_Absyn_Swap_e_struct*_tmp25E=(struct
Cyc_Absyn_Swap_e_struct*)_tmp234;if(_tmp25E->tag != 35)goto _LL188;}_LL187: goto
_LL189;_LL188: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp25F=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp234;if(_tmp25F->tag != 36)goto _LL18A;}_LL189: return 140;_LL18A: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp260=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp234;if(_tmp260->tag != 37)goto
_LL139;}_LL18B: return 10000;_LL139:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct
Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp261=e->r;union Cyc_Absyn_Cnst _tmp263;struct _tuple0*_tmp265;struct
_tuple0*_tmp267;enum Cyc_Absyn_Primop _tmp269;struct Cyc_List_List*_tmp26A;struct
Cyc_Absyn_Exp*_tmp26C;struct Cyc_Core_Opt*_tmp26D;struct Cyc_Absyn_Exp*_tmp26E;
struct Cyc_Absyn_Exp*_tmp270;enum Cyc_Absyn_Incrementor _tmp271;struct Cyc_Absyn_Exp*
_tmp273;struct Cyc_Absyn_Exp*_tmp274;struct Cyc_Absyn_Exp*_tmp275;struct Cyc_Absyn_Exp*
_tmp277;struct Cyc_Absyn_Exp*_tmp278;struct Cyc_Absyn_Exp*_tmp27A;struct Cyc_Absyn_Exp*
_tmp27B;struct Cyc_Absyn_Exp*_tmp27D;struct Cyc_Absyn_Exp*_tmp27E;struct Cyc_Absyn_Exp*
_tmp280;struct Cyc_List_List*_tmp281;struct Cyc_Absyn_Exp*_tmp283;struct Cyc_List_List*
_tmp284;struct Cyc_Absyn_Exp*_tmp286;struct Cyc_Absyn_Exp*_tmp288;struct Cyc_Absyn_Exp*
_tmp28A;void*_tmp28C;struct Cyc_Absyn_Exp*_tmp28D;struct Cyc_Absyn_Exp*_tmp28F;
struct Cyc_Absyn_Exp*_tmp291;struct Cyc_Absyn_Exp*_tmp292;void*_tmp294;struct Cyc_Absyn_Exp*
_tmp296;void*_tmp298;int _tmp29A;struct _dyneither_ptr _tmp29B;struct Cyc_Absyn_Exp*
_tmp29D;struct _dyneither_ptr*_tmp29E;void*_tmp2A0;void*_tmp2A1;struct
_dyneither_ptr*_tmp2A3;void*_tmp2A5;void*_tmp2A6;unsigned int _tmp2A8;struct Cyc_Absyn_Exp*
_tmp2AA;struct Cyc_Absyn_Exp*_tmp2AC;struct _dyneither_ptr*_tmp2AD;struct Cyc_Absyn_Exp*
_tmp2AF;struct _dyneither_ptr*_tmp2B0;struct Cyc_Absyn_Exp*_tmp2B2;struct Cyc_Absyn_Exp*
_tmp2B3;struct Cyc_List_List*_tmp2B5;struct _tuple8*_tmp2B7;struct Cyc_List_List*
_tmp2B8;struct Cyc_List_List*_tmp2BA;struct Cyc_Absyn_Vardecl*_tmp2BC;struct Cyc_Absyn_Exp*
_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BE;struct _tuple0*_tmp2C0;struct Cyc_List_List*
_tmp2C1;struct Cyc_List_List*_tmp2C2;struct Cyc_List_List*_tmp2C4;struct Cyc_List_List*
_tmp2C6;struct Cyc_Absyn_Datatypefield*_tmp2C7;struct _tuple0*_tmp2C9;struct
_tuple0*_tmp2CB;struct Cyc_Absyn_MallocInfo _tmp2CD;int _tmp2CE;struct Cyc_Absyn_Exp*
_tmp2CF;void**_tmp2D0;struct Cyc_Absyn_Exp*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D3;
struct Cyc_Absyn_Exp*_tmp2D4;struct Cyc_Core_Opt*_tmp2D6;struct Cyc_List_List*
_tmp2D7;struct Cyc_Absyn_Stmt*_tmp2D9;_LL1A2: {struct Cyc_Absyn_Const_e_struct*
_tmp262=(struct Cyc_Absyn_Const_e_struct*)_tmp261;if(_tmp262->tag != 0)goto _LL1A4;
else{_tmp263=_tmp262->f1;}}_LL1A3: s=Cyc_Absynpp_cnst2doc(_tmp263);goto _LL1A1;
_LL1A4: {struct Cyc_Absyn_Var_e_struct*_tmp264=(struct Cyc_Absyn_Var_e_struct*)
_tmp261;if(_tmp264->tag != 1)goto _LL1A6;else{_tmp265=_tmp264->f1;}}_LL1A5: _tmp267=
_tmp265;goto _LL1A7;_LL1A6: {struct Cyc_Absyn_UnknownId_e_struct*_tmp266=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp261;if(_tmp266->tag != 2)goto _LL1A8;else{
_tmp267=_tmp266->f1;}}_LL1A7: s=Cyc_Absynpp_qvar2doc(_tmp267);goto _LL1A1;_LL1A8: {
struct Cyc_Absyn_Primop_e_struct*_tmp268=(struct Cyc_Absyn_Primop_e_struct*)
_tmp261;if(_tmp268->tag != 3)goto _LL1AA;else{_tmp269=_tmp268->f1;_tmp26A=_tmp268->f2;}}
_LL1A9: s=Cyc_Absynpp_primapp2doc(myprec,_tmp269,_tmp26A);goto _LL1A1;_LL1AA: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp26B=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp261;if(_tmp26B->tag != 4)goto _LL1AC;else{_tmp26C=_tmp26B->f1;_tmp26D=_tmp26B->f2;
_tmp26E=_tmp26B->f3;}}_LL1AB:{const char*_tmp7A8;const char*_tmp7A7;struct Cyc_PP_Doc*
_tmp7A6[5];s=((_tmp7A6[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26E),((_tmp7A6[3]=
Cyc_PP_text(((_tmp7A7="= ",_tag_dyneither(_tmp7A7,sizeof(char),3)))),((_tmp7A6[2]=
_tmp26D == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp26D->v),((
_tmp7A6[1]=Cyc_PP_text(((_tmp7A8=" ",_tag_dyneither(_tmp7A8,sizeof(char),2)))),((
_tmp7A6[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26C),Cyc_PP_cat(_tag_dyneither(
_tmp7A6,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1A1;_LL1AC: {struct Cyc_Absyn_Increment_e_struct*
_tmp26F=(struct Cyc_Absyn_Increment_e_struct*)_tmp261;if(_tmp26F->tag != 5)goto
_LL1AE;else{_tmp270=_tmp26F->f1;_tmp271=_tmp26F->f2;}}_LL1AD: {struct Cyc_PP_Doc*
_tmp2DD=Cyc_Absynpp_exp2doc_prec(myprec,_tmp270);switch(_tmp271){case Cyc_Absyn_PreInc:
_LL1F6:{const char*_tmp7AB;struct Cyc_PP_Doc*_tmp7AA[2];s=((_tmp7AA[1]=_tmp2DD,((
_tmp7AA[0]=Cyc_PP_text(((_tmp7AB="++",_tag_dyneither(_tmp7AB,sizeof(char),3)))),
Cyc_PP_cat(_tag_dyneither(_tmp7AA,sizeof(struct Cyc_PP_Doc*),2))))));}break;case
Cyc_Absyn_PreDec: _LL1F7:{const char*_tmp7AE;struct Cyc_PP_Doc*_tmp7AD[2];s=((
_tmp7AD[1]=_tmp2DD,((_tmp7AD[0]=Cyc_PP_text(((_tmp7AE="--",_tag_dyneither(
_tmp7AE,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7AD,sizeof(struct Cyc_PP_Doc*),
2))))));}break;case Cyc_Absyn_PostInc: _LL1F8:{const char*_tmp7B1;struct Cyc_PP_Doc*
_tmp7B0[2];s=((_tmp7B0[1]=Cyc_PP_text(((_tmp7B1="++",_tag_dyneither(_tmp7B1,
sizeof(char),3)))),((_tmp7B0[0]=_tmp2DD,Cyc_PP_cat(_tag_dyneither(_tmp7B0,
sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1F9:{const char*
_tmp7B4;struct Cyc_PP_Doc*_tmp7B3[2];s=((_tmp7B3[1]=Cyc_PP_text(((_tmp7B4="--",
_tag_dyneither(_tmp7B4,sizeof(char),3)))),((_tmp7B3[0]=_tmp2DD,Cyc_PP_cat(
_tag_dyneither(_tmp7B3,sizeof(struct Cyc_PP_Doc*),2))))));}break;}goto _LL1A1;}
_LL1AE: {struct Cyc_Absyn_Conditional_e_struct*_tmp272=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp261;if(_tmp272->tag != 6)goto _LL1B0;else{_tmp273=_tmp272->f1;_tmp274=_tmp272->f2;
_tmp275=_tmp272->f3;}}_LL1AF:{const char*_tmp7B9;const char*_tmp7B8;struct Cyc_PP_Doc*
_tmp7B7[5];s=((_tmp7B7[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp275),((_tmp7B7[3]=
Cyc_PP_text(((_tmp7B8=" : ",_tag_dyneither(_tmp7B8,sizeof(char),4)))),((_tmp7B7[
2]=Cyc_Absynpp_exp2doc_prec(0,_tmp274),((_tmp7B7[1]=Cyc_PP_text(((_tmp7B9=" ? ",
_tag_dyneither(_tmp7B9,sizeof(char),4)))),((_tmp7B7[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp273),Cyc_PP_cat(_tag_dyneither(_tmp7B7,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A1;_LL1B0: {struct Cyc_Absyn_And_e_struct*_tmp276=(struct Cyc_Absyn_And_e_struct*)
_tmp261;if(_tmp276->tag != 7)goto _LL1B2;else{_tmp277=_tmp276->f1;_tmp278=_tmp276->f2;}}
_LL1B1:{const char*_tmp7BC;struct Cyc_PP_Doc*_tmp7BB[3];s=((_tmp7BB[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp278),((_tmp7BB[1]=Cyc_PP_text(((_tmp7BC=" && ",_tag_dyneither(_tmp7BC,
sizeof(char),5)))),((_tmp7BB[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp277),Cyc_PP_cat(
_tag_dyneither(_tmp7BB,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A1;_LL1B2: {
struct Cyc_Absyn_Or_e_struct*_tmp279=(struct Cyc_Absyn_Or_e_struct*)_tmp261;if(
_tmp279->tag != 8)goto _LL1B4;else{_tmp27A=_tmp279->f1;_tmp27B=_tmp279->f2;}}
_LL1B3:{const char*_tmp7BF;struct Cyc_PP_Doc*_tmp7BE[3];s=((_tmp7BE[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp27B),((_tmp7BE[1]=Cyc_PP_text(((_tmp7BF=" || ",_tag_dyneither(_tmp7BF,
sizeof(char),5)))),((_tmp7BE[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27A),Cyc_PP_cat(
_tag_dyneither(_tmp7BE,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A1;_LL1B4: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp27C=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp261;if(_tmp27C->tag != 9)goto _LL1B6;else{_tmp27D=_tmp27C->f1;_tmp27E=_tmp27C->f2;}}
_LL1B5:{const char*_tmp7C6;const char*_tmp7C5;const char*_tmp7C4;struct Cyc_PP_Doc*
_tmp7C3[5];s=((_tmp7C3[4]=Cyc_PP_text(((_tmp7C4=")",_tag_dyneither(_tmp7C4,
sizeof(char),2)))),((_tmp7C3[3]=Cyc_Absynpp_exp2doc(_tmp27E),((_tmp7C3[2]=Cyc_PP_text(((
_tmp7C5=", ",_tag_dyneither(_tmp7C5,sizeof(char),3)))),((_tmp7C3[1]=Cyc_Absynpp_exp2doc(
_tmp27D),((_tmp7C3[0]=Cyc_PP_text(((_tmp7C6="(",_tag_dyneither(_tmp7C6,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7C3,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A1;_LL1B6: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp27F=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp261;if(_tmp27F->tag != 10)goto _LL1B8;else{_tmp280=_tmp27F->f1;_tmp281=_tmp27F->f2;}}
_LL1B7:{const char*_tmp7CB;const char*_tmp7CA;struct Cyc_PP_Doc*_tmp7C9[4];s=((
_tmp7C9[3]=Cyc_PP_text(((_tmp7CA=")",_tag_dyneither(_tmp7CA,sizeof(char),2)))),((
_tmp7C9[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp281),((_tmp7C9[1]=Cyc_PP_text(((
_tmp7CB="(",_tag_dyneither(_tmp7CB,sizeof(char),2)))),((_tmp7C9[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp280),Cyc_PP_cat(_tag_dyneither(_tmp7C9,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1A1;_LL1B8: {struct Cyc_Absyn_FnCall_e_struct*_tmp282=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp261;if(_tmp282->tag != 11)goto _LL1BA;else{_tmp283=_tmp282->f1;_tmp284=_tmp282->f2;}}
_LL1B9:{const char*_tmp7D0;const char*_tmp7CF;struct Cyc_PP_Doc*_tmp7CE[4];s=((
_tmp7CE[3]=Cyc_PP_text(((_tmp7CF=")",_tag_dyneither(_tmp7CF,sizeof(char),2)))),((
_tmp7CE[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp284),((_tmp7CE[1]=Cyc_PP_text(((
_tmp7D0="(",_tag_dyneither(_tmp7D0,sizeof(char),2)))),((_tmp7CE[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp283),Cyc_PP_cat(_tag_dyneither(_tmp7CE,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1A1;_LL1BA: {struct Cyc_Absyn_Throw_e_struct*_tmp285=(struct Cyc_Absyn_Throw_e_struct*)
_tmp261;if(_tmp285->tag != 12)goto _LL1BC;else{_tmp286=_tmp285->f1;}}_LL1BB:{const
char*_tmp7D3;struct Cyc_PP_Doc*_tmp7D2[2];s=((_tmp7D2[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp286),((_tmp7D2[0]=Cyc_PP_text(((_tmp7D3="throw ",_tag_dyneither(
_tmp7D3,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp7D2,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL1A1;_LL1BC: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp287=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp261;if(_tmp287->tag != 13)goto _LL1BE;
else{_tmp288=_tmp287->f1;}}_LL1BD: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp288);
goto _LL1A1;_LL1BE: {struct Cyc_Absyn_Instantiate_e_struct*_tmp289=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp261;if(_tmp289->tag != 14)goto _LL1C0;else{_tmp28A=_tmp289->f1;}}_LL1BF: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmp28A);goto _LL1A1;_LL1C0: {struct Cyc_Absyn_Cast_e_struct*_tmp28B=(
struct Cyc_Absyn_Cast_e_struct*)_tmp261;if(_tmp28B->tag != 15)goto _LL1C2;else{
_tmp28C=(void*)_tmp28B->f1;_tmp28D=_tmp28B->f2;}}_LL1C1:{const char*_tmp7D8;const
char*_tmp7D7;struct Cyc_PP_Doc*_tmp7D6[4];s=((_tmp7D6[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp28D),((_tmp7D6[2]=Cyc_PP_text(((_tmp7D7=")",_tag_dyneither(_tmp7D7,
sizeof(char),2)))),((_tmp7D6[1]=Cyc_Absynpp_typ2doc(_tmp28C),((_tmp7D6[0]=Cyc_PP_text(((
_tmp7D8="(",_tag_dyneither(_tmp7D8,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp7D6,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1A1;_LL1C2: {struct Cyc_Absyn_Address_e_struct*
_tmp28E=(struct Cyc_Absyn_Address_e_struct*)_tmp261;if(_tmp28E->tag != 16)goto
_LL1C4;else{_tmp28F=_tmp28E->f1;}}_LL1C3:{const char*_tmp7DB;struct Cyc_PP_Doc*
_tmp7DA[2];s=((_tmp7DA[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28F),((_tmp7DA[0]=
Cyc_PP_text(((_tmp7DB="&",_tag_dyneither(_tmp7DB,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp7DA,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1A1;_LL1C4: {
struct Cyc_Absyn_New_e_struct*_tmp290=(struct Cyc_Absyn_New_e_struct*)_tmp261;if(
_tmp290->tag != 17)goto _LL1C6;else{_tmp291=_tmp290->f1;_tmp292=_tmp290->f2;}}
_LL1C5: if(_tmp291 == 0){const char*_tmp7DE;struct Cyc_PP_Doc*_tmp7DD[2];s=((_tmp7DD[
1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp292),((_tmp7DD[0]=Cyc_PP_text(((_tmp7DE="new ",
_tag_dyneither(_tmp7DE,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7DD,
sizeof(struct Cyc_PP_Doc*),2))))));}else{const char*_tmp7E3;const char*_tmp7E2;
struct Cyc_PP_Doc*_tmp7E1[4];s=((_tmp7E1[3]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp292),((_tmp7E1[2]=Cyc_PP_text(((_tmp7E2=") ",_tag_dyneither(_tmp7E2,sizeof(
char),3)))),((_tmp7E1[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp291),((
_tmp7E1[0]=Cyc_PP_text(((_tmp7E3="rnew(",_tag_dyneither(_tmp7E3,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp7E1,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL1A1;_LL1C6: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp293=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp261;if(_tmp293->tag != 18)goto _LL1C8;else{_tmp294=(void*)_tmp293->f1;}}_LL1C7:{
const char*_tmp7E8;const char*_tmp7E7;struct Cyc_PP_Doc*_tmp7E6[3];s=((_tmp7E6[2]=
Cyc_PP_text(((_tmp7E7=")",_tag_dyneither(_tmp7E7,sizeof(char),2)))),((_tmp7E6[1]=
Cyc_Absynpp_typ2doc(_tmp294),((_tmp7E6[0]=Cyc_PP_text(((_tmp7E8="sizeof(",
_tag_dyneither(_tmp7E8,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7E6,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A1;_LL1C8: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp295=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp261;if(_tmp295->tag != 19)goto
_LL1CA;else{_tmp296=_tmp295->f1;}}_LL1C9:{const char*_tmp7ED;const char*_tmp7EC;
struct Cyc_PP_Doc*_tmp7EB[3];s=((_tmp7EB[2]=Cyc_PP_text(((_tmp7EC=")",
_tag_dyneither(_tmp7EC,sizeof(char),2)))),((_tmp7EB[1]=Cyc_Absynpp_exp2doc(
_tmp296),((_tmp7EB[0]=Cyc_PP_text(((_tmp7ED="sizeof(",_tag_dyneither(_tmp7ED,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7EB,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A1;_LL1CA: {struct Cyc_Absyn_Valueof_e_struct*_tmp297=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp261;if(_tmp297->tag != 39)goto _LL1CC;else{_tmp298=(void*)_tmp297->f1;}}_LL1CB:{
const char*_tmp7F2;const char*_tmp7F1;struct Cyc_PP_Doc*_tmp7F0[3];s=((_tmp7F0[2]=
Cyc_PP_text(((_tmp7F1=")",_tag_dyneither(_tmp7F1,sizeof(char),2)))),((_tmp7F0[1]=
Cyc_Absynpp_typ2doc(_tmp298),((_tmp7F0[0]=Cyc_PP_text(((_tmp7F2="valueof(",
_tag_dyneither(_tmp7F2,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7F0,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A1;_LL1CC: {struct Cyc_Absyn_Asm_e_struct*
_tmp299=(struct Cyc_Absyn_Asm_e_struct*)_tmp261;if(_tmp299->tag != 40)goto _LL1CE;
else{_tmp29A=_tmp299->f1;_tmp29B=_tmp299->f2;}}_LL1CD: if(_tmp29A){const char*
_tmp7F9;const char*_tmp7F8;const char*_tmp7F7;struct Cyc_PP_Doc*_tmp7F6[4];s=((
_tmp7F6[3]=Cyc_PP_text(((_tmp7F7=")",_tag_dyneither(_tmp7F7,sizeof(char),2)))),((
_tmp7F6[2]=Cyc_PP_text(_tmp29B),((_tmp7F6[1]=Cyc_PP_text(((_tmp7F8=" volatile (",
_tag_dyneither(_tmp7F8,sizeof(char),12)))),((_tmp7F6[0]=Cyc_PP_text(((_tmp7F9="__asm__",
_tag_dyneither(_tmp7F9,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7F6,
sizeof(struct Cyc_PP_Doc*),4))))))))));}else{const char*_tmp7FE;const char*_tmp7FD;
struct Cyc_PP_Doc*_tmp7FC[3];s=((_tmp7FC[2]=Cyc_PP_text(((_tmp7FD=")",
_tag_dyneither(_tmp7FD,sizeof(char),2)))),((_tmp7FC[1]=Cyc_PP_text(_tmp29B),((
_tmp7FC[0]=Cyc_PP_text(((_tmp7FE="__asm__(",_tag_dyneither(_tmp7FE,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp7FC,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A1;_LL1CE: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp29C=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp261;if(_tmp29C->tag != 38)goto _LL1D0;else{_tmp29D=_tmp29C->f1;_tmp29E=_tmp29C->f2;}}
_LL1CF:{const char*_tmp805;const char*_tmp804;const char*_tmp803;struct Cyc_PP_Doc*
_tmp802[5];s=((_tmp802[4]=Cyc_PP_text(((_tmp803=")",_tag_dyneither(_tmp803,
sizeof(char),2)))),((_tmp802[3]=Cyc_PP_textptr(_tmp29E),((_tmp802[2]=Cyc_PP_text(((
_tmp804=".",_tag_dyneither(_tmp804,sizeof(char),2)))),((_tmp802[1]=Cyc_Absynpp_exp2doc(
_tmp29D),((_tmp802[0]=Cyc_PP_text(((_tmp805="tagcheck(",_tag_dyneither(_tmp805,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp802,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL1A1;_LL1D0: {struct Cyc_Absyn_Offsetof_e_struct*_tmp29F=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp261;if(_tmp29F->tag != 20)goto _LL1D2;else{
_tmp2A0=(void*)_tmp29F->f1;_tmp2A1=(void*)_tmp29F->f2;{struct Cyc_Absyn_StructField_struct*
_tmp2A2=(struct Cyc_Absyn_StructField_struct*)_tmp2A1;if(_tmp2A2->tag != 0)goto
_LL1D2;else{_tmp2A3=_tmp2A2->f1;}};}}_LL1D1:{const char*_tmp80C;const char*_tmp80B;
const char*_tmp80A;struct Cyc_PP_Doc*_tmp809[5];s=((_tmp809[4]=Cyc_PP_text(((
_tmp80A=")",_tag_dyneither(_tmp80A,sizeof(char),2)))),((_tmp809[3]=Cyc_PP_textptr(
_tmp2A3),((_tmp809[2]=Cyc_PP_text(((_tmp80B=",",_tag_dyneither(_tmp80B,sizeof(
char),2)))),((_tmp809[1]=Cyc_Absynpp_typ2doc(_tmp2A0),((_tmp809[0]=Cyc_PP_text(((
_tmp80C="offsetof(",_tag_dyneither(_tmp80C,sizeof(char),10)))),Cyc_PP_cat(
_tag_dyneither(_tmp809,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1A1;
_LL1D2: {struct Cyc_Absyn_Offsetof_e_struct*_tmp2A4=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp261;if(_tmp2A4->tag != 20)goto _LL1D4;else{_tmp2A5=(void*)_tmp2A4->f1;_tmp2A6=(
void*)_tmp2A4->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp2A7=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2A6;if(_tmp2A7->tag != 1)goto _LL1D4;else{_tmp2A8=_tmp2A7->f1;}};}}_LL1D3:{
const char*_tmp81A;const char*_tmp819;struct Cyc_Int_pa_struct _tmp818;void*_tmp817[
1];const char*_tmp816;const char*_tmp815;struct Cyc_PP_Doc*_tmp814[5];s=((_tmp814[4]=
Cyc_PP_text(((_tmp815=")",_tag_dyneither(_tmp815,sizeof(char),2)))),((_tmp814[3]=
Cyc_PP_text((struct _dyneither_ptr)((_tmp818.tag=1,((_tmp818.f1=(unsigned long)((
int)_tmp2A8),((_tmp817[0]=& _tmp818,Cyc_aprintf(((_tmp816="%d",_tag_dyneither(
_tmp816,sizeof(char),3))),_tag_dyneither(_tmp817,sizeof(void*),1))))))))),((
_tmp814[2]=Cyc_PP_text(((_tmp819=",",_tag_dyneither(_tmp819,sizeof(char),2)))),((
_tmp814[1]=Cyc_Absynpp_typ2doc(_tmp2A5),((_tmp814[0]=Cyc_PP_text(((_tmp81A="offsetof(",
_tag_dyneither(_tmp81A,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp814,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1A1;_LL1D4: {struct Cyc_Absyn_Deref_e_struct*
_tmp2A9=(struct Cyc_Absyn_Deref_e_struct*)_tmp261;if(_tmp2A9->tag != 21)goto _LL1D6;
else{_tmp2AA=_tmp2A9->f1;}}_LL1D5:{const char*_tmp81D;struct Cyc_PP_Doc*_tmp81C[2];
s=((_tmp81C[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AA),((_tmp81C[0]=Cyc_PP_text(((
_tmp81D="*",_tag_dyneither(_tmp81D,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp81C,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1A1;_LL1D6: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp2AB=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp261;if(_tmp2AB->tag != 22)goto
_LL1D8;else{_tmp2AC=_tmp2AB->f1;_tmp2AD=_tmp2AB->f2;}}_LL1D7:{const char*_tmp820;
struct Cyc_PP_Doc*_tmp81F[3];s=((_tmp81F[2]=Cyc_PP_textptr(_tmp2AD),((_tmp81F[1]=
Cyc_PP_text(((_tmp820=".",_tag_dyneither(_tmp820,sizeof(char),2)))),((_tmp81F[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AC),Cyc_PP_cat(_tag_dyneither(_tmp81F,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A1;_LL1D8: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp2AE=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp261;if(_tmp2AE->tag != 23)goto
_LL1DA;else{_tmp2AF=_tmp2AE->f1;_tmp2B0=_tmp2AE->f2;}}_LL1D9:{const char*_tmp823;
struct Cyc_PP_Doc*_tmp822[3];s=((_tmp822[2]=Cyc_PP_textptr(_tmp2B0),((_tmp822[1]=
Cyc_PP_text(((_tmp823="->",_tag_dyneither(_tmp823,sizeof(char),3)))),((_tmp822[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AF),Cyc_PP_cat(_tag_dyneither(_tmp822,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A1;_LL1DA: {struct Cyc_Absyn_Subscript_e_struct*
_tmp2B1=(struct Cyc_Absyn_Subscript_e_struct*)_tmp261;if(_tmp2B1->tag != 24)goto
_LL1DC;else{_tmp2B2=_tmp2B1->f1;_tmp2B3=_tmp2B1->f2;}}_LL1DB:{const char*_tmp828;
const char*_tmp827;struct Cyc_PP_Doc*_tmp826[4];s=((_tmp826[3]=Cyc_PP_text(((
_tmp827="]",_tag_dyneither(_tmp827,sizeof(char),2)))),((_tmp826[2]=Cyc_Absynpp_exp2doc(
_tmp2B3),((_tmp826[1]=Cyc_PP_text(((_tmp828="[",_tag_dyneither(_tmp828,sizeof(
char),2)))),((_tmp826[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B2),Cyc_PP_cat(
_tag_dyneither(_tmp826,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1A1;_LL1DC: {
struct Cyc_Absyn_Tuple_e_struct*_tmp2B4=(struct Cyc_Absyn_Tuple_e_struct*)_tmp261;
if(_tmp2B4->tag != 25)goto _LL1DE;else{_tmp2B5=_tmp2B4->f1;}}_LL1DD:{const char*
_tmp82D;const char*_tmp82C;struct Cyc_PP_Doc*_tmp82B[4];s=((_tmp82B[3]=Cyc_PP_text(((
_tmp82C=")",_tag_dyneither(_tmp82C,sizeof(char),2)))),((_tmp82B[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp2B5),((_tmp82B[1]=Cyc_PP_text(((_tmp82D="(",_tag_dyneither(_tmp82D,
sizeof(char),2)))),((_tmp82B[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(
_tmp82B,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1A1;_LL1DE: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp2B6=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp261;if(_tmp2B6->tag != 26)goto
_LL1E0;else{_tmp2B7=_tmp2B6->f1;_tmp2B8=_tmp2B6->f2;}}_LL1DF:{const char*_tmp834;
const char*_tmp833;const char*_tmp832;struct Cyc_PP_Doc*_tmp831[4];s=((_tmp831[3]=
Cyc_Absynpp_group_braces(((_tmp832=",",_tag_dyneither(_tmp832,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B8)),((_tmp831[2]=Cyc_PP_text(((_tmp833=")",
_tag_dyneither(_tmp833,sizeof(char),2)))),((_tmp831[1]=Cyc_Absynpp_typ2doc((*
_tmp2B7).f3),((_tmp831[0]=Cyc_PP_text(((_tmp834="(",_tag_dyneither(_tmp834,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp831,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1A1;_LL1E0: {struct Cyc_Absyn_Array_e_struct*_tmp2B9=(struct Cyc_Absyn_Array_e_struct*)
_tmp261;if(_tmp2B9->tag != 27)goto _LL1E2;else{_tmp2BA=_tmp2B9->f1;}}_LL1E1:{const
char*_tmp835;s=Cyc_Absynpp_group_braces(((_tmp835=",",_tag_dyneither(_tmp835,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BA));}goto _LL1A1;
_LL1E2: {struct Cyc_Absyn_Comprehension_e_struct*_tmp2BB=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp261;if(_tmp2BB->tag != 28)goto _LL1E4;else{_tmp2BC=_tmp2BB->f1;_tmp2BD=_tmp2BB->f2;
_tmp2BE=_tmp2BB->f3;}}_LL1E3:{const char*_tmp83C;const char*_tmp83B;const char*
_tmp83A;struct Cyc_PP_Doc*_tmp839[8];s=((_tmp839[7]=Cyc_Absynpp_rb(),((_tmp839[6]=
Cyc_Absynpp_exp2doc(_tmp2BE),((_tmp839[5]=Cyc_PP_text(((_tmp83A=" : ",
_tag_dyneither(_tmp83A,sizeof(char),4)))),((_tmp839[4]=Cyc_Absynpp_exp2doc(
_tmp2BD),((_tmp839[3]=Cyc_PP_text(((_tmp83B=" < ",_tag_dyneither(_tmp83B,sizeof(
char),4)))),((_tmp839[2]=Cyc_PP_text(*(*_tmp2BC->name).f2),((_tmp839[1]=Cyc_PP_text(((
_tmp83C="for ",_tag_dyneither(_tmp83C,sizeof(char),5)))),((_tmp839[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp839,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL1A1;_LL1E4: {struct Cyc_Absyn_Aggregate_e_struct*_tmp2BF=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp261;if(_tmp2BF->tag != 29)goto _LL1E6;else{_tmp2C0=_tmp2BF->f1;_tmp2C1=_tmp2BF->f2;
_tmp2C2=_tmp2BF->f3;}}_LL1E5: {struct Cyc_List_List*_tmp337=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2C2);{const char*_tmp841;struct Cyc_List_List*_tmp840;struct Cyc_PP_Doc*_tmp83F[
2];s=((_tmp83F[1]=Cyc_Absynpp_group_braces(((_tmp841=",",_tag_dyneither(_tmp841,
sizeof(char),2))),_tmp2C1 != 0?(_tmp840=_cycalloc(sizeof(*_tmp840)),((_tmp840->hd=
Cyc_Absynpp_tps2doc(_tmp2C1),((_tmp840->tl=_tmp337,_tmp840))))): _tmp337),((
_tmp83F[0]=Cyc_Absynpp_qvar2doc(_tmp2C0),Cyc_PP_cat(_tag_dyneither(_tmp83F,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1A1;}_LL1E6: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp2C3=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp261;if(_tmp2C3->tag != 30)goto
_LL1E8;else{_tmp2C4=_tmp2C3->f2;}}_LL1E7:{const char*_tmp842;s=Cyc_Absynpp_group_braces(((
_tmp842=",",_tag_dyneither(_tmp842,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2C4));}goto _LL1A1;_LL1E8: {struct Cyc_Absyn_Datatype_e_struct*_tmp2C5=(struct
Cyc_Absyn_Datatype_e_struct*)_tmp261;if(_tmp2C5->tag != 31)goto _LL1EA;else{
_tmp2C6=_tmp2C5->f1;_tmp2C7=_tmp2C5->f3;}}_LL1E9: if(_tmp2C6 == 0)s=Cyc_Absynpp_qvar2doc(
_tmp2C7->name);else{const char*_tmp849;const char*_tmp848;const char*_tmp847;struct
Cyc_PP_Doc*_tmp846[2];s=((_tmp846[1]=Cyc_PP_egroup(((_tmp849="(",_tag_dyneither(
_tmp849,sizeof(char),2))),((_tmp848=")",_tag_dyneither(_tmp848,sizeof(char),2))),((
_tmp847=",",_tag_dyneither(_tmp847,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_exp2doc,_tmp2C6)),((_tmp846[0]=Cyc_Absynpp_qvar2doc(_tmp2C7->name),
Cyc_PP_cat(_tag_dyneither(_tmp846,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1A1;
_LL1EA: {struct Cyc_Absyn_Enum_e_struct*_tmp2C8=(struct Cyc_Absyn_Enum_e_struct*)
_tmp261;if(_tmp2C8->tag != 32)goto _LL1EC;else{_tmp2C9=_tmp2C8->f1;}}_LL1EB: s=Cyc_Absynpp_qvar2doc(
_tmp2C9);goto _LL1A1;_LL1EC: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp2CA=(struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp261;if(_tmp2CA->tag != 33)goto _LL1EE;else{
_tmp2CB=_tmp2CA->f1;}}_LL1ED: s=Cyc_Absynpp_qvar2doc(_tmp2CB);goto _LL1A1;_LL1EE: {
struct Cyc_Absyn_Malloc_e_struct*_tmp2CC=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp261;if(_tmp2CC->tag != 34)goto _LL1F0;else{_tmp2CD=_tmp2CC->f1;_tmp2CE=_tmp2CD.is_calloc;
_tmp2CF=_tmp2CD.rgn;_tmp2D0=_tmp2CD.elt_type;_tmp2D1=_tmp2CD.num_elts;}}_LL1EF:
if(_tmp2CE){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp2D0)),0);if(_tmp2CF == 0){const char*_tmp850;const char*_tmp84F;const char*
_tmp84E;struct Cyc_PP_Doc*_tmp84D[5];s=((_tmp84D[4]=Cyc_PP_text(((_tmp84E=")",
_tag_dyneither(_tmp84E,sizeof(char),2)))),((_tmp84D[3]=Cyc_Absynpp_exp2doc(st),((
_tmp84D[2]=Cyc_PP_text(((_tmp84F=",",_tag_dyneither(_tmp84F,sizeof(char),2)))),((
_tmp84D[1]=Cyc_Absynpp_exp2doc(_tmp2D1),((_tmp84D[0]=Cyc_PP_text(((_tmp850="calloc(",
_tag_dyneither(_tmp850,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp84D,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp859;const char*
_tmp858;const char*_tmp857;const char*_tmp856;struct Cyc_PP_Doc*_tmp855[7];s=((
_tmp855[6]=Cyc_PP_text(((_tmp856=")",_tag_dyneither(_tmp856,sizeof(char),2)))),((
_tmp855[5]=Cyc_Absynpp_exp2doc(st),((_tmp855[4]=Cyc_PP_text(((_tmp857=",",
_tag_dyneither(_tmp857,sizeof(char),2)))),((_tmp855[3]=Cyc_Absynpp_exp2doc(
_tmp2D1),((_tmp855[2]=Cyc_PP_text(((_tmp858=",",_tag_dyneither(_tmp858,sizeof(
char),2)))),((_tmp855[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2CF),((
_tmp855[0]=Cyc_PP_text(((_tmp859="rcalloc(",_tag_dyneither(_tmp859,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp855,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp2D0 == 0)new_e=_tmp2D1;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp2D0,0),_tmp2D1,0);}if(_tmp2CF == 0){const char*
_tmp85E;const char*_tmp85D;struct Cyc_PP_Doc*_tmp85C[3];s=((_tmp85C[2]=Cyc_PP_text(((
_tmp85D=")",_tag_dyneither(_tmp85D,sizeof(char),2)))),((_tmp85C[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp85C[0]=Cyc_PP_text(((_tmp85E="malloc(",_tag_dyneither(_tmp85E,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp85C,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp865;const char*_tmp864;const char*_tmp863;struct Cyc_PP_Doc*
_tmp862[5];s=((_tmp862[4]=Cyc_PP_text(((_tmp863=")",_tag_dyneither(_tmp863,
sizeof(char),2)))),((_tmp862[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp862[2]=Cyc_PP_text(((
_tmp864=",",_tag_dyneither(_tmp864,sizeof(char),2)))),((_tmp862[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2CF),((_tmp862[0]=Cyc_PP_text(((_tmp865="rmalloc(",
_tag_dyneither(_tmp865,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp862,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL1A1;_LL1F0: {struct Cyc_Absyn_Swap_e_struct*
_tmp2D2=(struct Cyc_Absyn_Swap_e_struct*)_tmp261;if(_tmp2D2->tag != 35)goto _LL1F2;
else{_tmp2D3=_tmp2D2->f1;_tmp2D4=_tmp2D2->f2;}}_LL1F1:{const char*_tmp868;struct
Cyc_PP_Doc*_tmp867[3];s=((_tmp867[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D4),((
_tmp867[1]=Cyc_PP_text(((_tmp868=" :=: ",_tag_dyneither(_tmp868,sizeof(char),6)))),((
_tmp867[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D3),Cyc_PP_cat(_tag_dyneither(
_tmp867,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A1;_LL1F2: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp2D5=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp261;if(_tmp2D5->tag != 36)
goto _LL1F4;else{_tmp2D6=_tmp2D5->f1;_tmp2D7=_tmp2D5->f2;}}_LL1F3:{const char*
_tmp869;s=Cyc_Absynpp_group_braces(((_tmp869=",",_tag_dyneither(_tmp869,sizeof(
char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2D7));}goto _LL1A1;
_LL1F4: {struct Cyc_Absyn_StmtExp_e_struct*_tmp2D8=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp261;if(_tmp2D8->tag != 37)goto _LL1A1;else{_tmp2D9=_tmp2D8->f1;}}_LL1F5:{const
char*_tmp86E;const char*_tmp86D;struct Cyc_PP_Doc*_tmp86C[7];s=((_tmp86C[6]=Cyc_PP_text(((
_tmp86D=")",_tag_dyneither(_tmp86D,sizeof(char),2)))),((_tmp86C[5]=Cyc_Absynpp_rb(),((
_tmp86C[4]=Cyc_PP_blank_doc(),((_tmp86C[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2D9,1)),((_tmp86C[2]=Cyc_PP_blank_doc(),((_tmp86C[1]=Cyc_Absynpp_lb(),((
_tmp86C[0]=Cyc_PP_text(((_tmp86E="(",_tag_dyneither(_tmp86E,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp86C,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
goto _LL1A1;_LL1A1:;}if(inprec >= myprec){const char*_tmp873;const char*_tmp872;
struct Cyc_PP_Doc*_tmp871[3];s=((_tmp871[2]=Cyc_PP_text(((_tmp872=")",
_tag_dyneither(_tmp872,sizeof(char),2)))),((_tmp871[1]=s,((_tmp871[0]=Cyc_PP_text(((
_tmp873="(",_tag_dyneither(_tmp873,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp871,sizeof(struct Cyc_PP_Doc*),3))))))));}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d){void*_tmp359=d;struct Cyc_Absyn_Exp*_tmp35B;struct _dyneither_ptr*_tmp35D;
_LL1FC: {struct Cyc_Absyn_ArrayElement_struct*_tmp35A=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp359;if(_tmp35A->tag != 0)goto _LL1FE;else{_tmp35B=_tmp35A->f1;}}_LL1FD: {const
char*_tmp878;const char*_tmp877;struct Cyc_PP_Doc*_tmp876[3];return(_tmp876[2]=Cyc_PP_text(((
_tmp877="]",_tag_dyneither(_tmp877,sizeof(char),2)))),((_tmp876[1]=Cyc_Absynpp_exp2doc(
_tmp35B),((_tmp876[0]=Cyc_PP_text(((_tmp878=".[",_tag_dyneither(_tmp878,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp876,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL1FE: {struct Cyc_Absyn_FieldName_struct*_tmp35C=(struct Cyc_Absyn_FieldName_struct*)
_tmp359;if(_tmp35C->tag != 1)goto _LL1FB;else{_tmp35D=_tmp35C->f1;}}_LL1FF: {const
char*_tmp87B;struct Cyc_PP_Doc*_tmp87A[2];return(_tmp87A[1]=Cyc_PP_textptr(
_tmp35D),((_tmp87A[0]=Cyc_PP_text(((_tmp87B=".",_tag_dyneither(_tmp87B,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp87A,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL1FB:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple15*de){if((*de).f1 == 0)
return Cyc_Absynpp_exp2doc((*de).f2);else{const char*_tmp882;const char*_tmp881;
const char*_tmp880;struct Cyc_PP_Doc*_tmp87F[2];return(_tmp87F[1]=Cyc_Absynpp_exp2doc((*
de).f2),((_tmp87F[0]=Cyc_PP_egroup(((_tmp882="",_tag_dyneither(_tmp882,sizeof(
char),1))),((_tmp881="=",_tag_dyneither(_tmp881,sizeof(char),2))),((_tmp880="=",
_tag_dyneither(_tmp880,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),
Cyc_PP_cat(_tag_dyneither(_tmp87F,sizeof(struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){const char*_tmp885;
const char*_tmp884;const char*_tmp883;return Cyc_PP_group(((_tmp883="",
_tag_dyneither(_tmp883,sizeof(char),1))),((_tmp884="",_tag_dyneither(_tmp884,
sizeof(char),1))),((_tmp885=",",_tag_dyneither(_tmp885,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst
_tmp36A=c;struct _tuple3 _tmp36B;enum Cyc_Absyn_Sign _tmp36C;char _tmp36D;struct
_dyneither_ptr _tmp36E;struct _tuple4 _tmp36F;enum Cyc_Absyn_Sign _tmp370;short
_tmp371;struct _tuple5 _tmp372;enum Cyc_Absyn_Sign _tmp373;int _tmp374;struct _tuple5
_tmp375;enum Cyc_Absyn_Sign _tmp376;int _tmp377;struct _tuple5 _tmp378;enum Cyc_Absyn_Sign
_tmp379;int _tmp37A;struct _tuple6 _tmp37B;enum Cyc_Absyn_Sign _tmp37C;long long
_tmp37D;struct _tuple7 _tmp37E;struct _dyneither_ptr _tmp37F;int _tmp380;struct
_dyneither_ptr _tmp381;struct _dyneither_ptr _tmp382;_LL201: if((_tmp36A.Char_c).tag
!= 2)goto _LL203;_tmp36B=(struct _tuple3)(_tmp36A.Char_c).val;_tmp36C=_tmp36B.f1;
_tmp36D=_tmp36B.f2;_LL202: {const char*_tmp889;void*_tmp888[1];struct Cyc_String_pa_struct
_tmp887;return Cyc_PP_text((struct _dyneither_ptr)((_tmp887.tag=0,((_tmp887.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp36D)),((
_tmp888[0]=& _tmp887,Cyc_aprintf(((_tmp889="'%s'",_tag_dyneither(_tmp889,sizeof(
char),5))),_tag_dyneither(_tmp888,sizeof(void*),1)))))))));}_LL203: if((_tmp36A.Wchar_c).tag
!= 3)goto _LL205;_tmp36E=(struct _dyneither_ptr)(_tmp36A.Wchar_c).val;_LL204: {
const char*_tmp88D;void*_tmp88C[1];struct Cyc_String_pa_struct _tmp88B;return Cyc_PP_text((
struct _dyneither_ptr)((_tmp88B.tag=0,((_tmp88B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp36E),((_tmp88C[0]=& _tmp88B,Cyc_aprintf(((_tmp88D="L'%s'",
_tag_dyneither(_tmp88D,sizeof(char),6))),_tag_dyneither(_tmp88C,sizeof(void*),1)))))))));}
_LL205: if((_tmp36A.Short_c).tag != 4)goto _LL207;_tmp36F=(struct _tuple4)(_tmp36A.Short_c).val;
_tmp370=_tmp36F.f1;_tmp371=_tmp36F.f2;_LL206: {const char*_tmp891;void*_tmp890[1];
struct Cyc_Int_pa_struct _tmp88F;return Cyc_PP_text((struct _dyneither_ptr)((_tmp88F.tag=
1,((_tmp88F.f1=(unsigned long)((int)_tmp371),((_tmp890[0]=& _tmp88F,Cyc_aprintf(((
_tmp891="%d",_tag_dyneither(_tmp891,sizeof(char),3))),_tag_dyneither(_tmp890,
sizeof(void*),1)))))))));}_LL207: if((_tmp36A.Int_c).tag != 5)goto _LL209;_tmp372=(
struct _tuple5)(_tmp36A.Int_c).val;_tmp373=_tmp372.f1;if(_tmp373 != Cyc_Absyn_None)
goto _LL209;_tmp374=_tmp372.f2;_LL208: _tmp377=_tmp374;goto _LL20A;_LL209: if((
_tmp36A.Int_c).tag != 5)goto _LL20B;_tmp375=(struct _tuple5)(_tmp36A.Int_c).val;
_tmp376=_tmp375.f1;if(_tmp376 != Cyc_Absyn_Signed)goto _LL20B;_tmp377=_tmp375.f2;
_LL20A: {const char*_tmp895;void*_tmp894[1];struct Cyc_Int_pa_struct _tmp893;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp893.tag=1,((_tmp893.f1=(unsigned long)
_tmp377,((_tmp894[0]=& _tmp893,Cyc_aprintf(((_tmp895="%d",_tag_dyneither(_tmp895,
sizeof(char),3))),_tag_dyneither(_tmp894,sizeof(void*),1)))))))));}_LL20B: if((
_tmp36A.Int_c).tag != 5)goto _LL20D;_tmp378=(struct _tuple5)(_tmp36A.Int_c).val;
_tmp379=_tmp378.f1;if(_tmp379 != Cyc_Absyn_Unsigned)goto _LL20D;_tmp37A=_tmp378.f2;
_LL20C: {const char*_tmp899;void*_tmp898[1];struct Cyc_Int_pa_struct _tmp897;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp897.tag=1,((_tmp897.f1=(unsigned int)
_tmp37A,((_tmp898[0]=& _tmp897,Cyc_aprintf(((_tmp899="%u",_tag_dyneither(_tmp899,
sizeof(char),3))),_tag_dyneither(_tmp898,sizeof(void*),1)))))))));}_LL20D: if((
_tmp36A.LongLong_c).tag != 6)goto _LL20F;_tmp37B=(struct _tuple6)(_tmp36A.LongLong_c).val;
_tmp37C=_tmp37B.f1;_tmp37D=_tmp37B.f2;_LL20E: {const char*_tmp89A;return Cyc_PP_text(((
_tmp89A="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp89A,sizeof(char),27))));}
_LL20F: if((_tmp36A.Float_c).tag != 7)goto _LL211;_tmp37E=(struct _tuple7)(_tmp36A.Float_c).val;
_tmp37F=_tmp37E.f1;_LL210: return Cyc_PP_text(_tmp37F);_LL211: if((_tmp36A.Null_c).tag
!= 1)goto _LL213;_tmp380=(int)(_tmp36A.Null_c).val;_LL212: {const char*_tmp89B;
return Cyc_PP_text(((_tmp89B="NULL",_tag_dyneither(_tmp89B,sizeof(char),5))));}
_LL213: if((_tmp36A.String_c).tag != 8)goto _LL215;_tmp381=(struct _dyneither_ptr)(
_tmp36A.String_c).val;_LL214: {const char*_tmp8A0;const char*_tmp89F;struct Cyc_PP_Doc*
_tmp89E[3];return(_tmp89E[2]=Cyc_PP_text(((_tmp89F="\"",_tag_dyneither(_tmp89F,
sizeof(char),2)))),((_tmp89E[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp381)),((
_tmp89E[0]=Cyc_PP_text(((_tmp8A0="\"",_tag_dyneither(_tmp8A0,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp89E,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL215: if((
_tmp36A.Wstring_c).tag != 9)goto _LL200;_tmp382=(struct _dyneither_ptr)(_tmp36A.Wstring_c).val;
_LL216: {const char*_tmp8A5;const char*_tmp8A4;struct Cyc_PP_Doc*_tmp8A3[3];return(
_tmp8A3[2]=Cyc_PP_text(((_tmp8A4="\"",_tag_dyneither(_tmp8A4,sizeof(char),2)))),((
_tmp8A3[1]=Cyc_PP_text(_tmp382),((_tmp8A3[0]=Cyc_PP_text(((_tmp8A5="L\"",
_tag_dyneither(_tmp8A5,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp8A3,
sizeof(struct Cyc_PP_Doc*),3)))))));}_LL200:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(
int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(
p);if(p == (enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){if(es == 0  || es->tl != 0){
struct Cyc_Core_Failure_struct _tmp8B2;const char*_tmp8B1;void*_tmp8B0[1];struct Cyc_String_pa_struct
_tmp8AF;struct Cyc_Core_Failure_struct*_tmp8AE;(int)_throw((void*)((_tmp8AE=
_cycalloc(sizeof(*_tmp8AE)),((_tmp8AE[0]=((_tmp8B2.tag=Cyc_Core_Failure,((
_tmp8B2.f1=(struct _dyneither_ptr)((_tmp8AF.tag=0,((_tmp8AF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8B0[0]=&
_tmp8AF,Cyc_aprintf(((_tmp8B1="Absynpp::primapp2doc Numelts: %s with bad args",
_tag_dyneither(_tmp8B1,sizeof(char),47))),_tag_dyneither(_tmp8B0,sizeof(void*),1)))))))),
_tmp8B2)))),_tmp8AE)))));}{const char*_tmp8B7;const char*_tmp8B6;struct Cyc_PP_Doc*
_tmp8B5[3];return(_tmp8B5[2]=Cyc_PP_text(((_tmp8B6=")",_tag_dyneither(_tmp8B6,
sizeof(char),2)))),((_tmp8B5[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((
_tmp8B5[0]=Cyc_PP_text(((_tmp8B7="numelts(",_tag_dyneither(_tmp8B7,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp8B5,sizeof(struct Cyc_PP_Doc*),3)))))));};}
else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0){struct Cyc_Core_Failure_struct _tmp8C4;const char*_tmp8C3;
void*_tmp8C2[1];struct Cyc_String_pa_struct _tmp8C1;struct Cyc_Core_Failure_struct*
_tmp8C0;(int)_throw((void*)((_tmp8C0=_cycalloc(sizeof(*_tmp8C0)),((_tmp8C0[0]=((
_tmp8C4.tag=Cyc_Core_Failure,((_tmp8C4.f1=(struct _dyneither_ptr)((_tmp8C1.tag=0,((
_tmp8C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp8C2[0]=& _tmp8C1,Cyc_aprintf(((_tmp8C3="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp8C3,sizeof(char),38))),_tag_dyneither(_tmp8C2,sizeof(void*),1)))))))),
_tmp8C4)))),_tmp8C0)))));}else{if(ds->tl == 0){const char*_tmp8C7;struct Cyc_PP_Doc*
_tmp8C6[3];return(_tmp8C6[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp8C6[1]=Cyc_PP_text(((
_tmp8C7=" ",_tag_dyneither(_tmp8C7,sizeof(char),2)))),((_tmp8C6[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp8C6,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp8D4;const char*
_tmp8D3;void*_tmp8D2[1];struct Cyc_String_pa_struct _tmp8D1;struct Cyc_Core_Failure_struct*
_tmp8D0;(int)_throw((void*)((_tmp8D0=_cycalloc(sizeof(*_tmp8D0)),((_tmp8D0[0]=((
_tmp8D4.tag=Cyc_Core_Failure,((_tmp8D4.f1=(struct _dyneither_ptr)((_tmp8D1.tag=0,((
_tmp8D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp8D2[0]=& _tmp8D1,Cyc_aprintf(((_tmp8D3="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp8D3,sizeof(char),47))),_tag_dyneither(_tmp8D2,sizeof(void*),1)))))))),
_tmp8D4)))),_tmp8D0)))));}else{const char*_tmp8D9;const char*_tmp8D8;struct Cyc_PP_Doc*
_tmp8D7[5];return(_tmp8D7[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp8D7[3]=Cyc_PP_text(((_tmp8D8=" ",_tag_dyneither(
_tmp8D8,sizeof(char),2)))),((_tmp8D7[2]=ps,((_tmp8D7[1]=Cyc_PP_text(((_tmp8D9=" ",
_tag_dyneither(_tmp8D9,sizeof(char),2)))),((_tmp8D7[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp8D7,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){switch(p){case
Cyc_Absyn_Plus: _LL217: {const char*_tmp8DA;return(_tmp8DA="+",_tag_dyneither(
_tmp8DA,sizeof(char),2));}case Cyc_Absyn_Times: _LL218: {const char*_tmp8DB;return(
_tmp8DB="*",_tag_dyneither(_tmp8DB,sizeof(char),2));}case Cyc_Absyn_Minus: _LL219: {
const char*_tmp8DC;return(_tmp8DC="-",_tag_dyneither(_tmp8DC,sizeof(char),2));}
case Cyc_Absyn_Div: _LL21A: {const char*_tmp8DD;return(_tmp8DD="/",_tag_dyneither(
_tmp8DD,sizeof(char),2));}case Cyc_Absyn_Mod: _LL21B: {const char*_tmp8DF;const char*
_tmp8DE;return Cyc_Absynpp_print_for_cycdoc?(_tmp8DF="\\%",_tag_dyneither(_tmp8DF,
sizeof(char),3)):((_tmp8DE="%",_tag_dyneither(_tmp8DE,sizeof(char),2)));}case Cyc_Absyn_Eq:
_LL21C: {const char*_tmp8E0;return(_tmp8E0="==",_tag_dyneither(_tmp8E0,sizeof(
char),3));}case Cyc_Absyn_Neq: _LL21D: {const char*_tmp8E1;return(_tmp8E1="!=",
_tag_dyneither(_tmp8E1,sizeof(char),3));}case Cyc_Absyn_Gt: _LL21E: {const char*
_tmp8E2;return(_tmp8E2=">",_tag_dyneither(_tmp8E2,sizeof(char),2));}case Cyc_Absyn_Lt:
_LL21F: {const char*_tmp8E3;return(_tmp8E3="<",_tag_dyneither(_tmp8E3,sizeof(char),
2));}case Cyc_Absyn_Gte: _LL220: {const char*_tmp8E4;return(_tmp8E4=">=",
_tag_dyneither(_tmp8E4,sizeof(char),3));}case Cyc_Absyn_Lte: _LL221: {const char*
_tmp8E5;return(_tmp8E5="<=",_tag_dyneither(_tmp8E5,sizeof(char),3));}case Cyc_Absyn_Not:
_LL222: {const char*_tmp8E6;return(_tmp8E6="!",_tag_dyneither(_tmp8E6,sizeof(char),
2));}case Cyc_Absyn_Bitnot: _LL223: {const char*_tmp8E7;return(_tmp8E7="~",
_tag_dyneither(_tmp8E7,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL224: {const char*
_tmp8E8;return(_tmp8E8="&",_tag_dyneither(_tmp8E8,sizeof(char),2));}case Cyc_Absyn_Bitor:
_LL225: {const char*_tmp8E9;return(_tmp8E9="|",_tag_dyneither(_tmp8E9,sizeof(char),
2));}case Cyc_Absyn_Bitxor: _LL226: {const char*_tmp8EA;return(_tmp8EA="^",
_tag_dyneither(_tmp8EA,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL227: {const
char*_tmp8EB;return(_tmp8EB="<<",_tag_dyneither(_tmp8EB,sizeof(char),3));}case
Cyc_Absyn_Bitlrshift: _LL228: {const char*_tmp8EC;return(_tmp8EC=">>",
_tag_dyneither(_tmp8EC,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL229: {const
char*_tmp8ED;return(_tmp8ED=">>>",_tag_dyneither(_tmp8ED,sizeof(char),4));}case
Cyc_Absyn_Numelts: _LL22A: {const char*_tmp8EE;return(_tmp8EE="numelts",
_tag_dyneither(_tmp8EE,sizeof(char),8));}}}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
enum Cyc_Absyn_Primop p){return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp3C6=s->r;_LL22D: {struct Cyc_Absyn_Decl_s_struct*
_tmp3C7=(struct Cyc_Absyn_Decl_s_struct*)_tmp3C6;if(_tmp3C7->tag != 12)goto _LL22F;}
_LL22E: return 1;_LL22F:;_LL230: return 0;_LL22C:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st,int stmtexp){struct Cyc_PP_Doc*s;{void*_tmp3C8=st->r;
struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_Stmt*_tmp3CD;struct Cyc_Absyn_Stmt*
_tmp3CE;struct Cyc_Absyn_Exp*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D2;struct Cyc_Absyn_Stmt*
_tmp3D3;struct Cyc_Absyn_Stmt*_tmp3D4;struct _tuple9 _tmp3D6;struct Cyc_Absyn_Exp*
_tmp3D7;struct Cyc_Absyn_Stmt*_tmp3D8;struct _dyneither_ptr*_tmp3DC;struct Cyc_Absyn_Exp*
_tmp3DE;struct _tuple9 _tmp3DF;struct Cyc_Absyn_Exp*_tmp3E0;struct _tuple9 _tmp3E1;
struct Cyc_Absyn_Exp*_tmp3E2;struct Cyc_Absyn_Stmt*_tmp3E3;struct Cyc_Absyn_Exp*
_tmp3E5;struct Cyc_List_List*_tmp3E6;struct Cyc_List_List*_tmp3E8;struct Cyc_List_List*
_tmp3EA;struct Cyc_Absyn_Decl*_tmp3EC;struct Cyc_Absyn_Stmt*_tmp3ED;struct
_dyneither_ptr*_tmp3EF;struct Cyc_Absyn_Stmt*_tmp3F0;struct Cyc_Absyn_Stmt*_tmp3F2;
struct _tuple9 _tmp3F3;struct Cyc_Absyn_Exp*_tmp3F4;struct Cyc_Absyn_Stmt*_tmp3F6;
struct Cyc_List_List*_tmp3F7;struct Cyc_Absyn_Exp*_tmp3F9;_LL232: {struct Cyc_Absyn_Skip_s_struct*
_tmp3C9=(struct Cyc_Absyn_Skip_s_struct*)_tmp3C8;if(_tmp3C9->tag != 0)goto _LL234;}
_LL233:{const char*_tmp8EF;s=Cyc_PP_text(((_tmp8EF=";",_tag_dyneither(_tmp8EF,
sizeof(char),2))));}goto _LL231;_LL234: {struct Cyc_Absyn_Exp_s_struct*_tmp3CA=(
struct Cyc_Absyn_Exp_s_struct*)_tmp3C8;if(_tmp3CA->tag != 1)goto _LL236;else{
_tmp3CB=_tmp3CA->f1;}}_LL235:{const char*_tmp8F2;struct Cyc_PP_Doc*_tmp8F1[2];s=((
_tmp8F1[1]=Cyc_PP_text(((_tmp8F2=";",_tag_dyneither(_tmp8F2,sizeof(char),2)))),((
_tmp8F1[0]=Cyc_Absynpp_exp2doc(_tmp3CB),Cyc_PP_cat(_tag_dyneither(_tmp8F1,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL231;_LL236: {struct Cyc_Absyn_Seq_s_struct*
_tmp3CC=(struct Cyc_Absyn_Seq_s_struct*)_tmp3C8;if(_tmp3CC->tag != 2)goto _LL238;
else{_tmp3CD=_tmp3CC->f1;_tmp3CE=_tmp3CC->f2;}}_LL237: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3CD)){struct Cyc_PP_Doc*_tmp8FD[5];struct Cyc_PP_Doc*
_tmp8FC[7];const char*_tmp8FB;const char*_tmp8FA;struct Cyc_PP_Doc*_tmp8F9[7];s=((
_tmp8F9[6]=Cyc_Absynpp_is_declaration(_tmp3CE)?stmtexp?(_tmp8FC[6]=Cyc_PP_line_doc(),((
_tmp8FC[5]=Cyc_PP_text(((_tmp8FB=");",_tag_dyneither(_tmp8FB,sizeof(char),3)))),((
_tmp8FC[4]=Cyc_Absynpp_rb(),((_tmp8FC[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3CE,stmtexp)),((_tmp8FC[2]=Cyc_PP_blank_doc(),((_tmp8FC[1]=Cyc_Absynpp_lb(),((
_tmp8FC[0]=Cyc_PP_text(((_tmp8FA="(",_tag_dyneither(_tmp8FA,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp8FC,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):((
_tmp8FD[4]=Cyc_PP_line_doc(),((_tmp8FD[3]=Cyc_Absynpp_rb(),((_tmp8FD[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3CE,stmtexp)),((_tmp8FD[1]=Cyc_PP_blank_doc(),((
_tmp8FD[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8FD,sizeof(struct Cyc_PP_Doc*),
5)))))))))))): Cyc_Absynpp_stmt2doc(_tmp3CE,stmtexp),((_tmp8F9[5]=Cyc_PP_line_doc(),((
_tmp8F9[4]=Cyc_Absynpp_rb(),((_tmp8F9[3]=Cyc_PP_line_doc(),((_tmp8F9[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3CD,0)),((_tmp8F9[1]=Cyc_PP_blank_doc(),((_tmp8F9[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8F9,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
else{if(Cyc_Absynpp_is_declaration(_tmp3CE)){struct Cyc_PP_Doc*_tmp908[4];struct
Cyc_PP_Doc*_tmp907[6];const char*_tmp906;const char*_tmp905;struct Cyc_PP_Doc*
_tmp904[4];s=((_tmp904[3]=Cyc_PP_line_doc(),((_tmp904[2]=stmtexp?(_tmp907[5]=Cyc_PP_text(((
_tmp906=");",_tag_dyneither(_tmp906,sizeof(char),3)))),((_tmp907[4]=Cyc_Absynpp_rb(),((
_tmp907[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CE,stmtexp)),((_tmp907[2]=Cyc_PP_blank_doc(),((
_tmp907[1]=Cyc_Absynpp_lb(),((_tmp907[0]=Cyc_PP_text(((_tmp905="(",
_tag_dyneither(_tmp905,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp907,
sizeof(struct Cyc_PP_Doc*),6))))))))))))):((_tmp908[3]=Cyc_Absynpp_rb(),((_tmp908[
2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CE,stmtexp)),((_tmp908[1]=Cyc_PP_blank_doc(),((
_tmp908[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp908,sizeof(struct Cyc_PP_Doc*),
4)))))))))),((_tmp904[1]=Cyc_PP_line_doc(),((_tmp904[0]=Cyc_Absynpp_stmt2doc(
_tmp3CD,0),Cyc_PP_cat(_tag_dyneither(_tmp904,sizeof(struct Cyc_PP_Doc*),4))))))))));}
else{struct Cyc_PP_Doc*_tmp909[3];s=((_tmp909[2]=Cyc_Absynpp_stmt2doc(_tmp3CE,
stmtexp),((_tmp909[1]=Cyc_PP_line_doc(),((_tmp909[0]=Cyc_Absynpp_stmt2doc(
_tmp3CD,0),Cyc_PP_cat(_tag_dyneither(_tmp909,sizeof(struct Cyc_PP_Doc*),3))))))));}}}
else{struct Cyc_PP_Doc*_tmp90A[3];s=((_tmp90A[2]=Cyc_Absynpp_stmt2doc(_tmp3CE,
stmtexp),((_tmp90A[1]=Cyc_PP_line_doc(),((_tmp90A[0]=Cyc_Absynpp_stmt2doc(
_tmp3CD,0),Cyc_PP_cat(_tag_dyneither(_tmp90A,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL231;_LL238: {struct Cyc_Absyn_Return_s_struct*_tmp3CF=(struct Cyc_Absyn_Return_s_struct*)
_tmp3C8;if(_tmp3CF->tag != 3)goto _LL23A;else{_tmp3D0=_tmp3CF->f1;}}_LL239: if(
_tmp3D0 == 0){const char*_tmp90B;s=Cyc_PP_text(((_tmp90B="return;",_tag_dyneither(
_tmp90B,sizeof(char),8))));}else{const char*_tmp910;const char*_tmp90F;struct Cyc_PP_Doc*
_tmp90E[3];s=((_tmp90E[2]=Cyc_PP_text(((_tmp90F=";",_tag_dyneither(_tmp90F,
sizeof(char),2)))),((_tmp90E[1]=_tmp3D0 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp3D0),((_tmp90E[0]=Cyc_PP_text(((_tmp910="return ",
_tag_dyneither(_tmp910,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp90E,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL231;_LL23A: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp3D1=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3C8;if(_tmp3D1->tag != 4)goto
_LL23C;else{_tmp3D2=_tmp3D1->f1;_tmp3D3=_tmp3D1->f2;_tmp3D4=_tmp3D1->f3;}}_LL23B: {
int print_else;{void*_tmp40D=_tmp3D4->r;_LL257: {struct Cyc_Absyn_Skip_s_struct*
_tmp40E=(struct Cyc_Absyn_Skip_s_struct*)_tmp40D;if(_tmp40E->tag != 0)goto _LL259;}
_LL258: print_else=0;goto _LL256;_LL259:;_LL25A: print_else=1;goto _LL256;_LL256:;}{
const char*_tmp91F;const char*_tmp91E;struct Cyc_PP_Doc*_tmp91D[2];struct Cyc_PP_Doc*
_tmp91C[6];struct Cyc_PP_Doc*_tmp91B[2];const char*_tmp91A;struct Cyc_PP_Doc*
_tmp919[8];s=((_tmp919[7]=print_else?(_tmp91C[5]=Cyc_Absynpp_rb(),((_tmp91C[4]=
Cyc_PP_line_doc(),((_tmp91C[3]=Cyc_PP_nest(2,((_tmp91B[1]=Cyc_Absynpp_stmt2doc(
_tmp3D4,0),((_tmp91B[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp91B,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp91C[2]=Cyc_Absynpp_lb(),((_tmp91C[1]=
Cyc_PP_text(((_tmp91A="else ",_tag_dyneither(_tmp91A,sizeof(char),6)))),((
_tmp91C[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp91C,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp919[6]=Cyc_Absynpp_rb(),((_tmp919[5]=Cyc_PP_line_doc(),((
_tmp919[4]=Cyc_PP_nest(2,((_tmp91D[1]=Cyc_Absynpp_stmt2doc(_tmp3D3,0),((_tmp91D[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp91D,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp919[3]=Cyc_Absynpp_lb(),((_tmp919[2]=Cyc_PP_text(((_tmp91E=") ",
_tag_dyneither(_tmp91E,sizeof(char),3)))),((_tmp919[1]=Cyc_Absynpp_exp2doc(
_tmp3D2),((_tmp919[0]=Cyc_PP_text(((_tmp91F="if (",_tag_dyneither(_tmp91F,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp919,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL231;}_LL23C: {struct Cyc_Absyn_While_s_struct*_tmp3D5=(struct Cyc_Absyn_While_s_struct*)
_tmp3C8;if(_tmp3D5->tag != 5)goto _LL23E;else{_tmp3D6=_tmp3D5->f1;_tmp3D7=_tmp3D6.f1;
_tmp3D8=_tmp3D5->f2;}}_LL23D:{const char*_tmp926;const char*_tmp925;struct Cyc_PP_Doc*
_tmp924[2];struct Cyc_PP_Doc*_tmp923[7];s=((_tmp923[6]=Cyc_Absynpp_rb(),((_tmp923[
5]=Cyc_PP_line_doc(),((_tmp923[4]=Cyc_PP_nest(2,((_tmp924[1]=Cyc_Absynpp_stmt2doc(
_tmp3D8,0),((_tmp924[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp924,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp923[3]=Cyc_Absynpp_lb(),((_tmp923[2]=
Cyc_PP_text(((_tmp925=") ",_tag_dyneither(_tmp925,sizeof(char),3)))),((_tmp923[1]=
Cyc_Absynpp_exp2doc(_tmp3D7),((_tmp923[0]=Cyc_PP_text(((_tmp926="while (",
_tag_dyneither(_tmp926,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp923,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL231;_LL23E: {struct Cyc_Absyn_Break_s_struct*
_tmp3D9=(struct Cyc_Absyn_Break_s_struct*)_tmp3C8;if(_tmp3D9->tag != 6)goto _LL240;}
_LL23F:{const char*_tmp927;s=Cyc_PP_text(((_tmp927="break;",_tag_dyneither(
_tmp927,sizeof(char),7))));}goto _LL231;_LL240: {struct Cyc_Absyn_Continue_s_struct*
_tmp3DA=(struct Cyc_Absyn_Continue_s_struct*)_tmp3C8;if(_tmp3DA->tag != 7)goto
_LL242;}_LL241:{const char*_tmp928;s=Cyc_PP_text(((_tmp928="continue;",
_tag_dyneither(_tmp928,sizeof(char),10))));}goto _LL231;_LL242: {struct Cyc_Absyn_Goto_s_struct*
_tmp3DB=(struct Cyc_Absyn_Goto_s_struct*)_tmp3C8;if(_tmp3DB->tag != 8)goto _LL244;
else{_tmp3DC=_tmp3DB->f1;}}_LL243:{const char*_tmp92C;void*_tmp92B[1];struct Cyc_String_pa_struct
_tmp92A;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp92A.tag=0,((_tmp92A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3DC),((_tmp92B[0]=& _tmp92A,Cyc_aprintf(((
_tmp92C="goto %s;",_tag_dyneither(_tmp92C,sizeof(char),9))),_tag_dyneither(
_tmp92B,sizeof(void*),1)))))))));}goto _LL231;_LL244: {struct Cyc_Absyn_For_s_struct*
_tmp3DD=(struct Cyc_Absyn_For_s_struct*)_tmp3C8;if(_tmp3DD->tag != 9)goto _LL246;
else{_tmp3DE=_tmp3DD->f1;_tmp3DF=_tmp3DD->f2;_tmp3E0=_tmp3DF.f1;_tmp3E1=_tmp3DD->f3;
_tmp3E2=_tmp3E1.f1;_tmp3E3=_tmp3DD->f4;}}_LL245:{const char*_tmp937;const char*
_tmp936;const char*_tmp935;const char*_tmp934;struct Cyc_PP_Doc*_tmp933[2];struct
Cyc_PP_Doc*_tmp932[11];s=((_tmp932[10]=Cyc_Absynpp_rb(),((_tmp932[9]=Cyc_PP_line_doc(),((
_tmp932[8]=Cyc_PP_nest(2,((_tmp933[1]=Cyc_Absynpp_stmt2doc(_tmp3E3,0),((_tmp933[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp933,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp932[7]=Cyc_Absynpp_lb(),((_tmp932[6]=Cyc_PP_text(((_tmp934=") ",
_tag_dyneither(_tmp934,sizeof(char),3)))),((_tmp932[5]=Cyc_Absynpp_exp2doc(
_tmp3E2),((_tmp932[4]=Cyc_PP_text(((_tmp935="; ",_tag_dyneither(_tmp935,sizeof(
char),3)))),((_tmp932[3]=Cyc_Absynpp_exp2doc(_tmp3E0),((_tmp932[2]=Cyc_PP_text(((
_tmp936="; ",_tag_dyneither(_tmp936,sizeof(char),3)))),((_tmp932[1]=Cyc_Absynpp_exp2doc(
_tmp3DE),((_tmp932[0]=Cyc_PP_text(((_tmp937="for(",_tag_dyneither(_tmp937,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp932,sizeof(struct Cyc_PP_Doc*),
11))))))))))))))))))))))));}goto _LL231;_LL246: {struct Cyc_Absyn_Switch_s_struct*
_tmp3E4=(struct Cyc_Absyn_Switch_s_struct*)_tmp3C8;if(_tmp3E4->tag != 10)goto
_LL248;else{_tmp3E5=_tmp3E4->f1;_tmp3E6=_tmp3E4->f2;}}_LL247:{const char*_tmp93C;
const char*_tmp93B;struct Cyc_PP_Doc*_tmp93A[8];s=((_tmp93A[7]=Cyc_Absynpp_rb(),((
_tmp93A[6]=Cyc_PP_line_doc(),((_tmp93A[5]=Cyc_Absynpp_switchclauses2doc(_tmp3E6),((
_tmp93A[4]=Cyc_PP_line_doc(),((_tmp93A[3]=Cyc_Absynpp_lb(),((_tmp93A[2]=Cyc_PP_text(((
_tmp93B=") ",_tag_dyneither(_tmp93B,sizeof(char),3)))),((_tmp93A[1]=Cyc_Absynpp_exp2doc(
_tmp3E5),((_tmp93A[0]=Cyc_PP_text(((_tmp93C="switch (",_tag_dyneither(_tmp93C,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp93A,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL231;_LL248: {struct Cyc_Absyn_Fallthru_s_struct*_tmp3E7=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp3C8;if(_tmp3E7->tag != 11)goto _LL24A;else{_tmp3E8=_tmp3E7->f1;if(_tmp3E8 != 0)
goto _LL24A;}}_LL249:{const char*_tmp93D;s=Cyc_PP_text(((_tmp93D="fallthru;",
_tag_dyneither(_tmp93D,sizeof(char),10))));}goto _LL231;_LL24A: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp3E9=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp3C8;if(_tmp3E9->tag != 11)goto
_LL24C;else{_tmp3EA=_tmp3E9->f1;}}_LL24B:{const char*_tmp942;const char*_tmp941;
struct Cyc_PP_Doc*_tmp940[3];s=((_tmp940[2]=Cyc_PP_text(((_tmp941=");",
_tag_dyneither(_tmp941,sizeof(char),3)))),((_tmp940[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp3EA),((_tmp940[0]=Cyc_PP_text(((_tmp942="fallthru(",_tag_dyneither(
_tmp942,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp940,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL231;_LL24C: {struct Cyc_Absyn_Decl_s_struct*_tmp3EB=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3C8;if(_tmp3EB->tag != 12)goto _LL24E;else{_tmp3EC=_tmp3EB->f1;_tmp3ED=_tmp3EB->f2;}}
_LL24D:{struct Cyc_PP_Doc*_tmp943[3];s=((_tmp943[2]=Cyc_Absynpp_stmt2doc(_tmp3ED,
stmtexp),((_tmp943[1]=Cyc_PP_line_doc(),((_tmp943[0]=Cyc_Absynpp_decl2doc(
_tmp3EC),Cyc_PP_cat(_tag_dyneither(_tmp943,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL231;_LL24E: {struct Cyc_Absyn_Label_s_struct*_tmp3EE=(struct Cyc_Absyn_Label_s_struct*)
_tmp3C8;if(_tmp3EE->tag != 13)goto _LL250;else{_tmp3EF=_tmp3EE->f1;_tmp3F0=_tmp3EE->f2;}}
_LL24F: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3F0)){if(
stmtexp){const char*_tmp948;const char*_tmp947;struct Cyc_PP_Doc*_tmp946[8];s=((
_tmp946[7]=Cyc_PP_text(((_tmp947=");",_tag_dyneither(_tmp947,sizeof(char),3)))),((
_tmp946[6]=Cyc_Absynpp_rb(),((_tmp946[5]=Cyc_PP_line_doc(),((_tmp946[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3F0,1)),((_tmp946[3]=Cyc_PP_line_doc(),((_tmp946[2]=
Cyc_Absynpp_lb(),((_tmp946[1]=Cyc_PP_text(((_tmp948=": (",_tag_dyneither(_tmp948,
sizeof(char),4)))),((_tmp946[0]=Cyc_PP_textptr(_tmp3EF),Cyc_PP_cat(
_tag_dyneither(_tmp946,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{const
char*_tmp94B;struct Cyc_PP_Doc*_tmp94A[7];s=((_tmp94A[6]=Cyc_Absynpp_rb(),((
_tmp94A[5]=Cyc_PP_line_doc(),((_tmp94A[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3F0,0)),((_tmp94A[3]=Cyc_PP_line_doc(),((_tmp94A[2]=Cyc_Absynpp_lb(),((
_tmp94A[1]=Cyc_PP_text(((_tmp94B=": ",_tag_dyneither(_tmp94B,sizeof(char),3)))),((
_tmp94A[0]=Cyc_PP_textptr(_tmp3EF),Cyc_PP_cat(_tag_dyneither(_tmp94A,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}}else{const char*_tmp94E;struct Cyc_PP_Doc*
_tmp94D[3];s=((_tmp94D[2]=Cyc_Absynpp_stmt2doc(_tmp3F0,stmtexp),((_tmp94D[1]=Cyc_PP_text(((
_tmp94E=": ",_tag_dyneither(_tmp94E,sizeof(char),3)))),((_tmp94D[0]=Cyc_PP_textptr(
_tmp3EF),Cyc_PP_cat(_tag_dyneither(_tmp94D,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL231;_LL250: {struct Cyc_Absyn_Do_s_struct*_tmp3F1=(struct Cyc_Absyn_Do_s_struct*)
_tmp3C8;if(_tmp3F1->tag != 14)goto _LL252;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F1->f2;
_tmp3F4=_tmp3F3.f1;}}_LL251:{const char*_tmp955;const char*_tmp954;const char*
_tmp953;struct Cyc_PP_Doc*_tmp952[9];s=((_tmp952[8]=Cyc_PP_text(((_tmp953=");",
_tag_dyneither(_tmp953,sizeof(char),3)))),((_tmp952[7]=Cyc_Absynpp_exp2doc(
_tmp3F4),((_tmp952[6]=Cyc_PP_text(((_tmp954=" while (",_tag_dyneither(_tmp954,
sizeof(char),9)))),((_tmp952[5]=Cyc_Absynpp_rb(),((_tmp952[4]=Cyc_PP_line_doc(),((
_tmp952[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3F2,0)),((_tmp952[2]=Cyc_PP_line_doc(),((
_tmp952[1]=Cyc_Absynpp_lb(),((_tmp952[0]=Cyc_PP_text(((_tmp955="do ",
_tag_dyneither(_tmp955,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp952,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL231;_LL252: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp3F5=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp3C8;if(_tmp3F5->tag != 15)goto
_LL254;else{_tmp3F6=_tmp3F5->f1;_tmp3F7=_tmp3F5->f2;}}_LL253:{const char*_tmp95A;
const char*_tmp959;struct Cyc_PP_Doc*_tmp958[12];s=((_tmp958[11]=Cyc_Absynpp_rb(),((
_tmp958[10]=Cyc_PP_line_doc(),((_tmp958[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp3F7)),((_tmp958[8]=Cyc_PP_line_doc(),((_tmp958[7]=Cyc_Absynpp_lb(),((_tmp958[
6]=Cyc_PP_text(((_tmp959=" catch ",_tag_dyneither(_tmp959,sizeof(char),8)))),((
_tmp958[5]=Cyc_Absynpp_rb(),((_tmp958[4]=Cyc_PP_line_doc(),((_tmp958[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3F6,0)),((_tmp958[2]=Cyc_PP_line_doc(),((_tmp958[1]=
Cyc_Absynpp_lb(),((_tmp958[0]=Cyc_PP_text(((_tmp95A="try ",_tag_dyneither(
_tmp95A,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp958,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL231;_LL254: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp3F8=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp3C8;if(_tmp3F8->tag != 16)goto
_LL231;else{_tmp3F9=_tmp3F8->f1;}}_LL255:{const char*_tmp95F;const char*_tmp95E;
struct Cyc_PP_Doc*_tmp95D[3];s=((_tmp95D[2]=Cyc_PP_text(((_tmp95E=");",
_tag_dyneither(_tmp95E,sizeof(char),3)))),((_tmp95D[1]=Cyc_Absynpp_exp2doc(
_tmp3F9),((_tmp95D[0]=Cyc_PP_text(((_tmp95F="reset_region(",_tag_dyneither(
_tmp95F,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp95D,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL231;_LL231:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp43E=p->r;enum Cyc_Absyn_Sign
_tmp442;int _tmp443;char _tmp445;struct _dyneither_ptr _tmp447;struct Cyc_Absyn_Vardecl*
_tmp449;struct Cyc_Absyn_Pat*_tmp44A;struct Cyc_Absyn_Pat _tmp44B;void*_tmp44C;
struct Cyc_Absyn_Vardecl*_tmp44F;struct Cyc_Absyn_Pat*_tmp450;struct Cyc_Absyn_Tvar*
_tmp452;struct Cyc_Absyn_Vardecl*_tmp453;struct Cyc_List_List*_tmp455;int _tmp456;
struct Cyc_Absyn_Pat*_tmp458;struct Cyc_Absyn_Vardecl*_tmp45A;struct Cyc_Absyn_Pat*
_tmp45B;struct Cyc_Absyn_Pat _tmp45C;void*_tmp45D;struct Cyc_Absyn_Vardecl*_tmp460;
struct Cyc_Absyn_Pat*_tmp461;struct _tuple0*_tmp463;struct _tuple0*_tmp465;struct
Cyc_List_List*_tmp466;int _tmp467;struct Cyc_Absyn_AggrInfo*_tmp469;struct Cyc_Absyn_AggrInfo
_tmp46A;union Cyc_Absyn_AggrInfoU _tmp46B;struct Cyc_List_List*_tmp46C;struct Cyc_List_List*
_tmp46D;int _tmp46E;struct Cyc_Absyn_AggrInfo*_tmp470;struct Cyc_List_List*_tmp471;
struct Cyc_List_List*_tmp472;int _tmp473;struct Cyc_Absyn_Enumfield*_tmp475;struct
Cyc_Absyn_Enumfield*_tmp477;struct Cyc_Absyn_Datatypefield*_tmp479;struct Cyc_List_List*
_tmp47A;struct Cyc_Absyn_Datatypefield*_tmp47C;struct Cyc_List_List*_tmp47D;int
_tmp47E;struct Cyc_Absyn_Exp*_tmp480;_LL25C: {struct Cyc_Absyn_Wild_p_struct*
_tmp43F=(struct Cyc_Absyn_Wild_p_struct*)_tmp43E;if(_tmp43F->tag != 0)goto _LL25E;}
_LL25D:{const char*_tmp960;s=Cyc_PP_text(((_tmp960="_",_tag_dyneither(_tmp960,
sizeof(char),2))));}goto _LL25B;_LL25E: {struct Cyc_Absyn_Null_p_struct*_tmp440=(
struct Cyc_Absyn_Null_p_struct*)_tmp43E;if(_tmp440->tag != 8)goto _LL260;}_LL25F:{
const char*_tmp961;s=Cyc_PP_text(((_tmp961="NULL",_tag_dyneither(_tmp961,sizeof(
char),5))));}goto _LL25B;_LL260: {struct Cyc_Absyn_Int_p_struct*_tmp441=(struct Cyc_Absyn_Int_p_struct*)
_tmp43E;if(_tmp441->tag != 9)goto _LL262;else{_tmp442=_tmp441->f1;_tmp443=_tmp441->f2;}}
_LL261: if(_tmp442 != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){const char*_tmp965;
void*_tmp964[1];struct Cyc_Int_pa_struct _tmp963;s=Cyc_PP_text((struct
_dyneither_ptr)((_tmp963.tag=1,((_tmp963.f1=(unsigned long)_tmp443,((_tmp964[0]=&
_tmp963,Cyc_aprintf(((_tmp965="%d",_tag_dyneither(_tmp965,sizeof(char),3))),
_tag_dyneither(_tmp964,sizeof(void*),1)))))))));}else{const char*_tmp969;void*
_tmp968[1];struct Cyc_Int_pa_struct _tmp967;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp967.tag=1,((_tmp967.f1=(unsigned int)_tmp443,((_tmp968[0]=& _tmp967,Cyc_aprintf(((
_tmp969="%u",_tag_dyneither(_tmp969,sizeof(char),3))),_tag_dyneither(_tmp968,
sizeof(void*),1)))))))));}goto _LL25B;_LL262: {struct Cyc_Absyn_Char_p_struct*
_tmp444=(struct Cyc_Absyn_Char_p_struct*)_tmp43E;if(_tmp444->tag != 10)goto _LL264;
else{_tmp445=_tmp444->f1;}}_LL263:{const char*_tmp96D;void*_tmp96C[1];struct Cyc_String_pa_struct
_tmp96B;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp96B.tag=0,((_tmp96B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp445)),((
_tmp96C[0]=& _tmp96B,Cyc_aprintf(((_tmp96D="'%s'",_tag_dyneither(_tmp96D,sizeof(
char),5))),_tag_dyneither(_tmp96C,sizeof(void*),1)))))))));}goto _LL25B;_LL264: {
struct Cyc_Absyn_Float_p_struct*_tmp446=(struct Cyc_Absyn_Float_p_struct*)_tmp43E;
if(_tmp446->tag != 11)goto _LL266;else{_tmp447=_tmp446->f1;}}_LL265: s=Cyc_PP_text(
_tmp447);goto _LL25B;_LL266: {struct Cyc_Absyn_Var_p_struct*_tmp448=(struct Cyc_Absyn_Var_p_struct*)
_tmp43E;if(_tmp448->tag != 1)goto _LL268;else{_tmp449=_tmp448->f1;_tmp44A=_tmp448->f2;
_tmp44B=*_tmp44A;_tmp44C=_tmp44B.r;{struct Cyc_Absyn_Wild_p_struct*_tmp44D=(
struct Cyc_Absyn_Wild_p_struct*)_tmp44C;if(_tmp44D->tag != 0)goto _LL268;};}}_LL267:
s=Cyc_Absynpp_qvar2doc(_tmp449->name);goto _LL25B;_LL268: {struct Cyc_Absyn_Var_p_struct*
_tmp44E=(struct Cyc_Absyn_Var_p_struct*)_tmp43E;if(_tmp44E->tag != 1)goto _LL26A;
else{_tmp44F=_tmp44E->f1;_tmp450=_tmp44E->f2;}}_LL269:{const char*_tmp970;struct
Cyc_PP_Doc*_tmp96F[3];s=((_tmp96F[2]=Cyc_Absynpp_pat2doc(_tmp450),((_tmp96F[1]=
Cyc_PP_text(((_tmp970=" as ",_tag_dyneither(_tmp970,sizeof(char),5)))),((_tmp96F[
0]=Cyc_Absynpp_qvar2doc(_tmp44F->name),Cyc_PP_cat(_tag_dyneither(_tmp96F,sizeof(
struct Cyc_PP_Doc*),3))))))));}goto _LL25B;_LL26A: {struct Cyc_Absyn_TagInt_p_struct*
_tmp451=(struct Cyc_Absyn_TagInt_p_struct*)_tmp43E;if(_tmp451->tag != 3)goto _LL26C;
else{_tmp452=_tmp451->f1;_tmp453=_tmp451->f2;}}_LL26B:{const char*_tmp975;const
char*_tmp974;struct Cyc_PP_Doc*_tmp973[4];s=((_tmp973[3]=Cyc_PP_text(((_tmp974=">",
_tag_dyneither(_tmp974,sizeof(char),2)))),((_tmp973[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp452)),((_tmp973[1]=Cyc_PP_text(((_tmp975="<",_tag_dyneither(_tmp975,sizeof(
char),2)))),((_tmp973[0]=Cyc_Absynpp_qvar2doc(_tmp453->name),Cyc_PP_cat(
_tag_dyneither(_tmp973,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL25B;_LL26C: {
struct Cyc_Absyn_Tuple_p_struct*_tmp454=(struct Cyc_Absyn_Tuple_p_struct*)_tmp43E;
if(_tmp454->tag != 4)goto _LL26E;else{_tmp455=_tmp454->f1;_tmp456=_tmp454->f2;}}
_LL26D:{const char*_tmp97E;const char*_tmp97D;const char*_tmp97C;const char*_tmp97B;
struct Cyc_PP_Doc*_tmp97A[4];s=((_tmp97A[3]=_tmp456?Cyc_PP_text(((_tmp97B=", ...)",
_tag_dyneither(_tmp97B,sizeof(char),7)))): Cyc_PP_text(((_tmp97C=")",
_tag_dyneither(_tmp97C,sizeof(char),2)))),((_tmp97A[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp97D=",",_tag_dyneither(_tmp97D,
sizeof(char),2))),_tmp455),((_tmp97A[1]=Cyc_PP_text(((_tmp97E="(",_tag_dyneither(
_tmp97E,sizeof(char),2)))),((_tmp97A[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp97A,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL25B;_LL26E: {
struct Cyc_Absyn_Pointer_p_struct*_tmp457=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp43E;if(_tmp457->tag != 5)goto _LL270;else{_tmp458=_tmp457->f1;}}_LL26F:{const
char*_tmp981;struct Cyc_PP_Doc*_tmp980[2];s=((_tmp980[1]=Cyc_Absynpp_pat2doc(
_tmp458),((_tmp980[0]=Cyc_PP_text(((_tmp981="&",_tag_dyneither(_tmp981,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp980,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL25B;_LL270: {struct Cyc_Absyn_Reference_p_struct*_tmp459=(struct Cyc_Absyn_Reference_p_struct*)
_tmp43E;if(_tmp459->tag != 2)goto _LL272;else{_tmp45A=_tmp459->f1;_tmp45B=_tmp459->f2;
_tmp45C=*_tmp45B;_tmp45D=_tmp45C.r;{struct Cyc_Absyn_Wild_p_struct*_tmp45E=(
struct Cyc_Absyn_Wild_p_struct*)_tmp45D;if(_tmp45E->tag != 0)goto _LL272;};}}_LL271:{
const char*_tmp984;struct Cyc_PP_Doc*_tmp983[2];s=((_tmp983[1]=Cyc_Absynpp_qvar2doc(
_tmp45A->name),((_tmp983[0]=Cyc_PP_text(((_tmp984="*",_tag_dyneither(_tmp984,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp983,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL25B;_LL272: {struct Cyc_Absyn_Reference_p_struct*_tmp45F=(struct Cyc_Absyn_Reference_p_struct*)
_tmp43E;if(_tmp45F->tag != 2)goto _LL274;else{_tmp460=_tmp45F->f1;_tmp461=_tmp45F->f2;}}
_LL273:{const char*_tmp989;const char*_tmp988;struct Cyc_PP_Doc*_tmp987[4];s=((
_tmp987[3]=Cyc_Absynpp_pat2doc(_tmp461),((_tmp987[2]=Cyc_PP_text(((_tmp988=" as ",
_tag_dyneither(_tmp988,sizeof(char),5)))),((_tmp987[1]=Cyc_Absynpp_qvar2doc(
_tmp460->name),((_tmp987[0]=Cyc_PP_text(((_tmp989="*",_tag_dyneither(_tmp989,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp987,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL25B;_LL274: {struct Cyc_Absyn_UnknownId_p_struct*_tmp462=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp43E;if(_tmp462->tag != 14)goto _LL276;else{_tmp463=_tmp462->f1;}}_LL275: s=Cyc_Absynpp_qvar2doc(
_tmp463);goto _LL25B;_LL276: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp464=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp43E;if(_tmp464->tag != 15)goto _LL278;
else{_tmp465=_tmp464->f1;_tmp466=_tmp464->f2;_tmp467=_tmp464->f3;}}_LL277: {
const char*_tmp98B;const char*_tmp98A;struct _dyneither_ptr term=_tmp467?(_tmp98B=", ...)",
_tag_dyneither(_tmp98B,sizeof(char),7)):((_tmp98A=")",_tag_dyneither(_tmp98A,
sizeof(char),2)));{const char*_tmp990;const char*_tmp98F;struct Cyc_PP_Doc*_tmp98E[
2];s=((_tmp98E[1]=Cyc_PP_group(((_tmp990="(",_tag_dyneither(_tmp990,sizeof(char),
2))),term,((_tmp98F=",",_tag_dyneither(_tmp98F,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp466)),((_tmp98E[0]=Cyc_Absynpp_qvar2doc(_tmp465),Cyc_PP_cat(
_tag_dyneither(_tmp98E,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL25B;}_LL278: {
struct Cyc_Absyn_Aggr_p_struct*_tmp468=(struct Cyc_Absyn_Aggr_p_struct*)_tmp43E;
if(_tmp468->tag != 6)goto _LL27A;else{_tmp469=_tmp468->f1;if(_tmp469 == 0)goto
_LL27A;_tmp46A=*_tmp469;_tmp46B=_tmp46A.aggr_info;_tmp46C=_tmp468->f2;_tmp46D=
_tmp468->f3;_tmp46E=_tmp468->f4;}}_LL279: {const char*_tmp992;const char*_tmp991;
struct _dyneither_ptr term=_tmp46E?(_tmp992=", ...}",_tag_dyneither(_tmp992,
sizeof(char),7)):((_tmp991="}",_tag_dyneither(_tmp991,sizeof(char),2)));struct
_tuple0*_tmp4A3;struct _tuple10 _tmp4A2=Cyc_Absyn_aggr_kinded_name(_tmp46B);
_tmp4A3=_tmp4A2.f2;{const char*_tmp99D;const char*_tmp99C;const char*_tmp99B;const
char*_tmp99A;const char*_tmp999;struct Cyc_PP_Doc*_tmp998[4];s=((_tmp998[3]=Cyc_PP_group(((
_tmp99A="",_tag_dyneither(_tmp99A,sizeof(char),1))),term,((_tmp999=",",
_tag_dyneither(_tmp999,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp46D)),((_tmp998[2]=Cyc_PP_egroup(((_tmp99D="[",_tag_dyneither(_tmp99D,
sizeof(char),2))),((_tmp99C="]",_tag_dyneither(_tmp99C,sizeof(char),2))),((
_tmp99B=",",_tag_dyneither(_tmp99B,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp46C))),((_tmp998[1]=
Cyc_Absynpp_lb(),((_tmp998[0]=Cyc_Absynpp_qvar2doc(_tmp4A3),Cyc_PP_cat(
_tag_dyneither(_tmp998,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL25B;}_LL27A: {
struct Cyc_Absyn_Aggr_p_struct*_tmp46F=(struct Cyc_Absyn_Aggr_p_struct*)_tmp43E;
if(_tmp46F->tag != 6)goto _LL27C;else{_tmp470=_tmp46F->f1;if(_tmp470 != 0)goto
_LL27C;_tmp471=_tmp46F->f2;_tmp472=_tmp46F->f3;_tmp473=_tmp46F->f4;}}_LL27B: {
const char*_tmp99F;const char*_tmp99E;struct _dyneither_ptr term=_tmp473?(_tmp99F=", ...}",
_tag_dyneither(_tmp99F,sizeof(char),7)):((_tmp99E="}",_tag_dyneither(_tmp99E,
sizeof(char),2)));{const char*_tmp9AA;const char*_tmp9A9;const char*_tmp9A8;const
char*_tmp9A7;const char*_tmp9A6;struct Cyc_PP_Doc*_tmp9A5[3];s=((_tmp9A5[2]=Cyc_PP_group(((
_tmp9A7="",_tag_dyneither(_tmp9A7,sizeof(char),1))),term,((_tmp9A6=",",
_tag_dyneither(_tmp9A6,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp472)),((_tmp9A5[1]=Cyc_PP_egroup(((_tmp9AA="[",_tag_dyneither(_tmp9AA,
sizeof(char),2))),((_tmp9A9="]",_tag_dyneither(_tmp9A9,sizeof(char),2))),((
_tmp9A8=",",_tag_dyneither(_tmp9A8,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp471))),((_tmp9A5[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp9A5,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL25B;}_LL27C: {struct Cyc_Absyn_Enum_p_struct*_tmp474=(struct Cyc_Absyn_Enum_p_struct*)
_tmp43E;if(_tmp474->tag != 12)goto _LL27E;else{_tmp475=_tmp474->f2;}}_LL27D:
_tmp477=_tmp475;goto _LL27F;_LL27E: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp476=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp43E;if(_tmp476->tag != 13)goto _LL280;else{
_tmp477=_tmp476->f2;}}_LL27F: s=Cyc_Absynpp_qvar2doc(_tmp477->name);goto _LL25B;
_LL280: {struct Cyc_Absyn_Datatype_p_struct*_tmp478=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp43E;if(_tmp478->tag != 7)goto _LL282;else{_tmp479=_tmp478->f2;_tmp47A=_tmp478->f3;
if(_tmp47A != 0)goto _LL282;}}_LL281: s=Cyc_Absynpp_qvar2doc(_tmp479->name);goto
_LL25B;_LL282: {struct Cyc_Absyn_Datatype_p_struct*_tmp47B=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp43E;if(_tmp47B->tag != 7)goto _LL284;else{_tmp47C=_tmp47B->f2;_tmp47D=_tmp47B->f3;
_tmp47E=_tmp47B->f4;}}_LL283: {const char*_tmp9AC;const char*_tmp9AB;struct
_dyneither_ptr term=_tmp47E?(_tmp9AC=", ...)",_tag_dyneither(_tmp9AC,sizeof(char),
7)):((_tmp9AB=")",_tag_dyneither(_tmp9AB,sizeof(char),2)));{const char*_tmp9B1;
const char*_tmp9B0;struct Cyc_PP_Doc*_tmp9AF[2];s=((_tmp9AF[1]=Cyc_PP_egroup(((
_tmp9B1="(",_tag_dyneither(_tmp9B1,sizeof(char),2))),term,((_tmp9B0=",",
_tag_dyneither(_tmp9B0,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp47D)),((_tmp9AF[0]=Cyc_Absynpp_qvar2doc(_tmp47C->name),Cyc_PP_cat(
_tag_dyneither(_tmp9AF,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL25B;}_LL284: {
struct Cyc_Absyn_Exp_p_struct*_tmp47F=(struct Cyc_Absyn_Exp_p_struct*)_tmp43E;if(
_tmp47F->tag != 16)goto _LL25B;else{_tmp480=_tmp47F->f1;}}_LL285: s=Cyc_Absynpp_exp2doc(
_tmp480);goto _LL25B;_LL25B:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple14*dp){const char*_tmp9B8;const char*_tmp9B7;const char*_tmp9B6;struct Cyc_PP_Doc*
_tmp9B5[2];return(_tmp9B5[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp9B5[0]=Cyc_PP_egroup(((
_tmp9B8="",_tag_dyneither(_tmp9B8,sizeof(char),1))),((_tmp9B7="=",_tag_dyneither(
_tmp9B7,sizeof(char),2))),((_tmp9B6="=",_tag_dyneither(_tmp9B6,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp9B5,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (c->pattern)->r == (void*)&
Cyc_Absyn_Wild_p_val){const char*_tmp9BD;struct Cyc_PP_Doc*_tmp9BC[2];struct Cyc_PP_Doc*
_tmp9BB[2];return(_tmp9BB[1]=Cyc_PP_nest(2,((_tmp9BC[1]=Cyc_Absynpp_stmt2doc(c->body,
0),((_tmp9BC[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9BC,sizeof(
struct Cyc_PP_Doc*),2))))))),((_tmp9BB[0]=Cyc_PP_text(((_tmp9BD="default: ",
_tag_dyneither(_tmp9BD,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp9BB,
sizeof(struct Cyc_PP_Doc*),2)))));}else{if(c->where_clause == 0){const char*_tmp9C4;
const char*_tmp9C3;struct Cyc_PP_Doc*_tmp9C2[2];struct Cyc_PP_Doc*_tmp9C1[4];return(
_tmp9C1[3]=Cyc_PP_nest(2,((_tmp9C2[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9C2[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9C2,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp9C1[2]=Cyc_PP_text(((_tmp9C3=": ",_tag_dyneither(_tmp9C3,sizeof(
char),3)))),((_tmp9C1[1]=Cyc_Absynpp_pat2doc(c->pattern),((_tmp9C1[0]=Cyc_PP_text(((
_tmp9C4="case ",_tag_dyneither(_tmp9C4,sizeof(char),6)))),Cyc_PP_cat(
_tag_dyneither(_tmp9C1,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{const char*
_tmp9CD;const char*_tmp9CC;const char*_tmp9CB;struct Cyc_PP_Doc*_tmp9CA[2];struct
Cyc_PP_Doc*_tmp9C9[6];return(_tmp9C9[5]=Cyc_PP_nest(2,((_tmp9CA[1]=Cyc_Absynpp_stmt2doc(
c->body,0),((_tmp9CA[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9CA,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9C9[4]=Cyc_PP_text(((_tmp9CB=": ",
_tag_dyneither(_tmp9CB,sizeof(char),3)))),((_tmp9C9[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp9C9[2]=Cyc_PP_text(((
_tmp9CC=" && ",_tag_dyneither(_tmp9CC,sizeof(char),5)))),((_tmp9C9[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp9C9[0]=Cyc_PP_text(((_tmp9CD="case ",_tag_dyneither(_tmp9CD,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9C9,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*
_tmp9CE;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp9CE="",_tag_dyneither(_tmp9CE,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);
else{const char*_tmp9D1;struct Cyc_PP_Doc*_tmp9D0[3];return(_tmp9D0[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp9D0[1]=Cyc_PP_text(((_tmp9D1=" = ",
_tag_dyneither(_tmp9D1,sizeof(char),4)))),((_tmp9D0[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp9D0,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){const char*_tmp9D2;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((
_tmp9D2=",",_tag_dyneither(_tmp9D2,sizeof(char),2))),fs);}static struct Cyc_PP_Doc*
Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(v->name);}
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){const char*
_tmp9D3;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((
_tmp9D3=",",_tag_dyneither(_tmp9D3,sizeof(char),2))),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(
struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl _tmp4CF;enum Cyc_Absyn_Scope
_tmp4D0;struct _tuple0*_tmp4D1;struct Cyc_Absyn_Tqual _tmp4D2;void*_tmp4D3;struct
Cyc_Absyn_Exp*_tmp4D4;struct Cyc_List_List*_tmp4D5;struct Cyc_Absyn_Vardecl*
_tmp4CE=vd;_tmp4CF=*_tmp4CE;_tmp4D0=_tmp4CF.sc;_tmp4D1=_tmp4CF.name;_tmp4D2=
_tmp4CF.tq;_tmp4D3=_tmp4CF.type;_tmp4D4=_tmp4CF.initializer;_tmp4D5=_tmp4CF.attributes;{
struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp4D1);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4D5);struct Cyc_PP_Doc*
beforenamedoc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL286:
beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL287:{void*_tmp4D6=Cyc_Tcutil_compress(
_tmp4D3);struct Cyc_Absyn_FnInfo _tmp4D8;struct Cyc_List_List*_tmp4D9;_LL28A: {
struct Cyc_Absyn_FnType_struct*_tmp4D7=(struct Cyc_Absyn_FnType_struct*)_tmp4D6;
if(_tmp4D7->tag != 9)goto _LL28C;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D8.attributes;}}
_LL28B: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4D9);goto _LL289;_LL28C:;_LL28D:
beforenamedoc=Cyc_PP_nil_doc();goto _LL289;_LL289:;}break;}{struct Cyc_PP_Doc*
tmp_doc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL28E: tmp_doc=Cyc_PP_nil_doc();
break;case Cyc_Cyclone_Vc_c: _LL28F: tmp_doc=attsdoc;break;}{struct Cyc_Core_Opt*
_tmp9E0;struct Cyc_PP_Doc*_tmp9DF[2];struct Cyc_PP_Doc*_tmp9DE[2];const char*
_tmp9DD;const char*_tmp9DC;struct Cyc_PP_Doc*_tmp9DB[5];s=((_tmp9DB[4]=Cyc_PP_text(((
_tmp9DC=";",_tag_dyneither(_tmp9DC,sizeof(char),2)))),((_tmp9DB[3]=_tmp4D4 == 0?
Cyc_PP_nil_doc():((_tmp9DE[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4D4),((
_tmp9DE[0]=Cyc_PP_text(((_tmp9DD=" = ",_tag_dyneither(_tmp9DD,sizeof(char),4)))),
Cyc_PP_cat(_tag_dyneither(_tmp9DE,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9DB[2]=
Cyc_Absynpp_tqtd2doc(_tmp4D2,_tmp4D3,((_tmp9E0=_cycalloc(sizeof(*_tmp9E0)),((
_tmp9E0->v=((_tmp9DF[1]=sn,((_tmp9DF[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(
_tmp9DF,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9E0))))),((_tmp9DB[1]=Cyc_Absynpp_scope2doc(
_tmp4D0),((_tmp9DB[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp9DB,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}return s;};};}struct _tuple18{struct Cyc_Position_Segment*f1;struct
_tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple18*x){
struct _tuple0*_tmp4E1;struct _tuple18 _tmp4E0=*x;_tmp4E1=_tmp4E0.f2;return Cyc_Absynpp_qvar2doc(
_tmp4E1);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){struct Cyc_PP_Doc*_tmp9E1[4];return(_tmp9E1[3]=Cyc_Absynpp_ktvars2doc(
ad->tvs),((_tmp9E1[2]=Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9E1[1]=Cyc_Absynpp_aggr_kind2doc(
ad->kind),((_tmp9E1[0]=Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(
_tmp9E1,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{struct Cyc_PP_Doc*_tmp9E9[2];
const char*_tmp9E8;struct Cyc_PP_Doc*_tmp9E7[2];struct Cyc_PP_Doc*_tmp9E6[12];
return(_tmp9E6[11]=Cyc_Absynpp_atts2doc(ad->attributes),((_tmp9E6[10]=Cyc_Absynpp_rb(),((
_tmp9E6[9]=Cyc_PP_line_doc(),((_tmp9E6[8]=Cyc_PP_nest(2,((_tmp9E7[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmp9E7[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp9E7,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9E6[7]=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():((
_tmp9E9[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((
_tmp9E9[0]=Cyc_PP_text(((_tmp9E8=":",_tag_dyneither(_tmp9E8,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp9E9,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9E6[6]=
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((
_tmp9E6[5]=Cyc_Absynpp_lb(),((_tmp9E6[4]=Cyc_PP_blank_doc(),((_tmp9E6[3]=Cyc_Absynpp_ktvars2doc(
ad->tvs),((_tmp9E6[2]=Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9E6[1]=Cyc_Absynpp_aggr_kind2doc(
ad->kind),((_tmp9E6[0]=Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(
_tmp9E6,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){struct Cyc_Absyn_Datatypedecl
_tmp4E8;enum Cyc_Absyn_Scope _tmp4E9;struct _tuple0*_tmp4EA;struct Cyc_List_List*
_tmp4EB;struct Cyc_Core_Opt*_tmp4EC;int _tmp4ED;struct Cyc_Absyn_Datatypedecl*
_tmp4E7=dd;_tmp4E8=*_tmp4E7;_tmp4E9=_tmp4E8.sc;_tmp4EA=_tmp4E8.name;_tmp4EB=
_tmp4E8.tvs;_tmp4EC=_tmp4E8.fields;_tmp4ED=_tmp4E8.is_extensible;if(_tmp4EC == 0){
const char*_tmp9EE;const char*_tmp9ED;struct Cyc_PP_Doc*_tmp9EC[5];return(_tmp9EC[4]=
Cyc_Absynpp_ktvars2doc(_tmp4EB),((_tmp9EC[3]=_tmp4ED?Cyc_Absynpp_qvar2bolddoc(
_tmp4EA): Cyc_Absynpp_typedef_name2bolddoc(_tmp4EA),((_tmp9EC[2]=Cyc_PP_text(((
_tmp9ED="datatype ",_tag_dyneither(_tmp9ED,sizeof(char),10)))),((_tmp9EC[1]=
_tmp4ED?Cyc_PP_text(((_tmp9EE="@extensible",_tag_dyneither(_tmp9EE,sizeof(char),
12)))): Cyc_PP_blank_doc(),((_tmp9EC[0]=Cyc_Absynpp_scope2doc(_tmp4E9),Cyc_PP_cat(
_tag_dyneither(_tmp9EC,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{const char*
_tmp9F5;const char*_tmp9F4;struct Cyc_PP_Doc*_tmp9F3[2];struct Cyc_PP_Doc*_tmp9F2[
10];return(_tmp9F2[9]=Cyc_Absynpp_rb(),((_tmp9F2[8]=Cyc_PP_line_doc(),((_tmp9F2[
7]=Cyc_PP_nest(2,((_tmp9F3[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)
_tmp4EC->v),((_tmp9F3[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9F3,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9F2[6]=Cyc_Absynpp_lb(),((_tmp9F2[5]=
Cyc_PP_blank_doc(),((_tmp9F2[4]=Cyc_Absynpp_ktvars2doc(_tmp4EB),((_tmp9F2[3]=
_tmp4ED?Cyc_Absynpp_qvar2bolddoc(_tmp4EA): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4EA),((_tmp9F2[2]=Cyc_PP_text(((_tmp9F4="datatype ",_tag_dyneither(_tmp9F4,
sizeof(char),10)))),((_tmp9F2[1]=_tmp4ED?Cyc_PP_text(((_tmp9F5="@extensible ",
_tag_dyneither(_tmp9F5,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9F2[0]=Cyc_Absynpp_scope2doc(
_tmp4E9),Cyc_PP_cat(_tag_dyneither(_tmp9F2,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){struct Cyc_Absyn_Enumdecl
_tmp4F6;enum Cyc_Absyn_Scope _tmp4F7;struct _tuple0*_tmp4F8;struct Cyc_Core_Opt*
_tmp4F9;struct Cyc_Absyn_Enumdecl*_tmp4F5=ed;_tmp4F6=*_tmp4F5;_tmp4F7=_tmp4F6.sc;
_tmp4F8=_tmp4F6.name;_tmp4F9=_tmp4F6.fields;if(_tmp4F9 == 0){const char*_tmp9F8;
struct Cyc_PP_Doc*_tmp9F7[3];return(_tmp9F7[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp4F8),((_tmp9F7[1]=Cyc_PP_text(((_tmp9F8="enum ",_tag_dyneither(_tmp9F8,
sizeof(char),6)))),((_tmp9F7[0]=Cyc_Absynpp_scope2doc(_tmp4F7),Cyc_PP_cat(
_tag_dyneither(_tmp9F7,sizeof(struct Cyc_PP_Doc*),3)))))));}else{const char*
_tmp9FD;struct Cyc_PP_Doc*_tmp9FC[2];struct Cyc_PP_Doc*_tmp9FB[8];return(_tmp9FB[7]=
Cyc_Absynpp_rb(),((_tmp9FB[6]=Cyc_PP_line_doc(),((_tmp9FB[5]=Cyc_PP_nest(2,((
_tmp9FC[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp4F9->v),((
_tmp9FC[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9FC,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp9FB[4]=Cyc_Absynpp_lb(),((_tmp9FB[3]=Cyc_PP_blank_doc(),((_tmp9FB[
2]=Cyc_Absynpp_qvar2bolddoc(_tmp4F8),((_tmp9FB[1]=Cyc_PP_text(((_tmp9FD="enum ",
_tag_dyneither(_tmp9FD,sizeof(char),6)))),((_tmp9FB[0]=Cyc_Absynpp_scope2doc(
_tmp4F7),Cyc_PP_cat(_tag_dyneither(_tmp9FB,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{
void*_tmp4FF=d->r;struct Cyc_Absyn_Fndecl*_tmp501;struct Cyc_Absyn_Aggrdecl*
_tmp503;struct Cyc_Absyn_Vardecl*_tmp505;struct Cyc_Absyn_Tvar*_tmp507;struct Cyc_Absyn_Vardecl*
_tmp508;int _tmp509;struct Cyc_Absyn_Exp*_tmp50A;struct Cyc_Absyn_Exp*_tmp50C;
struct Cyc_Absyn_Tvar*_tmp50D;struct Cyc_Absyn_Vardecl*_tmp50E;struct Cyc_Absyn_Datatypedecl*
_tmp510;struct Cyc_Absyn_Pat*_tmp512;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_List_List*
_tmp515;struct Cyc_Absyn_Enumdecl*_tmp517;struct Cyc_Absyn_Typedefdecl*_tmp519;
struct _dyneither_ptr*_tmp51B;struct Cyc_List_List*_tmp51C;struct _tuple0*_tmp51E;
struct Cyc_List_List*_tmp51F;struct Cyc_List_List*_tmp521;struct Cyc_List_List*
_tmp523;struct Cyc_List_List*_tmp524;_LL292: {struct Cyc_Absyn_Fn_d_struct*_tmp500=(
struct Cyc_Absyn_Fn_d_struct*)_tmp4FF;if(_tmp500->tag != 1)goto _LL294;else{_tmp501=
_tmp500->f1;}}_LL293: {struct Cyc_Absyn_FnType_struct _tmpA03;struct Cyc_Absyn_FnInfo
_tmpA02;struct Cyc_Absyn_FnType_struct*_tmpA01;void*t=(void*)((_tmpA01=_cycalloc(
sizeof(*_tmpA01)),((_tmpA01[0]=((_tmpA03.tag=9,((_tmpA03.f1=((_tmpA02.tvars=
_tmp501->tvs,((_tmpA02.effect=_tmp501->effect,((_tmpA02.ret_tqual=_tmp501->ret_tqual,((
_tmpA02.ret_typ=_tmp501->ret_type,((_tmpA02.args=((struct Cyc_List_List*(*)(
struct _tuple8*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp501->args),((_tmpA02.c_varargs=_tmp501->c_varargs,((_tmpA02.cyc_varargs=
_tmp501->cyc_varargs,((_tmpA02.rgn_po=_tmp501->rgn_po,((_tmpA02.attributes=0,
_tmpA02)))))))))))))))))),_tmpA03)))),_tmpA01))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp501->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp501->is_inline)switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL2B2:{const char*_tmpA04;inlinedoc=Cyc_PP_text(((_tmpA04="inline ",
_tag_dyneither(_tmpA04,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2B3:{
const char*_tmpA05;inlinedoc=Cyc_PP_text(((_tmpA05="__inline ",_tag_dyneither(
_tmpA05,sizeof(char),10))));}break;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc(_tmp501->sc);struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2B5: beforenamedoc=
attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2B6: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp501->attributes);break;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp501->name);struct Cyc_PP_Doc*_tmpA08[2];struct Cyc_Core_Opt*_tmpA07;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmpA07=_cycalloc(
sizeof(*_tmpA07)),((_tmpA07->v=((_tmpA08[1]=namedoc,((_tmpA08[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmpA08,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA07)))));
struct Cyc_PP_Doc*_tmpA0B[2];struct Cyc_PP_Doc*_tmpA0A[5];struct Cyc_PP_Doc*bodydoc=(
_tmpA0A[4]=Cyc_Absynpp_rb(),((_tmpA0A[3]=Cyc_PP_line_doc(),((_tmpA0A[2]=Cyc_PP_nest(
2,((_tmpA0B[1]=Cyc_Absynpp_stmt2doc(_tmp501->body,0),((_tmpA0B[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmpA0B,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA0A[1]=
Cyc_Absynpp_lb(),((_tmpA0A[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmpA0A,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmpA0C[4];s=((
_tmpA0C[3]=bodydoc,((_tmpA0C[2]=tqtddoc,((_tmpA0C[1]=scopedoc,((_tmpA0C[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmpA0C,sizeof(struct Cyc_PP_Doc*),4))))))))));}
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2B8:{struct Cyc_PP_Doc*
_tmpA0D[2];s=((_tmpA0D[1]=s,((_tmpA0D[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(
_tmpA0D,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2B9: break;}goto
_LL291;};};}_LL294: {struct Cyc_Absyn_Aggr_d_struct*_tmp502=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4FF;if(_tmp502->tag != 6)goto _LL296;else{_tmp503=_tmp502->f1;}}_LL295:{const
char*_tmpA10;struct Cyc_PP_Doc*_tmpA0F[2];s=((_tmpA0F[1]=Cyc_PP_text(((_tmpA10=";",
_tag_dyneither(_tmpA10,sizeof(char),2)))),((_tmpA0F[0]=Cyc_Absynpp_aggrdecl2doc(
_tmp503),Cyc_PP_cat(_tag_dyneither(_tmpA0F,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL291;_LL296: {struct Cyc_Absyn_Var_d_struct*_tmp504=(struct Cyc_Absyn_Var_d_struct*)
_tmp4FF;if(_tmp504->tag != 0)goto _LL298;else{_tmp505=_tmp504->f1;}}_LL297: s=Cyc_Absynpp_vardecl2doc(
_tmp505);goto _LL291;_LL298: {struct Cyc_Absyn_Region_d_struct*_tmp506=(struct Cyc_Absyn_Region_d_struct*)
_tmp4FF;if(_tmp506->tag != 4)goto _LL29A;else{_tmp507=_tmp506->f1;_tmp508=_tmp506->f2;
_tmp509=_tmp506->f3;_tmp50A=_tmp506->f4;}}_LL299:{const char*_tmpA23;const char*
_tmpA22;const char*_tmpA21;const char*_tmpA20;struct Cyc_PP_Doc*_tmpA1F[3];const
char*_tmpA1E;const char*_tmpA1D;const char*_tmpA1C;struct Cyc_PP_Doc*_tmpA1B[8];s=((
_tmpA1B[7]=Cyc_PP_text(((_tmpA1C=";",_tag_dyneither(_tmpA1C,sizeof(char),2)))),((
_tmpA1B[6]=_tmp50A != 0?(_tmpA1F[2]=Cyc_PP_text(((_tmpA1E=")",_tag_dyneither(
_tmpA1E,sizeof(char),2)))),((_tmpA1F[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp50A),((_tmpA1F[0]=Cyc_PP_text(((_tmpA1D=" = open(",_tag_dyneither(_tmpA1D,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA1F,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmpA1B[5]=_tmp509?Cyc_PP_text(((_tmpA20=" @resetable",
_tag_dyneither(_tmpA20,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmpA1B[4]=Cyc_Absynpp_qvar2doc(
_tmp508->name),((_tmpA1B[3]=Cyc_PP_text(((_tmpA21=">",_tag_dyneither(_tmpA21,
sizeof(char),2)))),((_tmpA1B[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp507)),((
_tmpA1B[1]=Cyc_PP_text(((_tmpA22="<",_tag_dyneither(_tmpA22,sizeof(char),2)))),((
_tmpA1B[0]=Cyc_PP_text(((_tmpA23="region",_tag_dyneither(_tmpA23,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmpA1B,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL291;_LL29A: {struct Cyc_Absyn_Alias_d_struct*_tmp50B=(struct Cyc_Absyn_Alias_d_struct*)
_tmp4FF;if(_tmp50B->tag != 5)goto _LL29C;else{_tmp50C=_tmp50B->f1;_tmp50D=_tmp50B->f2;
_tmp50E=_tmp50B->f3;}}_LL29B:{const char*_tmpA2E;const char*_tmpA2D;const char*
_tmpA2C;const char*_tmpA2B;const char*_tmpA2A;struct Cyc_PP_Doc*_tmpA29[8];s=((
_tmpA29[7]=Cyc_PP_text(((_tmpA2A=";",_tag_dyneither(_tmpA2A,sizeof(char),2)))),((
_tmpA29[6]=Cyc_Absynpp_exp2doc(_tmp50C),((_tmpA29[5]=Cyc_PP_text(((_tmpA2B=" = ",
_tag_dyneither(_tmpA2B,sizeof(char),4)))),((_tmpA29[4]=Cyc_Absynpp_qvar2doc(
_tmp50E->name),((_tmpA29[3]=Cyc_PP_text(((_tmpA2C="> ",_tag_dyneither(_tmpA2C,
sizeof(char),3)))),((_tmpA29[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp50D)),((
_tmpA29[1]=Cyc_PP_text(((_tmpA2D=" <",_tag_dyneither(_tmpA2D,sizeof(char),3)))),((
_tmpA29[0]=Cyc_PP_text(((_tmpA2E="alias",_tag_dyneither(_tmpA2E,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmpA29,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL291;_LL29C: {struct Cyc_Absyn_Datatype_d_struct*_tmp50F=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp4FF;if(_tmp50F->tag != 7)goto _LL29E;else{_tmp510=_tmp50F->f1;}}_LL29D:{const
char*_tmpA31;struct Cyc_PP_Doc*_tmpA30[2];s=((_tmpA30[1]=Cyc_PP_text(((_tmpA31=";",
_tag_dyneither(_tmpA31,sizeof(char),2)))),((_tmpA30[0]=Cyc_Absynpp_datatypedecl2doc(
_tmp510),Cyc_PP_cat(_tag_dyneither(_tmpA30,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL291;_LL29E: {struct Cyc_Absyn_Let_d_struct*_tmp511=(struct Cyc_Absyn_Let_d_struct*)
_tmp4FF;if(_tmp511->tag != 2)goto _LL2A0;else{_tmp512=_tmp511->f1;_tmp513=_tmp511->f3;}}
_LL29F:{const char*_tmpA38;const char*_tmpA37;const char*_tmpA36;struct Cyc_PP_Doc*
_tmpA35[5];s=((_tmpA35[4]=Cyc_PP_text(((_tmpA36=";",_tag_dyneither(_tmpA36,
sizeof(char),2)))),((_tmpA35[3]=Cyc_Absynpp_exp2doc(_tmp513),((_tmpA35[2]=Cyc_PP_text(((
_tmpA37=" = ",_tag_dyneither(_tmpA37,sizeof(char),4)))),((_tmpA35[1]=Cyc_Absynpp_pat2doc(
_tmp512),((_tmpA35[0]=Cyc_PP_text(((_tmpA38="let ",_tag_dyneither(_tmpA38,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA35,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL291;_LL2A0: {struct Cyc_Absyn_Letv_d_struct*_tmp514=(struct Cyc_Absyn_Letv_d_struct*)
_tmp4FF;if(_tmp514->tag != 3)goto _LL2A2;else{_tmp515=_tmp514->f1;}}_LL2A1:{const
char*_tmpA3D;const char*_tmpA3C;struct Cyc_PP_Doc*_tmpA3B[3];s=((_tmpA3B[2]=Cyc_PP_text(((
_tmpA3C=";",_tag_dyneither(_tmpA3C,sizeof(char),2)))),((_tmpA3B[1]=Cyc_Absynpp_ids2doc(
_tmp515),((_tmpA3B[0]=Cyc_PP_text(((_tmpA3D="let ",_tag_dyneither(_tmpA3D,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA3B,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL291;_LL2A2: {struct Cyc_Absyn_Enum_d_struct*_tmp516=(struct Cyc_Absyn_Enum_d_struct*)
_tmp4FF;if(_tmp516->tag != 8)goto _LL2A4;else{_tmp517=_tmp516->f1;}}_LL2A3:{const
char*_tmpA40;struct Cyc_PP_Doc*_tmpA3F[2];s=((_tmpA3F[1]=Cyc_PP_text(((_tmpA40=";",
_tag_dyneither(_tmpA40,sizeof(char),2)))),((_tmpA3F[0]=Cyc_Absynpp_enumdecl2doc(
_tmp517),Cyc_PP_cat(_tag_dyneither(_tmpA3F,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL291;_LL2A4: {struct Cyc_Absyn_Typedef_d_struct*_tmp518=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp4FF;if(_tmp518->tag != 9)goto _LL2A6;else{_tmp519=_tmp518->f1;}}_LL2A5: {void*
t;if(_tmp519->defn != 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp519->defn))->v;
else{t=Cyc_Absyn_new_evar(_tmp519->kind,0);}{const char*_tmpA4A;struct Cyc_Core_Opt*
_tmpA49;struct Cyc_PP_Doc*_tmpA48[2];const char*_tmpA47;struct Cyc_PP_Doc*_tmpA46[4];
s=((_tmpA46[3]=Cyc_PP_text(((_tmpA47=";",_tag_dyneither(_tmpA47,sizeof(char),2)))),((
_tmpA46[2]=Cyc_Absynpp_atts2doc(_tmp519->atts),((_tmpA46[1]=Cyc_Absynpp_tqtd2doc(
_tmp519->tq,t,((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49->v=((_tmpA48[1]=
Cyc_Absynpp_tvars2doc(_tmp519->tvs),((_tmpA48[0]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp519->name),Cyc_PP_cat(_tag_dyneither(_tmpA48,sizeof(struct Cyc_PP_Doc*),2)))))),
_tmpA49))))),((_tmpA46[0]=Cyc_PP_text(((_tmpA4A="typedef ",_tag_dyneither(
_tmpA4A,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA46,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL291;}_LL2A6: {struct Cyc_Absyn_Namespace_d_struct*_tmp51A=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp4FF;if(_tmp51A->tag != 10)goto _LL2A8;
else{_tmp51B=_tmp51A->f1;_tmp51C=_tmp51A->f2;}}_LL2A7: if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_add(_tmp51B);{const char*_tmpA4F;const char*_tmpA4E;
struct Cyc_PP_Doc*_tmpA4D[8];s=((_tmpA4D[7]=Cyc_Absynpp_rb(),((_tmpA4D[6]=Cyc_PP_line_doc(),((
_tmpA4D[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA4E="",_tag_dyneither(_tmpA4E,sizeof(char),1))),_tmp51C),((_tmpA4D[4]=Cyc_PP_line_doc(),((
_tmpA4D[3]=Cyc_Absynpp_lb(),((_tmpA4D[2]=Cyc_PP_blank_doc(),((_tmpA4D[1]=Cyc_PP_textptr(
_tmp51B),((_tmpA4D[0]=Cyc_PP_text(((_tmpA4F="namespace ",_tag_dyneither(_tmpA4F,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA4D,sizeof(struct Cyc_PP_Doc*),
8))))))))))))))))));}if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL291;_LL2A8: {struct Cyc_Absyn_Using_d_struct*_tmp51D=(struct Cyc_Absyn_Using_d_struct*)
_tmp4FF;if(_tmp51D->tag != 11)goto _LL2AA;else{_tmp51E=_tmp51D->f1;_tmp51F=_tmp51D->f2;}}
_LL2A9: if(Cyc_Absynpp_print_using_stmts){const char*_tmpA54;const char*_tmpA53;
struct Cyc_PP_Doc*_tmpA52[8];s=((_tmpA52[7]=Cyc_Absynpp_rb(),((_tmpA52[6]=Cyc_PP_line_doc(),((
_tmpA52[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA53="",_tag_dyneither(_tmpA53,sizeof(char),1))),_tmp51F),((_tmpA52[4]=Cyc_PP_line_doc(),((
_tmpA52[3]=Cyc_Absynpp_lb(),((_tmpA52[2]=Cyc_PP_blank_doc(),((_tmpA52[1]=Cyc_Absynpp_qvar2doc(
_tmp51E),((_tmpA52[0]=Cyc_PP_text(((_tmpA54="using ",_tag_dyneither(_tmpA54,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA52,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmpA5F;const char*_tmpA5E;const char*_tmpA5D;const char*_tmpA5C;
const char*_tmpA5B;struct Cyc_PP_Doc*_tmpA5A[11];s=((_tmpA5A[10]=Cyc_PP_text(((
_tmpA5B=" */",_tag_dyneither(_tmpA5B,sizeof(char),4)))),((_tmpA5A[9]=Cyc_Absynpp_rb(),((
_tmpA5A[8]=Cyc_PP_text(((_tmpA5C="/* ",_tag_dyneither(_tmpA5C,sizeof(char),4)))),((
_tmpA5A[7]=Cyc_PP_line_doc(),((_tmpA5A[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA5D="",_tag_dyneither(_tmpA5D,sizeof(char),1))),
_tmp51F),((_tmpA5A[5]=Cyc_PP_line_doc(),((_tmpA5A[4]=Cyc_PP_text(((_tmpA5E=" */",
_tag_dyneither(_tmpA5E,sizeof(char),4)))),((_tmpA5A[3]=Cyc_Absynpp_lb(),((
_tmpA5A[2]=Cyc_PP_blank_doc(),((_tmpA5A[1]=Cyc_Absynpp_qvar2doc(_tmp51E),((
_tmpA5A[0]=Cyc_PP_text(((_tmpA5F="/* using ",_tag_dyneither(_tmpA5F,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmpA5A,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL291;_LL2AA: {struct Cyc_Absyn_ExternC_d_struct*_tmp520=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp4FF;if(_tmp520->tag != 12)goto _LL2AC;else{_tmp521=_tmp520->f1;}}_LL2AB: if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA64;const char*_tmpA63;struct Cyc_PP_Doc*_tmpA62[6];s=((_tmpA62[5]=
Cyc_Absynpp_rb(),((_tmpA62[4]=Cyc_PP_line_doc(),((_tmpA62[3]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA63="",_tag_dyneither(_tmpA63,
sizeof(char),1))),_tmp521),((_tmpA62[2]=Cyc_PP_line_doc(),((_tmpA62[1]=Cyc_Absynpp_lb(),((
_tmpA62[0]=Cyc_PP_text(((_tmpA64="extern \"C\" ",_tag_dyneither(_tmpA64,sizeof(
char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA62,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmpA6F;const char*_tmpA6E;const char*_tmpA6D;const char*_tmpA6C;
const char*_tmpA6B;struct Cyc_PP_Doc*_tmpA6A[9];s=((_tmpA6A[8]=Cyc_PP_text(((
_tmpA6B=" */",_tag_dyneither(_tmpA6B,sizeof(char),4)))),((_tmpA6A[7]=Cyc_Absynpp_rb(),((
_tmpA6A[6]=Cyc_PP_text(((_tmpA6C="/* ",_tag_dyneither(_tmpA6C,sizeof(char),4)))),((
_tmpA6A[5]=Cyc_PP_line_doc(),((_tmpA6A[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA6D="",_tag_dyneither(_tmpA6D,sizeof(char),1))),
_tmp521),((_tmpA6A[3]=Cyc_PP_line_doc(),((_tmpA6A[2]=Cyc_PP_text(((_tmpA6E=" */",
_tag_dyneither(_tmpA6E,sizeof(char),4)))),((_tmpA6A[1]=Cyc_Absynpp_lb(),((
_tmpA6A[0]=Cyc_PP_text(((_tmpA6F="/* extern \"C\" ",_tag_dyneither(_tmpA6F,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA6A,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL291;_LL2AC: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp522=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp4FF;if(_tmp522->tag != 13)
goto _LL2AE;else{_tmp523=_tmp522->f1;_tmp524=_tmp522->f2;}}_LL2AD: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp524 != 0){const char*_tmpA74;const char*_tmpA73;
struct Cyc_PP_Doc*_tmpA72[7];exs_doc=((_tmpA72[6]=Cyc_Absynpp_rb(),((_tmpA72[5]=
Cyc_PP_line_doc(),((_tmpA72[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple18*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmpA73=",",_tag_dyneither(_tmpA73,sizeof(char),2))),
_tmp524),((_tmpA72[3]=Cyc_PP_line_doc(),((_tmpA72[2]=Cyc_Absynpp_lb(),((_tmpA72[
1]=Cyc_PP_text(((_tmpA74=" export ",_tag_dyneither(_tmpA74,sizeof(char),9)))),((
_tmpA72[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA72,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmpA79;const char*
_tmpA78;struct Cyc_PP_Doc*_tmpA77[6];s=((_tmpA77[5]=exs_doc,((_tmpA77[4]=Cyc_PP_line_doc(),((
_tmpA77[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA78="",_tag_dyneither(_tmpA78,sizeof(char),1))),_tmp523),((_tmpA77[2]=Cyc_PP_line_doc(),((
_tmpA77[1]=Cyc_Absynpp_lb(),((_tmpA77[0]=Cyc_PP_text(((_tmpA79="extern \"C include\" ",
_tag_dyneither(_tmpA79,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA77,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{const char*_tmpA84;const char*
_tmpA83;const char*_tmpA82;const char*_tmpA81;const char*_tmpA80;struct Cyc_PP_Doc*
_tmpA7F[9];s=((_tmpA7F[8]=Cyc_PP_text(((_tmpA80=" */",_tag_dyneither(_tmpA80,
sizeof(char),4)))),((_tmpA7F[7]=Cyc_Absynpp_rb(),((_tmpA7F[6]=Cyc_PP_text(((
_tmpA81="/* ",_tag_dyneither(_tmpA81,sizeof(char),4)))),((_tmpA7F[5]=Cyc_PP_line_doc(),((
_tmpA7F[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA82="",_tag_dyneither(_tmpA82,sizeof(char),1))),_tmp523),((_tmpA7F[3]=Cyc_PP_line_doc(),((
_tmpA7F[2]=Cyc_PP_text(((_tmpA83=" */",_tag_dyneither(_tmpA83,sizeof(char),4)))),((
_tmpA7F[1]=Cyc_Absynpp_lb(),((_tmpA7F[0]=Cyc_PP_text(((_tmpA84="/* extern \"C include\" ",
_tag_dyneither(_tmpA84,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA7F,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL291;_LL2AE: {struct Cyc_Absyn_Porton_d_struct*
_tmp525=(struct Cyc_Absyn_Porton_d_struct*)_tmp4FF;if(_tmp525->tag != 14)goto
_LL2B0;}_LL2AF:{const char*_tmpA87;struct Cyc_PP_Doc*_tmpA86[2];s=((_tmpA86[1]=Cyc_Absynpp_lb(),((
_tmpA86[0]=Cyc_PP_text(((_tmpA87="__cyclone_port_on__;",_tag_dyneither(_tmpA87,
sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA86,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL291;_LL2B0: {struct Cyc_Absyn_Portoff_d_struct*_tmp526=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp4FF;if(_tmp526->tag != 15)goto _LL291;}_LL2B1:{const char*_tmpA8A;struct Cyc_PP_Doc*
_tmpA89[2];s=((_tmpA89[1]=Cyc_Absynpp_lb(),((_tmpA89[0]=Cyc_PP_text(((_tmpA8A="__cyclone_port_off__;",
_tag_dyneither(_tmpA8A,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA89,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL291;_LL291:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){if(Cyc_Absynpp_print_for_cycdoc)
return Cyc_PP_nil_doc();switch(sc){case Cyc_Absyn_Static: _LL2BB: {const char*
_tmpA8B;return Cyc_PP_text(((_tmpA8B="static ",_tag_dyneither(_tmpA8B,sizeof(char),
8))));}case Cyc_Absyn_Public: _LL2BC: return Cyc_PP_nil_doc();case Cyc_Absyn_Extern:
_LL2BD: {const char*_tmpA8C;return Cyc_PP_text(((_tmpA8C="extern ",_tag_dyneither(
_tmpA8C,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2BE: {const char*_tmpA8D;
return Cyc_PP_text(((_tmpA8D="extern \"C\" ",_tag_dyneither(_tmpA8D,sizeof(char),
12))));}case Cyc_Absyn_Abstract: _LL2BF: {const char*_tmpA8E;return Cyc_PP_text(((
_tmpA8E="abstract ",_tag_dyneither(_tmpA8E,sizeof(char),10))));}case Cyc_Absyn_Register:
_LL2C0: {const char*_tmpA8F;return Cyc_PP_text(((_tmpA8F="register ",
_tag_dyneither(_tmpA8F,sizeof(char),10))));}}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp57D=t;struct Cyc_Absyn_Tvar*_tmp57F;struct Cyc_List_List*_tmp581;
_LL2C3: {struct Cyc_Absyn_VarType_struct*_tmp57E=(struct Cyc_Absyn_VarType_struct*)
_tmp57D;if(_tmp57E->tag != 2)goto _LL2C5;else{_tmp57F=_tmp57E->f1;}}_LL2C4: return
Cyc_Tcutil_is_temp_tvar(_tmp57F);_LL2C5: {struct Cyc_Absyn_JoinEff_struct*_tmp580=(
struct Cyc_Absyn_JoinEff_struct*)_tmp57D;if(_tmp580->tag != 24)goto _LL2C7;else{
_tmp581=_tmp580->f1;}}_LL2C6: return((int(*)(int(*pred)(void*),struct Cyc_List_List*
x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp581);_LL2C7:;
_LL2C8: return 0;_LL2C2:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp582=t;
struct Cyc_Absyn_Typedefdecl*_tmp586;void**_tmp587;void*_tmp588;_LL2CA: {struct
Cyc_Absyn_AnonAggrType_struct*_tmp583=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp582;if(_tmp583->tag != 12)goto _LL2CC;}_LL2CB: return 1;_LL2CC: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp584=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp582;if(_tmp584->tag != 14)goto
_LL2CE;}_LL2CD: return 1;_LL2CE: {struct Cyc_Absyn_TypedefType_struct*_tmp585=(
struct Cyc_Absyn_TypedefType_struct*)_tmp582;if(_tmp585->tag != 17)goto _LL2D0;
else{_tmp586=_tmp585->f3;_tmp587=_tmp585->f4;if(_tmp587 == 0)goto _LL2D0;_tmp588=*
_tmp587;}}_LL2CF: return Cyc_Absynpp_is_anon_aggrtype(_tmp588);_LL2D0:;_LL2D1:
return 0;_LL2C9:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct
_RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 0){
struct _tuple17 _tmpA90;struct _tuple17 _tmp58A=(_tmpA90.f1=(void*)tms->hd,((_tmpA90.f2=(
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA90)));void*_tmp58B;
void*_tmp58D;_LL2D3: _tmp58B=_tmp58A.f1;{struct Cyc_Absyn_Pointer_mod_struct*
_tmp58C=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp58B;if(_tmp58C->tag != 2)goto
_LL2D5;};_tmp58D=_tmp58A.f2;{struct Cyc_Absyn_Function_mod_struct*_tmp58E=(struct
Cyc_Absyn_Function_mod_struct*)_tmp58D;if(_tmp58E->tag != 3)goto _LL2D5;};_LL2D4: {
struct Cyc_List_List*_tmpA93;struct Cyc_List_List*_tmpA92;return(_tmpA92=
_region_malloc(r,sizeof(*_tmpA92)),((_tmpA92->hd=(void*)tms->hd,((_tmpA92->tl=((
_tmpA93=_region_malloc(r,sizeof(*_tmpA93)),((_tmpA93->hd=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd,((_tmpA93->tl=Cyc_Absynpp_bubble_attributes(r,atts,((
struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA93)))))),_tmpA92)))));}
_LL2D5:;_LL2D6: {struct Cyc_List_List*_tmpA94;return(_tmpA94=_region_malloc(r,
sizeof(*_tmpA94)),((_tmpA94->hd=atts,((_tmpA94->tl=tms,_tmpA94)))));}_LL2D2:;}
else{struct Cyc_List_List*_tmpA95;return(_tmpA95=_region_malloc(r,sizeof(*_tmpA95)),((
_tmpA95->hd=atts,((_tmpA95->tl=tms,_tmpA95)))));}}struct _tuple13 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){void*_tmp593=t;struct Cyc_Absyn_ArrayInfo
_tmp595;void*_tmp596;struct Cyc_Absyn_Tqual _tmp597;struct Cyc_Absyn_Exp*_tmp598;
union Cyc_Absyn_Constraint*_tmp599;struct Cyc_Position_Segment*_tmp59A;struct Cyc_Absyn_PtrInfo
_tmp59C;void*_tmp59D;struct Cyc_Absyn_Tqual _tmp59E;struct Cyc_Absyn_PtrAtts _tmp59F;
struct Cyc_Absyn_FnInfo _tmp5A1;struct Cyc_List_List*_tmp5A2;struct Cyc_Core_Opt*
_tmp5A3;struct Cyc_Absyn_Tqual _tmp5A4;void*_tmp5A5;struct Cyc_List_List*_tmp5A6;
int _tmp5A7;struct Cyc_Absyn_VarargInfo*_tmp5A8;struct Cyc_List_List*_tmp5A9;struct
Cyc_List_List*_tmp5AA;struct Cyc_Core_Opt*_tmp5AC;struct Cyc_Core_Opt*_tmp5AD;int
_tmp5AE;struct _tuple0*_tmp5B0;struct Cyc_List_List*_tmp5B1;struct Cyc_Absyn_Typedefdecl*
_tmp5B2;void**_tmp5B3;_LL2D8: {struct Cyc_Absyn_ArrayType_struct*_tmp594=(struct
Cyc_Absyn_ArrayType_struct*)_tmp593;if(_tmp594->tag != 8)goto _LL2DA;else{_tmp595=
_tmp594->f1;_tmp596=_tmp595.elt_type;_tmp597=_tmp595.tq;_tmp598=_tmp595.num_elts;
_tmp599=_tmp595.zero_term;_tmp59A=_tmp595.zt_loc;}}_LL2D9: {struct Cyc_Absyn_Tqual
_tmp5B5;void*_tmp5B6;struct Cyc_List_List*_tmp5B7;struct _tuple13 _tmp5B4=Cyc_Absynpp_to_tms(
r,_tmp597,_tmp596);_tmp5B5=_tmp5B4.f1;_tmp5B6=_tmp5B4.f2;_tmp5B7=_tmp5B4.f3;{
void*tm;if(_tmp598 == 0){struct Cyc_Absyn_Carray_mod_struct _tmpA98;struct Cyc_Absyn_Carray_mod_struct*
_tmpA97;tm=(void*)((_tmpA97=_region_malloc(r,sizeof(*_tmpA97)),((_tmpA97[0]=((
_tmpA98.tag=0,((_tmpA98.f1=_tmp599,((_tmpA98.f2=_tmp59A,_tmpA98)))))),_tmpA97))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmpA9B;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpA9A;tm=(void*)((_tmpA9A=_region_malloc(r,sizeof(*_tmpA9A)),((_tmpA9A[0]=((
_tmpA9B.tag=1,((_tmpA9B.f1=(struct Cyc_Absyn_Exp*)_tmp598,((_tmpA9B.f2=_tmp599,((
_tmpA9B.f3=_tmp59A,_tmpA9B)))))))),_tmpA9A))));}{struct Cyc_List_List*_tmpA9E;
struct _tuple13 _tmpA9D;return(_tmpA9D.f1=_tmp5B5,((_tmpA9D.f2=_tmp5B6,((_tmpA9D.f3=((
_tmpA9E=_region_malloc(r,sizeof(*_tmpA9E)),((_tmpA9E->hd=tm,((_tmpA9E->tl=
_tmp5B7,_tmpA9E)))))),_tmpA9D)))));};};}_LL2DA: {struct Cyc_Absyn_PointerType_struct*
_tmp59B=(struct Cyc_Absyn_PointerType_struct*)_tmp593;if(_tmp59B->tag != 5)goto
_LL2DC;else{_tmp59C=_tmp59B->f1;_tmp59D=_tmp59C.elt_typ;_tmp59E=_tmp59C.elt_tq;
_tmp59F=_tmp59C.ptr_atts;}}_LL2DB: {struct Cyc_Absyn_Tqual _tmp5BF;void*_tmp5C0;
struct Cyc_List_List*_tmp5C1;struct _tuple13 _tmp5BE=Cyc_Absynpp_to_tms(r,_tmp59E,
_tmp59D);_tmp5BF=_tmp5BE.f1;_tmp5C0=_tmp5BE.f2;_tmp5C1=_tmp5BE.f3;{struct Cyc_Absyn_Pointer_mod_struct*
_tmpAA4;struct Cyc_Absyn_Pointer_mod_struct _tmpAA3;struct Cyc_List_List*_tmpAA2;
_tmp5C1=((_tmpAA2=_region_malloc(r,sizeof(*_tmpAA2)),((_tmpAA2->hd=(void*)((
_tmpAA4=_region_malloc(r,sizeof(*_tmpAA4)),((_tmpAA4[0]=((_tmpAA3.tag=2,((
_tmpAA3.f1=_tmp59F,((_tmpAA3.f2=tq,_tmpAA3)))))),_tmpAA4)))),((_tmpAA2->tl=
_tmp5C1,_tmpAA2))))));}{struct _tuple13 _tmpAA5;return(_tmpAA5.f1=_tmp5BF,((
_tmpAA5.f2=_tmp5C0,((_tmpAA5.f3=_tmp5C1,_tmpAA5)))));};}_LL2DC: {struct Cyc_Absyn_FnType_struct*
_tmp5A0=(struct Cyc_Absyn_FnType_struct*)_tmp593;if(_tmp5A0->tag != 9)goto _LL2DE;
else{_tmp5A1=_tmp5A0->f1;_tmp5A2=_tmp5A1.tvars;_tmp5A3=_tmp5A1.effect;_tmp5A4=
_tmp5A1.ret_tqual;_tmp5A5=_tmp5A1.ret_typ;_tmp5A6=_tmp5A1.args;_tmp5A7=_tmp5A1.c_varargs;
_tmp5A8=_tmp5A1.cyc_varargs;_tmp5A9=_tmp5A1.rgn_po;_tmp5AA=_tmp5A1.attributes;}}
_LL2DD: if(!Cyc_Absynpp_print_all_tvars){if(_tmp5A3 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)_tmp5A3->v)){_tmp5A3=0;_tmp5A2=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar,_tmp5A2);}{struct Cyc_Absyn_Tqual _tmp5C7;void*
_tmp5C8;struct Cyc_List_List*_tmp5C9;struct _tuple13 _tmp5C6=Cyc_Absynpp_to_tms(r,
_tmp5A4,_tmp5A5);_tmp5C7=_tmp5C6.f1;_tmp5C8=_tmp5C6.f2;_tmp5C9=_tmp5C6.f3;{
struct Cyc_List_List*tms=_tmp5C9;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL2E4: if(_tmp5AA != 0){struct Cyc_Absyn_Attributes_mod_struct _tmpAA8;struct Cyc_Absyn_Attributes_mod_struct*
_tmpAA7;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpAA7=_region_malloc(r,
sizeof(*_tmpAA7)),((_tmpAA7[0]=((_tmpAA8.tag=5,((_tmpAA8.f1=0,((_tmpAA8.f2=
_tmp5AA,_tmpAA8)))))),_tmpAA7)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmpAB7;struct Cyc_Absyn_WithTypes_struct*_tmpAB6;struct Cyc_Absyn_WithTypes_struct
_tmpAB5;struct Cyc_Absyn_Function_mod_struct _tmpAB4;struct Cyc_List_List*_tmpAB3;
tms=((_tmpAB3=_region_malloc(r,sizeof(*_tmpAB3)),((_tmpAB3->hd=(void*)((_tmpAB7=
_region_malloc(r,sizeof(*_tmpAB7)),((_tmpAB7[0]=((_tmpAB4.tag=3,((_tmpAB4.f1=(
void*)((void*)((_tmpAB6=_region_malloc(r,sizeof(*_tmpAB6)),((_tmpAB6[0]=((
_tmpAB5.tag=1,((_tmpAB5.f1=_tmp5A6,((_tmpAB5.f2=_tmp5A7,((_tmpAB5.f3=_tmp5A8,((
_tmpAB5.f4=_tmp5A3,((_tmpAB5.f5=_tmp5A9,_tmpAB5)))))))))))),_tmpAB6))))),_tmpAB4)))),
_tmpAB7)))),((_tmpAB3->tl=tms,_tmpAB3))))));}break;case Cyc_Cyclone_Vc_c: _LL2E5:{
struct Cyc_Absyn_Function_mod_struct*_tmpAC6;struct Cyc_Absyn_WithTypes_struct*
_tmpAC5;struct Cyc_Absyn_WithTypes_struct _tmpAC4;struct Cyc_Absyn_Function_mod_struct
_tmpAC3;struct Cyc_List_List*_tmpAC2;tms=((_tmpAC2=_region_malloc(r,sizeof(*
_tmpAC2)),((_tmpAC2->hd=(void*)((_tmpAC6=_region_malloc(r,sizeof(*_tmpAC6)),((
_tmpAC6[0]=((_tmpAC3.tag=3,((_tmpAC3.f1=(void*)((void*)((_tmpAC5=_region_malloc(
r,sizeof(*_tmpAC5)),((_tmpAC5[0]=((_tmpAC4.tag=1,((_tmpAC4.f1=_tmp5A6,((_tmpAC4.f2=
_tmp5A7,((_tmpAC4.f3=_tmp5A8,((_tmpAC4.f4=_tmp5A3,((_tmpAC4.f5=_tmp5A9,_tmpAC4)))))))))))),
_tmpAC5))))),_tmpAC3)))),_tmpAC6)))),((_tmpAC2->tl=tms,_tmpAC2))))));}for(0;
_tmp5AA != 0;_tmp5AA=_tmp5AA->tl){void*_tmp5D6=(void*)_tmp5AA->hd;_LL2E8: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp5D7=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp5D6;if(_tmp5D7->tag != 1)goto _LL2EA;}_LL2E9: goto _LL2EB;_LL2EA: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp5D8=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp5D6;if(_tmp5D8->tag != 2)goto
_LL2EC;}_LL2EB: goto _LL2ED;_LL2EC: {struct Cyc_Absyn_Fastcall_att_struct*_tmp5D9=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp5D6;if(_tmp5D9->tag != 3)goto _LL2EE;}
_LL2ED:{struct Cyc_Absyn_Attributes_mod_struct*_tmpAD0;struct Cyc_List_List*
_tmpACF;struct Cyc_Absyn_Attributes_mod_struct _tmpACE;struct Cyc_List_List*_tmpACD;
tms=((_tmpACD=_region_malloc(r,sizeof(*_tmpACD)),((_tmpACD->hd=(void*)((_tmpAD0=
_region_malloc(r,sizeof(*_tmpAD0)),((_tmpAD0[0]=((_tmpACE.tag=5,((_tmpACE.f1=0,((
_tmpACE.f2=((_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF->hd=(void*)_tmp5AA->hd,((
_tmpACF->tl=0,_tmpACF)))))),_tmpACE)))))),_tmpAD0)))),((_tmpACD->tl=tms,_tmpACD))))));}
goto AfterAtts;_LL2EE:;_LL2EF: goto _LL2E7;_LL2E7:;}break;}AfterAtts: if(_tmp5A2 != 0){
struct Cyc_Absyn_TypeParams_mod_struct*_tmpAD6;struct Cyc_Absyn_TypeParams_mod_struct
_tmpAD5;struct Cyc_List_List*_tmpAD4;tms=((_tmpAD4=_region_malloc(r,sizeof(*
_tmpAD4)),((_tmpAD4->hd=(void*)((_tmpAD6=_region_malloc(r,sizeof(*_tmpAD6)),((
_tmpAD6[0]=((_tmpAD5.tag=4,((_tmpAD5.f1=_tmp5A2,((_tmpAD5.f2=0,((_tmpAD5.f3=1,
_tmpAD5)))))))),_tmpAD6)))),((_tmpAD4->tl=tms,_tmpAD4))))));}{struct _tuple13
_tmpAD7;return(_tmpAD7.f1=_tmp5C7,((_tmpAD7.f2=_tmp5C8,((_tmpAD7.f3=tms,_tmpAD7)))));};};};
_LL2DE: {struct Cyc_Absyn_Evar_struct*_tmp5AB=(struct Cyc_Absyn_Evar_struct*)
_tmp593;if(_tmp5AB->tag != 1)goto _LL2E0;else{_tmp5AC=_tmp5AB->f1;_tmp5AD=_tmp5AB->f2;
_tmp5AE=_tmp5AB->f3;}}_LL2DF: if(_tmp5AD == 0){struct _tuple13 _tmpAD8;return(
_tmpAD8.f1=tq,((_tmpAD8.f2=t,((_tmpAD8.f3=0,_tmpAD8)))));}else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp5AD->v);}_LL2E0: {struct Cyc_Absyn_TypedefType_struct*_tmp5AF=(
struct Cyc_Absyn_TypedefType_struct*)_tmp593;if(_tmp5AF->tag != 17)goto _LL2E2;
else{_tmp5B0=_tmp5AF->f1;_tmp5B1=_tmp5AF->f2;_tmp5B2=_tmp5AF->f3;_tmp5B3=_tmp5AF->f4;}}
_LL2E1: if((_tmp5B3 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp5B3)){struct _tuple13 _tmpAD9;return(_tmpAD9.f1=tq,((_tmpAD9.f2=t,((_tmpAD9.f3=
0,_tmpAD9)))));}else{if(tq.real_const)tq.print_const=tq.real_const;return Cyc_Absynpp_to_tms(
r,tq,*_tmp5B3);}_LL2E2:;_LL2E3: {struct _tuple13 _tmpADA;return(_tmpADA.f1=tq,((
_tmpADA.f2=t,((_tmpADA.f3=0,_tmpADA)))));}_LL2D7:;}static int Cyc_Absynpp_is_char_ptr(
void*t){void*_tmp5E5=t;struct Cyc_Core_Opt*_tmp5E7;struct Cyc_Core_Opt _tmp5E8;void*
_tmp5E9;struct Cyc_Absyn_PtrInfo _tmp5EB;void*_tmp5EC;_LL2F1: {struct Cyc_Absyn_Evar_struct*
_tmp5E6=(struct Cyc_Absyn_Evar_struct*)_tmp5E5;if(_tmp5E6->tag != 1)goto _LL2F3;
else{_tmp5E7=_tmp5E6->f2;if(_tmp5E7 == 0)goto _LL2F3;_tmp5E8=*_tmp5E7;_tmp5E9=(
void*)_tmp5E8.v;}}_LL2F2: return Cyc_Absynpp_is_char_ptr(_tmp5E9);_LL2F3: {struct
Cyc_Absyn_PointerType_struct*_tmp5EA=(struct Cyc_Absyn_PointerType_struct*)
_tmp5E5;if(_tmp5EA->tag != 5)goto _LL2F5;else{_tmp5EB=_tmp5EA->f1;_tmp5EC=_tmp5EB.elt_typ;}}
_LL2F4: L: {void*_tmp5ED=_tmp5EC;struct Cyc_Core_Opt*_tmp5EF;struct Cyc_Core_Opt
_tmp5F0;void*_tmp5F1;void**_tmp5F3;void*_tmp5F4;enum Cyc_Absyn_Size_of _tmp5F6;
_LL2F8: {struct Cyc_Absyn_Evar_struct*_tmp5EE=(struct Cyc_Absyn_Evar_struct*)
_tmp5ED;if(_tmp5EE->tag != 1)goto _LL2FA;else{_tmp5EF=_tmp5EE->f2;if(_tmp5EF == 0)
goto _LL2FA;_tmp5F0=*_tmp5EF;_tmp5F1=(void*)_tmp5F0.v;}}_LL2F9: _tmp5EC=_tmp5F1;
goto L;_LL2FA: {struct Cyc_Absyn_TypedefType_struct*_tmp5F2=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5ED;if(_tmp5F2->tag != 17)goto _LL2FC;else{_tmp5F3=_tmp5F2->f4;if(_tmp5F3 == 0)
goto _LL2FC;_tmp5F4=*_tmp5F3;}}_LL2FB: _tmp5EC=_tmp5F4;goto L;_LL2FC: {struct Cyc_Absyn_IntType_struct*
_tmp5F5=(struct Cyc_Absyn_IntType_struct*)_tmp5ED;if(_tmp5F5->tag != 6)goto _LL2FE;
else{_tmp5F6=_tmp5F5->f2;if(_tmp5F6 != Cyc_Absyn_Char_sz)goto _LL2FE;}}_LL2FD:
return 1;_LL2FE:;_LL2FF: return 0;_LL2F7:;}_LL2F5:;_LL2F6: return 0;_LL2F0:;}struct
Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*
dopt){struct _RegionHandle _tmp5F7=_new_region("temp");struct _RegionHandle*temp=&
_tmp5F7;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp5F9;void*_tmp5FA;struct Cyc_List_List*
_tmp5FB;struct _tuple13 _tmp5F8=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp5F9=_tmp5F8.f1;
_tmp5FA=_tmp5F8.f2;_tmp5FB=_tmp5F8.f3;_tmp5FB=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5FB);if(_tmp5FB == 0  && dopt == 0){struct
Cyc_PP_Doc*_tmpADB[2];struct Cyc_PP_Doc*_tmp5FD=(_tmpADB[1]=Cyc_Absynpp_ntyp2doc(
_tmp5FA),((_tmpADB[0]=Cyc_Absynpp_tqual2doc(_tmp5F9),Cyc_PP_cat(_tag_dyneither(
_tmpADB,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5FD;}else{
const char*_tmpADE;struct Cyc_PP_Doc*_tmpADD[4];struct Cyc_PP_Doc*_tmp600=(_tmpADD[
3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*)dopt->v,_tmp5FB),((_tmpADD[2]=Cyc_PP_text(((_tmpADE=" ",
_tag_dyneither(_tmpADE,sizeof(char),2)))),((_tmpADD[1]=Cyc_Absynpp_ntyp2doc(
_tmp5FA),((_tmpADD[0]=Cyc_Absynpp_tqual2doc(_tmp5F9),Cyc_PP_cat(_tag_dyneither(
_tmpADD,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp600;}};
_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*
f){switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL300: if(f->width != 0){
struct Cyc_Core_Opt*_tmpAE5;const char*_tmpAE4;const char*_tmpAE3;struct Cyc_PP_Doc*
_tmpAE2[5];return(_tmpAE2[4]=Cyc_PP_text(((_tmpAE3=";",_tag_dyneither(_tmpAE3,
sizeof(char),2)))),((_tmpAE2[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpAE2[2]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAE2[1]=
Cyc_PP_text(((_tmpAE4=":",_tag_dyneither(_tmpAE4,sizeof(char),2)))),((_tmpAE2[0]=
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((
_tmpAE5->v=Cyc_PP_textptr(f->name),_tmpAE5))))),Cyc_PP_cat(_tag_dyneither(
_tmpAE2,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*_tmpAEA;
const char*_tmpAE9;struct Cyc_PP_Doc*_tmpAE8[3];return(_tmpAE8[2]=Cyc_PP_text(((
_tmpAE9=";",_tag_dyneither(_tmpAE9,sizeof(char),2)))),((_tmpAE8[1]=Cyc_Absynpp_atts2doc(
f->attributes),((_tmpAE8[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAEA=
_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA->v=Cyc_PP_textptr(f->name),_tmpAEA))))),
Cyc_PP_cat(_tag_dyneither(_tmpAE8,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c:
_LL301: if(f->width != 0){struct Cyc_Core_Opt*_tmpAF1;const char*_tmpAF0;const char*
_tmpAEF;struct Cyc_PP_Doc*_tmpAEE[5];return(_tmpAEE[4]=Cyc_PP_text(((_tmpAEF=";",
_tag_dyneither(_tmpAEF,sizeof(char),2)))),((_tmpAEE[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAEE[2]=Cyc_PP_text(((_tmpAF0=":",
_tag_dyneither(_tmpAF0,sizeof(char),2)))),((_tmpAEE[1]=Cyc_Absynpp_tqtd2doc(f->tq,
f->type,((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1->v=Cyc_PP_textptr(f->name),
_tmpAF1))))),((_tmpAEE[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAEE,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpAF6;const char*_tmpAF5;struct Cyc_PP_Doc*_tmpAF4[3];return(_tmpAF4[2]=Cyc_PP_text(((
_tmpAF5=";",_tag_dyneither(_tmpAF5,sizeof(char),2)))),((_tmpAF4[1]=Cyc_Absynpp_tqtd2doc(
f->tq,f->type,((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6->v=Cyc_PP_textptr(
f->name),_tmpAF6))))),((_tmpAF4[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAF4,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){const char*_tmpAF7;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((
_tmpAF7="",_tag_dyneither(_tmpAF7,sizeof(char),1))),fields);}struct Cyc_PP_Doc*
Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*
_tmpAF8[3];return(_tmpAF8[2]=f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((
_tmpAF8[1]=Cyc_Absynpp_typedef_name2doc(f->name),((_tmpAF8[0]=Cyc_Absynpp_scope2doc(
f->sc),Cyc_PP_cat(_tag_dyneither(_tmpAF8,sizeof(struct Cyc_PP_Doc*),3)))))));}
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpAF9;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((
_tmpAF9=",",_tag_dyneither(_tmpAF9,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmpAFC;
void*_tmpAFB;(_tmpAFB=0,Cyc_fprintf(f,((_tmpAFC="\n",_tag_dyneither(_tmpAFC,
sizeof(char),2))),_tag_dyneither(_tmpAFB,sizeof(void*),0)));};}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmpAFD;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAFD="",_tag_dyneither(
_tmpAFD,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),
72);}struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(
Cyc_Absynpp_exp2doc(e),72);}struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct
Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(
Cyc_Absynpp_typ2doc(t),72);}struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dyneither_ptr s=
Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;};}struct _dyneither_ptr Cyc_Absynpp_prim2string(enum 
Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(
Cyc_Absynpp_pat2doc(p),72);}struct _dyneither_ptr Cyc_Absynpp_scope2string(enum 
Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
