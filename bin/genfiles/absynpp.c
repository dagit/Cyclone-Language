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
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};typedef void*Cyc_parg_t;struct _dyneither_ptr Cyc_aprintf(struct
_dyneither_ptr,struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};typedef
unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t;struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char
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
Cyc_PP_tex_output;struct Cyc_PP_Ppstate;typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;
struct Cyc_PP_Out;typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;typedef
struct Cyc_PP_Doc*Cyc_PP_doc_t;void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,
struct Cyc___cycFILE*f);struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*
d,int w);struct Cyc_PP_Doc*Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();
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
line_no;int col;};typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_struct{char*tag;};typedef unsigned int Cyc_Position_seg_t;
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};typedef void*Cyc_Position_error_kind_t;
struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;
struct _dyneither_ptr desc;};typedef struct Cyc_Position_Error*Cyc_Position_error_t;
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;typedef struct
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
print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};enum Cyc_Absyn_Size_of{
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
Forward_constr;struct _union_Constraint_No_constr No_constr;};typedef union Cyc_Absyn_Constraint*
Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_struct{int tag;struct Cyc_Absyn_Kind*f1;};
struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{
struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};
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
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};
struct Cyc_Absyn_Aggr_td_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;};typedef void*Cyc_Absyn_raw_type_decl_t;struct
Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;
struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
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
};struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};
struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};typedef void*Cyc_Absyn_funcparams_t;
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};
struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_struct{
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
Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};
struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual
f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{
int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;unsigned int f1;struct Cyc_List_List*f2;};typedef void*Cyc_Absyn_type_modifier_t;
struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;
char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct
_union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};
struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct
_union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;
struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct
_dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct
_union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};enum Cyc_Absyn_Primop{
Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod
 = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte
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
topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_struct{int tag;};
struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
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
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;
struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{
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
struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_struct{
int tag;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
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
name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
unsigned int loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct _dyneither_ptr*f1;};extern
char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{char*tag;};struct
Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind
f1;struct _tuple0*f2;};struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
struct Cyc_Buffer_t;typedef struct Cyc_Buffer_t*Cyc_Buffer_T;unsigned long Cyc_strlen(
struct _dyneither_ptr s);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct
_dyneither_ptr);struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct
_dyneither_ptr);struct _tuple11{unsigned int f1;int f2;};struct _tuple11 Cyc_Evexp_eval_const_uint_exp(
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
struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int
Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*
tl;};typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;typedef const struct Cyc_Tcenv_CList*
const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
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
void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_no_qual(
unsigned int loc,void*t);struct Cyc_Absynpp_Params{int expand_typedefs;int
qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int
print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int
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
_dyneither_ptr*v){struct Cyc_List_List*_tmp619;Cyc_Absynpp_curr_namespace=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_Absynpp_curr_namespace,((_tmp619=_cycalloc(sizeof(*_tmp619)),((_tmp619->hd=v,((
_tmp619->tl=0,_tmp619)))))));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**
l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((
struct Cyc_List_List*)_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0: {
const char*_tmp61A;return(_tmp61A="\\a",_tag_dyneither(_tmp61A,sizeof(char),3));}
case '\b': _LL1: {const char*_tmp61B;return(_tmp61B="\\b",_tag_dyneither(_tmp61B,
sizeof(char),3));}case '\f': _LL2: {const char*_tmp61C;return(_tmp61C="\\f",
_tag_dyneither(_tmp61C,sizeof(char),3));}case '\n': _LL3: {const char*_tmp61D;
return(_tmp61D="\\n",_tag_dyneither(_tmp61D,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp61E;return(_tmp61E="\\r",_tag_dyneither(_tmp61E,sizeof(char),3));}
case '\t': _LL5: {const char*_tmp61F;return(_tmp61F="\\t",_tag_dyneither(_tmp61F,
sizeof(char),3));}case '\v': _LL6: {const char*_tmp620;return(_tmp620="\\v",
_tag_dyneither(_tmp620,sizeof(char),3));}case '\\': _LL7: {const char*_tmp621;
return(_tmp621="\\\\",_tag_dyneither(_tmp621,sizeof(char),3));}case '"': _LL8: {
const char*_tmp622;return(_tmp622="\"",_tag_dyneither(_tmp622,sizeof(char),2));}
case '\'': _LL9: {const char*_tmp623;return(_tmp623="\\'",_tag_dyneither(_tmp623,
sizeof(char),3));}default: _LLA: if(c >= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=
Cyc_Core_new_string(2);{char _tmp626;char _tmp625;struct _dyneither_ptr _tmp624;(
_tmp624=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp625=*((char*)
_check_dyneither_subscript(_tmp624,sizeof(char),0)),((_tmp626=c,((
_get_dyneither_size(_tmp624,sizeof(char))== 1  && (_tmp625 == '\000'  && _tmp626 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp624.curr)=_tmp626)))))));}return(struct
_dyneither_ptr)_tmpC;}else{struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);int
j=0;{char _tmp629;char _tmp628;struct _dyneither_ptr _tmp627;(_tmp627=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp628=*((char*)
_check_dyneither_subscript(_tmp627,sizeof(char),0)),((_tmp629='\\',((
_get_dyneither_size(_tmp627,sizeof(char))== 1  && (_tmp628 == '\000'  && _tmp629 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp627.curr)=_tmp629)))))));}{char _tmp62C;char
_tmp62B;struct _dyneither_ptr _tmp62A;(_tmp62A=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp62B=*((char*)_check_dyneither_subscript(_tmp62A,sizeof(char),0)),((
_tmp62C=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp62A,
sizeof(char))== 1  && (_tmp62B == '\000'  && _tmp62C != '\000')?_throw_arraybounds():
1,*((char*)_tmp62A.curr)=_tmp62C)))))));}{char _tmp62F;char _tmp62E;struct
_dyneither_ptr _tmp62D;(_tmp62D=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp62E=*((char*)_check_dyneither_subscript(_tmp62D,sizeof(char),0)),((_tmp62F=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp62D,sizeof(char))== 1  && (
_tmp62E == '\000'  && _tmp62F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62D.curr)=
_tmp62F)))))));}{char _tmp632;char _tmp631;struct _dyneither_ptr _tmp630;(_tmp630=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp631=*((char*)
_check_dyneither_subscript(_tmp630,sizeof(char),0)),((_tmp632=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp630,sizeof(char))== 1  && (_tmp631 == '\000'  && _tmp632 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp630.curr)=_tmp632)))))));}return(struct
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
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp635;char _tmp634;struct
_dyneither_ptr _tmp633;(_tmp633=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp634=*((
char*)_check_dyneither_subscript(_tmp633,sizeof(char),0)),((_tmp635='\\',((
_get_dyneither_size(_tmp633,sizeof(char))== 1  && (_tmp634 == '\000'  && _tmp635 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp633.curr)=_tmp635)))))));}{char _tmp638;char
_tmp637;struct _dyneither_ptr _tmp636;(_tmp636=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp637=*((char*)_check_dyneither_subscript(_tmp636,sizeof(char),0)),((
_tmp638='a',((_get_dyneither_size(_tmp636,sizeof(char))== 1  && (_tmp637 == '\000'
 && _tmp638 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp636.curr)=_tmp638)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp63B;char _tmp63A;struct
_dyneither_ptr _tmp639;(_tmp639=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63A=*((
char*)_check_dyneither_subscript(_tmp639,sizeof(char),0)),((_tmp63B='\\',((
_get_dyneither_size(_tmp639,sizeof(char))== 1  && (_tmp63A == '\000'  && _tmp63B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp639.curr)=_tmp63B)))))));}{char _tmp63E;char
_tmp63D;struct _dyneither_ptr _tmp63C;(_tmp63C=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp63D=*((char*)_check_dyneither_subscript(_tmp63C,sizeof(char),0)),((
_tmp63E='b',((_get_dyneither_size(_tmp63C,sizeof(char))== 1  && (_tmp63D == '\000'
 && _tmp63E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63C.curr)=_tmp63E)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp641;char _tmp640;struct
_dyneither_ptr _tmp63F;(_tmp63F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp640=*((
char*)_check_dyneither_subscript(_tmp63F,sizeof(char),0)),((_tmp641='\\',((
_get_dyneither_size(_tmp63F,sizeof(char))== 1  && (_tmp640 == '\000'  && _tmp641 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp63F.curr)=_tmp641)))))));}{char _tmp644;char
_tmp643;struct _dyneither_ptr _tmp642;(_tmp642=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp643=*((char*)_check_dyneither_subscript(_tmp642,sizeof(char),0)),((
_tmp644='f',((_get_dyneither_size(_tmp642,sizeof(char))== 1  && (_tmp643 == '\000'
 && _tmp644 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp642.curr)=_tmp644)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp647;char _tmp646;struct
_dyneither_ptr _tmp645;(_tmp645=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp646=*((
char*)_check_dyneither_subscript(_tmp645,sizeof(char),0)),((_tmp647='\\',((
_get_dyneither_size(_tmp645,sizeof(char))== 1  && (_tmp646 == '\000'  && _tmp647 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp645.curr)=_tmp647)))))));}{char _tmp64A;char
_tmp649;struct _dyneither_ptr _tmp648;(_tmp648=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp649=*((char*)_check_dyneither_subscript(_tmp648,sizeof(char),0)),((
_tmp64A='n',((_get_dyneither_size(_tmp648,sizeof(char))== 1  && (_tmp649 == '\000'
 && _tmp64A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp648.curr)=_tmp64A)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp64D;char _tmp64C;struct
_dyneither_ptr _tmp64B;(_tmp64B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64C=*((
char*)_check_dyneither_subscript(_tmp64B,sizeof(char),0)),((_tmp64D='\\',((
_get_dyneither_size(_tmp64B,sizeof(char))== 1  && (_tmp64C == '\000'  && _tmp64D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp64B.curr)=_tmp64D)))))));}{char _tmp650;char
_tmp64F;struct _dyneither_ptr _tmp64E;(_tmp64E=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp64F=*((char*)_check_dyneither_subscript(_tmp64E,sizeof(char),0)),((
_tmp650='r',((_get_dyneither_size(_tmp64E,sizeof(char))== 1  && (_tmp64F == '\000'
 && _tmp650 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64E.curr)=_tmp650)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp653;char _tmp652;struct
_dyneither_ptr _tmp651;(_tmp651=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp652=*((
char*)_check_dyneither_subscript(_tmp651,sizeof(char),0)),((_tmp653='\\',((
_get_dyneither_size(_tmp651,sizeof(char))== 1  && (_tmp652 == '\000'  && _tmp653 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp651.curr)=_tmp653)))))));}{char _tmp656;char
_tmp655;struct _dyneither_ptr _tmp654;(_tmp654=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp655=*((char*)_check_dyneither_subscript(_tmp654,sizeof(char),0)),((
_tmp656='t',((_get_dyneither_size(_tmp654,sizeof(char))== 1  && (_tmp655 == '\000'
 && _tmp656 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp654.curr)=_tmp656)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp659;char _tmp658;struct
_dyneither_ptr _tmp657;(_tmp657=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp658=*((
char*)_check_dyneither_subscript(_tmp657,sizeof(char),0)),((_tmp659='\\',((
_get_dyneither_size(_tmp657,sizeof(char))== 1  && (_tmp658 == '\000'  && _tmp659 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp657.curr)=_tmp659)))))));}{char _tmp65C;char
_tmp65B;struct _dyneither_ptr _tmp65A;(_tmp65A=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp65B=*((char*)_check_dyneither_subscript(_tmp65A,sizeof(char),0)),((
_tmp65C='v',((_get_dyneither_size(_tmp65A,sizeof(char))== 1  && (_tmp65B == '\000'
 && _tmp65C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65A.curr)=_tmp65C)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp65F;char _tmp65E;struct
_dyneither_ptr _tmp65D;(_tmp65D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp65E=*((
char*)_check_dyneither_subscript(_tmp65D,sizeof(char),0)),((_tmp65F='\\',((
_get_dyneither_size(_tmp65D,sizeof(char))== 1  && (_tmp65E == '\000'  && _tmp65F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp65D.curr)=_tmp65F)))))));}{char _tmp662;char
_tmp661;struct _dyneither_ptr _tmp660;(_tmp660=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp661=*((char*)_check_dyneither_subscript(_tmp660,sizeof(char),0)),((
_tmp662='\\',((_get_dyneither_size(_tmp660,sizeof(char))== 1  && (_tmp661 == '\000'
 && _tmp662 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp660.curr)=_tmp662)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp665;char _tmp664;struct
_dyneither_ptr _tmp663;(_tmp663=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp664=*((
char*)_check_dyneither_subscript(_tmp663,sizeof(char),0)),((_tmp665='\\',((
_get_dyneither_size(_tmp663,sizeof(char))== 1  && (_tmp664 == '\000'  && _tmp665 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp663.curr)=_tmp665)))))));}{char _tmp668;char
_tmp667;struct _dyneither_ptr _tmp666;(_tmp666=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp667=*((char*)_check_dyneither_subscript(_tmp666,sizeof(char),0)),((
_tmp668='"',((_get_dyneither_size(_tmp666,sizeof(char))== 1  && (_tmp667 == '\000'
 && _tmp668 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp666.curr)=_tmp668)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp66B;char _tmp66A;
struct _dyneither_ptr _tmp669;(_tmp669=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp66A=*((char*)_check_dyneither_subscript(_tmp669,sizeof(char),0)),((_tmp66B=
_tmp1E,((_get_dyneither_size(_tmp669,sizeof(char))== 1  && (_tmp66A == '\000'  && 
_tmp66B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp669.curr)=_tmp66B)))))));}
else{{char _tmp66E;char _tmp66D;struct _dyneither_ptr _tmp66C;(_tmp66C=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp66D=*((char*)
_check_dyneither_subscript(_tmp66C,sizeof(char),0)),((_tmp66E='\\',((
_get_dyneither_size(_tmp66C,sizeof(char))== 1  && (_tmp66D == '\000'  && _tmp66E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp66C.curr)=_tmp66E)))))));}{char _tmp671;char
_tmp670;struct _dyneither_ptr _tmp66F;(_tmp66F=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp670=*((char*)_check_dyneither_subscript(_tmp66F,sizeof(char),0)),((
_tmp671=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp66F,sizeof(char))
== 1  && (_tmp670 == '\000'  && _tmp671 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp66F.curr)=_tmp671)))))));}{char _tmp674;char _tmp673;struct _dyneither_ptr
_tmp672;(_tmp672=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp673=*((char*)
_check_dyneither_subscript(_tmp672,sizeof(char),0)),((_tmp674=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp672,sizeof(char))== 1  && (_tmp673 == '\000'
 && _tmp674 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp672.curr)=_tmp674)))))));}{
char _tmp677;char _tmp676;struct _dyneither_ptr _tmp675;(_tmp675=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp676=*((char*)_check_dyneither_subscript(_tmp675,
sizeof(char),0)),((_tmp677=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp675,sizeof(char))== 1  && (_tmp676 == '\000'  && _tmp677 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp675.curr)=_tmp677)))))));};}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;};};};}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*
l=0;if(tq.q_restrict){struct Cyc_List_List*_tmp678;l=((_tmp678=_cycalloc(sizeof(*
_tmp678)),((_tmp678->hd=Cyc_Absynpp_restrict_sp,((_tmp678->tl=l,_tmp678))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp679;l=((_tmp679=_cycalloc(sizeof(*
_tmp679)),((_tmp679->hd=Cyc_Absynpp_volatile_sp,((_tmp679->tl=l,_tmp679))))));}
if(tq.print_const){struct Cyc_List_List*_tmp67A;l=((_tmp67A=_cycalloc(sizeof(*
_tmp67A)),((_tmp67A->hd=Cyc_Absynpp_const_sp,((_tmp67A->tl=l,_tmp67A))))));}{
const char*_tmp67D;const char*_tmp67C;const char*_tmp67B;return Cyc_PP_egroup(((
_tmp67B="",_tag_dyneither(_tmp67B,sizeof(char),1))),((_tmp67C=" ",_tag_dyneither(
_tmp67C,sizeof(char),2))),((_tmp67D=" ",_tag_dyneither(_tmp67D,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,l));};}struct _dyneither_ptr Cyc_Absynpp_kind2string(
struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp6E;enum Cyc_Absyn_KindQual
_tmp6F;enum Cyc_Absyn_AliasQual _tmp70;struct Cyc_Absyn_Kind*_tmp6D=ka;_tmp6E=*
_tmp6D;_tmp6F=_tmp6E.kind;_tmp70=_tmp6E.aliasqual;switch(_tmp6F){case Cyc_Absyn_AnyKind:
_LL36: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL38: {const char*_tmp67E;return(
_tmp67E="A",_tag_dyneither(_tmp67E,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp67F;return(_tmp67F="UA",_tag_dyneither(_tmp67F,sizeof(char),3));}
case Cyc_Absyn_Top: _LL3A: {const char*_tmp680;return(_tmp680="TA",_tag_dyneither(
_tmp680,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37: switch(_tmp70){case Cyc_Absyn_Aliasable:
_LL3D: {const char*_tmp681;return(_tmp681="M",_tag_dyneither(_tmp681,sizeof(char),
2));}case Cyc_Absyn_Unique: _LL3E: {const char*_tmp682;return(_tmp682="UM",
_tag_dyneither(_tmp682,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {const char*
_tmp683;return(_tmp683="TM",_tag_dyneither(_tmp683,sizeof(char),3));}}case Cyc_Absyn_BoxKind:
_LL3C: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL42: {const char*_tmp684;return(
_tmp684="B",_tag_dyneither(_tmp684,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp685;return(_tmp685="UB",_tag_dyneither(_tmp685,sizeof(char),3));}
case Cyc_Absyn_Top: _LL44: {const char*_tmp686;return(_tmp686="TB",_tag_dyneither(
_tmp686,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41: switch(_tmp70){case Cyc_Absyn_Aliasable:
_LL47: {const char*_tmp687;return(_tmp687="R",_tag_dyneither(_tmp687,sizeof(char),
2));}case Cyc_Absyn_Unique: _LL48: {const char*_tmp688;return(_tmp688="UR",
_tag_dyneither(_tmp688,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {const char*
_tmp689;return(_tmp689="TR",_tag_dyneither(_tmp689,sizeof(char),3));}}case Cyc_Absyn_EffKind:
_LL46: {const char*_tmp68A;return(_tmp68A="E",_tag_dyneither(_tmp68A,sizeof(char),
2));}case Cyc_Absyn_IntKind: _LL4B: {const char*_tmp68B;return(_tmp68B="I",
_tag_dyneither(_tmp68B,sizeof(char),2));}}}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp7F=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp81;struct Cyc_Absyn_Kind*_tmp84;_LL4E: {struct Cyc_Absyn_Eq_kb_struct*
_tmp80=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7F;if(_tmp80->tag != 0)goto _LL50;else{
_tmp81=_tmp80->f1;}}_LL4F: return Cyc_Absynpp_kind2string(_tmp81);_LL50: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp82=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp7F;
if(_tmp82->tag != 1)goto _LL52;}_LL51: if(Cyc_PP_tex_output){const char*_tmp68C;
return(_tmp68C="{?}",_tag_dyneither(_tmp68C,sizeof(char),4));}else{const char*
_tmp68D;return(_tmp68D="?",_tag_dyneither(_tmp68D,sizeof(char),2));}_LL52: {
struct Cyc_Absyn_Less_kb_struct*_tmp83=(struct Cyc_Absyn_Less_kb_struct*)_tmp7F;
if(_tmp83->tag != 2)goto _LL4D;else{_tmp84=_tmp83->f2;}}_LL53: {const char*_tmp691;
void*_tmp690[1];struct Cyc_String_pa_struct _tmp68F;return(struct _dyneither_ptr)((
_tmp68F.tag=0,((_tmp68F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp84)),((_tmp690[0]=& _tmp68F,Cyc_aprintf(((_tmp691="<=%s",_tag_dyneither(
_tmp691,sizeof(char),5))),_tag_dyneither(_tmp690,sizeof(void*),1))))))));}_LL4D:;}
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){void*_tmp8A=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp8C;struct Cyc_Absyn_Kind*_tmp8F;_LL55: {struct Cyc_Absyn_Eq_kb_struct*
_tmp8B=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL57;else{
_tmp8C=_tmp8B->f1;}}_LL56: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8C));
_LL57: {struct Cyc_Absyn_Unknown_kb_struct*_tmp8D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8A;if(_tmp8D->tag != 1)goto _LL59;}_LL58: if(Cyc_PP_tex_output){const char*
_tmp692;return Cyc_PP_text_width(((_tmp692="{?}",_tag_dyneither(_tmp692,sizeof(
char),4))),1);}else{const char*_tmp693;return Cyc_PP_text(((_tmp693="?",
_tag_dyneither(_tmp693,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_struct*
_tmp8E=(struct Cyc_Absyn_Less_kb_struct*)_tmp8A;if(_tmp8E->tag != 2)goto _LL54;
else{_tmp8F=_tmp8E->f2;}}_LL5A: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8F));
_LL54:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*
_tmp696;const char*_tmp695;const char*_tmp694;return Cyc_PP_egroup(((_tmp694="<",
_tag_dyneither(_tmp694,sizeof(char),2))),((_tmp695=">",_tag_dyneither(_tmp695,
sizeof(char),2))),((_tmp696=",",_tag_dyneither(_tmp696,sizeof(char),2))),((
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
_tmp699;struct Cyc_PP_Doc*_tmp698[3];return(_tmp698[2]=Cyc_Absynpp_kind2doc(
_tmp9F),((_tmp698[1]=Cyc_PP_text(((_tmp699="::",_tag_dyneither(_tmp699,sizeof(
char),3)))),((_tmp698[0]=Cyc_PP_textptr(tv->name),Cyc_PP_cat(_tag_dyneither(
_tmp698,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs){const char*_tmp69C;const char*_tmp69B;const char*_tmp69A;
return Cyc_PP_egroup(((_tmp69A="<",_tag_dyneither(_tmp69A,sizeof(char),2))),((
_tmp69B=">",_tag_dyneither(_tmp69B,sizeof(char),2))),((_tmp69C=",",
_tag_dyneither(_tmp69C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,
tvs));}static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);{const char*
_tmp69F;const char*_tmp69E;const char*_tmp69D;return Cyc_PP_egroup(((_tmp69D="<",
_tag_dyneither(_tmp69D,sizeof(char),2))),((_tmp69E=">",_tag_dyneither(_tmp69E,
sizeof(char),2))),((_tmp69F=",",_tag_dyneither(_tmp69F,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));};}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple16*t){return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*
Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){const char*_tmp6A2;const char*_tmp6A1;
const char*_tmp6A0;return Cyc_PP_group(((_tmp6A0="(",_tag_dyneither(_tmp6A0,
sizeof(char),2))),((_tmp6A1=")",_tag_dyneither(_tmp6A1,sizeof(char),2))),((
_tmp6A2=",",_tag_dyneither(_tmp6A2,sizeof(char),2))),((struct Cyc_List_List*(*)(
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
_tmpAF;if(_tmpB0->tag != 1)goto _LL70;}_LL6F: {const char*_tmp6A3;return Cyc_PP_text(((
_tmp6A3=" _stdcall ",_tag_dyneither(_tmp6A3,sizeof(char),11))));}_LL70: {struct
Cyc_Absyn_Cdecl_att_struct*_tmpB1=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpAF;if(
_tmpB1->tag != 2)goto _LL72;}_LL71: {const char*_tmp6A4;return Cyc_PP_text(((_tmp6A4=" _cdecl ",
_tag_dyneither(_tmp6A4,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpB2=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpAF;if(_tmpB2->tag != 3)goto _LL74;}
_LL73: {const char*_tmp6A5;return Cyc_PP_text(((_tmp6A5=" _fastcall ",
_tag_dyneither(_tmp6A5,sizeof(char),12))));}_LL74:;_LL75: goto _LL6D;_LL6D:;}
return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpB6=(void*)atts2->hd;_LL77: {struct Cyc_Absyn_Stdcall_att_struct*_tmpB7=(
struct Cyc_Absyn_Stdcall_att_struct*)_tmpB6;if(_tmpB7->tag != 1)goto _LL79;}_LL78:
goto _LL7A;_LL79: {struct Cyc_Absyn_Cdecl_att_struct*_tmpB8=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmpB6;if(_tmpB8->tag != 2)goto _LL7B;}_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpB9=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpB6;if(_tmpB9->tag != 3)goto _LL7D;}
_LL7C: goto _LL76;_LL7D:;_LL7E: hasatt=1;goto _LL76;_LL76:;}}if(!hasatt)return Cyc_PP_nil_doc();{
const char*_tmp6B0;const char*_tmp6AF;const char*_tmp6AE;const char*_tmp6AD;const
char*_tmp6AC;struct Cyc_PP_Doc*_tmp6AB[3];return(_tmp6AB[2]=Cyc_PP_text(((_tmp6AC=")",
_tag_dyneither(_tmp6AC,sizeof(char),2)))),((_tmp6AB[1]=Cyc_PP_group(((_tmp6AF="",
_tag_dyneither(_tmp6AF,sizeof(char),1))),((_tmp6AE="",_tag_dyneither(_tmp6AE,
sizeof(char),1))),((_tmp6AD=" ",_tag_dyneither(_tmp6AD,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_noncallatt2doc,atts)),((_tmp6AB[0]=Cyc_PP_text(((_tmp6B0=" __declspec(",
_tag_dyneither(_tmp6B0,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp6AB,
sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*
a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Vc_c: _LL7F: return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c:
_LL80: {const char*_tmp6B9;const char*_tmp6B8;const char*_tmp6B7;const char*_tmp6B6;
struct Cyc_PP_Doc*_tmp6B5[2];return(_tmp6B5[1]=Cyc_PP_group(((_tmp6B8="((",
_tag_dyneither(_tmp6B8,sizeof(char),3))),((_tmp6B7="))",_tag_dyneither(_tmp6B7,
sizeof(char),3))),((_tmp6B6=",",_tag_dyneither(_tmp6B6,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_att2doc,atts)),((_tmp6B5[0]=Cyc_PP_text(((_tmp6B9=" __attribute__",
_tag_dyneither(_tmp6B9,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp6B5,
sizeof(struct Cyc_PP_Doc*),2)))));}}}int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*
tms){if(tms == 0)return 0;{void*_tmpC5=(void*)tms->hd;_LL83: {struct Cyc_Absyn_Pointer_mod_struct*
_tmpC6=(struct Cyc_Absyn_Pointer_mod_struct*)_tmpC5;if(_tmpC6->tag != 2)goto _LL85;}
_LL84: return 1;_LL85: {struct Cyc_Absyn_Attributes_mod_struct*_tmpC7=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC5;if(_tmpC7->tag != 5)goto _LL87;}_LL86: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL89: return 0;default: _LL8A: return Cyc_Absynpp_next_is_pointer(
tms->tl);}_LL87:;_LL88: return 0;_LL82:;};}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*
Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output){
const char*_tmp6BA;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp6BA="{?}",_tag_dyneither(_tmp6BA,sizeof(char),4))),1);}else{const char*
_tmp6BB;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp6BB="?",
_tag_dyneither(_tmp6BB,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){const char*_tmp6BC;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp6BC="{\\lb}",_tag_dyneither(_tmp6BC,sizeof(char),6))),1);}
else{const char*_tmp6BD;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp6BD="{",_tag_dyneither(_tmp6BD,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_lb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){const char*_tmp6BE;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp6BE="{\\rb}",_tag_dyneither(_tmp6BE,sizeof(char),6))),1);}
else{const char*_tmp6BF;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp6BF="}",_tag_dyneither(_tmp6BF,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){const char*_tmp6C0;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp6C0="\\$",_tag_dyneither(_tmp6C0,sizeof(char),3))),1);}
else{const char*_tmp6C1;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp6C1="$",_tag_dyneither(_tmp6C1,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_dollar);}struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(
struct _dyneither_ptr sep,struct Cyc_List_List*ss){struct Cyc_PP_Doc*_tmp6C2[3];
return(_tmp6C2[2]=Cyc_Absynpp_rb(),((_tmp6C2[1]=Cyc_PP_seq(sep,ss),((_tmp6C2[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp6C2,sizeof(struct Cyc_PP_Doc*),3)))))));}
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){for(0;tms != 0;tms=tms->tl){
void*_tmpD1=(void*)tms->hd;void*_tmpD5;struct Cyc_List_List*_tmpD7;_LL8D: {struct
Cyc_Absyn_Carray_mod_struct*_tmpD2=(struct Cyc_Absyn_Carray_mod_struct*)_tmpD1;
if(_tmpD2->tag != 0)goto _LL8F;}_LL8E:{const char*_tmp6C5;void*_tmp6C4;(_tmp6C4=0,
Cyc_fprintf(Cyc_stderr,((_tmp6C5="Carray_mod ",_tag_dyneither(_tmp6C5,sizeof(
char),12))),_tag_dyneither(_tmp6C4,sizeof(void*),0)));}goto _LL8C;_LL8F: {struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD3=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD1;if(_tmpD3->tag != 1)goto _LL91;}_LL90:{const char*_tmp6C8;void*_tmp6C7;(
_tmp6C7=0,Cyc_fprintf(Cyc_stderr,((_tmp6C8="ConstArray_mod ",_tag_dyneither(
_tmp6C8,sizeof(char),16))),_tag_dyneither(_tmp6C7,sizeof(void*),0)));}goto _LL8C;
_LL91: {struct Cyc_Absyn_Function_mod_struct*_tmpD4=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD4->tag != 3)goto _LL93;else{_tmpD5=(void*)_tmpD4->f1;{struct Cyc_Absyn_WithTypes_struct*
_tmpD6=(struct Cyc_Absyn_WithTypes_struct*)_tmpD5;if(_tmpD6->tag != 1)goto _LL93;
else{_tmpD7=_tmpD6->f1;}};}}_LL92:{const char*_tmp6CB;void*_tmp6CA;(_tmp6CA=0,Cyc_fprintf(
Cyc_stderr,((_tmp6CB="Function_mod(",_tag_dyneither(_tmp6CB,sizeof(char),14))),
_tag_dyneither(_tmp6CA,sizeof(void*),0)));}for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){
struct Cyc_Core_Opt*_tmpE2=(*((struct _tuple8*)_tmpD7->hd)).f1;if(_tmpE2 == 0){
const char*_tmp6CE;void*_tmp6CD;(_tmp6CD=0,Cyc_fprintf(Cyc_stderr,((_tmp6CE="?",
_tag_dyneither(_tmp6CE,sizeof(char),2))),_tag_dyneither(_tmp6CD,sizeof(void*),0)));}
else{void*_tmp6CF;(_tmp6CF=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)
_tmpE2->v),_tag_dyneither(_tmp6CF,sizeof(void*),0)));}if(_tmpD7->tl != 0){const
char*_tmp6D2;void*_tmp6D1;(_tmp6D1=0,Cyc_fprintf(Cyc_stderr,((_tmp6D2=",",
_tag_dyneither(_tmp6D2,sizeof(char),2))),_tag_dyneither(_tmp6D1,sizeof(void*),0)));}}{
const char*_tmp6D5;void*_tmp6D4;(_tmp6D4=0,Cyc_fprintf(Cyc_stderr,((_tmp6D5=") ",
_tag_dyneither(_tmp6D5,sizeof(char),3))),_tag_dyneither(_tmp6D4,sizeof(void*),0)));}
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_struct*_tmpD8=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD8->tag != 3)goto _LL95;}_LL94:{const char*_tmp6D8;void*_tmp6D7;(
_tmp6D7=0,Cyc_fprintf(Cyc_stderr,((_tmp6D8="Function_mod()",_tag_dyneither(
_tmp6D8,sizeof(char),15))),_tag_dyneither(_tmp6D7,sizeof(void*),0)));}goto _LL8C;
_LL95: {struct Cyc_Absyn_Attributes_mod_struct*_tmpD9=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpD1;if(_tmpD9->tag != 5)goto _LL97;}_LL96:{const char*_tmp6DB;void*_tmp6DA;(
_tmp6DA=0,Cyc_fprintf(Cyc_stderr,((_tmp6DB="Attributes_mod ",_tag_dyneither(
_tmp6DB,sizeof(char),16))),_tag_dyneither(_tmp6DA,sizeof(void*),0)));}goto _LL8C;
_LL97: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpDA=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpD1;if(_tmpDA->tag != 4)goto _LL99;}_LL98:{const char*_tmp6DE;void*_tmp6DD;(
_tmp6DD=0,Cyc_fprintf(Cyc_stderr,((_tmp6DE="TypeParams_mod ",_tag_dyneither(
_tmp6DE,sizeof(char),16))),_tag_dyneither(_tmp6DD,sizeof(void*),0)));}goto _LL8C;
_LL99: {struct Cyc_Absyn_Pointer_mod_struct*_tmpDB=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpD1;if(_tmpDB->tag != 2)goto _LL8C;}_LL9A:{const char*_tmp6E1;void*_tmp6E0;(
_tmp6E0=0,Cyc_fprintf(Cyc_stderr,((_tmp6E1="Pointer_mod ",_tag_dyneither(_tmp6E1,
sizeof(char),13))),_tag_dyneither(_tmp6E0,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
const char*_tmp6E4;void*_tmp6E3;(_tmp6E3=0,Cyc_fprintf(Cyc_stderr,((_tmp6E4="\n",
_tag_dyneither(_tmp6E4,sizeof(char),2))),_tag_dyneither(_tmp6E3,sizeof(void*),0)));};}
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(
int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);const char*_tmp6E9;const
char*_tmp6E8;struct Cyc_PP_Doc*_tmp6E7[3];struct Cyc_PP_Doc*p_rest=(_tmp6E7[2]=Cyc_PP_text(((
_tmp6E8=")",_tag_dyneither(_tmp6E8,sizeof(char),2)))),((_tmp6E7[1]=rest,((
_tmp6E7[0]=Cyc_PP_text(((_tmp6E9="(",_tag_dyneither(_tmp6E9,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp6E7,sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpF4=(
void*)tms->hd;union Cyc_Absyn_Constraint*_tmpF6;struct Cyc_Absyn_Exp*_tmpF8;union
Cyc_Absyn_Constraint*_tmpF9;void*_tmpFB;struct Cyc_List_List*_tmpFD;struct Cyc_List_List*
_tmpFF;unsigned int _tmp100;int _tmp101;struct Cyc_Absyn_PtrAtts _tmp103;void*
_tmp104;union Cyc_Absyn_Constraint*_tmp105;union Cyc_Absyn_Constraint*_tmp106;
union Cyc_Absyn_Constraint*_tmp107;struct Cyc_Absyn_Tqual _tmp108;_LL9C: {struct Cyc_Absyn_Carray_mod_struct*
_tmpF5=(struct Cyc_Absyn_Carray_mod_struct*)_tmpF4;if(_tmpF5->tag != 0)goto _LL9E;
else{_tmpF6=_tmpF5->f1;}}_LL9D: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=
p_rest;{const char*_tmp6EE;const char*_tmp6ED;struct Cyc_PP_Doc*_tmp6EC[2];return(
_tmp6EC[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpF6)?Cyc_PP_text(((_tmp6ED="[]@zeroterm",_tag_dyneither(_tmp6ED,sizeof(char),
12)))): Cyc_PP_text(((_tmp6EE="[]",_tag_dyneither(_tmp6EE,sizeof(char),3)))),((
_tmp6EC[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6EC,sizeof(struct Cyc_PP_Doc*),2)))));};
_LL9E: {struct Cyc_Absyn_ConstArray_mod_struct*_tmpF7=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpF4;if(_tmpF7->tag != 1)goto _LLA0;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;}}
_LL9F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{const char*_tmp6F5;
const char*_tmp6F4;const char*_tmp6F3;struct Cyc_PP_Doc*_tmp6F2[4];return(_tmp6F2[3]=((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF9)?Cyc_PP_text(((
_tmp6F3="]@zeroterm",_tag_dyneither(_tmp6F3,sizeof(char),11)))): Cyc_PP_text(((
_tmp6F4="]",_tag_dyneither(_tmp6F4,sizeof(char),2)))),((_tmp6F2[2]=Cyc_Absynpp_exp2doc(
_tmpF8),((_tmp6F2[1]=Cyc_PP_text(((_tmp6F5="[",_tag_dyneither(_tmp6F5,sizeof(
char),2)))),((_tmp6F2[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F2,sizeof(struct Cyc_PP_Doc*),
4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_struct*_tmpFA=(struct Cyc_Absyn_Function_mod_struct*)
_tmpF4;if(_tmpFA->tag != 3)goto _LLA2;else{_tmpFB=(void*)_tmpFA->f1;}}_LLA1: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmp110=_tmpFB;struct Cyc_List_List*_tmp112;int _tmp113;
struct Cyc_Absyn_VarargInfo*_tmp114;struct Cyc_Core_Opt*_tmp115;struct Cyc_List_List*
_tmp116;struct Cyc_List_List*_tmp118;unsigned int _tmp119;_LLA9: {struct Cyc_Absyn_WithTypes_struct*
_tmp111=(struct Cyc_Absyn_WithTypes_struct*)_tmp110;if(_tmp111->tag != 1)goto _LLAB;
else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;_tmp114=_tmp111->f3;_tmp115=_tmp111->f4;
_tmp116=_tmp111->f5;}}_LLAA: {struct Cyc_PP_Doc*_tmp6F6[2];return(_tmp6F6[1]=Cyc_Absynpp_funargs2doc(
_tmp112,_tmp113,_tmp114,_tmp115,_tmp116),((_tmp6F6[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp6F6,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_struct*
_tmp117=(struct Cyc_Absyn_NoTypes_struct*)_tmp110;if(_tmp117->tag != 0)goto _LLA8;
else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LLAC: {const char*_tmp6FD;const
char*_tmp6FC;const char*_tmp6FB;struct Cyc_PP_Doc*_tmp6FA[2];return(_tmp6FA[1]=Cyc_PP_group(((
_tmp6FD="(",_tag_dyneither(_tmp6FD,sizeof(char),2))),((_tmp6FC=")",
_tag_dyneither(_tmp6FC,sizeof(char),2))),((_tmp6FB=",",_tag_dyneither(_tmp6FB,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp118)),((
_tmp6FA[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6FA,sizeof(struct Cyc_PP_Doc*),2)))));}
_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_struct*_tmpFC=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpF4;if(_tmpFC->tag != 5)goto _LLA4;else{_tmpFD=_tmpFC->f2;}}_LLA3: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6FE[2];return(_tmp6FE[1]=Cyc_Absynpp_atts2doc(_tmpFD),((
_tmp6FE[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6FE,sizeof(struct Cyc_PP_Doc*),2)))));};
case Cyc_Cyclone_Vc_c: _LLAE: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp6FF[2];return(_tmp6FF[1]=rest,((_tmp6FF[0]=Cyc_Absynpp_callconv2doc(_tmpFD),
Cyc_PP_cat(_tag_dyneither(_tmp6FF,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;}
_LLA4: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpFE=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpF4;if(_tmpFE->tag != 4)goto _LLA6;else{_tmpFF=_tmpFE->f1;_tmp100=_tmpFE->f2;
_tmp101=_tmpFE->f3;}}_LLA5: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp101){struct Cyc_PP_Doc*_tmp700[2];return(_tmp700[1]=Cyc_Absynpp_ktvars2doc(
_tmpFF),((_tmp700[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp700,sizeof(struct Cyc_PP_Doc*),
2)))));}else{struct Cyc_PP_Doc*_tmp701[2];return(_tmp701[1]=Cyc_Absynpp_tvars2doc(
_tmpFF),((_tmp701[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp701,sizeof(struct Cyc_PP_Doc*),
2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_struct*_tmp102=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpF4;if(_tmp102->tag != 2)goto _LL9B;else{_tmp103=_tmp102->f1;_tmp104=_tmp103.rgn;
_tmp105=_tmp103.nullable;_tmp106=_tmp103.bounds;_tmp107=_tmp103.zero_term;
_tmp108=_tmp102->f2;}}_LLA7: {struct Cyc_PP_Doc*ptr;{void*_tmp123=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp106);struct Cyc_Absyn_Exp*_tmp126;_LLB1: {struct Cyc_Absyn_DynEither_b_struct*
_tmp124=(struct Cyc_Absyn_DynEither_b_struct*)_tmp123;if(_tmp124->tag != 0)goto
_LLB3;}_LLB2: ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_struct*
_tmp125=(struct Cyc_Absyn_Upper_b_struct*)_tmp123;if(_tmp125->tag != 1)goto _LLB0;
else{_tmp126=_tmp125->f1;}}_LLB4:{const char*_tmp703;const char*_tmp702;ptr=Cyc_PP_text(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp105)?(
_tmp703="*",_tag_dyneither(_tmp703,sizeof(char),2)):((_tmp702="@",_tag_dyneither(
_tmp702,sizeof(char),2))));}{unsigned int _tmp12A;int _tmp12B;struct _tuple11
_tmp129=Cyc_Evexp_eval_const_uint_exp(_tmp126);_tmp12A=_tmp129.f1;_tmp12B=
_tmp129.f2;if(!_tmp12B  || _tmp12A != 1){struct Cyc_PP_Doc*_tmp704[4];ptr=((_tmp704[
3]=Cyc_Absynpp_rb(),((_tmp704[2]=Cyc_Absynpp_exp2doc(_tmp126),((_tmp704[1]=Cyc_Absynpp_lb(),((
_tmp704[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp704,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;};_LLB0:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){const char*
_tmp707;struct Cyc_PP_Doc*_tmp706[2];ptr=((_tmp706[1]=Cyc_PP_text(((_tmp707="@zeroterm ",
_tag_dyneither(_tmp707,sizeof(char),11)))),((_tmp706[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp706,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){
const char*_tmp70A;struct Cyc_PP_Doc*_tmp709[2];ptr=((_tmp709[1]=Cyc_PP_text(((
_tmp70A="@nozeroterm ",_tag_dyneither(_tmp70A,sizeof(char),13)))),((_tmp709[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp709,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmp131=Cyc_Tcutil_compress(_tmp104);_LLB6: {struct Cyc_Absyn_HeapRgn_struct*
_tmp132=(struct Cyc_Absyn_HeapRgn_struct*)_tmp131;if(_tmp132->tag != 20)goto _LLB8;}
_LLB7: goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_struct*_tmp133=(struct Cyc_Absyn_Evar_struct*)
_tmp131;if(_tmp133->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto
_LLBA;_LLB9: goto _LLB5;_LLBA:;_LLBB: {const char*_tmp70D;struct Cyc_PP_Doc*_tmp70C[
3];ptr=((_tmp70C[2]=Cyc_PP_text(((_tmp70D=" ",_tag_dyneither(_tmp70D,sizeof(char),
2)))),((_tmp70C[1]=Cyc_Absynpp_rgn2doc(_tmp104),((_tmp70C[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp70C,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}{struct Cyc_PP_Doc*
_tmp70E[2];ptr=((_tmp70E[1]=Cyc_Absynpp_tqual2doc(_tmp108),((_tmp70E[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp70E,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp70F[2];return(_tmp70F[1]=rest,((_tmp70F[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp70F,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t){void*_tmp13B=Cyc_Tcutil_compress(t);_LLBD: {struct Cyc_Absyn_HeapRgn_struct*
_tmp13C=(struct Cyc_Absyn_HeapRgn_struct*)_tmp13B;if(_tmp13C->tag != 20)goto _LLBF;}
_LLBE: {const char*_tmp710;return Cyc_PP_text(((_tmp710="`H",_tag_dyneither(
_tmp710,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_struct*_tmp13D=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp13B;if(_tmp13D->tag != 21)goto _LLC1;}_LLC0: {
const char*_tmp711;return Cyc_PP_text(((_tmp711="`U",_tag_dyneither(_tmp711,
sizeof(char),3))));}_LLC1: {struct Cyc_Absyn_RefCntRgn_struct*_tmp13E=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp13B;if(_tmp13E->tag != 22)goto _LLC3;}_LLC2: {const char*_tmp712;return Cyc_PP_text(((
_tmp712="`RC",_tag_dyneither(_tmp712,sizeof(char),4))));}_LLC3: {struct Cyc_Absyn_VarType_struct*
_tmp13F=(struct Cyc_Absyn_VarType_struct*)_tmp13B;if(_tmp13F->tag != 2)goto _LLC5;}
_LLC4: return Cyc_Absynpp_ntyp2doc(t);_LLC5:;_LLC6: {const char*_tmp717;const char*
_tmp716;struct Cyc_PP_Doc*_tmp715[3];return(_tmp715[2]=Cyc_PP_text(((_tmp716=")",
_tag_dyneither(_tmp716,sizeof(char),2)))),((_tmp715[1]=Cyc_Absynpp_ntyp2doc(t),((
_tmp715[0]=Cyc_PP_text(((_tmp717="@region(",_tag_dyneither(_tmp717,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp715,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LLBC:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t){void*_tmp146=Cyc_Tcutil_compress(t);void*_tmp148;struct Cyc_List_List*
_tmp14A;_LLC8: {struct Cyc_Absyn_AccessEff_struct*_tmp147=(struct Cyc_Absyn_AccessEff_struct*)
_tmp146;if(_tmp147->tag != 23)goto _LLCA;else{_tmp148=(void*)_tmp147->f1;}}_LLC9:{
struct Cyc_List_List*_tmp718;*rgions=((_tmp718=_cycalloc(sizeof(*_tmp718)),((
_tmp718->hd=Cyc_Absynpp_rgn2doc(_tmp148),((_tmp718->tl=*rgions,_tmp718))))));}
goto _LLC7;_LLCA: {struct Cyc_Absyn_JoinEff_struct*_tmp149=(struct Cyc_Absyn_JoinEff_struct*)
_tmp146;if(_tmp149->tag != 24)goto _LLCC;else{_tmp14A=_tmp149->f1;}}_LLCB: for(0;
_tmp14A != 0;_tmp14A=_tmp14A->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)
_tmp14A->hd);}goto _LLC7;_LLCC:;_LLCD:{struct Cyc_List_List*_tmp719;*effects=((
_tmp719=_cycalloc(sizeof(*_tmp719)),((_tmp719->hd=Cyc_Absynpp_typ2doc(t),((
_tmp719->tl=*effects,_tmp719))))));}goto _LLC7;_LLC7:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(
void*t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0){const char*_tmp71C;const char*_tmp71B;const
char*_tmp71A;return Cyc_PP_group(((_tmp71A="",_tag_dyneither(_tmp71A,sizeof(char),
1))),((_tmp71B="",_tag_dyneither(_tmp71B,sizeof(char),1))),((_tmp71C="+",
_tag_dyneither(_tmp71C,sizeof(char),2))),effects);}else{const char*_tmp71D;struct
Cyc_PP_Doc*_tmp150=Cyc_Absynpp_group_braces(((_tmp71D=",",_tag_dyneither(_tmp71D,
sizeof(char),2))),rgions);struct Cyc_List_List*_tmp721;const char*_tmp720;const
char*_tmp71F;const char*_tmp71E;return Cyc_PP_group(((_tmp71E="",_tag_dyneither(
_tmp71E,sizeof(char),1))),((_tmp71F="",_tag_dyneither(_tmp71F,sizeof(char),1))),((
_tmp720="+",_tag_dyneither(_tmp720,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp721=_cycalloc(sizeof(*_tmp721)),((_tmp721->hd=_tmp150,((_tmp721->tl=0,
_tmp721))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind
k){switch(k){case Cyc_Absyn_StructA: _LLCE: {const char*_tmp722;return Cyc_PP_text(((
_tmp722="struct ",_tag_dyneither(_tmp722,sizeof(char),8))));}case Cyc_Absyn_UnionA:
_LLCF: {const char*_tmp723;return Cyc_PP_text(((_tmp723="union ",_tag_dyneither(
_tmp723,sizeof(char),7))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;{void*_tmp158=t;struct Cyc_Core_Opt*_tmp15E;struct Cyc_Core_Opt*
_tmp15F;int _tmp160;struct Cyc_Core_Opt*_tmp161;struct Cyc_Absyn_Tvar*_tmp163;
struct Cyc_Absyn_DatatypeInfo _tmp165;union Cyc_Absyn_DatatypeInfoU _tmp166;struct
Cyc_List_List*_tmp167;struct Cyc_Absyn_DatatypeFieldInfo _tmp169;union Cyc_Absyn_DatatypeFieldInfoU
_tmp16A;struct Cyc_List_List*_tmp16B;enum Cyc_Absyn_Sign _tmp16D;enum Cyc_Absyn_Size_of
_tmp16E;int _tmp170;struct Cyc_List_List*_tmp172;struct Cyc_Absyn_AggrInfo _tmp174;
union Cyc_Absyn_AggrInfoU _tmp175;struct Cyc_List_List*_tmp176;enum Cyc_Absyn_AggrKind
_tmp178;struct Cyc_List_List*_tmp179;struct Cyc_List_List*_tmp17B;struct _tuple0*
_tmp17D;struct Cyc_Absyn_Exp*_tmp17F;struct _tuple0*_tmp181;struct Cyc_List_List*
_tmp182;struct Cyc_Absyn_Typedefdecl*_tmp183;void*_tmp185;void*_tmp187;void*
_tmp188;void*_tmp18A;void*_tmp18F;struct Cyc_Absyn_TypeDecl*_tmp193;struct Cyc_Absyn_TypeDecl
_tmp194;void*_tmp195;struct Cyc_Absyn_Aggrdecl*_tmp197;struct Cyc_Absyn_TypeDecl*
_tmp199;struct Cyc_Absyn_TypeDecl _tmp19A;void*_tmp19B;struct Cyc_Absyn_Enumdecl*
_tmp19D;struct Cyc_Absyn_TypeDecl*_tmp19F;struct Cyc_Absyn_TypeDecl _tmp1A0;void*
_tmp1A1;struct Cyc_Absyn_Datatypedecl*_tmp1A3;_LLD2: {struct Cyc_Absyn_ArrayType_struct*
_tmp159=(struct Cyc_Absyn_ArrayType_struct*)_tmp158;if(_tmp159->tag != 8)goto _LLD4;}
_LLD3: {const char*_tmp724;return Cyc_PP_text(((_tmp724="[[[array]]]",
_tag_dyneither(_tmp724,sizeof(char),12))));}_LLD4: {struct Cyc_Absyn_FnType_struct*
_tmp15A=(struct Cyc_Absyn_FnType_struct*)_tmp158;if(_tmp15A->tag != 9)goto _LLD6;}
_LLD5: return Cyc_PP_nil_doc();_LLD6: {struct Cyc_Absyn_PointerType_struct*_tmp15B=(
struct Cyc_Absyn_PointerType_struct*)_tmp158;if(_tmp15B->tag != 5)goto _LLD8;}_LLD7:
return Cyc_PP_nil_doc();_LLD8: {struct Cyc_Absyn_VoidType_struct*_tmp15C=(struct
Cyc_Absyn_VoidType_struct*)_tmp158;if(_tmp15C->tag != 0)goto _LLDA;}_LLD9:{const
char*_tmp725;s=Cyc_PP_text(((_tmp725="void",_tag_dyneither(_tmp725,sizeof(char),
5))));}goto _LLD1;_LLDA: {struct Cyc_Absyn_Evar_struct*_tmp15D=(struct Cyc_Absyn_Evar_struct*)
_tmp158;if(_tmp15D->tag != 1)goto _LLDC;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15D->f2;
_tmp160=_tmp15D->f3;_tmp161=_tmp15D->f4;}}_LLDB: if(_tmp15F != 0)return Cyc_Absynpp_ntyp2doc((
void*)_tmp15F->v);else{const char*_tmp735;const char*_tmp734;const char*_tmp733;
struct Cyc_Int_pa_struct _tmp732;void*_tmp731[1];const char*_tmp730;const char*
_tmp72F;struct Cyc_PP_Doc*_tmp72E[6];s=((_tmp72E[5]=_tmp15E == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp15E->v),((_tmp72E[4]=Cyc_PP_text(((
_tmp72F=")::",_tag_dyneither(_tmp72F,sizeof(char),4)))),((_tmp72E[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp161 == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp161->v),((_tmp72E[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp732.tag=1,((
_tmp732.f1=(unsigned long)_tmp160,((_tmp731[0]=& _tmp732,Cyc_aprintf(((_tmp730="%d",
_tag_dyneither(_tmp730,sizeof(char),3))),_tag_dyneither(_tmp731,sizeof(void*),1))))))))),((
_tmp72E[1]=Cyc_PP_text(((_tmp733="(",_tag_dyneither(_tmp733,sizeof(char),2)))),((
_tmp72E[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp734="\\%",
_tag_dyneither(_tmp734,sizeof(char),3)))): Cyc_PP_text(((_tmp735="%",
_tag_dyneither(_tmp735,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp72E,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLD1;_LLDC: {struct Cyc_Absyn_VarType_struct*
_tmp162=(struct Cyc_Absyn_VarType_struct*)_tmp158;if(_tmp162->tag != 2)goto _LLDE;
else{_tmp163=_tmp162->f1;}}_LLDD: s=Cyc_PP_textptr(_tmp163->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp738;struct Cyc_PP_Doc*_tmp737[3];s=((_tmp737[2]=Cyc_Absynpp_kindbound2doc(
_tmp163->kind),((_tmp737[1]=Cyc_PP_text(((_tmp738="::",_tag_dyneither(_tmp738,
sizeof(char),3)))),((_tmp737[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp737,sizeof(
struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp163)){const char*_tmp73D;const char*_tmp73C;struct Cyc_PP_Doc*_tmp73B[3];s=((
_tmp73B[2]=Cyc_PP_text(((_tmp73C=" */",_tag_dyneither(_tmp73C,sizeof(char),4)))),((
_tmp73B[1]=s,((_tmp73B[0]=Cyc_PP_text(((_tmp73D="_ /* ",_tag_dyneither(_tmp73D,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp73B,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLD1;_LLDE: {struct Cyc_Absyn_DatatypeType_struct*_tmp164=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp158;if(_tmp164->tag != 3)goto _LLE0;else{_tmp165=_tmp164->f1;_tmp166=_tmp165.datatype_info;
_tmp167=_tmp165.targs;}}_LLDF:{union Cyc_Absyn_DatatypeInfoU _tmp1B3=_tmp166;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp1B4;struct _tuple0*_tmp1B5;int _tmp1B6;
struct Cyc_Absyn_Datatypedecl**_tmp1B7;struct Cyc_Absyn_Datatypedecl*_tmp1B8;
struct Cyc_Absyn_Datatypedecl _tmp1B9;struct _tuple0*_tmp1BA;int _tmp1BB;_LL10D: if((
_tmp1B3.UnknownDatatype).tag != 1)goto _LL10F;_tmp1B4=(struct Cyc_Absyn_UnknownDatatypeInfo)(
_tmp1B3.UnknownDatatype).val;_tmp1B5=_tmp1B4.name;_tmp1B6=_tmp1B4.is_extensible;
_LL10E: _tmp1BA=_tmp1B5;_tmp1BB=_tmp1B6;goto _LL110;_LL10F: if((_tmp1B3.KnownDatatype).tag
!= 2)goto _LL10C;_tmp1B7=(struct Cyc_Absyn_Datatypedecl**)(_tmp1B3.KnownDatatype).val;
_tmp1B8=*_tmp1B7;_tmp1B9=*_tmp1B8;_tmp1BA=_tmp1B9.name;_tmp1BB=_tmp1B9.is_extensible;
_LL110: {const char*_tmp73E;struct Cyc_PP_Doc*_tmp1BC=Cyc_PP_text(((_tmp73E="datatype ",
_tag_dyneither(_tmp73E,sizeof(char),10))));const char*_tmp73F;struct Cyc_PP_Doc*
_tmp1BD=_tmp1BB?Cyc_PP_text(((_tmp73F="@extensible ",_tag_dyneither(_tmp73F,
sizeof(char),13)))): Cyc_PP_nil_doc();{struct Cyc_PP_Doc*_tmp740[4];s=((_tmp740[3]=
Cyc_Absynpp_tps2doc(_tmp167),((_tmp740[2]=Cyc_Absynpp_qvar2doc(_tmp1BA),((
_tmp740[1]=_tmp1BC,((_tmp740[0]=_tmp1BD,Cyc_PP_cat(_tag_dyneither(_tmp740,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL10C;}_LL10C:;}goto _LLD1;_LLE0: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp168=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp158;if(_tmp168->tag != 4)goto _LLE2;else{_tmp169=_tmp168->f1;_tmp16A=_tmp169.field_info;
_tmp16B=_tmp169.targs;}}_LLE1:{union Cyc_Absyn_DatatypeFieldInfoU _tmp1C1=_tmp16A;
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp1C2;struct _tuple0*_tmp1C3;struct
_tuple0*_tmp1C4;int _tmp1C5;struct _tuple1 _tmp1C6;struct Cyc_Absyn_Datatypedecl*
_tmp1C7;struct Cyc_Absyn_Datatypedecl _tmp1C8;struct _tuple0*_tmp1C9;int _tmp1CA;
struct Cyc_Absyn_Datatypefield*_tmp1CB;struct Cyc_Absyn_Datatypefield _tmp1CC;
struct _tuple0*_tmp1CD;_LL112: if((_tmp1C1.UnknownDatatypefield).tag != 1)goto
_LL114;_tmp1C2=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1C1.UnknownDatatypefield).val;
_tmp1C3=_tmp1C2.datatype_name;_tmp1C4=_tmp1C2.field_name;_tmp1C5=_tmp1C2.is_extensible;
_LL113: _tmp1C9=_tmp1C3;_tmp1CA=_tmp1C5;_tmp1CD=_tmp1C4;goto _LL115;_LL114: if((
_tmp1C1.KnownDatatypefield).tag != 2)goto _LL111;_tmp1C6=(struct _tuple1)(_tmp1C1.KnownDatatypefield).val;
_tmp1C7=_tmp1C6.f1;_tmp1C8=*_tmp1C7;_tmp1C9=_tmp1C8.name;_tmp1CA=_tmp1C8.is_extensible;
_tmp1CB=_tmp1C6.f2;_tmp1CC=*_tmp1CB;_tmp1CD=_tmp1CC.name;_LL115: {const char*
_tmp742;const char*_tmp741;struct Cyc_PP_Doc*_tmp1CE=Cyc_PP_text(_tmp1CA?(_tmp742="@extensible datatype ",
_tag_dyneither(_tmp742,sizeof(char),22)):((_tmp741="datatype ",_tag_dyneither(
_tmp741,sizeof(char),10))));{const char*_tmp745;struct Cyc_PP_Doc*_tmp744[4];s=((
_tmp744[3]=Cyc_Absynpp_qvar2doc(_tmp1CD),((_tmp744[2]=Cyc_PP_text(((_tmp745=".",
_tag_dyneither(_tmp745,sizeof(char),2)))),((_tmp744[1]=Cyc_Absynpp_qvar2doc(
_tmp1C9),((_tmp744[0]=_tmp1CE,Cyc_PP_cat(_tag_dyneither(_tmp744,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL111;}_LL111:;}goto _LLD1;_LLE2: {struct Cyc_Absyn_IntType_struct*
_tmp16C=(struct Cyc_Absyn_IntType_struct*)_tmp158;if(_tmp16C->tag != 6)goto _LLE4;
else{_tmp16D=_tmp16C->f1;_tmp16E=_tmp16C->f2;}}_LLE3: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;switch(_tmp16D){case Cyc_Absyn_None: _LL116: goto _LL117;case
Cyc_Absyn_Signed: _LL117:{const char*_tmp746;sns=((_tmp746="",_tag_dyneither(
_tmp746,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL118:{const char*
_tmp747;sns=((_tmp747="unsigned ",_tag_dyneither(_tmp747,sizeof(char),10)));}
break;}switch(_tmp16E){case Cyc_Absyn_Char_sz: _LL11A: switch(_tmp16D){case Cyc_Absyn_None:
_LL11C:{const char*_tmp748;sns=((_tmp748="",_tag_dyneither(_tmp748,sizeof(char),1)));}
break;case Cyc_Absyn_Signed: _LL11D:{const char*_tmp749;sns=((_tmp749="signed ",
_tag_dyneither(_tmp749,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL11E:{
const char*_tmp74A;sns=((_tmp74A="unsigned ",_tag_dyneither(_tmp74A,sizeof(char),
10)));}break;}{const char*_tmp74B;ts=((_tmp74B="char",_tag_dyneither(_tmp74B,
sizeof(char),5)));}break;case Cyc_Absyn_Short_sz: _LL11B:{const char*_tmp74C;ts=((
_tmp74C="short",_tag_dyneither(_tmp74C,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz:
_LL120:{const char*_tmp74D;ts=((_tmp74D="int",_tag_dyneither(_tmp74D,sizeof(char),
4)));}break;case Cyc_Absyn_Long_sz: _LL121:{const char*_tmp74E;ts=((_tmp74E="long",
_tag_dyneither(_tmp74E,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL122:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL124:{const char*_tmp74F;
ts=((_tmp74F="long long",_tag_dyneither(_tmp74F,sizeof(char),10)));}break;case
Cyc_Cyclone_Vc_c: _LL125:{const char*_tmp750;ts=((_tmp750="__int64",_tag_dyneither(
_tmp750,sizeof(char),8)));}break;}break;}{const char*_tmp755;void*_tmp754[2];
struct Cyc_String_pa_struct _tmp753;struct Cyc_String_pa_struct _tmp752;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp752.tag=0,((_tmp752.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ts),((_tmp753.tag=0,((_tmp753.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sns),((_tmp754[0]=& _tmp753,((_tmp754[1]=& _tmp752,Cyc_aprintf(((
_tmp755="%s%s",_tag_dyneither(_tmp755,sizeof(char),5))),_tag_dyneither(_tmp754,
sizeof(void*),2)))))))))))))));}goto _LLD1;}_LLE4: {struct Cyc_Absyn_FloatType_struct*
_tmp16F=(struct Cyc_Absyn_FloatType_struct*)_tmp158;if(_tmp16F->tag != 7)goto _LLE6;
else{_tmp170=_tmp16F->f1;}}_LLE5: if(_tmp170 == 0){const char*_tmp756;s=Cyc_PP_text(((
_tmp756="float",_tag_dyneither(_tmp756,sizeof(char),6))));}else{if(_tmp170 == 1){
const char*_tmp757;s=Cyc_PP_text(((_tmp757="double",_tag_dyneither(_tmp757,
sizeof(char),7))));}else{const char*_tmp758;s=Cyc_PP_text(((_tmp758="long double",
_tag_dyneither(_tmp758,sizeof(char),12))));}}goto _LLD1;_LLE6: {struct Cyc_Absyn_TupleType_struct*
_tmp171=(struct Cyc_Absyn_TupleType_struct*)_tmp158;if(_tmp171->tag != 10)goto
_LLE8;else{_tmp172=_tmp171->f1;}}_LLE7:{struct Cyc_PP_Doc*_tmp759[2];s=((_tmp759[
1]=Cyc_Absynpp_args2doc(_tmp172),((_tmp759[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp759,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD1;_LLE8: {
struct Cyc_Absyn_AggrType_struct*_tmp173=(struct Cyc_Absyn_AggrType_struct*)
_tmp158;if(_tmp173->tag != 11)goto _LLEA;else{_tmp174=_tmp173->f1;_tmp175=_tmp174.aggr_info;
_tmp176=_tmp174.targs;}}_LLE9: {enum Cyc_Absyn_AggrKind _tmp1E7;struct _tuple0*
_tmp1E8;struct _tuple10 _tmp1E6=Cyc_Absyn_aggr_kinded_name(_tmp175);_tmp1E7=
_tmp1E6.f1;_tmp1E8=_tmp1E6.f2;{struct Cyc_PP_Doc*_tmp75A[3];s=((_tmp75A[2]=Cyc_Absynpp_tps2doc(
_tmp176),((_tmp75A[1]=Cyc_Absynpp_qvar2doc(_tmp1E8),((_tmp75A[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp1E7),Cyc_PP_cat(_tag_dyneither(_tmp75A,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLD1;}_LLEA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp177=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp158;if(_tmp177->tag != 12)goto _LLEC;else{_tmp178=_tmp177->f1;_tmp179=_tmp177->f2;}}
_LLEB:{struct Cyc_PP_Doc*_tmp75B[4];s=((_tmp75B[3]=Cyc_Absynpp_rb(),((_tmp75B[2]=
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp179)),((_tmp75B[1]=Cyc_Absynpp_lb(),((
_tmp75B[0]=Cyc_Absynpp_aggr_kind2doc(_tmp178),Cyc_PP_cat(_tag_dyneither(_tmp75B,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD1;_LLEC: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp17A=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp158;if(_tmp17A->tag != 14)goto
_LLEE;else{_tmp17B=_tmp17A->f1;}}_LLED:{const char*_tmp75E;struct Cyc_PP_Doc*
_tmp75D[4];s=((_tmp75D[3]=Cyc_Absynpp_rb(),((_tmp75D[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(
_tmp17B)),((_tmp75D[1]=Cyc_Absynpp_lb(),((_tmp75D[0]=Cyc_PP_text(((_tmp75E="enum ",
_tag_dyneither(_tmp75E,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp75D,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD1;_LLEE: {struct Cyc_Absyn_EnumType_struct*
_tmp17C=(struct Cyc_Absyn_EnumType_struct*)_tmp158;if(_tmp17C->tag != 13)goto _LLF0;
else{_tmp17D=_tmp17C->f1;}}_LLEF:{const char*_tmp761;struct Cyc_PP_Doc*_tmp760[2];
s=((_tmp760[1]=Cyc_Absynpp_qvar2doc(_tmp17D),((_tmp760[0]=Cyc_PP_text(((_tmp761="enum ",
_tag_dyneither(_tmp761,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp760,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD1;_LLF0: {struct Cyc_Absyn_ValueofType_struct*
_tmp17E=(struct Cyc_Absyn_ValueofType_struct*)_tmp158;if(_tmp17E->tag != 18)goto
_LLF2;else{_tmp17F=_tmp17E->f1;}}_LLF1:{const char*_tmp766;const char*_tmp765;
struct Cyc_PP_Doc*_tmp764[3];s=((_tmp764[2]=Cyc_PP_text(((_tmp765=")",
_tag_dyneither(_tmp765,sizeof(char),2)))),((_tmp764[1]=Cyc_Absynpp_exp2doc(
_tmp17F),((_tmp764[0]=Cyc_PP_text(((_tmp766="valueof_t(",_tag_dyneither(_tmp766,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp764,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLD1;_LLF2: {struct Cyc_Absyn_TypedefType_struct*_tmp180=(struct
Cyc_Absyn_TypedefType_struct*)_tmp158;if(_tmp180->tag != 17)goto _LLF4;else{
_tmp181=_tmp180->f1;_tmp182=_tmp180->f2;_tmp183=_tmp180->f3;}}_LLF3:{struct Cyc_PP_Doc*
_tmp767[2];s=((_tmp767[1]=Cyc_Absynpp_tps2doc(_tmp182),((_tmp767[0]=Cyc_Absynpp_qvar2doc(
_tmp181),Cyc_PP_cat(_tag_dyneither(_tmp767,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD1;_LLF4: {struct Cyc_Absyn_RgnHandleType_struct*_tmp184=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp158;if(_tmp184->tag != 15)goto _LLF6;else{_tmp185=(void*)_tmp184->f1;}}_LLF5:{
const char*_tmp76C;const char*_tmp76B;struct Cyc_PP_Doc*_tmp76A[3];s=((_tmp76A[2]=
Cyc_PP_text(((_tmp76B=">",_tag_dyneither(_tmp76B,sizeof(char),2)))),((_tmp76A[1]=
Cyc_Absynpp_rgn2doc(_tmp185),((_tmp76A[0]=Cyc_PP_text(((_tmp76C="region_t<",
_tag_dyneither(_tmp76C,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp76A,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF6: {struct Cyc_Absyn_DynRgnType_struct*
_tmp186=(struct Cyc_Absyn_DynRgnType_struct*)_tmp158;if(_tmp186->tag != 16)goto
_LLF8;else{_tmp187=(void*)_tmp186->f1;_tmp188=(void*)_tmp186->f2;}}_LLF7:{const
char*_tmp773;const char*_tmp772;const char*_tmp771;struct Cyc_PP_Doc*_tmp770[5];s=((
_tmp770[4]=Cyc_PP_text(((_tmp771=">",_tag_dyneither(_tmp771,sizeof(char),2)))),((
_tmp770[3]=Cyc_Absynpp_rgn2doc(_tmp188),((_tmp770[2]=Cyc_PP_text(((_tmp772=",",
_tag_dyneither(_tmp772,sizeof(char),2)))),((_tmp770[1]=Cyc_Absynpp_rgn2doc(
_tmp187),((_tmp770[0]=Cyc_PP_text(((_tmp773="dynregion_t<",_tag_dyneither(
_tmp773,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp770,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLD1;_LLF8: {struct Cyc_Absyn_TagType_struct*_tmp189=(struct
Cyc_Absyn_TagType_struct*)_tmp158;if(_tmp189->tag != 19)goto _LLFA;else{_tmp18A=(
void*)_tmp189->f1;}}_LLF9:{const char*_tmp778;const char*_tmp777;struct Cyc_PP_Doc*
_tmp776[3];s=((_tmp776[2]=Cyc_PP_text(((_tmp777=">",_tag_dyneither(_tmp777,
sizeof(char),2)))),((_tmp776[1]=Cyc_Absynpp_typ2doc(_tmp18A),((_tmp776[0]=Cyc_PP_text(((
_tmp778="tag_t<",_tag_dyneither(_tmp778,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp776,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLFA: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp18B=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp158;if(_tmp18B->tag != 21)goto _LLFC;}_LLFB: goto _LLFD;_LLFC: {struct Cyc_Absyn_HeapRgn_struct*
_tmp18C=(struct Cyc_Absyn_HeapRgn_struct*)_tmp158;if(_tmp18C->tag != 20)goto _LLFE;}
_LLFD: goto _LLFF;_LLFE: {struct Cyc_Absyn_RefCntRgn_struct*_tmp18D=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp158;if(_tmp18D->tag != 22)goto _LL100;}_LLFF: s=Cyc_Absynpp_rgn2doc(t);goto
_LLD1;_LL100: {struct Cyc_Absyn_RgnsEff_struct*_tmp18E=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp158;if(_tmp18E->tag != 25)goto _LL102;else{_tmp18F=(void*)_tmp18E->f1;}}_LL101:{
const char*_tmp77D;const char*_tmp77C;struct Cyc_PP_Doc*_tmp77B[3];s=((_tmp77B[2]=
Cyc_PP_text(((_tmp77C=")",_tag_dyneither(_tmp77C,sizeof(char),2)))),((_tmp77B[1]=
Cyc_Absynpp_typ2doc(_tmp18F),((_tmp77B[0]=Cyc_PP_text(((_tmp77D="regions(",
_tag_dyneither(_tmp77D,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp77B,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LL102: {struct Cyc_Absyn_AccessEff_struct*
_tmp190=(struct Cyc_Absyn_AccessEff_struct*)_tmp158;if(_tmp190->tag != 23)goto
_LL104;}_LL103: goto _LL105;_LL104: {struct Cyc_Absyn_JoinEff_struct*_tmp191=(
struct Cyc_Absyn_JoinEff_struct*)_tmp158;if(_tmp191->tag != 24)goto _LL106;}_LL105:
s=Cyc_Absynpp_eff2doc(t);goto _LLD1;_LL106: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp192=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp158;if(_tmp192->tag != 26)goto
_LL108;else{_tmp193=_tmp192->f1;_tmp194=*_tmp193;_tmp195=_tmp194.r;{struct Cyc_Absyn_Aggr_td_struct*
_tmp196=(struct Cyc_Absyn_Aggr_td_struct*)_tmp195;if(_tmp196->tag != 0)goto _LL108;
else{_tmp197=_tmp196->f1;}};}}_LL107: s=Cyc_Absynpp_aggrdecl2doc(_tmp197);goto
_LLD1;_LL108: {struct Cyc_Absyn_TypeDeclType_struct*_tmp198=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp158;if(_tmp198->tag != 26)goto _LL10A;else{_tmp199=_tmp198->f1;_tmp19A=*
_tmp199;_tmp19B=_tmp19A.r;{struct Cyc_Absyn_Enum_td_struct*_tmp19C=(struct Cyc_Absyn_Enum_td_struct*)
_tmp19B;if(_tmp19C->tag != 1)goto _LL10A;else{_tmp19D=_tmp19C->f1;}};}}_LL109: s=
Cyc_Absynpp_enumdecl2doc(_tmp19D);goto _LLD1;_LL10A: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp19E=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp158;if(_tmp19E->tag != 26)goto
_LLD1;else{_tmp19F=_tmp19E->f1;_tmp1A0=*_tmp19F;_tmp1A1=_tmp1A0.r;{struct Cyc_Absyn_Datatype_td_struct*
_tmp1A2=(struct Cyc_Absyn_Datatype_td_struct*)_tmp1A1;if(_tmp1A2->tag != 2)goto
_LLD1;else{_tmp1A3=_tmp1A2->f1;}};}}_LL10B: s=Cyc_Absynpp_datatypedecl2doc(
_tmp1A3);goto _LLD1;_LLD1:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct
Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct
_dyneither_ptr*)vo->v));}struct _tuple17{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(
struct _tuple17*cmp){struct _tuple17 _tmp201;void*_tmp202;void*_tmp203;struct
_tuple17*_tmp200=cmp;_tmp201=*_tmp200;_tmp202=_tmp201.f1;_tmp203=_tmp201.f2;{
const char*_tmp780;struct Cyc_PP_Doc*_tmp77F[3];return(_tmp77F[2]=Cyc_Absynpp_rgn2doc(
_tmp203),((_tmp77F[1]=Cyc_PP_text(((_tmp780=" > ",_tag_dyneither(_tmp780,sizeof(
char),4)))),((_tmp77F[0]=Cyc_Absynpp_rgn2doc(_tmp202),Cyc_PP_cat(_tag_dyneither(
_tmp77F,sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List*po){const char*_tmp783;const char*_tmp782;const char*_tmp781;
return Cyc_PP_group(((_tmp781="",_tag_dyneither(_tmp781,sizeof(char),1))),((
_tmp782="",_tag_dyneither(_tmp782,sizeof(char),1))),((_tmp783=",",_tag_dyneither(
_tmp783,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){struct Cyc_Core_Opt*
_tmp784;struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:((_tmp784=_cycalloc(sizeof(*
_tmp784)),((_tmp784->v=Cyc_PP_text(*((struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v)),_tmp784))));return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp20A=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs){const char*_tmp787;struct Cyc_List_List*
_tmp786;_tmp20A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp20A,((_tmp786=_cycalloc(sizeof(*_tmp786)),((_tmp786->hd=
Cyc_PP_text(((_tmp787="...",_tag_dyneither(_tmp787,sizeof(char),4)))),((_tmp786->tl=
0,_tmp786)))))));}else{if(cyc_varargs != 0){const char*_tmp790;const char*_tmp78F;
const char*_tmp78E;struct _tuple8*_tmp78D;struct Cyc_PP_Doc*_tmp78C[3];struct Cyc_PP_Doc*
_tmp20D=(_tmp78C[2]=Cyc_Absynpp_funarg2doc(((_tmp78D=_cycalloc(sizeof(*_tmp78D)),((
_tmp78D->f1=cyc_varargs->name,((_tmp78D->f2=cyc_varargs->tq,((_tmp78D->f3=
cyc_varargs->type,_tmp78D))))))))),((_tmp78C[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp78E=" inject ",_tag_dyneither(_tmp78E,sizeof(char),9)))): Cyc_PP_text(((
_tmp78F=" ",_tag_dyneither(_tmp78F,sizeof(char),2)))),((_tmp78C[0]=Cyc_PP_text(((
_tmp790="...",_tag_dyneither(_tmp790,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp78C,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp791;_tmp20A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp20A,((_tmp791=_cycalloc(sizeof(*_tmp791)),((_tmp791->hd=
_tmp20D,((_tmp791->tl=0,_tmp791)))))));}}{const char*_tmp794;const char*_tmp793;
const char*_tmp792;struct Cyc_PP_Doc*_tmp214=Cyc_PP_group(((_tmp792="",
_tag_dyneither(_tmp792,sizeof(char),1))),((_tmp793="",_tag_dyneither(_tmp793,
sizeof(char),1))),((_tmp794=",",_tag_dyneither(_tmp794,sizeof(char),2))),_tmp20A);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp797;struct Cyc_PP_Doc*
_tmp796[3];_tmp214=((_tmp796[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp796[
1]=Cyc_PP_text(((_tmp797=";",_tag_dyneither(_tmp797,sizeof(char),2)))),((_tmp796[
0]=_tmp214,Cyc_PP_cat(_tag_dyneither(_tmp796,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp79A;struct Cyc_PP_Doc*_tmp799[3];_tmp214=((_tmp799[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp799[1]=Cyc_PP_text(((_tmp79A=":",
_tag_dyneither(_tmp79A,sizeof(char),2)))),((_tmp799[0]=_tmp214,Cyc_PP_cat(
_tag_dyneither(_tmp799,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp79F;
const char*_tmp79E;struct Cyc_PP_Doc*_tmp79D[3];return(_tmp79D[2]=Cyc_PP_text(((
_tmp79E=")",_tag_dyneither(_tmp79E,sizeof(char),2)))),((_tmp79D[1]=_tmp214,((
_tmp79D[0]=Cyc_PP_text(((_tmp79F="(",_tag_dyneither(_tmp79F,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp79D,sizeof(struct Cyc_PP_Doc*),3)))))));};};}struct
_tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple12*arg){struct Cyc_Core_Opt*_tmp7A2;
struct _tuple8*_tmp7A1;return(_tmp7A1=_cycalloc(sizeof(*_tmp7A1)),((_tmp7A1->f1=((
_tmp7A2=_cycalloc(sizeof(*_tmp7A2)),((_tmp7A2->v=(*arg).f1,_tmp7A2)))),((_tmp7A1->f2=(*
arg).f2,((_tmp7A1->f3=(*arg).f3,_tmp7A1)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(
struct _dyneither_ptr*v){return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*q){struct Cyc_List_List*_tmp221=0;int match;{union Cyc_Absyn_Nmspace
_tmp222=(*q).f1;int _tmp223;struct Cyc_List_List*_tmp224;struct Cyc_List_List*
_tmp225;struct Cyc_List_List*_tmp226;_LL128: if((_tmp222.Loc_n).tag != 4)goto _LL12A;
_tmp223=(int)(_tmp222.Loc_n).val;_LL129: _tmp224=0;goto _LL12B;_LL12A: if((_tmp222.Rel_n).tag
!= 1)goto _LL12C;_tmp224=(struct Cyc_List_List*)(_tmp222.Rel_n).val;_LL12B: match=0;
_tmp221=_tmp224;goto _LL127;_LL12C: if((_tmp222.C_n).tag != 3)goto _LL12E;_tmp225=(
struct Cyc_List_List*)(_tmp222.C_n).val;_LL12D: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp225,Cyc_Absynpp_curr_namespace);
goto _LL127;_LL12E: if((_tmp222.Abs_n).tag != 2)goto _LL127;_tmp226=(struct Cyc_List_List*)(
_tmp222.Abs_n).val;_LL12F: match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp226,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp7A3;_tmp221=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp7A3=_cycalloc(sizeof(*_tmp7A3)),((_tmp7A3->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp7A3->tl=_tmp226,_tmp7A3))))): _tmp226;}goto _LL127;_LL127:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp7A5;struct Cyc_List_List*_tmp7A4;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp221,((_tmp7A4=_cycalloc(sizeof(*_tmp7A4)),((_tmp7A4->hd=(*q).f2,((_tmp7A4->tl=
0,_tmp7A4))))))),((_tmp7A5="_",_tag_dyneither(_tmp7A5,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp7A7;struct Cyc_List_List*_tmp7A6;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp221,((_tmp7A6=_cycalloc(sizeof(*
_tmp7A6)),((_tmp7A6->hd=(*q).f2,((_tmp7A6->tl=0,_tmp7A6))))))),((_tmp7A7="::",
_tag_dyneither(_tmp7A7,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*
Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct _dyneither_ptr _tmp22C=Cyc_Absynpp_qvar2string(
q);if(Cyc_PP_tex_output){const char*_tmp7A9;const char*_tmp7A8;return Cyc_PP_text_width((
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp7A8="\\textbf{",
_tag_dyneither(_tmp7A8,sizeof(char),9))),(struct _dyneither_ptr)_tmp22C),((
_tmp7A9="}",_tag_dyneither(_tmp7A9,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp22C));}else{return Cyc_PP_text(_tmp22C);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)
return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace
_tmp22F=(*v).f1;int _tmp230;struct Cyc_List_List*_tmp231;struct Cyc_List_List*
_tmp232;struct Cyc_List_List*_tmp233;_LL131: if((_tmp22F.Loc_n).tag != 4)goto _LL133;
_tmp230=(int)(_tmp22F.Loc_n).val;_LL132: goto _LL134;_LL133: if((_tmp22F.Rel_n).tag
!= 1)goto _LL135;_tmp231=(struct Cyc_List_List*)(_tmp22F.Rel_n).val;if(_tmp231 != 0)
goto _LL135;_LL134: return*(*v).f2;_LL135: if((_tmp22F.C_n).tag != 3)goto _LL137;
_tmp232=(struct Cyc_List_List*)(_tmp22F.C_n).val;_LL136: _tmp233=_tmp232;goto
_LL138;_LL137: if((_tmp22F.Abs_n).tag != 2)goto _LL139;_tmp233=(struct Cyc_List_List*)(
_tmp22F.Abs_n).val;_LL138: if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_strptrcmp,_tmp233,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto
_LL13A;}_LL139:;_LL13A: {const char*_tmp7AA;return(struct _dyneither_ptr)Cyc_strconcat(((
_tmp7AA="/* bad namespace : */ ",_tag_dyneither(_tmp7AA,sizeof(char),23))),(
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL130:;}else{return*(*v).f2;}}
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){return Cyc_PP_text(
Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v){struct _dyneither_ptr _tmp235=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){const char*_tmp7AC;const char*_tmp7AB;return Cyc_PP_text_width((
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp7AB="\\textbf{",
_tag_dyneither(_tmp7AB,sizeof(char),9))),(struct _dyneither_ptr)_tmp235),((
_tmp7AC="}",_tag_dyneither(_tmp7AC,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp235));}else{return Cyc_PP_text(_tmp235);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmp238=e->r;enum Cyc_Absyn_Primop _tmp23D;struct Cyc_Absyn_Exp*
_tmp248;struct Cyc_Absyn_Exp*_tmp24A;_LL13C: {struct Cyc_Absyn_Const_e_struct*
_tmp239=(struct Cyc_Absyn_Const_e_struct*)_tmp238;if(_tmp239->tag != 0)goto _LL13E;}
_LL13D: goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_struct*_tmp23A=(struct Cyc_Absyn_Var_e_struct*)
_tmp238;if(_tmp23A->tag != 1)goto _LL140;}_LL13F: goto _LL141;_LL140: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp23B=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp238;if(_tmp23B->tag != 2)goto
_LL142;}_LL141: return 10000;_LL142: {struct Cyc_Absyn_Primop_e_struct*_tmp23C=(
struct Cyc_Absyn_Primop_e_struct*)_tmp238;if(_tmp23C->tag != 3)goto _LL144;else{
_tmp23D=_tmp23C->f1;}}_LL143: switch(_tmp23D){case Cyc_Absyn_Plus: _LL18E: return 100;
case Cyc_Absyn_Times: _LL18F: return 110;case Cyc_Absyn_Minus: _LL190: return 100;case
Cyc_Absyn_Div: _LL191: goto _LL192;case Cyc_Absyn_Mod: _LL192: return 110;case Cyc_Absyn_Eq:
_LL193: goto _LL194;case Cyc_Absyn_Neq: _LL194: return 70;case Cyc_Absyn_Gt: _LL195: goto
_LL196;case Cyc_Absyn_Lt: _LL196: goto _LL197;case Cyc_Absyn_Gte: _LL197: goto _LL198;
case Cyc_Absyn_Lte: _LL198: return 80;case Cyc_Absyn_Not: _LL199: goto _LL19A;case Cyc_Absyn_Bitnot:
_LL19A: return 130;case Cyc_Absyn_Bitand: _LL19B: return 60;case Cyc_Absyn_Bitor: _LL19C:
return 40;case Cyc_Absyn_Bitxor: _LL19D: return 50;case Cyc_Absyn_Bitlshift: _LL19E:
return 90;case Cyc_Absyn_Bitlrshift: _LL19F: return 80;case Cyc_Absyn_Bitarshift:
_LL1A0: return 80;case Cyc_Absyn_Numelts: _LL1A1: return 140;}_LL144: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp23E=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp238;if(_tmp23E->tag != 4)goto
_LL146;}_LL145: return 20;_LL146: {struct Cyc_Absyn_Increment_e_struct*_tmp23F=(
struct Cyc_Absyn_Increment_e_struct*)_tmp238;if(_tmp23F->tag != 5)goto _LL148;}
_LL147: return 130;_LL148: {struct Cyc_Absyn_Conditional_e_struct*_tmp240=(struct
Cyc_Absyn_Conditional_e_struct*)_tmp238;if(_tmp240->tag != 6)goto _LL14A;}_LL149:
return 30;_LL14A: {struct Cyc_Absyn_And_e_struct*_tmp241=(struct Cyc_Absyn_And_e_struct*)
_tmp238;if(_tmp241->tag != 7)goto _LL14C;}_LL14B: return 35;_LL14C: {struct Cyc_Absyn_Or_e_struct*
_tmp242=(struct Cyc_Absyn_Or_e_struct*)_tmp238;if(_tmp242->tag != 8)goto _LL14E;}
_LL14D: return 30;_LL14E: {struct Cyc_Absyn_SeqExp_e_struct*_tmp243=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp238;if(_tmp243->tag != 9)goto _LL150;}_LL14F: return 10;_LL150: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp244=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp238;if(_tmp244->tag != 10)goto
_LL152;}_LL151: goto _LL153;_LL152: {struct Cyc_Absyn_FnCall_e_struct*_tmp245=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp238;if(_tmp245->tag != 11)goto _LL154;}_LL153:
return 140;_LL154: {struct Cyc_Absyn_Throw_e_struct*_tmp246=(struct Cyc_Absyn_Throw_e_struct*)
_tmp238;if(_tmp246->tag != 12)goto _LL156;}_LL155: return 130;_LL156: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp247=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp238;if(_tmp247->tag != 13)
goto _LL158;else{_tmp248=_tmp247->f1;}}_LL157: return Cyc_Absynpp_exp_prec(_tmp248);
_LL158: {struct Cyc_Absyn_Instantiate_e_struct*_tmp249=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp238;if(_tmp249->tag != 14)goto _LL15A;else{_tmp24A=_tmp249->f1;}}_LL159: return
Cyc_Absynpp_exp_prec(_tmp24A);_LL15A: {struct Cyc_Absyn_Cast_e_struct*_tmp24B=(
struct Cyc_Absyn_Cast_e_struct*)_tmp238;if(_tmp24B->tag != 15)goto _LL15C;}_LL15B:
return 120;_LL15C: {struct Cyc_Absyn_New_e_struct*_tmp24C=(struct Cyc_Absyn_New_e_struct*)
_tmp238;if(_tmp24C->tag != 17)goto _LL15E;}_LL15D: return 15;_LL15E: {struct Cyc_Absyn_Address_e_struct*
_tmp24D=(struct Cyc_Absyn_Address_e_struct*)_tmp238;if(_tmp24D->tag != 16)goto
_LL160;}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp24E=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp238;if(_tmp24E->tag != 18)goto _LL162;}
_LL161: goto _LL163;_LL162: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp24F=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp238;if(_tmp24F->tag != 19)goto _LL164;}_LL163: goto _LL165;_LL164: {struct Cyc_Absyn_Valueof_e_struct*
_tmp250=(struct Cyc_Absyn_Valueof_e_struct*)_tmp238;if(_tmp250->tag != 39)goto
_LL166;}_LL165: goto _LL167;_LL166: {struct Cyc_Absyn_Asm_e_struct*_tmp251=(struct
Cyc_Absyn_Asm_e_struct*)_tmp238;if(_tmp251->tag != 40)goto _LL168;}_LL167: goto
_LL169;_LL168: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp252=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp238;if(_tmp252->tag != 38)goto _LL16A;}_LL169: goto _LL16B;_LL16A: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp253=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp238;if(_tmp253->tag != 20)goto
_LL16C;}_LL16B: goto _LL16D;_LL16C: {struct Cyc_Absyn_Deref_e_struct*_tmp254=(
struct Cyc_Absyn_Deref_e_struct*)_tmp238;if(_tmp254->tag != 21)goto _LL16E;}_LL16D:
return 130;_LL16E: {struct Cyc_Absyn_AggrMember_e_struct*_tmp255=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp238;if(_tmp255->tag != 22)goto _LL170;}_LL16F: goto _LL171;_LL170: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp256=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp238;if(_tmp256->tag != 23)goto
_LL172;}_LL171: goto _LL173;_LL172: {struct Cyc_Absyn_Subscript_e_struct*_tmp257=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp238;if(_tmp257->tag != 24)goto _LL174;}
_LL173: return 140;_LL174: {struct Cyc_Absyn_Tuple_e_struct*_tmp258=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp238;if(_tmp258->tag != 25)goto _LL176;}_LL175: return 150;_LL176: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp259=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp238;if(_tmp259->tag != 26)goto
_LL178;}_LL177: goto _LL179;_LL178: {struct Cyc_Absyn_Array_e_struct*_tmp25A=(
struct Cyc_Absyn_Array_e_struct*)_tmp238;if(_tmp25A->tag != 27)goto _LL17A;}_LL179:
goto _LL17B;_LL17A: {struct Cyc_Absyn_Comprehension_e_struct*_tmp25B=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp238;if(_tmp25B->tag != 28)goto _LL17C;}_LL17B: goto _LL17D;_LL17C: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp25C=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp238;if(_tmp25C->tag != 29)goto
_LL17E;}_LL17D: goto _LL17F;_LL17E: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp25D=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp238;if(_tmp25D->tag != 30)goto _LL180;}
_LL17F: goto _LL181;_LL180: {struct Cyc_Absyn_Datatype_e_struct*_tmp25E=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp238;if(_tmp25E->tag != 31)goto _LL182;}_LL181: goto _LL183;_LL182: {struct Cyc_Absyn_Enum_e_struct*
_tmp25F=(struct Cyc_Absyn_Enum_e_struct*)_tmp238;if(_tmp25F->tag != 32)goto _LL184;}
_LL183: goto _LL185;_LL184: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp260=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp238;if(_tmp260->tag != 33)goto _LL186;}_LL185: goto _LL187;_LL186: {struct Cyc_Absyn_Malloc_e_struct*
_tmp261=(struct Cyc_Absyn_Malloc_e_struct*)_tmp238;if(_tmp261->tag != 34)goto
_LL188;}_LL187: goto _LL189;_LL188: {struct Cyc_Absyn_Swap_e_struct*_tmp262=(struct
Cyc_Absyn_Swap_e_struct*)_tmp238;if(_tmp262->tag != 35)goto _LL18A;}_LL189: goto
_LL18B;_LL18A: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp263=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp238;if(_tmp263->tag != 36)goto _LL18C;}_LL18B: return 140;_LL18C: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp264=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp238;if(_tmp264->tag != 37)goto
_LL13B;}_LL18D: return 10000;_LL13B:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct
Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp265=e->r;union Cyc_Absyn_Cnst _tmp267;struct _tuple0*_tmp269;struct
_tuple0*_tmp26B;enum Cyc_Absyn_Primop _tmp26D;struct Cyc_List_List*_tmp26E;struct
Cyc_Absyn_Exp*_tmp270;struct Cyc_Core_Opt*_tmp271;struct Cyc_Absyn_Exp*_tmp272;
struct Cyc_Absyn_Exp*_tmp274;enum Cyc_Absyn_Incrementor _tmp275;struct Cyc_Absyn_Exp*
_tmp277;struct Cyc_Absyn_Exp*_tmp278;struct Cyc_Absyn_Exp*_tmp279;struct Cyc_Absyn_Exp*
_tmp27B;struct Cyc_Absyn_Exp*_tmp27C;struct Cyc_Absyn_Exp*_tmp27E;struct Cyc_Absyn_Exp*
_tmp27F;struct Cyc_Absyn_Exp*_tmp281;struct Cyc_Absyn_Exp*_tmp282;struct Cyc_Absyn_Exp*
_tmp284;struct Cyc_List_List*_tmp285;struct Cyc_Absyn_Exp*_tmp287;struct Cyc_List_List*
_tmp288;struct Cyc_Absyn_Exp*_tmp28A;struct Cyc_Absyn_Exp*_tmp28C;struct Cyc_Absyn_Exp*
_tmp28E;void*_tmp290;struct Cyc_Absyn_Exp*_tmp291;struct Cyc_Absyn_Exp*_tmp293;
struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Exp*_tmp296;void*_tmp298;struct Cyc_Absyn_Exp*
_tmp29A;void*_tmp29C;int _tmp29E;struct _dyneither_ptr _tmp29F;struct Cyc_Absyn_Exp*
_tmp2A1;struct _dyneither_ptr*_tmp2A2;void*_tmp2A4;void*_tmp2A5;struct
_dyneither_ptr*_tmp2A7;void*_tmp2A9;void*_tmp2AA;unsigned int _tmp2AC;struct Cyc_Absyn_Exp*
_tmp2AE;struct Cyc_Absyn_Exp*_tmp2B0;struct _dyneither_ptr*_tmp2B1;struct Cyc_Absyn_Exp*
_tmp2B3;struct _dyneither_ptr*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B6;struct Cyc_Absyn_Exp*
_tmp2B7;struct Cyc_List_List*_tmp2B9;struct _tuple8*_tmp2BB;struct Cyc_List_List*
_tmp2BC;struct Cyc_List_List*_tmp2BE;struct Cyc_Absyn_Vardecl*_tmp2C0;struct Cyc_Absyn_Exp*
_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C2;struct _tuple0*_tmp2C4;struct Cyc_List_List*
_tmp2C5;struct Cyc_List_List*_tmp2C6;struct Cyc_List_List*_tmp2C8;struct Cyc_List_List*
_tmp2CA;struct Cyc_Absyn_Datatypefield*_tmp2CB;struct _tuple0*_tmp2CD;struct
_tuple0*_tmp2CF;struct Cyc_Absyn_MallocInfo _tmp2D1;int _tmp2D2;struct Cyc_Absyn_Exp*
_tmp2D3;void**_tmp2D4;struct Cyc_Absyn_Exp*_tmp2D5;struct Cyc_Absyn_Exp*_tmp2D7;
struct Cyc_Absyn_Exp*_tmp2D8;struct Cyc_Core_Opt*_tmp2DA;struct Cyc_List_List*
_tmp2DB;struct Cyc_Absyn_Stmt*_tmp2DD;_LL1A4: {struct Cyc_Absyn_Const_e_struct*
_tmp266=(struct Cyc_Absyn_Const_e_struct*)_tmp265;if(_tmp266->tag != 0)goto _LL1A6;
else{_tmp267=_tmp266->f1;}}_LL1A5: s=Cyc_Absynpp_cnst2doc(_tmp267);goto _LL1A3;
_LL1A6: {struct Cyc_Absyn_Var_e_struct*_tmp268=(struct Cyc_Absyn_Var_e_struct*)
_tmp265;if(_tmp268->tag != 1)goto _LL1A8;else{_tmp269=_tmp268->f1;}}_LL1A7: _tmp26B=
_tmp269;goto _LL1A9;_LL1A8: {struct Cyc_Absyn_UnknownId_e_struct*_tmp26A=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp265;if(_tmp26A->tag != 2)goto _LL1AA;else{
_tmp26B=_tmp26A->f1;}}_LL1A9: s=Cyc_Absynpp_qvar2doc(_tmp26B);goto _LL1A3;_LL1AA: {
struct Cyc_Absyn_Primop_e_struct*_tmp26C=(struct Cyc_Absyn_Primop_e_struct*)
_tmp265;if(_tmp26C->tag != 3)goto _LL1AC;else{_tmp26D=_tmp26C->f1;_tmp26E=_tmp26C->f2;}}
_LL1AB: s=Cyc_Absynpp_primapp2doc(myprec,_tmp26D,_tmp26E);goto _LL1A3;_LL1AC: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp26F=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp265;if(_tmp26F->tag != 4)goto _LL1AE;else{_tmp270=_tmp26F->f1;_tmp271=_tmp26F->f2;
_tmp272=_tmp26F->f3;}}_LL1AD:{const char*_tmp7B1;const char*_tmp7B0;struct Cyc_PP_Doc*
_tmp7AF[5];s=((_tmp7AF[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp272),((_tmp7AF[3]=
Cyc_PP_text(((_tmp7B0="= ",_tag_dyneither(_tmp7B0,sizeof(char),3)))),((_tmp7AF[2]=
_tmp271 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp271->v),((
_tmp7AF[1]=Cyc_PP_text(((_tmp7B1=" ",_tag_dyneither(_tmp7B1,sizeof(char),2)))),((
_tmp7AF[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp270),Cyc_PP_cat(_tag_dyneither(
_tmp7AF,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1A3;_LL1AE: {struct Cyc_Absyn_Increment_e_struct*
_tmp273=(struct Cyc_Absyn_Increment_e_struct*)_tmp265;if(_tmp273->tag != 5)goto
_LL1B0;else{_tmp274=_tmp273->f1;_tmp275=_tmp273->f2;}}_LL1AF: {struct Cyc_PP_Doc*
_tmp2E1=Cyc_Absynpp_exp2doc_prec(myprec,_tmp274);switch(_tmp275){case Cyc_Absyn_PreInc:
_LL1F8:{const char*_tmp7B4;struct Cyc_PP_Doc*_tmp7B3[2];s=((_tmp7B3[1]=_tmp2E1,((
_tmp7B3[0]=Cyc_PP_text(((_tmp7B4="++",_tag_dyneither(_tmp7B4,sizeof(char),3)))),
Cyc_PP_cat(_tag_dyneither(_tmp7B3,sizeof(struct Cyc_PP_Doc*),2))))));}break;case
Cyc_Absyn_PreDec: _LL1F9:{const char*_tmp7B7;struct Cyc_PP_Doc*_tmp7B6[2];s=((
_tmp7B6[1]=_tmp2E1,((_tmp7B6[0]=Cyc_PP_text(((_tmp7B7="--",_tag_dyneither(
_tmp7B7,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7B6,sizeof(struct Cyc_PP_Doc*),
2))))));}break;case Cyc_Absyn_PostInc: _LL1FA:{const char*_tmp7BA;struct Cyc_PP_Doc*
_tmp7B9[2];s=((_tmp7B9[1]=Cyc_PP_text(((_tmp7BA="++",_tag_dyneither(_tmp7BA,
sizeof(char),3)))),((_tmp7B9[0]=_tmp2E1,Cyc_PP_cat(_tag_dyneither(_tmp7B9,
sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1FB:{const char*
_tmp7BD;struct Cyc_PP_Doc*_tmp7BC[2];s=((_tmp7BC[1]=Cyc_PP_text(((_tmp7BD="--",
_tag_dyneither(_tmp7BD,sizeof(char),3)))),((_tmp7BC[0]=_tmp2E1,Cyc_PP_cat(
_tag_dyneither(_tmp7BC,sizeof(struct Cyc_PP_Doc*),2))))));}break;}goto _LL1A3;}
_LL1B0: {struct Cyc_Absyn_Conditional_e_struct*_tmp276=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp265;if(_tmp276->tag != 6)goto _LL1B2;else{_tmp277=_tmp276->f1;_tmp278=_tmp276->f2;
_tmp279=_tmp276->f3;}}_LL1B1:{const char*_tmp7C2;const char*_tmp7C1;struct Cyc_PP_Doc*
_tmp7C0[5];s=((_tmp7C0[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp279),((_tmp7C0[3]=
Cyc_PP_text(((_tmp7C1=" : ",_tag_dyneither(_tmp7C1,sizeof(char),4)))),((_tmp7C0[
2]=Cyc_Absynpp_exp2doc_prec(0,_tmp278),((_tmp7C0[1]=Cyc_PP_text(((_tmp7C2=" ? ",
_tag_dyneither(_tmp7C2,sizeof(char),4)))),((_tmp7C0[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp277),Cyc_PP_cat(_tag_dyneither(_tmp7C0,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A3;_LL1B2: {struct Cyc_Absyn_And_e_struct*_tmp27A=(struct Cyc_Absyn_And_e_struct*)
_tmp265;if(_tmp27A->tag != 7)goto _LL1B4;else{_tmp27B=_tmp27A->f1;_tmp27C=_tmp27A->f2;}}
_LL1B3:{const char*_tmp7C5;struct Cyc_PP_Doc*_tmp7C4[3];s=((_tmp7C4[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp27C),((_tmp7C4[1]=Cyc_PP_text(((_tmp7C5=" && ",_tag_dyneither(_tmp7C5,
sizeof(char),5)))),((_tmp7C4[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27B),Cyc_PP_cat(
_tag_dyneither(_tmp7C4,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1B4: {
struct Cyc_Absyn_Or_e_struct*_tmp27D=(struct Cyc_Absyn_Or_e_struct*)_tmp265;if(
_tmp27D->tag != 8)goto _LL1B6;else{_tmp27E=_tmp27D->f1;_tmp27F=_tmp27D->f2;}}
_LL1B5:{const char*_tmp7C8;struct Cyc_PP_Doc*_tmp7C7[3];s=((_tmp7C7[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp27F),((_tmp7C7[1]=Cyc_PP_text(((_tmp7C8=" || ",_tag_dyneither(_tmp7C8,
sizeof(char),5)))),((_tmp7C7[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27E),Cyc_PP_cat(
_tag_dyneither(_tmp7C7,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1B6: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp280=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp265;if(_tmp280->tag != 9)goto _LL1B8;else{_tmp281=_tmp280->f1;_tmp282=_tmp280->f2;}}
_LL1B7:{const char*_tmp7CF;const char*_tmp7CE;const char*_tmp7CD;struct Cyc_PP_Doc*
_tmp7CC[5];s=((_tmp7CC[4]=Cyc_PP_text(((_tmp7CD=")",_tag_dyneither(_tmp7CD,
sizeof(char),2)))),((_tmp7CC[3]=Cyc_Absynpp_exp2doc(_tmp282),((_tmp7CC[2]=Cyc_PP_text(((
_tmp7CE=", ",_tag_dyneither(_tmp7CE,sizeof(char),3)))),((_tmp7CC[1]=Cyc_Absynpp_exp2doc(
_tmp281),((_tmp7CC[0]=Cyc_PP_text(((_tmp7CF="(",_tag_dyneither(_tmp7CF,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7CC,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A3;_LL1B8: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp283=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp265;if(_tmp283->tag != 10)goto _LL1BA;else{_tmp284=_tmp283->f1;_tmp285=_tmp283->f2;}}
_LL1B9:{const char*_tmp7D4;const char*_tmp7D3;struct Cyc_PP_Doc*_tmp7D2[4];s=((
_tmp7D2[3]=Cyc_PP_text(((_tmp7D3=")",_tag_dyneither(_tmp7D3,sizeof(char),2)))),((
_tmp7D2[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp285),((_tmp7D2[1]=Cyc_PP_text(((
_tmp7D4="(",_tag_dyneither(_tmp7D4,sizeof(char),2)))),((_tmp7D2[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp284),Cyc_PP_cat(_tag_dyneither(_tmp7D2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1A3;_LL1BA: {struct Cyc_Absyn_FnCall_e_struct*_tmp286=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp265;if(_tmp286->tag != 11)goto _LL1BC;else{_tmp287=_tmp286->f1;_tmp288=_tmp286->f2;}}
_LL1BB:{const char*_tmp7D9;const char*_tmp7D8;struct Cyc_PP_Doc*_tmp7D7[4];s=((
_tmp7D7[3]=Cyc_PP_text(((_tmp7D8=")",_tag_dyneither(_tmp7D8,sizeof(char),2)))),((
_tmp7D7[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp288),((_tmp7D7[1]=Cyc_PP_text(((
_tmp7D9="(",_tag_dyneither(_tmp7D9,sizeof(char),2)))),((_tmp7D7[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp287),Cyc_PP_cat(_tag_dyneither(_tmp7D7,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1A3;_LL1BC: {struct Cyc_Absyn_Throw_e_struct*_tmp289=(struct Cyc_Absyn_Throw_e_struct*)
_tmp265;if(_tmp289->tag != 12)goto _LL1BE;else{_tmp28A=_tmp289->f1;}}_LL1BD:{const
char*_tmp7DC;struct Cyc_PP_Doc*_tmp7DB[2];s=((_tmp7DB[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp28A),((_tmp7DB[0]=Cyc_PP_text(((_tmp7DC="throw ",_tag_dyneither(
_tmp7DC,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp7DB,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL1A3;_LL1BE: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp28B=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp265;if(_tmp28B->tag != 13)goto _LL1C0;
else{_tmp28C=_tmp28B->f1;}}_LL1BF: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp28C);
goto _LL1A3;_LL1C0: {struct Cyc_Absyn_Instantiate_e_struct*_tmp28D=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp265;if(_tmp28D->tag != 14)goto _LL1C2;else{_tmp28E=_tmp28D->f1;}}_LL1C1: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmp28E);goto _LL1A3;_LL1C2: {struct Cyc_Absyn_Cast_e_struct*_tmp28F=(
struct Cyc_Absyn_Cast_e_struct*)_tmp265;if(_tmp28F->tag != 15)goto _LL1C4;else{
_tmp290=(void*)_tmp28F->f1;_tmp291=_tmp28F->f2;}}_LL1C3:{const char*_tmp7E1;const
char*_tmp7E0;struct Cyc_PP_Doc*_tmp7DF[4];s=((_tmp7DF[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp291),((_tmp7DF[2]=Cyc_PP_text(((_tmp7E0=")",_tag_dyneither(_tmp7E0,
sizeof(char),2)))),((_tmp7DF[1]=Cyc_Absynpp_typ2doc(_tmp290),((_tmp7DF[0]=Cyc_PP_text(((
_tmp7E1="(",_tag_dyneither(_tmp7E1,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp7DF,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1A3;_LL1C4: {struct Cyc_Absyn_Address_e_struct*
_tmp292=(struct Cyc_Absyn_Address_e_struct*)_tmp265;if(_tmp292->tag != 16)goto
_LL1C6;else{_tmp293=_tmp292->f1;}}_LL1C5:{const char*_tmp7E4;struct Cyc_PP_Doc*
_tmp7E3[2];s=((_tmp7E3[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp293),((_tmp7E3[0]=
Cyc_PP_text(((_tmp7E4="&",_tag_dyneither(_tmp7E4,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp7E3,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1A3;_LL1C6: {
struct Cyc_Absyn_New_e_struct*_tmp294=(struct Cyc_Absyn_New_e_struct*)_tmp265;if(
_tmp294->tag != 17)goto _LL1C8;else{_tmp295=_tmp294->f1;_tmp296=_tmp294->f2;}}
_LL1C7: if(_tmp295 == 0){const char*_tmp7E7;struct Cyc_PP_Doc*_tmp7E6[2];s=((_tmp7E6[
1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp296),((_tmp7E6[0]=Cyc_PP_text(((_tmp7E7="new ",
_tag_dyneither(_tmp7E7,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7E6,
sizeof(struct Cyc_PP_Doc*),2))))));}else{const char*_tmp7EC;const char*_tmp7EB;
struct Cyc_PP_Doc*_tmp7EA[4];s=((_tmp7EA[3]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp296),((_tmp7EA[2]=Cyc_PP_text(((_tmp7EB=") ",_tag_dyneither(_tmp7EB,sizeof(
char),3)))),((_tmp7EA[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp295),((
_tmp7EA[0]=Cyc_PP_text(((_tmp7EC="rnew(",_tag_dyneither(_tmp7EC,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp7EA,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL1A3;_LL1C8: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp297=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp265;if(_tmp297->tag != 18)goto _LL1CA;else{_tmp298=(void*)_tmp297->f1;}}_LL1C9:{
const char*_tmp7F1;const char*_tmp7F0;struct Cyc_PP_Doc*_tmp7EF[3];s=((_tmp7EF[2]=
Cyc_PP_text(((_tmp7F0=")",_tag_dyneither(_tmp7F0,sizeof(char),2)))),((_tmp7EF[1]=
Cyc_Absynpp_typ2doc(_tmp298),((_tmp7EF[0]=Cyc_PP_text(((_tmp7F1="sizeof(",
_tag_dyneither(_tmp7F1,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7EF,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1CA: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp299=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp265;if(_tmp299->tag != 19)goto
_LL1CC;else{_tmp29A=_tmp299->f1;}}_LL1CB:{const char*_tmp7F6;const char*_tmp7F5;
struct Cyc_PP_Doc*_tmp7F4[3];s=((_tmp7F4[2]=Cyc_PP_text(((_tmp7F5=")",
_tag_dyneither(_tmp7F5,sizeof(char),2)))),((_tmp7F4[1]=Cyc_Absynpp_exp2doc(
_tmp29A),((_tmp7F4[0]=Cyc_PP_text(((_tmp7F6="sizeof(",_tag_dyneither(_tmp7F6,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7F4,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A3;_LL1CC: {struct Cyc_Absyn_Valueof_e_struct*_tmp29B=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp265;if(_tmp29B->tag != 39)goto _LL1CE;else{_tmp29C=(void*)_tmp29B->f1;}}_LL1CD:{
const char*_tmp7FB;const char*_tmp7FA;struct Cyc_PP_Doc*_tmp7F9[3];s=((_tmp7F9[2]=
Cyc_PP_text(((_tmp7FA=")",_tag_dyneither(_tmp7FA,sizeof(char),2)))),((_tmp7F9[1]=
Cyc_Absynpp_typ2doc(_tmp29C),((_tmp7F9[0]=Cyc_PP_text(((_tmp7FB="valueof(",
_tag_dyneither(_tmp7FB,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7F9,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1CE: {struct Cyc_Absyn_Asm_e_struct*
_tmp29D=(struct Cyc_Absyn_Asm_e_struct*)_tmp265;if(_tmp29D->tag != 40)goto _LL1D0;
else{_tmp29E=_tmp29D->f1;_tmp29F=_tmp29D->f2;}}_LL1CF: if(_tmp29E){const char*
_tmp802;const char*_tmp801;const char*_tmp800;struct Cyc_PP_Doc*_tmp7FF[4];s=((
_tmp7FF[3]=Cyc_PP_text(((_tmp800=")",_tag_dyneither(_tmp800,sizeof(char),2)))),((
_tmp7FF[2]=Cyc_PP_text(_tmp29F),((_tmp7FF[1]=Cyc_PP_text(((_tmp801=" volatile (",
_tag_dyneither(_tmp801,sizeof(char),12)))),((_tmp7FF[0]=Cyc_PP_text(((_tmp802="__asm__",
_tag_dyneither(_tmp802,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7FF,
sizeof(struct Cyc_PP_Doc*),4))))))))));}else{const char*_tmp807;const char*_tmp806;
struct Cyc_PP_Doc*_tmp805[3];s=((_tmp805[2]=Cyc_PP_text(((_tmp806=")",
_tag_dyneither(_tmp806,sizeof(char),2)))),((_tmp805[1]=Cyc_PP_text(_tmp29F),((
_tmp805[0]=Cyc_PP_text(((_tmp807="__asm__(",_tag_dyneither(_tmp807,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp805,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A3;_LL1D0: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp2A0=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp265;if(_tmp2A0->tag != 38)goto _LL1D2;else{_tmp2A1=_tmp2A0->f1;_tmp2A2=_tmp2A0->f2;}}
_LL1D1:{const char*_tmp80E;const char*_tmp80D;const char*_tmp80C;struct Cyc_PP_Doc*
_tmp80B[5];s=((_tmp80B[4]=Cyc_PP_text(((_tmp80C=")",_tag_dyneither(_tmp80C,
sizeof(char),2)))),((_tmp80B[3]=Cyc_PP_textptr(_tmp2A2),((_tmp80B[2]=Cyc_PP_text(((
_tmp80D=".",_tag_dyneither(_tmp80D,sizeof(char),2)))),((_tmp80B[1]=Cyc_Absynpp_exp2doc(
_tmp2A1),((_tmp80B[0]=Cyc_PP_text(((_tmp80E="tagcheck(",_tag_dyneither(_tmp80E,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp80B,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL1A3;_LL1D2: {struct Cyc_Absyn_Offsetof_e_struct*_tmp2A3=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp265;if(_tmp2A3->tag != 20)goto _LL1D4;else{
_tmp2A4=(void*)_tmp2A3->f1;_tmp2A5=(void*)_tmp2A3->f2;{struct Cyc_Absyn_StructField_struct*
_tmp2A6=(struct Cyc_Absyn_StructField_struct*)_tmp2A5;if(_tmp2A6->tag != 0)goto
_LL1D4;else{_tmp2A7=_tmp2A6->f1;}};}}_LL1D3:{const char*_tmp815;const char*_tmp814;
const char*_tmp813;struct Cyc_PP_Doc*_tmp812[5];s=((_tmp812[4]=Cyc_PP_text(((
_tmp813=")",_tag_dyneither(_tmp813,sizeof(char),2)))),((_tmp812[3]=Cyc_PP_textptr(
_tmp2A7),((_tmp812[2]=Cyc_PP_text(((_tmp814=",",_tag_dyneither(_tmp814,sizeof(
char),2)))),((_tmp812[1]=Cyc_Absynpp_typ2doc(_tmp2A4),((_tmp812[0]=Cyc_PP_text(((
_tmp815="offsetof(",_tag_dyneither(_tmp815,sizeof(char),10)))),Cyc_PP_cat(
_tag_dyneither(_tmp812,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1A3;
_LL1D4: {struct Cyc_Absyn_Offsetof_e_struct*_tmp2A8=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp265;if(_tmp2A8->tag != 20)goto _LL1D6;else{_tmp2A9=(void*)_tmp2A8->f1;_tmp2AA=(
void*)_tmp2A8->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp2AB=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2AA;if(_tmp2AB->tag != 1)goto _LL1D6;else{_tmp2AC=_tmp2AB->f1;}};}}_LL1D5:{
const char*_tmp823;const char*_tmp822;struct Cyc_Int_pa_struct _tmp821;void*_tmp820[
1];const char*_tmp81F;const char*_tmp81E;struct Cyc_PP_Doc*_tmp81D[5];s=((_tmp81D[4]=
Cyc_PP_text(((_tmp81E=")",_tag_dyneither(_tmp81E,sizeof(char),2)))),((_tmp81D[3]=
Cyc_PP_text((struct _dyneither_ptr)((_tmp821.tag=1,((_tmp821.f1=(unsigned long)((
int)_tmp2AC),((_tmp820[0]=& _tmp821,Cyc_aprintf(((_tmp81F="%d",_tag_dyneither(
_tmp81F,sizeof(char),3))),_tag_dyneither(_tmp820,sizeof(void*),1))))))))),((
_tmp81D[2]=Cyc_PP_text(((_tmp822=",",_tag_dyneither(_tmp822,sizeof(char),2)))),((
_tmp81D[1]=Cyc_Absynpp_typ2doc(_tmp2A9),((_tmp81D[0]=Cyc_PP_text(((_tmp823="offsetof(",
_tag_dyneither(_tmp823,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp81D,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1A3;_LL1D6: {struct Cyc_Absyn_Deref_e_struct*
_tmp2AD=(struct Cyc_Absyn_Deref_e_struct*)_tmp265;if(_tmp2AD->tag != 21)goto _LL1D8;
else{_tmp2AE=_tmp2AD->f1;}}_LL1D7:{const char*_tmp826;struct Cyc_PP_Doc*_tmp825[2];
s=((_tmp825[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AE),((_tmp825[0]=Cyc_PP_text(((
_tmp826="*",_tag_dyneither(_tmp826,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp825,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1A3;_LL1D8: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp2AF=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp265;if(_tmp2AF->tag != 22)goto
_LL1DA;else{_tmp2B0=_tmp2AF->f1;_tmp2B1=_tmp2AF->f2;}}_LL1D9:{const char*_tmp829;
struct Cyc_PP_Doc*_tmp828[3];s=((_tmp828[2]=Cyc_PP_textptr(_tmp2B1),((_tmp828[1]=
Cyc_PP_text(((_tmp829=".",_tag_dyneither(_tmp829,sizeof(char),2)))),((_tmp828[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B0),Cyc_PP_cat(_tag_dyneither(_tmp828,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1DA: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp2B2=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp265;if(_tmp2B2->tag != 23)goto
_LL1DC;else{_tmp2B3=_tmp2B2->f1;_tmp2B4=_tmp2B2->f2;}}_LL1DB:{const char*_tmp82C;
struct Cyc_PP_Doc*_tmp82B[3];s=((_tmp82B[2]=Cyc_PP_textptr(_tmp2B4),((_tmp82B[1]=
Cyc_PP_text(((_tmp82C="->",_tag_dyneither(_tmp82C,sizeof(char),3)))),((_tmp82B[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B3),Cyc_PP_cat(_tag_dyneither(_tmp82B,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1DC: {struct Cyc_Absyn_Subscript_e_struct*
_tmp2B5=(struct Cyc_Absyn_Subscript_e_struct*)_tmp265;if(_tmp2B5->tag != 24)goto
_LL1DE;else{_tmp2B6=_tmp2B5->f1;_tmp2B7=_tmp2B5->f2;}}_LL1DD:{const char*_tmp831;
const char*_tmp830;struct Cyc_PP_Doc*_tmp82F[4];s=((_tmp82F[3]=Cyc_PP_text(((
_tmp830="]",_tag_dyneither(_tmp830,sizeof(char),2)))),((_tmp82F[2]=Cyc_Absynpp_exp2doc(
_tmp2B7),((_tmp82F[1]=Cyc_PP_text(((_tmp831="[",_tag_dyneither(_tmp831,sizeof(
char),2)))),((_tmp82F[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B6),Cyc_PP_cat(
_tag_dyneither(_tmp82F,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1A3;_LL1DE: {
struct Cyc_Absyn_Tuple_e_struct*_tmp2B8=(struct Cyc_Absyn_Tuple_e_struct*)_tmp265;
if(_tmp2B8->tag != 25)goto _LL1E0;else{_tmp2B9=_tmp2B8->f1;}}_LL1DF:{const char*
_tmp836;const char*_tmp835;struct Cyc_PP_Doc*_tmp834[4];s=((_tmp834[3]=Cyc_PP_text(((
_tmp835=")",_tag_dyneither(_tmp835,sizeof(char),2)))),((_tmp834[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp2B9),((_tmp834[1]=Cyc_PP_text(((_tmp836="(",_tag_dyneither(_tmp836,
sizeof(char),2)))),((_tmp834[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(
_tmp834,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1A3;_LL1E0: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp2BA=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp265;if(_tmp2BA->tag != 26)goto
_LL1E2;else{_tmp2BB=_tmp2BA->f1;_tmp2BC=_tmp2BA->f2;}}_LL1E1:{const char*_tmp83D;
const char*_tmp83C;const char*_tmp83B;struct Cyc_PP_Doc*_tmp83A[4];s=((_tmp83A[3]=
Cyc_Absynpp_group_braces(((_tmp83B=",",_tag_dyneither(_tmp83B,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BC)),((_tmp83A[2]=Cyc_PP_text(((_tmp83C=")",
_tag_dyneither(_tmp83C,sizeof(char),2)))),((_tmp83A[1]=Cyc_Absynpp_typ2doc((*
_tmp2BB).f3),((_tmp83A[0]=Cyc_PP_text(((_tmp83D="(",_tag_dyneither(_tmp83D,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp83A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1A3;_LL1E2: {struct Cyc_Absyn_Array_e_struct*_tmp2BD=(struct Cyc_Absyn_Array_e_struct*)
_tmp265;if(_tmp2BD->tag != 27)goto _LL1E4;else{_tmp2BE=_tmp2BD->f1;}}_LL1E3:{const
char*_tmp83E;s=Cyc_Absynpp_group_braces(((_tmp83E=",",_tag_dyneither(_tmp83E,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BE));}goto _LL1A3;
_LL1E4: {struct Cyc_Absyn_Comprehension_e_struct*_tmp2BF=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp265;if(_tmp2BF->tag != 28)goto _LL1E6;else{_tmp2C0=_tmp2BF->f1;_tmp2C1=_tmp2BF->f2;
_tmp2C2=_tmp2BF->f3;}}_LL1E5:{const char*_tmp845;const char*_tmp844;const char*
_tmp843;struct Cyc_PP_Doc*_tmp842[8];s=((_tmp842[7]=Cyc_Absynpp_rb(),((_tmp842[6]=
Cyc_Absynpp_exp2doc(_tmp2C2),((_tmp842[5]=Cyc_PP_text(((_tmp843=" : ",
_tag_dyneither(_tmp843,sizeof(char),4)))),((_tmp842[4]=Cyc_Absynpp_exp2doc(
_tmp2C1),((_tmp842[3]=Cyc_PP_text(((_tmp844=" < ",_tag_dyneither(_tmp844,sizeof(
char),4)))),((_tmp842[2]=Cyc_PP_text(*(*_tmp2C0->name).f2),((_tmp842[1]=Cyc_PP_text(((
_tmp845="for ",_tag_dyneither(_tmp845,sizeof(char),5)))),((_tmp842[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp842,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL1A3;_LL1E6: {struct Cyc_Absyn_Aggregate_e_struct*_tmp2C3=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp265;if(_tmp2C3->tag != 29)goto _LL1E8;else{_tmp2C4=_tmp2C3->f1;_tmp2C5=_tmp2C3->f2;
_tmp2C6=_tmp2C3->f3;}}_LL1E7: {struct Cyc_List_List*_tmp33B=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2C6);{const char*_tmp84A;struct Cyc_List_List*_tmp849;struct Cyc_PP_Doc*_tmp848[
2];s=((_tmp848[1]=Cyc_Absynpp_group_braces(((_tmp84A=",",_tag_dyneither(_tmp84A,
sizeof(char),2))),_tmp2C5 != 0?(_tmp849=_cycalloc(sizeof(*_tmp849)),((_tmp849->hd=
Cyc_Absynpp_tps2doc(_tmp2C5),((_tmp849->tl=_tmp33B,_tmp849))))): _tmp33B),((
_tmp848[0]=Cyc_Absynpp_qvar2doc(_tmp2C4),Cyc_PP_cat(_tag_dyneither(_tmp848,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1A3;}_LL1E8: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp2C7=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp265;if(_tmp2C7->tag != 30)goto
_LL1EA;else{_tmp2C8=_tmp2C7->f2;}}_LL1E9:{const char*_tmp84B;s=Cyc_Absynpp_group_braces(((
_tmp84B=",",_tag_dyneither(_tmp84B,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2C8));}goto _LL1A3;_LL1EA: {struct Cyc_Absyn_Datatype_e_struct*_tmp2C9=(struct
Cyc_Absyn_Datatype_e_struct*)_tmp265;if(_tmp2C9->tag != 31)goto _LL1EC;else{
_tmp2CA=_tmp2C9->f1;_tmp2CB=_tmp2C9->f3;}}_LL1EB: if(_tmp2CA == 0)s=Cyc_Absynpp_qvar2doc(
_tmp2CB->name);else{const char*_tmp852;const char*_tmp851;const char*_tmp850;struct
Cyc_PP_Doc*_tmp84F[2];s=((_tmp84F[1]=Cyc_PP_egroup(((_tmp852="(",_tag_dyneither(
_tmp852,sizeof(char),2))),((_tmp851=")",_tag_dyneither(_tmp851,sizeof(char),2))),((
_tmp850=",",_tag_dyneither(_tmp850,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_exp2doc,_tmp2CA)),((_tmp84F[0]=Cyc_Absynpp_qvar2doc(_tmp2CB->name),
Cyc_PP_cat(_tag_dyneither(_tmp84F,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1A3;
_LL1EC: {struct Cyc_Absyn_Enum_e_struct*_tmp2CC=(struct Cyc_Absyn_Enum_e_struct*)
_tmp265;if(_tmp2CC->tag != 32)goto _LL1EE;else{_tmp2CD=_tmp2CC->f1;}}_LL1ED: s=Cyc_Absynpp_qvar2doc(
_tmp2CD);goto _LL1A3;_LL1EE: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp2CE=(struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp265;if(_tmp2CE->tag != 33)goto _LL1F0;else{
_tmp2CF=_tmp2CE->f1;}}_LL1EF: s=Cyc_Absynpp_qvar2doc(_tmp2CF);goto _LL1A3;_LL1F0: {
struct Cyc_Absyn_Malloc_e_struct*_tmp2D0=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp265;if(_tmp2D0->tag != 34)goto _LL1F2;else{_tmp2D1=_tmp2D0->f1;_tmp2D2=_tmp2D1.is_calloc;
_tmp2D3=_tmp2D1.rgn;_tmp2D4=_tmp2D1.elt_type;_tmp2D5=_tmp2D1.num_elts;}}_LL1F1:
if(_tmp2D2){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp2D4)),0);if(_tmp2D3 == 0){const char*_tmp859;const char*_tmp858;const char*
_tmp857;struct Cyc_PP_Doc*_tmp856[5];s=((_tmp856[4]=Cyc_PP_text(((_tmp857=")",
_tag_dyneither(_tmp857,sizeof(char),2)))),((_tmp856[3]=Cyc_Absynpp_exp2doc(st),((
_tmp856[2]=Cyc_PP_text(((_tmp858=",",_tag_dyneither(_tmp858,sizeof(char),2)))),((
_tmp856[1]=Cyc_Absynpp_exp2doc(_tmp2D5),((_tmp856[0]=Cyc_PP_text(((_tmp859="calloc(",
_tag_dyneither(_tmp859,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp856,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp862;const char*
_tmp861;const char*_tmp860;const char*_tmp85F;struct Cyc_PP_Doc*_tmp85E[7];s=((
_tmp85E[6]=Cyc_PP_text(((_tmp85F=")",_tag_dyneither(_tmp85F,sizeof(char),2)))),((
_tmp85E[5]=Cyc_Absynpp_exp2doc(st),((_tmp85E[4]=Cyc_PP_text(((_tmp860=",",
_tag_dyneither(_tmp860,sizeof(char),2)))),((_tmp85E[3]=Cyc_Absynpp_exp2doc(
_tmp2D5),((_tmp85E[2]=Cyc_PP_text(((_tmp861=",",_tag_dyneither(_tmp861,sizeof(
char),2)))),((_tmp85E[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2D3),((
_tmp85E[0]=Cyc_PP_text(((_tmp862="rcalloc(",_tag_dyneither(_tmp862,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp85E,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp2D4 == 0)new_e=_tmp2D5;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp2D4,0),_tmp2D5,0);}if(_tmp2D3 == 0){const char*
_tmp867;const char*_tmp866;struct Cyc_PP_Doc*_tmp865[3];s=((_tmp865[2]=Cyc_PP_text(((
_tmp866=")",_tag_dyneither(_tmp866,sizeof(char),2)))),((_tmp865[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp865[0]=Cyc_PP_text(((_tmp867="malloc(",_tag_dyneither(_tmp867,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp865,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp86E;const char*_tmp86D;const char*_tmp86C;struct Cyc_PP_Doc*
_tmp86B[5];s=((_tmp86B[4]=Cyc_PP_text(((_tmp86C=")",_tag_dyneither(_tmp86C,
sizeof(char),2)))),((_tmp86B[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp86B[2]=Cyc_PP_text(((
_tmp86D=",",_tag_dyneither(_tmp86D,sizeof(char),2)))),((_tmp86B[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2D3),((_tmp86B[0]=Cyc_PP_text(((_tmp86E="rmalloc(",
_tag_dyneither(_tmp86E,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp86B,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL1A3;_LL1F2: {struct Cyc_Absyn_Swap_e_struct*
_tmp2D6=(struct Cyc_Absyn_Swap_e_struct*)_tmp265;if(_tmp2D6->tag != 35)goto _LL1F4;
else{_tmp2D7=_tmp2D6->f1;_tmp2D8=_tmp2D6->f2;}}_LL1F3:{const char*_tmp871;struct
Cyc_PP_Doc*_tmp870[3];s=((_tmp870[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D8),((
_tmp870[1]=Cyc_PP_text(((_tmp871=" :=: ",_tag_dyneither(_tmp871,sizeof(char),6)))),((
_tmp870[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D7),Cyc_PP_cat(_tag_dyneither(
_tmp870,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1F4: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp2D9=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp265;if(_tmp2D9->tag != 36)
goto _LL1F6;else{_tmp2DA=_tmp2D9->f1;_tmp2DB=_tmp2D9->f2;}}_LL1F5:{const char*
_tmp872;s=Cyc_Absynpp_group_braces(((_tmp872=",",_tag_dyneither(_tmp872,sizeof(
char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2DB));}goto _LL1A3;
_LL1F6: {struct Cyc_Absyn_StmtExp_e_struct*_tmp2DC=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp265;if(_tmp2DC->tag != 37)goto _LL1A3;else{_tmp2DD=_tmp2DC->f1;}}_LL1F7:{const
char*_tmp877;const char*_tmp876;struct Cyc_PP_Doc*_tmp875[7];s=((_tmp875[6]=Cyc_PP_text(((
_tmp876=")",_tag_dyneither(_tmp876,sizeof(char),2)))),((_tmp875[5]=Cyc_Absynpp_rb(),((
_tmp875[4]=Cyc_PP_blank_doc(),((_tmp875[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2DD,1)),((_tmp875[2]=Cyc_PP_blank_doc(),((_tmp875[1]=Cyc_Absynpp_lb(),((
_tmp875[0]=Cyc_PP_text(((_tmp877="(",_tag_dyneither(_tmp877,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp875,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
goto _LL1A3;_LL1A3:;}if(inprec >= myprec){const char*_tmp87C;const char*_tmp87B;
struct Cyc_PP_Doc*_tmp87A[3];s=((_tmp87A[2]=Cyc_PP_text(((_tmp87B=")",
_tag_dyneither(_tmp87B,sizeof(char),2)))),((_tmp87A[1]=s,((_tmp87A[0]=Cyc_PP_text(((
_tmp87C="(",_tag_dyneither(_tmp87C,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp87A,sizeof(struct Cyc_PP_Doc*),3))))))));}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d){void*_tmp35D=d;struct Cyc_Absyn_Exp*_tmp35F;struct _dyneither_ptr*_tmp361;
_LL1FE: {struct Cyc_Absyn_ArrayElement_struct*_tmp35E=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp35D;if(_tmp35E->tag != 0)goto _LL200;else{_tmp35F=_tmp35E->f1;}}_LL1FF: {const
char*_tmp881;const char*_tmp880;struct Cyc_PP_Doc*_tmp87F[3];return(_tmp87F[2]=Cyc_PP_text(((
_tmp880="]",_tag_dyneither(_tmp880,sizeof(char),2)))),((_tmp87F[1]=Cyc_Absynpp_exp2doc(
_tmp35F),((_tmp87F[0]=Cyc_PP_text(((_tmp881=".[",_tag_dyneither(_tmp881,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp87F,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL200: {struct Cyc_Absyn_FieldName_struct*_tmp360=(struct Cyc_Absyn_FieldName_struct*)
_tmp35D;if(_tmp360->tag != 1)goto _LL1FD;else{_tmp361=_tmp360->f1;}}_LL201: {const
char*_tmp884;struct Cyc_PP_Doc*_tmp883[2];return(_tmp883[1]=Cyc_PP_textptr(
_tmp361),((_tmp883[0]=Cyc_PP_text(((_tmp884=".",_tag_dyneither(_tmp884,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp883,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL1FD:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple15*de){if((*de).f1 == 0)
return Cyc_Absynpp_exp2doc((*de).f2);else{const char*_tmp88B;const char*_tmp88A;
const char*_tmp889;struct Cyc_PP_Doc*_tmp888[2];return(_tmp888[1]=Cyc_Absynpp_exp2doc((*
de).f2),((_tmp888[0]=Cyc_PP_egroup(((_tmp88B="",_tag_dyneither(_tmp88B,sizeof(
char),1))),((_tmp88A="=",_tag_dyneither(_tmp88A,sizeof(char),2))),((_tmp889="=",
_tag_dyneither(_tmp889,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),
Cyc_PP_cat(_tag_dyneither(_tmp888,sizeof(struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){const char*_tmp88E;
const char*_tmp88D;const char*_tmp88C;return Cyc_PP_group(((_tmp88C="",
_tag_dyneither(_tmp88C,sizeof(char),1))),((_tmp88D="",_tag_dyneither(_tmp88D,
sizeof(char),1))),((_tmp88E=",",_tag_dyneither(_tmp88E,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst
_tmp36E=c;struct _tuple3 _tmp36F;enum Cyc_Absyn_Sign _tmp370;char _tmp371;struct
_dyneither_ptr _tmp372;struct _tuple4 _tmp373;enum Cyc_Absyn_Sign _tmp374;short
_tmp375;struct _tuple5 _tmp376;enum Cyc_Absyn_Sign _tmp377;int _tmp378;struct _tuple5
_tmp379;enum Cyc_Absyn_Sign _tmp37A;int _tmp37B;struct _tuple5 _tmp37C;enum Cyc_Absyn_Sign
_tmp37D;int _tmp37E;struct _tuple6 _tmp37F;enum Cyc_Absyn_Sign _tmp380;long long
_tmp381;struct _tuple7 _tmp382;struct _dyneither_ptr _tmp383;int _tmp384;struct
_dyneither_ptr _tmp385;struct _dyneither_ptr _tmp386;_LL203: if((_tmp36E.Char_c).tag
!= 2)goto _LL205;_tmp36F=(struct _tuple3)(_tmp36E.Char_c).val;_tmp370=_tmp36F.f1;
_tmp371=_tmp36F.f2;_LL204: {const char*_tmp892;void*_tmp891[1];struct Cyc_String_pa_struct
_tmp890;return Cyc_PP_text((struct _dyneither_ptr)((_tmp890.tag=0,((_tmp890.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp371)),((
_tmp891[0]=& _tmp890,Cyc_aprintf(((_tmp892="'%s'",_tag_dyneither(_tmp892,sizeof(
char),5))),_tag_dyneither(_tmp891,sizeof(void*),1)))))))));}_LL205: if((_tmp36E.Wchar_c).tag
!= 3)goto _LL207;_tmp372=(struct _dyneither_ptr)(_tmp36E.Wchar_c).val;_LL206: {
const char*_tmp896;void*_tmp895[1];struct Cyc_String_pa_struct _tmp894;return Cyc_PP_text((
struct _dyneither_ptr)((_tmp894.tag=0,((_tmp894.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp372),((_tmp895[0]=& _tmp894,Cyc_aprintf(((_tmp896="L'%s'",
_tag_dyneither(_tmp896,sizeof(char),6))),_tag_dyneither(_tmp895,sizeof(void*),1)))))))));}
_LL207: if((_tmp36E.Short_c).tag != 4)goto _LL209;_tmp373=(struct _tuple4)(_tmp36E.Short_c).val;
_tmp374=_tmp373.f1;_tmp375=_tmp373.f2;_LL208: {const char*_tmp89A;void*_tmp899[1];
struct Cyc_Int_pa_struct _tmp898;return Cyc_PP_text((struct _dyneither_ptr)((_tmp898.tag=
1,((_tmp898.f1=(unsigned long)((int)_tmp375),((_tmp899[0]=& _tmp898,Cyc_aprintf(((
_tmp89A="%d",_tag_dyneither(_tmp89A,sizeof(char),3))),_tag_dyneither(_tmp899,
sizeof(void*),1)))))))));}_LL209: if((_tmp36E.Int_c).tag != 5)goto _LL20B;_tmp376=(
struct _tuple5)(_tmp36E.Int_c).val;_tmp377=_tmp376.f1;if(_tmp377 != Cyc_Absyn_None)
goto _LL20B;_tmp378=_tmp376.f2;_LL20A: _tmp37B=_tmp378;goto _LL20C;_LL20B: if((
_tmp36E.Int_c).tag != 5)goto _LL20D;_tmp379=(struct _tuple5)(_tmp36E.Int_c).val;
_tmp37A=_tmp379.f1;if(_tmp37A != Cyc_Absyn_Signed)goto _LL20D;_tmp37B=_tmp379.f2;
_LL20C: {const char*_tmp89E;void*_tmp89D[1];struct Cyc_Int_pa_struct _tmp89C;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp89C.tag=1,((_tmp89C.f1=(unsigned long)
_tmp37B,((_tmp89D[0]=& _tmp89C,Cyc_aprintf(((_tmp89E="%d",_tag_dyneither(_tmp89E,
sizeof(char),3))),_tag_dyneither(_tmp89D,sizeof(void*),1)))))))));}_LL20D: if((
_tmp36E.Int_c).tag != 5)goto _LL20F;_tmp37C=(struct _tuple5)(_tmp36E.Int_c).val;
_tmp37D=_tmp37C.f1;if(_tmp37D != Cyc_Absyn_Unsigned)goto _LL20F;_tmp37E=_tmp37C.f2;
_LL20E: {const char*_tmp8A2;void*_tmp8A1[1];struct Cyc_Int_pa_struct _tmp8A0;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp8A0.tag=1,((_tmp8A0.f1=(unsigned int)
_tmp37E,((_tmp8A1[0]=& _tmp8A0,Cyc_aprintf(((_tmp8A2="%u",_tag_dyneither(_tmp8A2,
sizeof(char),3))),_tag_dyneither(_tmp8A1,sizeof(void*),1)))))))));}_LL20F: if((
_tmp36E.LongLong_c).tag != 6)goto _LL211;_tmp37F=(struct _tuple6)(_tmp36E.LongLong_c).val;
_tmp380=_tmp37F.f1;_tmp381=_tmp37F.f2;_LL210: {const char*_tmp8A3;return Cyc_PP_text(((
_tmp8A3="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp8A3,sizeof(char),27))));}
_LL211: if((_tmp36E.Float_c).tag != 7)goto _LL213;_tmp382=(struct _tuple7)(_tmp36E.Float_c).val;
_tmp383=_tmp382.f1;_LL212: return Cyc_PP_text(_tmp383);_LL213: if((_tmp36E.Null_c).tag
!= 1)goto _LL215;_tmp384=(int)(_tmp36E.Null_c).val;_LL214: {const char*_tmp8A4;
return Cyc_PP_text(((_tmp8A4="NULL",_tag_dyneither(_tmp8A4,sizeof(char),5))));}
_LL215: if((_tmp36E.String_c).tag != 8)goto _LL217;_tmp385=(struct _dyneither_ptr)(
_tmp36E.String_c).val;_LL216: {const char*_tmp8A9;const char*_tmp8A8;struct Cyc_PP_Doc*
_tmp8A7[3];return(_tmp8A7[2]=Cyc_PP_text(((_tmp8A8="\"",_tag_dyneither(_tmp8A8,
sizeof(char),2)))),((_tmp8A7[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp385)),((
_tmp8A7[0]=Cyc_PP_text(((_tmp8A9="\"",_tag_dyneither(_tmp8A9,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp8A7,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL217: if((
_tmp36E.Wstring_c).tag != 9)goto _LL202;_tmp386=(struct _dyneither_ptr)(_tmp36E.Wstring_c).val;
_LL218: {const char*_tmp8AE;const char*_tmp8AD;struct Cyc_PP_Doc*_tmp8AC[3];return(
_tmp8AC[2]=Cyc_PP_text(((_tmp8AD="\"",_tag_dyneither(_tmp8AD,sizeof(char),2)))),((
_tmp8AC[1]=Cyc_PP_text(_tmp386),((_tmp8AC[0]=Cyc_PP_text(((_tmp8AE="L\"",
_tag_dyneither(_tmp8AE,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp8AC,
sizeof(struct Cyc_PP_Doc*),3)))))));}_LL202:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(
int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(
p);if(p == (enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){if(es == 0  || es->tl != 0){
struct Cyc_Core_Failure_struct _tmp8BB;const char*_tmp8BA;void*_tmp8B9[1];struct Cyc_String_pa_struct
_tmp8B8;struct Cyc_Core_Failure_struct*_tmp8B7;(int)_throw((void*)((_tmp8B7=
_cycalloc(sizeof(*_tmp8B7)),((_tmp8B7[0]=((_tmp8BB.tag=Cyc_Core_Failure,((
_tmp8BB.f1=(struct _dyneither_ptr)((_tmp8B8.tag=0,((_tmp8B8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8B9[0]=&
_tmp8B8,Cyc_aprintf(((_tmp8BA="Absynpp::primapp2doc Numelts: %s with bad args",
_tag_dyneither(_tmp8BA,sizeof(char),47))),_tag_dyneither(_tmp8B9,sizeof(void*),1)))))))),
_tmp8BB)))),_tmp8B7)))));}{const char*_tmp8C0;const char*_tmp8BF;struct Cyc_PP_Doc*
_tmp8BE[3];return(_tmp8BE[2]=Cyc_PP_text(((_tmp8BF=")",_tag_dyneither(_tmp8BF,
sizeof(char),2)))),((_tmp8BE[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((
_tmp8BE[0]=Cyc_PP_text(((_tmp8C0="numelts(",_tag_dyneither(_tmp8C0,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp8BE,sizeof(struct Cyc_PP_Doc*),3)))))));};}
else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0){struct Cyc_Core_Failure_struct _tmp8CD;const char*_tmp8CC;
void*_tmp8CB[1];struct Cyc_String_pa_struct _tmp8CA;struct Cyc_Core_Failure_struct*
_tmp8C9;(int)_throw((void*)((_tmp8C9=_cycalloc(sizeof(*_tmp8C9)),((_tmp8C9[0]=((
_tmp8CD.tag=Cyc_Core_Failure,((_tmp8CD.f1=(struct _dyneither_ptr)((_tmp8CA.tag=0,((
_tmp8CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp8CB[0]=& _tmp8CA,Cyc_aprintf(((_tmp8CC="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp8CC,sizeof(char),38))),_tag_dyneither(_tmp8CB,sizeof(void*),1)))))))),
_tmp8CD)))),_tmp8C9)))));}else{if(ds->tl == 0){const char*_tmp8D0;struct Cyc_PP_Doc*
_tmp8CF[3];return(_tmp8CF[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp8CF[1]=Cyc_PP_text(((
_tmp8D0=" ",_tag_dyneither(_tmp8D0,sizeof(char),2)))),((_tmp8CF[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp8CF,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp8DD;const char*
_tmp8DC;void*_tmp8DB[1];struct Cyc_String_pa_struct _tmp8DA;struct Cyc_Core_Failure_struct*
_tmp8D9;(int)_throw((void*)((_tmp8D9=_cycalloc(sizeof(*_tmp8D9)),((_tmp8D9[0]=((
_tmp8DD.tag=Cyc_Core_Failure,((_tmp8DD.f1=(struct _dyneither_ptr)((_tmp8DA.tag=0,((
_tmp8DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp8DB[0]=& _tmp8DA,Cyc_aprintf(((_tmp8DC="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp8DC,sizeof(char),47))),_tag_dyneither(_tmp8DB,sizeof(void*),1)))))))),
_tmp8DD)))),_tmp8D9)))));}else{const char*_tmp8E2;const char*_tmp8E1;struct Cyc_PP_Doc*
_tmp8E0[5];return(_tmp8E0[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp8E0[3]=Cyc_PP_text(((_tmp8E1=" ",_tag_dyneither(
_tmp8E1,sizeof(char),2)))),((_tmp8E0[2]=ps,((_tmp8E0[1]=Cyc_PP_text(((_tmp8E2=" ",
_tag_dyneither(_tmp8E2,sizeof(char),2)))),((_tmp8E0[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp8E0,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){switch(p){case
Cyc_Absyn_Plus: _LL219: {const char*_tmp8E3;return(_tmp8E3="+",_tag_dyneither(
_tmp8E3,sizeof(char),2));}case Cyc_Absyn_Times: _LL21A: {const char*_tmp8E4;return(
_tmp8E4="*",_tag_dyneither(_tmp8E4,sizeof(char),2));}case Cyc_Absyn_Minus: _LL21B: {
const char*_tmp8E5;return(_tmp8E5="-",_tag_dyneither(_tmp8E5,sizeof(char),2));}
case Cyc_Absyn_Div: _LL21C: {const char*_tmp8E6;return(_tmp8E6="/",_tag_dyneither(
_tmp8E6,sizeof(char),2));}case Cyc_Absyn_Mod: _LL21D: {const char*_tmp8E8;const char*
_tmp8E7;return Cyc_Absynpp_print_for_cycdoc?(_tmp8E8="\\%",_tag_dyneither(_tmp8E8,
sizeof(char),3)):((_tmp8E7="%",_tag_dyneither(_tmp8E7,sizeof(char),2)));}case Cyc_Absyn_Eq:
_LL21E: {const char*_tmp8E9;return(_tmp8E9="==",_tag_dyneither(_tmp8E9,sizeof(
char),3));}case Cyc_Absyn_Neq: _LL21F: {const char*_tmp8EA;return(_tmp8EA="!=",
_tag_dyneither(_tmp8EA,sizeof(char),3));}case Cyc_Absyn_Gt: _LL220: {const char*
_tmp8EB;return(_tmp8EB=">",_tag_dyneither(_tmp8EB,sizeof(char),2));}case Cyc_Absyn_Lt:
_LL221: {const char*_tmp8EC;return(_tmp8EC="<",_tag_dyneither(_tmp8EC,sizeof(char),
2));}case Cyc_Absyn_Gte: _LL222: {const char*_tmp8ED;return(_tmp8ED=">=",
_tag_dyneither(_tmp8ED,sizeof(char),3));}case Cyc_Absyn_Lte: _LL223: {const char*
_tmp8EE;return(_tmp8EE="<=",_tag_dyneither(_tmp8EE,sizeof(char),3));}case Cyc_Absyn_Not:
_LL224: {const char*_tmp8EF;return(_tmp8EF="!",_tag_dyneither(_tmp8EF,sizeof(char),
2));}case Cyc_Absyn_Bitnot: _LL225: {const char*_tmp8F0;return(_tmp8F0="~",
_tag_dyneither(_tmp8F0,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL226: {const char*
_tmp8F1;return(_tmp8F1="&",_tag_dyneither(_tmp8F1,sizeof(char),2));}case Cyc_Absyn_Bitor:
_LL227: {const char*_tmp8F2;return(_tmp8F2="|",_tag_dyneither(_tmp8F2,sizeof(char),
2));}case Cyc_Absyn_Bitxor: _LL228: {const char*_tmp8F3;return(_tmp8F3="^",
_tag_dyneither(_tmp8F3,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL229: {const
char*_tmp8F4;return(_tmp8F4="<<",_tag_dyneither(_tmp8F4,sizeof(char),3));}case
Cyc_Absyn_Bitlrshift: _LL22A: {const char*_tmp8F5;return(_tmp8F5=">>",
_tag_dyneither(_tmp8F5,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL22B: {const
char*_tmp8F6;return(_tmp8F6=">>>",_tag_dyneither(_tmp8F6,sizeof(char),4));}case
Cyc_Absyn_Numelts: _LL22C: {const char*_tmp8F7;return(_tmp8F7="numelts",
_tag_dyneither(_tmp8F7,sizeof(char),8));}}}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
enum Cyc_Absyn_Primop p){return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp3CA=s->r;_LL22F: {struct Cyc_Absyn_Decl_s_struct*
_tmp3CB=(struct Cyc_Absyn_Decl_s_struct*)_tmp3CA;if(_tmp3CB->tag != 12)goto _LL231;}
_LL230: return 1;_LL231:;_LL232: return 0;_LL22E:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st,int stmtexp){struct Cyc_PP_Doc*s;{void*_tmp3CC=st->r;
struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Absyn_Stmt*_tmp3D1;struct Cyc_Absyn_Stmt*
_tmp3D2;struct Cyc_Absyn_Exp*_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D6;struct Cyc_Absyn_Stmt*
_tmp3D7;struct Cyc_Absyn_Stmt*_tmp3D8;struct _tuple9 _tmp3DA;struct Cyc_Absyn_Exp*
_tmp3DB;struct Cyc_Absyn_Stmt*_tmp3DC;struct _dyneither_ptr*_tmp3E0;struct Cyc_Absyn_Exp*
_tmp3E2;struct _tuple9 _tmp3E3;struct Cyc_Absyn_Exp*_tmp3E4;struct _tuple9 _tmp3E5;
struct Cyc_Absyn_Exp*_tmp3E6;struct Cyc_Absyn_Stmt*_tmp3E7;struct Cyc_Absyn_Exp*
_tmp3E9;struct Cyc_List_List*_tmp3EA;struct Cyc_List_List*_tmp3EC;struct Cyc_List_List*
_tmp3EE;struct Cyc_Absyn_Decl*_tmp3F0;struct Cyc_Absyn_Stmt*_tmp3F1;struct
_dyneither_ptr*_tmp3F3;struct Cyc_Absyn_Stmt*_tmp3F4;struct Cyc_Absyn_Stmt*_tmp3F6;
struct _tuple9 _tmp3F7;struct Cyc_Absyn_Exp*_tmp3F8;struct Cyc_Absyn_Stmt*_tmp3FA;
struct Cyc_List_List*_tmp3FB;struct Cyc_Absyn_Exp*_tmp3FD;_LL234: {struct Cyc_Absyn_Skip_s_struct*
_tmp3CD=(struct Cyc_Absyn_Skip_s_struct*)_tmp3CC;if(_tmp3CD->tag != 0)goto _LL236;}
_LL235:{const char*_tmp8F8;s=Cyc_PP_text(((_tmp8F8=";",_tag_dyneither(_tmp8F8,
sizeof(char),2))));}goto _LL233;_LL236: {struct Cyc_Absyn_Exp_s_struct*_tmp3CE=(
struct Cyc_Absyn_Exp_s_struct*)_tmp3CC;if(_tmp3CE->tag != 1)goto _LL238;else{
_tmp3CF=_tmp3CE->f1;}}_LL237:{const char*_tmp8FB;struct Cyc_PP_Doc*_tmp8FA[2];s=((
_tmp8FA[1]=Cyc_PP_text(((_tmp8FB=";",_tag_dyneither(_tmp8FB,sizeof(char),2)))),((
_tmp8FA[0]=Cyc_Absynpp_exp2doc(_tmp3CF),Cyc_PP_cat(_tag_dyneither(_tmp8FA,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL233;_LL238: {struct Cyc_Absyn_Seq_s_struct*
_tmp3D0=(struct Cyc_Absyn_Seq_s_struct*)_tmp3CC;if(_tmp3D0->tag != 2)goto _LL23A;
else{_tmp3D1=_tmp3D0->f1;_tmp3D2=_tmp3D0->f2;}}_LL239: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3D1)){struct Cyc_PP_Doc*_tmp906[5];struct Cyc_PP_Doc*
_tmp905[7];const char*_tmp904;const char*_tmp903;struct Cyc_PP_Doc*_tmp902[7];s=((
_tmp902[6]=Cyc_Absynpp_is_declaration(_tmp3D2)?stmtexp?(_tmp905[6]=Cyc_PP_line_doc(),((
_tmp905[5]=Cyc_PP_text(((_tmp904=");",_tag_dyneither(_tmp904,sizeof(char),3)))),((
_tmp905[4]=Cyc_Absynpp_rb(),((_tmp905[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3D2,stmtexp)),((_tmp905[2]=Cyc_PP_blank_doc(),((_tmp905[1]=Cyc_Absynpp_lb(),((
_tmp905[0]=Cyc_PP_text(((_tmp903="(",_tag_dyneither(_tmp903,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp905,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):((
_tmp906[4]=Cyc_PP_line_doc(),((_tmp906[3]=Cyc_Absynpp_rb(),((_tmp906[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3D2,stmtexp)),((_tmp906[1]=Cyc_PP_blank_doc(),((
_tmp906[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp906,sizeof(struct Cyc_PP_Doc*),
5)))))))))))): Cyc_Absynpp_stmt2doc(_tmp3D2,stmtexp),((_tmp902[5]=Cyc_PP_line_doc(),((
_tmp902[4]=Cyc_Absynpp_rb(),((_tmp902[3]=Cyc_PP_line_doc(),((_tmp902[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3D1,0)),((_tmp902[1]=Cyc_PP_blank_doc(),((_tmp902[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp902,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
else{if(Cyc_Absynpp_is_declaration(_tmp3D2)){struct Cyc_PP_Doc*_tmp911[4];struct
Cyc_PP_Doc*_tmp910[6];const char*_tmp90F;const char*_tmp90E;struct Cyc_PP_Doc*
_tmp90D[4];s=((_tmp90D[3]=Cyc_PP_line_doc(),((_tmp90D[2]=stmtexp?(_tmp910[5]=Cyc_PP_text(((
_tmp90F=");",_tag_dyneither(_tmp90F,sizeof(char),3)))),((_tmp910[4]=Cyc_Absynpp_rb(),((
_tmp910[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3D2,stmtexp)),((_tmp910[2]=Cyc_PP_blank_doc(),((
_tmp910[1]=Cyc_Absynpp_lb(),((_tmp910[0]=Cyc_PP_text(((_tmp90E="(",
_tag_dyneither(_tmp90E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp910,
sizeof(struct Cyc_PP_Doc*),6))))))))))))):((_tmp911[3]=Cyc_Absynpp_rb(),((_tmp911[
2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3D2,stmtexp)),((_tmp911[1]=Cyc_PP_blank_doc(),((
_tmp911[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp911,sizeof(struct Cyc_PP_Doc*),
4)))))))))),((_tmp90D[1]=Cyc_PP_line_doc(),((_tmp90D[0]=Cyc_Absynpp_stmt2doc(
_tmp3D1,0),Cyc_PP_cat(_tag_dyneither(_tmp90D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
else{struct Cyc_PP_Doc*_tmp912[3];s=((_tmp912[2]=Cyc_Absynpp_stmt2doc(_tmp3D2,
stmtexp),((_tmp912[1]=Cyc_PP_line_doc(),((_tmp912[0]=Cyc_Absynpp_stmt2doc(
_tmp3D1,0),Cyc_PP_cat(_tag_dyneither(_tmp912,sizeof(struct Cyc_PP_Doc*),3))))))));}}}
else{struct Cyc_PP_Doc*_tmp913[3];s=((_tmp913[2]=Cyc_Absynpp_stmt2doc(_tmp3D2,
stmtexp),((_tmp913[1]=Cyc_PP_line_doc(),((_tmp913[0]=Cyc_Absynpp_stmt2doc(
_tmp3D1,0),Cyc_PP_cat(_tag_dyneither(_tmp913,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL233;_LL23A: {struct Cyc_Absyn_Return_s_struct*_tmp3D3=(struct Cyc_Absyn_Return_s_struct*)
_tmp3CC;if(_tmp3D3->tag != 3)goto _LL23C;else{_tmp3D4=_tmp3D3->f1;}}_LL23B: if(
_tmp3D4 == 0){const char*_tmp914;s=Cyc_PP_text(((_tmp914="return;",_tag_dyneither(
_tmp914,sizeof(char),8))));}else{const char*_tmp919;const char*_tmp918;struct Cyc_PP_Doc*
_tmp917[3];s=((_tmp917[2]=Cyc_PP_text(((_tmp918=";",_tag_dyneither(_tmp918,
sizeof(char),2)))),((_tmp917[1]=_tmp3D4 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp3D4),((_tmp917[0]=Cyc_PP_text(((_tmp919="return ",
_tag_dyneither(_tmp919,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp917,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL233;_LL23C: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp3D5=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3CC;if(_tmp3D5->tag != 4)goto
_LL23E;else{_tmp3D6=_tmp3D5->f1;_tmp3D7=_tmp3D5->f2;_tmp3D8=_tmp3D5->f3;}}_LL23D: {
int print_else;{void*_tmp411=_tmp3D8->r;_LL259: {struct Cyc_Absyn_Skip_s_struct*
_tmp412=(struct Cyc_Absyn_Skip_s_struct*)_tmp411;if(_tmp412->tag != 0)goto _LL25B;}
_LL25A: print_else=0;goto _LL258;_LL25B:;_LL25C: print_else=1;goto _LL258;_LL258:;}{
const char*_tmp928;const char*_tmp927;struct Cyc_PP_Doc*_tmp926[2];struct Cyc_PP_Doc*
_tmp925[6];struct Cyc_PP_Doc*_tmp924[2];const char*_tmp923;struct Cyc_PP_Doc*
_tmp922[8];s=((_tmp922[7]=print_else?(_tmp925[5]=Cyc_Absynpp_rb(),((_tmp925[4]=
Cyc_PP_line_doc(),((_tmp925[3]=Cyc_PP_nest(2,((_tmp924[1]=Cyc_Absynpp_stmt2doc(
_tmp3D8,0),((_tmp924[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp924,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp925[2]=Cyc_Absynpp_lb(),((_tmp925[1]=
Cyc_PP_text(((_tmp923="else ",_tag_dyneither(_tmp923,sizeof(char),6)))),((
_tmp925[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp925,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp922[6]=Cyc_Absynpp_rb(),((_tmp922[5]=Cyc_PP_line_doc(),((
_tmp922[4]=Cyc_PP_nest(2,((_tmp926[1]=Cyc_Absynpp_stmt2doc(_tmp3D7,0),((_tmp926[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp926,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp922[3]=Cyc_Absynpp_lb(),((_tmp922[2]=Cyc_PP_text(((_tmp927=") ",
_tag_dyneither(_tmp927,sizeof(char),3)))),((_tmp922[1]=Cyc_Absynpp_exp2doc(
_tmp3D6),((_tmp922[0]=Cyc_PP_text(((_tmp928="if (",_tag_dyneither(_tmp928,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp922,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL233;}_LL23E: {struct Cyc_Absyn_While_s_struct*_tmp3D9=(struct Cyc_Absyn_While_s_struct*)
_tmp3CC;if(_tmp3D9->tag != 5)goto _LL240;else{_tmp3DA=_tmp3D9->f1;_tmp3DB=_tmp3DA.f1;
_tmp3DC=_tmp3D9->f2;}}_LL23F:{const char*_tmp92F;const char*_tmp92E;struct Cyc_PP_Doc*
_tmp92D[2];struct Cyc_PP_Doc*_tmp92C[7];s=((_tmp92C[6]=Cyc_Absynpp_rb(),((_tmp92C[
5]=Cyc_PP_line_doc(),((_tmp92C[4]=Cyc_PP_nest(2,((_tmp92D[1]=Cyc_Absynpp_stmt2doc(
_tmp3DC,0),((_tmp92D[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp92D,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp92C[3]=Cyc_Absynpp_lb(),((_tmp92C[2]=
Cyc_PP_text(((_tmp92E=") ",_tag_dyneither(_tmp92E,sizeof(char),3)))),((_tmp92C[1]=
Cyc_Absynpp_exp2doc(_tmp3DB),((_tmp92C[0]=Cyc_PP_text(((_tmp92F="while (",
_tag_dyneither(_tmp92F,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp92C,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL233;_LL240: {struct Cyc_Absyn_Break_s_struct*
_tmp3DD=(struct Cyc_Absyn_Break_s_struct*)_tmp3CC;if(_tmp3DD->tag != 6)goto _LL242;}
_LL241:{const char*_tmp930;s=Cyc_PP_text(((_tmp930="break;",_tag_dyneither(
_tmp930,sizeof(char),7))));}goto _LL233;_LL242: {struct Cyc_Absyn_Continue_s_struct*
_tmp3DE=(struct Cyc_Absyn_Continue_s_struct*)_tmp3CC;if(_tmp3DE->tag != 7)goto
_LL244;}_LL243:{const char*_tmp931;s=Cyc_PP_text(((_tmp931="continue;",
_tag_dyneither(_tmp931,sizeof(char),10))));}goto _LL233;_LL244: {struct Cyc_Absyn_Goto_s_struct*
_tmp3DF=(struct Cyc_Absyn_Goto_s_struct*)_tmp3CC;if(_tmp3DF->tag != 8)goto _LL246;
else{_tmp3E0=_tmp3DF->f1;}}_LL245:{const char*_tmp935;void*_tmp934[1];struct Cyc_String_pa_struct
_tmp933;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp933.tag=0,((_tmp933.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3E0),((_tmp934[0]=& _tmp933,Cyc_aprintf(((
_tmp935="goto %s;",_tag_dyneither(_tmp935,sizeof(char),9))),_tag_dyneither(
_tmp934,sizeof(void*),1)))))))));}goto _LL233;_LL246: {struct Cyc_Absyn_For_s_struct*
_tmp3E1=(struct Cyc_Absyn_For_s_struct*)_tmp3CC;if(_tmp3E1->tag != 9)goto _LL248;
else{_tmp3E2=_tmp3E1->f1;_tmp3E3=_tmp3E1->f2;_tmp3E4=_tmp3E3.f1;_tmp3E5=_tmp3E1->f3;
_tmp3E6=_tmp3E5.f1;_tmp3E7=_tmp3E1->f4;}}_LL247:{const char*_tmp940;const char*
_tmp93F;const char*_tmp93E;const char*_tmp93D;struct Cyc_PP_Doc*_tmp93C[2];struct
Cyc_PP_Doc*_tmp93B[11];s=((_tmp93B[10]=Cyc_Absynpp_rb(),((_tmp93B[9]=Cyc_PP_line_doc(),((
_tmp93B[8]=Cyc_PP_nest(2,((_tmp93C[1]=Cyc_Absynpp_stmt2doc(_tmp3E7,0),((_tmp93C[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp93C,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp93B[7]=Cyc_Absynpp_lb(),((_tmp93B[6]=Cyc_PP_text(((_tmp93D=") ",
_tag_dyneither(_tmp93D,sizeof(char),3)))),((_tmp93B[5]=Cyc_Absynpp_exp2doc(
_tmp3E6),((_tmp93B[4]=Cyc_PP_text(((_tmp93E="; ",_tag_dyneither(_tmp93E,sizeof(
char),3)))),((_tmp93B[3]=Cyc_Absynpp_exp2doc(_tmp3E4),((_tmp93B[2]=Cyc_PP_text(((
_tmp93F="; ",_tag_dyneither(_tmp93F,sizeof(char),3)))),((_tmp93B[1]=Cyc_Absynpp_exp2doc(
_tmp3E2),((_tmp93B[0]=Cyc_PP_text(((_tmp940="for(",_tag_dyneither(_tmp940,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp93B,sizeof(struct Cyc_PP_Doc*),
11))))))))))))))))))))))));}goto _LL233;_LL248: {struct Cyc_Absyn_Switch_s_struct*
_tmp3E8=(struct Cyc_Absyn_Switch_s_struct*)_tmp3CC;if(_tmp3E8->tag != 10)goto
_LL24A;else{_tmp3E9=_tmp3E8->f1;_tmp3EA=_tmp3E8->f2;}}_LL249:{const char*_tmp945;
const char*_tmp944;struct Cyc_PP_Doc*_tmp943[8];s=((_tmp943[7]=Cyc_Absynpp_rb(),((
_tmp943[6]=Cyc_PP_line_doc(),((_tmp943[5]=Cyc_Absynpp_switchclauses2doc(_tmp3EA),((
_tmp943[4]=Cyc_PP_line_doc(),((_tmp943[3]=Cyc_Absynpp_lb(),((_tmp943[2]=Cyc_PP_text(((
_tmp944=") ",_tag_dyneither(_tmp944,sizeof(char),3)))),((_tmp943[1]=Cyc_Absynpp_exp2doc(
_tmp3E9),((_tmp943[0]=Cyc_PP_text(((_tmp945="switch (",_tag_dyneither(_tmp945,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp943,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL233;_LL24A: {struct Cyc_Absyn_Fallthru_s_struct*_tmp3EB=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp3CC;if(_tmp3EB->tag != 11)goto _LL24C;else{_tmp3EC=_tmp3EB->f1;if(_tmp3EC != 0)
goto _LL24C;}}_LL24B:{const char*_tmp946;s=Cyc_PP_text(((_tmp946="fallthru;",
_tag_dyneither(_tmp946,sizeof(char),10))));}goto _LL233;_LL24C: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp3ED=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp3CC;if(_tmp3ED->tag != 11)goto
_LL24E;else{_tmp3EE=_tmp3ED->f1;}}_LL24D:{const char*_tmp94B;const char*_tmp94A;
struct Cyc_PP_Doc*_tmp949[3];s=((_tmp949[2]=Cyc_PP_text(((_tmp94A=");",
_tag_dyneither(_tmp94A,sizeof(char),3)))),((_tmp949[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp3EE),((_tmp949[0]=Cyc_PP_text(((_tmp94B="fallthru(",_tag_dyneither(
_tmp94B,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp949,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL233;_LL24E: {struct Cyc_Absyn_Decl_s_struct*_tmp3EF=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3CC;if(_tmp3EF->tag != 12)goto _LL250;else{_tmp3F0=_tmp3EF->f1;_tmp3F1=_tmp3EF->f2;}}
_LL24F:{struct Cyc_PP_Doc*_tmp94C[3];s=((_tmp94C[2]=Cyc_Absynpp_stmt2doc(_tmp3F1,
stmtexp),((_tmp94C[1]=Cyc_PP_line_doc(),((_tmp94C[0]=Cyc_Absynpp_decl2doc(
_tmp3F0),Cyc_PP_cat(_tag_dyneither(_tmp94C,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL233;_LL250: {struct Cyc_Absyn_Label_s_struct*_tmp3F2=(struct Cyc_Absyn_Label_s_struct*)
_tmp3CC;if(_tmp3F2->tag != 13)goto _LL252;else{_tmp3F3=_tmp3F2->f1;_tmp3F4=_tmp3F2->f2;}}
_LL251: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3F4)){if(
stmtexp){const char*_tmp951;const char*_tmp950;struct Cyc_PP_Doc*_tmp94F[8];s=((
_tmp94F[7]=Cyc_PP_text(((_tmp950=");",_tag_dyneither(_tmp950,sizeof(char),3)))),((
_tmp94F[6]=Cyc_Absynpp_rb(),((_tmp94F[5]=Cyc_PP_line_doc(),((_tmp94F[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3F4,1)),((_tmp94F[3]=Cyc_PP_line_doc(),((_tmp94F[2]=
Cyc_Absynpp_lb(),((_tmp94F[1]=Cyc_PP_text(((_tmp951=": (",_tag_dyneither(_tmp951,
sizeof(char),4)))),((_tmp94F[0]=Cyc_PP_textptr(_tmp3F3),Cyc_PP_cat(
_tag_dyneither(_tmp94F,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{const
char*_tmp954;struct Cyc_PP_Doc*_tmp953[7];s=((_tmp953[6]=Cyc_Absynpp_rb(),((
_tmp953[5]=Cyc_PP_line_doc(),((_tmp953[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3F4,0)),((_tmp953[3]=Cyc_PP_line_doc(),((_tmp953[2]=Cyc_Absynpp_lb(),((
_tmp953[1]=Cyc_PP_text(((_tmp954=": ",_tag_dyneither(_tmp954,sizeof(char),3)))),((
_tmp953[0]=Cyc_PP_textptr(_tmp3F3),Cyc_PP_cat(_tag_dyneither(_tmp953,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}}else{const char*_tmp957;struct Cyc_PP_Doc*
_tmp956[3];s=((_tmp956[2]=Cyc_Absynpp_stmt2doc(_tmp3F4,stmtexp),((_tmp956[1]=Cyc_PP_text(((
_tmp957=": ",_tag_dyneither(_tmp957,sizeof(char),3)))),((_tmp956[0]=Cyc_PP_textptr(
_tmp3F3),Cyc_PP_cat(_tag_dyneither(_tmp956,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL233;_LL252: {struct Cyc_Absyn_Do_s_struct*_tmp3F5=(struct Cyc_Absyn_Do_s_struct*)
_tmp3CC;if(_tmp3F5->tag != 14)goto _LL254;else{_tmp3F6=_tmp3F5->f1;_tmp3F7=_tmp3F5->f2;
_tmp3F8=_tmp3F7.f1;}}_LL253:{const char*_tmp95E;const char*_tmp95D;const char*
_tmp95C;struct Cyc_PP_Doc*_tmp95B[9];s=((_tmp95B[8]=Cyc_PP_text(((_tmp95C=");",
_tag_dyneither(_tmp95C,sizeof(char),3)))),((_tmp95B[7]=Cyc_Absynpp_exp2doc(
_tmp3F8),((_tmp95B[6]=Cyc_PP_text(((_tmp95D=" while (",_tag_dyneither(_tmp95D,
sizeof(char),9)))),((_tmp95B[5]=Cyc_Absynpp_rb(),((_tmp95B[4]=Cyc_PP_line_doc(),((
_tmp95B[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3F6,0)),((_tmp95B[2]=Cyc_PP_line_doc(),((
_tmp95B[1]=Cyc_Absynpp_lb(),((_tmp95B[0]=Cyc_PP_text(((_tmp95E="do ",
_tag_dyneither(_tmp95E,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp95B,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL233;_LL254: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp3F9=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp3CC;if(_tmp3F9->tag != 15)goto
_LL256;else{_tmp3FA=_tmp3F9->f1;_tmp3FB=_tmp3F9->f2;}}_LL255:{const char*_tmp963;
const char*_tmp962;struct Cyc_PP_Doc*_tmp961[12];s=((_tmp961[11]=Cyc_Absynpp_rb(),((
_tmp961[10]=Cyc_PP_line_doc(),((_tmp961[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp3FB)),((_tmp961[8]=Cyc_PP_line_doc(),((_tmp961[7]=Cyc_Absynpp_lb(),((_tmp961[
6]=Cyc_PP_text(((_tmp962=" catch ",_tag_dyneither(_tmp962,sizeof(char),8)))),((
_tmp961[5]=Cyc_Absynpp_rb(),((_tmp961[4]=Cyc_PP_line_doc(),((_tmp961[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3FA,0)),((_tmp961[2]=Cyc_PP_line_doc(),((_tmp961[1]=
Cyc_Absynpp_lb(),((_tmp961[0]=Cyc_PP_text(((_tmp963="try ",_tag_dyneither(
_tmp963,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp961,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL233;_LL256: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp3FC=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp3CC;if(_tmp3FC->tag != 16)goto
_LL233;else{_tmp3FD=_tmp3FC->f1;}}_LL257:{const char*_tmp968;const char*_tmp967;
struct Cyc_PP_Doc*_tmp966[3];s=((_tmp966[2]=Cyc_PP_text(((_tmp967=");",
_tag_dyneither(_tmp967,sizeof(char),3)))),((_tmp966[1]=Cyc_Absynpp_exp2doc(
_tmp3FD),((_tmp966[0]=Cyc_PP_text(((_tmp968="reset_region(",_tag_dyneither(
_tmp968,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp966,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL233;_LL233:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp442=p->r;enum Cyc_Absyn_Sign
_tmp446;int _tmp447;char _tmp449;struct _dyneither_ptr _tmp44B;struct Cyc_Absyn_Vardecl*
_tmp44D;struct Cyc_Absyn_Pat*_tmp44E;struct Cyc_Absyn_Pat _tmp44F;void*_tmp450;
struct Cyc_Absyn_Vardecl*_tmp453;struct Cyc_Absyn_Pat*_tmp454;struct Cyc_Absyn_Tvar*
_tmp456;struct Cyc_Absyn_Vardecl*_tmp457;struct Cyc_List_List*_tmp459;int _tmp45A;
struct Cyc_Absyn_Pat*_tmp45C;struct Cyc_Absyn_Vardecl*_tmp45E;struct Cyc_Absyn_Pat*
_tmp45F;struct Cyc_Absyn_Pat _tmp460;void*_tmp461;struct Cyc_Absyn_Vardecl*_tmp464;
struct Cyc_Absyn_Pat*_tmp465;struct _tuple0*_tmp467;struct _tuple0*_tmp469;struct
Cyc_List_List*_tmp46A;int _tmp46B;struct Cyc_Absyn_AggrInfo*_tmp46D;struct Cyc_Absyn_AggrInfo
_tmp46E;union Cyc_Absyn_AggrInfoU _tmp46F;struct Cyc_List_List*_tmp470;struct Cyc_List_List*
_tmp471;int _tmp472;struct Cyc_Absyn_AggrInfo*_tmp474;struct Cyc_List_List*_tmp475;
struct Cyc_List_List*_tmp476;int _tmp477;struct Cyc_Absyn_Enumfield*_tmp479;struct
Cyc_Absyn_Enumfield*_tmp47B;struct Cyc_Absyn_Datatypefield*_tmp47D;struct Cyc_List_List*
_tmp47E;struct Cyc_Absyn_Datatypefield*_tmp480;struct Cyc_List_List*_tmp481;int
_tmp482;struct Cyc_Absyn_Exp*_tmp484;_LL25E: {struct Cyc_Absyn_Wild_p_struct*
_tmp443=(struct Cyc_Absyn_Wild_p_struct*)_tmp442;if(_tmp443->tag != 0)goto _LL260;}
_LL25F:{const char*_tmp969;s=Cyc_PP_text(((_tmp969="_",_tag_dyneither(_tmp969,
sizeof(char),2))));}goto _LL25D;_LL260: {struct Cyc_Absyn_Null_p_struct*_tmp444=(
struct Cyc_Absyn_Null_p_struct*)_tmp442;if(_tmp444->tag != 8)goto _LL262;}_LL261:{
const char*_tmp96A;s=Cyc_PP_text(((_tmp96A="NULL",_tag_dyneither(_tmp96A,sizeof(
char),5))));}goto _LL25D;_LL262: {struct Cyc_Absyn_Int_p_struct*_tmp445=(struct Cyc_Absyn_Int_p_struct*)
_tmp442;if(_tmp445->tag != 9)goto _LL264;else{_tmp446=_tmp445->f1;_tmp447=_tmp445->f2;}}
_LL263: if(_tmp446 != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){const char*_tmp96E;
void*_tmp96D[1];struct Cyc_Int_pa_struct _tmp96C;s=Cyc_PP_text((struct
_dyneither_ptr)((_tmp96C.tag=1,((_tmp96C.f1=(unsigned long)_tmp447,((_tmp96D[0]=&
_tmp96C,Cyc_aprintf(((_tmp96E="%d",_tag_dyneither(_tmp96E,sizeof(char),3))),
_tag_dyneither(_tmp96D,sizeof(void*),1)))))))));}else{const char*_tmp972;void*
_tmp971[1];struct Cyc_Int_pa_struct _tmp970;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp970.tag=1,((_tmp970.f1=(unsigned int)_tmp447,((_tmp971[0]=& _tmp970,Cyc_aprintf(((
_tmp972="%u",_tag_dyneither(_tmp972,sizeof(char),3))),_tag_dyneither(_tmp971,
sizeof(void*),1)))))))));}goto _LL25D;_LL264: {struct Cyc_Absyn_Char_p_struct*
_tmp448=(struct Cyc_Absyn_Char_p_struct*)_tmp442;if(_tmp448->tag != 10)goto _LL266;
else{_tmp449=_tmp448->f1;}}_LL265:{const char*_tmp976;void*_tmp975[1];struct Cyc_String_pa_struct
_tmp974;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp974.tag=0,((_tmp974.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp449)),((
_tmp975[0]=& _tmp974,Cyc_aprintf(((_tmp976="'%s'",_tag_dyneither(_tmp976,sizeof(
char),5))),_tag_dyneither(_tmp975,sizeof(void*),1)))))))));}goto _LL25D;_LL266: {
struct Cyc_Absyn_Float_p_struct*_tmp44A=(struct Cyc_Absyn_Float_p_struct*)_tmp442;
if(_tmp44A->tag != 11)goto _LL268;else{_tmp44B=_tmp44A->f1;}}_LL267: s=Cyc_PP_text(
_tmp44B);goto _LL25D;_LL268: {struct Cyc_Absyn_Var_p_struct*_tmp44C=(struct Cyc_Absyn_Var_p_struct*)
_tmp442;if(_tmp44C->tag != 1)goto _LL26A;else{_tmp44D=_tmp44C->f1;_tmp44E=_tmp44C->f2;
_tmp44F=*_tmp44E;_tmp450=_tmp44F.r;{struct Cyc_Absyn_Wild_p_struct*_tmp451=(
struct Cyc_Absyn_Wild_p_struct*)_tmp450;if(_tmp451->tag != 0)goto _LL26A;};}}_LL269:
s=Cyc_Absynpp_qvar2doc(_tmp44D->name);goto _LL25D;_LL26A: {struct Cyc_Absyn_Var_p_struct*
_tmp452=(struct Cyc_Absyn_Var_p_struct*)_tmp442;if(_tmp452->tag != 1)goto _LL26C;
else{_tmp453=_tmp452->f1;_tmp454=_tmp452->f2;}}_LL26B:{const char*_tmp979;struct
Cyc_PP_Doc*_tmp978[3];s=((_tmp978[2]=Cyc_Absynpp_pat2doc(_tmp454),((_tmp978[1]=
Cyc_PP_text(((_tmp979=" as ",_tag_dyneither(_tmp979,sizeof(char),5)))),((_tmp978[
0]=Cyc_Absynpp_qvar2doc(_tmp453->name),Cyc_PP_cat(_tag_dyneither(_tmp978,sizeof(
struct Cyc_PP_Doc*),3))))))));}goto _LL25D;_LL26C: {struct Cyc_Absyn_TagInt_p_struct*
_tmp455=(struct Cyc_Absyn_TagInt_p_struct*)_tmp442;if(_tmp455->tag != 3)goto _LL26E;
else{_tmp456=_tmp455->f1;_tmp457=_tmp455->f2;}}_LL26D:{const char*_tmp97E;const
char*_tmp97D;struct Cyc_PP_Doc*_tmp97C[4];s=((_tmp97C[3]=Cyc_PP_text(((_tmp97D=">",
_tag_dyneither(_tmp97D,sizeof(char),2)))),((_tmp97C[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp456)),((_tmp97C[1]=Cyc_PP_text(((_tmp97E="<",_tag_dyneither(_tmp97E,sizeof(
char),2)))),((_tmp97C[0]=Cyc_Absynpp_qvar2doc(_tmp457->name),Cyc_PP_cat(
_tag_dyneither(_tmp97C,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL25D;_LL26E: {
struct Cyc_Absyn_Tuple_p_struct*_tmp458=(struct Cyc_Absyn_Tuple_p_struct*)_tmp442;
if(_tmp458->tag != 4)goto _LL270;else{_tmp459=_tmp458->f1;_tmp45A=_tmp458->f2;}}
_LL26F:{const char*_tmp987;const char*_tmp986;const char*_tmp985;const char*_tmp984;
struct Cyc_PP_Doc*_tmp983[4];s=((_tmp983[3]=_tmp45A?Cyc_PP_text(((_tmp984=", ...)",
_tag_dyneither(_tmp984,sizeof(char),7)))): Cyc_PP_text(((_tmp985=")",
_tag_dyneither(_tmp985,sizeof(char),2)))),((_tmp983[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp986=",",_tag_dyneither(_tmp986,
sizeof(char),2))),_tmp459),((_tmp983[1]=Cyc_PP_text(((_tmp987="(",_tag_dyneither(
_tmp987,sizeof(char),2)))),((_tmp983[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp983,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL25D;_LL270: {
struct Cyc_Absyn_Pointer_p_struct*_tmp45B=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp442;if(_tmp45B->tag != 5)goto _LL272;else{_tmp45C=_tmp45B->f1;}}_LL271:{const
char*_tmp98A;struct Cyc_PP_Doc*_tmp989[2];s=((_tmp989[1]=Cyc_Absynpp_pat2doc(
_tmp45C),((_tmp989[0]=Cyc_PP_text(((_tmp98A="&",_tag_dyneither(_tmp98A,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp989,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL25D;_LL272: {struct Cyc_Absyn_Reference_p_struct*_tmp45D=(struct Cyc_Absyn_Reference_p_struct*)
_tmp442;if(_tmp45D->tag != 2)goto _LL274;else{_tmp45E=_tmp45D->f1;_tmp45F=_tmp45D->f2;
_tmp460=*_tmp45F;_tmp461=_tmp460.r;{struct Cyc_Absyn_Wild_p_struct*_tmp462=(
struct Cyc_Absyn_Wild_p_struct*)_tmp461;if(_tmp462->tag != 0)goto _LL274;};}}_LL273:{
const char*_tmp98D;struct Cyc_PP_Doc*_tmp98C[2];s=((_tmp98C[1]=Cyc_Absynpp_qvar2doc(
_tmp45E->name),((_tmp98C[0]=Cyc_PP_text(((_tmp98D="*",_tag_dyneither(_tmp98D,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp98C,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL25D;_LL274: {struct Cyc_Absyn_Reference_p_struct*_tmp463=(struct Cyc_Absyn_Reference_p_struct*)
_tmp442;if(_tmp463->tag != 2)goto _LL276;else{_tmp464=_tmp463->f1;_tmp465=_tmp463->f2;}}
_LL275:{const char*_tmp992;const char*_tmp991;struct Cyc_PP_Doc*_tmp990[4];s=((
_tmp990[3]=Cyc_Absynpp_pat2doc(_tmp465),((_tmp990[2]=Cyc_PP_text(((_tmp991=" as ",
_tag_dyneither(_tmp991,sizeof(char),5)))),((_tmp990[1]=Cyc_Absynpp_qvar2doc(
_tmp464->name),((_tmp990[0]=Cyc_PP_text(((_tmp992="*",_tag_dyneither(_tmp992,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp990,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL25D;_LL276: {struct Cyc_Absyn_UnknownId_p_struct*_tmp466=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp442;if(_tmp466->tag != 14)goto _LL278;else{_tmp467=_tmp466->f1;}}_LL277: s=Cyc_Absynpp_qvar2doc(
_tmp467);goto _LL25D;_LL278: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp468=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp442;if(_tmp468->tag != 15)goto _LL27A;
else{_tmp469=_tmp468->f1;_tmp46A=_tmp468->f2;_tmp46B=_tmp468->f3;}}_LL279: {
const char*_tmp994;const char*_tmp993;struct _dyneither_ptr term=_tmp46B?(_tmp994=", ...)",
_tag_dyneither(_tmp994,sizeof(char),7)):((_tmp993=")",_tag_dyneither(_tmp993,
sizeof(char),2)));{const char*_tmp999;const char*_tmp998;struct Cyc_PP_Doc*_tmp997[
2];s=((_tmp997[1]=Cyc_PP_group(((_tmp999="(",_tag_dyneither(_tmp999,sizeof(char),
2))),term,((_tmp998=",",_tag_dyneither(_tmp998,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp46A)),((_tmp997[0]=Cyc_Absynpp_qvar2doc(_tmp469),Cyc_PP_cat(
_tag_dyneither(_tmp997,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL25D;}_LL27A: {
struct Cyc_Absyn_Aggr_p_struct*_tmp46C=(struct Cyc_Absyn_Aggr_p_struct*)_tmp442;
if(_tmp46C->tag != 6)goto _LL27C;else{_tmp46D=_tmp46C->f1;if(_tmp46D == 0)goto
_LL27C;_tmp46E=*_tmp46D;_tmp46F=_tmp46E.aggr_info;_tmp470=_tmp46C->f2;_tmp471=
_tmp46C->f3;_tmp472=_tmp46C->f4;}}_LL27B: {const char*_tmp99B;const char*_tmp99A;
struct _dyneither_ptr term=_tmp472?(_tmp99B=", ...}",_tag_dyneither(_tmp99B,
sizeof(char),7)):((_tmp99A="}",_tag_dyneither(_tmp99A,sizeof(char),2)));struct
_tuple0*_tmp4A7;struct _tuple10 _tmp4A6=Cyc_Absyn_aggr_kinded_name(_tmp46F);
_tmp4A7=_tmp4A6.f2;{const char*_tmp9A6;const char*_tmp9A5;const char*_tmp9A4;const
char*_tmp9A3;const char*_tmp9A2;struct Cyc_PP_Doc*_tmp9A1[4];s=((_tmp9A1[3]=Cyc_PP_group(((
_tmp9A3="",_tag_dyneither(_tmp9A3,sizeof(char),1))),term,((_tmp9A2=",",
_tag_dyneither(_tmp9A2,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp471)),((_tmp9A1[2]=Cyc_PP_egroup(((_tmp9A6="[",_tag_dyneither(_tmp9A6,
sizeof(char),2))),((_tmp9A5="]",_tag_dyneither(_tmp9A5,sizeof(char),2))),((
_tmp9A4=",",_tag_dyneither(_tmp9A4,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp470))),((_tmp9A1[1]=
Cyc_Absynpp_lb(),((_tmp9A1[0]=Cyc_Absynpp_qvar2doc(_tmp4A7),Cyc_PP_cat(
_tag_dyneither(_tmp9A1,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL25D;}_LL27C: {
struct Cyc_Absyn_Aggr_p_struct*_tmp473=(struct Cyc_Absyn_Aggr_p_struct*)_tmp442;
if(_tmp473->tag != 6)goto _LL27E;else{_tmp474=_tmp473->f1;if(_tmp474 != 0)goto
_LL27E;_tmp475=_tmp473->f2;_tmp476=_tmp473->f3;_tmp477=_tmp473->f4;}}_LL27D: {
const char*_tmp9A8;const char*_tmp9A7;struct _dyneither_ptr term=_tmp477?(_tmp9A8=", ...}",
_tag_dyneither(_tmp9A8,sizeof(char),7)):((_tmp9A7="}",_tag_dyneither(_tmp9A7,
sizeof(char),2)));{const char*_tmp9B3;const char*_tmp9B2;const char*_tmp9B1;const
char*_tmp9B0;const char*_tmp9AF;struct Cyc_PP_Doc*_tmp9AE[3];s=((_tmp9AE[2]=Cyc_PP_group(((
_tmp9B0="",_tag_dyneither(_tmp9B0,sizeof(char),1))),term,((_tmp9AF=",",
_tag_dyneither(_tmp9AF,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp476)),((_tmp9AE[1]=Cyc_PP_egroup(((_tmp9B3="[",_tag_dyneither(_tmp9B3,
sizeof(char),2))),((_tmp9B2="]",_tag_dyneither(_tmp9B2,sizeof(char),2))),((
_tmp9B1=",",_tag_dyneither(_tmp9B1,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp475))),((_tmp9AE[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp9AE,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL25D;}_LL27E: {struct Cyc_Absyn_Enum_p_struct*_tmp478=(struct Cyc_Absyn_Enum_p_struct*)
_tmp442;if(_tmp478->tag != 12)goto _LL280;else{_tmp479=_tmp478->f2;}}_LL27F:
_tmp47B=_tmp479;goto _LL281;_LL280: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp47A=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp442;if(_tmp47A->tag != 13)goto _LL282;else{
_tmp47B=_tmp47A->f2;}}_LL281: s=Cyc_Absynpp_qvar2doc(_tmp47B->name);goto _LL25D;
_LL282: {struct Cyc_Absyn_Datatype_p_struct*_tmp47C=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp442;if(_tmp47C->tag != 7)goto _LL284;else{_tmp47D=_tmp47C->f2;_tmp47E=_tmp47C->f3;
if(_tmp47E != 0)goto _LL284;}}_LL283: s=Cyc_Absynpp_qvar2doc(_tmp47D->name);goto
_LL25D;_LL284: {struct Cyc_Absyn_Datatype_p_struct*_tmp47F=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp442;if(_tmp47F->tag != 7)goto _LL286;else{_tmp480=_tmp47F->f2;_tmp481=_tmp47F->f3;
_tmp482=_tmp47F->f4;}}_LL285: {const char*_tmp9B5;const char*_tmp9B4;struct
_dyneither_ptr term=_tmp482?(_tmp9B5=", ...)",_tag_dyneither(_tmp9B5,sizeof(char),
7)):((_tmp9B4=")",_tag_dyneither(_tmp9B4,sizeof(char),2)));{const char*_tmp9BA;
const char*_tmp9B9;struct Cyc_PP_Doc*_tmp9B8[2];s=((_tmp9B8[1]=Cyc_PP_egroup(((
_tmp9BA="(",_tag_dyneither(_tmp9BA,sizeof(char),2))),term,((_tmp9B9=",",
_tag_dyneither(_tmp9B9,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp481)),((_tmp9B8[0]=Cyc_Absynpp_qvar2doc(_tmp480->name),Cyc_PP_cat(
_tag_dyneither(_tmp9B8,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL25D;}_LL286: {
struct Cyc_Absyn_Exp_p_struct*_tmp483=(struct Cyc_Absyn_Exp_p_struct*)_tmp442;if(
_tmp483->tag != 16)goto _LL25D;else{_tmp484=_tmp483->f1;}}_LL287: s=Cyc_Absynpp_exp2doc(
_tmp484);goto _LL25D;_LL25D:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple14*dp){const char*_tmp9C1;const char*_tmp9C0;const char*_tmp9BF;struct Cyc_PP_Doc*
_tmp9BE[2];return(_tmp9BE[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp9BE[0]=Cyc_PP_egroup(((
_tmp9C1="",_tag_dyneither(_tmp9C1,sizeof(char),1))),((_tmp9C0="=",_tag_dyneither(
_tmp9C0,sizeof(char),2))),((_tmp9BF="=",_tag_dyneither(_tmp9BF,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp9BE,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (c->pattern)->r == (void*)&
Cyc_Absyn_Wild_p_val){const char*_tmp9C6;struct Cyc_PP_Doc*_tmp9C5[2];struct Cyc_PP_Doc*
_tmp9C4[2];return(_tmp9C4[1]=Cyc_PP_nest(2,((_tmp9C5[1]=Cyc_Absynpp_stmt2doc(c->body,
0),((_tmp9C5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9C5,sizeof(
struct Cyc_PP_Doc*),2))))))),((_tmp9C4[0]=Cyc_PP_text(((_tmp9C6="default: ",
_tag_dyneither(_tmp9C6,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp9C4,
sizeof(struct Cyc_PP_Doc*),2)))));}else{if(c->where_clause == 0){const char*_tmp9CD;
const char*_tmp9CC;struct Cyc_PP_Doc*_tmp9CB[2];struct Cyc_PP_Doc*_tmp9CA[4];return(
_tmp9CA[3]=Cyc_PP_nest(2,((_tmp9CB[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9CB[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9CB,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp9CA[2]=Cyc_PP_text(((_tmp9CC=": ",_tag_dyneither(_tmp9CC,sizeof(
char),3)))),((_tmp9CA[1]=Cyc_Absynpp_pat2doc(c->pattern),((_tmp9CA[0]=Cyc_PP_text(((
_tmp9CD="case ",_tag_dyneither(_tmp9CD,sizeof(char),6)))),Cyc_PP_cat(
_tag_dyneither(_tmp9CA,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{const char*
_tmp9D6;const char*_tmp9D5;const char*_tmp9D4;struct Cyc_PP_Doc*_tmp9D3[2];struct
Cyc_PP_Doc*_tmp9D2[6];return(_tmp9D2[5]=Cyc_PP_nest(2,((_tmp9D3[1]=Cyc_Absynpp_stmt2doc(
c->body,0),((_tmp9D3[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9D3,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9D2[4]=Cyc_PP_text(((_tmp9D4=": ",
_tag_dyneither(_tmp9D4,sizeof(char),3)))),((_tmp9D2[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp9D2[2]=Cyc_PP_text(((
_tmp9D5=" && ",_tag_dyneither(_tmp9D5,sizeof(char),5)))),((_tmp9D2[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp9D2[0]=Cyc_PP_text(((_tmp9D6="case ",_tag_dyneither(_tmp9D6,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9D2,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*
_tmp9D7;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp9D7="",_tag_dyneither(_tmp9D7,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);
else{const char*_tmp9DA;struct Cyc_PP_Doc*_tmp9D9[3];return(_tmp9D9[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp9D9[1]=Cyc_PP_text(((_tmp9DA=" = ",
_tag_dyneither(_tmp9DA,sizeof(char),4)))),((_tmp9D9[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp9D9,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){const char*_tmp9DB;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((
_tmp9DB=",",_tag_dyneither(_tmp9DB,sizeof(char),2))),fs);}static struct Cyc_PP_Doc*
Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(v->name);}
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){const char*
_tmp9DC;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((
_tmp9DC=",",_tag_dyneither(_tmp9DC,sizeof(char),2))),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(
struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl _tmp4D3;enum Cyc_Absyn_Scope
_tmp4D4;struct _tuple0*_tmp4D5;struct Cyc_Absyn_Tqual _tmp4D6;void*_tmp4D7;struct
Cyc_Absyn_Exp*_tmp4D8;struct Cyc_List_List*_tmp4D9;struct Cyc_Absyn_Vardecl*
_tmp4D2=vd;_tmp4D3=*_tmp4D2;_tmp4D4=_tmp4D3.sc;_tmp4D5=_tmp4D3.name;_tmp4D6=
_tmp4D3.tq;_tmp4D7=_tmp4D3.type;_tmp4D8=_tmp4D3.initializer;_tmp4D9=_tmp4D3.attributes;{
struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp4D5);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4D9);struct Cyc_PP_Doc*
beforenamedoc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL288:
beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL289:{void*_tmp4DA=Cyc_Tcutil_compress(
_tmp4D7);struct Cyc_Absyn_FnInfo _tmp4DC;struct Cyc_List_List*_tmp4DD;_LL28C: {
struct Cyc_Absyn_FnType_struct*_tmp4DB=(struct Cyc_Absyn_FnType_struct*)_tmp4DA;
if(_tmp4DB->tag != 9)goto _LL28E;else{_tmp4DC=_tmp4DB->f1;_tmp4DD=_tmp4DC.attributes;}}
_LL28D: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4DD);goto _LL28B;_LL28E:;_LL28F:
beforenamedoc=Cyc_PP_nil_doc();goto _LL28B;_LL28B:;}break;}{struct Cyc_PP_Doc*
tmp_doc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL290: tmp_doc=Cyc_PP_nil_doc();
break;case Cyc_Cyclone_Vc_c: _LL291: tmp_doc=attsdoc;break;}{struct Cyc_Core_Opt*
_tmp9E9;struct Cyc_PP_Doc*_tmp9E8[2];struct Cyc_PP_Doc*_tmp9E7[2];const char*
_tmp9E6;const char*_tmp9E5;struct Cyc_PP_Doc*_tmp9E4[5];s=((_tmp9E4[4]=Cyc_PP_text(((
_tmp9E5=";",_tag_dyneither(_tmp9E5,sizeof(char),2)))),((_tmp9E4[3]=_tmp4D8 == 0?
Cyc_PP_nil_doc():((_tmp9E7[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4D8),((
_tmp9E7[0]=Cyc_PP_text(((_tmp9E6=" = ",_tag_dyneither(_tmp9E6,sizeof(char),4)))),
Cyc_PP_cat(_tag_dyneither(_tmp9E7,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9E4[2]=
Cyc_Absynpp_tqtd2doc(_tmp4D6,_tmp4D7,((_tmp9E9=_cycalloc(sizeof(*_tmp9E9)),((
_tmp9E9->v=((_tmp9E8[1]=sn,((_tmp9E8[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(
_tmp9E8,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9E9))))),((_tmp9E4[1]=Cyc_Absynpp_scope2doc(
_tmp4D4),((_tmp9E4[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp9E4,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}return s;};};}struct _tuple18{unsigned int f1;struct _tuple0*f2;int f3;
};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple18*x){struct _tuple0*
_tmp4E5;struct _tuple18 _tmp4E4=*x;_tmp4E5=_tmp4E4.f2;return Cyc_Absynpp_qvar2doc(
_tmp4E5);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){struct Cyc_PP_Doc*_tmp9EA[4];return(_tmp9EA[3]=Cyc_Absynpp_ktvars2doc(
ad->tvs),((_tmp9EA[2]=Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9EA[1]=Cyc_Absynpp_aggr_kind2doc(
ad->kind),((_tmp9EA[0]=Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(
_tmp9EA,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{struct Cyc_PP_Doc*_tmp9F2[2];
const char*_tmp9F1;struct Cyc_PP_Doc*_tmp9F0[2];struct Cyc_PP_Doc*_tmp9EF[12];
return(_tmp9EF[11]=Cyc_Absynpp_atts2doc(ad->attributes),((_tmp9EF[10]=Cyc_Absynpp_rb(),((
_tmp9EF[9]=Cyc_PP_line_doc(),((_tmp9EF[8]=Cyc_PP_nest(2,((_tmp9F0[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmp9F0[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp9F0,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9EF[7]=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():((
_tmp9F2[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((
_tmp9F2[0]=Cyc_PP_text(((_tmp9F1=":",_tag_dyneither(_tmp9F1,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp9F2,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9EF[6]=
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((
_tmp9EF[5]=Cyc_Absynpp_lb(),((_tmp9EF[4]=Cyc_PP_blank_doc(),((_tmp9EF[3]=Cyc_Absynpp_ktvars2doc(
ad->tvs),((_tmp9EF[2]=Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9EF[1]=Cyc_Absynpp_aggr_kind2doc(
ad->kind),((_tmp9EF[0]=Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(
_tmp9EF,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){struct Cyc_Absyn_Datatypedecl
_tmp4EC;enum Cyc_Absyn_Scope _tmp4ED;struct _tuple0*_tmp4EE;struct Cyc_List_List*
_tmp4EF;struct Cyc_Core_Opt*_tmp4F0;int _tmp4F1;struct Cyc_Absyn_Datatypedecl*
_tmp4EB=dd;_tmp4EC=*_tmp4EB;_tmp4ED=_tmp4EC.sc;_tmp4EE=_tmp4EC.name;_tmp4EF=
_tmp4EC.tvs;_tmp4F0=_tmp4EC.fields;_tmp4F1=_tmp4EC.is_extensible;if(_tmp4F0 == 0){
const char*_tmp9F7;const char*_tmp9F6;struct Cyc_PP_Doc*_tmp9F5[5];return(_tmp9F5[4]=
Cyc_Absynpp_ktvars2doc(_tmp4EF),((_tmp9F5[3]=_tmp4F1?Cyc_Absynpp_qvar2bolddoc(
_tmp4EE): Cyc_Absynpp_typedef_name2bolddoc(_tmp4EE),((_tmp9F5[2]=Cyc_PP_text(((
_tmp9F6="datatype ",_tag_dyneither(_tmp9F6,sizeof(char),10)))),((_tmp9F5[1]=
_tmp4F1?Cyc_PP_text(((_tmp9F7="@extensible ",_tag_dyneither(_tmp9F7,sizeof(char),
13)))): Cyc_PP_blank_doc(),((_tmp9F5[0]=Cyc_Absynpp_scope2doc(_tmp4ED),Cyc_PP_cat(
_tag_dyneither(_tmp9F5,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{const char*
_tmp9FE;const char*_tmp9FD;struct Cyc_PP_Doc*_tmp9FC[2];struct Cyc_PP_Doc*_tmp9FB[
10];return(_tmp9FB[9]=Cyc_Absynpp_rb(),((_tmp9FB[8]=Cyc_PP_line_doc(),((_tmp9FB[
7]=Cyc_PP_nest(2,((_tmp9FC[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)
_tmp4F0->v),((_tmp9FC[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9FC,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9FB[6]=Cyc_Absynpp_lb(),((_tmp9FB[5]=
Cyc_PP_blank_doc(),((_tmp9FB[4]=Cyc_Absynpp_ktvars2doc(_tmp4EF),((_tmp9FB[3]=
_tmp4F1?Cyc_Absynpp_qvar2bolddoc(_tmp4EE): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4EE),((_tmp9FB[2]=Cyc_PP_text(((_tmp9FD="datatype ",_tag_dyneither(_tmp9FD,
sizeof(char),10)))),((_tmp9FB[1]=_tmp4F1?Cyc_PP_text(((_tmp9FE="@extensible ",
_tag_dyneither(_tmp9FE,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9FB[0]=Cyc_Absynpp_scope2doc(
_tmp4ED),Cyc_PP_cat(_tag_dyneither(_tmp9FB,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){struct Cyc_Absyn_Enumdecl
_tmp4FA;enum Cyc_Absyn_Scope _tmp4FB;struct _tuple0*_tmp4FC;struct Cyc_Core_Opt*
_tmp4FD;struct Cyc_Absyn_Enumdecl*_tmp4F9=ed;_tmp4FA=*_tmp4F9;_tmp4FB=_tmp4FA.sc;
_tmp4FC=_tmp4FA.name;_tmp4FD=_tmp4FA.fields;if(_tmp4FD == 0){const char*_tmpA01;
struct Cyc_PP_Doc*_tmpA00[3];return(_tmpA00[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp4FC),((_tmpA00[1]=Cyc_PP_text(((_tmpA01="enum ",_tag_dyneither(_tmpA01,
sizeof(char),6)))),((_tmpA00[0]=Cyc_Absynpp_scope2doc(_tmp4FB),Cyc_PP_cat(
_tag_dyneither(_tmpA00,sizeof(struct Cyc_PP_Doc*),3)))))));}else{const char*
_tmpA06;struct Cyc_PP_Doc*_tmpA05[2];struct Cyc_PP_Doc*_tmpA04[8];return(_tmpA04[7]=
Cyc_Absynpp_rb(),((_tmpA04[6]=Cyc_PP_line_doc(),((_tmpA04[5]=Cyc_PP_nest(2,((
_tmpA05[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp4FD->v),((
_tmpA05[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA05,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmpA04[4]=Cyc_Absynpp_lb(),((_tmpA04[3]=Cyc_PP_blank_doc(),((_tmpA04[
2]=Cyc_Absynpp_qvar2bolddoc(_tmp4FC),((_tmpA04[1]=Cyc_PP_text(((_tmpA06="enum ",
_tag_dyneither(_tmpA06,sizeof(char),6)))),((_tmpA04[0]=Cyc_Absynpp_scope2doc(
_tmp4FB),Cyc_PP_cat(_tag_dyneither(_tmpA04,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{
void*_tmp503=d->r;struct Cyc_Absyn_Fndecl*_tmp505;struct Cyc_Absyn_Aggrdecl*
_tmp507;struct Cyc_Absyn_Vardecl*_tmp509;struct Cyc_Absyn_Tvar*_tmp50B;struct Cyc_Absyn_Vardecl*
_tmp50C;int _tmp50D;struct Cyc_Absyn_Exp*_tmp50E;struct Cyc_Absyn_Exp*_tmp510;
struct Cyc_Absyn_Tvar*_tmp511;struct Cyc_Absyn_Vardecl*_tmp512;struct Cyc_Absyn_Datatypedecl*
_tmp514;struct Cyc_Absyn_Pat*_tmp516;struct Cyc_Absyn_Exp*_tmp517;struct Cyc_List_List*
_tmp519;struct Cyc_Absyn_Enumdecl*_tmp51B;struct Cyc_Absyn_Typedefdecl*_tmp51D;
struct _dyneither_ptr*_tmp51F;struct Cyc_List_List*_tmp520;struct _tuple0*_tmp522;
struct Cyc_List_List*_tmp523;struct Cyc_List_List*_tmp525;struct Cyc_List_List*
_tmp527;struct Cyc_List_List*_tmp528;_LL294: {struct Cyc_Absyn_Fn_d_struct*_tmp504=(
struct Cyc_Absyn_Fn_d_struct*)_tmp503;if(_tmp504->tag != 1)goto _LL296;else{_tmp505=
_tmp504->f1;}}_LL295: {struct Cyc_Absyn_FnType_struct _tmpA0C;struct Cyc_Absyn_FnInfo
_tmpA0B;struct Cyc_Absyn_FnType_struct*_tmpA0A;void*t=(void*)((_tmpA0A=_cycalloc(
sizeof(*_tmpA0A)),((_tmpA0A[0]=((_tmpA0C.tag=9,((_tmpA0C.f1=((_tmpA0B.tvars=
_tmp505->tvs,((_tmpA0B.effect=_tmp505->effect,((_tmpA0B.ret_tqual=_tmp505->ret_tqual,((
_tmpA0B.ret_typ=_tmp505->ret_type,((_tmpA0B.args=((struct Cyc_List_List*(*)(
struct _tuple8*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp505->args),((_tmpA0B.c_varargs=_tmp505->c_varargs,((_tmpA0B.cyc_varargs=
_tmp505->cyc_varargs,((_tmpA0B.rgn_po=_tmp505->rgn_po,((_tmpA0B.attributes=0,
_tmpA0B)))))))))))))))))),_tmpA0C)))),_tmpA0A))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp505->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp505->is_inline)switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL2B4:{const char*_tmpA0D;inlinedoc=Cyc_PP_text(((_tmpA0D="inline ",
_tag_dyneither(_tmpA0D,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2B5:{
const char*_tmpA0E;inlinedoc=Cyc_PP_text(((_tmpA0E="__inline ",_tag_dyneither(
_tmpA0E,sizeof(char),10))));}break;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc(_tmp505->sc);struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2B7: beforenamedoc=
attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2B8: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp505->attributes);break;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp505->name);struct Cyc_PP_Doc*_tmpA11[2];struct Cyc_Core_Opt*_tmpA10;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmpA10=_cycalloc(
sizeof(*_tmpA10)),((_tmpA10->v=((_tmpA11[1]=namedoc,((_tmpA11[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmpA11,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA10)))));
struct Cyc_PP_Doc*_tmpA14[2];struct Cyc_PP_Doc*_tmpA13[5];struct Cyc_PP_Doc*bodydoc=(
_tmpA13[4]=Cyc_Absynpp_rb(),((_tmpA13[3]=Cyc_PP_line_doc(),((_tmpA13[2]=Cyc_PP_nest(
2,((_tmpA14[1]=Cyc_Absynpp_stmt2doc(_tmp505->body,0),((_tmpA14[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmpA14,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA13[1]=
Cyc_Absynpp_lb(),((_tmpA13[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmpA13,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmpA15[4];s=((
_tmpA15[3]=bodydoc,((_tmpA15[2]=tqtddoc,((_tmpA15[1]=scopedoc,((_tmpA15[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmpA15,sizeof(struct Cyc_PP_Doc*),4))))))))));}
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2BA:{struct Cyc_PP_Doc*
_tmpA16[2];s=((_tmpA16[1]=s,((_tmpA16[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(
_tmpA16,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2BB: break;}goto
_LL293;};};}_LL296: {struct Cyc_Absyn_Aggr_d_struct*_tmp506=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp503;if(_tmp506->tag != 6)goto _LL298;else{_tmp507=_tmp506->f1;}}_LL297:{const
char*_tmpA19;struct Cyc_PP_Doc*_tmpA18[2];s=((_tmpA18[1]=Cyc_PP_text(((_tmpA19=";",
_tag_dyneither(_tmpA19,sizeof(char),2)))),((_tmpA18[0]=Cyc_Absynpp_aggrdecl2doc(
_tmp507),Cyc_PP_cat(_tag_dyneither(_tmpA18,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL293;_LL298: {struct Cyc_Absyn_Var_d_struct*_tmp508=(struct Cyc_Absyn_Var_d_struct*)
_tmp503;if(_tmp508->tag != 0)goto _LL29A;else{_tmp509=_tmp508->f1;}}_LL299: s=Cyc_Absynpp_vardecl2doc(
_tmp509);goto _LL293;_LL29A: {struct Cyc_Absyn_Region_d_struct*_tmp50A=(struct Cyc_Absyn_Region_d_struct*)
_tmp503;if(_tmp50A->tag != 4)goto _LL29C;else{_tmp50B=_tmp50A->f1;_tmp50C=_tmp50A->f2;
_tmp50D=_tmp50A->f3;_tmp50E=_tmp50A->f4;}}_LL29B:{const char*_tmpA2C;const char*
_tmpA2B;const char*_tmpA2A;const char*_tmpA29;struct Cyc_PP_Doc*_tmpA28[3];const
char*_tmpA27;const char*_tmpA26;const char*_tmpA25;struct Cyc_PP_Doc*_tmpA24[8];s=((
_tmpA24[7]=Cyc_PP_text(((_tmpA25=";",_tag_dyneither(_tmpA25,sizeof(char),2)))),((
_tmpA24[6]=_tmp50E != 0?(_tmpA28[2]=Cyc_PP_text(((_tmpA27=")",_tag_dyneither(
_tmpA27,sizeof(char),2)))),((_tmpA28[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp50E),((_tmpA28[0]=Cyc_PP_text(((_tmpA26=" = open(",_tag_dyneither(_tmpA26,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA28,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmpA24[5]=_tmp50D?Cyc_PP_text(((_tmpA29=" @resetable",
_tag_dyneither(_tmpA29,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmpA24[4]=Cyc_Absynpp_qvar2doc(
_tmp50C->name),((_tmpA24[3]=Cyc_PP_text(((_tmpA2A=">",_tag_dyneither(_tmpA2A,
sizeof(char),2)))),((_tmpA24[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp50B)),((
_tmpA24[1]=Cyc_PP_text(((_tmpA2B="<",_tag_dyneither(_tmpA2B,sizeof(char),2)))),((
_tmpA24[0]=Cyc_PP_text(((_tmpA2C="region",_tag_dyneither(_tmpA2C,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmpA24,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL293;_LL29C: {struct Cyc_Absyn_Alias_d_struct*_tmp50F=(struct Cyc_Absyn_Alias_d_struct*)
_tmp503;if(_tmp50F->tag != 5)goto _LL29E;else{_tmp510=_tmp50F->f1;_tmp511=_tmp50F->f2;
_tmp512=_tmp50F->f3;}}_LL29D:{const char*_tmpA37;const char*_tmpA36;const char*
_tmpA35;const char*_tmpA34;const char*_tmpA33;struct Cyc_PP_Doc*_tmpA32[8];s=((
_tmpA32[7]=Cyc_PP_text(((_tmpA33=";",_tag_dyneither(_tmpA33,sizeof(char),2)))),((
_tmpA32[6]=Cyc_Absynpp_exp2doc(_tmp510),((_tmpA32[5]=Cyc_PP_text(((_tmpA34=" = ",
_tag_dyneither(_tmpA34,sizeof(char),4)))),((_tmpA32[4]=Cyc_Absynpp_qvar2doc(
_tmp512->name),((_tmpA32[3]=Cyc_PP_text(((_tmpA35="> ",_tag_dyneither(_tmpA35,
sizeof(char),3)))),((_tmpA32[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp511)),((
_tmpA32[1]=Cyc_PP_text(((_tmpA36=" <",_tag_dyneither(_tmpA36,sizeof(char),3)))),((
_tmpA32[0]=Cyc_PP_text(((_tmpA37="alias",_tag_dyneither(_tmpA37,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmpA32,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL293;_LL29E: {struct Cyc_Absyn_Datatype_d_struct*_tmp513=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp503;if(_tmp513->tag != 7)goto _LL2A0;else{_tmp514=_tmp513->f1;}}_LL29F:{const
char*_tmpA3A;struct Cyc_PP_Doc*_tmpA39[2];s=((_tmpA39[1]=Cyc_PP_text(((_tmpA3A=";",
_tag_dyneither(_tmpA3A,sizeof(char),2)))),((_tmpA39[0]=Cyc_Absynpp_datatypedecl2doc(
_tmp514),Cyc_PP_cat(_tag_dyneither(_tmpA39,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL293;_LL2A0: {struct Cyc_Absyn_Let_d_struct*_tmp515=(struct Cyc_Absyn_Let_d_struct*)
_tmp503;if(_tmp515->tag != 2)goto _LL2A2;else{_tmp516=_tmp515->f1;_tmp517=_tmp515->f3;}}
_LL2A1:{const char*_tmpA41;const char*_tmpA40;const char*_tmpA3F;struct Cyc_PP_Doc*
_tmpA3E[5];s=((_tmpA3E[4]=Cyc_PP_text(((_tmpA3F=";",_tag_dyneither(_tmpA3F,
sizeof(char),2)))),((_tmpA3E[3]=Cyc_Absynpp_exp2doc(_tmp517),((_tmpA3E[2]=Cyc_PP_text(((
_tmpA40=" = ",_tag_dyneither(_tmpA40,sizeof(char),4)))),((_tmpA3E[1]=Cyc_Absynpp_pat2doc(
_tmp516),((_tmpA3E[0]=Cyc_PP_text(((_tmpA41="let ",_tag_dyneither(_tmpA41,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA3E,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL293;_LL2A2: {struct Cyc_Absyn_Letv_d_struct*_tmp518=(struct Cyc_Absyn_Letv_d_struct*)
_tmp503;if(_tmp518->tag != 3)goto _LL2A4;else{_tmp519=_tmp518->f1;}}_LL2A3:{const
char*_tmpA46;const char*_tmpA45;struct Cyc_PP_Doc*_tmpA44[3];s=((_tmpA44[2]=Cyc_PP_text(((
_tmpA45=";",_tag_dyneither(_tmpA45,sizeof(char),2)))),((_tmpA44[1]=Cyc_Absynpp_ids2doc(
_tmp519),((_tmpA44[0]=Cyc_PP_text(((_tmpA46="let ",_tag_dyneither(_tmpA46,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA44,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL293;_LL2A4: {struct Cyc_Absyn_Enum_d_struct*_tmp51A=(struct Cyc_Absyn_Enum_d_struct*)
_tmp503;if(_tmp51A->tag != 8)goto _LL2A6;else{_tmp51B=_tmp51A->f1;}}_LL2A5:{const
char*_tmpA49;struct Cyc_PP_Doc*_tmpA48[2];s=((_tmpA48[1]=Cyc_PP_text(((_tmpA49=";",
_tag_dyneither(_tmpA49,sizeof(char),2)))),((_tmpA48[0]=Cyc_Absynpp_enumdecl2doc(
_tmp51B),Cyc_PP_cat(_tag_dyneither(_tmpA48,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL293;_LL2A6: {struct Cyc_Absyn_Typedef_d_struct*_tmp51C=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp503;if(_tmp51C->tag != 9)goto _LL2A8;else{_tmp51D=_tmp51C->f1;}}_LL2A7: {void*
t;if(_tmp51D->defn != 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp51D->defn))->v;
else{t=Cyc_Absyn_new_evar(_tmp51D->kind,0);}{const char*_tmpA53;struct Cyc_Core_Opt*
_tmpA52;struct Cyc_PP_Doc*_tmpA51[2];const char*_tmpA50;struct Cyc_PP_Doc*_tmpA4F[4];
s=((_tmpA4F[3]=Cyc_PP_text(((_tmpA50=";",_tag_dyneither(_tmpA50,sizeof(char),2)))),((
_tmpA4F[2]=Cyc_Absynpp_atts2doc(_tmp51D->atts),((_tmpA4F[1]=Cyc_Absynpp_tqtd2doc(
_tmp51D->tq,t,((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->v=((_tmpA51[1]=
Cyc_Absynpp_tvars2doc(_tmp51D->tvs),((_tmpA51[0]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp51D->name),Cyc_PP_cat(_tag_dyneither(_tmpA51,sizeof(struct Cyc_PP_Doc*),2)))))),
_tmpA52))))),((_tmpA4F[0]=Cyc_PP_text(((_tmpA53="typedef ",_tag_dyneither(
_tmpA53,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA4F,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL293;}_LL2A8: {struct Cyc_Absyn_Namespace_d_struct*_tmp51E=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp503;if(_tmp51E->tag != 10)goto _LL2AA;
else{_tmp51F=_tmp51E->f1;_tmp520=_tmp51E->f2;}}_LL2A9: if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_add(_tmp51F);{const char*_tmpA58;const char*_tmpA57;
struct Cyc_PP_Doc*_tmpA56[8];s=((_tmpA56[7]=Cyc_Absynpp_rb(),((_tmpA56[6]=Cyc_PP_line_doc(),((
_tmpA56[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA57="",_tag_dyneither(_tmpA57,sizeof(char),1))),_tmp520),((_tmpA56[4]=Cyc_PP_line_doc(),((
_tmpA56[3]=Cyc_Absynpp_lb(),((_tmpA56[2]=Cyc_PP_blank_doc(),((_tmpA56[1]=Cyc_PP_textptr(
_tmp51F),((_tmpA56[0]=Cyc_PP_text(((_tmpA58="namespace ",_tag_dyneither(_tmpA58,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA56,sizeof(struct Cyc_PP_Doc*),
8))))))))))))))))));}if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL293;_LL2AA: {struct Cyc_Absyn_Using_d_struct*_tmp521=(struct Cyc_Absyn_Using_d_struct*)
_tmp503;if(_tmp521->tag != 11)goto _LL2AC;else{_tmp522=_tmp521->f1;_tmp523=_tmp521->f2;}}
_LL2AB: if(Cyc_Absynpp_print_using_stmts){const char*_tmpA5D;const char*_tmpA5C;
struct Cyc_PP_Doc*_tmpA5B[8];s=((_tmpA5B[7]=Cyc_Absynpp_rb(),((_tmpA5B[6]=Cyc_PP_line_doc(),((
_tmpA5B[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA5C="",_tag_dyneither(_tmpA5C,sizeof(char),1))),_tmp523),((_tmpA5B[4]=Cyc_PP_line_doc(),((
_tmpA5B[3]=Cyc_Absynpp_lb(),((_tmpA5B[2]=Cyc_PP_blank_doc(),((_tmpA5B[1]=Cyc_Absynpp_qvar2doc(
_tmp522),((_tmpA5B[0]=Cyc_PP_text(((_tmpA5D="using ",_tag_dyneither(_tmpA5D,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA5B,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmpA68;const char*_tmpA67;const char*_tmpA66;const char*_tmpA65;
const char*_tmpA64;struct Cyc_PP_Doc*_tmpA63[11];s=((_tmpA63[10]=Cyc_PP_text(((
_tmpA64=" */",_tag_dyneither(_tmpA64,sizeof(char),4)))),((_tmpA63[9]=Cyc_Absynpp_rb(),((
_tmpA63[8]=Cyc_PP_text(((_tmpA65="/* ",_tag_dyneither(_tmpA65,sizeof(char),4)))),((
_tmpA63[7]=Cyc_PP_line_doc(),((_tmpA63[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA66="",_tag_dyneither(_tmpA66,sizeof(char),1))),
_tmp523),((_tmpA63[5]=Cyc_PP_line_doc(),((_tmpA63[4]=Cyc_PP_text(((_tmpA67=" */",
_tag_dyneither(_tmpA67,sizeof(char),4)))),((_tmpA63[3]=Cyc_Absynpp_lb(),((
_tmpA63[2]=Cyc_PP_blank_doc(),((_tmpA63[1]=Cyc_Absynpp_qvar2doc(_tmp522),((
_tmpA63[0]=Cyc_PP_text(((_tmpA68="/* using ",_tag_dyneither(_tmpA68,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmpA63,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL293;_LL2AC: {struct Cyc_Absyn_ExternC_d_struct*_tmp524=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp503;if(_tmp524->tag != 12)goto _LL2AE;else{_tmp525=_tmp524->f1;}}_LL2AD: if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA6D;const char*_tmpA6C;struct Cyc_PP_Doc*_tmpA6B[6];s=((_tmpA6B[5]=
Cyc_Absynpp_rb(),((_tmpA6B[4]=Cyc_PP_line_doc(),((_tmpA6B[3]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA6C="",_tag_dyneither(_tmpA6C,
sizeof(char),1))),_tmp525),((_tmpA6B[2]=Cyc_PP_line_doc(),((_tmpA6B[1]=Cyc_Absynpp_lb(),((
_tmpA6B[0]=Cyc_PP_text(((_tmpA6D="extern \"C\" ",_tag_dyneither(_tmpA6D,sizeof(
char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA6B,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmpA78;const char*_tmpA77;const char*_tmpA76;const char*_tmpA75;
const char*_tmpA74;struct Cyc_PP_Doc*_tmpA73[9];s=((_tmpA73[8]=Cyc_PP_text(((
_tmpA74=" */",_tag_dyneither(_tmpA74,sizeof(char),4)))),((_tmpA73[7]=Cyc_Absynpp_rb(),((
_tmpA73[6]=Cyc_PP_text(((_tmpA75="/* ",_tag_dyneither(_tmpA75,sizeof(char),4)))),((
_tmpA73[5]=Cyc_PP_line_doc(),((_tmpA73[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA76="",_tag_dyneither(_tmpA76,sizeof(char),1))),
_tmp525),((_tmpA73[3]=Cyc_PP_line_doc(),((_tmpA73[2]=Cyc_PP_text(((_tmpA77=" */",
_tag_dyneither(_tmpA77,sizeof(char),4)))),((_tmpA73[1]=Cyc_Absynpp_lb(),((
_tmpA73[0]=Cyc_PP_text(((_tmpA78="/* extern \"C\" ",_tag_dyneither(_tmpA78,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA73,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL293;_LL2AE: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp526=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp503;if(_tmp526->tag != 13)
goto _LL2B0;else{_tmp527=_tmp526->f1;_tmp528=_tmp526->f2;}}_LL2AF: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp528 != 0){const char*_tmpA7D;const char*_tmpA7C;
struct Cyc_PP_Doc*_tmpA7B[7];exs_doc=((_tmpA7B[6]=Cyc_Absynpp_rb(),((_tmpA7B[5]=
Cyc_PP_line_doc(),((_tmpA7B[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple18*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmpA7C=",",_tag_dyneither(_tmpA7C,sizeof(char),2))),
_tmp528),((_tmpA7B[3]=Cyc_PP_line_doc(),((_tmpA7B[2]=Cyc_Absynpp_lb(),((_tmpA7B[
1]=Cyc_PP_text(((_tmpA7D=" export ",_tag_dyneither(_tmpA7D,sizeof(char),9)))),((
_tmpA7B[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA7B,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmpA82;const char*
_tmpA81;struct Cyc_PP_Doc*_tmpA80[6];s=((_tmpA80[5]=exs_doc,((_tmpA80[4]=Cyc_PP_line_doc(),((
_tmpA80[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA81="",_tag_dyneither(_tmpA81,sizeof(char),1))),_tmp527),((_tmpA80[2]=Cyc_PP_line_doc(),((
_tmpA80[1]=Cyc_Absynpp_lb(),((_tmpA80[0]=Cyc_PP_text(((_tmpA82="extern \"C include\" ",
_tag_dyneither(_tmpA82,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA80,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{const char*_tmpA8D;const char*
_tmpA8C;const char*_tmpA8B;const char*_tmpA8A;const char*_tmpA89;struct Cyc_PP_Doc*
_tmpA88[9];s=((_tmpA88[8]=Cyc_PP_text(((_tmpA89=" */",_tag_dyneither(_tmpA89,
sizeof(char),4)))),((_tmpA88[7]=Cyc_Absynpp_rb(),((_tmpA88[6]=Cyc_PP_text(((
_tmpA8A="/* ",_tag_dyneither(_tmpA8A,sizeof(char),4)))),((_tmpA88[5]=Cyc_PP_line_doc(),((
_tmpA88[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA8B="",_tag_dyneither(_tmpA8B,sizeof(char),1))),_tmp527),((_tmpA88[3]=Cyc_PP_line_doc(),((
_tmpA88[2]=Cyc_PP_text(((_tmpA8C=" */",_tag_dyneither(_tmpA8C,sizeof(char),4)))),((
_tmpA88[1]=Cyc_Absynpp_lb(),((_tmpA88[0]=Cyc_PP_text(((_tmpA8D="/* extern \"C include\" ",
_tag_dyneither(_tmpA8D,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA88,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL293;_LL2B0: {struct Cyc_Absyn_Porton_d_struct*
_tmp529=(struct Cyc_Absyn_Porton_d_struct*)_tmp503;if(_tmp529->tag != 14)goto
_LL2B2;}_LL2B1:{const char*_tmpA90;struct Cyc_PP_Doc*_tmpA8F[2];s=((_tmpA8F[1]=Cyc_Absynpp_lb(),((
_tmpA8F[0]=Cyc_PP_text(((_tmpA90="__cyclone_port_on__;",_tag_dyneither(_tmpA90,
sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA8F,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL293;_LL2B2: {struct Cyc_Absyn_Portoff_d_struct*_tmp52A=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp503;if(_tmp52A->tag != 15)goto _LL293;}_LL2B3:{const char*_tmpA93;struct Cyc_PP_Doc*
_tmpA92[2];s=((_tmpA92[1]=Cyc_Absynpp_lb(),((_tmpA92[0]=Cyc_PP_text(((_tmpA93="__cyclone_port_off__;",
_tag_dyneither(_tmpA93,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA92,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL293;_LL293:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){if(Cyc_Absynpp_print_for_cycdoc)
return Cyc_PP_nil_doc();switch(sc){case Cyc_Absyn_Static: _LL2BD: {const char*
_tmpA94;return Cyc_PP_text(((_tmpA94="static ",_tag_dyneither(_tmpA94,sizeof(char),
8))));}case Cyc_Absyn_Public: _LL2BE: return Cyc_PP_nil_doc();case Cyc_Absyn_Extern:
_LL2BF: {const char*_tmpA95;return Cyc_PP_text(((_tmpA95="extern ",_tag_dyneither(
_tmpA95,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2C0: {const char*_tmpA96;
return Cyc_PP_text(((_tmpA96="extern \"C\" ",_tag_dyneither(_tmpA96,sizeof(char),
12))));}case Cyc_Absyn_Abstract: _LL2C1: {const char*_tmpA97;return Cyc_PP_text(((
_tmpA97="abstract ",_tag_dyneither(_tmpA97,sizeof(char),10))));}case Cyc_Absyn_Register:
_LL2C2: {const char*_tmpA98;return Cyc_PP_text(((_tmpA98="register ",
_tag_dyneither(_tmpA98,sizeof(char),10))));}}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp581=t;struct Cyc_Absyn_Tvar*_tmp583;struct Cyc_List_List*_tmp585;
_LL2C5: {struct Cyc_Absyn_VarType_struct*_tmp582=(struct Cyc_Absyn_VarType_struct*)
_tmp581;if(_tmp582->tag != 2)goto _LL2C7;else{_tmp583=_tmp582->f1;}}_LL2C6: return
Cyc_Tcutil_is_temp_tvar(_tmp583);_LL2C7: {struct Cyc_Absyn_JoinEff_struct*_tmp584=(
struct Cyc_Absyn_JoinEff_struct*)_tmp581;if(_tmp584->tag != 24)goto _LL2C9;else{
_tmp585=_tmp584->f1;}}_LL2C8: return((int(*)(int(*pred)(void*),struct Cyc_List_List*
x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp585);_LL2C9:;
_LL2CA: return 0;_LL2C4:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp586=t;
struct Cyc_Absyn_Typedefdecl*_tmp58A;void**_tmp58B;void*_tmp58C;_LL2CC: {struct
Cyc_Absyn_AnonAggrType_struct*_tmp587=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp586;if(_tmp587->tag != 12)goto _LL2CE;}_LL2CD: return 1;_LL2CE: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp588=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp586;if(_tmp588->tag != 14)goto
_LL2D0;}_LL2CF: return 1;_LL2D0: {struct Cyc_Absyn_TypedefType_struct*_tmp589=(
struct Cyc_Absyn_TypedefType_struct*)_tmp586;if(_tmp589->tag != 17)goto _LL2D2;
else{_tmp58A=_tmp589->f3;_tmp58B=_tmp589->f4;if(_tmp58B == 0)goto _LL2D2;_tmp58C=*
_tmp58B;}}_LL2D1: return Cyc_Absynpp_is_anon_aggrtype(_tmp58C);_LL2D2:;_LL2D3:
return 0;_LL2CB:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct
_RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 0){
struct _tuple17 _tmpA99;struct _tuple17 _tmp58E=(_tmpA99.f1=(void*)tms->hd,((_tmpA99.f2=(
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA99)));void*_tmp58F;
void*_tmp591;_LL2D5: _tmp58F=_tmp58E.f1;{struct Cyc_Absyn_Pointer_mod_struct*
_tmp590=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp58F;if(_tmp590->tag != 2)goto
_LL2D7;};_tmp591=_tmp58E.f2;{struct Cyc_Absyn_Function_mod_struct*_tmp592=(struct
Cyc_Absyn_Function_mod_struct*)_tmp591;if(_tmp592->tag != 3)goto _LL2D7;};_LL2D6: {
struct Cyc_List_List*_tmpA9C;struct Cyc_List_List*_tmpA9B;return(_tmpA9B=
_region_malloc(r,sizeof(*_tmpA9B)),((_tmpA9B->hd=(void*)tms->hd,((_tmpA9B->tl=((
_tmpA9C=_region_malloc(r,sizeof(*_tmpA9C)),((_tmpA9C->hd=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd,((_tmpA9C->tl=Cyc_Absynpp_bubble_attributes(r,atts,((
struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA9C)))))),_tmpA9B)))));}
_LL2D7:;_LL2D8: {struct Cyc_List_List*_tmpA9D;return(_tmpA9D=_region_malloc(r,
sizeof(*_tmpA9D)),((_tmpA9D->hd=atts,((_tmpA9D->tl=tms,_tmpA9D)))));}_LL2D4:;}
else{struct Cyc_List_List*_tmpA9E;return(_tmpA9E=_region_malloc(r,sizeof(*_tmpA9E)),((
_tmpA9E->hd=atts,((_tmpA9E->tl=tms,_tmpA9E)))));}}struct _tuple13 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){void*_tmp597=t;struct Cyc_Absyn_ArrayInfo
_tmp599;void*_tmp59A;struct Cyc_Absyn_Tqual _tmp59B;struct Cyc_Absyn_Exp*_tmp59C;
union Cyc_Absyn_Constraint*_tmp59D;unsigned int _tmp59E;struct Cyc_Absyn_PtrInfo
_tmp5A0;void*_tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;struct Cyc_Absyn_PtrAtts _tmp5A3;
struct Cyc_Absyn_FnInfo _tmp5A5;struct Cyc_List_List*_tmp5A6;struct Cyc_Core_Opt*
_tmp5A7;struct Cyc_Absyn_Tqual _tmp5A8;void*_tmp5A9;struct Cyc_List_List*_tmp5AA;
int _tmp5AB;struct Cyc_Absyn_VarargInfo*_tmp5AC;struct Cyc_List_List*_tmp5AD;struct
Cyc_List_List*_tmp5AE;struct Cyc_Core_Opt*_tmp5B0;struct Cyc_Core_Opt*_tmp5B1;int
_tmp5B2;struct _tuple0*_tmp5B4;struct Cyc_List_List*_tmp5B5;struct Cyc_Absyn_Typedefdecl*
_tmp5B6;void**_tmp5B7;_LL2DA: {struct Cyc_Absyn_ArrayType_struct*_tmp598=(struct
Cyc_Absyn_ArrayType_struct*)_tmp597;if(_tmp598->tag != 8)goto _LL2DC;else{_tmp599=
_tmp598->f1;_tmp59A=_tmp599.elt_type;_tmp59B=_tmp599.tq;_tmp59C=_tmp599.num_elts;
_tmp59D=_tmp599.zero_term;_tmp59E=_tmp599.zt_loc;}}_LL2DB: {struct Cyc_Absyn_Tqual
_tmp5B9;void*_tmp5BA;struct Cyc_List_List*_tmp5BB;struct _tuple13 _tmp5B8=Cyc_Absynpp_to_tms(
r,_tmp59B,_tmp59A);_tmp5B9=_tmp5B8.f1;_tmp5BA=_tmp5B8.f2;_tmp5BB=_tmp5B8.f3;{
void*tm;if(_tmp59C == 0){struct Cyc_Absyn_Carray_mod_struct _tmpAA1;struct Cyc_Absyn_Carray_mod_struct*
_tmpAA0;tm=(void*)((_tmpAA0=_region_malloc(r,sizeof(*_tmpAA0)),((_tmpAA0[0]=((
_tmpAA1.tag=0,((_tmpAA1.f1=_tmp59D,((_tmpAA1.f2=_tmp59E,_tmpAA1)))))),_tmpAA0))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmpAA4;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpAA3;tm=(void*)((_tmpAA3=_region_malloc(r,sizeof(*_tmpAA3)),((_tmpAA3[0]=((
_tmpAA4.tag=1,((_tmpAA4.f1=(struct Cyc_Absyn_Exp*)_tmp59C,((_tmpAA4.f2=_tmp59D,((
_tmpAA4.f3=_tmp59E,_tmpAA4)))))))),_tmpAA3))));}{struct Cyc_List_List*_tmpAA7;
struct _tuple13 _tmpAA6;return(_tmpAA6.f1=_tmp5B9,((_tmpAA6.f2=_tmp5BA,((_tmpAA6.f3=((
_tmpAA7=_region_malloc(r,sizeof(*_tmpAA7)),((_tmpAA7->hd=tm,((_tmpAA7->tl=
_tmp5BB,_tmpAA7)))))),_tmpAA6)))));};};}_LL2DC: {struct Cyc_Absyn_PointerType_struct*
_tmp59F=(struct Cyc_Absyn_PointerType_struct*)_tmp597;if(_tmp59F->tag != 5)goto
_LL2DE;else{_tmp5A0=_tmp59F->f1;_tmp5A1=_tmp5A0.elt_typ;_tmp5A2=_tmp5A0.elt_tq;
_tmp5A3=_tmp5A0.ptr_atts;}}_LL2DD: {struct Cyc_Absyn_Tqual _tmp5C3;void*_tmp5C4;
struct Cyc_List_List*_tmp5C5;struct _tuple13 _tmp5C2=Cyc_Absynpp_to_tms(r,_tmp5A2,
_tmp5A1);_tmp5C3=_tmp5C2.f1;_tmp5C4=_tmp5C2.f2;_tmp5C5=_tmp5C2.f3;{struct Cyc_Absyn_Pointer_mod_struct*
_tmpAAD;struct Cyc_Absyn_Pointer_mod_struct _tmpAAC;struct Cyc_List_List*_tmpAAB;
_tmp5C5=((_tmpAAB=_region_malloc(r,sizeof(*_tmpAAB)),((_tmpAAB->hd=(void*)((
_tmpAAD=_region_malloc(r,sizeof(*_tmpAAD)),((_tmpAAD[0]=((_tmpAAC.tag=2,((
_tmpAAC.f1=_tmp5A3,((_tmpAAC.f2=tq,_tmpAAC)))))),_tmpAAD)))),((_tmpAAB->tl=
_tmp5C5,_tmpAAB))))));}{struct _tuple13 _tmpAAE;return(_tmpAAE.f1=_tmp5C3,((
_tmpAAE.f2=_tmp5C4,((_tmpAAE.f3=_tmp5C5,_tmpAAE)))));};}_LL2DE: {struct Cyc_Absyn_FnType_struct*
_tmp5A4=(struct Cyc_Absyn_FnType_struct*)_tmp597;if(_tmp5A4->tag != 9)goto _LL2E0;
else{_tmp5A5=_tmp5A4->f1;_tmp5A6=_tmp5A5.tvars;_tmp5A7=_tmp5A5.effect;_tmp5A8=
_tmp5A5.ret_tqual;_tmp5A9=_tmp5A5.ret_typ;_tmp5AA=_tmp5A5.args;_tmp5AB=_tmp5A5.c_varargs;
_tmp5AC=_tmp5A5.cyc_varargs;_tmp5AD=_tmp5A5.rgn_po;_tmp5AE=_tmp5A5.attributes;}}
_LL2DF: if(!Cyc_Absynpp_print_all_tvars){if(_tmp5A7 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)_tmp5A7->v)){_tmp5A7=0;_tmp5A6=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar,_tmp5A6);}{struct Cyc_Absyn_Tqual _tmp5CB;void*
_tmp5CC;struct Cyc_List_List*_tmp5CD;struct _tuple13 _tmp5CA=Cyc_Absynpp_to_tms(r,
_tmp5A8,_tmp5A9);_tmp5CB=_tmp5CA.f1;_tmp5CC=_tmp5CA.f2;_tmp5CD=_tmp5CA.f3;{
struct Cyc_List_List*tms=_tmp5CD;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL2E6: if(_tmp5AE != 0){struct Cyc_Absyn_Attributes_mod_struct _tmpAB1;struct Cyc_Absyn_Attributes_mod_struct*
_tmpAB0;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpAB0=_region_malloc(r,
sizeof(*_tmpAB0)),((_tmpAB0[0]=((_tmpAB1.tag=5,((_tmpAB1.f1=0,((_tmpAB1.f2=
_tmp5AE,_tmpAB1)))))),_tmpAB0)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmpAC0;struct Cyc_Absyn_WithTypes_struct*_tmpABF;struct Cyc_Absyn_WithTypes_struct
_tmpABE;struct Cyc_Absyn_Function_mod_struct _tmpABD;struct Cyc_List_List*_tmpABC;
tms=((_tmpABC=_region_malloc(r,sizeof(*_tmpABC)),((_tmpABC->hd=(void*)((_tmpAC0=
_region_malloc(r,sizeof(*_tmpAC0)),((_tmpAC0[0]=((_tmpABD.tag=3,((_tmpABD.f1=(
void*)((void*)((_tmpABF=_region_malloc(r,sizeof(*_tmpABF)),((_tmpABF[0]=((
_tmpABE.tag=1,((_tmpABE.f1=_tmp5AA,((_tmpABE.f2=_tmp5AB,((_tmpABE.f3=_tmp5AC,((
_tmpABE.f4=_tmp5A7,((_tmpABE.f5=_tmp5AD,_tmpABE)))))))))))),_tmpABF))))),_tmpABD)))),
_tmpAC0)))),((_tmpABC->tl=tms,_tmpABC))))));}break;case Cyc_Cyclone_Vc_c: _LL2E7:{
struct Cyc_Absyn_Function_mod_struct*_tmpACF;struct Cyc_Absyn_WithTypes_struct*
_tmpACE;struct Cyc_Absyn_WithTypes_struct _tmpACD;struct Cyc_Absyn_Function_mod_struct
_tmpACC;struct Cyc_List_List*_tmpACB;tms=((_tmpACB=_region_malloc(r,sizeof(*
_tmpACB)),((_tmpACB->hd=(void*)((_tmpACF=_region_malloc(r,sizeof(*_tmpACF)),((
_tmpACF[0]=((_tmpACC.tag=3,((_tmpACC.f1=(void*)((void*)((_tmpACE=_region_malloc(
r,sizeof(*_tmpACE)),((_tmpACE[0]=((_tmpACD.tag=1,((_tmpACD.f1=_tmp5AA,((_tmpACD.f2=
_tmp5AB,((_tmpACD.f3=_tmp5AC,((_tmpACD.f4=_tmp5A7,((_tmpACD.f5=_tmp5AD,_tmpACD)))))))))))),
_tmpACE))))),_tmpACC)))),_tmpACF)))),((_tmpACB->tl=tms,_tmpACB))))));}for(0;
_tmp5AE != 0;_tmp5AE=_tmp5AE->tl){void*_tmp5DA=(void*)_tmp5AE->hd;_LL2EA: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp5DB=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp5DA;if(_tmp5DB->tag != 1)goto _LL2EC;}_LL2EB: goto _LL2ED;_LL2EC: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp5DC=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp5DA;if(_tmp5DC->tag != 2)goto
_LL2EE;}_LL2ED: goto _LL2EF;_LL2EE: {struct Cyc_Absyn_Fastcall_att_struct*_tmp5DD=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp5DA;if(_tmp5DD->tag != 3)goto _LL2F0;}
_LL2EF:{struct Cyc_Absyn_Attributes_mod_struct*_tmpAD9;struct Cyc_List_List*
_tmpAD8;struct Cyc_Absyn_Attributes_mod_struct _tmpAD7;struct Cyc_List_List*_tmpAD6;
tms=((_tmpAD6=_region_malloc(r,sizeof(*_tmpAD6)),((_tmpAD6->hd=(void*)((_tmpAD9=
_region_malloc(r,sizeof(*_tmpAD9)),((_tmpAD9[0]=((_tmpAD7.tag=5,((_tmpAD7.f1=0,((
_tmpAD7.f2=((_tmpAD8=_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8->hd=(void*)_tmp5AE->hd,((
_tmpAD8->tl=0,_tmpAD8)))))),_tmpAD7)))))),_tmpAD9)))),((_tmpAD6->tl=tms,_tmpAD6))))));}
goto AfterAtts;_LL2F0:;_LL2F1: goto _LL2E9;_LL2E9:;}break;}AfterAtts: if(_tmp5A6 != 0){
struct Cyc_Absyn_TypeParams_mod_struct*_tmpADF;struct Cyc_Absyn_TypeParams_mod_struct
_tmpADE;struct Cyc_List_List*_tmpADD;tms=((_tmpADD=_region_malloc(r,sizeof(*
_tmpADD)),((_tmpADD->hd=(void*)((_tmpADF=_region_malloc(r,sizeof(*_tmpADF)),((
_tmpADF[0]=((_tmpADE.tag=4,((_tmpADE.f1=_tmp5A6,((_tmpADE.f2=0,((_tmpADE.f3=1,
_tmpADE)))))))),_tmpADF)))),((_tmpADD->tl=tms,_tmpADD))))));}{struct _tuple13
_tmpAE0;return(_tmpAE0.f1=_tmp5CB,((_tmpAE0.f2=_tmp5CC,((_tmpAE0.f3=tms,_tmpAE0)))));};};};
_LL2E0: {struct Cyc_Absyn_Evar_struct*_tmp5AF=(struct Cyc_Absyn_Evar_struct*)
_tmp597;if(_tmp5AF->tag != 1)goto _LL2E2;else{_tmp5B0=_tmp5AF->f1;_tmp5B1=_tmp5AF->f2;
_tmp5B2=_tmp5AF->f3;}}_LL2E1: if(_tmp5B1 == 0){struct _tuple13 _tmpAE1;return(
_tmpAE1.f1=tq,((_tmpAE1.f2=t,((_tmpAE1.f3=0,_tmpAE1)))));}else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp5B1->v);}_LL2E2: {struct Cyc_Absyn_TypedefType_struct*_tmp5B3=(
struct Cyc_Absyn_TypedefType_struct*)_tmp597;if(_tmp5B3->tag != 17)goto _LL2E4;
else{_tmp5B4=_tmp5B3->f1;_tmp5B5=_tmp5B3->f2;_tmp5B6=_tmp5B3->f3;_tmp5B7=_tmp5B3->f4;}}
_LL2E3: if((_tmp5B7 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp5B7)){struct _tuple13 _tmpAE2;return(_tmpAE2.f1=tq,((_tmpAE2.f2=t,((_tmpAE2.f3=
0,_tmpAE2)))));}else{if(tq.real_const)tq.print_const=tq.real_const;return Cyc_Absynpp_to_tms(
r,tq,*_tmp5B7);}_LL2E4:;_LL2E5: {struct _tuple13 _tmpAE3;return(_tmpAE3.f1=tq,((
_tmpAE3.f2=t,((_tmpAE3.f3=0,_tmpAE3)))));}_LL2D9:;}static int Cyc_Absynpp_is_char_ptr(
void*t){void*_tmp5E9=t;struct Cyc_Core_Opt*_tmp5EB;struct Cyc_Core_Opt _tmp5EC;void*
_tmp5ED;struct Cyc_Absyn_PtrInfo _tmp5EF;void*_tmp5F0;_LL2F3: {struct Cyc_Absyn_Evar_struct*
_tmp5EA=(struct Cyc_Absyn_Evar_struct*)_tmp5E9;if(_tmp5EA->tag != 1)goto _LL2F5;
else{_tmp5EB=_tmp5EA->f2;if(_tmp5EB == 0)goto _LL2F5;_tmp5EC=*_tmp5EB;_tmp5ED=(
void*)_tmp5EC.v;}}_LL2F4: return Cyc_Absynpp_is_char_ptr(_tmp5ED);_LL2F5: {struct
Cyc_Absyn_PointerType_struct*_tmp5EE=(struct Cyc_Absyn_PointerType_struct*)
_tmp5E9;if(_tmp5EE->tag != 5)goto _LL2F7;else{_tmp5EF=_tmp5EE->f1;_tmp5F0=_tmp5EF.elt_typ;}}
_LL2F6: L: {void*_tmp5F1=_tmp5F0;struct Cyc_Core_Opt*_tmp5F3;struct Cyc_Core_Opt
_tmp5F4;void*_tmp5F5;void**_tmp5F7;void*_tmp5F8;enum Cyc_Absyn_Size_of _tmp5FA;
_LL2FA: {struct Cyc_Absyn_Evar_struct*_tmp5F2=(struct Cyc_Absyn_Evar_struct*)
_tmp5F1;if(_tmp5F2->tag != 1)goto _LL2FC;else{_tmp5F3=_tmp5F2->f2;if(_tmp5F3 == 0)
goto _LL2FC;_tmp5F4=*_tmp5F3;_tmp5F5=(void*)_tmp5F4.v;}}_LL2FB: _tmp5F0=_tmp5F5;
goto L;_LL2FC: {struct Cyc_Absyn_TypedefType_struct*_tmp5F6=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5F1;if(_tmp5F6->tag != 17)goto _LL2FE;else{_tmp5F7=_tmp5F6->f4;if(_tmp5F7 == 0)
goto _LL2FE;_tmp5F8=*_tmp5F7;}}_LL2FD: _tmp5F0=_tmp5F8;goto L;_LL2FE: {struct Cyc_Absyn_IntType_struct*
_tmp5F9=(struct Cyc_Absyn_IntType_struct*)_tmp5F1;if(_tmp5F9->tag != 6)goto _LL300;
else{_tmp5FA=_tmp5F9->f2;if(_tmp5FA != Cyc_Absyn_Char_sz)goto _LL300;}}_LL2FF:
return 1;_LL300:;_LL301: return 0;_LL2F9:;}_LL2F7:;_LL2F8: return 0;_LL2F2:;}struct
Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*
dopt){struct _RegionHandle _tmp5FB=_new_region("temp");struct _RegionHandle*temp=&
_tmp5FB;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp5FD;void*_tmp5FE;struct Cyc_List_List*
_tmp5FF;struct _tuple13 _tmp5FC=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp5FD=_tmp5FC.f1;
_tmp5FE=_tmp5FC.f2;_tmp5FF=_tmp5FC.f3;_tmp5FF=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5FF);if(_tmp5FF == 0  && dopt == 0){struct
Cyc_PP_Doc*_tmpAE4[2];struct Cyc_PP_Doc*_tmp601=(_tmpAE4[1]=Cyc_Absynpp_ntyp2doc(
_tmp5FE),((_tmpAE4[0]=Cyc_Absynpp_tqual2doc(_tmp5FD),Cyc_PP_cat(_tag_dyneither(
_tmpAE4,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp601;}else{
const char*_tmpAE7;struct Cyc_PP_Doc*_tmpAE6[4];struct Cyc_PP_Doc*_tmp604=(_tmpAE6[
3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*)dopt->v,_tmp5FF),((_tmpAE6[2]=Cyc_PP_text(((_tmpAE7=" ",
_tag_dyneither(_tmpAE7,sizeof(char),2)))),((_tmpAE6[1]=Cyc_Absynpp_ntyp2doc(
_tmp5FE),((_tmpAE6[0]=Cyc_Absynpp_tqual2doc(_tmp5FD),Cyc_PP_cat(_tag_dyneither(
_tmpAE6,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp604;}};
_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*
f){switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL302: if(f->width != 0){
struct Cyc_Core_Opt*_tmpAEE;const char*_tmpAED;const char*_tmpAEC;struct Cyc_PP_Doc*
_tmpAEB[5];return(_tmpAEB[4]=Cyc_PP_text(((_tmpAEC=";",_tag_dyneither(_tmpAEC,
sizeof(char),2)))),((_tmpAEB[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpAEB[2]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAEB[1]=
Cyc_PP_text(((_tmpAED=":",_tag_dyneither(_tmpAED,sizeof(char),2)))),((_tmpAEB[0]=
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((
_tmpAEE->v=Cyc_PP_textptr(f->name),_tmpAEE))))),Cyc_PP_cat(_tag_dyneither(
_tmpAEB,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*_tmpAF3;
const char*_tmpAF2;struct Cyc_PP_Doc*_tmpAF1[3];return(_tmpAF1[2]=Cyc_PP_text(((
_tmpAF2=";",_tag_dyneither(_tmpAF2,sizeof(char),2)))),((_tmpAF1[1]=Cyc_Absynpp_atts2doc(
f->attributes),((_tmpAF1[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAF3=
_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3->v=Cyc_PP_textptr(f->name),_tmpAF3))))),
Cyc_PP_cat(_tag_dyneither(_tmpAF1,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c:
_LL303: if(f->width != 0){struct Cyc_Core_Opt*_tmpAFA;const char*_tmpAF9;const char*
_tmpAF8;struct Cyc_PP_Doc*_tmpAF7[5];return(_tmpAF7[4]=Cyc_PP_text(((_tmpAF8=";",
_tag_dyneither(_tmpAF8,sizeof(char),2)))),((_tmpAF7[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAF7[2]=Cyc_PP_text(((_tmpAF9=":",
_tag_dyneither(_tmpAF9,sizeof(char),2)))),((_tmpAF7[1]=Cyc_Absynpp_tqtd2doc(f->tq,
f->type,((_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA->v=Cyc_PP_textptr(f->name),
_tmpAFA))))),((_tmpAF7[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAF7,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpAFF;const char*_tmpAFE;struct Cyc_PP_Doc*_tmpAFD[3];return(_tmpAFD[2]=Cyc_PP_text(((
_tmpAFE=";",_tag_dyneither(_tmpAFE,sizeof(char),2)))),((_tmpAFD[1]=Cyc_Absynpp_tqtd2doc(
f->tq,f->type,((_tmpAFF=_cycalloc(sizeof(*_tmpAFF)),((_tmpAFF->v=Cyc_PP_textptr(
f->name),_tmpAFF))))),((_tmpAFD[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAFD,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){const char*_tmpB00;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((
_tmpB00="",_tag_dyneither(_tmpB00,sizeof(char),1))),fields);}struct Cyc_PP_Doc*
Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*
_tmpB01[3];return(_tmpB01[2]=f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((
_tmpB01[1]=Cyc_Absynpp_typedef_name2doc(f->name),((_tmpB01[0]=Cyc_Absynpp_scope2doc(
f->sc),Cyc_PP_cat(_tag_dyneither(_tmpB01,sizeof(struct Cyc_PP_Doc*),3)))))));}
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpB02;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((
_tmpB02=",",_tag_dyneither(_tmpB02,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmpB05;
void*_tmpB04;(_tmpB04=0,Cyc_fprintf(f,((_tmpB05="\n",_tag_dyneither(_tmpB05,
sizeof(char),2))),_tag_dyneither(_tmpB04,sizeof(void*),0)));};}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmpB06;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpB06="",_tag_dyneither(
_tmpB06,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
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
