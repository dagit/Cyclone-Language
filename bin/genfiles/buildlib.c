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
 void exit(int);struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};int Cyc_open(
const char*,int,struct _dyneither_ptr);struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stdout;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_struct{int
tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_fflush(struct
Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_fputc(int,
struct Cyc___cycFILE*);int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int remove(const char*);int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};int isspace(int);int toupper(int);int system(const char*);
int mkdir(const char*pathname,unsigned short mode);int close(int);int chdir(const char*);
struct _dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long size);struct
Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct _dyneither_ptr Cstring_to_string(char*);inline static void* arrcast(struct
_dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || 
dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}extern
char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int
lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;
int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(
struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf*,int);int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*
x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*
cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);struct
Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char
Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Iter_Iter Cyc_Set_make_iter(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s);unsigned long Cyc_strlen(struct
_dyneither_ptr s);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*
s2);struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);struct _dyneither_ptr Cyc_substring(
struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct
Cyc_Hashtable_Table*t,void*key);int Cyc_Hashtable_hash_stringptr(struct
_dyneither_ptr*p);struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Filename_chop_extension(struct
_dyneither_ptr);struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};void Cyc_Position_reset_position(
struct _dyneither_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};extern char Cyc_Arg_Bad[4];struct
Cyc_Arg_Bad_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];
struct Cyc_Arg_Error_struct{char*tag;};struct Cyc_Arg_Unit_spec_struct{int tag;void(*
f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct _dyneither_ptr,
struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{
int tag;int*f1;};struct Cyc_Arg_String_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*f1)(int);};struct
Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};void Cyc_Arg_usage(
struct Cyc_List_List*,struct _dyneither_ptr);void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct
_dyneither_ptr args);struct Cyc_Buffer_t;struct Cyc_Buffer_t*Cyc_Buffer_create(
unsigned int n);struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);void
Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);void Cyc_Buffer_add_string(struct
Cyc_Buffer_t*,struct _dyneither_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
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
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*
ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};
struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*
bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};
struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts
ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{
int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{
int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{int tag;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
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
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};struct _tuple9{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};struct
_tuple9 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc
 = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,
Cyc_Abstract_sc  = 6};struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple10{
int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct
_tuple10 val;};struct _tuple11{struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*
f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple11*
val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple12{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple12*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple13{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple13*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec*val;};struct
_tuple14{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator*val;};struct
_union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope
val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};
struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple15{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple7*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple16{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple16*val;};
struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{
int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};
struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct
_union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct
_union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{
int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};
struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct
_union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct
_union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok
Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct
_union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct
_union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;
struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7
YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct
_union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12
YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct
_union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17
YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct
_union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22
YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct
_union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27
YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct
_union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32
YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct
_union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37
YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct
_union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42
YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct
_union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47
YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct
_union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52
YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct
_union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int
first_line;int first_column;int last_line;int last_column;};struct Cyc_Dict_T;struct
Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tc_tc(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);void Cyc_Lex_lex_init(
int use_cyclone_keywords);extern char*Ccomp;static int Cyc_do_setjmp=0;struct Cyc___cycFILE*
Cyc_log_file=0;struct Cyc___cycFILE*Cyc_cstubs_file=0;struct Cyc___cycFILE*Cyc_cycstubs_file=
0;int Cyc_log(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){if(Cyc_log_file == 0){{
const char*_tmp3D1;void*_tmp3D0;(_tmp3D0=0,Cyc_fprintf(Cyc_stderr,((_tmp3D1="Internal error: log file is NULL\n",
_tag_dyneither(_tmp3D1,sizeof(char),34))),_tag_dyneither(_tmp3D0,sizeof(void*),0)));}
exit(1);}{int _tmp2=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),
fmt,ap);Cyc_fflush((struct Cyc___cycFILE*)((struct Cyc___cycFILE*)_check_null(Cyc_log_file)));
return _tmp2;};}static struct _dyneither_ptr*Cyc_current_source=0;static struct Cyc_List_List*
Cyc_current_args=0;static struct Cyc_Set_Set**Cyc_current_targets=0;static void Cyc_add_target(
struct _dyneither_ptr*sptr){struct Cyc_Set_Set**_tmp3D2;Cyc_current_targets=((
_tmp3D2=_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2[0]=((struct Cyc_Set_Set*(*)(struct
Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets)),sptr),_tmp3D2))));}struct _tuple17{struct
_dyneither_ptr*f1;struct Cyc_Set_Set*f2;};struct _tuple17*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);int Cyc_string(struct Cyc_Lexing_lexbuf*);
struct Cyc___cycFILE*Cyc_slurp_out=0;int Cyc_slurp(struct Cyc_Lexing_lexbuf*);int
Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);int Cyc_asmtok(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);
struct _tuple18{struct _dyneither_ptr f1;struct _dyneither_ptr*f2;};struct _tuple18*
Cyc_suck_line(struct Cyc_Lexing_lexbuf*);int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);struct _dyneither_ptr Cyc_current_line=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};struct _tuple19{struct
_dyneither_ptr f1;struct _dyneither_ptr f2;};struct _tuple20{struct _dyneither_ptr f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*
f5;struct Cyc_List_List*f6;};struct _tuple20*Cyc_spec(struct Cyc_Lexing_lexbuf*);
int Cyc_commands(struct Cyc_Lexing_lexbuf*);int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);
int Cyc_block(struct Cyc_Lexing_lexbuf*);int Cyc_block_string(struct Cyc_Lexing_lexbuf*);
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);struct _dyneither_ptr Cyc_current_headerfile=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};struct Cyc_List_List*Cyc_snarfed_symbols=
0;struct Cyc_List_List*Cyc_current_symbols=0;struct Cyc_List_List*Cyc_current_cstubs=
0;struct Cyc_List_List*Cyc_current_cycstubs=0;struct Cyc_List_List*Cyc_current_hstubs=
0;struct Cyc_List_List*Cyc_current_omit_symbols=0;struct Cyc_List_List*Cyc_current_cpp=
0;struct Cyc_Buffer_t*Cyc_specbuf=0;int Cyc_braces_to_match=0;int Cyc_parens_to_match=
0;int Cyc_numdef=0;const int Cyc_lex_base[425]=(const int[425]){0,0,75,192,305,310,
311,166,312,91,27,384,28,523,637,715,283,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,
166,- 5,605,797,312,- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 14,223,12,- 2,216,20,26,28,
33,23,48,70,54,64,74,100,91,107,94,370,386,111,103,95,122,122,375,414,111,111,
153,377,1024,1139,525,176,210,228,214,216,217,242,498,1253,1368,- 9,654,- 10,224,
245,508,1482,1597,684,- 8,720,- 11,432,510,515,1674,1751,1828,1909,434,465,550,
1984,249,249,249,248,244,254,0,13,4,2065,5,628,2073,2138,660,49,467,6,2099,7,705,
2161,2199,820,- 22,1051,1056,261,314,242,251,258,250,271,281,274,275,278,288,295,
282,- 20,292,296,300,313,321,326,- 15,309,325,319,315,326,365,400,417,418,404,400,
400,427,431,- 17,422,421,415,436,433,449,427,449,453,441,445,436,436,- 19,444,438,
442,453,464,447,449,482,489,490,4,6,21,491,492,505,504,516,516,524,554,24,556,
557,24,20,570,571,53,627,644,- 13,647,594,596,583,584,598,598,605,606,1,677,623,
624,654,647,654,658,659,623,624,639,645,648,703,704,705,654,655,710,720,721,679,
680,735,737,765,713,714,770,797,798,746,747,803,814,815,- 12,762,763,1030,- 21,
1165,762,763,760,773,772,767,769,772,773,771,827,1144,819,820,818,832,1258,1170,
871,872,862,864,862,875,1370,893,894,892,905,1375,- 7,- 8,8,1263,2231,9,996,2255,
2293,1341,1279,- 49,1150,- 2,945,- 4,974,998,1028,980,1010,1023,1448,981,2320,2363,
994,1055,993,996,2433,994,1057,- 36,- 42,- 37,2508,- 28,1002,- 40,- 25,1021,- 27,- 45,-
39,- 48,2583,2612,1467,1081,1091,1563,2622,2652,1582,2281,2685,2716,2754,1083,
1093,2824,2862,1118,1157,1195,1206,1198,1241,- 6,- 34,1040,2794,- 47,- 30,- 32,- 46,-
29,- 31,- 33,1048,2902,1112,1117,2128,1119,1124,1125,1133,1134,1138,1146,1147,1191,
2975,3059,- 23,- 16,- 18,2239,1192,- 24,- 41,- 38,- 35,- 26,1394,3141,2,3224,1191,15,
1170,1174,1175,1173,1171,1185,1255};const int Cyc_lex_backtrk[425]=(const int[425]){
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,
- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,- 1,0,1,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,-
1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,6,5,2,8,3,5,- 1,6,5,- 1,21,21,21,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,
48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,
4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[425]=(const int[425]){- 1,- 1,- 1,317,306,138,23,36,23,19,- 1,
- 1,12,31,46,31,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,
- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,-
1,- 1,- 1,0,- 1,0,103,- 1,- 1,- 1,- 1,- 1,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,386,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,-
1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_trans[
3481]=(const int[3481]){0,0,0,0,0,0,0,0,0,0,22,19,28,417,19,28,19,28,100,19,45,45,
45,45,45,22,45,0,0,0,0,0,21,234,21,418,203,22,- 1,- 1,22,- 1,- 1,45,204,45,224,22,
415,415,415,415,415,415,415,415,415,415,31,103,22,205,114,40,214,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,31,217,218,221,415,130,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,413,413,413,413,413,413,
413,413,413,413,121,20,74,67,54,55,56,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,57,58,59,60,413,
61,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,62,63,37,318,319,318,318,319,39,22,64,65,68,69,70,
129,34,34,34,34,34,34,34,34,71,72,318,320,321,75,76,322,323,324,104,104,325,326,
104,327,328,329,330,331,331,331,331,331,331,331,331,331,332,77,333,334,335,104,
19,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,19,- 1,- 1,337,336,101,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,307,338,
37,308,139,139,24,24,139,131,122,94,84,81,85,38,82,86,28,87,24,29,83,25,309,88,
95,139,96,22,26,26,21,21,115,116,117,140,118,119,120,142,192,193,26,35,35,35,35,
35,35,35,35,178,172,163,30,30,30,30,30,30,30,30,66,66,156,149,66,73,73,78,78,73,
150,78,151,152,153,154,66,66,310,155,66,157,158,66,132,123,141,39,73,22,78,159,
143,144,145,160,146,161,27,66,147,31,162,21,73,73,164,165,73,148,166,167,168,113,
113,113,113,113,113,113,113,113,113,- 1,32,- 1,- 1,73,- 1,22,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
- 1,169,- 1,- 1,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,89,89,28,170,89,171,100,173,174,175,
97,97,104,104,97,176,104,112,112,177,179,112,180,89,181,105,105,80,80,105,19,80,
21,97,182,104,183,184,185,186,112,187,188,189,190,191,271,194,105,195,80,196,112,
112,91,197,112,198,199,21,21,21,106,107,106,106,106,106,106,106,106,106,106,106,
21,112,200,201,202,206,207,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,208,209,210,211,106,212,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,47,47,35,213,47,215,216,33,33,33,33,33,33,33,33,33,33,92,92,
219,220,92,222,47,33,33,33,33,33,33,128,128,128,128,128,128,128,128,48,223,92,
225,- 1,226,- 1,227,228,99,99,229,49,99,230,231,232,233,33,33,33,33,33,33,35,35,35,
35,35,35,35,35,99,235,236,237,265,- 1,238,- 1,41,41,239,260,41,101,101,255,250,101,
243,240,50,241,244,245,246,51,52,247,248,249,46,41,53,251,252,253,101,137,137,
137,137,137,137,137,137,254,93,42,42,42,42,42,42,42,42,42,42,242,256,257,258,93,
259,28,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,102,100,261,262,42,263,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,264,91,43,266,267,102,268,33,33,33,33,33,33,33,33,33,
33,269,270,272,273,288,283,279,33,33,33,33,33,33,35,35,35,35,35,35,35,35,280,281,
282,36,284,285,286,44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,287,289,290,
291,44,292,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,301,296,297,28,298,299,300,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,302,303,304,305,
44,411,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,78,78,410,408,78,- 1,273,273,385,376,273,315,315,315,315,315,315,315,315,
351,383,346,341,78,343,344,409,293,293,273,345,293,275,275,- 1,380,275,274,384,79,
79,79,79,79,79,79,79,79,79,350,293,381,382,379,407,275,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,347,348,349,339,79,387,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,31,80,
80,294,155,80,287,287,276,191,287,274,412,295,412,412,404,102,277,100,100,35,35,
278,80,270,223,275,275,287,46,275,293,293,340,412,293,19,162,403,79,79,79,79,79,
79,79,79,79,79,275,100,100,35,35,293,31,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,177,138,21,31,79,31,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,276,31,89,292,292,
294,419,292,316,316,277,420,316,421,422,295,36,423,36,375,- 1,89,424,21,316,316,
292,0,316,0,0,316,0,21,31,0,0,90,90,90,90,90,90,90,90,90,90,316,36,21,36,375,0,
375,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,0,0,0,0,90,375,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,91,92,92,300,300,92,0,300,305,305,0,0,305,31,31,31,31,31,31,31,31,
0,0,0,92,0,300,412,0,412,412,305,0,0,28,0,0,0,0,35,90,90,90,90,90,90,90,90,90,90,
412,0,0,0,0,0,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,0,0,0,0,90,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,97,97,0,377,97,378,378,378,378,378,378,378,378,378,
378,0,0,0,0,357,0,357,0,97,358,358,358,358,358,358,358,358,358,358,0,0,0,0,0,98,
98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,36,99,99,0,0,99,358,358,358,
358,358,358,358,358,358,358,0,0,0,0,361,0,361,0,99,362,362,362,362,362,362,362,
362,362,362,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,108,
108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,109,108,108,108,108,108,108,108,
108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,- 1,0,0,- 1,108,0,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,109,109,109,
109,109,109,109,109,109,109,109,109,111,0,0,0,0,0,0,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,0,0,
0,0,109,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,0,0,
0,0,0,0,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,28,0,0,124,113,0,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,0,0,
28,0,0,133,125,125,125,125,125,125,125,125,127,127,127,127,127,127,127,127,127,
127,0,0,0,0,0,0,0,127,127,127,127,127,127,0,0,0,134,134,134,134,134,134,134,134,
0,0,0,0,0,0,0,31,0,0,- 1,0,138,0,0,127,127,127,127,127,127,405,405,405,405,405,
405,405,405,0,126,127,127,127,127,127,127,127,127,127,127,31,0,0,0,0,0,0,127,127,
127,127,127,127,136,136,136,136,136,136,136,136,136,136,135,0,0,0,0,0,0,136,136,
136,136,136,136,0,0,0,127,127,127,127,127,127,19,0,0,311,0,0,136,136,136,136,136,
136,136,136,136,136,0,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,
0,0,0,138,312,312,312,312,312,312,312,312,406,406,406,406,406,406,406,406,0,136,
136,136,136,136,136,0,314,314,314,314,314,314,314,314,314,314,0,0,0,0,0,0,0,314,
314,314,314,314,314,0,0,28,362,362,362,362,362,362,362,362,362,362,0,0,314,314,
314,314,314,314,314,314,314,314,313,314,314,314,314,314,314,314,314,314,314,314,
314,0,0,352,0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,
314,314,314,314,314,314,365,0,0,0,0,0,0,0,0,366,0,0,367,352,0,353,353,353,353,
353,353,353,353,353,353,354,0,0,0,0,0,0,365,0,0,0,354,0,0,0,0,366,0,355,367,0,0,
0,0,0,0,0,356,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,354,0,0,0,0,0,0,355,0,0,0,0,0,0,0,0,
356,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,0,0,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,0,0,0,0,0,0,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,342,0,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,359,359,359,359,359,359,359,359,359,359,0,0,0,0,0,0,0,0,0,0,0,360,93,0,0,0,0,
352,93,353,353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,
358,358,0,354,0,0,360,93,0,0,355,0,0,93,91,0,0,0,0,356,91,0,359,359,359,359,359,
359,359,359,359,359,0,0,0,354,0,0,0,0,0,0,355,360,93,0,91,0,0,0,93,356,91,0,0,
362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,0,0,0,0,0,360,93,93,0,0,0,0,93,
93,352,0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,0,93,
0,0,0,0,373,93,0,0,0,0,0,0,352,374,364,364,364,364,364,364,364,364,364,364,0,0,0,
0,0,354,0,0,0,0,0,354,373,0,0,0,0,0,371,0,0,374,0,0,0,0,0,372,0,0,378,378,378,
378,378,378,378,378,378,378,0,0,0,354,0,0,0,0,0,0,371,360,93,0,0,0,0,0,93,372,
368,368,368,368,368,368,368,368,368,368,0,0,0,0,0,0,0,368,368,368,368,368,368,
360,93,0,0,0,0,0,93,0,0,0,0,0,0,0,368,368,368,368,368,368,368,368,368,368,0,368,
368,368,368,368,368,368,368,368,368,368,368,0,0,0,388,0,369,0,0,389,0,0,0,0,0,
370,0,0,390,390,390,390,390,390,390,390,0,368,368,368,368,368,368,391,0,0,0,0,
369,0,0,0,0,0,0,0,0,370,0,0,0,0,0,0,0,0,0,0,0,0,0,0,392,0,0,0,0,393,394,0,0,0,
395,0,0,0,0,0,0,0,396,0,0,0,397,0,398,0,399,0,400,401,401,401,401,401,401,401,
401,401,401,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,402,0,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,
0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,19,0,0,414,0,0,0,
413,413,413,413,413,413,413,413,413,413,0,0,0,0,0,0,0,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
0,0,0,0,413,0,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,416,0,0,0,0,0,0,0,415,415,415,415,415,
415,415,415,415,415,0,0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,0,0,0,0,415,0,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};const int Cyc_lex_check[3481]=(
const int[3481]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,122,124,131,133,308,311,
41,41,45,45,41,417,45,- 1,- 1,- 1,- 1,- 1,120,233,414,0,202,10,12,40,10,12,40,41,202,
45,203,20,1,1,1,1,1,1,1,1,1,1,38,48,121,204,10,38,213,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,129,216,217,220,1,129,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,57,58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,61,62,7,3,3,3,3,3,7,7,63,64,67,68,69,7,30,30,30,30,30,30,30,
30,70,71,3,3,3,74,75,3,3,3,47,47,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,76,3,3,3,47,
0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,40,3,3,81,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,16,4,5,5,6,8,5,6,8,82,83,50,84,16,
50,85,27,86,17,27,50,17,4,87,94,5,95,5,6,8,9,18,114,115,116,5,117,118,119,141,
143,143,17,34,34,34,34,34,34,34,34,144,145,146,27,27,27,27,27,27,27,27,65,65,147,
148,65,72,72,77,77,72,149,77,150,151,152,153,66,66,4,154,66,156,157,65,6,8,5,16,
72,16,77,158,142,142,142,159,142,160,17,66,142,27,161,7,73,73,163,164,73,142,165,
166,167,11,11,11,11,11,11,11,11,11,11,103,27,110,103,73,110,3,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,111,168,130,111,11,
130,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,88,88,66,169,88,170,171,172,173,174,96,96,104,104,96,175,104,105,105,176,178,
105,179,88,180,13,13,80,80,13,73,80,16,96,181,104,182,183,184,185,105,186,187,
188,189,190,192,193,13,194,80,195,112,112,4,196,112,197,198,5,6,8,13,13,13,13,13,
13,13,13,13,13,13,13,17,112,199,200,201,205,206,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,207,208,209,210,13,211,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,80,212,
14,214,215,32,32,32,32,32,32,32,32,32,32,92,92,218,219,92,221,14,32,32,32,32,32,
32,125,125,125,125,125,125,125,125,14,222,92,224,103,225,110,226,227,99,99,228,
14,99,229,230,231,232,32,32,32,32,32,32,128,128,128,128,128,128,128,128,99,234,
235,236,238,111,237,130,15,15,237,239,15,101,101,240,241,101,242,237,14,237,243,
244,245,14,14,246,247,248,249,15,14,250,251,252,101,134,134,134,134,134,134,134,
134,253,254,15,15,15,15,15,15,15,15,15,15,237,255,256,257,92,258,13,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,259,99,260,261,
15,262,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,263,264,15,265,266,101,267,33,33,33,33,33,33,33,33,33,33,268,269,271,272,
276,277,278,33,33,33,33,33,33,137,137,137,137,137,137,137,137,279,280,281,282,
283,284,285,42,42,42,42,42,42,42,42,42,42,14,33,33,33,33,33,33,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,286,288,289,290,42,
291,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,44,44,44,44,44,44,44,44,44,44,294,295,296,15,297,298,299,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,301,302,303,304,44,320,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,
78,322,323,78,324,273,273,325,329,273,312,312,312,312,312,312,312,312,332,326,
334,337,78,335,335,323,139,139,273,344,139,140,140,324,327,140,273,326,78,78,78,
78,78,78,78,78,78,78,347,139,327,327,377,386,140,78,78,78,78,78,78,78,78,78,78,
78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,333,333,333,338,78,324,78,78,78,
78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,79,79,
139,388,79,287,287,140,389,287,391,318,139,318,318,392,393,140,355,356,365,366,
140,79,394,395,275,275,287,396,275,293,293,338,318,293,287,397,398,79,79,79,79,
79,79,79,79,79,79,275,355,356,365,366,293,369,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,399,406,416,370,79,369,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,275,370,
89,292,292,293,418,292,309,309,275,419,309,420,421,293,371,422,372,373,324,89,
423,424,316,316,292,- 1,316,- 1,- 1,309,- 1,309,292,- 1,- 1,89,89,89,89,89,89,89,89,89,
89,316,371,316,372,373,- 1,374,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,
89,89,89,89,89,89,89,89,89,- 1,- 1,- 1,- 1,89,374,89,89,89,89,89,89,89,89,89,89,89,
89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,90,90,300,300,90,- 1,300,305,305,
- 1,- 1,305,315,315,315,315,315,315,315,315,- 1,- 1,- 1,90,- 1,300,412,- 1,412,412,305,
- 1,- 1,300,- 1,- 1,- 1,- 1,305,90,90,90,90,90,90,90,90,90,90,412,- 1,- 1,- 1,- 1,- 1,- 1,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,- 1,- 1,
- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,97,97,- 1,328,97,328,328,328,328,328,328,328,328,328,328,- 1,- 1,- 1,- 1,354,
- 1,354,- 1,97,354,354,354,354,354,354,354,354,354,354,- 1,- 1,- 1,- 1,- 1,97,97,97,97,
97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,
97,97,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,97,97,97,
97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,98,98,- 1,- 1,98,357,357,357,
357,357,357,357,357,357,357,- 1,- 1,- 1,- 1,360,- 1,360,- 1,98,360,360,360,360,360,360,
360,360,360,360,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,- 1,- 1,-
1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
- 1,- 1,- 1,- 1,98,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,106,106,106,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,-
1,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,106,- 1,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,107,107,
107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,- 1,- 1,107,107,107,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,
107,107,- 1,- 1,- 1,- 1,107,- 1,107,107,107,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,108,108,108,108,108,108,108,
108,108,108,108,108,108,- 1,- 1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,109,- 1,- 1,
109,108,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,
109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,
- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,123,- 1,- 1,123,113,- 1,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,-
1,132,- 1,- 1,132,123,123,123,123,123,123,123,123,126,126,126,126,126,126,126,126,
126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,126,126,126,126,126,- 1,- 1,- 1,132,132,132,132,
132,132,132,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,109,- 1,390,- 1,- 1,126,126,126,126,
126,126,390,390,390,390,390,390,390,390,- 1,123,127,127,127,127,127,127,127,127,
127,127,132,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,135,135,135,135,135,135,
135,135,135,135,132,- 1,- 1,- 1,- 1,- 1,- 1,135,135,135,135,135,135,- 1,- 1,- 1,127,127,
127,127,127,127,310,- 1,- 1,310,- 1,- 1,136,136,136,136,136,136,136,136,136,136,- 1,
135,135,135,135,135,135,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,405,310,
310,310,310,310,310,310,310,405,405,405,405,405,405,405,405,- 1,136,136,136,136,
136,136,- 1,313,313,313,313,313,313,313,313,313,313,- 1,- 1,- 1,- 1,- 1,- 1,- 1,313,313,
313,313,313,313,- 1,- 1,310,361,361,361,361,361,361,361,361,361,361,- 1,- 1,314,314,
314,314,314,314,314,314,314,314,310,313,313,313,313,313,313,314,314,314,314,314,
314,- 1,- 1,330,- 1,330,330,330,330,330,330,330,330,330,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,330,314,314,314,314,314,314,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,330,
331,- 1,331,331,331,331,331,331,331,331,331,331,330,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,-
1,331,- 1,- 1,- 1,- 1,330,- 1,331,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,336,
336,336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,- 1,- 1,- 1,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
- 1,- 1,- 1,- 1,336,- 1,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,342,342,342,342,342,342,342,342,342,
342,- 1,- 1,- 1,- 1,- 1,- 1,- 1,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,- 1,- 1,- 1,- 1,342,- 1,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,352,352,352,352,352,352,352,352,352,352,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,352,352,- 1,- 1,- 1,- 1,353,352,353,353,353,353,353,353,353,353,353,353,358,358,
358,358,358,358,358,358,358,358,- 1,353,- 1,- 1,352,352,- 1,- 1,353,- 1,- 1,352,358,- 1,
- 1,- 1,- 1,353,358,- 1,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,353,- 1,- 1,-
1,- 1,- 1,- 1,353,359,359,- 1,358,- 1,- 1,- 1,359,353,358,- 1,- 1,362,362,362,362,362,362,
362,362,362,362,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,362,- 1,- 1,- 1,- 1,359,362,
363,- 1,363,363,363,363,363,363,363,363,363,363,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
363,- 1,362,- 1,- 1,- 1,- 1,363,362,- 1,- 1,- 1,- 1,- 1,- 1,364,363,364,364,364,364,364,364,
364,364,364,364,- 1,- 1,- 1,- 1,- 1,363,- 1,- 1,- 1,- 1,- 1,364,363,- 1,- 1,- 1,- 1,- 1,364,- 1,
- 1,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,378,378,378,378,378,378,378,378,378,378,- 1,- 1,- 1,
364,- 1,- 1,- 1,- 1,- 1,- 1,364,378,378,- 1,- 1,- 1,- 1,- 1,378,364,367,367,367,367,367,367,
367,367,367,367,- 1,- 1,- 1,- 1,- 1,- 1,- 1,367,367,367,367,367,367,378,378,- 1,- 1,- 1,- 1,
- 1,378,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,368,368,368,368,368,368,368,368,368,- 1,367,367,
367,367,367,367,368,368,368,368,368,368,- 1,- 1,- 1,387,- 1,368,- 1,- 1,387,- 1,- 1,- 1,-
1,- 1,368,- 1,- 1,387,387,387,387,387,387,387,387,- 1,368,368,368,368,368,368,387,- 1,
- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,387,- 1,- 1,- 1,- 1,387,387,- 1,- 1,- 1,387,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,387,- 1,
387,- 1,387,- 1,387,400,400,400,400,400,400,400,400,400,400,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
400,400,400,400,400,400,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,400,400,400,
400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,401,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,413,- 1,- 1,413,- 1,
- 1,- 1,413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,- 1,- 1,- 1,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,- 1,- 1,- 1,- 1,413,- 1,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,- 1,- 1,- 1,- 1,415,- 1,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_lex_engine(int start_state,
struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;state=start_state;
if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);lbuf->lex_last_action=
- 1;}else{state=(- state)- 1;}while(1){base=Cyc_lex_base[
_check_known_subscript_notnull(425,state)];if(base < 0)return(- base)- 1;backtrk=
Cyc_lex_backtrk[_check_known_subscript_notnull(425,state)];if(backtrk >= 0){lbuf->lex_last_pos=
lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}else{c=(int)*((char*)
_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}if(Cyc_lex_check[_check_known_subscript_notnull(3481,base + c)]
== state)state=Cyc_lex_trans[_check_known_subscript_notnull(3481,base + c)];else{
state=Cyc_lex_default[_check_known_subscript_notnull(425,state)];}if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;if(lbuf->lex_last_action == - 1){struct Cyc_Lexing_Error_struct
_tmp3D8;const char*_tmp3D7;struct Cyc_Lexing_Error_struct*_tmp3D6;(int)_throw((
void*)((_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6[0]=((_tmp3D8.tag=Cyc_Lexing_Error,((
_tmp3D8.f1=((_tmp3D7="empty token",_tag_dyneither(_tmp3D7,sizeof(char),12))),
_tmp3D8)))),_tmp3D6)))));}else{return lbuf->lex_last_action;}}else{if(c == 256)
lbuf->lex_eof_reached=0;}}}struct _tuple17*Cyc_line_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL0: Cyc_macroname(lexbuf);for(0;Cyc_current_args != 0;Cyc_current_args=((
struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){struct Cyc_Set_Set**
_tmp3D9;Cyc_current_targets=((_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9[0]=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(*((
struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_current_args))->hd),_tmp3D9))));}{struct
_tuple17*_tmp3DA;return(_tmp3DA=_cycalloc(sizeof(*_tmp3DA)),((_tmp3DA->f1=(
struct _dyneither_ptr*)_check_null(Cyc_current_source),((_tmp3DA->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets)),_tmp3DA)))));};case 1: _LL1: return Cyc_line(
lexbuf);case 2: _LL2: return 0;default: _LL3:(lexbuf->refill_buff)(lexbuf);return Cyc_line_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3E0;const char*_tmp3DF;struct
Cyc_Lexing_Error_struct*_tmp3DE;(int)_throw((void*)((_tmp3DE=_cycalloc(sizeof(*
_tmp3DE)),((_tmp3DE[0]=((_tmp3E0.tag=Cyc_Lexing_Error,((_tmp3E0.f1=((_tmp3DF="some action didn't return!",
_tag_dyneither(_tmp3DF,sizeof(char),27))),_tmp3E0)))),_tmp3DE)))));};}struct
_tuple17*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL5:{struct _dyneither_ptr*_tmp3E1;Cyc_current_source=((
_tmp3E1=_cycalloc(sizeof(*_tmp3E1)),((_tmp3E1[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp3E1))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp3E2;Cyc_current_targets=((_tmp3E2=_cycalloc(sizeof(*
_tmp3E2)),((_tmp3E2[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3E2))));}Cyc_token(
lexbuf);return 0;case 1: _LL6:{struct _dyneither_ptr*_tmp3E3;Cyc_current_source=((
_tmp3E3=_cycalloc(sizeof(*_tmp3E3)),((_tmp3E3[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3E3))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp3E4;Cyc_current_targets=((_tmp3E4=_cycalloc(sizeof(*
_tmp3E4)),((_tmp3E4[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3E4))));}Cyc_args(
lexbuf);return 0;case 2: _LL7:{struct _dyneither_ptr*_tmp3E5;Cyc_current_source=((
_tmp3E5=_cycalloc(sizeof(*_tmp3E5)),((_tmp3E5[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf),_tmp3E5))));}Cyc_current_args=0;{struct Cyc_Set_Set**_tmp3E6;Cyc_current_targets=((
_tmp3E6=_cycalloc(sizeof(*_tmp3E6)),((_tmp3E6[0]=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp3E6))));}Cyc_token(lexbuf);return 0;default: _LL8:(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3EC;
const char*_tmp3EB;struct Cyc_Lexing_Error_struct*_tmp3EA;(int)_throw((void*)((
_tmp3EA=_cycalloc(sizeof(*_tmp3EA)),((_tmp3EA[0]=((_tmp3EC.tag=Cyc_Lexing_Error,((
_tmp3EC.f1=((_tmp3EB="some action didn't return!",_tag_dyneither(_tmp3EB,sizeof(
char),27))),_tmp3EC)))),_tmp3EA)))));};}int Cyc_macroname(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_macroname_rec(lexbuf,1);}int Cyc_args_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLA: {struct _dyneither_ptr*_tmp3ED;struct _dyneither_ptr*_tmp15=(_tmp3ED=
_cycalloc(sizeof(*_tmp3ED)),((_tmp3ED[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp3ED)));{struct Cyc_List_List*
_tmp3EE;Cyc_current_args=((_tmp3EE=_cycalloc(sizeof(*_tmp3EE)),((_tmp3EE->hd=
_tmp15,((_tmp3EE->tl=Cyc_current_args,_tmp3EE))))));}return Cyc_args(lexbuf);}
case 1: _LLB: {struct _dyneither_ptr*_tmp3EF;struct _dyneither_ptr*_tmp18=(_tmp3EF=
_cycalloc(sizeof(*_tmp3EF)),((_tmp3EF[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3EF)));{struct Cyc_List_List*
_tmp3F0;Cyc_current_args=((_tmp3F0=_cycalloc(sizeof(*_tmp3F0)),((_tmp3F0->hd=
_tmp18,((_tmp3F0->tl=Cyc_current_args,_tmp3F0))))));}return Cyc_args(lexbuf);}
case 2: _LLC: {struct _dyneither_ptr*_tmp3F1;struct _dyneither_ptr*_tmp1B=(_tmp3F1=
_cycalloc(sizeof(*_tmp3F1)),((_tmp3F1[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3F1)));{struct Cyc_List_List*
_tmp3F2;Cyc_current_args=((_tmp3F2=_cycalloc(sizeof(*_tmp3F2)),((_tmp3F2->hd=
_tmp1B,((_tmp3F2->tl=Cyc_current_args,_tmp3F2))))));}return Cyc_token(lexbuf);}
default: _LLD:(lexbuf->refill_buff)(lexbuf);return Cyc_args_rec(lexbuf,lexstate);}{
struct Cyc_Lexing_Error_struct _tmp3F8;const char*_tmp3F7;struct Cyc_Lexing_Error_struct*
_tmp3F6;(int)_throw((void*)((_tmp3F6=_cycalloc(sizeof(*_tmp3F6)),((_tmp3F6[0]=((
_tmp3F8.tag=Cyc_Lexing_Error,((_tmp3F8.f1=((_tmp3F7="some action didn't return!",
_tag_dyneither(_tmp3F7,sizeof(char),27))),_tmp3F8)))),_tmp3F6)))));};}int Cyc_args(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}int Cyc_token_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLF:{struct _dyneither_ptr*_tmp3F9;Cyc_add_target(((
_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((_tmp3F9[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf),_tmp3F9)))));}return Cyc_token(lexbuf);case 1: _LL10: return 0;case 2: _LL11:
return Cyc_token(lexbuf);case 3: _LL12: Cyc_string(lexbuf);return Cyc_token(lexbuf);
case 4: _LL13: return Cyc_token(lexbuf);case 5: _LL14: return Cyc_token(lexbuf);case 6:
_LL15: return Cyc_token(lexbuf);case 7: _LL16: return Cyc_token(lexbuf);case 8: _LL17:
return Cyc_token(lexbuf);case 9: _LL18: return Cyc_token(lexbuf);case 10: _LL19: return
Cyc_token(lexbuf);case 11: _LL1A: return Cyc_token(lexbuf);case 12: _LL1B: return Cyc_token(
lexbuf);case 13: _LL1C: return Cyc_token(lexbuf);case 14: _LL1D: return Cyc_token(lexbuf);
case 15: _LL1E: return Cyc_token(lexbuf);case 16: _LL1F: return Cyc_token(lexbuf);case 17:
_LL20: return Cyc_token(lexbuf);case 18: _LL21: return Cyc_token(lexbuf);case 19: _LL22:
return Cyc_token(lexbuf);case 20: _LL23: return Cyc_token(lexbuf);case 21: _LL24: return
Cyc_token(lexbuf);case 22: _LL25: return Cyc_token(lexbuf);case 23: _LL26: return Cyc_token(
lexbuf);case 24: _LL27: return Cyc_token(lexbuf);case 25: _LL28: return Cyc_token(lexbuf);
case 26: _LL29: return Cyc_token(lexbuf);case 27: _LL2A: return Cyc_token(lexbuf);case 28:
_LL2B: return Cyc_token(lexbuf);case 29: _LL2C: return Cyc_token(lexbuf);case 30: _LL2D:
return Cyc_token(lexbuf);case 31: _LL2E: return Cyc_token(lexbuf);case 32: _LL2F: return
Cyc_token(lexbuf);case 33: _LL30: return Cyc_token(lexbuf);case 34: _LL31: return Cyc_token(
lexbuf);case 35: _LL32: return Cyc_token(lexbuf);case 36: _LL33: return Cyc_token(lexbuf);
case 37: _LL34: return Cyc_token(lexbuf);case 38: _LL35: return Cyc_token(lexbuf);case 39:
_LL36: return Cyc_token(lexbuf);case 40: _LL37: return Cyc_token(lexbuf);case 41: _LL38:
return Cyc_token(lexbuf);case 42: _LL39: return Cyc_token(lexbuf);case 43: _LL3A: return
Cyc_token(lexbuf);case 44: _LL3B: return Cyc_token(lexbuf);case 45: _LL3C: return Cyc_token(
lexbuf);case 46: _LL3D: return Cyc_token(lexbuf);case 47: _LL3E: return Cyc_token(lexbuf);
case 48: _LL3F: return Cyc_token(lexbuf);default: _LL40:(lexbuf->refill_buff)(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3FF;
const char*_tmp3FE;struct Cyc_Lexing_Error_struct*_tmp3FD;(int)_throw((void*)((
_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((_tmp3FD[0]=((_tmp3FF.tag=Cyc_Lexing_Error,((
_tmp3FF.f1=((_tmp3FE="some action didn't return!",_tag_dyneither(_tmp3FE,sizeof(
char),27))),_tmp3FF)))),_tmp3FD)))));};}int Cyc_token(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_token_rec(lexbuf,3);}int Cyc_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL42: return Cyc_string(lexbuf);case 1: _LL43: return 0;case 2: _LL44: return Cyc_string(
lexbuf);case 3: _LL45: return Cyc_string(lexbuf);case 4: _LL46: return Cyc_string(lexbuf);
case 5: _LL47: return Cyc_string(lexbuf);case 6: _LL48: return Cyc_string(lexbuf);case 7:
_LL49: return 0;case 8: _LL4A: return 0;case 9: _LL4B: return Cyc_string(lexbuf);default:
_LL4C:(lexbuf->refill_buff)(lexbuf);return Cyc_string_rec(lexbuf,lexstate);}{
struct Cyc_Lexing_Error_struct _tmp405;const char*_tmp404;struct Cyc_Lexing_Error_struct*
_tmp403;(int)_throw((void*)((_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403[0]=((
_tmp405.tag=Cyc_Lexing_Error,((_tmp405.f1=((_tmp404="some action didn't return!",
_tag_dyneither(_tmp404,sizeof(char),27))),_tmp405)))),_tmp403)))));};}int Cyc_string(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}int Cyc_slurp_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL4E: return 0;case 1: _LL4F: Cyc_fputc((int)'"',(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LL50: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp408;void*_tmp407;(
_tmp407=0,Cyc_log(((_tmp408="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp408,sizeof(char),44))),_tag_dyneither(_tmp407,sizeof(void*),0)));}
return 1;case 3: _LL51: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp40B;void*_tmp40A;(
_tmp40A=0,Cyc_log(((_tmp40B="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp40B,sizeof(char),44))),_tag_dyneither(_tmp40A,sizeof(void*),0)));}
return 1;case 4: _LL52: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp40E;void*_tmp40D;(
_tmp40D=0,Cyc_log(((_tmp40E="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp40E,sizeof(char),44))),_tag_dyneither(_tmp40D,sizeof(void*),0)));}
return 1;case 5: _LL53: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp411;void*_tmp410;(
_tmp410=0,Cyc_log(((_tmp411="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp411,sizeof(char),44))),_tag_dyneither(_tmp410,sizeof(void*),0)));}
return 1;case 6: _LL54: Cyc_fputs((const char*)"__region",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));{const char*_tmp414;void*_tmp413;(_tmp413=0,Cyc_log(((
_tmp414="Warning: use of region sidestepped\n",_tag_dyneither(_tmp414,sizeof(
char),36))),_tag_dyneither(_tmp413,sizeof(void*),0)));}return 1;case 7: _LL55:{
const char*_tmp417;void*_tmp416;(_tmp416=0,Cyc_log(((_tmp417="Warning: use of __extension__ deleted\n",
_tag_dyneither(_tmp417,sizeof(char),39))),_tag_dyneither(_tmp416,sizeof(void*),0)));}
return 1;case 8: _LL56:{const char*_tmp41A;void*_tmp419;(_tmp419=0,Cyc_log(((_tmp41A="Warning: use of mode HI deleted\n",
_tag_dyneither(_tmp41A,sizeof(char),33))),_tag_dyneither(_tmp419,sizeof(void*),0)));}
return 1;case 9: _LL57:{const char*_tmp41D;void*_tmp41C;(_tmp41C=0,Cyc_log(((_tmp41D="Warning: use of mode SI deleted\n",
_tag_dyneither(_tmp41D,sizeof(char),33))),_tag_dyneither(_tmp41C,sizeof(void*),0)));}
return 1;case 10: _LL58:{const char*_tmp420;void*_tmp41F;(_tmp41F=0,Cyc_log(((
_tmp420="Warning: use of mode QI deleted\n",_tag_dyneither(_tmp420,sizeof(char),
33))),_tag_dyneither(_tmp41F,sizeof(void*),0)));}return 1;case 11: _LL59:{const char*
_tmp423;void*_tmp422;(_tmp422=0,Cyc_log(((_tmp423="Warning: use of mode DI deleted\n",
_tag_dyneither(_tmp423,sizeof(char),33))),_tag_dyneither(_tmp422,sizeof(void*),0)));}
return 1;case 12: _LL5A:{const char*_tmp426;void*_tmp425;(_tmp425=0,Cyc_log(((
_tmp426="Warning: use of mode DI deleted\n",_tag_dyneither(_tmp426,sizeof(char),
33))),_tag_dyneither(_tmp425,sizeof(void*),0)));}return 1;case 13: _LL5B:{const char*
_tmp429;void*_tmp428;(_tmp428=0,Cyc_log(((_tmp429="Warning: use of mode word deleted\n",
_tag_dyneither(_tmp429,sizeof(char),35))),_tag_dyneither(_tmp428,sizeof(void*),0)));}
return 1;case 14: _LL5C: Cyc_fputs((const char*)"inline",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 15: _LL5D: Cyc_fputs((const char*)"inline",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16: _LL5E: Cyc_fputs((
const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 17: _LL5F: Cyc_fputs((const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 18: _LL60: Cyc_fputs((const char*)"int",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 19: _LL61: return 1;case 20: _LL62: Cyc_parens_to_match=
1;while(Cyc_asmtok(lexbuf)){;}Cyc_fputs((const char*)"0",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));{const char*_tmp42C;void*_tmp42B;(_tmp42B=0,Cyc_log(((
_tmp42C="Warning: replacing use of __asm__ with 0\n",_tag_dyneither(_tmp42C,
sizeof(char),42))),_tag_dyneither(_tmp42B,sizeof(void*),0)));}return 1;case 21:
_LL63: Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;default: _LL64:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp432;
const char*_tmp431;struct Cyc_Lexing_Error_struct*_tmp430;(int)_throw((void*)((
_tmp430=_cycalloc(sizeof(*_tmp430)),((_tmp430[0]=((_tmp432.tag=Cyc_Lexing_Error,((
_tmp432.f1=((_tmp431="some action didn't return!",_tag_dyneither(_tmp431,sizeof(
char),27))),_tmp432)))),_tmp430)))));};}int Cyc_slurp(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_slurp_rec(lexbuf,5);}int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL66: return 0;case 1: _LL67: Cyc_fputc((int)'"',(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 0;case 2: _LL68:{const char*_tmp435;void*_tmp434;(
_tmp434=0,Cyc_log(((_tmp435="Warning: unclosed string\n",_tag_dyneither(_tmp435,
sizeof(char),26))),_tag_dyneither(_tmp434,sizeof(void*),0)));}{const char*_tmp439;
void*_tmp438[1];struct Cyc_String_pa_struct _tmp437;(_tmp437.tag=0,((_tmp437.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp438[
0]=& _tmp437,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp439="%s",_tag_dyneither(_tmp439,sizeof(char),3))),_tag_dyneither(_tmp438,
sizeof(void*),1)))))));}return 1;case 3: _LL69:{const char*_tmp43D;void*_tmp43C[1];
struct Cyc_String_pa_struct _tmp43B;(_tmp43B.tag=0,((_tmp43B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp43C[0]=&
_tmp43B,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp43D="%s",
_tag_dyneither(_tmp43D,sizeof(char),3))),_tag_dyneither(_tmp43C,sizeof(void*),1)))))));}
return 1;case 4: _LL6A:{const char*_tmp441;void*_tmp440[1];struct Cyc_String_pa_struct
_tmp43F;(_tmp43F.tag=0,((_tmp43F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf)),((_tmp440[0]=& _tmp43F,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),((_tmp441="%s",_tag_dyneither(_tmp441,sizeof(char),3))),
_tag_dyneither(_tmp440,sizeof(void*),1)))))));}return 1;case 5: _LL6B:{const char*
_tmp445;void*_tmp444[1];struct Cyc_String_pa_struct _tmp443;(_tmp443.tag=0,((
_tmp443.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((
_tmp444[0]=& _tmp443,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp445="%s",_tag_dyneither(_tmp445,sizeof(char),3))),_tag_dyneither(_tmp444,
sizeof(void*),1)))))));}return 1;case 6: _LL6C:{const char*_tmp449;void*_tmp448[1];
struct Cyc_String_pa_struct _tmp447;(_tmp447.tag=0,((_tmp447.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp448[0]=&
_tmp447,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp449="%s",
_tag_dyneither(_tmp449,sizeof(char),3))),_tag_dyneither(_tmp448,sizeof(void*),1)))))));}
return 1;case 7: _LL6D:{const char*_tmp44D;void*_tmp44C[1];struct Cyc_String_pa_struct
_tmp44B;(_tmp44B.tag=0,((_tmp44B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf)),((_tmp44C[0]=& _tmp44B,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),((_tmp44D="%s",_tag_dyneither(_tmp44D,sizeof(char),3))),
_tag_dyneither(_tmp44C,sizeof(void*),1)))))));}return 1;case 8: _LL6E:{const char*
_tmp451;void*_tmp450[1];struct Cyc_String_pa_struct _tmp44F;(_tmp44F.tag=0,((
_tmp44F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((
_tmp450[0]=& _tmp44F,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp451="%s",_tag_dyneither(_tmp451,sizeof(char),3))),_tag_dyneither(_tmp450,
sizeof(void*),1)))))));}return 1;default: _LL6F:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp457;const char*_tmp456;struct Cyc_Lexing_Error_struct*_tmp455;(int)_throw((
void*)((_tmp455=_cycalloc(sizeof(*_tmp455)),((_tmp455[0]=((_tmp457.tag=Cyc_Lexing_Error,((
_tmp457.f1=((_tmp456="some action didn't return!",_tag_dyneither(_tmp456,sizeof(
char),27))),_tmp457)))),_tmp455)))));};}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL71: return 0;case 1: _LL72: if(Cyc_parens_to_match == 1)return 0;-- Cyc_parens_to_match;
return 1;case 2: _LL73: ++ Cyc_parens_to_match;return 1;case 3: _LL74: while(Cyc_asm_string(
lexbuf)){;}return 1;case 4: _LL75: while(Cyc_asm_comment(lexbuf)){;}return 1;case 5:
_LL76: return 1;case 6: _LL77: return 1;default: _LL78:(lexbuf->refill_buff)(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp45D;
const char*_tmp45C;struct Cyc_Lexing_Error_struct*_tmp45B;(int)_throw((void*)((
_tmp45B=_cycalloc(sizeof(*_tmp45B)),((_tmp45B[0]=((_tmp45D.tag=Cyc_Lexing_Error,((
_tmp45D.f1=((_tmp45C="some action didn't return!",_tag_dyneither(_tmp45C,sizeof(
char),27))),_tmp45D)))),_tmp45B)))));};}int Cyc_asmtok(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asmtok_rec(lexbuf,7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL7A:{const char*_tmp460;void*_tmp45F;(_tmp45F=0,Cyc_log(((_tmp460="Warning: unclosed string\n",
_tag_dyneither(_tmp460,sizeof(char),26))),_tag_dyneither(_tmp45F,sizeof(void*),0)));}
return 0;case 1: _LL7B: return 0;case 2: _LL7C:{const char*_tmp463;void*_tmp462;(_tmp462=
0,Cyc_log(((_tmp463="Warning: unclosed string\n",_tag_dyneither(_tmp463,sizeof(
char),26))),_tag_dyneither(_tmp462,sizeof(void*),0)));}return 1;case 3: _LL7D:
return 1;case 4: _LL7E: return 1;case 5: _LL7F: return 1;case 6: _LL80: return 1;case 7: _LL81:
return 1;case 8: _LL82: return 1;default: _LL83:(lexbuf->refill_buff)(lexbuf);return
Cyc_asm_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp469;const
char*_tmp468;struct Cyc_Lexing_Error_struct*_tmp467;(int)_throw((void*)((_tmp467=
_cycalloc(sizeof(*_tmp467)),((_tmp467[0]=((_tmp469.tag=Cyc_Lexing_Error,((
_tmp469.f1=((_tmp468="some action didn't return!",_tag_dyneither(_tmp468,sizeof(
char),27))),_tmp469)))),_tmp467)))));};}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_string_rec(lexbuf,8);}int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL85:{const char*_tmp46C;void*_tmp46B;(_tmp46B=0,Cyc_log(((_tmp46C="Warning: unclosed comment\n",
_tag_dyneither(_tmp46C,sizeof(char),27))),_tag_dyneither(_tmp46B,sizeof(void*),0)));}
return 0;case 1: _LL86: return 0;case 2: _LL87: return 1;default: _LL88:(lexbuf->refill_buff)(
lexbuf);return Cyc_asm_comment_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp472;const char*_tmp471;struct Cyc_Lexing_Error_struct*_tmp470;(int)_throw((
void*)((_tmp470=_cycalloc(sizeof(*_tmp470)),((_tmp470[0]=((_tmp472.tag=Cyc_Lexing_Error,((
_tmp472.f1=((_tmp471="some action didn't return!",_tag_dyneither(_tmp471,sizeof(
char),27))),_tmp472)))),_tmp470)))));};}int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}struct _tuple18*Cyc_suck_line_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL8A:{const char*_tmp473;Cyc_current_line=((
_tmp473="#define ",_tag_dyneither(_tmp473,sizeof(char),9)));}Cyc_suck_macroname(
lexbuf);{struct _tuple18*_tmp474;return(_tmp474=_cycalloc(sizeof(*_tmp474)),((
_tmp474->f1=Cyc_current_line,((_tmp474->f2=(struct _dyneither_ptr*)_check_null(
Cyc_current_source),_tmp474)))));};case 1: _LL8B: return Cyc_suck_line(lexbuf);case 2:
_LL8C: return 0;default: _LL8D:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_line_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp47A;const char*_tmp479;struct
Cyc_Lexing_Error_struct*_tmp478;(int)_throw((void*)((_tmp478=_cycalloc(sizeof(*
_tmp478)),((_tmp478[0]=((_tmp47A.tag=Cyc_Lexing_Error,((_tmp47A.f1=((_tmp479="some action didn't return!",
_tag_dyneither(_tmp479,sizeof(char),27))),_tmp47A)))),_tmp478)))));};}struct
_tuple18*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(
lexbuf,10);}int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8F:{struct
_dyneither_ptr*_tmp47B;Cyc_current_source=((_tmp47B=_cycalloc(sizeof(*_tmp47B)),((
_tmp47B[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp47B))));}Cyc_current_line=(
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);default: _LL90:(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp481;const char*_tmp480;struct Cyc_Lexing_Error_struct*_tmp47F;(int)_throw((
void*)((_tmp47F=_cycalloc(sizeof(*_tmp47F)),((_tmp47F[0]=((_tmp481.tag=Cyc_Lexing_Error,((
_tmp481.f1=((_tmp480="some action didn't return!",_tag_dyneither(_tmp480,sizeof(
char),27))),_tmp481)))),_tmp47F)))));};}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}int Cyc_suck_restofline_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL92: Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((
struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 0;default: _LL93:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_restofline_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp487;const char*_tmp486;struct
Cyc_Lexing_Error_struct*_tmp485;(int)_throw((void*)((_tmp485=_cycalloc(sizeof(*
_tmp485)),((_tmp485[0]=((_tmp487.tag=Cyc_Lexing_Error,((_tmp487.f1=((_tmp486="some action didn't return!",
_tag_dyneither(_tmp486,sizeof(char),27))),_tmp487)))),_tmp485)))));};}int Cyc_suck_restofline(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}struct
_tuple20*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL95: return Cyc_spec(lexbuf);case 1: _LL96:
Cyc_current_headerfile=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(
lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=0;Cyc_current_cstubs=
0;Cyc_current_cycstubs=0;Cyc_current_hstubs=0;while(Cyc_commands(lexbuf)){;}Cyc_current_hstubs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_hstubs);
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cstubs);Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_current_cycstubs);{struct _tuple20*_tmp488;return(
_tmp488=_cycalloc(sizeof(*_tmp488)),((_tmp488->f1=Cyc_current_headerfile,((
_tmp488->f2=Cyc_current_symbols,((_tmp488->f3=Cyc_current_omit_symbols,((_tmp488->f4=
Cyc_current_hstubs,((_tmp488->f5=Cyc_current_cstubs,((_tmp488->f6=Cyc_current_cycstubs,
_tmp488)))))))))))));};case 2: _LL97: return Cyc_spec(lexbuf);case 3: _LL98: return 0;
case 4: _LL99:{const char*_tmp48C;void*_tmp48B[1];struct Cyc_Int_pa_struct _tmp48A;(
_tmp48A.tag=1,((_tmp48A.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0)),((
_tmp48B[0]=& _tmp48A,Cyc_fprintf(Cyc_stderr,((_tmp48C="Error in .cys file: expected header file name, found '%c' instead\n",
_tag_dyneither(_tmp48C,sizeof(char),67))),_tag_dyneither(_tmp48B,sizeof(void*),1)))))));}
return 0;default: _LL9A:(lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp492;const char*_tmp491;struct Cyc_Lexing_Error_struct*
_tmp490;(int)_throw((void*)((_tmp490=_cycalloc(sizeof(*_tmp490)),((_tmp490[0]=((
_tmp492.tag=Cyc_Lexing_Error,((_tmp492.f1=((_tmp491="some action didn't return!",
_tag_dyneither(_tmp491,sizeof(char),27))),_tmp492)))),_tmp490)))));};}struct
_tuple20*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL9C: return 0;case 1: _LL9D: return 0;case 2:
_LL9E: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(lexbuf)){;}Cyc_current_symbols=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_snarfed_symbols,Cyc_current_symbols);return 1;case 3: _LL9F: Cyc_snarfed_symbols=
0;while(Cyc_snarfsymbols(lexbuf)){;}Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,
Cyc_current_omit_symbols);return 1;case 4: _LLA0: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_tuple19*_tmp493;struct _tuple19*x=(_tmp493=_cycalloc(sizeof(*_tmp493)),((_tmp493->f1=(
struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp493->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp493)))));{
struct Cyc_List_List*_tmp494;Cyc_current_hstubs=((_tmp494=_cycalloc(sizeof(*
_tmp494)),((_tmp494->hd=x,((_tmp494->tl=Cyc_current_hstubs,_tmp494))))));}return
1;};case 5: _LLA1: {struct _dyneither_ptr _tmp83=Cyc_Lexing_lexeme(lexbuf);{const
char*_tmp495;_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),(int)Cyc_strlen(((
_tmp495="hstub",_tag_dyneither(_tmp495,sizeof(char),6)))));}while(isspace((int)*((
char*)_check_dyneither_subscript(_tmp83,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),1);}{struct _dyneither_ptr t=
_tmp83;while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp85=Cyc_substring((
struct _dyneither_ptr)_tmp83,0,(unsigned long)((t.curr - _tmp83.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp496;struct _tuple19*x=(_tmp496=
_cycalloc(sizeof(*_tmp496)),((_tmp496->f1=(struct _dyneither_ptr)_tmp85,((_tmp496->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp496)))));{struct Cyc_List_List*_tmp497;Cyc_current_hstubs=((_tmp497=_cycalloc(
sizeof(*_tmp497)),((_tmp497->hd=x,((_tmp497->tl=Cyc_current_hstubs,_tmp497))))));}
return 1;};};};}case 6: _LLA2: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp498;struct
_tuple19*x=(_tmp498=_cycalloc(sizeof(*_tmp498)),((_tmp498->f1=(struct
_dyneither_ptr)_tag_dyneither(0,0,0),((_tmp498->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp498)))));{struct Cyc_List_List*
_tmp499;Cyc_current_cstubs=((_tmp499=_cycalloc(sizeof(*_tmp499)),((_tmp499->hd=x,((
_tmp499->tl=Cyc_current_cstubs,_tmp499))))));}return 1;};case 7: _LLA3: {struct
_dyneither_ptr _tmp8A=Cyc_Lexing_lexeme(lexbuf);{const char*_tmp49A;
_dyneither_ptr_inplace_plus(& _tmp8A,sizeof(char),(int)Cyc_strlen(((_tmp49A="cstub",
_tag_dyneither(_tmp49A,sizeof(char),6)))));}while(isspace((int)*((char*)
_check_dyneither_subscript(_tmp8A,sizeof(char),0)))){_dyneither_ptr_inplace_plus(&
_tmp8A,sizeof(char),1);}{struct _dyneither_ptr t=_tmp8A;while(!isspace((int)*((
char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp8C=Cyc_substring((
struct _dyneither_ptr)_tmp8A,0,(unsigned long)((t.curr - _tmp8A.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp49B;struct _tuple19*x=(_tmp49B=
_cycalloc(sizeof(*_tmp49B)),((_tmp49B->f1=(struct _dyneither_ptr)_tmp8C,((_tmp49B->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp49B)))));{struct Cyc_List_List*_tmp49C;Cyc_current_cstubs=((_tmp49C=_cycalloc(
sizeof(*_tmp49C)),((_tmp49C->hd=x,((_tmp49C->tl=Cyc_current_cstubs,_tmp49C))))));}
return 1;};};};}case 8: _LLA4: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp49D;struct
_tuple19*x=(_tmp49D=_cycalloc(sizeof(*_tmp49D)),((_tmp49D->f1=(struct
_dyneither_ptr)_tag_dyneither(0,0,0),((_tmp49D->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp49D)))));{struct Cyc_List_List*
_tmp49E;Cyc_current_cycstubs=((_tmp49E=_cycalloc(sizeof(*_tmp49E)),((_tmp49E->hd=
x,((_tmp49E->tl=Cyc_current_cycstubs,_tmp49E))))));}return 1;};case 9: _LLA5: {
struct _dyneither_ptr _tmp91=Cyc_Lexing_lexeme(lexbuf);{const char*_tmp49F;
_dyneither_ptr_inplace_plus(& _tmp91,sizeof(char),(int)Cyc_strlen(((_tmp49F="cycstub",
_tag_dyneither(_tmp49F,sizeof(char),8)))));}while(isspace((int)*((char*)
_check_dyneither_subscript(_tmp91,sizeof(char),0)))){_dyneither_ptr_inplace_plus(&
_tmp91,sizeof(char),1);}{struct _dyneither_ptr t=_tmp91;while(!isspace((int)*((
char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp93=Cyc_substring((
struct _dyneither_ptr)_tmp91,0,(unsigned long)((t.curr - _tmp91.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp4A0;struct _tuple19*x=(_tmp4A0=
_cycalloc(sizeof(*_tmp4A0)),((_tmp4A0->f1=(struct _dyneither_ptr)_tmp93,((_tmp4A0->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp4A0)))));{struct Cyc_List_List*_tmp4A1;Cyc_current_cycstubs=((_tmp4A1=
_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1->hd=x,((_tmp4A1->tl=Cyc_current_cycstubs,
_tmp4A1))))));}return 1;};};};}case 10: _LLA6: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_dyneither_ptr*_tmp4A2;struct _dyneither_ptr*x=(_tmp4A2=_cycalloc(sizeof(*_tmp4A2)),((
_tmp4A2[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf)),_tmp4A2)));{struct Cyc_List_List*_tmp4A3;Cyc_current_cpp=((
_tmp4A3=_cycalloc(sizeof(*_tmp4A3)),((_tmp4A3->hd=x,((_tmp4A3->tl=Cyc_current_cpp,
_tmp4A3))))));}return 1;};case 11: _LLA7: return 1;case 12: _LLA8: return 1;case 13: _LLA9:{
const char*_tmp4A7;void*_tmp4A6[1];struct Cyc_Int_pa_struct _tmp4A5;(_tmp4A5.tag=1,((
_tmp4A5.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp4A6[0]=&
_tmp4A5,Cyc_fprintf(Cyc_stderr,((_tmp4A7="Error in .cys file: expected command, found '%c' instead\n",
_tag_dyneither(_tmp4A7,sizeof(char),58))),_tag_dyneither(_tmp4A6,sizeof(void*),1)))))));}
return 0;default: _LLAA:(lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp4AD;const char*_tmp4AC;struct
Cyc_Lexing_Error_struct*_tmp4AB;(int)_throw((void*)((_tmp4AB=_cycalloc(sizeof(*
_tmp4AB)),((_tmp4AB[0]=((_tmp4AD.tag=Cyc_Lexing_Error,((_tmp4AD.f1=((_tmp4AC="some action didn't return!",
_tag_dyneither(_tmp4AC,sizeof(char),27))),_tmp4AD)))),_tmp4AB)))));};}int Cyc_commands(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}int Cyc_snarfsymbols_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLAC:{struct _dyneither_ptr*_tmp4B0;struct Cyc_List_List*
_tmp4AF;Cyc_snarfed_symbols=((_tmp4AF=_cycalloc(sizeof(*_tmp4AF)),((_tmp4AF->hd=((
_tmp4B0=_cycalloc(sizeof(*_tmp4B0)),((_tmp4B0[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf),_tmp4B0)))),((_tmp4AF->tl=Cyc_snarfed_symbols,_tmp4AF))))));}return 1;
case 1: _LLAD: return 1;case 2: _LLAE: return 0;case 3: _LLAF:{const char*_tmp4B3;void*
_tmp4B2;(_tmp4B2=0,Cyc_fprintf(Cyc_stderr,((_tmp4B3="Error in .cys file: unexpected end-of-file\n",
_tag_dyneither(_tmp4B3,sizeof(char),44))),_tag_dyneither(_tmp4B2,sizeof(void*),0)));}
return 0;case 4: _LLB0:{const char*_tmp4B7;void*_tmp4B6[1];struct Cyc_Int_pa_struct
_tmp4B5;(_tmp4B5.tag=1,((_tmp4B5.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)),((_tmp4B6[0]=& _tmp4B5,Cyc_fprintf(Cyc_stderr,((_tmp4B7="Error in .cys file: expected symbol, found '%c' instead\n",
_tag_dyneither(_tmp4B7,sizeof(char),57))),_tag_dyneither(_tmp4B6,sizeof(void*),1)))))));}
return 0;default: _LLB1:(lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp4BD;const char*_tmp4BC;struct
Cyc_Lexing_Error_struct*_tmp4BB;(int)_throw((void*)((_tmp4BB=_cycalloc(sizeof(*
_tmp4BB)),((_tmp4BB[0]=((_tmp4BD.tag=Cyc_Lexing_Error,((_tmp4BD.f1=((_tmp4BC="some action didn't return!",
_tag_dyneither(_tmp4BC,sizeof(char),27))),_tmp4BD)))),_tmp4BB)))));};}int Cyc_snarfsymbols(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}int Cyc_block_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLB3:{const char*_tmp4C0;void*_tmp4BF;(_tmp4BF=0,
Cyc_log(((_tmp4C0="Warning: unclosed brace\n",_tag_dyneither(_tmp4C0,sizeof(char),
25))),_tag_dyneither(_tmp4BF,sizeof(void*),0)));}return 0;case 1: _LLB4: if(Cyc_braces_to_match
== 1)return 0;-- Cyc_braces_to_match;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'}');return 1;case 2: _LLB5: ++ Cyc_braces_to_match;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return 1;case 3: _LLB6: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LLB7:{const char*_tmp4C1;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),((_tmp4C1="/*",_tag_dyneither(_tmp4C1,sizeof(char),3))));}
while(Cyc_block_comment(lexbuf)){;}return 1;case 5: _LLB8: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 6: _LLB9: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));return 1;default: _LLBA:(
lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp4C7;const char*_tmp4C6;struct Cyc_Lexing_Error_struct*_tmp4C5;(int)_throw((
void*)((_tmp4C5=_cycalloc(sizeof(*_tmp4C5)),((_tmp4C5[0]=((_tmp4C7.tag=Cyc_Lexing_Error,((
_tmp4C7.f1=((_tmp4C6="some action didn't return!",_tag_dyneither(_tmp4C6,sizeof(
char),27))),_tmp4C7)))),_tmp4C5)))));};}int Cyc_block(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_block_rec(lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLBC:{const char*_tmp4CA;void*_tmp4C9;(_tmp4C9=0,Cyc_log(((_tmp4CA="Warning: unclosed string\n",
_tag_dyneither(_tmp4CA,sizeof(char),26))),_tag_dyneither(_tmp4C9,sizeof(void*),0)));}
return 0;case 1: _LLBD: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
return 0;case 2: _LLBE:{const char*_tmp4CD;void*_tmp4CC;(_tmp4CC=0,Cyc_log(((_tmp4CD="Warning: unclosed string\n",
_tag_dyneither(_tmp4CD,sizeof(char),26))),_tag_dyneither(_tmp4CC,sizeof(void*),0)));}
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 3: _LLBF: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 4: _LLC0: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return
1;case 5: _LLC1: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLC2: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 7: _LLC3: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return
1;case 8: _LLC4: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLC5:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp4D3;const char*_tmp4D2;struct Cyc_Lexing_Error_struct*_tmp4D1;(int)_throw((
void*)((_tmp4D1=_cycalloc(sizeof(*_tmp4D1)),((_tmp4D1[0]=((_tmp4D3.tag=Cyc_Lexing_Error,((
_tmp4D3.f1=((_tmp4D2="some action didn't return!",_tag_dyneither(_tmp4D2,sizeof(
char),27))),_tmp4D3)))),_tmp4D1)))));};}int Cyc_block_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_block_string_rec(lexbuf,17);}int Cyc_block_comment_rec(struct
Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLC7:{const char*_tmp4D6;void*_tmp4D5;(_tmp4D5=0,Cyc_log(((
_tmp4D6="Warning: unclosed comment\n",_tag_dyneither(_tmp4D6,sizeof(char),27))),
_tag_dyneither(_tmp4D5,sizeof(void*),0)));}return 0;case 1: _LLC8:{const char*
_tmp4D7;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((
_tmp4D7="*/",_tag_dyneither(_tmp4D7,sizeof(char),3))));}return 0;case 2: _LLC9: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;default: _LLCA:(lexbuf->refill_buff)(lexbuf);return Cyc_block_comment_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp4DD;const char*_tmp4DC;struct
Cyc_Lexing_Error_struct*_tmp4DB;(int)_throw((void*)((_tmp4DB=_cycalloc(sizeof(*
_tmp4DB)),((_tmp4DB[0]=((_tmp4DD.tag=Cyc_Lexing_Error,((_tmp4DD.f1=((_tmp4DC="some action didn't return!",
_tag_dyneither(_tmp4DC,sizeof(char),27))),_tmp4DD)))),_tmp4DB)))));};}int Cyc_block_comment(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}void Cyc_scan_type(
void*t);struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(
struct Cyc_Absyn_Exp*e){void*_tmpBB=((struct Cyc_Absyn_Exp*)_check_null(e))->r;
struct _tuple0*_tmpBD;struct _tuple0 _tmpBE;struct _dyneither_ptr*_tmpBF;struct
_tuple0*_tmpC1;struct _tuple0 _tmpC2;struct _dyneither_ptr*_tmpC3;struct Cyc_List_List*
_tmpC5;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*
_tmpCA;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*
_tmpCE;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*
_tmpD4;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*
_tmpD9;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*
_tmpDD;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*
_tmpE2;struct Cyc_List_List*_tmpE3;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_List_List*
_tmpE6;void*_tmpE8;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_MallocInfo _tmpEB;
int _tmpEC;struct Cyc_Absyn_Exp*_tmpED;void**_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;
struct Cyc_Absyn_Exp*_tmpF1;void*_tmpF3;void*_tmpF5;struct Cyc_Absyn_Exp*_tmpF7;
struct _dyneither_ptr*_tmpF8;struct Cyc_Absyn_Exp*_tmpFA;struct _dyneither_ptr*
_tmpFB;void*_tmpFD;void*_tmpFE;struct Cyc_List_List*_tmp101;_LLCD: {struct Cyc_Absyn_Var_e_struct*
_tmpBC=(struct Cyc_Absyn_Var_e_struct*)_tmpBB;if(_tmpBC->tag != 1)goto _LLCF;else{
_tmpBD=_tmpBC->f1;_tmpBE=*_tmpBD;_tmpBF=_tmpBE.f2;}}_LLCE: _tmpC3=_tmpBF;goto
_LLD0;_LLCF: {struct Cyc_Absyn_UnknownId_e_struct*_tmpC0=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmpBB;if(_tmpC0->tag != 2)goto _LLD1;else{_tmpC1=_tmpC0->f1;_tmpC2=*_tmpC1;_tmpC3=
_tmpC2.f2;}}_LLD0: Cyc_add_target(_tmpC3);return;_LLD1: {struct Cyc_Absyn_Primop_e_struct*
_tmpC4=(struct Cyc_Absyn_Primop_e_struct*)_tmpBB;if(_tmpC4->tag != 3)goto _LLD3;
else{_tmpC5=_tmpC4->f2;}}_LLD2: for(0;_tmpC5 != 0;_tmpC5=_tmpC5->tl){Cyc_scan_exp((
struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmpC5->hd));}return;_LLD3: {struct
Cyc_Absyn_Subscript_e_struct*_tmpC6=(struct Cyc_Absyn_Subscript_e_struct*)_tmpBB;
if(_tmpC6->tag != 24)goto _LLD5;else{_tmpC7=_tmpC6->f1;_tmpC8=_tmpC6->f2;}}_LLD4:
_tmpCA=_tmpC7;_tmpCB=_tmpC8;goto _LLD6;_LLD5: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpC9=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpBB;if(_tmpC9->tag != 9)goto _LLD7;
else{_tmpCA=_tmpC9->f1;_tmpCB=_tmpC9->f2;}}_LLD6: _tmpCD=_tmpCA;_tmpCE=_tmpCB;
goto _LLD8;_LLD7: {struct Cyc_Absyn_AssignOp_e_struct*_tmpCC=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmpBB;if(_tmpCC->tag != 4)goto _LLD9;else{_tmpCD=_tmpCC->f1;_tmpCE=_tmpCC->f3;}}
_LLD8: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpCD);Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpCE);return;_LLD9: {struct Cyc_Absyn_Deref_e_struct*_tmpCF=(struct Cyc_Absyn_Deref_e_struct*)
_tmpBB;if(_tmpCF->tag != 21)goto _LLDB;else{_tmpD0=_tmpCF->f1;}}_LLDA: _tmpD2=
_tmpD0;goto _LLDC;_LLDB: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpD1=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpBB;if(_tmpD1->tag != 19)goto _LLDD;else{_tmpD2=_tmpD1->f1;}}_LLDC: _tmpD4=
_tmpD2;goto _LLDE;_LLDD: {struct Cyc_Absyn_Address_e_struct*_tmpD3=(struct Cyc_Absyn_Address_e_struct*)
_tmpBB;if(_tmpD3->tag != 16)goto _LLDF;else{_tmpD4=_tmpD3->f1;}}_LLDE: _tmpD6=
_tmpD4;goto _LLE0;_LLDF: {struct Cyc_Absyn_Increment_e_struct*_tmpD5=(struct Cyc_Absyn_Increment_e_struct*)
_tmpBB;if(_tmpD5->tag != 5)goto _LLE1;else{_tmpD6=_tmpD5->f1;}}_LLE0: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpD6);return;_LLE1: {struct Cyc_Absyn_Conditional_e_struct*
_tmpD7=(struct Cyc_Absyn_Conditional_e_struct*)_tmpBB;if(_tmpD7->tag != 6)goto
_LLE3;else{_tmpD8=_tmpD7->f1;_tmpD9=_tmpD7->f2;_tmpDA=_tmpD7->f3;}}_LLE2: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpD8);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD9);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpDA);return;_LLE3: {struct Cyc_Absyn_And_e_struct*_tmpDB=(
struct Cyc_Absyn_And_e_struct*)_tmpBB;if(_tmpDB->tag != 7)goto _LLE5;else{_tmpDC=
_tmpDB->f1;_tmpDD=_tmpDB->f2;}}_LLE4: _tmpDF=_tmpDC;_tmpE0=_tmpDD;goto _LLE6;_LLE5: {
struct Cyc_Absyn_Or_e_struct*_tmpDE=(struct Cyc_Absyn_Or_e_struct*)_tmpBB;if(
_tmpDE->tag != 8)goto _LLE7;else{_tmpDF=_tmpDE->f1;_tmpE0=_tmpDE->f2;}}_LLE6: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpDF);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE0);return;
_LLE7: {struct Cyc_Absyn_FnCall_e_struct*_tmpE1=(struct Cyc_Absyn_FnCall_e_struct*)
_tmpBB;if(_tmpE1->tag != 11)goto _LLE9;else{_tmpE2=_tmpE1->f1;_tmpE3=_tmpE1->f2;}}
_LLE8: _tmpE5=_tmpE2;_tmpE6=_tmpE3;goto _LLEA;_LLE9: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmpE4=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmpBB;if(_tmpE4->tag != 10)goto
_LLEB;else{_tmpE5=_tmpE4->f1;_tmpE6=_tmpE4->f2;}}_LLEA: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpE5);for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpE6->hd));}return;_LLEB: {struct Cyc_Absyn_Cast_e_struct*
_tmpE7=(struct Cyc_Absyn_Cast_e_struct*)_tmpBB;if(_tmpE7->tag != 15)goto _LLED;
else{_tmpE8=(void*)_tmpE7->f1;_tmpE9=_tmpE7->f2;}}_LLEC: Cyc_scan_type(_tmpE8);
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE9);return;_LLED: {struct Cyc_Absyn_Malloc_e_struct*
_tmpEA=(struct Cyc_Absyn_Malloc_e_struct*)_tmpBB;if(_tmpEA->tag != 34)goto _LLEF;
else{_tmpEB=_tmpEA->f1;_tmpEC=_tmpEB.is_calloc;_tmpED=_tmpEB.rgn;_tmpEE=_tmpEB.elt_type;
_tmpEF=_tmpEB.num_elts;}}_LLEE: if(_tmpED != 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpED));if(_tmpEE != 0)Cyc_scan_type(*_tmpEE);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpEF);return;_LLEF: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmpF0=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmpBB;if(_tmpF0->tag != 38)goto _LLF1;
else{_tmpF1=_tmpF0->f1;}}_LLF0: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpF1);
return;_LLF1: {struct Cyc_Absyn_Valueof_e_struct*_tmpF2=(struct Cyc_Absyn_Valueof_e_struct*)
_tmpBB;if(_tmpF2->tag != 39)goto _LLF3;else{_tmpF3=(void*)_tmpF2->f1;}}_LLF2:
_tmpF5=_tmpF3;goto _LLF4;_LLF3: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpF4=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpBB;if(_tmpF4->tag != 18)goto _LLF5;else{
_tmpF5=(void*)_tmpF4->f1;}}_LLF4: Cyc_scan_type(_tmpF5);return;_LLF5: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpF6=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpBB;if(_tmpF6->tag != 22)goto
_LLF7;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f2;}}_LLF6: _tmpFA=_tmpF7;_tmpFB=
_tmpF8;goto _LLF8;_LLF7: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF9=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpBB;if(_tmpF9->tag != 23)goto _LLF9;else{_tmpFA=_tmpF9->f1;_tmpFB=_tmpF9->f2;}}
_LLF8: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpFA);Cyc_add_target(_tmpFB);return;
_LLF9: {struct Cyc_Absyn_Offsetof_e_struct*_tmpFC=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpBB;if(_tmpFC->tag != 20)goto _LLFB;else{_tmpFD=(void*)_tmpFC->f1;_tmpFE=(void*)
_tmpFC->f2;}}_LLFA: Cyc_scan_type(_tmpFD);{void*_tmp112=_tmpFE;struct
_dyneither_ptr*_tmp114;_LL120: {struct Cyc_Absyn_StructField_struct*_tmp113=(
struct Cyc_Absyn_StructField_struct*)_tmp112;if(_tmp113->tag != 0)goto _LL122;else{
_tmp114=_tmp113->f1;}}_LL121: Cyc_add_target(_tmp114);goto _LL11F;_LL122: {struct
Cyc_Absyn_TupleIndex_struct*_tmp115=(struct Cyc_Absyn_TupleIndex_struct*)_tmp112;
if(_tmp115->tag != 1)goto _LL11F;}_LL123: goto _LL11F;_LL11F:;}return;_LLFB: {struct
Cyc_Absyn_Const_e_struct*_tmpFF=(struct Cyc_Absyn_Const_e_struct*)_tmpBB;if(
_tmpFF->tag != 0)goto _LLFD;}_LLFC: return;_LLFD: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp100=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpBB;if(_tmp100->tag != 36)
goto _LLFF;else{_tmp101=_tmp100->f2;}}_LLFE: for(0;_tmp101 != 0;_tmp101=_tmp101->tl){
struct _tuple21 _tmp117;struct Cyc_Absyn_Exp*_tmp118;struct _tuple21*_tmp116=(struct
_tuple21*)_tmp101->hd;_tmp117=*_tmp116;_tmp118=_tmp117.f2;Cyc_scan_exp((struct
Cyc_Absyn_Exp*)_tmp118);}return;_LLFF: {struct Cyc_Absyn_Asm_e_struct*_tmp102=(
struct Cyc_Absyn_Asm_e_struct*)_tmpBB;if(_tmp102->tag != 40)goto _LL101;}_LL100:
return;_LL101: {struct Cyc_Absyn_Swap_e_struct*_tmp103=(struct Cyc_Absyn_Swap_e_struct*)
_tmpBB;if(_tmp103->tag != 35)goto _LL103;}_LL102:{const char*_tmp4E0;void*_tmp4DF;(
_tmp4DF=0,Cyc_fprintf(Cyc_stderr,((_tmp4E0="Error: unexpected Swap_e\n",
_tag_dyneither(_tmp4E0,sizeof(char),26))),_tag_dyneither(_tmp4DF,sizeof(void*),0)));}
exit(1);return;_LL103: {struct Cyc_Absyn_StmtExp_e_struct*_tmp104=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpBB;if(_tmp104->tag != 37)goto _LL105;}_LL104:{const char*_tmp4E3;void*_tmp4E2;(
_tmp4E2=0,Cyc_fprintf(Cyc_stderr,((_tmp4E3="Error: unexpected Stmt_e\n",
_tag_dyneither(_tmp4E3,sizeof(char),26))),_tag_dyneither(_tmp4E2,sizeof(void*),0)));}
exit(1);return;_LL105: {struct Cyc_Absyn_Throw_e_struct*_tmp105=(struct Cyc_Absyn_Throw_e_struct*)
_tmpBB;if(_tmp105->tag != 12)goto _LL107;}_LL106:{const char*_tmp4E6;void*_tmp4E5;(
_tmp4E5=0,Cyc_fprintf(Cyc_stderr,((_tmp4E6="Error: unexpected Throw_e\n",
_tag_dyneither(_tmp4E6,sizeof(char),27))),_tag_dyneither(_tmp4E5,sizeof(void*),0)));}
exit(1);return;_LL107: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp106=(struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmpBB;if(_tmp106->tag != 13)goto _LL109;}_LL108:{
const char*_tmp4E9;void*_tmp4E8;(_tmp4E8=0,Cyc_fprintf(Cyc_stderr,((_tmp4E9="Error: unexpected NoInstantiate_e\n",
_tag_dyneither(_tmp4E9,sizeof(char),35))),_tag_dyneither(_tmp4E8,sizeof(void*),0)));}
exit(1);return;_LL109: {struct Cyc_Absyn_Instantiate_e_struct*_tmp107=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpBB;if(_tmp107->tag != 14)goto _LL10B;}_LL10A:{const char*_tmp4EC;void*_tmp4EB;(
_tmp4EB=0,Cyc_fprintf(Cyc_stderr,((_tmp4EC="Error: unexpected Instantiate_e\n",
_tag_dyneither(_tmp4EC,sizeof(char),33))),_tag_dyneither(_tmp4EB,sizeof(void*),0)));}
exit(1);return;_LL10B: {struct Cyc_Absyn_New_e_struct*_tmp108=(struct Cyc_Absyn_New_e_struct*)
_tmpBB;if(_tmp108->tag != 17)goto _LL10D;}_LL10C:{const char*_tmp4EF;void*_tmp4EE;(
_tmp4EE=0,Cyc_fprintf(Cyc_stderr,((_tmp4EF="Error: unexpected New_e\n",
_tag_dyneither(_tmp4EF,sizeof(char),25))),_tag_dyneither(_tmp4EE,sizeof(void*),0)));}
exit(1);return;_LL10D: {struct Cyc_Absyn_Tuple_e_struct*_tmp109=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpBB;if(_tmp109->tag != 25)goto _LL10F;}_LL10E:{const char*_tmp4F2;void*_tmp4F1;(
_tmp4F1=0,Cyc_fprintf(Cyc_stderr,((_tmp4F2="Error: unexpected Tuple_e\n",
_tag_dyneither(_tmp4F2,sizeof(char),27))),_tag_dyneither(_tmp4F1,sizeof(void*),0)));}
exit(1);return;_LL10F: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp10A=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpBB;if(_tmp10A->tag != 26)goto _LL111;}_LL110:{const char*_tmp4F5;void*_tmp4F4;(
_tmp4F4=0,Cyc_fprintf(Cyc_stderr,((_tmp4F5="Error: unexpected CompoundLit_e\n",
_tag_dyneither(_tmp4F5,sizeof(char),33))),_tag_dyneither(_tmp4F4,sizeof(void*),0)));}
exit(1);return;_LL111: {struct Cyc_Absyn_Array_e_struct*_tmp10B=(struct Cyc_Absyn_Array_e_struct*)
_tmpBB;if(_tmp10B->tag != 27)goto _LL113;}_LL112:{const char*_tmp4F8;void*_tmp4F7;(
_tmp4F7=0,Cyc_fprintf(Cyc_stderr,((_tmp4F8="Error: unexpected Array_e\n",
_tag_dyneither(_tmp4F8,sizeof(char),27))),_tag_dyneither(_tmp4F7,sizeof(void*),0)));}
exit(1);return;_LL113: {struct Cyc_Absyn_Comprehension_e_struct*_tmp10C=(struct
Cyc_Absyn_Comprehension_e_struct*)_tmpBB;if(_tmp10C->tag != 28)goto _LL115;}_LL114:{
const char*_tmp4FB;void*_tmp4FA;(_tmp4FA=0,Cyc_fprintf(Cyc_stderr,((_tmp4FB="Error: unexpected Comprehension_e\n",
_tag_dyneither(_tmp4FB,sizeof(char),35))),_tag_dyneither(_tmp4FA,sizeof(void*),0)));}
exit(1);return;_LL115: {struct Cyc_Absyn_Aggregate_e_struct*_tmp10D=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpBB;if(_tmp10D->tag != 29)goto _LL117;}_LL116:{const char*_tmp4FE;void*_tmp4FD;(
_tmp4FD=0,Cyc_fprintf(Cyc_stderr,((_tmp4FE="Error: unexpected Aggregate_e\n",
_tag_dyneither(_tmp4FE,sizeof(char),31))),_tag_dyneither(_tmp4FD,sizeof(void*),0)));}
exit(1);return;_LL117: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp10E=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpBB;if(_tmp10E->tag != 30)goto _LL119;}_LL118:{const char*_tmp501;void*_tmp500;(
_tmp500=0,Cyc_fprintf(Cyc_stderr,((_tmp501="Error: unexpected AnonStruct_e\n",
_tag_dyneither(_tmp501,sizeof(char),32))),_tag_dyneither(_tmp500,sizeof(void*),0)));}
exit(1);return;_LL119: {struct Cyc_Absyn_Datatype_e_struct*_tmp10F=(struct Cyc_Absyn_Datatype_e_struct*)
_tmpBB;if(_tmp10F->tag != 31)goto _LL11B;}_LL11A:{const char*_tmp504;void*_tmp503;(
_tmp503=0,Cyc_fprintf(Cyc_stderr,((_tmp504="Error: unexpected Datatype_e\n",
_tag_dyneither(_tmp504,sizeof(char),30))),_tag_dyneither(_tmp503,sizeof(void*),0)));}
exit(1);return;_LL11B: {struct Cyc_Absyn_Enum_e_struct*_tmp110=(struct Cyc_Absyn_Enum_e_struct*)
_tmpBB;if(_tmp110->tag != 32)goto _LL11D;}_LL11C:{const char*_tmp507;void*_tmp506;(
_tmp506=0,Cyc_fprintf(Cyc_stderr,((_tmp507="Error: unexpected Enum_e\n",
_tag_dyneither(_tmp507,sizeof(char),26))),_tag_dyneither(_tmp506,sizeof(void*),0)));}
exit(1);return;_LL11D: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp111=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpBB;if(_tmp111->tag != 33)goto _LLCC;}_LL11E:{const char*_tmp50A;void*_tmp509;(
_tmp509=0,Cyc_fprintf(Cyc_stderr,((_tmp50A="Error: unexpected AnonEnum_e\n",
_tag_dyneither(_tmp50A,sizeof(char),30))),_tag_dyneither(_tmp509,sizeof(void*),0)));}
exit(1);return;_LLCC:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo){if((
unsigned int)eo)Cyc_scan_exp(eo);return;}void Cyc_scan_type(void*t){void*_tmp137=
t;struct Cyc_Absyn_PtrInfo _tmp13D;struct Cyc_Absyn_ArrayInfo _tmp13F;void*_tmp140;
struct Cyc_Absyn_Exp*_tmp141;union Cyc_Absyn_Constraint*_tmp142;struct Cyc_Absyn_FnInfo
_tmp144;struct Cyc_List_List*_tmp146;struct Cyc_Absyn_AggrInfo _tmp149;union Cyc_Absyn_AggrInfoU
_tmp14A;struct _tuple0*_tmp14C;struct _tuple0 _tmp14D;struct _dyneither_ptr*_tmp14E;
struct _tuple0*_tmp150;struct _tuple0 _tmp151;struct _dyneither_ptr*_tmp152;struct
Cyc_Absyn_TypeDecl*_tmp154;struct Cyc_Absyn_TypeDecl _tmp155;void*_tmp156;struct
Cyc_Absyn_Aggrdecl*_tmp158;struct Cyc_Absyn_TypeDecl*_tmp15A;struct Cyc_Absyn_TypeDecl
_tmp15B;void*_tmp15C;struct Cyc_Absyn_Enumdecl*_tmp15E;struct Cyc_Absyn_TypeDecl*
_tmp160;struct Cyc_Absyn_TypeDecl _tmp161;void*_tmp162;struct Cyc_Absyn_Datatypedecl*
_tmp164;_LL125: {struct Cyc_Absyn_VoidType_struct*_tmp138=(struct Cyc_Absyn_VoidType_struct*)
_tmp137;if(_tmp138->tag != 0)goto _LL127;}_LL126: goto _LL128;_LL127: {struct Cyc_Absyn_IntType_struct*
_tmp139=(struct Cyc_Absyn_IntType_struct*)_tmp137;if(_tmp139->tag != 6)goto _LL129;}
_LL128: goto _LL12A;_LL129: {struct Cyc_Absyn_FloatType_struct*_tmp13A=(struct Cyc_Absyn_FloatType_struct*)
_tmp137;if(_tmp13A->tag != 7)goto _LL12B;}_LL12A: goto _LL12C;_LL12B: {struct Cyc_Absyn_DoubleType_struct*
_tmp13B=(struct Cyc_Absyn_DoubleType_struct*)_tmp137;if(_tmp13B->tag != 8)goto
_LL12D;}_LL12C: return;_LL12D: {struct Cyc_Absyn_PointerType_struct*_tmp13C=(
struct Cyc_Absyn_PointerType_struct*)_tmp137;if(_tmp13C->tag != 5)goto _LL12F;else{
_tmp13D=_tmp13C->f1;}}_LL12E: Cyc_scan_type(_tmp13D.elt_typ);return;_LL12F: {
struct Cyc_Absyn_ArrayType_struct*_tmp13E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp137;if(_tmp13E->tag != 9)goto _LL131;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13F.elt_type;
_tmp141=_tmp13F.num_elts;_tmp142=_tmp13F.zero_term;}}_LL130: Cyc_scan_type(
_tmp140);Cyc_scan_exp_opt(_tmp141);return;_LL131: {struct Cyc_Absyn_FnType_struct*
_tmp143=(struct Cyc_Absyn_FnType_struct*)_tmp137;if(_tmp143->tag != 10)goto _LL133;
else{_tmp144=_tmp143->f1;}}_LL132: Cyc_scan_type(_tmp144.ret_typ);{struct Cyc_List_List*
_tmp173=_tmp144.args;for(0;_tmp173 != 0;_tmp173=_tmp173->tl){struct _tuple7 _tmp175;
void*_tmp176;struct _tuple7*_tmp174=(struct _tuple7*)_tmp173->hd;_tmp175=*_tmp174;
_tmp176=_tmp175.f3;Cyc_scan_type(_tmp176);}}if(_tmp144.cyc_varargs != 0)Cyc_scan_type((
_tmp144.cyc_varargs)->type);return;_LL133: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp145=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp137;if(_tmp145->tag != 13)goto
_LL135;else{_tmp146=_tmp145->f2;}}_LL134: for(0;_tmp146 != 0;_tmp146=_tmp146->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp146->hd)->type);Cyc_scan_exp_opt(((
struct Cyc_Absyn_Aggrfield*)_tmp146->hd)->width);}return;_LL135: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp147=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp137;if(_tmp147->tag != 15)goto
_LL137;}_LL136: return;_LL137: {struct Cyc_Absyn_AggrType_struct*_tmp148=(struct
Cyc_Absyn_AggrType_struct*)_tmp137;if(_tmp148->tag != 12)goto _LL139;else{_tmp149=
_tmp148->f1;_tmp14A=_tmp149.aggr_info;}}_LL138: {struct _tuple0*_tmp178;struct
_tuple0 _tmp179;struct _dyneither_ptr*_tmp17A;struct _tuple9 _tmp177=Cyc_Absyn_aggr_kinded_name(
_tmp14A);_tmp178=_tmp177.f2;_tmp179=*_tmp178;_tmp17A=_tmp179.f2;_tmp14E=_tmp17A;
goto _LL13A;}_LL139: {struct Cyc_Absyn_EnumType_struct*_tmp14B=(struct Cyc_Absyn_EnumType_struct*)
_tmp137;if(_tmp14B->tag != 14)goto _LL13B;else{_tmp14C=_tmp14B->f1;_tmp14D=*
_tmp14C;_tmp14E=_tmp14D.f2;}}_LL13A: _tmp152=_tmp14E;goto _LL13C;_LL13B: {struct
Cyc_Absyn_TypedefType_struct*_tmp14F=(struct Cyc_Absyn_TypedefType_struct*)
_tmp137;if(_tmp14F->tag != 18)goto _LL13D;else{_tmp150=_tmp14F->f1;_tmp151=*
_tmp150;_tmp152=_tmp151.f2;}}_LL13C: Cyc_add_target(_tmp152);return;_LL13D: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp153=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp137;if(_tmp153->tag != 26)goto _LL13F;else{_tmp154=_tmp153->f1;_tmp155=*
_tmp154;_tmp156=_tmp155.r;{struct Cyc_Absyn_Aggr_td_struct*_tmp157=(struct Cyc_Absyn_Aggr_td_struct*)
_tmp156;if(_tmp157->tag != 0)goto _LL13F;else{_tmp158=_tmp157->f1;}};}}_LL13E: {
struct _tuple0 _tmp17C;struct _dyneither_ptr*_tmp17D;struct _tuple0*_tmp17B=_tmp158->name;
_tmp17C=*_tmp17B;_tmp17D=_tmp17C.f2;Cyc_current_source=(struct _dyneither_ptr*)
_tmp17D;if((unsigned int)_tmp158->impl){{struct Cyc_List_List*_tmp17E=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp158->impl))->fields;for(0;_tmp17E != 0;_tmp17E=_tmp17E->tl){
struct Cyc_Absyn_Aggrfield*_tmp17F=(struct Cyc_Absyn_Aggrfield*)_tmp17E->hd;Cyc_scan_type(
_tmp17F->type);Cyc_scan_exp_opt(_tmp17F->width);}}{struct Cyc_List_List*_tmp180=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp158->impl))->fields;for(0;_tmp180
!= 0;_tmp180=_tmp180->tl){;}};}return;}_LL13F: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp159=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp137;if(_tmp159->tag != 26)goto
_LL141;else{_tmp15A=_tmp159->f1;_tmp15B=*_tmp15A;_tmp15C=_tmp15B.r;{struct Cyc_Absyn_Enum_td_struct*
_tmp15D=(struct Cyc_Absyn_Enum_td_struct*)_tmp15C;if(_tmp15D->tag != 1)goto _LL141;
else{_tmp15E=_tmp15D->f1;}};}}_LL140: {struct _tuple0 _tmp182;struct _dyneither_ptr*
_tmp183;struct _tuple0*_tmp181=_tmp15E->name;_tmp182=*_tmp181;_tmp183=_tmp182.f2;
Cyc_current_source=(struct _dyneither_ptr*)_tmp183;if((unsigned int)_tmp15E->fields){{
struct Cyc_List_List*_tmp184=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp15E->fields))->v;for(0;_tmp184 != 0;_tmp184=_tmp184->tl){struct
Cyc_Absyn_Enumfield*_tmp185=(struct Cyc_Absyn_Enumfield*)_tmp184->hd;Cyc_scan_exp_opt(
_tmp185->tag);}}{struct Cyc_List_List*_tmp186=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp15E->fields))->v;for(0;_tmp186 != 0;_tmp186=_tmp186->tl){;}};}
return;}_LL141: {struct Cyc_Absyn_TypeDeclType_struct*_tmp15F=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp137;if(_tmp15F->tag != 26)goto _LL143;else{_tmp160=_tmp15F->f1;_tmp161=*
_tmp160;_tmp162=_tmp161.r;{struct Cyc_Absyn_Datatype_td_struct*_tmp163=(struct Cyc_Absyn_Datatype_td_struct*)
_tmp162;if(_tmp163->tag != 2)goto _LL143;else{_tmp164=_tmp163->f1;}};}}_LL142:{
const char*_tmp50D;void*_tmp50C;(_tmp50C=0,Cyc_fprintf(Cyc_stderr,((_tmp50D="Error: unexpected Datatype declaration\n",
_tag_dyneither(_tmp50D,sizeof(char),40))),_tag_dyneither(_tmp50C,sizeof(void*),0)));}
exit(1);return;_LL143: {struct Cyc_Absyn_Evar_struct*_tmp165=(struct Cyc_Absyn_Evar_struct*)
_tmp137;if(_tmp165->tag != 1)goto _LL145;}_LL144:{const char*_tmp510;void*_tmp50F;(
_tmp50F=0,Cyc_fprintf(Cyc_stderr,((_tmp510="Error: unexpected Evar\n",
_tag_dyneither(_tmp510,sizeof(char),24))),_tag_dyneither(_tmp50F,sizeof(void*),0)));}
exit(1);return;_LL145: {struct Cyc_Absyn_VarType_struct*_tmp166=(struct Cyc_Absyn_VarType_struct*)
_tmp137;if(_tmp166->tag != 2)goto _LL147;}_LL146:{const char*_tmp513;void*_tmp512;(
_tmp512=0,Cyc_fprintf(Cyc_stderr,((_tmp513="Error: unexpected VarType\n",
_tag_dyneither(_tmp513,sizeof(char),27))),_tag_dyneither(_tmp512,sizeof(void*),0)));}
exit(1);return;_LL147: {struct Cyc_Absyn_DatatypeType_struct*_tmp167=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp137;if(_tmp167->tag != 3)goto _LL149;}_LL148:{const char*_tmp516;void*_tmp515;(
_tmp515=0,Cyc_fprintf(Cyc_stderr,((_tmp516="Error: unexpected DatatypeType\n",
_tag_dyneither(_tmp516,sizeof(char),32))),_tag_dyneither(_tmp515,sizeof(void*),0)));}
exit(1);return;_LL149: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp168=(struct
Cyc_Absyn_DatatypeFieldType_struct*)_tmp137;if(_tmp168->tag != 4)goto _LL14B;}
_LL14A:{const char*_tmp519;void*_tmp518;(_tmp518=0,Cyc_fprintf(Cyc_stderr,((
_tmp519="Error: unexpected DatatypeFieldType\n",_tag_dyneither(_tmp519,sizeof(
char),37))),_tag_dyneither(_tmp518,sizeof(void*),0)));}exit(1);return;_LL14B: {
struct Cyc_Absyn_TupleType_struct*_tmp169=(struct Cyc_Absyn_TupleType_struct*)
_tmp137;if(_tmp169->tag != 11)goto _LL14D;}_LL14C:{const char*_tmp51C;void*_tmp51B;(
_tmp51B=0,Cyc_fprintf(Cyc_stderr,((_tmp51C="Error: unexpected TupleType\n",
_tag_dyneither(_tmp51C,sizeof(char),29))),_tag_dyneither(_tmp51B,sizeof(void*),0)));}
exit(1);return;_LL14D: {struct Cyc_Absyn_RgnHandleType_struct*_tmp16A=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp137;if(_tmp16A->tag != 16)goto _LL14F;}_LL14E:{const char*_tmp51F;void*_tmp51E;(
_tmp51E=0,Cyc_fprintf(Cyc_stderr,((_tmp51F="Error: unexpected RgnHandleType\n",
_tag_dyneither(_tmp51F,sizeof(char),33))),_tag_dyneither(_tmp51E,sizeof(void*),0)));}
exit(1);return;_LL14F: {struct Cyc_Absyn_DynRgnType_struct*_tmp16B=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp137;if(_tmp16B->tag != 17)goto _LL151;}_LL150:{const char*_tmp522;void*_tmp521;(
_tmp521=0,Cyc_fprintf(Cyc_stderr,((_tmp522="Error: unexpected DynRgnType\n",
_tag_dyneither(_tmp522,sizeof(char),30))),_tag_dyneither(_tmp521,sizeof(void*),0)));}
exit(1);return;_LL151: {struct Cyc_Absyn_HeapRgn_struct*_tmp16C=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp137;if(_tmp16C->tag != 21)goto _LL153;}_LL152:{const char*_tmp525;void*_tmp524;(
_tmp524=0,Cyc_fprintf(Cyc_stderr,((_tmp525="Error: unexpected HeapRgn\n",
_tag_dyneither(_tmp525,sizeof(char),27))),_tag_dyneither(_tmp524,sizeof(void*),0)));}
exit(1);return;_LL153: {struct Cyc_Absyn_UniqueRgn_struct*_tmp16D=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp137;if(_tmp16D->tag != 22)goto _LL155;}_LL154:{const char*_tmp528;void*_tmp527;(
_tmp527=0,Cyc_fprintf(Cyc_stderr,((_tmp528="Error: unexpected UniqueRgn\n",
_tag_dyneither(_tmp528,sizeof(char),29))),_tag_dyneither(_tmp527,sizeof(void*),0)));}
exit(1);return;_LL155: {struct Cyc_Absyn_AccessEff_struct*_tmp16E=(struct Cyc_Absyn_AccessEff_struct*)
_tmp137;if(_tmp16E->tag != 23)goto _LL157;}_LL156:{const char*_tmp52B;void*_tmp52A;(
_tmp52A=0,Cyc_fprintf(Cyc_stderr,((_tmp52B="Error: unexpected AccessEff\n",
_tag_dyneither(_tmp52B,sizeof(char),29))),_tag_dyneither(_tmp52A,sizeof(void*),0)));}
exit(1);return;_LL157: {struct Cyc_Absyn_JoinEff_struct*_tmp16F=(struct Cyc_Absyn_JoinEff_struct*)
_tmp137;if(_tmp16F->tag != 24)goto _LL159;}_LL158:{const char*_tmp52E;void*_tmp52D;(
_tmp52D=0,Cyc_fprintf(Cyc_stderr,((_tmp52E="Error: unexpected JoinEff\n",
_tag_dyneither(_tmp52E,sizeof(char),27))),_tag_dyneither(_tmp52D,sizeof(void*),0)));}
exit(1);return;_LL159: {struct Cyc_Absyn_RgnsEff_struct*_tmp170=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp137;if(_tmp170->tag != 25)goto _LL15B;}_LL15A:{const char*_tmp531;void*_tmp530;(
_tmp530=0,Cyc_fprintf(Cyc_stderr,((_tmp531="Error: unexpected RgnsEff\n",
_tag_dyneither(_tmp531,sizeof(char),27))),_tag_dyneither(_tmp530,sizeof(void*),0)));}
exit(1);return;_LL15B: {struct Cyc_Absyn_TagType_struct*_tmp171=(struct Cyc_Absyn_TagType_struct*)
_tmp137;if(_tmp171->tag != 20)goto _LL15D;}_LL15C:{const char*_tmp534;void*_tmp533;(
_tmp533=0,Cyc_fprintf(Cyc_stderr,((_tmp534="Error: unexpected tag_t\n",
_tag_dyneither(_tmp534,sizeof(char),25))),_tag_dyneither(_tmp533,sizeof(void*),0)));}
exit(1);return;_LL15D: {struct Cyc_Absyn_ValueofType_struct*_tmp172=(struct Cyc_Absyn_ValueofType_struct*)
_tmp137;if(_tmp172->tag != 19)goto _LL124;}_LL15E:{const char*_tmp537;void*_tmp536;(
_tmp536=0,Cyc_fprintf(Cyc_stderr,((_tmp537="Error: unexpected valueof_t\n",
_tag_dyneither(_tmp537,sizeof(char),29))),_tag_dyneither(_tmp536,sizeof(void*),0)));}
exit(1);return;_LL124:;}struct _tuple22{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple17*Cyc_scan_decl(struct Cyc_Absyn_Decl*d){{struct Cyc_Set_Set**
_tmp538;Cyc_current_targets=((_tmp538=_cycalloc(sizeof(*_tmp538)),((_tmp538[0]=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Set_empty)(Cyc_strptrcmp),_tmp538))));}{void*_tmp1A6=d->r;struct Cyc_Absyn_Vardecl*
_tmp1A8;struct Cyc_Absyn_Fndecl*_tmp1AA;struct Cyc_Absyn_Aggrdecl*_tmp1AC;struct
Cyc_Absyn_Enumdecl*_tmp1AE;struct Cyc_Absyn_Typedefdecl*_tmp1B0;_LL160: {struct
Cyc_Absyn_Var_d_struct*_tmp1A7=(struct Cyc_Absyn_Var_d_struct*)_tmp1A6;if(_tmp1A7->tag
!= 0)goto _LL162;else{_tmp1A8=_tmp1A7->f1;}}_LL161: {struct _tuple0 _tmp1BD;struct
_dyneither_ptr*_tmp1BE;struct _tuple0*_tmp1BC=_tmp1A8->name;_tmp1BD=*_tmp1BC;
_tmp1BE=_tmp1BD.f2;Cyc_current_source=(struct _dyneither_ptr*)_tmp1BE;Cyc_scan_type(
_tmp1A8->type);Cyc_scan_exp_opt(_tmp1A8->initializer);goto _LL15F;}_LL162: {
struct Cyc_Absyn_Fn_d_struct*_tmp1A9=(struct Cyc_Absyn_Fn_d_struct*)_tmp1A6;if(
_tmp1A9->tag != 1)goto _LL164;else{_tmp1AA=_tmp1A9->f1;}}_LL163: {struct _tuple0
_tmp1C0;struct _dyneither_ptr*_tmp1C1;struct _tuple0*_tmp1BF=_tmp1AA->name;_tmp1C0=*
_tmp1BF;_tmp1C1=_tmp1C0.f2;Cyc_current_source=(struct _dyneither_ptr*)_tmp1C1;Cyc_scan_type(
_tmp1AA->ret_type);{struct Cyc_List_List*_tmp1C2=_tmp1AA->args;for(0;_tmp1C2 != 0;
_tmp1C2=_tmp1C2->tl){struct _tuple22 _tmp1C4;void*_tmp1C5;struct _tuple22*_tmp1C3=(
struct _tuple22*)_tmp1C2->hd;_tmp1C4=*_tmp1C3;_tmp1C5=_tmp1C4.f3;Cyc_scan_type(
_tmp1C5);}}if(_tmp1AA->cyc_varargs != 0)Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp1AA->cyc_varargs))->type);if(_tmp1AA->is_inline){const char*
_tmp53B;void*_tmp53A;(_tmp53A=0,Cyc_fprintf(Cyc_stderr,((_tmp53B="Warning: ignoring inline function\n",
_tag_dyneither(_tmp53B,sizeof(char),35))),_tag_dyneither(_tmp53A,sizeof(void*),0)));}
goto _LL15F;}_LL164: {struct Cyc_Absyn_Aggr_d_struct*_tmp1AB=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp1A6;if(_tmp1AB->tag != 6)goto _LL166;else{_tmp1AC=_tmp1AB->f1;}}_LL165: {
struct _tuple0 _tmp1C9;struct _dyneither_ptr*_tmp1CA;struct _tuple0*_tmp1C8=_tmp1AC->name;
_tmp1C9=*_tmp1C8;_tmp1CA=_tmp1C9.f2;Cyc_current_source=(struct _dyneither_ptr*)
_tmp1CA;if((unsigned int)_tmp1AC->impl){{struct Cyc_List_List*_tmp1CB=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1AC->impl))->fields;for(0;_tmp1CB != 0;_tmp1CB=_tmp1CB->tl){
struct Cyc_Absyn_Aggrfield*_tmp1CC=(struct Cyc_Absyn_Aggrfield*)_tmp1CB->hd;Cyc_scan_type(
_tmp1CC->type);Cyc_scan_exp_opt(_tmp1CC->width);}}{struct Cyc_List_List*_tmp1CD=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AC->impl))->fields;for(0;_tmp1CD
!= 0;_tmp1CD=_tmp1CD->tl){;}};}goto _LL15F;}_LL166: {struct Cyc_Absyn_Enum_d_struct*
_tmp1AD=(struct Cyc_Absyn_Enum_d_struct*)_tmp1A6;if(_tmp1AD->tag != 8)goto _LL168;
else{_tmp1AE=_tmp1AD->f1;}}_LL167: {struct _tuple0 _tmp1CF;struct _dyneither_ptr*
_tmp1D0;struct _tuple0*_tmp1CE=_tmp1AE->name;_tmp1CF=*_tmp1CE;_tmp1D0=_tmp1CF.f2;
Cyc_current_source=(struct _dyneither_ptr*)_tmp1D0;if((unsigned int)_tmp1AE->fields){{
struct Cyc_List_List*_tmp1D1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1AE->fields))->v;for(0;_tmp1D1 != 0;_tmp1D1=_tmp1D1->tl){struct
Cyc_Absyn_Enumfield*_tmp1D2=(struct Cyc_Absyn_Enumfield*)_tmp1D1->hd;Cyc_scan_exp_opt(
_tmp1D2->tag);}}{struct Cyc_List_List*_tmp1D3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1AE->fields))->v;for(0;_tmp1D3 != 0;_tmp1D3=_tmp1D3->tl){;}};}
goto _LL15F;}_LL168: {struct Cyc_Absyn_Typedef_d_struct*_tmp1AF=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp1A6;if(_tmp1AF->tag != 9)goto _LL16A;else{_tmp1B0=_tmp1AF->f1;}}_LL169: {
struct _tuple0 _tmp1D5;struct _dyneither_ptr*_tmp1D6;struct _tuple0*_tmp1D4=_tmp1B0->name;
_tmp1D5=*_tmp1D4;_tmp1D6=_tmp1D5.f2;Cyc_current_source=(struct _dyneither_ptr*)
_tmp1D6;if((unsigned int)_tmp1B0->defn)Cyc_scan_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1B0->defn))->v);goto _LL15F;}_LL16A: {struct Cyc_Absyn_Region_d_struct*
_tmp1B1=(struct Cyc_Absyn_Region_d_struct*)_tmp1A6;if(_tmp1B1->tag != 4)goto _LL16C;}
_LL16B:{const char*_tmp53E;void*_tmp53D;(_tmp53D=0,Cyc_fprintf(Cyc_stderr,((
_tmp53E="Error: unexpected region declaration",_tag_dyneither(_tmp53E,sizeof(
char),37))),_tag_dyneither(_tmp53D,sizeof(void*),0)));}exit(1);_LL16C: {struct
Cyc_Absyn_Alias_d_struct*_tmp1B2=(struct Cyc_Absyn_Alias_d_struct*)_tmp1A6;if(
_tmp1B2->tag != 5)goto _LL16E;}_LL16D:{const char*_tmp541;void*_tmp540;(_tmp540=0,
Cyc_fprintf(Cyc_stderr,((_tmp541="Error: unexpected alias declaration",
_tag_dyneither(_tmp541,sizeof(char),36))),_tag_dyneither(_tmp540,sizeof(void*),0)));}
exit(1);_LL16E: {struct Cyc_Absyn_Porton_d_struct*_tmp1B3=(struct Cyc_Absyn_Porton_d_struct*)
_tmp1A6;if(_tmp1B3->tag != 14)goto _LL170;}_LL16F:{const char*_tmp544;void*_tmp543;(
_tmp543=0,Cyc_fprintf(Cyc_stderr,((_tmp544="Error: unexpected __cyclone_port_on__",
_tag_dyneither(_tmp544,sizeof(char),38))),_tag_dyneither(_tmp543,sizeof(void*),0)));}
exit(1);return 0;_LL170: {struct Cyc_Absyn_Portoff_d_struct*_tmp1B4=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp1A6;if(_tmp1B4->tag != 15)goto _LL172;}_LL171:{const char*_tmp547;void*_tmp546;(
_tmp546=0,Cyc_fprintf(Cyc_stderr,((_tmp547="Error: unexpected __cyclone_port_off__",
_tag_dyneither(_tmp547,sizeof(char),39))),_tag_dyneither(_tmp546,sizeof(void*),0)));}
exit(1);return 0;_LL172: {struct Cyc_Absyn_Let_d_struct*_tmp1B5=(struct Cyc_Absyn_Let_d_struct*)
_tmp1A6;if(_tmp1B5->tag != 2)goto _LL174;}_LL173:{const char*_tmp54A;void*_tmp549;(
_tmp549=0,Cyc_fprintf(Cyc_stderr,((_tmp54A="Error: unexpected let declaration\n",
_tag_dyneither(_tmp54A,sizeof(char),35))),_tag_dyneither(_tmp549,sizeof(void*),0)));}
exit(1);return 0;_LL174: {struct Cyc_Absyn_Datatype_d_struct*_tmp1B6=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp1A6;if(_tmp1B6->tag != 7)goto _LL176;}_LL175:{const char*_tmp54D;void*_tmp54C;(
_tmp54C=0,Cyc_fprintf(Cyc_stderr,((_tmp54D="Error: unexpected datatype declaration\n",
_tag_dyneither(_tmp54D,sizeof(char),40))),_tag_dyneither(_tmp54C,sizeof(void*),0)));}
exit(1);return 0;_LL176: {struct Cyc_Absyn_Letv_d_struct*_tmp1B7=(struct Cyc_Absyn_Letv_d_struct*)
_tmp1A6;if(_tmp1B7->tag != 3)goto _LL178;}_LL177:{const char*_tmp550;void*_tmp54F;(
_tmp54F=0,Cyc_fprintf(Cyc_stderr,((_tmp550="Error: unexpected let declaration\n",
_tag_dyneither(_tmp550,sizeof(char),35))),_tag_dyneither(_tmp54F,sizeof(void*),0)));}
exit(1);return 0;_LL178: {struct Cyc_Absyn_Namespace_d_struct*_tmp1B8=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp1A6;if(_tmp1B8->tag != 10)goto _LL17A;}_LL179:{const char*_tmp553;void*_tmp552;(
_tmp552=0,Cyc_fprintf(Cyc_stderr,((_tmp553="Error: unexpected namespace declaration\n",
_tag_dyneither(_tmp553,sizeof(char),41))),_tag_dyneither(_tmp552,sizeof(void*),0)));}
exit(1);return 0;_LL17A: {struct Cyc_Absyn_Using_d_struct*_tmp1B9=(struct Cyc_Absyn_Using_d_struct*)
_tmp1A6;if(_tmp1B9->tag != 11)goto _LL17C;}_LL17B:{const char*_tmp556;void*_tmp555;(
_tmp555=0,Cyc_fprintf(Cyc_stderr,((_tmp556="Error: unexpected using declaration\n",
_tag_dyneither(_tmp556,sizeof(char),37))),_tag_dyneither(_tmp555,sizeof(void*),0)));}
exit(1);return 0;_LL17C: {struct Cyc_Absyn_ExternC_d_struct*_tmp1BA=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp1A6;if(_tmp1BA->tag != 12)goto _LL17E;}_LL17D:{const char*_tmp559;void*_tmp558;(
_tmp558=0,Cyc_fprintf(Cyc_stderr,((_tmp559="Error: unexpected extern \"C\" declaration\n",
_tag_dyneither(_tmp559,sizeof(char),42))),_tag_dyneither(_tmp558,sizeof(void*),0)));}
exit(1);return 0;_LL17E: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp1BB=(struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp1A6;if(_tmp1BB->tag != 13)goto _LL15F;}
_LL17F:{const char*_tmp55C;void*_tmp55B;(_tmp55B=0,Cyc_fprintf(Cyc_stderr,((
_tmp55C="Error: unexpected extern \"C include\" declaration\n",_tag_dyneither(
_tmp55C,sizeof(char),50))),_tag_dyneither(_tmp55B,sizeof(void*),0)));}exit(1);
return 0;_LL15F:;}{struct _tuple17*_tmp55D;return(_tmp55D=_cycalloc(sizeof(*
_tmp55D)),((_tmp55D->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source),((
_tmp55D->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),_tmp55D)))));};}
struct Cyc_Hashtable_Table*Cyc_new_deps(){return((struct Cyc_Hashtable_Table*(*)(
int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct
_dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp1EE;_push_handler(& _tmp1EE);{int _tmp1F0=0;if(setjmp(
_tmp1EE.handler))_tmp1F0=1;if(!_tmp1F0){{struct Cyc_Set_Set*_tmp1F1=((struct Cyc_Set_Set*(*)(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);
_npop_handler(0);return _tmp1F1;};_pop_handler();}else{void*_tmp1EF=(void*)
_exn_thrown;void*_tmp1F3=_tmp1EF;_LL181: {struct Cyc_Core_Not_found_struct*
_tmp1F4=(struct Cyc_Core_Not_found_struct*)_tmp1F3;if(_tmp1F4->tag != Cyc_Core_Not_found)
goto _LL183;}_LL182: return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_LL183:;_LL184:(void)
_throw(_tmp1F3);_LL180:;}};}struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*
init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;for(curr=emptyset;init != 0;init=init->tl){curr=((struct
Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(
curr,(struct _dyneither_ptr*)init->hd);}{struct Cyc_Set_Set*_tmp1F5=curr;struct
_dyneither_ptr*_tmp1F6=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",
sizeof(char),1);while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1F5)
> 0){struct Cyc_Set_Set*_tmp1F7=emptyset;struct Cyc_Iter_Iter _tmp1F8=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,
_tmp1F5);while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(
_tmp1F8,& _tmp1F6)){_tmp1F7=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct
Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp1F7,Cyc_find(t,_tmp1F6));}_tmp1F5=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(
_tmp1F7,curr);curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(curr,_tmp1F5);}return curr;};}enum Cyc_buildlib_mode{Cyc_NORMAL
 = 0,Cyc_GATHER  = 1,Cyc_GATHERSCRIPT  = 2,Cyc_FINISH  = 3};static enum Cyc_buildlib_mode
Cyc_mode=Cyc_NORMAL;static int Cyc_gathering(){return Cyc_mode == Cyc_GATHER  || Cyc_mode
== Cyc_GATHERSCRIPT;}static struct Cyc___cycFILE*Cyc_script_file=0;int Cyc_prscript(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){if(Cyc_script_file == 0){{const
char*_tmp560;void*_tmp55F;(_tmp55F=0,Cyc_fprintf(Cyc_stderr,((_tmp560="Internal error: script file is NULL\n",
_tag_dyneither(_tmp560,sizeof(char),37))),_tag_dyneither(_tmp55F,sizeof(void*),0)));}
exit(1);}return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),
fmt,ap);}int Cyc_force_directory(struct _dyneither_ptr d){if(Cyc_mode == Cyc_GATHERSCRIPT){
const char*_tmp565;void*_tmp564[2];struct Cyc_String_pa_struct _tmp563;struct Cyc_String_pa_struct
_tmp562;(_tmp562.tag=0,((_tmp562.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
d),((_tmp563.tag=0,((_tmp563.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((
_tmp564[0]=& _tmp563,((_tmp564[1]=& _tmp562,Cyc_prscript(((_tmp565="if ! test -e %s; then mkdir %s; fi\n",
_tag_dyneither(_tmp565,sizeof(char),36))),_tag_dyneither(_tmp564,sizeof(void*),2)))))))))))));}
else{unsigned short _tmp566[0];int _tmp200=Cyc_open((const char*)_check_null(
_untag_dyneither_ptr(d,sizeof(char),1)),0,_tag_dyneither(_tmp566,sizeof(
unsigned short),0));if(_tmp200 == - 1){if(mkdir((const char*)_untag_dyneither_ptr(d,
sizeof(char),1),448)== - 1){{const char*_tmp56A;void*_tmp569[1];struct Cyc_String_pa_struct
_tmp568;(_tmp568.tag=0,((_tmp568.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
d),((_tmp569[0]=& _tmp568,Cyc_fprintf(Cyc_stderr,((_tmp56A="Error: could not create directory %s\n",
_tag_dyneither(_tmp56A,sizeof(char),38))),_tag_dyneither(_tmp569,sizeof(void*),1)))))));}
return 1;}}else{close(_tmp200);}}return 0;}int Cyc_force_directory_prefixes(struct
_dyneither_ptr file){struct _dyneither_ptr _tmp205=Cyc_strdup((struct _dyneither_ptr)
file);struct Cyc_List_List*_tmp206=0;while(1){_tmp205=Cyc_Filename_dirname((
struct _dyneither_ptr)_tmp205);if(Cyc_strlen((struct _dyneither_ptr)_tmp205)== 0)
break;{struct _dyneither_ptr*_tmp56D;struct Cyc_List_List*_tmp56C;_tmp206=((
_tmp56C=_cycalloc(sizeof(*_tmp56C)),((_tmp56C->hd=((_tmp56D=_cycalloc(sizeof(*
_tmp56D)),((_tmp56D[0]=(struct _dyneither_ptr)_tmp205,_tmp56D)))),((_tmp56C->tl=
_tmp206,_tmp56C))))));};}for(0;_tmp206 != 0;_tmp206=_tmp206->tl){if(Cyc_force_directory(*((
struct _dyneither_ptr*)_tmp206->hd)))return 1;}return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";
struct Cyc_NO_SUPPORT_struct{char*tag;struct _dyneither_ptr f1;};struct _tuple23{
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};int Cyc_process_file(const char*
filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,
struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*
cycstubs){struct Cyc___cycFILE*maybe;struct Cyc___cycFILE*in_file;struct Cyc___cycFILE*
out_file;int errorcode=0;{const char*_tmp573;void*_tmp572[1];const char*_tmp571;
struct Cyc_String_pa_struct _tmp570;(_tmp570.tag=0,((_tmp570.f1=(struct
_dyneither_ptr)((_tmp571=filename,_tag_dyneither(_tmp571,sizeof(char),
_get_zero_arr_size_char((void*)_tmp571,1)))),((_tmp572[0]=& _tmp570,Cyc_fprintf(
Cyc_stderr,((_tmp573="********************************* %s...\n",_tag_dyneither(
_tmp573,sizeof(char),41))),_tag_dyneither(_tmp572,sizeof(void*),1)))))));}if(!
Cyc_gathering()){const char*_tmp579;void*_tmp578[1];const char*_tmp577;struct Cyc_String_pa_struct
_tmp576;(_tmp576.tag=0,((_tmp576.f1=(struct _dyneither_ptr)((_tmp577=filename,
_tag_dyneither(_tmp577,sizeof(char),_get_zero_arr_size_char((void*)_tmp577,1)))),((
_tmp578[0]=& _tmp576,Cyc_log(((_tmp579="\n%s:\n",_tag_dyneither(_tmp579,sizeof(
char),6))),_tag_dyneither(_tmp578,sizeof(void*),1)))))));}{const char*_tmp57A;
struct _dyneither_ptr _tmp211=Cyc_Filename_basename(((_tmp57A=filename,
_tag_dyneither(_tmp57A,sizeof(char),_get_zero_arr_size_char((void*)_tmp57A,1)))));
const char*_tmp57B;struct _dyneither_ptr _tmp212=Cyc_Filename_dirname(((_tmp57B=
filename,_tag_dyneither(_tmp57B,sizeof(char),_get_zero_arr_size_char((void*)
_tmp57B,1)))));struct _dyneither_ptr _tmp213=Cyc_Filename_chop_extension((struct
_dyneither_ptr)_tmp211);const char*_tmp57C;const char*_tmp214=(const char*)
_untag_dyneither_ptr(Cyc_strconcat((struct _dyneither_ptr)_tmp213,((_tmp57C=".iA",
_tag_dyneither(_tmp57C,sizeof(char),4)))),sizeof(char),1);const char*_tmp584;void*
_tmp583[1];struct Cyc_String_pa_struct _tmp582;const char*_tmp580;void*_tmp57F[1];
struct Cyc_String_pa_struct _tmp57E;const char*_tmp215=(const char*)
_untag_dyneither_ptr(_get_dyneither_size(_tmp212,sizeof(char))== 0?(_tmp582.tag=
0,((_tmp582.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp213),((_tmp583[0]=&
_tmp582,Cyc_aprintf(((_tmp584="%s.iB",_tag_dyneither(_tmp584,sizeof(char),6))),
_tag_dyneither(_tmp583,sizeof(void*),1))))))): Cyc_Filename_concat((struct
_dyneither_ptr)_tmp212,(struct _dyneither_ptr)((_tmp57E.tag=0,((_tmp57E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp213),((_tmp57F[0]=& _tmp57E,Cyc_aprintf(((
_tmp580="%s.iB",_tag_dyneither(_tmp580,sizeof(char),6))),_tag_dyneither(_tmp57F,
sizeof(void*),1))))))))),sizeof(char),1);const char*_tmp58C;void*_tmp58B[1];
struct Cyc_String_pa_struct _tmp58A;const char*_tmp588;void*_tmp587[1];struct Cyc_String_pa_struct
_tmp586;const char*_tmp216=(const char*)_untag_dyneither_ptr(_get_dyneither_size(
_tmp212,sizeof(char))== 0?(_tmp58A.tag=0,((_tmp58A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp213),((_tmp58B[0]=& _tmp58A,Cyc_aprintf(((_tmp58C="%s.iC",
_tag_dyneither(_tmp58C,sizeof(char),6))),_tag_dyneither(_tmp58B,sizeof(void*),1))))))):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp212,(struct _dyneither_ptr)((
_tmp586.tag=0,((_tmp586.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp213),((
_tmp587[0]=& _tmp586,Cyc_aprintf(((_tmp588="%s.iC",_tag_dyneither(_tmp588,sizeof(
char),6))),_tag_dyneither(_tmp587,sizeof(void*),1))))))))),sizeof(char),1);const
char*_tmp594;void*_tmp593[1];struct Cyc_String_pa_struct _tmp592;const char*_tmp590;
void*_tmp58F[1];struct Cyc_String_pa_struct _tmp58E;const char*_tmp217=(const char*)
_untag_dyneither_ptr(_get_dyneither_size(_tmp212,sizeof(char))== 0?(_tmp592.tag=
0,((_tmp592.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp213),((_tmp593[0]=&
_tmp592,Cyc_aprintf(((_tmp594="%s.iD",_tag_dyneither(_tmp594,sizeof(char),6))),
_tag_dyneither(_tmp593,sizeof(void*),1))))))): Cyc_Filename_concat((struct
_dyneither_ptr)_tmp212,(struct _dyneither_ptr)((_tmp58E.tag=0,((_tmp58E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp213),((_tmp58F[0]=& _tmp58E,Cyc_aprintf(((
_tmp590="%s.iD",_tag_dyneither(_tmp590,sizeof(char),6))),_tag_dyneither(_tmp58F,
sizeof(void*),1))))))))),sizeof(char),1);struct _handler_cons _tmp218;
_push_handler(& _tmp218);{int _tmp21A=0;if(setjmp(_tmp218.handler))_tmp21A=1;if(!
_tmp21A){{const char*_tmp595;if(Cyc_force_directory_prefixes(((_tmp595=filename,
_tag_dyneither(_tmp595,sizeof(char),_get_zero_arr_size_char((void*)_tmp595,1)))))){
int _tmp21C=1;_npop_handler(0);return _tmp21C;}}if(Cyc_mode != Cyc_FINISH){Cyc_current_cpp=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);
if(Cyc_mode == Cyc_GATHERSCRIPT){{const char*_tmp59B;void*_tmp59A[1];const char*
_tmp599;struct Cyc_String_pa_struct _tmp598;(_tmp598.tag=0,((_tmp598.f1=(struct
_dyneither_ptr)((_tmp599=_tmp214,_tag_dyneither(_tmp599,sizeof(char),
_get_zero_arr_size_char((void*)_tmp599,1)))),((_tmp59A[0]=& _tmp598,Cyc_prscript(((
_tmp59B="cat >%s <<XXX\n",_tag_dyneither(_tmp59B,sizeof(char),15))),
_tag_dyneither(_tmp59A,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp221=Cyc_current_cpp;
for(0;_tmp221 != 0;_tmp221=_tmp221->tl){const char*_tmp59F;void*_tmp59E[1];struct
Cyc_String_pa_struct _tmp59D;(_tmp59D.tag=0,((_tmp59D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp221->hd)),((_tmp59E[0]=&
_tmp59D,Cyc_prscript(((_tmp59F="%s",_tag_dyneither(_tmp59F,sizeof(char),3))),
_tag_dyneither(_tmp59E,sizeof(void*),1)))))));}}{const char*_tmp5A5;void*_tmp5A4[
1];const char*_tmp5A3;struct Cyc_String_pa_struct _tmp5A2;(_tmp5A2.tag=0,((_tmp5A2.f1=(
struct _dyneither_ptr)((_tmp5A3=filename,_tag_dyneither(_tmp5A3,sizeof(char),
_get_zero_arr_size_char((void*)_tmp5A3,1)))),((_tmp5A4[0]=& _tmp5A2,Cyc_prscript(((
_tmp5A5="#include <%s>\n",_tag_dyneither(_tmp5A5,sizeof(char),15))),
_tag_dyneither(_tmp5A4,sizeof(void*),1)))))));}{const char*_tmp5A8;void*_tmp5A7;(
_tmp5A7=0,Cyc_prscript(((_tmp5A8="XXX\n",_tag_dyneither(_tmp5A8,sizeof(char),5))),
_tag_dyneither(_tmp5A7,sizeof(void*),0)));}{const char*_tmp5B1;void*_tmp5B0[2];
const char*_tmp5AF;struct Cyc_String_pa_struct _tmp5AE;const char*_tmp5AD;struct Cyc_String_pa_struct
_tmp5AC;(_tmp5AC.tag=0,((_tmp5AC.f1=(struct _dyneither_ptr)((_tmp5AD=_tmp214,
_tag_dyneither(_tmp5AD,sizeof(char),_get_zero_arr_size_char((void*)_tmp5AD,1)))),((
_tmp5AE.tag=0,((_tmp5AE.f1=(struct _dyneither_ptr)((_tmp5AF=_tmp215,
_tag_dyneither(_tmp5AF,sizeof(char),_get_zero_arr_size_char((void*)_tmp5AF,1)))),((
_tmp5B0[0]=& _tmp5AE,((_tmp5B0[1]=& _tmp5AC,Cyc_prscript(((_tmp5B1="$GCC -E -dM -o %s -x c %s && \\\n",
_tag_dyneither(_tmp5B1,sizeof(char),32))),_tag_dyneither(_tmp5B0,sizeof(void*),2)))))))))))));}{
const char*_tmp5BA;void*_tmp5B9[2];const char*_tmp5B8;struct Cyc_String_pa_struct
_tmp5B7;const char*_tmp5B6;struct Cyc_String_pa_struct _tmp5B5;(_tmp5B5.tag=0,((
_tmp5B5.f1=(struct _dyneither_ptr)((_tmp5B6=_tmp214,_tag_dyneither(_tmp5B6,
sizeof(char),_get_zero_arr_size_char((void*)_tmp5B6,1)))),((_tmp5B7.tag=0,((
_tmp5B7.f1=(struct _dyneither_ptr)((_tmp5B8=_tmp216,_tag_dyneither(_tmp5B8,
sizeof(char),_get_zero_arr_size_char((void*)_tmp5B8,1)))),((_tmp5B9[0]=& _tmp5B7,((
_tmp5B9[1]=& _tmp5B5,Cyc_prscript(((_tmp5BA="$GCC -E     -o %s -x c %s;\n",
_tag_dyneither(_tmp5BA,sizeof(char),28))),_tag_dyneither(_tmp5B9,sizeof(void*),2)))))))))))));}{
const char*_tmp5C0;void*_tmp5BF[1];const char*_tmp5BE;struct Cyc_String_pa_struct
_tmp5BD;(_tmp5BD.tag=0,((_tmp5BD.f1=(struct _dyneither_ptr)((_tmp5BE=_tmp214,
_tag_dyneither(_tmp5BE,sizeof(char),_get_zero_arr_size_char((void*)_tmp5BE,1)))),((
_tmp5BF[0]=& _tmp5BD,Cyc_prscript(((_tmp5C0="rm %s\n",_tag_dyneither(_tmp5C0,
sizeof(char),7))),_tag_dyneither(_tmp5BF,sizeof(void*),1)))))));};}else{maybe=
Cyc_fopen(_tmp214,(const char*)"w");if(!((unsigned int)maybe)){{const char*_tmp5C6;
void*_tmp5C5[1];const char*_tmp5C4;struct Cyc_String_pa_struct _tmp5C3;(_tmp5C3.tag=
0,((_tmp5C3.f1=(struct _dyneither_ptr)((_tmp5C4=_tmp214,_tag_dyneither(_tmp5C4,
sizeof(char),_get_zero_arr_size_char((void*)_tmp5C4,1)))),((_tmp5C5[0]=& _tmp5C3,
Cyc_fprintf(Cyc_stderr,((_tmp5C6="Error: could not create file %s\n",
_tag_dyneither(_tmp5C6,sizeof(char),33))),_tag_dyneither(_tmp5C5,sizeof(void*),1)))))));}{
int _tmp23F=1;_npop_handler(0);return _tmp23F;};}out_file=(struct Cyc___cycFILE*)
maybe;{struct Cyc_List_List*_tmp240=Cyc_current_cpp;for(0;_tmp240 != 0;_tmp240=
_tmp240->tl){Cyc_fputs((const char*)_untag_dyneither_ptr(*((struct _dyneither_ptr*)
_tmp240->hd),sizeof(char),1),out_file);}}{const char*_tmp5CC;void*_tmp5CB[1];
const char*_tmp5CA;struct Cyc_String_pa_struct _tmp5C9;(_tmp5C9.tag=0,((_tmp5C9.f1=(
struct _dyneither_ptr)((_tmp5CA=filename,_tag_dyneither(_tmp5CA,sizeof(char),
_get_zero_arr_size_char((void*)_tmp5CA,1)))),((_tmp5CB[0]=& _tmp5C9,Cyc_fprintf(
out_file,((_tmp5CC="#include <%s>\n",_tag_dyneither(_tmp5CC,sizeof(char),15))),
_tag_dyneither(_tmp5CB,sizeof(void*),1)))))));}Cyc_fclose(out_file);{struct
_dyneither_ptr _tmp245=Cstring_to_string(Ccomp);const char*_tmp5D6;void*_tmp5D5[3];
struct Cyc_String_pa_struct _tmp5D4;const char*_tmp5D3;struct Cyc_String_pa_struct
_tmp5D2;const char*_tmp5D1;struct Cyc_String_pa_struct _tmp5D0;char*cmd=(char*)
_untag_dyneither_ptr(((_tmp5D0.tag=0,((_tmp5D0.f1=(struct _dyneither_ptr)((
_tmp5D1=_tmp214,_tag_dyneither(_tmp5D1,sizeof(char),_get_zero_arr_size_char((
void*)_tmp5D1,1)))),((_tmp5D2.tag=0,((_tmp5D2.f1=(struct _dyneither_ptr)((_tmp5D3=
_tmp215,_tag_dyneither(_tmp5D3,sizeof(char),_get_zero_arr_size_char((void*)
_tmp5D3,1)))),((_tmp5D4.tag=0,((_tmp5D4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp245),((_tmp5D5[0]=& _tmp5D4,((_tmp5D5[1]=& _tmp5D2,((_tmp5D5[2]=&
_tmp5D0,Cyc_aprintf(((_tmp5D6="%s -E -dM -o %s -x c %s",_tag_dyneither(_tmp5D6,
sizeof(char),24))),_tag_dyneither(_tmp5D5,sizeof(void*),3)))))))))))))))))))),
sizeof(char),1);if(!system((const char*)cmd)){{const char*_tmp5E0;void*_tmp5DF[3];
struct Cyc_String_pa_struct _tmp5DE;const char*_tmp5DD;struct Cyc_String_pa_struct
_tmp5DC;const char*_tmp5DB;struct Cyc_String_pa_struct _tmp5DA;cmd=(char*)
_untag_dyneither_ptr(((_tmp5DA.tag=0,((_tmp5DA.f1=(struct _dyneither_ptr)((
_tmp5DB=_tmp214,_tag_dyneither(_tmp5DB,sizeof(char),_get_zero_arr_size_char((
void*)_tmp5DB,1)))),((_tmp5DC.tag=0,((_tmp5DC.f1=(struct _dyneither_ptr)((_tmp5DD=
_tmp216,_tag_dyneither(_tmp5DD,sizeof(char),_get_zero_arr_size_char((void*)
_tmp5DD,1)))),((_tmp5DE.tag=0,((_tmp5DE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp245),((_tmp5DF[0]=& _tmp5DE,((_tmp5DF[1]=& _tmp5DC,((_tmp5DF[2]=&
_tmp5DA,Cyc_aprintf(((_tmp5E0="%s -E -o %s -x c %s",_tag_dyneither(_tmp5E0,
sizeof(char),20))),_tag_dyneither(_tmp5DF,sizeof(void*),3)))))))))))))))))))),
sizeof(char),1);}system((const char*)cmd);}remove(_tmp214);};}}if(Cyc_gathering()){
int _tmp254=0;_npop_handler(0);return _tmp254;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp215,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp5F1;const char*_tmp5F0;void*_tmp5EF[1];const char*_tmp5EE;struct Cyc_String_pa_struct
_tmp5ED;struct Cyc_NO_SUPPORT_struct*_tmp5EC;(int)_throw((void*)((_tmp5EC=
_cycalloc(sizeof(*_tmp5EC)),((_tmp5EC[0]=((_tmp5F1.tag=Cyc_NO_SUPPORT,((_tmp5F1.f1=(
struct _dyneither_ptr)((_tmp5ED.tag=0,((_tmp5ED.f1=(struct _dyneither_ptr)((
_tmp5EE=_tmp215,_tag_dyneither(_tmp5EE,sizeof(char),_get_zero_arr_size_char((
void*)_tmp5EE,1)))),((_tmp5EF[0]=& _tmp5ED,Cyc_aprintf(((_tmp5F0="can't open macrosfile %s",
_tag_dyneither(_tmp5F0,sizeof(char),25))),_tag_dyneither(_tmp5EF,sizeof(void*),1)))))))),
_tmp5F1)))),_tmp5EC)))));}in_file=(struct Cyc___cycFILE*)maybe;{struct Cyc_Lexing_lexbuf*
_tmp25B=Cyc_Lexing_from_file(in_file);struct _tuple17*entry;while((entry=((struct
_tuple17*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp25B))!= 0){struct
_tuple17 _tmp25D;struct _dyneither_ptr*_tmp25E;struct Cyc_Set_Set*_tmp25F;struct
_tuple17*_tmp25C=(struct _tuple17*)_check_null(entry);_tmp25D=*_tmp25C;_tmp25E=
_tmp25D.f1;_tmp25F=_tmp25D.f2;((void(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp25E,
_tmp25F);}Cyc_fclose(in_file);maybe=Cyc_fopen(_tmp216,(const char*)"r");if(!((
unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct _tmp602;const char*_tmp601;void*
_tmp600[1];const char*_tmp5FF;struct Cyc_String_pa_struct _tmp5FE;struct Cyc_NO_SUPPORT_struct*
_tmp5FD;(int)_throw((void*)((_tmp5FD=_cycalloc(sizeof(*_tmp5FD)),((_tmp5FD[0]=((
_tmp602.tag=Cyc_NO_SUPPORT,((_tmp602.f1=(struct _dyneither_ptr)((_tmp5FE.tag=0,((
_tmp5FE.f1=(struct _dyneither_ptr)((_tmp5FF=_tmp216,_tag_dyneither(_tmp5FF,
sizeof(char),_get_zero_arr_size_char((void*)_tmp5FF,1)))),((_tmp600[0]=& _tmp5FE,
Cyc_aprintf(((_tmp601="can't open declsfile %s",_tag_dyneither(_tmp601,sizeof(
char),24))),_tag_dyneither(_tmp600,sizeof(void*),1)))))))),_tmp602)))),_tmp5FD)))));}
in_file=(struct Cyc___cycFILE*)maybe;_tmp25B=Cyc_Lexing_from_file(in_file);Cyc_slurp_out=
Cyc_fopen(_tmp217,(const char*)"w");if(!((unsigned int)Cyc_slurp_out)){int _tmp266=
1;_npop_handler(0);return _tmp266;}while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))
Cyc_slurp)(_tmp25B)){;}Cyc_fclose(in_file);Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));if(Cyc_mode != Cyc_FINISH)remove(_tmp216);maybe=Cyc_fopen(
_tmp217,(const char*)"r");if(!((unsigned int)maybe)){int _tmp267=1;_npop_handler(0);
return _tmp267;}in_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp603;Cyc_Position_reset_position(((
_tmp603=_tmp217,_tag_dyneither(_tmp603,sizeof(char),_get_zero_arr_size_char((
void*)_tmp603,1)))));}Cyc_Lex_lex_init(0);{struct Cyc_List_List*_tmp269=Cyc_Parse_parse_file(
in_file);Cyc_Lex_lex_init(0);Cyc_fclose(in_file);remove(_tmp217);{struct Cyc_List_List*
_tmp26A=_tmp269;for(0;_tmp26A != 0;_tmp26A=_tmp26A->tl){struct _tuple17*_tmp26B=
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp26A->hd);if(_tmp26B == 0)continue;{
struct _tuple17 _tmp26D;struct _dyneither_ptr*_tmp26E;struct Cyc_Set_Set*_tmp26F;
struct _tuple17*_tmp26C=(struct _tuple17*)_tmp26B;_tmp26D=*_tmp26C;_tmp26E=_tmp26D.f1;
_tmp26F=_tmp26D.f2;{struct Cyc_Set_Set*old;{struct _handler_cons _tmp270;
_push_handler(& _tmp270);{int _tmp272=0;if(setjmp(_tmp270.handler))_tmp272=1;if(!
_tmp272){old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key))Cyc_Hashtable_lookup)(t,_tmp26E);;_pop_handler();}else{void*
_tmp271=(void*)_exn_thrown;void*_tmp274=_tmp271;_LL186: {struct Cyc_Core_Not_found_struct*
_tmp275=(struct Cyc_Core_Not_found_struct*)_tmp274;if(_tmp275->tag != Cyc_Core_Not_found)
goto _LL188;}_LL187: old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL185;_LL188:;_LL189:(
void)_throw(_tmp274);_LL185:;}};}((void(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp26E,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp26F,old));};};}}{struct Cyc_Set_Set*_tmp276=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp277=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);{
struct Cyc_List_List*_tmp278=_tmp269;for(0;_tmp278 != 0;_tmp278=_tmp278->tl){
struct Cyc_Absyn_Decl*_tmp279=(struct Cyc_Absyn_Decl*)_tmp278->hd;struct
_dyneither_ptr*name;{void*_tmp27A=_tmp279->r;struct Cyc_Absyn_Vardecl*_tmp27C;
struct Cyc_Absyn_Fndecl*_tmp27E;struct Cyc_Absyn_Aggrdecl*_tmp280;struct Cyc_Absyn_Enumdecl*
_tmp282;struct Cyc_Absyn_Typedefdecl*_tmp284;_LL18B: {struct Cyc_Absyn_Var_d_struct*
_tmp27B=(struct Cyc_Absyn_Var_d_struct*)_tmp27A;if(_tmp27B->tag != 0)goto _LL18D;
else{_tmp27C=_tmp27B->f1;}}_LL18C: {struct _tuple0 _tmp291;struct _dyneither_ptr*
_tmp292;struct _tuple0*_tmp290=_tmp27C->name;_tmp291=*_tmp290;_tmp292=_tmp291.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_insert)(defined_symbols,_tmp292);if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp292))name=0;else{
name=(struct _dyneither_ptr*)_tmp292;}goto _LL18A;}_LL18D: {struct Cyc_Absyn_Fn_d_struct*
_tmp27D=(struct Cyc_Absyn_Fn_d_struct*)_tmp27A;if(_tmp27D->tag != 1)goto _LL18F;
else{_tmp27E=_tmp27D->f1;}}_LL18E: {struct _tuple0 _tmp294;struct _dyneither_ptr*
_tmp295;struct _tuple0*_tmp293=_tmp27E->name;_tmp294=*_tmp293;_tmp295=_tmp294.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_insert)(defined_symbols,_tmp295);if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp295))name=0;else{
name=(struct _dyneither_ptr*)_tmp295;}goto _LL18A;}_LL18F: {struct Cyc_Absyn_Aggr_d_struct*
_tmp27F=(struct Cyc_Absyn_Aggr_d_struct*)_tmp27A;if(_tmp27F->tag != 6)goto _LL191;
else{_tmp280=_tmp27F->f1;}}_LL190: {struct _tuple0 _tmp297;struct _dyneither_ptr*
_tmp298;struct _tuple0*_tmp296=_tmp280->name;_tmp297=*_tmp296;_tmp298=_tmp297.f2;
name=(struct _dyneither_ptr*)_tmp298;goto _LL18A;}_LL191: {struct Cyc_Absyn_Enum_d_struct*
_tmp281=(struct Cyc_Absyn_Enum_d_struct*)_tmp27A;if(_tmp281->tag != 8)goto _LL193;
else{_tmp282=_tmp281->f1;}}_LL192: {struct _tuple0 _tmp29A;struct _dyneither_ptr*
_tmp29B;struct _tuple0*_tmp299=_tmp282->name;_tmp29A=*_tmp299;_tmp29B=_tmp29A.f2;
name=(struct _dyneither_ptr*)_tmp29B;if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp276,(struct _dyneither_ptr*)name)){
struct Cyc_List_List*_tmp604;_tmp277=((_tmp604=_cycalloc(sizeof(*_tmp604)),((
_tmp604->hd=_tmp279,((_tmp604->tl=_tmp277,_tmp604))))));}else{if((unsigned int)
_tmp282->fields){struct Cyc_List_List*_tmp29D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp282->fields))->v;for(0;_tmp29D != 0;_tmp29D=_tmp29D->tl){struct
Cyc_Absyn_Enumfield*_tmp29E=(struct Cyc_Absyn_Enumfield*)_tmp29D->hd;struct
_tuple0 _tmp2A0;struct _dyneither_ptr*_tmp2A1;struct _tuple0*_tmp29F=_tmp29E->name;
_tmp2A0=*_tmp29F;_tmp2A1=_tmp2A0.f2;if(((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(_tmp276,_tmp2A1)){{struct Cyc_List_List*
_tmp605;_tmp277=((_tmp605=_cycalloc(sizeof(*_tmp605)),((_tmp605->hd=_tmp279,((
_tmp605->tl=_tmp277,_tmp605))))));}break;}}}}name=0;goto _LL18A;}_LL193: {struct
Cyc_Absyn_Typedef_d_struct*_tmp283=(struct Cyc_Absyn_Typedef_d_struct*)_tmp27A;
if(_tmp283->tag != 9)goto _LL195;else{_tmp284=_tmp283->f1;}}_LL194: {struct _tuple0
_tmp2A4;struct _dyneither_ptr*_tmp2A5;struct _tuple0*_tmp2A3=_tmp284->name;_tmp2A4=*
_tmp2A3;_tmp2A5=_tmp2A4.f2;name=(struct _dyneither_ptr*)_tmp2A5;goto _LL18A;}
_LL195: {struct Cyc_Absyn_Porton_d_struct*_tmp285=(struct Cyc_Absyn_Porton_d_struct*)
_tmp27A;if(_tmp285->tag != 14)goto _LL197;}_LL196: goto _LL198;_LL197: {struct Cyc_Absyn_Portoff_d_struct*
_tmp286=(struct Cyc_Absyn_Portoff_d_struct*)_tmp27A;if(_tmp286->tag != 15)goto
_LL199;}_LL198: goto _LL19A;_LL199: {struct Cyc_Absyn_Let_d_struct*_tmp287=(struct
Cyc_Absyn_Let_d_struct*)_tmp27A;if(_tmp287->tag != 2)goto _LL19B;}_LL19A: goto
_LL19C;_LL19B: {struct Cyc_Absyn_Datatype_d_struct*_tmp288=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp27A;if(_tmp288->tag != 7)goto _LL19D;}_LL19C: goto _LL19E;_LL19D: {struct Cyc_Absyn_Letv_d_struct*
_tmp289=(struct Cyc_Absyn_Letv_d_struct*)_tmp27A;if(_tmp289->tag != 3)goto _LL19F;}
_LL19E: goto _LL1A0;_LL19F: {struct Cyc_Absyn_Namespace_d_struct*_tmp28A=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp27A;if(_tmp28A->tag != 10)goto _LL1A1;}_LL1A0: goto _LL1A2;_LL1A1: {struct Cyc_Absyn_Using_d_struct*
_tmp28B=(struct Cyc_Absyn_Using_d_struct*)_tmp27A;if(_tmp28B->tag != 11)goto _LL1A3;}
_LL1A2: goto _LL1A4;_LL1A3: {struct Cyc_Absyn_ExternC_d_struct*_tmp28C=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp27A;if(_tmp28C->tag != 12)goto _LL1A5;}_LL1A4: goto _LL1A6;_LL1A5: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp28D=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp27A;if(_tmp28D->tag != 13)
goto _LL1A7;}_LL1A6: goto _LL1A8;_LL1A7: {struct Cyc_Absyn_Region_d_struct*_tmp28E=(
struct Cyc_Absyn_Region_d_struct*)_tmp27A;if(_tmp28E->tag != 4)goto _LL1A9;}_LL1A8:
goto _LL1AA;_LL1A9: {struct Cyc_Absyn_Alias_d_struct*_tmp28F=(struct Cyc_Absyn_Alias_d_struct*)
_tmp27A;if(_tmp28F->tag != 5)goto _LL18A;}_LL1AA: name=0;goto _LL18A;_LL18A:;}if(
name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
_tmp276,(struct _dyneither_ptr*)name)){struct Cyc_List_List*_tmp606;_tmp277=((
_tmp606=_cycalloc(sizeof(*_tmp606)),((_tmp606->hd=_tmp279,((_tmp606->tl=_tmp277,
_tmp606))))));}}}if(!Cyc_do_setjmp){maybe=Cyc_fopen(filename,(const char*)"w");
if(!((unsigned int)maybe)){int _tmp2A7=1;_npop_handler(0);return _tmp2A7;}out_file=(
struct Cyc___cycFILE*)maybe;}else{out_file=Cyc_stdout;}{const char*_tmp60C;void*
_tmp60B[1];const char*_tmp60A;struct Cyc_String_pa_struct _tmp609;struct
_dyneither_ptr ifdefmacro=(_tmp609.tag=0,((_tmp609.f1=(struct _dyneither_ptr)((
_tmp60A=filename,_tag_dyneither(_tmp60A,sizeof(char),_get_zero_arr_size_char((
void*)_tmp60A,1)))),((_tmp60B[0]=& _tmp609,Cyc_aprintf(((_tmp60C="_%s_",
_tag_dyneither(_tmp60C,sizeof(char),5))),_tag_dyneither(_tmp60B,sizeof(void*),1)))))));{
int _tmp2A8=0;for(0;_tmp2A8 < _get_dyneither_size(ifdefmacro,sizeof(char));++
_tmp2A8){if(((char*)ifdefmacro.curr)[_tmp2A8]== '.'  || ((char*)ifdefmacro.curr)[
_tmp2A8]== '/'){char _tmp60F;char _tmp60E;struct _dyneither_ptr _tmp60D;(_tmp60D=
_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp2A8),((_tmp60E=*((char*)
_check_dyneither_subscript(_tmp60D,sizeof(char),0)),((_tmp60F='_',((
_get_dyneither_size(_tmp60D,sizeof(char))== 1  && (_tmp60E == '\000'  && _tmp60F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp60D.curr)=_tmp60F)))))));}else{if(((char*)
ifdefmacro.curr)[_tmp2A8]!= '_'  && ((char*)ifdefmacro.curr)[_tmp2A8]!= '/'){char
_tmp612;char _tmp611;struct _dyneither_ptr _tmp610;(_tmp610=_dyneither_ptr_plus(
ifdefmacro,sizeof(char),_tmp2A8),((_tmp611=*((char*)_check_dyneither_subscript(
_tmp610,sizeof(char),0)),((_tmp612=(char)toupper((int)((char*)ifdefmacro.curr)[
_tmp2A8]),((_get_dyneither_size(_tmp610,sizeof(char))== 1  && (_tmp611 == '\000'
 && _tmp612 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp610.curr)=_tmp612)))))));}}}}{
const char*_tmp617;void*_tmp616[2];struct Cyc_String_pa_struct _tmp615;struct Cyc_String_pa_struct
_tmp614;(_tmp614.tag=0,((_tmp614.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
ifdefmacro),((_tmp615.tag=0,((_tmp615.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ifdefmacro),((_tmp616[0]=& _tmp615,((_tmp616[1]=& _tmp614,Cyc_fprintf(
out_file,((_tmp617="#ifndef %s\n#define %s\n",_tag_dyneither(_tmp617,sizeof(char),
23))),_tag_dyneither(_tmp616,sizeof(void*),2)))))))))))));}{struct Cyc_List_List*
_tmp2B3=0;struct Cyc_List_List*_tmp2B4=0;{struct Cyc_List_List*_tmp2B5=_tmp277;
for(0;_tmp2B5 != 0;_tmp2B5=_tmp2B5->tl){struct Cyc_Absyn_Decl*_tmp2B6=(struct Cyc_Absyn_Decl*)
_tmp2B5->hd;int _tmp2B7=0;struct _dyneither_ptr*name;{void*_tmp2B8=_tmp2B6->r;
struct Cyc_Absyn_Vardecl*_tmp2BA;struct Cyc_Absyn_Fndecl*_tmp2BC;struct Cyc_Absyn_Aggrdecl*
_tmp2BE;struct Cyc_Absyn_Enumdecl*_tmp2C0;struct Cyc_Absyn_Typedefdecl*_tmp2C2;
_LL1AC: {struct Cyc_Absyn_Var_d_struct*_tmp2B9=(struct Cyc_Absyn_Var_d_struct*)
_tmp2B8;if(_tmp2B9->tag != 0)goto _LL1AE;else{_tmp2BA=_tmp2B9->f1;}}_LL1AD: {
struct _tuple0 _tmp2CF;struct _dyneither_ptr*_tmp2D0;struct _tuple0*_tmp2CE=_tmp2BA->name;
_tmp2CF=*_tmp2CE;_tmp2D0=_tmp2CF.f2;name=(struct _dyneither_ptr*)_tmp2D0;goto
_LL1AB;}_LL1AE: {struct Cyc_Absyn_Fn_d_struct*_tmp2BB=(struct Cyc_Absyn_Fn_d_struct*)
_tmp2B8;if(_tmp2BB->tag != 1)goto _LL1B0;else{_tmp2BC=_tmp2BB->f1;}}_LL1AF: {
struct _tuple0 _tmp2D2;struct _dyneither_ptr*_tmp2D3;struct _tuple0*_tmp2D1=_tmp2BC->name;
_tmp2D2=*_tmp2D1;_tmp2D3=_tmp2D2.f2;name=(struct _dyneither_ptr*)_tmp2D3;goto
_LL1AB;}_LL1B0: {struct Cyc_Absyn_Aggr_d_struct*_tmp2BD=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp2B8;if(_tmp2BD->tag != 6)goto _LL1B2;else{_tmp2BE=_tmp2BD->f1;}}_LL1B1: {
struct _tuple0 _tmp2D5;struct _dyneither_ptr*_tmp2D6;struct _tuple0*_tmp2D4=_tmp2BE->name;
_tmp2D5=*_tmp2D4;_tmp2D6=_tmp2D5.f2;name=(struct _dyneither_ptr*)_tmp2D6;goto
_LL1AB;}_LL1B2: {struct Cyc_Absyn_Enum_d_struct*_tmp2BF=(struct Cyc_Absyn_Enum_d_struct*)
_tmp2B8;if(_tmp2BF->tag != 8)goto _LL1B4;else{_tmp2C0=_tmp2BF->f1;}}_LL1B3: {
struct _tuple0 _tmp2D8;struct _dyneither_ptr*_tmp2D9;struct _tuple0*_tmp2D7=_tmp2C0->name;
_tmp2D8=*_tmp2D7;_tmp2D9=_tmp2D8.f2;name=(struct _dyneither_ptr*)_tmp2D9;goto
_LL1AB;}_LL1B4: {struct Cyc_Absyn_Typedef_d_struct*_tmp2C1=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp2B8;if(_tmp2C1->tag != 9)goto _LL1B6;else{_tmp2C2=_tmp2C1->f1;}}_LL1B5: {
struct _tuple0 _tmp2DB;struct _dyneither_ptr*_tmp2DC;struct _tuple0*_tmp2DA=_tmp2C2->name;
_tmp2DB=*_tmp2DA;_tmp2DC=_tmp2DB.f2;name=(struct _dyneither_ptr*)_tmp2DC;goto
_LL1AB;}_LL1B6: {struct Cyc_Absyn_Region_d_struct*_tmp2C3=(struct Cyc_Absyn_Region_d_struct*)
_tmp2B8;if(_tmp2C3->tag != 4)goto _LL1B8;}_LL1B7: goto _LL1B9;_LL1B8: {struct Cyc_Absyn_Alias_d_struct*
_tmp2C4=(struct Cyc_Absyn_Alias_d_struct*)_tmp2B8;if(_tmp2C4->tag != 5)goto _LL1BA;}
_LL1B9: goto _LL1BB;_LL1BA: {struct Cyc_Absyn_Porton_d_struct*_tmp2C5=(struct Cyc_Absyn_Porton_d_struct*)
_tmp2B8;if(_tmp2C5->tag != 14)goto _LL1BC;}_LL1BB: goto _LL1BD;_LL1BC: {struct Cyc_Absyn_Portoff_d_struct*
_tmp2C6=(struct Cyc_Absyn_Portoff_d_struct*)_tmp2B8;if(_tmp2C6->tag != 15)goto
_LL1BE;}_LL1BD: goto _LL1BF;_LL1BE: {struct Cyc_Absyn_Let_d_struct*_tmp2C7=(struct
Cyc_Absyn_Let_d_struct*)_tmp2B8;if(_tmp2C7->tag != 2)goto _LL1C0;}_LL1BF: goto
_LL1C1;_LL1C0: {struct Cyc_Absyn_Datatype_d_struct*_tmp2C8=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp2B8;if(_tmp2C8->tag != 7)goto _LL1C2;}_LL1C1: goto _LL1C3;_LL1C2: {struct Cyc_Absyn_Letv_d_struct*
_tmp2C9=(struct Cyc_Absyn_Letv_d_struct*)_tmp2B8;if(_tmp2C9->tag != 3)goto _LL1C4;}
_LL1C3: goto _LL1C5;_LL1C4: {struct Cyc_Absyn_Namespace_d_struct*_tmp2CA=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp2B8;if(_tmp2CA->tag != 10)goto _LL1C6;}_LL1C5: goto _LL1C7;_LL1C6: {struct Cyc_Absyn_Using_d_struct*
_tmp2CB=(struct Cyc_Absyn_Using_d_struct*)_tmp2B8;if(_tmp2CB->tag != 11)goto _LL1C8;}
_LL1C7: goto _LL1C9;_LL1C8: {struct Cyc_Absyn_ExternC_d_struct*_tmp2CC=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp2B8;if(_tmp2CC->tag != 12)goto _LL1CA;}_LL1C9: goto _LL1CB;_LL1CA: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp2CD=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp2B8;if(_tmp2CD->tag != 13)
goto _LL1AB;}_LL1CB: name=0;goto _LL1AB;_LL1AB:;}if(!((unsigned int)name) && !
_tmp2B7)continue;{struct Cyc_List_List*_tmp618;_tmp2B3=((_tmp618=_cycalloc(
sizeof(*_tmp618)),((_tmp618->hd=_tmp2B6,((_tmp618->tl=_tmp2B3,_tmp618))))));}{
struct Cyc_List_List*_tmp619;_tmp2B4=((_tmp619=_cycalloc(sizeof(*_tmp619)),((
_tmp619->hd=name,((_tmp619->tl=_tmp2B4,_tmp619))))));};}}{struct _RegionHandle
_tmp2DF=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp2DF;_push_region(
tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp2E0=Cyc_Tcenv_tc_init(tc_rgn);Cyc_Tc_tc(tc_rgn,
_tmp2E0,1,_tmp2B3);}{struct Cyc_List_List*_tmp2E3;struct Cyc_List_List*_tmp2E4;
struct _tuple23 _tmp61A;struct _tuple23 _tmp2E2=(_tmp61A.f1=_tmp2B3,((_tmp61A.f2=
_tmp2B4,_tmp61A)));_tmp2E3=_tmp2E2.f1;_tmp2E4=_tmp2E2.f2;for(0;_tmp2E3 != 0  && 
_tmp2E4 != 0;(_tmp2E3=_tmp2E3->tl,_tmp2E4=_tmp2E4->tl)){struct Cyc_Absyn_Decl*
_tmp2E5=(struct Cyc_Absyn_Decl*)_tmp2E3->hd;struct _dyneither_ptr*_tmp2E6=(struct
_dyneither_ptr*)_tmp2E4->hd;int _tmp2E7=0;if(!((unsigned int)_tmp2E6))_tmp2E7=1;
if((unsigned int)_tmp2E6){{const char*_tmp61E;void*_tmp61D[1];struct Cyc_String_pa_struct
_tmp61C;ifdefmacro=((_tmp61C.tag=0,((_tmp61C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp2E6),((_tmp61D[0]=& _tmp61C,Cyc_aprintf(((_tmp61E="_%s_def_",
_tag_dyneither(_tmp61E,sizeof(char),9))),_tag_dyneither(_tmp61D,sizeof(void*),1))))))));}{
const char*_tmp622;void*_tmp621[1];struct Cyc_String_pa_struct _tmp620;(_tmp620.tag=
0,((_tmp620.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((
_tmp621[0]=& _tmp620,Cyc_fprintf(out_file,((_tmp622="#ifndef %s\n",_tag_dyneither(
_tmp622,sizeof(char),12))),_tag_dyneither(_tmp621,sizeof(void*),1)))))));}{const
char*_tmp626;void*_tmp625[1];struct Cyc_String_pa_struct _tmp624;(_tmp624.tag=0,((
_tmp624.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp625[0]=&
_tmp624,Cyc_fprintf(out_file,((_tmp626="#define %s\n",_tag_dyneither(_tmp626,
sizeof(char),12))),_tag_dyneither(_tmp625,sizeof(void*),1)))))));}{struct Cyc_Absyn_Decl*
_tmp627[1];Cyc_Absynpp_decllist2file(((_tmp627[0]=_tmp2E5,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp627,sizeof(struct Cyc_Absyn_Decl*),
1)))),out_file);}{const char*_tmp62A;void*_tmp629;(_tmp629=0,Cyc_fprintf(out_file,((
_tmp62A="#endif\n",_tag_dyneither(_tmp62A,sizeof(char),8))),_tag_dyneither(
_tmp629,sizeof(void*),0)));};}else{struct Cyc_Absyn_Decl*_tmp62B[1];Cyc_Absynpp_decllist2file(((
_tmp62B[0]=_tmp2E5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp62B,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}}}maybe=
Cyc_fopen(_tmp215,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp63C;const char*_tmp63B;void*_tmp63A[1];const char*_tmp639;struct Cyc_String_pa_struct
_tmp638;struct Cyc_NO_SUPPORT_struct*_tmp637;(int)_throw((void*)((_tmp637=
_cycalloc(sizeof(*_tmp637)),((_tmp637[0]=((_tmp63C.tag=Cyc_NO_SUPPORT,((_tmp63C.f1=(
struct _dyneither_ptr)((_tmp638.tag=0,((_tmp638.f1=(struct _dyneither_ptr)((
_tmp639=_tmp215,_tag_dyneither(_tmp639,sizeof(char),_get_zero_arr_size_char((
void*)_tmp639,1)))),((_tmp63A[0]=& _tmp638,Cyc_aprintf(((_tmp63B="can't open macrosfile %s",
_tag_dyneither(_tmp63B,sizeof(char),25))),_tag_dyneither(_tmp63A,sizeof(void*),1)))))))),
_tmp63C)))),_tmp637)))));}in_file=(struct Cyc___cycFILE*)maybe;_tmp25B=Cyc_Lexing_from_file(
in_file);{struct _tuple18*entry2;while((entry2=((struct _tuple18*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_suck_line)(_tmp25B))!= 0){struct _tuple18 _tmp2FC;struct _dyneither_ptr
_tmp2FD;struct _dyneither_ptr*_tmp2FE;struct _tuple18*_tmp2FB=(struct _tuple18*)
_check_null(entry2);_tmp2FC=*_tmp2FB;_tmp2FD=_tmp2FC.f1;_tmp2FE=_tmp2FC.f2;if(((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp276,
_tmp2FE)){{const char*_tmp640;void*_tmp63F[1];struct Cyc_String_pa_struct _tmp63E;(
_tmp63E.tag=0,((_tmp63E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2FE),((
_tmp63F[0]=& _tmp63E,Cyc_fprintf(out_file,((_tmp640="#ifndef %s\n",_tag_dyneither(
_tmp640,sizeof(char),12))),_tag_dyneither(_tmp63F,sizeof(void*),1)))))));}{const
char*_tmp644;void*_tmp643[1];struct Cyc_String_pa_struct _tmp642;(_tmp642.tag=0,((
_tmp642.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2FD),((_tmp643[0]=&
_tmp642,Cyc_fprintf(out_file,((_tmp644="%s\n",_tag_dyneither(_tmp644,sizeof(char),
4))),_tag_dyneither(_tmp643,sizeof(void*),1)))))));}{const char*_tmp647;void*
_tmp646;(_tmp646=0,Cyc_fprintf(out_file,((_tmp647="#endif\n",_tag_dyneither(
_tmp647,sizeof(char),8))),_tag_dyneither(_tmp646,sizeof(void*),0)));};}}Cyc_fclose(
in_file);if(Cyc_mode != Cyc_FINISH)remove(_tmp215);if(hstubs != 0){struct Cyc_List_List*
_tmp307=hstubs;for(0;_tmp307 != 0;_tmp307=_tmp307->tl){struct _tuple19 _tmp309;
struct _dyneither_ptr _tmp30A;struct _dyneither_ptr _tmp30B;struct _tuple19*_tmp308=(
struct _tuple19*)_tmp307->hd;_tmp309=*_tmp308;_tmp30A=_tmp309.f1;_tmp30B=_tmp309.f2;{
struct _dyneither_ptr*_tmp648;if((char*)_tmp30B.curr != (char*)(_tag_dyneither(0,0,
0)).curr  && ((char*)_tmp30A.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((
_tmp648=_cycalloc(sizeof(*_tmp648)),((_tmp648[0]=_tmp30A,_tmp648)))))))Cyc_fputs((
const char*)_untag_dyneither_ptr(_tmp30B,sizeof(char),1),out_file);else{const char*
_tmp64C;void*_tmp64B[1];struct Cyc_String_pa_struct _tmp64A;(_tmp64A.tag=0,((
_tmp64A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp30A),((_tmp64B[0]=&
_tmp64A,Cyc_log(((_tmp64C="%s is not supported on this platform\n",
_tag_dyneither(_tmp64C,sizeof(char),38))),_tag_dyneither(_tmp64B,sizeof(void*),1)))))));}};}}{
const char*_tmp64F;void*_tmp64E;(_tmp64E=0,Cyc_fprintf(out_file,((_tmp64F="#endif\n",
_tag_dyneither(_tmp64F,sizeof(char),8))),_tag_dyneither(_tmp64E,sizeof(void*),0)));}
if(Cyc_do_setjmp){int _tmp312=0;_npop_handler(1);return _tmp312;}else{Cyc_fclose(
out_file);}if(cstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp313=cstubs;for(0;_tmp313 != 0;_tmp313=_tmp313->tl){struct
_tuple19 _tmp315;struct _dyneither_ptr _tmp316;struct _dyneither_ptr _tmp317;struct
_tuple19*_tmp314=(struct _tuple19*)_tmp313->hd;_tmp315=*_tmp314;_tmp316=_tmp315.f1;
_tmp317=_tmp315.f2;{struct _dyneither_ptr*_tmp650;if((char*)_tmp317.curr != (char*)(
_tag_dyneither(0,0,0)).curr  && ((char*)_tmp316.curr == (char*)(_tag_dyneither(0,0,
0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
defined_symbols,((_tmp650=_cycalloc(sizeof(*_tmp650)),((_tmp650[0]=_tmp316,
_tmp650)))))))Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp317,sizeof(char),1),
out_file);};}};}out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
const char*_tmp656;void*_tmp655[1];const char*_tmp654;struct Cyc_String_pa_struct
_tmp653;(_tmp653.tag=0,((_tmp653.f1=(struct _dyneither_ptr)((_tmp654=filename,
_tag_dyneither(_tmp654,sizeof(char),_get_zero_arr_size_char((void*)_tmp654,1)))),((
_tmp655[0]=& _tmp653,Cyc_fprintf(out_file,((_tmp656="#include <%s>\n\n",
_tag_dyneither(_tmp656,sizeof(char),16))),_tag_dyneither(_tmp655,sizeof(void*),1)))))));}
if(cycstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
struct Cyc_List_List*_tmp31D=cycstubs;for(0;_tmp31D != 0;_tmp31D=_tmp31D->tl){
struct _tuple19 _tmp31F;struct _dyneither_ptr _tmp320;struct _dyneither_ptr _tmp321;
struct _tuple19*_tmp31E=(struct _tuple19*)_tmp31D->hd;_tmp31F=*_tmp31E;_tmp320=
_tmp31F.f1;_tmp321=_tmp31F.f2;{struct _dyneither_ptr*_tmp657;if((char*)_tmp321.curr
!= (char*)(_tag_dyneither(0,0,0)).curr  && ((char*)_tmp320.curr == (char*)(
_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_member)(defined_symbols,((_tmp657=_cycalloc(sizeof(*_tmp657)),((
_tmp657[0]=_tmp320,_tmp657)))))))Cyc_fputs((const char*)_untag_dyneither_ptr(
_tmp321,sizeof(char),1),out_file);};}}{const char*_tmp65A;void*_tmp659;(_tmp659=0,
Cyc_fprintf(out_file,((_tmp65A="\n",_tag_dyneither(_tmp65A,sizeof(char),2))),
_tag_dyneither(_tmp659,sizeof(void*),0)));};}{int _tmp325=0;_npop_handler(1);
return _tmp325;};};;_pop_region(tc_rgn);};};};};};};};;_pop_handler();}else{void*
_tmp219=(void*)_exn_thrown;void*_tmp32B=_tmp219;struct _dyneither_ptr _tmp32D;
_LL1CD: {struct Cyc_NO_SUPPORT_struct*_tmp32C=(struct Cyc_NO_SUPPORT_struct*)
_tmp32B;if(_tmp32C->tag != Cyc_NO_SUPPORT)goto _LL1CF;else{_tmp32D=_tmp32C->f1;}}
_LL1CE:{const char*_tmp65E;void*_tmp65D[1];struct Cyc_String_pa_struct _tmp65C;(
_tmp65C.tag=0,((_tmp65C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32D),((
_tmp65D[0]=& _tmp65C,Cyc_fprintf(Cyc_stderr,((_tmp65E="No support because %s\n",
_tag_dyneither(_tmp65E,sizeof(char),23))),_tag_dyneither(_tmp65D,sizeof(void*),1)))))));}
goto _LL1D0;_LL1CF:;_LL1D0: maybe=Cyc_fopen(filename,(const char*)"w");if(!((
unsigned int)maybe)){{const char*_tmp664;void*_tmp663[1];const char*_tmp662;struct
Cyc_String_pa_struct _tmp661;(_tmp661.tag=0,((_tmp661.f1=(struct _dyneither_ptr)((
_tmp662=filename,_tag_dyneither(_tmp662,sizeof(char),_get_zero_arr_size_char((
void*)_tmp662,1)))),((_tmp663[0]=& _tmp661,Cyc_fprintf(Cyc_stderr,((_tmp664="Error: could not create file %s\n",
_tag_dyneither(_tmp664,sizeof(char),33))),_tag_dyneither(_tmp663,sizeof(void*),1)))))));}
return 1;}out_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp66A;void*_tmp669[1];
const char*_tmp668;struct Cyc_String_pa_struct _tmp667;(_tmp667.tag=0,((_tmp667.f1=(
struct _dyneither_ptr)((_tmp668=filename,_tag_dyneither(_tmp668,sizeof(char),
_get_zero_arr_size_char((void*)_tmp668,1)))),((_tmp669[0]=& _tmp667,Cyc_fprintf(
out_file,((_tmp66A="#error -- %s is not supported on this platform\n",
_tag_dyneither(_tmp66A,sizeof(char),48))),_tag_dyneither(_tmp669,sizeof(void*),1)))))));}
Cyc_fclose(out_file);{const char*_tmp670;void*_tmp66F[1];const char*_tmp66E;struct
Cyc_String_pa_struct _tmp66D;(_tmp66D.tag=0,((_tmp66D.f1=(struct _dyneither_ptr)((
_tmp66E=filename,_tag_dyneither(_tmp66E,sizeof(char),_get_zero_arr_size_char((
void*)_tmp66E,1)))),((_tmp66F[0]=& _tmp66D,Cyc_fprintf(Cyc_stderr,((_tmp670="Warning: %s will not be supported on this platform\n",
_tag_dyneither(_tmp670,sizeof(char),52))),_tag_dyneither(_tmp66F,sizeof(void*),1)))))));}{
const char*_tmp673;void*_tmp672;(_tmp672=0,Cyc_log(((_tmp673="Not supported on this platform\n",
_tag_dyneither(_tmp673,sizeof(char),32))),_tag_dyneither(_tmp672,sizeof(void*),0)));}
remove(_tmp215);remove(_tmp216);remove(_tmp217);return 0;_LL1D1:;_LL1D2:(void)
_throw(_tmp32B);_LL1CC:;}};};}int Cyc_process_specfile(const char*file,const char*
dir);static void _tmp67E(unsigned int*_tmp67D,unsigned int*_tmp67C,char**_tmp67B){
for(*_tmp67D=0;*_tmp67D < *_tmp67C;(*_tmp67D)++){(*_tmp67B)[*_tmp67D]='\000';}}
int Cyc_process_specfile(const char*file,const char*dir){struct Cyc___cycFILE*
_tmp354=Cyc_fopen(file,(const char*)"r");if(!((unsigned int)_tmp354)){{const char*
_tmp679;void*_tmp678[1];const char*_tmp677;struct Cyc_String_pa_struct _tmp676;(
_tmp676.tag=0,((_tmp676.f1=(struct _dyneither_ptr)((_tmp677=file,_tag_dyneither(
_tmp677,sizeof(char),_get_zero_arr_size_char((void*)_tmp677,1)))),((_tmp678[0]=&
_tmp676,Cyc_fprintf(Cyc_stderr,((_tmp679="Error: could not open %s\n",
_tag_dyneither(_tmp679,sizeof(char),26))),_tag_dyneither(_tmp678,sizeof(void*),1)))))));}
return 1;}{struct Cyc___cycFILE*_tmp359=(struct Cyc___cycFILE*)_tmp354;char*_tmp683;
unsigned int _tmp682;char*_tmp681;unsigned int _tmp680;unsigned int _tmp67F;struct
_dyneither_ptr buf=(_tmp683=((_tmp682=(unsigned int)1024,((_tmp681=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp682 + 1)),((((_tmp680=_tmp682,((
_tmp67E(& _tmp67F,& _tmp680,& _tmp681),_tmp681[_tmp680]=(char)0)))),_tmp681)))))),
_tag_dyneither(_tmp683,sizeof(char),_get_zero_arr_size_char((void*)_tmp683,(
unsigned int)1024 + 1)));struct _dyneither_ptr _tmp35A=Cyc_getcwd(buf,
_get_dyneither_size(buf,sizeof(char)));if(Cyc_mode != Cyc_GATHERSCRIPT){if(chdir(
dir)){{const char*_tmp689;void*_tmp688[1];const char*_tmp687;struct Cyc_String_pa_struct
_tmp686;(_tmp686.tag=0,((_tmp686.f1=(struct _dyneither_ptr)((_tmp687=dir,
_tag_dyneither(_tmp687,sizeof(char),_get_zero_arr_size_char((void*)_tmp687,1)))),((
_tmp688[0]=& _tmp686,Cyc_fprintf(Cyc_stderr,((_tmp689="Error: can't change directory to %s\n",
_tag_dyneither(_tmp689,sizeof(char),37))),_tag_dyneither(_tmp688,sizeof(void*),1)))))));}
return 1;}}if(Cyc_mode == Cyc_GATHER){struct _dyneither_ptr _tmp35F=
Cstring_to_string(Ccomp);const char*_tmp68D;void*_tmp68C[1];struct Cyc_String_pa_struct
_tmp68B;system((const char*)_untag_dyneither_ptr(((_tmp68B.tag=0,((_tmp68B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35F),((_tmp68C[0]=& _tmp68B,Cyc_aprintf(((
_tmp68D="echo | %s -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp68D,sizeof(
char),36))),_tag_dyneither(_tmp68C,sizeof(void*),1)))))))),sizeof(char),1));}{
struct Cyc_Lexing_lexbuf*_tmp363=Cyc_Lexing_from_file(_tmp359);struct _tuple20*
entry;while((entry=((struct _tuple20*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(
_tmp363))!= 0){struct _tuple20 _tmp365;struct _dyneither_ptr _tmp366;struct Cyc_List_List*
_tmp367;struct Cyc_List_List*_tmp368;struct Cyc_List_List*_tmp369;struct Cyc_List_List*
_tmp36A;struct Cyc_List_List*_tmp36B;struct _tuple20*_tmp364=(struct _tuple20*)
_check_null(entry);_tmp365=*_tmp364;_tmp366=_tmp365.f1;_tmp367=_tmp365.f2;
_tmp368=_tmp365.f3;_tmp369=_tmp365.f4;_tmp36A=_tmp365.f5;_tmp36B=_tmp365.f6;if(
Cyc_process_file((const char*)_untag_dyneither_ptr(_tmp366,sizeof(char),1),
_tmp367,_tmp368,_tmp369,_tmp36A,_tmp36B))return 1;}Cyc_fclose(_tmp359);if(Cyc_mode
!= Cyc_GATHERSCRIPT){if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp35A,
sizeof(char),1)))){{const char*_tmp691;void*_tmp690[1];struct Cyc_String_pa_struct
_tmp68F;(_tmp68F.tag=0,((_tmp68F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp35A),((_tmp690[0]=& _tmp68F,Cyc_fprintf(Cyc_stderr,((_tmp691="Error: could not change directory to %s\n",
_tag_dyneither(_tmp691,sizeof(char),41))),_tag_dyneither(_tmp690,sizeof(void*),1)))))));}
return 1;}}return 0;};};}int Cyc_process_setjmp(const char*dir);static void _tmp696(
unsigned int*_tmp695,unsigned int*_tmp694,char**_tmp693){for(*_tmp695=0;*_tmp695
< *_tmp694;(*_tmp695)++){(*_tmp693)[*_tmp695]='\000';}}int Cyc_process_setjmp(
const char*dir){char*_tmp69B;unsigned int _tmp69A;char*_tmp699;unsigned int _tmp698;
unsigned int _tmp697;struct _dyneither_ptr buf=(_tmp69B=((_tmp69A=(unsigned int)
1024,((_tmp699=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp69A + 1)),((((
_tmp698=_tmp69A,((_tmp696(& _tmp697,& _tmp698,& _tmp699),_tmp699[_tmp698]=(char)0)))),
_tmp699)))))),_tag_dyneither(_tmp69B,sizeof(char),_get_zero_arr_size_char((void*)
_tmp69B,(unsigned int)1024 + 1)));struct _dyneither_ptr _tmp373=Cyc_getcwd(buf,
_get_dyneither_size(buf,sizeof(char)));if(chdir(dir)){{const char*_tmp6A1;void*
_tmp6A0[1];const char*_tmp69F;struct Cyc_String_pa_struct _tmp69E;(_tmp69E.tag=0,((
_tmp69E.f1=(struct _dyneither_ptr)((_tmp69F=dir,_tag_dyneither(_tmp69F,sizeof(
char),_get_zero_arr_size_char((void*)_tmp69F,1)))),((_tmp6A0[0]=& _tmp69E,Cyc_fprintf(
Cyc_stderr,((_tmp6A1="Error: can't change directory to %s\n",_tag_dyneither(
_tmp6A1,sizeof(char),37))),_tag_dyneither(_tmp6A0,sizeof(void*),1)))))));}return
1;}{struct _tuple19*_tmp6AB;const char*_tmp6AA;const char*_tmp6A9;struct _tuple19*
_tmp6A8[1];struct _dyneither_ptr*_tmp6A2[1];if(Cyc_process_file((const char*)"setjmp.h",((
_tmp6A2[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",
sizeof(char),8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp6A2,sizeof(struct _dyneither_ptr*),1)))),0,((_tmp6A8[0]=((
_tmp6AB=_cycalloc(sizeof(*_tmp6AB)),((_tmp6AB->f1=((_tmp6AA="setjmp",
_tag_dyneither(_tmp6AA,sizeof(char),7))),((_tmp6AB->f2=((_tmp6A9="extern int setjmp(jmp_buf);\n",
_tag_dyneither(_tmp6A9,sizeof(char),29))),_tmp6AB)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6A8,sizeof(struct
_tuple19*),1)))),0,0))return 1;}if(chdir((const char*)((char*)_untag_dyneither_ptr(
_tmp373,sizeof(char),1)))){{const char*_tmp6AF;void*_tmp6AE[1];struct Cyc_String_pa_struct
_tmp6AD;(_tmp6AD.tag=0,((_tmp6AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp373),((_tmp6AE[0]=& _tmp6AD,Cyc_fprintf(Cyc_stderr,((_tmp6AF="Error: could not change directory to %s\n",
_tag_dyneither(_tmp6AF,sizeof(char),41))),_tag_dyneither(_tmp6AE,sizeof(void*),1)))))));}
return 1;}return 0;}static char _tmp385[13]="BUILDLIB.OUT";static struct
_dyneither_ptr Cyc_output_dir={_tmp385,_tmp385,_tmp385 + 13};static void Cyc_set_output_dir(
struct _dyneither_ptr s){Cyc_output_dir=s;}static struct Cyc_List_List*Cyc_spec_files=
0;static void Cyc_add_spec_file(struct _dyneither_ptr s){struct Cyc_List_List*_tmp6B0;
Cyc_spec_files=((_tmp6B0=_cycalloc(sizeof(*_tmp6B0)),((_tmp6B0->hd=(const char*)
_untag_dyneither_ptr(s,sizeof(char),1),((_tmp6B0->tl=Cyc_spec_files,_tmp6B0))))));}
static void Cyc_set_GATHER(){Cyc_mode=Cyc_GATHER;}static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}static void Cyc_set_FINISH(){Cyc_mode=Cyc_FINISH;}
static int Cyc_badparse=0;static void Cyc_unsupported_option(struct _dyneither_ptr s){{
const char*_tmp6B4;void*_tmp6B3[1];struct Cyc_String_pa_struct _tmp6B2;(_tmp6B2.tag=
0,((_tmp6B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp6B3[0]=&
_tmp6B2,Cyc_fprintf(Cyc_stderr,((_tmp6B4="Unsupported option %s\n",
_tag_dyneither(_tmp6B4,sizeof(char),23))),_tag_dyneither(_tmp6B3,sizeof(void*),1)))))));}
Cyc_badparse=1;}void GC_blacklist_warn_clear();struct _tuple24{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){GC_blacklist_warn_clear();{struct
_tuple24*_tmp721;const char*_tmp720;const char*_tmp71F;struct Cyc_Arg_String_spec_struct
_tmp71E;struct Cyc_Arg_String_spec_struct*_tmp71D;const char*_tmp71C;struct
_tuple24*_tmp71B;const char*_tmp71A;const char*_tmp719;struct Cyc_Arg_Unit_spec_struct
_tmp718;struct Cyc_Arg_Unit_spec_struct*_tmp717;const char*_tmp716;struct _tuple24*
_tmp715;const char*_tmp714;const char*_tmp713;struct Cyc_Arg_Unit_spec_struct
_tmp712;struct Cyc_Arg_Unit_spec_struct*_tmp711;const char*_tmp710;struct _tuple24*
_tmp70F;const char*_tmp70E;const char*_tmp70D;struct Cyc_Arg_Unit_spec_struct
_tmp70C;struct Cyc_Arg_Unit_spec_struct*_tmp70B;const char*_tmp70A;struct _tuple24*
_tmp709;const char*_tmp708;const char*_tmp707;struct Cyc_Arg_Set_spec_struct _tmp706;
struct Cyc_Arg_Set_spec_struct*_tmp705;const char*_tmp704;struct _tuple24*_tmp703;
const char*_tmp702;const char*_tmp701;struct Cyc_Arg_Flag_spec_struct _tmp700;struct
Cyc_Arg_Flag_spec_struct*_tmp6FF;const char*_tmp6FE;struct _tuple24*_tmp6FD[6];
struct Cyc_List_List*options=(_tmp6FD[5]=((_tmp703=_cycalloc(sizeof(*_tmp703)),((
_tmp703->f1=((_tmp702="-",_tag_dyneither(_tmp702,sizeof(char),2))),((_tmp703->f2=
1,((_tmp703->f3=((_tmp701="",_tag_dyneither(_tmp701,sizeof(char),1))),((_tmp703->f4=(
void*)((_tmp6FF=_cycalloc(sizeof(*_tmp6FF)),((_tmp6FF[0]=((_tmp700.tag=1,((
_tmp700.f1=Cyc_unsupported_option,_tmp700)))),_tmp6FF)))),((_tmp703->f5=((
_tmp6FE="",_tag_dyneither(_tmp6FE,sizeof(char),1))),_tmp703)))))))))))),((
_tmp6FD[4]=((_tmp709=_cycalloc(sizeof(*_tmp709)),((_tmp709->f1=((_tmp708="-setjmp",
_tag_dyneither(_tmp708,sizeof(char),8))),((_tmp709->f2=0,((_tmp709->f3=((_tmp707="",
_tag_dyneither(_tmp707,sizeof(char),1))),((_tmp709->f4=(void*)((_tmp705=
_cycalloc(sizeof(*_tmp705)),((_tmp705[0]=((_tmp706.tag=3,((_tmp706.f1=& Cyc_do_setjmp,
_tmp706)))),_tmp705)))),((_tmp709->f5=((_tmp704="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.",
_tag_dyneither(_tmp704,sizeof(char),186))),_tmp709)))))))))))),((_tmp6FD[3]=((
_tmp70F=_cycalloc(sizeof(*_tmp70F)),((_tmp70F->f1=((_tmp70E="-finish",
_tag_dyneither(_tmp70E,sizeof(char),8))),((_tmp70F->f2=0,((_tmp70F->f3=((_tmp70D="",
_tag_dyneither(_tmp70D,sizeof(char),1))),((_tmp70F->f4=(void*)((_tmp70B=
_cycalloc(sizeof(*_tmp70B)),((_tmp70B[0]=((_tmp70C.tag=0,((_tmp70C.f1=Cyc_set_FINISH,
_tmp70C)))),_tmp70B)))),((_tmp70F->f5=((_tmp70A="Produce Cyclone headers from pre-gathered C library info",
_tag_dyneither(_tmp70A,sizeof(char),57))),_tmp70F)))))))))))),((_tmp6FD[2]=((
_tmp715=_cycalloc(sizeof(*_tmp715)),((_tmp715->f1=((_tmp714="-gatherscript",
_tag_dyneither(_tmp714,sizeof(char),14))),((_tmp715->f2=0,((_tmp715->f3=((
_tmp713="",_tag_dyneither(_tmp713,sizeof(char),1))),((_tmp715->f4=(void*)((
_tmp711=_cycalloc(sizeof(*_tmp711)),((_tmp711[0]=((_tmp712.tag=0,((_tmp712.f1=
Cyc_set_GATHERSCRIPT,_tmp712)))),_tmp711)))),((_tmp715->f5=((_tmp710="Produce a script to gather C library info",
_tag_dyneither(_tmp710,sizeof(char),42))),_tmp715)))))))))))),((_tmp6FD[1]=((
_tmp71B=_cycalloc(sizeof(*_tmp71B)),((_tmp71B->f1=((_tmp71A="-gather",
_tag_dyneither(_tmp71A,sizeof(char),8))),((_tmp71B->f2=0,((_tmp71B->f3=((_tmp719="",
_tag_dyneither(_tmp719,sizeof(char),1))),((_tmp71B->f4=(void*)((_tmp717=
_cycalloc(sizeof(*_tmp717)),((_tmp717[0]=((_tmp718.tag=0,((_tmp718.f1=Cyc_set_GATHER,
_tmp718)))),_tmp717)))),((_tmp71B->f5=((_tmp716="Gather C library info but don't produce Cyclone headers",
_tag_dyneither(_tmp716,sizeof(char),56))),_tmp71B)))))))))))),((_tmp6FD[0]=((
_tmp721=_cycalloc(sizeof(*_tmp721)),((_tmp721->f1=((_tmp720="-d",_tag_dyneither(
_tmp720,sizeof(char),3))),((_tmp721->f2=0,((_tmp721->f3=((_tmp71F=" <file>",
_tag_dyneither(_tmp71F,sizeof(char),8))),((_tmp721->f4=(void*)((_tmp71D=
_cycalloc(sizeof(*_tmp71D)),((_tmp71D[0]=((_tmp71E.tag=5,((_tmp71E.f1=Cyc_set_output_dir,
_tmp71E)))),_tmp71D)))),((_tmp721->f5=((_tmp71C="Set the output directory to <file>",
_tag_dyneither(_tmp71C,sizeof(char),35))),_tmp721)))))))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6FD,sizeof(struct
_tuple24*),6)))))))))))));{const char*_tmp722;Cyc_Arg_parse(options,Cyc_add_spec_file,((
_tmp722="Options:",_tag_dyneither(_tmp722,sizeof(char),9))),argv);}if((((Cyc_badparse
 || !Cyc_do_setjmp  && Cyc_spec_files == 0) || Cyc_do_setjmp  && Cyc_spec_files != 0)
 || Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){{
const char*_tmp723;Cyc_Arg_usage(options,((_tmp723="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",
_tag_dyneither(_tmp723,sizeof(char),59))));}return 1;}if(Cyc_mode == Cyc_GATHERSCRIPT){
Cyc_script_file=Cyc_fopen((const char*)"BUILDLIB.sh",(const char*)"w");if(!((
unsigned int)Cyc_script_file)){{const char*_tmp726;void*_tmp725;(_tmp725=0,Cyc_fprintf(
Cyc_stderr,((_tmp726="Could not create file BUILDLIB.sh\n",_tag_dyneither(
_tmp726,sizeof(char),35))),_tag_dyneither(_tmp725,sizeof(void*),0)));}exit(1);}{
const char*_tmp729;void*_tmp728;(_tmp728=0,Cyc_prscript(((_tmp729="#!/bin/sh\n",
_tag_dyneither(_tmp729,sizeof(char),11))),_tag_dyneither(_tmp728,sizeof(void*),0)));}{
const char*_tmp72C;void*_tmp72B;(_tmp72B=0,Cyc_prscript(((_tmp72C="GCC=\"gcc\"\n",
_tag_dyneither(_tmp72C,sizeof(char),11))),_tag_dyneither(_tmp72B,sizeof(void*),0)));};}
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){{
const char*_tmp730;void*_tmp72F[1];struct Cyc_String_pa_struct _tmp72E;(_tmp72E.tag=
0,((_tmp72E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp72F[0]=& _tmp72E,Cyc_fprintf(Cyc_stderr,((_tmp730="Error: could not create directory %s\n",
_tag_dyneither(_tmp730,sizeof(char),38))),_tag_dyneither(_tmp72F,sizeof(void*),1)))))));}
return 1;}if(Cyc_mode == Cyc_GATHERSCRIPT){{const char*_tmp734;void*_tmp733[1];
struct Cyc_String_pa_struct _tmp732;(_tmp732.tag=0,((_tmp732.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp733[0]=& _tmp732,Cyc_prscript(((
_tmp734="cd %s\n",_tag_dyneither(_tmp734,sizeof(char),7))),_tag_dyneither(
_tmp733,sizeof(void*),1)))))));}{const char*_tmp737;void*_tmp736;(_tmp736=0,Cyc_prscript(((
_tmp737="echo | $GCC -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp737,sizeof(
char),38))),_tag_dyneither(_tmp736,sizeof(void*),0)));};}if(!Cyc_gathering()){{
const char*_tmp738;Cyc_log_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(
Cyc_output_dir,((_tmp738="BUILDLIB.LOG",_tag_dyneither(_tmp738,sizeof(char),13)))),
sizeof(char),1),(const char*)"w");}if(!((unsigned int)Cyc_log_file)){{const char*
_tmp73C;void*_tmp73B[1];struct Cyc_String_pa_struct _tmp73A;(_tmp73A.tag=0,((
_tmp73A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp73B[0]=& _tmp73A,Cyc_fprintf(Cyc_stderr,((_tmp73C="Error: could not create log file in directory %s\n",
_tag_dyneither(_tmp73C,sizeof(char),50))),_tag_dyneither(_tmp73B,sizeof(void*),1)))))));}
return 1;}if(!Cyc_do_setjmp){{const char*_tmp73D;Cyc_cstubs_file=Cyc_fopen((const
char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp73D="cstubs.c",
_tag_dyneither(_tmp73D,sizeof(char),9)))),sizeof(char),1),(const char*)"w");}if(!((
unsigned int)Cyc_cstubs_file)){{const char*_tmp741;void*_tmp740[1];struct Cyc_String_pa_struct
_tmp73F;(_tmp73F.tag=0,((_tmp73F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp740[0]=& _tmp73F,Cyc_fprintf(Cyc_stderr,((_tmp741="Error: could not create cstubs.c in directory %s\n",
_tag_dyneither(_tmp741,sizeof(char),50))),_tag_dyneither(_tmp740,sizeof(void*),1)))))));}
return 1;}{const char*_tmp742;Cyc_cycstubs_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp742="cycstubs.cyc",
_tag_dyneither(_tmp742,sizeof(char),13)))),sizeof(char),1),(const char*)"w");}if(
!((unsigned int)Cyc_cycstubs_file)){{const char*_tmp746;void*_tmp745[1];struct Cyc_String_pa_struct
_tmp744;(_tmp744.tag=0,((_tmp744.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp745[0]=& _tmp744,Cyc_fprintf(Cyc_stderr,((_tmp746="Error: could not create cycstubs.c in directory %s\n",
_tag_dyneither(_tmp746,sizeof(char),52))),_tag_dyneither(_tmp745,sizeof(void*),1)))))));}
return 1;}{const char*_tmp749;void*_tmp748;(_tmp748=0,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_cycstubs_file),((_tmp749="#include <core.h>\nusing Core;\n\n",
_tag_dyneither(_tmp749,sizeof(char),32))),_tag_dyneither(_tmp748,sizeof(void*),0)));};}}{
const char*outdir=(const char*)_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1);
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))return 1;else{for(0;Cyc_spec_files
!= 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){if(
Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,
outdir)){{const char*_tmp74C;void*_tmp74B;(_tmp74B=0,Cyc_fprintf(Cyc_stderr,((
_tmp74C="FATAL ERROR -- QUIT!\n",_tag_dyneither(_tmp74C,sizeof(char),22))),
_tag_dyneither(_tmp74B,sizeof(void*),0)));}exit(1);}}}if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));Cyc_fclose((
struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}return 0;};};}
