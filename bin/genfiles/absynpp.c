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
};struct Cyc_Absyn_RefCntRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{int
tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,
Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct
Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};
struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
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
_dyneither_ptr*v){struct Cyc_List_List*_tmp60D;Cyc_Absynpp_curr_namespace=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_Absynpp_curr_namespace,((_tmp60D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp60D)),((_tmp60D->hd=v,((_tmp60D->tl=0,_tmp60D)))))));}static void Cyc_Absynpp_suppr_last(
struct Cyc_List_List**l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;
else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop(){((void(*)(struct Cyc_List_List**l))
Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}struct _dyneither_ptr Cyc_Absynpp_char_escape(
char c){switch(c){case '\a': _LL0: {const char*_tmp60E;return(_tmp60E="\\a",
_tag_dyneither(_tmp60E,sizeof(char),3));}case '\b': _LL1: {const char*_tmp60F;
return(_tmp60F="\\b",_tag_dyneither(_tmp60F,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp610;return(_tmp610="\\f",_tag_dyneither(_tmp610,sizeof(char),3));}
case '\n': _LL3: {const char*_tmp611;return(_tmp611="\\n",_tag_dyneither(_tmp611,
sizeof(char),3));}case '\r': _LL4: {const char*_tmp612;return(_tmp612="\\r",
_tag_dyneither(_tmp612,sizeof(char),3));}case '\t': _LL5: {const char*_tmp613;
return(_tmp613="\\t",_tag_dyneither(_tmp613,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp614;return(_tmp614="\\v",_tag_dyneither(_tmp614,sizeof(char),3));}
case '\\': _LL7: {const char*_tmp615;return(_tmp615="\\\\",_tag_dyneither(_tmp615,
sizeof(char),3));}case '"': _LL8: {const char*_tmp616;return(_tmp616="\"",
_tag_dyneither(_tmp616,sizeof(char),2));}case '\'': _LL9: {const char*_tmp617;
return(_tmp617="\\'",_tag_dyneither(_tmp617,sizeof(char),3));}default: _LLA: if(c
>= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);{char _tmp61A;
char _tmp619;struct _dyneither_ptr _tmp618;(_tmp618=_dyneither_ptr_plus(_tmpC,
sizeof(char),0),((_tmp619=*((char*)_check_dyneither_subscript(_tmp618,sizeof(
char),0)),((_tmp61A=c,((_get_dyneither_size(_tmp618,sizeof(char))== 1  && (
_tmp619 == '\000'  && _tmp61A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp618.curr)=
_tmp61A)))))));}return(struct _dyneither_ptr)_tmpC;}else{struct _dyneither_ptr
_tmp10=Cyc_Core_new_string(5);int j=0;{char _tmp61D;char _tmp61C;struct
_dyneither_ptr _tmp61B;(_tmp61B=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp61C=*((char*)_check_dyneither_subscript(_tmp61B,sizeof(char),0)),((_tmp61D='\\',((
_get_dyneither_size(_tmp61B,sizeof(char))== 1  && (_tmp61C == '\000'  && _tmp61D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp61B.curr)=_tmp61D)))))));}{char _tmp620;char
_tmp61F;struct _dyneither_ptr _tmp61E;(_tmp61E=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp61F=*((char*)_check_dyneither_subscript(_tmp61E,sizeof(char),0)),((
_tmp620=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp61E,
sizeof(char))== 1  && (_tmp61F == '\000'  && _tmp620 != '\000')?_throw_arraybounds():
1,*((char*)_tmp61E.curr)=_tmp620)))))));}{char _tmp623;char _tmp622;struct
_dyneither_ptr _tmp621;(_tmp621=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp622=*((char*)_check_dyneither_subscript(_tmp621,sizeof(char),0)),((_tmp623=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp621,sizeof(char))== 1  && (
_tmp622 == '\000'  && _tmp623 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp621.curr)=
_tmp623)))))));}{char _tmp626;char _tmp625;struct _dyneither_ptr _tmp624;(_tmp624=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp625=*((char*)
_check_dyneither_subscript(_tmp624,sizeof(char),0)),((_tmp626=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp624,sizeof(char))== 1  && (_tmp625 == '\000'  && _tmp626 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp624.curr)=_tmp626)))))));}return(struct
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
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp629;char _tmp628;struct
_dyneither_ptr _tmp627;(_tmp627=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp628=*((
char*)_check_dyneither_subscript(_tmp627,sizeof(char),0)),((_tmp629='\\',((
_get_dyneither_size(_tmp627,sizeof(char))== 1  && (_tmp628 == '\000'  && _tmp629 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp627.curr)=_tmp629)))))));}{char _tmp62C;char
_tmp62B;struct _dyneither_ptr _tmp62A;(_tmp62A=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp62B=*((char*)_check_dyneither_subscript(_tmp62A,sizeof(char),0)),((
_tmp62C='a',((_get_dyneither_size(_tmp62A,sizeof(char))== 1  && (_tmp62B == '\000'
 && _tmp62C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62A.curr)=_tmp62C)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp62F;char _tmp62E;struct
_dyneither_ptr _tmp62D;(_tmp62D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62E=*((
char*)_check_dyneither_subscript(_tmp62D,sizeof(char),0)),((_tmp62F='\\',((
_get_dyneither_size(_tmp62D,sizeof(char))== 1  && (_tmp62E == '\000'  && _tmp62F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp62D.curr)=_tmp62F)))))));}{char _tmp632;char
_tmp631;struct _dyneither_ptr _tmp630;(_tmp630=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp631=*((char*)_check_dyneither_subscript(_tmp630,sizeof(char),0)),((
_tmp632='b',((_get_dyneither_size(_tmp630,sizeof(char))== 1  && (_tmp631 == '\000'
 && _tmp632 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp630.curr)=_tmp632)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp635;char _tmp634;struct
_dyneither_ptr _tmp633;(_tmp633=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp634=*((
char*)_check_dyneither_subscript(_tmp633,sizeof(char),0)),((_tmp635='\\',((
_get_dyneither_size(_tmp633,sizeof(char))== 1  && (_tmp634 == '\000'  && _tmp635 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp633.curr)=_tmp635)))))));}{char _tmp638;char
_tmp637;struct _dyneither_ptr _tmp636;(_tmp636=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp637=*((char*)_check_dyneither_subscript(_tmp636,sizeof(char),0)),((
_tmp638='f',((_get_dyneither_size(_tmp636,sizeof(char))== 1  && (_tmp637 == '\000'
 && _tmp638 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp636.curr)=_tmp638)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp63B;char _tmp63A;struct
_dyneither_ptr _tmp639;(_tmp639=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63A=*((
char*)_check_dyneither_subscript(_tmp639,sizeof(char),0)),((_tmp63B='\\',((
_get_dyneither_size(_tmp639,sizeof(char))== 1  && (_tmp63A == '\000'  && _tmp63B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp639.curr)=_tmp63B)))))));}{char _tmp63E;char
_tmp63D;struct _dyneither_ptr _tmp63C;(_tmp63C=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp63D=*((char*)_check_dyneither_subscript(_tmp63C,sizeof(char),0)),((
_tmp63E='n',((_get_dyneither_size(_tmp63C,sizeof(char))== 1  && (_tmp63D == '\000'
 && _tmp63E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63C.curr)=_tmp63E)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp641;char _tmp640;struct
_dyneither_ptr _tmp63F;(_tmp63F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp640=*((
char*)_check_dyneither_subscript(_tmp63F,sizeof(char),0)),((_tmp641='\\',((
_get_dyneither_size(_tmp63F,sizeof(char))== 1  && (_tmp640 == '\000'  && _tmp641 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp63F.curr)=_tmp641)))))));}{char _tmp644;char
_tmp643;struct _dyneither_ptr _tmp642;(_tmp642=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp643=*((char*)_check_dyneither_subscript(_tmp642,sizeof(char),0)),((
_tmp644='r',((_get_dyneither_size(_tmp642,sizeof(char))== 1  && (_tmp643 == '\000'
 && _tmp644 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp642.curr)=_tmp644)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp647;char _tmp646;struct
_dyneither_ptr _tmp645;(_tmp645=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp646=*((
char*)_check_dyneither_subscript(_tmp645,sizeof(char),0)),((_tmp647='\\',((
_get_dyneither_size(_tmp645,sizeof(char))== 1  && (_tmp646 == '\000'  && _tmp647 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp645.curr)=_tmp647)))))));}{char _tmp64A;char
_tmp649;struct _dyneither_ptr _tmp648;(_tmp648=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp649=*((char*)_check_dyneither_subscript(_tmp648,sizeof(char),0)),((
_tmp64A='t',((_get_dyneither_size(_tmp648,sizeof(char))== 1  && (_tmp649 == '\000'
 && _tmp64A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp648.curr)=_tmp64A)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp64D;char _tmp64C;struct
_dyneither_ptr _tmp64B;(_tmp64B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64C=*((
char*)_check_dyneither_subscript(_tmp64B,sizeof(char),0)),((_tmp64D='\\',((
_get_dyneither_size(_tmp64B,sizeof(char))== 1  && (_tmp64C == '\000'  && _tmp64D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp64B.curr)=_tmp64D)))))));}{char _tmp650;char
_tmp64F;struct _dyneither_ptr _tmp64E;(_tmp64E=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp64F=*((char*)_check_dyneither_subscript(_tmp64E,sizeof(char),0)),((
_tmp650='v',((_get_dyneither_size(_tmp64E,sizeof(char))== 1  && (_tmp64F == '\000'
 && _tmp650 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64E.curr)=_tmp650)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp653;char _tmp652;struct
_dyneither_ptr _tmp651;(_tmp651=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp652=*((
char*)_check_dyneither_subscript(_tmp651,sizeof(char),0)),((_tmp653='\\',((
_get_dyneither_size(_tmp651,sizeof(char))== 1  && (_tmp652 == '\000'  && _tmp653 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp651.curr)=_tmp653)))))));}{char _tmp656;char
_tmp655;struct _dyneither_ptr _tmp654;(_tmp654=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp655=*((char*)_check_dyneither_subscript(_tmp654,sizeof(char),0)),((
_tmp656='\\',((_get_dyneither_size(_tmp654,sizeof(char))== 1  && (_tmp655 == '\000'
 && _tmp656 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp654.curr)=_tmp656)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp659;char _tmp658;struct
_dyneither_ptr _tmp657;(_tmp657=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp658=*((
char*)_check_dyneither_subscript(_tmp657,sizeof(char),0)),((_tmp659='\\',((
_get_dyneither_size(_tmp657,sizeof(char))== 1  && (_tmp658 == '\000'  && _tmp659 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp657.curr)=_tmp659)))))));}{char _tmp65C;char
_tmp65B;struct _dyneither_ptr _tmp65A;(_tmp65A=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp65B=*((char*)_check_dyneither_subscript(_tmp65A,sizeof(char),0)),((
_tmp65C='"',((_get_dyneither_size(_tmp65A,sizeof(char))== 1  && (_tmp65B == '\000'
 && _tmp65C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65A.curr)=_tmp65C)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp65F;char _tmp65E;
struct _dyneither_ptr _tmp65D;(_tmp65D=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp65E=*((char*)_check_dyneither_subscript(_tmp65D,sizeof(char),0)),((_tmp65F=
_tmp1E,((_get_dyneither_size(_tmp65D,sizeof(char))== 1  && (_tmp65E == '\000'  && 
_tmp65F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65D.curr)=_tmp65F)))))));}
else{{char _tmp662;char _tmp661;struct _dyneither_ptr _tmp660;(_tmp660=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp661=*((char*)
_check_dyneither_subscript(_tmp660,sizeof(char),0)),((_tmp662='\\',((
_get_dyneither_size(_tmp660,sizeof(char))== 1  && (_tmp661 == '\000'  && _tmp662 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp660.curr)=_tmp662)))))));}{char _tmp665;char
_tmp664;struct _dyneither_ptr _tmp663;(_tmp663=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp664=*((char*)_check_dyneither_subscript(_tmp663,sizeof(char),0)),((
_tmp665=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp663,sizeof(char))
== 1  && (_tmp664 == '\000'  && _tmp665 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp663.curr)=_tmp665)))))));}{char _tmp668;char _tmp667;struct _dyneither_ptr
_tmp666;(_tmp666=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp667=*((char*)
_check_dyneither_subscript(_tmp666,sizeof(char),0)),((_tmp668=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp666,sizeof(char))== 1  && (_tmp667 == '\000'
 && _tmp668 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp666.curr)=_tmp668)))))));}{
char _tmp66B;char _tmp66A;struct _dyneither_ptr _tmp669;(_tmp669=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp66A=*((char*)_check_dyneither_subscript(_tmp669,
sizeof(char),0)),((_tmp66B=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp669,sizeof(char))== 1  && (_tmp66A == '\000'  && _tmp66B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp669.curr)=_tmp66B)))))));};}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;};};};}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*
l=0;if(tq.q_restrict){struct Cyc_List_List*_tmp66C;l=((_tmp66C=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp66C)),((_tmp66C->hd=Cyc_Absynpp_restrict_sp,((_tmp66C->tl=l,_tmp66C))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp66D;l=((_tmp66D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp66D)),((_tmp66D->hd=Cyc_Absynpp_volatile_sp,((_tmp66D->tl=l,_tmp66D))))));}
if(tq.print_const){struct Cyc_List_List*_tmp66E;l=((_tmp66E=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp66E)),((_tmp66E->hd=Cyc_Absynpp_const_sp,((_tmp66E->tl=l,_tmp66E))))));}{
const char*_tmp671;const char*_tmp670;const char*_tmp66F;return Cyc_PP_egroup(((
_tmp66F="",_tag_dyneither(_tmp66F,sizeof(char),1))),((_tmp670=" ",_tag_dyneither(
_tmp670,sizeof(char),2))),((_tmp671=" ",_tag_dyneither(_tmp671,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,l));};}struct _dyneither_ptr Cyc_Absynpp_kind2string(
struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp6E;enum Cyc_Absyn_KindQual
_tmp6F;enum Cyc_Absyn_AliasQual _tmp70;struct Cyc_Absyn_Kind*_tmp6D=ka;_tmp6E=*
_tmp6D;_tmp6F=_tmp6E.kind;_tmp70=_tmp6E.aliasqual;switch(_tmp6F){case Cyc_Absyn_AnyKind:
_LL36: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL38: {const char*_tmp672;return(
_tmp672="A",_tag_dyneither(_tmp672,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp673;return(_tmp673="UA",_tag_dyneither(_tmp673,sizeof(char),3));}
case Cyc_Absyn_Top: _LL3A: {const char*_tmp674;return(_tmp674="TA",_tag_dyneither(
_tmp674,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37: switch(_tmp70){case Cyc_Absyn_Aliasable:
_LL3D: {const char*_tmp675;return(_tmp675="M",_tag_dyneither(_tmp675,sizeof(char),
2));}case Cyc_Absyn_Unique: _LL3E: {const char*_tmp676;return(_tmp676="UM",
_tag_dyneither(_tmp676,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {const char*
_tmp677;return(_tmp677="TM",_tag_dyneither(_tmp677,sizeof(char),3));}}case Cyc_Absyn_BoxKind:
_LL3C: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL42: {const char*_tmp678;return(
_tmp678="B",_tag_dyneither(_tmp678,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp679;return(_tmp679="UB",_tag_dyneither(_tmp679,sizeof(char),3));}
case Cyc_Absyn_Top: _LL44: {const char*_tmp67A;return(_tmp67A="TB",_tag_dyneither(
_tmp67A,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41: switch(_tmp70){case Cyc_Absyn_Aliasable:
_LL47: {const char*_tmp67B;return(_tmp67B="R",_tag_dyneither(_tmp67B,sizeof(char),
2));}case Cyc_Absyn_Unique: _LL48: {const char*_tmp67C;return(_tmp67C="UR",
_tag_dyneither(_tmp67C,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {const char*
_tmp67D;return(_tmp67D="TR",_tag_dyneither(_tmp67D,sizeof(char),3));}}case Cyc_Absyn_EffKind:
_LL46: {const char*_tmp67E;return(_tmp67E="E",_tag_dyneither(_tmp67E,sizeof(char),
2));}case Cyc_Absyn_IntKind: _LL4B: {const char*_tmp67F;return(_tmp67F="I",
_tag_dyneither(_tmp67F,sizeof(char),2));}}}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp7F=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp81;struct Cyc_Absyn_Kind*_tmp84;_LL4E: {struct Cyc_Absyn_Eq_kb_struct*
_tmp80=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7F;if(_tmp80->tag != 0)goto _LL50;else{
_tmp81=_tmp80->f1;}}_LL4F: return Cyc_Absynpp_kind2string(_tmp81);_LL50: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp82=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp7F;
if(_tmp82->tag != 1)goto _LL52;}_LL51: if(Cyc_PP_tex_output){const char*_tmp680;
return(_tmp680="{?}",_tag_dyneither(_tmp680,sizeof(char),4));}else{const char*
_tmp681;return(_tmp681="?",_tag_dyneither(_tmp681,sizeof(char),2));}_LL52: {
struct Cyc_Absyn_Less_kb_struct*_tmp83=(struct Cyc_Absyn_Less_kb_struct*)_tmp7F;
if(_tmp83->tag != 2)goto _LL4D;else{_tmp84=_tmp83->f2;}}_LL53: {const char*_tmp685;
void*_tmp684[1];struct Cyc_String_pa_struct _tmp683;return(struct _dyneither_ptr)((
_tmp683.tag=0,((_tmp683.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp84)),((_tmp684[0]=& _tmp683,Cyc_aprintf(((_tmp685="<=%s",_tag_dyneither(
_tmp685,sizeof(char),5))),_tag_dyneither(_tmp684,sizeof(void*),1))))))));}_LL4D:;}
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){void*_tmp8A=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp8C;struct Cyc_Absyn_Kind*_tmp8F;_LL55: {struct Cyc_Absyn_Eq_kb_struct*
_tmp8B=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL57;else{
_tmp8C=_tmp8B->f1;}}_LL56: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8C));
_LL57: {struct Cyc_Absyn_Unknown_kb_struct*_tmp8D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8A;if(_tmp8D->tag != 1)goto _LL59;}_LL58: if(Cyc_PP_tex_output){const char*
_tmp686;return Cyc_PP_text_width(((_tmp686="{?}",_tag_dyneither(_tmp686,sizeof(
char),4))),1);}else{const char*_tmp687;return Cyc_PP_text(((_tmp687="?",
_tag_dyneither(_tmp687,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_struct*
_tmp8E=(struct Cyc_Absyn_Less_kb_struct*)_tmp8A;if(_tmp8E->tag != 2)goto _LL54;
else{_tmp8F=_tmp8E->f2;}}_LL5A: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8F));
_LL54:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*
_tmp68A;const char*_tmp689;const char*_tmp688;return Cyc_PP_egroup(((_tmp688="<",
_tag_dyneither(_tmp688,sizeof(char),2))),((_tmp689=">",_tag_dyneither(_tmp689,
sizeof(char),2))),((_tmp68A=",",_tag_dyneither(_tmp68A,sizeof(char),2))),((
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
_tmp68D;struct Cyc_PP_Doc*_tmp68C[3];return(_tmp68C[2]=Cyc_Absynpp_kind2doc(
_tmp9F),((_tmp68C[1]=Cyc_PP_text(((_tmp68D="::",_tag_dyneither(_tmp68D,sizeof(
char),3)))),((_tmp68C[0]=Cyc_PP_textptr(tv->name),Cyc_PP_cat(_tag_dyneither(
_tmp68C,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs){const char*_tmp690;const char*_tmp68F;const char*_tmp68E;
return Cyc_PP_egroup(((_tmp68E="<",_tag_dyneither(_tmp68E,sizeof(char),2))),((
_tmp68F=">",_tag_dyneither(_tmp68F,sizeof(char),2))),((_tmp690=",",
_tag_dyneither(_tmp690,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,
tvs));}static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);{const char*
_tmp693;const char*_tmp692;const char*_tmp691;return Cyc_PP_egroup(((_tmp691="<",
_tag_dyneither(_tmp691,sizeof(char),2))),((_tmp692=">",_tag_dyneither(_tmp692,
sizeof(char),2))),((_tmp693=",",_tag_dyneither(_tmp693,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple15*t){return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*
Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){const char*_tmp696;const char*_tmp695;
const char*_tmp694;return Cyc_PP_group(((_tmp694="(",_tag_dyneither(_tmp694,
sizeof(char),2))),((_tmp695=")",_tag_dyneither(_tmp695,sizeof(char),2))),((
_tmp696=",",_tag_dyneither(_tmp696,sizeof(char),2))),((struct Cyc_List_List*(*)(
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
_tmpAF;if(_tmpB0->tag != 1)goto _LL70;}_LL6F: {const char*_tmp697;return Cyc_PP_text(((
_tmp697=" _stdcall ",_tag_dyneither(_tmp697,sizeof(char),11))));}_LL70: {struct
Cyc_Absyn_Cdecl_att_struct*_tmpB1=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpAF;if(
_tmpB1->tag != 2)goto _LL72;}_LL71: {const char*_tmp698;return Cyc_PP_text(((_tmp698=" _cdecl ",
_tag_dyneither(_tmp698,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpB2=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpAF;if(_tmpB2->tag != 3)goto _LL74;}
_LL73: {const char*_tmp699;return Cyc_PP_text(((_tmp699=" _fastcall ",
_tag_dyneither(_tmp699,sizeof(char),12))));}_LL74:;_LL75: goto _LL6D;_LL6D:;}
return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpB6=(void*)atts2->hd;_LL77: {struct Cyc_Absyn_Stdcall_att_struct*_tmpB7=(
struct Cyc_Absyn_Stdcall_att_struct*)_tmpB6;if(_tmpB7->tag != 1)goto _LL79;}_LL78:
goto _LL7A;_LL79: {struct Cyc_Absyn_Cdecl_att_struct*_tmpB8=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmpB6;if(_tmpB8->tag != 2)goto _LL7B;}_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpB9=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpB6;if(_tmpB9->tag != 3)goto _LL7D;}
_LL7C: goto _LL76;_LL7D:;_LL7E: hasatt=1;goto _LL76;_LL76:;}}if(!hasatt)return Cyc_PP_nil_doc();{
const char*_tmp6A4;const char*_tmp6A3;const char*_tmp6A2;const char*_tmp6A1;const
char*_tmp6A0;struct Cyc_PP_Doc*_tmp69F[3];return(_tmp69F[2]=Cyc_PP_text(((_tmp6A0=")",
_tag_dyneither(_tmp6A0,sizeof(char),2)))),((_tmp69F[1]=Cyc_PP_group(((_tmp6A3="",
_tag_dyneither(_tmp6A3,sizeof(char),1))),((_tmp6A2="",_tag_dyneither(_tmp6A2,
sizeof(char),1))),((_tmp6A1=" ",_tag_dyneither(_tmp6A1,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_noncallatt2doc,atts)),((_tmp69F[0]=Cyc_PP_text(((_tmp6A4=" __declspec(",
_tag_dyneither(_tmp6A4,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp69F,
sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*
a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Vc_c: _LL7F: return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c:
_LL80: {const char*_tmp6AD;const char*_tmp6AC;const char*_tmp6AB;const char*_tmp6AA;
struct Cyc_PP_Doc*_tmp6A9[2];return(_tmp6A9[1]=Cyc_PP_group(((_tmp6AC="((",
_tag_dyneither(_tmp6AC,sizeof(char),3))),((_tmp6AB="))",_tag_dyneither(_tmp6AB,
sizeof(char),3))),((_tmp6AA=",",_tag_dyneither(_tmp6AA,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_att2doc,atts)),((_tmp6A9[0]=Cyc_PP_text(((_tmp6AD=" __attribute__",
_tag_dyneither(_tmp6AD,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp6A9,
sizeof(struct Cyc_PP_Doc*),2)))));}}}int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*
tms){if(tms == 0)return 0;{void*_tmpC5=(void*)tms->hd;_LL83: {struct Cyc_Absyn_Pointer_mod_struct*
_tmpC6=(struct Cyc_Absyn_Pointer_mod_struct*)_tmpC5;if(_tmpC6->tag != 2)goto _LL85;}
_LL84: return 1;_LL85: {struct Cyc_Absyn_Attributes_mod_struct*_tmpC7=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC5;if(_tmpC7->tag != 5)goto _LL87;}_LL86: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL89: return 0;default: _LL8A: return Cyc_Absynpp_next_is_pointer(
tms->tl);}_LL87:;_LL88: return 0;_LL82:;};}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*
Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output){
const char*_tmp6AE;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp6AE="{?}",_tag_dyneither(_tmp6AE,sizeof(char),4))),1);}else{const char*
_tmp6AF;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp6AF="?",
_tag_dyneither(_tmp6AF,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){const char*_tmp6B0;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp6B0="{\\lb}",_tag_dyneither(_tmp6B0,sizeof(char),6))),1);}
else{const char*_tmp6B1;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp6B1="{",_tag_dyneither(_tmp6B1,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_lb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){const char*_tmp6B2;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp6B2="{\\rb}",_tag_dyneither(_tmp6B2,sizeof(char),6))),1);}
else{const char*_tmp6B3;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp6B3="}",_tag_dyneither(_tmp6B3,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){const char*_tmp6B4;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp6B4="\\$",_tag_dyneither(_tmp6B4,sizeof(char),3))),1);}
else{const char*_tmp6B5;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp6B5="$",_tag_dyneither(_tmp6B5,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_dollar);}struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(
struct _dyneither_ptr sep,struct Cyc_List_List*ss){struct Cyc_PP_Doc*_tmp6B6[3];
return(_tmp6B6[2]=Cyc_Absynpp_rb(),((_tmp6B6[1]=Cyc_PP_seq(sep,ss),((_tmp6B6[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp6B6,sizeof(struct Cyc_PP_Doc*),3)))))));}
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){for(0;tms != 0;tms=tms->tl){
void*_tmpD1=(void*)tms->hd;void*_tmpD5;struct Cyc_List_List*_tmpD7;_LL8D: {struct
Cyc_Absyn_Carray_mod_struct*_tmpD2=(struct Cyc_Absyn_Carray_mod_struct*)_tmpD1;
if(_tmpD2->tag != 0)goto _LL8F;}_LL8E:{const char*_tmp6B9;void*_tmp6B8;(_tmp6B8=0,
Cyc_fprintf(Cyc_stderr,((_tmp6B9="Carray_mod ",_tag_dyneither(_tmp6B9,sizeof(
char),12))),_tag_dyneither(_tmp6B8,sizeof(void*),0)));}goto _LL8C;_LL8F: {struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD3=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD1;if(_tmpD3->tag != 1)goto _LL91;}_LL90:{const char*_tmp6BC;void*_tmp6BB;(
_tmp6BB=0,Cyc_fprintf(Cyc_stderr,((_tmp6BC="ConstArray_mod ",_tag_dyneither(
_tmp6BC,sizeof(char),16))),_tag_dyneither(_tmp6BB,sizeof(void*),0)));}goto _LL8C;
_LL91: {struct Cyc_Absyn_Function_mod_struct*_tmpD4=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD4->tag != 3)goto _LL93;else{_tmpD5=(void*)_tmpD4->f1;{struct Cyc_Absyn_WithTypes_struct*
_tmpD6=(struct Cyc_Absyn_WithTypes_struct*)_tmpD5;if(_tmpD6->tag != 1)goto _LL93;
else{_tmpD7=_tmpD6->f1;}};}}_LL92:{const char*_tmp6BF;void*_tmp6BE;(_tmp6BE=0,Cyc_fprintf(
Cyc_stderr,((_tmp6BF="Function_mod(",_tag_dyneither(_tmp6BF,sizeof(char),14))),
_tag_dyneither(_tmp6BE,sizeof(void*),0)));}for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){
struct Cyc_Core_Opt*_tmpE2=(*((struct _tuple7*)_tmpD7->hd)).f1;if(_tmpE2 == 0){
const char*_tmp6C2;void*_tmp6C1;(_tmp6C1=0,Cyc_fprintf(Cyc_stderr,((_tmp6C2="?",
_tag_dyneither(_tmp6C2,sizeof(char),2))),_tag_dyneither(_tmp6C1,sizeof(void*),0)));}
else{void*_tmp6C3;(_tmp6C3=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)
_tmpE2->v),_tag_dyneither(_tmp6C3,sizeof(void*),0)));}if(_tmpD7->tl != 0){const
char*_tmp6C6;void*_tmp6C5;(_tmp6C5=0,Cyc_fprintf(Cyc_stderr,((_tmp6C6=",",
_tag_dyneither(_tmp6C6,sizeof(char),2))),_tag_dyneither(_tmp6C5,sizeof(void*),0)));}}{
const char*_tmp6C9;void*_tmp6C8;(_tmp6C8=0,Cyc_fprintf(Cyc_stderr,((_tmp6C9=") ",
_tag_dyneither(_tmp6C9,sizeof(char),3))),_tag_dyneither(_tmp6C8,sizeof(void*),0)));}
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_struct*_tmpD8=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD8->tag != 3)goto _LL95;}_LL94:{const char*_tmp6CC;void*_tmp6CB;(
_tmp6CB=0,Cyc_fprintf(Cyc_stderr,((_tmp6CC="Function_mod()",_tag_dyneither(
_tmp6CC,sizeof(char),15))),_tag_dyneither(_tmp6CB,sizeof(void*),0)));}goto _LL8C;
_LL95: {struct Cyc_Absyn_Attributes_mod_struct*_tmpD9=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpD1;if(_tmpD9->tag != 5)goto _LL97;}_LL96:{const char*_tmp6CF;void*_tmp6CE;(
_tmp6CE=0,Cyc_fprintf(Cyc_stderr,((_tmp6CF="Attributes_mod ",_tag_dyneither(
_tmp6CF,sizeof(char),16))),_tag_dyneither(_tmp6CE,sizeof(void*),0)));}goto _LL8C;
_LL97: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpDA=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpD1;if(_tmpDA->tag != 4)goto _LL99;}_LL98:{const char*_tmp6D2;void*_tmp6D1;(
_tmp6D1=0,Cyc_fprintf(Cyc_stderr,((_tmp6D2="TypeParams_mod ",_tag_dyneither(
_tmp6D2,sizeof(char),16))),_tag_dyneither(_tmp6D1,sizeof(void*),0)));}goto _LL8C;
_LL99: {struct Cyc_Absyn_Pointer_mod_struct*_tmpDB=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpD1;if(_tmpDB->tag != 2)goto _LL8C;}_LL9A:{const char*_tmp6D5;void*_tmp6D4;(
_tmp6D4=0,Cyc_fprintf(Cyc_stderr,((_tmp6D5="Pointer_mod ",_tag_dyneither(_tmp6D5,
sizeof(char),13))),_tag_dyneither(_tmp6D4,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
const char*_tmp6D8;void*_tmp6D7;(_tmp6D7=0,Cyc_fprintf(Cyc_stderr,((_tmp6D8="\n",
_tag_dyneither(_tmp6D8,sizeof(char),2))),_tag_dyneither(_tmp6D7,sizeof(void*),0)));};}
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct
Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(
0,d,tms->tl);const char*_tmp6DD;const char*_tmp6DC;struct Cyc_PP_Doc*_tmp6DB[3];
struct Cyc_PP_Doc*p_rest=(_tmp6DB[2]=Cyc_PP_text(((_tmp6DC=")",_tag_dyneither(
_tmp6DC,sizeof(char),2)))),((_tmp6DB[1]=rest,((_tmp6DB[0]=Cyc_PP_text(((_tmp6DD="(",
_tag_dyneither(_tmp6DD,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6DB,
sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpF4=(void*)tms->hd;union Cyc_Absyn_Constraint*
_tmpF6;struct Cyc_Absyn_Exp*_tmpF8;union Cyc_Absyn_Constraint*_tmpF9;void*_tmpFB;
struct Cyc_List_List*_tmpFD;struct Cyc_List_List*_tmpFF;struct Cyc_Position_Segment*
_tmp100;int _tmp101;struct Cyc_Absyn_PtrAtts _tmp103;void*_tmp104;union Cyc_Absyn_Constraint*
_tmp105;union Cyc_Absyn_Constraint*_tmp106;union Cyc_Absyn_Constraint*_tmp107;
struct Cyc_Absyn_Tqual _tmp108;_LL9C: {struct Cyc_Absyn_Carray_mod_struct*_tmpF5=(
struct Cyc_Absyn_Carray_mod_struct*)_tmpF4;if(_tmpF5->tag != 0)goto _LL9E;else{
_tmpF6=_tmpF5->f1;}}_LL9D: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6E2;const char*_tmp6E1;struct Cyc_PP_Doc*_tmp6E0[2];return(_tmp6E0[1]=((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF6)?Cyc_PP_text(((
_tmp6E1="[]@zeroterm",_tag_dyneither(_tmp6E1,sizeof(char),12)))): Cyc_PP_text(((
_tmp6E2="[]",_tag_dyneither(_tmp6E2,sizeof(char),3)))),((_tmp6E0[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp6E0,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpF7=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpF4;if(_tmpF7->tag != 1)goto
_LLA0;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;}}_LL9F: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp6E9;const char*_tmp6E8;const char*_tmp6E7;
struct Cyc_PP_Doc*_tmp6E6[4];return(_tmp6E6[3]=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpF9)?Cyc_PP_text(((_tmp6E7="]@zeroterm",
_tag_dyneither(_tmp6E7,sizeof(char),11)))): Cyc_PP_text(((_tmp6E8="]",
_tag_dyneither(_tmp6E8,sizeof(char),2)))),((_tmp6E6[2]=Cyc_Absynpp_exp2doc(
_tmpF8),((_tmp6E6[1]=Cyc_PP_text(((_tmp6E9="[",_tag_dyneither(_tmp6E9,sizeof(
char),2)))),((_tmp6E6[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6E6,sizeof(struct Cyc_PP_Doc*),
4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_struct*_tmpFA=(struct Cyc_Absyn_Function_mod_struct*)
_tmpF4;if(_tmpFA->tag != 3)goto _LLA2;else{_tmpFB=(void*)_tmpFA->f1;}}_LLA1: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmp110=_tmpFB;struct Cyc_List_List*_tmp112;int _tmp113;
struct Cyc_Absyn_VarargInfo*_tmp114;struct Cyc_Core_Opt*_tmp115;struct Cyc_List_List*
_tmp116;struct Cyc_List_List*_tmp118;struct Cyc_Position_Segment*_tmp119;_LLA9: {
struct Cyc_Absyn_WithTypes_struct*_tmp111=(struct Cyc_Absyn_WithTypes_struct*)
_tmp110;if(_tmp111->tag != 1)goto _LLAB;else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;
_tmp114=_tmp111->f3;_tmp115=_tmp111->f4;_tmp116=_tmp111->f5;}}_LLAA: {struct Cyc_PP_Doc*
_tmp6EA[2];return(_tmp6EA[1]=Cyc_Absynpp_funargs2doc(_tmp112,_tmp113,_tmp114,
_tmp115,_tmp116),((_tmp6EA[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6EA,sizeof(
struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_struct*_tmp117=(
struct Cyc_Absyn_NoTypes_struct*)_tmp110;if(_tmp117->tag != 0)goto _LLA8;else{
_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LLAC: {const char*_tmp6F1;const char*
_tmp6F0;const char*_tmp6EF;struct Cyc_PP_Doc*_tmp6EE[2];return(_tmp6EE[1]=Cyc_PP_group(((
_tmp6F1="(",_tag_dyneither(_tmp6F1,sizeof(char),2))),((_tmp6F0=")",
_tag_dyneither(_tmp6F0,sizeof(char),2))),((_tmp6EF=",",_tag_dyneither(_tmp6EF,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp118)),((
_tmp6EE[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6EE,sizeof(struct Cyc_PP_Doc*),2)))));}
_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_struct*_tmpFC=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpF4;if(_tmpFC->tag != 5)goto _LLA4;else{_tmpFD=_tmpFC->f2;}}_LLA3: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6F2[2];return(_tmp6F2[1]=Cyc_Absynpp_atts2doc(_tmpFD),((
_tmp6F2[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F2,sizeof(struct Cyc_PP_Doc*),2)))));};
case Cyc_Cyclone_Vc_c: _LLAE: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp6F3[2];return(_tmp6F3[1]=rest,((_tmp6F3[0]=Cyc_Absynpp_callconv2doc(_tmpFD),
Cyc_PP_cat(_tag_dyneither(_tmp6F3,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;}
_LLA4: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpFE=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpF4;if(_tmpFE->tag != 4)goto _LLA6;else{_tmpFF=_tmpFE->f1;_tmp100=_tmpFE->f2;
_tmp101=_tmpFE->f3;}}_LLA5: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp101){struct Cyc_PP_Doc*_tmp6F4[2];return(_tmp6F4[1]=Cyc_Absynpp_ktvars2doc(
_tmpFF),((_tmp6F4[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F4,sizeof(struct Cyc_PP_Doc*),
2)))));}else{struct Cyc_PP_Doc*_tmp6F5[2];return(_tmp6F5[1]=Cyc_Absynpp_tvars2doc(
_tmpFF),((_tmp6F5[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F5,sizeof(struct Cyc_PP_Doc*),
2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_struct*_tmp102=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpF4;if(_tmp102->tag != 2)goto _LL9B;else{_tmp103=_tmp102->f1;_tmp104=_tmp103.rgn;
_tmp105=_tmp103.nullable;_tmp106=_tmp103.bounds;_tmp107=_tmp103.zero_term;
_tmp108=_tmp102->f2;}}_LLA7: {struct Cyc_PP_Doc*ptr;{void*_tmp123=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp106);struct Cyc_Absyn_Exp*_tmp126;_LLB1: {struct Cyc_Absyn_DynEither_b_struct*
_tmp124=(struct Cyc_Absyn_DynEither_b_struct*)_tmp123;if(_tmp124->tag != 0)goto
_LLB3;}_LLB2: ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_struct*
_tmp125=(struct Cyc_Absyn_Upper_b_struct*)_tmp123;if(_tmp125->tag != 1)goto _LLB0;
else{_tmp126=_tmp125->f1;}}_LLB4:{const char*_tmp6F7;const char*_tmp6F6;ptr=Cyc_PP_text(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp105)?(
_tmp6F7="*",_tag_dyneither(_tmp6F7,sizeof(char),2)):((_tmp6F6="@",_tag_dyneither(
_tmp6F6,sizeof(char),2))));}{unsigned int _tmp12A;int _tmp12B;struct _tuple10
_tmp129=Cyc_Evexp_eval_const_uint_exp(_tmp126);_tmp12A=_tmp129.f1;_tmp12B=
_tmp129.f2;if(!_tmp12B  || _tmp12A != 1){struct Cyc_PP_Doc*_tmp6F8[4];ptr=((_tmp6F8[
3]=Cyc_Absynpp_rb(),((_tmp6F8[2]=Cyc_Absynpp_exp2doc(_tmp126),((_tmp6F8[1]=Cyc_Absynpp_lb(),((
_tmp6F8[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6F8,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;};_LLB0:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){const char*
_tmp6FB;struct Cyc_PP_Doc*_tmp6FA[2];ptr=((_tmp6FA[1]=Cyc_PP_text(((_tmp6FB="@zeroterm ",
_tag_dyneither(_tmp6FB,sizeof(char),11)))),((_tmp6FA[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6FA,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){
const char*_tmp6FE;struct Cyc_PP_Doc*_tmp6FD[2];ptr=((_tmp6FD[1]=Cyc_PP_text(((
_tmp6FE="@nozeroterm ",_tag_dyneither(_tmp6FE,sizeof(char),13)))),((_tmp6FD[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp6FD,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmp131=Cyc_Tcutil_compress(_tmp104);_LLB6: {struct Cyc_Absyn_HeapRgn_struct*
_tmp132=(struct Cyc_Absyn_HeapRgn_struct*)_tmp131;if(_tmp132->tag != 21)goto _LLB8;}
_LLB7: goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_struct*_tmp133=(struct Cyc_Absyn_Evar_struct*)
_tmp131;if(_tmp133->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto
_LLBA;_LLB9: goto _LLB5;_LLBA:;_LLBB: {const char*_tmp701;struct Cyc_PP_Doc*_tmp700[
3];ptr=((_tmp700[2]=Cyc_PP_text(((_tmp701=" ",_tag_dyneither(_tmp701,sizeof(char),
2)))),((_tmp700[1]=Cyc_Absynpp_typ2doc(_tmp104),((_tmp700[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp700,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}{struct Cyc_PP_Doc*
_tmp702[2];ptr=((_tmp702[1]=Cyc_Absynpp_tqual2doc(_tmp108),((_tmp702[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp702,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp703[2];return(_tmp703[1]=rest,((_tmp703[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp703,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t){void*_tmp13B=Cyc_Tcutil_compress(t);_LLBD: {struct Cyc_Absyn_HeapRgn_struct*
_tmp13C=(struct Cyc_Absyn_HeapRgn_struct*)_tmp13B;if(_tmp13C->tag != 21)goto _LLBF;}
_LLBE: {const char*_tmp704;return Cyc_PP_text(((_tmp704="`H",_tag_dyneither(
_tmp704,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_struct*_tmp13D=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp13B;if(_tmp13D->tag != 22)goto _LLC1;}_LLC0: {
const char*_tmp705;return Cyc_PP_text(((_tmp705="`U",_tag_dyneither(_tmp705,
sizeof(char),3))));}_LLC1: {struct Cyc_Absyn_RefCntRgn_struct*_tmp13E=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp13B;if(_tmp13E->tag != 23)goto _LLC3;}_LLC2: {const char*_tmp706;return Cyc_PP_text(((
_tmp706="`RC",_tag_dyneither(_tmp706,sizeof(char),4))));}_LLC3:;_LLC4: return Cyc_Absynpp_ntyp2doc(
t);_LLBC:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct
Cyc_List_List**effects,void*t){void*_tmp142=Cyc_Tcutil_compress(t);void*_tmp144;
struct Cyc_List_List*_tmp146;_LLC6: {struct Cyc_Absyn_AccessEff_struct*_tmp143=(
struct Cyc_Absyn_AccessEff_struct*)_tmp142;if(_tmp143->tag != 24)goto _LLC8;else{
_tmp144=(void*)_tmp143->f1;}}_LLC7:{struct Cyc_List_List*_tmp707;*rgions=((
_tmp707=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp707)),((_tmp707->hd=
Cyc_Absynpp_rgn2doc(_tmp144),((_tmp707->tl=*rgions,_tmp707))))));}goto _LLC5;
_LLC8: {struct Cyc_Absyn_JoinEff_struct*_tmp145=(struct Cyc_Absyn_JoinEff_struct*)
_tmp142;if(_tmp145->tag != 25)goto _LLCA;else{_tmp146=_tmp145->f1;}}_LLC9: for(0;
_tmp146 != 0;_tmp146=_tmp146->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)
_tmp146->hd);}goto _LLC5;_LLCA:;_LLCB:{struct Cyc_List_List*_tmp708;*effects=((
_tmp708=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp708)),((_tmp708->hd=
Cyc_Absynpp_typ2doc(t),((_tmp708->tl=*effects,_tmp708))))));}goto _LLC5;_LLC5:;}
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=0;struct
Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);rgions=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);effects=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);if(
rgions == 0  && effects != 0){const char*_tmp70B;const char*_tmp70A;const char*_tmp709;
return Cyc_PP_group(((_tmp709="",_tag_dyneither(_tmp709,sizeof(char),1))),((
_tmp70A="",_tag_dyneither(_tmp70A,sizeof(char),1))),((_tmp70B="+",_tag_dyneither(
_tmp70B,sizeof(char),2))),effects);}else{const char*_tmp70C;struct Cyc_PP_Doc*
_tmp14C=Cyc_Absynpp_group_braces(((_tmp70C=",",_tag_dyneither(_tmp70C,sizeof(
char),2))),rgions);struct Cyc_List_List*_tmp710;const char*_tmp70F;const char*
_tmp70E;const char*_tmp70D;return Cyc_PP_group(((_tmp70D="",_tag_dyneither(_tmp70D,
sizeof(char),1))),((_tmp70E="",_tag_dyneither(_tmp70E,sizeof(char),1))),((
_tmp70F="+",_tag_dyneither(_tmp70F,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp710=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp710)),((_tmp710->hd=
_tmp14C,((_tmp710->tl=0,_tmp710))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(
enum Cyc_Absyn_AggrKind k){switch(k){case Cyc_Absyn_StructA: _LLCC: {const char*
_tmp711;return Cyc_PP_text(((_tmp711="struct ",_tag_dyneither(_tmp711,sizeof(char),
8))));}case Cyc_Absyn_UnionA: _LLCD: {const char*_tmp712;return Cyc_PP_text(((
_tmp712="union ",_tag_dyneither(_tmp712,sizeof(char),7))));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp154=t;struct Cyc_Core_Opt*
_tmp15A;struct Cyc_Core_Opt*_tmp15B;int _tmp15C;struct Cyc_Core_Opt*_tmp15D;struct
Cyc_Absyn_Tvar*_tmp15F;struct Cyc_Absyn_DatatypeInfo _tmp161;union Cyc_Absyn_DatatypeInfoU
_tmp162;struct Cyc_List_List*_tmp163;struct Cyc_Absyn_DatatypeFieldInfo _tmp165;
union Cyc_Absyn_DatatypeFieldInfoU _tmp166;struct Cyc_List_List*_tmp167;enum Cyc_Absyn_Sign
_tmp169;enum Cyc_Absyn_Size_of _tmp16A;int _tmp16D;struct Cyc_List_List*_tmp16F;
struct Cyc_Absyn_AggrInfo _tmp171;union Cyc_Absyn_AggrInfoU _tmp172;struct Cyc_List_List*
_tmp173;enum Cyc_Absyn_AggrKind _tmp175;struct Cyc_List_List*_tmp176;struct Cyc_List_List*
_tmp178;struct _tuple0*_tmp17A;struct Cyc_Absyn_Exp*_tmp17C;struct _tuple0*_tmp17E;
struct Cyc_List_List*_tmp17F;struct Cyc_Absyn_Typedefdecl*_tmp180;void*_tmp182;
void*_tmp184;void*_tmp185;void*_tmp187;void*_tmp18C;struct Cyc_Absyn_TypeDecl*
_tmp190;struct Cyc_Absyn_TypeDecl _tmp191;void*_tmp192;struct Cyc_Absyn_Aggrdecl*
_tmp194;struct Cyc_Absyn_TypeDecl*_tmp196;struct Cyc_Absyn_TypeDecl _tmp197;void*
_tmp198;struct Cyc_Absyn_Enumdecl*_tmp19A;struct Cyc_Absyn_TypeDecl*_tmp19C;struct
Cyc_Absyn_TypeDecl _tmp19D;void*_tmp19E;struct Cyc_Absyn_Datatypedecl*_tmp1A0;
_LLD0: {struct Cyc_Absyn_ArrayType_struct*_tmp155=(struct Cyc_Absyn_ArrayType_struct*)
_tmp154;if(_tmp155->tag != 9)goto _LLD2;}_LLD1: {const char*_tmp713;return Cyc_PP_text(((
_tmp713="[[[array]]]",_tag_dyneither(_tmp713,sizeof(char),12))));}_LLD2: {struct
Cyc_Absyn_FnType_struct*_tmp156=(struct Cyc_Absyn_FnType_struct*)_tmp154;if(
_tmp156->tag != 10)goto _LLD4;}_LLD3: return Cyc_PP_nil_doc();_LLD4: {struct Cyc_Absyn_PointerType_struct*
_tmp157=(struct Cyc_Absyn_PointerType_struct*)_tmp154;if(_tmp157->tag != 5)goto
_LLD6;}_LLD5: return Cyc_PP_nil_doc();_LLD6: {struct Cyc_Absyn_VoidType_struct*
_tmp158=(struct Cyc_Absyn_VoidType_struct*)_tmp154;if(_tmp158->tag != 0)goto _LLD8;}
_LLD7:{const char*_tmp714;s=Cyc_PP_text(((_tmp714="void",_tag_dyneither(_tmp714,
sizeof(char),5))));}goto _LLCF;_LLD8: {struct Cyc_Absyn_Evar_struct*_tmp159=(
struct Cyc_Absyn_Evar_struct*)_tmp154;if(_tmp159->tag != 1)goto _LLDA;else{_tmp15A=
_tmp159->f1;_tmp15B=_tmp159->f2;_tmp15C=_tmp159->f3;_tmp15D=_tmp159->f4;}}_LLD9:
if(_tmp15B != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp15B->v);else{const char*
_tmp724;const char*_tmp723;const char*_tmp722;struct Cyc_Int_pa_struct _tmp721;void*
_tmp720[1];const char*_tmp71F;const char*_tmp71E;struct Cyc_PP_Doc*_tmp71D[6];s=((
_tmp71D[5]=_tmp15A == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)
_tmp15A->v),((_tmp71D[4]=Cyc_PP_text(((_tmp71E=")::",_tag_dyneither(_tmp71E,
sizeof(char),4)))),((_tmp71D[3]=(!Cyc_Absynpp_print_full_evars  || _tmp15D == 0)?
Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp15D->v),((
_tmp71D[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp721.tag=1,((_tmp721.f1=(
unsigned long)_tmp15C,((_tmp720[0]=& _tmp721,Cyc_aprintf(((_tmp71F="%d",
_tag_dyneither(_tmp71F,sizeof(char),3))),_tag_dyneither(_tmp720,sizeof(void*),1))))))))),((
_tmp71D[1]=Cyc_PP_text(((_tmp722="(",_tag_dyneither(_tmp722,sizeof(char),2)))),((
_tmp71D[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp723="\\%",
_tag_dyneither(_tmp723,sizeof(char),3)))): Cyc_PP_text(((_tmp724="%",
_tag_dyneither(_tmp724,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp71D,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLCF;_LLDA: {struct Cyc_Absyn_VarType_struct*
_tmp15E=(struct Cyc_Absyn_VarType_struct*)_tmp154;if(_tmp15E->tag != 2)goto _LLDC;
else{_tmp15F=_tmp15E->f1;}}_LLDB: s=Cyc_PP_textptr(_tmp15F->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp727;struct Cyc_PP_Doc*_tmp726[3];s=((_tmp726[2]=Cyc_Absynpp_kindbound2doc(
_tmp15F->kind),((_tmp726[1]=Cyc_PP_text(((_tmp727="::",_tag_dyneither(_tmp727,
sizeof(char),3)))),((_tmp726[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp726,sizeof(
struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp15F)){const char*_tmp72C;const char*_tmp72B;struct Cyc_PP_Doc*_tmp72A[3];s=((
_tmp72A[2]=Cyc_PP_text(((_tmp72B=" */",_tag_dyneither(_tmp72B,sizeof(char),4)))),((
_tmp72A[1]=s,((_tmp72A[0]=Cyc_PP_text(((_tmp72C="_ /* ",_tag_dyneither(_tmp72C,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp72A,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCF;_LLDC: {struct Cyc_Absyn_DatatypeType_struct*_tmp160=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp154;if(_tmp160->tag != 3)goto _LLDE;else{_tmp161=_tmp160->f1;_tmp162=_tmp161.datatype_info;
_tmp163=_tmp161.targs;}}_LLDD:{union Cyc_Absyn_DatatypeInfoU _tmp1B0=_tmp162;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp1B1;struct _tuple0*_tmp1B2;int _tmp1B3;
struct Cyc_Absyn_Datatypedecl**_tmp1B4;struct Cyc_Absyn_Datatypedecl*_tmp1B5;
struct Cyc_Absyn_Datatypedecl _tmp1B6;struct _tuple0*_tmp1B7;int _tmp1B8;_LL10D: if((
_tmp1B0.UnknownDatatype).tag != 1)goto _LL10F;_tmp1B1=(struct Cyc_Absyn_UnknownDatatypeInfo)(
_tmp1B0.UnknownDatatype).val;_tmp1B2=_tmp1B1.name;_tmp1B3=_tmp1B1.is_extensible;
_LL10E: _tmp1B7=_tmp1B2;_tmp1B8=_tmp1B3;goto _LL110;_LL10F: if((_tmp1B0.KnownDatatype).tag
!= 2)goto _LL10C;_tmp1B4=(struct Cyc_Absyn_Datatypedecl**)(_tmp1B0.KnownDatatype).val;
_tmp1B5=*_tmp1B4;_tmp1B6=*_tmp1B5;_tmp1B7=_tmp1B6.name;_tmp1B8=_tmp1B6.is_extensible;
_LL110: {const char*_tmp72D;struct Cyc_PP_Doc*_tmp1B9=Cyc_PP_text(((_tmp72D="datatype ",
_tag_dyneither(_tmp72D,sizeof(char),10))));const char*_tmp72E;struct Cyc_PP_Doc*
_tmp1BA=_tmp1B8?Cyc_PP_text(((_tmp72E="@extensible ",_tag_dyneither(_tmp72E,
sizeof(char),13)))): Cyc_PP_nil_doc();{struct Cyc_PP_Doc*_tmp72F[4];s=((_tmp72F[3]=
Cyc_Absynpp_tps2doc(_tmp163),((_tmp72F[2]=Cyc_Absynpp_qvar2doc(_tmp1B7),((
_tmp72F[1]=_tmp1B9,((_tmp72F[0]=_tmp1BA,Cyc_PP_cat(_tag_dyneither(_tmp72F,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL10C;}_LL10C:;}goto _LLCF;_LLDE: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp164=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp154;if(_tmp164->tag != 4)goto _LLE0;else{_tmp165=_tmp164->f1;_tmp166=_tmp165.field_info;
_tmp167=_tmp165.targs;}}_LLDF:{union Cyc_Absyn_DatatypeFieldInfoU _tmp1BE=_tmp166;
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp1BF;struct _tuple0*_tmp1C0;struct
_tuple0*_tmp1C1;int _tmp1C2;struct _tuple1 _tmp1C3;struct Cyc_Absyn_Datatypedecl*
_tmp1C4;struct Cyc_Absyn_Datatypedecl _tmp1C5;struct _tuple0*_tmp1C6;int _tmp1C7;
struct Cyc_Absyn_Datatypefield*_tmp1C8;struct Cyc_Absyn_Datatypefield _tmp1C9;
struct _tuple0*_tmp1CA;_LL112: if((_tmp1BE.UnknownDatatypefield).tag != 1)goto
_LL114;_tmp1BF=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1BE.UnknownDatatypefield).val;
_tmp1C0=_tmp1BF.datatype_name;_tmp1C1=_tmp1BF.field_name;_tmp1C2=_tmp1BF.is_extensible;
_LL113: _tmp1C6=_tmp1C0;_tmp1C7=_tmp1C2;_tmp1CA=_tmp1C1;goto _LL115;_LL114: if((
_tmp1BE.KnownDatatypefield).tag != 2)goto _LL111;_tmp1C3=(struct _tuple1)(_tmp1BE.KnownDatatypefield).val;
_tmp1C4=_tmp1C3.f1;_tmp1C5=*_tmp1C4;_tmp1C6=_tmp1C5.name;_tmp1C7=_tmp1C5.is_extensible;
_tmp1C8=_tmp1C3.f2;_tmp1C9=*_tmp1C8;_tmp1CA=_tmp1C9.name;_LL115: {const char*
_tmp731;const char*_tmp730;struct Cyc_PP_Doc*_tmp1CB=Cyc_PP_text(_tmp1C7?(_tmp731="@extensible datatype ",
_tag_dyneither(_tmp731,sizeof(char),22)):((_tmp730="datatype ",_tag_dyneither(
_tmp730,sizeof(char),10))));{const char*_tmp734;struct Cyc_PP_Doc*_tmp733[4];s=((
_tmp733[3]=Cyc_Absynpp_qvar2doc(_tmp1CA),((_tmp733[2]=Cyc_PP_text(((_tmp734=".",
_tag_dyneither(_tmp734,sizeof(char),2)))),((_tmp733[1]=Cyc_Absynpp_qvar2doc(
_tmp1C6),((_tmp733[0]=_tmp1CB,Cyc_PP_cat(_tag_dyneither(_tmp733,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL111;}_LL111:;}goto _LLCF;_LLE0: {struct Cyc_Absyn_IntType_struct*
_tmp168=(struct Cyc_Absyn_IntType_struct*)_tmp154;if(_tmp168->tag != 6)goto _LLE2;
else{_tmp169=_tmp168->f1;_tmp16A=_tmp168->f2;}}_LLE1: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;switch(_tmp169){case Cyc_Absyn_None: _LL116: goto _LL117;case
Cyc_Absyn_Signed: _LL117:{const char*_tmp735;sns=((_tmp735="",_tag_dyneither(
_tmp735,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL118:{const char*
_tmp736;sns=((_tmp736="unsigned ",_tag_dyneither(_tmp736,sizeof(char),10)));}
break;}switch(_tmp16A){case Cyc_Absyn_Char_sz: _LL11A: switch(_tmp169){case Cyc_Absyn_None:
_LL11C:{const char*_tmp737;sns=((_tmp737="",_tag_dyneither(_tmp737,sizeof(char),1)));}
break;case Cyc_Absyn_Signed: _LL11D:{const char*_tmp738;sns=((_tmp738="signed ",
_tag_dyneither(_tmp738,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL11E:{
const char*_tmp739;sns=((_tmp739="unsigned ",_tag_dyneither(_tmp739,sizeof(char),
10)));}break;}{const char*_tmp73A;ts=((_tmp73A="char",_tag_dyneither(_tmp73A,
sizeof(char),5)));}break;case Cyc_Absyn_Short_sz: _LL11B:{const char*_tmp73B;ts=((
_tmp73B="short",_tag_dyneither(_tmp73B,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz:
_LL120:{const char*_tmp73C;ts=((_tmp73C="int",_tag_dyneither(_tmp73C,sizeof(char),
4)));}break;case Cyc_Absyn_Long_sz: _LL121:{const char*_tmp73D;ts=((_tmp73D="long",
_tag_dyneither(_tmp73D,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL122:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL124:{const char*_tmp73E;
ts=((_tmp73E="long long",_tag_dyneither(_tmp73E,sizeof(char),10)));}break;case
Cyc_Cyclone_Vc_c: _LL125:{const char*_tmp73F;ts=((_tmp73F="__int64",_tag_dyneither(
_tmp73F,sizeof(char),8)));}break;}break;}{const char*_tmp744;void*_tmp743[2];
struct Cyc_String_pa_struct _tmp742;struct Cyc_String_pa_struct _tmp741;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp741.tag=0,((_tmp741.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ts),((_tmp742.tag=0,((_tmp742.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sns),((_tmp743[0]=& _tmp742,((_tmp743[1]=& _tmp741,Cyc_aprintf(((
_tmp744="%s%s",_tag_dyneither(_tmp744,sizeof(char),5))),_tag_dyneither(_tmp743,
sizeof(void*),2)))))))))))))));}goto _LLCF;}_LLE2: {struct Cyc_Absyn_FloatType_struct*
_tmp16B=(struct Cyc_Absyn_FloatType_struct*)_tmp154;if(_tmp16B->tag != 7)goto _LLE4;}
_LLE3:{const char*_tmp745;s=Cyc_PP_text(((_tmp745="float",_tag_dyneither(_tmp745,
sizeof(char),6))));}goto _LLCF;_LLE4: {struct Cyc_Absyn_DoubleType_struct*_tmp16C=(
struct Cyc_Absyn_DoubleType_struct*)_tmp154;if(_tmp16C->tag != 8)goto _LLE6;else{
_tmp16D=_tmp16C->f1;}}_LLE5: if(_tmp16D){const char*_tmp746;s=Cyc_PP_text(((
_tmp746="long double",_tag_dyneither(_tmp746,sizeof(char),12))));}else{const char*
_tmp747;s=Cyc_PP_text(((_tmp747="double",_tag_dyneither(_tmp747,sizeof(char),7))));}
goto _LLCF;_LLE6: {struct Cyc_Absyn_TupleType_struct*_tmp16E=(struct Cyc_Absyn_TupleType_struct*)
_tmp154;if(_tmp16E->tag != 11)goto _LLE8;else{_tmp16F=_tmp16E->f1;}}_LLE7:{struct
Cyc_PP_Doc*_tmp748[2];s=((_tmp748[1]=Cyc_Absynpp_args2doc(_tmp16F),((_tmp748[0]=
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp748,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LLCF;_LLE8: {struct Cyc_Absyn_AggrType_struct*_tmp170=(struct Cyc_Absyn_AggrType_struct*)
_tmp154;if(_tmp170->tag != 12)goto _LLEA;else{_tmp171=_tmp170->f1;_tmp172=_tmp171.aggr_info;
_tmp173=_tmp171.targs;}}_LLE9: {enum Cyc_Absyn_AggrKind _tmp1E4;struct _tuple0*
_tmp1E5;struct _tuple9 _tmp1E3=Cyc_Absyn_aggr_kinded_name(_tmp172);_tmp1E4=_tmp1E3.f1;
_tmp1E5=_tmp1E3.f2;{struct Cyc_PP_Doc*_tmp749[3];s=((_tmp749[2]=Cyc_Absynpp_tps2doc(
_tmp173),((_tmp749[1]=Cyc_Absynpp_qvar2doc(_tmp1E5),((_tmp749[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp1E4),Cyc_PP_cat(_tag_dyneither(_tmp749,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCF;}_LLEA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp174=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp154;if(_tmp174->tag != 13)goto _LLEC;else{_tmp175=_tmp174->f1;_tmp176=_tmp174->f2;}}
_LLEB:{struct Cyc_PP_Doc*_tmp74A[4];s=((_tmp74A[3]=Cyc_Absynpp_rb(),((_tmp74A[2]=
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp176)),((_tmp74A[1]=Cyc_Absynpp_lb(),((
_tmp74A[0]=Cyc_Absynpp_aggr_kind2doc(_tmp175),Cyc_PP_cat(_tag_dyneither(_tmp74A,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCF;_LLEC: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp177=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp154;if(_tmp177->tag != 15)goto
_LLEE;else{_tmp178=_tmp177->f1;}}_LLED:{const char*_tmp74D;struct Cyc_PP_Doc*
_tmp74C[4];s=((_tmp74C[3]=Cyc_Absynpp_rb(),((_tmp74C[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(
_tmp178)),((_tmp74C[1]=Cyc_Absynpp_lb(),((_tmp74C[0]=Cyc_PP_text(((_tmp74D="enum ",
_tag_dyneither(_tmp74D,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp74C,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCF;_LLEE: {struct Cyc_Absyn_EnumType_struct*
_tmp179=(struct Cyc_Absyn_EnumType_struct*)_tmp154;if(_tmp179->tag != 14)goto _LLF0;
else{_tmp17A=_tmp179->f1;}}_LLEF:{const char*_tmp750;struct Cyc_PP_Doc*_tmp74F[2];
s=((_tmp74F[1]=Cyc_Absynpp_qvar2doc(_tmp17A),((_tmp74F[0]=Cyc_PP_text(((_tmp750="enum ",
_tag_dyneither(_tmp750,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp74F,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCF;_LLF0: {struct Cyc_Absyn_ValueofType_struct*
_tmp17B=(struct Cyc_Absyn_ValueofType_struct*)_tmp154;if(_tmp17B->tag != 19)goto
_LLF2;else{_tmp17C=_tmp17B->f1;}}_LLF1:{const char*_tmp755;const char*_tmp754;
struct Cyc_PP_Doc*_tmp753[3];s=((_tmp753[2]=Cyc_PP_text(((_tmp754=")",
_tag_dyneither(_tmp754,sizeof(char),2)))),((_tmp753[1]=Cyc_Absynpp_exp2doc(
_tmp17C),((_tmp753[0]=Cyc_PP_text(((_tmp755="valueof_t(",_tag_dyneither(_tmp755,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp753,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLCF;_LLF2: {struct Cyc_Absyn_TypedefType_struct*_tmp17D=(struct
Cyc_Absyn_TypedefType_struct*)_tmp154;if(_tmp17D->tag != 18)goto _LLF4;else{
_tmp17E=_tmp17D->f1;_tmp17F=_tmp17D->f2;_tmp180=_tmp17D->f3;}}_LLF3:{struct Cyc_PP_Doc*
_tmp756[2];s=((_tmp756[1]=Cyc_Absynpp_tps2doc(_tmp17F),((_tmp756[0]=Cyc_Absynpp_qvar2doc(
_tmp17E),Cyc_PP_cat(_tag_dyneither(_tmp756,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLCF;_LLF4: {struct Cyc_Absyn_RgnHandleType_struct*_tmp181=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp154;if(_tmp181->tag != 16)goto _LLF6;else{_tmp182=(void*)_tmp181->f1;}}_LLF5:{
const char*_tmp75B;const char*_tmp75A;struct Cyc_PP_Doc*_tmp759[3];s=((_tmp759[2]=
Cyc_PP_text(((_tmp75A=">",_tag_dyneither(_tmp75A,sizeof(char),2)))),((_tmp759[1]=
Cyc_Absynpp_rgn2doc(_tmp182),((_tmp759[0]=Cyc_PP_text(((_tmp75B="region_t<",
_tag_dyneither(_tmp75B,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp759,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCF;_LLF6: {struct Cyc_Absyn_DynRgnType_struct*
_tmp183=(struct Cyc_Absyn_DynRgnType_struct*)_tmp154;if(_tmp183->tag != 17)goto
_LLF8;else{_tmp184=(void*)_tmp183->f1;_tmp185=(void*)_tmp183->f2;}}_LLF7:{const
char*_tmp762;const char*_tmp761;const char*_tmp760;struct Cyc_PP_Doc*_tmp75F[5];s=((
_tmp75F[4]=Cyc_PP_text(((_tmp760=">",_tag_dyneither(_tmp760,sizeof(char),2)))),((
_tmp75F[3]=Cyc_Absynpp_rgn2doc(_tmp185),((_tmp75F[2]=Cyc_PP_text(((_tmp761=",",
_tag_dyneither(_tmp761,sizeof(char),2)))),((_tmp75F[1]=Cyc_Absynpp_rgn2doc(
_tmp184),((_tmp75F[0]=Cyc_PP_text(((_tmp762="dynregion_t<",_tag_dyneither(
_tmp762,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp75F,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLCF;_LLF8: {struct Cyc_Absyn_TagType_struct*_tmp186=(struct
Cyc_Absyn_TagType_struct*)_tmp154;if(_tmp186->tag != 20)goto _LLFA;else{_tmp187=(
void*)_tmp186->f1;}}_LLF9:{const char*_tmp767;const char*_tmp766;struct Cyc_PP_Doc*
_tmp765[3];s=((_tmp765[2]=Cyc_PP_text(((_tmp766=">",_tag_dyneither(_tmp766,
sizeof(char),2)))),((_tmp765[1]=Cyc_Absynpp_typ2doc(_tmp187),((_tmp765[0]=Cyc_PP_text(((
_tmp767="tag_t<",_tag_dyneither(_tmp767,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp765,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCF;_LLFA: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp188=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp154;if(_tmp188->tag != 22)goto _LLFC;}_LLFB: goto _LLFD;_LLFC: {struct Cyc_Absyn_HeapRgn_struct*
_tmp189=(struct Cyc_Absyn_HeapRgn_struct*)_tmp154;if(_tmp189->tag != 21)goto _LLFE;}
_LLFD: goto _LLFF;_LLFE: {struct Cyc_Absyn_RefCntRgn_struct*_tmp18A=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp154;if(_tmp18A->tag != 23)goto _LL100;}_LLFF: s=Cyc_Absynpp_rgn2doc(t);goto
_LLCF;_LL100: {struct Cyc_Absyn_RgnsEff_struct*_tmp18B=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp154;if(_tmp18B->tag != 26)goto _LL102;else{_tmp18C=(void*)_tmp18B->f1;}}_LL101:{
const char*_tmp76C;const char*_tmp76B;struct Cyc_PP_Doc*_tmp76A[3];s=((_tmp76A[2]=
Cyc_PP_text(((_tmp76B=")",_tag_dyneither(_tmp76B,sizeof(char),2)))),((_tmp76A[1]=
Cyc_Absynpp_typ2doc(_tmp18C),((_tmp76A[0]=Cyc_PP_text(((_tmp76C="regions(",
_tag_dyneither(_tmp76C,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp76A,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCF;_LL102: {struct Cyc_Absyn_AccessEff_struct*
_tmp18D=(struct Cyc_Absyn_AccessEff_struct*)_tmp154;if(_tmp18D->tag != 24)goto
_LL104;}_LL103: goto _LL105;_LL104: {struct Cyc_Absyn_JoinEff_struct*_tmp18E=(
struct Cyc_Absyn_JoinEff_struct*)_tmp154;if(_tmp18E->tag != 25)goto _LL106;}_LL105:
s=Cyc_Absynpp_eff2doc(t);goto _LLCF;_LL106: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp18F=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp154;if(_tmp18F->tag != 27)goto
_LL108;else{_tmp190=_tmp18F->f1;_tmp191=*_tmp190;_tmp192=_tmp191.r;{struct Cyc_Absyn_Aggr_td_struct*
_tmp193=(struct Cyc_Absyn_Aggr_td_struct*)_tmp192;if(_tmp193->tag != 0)goto _LL108;
else{_tmp194=_tmp193->f1;}};}}_LL107: s=Cyc_Absynpp_aggrdecl2doc(_tmp194);goto
_LLCF;_LL108: {struct Cyc_Absyn_TypeDeclType_struct*_tmp195=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp154;if(_tmp195->tag != 27)goto _LL10A;else{_tmp196=_tmp195->f1;_tmp197=*
_tmp196;_tmp198=_tmp197.r;{struct Cyc_Absyn_Enum_td_struct*_tmp199=(struct Cyc_Absyn_Enum_td_struct*)
_tmp198;if(_tmp199->tag != 1)goto _LL10A;else{_tmp19A=_tmp199->f1;}};}}_LL109: s=
Cyc_Absynpp_enumdecl2doc(_tmp19A);goto _LLCF;_LL10A: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp19B=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp154;if(_tmp19B->tag != 27)goto
_LLCF;else{_tmp19C=_tmp19B->f1;_tmp19D=*_tmp19C;_tmp19E=_tmp19D.r;{struct Cyc_Absyn_Datatype_td_struct*
_tmp19F=(struct Cyc_Absyn_Datatype_td_struct*)_tmp19E;if(_tmp19F->tag != 2)goto
_LLCF;else{_tmp1A0=_tmp19F->f1;}};}}_LL10B: s=Cyc_Absynpp_datatypedecl2doc(
_tmp1A0);goto _LLCF;_LLCF:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct
Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct
_dyneither_ptr*)vo->v));}struct _tuple16{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(
struct _tuple16*cmp){struct _tuple16 _tmp1FE;void*_tmp1FF;void*_tmp200;struct
_tuple16*_tmp1FD=cmp;_tmp1FE=*_tmp1FD;_tmp1FF=_tmp1FE.f1;_tmp200=_tmp1FE.f2;{
const char*_tmp76F;struct Cyc_PP_Doc*_tmp76E[3];return(_tmp76E[2]=Cyc_Absynpp_rgn2doc(
_tmp200),((_tmp76E[1]=Cyc_PP_text(((_tmp76F=" > ",_tag_dyneither(_tmp76F,sizeof(
char),4)))),((_tmp76E[0]=Cyc_Absynpp_rgn2doc(_tmp1FF),Cyc_PP_cat(_tag_dyneither(
_tmp76E,sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List*po){const char*_tmp772;const char*_tmp771;const char*_tmp770;
return Cyc_PP_group(((_tmp770="",_tag_dyneither(_tmp770,sizeof(char),1))),((
_tmp771="",_tag_dyneither(_tmp771,sizeof(char),1))),((_tmp772=",",_tag_dyneither(
_tmp772,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple7*t){struct Cyc_Core_Opt*
_tmp773;struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:((_tmp773=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp773)),((_tmp773->v=Cyc_PP_text(*((struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v)),_tmp773))));return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp207=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs){const char*_tmp776;struct Cyc_List_List*
_tmp775;_tmp207=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp207,((_tmp775=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp775)),((_tmp775->hd=Cyc_PP_text(((_tmp776="...",_tag_dyneither(
_tmp776,sizeof(char),4)))),((_tmp775->tl=0,_tmp775)))))));}else{if(cyc_varargs != 
0){const char*_tmp77F;const char*_tmp77E;const char*_tmp77D;struct _tuple7*_tmp77C;
struct Cyc_PP_Doc*_tmp77B[3];struct Cyc_PP_Doc*_tmp20A=(_tmp77B[2]=Cyc_Absynpp_funarg2doc(((
_tmp77C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp77C)),((_tmp77C->f1=
cyc_varargs->name,((_tmp77C->f2=cyc_varargs->tq,((_tmp77C->f3=cyc_varargs->type,
_tmp77C))))))))),((_tmp77B[1]=cyc_varargs->inject?Cyc_PP_text(((_tmp77D=" inject ",
_tag_dyneither(_tmp77D,sizeof(char),9)))): Cyc_PP_text(((_tmp77E=" ",
_tag_dyneither(_tmp77E,sizeof(char),2)))),((_tmp77B[0]=Cyc_PP_text(((_tmp77F="...",
_tag_dyneither(_tmp77F,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp77B,
sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*_tmp780;_tmp207=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp207,((_tmp780=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp780)),((
_tmp780->hd=_tmp20A,((_tmp780->tl=0,_tmp780)))))));}}{const char*_tmp783;const
char*_tmp782;const char*_tmp781;struct Cyc_PP_Doc*_tmp211=Cyc_PP_group(((_tmp781="",
_tag_dyneither(_tmp781,sizeof(char),1))),((_tmp782="",_tag_dyneither(_tmp782,
sizeof(char),1))),((_tmp783=",",_tag_dyneither(_tmp783,sizeof(char),2))),_tmp207);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp786;struct Cyc_PP_Doc*
_tmp785[3];_tmp211=((_tmp785[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp785[
1]=Cyc_PP_text(((_tmp786=";",_tag_dyneither(_tmp786,sizeof(char),2)))),((_tmp785[
0]=_tmp211,Cyc_PP_cat(_tag_dyneither(_tmp785,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp789;struct Cyc_PP_Doc*_tmp788[3];_tmp211=((_tmp788[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp788[1]=Cyc_PP_text(((_tmp789=":",
_tag_dyneither(_tmp789,sizeof(char),2)))),((_tmp788[0]=_tmp211,Cyc_PP_cat(
_tag_dyneither(_tmp788,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp78E;
const char*_tmp78D;struct Cyc_PP_Doc*_tmp78C[3];return(_tmp78C[2]=Cyc_PP_text(((
_tmp78D=")",_tag_dyneither(_tmp78D,sizeof(char),2)))),((_tmp78C[1]=_tmp211,((
_tmp78C[0]=Cyc_PP_text(((_tmp78E="(",_tag_dyneither(_tmp78E,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp78C,sizeof(struct Cyc_PP_Doc*),3)))))));};};}struct
_tuple7*Cyc_Absynpp_arg_mk_opt(struct _tuple11*arg){struct Cyc_Core_Opt*_tmp791;
struct _tuple7*_tmp790;return(_tmp790=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp790)),((_tmp790->f1=((_tmp791=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp791)),((_tmp791->v=(*arg).f1,_tmp791)))),((_tmp790->f2=(*arg).f2,((
_tmp790->f3=(*arg).f3,_tmp790)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(
struct _dyneither_ptr*v){return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*q){struct Cyc_List_List*_tmp21E=0;int match;{union Cyc_Absyn_Nmspace
_tmp21F=(*q).f1;int _tmp220;struct Cyc_List_List*_tmp221;struct Cyc_List_List*
_tmp222;struct Cyc_List_List*_tmp223;_LL128: if((_tmp21F.Loc_n).tag != 4)goto _LL12A;
_tmp220=(int)(_tmp21F.Loc_n).val;_LL129: _tmp221=0;goto _LL12B;_LL12A: if((_tmp21F.Rel_n).tag
!= 1)goto _LL12C;_tmp221=(struct Cyc_List_List*)(_tmp21F.Rel_n).val;_LL12B: match=0;
_tmp21E=_tmp221;goto _LL127;_LL12C: if((_tmp21F.C_n).tag != 3)goto _LL12E;_tmp222=(
struct Cyc_List_List*)(_tmp21F.C_n).val;_LL12D: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp222,Cyc_Absynpp_curr_namespace);
goto _LL127;_LL12E: if((_tmp21F.Abs_n).tag != 2)goto _LL127;_tmp223=(struct Cyc_List_List*)(
_tmp21F.Abs_n).val;_LL12F: match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp223,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp792;_tmp21E=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp792=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp792)),((_tmp792->hd=
Cyc_Absynpp_cyc_stringptr,((_tmp792->tl=_tmp223,_tmp792))))): _tmp223;}goto _LL127;
_LL127:;}if(Cyc_Absynpp_qvar_to_Cids){const char*_tmp794;struct Cyc_List_List*
_tmp793;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp21E,((_tmp793=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp793)),((_tmp793->hd=(*q).f2,((
_tmp793->tl=0,_tmp793))))))),((_tmp794="_",_tag_dyneither(_tmp794,sizeof(char),2))));}
else{if(match)return*(*q).f2;else{const char*_tmp796;struct Cyc_List_List*_tmp795;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp21E,((_tmp795=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp795)),((_tmp795->hd=(*q).f2,((_tmp795->tl=0,_tmp795))))))),((_tmp796="::",
_tag_dyneither(_tmp796,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*
Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct _dyneither_ptr _tmp229=Cyc_Absynpp_qvar2string(
q);if(Cyc_PP_tex_output){const char*_tmp798;const char*_tmp797;return Cyc_PP_text_width((
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp797="\\textbf{",
_tag_dyneither(_tmp797,sizeof(char),9))),(struct _dyneither_ptr)_tmp229),((
_tmp798="}",_tag_dyneither(_tmp798,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp229));}else{return Cyc_PP_text(_tmp229);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)
return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace
_tmp22C=(*v).f1;int _tmp22D;struct Cyc_List_List*_tmp22E;struct Cyc_List_List*
_tmp22F;struct Cyc_List_List*_tmp230;_LL131: if((_tmp22C.Loc_n).tag != 4)goto _LL133;
_tmp22D=(int)(_tmp22C.Loc_n).val;_LL132: goto _LL134;_LL133: if((_tmp22C.Rel_n).tag
!= 1)goto _LL135;_tmp22E=(struct Cyc_List_List*)(_tmp22C.Rel_n).val;if(_tmp22E != 0)
goto _LL135;_LL134: return*(*v).f2;_LL135: if((_tmp22C.C_n).tag != 3)goto _LL137;
_tmp22F=(struct Cyc_List_List*)(_tmp22C.C_n).val;_LL136: _tmp230=_tmp22F;goto
_LL138;_LL137: if((_tmp22C.Abs_n).tag != 2)goto _LL139;_tmp230=(struct Cyc_List_List*)(
_tmp22C.Abs_n).val;_LL138: if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_strptrcmp,_tmp230,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto
_LL13A;}_LL139:;_LL13A: {const char*_tmp799;return(struct _dyneither_ptr)Cyc_strconcat(((
_tmp799="/* bad namespace : */ ",_tag_dyneither(_tmp799,sizeof(char),23))),(
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL130:;}else{return*(*v).f2;}}
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){return Cyc_PP_text(
Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v){struct _dyneither_ptr _tmp232=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){const char*_tmp79B;const char*_tmp79A;return Cyc_PP_text_width((
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp79A="\\textbf{",
_tag_dyneither(_tmp79A,sizeof(char),9))),(struct _dyneither_ptr)_tmp232),((
_tmp79B="}",_tag_dyneither(_tmp79B,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp232));}else{return Cyc_PP_text(_tmp232);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmp235=e->r;enum Cyc_Absyn_Primop _tmp23A;struct Cyc_Absyn_Exp*
_tmp245;struct Cyc_Absyn_Exp*_tmp247;_LL13C: {struct Cyc_Absyn_Const_e_struct*
_tmp236=(struct Cyc_Absyn_Const_e_struct*)_tmp235;if(_tmp236->tag != 0)goto _LL13E;}
_LL13D: goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_struct*_tmp237=(struct Cyc_Absyn_Var_e_struct*)
_tmp235;if(_tmp237->tag != 1)goto _LL140;}_LL13F: goto _LL141;_LL140: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp238=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp235;if(_tmp238->tag != 2)goto
_LL142;}_LL141: return 10000;_LL142: {struct Cyc_Absyn_Primop_e_struct*_tmp239=(
struct Cyc_Absyn_Primop_e_struct*)_tmp235;if(_tmp239->tag != 3)goto _LL144;else{
_tmp23A=_tmp239->f1;}}_LL143: switch(_tmp23A){case Cyc_Absyn_Plus: _LL18E: return 100;
case Cyc_Absyn_Times: _LL18F: return 110;case Cyc_Absyn_Minus: _LL190: return 100;case
Cyc_Absyn_Div: _LL191: goto _LL192;case Cyc_Absyn_Mod: _LL192: return 110;case Cyc_Absyn_Eq:
_LL193: goto _LL194;case Cyc_Absyn_Neq: _LL194: return 70;case Cyc_Absyn_Gt: _LL195: goto
_LL196;case Cyc_Absyn_Lt: _LL196: goto _LL197;case Cyc_Absyn_Gte: _LL197: goto _LL198;
case Cyc_Absyn_Lte: _LL198: return 80;case Cyc_Absyn_Not: _LL199: goto _LL19A;case Cyc_Absyn_Bitnot:
_LL19A: return 130;case Cyc_Absyn_Bitand: _LL19B: return 60;case Cyc_Absyn_Bitor: _LL19C:
return 40;case Cyc_Absyn_Bitxor: _LL19D: return 50;case Cyc_Absyn_Bitlshift: _LL19E:
return 90;case Cyc_Absyn_Bitlrshift: _LL19F: return 80;case Cyc_Absyn_Bitarshift:
_LL1A0: return 80;case Cyc_Absyn_Numelts: _LL1A1: return 140;}_LL144: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp23B=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp235;if(_tmp23B->tag != 4)goto
_LL146;}_LL145: return 20;_LL146: {struct Cyc_Absyn_Increment_e_struct*_tmp23C=(
struct Cyc_Absyn_Increment_e_struct*)_tmp235;if(_tmp23C->tag != 5)goto _LL148;}
_LL147: return 130;_LL148: {struct Cyc_Absyn_Conditional_e_struct*_tmp23D=(struct
Cyc_Absyn_Conditional_e_struct*)_tmp235;if(_tmp23D->tag != 6)goto _LL14A;}_LL149:
return 30;_LL14A: {struct Cyc_Absyn_And_e_struct*_tmp23E=(struct Cyc_Absyn_And_e_struct*)
_tmp235;if(_tmp23E->tag != 7)goto _LL14C;}_LL14B: return 35;_LL14C: {struct Cyc_Absyn_Or_e_struct*
_tmp23F=(struct Cyc_Absyn_Or_e_struct*)_tmp235;if(_tmp23F->tag != 8)goto _LL14E;}
_LL14D: return 30;_LL14E: {struct Cyc_Absyn_SeqExp_e_struct*_tmp240=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp235;if(_tmp240->tag != 9)goto _LL150;}_LL14F: return 10;_LL150: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp241=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp235;if(_tmp241->tag != 10)goto
_LL152;}_LL151: goto _LL153;_LL152: {struct Cyc_Absyn_FnCall_e_struct*_tmp242=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp235;if(_tmp242->tag != 11)goto _LL154;}_LL153:
return 140;_LL154: {struct Cyc_Absyn_Throw_e_struct*_tmp243=(struct Cyc_Absyn_Throw_e_struct*)
_tmp235;if(_tmp243->tag != 12)goto _LL156;}_LL155: return 130;_LL156: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp244=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp235;if(_tmp244->tag != 13)
goto _LL158;else{_tmp245=_tmp244->f1;}}_LL157: return Cyc_Absynpp_exp_prec(_tmp245);
_LL158: {struct Cyc_Absyn_Instantiate_e_struct*_tmp246=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp235;if(_tmp246->tag != 14)goto _LL15A;else{_tmp247=_tmp246->f1;}}_LL159: return
Cyc_Absynpp_exp_prec(_tmp247);_LL15A: {struct Cyc_Absyn_Cast_e_struct*_tmp248=(
struct Cyc_Absyn_Cast_e_struct*)_tmp235;if(_tmp248->tag != 15)goto _LL15C;}_LL15B:
return 120;_LL15C: {struct Cyc_Absyn_New_e_struct*_tmp249=(struct Cyc_Absyn_New_e_struct*)
_tmp235;if(_tmp249->tag != 17)goto _LL15E;}_LL15D: return 15;_LL15E: {struct Cyc_Absyn_Address_e_struct*
_tmp24A=(struct Cyc_Absyn_Address_e_struct*)_tmp235;if(_tmp24A->tag != 16)goto
_LL160;}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp24B=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp235;if(_tmp24B->tag != 18)goto _LL162;}
_LL161: goto _LL163;_LL162: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp24C=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp235;if(_tmp24C->tag != 19)goto _LL164;}_LL163: goto _LL165;_LL164: {struct Cyc_Absyn_Valueof_e_struct*
_tmp24D=(struct Cyc_Absyn_Valueof_e_struct*)_tmp235;if(_tmp24D->tag != 39)goto
_LL166;}_LL165: goto _LL167;_LL166: {struct Cyc_Absyn_Asm_e_struct*_tmp24E=(struct
Cyc_Absyn_Asm_e_struct*)_tmp235;if(_tmp24E->tag != 40)goto _LL168;}_LL167: goto
_LL169;_LL168: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp24F=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp235;if(_tmp24F->tag != 38)goto _LL16A;}_LL169: goto _LL16B;_LL16A: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp250=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp235;if(_tmp250->tag != 20)goto
_LL16C;}_LL16B: goto _LL16D;_LL16C: {struct Cyc_Absyn_Deref_e_struct*_tmp251=(
struct Cyc_Absyn_Deref_e_struct*)_tmp235;if(_tmp251->tag != 21)goto _LL16E;}_LL16D:
return 130;_LL16E: {struct Cyc_Absyn_AggrMember_e_struct*_tmp252=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp235;if(_tmp252->tag != 22)goto _LL170;}_LL16F: goto _LL171;_LL170: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp253=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp235;if(_tmp253->tag != 23)goto
_LL172;}_LL171: goto _LL173;_LL172: {struct Cyc_Absyn_Subscript_e_struct*_tmp254=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp235;if(_tmp254->tag != 24)goto _LL174;}
_LL173: return 140;_LL174: {struct Cyc_Absyn_Tuple_e_struct*_tmp255=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp235;if(_tmp255->tag != 25)goto _LL176;}_LL175: return 150;_LL176: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp256=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp235;if(_tmp256->tag != 26)goto
_LL178;}_LL177: goto _LL179;_LL178: {struct Cyc_Absyn_Array_e_struct*_tmp257=(
struct Cyc_Absyn_Array_e_struct*)_tmp235;if(_tmp257->tag != 27)goto _LL17A;}_LL179:
goto _LL17B;_LL17A: {struct Cyc_Absyn_Comprehension_e_struct*_tmp258=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp235;if(_tmp258->tag != 28)goto _LL17C;}_LL17B: goto _LL17D;_LL17C: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp259=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp235;if(_tmp259->tag != 29)goto
_LL17E;}_LL17D: goto _LL17F;_LL17E: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp25A=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp235;if(_tmp25A->tag != 30)goto _LL180;}
_LL17F: goto _LL181;_LL180: {struct Cyc_Absyn_Datatype_e_struct*_tmp25B=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp235;if(_tmp25B->tag != 31)goto _LL182;}_LL181: goto _LL183;_LL182: {struct Cyc_Absyn_Enum_e_struct*
_tmp25C=(struct Cyc_Absyn_Enum_e_struct*)_tmp235;if(_tmp25C->tag != 32)goto _LL184;}
_LL183: goto _LL185;_LL184: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp25D=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp235;if(_tmp25D->tag != 33)goto _LL186;}_LL185: goto _LL187;_LL186: {struct Cyc_Absyn_Malloc_e_struct*
_tmp25E=(struct Cyc_Absyn_Malloc_e_struct*)_tmp235;if(_tmp25E->tag != 34)goto
_LL188;}_LL187: goto _LL189;_LL188: {struct Cyc_Absyn_Swap_e_struct*_tmp25F=(struct
Cyc_Absyn_Swap_e_struct*)_tmp235;if(_tmp25F->tag != 35)goto _LL18A;}_LL189: goto
_LL18B;_LL18A: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp260=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp235;if(_tmp260->tag != 36)goto _LL18C;}_LL18B: return 140;_LL18C: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp261=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp235;if(_tmp261->tag != 37)goto
_LL13B;}_LL18D: return 10000;_LL13B:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct
Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp262=e->r;union Cyc_Absyn_Cnst _tmp264;struct _tuple0*_tmp266;struct
_tuple0*_tmp268;enum Cyc_Absyn_Primop _tmp26A;struct Cyc_List_List*_tmp26B;struct
Cyc_Absyn_Exp*_tmp26D;struct Cyc_Core_Opt*_tmp26E;struct Cyc_Absyn_Exp*_tmp26F;
struct Cyc_Absyn_Exp*_tmp271;enum Cyc_Absyn_Incrementor _tmp272;struct Cyc_Absyn_Exp*
_tmp274;struct Cyc_Absyn_Exp*_tmp275;struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp*
_tmp278;struct Cyc_Absyn_Exp*_tmp279;struct Cyc_Absyn_Exp*_tmp27B;struct Cyc_Absyn_Exp*
_tmp27C;struct Cyc_Absyn_Exp*_tmp27E;struct Cyc_Absyn_Exp*_tmp27F;struct Cyc_Absyn_Exp*
_tmp281;struct Cyc_List_List*_tmp282;struct Cyc_Absyn_Exp*_tmp284;struct Cyc_List_List*
_tmp285;struct Cyc_Absyn_Exp*_tmp287;struct Cyc_Absyn_Exp*_tmp289;struct Cyc_Absyn_Exp*
_tmp28B;void*_tmp28D;struct Cyc_Absyn_Exp*_tmp28E;struct Cyc_Absyn_Exp*_tmp290;
struct Cyc_Absyn_Exp*_tmp292;struct Cyc_Absyn_Exp*_tmp293;void*_tmp295;struct Cyc_Absyn_Exp*
_tmp297;void*_tmp299;int _tmp29B;struct _dyneither_ptr _tmp29C;struct Cyc_Absyn_Exp*
_tmp29E;struct _dyneither_ptr*_tmp29F;void*_tmp2A1;void*_tmp2A2;struct
_dyneither_ptr*_tmp2A4;void*_tmp2A6;void*_tmp2A7;unsigned int _tmp2A9;struct Cyc_Absyn_Exp*
_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AD;struct _dyneither_ptr*_tmp2AE;struct Cyc_Absyn_Exp*
_tmp2B0;struct _dyneither_ptr*_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B3;struct Cyc_Absyn_Exp*
_tmp2B4;struct Cyc_List_List*_tmp2B6;struct _tuple7*_tmp2B8;struct Cyc_List_List*
_tmp2B9;struct Cyc_List_List*_tmp2BB;struct Cyc_Absyn_Vardecl*_tmp2BD;struct Cyc_Absyn_Exp*
_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BF;struct _tuple0*_tmp2C1;struct Cyc_List_List*
_tmp2C2;struct Cyc_List_List*_tmp2C3;struct Cyc_List_List*_tmp2C5;struct Cyc_List_List*
_tmp2C7;struct Cyc_Absyn_Datatypefield*_tmp2C8;struct _tuple0*_tmp2CA;struct
_tuple0*_tmp2CC;struct Cyc_Absyn_MallocInfo _tmp2CE;int _tmp2CF;struct Cyc_Absyn_Exp*
_tmp2D0;void**_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D4;
struct Cyc_Absyn_Exp*_tmp2D5;struct Cyc_Core_Opt*_tmp2D7;struct Cyc_List_List*
_tmp2D8;struct Cyc_Absyn_Stmt*_tmp2DA;_LL1A4: {struct Cyc_Absyn_Const_e_struct*
_tmp263=(struct Cyc_Absyn_Const_e_struct*)_tmp262;if(_tmp263->tag != 0)goto _LL1A6;
else{_tmp264=_tmp263->f1;}}_LL1A5: s=Cyc_Absynpp_cnst2doc(_tmp264);goto _LL1A3;
_LL1A6: {struct Cyc_Absyn_Var_e_struct*_tmp265=(struct Cyc_Absyn_Var_e_struct*)
_tmp262;if(_tmp265->tag != 1)goto _LL1A8;else{_tmp266=_tmp265->f1;}}_LL1A7: _tmp268=
_tmp266;goto _LL1A9;_LL1A8: {struct Cyc_Absyn_UnknownId_e_struct*_tmp267=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp262;if(_tmp267->tag != 2)goto _LL1AA;else{
_tmp268=_tmp267->f1;}}_LL1A9: s=Cyc_Absynpp_qvar2doc(_tmp268);goto _LL1A3;_LL1AA: {
struct Cyc_Absyn_Primop_e_struct*_tmp269=(struct Cyc_Absyn_Primop_e_struct*)
_tmp262;if(_tmp269->tag != 3)goto _LL1AC;else{_tmp26A=_tmp269->f1;_tmp26B=_tmp269->f2;}}
_LL1AB: s=Cyc_Absynpp_primapp2doc(myprec,_tmp26A,_tmp26B);goto _LL1A3;_LL1AC: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp26C=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp262;if(_tmp26C->tag != 4)goto _LL1AE;else{_tmp26D=_tmp26C->f1;_tmp26E=_tmp26C->f2;
_tmp26F=_tmp26C->f3;}}_LL1AD:{const char*_tmp7A0;const char*_tmp79F;struct Cyc_PP_Doc*
_tmp79E[5];s=((_tmp79E[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26F),((_tmp79E[3]=
Cyc_PP_text(((_tmp79F="= ",_tag_dyneither(_tmp79F,sizeof(char),3)))),((_tmp79E[2]=
_tmp26E == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp26E->v),((
_tmp79E[1]=Cyc_PP_text(((_tmp7A0=" ",_tag_dyneither(_tmp7A0,sizeof(char),2)))),((
_tmp79E[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26D),Cyc_PP_cat(_tag_dyneither(
_tmp79E,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1A3;_LL1AE: {struct Cyc_Absyn_Increment_e_struct*
_tmp270=(struct Cyc_Absyn_Increment_e_struct*)_tmp262;if(_tmp270->tag != 5)goto
_LL1B0;else{_tmp271=_tmp270->f1;_tmp272=_tmp270->f2;}}_LL1AF: {struct Cyc_PP_Doc*
_tmp2DE=Cyc_Absynpp_exp2doc_prec(myprec,_tmp271);switch(_tmp272){case Cyc_Absyn_PreInc:
_LL1F8:{const char*_tmp7A3;struct Cyc_PP_Doc*_tmp7A2[2];s=((_tmp7A2[1]=_tmp2DE,((
_tmp7A2[0]=Cyc_PP_text(((_tmp7A3="++",_tag_dyneither(_tmp7A3,sizeof(char),3)))),
Cyc_PP_cat(_tag_dyneither(_tmp7A2,sizeof(struct Cyc_PP_Doc*),2))))));}break;case
Cyc_Absyn_PreDec: _LL1F9:{const char*_tmp7A6;struct Cyc_PP_Doc*_tmp7A5[2];s=((
_tmp7A5[1]=_tmp2DE,((_tmp7A5[0]=Cyc_PP_text(((_tmp7A6="--",_tag_dyneither(
_tmp7A6,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7A5,sizeof(struct Cyc_PP_Doc*),
2))))));}break;case Cyc_Absyn_PostInc: _LL1FA:{const char*_tmp7A9;struct Cyc_PP_Doc*
_tmp7A8[2];s=((_tmp7A8[1]=Cyc_PP_text(((_tmp7A9="++",_tag_dyneither(_tmp7A9,
sizeof(char),3)))),((_tmp7A8[0]=_tmp2DE,Cyc_PP_cat(_tag_dyneither(_tmp7A8,
sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1FB:{const char*
_tmp7AC;struct Cyc_PP_Doc*_tmp7AB[2];s=((_tmp7AB[1]=Cyc_PP_text(((_tmp7AC="--",
_tag_dyneither(_tmp7AC,sizeof(char),3)))),((_tmp7AB[0]=_tmp2DE,Cyc_PP_cat(
_tag_dyneither(_tmp7AB,sizeof(struct Cyc_PP_Doc*),2))))));}break;}goto _LL1A3;}
_LL1B0: {struct Cyc_Absyn_Conditional_e_struct*_tmp273=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp262;if(_tmp273->tag != 6)goto _LL1B2;else{_tmp274=_tmp273->f1;_tmp275=_tmp273->f2;
_tmp276=_tmp273->f3;}}_LL1B1:{const char*_tmp7B1;const char*_tmp7B0;struct Cyc_PP_Doc*
_tmp7AF[5];s=((_tmp7AF[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp276),((_tmp7AF[3]=
Cyc_PP_text(((_tmp7B0=" : ",_tag_dyneither(_tmp7B0,sizeof(char),4)))),((_tmp7AF[
2]=Cyc_Absynpp_exp2doc_prec(0,_tmp275),((_tmp7AF[1]=Cyc_PP_text(((_tmp7B1=" ? ",
_tag_dyneither(_tmp7B1,sizeof(char),4)))),((_tmp7AF[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp274),Cyc_PP_cat(_tag_dyneither(_tmp7AF,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A3;_LL1B2: {struct Cyc_Absyn_And_e_struct*_tmp277=(struct Cyc_Absyn_And_e_struct*)
_tmp262;if(_tmp277->tag != 7)goto _LL1B4;else{_tmp278=_tmp277->f1;_tmp279=_tmp277->f2;}}
_LL1B3:{const char*_tmp7B4;struct Cyc_PP_Doc*_tmp7B3[3];s=((_tmp7B3[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp279),((_tmp7B3[1]=Cyc_PP_text(((_tmp7B4=" && ",_tag_dyneither(_tmp7B4,
sizeof(char),5)))),((_tmp7B3[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp278),Cyc_PP_cat(
_tag_dyneither(_tmp7B3,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1B4: {
struct Cyc_Absyn_Or_e_struct*_tmp27A=(struct Cyc_Absyn_Or_e_struct*)_tmp262;if(
_tmp27A->tag != 8)goto _LL1B6;else{_tmp27B=_tmp27A->f1;_tmp27C=_tmp27A->f2;}}
_LL1B5:{const char*_tmp7B7;struct Cyc_PP_Doc*_tmp7B6[3];s=((_tmp7B6[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp27C),((_tmp7B6[1]=Cyc_PP_text(((_tmp7B7=" || ",_tag_dyneither(_tmp7B7,
sizeof(char),5)))),((_tmp7B6[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27B),Cyc_PP_cat(
_tag_dyneither(_tmp7B6,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1B6: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp27D=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp262;if(_tmp27D->tag != 9)goto _LL1B8;else{_tmp27E=_tmp27D->f1;_tmp27F=_tmp27D->f2;}}
_LL1B7:{const char*_tmp7BE;const char*_tmp7BD;const char*_tmp7BC;struct Cyc_PP_Doc*
_tmp7BB[5];s=((_tmp7BB[4]=Cyc_PP_text(((_tmp7BC=")",_tag_dyneither(_tmp7BC,
sizeof(char),2)))),((_tmp7BB[3]=Cyc_Absynpp_exp2doc(_tmp27F),((_tmp7BB[2]=Cyc_PP_text(((
_tmp7BD=", ",_tag_dyneither(_tmp7BD,sizeof(char),3)))),((_tmp7BB[1]=Cyc_Absynpp_exp2doc(
_tmp27E),((_tmp7BB[0]=Cyc_PP_text(((_tmp7BE="(",_tag_dyneither(_tmp7BE,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7BB,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A3;_LL1B8: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp280=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp262;if(_tmp280->tag != 10)goto _LL1BA;else{_tmp281=_tmp280->f1;_tmp282=_tmp280->f2;}}
_LL1B9:{const char*_tmp7C3;const char*_tmp7C2;struct Cyc_PP_Doc*_tmp7C1[4];s=((
_tmp7C1[3]=Cyc_PP_text(((_tmp7C2=")",_tag_dyneither(_tmp7C2,sizeof(char),2)))),((
_tmp7C1[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp282),((_tmp7C1[1]=Cyc_PP_text(((
_tmp7C3="(",_tag_dyneither(_tmp7C3,sizeof(char),2)))),((_tmp7C1[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp281),Cyc_PP_cat(_tag_dyneither(_tmp7C1,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1A3;_LL1BA: {struct Cyc_Absyn_FnCall_e_struct*_tmp283=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp262;if(_tmp283->tag != 11)goto _LL1BC;else{_tmp284=_tmp283->f1;_tmp285=_tmp283->f2;}}
_LL1BB:{const char*_tmp7C8;const char*_tmp7C7;struct Cyc_PP_Doc*_tmp7C6[4];s=((
_tmp7C6[3]=Cyc_PP_text(((_tmp7C7=")",_tag_dyneither(_tmp7C7,sizeof(char),2)))),((
_tmp7C6[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp285),((_tmp7C6[1]=Cyc_PP_text(((
_tmp7C8="(",_tag_dyneither(_tmp7C8,sizeof(char),2)))),((_tmp7C6[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp284),Cyc_PP_cat(_tag_dyneither(_tmp7C6,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1A3;_LL1BC: {struct Cyc_Absyn_Throw_e_struct*_tmp286=(struct Cyc_Absyn_Throw_e_struct*)
_tmp262;if(_tmp286->tag != 12)goto _LL1BE;else{_tmp287=_tmp286->f1;}}_LL1BD:{const
char*_tmp7CB;struct Cyc_PP_Doc*_tmp7CA[2];s=((_tmp7CA[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp287),((_tmp7CA[0]=Cyc_PP_text(((_tmp7CB="throw ",_tag_dyneither(
_tmp7CB,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp7CA,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL1A3;_LL1BE: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp288=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp262;if(_tmp288->tag != 13)goto _LL1C0;
else{_tmp289=_tmp288->f1;}}_LL1BF: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp289);
goto _LL1A3;_LL1C0: {struct Cyc_Absyn_Instantiate_e_struct*_tmp28A=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp262;if(_tmp28A->tag != 14)goto _LL1C2;else{_tmp28B=_tmp28A->f1;}}_LL1C1: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmp28B);goto _LL1A3;_LL1C2: {struct Cyc_Absyn_Cast_e_struct*_tmp28C=(
struct Cyc_Absyn_Cast_e_struct*)_tmp262;if(_tmp28C->tag != 15)goto _LL1C4;else{
_tmp28D=(void*)_tmp28C->f1;_tmp28E=_tmp28C->f2;}}_LL1C3:{const char*_tmp7D0;const
char*_tmp7CF;struct Cyc_PP_Doc*_tmp7CE[4];s=((_tmp7CE[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp28E),((_tmp7CE[2]=Cyc_PP_text(((_tmp7CF=")",_tag_dyneither(_tmp7CF,
sizeof(char),2)))),((_tmp7CE[1]=Cyc_Absynpp_typ2doc(_tmp28D),((_tmp7CE[0]=Cyc_PP_text(((
_tmp7D0="(",_tag_dyneither(_tmp7D0,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp7CE,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1A3;_LL1C4: {struct Cyc_Absyn_Address_e_struct*
_tmp28F=(struct Cyc_Absyn_Address_e_struct*)_tmp262;if(_tmp28F->tag != 16)goto
_LL1C6;else{_tmp290=_tmp28F->f1;}}_LL1C5:{const char*_tmp7D3;struct Cyc_PP_Doc*
_tmp7D2[2];s=((_tmp7D2[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp290),((_tmp7D2[0]=
Cyc_PP_text(((_tmp7D3="&",_tag_dyneither(_tmp7D3,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp7D2,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1A3;_LL1C6: {
struct Cyc_Absyn_New_e_struct*_tmp291=(struct Cyc_Absyn_New_e_struct*)_tmp262;if(
_tmp291->tag != 17)goto _LL1C8;else{_tmp292=_tmp291->f1;_tmp293=_tmp291->f2;}}
_LL1C7: if(_tmp292 == 0){const char*_tmp7D6;struct Cyc_PP_Doc*_tmp7D5[2];s=((_tmp7D5[
1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp293),((_tmp7D5[0]=Cyc_PP_text(((_tmp7D6="new ",
_tag_dyneither(_tmp7D6,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7D5,
sizeof(struct Cyc_PP_Doc*),2))))));}else{const char*_tmp7DB;const char*_tmp7DA;
struct Cyc_PP_Doc*_tmp7D9[4];s=((_tmp7D9[3]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp293),((_tmp7D9[2]=Cyc_PP_text(((_tmp7DA=") ",_tag_dyneither(_tmp7DA,sizeof(
char),3)))),((_tmp7D9[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp292),((
_tmp7D9[0]=Cyc_PP_text(((_tmp7DB="rnew(",_tag_dyneither(_tmp7DB,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp7D9,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL1A3;_LL1C8: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp294=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp262;if(_tmp294->tag != 18)goto _LL1CA;else{_tmp295=(void*)_tmp294->f1;}}_LL1C9:{
const char*_tmp7E0;const char*_tmp7DF;struct Cyc_PP_Doc*_tmp7DE[3];s=((_tmp7DE[2]=
Cyc_PP_text(((_tmp7DF=")",_tag_dyneither(_tmp7DF,sizeof(char),2)))),((_tmp7DE[1]=
Cyc_Absynpp_typ2doc(_tmp295),((_tmp7DE[0]=Cyc_PP_text(((_tmp7E0="sizeof(",
_tag_dyneither(_tmp7E0,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7DE,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1CA: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp296=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp262;if(_tmp296->tag != 19)goto
_LL1CC;else{_tmp297=_tmp296->f1;}}_LL1CB:{const char*_tmp7E5;const char*_tmp7E4;
struct Cyc_PP_Doc*_tmp7E3[3];s=((_tmp7E3[2]=Cyc_PP_text(((_tmp7E4=")",
_tag_dyneither(_tmp7E4,sizeof(char),2)))),((_tmp7E3[1]=Cyc_Absynpp_exp2doc(
_tmp297),((_tmp7E3[0]=Cyc_PP_text(((_tmp7E5="sizeof(",_tag_dyneither(_tmp7E5,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7E3,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A3;_LL1CC: {struct Cyc_Absyn_Valueof_e_struct*_tmp298=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp262;if(_tmp298->tag != 39)goto _LL1CE;else{_tmp299=(void*)_tmp298->f1;}}_LL1CD:{
const char*_tmp7EA;const char*_tmp7E9;struct Cyc_PP_Doc*_tmp7E8[3];s=((_tmp7E8[2]=
Cyc_PP_text(((_tmp7E9=")",_tag_dyneither(_tmp7E9,sizeof(char),2)))),((_tmp7E8[1]=
Cyc_Absynpp_typ2doc(_tmp299),((_tmp7E8[0]=Cyc_PP_text(((_tmp7EA="valueof(",
_tag_dyneither(_tmp7EA,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7E8,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1CE: {struct Cyc_Absyn_Asm_e_struct*
_tmp29A=(struct Cyc_Absyn_Asm_e_struct*)_tmp262;if(_tmp29A->tag != 40)goto _LL1D0;
else{_tmp29B=_tmp29A->f1;_tmp29C=_tmp29A->f2;}}_LL1CF: if(_tmp29B){const char*
_tmp7F1;const char*_tmp7F0;const char*_tmp7EF;struct Cyc_PP_Doc*_tmp7EE[4];s=((
_tmp7EE[3]=Cyc_PP_text(((_tmp7EF=")",_tag_dyneither(_tmp7EF,sizeof(char),2)))),((
_tmp7EE[2]=Cyc_PP_text(_tmp29C),((_tmp7EE[1]=Cyc_PP_text(((_tmp7F0=" volatile (",
_tag_dyneither(_tmp7F0,sizeof(char),12)))),((_tmp7EE[0]=Cyc_PP_text(((_tmp7F1="__asm__",
_tag_dyneither(_tmp7F1,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7EE,
sizeof(struct Cyc_PP_Doc*),4))))))))));}else{const char*_tmp7F6;const char*_tmp7F5;
struct Cyc_PP_Doc*_tmp7F4[3];s=((_tmp7F4[2]=Cyc_PP_text(((_tmp7F5=")",
_tag_dyneither(_tmp7F5,sizeof(char),2)))),((_tmp7F4[1]=Cyc_PP_text(_tmp29C),((
_tmp7F4[0]=Cyc_PP_text(((_tmp7F6="__asm__(",_tag_dyneither(_tmp7F6,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp7F4,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A3;_LL1D0: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp29D=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp262;if(_tmp29D->tag != 38)goto _LL1D2;else{_tmp29E=_tmp29D->f1;_tmp29F=_tmp29D->f2;}}
_LL1D1:{const char*_tmp7FD;const char*_tmp7FC;const char*_tmp7FB;struct Cyc_PP_Doc*
_tmp7FA[5];s=((_tmp7FA[4]=Cyc_PP_text(((_tmp7FB=")",_tag_dyneither(_tmp7FB,
sizeof(char),2)))),((_tmp7FA[3]=Cyc_PP_textptr(_tmp29F),((_tmp7FA[2]=Cyc_PP_text(((
_tmp7FC=".",_tag_dyneither(_tmp7FC,sizeof(char),2)))),((_tmp7FA[1]=Cyc_Absynpp_exp2doc(
_tmp29E),((_tmp7FA[0]=Cyc_PP_text(((_tmp7FD="tagcheck(",_tag_dyneither(_tmp7FD,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7FA,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL1A3;_LL1D2: {struct Cyc_Absyn_Offsetof_e_struct*_tmp2A0=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp262;if(_tmp2A0->tag != 20)goto _LL1D4;else{
_tmp2A1=(void*)_tmp2A0->f1;_tmp2A2=(void*)_tmp2A0->f2;{struct Cyc_Absyn_StructField_struct*
_tmp2A3=(struct Cyc_Absyn_StructField_struct*)_tmp2A2;if(_tmp2A3->tag != 0)goto
_LL1D4;else{_tmp2A4=_tmp2A3->f1;}};}}_LL1D3:{const char*_tmp804;const char*_tmp803;
const char*_tmp802;struct Cyc_PP_Doc*_tmp801[5];s=((_tmp801[4]=Cyc_PP_text(((
_tmp802=")",_tag_dyneither(_tmp802,sizeof(char),2)))),((_tmp801[3]=Cyc_PP_textptr(
_tmp2A4),((_tmp801[2]=Cyc_PP_text(((_tmp803=",",_tag_dyneither(_tmp803,sizeof(
char),2)))),((_tmp801[1]=Cyc_Absynpp_typ2doc(_tmp2A1),((_tmp801[0]=Cyc_PP_text(((
_tmp804="offsetof(",_tag_dyneither(_tmp804,sizeof(char),10)))),Cyc_PP_cat(
_tag_dyneither(_tmp801,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1A3;
_LL1D4: {struct Cyc_Absyn_Offsetof_e_struct*_tmp2A5=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp262;if(_tmp2A5->tag != 20)goto _LL1D6;else{_tmp2A6=(void*)_tmp2A5->f1;_tmp2A7=(
void*)_tmp2A5->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp2A8=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2A7;if(_tmp2A8->tag != 1)goto _LL1D6;else{_tmp2A9=_tmp2A8->f1;}};}}_LL1D5:{
const char*_tmp812;const char*_tmp811;struct Cyc_Int_pa_struct _tmp810;void*_tmp80F[
1];const char*_tmp80E;const char*_tmp80D;struct Cyc_PP_Doc*_tmp80C[5];s=((_tmp80C[4]=
Cyc_PP_text(((_tmp80D=")",_tag_dyneither(_tmp80D,sizeof(char),2)))),((_tmp80C[3]=
Cyc_PP_text((struct _dyneither_ptr)((_tmp810.tag=1,((_tmp810.f1=(unsigned long)((
int)_tmp2A9),((_tmp80F[0]=& _tmp810,Cyc_aprintf(((_tmp80E="%d",_tag_dyneither(
_tmp80E,sizeof(char),3))),_tag_dyneither(_tmp80F,sizeof(void*),1))))))))),((
_tmp80C[2]=Cyc_PP_text(((_tmp811=",",_tag_dyneither(_tmp811,sizeof(char),2)))),((
_tmp80C[1]=Cyc_Absynpp_typ2doc(_tmp2A6),((_tmp80C[0]=Cyc_PP_text(((_tmp812="offsetof(",
_tag_dyneither(_tmp812,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp80C,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1A3;_LL1D6: {struct Cyc_Absyn_Deref_e_struct*
_tmp2AA=(struct Cyc_Absyn_Deref_e_struct*)_tmp262;if(_tmp2AA->tag != 21)goto _LL1D8;
else{_tmp2AB=_tmp2AA->f1;}}_LL1D7:{const char*_tmp815;struct Cyc_PP_Doc*_tmp814[2];
s=((_tmp814[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AB),((_tmp814[0]=Cyc_PP_text(((
_tmp815="*",_tag_dyneither(_tmp815,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp814,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1A3;_LL1D8: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp2AC=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp262;if(_tmp2AC->tag != 22)goto
_LL1DA;else{_tmp2AD=_tmp2AC->f1;_tmp2AE=_tmp2AC->f2;}}_LL1D9:{const char*_tmp818;
struct Cyc_PP_Doc*_tmp817[3];s=((_tmp817[2]=Cyc_PP_textptr(_tmp2AE),((_tmp817[1]=
Cyc_PP_text(((_tmp818=".",_tag_dyneither(_tmp818,sizeof(char),2)))),((_tmp817[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AD),Cyc_PP_cat(_tag_dyneither(_tmp817,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1DA: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp2AF=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp262;if(_tmp2AF->tag != 23)goto
_LL1DC;else{_tmp2B0=_tmp2AF->f1;_tmp2B1=_tmp2AF->f2;}}_LL1DB:{const char*_tmp81B;
struct Cyc_PP_Doc*_tmp81A[3];s=((_tmp81A[2]=Cyc_PP_textptr(_tmp2B1),((_tmp81A[1]=
Cyc_PP_text(((_tmp81B="->",_tag_dyneither(_tmp81B,sizeof(char),3)))),((_tmp81A[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B0),Cyc_PP_cat(_tag_dyneither(_tmp81A,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1DC: {struct Cyc_Absyn_Subscript_e_struct*
_tmp2B2=(struct Cyc_Absyn_Subscript_e_struct*)_tmp262;if(_tmp2B2->tag != 24)goto
_LL1DE;else{_tmp2B3=_tmp2B2->f1;_tmp2B4=_tmp2B2->f2;}}_LL1DD:{const char*_tmp820;
const char*_tmp81F;struct Cyc_PP_Doc*_tmp81E[4];s=((_tmp81E[3]=Cyc_PP_text(((
_tmp81F="]",_tag_dyneither(_tmp81F,sizeof(char),2)))),((_tmp81E[2]=Cyc_Absynpp_exp2doc(
_tmp2B4),((_tmp81E[1]=Cyc_PP_text(((_tmp820="[",_tag_dyneither(_tmp820,sizeof(
char),2)))),((_tmp81E[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B3),Cyc_PP_cat(
_tag_dyneither(_tmp81E,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1A3;_LL1DE: {
struct Cyc_Absyn_Tuple_e_struct*_tmp2B5=(struct Cyc_Absyn_Tuple_e_struct*)_tmp262;
if(_tmp2B5->tag != 25)goto _LL1E0;else{_tmp2B6=_tmp2B5->f1;}}_LL1DF:{const char*
_tmp825;const char*_tmp824;struct Cyc_PP_Doc*_tmp823[4];s=((_tmp823[3]=Cyc_PP_text(((
_tmp824=")",_tag_dyneither(_tmp824,sizeof(char),2)))),((_tmp823[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp2B6),((_tmp823[1]=Cyc_PP_text(((_tmp825="(",_tag_dyneither(_tmp825,
sizeof(char),2)))),((_tmp823[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(
_tmp823,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1A3;_LL1E0: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp2B7=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp262;if(_tmp2B7->tag != 26)goto
_LL1E2;else{_tmp2B8=_tmp2B7->f1;_tmp2B9=_tmp2B7->f2;}}_LL1E1:{const char*_tmp82C;
const char*_tmp82B;const char*_tmp82A;struct Cyc_PP_Doc*_tmp829[4];s=((_tmp829[3]=
Cyc_Absynpp_group_braces(((_tmp82A=",",_tag_dyneither(_tmp82A,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B9)),((_tmp829[2]=Cyc_PP_text(((_tmp82B=")",
_tag_dyneither(_tmp82B,sizeof(char),2)))),((_tmp829[1]=Cyc_Absynpp_typ2doc((*
_tmp2B8).f3),((_tmp829[0]=Cyc_PP_text(((_tmp82C="(",_tag_dyneither(_tmp82C,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp829,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1A3;_LL1E2: {struct Cyc_Absyn_Array_e_struct*_tmp2BA=(struct Cyc_Absyn_Array_e_struct*)
_tmp262;if(_tmp2BA->tag != 27)goto _LL1E4;else{_tmp2BB=_tmp2BA->f1;}}_LL1E3:{const
char*_tmp82D;s=Cyc_Absynpp_group_braces(((_tmp82D=",",_tag_dyneither(_tmp82D,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BB));}goto _LL1A3;
_LL1E4: {struct Cyc_Absyn_Comprehension_e_struct*_tmp2BC=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp262;if(_tmp2BC->tag != 28)goto _LL1E6;else{_tmp2BD=_tmp2BC->f1;_tmp2BE=_tmp2BC->f2;
_tmp2BF=_tmp2BC->f3;}}_LL1E5:{const char*_tmp834;const char*_tmp833;const char*
_tmp832;struct Cyc_PP_Doc*_tmp831[8];s=((_tmp831[7]=Cyc_Absynpp_rb(),((_tmp831[6]=
Cyc_Absynpp_exp2doc(_tmp2BF),((_tmp831[5]=Cyc_PP_text(((_tmp832=" : ",
_tag_dyneither(_tmp832,sizeof(char),4)))),((_tmp831[4]=Cyc_Absynpp_exp2doc(
_tmp2BE),((_tmp831[3]=Cyc_PP_text(((_tmp833=" < ",_tag_dyneither(_tmp833,sizeof(
char),4)))),((_tmp831[2]=Cyc_PP_text(*(*_tmp2BD->name).f2),((_tmp831[1]=Cyc_PP_text(((
_tmp834="for ",_tag_dyneither(_tmp834,sizeof(char),5)))),((_tmp831[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp831,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL1A3;_LL1E6: {struct Cyc_Absyn_Aggregate_e_struct*_tmp2C0=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp262;if(_tmp2C0->tag != 29)goto _LL1E8;else{_tmp2C1=_tmp2C0->f1;_tmp2C2=_tmp2C0->f2;
_tmp2C3=_tmp2C0->f3;}}_LL1E7: {struct Cyc_List_List*_tmp338=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2C3);{const char*_tmp839;struct Cyc_List_List*_tmp838;struct Cyc_PP_Doc*_tmp837[
2];s=((_tmp837[1]=Cyc_Absynpp_group_braces(((_tmp839=",",_tag_dyneither(_tmp839,
sizeof(char),2))),_tmp2C2 != 0?(_tmp838=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp838)),((_tmp838->hd=Cyc_Absynpp_tps2doc(_tmp2C2),((_tmp838->tl=
_tmp338,_tmp838))))): _tmp338),((_tmp837[0]=Cyc_Absynpp_qvar2doc(_tmp2C1),Cyc_PP_cat(
_tag_dyneither(_tmp837,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1A3;}_LL1E8: {
struct Cyc_Absyn_AnonStruct_e_struct*_tmp2C4=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp262;if(_tmp2C4->tag != 30)goto _LL1EA;else{_tmp2C5=_tmp2C4->f2;}}_LL1E9:{const
char*_tmp83A;s=Cyc_Absynpp_group_braces(((_tmp83A=",",_tag_dyneither(_tmp83A,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C5));}goto _LL1A3;
_LL1EA: {struct Cyc_Absyn_Datatype_e_struct*_tmp2C6=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp262;if(_tmp2C6->tag != 31)goto _LL1EC;else{_tmp2C7=_tmp2C6->f1;_tmp2C8=_tmp2C6->f3;}}
_LL1EB: if(_tmp2C7 == 0)s=Cyc_Absynpp_qvar2doc(_tmp2C8->name);else{const char*
_tmp841;const char*_tmp840;const char*_tmp83F;struct Cyc_PP_Doc*_tmp83E[2];s=((
_tmp83E[1]=Cyc_PP_egroup(((_tmp841="(",_tag_dyneither(_tmp841,sizeof(char),2))),((
_tmp840=")",_tag_dyneither(_tmp840,sizeof(char),2))),((_tmp83F=",",
_tag_dyneither(_tmp83F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,
_tmp2C7)),((_tmp83E[0]=Cyc_Absynpp_qvar2doc(_tmp2C8->name),Cyc_PP_cat(
_tag_dyneither(_tmp83E,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1A3;_LL1EC: {
struct Cyc_Absyn_Enum_e_struct*_tmp2C9=(struct Cyc_Absyn_Enum_e_struct*)_tmp262;
if(_tmp2C9->tag != 32)goto _LL1EE;else{_tmp2CA=_tmp2C9->f1;}}_LL1ED: s=Cyc_Absynpp_qvar2doc(
_tmp2CA);goto _LL1A3;_LL1EE: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp2CB=(struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp262;if(_tmp2CB->tag != 33)goto _LL1F0;else{
_tmp2CC=_tmp2CB->f1;}}_LL1EF: s=Cyc_Absynpp_qvar2doc(_tmp2CC);goto _LL1A3;_LL1F0: {
struct Cyc_Absyn_Malloc_e_struct*_tmp2CD=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp262;if(_tmp2CD->tag != 34)goto _LL1F2;else{_tmp2CE=_tmp2CD->f1;_tmp2CF=_tmp2CE.is_calloc;
_tmp2D0=_tmp2CE.rgn;_tmp2D1=_tmp2CE.elt_type;_tmp2D2=_tmp2CE.num_elts;}}_LL1F1:
if(_tmp2CF){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp2D1)),0);if(_tmp2D0 == 0){const char*_tmp848;const char*_tmp847;const char*
_tmp846;struct Cyc_PP_Doc*_tmp845[5];s=((_tmp845[4]=Cyc_PP_text(((_tmp846=")",
_tag_dyneither(_tmp846,sizeof(char),2)))),((_tmp845[3]=Cyc_Absynpp_exp2doc(st),((
_tmp845[2]=Cyc_PP_text(((_tmp847=",",_tag_dyneither(_tmp847,sizeof(char),2)))),((
_tmp845[1]=Cyc_Absynpp_exp2doc(_tmp2D2),((_tmp845[0]=Cyc_PP_text(((_tmp848="calloc(",
_tag_dyneither(_tmp848,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp845,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp851;const char*
_tmp850;const char*_tmp84F;const char*_tmp84E;struct Cyc_PP_Doc*_tmp84D[7];s=((
_tmp84D[6]=Cyc_PP_text(((_tmp84E=")",_tag_dyneither(_tmp84E,sizeof(char),2)))),((
_tmp84D[5]=Cyc_Absynpp_exp2doc(st),((_tmp84D[4]=Cyc_PP_text(((_tmp84F=",",
_tag_dyneither(_tmp84F,sizeof(char),2)))),((_tmp84D[3]=Cyc_Absynpp_exp2doc(
_tmp2D2),((_tmp84D[2]=Cyc_PP_text(((_tmp850=",",_tag_dyneither(_tmp850,sizeof(
char),2)))),((_tmp84D[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2D0),((
_tmp84D[0]=Cyc_PP_text(((_tmp851="rcalloc(",_tag_dyneither(_tmp851,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp84D,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp2D1 == 0)new_e=_tmp2D2;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp2D1,0),_tmp2D2,0);}if(_tmp2D0 == 0){const char*
_tmp856;const char*_tmp855;struct Cyc_PP_Doc*_tmp854[3];s=((_tmp854[2]=Cyc_PP_text(((
_tmp855=")",_tag_dyneither(_tmp855,sizeof(char),2)))),((_tmp854[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp854[0]=Cyc_PP_text(((_tmp856="malloc(",_tag_dyneither(_tmp856,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp854,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp85D;const char*_tmp85C;const char*_tmp85B;struct Cyc_PP_Doc*
_tmp85A[5];s=((_tmp85A[4]=Cyc_PP_text(((_tmp85B=")",_tag_dyneither(_tmp85B,
sizeof(char),2)))),((_tmp85A[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp85A[2]=Cyc_PP_text(((
_tmp85C=",",_tag_dyneither(_tmp85C,sizeof(char),2)))),((_tmp85A[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2D0),((_tmp85A[0]=Cyc_PP_text(((_tmp85D="rmalloc(",
_tag_dyneither(_tmp85D,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp85A,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL1A3;_LL1F2: {struct Cyc_Absyn_Swap_e_struct*
_tmp2D3=(struct Cyc_Absyn_Swap_e_struct*)_tmp262;if(_tmp2D3->tag != 35)goto _LL1F4;
else{_tmp2D4=_tmp2D3->f1;_tmp2D5=_tmp2D3->f2;}}_LL1F3:{const char*_tmp860;struct
Cyc_PP_Doc*_tmp85F[3];s=((_tmp85F[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D5),((
_tmp85F[1]=Cyc_PP_text(((_tmp860=" :=: ",_tag_dyneither(_tmp860,sizeof(char),6)))),((
_tmp85F[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D4),Cyc_PP_cat(_tag_dyneither(
_tmp85F,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A3;_LL1F4: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp2D6=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp262;if(_tmp2D6->tag != 36)
goto _LL1F6;else{_tmp2D7=_tmp2D6->f1;_tmp2D8=_tmp2D6->f2;}}_LL1F5:{const char*
_tmp861;s=Cyc_Absynpp_group_braces(((_tmp861=",",_tag_dyneither(_tmp861,sizeof(
char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2D8));}goto _LL1A3;
_LL1F6: {struct Cyc_Absyn_StmtExp_e_struct*_tmp2D9=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp262;if(_tmp2D9->tag != 37)goto _LL1A3;else{_tmp2DA=_tmp2D9->f1;}}_LL1F7:{const
char*_tmp866;const char*_tmp865;struct Cyc_PP_Doc*_tmp864[7];s=((_tmp864[6]=Cyc_PP_text(((
_tmp865=")",_tag_dyneither(_tmp865,sizeof(char),2)))),((_tmp864[5]=Cyc_Absynpp_rb(),((
_tmp864[4]=Cyc_PP_blank_doc(),((_tmp864[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2DA,1)),((_tmp864[2]=Cyc_PP_blank_doc(),((_tmp864[1]=Cyc_Absynpp_lb(),((
_tmp864[0]=Cyc_PP_text(((_tmp866="(",_tag_dyneither(_tmp866,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp864,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
goto _LL1A3;_LL1A3:;}if(inprec >= myprec){const char*_tmp86B;const char*_tmp86A;
struct Cyc_PP_Doc*_tmp869[3];s=((_tmp869[2]=Cyc_PP_text(((_tmp86A=")",
_tag_dyneither(_tmp86A,sizeof(char),2)))),((_tmp869[1]=s,((_tmp869[0]=Cyc_PP_text(((
_tmp86B="(",_tag_dyneither(_tmp86B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp869,sizeof(struct Cyc_PP_Doc*),3))))))));}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d){void*_tmp35A=d;struct Cyc_Absyn_Exp*_tmp35C;struct _dyneither_ptr*_tmp35E;
_LL1FE: {struct Cyc_Absyn_ArrayElement_struct*_tmp35B=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp35A;if(_tmp35B->tag != 0)goto _LL200;else{_tmp35C=_tmp35B->f1;}}_LL1FF: {const
char*_tmp870;const char*_tmp86F;struct Cyc_PP_Doc*_tmp86E[3];return(_tmp86E[2]=Cyc_PP_text(((
_tmp86F="]",_tag_dyneither(_tmp86F,sizeof(char),2)))),((_tmp86E[1]=Cyc_Absynpp_exp2doc(
_tmp35C),((_tmp86E[0]=Cyc_PP_text(((_tmp870=".[",_tag_dyneither(_tmp870,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp86E,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL200: {struct Cyc_Absyn_FieldName_struct*_tmp35D=(struct Cyc_Absyn_FieldName_struct*)
_tmp35A;if(_tmp35D->tag != 1)goto _LL1FD;else{_tmp35E=_tmp35D->f1;}}_LL201: {const
char*_tmp873;struct Cyc_PP_Doc*_tmp872[2];return(_tmp872[1]=Cyc_PP_textptr(
_tmp35E),((_tmp872[0]=Cyc_PP_text(((_tmp873=".",_tag_dyneither(_tmp873,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp872,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL1FD:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){if((*de).f1 == 0)
return Cyc_Absynpp_exp2doc((*de).f2);else{const char*_tmp87A;const char*_tmp879;
const char*_tmp878;struct Cyc_PP_Doc*_tmp877[2];return(_tmp877[1]=Cyc_Absynpp_exp2doc((*
de).f2),((_tmp877[0]=Cyc_PP_egroup(((_tmp87A="",_tag_dyneither(_tmp87A,sizeof(
char),1))),((_tmp879="=",_tag_dyneither(_tmp879,sizeof(char),2))),((_tmp878="=",
_tag_dyneither(_tmp878,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),
Cyc_PP_cat(_tag_dyneither(_tmp877,sizeof(struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){const char*_tmp87D;
const char*_tmp87C;const char*_tmp87B;return Cyc_PP_group(((_tmp87B="",
_tag_dyneither(_tmp87B,sizeof(char),1))),((_tmp87C="",_tag_dyneither(_tmp87C,
sizeof(char),1))),((_tmp87D=",",_tag_dyneither(_tmp87D,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst
_tmp36B=c;struct _tuple3 _tmp36C;enum Cyc_Absyn_Sign _tmp36D;char _tmp36E;struct
_tuple4 _tmp36F;enum Cyc_Absyn_Sign _tmp370;short _tmp371;struct _tuple5 _tmp372;
enum Cyc_Absyn_Sign _tmp373;int _tmp374;struct _tuple5 _tmp375;enum Cyc_Absyn_Sign
_tmp376;int _tmp377;struct _tuple5 _tmp378;enum Cyc_Absyn_Sign _tmp379;int _tmp37A;
struct _tuple6 _tmp37B;enum Cyc_Absyn_Sign _tmp37C;long long _tmp37D;struct
_dyneither_ptr _tmp37E;int _tmp37F;struct _dyneither_ptr _tmp380;_LL203: if((_tmp36B.Char_c).tag
!= 2)goto _LL205;_tmp36C=(struct _tuple3)(_tmp36B.Char_c).val;_tmp36D=_tmp36C.f1;
_tmp36E=_tmp36C.f2;_LL204: {const char*_tmp881;void*_tmp880[1];struct Cyc_String_pa_struct
_tmp87F;return Cyc_PP_text((struct _dyneither_ptr)((_tmp87F.tag=0,((_tmp87F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp36E)),((
_tmp880[0]=& _tmp87F,Cyc_aprintf(((_tmp881="'%s'",_tag_dyneither(_tmp881,sizeof(
char),5))),_tag_dyneither(_tmp880,sizeof(void*),1)))))))));}_LL205: if((_tmp36B.Short_c).tag
!= 3)goto _LL207;_tmp36F=(struct _tuple4)(_tmp36B.Short_c).val;_tmp370=_tmp36F.f1;
_tmp371=_tmp36F.f2;_LL206: {const char*_tmp885;void*_tmp884[1];struct Cyc_Int_pa_struct
_tmp883;return Cyc_PP_text((struct _dyneither_ptr)((_tmp883.tag=1,((_tmp883.f1=(
unsigned long)((int)_tmp371),((_tmp884[0]=& _tmp883,Cyc_aprintf(((_tmp885="%d",
_tag_dyneither(_tmp885,sizeof(char),3))),_tag_dyneither(_tmp884,sizeof(void*),1)))))))));}
_LL207: if((_tmp36B.Int_c).tag != 4)goto _LL209;_tmp372=(struct _tuple5)(_tmp36B.Int_c).val;
_tmp373=_tmp372.f1;if(_tmp373 != Cyc_Absyn_None)goto _LL209;_tmp374=_tmp372.f2;
_LL208: _tmp377=_tmp374;goto _LL20A;_LL209: if((_tmp36B.Int_c).tag != 4)goto _LL20B;
_tmp375=(struct _tuple5)(_tmp36B.Int_c).val;_tmp376=_tmp375.f1;if(_tmp376 != Cyc_Absyn_Signed)
goto _LL20B;_tmp377=_tmp375.f2;_LL20A: {const char*_tmp889;void*_tmp888[1];struct
Cyc_Int_pa_struct _tmp887;return Cyc_PP_text((struct _dyneither_ptr)((_tmp887.tag=1,((
_tmp887.f1=(unsigned long)_tmp377,((_tmp888[0]=& _tmp887,Cyc_aprintf(((_tmp889="%d",
_tag_dyneither(_tmp889,sizeof(char),3))),_tag_dyneither(_tmp888,sizeof(void*),1)))))))));}
_LL20B: if((_tmp36B.Int_c).tag != 4)goto _LL20D;_tmp378=(struct _tuple5)(_tmp36B.Int_c).val;
_tmp379=_tmp378.f1;if(_tmp379 != Cyc_Absyn_Unsigned)goto _LL20D;_tmp37A=_tmp378.f2;
_LL20C: {const char*_tmp88D;void*_tmp88C[1];struct Cyc_Int_pa_struct _tmp88B;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp88B.tag=1,((_tmp88B.f1=(unsigned int)
_tmp37A,((_tmp88C[0]=& _tmp88B,Cyc_aprintf(((_tmp88D="%u",_tag_dyneither(_tmp88D,
sizeof(char),3))),_tag_dyneither(_tmp88C,sizeof(void*),1)))))))));}_LL20D: if((
_tmp36B.LongLong_c).tag != 5)goto _LL20F;_tmp37B=(struct _tuple6)(_tmp36B.LongLong_c).val;
_tmp37C=_tmp37B.f1;_tmp37D=_tmp37B.f2;_LL20E: {const char*_tmp88E;return Cyc_PP_text(((
_tmp88E="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp88E,sizeof(char),27))));}
_LL20F: if((_tmp36B.Float_c).tag != 6)goto _LL211;_tmp37E=(struct _dyneither_ptr)(
_tmp36B.Float_c).val;_LL210: return Cyc_PP_text(_tmp37E);_LL211: if((_tmp36B.Null_c).tag
!= 1)goto _LL213;_tmp37F=(int)(_tmp36B.Null_c).val;_LL212: {const char*_tmp88F;
return Cyc_PP_text(((_tmp88F="NULL",_tag_dyneither(_tmp88F,sizeof(char),5))));}
_LL213: if((_tmp36B.String_c).tag != 7)goto _LL202;_tmp380=(struct _dyneither_ptr)(
_tmp36B.String_c).val;_LL214: {const char*_tmp894;const char*_tmp893;struct Cyc_PP_Doc*
_tmp892[3];return(_tmp892[2]=Cyc_PP_text(((_tmp893="\"",_tag_dyneither(_tmp893,
sizeof(char),2)))),((_tmp892[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp380)),((
_tmp892[0]=Cyc_PP_text(((_tmp894="\"",_tag_dyneither(_tmp894,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp892,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL202:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct
Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (enum Cyc_Absyn_Primop)
Cyc_Absyn_Numelts){if(es == 0  || es->tl != 0){struct Cyc_Core_Failure_struct _tmp8A1;
const char*_tmp8A0;void*_tmp89F[1];struct Cyc_String_pa_struct _tmp89E;struct Cyc_Core_Failure_struct*
_tmp89D;(int)_throw((void*)((_tmp89D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp89D)),((_tmp89D[0]=((_tmp8A1.tag=Cyc_Core_Failure,((_tmp8A1.f1=(
struct _dyneither_ptr)((_tmp89E.tag=0,((_tmp89E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp89F[0]=& _tmp89E,Cyc_aprintf(((
_tmp8A0="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp8A0,
sizeof(char),47))),_tag_dyneither(_tmp89F,sizeof(void*),1)))))))),_tmp8A1)))),
_tmp89D)))));}{const char*_tmp8A6;const char*_tmp8A5;struct Cyc_PP_Doc*_tmp8A4[3];
return(_tmp8A4[2]=Cyc_PP_text(((_tmp8A5=")",_tag_dyneither(_tmp8A5,sizeof(char),
2)))),((_tmp8A4[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp8A4[0]=
Cyc_PP_text(((_tmp8A6="numelts(",_tag_dyneither(_tmp8A6,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp8A4,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{struct Cyc_List_List*
ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){struct Cyc_Core_Failure_struct _tmp8B3;const char*_tmp8B2;void*_tmp8B1[1];
struct Cyc_String_pa_struct _tmp8B0;struct Cyc_Core_Failure_struct*_tmp8AF;(int)
_throw((void*)((_tmp8AF=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8AF)),((
_tmp8AF[0]=((_tmp8B3.tag=Cyc_Core_Failure,((_tmp8B3.f1=(struct _dyneither_ptr)((
_tmp8B0.tag=0,((_tmp8B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(
ps,72)),((_tmp8B1[0]=& _tmp8B0,Cyc_aprintf(((_tmp8B2="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp8B2,sizeof(char),38))),_tag_dyneither(_tmp8B1,sizeof(void*),1)))))))),
_tmp8B3)))),_tmp8AF)))));}else{if(ds->tl == 0){const char*_tmp8B6;struct Cyc_PP_Doc*
_tmp8B5[3];return(_tmp8B5[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp8B5[1]=Cyc_PP_text(((
_tmp8B6=" ",_tag_dyneither(_tmp8B6,sizeof(char),2)))),((_tmp8B5[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp8B5,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp8C3;const char*
_tmp8C2;void*_tmp8C1[1];struct Cyc_String_pa_struct _tmp8C0;struct Cyc_Core_Failure_struct*
_tmp8BF;(int)_throw((void*)((_tmp8BF=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp8BF)),((_tmp8BF[0]=((_tmp8C3.tag=Cyc_Core_Failure,((_tmp8C3.f1=(
struct _dyneither_ptr)((_tmp8C0.tag=0,((_tmp8C0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8C1[0]=& _tmp8C0,Cyc_aprintf(((
_tmp8C2="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp8C2,
sizeof(char),47))),_tag_dyneither(_tmp8C1,sizeof(void*),1)))))))),_tmp8C3)))),
_tmp8BF)))));}else{const char*_tmp8C8;const char*_tmp8C7;struct Cyc_PP_Doc*_tmp8C6[
5];return(_tmp8C6[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((
_tmp8C6[3]=Cyc_PP_text(((_tmp8C7=" ",_tag_dyneither(_tmp8C7,sizeof(char),2)))),((
_tmp8C6[2]=ps,((_tmp8C6[1]=Cyc_PP_text(((_tmp8C8=" ",_tag_dyneither(_tmp8C8,
sizeof(char),2)))),((_tmp8C6[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(
_tag_dyneither(_tmp8C6,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}struct
_dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){switch(p){case Cyc_Absyn_Plus:
_LL215: {const char*_tmp8C9;return(_tmp8C9="+",_tag_dyneither(_tmp8C9,sizeof(char),
2));}case Cyc_Absyn_Times: _LL216: {const char*_tmp8CA;return(_tmp8CA="*",
_tag_dyneither(_tmp8CA,sizeof(char),2));}case Cyc_Absyn_Minus: _LL217: {const char*
_tmp8CB;return(_tmp8CB="-",_tag_dyneither(_tmp8CB,sizeof(char),2));}case Cyc_Absyn_Div:
_LL218: {const char*_tmp8CC;return(_tmp8CC="/",_tag_dyneither(_tmp8CC,sizeof(char),
2));}case Cyc_Absyn_Mod: _LL219: {const char*_tmp8CE;const char*_tmp8CD;return Cyc_Absynpp_print_for_cycdoc?(
_tmp8CE="\\%",_tag_dyneither(_tmp8CE,sizeof(char),3)):((_tmp8CD="%",
_tag_dyneither(_tmp8CD,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL21A: {const char*
_tmp8CF;return(_tmp8CF="==",_tag_dyneither(_tmp8CF,sizeof(char),3));}case Cyc_Absyn_Neq:
_LL21B: {const char*_tmp8D0;return(_tmp8D0="!=",_tag_dyneither(_tmp8D0,sizeof(
char),3));}case Cyc_Absyn_Gt: _LL21C: {const char*_tmp8D1;return(_tmp8D1=">",
_tag_dyneither(_tmp8D1,sizeof(char),2));}case Cyc_Absyn_Lt: _LL21D: {const char*
_tmp8D2;return(_tmp8D2="<",_tag_dyneither(_tmp8D2,sizeof(char),2));}case Cyc_Absyn_Gte:
_LL21E: {const char*_tmp8D3;return(_tmp8D3=">=",_tag_dyneither(_tmp8D3,sizeof(
char),3));}case Cyc_Absyn_Lte: _LL21F: {const char*_tmp8D4;return(_tmp8D4="<=",
_tag_dyneither(_tmp8D4,sizeof(char),3));}case Cyc_Absyn_Not: _LL220: {const char*
_tmp8D5;return(_tmp8D5="!",_tag_dyneither(_tmp8D5,sizeof(char),2));}case Cyc_Absyn_Bitnot:
_LL221: {const char*_tmp8D6;return(_tmp8D6="~",_tag_dyneither(_tmp8D6,sizeof(char),
2));}case Cyc_Absyn_Bitand: _LL222: {const char*_tmp8D7;return(_tmp8D7="&",
_tag_dyneither(_tmp8D7,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL223: {const char*
_tmp8D8;return(_tmp8D8="|",_tag_dyneither(_tmp8D8,sizeof(char),2));}case Cyc_Absyn_Bitxor:
_LL224: {const char*_tmp8D9;return(_tmp8D9="^",_tag_dyneither(_tmp8D9,sizeof(char),
2));}case Cyc_Absyn_Bitlshift: _LL225: {const char*_tmp8DA;return(_tmp8DA="<<",
_tag_dyneither(_tmp8DA,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL226: {const
char*_tmp8DB;return(_tmp8DB=">>",_tag_dyneither(_tmp8DB,sizeof(char),3));}case
Cyc_Absyn_Bitarshift: _LL227: {const char*_tmp8DC;return(_tmp8DC=">>>",
_tag_dyneither(_tmp8DC,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL228: {const
char*_tmp8DD;return(_tmp8DD="numelts",_tag_dyneither(_tmp8DD,sizeof(char),8));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){return Cyc_PP_text(
Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp3BE=s->r;_LL22B: {struct Cyc_Absyn_Decl_s_struct*_tmp3BF=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3BE;if(_tmp3BF->tag != 12)goto _LL22D;}_LL22C: return 1;_LL22D:;_LL22E: return 0;
_LL22A:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int
stmtexp){struct Cyc_PP_Doc*s;{void*_tmp3C0=st->r;struct Cyc_Absyn_Exp*_tmp3C3;
struct Cyc_Absyn_Stmt*_tmp3C5;struct Cyc_Absyn_Stmt*_tmp3C6;struct Cyc_Absyn_Exp*
_tmp3C8;struct Cyc_Absyn_Exp*_tmp3CA;struct Cyc_Absyn_Stmt*_tmp3CB;struct Cyc_Absyn_Stmt*
_tmp3CC;struct _tuple8 _tmp3CE;struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Absyn_Stmt*
_tmp3D0;struct _dyneither_ptr*_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D6;struct _tuple8
_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D8;struct _tuple8 _tmp3D9;struct Cyc_Absyn_Exp*
_tmp3DA;struct Cyc_Absyn_Stmt*_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DD;struct Cyc_List_List*
_tmp3DE;struct Cyc_List_List*_tmp3E0;struct Cyc_List_List*_tmp3E2;struct Cyc_Absyn_Decl*
_tmp3E4;struct Cyc_Absyn_Stmt*_tmp3E5;struct _dyneither_ptr*_tmp3E7;struct Cyc_Absyn_Stmt*
_tmp3E8;struct Cyc_Absyn_Stmt*_tmp3EA;struct _tuple8 _tmp3EB;struct Cyc_Absyn_Exp*
_tmp3EC;struct Cyc_Absyn_Stmt*_tmp3EE;struct Cyc_List_List*_tmp3EF;struct Cyc_Absyn_Exp*
_tmp3F1;_LL230: {struct Cyc_Absyn_Skip_s_struct*_tmp3C1=(struct Cyc_Absyn_Skip_s_struct*)
_tmp3C0;if(_tmp3C1->tag != 0)goto _LL232;}_LL231:{const char*_tmp8DE;s=Cyc_PP_text(((
_tmp8DE=";",_tag_dyneither(_tmp8DE,sizeof(char),2))));}goto _LL22F;_LL232: {
struct Cyc_Absyn_Exp_s_struct*_tmp3C2=(struct Cyc_Absyn_Exp_s_struct*)_tmp3C0;if(
_tmp3C2->tag != 1)goto _LL234;else{_tmp3C3=_tmp3C2->f1;}}_LL233:{const char*_tmp8E1;
struct Cyc_PP_Doc*_tmp8E0[2];s=((_tmp8E0[1]=Cyc_PP_text(((_tmp8E1=";",
_tag_dyneither(_tmp8E1,sizeof(char),2)))),((_tmp8E0[0]=Cyc_Absynpp_exp2doc(
_tmp3C3),Cyc_PP_cat(_tag_dyneither(_tmp8E0,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL22F;_LL234: {struct Cyc_Absyn_Seq_s_struct*_tmp3C4=(struct Cyc_Absyn_Seq_s_struct*)
_tmp3C0;if(_tmp3C4->tag != 2)goto _LL236;else{_tmp3C5=_tmp3C4->f1;_tmp3C6=_tmp3C4->f2;}}
_LL235: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(_tmp3C5)){
struct Cyc_PP_Doc*_tmp8EC[5];struct Cyc_PP_Doc*_tmp8EB[7];const char*_tmp8EA;const
char*_tmp8E9;struct Cyc_PP_Doc*_tmp8E8[7];s=((_tmp8E8[6]=Cyc_Absynpp_is_declaration(
_tmp3C6)?stmtexp?(_tmp8EB[6]=Cyc_PP_line_doc(),((_tmp8EB[5]=Cyc_PP_text(((
_tmp8EA=");",_tag_dyneither(_tmp8EA,sizeof(char),3)))),((_tmp8EB[4]=Cyc_Absynpp_rb(),((
_tmp8EB[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C6,stmtexp)),((_tmp8EB[2]=Cyc_PP_blank_doc(),((
_tmp8EB[1]=Cyc_Absynpp_lb(),((_tmp8EB[0]=Cyc_PP_text(((_tmp8E9="(",
_tag_dyneither(_tmp8E9,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8EB,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))):((_tmp8EC[4]=Cyc_PP_line_doc(),((
_tmp8EC[3]=Cyc_Absynpp_rb(),((_tmp8EC[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3C6,stmtexp)),((_tmp8EC[1]=Cyc_PP_blank_doc(),((_tmp8EC[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8EC,sizeof(struct Cyc_PP_Doc*),5)))))))))))): Cyc_Absynpp_stmt2doc(
_tmp3C6,stmtexp),((_tmp8E8[5]=Cyc_PP_line_doc(),((_tmp8E8[4]=Cyc_Absynpp_rb(),((
_tmp8E8[3]=Cyc_PP_line_doc(),((_tmp8E8[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3C5,0)),((_tmp8E8[1]=Cyc_PP_blank_doc(),((_tmp8E8[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp8E8,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{if(Cyc_Absynpp_is_declaration(
_tmp3C6)){struct Cyc_PP_Doc*_tmp8F7[4];struct Cyc_PP_Doc*_tmp8F6[6];const char*
_tmp8F5;const char*_tmp8F4;struct Cyc_PP_Doc*_tmp8F3[4];s=((_tmp8F3[3]=Cyc_PP_line_doc(),((
_tmp8F3[2]=stmtexp?(_tmp8F6[5]=Cyc_PP_text(((_tmp8F5=");",_tag_dyneither(_tmp8F5,
sizeof(char),3)))),((_tmp8F6[4]=Cyc_Absynpp_rb(),((_tmp8F6[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3C6,stmtexp)),((_tmp8F6[2]=Cyc_PP_blank_doc(),((_tmp8F6[1]=Cyc_Absynpp_lb(),((
_tmp8F6[0]=Cyc_PP_text(((_tmp8F4="(",_tag_dyneither(_tmp8F4,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp8F6,sizeof(struct Cyc_PP_Doc*),6))))))))))))):((
_tmp8F7[3]=Cyc_Absynpp_rb(),((_tmp8F7[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3C6,stmtexp)),((_tmp8F7[1]=Cyc_PP_blank_doc(),((_tmp8F7[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8F7,sizeof(struct Cyc_PP_Doc*),4)))))))))),((
_tmp8F3[1]=Cyc_PP_line_doc(),((_tmp8F3[0]=Cyc_Absynpp_stmt2doc(_tmp3C5,0),Cyc_PP_cat(
_tag_dyneither(_tmp8F3,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{struct Cyc_PP_Doc*
_tmp8F8[3];s=((_tmp8F8[2]=Cyc_Absynpp_stmt2doc(_tmp3C6,stmtexp),((_tmp8F8[1]=Cyc_PP_line_doc(),((
_tmp8F8[0]=Cyc_Absynpp_stmt2doc(_tmp3C5,0),Cyc_PP_cat(_tag_dyneither(_tmp8F8,
sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{struct Cyc_PP_Doc*_tmp8F9[3];s=((
_tmp8F9[2]=Cyc_Absynpp_stmt2doc(_tmp3C6,stmtexp),((_tmp8F9[1]=Cyc_PP_line_doc(),((
_tmp8F9[0]=Cyc_Absynpp_stmt2doc(_tmp3C5,0),Cyc_PP_cat(_tag_dyneither(_tmp8F9,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL22F;_LL236: {struct Cyc_Absyn_Return_s_struct*
_tmp3C7=(struct Cyc_Absyn_Return_s_struct*)_tmp3C0;if(_tmp3C7->tag != 3)goto _LL238;
else{_tmp3C8=_tmp3C7->f1;}}_LL237: if(_tmp3C8 == 0){const char*_tmp8FA;s=Cyc_PP_text(((
_tmp8FA="return;",_tag_dyneither(_tmp8FA,sizeof(char),8))));}else{const char*
_tmp8FF;const char*_tmp8FE;struct Cyc_PP_Doc*_tmp8FD[3];s=((_tmp8FD[2]=Cyc_PP_text(((
_tmp8FE=";",_tag_dyneither(_tmp8FE,sizeof(char),2)))),((_tmp8FD[1]=_tmp3C8 == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp3C8),((_tmp8FD[0]=
Cyc_PP_text(((_tmp8FF="return ",_tag_dyneither(_tmp8FF,sizeof(char),8)))),Cyc_PP_cat(
_tag_dyneither(_tmp8FD,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL22F;_LL238: {
struct Cyc_Absyn_IfThenElse_s_struct*_tmp3C9=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp3C0;if(_tmp3C9->tag != 4)goto _LL23A;else{_tmp3CA=_tmp3C9->f1;_tmp3CB=_tmp3C9->f2;
_tmp3CC=_tmp3C9->f3;}}_LL239: {int print_else;{void*_tmp405=_tmp3CC->r;_LL255: {
struct Cyc_Absyn_Skip_s_struct*_tmp406=(struct Cyc_Absyn_Skip_s_struct*)_tmp405;
if(_tmp406->tag != 0)goto _LL257;}_LL256: print_else=0;goto _LL254;_LL257:;_LL258:
print_else=1;goto _LL254;_LL254:;}{const char*_tmp90E;const char*_tmp90D;struct Cyc_PP_Doc*
_tmp90C[2];struct Cyc_PP_Doc*_tmp90B[6];struct Cyc_PP_Doc*_tmp90A[2];const char*
_tmp909;struct Cyc_PP_Doc*_tmp908[8];s=((_tmp908[7]=print_else?(_tmp90B[5]=Cyc_Absynpp_rb(),((
_tmp90B[4]=Cyc_PP_line_doc(),((_tmp90B[3]=Cyc_PP_nest(2,((_tmp90A[1]=Cyc_Absynpp_stmt2doc(
_tmp3CC,0),((_tmp90A[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp90A,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp90B[2]=Cyc_Absynpp_lb(),((_tmp90B[1]=
Cyc_PP_text(((_tmp909="else ",_tag_dyneither(_tmp909,sizeof(char),6)))),((
_tmp90B[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp90B,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp908[6]=Cyc_Absynpp_rb(),((_tmp908[5]=Cyc_PP_line_doc(),((
_tmp908[4]=Cyc_PP_nest(2,((_tmp90C[1]=Cyc_Absynpp_stmt2doc(_tmp3CB,0),((_tmp90C[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp90C,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp908[3]=Cyc_Absynpp_lb(),((_tmp908[2]=Cyc_PP_text(((_tmp90D=") ",
_tag_dyneither(_tmp90D,sizeof(char),3)))),((_tmp908[1]=Cyc_Absynpp_exp2doc(
_tmp3CA),((_tmp908[0]=Cyc_PP_text(((_tmp90E="if (",_tag_dyneither(_tmp90E,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp908,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL22F;}_LL23A: {struct Cyc_Absyn_While_s_struct*_tmp3CD=(struct Cyc_Absyn_While_s_struct*)
_tmp3C0;if(_tmp3CD->tag != 5)goto _LL23C;else{_tmp3CE=_tmp3CD->f1;_tmp3CF=_tmp3CE.f1;
_tmp3D0=_tmp3CD->f2;}}_LL23B:{const char*_tmp915;const char*_tmp914;struct Cyc_PP_Doc*
_tmp913[2];struct Cyc_PP_Doc*_tmp912[7];s=((_tmp912[6]=Cyc_Absynpp_rb(),((_tmp912[
5]=Cyc_PP_line_doc(),((_tmp912[4]=Cyc_PP_nest(2,((_tmp913[1]=Cyc_Absynpp_stmt2doc(
_tmp3D0,0),((_tmp913[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp913,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp912[3]=Cyc_Absynpp_lb(),((_tmp912[2]=
Cyc_PP_text(((_tmp914=") ",_tag_dyneither(_tmp914,sizeof(char),3)))),((_tmp912[1]=
Cyc_Absynpp_exp2doc(_tmp3CF),((_tmp912[0]=Cyc_PP_text(((_tmp915="while (",
_tag_dyneither(_tmp915,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp912,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL22F;_LL23C: {struct Cyc_Absyn_Break_s_struct*
_tmp3D1=(struct Cyc_Absyn_Break_s_struct*)_tmp3C0;if(_tmp3D1->tag != 6)goto _LL23E;}
_LL23D:{const char*_tmp916;s=Cyc_PP_text(((_tmp916="break;",_tag_dyneither(
_tmp916,sizeof(char),7))));}goto _LL22F;_LL23E: {struct Cyc_Absyn_Continue_s_struct*
_tmp3D2=(struct Cyc_Absyn_Continue_s_struct*)_tmp3C0;if(_tmp3D2->tag != 7)goto
_LL240;}_LL23F:{const char*_tmp917;s=Cyc_PP_text(((_tmp917="continue;",
_tag_dyneither(_tmp917,sizeof(char),10))));}goto _LL22F;_LL240: {struct Cyc_Absyn_Goto_s_struct*
_tmp3D3=(struct Cyc_Absyn_Goto_s_struct*)_tmp3C0;if(_tmp3D3->tag != 8)goto _LL242;
else{_tmp3D4=_tmp3D3->f1;}}_LL241:{const char*_tmp91B;void*_tmp91A[1];struct Cyc_String_pa_struct
_tmp919;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp919.tag=0,((_tmp919.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3D4),((_tmp91A[0]=& _tmp919,Cyc_aprintf(((
_tmp91B="goto %s;",_tag_dyneither(_tmp91B,sizeof(char),9))),_tag_dyneither(
_tmp91A,sizeof(void*),1)))))))));}goto _LL22F;_LL242: {struct Cyc_Absyn_For_s_struct*
_tmp3D5=(struct Cyc_Absyn_For_s_struct*)_tmp3C0;if(_tmp3D5->tag != 9)goto _LL244;
else{_tmp3D6=_tmp3D5->f1;_tmp3D7=_tmp3D5->f2;_tmp3D8=_tmp3D7.f1;_tmp3D9=_tmp3D5->f3;
_tmp3DA=_tmp3D9.f1;_tmp3DB=_tmp3D5->f4;}}_LL243:{const char*_tmp926;const char*
_tmp925;const char*_tmp924;const char*_tmp923;struct Cyc_PP_Doc*_tmp922[2];struct
Cyc_PP_Doc*_tmp921[11];s=((_tmp921[10]=Cyc_Absynpp_rb(),((_tmp921[9]=Cyc_PP_line_doc(),((
_tmp921[8]=Cyc_PP_nest(2,((_tmp922[1]=Cyc_Absynpp_stmt2doc(_tmp3DB,0),((_tmp922[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp922,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp921[7]=Cyc_Absynpp_lb(),((_tmp921[6]=Cyc_PP_text(((_tmp923=") ",
_tag_dyneither(_tmp923,sizeof(char),3)))),((_tmp921[5]=Cyc_Absynpp_exp2doc(
_tmp3DA),((_tmp921[4]=Cyc_PP_text(((_tmp924="; ",_tag_dyneither(_tmp924,sizeof(
char),3)))),((_tmp921[3]=Cyc_Absynpp_exp2doc(_tmp3D8),((_tmp921[2]=Cyc_PP_text(((
_tmp925="; ",_tag_dyneither(_tmp925,sizeof(char),3)))),((_tmp921[1]=Cyc_Absynpp_exp2doc(
_tmp3D6),((_tmp921[0]=Cyc_PP_text(((_tmp926="for(",_tag_dyneither(_tmp926,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp921,sizeof(struct Cyc_PP_Doc*),
11))))))))))))))))))))))));}goto _LL22F;_LL244: {struct Cyc_Absyn_Switch_s_struct*
_tmp3DC=(struct Cyc_Absyn_Switch_s_struct*)_tmp3C0;if(_tmp3DC->tag != 10)goto
_LL246;else{_tmp3DD=_tmp3DC->f1;_tmp3DE=_tmp3DC->f2;}}_LL245:{const char*_tmp92B;
const char*_tmp92A;struct Cyc_PP_Doc*_tmp929[8];s=((_tmp929[7]=Cyc_Absynpp_rb(),((
_tmp929[6]=Cyc_PP_line_doc(),((_tmp929[5]=Cyc_Absynpp_switchclauses2doc(_tmp3DE),((
_tmp929[4]=Cyc_PP_line_doc(),((_tmp929[3]=Cyc_Absynpp_lb(),((_tmp929[2]=Cyc_PP_text(((
_tmp92A=") ",_tag_dyneither(_tmp92A,sizeof(char),3)))),((_tmp929[1]=Cyc_Absynpp_exp2doc(
_tmp3DD),((_tmp929[0]=Cyc_PP_text(((_tmp92B="switch (",_tag_dyneither(_tmp92B,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp929,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL22F;_LL246: {struct Cyc_Absyn_Fallthru_s_struct*_tmp3DF=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp3C0;if(_tmp3DF->tag != 11)goto _LL248;else{_tmp3E0=_tmp3DF->f1;if(_tmp3E0 != 0)
goto _LL248;}}_LL247:{const char*_tmp92C;s=Cyc_PP_text(((_tmp92C="fallthru;",
_tag_dyneither(_tmp92C,sizeof(char),10))));}goto _LL22F;_LL248: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp3E1=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp3C0;if(_tmp3E1->tag != 11)goto
_LL24A;else{_tmp3E2=_tmp3E1->f1;}}_LL249:{const char*_tmp931;const char*_tmp930;
struct Cyc_PP_Doc*_tmp92F[3];s=((_tmp92F[2]=Cyc_PP_text(((_tmp930=");",
_tag_dyneither(_tmp930,sizeof(char),3)))),((_tmp92F[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp3E2),((_tmp92F[0]=Cyc_PP_text(((_tmp931="fallthru(",_tag_dyneither(
_tmp931,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp92F,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL22F;_LL24A: {struct Cyc_Absyn_Decl_s_struct*_tmp3E3=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3C0;if(_tmp3E3->tag != 12)goto _LL24C;else{_tmp3E4=_tmp3E3->f1;_tmp3E5=_tmp3E3->f2;}}
_LL24B:{struct Cyc_PP_Doc*_tmp932[3];s=((_tmp932[2]=Cyc_Absynpp_stmt2doc(_tmp3E5,
stmtexp),((_tmp932[1]=Cyc_PP_line_doc(),((_tmp932[0]=Cyc_Absynpp_decl2doc(
_tmp3E4),Cyc_PP_cat(_tag_dyneither(_tmp932,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22F;_LL24C: {struct Cyc_Absyn_Label_s_struct*_tmp3E6=(struct Cyc_Absyn_Label_s_struct*)
_tmp3C0;if(_tmp3E6->tag != 13)goto _LL24E;else{_tmp3E7=_tmp3E6->f1;_tmp3E8=_tmp3E6->f2;}}
_LL24D: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3E8)){if(
stmtexp){const char*_tmp937;const char*_tmp936;struct Cyc_PP_Doc*_tmp935[8];s=((
_tmp935[7]=Cyc_PP_text(((_tmp936=");",_tag_dyneither(_tmp936,sizeof(char),3)))),((
_tmp935[6]=Cyc_Absynpp_rb(),((_tmp935[5]=Cyc_PP_line_doc(),((_tmp935[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3E8,1)),((_tmp935[3]=Cyc_PP_line_doc(),((_tmp935[2]=
Cyc_Absynpp_lb(),((_tmp935[1]=Cyc_PP_text(((_tmp937=": (",_tag_dyneither(_tmp937,
sizeof(char),4)))),((_tmp935[0]=Cyc_PP_textptr(_tmp3E7),Cyc_PP_cat(
_tag_dyneither(_tmp935,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{const
char*_tmp93A;struct Cyc_PP_Doc*_tmp939[7];s=((_tmp939[6]=Cyc_Absynpp_rb(),((
_tmp939[5]=Cyc_PP_line_doc(),((_tmp939[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3E8,0)),((_tmp939[3]=Cyc_PP_line_doc(),((_tmp939[2]=Cyc_Absynpp_lb(),((
_tmp939[1]=Cyc_PP_text(((_tmp93A=": ",_tag_dyneither(_tmp93A,sizeof(char),3)))),((
_tmp939[0]=Cyc_PP_textptr(_tmp3E7),Cyc_PP_cat(_tag_dyneither(_tmp939,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}}else{const char*_tmp93D;struct Cyc_PP_Doc*
_tmp93C[3];s=((_tmp93C[2]=Cyc_Absynpp_stmt2doc(_tmp3E8,stmtexp),((_tmp93C[1]=Cyc_PP_text(((
_tmp93D=": ",_tag_dyneither(_tmp93D,sizeof(char),3)))),((_tmp93C[0]=Cyc_PP_textptr(
_tmp3E7),Cyc_PP_cat(_tag_dyneither(_tmp93C,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22F;_LL24E: {struct Cyc_Absyn_Do_s_struct*_tmp3E9=(struct Cyc_Absyn_Do_s_struct*)
_tmp3C0;if(_tmp3E9->tag != 14)goto _LL250;else{_tmp3EA=_tmp3E9->f1;_tmp3EB=_tmp3E9->f2;
_tmp3EC=_tmp3EB.f1;}}_LL24F:{const char*_tmp944;const char*_tmp943;const char*
_tmp942;struct Cyc_PP_Doc*_tmp941[9];s=((_tmp941[8]=Cyc_PP_text(((_tmp942=");",
_tag_dyneither(_tmp942,sizeof(char),3)))),((_tmp941[7]=Cyc_Absynpp_exp2doc(
_tmp3EC),((_tmp941[6]=Cyc_PP_text(((_tmp943=" while (",_tag_dyneither(_tmp943,
sizeof(char),9)))),((_tmp941[5]=Cyc_Absynpp_rb(),((_tmp941[4]=Cyc_PP_line_doc(),((
_tmp941[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3EA,0)),((_tmp941[2]=Cyc_PP_line_doc(),((
_tmp941[1]=Cyc_Absynpp_lb(),((_tmp941[0]=Cyc_PP_text(((_tmp944="do ",
_tag_dyneither(_tmp944,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp941,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL22F;_LL250: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp3ED=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp3C0;if(_tmp3ED->tag != 15)goto
_LL252;else{_tmp3EE=_tmp3ED->f1;_tmp3EF=_tmp3ED->f2;}}_LL251:{const char*_tmp949;
const char*_tmp948;struct Cyc_PP_Doc*_tmp947[12];s=((_tmp947[11]=Cyc_Absynpp_rb(),((
_tmp947[10]=Cyc_PP_line_doc(),((_tmp947[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp3EF)),((_tmp947[8]=Cyc_PP_line_doc(),((_tmp947[7]=Cyc_Absynpp_lb(),((_tmp947[
6]=Cyc_PP_text(((_tmp948=" catch ",_tag_dyneither(_tmp948,sizeof(char),8)))),((
_tmp947[5]=Cyc_Absynpp_rb(),((_tmp947[4]=Cyc_PP_line_doc(),((_tmp947[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3EE,0)),((_tmp947[2]=Cyc_PP_line_doc(),((_tmp947[1]=
Cyc_Absynpp_lb(),((_tmp947[0]=Cyc_PP_text(((_tmp949="try ",_tag_dyneither(
_tmp949,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp947,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL22F;_LL252: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp3F0=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp3C0;if(_tmp3F0->tag != 16)goto
_LL22F;else{_tmp3F1=_tmp3F0->f1;}}_LL253:{const char*_tmp94E;const char*_tmp94D;
struct Cyc_PP_Doc*_tmp94C[3];s=((_tmp94C[2]=Cyc_PP_text(((_tmp94D=");",
_tag_dyneither(_tmp94D,sizeof(char),3)))),((_tmp94C[1]=Cyc_Absynpp_exp2doc(
_tmp3F1),((_tmp94C[0]=Cyc_PP_text(((_tmp94E="reset_region(",_tag_dyneither(
_tmp94E,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp94C,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL22F;_LL22F:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp436=p->r;enum Cyc_Absyn_Sign
_tmp43A;int _tmp43B;char _tmp43D;struct _dyneither_ptr _tmp43F;struct Cyc_Absyn_Vardecl*
_tmp441;struct Cyc_Absyn_Pat*_tmp442;struct Cyc_Absyn_Pat _tmp443;void*_tmp444;
struct Cyc_Absyn_Vardecl*_tmp447;struct Cyc_Absyn_Pat*_tmp448;struct Cyc_Absyn_Tvar*
_tmp44A;struct Cyc_Absyn_Vardecl*_tmp44B;struct Cyc_List_List*_tmp44D;int _tmp44E;
struct Cyc_Absyn_Pat*_tmp450;struct Cyc_Absyn_Vardecl*_tmp452;struct Cyc_Absyn_Pat*
_tmp453;struct Cyc_Absyn_Pat _tmp454;void*_tmp455;struct Cyc_Absyn_Vardecl*_tmp458;
struct Cyc_Absyn_Pat*_tmp459;struct _tuple0*_tmp45B;struct _tuple0*_tmp45D;struct
Cyc_List_List*_tmp45E;int _tmp45F;struct Cyc_Absyn_AggrInfo*_tmp461;struct Cyc_Absyn_AggrInfo
_tmp462;union Cyc_Absyn_AggrInfoU _tmp463;struct Cyc_List_List*_tmp464;struct Cyc_List_List*
_tmp465;int _tmp466;struct Cyc_Absyn_AggrInfo*_tmp468;struct Cyc_List_List*_tmp469;
struct Cyc_List_List*_tmp46A;int _tmp46B;struct Cyc_Absyn_Enumfield*_tmp46D;struct
Cyc_Absyn_Enumfield*_tmp46F;struct Cyc_Absyn_Datatypefield*_tmp471;struct Cyc_List_List*
_tmp472;struct Cyc_Absyn_Datatypefield*_tmp474;struct Cyc_List_List*_tmp475;int
_tmp476;struct Cyc_Absyn_Exp*_tmp478;_LL25A: {struct Cyc_Absyn_Wild_p_struct*
_tmp437=(struct Cyc_Absyn_Wild_p_struct*)_tmp436;if(_tmp437->tag != 0)goto _LL25C;}
_LL25B:{const char*_tmp94F;s=Cyc_PP_text(((_tmp94F="_",_tag_dyneither(_tmp94F,
sizeof(char),2))));}goto _LL259;_LL25C: {struct Cyc_Absyn_Null_p_struct*_tmp438=(
struct Cyc_Absyn_Null_p_struct*)_tmp436;if(_tmp438->tag != 8)goto _LL25E;}_LL25D:{
const char*_tmp950;s=Cyc_PP_text(((_tmp950="NULL",_tag_dyneither(_tmp950,sizeof(
char),5))));}goto _LL259;_LL25E: {struct Cyc_Absyn_Int_p_struct*_tmp439=(struct Cyc_Absyn_Int_p_struct*)
_tmp436;if(_tmp439->tag != 9)goto _LL260;else{_tmp43A=_tmp439->f1;_tmp43B=_tmp439->f2;}}
_LL25F: if(_tmp43A != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){const char*_tmp954;
void*_tmp953[1];struct Cyc_Int_pa_struct _tmp952;s=Cyc_PP_text((struct
_dyneither_ptr)((_tmp952.tag=1,((_tmp952.f1=(unsigned long)_tmp43B,((_tmp953[0]=&
_tmp952,Cyc_aprintf(((_tmp954="%d",_tag_dyneither(_tmp954,sizeof(char),3))),
_tag_dyneither(_tmp953,sizeof(void*),1)))))))));}else{const char*_tmp958;void*
_tmp957[1];struct Cyc_Int_pa_struct _tmp956;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp956.tag=1,((_tmp956.f1=(unsigned int)_tmp43B,((_tmp957[0]=& _tmp956,Cyc_aprintf(((
_tmp958="%u",_tag_dyneither(_tmp958,sizeof(char),3))),_tag_dyneither(_tmp957,
sizeof(void*),1)))))))));}goto _LL259;_LL260: {struct Cyc_Absyn_Char_p_struct*
_tmp43C=(struct Cyc_Absyn_Char_p_struct*)_tmp436;if(_tmp43C->tag != 10)goto _LL262;
else{_tmp43D=_tmp43C->f1;}}_LL261:{const char*_tmp95C;void*_tmp95B[1];struct Cyc_String_pa_struct
_tmp95A;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp95A.tag=0,((_tmp95A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp43D)),((
_tmp95B[0]=& _tmp95A,Cyc_aprintf(((_tmp95C="'%s'",_tag_dyneither(_tmp95C,sizeof(
char),5))),_tag_dyneither(_tmp95B,sizeof(void*),1)))))))));}goto _LL259;_LL262: {
struct Cyc_Absyn_Float_p_struct*_tmp43E=(struct Cyc_Absyn_Float_p_struct*)_tmp436;
if(_tmp43E->tag != 11)goto _LL264;else{_tmp43F=_tmp43E->f1;}}_LL263: s=Cyc_PP_text(
_tmp43F);goto _LL259;_LL264: {struct Cyc_Absyn_Var_p_struct*_tmp440=(struct Cyc_Absyn_Var_p_struct*)
_tmp436;if(_tmp440->tag != 1)goto _LL266;else{_tmp441=_tmp440->f1;_tmp442=_tmp440->f2;
_tmp443=*_tmp442;_tmp444=_tmp443.r;{struct Cyc_Absyn_Wild_p_struct*_tmp445=(
struct Cyc_Absyn_Wild_p_struct*)_tmp444;if(_tmp445->tag != 0)goto _LL266;};}}_LL265:
s=Cyc_Absynpp_qvar2doc(_tmp441->name);goto _LL259;_LL266: {struct Cyc_Absyn_Var_p_struct*
_tmp446=(struct Cyc_Absyn_Var_p_struct*)_tmp436;if(_tmp446->tag != 1)goto _LL268;
else{_tmp447=_tmp446->f1;_tmp448=_tmp446->f2;}}_LL267:{const char*_tmp95F;struct
Cyc_PP_Doc*_tmp95E[3];s=((_tmp95E[2]=Cyc_Absynpp_pat2doc(_tmp448),((_tmp95E[1]=
Cyc_PP_text(((_tmp95F=" as ",_tag_dyneither(_tmp95F,sizeof(char),5)))),((_tmp95E[
0]=Cyc_Absynpp_qvar2doc(_tmp447->name),Cyc_PP_cat(_tag_dyneither(_tmp95E,sizeof(
struct Cyc_PP_Doc*),3))))))));}goto _LL259;_LL268: {struct Cyc_Absyn_TagInt_p_struct*
_tmp449=(struct Cyc_Absyn_TagInt_p_struct*)_tmp436;if(_tmp449->tag != 3)goto _LL26A;
else{_tmp44A=_tmp449->f1;_tmp44B=_tmp449->f2;}}_LL269:{const char*_tmp964;const
char*_tmp963;struct Cyc_PP_Doc*_tmp962[4];s=((_tmp962[3]=Cyc_PP_text(((_tmp963=">",
_tag_dyneither(_tmp963,sizeof(char),2)))),((_tmp962[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp44A)),((_tmp962[1]=Cyc_PP_text(((_tmp964="<",_tag_dyneither(_tmp964,sizeof(
char),2)))),((_tmp962[0]=Cyc_Absynpp_qvar2doc(_tmp44B->name),Cyc_PP_cat(
_tag_dyneither(_tmp962,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL259;_LL26A: {
struct Cyc_Absyn_Tuple_p_struct*_tmp44C=(struct Cyc_Absyn_Tuple_p_struct*)_tmp436;
if(_tmp44C->tag != 4)goto _LL26C;else{_tmp44D=_tmp44C->f1;_tmp44E=_tmp44C->f2;}}
_LL26B:{const char*_tmp96D;const char*_tmp96C;const char*_tmp96B;const char*_tmp96A;
struct Cyc_PP_Doc*_tmp969[4];s=((_tmp969[3]=_tmp44E?Cyc_PP_text(((_tmp96A=", ...)",
_tag_dyneither(_tmp96A,sizeof(char),7)))): Cyc_PP_text(((_tmp96B=")",
_tag_dyneither(_tmp96B,sizeof(char),2)))),((_tmp969[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp96C=",",_tag_dyneither(_tmp96C,
sizeof(char),2))),_tmp44D),((_tmp969[1]=Cyc_PP_text(((_tmp96D="(",_tag_dyneither(
_tmp96D,sizeof(char),2)))),((_tmp969[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp969,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL259;_LL26C: {
struct Cyc_Absyn_Pointer_p_struct*_tmp44F=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp436;if(_tmp44F->tag != 5)goto _LL26E;else{_tmp450=_tmp44F->f1;}}_LL26D:{const
char*_tmp970;struct Cyc_PP_Doc*_tmp96F[2];s=((_tmp96F[1]=Cyc_Absynpp_pat2doc(
_tmp450),((_tmp96F[0]=Cyc_PP_text(((_tmp970="&",_tag_dyneither(_tmp970,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp96F,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL259;_LL26E: {struct Cyc_Absyn_Reference_p_struct*_tmp451=(struct Cyc_Absyn_Reference_p_struct*)
_tmp436;if(_tmp451->tag != 2)goto _LL270;else{_tmp452=_tmp451->f1;_tmp453=_tmp451->f2;
_tmp454=*_tmp453;_tmp455=_tmp454.r;{struct Cyc_Absyn_Wild_p_struct*_tmp456=(
struct Cyc_Absyn_Wild_p_struct*)_tmp455;if(_tmp456->tag != 0)goto _LL270;};}}_LL26F:{
const char*_tmp973;struct Cyc_PP_Doc*_tmp972[2];s=((_tmp972[1]=Cyc_Absynpp_qvar2doc(
_tmp452->name),((_tmp972[0]=Cyc_PP_text(((_tmp973="*",_tag_dyneither(_tmp973,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp972,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL259;_LL270: {struct Cyc_Absyn_Reference_p_struct*_tmp457=(struct Cyc_Absyn_Reference_p_struct*)
_tmp436;if(_tmp457->tag != 2)goto _LL272;else{_tmp458=_tmp457->f1;_tmp459=_tmp457->f2;}}
_LL271:{const char*_tmp978;const char*_tmp977;struct Cyc_PP_Doc*_tmp976[4];s=((
_tmp976[3]=Cyc_Absynpp_pat2doc(_tmp459),((_tmp976[2]=Cyc_PP_text(((_tmp977=" as ",
_tag_dyneither(_tmp977,sizeof(char),5)))),((_tmp976[1]=Cyc_Absynpp_qvar2doc(
_tmp458->name),((_tmp976[0]=Cyc_PP_text(((_tmp978="*",_tag_dyneither(_tmp978,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp976,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL259;_LL272: {struct Cyc_Absyn_UnknownId_p_struct*_tmp45A=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp436;if(_tmp45A->tag != 14)goto _LL274;else{_tmp45B=_tmp45A->f1;}}_LL273: s=Cyc_Absynpp_qvar2doc(
_tmp45B);goto _LL259;_LL274: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp45C=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp436;if(_tmp45C->tag != 15)goto _LL276;
else{_tmp45D=_tmp45C->f1;_tmp45E=_tmp45C->f2;_tmp45F=_tmp45C->f3;}}_LL275: {
const char*_tmp97A;const char*_tmp979;struct _dyneither_ptr term=_tmp45F?(_tmp97A=", ...)",
_tag_dyneither(_tmp97A,sizeof(char),7)):((_tmp979=")",_tag_dyneither(_tmp979,
sizeof(char),2)));{const char*_tmp97F;const char*_tmp97E;struct Cyc_PP_Doc*_tmp97D[
2];s=((_tmp97D[1]=Cyc_PP_group(((_tmp97F="(",_tag_dyneither(_tmp97F,sizeof(char),
2))),term,((_tmp97E=",",_tag_dyneither(_tmp97E,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp45E)),((_tmp97D[0]=Cyc_Absynpp_qvar2doc(_tmp45D),Cyc_PP_cat(
_tag_dyneither(_tmp97D,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL259;}_LL276: {
struct Cyc_Absyn_Aggr_p_struct*_tmp460=(struct Cyc_Absyn_Aggr_p_struct*)_tmp436;
if(_tmp460->tag != 6)goto _LL278;else{_tmp461=_tmp460->f1;if(_tmp461 == 0)goto
_LL278;_tmp462=*_tmp461;_tmp463=_tmp462.aggr_info;_tmp464=_tmp460->f2;_tmp465=
_tmp460->f3;_tmp466=_tmp460->f4;}}_LL277: {const char*_tmp981;const char*_tmp980;
struct _dyneither_ptr term=_tmp466?(_tmp981=", ...}",_tag_dyneither(_tmp981,
sizeof(char),7)):((_tmp980="}",_tag_dyneither(_tmp980,sizeof(char),2)));struct
_tuple0*_tmp49B;struct _tuple9 _tmp49A=Cyc_Absyn_aggr_kinded_name(_tmp463);_tmp49B=
_tmp49A.f2;{const char*_tmp98C;const char*_tmp98B;const char*_tmp98A;const char*
_tmp989;const char*_tmp988;struct Cyc_PP_Doc*_tmp987[4];s=((_tmp987[3]=Cyc_PP_group(((
_tmp989="",_tag_dyneither(_tmp989,sizeof(char),1))),term,((_tmp988=",",
_tag_dyneither(_tmp988,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp465)),((_tmp987[2]=Cyc_PP_egroup(((_tmp98C="[",_tag_dyneither(_tmp98C,
sizeof(char),2))),((_tmp98B="]",_tag_dyneither(_tmp98B,sizeof(char),2))),((
_tmp98A=",",_tag_dyneither(_tmp98A,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp464))),((_tmp987[1]=
Cyc_Absynpp_lb(),((_tmp987[0]=Cyc_Absynpp_qvar2doc(_tmp49B),Cyc_PP_cat(
_tag_dyneither(_tmp987,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL259;}_LL278: {
struct Cyc_Absyn_Aggr_p_struct*_tmp467=(struct Cyc_Absyn_Aggr_p_struct*)_tmp436;
if(_tmp467->tag != 6)goto _LL27A;else{_tmp468=_tmp467->f1;if(_tmp468 != 0)goto
_LL27A;_tmp469=_tmp467->f2;_tmp46A=_tmp467->f3;_tmp46B=_tmp467->f4;}}_LL279: {
const char*_tmp98E;const char*_tmp98D;struct _dyneither_ptr term=_tmp46B?(_tmp98E=", ...}",
_tag_dyneither(_tmp98E,sizeof(char),7)):((_tmp98D="}",_tag_dyneither(_tmp98D,
sizeof(char),2)));{const char*_tmp999;const char*_tmp998;const char*_tmp997;const
char*_tmp996;const char*_tmp995;struct Cyc_PP_Doc*_tmp994[3];s=((_tmp994[2]=Cyc_PP_group(((
_tmp996="",_tag_dyneither(_tmp996,sizeof(char),1))),term,((_tmp995=",",
_tag_dyneither(_tmp995,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp46A)),((_tmp994[1]=Cyc_PP_egroup(((_tmp999="[",_tag_dyneither(_tmp999,
sizeof(char),2))),((_tmp998="]",_tag_dyneither(_tmp998,sizeof(char),2))),((
_tmp997=",",_tag_dyneither(_tmp997,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp469))),((_tmp994[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp994,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL259;}_LL27A: {struct Cyc_Absyn_Enum_p_struct*_tmp46C=(struct Cyc_Absyn_Enum_p_struct*)
_tmp436;if(_tmp46C->tag != 12)goto _LL27C;else{_tmp46D=_tmp46C->f2;}}_LL27B:
_tmp46F=_tmp46D;goto _LL27D;_LL27C: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp46E=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp436;if(_tmp46E->tag != 13)goto _LL27E;else{
_tmp46F=_tmp46E->f2;}}_LL27D: s=Cyc_Absynpp_qvar2doc(_tmp46F->name);goto _LL259;
_LL27E: {struct Cyc_Absyn_Datatype_p_struct*_tmp470=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp436;if(_tmp470->tag != 7)goto _LL280;else{_tmp471=_tmp470->f2;_tmp472=_tmp470->f3;
if(_tmp472 != 0)goto _LL280;}}_LL27F: s=Cyc_Absynpp_qvar2doc(_tmp471->name);goto
_LL259;_LL280: {struct Cyc_Absyn_Datatype_p_struct*_tmp473=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp436;if(_tmp473->tag != 7)goto _LL282;else{_tmp474=_tmp473->f2;_tmp475=_tmp473->f3;
_tmp476=_tmp473->f4;}}_LL281: {const char*_tmp99B;const char*_tmp99A;struct
_dyneither_ptr term=_tmp476?(_tmp99B=", ...)",_tag_dyneither(_tmp99B,sizeof(char),
7)):((_tmp99A=")",_tag_dyneither(_tmp99A,sizeof(char),2)));{const char*_tmp9A0;
const char*_tmp99F;struct Cyc_PP_Doc*_tmp99E[2];s=((_tmp99E[1]=Cyc_PP_egroup(((
_tmp9A0="(",_tag_dyneither(_tmp9A0,sizeof(char),2))),term,((_tmp99F=",",
_tag_dyneither(_tmp99F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp475)),((_tmp99E[0]=Cyc_Absynpp_qvar2doc(_tmp474->name),Cyc_PP_cat(
_tag_dyneither(_tmp99E,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL259;}_LL282: {
struct Cyc_Absyn_Exp_p_struct*_tmp477=(struct Cyc_Absyn_Exp_p_struct*)_tmp436;if(
_tmp477->tag != 16)goto _LL259;else{_tmp478=_tmp477->f1;}}_LL283: s=Cyc_Absynpp_exp2doc(
_tmp478);goto _LL259;_LL259:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple13*dp){const char*_tmp9A7;const char*_tmp9A6;const char*_tmp9A5;struct Cyc_PP_Doc*
_tmp9A4[2];return(_tmp9A4[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp9A4[0]=Cyc_PP_egroup(((
_tmp9A7="",_tag_dyneither(_tmp9A7,sizeof(char),1))),((_tmp9A6="=",_tag_dyneither(
_tmp9A6,sizeof(char),2))),((_tmp9A5="=",_tag_dyneither(_tmp9A5,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp9A4,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (c->pattern)->r == (void*)&
Cyc_Absyn_Wild_p_val){const char*_tmp9AC;struct Cyc_PP_Doc*_tmp9AB[2];struct Cyc_PP_Doc*
_tmp9AA[2];return(_tmp9AA[1]=Cyc_PP_nest(2,((_tmp9AB[1]=Cyc_Absynpp_stmt2doc(c->body,
0),((_tmp9AB[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9AB,sizeof(
struct Cyc_PP_Doc*),2))))))),((_tmp9AA[0]=Cyc_PP_text(((_tmp9AC="default: ",
_tag_dyneither(_tmp9AC,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp9AA,
sizeof(struct Cyc_PP_Doc*),2)))));}else{if(c->where_clause == 0){const char*_tmp9B3;
const char*_tmp9B2;struct Cyc_PP_Doc*_tmp9B1[2];struct Cyc_PP_Doc*_tmp9B0[4];return(
_tmp9B0[3]=Cyc_PP_nest(2,((_tmp9B1[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9B1[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9B1,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp9B0[2]=Cyc_PP_text(((_tmp9B2=": ",_tag_dyneither(_tmp9B2,sizeof(
char),3)))),((_tmp9B0[1]=Cyc_Absynpp_pat2doc(c->pattern),((_tmp9B0[0]=Cyc_PP_text(((
_tmp9B3="case ",_tag_dyneither(_tmp9B3,sizeof(char),6)))),Cyc_PP_cat(
_tag_dyneither(_tmp9B0,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{const char*
_tmp9BC;const char*_tmp9BB;const char*_tmp9BA;struct Cyc_PP_Doc*_tmp9B9[2];struct
Cyc_PP_Doc*_tmp9B8[6];return(_tmp9B8[5]=Cyc_PP_nest(2,((_tmp9B9[1]=Cyc_Absynpp_stmt2doc(
c->body,0),((_tmp9B9[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9B9,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9B8[4]=Cyc_PP_text(((_tmp9BA=": ",
_tag_dyneither(_tmp9BA,sizeof(char),3)))),((_tmp9B8[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp9B8[2]=Cyc_PP_text(((
_tmp9BB=" && ",_tag_dyneither(_tmp9BB,sizeof(char),5)))),((_tmp9B8[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp9B8[0]=Cyc_PP_text(((_tmp9BC="case ",_tag_dyneither(_tmp9BC,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9B8,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*
_tmp9BD;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp9BD="",_tag_dyneither(_tmp9BD,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);
else{const char*_tmp9C0;struct Cyc_PP_Doc*_tmp9BF[3];return(_tmp9BF[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp9BF[1]=Cyc_PP_text(((_tmp9C0=" = ",
_tag_dyneither(_tmp9C0,sizeof(char),4)))),((_tmp9BF[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp9BF,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){const char*_tmp9C1;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((
_tmp9C1=",",_tag_dyneither(_tmp9C1,sizeof(char),2))),fs);}static struct Cyc_PP_Doc*
Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(v->name);}
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){const char*
_tmp9C2;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((
_tmp9C2=",",_tag_dyneither(_tmp9C2,sizeof(char),2))),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(
struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl _tmp4C7;enum Cyc_Absyn_Scope
_tmp4C8;struct _tuple0*_tmp4C9;struct Cyc_Absyn_Tqual _tmp4CA;void*_tmp4CB;struct
Cyc_Absyn_Exp*_tmp4CC;struct Cyc_List_List*_tmp4CD;struct Cyc_Absyn_Vardecl*
_tmp4C6=vd;_tmp4C7=*_tmp4C6;_tmp4C8=_tmp4C7.sc;_tmp4C9=_tmp4C7.name;_tmp4CA=
_tmp4C7.tq;_tmp4CB=_tmp4C7.type;_tmp4CC=_tmp4C7.initializer;_tmp4CD=_tmp4C7.attributes;{
struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp4C9);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4CD);struct Cyc_PP_Doc*
beforenamedoc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL284:
beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL285:{void*_tmp4CE=Cyc_Tcutil_compress(
_tmp4CB);struct Cyc_Absyn_FnInfo _tmp4D0;struct Cyc_List_List*_tmp4D1;_LL288: {
struct Cyc_Absyn_FnType_struct*_tmp4CF=(struct Cyc_Absyn_FnType_struct*)_tmp4CE;
if(_tmp4CF->tag != 10)goto _LL28A;else{_tmp4D0=_tmp4CF->f1;_tmp4D1=_tmp4D0.attributes;}}
_LL289: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4D1);goto _LL287;_LL28A:;_LL28B:
beforenamedoc=Cyc_PP_nil_doc();goto _LL287;_LL287:;}break;}{struct Cyc_PP_Doc*
tmp_doc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL28C: tmp_doc=Cyc_PP_nil_doc();
break;case Cyc_Cyclone_Vc_c: _LL28D: tmp_doc=attsdoc;break;}{struct Cyc_Core_Opt*
_tmp9CF;struct Cyc_PP_Doc*_tmp9CE[2];struct Cyc_PP_Doc*_tmp9CD[2];const char*
_tmp9CC;const char*_tmp9CB;struct Cyc_PP_Doc*_tmp9CA[5];s=((_tmp9CA[4]=Cyc_PP_text(((
_tmp9CB=";",_tag_dyneither(_tmp9CB,sizeof(char),2)))),((_tmp9CA[3]=_tmp4CC == 0?
Cyc_PP_nil_doc():((_tmp9CD[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4CC),((
_tmp9CD[0]=Cyc_PP_text(((_tmp9CC=" = ",_tag_dyneither(_tmp9CC,sizeof(char),4)))),
Cyc_PP_cat(_tag_dyneither(_tmp9CD,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9CA[2]=
Cyc_Absynpp_tqtd2doc(_tmp4CA,_tmp4CB,((_tmp9CF=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp9CF)),((_tmp9CF->v=((_tmp9CE[1]=sn,((_tmp9CE[0]=beforenamedoc,Cyc_PP_cat(
_tag_dyneither(_tmp9CE,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9CF))))),((_tmp9CA[
1]=Cyc_Absynpp_scope2doc(_tmp4C8),((_tmp9CA[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(
_tmp9CA,sizeof(struct Cyc_PP_Doc*),5))))))))))));}return s;};};}struct _tuple17{
struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(
struct _tuple17*x){struct _tuple0*_tmp4D9;struct _tuple17 _tmp4D8=*x;_tmp4D9=_tmp4D8.f2;
return Cyc_Absynpp_qvar2doc(_tmp4D9);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(
struct Cyc_Absyn_Aggrdecl*ad){if(ad->impl == 0){struct Cyc_PP_Doc*_tmp9D0[4];return(
_tmp9D0[3]=Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp9D0[2]=Cyc_Absynpp_qvar2bolddoc(
ad->name),((_tmp9D0[1]=Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp9D0[0]=Cyc_Absynpp_scope2doc(
ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp9D0,sizeof(struct Cyc_PP_Doc*),4)))))))));}
else{struct Cyc_PP_Doc*_tmp9D8[2];const char*_tmp9D7;struct Cyc_PP_Doc*_tmp9D6[2];
struct Cyc_PP_Doc*_tmp9D5[12];return(_tmp9D5[11]=Cyc_Absynpp_atts2doc(ad->attributes),((
_tmp9D5[10]=Cyc_Absynpp_rb(),((_tmp9D5[9]=Cyc_PP_line_doc(),((_tmp9D5[8]=Cyc_PP_nest(
2,((_tmp9D6[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields),((_tmp9D6[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp9D6,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9D5[7]=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():((
_tmp9D8[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((
_tmp9D8[0]=Cyc_PP_text(((_tmp9D7=":",_tag_dyneither(_tmp9D7,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp9D8,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9D5[6]=
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((
_tmp9D5[5]=Cyc_Absynpp_lb(),((_tmp9D5[4]=Cyc_PP_blank_doc(),((_tmp9D5[3]=Cyc_Absynpp_ktvars2doc(
ad->tvs),((_tmp9D5[2]=Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9D5[1]=Cyc_Absynpp_aggr_kind2doc(
ad->kind),((_tmp9D5[0]=Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(
_tmp9D5,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){struct Cyc_Absyn_Datatypedecl
_tmp4E0;enum Cyc_Absyn_Scope _tmp4E1;struct _tuple0*_tmp4E2;struct Cyc_List_List*
_tmp4E3;struct Cyc_Core_Opt*_tmp4E4;int _tmp4E5;struct Cyc_Absyn_Datatypedecl*
_tmp4DF=dd;_tmp4E0=*_tmp4DF;_tmp4E1=_tmp4E0.sc;_tmp4E2=_tmp4E0.name;_tmp4E3=
_tmp4E0.tvs;_tmp4E4=_tmp4E0.fields;_tmp4E5=_tmp4E0.is_extensible;if(_tmp4E4 == 0){
const char*_tmp9DD;const char*_tmp9DC;struct Cyc_PP_Doc*_tmp9DB[5];return(_tmp9DB[4]=
Cyc_Absynpp_ktvars2doc(_tmp4E3),((_tmp9DB[3]=_tmp4E5?Cyc_Absynpp_qvar2bolddoc(
_tmp4E2): Cyc_Absynpp_typedef_name2bolddoc(_tmp4E2),((_tmp9DB[2]=Cyc_PP_text(((
_tmp9DC="datatype ",_tag_dyneither(_tmp9DC,sizeof(char),10)))),((_tmp9DB[1]=
_tmp4E5?Cyc_PP_text(((_tmp9DD="@extensible",_tag_dyneither(_tmp9DD,sizeof(char),
12)))): Cyc_PP_blank_doc(),((_tmp9DB[0]=Cyc_Absynpp_scope2doc(_tmp4E1),Cyc_PP_cat(
_tag_dyneither(_tmp9DB,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{const char*
_tmp9E4;const char*_tmp9E3;struct Cyc_PP_Doc*_tmp9E2[2];struct Cyc_PP_Doc*_tmp9E1[
10];return(_tmp9E1[9]=Cyc_Absynpp_rb(),((_tmp9E1[8]=Cyc_PP_line_doc(),((_tmp9E1[
7]=Cyc_PP_nest(2,((_tmp9E2[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)
_tmp4E4->v),((_tmp9E2[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9E2,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9E1[6]=Cyc_Absynpp_lb(),((_tmp9E1[5]=
Cyc_PP_blank_doc(),((_tmp9E1[4]=Cyc_Absynpp_ktvars2doc(_tmp4E3),((_tmp9E1[3]=
_tmp4E5?Cyc_Absynpp_qvar2bolddoc(_tmp4E2): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4E2),((_tmp9E1[2]=Cyc_PP_text(((_tmp9E3="datatype ",_tag_dyneither(_tmp9E3,
sizeof(char),10)))),((_tmp9E1[1]=_tmp4E5?Cyc_PP_text(((_tmp9E4="@extensible ",
_tag_dyneither(_tmp9E4,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9E1[0]=Cyc_Absynpp_scope2doc(
_tmp4E1),Cyc_PP_cat(_tag_dyneither(_tmp9E1,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){struct Cyc_Absyn_Enumdecl
_tmp4EE;enum Cyc_Absyn_Scope _tmp4EF;struct _tuple0*_tmp4F0;struct Cyc_Core_Opt*
_tmp4F1;struct Cyc_Absyn_Enumdecl*_tmp4ED=ed;_tmp4EE=*_tmp4ED;_tmp4EF=_tmp4EE.sc;
_tmp4F0=_tmp4EE.name;_tmp4F1=_tmp4EE.fields;if(_tmp4F1 == 0){const char*_tmp9E7;
struct Cyc_PP_Doc*_tmp9E6[3];return(_tmp9E6[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp4F0),((_tmp9E6[1]=Cyc_PP_text(((_tmp9E7="enum ",_tag_dyneither(_tmp9E7,
sizeof(char),6)))),((_tmp9E6[0]=Cyc_Absynpp_scope2doc(_tmp4EF),Cyc_PP_cat(
_tag_dyneither(_tmp9E6,sizeof(struct Cyc_PP_Doc*),3)))))));}else{const char*
_tmp9EC;struct Cyc_PP_Doc*_tmp9EB[2];struct Cyc_PP_Doc*_tmp9EA[8];return(_tmp9EA[7]=
Cyc_Absynpp_rb(),((_tmp9EA[6]=Cyc_PP_line_doc(),((_tmp9EA[5]=Cyc_PP_nest(2,((
_tmp9EB[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp4F1->v),((
_tmp9EB[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9EB,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp9EA[4]=Cyc_Absynpp_lb(),((_tmp9EA[3]=Cyc_PP_blank_doc(),((_tmp9EA[
2]=Cyc_Absynpp_qvar2bolddoc(_tmp4F0),((_tmp9EA[1]=Cyc_PP_text(((_tmp9EC="enum ",
_tag_dyneither(_tmp9EC,sizeof(char),6)))),((_tmp9EA[0]=Cyc_Absynpp_scope2doc(
_tmp4EF),Cyc_PP_cat(_tag_dyneither(_tmp9EA,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{
void*_tmp4F7=d->r;struct Cyc_Absyn_Fndecl*_tmp4F9;struct Cyc_Absyn_Aggrdecl*
_tmp4FB;struct Cyc_Absyn_Vardecl*_tmp4FD;struct Cyc_Absyn_Tvar*_tmp4FF;struct Cyc_Absyn_Vardecl*
_tmp500;int _tmp501;struct Cyc_Absyn_Exp*_tmp502;struct Cyc_Absyn_Exp*_tmp504;
struct Cyc_Absyn_Tvar*_tmp505;struct Cyc_Absyn_Vardecl*_tmp506;struct Cyc_Absyn_Datatypedecl*
_tmp508;struct Cyc_Absyn_Pat*_tmp50A;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_List_List*
_tmp50D;struct Cyc_Absyn_Enumdecl*_tmp50F;struct Cyc_Absyn_Typedefdecl*_tmp511;
struct _dyneither_ptr*_tmp513;struct Cyc_List_List*_tmp514;struct _tuple0*_tmp516;
struct Cyc_List_List*_tmp517;struct Cyc_List_List*_tmp519;struct Cyc_List_List*
_tmp51B;struct Cyc_List_List*_tmp51C;_LL290: {struct Cyc_Absyn_Fn_d_struct*_tmp4F8=(
struct Cyc_Absyn_Fn_d_struct*)_tmp4F7;if(_tmp4F8->tag != 1)goto _LL292;else{_tmp4F9=
_tmp4F8->f1;}}_LL291: {struct Cyc_Absyn_FnType_struct _tmp9F2;struct Cyc_Absyn_FnInfo
_tmp9F1;struct Cyc_Absyn_FnType_struct*_tmp9F0;void*t=(void*)((_tmp9F0=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp9F0)),((_tmp9F0[0]=((_tmp9F2.tag=
10,((_tmp9F2.f1=((_tmp9F1.tvars=_tmp4F9->tvs,((_tmp9F1.effect=_tmp4F9->effect,((
_tmp9F1.ret_tqual=_tmp4F9->ret_tqual,((_tmp9F1.ret_typ=_tmp4F9->ret_type,((
_tmp9F1.args=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4F9->args),((
_tmp9F1.c_varargs=_tmp4F9->c_varargs,((_tmp9F1.cyc_varargs=_tmp4F9->cyc_varargs,((
_tmp9F1.rgn_po=_tmp4F9->rgn_po,((_tmp9F1.attributes=0,_tmp9F1)))))))))))))))))),
_tmp9F2)))),_tmp9F0))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4F9->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp4F9->is_inline)switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL2B0:{const char*_tmp9F3;inlinedoc=Cyc_PP_text(((_tmp9F3="inline ",
_tag_dyneither(_tmp9F3,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2B1:{
const char*_tmp9F4;inlinedoc=Cyc_PP_text(((_tmp9F4="__inline ",_tag_dyneither(
_tmp9F4,sizeof(char),10))));}break;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc(_tmp4F9->sc);struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2B3: beforenamedoc=
attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2B4: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp4F9->attributes);break;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp4F9->name);struct Cyc_PP_Doc*_tmp9F7[2];struct Cyc_Core_Opt*_tmp9F6;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp9F6=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp9F6)),((_tmp9F6->v=((_tmp9F7[1]=namedoc,((
_tmp9F7[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp9F7,sizeof(struct Cyc_PP_Doc*),
2)))))),_tmp9F6)))));struct Cyc_PP_Doc*_tmp9FA[2];struct Cyc_PP_Doc*_tmp9F9[5];
struct Cyc_PP_Doc*bodydoc=(_tmp9F9[4]=Cyc_Absynpp_rb(),((_tmp9F9[3]=Cyc_PP_line_doc(),((
_tmp9F9[2]=Cyc_PP_nest(2,((_tmp9FA[1]=Cyc_Absynpp_stmt2doc(_tmp4F9->body,0),((
_tmp9FA[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9FA,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp9F9[1]=Cyc_Absynpp_lb(),((_tmp9F9[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp9F9,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*
_tmp9FB[4];s=((_tmp9FB[3]=bodydoc,((_tmp9FB[2]=tqtddoc,((_tmp9FB[1]=scopedoc,((
_tmp9FB[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9FB,sizeof(struct Cyc_PP_Doc*),
4))))))))));}switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2B6:{struct
Cyc_PP_Doc*_tmp9FC[2];s=((_tmp9FC[1]=s,((_tmp9FC[0]=attsdoc,Cyc_PP_cat(
_tag_dyneither(_tmp9FC,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2B7:
break;}goto _LL28F;};};}_LL292: {struct Cyc_Absyn_Aggr_d_struct*_tmp4FA=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4F7;if(_tmp4FA->tag != 6)goto _LL294;else{_tmp4FB=_tmp4FA->f1;}}_LL293:{const
char*_tmp9FF;struct Cyc_PP_Doc*_tmp9FE[2];s=((_tmp9FE[1]=Cyc_PP_text(((_tmp9FF=";",
_tag_dyneither(_tmp9FF,sizeof(char),2)))),((_tmp9FE[0]=Cyc_Absynpp_aggrdecl2doc(
_tmp4FB),Cyc_PP_cat(_tag_dyneither(_tmp9FE,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28F;_LL294: {struct Cyc_Absyn_Var_d_struct*_tmp4FC=(struct Cyc_Absyn_Var_d_struct*)
_tmp4F7;if(_tmp4FC->tag != 0)goto _LL296;else{_tmp4FD=_tmp4FC->f1;}}_LL295: s=Cyc_Absynpp_vardecl2doc(
_tmp4FD);goto _LL28F;_LL296: {struct Cyc_Absyn_Region_d_struct*_tmp4FE=(struct Cyc_Absyn_Region_d_struct*)
_tmp4F7;if(_tmp4FE->tag != 4)goto _LL298;else{_tmp4FF=_tmp4FE->f1;_tmp500=_tmp4FE->f2;
_tmp501=_tmp4FE->f3;_tmp502=_tmp4FE->f4;}}_LL297:{const char*_tmpA12;const char*
_tmpA11;const char*_tmpA10;const char*_tmpA0F;struct Cyc_PP_Doc*_tmpA0E[3];const
char*_tmpA0D;const char*_tmpA0C;const char*_tmpA0B;struct Cyc_PP_Doc*_tmpA0A[8];s=((
_tmpA0A[7]=Cyc_PP_text(((_tmpA0B=";",_tag_dyneither(_tmpA0B,sizeof(char),2)))),((
_tmpA0A[6]=_tmp502 != 0?(_tmpA0E[2]=Cyc_PP_text(((_tmpA0D=")",_tag_dyneither(
_tmpA0D,sizeof(char),2)))),((_tmpA0E[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp502),((_tmpA0E[0]=Cyc_PP_text(((_tmpA0C=" = open(",_tag_dyneither(_tmpA0C,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA0E,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmpA0A[5]=_tmp501?Cyc_PP_text(((_tmpA0F=" @resetable",
_tag_dyneither(_tmpA0F,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmpA0A[4]=Cyc_Absynpp_qvar2doc(
_tmp500->name),((_tmpA0A[3]=Cyc_PP_text(((_tmpA10=">",_tag_dyneither(_tmpA10,
sizeof(char),2)))),((_tmpA0A[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4FF)),((
_tmpA0A[1]=Cyc_PP_text(((_tmpA11="<",_tag_dyneither(_tmpA11,sizeof(char),2)))),((
_tmpA0A[0]=Cyc_PP_text(((_tmpA12="region",_tag_dyneither(_tmpA12,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmpA0A,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL28F;_LL298: {struct Cyc_Absyn_Alias_d_struct*_tmp503=(struct Cyc_Absyn_Alias_d_struct*)
_tmp4F7;if(_tmp503->tag != 5)goto _LL29A;else{_tmp504=_tmp503->f1;_tmp505=_tmp503->f2;
_tmp506=_tmp503->f3;}}_LL299:{const char*_tmpA1D;const char*_tmpA1C;const char*
_tmpA1B;const char*_tmpA1A;const char*_tmpA19;struct Cyc_PP_Doc*_tmpA18[8];s=((
_tmpA18[7]=Cyc_PP_text(((_tmpA19=";",_tag_dyneither(_tmpA19,sizeof(char),2)))),((
_tmpA18[6]=Cyc_Absynpp_exp2doc(_tmp504),((_tmpA18[5]=Cyc_PP_text(((_tmpA1A=" = ",
_tag_dyneither(_tmpA1A,sizeof(char),4)))),((_tmpA18[4]=Cyc_Absynpp_qvar2doc(
_tmp506->name),((_tmpA18[3]=Cyc_PP_text(((_tmpA1B="> ",_tag_dyneither(_tmpA1B,
sizeof(char),3)))),((_tmpA18[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp505)),((
_tmpA18[1]=Cyc_PP_text(((_tmpA1C=" <",_tag_dyneither(_tmpA1C,sizeof(char),3)))),((
_tmpA18[0]=Cyc_PP_text(((_tmpA1D="alias",_tag_dyneither(_tmpA1D,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmpA18,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL28F;_LL29A: {struct Cyc_Absyn_Datatype_d_struct*_tmp507=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp4F7;if(_tmp507->tag != 7)goto _LL29C;else{_tmp508=_tmp507->f1;}}_LL29B:{const
char*_tmpA20;struct Cyc_PP_Doc*_tmpA1F[2];s=((_tmpA1F[1]=Cyc_PP_text(((_tmpA20=";",
_tag_dyneither(_tmpA20,sizeof(char),2)))),((_tmpA1F[0]=Cyc_Absynpp_datatypedecl2doc(
_tmp508),Cyc_PP_cat(_tag_dyneither(_tmpA1F,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28F;_LL29C: {struct Cyc_Absyn_Let_d_struct*_tmp509=(struct Cyc_Absyn_Let_d_struct*)
_tmp4F7;if(_tmp509->tag != 2)goto _LL29E;else{_tmp50A=_tmp509->f1;_tmp50B=_tmp509->f3;}}
_LL29D:{const char*_tmpA27;const char*_tmpA26;const char*_tmpA25;struct Cyc_PP_Doc*
_tmpA24[5];s=((_tmpA24[4]=Cyc_PP_text(((_tmpA25=";",_tag_dyneither(_tmpA25,
sizeof(char),2)))),((_tmpA24[3]=Cyc_Absynpp_exp2doc(_tmp50B),((_tmpA24[2]=Cyc_PP_text(((
_tmpA26=" = ",_tag_dyneither(_tmpA26,sizeof(char),4)))),((_tmpA24[1]=Cyc_Absynpp_pat2doc(
_tmp50A),((_tmpA24[0]=Cyc_PP_text(((_tmpA27="let ",_tag_dyneither(_tmpA27,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA24,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL28F;_LL29E: {struct Cyc_Absyn_Letv_d_struct*_tmp50C=(struct Cyc_Absyn_Letv_d_struct*)
_tmp4F7;if(_tmp50C->tag != 3)goto _LL2A0;else{_tmp50D=_tmp50C->f1;}}_LL29F:{const
char*_tmpA2C;const char*_tmpA2B;struct Cyc_PP_Doc*_tmpA2A[3];s=((_tmpA2A[2]=Cyc_PP_text(((
_tmpA2B=";",_tag_dyneither(_tmpA2B,sizeof(char),2)))),((_tmpA2A[1]=Cyc_Absynpp_ids2doc(
_tmp50D),((_tmpA2A[0]=Cyc_PP_text(((_tmpA2C="let ",_tag_dyneither(_tmpA2C,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA2A,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL28F;_LL2A0: {struct Cyc_Absyn_Enum_d_struct*_tmp50E=(struct Cyc_Absyn_Enum_d_struct*)
_tmp4F7;if(_tmp50E->tag != 8)goto _LL2A2;else{_tmp50F=_tmp50E->f1;}}_LL2A1:{const
char*_tmpA2F;struct Cyc_PP_Doc*_tmpA2E[2];s=((_tmpA2E[1]=Cyc_PP_text(((_tmpA2F=";",
_tag_dyneither(_tmpA2F,sizeof(char),2)))),((_tmpA2E[0]=Cyc_Absynpp_enumdecl2doc(
_tmp50F),Cyc_PP_cat(_tag_dyneither(_tmpA2E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28F;_LL2A2: {struct Cyc_Absyn_Typedef_d_struct*_tmp510=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp4F7;if(_tmp510->tag != 9)goto _LL2A4;else{_tmp511=_tmp510->f1;}}_LL2A3: {void*
t;if(_tmp511->defn != 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp511->defn))->v;
else{t=Cyc_Absyn_new_evar(_tmp511->kind,0);}{const char*_tmpA39;struct Cyc_Core_Opt*
_tmpA38;struct Cyc_PP_Doc*_tmpA37[2];const char*_tmpA36;struct Cyc_PP_Doc*_tmpA35[4];
s=((_tmpA35[3]=Cyc_PP_text(((_tmpA36=";",_tag_dyneither(_tmpA36,sizeof(char),2)))),((
_tmpA35[2]=Cyc_Absynpp_atts2doc(_tmp511->atts),((_tmpA35[1]=Cyc_Absynpp_tqtd2doc(
_tmp511->tq,t,((_tmpA38=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA38)),((
_tmpA38->v=((_tmpA37[1]=Cyc_Absynpp_tvars2doc(_tmp511->tvs),((_tmpA37[0]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp511->name),Cyc_PP_cat(_tag_dyneither(_tmpA37,sizeof(struct Cyc_PP_Doc*),2)))))),
_tmpA38))))),((_tmpA35[0]=Cyc_PP_text(((_tmpA39="typedef ",_tag_dyneither(
_tmpA39,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA35,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL28F;}_LL2A4: {struct Cyc_Absyn_Namespace_d_struct*_tmp512=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp4F7;if(_tmp512->tag != 10)goto _LL2A6;
else{_tmp513=_tmp512->f1;_tmp514=_tmp512->f2;}}_LL2A5: if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_add(_tmp513);{const char*_tmpA3E;const char*_tmpA3D;
struct Cyc_PP_Doc*_tmpA3C[8];s=((_tmpA3C[7]=Cyc_Absynpp_rb(),((_tmpA3C[6]=Cyc_PP_line_doc(),((
_tmpA3C[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA3D="",_tag_dyneither(_tmpA3D,sizeof(char),1))),_tmp514),((_tmpA3C[4]=Cyc_PP_line_doc(),((
_tmpA3C[3]=Cyc_Absynpp_lb(),((_tmpA3C[2]=Cyc_PP_blank_doc(),((_tmpA3C[1]=Cyc_PP_textptr(
_tmp513),((_tmpA3C[0]=Cyc_PP_text(((_tmpA3E="namespace ",_tag_dyneither(_tmpA3E,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA3C,sizeof(struct Cyc_PP_Doc*),
8))))))))))))))))));}if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL28F;_LL2A6: {struct Cyc_Absyn_Using_d_struct*_tmp515=(struct Cyc_Absyn_Using_d_struct*)
_tmp4F7;if(_tmp515->tag != 11)goto _LL2A8;else{_tmp516=_tmp515->f1;_tmp517=_tmp515->f2;}}
_LL2A7: if(Cyc_Absynpp_print_using_stmts){const char*_tmpA43;const char*_tmpA42;
struct Cyc_PP_Doc*_tmpA41[8];s=((_tmpA41[7]=Cyc_Absynpp_rb(),((_tmpA41[6]=Cyc_PP_line_doc(),((
_tmpA41[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA42="",_tag_dyneither(_tmpA42,sizeof(char),1))),_tmp517),((_tmpA41[4]=Cyc_PP_line_doc(),((
_tmpA41[3]=Cyc_Absynpp_lb(),((_tmpA41[2]=Cyc_PP_blank_doc(),((_tmpA41[1]=Cyc_Absynpp_qvar2doc(
_tmp516),((_tmpA41[0]=Cyc_PP_text(((_tmpA43="using ",_tag_dyneither(_tmpA43,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA41,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmpA4E;const char*_tmpA4D;const char*_tmpA4C;const char*_tmpA4B;
const char*_tmpA4A;struct Cyc_PP_Doc*_tmpA49[11];s=((_tmpA49[10]=Cyc_PP_text(((
_tmpA4A=" */",_tag_dyneither(_tmpA4A,sizeof(char),4)))),((_tmpA49[9]=Cyc_Absynpp_rb(),((
_tmpA49[8]=Cyc_PP_text(((_tmpA4B="/* ",_tag_dyneither(_tmpA4B,sizeof(char),4)))),((
_tmpA49[7]=Cyc_PP_line_doc(),((_tmpA49[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA4C="",_tag_dyneither(_tmpA4C,sizeof(char),1))),
_tmp517),((_tmpA49[5]=Cyc_PP_line_doc(),((_tmpA49[4]=Cyc_PP_text(((_tmpA4D=" */",
_tag_dyneither(_tmpA4D,sizeof(char),4)))),((_tmpA49[3]=Cyc_Absynpp_lb(),((
_tmpA49[2]=Cyc_PP_blank_doc(),((_tmpA49[1]=Cyc_Absynpp_qvar2doc(_tmp516),((
_tmpA49[0]=Cyc_PP_text(((_tmpA4E="/* using ",_tag_dyneither(_tmpA4E,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmpA49,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL28F;_LL2A8: {struct Cyc_Absyn_ExternC_d_struct*_tmp518=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp4F7;if(_tmp518->tag != 12)goto _LL2AA;else{_tmp519=_tmp518->f1;}}_LL2A9: if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA53;const char*_tmpA52;struct Cyc_PP_Doc*_tmpA51[6];s=((_tmpA51[5]=
Cyc_Absynpp_rb(),((_tmpA51[4]=Cyc_PP_line_doc(),((_tmpA51[3]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA52="",_tag_dyneither(_tmpA52,
sizeof(char),1))),_tmp519),((_tmpA51[2]=Cyc_PP_line_doc(),((_tmpA51[1]=Cyc_Absynpp_lb(),((
_tmpA51[0]=Cyc_PP_text(((_tmpA53="extern \"C\" ",_tag_dyneither(_tmpA53,sizeof(
char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA51,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmpA5E;const char*_tmpA5D;const char*_tmpA5C;const char*_tmpA5B;
const char*_tmpA5A;struct Cyc_PP_Doc*_tmpA59[9];s=((_tmpA59[8]=Cyc_PP_text(((
_tmpA5A=" */",_tag_dyneither(_tmpA5A,sizeof(char),4)))),((_tmpA59[7]=Cyc_Absynpp_rb(),((
_tmpA59[6]=Cyc_PP_text(((_tmpA5B="/* ",_tag_dyneither(_tmpA5B,sizeof(char),4)))),((
_tmpA59[5]=Cyc_PP_line_doc(),((_tmpA59[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA5C="",_tag_dyneither(_tmpA5C,sizeof(char),1))),
_tmp519),((_tmpA59[3]=Cyc_PP_line_doc(),((_tmpA59[2]=Cyc_PP_text(((_tmpA5D=" */",
_tag_dyneither(_tmpA5D,sizeof(char),4)))),((_tmpA59[1]=Cyc_Absynpp_lb(),((
_tmpA59[0]=Cyc_PP_text(((_tmpA5E="/* extern \"C\" ",_tag_dyneither(_tmpA5E,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA59,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL28F;_LL2AA: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp51A=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp4F7;if(_tmp51A->tag != 13)
goto _LL2AC;else{_tmp51B=_tmp51A->f1;_tmp51C=_tmp51A->f2;}}_LL2AB: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp51C != 0){const char*_tmpA63;const char*_tmpA62;
struct Cyc_PP_Doc*_tmpA61[7];exs_doc=((_tmpA61[6]=Cyc_Absynpp_rb(),((_tmpA61[5]=
Cyc_PP_line_doc(),((_tmpA61[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmpA62=",",_tag_dyneither(_tmpA62,sizeof(char),2))),
_tmp51C),((_tmpA61[3]=Cyc_PP_line_doc(),((_tmpA61[2]=Cyc_Absynpp_lb(),((_tmpA61[
1]=Cyc_PP_text(((_tmpA63=" export ",_tag_dyneither(_tmpA63,sizeof(char),9)))),((
_tmpA61[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA61,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmpA68;const char*
_tmpA67;struct Cyc_PP_Doc*_tmpA66[6];s=((_tmpA66[5]=exs_doc,((_tmpA66[4]=Cyc_PP_line_doc(),((
_tmpA66[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA67="",_tag_dyneither(_tmpA67,sizeof(char),1))),_tmp51B),((_tmpA66[2]=Cyc_PP_line_doc(),((
_tmpA66[1]=Cyc_Absynpp_lb(),((_tmpA66[0]=Cyc_PP_text(((_tmpA68="extern \"C include\" ",
_tag_dyneither(_tmpA68,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA66,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{const char*_tmpA73;const char*
_tmpA72;const char*_tmpA71;const char*_tmpA70;const char*_tmpA6F;struct Cyc_PP_Doc*
_tmpA6E[9];s=((_tmpA6E[8]=Cyc_PP_text(((_tmpA6F=" */",_tag_dyneither(_tmpA6F,
sizeof(char),4)))),((_tmpA6E[7]=Cyc_Absynpp_rb(),((_tmpA6E[6]=Cyc_PP_text(((
_tmpA70="/* ",_tag_dyneither(_tmpA70,sizeof(char),4)))),((_tmpA6E[5]=Cyc_PP_line_doc(),((
_tmpA6E[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA71="",_tag_dyneither(_tmpA71,sizeof(char),1))),_tmp51B),((_tmpA6E[3]=Cyc_PP_line_doc(),((
_tmpA6E[2]=Cyc_PP_text(((_tmpA72=" */",_tag_dyneither(_tmpA72,sizeof(char),4)))),((
_tmpA6E[1]=Cyc_Absynpp_lb(),((_tmpA6E[0]=Cyc_PP_text(((_tmpA73="/* extern \"C include\" ",
_tag_dyneither(_tmpA73,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA6E,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL28F;_LL2AC: {struct Cyc_Absyn_Porton_d_struct*
_tmp51D=(struct Cyc_Absyn_Porton_d_struct*)_tmp4F7;if(_tmp51D->tag != 14)goto
_LL2AE;}_LL2AD:{const char*_tmpA76;struct Cyc_PP_Doc*_tmpA75[2];s=((_tmpA75[1]=Cyc_Absynpp_lb(),((
_tmpA75[0]=Cyc_PP_text(((_tmpA76="__cyclone_port_on__;",_tag_dyneither(_tmpA76,
sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA75,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL28F;_LL2AE: {struct Cyc_Absyn_Portoff_d_struct*_tmp51E=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp4F7;if(_tmp51E->tag != 15)goto _LL28F;}_LL2AF:{const char*_tmpA79;struct Cyc_PP_Doc*
_tmpA78[2];s=((_tmpA78[1]=Cyc_Absynpp_lb(),((_tmpA78[0]=Cyc_PP_text(((_tmpA79="__cyclone_port_off__;",
_tag_dyneither(_tmpA79,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA78,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL28F;_LL28F:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){if(Cyc_Absynpp_print_for_cycdoc)
return Cyc_PP_nil_doc();switch(sc){case Cyc_Absyn_Static: _LL2B9: {const char*
_tmpA7A;return Cyc_PP_text(((_tmpA7A="static ",_tag_dyneither(_tmpA7A,sizeof(char),
8))));}case Cyc_Absyn_Public: _LL2BA: return Cyc_PP_nil_doc();case Cyc_Absyn_Extern:
_LL2BB: {const char*_tmpA7B;return Cyc_PP_text(((_tmpA7B="extern ",_tag_dyneither(
_tmpA7B,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2BC: {const char*_tmpA7C;
return Cyc_PP_text(((_tmpA7C="extern \"C\" ",_tag_dyneither(_tmpA7C,sizeof(char),
12))));}case Cyc_Absyn_Abstract: _LL2BD: {const char*_tmpA7D;return Cyc_PP_text(((
_tmpA7D="abstract ",_tag_dyneither(_tmpA7D,sizeof(char),10))));}case Cyc_Absyn_Register:
_LL2BE: {const char*_tmpA7E;return Cyc_PP_text(((_tmpA7E="register ",
_tag_dyneither(_tmpA7E,sizeof(char),10))));}}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp575=t;struct Cyc_Absyn_Tvar*_tmp577;struct Cyc_List_List*_tmp579;
_LL2C1: {struct Cyc_Absyn_VarType_struct*_tmp576=(struct Cyc_Absyn_VarType_struct*)
_tmp575;if(_tmp576->tag != 2)goto _LL2C3;else{_tmp577=_tmp576->f1;}}_LL2C2: return
Cyc_Tcutil_is_temp_tvar(_tmp577);_LL2C3: {struct Cyc_Absyn_JoinEff_struct*_tmp578=(
struct Cyc_Absyn_JoinEff_struct*)_tmp575;if(_tmp578->tag != 25)goto _LL2C5;else{
_tmp579=_tmp578->f1;}}_LL2C4: return((int(*)(int(*pred)(void*),struct Cyc_List_List*
x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp579);_LL2C5:;
_LL2C6: return 0;_LL2C0:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp57A=t;
struct Cyc_Absyn_Typedefdecl*_tmp57E;void**_tmp57F;void*_tmp580;_LL2C8: {struct
Cyc_Absyn_AnonAggrType_struct*_tmp57B=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp57A;if(_tmp57B->tag != 13)goto _LL2CA;}_LL2C9: return 1;_LL2CA: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp57C=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp57A;if(_tmp57C->tag != 15)goto
_LL2CC;}_LL2CB: return 1;_LL2CC: {struct Cyc_Absyn_TypedefType_struct*_tmp57D=(
struct Cyc_Absyn_TypedefType_struct*)_tmp57A;if(_tmp57D->tag != 18)goto _LL2CE;
else{_tmp57E=_tmp57D->f3;_tmp57F=_tmp57D->f4;if(_tmp57F == 0)goto _LL2CE;_tmp580=*
_tmp57F;}}_LL2CD: return Cyc_Absynpp_is_anon_aggrtype(_tmp580);_LL2CE:;_LL2CF:
return 0;_LL2C7:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct
_RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmpA7F;struct _tuple16 _tmp582=(_tmpA7F.f1=(void*)tms->hd,((_tmpA7F.f2=(
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA7F)));void*_tmp583;
void*_tmp585;_LL2D1: _tmp583=_tmp582.f1;{struct Cyc_Absyn_Pointer_mod_struct*
_tmp584=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp583;if(_tmp584->tag != 2)goto
_LL2D3;};_tmp585=_tmp582.f2;{struct Cyc_Absyn_Function_mod_struct*_tmp586=(struct
Cyc_Absyn_Function_mod_struct*)_tmp585;if(_tmp586->tag != 3)goto _LL2D3;};_LL2D2: {
struct Cyc_List_List*_tmpA82;struct Cyc_List_List*_tmpA81;return(_tmpA81=
_region_malloc(r,sizeof(*_tmpA81)),((_tmpA81->hd=(void*)tms->hd,((_tmpA81->tl=((
_tmpA82=_region_malloc(r,sizeof(*_tmpA82)),((_tmpA82->hd=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd,((_tmpA82->tl=Cyc_Absynpp_bubble_attributes(r,atts,((
struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA82)))))),_tmpA81)))));}
_LL2D3:;_LL2D4: {struct Cyc_List_List*_tmpA83;return(_tmpA83=_region_malloc(r,
sizeof(*_tmpA83)),((_tmpA83->hd=atts,((_tmpA83->tl=tms,_tmpA83)))));}_LL2D0:;}
else{struct Cyc_List_List*_tmpA84;return(_tmpA84=_region_malloc(r,sizeof(*_tmpA84)),((
_tmpA84->hd=atts,((_tmpA84->tl=tms,_tmpA84)))));}}struct _tuple12 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){void*_tmp58B=t;struct Cyc_Absyn_ArrayInfo
_tmp58D;void*_tmp58E;struct Cyc_Absyn_Tqual _tmp58F;struct Cyc_Absyn_Exp*_tmp590;
union Cyc_Absyn_Constraint*_tmp591;struct Cyc_Position_Segment*_tmp592;struct Cyc_Absyn_PtrInfo
_tmp594;void*_tmp595;struct Cyc_Absyn_Tqual _tmp596;struct Cyc_Absyn_PtrAtts _tmp597;
struct Cyc_Absyn_FnInfo _tmp599;struct Cyc_List_List*_tmp59A;struct Cyc_Core_Opt*
_tmp59B;struct Cyc_Absyn_Tqual _tmp59C;void*_tmp59D;struct Cyc_List_List*_tmp59E;
int _tmp59F;struct Cyc_Absyn_VarargInfo*_tmp5A0;struct Cyc_List_List*_tmp5A1;struct
Cyc_List_List*_tmp5A2;struct Cyc_Core_Opt*_tmp5A4;struct Cyc_Core_Opt*_tmp5A5;int
_tmp5A6;struct _tuple0*_tmp5A8;struct Cyc_List_List*_tmp5A9;struct Cyc_Absyn_Typedefdecl*
_tmp5AA;void**_tmp5AB;_LL2D6: {struct Cyc_Absyn_ArrayType_struct*_tmp58C=(struct
Cyc_Absyn_ArrayType_struct*)_tmp58B;if(_tmp58C->tag != 9)goto _LL2D8;else{_tmp58D=
_tmp58C->f1;_tmp58E=_tmp58D.elt_type;_tmp58F=_tmp58D.tq;_tmp590=_tmp58D.num_elts;
_tmp591=_tmp58D.zero_term;_tmp592=_tmp58D.zt_loc;}}_LL2D7: {struct Cyc_Absyn_Tqual
_tmp5AD;void*_tmp5AE;struct Cyc_List_List*_tmp5AF;struct _tuple12 _tmp5AC=Cyc_Absynpp_to_tms(
r,_tmp58F,_tmp58E);_tmp5AD=_tmp5AC.f1;_tmp5AE=_tmp5AC.f2;_tmp5AF=_tmp5AC.f3;{
void*tm;if(_tmp590 == 0){struct Cyc_Absyn_Carray_mod_struct _tmpA87;struct Cyc_Absyn_Carray_mod_struct*
_tmpA86;tm=(void*)((_tmpA86=_region_malloc(r,sizeof(*_tmpA86)),((_tmpA86[0]=((
_tmpA87.tag=0,((_tmpA87.f1=_tmp591,((_tmpA87.f2=_tmp592,_tmpA87)))))),_tmpA86))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmpA8A;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpA89;tm=(void*)((_tmpA89=_region_malloc(r,sizeof(*_tmpA89)),((_tmpA89[0]=((
_tmpA8A.tag=1,((_tmpA8A.f1=(struct Cyc_Absyn_Exp*)_tmp590,((_tmpA8A.f2=_tmp591,((
_tmpA8A.f3=_tmp592,_tmpA8A)))))))),_tmpA89))));}{struct Cyc_List_List*_tmpA8D;
struct _tuple12 _tmpA8C;return(_tmpA8C.f1=_tmp5AD,((_tmpA8C.f2=_tmp5AE,((_tmpA8C.f3=((
_tmpA8D=_region_malloc(r,sizeof(*_tmpA8D)),((_tmpA8D->hd=tm,((_tmpA8D->tl=
_tmp5AF,_tmpA8D)))))),_tmpA8C)))));};};}_LL2D8: {struct Cyc_Absyn_PointerType_struct*
_tmp593=(struct Cyc_Absyn_PointerType_struct*)_tmp58B;if(_tmp593->tag != 5)goto
_LL2DA;else{_tmp594=_tmp593->f1;_tmp595=_tmp594.elt_typ;_tmp596=_tmp594.elt_tq;
_tmp597=_tmp594.ptr_atts;}}_LL2D9: {struct Cyc_Absyn_Tqual _tmp5B7;void*_tmp5B8;
struct Cyc_List_List*_tmp5B9;struct _tuple12 _tmp5B6=Cyc_Absynpp_to_tms(r,_tmp596,
_tmp595);_tmp5B7=_tmp5B6.f1;_tmp5B8=_tmp5B6.f2;_tmp5B9=_tmp5B6.f3;{struct Cyc_Absyn_Pointer_mod_struct*
_tmpA93;struct Cyc_Absyn_Pointer_mod_struct _tmpA92;struct Cyc_List_List*_tmpA91;
_tmp5B9=((_tmpA91=_region_malloc(r,sizeof(*_tmpA91)),((_tmpA91->hd=(void*)((
_tmpA93=_region_malloc(r,sizeof(*_tmpA93)),((_tmpA93[0]=((_tmpA92.tag=2,((
_tmpA92.f1=_tmp597,((_tmpA92.f2=tq,_tmpA92)))))),_tmpA93)))),((_tmpA91->tl=
_tmp5B9,_tmpA91))))));}{struct _tuple12 _tmpA94;return(_tmpA94.f1=_tmp5B7,((
_tmpA94.f2=_tmp5B8,((_tmpA94.f3=_tmp5B9,_tmpA94)))));};}_LL2DA: {struct Cyc_Absyn_FnType_struct*
_tmp598=(struct Cyc_Absyn_FnType_struct*)_tmp58B;if(_tmp598->tag != 10)goto _LL2DC;
else{_tmp599=_tmp598->f1;_tmp59A=_tmp599.tvars;_tmp59B=_tmp599.effect;_tmp59C=
_tmp599.ret_tqual;_tmp59D=_tmp599.ret_typ;_tmp59E=_tmp599.args;_tmp59F=_tmp599.c_varargs;
_tmp5A0=_tmp599.cyc_varargs;_tmp5A1=_tmp599.rgn_po;_tmp5A2=_tmp599.attributes;}}
_LL2DB: if(!Cyc_Absynpp_print_all_tvars){if(_tmp59B == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)_tmp59B->v)){_tmp59B=0;_tmp59A=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar,_tmp59A);}{struct Cyc_Absyn_Tqual _tmp5BF;void*
_tmp5C0;struct Cyc_List_List*_tmp5C1;struct _tuple12 _tmp5BE=Cyc_Absynpp_to_tms(r,
_tmp59C,_tmp59D);_tmp5BF=_tmp5BE.f1;_tmp5C0=_tmp5BE.f2;_tmp5C1=_tmp5BE.f3;{
struct Cyc_List_List*tms=_tmp5C1;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL2E2: if(_tmp5A2 != 0){struct Cyc_Absyn_Attributes_mod_struct _tmpA97;struct Cyc_Absyn_Attributes_mod_struct*
_tmpA96;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA96=_region_malloc(r,
sizeof(*_tmpA96)),((_tmpA96[0]=((_tmpA97.tag=5,((_tmpA97.f1=0,((_tmpA97.f2=
_tmp5A2,_tmpA97)))))),_tmpA96)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmpAA6;struct Cyc_Absyn_WithTypes_struct*_tmpAA5;struct Cyc_Absyn_WithTypes_struct
_tmpAA4;struct Cyc_Absyn_Function_mod_struct _tmpAA3;struct Cyc_List_List*_tmpAA2;
tms=((_tmpAA2=_region_malloc(r,sizeof(*_tmpAA2)),((_tmpAA2->hd=(void*)((_tmpAA6=
_region_malloc(r,sizeof(*_tmpAA6)),((_tmpAA6[0]=((_tmpAA3.tag=3,((_tmpAA3.f1=(
void*)((void*)((_tmpAA5=_region_malloc(r,sizeof(*_tmpAA5)),((_tmpAA5[0]=((
_tmpAA4.tag=1,((_tmpAA4.f1=_tmp59E,((_tmpAA4.f2=_tmp59F,((_tmpAA4.f3=_tmp5A0,((
_tmpAA4.f4=_tmp59B,((_tmpAA4.f5=_tmp5A1,_tmpAA4)))))))))))),_tmpAA5))))),_tmpAA3)))),
_tmpAA6)))),((_tmpAA2->tl=tms,_tmpAA2))))));}break;case Cyc_Cyclone_Vc_c: _LL2E3:{
struct Cyc_Absyn_Function_mod_struct*_tmpAB5;struct Cyc_Absyn_WithTypes_struct*
_tmpAB4;struct Cyc_Absyn_WithTypes_struct _tmpAB3;struct Cyc_Absyn_Function_mod_struct
_tmpAB2;struct Cyc_List_List*_tmpAB1;tms=((_tmpAB1=_region_malloc(r,sizeof(*
_tmpAB1)),((_tmpAB1->hd=(void*)((_tmpAB5=_region_malloc(r,sizeof(*_tmpAB5)),((
_tmpAB5[0]=((_tmpAB2.tag=3,((_tmpAB2.f1=(void*)((void*)((_tmpAB4=_region_malloc(
r,sizeof(*_tmpAB4)),((_tmpAB4[0]=((_tmpAB3.tag=1,((_tmpAB3.f1=_tmp59E,((_tmpAB3.f2=
_tmp59F,((_tmpAB3.f3=_tmp5A0,((_tmpAB3.f4=_tmp59B,((_tmpAB3.f5=_tmp5A1,_tmpAB3)))))))))))),
_tmpAB4))))),_tmpAB2)))),_tmpAB5)))),((_tmpAB1->tl=tms,_tmpAB1))))));}for(0;
_tmp5A2 != 0;_tmp5A2=_tmp5A2->tl){void*_tmp5CE=(void*)_tmp5A2->hd;_LL2E6: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp5CF=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp5CE;if(_tmp5CF->tag != 1)goto _LL2E8;}_LL2E7: goto _LL2E9;_LL2E8: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp5D0=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp5CE;if(_tmp5D0->tag != 2)goto
_LL2EA;}_LL2E9: goto _LL2EB;_LL2EA: {struct Cyc_Absyn_Fastcall_att_struct*_tmp5D1=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp5CE;if(_tmp5D1->tag != 3)goto _LL2EC;}
_LL2EB:{struct Cyc_Absyn_Attributes_mod_struct*_tmpABF;struct Cyc_List_List*
_tmpABE;struct Cyc_Absyn_Attributes_mod_struct _tmpABD;struct Cyc_List_List*_tmpABC;
tms=((_tmpABC=_region_malloc(r,sizeof(*_tmpABC)),((_tmpABC->hd=(void*)((_tmpABF=
_region_malloc(r,sizeof(*_tmpABF)),((_tmpABF[0]=((_tmpABD.tag=5,((_tmpABD.f1=0,((
_tmpABD.f2=((_tmpABE=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpABE)),((
_tmpABE->hd=(void*)_tmp5A2->hd,((_tmpABE->tl=0,_tmpABE)))))),_tmpABD)))))),
_tmpABF)))),((_tmpABC->tl=tms,_tmpABC))))));}goto AfterAtts;_LL2EC:;_LL2ED: goto
_LL2E5;_LL2E5:;}break;}AfterAtts: if(_tmp59A != 0){struct Cyc_Absyn_TypeParams_mod_struct*
_tmpAC5;struct Cyc_Absyn_TypeParams_mod_struct _tmpAC4;struct Cyc_List_List*_tmpAC3;
tms=((_tmpAC3=_region_malloc(r,sizeof(*_tmpAC3)),((_tmpAC3->hd=(void*)((_tmpAC5=
_region_malloc(r,sizeof(*_tmpAC5)),((_tmpAC5[0]=((_tmpAC4.tag=4,((_tmpAC4.f1=
_tmp59A,((_tmpAC4.f2=0,((_tmpAC4.f3=1,_tmpAC4)))))))),_tmpAC5)))),((_tmpAC3->tl=
tms,_tmpAC3))))));}{struct _tuple12 _tmpAC6;return(_tmpAC6.f1=_tmp5BF,((_tmpAC6.f2=
_tmp5C0,((_tmpAC6.f3=tms,_tmpAC6)))));};};};_LL2DC: {struct Cyc_Absyn_Evar_struct*
_tmp5A3=(struct Cyc_Absyn_Evar_struct*)_tmp58B;if(_tmp5A3->tag != 1)goto _LL2DE;
else{_tmp5A4=_tmp5A3->f1;_tmp5A5=_tmp5A3->f2;_tmp5A6=_tmp5A3->f3;}}_LL2DD: if(
_tmp5A5 == 0){struct _tuple12 _tmpAC7;return(_tmpAC7.f1=tq,((_tmpAC7.f2=t,((_tmpAC7.f3=
0,_tmpAC7)))));}else{return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp5A5->v);}_LL2DE: {
struct Cyc_Absyn_TypedefType_struct*_tmp5A7=(struct Cyc_Absyn_TypedefType_struct*)
_tmp58B;if(_tmp5A7->tag != 18)goto _LL2E0;else{_tmp5A8=_tmp5A7->f1;_tmp5A9=_tmp5A7->f2;
_tmp5AA=_tmp5A7->f3;_tmp5AB=_tmp5A7->f4;}}_LL2DF: if((_tmp5AB == 0  || !Cyc_Absynpp_expand_typedefs)
 || Cyc_Absynpp_is_anon_aggrtype(*_tmp5AB)){struct _tuple12 _tmpAC8;return(_tmpAC8.f1=
tq,((_tmpAC8.f2=t,((_tmpAC8.f3=0,_tmpAC8)))));}else{if(tq.real_const)tq.print_const=
tq.real_const;return Cyc_Absynpp_to_tms(r,tq,*_tmp5AB);}_LL2E0:;_LL2E1: {struct
_tuple12 _tmpAC9;return(_tmpAC9.f1=tq,((_tmpAC9.f2=t,((_tmpAC9.f3=0,_tmpAC9)))));}
_LL2D5:;}static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp5DD=t;struct Cyc_Core_Opt*
_tmp5DF;struct Cyc_Core_Opt _tmp5E0;void*_tmp5E1;struct Cyc_Absyn_PtrInfo _tmp5E3;
void*_tmp5E4;_LL2EF: {struct Cyc_Absyn_Evar_struct*_tmp5DE=(struct Cyc_Absyn_Evar_struct*)
_tmp5DD;if(_tmp5DE->tag != 1)goto _LL2F1;else{_tmp5DF=_tmp5DE->f2;if(_tmp5DF == 0)
goto _LL2F1;_tmp5E0=*_tmp5DF;_tmp5E1=(void*)_tmp5E0.v;}}_LL2F0: return Cyc_Absynpp_is_char_ptr(
_tmp5E1);_LL2F1: {struct Cyc_Absyn_PointerType_struct*_tmp5E2=(struct Cyc_Absyn_PointerType_struct*)
_tmp5DD;if(_tmp5E2->tag != 5)goto _LL2F3;else{_tmp5E3=_tmp5E2->f1;_tmp5E4=_tmp5E3.elt_typ;}}
_LL2F2: L: {void*_tmp5E5=_tmp5E4;struct Cyc_Core_Opt*_tmp5E7;struct Cyc_Core_Opt
_tmp5E8;void*_tmp5E9;void**_tmp5EB;void*_tmp5EC;enum Cyc_Absyn_Size_of _tmp5EE;
_LL2F6: {struct Cyc_Absyn_Evar_struct*_tmp5E6=(struct Cyc_Absyn_Evar_struct*)
_tmp5E5;if(_tmp5E6->tag != 1)goto _LL2F8;else{_tmp5E7=_tmp5E6->f2;if(_tmp5E7 == 0)
goto _LL2F8;_tmp5E8=*_tmp5E7;_tmp5E9=(void*)_tmp5E8.v;}}_LL2F7: _tmp5E4=_tmp5E9;
goto L;_LL2F8: {struct Cyc_Absyn_TypedefType_struct*_tmp5EA=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5E5;if(_tmp5EA->tag != 18)goto _LL2FA;else{_tmp5EB=_tmp5EA->f4;if(_tmp5EB == 0)
goto _LL2FA;_tmp5EC=*_tmp5EB;}}_LL2F9: _tmp5E4=_tmp5EC;goto L;_LL2FA: {struct Cyc_Absyn_IntType_struct*
_tmp5ED=(struct Cyc_Absyn_IntType_struct*)_tmp5E5;if(_tmp5ED->tag != 6)goto _LL2FC;
else{_tmp5EE=_tmp5ED->f2;if(_tmp5EE != Cyc_Absyn_Char_sz)goto _LL2FC;}}_LL2FB:
return 1;_LL2FC:;_LL2FD: return 0;_LL2F5:;}_LL2F3:;_LL2F4: return 0;_LL2EE:;}struct
Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*
dopt){struct _RegionHandle _tmp5EF=_new_region("temp");struct _RegionHandle*temp=&
_tmp5EF;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp5F1;void*_tmp5F2;struct Cyc_List_List*
_tmp5F3;struct _tuple12 _tmp5F0=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp5F1=_tmp5F0.f1;
_tmp5F2=_tmp5F0.f2;_tmp5F3=_tmp5F0.f3;_tmp5F3=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5F3);if(_tmp5F3 == 0  && dopt == 0){struct
Cyc_PP_Doc*_tmpACA[2];struct Cyc_PP_Doc*_tmp5F5=(_tmpACA[1]=Cyc_Absynpp_ntyp2doc(
_tmp5F2),((_tmpACA[0]=Cyc_Absynpp_tqual2doc(_tmp5F1),Cyc_PP_cat(_tag_dyneither(
_tmpACA,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5F5;}else{
const char*_tmpACD;struct Cyc_PP_Doc*_tmpACC[4];struct Cyc_PP_Doc*_tmp5F8=(_tmpACC[
3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*)dopt->v,_tmp5F3),((_tmpACC[2]=Cyc_PP_text(((_tmpACD=" ",
_tag_dyneither(_tmpACD,sizeof(char),2)))),((_tmpACC[1]=Cyc_Absynpp_ntyp2doc(
_tmp5F2),((_tmpACC[0]=Cyc_Absynpp_tqual2doc(_tmp5F1),Cyc_PP_cat(_tag_dyneither(
_tmpACC,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5F8;}};
_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*
f){switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2FE: if(f->width != 0){
struct Cyc_Core_Opt*_tmpAD4;const char*_tmpAD3;const char*_tmpAD2;struct Cyc_PP_Doc*
_tmpAD1[5];return(_tmpAD1[4]=Cyc_PP_text(((_tmpAD2=";",_tag_dyneither(_tmpAD2,
sizeof(char),2)))),((_tmpAD1[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpAD1[2]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAD1[1]=
Cyc_PP_text(((_tmpAD3=":",_tag_dyneither(_tmpAD3,sizeof(char),2)))),((_tmpAD1[0]=
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAD4=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAD4)),((_tmpAD4->v=Cyc_PP_textptr(f->name),_tmpAD4))))),Cyc_PP_cat(
_tag_dyneither(_tmpAD1,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpAD9;const char*_tmpAD8;struct Cyc_PP_Doc*_tmpAD7[3];return(_tmpAD7[2]=Cyc_PP_text(((
_tmpAD8=";",_tag_dyneither(_tmpAD8,sizeof(char),2)))),((_tmpAD7[1]=Cyc_Absynpp_atts2doc(
f->attributes),((_tmpAD7[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAD9=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAD9)),((_tmpAD9->v=Cyc_PP_textptr(
f->name),_tmpAD9))))),Cyc_PP_cat(_tag_dyneither(_tmpAD7,sizeof(struct Cyc_PP_Doc*),
3)))))));}case Cyc_Cyclone_Vc_c: _LL2FF: if(f->width != 0){struct Cyc_Core_Opt*
_tmpAE0;const char*_tmpADF;const char*_tmpADE;struct Cyc_PP_Doc*_tmpADD[5];return(
_tmpADD[4]=Cyc_PP_text(((_tmpADE=";",_tag_dyneither(_tmpADE,sizeof(char),2)))),((
_tmpADD[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((
_tmpADD[2]=Cyc_PP_text(((_tmpADF=":",_tag_dyneither(_tmpADF,sizeof(char),2)))),((
_tmpADD[1]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAE0=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAE0)),((_tmpAE0->v=Cyc_PP_textptr(f->name),_tmpAE0))))),((_tmpADD[0]=
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpADD,sizeof(
struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*_tmpAE5;const char*
_tmpAE4;struct Cyc_PP_Doc*_tmpAE3[3];return(_tmpAE3[2]=Cyc_PP_text(((_tmpAE4=";",
_tag_dyneither(_tmpAE4,sizeof(char),2)))),((_tmpAE3[1]=Cyc_Absynpp_tqtd2doc(f->tq,
f->type,((_tmpAE5=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAE5)),((
_tmpAE5->v=Cyc_PP_textptr(f->name),_tmpAE5))))),((_tmpAE3[0]=Cyc_Absynpp_atts2doc(
f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAE3,sizeof(struct Cyc_PP_Doc*),3)))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){const
char*_tmpAE6;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((
_tmpAE6="",_tag_dyneither(_tmpAE6,sizeof(char),1))),fields);}struct Cyc_PP_Doc*
Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*
_tmpAE7[3];return(_tmpAE7[2]=f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((
_tmpAE7[1]=Cyc_Absynpp_typedef_name2doc(f->name),((_tmpAE7[0]=Cyc_Absynpp_scope2doc(
f->sc),Cyc_PP_cat(_tag_dyneither(_tmpAE7,sizeof(struct Cyc_PP_Doc*),3)))))));}
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpAE8;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((
_tmpAE8=",",_tag_dyneither(_tmpAE8,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmpAEB;
void*_tmpAEA;(_tmpAEA=0,Cyc_fprintf(f,((_tmpAEB="\n",_tag_dyneither(_tmpAEB,
sizeof(char),2))),_tag_dyneither(_tmpAEA,sizeof(void*),0)));};}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmpAEC;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAEC="",_tag_dyneither(
_tmpAEC,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
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
