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
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
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
Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};
extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};extern
char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tc_tc(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);void Cyc_Lex_lex_init(
int use_cyclone_keywords);extern char*Ccomp;static int Cyc_do_setjmp=0;struct Cyc___cycFILE*
Cyc_log_file=0;struct Cyc___cycFILE*Cyc_cstubs_file=0;struct Cyc___cycFILE*Cyc_cycstubs_file=
0;int Cyc_log(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){if(Cyc_log_file == 0){{
const char*_tmp3B1;void*_tmp3B0;(_tmp3B0=0,Cyc_fprintf(Cyc_stderr,((_tmp3B1="Internal error: log file is NULL\n",
_tag_dyneither(_tmp3B1,sizeof(char),34))),_tag_dyneither(_tmp3B0,sizeof(void*),0)));}
exit(1);}{int _tmp2=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),
fmt,ap);Cyc_fflush((struct Cyc___cycFILE*)((struct Cyc___cycFILE*)_check_null(Cyc_log_file)));
return _tmp2;};}static struct _dyneither_ptr*Cyc_current_source=0;static struct Cyc_List_List*
Cyc_current_args=0;static struct Cyc_Set_Set**Cyc_current_targets=0;static void Cyc_add_target(
struct _dyneither_ptr*sptr){struct Cyc_Set_Set**_tmp3B2;Cyc_current_targets=((
_tmp3B2=_cycalloc(sizeof(*_tmp3B2)),((_tmp3B2[0]=((struct Cyc_Set_Set*(*)(struct
Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets)),sptr),_tmp3B2))));}struct _tuple17{struct
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
_tmp3B8;const char*_tmp3B7;struct Cyc_Lexing_Error_struct*_tmp3B6;(int)_throw((
void*)((_tmp3B6=_cycalloc(sizeof(*_tmp3B6)),((_tmp3B6[0]=((_tmp3B8.tag=Cyc_Lexing_Error,((
_tmp3B8.f1=((_tmp3B7="empty token",_tag_dyneither(_tmp3B7,sizeof(char),12))),
_tmp3B8)))),_tmp3B6)))));}else{return lbuf->lex_last_action;}}else{if(c == 256)
lbuf->lex_eof_reached=0;}}}struct _tuple17*Cyc_line_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL0: Cyc_macroname(lexbuf);for(0;Cyc_current_args != 0;Cyc_current_args=((
struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){struct Cyc_Set_Set**
_tmp3B9;Cyc_current_targets=((_tmp3B9=_cycalloc(sizeof(*_tmp3B9)),((_tmp3B9[0]=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(*((
struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_current_args))->hd),_tmp3B9))));}{struct
_tuple17*_tmp3BA;return(_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA->f1=(
struct _dyneither_ptr*)_check_null(Cyc_current_source),((_tmp3BA->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets)),_tmp3BA)))));};case 1: _LL1: return Cyc_line(
lexbuf);case 2: _LL2: return 0;default: _LL3:(lexbuf->refill_buff)(lexbuf);return Cyc_line_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3C0;const char*_tmp3BF;struct
Cyc_Lexing_Error_struct*_tmp3BE;(int)_throw((void*)((_tmp3BE=_cycalloc(sizeof(*
_tmp3BE)),((_tmp3BE[0]=((_tmp3C0.tag=Cyc_Lexing_Error,((_tmp3C0.f1=((_tmp3BF="some action didn't return!",
_tag_dyneither(_tmp3BF,sizeof(char),27))),_tmp3C0)))),_tmp3BE)))));};}struct
_tuple17*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL5:{struct _dyneither_ptr*_tmp3C1;Cyc_current_source=((
_tmp3C1=_cycalloc(sizeof(*_tmp3C1)),((_tmp3C1[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp3C1))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp3C2;Cyc_current_targets=((_tmp3C2=_cycalloc(sizeof(*
_tmp3C2)),((_tmp3C2[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3C2))));}Cyc_token(
lexbuf);return 0;case 1: _LL6:{struct _dyneither_ptr*_tmp3C3;Cyc_current_source=((
_tmp3C3=_cycalloc(sizeof(*_tmp3C3)),((_tmp3C3[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3C3))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp3C4;Cyc_current_targets=((_tmp3C4=_cycalloc(sizeof(*
_tmp3C4)),((_tmp3C4[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3C4))));}Cyc_args(
lexbuf);return 0;case 2: _LL7:{struct _dyneither_ptr*_tmp3C5;Cyc_current_source=((
_tmp3C5=_cycalloc(sizeof(*_tmp3C5)),((_tmp3C5[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf),_tmp3C5))));}Cyc_current_args=0;{struct Cyc_Set_Set**_tmp3C6;Cyc_current_targets=((
_tmp3C6=_cycalloc(sizeof(*_tmp3C6)),((_tmp3C6[0]=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp3C6))));}Cyc_token(lexbuf);return 0;default: _LL8:(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3CC;
const char*_tmp3CB;struct Cyc_Lexing_Error_struct*_tmp3CA;(int)_throw((void*)((
_tmp3CA=_cycalloc(sizeof(*_tmp3CA)),((_tmp3CA[0]=((_tmp3CC.tag=Cyc_Lexing_Error,((
_tmp3CC.f1=((_tmp3CB="some action didn't return!",_tag_dyneither(_tmp3CB,sizeof(
char),27))),_tmp3CC)))),_tmp3CA)))));};}int Cyc_macroname(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_macroname_rec(lexbuf,1);}int Cyc_args_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLA: {struct _dyneither_ptr*_tmp3CD;struct _dyneither_ptr*_tmp15=(_tmp3CD=
_cycalloc(sizeof(*_tmp3CD)),((_tmp3CD[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp3CD)));{struct Cyc_List_List*
_tmp3CE;Cyc_current_args=((_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE->hd=
_tmp15,((_tmp3CE->tl=Cyc_current_args,_tmp3CE))))));}return Cyc_args(lexbuf);}
case 1: _LLB: {struct _dyneither_ptr*_tmp3CF;struct _dyneither_ptr*_tmp18=(_tmp3CF=
_cycalloc(sizeof(*_tmp3CF)),((_tmp3CF[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3CF)));{struct Cyc_List_List*
_tmp3D0;Cyc_current_args=((_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0->hd=
_tmp18,((_tmp3D0->tl=Cyc_current_args,_tmp3D0))))));}return Cyc_args(lexbuf);}
case 2: _LLC: {struct _dyneither_ptr*_tmp3D1;struct _dyneither_ptr*_tmp1B=(_tmp3D1=
_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3D1)));{struct Cyc_List_List*
_tmp3D2;Cyc_current_args=((_tmp3D2=_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2->hd=
_tmp1B,((_tmp3D2->tl=Cyc_current_args,_tmp3D2))))));}return Cyc_token(lexbuf);}
default: _LLD:(lexbuf->refill_buff)(lexbuf);return Cyc_args_rec(lexbuf,lexstate);}{
struct Cyc_Lexing_Error_struct _tmp3D8;const char*_tmp3D7;struct Cyc_Lexing_Error_struct*
_tmp3D6;(int)_throw((void*)((_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6[0]=((
_tmp3D8.tag=Cyc_Lexing_Error,((_tmp3D8.f1=((_tmp3D7="some action didn't return!",
_tag_dyneither(_tmp3D7,sizeof(char),27))),_tmp3D8)))),_tmp3D6)))));};}int Cyc_args(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}int Cyc_token_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLF:{struct _dyneither_ptr*_tmp3D9;Cyc_add_target(((
_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf),_tmp3D9)))));}return Cyc_token(lexbuf);case 1: _LL10: return 0;case 2: _LL11:
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
return Cyc_token_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3DF;
const char*_tmp3DE;struct Cyc_Lexing_Error_struct*_tmp3DD;(int)_throw((void*)((
_tmp3DD=_cycalloc(sizeof(*_tmp3DD)),((_tmp3DD[0]=((_tmp3DF.tag=Cyc_Lexing_Error,((
_tmp3DF.f1=((_tmp3DE="some action didn't return!",_tag_dyneither(_tmp3DE,sizeof(
char),27))),_tmp3DF)))),_tmp3DD)))));};}int Cyc_token(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_token_rec(lexbuf,3);}int Cyc_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL42: return Cyc_string(lexbuf);case 1: _LL43: return 0;case 2: _LL44: return Cyc_string(
lexbuf);case 3: _LL45: return Cyc_string(lexbuf);case 4: _LL46: return Cyc_string(lexbuf);
case 5: _LL47: return Cyc_string(lexbuf);case 6: _LL48: return Cyc_string(lexbuf);case 7:
_LL49: return 0;case 8: _LL4A: return 0;case 9: _LL4B: return Cyc_string(lexbuf);default:
_LL4C:(lexbuf->refill_buff)(lexbuf);return Cyc_string_rec(lexbuf,lexstate);}{
struct Cyc_Lexing_Error_struct _tmp3E5;const char*_tmp3E4;struct Cyc_Lexing_Error_struct*
_tmp3E3;(int)_throw((void*)((_tmp3E3=_cycalloc(sizeof(*_tmp3E3)),((_tmp3E3[0]=((
_tmp3E5.tag=Cyc_Lexing_Error,((_tmp3E5.f1=((_tmp3E4="some action didn't return!",
_tag_dyneither(_tmp3E4,sizeof(char),27))),_tmp3E5)))),_tmp3E3)))));};}int Cyc_string(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}int Cyc_slurp_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL4E: return 0;case 1: _LL4F: Cyc_fputc((int)'"',(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LL50: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3E8;void*_tmp3E7;(
_tmp3E7=0,Cyc_log(((_tmp3E8="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp3E8,sizeof(char),44))),_tag_dyneither(_tmp3E7,sizeof(void*),0)));}
return 1;case 3: _LL51: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3EB;void*_tmp3EA;(
_tmp3EA=0,Cyc_log(((_tmp3EB="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp3EB,sizeof(char),44))),_tag_dyneither(_tmp3EA,sizeof(void*),0)));}
return 1;case 4: _LL52: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3EE;void*_tmp3ED;(
_tmp3ED=0,Cyc_log(((_tmp3EE="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp3EE,sizeof(char),44))),_tag_dyneither(_tmp3ED,sizeof(void*),0)));}
return 1;case 5: _LL53: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3F1;void*_tmp3F0;(
_tmp3F0=0,Cyc_log(((_tmp3F1="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp3F1,sizeof(char),44))),_tag_dyneither(_tmp3F0,sizeof(void*),0)));}
return 1;case 6: _LL54: Cyc_fputs((const char*)"__region",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));{const char*_tmp3F4;void*_tmp3F3;(_tmp3F3=0,Cyc_log(((
_tmp3F4="Warning: use of region sidestepped\n",_tag_dyneither(_tmp3F4,sizeof(
char),36))),_tag_dyneither(_tmp3F3,sizeof(void*),0)));}return 1;case 7: _LL55:{
const char*_tmp3F7;void*_tmp3F6;(_tmp3F6=0,Cyc_log(((_tmp3F7="Warning: use of __extension__ deleted\n",
_tag_dyneither(_tmp3F7,sizeof(char),39))),_tag_dyneither(_tmp3F6,sizeof(void*),0)));}
return 1;case 8: _LL56:{const char*_tmp3FA;void*_tmp3F9;(_tmp3F9=0,Cyc_log(((_tmp3FA="Warning: use of mode HI deleted\n",
_tag_dyneither(_tmp3FA,sizeof(char),33))),_tag_dyneither(_tmp3F9,sizeof(void*),0)));}
return 1;case 9: _LL57:{const char*_tmp3FD;void*_tmp3FC;(_tmp3FC=0,Cyc_log(((_tmp3FD="Warning: use of mode SI deleted\n",
_tag_dyneither(_tmp3FD,sizeof(char),33))),_tag_dyneither(_tmp3FC,sizeof(void*),0)));}
return 1;case 10: _LL58:{const char*_tmp400;void*_tmp3FF;(_tmp3FF=0,Cyc_log(((
_tmp400="Warning: use of mode QI deleted\n",_tag_dyneither(_tmp400,sizeof(char),
33))),_tag_dyneither(_tmp3FF,sizeof(void*),0)));}return 1;case 11: _LL59:{const char*
_tmp403;void*_tmp402;(_tmp402=0,Cyc_log(((_tmp403="Warning: use of mode DI deleted\n",
_tag_dyneither(_tmp403,sizeof(char),33))),_tag_dyneither(_tmp402,sizeof(void*),0)));}
return 1;case 12: _LL5A:{const char*_tmp406;void*_tmp405;(_tmp405=0,Cyc_log(((
_tmp406="Warning: use of mode DI deleted\n",_tag_dyneither(_tmp406,sizeof(char),
33))),_tag_dyneither(_tmp405,sizeof(void*),0)));}return 1;case 13: _LL5B:{const char*
_tmp409;void*_tmp408;(_tmp408=0,Cyc_log(((_tmp409="Warning: use of mode word deleted\n",
_tag_dyneither(_tmp409,sizeof(char),35))),_tag_dyneither(_tmp408,sizeof(void*),0)));}
return 1;case 14: _LL5C: Cyc_fputs((const char*)"inline",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 15: _LL5D: Cyc_fputs((const char*)"inline",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16: _LL5E: Cyc_fputs((
const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 17: _LL5F: Cyc_fputs((const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 18: _LL60: Cyc_fputs((const char*)"int",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 19: _LL61: return 1;case 20: _LL62: Cyc_parens_to_match=
1;while(Cyc_asmtok(lexbuf)){;}Cyc_fputs((const char*)"0",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));{const char*_tmp40C;void*_tmp40B;(_tmp40B=0,Cyc_log(((
_tmp40C="Warning: replacing use of __asm__ with 0\n",_tag_dyneither(_tmp40C,
sizeof(char),42))),_tag_dyneither(_tmp40B,sizeof(void*),0)));}return 1;case 21:
_LL63: Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;default: _LL64:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp412;
const char*_tmp411;struct Cyc_Lexing_Error_struct*_tmp410;(int)_throw((void*)((
_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410[0]=((_tmp412.tag=Cyc_Lexing_Error,((
_tmp412.f1=((_tmp411="some action didn't return!",_tag_dyneither(_tmp411,sizeof(
char),27))),_tmp412)))),_tmp410)))));};}int Cyc_slurp(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_slurp_rec(lexbuf,5);}int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL66: return 0;case 1: _LL67: Cyc_fputc((int)'"',(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 0;case 2: _LL68:{const char*_tmp415;void*_tmp414;(
_tmp414=0,Cyc_log(((_tmp415="Warning: unclosed string\n",_tag_dyneither(_tmp415,
sizeof(char),26))),_tag_dyneither(_tmp414,sizeof(void*),0)));}{const char*_tmp419;
void*_tmp418[1];struct Cyc_String_pa_struct _tmp417;(_tmp417.tag=0,((_tmp417.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp418[
0]=& _tmp417,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp419="%s",_tag_dyneither(_tmp419,sizeof(char),3))),_tag_dyneither(_tmp418,
sizeof(void*),1)))))));}return 1;case 3: _LL69:{const char*_tmp41D;void*_tmp41C[1];
struct Cyc_String_pa_struct _tmp41B;(_tmp41B.tag=0,((_tmp41B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp41C[0]=&
_tmp41B,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp41D="%s",
_tag_dyneither(_tmp41D,sizeof(char),3))),_tag_dyneither(_tmp41C,sizeof(void*),1)))))));}
return 1;case 4: _LL6A:{const char*_tmp421;void*_tmp420[1];struct Cyc_String_pa_struct
_tmp41F;(_tmp41F.tag=0,((_tmp41F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf)),((_tmp420[0]=& _tmp41F,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),((_tmp421="%s",_tag_dyneither(_tmp421,sizeof(char),3))),
_tag_dyneither(_tmp420,sizeof(void*),1)))))));}return 1;case 5: _LL6B:{const char*
_tmp425;void*_tmp424[1];struct Cyc_String_pa_struct _tmp423;(_tmp423.tag=0,((
_tmp423.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((
_tmp424[0]=& _tmp423,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp425="%s",_tag_dyneither(_tmp425,sizeof(char),3))),_tag_dyneither(_tmp424,
sizeof(void*),1)))))));}return 1;case 6: _LL6C:{const char*_tmp429;void*_tmp428[1];
struct Cyc_String_pa_struct _tmp427;(_tmp427.tag=0,((_tmp427.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp428[0]=&
_tmp427,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp429="%s",
_tag_dyneither(_tmp429,sizeof(char),3))),_tag_dyneither(_tmp428,sizeof(void*),1)))))));}
return 1;case 7: _LL6D:{const char*_tmp42D;void*_tmp42C[1];struct Cyc_String_pa_struct
_tmp42B;(_tmp42B.tag=0,((_tmp42B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf)),((_tmp42C[0]=& _tmp42B,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),((_tmp42D="%s",_tag_dyneither(_tmp42D,sizeof(char),3))),
_tag_dyneither(_tmp42C,sizeof(void*),1)))))));}return 1;case 8: _LL6E:{const char*
_tmp431;void*_tmp430[1];struct Cyc_String_pa_struct _tmp42F;(_tmp42F.tag=0,((
_tmp42F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((
_tmp430[0]=& _tmp42F,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp431="%s",_tag_dyneither(_tmp431,sizeof(char),3))),_tag_dyneither(_tmp430,
sizeof(void*),1)))))));}return 1;default: _LL6F:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp437;const char*_tmp436;struct Cyc_Lexing_Error_struct*_tmp435;(int)_throw((
void*)((_tmp435=_cycalloc(sizeof(*_tmp435)),((_tmp435[0]=((_tmp437.tag=Cyc_Lexing_Error,((
_tmp437.f1=((_tmp436="some action didn't return!",_tag_dyneither(_tmp436,sizeof(
char),27))),_tmp437)))),_tmp435)))));};}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL71: return 0;case 1: _LL72: if(Cyc_parens_to_match == 1)return 0;-- Cyc_parens_to_match;
return 1;case 2: _LL73: ++ Cyc_parens_to_match;return 1;case 3: _LL74: while(Cyc_asm_string(
lexbuf)){;}return 1;case 4: _LL75: while(Cyc_asm_comment(lexbuf)){;}return 1;case 5:
_LL76: return 1;case 6: _LL77: return 1;default: _LL78:(lexbuf->refill_buff)(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp43D;
const char*_tmp43C;struct Cyc_Lexing_Error_struct*_tmp43B;(int)_throw((void*)((
_tmp43B=_cycalloc(sizeof(*_tmp43B)),((_tmp43B[0]=((_tmp43D.tag=Cyc_Lexing_Error,((
_tmp43D.f1=((_tmp43C="some action didn't return!",_tag_dyneither(_tmp43C,sizeof(
char),27))),_tmp43D)))),_tmp43B)))));};}int Cyc_asmtok(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asmtok_rec(lexbuf,7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL7A:{const char*_tmp440;void*_tmp43F;(_tmp43F=0,Cyc_log(((_tmp440="Warning: unclosed string\n",
_tag_dyneither(_tmp440,sizeof(char),26))),_tag_dyneither(_tmp43F,sizeof(void*),0)));}
return 0;case 1: _LL7B: return 0;case 2: _LL7C:{const char*_tmp443;void*_tmp442;(_tmp442=
0,Cyc_log(((_tmp443="Warning: unclosed string\n",_tag_dyneither(_tmp443,sizeof(
char),26))),_tag_dyneither(_tmp442,sizeof(void*),0)));}return 1;case 3: _LL7D:
return 1;case 4: _LL7E: return 1;case 5: _LL7F: return 1;case 6: _LL80: return 1;case 7: _LL81:
return 1;case 8: _LL82: return 1;default: _LL83:(lexbuf->refill_buff)(lexbuf);return
Cyc_asm_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp449;const
char*_tmp448;struct Cyc_Lexing_Error_struct*_tmp447;(int)_throw((void*)((_tmp447=
_cycalloc(sizeof(*_tmp447)),((_tmp447[0]=((_tmp449.tag=Cyc_Lexing_Error,((
_tmp449.f1=((_tmp448="some action didn't return!",_tag_dyneither(_tmp448,sizeof(
char),27))),_tmp449)))),_tmp447)))));};}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_string_rec(lexbuf,8);}int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL85:{const char*_tmp44C;void*_tmp44B;(_tmp44B=0,Cyc_log(((_tmp44C="Warning: unclosed comment\n",
_tag_dyneither(_tmp44C,sizeof(char),27))),_tag_dyneither(_tmp44B,sizeof(void*),0)));}
return 0;case 1: _LL86: return 0;case 2: _LL87: return 1;default: _LL88:(lexbuf->refill_buff)(
lexbuf);return Cyc_asm_comment_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp452;const char*_tmp451;struct Cyc_Lexing_Error_struct*_tmp450;(int)_throw((
void*)((_tmp450=_cycalloc(sizeof(*_tmp450)),((_tmp450[0]=((_tmp452.tag=Cyc_Lexing_Error,((
_tmp452.f1=((_tmp451="some action didn't return!",_tag_dyneither(_tmp451,sizeof(
char),27))),_tmp452)))),_tmp450)))));};}int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}struct _tuple18*Cyc_suck_line_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL8A:{const char*_tmp453;Cyc_current_line=((
_tmp453="#define ",_tag_dyneither(_tmp453,sizeof(char),9)));}Cyc_suck_macroname(
lexbuf);{struct _tuple18*_tmp454;return(_tmp454=_cycalloc(sizeof(*_tmp454)),((
_tmp454->f1=Cyc_current_line,((_tmp454->f2=(struct _dyneither_ptr*)_check_null(
Cyc_current_source),_tmp454)))));};case 1: _LL8B: return Cyc_suck_line(lexbuf);case 2:
_LL8C: return 0;default: _LL8D:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_line_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp45A;const char*_tmp459;struct
Cyc_Lexing_Error_struct*_tmp458;(int)_throw((void*)((_tmp458=_cycalloc(sizeof(*
_tmp458)),((_tmp458[0]=((_tmp45A.tag=Cyc_Lexing_Error,((_tmp45A.f1=((_tmp459="some action didn't return!",
_tag_dyneither(_tmp459,sizeof(char),27))),_tmp45A)))),_tmp458)))));};}struct
_tuple18*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(
lexbuf,10);}int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8F:{struct
_dyneither_ptr*_tmp45B;Cyc_current_source=((_tmp45B=_cycalloc(sizeof(*_tmp45B)),((
_tmp45B[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp45B))));}Cyc_current_line=(
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);default: _LL90:(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp461;const char*_tmp460;struct Cyc_Lexing_Error_struct*_tmp45F;(int)_throw((
void*)((_tmp45F=_cycalloc(sizeof(*_tmp45F)),((_tmp45F[0]=((_tmp461.tag=Cyc_Lexing_Error,((
_tmp461.f1=((_tmp460="some action didn't return!",_tag_dyneither(_tmp460,sizeof(
char),27))),_tmp461)))),_tmp45F)))));};}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}int Cyc_suck_restofline_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL92: Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((
struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 0;default: _LL93:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_restofline_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp467;const char*_tmp466;struct
Cyc_Lexing_Error_struct*_tmp465;(int)_throw((void*)((_tmp465=_cycalloc(sizeof(*
_tmp465)),((_tmp465[0]=((_tmp467.tag=Cyc_Lexing_Error,((_tmp467.f1=((_tmp466="some action didn't return!",
_tag_dyneither(_tmp466,sizeof(char),27))),_tmp467)))),_tmp465)))));};}int Cyc_suck_restofline(
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
x))Cyc_List_imp_rev)(Cyc_current_cycstubs);{struct _tuple20*_tmp468;return(
_tmp468=_cycalloc(sizeof(*_tmp468)),((_tmp468->f1=Cyc_current_headerfile,((
_tmp468->f2=Cyc_current_symbols,((_tmp468->f3=Cyc_current_omit_symbols,((_tmp468->f4=
Cyc_current_hstubs,((_tmp468->f5=Cyc_current_cstubs,((_tmp468->f6=Cyc_current_cycstubs,
_tmp468)))))))))))));};case 2: _LL97: return Cyc_spec(lexbuf);case 3: _LL98: return 0;
case 4: _LL99:{const char*_tmp46C;void*_tmp46B[1];struct Cyc_Int_pa_struct _tmp46A;(
_tmp46A.tag=1,((_tmp46A.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0)),((
_tmp46B[0]=& _tmp46A,Cyc_fprintf(Cyc_stderr,((_tmp46C="Error in .cys file: expected header file name, found '%c' instead\n",
_tag_dyneither(_tmp46C,sizeof(char),67))),_tag_dyneither(_tmp46B,sizeof(void*),1)))))));}
return 0;default: _LL9A:(lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp472;const char*_tmp471;struct Cyc_Lexing_Error_struct*
_tmp470;(int)_throw((void*)((_tmp470=_cycalloc(sizeof(*_tmp470)),((_tmp470[0]=((
_tmp472.tag=Cyc_Lexing_Error,((_tmp472.f1=((_tmp471="some action didn't return!",
_tag_dyneither(_tmp471,sizeof(char),27))),_tmp472)))),_tmp470)))));};}struct
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
_tuple19*_tmp473;struct _tuple19*x=(_tmp473=_cycalloc(sizeof(*_tmp473)),((_tmp473->f1=(
struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp473->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp473)))));{
struct Cyc_List_List*_tmp474;Cyc_current_hstubs=((_tmp474=_cycalloc(sizeof(*
_tmp474)),((_tmp474->hd=x,((_tmp474->tl=Cyc_current_hstubs,_tmp474))))));}return
1;};case 5: _LLA1: {struct _dyneither_ptr _tmp83=Cyc_Lexing_lexeme(lexbuf);{const
char*_tmp475;_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),(int)Cyc_strlen(((
_tmp475="hstub",_tag_dyneither(_tmp475,sizeof(char),6)))));}while(isspace((int)*((
char*)_check_dyneither_subscript(_tmp83,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),1);}{struct _dyneither_ptr t=
_tmp83;while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp85=Cyc_substring((
struct _dyneither_ptr)_tmp83,0,(unsigned long)((t.curr - _tmp83.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp476;struct _tuple19*x=(_tmp476=
_cycalloc(sizeof(*_tmp476)),((_tmp476->f1=(struct _dyneither_ptr)_tmp85,((_tmp476->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp476)))));{struct Cyc_List_List*_tmp477;Cyc_current_hstubs=((_tmp477=_cycalloc(
sizeof(*_tmp477)),((_tmp477->hd=x,((_tmp477->tl=Cyc_current_hstubs,_tmp477))))));}
return 1;};};};}case 6: _LLA2: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp478;struct
_tuple19*x=(_tmp478=_cycalloc(sizeof(*_tmp478)),((_tmp478->f1=(struct
_dyneither_ptr)_tag_dyneither(0,0,0),((_tmp478->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp478)))));{struct Cyc_List_List*
_tmp479;Cyc_current_cstubs=((_tmp479=_cycalloc(sizeof(*_tmp479)),((_tmp479->hd=x,((
_tmp479->tl=Cyc_current_cstubs,_tmp479))))));}return 1;};case 7: _LLA3: {struct
_dyneither_ptr _tmp8A=Cyc_Lexing_lexeme(lexbuf);{const char*_tmp47A;
_dyneither_ptr_inplace_plus(& _tmp8A,sizeof(char),(int)Cyc_strlen(((_tmp47A="cstub",
_tag_dyneither(_tmp47A,sizeof(char),6)))));}while(isspace((int)*((char*)
_check_dyneither_subscript(_tmp8A,sizeof(char),0)))){_dyneither_ptr_inplace_plus(&
_tmp8A,sizeof(char),1);}{struct _dyneither_ptr t=_tmp8A;while(!isspace((int)*((
char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp8C=Cyc_substring((
struct _dyneither_ptr)_tmp8A,0,(unsigned long)((t.curr - _tmp8A.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp47B;struct _tuple19*x=(_tmp47B=
_cycalloc(sizeof(*_tmp47B)),((_tmp47B->f1=(struct _dyneither_ptr)_tmp8C,((_tmp47B->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp47B)))));{struct Cyc_List_List*_tmp47C;Cyc_current_cstubs=((_tmp47C=_cycalloc(
sizeof(*_tmp47C)),((_tmp47C->hd=x,((_tmp47C->tl=Cyc_current_cstubs,_tmp47C))))));}
return 1;};};};}case 8: _LLA4: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp47D;struct
_tuple19*x=(_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D->f1=(struct
_dyneither_ptr)_tag_dyneither(0,0,0),((_tmp47D->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp47D)))));{struct Cyc_List_List*
_tmp47E;Cyc_current_cycstubs=((_tmp47E=_cycalloc(sizeof(*_tmp47E)),((_tmp47E->hd=
x,((_tmp47E->tl=Cyc_current_cycstubs,_tmp47E))))));}return 1;};case 9: _LLA5: {
struct _dyneither_ptr _tmp91=Cyc_Lexing_lexeme(lexbuf);{const char*_tmp47F;
_dyneither_ptr_inplace_plus(& _tmp91,sizeof(char),(int)Cyc_strlen(((_tmp47F="cycstub",
_tag_dyneither(_tmp47F,sizeof(char),8)))));}while(isspace((int)*((char*)
_check_dyneither_subscript(_tmp91,sizeof(char),0)))){_dyneither_ptr_inplace_plus(&
_tmp91,sizeof(char),1);}{struct _dyneither_ptr t=_tmp91;while(!isspace((int)*((
char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp93=Cyc_substring((
struct _dyneither_ptr)_tmp91,0,(unsigned long)((t.curr - _tmp91.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp480;struct _tuple19*x=(_tmp480=
_cycalloc(sizeof(*_tmp480)),((_tmp480->f1=(struct _dyneither_ptr)_tmp93,((_tmp480->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp480)))));{struct Cyc_List_List*_tmp481;Cyc_current_cycstubs=((_tmp481=
_cycalloc(sizeof(*_tmp481)),((_tmp481->hd=x,((_tmp481->tl=Cyc_current_cycstubs,
_tmp481))))));}return 1;};};};}case 10: _LLA6: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_dyneither_ptr*_tmp482;struct _dyneither_ptr*x=(_tmp482=_cycalloc(sizeof(*_tmp482)),((
_tmp482[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf)),_tmp482)));{struct Cyc_List_List*_tmp483;Cyc_current_cpp=((
_tmp483=_cycalloc(sizeof(*_tmp483)),((_tmp483->hd=x,((_tmp483->tl=Cyc_current_cpp,
_tmp483))))));}return 1;};case 11: _LLA7: return 1;case 12: _LLA8: return 1;case 13: _LLA9:{
const char*_tmp487;void*_tmp486[1];struct Cyc_Int_pa_struct _tmp485;(_tmp485.tag=1,((
_tmp485.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp486[0]=&
_tmp485,Cyc_fprintf(Cyc_stderr,((_tmp487="Error in .cys file: expected command, found '%c' instead\n",
_tag_dyneither(_tmp487,sizeof(char),58))),_tag_dyneither(_tmp486,sizeof(void*),1)))))));}
return 0;default: _LLAA:(lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp48D;const char*_tmp48C;struct
Cyc_Lexing_Error_struct*_tmp48B;(int)_throw((void*)((_tmp48B=_cycalloc(sizeof(*
_tmp48B)),((_tmp48B[0]=((_tmp48D.tag=Cyc_Lexing_Error,((_tmp48D.f1=((_tmp48C="some action didn't return!",
_tag_dyneither(_tmp48C,sizeof(char),27))),_tmp48D)))),_tmp48B)))));};}int Cyc_commands(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}int Cyc_snarfsymbols_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLAC:{struct _dyneither_ptr*_tmp490;struct Cyc_List_List*
_tmp48F;Cyc_snarfed_symbols=((_tmp48F=_cycalloc(sizeof(*_tmp48F)),((_tmp48F->hd=((
_tmp490=_cycalloc(sizeof(*_tmp490)),((_tmp490[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf),_tmp490)))),((_tmp48F->tl=Cyc_snarfed_symbols,_tmp48F))))));}return 1;
case 1: _LLAD: return 1;case 2: _LLAE: return 0;case 3: _LLAF:{const char*_tmp493;void*
_tmp492;(_tmp492=0,Cyc_fprintf(Cyc_stderr,((_tmp493="Error in .cys file: unexpected end-of-file\n",
_tag_dyneither(_tmp493,sizeof(char),44))),_tag_dyneither(_tmp492,sizeof(void*),0)));}
return 0;case 4: _LLB0:{const char*_tmp497;void*_tmp496[1];struct Cyc_Int_pa_struct
_tmp495;(_tmp495.tag=1,((_tmp495.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)),((_tmp496[0]=& _tmp495,Cyc_fprintf(Cyc_stderr,((_tmp497="Error in .cys file: expected symbol, found '%c' instead\n",
_tag_dyneither(_tmp497,sizeof(char),57))),_tag_dyneither(_tmp496,sizeof(void*),1)))))));}
return 0;default: _LLB1:(lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp49D;const char*_tmp49C;struct
Cyc_Lexing_Error_struct*_tmp49B;(int)_throw((void*)((_tmp49B=_cycalloc(sizeof(*
_tmp49B)),((_tmp49B[0]=((_tmp49D.tag=Cyc_Lexing_Error,((_tmp49D.f1=((_tmp49C="some action didn't return!",
_tag_dyneither(_tmp49C,sizeof(char),27))),_tmp49D)))),_tmp49B)))));};}int Cyc_snarfsymbols(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}int Cyc_block_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLB3:{const char*_tmp4A0;void*_tmp49F;(_tmp49F=0,
Cyc_log(((_tmp4A0="Warning: unclosed brace\n",_tag_dyneither(_tmp4A0,sizeof(char),
25))),_tag_dyneither(_tmp49F,sizeof(void*),0)));}return 0;case 1: _LLB4: if(Cyc_braces_to_match
== 1)return 0;-- Cyc_braces_to_match;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'}');return 1;case 2: _LLB5: ++ Cyc_braces_to_match;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return 1;case 3: _LLB6: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LLB7:{const char*_tmp4A1;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),((_tmp4A1="/*",_tag_dyneither(_tmp4A1,sizeof(char),3))));}
while(Cyc_block_comment(lexbuf)){;}return 1;case 5: _LLB8: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 6: _LLB9: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));return 1;default: _LLBA:(
lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp4A7;const char*_tmp4A6;struct Cyc_Lexing_Error_struct*_tmp4A5;(int)_throw((
void*)((_tmp4A5=_cycalloc(sizeof(*_tmp4A5)),((_tmp4A5[0]=((_tmp4A7.tag=Cyc_Lexing_Error,((
_tmp4A7.f1=((_tmp4A6="some action didn't return!",_tag_dyneither(_tmp4A6,sizeof(
char),27))),_tmp4A7)))),_tmp4A5)))));};}int Cyc_block(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_block_rec(lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLBC:{const char*_tmp4AA;void*_tmp4A9;(_tmp4A9=0,Cyc_log(((_tmp4AA="Warning: unclosed string\n",
_tag_dyneither(_tmp4AA,sizeof(char),26))),_tag_dyneither(_tmp4A9,sizeof(void*),0)));}
return 0;case 1: _LLBD: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
return 0;case 2: _LLBE:{const char*_tmp4AD;void*_tmp4AC;(_tmp4AC=0,Cyc_log(((_tmp4AD="Warning: unclosed string\n",
_tag_dyneither(_tmp4AD,sizeof(char),26))),_tag_dyneither(_tmp4AC,sizeof(void*),0)));}
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
_tmp4B3;const char*_tmp4B2;struct Cyc_Lexing_Error_struct*_tmp4B1;(int)_throw((
void*)((_tmp4B1=_cycalloc(sizeof(*_tmp4B1)),((_tmp4B1[0]=((_tmp4B3.tag=Cyc_Lexing_Error,((
_tmp4B3.f1=((_tmp4B2="some action didn't return!",_tag_dyneither(_tmp4B2,sizeof(
char),27))),_tmp4B3)))),_tmp4B1)))));};}int Cyc_block_string(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_block_string_rec(lexbuf,17);}int Cyc_block_comment_rec(struct
Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLC7:{const char*_tmp4B6;void*_tmp4B5;(_tmp4B5=0,Cyc_log(((
_tmp4B6="Warning: unclosed comment\n",_tag_dyneither(_tmp4B6,sizeof(char),27))),
_tag_dyneither(_tmp4B5,sizeof(void*),0)));}return 0;case 1: _LLC8:{const char*
_tmp4B7;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((
_tmp4B7="*/",_tag_dyneither(_tmp4B7,sizeof(char),3))));}return 0;case 2: _LLC9: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;default: _LLCA:(lexbuf->refill_buff)(lexbuf);return Cyc_block_comment_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp4BD;const char*_tmp4BC;struct
Cyc_Lexing_Error_struct*_tmp4BB;(int)_throw((void*)((_tmp4BB=_cycalloc(sizeof(*
_tmp4BB)),((_tmp4BB[0]=((_tmp4BD.tag=Cyc_Lexing_Error,((_tmp4BD.f1=((_tmp4BC="some action didn't return!",
_tag_dyneither(_tmp4BC,sizeof(char),27))),_tmp4BD)))),_tmp4BB)))));};}int Cyc_block_comment(
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
_tmpBB;if(_tmp103->tag != 35)goto _LL103;}_LL102:{const char*_tmp4C0;void*_tmp4BF;(
_tmp4BF=0,Cyc_fprintf(Cyc_stderr,((_tmp4C0="Error: unexpected Swap_e\n",
_tag_dyneither(_tmp4C0,sizeof(char),26))),_tag_dyneither(_tmp4BF,sizeof(void*),0)));}
exit(1);return;_LL103: {struct Cyc_Absyn_StmtExp_e_struct*_tmp104=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpBB;if(_tmp104->tag != 37)goto _LL105;}_LL104:{const char*_tmp4C3;void*_tmp4C2;(
_tmp4C2=0,Cyc_fprintf(Cyc_stderr,((_tmp4C3="Error: unexpected Stmt_e\n",
_tag_dyneither(_tmp4C3,sizeof(char),26))),_tag_dyneither(_tmp4C2,sizeof(void*),0)));}
exit(1);return;_LL105: {struct Cyc_Absyn_Throw_e_struct*_tmp105=(struct Cyc_Absyn_Throw_e_struct*)
_tmpBB;if(_tmp105->tag != 12)goto _LL107;}_LL106:{const char*_tmp4C6;void*_tmp4C5;(
_tmp4C5=0,Cyc_fprintf(Cyc_stderr,((_tmp4C6="Error: unexpected Throw_e\n",
_tag_dyneither(_tmp4C6,sizeof(char),27))),_tag_dyneither(_tmp4C5,sizeof(void*),0)));}
exit(1);return;_LL107: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp106=(struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmpBB;if(_tmp106->tag != 13)goto _LL109;}_LL108:{
const char*_tmp4C9;void*_tmp4C8;(_tmp4C8=0,Cyc_fprintf(Cyc_stderr,((_tmp4C9="Error: unexpected NoInstantiate_e\n",
_tag_dyneither(_tmp4C9,sizeof(char),35))),_tag_dyneither(_tmp4C8,sizeof(void*),0)));}
exit(1);return;_LL109: {struct Cyc_Absyn_Instantiate_e_struct*_tmp107=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpBB;if(_tmp107->tag != 14)goto _LL10B;}_LL10A:{const char*_tmp4CC;void*_tmp4CB;(
_tmp4CB=0,Cyc_fprintf(Cyc_stderr,((_tmp4CC="Error: unexpected Instantiate_e\n",
_tag_dyneither(_tmp4CC,sizeof(char),33))),_tag_dyneither(_tmp4CB,sizeof(void*),0)));}
exit(1);return;_LL10B: {struct Cyc_Absyn_New_e_struct*_tmp108=(struct Cyc_Absyn_New_e_struct*)
_tmpBB;if(_tmp108->tag != 17)goto _LL10D;}_LL10C:{const char*_tmp4CF;void*_tmp4CE;(
_tmp4CE=0,Cyc_fprintf(Cyc_stderr,((_tmp4CF="Error: unexpected New_e\n",
_tag_dyneither(_tmp4CF,sizeof(char),25))),_tag_dyneither(_tmp4CE,sizeof(void*),0)));}
exit(1);return;_LL10D: {struct Cyc_Absyn_Tuple_e_struct*_tmp109=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpBB;if(_tmp109->tag != 25)goto _LL10F;}_LL10E:{const char*_tmp4D2;void*_tmp4D1;(
_tmp4D1=0,Cyc_fprintf(Cyc_stderr,((_tmp4D2="Error: unexpected Tuple_e\n",
_tag_dyneither(_tmp4D2,sizeof(char),27))),_tag_dyneither(_tmp4D1,sizeof(void*),0)));}
exit(1);return;_LL10F: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp10A=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpBB;if(_tmp10A->tag != 26)goto _LL111;}_LL110:{const char*_tmp4D5;void*_tmp4D4;(
_tmp4D4=0,Cyc_fprintf(Cyc_stderr,((_tmp4D5="Error: unexpected CompoundLit_e\n",
_tag_dyneither(_tmp4D5,sizeof(char),33))),_tag_dyneither(_tmp4D4,sizeof(void*),0)));}
exit(1);return;_LL111: {struct Cyc_Absyn_Array_e_struct*_tmp10B=(struct Cyc_Absyn_Array_e_struct*)
_tmpBB;if(_tmp10B->tag != 27)goto _LL113;}_LL112:{const char*_tmp4D8;void*_tmp4D7;(
_tmp4D7=0,Cyc_fprintf(Cyc_stderr,((_tmp4D8="Error: unexpected Array_e\n",
_tag_dyneither(_tmp4D8,sizeof(char),27))),_tag_dyneither(_tmp4D7,sizeof(void*),0)));}
exit(1);return;_LL113: {struct Cyc_Absyn_Comprehension_e_struct*_tmp10C=(struct
Cyc_Absyn_Comprehension_e_struct*)_tmpBB;if(_tmp10C->tag != 28)goto _LL115;}_LL114:{
const char*_tmp4DB;void*_tmp4DA;(_tmp4DA=0,Cyc_fprintf(Cyc_stderr,((_tmp4DB="Error: unexpected Comprehension_e\n",
_tag_dyneither(_tmp4DB,sizeof(char),35))),_tag_dyneither(_tmp4DA,sizeof(void*),0)));}
exit(1);return;_LL115: {struct Cyc_Absyn_Aggregate_e_struct*_tmp10D=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpBB;if(_tmp10D->tag != 29)goto _LL117;}_LL116:{const char*_tmp4DE;void*_tmp4DD;(
_tmp4DD=0,Cyc_fprintf(Cyc_stderr,((_tmp4DE="Error: unexpected Aggregate_e\n",
_tag_dyneither(_tmp4DE,sizeof(char),31))),_tag_dyneither(_tmp4DD,sizeof(void*),0)));}
exit(1);return;_LL117: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp10E=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpBB;if(_tmp10E->tag != 30)goto _LL119;}_LL118:{const char*_tmp4E1;void*_tmp4E0;(
_tmp4E0=0,Cyc_fprintf(Cyc_stderr,((_tmp4E1="Error: unexpected AnonStruct_e\n",
_tag_dyneither(_tmp4E1,sizeof(char),32))),_tag_dyneither(_tmp4E0,sizeof(void*),0)));}
exit(1);return;_LL119: {struct Cyc_Absyn_Datatype_e_struct*_tmp10F=(struct Cyc_Absyn_Datatype_e_struct*)
_tmpBB;if(_tmp10F->tag != 31)goto _LL11B;}_LL11A:{const char*_tmp4E4;void*_tmp4E3;(
_tmp4E3=0,Cyc_fprintf(Cyc_stderr,((_tmp4E4="Error: unexpected Datatype_e\n",
_tag_dyneither(_tmp4E4,sizeof(char),30))),_tag_dyneither(_tmp4E3,sizeof(void*),0)));}
exit(1);return;_LL11B: {struct Cyc_Absyn_Enum_e_struct*_tmp110=(struct Cyc_Absyn_Enum_e_struct*)
_tmpBB;if(_tmp110->tag != 32)goto _LL11D;}_LL11C:{const char*_tmp4E7;void*_tmp4E6;(
_tmp4E6=0,Cyc_fprintf(Cyc_stderr,((_tmp4E7="Error: unexpected Enum_e\n",
_tag_dyneither(_tmp4E7,sizeof(char),26))),_tag_dyneither(_tmp4E6,sizeof(void*),0)));}
exit(1);return;_LL11D: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp111=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpBB;if(_tmp111->tag != 33)goto _LLCC;}_LL11E:{const char*_tmp4EA;void*_tmp4E9;(
_tmp4E9=0,Cyc_fprintf(Cyc_stderr,((_tmp4EA="Error: unexpected AnonEnum_e\n",
_tag_dyneither(_tmp4EA,sizeof(char),30))),_tag_dyneither(_tmp4E9,sizeof(void*),0)));}
exit(1);return;_LLCC:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo){if((
unsigned int)eo)Cyc_scan_exp(eo);return;}void Cyc_scan_type(void*t){void*_tmp137=
t;struct Cyc_Absyn_PtrInfo _tmp13D;struct Cyc_Absyn_ArrayInfo _tmp13F;void*_tmp140;
struct Cyc_Absyn_Exp*_tmp141;union Cyc_Absyn_Constraint*_tmp142;struct Cyc_Absyn_FnInfo
_tmp144;struct Cyc_List_List*_tmp146;struct Cyc_Absyn_AggrInfo _tmp149;union Cyc_Absyn_AggrInfoU
_tmp14A;struct _tuple0*_tmp14C;struct _tuple0 _tmp14D;struct _dyneither_ptr*_tmp14E;
struct _tuple0*_tmp150;struct _tuple0 _tmp151;struct _dyneither_ptr*_tmp152;_LL125: {
struct Cyc_Absyn_VoidType_struct*_tmp138=(struct Cyc_Absyn_VoidType_struct*)
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
_tmp161=_tmp144.args;for(0;_tmp161 != 0;_tmp161=_tmp161->tl){struct _tuple7 _tmp163;
void*_tmp164;struct _tuple7*_tmp162=(struct _tuple7*)_tmp161->hd;_tmp163=*_tmp162;
_tmp164=_tmp163.f3;Cyc_scan_type(_tmp164);}}if(_tmp144.cyc_varargs != 0)Cyc_scan_type((
_tmp144.cyc_varargs)->type);return;_LL133: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp145=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp137;if(_tmp145->tag != 13)goto
_LL135;else{_tmp146=_tmp145->f2;}}_LL134: for(0;_tmp146 != 0;_tmp146=_tmp146->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp146->hd)->type);Cyc_scan_exp_opt(((
struct Cyc_Absyn_Aggrfield*)_tmp146->hd)->width);}return;_LL135: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp147=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp137;if(_tmp147->tag != 15)goto
_LL137;}_LL136: return;_LL137: {struct Cyc_Absyn_AggrType_struct*_tmp148=(struct
Cyc_Absyn_AggrType_struct*)_tmp137;if(_tmp148->tag != 12)goto _LL139;else{_tmp149=
_tmp148->f1;_tmp14A=_tmp149.aggr_info;}}_LL138: {struct _tuple0*_tmp166;struct
_tuple0 _tmp167;struct _dyneither_ptr*_tmp168;struct _tuple9 _tmp165=Cyc_Absyn_aggr_kinded_name(
_tmp14A);_tmp166=_tmp165.f2;_tmp167=*_tmp166;_tmp168=_tmp167.f2;_tmp14E=_tmp168;
goto _LL13A;}_LL139: {struct Cyc_Absyn_EnumType_struct*_tmp14B=(struct Cyc_Absyn_EnumType_struct*)
_tmp137;if(_tmp14B->tag != 14)goto _LL13B;else{_tmp14C=_tmp14B->f1;_tmp14D=*
_tmp14C;_tmp14E=_tmp14D.f2;}}_LL13A: _tmp152=_tmp14E;goto _LL13C;_LL13B: {struct
Cyc_Absyn_TypedefType_struct*_tmp14F=(struct Cyc_Absyn_TypedefType_struct*)
_tmp137;if(_tmp14F->tag != 18)goto _LL13D;else{_tmp150=_tmp14F->f1;_tmp151=*
_tmp150;_tmp152=_tmp151.f2;}}_LL13C: Cyc_add_target(_tmp152);return;_LL13D: {
struct Cyc_Absyn_Evar_struct*_tmp153=(struct Cyc_Absyn_Evar_struct*)_tmp137;if(
_tmp153->tag != 1)goto _LL13F;}_LL13E:{const char*_tmp4ED;void*_tmp4EC;(_tmp4EC=0,
Cyc_fprintf(Cyc_stderr,((_tmp4ED="Error: unexpected Evar\n",_tag_dyneither(
_tmp4ED,sizeof(char),24))),_tag_dyneither(_tmp4EC,sizeof(void*),0)));}exit(1);
return;_LL13F: {struct Cyc_Absyn_VarType_struct*_tmp154=(struct Cyc_Absyn_VarType_struct*)
_tmp137;if(_tmp154->tag != 2)goto _LL141;}_LL140:{const char*_tmp4F0;void*_tmp4EF;(
_tmp4EF=0,Cyc_fprintf(Cyc_stderr,((_tmp4F0="Error: unexpected VarType\n",
_tag_dyneither(_tmp4F0,sizeof(char),27))),_tag_dyneither(_tmp4EF,sizeof(void*),0)));}
exit(1);return;_LL141: {struct Cyc_Absyn_DatatypeType_struct*_tmp155=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp137;if(_tmp155->tag != 3)goto _LL143;}_LL142:{const char*_tmp4F3;void*_tmp4F2;(
_tmp4F2=0,Cyc_fprintf(Cyc_stderr,((_tmp4F3="Error: unexpected DatatypeType\n",
_tag_dyneither(_tmp4F3,sizeof(char),32))),_tag_dyneither(_tmp4F2,sizeof(void*),0)));}
exit(1);return;_LL143: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp156=(struct
Cyc_Absyn_DatatypeFieldType_struct*)_tmp137;if(_tmp156->tag != 4)goto _LL145;}
_LL144:{const char*_tmp4F6;void*_tmp4F5;(_tmp4F5=0,Cyc_fprintf(Cyc_stderr,((
_tmp4F6="Error: unexpected DatatypeFieldType\n",_tag_dyneither(_tmp4F6,sizeof(
char),37))),_tag_dyneither(_tmp4F5,sizeof(void*),0)));}exit(1);return;_LL145: {
struct Cyc_Absyn_TupleType_struct*_tmp157=(struct Cyc_Absyn_TupleType_struct*)
_tmp137;if(_tmp157->tag != 11)goto _LL147;}_LL146:{const char*_tmp4F9;void*_tmp4F8;(
_tmp4F8=0,Cyc_fprintf(Cyc_stderr,((_tmp4F9="Error: unexpected TupleType\n",
_tag_dyneither(_tmp4F9,sizeof(char),29))),_tag_dyneither(_tmp4F8,sizeof(void*),0)));}
exit(1);return;_LL147: {struct Cyc_Absyn_RgnHandleType_struct*_tmp158=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp137;if(_tmp158->tag != 16)goto _LL149;}_LL148:{const char*_tmp4FC;void*_tmp4FB;(
_tmp4FB=0,Cyc_fprintf(Cyc_stderr,((_tmp4FC="Error: unexpected RgnHandleType\n",
_tag_dyneither(_tmp4FC,sizeof(char),33))),_tag_dyneither(_tmp4FB,sizeof(void*),0)));}
exit(1);return;_LL149: {struct Cyc_Absyn_DynRgnType_struct*_tmp159=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp137;if(_tmp159->tag != 17)goto _LL14B;}_LL14A:{const char*_tmp4FF;void*_tmp4FE;(
_tmp4FE=0,Cyc_fprintf(Cyc_stderr,((_tmp4FF="Error: unexpected DynRgnType\n",
_tag_dyneither(_tmp4FF,sizeof(char),30))),_tag_dyneither(_tmp4FE,sizeof(void*),0)));}
exit(1);return;_LL14B: {struct Cyc_Absyn_HeapRgn_struct*_tmp15A=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp137;if(_tmp15A->tag != 21)goto _LL14D;}_LL14C:{const char*_tmp502;void*_tmp501;(
_tmp501=0,Cyc_fprintf(Cyc_stderr,((_tmp502="Error: unexpected HeapRgn\n",
_tag_dyneither(_tmp502,sizeof(char),27))),_tag_dyneither(_tmp501,sizeof(void*),0)));}
exit(1);return;_LL14D: {struct Cyc_Absyn_UniqueRgn_struct*_tmp15B=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp137;if(_tmp15B->tag != 22)goto _LL14F;}_LL14E:{const char*_tmp505;void*_tmp504;(
_tmp504=0,Cyc_fprintf(Cyc_stderr,((_tmp505="Error: unexpected UniqueRgn\n",
_tag_dyneither(_tmp505,sizeof(char),29))),_tag_dyneither(_tmp504,sizeof(void*),0)));}
exit(1);return;_LL14F: {struct Cyc_Absyn_AccessEff_struct*_tmp15C=(struct Cyc_Absyn_AccessEff_struct*)
_tmp137;if(_tmp15C->tag != 23)goto _LL151;}_LL150:{const char*_tmp508;void*_tmp507;(
_tmp507=0,Cyc_fprintf(Cyc_stderr,((_tmp508="Error: unexpected AccessEff\n",
_tag_dyneither(_tmp508,sizeof(char),29))),_tag_dyneither(_tmp507,sizeof(void*),0)));}
exit(1);return;_LL151: {struct Cyc_Absyn_JoinEff_struct*_tmp15D=(struct Cyc_Absyn_JoinEff_struct*)
_tmp137;if(_tmp15D->tag != 24)goto _LL153;}_LL152:{const char*_tmp50B;void*_tmp50A;(
_tmp50A=0,Cyc_fprintf(Cyc_stderr,((_tmp50B="Error: unexpected JoinEff\n",
_tag_dyneither(_tmp50B,sizeof(char),27))),_tag_dyneither(_tmp50A,sizeof(void*),0)));}
exit(1);return;_LL153: {struct Cyc_Absyn_RgnsEff_struct*_tmp15E=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp137;if(_tmp15E->tag != 25)goto _LL155;}_LL154:{const char*_tmp50E;void*_tmp50D;(
_tmp50D=0,Cyc_fprintf(Cyc_stderr,((_tmp50E="Error: unexpected RgnsEff\n",
_tag_dyneither(_tmp50E,sizeof(char),27))),_tag_dyneither(_tmp50D,sizeof(void*),0)));}
exit(1);return;_LL155: {struct Cyc_Absyn_TagType_struct*_tmp15F=(struct Cyc_Absyn_TagType_struct*)
_tmp137;if(_tmp15F->tag != 20)goto _LL157;}_LL156:{const char*_tmp511;void*_tmp510;(
_tmp510=0,Cyc_fprintf(Cyc_stderr,((_tmp511="Error: unexpected tag_t\n",
_tag_dyneither(_tmp511,sizeof(char),25))),_tag_dyneither(_tmp510,sizeof(void*),0)));}
exit(1);return;_LL157: {struct Cyc_Absyn_ValueofType_struct*_tmp160=(struct Cyc_Absyn_ValueofType_struct*)
_tmp137;if(_tmp160->tag != 19)goto _LL124;}_LL158:{const char*_tmp514;void*_tmp513;(
_tmp513=0,Cyc_fprintf(Cyc_stderr,((_tmp514="Error: unexpected valueof_t\n",
_tag_dyneither(_tmp514,sizeof(char),29))),_tag_dyneither(_tmp513,sizeof(void*),0)));}
exit(1);return;_LL124:;}struct _tuple22{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple17*Cyc_scan_decl(struct Cyc_Absyn_Decl*d){{struct Cyc_Set_Set**
_tmp515;Cyc_current_targets=((_tmp515=_cycalloc(sizeof(*_tmp515)),((_tmp515[0]=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Set_empty)(Cyc_strptrcmp),_tmp515))));}{void*_tmp186=d->r;struct Cyc_Absyn_Vardecl*
_tmp188;struct Cyc_Absyn_Fndecl*_tmp18A;struct Cyc_Absyn_Aggrdecl*_tmp18C;struct
Cyc_Absyn_Enumdecl*_tmp18E;struct Cyc_Absyn_Typedefdecl*_tmp190;_LL15A: {struct
Cyc_Absyn_Var_d_struct*_tmp187=(struct Cyc_Absyn_Var_d_struct*)_tmp186;if(_tmp187->tag
!= 0)goto _LL15C;else{_tmp188=_tmp187->f1;}}_LL15B: {struct _tuple0 _tmp19D;struct
_dyneither_ptr*_tmp19E;struct _tuple0*_tmp19C=_tmp188->name;_tmp19D=*_tmp19C;
_tmp19E=_tmp19D.f2;Cyc_current_source=(struct _dyneither_ptr*)_tmp19E;Cyc_scan_type(
_tmp188->type);Cyc_scan_exp_opt(_tmp188->initializer);goto _LL159;}_LL15C: {
struct Cyc_Absyn_Fn_d_struct*_tmp189=(struct Cyc_Absyn_Fn_d_struct*)_tmp186;if(
_tmp189->tag != 1)goto _LL15E;else{_tmp18A=_tmp189->f1;}}_LL15D: {struct _tuple0
_tmp1A0;struct _dyneither_ptr*_tmp1A1;struct _tuple0*_tmp19F=_tmp18A->name;_tmp1A0=*
_tmp19F;_tmp1A1=_tmp1A0.f2;Cyc_current_source=(struct _dyneither_ptr*)_tmp1A1;Cyc_scan_type(
_tmp18A->ret_type);{struct Cyc_List_List*_tmp1A2=_tmp18A->args;for(0;_tmp1A2 != 0;
_tmp1A2=_tmp1A2->tl){struct _tuple22 _tmp1A4;void*_tmp1A5;struct _tuple22*_tmp1A3=(
struct _tuple22*)_tmp1A2->hd;_tmp1A4=*_tmp1A3;_tmp1A5=_tmp1A4.f3;Cyc_scan_type(
_tmp1A5);}}if(_tmp18A->cyc_varargs != 0)Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp18A->cyc_varargs))->type);if(_tmp18A->is_inline){const char*
_tmp518;void*_tmp517;(_tmp517=0,Cyc_fprintf(Cyc_stderr,((_tmp518="Warning: ignoring inline function\n",
_tag_dyneither(_tmp518,sizeof(char),35))),_tag_dyneither(_tmp517,sizeof(void*),0)));}
goto _LL159;}_LL15E: {struct Cyc_Absyn_Aggr_d_struct*_tmp18B=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp186;if(_tmp18B->tag != 6)goto _LL160;else{_tmp18C=_tmp18B->f1;}}_LL15F: {
struct _tuple0 _tmp1A9;struct _dyneither_ptr*_tmp1AA;struct _tuple0*_tmp1A8=_tmp18C->name;
_tmp1A9=*_tmp1A8;_tmp1AA=_tmp1A9.f2;Cyc_current_source=(struct _dyneither_ptr*)
_tmp1AA;if((unsigned int)_tmp18C->impl){{struct Cyc_List_List*_tmp1AB=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp18C->impl))->fields;for(0;_tmp1AB != 0;_tmp1AB=_tmp1AB->tl){
struct Cyc_Absyn_Aggrfield*_tmp1AC=(struct Cyc_Absyn_Aggrfield*)_tmp1AB->hd;Cyc_scan_type(
_tmp1AC->type);Cyc_scan_exp_opt(_tmp1AC->width);}}{struct Cyc_List_List*_tmp1AD=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp18C->impl))->fields;for(0;_tmp1AD
!= 0;_tmp1AD=_tmp1AD->tl){;}};}goto _LL159;}_LL160: {struct Cyc_Absyn_Enum_d_struct*
_tmp18D=(struct Cyc_Absyn_Enum_d_struct*)_tmp186;if(_tmp18D->tag != 8)goto _LL162;
else{_tmp18E=_tmp18D->f1;}}_LL161: {struct _tuple0 _tmp1AF;struct _dyneither_ptr*
_tmp1B0;struct _tuple0*_tmp1AE=_tmp18E->name;_tmp1AF=*_tmp1AE;_tmp1B0=_tmp1AF.f2;
Cyc_current_source=(struct _dyneither_ptr*)_tmp1B0;if((unsigned int)_tmp18E->fields){{
struct Cyc_List_List*_tmp1B1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp18E->fields))->v;for(0;_tmp1B1 != 0;_tmp1B1=_tmp1B1->tl){struct
Cyc_Absyn_Enumfield*_tmp1B2=(struct Cyc_Absyn_Enumfield*)_tmp1B1->hd;Cyc_scan_exp_opt(
_tmp1B2->tag);}}{struct Cyc_List_List*_tmp1B3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp18E->fields))->v;for(0;_tmp1B3 != 0;_tmp1B3=_tmp1B3->tl){;}};}
goto _LL159;}_LL162: {struct Cyc_Absyn_Typedef_d_struct*_tmp18F=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp186;if(_tmp18F->tag != 9)goto _LL164;else{_tmp190=_tmp18F->f1;}}_LL163: {
struct _tuple0 _tmp1B5;struct _dyneither_ptr*_tmp1B6;struct _tuple0*_tmp1B4=_tmp190->name;
_tmp1B5=*_tmp1B4;_tmp1B6=_tmp1B5.f2;Cyc_current_source=(struct _dyneither_ptr*)
_tmp1B6;if((unsigned int)_tmp190->defn)Cyc_scan_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp190->defn))->v);goto _LL159;}_LL164: {struct Cyc_Absyn_Region_d_struct*
_tmp191=(struct Cyc_Absyn_Region_d_struct*)_tmp186;if(_tmp191->tag != 4)goto _LL166;}
_LL165:{const char*_tmp51B;void*_tmp51A;(_tmp51A=0,Cyc_fprintf(Cyc_stderr,((
_tmp51B="Error: unexpected region declaration",_tag_dyneither(_tmp51B,sizeof(
char),37))),_tag_dyneither(_tmp51A,sizeof(void*),0)));}exit(1);_LL166: {struct
Cyc_Absyn_Alias_d_struct*_tmp192=(struct Cyc_Absyn_Alias_d_struct*)_tmp186;if(
_tmp192->tag != 5)goto _LL168;}_LL167:{const char*_tmp51E;void*_tmp51D;(_tmp51D=0,
Cyc_fprintf(Cyc_stderr,((_tmp51E="Error: unexpected alias declaration",
_tag_dyneither(_tmp51E,sizeof(char),36))),_tag_dyneither(_tmp51D,sizeof(void*),0)));}
exit(1);_LL168: {struct Cyc_Absyn_Porton_d_struct*_tmp193=(struct Cyc_Absyn_Porton_d_struct*)
_tmp186;if(_tmp193->tag != 14)goto _LL16A;}_LL169:{const char*_tmp521;void*_tmp520;(
_tmp520=0,Cyc_fprintf(Cyc_stderr,((_tmp521="Error: unexpected __cyclone_port_on__",
_tag_dyneither(_tmp521,sizeof(char),38))),_tag_dyneither(_tmp520,sizeof(void*),0)));}
exit(1);return 0;_LL16A: {struct Cyc_Absyn_Portoff_d_struct*_tmp194=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp186;if(_tmp194->tag != 15)goto _LL16C;}_LL16B:{const char*_tmp524;void*_tmp523;(
_tmp523=0,Cyc_fprintf(Cyc_stderr,((_tmp524="Error: unexpected __cyclone_port_off__",
_tag_dyneither(_tmp524,sizeof(char),39))),_tag_dyneither(_tmp523,sizeof(void*),0)));}
exit(1);return 0;_LL16C: {struct Cyc_Absyn_Let_d_struct*_tmp195=(struct Cyc_Absyn_Let_d_struct*)
_tmp186;if(_tmp195->tag != 2)goto _LL16E;}_LL16D:{const char*_tmp527;void*_tmp526;(
_tmp526=0,Cyc_fprintf(Cyc_stderr,((_tmp527="Error: unexpected let declaration\n",
_tag_dyneither(_tmp527,sizeof(char),35))),_tag_dyneither(_tmp526,sizeof(void*),0)));}
exit(1);return 0;_LL16E: {struct Cyc_Absyn_Datatype_d_struct*_tmp196=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp186;if(_tmp196->tag != 7)goto _LL170;}_LL16F:{const char*_tmp52A;void*_tmp529;(
_tmp529=0,Cyc_fprintf(Cyc_stderr,((_tmp52A="Error: unexpected datatype declaration\n",
_tag_dyneither(_tmp52A,sizeof(char),40))),_tag_dyneither(_tmp529,sizeof(void*),0)));}
exit(1);return 0;_LL170: {struct Cyc_Absyn_Letv_d_struct*_tmp197=(struct Cyc_Absyn_Letv_d_struct*)
_tmp186;if(_tmp197->tag != 3)goto _LL172;}_LL171:{const char*_tmp52D;void*_tmp52C;(
_tmp52C=0,Cyc_fprintf(Cyc_stderr,((_tmp52D="Error: unexpected let declaration\n",
_tag_dyneither(_tmp52D,sizeof(char),35))),_tag_dyneither(_tmp52C,sizeof(void*),0)));}
exit(1);return 0;_LL172: {struct Cyc_Absyn_Namespace_d_struct*_tmp198=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp186;if(_tmp198->tag != 10)goto _LL174;}_LL173:{const char*_tmp530;void*_tmp52F;(
_tmp52F=0,Cyc_fprintf(Cyc_stderr,((_tmp530="Error: unexpected namespace declaration\n",
_tag_dyneither(_tmp530,sizeof(char),41))),_tag_dyneither(_tmp52F,sizeof(void*),0)));}
exit(1);return 0;_LL174: {struct Cyc_Absyn_Using_d_struct*_tmp199=(struct Cyc_Absyn_Using_d_struct*)
_tmp186;if(_tmp199->tag != 11)goto _LL176;}_LL175:{const char*_tmp533;void*_tmp532;(
_tmp532=0,Cyc_fprintf(Cyc_stderr,((_tmp533="Error: unexpected using declaration\n",
_tag_dyneither(_tmp533,sizeof(char),37))),_tag_dyneither(_tmp532,sizeof(void*),0)));}
exit(1);return 0;_LL176: {struct Cyc_Absyn_ExternC_d_struct*_tmp19A=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp186;if(_tmp19A->tag != 12)goto _LL178;}_LL177:{const char*_tmp536;void*_tmp535;(
_tmp535=0,Cyc_fprintf(Cyc_stderr,((_tmp536="Error: unexpected extern \"C\" declaration\n",
_tag_dyneither(_tmp536,sizeof(char),42))),_tag_dyneither(_tmp535,sizeof(void*),0)));}
exit(1);return 0;_LL178: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp19B=(struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp186;if(_tmp19B->tag != 13)goto _LL159;}
_LL179:{const char*_tmp539;void*_tmp538;(_tmp538=0,Cyc_fprintf(Cyc_stderr,((
_tmp539="Error: unexpected extern \"C include\" declaration\n",_tag_dyneither(
_tmp539,sizeof(char),50))),_tag_dyneither(_tmp538,sizeof(void*),0)));}exit(1);
return 0;_LL159:;}{struct _tuple17*_tmp53A;return(_tmp53A=_cycalloc(sizeof(*
_tmp53A)),((_tmp53A->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source),((
_tmp53A->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),_tmp53A)))));};}
struct Cyc_Hashtable_Table*Cyc_new_deps(){return((struct Cyc_Hashtable_Table*(*)(
int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct
_dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp1CE;_push_handler(& _tmp1CE);{int _tmp1D0=0;if(setjmp(
_tmp1CE.handler))_tmp1D0=1;if(!_tmp1D0){{struct Cyc_Set_Set*_tmp1D1=((struct Cyc_Set_Set*(*)(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);
_npop_handler(0);return _tmp1D1;};_pop_handler();}else{void*_tmp1CF=(void*)
_exn_thrown;void*_tmp1D3=_tmp1CF;_LL17B: {struct Cyc_Core_Not_found_struct*
_tmp1D4=(struct Cyc_Core_Not_found_struct*)_tmp1D3;if(_tmp1D4->tag != Cyc_Core_Not_found)
goto _LL17D;}_LL17C: return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_LL17D:;_LL17E:(void)
_throw(_tmp1D3);_LL17A:;}};}struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*
init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;for(curr=emptyset;init != 0;init=init->tl){curr=((struct
Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(
curr,(struct _dyneither_ptr*)init->hd);}{struct Cyc_Set_Set*_tmp1D5=curr;struct
_dyneither_ptr*_tmp1D6=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",
sizeof(char),1);while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1D5)
> 0){struct Cyc_Set_Set*_tmp1D7=emptyset;struct Cyc_Iter_Iter _tmp1D8=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,
_tmp1D5);while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(
_tmp1D8,& _tmp1D6)){_tmp1D7=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct
Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp1D7,Cyc_find(t,_tmp1D6));}_tmp1D5=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(
_tmp1D7,curr);curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(curr,_tmp1D5);}return curr;};}enum Cyc_buildlib_mode{Cyc_NORMAL
 = 0,Cyc_GATHER  = 1,Cyc_GATHERSCRIPT  = 2,Cyc_FINISH  = 3};static enum Cyc_buildlib_mode
Cyc_mode=Cyc_NORMAL;static int Cyc_gathering(){return Cyc_mode == Cyc_GATHER  || Cyc_mode
== Cyc_GATHERSCRIPT;}static struct Cyc___cycFILE*Cyc_script_file=0;int Cyc_prscript(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){if(Cyc_script_file == 0){{const
char*_tmp53D;void*_tmp53C;(_tmp53C=0,Cyc_fprintf(Cyc_stderr,((_tmp53D="Internal error: script file is NULL\n",
_tag_dyneither(_tmp53D,sizeof(char),37))),_tag_dyneither(_tmp53C,sizeof(void*),0)));}
exit(1);}return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),
fmt,ap);}int Cyc_force_directory(struct _dyneither_ptr d){if(Cyc_mode == Cyc_GATHERSCRIPT){
const char*_tmp542;void*_tmp541[2];struct Cyc_String_pa_struct _tmp540;struct Cyc_String_pa_struct
_tmp53F;(_tmp53F.tag=0,((_tmp53F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
d),((_tmp540.tag=0,((_tmp540.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((
_tmp541[0]=& _tmp540,((_tmp541[1]=& _tmp53F,Cyc_prscript(((_tmp542="if ! test -e %s; then mkdir %s; fi\n",
_tag_dyneither(_tmp542,sizeof(char),36))),_tag_dyneither(_tmp541,sizeof(void*),2)))))))))))));}
else{unsigned short _tmp543[0];int _tmp1E0=Cyc_open((const char*)_check_null(
_untag_dyneither_ptr(d,sizeof(char),1)),0,_tag_dyneither(_tmp543,sizeof(
unsigned short),0));if(_tmp1E0 == - 1){if(mkdir((const char*)_untag_dyneither_ptr(d,
sizeof(char),1),448)== - 1){{const char*_tmp547;void*_tmp546[1];struct Cyc_String_pa_struct
_tmp545;(_tmp545.tag=0,((_tmp545.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
d),((_tmp546[0]=& _tmp545,Cyc_fprintf(Cyc_stderr,((_tmp547="Error: could not create directory %s\n",
_tag_dyneither(_tmp547,sizeof(char),38))),_tag_dyneither(_tmp546,sizeof(void*),1)))))));}
return 1;}}else{close(_tmp1E0);}}return 0;}int Cyc_force_directory_prefixes(struct
_dyneither_ptr file){struct _dyneither_ptr _tmp1E5=Cyc_strdup((struct _dyneither_ptr)
file);struct Cyc_List_List*_tmp1E6=0;while(1){_tmp1E5=Cyc_Filename_dirname((
struct _dyneither_ptr)_tmp1E5);if(Cyc_strlen((struct _dyneither_ptr)_tmp1E5)== 0)
break;{struct _dyneither_ptr*_tmp54A;struct Cyc_List_List*_tmp549;_tmp1E6=((
_tmp549=_cycalloc(sizeof(*_tmp549)),((_tmp549->hd=((_tmp54A=_cycalloc(sizeof(*
_tmp54A)),((_tmp54A[0]=(struct _dyneither_ptr)_tmp1E5,_tmp54A)))),((_tmp549->tl=
_tmp1E6,_tmp549))))));};}for(0;_tmp1E6 != 0;_tmp1E6=_tmp1E6->tl){if(Cyc_force_directory(*((
struct _dyneither_ptr*)_tmp1E6->hd)))return 1;}return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";
struct Cyc_NO_SUPPORT_struct{char*tag;struct _dyneither_ptr f1;};struct _tuple23{
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};int Cyc_process_file(const char*
filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,
struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*
cycstubs){struct Cyc___cycFILE*maybe;struct Cyc___cycFILE*in_file;struct Cyc___cycFILE*
out_file;int errorcode=0;{const char*_tmp550;void*_tmp54F[1];const char*_tmp54E;
struct Cyc_String_pa_struct _tmp54D;(_tmp54D.tag=0,((_tmp54D.f1=(struct
_dyneither_ptr)((_tmp54E=filename,_tag_dyneither(_tmp54E,sizeof(char),
_get_zero_arr_size_char((void*)_tmp54E,1)))),((_tmp54F[0]=& _tmp54D,Cyc_fprintf(
Cyc_stderr,((_tmp550="********************************* %s...\n",_tag_dyneither(
_tmp550,sizeof(char),41))),_tag_dyneither(_tmp54F,sizeof(void*),1)))))));}if(!
Cyc_gathering()){const char*_tmp556;void*_tmp555[1];const char*_tmp554;struct Cyc_String_pa_struct
_tmp553;(_tmp553.tag=0,((_tmp553.f1=(struct _dyneither_ptr)((_tmp554=filename,
_tag_dyneither(_tmp554,sizeof(char),_get_zero_arr_size_char((void*)_tmp554,1)))),((
_tmp555[0]=& _tmp553,Cyc_log(((_tmp556="\n%s:\n",_tag_dyneither(_tmp556,sizeof(
char),6))),_tag_dyneither(_tmp555,sizeof(void*),1)))))));}{const char*_tmp557;
struct _dyneither_ptr _tmp1F1=Cyc_Filename_basename(((_tmp557=filename,
_tag_dyneither(_tmp557,sizeof(char),_get_zero_arr_size_char((void*)_tmp557,1)))));
const char*_tmp558;struct _dyneither_ptr _tmp1F2=Cyc_Filename_dirname(((_tmp558=
filename,_tag_dyneither(_tmp558,sizeof(char),_get_zero_arr_size_char((void*)
_tmp558,1)))));struct _dyneither_ptr _tmp1F3=Cyc_Filename_chop_extension((struct
_dyneither_ptr)_tmp1F1);const char*_tmp559;const char*_tmp1F4=(const char*)
_untag_dyneither_ptr(Cyc_strconcat((struct _dyneither_ptr)_tmp1F3,((_tmp559=".iA",
_tag_dyneither(_tmp559,sizeof(char),4)))),sizeof(char),1);const char*_tmp561;void*
_tmp560[1];struct Cyc_String_pa_struct _tmp55F;const char*_tmp55D;void*_tmp55C[1];
struct Cyc_String_pa_struct _tmp55B;const char*_tmp1F5=(const char*)
_untag_dyneither_ptr(_get_dyneither_size(_tmp1F2,sizeof(char))== 0?(_tmp55F.tag=
0,((_tmp55F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1F3),((_tmp560[0]=&
_tmp55F,Cyc_aprintf(((_tmp561="%s.iB",_tag_dyneither(_tmp561,sizeof(char),6))),
_tag_dyneither(_tmp560,sizeof(void*),1))))))): Cyc_Filename_concat((struct
_dyneither_ptr)_tmp1F2,(struct _dyneither_ptr)((_tmp55B.tag=0,((_tmp55B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1F3),((_tmp55C[0]=& _tmp55B,Cyc_aprintf(((
_tmp55D="%s.iB",_tag_dyneither(_tmp55D,sizeof(char),6))),_tag_dyneither(_tmp55C,
sizeof(void*),1))))))))),sizeof(char),1);const char*_tmp569;void*_tmp568[1];
struct Cyc_String_pa_struct _tmp567;const char*_tmp565;void*_tmp564[1];struct Cyc_String_pa_struct
_tmp563;const char*_tmp1F6=(const char*)_untag_dyneither_ptr(_get_dyneither_size(
_tmp1F2,sizeof(char))== 0?(_tmp567.tag=0,((_tmp567.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp1F3),((_tmp568[0]=& _tmp567,Cyc_aprintf(((_tmp569="%s.iC",
_tag_dyneither(_tmp569,sizeof(char),6))),_tag_dyneither(_tmp568,sizeof(void*),1))))))):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp1F2,(struct _dyneither_ptr)((
_tmp563.tag=0,((_tmp563.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1F3),((
_tmp564[0]=& _tmp563,Cyc_aprintf(((_tmp565="%s.iC",_tag_dyneither(_tmp565,sizeof(
char),6))),_tag_dyneither(_tmp564,sizeof(void*),1))))))))),sizeof(char),1);const
char*_tmp571;void*_tmp570[1];struct Cyc_String_pa_struct _tmp56F;const char*_tmp56D;
void*_tmp56C[1];struct Cyc_String_pa_struct _tmp56B;const char*_tmp1F7=(const char*)
_untag_dyneither_ptr(_get_dyneither_size(_tmp1F2,sizeof(char))== 0?(_tmp56F.tag=
0,((_tmp56F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1F3),((_tmp570[0]=&
_tmp56F,Cyc_aprintf(((_tmp571="%s.iD",_tag_dyneither(_tmp571,sizeof(char),6))),
_tag_dyneither(_tmp570,sizeof(void*),1))))))): Cyc_Filename_concat((struct
_dyneither_ptr)_tmp1F2,(struct _dyneither_ptr)((_tmp56B.tag=0,((_tmp56B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1F3),((_tmp56C[0]=& _tmp56B,Cyc_aprintf(((
_tmp56D="%s.iD",_tag_dyneither(_tmp56D,sizeof(char),6))),_tag_dyneither(_tmp56C,
sizeof(void*),1))))))))),sizeof(char),1);struct _handler_cons _tmp1F8;
_push_handler(& _tmp1F8);{int _tmp1FA=0;if(setjmp(_tmp1F8.handler))_tmp1FA=1;if(!
_tmp1FA){{const char*_tmp572;if(Cyc_force_directory_prefixes(((_tmp572=filename,
_tag_dyneither(_tmp572,sizeof(char),_get_zero_arr_size_char((void*)_tmp572,1)))))){
int _tmp1FC=1;_npop_handler(0);return _tmp1FC;}}if(Cyc_mode != Cyc_FINISH){Cyc_current_cpp=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);
if(Cyc_mode == Cyc_GATHERSCRIPT){{const char*_tmp578;void*_tmp577[1];const char*
_tmp576;struct Cyc_String_pa_struct _tmp575;(_tmp575.tag=0,((_tmp575.f1=(struct
_dyneither_ptr)((_tmp576=_tmp1F4,_tag_dyneither(_tmp576,sizeof(char),
_get_zero_arr_size_char((void*)_tmp576,1)))),((_tmp577[0]=& _tmp575,Cyc_prscript(((
_tmp578="cat >%s <<XXX\n",_tag_dyneither(_tmp578,sizeof(char),15))),
_tag_dyneither(_tmp577,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp201=Cyc_current_cpp;
for(0;_tmp201 != 0;_tmp201=_tmp201->tl){const char*_tmp57C;void*_tmp57B[1];struct
Cyc_String_pa_struct _tmp57A;(_tmp57A.tag=0,((_tmp57A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp201->hd)),((_tmp57B[0]=&
_tmp57A,Cyc_prscript(((_tmp57C="%s",_tag_dyneither(_tmp57C,sizeof(char),3))),
_tag_dyneither(_tmp57B,sizeof(void*),1)))))));}}{const char*_tmp582;void*_tmp581[
1];const char*_tmp580;struct Cyc_String_pa_struct _tmp57F;(_tmp57F.tag=0,((_tmp57F.f1=(
struct _dyneither_ptr)((_tmp580=filename,_tag_dyneither(_tmp580,sizeof(char),
_get_zero_arr_size_char((void*)_tmp580,1)))),((_tmp581[0]=& _tmp57F,Cyc_prscript(((
_tmp582="#include <%s>\n",_tag_dyneither(_tmp582,sizeof(char),15))),
_tag_dyneither(_tmp581,sizeof(void*),1)))))));}{const char*_tmp585;void*_tmp584;(
_tmp584=0,Cyc_prscript(((_tmp585="XXX\n",_tag_dyneither(_tmp585,sizeof(char),5))),
_tag_dyneither(_tmp584,sizeof(void*),0)));}{const char*_tmp58E;void*_tmp58D[2];
const char*_tmp58C;struct Cyc_String_pa_struct _tmp58B;const char*_tmp58A;struct Cyc_String_pa_struct
_tmp589;(_tmp589.tag=0,((_tmp589.f1=(struct _dyneither_ptr)((_tmp58A=_tmp1F4,
_tag_dyneither(_tmp58A,sizeof(char),_get_zero_arr_size_char((void*)_tmp58A,1)))),((
_tmp58B.tag=0,((_tmp58B.f1=(struct _dyneither_ptr)((_tmp58C=_tmp1F5,
_tag_dyneither(_tmp58C,sizeof(char),_get_zero_arr_size_char((void*)_tmp58C,1)))),((
_tmp58D[0]=& _tmp58B,((_tmp58D[1]=& _tmp589,Cyc_prscript(((_tmp58E="$GCC -E -dM -o %s -x c %s && \\\n",
_tag_dyneither(_tmp58E,sizeof(char),32))),_tag_dyneither(_tmp58D,sizeof(void*),2)))))))))))));}{
const char*_tmp597;void*_tmp596[2];const char*_tmp595;struct Cyc_String_pa_struct
_tmp594;const char*_tmp593;struct Cyc_String_pa_struct _tmp592;(_tmp592.tag=0,((
_tmp592.f1=(struct _dyneither_ptr)((_tmp593=_tmp1F4,_tag_dyneither(_tmp593,
sizeof(char),_get_zero_arr_size_char((void*)_tmp593,1)))),((_tmp594.tag=0,((
_tmp594.f1=(struct _dyneither_ptr)((_tmp595=_tmp1F6,_tag_dyneither(_tmp595,
sizeof(char),_get_zero_arr_size_char((void*)_tmp595,1)))),((_tmp596[0]=& _tmp594,((
_tmp596[1]=& _tmp592,Cyc_prscript(((_tmp597="$GCC -E     -o %s -x c %s;\n",
_tag_dyneither(_tmp597,sizeof(char),28))),_tag_dyneither(_tmp596,sizeof(void*),2)))))))))))));}{
const char*_tmp59D;void*_tmp59C[1];const char*_tmp59B;struct Cyc_String_pa_struct
_tmp59A;(_tmp59A.tag=0,((_tmp59A.f1=(struct _dyneither_ptr)((_tmp59B=_tmp1F4,
_tag_dyneither(_tmp59B,sizeof(char),_get_zero_arr_size_char((void*)_tmp59B,1)))),((
_tmp59C[0]=& _tmp59A,Cyc_prscript(((_tmp59D="rm %s\n",_tag_dyneither(_tmp59D,
sizeof(char),7))),_tag_dyneither(_tmp59C,sizeof(void*),1)))))));};}else{maybe=
Cyc_fopen(_tmp1F4,(const char*)"w");if(!((unsigned int)maybe)){{const char*_tmp5A3;
void*_tmp5A2[1];const char*_tmp5A1;struct Cyc_String_pa_struct _tmp5A0;(_tmp5A0.tag=
0,((_tmp5A0.f1=(struct _dyneither_ptr)((_tmp5A1=_tmp1F4,_tag_dyneither(_tmp5A1,
sizeof(char),_get_zero_arr_size_char((void*)_tmp5A1,1)))),((_tmp5A2[0]=& _tmp5A0,
Cyc_fprintf(Cyc_stderr,((_tmp5A3="Error: could not create file %s\n",
_tag_dyneither(_tmp5A3,sizeof(char),33))),_tag_dyneither(_tmp5A2,sizeof(void*),1)))))));}{
int _tmp21F=1;_npop_handler(0);return _tmp21F;};}out_file=(struct Cyc___cycFILE*)
maybe;{struct Cyc_List_List*_tmp220=Cyc_current_cpp;for(0;_tmp220 != 0;_tmp220=
_tmp220->tl){Cyc_fputs((const char*)_untag_dyneither_ptr(*((struct _dyneither_ptr*)
_tmp220->hd),sizeof(char),1),out_file);}}{const char*_tmp5A9;void*_tmp5A8[1];
const char*_tmp5A7;struct Cyc_String_pa_struct _tmp5A6;(_tmp5A6.tag=0,((_tmp5A6.f1=(
struct _dyneither_ptr)((_tmp5A7=filename,_tag_dyneither(_tmp5A7,sizeof(char),
_get_zero_arr_size_char((void*)_tmp5A7,1)))),((_tmp5A8[0]=& _tmp5A6,Cyc_fprintf(
out_file,((_tmp5A9="#include <%s>\n",_tag_dyneither(_tmp5A9,sizeof(char),15))),
_tag_dyneither(_tmp5A8,sizeof(void*),1)))))));}Cyc_fclose(out_file);{struct
_dyneither_ptr _tmp225=Cstring_to_string(Ccomp);const char*_tmp5B3;void*_tmp5B2[3];
struct Cyc_String_pa_struct _tmp5B1;const char*_tmp5B0;struct Cyc_String_pa_struct
_tmp5AF;const char*_tmp5AE;struct Cyc_String_pa_struct _tmp5AD;char*cmd=(char*)
_untag_dyneither_ptr(((_tmp5AD.tag=0,((_tmp5AD.f1=(struct _dyneither_ptr)((
_tmp5AE=_tmp1F4,_tag_dyneither(_tmp5AE,sizeof(char),_get_zero_arr_size_char((
void*)_tmp5AE,1)))),((_tmp5AF.tag=0,((_tmp5AF.f1=(struct _dyneither_ptr)((_tmp5B0=
_tmp1F5,_tag_dyneither(_tmp5B0,sizeof(char),_get_zero_arr_size_char((void*)
_tmp5B0,1)))),((_tmp5B1.tag=0,((_tmp5B1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp225),((_tmp5B2[0]=& _tmp5B1,((_tmp5B2[1]=& _tmp5AF,((_tmp5B2[2]=&
_tmp5AD,Cyc_aprintf(((_tmp5B3="%s -E -dM -o %s -x c %s",_tag_dyneither(_tmp5B3,
sizeof(char),24))),_tag_dyneither(_tmp5B2,sizeof(void*),3)))))))))))))))))))),
sizeof(char),1);if(!system((const char*)cmd)){{const char*_tmp5BD;void*_tmp5BC[3];
struct Cyc_String_pa_struct _tmp5BB;const char*_tmp5BA;struct Cyc_String_pa_struct
_tmp5B9;const char*_tmp5B8;struct Cyc_String_pa_struct _tmp5B7;cmd=(char*)
_untag_dyneither_ptr(((_tmp5B7.tag=0,((_tmp5B7.f1=(struct _dyneither_ptr)((
_tmp5B8=_tmp1F4,_tag_dyneither(_tmp5B8,sizeof(char),_get_zero_arr_size_char((
void*)_tmp5B8,1)))),((_tmp5B9.tag=0,((_tmp5B9.f1=(struct _dyneither_ptr)((_tmp5BA=
_tmp1F6,_tag_dyneither(_tmp5BA,sizeof(char),_get_zero_arr_size_char((void*)
_tmp5BA,1)))),((_tmp5BB.tag=0,((_tmp5BB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp225),((_tmp5BC[0]=& _tmp5BB,((_tmp5BC[1]=& _tmp5B9,((_tmp5BC[2]=&
_tmp5B7,Cyc_aprintf(((_tmp5BD="%s -E -o %s -x c %s",_tag_dyneither(_tmp5BD,
sizeof(char),20))),_tag_dyneither(_tmp5BC,sizeof(void*),3)))))))))))))))))))),
sizeof(char),1);}system((const char*)cmd);}remove(_tmp1F4);};}}if(Cyc_gathering()){
int _tmp234=0;_npop_handler(0);return _tmp234;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp1F5,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp5CE;const char*_tmp5CD;void*_tmp5CC[1];const char*_tmp5CB;struct Cyc_String_pa_struct
_tmp5CA;struct Cyc_NO_SUPPORT_struct*_tmp5C9;(int)_throw((void*)((_tmp5C9=
_cycalloc(sizeof(*_tmp5C9)),((_tmp5C9[0]=((_tmp5CE.tag=Cyc_NO_SUPPORT,((_tmp5CE.f1=(
struct _dyneither_ptr)((_tmp5CA.tag=0,((_tmp5CA.f1=(struct _dyneither_ptr)((
_tmp5CB=_tmp1F5,_tag_dyneither(_tmp5CB,sizeof(char),_get_zero_arr_size_char((
void*)_tmp5CB,1)))),((_tmp5CC[0]=& _tmp5CA,Cyc_aprintf(((_tmp5CD="can't open macrosfile %s",
_tag_dyneither(_tmp5CD,sizeof(char),25))),_tag_dyneither(_tmp5CC,sizeof(void*),1)))))))),
_tmp5CE)))),_tmp5C9)))));}in_file=(struct Cyc___cycFILE*)maybe;{struct Cyc_Lexing_lexbuf*
_tmp23B=Cyc_Lexing_from_file(in_file);struct _tuple17*entry;while((entry=((struct
_tuple17*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp23B))!= 0){struct
_tuple17 _tmp23D;struct _dyneither_ptr*_tmp23E;struct Cyc_Set_Set*_tmp23F;struct
_tuple17*_tmp23C=(struct _tuple17*)_check_null(entry);_tmp23D=*_tmp23C;_tmp23E=
_tmp23D.f1;_tmp23F=_tmp23D.f2;((void(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp23E,
_tmp23F);}Cyc_fclose(in_file);maybe=Cyc_fopen(_tmp1F6,(const char*)"r");if(!((
unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct _tmp5DF;const char*_tmp5DE;void*
_tmp5DD[1];const char*_tmp5DC;struct Cyc_String_pa_struct _tmp5DB;struct Cyc_NO_SUPPORT_struct*
_tmp5DA;(int)_throw((void*)((_tmp5DA=_cycalloc(sizeof(*_tmp5DA)),((_tmp5DA[0]=((
_tmp5DF.tag=Cyc_NO_SUPPORT,((_tmp5DF.f1=(struct _dyneither_ptr)((_tmp5DB.tag=0,((
_tmp5DB.f1=(struct _dyneither_ptr)((_tmp5DC=_tmp1F6,_tag_dyneither(_tmp5DC,
sizeof(char),_get_zero_arr_size_char((void*)_tmp5DC,1)))),((_tmp5DD[0]=& _tmp5DB,
Cyc_aprintf(((_tmp5DE="can't open declsfile %s",_tag_dyneither(_tmp5DE,sizeof(
char),24))),_tag_dyneither(_tmp5DD,sizeof(void*),1)))))))),_tmp5DF)))),_tmp5DA)))));}
in_file=(struct Cyc___cycFILE*)maybe;_tmp23B=Cyc_Lexing_from_file(in_file);Cyc_slurp_out=
Cyc_fopen(_tmp1F7,(const char*)"w");if(!((unsigned int)Cyc_slurp_out)){int _tmp246=
1;_npop_handler(0);return _tmp246;}while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))
Cyc_slurp)(_tmp23B)){;}Cyc_fclose(in_file);Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));if(Cyc_mode != Cyc_FINISH)remove(_tmp1F6);maybe=Cyc_fopen(
_tmp1F7,(const char*)"r");if(!((unsigned int)maybe)){int _tmp247=1;_npop_handler(0);
return _tmp247;}in_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp5E0;Cyc_Position_reset_position(((
_tmp5E0=_tmp1F7,_tag_dyneither(_tmp5E0,sizeof(char),_get_zero_arr_size_char((
void*)_tmp5E0,1)))));}Cyc_Lex_lex_init(0);{struct Cyc_List_List*_tmp249=Cyc_Parse_parse_file(
in_file);Cyc_Lex_lex_init(0);Cyc_fclose(in_file);remove(_tmp1F7);{struct Cyc_List_List*
_tmp24A=_tmp249;for(0;_tmp24A != 0;_tmp24A=_tmp24A->tl){struct _tuple17*_tmp24B=
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp24A->hd);if(_tmp24B == 0)continue;{
struct _tuple17 _tmp24D;struct _dyneither_ptr*_tmp24E;struct Cyc_Set_Set*_tmp24F;
struct _tuple17*_tmp24C=(struct _tuple17*)_tmp24B;_tmp24D=*_tmp24C;_tmp24E=_tmp24D.f1;
_tmp24F=_tmp24D.f2;{struct Cyc_Set_Set*old;{struct _handler_cons _tmp250;
_push_handler(& _tmp250);{int _tmp252=0;if(setjmp(_tmp250.handler))_tmp252=1;if(!
_tmp252){old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key))Cyc_Hashtable_lookup)(t,_tmp24E);;_pop_handler();}else{void*
_tmp251=(void*)_exn_thrown;void*_tmp254=_tmp251;_LL180: {struct Cyc_Core_Not_found_struct*
_tmp255=(struct Cyc_Core_Not_found_struct*)_tmp254;if(_tmp255->tag != Cyc_Core_Not_found)
goto _LL182;}_LL181: old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL17F;_LL182:;_LL183:(
void)_throw(_tmp254);_LL17F:;}};}((void(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp24E,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp24F,old));};};}}{struct Cyc_Set_Set*_tmp256=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp257=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);{
struct Cyc_List_List*_tmp258=_tmp249;for(0;_tmp258 != 0;_tmp258=_tmp258->tl){
struct Cyc_Absyn_Decl*_tmp259=(struct Cyc_Absyn_Decl*)_tmp258->hd;struct
_dyneither_ptr*name;{void*_tmp25A=_tmp259->r;struct Cyc_Absyn_Vardecl*_tmp25C;
struct Cyc_Absyn_Fndecl*_tmp25E;struct Cyc_Absyn_Aggrdecl*_tmp260;struct Cyc_Absyn_Enumdecl*
_tmp262;struct Cyc_Absyn_Typedefdecl*_tmp264;_LL185: {struct Cyc_Absyn_Var_d_struct*
_tmp25B=(struct Cyc_Absyn_Var_d_struct*)_tmp25A;if(_tmp25B->tag != 0)goto _LL187;
else{_tmp25C=_tmp25B->f1;}}_LL186: {struct _tuple0 _tmp271;struct _dyneither_ptr*
_tmp272;struct _tuple0*_tmp270=_tmp25C->name;_tmp271=*_tmp270;_tmp272=_tmp271.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_insert)(defined_symbols,_tmp272);if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp272))name=0;else{
name=(struct _dyneither_ptr*)_tmp272;}goto _LL184;}_LL187: {struct Cyc_Absyn_Fn_d_struct*
_tmp25D=(struct Cyc_Absyn_Fn_d_struct*)_tmp25A;if(_tmp25D->tag != 1)goto _LL189;
else{_tmp25E=_tmp25D->f1;}}_LL188: {struct _tuple0 _tmp274;struct _dyneither_ptr*
_tmp275;struct _tuple0*_tmp273=_tmp25E->name;_tmp274=*_tmp273;_tmp275=_tmp274.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_insert)(defined_symbols,_tmp275);if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp275))name=0;else{
name=(struct _dyneither_ptr*)_tmp275;}goto _LL184;}_LL189: {struct Cyc_Absyn_Aggr_d_struct*
_tmp25F=(struct Cyc_Absyn_Aggr_d_struct*)_tmp25A;if(_tmp25F->tag != 6)goto _LL18B;
else{_tmp260=_tmp25F->f1;}}_LL18A: {struct _tuple0 _tmp277;struct _dyneither_ptr*
_tmp278;struct _tuple0*_tmp276=_tmp260->name;_tmp277=*_tmp276;_tmp278=_tmp277.f2;
name=(struct _dyneither_ptr*)_tmp278;goto _LL184;}_LL18B: {struct Cyc_Absyn_Enum_d_struct*
_tmp261=(struct Cyc_Absyn_Enum_d_struct*)_tmp25A;if(_tmp261->tag != 8)goto _LL18D;
else{_tmp262=_tmp261->f1;}}_LL18C: {struct _tuple0 _tmp27A;struct _dyneither_ptr*
_tmp27B;struct _tuple0*_tmp279=_tmp262->name;_tmp27A=*_tmp279;_tmp27B=_tmp27A.f2;
name=(struct _dyneither_ptr*)_tmp27B;if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp256,(struct _dyneither_ptr*)name)){
struct Cyc_List_List*_tmp5E1;_tmp257=((_tmp5E1=_cycalloc(sizeof(*_tmp5E1)),((
_tmp5E1->hd=_tmp259,((_tmp5E1->tl=_tmp257,_tmp5E1))))));}else{if((unsigned int)
_tmp262->fields){struct Cyc_List_List*_tmp27D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp262->fields))->v;for(0;_tmp27D != 0;_tmp27D=_tmp27D->tl){struct
Cyc_Absyn_Enumfield*_tmp27E=(struct Cyc_Absyn_Enumfield*)_tmp27D->hd;struct
_tuple0 _tmp280;struct _dyneither_ptr*_tmp281;struct _tuple0*_tmp27F=_tmp27E->name;
_tmp280=*_tmp27F;_tmp281=_tmp280.f2;if(((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(_tmp256,_tmp281)){{struct Cyc_List_List*
_tmp5E2;_tmp257=((_tmp5E2=_cycalloc(sizeof(*_tmp5E2)),((_tmp5E2->hd=_tmp259,((
_tmp5E2->tl=_tmp257,_tmp5E2))))));}break;}}}}name=0;goto _LL184;}_LL18D: {struct
Cyc_Absyn_Typedef_d_struct*_tmp263=(struct Cyc_Absyn_Typedef_d_struct*)_tmp25A;
if(_tmp263->tag != 9)goto _LL18F;else{_tmp264=_tmp263->f1;}}_LL18E: {struct _tuple0
_tmp284;struct _dyneither_ptr*_tmp285;struct _tuple0*_tmp283=_tmp264->name;_tmp284=*
_tmp283;_tmp285=_tmp284.f2;name=(struct _dyneither_ptr*)_tmp285;goto _LL184;}
_LL18F: {struct Cyc_Absyn_Porton_d_struct*_tmp265=(struct Cyc_Absyn_Porton_d_struct*)
_tmp25A;if(_tmp265->tag != 14)goto _LL191;}_LL190: goto _LL192;_LL191: {struct Cyc_Absyn_Portoff_d_struct*
_tmp266=(struct Cyc_Absyn_Portoff_d_struct*)_tmp25A;if(_tmp266->tag != 15)goto
_LL193;}_LL192: goto _LL194;_LL193: {struct Cyc_Absyn_Let_d_struct*_tmp267=(struct
Cyc_Absyn_Let_d_struct*)_tmp25A;if(_tmp267->tag != 2)goto _LL195;}_LL194: goto
_LL196;_LL195: {struct Cyc_Absyn_Datatype_d_struct*_tmp268=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp25A;if(_tmp268->tag != 7)goto _LL197;}_LL196: goto _LL198;_LL197: {struct Cyc_Absyn_Letv_d_struct*
_tmp269=(struct Cyc_Absyn_Letv_d_struct*)_tmp25A;if(_tmp269->tag != 3)goto _LL199;}
_LL198: goto _LL19A;_LL199: {struct Cyc_Absyn_Namespace_d_struct*_tmp26A=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp25A;if(_tmp26A->tag != 10)goto _LL19B;}_LL19A: goto _LL19C;_LL19B: {struct Cyc_Absyn_Using_d_struct*
_tmp26B=(struct Cyc_Absyn_Using_d_struct*)_tmp25A;if(_tmp26B->tag != 11)goto _LL19D;}
_LL19C: goto _LL19E;_LL19D: {struct Cyc_Absyn_ExternC_d_struct*_tmp26C=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp25A;if(_tmp26C->tag != 12)goto _LL19F;}_LL19E: goto _LL1A0;_LL19F: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp26D=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp25A;if(_tmp26D->tag != 13)
goto _LL1A1;}_LL1A0: goto _LL1A2;_LL1A1: {struct Cyc_Absyn_Region_d_struct*_tmp26E=(
struct Cyc_Absyn_Region_d_struct*)_tmp25A;if(_tmp26E->tag != 4)goto _LL1A3;}_LL1A2:
goto _LL1A4;_LL1A3: {struct Cyc_Absyn_Alias_d_struct*_tmp26F=(struct Cyc_Absyn_Alias_d_struct*)
_tmp25A;if(_tmp26F->tag != 5)goto _LL184;}_LL1A4: name=0;goto _LL184;_LL184:;}if(
name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
_tmp256,(struct _dyneither_ptr*)name)){struct Cyc_List_List*_tmp5E3;_tmp257=((
_tmp5E3=_cycalloc(sizeof(*_tmp5E3)),((_tmp5E3->hd=_tmp259,((_tmp5E3->tl=_tmp257,
_tmp5E3))))));}}}if(!Cyc_do_setjmp){maybe=Cyc_fopen(filename,(const char*)"w");
if(!((unsigned int)maybe)){int _tmp287=1;_npop_handler(0);return _tmp287;}out_file=(
struct Cyc___cycFILE*)maybe;}else{out_file=Cyc_stdout;}{const char*_tmp5E9;void*
_tmp5E8[1];const char*_tmp5E7;struct Cyc_String_pa_struct _tmp5E6;struct
_dyneither_ptr ifdefmacro=(_tmp5E6.tag=0,((_tmp5E6.f1=(struct _dyneither_ptr)((
_tmp5E7=filename,_tag_dyneither(_tmp5E7,sizeof(char),_get_zero_arr_size_char((
void*)_tmp5E7,1)))),((_tmp5E8[0]=& _tmp5E6,Cyc_aprintf(((_tmp5E9="_%s_",
_tag_dyneither(_tmp5E9,sizeof(char),5))),_tag_dyneither(_tmp5E8,sizeof(void*),1)))))));{
int _tmp288=0;for(0;_tmp288 < _get_dyneither_size(ifdefmacro,sizeof(char));++
_tmp288){if(((char*)ifdefmacro.curr)[_tmp288]== '.'  || ((char*)ifdefmacro.curr)[
_tmp288]== '/'){char _tmp5EC;char _tmp5EB;struct _dyneither_ptr _tmp5EA;(_tmp5EA=
_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp288),((_tmp5EB=*((char*)
_check_dyneither_subscript(_tmp5EA,sizeof(char),0)),((_tmp5EC='_',((
_get_dyneither_size(_tmp5EA,sizeof(char))== 1  && (_tmp5EB == '\000'  && _tmp5EC != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5EA.curr)=_tmp5EC)))))));}else{if(((char*)
ifdefmacro.curr)[_tmp288]!= '_'  && ((char*)ifdefmacro.curr)[_tmp288]!= '/'){char
_tmp5EF;char _tmp5EE;struct _dyneither_ptr _tmp5ED;(_tmp5ED=_dyneither_ptr_plus(
ifdefmacro,sizeof(char),_tmp288),((_tmp5EE=*((char*)_check_dyneither_subscript(
_tmp5ED,sizeof(char),0)),((_tmp5EF=(char)toupper((int)((char*)ifdefmacro.curr)[
_tmp288]),((_get_dyneither_size(_tmp5ED,sizeof(char))== 1  && (_tmp5EE == '\000'
 && _tmp5EF != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5ED.curr)=_tmp5EF)))))));}}}}{
const char*_tmp5F4;void*_tmp5F3[2];struct Cyc_String_pa_struct _tmp5F2;struct Cyc_String_pa_struct
_tmp5F1;(_tmp5F1.tag=0,((_tmp5F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
ifdefmacro),((_tmp5F2.tag=0,((_tmp5F2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ifdefmacro),((_tmp5F3[0]=& _tmp5F2,((_tmp5F3[1]=& _tmp5F1,Cyc_fprintf(
out_file,((_tmp5F4="#ifndef %s\n#define %s\n",_tag_dyneither(_tmp5F4,sizeof(char),
23))),_tag_dyneither(_tmp5F3,sizeof(void*),2)))))))))))));}{struct Cyc_List_List*
_tmp293=0;struct Cyc_List_List*_tmp294=0;{struct Cyc_List_List*_tmp295=_tmp257;
for(0;_tmp295 != 0;_tmp295=_tmp295->tl){struct Cyc_Absyn_Decl*_tmp296=(struct Cyc_Absyn_Decl*)
_tmp295->hd;int _tmp297=0;struct _dyneither_ptr*name;{void*_tmp298=_tmp296->r;
struct Cyc_Absyn_Vardecl*_tmp29A;struct Cyc_Absyn_Fndecl*_tmp29C;struct Cyc_Absyn_Aggrdecl*
_tmp29E;struct Cyc_Absyn_Enumdecl*_tmp2A0;struct Cyc_Absyn_Typedefdecl*_tmp2A2;
_LL1A6: {struct Cyc_Absyn_Var_d_struct*_tmp299=(struct Cyc_Absyn_Var_d_struct*)
_tmp298;if(_tmp299->tag != 0)goto _LL1A8;else{_tmp29A=_tmp299->f1;}}_LL1A7: {
struct _tuple0 _tmp2AF;struct _dyneither_ptr*_tmp2B0;struct _tuple0*_tmp2AE=_tmp29A->name;
_tmp2AF=*_tmp2AE;_tmp2B0=_tmp2AF.f2;name=(struct _dyneither_ptr*)_tmp2B0;goto
_LL1A5;}_LL1A8: {struct Cyc_Absyn_Fn_d_struct*_tmp29B=(struct Cyc_Absyn_Fn_d_struct*)
_tmp298;if(_tmp29B->tag != 1)goto _LL1AA;else{_tmp29C=_tmp29B->f1;}}_LL1A9: {
struct _tuple0 _tmp2B2;struct _dyneither_ptr*_tmp2B3;struct _tuple0*_tmp2B1=_tmp29C->name;
_tmp2B2=*_tmp2B1;_tmp2B3=_tmp2B2.f2;name=(struct _dyneither_ptr*)_tmp2B3;goto
_LL1A5;}_LL1AA: {struct Cyc_Absyn_Aggr_d_struct*_tmp29D=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp298;if(_tmp29D->tag != 6)goto _LL1AC;else{_tmp29E=_tmp29D->f1;}}_LL1AB: {
struct _tuple0 _tmp2B5;struct _dyneither_ptr*_tmp2B6;struct _tuple0*_tmp2B4=_tmp29E->name;
_tmp2B5=*_tmp2B4;_tmp2B6=_tmp2B5.f2;name=(struct _dyneither_ptr*)_tmp2B6;goto
_LL1A5;}_LL1AC: {struct Cyc_Absyn_Enum_d_struct*_tmp29F=(struct Cyc_Absyn_Enum_d_struct*)
_tmp298;if(_tmp29F->tag != 8)goto _LL1AE;else{_tmp2A0=_tmp29F->f1;}}_LL1AD: {
struct _tuple0 _tmp2B8;struct _dyneither_ptr*_tmp2B9;struct _tuple0*_tmp2B7=_tmp2A0->name;
_tmp2B8=*_tmp2B7;_tmp2B9=_tmp2B8.f2;name=(struct _dyneither_ptr*)_tmp2B9;goto
_LL1A5;}_LL1AE: {struct Cyc_Absyn_Typedef_d_struct*_tmp2A1=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp298;if(_tmp2A1->tag != 9)goto _LL1B0;else{_tmp2A2=_tmp2A1->f1;}}_LL1AF: {
struct _tuple0 _tmp2BB;struct _dyneither_ptr*_tmp2BC;struct _tuple0*_tmp2BA=_tmp2A2->name;
_tmp2BB=*_tmp2BA;_tmp2BC=_tmp2BB.f2;name=(struct _dyneither_ptr*)_tmp2BC;goto
_LL1A5;}_LL1B0: {struct Cyc_Absyn_Region_d_struct*_tmp2A3=(struct Cyc_Absyn_Region_d_struct*)
_tmp298;if(_tmp2A3->tag != 4)goto _LL1B2;}_LL1B1: goto _LL1B3;_LL1B2: {struct Cyc_Absyn_Alias_d_struct*
_tmp2A4=(struct Cyc_Absyn_Alias_d_struct*)_tmp298;if(_tmp2A4->tag != 5)goto _LL1B4;}
_LL1B3: goto _LL1B5;_LL1B4: {struct Cyc_Absyn_Porton_d_struct*_tmp2A5=(struct Cyc_Absyn_Porton_d_struct*)
_tmp298;if(_tmp2A5->tag != 14)goto _LL1B6;}_LL1B5: goto _LL1B7;_LL1B6: {struct Cyc_Absyn_Portoff_d_struct*
_tmp2A6=(struct Cyc_Absyn_Portoff_d_struct*)_tmp298;if(_tmp2A6->tag != 15)goto
_LL1B8;}_LL1B7: goto _LL1B9;_LL1B8: {struct Cyc_Absyn_Let_d_struct*_tmp2A7=(struct
Cyc_Absyn_Let_d_struct*)_tmp298;if(_tmp2A7->tag != 2)goto _LL1BA;}_LL1B9: goto
_LL1BB;_LL1BA: {struct Cyc_Absyn_Datatype_d_struct*_tmp2A8=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp298;if(_tmp2A8->tag != 7)goto _LL1BC;}_LL1BB: goto _LL1BD;_LL1BC: {struct Cyc_Absyn_Letv_d_struct*
_tmp2A9=(struct Cyc_Absyn_Letv_d_struct*)_tmp298;if(_tmp2A9->tag != 3)goto _LL1BE;}
_LL1BD: goto _LL1BF;_LL1BE: {struct Cyc_Absyn_Namespace_d_struct*_tmp2AA=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp298;if(_tmp2AA->tag != 10)goto _LL1C0;}_LL1BF: goto _LL1C1;_LL1C0: {struct Cyc_Absyn_Using_d_struct*
_tmp2AB=(struct Cyc_Absyn_Using_d_struct*)_tmp298;if(_tmp2AB->tag != 11)goto _LL1C2;}
_LL1C1: goto _LL1C3;_LL1C2: {struct Cyc_Absyn_ExternC_d_struct*_tmp2AC=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp298;if(_tmp2AC->tag != 12)goto _LL1C4;}_LL1C3: goto _LL1C5;_LL1C4: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp2AD=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp298;if(_tmp2AD->tag != 13)
goto _LL1A5;}_LL1C5: name=0;goto _LL1A5;_LL1A5:;}if(!((unsigned int)name) && !
_tmp297)continue;{struct Cyc_List_List*_tmp5F5;_tmp293=((_tmp5F5=_cycalloc(
sizeof(*_tmp5F5)),((_tmp5F5->hd=_tmp296,((_tmp5F5->tl=_tmp293,_tmp5F5))))));}{
struct Cyc_List_List*_tmp5F6;_tmp294=((_tmp5F6=_cycalloc(sizeof(*_tmp5F6)),((
_tmp5F6->hd=name,((_tmp5F6->tl=_tmp294,_tmp5F6))))));};}}{struct _RegionHandle
_tmp2BF=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp2BF;_push_region(
tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp2C0=Cyc_Tcenv_tc_init(tc_rgn);Cyc_Tc_tc(tc_rgn,
_tmp2C0,1,_tmp293);}{struct Cyc_List_List*_tmp2C3;struct Cyc_List_List*_tmp2C4;
struct _tuple23 _tmp5F7;struct _tuple23 _tmp2C2=(_tmp5F7.f1=_tmp293,((_tmp5F7.f2=
_tmp294,_tmp5F7)));_tmp2C3=_tmp2C2.f1;_tmp2C4=_tmp2C2.f2;for(0;_tmp2C3 != 0  && 
_tmp2C4 != 0;(_tmp2C3=_tmp2C3->tl,_tmp2C4=_tmp2C4->tl)){struct Cyc_Absyn_Decl*
_tmp2C5=(struct Cyc_Absyn_Decl*)_tmp2C3->hd;struct _dyneither_ptr*_tmp2C6=(struct
_dyneither_ptr*)_tmp2C4->hd;int _tmp2C7=0;if(!((unsigned int)_tmp2C6))_tmp2C7=1;
if((unsigned int)_tmp2C6){{const char*_tmp5FB;void*_tmp5FA[1];struct Cyc_String_pa_struct
_tmp5F9;ifdefmacro=((_tmp5F9.tag=0,((_tmp5F9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp2C6),((_tmp5FA[0]=& _tmp5F9,Cyc_aprintf(((_tmp5FB="_%s_def_",
_tag_dyneither(_tmp5FB,sizeof(char),9))),_tag_dyneither(_tmp5FA,sizeof(void*),1))))))));}{
const char*_tmp5FF;void*_tmp5FE[1];struct Cyc_String_pa_struct _tmp5FD;(_tmp5FD.tag=
0,((_tmp5FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((
_tmp5FE[0]=& _tmp5FD,Cyc_fprintf(out_file,((_tmp5FF="#ifndef %s\n",_tag_dyneither(
_tmp5FF,sizeof(char),12))),_tag_dyneither(_tmp5FE,sizeof(void*),1)))))));}{const
char*_tmp603;void*_tmp602[1];struct Cyc_String_pa_struct _tmp601;(_tmp601.tag=0,((
_tmp601.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp602[0]=&
_tmp601,Cyc_fprintf(out_file,((_tmp603="#define %s\n",_tag_dyneither(_tmp603,
sizeof(char),12))),_tag_dyneither(_tmp602,sizeof(void*),1)))))));}{struct Cyc_Absyn_Decl*
_tmp604[1];Cyc_Absynpp_decllist2file(((_tmp604[0]=_tmp2C5,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp604,sizeof(struct Cyc_Absyn_Decl*),
1)))),out_file);}{const char*_tmp607;void*_tmp606;(_tmp606=0,Cyc_fprintf(out_file,((
_tmp607="#endif\n",_tag_dyneither(_tmp607,sizeof(char),8))),_tag_dyneither(
_tmp606,sizeof(void*),0)));};}else{struct Cyc_Absyn_Decl*_tmp608[1];Cyc_Absynpp_decllist2file(((
_tmp608[0]=_tmp2C5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp608,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}}}maybe=
Cyc_fopen(_tmp1F5,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp619;const char*_tmp618;void*_tmp617[1];const char*_tmp616;struct Cyc_String_pa_struct
_tmp615;struct Cyc_NO_SUPPORT_struct*_tmp614;(int)_throw((void*)((_tmp614=
_cycalloc(sizeof(*_tmp614)),((_tmp614[0]=((_tmp619.tag=Cyc_NO_SUPPORT,((_tmp619.f1=(
struct _dyneither_ptr)((_tmp615.tag=0,((_tmp615.f1=(struct _dyneither_ptr)((
_tmp616=_tmp1F5,_tag_dyneither(_tmp616,sizeof(char),_get_zero_arr_size_char((
void*)_tmp616,1)))),((_tmp617[0]=& _tmp615,Cyc_aprintf(((_tmp618="can't open macrosfile %s",
_tag_dyneither(_tmp618,sizeof(char),25))),_tag_dyneither(_tmp617,sizeof(void*),1)))))))),
_tmp619)))),_tmp614)))));}in_file=(struct Cyc___cycFILE*)maybe;_tmp23B=Cyc_Lexing_from_file(
in_file);{struct _tuple18*entry2;while((entry2=((struct _tuple18*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_suck_line)(_tmp23B))!= 0){struct _tuple18 _tmp2DC;struct _dyneither_ptr
_tmp2DD;struct _dyneither_ptr*_tmp2DE;struct _tuple18*_tmp2DB=(struct _tuple18*)
_check_null(entry2);_tmp2DC=*_tmp2DB;_tmp2DD=_tmp2DC.f1;_tmp2DE=_tmp2DC.f2;if(((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp256,
_tmp2DE)){{const char*_tmp61D;void*_tmp61C[1];struct Cyc_String_pa_struct _tmp61B;(
_tmp61B.tag=0,((_tmp61B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2DE),((
_tmp61C[0]=& _tmp61B,Cyc_fprintf(out_file,((_tmp61D="#ifndef %s\n",_tag_dyneither(
_tmp61D,sizeof(char),12))),_tag_dyneither(_tmp61C,sizeof(void*),1)))))));}{const
char*_tmp621;void*_tmp620[1];struct Cyc_String_pa_struct _tmp61F;(_tmp61F.tag=0,((
_tmp61F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DD),((_tmp620[0]=&
_tmp61F,Cyc_fprintf(out_file,((_tmp621="%s\n",_tag_dyneither(_tmp621,sizeof(char),
4))),_tag_dyneither(_tmp620,sizeof(void*),1)))))));}{const char*_tmp624;void*
_tmp623;(_tmp623=0,Cyc_fprintf(out_file,((_tmp624="#endif\n",_tag_dyneither(
_tmp624,sizeof(char),8))),_tag_dyneither(_tmp623,sizeof(void*),0)));};}}Cyc_fclose(
in_file);if(Cyc_mode != Cyc_FINISH)remove(_tmp1F5);if(hstubs != 0){struct Cyc_List_List*
_tmp2E7=hstubs;for(0;_tmp2E7 != 0;_tmp2E7=_tmp2E7->tl){struct _tuple19 _tmp2E9;
struct _dyneither_ptr _tmp2EA;struct _dyneither_ptr _tmp2EB;struct _tuple19*_tmp2E8=(
struct _tuple19*)_tmp2E7->hd;_tmp2E9=*_tmp2E8;_tmp2EA=_tmp2E9.f1;_tmp2EB=_tmp2E9.f2;{
struct _dyneither_ptr*_tmp625;if((char*)_tmp2EB.curr != (char*)(_tag_dyneither(0,0,
0)).curr  && ((char*)_tmp2EA.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((
_tmp625=_cycalloc(sizeof(*_tmp625)),((_tmp625[0]=_tmp2EA,_tmp625)))))))Cyc_fputs((
const char*)_untag_dyneither_ptr(_tmp2EB,sizeof(char),1),out_file);else{const char*
_tmp629;void*_tmp628[1];struct Cyc_String_pa_struct _tmp627;(_tmp627.tag=0,((
_tmp627.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2EA),((_tmp628[0]=&
_tmp627,Cyc_log(((_tmp629="%s is not supported on this platform\n",
_tag_dyneither(_tmp629,sizeof(char),38))),_tag_dyneither(_tmp628,sizeof(void*),1)))))));}};}}{
const char*_tmp62C;void*_tmp62B;(_tmp62B=0,Cyc_fprintf(out_file,((_tmp62C="#endif\n",
_tag_dyneither(_tmp62C,sizeof(char),8))),_tag_dyneither(_tmp62B,sizeof(void*),0)));}
if(Cyc_do_setjmp){int _tmp2F2=0;_npop_handler(1);return _tmp2F2;}else{Cyc_fclose(
out_file);}if(cstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp2F3=cstubs;for(0;_tmp2F3 != 0;_tmp2F3=_tmp2F3->tl){struct
_tuple19 _tmp2F5;struct _dyneither_ptr _tmp2F6;struct _dyneither_ptr _tmp2F7;struct
_tuple19*_tmp2F4=(struct _tuple19*)_tmp2F3->hd;_tmp2F5=*_tmp2F4;_tmp2F6=_tmp2F5.f1;
_tmp2F7=_tmp2F5.f2;{struct _dyneither_ptr*_tmp62D;if((char*)_tmp2F7.curr != (char*)(
_tag_dyneither(0,0,0)).curr  && ((char*)_tmp2F6.curr == (char*)(_tag_dyneither(0,0,
0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
defined_symbols,((_tmp62D=_cycalloc(sizeof(*_tmp62D)),((_tmp62D[0]=_tmp2F6,
_tmp62D)))))))Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp2F7,sizeof(char),1),
out_file);};}};}out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
const char*_tmp633;void*_tmp632[1];const char*_tmp631;struct Cyc_String_pa_struct
_tmp630;(_tmp630.tag=0,((_tmp630.f1=(struct _dyneither_ptr)((_tmp631=filename,
_tag_dyneither(_tmp631,sizeof(char),_get_zero_arr_size_char((void*)_tmp631,1)))),((
_tmp632[0]=& _tmp630,Cyc_fprintf(out_file,((_tmp633="#include <%s>\n\n",
_tag_dyneither(_tmp633,sizeof(char),16))),_tag_dyneither(_tmp632,sizeof(void*),1)))))));}
if(cycstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
struct Cyc_List_List*_tmp2FD=cycstubs;for(0;_tmp2FD != 0;_tmp2FD=_tmp2FD->tl){
struct _tuple19 _tmp2FF;struct _dyneither_ptr _tmp300;struct _dyneither_ptr _tmp301;
struct _tuple19*_tmp2FE=(struct _tuple19*)_tmp2FD->hd;_tmp2FF=*_tmp2FE;_tmp300=
_tmp2FF.f1;_tmp301=_tmp2FF.f2;{struct _dyneither_ptr*_tmp634;if((char*)_tmp301.curr
!= (char*)(_tag_dyneither(0,0,0)).curr  && ((char*)_tmp300.curr == (char*)(
_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_member)(defined_symbols,((_tmp634=_cycalloc(sizeof(*_tmp634)),((
_tmp634[0]=_tmp300,_tmp634)))))))Cyc_fputs((const char*)_untag_dyneither_ptr(
_tmp301,sizeof(char),1),out_file);};}}{const char*_tmp637;void*_tmp636;(_tmp636=0,
Cyc_fprintf(out_file,((_tmp637="\n",_tag_dyneither(_tmp637,sizeof(char),2))),
_tag_dyneither(_tmp636,sizeof(void*),0)));};}{int _tmp305=0;_npop_handler(1);
return _tmp305;};};;_pop_region(tc_rgn);};};};};};};};;_pop_handler();}else{void*
_tmp1F9=(void*)_exn_thrown;void*_tmp30B=_tmp1F9;struct _dyneither_ptr _tmp30D;
_LL1C7: {struct Cyc_NO_SUPPORT_struct*_tmp30C=(struct Cyc_NO_SUPPORT_struct*)
_tmp30B;if(_tmp30C->tag != Cyc_NO_SUPPORT)goto _LL1C9;else{_tmp30D=_tmp30C->f1;}}
_LL1C8:{const char*_tmp63B;void*_tmp63A[1];struct Cyc_String_pa_struct _tmp639;(
_tmp639.tag=0,((_tmp639.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp30D),((
_tmp63A[0]=& _tmp639,Cyc_fprintf(Cyc_stderr,((_tmp63B="No support because %s\n",
_tag_dyneither(_tmp63B,sizeof(char),23))),_tag_dyneither(_tmp63A,sizeof(void*),1)))))));}
goto _LL1CA;_LL1C9:;_LL1CA: maybe=Cyc_fopen(filename,(const char*)"w");if(!((
unsigned int)maybe)){{const char*_tmp641;void*_tmp640[1];const char*_tmp63F;struct
Cyc_String_pa_struct _tmp63E;(_tmp63E.tag=0,((_tmp63E.f1=(struct _dyneither_ptr)((
_tmp63F=filename,_tag_dyneither(_tmp63F,sizeof(char),_get_zero_arr_size_char((
void*)_tmp63F,1)))),((_tmp640[0]=& _tmp63E,Cyc_fprintf(Cyc_stderr,((_tmp641="Error: could not create file %s\n",
_tag_dyneither(_tmp641,sizeof(char),33))),_tag_dyneither(_tmp640,sizeof(void*),1)))))));}
return 1;}out_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp647;void*_tmp646[1];
const char*_tmp645;struct Cyc_String_pa_struct _tmp644;(_tmp644.tag=0,((_tmp644.f1=(
struct _dyneither_ptr)((_tmp645=filename,_tag_dyneither(_tmp645,sizeof(char),
_get_zero_arr_size_char((void*)_tmp645,1)))),((_tmp646[0]=& _tmp644,Cyc_fprintf(
out_file,((_tmp647="#error -- %s is not supported on this platform\n",
_tag_dyneither(_tmp647,sizeof(char),48))),_tag_dyneither(_tmp646,sizeof(void*),1)))))));}
Cyc_fclose(out_file);{const char*_tmp64D;void*_tmp64C[1];const char*_tmp64B;struct
Cyc_String_pa_struct _tmp64A;(_tmp64A.tag=0,((_tmp64A.f1=(struct _dyneither_ptr)((
_tmp64B=filename,_tag_dyneither(_tmp64B,sizeof(char),_get_zero_arr_size_char((
void*)_tmp64B,1)))),((_tmp64C[0]=& _tmp64A,Cyc_fprintf(Cyc_stderr,((_tmp64D="Warning: %s will not be supported on this platform\n",
_tag_dyneither(_tmp64D,sizeof(char),52))),_tag_dyneither(_tmp64C,sizeof(void*),1)))))));}{
const char*_tmp650;void*_tmp64F;(_tmp64F=0,Cyc_log(((_tmp650="Not supported on this platform\n",
_tag_dyneither(_tmp650,sizeof(char),32))),_tag_dyneither(_tmp64F,sizeof(void*),0)));}
remove(_tmp1F5);remove(_tmp1F6);remove(_tmp1F7);return 0;_LL1CB:;_LL1CC:(void)
_throw(_tmp30B);_LL1C6:;}};};}int Cyc_process_specfile(const char*file,const char*
dir);static void _tmp65B(unsigned int*_tmp65A,unsigned int*_tmp659,char**_tmp658){
for(*_tmp65A=0;*_tmp65A < *_tmp659;(*_tmp65A)++){(*_tmp658)[*_tmp65A]='\000';}}
int Cyc_process_specfile(const char*file,const char*dir){struct Cyc___cycFILE*
_tmp334=Cyc_fopen(file,(const char*)"r");if(!((unsigned int)_tmp334)){{const char*
_tmp656;void*_tmp655[1];const char*_tmp654;struct Cyc_String_pa_struct _tmp653;(
_tmp653.tag=0,((_tmp653.f1=(struct _dyneither_ptr)((_tmp654=file,_tag_dyneither(
_tmp654,sizeof(char),_get_zero_arr_size_char((void*)_tmp654,1)))),((_tmp655[0]=&
_tmp653,Cyc_fprintf(Cyc_stderr,((_tmp656="Error: could not open %s\n",
_tag_dyneither(_tmp656,sizeof(char),26))),_tag_dyneither(_tmp655,sizeof(void*),1)))))));}
return 1;}{struct Cyc___cycFILE*_tmp339=(struct Cyc___cycFILE*)_tmp334;char*_tmp660;
unsigned int _tmp65F;char*_tmp65E;unsigned int _tmp65D;unsigned int _tmp65C;struct
_dyneither_ptr buf=(_tmp660=((_tmp65F=(unsigned int)1024,((_tmp65E=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp65F + 1)),((((_tmp65D=_tmp65F,((
_tmp65B(& _tmp65C,& _tmp65D,& _tmp65E),_tmp65E[_tmp65D]=(char)0)))),_tmp65E)))))),
_tag_dyneither(_tmp660,sizeof(char),_get_zero_arr_size_char((void*)_tmp660,(
unsigned int)1024 + 1)));struct _dyneither_ptr _tmp33A=Cyc_getcwd(buf,
_get_dyneither_size(buf,sizeof(char)));if(Cyc_mode != Cyc_GATHERSCRIPT){if(chdir(
dir)){{const char*_tmp666;void*_tmp665[1];const char*_tmp664;struct Cyc_String_pa_struct
_tmp663;(_tmp663.tag=0,((_tmp663.f1=(struct _dyneither_ptr)((_tmp664=dir,
_tag_dyneither(_tmp664,sizeof(char),_get_zero_arr_size_char((void*)_tmp664,1)))),((
_tmp665[0]=& _tmp663,Cyc_fprintf(Cyc_stderr,((_tmp666="Error: can't change directory to %s\n",
_tag_dyneither(_tmp666,sizeof(char),37))),_tag_dyneither(_tmp665,sizeof(void*),1)))))));}
return 1;}}if(Cyc_mode == Cyc_GATHER){struct _dyneither_ptr _tmp33F=
Cstring_to_string(Ccomp);const char*_tmp66A;void*_tmp669[1];struct Cyc_String_pa_struct
_tmp668;system((const char*)_untag_dyneither_ptr(((_tmp668.tag=0,((_tmp668.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33F),((_tmp669[0]=& _tmp668,Cyc_aprintf(((
_tmp66A="echo | %s -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp66A,sizeof(
char),36))),_tag_dyneither(_tmp669,sizeof(void*),1)))))))),sizeof(char),1));}{
struct Cyc_Lexing_lexbuf*_tmp343=Cyc_Lexing_from_file(_tmp339);struct _tuple20*
entry;while((entry=((struct _tuple20*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(
_tmp343))!= 0){struct _tuple20 _tmp345;struct _dyneither_ptr _tmp346;struct Cyc_List_List*
_tmp347;struct Cyc_List_List*_tmp348;struct Cyc_List_List*_tmp349;struct Cyc_List_List*
_tmp34A;struct Cyc_List_List*_tmp34B;struct _tuple20*_tmp344=(struct _tuple20*)
_check_null(entry);_tmp345=*_tmp344;_tmp346=_tmp345.f1;_tmp347=_tmp345.f2;
_tmp348=_tmp345.f3;_tmp349=_tmp345.f4;_tmp34A=_tmp345.f5;_tmp34B=_tmp345.f6;if(
Cyc_process_file((const char*)_untag_dyneither_ptr(_tmp346,sizeof(char),1),
_tmp347,_tmp348,_tmp349,_tmp34A,_tmp34B))return 1;}Cyc_fclose(_tmp339);if(Cyc_mode
!= Cyc_GATHERSCRIPT){if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp33A,
sizeof(char),1)))){{const char*_tmp66E;void*_tmp66D[1];struct Cyc_String_pa_struct
_tmp66C;(_tmp66C.tag=0,((_tmp66C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp33A),((_tmp66D[0]=& _tmp66C,Cyc_fprintf(Cyc_stderr,((_tmp66E="Error: could not change directory to %s\n",
_tag_dyneither(_tmp66E,sizeof(char),41))),_tag_dyneither(_tmp66D,sizeof(void*),1)))))));}
return 1;}}return 0;};};}int Cyc_process_setjmp(const char*dir);static void _tmp673(
unsigned int*_tmp672,unsigned int*_tmp671,char**_tmp670){for(*_tmp672=0;*_tmp672
< *_tmp671;(*_tmp672)++){(*_tmp670)[*_tmp672]='\000';}}int Cyc_process_setjmp(
const char*dir){char*_tmp678;unsigned int _tmp677;char*_tmp676;unsigned int _tmp675;
unsigned int _tmp674;struct _dyneither_ptr buf=(_tmp678=((_tmp677=(unsigned int)
1024,((_tmp676=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp677 + 1)),((((
_tmp675=_tmp677,((_tmp673(& _tmp674,& _tmp675,& _tmp676),_tmp676[_tmp675]=(char)0)))),
_tmp676)))))),_tag_dyneither(_tmp678,sizeof(char),_get_zero_arr_size_char((void*)
_tmp678,(unsigned int)1024 + 1)));struct _dyneither_ptr _tmp353=Cyc_getcwd(buf,
_get_dyneither_size(buf,sizeof(char)));if(chdir(dir)){{const char*_tmp67E;void*
_tmp67D[1];const char*_tmp67C;struct Cyc_String_pa_struct _tmp67B;(_tmp67B.tag=0,((
_tmp67B.f1=(struct _dyneither_ptr)((_tmp67C=dir,_tag_dyneither(_tmp67C,sizeof(
char),_get_zero_arr_size_char((void*)_tmp67C,1)))),((_tmp67D[0]=& _tmp67B,Cyc_fprintf(
Cyc_stderr,((_tmp67E="Error: can't change directory to %s\n",_tag_dyneither(
_tmp67E,sizeof(char),37))),_tag_dyneither(_tmp67D,sizeof(void*),1)))))));}return
1;}{struct _tuple19*_tmp688;const char*_tmp687;const char*_tmp686;struct _tuple19*
_tmp685[1];struct _dyneither_ptr*_tmp67F[1];if(Cyc_process_file((const char*)"setjmp.h",((
_tmp67F[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",
sizeof(char),8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp67F,sizeof(struct _dyneither_ptr*),1)))),0,((_tmp685[0]=((
_tmp688=_cycalloc(sizeof(*_tmp688)),((_tmp688->f1=((_tmp687="setjmp",
_tag_dyneither(_tmp687,sizeof(char),7))),((_tmp688->f2=((_tmp686="extern int setjmp(jmp_buf);\n",
_tag_dyneither(_tmp686,sizeof(char),29))),_tmp688)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp685,sizeof(struct
_tuple19*),1)))),0,0))return 1;}if(chdir((const char*)((char*)_untag_dyneither_ptr(
_tmp353,sizeof(char),1)))){{const char*_tmp68C;void*_tmp68B[1];struct Cyc_String_pa_struct
_tmp68A;(_tmp68A.tag=0,((_tmp68A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp353),((_tmp68B[0]=& _tmp68A,Cyc_fprintf(Cyc_stderr,((_tmp68C="Error: could not change directory to %s\n",
_tag_dyneither(_tmp68C,sizeof(char),41))),_tag_dyneither(_tmp68B,sizeof(void*),1)))))));}
return 1;}return 0;}static char _tmp365[13]="BUILDLIB.OUT";static struct
_dyneither_ptr Cyc_output_dir={_tmp365,_tmp365,_tmp365 + 13};static void Cyc_set_output_dir(
struct _dyneither_ptr s){Cyc_output_dir=s;}static struct Cyc_List_List*Cyc_spec_files=
0;static void Cyc_add_spec_file(struct _dyneither_ptr s){struct Cyc_List_List*_tmp68D;
Cyc_spec_files=((_tmp68D=_cycalloc(sizeof(*_tmp68D)),((_tmp68D->hd=(const char*)
_untag_dyneither_ptr(s,sizeof(char),1),((_tmp68D->tl=Cyc_spec_files,_tmp68D))))));}
static void Cyc_set_GATHER(){Cyc_mode=Cyc_GATHER;}static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}static void Cyc_set_FINISH(){Cyc_mode=Cyc_FINISH;}
static int Cyc_badparse=0;static void Cyc_unsupported_option(struct _dyneither_ptr s){{
const char*_tmp691;void*_tmp690[1];struct Cyc_String_pa_struct _tmp68F;(_tmp68F.tag=
0,((_tmp68F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp690[0]=&
_tmp68F,Cyc_fprintf(Cyc_stderr,((_tmp691="Unsupported option %s\n",
_tag_dyneither(_tmp691,sizeof(char),23))),_tag_dyneither(_tmp690,sizeof(void*),1)))))));}
Cyc_badparse=1;}void GC_blacklist_warn_clear();struct _tuple24{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){GC_blacklist_warn_clear();{struct
_tuple24*_tmp6FE;const char*_tmp6FD;const char*_tmp6FC;struct Cyc_Arg_String_spec_struct
_tmp6FB;struct Cyc_Arg_String_spec_struct*_tmp6FA;const char*_tmp6F9;struct
_tuple24*_tmp6F8;const char*_tmp6F7;const char*_tmp6F6;struct Cyc_Arg_Unit_spec_struct
_tmp6F5;struct Cyc_Arg_Unit_spec_struct*_tmp6F4;const char*_tmp6F3;struct _tuple24*
_tmp6F2;const char*_tmp6F1;const char*_tmp6F0;struct Cyc_Arg_Unit_spec_struct
_tmp6EF;struct Cyc_Arg_Unit_spec_struct*_tmp6EE;const char*_tmp6ED;struct _tuple24*
_tmp6EC;const char*_tmp6EB;const char*_tmp6EA;struct Cyc_Arg_Unit_spec_struct
_tmp6E9;struct Cyc_Arg_Unit_spec_struct*_tmp6E8;const char*_tmp6E7;struct _tuple24*
_tmp6E6;const char*_tmp6E5;const char*_tmp6E4;struct Cyc_Arg_Set_spec_struct _tmp6E3;
struct Cyc_Arg_Set_spec_struct*_tmp6E2;const char*_tmp6E1;struct _tuple24*_tmp6E0;
const char*_tmp6DF;const char*_tmp6DE;struct Cyc_Arg_Flag_spec_struct _tmp6DD;struct
Cyc_Arg_Flag_spec_struct*_tmp6DC;const char*_tmp6DB;struct _tuple24*_tmp6DA[6];
struct Cyc_List_List*options=(_tmp6DA[5]=((_tmp6E0=_cycalloc(sizeof(*_tmp6E0)),((
_tmp6E0->f1=((_tmp6DF="-",_tag_dyneither(_tmp6DF,sizeof(char),2))),((_tmp6E0->f2=
1,((_tmp6E0->f3=((_tmp6DE="",_tag_dyneither(_tmp6DE,sizeof(char),1))),((_tmp6E0->f4=(
void*)((_tmp6DC=_cycalloc(sizeof(*_tmp6DC)),((_tmp6DC[0]=((_tmp6DD.tag=1,((
_tmp6DD.f1=Cyc_unsupported_option,_tmp6DD)))),_tmp6DC)))),((_tmp6E0->f5=((
_tmp6DB="",_tag_dyneither(_tmp6DB,sizeof(char),1))),_tmp6E0)))))))))))),((
_tmp6DA[4]=((_tmp6E6=_cycalloc(sizeof(*_tmp6E6)),((_tmp6E6->f1=((_tmp6E5="-setjmp",
_tag_dyneither(_tmp6E5,sizeof(char),8))),((_tmp6E6->f2=0,((_tmp6E6->f3=((_tmp6E4="",
_tag_dyneither(_tmp6E4,sizeof(char),1))),((_tmp6E6->f4=(void*)((_tmp6E2=
_cycalloc(sizeof(*_tmp6E2)),((_tmp6E2[0]=((_tmp6E3.tag=3,((_tmp6E3.f1=& Cyc_do_setjmp,
_tmp6E3)))),_tmp6E2)))),((_tmp6E6->f5=((_tmp6E1="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.",
_tag_dyneither(_tmp6E1,sizeof(char),186))),_tmp6E6)))))))))))),((_tmp6DA[3]=((
_tmp6EC=_cycalloc(sizeof(*_tmp6EC)),((_tmp6EC->f1=((_tmp6EB="-finish",
_tag_dyneither(_tmp6EB,sizeof(char),8))),((_tmp6EC->f2=0,((_tmp6EC->f3=((_tmp6EA="",
_tag_dyneither(_tmp6EA,sizeof(char),1))),((_tmp6EC->f4=(void*)((_tmp6E8=
_cycalloc(sizeof(*_tmp6E8)),((_tmp6E8[0]=((_tmp6E9.tag=0,((_tmp6E9.f1=Cyc_set_FINISH,
_tmp6E9)))),_tmp6E8)))),((_tmp6EC->f5=((_tmp6E7="Produce Cyclone headers from pre-gathered C library info",
_tag_dyneither(_tmp6E7,sizeof(char),57))),_tmp6EC)))))))))))),((_tmp6DA[2]=((
_tmp6F2=_cycalloc(sizeof(*_tmp6F2)),((_tmp6F2->f1=((_tmp6F1="-gatherscript",
_tag_dyneither(_tmp6F1,sizeof(char),14))),((_tmp6F2->f2=0,((_tmp6F2->f3=((
_tmp6F0="",_tag_dyneither(_tmp6F0,sizeof(char),1))),((_tmp6F2->f4=(void*)((
_tmp6EE=_cycalloc(sizeof(*_tmp6EE)),((_tmp6EE[0]=((_tmp6EF.tag=0,((_tmp6EF.f1=
Cyc_set_GATHERSCRIPT,_tmp6EF)))),_tmp6EE)))),((_tmp6F2->f5=((_tmp6ED="Produce a script to gather C library info",
_tag_dyneither(_tmp6ED,sizeof(char),42))),_tmp6F2)))))))))))),((_tmp6DA[1]=((
_tmp6F8=_cycalloc(sizeof(*_tmp6F8)),((_tmp6F8->f1=((_tmp6F7="-gather",
_tag_dyneither(_tmp6F7,sizeof(char),8))),((_tmp6F8->f2=0,((_tmp6F8->f3=((_tmp6F6="",
_tag_dyneither(_tmp6F6,sizeof(char),1))),((_tmp6F8->f4=(void*)((_tmp6F4=
_cycalloc(sizeof(*_tmp6F4)),((_tmp6F4[0]=((_tmp6F5.tag=0,((_tmp6F5.f1=Cyc_set_GATHER,
_tmp6F5)))),_tmp6F4)))),((_tmp6F8->f5=((_tmp6F3="Gather C library info but don't produce Cyclone headers",
_tag_dyneither(_tmp6F3,sizeof(char),56))),_tmp6F8)))))))))))),((_tmp6DA[0]=((
_tmp6FE=_cycalloc(sizeof(*_tmp6FE)),((_tmp6FE->f1=((_tmp6FD="-d",_tag_dyneither(
_tmp6FD,sizeof(char),3))),((_tmp6FE->f2=0,((_tmp6FE->f3=((_tmp6FC=" <file>",
_tag_dyneither(_tmp6FC,sizeof(char),8))),((_tmp6FE->f4=(void*)((_tmp6FA=
_cycalloc(sizeof(*_tmp6FA)),((_tmp6FA[0]=((_tmp6FB.tag=5,((_tmp6FB.f1=Cyc_set_output_dir,
_tmp6FB)))),_tmp6FA)))),((_tmp6FE->f5=((_tmp6F9="Set the output directory to <file>",
_tag_dyneither(_tmp6F9,sizeof(char),35))),_tmp6FE)))))))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6DA,sizeof(struct
_tuple24*),6)))))))))))));{const char*_tmp6FF;Cyc_Arg_parse(options,Cyc_add_spec_file,((
_tmp6FF="Options:",_tag_dyneither(_tmp6FF,sizeof(char),9))),argv);}if((((Cyc_badparse
 || !Cyc_do_setjmp  && Cyc_spec_files == 0) || Cyc_do_setjmp  && Cyc_spec_files != 0)
 || Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){{
const char*_tmp700;Cyc_Arg_usage(options,((_tmp700="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",
_tag_dyneither(_tmp700,sizeof(char),59))));}return 1;}if(Cyc_mode == Cyc_GATHERSCRIPT){
Cyc_script_file=Cyc_fopen((const char*)"BUILDLIB.sh",(const char*)"w");if(!((
unsigned int)Cyc_script_file)){{const char*_tmp703;void*_tmp702;(_tmp702=0,Cyc_fprintf(
Cyc_stderr,((_tmp703="Could not create file BUILDLIB.sh\n",_tag_dyneither(
_tmp703,sizeof(char),35))),_tag_dyneither(_tmp702,sizeof(void*),0)));}exit(1);}{
const char*_tmp706;void*_tmp705;(_tmp705=0,Cyc_prscript(((_tmp706="#!/bin/sh\n",
_tag_dyneither(_tmp706,sizeof(char),11))),_tag_dyneither(_tmp705,sizeof(void*),0)));}{
const char*_tmp709;void*_tmp708;(_tmp708=0,Cyc_prscript(((_tmp709="GCC=\"gcc\"\n",
_tag_dyneither(_tmp709,sizeof(char),11))),_tag_dyneither(_tmp708,sizeof(void*),0)));};}
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){{
const char*_tmp70D;void*_tmp70C[1];struct Cyc_String_pa_struct _tmp70B;(_tmp70B.tag=
0,((_tmp70B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp70C[0]=& _tmp70B,Cyc_fprintf(Cyc_stderr,((_tmp70D="Error: could not create directory %s\n",
_tag_dyneither(_tmp70D,sizeof(char),38))),_tag_dyneither(_tmp70C,sizeof(void*),1)))))));}
return 1;}if(Cyc_mode == Cyc_GATHERSCRIPT){{const char*_tmp711;void*_tmp710[1];
struct Cyc_String_pa_struct _tmp70F;(_tmp70F.tag=0,((_tmp70F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp710[0]=& _tmp70F,Cyc_prscript(((
_tmp711="cd %s\n",_tag_dyneither(_tmp711,sizeof(char),7))),_tag_dyneither(
_tmp710,sizeof(void*),1)))))));}{const char*_tmp714;void*_tmp713;(_tmp713=0,Cyc_prscript(((
_tmp714="echo | $GCC -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp714,sizeof(
char),38))),_tag_dyneither(_tmp713,sizeof(void*),0)));};}if(!Cyc_gathering()){{
const char*_tmp715;Cyc_log_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(
Cyc_output_dir,((_tmp715="BUILDLIB.LOG",_tag_dyneither(_tmp715,sizeof(char),13)))),
sizeof(char),1),(const char*)"w");}if(!((unsigned int)Cyc_log_file)){{const char*
_tmp719;void*_tmp718[1];struct Cyc_String_pa_struct _tmp717;(_tmp717.tag=0,((
_tmp717.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp718[0]=& _tmp717,Cyc_fprintf(Cyc_stderr,((_tmp719="Error: could not create log file in directory %s\n",
_tag_dyneither(_tmp719,sizeof(char),50))),_tag_dyneither(_tmp718,sizeof(void*),1)))))));}
return 1;}if(!Cyc_do_setjmp){{const char*_tmp71A;Cyc_cstubs_file=Cyc_fopen((const
char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp71A="cstubs.c",
_tag_dyneither(_tmp71A,sizeof(char),9)))),sizeof(char),1),(const char*)"w");}if(!((
unsigned int)Cyc_cstubs_file)){{const char*_tmp71E;void*_tmp71D[1];struct Cyc_String_pa_struct
_tmp71C;(_tmp71C.tag=0,((_tmp71C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp71D[0]=& _tmp71C,Cyc_fprintf(Cyc_stderr,((_tmp71E="Error: could not create cstubs.c in directory %s\n",
_tag_dyneither(_tmp71E,sizeof(char),50))),_tag_dyneither(_tmp71D,sizeof(void*),1)))))));}
return 1;}{const char*_tmp71F;Cyc_cycstubs_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp71F="cycstubs.cyc",
_tag_dyneither(_tmp71F,sizeof(char),13)))),sizeof(char),1),(const char*)"w");}if(
!((unsigned int)Cyc_cycstubs_file)){{const char*_tmp723;void*_tmp722[1];struct Cyc_String_pa_struct
_tmp721;(_tmp721.tag=0,((_tmp721.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp722[0]=& _tmp721,Cyc_fprintf(Cyc_stderr,((_tmp723="Error: could not create cycstubs.c in directory %s\n",
_tag_dyneither(_tmp723,sizeof(char),52))),_tag_dyneither(_tmp722,sizeof(void*),1)))))));}
return 1;}{const char*_tmp726;void*_tmp725;(_tmp725=0,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_cycstubs_file),((_tmp726="#include <core.h>\nusing Core;\n\n",
_tag_dyneither(_tmp726,sizeof(char),32))),_tag_dyneither(_tmp725,sizeof(void*),0)));};}}{
const char*outdir=(const char*)_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1);
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))return 1;else{for(0;Cyc_spec_files
!= 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){if(
Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,
outdir)){{const char*_tmp729;void*_tmp728;(_tmp728=0,Cyc_fprintf(Cyc_stderr,((
_tmp729="FATAL ERROR -- QUIT!\n",_tag_dyneither(_tmp729,sizeof(char),22))),
_tag_dyneither(_tmp728,sizeof(void*),0)));}exit(1);}}}if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));Cyc_fclose((
struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}return 0;};};}
