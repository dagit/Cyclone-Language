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
Cyc_Core_opt_t;int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
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
int Cyc_List_length(struct Cyc_List_List*x);void*Cyc_List_hd(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_exists_c(
int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);int
Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,
void*),void*env,struct Cyc_List_List*x);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*Cyc_Set_set_t;
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict
Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*
tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(
struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,
void*k);void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d);void Cyc_Dict_iter_c(
void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict
Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2);struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,
void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};typedef
unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};typedef void*Cyc_Position_error_kind_t;
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
_tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};typedef struct _tuple1*
Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;typedef struct _tuple1*
Cyc_Absyn_typedef_name_t;typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
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
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_RefCntRgn_struct{
int tag;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_TypeDeclType_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;
};extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_struct{
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
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};struct
Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};
struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*
f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts
f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;
};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;
unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;unsigned int
f1;struct Cyc_List_List*f2;};typedef void*Cyc_Absyn_type_modifier_t;struct
_union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int
tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};
struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int
tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;
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
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
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
int tag;struct Cyc_List_List*f1;};struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple9*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
_tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;
struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*
name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
unsigned int loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct _dyneither_ptr*f1;};extern
char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{char*tag;};extern
struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val;int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(void*y,
union Cyc_Absyn_Constraint*x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_Absyn_is_nontagged_union_type(void*);struct
Cyc_RgnOrder_RgnPO;typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;struct
Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*
tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct
Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
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
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(
unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(
void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_bits_only(void*t);
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;typedef struct Cyc_PP_Out*
Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{
int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int
rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;
int print_using_stmts;int print_externC_stmts;int print_full_evars;int
print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_CfFlowInfo_VarRoot_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 
1,Cyc_CfFlowInfo_AllIL  = 2};typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(unsigned int);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(
struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(
unsigned int);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp
rop;};typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;typedef struct Cyc_List_List*
Cyc_CfFlowInfo_relns_t;char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9]="UnknownZ";
struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};extern
struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val;struct
_union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
_union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct
_union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};typedef union
Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(
struct Cyc_CfFlowInfo_Place*);union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
typedef void*Cyc_CfFlowInfo_absRval_t;typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{
int is_union;int fieldnum;};typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;
struct Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;
struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{
int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,
unsigned int loc);int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct
Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct
Cyc_Dict_Dict s2);typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct
Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int
tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{struct
_union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_BottomFL();union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(
struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct
_RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;
void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;
struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index_fs(
struct Cyc_List_List*fs,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_root_cmp(void*,
void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_List_List*,int no_init_bits_only,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*);int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int
env_iteration,void*r,int*needs_unconsume);void*Cyc_CfFlowInfo_make_unique_unconsumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);void Cyc_CfFlowInfo_print_absrval(
void*rval);void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);void Cyc_CfFlowInfo_print_place(struct
Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,
void(*pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(
void*));void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo f);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,
struct _tuple13 pr2);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union
Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
extern struct Cyc_CfFlowInfo_UniqueRgn_k_struct Cyc_CfFlowInfo_UniqueRgn_k_val;
extern struct Cyc_CfFlowInfo_RefCntRgn_k_struct Cyc_CfFlowInfo_RefCntRgn_k_val;
typedef void*Cyc_CfFlowInfo_killrgn_t;int Cyc_CfFlowInfo_contains_region(void*rgn,
void*t);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*
place);struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
struct Cyc_CfFlowInfo_UniqueRgn_k_struct Cyc_CfFlowInfo_UniqueRgn_k_val={0};struct
Cyc_CfFlowInfo_RefCntRgn_k_struct Cyc_CfFlowInfo_RefCntRgn_k_val={1};union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){union Cyc_CfFlowInfo_AbsLVal
_tmp49B;return((_tmp49B.PlaceL).val=x,(((_tmp49B.PlaceL).tag=1,_tmp49B)));}union
Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){union Cyc_CfFlowInfo_AbsLVal
_tmp49C;return((_tmp49C.UnknownL).val=0,(((_tmp49C.UnknownL).tag=2,_tmp49C)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo
_tmp49D;return((_tmp49D.BottomFL).val=0,(((_tmp49D.BottomFL).tag=1,_tmp49D)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,
struct Cyc_List_List*r){struct _tuple12 _tmp4A0;union Cyc_CfFlowInfo_FlowInfo _tmp49F;
return((_tmp49F.ReachableFL).val=((_tmp4A0.f1=fd,((_tmp4A0.f2=r,_tmp4A0)))),(((
_tmp49F.ReachableFL).tag=2,_tmp49F)));}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,{.Null_c={1,0}}};static
struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp4D7;struct Cyc_CfFlowInfo_MallocPt_struct _tmp4D6;
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp4D5;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4D4;struct Cyc_CfFlowInfo_Esc_struct _tmp4D3;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4D2;struct Cyc_CfFlowInfo_Esc_struct _tmp4D1;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4D0;struct Cyc_CfFlowInfo_Esc_struct _tmp4CF;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4CE;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4CD;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4CC;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4CB;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4CA;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4C9;struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4C8;struct Cyc_CfFlowInfo_NotZeroThis_struct _tmp4C7;struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4C6;struct Cyc_CfFlowInfo_NotZeroAll_struct _tmp4C5;struct Cyc_CfFlowInfo_Zero_struct*
_tmp4C4;struct Cyc_CfFlowInfo_Zero_struct _tmp4C3;struct Cyc_CfFlowInfo_FlowEnv*
_tmp4C2;return(_tmp4C2=_region_malloc(r,sizeof(*_tmp4C2)),((_tmp4C2->r=r,((
_tmp4C2->zero=(void*)((_tmp4C4=_region_malloc(r,sizeof(*_tmp4C4)),((_tmp4C4[0]=((
_tmp4C3.tag=0,_tmp4C3)),_tmp4C4)))),((_tmp4C2->notzeroall=(void*)((_tmp4C6=
_region_malloc(r,sizeof(*_tmp4C6)),((_tmp4C6[0]=((_tmp4C5.tag=1,_tmp4C5)),
_tmp4C6)))),((_tmp4C2->notzerothis=(void*)((_tmp4C8=_region_malloc(r,sizeof(*
_tmp4C8)),((_tmp4C8[0]=((_tmp4C7.tag=2,_tmp4C7)),_tmp4C8)))),((_tmp4C2->unknown_none=(
void*)((_tmp4CA=_region_malloc(r,sizeof(*_tmp4CA)),((_tmp4CA[0]=((_tmp4C9.tag=3,((
_tmp4C9.f1=Cyc_CfFlowInfo_NoneIL,_tmp4C9)))),_tmp4CA)))),((_tmp4C2->unknown_this=(
void*)((_tmp4CC=_region_malloc(r,sizeof(*_tmp4CC)),((_tmp4CC[0]=((_tmp4CB.tag=3,((
_tmp4CB.f1=Cyc_CfFlowInfo_ThisIL,_tmp4CB)))),_tmp4CC)))),((_tmp4C2->unknown_all=(
void*)((_tmp4CE=_region_malloc(r,sizeof(*_tmp4CE)),((_tmp4CE[0]=((_tmp4CD.tag=3,((
_tmp4CD.f1=Cyc_CfFlowInfo_AllIL,_tmp4CD)))),_tmp4CE)))),((_tmp4C2->esc_none=(
void*)((_tmp4D0=_region_malloc(r,sizeof(*_tmp4D0)),((_tmp4D0[0]=((_tmp4CF.tag=4,((
_tmp4CF.f1=Cyc_CfFlowInfo_NoneIL,_tmp4CF)))),_tmp4D0)))),((_tmp4C2->esc_this=(
void*)((_tmp4D2=_region_malloc(r,sizeof(*_tmp4D2)),((_tmp4D2[0]=((_tmp4D1.tag=4,((
_tmp4D1.f1=Cyc_CfFlowInfo_ThisIL,_tmp4D1)))),_tmp4D2)))),((_tmp4C2->esc_all=(
void*)((_tmp4D4=_region_malloc(r,sizeof(*_tmp4D4)),((_tmp4D4[0]=((_tmp4D3.tag=4,((
_tmp4D3.f1=Cyc_CfFlowInfo_AllIL,_tmp4D3)))),_tmp4D4)))),((_tmp4C2->mt_flowdict=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(
r,Cyc_CfFlowInfo_root_cmp),((_tmp4C2->mt_place_set=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp4C2->dummy_place=((_tmp4D7=
_region_malloc(r,sizeof(*_tmp4D7)),((_tmp4D7->root=(void*)((_tmp4D5=
_region_malloc(r,sizeof(*_tmp4D5)),((_tmp4D5[0]=((_tmp4D6.tag=1,((_tmp4D6.f1=&
dummy_exp,((_tmp4D6.f2=(void*)((void*)& Cyc_Absyn_VoidType_val),_tmp4D6)))))),
_tmp4D5)))),((_tmp4D7->fields=0,_tmp4D7)))))),_tmp4C2)))))))))))))))))))))))))));}
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,
unsigned int loc){if(set != 0){if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*
set,place))return 1;else{*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*
k,unsigned int v))Cyc_Dict_insert)(*set,place,loc);}}return 0;}struct
_dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
void*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_CfFlowInfo_Place _tmp1F=*place;
_tmp20=_tmp1F.root;_tmp21=_tmp1F.fields;{void*_tmp22=_tmp20;struct Cyc_Absyn_Vardecl*
_tmp24;_LL1: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp23=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp22;if(_tmp23->tag != 0)goto _LL3;else{_tmp24=_tmp23->f1;}}_LL2: if(_tmp21 == 0)
return Cyc_Absynpp_qvar2string(_tmp24->name);else{const char*_tmp4DB;void*_tmp4DA[
1];struct Cyc_String_pa_struct _tmp4D9;return(struct _dyneither_ptr)((_tmp4D9.tag=0,((
_tmp4D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp24->name)),((_tmp4DA[0]=& _tmp4D9,Cyc_aprintf(((_tmp4DB="reachable from %s",
_tag_dyneither(_tmp4DB,sizeof(char),18))),_tag_dyneither(_tmp4DA,sizeof(void*),1))))))));}
_LL3:;_LL4: {const char*_tmp4DE;void*_tmp4DD;(_tmp4DD=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4DE="error locations not for VarRoots",
_tag_dyneither(_tmp4DE,sizeof(char),33))),_tag_dyneither(_tmp4DD,sizeof(void*),0)));}
_LL0:;};}static unsigned int Cyc_CfFlowInfo_combine_consume_f(void*env,void*place,
unsigned int loc1,unsigned int loc2){return loc1;}struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){int _tmp2A=0;struct Cyc_Dict_Dict
_tmp2B=((struct Cyc_Dict_Dict(*)(unsigned int(*f)(int*,struct Cyc_CfFlowInfo_Place*,
unsigned int,unsigned int),int*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))
Cyc_Dict_union_two_c)((unsigned int(*)(int*env,struct Cyc_CfFlowInfo_Place*place,
unsigned int loc1,unsigned int loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp2A,s1,
s2);return _tmp2B;}struct _tuple14{void*f1;unsigned int f2;};int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))
_tmp2E=1;if(!_tmp2E){{struct _RegionHandle _tmp2F=_new_region("temp");struct
_RegionHandle*temp=& _tmp2F;_push_region(temp);{struct Cyc_Iter_Iter iter=((struct
Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
temp,s1);struct _tuple14 _tmp30=*((struct _tuple14*(*)(struct _RegionHandle*,struct
Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple14*))Cyc_Iter_next)(iter,& _tmp30)){void*_tmp31=_tmp30.f1;if(!((int(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp31)){int _tmp32=0;
_npop_handler(1);return _tmp32;}}{int _tmp33=1;_npop_handler(1);return _tmp33;};};
_pop_region(temp);};_pop_handler();}else{void*_tmp2D=(void*)_exn_thrown;void*
_tmp35=_tmp2D;_LL6:;_LL7: return 1;_LL8:;_LL9:(void)_throw(_tmp35);_LL5:;}};};}int
Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct
_dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=fs->tl){struct _dyneither_ptr*
_tmp36=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;if(Cyc_strptrcmp(_tmp36,f)== 0)
return n;n=n + 1;}{struct Cyc_Core_Impossible_struct _tmp4EB;const char*_tmp4EA;void*
_tmp4E9[1];struct Cyc_String_pa_struct _tmp4E8;struct Cyc_Core_Impossible_struct*
_tmp4E7;(int)_throw((void*)((_tmp4E7=_cycalloc(sizeof(*_tmp4E7)),((_tmp4E7[0]=((
_tmp4EB.tag=Cyc_Core_Impossible,((_tmp4EB.f1=(struct _dyneither_ptr)((_tmp4E8.tag=
0,((_tmp4E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp4E9[0]=&
_tmp4E8,Cyc_aprintf(((_tmp4EA="get_field_index_fs failed: %s",_tag_dyneither(
_tmp4EA,sizeof(char),30))),_tag_dyneither(_tmp4E9,sizeof(void*),1)))))))),
_tmp4EB)))),_tmp4E7)))));};}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f){void*_tmp3C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp3E;union Cyc_Absyn_AggrInfoU _tmp3F;struct Cyc_List_List*_tmp41;_LLB: {struct
Cyc_Absyn_AggrType_struct*_tmp3D=(struct Cyc_Absyn_AggrType_struct*)_tmp3C;if(
_tmp3D->tag != 11)goto _LLD;else{_tmp3E=_tmp3D->f1;_tmp3F=_tmp3E.aggr_info;}}_LLC: {
struct Cyc_List_List*_tmp42=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(
_tmp3F))->impl))->fields;_tmp41=_tmp42;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp40=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp3C;if(_tmp40->tag != 12)goto _LLF;
else{_tmp41=_tmp40->f2;}}_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp41,f);
_LLF:;_LL10: {struct Cyc_Core_Impossible_struct _tmp4F8;const char*_tmp4F7;void*
_tmp4F6[1];struct Cyc_String_pa_struct _tmp4F5;struct Cyc_Core_Impossible_struct*
_tmp4F4;(int)_throw((void*)((_tmp4F4=_cycalloc(sizeof(*_tmp4F4)),((_tmp4F4[0]=((
_tmp4F8.tag=Cyc_Core_Impossible,((_tmp4F8.f1=(struct _dyneither_ptr)((_tmp4F5.tag=
0,((_tmp4F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp4F6[0]=& _tmp4F5,Cyc_aprintf(((_tmp4F7="get_field_index failed: %s",
_tag_dyneither(_tmp4F7,sizeof(char),27))),_tag_dyneither(_tmp4F6,sizeof(void*),1)))))))),
_tmp4F8)))),_tmp4F4)))));}_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((
int)r1 == (int)r2)return 0;{struct _tuple0 _tmp4F9;struct _tuple0 _tmp49=(_tmp4F9.f1=
r1,((_tmp4F9.f2=r2,_tmp4F9)));void*_tmp4A;struct Cyc_Absyn_Vardecl*_tmp4C;void*
_tmp4D;struct Cyc_Absyn_Vardecl*_tmp4F;void*_tmp50;void*_tmp52;void*_tmp54;struct
Cyc_Absyn_Exp*_tmp56;void*_tmp57;struct Cyc_Absyn_Exp*_tmp59;void*_tmp5A;void*
_tmp5C;void*_tmp5E;int _tmp60;void*_tmp61;int _tmp63;_LL12: _tmp4A=_tmp49.f1;{
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp4B=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp4A;if(_tmp4B->tag != 0)goto _LL14;else{_tmp4C=_tmp4B->f1;}};_tmp4D=_tmp49.f2;{
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp4E=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp4D;if(_tmp4E->tag != 0)goto _LL14;else{_tmp4F=_tmp4E->f1;}};_LL13: return(int)
_tmp4C - (int)_tmp4F;_LL14: _tmp50=_tmp49.f1;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp51=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp50;if(_tmp51->tag != 0)goto _LL16;};
_LL15: return - 1;_LL16: _tmp52=_tmp49.f2;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp53=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp52;if(_tmp53->tag != 0)goto _LL18;};
_LL17: return 1;_LL18: _tmp54=_tmp49.f1;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp55=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp54;if(_tmp55->tag != 1)goto
_LL1A;else{_tmp56=_tmp55->f1;}};_tmp57=_tmp49.f2;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp58=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp57;if(_tmp58->tag != 1)goto
_LL1A;else{_tmp59=_tmp58->f1;}};_LL19: return(int)_tmp56 - (int)_tmp59;_LL1A:
_tmp5A=_tmp49.f1;{struct Cyc_CfFlowInfo_MallocPt_struct*_tmp5B=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp5A;if(_tmp5B->tag != 1)goto _LL1C;};_LL1B: return - 1;_LL1C: _tmp5C=_tmp49.f2;{
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp5D=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp5C;if(_tmp5D->tag != 1)goto _LL1E;};_LL1D: return 1;_LL1E: _tmp5E=_tmp49.f1;{
struct Cyc_CfFlowInfo_InitParam_struct*_tmp5F=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp5E;if(_tmp5F->tag != 2)goto _LL11;else{_tmp60=_tmp5F->f1;}};_tmp61=_tmp49.f2;{
struct Cyc_CfFlowInfo_InitParam_struct*_tmp62=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp61;if(_tmp62->tag != 2)goto _LL11;else{_tmp63=_tmp62->f1;}};_LL1F: return _tmp60
- _tmp63;_LL11:;};}static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,
struct Cyc_List_List*list2){return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}int
Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*
p2){if((int)p1 == (int)p2)return 0;{void*_tmp65;struct Cyc_List_List*_tmp66;struct
Cyc_CfFlowInfo_Place _tmp64=*p1;_tmp65=_tmp64.root;_tmp66=_tmp64.fields;{void*
_tmp68;struct Cyc_List_List*_tmp69;struct Cyc_CfFlowInfo_Place _tmp67=*p2;_tmp68=
_tmp67.root;_tmp69=_tmp67.fields;{int i=Cyc_CfFlowInfo_root_cmp(_tmp65,_tmp68);
if(i != 0)return i;return Cyc_CfFlowInfo_intlist_cmp(_tmp66,_tmp69);};};};}static
struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_List_List*sl=0;void*_tmp6B;struct Cyc_List_List*_tmp6C;struct Cyc_CfFlowInfo_Place
_tmp6A=*p;_tmp6B=_tmp6A.root;_tmp6C=_tmp6A.fields;{void*_tmp6D=_tmp6B;struct Cyc_Absyn_Vardecl*
_tmp6F;struct Cyc_Absyn_Exp*_tmp71;int _tmp73;_LL21: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp6E=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp6D;if(_tmp6E->tag != 0)goto _LL23;
else{_tmp6F=_tmp6E->f1;}}_LL22:{struct _dyneither_ptr*_tmp506;const char*_tmp505;
void*_tmp504[1];struct Cyc_String_pa_struct _tmp503;struct Cyc_List_List*_tmp502;sl=((
_tmp502=_cycalloc(sizeof(*_tmp502)),((_tmp502->hd=((_tmp506=_cycalloc(sizeof(*
_tmp506)),((_tmp506[0]=((_tmp503.tag=0,((_tmp503.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp6F->name).f2),((_tmp504[0]=& _tmp503,Cyc_aprintf(((
_tmp505="%s",_tag_dyneither(_tmp505,sizeof(char),3))),_tag_dyneither(_tmp504,
sizeof(void*),1)))))))),_tmp506)))),((_tmp502->tl=sl,_tmp502))))));}goto _LL20;
_LL23: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp70=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp6D;if(_tmp70->tag != 1)goto _LL25;else{_tmp71=_tmp70->f1;}}_LL24:{struct
_dyneither_ptr*_tmp513;const char*_tmp512;void*_tmp511[1];struct Cyc_Int_pa_struct
_tmp510;struct Cyc_List_List*_tmp50F;sl=((_tmp50F=_cycalloc(sizeof(*_tmp50F)),((
_tmp50F->hd=((_tmp513=_cycalloc(sizeof(*_tmp513)),((_tmp513[0]=((_tmp510.tag=1,((
_tmp510.f1=(unsigned long)((int)_tmp71),((_tmp511[0]=& _tmp510,Cyc_aprintf(((
_tmp512="mpt%d",_tag_dyneither(_tmp512,sizeof(char),6))),_tag_dyneither(_tmp511,
sizeof(void*),1)))))))),_tmp513)))),((_tmp50F->tl=sl,_tmp50F))))));}goto _LL20;
_LL25: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp72=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp6D;if(_tmp72->tag != 2)goto _LL20;else{_tmp73=_tmp72->f1;}}_LL26:{struct
_dyneither_ptr*_tmp520;const char*_tmp51F;void*_tmp51E[1];struct Cyc_Int_pa_struct
_tmp51D;struct Cyc_List_List*_tmp51C;sl=((_tmp51C=_cycalloc(sizeof(*_tmp51C)),((
_tmp51C->hd=((_tmp520=_cycalloc(sizeof(*_tmp520)),((_tmp520[0]=((_tmp51D.tag=1,((
_tmp51D.f1=(unsigned long)_tmp73,((_tmp51E[0]=& _tmp51D,Cyc_aprintf(((_tmp51F="param%d",
_tag_dyneither(_tmp51F,sizeof(char),8))),_tag_dyneither(_tmp51E,sizeof(void*),1)))))))),
_tmp520)))),((_tmp51C->tl=sl,_tmp51C))))));}goto _LL20;_LL20:;}for(0;_tmp6C != 0;
_tmp6C=_tmp6C->tl){struct _dyneither_ptr*_tmp52D;const char*_tmp52C;void*_tmp52B[1];
struct Cyc_Int_pa_struct _tmp52A;struct Cyc_List_List*_tmp529;sl=((_tmp529=
_cycalloc(sizeof(*_tmp529)),((_tmp529->hd=((_tmp52D=_cycalloc(sizeof(*_tmp52D)),((
_tmp52D[0]=((_tmp52A.tag=1,((_tmp52A.f1=(unsigned long)((int)_tmp6C->hd),((
_tmp52B[0]=& _tmp52A,Cyc_aprintf(((_tmp52C="%d",_tag_dyneither(_tmp52C,sizeof(
char),3))),_tag_dyneither(_tmp52B,sizeof(void*),1)))))))),_tmp52D)))),((_tmp529->tl=
sl,_tmp529))))));}{struct Cyc_String_pa_struct _tmp538;const char*_tmp537;void*
_tmp536[1];const char*_tmp535;struct _dyneither_ptr*_tmp534;struct _dyneither_ptr*
_tmp88=(_tmp534=_cycalloc(sizeof(*_tmp534)),((_tmp534[0]=((_tmp538.tag=0,((
_tmp538.f1=(struct _dyneither_ptr)((_tmp537="",_tag_dyneither(_tmp537,sizeof(char),
1))),((_tmp536[0]=& _tmp538,Cyc_aprintf(((_tmp535="%s",_tag_dyneither(_tmp535,
sizeof(char),3))),_tag_dyneither(_tmp536,sizeof(void*),1)))))))),_tmp534)));for(
0;sl != 0;sl=sl->tl){const char*_tmp53D;void*_tmp53C[2];struct Cyc_String_pa_struct
_tmp53B;struct Cyc_String_pa_struct _tmp53A;*_tmp88=((_tmp53A.tag=0,((_tmp53A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp88),((_tmp53B.tag=0,((_tmp53B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((
_tmp53C[0]=& _tmp53B,((_tmp53C[1]=& _tmp53A,Cyc_aprintf(((_tmp53D="%s.%s",
_tag_dyneither(_tmp53D,sizeof(char),6))),_tag_dyneither(_tmp53C,sizeof(void*),2))))))))))))));}
return _tmp88;};}static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct
_dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp543(
struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp542,unsigned int*_tmp541,
void***_tmp53F){for(*_tmp542=0;*_tmp542 < *_tmp541;(*_tmp542)++){(*_tmp53F)[*
_tmp542]=(*fenv)->unknown_all;}}struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,
void*leafval){unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
fs);unsigned int _tmp542;unsigned int _tmp541;struct _dyneither_ptr _tmp540;void**
_tmp53F;unsigned int _tmp53E;struct _dyneither_ptr d=(_tmp53E=sz,((_tmp53F=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp53E)),((_tmp540=
_tag_dyneither(_tmp53F,sizeof(void*),_tmp53E),((((_tmp541=_tmp53E,_tmp543(& fenv,&
_tmp542,& _tmp541,& _tmp53F))),_tmp540)))))));{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield _tmp93;struct _dyneither_ptr*_tmp94;void*_tmp95;struct
Cyc_Absyn_Aggrfield*_tmp92=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)
_check_null(fs))->hd;_tmp93=*_tmp92;_tmp94=_tmp93.name;_tmp95=_tmp93.type;if(
_get_dyneither_size(*_tmp94,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,no_init_bits_only,_tmp95,leafval);}}return d;}struct _tuple15{struct
_RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*f){struct _tuple15 _tmp9B;struct
_RegionHandle*_tmp9C;struct Cyc_List_List*_tmp9D;struct _tuple15*_tmp9A=env;_tmp9B=*
_tmp9A;_tmp9C=_tmp9B.f1;_tmp9D=_tmp9B.f2;{void*_tmp9E=Cyc_Tcutil_rsubstitute(
_tmp9C,_tmp9D,f->type);struct Cyc_Absyn_Aggrfield*_tmp544;return(_tmp544=
_region_malloc(_tmp9C,sizeof(*_tmp544)),((_tmp544->name=f->name,((_tmp544->tq=f->tq,((
_tmp544->type=_tmp9E,((_tmp544->width=f->width,((_tmp544->attributes=f->attributes,
_tmp544)))))))))));};}static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*
targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){struct
_RegionHandle _tmpA0=_new_region("temp");struct _RegionHandle*temp=& _tmpA0;
_push_region(temp);{struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,tvs,targs);struct _tuple15 _tmp545;struct _tuple15
_tmpA1=(_tmp545.f1=temp,((_tmp545.f2=inst,_tmp545)));struct Cyc_List_List*subs_fs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(
struct _tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmpA1,fs);struct
_dyneither_ptr _tmpA2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,
no_init_bits_only,leafval);_npop_handler(0);return _tmpA2;};_pop_region(temp);}
struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,
void*leafval);static void _tmp54B(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*
_tmp54A,unsigned int*_tmp549,void***_tmp547){for(*_tmp54A=0;*_tmp54A < *_tmp549;(*
_tmp54A)++){(*_tmp547)[*_tmp54A]=(*fenv)->unknown_all;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,
void*leafval){if(Cyc_Absyn_is_nontagged_union_type(t))return fenv->unknown_all;{
void*_tmpA4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_DatatypeFieldInfo _tmpA6;union
Cyc_Absyn_DatatypeFieldInfoU _tmpA7;struct _tuple2 _tmpA8;struct Cyc_Absyn_Datatypefield*
_tmpA9;struct Cyc_List_List*_tmpAB;struct Cyc_Absyn_AggrInfo _tmpAD;union Cyc_Absyn_AggrInfoU
_tmpAE;struct Cyc_List_List*_tmpAF;enum Cyc_Absyn_AggrKind _tmpB1;struct Cyc_List_List*
_tmpB2;struct Cyc_Absyn_ArrayInfo _tmpB4;void*_tmpB5;union Cyc_Absyn_Constraint*
_tmpB6;void*_tmpB8;struct Cyc_Absyn_PtrInfo _tmpBA;struct Cyc_Absyn_PtrAtts _tmpBB;
union Cyc_Absyn_Constraint*_tmpBC;_LL28: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpA5=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpA4;if(_tmpA5->tag != 4)goto
_LL2A;else{_tmpA6=_tmpA5->f1;_tmpA7=_tmpA6.field_info;if((_tmpA7.KnownDatatypefield).tag
!= 2)goto _LL2A;_tmpA8=(struct _tuple2)(_tmpA7.KnownDatatypefield).val;_tmpA9=
_tmpA8.f2;}}_LL29: if(_tmpA9->typs == 0)return leafval;_tmpAB=_tmpA9->typs;goto
_LL2B;_LL2A: {struct Cyc_Absyn_TupleType_struct*_tmpAA=(struct Cyc_Absyn_TupleType_struct*)
_tmpA4;if(_tmpAA->tag != 10)goto _LL2C;else{_tmpAB=_tmpAA->f1;}}_LL2B: {
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpAB);unsigned int _tmp54A;unsigned int _tmp549;struct _dyneither_ptr _tmp548;void**
_tmp547;unsigned int _tmp546;struct _dyneither_ptr d=(_tmp546=sz,((_tmp547=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp546)),((_tmp548=
_tag_dyneither(_tmp547,sizeof(void*),_tmp546),((((_tmp549=_tmp546,_tmp54B(& fenv,&
_tmp54A,& _tmp549,& _tmp547))),_tmp548)))))));{int i=0;for(0;i < sz;++ i){*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,no_init_bits_only,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(
_tmpAB))->hd)).f2,leafval);_tmpAB=_tmpAB->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp551;struct Cyc_CfFlowInfo_UnionRInfo _tmp550;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp54F;return(void*)((_tmp54F=_region_malloc(fenv->r,sizeof(*_tmp54F)),((
_tmp54F[0]=((_tmp551.tag=6,((_tmp551.f1=((_tmp550.is_union=0,((_tmp550.fieldnum=
- 1,_tmp550)))),((_tmp551.f2=d,_tmp551)))))),_tmp54F))));};}_LL2C: {struct Cyc_Absyn_AggrType_struct*
_tmpAC=(struct Cyc_Absyn_AggrType_struct*)_tmpA4;if(_tmpAC->tag != 11)goto _LL2E;
else{_tmpAD=_tmpAC->f1;_tmpAE=_tmpAD.aggr_info;_tmpAF=_tmpAD.targs;}}_LL2D: {
struct Cyc_Absyn_Aggrdecl*_tmpC4=Cyc_Absyn_get_known_aggrdecl(_tmpAE);if(_tmpC4->impl
== 0)goto _LL27;{struct Cyc_List_List*_tmpC5=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpC4->impl))->fields;if(_tmpAF == 0){_tmpB1=_tmpC4->kind;_tmpB2=
_tmpC5;goto _LL2F;}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp557;struct Cyc_CfFlowInfo_UnionRInfo
_tmp556;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp555;return(void*)((_tmp555=
_region_malloc(fenv->r,sizeof(*_tmp555)),((_tmp555[0]=((_tmp557.tag=6,((_tmp557.f1=((
_tmp556.is_union=_tmpC4->kind == Cyc_Absyn_UnionA,((_tmp556.fieldnum=- 1,_tmp556)))),((
_tmp557.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpC4->tvs,
_tmpAF,_tmpC5,_tmpC4->kind == Cyc_Absyn_UnionA,leafval),_tmp557)))))),_tmp555))));};};}
_LL2E: {struct Cyc_Absyn_AnonAggrType_struct*_tmpB0=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA4;if(_tmpB0->tag != 12)goto _LL30;else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB0->f2;}}
_LL2F: {struct Cyc_CfFlowInfo_Aggregate_struct _tmp55D;struct Cyc_CfFlowInfo_UnionRInfo
_tmp55C;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp55B;return(void*)((_tmp55B=
_region_malloc(fenv->r,sizeof(*_tmp55B)),((_tmp55B[0]=((_tmp55D.tag=6,((_tmp55D.f1=((
_tmp55C.is_union=_tmpB1 == Cyc_Absyn_UnionA,((_tmp55C.fieldnum=- 1,_tmp55C)))),((
_tmp55D.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpB2,_tmpB1 == Cyc_Absyn_UnionA,
leafval),_tmp55D)))))),_tmp55B))));}_LL30:{struct Cyc_Absyn_ArrayType_struct*
_tmpB3=(struct Cyc_Absyn_ArrayType_struct*)_tmpA4;if(_tmpB3->tag != 8)goto _LL32;
else{_tmpB4=_tmpB3->f1;_tmpB5=_tmpB4.elt_type;_tmpB6=_tmpB4.zero_term;}}if(!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB6))goto
_LL32;_LL31: return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(
_tmpB5)?fenv->unknown_all: leafval;_LL32: {struct Cyc_Absyn_TagType_struct*_tmpB7=(
struct Cyc_Absyn_TagType_struct*)_tmpA4;if(_tmpB7->tag != 19)goto _LL34;else{_tmpB8=(
void*)_tmpB7->f1;}}_LL33: return leafval;_LL34:{struct Cyc_Absyn_PointerType_struct*
_tmpB9=(struct Cyc_Absyn_PointerType_struct*)_tmpA4;if(_tmpB9->tag != 5)goto _LL36;
else{_tmpBA=_tmpB9->f1;_tmpBB=_tmpBA.ptr_atts;_tmpBC=_tmpBB.nullable;}}if(!(!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBC)))goto
_LL36;_LL35:{void*_tmpCC=leafval;enum Cyc_CfFlowInfo_InitLevel _tmpCE;enum Cyc_CfFlowInfo_InitLevel
_tmpD0;_LL39: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpCD=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpCC;if(_tmpCD->tag != 3)goto _LL3B;else{_tmpCE=_tmpCD->f1;if(_tmpCE != Cyc_CfFlowInfo_ThisIL)
goto _LL3B;}}_LL3A: return fenv->notzerothis;_LL3B: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpCF=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpCC;if(_tmpCF->tag != 3)goto
_LL3D;else{_tmpD0=_tmpCF->f1;if(_tmpD0 != Cyc_CfFlowInfo_AllIL)goto _LL3D;}}_LL3C:
return fenv->notzeroall;_LL3D:;_LL3E: goto _LL38;_LL38:;}goto _LL27;_LL36:;_LL37:
goto _LL27;_LL27:;}return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all:
leafval;}void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
void*t,int no_init_bits_only,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,1,no_init_bits_only,t,leafval);}int Cyc_CfFlowInfo_is_unique_consumed(struct
Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){void*_tmpD1=r;
struct Cyc_Absyn_Exp*_tmpD3;int _tmpD4;void*_tmpD5;struct Cyc_CfFlowInfo_UnionRInfo
_tmpD7;int _tmpD8;int _tmpD9;struct _dyneither_ptr _tmpDA;_LL40: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmpD2=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmpD1;if(_tmpD2->tag != 7)goto
_LL42;else{_tmpD3=_tmpD2->f1;_tmpD4=_tmpD2->f2;_tmpD5=(void*)_tmpD2->f3;}}_LL41:
if(_tmpD3 == e  && _tmpD4 == env_iteration){*needs_unconsume=1;return 0;}return 1;
_LL42: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmpD6=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpD1;if(_tmpD6->tag != 6)goto _LL44;else{_tmpD7=_tmpD6->f1;_tmpD8=_tmpD7.is_union;
_tmpD9=_tmpD7.fieldnum;_tmpDA=_tmpD6->f2;}}_LL43: if(!_tmpD8  || _tmpD9 == - 1){
unsigned int _tmpDB=_get_dyneither_size(_tmpDA,sizeof(void*));{int i=0;for(0;i < 
_tmpDB;++ i){if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)
_check_dyneither_subscript(_tmpDA,sizeof(void*),i)),needs_unconsume))return 1;}}
return 0;}else{return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)
_check_dyneither_subscript(_tmpDA,sizeof(void*),_tmpD9)),needs_unconsume);}_LL44:;
_LL45: return 0;_LL3F:;}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*
fenv,void*r);static void _tmp563(struct _dyneither_ptr*_tmpE3,unsigned int*_tmp562,
unsigned int*_tmp561,void***_tmp55F){for(*_tmp562=0;*_tmp562 < *_tmp561;(*_tmp562)
++){(*_tmp55F)[*_tmp562]=*((void**)_check_dyneither_subscript(*_tmpE3,sizeof(
void*),(int)*_tmp562));}}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*
fenv,void*r){void*_tmpDC=r;struct Cyc_Absyn_Exp*_tmpDE;int _tmpDF;void*_tmpE0;
struct Cyc_CfFlowInfo_UnionRInfo _tmpE2;struct _dyneither_ptr _tmpE3;_LL47: {struct
Cyc_CfFlowInfo_Consumed_struct*_tmpDD=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpDC;if(_tmpDD->tag != 7)goto _LL49;else{_tmpDE=_tmpDD->f1;_tmpDF=_tmpDD->f2;
_tmpE0=(void*)_tmpDD->f3;}}_LL48: return _tmpE0;_LL49: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpE1=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpDC;if(_tmpE1->tag != 6)goto
_LL4B;else{_tmpE2=_tmpE1->f1;_tmpE3=_tmpE1->f2;}}_LL4A: {unsigned int _tmpE4=
_get_dyneither_size(_tmpE3,sizeof(void*));int change=0;unsigned int _tmp562;
unsigned int _tmp561;struct _dyneither_ptr _tmp560;void**_tmp55F;unsigned int
_tmp55E;struct _dyneither_ptr d2=(_tmp55E=_tmpE4,((_tmp55F=(void**)_region_malloc(
fenv->r,_check_times(sizeof(void*),_tmp55E)),((_tmp560=_tag_dyneither(_tmp55F,
sizeof(void*),_tmp55E),((((_tmp561=_tmp55E,_tmp563(& _tmpE3,& _tmp562,& _tmp561,&
_tmp55F))),_tmp560)))))));{int i=0;for(0;i < _tmpE4;++ i){*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(
fenv,*((void**)_check_dyneither_subscript(_tmpE3,sizeof(void*),i)));if(*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmpE3,sizeof(void*),i)))change=1;}}if(change){struct
Cyc_CfFlowInfo_Aggregate_struct _tmp566;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp565;return(void*)((_tmp565=_region_malloc(fenv->r,sizeof(*_tmp565)),((
_tmp565[0]=((_tmp566.tag=6,((_tmp566.f1=_tmpE2,((_tmp566.f2=d2,_tmp566)))))),
_tmp565))));}else{return r;}}_LL4B:;_LL4C: return r;_LL46:;}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r);static void _tmp570(struct Cyc_CfFlowInfo_FlowEnv**fenv,
unsigned int*_tmp56F,unsigned int*_tmp56E,void***_tmp56C){for(*_tmp56F=0;*
_tmp56F < *_tmp56E;(*_tmp56F)++){(*_tmp56C)[*_tmp56F]=(*fenv)->unknown_all;}}
static void _tmp579(unsigned int*_tmp578,unsigned int*_tmp577,void***_tmp575,
struct _dyneither_ptr*_tmp105){for(*_tmp578=0;*_tmp578 < *_tmp577;(*_tmp578)++){(*
_tmp575)[*_tmp578]=((void**)(*_tmp105).curr)[(int)*_tmp578];}}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r){if(Cyc_Tcutil_is_noalias_pointer(t)){struct Cyc_CfFlowInfo_Consumed_struct
_tmp569;struct Cyc_CfFlowInfo_Consumed_struct*_tmp568;return(void*)((_tmp568=
_region_malloc(fenv->r,sizeof(*_tmp568)),((_tmp568[0]=((_tmp569.tag=7,((_tmp569.f1=
consumer,((_tmp569.f2=iteration,((_tmp569.f3=(void*)r,_tmp569)))))))),_tmp568))));}{
struct _tuple0 _tmp56A;struct _tuple0 _tmpEE=(_tmp56A.f1=Cyc_Tcutil_compress(t),((
_tmp56A.f2=r,_tmp56A)));void*_tmpEF;struct Cyc_List_List*_tmpF1;void*_tmpF2;
struct Cyc_CfFlowInfo_UnionRInfo _tmpF4;struct _dyneither_ptr _tmpF5;void*_tmpF6;
struct Cyc_Absyn_AggrInfo _tmpF8;union Cyc_Absyn_AggrInfoU _tmpF9;void*_tmpFA;struct
Cyc_CfFlowInfo_UnionRInfo _tmpFC;struct _dyneither_ptr _tmpFD;void*_tmpFE;enum Cyc_Absyn_AggrKind
_tmp100;struct Cyc_List_List*_tmp101;void*_tmp102;struct Cyc_CfFlowInfo_UnionRInfo
_tmp104;struct _dyneither_ptr _tmp105;_LL4E: _tmpEF=_tmpEE.f1;{struct Cyc_Absyn_TupleType_struct*
_tmpF0=(struct Cyc_Absyn_TupleType_struct*)_tmpEF;if(_tmpF0->tag != 10)goto _LL50;
else{_tmpF1=_tmpF0->f1;}};_tmpF2=_tmpEE.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpF3=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpF2;if(_tmpF3->tag != 6)goto
_LL50;else{_tmpF4=_tmpF3->f1;_tmpF5=_tmpF3->f2;}};_LL4F: {unsigned int _tmp106=
_get_dyneither_size(_tmpF5,sizeof(void*));unsigned int _tmp56F;unsigned int
_tmp56E;struct _dyneither_ptr _tmp56D;void**_tmp56C;unsigned int _tmp56B;struct
_dyneither_ptr d2=(_tmp56B=_tmp106,((_tmp56C=(void**)_region_malloc(fenv->r,
_check_times(sizeof(void*),_tmp56B)),((_tmp56D=_tag_dyneither(_tmp56C,sizeof(
void*),_tmp56B),((((_tmp56E=_tmp56B,_tmp570(& fenv,& _tmp56F,& _tmp56E,& _tmp56C))),
_tmp56D)))))));{int i=0;for(0;i < _tmp106;++ i){*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmpF1))->hd)).f2,
consumer,iteration,*((void**)_check_dyneither_subscript(_tmpF5,sizeof(void*),i)));
_tmpF1=_tmpF1->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp573;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp572;return(void*)((_tmp572=_region_malloc(fenv->r,sizeof(*_tmp572)),((
_tmp572[0]=((_tmp573.tag=6,((_tmp573.f1=_tmpF4,((_tmp573.f2=d2,_tmp573)))))),
_tmp572))));};}_LL50: _tmpF6=_tmpEE.f1;{struct Cyc_Absyn_AggrType_struct*_tmpF7=(
struct Cyc_Absyn_AggrType_struct*)_tmpF6;if(_tmpF7->tag != 11)goto _LL52;else{
_tmpF8=_tmpF7->f1;_tmpF9=_tmpF8.aggr_info;}};_tmpFA=_tmpEE.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpFB=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpFA;if(_tmpFB->tag != 6)goto
_LL52;else{_tmpFC=_tmpFB->f1;_tmpFD=_tmpFB->f2;}};_LL51: {struct Cyc_Absyn_Aggrdecl*
_tmp10D=Cyc_Absyn_get_known_aggrdecl(_tmpF9);if(_tmp10D->impl == 0)return r;
_tmp100=_tmp10D->kind;_tmp101=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp10D->impl))->fields;_tmp104=_tmpFC;_tmp105=_tmpFD;goto _LL53;}_LL52: _tmpFE=
_tmpEE.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmpFF=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpFE;if(_tmpFF->tag != 12)goto _LL54;else{_tmp100=_tmpFF->f1;_tmp101=_tmpFF->f2;}};
_tmp102=_tmpEE.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp103=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp102;if(_tmp103->tag != 6)goto _LL54;else{_tmp104=_tmp103->f1;_tmp105=_tmp103->f2;}};
_LL53: {unsigned int _tmp578;unsigned int _tmp577;struct _dyneither_ptr _tmp576;void**
_tmp575;unsigned int _tmp574;struct _dyneither_ptr d2=(_tmp574=_get_dyneither_size(
_tmp105,sizeof(void*)),((_tmp575=(void**)_region_malloc(fenv->r,_check_times(
sizeof(void*),_tmp574)),((_tmp576=_tag_dyneither(_tmp575,sizeof(void*),_tmp574),((((
_tmp577=_tmp574,_tmp579(& _tmp578,& _tmp577,& _tmp575,& _tmp105))),_tmp576)))))));
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp101);{int i=0;for(0;i < sz;(i ++,_tmp101=_tmp101->tl)){struct Cyc_Absyn_Aggrfield
_tmp10F;struct _dyneither_ptr*_tmp110;void*_tmp111;struct Cyc_Absyn_Aggrfield*
_tmp10E=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp101))->hd;
_tmp10F=*_tmp10E;_tmp110=_tmp10F.name;_tmp111=_tmp10F.type;if(
_get_dyneither_size(*_tmp110,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,_tmp111,consumer,iteration,*((void**)_check_dyneither_subscript(_tmp105,
sizeof(void*),i)));}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp57C;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp57B;return(void*)((_tmp57B=_region_malloc(fenv->r,sizeof(*_tmp57B)),((
_tmp57B[0]=((_tmp57C.tag=6,((_tmp57C.f1=_tmp104,((_tmp57C.f2=d2,_tmp57C)))))),
_tmp57B))));};}_LL54:;_LL55: return r;_LL4D:;};}struct _tuple17{struct Cyc_CfFlowInfo_Place*
f1;unsigned int f2;};static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Dict_Dict set){struct _RegionHandle _tmp118=_new_region("r");struct
_RegionHandle*r=& _tmp118;_push_region(r);{struct _tuple17 _tmp57D;struct _tuple17
elem=(_tmp57D.f1=place,((_tmp57D.f2=0,_tmp57D)));struct Cyc_Iter_Iter iter=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
r,set);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple17*))Cyc_Iter_next)(iter,&
elem)){struct Cyc_CfFlowInfo_Place*_tmp119=elem.f1;void*_tmp11B;struct Cyc_List_List*
_tmp11C;struct Cyc_CfFlowInfo_Place _tmp11A=*place;_tmp11B=_tmp11A.root;_tmp11C=
_tmp11A.fields;{void*_tmp11E;struct Cyc_List_List*_tmp11F;struct Cyc_CfFlowInfo_Place
_tmp11D=*_tmp119;_tmp11E=_tmp11D.root;_tmp11F=_tmp11D.fields;if(Cyc_CfFlowInfo_root_cmp(
_tmp11B,_tmp11E)!= 0)continue;for(0;_tmp11C != 0  && _tmp11F != 0;(_tmp11C=_tmp11C->tl,
_tmp11F=_tmp11F->tl)){if((int)_tmp11C->hd != (int)_tmp11F->hd)break;}if(_tmp11C == 
0){int _tmp120=1;_npop_handler(0);return _tmp120;}};}{int _tmp121=0;_npop_handler(0);
return _tmp121;};};_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct
_RegionHandle*rgn;struct Cyc_List_List*places;};typedef struct Cyc_CfFlowInfo_EscPile*
Cyc_CfFlowInfo_escpile_t;static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){struct Cyc_List_List*
_tmp57E;pile->places=((_tmp57E=_region_malloc(pile->rgn,sizeof(*_tmp57E)),((
_tmp57E->hd=place,((_tmp57E->tl=pile->places,_tmp57E))))));}}static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*r){void*_tmp124=r;void*_tmp126;struct Cyc_CfFlowInfo_Place*
_tmp128;struct Cyc_CfFlowInfo_UnionRInfo _tmp12A;struct _dyneither_ptr _tmp12B;_LL57: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmp125=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp124;if(_tmp125->tag != 7)goto _LL59;else{_tmp126=(void*)_tmp125->f3;}}_LL58:
Cyc_CfFlowInfo_add_places(pile,_tmp126);return;_LL59: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp127=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp124;if(_tmp127->tag != 5)goto
_LL5B;else{_tmp128=_tmp127->f1;}}_LL5A: Cyc_CfFlowInfo_add_place(pile,_tmp128);
return;_LL5B: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp129=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp124;if(_tmp129->tag != 6)goto _LL5D;else{_tmp12A=_tmp129->f1;_tmp12B=_tmp129->f2;}}
_LL5C:{int i=0;for(0;i < _get_dyneither_size(_tmp12B,sizeof(void*));++ i){Cyc_CfFlowInfo_add_places(
pile,((void**)_tmp12B.curr)[i]);}}return;_LL5D:;_LL5E: return;_LL56:;}static void*
Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*
old_val);static void _tmp584(struct _RegionHandle**r,void**new_val,unsigned int*
_tmp583,unsigned int*_tmp582,void***_tmp580,struct _dyneither_ptr*_tmp130){for(*
_tmp583=0;*_tmp583 < *_tmp582;(*_tmp583)++){(*_tmp580)[*_tmp583]=Cyc_CfFlowInfo_insert_place_inner(*
r,*new_val,((void**)(*_tmp130).curr)[(int)*_tmp583]);}}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmp12C=old_val;struct Cyc_CfFlowInfo_UnionRInfo
_tmp12E;int _tmp12F;struct _dyneither_ptr _tmp130;struct Cyc_Absyn_Exp*_tmp132;int
_tmp133;void*_tmp134;_LL60: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp12D=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp12C;if(_tmp12D->tag != 6)goto _LL62;
else{_tmp12E=_tmp12D->f1;_tmp12F=_tmp12E.is_union;_tmp130=_tmp12D->f2;}}_LL61: {
unsigned int _tmp583;unsigned int _tmp582;struct _dyneither_ptr _tmp581;void**
_tmp580;unsigned int _tmp57F;struct _dyneither_ptr d2=(_tmp57F=_get_dyneither_size(
_tmp130,sizeof(void*)),((_tmp580=(void**)_region_malloc(r,_check_times(sizeof(
void*),_tmp57F)),((_tmp581=_tag_dyneither(_tmp580,sizeof(void*),_tmp57F),((((
_tmp582=_tmp57F,_tmp584(& r,& new_val,& _tmp583,& _tmp582,& _tmp580,& _tmp130))),
_tmp581)))))));struct Cyc_CfFlowInfo_Aggregate_struct _tmp58A;struct Cyc_CfFlowInfo_UnionRInfo
_tmp589;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp588;return(void*)((_tmp588=
_region_malloc(r,sizeof(*_tmp588)),((_tmp588[0]=((_tmp58A.tag=6,((_tmp58A.f1=((
_tmp589.is_union=_tmp12F,((_tmp589.fieldnum=- 1,_tmp589)))),((_tmp58A.f2=d2,
_tmp58A)))))),_tmp588))));}_LL62: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp131=(
struct Cyc_CfFlowInfo_Consumed_struct*)_tmp12C;if(_tmp131->tag != 7)goto _LL64;
else{_tmp132=_tmp131->f1;_tmp133=_tmp131->f2;_tmp134=(void*)_tmp131->f3;}}_LL63: {
struct Cyc_CfFlowInfo_Consumed_struct _tmp58D;struct Cyc_CfFlowInfo_Consumed_struct*
_tmp58C;return(void*)((_tmp58C=_region_malloc(r,sizeof(*_tmp58C)),((_tmp58C[0]=((
_tmp58D.tag=7,((_tmp58D.f1=_tmp132,((_tmp58D.f2=_tmp133,((_tmp58D.f3=(void*)Cyc_CfFlowInfo_insert_place_inner(
r,new_val,_tmp134),_tmp58D)))))))),_tmp58C))));}_LL64:;_LL65: return new_val;_LL5F:;}
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,
struct _dyneither_ptr d,int n,void*rval);static void _tmp593(struct _dyneither_ptr*d,
unsigned int*_tmp592,unsigned int*_tmp591,void***_tmp58F){for(*_tmp592=0;*
_tmp592 < *_tmp591;(*_tmp592)++){(*_tmp58F)[*_tmp592]=((void**)(*d).curr)[(int)*
_tmp592];}}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmp13E=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmp13E == rval)return d;{
unsigned int _tmp592;unsigned int _tmp591;struct _dyneither_ptr _tmp590;void**
_tmp58F;unsigned int _tmp58E;struct _dyneither_ptr res=(_tmp58E=_get_dyneither_size(
d,sizeof(void*)),((_tmp58F=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp58E)),((_tmp590=_tag_dyneither(_tmp58F,sizeof(void*),_tmp58E),((((_tmp591=
_tmp58E,_tmp593(& d,& _tmp592,& _tmp591,& _tmp58F))),_tmp590)))))));*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;};}struct
_tuple18{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 
0)return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{struct _tuple18
_tmp594;struct _tuple18 _tmp144=(_tmp594.f1=fs,((_tmp594.f2=old_val,_tmp594)));
struct Cyc_List_List*_tmp145;struct Cyc_List_List _tmp146;int _tmp147;struct Cyc_List_List*
_tmp148;void*_tmp149;struct Cyc_CfFlowInfo_UnionRInfo _tmp14B;int _tmp14C;struct
_dyneither_ptr _tmp14D;_LL67: _tmp145=_tmp144.f1;if(_tmp145 == 0)goto _LL69;_tmp146=*
_tmp145;_tmp147=(int)_tmp146.hd;_tmp148=_tmp146.tl;_tmp149=_tmp144.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp14A=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp149;if(_tmp14A->tag != 6)goto
_LL69;else{_tmp14B=_tmp14A->f1;_tmp14C=_tmp14B.is_union;_tmp14D=_tmp14A->f2;}};
_LL68: {void*_tmp14E=Cyc_CfFlowInfo_insert_place_outer(r,_tmp148,*((void**)
_check_dyneither_subscript(_tmp14D,sizeof(void*),_tmp147)),new_val);struct
_dyneither_ptr _tmp14F=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp14D,_tmp147,_tmp14E);
if((void**)_tmp14F.curr == (void**)_tmp14D.curr)return old_val;else{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp59A;struct Cyc_CfFlowInfo_UnionRInfo _tmp599;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp598;return(void*)((_tmp598=_region_malloc(r,sizeof(*_tmp598)),((_tmp598[0]=((
_tmp59A.tag=6,((_tmp59A.f1=((_tmp599.is_union=_tmp14C,((_tmp599.fieldnum=- 1,
_tmp599)))),((_tmp59A.f2=_tmp14F,_tmp59A)))))),_tmp598))));}}_LL69:;_LL6A: {
struct Cyc_Core_Impossible_struct _tmp5A0;const char*_tmp59F;struct Cyc_Core_Impossible_struct*
_tmp59E;(int)_throw((void*)((_tmp59E=_cycalloc(sizeof(*_tmp59E)),((_tmp59E[0]=((
_tmp5A0.tag=Cyc_Core_Impossible,((_tmp5A0.f1=((_tmp59F="bad insert place",
_tag_dyneither(_tmp59F,sizeof(char),17))),_tmp5A0)))),_tmp59E)))));}_LL66:;};}
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict
d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*_tmp156=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)
_check_null(pile->places))->tl;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp156,0);{
void*oldval;void*newval;{struct _handler_cons _tmp157;_push_handler(& _tmp157);{int
_tmp159=0;if(setjmp(_tmp157.handler))_tmp159=1;if(!_tmp159){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmp156);;_pop_handler();}else{void*_tmp158=(void*)_exn_thrown;void*_tmp15B=
_tmp158;_LL6C: {struct Cyc_Dict_Absent_struct*_tmp15C=(struct Cyc_Dict_Absent_struct*)
_tmp15B;if(_tmp15C->tag != Cyc_Dict_Absent)goto _LL6E;}_LL6D: continue;_LL6E:;_LL6F:(
void)_throw(_tmp15B);_LL6B:;}};}switch(Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){
case Cyc_CfFlowInfo_AllIL: _LL70: newval=fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL:
_LL71: newval=fenv->esc_this;break;case Cyc_CfFlowInfo_NoneIL: _LL72: newval=fenv->esc_none;
break;}Cyc_CfFlowInfo_add_places(pile,oldval);{void*_tmp15E;struct Cyc_List_List*
_tmp15F;struct Cyc_CfFlowInfo_Place _tmp15D=*_tmp156;_tmp15E=_tmp15D.root;_tmp15F=
_tmp15D.fields;d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))
Cyc_Dict_insert)(d,_tmp15E,Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp15F,((
void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp15E),newval));};};}
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict
d;struct Cyc_List_List*seen;};static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(
void*r){void*_tmp160=r;enum Cyc_CfFlowInfo_InitLevel _tmp162;enum Cyc_CfFlowInfo_InitLevel
_tmp164;void*_tmp169;_LL75: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp161=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp160;if(_tmp161->tag != 3)goto _LL77;
else{_tmp162=_tmp161->f1;}}_LL76: return _tmp162;_LL77: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp163=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp160;if(_tmp163->tag != 4)goto _LL79;
else{_tmp164=_tmp163->f1;}}_LL78: return _tmp164;_LL79: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp165=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp160;if(_tmp165->tag != 0)goto _LL7B;}
_LL7A: goto _LL7C;_LL7B: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp166=(struct
Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp160;if(_tmp166->tag != 1)goto _LL7D;}_LL7C:
return Cyc_CfFlowInfo_AllIL;_LL7D: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp167=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp160;if(_tmp167->tag != 2)
goto _LL7F;}_LL7E: return Cyc_CfFlowInfo_ThisIL;_LL7F: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp168=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp160;if(_tmp168->tag != 7)goto
_LL81;else{_tmp169=(void*)_tmp168->f3;}}_LL80: return Cyc_CfFlowInfo_NoneIL;_LL81:;
_LL82: {struct Cyc_Core_Impossible_struct _tmp5A6;const char*_tmp5A5;struct Cyc_Core_Impossible_struct*
_tmp5A4;(int)_throw((void*)((_tmp5A4=_cycalloc(sizeof(*_tmp5A4)),((_tmp5A4[0]=((
_tmp5A6.tag=Cyc_Core_Impossible,((_tmp5A6.f1=((_tmp5A5="initlevel_approx",
_tag_dyneither(_tmp5A5,sizeof(char),17))),_tmp5A6)))),_tmp5A4)))));}_LL74:;}
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){enum Cyc_CfFlowInfo_InitLevel
this_ans;if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;{void*
_tmp16D=r;struct Cyc_CfFlowInfo_UnionRInfo _tmp16F;int _tmp170;int _tmp171;struct
_dyneither_ptr _tmp172;struct Cyc_CfFlowInfo_UnionRInfo _tmp174;int _tmp175;struct
_dyneither_ptr _tmp176;struct Cyc_CfFlowInfo_Place*_tmp178;_LL84: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp16E=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp16D;if(_tmp16E->tag != 6)goto
_LL86;else{_tmp16F=_tmp16E->f1;_tmp170=_tmp16F.is_union;if(_tmp170 != 1)goto _LL86;
_tmp171=_tmp16F.fieldnum;_tmp172=_tmp16E->f2;}}_LL85: {unsigned int _tmp179=
_get_dyneither_size(_tmp172,sizeof(void*));this_ans=Cyc_CfFlowInfo_NoneIL;if(
_tmp171 == - 1){int i=0;for(0;i < _tmp179;++ i){if(Cyc_CfFlowInfo_initlevel_rec(env,*((
void**)_check_dyneither_subscript(_tmp172,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL){this_ans=Cyc_CfFlowInfo_AllIL;break;}}}else{if(Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp172,sizeof(void*),_tmp171)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL)this_ans=Cyc_CfFlowInfo_AllIL;}goto _LL83;}_LL86: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp173=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp16D;if(_tmp173->tag != 6)goto _LL88;else{_tmp174=_tmp173->f1;_tmp175=_tmp174.is_union;
if(_tmp175 != 0)goto _LL88;_tmp176=_tmp173->f2;}}_LL87: {unsigned int _tmp17A=
_get_dyneither_size(_tmp176,sizeof(void*));this_ans=Cyc_CfFlowInfo_AllIL;{int i=0;
for(0;i < _tmp17A;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)
_check_dyneither_subscript(_tmp176,sizeof(void*),i)),this_ans);}}goto _LL83;}
_LL88: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp177=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp16D;if(_tmp177->tag != 5)goto _LL8A;else{_tmp178=_tmp177->f1;}}_LL89: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp178))this_ans=Cyc_CfFlowInfo_AllIL;else{{struct Cyc_List_List*_tmp5A7;env->seen=((
_tmp5A7=_region_malloc(env->e,sizeof(*_tmp5A7)),((_tmp5A7->hd=_tmp178,((_tmp5A7->tl=
env->seen,_tmp5A7))))));}this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(
env->d,_tmp178),Cyc_CfFlowInfo_AllIL);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == Cyc_CfFlowInfo_NoneIL)this_ans=Cyc_CfFlowInfo_ThisIL;}
goto _LL83;_LL8A:;_LL8B: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL83:;}if(
this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;if(this_ans == Cyc_CfFlowInfo_ThisIL
 || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;return Cyc_CfFlowInfo_AllIL;}
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*
env,struct Cyc_Dict_Dict d,void*r){struct _RegionHandle*_tmp17C=env->r;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp5A8;struct Cyc_CfFlowInfo_InitlevelEnv _tmp17D=(_tmp5A8.e=_tmp17C,((_tmp5A8.d=
d,((_tmp5A8.seen=0,_tmp5A8)))));return Cyc_CfFlowInfo_initlevel_rec(& _tmp17D,r,
Cyc_CfFlowInfo_AllIL);}struct _tuple19{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){void*_tmp180;struct Cyc_List_List*
_tmp181;struct Cyc_CfFlowInfo_Place _tmp17F=*place;_tmp180=_tmp17F.root;_tmp181=
_tmp17F.fields;{void*_tmp182=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,_tmp180);for(0;_tmp181 != 0;_tmp181=_tmp181->tl){struct _tuple19 _tmp5A9;struct
_tuple19 _tmp184=(_tmp5A9.f1=_tmp182,((_tmp5A9.f2=(int)_tmp181->hd,_tmp5A9)));
void*_tmp185;struct Cyc_CfFlowInfo_UnionRInfo _tmp187;struct _dyneither_ptr _tmp188;
int _tmp189;_LL8D: _tmp185=_tmp184.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp186=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp185;if(_tmp186->tag != 6)goto
_LL8F;else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;}};_tmp189=_tmp184.f2;_LL8E:
_tmp182=*((void**)_check_dyneither_subscript(_tmp188,sizeof(void*),_tmp189));
goto _LL8C;_LL8F:;_LL90: {struct Cyc_Core_Impossible_struct _tmp5AF;const char*
_tmp5AE;struct Cyc_Core_Impossible_struct*_tmp5AD;(int)_throw((void*)((_tmp5AD=
_cycalloc(sizeof(*_tmp5AD)),((_tmp5AD[0]=((_tmp5AF.tag=Cyc_Core_Impossible,((
_tmp5AF.f1=((_tmp5AE="bad lookup_place",_tag_dyneither(_tmp5AE,sizeof(char),17))),
_tmp5AF)))),_tmp5AD)))));}_LL8C:;}return _tmp182;};}static int Cyc_CfFlowInfo_is_rval_unescaped(
void*rval){void*_tmp18D=rval;void*_tmp190;struct Cyc_CfFlowInfo_UnionRInfo _tmp192;
int _tmp193;int _tmp194;struct _dyneither_ptr _tmp195;_LL92: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp18E=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp18D;if(_tmp18E->tag != 4)goto _LL94;}
_LL93: return 0;_LL94: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp18F=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp18D;if(_tmp18F->tag != 7)goto _LL96;else{_tmp190=(void*)_tmp18F->f3;}}_LL95:
return Cyc_CfFlowInfo_is_rval_unescaped(rval);_LL96: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp191=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp18D;if(_tmp191->tag != 6)goto
_LL98;else{_tmp192=_tmp191->f1;_tmp193=_tmp192.is_union;_tmp194=_tmp192.fieldnum;
_tmp195=_tmp191->f2;}}_LL97: if(_tmp193  && _tmp194 != - 1)return !Cyc_CfFlowInfo_is_rval_unescaped(*((
void**)_check_dyneither_subscript(_tmp195,sizeof(void*),_tmp194)));else{
unsigned int sz=_get_dyneither_size(_tmp195,sizeof(void*));{int i=0;for(0;i < sz;++
i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp195.curr)[i]))return 0;}}
return 1;}_LL98:;_LL99: return 1;_LL91:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(
d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct
_RegionHandle*_tmp196=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp5B0;struct Cyc_CfFlowInfo_EscPile*
pile=(_tmp5B0=_region_malloc(_tmp196,sizeof(*_tmp5B0)),((_tmp5B0->rgn=_tmp196,((
_tmp5B0->places=0,_tmp5B0)))));Cyc_CfFlowInfo_add_places(pile,r);return Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;
struct Cyc_CfFlowInfo_Place*root_place;struct Cyc_Dict_Dict*all_changed;};static
void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*
oldval,void*newval);static void _tmp5BA(struct Cyc_CfFlowInfo_AssignEnv**env,
unsigned int*_tmp5B9,unsigned int*_tmp5B8,void***_tmp5B6,struct _dyneither_ptr*
_tmp1A8,struct _dyneither_ptr*_tmp1A4){for(*_tmp5B9=0;*_tmp5B9 < *_tmp5B8;(*
_tmp5B9)++){(*_tmp5B6)[*_tmp5B9]=Cyc_CfFlowInfo_assign_place_inner(*env,((void**)(*
_tmp1A4).curr)[(int)*_tmp5B9],*((void**)_check_dyneither_subscript(*_tmp1A8,
sizeof(void*),(int)*_tmp5B9)));}}static void*Cyc_CfFlowInfo_assign_place_inner(
struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){struct _tuple0 _tmp5B1;
struct _tuple0 _tmp199=(_tmp5B1.f1=oldval,((_tmp5B1.f2=newval,_tmp5B1)));void*
_tmp19A;void*_tmp19C;struct Cyc_CfFlowInfo_Place*_tmp19E;void*_tmp19F;void*
_tmp1A1;struct Cyc_CfFlowInfo_UnionRInfo _tmp1A3;struct _dyneither_ptr _tmp1A4;void*
_tmp1A5;struct Cyc_CfFlowInfo_UnionRInfo _tmp1A7;struct _dyneither_ptr _tmp1A8;void*
_tmp1A9;enum Cyc_CfFlowInfo_InitLevel _tmp1AB;_LL9B: _tmp19A=_tmp199.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp19B=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp19A;if(_tmp19B->tag != 4)goto _LL9D;};
_tmp19C=_tmp199.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp19D=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp19C;if(_tmp19D->tag != 5)goto _LL9D;else{_tmp19E=_tmp19D->f1;}};_LL9C: Cyc_CfFlowInfo_add_place(
env->pile,_tmp19E);goto _LL9E;_LL9D: _tmp19F=_tmp199.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp1A0=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp19F;if(_tmp1A0->tag != 4)goto _LL9F;};
_LL9E: if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp5B4;void*_tmp5B3;(_tmp5B3=0,Cyc_Tcutil_terr(env->loc,((_tmp5B4="assignment puts possibly-uninitialized data in an escaped location",
_tag_dyneither(_tmp5B4,sizeof(char),67))),_tag_dyneither(_tmp5B3,sizeof(void*),0)));}
return(env->fenv)->esc_all;_LL9F: _tmp1A1=_tmp199.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp1A2=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1A1;if(_tmp1A2->tag != 6)goto
_LLA1;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f2;}};_tmp1A5=_tmp199.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp1A6=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1A5;if(_tmp1A6->tag != 6)goto _LLA1;else{_tmp1A7=_tmp1A6->f1;_tmp1A8=_tmp1A6->f2;}};
_LLA0: {unsigned int _tmp5B9;unsigned int _tmp5B8;struct _dyneither_ptr _tmp5B7;void**
_tmp5B6;unsigned int _tmp5B5;struct _dyneither_ptr new_d=(_tmp5B5=
_get_dyneither_size(_tmp1A4,sizeof(void*)),((_tmp5B6=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp5B5)),((_tmp5B7=_tag_dyneither(_tmp5B6,sizeof(
void*),_tmp5B5),((((_tmp5B8=_tmp5B5,_tmp5BA(& env,& _tmp5B9,& _tmp5B8,& _tmp5B6,&
_tmp1A8,& _tmp1A4))),_tmp5B7)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1A4,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1A4.curr)[i]){
change=1;break;}}}if(!change){if(!_tmp1A3.is_union)return oldval;new_d=_tmp1A4;}
else{change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp1A4,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1A8,sizeof(void*),i))){change=1;break;}}}if(!
change){if(!_tmp1A3.is_union)return newval;new_d=_tmp1A8;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp5BD;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5BC;return(void*)((_tmp5BC=
_region_malloc((env->fenv)->r,sizeof(*_tmp5BC)),((_tmp5BC[0]=((_tmp5BD.tag=6,((
_tmp5BD.f1=_tmp1A7,((_tmp5BD.f2=new_d,_tmp5BD)))))),_tmp5BC))));};}_LLA1: _tmp1A9=
_tmp199.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp1AA=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp1A9;if(_tmp1AA->tag != 4)goto _LLA3;else{_tmp1AB=_tmp1AA->f1;}};_LLA2: switch(
_tmp1AB){case Cyc_CfFlowInfo_NoneIL: _LLA5: return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL:
_LLA6: return(env->fenv)->unknown_this;case Cyc_CfFlowInfo_AllIL: _LLA7: return(env->fenv)->unknown_all;}
_LLA3:;_LLA4: return newval;_LL9A:;}static int Cyc_CfFlowInfo_nprefix(int*n,void*
unused){if(*n > 0){*n=*n - 1;return 1;}else{return 0;}}static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,int fs_prefix,void*
oldval,void*newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,
newval);{struct _tuple18 _tmp5BE;struct _tuple18 _tmp1B5=(_tmp5BE.f1=fs,((_tmp5BE.f2=
oldval,_tmp5BE)));struct Cyc_List_List*_tmp1B6;struct Cyc_List_List _tmp1B7;int
_tmp1B8;struct Cyc_List_List*_tmp1B9;void*_tmp1BA;struct Cyc_CfFlowInfo_UnionRInfo
_tmp1BC;int _tmp1BD;int _tmp1BE;struct _dyneither_ptr _tmp1BF;_LLAA: _tmp1B6=_tmp1B5.f1;
if(_tmp1B6 == 0)goto _LLAC;_tmp1B7=*_tmp1B6;_tmp1B8=(int)_tmp1B7.hd;_tmp1B9=
_tmp1B7.tl;_tmp1BA=_tmp1B5.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1BB=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1BA;if(_tmp1BB->tag != 6)goto _LLAC;
else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BC.is_union;_tmp1BE=_tmp1BC.fieldnum;
_tmp1BF=_tmp1BB->f2;}};_LLAB: {void*_tmp1C0=Cyc_CfFlowInfo_assign_place_outer(
env,_tmp1B9,fs_prefix + 1,*((void**)_check_dyneither_subscript(_tmp1BF,sizeof(
void*),_tmp1B8)),newval);struct _dyneither_ptr _tmp1C1=Cyc_CfFlowInfo_aggr_dict_insert((
env->fenv)->r,_tmp1BF,_tmp1B8,_tmp1C0);if((void**)_tmp1C1.curr == (void**)_tmp1BF.curr
 && (!_tmp1BD  || _tmp1BE == _tmp1B8))return oldval;_tmp1BF=_tmp1C1;if(_tmp1BD){int
changed=0;int sz=(int)_get_dyneither_size(_tmp1BF,sizeof(void*));{int i=0;for(0;i < 
sz;++ i){if(i != _tmp1B8){struct _dyneither_ptr _tmp1C2=Cyc_CfFlowInfo_aggr_dict_insert((
env->fenv)->r,_tmp1BF,i,Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((
void**)_check_dyneither_subscript(_tmp1BF,sizeof(void*),i))));if((void**)_tmp1C2.curr
!= (void**)_tmp1BF.curr){_tmp1BF=_tmp1C2;changed=1;}}}}if(changed){struct Cyc_CfFlowInfo_Place
_tmp1C4;void*_tmp1C5;struct Cyc_List_List*_tmp1C6;struct Cyc_CfFlowInfo_Place*
_tmp1C3=env->root_place;_tmp1C4=*_tmp1C3;_tmp1C5=_tmp1C4.root;_tmp1C6=_tmp1C4.fields;{
int*_tmp5BF;struct Cyc_List_List*_tmp1C7=((struct Cyc_List_List*(*)(struct
_RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)((
env->pile)->rgn,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,((_tmp5BF=
_cycalloc_atomic(sizeof(*_tmp5BF)),((_tmp5BF[0]=fs_prefix,_tmp5BF)))),_tmp1C6);
struct Cyc_CfFlowInfo_Place*_tmp5C0;struct Cyc_CfFlowInfo_Place*_tmp1C8=(_tmp5C0=
_region_malloc((env->pile)->rgn,sizeof(*_tmp5C0)),((_tmp5C0->root=_tmp1C5,((
_tmp5C0->fields=_tmp1C7,_tmp5C0)))));((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,_tmp1C8,
0);};}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5C6;struct Cyc_CfFlowInfo_UnionRInfo
_tmp5C5;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5C4;return(void*)((_tmp5C4=
_region_malloc((env->fenv)->r,sizeof(*_tmp5C4)),((_tmp5C4[0]=((_tmp5C6.tag=6,((
_tmp5C6.f1=((_tmp5C5.is_union=_tmp1BD,((_tmp5C5.fieldnum=_tmp1B8,_tmp5C5)))),((
_tmp5C6.f2=_tmp1BF,_tmp5C6)))))),_tmp5C4))));};}_LLAC:;_LLAD: {struct Cyc_Core_Impossible_struct
_tmp5CC;const char*_tmp5CB;struct Cyc_Core_Impossible_struct*_tmp5CA;(int)_throw((
void*)((_tmp5CA=_cycalloc(sizeof(*_tmp5CA)),((_tmp5CA[0]=((_tmp5CC.tag=Cyc_Core_Impossible,((
_tmp5CC.f1=((_tmp5CB="bad assign place",_tag_dyneither(_tmp5CB,sizeof(char),17))),
_tmp5CC)))),_tmp5CA)))));}_LLA9:;};}struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,place,0);{struct _RegionHandle*_tmp1D1=fenv->r;struct Cyc_CfFlowInfo_Place
_tmp1D3;void*_tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_CfFlowInfo_Place*
_tmp1D2=place;_tmp1D3=*_tmp1D2;_tmp1D4=_tmp1D3.root;_tmp1D5=_tmp1D3.fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp5CF;struct Cyc_CfFlowInfo_AssignEnv _tmp5CE;
struct Cyc_CfFlowInfo_AssignEnv env=(_tmp5CE.fenv=fenv,((_tmp5CE.pile=((_tmp5CF=
_region_malloc(_tmp1D1,sizeof(*_tmp5CF)),((_tmp5CF->rgn=_tmp1D1,((_tmp5CF->places=
0,_tmp5CF)))))),((_tmp5CE.d=d,((_tmp5CE.loc=loc,((_tmp5CE.root_place=place,((
_tmp5CE.all_changed=all_changed,_tmp5CE)))))))))));void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp1D5,0,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1D4),
r);return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1D4,newval));};};}
struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};typedef struct Cyc_CfFlowInfo_JoinEnv*
Cyc_CfFlowInfo_joinenv_t;enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither
 = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{
struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict
chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged changed;};typedef struct Cyc_CfFlowInfo_AfterEnv*
Cyc_CfFlowInfo_afterenv_t;static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*
ignore,void*r1,void*r2);struct _tuple20{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel
f2;};static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,
void*a,void*r1,void*r2);static void _tmp5DF(struct Cyc_CfFlowInfo_JoinEnv**env,
unsigned int*_tmp5DE,unsigned int*_tmp5DD,void***_tmp5DB,struct _dyneither_ptr*
_tmp222,struct _dyneither_ptr*_tmp21C){for(*_tmp5DE=0;*_tmp5DE < *_tmp5DD;(*
_tmp5DE)++){(*_tmp5DB)[*_tmp5DE]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int
a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp21C).curr)[(
int)*_tmp5DE],*((void**)_check_dyneither_subscript(*_tmp222,sizeof(void*),(int)*
_tmp5DE)));}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp1D8=(
env->fenv)->r;{struct _tuple0 _tmp5D0;struct _tuple0 _tmp1DA=(_tmp5D0.f1=r1,((
_tmp5D0.f2=r2,_tmp5D0)));void*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DD;int _tmp1DE;
void*_tmp1DF;void*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E2;int _tmp1E3;void*_tmp1E4;
void*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E7;int _tmp1E8;void*_tmp1E9;void*_tmp1EA;
struct Cyc_Absyn_Exp*_tmp1EC;int _tmp1ED;void*_tmp1EE;void*_tmp1EF;struct Cyc_CfFlowInfo_Place*
_tmp1F1;void*_tmp1F2;struct Cyc_CfFlowInfo_Place*_tmp1F4;void*_tmp1F5;struct Cyc_CfFlowInfo_Place*
_tmp1F7;void*_tmp1F8;void*_tmp1FA;void*_tmp1FC;struct Cyc_CfFlowInfo_Place*
_tmp1FE;void*_tmp1FF;struct Cyc_CfFlowInfo_Place*_tmp201;void*_tmp202;void*
_tmp204;void*_tmp206;struct Cyc_CfFlowInfo_Place*_tmp208;void*_tmp209;struct Cyc_CfFlowInfo_Place*
_tmp20B;void*_tmp20C;struct Cyc_CfFlowInfo_Place*_tmp20E;void*_tmp20F;void*
_tmp211;void*_tmp213;void*_tmp215;void*_tmp217;struct Cyc_CfFlowInfo_UnionRInfo
_tmp219;int _tmp21A;int _tmp21B;struct _dyneither_ptr _tmp21C;void*_tmp21D;struct Cyc_CfFlowInfo_UnionRInfo
_tmp21F;int _tmp220;int _tmp221;struct _dyneither_ptr _tmp222;_LLAF: _tmp1DB=_tmp1DA.f1;{
struct Cyc_CfFlowInfo_Consumed_struct*_tmp1DC=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1DB;if(_tmp1DC->tag != 7)goto _LLB1;else{_tmp1DD=_tmp1DC->f1;_tmp1DE=_tmp1DC->f2;
_tmp1DF=(void*)_tmp1DC->f3;}};_tmp1E0=_tmp1DA.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1E1=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1E0;if(_tmp1E1->tag != 7)goto
_LLB1;else{_tmp1E2=_tmp1E1->f1;_tmp1E3=_tmp1E1->f2;_tmp1E4=(void*)_tmp1E1->f3;}};
_LLB0: if(_tmp1DD == _tmp1E2  && _tmp1DE == _tmp1E3){struct Cyc_CfFlowInfo_Consumed_struct
_tmp5D3;struct Cyc_CfFlowInfo_Consumed_struct*_tmp5D2;return(void*)((_tmp5D2=
_region_malloc(_tmp1D8,sizeof(*_tmp5D2)),((_tmp5D2[0]=((_tmp5D3.tag=7,((_tmp5D3.f1=
_tmp1DD,((_tmp5D3.f2=_tmp1DE,((_tmp5D3.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1DF,_tmp1E4),_tmp5D3)))))))),_tmp5D2))));}{void*_tmp225=_tmp1DF;struct Cyc_CfFlowInfo_Place*
_tmp227;_LLCC: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp226=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp225;if(_tmp226->tag != 5)goto _LLCE;else{_tmp227=_tmp226->f1;}}_LLCD: Cyc_CfFlowInfo_add_place(
env->pile,_tmp227);goto _LLCB;_LLCE:;_LLCF: goto _LLCB;_LLCB:;}{void*_tmp228=
_tmp1E4;struct Cyc_CfFlowInfo_Place*_tmp22A;_LLD1: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp229=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp228;if(_tmp229->tag != 5)goto
_LLD3;else{_tmp22A=_tmp229->f1;}}_LLD2: Cyc_CfFlowInfo_add_place(env->pile,
_tmp22A);goto _LLD0;_LLD3:;_LLD4: goto _LLD0;_LLD0:;}goto _LLAE;_LLB1: _tmp1E5=
_tmp1DA.f1;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp1E6=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1E5;if(_tmp1E6->tag != 7)goto _LLB3;else{_tmp1E7=_tmp1E6->f1;_tmp1E8=_tmp1E6->f2;
_tmp1E9=(void*)_tmp1E6->f3;}};_LLB2: {struct Cyc_CfFlowInfo_Consumed_struct
_tmp5D6;struct Cyc_CfFlowInfo_Consumed_struct*_tmp5D5;return(void*)((_tmp5D5=
_region_malloc(_tmp1D8,sizeof(*_tmp5D5)),((_tmp5D5[0]=((_tmp5D6.tag=7,((_tmp5D6.f1=
_tmp1E7,((_tmp5D6.f2=_tmp1E8,((_tmp5D6.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1E9,r2),_tmp5D6)))))))),_tmp5D5))));}_LLB3: _tmp1EA=_tmp1DA.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1EB=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1EA;if(_tmp1EB->tag != 7)goto
_LLB5;else{_tmp1EC=_tmp1EB->f1;_tmp1ED=_tmp1EB->f2;_tmp1EE=(void*)_tmp1EB->f3;}};
_LLB4: {struct Cyc_CfFlowInfo_Consumed_struct _tmp5D9;struct Cyc_CfFlowInfo_Consumed_struct*
_tmp5D8;return(void*)((_tmp5D8=_region_malloc(_tmp1D8,sizeof(*_tmp5D8)),((
_tmp5D8[0]=((_tmp5D9.tag=7,((_tmp5D9.f1=_tmp1EC,((_tmp5D9.f2=_tmp1ED,((_tmp5D9.f3=(
void*)Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1EE),_tmp5D9)))))))),_tmp5D8))));}
_LLB5: _tmp1EF=_tmp1DA.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1F0=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1EF;if(_tmp1F0->tag != 5)goto _LLB7;else{
_tmp1F1=_tmp1F0->f1;}};_tmp1F2=_tmp1DA.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1F3=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F2;if(_tmp1F3->tag != 5)goto
_LLB7;else{_tmp1F4=_tmp1F3->f1;}};_LLB6: if(Cyc_CfFlowInfo_place_cmp(_tmp1F1,
_tmp1F4)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp1F1);Cyc_CfFlowInfo_add_place(
env->pile,_tmp1F4);goto _LLAE;_LLB7: _tmp1F5=_tmp1DA.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1F6=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F5;if(_tmp1F6->tag != 5)goto
_LLB9;else{_tmp1F7=_tmp1F6->f1;}};_tmp1F8=_tmp1DA.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1F9=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1F8;if(_tmp1F9->tag != 2)
goto _LLB9;};_LLB8: _tmp1FE=_tmp1F7;goto _LLBA;_LLB9: _tmp1FA=_tmp1DA.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1FB=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1FA;if(_tmp1FB->tag != 2)
goto _LLBB;};_tmp1FC=_tmp1DA.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1FD=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1FC;if(_tmp1FD->tag != 5)goto _LLBB;
else{_tmp1FE=_tmp1FD->f1;}};_LLBA: Cyc_CfFlowInfo_add_place(env->pile,_tmp1FE);
return(env->fenv)->notzerothis;_LLBB: _tmp1FF=_tmp1DA.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp200=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1FF;if(_tmp200->tag != 5)goto
_LLBD;else{_tmp201=_tmp200->f1;}};_tmp202=_tmp1DA.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp203=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp202;if(_tmp203->tag != 1)
goto _LLBD;};_LLBC: Cyc_CfFlowInfo_add_place(env->pile,_tmp201);switch(Cyc_CfFlowInfo_initlevel(
env->fenv,env->d1,r1)){case Cyc_CfFlowInfo_AllIL: _LLD5: return(env->fenv)->notzeroall;
default: _LLD6: return(env->fenv)->notzerothis;}_LLBD: _tmp204=_tmp1DA.f1;{struct
Cyc_CfFlowInfo_NotZeroAll_struct*_tmp205=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp204;if(_tmp205->tag != 1)goto _LLBF;};_tmp206=_tmp1DA.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp207=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp206;if(_tmp207->tag != 5)goto
_LLBF;else{_tmp208=_tmp207->f1;}};_LLBE: Cyc_CfFlowInfo_add_place(env->pile,
_tmp208);switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2)){case Cyc_CfFlowInfo_AllIL:
_LLD8: return(env->fenv)->notzeroall;default: _LLD9: return(env->fenv)->notzerothis;}
_LLBF: _tmp209=_tmp1DA.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp20A=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp209;if(_tmp20A->tag != 5)goto _LLC1;else{
_tmp20B=_tmp20A->f1;}};_LLC0: Cyc_CfFlowInfo_add_place(env->pile,_tmp20B);goto
_LLAE;_LLC1: _tmp20C=_tmp1DA.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp20D=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp20C;if(_tmp20D->tag != 5)goto _LLC3;
else{_tmp20E=_tmp20D->f1;}};_LLC2: Cyc_CfFlowInfo_add_place(env->pile,_tmp20E);
goto _LLAE;_LLC3: _tmp20F=_tmp1DA.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp210=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp20F;if(_tmp210->tag != 1)
goto _LLC5;};_tmp211=_tmp1DA.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp212=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp211;if(_tmp212->tag != 2)goto _LLC5;};
_LLC4: goto _LLC6;_LLC5: _tmp213=_tmp1DA.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp214=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp213;if(_tmp214->tag != 2)
goto _LLC7;};_tmp215=_tmp1DA.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp216=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp215;if(_tmp216->tag != 1)goto _LLC7;};
_LLC6: return(env->fenv)->notzerothis;_LLC7: _tmp217=_tmp1DA.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp218=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp217;if(_tmp218->tag != 6)goto
_LLC9;else{_tmp219=_tmp218->f1;_tmp21A=_tmp219.is_union;_tmp21B=_tmp219.fieldnum;
_tmp21C=_tmp218->f2;}};_tmp21D=_tmp1DA.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp21E=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp21D;if(_tmp21E->tag != 6)goto
_LLC9;else{_tmp21F=_tmp21E->f1;_tmp220=_tmp21F.is_union;_tmp221=_tmp21F.fieldnum;
_tmp222=_tmp21E->f2;}};_LLC8: {unsigned int _tmp5DE;unsigned int _tmp5DD;struct
_dyneither_ptr _tmp5DC;void**_tmp5DB;unsigned int _tmp5DA;struct _dyneither_ptr
new_d=(_tmp5DA=_get_dyneither_size(_tmp21C,sizeof(void*)),((_tmp5DB=(void**)
_region_malloc(_tmp1D8,_check_times(sizeof(void*),_tmp5DA)),((_tmp5DC=
_tag_dyneither(_tmp5DB,sizeof(void*),_tmp5DA),((((_tmp5DD=_tmp5DA,_tmp5DF(& env,&
_tmp5DE,& _tmp5DD,& _tmp5DB,& _tmp222,& _tmp21C))),_tmp5DC)))))));int change=0;{int i=
0;for(0;i < _get_dyneither_size(_tmp21C,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp21C.curr)[i]){
change=1;break;}}}if(!change){if(!_tmp21A)return r1;new_d=_tmp21C;}else{change=0;{
int i=0;for(0;i < _get_dyneither_size(_tmp21C,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp222,sizeof(void*),i))){change=1;break;}}}if(!
change){if(!_tmp21A)return r2;new_d=_tmp222;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp5E5;struct Cyc_CfFlowInfo_UnionRInfo _tmp5E4;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5E3;return(void*)((_tmp5E3=_region_malloc(_tmp1D8,sizeof(*_tmp5E3)),((
_tmp5E3[0]=((_tmp5E5.tag=6,((_tmp5E5.f1=((_tmp5E4.is_union=_tmp21A,((_tmp5E4.fieldnum=
_tmp21B == _tmp221?_tmp21B: - 1,_tmp5E4)))),((_tmp5E5.f2=new_d,_tmp5E5)))))),
_tmp5E3))));};}_LLC9:;_LLCA: goto _LLAE;_LLAE:;}{enum Cyc_CfFlowInfo_InitLevel il1=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel il2=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);struct _tuple0 _tmp5E6;struct
_tuple0 _tmp237=(_tmp5E6.f1=r1,((_tmp5E6.f2=r2,_tmp5E6)));void*_tmp238;void*
_tmp23A;_LLDC: _tmp238=_tmp237.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp239=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp238;if(_tmp239->tag != 4)goto _LLDE;};_LLDD:
goto _LLDF;_LLDE: _tmp23A=_tmp237.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp23B=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp23A;if(_tmp23B->tag != 4)goto _LLE0;};_LLDF: {
struct _tuple20 _tmp5E7;struct _tuple20 _tmp23D=(_tmp5E7.f1=il1,((_tmp5E7.f2=il2,
_tmp5E7)));enum Cyc_CfFlowInfo_InitLevel _tmp23E;enum Cyc_CfFlowInfo_InitLevel
_tmp23F;enum Cyc_CfFlowInfo_InitLevel _tmp240;enum Cyc_CfFlowInfo_InitLevel
_tmp241;_LLE3: _tmp23E=_tmp23D.f2;if(_tmp23E != Cyc_CfFlowInfo_NoneIL)goto _LLE5;
_LLE4: goto _LLE6;_LLE5: _tmp23F=_tmp23D.f1;if(_tmp23F != Cyc_CfFlowInfo_NoneIL)goto
_LLE7;_LLE6: return(env->fenv)->esc_none;_LLE7: _tmp240=_tmp23D.f2;if(_tmp240 != 
Cyc_CfFlowInfo_ThisIL)goto _LLE9;_LLE8: goto _LLEA;_LLE9: _tmp241=_tmp23D.f1;if(
_tmp241 != Cyc_CfFlowInfo_ThisIL)goto _LLEB;_LLEA: return(env->fenv)->esc_this;
_LLEB:;_LLEC: return(env->fenv)->esc_all;_LLE2:;}_LLE0:;_LLE1: {struct _tuple20
_tmp5E8;struct _tuple20 _tmp243=(_tmp5E8.f1=il1,((_tmp5E8.f2=il2,_tmp5E8)));enum 
Cyc_CfFlowInfo_InitLevel _tmp244;enum Cyc_CfFlowInfo_InitLevel _tmp245;enum Cyc_CfFlowInfo_InitLevel
_tmp246;enum Cyc_CfFlowInfo_InitLevel _tmp247;_LLEE: _tmp244=_tmp243.f2;if(_tmp244
!= Cyc_CfFlowInfo_NoneIL)goto _LLF0;_LLEF: goto _LLF1;_LLF0: _tmp245=_tmp243.f1;if(
_tmp245 != Cyc_CfFlowInfo_NoneIL)goto _LLF2;_LLF1: return(env->fenv)->unknown_none;
_LLF2: _tmp246=_tmp243.f2;if(_tmp246 != Cyc_CfFlowInfo_ThisIL)goto _LLF4;_LLF3: goto
_LLF5;_LLF4: _tmp247=_tmp243.f1;if(_tmp247 != Cyc_CfFlowInfo_ThisIL)goto _LLF6;
_LLF5: return(env->fenv)->unknown_this;_LLF6:;_LLF7: return(env->fenv)->unknown_all;
_LLED:;}_LLDB:;};};}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5E9;return((_tmp5E9.EqualConst).val=
i,(((_tmp5E9.EqualConst).tag=1,_tmp5E9)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(
struct Cyc_Absyn_Vardecl*v,void*t){struct _tuple11 _tmp5EC;union Cyc_CfFlowInfo_RelnOp
_tmp5EB;return((_tmp5EB.LessVar).val=((_tmp5EC.f1=v,((_tmp5EC.f2=t,_tmp5EC)))),(((
_tmp5EB.LessVar).tag=2,_tmp5EB)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(
struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp5ED;return((_tmp5ED.LessNumelts).val=
v,(((_tmp5ED.LessNumelts).tag=3,_tmp5ED)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5EE;return((_tmp5EE.LessConst).val=
i,(((_tmp5EE.LessConst).tag=4,_tmp5EE)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5EF;return((_tmp5EF.LessEqConst).val=
i,(((_tmp5EF.LessEqConst).tag=6,_tmp5EF)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(
struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp5F0;return((_tmp5F0.LessEqNumelts).val=
v,(((_tmp5F0.LessEqNumelts).tag=5,_tmp5F0)));}struct _tuple21{union Cyc_CfFlowInfo_RelnOp
f1;union Cyc_CfFlowInfo_RelnOp f2;};static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2){struct _tuple21 _tmp5F1;struct _tuple21 _tmp250=(
_tmp5F1.f1=r1,((_tmp5F1.f2=r2,_tmp5F1)));union Cyc_CfFlowInfo_RelnOp _tmp251;
unsigned int _tmp252;union Cyc_CfFlowInfo_RelnOp _tmp253;unsigned int _tmp254;union
Cyc_CfFlowInfo_RelnOp _tmp255;struct _tuple11 _tmp256;struct Cyc_Absyn_Vardecl*
_tmp257;union Cyc_CfFlowInfo_RelnOp _tmp258;struct _tuple11 _tmp259;struct Cyc_Absyn_Vardecl*
_tmp25A;union Cyc_CfFlowInfo_RelnOp _tmp25B;struct Cyc_Absyn_Vardecl*_tmp25C;union
Cyc_CfFlowInfo_RelnOp _tmp25D;struct Cyc_Absyn_Vardecl*_tmp25E;union Cyc_CfFlowInfo_RelnOp
_tmp25F;unsigned int _tmp260;union Cyc_CfFlowInfo_RelnOp _tmp261;unsigned int
_tmp262;union Cyc_CfFlowInfo_RelnOp _tmp263;unsigned int _tmp264;union Cyc_CfFlowInfo_RelnOp
_tmp265;unsigned int _tmp266;union Cyc_CfFlowInfo_RelnOp _tmp267;struct Cyc_Absyn_Vardecl*
_tmp268;union Cyc_CfFlowInfo_RelnOp _tmp269;struct Cyc_Absyn_Vardecl*_tmp26A;_LLF9:
_tmp251=_tmp250.f1;if((_tmp251.EqualConst).tag != 1)goto _LLFB;_tmp252=(
unsigned int)(_tmp251.EqualConst).val;_tmp253=_tmp250.f2;if((_tmp253.EqualConst).tag
!= 1)goto _LLFB;_tmp254=(unsigned int)(_tmp253.EqualConst).val;_LLFA: return
_tmp252 == _tmp254;_LLFB: _tmp255=_tmp250.f1;if((_tmp255.LessVar).tag != 2)goto
_LLFD;_tmp256=(struct _tuple11)(_tmp255.LessVar).val;_tmp257=_tmp256.f1;_tmp258=
_tmp250.f2;if((_tmp258.LessVar).tag != 2)goto _LLFD;_tmp259=(struct _tuple11)(
_tmp258.LessVar).val;_tmp25A=_tmp259.f1;_LLFC: return _tmp257 == _tmp25A;_LLFD:
_tmp25B=_tmp250.f1;if((_tmp25B.LessNumelts).tag != 3)goto _LLFF;_tmp25C=(struct Cyc_Absyn_Vardecl*)(
_tmp25B.LessNumelts).val;_tmp25D=_tmp250.f2;if((_tmp25D.LessNumelts).tag != 3)
goto _LLFF;_tmp25E=(struct Cyc_Absyn_Vardecl*)(_tmp25D.LessNumelts).val;_LLFE:
return _tmp25C == _tmp25E;_LLFF: _tmp25F=_tmp250.f1;if((_tmp25F.LessConst).tag != 4)
goto _LL101;_tmp260=(unsigned int)(_tmp25F.LessConst).val;_tmp261=_tmp250.f2;if((
_tmp261.LessConst).tag != 4)goto _LL101;_tmp262=(unsigned int)(_tmp261.LessConst).val;
_LL100: return _tmp260 == _tmp262;_LL101: _tmp263=_tmp250.f1;if((_tmp263.LessEqConst).tag
!= 6)goto _LL103;_tmp264=(unsigned int)(_tmp263.LessEqConst).val;_tmp265=_tmp250.f2;
if((_tmp265.LessEqConst).tag != 6)goto _LL103;_tmp266=(unsigned int)(_tmp265.LessEqConst).val;
_LL102: return _tmp264 == _tmp266;_LL103: _tmp267=_tmp250.f1;if((_tmp267.LessEqNumelts).tag
!= 5)goto _LL105;_tmp268=(struct Cyc_Absyn_Vardecl*)(_tmp267.LessEqNumelts).val;
_tmp269=_tmp250.f2;if((_tmp269.LessEqNumelts).tag != 5)goto _LL105;_tmp26A=(struct
Cyc_Absyn_Vardecl*)(_tmp269.LessEqNumelts).val;_LL104: return _tmp268 == _tmp26A;
_LL105:;_LL106: return 0;_LLF8:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == 
r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*_tmp26B=
r1s;for(0;_tmp26B != 0;_tmp26B=_tmp26B->tl){struct Cyc_CfFlowInfo_Reln*_tmp26C=(
struct Cyc_CfFlowInfo_Reln*)_tmp26B->hd;int found=0;{struct Cyc_List_List*_tmp26D=
r2s;for(0;_tmp26D != 0;_tmp26D=_tmp26D->tl){struct Cyc_CfFlowInfo_Reln*_tmp26E=(
struct Cyc_CfFlowInfo_Reln*)_tmp26D->hd;if(_tmp26C == _tmp26E  || _tmp26C->vd == 
_tmp26E->vd  && Cyc_CfFlowInfo_same_relop(_tmp26C->rop,_tmp26E->rop)){{struct Cyc_List_List*
_tmp5F2;res=((_tmp5F2=_region_malloc(r,sizeof(*_tmp5F2)),((_tmp5F2->hd=_tmp26C,((
_tmp5F2->tl=res,_tmp5F2))))));}found=1;break;}}}if(!found)diff=1;}}if(!diff)res=
r1s;return res;};}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2){struct _tuple22 _tmp5F3;struct _tuple22 _tmp271=(_tmp5F3.f1=f1,((_tmp5F3.f2=f2,
_tmp5F3)));union Cyc_CfFlowInfo_FlowInfo _tmp272;int _tmp273;union Cyc_CfFlowInfo_FlowInfo
_tmp274;int _tmp275;union Cyc_CfFlowInfo_FlowInfo _tmp276;struct _tuple12 _tmp277;
struct Cyc_Dict_Dict _tmp278;struct Cyc_List_List*_tmp279;union Cyc_CfFlowInfo_FlowInfo
_tmp27A;struct _tuple12 _tmp27B;struct Cyc_Dict_Dict _tmp27C;struct Cyc_List_List*
_tmp27D;_LL108: _tmp272=_tmp271.f1;if((_tmp272.BottomFL).tag != 1)goto _LL10A;
_tmp273=(int)(_tmp272.BottomFL).val;_LL109: return f2;_LL10A: _tmp274=_tmp271.f2;
if((_tmp274.BottomFL).tag != 1)goto _LL10C;_tmp275=(int)(_tmp274.BottomFL).val;
_LL10B: return f1;_LL10C: _tmp276=_tmp271.f1;if((_tmp276.ReachableFL).tag != 2)goto
_LL107;_tmp277=(struct _tuple12)(_tmp276.ReachableFL).val;_tmp278=_tmp277.f1;
_tmp279=_tmp277.f2;_tmp27A=_tmp271.f2;if((_tmp27A.ReachableFL).tag != 2)goto
_LL107;_tmp27B=(struct _tuple12)(_tmp27A.ReachableFL).val;_tmp27C=_tmp27B.f1;
_tmp27D=_tmp27B.f2;_LL10D: if(_tmp278.t == _tmp27C.t  && _tmp279 == _tmp27D)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f2,f1))return f1;{struct _RegionHandle*_tmp27E=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp5F6;struct Cyc_CfFlowInfo_JoinEnv _tmp5F5;struct Cyc_CfFlowInfo_JoinEnv _tmp27F=(
_tmp5F5.fenv=fenv,((_tmp5F5.pile=((_tmp5F6=_region_malloc(_tmp27E,sizeof(*
_tmp5F6)),((_tmp5F6->rgn=_tmp27E,((_tmp5F6->places=0,_tmp5F6)))))),((_tmp5F5.d1=
_tmp278,((_tmp5F5.d2=_tmp27C,_tmp5F5)))))));struct Cyc_Dict_Dict _tmp280=((struct
Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),
struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))
Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,
void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp27F,_tmp278,_tmp27C);struct Cyc_List_List*
_tmp281=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp279,_tmp27D);return Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_escape_these(fenv,_tmp27F.pile,all_changed,_tmp280),_tmp281);};
_LL107:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;union Cyc_CfFlowInfo_FlowInfo f3;};struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple13
pr1,struct _tuple13 pr2){union Cyc_CfFlowInfo_FlowInfo _tmp285;void*_tmp286;struct
_tuple13 _tmp284=pr1;_tmp285=_tmp284.f1;_tmp286=_tmp284.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp288;void*_tmp289;struct _tuple13 _tmp287=pr2;_tmp288=_tmp287.f1;_tmp289=
_tmp287.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,
all_changed,_tmp285,_tmp288);struct _tuple23 _tmp5F7;struct _tuple23 _tmp28B=(
_tmp5F7.f1=_tmp285,((_tmp5F7.f2=_tmp288,((_tmp5F7.f3=outflow,_tmp5F7)))));union
Cyc_CfFlowInfo_FlowInfo _tmp28C;int _tmp28D;union Cyc_CfFlowInfo_FlowInfo _tmp28E;
int _tmp28F;union Cyc_CfFlowInfo_FlowInfo _tmp290;struct _tuple12 _tmp291;struct Cyc_Dict_Dict
_tmp292;union Cyc_CfFlowInfo_FlowInfo _tmp293;struct _tuple12 _tmp294;struct Cyc_Dict_Dict
_tmp295;union Cyc_CfFlowInfo_FlowInfo _tmp296;struct _tuple12 _tmp297;struct Cyc_Dict_Dict
_tmp298;struct Cyc_List_List*_tmp299;_LL10F: _tmp28C=_tmp28B.f1;if((_tmp28C.BottomFL).tag
!= 1)goto _LL111;_tmp28D=(int)(_tmp28C.BottomFL).val;_LL110: {struct _tuple13
_tmp5F8;return(_tmp5F8.f1=outflow,((_tmp5F8.f2=_tmp289,_tmp5F8)));}_LL111:
_tmp28E=_tmp28B.f2;if((_tmp28E.BottomFL).tag != 1)goto _LL113;_tmp28F=(int)(
_tmp28E.BottomFL).val;_LL112: {struct _tuple13 _tmp5F9;return(_tmp5F9.f1=outflow,((
_tmp5F9.f2=_tmp286,_tmp5F9)));}_LL113: _tmp290=_tmp28B.f1;if((_tmp290.ReachableFL).tag
!= 2)goto _LL115;_tmp291=(struct _tuple12)(_tmp290.ReachableFL).val;_tmp292=
_tmp291.f1;_tmp293=_tmp28B.f2;if((_tmp293.ReachableFL).tag != 2)goto _LL115;
_tmp294=(struct _tuple12)(_tmp293.ReachableFL).val;_tmp295=_tmp294.f1;_tmp296=
_tmp28B.f3;if((_tmp296.ReachableFL).tag != 2)goto _LL115;_tmp297=(struct _tuple12)(
_tmp296.ReachableFL).val;_tmp298=_tmp297.f1;_tmp299=_tmp297.f2;_LL114: if(((int(*)(
int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp286,
_tmp289)){struct _tuple13 _tmp5FA;return(_tmp5FA.f1=outflow,((_tmp5FA.f2=_tmp289,
_tmp5FA)));}if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp289,_tmp286)){struct _tuple13 _tmp5FB;return(_tmp5FB.f1=outflow,((_tmp5FB.f2=
_tmp286,_tmp5FB)));}{struct _RegionHandle*_tmp29E=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp5FE;struct Cyc_CfFlowInfo_JoinEnv _tmp5FD;struct Cyc_CfFlowInfo_JoinEnv _tmp29F=(
_tmp5FD.fenv=fenv,((_tmp5FD.pile=((_tmp5FE=_region_malloc(_tmp29E,sizeof(*
_tmp5FE)),((_tmp5FE->rgn=_tmp29E,((_tmp5FE->places=0,_tmp5FE)))))),((_tmp5FD.d1=
_tmp292,((_tmp5FD.d2=_tmp295,_tmp5FD)))))));void*_tmp2A0=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp29F,0,_tmp286,
_tmp289);struct _tuple13 _tmp5FF;return(_tmp5FF.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp29F.pile,all_changed,_tmp298),_tmp299),((_tmp5FF.f2=_tmp2A0,_tmp5FF)));};
_LL115:;_LL116: {struct Cyc_Core_Impossible_struct _tmp605;const char*_tmp604;
struct Cyc_Core_Impossible_struct*_tmp603;(int)_throw((void*)((_tmp603=_cycalloc(
sizeof(*_tmp603)),((_tmp603[0]=((_tmp605.tag=Cyc_Core_Impossible,((_tmp605.f1=((
_tmp604="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp604,sizeof(
char),37))),_tmp605)))),_tmp603)))));}_LL10E:;};};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp60C(struct Cyc_CfFlowInfo_AfterEnv**
env,unsigned int*_tmp60B,unsigned int*_tmp60A,void***_tmp608,struct
_dyneither_ptr*_tmp2B8,struct _dyneither_ptr*_tmp2B2){for(*_tmp60B=0;*_tmp60B < *
_tmp60A;(*_tmp60B)++){(*_tmp608)[*_tmp60B]=Cyc_CfFlowInfo_after_absRval_d(*env,(
int)*_tmp60B,((void**)(*_tmp2B2).curr)[(int)*_tmp60B],*((void**)
_check_dyneither_subscript(*_tmp2B8,sizeof(void*),(int)*_tmp60B)));}}static void*
Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);int
changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);if(
changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*
r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);if(changed1){if(
!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))return r1;env->changed=
Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,
env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct _tuple0 _tmp606;
struct _tuple0 _tmp2A8=(_tmp606.f1=r1,((_tmp606.f2=r2,_tmp606)));void*_tmp2A9;void*
_tmp2AB;void*_tmp2AD;struct Cyc_CfFlowInfo_UnionRInfo _tmp2AF;int _tmp2B0;int
_tmp2B1;struct _dyneither_ptr _tmp2B2;void*_tmp2B3;struct Cyc_CfFlowInfo_UnionRInfo
_tmp2B5;int _tmp2B6;int _tmp2B7;struct _dyneither_ptr _tmp2B8;_LL118: _tmp2A9=_tmp2A8.f1;{
struct Cyc_CfFlowInfo_Consumed_struct*_tmp2AA=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp2A9;if(_tmp2AA->tag != 7)goto _LL11A;};_LL119: goto _LL11B;_LL11A: _tmp2AB=
_tmp2A8.f2;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp2AC=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp2AB;if(_tmp2AC->tag != 7)goto _LL11C;};_LL11B: return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
_LL11C: _tmp2AD=_tmp2A8.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2AE=(struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmp2AD;if(_tmp2AE->tag != 6)goto _LL11E;else{
_tmp2AF=_tmp2AE->f1;_tmp2B0=_tmp2AF.is_union;_tmp2B1=_tmp2AF.fieldnum;_tmp2B2=
_tmp2AE->f2;}};_tmp2B3=_tmp2A8.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2B4=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2B3;if(_tmp2B4->tag != 6)goto _LL11E;
else{_tmp2B5=_tmp2B4->f1;_tmp2B6=_tmp2B5.is_union;_tmp2B7=_tmp2B5.fieldnum;
_tmp2B8=_tmp2B4->f2;}};_LL11D: {unsigned int _tmp60B;unsigned int _tmp60A;struct
_dyneither_ptr _tmp609;void**_tmp608;unsigned int _tmp607;struct _dyneither_ptr
new_d=(_tmp607=_get_dyneither_size(_tmp2B2,sizeof(void*)),((_tmp608=(void**)
_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp607)),((
_tmp609=_tag_dyneither(_tmp608,sizeof(void*),_tmp607),((((_tmp60A=_tmp607,
_tmp60C(& env,& _tmp60B,& _tmp60A,& _tmp608,& _tmp2B8,& _tmp2B2))),_tmp609)))))));int
change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp2B2,sizeof(void*));++ i){if(*((
void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp2B2.curr)[
i]){change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp2B2,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp2B8,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp612;struct Cyc_CfFlowInfo_UnionRInfo
_tmp611;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp610;return(void*)((_tmp610=
_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp610)),((_tmp610[0]=((_tmp612.tag=
6,((_tmp612.f1=((_tmp611.is_union=_tmp2B0,((_tmp611.fieldnum=_tmp2B1 == _tmp2B7?
_tmp2B1: - 1,_tmp611)))),((_tmp612.f2=new_d,_tmp612)))))),_tmp610))));};}_LL11E:;
_LL11F: {struct Cyc_Core_Impossible_struct _tmp618;const char*_tmp617;struct Cyc_Core_Impossible_struct*
_tmp616;(int)_throw((void*)((_tmp616=_cycalloc(sizeof(*_tmp616)),((_tmp616[0]=((
_tmp618.tag=Cyc_Core_Impossible,((_tmp618.f1=((_tmp617="after_absRval -- non-aggregates!",
_tag_dyneither(_tmp617,sizeof(char),33))),_tmp618)))),_tmp616)))));}_LL117:;};}
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int
key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**_tmp2C3=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp2C4=env->changed;{struct Cyc_List_List*
_tmp619;*env->last_field_cell=((_tmp619=_region_malloc(((env->joinenv).fenv)->r,
sizeof(*_tmp619)),((_tmp619->hd=(void*)key,((_tmp619->tl=0,_tmp619))))));}env->last_field_cell=&((
struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp2C6=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp2C3;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp2C4;return _tmp2C6;};};}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_CfFlowInfo_Place _tmp61A;*env->curr_place=((_tmp61A.root=root,((_tmp61A.fields=
0,_tmp61A))));}env->last_field_cell=&(env->curr_place)->fields;env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo
f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict
chg2){struct _tuple22 _tmp61B;struct _tuple22 _tmp2C9=(_tmp61B.f1=f1,((_tmp61B.f2=f2,
_tmp61B)));union Cyc_CfFlowInfo_FlowInfo _tmp2CA;int _tmp2CB;union Cyc_CfFlowInfo_FlowInfo
_tmp2CC;int _tmp2CD;union Cyc_CfFlowInfo_FlowInfo _tmp2CE;struct _tuple12 _tmp2CF;
struct Cyc_Dict_Dict _tmp2D0;struct Cyc_List_List*_tmp2D1;union Cyc_CfFlowInfo_FlowInfo
_tmp2D2;struct _tuple12 _tmp2D3;struct Cyc_Dict_Dict _tmp2D4;struct Cyc_List_List*
_tmp2D5;_LL121: _tmp2CA=_tmp2C9.f1;if((_tmp2CA.BottomFL).tag != 1)goto _LL123;
_tmp2CB=(int)(_tmp2CA.BottomFL).val;_LL122: goto _LL124;_LL123: _tmp2CC=_tmp2C9.f2;
if((_tmp2CC.BottomFL).tag != 1)goto _LL125;_tmp2CD=(int)(_tmp2CC.BottomFL).val;
_LL124: return Cyc_CfFlowInfo_BottomFL();_LL125: _tmp2CE=_tmp2C9.f1;if((_tmp2CE.ReachableFL).tag
!= 2)goto _LL120;_tmp2CF=(struct _tuple12)(_tmp2CE.ReachableFL).val;_tmp2D0=
_tmp2CF.f1;_tmp2D1=_tmp2CF.f2;_tmp2D2=_tmp2C9.f2;if((_tmp2D2.ReachableFL).tag != 
2)goto _LL120;_tmp2D3=(struct _tuple12)(_tmp2D2.ReachableFL).val;_tmp2D4=_tmp2D3.f1;
_tmp2D5=_tmp2D3.f2;_LL126: if(_tmp2D0.t == _tmp2D4.t  && _tmp2D1 == _tmp2D5)return f1;{
struct _RegionHandle*_tmp2D6=fenv->r;struct Cyc_CfFlowInfo_Place*_tmp2D7=fenv->dummy_place;
struct Cyc_CfFlowInfo_JoinEnv _tmp621;struct Cyc_CfFlowInfo_EscPile*_tmp620;struct
Cyc_CfFlowInfo_AfterEnv _tmp61F;struct Cyc_CfFlowInfo_AfterEnv _tmp2D8=(_tmp61F.joinenv=((
_tmp621.fenv=fenv,((_tmp621.pile=((_tmp620=_region_malloc(_tmp2D6,sizeof(*
_tmp620)),((_tmp620->rgn=_tmp2D6,((_tmp620->places=0,_tmp620)))))),((_tmp621.d1=
_tmp2D0,((_tmp621.d2=_tmp2D4,_tmp621)))))))),((_tmp61F.chg1=chg1,((_tmp61F.chg2=
chg2,((_tmp61F.curr_place=_tmp2D7,((_tmp61F.last_field_cell=& _tmp2D7->fields,((
_tmp61F.changed=Cyc_CfFlowInfo_Neither,_tmp61F)))))))))));struct Cyc_Dict_Dict
_tmp2D9=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,
void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp2D8,_tmp2D0,_tmp2D4);
struct Cyc_List_List*_tmp2DA=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp2D1,_tmp2D5);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,(_tmp2D8.joinenv).pile,
all_changed,_tmp2D9),_tmp2DA);};_LL120:;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp622;struct
_tuple0 _tmp2DF=(_tmp622.f1=r1,((_tmp622.f2=r2,_tmp622)));void*_tmp2E0;struct Cyc_CfFlowInfo_Place*
_tmp2E2;void*_tmp2E3;struct Cyc_CfFlowInfo_Place*_tmp2E5;void*_tmp2E6;void*
_tmp2E8;void*_tmp2EA;struct Cyc_CfFlowInfo_UnionRInfo _tmp2EC;struct _dyneither_ptr
_tmp2ED;void*_tmp2EE;struct Cyc_CfFlowInfo_UnionRInfo _tmp2F0;struct _dyneither_ptr
_tmp2F1;void*_tmp2F2;void*_tmp2F4;void*_tmp2F6;void*_tmp2F8;void*_tmp2FA;void*
_tmp2FC;void*_tmp2FE;void*_tmp300;_LL128: _tmp2E0=_tmp2DF.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2E1=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2E0;if(_tmp2E1->tag != 5)goto
_LL12A;else{_tmp2E2=_tmp2E1->f1;}};_tmp2E3=_tmp2DF.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2E4=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2E3;if(_tmp2E4->tag != 5)goto
_LL12A;else{_tmp2E5=_tmp2E4->f1;}};_LL129: return Cyc_CfFlowInfo_place_cmp(_tmp2E2,
_tmp2E5)== 0;_LL12A: _tmp2E6=_tmp2DF.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2E7=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2E6;if(_tmp2E7->tag != 5)goto
_LL12C;};_LL12B: goto _LL12D;_LL12C: _tmp2E8=_tmp2DF.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2E9=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2E8;if(_tmp2E9->tag != 5)goto
_LL12E;};_LL12D: return 0;_LL12E: _tmp2EA=_tmp2DF.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2EB=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2EA;if(_tmp2EB->tag != 6)goto
_LL130;else{_tmp2EC=_tmp2EB->f1;_tmp2ED=_tmp2EB->f2;}};_tmp2EE=_tmp2DF.f2;{
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2EF=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2EE;if(_tmp2EF->tag != 6)goto _LL130;else{_tmp2F0=_tmp2EF->f1;_tmp2F1=_tmp2EF->f2;}};
_LL12F: if((void**)_tmp2ED.curr == (void**)_tmp2F1.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp2ED,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2ED.curr)[i],*((
void**)_check_dyneither_subscript(_tmp2F1,sizeof(void*),i))))return 0;}}return 1;
_LL130: _tmp2F2=_tmp2DF.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2F3=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp2F2;if(_tmp2F3->tag != 1)goto _LL132;};
_tmp2F4=_tmp2DF.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2F5=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2F4;if(_tmp2F5->tag != 2)goto _LL132;};_LL131: return 1;_LL132: _tmp2F6=_tmp2DF.f2;{
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2F7=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2F6;if(_tmp2F7->tag != 2)goto _LL134;};_LL133: return 0;_LL134: _tmp2F8=_tmp2DF.f2;{
struct Cyc_CfFlowInfo_Zero_struct*_tmp2F9=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp2F8;if(_tmp2F9->tag != 0)goto _LL136;};_LL135: goto _LL137;_LL136: _tmp2FA=
_tmp2DF.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2FB=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp2FA;if(_tmp2FB->tag != 1)goto _LL138;};_LL137: return 0;_LL138: _tmp2FC=_tmp2DF.f1;{
struct Cyc_CfFlowInfo_Esc_struct*_tmp2FD=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp2FC;if(_tmp2FD->tag != 4)goto _LL13A;};_tmp2FE=_tmp2DF.f2;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp2FF=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2FE;if(_tmp2FF->tag != 4)goto _LL13A;};
_LL139: goto _LL127;_LL13A: _tmp300=_tmp2DF.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp301=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp300;if(_tmp301->tag != 4)goto _LL13C;};
_LL13B: return 0;_LL13C:;_LL13D: goto _LL127;_LL127:;}{struct _tuple20 _tmp623;struct
_tuple20 _tmp303=(_tmp623.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp623.f2=Cyc_CfFlowInfo_initlevel_approx(
r2),_tmp623)));enum Cyc_CfFlowInfo_InitLevel _tmp304;enum Cyc_CfFlowInfo_InitLevel
_tmp305;enum Cyc_CfFlowInfo_InitLevel _tmp306;enum Cyc_CfFlowInfo_InitLevel
_tmp307;enum Cyc_CfFlowInfo_InitLevel _tmp308;enum Cyc_CfFlowInfo_InitLevel
_tmp309;_LL13F: _tmp304=_tmp303.f1;if(_tmp304 != Cyc_CfFlowInfo_AllIL)goto _LL141;
_tmp305=_tmp303.f2;if(_tmp305 != Cyc_CfFlowInfo_AllIL)goto _LL141;_LL140: return 1;
_LL141: _tmp306=_tmp303.f2;if(_tmp306 != Cyc_CfFlowInfo_NoneIL)goto _LL143;_LL142:
return 1;_LL143: _tmp307=_tmp303.f1;if(_tmp307 != Cyc_CfFlowInfo_NoneIL)goto _LL145;
_LL144: return 0;_LL145: _tmp308=_tmp303.f2;if(_tmp308 != Cyc_CfFlowInfo_ThisIL)goto
_LL147;_LL146: return 1;_LL147: _tmp309=_tmp303.f1;if(_tmp309 != Cyc_CfFlowInfo_ThisIL)
goto _LL13E;_LL148: return 0;_LL13E:;};}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s != 0;
r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp30A=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;
int found=0;{struct Cyc_List_List*_tmp30B=r2s;for(0;_tmp30B != 0;_tmp30B=_tmp30B->tl){
struct Cyc_CfFlowInfo_Reln*_tmp30C=(struct Cyc_CfFlowInfo_Reln*)_tmp30B->hd;if(
_tmp30A == _tmp30C  || _tmp30A->vd == _tmp30C->vd  && Cyc_CfFlowInfo_same_relop(
_tmp30A->rop,_tmp30C->rop)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){struct _tuple22
_tmp624;struct _tuple22 _tmp30E=(_tmp624.f1=f1,((_tmp624.f2=f2,_tmp624)));union Cyc_CfFlowInfo_FlowInfo
_tmp30F;int _tmp310;union Cyc_CfFlowInfo_FlowInfo _tmp311;int _tmp312;union Cyc_CfFlowInfo_FlowInfo
_tmp313;struct _tuple12 _tmp314;struct Cyc_Dict_Dict _tmp315;struct Cyc_List_List*
_tmp316;union Cyc_CfFlowInfo_FlowInfo _tmp317;struct _tuple12 _tmp318;struct Cyc_Dict_Dict
_tmp319;struct Cyc_List_List*_tmp31A;_LL14A: _tmp30F=_tmp30E.f1;if((_tmp30F.BottomFL).tag
!= 1)goto _LL14C;_tmp310=(int)(_tmp30F.BottomFL).val;_LL14B: return 1;_LL14C:
_tmp311=_tmp30E.f2;if((_tmp311.BottomFL).tag != 1)goto _LL14E;_tmp312=(int)(
_tmp311.BottomFL).val;_LL14D: return 0;_LL14E: _tmp313=_tmp30E.f1;if((_tmp313.ReachableFL).tag
!= 2)goto _LL149;_tmp314=(struct _tuple12)(_tmp313.ReachableFL).val;_tmp315=
_tmp314.f1;_tmp316=_tmp314.f2;_tmp317=_tmp30E.f2;if((_tmp317.ReachableFL).tag != 
2)goto _LL149;_tmp318=(struct _tuple12)(_tmp317.ReachableFL).val;_tmp319=_tmp318.f1;
_tmp31A=_tmp318.f2;_LL14F: if(_tmp315.t == _tmp319.t  && _tmp316 == _tmp31A)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp315,_tmp319) && Cyc_CfFlowInfo_relns_approx(_tmp316,_tmp31A);_LL149:;}struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*
rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=0;for(p=rs;!found
 && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp31B=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp31B->vd == v){found=1;break;}{union Cyc_CfFlowInfo_RelnOp _tmp31C=
_tmp31B->rop;struct _tuple11 _tmp31D;struct Cyc_Absyn_Vardecl*_tmp31E;struct Cyc_Absyn_Vardecl*
_tmp31F;struct Cyc_Absyn_Vardecl*_tmp320;_LL151: if((_tmp31C.LessVar).tag != 2)goto
_LL153;_tmp31D=(struct _tuple11)(_tmp31C.LessVar).val;_tmp31E=_tmp31D.f1;_LL152:
_tmp31F=_tmp31E;goto _LL154;_LL153: if((_tmp31C.LessNumelts).tag != 3)goto _LL155;
_tmp31F=(struct Cyc_Absyn_Vardecl*)(_tmp31C.LessNumelts).val;_LL154: _tmp320=
_tmp31F;goto _LL156;_LL155: if((_tmp31C.LessEqNumelts).tag != 5)goto _LL157;_tmp320=(
struct Cyc_Absyn_Vardecl*)(_tmp31C.LessEqNumelts).val;_LL156: if(v == _tmp320)found=
1;goto _LL150;_LL157:;_LL158: goto _LL150;_LL150:;};}if(!found)return rs;{struct Cyc_List_List*
_tmp321=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp322=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp322->vd != v){{union Cyc_CfFlowInfo_RelnOp _tmp323=_tmp322->rop;struct
_tuple11 _tmp324;struct Cyc_Absyn_Vardecl*_tmp325;struct Cyc_Absyn_Vardecl*_tmp326;
struct Cyc_Absyn_Vardecl*_tmp327;_LL15A: if((_tmp323.LessVar).tag != 2)goto _LL15C;
_tmp324=(struct _tuple11)(_tmp323.LessVar).val;_tmp325=_tmp324.f1;_LL15B: _tmp326=
_tmp325;goto _LL15D;_LL15C: if((_tmp323.LessNumelts).tag != 3)goto _LL15E;_tmp326=(
struct Cyc_Absyn_Vardecl*)(_tmp323.LessNumelts).val;_LL15D: _tmp327=_tmp326;goto
_LL15F;_LL15E: if((_tmp323.LessEqNumelts).tag != 5)goto _LL160;_tmp327=(struct Cyc_Absyn_Vardecl*)(
_tmp323.LessEqNumelts).val;_LL15F: if(v == _tmp327)continue;goto _LL159;_LL160:;
_LL161: goto _LL159;_LL159:;}{struct Cyc_List_List*_tmp625;_tmp321=((_tmp625=
_region_malloc(rgn,sizeof(*_tmp625)),((_tmp625->hd=_tmp322,((_tmp625->tl=_tmp321,
_tmp625))))));};}}return _tmp321;};}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*
_tmp329=e->r;void*_tmp32B;struct Cyc_Absyn_Vardecl*_tmp32D;void*_tmp32F;struct Cyc_Absyn_Vardecl*
_tmp331;void*_tmp333;struct Cyc_Absyn_Vardecl*_tmp335;void*_tmp337;struct Cyc_Absyn_Vardecl*
_tmp339;_LL163: {struct Cyc_Absyn_Var_e_struct*_tmp32A=(struct Cyc_Absyn_Var_e_struct*)
_tmp329;if(_tmp32A->tag != 1)goto _LL165;else{_tmp32B=(void*)_tmp32A->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp32C=(struct Cyc_Absyn_Global_b_struct*)_tmp32B;if(
_tmp32C->tag != 1)goto _LL165;else{_tmp32D=_tmp32C->f1;}};}}_LL164: _tmp331=_tmp32D;
goto _LL166;_LL165: {struct Cyc_Absyn_Var_e_struct*_tmp32E=(struct Cyc_Absyn_Var_e_struct*)
_tmp329;if(_tmp32E->tag != 1)goto _LL167;else{_tmp32F=(void*)_tmp32E->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp330=(struct Cyc_Absyn_Param_b_struct*)_tmp32F;if(
_tmp330->tag != 3)goto _LL167;else{_tmp331=_tmp330->f1;}};}}_LL166: _tmp335=_tmp331;
goto _LL168;_LL167: {struct Cyc_Absyn_Var_e_struct*_tmp332=(struct Cyc_Absyn_Var_e_struct*)
_tmp329;if(_tmp332->tag != 1)goto _LL169;else{_tmp333=(void*)_tmp332->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp334=(struct Cyc_Absyn_Local_b_struct*)_tmp333;if(
_tmp334->tag != 4)goto _LL169;else{_tmp335=_tmp334->f1;}};}}_LL168: _tmp339=_tmp335;
goto _LL16A;_LL169: {struct Cyc_Absyn_Var_e_struct*_tmp336=(struct Cyc_Absyn_Var_e_struct*)
_tmp329;if(_tmp336->tag != 1)goto _LL16B;else{_tmp337=(void*)_tmp336->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp338=(struct Cyc_Absyn_Pat_b_struct*)_tmp337;if(_tmp338->tag
!= 5)goto _LL16B;else{_tmp339=_tmp338->f1;}};}}_LL16A: if(!_tmp339->escapes)return
Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp339);goto _LL162;_LL16B:;_LL16C: goto _LL162;
_LL162:;}return r;}struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct
_RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){struct Cyc_CfFlowInfo_Reln*_tmp626;
return(_tmp626=_region_malloc(r2,sizeof(*_tmp626)),((_tmp626[0]=*r,_tmp626)));}
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*
relns){return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*
f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){for(
0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct Cyc_CfFlowInfo_Reln*_tmp33B=(
struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*_tmp33C=(struct Cyc_CfFlowInfo_Reln*)
r2->hd;if(_tmp33B->vd != _tmp33C->vd  || !Cyc_CfFlowInfo_same_relop(_tmp33B->rop,
_tmp33C->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(
rgn,r,v);{void*_tmp33D=e->r;struct Cyc_Absyn_MallocInfo _tmp33F;struct Cyc_Absyn_Exp*
_tmp340;int _tmp341;_LL16E: {struct Cyc_Absyn_Malloc_e_struct*_tmp33E=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp33D;if(_tmp33E->tag != 34)goto _LL170;else{_tmp33F=_tmp33E->f1;_tmp340=_tmp33F.num_elts;
_tmp341=_tmp33F.fat_result;if(_tmp341 != 1)goto _LL170;}}_LL16F: malloc_loop: {void*
_tmp342=_tmp340->r;struct Cyc_Absyn_Exp*_tmp344;void*_tmp346;struct Cyc_Absyn_Vardecl*
_tmp348;void*_tmp34A;struct Cyc_Absyn_Vardecl*_tmp34C;void*_tmp34E;struct Cyc_Absyn_Vardecl*
_tmp350;void*_tmp352;struct Cyc_Absyn_Vardecl*_tmp354;_LL173: {struct Cyc_Absyn_Cast_e_struct*
_tmp343=(struct Cyc_Absyn_Cast_e_struct*)_tmp342;if(_tmp343->tag != 15)goto _LL175;
else{_tmp344=_tmp343->f2;}}_LL174: _tmp340=_tmp344;goto malloc_loop;_LL175: {
struct Cyc_Absyn_Var_e_struct*_tmp345=(struct Cyc_Absyn_Var_e_struct*)_tmp342;if(
_tmp345->tag != 1)goto _LL177;else{_tmp346=(void*)_tmp345->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp347=(struct Cyc_Absyn_Pat_b_struct*)_tmp346;if(_tmp347->tag != 5)goto _LL177;
else{_tmp348=_tmp347->f1;}};}}_LL176: _tmp34C=_tmp348;goto _LL178;_LL177: {struct
Cyc_Absyn_Var_e_struct*_tmp349=(struct Cyc_Absyn_Var_e_struct*)_tmp342;if(_tmp349->tag
!= 1)goto _LL179;else{_tmp34A=(void*)_tmp349->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp34B=(struct Cyc_Absyn_Local_b_struct*)_tmp34A;if(_tmp34B->tag != 4)goto _LL179;
else{_tmp34C=_tmp34B->f1;}};}}_LL178: _tmp350=_tmp34C;goto _LL17A;_LL179: {struct
Cyc_Absyn_Var_e_struct*_tmp34D=(struct Cyc_Absyn_Var_e_struct*)_tmp342;if(_tmp34D->tag
!= 1)goto _LL17B;else{_tmp34E=(void*)_tmp34D->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp34F=(struct Cyc_Absyn_Param_b_struct*)_tmp34E;if(_tmp34F->tag != 3)goto _LL17B;
else{_tmp350=_tmp34F->f1;}};}}_LL17A: _tmp354=_tmp350;goto _LL17C;_LL17B: {struct
Cyc_Absyn_Var_e_struct*_tmp351=(struct Cyc_Absyn_Var_e_struct*)_tmp342;if(_tmp351->tag
!= 1)goto _LL17D;else{_tmp352=(void*)_tmp351->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp353=(struct Cyc_Absyn_Global_b_struct*)_tmp352;if(_tmp353->tag != 1)goto _LL17D;
else{_tmp354=_tmp353->f1;}};}}_LL17C: if(_tmp354->escapes)return r;{struct Cyc_CfFlowInfo_Reln*
_tmp629;struct Cyc_List_List*_tmp628;return(_tmp628=_region_malloc(rgn,sizeof(*
_tmp628)),((_tmp628->hd=((_tmp629=_region_malloc(rgn,sizeof(*_tmp629)),((_tmp629->vd=
_tmp354,((_tmp629->rop=Cyc_CfFlowInfo_LessEqNumelts(v),_tmp629)))))),((_tmp628->tl=
r,_tmp628)))));};_LL17D:;_LL17E: return r;_LL172:;}_LL170:;_LL171: goto _LL16D;
_LL16D:;}{void*_tmp357=Cyc_Tcutil_compress(v->type);_LL180: {struct Cyc_Absyn_IntType_struct*
_tmp358=(struct Cyc_Absyn_IntType_struct*)_tmp357;if(_tmp358->tag != 6)goto _LL182;}
_LL181: goto _LL17F;_LL182:;_LL183: return r;_LL17F:;}loop: {void*_tmp359=e->r;
struct Cyc_Absyn_Exp*_tmp35B;union Cyc_Absyn_Cnst _tmp35D;struct _tuple6 _tmp35E;int
_tmp35F;enum Cyc_Absyn_Primop _tmp361;struct Cyc_List_List*_tmp362;struct Cyc_List_List
_tmp363;struct Cyc_List_List*_tmp364;struct Cyc_List_List _tmp365;struct Cyc_Absyn_Exp*
_tmp366;enum Cyc_Absyn_Primop _tmp368;struct Cyc_List_List*_tmp369;struct Cyc_List_List
_tmp36A;struct Cyc_Absyn_Exp*_tmp36B;_LL185: {struct Cyc_Absyn_Cast_e_struct*
_tmp35A=(struct Cyc_Absyn_Cast_e_struct*)_tmp359;if(_tmp35A->tag != 15)goto _LL187;
else{_tmp35B=_tmp35A->f2;}}_LL186: e=_tmp35B;goto loop;_LL187: {struct Cyc_Absyn_Const_e_struct*
_tmp35C=(struct Cyc_Absyn_Const_e_struct*)_tmp359;if(_tmp35C->tag != 0)goto _LL189;
else{_tmp35D=_tmp35C->f1;if((_tmp35D.Int_c).tag != 5)goto _LL189;_tmp35E=(struct
_tuple6)(_tmp35D.Int_c).val;_tmp35F=_tmp35E.f2;}}_LL188: {struct Cyc_CfFlowInfo_Reln*
_tmp62C;struct Cyc_List_List*_tmp62B;return(_tmp62B=_region_malloc(rgn,sizeof(*
_tmp62B)),((_tmp62B->hd=((_tmp62C=_region_malloc(rgn,sizeof(*_tmp62C)),((_tmp62C->vd=
v,((_tmp62C->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp35F),_tmp62C)))))),((
_tmp62B->tl=r,_tmp62B)))));}_LL189: {struct Cyc_Absyn_Primop_e_struct*_tmp360=(
struct Cyc_Absyn_Primop_e_struct*)_tmp359;if(_tmp360->tag != 3)goto _LL18B;else{
_tmp361=_tmp360->f1;if(_tmp361 != Cyc_Absyn_Mod)goto _LL18B;_tmp362=_tmp360->f2;
if(_tmp362 == 0)goto _LL18B;_tmp363=*_tmp362;_tmp364=_tmp363.tl;if(_tmp364 == 0)
goto _LL18B;_tmp365=*_tmp364;_tmp366=(struct Cyc_Absyn_Exp*)_tmp365.hd;}}_LL18A:{
void*_tmp36E=_tmp366->r;enum Cyc_Absyn_Primop _tmp370;struct Cyc_List_List*_tmp371;
struct Cyc_List_List _tmp372;struct Cyc_Absyn_Exp*_tmp373;_LL190: {struct Cyc_Absyn_Primop_e_struct*
_tmp36F=(struct Cyc_Absyn_Primop_e_struct*)_tmp36E;if(_tmp36F->tag != 3)goto _LL192;
else{_tmp370=_tmp36F->f1;_tmp371=_tmp36F->f2;if(_tmp371 == 0)goto _LL192;_tmp372=*
_tmp371;_tmp373=(struct Cyc_Absyn_Exp*)_tmp372.hd;}}_LL191:{void*_tmp374=_tmp373->r;
void*_tmp376;struct Cyc_Absyn_Vardecl*_tmp378;void*_tmp37A;struct Cyc_Absyn_Vardecl*
_tmp37C;void*_tmp37E;struct Cyc_Absyn_Vardecl*_tmp380;void*_tmp382;struct Cyc_Absyn_Vardecl*
_tmp384;_LL195: {struct Cyc_Absyn_Var_e_struct*_tmp375=(struct Cyc_Absyn_Var_e_struct*)
_tmp374;if(_tmp375->tag != 1)goto _LL197;else{_tmp376=(void*)_tmp375->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp377=(struct Cyc_Absyn_Global_b_struct*)_tmp376;if(
_tmp377->tag != 1)goto _LL197;else{_tmp378=_tmp377->f1;}};}}_LL196: _tmp37C=_tmp378;
goto _LL198;_LL197: {struct Cyc_Absyn_Var_e_struct*_tmp379=(struct Cyc_Absyn_Var_e_struct*)
_tmp374;if(_tmp379->tag != 1)goto _LL199;else{_tmp37A=(void*)_tmp379->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp37B=(struct Cyc_Absyn_Local_b_struct*)_tmp37A;if(
_tmp37B->tag != 4)goto _LL199;else{_tmp37C=_tmp37B->f1;}};}}_LL198: _tmp380=_tmp37C;
goto _LL19A;_LL199: {struct Cyc_Absyn_Var_e_struct*_tmp37D=(struct Cyc_Absyn_Var_e_struct*)
_tmp374;if(_tmp37D->tag != 1)goto _LL19B;else{_tmp37E=(void*)_tmp37D->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp37F=(struct Cyc_Absyn_Param_b_struct*)_tmp37E;if(
_tmp37F->tag != 3)goto _LL19B;else{_tmp380=_tmp37F->f1;}};}}_LL19A: _tmp384=_tmp380;
goto _LL19C;_LL19B: {struct Cyc_Absyn_Var_e_struct*_tmp381=(struct Cyc_Absyn_Var_e_struct*)
_tmp374;if(_tmp381->tag != 1)goto _LL19D;else{_tmp382=(void*)_tmp381->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp383=(struct Cyc_Absyn_Pat_b_struct*)_tmp382;if(_tmp383->tag
!= 5)goto _LL19D;else{_tmp384=_tmp383->f1;}};}}_LL19C: if(_tmp384->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp62F;struct Cyc_List_List*_tmp62E;return(_tmp62E=
_region_malloc(rgn,sizeof(*_tmp62E)),((_tmp62E->hd=((_tmp62F=_region_malloc(rgn,
sizeof(*_tmp62F)),((_tmp62F->vd=v,((_tmp62F->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp384),_tmp62F)))))),((_tmp62E->tl=r,_tmp62E)))));};_LL19D:;_LL19E: goto _LL194;
_LL194:;}goto _LL18F;_LL192:;_LL193: goto _LL18F;_LL18F:;}goto _LL184;_LL18B: {
struct Cyc_Absyn_Primop_e_struct*_tmp367=(struct Cyc_Absyn_Primop_e_struct*)
_tmp359;if(_tmp367->tag != 3)goto _LL18D;else{_tmp368=_tmp367->f1;_tmp369=_tmp367->f2;
if(_tmp369 == 0)goto _LL18D;_tmp36A=*_tmp369;_tmp36B=(struct Cyc_Absyn_Exp*)_tmp36A.hd;}}
_LL18C:{void*_tmp387=_tmp36B->r;void*_tmp389;struct Cyc_Absyn_Vardecl*_tmp38B;
void*_tmp38D;struct Cyc_Absyn_Vardecl*_tmp38F;void*_tmp391;struct Cyc_Absyn_Vardecl*
_tmp393;void*_tmp395;struct Cyc_Absyn_Vardecl*_tmp397;_LL1A0: {struct Cyc_Absyn_Var_e_struct*
_tmp388=(struct Cyc_Absyn_Var_e_struct*)_tmp387;if(_tmp388->tag != 1)goto _LL1A2;
else{_tmp389=(void*)_tmp388->f2;{struct Cyc_Absyn_Global_b_struct*_tmp38A=(struct
Cyc_Absyn_Global_b_struct*)_tmp389;if(_tmp38A->tag != 1)goto _LL1A2;else{_tmp38B=
_tmp38A->f1;}};}}_LL1A1: _tmp38F=_tmp38B;goto _LL1A3;_LL1A2: {struct Cyc_Absyn_Var_e_struct*
_tmp38C=(struct Cyc_Absyn_Var_e_struct*)_tmp387;if(_tmp38C->tag != 1)goto _LL1A4;
else{_tmp38D=(void*)_tmp38C->f2;{struct Cyc_Absyn_Local_b_struct*_tmp38E=(struct
Cyc_Absyn_Local_b_struct*)_tmp38D;if(_tmp38E->tag != 4)goto _LL1A4;else{_tmp38F=
_tmp38E->f1;}};}}_LL1A3: _tmp393=_tmp38F;goto _LL1A5;_LL1A4: {struct Cyc_Absyn_Var_e_struct*
_tmp390=(struct Cyc_Absyn_Var_e_struct*)_tmp387;if(_tmp390->tag != 1)goto _LL1A6;
else{_tmp391=(void*)_tmp390->f2;{struct Cyc_Absyn_Param_b_struct*_tmp392=(struct
Cyc_Absyn_Param_b_struct*)_tmp391;if(_tmp392->tag != 3)goto _LL1A6;else{_tmp393=
_tmp392->f1;}};}}_LL1A5: _tmp397=_tmp393;goto _LL1A7;_LL1A6: {struct Cyc_Absyn_Var_e_struct*
_tmp394=(struct Cyc_Absyn_Var_e_struct*)_tmp387;if(_tmp394->tag != 1)goto _LL1A8;
else{_tmp395=(void*)_tmp394->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp396=(struct Cyc_Absyn_Pat_b_struct*)
_tmp395;if(_tmp396->tag != 5)goto _LL1A8;else{_tmp397=_tmp396->f1;}};}}_LL1A7: if(
_tmp397->escapes)return r;{struct Cyc_CfFlowInfo_Reln*_tmp632;struct Cyc_List_List*
_tmp631;return(_tmp631=_region_malloc(rgn,sizeof(*_tmp631)),((_tmp631->hd=((
_tmp632=_region_malloc(rgn,sizeof(*_tmp632)),((_tmp632->vd=v,((_tmp632->rop=Cyc_CfFlowInfo_LessEqNumelts(
_tmp397),_tmp632)))))),((_tmp631->tl=r,_tmp631)))));};_LL1A8:;_LL1A9: goto _LL19F;
_LL19F:;}goto _LL184;_LL18D:;_LL18E: goto _LL184;_LL184:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp39A=e1->r;void*_tmp39C;
struct Cyc_Absyn_Vardecl*_tmp39E;void*_tmp3A0;struct Cyc_Absyn_Vardecl*_tmp3A2;
void*_tmp3A4;struct Cyc_Absyn_Vardecl*_tmp3A6;void*_tmp3A8;struct Cyc_Absyn_Vardecl*
_tmp3AA;_LL1AB: {struct Cyc_Absyn_Var_e_struct*_tmp39B=(struct Cyc_Absyn_Var_e_struct*)
_tmp39A;if(_tmp39B->tag != 1)goto _LL1AD;else{_tmp39C=(void*)_tmp39B->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp39D=(struct Cyc_Absyn_Global_b_struct*)_tmp39C;if(
_tmp39D->tag != 1)goto _LL1AD;else{_tmp39E=_tmp39D->f1;}};}}_LL1AC: _tmp3A2=_tmp39E;
goto _LL1AE;_LL1AD: {struct Cyc_Absyn_Var_e_struct*_tmp39F=(struct Cyc_Absyn_Var_e_struct*)
_tmp39A;if(_tmp39F->tag != 1)goto _LL1AF;else{_tmp3A0=(void*)_tmp39F->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp3A1=(struct Cyc_Absyn_Param_b_struct*)_tmp3A0;if(
_tmp3A1->tag != 3)goto _LL1AF;else{_tmp3A2=_tmp3A1->f1;}};}}_LL1AE: _tmp3A6=_tmp3A2;
goto _LL1B0;_LL1AF: {struct Cyc_Absyn_Var_e_struct*_tmp3A3=(struct Cyc_Absyn_Var_e_struct*)
_tmp39A;if(_tmp3A3->tag != 1)goto _LL1B1;else{_tmp3A4=(void*)_tmp3A3->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp3A5=(struct Cyc_Absyn_Local_b_struct*)_tmp3A4;if(
_tmp3A5->tag != 4)goto _LL1B1;else{_tmp3A6=_tmp3A5->f1;}};}}_LL1B0: _tmp3AA=_tmp3A6;
goto _LL1B2;_LL1B1: {struct Cyc_Absyn_Var_e_struct*_tmp3A7=(struct Cyc_Absyn_Var_e_struct*)
_tmp39A;if(_tmp3A7->tag != 1)goto _LL1B3;else{_tmp3A8=(void*)_tmp3A7->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp3A9=(struct Cyc_Absyn_Pat_b_struct*)_tmp3A8;if(_tmp3A9->tag
!= 5)goto _LL1B3;else{_tmp3AA=_tmp3A9->f1;}};}}_LL1B2: if(!_tmp3AA->escapes)return
Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp3AA,e2);goto _LL1AA;_LL1B3:;_LL1B4: goto
_LL1AA;_LL1AA:;}return r;}static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){{const char*_tmp636;void*_tmp635[1];struct Cyc_String_pa_struct _tmp634;(_tmp634.tag=
0,((_tmp634.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name)),((_tmp635[0]=& _tmp634,Cyc_fprintf(Cyc_stderr,((_tmp636="%s",
_tag_dyneither(_tmp636,sizeof(char),3))),_tag_dyneither(_tmp635,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp3AE=r->rop;unsigned int _tmp3AF;struct _tuple11
_tmp3B0;struct Cyc_Absyn_Vardecl*_tmp3B1;struct Cyc_Absyn_Vardecl*_tmp3B2;
unsigned int _tmp3B3;unsigned int _tmp3B4;struct Cyc_Absyn_Vardecl*_tmp3B5;_LL1B6:
if((_tmp3AE.EqualConst).tag != 1)goto _LL1B8;_tmp3AF=(unsigned int)(_tmp3AE.EqualConst).val;
_LL1B7:{const char*_tmp63A;void*_tmp639[1];struct Cyc_Int_pa_struct _tmp638;(
_tmp638.tag=1,((_tmp638.f1=(unsigned long)((int)_tmp3AF),((_tmp639[0]=& _tmp638,
Cyc_fprintf(Cyc_stderr,((_tmp63A="==%d",_tag_dyneither(_tmp63A,sizeof(char),5))),
_tag_dyneither(_tmp639,sizeof(void*),1)))))));}goto _LL1B5;_LL1B8: if((_tmp3AE.LessVar).tag
!= 2)goto _LL1BA;_tmp3B0=(struct _tuple11)(_tmp3AE.LessVar).val;_tmp3B1=_tmp3B0.f1;
_LL1B9:{const char*_tmp63E;void*_tmp63D[1];struct Cyc_String_pa_struct _tmp63C;(
_tmp63C.tag=0,((_tmp63C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp3B1->name)),((_tmp63D[0]=& _tmp63C,Cyc_fprintf(Cyc_stderr,((_tmp63E="<%s",
_tag_dyneither(_tmp63E,sizeof(char),4))),_tag_dyneither(_tmp63D,sizeof(void*),1)))))));}
goto _LL1B5;_LL1BA: if((_tmp3AE.LessNumelts).tag != 3)goto _LL1BC;_tmp3B2=(struct Cyc_Absyn_Vardecl*)(
_tmp3AE.LessNumelts).val;_LL1BB:{const char*_tmp642;void*_tmp641[1];struct Cyc_String_pa_struct
_tmp640;(_tmp640.tag=0,((_tmp640.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp3B2->name)),((_tmp641[0]=& _tmp640,Cyc_fprintf(Cyc_stderr,((
_tmp642="<numelts(%s)",_tag_dyneither(_tmp642,sizeof(char),13))),_tag_dyneither(
_tmp641,sizeof(void*),1)))))));}goto _LL1B5;_LL1BC: if((_tmp3AE.LessConst).tag != 4)
goto _LL1BE;_tmp3B3=(unsigned int)(_tmp3AE.LessConst).val;_LL1BD:{const char*
_tmp646;void*_tmp645[1];struct Cyc_Int_pa_struct _tmp644;(_tmp644.tag=1,((_tmp644.f1=(
unsigned long)((int)_tmp3B3),((_tmp645[0]=& _tmp644,Cyc_fprintf(Cyc_stderr,((
_tmp646="<%d",_tag_dyneither(_tmp646,sizeof(char),4))),_tag_dyneither(_tmp645,
sizeof(void*),1)))))));}goto _LL1B5;_LL1BE: if((_tmp3AE.LessEqConst).tag != 6)goto
_LL1C0;_tmp3B4=(unsigned int)(_tmp3AE.LessEqConst).val;_LL1BF:{const char*_tmp64A;
void*_tmp649[1];struct Cyc_Int_pa_struct _tmp648;(_tmp648.tag=1,((_tmp648.f1=(
unsigned long)((int)_tmp3B4),((_tmp649[0]=& _tmp648,Cyc_fprintf(Cyc_stderr,((
_tmp64A="<=%d",_tag_dyneither(_tmp64A,sizeof(char),5))),_tag_dyneither(_tmp649,
sizeof(void*),1)))))));}goto _LL1B5;_LL1C0: if((_tmp3AE.LessEqNumelts).tag != 5)
goto _LL1B5;_tmp3B5=(struct Cyc_Absyn_Vardecl*)(_tmp3AE.LessEqNumelts).val;_LL1C1:{
const char*_tmp64E;void*_tmp64D[1];struct Cyc_String_pa_struct _tmp64C;(_tmp64C.tag=
0,((_tmp64C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp3B5->name)),((_tmp64D[0]=& _tmp64C,Cyc_fprintf(Cyc_stderr,((_tmp64E="<=numelts(%s)",
_tag_dyneither(_tmp64E,sizeof(char),14))),_tag_dyneither(_tmp64D,sizeof(void*),1)))))));}
goto _LL1B5;_LL1B5:;};}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(
0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);
if(r->tl != 0){const char*_tmp651;void*_tmp650;(_tmp650=0,Cyc_fprintf(Cyc_stderr,((
_tmp651=",",_tag_dyneither(_tmp651,sizeof(char),2))),_tag_dyneither(_tmp650,
sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel
il){switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1C2:{const char*_tmp654;void*_tmp653;(
_tmp653=0,Cyc_fprintf(Cyc_stderr,((_tmp654="uninitialized",_tag_dyneither(
_tmp654,sizeof(char),14))),_tag_dyneither(_tmp653,sizeof(void*),0)));}break;case
Cyc_CfFlowInfo_ThisIL: _LL1C3:{const char*_tmp657;void*_tmp656;(_tmp656=0,Cyc_fprintf(
Cyc_stderr,((_tmp657="this-initialized",_tag_dyneither(_tmp657,sizeof(char),17))),
_tag_dyneither(_tmp656,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1C4:{
const char*_tmp65A;void*_tmp659;(_tmp659=0,Cyc_fprintf(Cyc_stderr,((_tmp65A="all-initialized",
_tag_dyneither(_tmp65A,sizeof(char),16))),_tag_dyneither(_tmp659,sizeof(void*),0)));}
break;}}void Cyc_CfFlowInfo_print_root(void*root){void*_tmp3D0=root;struct Cyc_Absyn_Vardecl*
_tmp3D2;struct Cyc_Absyn_Exp*_tmp3D4;void*_tmp3D5;_LL1C7: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp3D1=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp3D0;if(_tmp3D1->tag != 0)goto
_LL1C9;else{_tmp3D2=_tmp3D1->f1;}}_LL1C8:{const char*_tmp65E;void*_tmp65D[1];
struct Cyc_String_pa_struct _tmp65C;(_tmp65C.tag=0,((_tmp65C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3D2->name)),((
_tmp65D[0]=& _tmp65C,Cyc_fprintf(Cyc_stderr,((_tmp65E="Root(%s)",_tag_dyneither(
_tmp65E,sizeof(char),9))),_tag_dyneither(_tmp65D,sizeof(void*),1)))))));}goto
_LL1C6;_LL1C9: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp3D3=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp3D0;if(_tmp3D3->tag != 1)goto _LL1CB;else{_tmp3D4=_tmp3D3->f1;_tmp3D5=(void*)
_tmp3D3->f2;}}_LL1CA:{const char*_tmp663;void*_tmp662[2];struct Cyc_String_pa_struct
_tmp661;struct Cyc_String_pa_struct _tmp660;(_tmp660.tag=0,((_tmp660.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3D5)),((_tmp661.tag=
0,((_tmp661.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
_tmp3D4)),((_tmp662[0]=& _tmp661,((_tmp662[1]=& _tmp660,Cyc_fprintf(Cyc_stderr,((
_tmp663="MallocPt(%s,%s)",_tag_dyneither(_tmp663,sizeof(char),16))),
_tag_dyneither(_tmp662,sizeof(void*),2)))))))))))));}goto _LL1C6;_LL1CB: {struct
Cyc_CfFlowInfo_InitParam_struct*_tmp3D6=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp3D0;if(_tmp3D6->tag != 2)goto _LL1C6;}_LL1CC:{const char*_tmp666;void*_tmp665;(
_tmp665=0,Cyc_fprintf(Cyc_stderr,((_tmp666="InitParam(_,_)",_tag_dyneither(
_tmp666,sizeof(char),15))),_tag_dyneither(_tmp665,sizeof(void*),0)));}goto _LL1C6;
_LL1C6:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){Cyc_CfFlowInfo_print_root(
p->root);{struct Cyc_List_List*_tmp3E0=p->fields;while(_tmp3E0 != 0){{const char*
_tmp66A;void*_tmp669[1];struct Cyc_Int_pa_struct _tmp668;(_tmp668.tag=1,((_tmp668.f1=(
unsigned long)((int)_tmp3E0->hd),((_tmp669[0]=& _tmp668,Cyc_fprintf(Cyc_stderr,((
_tmp66A=".%d",_tag_dyneither(_tmp66A,sizeof(char),4))),_tag_dyneither(_tmp669,
sizeof(void*),1)))))));}_tmp3E0=_tmp3E0->tl;}{const char*_tmp66D;void*_tmp66C;(
_tmp66C=0,Cyc_fprintf(Cyc_stderr,((_tmp66D=" ",_tag_dyneither(_tmp66D,sizeof(
char),2))),_tag_dyneither(_tmp66C,sizeof(void*),0)));};};}void Cyc_CfFlowInfo_print_list(
struct Cyc_List_List*x,void(*pr)(void*)){int _tmp3E6=1;while(x != 0){if(!_tmp3E6){{
const char*_tmp670;void*_tmp66F;(_tmp66F=0,Cyc_fprintf(Cyc_stderr,((_tmp670=", ",
_tag_dyneither(_tmp670,sizeof(char),3))),_tag_dyneither(_tmp66F,sizeof(void*),0)));}
_tmp3E6=0;}pr((void*)x->hd);x=x->tl;}{const char*_tmp673;void*_tmp672;(_tmp672=0,
Cyc_fprintf(Cyc_stderr,((_tmp673="\n",_tag_dyneither(_tmp673,sizeof(char),2))),
_tag_dyneither(_tmp672,sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*)){struct _RegionHandle _tmp3EB=_new_region("r");
struct _RegionHandle*r=& _tmp3EB;_push_region(r);{struct _handler_cons _tmp3EC;
_push_handler(& _tmp3EC);{int _tmp3EE=0;if(setjmp(_tmp3EC.handler))_tmp3EE=1;if(!
_tmp3EE){{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple14 _tmp3EF=*((
struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;{const char*_tmp676;void*_tmp675;(_tmp675=0,Cyc_fprintf(Cyc_stderr,((
_tmp676="{ ",_tag_dyneither(_tmp676,sizeof(char),3))),_tag_dyneither(_tmp675,
sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(
iter,& _tmp3EF)){void*_tmp3F2=_tmp3EF.f1;if(!first){{const char*_tmp679;void*
_tmp678;(_tmp678=0,Cyc_fprintf(Cyc_stderr,((_tmp679=", ",_tag_dyneither(_tmp679,
sizeof(char),3))),_tag_dyneither(_tmp678,sizeof(void*),0)));}first=0;}pr(_tmp3F2);}{
const char*_tmp67C;void*_tmp67B;(_tmp67B=0,Cyc_fprintf(Cyc_stderr,((_tmp67C="}\n",
_tag_dyneither(_tmp67C,sizeof(char),3))),_tag_dyneither(_tmp67B,sizeof(void*),0)));};};
_pop_handler();}else{void*_tmp3ED=(void*)_exn_thrown;void*_tmp3F8=_tmp3ED;_LL1CE:;
_LL1CF:{const char*_tmp67F;void*_tmp67E;(_tmp67E=0,Cyc_fprintf(Cyc_stderr,((
_tmp67F="{ }\n",_tag_dyneither(_tmp67F,sizeof(char),5))),_tag_dyneither(_tmp67E,
sizeof(void*),0)));}_npop_handler(0);return;_LL1D0:;_LL1D1:(void)_throw(_tmp3F8);
_LL1CD:;}};};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval){void*
_tmp3FB=rval;enum Cyc_CfFlowInfo_InitLevel _tmp400;enum Cyc_CfFlowInfo_InitLevel
_tmp402;struct Cyc_CfFlowInfo_Place*_tmp404;struct Cyc_CfFlowInfo_UnionRInfo
_tmp406;int _tmp407;int _tmp408;struct _dyneither_ptr _tmp409;struct Cyc_Absyn_Exp*
_tmp40B;int _tmp40C;void*_tmp40D;_LL1D3: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3FC=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3FB;if(_tmp3FC->tag != 0)goto
_LL1D5;}_LL1D4:{const char*_tmp682;void*_tmp681;(_tmp681=0,Cyc_fprintf(Cyc_stderr,((
_tmp682="Zero",_tag_dyneither(_tmp682,sizeof(char),5))),_tag_dyneither(_tmp681,
sizeof(void*),0)));}goto _LL1D2;_LL1D5: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3FD=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3FB;if(_tmp3FD->tag != 1)
goto _LL1D7;}_LL1D6:{const char*_tmp685;void*_tmp684;(_tmp684=0,Cyc_fprintf(Cyc_stderr,((
_tmp685="NotZeroAll",_tag_dyneither(_tmp685,sizeof(char),11))),_tag_dyneither(
_tmp684,sizeof(void*),0)));}goto _LL1D2;_LL1D7: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3FE=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3FB;if(_tmp3FE->tag != 2)
goto _LL1D9;}_LL1D8:{const char*_tmp688;void*_tmp687;(_tmp687=0,Cyc_fprintf(Cyc_stderr,((
_tmp688="NotZeroThis",_tag_dyneither(_tmp688,sizeof(char),12))),_tag_dyneither(
_tmp687,sizeof(void*),0)));}goto _LL1D2;_LL1D9: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3FF=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3FB;if(_tmp3FF->tag != 3)goto
_LL1DB;else{_tmp400=_tmp3FF->f1;}}_LL1DA:{const char*_tmp68B;void*_tmp68A;(
_tmp68A=0,Cyc_fprintf(Cyc_stderr,((_tmp68B="Unknown(",_tag_dyneither(_tmp68B,
sizeof(char),9))),_tag_dyneither(_tmp68A,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp400);{const char*_tmp68E;void*_tmp68D;(_tmp68D=0,Cyc_fprintf(Cyc_stderr,((
_tmp68E=")",_tag_dyneither(_tmp68E,sizeof(char),2))),_tag_dyneither(_tmp68D,
sizeof(void*),0)));}goto _LL1D2;_LL1DB: {struct Cyc_CfFlowInfo_Esc_struct*_tmp401=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp3FB;if(_tmp401->tag != 4)goto _LL1DD;else{
_tmp402=_tmp401->f1;}}_LL1DC:{const char*_tmp691;void*_tmp690;(_tmp690=0,Cyc_fprintf(
Cyc_stderr,((_tmp691="Esc(",_tag_dyneither(_tmp691,sizeof(char),5))),
_tag_dyneither(_tmp690,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp402);{const char*_tmp694;void*_tmp693;(_tmp693=0,Cyc_fprintf(Cyc_stderr,((
_tmp694=")",_tag_dyneither(_tmp694,sizeof(char),2))),_tag_dyneither(_tmp693,
sizeof(void*),0)));}goto _LL1D2;_LL1DD: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp403=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3FB;if(_tmp403->tag != 5)goto
_LL1DF;else{_tmp404=_tmp403->f1;}}_LL1DE:{const char*_tmp697;void*_tmp696;(
_tmp696=0,Cyc_fprintf(Cyc_stderr,((_tmp697="AddrOf(",_tag_dyneither(_tmp697,
sizeof(char),8))),_tag_dyneither(_tmp696,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(
_tmp404);{const char*_tmp69A;void*_tmp699;(_tmp699=0,Cyc_fprintf(Cyc_stderr,((
_tmp69A=")",_tag_dyneither(_tmp69A,sizeof(char),2))),_tag_dyneither(_tmp699,
sizeof(void*),0)));}goto _LL1D2;_LL1DF: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp405=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp3FB;if(_tmp405->tag != 6)goto
_LL1E1;else{_tmp406=_tmp405->f1;_tmp407=_tmp406.is_union;_tmp408=_tmp406.fieldnum;
_tmp409=_tmp405->f2;}}_LL1E0: if(_tmp407){{const char*_tmp69D;void*_tmp69C;(
_tmp69C=0,Cyc_fprintf(Cyc_stderr,((_tmp69D="AggrUnion{",_tag_dyneither(_tmp69D,
sizeof(char),11))),_tag_dyneither(_tmp69C,sizeof(void*),0)));}if(_tmp408 != - 1){
const char*_tmp6A1;void*_tmp6A0[1];struct Cyc_Int_pa_struct _tmp69F;(_tmp69F.tag=1,((
_tmp69F.f1=(unsigned long)_tmp408,((_tmp6A0[0]=& _tmp69F,Cyc_fprintf(Cyc_stderr,((
_tmp6A1="tag == %d;",_tag_dyneither(_tmp6A1,sizeof(char),11))),_tag_dyneither(
_tmp6A0,sizeof(void*),1)))))));}}else{const char*_tmp6A4;void*_tmp6A3;(_tmp6A3=0,
Cyc_fprintf(Cyc_stderr,((_tmp6A4="AggrStruct{",_tag_dyneither(_tmp6A4,sizeof(
char),12))),_tag_dyneither(_tmp6A3,sizeof(void*),0)));}{int i=0;for(0;i < 
_get_dyneither_size(_tmp409,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp409.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp409,sizeof(void*))){
const char*_tmp6A7;void*_tmp6A6;(_tmp6A6=0,Cyc_fprintf(Cyc_stderr,((_tmp6A7=",",
_tag_dyneither(_tmp6A7,sizeof(char),2))),_tag_dyneither(_tmp6A6,sizeof(void*),0)));}}}{
const char*_tmp6AA;void*_tmp6A9;(_tmp6A9=0,Cyc_fprintf(Cyc_stderr,((_tmp6AA="}",
_tag_dyneither(_tmp6AA,sizeof(char),2))),_tag_dyneither(_tmp6A9,sizeof(void*),0)));}
goto _LL1D2;_LL1E1: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp40A=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp3FB;if(_tmp40A->tag != 7)goto _LL1D2;else{_tmp40B=_tmp40A->f1;_tmp40C=_tmp40A->f2;
_tmp40D=(void*)_tmp40A->f3;}}_LL1E2:{const char*_tmp6AF;void*_tmp6AE[2];struct Cyc_String_pa_struct
_tmp6AD;struct Cyc_Int_pa_struct _tmp6AC;(_tmp6AC.tag=1,((_tmp6AC.f1=(
unsigned long)_tmp40C,((_tmp6AD.tag=0,((_tmp6AD.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp40B)),((_tmp6AE[0]=& _tmp6AD,((
_tmp6AE[1]=& _tmp6AC,Cyc_fprintf(Cyc_stderr,((_tmp6AF="[Consumed(%s,%d,",
_tag_dyneither(_tmp6AF,sizeof(char),17))),_tag_dyneither(_tmp6AE,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp40D);{const char*_tmp6B2;void*_tmp6B1;(_tmp6B1=0,
Cyc_fprintf(Cyc_stderr,((_tmp6B2="]",_tag_dyneither(_tmp6B2,sizeof(char),2))),
_tag_dyneither(_tmp6B1,sizeof(void*),0)));};_LL1D2:;}static void Cyc_CfFlowInfo_print_flow_mapping(
void*root,void*rval){{const char*_tmp6B5;void*_tmp6B4;(_tmp6B4=0,Cyc_fprintf(Cyc_stderr,((
_tmp6B5="    ",_tag_dyneither(_tmp6B5,sizeof(char),5))),_tag_dyneither(_tmp6B4,
sizeof(void*),0)));}Cyc_CfFlowInfo_print_root(root);{const char*_tmp6B8;void*
_tmp6B7;(_tmp6B7=0,Cyc_fprintf(Cyc_stderr,((_tmp6B8=" --> ",_tag_dyneither(
_tmp6B8,sizeof(char),6))),_tag_dyneither(_tmp6B7,sizeof(void*),0)));}Cyc_CfFlowInfo_print_absrval(
rval);{const char*_tmp6BB;void*_tmp6BA;(_tmp6BA=0,Cyc_fprintf(Cyc_stderr,((
_tmp6BB="\n",_tag_dyneither(_tmp6BB,sizeof(char),2))),_tag_dyneither(_tmp6BA,
sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){((
void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,
d);}void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo
_tmp437=f;int _tmp438;struct _tuple12 _tmp439;struct Cyc_Dict_Dict _tmp43A;_LL1E4: if((
_tmp437.BottomFL).tag != 1)goto _LL1E6;_tmp438=(int)(_tmp437.BottomFL).val;_LL1E5:{
const char*_tmp6BE;void*_tmp6BD;(_tmp6BD=0,Cyc_fprintf(Cyc_stderr,((_tmp6BE="  BottomFL\n",
_tag_dyneither(_tmp6BE,sizeof(char),12))),_tag_dyneither(_tmp6BD,sizeof(void*),0)));}
goto _LL1E3;_LL1E6: if((_tmp437.ReachableFL).tag != 2)goto _LL1E3;_tmp439=(struct
_tuple12)(_tmp437.ReachableFL).val;_tmp43A=_tmp439.f1;_LL1E7:{const char*_tmp6C1;
void*_tmp6C0;(_tmp6C0=0,Cyc_fprintf(Cyc_stderr,((_tmp6C1="  ReachableFL:\n",
_tag_dyneither(_tmp6C1,sizeof(char),16))),_tag_dyneither(_tmp6C0,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp43A);{const char*_tmp6C4;void*_tmp6C3;(_tmp6C3=
0,Cyc_fprintf(Cyc_stderr,((_tmp6C4="\n",_tag_dyneither(_tmp6C4,sizeof(char),2))),
_tag_dyneither(_tmp6C3,sizeof(void*),0)));}goto _LL1E3;_LL1E3:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t){void*_tmp441=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp44E;struct Cyc_Absyn_DatatypeInfo _tmp450;struct Cyc_List_List*_tmp451;struct
Cyc_List_List*_tmp453;struct Cyc_Absyn_AggrInfo _tmp455;struct Cyc_List_List*
_tmp456;struct Cyc_Absyn_DatatypeFieldInfo _tmp458;struct Cyc_List_List*_tmp459;
struct Cyc_List_List*_tmp45B;struct Cyc_Absyn_PtrInfo _tmp45D;void*_tmp45E;struct
Cyc_Absyn_PtrAtts _tmp45F;void*_tmp460;struct Cyc_List_List*_tmp463;struct Cyc_List_List*
_tmp465;struct Cyc_Absyn_ArrayInfo _tmp467;void*_tmp468;void*_tmp46A;void*_tmp46C;
void*_tmp46E;void*_tmp46F;void*_tmp471;_LL1E9: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp442=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp441;if(_tmp442->tag != 26)goto
_LL1EB;}_LL1EA: goto _LL1EC;_LL1EB: {struct Cyc_Absyn_VoidType_struct*_tmp443=(
struct Cyc_Absyn_VoidType_struct*)_tmp441;if(_tmp443->tag != 0)goto _LL1ED;}_LL1EC:
goto _LL1EE;_LL1ED: {struct Cyc_Absyn_IntType_struct*_tmp444=(struct Cyc_Absyn_IntType_struct*)
_tmp441;if(_tmp444->tag != 6)goto _LL1EF;}_LL1EE: goto _LL1F0;_LL1EF: {struct Cyc_Absyn_FloatType_struct*
_tmp445=(struct Cyc_Absyn_FloatType_struct*)_tmp441;if(_tmp445->tag != 7)goto
_LL1F1;}_LL1F0: goto _LL1F2;_LL1F1: {struct Cyc_Absyn_EnumType_struct*_tmp446=(
struct Cyc_Absyn_EnumType_struct*)_tmp441;if(_tmp446->tag != 13)goto _LL1F3;}_LL1F2:
goto _LL1F4;_LL1F3: {struct Cyc_Absyn_AnonEnumType_struct*_tmp447=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp441;if(_tmp447->tag != 14)goto _LL1F5;}_LL1F4: goto _LL1F6;_LL1F5: {struct Cyc_Absyn_TagType_struct*
_tmp448=(struct Cyc_Absyn_TagType_struct*)_tmp441;if(_tmp448->tag != 19)goto _LL1F7;}
_LL1F6: goto _LL1F8;_LL1F7: {struct Cyc_Absyn_HeapRgn_struct*_tmp449=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp441;if(_tmp449->tag != 20)goto _LL1F9;}_LL1F8: goto _LL1FA;_LL1F9: {struct Cyc_Absyn_Evar_struct*
_tmp44A=(struct Cyc_Absyn_Evar_struct*)_tmp441;if(_tmp44A->tag != 1)goto _LL1FB;}
_LL1FA: return 0;_LL1FB: {struct Cyc_Absyn_UniqueRgn_struct*_tmp44B=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp441;if(_tmp44B->tag != 21)goto _LL1FD;}_LL1FC: {void*_tmp473=rgn;_LL220: {
struct Cyc_CfFlowInfo_UniqueRgn_k_struct*_tmp474=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)
_tmp473;if(_tmp474->tag != 0)goto _LL222;}_LL221: return 1;_LL222:;_LL223: return 0;
_LL21F:;}_LL1FD: {struct Cyc_Absyn_RefCntRgn_struct*_tmp44C=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp441;if(_tmp44C->tag != 22)goto _LL1FF;}_LL1FE: {void*_tmp475=rgn;_LL225: {
struct Cyc_CfFlowInfo_RefCntRgn_k_struct*_tmp476=(struct Cyc_CfFlowInfo_RefCntRgn_k_struct*)
_tmp475;if(_tmp476->tag != 1)goto _LL227;}_LL226: return 1;_LL227:;_LL228: return 0;
_LL224:;}_LL1FF: {struct Cyc_Absyn_VarType_struct*_tmp44D=(struct Cyc_Absyn_VarType_struct*)
_tmp441;if(_tmp44D->tag != 2)goto _LL201;else{_tmp44E=_tmp44D->f1;}}_LL200: {void*
_tmp477=rgn;struct Cyc_Absyn_Tvar*_tmp47B;_LL22A: {struct Cyc_CfFlowInfo_RefCntRgn_k_struct*
_tmp478=(struct Cyc_CfFlowInfo_RefCntRgn_k_struct*)_tmp477;if(_tmp478->tag != 1)
goto _LL22C;}_LL22B: goto _LL22D;_LL22C: {struct Cyc_CfFlowInfo_UniqueRgn_k_struct*
_tmp479=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)_tmp477;if(_tmp479->tag != 0)
goto _LL22E;}_LL22D: return 0;_LL22E: {struct Cyc_CfFlowInfo_Region_k_struct*_tmp47A=(
struct Cyc_CfFlowInfo_Region_k_struct*)_tmp477;if(_tmp47A->tag != 2)goto _LL229;
else{_tmp47B=_tmp47A->f1;}}_LL22F: return Cyc_Absyn_tvar_cmp(_tmp44E,_tmp47B)== 0;
_LL229:;}_LL201: {struct Cyc_Absyn_DatatypeType_struct*_tmp44F=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp441;if(_tmp44F->tag != 3)goto _LL203;else{_tmp450=_tmp44F->f1;_tmp451=_tmp450.targs;}}
_LL202: _tmp453=_tmp451;goto _LL204;_LL203: {struct Cyc_Absyn_TypedefType_struct*
_tmp452=(struct Cyc_Absyn_TypedefType_struct*)_tmp441;if(_tmp452->tag != 17)goto
_LL205;else{_tmp453=_tmp452->f2;}}_LL204: _tmp456=_tmp453;goto _LL206;_LL205: {
struct Cyc_Absyn_AggrType_struct*_tmp454=(struct Cyc_Absyn_AggrType_struct*)
_tmp441;if(_tmp454->tag != 11)goto _LL207;else{_tmp455=_tmp454->f1;_tmp456=_tmp455.targs;}}
_LL206: _tmp459=_tmp456;goto _LL208;_LL207: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp457=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp441;if(_tmp457->tag != 4)
goto _LL209;else{_tmp458=_tmp457->f1;_tmp459=_tmp458.targs;}}_LL208: _tmp45B=
_tmp459;goto _LL20A;_LL209: {struct Cyc_Absyn_JoinEff_struct*_tmp45A=(struct Cyc_Absyn_JoinEff_struct*)
_tmp441;if(_tmp45A->tag != 24)goto _LL20B;else{_tmp45B=_tmp45A->f1;}}_LL20A: return((
int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(
Cyc_CfFlowInfo_contains_region,rgn,_tmp45B);_LL20B: {struct Cyc_Absyn_PointerType_struct*
_tmp45C=(struct Cyc_Absyn_PointerType_struct*)_tmp441;if(_tmp45C->tag != 5)goto
_LL20D;else{_tmp45D=_tmp45C->f1;_tmp45E=_tmp45D.elt_typ;_tmp45F=_tmp45D.ptr_atts;
_tmp460=_tmp45F.rgn;}}_LL20C: return Cyc_CfFlowInfo_contains_region(rgn,_tmp460)
 || Cyc_CfFlowInfo_contains_region(rgn,_tmp45E);_LL20D: {struct Cyc_Absyn_FnType_struct*
_tmp461=(struct Cyc_Absyn_FnType_struct*)_tmp441;if(_tmp461->tag != 9)goto _LL20F;}
_LL20E: return 0;_LL20F: {struct Cyc_Absyn_TupleType_struct*_tmp462=(struct Cyc_Absyn_TupleType_struct*)
_tmp441;if(_tmp462->tag != 10)goto _LL211;else{_tmp463=_tmp462->f1;}}_LL210: for(0;
_tmp463 != 0;_tmp463=_tmp463->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((
struct _tuple16*)_tmp463->hd)).f2))return 1;}return 0;_LL211: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp464=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp441;if(_tmp464->tag != 12)goto
_LL213;else{_tmp465=_tmp464->f2;}}_LL212: for(0;_tmp465 != 0;_tmp465=_tmp465->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp465->hd)->type))
return 1;}return 0;_LL213: {struct Cyc_Absyn_ArrayType_struct*_tmp466=(struct Cyc_Absyn_ArrayType_struct*)
_tmp441;if(_tmp466->tag != 8)goto _LL215;else{_tmp467=_tmp466->f1;_tmp468=_tmp467.elt_type;}}
_LL214: _tmp46A=_tmp468;goto _LL216;_LL215: {struct Cyc_Absyn_AccessEff_struct*
_tmp469=(struct Cyc_Absyn_AccessEff_struct*)_tmp441;if(_tmp469->tag != 23)goto
_LL217;else{_tmp46A=(void*)_tmp469->f1;}}_LL216: _tmp46C=_tmp46A;goto _LL218;
_LL217: {struct Cyc_Absyn_RgnsEff_struct*_tmp46B=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp441;if(_tmp46B->tag != 25)goto _LL219;else{_tmp46C=(void*)_tmp46B->f1;}}_LL218:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp46C);_LL219: {struct Cyc_Absyn_DynRgnType_struct*
_tmp46D=(struct Cyc_Absyn_DynRgnType_struct*)_tmp441;if(_tmp46D->tag != 16)goto
_LL21B;else{_tmp46E=(void*)_tmp46D->f1;_tmp46F=(void*)_tmp46D->f2;}}_LL21A:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp46E) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp46F);_LL21B: {struct Cyc_Absyn_RgnHandleType_struct*_tmp470=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp441;if(_tmp470->tag != 15)goto _LL21D;else{_tmp471=(void*)_tmp470->f1;}}_LL21C:
return 0;_LL21D: {struct Cyc_Absyn_ValueofType_struct*_tmp472=(struct Cyc_Absyn_ValueofType_struct*)
_tmp441;if(_tmp472->tag != 18)goto _LL1E8;}_LL21E: return 0;_LL1E8:;}struct _tuple24{
struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(
struct _tuple24*env,void*root,void*rval){struct _tuple24 _tmp47D;struct Cyc_CfFlowInfo_FlowEnv*
_tmp47E;struct Cyc_Dict_Dict _tmp47F;struct Cyc_Dict_Dict*_tmp480;void*_tmp481;
struct _tuple24*_tmp47C=env;_tmp47D=*_tmp47C;_tmp47E=_tmp47D.f1;_tmp47F=_tmp47D.f2;
_tmp480=(struct Cyc_Dict_Dict*)&(*_tmp47C).f2;_tmp481=_tmp47D.f3;{void*_tmp482=
root;struct Cyc_Absyn_Vardecl*_tmp484;void*_tmp486;_LL231: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp483=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp482;if(_tmp483->tag != 0)goto
_LL233;else{_tmp484=_tmp483->f1;}}_LL232: if(Cyc_CfFlowInfo_contains_region(
_tmp481,_tmp484->type))rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp47E,_tmp484->type,
0,_tmp47E->unknown_none);*_tmp480=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(*_tmp480,root,rval);goto _LL230;_LL233: {struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp485=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp482;if(_tmp485->tag != 1)goto _LL235;else{_tmp486=(void*)_tmp485->f2;}}_LL234:
if(!Cyc_CfFlowInfo_contains_region(_tmp481,_tmp486))*_tmp480=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp480,root,rval);goto
_LL230;_LL235: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp487=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp482;if(_tmp487->tag != 2)goto _LL230;}_LL236: goto _LL230;_LL230:;};}static
struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp488=Cyc_Tcutil_compress(rgn);
struct Cyc_Absyn_Tvar*_tmp48A;_LL238: {struct Cyc_Absyn_VarType_struct*_tmp489=(
struct Cyc_Absyn_VarType_struct*)_tmp488;if(_tmp489->tag != 2)goto _LL23A;else{
_tmp48A=_tmp489->f1;}}_LL239: {struct Cyc_CfFlowInfo_Region_k_struct _tmp6C5;
struct Cyc_CfFlowInfo_Region_k_struct v=(_tmp6C5.tag=2,((_tmp6C5.f1=_tmp48A,
_tmp6C5)));void*v2=(void*)& v;struct _tuple24 _tmp6C6;struct _tuple24 env=(_tmp6C6.f1=
fenv,((_tmp6C6.f2=fenv->mt_flowdict,((_tmp6C6.f3=v2,_tmp6C6)))));((void(*)(void(*
f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL23A: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp48B=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp488;if(_tmp48B->tag != 21)goto
_LL23C;}_LL23B: {struct _tuple24 _tmp6C7;struct _tuple24 env=(_tmp6C7.f1=fenv,((
_tmp6C7.f2=fenv->mt_flowdict,((_tmp6C7.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,
_tmp6C7)))));((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return
env.f2;}_LL23C: {struct Cyc_Absyn_RefCntRgn_struct*_tmp48C=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp488;if(_tmp48C->tag != 22)goto _LL23E;}_LL23D: {struct _tuple24 _tmp6C8;struct
_tuple24 env=(_tmp6C8.f1=fenv,((_tmp6C8.f2=fenv->mt_flowdict,((_tmp6C8.f3=(void*)&
Cyc_CfFlowInfo_RefCntRgn_k_val,_tmp6C8)))));((void(*)(void(*f)(struct _tuple24*,
void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,&
env,fd);return env.f2;}_LL23E:;_LL23F: {struct Cyc_Core_Impossible_struct _tmp6CE;
const char*_tmp6CD;struct Cyc_Core_Impossible_struct*_tmp6CC;(int)_throw((void*)((
_tmp6CC=_cycalloc(sizeof(*_tmp6CC)),((_tmp6CC[0]=((_tmp6CE.tag=Cyc_Core_Impossible,((
_tmp6CE.f1=((_tmp6CD="kill_flowdict_region",_tag_dyneither(_tmp6CD,sizeof(char),
21))),_tmp6CE)))),_tmp6CC)))));}_LL237:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){union
Cyc_CfFlowInfo_FlowInfo _tmp494=f;int _tmp495;struct _tuple12 _tmp496;struct Cyc_Dict_Dict
_tmp497;struct Cyc_List_List*_tmp498;_LL241: if((_tmp494.BottomFL).tag != 1)goto
_LL243;_tmp495=(int)(_tmp494.BottomFL).val;_LL242: return f;_LL243: if((_tmp494.ReachableFL).tag
!= 2)goto _LL240;_tmp496=(struct _tuple12)(_tmp494.ReachableFL).val;_tmp497=
_tmp496.f1;_tmp498=_tmp496.f2;_LL244: {struct Cyc_Dict_Dict _tmp499=Cyc_CfFlowInfo_kill_flowdict_region(
fenv,_tmp497,rgn);struct Cyc_List_List*_tmp49A=Cyc_CfFlowInfo_kill_relns_region(
_tmp498,rgn);return Cyc_CfFlowInfo_ReachableFL(_tmp499,_tmp49A);}_LL240:;}
