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
Cyc_Absyn_attribute2string(void*);struct _tuple9{enum Cyc_Absyn_AggrKind f1;struct
_tuple0*f2;};struct _tuple9 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
struct Cyc_Buffer_t;unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dyneither_ptr);struct _tuple10{unsigned int f1;int f2;};
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
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
p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple11{struct
_dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple11*arg);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual
tq,void*t);enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c
 = 1};extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{
struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(
struct _tuple13*dp);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*
cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt);struct
Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int
inprec,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct
_tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
enum Cyc_Absyn_Primop);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum 
Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple14{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);
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
_dyneither_ptr*v){struct Cyc_List_List*_tmp60A;Cyc_Absynpp_curr_namespace=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_Absynpp_curr_namespace,((_tmp60A=_cycalloc(sizeof(*_tmp60A)),((_tmp60A->hd=v,((
_tmp60A->tl=0,_tmp60A)))))));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**
l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((
struct Cyc_List_List*)_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0: {
const char*_tmp60B;return(_tmp60B="\\a",_tag_dyneither(_tmp60B,sizeof(char),3));}
case '\b': _LL1: {const char*_tmp60C;return(_tmp60C="\\b",_tag_dyneither(_tmp60C,
sizeof(char),3));}case '\f': _LL2: {const char*_tmp60D;return(_tmp60D="\\f",
_tag_dyneither(_tmp60D,sizeof(char),3));}case '\n': _LL3: {const char*_tmp60E;
return(_tmp60E="\\n",_tag_dyneither(_tmp60E,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp60F;return(_tmp60F="\\r",_tag_dyneither(_tmp60F,sizeof(char),3));}
case '\t': _LL5: {const char*_tmp610;return(_tmp610="\\t",_tag_dyneither(_tmp610,
sizeof(char),3));}case '\v': _LL6: {const char*_tmp611;return(_tmp611="\\v",
_tag_dyneither(_tmp611,sizeof(char),3));}case '\\': _LL7: {const char*_tmp612;
return(_tmp612="\\\\",_tag_dyneither(_tmp612,sizeof(char),3));}case '"': _LL8: {
const char*_tmp613;return(_tmp613="\"",_tag_dyneither(_tmp613,sizeof(char),2));}
case '\'': _LL9: {const char*_tmp614;return(_tmp614="\\'",_tag_dyneither(_tmp614,
sizeof(char),3));}default: _LLA: if(c >= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=
Cyc_Core_new_string(2);{char _tmp617;char _tmp616;struct _dyneither_ptr _tmp615;(
_tmp615=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp616=*((char*)
_check_dyneither_subscript(_tmp615,sizeof(char),0)),((_tmp617=c,((
_get_dyneither_size(_tmp615,sizeof(char))== 1  && (_tmp616 == '\000'  && _tmp617 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp615.curr)=_tmp617)))))));}return(struct
_dyneither_ptr)_tmpC;}else{struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);int
j=0;{char _tmp61A;char _tmp619;struct _dyneither_ptr _tmp618;(_tmp618=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp619=*((char*)
_check_dyneither_subscript(_tmp618,sizeof(char),0)),((_tmp61A='\\',((
_get_dyneither_size(_tmp618,sizeof(char))== 1  && (_tmp619 == '\000'  && _tmp61A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp618.curr)=_tmp61A)))))));}{char _tmp61D;char
_tmp61C;struct _dyneither_ptr _tmp61B;(_tmp61B=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp61C=*((char*)_check_dyneither_subscript(_tmp61B,sizeof(char),0)),((
_tmp61D=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp61B,
sizeof(char))== 1  && (_tmp61C == '\000'  && _tmp61D != '\000')?_throw_arraybounds():
1,*((char*)_tmp61B.curr)=_tmp61D)))))));}{char _tmp620;char _tmp61F;struct
_dyneither_ptr _tmp61E;(_tmp61E=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp61F=*((char*)_check_dyneither_subscript(_tmp61E,sizeof(char),0)),((_tmp620=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp61E,sizeof(char))== 1  && (
_tmp61F == '\000'  && _tmp620 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61E.curr)=
_tmp620)))))));}{char _tmp623;char _tmp622;struct _dyneither_ptr _tmp621;(_tmp621=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp622=*((char*)
_check_dyneither_subscript(_tmp621,sizeof(char),0)),((_tmp623=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp621,sizeof(char))== 1  && (_tmp622 == '\000'  && _tmp623 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp621.curr)=_tmp623)))))));}return(struct
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
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp626;char _tmp625;struct
_dyneither_ptr _tmp624;(_tmp624=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp625=*((
char*)_check_dyneither_subscript(_tmp624,sizeof(char),0)),((_tmp626='\\',((
_get_dyneither_size(_tmp624,sizeof(char))== 1  && (_tmp625 == '\000'  && _tmp626 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp624.curr)=_tmp626)))))));}{char _tmp629;char
_tmp628;struct _dyneither_ptr _tmp627;(_tmp627=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp628=*((char*)_check_dyneither_subscript(_tmp627,sizeof(char),0)),((
_tmp629='a',((_get_dyneither_size(_tmp627,sizeof(char))== 1  && (_tmp628 == '\000'
 && _tmp629 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp627.curr)=_tmp629)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp62C;char _tmp62B;struct
_dyneither_ptr _tmp62A;(_tmp62A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62B=*((
char*)_check_dyneither_subscript(_tmp62A,sizeof(char),0)),((_tmp62C='\\',((
_get_dyneither_size(_tmp62A,sizeof(char))== 1  && (_tmp62B == '\000'  && _tmp62C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp62A.curr)=_tmp62C)))))));}{char _tmp62F;char
_tmp62E;struct _dyneither_ptr _tmp62D;(_tmp62D=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp62E=*((char*)_check_dyneither_subscript(_tmp62D,sizeof(char),0)),((
_tmp62F='b',((_get_dyneither_size(_tmp62D,sizeof(char))== 1  && (_tmp62E == '\000'
 && _tmp62F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62D.curr)=_tmp62F)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp632;char _tmp631;struct
_dyneither_ptr _tmp630;(_tmp630=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp631=*((
char*)_check_dyneither_subscript(_tmp630,sizeof(char),0)),((_tmp632='\\',((
_get_dyneither_size(_tmp630,sizeof(char))== 1  && (_tmp631 == '\000'  && _tmp632 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp630.curr)=_tmp632)))))));}{char _tmp635;char
_tmp634;struct _dyneither_ptr _tmp633;(_tmp633=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp634=*((char*)_check_dyneither_subscript(_tmp633,sizeof(char),0)),((
_tmp635='f',((_get_dyneither_size(_tmp633,sizeof(char))== 1  && (_tmp634 == '\000'
 && _tmp635 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp633.curr)=_tmp635)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp638;char _tmp637;struct
_dyneither_ptr _tmp636;(_tmp636=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp637=*((
char*)_check_dyneither_subscript(_tmp636,sizeof(char),0)),((_tmp638='\\',((
_get_dyneither_size(_tmp636,sizeof(char))== 1  && (_tmp637 == '\000'  && _tmp638 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp636.curr)=_tmp638)))))));}{char _tmp63B;char
_tmp63A;struct _dyneither_ptr _tmp639;(_tmp639=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp63A=*((char*)_check_dyneither_subscript(_tmp639,sizeof(char),0)),((
_tmp63B='n',((_get_dyneither_size(_tmp639,sizeof(char))== 1  && (_tmp63A == '\000'
 && _tmp63B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp639.curr)=_tmp63B)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp63E;char _tmp63D;struct
_dyneither_ptr _tmp63C;(_tmp63C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63D=*((
char*)_check_dyneither_subscript(_tmp63C,sizeof(char),0)),((_tmp63E='\\',((
_get_dyneither_size(_tmp63C,sizeof(char))== 1  && (_tmp63D == '\000'  && _tmp63E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp63C.curr)=_tmp63E)))))));}{char _tmp641;char
_tmp640;struct _dyneither_ptr _tmp63F;(_tmp63F=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp640=*((char*)_check_dyneither_subscript(_tmp63F,sizeof(char),0)),((
_tmp641='r',((_get_dyneither_size(_tmp63F,sizeof(char))== 1  && (_tmp640 == '\000'
 && _tmp641 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63F.curr)=_tmp641)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp644;char _tmp643;struct
_dyneither_ptr _tmp642;(_tmp642=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp643=*((
char*)_check_dyneither_subscript(_tmp642,sizeof(char),0)),((_tmp644='\\',((
_get_dyneither_size(_tmp642,sizeof(char))== 1  && (_tmp643 == '\000'  && _tmp644 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp642.curr)=_tmp644)))))));}{char _tmp647;char
_tmp646;struct _dyneither_ptr _tmp645;(_tmp645=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp646=*((char*)_check_dyneither_subscript(_tmp645,sizeof(char),0)),((
_tmp647='t',((_get_dyneither_size(_tmp645,sizeof(char))== 1  && (_tmp646 == '\000'
 && _tmp647 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp645.curr)=_tmp647)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp64A;char _tmp649;struct
_dyneither_ptr _tmp648;(_tmp648=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp649=*((
char*)_check_dyneither_subscript(_tmp648,sizeof(char),0)),((_tmp64A='\\',((
_get_dyneither_size(_tmp648,sizeof(char))== 1  && (_tmp649 == '\000'  && _tmp64A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp648.curr)=_tmp64A)))))));}{char _tmp64D;char
_tmp64C;struct _dyneither_ptr _tmp64B;(_tmp64B=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp64C=*((char*)_check_dyneither_subscript(_tmp64B,sizeof(char),0)),((
_tmp64D='v',((_get_dyneither_size(_tmp64B,sizeof(char))== 1  && (_tmp64C == '\000'
 && _tmp64D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64B.curr)=_tmp64D)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp650;char _tmp64F;struct
_dyneither_ptr _tmp64E;(_tmp64E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64F=*((
char*)_check_dyneither_subscript(_tmp64E,sizeof(char),0)),((_tmp650='\\',((
_get_dyneither_size(_tmp64E,sizeof(char))== 1  && (_tmp64F == '\000'  && _tmp650 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp64E.curr)=_tmp650)))))));}{char _tmp653;char
_tmp652;struct _dyneither_ptr _tmp651;(_tmp651=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp652=*((char*)_check_dyneither_subscript(_tmp651,sizeof(char),0)),((
_tmp653='\\',((_get_dyneither_size(_tmp651,sizeof(char))== 1  && (_tmp652 == '\000'
 && _tmp653 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp651.curr)=_tmp653)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp656;char _tmp655;struct
_dyneither_ptr _tmp654;(_tmp654=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp655=*((
char*)_check_dyneither_subscript(_tmp654,sizeof(char),0)),((_tmp656='\\',((
_get_dyneither_size(_tmp654,sizeof(char))== 1  && (_tmp655 == '\000'  && _tmp656 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp654.curr)=_tmp656)))))));}{char _tmp659;char
_tmp658;struct _dyneither_ptr _tmp657;(_tmp657=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp658=*((char*)_check_dyneither_subscript(_tmp657,sizeof(char),0)),((
_tmp659='"',((_get_dyneither_size(_tmp657,sizeof(char))== 1  && (_tmp658 == '\000'
 && _tmp659 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp657.curr)=_tmp659)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp65C;char _tmp65B;
struct _dyneither_ptr _tmp65A;(_tmp65A=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp65B=*((char*)_check_dyneither_subscript(_tmp65A,sizeof(char),0)),((_tmp65C=
_tmp1E,((_get_dyneither_size(_tmp65A,sizeof(char))== 1  && (_tmp65B == '\000'  && 
_tmp65C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65A.curr)=_tmp65C)))))));}
else{{char _tmp65F;char _tmp65E;struct _dyneither_ptr _tmp65D;(_tmp65D=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp65E=*((char*)
_check_dyneither_subscript(_tmp65D,sizeof(char),0)),((_tmp65F='\\',((
_get_dyneither_size(_tmp65D,sizeof(char))== 1  && (_tmp65E == '\000'  && _tmp65F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp65D.curr)=_tmp65F)))))));}{char _tmp662;char
_tmp661;struct _dyneither_ptr _tmp660;(_tmp660=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp661=*((char*)_check_dyneither_subscript(_tmp660,sizeof(char),0)),((
_tmp662=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp660,sizeof(char))
== 1  && (_tmp661 == '\000'  && _tmp662 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp660.curr)=_tmp662)))))));}{char _tmp665;char _tmp664;struct _dyneither_ptr
_tmp663;(_tmp663=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp664=*((char*)
_check_dyneither_subscript(_tmp663,sizeof(char),0)),((_tmp665=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp663,sizeof(char))== 1  && (_tmp664 == '\000'
 && _tmp665 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp663.curr)=_tmp665)))))));}{
char _tmp668;char _tmp667;struct _dyneither_ptr _tmp666;(_tmp666=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp667=*((char*)_check_dyneither_subscript(_tmp666,
sizeof(char),0)),((_tmp668=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp666,sizeof(char))== 1  && (_tmp667 == '\000'  && _tmp668 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp666.curr)=_tmp668)))))));};}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;};};};}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*
l=0;if(tq.q_restrict){struct Cyc_List_List*_tmp669;l=((_tmp669=_cycalloc(sizeof(*
_tmp669)),((_tmp669->hd=Cyc_Absynpp_restrict_sp,((_tmp669->tl=l,_tmp669))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp66A;l=((_tmp66A=_cycalloc(sizeof(*
_tmp66A)),((_tmp66A->hd=Cyc_Absynpp_volatile_sp,((_tmp66A->tl=l,_tmp66A))))));}
if(tq.print_const){struct Cyc_List_List*_tmp66B;l=((_tmp66B=_cycalloc(sizeof(*
_tmp66B)),((_tmp66B->hd=Cyc_Absynpp_const_sp,((_tmp66B->tl=l,_tmp66B))))));}{
const char*_tmp66E;const char*_tmp66D;const char*_tmp66C;return Cyc_PP_egroup(((
_tmp66C="",_tag_dyneither(_tmp66C,sizeof(char),1))),((_tmp66D=" ",_tag_dyneither(
_tmp66D,sizeof(char),2))),((_tmp66E=" ",_tag_dyneither(_tmp66E,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,l));};}struct _dyneither_ptr Cyc_Absynpp_kind2string(
struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp6E;enum Cyc_Absyn_KindQual
_tmp6F;enum Cyc_Absyn_AliasQual _tmp70;struct Cyc_Absyn_Kind*_tmp6D=ka;_tmp6E=*
_tmp6D;_tmp6F=_tmp6E.kind;_tmp70=_tmp6E.aliasqual;switch(_tmp6F){case Cyc_Absyn_AnyKind:
_LL36: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL38: {const char*_tmp66F;return(
_tmp66F="A",_tag_dyneither(_tmp66F,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp670;return(_tmp670="UA",_tag_dyneither(_tmp670,sizeof(char),3));}
case Cyc_Absyn_Top: _LL3A: {const char*_tmp671;return(_tmp671="TA",_tag_dyneither(
_tmp671,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37: switch(_tmp70){case Cyc_Absyn_Aliasable:
_LL3D: {const char*_tmp672;return(_tmp672="M",_tag_dyneither(_tmp672,sizeof(char),
2));}case Cyc_Absyn_Unique: _LL3E: {const char*_tmp673;return(_tmp673="UM",
_tag_dyneither(_tmp673,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {const char*
_tmp674;return(_tmp674="TM",_tag_dyneither(_tmp674,sizeof(char),3));}}case Cyc_Absyn_BoxKind:
_LL3C: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL42: {const char*_tmp675;return(
_tmp675="B",_tag_dyneither(_tmp675,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp676;return(_tmp676="UB",_tag_dyneither(_tmp676,sizeof(char),3));}
case Cyc_Absyn_Top: _LL44: {const char*_tmp677;return(_tmp677="TB",_tag_dyneither(
_tmp677,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41: switch(_tmp70){case Cyc_Absyn_Aliasable:
_LL47: {const char*_tmp678;return(_tmp678="R",_tag_dyneither(_tmp678,sizeof(char),
2));}case Cyc_Absyn_Unique: _LL48: {const char*_tmp679;return(_tmp679="UR",
_tag_dyneither(_tmp679,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {const char*
_tmp67A;return(_tmp67A="TR",_tag_dyneither(_tmp67A,sizeof(char),3));}}case Cyc_Absyn_EffKind:
_LL46: {const char*_tmp67B;return(_tmp67B="E",_tag_dyneither(_tmp67B,sizeof(char),
2));}case Cyc_Absyn_IntKind: _LL4B: {const char*_tmp67C;return(_tmp67C="I",
_tag_dyneither(_tmp67C,sizeof(char),2));}}}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp7F=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp81;struct Cyc_Absyn_Kind*_tmp84;_LL4E: {struct Cyc_Absyn_Eq_kb_struct*
_tmp80=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7F;if(_tmp80->tag != 0)goto _LL50;else{
_tmp81=_tmp80->f1;}}_LL4F: return Cyc_Absynpp_kind2string(_tmp81);_LL50: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp82=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp7F;
if(_tmp82->tag != 1)goto _LL52;}_LL51: if(Cyc_PP_tex_output){const char*_tmp67D;
return(_tmp67D="{?}",_tag_dyneither(_tmp67D,sizeof(char),4));}else{const char*
_tmp67E;return(_tmp67E="?",_tag_dyneither(_tmp67E,sizeof(char),2));}_LL52: {
struct Cyc_Absyn_Less_kb_struct*_tmp83=(struct Cyc_Absyn_Less_kb_struct*)_tmp7F;
if(_tmp83->tag != 2)goto _LL4D;else{_tmp84=_tmp83->f2;}}_LL53: {const char*_tmp682;
void*_tmp681[1];struct Cyc_String_pa_struct _tmp680;return(struct _dyneither_ptr)((
_tmp680.tag=0,((_tmp680.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp84)),((_tmp681[0]=& _tmp680,Cyc_aprintf(((_tmp682="<=%s",_tag_dyneither(
_tmp682,sizeof(char),5))),_tag_dyneither(_tmp681,sizeof(void*),1))))))));}_LL4D:;}
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){void*_tmp8A=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp8C;struct Cyc_Absyn_Kind*_tmp8F;_LL55: {struct Cyc_Absyn_Eq_kb_struct*
_tmp8B=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL57;else{
_tmp8C=_tmp8B->f1;}}_LL56: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8C));
_LL57: {struct Cyc_Absyn_Unknown_kb_struct*_tmp8D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8A;if(_tmp8D->tag != 1)goto _LL59;}_LL58: if(Cyc_PP_tex_output){const char*
_tmp683;return Cyc_PP_text_width(((_tmp683="{?}",_tag_dyneither(_tmp683,sizeof(
char),4))),1);}else{const char*_tmp684;return Cyc_PP_text(((_tmp684="?",
_tag_dyneither(_tmp684,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_struct*
_tmp8E=(struct Cyc_Absyn_Less_kb_struct*)_tmp8A;if(_tmp8E->tag != 2)goto _LL54;
else{_tmp8F=_tmp8E->f2;}}_LL5A: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8F));
_LL54:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*
_tmp687;const char*_tmp686;const char*_tmp685;return Cyc_PP_egroup(((_tmp685="<",
_tag_dyneither(_tmp685,sizeof(char),2))),((_tmp686=">",_tag_dyneither(_tmp686,
sizeof(char),2))),((_tmp687=",",_tag_dyneither(_tmp687,sizeof(char),2))),((
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
_tmp68A;struct Cyc_PP_Doc*_tmp689[3];return(_tmp689[2]=Cyc_Absynpp_kind2doc(
_tmp9F),((_tmp689[1]=Cyc_PP_text(((_tmp68A="::",_tag_dyneither(_tmp68A,sizeof(
char),3)))),((_tmp689[0]=Cyc_PP_textptr(tv->name),Cyc_PP_cat(_tag_dyneither(
_tmp689,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs){const char*_tmp68D;const char*_tmp68C;const char*_tmp68B;
return Cyc_PP_egroup(((_tmp68B="<",_tag_dyneither(_tmp68B,sizeof(char),2))),((
_tmp68C=">",_tag_dyneither(_tmp68C,sizeof(char),2))),((_tmp68D=",",
_tag_dyneither(_tmp68D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,
tvs));}static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);{const char*
_tmp690;const char*_tmp68F;const char*_tmp68E;return Cyc_PP_egroup(((_tmp68E="<",
_tag_dyneither(_tmp68E,sizeof(char),2))),((_tmp68F=">",_tag_dyneither(_tmp68F,
sizeof(char),2))),((_tmp690=",",_tag_dyneither(_tmp690,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple15*t){return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*
Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){const char*_tmp693;const char*_tmp692;
const char*_tmp691;return Cyc_PP_group(((_tmp691="(",_tag_dyneither(_tmp691,
sizeof(char),2))),((_tmp692=")",_tag_dyneither(_tmp692,sizeof(char),2))),((
_tmp693=",",_tag_dyneither(_tmp693,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmpAB=att;
_LL65: {struct Cyc_Absyn_Stdcall_att_struct*_tmpAC=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmpAB;if(_tmpAC->tag != 1)goto _LL67;}_LL66: goto _LL68;_LL67: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpAD=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpAB;if(_tmpAD->tag != 2)goto _LL69;}
_LL68: goto _LL6A;_LL69: {struct Cyc_Absyn_Fastcall_att_struct*_tmpAE=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpAB;if(_tmpAE->tag != 3)goto _LL6B;}_LL6A: return Cyc_PP_nil_doc();_LL6B:;_LL6C:
return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL64:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(
struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmpAF=(void*)atts->hd;
_LL6E: {struct Cyc_Absyn_Stdcall_att_struct*_tmpB0=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmpAF;if(_tmpB0->tag != 1)goto _LL70;}_LL6F: {const char*_tmp694;return Cyc_PP_text(((
_tmp694=" _stdcall ",_tag_dyneither(_tmp694,sizeof(char),11))));}_LL70: {struct
Cyc_Absyn_Cdecl_att_struct*_tmpB1=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpAF;if(
_tmpB1->tag != 2)goto _LL72;}_LL71: {const char*_tmp695;return Cyc_PP_text(((_tmp695=" _cdecl ",
_tag_dyneither(_tmp695,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpB2=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpAF;if(_tmpB2->tag != 3)goto _LL74;}
_LL73: {const char*_tmp696;return Cyc_PP_text(((_tmp696=" _fastcall ",
_tag_dyneither(_tmp696,sizeof(char),12))));}_LL74:;_LL75: goto _LL6D;_LL6D:;}
return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpB6=(void*)atts2->hd;_LL77: {struct Cyc_Absyn_Stdcall_att_struct*_tmpB7=(
struct Cyc_Absyn_Stdcall_att_struct*)_tmpB6;if(_tmpB7->tag != 1)goto _LL79;}_LL78:
goto _LL7A;_LL79: {struct Cyc_Absyn_Cdecl_att_struct*_tmpB8=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmpB6;if(_tmpB8->tag != 2)goto _LL7B;}_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpB9=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpB6;if(_tmpB9->tag != 3)goto _LL7D;}
_LL7C: goto _LL76;_LL7D:;_LL7E: hasatt=1;goto _LL76;_LL76:;}}if(!hasatt)return Cyc_PP_nil_doc();{
const char*_tmp6A1;const char*_tmp6A0;const char*_tmp69F;const char*_tmp69E;const
char*_tmp69D;struct Cyc_PP_Doc*_tmp69C[3];return(_tmp69C[2]=Cyc_PP_text(((_tmp69D=")",
_tag_dyneither(_tmp69D,sizeof(char),2)))),((_tmp69C[1]=Cyc_PP_group(((_tmp6A0="",
_tag_dyneither(_tmp6A0,sizeof(char),1))),((_tmp69F="",_tag_dyneither(_tmp69F,
sizeof(char),1))),((_tmp69E=" ",_tag_dyneither(_tmp69E,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_noncallatt2doc,atts)),((_tmp69C[0]=Cyc_PP_text(((_tmp6A1=" __declspec(",
_tag_dyneither(_tmp6A1,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp69C,
sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*
a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Vc_c: _LL7F: return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c:
_LL80: {const char*_tmp6AA;const char*_tmp6A9;const char*_tmp6A8;const char*_tmp6A7;
struct Cyc_PP_Doc*_tmp6A6[2];return(_tmp6A6[1]=Cyc_PP_group(((_tmp6A9="((",
_tag_dyneither(_tmp6A9,sizeof(char),3))),((_tmp6A8="))",_tag_dyneither(_tmp6A8,
sizeof(char),3))),((_tmp6A7=",",_tag_dyneither(_tmp6A7,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_att2doc,atts)),((_tmp6A6[0]=Cyc_PP_text(((_tmp6AA=" __attribute__",
_tag_dyneither(_tmp6AA,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp6A6,
sizeof(struct Cyc_PP_Doc*),2)))));}}}int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*
tms){if(tms == 0)return 0;{void*_tmpC5=(void*)tms->hd;_LL83: {struct Cyc_Absyn_Pointer_mod_struct*
_tmpC6=(struct Cyc_Absyn_Pointer_mod_struct*)_tmpC5;if(_tmpC6->tag != 2)goto _LL85;}
_LL84: return 1;_LL85: {struct Cyc_Absyn_Attributes_mod_struct*_tmpC7=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC5;if(_tmpC7->tag != 5)goto _LL87;}_LL86: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL89: return 0;default: _LL8A: return Cyc_Absynpp_next_is_pointer(
tms->tl);}_LL87:;_LL88: return 0;_LL82:;};}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*
Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output){
const char*_tmp6AB;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp6AB="{?}",_tag_dyneither(_tmp6AB,sizeof(char),4))),1);}else{const char*
_tmp6AC;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp6AC="?",
_tag_dyneither(_tmp6AC,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){const char*_tmp6AD;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp6AD="{\\lb}",_tag_dyneither(_tmp6AD,sizeof(char),6))),1);}
else{const char*_tmp6AE;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp6AE="{",_tag_dyneither(_tmp6AE,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_lb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){const char*_tmp6AF;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp6AF="{\\rb}",_tag_dyneither(_tmp6AF,sizeof(char),6))),1);}
else{const char*_tmp6B0;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp6B0="}",_tag_dyneither(_tmp6B0,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){const char*_tmp6B1;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp6B1="\\$",_tag_dyneither(_tmp6B1,sizeof(char),3))),1);}
else{const char*_tmp6B2;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp6B2="$",_tag_dyneither(_tmp6B2,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_dollar);}struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(
struct _dyneither_ptr sep,struct Cyc_List_List*ss){struct Cyc_PP_Doc*_tmp6B3[3];
return(_tmp6B3[2]=Cyc_Absynpp_rb(),((_tmp6B3[1]=Cyc_PP_seq(sep,ss),((_tmp6B3[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp6B3,sizeof(struct Cyc_PP_Doc*),3)))))));}
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){for(0;tms != 0;tms=tms->tl){
void*_tmpD1=(void*)tms->hd;void*_tmpD5;struct Cyc_List_List*_tmpD7;_LL8D: {struct
Cyc_Absyn_Carray_mod_struct*_tmpD2=(struct Cyc_Absyn_Carray_mod_struct*)_tmpD1;
if(_tmpD2->tag != 0)goto _LL8F;}_LL8E:{const char*_tmp6B6;void*_tmp6B5;(_tmp6B5=0,
Cyc_fprintf(Cyc_stderr,((_tmp6B6="Carray_mod ",_tag_dyneither(_tmp6B6,sizeof(
char),12))),_tag_dyneither(_tmp6B5,sizeof(void*),0)));}goto _LL8C;_LL8F: {struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD3=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD1;if(_tmpD3->tag != 1)goto _LL91;}_LL90:{const char*_tmp6B9;void*_tmp6B8;(
_tmp6B8=0,Cyc_fprintf(Cyc_stderr,((_tmp6B9="ConstArray_mod ",_tag_dyneither(
_tmp6B9,sizeof(char),16))),_tag_dyneither(_tmp6B8,sizeof(void*),0)));}goto _LL8C;
_LL91: {struct Cyc_Absyn_Function_mod_struct*_tmpD4=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD4->tag != 3)goto _LL93;else{_tmpD5=(void*)_tmpD4->f1;{struct Cyc_Absyn_WithTypes_struct*
_tmpD6=(struct Cyc_Absyn_WithTypes_struct*)_tmpD5;if(_tmpD6->tag != 1)goto _LL93;
else{_tmpD7=_tmpD6->f1;}};}}_LL92:{const char*_tmp6BC;void*_tmp6BB;(_tmp6BB=0,Cyc_fprintf(
Cyc_stderr,((_tmp6BC="Function_mod(",_tag_dyneither(_tmp6BC,sizeof(char),14))),
_tag_dyneither(_tmp6BB,sizeof(void*),0)));}for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){
struct Cyc_Core_Opt*_tmpE2=(*((struct _tuple7*)_tmpD7->hd)).f1;if(_tmpE2 == 0){
const char*_tmp6BF;void*_tmp6BE;(_tmp6BE=0,Cyc_fprintf(Cyc_stderr,((_tmp6BF="?",
_tag_dyneither(_tmp6BF,sizeof(char),2))),_tag_dyneither(_tmp6BE,sizeof(void*),0)));}
else{void*_tmp6C0;(_tmp6C0=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)
_tmpE2->v),_tag_dyneither(_tmp6C0,sizeof(void*),0)));}if(_tmpD7->tl != 0){const
char*_tmp6C3;void*_tmp6C2;(_tmp6C2=0,Cyc_fprintf(Cyc_stderr,((_tmp6C3=",",
_tag_dyneither(_tmp6C3,sizeof(char),2))),_tag_dyneither(_tmp6C2,sizeof(void*),0)));}}{
const char*_tmp6C6;void*_tmp6C5;(_tmp6C5=0,Cyc_fprintf(Cyc_stderr,((_tmp6C6=") ",
_tag_dyneither(_tmp6C6,sizeof(char),3))),_tag_dyneither(_tmp6C5,sizeof(void*),0)));}
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_struct*_tmpD8=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD8->tag != 3)goto _LL95;}_LL94:{const char*_tmp6C9;void*_tmp6C8;(
_tmp6C8=0,Cyc_fprintf(Cyc_stderr,((_tmp6C9="Function_mod()",_tag_dyneither(
_tmp6C9,sizeof(char),15))),_tag_dyneither(_tmp6C8,sizeof(void*),0)));}goto _LL8C;
_LL95: {struct Cyc_Absyn_Attributes_mod_struct*_tmpD9=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpD1;if(_tmpD9->tag != 5)goto _LL97;}_LL96:{const char*_tmp6CC;void*_tmp6CB;(
_tmp6CB=0,Cyc_fprintf(Cyc_stderr,((_tmp6CC="Attributes_mod ",_tag_dyneither(
_tmp6CC,sizeof(char),16))),_tag_dyneither(_tmp6CB,sizeof(void*),0)));}goto _LL8C;
_LL97: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpDA=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpD1;if(_tmpDA->tag != 4)goto _LL99;}_LL98:{const char*_tmp6CF;void*_tmp6CE;(
_tmp6CE=0,Cyc_fprintf(Cyc_stderr,((_tmp6CF="TypeParams_mod ",_tag_dyneither(
_tmp6CF,sizeof(char),16))),_tag_dyneither(_tmp6CE,sizeof(void*),0)));}goto _LL8C;
_LL99: {struct Cyc_Absyn_Pointer_mod_struct*_tmpDB=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpD1;if(_tmpDB->tag != 2)goto _LL8C;}_LL9A:{const char*_tmp6D2;void*_tmp6D1;(
_tmp6D1=0,Cyc_fprintf(Cyc_stderr,((_tmp6D2="Pointer_mod ",_tag_dyneither(_tmp6D2,
sizeof(char),13))),_tag_dyneither(_tmp6D1,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
const char*_tmp6D5;void*_tmp6D4;(_tmp6D4=0,Cyc_fprintf(Cyc_stderr,((_tmp6D5="\n",
_tag_dyneither(_tmp6D5,sizeof(char),2))),_tag_dyneither(_tmp6D4,sizeof(void*),0)));};}
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct
Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(
0,d,tms->tl);const char*_tmp6DA;const char*_tmp6D9;struct Cyc_PP_Doc*_tmp6D8[3];
struct Cyc_PP_Doc*p_rest=(_tmp6D8[2]=Cyc_PP_text(((_tmp6D9=")",_tag_dyneither(
_tmp6D9,sizeof(char),2)))),((_tmp6D8[1]=rest,((_tmp6D8[0]=Cyc_PP_text(((_tmp6DA="(",
_tag_dyneither(_tmp6DA,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6D8,
sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpF4=(void*)tms->hd;union Cyc_Absyn_Constraint*
_tmpF6;struct Cyc_Absyn_Exp*_tmpF8;union Cyc_Absyn_Constraint*_tmpF9;void*_tmpFB;
struct Cyc_List_List*_tmpFD;struct Cyc_List_List*_tmpFF;struct Cyc_Position_Segment*
_tmp100;int _tmp101;struct Cyc_Absyn_PtrAtts _tmp103;void*_tmp104;union Cyc_Absyn_Constraint*
_tmp105;union Cyc_Absyn_Constraint*_tmp106;union Cyc_Absyn_Constraint*_tmp107;
struct Cyc_Absyn_Tqual _tmp108;_LL9C: {struct Cyc_Absyn_Carray_mod_struct*_tmpF5=(
struct Cyc_Absyn_Carray_mod_struct*)_tmpF4;if(_tmpF5->tag != 0)goto _LL9E;else{
_tmpF6=_tmpF5->f1;}}_LL9D: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6DF;const char*_tmp6DE;struct Cyc_PP_Doc*_tmp6DD[2];return(_tmp6DD[1]=((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF6)?Cyc_PP_text(((
_tmp6DE="[]@zeroterm",_tag_dyneither(_tmp6DE,sizeof(char),12)))): Cyc_PP_text(((
_tmp6DF="[]",_tag_dyneither(_tmp6DF,sizeof(char),3)))),((_tmp6DD[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp6DD,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpF7=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpF4;if(_tmpF7->tag != 1)goto
_LLA0;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;}}_LL9F: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp6E6;const char*_tmp6E5;const char*_tmp6E4;
struct Cyc_PP_Doc*_tmp6E3[4];return(_tmp6E3[3]=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpF9)?Cyc_PP_text(((_tmp6E4="]@zeroterm",
_tag_dyneither(_tmp6E4,sizeof(char),11)))): Cyc_PP_text(((_tmp6E5="]",
_tag_dyneither(_tmp6E5,sizeof(char),2)))),((_tmp6E3[2]=Cyc_Absynpp_exp2doc(
_tmpF8),((_tmp6E3[1]=Cyc_PP_text(((_tmp6E6="[",_tag_dyneither(_tmp6E6,sizeof(
char),2)))),((_tmp6E3[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6E3,sizeof(struct Cyc_PP_Doc*),
4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_struct*_tmpFA=(struct Cyc_Absyn_Function_mod_struct*)
_tmpF4;if(_tmpFA->tag != 3)goto _LLA2;else{_tmpFB=(void*)_tmpFA->f1;}}_LLA1: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmp110=_tmpFB;struct Cyc_List_List*_tmp112;int _tmp113;
struct Cyc_Absyn_VarargInfo*_tmp114;struct Cyc_Core_Opt*_tmp115;struct Cyc_List_List*
_tmp116;struct Cyc_List_List*_tmp118;struct Cyc_Position_Segment*_tmp119;_LLA9: {
struct Cyc_Absyn_WithTypes_struct*_tmp111=(struct Cyc_Absyn_WithTypes_struct*)
_tmp110;if(_tmp111->tag != 1)goto _LLAB;else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;
_tmp114=_tmp111->f3;_tmp115=_tmp111->f4;_tmp116=_tmp111->f5;}}_LLAA: {struct Cyc_PP_Doc*
_tmp6E7[2];return(_tmp6E7[1]=Cyc_Absynpp_funargs2doc(_tmp112,_tmp113,_tmp114,
_tmp115,_tmp116),((_tmp6E7[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6E7,sizeof(
struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_struct*_tmp117=(
struct Cyc_Absyn_NoTypes_struct*)_tmp110;if(_tmp117->tag != 0)goto _LLA8;else{
_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LLAC: {const char*_tmp6EE;const char*
_tmp6ED;const char*_tmp6EC;struct Cyc_PP_Doc*_tmp6EB[2];return(_tmp6EB[1]=Cyc_PP_group(((
_tmp6EE="(",_tag_dyneither(_tmp6EE,sizeof(char),2))),((_tmp6ED=")",
_tag_dyneither(_tmp6ED,sizeof(char),2))),((_tmp6EC=",",_tag_dyneither(_tmp6EC,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp118)),((
_tmp6EB[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6EB,sizeof(struct Cyc_PP_Doc*),2)))));}
_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_struct*_tmpFC=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpF4;if(_tmpFC->tag != 5)goto _LLA4;else{_tmpFD=_tmpFC->f2;}}_LLA3: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6EF[2];return(_tmp6EF[1]=Cyc_Absynpp_atts2doc(_tmpFD),((
_tmp6EF[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6EF,sizeof(struct Cyc_PP_Doc*),2)))));};
case Cyc_Cyclone_Vc_c: _LLAE: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp6F0[2];return(_tmp6F0[1]=rest,((_tmp6F0[0]=Cyc_Absynpp_callconv2doc(_tmpFD),
Cyc_PP_cat(_tag_dyneither(_tmp6F0,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;}
_LLA4: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpFE=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpF4;if(_tmpFE->tag != 4)goto _LLA6;else{_tmpFF=_tmpFE->f1;_tmp100=_tmpFE->f2;
_tmp101=_tmpFE->f3;}}_LLA5: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp101){struct Cyc_PP_Doc*_tmp6F1[2];return(_tmp6F1[1]=Cyc_Absynpp_ktvars2doc(
_tmpFF),((_tmp6F1[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F1,sizeof(struct Cyc_PP_Doc*),
2)))));}else{struct Cyc_PP_Doc*_tmp6F2[2];return(_tmp6F2[1]=Cyc_Absynpp_tvars2doc(
_tmpFF),((_tmp6F2[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F2,sizeof(struct Cyc_PP_Doc*),
2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_struct*_tmp102=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpF4;if(_tmp102->tag != 2)goto _LL9B;else{_tmp103=_tmp102->f1;_tmp104=_tmp103.rgn;
_tmp105=_tmp103.nullable;_tmp106=_tmp103.bounds;_tmp107=_tmp103.zero_term;
_tmp108=_tmp102->f2;}}_LLA7: {struct Cyc_PP_Doc*ptr;{void*_tmp123=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp106);struct Cyc_Absyn_Exp*_tmp126;_LLB1: {struct Cyc_Absyn_DynEither_b_struct*
_tmp124=(struct Cyc_Absyn_DynEither_b_struct*)_tmp123;if(_tmp124->tag != 0)goto
_LLB3;}_LLB2: ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_struct*
_tmp125=(struct Cyc_Absyn_Upper_b_struct*)_tmp123;if(_tmp125->tag != 1)goto _LLB0;
else{_tmp126=_tmp125->f1;}}_LLB4:{const char*_tmp6F4;const char*_tmp6F3;ptr=Cyc_PP_text(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp105)?(
_tmp6F4="*",_tag_dyneither(_tmp6F4,sizeof(char),2)):((_tmp6F3="@",_tag_dyneither(
_tmp6F3,sizeof(char),2))));}{unsigned int _tmp12A;int _tmp12B;struct _tuple10
_tmp129=Cyc_Evexp_eval_const_uint_exp(_tmp126);_tmp12A=_tmp129.f1;_tmp12B=
_tmp129.f2;if(!_tmp12B  || _tmp12A != 1){struct Cyc_PP_Doc*_tmp6F5[4];ptr=((_tmp6F5[
3]=Cyc_Absynpp_rb(),((_tmp6F5[2]=Cyc_Absynpp_exp2doc(_tmp126),((_tmp6F5[1]=Cyc_Absynpp_lb(),((
_tmp6F5[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6F5,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;};_LLB0:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){const char*
_tmp6F8;struct Cyc_PP_Doc*_tmp6F7[2];ptr=((_tmp6F7[1]=Cyc_PP_text(((_tmp6F8="@zeroterm ",
_tag_dyneither(_tmp6F8,sizeof(char),11)))),((_tmp6F7[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6F7,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){
const char*_tmp6FB;struct Cyc_PP_Doc*_tmp6FA[2];ptr=((_tmp6FA[1]=Cyc_PP_text(((
_tmp6FB="@nozeroterm ",_tag_dyneither(_tmp6FB,sizeof(char),13)))),((_tmp6FA[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp6FA,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmp131=Cyc_Tcutil_compress(_tmp104);_LLB6: {struct Cyc_Absyn_HeapRgn_struct*
_tmp132=(struct Cyc_Absyn_HeapRgn_struct*)_tmp131;if(_tmp132->tag != 21)goto _LLB8;}
_LLB7: goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_struct*_tmp133=(struct Cyc_Absyn_Evar_struct*)
_tmp131;if(_tmp133->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto
_LLBA;_LLB9: goto _LLB5;_LLBA:;_LLBB: {const char*_tmp6FE;struct Cyc_PP_Doc*_tmp6FD[
3];ptr=((_tmp6FD[2]=Cyc_PP_text(((_tmp6FE=" ",_tag_dyneither(_tmp6FE,sizeof(char),
2)))),((_tmp6FD[1]=Cyc_Absynpp_typ2doc(_tmp104),((_tmp6FD[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6FD,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}{struct Cyc_PP_Doc*
_tmp6FF[2];ptr=((_tmp6FF[1]=Cyc_Absynpp_tqual2doc(_tmp108),((_tmp6FF[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6FF,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp700[2];return(_tmp700[1]=rest,((_tmp700[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp700,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t){void*_tmp13B=Cyc_Tcutil_compress(t);_LLBD: {struct Cyc_Absyn_HeapRgn_struct*
_tmp13C=(struct Cyc_Absyn_HeapRgn_struct*)_tmp13B;if(_tmp13C->tag != 21)goto _LLBF;}
_LLBE: {const char*_tmp701;return Cyc_PP_text(((_tmp701="`H",_tag_dyneither(
_tmp701,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_struct*_tmp13D=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp13B;if(_tmp13D->tag != 22)goto _LLC1;}_LLC0: {
const char*_tmp702;return Cyc_PP_text(((_tmp702="`U",_tag_dyneither(_tmp702,
sizeof(char),3))));}_LLC1:;_LLC2: return Cyc_Absynpp_ntyp2doc(t);_LLBC:;}static
void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t){void*_tmp140=Cyc_Tcutil_compress(t);void*_tmp142;struct Cyc_List_List*
_tmp144;_LLC4: {struct Cyc_Absyn_AccessEff_struct*_tmp141=(struct Cyc_Absyn_AccessEff_struct*)
_tmp140;if(_tmp141->tag != 23)goto _LLC6;else{_tmp142=(void*)_tmp141->f1;}}_LLC5:{
struct Cyc_List_List*_tmp703;*rgions=((_tmp703=_cycalloc(sizeof(*_tmp703)),((
_tmp703->hd=Cyc_Absynpp_rgn2doc(_tmp142),((_tmp703->tl=*rgions,_tmp703))))));}
goto _LLC3;_LLC6: {struct Cyc_Absyn_JoinEff_struct*_tmp143=(struct Cyc_Absyn_JoinEff_struct*)
_tmp140;if(_tmp143->tag != 24)goto _LLC8;else{_tmp144=_tmp143->f1;}}_LLC7: for(0;
_tmp144 != 0;_tmp144=_tmp144->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)
_tmp144->hd);}goto _LLC3;_LLC8:;_LLC9:{struct Cyc_List_List*_tmp704;*effects=((
_tmp704=_cycalloc(sizeof(*_tmp704)),((_tmp704->hd=Cyc_Absynpp_typ2doc(t),((
_tmp704->tl=*effects,_tmp704))))));}goto _LLC3;_LLC3:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(
void*t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0){const char*_tmp707;const char*_tmp706;const
char*_tmp705;return Cyc_PP_group(((_tmp705="",_tag_dyneither(_tmp705,sizeof(char),
1))),((_tmp706="",_tag_dyneither(_tmp706,sizeof(char),1))),((_tmp707="+",
_tag_dyneither(_tmp707,sizeof(char),2))),effects);}else{const char*_tmp708;struct
Cyc_PP_Doc*_tmp14A=Cyc_Absynpp_group_braces(((_tmp708=",",_tag_dyneither(_tmp708,
sizeof(char),2))),rgions);struct Cyc_List_List*_tmp70C;const char*_tmp70B;const
char*_tmp70A;const char*_tmp709;return Cyc_PP_group(((_tmp709="",_tag_dyneither(
_tmp709,sizeof(char),1))),((_tmp70A="",_tag_dyneither(_tmp70A,sizeof(char),1))),((
_tmp70B="+",_tag_dyneither(_tmp70B,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp70C=_cycalloc(sizeof(*_tmp70C)),((_tmp70C->hd=_tmp14A,((_tmp70C->tl=0,
_tmp70C))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind
k){switch(k){case Cyc_Absyn_StructA: _LLCA: {const char*_tmp70D;return Cyc_PP_text(((
_tmp70D="struct ",_tag_dyneither(_tmp70D,sizeof(char),8))));}case Cyc_Absyn_UnionA:
_LLCB: {const char*_tmp70E;return Cyc_PP_text(((_tmp70E="union ",_tag_dyneither(
_tmp70E,sizeof(char),7))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;{void*_tmp152=t;struct Cyc_Core_Opt*_tmp158;struct Cyc_Core_Opt*
_tmp159;int _tmp15A;struct Cyc_Core_Opt*_tmp15B;struct Cyc_Absyn_Tvar*_tmp15D;
struct Cyc_Absyn_DatatypeInfo _tmp15F;union Cyc_Absyn_DatatypeInfoU _tmp160;struct
Cyc_List_List*_tmp161;struct Cyc_Absyn_DatatypeFieldInfo _tmp163;union Cyc_Absyn_DatatypeFieldInfoU
_tmp164;struct Cyc_List_List*_tmp165;enum Cyc_Absyn_Sign _tmp167;enum Cyc_Absyn_Size_of
_tmp168;int _tmp16B;struct Cyc_List_List*_tmp16D;struct Cyc_Absyn_AggrInfo _tmp16F;
union Cyc_Absyn_AggrInfoU _tmp170;struct Cyc_List_List*_tmp171;enum Cyc_Absyn_AggrKind
_tmp173;struct Cyc_List_List*_tmp174;struct Cyc_List_List*_tmp176;struct _tuple0*
_tmp178;struct Cyc_Absyn_Exp*_tmp17A;struct _tuple0*_tmp17C;struct Cyc_List_List*
_tmp17D;struct Cyc_Absyn_Typedefdecl*_tmp17E;void*_tmp180;void*_tmp182;void*
_tmp183;void*_tmp185;void*_tmp189;struct Cyc_Absyn_TypeDecl*_tmp18D;struct Cyc_Absyn_TypeDecl
_tmp18E;void*_tmp18F;struct Cyc_Absyn_Aggrdecl*_tmp191;struct Cyc_Absyn_TypeDecl*
_tmp193;struct Cyc_Absyn_TypeDecl _tmp194;void*_tmp195;struct Cyc_Absyn_Enumdecl*
_tmp197;struct Cyc_Absyn_TypeDecl*_tmp199;struct Cyc_Absyn_TypeDecl _tmp19A;void*
_tmp19B;struct Cyc_Absyn_Datatypedecl*_tmp19D;_LLCE: {struct Cyc_Absyn_ArrayType_struct*
_tmp153=(struct Cyc_Absyn_ArrayType_struct*)_tmp152;if(_tmp153->tag != 9)goto _LLD0;}
_LLCF: {const char*_tmp70F;return Cyc_PP_text(((_tmp70F="[[[array]]]",
_tag_dyneither(_tmp70F,sizeof(char),12))));}_LLD0: {struct Cyc_Absyn_FnType_struct*
_tmp154=(struct Cyc_Absyn_FnType_struct*)_tmp152;if(_tmp154->tag != 10)goto _LLD2;}
_LLD1: return Cyc_PP_nil_doc();_LLD2: {struct Cyc_Absyn_PointerType_struct*_tmp155=(
struct Cyc_Absyn_PointerType_struct*)_tmp152;if(_tmp155->tag != 5)goto _LLD4;}_LLD3:
return Cyc_PP_nil_doc();_LLD4: {struct Cyc_Absyn_VoidType_struct*_tmp156=(struct
Cyc_Absyn_VoidType_struct*)_tmp152;if(_tmp156->tag != 0)goto _LLD6;}_LLD5:{const
char*_tmp710;s=Cyc_PP_text(((_tmp710="void",_tag_dyneither(_tmp710,sizeof(char),
5))));}goto _LLCD;_LLD6: {struct Cyc_Absyn_Evar_struct*_tmp157=(struct Cyc_Absyn_Evar_struct*)
_tmp152;if(_tmp157->tag != 1)goto _LLD8;else{_tmp158=_tmp157->f1;_tmp159=_tmp157->f2;
_tmp15A=_tmp157->f3;_tmp15B=_tmp157->f4;}}_LLD7: if(_tmp159 != 0)return Cyc_Absynpp_ntyp2doc((
void*)_tmp159->v);else{const char*_tmp720;const char*_tmp71F;const char*_tmp71E;
struct Cyc_Int_pa_struct _tmp71D;void*_tmp71C[1];const char*_tmp71B;const char*
_tmp71A;struct Cyc_PP_Doc*_tmp719[6];s=((_tmp719[5]=_tmp158 == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp158->v),((_tmp719[4]=Cyc_PP_text(((
_tmp71A=")::",_tag_dyneither(_tmp71A,sizeof(char),4)))),((_tmp719[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp15B == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp15B->v),((_tmp719[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp71D.tag=1,((
_tmp71D.f1=(unsigned long)_tmp15A,((_tmp71C[0]=& _tmp71D,Cyc_aprintf(((_tmp71B="%d",
_tag_dyneither(_tmp71B,sizeof(char),3))),_tag_dyneither(_tmp71C,sizeof(void*),1))))))))),((
_tmp719[1]=Cyc_PP_text(((_tmp71E="(",_tag_dyneither(_tmp71E,sizeof(char),2)))),((
_tmp719[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp71F="\\%",
_tag_dyneither(_tmp71F,sizeof(char),3)))): Cyc_PP_text(((_tmp720="%",
_tag_dyneither(_tmp720,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp719,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLCD;_LLD8: {struct Cyc_Absyn_VarType_struct*
_tmp15C=(struct Cyc_Absyn_VarType_struct*)_tmp152;if(_tmp15C->tag != 2)goto _LLDA;
else{_tmp15D=_tmp15C->f1;}}_LLD9: s=Cyc_PP_textptr(_tmp15D->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp723;struct Cyc_PP_Doc*_tmp722[3];s=((_tmp722[2]=Cyc_Absynpp_kindbound2doc(
_tmp15D->kind),((_tmp722[1]=Cyc_PP_text(((_tmp723="::",_tag_dyneither(_tmp723,
sizeof(char),3)))),((_tmp722[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp722,sizeof(
struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp15D)){const char*_tmp728;const char*_tmp727;struct Cyc_PP_Doc*_tmp726[3];s=((
_tmp726[2]=Cyc_PP_text(((_tmp727=" */",_tag_dyneither(_tmp727,sizeof(char),4)))),((
_tmp726[1]=s,((_tmp726[0]=Cyc_PP_text(((_tmp728="_ /* ",_tag_dyneither(_tmp728,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp726,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLDA: {struct Cyc_Absyn_DatatypeType_struct*_tmp15E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp152;if(_tmp15E->tag != 3)goto _LLDC;else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15F.datatype_info;
_tmp161=_tmp15F.targs;}}_LLDB:{union Cyc_Absyn_DatatypeInfoU _tmp1AD=_tmp160;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp1AE;struct _tuple0*_tmp1AF;int _tmp1B0;
struct Cyc_Absyn_Datatypedecl**_tmp1B1;struct Cyc_Absyn_Datatypedecl*_tmp1B2;
struct Cyc_Absyn_Datatypedecl _tmp1B3;struct _tuple0*_tmp1B4;int _tmp1B5;_LL109: if((
_tmp1AD.UnknownDatatype).tag != 1)goto _LL10B;_tmp1AE=(struct Cyc_Absyn_UnknownDatatypeInfo)(
_tmp1AD.UnknownDatatype).val;_tmp1AF=_tmp1AE.name;_tmp1B0=_tmp1AE.is_extensible;
_LL10A: _tmp1B4=_tmp1AF;_tmp1B5=_tmp1B0;goto _LL10C;_LL10B: if((_tmp1AD.KnownDatatype).tag
!= 2)goto _LL108;_tmp1B1=(struct Cyc_Absyn_Datatypedecl**)(_tmp1AD.KnownDatatype).val;
_tmp1B2=*_tmp1B1;_tmp1B3=*_tmp1B2;_tmp1B4=_tmp1B3.name;_tmp1B5=_tmp1B3.is_extensible;
_LL10C: {const char*_tmp729;struct Cyc_PP_Doc*_tmp1B6=Cyc_PP_text(((_tmp729="datatype ",
_tag_dyneither(_tmp729,sizeof(char),10))));const char*_tmp72A;struct Cyc_PP_Doc*
_tmp1B7=_tmp1B5?Cyc_PP_text(((_tmp72A="@extensible ",_tag_dyneither(_tmp72A,
sizeof(char),13)))): Cyc_PP_nil_doc();{struct Cyc_PP_Doc*_tmp72B[4];s=((_tmp72B[3]=
Cyc_Absynpp_tps2doc(_tmp161),((_tmp72B[2]=Cyc_Absynpp_qvar2doc(_tmp1B4),((
_tmp72B[1]=_tmp1B6,((_tmp72B[0]=_tmp1B7,Cyc_PP_cat(_tag_dyneither(_tmp72B,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL108;}_LL108:;}goto _LLCD;_LLDC: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp162=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp152;if(_tmp162->tag != 4)goto _LLDE;else{_tmp163=_tmp162->f1;_tmp164=_tmp163.field_info;
_tmp165=_tmp163.targs;}}_LLDD:{union Cyc_Absyn_DatatypeFieldInfoU _tmp1BB=_tmp164;
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp1BC;struct _tuple0*_tmp1BD;struct
_tuple0*_tmp1BE;int _tmp1BF;struct _tuple1 _tmp1C0;struct Cyc_Absyn_Datatypedecl*
_tmp1C1;struct Cyc_Absyn_Datatypedecl _tmp1C2;struct _tuple0*_tmp1C3;int _tmp1C4;
struct Cyc_Absyn_Datatypefield*_tmp1C5;struct Cyc_Absyn_Datatypefield _tmp1C6;
struct _tuple0*_tmp1C7;_LL10E: if((_tmp1BB.UnknownDatatypefield).tag != 1)goto
_LL110;_tmp1BC=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1BB.UnknownDatatypefield).val;
_tmp1BD=_tmp1BC.datatype_name;_tmp1BE=_tmp1BC.field_name;_tmp1BF=_tmp1BC.is_extensible;
_LL10F: _tmp1C3=_tmp1BD;_tmp1C4=_tmp1BF;_tmp1C7=_tmp1BE;goto _LL111;_LL110: if((
_tmp1BB.KnownDatatypefield).tag != 2)goto _LL10D;_tmp1C0=(struct _tuple1)(_tmp1BB.KnownDatatypefield).val;
_tmp1C1=_tmp1C0.f1;_tmp1C2=*_tmp1C1;_tmp1C3=_tmp1C2.name;_tmp1C4=_tmp1C2.is_extensible;
_tmp1C5=_tmp1C0.f2;_tmp1C6=*_tmp1C5;_tmp1C7=_tmp1C6.name;_LL111: {const char*
_tmp72D;const char*_tmp72C;struct Cyc_PP_Doc*_tmp1C8=Cyc_PP_text(_tmp1C4?(_tmp72D="@extensible datatype ",
_tag_dyneither(_tmp72D,sizeof(char),22)):((_tmp72C="datatype ",_tag_dyneither(
_tmp72C,sizeof(char),10))));{const char*_tmp730;struct Cyc_PP_Doc*_tmp72F[4];s=((
_tmp72F[3]=Cyc_Absynpp_qvar2doc(_tmp1C7),((_tmp72F[2]=Cyc_PP_text(((_tmp730=".",
_tag_dyneither(_tmp730,sizeof(char),2)))),((_tmp72F[1]=Cyc_Absynpp_qvar2doc(
_tmp1C3),((_tmp72F[0]=_tmp1C8,Cyc_PP_cat(_tag_dyneither(_tmp72F,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL10D;}_LL10D:;}goto _LLCD;_LLDE: {struct Cyc_Absyn_IntType_struct*
_tmp166=(struct Cyc_Absyn_IntType_struct*)_tmp152;if(_tmp166->tag != 6)goto _LLE0;
else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}_LLDF: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;switch(_tmp167){case Cyc_Absyn_None: _LL112: goto _LL113;case
Cyc_Absyn_Signed: _LL113:{const char*_tmp731;sns=((_tmp731="",_tag_dyneither(
_tmp731,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL114:{const char*
_tmp732;sns=((_tmp732="unsigned ",_tag_dyneither(_tmp732,sizeof(char),10)));}
break;}switch(_tmp168){case Cyc_Absyn_Char_sz: _LL116: switch(_tmp167){case Cyc_Absyn_None:
_LL118:{const char*_tmp733;sns=((_tmp733="",_tag_dyneither(_tmp733,sizeof(char),1)));}
break;case Cyc_Absyn_Signed: _LL119:{const char*_tmp734;sns=((_tmp734="signed ",
_tag_dyneither(_tmp734,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL11A:{
const char*_tmp735;sns=((_tmp735="unsigned ",_tag_dyneither(_tmp735,sizeof(char),
10)));}break;}{const char*_tmp736;ts=((_tmp736="char",_tag_dyneither(_tmp736,
sizeof(char),5)));}break;case Cyc_Absyn_Short_sz: _LL117:{const char*_tmp737;ts=((
_tmp737="short",_tag_dyneither(_tmp737,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz:
_LL11C:{const char*_tmp738;ts=((_tmp738="int",_tag_dyneither(_tmp738,sizeof(char),
4)));}break;case Cyc_Absyn_Long_sz: _LL11D:{const char*_tmp739;ts=((_tmp739="long",
_tag_dyneither(_tmp739,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL11E:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL120:{const char*_tmp73A;
ts=((_tmp73A="long long",_tag_dyneither(_tmp73A,sizeof(char),10)));}break;case
Cyc_Cyclone_Vc_c: _LL121:{const char*_tmp73B;ts=((_tmp73B="__int64",_tag_dyneither(
_tmp73B,sizeof(char),8)));}break;}break;}{const char*_tmp740;void*_tmp73F[2];
struct Cyc_String_pa_struct _tmp73E;struct Cyc_String_pa_struct _tmp73D;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp73D.tag=0,((_tmp73D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ts),((_tmp73E.tag=0,((_tmp73E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sns),((_tmp73F[0]=& _tmp73E,((_tmp73F[1]=& _tmp73D,Cyc_aprintf(((
_tmp740="%s%s",_tag_dyneither(_tmp740,sizeof(char),5))),_tag_dyneither(_tmp73F,
sizeof(void*),2)))))))))))))));}goto _LLCD;}_LLE0: {struct Cyc_Absyn_FloatType_struct*
_tmp169=(struct Cyc_Absyn_FloatType_struct*)_tmp152;if(_tmp169->tag != 7)goto _LLE2;}
_LLE1:{const char*_tmp741;s=Cyc_PP_text(((_tmp741="float",_tag_dyneither(_tmp741,
sizeof(char),6))));}goto _LLCD;_LLE2: {struct Cyc_Absyn_DoubleType_struct*_tmp16A=(
struct Cyc_Absyn_DoubleType_struct*)_tmp152;if(_tmp16A->tag != 8)goto _LLE4;else{
_tmp16B=_tmp16A->f1;}}_LLE3: if(_tmp16B){const char*_tmp742;s=Cyc_PP_text(((
_tmp742="long double",_tag_dyneither(_tmp742,sizeof(char),12))));}else{const char*
_tmp743;s=Cyc_PP_text(((_tmp743="double",_tag_dyneither(_tmp743,sizeof(char),7))));}
goto _LLCD;_LLE4: {struct Cyc_Absyn_TupleType_struct*_tmp16C=(struct Cyc_Absyn_TupleType_struct*)
_tmp152;if(_tmp16C->tag != 11)goto _LLE6;else{_tmp16D=_tmp16C->f1;}}_LLE5:{struct
Cyc_PP_Doc*_tmp744[2];s=((_tmp744[1]=Cyc_Absynpp_args2doc(_tmp16D),((_tmp744[0]=
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp744,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LLCD;_LLE6: {struct Cyc_Absyn_AggrType_struct*_tmp16E=(struct Cyc_Absyn_AggrType_struct*)
_tmp152;if(_tmp16E->tag != 12)goto _LLE8;else{_tmp16F=_tmp16E->f1;_tmp170=_tmp16F.aggr_info;
_tmp171=_tmp16F.targs;}}_LLE7: {enum Cyc_Absyn_AggrKind _tmp1E1;struct _tuple0*
_tmp1E2;struct _tuple9 _tmp1E0=Cyc_Absyn_aggr_kinded_name(_tmp170);_tmp1E1=_tmp1E0.f1;
_tmp1E2=_tmp1E0.f2;{struct Cyc_PP_Doc*_tmp745[3];s=((_tmp745[2]=Cyc_Absynpp_tps2doc(
_tmp171),((_tmp745[1]=Cyc_Absynpp_qvar2doc(_tmp1E2),((_tmp745[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp1E1),Cyc_PP_cat(_tag_dyneither(_tmp745,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;}_LLE8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp172=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp152;if(_tmp172->tag != 13)goto _LLEA;else{_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;}}
_LLE9:{struct Cyc_PP_Doc*_tmp746[4];s=((_tmp746[3]=Cyc_Absynpp_rb(),((_tmp746[2]=
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp174)),((_tmp746[1]=Cyc_Absynpp_lb(),((
_tmp746[0]=Cyc_Absynpp_aggr_kind2doc(_tmp173),Cyc_PP_cat(_tag_dyneither(_tmp746,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEA: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp175=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp152;if(_tmp175->tag != 15)goto
_LLEC;else{_tmp176=_tmp175->f1;}}_LLEB:{const char*_tmp749;struct Cyc_PP_Doc*
_tmp748[4];s=((_tmp748[3]=Cyc_Absynpp_rb(),((_tmp748[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(
_tmp176)),((_tmp748[1]=Cyc_Absynpp_lb(),((_tmp748[0]=Cyc_PP_text(((_tmp749="enum ",
_tag_dyneither(_tmp749,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp748,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEC: {struct Cyc_Absyn_EnumType_struct*
_tmp177=(struct Cyc_Absyn_EnumType_struct*)_tmp152;if(_tmp177->tag != 14)goto _LLEE;
else{_tmp178=_tmp177->f1;}}_LLED:{const char*_tmp74C;struct Cyc_PP_Doc*_tmp74B[2];
s=((_tmp74B[1]=Cyc_Absynpp_qvar2doc(_tmp178),((_tmp74B[0]=Cyc_PP_text(((_tmp74C="enum ",
_tag_dyneither(_tmp74C,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp74B,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCD;_LLEE: {struct Cyc_Absyn_ValueofType_struct*
_tmp179=(struct Cyc_Absyn_ValueofType_struct*)_tmp152;if(_tmp179->tag != 19)goto
_LLF0;else{_tmp17A=_tmp179->f1;}}_LLEF:{const char*_tmp751;const char*_tmp750;
struct Cyc_PP_Doc*_tmp74F[3];s=((_tmp74F[2]=Cyc_PP_text(((_tmp750=")",
_tag_dyneither(_tmp750,sizeof(char),2)))),((_tmp74F[1]=Cyc_Absynpp_exp2doc(
_tmp17A),((_tmp74F[0]=Cyc_PP_text(((_tmp751="valueof_t(",_tag_dyneither(_tmp751,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp74F,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLCD;_LLF0: {struct Cyc_Absyn_TypedefType_struct*_tmp17B=(struct
Cyc_Absyn_TypedefType_struct*)_tmp152;if(_tmp17B->tag != 18)goto _LLF2;else{
_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;_tmp17E=_tmp17B->f3;}}_LLF1:{struct Cyc_PP_Doc*
_tmp752[2];s=((_tmp752[1]=Cyc_Absynpp_tps2doc(_tmp17D),((_tmp752[0]=Cyc_Absynpp_qvar2doc(
_tmp17C),Cyc_PP_cat(_tag_dyneither(_tmp752,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLCD;_LLF2: {struct Cyc_Absyn_RgnHandleType_struct*_tmp17F=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp152;if(_tmp17F->tag != 16)goto _LLF4;else{_tmp180=(void*)_tmp17F->f1;}}_LLF3:{
const char*_tmp757;const char*_tmp756;struct Cyc_PP_Doc*_tmp755[3];s=((_tmp755[2]=
Cyc_PP_text(((_tmp756=">",_tag_dyneither(_tmp756,sizeof(char),2)))),((_tmp755[1]=
Cyc_Absynpp_rgn2doc(_tmp180),((_tmp755[0]=Cyc_PP_text(((_tmp757="region_t<",
_tag_dyneither(_tmp757,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp755,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF4: {struct Cyc_Absyn_DynRgnType_struct*
_tmp181=(struct Cyc_Absyn_DynRgnType_struct*)_tmp152;if(_tmp181->tag != 17)goto
_LLF6;else{_tmp182=(void*)_tmp181->f1;_tmp183=(void*)_tmp181->f2;}}_LLF5:{const
char*_tmp75E;const char*_tmp75D;const char*_tmp75C;struct Cyc_PP_Doc*_tmp75B[5];s=((
_tmp75B[4]=Cyc_PP_text(((_tmp75C=">",_tag_dyneither(_tmp75C,sizeof(char),2)))),((
_tmp75B[3]=Cyc_Absynpp_rgn2doc(_tmp183),((_tmp75B[2]=Cyc_PP_text(((_tmp75D=",",
_tag_dyneither(_tmp75D,sizeof(char),2)))),((_tmp75B[1]=Cyc_Absynpp_rgn2doc(
_tmp182),((_tmp75B[0]=Cyc_PP_text(((_tmp75E="dynregion_t<",_tag_dyneither(
_tmp75E,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp75B,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLCD;_LLF6: {struct Cyc_Absyn_TagType_struct*_tmp184=(struct
Cyc_Absyn_TagType_struct*)_tmp152;if(_tmp184->tag != 20)goto _LLF8;else{_tmp185=(
void*)_tmp184->f1;}}_LLF7:{const char*_tmp763;const char*_tmp762;struct Cyc_PP_Doc*
_tmp761[3];s=((_tmp761[2]=Cyc_PP_text(((_tmp762=">",_tag_dyneither(_tmp762,
sizeof(char),2)))),((_tmp761[1]=Cyc_Absynpp_typ2doc(_tmp185),((_tmp761[0]=Cyc_PP_text(((
_tmp763="tag_t<",_tag_dyneither(_tmp763,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp761,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF8: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp186=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp152;if(_tmp186->tag != 22)goto _LLFA;}_LLF9: goto _LLFB;_LLFA: {struct Cyc_Absyn_HeapRgn_struct*
_tmp187=(struct Cyc_Absyn_HeapRgn_struct*)_tmp152;if(_tmp187->tag != 21)goto _LLFC;}
_LLFB: s=Cyc_Absynpp_rgn2doc(t);goto _LLCD;_LLFC: {struct Cyc_Absyn_RgnsEff_struct*
_tmp188=(struct Cyc_Absyn_RgnsEff_struct*)_tmp152;if(_tmp188->tag != 25)goto _LLFE;
else{_tmp189=(void*)_tmp188->f1;}}_LLFD:{const char*_tmp768;const char*_tmp767;
struct Cyc_PP_Doc*_tmp766[3];s=((_tmp766[2]=Cyc_PP_text(((_tmp767=")",
_tag_dyneither(_tmp767,sizeof(char),2)))),((_tmp766[1]=Cyc_Absynpp_typ2doc(
_tmp189),((_tmp766[0]=Cyc_PP_text(((_tmp768="regions(",_tag_dyneither(_tmp768,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp766,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLFE: {struct Cyc_Absyn_AccessEff_struct*_tmp18A=(struct Cyc_Absyn_AccessEff_struct*)
_tmp152;if(_tmp18A->tag != 23)goto _LL100;}_LLFF: goto _LL101;_LL100: {struct Cyc_Absyn_JoinEff_struct*
_tmp18B=(struct Cyc_Absyn_JoinEff_struct*)_tmp152;if(_tmp18B->tag != 24)goto _LL102;}
_LL101: s=Cyc_Absynpp_eff2doc(t);goto _LLCD;_LL102: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp18C=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp152;if(_tmp18C->tag != 26)goto
_LL104;else{_tmp18D=_tmp18C->f1;_tmp18E=*_tmp18D;_tmp18F=_tmp18E.r;{struct Cyc_Absyn_Aggr_td_struct*
_tmp190=(struct Cyc_Absyn_Aggr_td_struct*)_tmp18F;if(_tmp190->tag != 0)goto _LL104;
else{_tmp191=_tmp190->f1;}};}}_LL103: s=Cyc_Absynpp_aggrdecl2doc(_tmp191);goto
_LLCD;_LL104: {struct Cyc_Absyn_TypeDeclType_struct*_tmp192=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp152;if(_tmp192->tag != 26)goto _LL106;else{_tmp193=_tmp192->f1;_tmp194=*
_tmp193;_tmp195=_tmp194.r;{struct Cyc_Absyn_Enum_td_struct*_tmp196=(struct Cyc_Absyn_Enum_td_struct*)
_tmp195;if(_tmp196->tag != 1)goto _LL106;else{_tmp197=_tmp196->f1;}};}}_LL105: s=
Cyc_Absynpp_enumdecl2doc(_tmp197);goto _LLCD;_LL106: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp198=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp152;if(_tmp198->tag != 26)goto
_LLCD;else{_tmp199=_tmp198->f1;_tmp19A=*_tmp199;_tmp19B=_tmp19A.r;{struct Cyc_Absyn_Datatype_td_struct*
_tmp19C=(struct Cyc_Absyn_Datatype_td_struct*)_tmp19B;if(_tmp19C->tag != 2)goto
_LLCD;else{_tmp19D=_tmp19C->f1;}};}}_LL107: s=Cyc_Absynpp_datatypedecl2doc(
_tmp19D);goto _LLCD;_LLCD:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct
Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct
_dyneither_ptr*)vo->v));}struct _tuple16{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(
struct _tuple16*cmp){struct _tuple16 _tmp1FB;void*_tmp1FC;void*_tmp1FD;struct
_tuple16*_tmp1FA=cmp;_tmp1FB=*_tmp1FA;_tmp1FC=_tmp1FB.f1;_tmp1FD=_tmp1FB.f2;{
const char*_tmp76B;struct Cyc_PP_Doc*_tmp76A[3];return(_tmp76A[2]=Cyc_Absynpp_rgn2doc(
_tmp1FD),((_tmp76A[1]=Cyc_PP_text(((_tmp76B=" > ",_tag_dyneither(_tmp76B,sizeof(
char),4)))),((_tmp76A[0]=Cyc_Absynpp_rgn2doc(_tmp1FC),Cyc_PP_cat(_tag_dyneither(
_tmp76A,sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List*po){const char*_tmp76E;const char*_tmp76D;const char*_tmp76C;
return Cyc_PP_group(((_tmp76C="",_tag_dyneither(_tmp76C,sizeof(char),1))),((
_tmp76D="",_tag_dyneither(_tmp76D,sizeof(char),1))),((_tmp76E=",",_tag_dyneither(
_tmp76E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple7*t){struct Cyc_Core_Opt*
_tmp76F;struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:((_tmp76F=_cycalloc(sizeof(*
_tmp76F)),((_tmp76F->v=Cyc_PP_text(*((struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v)),_tmp76F))));return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp204=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs){const char*_tmp772;struct Cyc_List_List*
_tmp771;_tmp204=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp204,((_tmp771=_cycalloc(sizeof(*_tmp771)),((_tmp771->hd=
Cyc_PP_text(((_tmp772="...",_tag_dyneither(_tmp772,sizeof(char),4)))),((_tmp771->tl=
0,_tmp771)))))));}else{if(cyc_varargs != 0){const char*_tmp77B;const char*_tmp77A;
const char*_tmp779;struct _tuple7*_tmp778;struct Cyc_PP_Doc*_tmp777[3];struct Cyc_PP_Doc*
_tmp207=(_tmp777[2]=Cyc_Absynpp_funarg2doc(((_tmp778=_cycalloc(sizeof(*_tmp778)),((
_tmp778->f1=cyc_varargs->name,((_tmp778->f2=cyc_varargs->tq,((_tmp778->f3=
cyc_varargs->type,_tmp778))))))))),((_tmp777[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp779=" inject ",_tag_dyneither(_tmp779,sizeof(char),9)))): Cyc_PP_text(((
_tmp77A=" ",_tag_dyneither(_tmp77A,sizeof(char),2)))),((_tmp777[0]=Cyc_PP_text(((
_tmp77B="...",_tag_dyneither(_tmp77B,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp777,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp77C;_tmp204=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp204,((_tmp77C=_cycalloc(sizeof(*_tmp77C)),((_tmp77C->hd=
_tmp207,((_tmp77C->tl=0,_tmp77C)))))));}}{const char*_tmp77F;const char*_tmp77E;
const char*_tmp77D;struct Cyc_PP_Doc*_tmp20E=Cyc_PP_group(((_tmp77D="",
_tag_dyneither(_tmp77D,sizeof(char),1))),((_tmp77E="",_tag_dyneither(_tmp77E,
sizeof(char),1))),((_tmp77F=",",_tag_dyneither(_tmp77F,sizeof(char),2))),_tmp204);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp782;struct Cyc_PP_Doc*
_tmp781[3];_tmp20E=((_tmp781[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp781[
1]=Cyc_PP_text(((_tmp782=";",_tag_dyneither(_tmp782,sizeof(char),2)))),((_tmp781[
0]=_tmp20E,Cyc_PP_cat(_tag_dyneither(_tmp781,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp785;struct Cyc_PP_Doc*_tmp784[3];_tmp20E=((_tmp784[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp784[1]=Cyc_PP_text(((_tmp785=":",
_tag_dyneither(_tmp785,sizeof(char),2)))),((_tmp784[0]=_tmp20E,Cyc_PP_cat(
_tag_dyneither(_tmp784,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp78A;
const char*_tmp789;struct Cyc_PP_Doc*_tmp788[3];return(_tmp788[2]=Cyc_PP_text(((
_tmp789=")",_tag_dyneither(_tmp789,sizeof(char),2)))),((_tmp788[1]=_tmp20E,((
_tmp788[0]=Cyc_PP_text(((_tmp78A="(",_tag_dyneither(_tmp78A,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp788,sizeof(struct Cyc_PP_Doc*),3)))))));};};}struct
_tuple7*Cyc_Absynpp_arg_mk_opt(struct _tuple11*arg){struct Cyc_Core_Opt*_tmp78D;
struct _tuple7*_tmp78C;return(_tmp78C=_cycalloc(sizeof(*_tmp78C)),((_tmp78C->f1=((
_tmp78D=_cycalloc(sizeof(*_tmp78D)),((_tmp78D->v=(*arg).f1,_tmp78D)))),((_tmp78C->f2=(*
arg).f2,((_tmp78C->f3=(*arg).f3,_tmp78C)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(
struct _dyneither_ptr*v){return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*q){struct Cyc_List_List*_tmp21B=0;int match;{union Cyc_Absyn_Nmspace
_tmp21C=(*q).f1;int _tmp21D;struct Cyc_List_List*_tmp21E;struct Cyc_List_List*
_tmp21F;struct Cyc_List_List*_tmp220;_LL124: if((_tmp21C.Loc_n).tag != 4)goto _LL126;
_tmp21D=(int)(_tmp21C.Loc_n).val;_LL125: _tmp21E=0;goto _LL127;_LL126: if((_tmp21C.Rel_n).tag
!= 1)goto _LL128;_tmp21E=(struct Cyc_List_List*)(_tmp21C.Rel_n).val;_LL127: match=0;
_tmp21B=_tmp21E;goto _LL123;_LL128: if((_tmp21C.C_n).tag != 3)goto _LL12A;_tmp21F=(
struct Cyc_List_List*)(_tmp21C.C_n).val;_LL129: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp21F,Cyc_Absynpp_curr_namespace);
goto _LL123;_LL12A: if((_tmp21C.Abs_n).tag != 2)goto _LL123;_tmp220=(struct Cyc_List_List*)(
_tmp21C.Abs_n).val;_LL12B: match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp220,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp78E;_tmp21B=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp78E=_cycalloc(sizeof(*_tmp78E)),((_tmp78E->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp78E->tl=_tmp220,_tmp78E))))): _tmp220;}goto _LL123;_LL123:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp790;struct Cyc_List_List*_tmp78F;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp21B,((_tmp78F=_cycalloc(sizeof(*_tmp78F)),((_tmp78F->hd=(*q).f2,((_tmp78F->tl=
0,_tmp78F))))))),((_tmp790="_",_tag_dyneither(_tmp790,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp792;struct Cyc_List_List*_tmp791;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp21B,((_tmp791=_cycalloc(sizeof(*
_tmp791)),((_tmp791->hd=(*q).f2,((_tmp791->tl=0,_tmp791))))))),((_tmp792="::",
_tag_dyneither(_tmp792,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*
Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct _dyneither_ptr _tmp226=Cyc_Absynpp_qvar2string(
q);if(Cyc_PP_tex_output){const char*_tmp794;const char*_tmp793;return Cyc_PP_text_width((
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp793="\\textbf{",
_tag_dyneither(_tmp793,sizeof(char),9))),(struct _dyneither_ptr)_tmp226),((
_tmp794="}",_tag_dyneither(_tmp794,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp226));}else{return Cyc_PP_text(_tmp226);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)
return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace
_tmp229=(*v).f1;int _tmp22A;struct Cyc_List_List*_tmp22B;struct Cyc_List_List*
_tmp22C;struct Cyc_List_List*_tmp22D;_LL12D: if((_tmp229.Loc_n).tag != 4)goto _LL12F;
_tmp22A=(int)(_tmp229.Loc_n).val;_LL12E: goto _LL130;_LL12F: if((_tmp229.Rel_n).tag
!= 1)goto _LL131;_tmp22B=(struct Cyc_List_List*)(_tmp229.Rel_n).val;if(_tmp22B != 0)
goto _LL131;_LL130: return*(*v).f2;_LL131: if((_tmp229.C_n).tag != 3)goto _LL133;
_tmp22C=(struct Cyc_List_List*)(_tmp229.C_n).val;_LL132: _tmp22D=_tmp22C;goto
_LL134;_LL133: if((_tmp229.Abs_n).tag != 2)goto _LL135;_tmp22D=(struct Cyc_List_List*)(
_tmp229.Abs_n).val;_LL134: if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_strptrcmp,_tmp22D,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto
_LL136;}_LL135:;_LL136: {const char*_tmp795;return(struct _dyneither_ptr)Cyc_strconcat(((
_tmp795="/* bad namespace : */ ",_tag_dyneither(_tmp795,sizeof(char),23))),(
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL12C:;}else{return*(*v).f2;}}
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){return Cyc_PP_text(
Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v){struct _dyneither_ptr _tmp22F=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){const char*_tmp797;const char*_tmp796;return Cyc_PP_text_width((
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp796="\\textbf{",
_tag_dyneither(_tmp796,sizeof(char),9))),(struct _dyneither_ptr)_tmp22F),((
_tmp797="}",_tag_dyneither(_tmp797,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp22F));}else{return Cyc_PP_text(_tmp22F);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmp232=e->r;enum Cyc_Absyn_Primop _tmp237;struct Cyc_Absyn_Exp*
_tmp242;struct Cyc_Absyn_Exp*_tmp244;_LL138: {struct Cyc_Absyn_Const_e_struct*
_tmp233=(struct Cyc_Absyn_Const_e_struct*)_tmp232;if(_tmp233->tag != 0)goto _LL13A;}
_LL139: goto _LL13B;_LL13A: {struct Cyc_Absyn_Var_e_struct*_tmp234=(struct Cyc_Absyn_Var_e_struct*)
_tmp232;if(_tmp234->tag != 1)goto _LL13C;}_LL13B: goto _LL13D;_LL13C: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp235=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp232;if(_tmp235->tag != 2)goto
_LL13E;}_LL13D: return 10000;_LL13E: {struct Cyc_Absyn_Primop_e_struct*_tmp236=(
struct Cyc_Absyn_Primop_e_struct*)_tmp232;if(_tmp236->tag != 3)goto _LL140;else{
_tmp237=_tmp236->f1;}}_LL13F: switch(_tmp237){case Cyc_Absyn_Plus: _LL18A: return 100;
case Cyc_Absyn_Times: _LL18B: return 110;case Cyc_Absyn_Minus: _LL18C: return 100;case
Cyc_Absyn_Div: _LL18D: goto _LL18E;case Cyc_Absyn_Mod: _LL18E: return 110;case Cyc_Absyn_Eq:
_LL18F: goto _LL190;case Cyc_Absyn_Neq: _LL190: return 70;case Cyc_Absyn_Gt: _LL191: goto
_LL192;case Cyc_Absyn_Lt: _LL192: goto _LL193;case Cyc_Absyn_Gte: _LL193: goto _LL194;
case Cyc_Absyn_Lte: _LL194: return 80;case Cyc_Absyn_Not: _LL195: goto _LL196;case Cyc_Absyn_Bitnot:
_LL196: return 130;case Cyc_Absyn_Bitand: _LL197: return 60;case Cyc_Absyn_Bitor: _LL198:
return 40;case Cyc_Absyn_Bitxor: _LL199: return 50;case Cyc_Absyn_Bitlshift: _LL19A:
return 90;case Cyc_Absyn_Bitlrshift: _LL19B: return 80;case Cyc_Absyn_Bitarshift:
_LL19C: return 80;case Cyc_Absyn_Numelts: _LL19D: return 140;}_LL140: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp238=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp232;if(_tmp238->tag != 4)goto
_LL142;}_LL141: return 20;_LL142: {struct Cyc_Absyn_Increment_e_struct*_tmp239=(
struct Cyc_Absyn_Increment_e_struct*)_tmp232;if(_tmp239->tag != 5)goto _LL144;}
_LL143: return 130;_LL144: {struct Cyc_Absyn_Conditional_e_struct*_tmp23A=(struct
Cyc_Absyn_Conditional_e_struct*)_tmp232;if(_tmp23A->tag != 6)goto _LL146;}_LL145:
return 30;_LL146: {struct Cyc_Absyn_And_e_struct*_tmp23B=(struct Cyc_Absyn_And_e_struct*)
_tmp232;if(_tmp23B->tag != 7)goto _LL148;}_LL147: return 35;_LL148: {struct Cyc_Absyn_Or_e_struct*
_tmp23C=(struct Cyc_Absyn_Or_e_struct*)_tmp232;if(_tmp23C->tag != 8)goto _LL14A;}
_LL149: return 30;_LL14A: {struct Cyc_Absyn_SeqExp_e_struct*_tmp23D=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp232;if(_tmp23D->tag != 9)goto _LL14C;}_LL14B: return 10;_LL14C: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp23E=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp232;if(_tmp23E->tag != 10)goto
_LL14E;}_LL14D: goto _LL14F;_LL14E: {struct Cyc_Absyn_FnCall_e_struct*_tmp23F=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp232;if(_tmp23F->tag != 11)goto _LL150;}_LL14F:
return 140;_LL150: {struct Cyc_Absyn_Throw_e_struct*_tmp240=(struct Cyc_Absyn_Throw_e_struct*)
_tmp232;if(_tmp240->tag != 12)goto _LL152;}_LL151: return 130;_LL152: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp241=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp232;if(_tmp241->tag != 13)
goto _LL154;else{_tmp242=_tmp241->f1;}}_LL153: return Cyc_Absynpp_exp_prec(_tmp242);
_LL154: {struct Cyc_Absyn_Instantiate_e_struct*_tmp243=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp232;if(_tmp243->tag != 14)goto _LL156;else{_tmp244=_tmp243->f1;}}_LL155: return
Cyc_Absynpp_exp_prec(_tmp244);_LL156: {struct Cyc_Absyn_Cast_e_struct*_tmp245=(
struct Cyc_Absyn_Cast_e_struct*)_tmp232;if(_tmp245->tag != 15)goto _LL158;}_LL157:
return 120;_LL158: {struct Cyc_Absyn_New_e_struct*_tmp246=(struct Cyc_Absyn_New_e_struct*)
_tmp232;if(_tmp246->tag != 17)goto _LL15A;}_LL159: return 15;_LL15A: {struct Cyc_Absyn_Address_e_struct*
_tmp247=(struct Cyc_Absyn_Address_e_struct*)_tmp232;if(_tmp247->tag != 16)goto
_LL15C;}_LL15B: goto _LL15D;_LL15C: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp248=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp232;if(_tmp248->tag != 18)goto _LL15E;}
_LL15D: goto _LL15F;_LL15E: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp249=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp232;if(_tmp249->tag != 19)goto _LL160;}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_Valueof_e_struct*
_tmp24A=(struct Cyc_Absyn_Valueof_e_struct*)_tmp232;if(_tmp24A->tag != 39)goto
_LL162;}_LL161: goto _LL163;_LL162: {struct Cyc_Absyn_Asm_e_struct*_tmp24B=(struct
Cyc_Absyn_Asm_e_struct*)_tmp232;if(_tmp24B->tag != 40)goto _LL164;}_LL163: goto
_LL165;_LL164: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp24C=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp232;if(_tmp24C->tag != 38)goto _LL166;}_LL165: goto _LL167;_LL166: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp24D=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp232;if(_tmp24D->tag != 20)goto
_LL168;}_LL167: goto _LL169;_LL168: {struct Cyc_Absyn_Deref_e_struct*_tmp24E=(
struct Cyc_Absyn_Deref_e_struct*)_tmp232;if(_tmp24E->tag != 21)goto _LL16A;}_LL169:
return 130;_LL16A: {struct Cyc_Absyn_AggrMember_e_struct*_tmp24F=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp232;if(_tmp24F->tag != 22)goto _LL16C;}_LL16B: goto _LL16D;_LL16C: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp250=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp232;if(_tmp250->tag != 23)goto
_LL16E;}_LL16D: goto _LL16F;_LL16E: {struct Cyc_Absyn_Subscript_e_struct*_tmp251=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp232;if(_tmp251->tag != 24)goto _LL170;}
_LL16F: return 140;_LL170: {struct Cyc_Absyn_Tuple_e_struct*_tmp252=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp232;if(_tmp252->tag != 25)goto _LL172;}_LL171: return 150;_LL172: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp253=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp232;if(_tmp253->tag != 26)goto
_LL174;}_LL173: goto _LL175;_LL174: {struct Cyc_Absyn_Array_e_struct*_tmp254=(
struct Cyc_Absyn_Array_e_struct*)_tmp232;if(_tmp254->tag != 27)goto _LL176;}_LL175:
goto _LL177;_LL176: {struct Cyc_Absyn_Comprehension_e_struct*_tmp255=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp232;if(_tmp255->tag != 28)goto _LL178;}_LL177: goto _LL179;_LL178: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp256=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp232;if(_tmp256->tag != 29)goto
_LL17A;}_LL179: goto _LL17B;_LL17A: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp257=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp232;if(_tmp257->tag != 30)goto _LL17C;}
_LL17B: goto _LL17D;_LL17C: {struct Cyc_Absyn_Datatype_e_struct*_tmp258=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp232;if(_tmp258->tag != 31)goto _LL17E;}_LL17D: goto _LL17F;_LL17E: {struct Cyc_Absyn_Enum_e_struct*
_tmp259=(struct Cyc_Absyn_Enum_e_struct*)_tmp232;if(_tmp259->tag != 32)goto _LL180;}
_LL17F: goto _LL181;_LL180: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp25A=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp232;if(_tmp25A->tag != 33)goto _LL182;}_LL181: goto _LL183;_LL182: {struct Cyc_Absyn_Malloc_e_struct*
_tmp25B=(struct Cyc_Absyn_Malloc_e_struct*)_tmp232;if(_tmp25B->tag != 34)goto
_LL184;}_LL183: goto _LL185;_LL184: {struct Cyc_Absyn_Swap_e_struct*_tmp25C=(struct
Cyc_Absyn_Swap_e_struct*)_tmp232;if(_tmp25C->tag != 35)goto _LL186;}_LL185: goto
_LL187;_LL186: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp25D=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp232;if(_tmp25D->tag != 36)goto _LL188;}_LL187: return 140;_LL188: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp25E=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp232;if(_tmp25E->tag != 37)goto
_LL137;}_LL189: return 10000;_LL137:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct
Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp25F=e->r;union Cyc_Absyn_Cnst _tmp261;struct _tuple0*_tmp263;struct
_tuple0*_tmp265;enum Cyc_Absyn_Primop _tmp267;struct Cyc_List_List*_tmp268;struct
Cyc_Absyn_Exp*_tmp26A;struct Cyc_Core_Opt*_tmp26B;struct Cyc_Absyn_Exp*_tmp26C;
struct Cyc_Absyn_Exp*_tmp26E;enum Cyc_Absyn_Incrementor _tmp26F;struct Cyc_Absyn_Exp*
_tmp271;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Absyn_Exp*_tmp273;struct Cyc_Absyn_Exp*
_tmp275;struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp*_tmp278;struct Cyc_Absyn_Exp*
_tmp279;struct Cyc_Absyn_Exp*_tmp27B;struct Cyc_Absyn_Exp*_tmp27C;struct Cyc_Absyn_Exp*
_tmp27E;struct Cyc_List_List*_tmp27F;struct Cyc_Absyn_Exp*_tmp281;struct Cyc_List_List*
_tmp282;struct Cyc_Absyn_Exp*_tmp284;struct Cyc_Absyn_Exp*_tmp286;struct Cyc_Absyn_Exp*
_tmp288;void*_tmp28A;struct Cyc_Absyn_Exp*_tmp28B;struct Cyc_Absyn_Exp*_tmp28D;
struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_Absyn_Exp*_tmp290;void*_tmp292;struct Cyc_Absyn_Exp*
_tmp294;void*_tmp296;int _tmp298;struct _dyneither_ptr _tmp299;struct Cyc_Absyn_Exp*
_tmp29B;struct _dyneither_ptr*_tmp29C;void*_tmp29E;void*_tmp29F;struct
_dyneither_ptr*_tmp2A1;void*_tmp2A3;void*_tmp2A4;unsigned int _tmp2A6;struct Cyc_Absyn_Exp*
_tmp2A8;struct Cyc_Absyn_Exp*_tmp2AA;struct _dyneither_ptr*_tmp2AB;struct Cyc_Absyn_Exp*
_tmp2AD;struct _dyneither_ptr*_tmp2AE;struct Cyc_Absyn_Exp*_tmp2B0;struct Cyc_Absyn_Exp*
_tmp2B1;struct Cyc_List_List*_tmp2B3;struct _tuple7*_tmp2B5;struct Cyc_List_List*
_tmp2B6;struct Cyc_List_List*_tmp2B8;struct Cyc_Absyn_Vardecl*_tmp2BA;struct Cyc_Absyn_Exp*
_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BC;struct _tuple0*_tmp2BE;struct Cyc_List_List*
_tmp2BF;struct Cyc_List_List*_tmp2C0;struct Cyc_List_List*_tmp2C2;struct Cyc_List_List*
_tmp2C4;struct Cyc_Absyn_Datatypefield*_tmp2C5;struct _tuple0*_tmp2C7;struct
_tuple0*_tmp2C9;struct Cyc_Absyn_MallocInfo _tmp2CB;int _tmp2CC;struct Cyc_Absyn_Exp*
_tmp2CD;void**_tmp2CE;struct Cyc_Absyn_Exp*_tmp2CF;struct Cyc_Absyn_Exp*_tmp2D1;
struct Cyc_Absyn_Exp*_tmp2D2;struct Cyc_Core_Opt*_tmp2D4;struct Cyc_List_List*
_tmp2D5;struct Cyc_Absyn_Stmt*_tmp2D7;_LL1A0: {struct Cyc_Absyn_Const_e_struct*
_tmp260=(struct Cyc_Absyn_Const_e_struct*)_tmp25F;if(_tmp260->tag != 0)goto _LL1A2;
else{_tmp261=_tmp260->f1;}}_LL1A1: s=Cyc_Absynpp_cnst2doc(_tmp261);goto _LL19F;
_LL1A2: {struct Cyc_Absyn_Var_e_struct*_tmp262=(struct Cyc_Absyn_Var_e_struct*)
_tmp25F;if(_tmp262->tag != 1)goto _LL1A4;else{_tmp263=_tmp262->f1;}}_LL1A3: _tmp265=
_tmp263;goto _LL1A5;_LL1A4: {struct Cyc_Absyn_UnknownId_e_struct*_tmp264=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp25F;if(_tmp264->tag != 2)goto _LL1A6;else{
_tmp265=_tmp264->f1;}}_LL1A5: s=Cyc_Absynpp_qvar2doc(_tmp265);goto _LL19F;_LL1A6: {
struct Cyc_Absyn_Primop_e_struct*_tmp266=(struct Cyc_Absyn_Primop_e_struct*)
_tmp25F;if(_tmp266->tag != 3)goto _LL1A8;else{_tmp267=_tmp266->f1;_tmp268=_tmp266->f2;}}
_LL1A7: s=Cyc_Absynpp_primapp2doc(myprec,_tmp267,_tmp268);goto _LL19F;_LL1A8: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp269=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp25F;if(_tmp269->tag != 4)goto _LL1AA;else{_tmp26A=_tmp269->f1;_tmp26B=_tmp269->f2;
_tmp26C=_tmp269->f3;}}_LL1A9:{const char*_tmp79C;const char*_tmp79B;struct Cyc_PP_Doc*
_tmp79A[5];s=((_tmp79A[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26C),((_tmp79A[3]=
Cyc_PP_text(((_tmp79B="= ",_tag_dyneither(_tmp79B,sizeof(char),3)))),((_tmp79A[2]=
_tmp26B == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp26B->v),((
_tmp79A[1]=Cyc_PP_text(((_tmp79C=" ",_tag_dyneither(_tmp79C,sizeof(char),2)))),((
_tmp79A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26A),Cyc_PP_cat(_tag_dyneither(
_tmp79A,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL19F;_LL1AA: {struct Cyc_Absyn_Increment_e_struct*
_tmp26D=(struct Cyc_Absyn_Increment_e_struct*)_tmp25F;if(_tmp26D->tag != 5)goto
_LL1AC;else{_tmp26E=_tmp26D->f1;_tmp26F=_tmp26D->f2;}}_LL1AB: {struct Cyc_PP_Doc*
_tmp2DB=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26E);switch(_tmp26F){case Cyc_Absyn_PreInc:
_LL1F4:{const char*_tmp79F;struct Cyc_PP_Doc*_tmp79E[2];s=((_tmp79E[1]=_tmp2DB,((
_tmp79E[0]=Cyc_PP_text(((_tmp79F="++",_tag_dyneither(_tmp79F,sizeof(char),3)))),
Cyc_PP_cat(_tag_dyneither(_tmp79E,sizeof(struct Cyc_PP_Doc*),2))))));}break;case
Cyc_Absyn_PreDec: _LL1F5:{const char*_tmp7A2;struct Cyc_PP_Doc*_tmp7A1[2];s=((
_tmp7A1[1]=_tmp2DB,((_tmp7A1[0]=Cyc_PP_text(((_tmp7A2="--",_tag_dyneither(
_tmp7A2,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7A1,sizeof(struct Cyc_PP_Doc*),
2))))));}break;case Cyc_Absyn_PostInc: _LL1F6:{const char*_tmp7A5;struct Cyc_PP_Doc*
_tmp7A4[2];s=((_tmp7A4[1]=Cyc_PP_text(((_tmp7A5="++",_tag_dyneither(_tmp7A5,
sizeof(char),3)))),((_tmp7A4[0]=_tmp2DB,Cyc_PP_cat(_tag_dyneither(_tmp7A4,
sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1F7:{const char*
_tmp7A8;struct Cyc_PP_Doc*_tmp7A7[2];s=((_tmp7A7[1]=Cyc_PP_text(((_tmp7A8="--",
_tag_dyneither(_tmp7A8,sizeof(char),3)))),((_tmp7A7[0]=_tmp2DB,Cyc_PP_cat(
_tag_dyneither(_tmp7A7,sizeof(struct Cyc_PP_Doc*),2))))));}break;}goto _LL19F;}
_LL1AC: {struct Cyc_Absyn_Conditional_e_struct*_tmp270=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp25F;if(_tmp270->tag != 6)goto _LL1AE;else{_tmp271=_tmp270->f1;_tmp272=_tmp270->f2;
_tmp273=_tmp270->f3;}}_LL1AD:{const char*_tmp7AD;const char*_tmp7AC;struct Cyc_PP_Doc*
_tmp7AB[5];s=((_tmp7AB[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp273),((_tmp7AB[3]=
Cyc_PP_text(((_tmp7AC=" : ",_tag_dyneither(_tmp7AC,sizeof(char),4)))),((_tmp7AB[
2]=Cyc_Absynpp_exp2doc_prec(0,_tmp272),((_tmp7AB[1]=Cyc_PP_text(((_tmp7AD=" ? ",
_tag_dyneither(_tmp7AD,sizeof(char),4)))),((_tmp7AB[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp271),Cyc_PP_cat(_tag_dyneither(_tmp7AB,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1AE: {struct Cyc_Absyn_And_e_struct*_tmp274=(struct Cyc_Absyn_And_e_struct*)
_tmp25F;if(_tmp274->tag != 7)goto _LL1B0;else{_tmp275=_tmp274->f1;_tmp276=_tmp274->f2;}}
_LL1AF:{const char*_tmp7B0;struct Cyc_PP_Doc*_tmp7AF[3];s=((_tmp7AF[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp276),((_tmp7AF[1]=Cyc_PP_text(((_tmp7B0=" && ",_tag_dyneither(_tmp7B0,
sizeof(char),5)))),((_tmp7AF[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp275),Cyc_PP_cat(
_tag_dyneither(_tmp7AF,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1B0: {
struct Cyc_Absyn_Or_e_struct*_tmp277=(struct Cyc_Absyn_Or_e_struct*)_tmp25F;if(
_tmp277->tag != 8)goto _LL1B2;else{_tmp278=_tmp277->f1;_tmp279=_tmp277->f2;}}
_LL1B1:{const char*_tmp7B3;struct Cyc_PP_Doc*_tmp7B2[3];s=((_tmp7B2[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp279),((_tmp7B2[1]=Cyc_PP_text(((_tmp7B3=" || ",_tag_dyneither(_tmp7B3,
sizeof(char),5)))),((_tmp7B2[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp278),Cyc_PP_cat(
_tag_dyneither(_tmp7B2,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1B2: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp27A=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp25F;if(_tmp27A->tag != 9)goto _LL1B4;else{_tmp27B=_tmp27A->f1;_tmp27C=_tmp27A->f2;}}
_LL1B3:{const char*_tmp7BA;const char*_tmp7B9;const char*_tmp7B8;struct Cyc_PP_Doc*
_tmp7B7[5];s=((_tmp7B7[4]=Cyc_PP_text(((_tmp7B8=")",_tag_dyneither(_tmp7B8,
sizeof(char),2)))),((_tmp7B7[3]=Cyc_Absynpp_exp2doc(_tmp27C),((_tmp7B7[2]=Cyc_PP_text(((
_tmp7B9=", ",_tag_dyneither(_tmp7B9,sizeof(char),3)))),((_tmp7B7[1]=Cyc_Absynpp_exp2doc(
_tmp27B),((_tmp7B7[0]=Cyc_PP_text(((_tmp7BA="(",_tag_dyneither(_tmp7BA,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7B7,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1B4: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp27D=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp25F;if(_tmp27D->tag != 10)goto _LL1B6;else{_tmp27E=_tmp27D->f1;_tmp27F=_tmp27D->f2;}}
_LL1B5:{const char*_tmp7BF;const char*_tmp7BE;struct Cyc_PP_Doc*_tmp7BD[4];s=((
_tmp7BD[3]=Cyc_PP_text(((_tmp7BE=")",_tag_dyneither(_tmp7BE,sizeof(char),2)))),((
_tmp7BD[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp27F),((_tmp7BD[1]=Cyc_PP_text(((
_tmp7BF="(",_tag_dyneither(_tmp7BF,sizeof(char),2)))),((_tmp7BD[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp27E),Cyc_PP_cat(_tag_dyneither(_tmp7BD,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL19F;_LL1B6: {struct Cyc_Absyn_FnCall_e_struct*_tmp280=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp25F;if(_tmp280->tag != 11)goto _LL1B8;else{_tmp281=_tmp280->f1;_tmp282=_tmp280->f2;}}
_LL1B7:{const char*_tmp7C4;const char*_tmp7C3;struct Cyc_PP_Doc*_tmp7C2[4];s=((
_tmp7C2[3]=Cyc_PP_text(((_tmp7C3=")",_tag_dyneither(_tmp7C3,sizeof(char),2)))),((
_tmp7C2[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp282),((_tmp7C2[1]=Cyc_PP_text(((
_tmp7C4="(",_tag_dyneither(_tmp7C4,sizeof(char),2)))),((_tmp7C2[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp281),Cyc_PP_cat(_tag_dyneither(_tmp7C2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL19F;_LL1B8: {struct Cyc_Absyn_Throw_e_struct*_tmp283=(struct Cyc_Absyn_Throw_e_struct*)
_tmp25F;if(_tmp283->tag != 12)goto _LL1BA;else{_tmp284=_tmp283->f1;}}_LL1B9:{const
char*_tmp7C7;struct Cyc_PP_Doc*_tmp7C6[2];s=((_tmp7C6[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp284),((_tmp7C6[0]=Cyc_PP_text(((_tmp7C7="throw ",_tag_dyneither(
_tmp7C7,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp7C6,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL19F;_LL1BA: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp285=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp25F;if(_tmp285->tag != 13)goto _LL1BC;
else{_tmp286=_tmp285->f1;}}_LL1BB: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp286);
goto _LL19F;_LL1BC: {struct Cyc_Absyn_Instantiate_e_struct*_tmp287=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp25F;if(_tmp287->tag != 14)goto _LL1BE;else{_tmp288=_tmp287->f1;}}_LL1BD: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmp288);goto _LL19F;_LL1BE: {struct Cyc_Absyn_Cast_e_struct*_tmp289=(
struct Cyc_Absyn_Cast_e_struct*)_tmp25F;if(_tmp289->tag != 15)goto _LL1C0;else{
_tmp28A=(void*)_tmp289->f1;_tmp28B=_tmp289->f2;}}_LL1BF:{const char*_tmp7CC;const
char*_tmp7CB;struct Cyc_PP_Doc*_tmp7CA[4];s=((_tmp7CA[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp28B),((_tmp7CA[2]=Cyc_PP_text(((_tmp7CB=")",_tag_dyneither(_tmp7CB,
sizeof(char),2)))),((_tmp7CA[1]=Cyc_Absynpp_typ2doc(_tmp28A),((_tmp7CA[0]=Cyc_PP_text(((
_tmp7CC="(",_tag_dyneither(_tmp7CC,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp7CA,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL19F;_LL1C0: {struct Cyc_Absyn_Address_e_struct*
_tmp28C=(struct Cyc_Absyn_Address_e_struct*)_tmp25F;if(_tmp28C->tag != 16)goto
_LL1C2;else{_tmp28D=_tmp28C->f1;}}_LL1C1:{const char*_tmp7CF;struct Cyc_PP_Doc*
_tmp7CE[2];s=((_tmp7CE[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28D),((_tmp7CE[0]=
Cyc_PP_text(((_tmp7CF="&",_tag_dyneither(_tmp7CF,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp7CE,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL19F;_LL1C2: {
struct Cyc_Absyn_New_e_struct*_tmp28E=(struct Cyc_Absyn_New_e_struct*)_tmp25F;if(
_tmp28E->tag != 17)goto _LL1C4;else{_tmp28F=_tmp28E->f1;_tmp290=_tmp28E->f2;}}
_LL1C3: if(_tmp28F == 0){const char*_tmp7D2;struct Cyc_PP_Doc*_tmp7D1[2];s=((_tmp7D1[
1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp290),((_tmp7D1[0]=Cyc_PP_text(((_tmp7D2="new ",
_tag_dyneither(_tmp7D2,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7D1,
sizeof(struct Cyc_PP_Doc*),2))))));}else{const char*_tmp7D7;const char*_tmp7D6;
struct Cyc_PP_Doc*_tmp7D5[4];s=((_tmp7D5[3]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp290),((_tmp7D5[2]=Cyc_PP_text(((_tmp7D6=") ",_tag_dyneither(_tmp7D6,sizeof(
char),3)))),((_tmp7D5[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp28F),((
_tmp7D5[0]=Cyc_PP_text(((_tmp7D7="rnew(",_tag_dyneither(_tmp7D7,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp7D5,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL19F;_LL1C4: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp291=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp25F;if(_tmp291->tag != 18)goto _LL1C6;else{_tmp292=(void*)_tmp291->f1;}}_LL1C5:{
const char*_tmp7DC;const char*_tmp7DB;struct Cyc_PP_Doc*_tmp7DA[3];s=((_tmp7DA[2]=
Cyc_PP_text(((_tmp7DB=")",_tag_dyneither(_tmp7DB,sizeof(char),2)))),((_tmp7DA[1]=
Cyc_Absynpp_typ2doc(_tmp292),((_tmp7DA[0]=Cyc_PP_text(((_tmp7DC="sizeof(",
_tag_dyneither(_tmp7DC,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7DA,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C6: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp293=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp25F;if(_tmp293->tag != 19)goto
_LL1C8;else{_tmp294=_tmp293->f1;}}_LL1C7:{const char*_tmp7E1;const char*_tmp7E0;
struct Cyc_PP_Doc*_tmp7DF[3];s=((_tmp7DF[2]=Cyc_PP_text(((_tmp7E0=")",
_tag_dyneither(_tmp7E0,sizeof(char),2)))),((_tmp7DF[1]=Cyc_Absynpp_exp2doc(
_tmp294),((_tmp7DF[0]=Cyc_PP_text(((_tmp7E1="sizeof(",_tag_dyneither(_tmp7E1,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7DF,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1C8: {struct Cyc_Absyn_Valueof_e_struct*_tmp295=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp25F;if(_tmp295->tag != 39)goto _LL1CA;else{_tmp296=(void*)_tmp295->f1;}}_LL1C9:{
const char*_tmp7E6;const char*_tmp7E5;struct Cyc_PP_Doc*_tmp7E4[3];s=((_tmp7E4[2]=
Cyc_PP_text(((_tmp7E5=")",_tag_dyneither(_tmp7E5,sizeof(char),2)))),((_tmp7E4[1]=
Cyc_Absynpp_typ2doc(_tmp296),((_tmp7E4[0]=Cyc_PP_text(((_tmp7E6="valueof(",
_tag_dyneither(_tmp7E6,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7E4,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1CA: {struct Cyc_Absyn_Asm_e_struct*
_tmp297=(struct Cyc_Absyn_Asm_e_struct*)_tmp25F;if(_tmp297->tag != 40)goto _LL1CC;
else{_tmp298=_tmp297->f1;_tmp299=_tmp297->f2;}}_LL1CB: if(_tmp298){const char*
_tmp7ED;const char*_tmp7EC;const char*_tmp7EB;struct Cyc_PP_Doc*_tmp7EA[4];s=((
_tmp7EA[3]=Cyc_PP_text(((_tmp7EB=")",_tag_dyneither(_tmp7EB,sizeof(char),2)))),((
_tmp7EA[2]=Cyc_PP_text(_tmp299),((_tmp7EA[1]=Cyc_PP_text(((_tmp7EC=" volatile (",
_tag_dyneither(_tmp7EC,sizeof(char),12)))),((_tmp7EA[0]=Cyc_PP_text(((_tmp7ED="__asm__",
_tag_dyneither(_tmp7ED,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7EA,
sizeof(struct Cyc_PP_Doc*),4))))))))));}else{const char*_tmp7F2;const char*_tmp7F1;
struct Cyc_PP_Doc*_tmp7F0[3];s=((_tmp7F0[2]=Cyc_PP_text(((_tmp7F1=")",
_tag_dyneither(_tmp7F1,sizeof(char),2)))),((_tmp7F0[1]=Cyc_PP_text(_tmp299),((
_tmp7F0[0]=Cyc_PP_text(((_tmp7F2="__asm__(",_tag_dyneither(_tmp7F2,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp7F0,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1CC: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp29A=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp25F;if(_tmp29A->tag != 38)goto _LL1CE;else{_tmp29B=_tmp29A->f1;_tmp29C=_tmp29A->f2;}}
_LL1CD:{const char*_tmp7F9;const char*_tmp7F8;const char*_tmp7F7;struct Cyc_PP_Doc*
_tmp7F6[5];s=((_tmp7F6[4]=Cyc_PP_text(((_tmp7F7=")",_tag_dyneither(_tmp7F7,
sizeof(char),2)))),((_tmp7F6[3]=Cyc_PP_textptr(_tmp29C),((_tmp7F6[2]=Cyc_PP_text(((
_tmp7F8=".",_tag_dyneither(_tmp7F8,sizeof(char),2)))),((_tmp7F6[1]=Cyc_Absynpp_exp2doc(
_tmp29B),((_tmp7F6[0]=Cyc_PP_text(((_tmp7F9="tagcheck(",_tag_dyneither(_tmp7F9,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7F6,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL19F;_LL1CE: {struct Cyc_Absyn_Offsetof_e_struct*_tmp29D=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp25F;if(_tmp29D->tag != 20)goto _LL1D0;else{
_tmp29E=(void*)_tmp29D->f1;_tmp29F=(void*)_tmp29D->f2;{struct Cyc_Absyn_StructField_struct*
_tmp2A0=(struct Cyc_Absyn_StructField_struct*)_tmp29F;if(_tmp2A0->tag != 0)goto
_LL1D0;else{_tmp2A1=_tmp2A0->f1;}};}}_LL1CF:{const char*_tmp800;const char*_tmp7FF;
const char*_tmp7FE;struct Cyc_PP_Doc*_tmp7FD[5];s=((_tmp7FD[4]=Cyc_PP_text(((
_tmp7FE=")",_tag_dyneither(_tmp7FE,sizeof(char),2)))),((_tmp7FD[3]=Cyc_PP_textptr(
_tmp2A1),((_tmp7FD[2]=Cyc_PP_text(((_tmp7FF=",",_tag_dyneither(_tmp7FF,sizeof(
char),2)))),((_tmp7FD[1]=Cyc_Absynpp_typ2doc(_tmp29E),((_tmp7FD[0]=Cyc_PP_text(((
_tmp800="offsetof(",_tag_dyneither(_tmp800,sizeof(char),10)))),Cyc_PP_cat(
_tag_dyneither(_tmp7FD,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL19F;
_LL1D0: {struct Cyc_Absyn_Offsetof_e_struct*_tmp2A2=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp25F;if(_tmp2A2->tag != 20)goto _LL1D2;else{_tmp2A3=(void*)_tmp2A2->f1;_tmp2A4=(
void*)_tmp2A2->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp2A5=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2A4;if(_tmp2A5->tag != 1)goto _LL1D2;else{_tmp2A6=_tmp2A5->f1;}};}}_LL1D1:{
const char*_tmp80E;const char*_tmp80D;struct Cyc_Int_pa_struct _tmp80C;void*_tmp80B[
1];const char*_tmp80A;const char*_tmp809;struct Cyc_PP_Doc*_tmp808[5];s=((_tmp808[4]=
Cyc_PP_text(((_tmp809=")",_tag_dyneither(_tmp809,sizeof(char),2)))),((_tmp808[3]=
Cyc_PP_text((struct _dyneither_ptr)((_tmp80C.tag=1,((_tmp80C.f1=(unsigned long)((
int)_tmp2A6),((_tmp80B[0]=& _tmp80C,Cyc_aprintf(((_tmp80A="%d",_tag_dyneither(
_tmp80A,sizeof(char),3))),_tag_dyneither(_tmp80B,sizeof(void*),1))))))))),((
_tmp808[2]=Cyc_PP_text(((_tmp80D=",",_tag_dyneither(_tmp80D,sizeof(char),2)))),((
_tmp808[1]=Cyc_Absynpp_typ2doc(_tmp2A3),((_tmp808[0]=Cyc_PP_text(((_tmp80E="offsetof(",
_tag_dyneither(_tmp80E,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp808,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL19F;_LL1D2: {struct Cyc_Absyn_Deref_e_struct*
_tmp2A7=(struct Cyc_Absyn_Deref_e_struct*)_tmp25F;if(_tmp2A7->tag != 21)goto _LL1D4;
else{_tmp2A8=_tmp2A7->f1;}}_LL1D3:{const char*_tmp811;struct Cyc_PP_Doc*_tmp810[2];
s=((_tmp810[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A8),((_tmp810[0]=Cyc_PP_text(((
_tmp811="*",_tag_dyneither(_tmp811,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp810,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL19F;_LL1D4: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp2A9=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp25F;if(_tmp2A9->tag != 22)goto
_LL1D6;else{_tmp2AA=_tmp2A9->f1;_tmp2AB=_tmp2A9->f2;}}_LL1D5:{const char*_tmp814;
struct Cyc_PP_Doc*_tmp813[3];s=((_tmp813[2]=Cyc_PP_textptr(_tmp2AB),((_tmp813[1]=
Cyc_PP_text(((_tmp814=".",_tag_dyneither(_tmp814,sizeof(char),2)))),((_tmp813[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AA),Cyc_PP_cat(_tag_dyneither(_tmp813,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1D6: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp2AC=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp25F;if(_tmp2AC->tag != 23)goto
_LL1D8;else{_tmp2AD=_tmp2AC->f1;_tmp2AE=_tmp2AC->f2;}}_LL1D7:{const char*_tmp817;
struct Cyc_PP_Doc*_tmp816[3];s=((_tmp816[2]=Cyc_PP_textptr(_tmp2AE),((_tmp816[1]=
Cyc_PP_text(((_tmp817="->",_tag_dyneither(_tmp817,sizeof(char),3)))),((_tmp816[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AD),Cyc_PP_cat(_tag_dyneither(_tmp816,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1D8: {struct Cyc_Absyn_Subscript_e_struct*
_tmp2AF=(struct Cyc_Absyn_Subscript_e_struct*)_tmp25F;if(_tmp2AF->tag != 24)goto
_LL1DA;else{_tmp2B0=_tmp2AF->f1;_tmp2B1=_tmp2AF->f2;}}_LL1D9:{const char*_tmp81C;
const char*_tmp81B;struct Cyc_PP_Doc*_tmp81A[4];s=((_tmp81A[3]=Cyc_PP_text(((
_tmp81B="]",_tag_dyneither(_tmp81B,sizeof(char),2)))),((_tmp81A[2]=Cyc_Absynpp_exp2doc(
_tmp2B1),((_tmp81A[1]=Cyc_PP_text(((_tmp81C="[",_tag_dyneither(_tmp81C,sizeof(
char),2)))),((_tmp81A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B0),Cyc_PP_cat(
_tag_dyneither(_tmp81A,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL19F;_LL1DA: {
struct Cyc_Absyn_Tuple_e_struct*_tmp2B2=(struct Cyc_Absyn_Tuple_e_struct*)_tmp25F;
if(_tmp2B2->tag != 25)goto _LL1DC;else{_tmp2B3=_tmp2B2->f1;}}_LL1DB:{const char*
_tmp821;const char*_tmp820;struct Cyc_PP_Doc*_tmp81F[4];s=((_tmp81F[3]=Cyc_PP_text(((
_tmp820=")",_tag_dyneither(_tmp820,sizeof(char),2)))),((_tmp81F[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp2B3),((_tmp81F[1]=Cyc_PP_text(((_tmp821="(",_tag_dyneither(_tmp821,
sizeof(char),2)))),((_tmp81F[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(
_tmp81F,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL19F;_LL1DC: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp2B4=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp25F;if(_tmp2B4->tag != 26)goto
_LL1DE;else{_tmp2B5=_tmp2B4->f1;_tmp2B6=_tmp2B4->f2;}}_LL1DD:{const char*_tmp828;
const char*_tmp827;const char*_tmp826;struct Cyc_PP_Doc*_tmp825[4];s=((_tmp825[3]=
Cyc_Absynpp_group_braces(((_tmp826=",",_tag_dyneither(_tmp826,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B6)),((_tmp825[2]=Cyc_PP_text(((_tmp827=")",
_tag_dyneither(_tmp827,sizeof(char),2)))),((_tmp825[1]=Cyc_Absynpp_typ2doc((*
_tmp2B5).f3),((_tmp825[0]=Cyc_PP_text(((_tmp828="(",_tag_dyneither(_tmp828,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp825,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL19F;_LL1DE: {struct Cyc_Absyn_Array_e_struct*_tmp2B7=(struct Cyc_Absyn_Array_e_struct*)
_tmp25F;if(_tmp2B7->tag != 27)goto _LL1E0;else{_tmp2B8=_tmp2B7->f1;}}_LL1DF:{const
char*_tmp829;s=Cyc_Absynpp_group_braces(((_tmp829=",",_tag_dyneither(_tmp829,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B8));}goto _LL19F;
_LL1E0: {struct Cyc_Absyn_Comprehension_e_struct*_tmp2B9=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp25F;if(_tmp2B9->tag != 28)goto _LL1E2;else{_tmp2BA=_tmp2B9->f1;_tmp2BB=_tmp2B9->f2;
_tmp2BC=_tmp2B9->f3;}}_LL1E1:{const char*_tmp830;const char*_tmp82F;const char*
_tmp82E;struct Cyc_PP_Doc*_tmp82D[8];s=((_tmp82D[7]=Cyc_Absynpp_rb(),((_tmp82D[6]=
Cyc_Absynpp_exp2doc(_tmp2BC),((_tmp82D[5]=Cyc_PP_text(((_tmp82E=" : ",
_tag_dyneither(_tmp82E,sizeof(char),4)))),((_tmp82D[4]=Cyc_Absynpp_exp2doc(
_tmp2BB),((_tmp82D[3]=Cyc_PP_text(((_tmp82F=" < ",_tag_dyneither(_tmp82F,sizeof(
char),4)))),((_tmp82D[2]=Cyc_PP_text(*(*_tmp2BA->name).f2),((_tmp82D[1]=Cyc_PP_text(((
_tmp830="for ",_tag_dyneither(_tmp830,sizeof(char),5)))),((_tmp82D[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp82D,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL19F;_LL1E2: {struct Cyc_Absyn_Aggregate_e_struct*_tmp2BD=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp25F;if(_tmp2BD->tag != 29)goto _LL1E4;else{_tmp2BE=_tmp2BD->f1;_tmp2BF=_tmp2BD->f2;
_tmp2C0=_tmp2BD->f3;}}_LL1E3: {struct Cyc_List_List*_tmp335=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2C0);{const char*_tmp835;struct Cyc_List_List*_tmp834;struct Cyc_PP_Doc*_tmp833[
2];s=((_tmp833[1]=Cyc_Absynpp_group_braces(((_tmp835=",",_tag_dyneither(_tmp835,
sizeof(char),2))),_tmp2BF != 0?(_tmp834=_cycalloc(sizeof(*_tmp834)),((_tmp834->hd=
Cyc_Absynpp_tps2doc(_tmp2BF),((_tmp834->tl=_tmp335,_tmp834))))): _tmp335),((
_tmp833[0]=Cyc_Absynpp_qvar2doc(_tmp2BE),Cyc_PP_cat(_tag_dyneither(_tmp833,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL19F;}_LL1E4: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp2C1=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp25F;if(_tmp2C1->tag != 30)goto
_LL1E6;else{_tmp2C2=_tmp2C1->f2;}}_LL1E5:{const char*_tmp836;s=Cyc_Absynpp_group_braces(((
_tmp836=",",_tag_dyneither(_tmp836,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2C2));}goto _LL19F;_LL1E6: {struct Cyc_Absyn_Datatype_e_struct*_tmp2C3=(struct
Cyc_Absyn_Datatype_e_struct*)_tmp25F;if(_tmp2C3->tag != 31)goto _LL1E8;else{
_tmp2C4=_tmp2C3->f1;_tmp2C5=_tmp2C3->f3;}}_LL1E7: if(_tmp2C4 == 0)s=Cyc_Absynpp_qvar2doc(
_tmp2C5->name);else{const char*_tmp83D;const char*_tmp83C;const char*_tmp83B;struct
Cyc_PP_Doc*_tmp83A[2];s=((_tmp83A[1]=Cyc_PP_egroup(((_tmp83D="(",_tag_dyneither(
_tmp83D,sizeof(char),2))),((_tmp83C=")",_tag_dyneither(_tmp83C,sizeof(char),2))),((
_tmp83B=",",_tag_dyneither(_tmp83B,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_exp2doc,_tmp2C4)),((_tmp83A[0]=Cyc_Absynpp_qvar2doc(_tmp2C5->name),
Cyc_PP_cat(_tag_dyneither(_tmp83A,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL19F;
_LL1E8: {struct Cyc_Absyn_Enum_e_struct*_tmp2C6=(struct Cyc_Absyn_Enum_e_struct*)
_tmp25F;if(_tmp2C6->tag != 32)goto _LL1EA;else{_tmp2C7=_tmp2C6->f1;}}_LL1E9: s=Cyc_Absynpp_qvar2doc(
_tmp2C7);goto _LL19F;_LL1EA: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp2C8=(struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp25F;if(_tmp2C8->tag != 33)goto _LL1EC;else{
_tmp2C9=_tmp2C8->f1;}}_LL1EB: s=Cyc_Absynpp_qvar2doc(_tmp2C9);goto _LL19F;_LL1EC: {
struct Cyc_Absyn_Malloc_e_struct*_tmp2CA=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp25F;if(_tmp2CA->tag != 34)goto _LL1EE;else{_tmp2CB=_tmp2CA->f1;_tmp2CC=_tmp2CB.is_calloc;
_tmp2CD=_tmp2CB.rgn;_tmp2CE=_tmp2CB.elt_type;_tmp2CF=_tmp2CB.num_elts;}}_LL1ED:
if(_tmp2CC){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp2CE)),0);if(_tmp2CD == 0){const char*_tmp844;const char*_tmp843;const char*
_tmp842;struct Cyc_PP_Doc*_tmp841[5];s=((_tmp841[4]=Cyc_PP_text(((_tmp842=")",
_tag_dyneither(_tmp842,sizeof(char),2)))),((_tmp841[3]=Cyc_Absynpp_exp2doc(st),((
_tmp841[2]=Cyc_PP_text(((_tmp843=",",_tag_dyneither(_tmp843,sizeof(char),2)))),((
_tmp841[1]=Cyc_Absynpp_exp2doc(_tmp2CF),((_tmp841[0]=Cyc_PP_text(((_tmp844="calloc(",
_tag_dyneither(_tmp844,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp841,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp84D;const char*
_tmp84C;const char*_tmp84B;const char*_tmp84A;struct Cyc_PP_Doc*_tmp849[7];s=((
_tmp849[6]=Cyc_PP_text(((_tmp84A=")",_tag_dyneither(_tmp84A,sizeof(char),2)))),((
_tmp849[5]=Cyc_Absynpp_exp2doc(st),((_tmp849[4]=Cyc_PP_text(((_tmp84B=",",
_tag_dyneither(_tmp84B,sizeof(char),2)))),((_tmp849[3]=Cyc_Absynpp_exp2doc(
_tmp2CF),((_tmp849[2]=Cyc_PP_text(((_tmp84C=",",_tag_dyneither(_tmp84C,sizeof(
char),2)))),((_tmp849[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2CD),((
_tmp849[0]=Cyc_PP_text(((_tmp84D="rcalloc(",_tag_dyneither(_tmp84D,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp849,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp2CE == 0)new_e=_tmp2CF;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp2CE,0),_tmp2CF,0);}if(_tmp2CD == 0){const char*
_tmp852;const char*_tmp851;struct Cyc_PP_Doc*_tmp850[3];s=((_tmp850[2]=Cyc_PP_text(((
_tmp851=")",_tag_dyneither(_tmp851,sizeof(char),2)))),((_tmp850[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp850[0]=Cyc_PP_text(((_tmp852="malloc(",_tag_dyneither(_tmp852,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp850,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp859;const char*_tmp858;const char*_tmp857;struct Cyc_PP_Doc*
_tmp856[5];s=((_tmp856[4]=Cyc_PP_text(((_tmp857=")",_tag_dyneither(_tmp857,
sizeof(char),2)))),((_tmp856[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp856[2]=Cyc_PP_text(((
_tmp858=",",_tag_dyneither(_tmp858,sizeof(char),2)))),((_tmp856[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2CD),((_tmp856[0]=Cyc_PP_text(((_tmp859="rmalloc(",
_tag_dyneither(_tmp859,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp856,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL19F;_LL1EE: {struct Cyc_Absyn_Swap_e_struct*
_tmp2D0=(struct Cyc_Absyn_Swap_e_struct*)_tmp25F;if(_tmp2D0->tag != 35)goto _LL1F0;
else{_tmp2D1=_tmp2D0->f1;_tmp2D2=_tmp2D0->f2;}}_LL1EF:{const char*_tmp85C;struct
Cyc_PP_Doc*_tmp85B[3];s=((_tmp85B[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D2),((
_tmp85B[1]=Cyc_PP_text(((_tmp85C=" :=: ",_tag_dyneither(_tmp85C,sizeof(char),6)))),((
_tmp85B[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D1),Cyc_PP_cat(_tag_dyneither(
_tmp85B,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1F0: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp2D3=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp25F;if(_tmp2D3->tag != 36)
goto _LL1F2;else{_tmp2D4=_tmp2D3->f1;_tmp2D5=_tmp2D3->f2;}}_LL1F1:{const char*
_tmp85D;s=Cyc_Absynpp_group_braces(((_tmp85D=",",_tag_dyneither(_tmp85D,sizeof(
char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2D5));}goto _LL19F;
_LL1F2: {struct Cyc_Absyn_StmtExp_e_struct*_tmp2D6=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp25F;if(_tmp2D6->tag != 37)goto _LL19F;else{_tmp2D7=_tmp2D6->f1;}}_LL1F3:{const
char*_tmp862;const char*_tmp861;struct Cyc_PP_Doc*_tmp860[7];s=((_tmp860[6]=Cyc_PP_text(((
_tmp861=")",_tag_dyneither(_tmp861,sizeof(char),2)))),((_tmp860[5]=Cyc_Absynpp_rb(),((
_tmp860[4]=Cyc_PP_blank_doc(),((_tmp860[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2D7,1)),((_tmp860[2]=Cyc_PP_blank_doc(),((_tmp860[1]=Cyc_Absynpp_lb(),((
_tmp860[0]=Cyc_PP_text(((_tmp862="(",_tag_dyneither(_tmp862,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp860,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
goto _LL19F;_LL19F:;}if(inprec >= myprec){const char*_tmp867;const char*_tmp866;
struct Cyc_PP_Doc*_tmp865[3];s=((_tmp865[2]=Cyc_PP_text(((_tmp866=")",
_tag_dyneither(_tmp866,sizeof(char),2)))),((_tmp865[1]=s,((_tmp865[0]=Cyc_PP_text(((
_tmp867="(",_tag_dyneither(_tmp867,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp865,sizeof(struct Cyc_PP_Doc*),3))))))));}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d){void*_tmp357=d;struct Cyc_Absyn_Exp*_tmp359;struct _dyneither_ptr*_tmp35B;
_LL1FA: {struct Cyc_Absyn_ArrayElement_struct*_tmp358=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp357;if(_tmp358->tag != 0)goto _LL1FC;else{_tmp359=_tmp358->f1;}}_LL1FB: {const
char*_tmp86C;const char*_tmp86B;struct Cyc_PP_Doc*_tmp86A[3];return(_tmp86A[2]=Cyc_PP_text(((
_tmp86B="]",_tag_dyneither(_tmp86B,sizeof(char),2)))),((_tmp86A[1]=Cyc_Absynpp_exp2doc(
_tmp359),((_tmp86A[0]=Cyc_PP_text(((_tmp86C=".[",_tag_dyneither(_tmp86C,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp86A,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL1FC: {struct Cyc_Absyn_FieldName_struct*_tmp35A=(struct Cyc_Absyn_FieldName_struct*)
_tmp357;if(_tmp35A->tag != 1)goto _LL1F9;else{_tmp35B=_tmp35A->f1;}}_LL1FD: {const
char*_tmp86F;struct Cyc_PP_Doc*_tmp86E[2];return(_tmp86E[1]=Cyc_PP_textptr(
_tmp35B),((_tmp86E[0]=Cyc_PP_text(((_tmp86F=".",_tag_dyneither(_tmp86F,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp86E,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL1F9:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){if((*de).f1 == 0)
return Cyc_Absynpp_exp2doc((*de).f2);else{const char*_tmp876;const char*_tmp875;
const char*_tmp874;struct Cyc_PP_Doc*_tmp873[2];return(_tmp873[1]=Cyc_Absynpp_exp2doc((*
de).f2),((_tmp873[0]=Cyc_PP_egroup(((_tmp876="",_tag_dyneither(_tmp876,sizeof(
char),1))),((_tmp875="=",_tag_dyneither(_tmp875,sizeof(char),2))),((_tmp874="=",
_tag_dyneither(_tmp874,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),
Cyc_PP_cat(_tag_dyneither(_tmp873,sizeof(struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){const char*_tmp879;
const char*_tmp878;const char*_tmp877;return Cyc_PP_group(((_tmp877="",
_tag_dyneither(_tmp877,sizeof(char),1))),((_tmp878="",_tag_dyneither(_tmp878,
sizeof(char),1))),((_tmp879=",",_tag_dyneither(_tmp879,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst
_tmp368=c;struct _tuple3 _tmp369;enum Cyc_Absyn_Sign _tmp36A;char _tmp36B;struct
_tuple4 _tmp36C;enum Cyc_Absyn_Sign _tmp36D;short _tmp36E;struct _tuple5 _tmp36F;
enum Cyc_Absyn_Sign _tmp370;int _tmp371;struct _tuple5 _tmp372;enum Cyc_Absyn_Sign
_tmp373;int _tmp374;struct _tuple5 _tmp375;enum Cyc_Absyn_Sign _tmp376;int _tmp377;
struct _tuple6 _tmp378;enum Cyc_Absyn_Sign _tmp379;long long _tmp37A;struct
_dyneither_ptr _tmp37B;int _tmp37C;struct _dyneither_ptr _tmp37D;_LL1FF: if((_tmp368.Char_c).tag
!= 2)goto _LL201;_tmp369=(struct _tuple3)(_tmp368.Char_c).val;_tmp36A=_tmp369.f1;
_tmp36B=_tmp369.f2;_LL200: {const char*_tmp87D;void*_tmp87C[1];struct Cyc_String_pa_struct
_tmp87B;return Cyc_PP_text((struct _dyneither_ptr)((_tmp87B.tag=0,((_tmp87B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp36B)),((
_tmp87C[0]=& _tmp87B,Cyc_aprintf(((_tmp87D="'%s'",_tag_dyneither(_tmp87D,sizeof(
char),5))),_tag_dyneither(_tmp87C,sizeof(void*),1)))))))));}_LL201: if((_tmp368.Short_c).tag
!= 3)goto _LL203;_tmp36C=(struct _tuple4)(_tmp368.Short_c).val;_tmp36D=_tmp36C.f1;
_tmp36E=_tmp36C.f2;_LL202: {const char*_tmp881;void*_tmp880[1];struct Cyc_Int_pa_struct
_tmp87F;return Cyc_PP_text((struct _dyneither_ptr)((_tmp87F.tag=1,((_tmp87F.f1=(
unsigned long)((int)_tmp36E),((_tmp880[0]=& _tmp87F,Cyc_aprintf(((_tmp881="%d",
_tag_dyneither(_tmp881,sizeof(char),3))),_tag_dyneither(_tmp880,sizeof(void*),1)))))))));}
_LL203: if((_tmp368.Int_c).tag != 4)goto _LL205;_tmp36F=(struct _tuple5)(_tmp368.Int_c).val;
_tmp370=_tmp36F.f1;if(_tmp370 != Cyc_Absyn_None)goto _LL205;_tmp371=_tmp36F.f2;
_LL204: _tmp374=_tmp371;goto _LL206;_LL205: if((_tmp368.Int_c).tag != 4)goto _LL207;
_tmp372=(struct _tuple5)(_tmp368.Int_c).val;_tmp373=_tmp372.f1;if(_tmp373 != Cyc_Absyn_Signed)
goto _LL207;_tmp374=_tmp372.f2;_LL206: {const char*_tmp885;void*_tmp884[1];struct
Cyc_Int_pa_struct _tmp883;return Cyc_PP_text((struct _dyneither_ptr)((_tmp883.tag=1,((
_tmp883.f1=(unsigned long)_tmp374,((_tmp884[0]=& _tmp883,Cyc_aprintf(((_tmp885="%d",
_tag_dyneither(_tmp885,sizeof(char),3))),_tag_dyneither(_tmp884,sizeof(void*),1)))))))));}
_LL207: if((_tmp368.Int_c).tag != 4)goto _LL209;_tmp375=(struct _tuple5)(_tmp368.Int_c).val;
_tmp376=_tmp375.f1;if(_tmp376 != Cyc_Absyn_Unsigned)goto _LL209;_tmp377=_tmp375.f2;
_LL208: {const char*_tmp889;void*_tmp888[1];struct Cyc_Int_pa_struct _tmp887;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp887.tag=1,((_tmp887.f1=(unsigned int)
_tmp377,((_tmp888[0]=& _tmp887,Cyc_aprintf(((_tmp889="%u",_tag_dyneither(_tmp889,
sizeof(char),3))),_tag_dyneither(_tmp888,sizeof(void*),1)))))))));}_LL209: if((
_tmp368.LongLong_c).tag != 5)goto _LL20B;_tmp378=(struct _tuple6)(_tmp368.LongLong_c).val;
_tmp379=_tmp378.f1;_tmp37A=_tmp378.f2;_LL20A: {const char*_tmp88A;return Cyc_PP_text(((
_tmp88A="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp88A,sizeof(char),27))));}
_LL20B: if((_tmp368.Float_c).tag != 6)goto _LL20D;_tmp37B=(struct _dyneither_ptr)(
_tmp368.Float_c).val;_LL20C: return Cyc_PP_text(_tmp37B);_LL20D: if((_tmp368.Null_c).tag
!= 1)goto _LL20F;_tmp37C=(int)(_tmp368.Null_c).val;_LL20E: {const char*_tmp88B;
return Cyc_PP_text(((_tmp88B="NULL",_tag_dyneither(_tmp88B,sizeof(char),5))));}
_LL20F: if((_tmp368.String_c).tag != 7)goto _LL1FE;_tmp37D=(struct _dyneither_ptr)(
_tmp368.String_c).val;_LL210: {const char*_tmp890;const char*_tmp88F;struct Cyc_PP_Doc*
_tmp88E[3];return(_tmp88E[2]=Cyc_PP_text(((_tmp88F="\"",_tag_dyneither(_tmp88F,
sizeof(char),2)))),((_tmp88E[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp37D)),((
_tmp88E[0]=Cyc_PP_text(((_tmp890="\"",_tag_dyneither(_tmp890,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp88E,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1FE:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct
Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (enum Cyc_Absyn_Primop)
Cyc_Absyn_Numelts){if(es == 0  || es->tl != 0){struct Cyc_Core_Failure_struct _tmp89D;
const char*_tmp89C;void*_tmp89B[1];struct Cyc_String_pa_struct _tmp89A;struct Cyc_Core_Failure_struct*
_tmp899;(int)_throw((void*)((_tmp899=_cycalloc(sizeof(*_tmp899)),((_tmp899[0]=((
_tmp89D.tag=Cyc_Core_Failure,((_tmp89D.f1=(struct _dyneither_ptr)((_tmp89A.tag=0,((
_tmp89A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp89B[0]=& _tmp89A,Cyc_aprintf(((_tmp89C="Absynpp::primapp2doc Numelts: %s with bad args",
_tag_dyneither(_tmp89C,sizeof(char),47))),_tag_dyneither(_tmp89B,sizeof(void*),1)))))))),
_tmp89D)))),_tmp899)))));}{const char*_tmp8A2;const char*_tmp8A1;struct Cyc_PP_Doc*
_tmp8A0[3];return(_tmp8A0[2]=Cyc_PP_text(((_tmp8A1=")",_tag_dyneither(_tmp8A1,
sizeof(char),2)))),((_tmp8A0[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((
_tmp8A0[0]=Cyc_PP_text(((_tmp8A2="numelts(",_tag_dyneither(_tmp8A2,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp8A0,sizeof(struct Cyc_PP_Doc*),3)))))));};}
else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0){struct Cyc_Core_Failure_struct _tmp8AF;const char*_tmp8AE;
void*_tmp8AD[1];struct Cyc_String_pa_struct _tmp8AC;struct Cyc_Core_Failure_struct*
_tmp8AB;(int)_throw((void*)((_tmp8AB=_cycalloc(sizeof(*_tmp8AB)),((_tmp8AB[0]=((
_tmp8AF.tag=Cyc_Core_Failure,((_tmp8AF.f1=(struct _dyneither_ptr)((_tmp8AC.tag=0,((
_tmp8AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp8AD[0]=& _tmp8AC,Cyc_aprintf(((_tmp8AE="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp8AE,sizeof(char),38))),_tag_dyneither(_tmp8AD,sizeof(void*),1)))))))),
_tmp8AF)))),_tmp8AB)))));}else{if(ds->tl == 0){const char*_tmp8B2;struct Cyc_PP_Doc*
_tmp8B1[3];return(_tmp8B1[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp8B1[1]=Cyc_PP_text(((
_tmp8B2=" ",_tag_dyneither(_tmp8B2,sizeof(char),2)))),((_tmp8B1[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp8B1,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp8BF;const char*
_tmp8BE;void*_tmp8BD[1];struct Cyc_String_pa_struct _tmp8BC;struct Cyc_Core_Failure_struct*
_tmp8BB;(int)_throw((void*)((_tmp8BB=_cycalloc(sizeof(*_tmp8BB)),((_tmp8BB[0]=((
_tmp8BF.tag=Cyc_Core_Failure,((_tmp8BF.f1=(struct _dyneither_ptr)((_tmp8BC.tag=0,((
_tmp8BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp8BD[0]=& _tmp8BC,Cyc_aprintf(((_tmp8BE="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp8BE,sizeof(char),47))),_tag_dyneither(_tmp8BD,sizeof(void*),1)))))))),
_tmp8BF)))),_tmp8BB)))));}else{const char*_tmp8C4;const char*_tmp8C3;struct Cyc_PP_Doc*
_tmp8C2[5];return(_tmp8C2[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp8C2[3]=Cyc_PP_text(((_tmp8C3=" ",_tag_dyneither(
_tmp8C3,sizeof(char),2)))),((_tmp8C2[2]=ps,((_tmp8C2[1]=Cyc_PP_text(((_tmp8C4=" ",
_tag_dyneither(_tmp8C4,sizeof(char),2)))),((_tmp8C2[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp8C2,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){switch(p){case
Cyc_Absyn_Plus: _LL211: {const char*_tmp8C5;return(_tmp8C5="+",_tag_dyneither(
_tmp8C5,sizeof(char),2));}case Cyc_Absyn_Times: _LL212: {const char*_tmp8C6;return(
_tmp8C6="*",_tag_dyneither(_tmp8C6,sizeof(char),2));}case Cyc_Absyn_Minus: _LL213: {
const char*_tmp8C7;return(_tmp8C7="-",_tag_dyneither(_tmp8C7,sizeof(char),2));}
case Cyc_Absyn_Div: _LL214: {const char*_tmp8C8;return(_tmp8C8="/",_tag_dyneither(
_tmp8C8,sizeof(char),2));}case Cyc_Absyn_Mod: _LL215: {const char*_tmp8CA;const char*
_tmp8C9;return Cyc_Absynpp_print_for_cycdoc?(_tmp8CA="\\%",_tag_dyneither(_tmp8CA,
sizeof(char),3)):((_tmp8C9="%",_tag_dyneither(_tmp8C9,sizeof(char),2)));}case Cyc_Absyn_Eq:
_LL216: {const char*_tmp8CB;return(_tmp8CB="==",_tag_dyneither(_tmp8CB,sizeof(
char),3));}case Cyc_Absyn_Neq: _LL217: {const char*_tmp8CC;return(_tmp8CC="!=",
_tag_dyneither(_tmp8CC,sizeof(char),3));}case Cyc_Absyn_Gt: _LL218: {const char*
_tmp8CD;return(_tmp8CD=">",_tag_dyneither(_tmp8CD,sizeof(char),2));}case Cyc_Absyn_Lt:
_LL219: {const char*_tmp8CE;return(_tmp8CE="<",_tag_dyneither(_tmp8CE,sizeof(char),
2));}case Cyc_Absyn_Gte: _LL21A: {const char*_tmp8CF;return(_tmp8CF=">=",
_tag_dyneither(_tmp8CF,sizeof(char),3));}case Cyc_Absyn_Lte: _LL21B: {const char*
_tmp8D0;return(_tmp8D0="<=",_tag_dyneither(_tmp8D0,sizeof(char),3));}case Cyc_Absyn_Not:
_LL21C: {const char*_tmp8D1;return(_tmp8D1="!",_tag_dyneither(_tmp8D1,sizeof(char),
2));}case Cyc_Absyn_Bitnot: _LL21D: {const char*_tmp8D2;return(_tmp8D2="~",
_tag_dyneither(_tmp8D2,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL21E: {const char*
_tmp8D3;return(_tmp8D3="&",_tag_dyneither(_tmp8D3,sizeof(char),2));}case Cyc_Absyn_Bitor:
_LL21F: {const char*_tmp8D4;return(_tmp8D4="|",_tag_dyneither(_tmp8D4,sizeof(char),
2));}case Cyc_Absyn_Bitxor: _LL220: {const char*_tmp8D5;return(_tmp8D5="^",
_tag_dyneither(_tmp8D5,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL221: {const
char*_tmp8D6;return(_tmp8D6="<<",_tag_dyneither(_tmp8D6,sizeof(char),3));}case
Cyc_Absyn_Bitlrshift: _LL222: {const char*_tmp8D7;return(_tmp8D7=">>",
_tag_dyneither(_tmp8D7,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL223: {const
char*_tmp8D8;return(_tmp8D8=">>>",_tag_dyneither(_tmp8D8,sizeof(char),4));}case
Cyc_Absyn_Numelts: _LL224: {const char*_tmp8D9;return(_tmp8D9="numelts",
_tag_dyneither(_tmp8D9,sizeof(char),8));}}}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
enum Cyc_Absyn_Primop p){return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp3BB=s->r;_LL227: {struct Cyc_Absyn_Decl_s_struct*
_tmp3BC=(struct Cyc_Absyn_Decl_s_struct*)_tmp3BB;if(_tmp3BC->tag != 12)goto _LL229;}
_LL228: return 1;_LL229:;_LL22A: return 0;_LL226:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st,int stmtexp){struct Cyc_PP_Doc*s;{void*_tmp3BD=st->r;
struct Cyc_Absyn_Exp*_tmp3C0;struct Cyc_Absyn_Stmt*_tmp3C2;struct Cyc_Absyn_Stmt*
_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C5;struct Cyc_Absyn_Exp*_tmp3C7;struct Cyc_Absyn_Stmt*
_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3C9;struct _tuple8 _tmp3CB;struct Cyc_Absyn_Exp*
_tmp3CC;struct Cyc_Absyn_Stmt*_tmp3CD;struct _dyneither_ptr*_tmp3D1;struct Cyc_Absyn_Exp*
_tmp3D3;struct _tuple8 _tmp3D4;struct Cyc_Absyn_Exp*_tmp3D5;struct _tuple8 _tmp3D6;
struct Cyc_Absyn_Exp*_tmp3D7;struct Cyc_Absyn_Stmt*_tmp3D8;struct Cyc_Absyn_Exp*
_tmp3DA;struct Cyc_List_List*_tmp3DB;struct Cyc_List_List*_tmp3DD;struct Cyc_List_List*
_tmp3DF;struct Cyc_Absyn_Decl*_tmp3E1;struct Cyc_Absyn_Stmt*_tmp3E2;struct
_dyneither_ptr*_tmp3E4;struct Cyc_Absyn_Stmt*_tmp3E5;struct Cyc_Absyn_Stmt*_tmp3E7;
struct _tuple8 _tmp3E8;struct Cyc_Absyn_Exp*_tmp3E9;struct Cyc_Absyn_Stmt*_tmp3EB;
struct Cyc_List_List*_tmp3EC;struct Cyc_Absyn_Exp*_tmp3EE;_LL22C: {struct Cyc_Absyn_Skip_s_struct*
_tmp3BE=(struct Cyc_Absyn_Skip_s_struct*)_tmp3BD;if(_tmp3BE->tag != 0)goto _LL22E;}
_LL22D:{const char*_tmp8DA;s=Cyc_PP_text(((_tmp8DA=";",_tag_dyneither(_tmp8DA,
sizeof(char),2))));}goto _LL22B;_LL22E: {struct Cyc_Absyn_Exp_s_struct*_tmp3BF=(
struct Cyc_Absyn_Exp_s_struct*)_tmp3BD;if(_tmp3BF->tag != 1)goto _LL230;else{
_tmp3C0=_tmp3BF->f1;}}_LL22F:{const char*_tmp8DD;struct Cyc_PP_Doc*_tmp8DC[2];s=((
_tmp8DC[1]=Cyc_PP_text(((_tmp8DD=";",_tag_dyneither(_tmp8DD,sizeof(char),2)))),((
_tmp8DC[0]=Cyc_Absynpp_exp2doc(_tmp3C0),Cyc_PP_cat(_tag_dyneither(_tmp8DC,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL22B;_LL230: {struct Cyc_Absyn_Seq_s_struct*
_tmp3C1=(struct Cyc_Absyn_Seq_s_struct*)_tmp3BD;if(_tmp3C1->tag != 2)goto _LL232;
else{_tmp3C2=_tmp3C1->f1;_tmp3C3=_tmp3C1->f2;}}_LL231: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3C2)){struct Cyc_PP_Doc*_tmp8E8[5];struct Cyc_PP_Doc*
_tmp8E7[7];const char*_tmp8E6;const char*_tmp8E5;struct Cyc_PP_Doc*_tmp8E4[7];s=((
_tmp8E4[6]=Cyc_Absynpp_is_declaration(_tmp3C3)?stmtexp?(_tmp8E7[6]=Cyc_PP_line_doc(),((
_tmp8E7[5]=Cyc_PP_text(((_tmp8E6=");",_tag_dyneither(_tmp8E6,sizeof(char),3)))),((
_tmp8E7[4]=Cyc_Absynpp_rb(),((_tmp8E7[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3C3,stmtexp)),((_tmp8E7[2]=Cyc_PP_blank_doc(),((_tmp8E7[1]=Cyc_Absynpp_lb(),((
_tmp8E7[0]=Cyc_PP_text(((_tmp8E5="(",_tag_dyneither(_tmp8E5,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp8E7,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):((
_tmp8E8[4]=Cyc_PP_line_doc(),((_tmp8E8[3]=Cyc_Absynpp_rb(),((_tmp8E8[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3C3,stmtexp)),((_tmp8E8[1]=Cyc_PP_blank_doc(),((
_tmp8E8[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8E8,sizeof(struct Cyc_PP_Doc*),
5)))))))))))): Cyc_Absynpp_stmt2doc(_tmp3C3,stmtexp),((_tmp8E4[5]=Cyc_PP_line_doc(),((
_tmp8E4[4]=Cyc_Absynpp_rb(),((_tmp8E4[3]=Cyc_PP_line_doc(),((_tmp8E4[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3C2,0)),((_tmp8E4[1]=Cyc_PP_blank_doc(),((_tmp8E4[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8E4,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
else{if(Cyc_Absynpp_is_declaration(_tmp3C3)){struct Cyc_PP_Doc*_tmp8F3[4];struct
Cyc_PP_Doc*_tmp8F2[6];const char*_tmp8F1;const char*_tmp8F0;struct Cyc_PP_Doc*
_tmp8EF[4];s=((_tmp8EF[3]=Cyc_PP_line_doc(),((_tmp8EF[2]=stmtexp?(_tmp8F2[5]=Cyc_PP_text(((
_tmp8F1=");",_tag_dyneither(_tmp8F1,sizeof(char),3)))),((_tmp8F2[4]=Cyc_Absynpp_rb(),((
_tmp8F2[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C3,stmtexp)),((_tmp8F2[2]=Cyc_PP_blank_doc(),((
_tmp8F2[1]=Cyc_Absynpp_lb(),((_tmp8F2[0]=Cyc_PP_text(((_tmp8F0="(",
_tag_dyneither(_tmp8F0,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8F2,
sizeof(struct Cyc_PP_Doc*),6))))))))))))):((_tmp8F3[3]=Cyc_Absynpp_rb(),((_tmp8F3[
2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C3,stmtexp)),((_tmp8F3[1]=Cyc_PP_blank_doc(),((
_tmp8F3[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8F3,sizeof(struct Cyc_PP_Doc*),
4)))))))))),((_tmp8EF[1]=Cyc_PP_line_doc(),((_tmp8EF[0]=Cyc_Absynpp_stmt2doc(
_tmp3C2,0),Cyc_PP_cat(_tag_dyneither(_tmp8EF,sizeof(struct Cyc_PP_Doc*),4))))))))));}
else{struct Cyc_PP_Doc*_tmp8F4[3];s=((_tmp8F4[2]=Cyc_Absynpp_stmt2doc(_tmp3C3,
stmtexp),((_tmp8F4[1]=Cyc_PP_line_doc(),((_tmp8F4[0]=Cyc_Absynpp_stmt2doc(
_tmp3C2,0),Cyc_PP_cat(_tag_dyneither(_tmp8F4,sizeof(struct Cyc_PP_Doc*),3))))))));}}}
else{struct Cyc_PP_Doc*_tmp8F5[3];s=((_tmp8F5[2]=Cyc_Absynpp_stmt2doc(_tmp3C3,
stmtexp),((_tmp8F5[1]=Cyc_PP_line_doc(),((_tmp8F5[0]=Cyc_Absynpp_stmt2doc(
_tmp3C2,0),Cyc_PP_cat(_tag_dyneither(_tmp8F5,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22B;_LL232: {struct Cyc_Absyn_Return_s_struct*_tmp3C4=(struct Cyc_Absyn_Return_s_struct*)
_tmp3BD;if(_tmp3C4->tag != 3)goto _LL234;else{_tmp3C5=_tmp3C4->f1;}}_LL233: if(
_tmp3C5 == 0){const char*_tmp8F6;s=Cyc_PP_text(((_tmp8F6="return;",_tag_dyneither(
_tmp8F6,sizeof(char),8))));}else{const char*_tmp8FB;const char*_tmp8FA;struct Cyc_PP_Doc*
_tmp8F9[3];s=((_tmp8F9[2]=Cyc_PP_text(((_tmp8FA=";",_tag_dyneither(_tmp8FA,
sizeof(char),2)))),((_tmp8F9[1]=_tmp3C5 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp3C5),((_tmp8F9[0]=Cyc_PP_text(((_tmp8FB="return ",
_tag_dyneither(_tmp8FB,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8F9,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL22B;_LL234: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp3C6=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3BD;if(_tmp3C6->tag != 4)goto
_LL236;else{_tmp3C7=_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;_tmp3C9=_tmp3C6->f3;}}_LL235: {
int print_else;{void*_tmp402=_tmp3C9->r;_LL251: {struct Cyc_Absyn_Skip_s_struct*
_tmp403=(struct Cyc_Absyn_Skip_s_struct*)_tmp402;if(_tmp403->tag != 0)goto _LL253;}
_LL252: print_else=0;goto _LL250;_LL253:;_LL254: print_else=1;goto _LL250;_LL250:;}{
const char*_tmp90A;const char*_tmp909;struct Cyc_PP_Doc*_tmp908[2];struct Cyc_PP_Doc*
_tmp907[6];struct Cyc_PP_Doc*_tmp906[2];const char*_tmp905;struct Cyc_PP_Doc*
_tmp904[8];s=((_tmp904[7]=print_else?(_tmp907[5]=Cyc_Absynpp_rb(),((_tmp907[4]=
Cyc_PP_line_doc(),((_tmp907[3]=Cyc_PP_nest(2,((_tmp906[1]=Cyc_Absynpp_stmt2doc(
_tmp3C9,0),((_tmp906[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp906,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp907[2]=Cyc_Absynpp_lb(),((_tmp907[1]=
Cyc_PP_text(((_tmp905="else ",_tag_dyneither(_tmp905,sizeof(char),6)))),((
_tmp907[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp907,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp904[6]=Cyc_Absynpp_rb(),((_tmp904[5]=Cyc_PP_line_doc(),((
_tmp904[4]=Cyc_PP_nest(2,((_tmp908[1]=Cyc_Absynpp_stmt2doc(_tmp3C8,0),((_tmp908[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp908,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp904[3]=Cyc_Absynpp_lb(),((_tmp904[2]=Cyc_PP_text(((_tmp909=") ",
_tag_dyneither(_tmp909,sizeof(char),3)))),((_tmp904[1]=Cyc_Absynpp_exp2doc(
_tmp3C7),((_tmp904[0]=Cyc_PP_text(((_tmp90A="if (",_tag_dyneither(_tmp90A,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp904,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL22B;}_LL236: {struct Cyc_Absyn_While_s_struct*_tmp3CA=(struct Cyc_Absyn_While_s_struct*)
_tmp3BD;if(_tmp3CA->tag != 5)goto _LL238;else{_tmp3CB=_tmp3CA->f1;_tmp3CC=_tmp3CB.f1;
_tmp3CD=_tmp3CA->f2;}}_LL237:{const char*_tmp911;const char*_tmp910;struct Cyc_PP_Doc*
_tmp90F[2];struct Cyc_PP_Doc*_tmp90E[7];s=((_tmp90E[6]=Cyc_Absynpp_rb(),((_tmp90E[
5]=Cyc_PP_line_doc(),((_tmp90E[4]=Cyc_PP_nest(2,((_tmp90F[1]=Cyc_Absynpp_stmt2doc(
_tmp3CD,0),((_tmp90F[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp90F,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp90E[3]=Cyc_Absynpp_lb(),((_tmp90E[2]=
Cyc_PP_text(((_tmp910=") ",_tag_dyneither(_tmp910,sizeof(char),3)))),((_tmp90E[1]=
Cyc_Absynpp_exp2doc(_tmp3CC),((_tmp90E[0]=Cyc_PP_text(((_tmp911="while (",
_tag_dyneither(_tmp911,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp90E,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL22B;_LL238: {struct Cyc_Absyn_Break_s_struct*
_tmp3CE=(struct Cyc_Absyn_Break_s_struct*)_tmp3BD;if(_tmp3CE->tag != 6)goto _LL23A;}
_LL239:{const char*_tmp912;s=Cyc_PP_text(((_tmp912="break;",_tag_dyneither(
_tmp912,sizeof(char),7))));}goto _LL22B;_LL23A: {struct Cyc_Absyn_Continue_s_struct*
_tmp3CF=(struct Cyc_Absyn_Continue_s_struct*)_tmp3BD;if(_tmp3CF->tag != 7)goto
_LL23C;}_LL23B:{const char*_tmp913;s=Cyc_PP_text(((_tmp913="continue;",
_tag_dyneither(_tmp913,sizeof(char),10))));}goto _LL22B;_LL23C: {struct Cyc_Absyn_Goto_s_struct*
_tmp3D0=(struct Cyc_Absyn_Goto_s_struct*)_tmp3BD;if(_tmp3D0->tag != 8)goto _LL23E;
else{_tmp3D1=_tmp3D0->f1;}}_LL23D:{const char*_tmp917;void*_tmp916[1];struct Cyc_String_pa_struct
_tmp915;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp915.tag=0,((_tmp915.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3D1),((_tmp916[0]=& _tmp915,Cyc_aprintf(((
_tmp917="goto %s;",_tag_dyneither(_tmp917,sizeof(char),9))),_tag_dyneither(
_tmp916,sizeof(void*),1)))))))));}goto _LL22B;_LL23E: {struct Cyc_Absyn_For_s_struct*
_tmp3D2=(struct Cyc_Absyn_For_s_struct*)_tmp3BD;if(_tmp3D2->tag != 9)goto _LL240;
else{_tmp3D3=_tmp3D2->f1;_tmp3D4=_tmp3D2->f2;_tmp3D5=_tmp3D4.f1;_tmp3D6=_tmp3D2->f3;
_tmp3D7=_tmp3D6.f1;_tmp3D8=_tmp3D2->f4;}}_LL23F:{const char*_tmp922;const char*
_tmp921;const char*_tmp920;const char*_tmp91F;struct Cyc_PP_Doc*_tmp91E[2];struct
Cyc_PP_Doc*_tmp91D[11];s=((_tmp91D[10]=Cyc_Absynpp_rb(),((_tmp91D[9]=Cyc_PP_line_doc(),((
_tmp91D[8]=Cyc_PP_nest(2,((_tmp91E[1]=Cyc_Absynpp_stmt2doc(_tmp3D8,0),((_tmp91E[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp91E,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp91D[7]=Cyc_Absynpp_lb(),((_tmp91D[6]=Cyc_PP_text(((_tmp91F=") ",
_tag_dyneither(_tmp91F,sizeof(char),3)))),((_tmp91D[5]=Cyc_Absynpp_exp2doc(
_tmp3D7),((_tmp91D[4]=Cyc_PP_text(((_tmp920="; ",_tag_dyneither(_tmp920,sizeof(
char),3)))),((_tmp91D[3]=Cyc_Absynpp_exp2doc(_tmp3D5),((_tmp91D[2]=Cyc_PP_text(((
_tmp921="; ",_tag_dyneither(_tmp921,sizeof(char),3)))),((_tmp91D[1]=Cyc_Absynpp_exp2doc(
_tmp3D3),((_tmp91D[0]=Cyc_PP_text(((_tmp922="for(",_tag_dyneither(_tmp922,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp91D,sizeof(struct Cyc_PP_Doc*),
11))))))))))))))))))))))));}goto _LL22B;_LL240: {struct Cyc_Absyn_Switch_s_struct*
_tmp3D9=(struct Cyc_Absyn_Switch_s_struct*)_tmp3BD;if(_tmp3D9->tag != 10)goto
_LL242;else{_tmp3DA=_tmp3D9->f1;_tmp3DB=_tmp3D9->f2;}}_LL241:{const char*_tmp927;
const char*_tmp926;struct Cyc_PP_Doc*_tmp925[8];s=((_tmp925[7]=Cyc_Absynpp_rb(),((
_tmp925[6]=Cyc_PP_line_doc(),((_tmp925[5]=Cyc_Absynpp_switchclauses2doc(_tmp3DB),((
_tmp925[4]=Cyc_PP_line_doc(),((_tmp925[3]=Cyc_Absynpp_lb(),((_tmp925[2]=Cyc_PP_text(((
_tmp926=") ",_tag_dyneither(_tmp926,sizeof(char),3)))),((_tmp925[1]=Cyc_Absynpp_exp2doc(
_tmp3DA),((_tmp925[0]=Cyc_PP_text(((_tmp927="switch (",_tag_dyneither(_tmp927,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp925,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL22B;_LL242: {struct Cyc_Absyn_Fallthru_s_struct*_tmp3DC=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp3BD;if(_tmp3DC->tag != 11)goto _LL244;else{_tmp3DD=_tmp3DC->f1;if(_tmp3DD != 0)
goto _LL244;}}_LL243:{const char*_tmp928;s=Cyc_PP_text(((_tmp928="fallthru;",
_tag_dyneither(_tmp928,sizeof(char),10))));}goto _LL22B;_LL244: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp3DE=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp3BD;if(_tmp3DE->tag != 11)goto
_LL246;else{_tmp3DF=_tmp3DE->f1;}}_LL245:{const char*_tmp92D;const char*_tmp92C;
struct Cyc_PP_Doc*_tmp92B[3];s=((_tmp92B[2]=Cyc_PP_text(((_tmp92C=");",
_tag_dyneither(_tmp92C,sizeof(char),3)))),((_tmp92B[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp3DF),((_tmp92B[0]=Cyc_PP_text(((_tmp92D="fallthru(",_tag_dyneither(
_tmp92D,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp92B,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL22B;_LL246: {struct Cyc_Absyn_Decl_s_struct*_tmp3E0=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3BD;if(_tmp3E0->tag != 12)goto _LL248;else{_tmp3E1=_tmp3E0->f1;_tmp3E2=_tmp3E0->f2;}}
_LL247:{struct Cyc_PP_Doc*_tmp92E[3];s=((_tmp92E[2]=Cyc_Absynpp_stmt2doc(_tmp3E2,
stmtexp),((_tmp92E[1]=Cyc_PP_line_doc(),((_tmp92E[0]=Cyc_Absynpp_decl2doc(
_tmp3E1),Cyc_PP_cat(_tag_dyneither(_tmp92E,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22B;_LL248: {struct Cyc_Absyn_Label_s_struct*_tmp3E3=(struct Cyc_Absyn_Label_s_struct*)
_tmp3BD;if(_tmp3E3->tag != 13)goto _LL24A;else{_tmp3E4=_tmp3E3->f1;_tmp3E5=_tmp3E3->f2;}}
_LL249: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3E5)){if(
stmtexp){const char*_tmp933;const char*_tmp932;struct Cyc_PP_Doc*_tmp931[8];s=((
_tmp931[7]=Cyc_PP_text(((_tmp932=");",_tag_dyneither(_tmp932,sizeof(char),3)))),((
_tmp931[6]=Cyc_Absynpp_rb(),((_tmp931[5]=Cyc_PP_line_doc(),((_tmp931[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3E5,1)),((_tmp931[3]=Cyc_PP_line_doc(),((_tmp931[2]=
Cyc_Absynpp_lb(),((_tmp931[1]=Cyc_PP_text(((_tmp933=": (",_tag_dyneither(_tmp933,
sizeof(char),4)))),((_tmp931[0]=Cyc_PP_textptr(_tmp3E4),Cyc_PP_cat(
_tag_dyneither(_tmp931,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{const
char*_tmp936;struct Cyc_PP_Doc*_tmp935[7];s=((_tmp935[6]=Cyc_Absynpp_rb(),((
_tmp935[5]=Cyc_PP_line_doc(),((_tmp935[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3E5,0)),((_tmp935[3]=Cyc_PP_line_doc(),((_tmp935[2]=Cyc_Absynpp_lb(),((
_tmp935[1]=Cyc_PP_text(((_tmp936=": ",_tag_dyneither(_tmp936,sizeof(char),3)))),((
_tmp935[0]=Cyc_PP_textptr(_tmp3E4),Cyc_PP_cat(_tag_dyneither(_tmp935,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}}else{const char*_tmp939;struct Cyc_PP_Doc*
_tmp938[3];s=((_tmp938[2]=Cyc_Absynpp_stmt2doc(_tmp3E5,stmtexp),((_tmp938[1]=Cyc_PP_text(((
_tmp939=": ",_tag_dyneither(_tmp939,sizeof(char),3)))),((_tmp938[0]=Cyc_PP_textptr(
_tmp3E4),Cyc_PP_cat(_tag_dyneither(_tmp938,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22B;_LL24A: {struct Cyc_Absyn_Do_s_struct*_tmp3E6=(struct Cyc_Absyn_Do_s_struct*)
_tmp3BD;if(_tmp3E6->tag != 14)goto _LL24C;else{_tmp3E7=_tmp3E6->f1;_tmp3E8=_tmp3E6->f2;
_tmp3E9=_tmp3E8.f1;}}_LL24B:{const char*_tmp940;const char*_tmp93F;const char*
_tmp93E;struct Cyc_PP_Doc*_tmp93D[9];s=((_tmp93D[8]=Cyc_PP_text(((_tmp93E=");",
_tag_dyneither(_tmp93E,sizeof(char),3)))),((_tmp93D[7]=Cyc_Absynpp_exp2doc(
_tmp3E9),((_tmp93D[6]=Cyc_PP_text(((_tmp93F=" while (",_tag_dyneither(_tmp93F,
sizeof(char),9)))),((_tmp93D[5]=Cyc_Absynpp_rb(),((_tmp93D[4]=Cyc_PP_line_doc(),((
_tmp93D[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3E7,0)),((_tmp93D[2]=Cyc_PP_line_doc(),((
_tmp93D[1]=Cyc_Absynpp_lb(),((_tmp93D[0]=Cyc_PP_text(((_tmp940="do ",
_tag_dyneither(_tmp940,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp93D,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL22B;_LL24C: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp3EA=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp3BD;if(_tmp3EA->tag != 15)goto
_LL24E;else{_tmp3EB=_tmp3EA->f1;_tmp3EC=_tmp3EA->f2;}}_LL24D:{const char*_tmp945;
const char*_tmp944;struct Cyc_PP_Doc*_tmp943[12];s=((_tmp943[11]=Cyc_Absynpp_rb(),((
_tmp943[10]=Cyc_PP_line_doc(),((_tmp943[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp3EC)),((_tmp943[8]=Cyc_PP_line_doc(),((_tmp943[7]=Cyc_Absynpp_lb(),((_tmp943[
6]=Cyc_PP_text(((_tmp944=" catch ",_tag_dyneither(_tmp944,sizeof(char),8)))),((
_tmp943[5]=Cyc_Absynpp_rb(),((_tmp943[4]=Cyc_PP_line_doc(),((_tmp943[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3EB,0)),((_tmp943[2]=Cyc_PP_line_doc(),((_tmp943[1]=
Cyc_Absynpp_lb(),((_tmp943[0]=Cyc_PP_text(((_tmp945="try ",_tag_dyneither(
_tmp945,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp943,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL22B;_LL24E: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp3ED=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp3BD;if(_tmp3ED->tag != 16)goto
_LL22B;else{_tmp3EE=_tmp3ED->f1;}}_LL24F:{const char*_tmp94A;const char*_tmp949;
struct Cyc_PP_Doc*_tmp948[3];s=((_tmp948[2]=Cyc_PP_text(((_tmp949=");",
_tag_dyneither(_tmp949,sizeof(char),3)))),((_tmp948[1]=Cyc_Absynpp_exp2doc(
_tmp3EE),((_tmp948[0]=Cyc_PP_text(((_tmp94A="reset_region(",_tag_dyneither(
_tmp94A,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp948,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL22B;_LL22B:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp433=p->r;enum Cyc_Absyn_Sign
_tmp437;int _tmp438;char _tmp43A;struct _dyneither_ptr _tmp43C;struct Cyc_Absyn_Vardecl*
_tmp43E;struct Cyc_Absyn_Pat*_tmp43F;struct Cyc_Absyn_Pat _tmp440;void*_tmp441;
struct Cyc_Absyn_Vardecl*_tmp444;struct Cyc_Absyn_Pat*_tmp445;struct Cyc_Absyn_Tvar*
_tmp447;struct Cyc_Absyn_Vardecl*_tmp448;struct Cyc_List_List*_tmp44A;int _tmp44B;
struct Cyc_Absyn_Pat*_tmp44D;struct Cyc_Absyn_Vardecl*_tmp44F;struct Cyc_Absyn_Pat*
_tmp450;struct Cyc_Absyn_Pat _tmp451;void*_tmp452;struct Cyc_Absyn_Vardecl*_tmp455;
struct Cyc_Absyn_Pat*_tmp456;struct _tuple0*_tmp458;struct _tuple0*_tmp45A;struct
Cyc_List_List*_tmp45B;int _tmp45C;struct Cyc_Absyn_AggrInfo*_tmp45E;struct Cyc_Absyn_AggrInfo
_tmp45F;union Cyc_Absyn_AggrInfoU _tmp460;struct Cyc_List_List*_tmp461;struct Cyc_List_List*
_tmp462;int _tmp463;struct Cyc_Absyn_AggrInfo*_tmp465;struct Cyc_List_List*_tmp466;
struct Cyc_List_List*_tmp467;int _tmp468;struct Cyc_Absyn_Enumfield*_tmp46A;struct
Cyc_Absyn_Enumfield*_tmp46C;struct Cyc_Absyn_Datatypefield*_tmp46E;struct Cyc_List_List*
_tmp46F;struct Cyc_Absyn_Datatypefield*_tmp471;struct Cyc_List_List*_tmp472;int
_tmp473;struct Cyc_Absyn_Exp*_tmp475;_LL256: {struct Cyc_Absyn_Wild_p_struct*
_tmp434=(struct Cyc_Absyn_Wild_p_struct*)_tmp433;if(_tmp434->tag != 0)goto _LL258;}
_LL257:{const char*_tmp94B;s=Cyc_PP_text(((_tmp94B="_",_tag_dyneither(_tmp94B,
sizeof(char),2))));}goto _LL255;_LL258: {struct Cyc_Absyn_Null_p_struct*_tmp435=(
struct Cyc_Absyn_Null_p_struct*)_tmp433;if(_tmp435->tag != 8)goto _LL25A;}_LL259:{
const char*_tmp94C;s=Cyc_PP_text(((_tmp94C="NULL",_tag_dyneither(_tmp94C,sizeof(
char),5))));}goto _LL255;_LL25A: {struct Cyc_Absyn_Int_p_struct*_tmp436=(struct Cyc_Absyn_Int_p_struct*)
_tmp433;if(_tmp436->tag != 9)goto _LL25C;else{_tmp437=_tmp436->f1;_tmp438=_tmp436->f2;}}
_LL25B: if(_tmp437 != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){const char*_tmp950;
void*_tmp94F[1];struct Cyc_Int_pa_struct _tmp94E;s=Cyc_PP_text((struct
_dyneither_ptr)((_tmp94E.tag=1,((_tmp94E.f1=(unsigned long)_tmp438,((_tmp94F[0]=&
_tmp94E,Cyc_aprintf(((_tmp950="%d",_tag_dyneither(_tmp950,sizeof(char),3))),
_tag_dyneither(_tmp94F,sizeof(void*),1)))))))));}else{const char*_tmp954;void*
_tmp953[1];struct Cyc_Int_pa_struct _tmp952;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp952.tag=1,((_tmp952.f1=(unsigned int)_tmp438,((_tmp953[0]=& _tmp952,Cyc_aprintf(((
_tmp954="%u",_tag_dyneither(_tmp954,sizeof(char),3))),_tag_dyneither(_tmp953,
sizeof(void*),1)))))))));}goto _LL255;_LL25C: {struct Cyc_Absyn_Char_p_struct*
_tmp439=(struct Cyc_Absyn_Char_p_struct*)_tmp433;if(_tmp439->tag != 10)goto _LL25E;
else{_tmp43A=_tmp439->f1;}}_LL25D:{const char*_tmp958;void*_tmp957[1];struct Cyc_String_pa_struct
_tmp956;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp956.tag=0,((_tmp956.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp43A)),((
_tmp957[0]=& _tmp956,Cyc_aprintf(((_tmp958="'%s'",_tag_dyneither(_tmp958,sizeof(
char),5))),_tag_dyneither(_tmp957,sizeof(void*),1)))))))));}goto _LL255;_LL25E: {
struct Cyc_Absyn_Float_p_struct*_tmp43B=(struct Cyc_Absyn_Float_p_struct*)_tmp433;
if(_tmp43B->tag != 11)goto _LL260;else{_tmp43C=_tmp43B->f1;}}_LL25F: s=Cyc_PP_text(
_tmp43C);goto _LL255;_LL260: {struct Cyc_Absyn_Var_p_struct*_tmp43D=(struct Cyc_Absyn_Var_p_struct*)
_tmp433;if(_tmp43D->tag != 1)goto _LL262;else{_tmp43E=_tmp43D->f1;_tmp43F=_tmp43D->f2;
_tmp440=*_tmp43F;_tmp441=_tmp440.r;{struct Cyc_Absyn_Wild_p_struct*_tmp442=(
struct Cyc_Absyn_Wild_p_struct*)_tmp441;if(_tmp442->tag != 0)goto _LL262;};}}_LL261:
s=Cyc_Absynpp_qvar2doc(_tmp43E->name);goto _LL255;_LL262: {struct Cyc_Absyn_Var_p_struct*
_tmp443=(struct Cyc_Absyn_Var_p_struct*)_tmp433;if(_tmp443->tag != 1)goto _LL264;
else{_tmp444=_tmp443->f1;_tmp445=_tmp443->f2;}}_LL263:{const char*_tmp95B;struct
Cyc_PP_Doc*_tmp95A[3];s=((_tmp95A[2]=Cyc_Absynpp_pat2doc(_tmp445),((_tmp95A[1]=
Cyc_PP_text(((_tmp95B=" as ",_tag_dyneither(_tmp95B,sizeof(char),5)))),((_tmp95A[
0]=Cyc_Absynpp_qvar2doc(_tmp444->name),Cyc_PP_cat(_tag_dyneither(_tmp95A,sizeof(
struct Cyc_PP_Doc*),3))))))));}goto _LL255;_LL264: {struct Cyc_Absyn_TagInt_p_struct*
_tmp446=(struct Cyc_Absyn_TagInt_p_struct*)_tmp433;if(_tmp446->tag != 3)goto _LL266;
else{_tmp447=_tmp446->f1;_tmp448=_tmp446->f2;}}_LL265:{const char*_tmp960;const
char*_tmp95F;struct Cyc_PP_Doc*_tmp95E[4];s=((_tmp95E[3]=Cyc_PP_text(((_tmp95F=">",
_tag_dyneither(_tmp95F,sizeof(char),2)))),((_tmp95E[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp447)),((_tmp95E[1]=Cyc_PP_text(((_tmp960="<",_tag_dyneither(_tmp960,sizeof(
char),2)))),((_tmp95E[0]=Cyc_Absynpp_qvar2doc(_tmp448->name),Cyc_PP_cat(
_tag_dyneither(_tmp95E,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL255;_LL266: {
struct Cyc_Absyn_Tuple_p_struct*_tmp449=(struct Cyc_Absyn_Tuple_p_struct*)_tmp433;
if(_tmp449->tag != 4)goto _LL268;else{_tmp44A=_tmp449->f1;_tmp44B=_tmp449->f2;}}
_LL267:{const char*_tmp969;const char*_tmp968;const char*_tmp967;const char*_tmp966;
struct Cyc_PP_Doc*_tmp965[4];s=((_tmp965[3]=_tmp44B?Cyc_PP_text(((_tmp966=", ...)",
_tag_dyneither(_tmp966,sizeof(char),7)))): Cyc_PP_text(((_tmp967=")",
_tag_dyneither(_tmp967,sizeof(char),2)))),((_tmp965[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp968=",",_tag_dyneither(_tmp968,
sizeof(char),2))),_tmp44A),((_tmp965[1]=Cyc_PP_text(((_tmp969="(",_tag_dyneither(
_tmp969,sizeof(char),2)))),((_tmp965[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp965,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL255;_LL268: {
struct Cyc_Absyn_Pointer_p_struct*_tmp44C=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp433;if(_tmp44C->tag != 5)goto _LL26A;else{_tmp44D=_tmp44C->f1;}}_LL269:{const
char*_tmp96C;struct Cyc_PP_Doc*_tmp96B[2];s=((_tmp96B[1]=Cyc_Absynpp_pat2doc(
_tmp44D),((_tmp96B[0]=Cyc_PP_text(((_tmp96C="&",_tag_dyneither(_tmp96C,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp96B,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;_LL26A: {struct Cyc_Absyn_Reference_p_struct*_tmp44E=(struct Cyc_Absyn_Reference_p_struct*)
_tmp433;if(_tmp44E->tag != 2)goto _LL26C;else{_tmp44F=_tmp44E->f1;_tmp450=_tmp44E->f2;
_tmp451=*_tmp450;_tmp452=_tmp451.r;{struct Cyc_Absyn_Wild_p_struct*_tmp453=(
struct Cyc_Absyn_Wild_p_struct*)_tmp452;if(_tmp453->tag != 0)goto _LL26C;};}}_LL26B:{
const char*_tmp96F;struct Cyc_PP_Doc*_tmp96E[2];s=((_tmp96E[1]=Cyc_Absynpp_qvar2doc(
_tmp44F->name),((_tmp96E[0]=Cyc_PP_text(((_tmp96F="*",_tag_dyneither(_tmp96F,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp96E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;_LL26C: {struct Cyc_Absyn_Reference_p_struct*_tmp454=(struct Cyc_Absyn_Reference_p_struct*)
_tmp433;if(_tmp454->tag != 2)goto _LL26E;else{_tmp455=_tmp454->f1;_tmp456=_tmp454->f2;}}
_LL26D:{const char*_tmp974;const char*_tmp973;struct Cyc_PP_Doc*_tmp972[4];s=((
_tmp972[3]=Cyc_Absynpp_pat2doc(_tmp456),((_tmp972[2]=Cyc_PP_text(((_tmp973=" as ",
_tag_dyneither(_tmp973,sizeof(char),5)))),((_tmp972[1]=Cyc_Absynpp_qvar2doc(
_tmp455->name),((_tmp972[0]=Cyc_PP_text(((_tmp974="*",_tag_dyneither(_tmp974,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp972,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL255;_LL26E: {struct Cyc_Absyn_UnknownId_p_struct*_tmp457=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp433;if(_tmp457->tag != 14)goto _LL270;else{_tmp458=_tmp457->f1;}}_LL26F: s=Cyc_Absynpp_qvar2doc(
_tmp458);goto _LL255;_LL270: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp459=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp433;if(_tmp459->tag != 15)goto _LL272;
else{_tmp45A=_tmp459->f1;_tmp45B=_tmp459->f2;_tmp45C=_tmp459->f3;}}_LL271: {
const char*_tmp976;const char*_tmp975;struct _dyneither_ptr term=_tmp45C?(_tmp976=", ...)",
_tag_dyneither(_tmp976,sizeof(char),7)):((_tmp975=")",_tag_dyneither(_tmp975,
sizeof(char),2)));{const char*_tmp97B;const char*_tmp97A;struct Cyc_PP_Doc*_tmp979[
2];s=((_tmp979[1]=Cyc_PP_group(((_tmp97B="(",_tag_dyneither(_tmp97B,sizeof(char),
2))),term,((_tmp97A=",",_tag_dyneither(_tmp97A,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp45B)),((_tmp979[0]=Cyc_Absynpp_qvar2doc(_tmp45A),Cyc_PP_cat(
_tag_dyneither(_tmp979,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL255;}_LL272: {
struct Cyc_Absyn_Aggr_p_struct*_tmp45D=(struct Cyc_Absyn_Aggr_p_struct*)_tmp433;
if(_tmp45D->tag != 6)goto _LL274;else{_tmp45E=_tmp45D->f1;if(_tmp45E == 0)goto
_LL274;_tmp45F=*_tmp45E;_tmp460=_tmp45F.aggr_info;_tmp461=_tmp45D->f2;_tmp462=
_tmp45D->f3;_tmp463=_tmp45D->f4;}}_LL273: {const char*_tmp97D;const char*_tmp97C;
struct _dyneither_ptr term=_tmp463?(_tmp97D=", ...}",_tag_dyneither(_tmp97D,
sizeof(char),7)):((_tmp97C="}",_tag_dyneither(_tmp97C,sizeof(char),2)));struct
_tuple0*_tmp498;struct _tuple9 _tmp497=Cyc_Absyn_aggr_kinded_name(_tmp460);_tmp498=
_tmp497.f2;{const char*_tmp988;const char*_tmp987;const char*_tmp986;const char*
_tmp985;const char*_tmp984;struct Cyc_PP_Doc*_tmp983[4];s=((_tmp983[3]=Cyc_PP_group(((
_tmp985="",_tag_dyneither(_tmp985,sizeof(char),1))),term,((_tmp984=",",
_tag_dyneither(_tmp984,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp462)),((_tmp983[2]=Cyc_PP_egroup(((_tmp988="[",_tag_dyneither(_tmp988,
sizeof(char),2))),((_tmp987="]",_tag_dyneither(_tmp987,sizeof(char),2))),((
_tmp986=",",_tag_dyneither(_tmp986,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp461))),((_tmp983[1]=
Cyc_Absynpp_lb(),((_tmp983[0]=Cyc_Absynpp_qvar2doc(_tmp498),Cyc_PP_cat(
_tag_dyneither(_tmp983,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL255;}_LL274: {
struct Cyc_Absyn_Aggr_p_struct*_tmp464=(struct Cyc_Absyn_Aggr_p_struct*)_tmp433;
if(_tmp464->tag != 6)goto _LL276;else{_tmp465=_tmp464->f1;if(_tmp465 != 0)goto
_LL276;_tmp466=_tmp464->f2;_tmp467=_tmp464->f3;_tmp468=_tmp464->f4;}}_LL275: {
const char*_tmp98A;const char*_tmp989;struct _dyneither_ptr term=_tmp468?(_tmp98A=", ...}",
_tag_dyneither(_tmp98A,sizeof(char),7)):((_tmp989="}",_tag_dyneither(_tmp989,
sizeof(char),2)));{const char*_tmp995;const char*_tmp994;const char*_tmp993;const
char*_tmp992;const char*_tmp991;struct Cyc_PP_Doc*_tmp990[3];s=((_tmp990[2]=Cyc_PP_group(((
_tmp992="",_tag_dyneither(_tmp992,sizeof(char),1))),term,((_tmp991=",",
_tag_dyneither(_tmp991,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp467)),((_tmp990[1]=Cyc_PP_egroup(((_tmp995="[",_tag_dyneither(_tmp995,
sizeof(char),2))),((_tmp994="]",_tag_dyneither(_tmp994,sizeof(char),2))),((
_tmp993=",",_tag_dyneither(_tmp993,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp466))),((_tmp990[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp990,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL255;}_LL276: {struct Cyc_Absyn_Enum_p_struct*_tmp469=(struct Cyc_Absyn_Enum_p_struct*)
_tmp433;if(_tmp469->tag != 12)goto _LL278;else{_tmp46A=_tmp469->f2;}}_LL277:
_tmp46C=_tmp46A;goto _LL279;_LL278: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp46B=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp433;if(_tmp46B->tag != 13)goto _LL27A;else{
_tmp46C=_tmp46B->f2;}}_LL279: s=Cyc_Absynpp_qvar2doc(_tmp46C->name);goto _LL255;
_LL27A: {struct Cyc_Absyn_Datatype_p_struct*_tmp46D=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp433;if(_tmp46D->tag != 7)goto _LL27C;else{_tmp46E=_tmp46D->f2;_tmp46F=_tmp46D->f3;
if(_tmp46F != 0)goto _LL27C;}}_LL27B: s=Cyc_Absynpp_qvar2doc(_tmp46E->name);goto
_LL255;_LL27C: {struct Cyc_Absyn_Datatype_p_struct*_tmp470=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp433;if(_tmp470->tag != 7)goto _LL27E;else{_tmp471=_tmp470->f2;_tmp472=_tmp470->f3;
_tmp473=_tmp470->f4;}}_LL27D: {const char*_tmp997;const char*_tmp996;struct
_dyneither_ptr term=_tmp473?(_tmp997=", ...)",_tag_dyneither(_tmp997,sizeof(char),
7)):((_tmp996=")",_tag_dyneither(_tmp996,sizeof(char),2)));{const char*_tmp99C;
const char*_tmp99B;struct Cyc_PP_Doc*_tmp99A[2];s=((_tmp99A[1]=Cyc_PP_egroup(((
_tmp99C="(",_tag_dyneither(_tmp99C,sizeof(char),2))),term,((_tmp99B=",",
_tag_dyneither(_tmp99B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp472)),((_tmp99A[0]=Cyc_Absynpp_qvar2doc(_tmp471->name),Cyc_PP_cat(
_tag_dyneither(_tmp99A,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL255;}_LL27E: {
struct Cyc_Absyn_Exp_p_struct*_tmp474=(struct Cyc_Absyn_Exp_p_struct*)_tmp433;if(
_tmp474->tag != 16)goto _LL255;else{_tmp475=_tmp474->f1;}}_LL27F: s=Cyc_Absynpp_exp2doc(
_tmp475);goto _LL255;_LL255:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple13*dp){const char*_tmp9A3;const char*_tmp9A2;const char*_tmp9A1;struct Cyc_PP_Doc*
_tmp9A0[2];return(_tmp9A0[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp9A0[0]=Cyc_PP_egroup(((
_tmp9A3="",_tag_dyneither(_tmp9A3,sizeof(char),1))),((_tmp9A2="=",_tag_dyneither(
_tmp9A2,sizeof(char),2))),((_tmp9A1="=",_tag_dyneither(_tmp9A1,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp9A0,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (c->pattern)->r == (void*)&
Cyc_Absyn_Wild_p_val){const char*_tmp9A8;struct Cyc_PP_Doc*_tmp9A7[2];struct Cyc_PP_Doc*
_tmp9A6[2];return(_tmp9A6[1]=Cyc_PP_nest(2,((_tmp9A7[1]=Cyc_Absynpp_stmt2doc(c->body,
0),((_tmp9A7[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9A7,sizeof(
struct Cyc_PP_Doc*),2))))))),((_tmp9A6[0]=Cyc_PP_text(((_tmp9A8="default: ",
_tag_dyneither(_tmp9A8,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp9A6,
sizeof(struct Cyc_PP_Doc*),2)))));}else{if(c->where_clause == 0){const char*_tmp9AF;
const char*_tmp9AE;struct Cyc_PP_Doc*_tmp9AD[2];struct Cyc_PP_Doc*_tmp9AC[4];return(
_tmp9AC[3]=Cyc_PP_nest(2,((_tmp9AD[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9AD[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9AD,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp9AC[2]=Cyc_PP_text(((_tmp9AE=": ",_tag_dyneither(_tmp9AE,sizeof(
char),3)))),((_tmp9AC[1]=Cyc_Absynpp_pat2doc(c->pattern),((_tmp9AC[0]=Cyc_PP_text(((
_tmp9AF="case ",_tag_dyneither(_tmp9AF,sizeof(char),6)))),Cyc_PP_cat(
_tag_dyneither(_tmp9AC,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{const char*
_tmp9B8;const char*_tmp9B7;const char*_tmp9B6;struct Cyc_PP_Doc*_tmp9B5[2];struct
Cyc_PP_Doc*_tmp9B4[6];return(_tmp9B4[5]=Cyc_PP_nest(2,((_tmp9B5[1]=Cyc_Absynpp_stmt2doc(
c->body,0),((_tmp9B5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9B5,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9B4[4]=Cyc_PP_text(((_tmp9B6=": ",
_tag_dyneither(_tmp9B6,sizeof(char),3)))),((_tmp9B4[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp9B4[2]=Cyc_PP_text(((
_tmp9B7=" && ",_tag_dyneither(_tmp9B7,sizeof(char),5)))),((_tmp9B4[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp9B4[0]=Cyc_PP_text(((_tmp9B8="case ",_tag_dyneither(_tmp9B8,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9B4,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*
_tmp9B9;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp9B9="",_tag_dyneither(_tmp9B9,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);
else{const char*_tmp9BC;struct Cyc_PP_Doc*_tmp9BB[3];return(_tmp9BB[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp9BB[1]=Cyc_PP_text(((_tmp9BC=" = ",
_tag_dyneither(_tmp9BC,sizeof(char),4)))),((_tmp9BB[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp9BB,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){const char*_tmp9BD;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((
_tmp9BD=",",_tag_dyneither(_tmp9BD,sizeof(char),2))),fs);}static struct Cyc_PP_Doc*
Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(v->name);}
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){const char*
_tmp9BE;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((
_tmp9BE=",",_tag_dyneither(_tmp9BE,sizeof(char),2))),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(
struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl _tmp4C4;enum Cyc_Absyn_Scope
_tmp4C5;struct _tuple0*_tmp4C6;struct Cyc_Absyn_Tqual _tmp4C7;void*_tmp4C8;struct
Cyc_Absyn_Exp*_tmp4C9;struct Cyc_List_List*_tmp4CA;struct Cyc_Absyn_Vardecl*
_tmp4C3=vd;_tmp4C4=*_tmp4C3;_tmp4C5=_tmp4C4.sc;_tmp4C6=_tmp4C4.name;_tmp4C7=
_tmp4C4.tq;_tmp4C8=_tmp4C4.type;_tmp4C9=_tmp4C4.initializer;_tmp4CA=_tmp4C4.attributes;{
struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp4C6);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4CA);struct Cyc_PP_Doc*
beforenamedoc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL280:
beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL281:{void*_tmp4CB=Cyc_Tcutil_compress(
_tmp4C8);struct Cyc_Absyn_FnInfo _tmp4CD;struct Cyc_List_List*_tmp4CE;_LL284: {
struct Cyc_Absyn_FnType_struct*_tmp4CC=(struct Cyc_Absyn_FnType_struct*)_tmp4CB;
if(_tmp4CC->tag != 10)goto _LL286;else{_tmp4CD=_tmp4CC->f1;_tmp4CE=_tmp4CD.attributes;}}
_LL285: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4CE);goto _LL283;_LL286:;_LL287:
beforenamedoc=Cyc_PP_nil_doc();goto _LL283;_LL283:;}break;}{struct Cyc_PP_Doc*
tmp_doc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL288: tmp_doc=Cyc_PP_nil_doc();
break;case Cyc_Cyclone_Vc_c: _LL289: tmp_doc=attsdoc;break;}{struct Cyc_Core_Opt*
_tmp9CB;struct Cyc_PP_Doc*_tmp9CA[2];struct Cyc_PP_Doc*_tmp9C9[2];const char*
_tmp9C8;const char*_tmp9C7;struct Cyc_PP_Doc*_tmp9C6[5];s=((_tmp9C6[4]=Cyc_PP_text(((
_tmp9C7=";",_tag_dyneither(_tmp9C7,sizeof(char),2)))),((_tmp9C6[3]=_tmp4C9 == 0?
Cyc_PP_nil_doc():((_tmp9C9[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4C9),((
_tmp9C9[0]=Cyc_PP_text(((_tmp9C8=" = ",_tag_dyneither(_tmp9C8,sizeof(char),4)))),
Cyc_PP_cat(_tag_dyneither(_tmp9C9,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9C6[2]=
Cyc_Absynpp_tqtd2doc(_tmp4C7,_tmp4C8,((_tmp9CB=_cycalloc(sizeof(*_tmp9CB)),((
_tmp9CB->v=((_tmp9CA[1]=sn,((_tmp9CA[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(
_tmp9CA,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9CB))))),((_tmp9C6[1]=Cyc_Absynpp_scope2doc(
_tmp4C5),((_tmp9C6[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp9C6,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}return s;};};}struct _tuple17{struct Cyc_Position_Segment*f1;struct
_tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple0*_tmp4D6;struct _tuple17 _tmp4D5=*x;_tmp4D6=_tmp4D5.f2;return Cyc_Absynpp_qvar2doc(
_tmp4D6);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){struct Cyc_PP_Doc*_tmp9CC[4];return(_tmp9CC[3]=Cyc_Absynpp_ktvars2doc(
ad->tvs),((_tmp9CC[2]=Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9CC[1]=Cyc_Absynpp_aggr_kind2doc(
ad->kind),((_tmp9CC[0]=Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(
_tmp9CC,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{struct Cyc_PP_Doc*_tmp9D4[2];
const char*_tmp9D3;struct Cyc_PP_Doc*_tmp9D2[2];struct Cyc_PP_Doc*_tmp9D1[12];
return(_tmp9D1[11]=Cyc_Absynpp_atts2doc(ad->attributes),((_tmp9D1[10]=Cyc_Absynpp_rb(),((
_tmp9D1[9]=Cyc_PP_line_doc(),((_tmp9D1[8]=Cyc_PP_nest(2,((_tmp9D2[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmp9D2[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp9D2,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9D1[7]=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():((
_tmp9D4[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((
_tmp9D4[0]=Cyc_PP_text(((_tmp9D3=":",_tag_dyneither(_tmp9D3,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp9D4,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9D1[6]=
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((
_tmp9D1[5]=Cyc_Absynpp_lb(),((_tmp9D1[4]=Cyc_PP_blank_doc(),((_tmp9D1[3]=Cyc_Absynpp_ktvars2doc(
ad->tvs),((_tmp9D1[2]=Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9D1[1]=Cyc_Absynpp_aggr_kind2doc(
ad->kind),((_tmp9D1[0]=Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(
_tmp9D1,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){struct Cyc_Absyn_Datatypedecl
_tmp4DD;enum Cyc_Absyn_Scope _tmp4DE;struct _tuple0*_tmp4DF;struct Cyc_List_List*
_tmp4E0;struct Cyc_Core_Opt*_tmp4E1;int _tmp4E2;struct Cyc_Absyn_Datatypedecl*
_tmp4DC=dd;_tmp4DD=*_tmp4DC;_tmp4DE=_tmp4DD.sc;_tmp4DF=_tmp4DD.name;_tmp4E0=
_tmp4DD.tvs;_tmp4E1=_tmp4DD.fields;_tmp4E2=_tmp4DD.is_extensible;if(_tmp4E1 == 0){
const char*_tmp9D9;const char*_tmp9D8;struct Cyc_PP_Doc*_tmp9D7[5];return(_tmp9D7[4]=
Cyc_Absynpp_ktvars2doc(_tmp4E0),((_tmp9D7[3]=_tmp4E2?Cyc_Absynpp_qvar2bolddoc(
_tmp4DF): Cyc_Absynpp_typedef_name2bolddoc(_tmp4DF),((_tmp9D7[2]=Cyc_PP_text(((
_tmp9D8="datatype ",_tag_dyneither(_tmp9D8,sizeof(char),10)))),((_tmp9D7[1]=
_tmp4E2?Cyc_PP_text(((_tmp9D9="@extensible",_tag_dyneither(_tmp9D9,sizeof(char),
12)))): Cyc_PP_blank_doc(),((_tmp9D7[0]=Cyc_Absynpp_scope2doc(_tmp4DE),Cyc_PP_cat(
_tag_dyneither(_tmp9D7,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{const char*
_tmp9E0;const char*_tmp9DF;struct Cyc_PP_Doc*_tmp9DE[2];struct Cyc_PP_Doc*_tmp9DD[
10];return(_tmp9DD[9]=Cyc_Absynpp_rb(),((_tmp9DD[8]=Cyc_PP_line_doc(),((_tmp9DD[
7]=Cyc_PP_nest(2,((_tmp9DE[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)
_tmp4E1->v),((_tmp9DE[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9DE,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9DD[6]=Cyc_Absynpp_lb(),((_tmp9DD[5]=
Cyc_PP_blank_doc(),((_tmp9DD[4]=Cyc_Absynpp_ktvars2doc(_tmp4E0),((_tmp9DD[3]=
_tmp4E2?Cyc_Absynpp_qvar2bolddoc(_tmp4DF): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4DF),((_tmp9DD[2]=Cyc_PP_text(((_tmp9DF="datatype ",_tag_dyneither(_tmp9DF,
sizeof(char),10)))),((_tmp9DD[1]=_tmp4E2?Cyc_PP_text(((_tmp9E0="@extensible ",
_tag_dyneither(_tmp9E0,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9DD[0]=Cyc_Absynpp_scope2doc(
_tmp4DE),Cyc_PP_cat(_tag_dyneither(_tmp9DD,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){struct Cyc_Absyn_Enumdecl
_tmp4EB;enum Cyc_Absyn_Scope _tmp4EC;struct _tuple0*_tmp4ED;struct Cyc_Core_Opt*
_tmp4EE;struct Cyc_Absyn_Enumdecl*_tmp4EA=ed;_tmp4EB=*_tmp4EA;_tmp4EC=_tmp4EB.sc;
_tmp4ED=_tmp4EB.name;_tmp4EE=_tmp4EB.fields;if(_tmp4EE == 0){const char*_tmp9E3;
struct Cyc_PP_Doc*_tmp9E2[3];return(_tmp9E2[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp4ED),((_tmp9E2[1]=Cyc_PP_text(((_tmp9E3="enum ",_tag_dyneither(_tmp9E3,
sizeof(char),6)))),((_tmp9E2[0]=Cyc_Absynpp_scope2doc(_tmp4EC),Cyc_PP_cat(
_tag_dyneither(_tmp9E2,sizeof(struct Cyc_PP_Doc*),3)))))));}else{const char*
_tmp9E8;struct Cyc_PP_Doc*_tmp9E7[2];struct Cyc_PP_Doc*_tmp9E6[8];return(_tmp9E6[7]=
Cyc_Absynpp_rb(),((_tmp9E6[6]=Cyc_PP_line_doc(),((_tmp9E6[5]=Cyc_PP_nest(2,((
_tmp9E7[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp4EE->v),((
_tmp9E7[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9E7,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp9E6[4]=Cyc_Absynpp_lb(),((_tmp9E6[3]=Cyc_PP_blank_doc(),((_tmp9E6[
2]=Cyc_Absynpp_qvar2bolddoc(_tmp4ED),((_tmp9E6[1]=Cyc_PP_text(((_tmp9E8="enum ",
_tag_dyneither(_tmp9E8,sizeof(char),6)))),((_tmp9E6[0]=Cyc_Absynpp_scope2doc(
_tmp4EC),Cyc_PP_cat(_tag_dyneither(_tmp9E6,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{
void*_tmp4F4=d->r;struct Cyc_Absyn_Fndecl*_tmp4F6;struct Cyc_Absyn_Aggrdecl*
_tmp4F8;struct Cyc_Absyn_Vardecl*_tmp4FA;struct Cyc_Absyn_Tvar*_tmp4FC;struct Cyc_Absyn_Vardecl*
_tmp4FD;int _tmp4FE;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_Absyn_Exp*_tmp501;
struct Cyc_Absyn_Tvar*_tmp502;struct Cyc_Absyn_Vardecl*_tmp503;struct Cyc_Absyn_Datatypedecl*
_tmp505;struct Cyc_Absyn_Pat*_tmp507;struct Cyc_Absyn_Exp*_tmp508;struct Cyc_List_List*
_tmp50A;struct Cyc_Absyn_Enumdecl*_tmp50C;struct Cyc_Absyn_Typedefdecl*_tmp50E;
struct _dyneither_ptr*_tmp510;struct Cyc_List_List*_tmp511;struct _tuple0*_tmp513;
struct Cyc_List_List*_tmp514;struct Cyc_List_List*_tmp516;struct Cyc_List_List*
_tmp518;struct Cyc_List_List*_tmp519;_LL28C: {struct Cyc_Absyn_Fn_d_struct*_tmp4F5=(
struct Cyc_Absyn_Fn_d_struct*)_tmp4F4;if(_tmp4F5->tag != 1)goto _LL28E;else{_tmp4F6=
_tmp4F5->f1;}}_LL28D: {struct Cyc_Absyn_FnType_struct _tmp9EE;struct Cyc_Absyn_FnInfo
_tmp9ED;struct Cyc_Absyn_FnType_struct*_tmp9EC;void*t=(void*)((_tmp9EC=_cycalloc(
sizeof(*_tmp9EC)),((_tmp9EC[0]=((_tmp9EE.tag=10,((_tmp9EE.f1=((_tmp9ED.tvars=
_tmp4F6->tvs,((_tmp9ED.effect=_tmp4F6->effect,((_tmp9ED.ret_tqual=_tmp4F6->ret_tqual,((
_tmp9ED.ret_typ=_tmp4F6->ret_type,((_tmp9ED.args=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp4F6->args),((_tmp9ED.c_varargs=_tmp4F6->c_varargs,((_tmp9ED.cyc_varargs=
_tmp4F6->cyc_varargs,((_tmp9ED.rgn_po=_tmp4F6->rgn_po,((_tmp9ED.attributes=0,
_tmp9ED)))))))))))))))))),_tmp9EE)))),_tmp9EC))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp4F6->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp4F6->is_inline)switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL2AC:{const char*_tmp9EF;inlinedoc=Cyc_PP_text(((_tmp9EF="inline ",
_tag_dyneither(_tmp9EF,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2AD:{
const char*_tmp9F0;inlinedoc=Cyc_PP_text(((_tmp9F0="__inline ",_tag_dyneither(
_tmp9F0,sizeof(char),10))));}break;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc(_tmp4F6->sc);struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2AF: beforenamedoc=
attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2B0: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp4F6->attributes);break;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp4F6->name);struct Cyc_PP_Doc*_tmp9F3[2];struct Cyc_Core_Opt*_tmp9F2;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp9F2=_cycalloc(
sizeof(*_tmp9F2)),((_tmp9F2->v=((_tmp9F3[1]=namedoc,((_tmp9F3[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmp9F3,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9F2)))));
struct Cyc_PP_Doc*_tmp9F6[2];struct Cyc_PP_Doc*_tmp9F5[5];struct Cyc_PP_Doc*bodydoc=(
_tmp9F5[4]=Cyc_Absynpp_rb(),((_tmp9F5[3]=Cyc_PP_line_doc(),((_tmp9F5[2]=Cyc_PP_nest(
2,((_tmp9F6[1]=Cyc_Absynpp_stmt2doc(_tmp4F6->body,0),((_tmp9F6[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp9F6,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9F5[1]=
Cyc_Absynpp_lb(),((_tmp9F5[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmp9F5,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp9F7[4];s=((
_tmp9F7[3]=bodydoc,((_tmp9F7[2]=tqtddoc,((_tmp9F7[1]=scopedoc,((_tmp9F7[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9F7,sizeof(struct Cyc_PP_Doc*),4))))))))));}
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2B2:{struct Cyc_PP_Doc*
_tmp9F8[2];s=((_tmp9F8[1]=s,((_tmp9F8[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(
_tmp9F8,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2B3: break;}goto
_LL28B;};};}_LL28E: {struct Cyc_Absyn_Aggr_d_struct*_tmp4F7=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4F4;if(_tmp4F7->tag != 6)goto _LL290;else{_tmp4F8=_tmp4F7->f1;}}_LL28F:{const
char*_tmp9FB;struct Cyc_PP_Doc*_tmp9FA[2];s=((_tmp9FA[1]=Cyc_PP_text(((_tmp9FB=";",
_tag_dyneither(_tmp9FB,sizeof(char),2)))),((_tmp9FA[0]=Cyc_Absynpp_aggrdecl2doc(
_tmp4F8),Cyc_PP_cat(_tag_dyneither(_tmp9FA,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL290: {struct Cyc_Absyn_Var_d_struct*_tmp4F9=(struct Cyc_Absyn_Var_d_struct*)
_tmp4F4;if(_tmp4F9->tag != 0)goto _LL292;else{_tmp4FA=_tmp4F9->f1;}}_LL291: s=Cyc_Absynpp_vardecl2doc(
_tmp4FA);goto _LL28B;_LL292: {struct Cyc_Absyn_Region_d_struct*_tmp4FB=(struct Cyc_Absyn_Region_d_struct*)
_tmp4F4;if(_tmp4FB->tag != 4)goto _LL294;else{_tmp4FC=_tmp4FB->f1;_tmp4FD=_tmp4FB->f2;
_tmp4FE=_tmp4FB->f3;_tmp4FF=_tmp4FB->f4;}}_LL293:{const char*_tmpA0E;const char*
_tmpA0D;const char*_tmpA0C;const char*_tmpA0B;struct Cyc_PP_Doc*_tmpA0A[3];const
char*_tmpA09;const char*_tmpA08;const char*_tmpA07;struct Cyc_PP_Doc*_tmpA06[8];s=((
_tmpA06[7]=Cyc_PP_text(((_tmpA07=";",_tag_dyneither(_tmpA07,sizeof(char),2)))),((
_tmpA06[6]=_tmp4FF != 0?(_tmpA0A[2]=Cyc_PP_text(((_tmpA09=")",_tag_dyneither(
_tmpA09,sizeof(char),2)))),((_tmpA0A[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp4FF),((_tmpA0A[0]=Cyc_PP_text(((_tmpA08=" = open(",_tag_dyneither(_tmpA08,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA0A,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmpA06[5]=_tmp4FE?Cyc_PP_text(((_tmpA0B=" @resetable",
_tag_dyneither(_tmpA0B,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmpA06[4]=Cyc_Absynpp_qvar2doc(
_tmp4FD->name),((_tmpA06[3]=Cyc_PP_text(((_tmpA0C=">",_tag_dyneither(_tmpA0C,
sizeof(char),2)))),((_tmpA06[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4FC)),((
_tmpA06[1]=Cyc_PP_text(((_tmpA0D="<",_tag_dyneither(_tmpA0D,sizeof(char),2)))),((
_tmpA06[0]=Cyc_PP_text(((_tmpA0E="region",_tag_dyneither(_tmpA0E,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmpA06,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL28B;_LL294: {struct Cyc_Absyn_Alias_d_struct*_tmp500=(struct Cyc_Absyn_Alias_d_struct*)
_tmp4F4;if(_tmp500->tag != 5)goto _LL296;else{_tmp501=_tmp500->f1;_tmp502=_tmp500->f2;
_tmp503=_tmp500->f3;}}_LL295:{const char*_tmpA19;const char*_tmpA18;const char*
_tmpA17;const char*_tmpA16;const char*_tmpA15;struct Cyc_PP_Doc*_tmpA14[8];s=((
_tmpA14[7]=Cyc_PP_text(((_tmpA15=";",_tag_dyneither(_tmpA15,sizeof(char),2)))),((
_tmpA14[6]=Cyc_Absynpp_exp2doc(_tmp501),((_tmpA14[5]=Cyc_PP_text(((_tmpA16=" = ",
_tag_dyneither(_tmpA16,sizeof(char),4)))),((_tmpA14[4]=Cyc_Absynpp_qvar2doc(
_tmp503->name),((_tmpA14[3]=Cyc_PP_text(((_tmpA17="> ",_tag_dyneither(_tmpA17,
sizeof(char),3)))),((_tmpA14[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp502)),((
_tmpA14[1]=Cyc_PP_text(((_tmpA18=" <",_tag_dyneither(_tmpA18,sizeof(char),3)))),((
_tmpA14[0]=Cyc_PP_text(((_tmpA19="alias",_tag_dyneither(_tmpA19,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmpA14,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL28B;_LL296: {struct Cyc_Absyn_Datatype_d_struct*_tmp504=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp4F4;if(_tmp504->tag != 7)goto _LL298;else{_tmp505=_tmp504->f1;}}_LL297:{const
char*_tmpA1C;struct Cyc_PP_Doc*_tmpA1B[2];s=((_tmpA1B[1]=Cyc_PP_text(((_tmpA1C=";",
_tag_dyneither(_tmpA1C,sizeof(char),2)))),((_tmpA1B[0]=Cyc_Absynpp_datatypedecl2doc(
_tmp505),Cyc_PP_cat(_tag_dyneither(_tmpA1B,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL298: {struct Cyc_Absyn_Let_d_struct*_tmp506=(struct Cyc_Absyn_Let_d_struct*)
_tmp4F4;if(_tmp506->tag != 2)goto _LL29A;else{_tmp507=_tmp506->f1;_tmp508=_tmp506->f3;}}
_LL299:{const char*_tmpA23;const char*_tmpA22;const char*_tmpA21;struct Cyc_PP_Doc*
_tmpA20[5];s=((_tmpA20[4]=Cyc_PP_text(((_tmpA21=";",_tag_dyneither(_tmpA21,
sizeof(char),2)))),((_tmpA20[3]=Cyc_Absynpp_exp2doc(_tmp508),((_tmpA20[2]=Cyc_PP_text(((
_tmpA22=" = ",_tag_dyneither(_tmpA22,sizeof(char),4)))),((_tmpA20[1]=Cyc_Absynpp_pat2doc(
_tmp507),((_tmpA20[0]=Cyc_PP_text(((_tmpA23="let ",_tag_dyneither(_tmpA23,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA20,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL28B;_LL29A: {struct Cyc_Absyn_Letv_d_struct*_tmp509=(struct Cyc_Absyn_Letv_d_struct*)
_tmp4F4;if(_tmp509->tag != 3)goto _LL29C;else{_tmp50A=_tmp509->f1;}}_LL29B:{const
char*_tmpA28;const char*_tmpA27;struct Cyc_PP_Doc*_tmpA26[3];s=((_tmpA26[2]=Cyc_PP_text(((
_tmpA27=";",_tag_dyneither(_tmpA27,sizeof(char),2)))),((_tmpA26[1]=Cyc_Absynpp_ids2doc(
_tmp50A),((_tmpA26[0]=Cyc_PP_text(((_tmpA28="let ",_tag_dyneither(_tmpA28,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA26,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL28B;_LL29C: {struct Cyc_Absyn_Enum_d_struct*_tmp50B=(struct Cyc_Absyn_Enum_d_struct*)
_tmp4F4;if(_tmp50B->tag != 8)goto _LL29E;else{_tmp50C=_tmp50B->f1;}}_LL29D:{const
char*_tmpA2B;struct Cyc_PP_Doc*_tmpA2A[2];s=((_tmpA2A[1]=Cyc_PP_text(((_tmpA2B=";",
_tag_dyneither(_tmpA2B,sizeof(char),2)))),((_tmpA2A[0]=Cyc_Absynpp_enumdecl2doc(
_tmp50C),Cyc_PP_cat(_tag_dyneither(_tmpA2A,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL29E: {struct Cyc_Absyn_Typedef_d_struct*_tmp50D=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp4F4;if(_tmp50D->tag != 9)goto _LL2A0;else{_tmp50E=_tmp50D->f1;}}_LL29F: {void*
t;if(_tmp50E->defn != 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp50E->defn))->v;
else{t=Cyc_Absyn_new_evar(_tmp50E->kind,0);}{const char*_tmpA35;struct Cyc_Core_Opt*
_tmpA34;struct Cyc_PP_Doc*_tmpA33[2];const char*_tmpA32;struct Cyc_PP_Doc*_tmpA31[4];
s=((_tmpA31[3]=Cyc_PP_text(((_tmpA32=";",_tag_dyneither(_tmpA32,sizeof(char),2)))),((
_tmpA31[2]=Cyc_Absynpp_atts2doc(_tmp50E->atts),((_tmpA31[1]=Cyc_Absynpp_tqtd2doc(
_tmp50E->tq,t,((_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34->v=((_tmpA33[1]=
Cyc_Absynpp_tvars2doc(_tmp50E->tvs),((_tmpA33[0]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp50E->name),Cyc_PP_cat(_tag_dyneither(_tmpA33,sizeof(struct Cyc_PP_Doc*),2)))))),
_tmpA34))))),((_tmpA31[0]=Cyc_PP_text(((_tmpA35="typedef ",_tag_dyneither(
_tmpA35,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA31,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL28B;}_LL2A0: {struct Cyc_Absyn_Namespace_d_struct*_tmp50F=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp4F4;if(_tmp50F->tag != 10)goto _LL2A2;
else{_tmp510=_tmp50F->f1;_tmp511=_tmp50F->f2;}}_LL2A1: if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_add(_tmp510);{const char*_tmpA3A;const char*_tmpA39;
struct Cyc_PP_Doc*_tmpA38[8];s=((_tmpA38[7]=Cyc_Absynpp_rb(),((_tmpA38[6]=Cyc_PP_line_doc(),((
_tmpA38[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA39="",_tag_dyneither(_tmpA39,sizeof(char),1))),_tmp511),((_tmpA38[4]=Cyc_PP_line_doc(),((
_tmpA38[3]=Cyc_Absynpp_lb(),((_tmpA38[2]=Cyc_PP_blank_doc(),((_tmpA38[1]=Cyc_PP_textptr(
_tmp510),((_tmpA38[0]=Cyc_PP_text(((_tmpA3A="namespace ",_tag_dyneither(_tmpA3A,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA38,sizeof(struct Cyc_PP_Doc*),
8))))))))))))))))));}if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL28B;_LL2A2: {struct Cyc_Absyn_Using_d_struct*_tmp512=(struct Cyc_Absyn_Using_d_struct*)
_tmp4F4;if(_tmp512->tag != 11)goto _LL2A4;else{_tmp513=_tmp512->f1;_tmp514=_tmp512->f2;}}
_LL2A3: if(Cyc_Absynpp_print_using_stmts){const char*_tmpA3F;const char*_tmpA3E;
struct Cyc_PP_Doc*_tmpA3D[8];s=((_tmpA3D[7]=Cyc_Absynpp_rb(),((_tmpA3D[6]=Cyc_PP_line_doc(),((
_tmpA3D[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA3E="",_tag_dyneither(_tmpA3E,sizeof(char),1))),_tmp514),((_tmpA3D[4]=Cyc_PP_line_doc(),((
_tmpA3D[3]=Cyc_Absynpp_lb(),((_tmpA3D[2]=Cyc_PP_blank_doc(),((_tmpA3D[1]=Cyc_Absynpp_qvar2doc(
_tmp513),((_tmpA3D[0]=Cyc_PP_text(((_tmpA3F="using ",_tag_dyneither(_tmpA3F,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA3D,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmpA4A;const char*_tmpA49;const char*_tmpA48;const char*_tmpA47;
const char*_tmpA46;struct Cyc_PP_Doc*_tmpA45[11];s=((_tmpA45[10]=Cyc_PP_text(((
_tmpA46=" */",_tag_dyneither(_tmpA46,sizeof(char),4)))),((_tmpA45[9]=Cyc_Absynpp_rb(),((
_tmpA45[8]=Cyc_PP_text(((_tmpA47="/* ",_tag_dyneither(_tmpA47,sizeof(char),4)))),((
_tmpA45[7]=Cyc_PP_line_doc(),((_tmpA45[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA48="",_tag_dyneither(_tmpA48,sizeof(char),1))),
_tmp514),((_tmpA45[5]=Cyc_PP_line_doc(),((_tmpA45[4]=Cyc_PP_text(((_tmpA49=" */",
_tag_dyneither(_tmpA49,sizeof(char),4)))),((_tmpA45[3]=Cyc_Absynpp_lb(),((
_tmpA45[2]=Cyc_PP_blank_doc(),((_tmpA45[1]=Cyc_Absynpp_qvar2doc(_tmp513),((
_tmpA45[0]=Cyc_PP_text(((_tmpA4A="/* using ",_tag_dyneither(_tmpA4A,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmpA45,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL28B;_LL2A4: {struct Cyc_Absyn_ExternC_d_struct*_tmp515=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp4F4;if(_tmp515->tag != 12)goto _LL2A6;else{_tmp516=_tmp515->f1;}}_LL2A5: if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA4F;const char*_tmpA4E;struct Cyc_PP_Doc*_tmpA4D[6];s=((_tmpA4D[5]=
Cyc_Absynpp_rb(),((_tmpA4D[4]=Cyc_PP_line_doc(),((_tmpA4D[3]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA4E="",_tag_dyneither(_tmpA4E,
sizeof(char),1))),_tmp516),((_tmpA4D[2]=Cyc_PP_line_doc(),((_tmpA4D[1]=Cyc_Absynpp_lb(),((
_tmpA4D[0]=Cyc_PP_text(((_tmpA4F="extern \"C\" ",_tag_dyneither(_tmpA4F,sizeof(
char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA4D,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmpA5A;const char*_tmpA59;const char*_tmpA58;const char*_tmpA57;
const char*_tmpA56;struct Cyc_PP_Doc*_tmpA55[9];s=((_tmpA55[8]=Cyc_PP_text(((
_tmpA56=" */",_tag_dyneither(_tmpA56,sizeof(char),4)))),((_tmpA55[7]=Cyc_Absynpp_rb(),((
_tmpA55[6]=Cyc_PP_text(((_tmpA57="/* ",_tag_dyneither(_tmpA57,sizeof(char),4)))),((
_tmpA55[5]=Cyc_PP_line_doc(),((_tmpA55[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA58="",_tag_dyneither(_tmpA58,sizeof(char),1))),
_tmp516),((_tmpA55[3]=Cyc_PP_line_doc(),((_tmpA55[2]=Cyc_PP_text(((_tmpA59=" */",
_tag_dyneither(_tmpA59,sizeof(char),4)))),((_tmpA55[1]=Cyc_Absynpp_lb(),((
_tmpA55[0]=Cyc_PP_text(((_tmpA5A="/* extern \"C\" ",_tag_dyneither(_tmpA5A,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA55,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL28B;_LL2A6: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp517=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp4F4;if(_tmp517->tag != 13)
goto _LL2A8;else{_tmp518=_tmp517->f1;_tmp519=_tmp517->f2;}}_LL2A7: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp519 != 0){const char*_tmpA5F;const char*_tmpA5E;
struct Cyc_PP_Doc*_tmpA5D[7];exs_doc=((_tmpA5D[6]=Cyc_Absynpp_rb(),((_tmpA5D[5]=
Cyc_PP_line_doc(),((_tmpA5D[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmpA5E=",",_tag_dyneither(_tmpA5E,sizeof(char),2))),
_tmp519),((_tmpA5D[3]=Cyc_PP_line_doc(),((_tmpA5D[2]=Cyc_Absynpp_lb(),((_tmpA5D[
1]=Cyc_PP_text(((_tmpA5F=" export ",_tag_dyneither(_tmpA5F,sizeof(char),9)))),((
_tmpA5D[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA5D,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmpA64;const char*
_tmpA63;struct Cyc_PP_Doc*_tmpA62[6];s=((_tmpA62[5]=exs_doc,((_tmpA62[4]=Cyc_PP_line_doc(),((
_tmpA62[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA63="",_tag_dyneither(_tmpA63,sizeof(char),1))),_tmp518),((_tmpA62[2]=Cyc_PP_line_doc(),((
_tmpA62[1]=Cyc_Absynpp_lb(),((_tmpA62[0]=Cyc_PP_text(((_tmpA64="extern \"C include\" ",
_tag_dyneither(_tmpA64,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA62,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{const char*_tmpA6F;const char*
_tmpA6E;const char*_tmpA6D;const char*_tmpA6C;const char*_tmpA6B;struct Cyc_PP_Doc*
_tmpA6A[9];s=((_tmpA6A[8]=Cyc_PP_text(((_tmpA6B=" */",_tag_dyneither(_tmpA6B,
sizeof(char),4)))),((_tmpA6A[7]=Cyc_Absynpp_rb(),((_tmpA6A[6]=Cyc_PP_text(((
_tmpA6C="/* ",_tag_dyneither(_tmpA6C,sizeof(char),4)))),((_tmpA6A[5]=Cyc_PP_line_doc(),((
_tmpA6A[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA6D="",_tag_dyneither(_tmpA6D,sizeof(char),1))),_tmp518),((_tmpA6A[3]=Cyc_PP_line_doc(),((
_tmpA6A[2]=Cyc_PP_text(((_tmpA6E=" */",_tag_dyneither(_tmpA6E,sizeof(char),4)))),((
_tmpA6A[1]=Cyc_Absynpp_lb(),((_tmpA6A[0]=Cyc_PP_text(((_tmpA6F="/* extern \"C include\" ",
_tag_dyneither(_tmpA6F,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA6A,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL28B;_LL2A8: {struct Cyc_Absyn_Porton_d_struct*
_tmp51A=(struct Cyc_Absyn_Porton_d_struct*)_tmp4F4;if(_tmp51A->tag != 14)goto
_LL2AA;}_LL2A9:{const char*_tmpA72;struct Cyc_PP_Doc*_tmpA71[2];s=((_tmpA71[1]=Cyc_Absynpp_lb(),((
_tmpA71[0]=Cyc_PP_text(((_tmpA72="__cyclone_port_on__;",_tag_dyneither(_tmpA72,
sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA71,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL28B;_LL2AA: {struct Cyc_Absyn_Portoff_d_struct*_tmp51B=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp4F4;if(_tmp51B->tag != 15)goto _LL28B;}_LL2AB:{const char*_tmpA75;struct Cyc_PP_Doc*
_tmpA74[2];s=((_tmpA74[1]=Cyc_Absynpp_lb(),((_tmpA74[0]=Cyc_PP_text(((_tmpA75="__cyclone_port_off__;",
_tag_dyneither(_tmpA75,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA74,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL28B;_LL28B:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){if(Cyc_Absynpp_print_for_cycdoc)
return Cyc_PP_nil_doc();switch(sc){case Cyc_Absyn_Static: _LL2B5: {const char*
_tmpA76;return Cyc_PP_text(((_tmpA76="static ",_tag_dyneither(_tmpA76,sizeof(char),
8))));}case Cyc_Absyn_Public: _LL2B6: return Cyc_PP_nil_doc();case Cyc_Absyn_Extern:
_LL2B7: {const char*_tmpA77;return Cyc_PP_text(((_tmpA77="extern ",_tag_dyneither(
_tmpA77,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2B8: {const char*_tmpA78;
return Cyc_PP_text(((_tmpA78="extern \"C\" ",_tag_dyneither(_tmpA78,sizeof(char),
12))));}case Cyc_Absyn_Abstract: _LL2B9: {const char*_tmpA79;return Cyc_PP_text(((
_tmpA79="abstract ",_tag_dyneither(_tmpA79,sizeof(char),10))));}case Cyc_Absyn_Register:
_LL2BA: {const char*_tmpA7A;return Cyc_PP_text(((_tmpA7A="register ",
_tag_dyneither(_tmpA7A,sizeof(char),10))));}}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp572=t;struct Cyc_Absyn_Tvar*_tmp574;struct Cyc_List_List*_tmp576;
_LL2BD: {struct Cyc_Absyn_VarType_struct*_tmp573=(struct Cyc_Absyn_VarType_struct*)
_tmp572;if(_tmp573->tag != 2)goto _LL2BF;else{_tmp574=_tmp573->f1;}}_LL2BE: return
Cyc_Tcutil_is_temp_tvar(_tmp574);_LL2BF: {struct Cyc_Absyn_JoinEff_struct*_tmp575=(
struct Cyc_Absyn_JoinEff_struct*)_tmp572;if(_tmp575->tag != 24)goto _LL2C1;else{
_tmp576=_tmp575->f1;}}_LL2C0: return((int(*)(int(*pred)(void*),struct Cyc_List_List*
x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp576);_LL2C1:;
_LL2C2: return 0;_LL2BC:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp577=t;
struct Cyc_Absyn_Typedefdecl*_tmp57B;void**_tmp57C;void*_tmp57D;_LL2C4: {struct
Cyc_Absyn_AnonAggrType_struct*_tmp578=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp577;if(_tmp578->tag != 13)goto _LL2C6;}_LL2C5: return 1;_LL2C6: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp579=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp577;if(_tmp579->tag != 15)goto
_LL2C8;}_LL2C7: return 1;_LL2C8: {struct Cyc_Absyn_TypedefType_struct*_tmp57A=(
struct Cyc_Absyn_TypedefType_struct*)_tmp577;if(_tmp57A->tag != 18)goto _LL2CA;
else{_tmp57B=_tmp57A->f3;_tmp57C=_tmp57A->f4;if(_tmp57C == 0)goto _LL2CA;_tmp57D=*
_tmp57C;}}_LL2C9: return Cyc_Absynpp_is_anon_aggrtype(_tmp57D);_LL2CA:;_LL2CB:
return 0;_LL2C3:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct
_RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmpA7B;struct _tuple16 _tmp57F=(_tmpA7B.f1=(void*)tms->hd,((_tmpA7B.f2=(
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA7B)));void*_tmp580;
void*_tmp582;_LL2CD: _tmp580=_tmp57F.f1;{struct Cyc_Absyn_Pointer_mod_struct*
_tmp581=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp580;if(_tmp581->tag != 2)goto
_LL2CF;};_tmp582=_tmp57F.f2;{struct Cyc_Absyn_Function_mod_struct*_tmp583=(struct
Cyc_Absyn_Function_mod_struct*)_tmp582;if(_tmp583->tag != 3)goto _LL2CF;};_LL2CE: {
struct Cyc_List_List*_tmpA7E;struct Cyc_List_List*_tmpA7D;return(_tmpA7D=
_region_malloc(r,sizeof(*_tmpA7D)),((_tmpA7D->hd=(void*)tms->hd,((_tmpA7D->tl=((
_tmpA7E=_region_malloc(r,sizeof(*_tmpA7E)),((_tmpA7E->hd=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd,((_tmpA7E->tl=Cyc_Absynpp_bubble_attributes(r,atts,((
struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA7E)))))),_tmpA7D)))));}
_LL2CF:;_LL2D0: {struct Cyc_List_List*_tmpA7F;return(_tmpA7F=_region_malloc(r,
sizeof(*_tmpA7F)),((_tmpA7F->hd=atts,((_tmpA7F->tl=tms,_tmpA7F)))));}_LL2CC:;}
else{struct Cyc_List_List*_tmpA80;return(_tmpA80=_region_malloc(r,sizeof(*_tmpA80)),((
_tmpA80->hd=atts,((_tmpA80->tl=tms,_tmpA80)))));}}struct _tuple12 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){void*_tmp588=t;struct Cyc_Absyn_ArrayInfo
_tmp58A;void*_tmp58B;struct Cyc_Absyn_Tqual _tmp58C;struct Cyc_Absyn_Exp*_tmp58D;
union Cyc_Absyn_Constraint*_tmp58E;struct Cyc_Position_Segment*_tmp58F;struct Cyc_Absyn_PtrInfo
_tmp591;void*_tmp592;struct Cyc_Absyn_Tqual _tmp593;struct Cyc_Absyn_PtrAtts _tmp594;
struct Cyc_Absyn_FnInfo _tmp596;struct Cyc_List_List*_tmp597;struct Cyc_Core_Opt*
_tmp598;struct Cyc_Absyn_Tqual _tmp599;void*_tmp59A;struct Cyc_List_List*_tmp59B;
int _tmp59C;struct Cyc_Absyn_VarargInfo*_tmp59D;struct Cyc_List_List*_tmp59E;struct
Cyc_List_List*_tmp59F;struct Cyc_Core_Opt*_tmp5A1;struct Cyc_Core_Opt*_tmp5A2;int
_tmp5A3;struct _tuple0*_tmp5A5;struct Cyc_List_List*_tmp5A6;struct Cyc_Absyn_Typedefdecl*
_tmp5A7;void**_tmp5A8;_LL2D2: {struct Cyc_Absyn_ArrayType_struct*_tmp589=(struct
Cyc_Absyn_ArrayType_struct*)_tmp588;if(_tmp589->tag != 9)goto _LL2D4;else{_tmp58A=
_tmp589->f1;_tmp58B=_tmp58A.elt_type;_tmp58C=_tmp58A.tq;_tmp58D=_tmp58A.num_elts;
_tmp58E=_tmp58A.zero_term;_tmp58F=_tmp58A.zt_loc;}}_LL2D3: {struct Cyc_Absyn_Tqual
_tmp5AA;void*_tmp5AB;struct Cyc_List_List*_tmp5AC;struct _tuple12 _tmp5A9=Cyc_Absynpp_to_tms(
r,_tmp58C,_tmp58B);_tmp5AA=_tmp5A9.f1;_tmp5AB=_tmp5A9.f2;_tmp5AC=_tmp5A9.f3;{
void*tm;if(_tmp58D == 0){struct Cyc_Absyn_Carray_mod_struct _tmpA83;struct Cyc_Absyn_Carray_mod_struct*
_tmpA82;tm=(void*)((_tmpA82=_region_malloc(r,sizeof(*_tmpA82)),((_tmpA82[0]=((
_tmpA83.tag=0,((_tmpA83.f1=_tmp58E,((_tmpA83.f2=_tmp58F,_tmpA83)))))),_tmpA82))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmpA86;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpA85;tm=(void*)((_tmpA85=_region_malloc(r,sizeof(*_tmpA85)),((_tmpA85[0]=((
_tmpA86.tag=1,((_tmpA86.f1=(struct Cyc_Absyn_Exp*)_tmp58D,((_tmpA86.f2=_tmp58E,((
_tmpA86.f3=_tmp58F,_tmpA86)))))))),_tmpA85))));}{struct Cyc_List_List*_tmpA89;
struct _tuple12 _tmpA88;return(_tmpA88.f1=_tmp5AA,((_tmpA88.f2=_tmp5AB,((_tmpA88.f3=((
_tmpA89=_region_malloc(r,sizeof(*_tmpA89)),((_tmpA89->hd=tm,((_tmpA89->tl=
_tmp5AC,_tmpA89)))))),_tmpA88)))));};};}_LL2D4: {struct Cyc_Absyn_PointerType_struct*
_tmp590=(struct Cyc_Absyn_PointerType_struct*)_tmp588;if(_tmp590->tag != 5)goto
_LL2D6;else{_tmp591=_tmp590->f1;_tmp592=_tmp591.elt_typ;_tmp593=_tmp591.elt_tq;
_tmp594=_tmp591.ptr_atts;}}_LL2D5: {struct Cyc_Absyn_Tqual _tmp5B4;void*_tmp5B5;
struct Cyc_List_List*_tmp5B6;struct _tuple12 _tmp5B3=Cyc_Absynpp_to_tms(r,_tmp593,
_tmp592);_tmp5B4=_tmp5B3.f1;_tmp5B5=_tmp5B3.f2;_tmp5B6=_tmp5B3.f3;{struct Cyc_Absyn_Pointer_mod_struct*
_tmpA8F;struct Cyc_Absyn_Pointer_mod_struct _tmpA8E;struct Cyc_List_List*_tmpA8D;
_tmp5B6=((_tmpA8D=_region_malloc(r,sizeof(*_tmpA8D)),((_tmpA8D->hd=(void*)((
_tmpA8F=_region_malloc(r,sizeof(*_tmpA8F)),((_tmpA8F[0]=((_tmpA8E.tag=2,((
_tmpA8E.f1=_tmp594,((_tmpA8E.f2=tq,_tmpA8E)))))),_tmpA8F)))),((_tmpA8D->tl=
_tmp5B6,_tmpA8D))))));}{struct _tuple12 _tmpA90;return(_tmpA90.f1=_tmp5B4,((
_tmpA90.f2=_tmp5B5,((_tmpA90.f3=_tmp5B6,_tmpA90)))));};}_LL2D6: {struct Cyc_Absyn_FnType_struct*
_tmp595=(struct Cyc_Absyn_FnType_struct*)_tmp588;if(_tmp595->tag != 10)goto _LL2D8;
else{_tmp596=_tmp595->f1;_tmp597=_tmp596.tvars;_tmp598=_tmp596.effect;_tmp599=
_tmp596.ret_tqual;_tmp59A=_tmp596.ret_typ;_tmp59B=_tmp596.args;_tmp59C=_tmp596.c_varargs;
_tmp59D=_tmp596.cyc_varargs;_tmp59E=_tmp596.rgn_po;_tmp59F=_tmp596.attributes;}}
_LL2D7: if(!Cyc_Absynpp_print_all_tvars){if(_tmp598 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)_tmp598->v)){_tmp598=0;_tmp597=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar,_tmp597);}{struct Cyc_Absyn_Tqual _tmp5BC;void*
_tmp5BD;struct Cyc_List_List*_tmp5BE;struct _tuple12 _tmp5BB=Cyc_Absynpp_to_tms(r,
_tmp599,_tmp59A);_tmp5BC=_tmp5BB.f1;_tmp5BD=_tmp5BB.f2;_tmp5BE=_tmp5BB.f3;{
struct Cyc_List_List*tms=_tmp5BE;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL2DE: if(_tmp59F != 0){struct Cyc_Absyn_Attributes_mod_struct _tmpA93;struct Cyc_Absyn_Attributes_mod_struct*
_tmpA92;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA92=_region_malloc(r,
sizeof(*_tmpA92)),((_tmpA92[0]=((_tmpA93.tag=5,((_tmpA93.f1=0,((_tmpA93.f2=
_tmp59F,_tmpA93)))))),_tmpA92)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmpAA2;struct Cyc_Absyn_WithTypes_struct*_tmpAA1;struct Cyc_Absyn_WithTypes_struct
_tmpAA0;struct Cyc_Absyn_Function_mod_struct _tmpA9F;struct Cyc_List_List*_tmpA9E;
tms=((_tmpA9E=_region_malloc(r,sizeof(*_tmpA9E)),((_tmpA9E->hd=(void*)((_tmpAA2=
_region_malloc(r,sizeof(*_tmpAA2)),((_tmpAA2[0]=((_tmpA9F.tag=3,((_tmpA9F.f1=(
void*)((void*)((_tmpAA1=_region_malloc(r,sizeof(*_tmpAA1)),((_tmpAA1[0]=((
_tmpAA0.tag=1,((_tmpAA0.f1=_tmp59B,((_tmpAA0.f2=_tmp59C,((_tmpAA0.f3=_tmp59D,((
_tmpAA0.f4=_tmp598,((_tmpAA0.f5=_tmp59E,_tmpAA0)))))))))))),_tmpAA1))))),_tmpA9F)))),
_tmpAA2)))),((_tmpA9E->tl=tms,_tmpA9E))))));}break;case Cyc_Cyclone_Vc_c: _LL2DF:{
struct Cyc_Absyn_Function_mod_struct*_tmpAB1;struct Cyc_Absyn_WithTypes_struct*
_tmpAB0;struct Cyc_Absyn_WithTypes_struct _tmpAAF;struct Cyc_Absyn_Function_mod_struct
_tmpAAE;struct Cyc_List_List*_tmpAAD;tms=((_tmpAAD=_region_malloc(r,sizeof(*
_tmpAAD)),((_tmpAAD->hd=(void*)((_tmpAB1=_region_malloc(r,sizeof(*_tmpAB1)),((
_tmpAB1[0]=((_tmpAAE.tag=3,((_tmpAAE.f1=(void*)((void*)((_tmpAB0=_region_malloc(
r,sizeof(*_tmpAB0)),((_tmpAB0[0]=((_tmpAAF.tag=1,((_tmpAAF.f1=_tmp59B,((_tmpAAF.f2=
_tmp59C,((_tmpAAF.f3=_tmp59D,((_tmpAAF.f4=_tmp598,((_tmpAAF.f5=_tmp59E,_tmpAAF)))))))))))),
_tmpAB0))))),_tmpAAE)))),_tmpAB1)))),((_tmpAAD->tl=tms,_tmpAAD))))));}for(0;
_tmp59F != 0;_tmp59F=_tmp59F->tl){void*_tmp5CB=(void*)_tmp59F->hd;_LL2E2: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp5CC=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp5CB;if(_tmp5CC->tag != 1)goto _LL2E4;}_LL2E3: goto _LL2E5;_LL2E4: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp5CD=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp5CB;if(_tmp5CD->tag != 2)goto
_LL2E6;}_LL2E5: goto _LL2E7;_LL2E6: {struct Cyc_Absyn_Fastcall_att_struct*_tmp5CE=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp5CB;if(_tmp5CE->tag != 3)goto _LL2E8;}
_LL2E7:{struct Cyc_Absyn_Attributes_mod_struct*_tmpABB;struct Cyc_List_List*
_tmpABA;struct Cyc_Absyn_Attributes_mod_struct _tmpAB9;struct Cyc_List_List*_tmpAB8;
tms=((_tmpAB8=_region_malloc(r,sizeof(*_tmpAB8)),((_tmpAB8->hd=(void*)((_tmpABB=
_region_malloc(r,sizeof(*_tmpABB)),((_tmpABB[0]=((_tmpAB9.tag=5,((_tmpAB9.f1=0,((
_tmpAB9.f2=((_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA->hd=(void*)_tmp59F->hd,((
_tmpABA->tl=0,_tmpABA)))))),_tmpAB9)))))),_tmpABB)))),((_tmpAB8->tl=tms,_tmpAB8))))));}
goto AfterAtts;_LL2E8:;_LL2E9: goto _LL2E1;_LL2E1:;}break;}AfterAtts: if(_tmp597 != 0){
struct Cyc_Absyn_TypeParams_mod_struct*_tmpAC1;struct Cyc_Absyn_TypeParams_mod_struct
_tmpAC0;struct Cyc_List_List*_tmpABF;tms=((_tmpABF=_region_malloc(r,sizeof(*
_tmpABF)),((_tmpABF->hd=(void*)((_tmpAC1=_region_malloc(r,sizeof(*_tmpAC1)),((
_tmpAC1[0]=((_tmpAC0.tag=4,((_tmpAC0.f1=_tmp597,((_tmpAC0.f2=0,((_tmpAC0.f3=1,
_tmpAC0)))))))),_tmpAC1)))),((_tmpABF->tl=tms,_tmpABF))))));}{struct _tuple12
_tmpAC2;return(_tmpAC2.f1=_tmp5BC,((_tmpAC2.f2=_tmp5BD,((_tmpAC2.f3=tms,_tmpAC2)))));};};};
_LL2D8: {struct Cyc_Absyn_Evar_struct*_tmp5A0=(struct Cyc_Absyn_Evar_struct*)
_tmp588;if(_tmp5A0->tag != 1)goto _LL2DA;else{_tmp5A1=_tmp5A0->f1;_tmp5A2=_tmp5A0->f2;
_tmp5A3=_tmp5A0->f3;}}_LL2D9: if(_tmp5A2 == 0){struct _tuple12 _tmpAC3;return(
_tmpAC3.f1=tq,((_tmpAC3.f2=t,((_tmpAC3.f3=0,_tmpAC3)))));}else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp5A2->v);}_LL2DA: {struct Cyc_Absyn_TypedefType_struct*_tmp5A4=(
struct Cyc_Absyn_TypedefType_struct*)_tmp588;if(_tmp5A4->tag != 18)goto _LL2DC;
else{_tmp5A5=_tmp5A4->f1;_tmp5A6=_tmp5A4->f2;_tmp5A7=_tmp5A4->f3;_tmp5A8=_tmp5A4->f4;}}
_LL2DB: if((_tmp5A8 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp5A8)){struct _tuple12 _tmpAC4;return(_tmpAC4.f1=tq,((_tmpAC4.f2=t,((_tmpAC4.f3=
0,_tmpAC4)))));}else{if(tq.real_const)tq.print_const=tq.real_const;return Cyc_Absynpp_to_tms(
r,tq,*_tmp5A8);}_LL2DC:;_LL2DD: {struct _tuple12 _tmpAC5;return(_tmpAC5.f1=tq,((
_tmpAC5.f2=t,((_tmpAC5.f3=0,_tmpAC5)))));}_LL2D1:;}static int Cyc_Absynpp_is_char_ptr(
void*t){void*_tmp5DA=t;struct Cyc_Core_Opt*_tmp5DC;struct Cyc_Core_Opt _tmp5DD;void*
_tmp5DE;struct Cyc_Absyn_PtrInfo _tmp5E0;void*_tmp5E1;_LL2EB: {struct Cyc_Absyn_Evar_struct*
_tmp5DB=(struct Cyc_Absyn_Evar_struct*)_tmp5DA;if(_tmp5DB->tag != 1)goto _LL2ED;
else{_tmp5DC=_tmp5DB->f2;if(_tmp5DC == 0)goto _LL2ED;_tmp5DD=*_tmp5DC;_tmp5DE=(
void*)_tmp5DD.v;}}_LL2EC: return Cyc_Absynpp_is_char_ptr(_tmp5DE);_LL2ED: {struct
Cyc_Absyn_PointerType_struct*_tmp5DF=(struct Cyc_Absyn_PointerType_struct*)
_tmp5DA;if(_tmp5DF->tag != 5)goto _LL2EF;else{_tmp5E0=_tmp5DF->f1;_tmp5E1=_tmp5E0.elt_typ;}}
_LL2EE: L: {void*_tmp5E2=_tmp5E1;struct Cyc_Core_Opt*_tmp5E4;struct Cyc_Core_Opt
_tmp5E5;void*_tmp5E6;void**_tmp5E8;void*_tmp5E9;enum Cyc_Absyn_Size_of _tmp5EB;
_LL2F2: {struct Cyc_Absyn_Evar_struct*_tmp5E3=(struct Cyc_Absyn_Evar_struct*)
_tmp5E2;if(_tmp5E3->tag != 1)goto _LL2F4;else{_tmp5E4=_tmp5E3->f2;if(_tmp5E4 == 0)
goto _LL2F4;_tmp5E5=*_tmp5E4;_tmp5E6=(void*)_tmp5E5.v;}}_LL2F3: _tmp5E1=_tmp5E6;
goto L;_LL2F4: {struct Cyc_Absyn_TypedefType_struct*_tmp5E7=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5E2;if(_tmp5E7->tag != 18)goto _LL2F6;else{_tmp5E8=_tmp5E7->f4;if(_tmp5E8 == 0)
goto _LL2F6;_tmp5E9=*_tmp5E8;}}_LL2F5: _tmp5E1=_tmp5E9;goto L;_LL2F6: {struct Cyc_Absyn_IntType_struct*
_tmp5EA=(struct Cyc_Absyn_IntType_struct*)_tmp5E2;if(_tmp5EA->tag != 6)goto _LL2F8;
else{_tmp5EB=_tmp5EA->f2;if(_tmp5EB != Cyc_Absyn_Char_sz)goto _LL2F8;}}_LL2F7:
return 1;_LL2F8:;_LL2F9: return 0;_LL2F1:;}_LL2EF:;_LL2F0: return 0;_LL2EA:;}struct
Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*
dopt){struct _RegionHandle _tmp5EC=_new_region("temp");struct _RegionHandle*temp=&
_tmp5EC;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp5EE;void*_tmp5EF;struct Cyc_List_List*
_tmp5F0;struct _tuple12 _tmp5ED=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp5EE=_tmp5ED.f1;
_tmp5EF=_tmp5ED.f2;_tmp5F0=_tmp5ED.f3;_tmp5F0=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5F0);if(_tmp5F0 == 0  && dopt == 0){struct
Cyc_PP_Doc*_tmpAC6[2];struct Cyc_PP_Doc*_tmp5F2=(_tmpAC6[1]=Cyc_Absynpp_ntyp2doc(
_tmp5EF),((_tmpAC6[0]=Cyc_Absynpp_tqual2doc(_tmp5EE),Cyc_PP_cat(_tag_dyneither(
_tmpAC6,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5F2;}else{
const char*_tmpAC9;struct Cyc_PP_Doc*_tmpAC8[4];struct Cyc_PP_Doc*_tmp5F5=(_tmpAC8[
3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*)dopt->v,_tmp5F0),((_tmpAC8[2]=Cyc_PP_text(((_tmpAC9=" ",
_tag_dyneither(_tmpAC9,sizeof(char),2)))),((_tmpAC8[1]=Cyc_Absynpp_ntyp2doc(
_tmp5EF),((_tmpAC8[0]=Cyc_Absynpp_tqual2doc(_tmp5EE),Cyc_PP_cat(_tag_dyneither(
_tmpAC8,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5F5;}};
_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*
f){switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2FA: if(f->width != 0){
struct Cyc_Core_Opt*_tmpAD0;const char*_tmpACF;const char*_tmpACE;struct Cyc_PP_Doc*
_tmpACD[5];return(_tmpACD[4]=Cyc_PP_text(((_tmpACE=";",_tag_dyneither(_tmpACE,
sizeof(char),2)))),((_tmpACD[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpACD[2]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpACD[1]=
Cyc_PP_text(((_tmpACF=":",_tag_dyneither(_tmpACF,sizeof(char),2)))),((_tmpACD[0]=
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((
_tmpAD0->v=Cyc_PP_textptr(f->name),_tmpAD0))))),Cyc_PP_cat(_tag_dyneither(
_tmpACD,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*_tmpAD5;
const char*_tmpAD4;struct Cyc_PP_Doc*_tmpAD3[3];return(_tmpAD3[2]=Cyc_PP_text(((
_tmpAD4=";",_tag_dyneither(_tmpAD4,sizeof(char),2)))),((_tmpAD3[1]=Cyc_Absynpp_atts2doc(
f->attributes),((_tmpAD3[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAD5=
_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5->v=Cyc_PP_textptr(f->name),_tmpAD5))))),
Cyc_PP_cat(_tag_dyneither(_tmpAD3,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c:
_LL2FB: if(f->width != 0){struct Cyc_Core_Opt*_tmpADC;const char*_tmpADB;const char*
_tmpADA;struct Cyc_PP_Doc*_tmpAD9[5];return(_tmpAD9[4]=Cyc_PP_text(((_tmpADA=";",
_tag_dyneither(_tmpADA,sizeof(char),2)))),((_tmpAD9[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAD9[2]=Cyc_PP_text(((_tmpADB=":",
_tag_dyneither(_tmpADB,sizeof(char),2)))),((_tmpAD9[1]=Cyc_Absynpp_tqtd2doc(f->tq,
f->type,((_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC->v=Cyc_PP_textptr(f->name),
_tmpADC))))),((_tmpAD9[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAD9,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpAE1;const char*_tmpAE0;struct Cyc_PP_Doc*_tmpADF[3];return(_tmpADF[2]=Cyc_PP_text(((
_tmpAE0=";",_tag_dyneither(_tmpAE0,sizeof(char),2)))),((_tmpADF[1]=Cyc_Absynpp_tqtd2doc(
f->tq,f->type,((_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1->v=Cyc_PP_textptr(
f->name),_tmpAE1))))),((_tmpADF[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpADF,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){const char*_tmpAE2;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((
_tmpAE2="",_tag_dyneither(_tmpAE2,sizeof(char),1))),fields);}struct Cyc_PP_Doc*
Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*
_tmpAE3[3];return(_tmpAE3[2]=f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((
_tmpAE3[1]=Cyc_Absynpp_typedef_name2doc(f->name),((_tmpAE3[0]=Cyc_Absynpp_scope2doc(
f->sc),Cyc_PP_cat(_tag_dyneither(_tmpAE3,sizeof(struct Cyc_PP_Doc*),3)))))));}
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpAE4;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((
_tmpAE4=",",_tag_dyneither(_tmpAE4,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmpAE7;
void*_tmpAE6;(_tmpAE6=0,Cyc_fprintf(f,((_tmpAE7="\n",_tag_dyneither(_tmpAE7,
sizeof(char),2))),_tag_dyneither(_tmpAE6,sizeof(void*),0)));};}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmpAE8;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAE8="",_tag_dyneither(
_tmpAE8,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
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
