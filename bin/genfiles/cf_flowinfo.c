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
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(int,int);
extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
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
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict
Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_member(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void Cyc_Dict_iter(
void(*f)(void*,void*),struct Cyc_Dict_Dict d);void Cyc_Dict_iter_c(void(*f)(void*,
void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_union_two_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2);struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),
void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
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
_tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{
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
struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
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
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
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
topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Skip_s_struct{
int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
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
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
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
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum Cyc_Absyn_Scope
sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*
fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};extern struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val;int
Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_Absyn_is_nontagged_union_type(void*);struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);int
Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_bits_only(void*t);void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int
add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;
int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_CfFlowInfo_VarRoot_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,
void*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessEqConst(unsigned int);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;union Cyc_CfFlowInfo_RelnOp rop;};char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct
Cyc_CfFlowInfo_IsZero_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";
struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[
9]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*
f1;};extern struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val;struct
_union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
_union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct
_union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int
fieldnum;};struct Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;
struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{
int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*loc);int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple12
val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct
_union_FlowInfo_ReachableFL ReachableFL;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,
struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*
unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict
mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*
f);int Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int
no_init_bits_only,void*leafval);void*Cyc_CfFlowInfo_make_unique_consumed(struct
Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,
void*);int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int
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
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
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
extern struct Cyc_CfFlowInfo_RefCntRgn_k_struct Cyc_CfFlowInfo_RefCntRgn_k_val;int
Cyc_CfFlowInfo_contains_region(void*rgn,void*t);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val={
Cyc_CfFlowInfo_IsZero};struct Cyc_CfFlowInfo_UniqueRgn_k_struct Cyc_CfFlowInfo_UniqueRgn_k_val={
0};struct Cyc_CfFlowInfo_RefCntRgn_k_struct Cyc_CfFlowInfo_RefCntRgn_k_val={1};
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
union Cyc_CfFlowInfo_AbsLVal _tmp49C;return((_tmp49C.PlaceL).val=x,(((_tmp49C.PlaceL).tag=
1,_tmp49C)));}union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){union Cyc_CfFlowInfo_AbsLVal
_tmp49D;return((_tmp49D.UnknownL).val=0,(((_tmp49D.UnknownL).tag=2,_tmp49D)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo
_tmp49E;return((_tmp49E.BottomFL).val=0,(((_tmp49E.BottomFL).tag=1,_tmp49E)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,
struct Cyc_List_List*r){struct _tuple12 _tmp4A1;union Cyc_CfFlowInfo_FlowInfo _tmp4A0;
return((_tmp4A0.ReachableFL).val=((_tmp4A1.f1=fd,((_tmp4A1.f2=r,_tmp4A1)))),(((
_tmp4A0.ReachableFL).tag=2,_tmp4A0)));}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,{.Null_c={1,0}}};static
struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp4D8;struct Cyc_CfFlowInfo_MallocPt_struct _tmp4D7;
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp4D6;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4D5;struct Cyc_CfFlowInfo_Esc_struct _tmp4D4;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4D3;struct Cyc_CfFlowInfo_Esc_struct _tmp4D2;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4D1;struct Cyc_CfFlowInfo_Esc_struct _tmp4D0;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4CF;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4CE;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4CD;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4CC;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4CB;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4CA;struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4C9;struct Cyc_CfFlowInfo_NotZeroThis_struct _tmp4C8;struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4C7;struct Cyc_CfFlowInfo_NotZeroAll_struct _tmp4C6;struct Cyc_CfFlowInfo_Zero_struct*
_tmp4C5;struct Cyc_CfFlowInfo_Zero_struct _tmp4C4;struct Cyc_CfFlowInfo_FlowEnv*
_tmp4C3;return(_tmp4C3=_region_malloc(r,sizeof(*_tmp4C3)),((_tmp4C3->r=r,((
_tmp4C3->zero=(void*)((_tmp4C5=_region_malloc(r,sizeof(*_tmp4C5)),((_tmp4C5[0]=((
_tmp4C4.tag=0,_tmp4C4)),_tmp4C5)))),((_tmp4C3->notzeroall=(void*)((_tmp4C7=
_region_malloc(r,sizeof(*_tmp4C7)),((_tmp4C7[0]=((_tmp4C6.tag=1,_tmp4C6)),
_tmp4C7)))),((_tmp4C3->notzerothis=(void*)((_tmp4C9=_region_malloc(r,sizeof(*
_tmp4C9)),((_tmp4C9[0]=((_tmp4C8.tag=2,_tmp4C8)),_tmp4C9)))),((_tmp4C3->unknown_none=(
void*)((_tmp4CB=_region_malloc(r,sizeof(*_tmp4CB)),((_tmp4CB[0]=((_tmp4CA.tag=3,((
_tmp4CA.f1=Cyc_CfFlowInfo_NoneIL,_tmp4CA)))),_tmp4CB)))),((_tmp4C3->unknown_this=(
void*)((_tmp4CD=_region_malloc(r,sizeof(*_tmp4CD)),((_tmp4CD[0]=((_tmp4CC.tag=3,((
_tmp4CC.f1=Cyc_CfFlowInfo_ThisIL,_tmp4CC)))),_tmp4CD)))),((_tmp4C3->unknown_all=(
void*)((_tmp4CF=_region_malloc(r,sizeof(*_tmp4CF)),((_tmp4CF[0]=((_tmp4CE.tag=3,((
_tmp4CE.f1=Cyc_CfFlowInfo_AllIL,_tmp4CE)))),_tmp4CF)))),((_tmp4C3->esc_none=(
void*)((_tmp4D1=_region_malloc(r,sizeof(*_tmp4D1)),((_tmp4D1[0]=((_tmp4D0.tag=4,((
_tmp4D0.f1=Cyc_CfFlowInfo_NoneIL,_tmp4D0)))),_tmp4D1)))),((_tmp4C3->esc_this=(
void*)((_tmp4D3=_region_malloc(r,sizeof(*_tmp4D3)),((_tmp4D3[0]=((_tmp4D2.tag=4,((
_tmp4D2.f1=Cyc_CfFlowInfo_ThisIL,_tmp4D2)))),_tmp4D3)))),((_tmp4C3->esc_all=(
void*)((_tmp4D5=_region_malloc(r,sizeof(*_tmp4D5)),((_tmp4D5[0]=((_tmp4D4.tag=4,((
_tmp4D4.f1=Cyc_CfFlowInfo_AllIL,_tmp4D4)))),_tmp4D5)))),((_tmp4C3->mt_flowdict=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(
r,Cyc_CfFlowInfo_root_cmp),((_tmp4C3->mt_place_set=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp4C3->dummy_place=((_tmp4D8=
_region_malloc(r,sizeof(*_tmp4D8)),((_tmp4D8->root=(void*)((_tmp4D6=
_region_malloc(r,sizeof(*_tmp4D6)),((_tmp4D6[0]=((_tmp4D7.tag=1,((_tmp4D7.f1=&
dummy_exp,((_tmp4D7.f2=(void*)((void*)& Cyc_Absyn_VoidType_val),_tmp4D7)))))),
_tmp4D6)))),((_tmp4D8->fields=0,_tmp4D8)))))),_tmp4C3)))))))))))))))))))))))))));}
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*
loc){if(set != 0){if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,
place))return 1;else{*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,
struct Cyc_Position_Segment*v))Cyc_Dict_insert)(*set,place,loc);}}return 0;}struct
_dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
void*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_CfFlowInfo_Place _tmp1F=*place;
_tmp20=_tmp1F.root;_tmp21=_tmp1F.fields;{void*_tmp22=_tmp20;struct Cyc_Absyn_Vardecl*
_tmp24;_LL1: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp23=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp22;if(_tmp23->tag != 0)goto _LL3;else{_tmp24=_tmp23->f1;}}_LL2: if(_tmp21 == 0)
return Cyc_Absynpp_qvar2string(_tmp24->name);else{const char*_tmp4DC;void*_tmp4DB[
1];struct Cyc_String_pa_struct _tmp4DA;return(struct _dyneither_ptr)((_tmp4DA.tag=0,((
_tmp4DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp24->name)),((_tmp4DB[0]=& _tmp4DA,Cyc_aprintf(((_tmp4DC="reachable from %s",
_tag_dyneither(_tmp4DC,sizeof(char),18))),_tag_dyneither(_tmp4DB,sizeof(void*),1))))))));}
_LL3:;_LL4: {const char*_tmp4DF;void*_tmp4DE;(_tmp4DE=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4DF="error locations not for VarRoots",
_tag_dyneither(_tmp4DF,sizeof(char),33))),_tag_dyneither(_tmp4DE,sizeof(void*),0)));}
_LL0:;};}static struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(void*
env,void*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*loc2){
return loc1;}struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2,int disjoint){int _tmp2A=0;struct Cyc_Dict_Dict _tmp2B=((
struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*f)(int*,struct Cyc_CfFlowInfo_Place*,
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*),int*env,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(
int*env,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc1,struct
Cyc_Position_Segment*loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp2A,s1,s2);
return _tmp2B;}struct _tuple14{void*f1;struct Cyc_Position_Segment*f2;};int Cyc_CfFlowInfo_place_set_subset(
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
return n;n=n + 1;}{struct Cyc_Core_Impossible_struct _tmp4EC;const char*_tmp4EB;void*
_tmp4EA[1];struct Cyc_String_pa_struct _tmp4E9;struct Cyc_Core_Impossible_struct*
_tmp4E8;(int)_throw((void*)((_tmp4E8=_cycalloc(sizeof(*_tmp4E8)),((_tmp4E8[0]=((
_tmp4EC.tag=Cyc_Core_Impossible,((_tmp4EC.f1=(struct _dyneither_ptr)((_tmp4E9.tag=
0,((_tmp4E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp4EA[0]=&
_tmp4E9,Cyc_aprintf(((_tmp4EB="get_field_index_fs failed: %s",_tag_dyneither(
_tmp4EB,sizeof(char),30))),_tag_dyneither(_tmp4EA,sizeof(void*),1)))))))),
_tmp4EC)))),_tmp4E8)))));};}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f){void*_tmp3C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp3E;union Cyc_Absyn_AggrInfoU _tmp3F;struct Cyc_List_List*_tmp41;_LLB: {struct
Cyc_Absyn_AggrType_struct*_tmp3D=(struct Cyc_Absyn_AggrType_struct*)_tmp3C;if(
_tmp3D->tag != 11)goto _LLD;else{_tmp3E=_tmp3D->f1;_tmp3F=_tmp3E.aggr_info;}}_LLC: {
struct Cyc_List_List*_tmp42=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(
_tmp3F))->impl))->fields;_tmp41=_tmp42;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp40=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp3C;if(_tmp40->tag != 12)goto _LLF;
else{_tmp41=_tmp40->f2;}}_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp41,f);
_LLF:;_LL10: {struct Cyc_Core_Impossible_struct _tmp4F9;const char*_tmp4F8;void*
_tmp4F7[1];struct Cyc_String_pa_struct _tmp4F6;struct Cyc_Core_Impossible_struct*
_tmp4F5;(int)_throw((void*)((_tmp4F5=_cycalloc(sizeof(*_tmp4F5)),((_tmp4F5[0]=((
_tmp4F9.tag=Cyc_Core_Impossible,((_tmp4F9.f1=(struct _dyneither_ptr)((_tmp4F6.tag=
0,((_tmp4F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp4F7[0]=& _tmp4F6,Cyc_aprintf(((_tmp4F8="get_field_index failed: %s",
_tag_dyneither(_tmp4F8,sizeof(char),27))),_tag_dyneither(_tmp4F7,sizeof(void*),1)))))))),
_tmp4F9)))),_tmp4F5)))));}_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((
int)r1 == (int)r2)return 0;{struct _tuple0 _tmp4FA;struct _tuple0 _tmp49=(_tmp4FA.f1=
r1,((_tmp4FA.f2=r2,_tmp4FA)));void*_tmp4A;struct Cyc_Absyn_Vardecl*_tmp4C;void*
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
else{_tmp6F=_tmp6E->f1;}}_LL22:{struct _dyneither_ptr*_tmp507;const char*_tmp506;
void*_tmp505[1];struct Cyc_String_pa_struct _tmp504;struct Cyc_List_List*_tmp503;sl=((
_tmp503=_cycalloc(sizeof(*_tmp503)),((_tmp503->hd=((_tmp507=_cycalloc(sizeof(*
_tmp507)),((_tmp507[0]=((_tmp504.tag=0,((_tmp504.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp6F->name).f2),((_tmp505[0]=& _tmp504,Cyc_aprintf(((
_tmp506="%s",_tag_dyneither(_tmp506,sizeof(char),3))),_tag_dyneither(_tmp505,
sizeof(void*),1)))))))),_tmp507)))),((_tmp503->tl=sl,_tmp503))))));}goto _LL20;
_LL23: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp70=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp6D;if(_tmp70->tag != 1)goto _LL25;else{_tmp71=_tmp70->f1;}}_LL24:{struct
_dyneither_ptr*_tmp514;const char*_tmp513;void*_tmp512[1];struct Cyc_Int_pa_struct
_tmp511;struct Cyc_List_List*_tmp510;sl=((_tmp510=_cycalloc(sizeof(*_tmp510)),((
_tmp510->hd=((_tmp514=_cycalloc(sizeof(*_tmp514)),((_tmp514[0]=((_tmp511.tag=1,((
_tmp511.f1=(unsigned long)((int)_tmp71),((_tmp512[0]=& _tmp511,Cyc_aprintf(((
_tmp513="mpt%d",_tag_dyneither(_tmp513,sizeof(char),6))),_tag_dyneither(_tmp512,
sizeof(void*),1)))))))),_tmp514)))),((_tmp510->tl=sl,_tmp510))))));}goto _LL20;
_LL25: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp72=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp6D;if(_tmp72->tag != 2)goto _LL20;else{_tmp73=_tmp72->f1;}}_LL26:{struct
_dyneither_ptr*_tmp521;const char*_tmp520;void*_tmp51F[1];struct Cyc_Int_pa_struct
_tmp51E;struct Cyc_List_List*_tmp51D;sl=((_tmp51D=_cycalloc(sizeof(*_tmp51D)),((
_tmp51D->hd=((_tmp521=_cycalloc(sizeof(*_tmp521)),((_tmp521[0]=((_tmp51E.tag=1,((
_tmp51E.f1=(unsigned long)_tmp73,((_tmp51F[0]=& _tmp51E,Cyc_aprintf(((_tmp520="param%d",
_tag_dyneither(_tmp520,sizeof(char),8))),_tag_dyneither(_tmp51F,sizeof(void*),1)))))))),
_tmp521)))),((_tmp51D->tl=sl,_tmp51D))))));}goto _LL20;_LL20:;}for(0;_tmp6C != 0;
_tmp6C=_tmp6C->tl){struct _dyneither_ptr*_tmp52E;const char*_tmp52D;void*_tmp52C[1];
struct Cyc_Int_pa_struct _tmp52B;struct Cyc_List_List*_tmp52A;sl=((_tmp52A=
_cycalloc(sizeof(*_tmp52A)),((_tmp52A->hd=((_tmp52E=_cycalloc(sizeof(*_tmp52E)),((
_tmp52E[0]=((_tmp52B.tag=1,((_tmp52B.f1=(unsigned long)((int)_tmp6C->hd),((
_tmp52C[0]=& _tmp52B,Cyc_aprintf(((_tmp52D="%d",_tag_dyneither(_tmp52D,sizeof(
char),3))),_tag_dyneither(_tmp52C,sizeof(void*),1)))))))),_tmp52E)))),((_tmp52A->tl=
sl,_tmp52A))))));}{struct Cyc_String_pa_struct _tmp539;const char*_tmp538;void*
_tmp537[1];const char*_tmp536;struct _dyneither_ptr*_tmp535;struct _dyneither_ptr*
_tmp88=(_tmp535=_cycalloc(sizeof(*_tmp535)),((_tmp535[0]=((_tmp539.tag=0,((
_tmp539.f1=(struct _dyneither_ptr)((_tmp538="",_tag_dyneither(_tmp538,sizeof(char),
1))),((_tmp537[0]=& _tmp539,Cyc_aprintf(((_tmp536="%s",_tag_dyneither(_tmp536,
sizeof(char),3))),_tag_dyneither(_tmp537,sizeof(void*),1)))))))),_tmp535)));for(
0;sl != 0;sl=sl->tl){const char*_tmp53E;void*_tmp53D[2];struct Cyc_String_pa_struct
_tmp53C;struct Cyc_String_pa_struct _tmp53B;*_tmp88=((_tmp53B.tag=0,((_tmp53B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp88),((_tmp53C.tag=0,((_tmp53C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((
_tmp53D[0]=& _tmp53C,((_tmp53D[1]=& _tmp53B,Cyc_aprintf(((_tmp53E="%s.%s",
_tag_dyneither(_tmp53E,sizeof(char),6))),_tag_dyneither(_tmp53D,sizeof(void*),2))))))))))))));}
return _tmp88;};}static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct
_dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp544(
struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp543,unsigned int*_tmp542,
void***_tmp540){for(*_tmp543=0;*_tmp543 < *_tmp542;(*_tmp543)++){(*_tmp540)[*
_tmp543]=(*fenv)->unknown_all;}}struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,
void*leafval){unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
fs);unsigned int _tmp543;unsigned int _tmp542;struct _dyneither_ptr _tmp541;void**
_tmp540;unsigned int _tmp53F;struct _dyneither_ptr d=(_tmp53F=sz,((_tmp540=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp53F)),((_tmp541=
_tag_dyneither(_tmp540,sizeof(void*),_tmp53F),((((_tmp542=_tmp53F,_tmp544(& fenv,&
_tmp543,& _tmp542,& _tmp540))),_tmp541)))))));{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
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
_tmp9C,_tmp9D,f->type);struct Cyc_Absyn_Aggrfield*_tmp545;return(_tmp545=
_region_malloc(_tmp9C,sizeof(*_tmp545)),((_tmp545->name=f->name,((_tmp545->tq=f->tq,((
_tmp545->type=_tmp9E,((_tmp545->width=f->width,((_tmp545->attributes=f->attributes,
_tmp545)))))))))));};}static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*
targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){struct
_RegionHandle _tmpA0=_new_region("temp");struct _RegionHandle*temp=& _tmpA0;
_push_region(temp);{struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,tvs,targs);struct _tuple15 _tmp546;struct _tuple15
_tmpA1=(_tmp546.f1=temp,((_tmp546.f2=inst,_tmp546)));struct Cyc_List_List*subs_fs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(
struct _tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmpA1,fs);struct
_dyneither_ptr _tmpA2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,
no_init_bits_only,leafval);_npop_handler(0);return _tmpA2;};_pop_region(temp);}
struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,
void*leafval);static void _tmp54C(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*
_tmp54B,unsigned int*_tmp54A,void***_tmp548){for(*_tmp54B=0;*_tmp54B < *_tmp54A;(*
_tmp54B)++){(*_tmp548)[*_tmp54B]=(*fenv)->unknown_all;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
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
_tmpAB);unsigned int _tmp54B;unsigned int _tmp54A;struct _dyneither_ptr _tmp549;void**
_tmp548;unsigned int _tmp547;struct _dyneither_ptr d=(_tmp547=sz,((_tmp548=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp547)),((_tmp549=
_tag_dyneither(_tmp548,sizeof(void*),_tmp547),((((_tmp54A=_tmp547,_tmp54C(& fenv,&
_tmp54B,& _tmp54A,& _tmp548))),_tmp549)))))));{int i=0;for(0;i < sz;++ i){*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,no_init_bits_only,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(
_tmpAB))->hd)).f2,leafval);_tmpAB=_tmpAB->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp552;struct Cyc_CfFlowInfo_UnionRInfo _tmp551;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp550;return(void*)((_tmp550=_region_malloc(fenv->r,sizeof(*_tmp550)),((
_tmp550[0]=((_tmp552.tag=6,((_tmp552.f1=((_tmp551.is_union=0,((_tmp551.fieldnum=
- 1,_tmp551)))),((_tmp552.f2=d,_tmp552)))))),_tmp550))));};}_LL2C: {struct Cyc_Absyn_AggrType_struct*
_tmpAC=(struct Cyc_Absyn_AggrType_struct*)_tmpA4;if(_tmpAC->tag != 11)goto _LL2E;
else{_tmpAD=_tmpAC->f1;_tmpAE=_tmpAD.aggr_info;_tmpAF=_tmpAD.targs;}}_LL2D: {
struct Cyc_Absyn_Aggrdecl*_tmpC4=Cyc_Absyn_get_known_aggrdecl(_tmpAE);if(_tmpC4->impl
== 0)goto _LL27;{struct Cyc_List_List*_tmpC5=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpC4->impl))->fields;if(_tmpAF == 0){_tmpB1=_tmpC4->kind;_tmpB2=
_tmpC5;goto _LL2F;}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp558;struct Cyc_CfFlowInfo_UnionRInfo
_tmp557;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp556;return(void*)((_tmp556=
_region_malloc(fenv->r,sizeof(*_tmp556)),((_tmp556[0]=((_tmp558.tag=6,((_tmp558.f1=((
_tmp557.is_union=_tmpC4->kind == Cyc_Absyn_UnionA,((_tmp557.fieldnum=- 1,_tmp557)))),((
_tmp558.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpC4->tvs,
_tmpAF,_tmpC5,_tmpC4->kind == Cyc_Absyn_UnionA,leafval),_tmp558)))))),_tmp556))));};};}
_LL2E: {struct Cyc_Absyn_AnonAggrType_struct*_tmpB0=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA4;if(_tmpB0->tag != 12)goto _LL30;else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB0->f2;}}
_LL2F: {struct Cyc_CfFlowInfo_Aggregate_struct _tmp55E;struct Cyc_CfFlowInfo_UnionRInfo
_tmp55D;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp55C;return(void*)((_tmp55C=
_region_malloc(fenv->r,sizeof(*_tmp55C)),((_tmp55C[0]=((_tmp55E.tag=6,((_tmp55E.f1=((
_tmp55D.is_union=_tmpB1 == Cyc_Absyn_UnionA,((_tmp55D.fieldnum=- 1,_tmp55D)))),((
_tmp55E.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpB2,_tmpB1 == Cyc_Absyn_UnionA,
leafval),_tmp55E)))))),_tmp55C))));}_LL30:{struct Cyc_Absyn_ArrayType_struct*
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
fenv,void*r);static void _tmp564(struct _dyneither_ptr*_tmpE3,unsigned int*_tmp563,
unsigned int*_tmp562,void***_tmp560){for(*_tmp563=0;*_tmp563 < *_tmp562;(*_tmp563)
++){(*_tmp560)[*_tmp563]=*((void**)_check_dyneither_subscript(*_tmpE3,sizeof(
void*),(int)*_tmp563));}}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*
fenv,void*r){void*_tmpDC=r;struct Cyc_Absyn_Exp*_tmpDE;int _tmpDF;void*_tmpE0;
struct Cyc_CfFlowInfo_UnionRInfo _tmpE2;struct _dyneither_ptr _tmpE3;_LL47: {struct
Cyc_CfFlowInfo_Consumed_struct*_tmpDD=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpDC;if(_tmpDD->tag != 7)goto _LL49;else{_tmpDE=_tmpDD->f1;_tmpDF=_tmpDD->f2;
_tmpE0=(void*)_tmpDD->f3;}}_LL48: return _tmpE0;_LL49: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpE1=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpDC;if(_tmpE1->tag != 6)goto
_LL4B;else{_tmpE2=_tmpE1->f1;_tmpE3=_tmpE1->f2;}}_LL4A: {unsigned int _tmpE4=
_get_dyneither_size(_tmpE3,sizeof(void*));int change=0;unsigned int _tmp563;
unsigned int _tmp562;struct _dyneither_ptr _tmp561;void**_tmp560;unsigned int
_tmp55F;struct _dyneither_ptr d2=(_tmp55F=_tmpE4,((_tmp560=(void**)_region_malloc(
fenv->r,_check_times(sizeof(void*),_tmp55F)),((_tmp561=_tag_dyneither(_tmp560,
sizeof(void*),_tmp55F),((((_tmp562=_tmp55F,_tmp564(& _tmpE3,& _tmp563,& _tmp562,&
_tmp560))),_tmp561)))))));{int i=0;for(0;i < _tmpE4;++ i){*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(
fenv,*((void**)_check_dyneither_subscript(_tmpE3,sizeof(void*),i)));if(*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmpE3,sizeof(void*),i)))change=1;}}if(change){struct
Cyc_CfFlowInfo_Aggregate_struct _tmp567;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp566;return(void*)((_tmp566=_region_malloc(fenv->r,sizeof(*_tmp566)),((
_tmp566[0]=((_tmp567.tag=6,((_tmp567.f1=_tmpE2,((_tmp567.f2=d2,_tmp567)))))),
_tmp566))));}else{return r;}}_LL4B:;_LL4C: return r;_LL46:;}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r);static void _tmp571(struct Cyc_CfFlowInfo_FlowEnv**fenv,
unsigned int*_tmp570,unsigned int*_tmp56F,void***_tmp56D){for(*_tmp570=0;*
_tmp570 < *_tmp56F;(*_tmp570)++){(*_tmp56D)[*_tmp570]=(*fenv)->unknown_all;}}
static void _tmp57A(unsigned int*_tmp579,unsigned int*_tmp578,void***_tmp576,
struct _dyneither_ptr*_tmp105){for(*_tmp579=0;*_tmp579 < *_tmp578;(*_tmp579)++){(*
_tmp576)[*_tmp579]=((void**)(*_tmp105).curr)[(int)*_tmp579];}}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r){if(Cyc_Tcutil_is_noalias_pointer(t)){struct Cyc_CfFlowInfo_Consumed_struct
_tmp56A;struct Cyc_CfFlowInfo_Consumed_struct*_tmp569;return(void*)((_tmp569=
_region_malloc(fenv->r,sizeof(*_tmp569)),((_tmp569[0]=((_tmp56A.tag=7,((_tmp56A.f1=
consumer,((_tmp56A.f2=iteration,((_tmp56A.f3=(void*)r,_tmp56A)))))))),_tmp569))));}{
struct _tuple0 _tmp56B;struct _tuple0 _tmpEE=(_tmp56B.f1=Cyc_Tcutil_compress(t),((
_tmp56B.f2=r,_tmp56B)));void*_tmpEF;struct Cyc_List_List*_tmpF1;void*_tmpF2;
struct Cyc_CfFlowInfo_UnionRInfo _tmpF4;struct _dyneither_ptr _tmpF5;void*_tmpF6;
struct Cyc_Absyn_AggrInfo _tmpF8;union Cyc_Absyn_AggrInfoU _tmpF9;void*_tmpFA;struct
Cyc_CfFlowInfo_UnionRInfo _tmpFC;struct _dyneither_ptr _tmpFD;void*_tmpFE;enum Cyc_Absyn_AggrKind
_tmp100;struct Cyc_List_List*_tmp101;void*_tmp102;struct Cyc_CfFlowInfo_UnionRInfo
_tmp104;struct _dyneither_ptr _tmp105;_LL4E: _tmpEF=_tmpEE.f1;{struct Cyc_Absyn_TupleType_struct*
_tmpF0=(struct Cyc_Absyn_TupleType_struct*)_tmpEF;if(_tmpF0->tag != 10)goto _LL50;
else{_tmpF1=_tmpF0->f1;}};_tmpF2=_tmpEE.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpF3=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpF2;if(_tmpF3->tag != 6)goto
_LL50;else{_tmpF4=_tmpF3->f1;_tmpF5=_tmpF3->f2;}};_LL4F: {unsigned int _tmp106=
_get_dyneither_size(_tmpF5,sizeof(void*));unsigned int _tmp570;unsigned int
_tmp56F;struct _dyneither_ptr _tmp56E;void**_tmp56D;unsigned int _tmp56C;struct
_dyneither_ptr d2=(_tmp56C=_tmp106,((_tmp56D=(void**)_region_malloc(fenv->r,
_check_times(sizeof(void*),_tmp56C)),((_tmp56E=_tag_dyneither(_tmp56D,sizeof(
void*),_tmp56C),((((_tmp56F=_tmp56C,_tmp571(& fenv,& _tmp570,& _tmp56F,& _tmp56D))),
_tmp56E)))))));{int i=0;for(0;i < _tmp106;++ i){*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmpF1))->hd)).f2,
consumer,iteration,*((void**)_check_dyneither_subscript(_tmpF5,sizeof(void*),i)));
_tmpF1=_tmpF1->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp574;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp573;return(void*)((_tmp573=_region_malloc(fenv->r,sizeof(*_tmp573)),((
_tmp573[0]=((_tmp574.tag=6,((_tmp574.f1=_tmpF4,((_tmp574.f2=d2,_tmp574)))))),
_tmp573))));};}_LL50: _tmpF6=_tmpEE.f1;{struct Cyc_Absyn_AggrType_struct*_tmpF7=(
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
_LL53: {unsigned int _tmp579;unsigned int _tmp578;struct _dyneither_ptr _tmp577;void**
_tmp576;unsigned int _tmp575;struct _dyneither_ptr d2=(_tmp575=_get_dyneither_size(
_tmp105,sizeof(void*)),((_tmp576=(void**)_region_malloc(fenv->r,_check_times(
sizeof(void*),_tmp575)),((_tmp577=_tag_dyneither(_tmp576,sizeof(void*),_tmp575),((((
_tmp578=_tmp575,_tmp57A(& _tmp579,& _tmp578,& _tmp576,& _tmp105))),_tmp577)))))));
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp101);{int i=0;for(0;i < sz;(i ++,_tmp101=_tmp101->tl)){struct Cyc_Absyn_Aggrfield
_tmp10F;struct _dyneither_ptr*_tmp110;void*_tmp111;struct Cyc_Absyn_Aggrfield*
_tmp10E=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp101))->hd;
_tmp10F=*_tmp10E;_tmp110=_tmp10F.name;_tmp111=_tmp10F.type;if(
_get_dyneither_size(*_tmp110,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,_tmp111,consumer,iteration,*((void**)_check_dyneither_subscript(_tmp105,
sizeof(void*),i)));}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp57D;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp57C;return(void*)((_tmp57C=_region_malloc(fenv->r,sizeof(*_tmp57C)),((
_tmp57C[0]=((_tmp57D.tag=6,((_tmp57D.f1=_tmp104,((_tmp57D.f2=d2,_tmp57D)))))),
_tmp57C))));};}_LL54:;_LL55: return r;_LL4D:;};}struct _tuple17{struct Cyc_CfFlowInfo_Place*
f1;struct Cyc_Position_Segment*f2;};static int Cyc_CfFlowInfo_prefix_of_member(
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){struct _RegionHandle
_tmp118=_new_region("r");struct _RegionHandle*r=& _tmp118;_push_region(r);{struct
_tuple17 _tmp57E;struct _tuple17 _tmp119=(_tmp57E.f1=place,((_tmp57E.f2=0,_tmp57E)));
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct
Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple17*))Cyc_Iter_next)(iter,& _tmp119)){struct Cyc_CfFlowInfo_Place*
_tmp11A=_tmp119.f1;void*_tmp11C;struct Cyc_List_List*_tmp11D;struct Cyc_CfFlowInfo_Place
_tmp11B=*place;_tmp11C=_tmp11B.root;_tmp11D=_tmp11B.fields;{void*_tmp11F;struct
Cyc_List_List*_tmp120;struct Cyc_CfFlowInfo_Place _tmp11E=*_tmp11A;_tmp11F=_tmp11E.root;
_tmp120=_tmp11E.fields;if(Cyc_CfFlowInfo_root_cmp(_tmp11C,_tmp11F)!= 0)continue;
for(0;_tmp11D != 0  && _tmp120 != 0;(_tmp11D=_tmp11D->tl,_tmp120=_tmp120->tl)){if((
int)_tmp11D->hd != (int)_tmp120->hd)break;}if(_tmp11D == 0){int _tmp121=1;
_npop_handler(0);return _tmp121;}};}{int _tmp122=0;_npop_handler(0);return _tmp122;};};
_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*
places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,
struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){struct Cyc_List_List*
_tmp57F;pile->places=((_tmp57F=_region_malloc(pile->rgn,sizeof(*_tmp57F)),((
_tmp57F->hd=place,((_tmp57F->tl=pile->places,_tmp57F))))));}}static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*r){void*_tmp125=r;void*_tmp127;struct Cyc_CfFlowInfo_Place*
_tmp129;struct Cyc_CfFlowInfo_UnionRInfo _tmp12B;struct _dyneither_ptr _tmp12C;_LL57: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmp126=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp125;if(_tmp126->tag != 7)goto _LL59;else{_tmp127=(void*)_tmp126->f3;}}_LL58:
Cyc_CfFlowInfo_add_places(pile,_tmp127);return;_LL59: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp128=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp125;if(_tmp128->tag != 5)goto
_LL5B;else{_tmp129=_tmp128->f1;}}_LL5A: Cyc_CfFlowInfo_add_place(pile,_tmp129);
return;_LL5B: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp12A=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp125;if(_tmp12A->tag != 6)goto _LL5D;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12A->f2;}}
_LL5C:{int i=0;for(0;i < _get_dyneither_size(_tmp12C,sizeof(void*));++ i){Cyc_CfFlowInfo_add_places(
pile,((void**)_tmp12C.curr)[i]);}}return;_LL5D:;_LL5E: return;_LL56:;}static void*
Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*
old_val);static void _tmp585(struct _RegionHandle**r,void**new_val,unsigned int*
_tmp584,unsigned int*_tmp583,void***_tmp581,struct _dyneither_ptr*_tmp131){for(*
_tmp584=0;*_tmp584 < *_tmp583;(*_tmp584)++){(*_tmp581)[*_tmp584]=Cyc_CfFlowInfo_insert_place_inner(*
r,*new_val,((void**)(*_tmp131).curr)[(int)*_tmp584]);}}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmp12D=old_val;struct Cyc_CfFlowInfo_UnionRInfo
_tmp12F;int _tmp130;struct _dyneither_ptr _tmp131;struct Cyc_Absyn_Exp*_tmp133;int
_tmp134;void*_tmp135;_LL60: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp12E=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp12D;if(_tmp12E->tag != 6)goto _LL62;
else{_tmp12F=_tmp12E->f1;_tmp130=_tmp12F.is_union;_tmp131=_tmp12E->f2;}}_LL61: {
unsigned int _tmp584;unsigned int _tmp583;struct _dyneither_ptr _tmp582;void**
_tmp581;unsigned int _tmp580;struct _dyneither_ptr d2=(_tmp580=_get_dyneither_size(
_tmp131,sizeof(void*)),((_tmp581=(void**)_region_malloc(r,_check_times(sizeof(
void*),_tmp580)),((_tmp582=_tag_dyneither(_tmp581,sizeof(void*),_tmp580),((((
_tmp583=_tmp580,_tmp585(& r,& new_val,& _tmp584,& _tmp583,& _tmp581,& _tmp131))),
_tmp582)))))));struct Cyc_CfFlowInfo_Aggregate_struct _tmp58B;struct Cyc_CfFlowInfo_UnionRInfo
_tmp58A;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp589;return(void*)((_tmp589=
_region_malloc(r,sizeof(*_tmp589)),((_tmp589[0]=((_tmp58B.tag=6,((_tmp58B.f1=((
_tmp58A.is_union=_tmp130,((_tmp58A.fieldnum=- 1,_tmp58A)))),((_tmp58B.f2=d2,
_tmp58B)))))),_tmp589))));}_LL62: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp132=(
struct Cyc_CfFlowInfo_Consumed_struct*)_tmp12D;if(_tmp132->tag != 7)goto _LL64;
else{_tmp133=_tmp132->f1;_tmp134=_tmp132->f2;_tmp135=(void*)_tmp132->f3;}}_LL63: {
struct Cyc_CfFlowInfo_Consumed_struct _tmp58E;struct Cyc_CfFlowInfo_Consumed_struct*
_tmp58D;return(void*)((_tmp58D=_region_malloc(r,sizeof(*_tmp58D)),((_tmp58D[0]=((
_tmp58E.tag=7,((_tmp58E.f1=_tmp133,((_tmp58E.f2=_tmp134,((_tmp58E.f3=(void*)Cyc_CfFlowInfo_insert_place_inner(
r,new_val,_tmp135),_tmp58E)))))))),_tmp58D))));}_LL64:;_LL65: return new_val;_LL5F:;}
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,
struct _dyneither_ptr d,int n,void*rval);static void _tmp594(struct _dyneither_ptr*d,
unsigned int*_tmp593,unsigned int*_tmp592,void***_tmp590){for(*_tmp593=0;*
_tmp593 < *_tmp592;(*_tmp593)++){(*_tmp590)[*_tmp593]=((void**)(*d).curr)[(int)*
_tmp593];}}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmp13F=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmp13F == rval)return d;{
unsigned int _tmp593;unsigned int _tmp592;struct _dyneither_ptr _tmp591;void**
_tmp590;unsigned int _tmp58F;struct _dyneither_ptr res=(_tmp58F=_get_dyneither_size(
d,sizeof(void*)),((_tmp590=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp58F)),((_tmp591=_tag_dyneither(_tmp590,sizeof(void*),_tmp58F),((((_tmp592=
_tmp58F,_tmp594(& d,& _tmp593,& _tmp592,& _tmp590))),_tmp591)))))));*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;};}struct
_tuple18{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 
0)return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{struct _tuple18
_tmp595;struct _tuple18 _tmp145=(_tmp595.f1=fs,((_tmp595.f2=old_val,_tmp595)));
struct Cyc_List_List*_tmp146;struct Cyc_List_List _tmp147;int _tmp148;struct Cyc_List_List*
_tmp149;void*_tmp14A;struct Cyc_CfFlowInfo_UnionRInfo _tmp14C;int _tmp14D;struct
_dyneither_ptr _tmp14E;_LL67: _tmp146=_tmp145.f1;if(_tmp146 == 0)goto _LL69;_tmp147=*
_tmp146;_tmp148=(int)_tmp147.hd;_tmp149=_tmp147.tl;_tmp14A=_tmp145.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp14B=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp14A;if(_tmp14B->tag != 6)goto
_LL69;else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14C.is_union;_tmp14E=_tmp14B->f2;}};
_LL68: {void*_tmp14F=Cyc_CfFlowInfo_insert_place_outer(r,_tmp149,*((void**)
_check_dyneither_subscript(_tmp14E,sizeof(void*),_tmp148)),new_val);struct
_dyneither_ptr _tmp150=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp14E,_tmp148,_tmp14F);
if((void**)_tmp150.curr == (void**)_tmp14E.curr)return old_val;else{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp59B;struct Cyc_CfFlowInfo_UnionRInfo _tmp59A;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp599;return(void*)((_tmp599=_region_malloc(r,sizeof(*_tmp599)),((_tmp599[0]=((
_tmp59B.tag=6,((_tmp59B.f1=((_tmp59A.is_union=_tmp14D,((_tmp59A.fieldnum=- 1,
_tmp59A)))),((_tmp59B.f2=_tmp150,_tmp59B)))))),_tmp599))));}}_LL69:;_LL6A: {
struct Cyc_Core_Impossible_struct _tmp5A1;const char*_tmp5A0;struct Cyc_Core_Impossible_struct*
_tmp59F;(int)_throw((void*)((_tmp59F=_cycalloc(sizeof(*_tmp59F)),((_tmp59F[0]=((
_tmp5A1.tag=Cyc_Core_Impossible,((_tmp5A1.f1=((_tmp5A0="bad insert place",
_tag_dyneither(_tmp5A0,sizeof(char),17))),_tmp5A1)))),_tmp59F)))));}_LL66:;};}
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict
d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*_tmp157=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)
_check_null(pile->places))->tl;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,_tmp157,0);{void*oldval;void*newval;{struct _handler_cons _tmp158;
_push_handler(& _tmp158);{int _tmp15A=0;if(setjmp(_tmp158.handler))_tmp15A=1;if(!
_tmp15A){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp157);;_pop_handler();}else{
void*_tmp159=(void*)_exn_thrown;void*_tmp15C=_tmp159;_LL6C: {struct Cyc_Dict_Absent_struct*
_tmp15D=(struct Cyc_Dict_Absent_struct*)_tmp15C;if(_tmp15D->tag != Cyc_Dict_Absent)
goto _LL6E;}_LL6D: continue;_LL6E:;_LL6F:(void)_throw(_tmp15C);_LL6B:;}};}switch(
Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){case Cyc_CfFlowInfo_AllIL: _LL70: newval=
fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL: _LL71: newval=fenv->esc_this;break;
case Cyc_CfFlowInfo_NoneIL: _LL72: newval=fenv->esc_none;break;}Cyc_CfFlowInfo_add_places(
pile,oldval);{void*_tmp15F;struct Cyc_List_List*_tmp160;struct Cyc_CfFlowInfo_Place
_tmp15E=*_tmp157;_tmp15F=_tmp15E.root;_tmp160=_tmp15E.fields;d=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp15F,Cyc_CfFlowInfo_insert_place_outer(
fenv->r,_tmp160,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,
_tmp15F),newval));};};}return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct
_RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};static enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmp161=r;enum Cyc_CfFlowInfo_InitLevel
_tmp163;enum Cyc_CfFlowInfo_InitLevel _tmp165;void*_tmp16A;_LL75: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp162=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp161;if(_tmp162->tag != 3)goto
_LL77;else{_tmp163=_tmp162->f1;}}_LL76: return _tmp163;_LL77: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp164=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp161;if(_tmp164->tag != 4)goto _LL79;
else{_tmp165=_tmp164->f1;}}_LL78: return _tmp165;_LL79: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp166=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp161;if(_tmp166->tag != 0)goto _LL7B;}
_LL7A: goto _LL7C;_LL7B: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp167=(struct
Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp161;if(_tmp167->tag != 1)goto _LL7D;}_LL7C:
return Cyc_CfFlowInfo_AllIL;_LL7D: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp168=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp161;if(_tmp168->tag != 2)
goto _LL7F;}_LL7E: return Cyc_CfFlowInfo_ThisIL;_LL7F: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp169=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp161;if(_tmp169->tag != 7)goto
_LL81;else{_tmp16A=(void*)_tmp169->f3;}}_LL80: return Cyc_CfFlowInfo_NoneIL;_LL81:;
_LL82: {struct Cyc_Core_Impossible_struct _tmp5A7;const char*_tmp5A6;struct Cyc_Core_Impossible_struct*
_tmp5A5;(int)_throw((void*)((_tmp5A5=_cycalloc(sizeof(*_tmp5A5)),((_tmp5A5[0]=((
_tmp5A7.tag=Cyc_Core_Impossible,((_tmp5A7.f1=((_tmp5A6="initlevel_approx",
_tag_dyneither(_tmp5A6,sizeof(char),17))),_tmp5A7)))),_tmp5A5)))));}_LL74:;}
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){enum Cyc_CfFlowInfo_InitLevel
this_ans;if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;{void*
_tmp16E=r;struct Cyc_CfFlowInfo_UnionRInfo _tmp170;int _tmp171;int _tmp172;struct
_dyneither_ptr _tmp173;struct Cyc_CfFlowInfo_UnionRInfo _tmp175;int _tmp176;struct
_dyneither_ptr _tmp177;struct Cyc_CfFlowInfo_Place*_tmp179;_LL84: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp16F=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp16E;if(_tmp16F->tag != 6)goto
_LL86;else{_tmp170=_tmp16F->f1;_tmp171=_tmp170.is_union;if(_tmp171 != 1)goto _LL86;
_tmp172=_tmp170.fieldnum;_tmp173=_tmp16F->f2;}}_LL85: {unsigned int _tmp17A=
_get_dyneither_size(_tmp173,sizeof(void*));this_ans=Cyc_CfFlowInfo_NoneIL;if(
_tmp172 == - 1){int i=0;for(0;i < _tmp17A;++ i){if(Cyc_CfFlowInfo_initlevel_rec(env,*((
void**)_check_dyneither_subscript(_tmp173,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL){this_ans=Cyc_CfFlowInfo_AllIL;break;}}}else{if(Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp173,sizeof(void*),_tmp172)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL)this_ans=Cyc_CfFlowInfo_AllIL;}goto _LL83;}_LL86: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp174=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp16E;if(_tmp174->tag != 6)goto _LL88;else{_tmp175=_tmp174->f1;_tmp176=_tmp175.is_union;
if(_tmp176 != 0)goto _LL88;_tmp177=_tmp174->f2;}}_LL87: {unsigned int _tmp17B=
_get_dyneither_size(_tmp177,sizeof(void*));this_ans=Cyc_CfFlowInfo_AllIL;{int i=0;
for(0;i < _tmp17B;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)
_check_dyneither_subscript(_tmp177,sizeof(void*),i)),this_ans);}}goto _LL83;}
_LL88: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp178=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp16E;if(_tmp178->tag != 5)goto _LL8A;else{_tmp179=_tmp178->f1;}}_LL89: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp179))this_ans=Cyc_CfFlowInfo_AllIL;else{{struct Cyc_List_List*_tmp5A8;env->seen=((
_tmp5A8=_region_malloc(env->e,sizeof(*_tmp5A8)),((_tmp5A8->hd=_tmp179,((_tmp5A8->tl=
env->seen,_tmp5A8))))));}this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(
env->d,_tmp179),Cyc_CfFlowInfo_AllIL);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == Cyc_CfFlowInfo_NoneIL)this_ans=Cyc_CfFlowInfo_ThisIL;}
goto _LL83;_LL8A:;_LL8B: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL83:;}if(
this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;if(this_ans == Cyc_CfFlowInfo_ThisIL
 || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;return Cyc_CfFlowInfo_AllIL;}
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*
env,struct Cyc_Dict_Dict d,void*r){struct _RegionHandle*_tmp17D=env->r;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp5A9;struct Cyc_CfFlowInfo_InitlevelEnv _tmp17E=(_tmp5A9.e=_tmp17D,((_tmp5A9.d=
d,((_tmp5A9.seen=0,_tmp5A9)))));return Cyc_CfFlowInfo_initlevel_rec(& _tmp17E,r,
Cyc_CfFlowInfo_AllIL);}struct _tuple19{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){void*_tmp181;struct Cyc_List_List*
_tmp182;struct Cyc_CfFlowInfo_Place _tmp180=*place;_tmp181=_tmp180.root;_tmp182=
_tmp180.fields;{void*_tmp183=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,_tmp181);for(0;_tmp182 != 0;_tmp182=_tmp182->tl){struct _tuple19 _tmp5AA;struct
_tuple19 _tmp185=(_tmp5AA.f1=_tmp183,((_tmp5AA.f2=(int)_tmp182->hd,_tmp5AA)));
void*_tmp186;struct Cyc_CfFlowInfo_UnionRInfo _tmp188;struct _dyneither_ptr _tmp189;
int _tmp18A;_LL8D: _tmp186=_tmp185.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp187=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp186;if(_tmp187->tag != 6)goto
_LL8F;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;}};_tmp18A=_tmp185.f2;_LL8E:
_tmp183=*((void**)_check_dyneither_subscript(_tmp189,sizeof(void*),_tmp18A));
goto _LL8C;_LL8F:;_LL90: {struct Cyc_Core_Impossible_struct _tmp5B0;const char*
_tmp5AF;struct Cyc_Core_Impossible_struct*_tmp5AE;(int)_throw((void*)((_tmp5AE=
_cycalloc(sizeof(*_tmp5AE)),((_tmp5AE[0]=((_tmp5B0.tag=Cyc_Core_Impossible,((
_tmp5B0.f1=((_tmp5AF="bad lookup_place",_tag_dyneither(_tmp5AF,sizeof(char),17))),
_tmp5B0)))),_tmp5AE)))));}_LL8C:;}return _tmp183;};}static int Cyc_CfFlowInfo_is_rval_unescaped(
void*rval){void*_tmp18E=rval;void*_tmp191;struct Cyc_CfFlowInfo_UnionRInfo _tmp193;
int _tmp194;int _tmp195;struct _dyneither_ptr _tmp196;_LL92: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp18F=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp18E;if(_tmp18F->tag != 4)goto _LL94;}
_LL93: return 0;_LL94: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp190=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp18E;if(_tmp190->tag != 7)goto _LL96;else{_tmp191=(void*)_tmp190->f3;}}_LL95:
return Cyc_CfFlowInfo_is_rval_unescaped(rval);_LL96: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp192=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp18E;if(_tmp192->tag != 6)goto
_LL98;else{_tmp193=_tmp192->f1;_tmp194=_tmp193.is_union;_tmp195=_tmp193.fieldnum;
_tmp196=_tmp192->f2;}}_LL97: if(_tmp194  && _tmp195 != - 1)return !Cyc_CfFlowInfo_is_rval_unescaped(*((
void**)_check_dyneither_subscript(_tmp196,sizeof(void*),_tmp195)));else{
unsigned int sz=_get_dyneither_size(_tmp196,sizeof(void*));{int i=0;for(0;i < sz;++
i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp196.curr)[i]))return 0;}}
return 1;}_LL98:;_LL99: return 1;_LL91:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(
d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct
_RegionHandle*_tmp197=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp5B1;struct Cyc_CfFlowInfo_EscPile*
pile=(_tmp5B1=_region_malloc(_tmp197,sizeof(*_tmp5B1)),((_tmp5B1->rgn=_tmp197,((
_tmp5B1->places=0,_tmp5B1)))));Cyc_CfFlowInfo_add_places(pile,r);return Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;struct Cyc_Position_Segment*
loc;struct Cyc_CfFlowInfo_Place*root_place;struct Cyc_Dict_Dict*all_changed;};
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,
void*oldval,void*newval);static void _tmp5BB(struct Cyc_CfFlowInfo_AssignEnv**env,
unsigned int*_tmp5BA,unsigned int*_tmp5B9,void***_tmp5B7,struct _dyneither_ptr*
_tmp1A9,struct _dyneither_ptr*_tmp1A5){for(*_tmp5BA=0;*_tmp5BA < *_tmp5B9;(*
_tmp5BA)++){(*_tmp5B7)[*_tmp5BA]=Cyc_CfFlowInfo_assign_place_inner(*env,((void**)(*
_tmp1A5).curr)[(int)*_tmp5BA],*((void**)_check_dyneither_subscript(*_tmp1A9,
sizeof(void*),(int)*_tmp5BA)));}}static void*Cyc_CfFlowInfo_assign_place_inner(
struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){struct _tuple0 _tmp5B2;
struct _tuple0 _tmp19A=(_tmp5B2.f1=oldval,((_tmp5B2.f2=newval,_tmp5B2)));void*
_tmp19B;void*_tmp19D;struct Cyc_CfFlowInfo_Place*_tmp19F;void*_tmp1A0;void*
_tmp1A2;struct Cyc_CfFlowInfo_UnionRInfo _tmp1A4;struct _dyneither_ptr _tmp1A5;void*
_tmp1A6;struct Cyc_CfFlowInfo_UnionRInfo _tmp1A8;struct _dyneither_ptr _tmp1A9;void*
_tmp1AA;enum Cyc_CfFlowInfo_InitLevel _tmp1AC;_LL9B: _tmp19B=_tmp19A.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp19C=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp19B;if(_tmp19C->tag != 4)goto _LL9D;};
_tmp19D=_tmp19A.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp19E=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp19D;if(_tmp19E->tag != 5)goto _LL9D;else{_tmp19F=_tmp19E->f1;}};_LL9C: Cyc_CfFlowInfo_add_place(
env->pile,_tmp19F);goto _LL9E;_LL9D: _tmp1A0=_tmp19A.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp1A1=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp1A0;if(_tmp1A1->tag != 4)goto _LL9F;};
_LL9E: if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp5B5;void*_tmp5B4;(_tmp5B4=0,Cyc_Tcutil_terr(env->loc,((_tmp5B5="assignment puts possibly-uninitialized data in an escaped location",
_tag_dyneither(_tmp5B5,sizeof(char),67))),_tag_dyneither(_tmp5B4,sizeof(void*),0)));}
return(env->fenv)->esc_all;_LL9F: _tmp1A2=_tmp19A.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp1A3=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1A2;if(_tmp1A3->tag != 6)goto
_LLA1;else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A3->f2;}};_tmp1A6=_tmp19A.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp1A7=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1A6;if(_tmp1A7->tag != 6)goto _LLA1;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;}};
_LLA0: {unsigned int _tmp5BA;unsigned int _tmp5B9;struct _dyneither_ptr _tmp5B8;void**
_tmp5B7;unsigned int _tmp5B6;struct _dyneither_ptr new_d=(_tmp5B6=
_get_dyneither_size(_tmp1A5,sizeof(void*)),((_tmp5B7=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp5B6)),((_tmp5B8=_tag_dyneither(_tmp5B7,sizeof(
void*),_tmp5B6),((((_tmp5B9=_tmp5B6,_tmp5BB(& env,& _tmp5BA,& _tmp5B9,& _tmp5B7,&
_tmp1A9,& _tmp1A5))),_tmp5B8)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1A5,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1A5.curr)[i]){
change=1;break;}}}if(!change){if(!_tmp1A4.is_union)return oldval;new_d=_tmp1A5;}
else{change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp1A5,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1A9,sizeof(void*),i))){change=1;break;}}}if(!
change){if(!_tmp1A4.is_union)return newval;new_d=_tmp1A9;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp5BE;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5BD;return(void*)((_tmp5BD=
_region_malloc((env->fenv)->r,sizeof(*_tmp5BD)),((_tmp5BD[0]=((_tmp5BE.tag=6,((
_tmp5BE.f1=_tmp1A8,((_tmp5BE.f2=new_d,_tmp5BE)))))),_tmp5BD))));};}_LLA1: _tmp1AA=
_tmp19A.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp1AB=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp1AA;if(_tmp1AB->tag != 4)goto _LLA3;else{_tmp1AC=_tmp1AB->f1;}};_LLA2: switch(
_tmp1AC){case Cyc_CfFlowInfo_NoneIL: _LLA5: return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL:
_LLA6: return(env->fenv)->unknown_this;case Cyc_CfFlowInfo_AllIL: _LLA7: return(env->fenv)->unknown_all;}
_LLA3:;_LLA4: return newval;_LL9A:;}static int Cyc_CfFlowInfo_nprefix(int*n,void*
unused){if(*n > 0){*n=*n - 1;return 1;}else{return 0;}}static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,int fs_prefix,void*
oldval,void*newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,
newval);{struct _tuple18 _tmp5BF;struct _tuple18 _tmp1B6=(_tmp5BF.f1=fs,((_tmp5BF.f2=
oldval,_tmp5BF)));struct Cyc_List_List*_tmp1B7;struct Cyc_List_List _tmp1B8;int
_tmp1B9;struct Cyc_List_List*_tmp1BA;void*_tmp1BB;struct Cyc_CfFlowInfo_UnionRInfo
_tmp1BD;int _tmp1BE;int _tmp1BF;struct _dyneither_ptr _tmp1C0;_LLAA: _tmp1B7=_tmp1B6.f1;
if(_tmp1B7 == 0)goto _LLAC;_tmp1B8=*_tmp1B7;_tmp1B9=(int)_tmp1B8.hd;_tmp1BA=
_tmp1B8.tl;_tmp1BB=_tmp1B6.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1BC=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1BB;if(_tmp1BC->tag != 6)goto _LLAC;
else{_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BD.is_union;_tmp1BF=_tmp1BD.fieldnum;
_tmp1C0=_tmp1BC->f2;}};_LLAB: {void*_tmp1C1=Cyc_CfFlowInfo_assign_place_outer(
env,_tmp1BA,fs_prefix + 1,*((void**)_check_dyneither_subscript(_tmp1C0,sizeof(
void*),_tmp1B9)),newval);struct _dyneither_ptr _tmp1C2=Cyc_CfFlowInfo_aggr_dict_insert((
env->fenv)->r,_tmp1C0,_tmp1B9,_tmp1C1);if((void**)_tmp1C2.curr == (void**)_tmp1C0.curr
 && (!_tmp1BE  || _tmp1BF == _tmp1B9))return oldval;_tmp1C0=_tmp1C2;if(_tmp1BE){int
changed=0;int sz=(int)_get_dyneither_size(_tmp1C0,sizeof(void*));{int i=0;for(0;i < 
sz;++ i){if(i != _tmp1B9){struct _dyneither_ptr _tmp1C3=Cyc_CfFlowInfo_aggr_dict_insert((
env->fenv)->r,_tmp1C0,i,Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((
void**)_check_dyneither_subscript(_tmp1C0,sizeof(void*),i))));if((void**)_tmp1C3.curr
!= (void**)_tmp1C0.curr){_tmp1C0=_tmp1C3;changed=1;}}}}if(changed){struct Cyc_CfFlowInfo_Place
_tmp1C5;void*_tmp1C6;struct Cyc_List_List*_tmp1C7;struct Cyc_CfFlowInfo_Place*
_tmp1C4=env->root_place;_tmp1C5=*_tmp1C4;_tmp1C6=_tmp1C5.root;_tmp1C7=_tmp1C5.fields;{
int*_tmp5C0;struct Cyc_List_List*_tmp1C8=((struct Cyc_List_List*(*)(struct
_RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)((
env->pile)->rgn,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,((_tmp5C0=
_cycalloc_atomic(sizeof(*_tmp5C0)),((_tmp5C0[0]=fs_prefix,_tmp5C0)))),_tmp1C7);
struct Cyc_CfFlowInfo_Place*_tmp5C1;struct Cyc_CfFlowInfo_Place*_tmp1C9=(_tmp5C1=
_region_malloc((env->pile)->rgn,sizeof(*_tmp5C1)),((_tmp5C1->root=_tmp1C6,((
_tmp5C1->fields=_tmp1C8,_tmp5C1)))));((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
_tmp1C9,0);};}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5C7;struct Cyc_CfFlowInfo_UnionRInfo
_tmp5C6;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5C5;return(void*)((_tmp5C5=
_region_malloc((env->fenv)->r,sizeof(*_tmp5C5)),((_tmp5C5[0]=((_tmp5C7.tag=6,((
_tmp5C7.f1=((_tmp5C6.is_union=_tmp1BE,((_tmp5C6.fieldnum=_tmp1B9,_tmp5C6)))),((
_tmp5C7.f2=_tmp1C0,_tmp5C7)))))),_tmp5C5))));};}_LLAC:;_LLAD: {struct Cyc_Core_Impossible_struct
_tmp5CD;const char*_tmp5CC;struct Cyc_Core_Impossible_struct*_tmp5CB;(int)_throw((
void*)((_tmp5CB=_cycalloc(sizeof(*_tmp5CB)),((_tmp5CB[0]=((_tmp5CD.tag=Cyc_Core_Impossible,((
_tmp5CD.f1=((_tmp5CC="bad assign place",_tag_dyneither(_tmp5CC,sizeof(char),17))),
_tmp5CD)))),_tmp5CB)))));}_LLA9:;};}struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{struct _RegionHandle*
_tmp1D2=fenv->r;struct Cyc_CfFlowInfo_Place _tmp1D4;void*_tmp1D5;struct Cyc_List_List*
_tmp1D6;struct Cyc_CfFlowInfo_Place*_tmp1D3=place;_tmp1D4=*_tmp1D3;_tmp1D5=
_tmp1D4.root;_tmp1D6=_tmp1D4.fields;{struct Cyc_CfFlowInfo_EscPile*_tmp5D0;struct
Cyc_CfFlowInfo_AssignEnv _tmp5CF;struct Cyc_CfFlowInfo_AssignEnv env=(_tmp5CF.fenv=
fenv,((_tmp5CF.pile=((_tmp5D0=_region_malloc(_tmp1D2,sizeof(*_tmp5D0)),((_tmp5D0->rgn=
_tmp1D2,((_tmp5D0->places=0,_tmp5D0)))))),((_tmp5CF.d=d,((_tmp5CF.loc=loc,((
_tmp5CF.root_place=place,((_tmp5CF.all_changed=all_changed,_tmp5CF)))))))))));
void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmp1D6,0,((void*(*)(struct
Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1D5),r);return Cyc_CfFlowInfo_escape_these(
fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,
void*v))Cyc_Dict_insert)(d,_tmp1D5,newval));};};}struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict
d1;struct Cyc_Dict_Dict d2;};enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither
 = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{
struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict
chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);struct _tuple20{enum Cyc_CfFlowInfo_InitLevel f1;
enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct
Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp5E0(struct Cyc_CfFlowInfo_JoinEnv**
env,unsigned int*_tmp5DF,unsigned int*_tmp5DE,void***_tmp5DC,struct
_dyneither_ptr*_tmp223,struct _dyneither_ptr*_tmp21D){for(*_tmp5DF=0;*_tmp5DF < *
_tmp5DE;(*_tmp5DF)++){(*_tmp5DC)[*_tmp5DF]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp21D).curr)[(
int)*_tmp5DF],*((void**)_check_dyneither_subscript(*_tmp223,sizeof(void*),(int)*
_tmp5DF)));}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp1D9=(
env->fenv)->r;{struct _tuple0 _tmp5D1;struct _tuple0 _tmp1DB=(_tmp5D1.f1=r1,((
_tmp5D1.f2=r2,_tmp5D1)));void*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DE;int _tmp1DF;
void*_tmp1E0;void*_tmp1E1;struct Cyc_Absyn_Exp*_tmp1E3;int _tmp1E4;void*_tmp1E5;
void*_tmp1E6;struct Cyc_Absyn_Exp*_tmp1E8;int _tmp1E9;void*_tmp1EA;void*_tmp1EB;
struct Cyc_Absyn_Exp*_tmp1ED;int _tmp1EE;void*_tmp1EF;void*_tmp1F0;struct Cyc_CfFlowInfo_Place*
_tmp1F2;void*_tmp1F3;struct Cyc_CfFlowInfo_Place*_tmp1F5;void*_tmp1F6;struct Cyc_CfFlowInfo_Place*
_tmp1F8;void*_tmp1F9;void*_tmp1FB;void*_tmp1FD;struct Cyc_CfFlowInfo_Place*
_tmp1FF;void*_tmp200;struct Cyc_CfFlowInfo_Place*_tmp202;void*_tmp203;void*
_tmp205;void*_tmp207;struct Cyc_CfFlowInfo_Place*_tmp209;void*_tmp20A;struct Cyc_CfFlowInfo_Place*
_tmp20C;void*_tmp20D;struct Cyc_CfFlowInfo_Place*_tmp20F;void*_tmp210;void*
_tmp212;void*_tmp214;void*_tmp216;void*_tmp218;struct Cyc_CfFlowInfo_UnionRInfo
_tmp21A;int _tmp21B;int _tmp21C;struct _dyneither_ptr _tmp21D;void*_tmp21E;struct Cyc_CfFlowInfo_UnionRInfo
_tmp220;int _tmp221;int _tmp222;struct _dyneither_ptr _tmp223;_LLAF: _tmp1DC=_tmp1DB.f1;{
struct Cyc_CfFlowInfo_Consumed_struct*_tmp1DD=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1DC;if(_tmp1DD->tag != 7)goto _LLB1;else{_tmp1DE=_tmp1DD->f1;_tmp1DF=_tmp1DD->f2;
_tmp1E0=(void*)_tmp1DD->f3;}};_tmp1E1=_tmp1DB.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1E2=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1E1;if(_tmp1E2->tag != 7)goto
_LLB1;else{_tmp1E3=_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;_tmp1E5=(void*)_tmp1E2->f3;}};
_LLB0: if(_tmp1DE == _tmp1E3  && _tmp1DF == _tmp1E4){struct Cyc_CfFlowInfo_Consumed_struct
_tmp5D4;struct Cyc_CfFlowInfo_Consumed_struct*_tmp5D3;return(void*)((_tmp5D3=
_region_malloc(_tmp1D9,sizeof(*_tmp5D3)),((_tmp5D3[0]=((_tmp5D4.tag=7,((_tmp5D4.f1=
_tmp1DE,((_tmp5D4.f2=_tmp1DF,((_tmp5D4.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1E0,_tmp1E5),_tmp5D4)))))))),_tmp5D3))));}{void*_tmp226=_tmp1E0;struct Cyc_CfFlowInfo_Place*
_tmp228;_LLCC: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp227=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp226;if(_tmp227->tag != 5)goto _LLCE;else{_tmp228=_tmp227->f1;}}_LLCD: Cyc_CfFlowInfo_add_place(
env->pile,_tmp228);goto _LLCB;_LLCE:;_LLCF: goto _LLCB;_LLCB:;}{void*_tmp229=
_tmp1E5;struct Cyc_CfFlowInfo_Place*_tmp22B;_LLD1: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp22A=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp229;if(_tmp22A->tag != 5)goto
_LLD3;else{_tmp22B=_tmp22A->f1;}}_LLD2: Cyc_CfFlowInfo_add_place(env->pile,
_tmp22B);goto _LLD0;_LLD3:;_LLD4: goto _LLD0;_LLD0:;}goto _LLAE;_LLB1: _tmp1E6=
_tmp1DB.f1;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp1E7=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1E6;if(_tmp1E7->tag != 7)goto _LLB3;else{_tmp1E8=_tmp1E7->f1;_tmp1E9=_tmp1E7->f2;
_tmp1EA=(void*)_tmp1E7->f3;}};_LLB2: {struct Cyc_CfFlowInfo_Consumed_struct
_tmp5D7;struct Cyc_CfFlowInfo_Consumed_struct*_tmp5D6;return(void*)((_tmp5D6=
_region_malloc(_tmp1D9,sizeof(*_tmp5D6)),((_tmp5D6[0]=((_tmp5D7.tag=7,((_tmp5D7.f1=
_tmp1E8,((_tmp5D7.f2=_tmp1E9,((_tmp5D7.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1EA,r2),_tmp5D7)))))))),_tmp5D6))));}_LLB3: _tmp1EB=_tmp1DB.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1EC=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1EB;if(_tmp1EC->tag != 7)goto
_LLB5;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=_tmp1EC->f2;_tmp1EF=(void*)_tmp1EC->f3;}};
_LLB4: {struct Cyc_CfFlowInfo_Consumed_struct _tmp5DA;struct Cyc_CfFlowInfo_Consumed_struct*
_tmp5D9;return(void*)((_tmp5D9=_region_malloc(_tmp1D9,sizeof(*_tmp5D9)),((
_tmp5D9[0]=((_tmp5DA.tag=7,((_tmp5DA.f1=_tmp1ED,((_tmp5DA.f2=_tmp1EE,((_tmp5DA.f3=(
void*)Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1EF),_tmp5DA)))))))),_tmp5D9))));}
_LLB5: _tmp1F0=_tmp1DB.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1F1=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F0;if(_tmp1F1->tag != 5)goto _LLB7;else{
_tmp1F2=_tmp1F1->f1;}};_tmp1F3=_tmp1DB.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1F4=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F3;if(_tmp1F4->tag != 5)goto
_LLB7;else{_tmp1F5=_tmp1F4->f1;}};_LLB6: if(Cyc_CfFlowInfo_place_cmp(_tmp1F2,
_tmp1F5)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp1F2);Cyc_CfFlowInfo_add_place(
env->pile,_tmp1F5);goto _LLAE;_LLB7: _tmp1F6=_tmp1DB.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1F7=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F6;if(_tmp1F7->tag != 5)goto
_LLB9;else{_tmp1F8=_tmp1F7->f1;}};_tmp1F9=_tmp1DB.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1FA=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1F9;if(_tmp1FA->tag != 2)
goto _LLB9;};_LLB8: _tmp1FF=_tmp1F8;goto _LLBA;_LLB9: _tmp1FB=_tmp1DB.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1FC=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1FB;if(_tmp1FC->tag != 2)
goto _LLBB;};_tmp1FD=_tmp1DB.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1FE=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1FD;if(_tmp1FE->tag != 5)goto _LLBB;
else{_tmp1FF=_tmp1FE->f1;}};_LLBA: Cyc_CfFlowInfo_add_place(env->pile,_tmp1FF);
return(env->fenv)->notzerothis;_LLBB: _tmp200=_tmp1DB.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp201=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp200;if(_tmp201->tag != 5)goto
_LLBD;else{_tmp202=_tmp201->f1;}};_tmp203=_tmp1DB.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp204=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp203;if(_tmp204->tag != 1)
goto _LLBD;};_LLBC: Cyc_CfFlowInfo_add_place(env->pile,_tmp202);switch(Cyc_CfFlowInfo_initlevel(
env->fenv,env->d1,r1)){case Cyc_CfFlowInfo_AllIL: _LLD5: return(env->fenv)->notzeroall;
default: _LLD6: return(env->fenv)->notzerothis;}_LLBD: _tmp205=_tmp1DB.f1;{struct
Cyc_CfFlowInfo_NotZeroAll_struct*_tmp206=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp205;if(_tmp206->tag != 1)goto _LLBF;};_tmp207=_tmp1DB.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp208=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp207;if(_tmp208->tag != 5)goto
_LLBF;else{_tmp209=_tmp208->f1;}};_LLBE: Cyc_CfFlowInfo_add_place(env->pile,
_tmp209);switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2)){case Cyc_CfFlowInfo_AllIL:
_LLD8: return(env->fenv)->notzeroall;default: _LLD9: return(env->fenv)->notzerothis;}
_LLBF: _tmp20A=_tmp1DB.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp20B=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp20A;if(_tmp20B->tag != 5)goto _LLC1;else{
_tmp20C=_tmp20B->f1;}};_LLC0: Cyc_CfFlowInfo_add_place(env->pile,_tmp20C);goto
_LLAE;_LLC1: _tmp20D=_tmp1DB.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp20E=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp20D;if(_tmp20E->tag != 5)goto _LLC3;
else{_tmp20F=_tmp20E->f1;}};_LLC2: Cyc_CfFlowInfo_add_place(env->pile,_tmp20F);
goto _LLAE;_LLC3: _tmp210=_tmp1DB.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp211=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp210;if(_tmp211->tag != 1)
goto _LLC5;};_tmp212=_tmp1DB.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp213=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp212;if(_tmp213->tag != 2)goto _LLC5;};
_LLC4: goto _LLC6;_LLC5: _tmp214=_tmp1DB.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp215=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp214;if(_tmp215->tag != 2)
goto _LLC7;};_tmp216=_tmp1DB.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp217=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp216;if(_tmp217->tag != 1)goto _LLC7;};
_LLC6: return(env->fenv)->notzerothis;_LLC7: _tmp218=_tmp1DB.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp219=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp218;if(_tmp219->tag != 6)goto
_LLC9;else{_tmp21A=_tmp219->f1;_tmp21B=_tmp21A.is_union;_tmp21C=_tmp21A.fieldnum;
_tmp21D=_tmp219->f2;}};_tmp21E=_tmp1DB.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp21F=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp21E;if(_tmp21F->tag != 6)goto
_LLC9;else{_tmp220=_tmp21F->f1;_tmp221=_tmp220.is_union;_tmp222=_tmp220.fieldnum;
_tmp223=_tmp21F->f2;}};_LLC8: {unsigned int _tmp5DF;unsigned int _tmp5DE;struct
_dyneither_ptr _tmp5DD;void**_tmp5DC;unsigned int _tmp5DB;struct _dyneither_ptr
new_d=(_tmp5DB=_get_dyneither_size(_tmp21D,sizeof(void*)),((_tmp5DC=(void**)
_region_malloc(_tmp1D9,_check_times(sizeof(void*),_tmp5DB)),((_tmp5DD=
_tag_dyneither(_tmp5DC,sizeof(void*),_tmp5DB),((((_tmp5DE=_tmp5DB,_tmp5E0(& env,&
_tmp5DF,& _tmp5DE,& _tmp5DC,& _tmp223,& _tmp21D))),_tmp5DD)))))));int change=0;{int i=
0;for(0;i < _get_dyneither_size(_tmp21D,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp21D.curr)[i]){
change=1;break;}}}if(!change){if(!_tmp21B)return r1;new_d=_tmp21D;}else{change=0;{
int i=0;for(0;i < _get_dyneither_size(_tmp21D,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp223,sizeof(void*),i))){change=1;break;}}}if(!
change){if(!_tmp21B)return r2;new_d=_tmp223;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp5E6;struct Cyc_CfFlowInfo_UnionRInfo _tmp5E5;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5E4;return(void*)((_tmp5E4=_region_malloc(_tmp1D9,sizeof(*_tmp5E4)),((
_tmp5E4[0]=((_tmp5E6.tag=6,((_tmp5E6.f1=((_tmp5E5.is_union=_tmp21B,((_tmp5E5.fieldnum=
_tmp21C == _tmp222?_tmp21C: - 1,_tmp5E5)))),((_tmp5E6.f2=new_d,_tmp5E6)))))),
_tmp5E4))));};}_LLC9:;_LLCA: goto _LLAE;_LLAE:;}{enum Cyc_CfFlowInfo_InitLevel il1=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel il2=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);struct _tuple0 _tmp5E7;struct
_tuple0 _tmp238=(_tmp5E7.f1=r1,((_tmp5E7.f2=r2,_tmp5E7)));void*_tmp239;void*
_tmp23B;_LLDC: _tmp239=_tmp238.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp23A=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp239;if(_tmp23A->tag != 4)goto _LLDE;};_LLDD:
goto _LLDF;_LLDE: _tmp23B=_tmp238.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp23C=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp23B;if(_tmp23C->tag != 4)goto _LLE0;};_LLDF: {
struct _tuple20 _tmp5E8;struct _tuple20 _tmp23E=(_tmp5E8.f1=il1,((_tmp5E8.f2=il2,
_tmp5E8)));enum Cyc_CfFlowInfo_InitLevel _tmp23F;enum Cyc_CfFlowInfo_InitLevel
_tmp240;enum Cyc_CfFlowInfo_InitLevel _tmp241;enum Cyc_CfFlowInfo_InitLevel
_tmp242;_LLE3: _tmp23F=_tmp23E.f2;if(_tmp23F != Cyc_CfFlowInfo_NoneIL)goto _LLE5;
_LLE4: goto _LLE6;_LLE5: _tmp240=_tmp23E.f1;if(_tmp240 != Cyc_CfFlowInfo_NoneIL)goto
_LLE7;_LLE6: return(env->fenv)->esc_none;_LLE7: _tmp241=_tmp23E.f2;if(_tmp241 != 
Cyc_CfFlowInfo_ThisIL)goto _LLE9;_LLE8: goto _LLEA;_LLE9: _tmp242=_tmp23E.f1;if(
_tmp242 != Cyc_CfFlowInfo_ThisIL)goto _LLEB;_LLEA: return(env->fenv)->esc_this;
_LLEB:;_LLEC: return(env->fenv)->esc_all;_LLE2:;}_LLE0:;_LLE1: {struct _tuple20
_tmp5E9;struct _tuple20 _tmp244=(_tmp5E9.f1=il1,((_tmp5E9.f2=il2,_tmp5E9)));enum 
Cyc_CfFlowInfo_InitLevel _tmp245;enum Cyc_CfFlowInfo_InitLevel _tmp246;enum Cyc_CfFlowInfo_InitLevel
_tmp247;enum Cyc_CfFlowInfo_InitLevel _tmp248;_LLEE: _tmp245=_tmp244.f2;if(_tmp245
!= Cyc_CfFlowInfo_NoneIL)goto _LLF0;_LLEF: goto _LLF1;_LLF0: _tmp246=_tmp244.f1;if(
_tmp246 != Cyc_CfFlowInfo_NoneIL)goto _LLF2;_LLF1: return(env->fenv)->unknown_none;
_LLF2: _tmp247=_tmp244.f2;if(_tmp247 != Cyc_CfFlowInfo_ThisIL)goto _LLF4;_LLF3: goto
_LLF5;_LLF4: _tmp248=_tmp244.f1;if(_tmp248 != Cyc_CfFlowInfo_ThisIL)goto _LLF6;
_LLF5: return(env->fenv)->unknown_this;_LLF6:;_LLF7: return(env->fenv)->unknown_all;
_LLED:;}_LLDB:;};};}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5EA;return((_tmp5EA.EqualConst).val=
i,(((_tmp5EA.EqualConst).tag=1,_tmp5EA)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(
struct Cyc_Absyn_Vardecl*v,void*t){struct _tuple11 _tmp5ED;union Cyc_CfFlowInfo_RelnOp
_tmp5EC;return((_tmp5EC.LessVar).val=((_tmp5ED.f1=v,((_tmp5ED.f2=t,_tmp5ED)))),(((
_tmp5EC.LessVar).tag=2,_tmp5EC)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(
struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp5EE;return((_tmp5EE.LessNumelts).val=
v,(((_tmp5EE.LessNumelts).tag=3,_tmp5EE)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5EF;return((_tmp5EF.LessConst).val=
i,(((_tmp5EF.LessConst).tag=4,_tmp5EF)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5F0;return((_tmp5F0.LessEqConst).val=
i,(((_tmp5F0.LessEqConst).tag=6,_tmp5F0)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(
struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp5F1;return((_tmp5F1.LessEqNumelts).val=
v,(((_tmp5F1.LessEqNumelts).tag=5,_tmp5F1)));}struct _tuple21{union Cyc_CfFlowInfo_RelnOp
f1;union Cyc_CfFlowInfo_RelnOp f2;};static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2){struct _tuple21 _tmp5F2;struct _tuple21 _tmp251=(
_tmp5F2.f1=r1,((_tmp5F2.f2=r2,_tmp5F2)));union Cyc_CfFlowInfo_RelnOp _tmp252;
unsigned int _tmp253;union Cyc_CfFlowInfo_RelnOp _tmp254;unsigned int _tmp255;union
Cyc_CfFlowInfo_RelnOp _tmp256;struct _tuple11 _tmp257;struct Cyc_Absyn_Vardecl*
_tmp258;union Cyc_CfFlowInfo_RelnOp _tmp259;struct _tuple11 _tmp25A;struct Cyc_Absyn_Vardecl*
_tmp25B;union Cyc_CfFlowInfo_RelnOp _tmp25C;struct Cyc_Absyn_Vardecl*_tmp25D;union
Cyc_CfFlowInfo_RelnOp _tmp25E;struct Cyc_Absyn_Vardecl*_tmp25F;union Cyc_CfFlowInfo_RelnOp
_tmp260;unsigned int _tmp261;union Cyc_CfFlowInfo_RelnOp _tmp262;unsigned int
_tmp263;union Cyc_CfFlowInfo_RelnOp _tmp264;unsigned int _tmp265;union Cyc_CfFlowInfo_RelnOp
_tmp266;unsigned int _tmp267;union Cyc_CfFlowInfo_RelnOp _tmp268;struct Cyc_Absyn_Vardecl*
_tmp269;union Cyc_CfFlowInfo_RelnOp _tmp26A;struct Cyc_Absyn_Vardecl*_tmp26B;_LLF9:
_tmp252=_tmp251.f1;if((_tmp252.EqualConst).tag != 1)goto _LLFB;_tmp253=(
unsigned int)(_tmp252.EqualConst).val;_tmp254=_tmp251.f2;if((_tmp254.EqualConst).tag
!= 1)goto _LLFB;_tmp255=(unsigned int)(_tmp254.EqualConst).val;_LLFA: return
_tmp253 == _tmp255;_LLFB: _tmp256=_tmp251.f1;if((_tmp256.LessVar).tag != 2)goto
_LLFD;_tmp257=(struct _tuple11)(_tmp256.LessVar).val;_tmp258=_tmp257.f1;_tmp259=
_tmp251.f2;if((_tmp259.LessVar).tag != 2)goto _LLFD;_tmp25A=(struct _tuple11)(
_tmp259.LessVar).val;_tmp25B=_tmp25A.f1;_LLFC: return _tmp258 == _tmp25B;_LLFD:
_tmp25C=_tmp251.f1;if((_tmp25C.LessNumelts).tag != 3)goto _LLFF;_tmp25D=(struct Cyc_Absyn_Vardecl*)(
_tmp25C.LessNumelts).val;_tmp25E=_tmp251.f2;if((_tmp25E.LessNumelts).tag != 3)
goto _LLFF;_tmp25F=(struct Cyc_Absyn_Vardecl*)(_tmp25E.LessNumelts).val;_LLFE:
return _tmp25D == _tmp25F;_LLFF: _tmp260=_tmp251.f1;if((_tmp260.LessConst).tag != 4)
goto _LL101;_tmp261=(unsigned int)(_tmp260.LessConst).val;_tmp262=_tmp251.f2;if((
_tmp262.LessConst).tag != 4)goto _LL101;_tmp263=(unsigned int)(_tmp262.LessConst).val;
_LL100: return _tmp261 == _tmp263;_LL101: _tmp264=_tmp251.f1;if((_tmp264.LessEqConst).tag
!= 6)goto _LL103;_tmp265=(unsigned int)(_tmp264.LessEqConst).val;_tmp266=_tmp251.f2;
if((_tmp266.LessEqConst).tag != 6)goto _LL103;_tmp267=(unsigned int)(_tmp266.LessEqConst).val;
_LL102: return _tmp265 == _tmp267;_LL103: _tmp268=_tmp251.f1;if((_tmp268.LessEqNumelts).tag
!= 5)goto _LL105;_tmp269=(struct Cyc_Absyn_Vardecl*)(_tmp268.LessEqNumelts).val;
_tmp26A=_tmp251.f2;if((_tmp26A.LessEqNumelts).tag != 5)goto _LL105;_tmp26B=(struct
Cyc_Absyn_Vardecl*)(_tmp26A.LessEqNumelts).val;_LL104: return _tmp269 == _tmp26B;
_LL105:;_LL106: return 0;_LLF8:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == 
r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*_tmp26C=
r1s;for(0;_tmp26C != 0;_tmp26C=_tmp26C->tl){struct Cyc_CfFlowInfo_Reln*_tmp26D=(
struct Cyc_CfFlowInfo_Reln*)_tmp26C->hd;int found=0;{struct Cyc_List_List*_tmp26E=
r2s;for(0;_tmp26E != 0;_tmp26E=_tmp26E->tl){struct Cyc_CfFlowInfo_Reln*_tmp26F=(
struct Cyc_CfFlowInfo_Reln*)_tmp26E->hd;if(_tmp26D == _tmp26F  || _tmp26D->vd == 
_tmp26F->vd  && Cyc_CfFlowInfo_same_relop(_tmp26D->rop,_tmp26F->rop)){{struct Cyc_List_List*
_tmp5F3;res=((_tmp5F3=_region_malloc(r,sizeof(*_tmp5F3)),((_tmp5F3->hd=_tmp26D,((
_tmp5F3->tl=res,_tmp5F3))))));}found=1;break;}}}if(!found)diff=1;}}if(!diff)res=
r1s;return res;};}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2){struct _tuple22 _tmp5F4;struct _tuple22 _tmp272=(_tmp5F4.f1=f1,((_tmp5F4.f2=f2,
_tmp5F4)));union Cyc_CfFlowInfo_FlowInfo _tmp273;int _tmp274;union Cyc_CfFlowInfo_FlowInfo
_tmp275;int _tmp276;union Cyc_CfFlowInfo_FlowInfo _tmp277;struct _tuple12 _tmp278;
struct Cyc_Dict_Dict _tmp279;struct Cyc_List_List*_tmp27A;union Cyc_CfFlowInfo_FlowInfo
_tmp27B;struct _tuple12 _tmp27C;struct Cyc_Dict_Dict _tmp27D;struct Cyc_List_List*
_tmp27E;_LL108: _tmp273=_tmp272.f1;if((_tmp273.BottomFL).tag != 1)goto _LL10A;
_tmp274=(int)(_tmp273.BottomFL).val;_LL109: return f2;_LL10A: _tmp275=_tmp272.f2;
if((_tmp275.BottomFL).tag != 1)goto _LL10C;_tmp276=(int)(_tmp275.BottomFL).val;
_LL10B: return f1;_LL10C: _tmp277=_tmp272.f1;if((_tmp277.ReachableFL).tag != 2)goto
_LL107;_tmp278=(struct _tuple12)(_tmp277.ReachableFL).val;_tmp279=_tmp278.f1;
_tmp27A=_tmp278.f2;_tmp27B=_tmp272.f2;if((_tmp27B.ReachableFL).tag != 2)goto
_LL107;_tmp27C=(struct _tuple12)(_tmp27B.ReachableFL).val;_tmp27D=_tmp27C.f1;
_tmp27E=_tmp27C.f2;_LL10D: if(_tmp279.t == _tmp27D.t  && _tmp27A == _tmp27E)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f2,f1))return f1;{struct _RegionHandle*_tmp27F=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp5F7;struct Cyc_CfFlowInfo_JoinEnv _tmp5F6;struct Cyc_CfFlowInfo_JoinEnv _tmp280=(
_tmp5F6.fenv=fenv,((_tmp5F6.pile=((_tmp5F7=_region_malloc(_tmp27F,sizeof(*
_tmp5F7)),((_tmp5F7->rgn=_tmp27F,((_tmp5F7->places=0,_tmp5F7)))))),((_tmp5F6.d1=
_tmp279,((_tmp5F6.d2=_tmp27D,_tmp5F6)))))));struct Cyc_Dict_Dict _tmp281=((struct
Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),
struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))
Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,
void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp280,_tmp279,_tmp27D);struct Cyc_List_List*
_tmp282=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp27A,_tmp27E);return Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_escape_these(fenv,_tmp280.pile,all_changed,_tmp281),_tmp282);};
_LL107:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;union Cyc_CfFlowInfo_FlowInfo f3;};struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple13
pr1,struct _tuple13 pr2){union Cyc_CfFlowInfo_FlowInfo _tmp286;void*_tmp287;struct
_tuple13 _tmp285=pr1;_tmp286=_tmp285.f1;_tmp287=_tmp285.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp289;void*_tmp28A;struct _tuple13 _tmp288=pr2;_tmp289=_tmp288.f1;_tmp28A=
_tmp288.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,
all_changed,_tmp286,_tmp289);struct _tuple23 _tmp5F8;struct _tuple23 _tmp28C=(
_tmp5F8.f1=_tmp286,((_tmp5F8.f2=_tmp289,((_tmp5F8.f3=outflow,_tmp5F8)))));union
Cyc_CfFlowInfo_FlowInfo _tmp28D;int _tmp28E;union Cyc_CfFlowInfo_FlowInfo _tmp28F;
int _tmp290;union Cyc_CfFlowInfo_FlowInfo _tmp291;struct _tuple12 _tmp292;struct Cyc_Dict_Dict
_tmp293;union Cyc_CfFlowInfo_FlowInfo _tmp294;struct _tuple12 _tmp295;struct Cyc_Dict_Dict
_tmp296;union Cyc_CfFlowInfo_FlowInfo _tmp297;struct _tuple12 _tmp298;struct Cyc_Dict_Dict
_tmp299;struct Cyc_List_List*_tmp29A;_LL10F: _tmp28D=_tmp28C.f1;if((_tmp28D.BottomFL).tag
!= 1)goto _LL111;_tmp28E=(int)(_tmp28D.BottomFL).val;_LL110: {struct _tuple13
_tmp5F9;return(_tmp5F9.f1=outflow,((_tmp5F9.f2=_tmp28A,_tmp5F9)));}_LL111:
_tmp28F=_tmp28C.f2;if((_tmp28F.BottomFL).tag != 1)goto _LL113;_tmp290=(int)(
_tmp28F.BottomFL).val;_LL112: {struct _tuple13 _tmp5FA;return(_tmp5FA.f1=outflow,((
_tmp5FA.f2=_tmp287,_tmp5FA)));}_LL113: _tmp291=_tmp28C.f1;if((_tmp291.ReachableFL).tag
!= 2)goto _LL115;_tmp292=(struct _tuple12)(_tmp291.ReachableFL).val;_tmp293=
_tmp292.f1;_tmp294=_tmp28C.f2;if((_tmp294.ReachableFL).tag != 2)goto _LL115;
_tmp295=(struct _tuple12)(_tmp294.ReachableFL).val;_tmp296=_tmp295.f1;_tmp297=
_tmp28C.f3;if((_tmp297.ReachableFL).tag != 2)goto _LL115;_tmp298=(struct _tuple12)(
_tmp297.ReachableFL).val;_tmp299=_tmp298.f1;_tmp29A=_tmp298.f2;_LL114: if(((int(*)(
int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp287,
_tmp28A)){struct _tuple13 _tmp5FB;return(_tmp5FB.f1=outflow,((_tmp5FB.f2=_tmp28A,
_tmp5FB)));}if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp28A,_tmp287)){struct _tuple13 _tmp5FC;return(_tmp5FC.f1=outflow,((_tmp5FC.f2=
_tmp287,_tmp5FC)));}{struct _RegionHandle*_tmp29F=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp5FF;struct Cyc_CfFlowInfo_JoinEnv _tmp5FE;struct Cyc_CfFlowInfo_JoinEnv _tmp2A0=(
_tmp5FE.fenv=fenv,((_tmp5FE.pile=((_tmp5FF=_region_malloc(_tmp29F,sizeof(*
_tmp5FF)),((_tmp5FF->rgn=_tmp29F,((_tmp5FF->places=0,_tmp5FF)))))),((_tmp5FE.d1=
_tmp293,((_tmp5FE.d2=_tmp296,_tmp5FE)))))));void*_tmp2A1=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp2A0,0,_tmp287,
_tmp28A);struct _tuple13 _tmp600;return(_tmp600.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp2A0.pile,all_changed,_tmp299),_tmp29A),((_tmp600.f2=_tmp2A1,_tmp600)));};
_LL115:;_LL116: {struct Cyc_Core_Impossible_struct _tmp606;const char*_tmp605;
struct Cyc_Core_Impossible_struct*_tmp604;(int)_throw((void*)((_tmp604=_cycalloc(
sizeof(*_tmp604)),((_tmp604[0]=((_tmp606.tag=Cyc_Core_Impossible,((_tmp606.f1=((
_tmp605="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp605,sizeof(
char),37))),_tmp606)))),_tmp604)))));}_LL10E:;};};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp60D(struct Cyc_CfFlowInfo_AfterEnv**
env,unsigned int*_tmp60C,unsigned int*_tmp60B,void***_tmp609,struct
_dyneither_ptr*_tmp2B9,struct _dyneither_ptr*_tmp2B3){for(*_tmp60C=0;*_tmp60C < *
_tmp60B;(*_tmp60C)++){(*_tmp609)[*_tmp60C]=Cyc_CfFlowInfo_after_absRval_d(*env,(
int)*_tmp60C,((void**)(*_tmp2B3).curr)[(int)*_tmp60C],*((void**)
_check_dyneither_subscript(*_tmp2B9,sizeof(void*),(int)*_tmp60C)));}}static void*
Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);int
changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);if(
changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*
r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);if(changed1){if(
!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))return r1;env->changed=
Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,
env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct _tuple0 _tmp607;
struct _tuple0 _tmp2A9=(_tmp607.f1=r1,((_tmp607.f2=r2,_tmp607)));void*_tmp2AA;void*
_tmp2AC;void*_tmp2AE;struct Cyc_CfFlowInfo_UnionRInfo _tmp2B0;int _tmp2B1;int
_tmp2B2;struct _dyneither_ptr _tmp2B3;void*_tmp2B4;struct Cyc_CfFlowInfo_UnionRInfo
_tmp2B6;int _tmp2B7;int _tmp2B8;struct _dyneither_ptr _tmp2B9;_LL118: _tmp2AA=_tmp2A9.f1;{
struct Cyc_CfFlowInfo_Consumed_struct*_tmp2AB=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp2AA;if(_tmp2AB->tag != 7)goto _LL11A;};_LL119: goto _LL11B;_LL11A: _tmp2AC=
_tmp2A9.f2;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp2AD=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp2AC;if(_tmp2AD->tag != 7)goto _LL11C;};_LL11B: return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
_LL11C: _tmp2AE=_tmp2A9.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2AF=(struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmp2AE;if(_tmp2AF->tag != 6)goto _LL11E;else{
_tmp2B0=_tmp2AF->f1;_tmp2B1=_tmp2B0.is_union;_tmp2B2=_tmp2B0.fieldnum;_tmp2B3=
_tmp2AF->f2;}};_tmp2B4=_tmp2A9.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2B5=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2B4;if(_tmp2B5->tag != 6)goto _LL11E;
else{_tmp2B6=_tmp2B5->f1;_tmp2B7=_tmp2B6.is_union;_tmp2B8=_tmp2B6.fieldnum;
_tmp2B9=_tmp2B5->f2;}};_LL11D: {unsigned int _tmp60C;unsigned int _tmp60B;struct
_dyneither_ptr _tmp60A;void**_tmp609;unsigned int _tmp608;struct _dyneither_ptr
new_d=(_tmp608=_get_dyneither_size(_tmp2B3,sizeof(void*)),((_tmp609=(void**)
_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp608)),((
_tmp60A=_tag_dyneither(_tmp609,sizeof(void*),_tmp608),((((_tmp60B=_tmp608,
_tmp60D(& env,& _tmp60C,& _tmp60B,& _tmp609,& _tmp2B9,& _tmp2B3))),_tmp60A)))))));int
change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp2B3,sizeof(void*));++ i){if(*((
void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp2B3.curr)[
i]){change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp2B3,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp2B9,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp613;struct Cyc_CfFlowInfo_UnionRInfo
_tmp612;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp611;return(void*)((_tmp611=
_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp611)),((_tmp611[0]=((_tmp613.tag=
6,((_tmp613.f1=((_tmp612.is_union=_tmp2B1,((_tmp612.fieldnum=_tmp2B2 == _tmp2B8?
_tmp2B2: - 1,_tmp612)))),((_tmp613.f2=new_d,_tmp613)))))),_tmp611))));};}_LL11E:;
_LL11F: {struct Cyc_Core_Impossible_struct _tmp619;const char*_tmp618;struct Cyc_Core_Impossible_struct*
_tmp617;(int)_throw((void*)((_tmp617=_cycalloc(sizeof(*_tmp617)),((_tmp617[0]=((
_tmp619.tag=Cyc_Core_Impossible,((_tmp619.f1=((_tmp618="after_absRval -- non-aggregates!",
_tag_dyneither(_tmp618,sizeof(char),33))),_tmp619)))),_tmp617)))));}_LL117:;};}
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int
key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**_tmp2C4=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp2C5=env->changed;{struct Cyc_List_List*
_tmp61A;*env->last_field_cell=((_tmp61A=_region_malloc(((env->joinenv).fenv)->r,
sizeof(*_tmp61A)),((_tmp61A->hd=(void*)key,((_tmp61A->tl=0,_tmp61A))))));}env->last_field_cell=&((
struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp2C7=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp2C4;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp2C5;return _tmp2C7;};};}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_CfFlowInfo_Place _tmp61B;*env->curr_place=((_tmp61B.root=root,((_tmp61B.fields=
0,_tmp61B))));}env->last_field_cell=&(env->curr_place)->fields;env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo
f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict
chg2){struct _tuple22 _tmp61C;struct _tuple22 _tmp2CA=(_tmp61C.f1=f1,((_tmp61C.f2=f2,
_tmp61C)));union Cyc_CfFlowInfo_FlowInfo _tmp2CB;int _tmp2CC;union Cyc_CfFlowInfo_FlowInfo
_tmp2CD;int _tmp2CE;union Cyc_CfFlowInfo_FlowInfo _tmp2CF;struct _tuple12 _tmp2D0;
struct Cyc_Dict_Dict _tmp2D1;struct Cyc_List_List*_tmp2D2;union Cyc_CfFlowInfo_FlowInfo
_tmp2D3;struct _tuple12 _tmp2D4;struct Cyc_Dict_Dict _tmp2D5;struct Cyc_List_List*
_tmp2D6;_LL121: _tmp2CB=_tmp2CA.f1;if((_tmp2CB.BottomFL).tag != 1)goto _LL123;
_tmp2CC=(int)(_tmp2CB.BottomFL).val;_LL122: goto _LL124;_LL123: _tmp2CD=_tmp2CA.f2;
if((_tmp2CD.BottomFL).tag != 1)goto _LL125;_tmp2CE=(int)(_tmp2CD.BottomFL).val;
_LL124: return Cyc_CfFlowInfo_BottomFL();_LL125: _tmp2CF=_tmp2CA.f1;if((_tmp2CF.ReachableFL).tag
!= 2)goto _LL120;_tmp2D0=(struct _tuple12)(_tmp2CF.ReachableFL).val;_tmp2D1=
_tmp2D0.f1;_tmp2D2=_tmp2D0.f2;_tmp2D3=_tmp2CA.f2;if((_tmp2D3.ReachableFL).tag != 
2)goto _LL120;_tmp2D4=(struct _tuple12)(_tmp2D3.ReachableFL).val;_tmp2D5=_tmp2D4.f1;
_tmp2D6=_tmp2D4.f2;_LL126: if(_tmp2D1.t == _tmp2D5.t  && _tmp2D2 == _tmp2D6)return f1;{
struct _RegionHandle*_tmp2D7=fenv->r;struct Cyc_CfFlowInfo_Place*_tmp2D8=fenv->dummy_place;
struct Cyc_CfFlowInfo_JoinEnv _tmp622;struct Cyc_CfFlowInfo_EscPile*_tmp621;struct
Cyc_CfFlowInfo_AfterEnv _tmp620;struct Cyc_CfFlowInfo_AfterEnv _tmp2D9=(_tmp620.joinenv=((
_tmp622.fenv=fenv,((_tmp622.pile=((_tmp621=_region_malloc(_tmp2D7,sizeof(*
_tmp621)),((_tmp621->rgn=_tmp2D7,((_tmp621->places=0,_tmp621)))))),((_tmp622.d1=
_tmp2D1,((_tmp622.d2=_tmp2D5,_tmp622)))))))),((_tmp620.chg1=chg1,((_tmp620.chg2=
chg2,((_tmp620.curr_place=_tmp2D8,((_tmp620.last_field_cell=& _tmp2D8->fields,((
_tmp620.changed=Cyc_CfFlowInfo_Neither,_tmp620)))))))))));struct Cyc_Dict_Dict
_tmp2DA=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,
void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp2D9,_tmp2D1,_tmp2D5);
struct Cyc_List_List*_tmp2DB=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp2D2,_tmp2D6);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,(_tmp2D9.joinenv).pile,
all_changed,_tmp2DA),_tmp2DB);};_LL120:;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp623;struct
_tuple0 _tmp2E0=(_tmp623.f1=r1,((_tmp623.f2=r2,_tmp623)));void*_tmp2E1;struct Cyc_CfFlowInfo_Place*
_tmp2E3;void*_tmp2E4;struct Cyc_CfFlowInfo_Place*_tmp2E6;void*_tmp2E7;void*
_tmp2E9;void*_tmp2EB;struct Cyc_CfFlowInfo_UnionRInfo _tmp2ED;struct _dyneither_ptr
_tmp2EE;void*_tmp2EF;struct Cyc_CfFlowInfo_UnionRInfo _tmp2F1;struct _dyneither_ptr
_tmp2F2;void*_tmp2F3;void*_tmp2F5;void*_tmp2F7;void*_tmp2F9;void*_tmp2FB;void*
_tmp2FD;void*_tmp2FF;void*_tmp301;_LL128: _tmp2E1=_tmp2E0.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2E2=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2E1;if(_tmp2E2->tag != 5)goto
_LL12A;else{_tmp2E3=_tmp2E2->f1;}};_tmp2E4=_tmp2E0.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2E5=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2E4;if(_tmp2E5->tag != 5)goto
_LL12A;else{_tmp2E6=_tmp2E5->f1;}};_LL129: return Cyc_CfFlowInfo_place_cmp(_tmp2E3,
_tmp2E6)== 0;_LL12A: _tmp2E7=_tmp2E0.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2E8=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2E7;if(_tmp2E8->tag != 5)goto
_LL12C;};_LL12B: goto _LL12D;_LL12C: _tmp2E9=_tmp2E0.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2EA=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2E9;if(_tmp2EA->tag != 5)goto
_LL12E;};_LL12D: return 0;_LL12E: _tmp2EB=_tmp2E0.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2EC=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2EB;if(_tmp2EC->tag != 6)goto
_LL130;else{_tmp2ED=_tmp2EC->f1;_tmp2EE=_tmp2EC->f2;}};_tmp2EF=_tmp2E0.f2;{
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2F0=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2EF;if(_tmp2F0->tag != 6)goto _LL130;else{_tmp2F1=_tmp2F0->f1;_tmp2F2=_tmp2F0->f2;}};
_LL12F: if((void**)_tmp2EE.curr == (void**)_tmp2F2.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp2EE,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2EE.curr)[i],*((
void**)_check_dyneither_subscript(_tmp2F2,sizeof(void*),i))))return 0;}}return 1;
_LL130: _tmp2F3=_tmp2E0.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2F4=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp2F3;if(_tmp2F4->tag != 1)goto _LL132;};
_tmp2F5=_tmp2E0.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2F6=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2F5;if(_tmp2F6->tag != 2)goto _LL132;};_LL131: return 1;_LL132: _tmp2F7=_tmp2E0.f2;{
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2F8=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2F7;if(_tmp2F8->tag != 2)goto _LL134;};_LL133: return 0;_LL134: _tmp2F9=_tmp2E0.f2;{
struct Cyc_CfFlowInfo_Zero_struct*_tmp2FA=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp2F9;if(_tmp2FA->tag != 0)goto _LL136;};_LL135: goto _LL137;_LL136: _tmp2FB=
_tmp2E0.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2FC=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp2FB;if(_tmp2FC->tag != 1)goto _LL138;};_LL137: return 0;_LL138: _tmp2FD=_tmp2E0.f1;{
struct Cyc_CfFlowInfo_Esc_struct*_tmp2FE=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp2FD;if(_tmp2FE->tag != 4)goto _LL13A;};_tmp2FF=_tmp2E0.f2;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp300=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2FF;if(_tmp300->tag != 4)goto _LL13A;};
_LL139: goto _LL127;_LL13A: _tmp301=_tmp2E0.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp302=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp301;if(_tmp302->tag != 4)goto _LL13C;};
_LL13B: return 0;_LL13C:;_LL13D: goto _LL127;_LL127:;}{struct _tuple20 _tmp624;struct
_tuple20 _tmp304=(_tmp624.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp624.f2=Cyc_CfFlowInfo_initlevel_approx(
r2),_tmp624)));enum Cyc_CfFlowInfo_InitLevel _tmp305;enum Cyc_CfFlowInfo_InitLevel
_tmp306;enum Cyc_CfFlowInfo_InitLevel _tmp307;enum Cyc_CfFlowInfo_InitLevel
_tmp308;enum Cyc_CfFlowInfo_InitLevel _tmp309;enum Cyc_CfFlowInfo_InitLevel
_tmp30A;_LL13F: _tmp305=_tmp304.f1;if(_tmp305 != Cyc_CfFlowInfo_AllIL)goto _LL141;
_tmp306=_tmp304.f2;if(_tmp306 != Cyc_CfFlowInfo_AllIL)goto _LL141;_LL140: return 1;
_LL141: _tmp307=_tmp304.f2;if(_tmp307 != Cyc_CfFlowInfo_NoneIL)goto _LL143;_LL142:
return 1;_LL143: _tmp308=_tmp304.f1;if(_tmp308 != Cyc_CfFlowInfo_NoneIL)goto _LL145;
_LL144: return 0;_LL145: _tmp309=_tmp304.f2;if(_tmp309 != Cyc_CfFlowInfo_ThisIL)goto
_LL147;_LL146: return 1;_LL147: _tmp30A=_tmp304.f1;if(_tmp30A != Cyc_CfFlowInfo_ThisIL)
goto _LL13E;_LL148: return 0;_LL13E:;};}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s != 0;
r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp30B=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;
int found=0;{struct Cyc_List_List*_tmp30C=r2s;for(0;_tmp30C != 0;_tmp30C=_tmp30C->tl){
struct Cyc_CfFlowInfo_Reln*_tmp30D=(struct Cyc_CfFlowInfo_Reln*)_tmp30C->hd;if(
_tmp30B == _tmp30D  || _tmp30B->vd == _tmp30D->vd  && Cyc_CfFlowInfo_same_relop(
_tmp30B->rop,_tmp30D->rop)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){struct _tuple22
_tmp625;struct _tuple22 _tmp30F=(_tmp625.f1=f1,((_tmp625.f2=f2,_tmp625)));union Cyc_CfFlowInfo_FlowInfo
_tmp310;int _tmp311;union Cyc_CfFlowInfo_FlowInfo _tmp312;int _tmp313;union Cyc_CfFlowInfo_FlowInfo
_tmp314;struct _tuple12 _tmp315;struct Cyc_Dict_Dict _tmp316;struct Cyc_List_List*
_tmp317;union Cyc_CfFlowInfo_FlowInfo _tmp318;struct _tuple12 _tmp319;struct Cyc_Dict_Dict
_tmp31A;struct Cyc_List_List*_tmp31B;_LL14A: _tmp310=_tmp30F.f1;if((_tmp310.BottomFL).tag
!= 1)goto _LL14C;_tmp311=(int)(_tmp310.BottomFL).val;_LL14B: return 1;_LL14C:
_tmp312=_tmp30F.f2;if((_tmp312.BottomFL).tag != 1)goto _LL14E;_tmp313=(int)(
_tmp312.BottomFL).val;_LL14D: return 0;_LL14E: _tmp314=_tmp30F.f1;if((_tmp314.ReachableFL).tag
!= 2)goto _LL149;_tmp315=(struct _tuple12)(_tmp314.ReachableFL).val;_tmp316=
_tmp315.f1;_tmp317=_tmp315.f2;_tmp318=_tmp30F.f2;if((_tmp318.ReachableFL).tag != 
2)goto _LL149;_tmp319=(struct _tuple12)(_tmp318.ReachableFL).val;_tmp31A=_tmp319.f1;
_tmp31B=_tmp319.f2;_LL14F: if(_tmp316.t == _tmp31A.t  && _tmp317 == _tmp31B)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp316,_tmp31A) && Cyc_CfFlowInfo_relns_approx(_tmp317,_tmp31B);_LL149:;}struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*
rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=0;for(p=rs;!found
 && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp31C=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp31C->vd == v){found=1;break;}{union Cyc_CfFlowInfo_RelnOp _tmp31D=
_tmp31C->rop;struct _tuple11 _tmp31E;struct Cyc_Absyn_Vardecl*_tmp31F;struct Cyc_Absyn_Vardecl*
_tmp320;struct Cyc_Absyn_Vardecl*_tmp321;_LL151: if((_tmp31D.LessVar).tag != 2)goto
_LL153;_tmp31E=(struct _tuple11)(_tmp31D.LessVar).val;_tmp31F=_tmp31E.f1;_LL152:
_tmp320=_tmp31F;goto _LL154;_LL153: if((_tmp31D.LessNumelts).tag != 3)goto _LL155;
_tmp320=(struct Cyc_Absyn_Vardecl*)(_tmp31D.LessNumelts).val;_LL154: _tmp321=
_tmp320;goto _LL156;_LL155: if((_tmp31D.LessEqNumelts).tag != 5)goto _LL157;_tmp321=(
struct Cyc_Absyn_Vardecl*)(_tmp31D.LessEqNumelts).val;_LL156: if(v == _tmp321)found=
1;goto _LL150;_LL157:;_LL158: goto _LL150;_LL150:;};}if(!found)return rs;{struct Cyc_List_List*
_tmp322=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp323=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp323->vd != v){{union Cyc_CfFlowInfo_RelnOp _tmp324=_tmp323->rop;struct
_tuple11 _tmp325;struct Cyc_Absyn_Vardecl*_tmp326;struct Cyc_Absyn_Vardecl*_tmp327;
struct Cyc_Absyn_Vardecl*_tmp328;_LL15A: if((_tmp324.LessVar).tag != 2)goto _LL15C;
_tmp325=(struct _tuple11)(_tmp324.LessVar).val;_tmp326=_tmp325.f1;_LL15B: _tmp327=
_tmp326;goto _LL15D;_LL15C: if((_tmp324.LessNumelts).tag != 3)goto _LL15E;_tmp327=(
struct Cyc_Absyn_Vardecl*)(_tmp324.LessNumelts).val;_LL15D: _tmp328=_tmp327;goto
_LL15F;_LL15E: if((_tmp324.LessEqNumelts).tag != 5)goto _LL160;_tmp328=(struct Cyc_Absyn_Vardecl*)(
_tmp324.LessEqNumelts).val;_LL15F: if(v == _tmp328)continue;goto _LL159;_LL160:;
_LL161: goto _LL159;_LL159:;}{struct Cyc_List_List*_tmp626;_tmp322=((_tmp626=
_region_malloc(rgn,sizeof(*_tmp626)),((_tmp626->hd=_tmp323,((_tmp626->tl=_tmp322,
_tmp626))))));};}}return _tmp322;};}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*
_tmp32A=e->r;void*_tmp32C;struct Cyc_Absyn_Vardecl*_tmp32E;void*_tmp330;struct Cyc_Absyn_Vardecl*
_tmp332;void*_tmp334;struct Cyc_Absyn_Vardecl*_tmp336;void*_tmp338;struct Cyc_Absyn_Vardecl*
_tmp33A;_LL163: {struct Cyc_Absyn_Var_e_struct*_tmp32B=(struct Cyc_Absyn_Var_e_struct*)
_tmp32A;if(_tmp32B->tag != 1)goto _LL165;else{_tmp32C=(void*)_tmp32B->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp32D=(struct Cyc_Absyn_Global_b_struct*)_tmp32C;if(
_tmp32D->tag != 1)goto _LL165;else{_tmp32E=_tmp32D->f1;}};}}_LL164: _tmp332=_tmp32E;
goto _LL166;_LL165: {struct Cyc_Absyn_Var_e_struct*_tmp32F=(struct Cyc_Absyn_Var_e_struct*)
_tmp32A;if(_tmp32F->tag != 1)goto _LL167;else{_tmp330=(void*)_tmp32F->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp331=(struct Cyc_Absyn_Param_b_struct*)_tmp330;if(
_tmp331->tag != 3)goto _LL167;else{_tmp332=_tmp331->f1;}};}}_LL166: _tmp336=_tmp332;
goto _LL168;_LL167: {struct Cyc_Absyn_Var_e_struct*_tmp333=(struct Cyc_Absyn_Var_e_struct*)
_tmp32A;if(_tmp333->tag != 1)goto _LL169;else{_tmp334=(void*)_tmp333->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp335=(struct Cyc_Absyn_Local_b_struct*)_tmp334;if(
_tmp335->tag != 4)goto _LL169;else{_tmp336=_tmp335->f1;}};}}_LL168: _tmp33A=_tmp336;
goto _LL16A;_LL169: {struct Cyc_Absyn_Var_e_struct*_tmp337=(struct Cyc_Absyn_Var_e_struct*)
_tmp32A;if(_tmp337->tag != 1)goto _LL16B;else{_tmp338=(void*)_tmp337->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp339=(struct Cyc_Absyn_Pat_b_struct*)_tmp338;if(_tmp339->tag
!= 5)goto _LL16B;else{_tmp33A=_tmp339->f1;}};}}_LL16A: if(!_tmp33A->escapes)return
Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp33A);goto _LL162;_LL16B:;_LL16C: goto _LL162;
_LL162:;}return r;}struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct
_RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){struct Cyc_CfFlowInfo_Reln*_tmp627;
return(_tmp627=_region_malloc(r2,sizeof(*_tmp627)),((_tmp627[0]=*r,_tmp627)));}
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*
relns){return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*
f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){for(
0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct Cyc_CfFlowInfo_Reln*_tmp33C=(
struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*_tmp33D=(struct Cyc_CfFlowInfo_Reln*)
r2->hd;if(_tmp33C->vd != _tmp33D->vd  || !Cyc_CfFlowInfo_same_relop(_tmp33C->rop,
_tmp33D->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(
rgn,r,v);{void*_tmp33E=e->r;struct Cyc_Absyn_MallocInfo _tmp340;struct Cyc_Absyn_Exp*
_tmp341;int _tmp342;_LL16E: {struct Cyc_Absyn_Malloc_e_struct*_tmp33F=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp33E;if(_tmp33F->tag != 34)goto _LL170;else{_tmp340=_tmp33F->f1;_tmp341=_tmp340.num_elts;
_tmp342=_tmp340.fat_result;if(_tmp342 != 1)goto _LL170;}}_LL16F: malloc_loop: {void*
_tmp343=_tmp341->r;struct Cyc_Absyn_Exp*_tmp345;void*_tmp347;struct Cyc_Absyn_Vardecl*
_tmp349;void*_tmp34B;struct Cyc_Absyn_Vardecl*_tmp34D;void*_tmp34F;struct Cyc_Absyn_Vardecl*
_tmp351;void*_tmp353;struct Cyc_Absyn_Vardecl*_tmp355;_LL173: {struct Cyc_Absyn_Cast_e_struct*
_tmp344=(struct Cyc_Absyn_Cast_e_struct*)_tmp343;if(_tmp344->tag != 15)goto _LL175;
else{_tmp345=_tmp344->f2;}}_LL174: _tmp341=_tmp345;goto malloc_loop;_LL175: {
struct Cyc_Absyn_Var_e_struct*_tmp346=(struct Cyc_Absyn_Var_e_struct*)_tmp343;if(
_tmp346->tag != 1)goto _LL177;else{_tmp347=(void*)_tmp346->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp348=(struct Cyc_Absyn_Pat_b_struct*)_tmp347;if(_tmp348->tag != 5)goto _LL177;
else{_tmp349=_tmp348->f1;}};}}_LL176: _tmp34D=_tmp349;goto _LL178;_LL177: {struct
Cyc_Absyn_Var_e_struct*_tmp34A=(struct Cyc_Absyn_Var_e_struct*)_tmp343;if(_tmp34A->tag
!= 1)goto _LL179;else{_tmp34B=(void*)_tmp34A->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp34C=(struct Cyc_Absyn_Local_b_struct*)_tmp34B;if(_tmp34C->tag != 4)goto _LL179;
else{_tmp34D=_tmp34C->f1;}};}}_LL178: _tmp351=_tmp34D;goto _LL17A;_LL179: {struct
Cyc_Absyn_Var_e_struct*_tmp34E=(struct Cyc_Absyn_Var_e_struct*)_tmp343;if(_tmp34E->tag
!= 1)goto _LL17B;else{_tmp34F=(void*)_tmp34E->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp350=(struct Cyc_Absyn_Param_b_struct*)_tmp34F;if(_tmp350->tag != 3)goto _LL17B;
else{_tmp351=_tmp350->f1;}};}}_LL17A: _tmp355=_tmp351;goto _LL17C;_LL17B: {struct
Cyc_Absyn_Var_e_struct*_tmp352=(struct Cyc_Absyn_Var_e_struct*)_tmp343;if(_tmp352->tag
!= 1)goto _LL17D;else{_tmp353=(void*)_tmp352->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp354=(struct Cyc_Absyn_Global_b_struct*)_tmp353;if(_tmp354->tag != 1)goto _LL17D;
else{_tmp355=_tmp354->f1;}};}}_LL17C: if(_tmp355->escapes)return r;{struct Cyc_CfFlowInfo_Reln*
_tmp62A;struct Cyc_List_List*_tmp629;return(_tmp629=_region_malloc(rgn,sizeof(*
_tmp629)),((_tmp629->hd=((_tmp62A=_region_malloc(rgn,sizeof(*_tmp62A)),((_tmp62A->vd=
_tmp355,((_tmp62A->rop=Cyc_CfFlowInfo_LessEqNumelts(v),_tmp62A)))))),((_tmp629->tl=
r,_tmp629)))));};_LL17D:;_LL17E: return r;_LL172:;}_LL170:;_LL171: goto _LL16D;
_LL16D:;}{void*_tmp358=Cyc_Tcutil_compress(v->type);_LL180: {struct Cyc_Absyn_IntType_struct*
_tmp359=(struct Cyc_Absyn_IntType_struct*)_tmp358;if(_tmp359->tag != 6)goto _LL182;}
_LL181: goto _LL17F;_LL182:;_LL183: return r;_LL17F:;}loop: {void*_tmp35A=e->r;
struct Cyc_Absyn_Exp*_tmp35C;union Cyc_Absyn_Cnst _tmp35E;struct _tuple6 _tmp35F;int
_tmp360;enum Cyc_Absyn_Primop _tmp362;struct Cyc_List_List*_tmp363;struct Cyc_List_List
_tmp364;struct Cyc_List_List*_tmp365;struct Cyc_List_List _tmp366;struct Cyc_Absyn_Exp*
_tmp367;enum Cyc_Absyn_Primop _tmp369;struct Cyc_List_List*_tmp36A;struct Cyc_List_List
_tmp36B;struct Cyc_Absyn_Exp*_tmp36C;_LL185: {struct Cyc_Absyn_Cast_e_struct*
_tmp35B=(struct Cyc_Absyn_Cast_e_struct*)_tmp35A;if(_tmp35B->tag != 15)goto _LL187;
else{_tmp35C=_tmp35B->f2;}}_LL186: e=_tmp35C;goto loop;_LL187: {struct Cyc_Absyn_Const_e_struct*
_tmp35D=(struct Cyc_Absyn_Const_e_struct*)_tmp35A;if(_tmp35D->tag != 0)goto _LL189;
else{_tmp35E=_tmp35D->f1;if((_tmp35E.Int_c).tag != 5)goto _LL189;_tmp35F=(struct
_tuple6)(_tmp35E.Int_c).val;_tmp360=_tmp35F.f2;}}_LL188: {struct Cyc_CfFlowInfo_Reln*
_tmp62D;struct Cyc_List_List*_tmp62C;return(_tmp62C=_region_malloc(rgn,sizeof(*
_tmp62C)),((_tmp62C->hd=((_tmp62D=_region_malloc(rgn,sizeof(*_tmp62D)),((_tmp62D->vd=
v,((_tmp62D->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp360),_tmp62D)))))),((
_tmp62C->tl=r,_tmp62C)))));}_LL189: {struct Cyc_Absyn_Primop_e_struct*_tmp361=(
struct Cyc_Absyn_Primop_e_struct*)_tmp35A;if(_tmp361->tag != 3)goto _LL18B;else{
_tmp362=_tmp361->f1;if(_tmp362 != Cyc_Absyn_Mod)goto _LL18B;_tmp363=_tmp361->f2;
if(_tmp363 == 0)goto _LL18B;_tmp364=*_tmp363;_tmp365=_tmp364.tl;if(_tmp365 == 0)
goto _LL18B;_tmp366=*_tmp365;_tmp367=(struct Cyc_Absyn_Exp*)_tmp366.hd;}}_LL18A:{
void*_tmp36F=_tmp367->r;enum Cyc_Absyn_Primop _tmp371;struct Cyc_List_List*_tmp372;
struct Cyc_List_List _tmp373;struct Cyc_Absyn_Exp*_tmp374;_LL190: {struct Cyc_Absyn_Primop_e_struct*
_tmp370=(struct Cyc_Absyn_Primop_e_struct*)_tmp36F;if(_tmp370->tag != 3)goto _LL192;
else{_tmp371=_tmp370->f1;_tmp372=_tmp370->f2;if(_tmp372 == 0)goto _LL192;_tmp373=*
_tmp372;_tmp374=(struct Cyc_Absyn_Exp*)_tmp373.hd;}}_LL191:{void*_tmp375=_tmp374->r;
void*_tmp377;struct Cyc_Absyn_Vardecl*_tmp379;void*_tmp37B;struct Cyc_Absyn_Vardecl*
_tmp37D;void*_tmp37F;struct Cyc_Absyn_Vardecl*_tmp381;void*_tmp383;struct Cyc_Absyn_Vardecl*
_tmp385;_LL195: {struct Cyc_Absyn_Var_e_struct*_tmp376=(struct Cyc_Absyn_Var_e_struct*)
_tmp375;if(_tmp376->tag != 1)goto _LL197;else{_tmp377=(void*)_tmp376->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp378=(struct Cyc_Absyn_Global_b_struct*)_tmp377;if(
_tmp378->tag != 1)goto _LL197;else{_tmp379=_tmp378->f1;}};}}_LL196: _tmp37D=_tmp379;
goto _LL198;_LL197: {struct Cyc_Absyn_Var_e_struct*_tmp37A=(struct Cyc_Absyn_Var_e_struct*)
_tmp375;if(_tmp37A->tag != 1)goto _LL199;else{_tmp37B=(void*)_tmp37A->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp37C=(struct Cyc_Absyn_Local_b_struct*)_tmp37B;if(
_tmp37C->tag != 4)goto _LL199;else{_tmp37D=_tmp37C->f1;}};}}_LL198: _tmp381=_tmp37D;
goto _LL19A;_LL199: {struct Cyc_Absyn_Var_e_struct*_tmp37E=(struct Cyc_Absyn_Var_e_struct*)
_tmp375;if(_tmp37E->tag != 1)goto _LL19B;else{_tmp37F=(void*)_tmp37E->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp380=(struct Cyc_Absyn_Param_b_struct*)_tmp37F;if(
_tmp380->tag != 3)goto _LL19B;else{_tmp381=_tmp380->f1;}};}}_LL19A: _tmp385=_tmp381;
goto _LL19C;_LL19B: {struct Cyc_Absyn_Var_e_struct*_tmp382=(struct Cyc_Absyn_Var_e_struct*)
_tmp375;if(_tmp382->tag != 1)goto _LL19D;else{_tmp383=(void*)_tmp382->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp384=(struct Cyc_Absyn_Pat_b_struct*)_tmp383;if(_tmp384->tag
!= 5)goto _LL19D;else{_tmp385=_tmp384->f1;}};}}_LL19C: if(_tmp385->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp630;struct Cyc_List_List*_tmp62F;return(_tmp62F=
_region_malloc(rgn,sizeof(*_tmp62F)),((_tmp62F->hd=((_tmp630=_region_malloc(rgn,
sizeof(*_tmp630)),((_tmp630->vd=v,((_tmp630->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp385),_tmp630)))))),((_tmp62F->tl=r,_tmp62F)))));};_LL19D:;_LL19E: goto _LL194;
_LL194:;}goto _LL18F;_LL192:;_LL193: goto _LL18F;_LL18F:;}goto _LL184;_LL18B: {
struct Cyc_Absyn_Primop_e_struct*_tmp368=(struct Cyc_Absyn_Primop_e_struct*)
_tmp35A;if(_tmp368->tag != 3)goto _LL18D;else{_tmp369=_tmp368->f1;_tmp36A=_tmp368->f2;
if(_tmp36A == 0)goto _LL18D;_tmp36B=*_tmp36A;_tmp36C=(struct Cyc_Absyn_Exp*)_tmp36B.hd;}}
_LL18C:{void*_tmp388=_tmp36C->r;void*_tmp38A;struct Cyc_Absyn_Vardecl*_tmp38C;
void*_tmp38E;struct Cyc_Absyn_Vardecl*_tmp390;void*_tmp392;struct Cyc_Absyn_Vardecl*
_tmp394;void*_tmp396;struct Cyc_Absyn_Vardecl*_tmp398;_LL1A0: {struct Cyc_Absyn_Var_e_struct*
_tmp389=(struct Cyc_Absyn_Var_e_struct*)_tmp388;if(_tmp389->tag != 1)goto _LL1A2;
else{_tmp38A=(void*)_tmp389->f2;{struct Cyc_Absyn_Global_b_struct*_tmp38B=(struct
Cyc_Absyn_Global_b_struct*)_tmp38A;if(_tmp38B->tag != 1)goto _LL1A2;else{_tmp38C=
_tmp38B->f1;}};}}_LL1A1: _tmp390=_tmp38C;goto _LL1A3;_LL1A2: {struct Cyc_Absyn_Var_e_struct*
_tmp38D=(struct Cyc_Absyn_Var_e_struct*)_tmp388;if(_tmp38D->tag != 1)goto _LL1A4;
else{_tmp38E=(void*)_tmp38D->f2;{struct Cyc_Absyn_Local_b_struct*_tmp38F=(struct
Cyc_Absyn_Local_b_struct*)_tmp38E;if(_tmp38F->tag != 4)goto _LL1A4;else{_tmp390=
_tmp38F->f1;}};}}_LL1A3: _tmp394=_tmp390;goto _LL1A5;_LL1A4: {struct Cyc_Absyn_Var_e_struct*
_tmp391=(struct Cyc_Absyn_Var_e_struct*)_tmp388;if(_tmp391->tag != 1)goto _LL1A6;
else{_tmp392=(void*)_tmp391->f2;{struct Cyc_Absyn_Param_b_struct*_tmp393=(struct
Cyc_Absyn_Param_b_struct*)_tmp392;if(_tmp393->tag != 3)goto _LL1A6;else{_tmp394=
_tmp393->f1;}};}}_LL1A5: _tmp398=_tmp394;goto _LL1A7;_LL1A6: {struct Cyc_Absyn_Var_e_struct*
_tmp395=(struct Cyc_Absyn_Var_e_struct*)_tmp388;if(_tmp395->tag != 1)goto _LL1A8;
else{_tmp396=(void*)_tmp395->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp397=(struct Cyc_Absyn_Pat_b_struct*)
_tmp396;if(_tmp397->tag != 5)goto _LL1A8;else{_tmp398=_tmp397->f1;}};}}_LL1A7: if(
_tmp398->escapes)return r;{struct Cyc_CfFlowInfo_Reln*_tmp633;struct Cyc_List_List*
_tmp632;return(_tmp632=_region_malloc(rgn,sizeof(*_tmp632)),((_tmp632->hd=((
_tmp633=_region_malloc(rgn,sizeof(*_tmp633)),((_tmp633->vd=v,((_tmp633->rop=Cyc_CfFlowInfo_LessEqNumelts(
_tmp398),_tmp633)))))),((_tmp632->tl=r,_tmp632)))));};_LL1A8:;_LL1A9: goto _LL19F;
_LL19F:;}goto _LL184;_LL18D:;_LL18E: goto _LL184;_LL184:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp39B=e1->r;void*_tmp39D;
struct Cyc_Absyn_Vardecl*_tmp39F;void*_tmp3A1;struct Cyc_Absyn_Vardecl*_tmp3A3;
void*_tmp3A5;struct Cyc_Absyn_Vardecl*_tmp3A7;void*_tmp3A9;struct Cyc_Absyn_Vardecl*
_tmp3AB;_LL1AB: {struct Cyc_Absyn_Var_e_struct*_tmp39C=(struct Cyc_Absyn_Var_e_struct*)
_tmp39B;if(_tmp39C->tag != 1)goto _LL1AD;else{_tmp39D=(void*)_tmp39C->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp39E=(struct Cyc_Absyn_Global_b_struct*)_tmp39D;if(
_tmp39E->tag != 1)goto _LL1AD;else{_tmp39F=_tmp39E->f1;}};}}_LL1AC: _tmp3A3=_tmp39F;
goto _LL1AE;_LL1AD: {struct Cyc_Absyn_Var_e_struct*_tmp3A0=(struct Cyc_Absyn_Var_e_struct*)
_tmp39B;if(_tmp3A0->tag != 1)goto _LL1AF;else{_tmp3A1=(void*)_tmp3A0->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp3A2=(struct Cyc_Absyn_Param_b_struct*)_tmp3A1;if(
_tmp3A2->tag != 3)goto _LL1AF;else{_tmp3A3=_tmp3A2->f1;}};}}_LL1AE: _tmp3A7=_tmp3A3;
goto _LL1B0;_LL1AF: {struct Cyc_Absyn_Var_e_struct*_tmp3A4=(struct Cyc_Absyn_Var_e_struct*)
_tmp39B;if(_tmp3A4->tag != 1)goto _LL1B1;else{_tmp3A5=(void*)_tmp3A4->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp3A6=(struct Cyc_Absyn_Local_b_struct*)_tmp3A5;if(
_tmp3A6->tag != 4)goto _LL1B1;else{_tmp3A7=_tmp3A6->f1;}};}}_LL1B0: _tmp3AB=_tmp3A7;
goto _LL1B2;_LL1B1: {struct Cyc_Absyn_Var_e_struct*_tmp3A8=(struct Cyc_Absyn_Var_e_struct*)
_tmp39B;if(_tmp3A8->tag != 1)goto _LL1B3;else{_tmp3A9=(void*)_tmp3A8->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp3AA=(struct Cyc_Absyn_Pat_b_struct*)_tmp3A9;if(_tmp3AA->tag
!= 5)goto _LL1B3;else{_tmp3AB=_tmp3AA->f1;}};}}_LL1B2: if(!_tmp3AB->escapes)return
Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp3AB,e2);goto _LL1AA;_LL1B3:;_LL1B4: goto
_LL1AA;_LL1AA:;}return r;}static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){{const char*_tmp637;void*_tmp636[1];struct Cyc_String_pa_struct _tmp635;(_tmp635.tag=
0,((_tmp635.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name)),((_tmp636[0]=& _tmp635,Cyc_fprintf(Cyc_stderr,((_tmp637="%s",
_tag_dyneither(_tmp637,sizeof(char),3))),_tag_dyneither(_tmp636,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp3AF=r->rop;unsigned int _tmp3B0;struct _tuple11
_tmp3B1;struct Cyc_Absyn_Vardecl*_tmp3B2;struct Cyc_Absyn_Vardecl*_tmp3B3;
unsigned int _tmp3B4;unsigned int _tmp3B5;struct Cyc_Absyn_Vardecl*_tmp3B6;_LL1B6:
if((_tmp3AF.EqualConst).tag != 1)goto _LL1B8;_tmp3B0=(unsigned int)(_tmp3AF.EqualConst).val;
_LL1B7:{const char*_tmp63B;void*_tmp63A[1];struct Cyc_Int_pa_struct _tmp639;(
_tmp639.tag=1,((_tmp639.f1=(unsigned long)((int)_tmp3B0),((_tmp63A[0]=& _tmp639,
Cyc_fprintf(Cyc_stderr,((_tmp63B="==%d",_tag_dyneither(_tmp63B,sizeof(char),5))),
_tag_dyneither(_tmp63A,sizeof(void*),1)))))));}goto _LL1B5;_LL1B8: if((_tmp3AF.LessVar).tag
!= 2)goto _LL1BA;_tmp3B1=(struct _tuple11)(_tmp3AF.LessVar).val;_tmp3B2=_tmp3B1.f1;
_LL1B9:{const char*_tmp63F;void*_tmp63E[1];struct Cyc_String_pa_struct _tmp63D;(
_tmp63D.tag=0,((_tmp63D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp3B2->name)),((_tmp63E[0]=& _tmp63D,Cyc_fprintf(Cyc_stderr,((_tmp63F="<%s",
_tag_dyneither(_tmp63F,sizeof(char),4))),_tag_dyneither(_tmp63E,sizeof(void*),1)))))));}
goto _LL1B5;_LL1BA: if((_tmp3AF.LessNumelts).tag != 3)goto _LL1BC;_tmp3B3=(struct Cyc_Absyn_Vardecl*)(
_tmp3AF.LessNumelts).val;_LL1BB:{const char*_tmp643;void*_tmp642[1];struct Cyc_String_pa_struct
_tmp641;(_tmp641.tag=0,((_tmp641.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp3B3->name)),((_tmp642[0]=& _tmp641,Cyc_fprintf(Cyc_stderr,((
_tmp643="<numelts(%s)",_tag_dyneither(_tmp643,sizeof(char),13))),_tag_dyneither(
_tmp642,sizeof(void*),1)))))));}goto _LL1B5;_LL1BC: if((_tmp3AF.LessConst).tag != 4)
goto _LL1BE;_tmp3B4=(unsigned int)(_tmp3AF.LessConst).val;_LL1BD:{const char*
_tmp647;void*_tmp646[1];struct Cyc_Int_pa_struct _tmp645;(_tmp645.tag=1,((_tmp645.f1=(
unsigned long)((int)_tmp3B4),((_tmp646[0]=& _tmp645,Cyc_fprintf(Cyc_stderr,((
_tmp647="<%d",_tag_dyneither(_tmp647,sizeof(char),4))),_tag_dyneither(_tmp646,
sizeof(void*),1)))))));}goto _LL1B5;_LL1BE: if((_tmp3AF.LessEqConst).tag != 6)goto
_LL1C0;_tmp3B5=(unsigned int)(_tmp3AF.LessEqConst).val;_LL1BF:{const char*_tmp64B;
void*_tmp64A[1];struct Cyc_Int_pa_struct _tmp649;(_tmp649.tag=1,((_tmp649.f1=(
unsigned long)((int)_tmp3B5),((_tmp64A[0]=& _tmp649,Cyc_fprintf(Cyc_stderr,((
_tmp64B="<=%d",_tag_dyneither(_tmp64B,sizeof(char),5))),_tag_dyneither(_tmp64A,
sizeof(void*),1)))))));}goto _LL1B5;_LL1C0: if((_tmp3AF.LessEqNumelts).tag != 5)
goto _LL1B5;_tmp3B6=(struct Cyc_Absyn_Vardecl*)(_tmp3AF.LessEqNumelts).val;_LL1C1:{
const char*_tmp64F;void*_tmp64E[1];struct Cyc_String_pa_struct _tmp64D;(_tmp64D.tag=
0,((_tmp64D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp3B6->name)),((_tmp64E[0]=& _tmp64D,Cyc_fprintf(Cyc_stderr,((_tmp64F="<=numelts(%s)",
_tag_dyneither(_tmp64F,sizeof(char),14))),_tag_dyneither(_tmp64E,sizeof(void*),1)))))));}
goto _LL1B5;_LL1B5:;};}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(
0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);
if(r->tl != 0){const char*_tmp652;void*_tmp651;(_tmp651=0,Cyc_fprintf(Cyc_stderr,((
_tmp652=",",_tag_dyneither(_tmp652,sizeof(char),2))),_tag_dyneither(_tmp651,
sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel
il){switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1C2:{const char*_tmp655;void*_tmp654;(
_tmp654=0,Cyc_fprintf(Cyc_stderr,((_tmp655="uninitialized",_tag_dyneither(
_tmp655,sizeof(char),14))),_tag_dyneither(_tmp654,sizeof(void*),0)));}break;case
Cyc_CfFlowInfo_ThisIL: _LL1C3:{const char*_tmp658;void*_tmp657;(_tmp657=0,Cyc_fprintf(
Cyc_stderr,((_tmp658="this-initialized",_tag_dyneither(_tmp658,sizeof(char),17))),
_tag_dyneither(_tmp657,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1C4:{
const char*_tmp65B;void*_tmp65A;(_tmp65A=0,Cyc_fprintf(Cyc_stderr,((_tmp65B="all-initialized",
_tag_dyneither(_tmp65B,sizeof(char),16))),_tag_dyneither(_tmp65A,sizeof(void*),0)));}
break;}}void Cyc_CfFlowInfo_print_root(void*root){void*_tmp3D1=root;struct Cyc_Absyn_Vardecl*
_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D5;void*_tmp3D6;_LL1C7: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp3D2=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp3D1;if(_tmp3D2->tag != 0)goto
_LL1C9;else{_tmp3D3=_tmp3D2->f1;}}_LL1C8:{const char*_tmp65F;void*_tmp65E[1];
struct Cyc_String_pa_struct _tmp65D;(_tmp65D.tag=0,((_tmp65D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3D3->name)),((
_tmp65E[0]=& _tmp65D,Cyc_fprintf(Cyc_stderr,((_tmp65F="Root(%s)",_tag_dyneither(
_tmp65F,sizeof(char),9))),_tag_dyneither(_tmp65E,sizeof(void*),1)))))));}goto
_LL1C6;_LL1C9: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp3D4=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp3D1;if(_tmp3D4->tag != 1)goto _LL1CB;else{_tmp3D5=_tmp3D4->f1;_tmp3D6=(void*)
_tmp3D4->f2;}}_LL1CA:{const char*_tmp664;void*_tmp663[2];struct Cyc_String_pa_struct
_tmp662;struct Cyc_String_pa_struct _tmp661;(_tmp661.tag=0,((_tmp661.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3D6)),((_tmp662.tag=
0,((_tmp662.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
_tmp3D5)),((_tmp663[0]=& _tmp662,((_tmp663[1]=& _tmp661,Cyc_fprintf(Cyc_stderr,((
_tmp664="MallocPt(%s,%s)",_tag_dyneither(_tmp664,sizeof(char),16))),
_tag_dyneither(_tmp663,sizeof(void*),2)))))))))))));}goto _LL1C6;_LL1CB: {struct
Cyc_CfFlowInfo_InitParam_struct*_tmp3D7=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp3D1;if(_tmp3D7->tag != 2)goto _LL1C6;}_LL1CC:{const char*_tmp667;void*_tmp666;(
_tmp666=0,Cyc_fprintf(Cyc_stderr,((_tmp667="InitParam(_,_)",_tag_dyneither(
_tmp667,sizeof(char),15))),_tag_dyneither(_tmp666,sizeof(void*),0)));}goto _LL1C6;
_LL1C6:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){Cyc_CfFlowInfo_print_root(
p->root);{struct Cyc_List_List*_tmp3E1=p->fields;while(_tmp3E1 != 0){{const char*
_tmp66B;void*_tmp66A[1];struct Cyc_Int_pa_struct _tmp669;(_tmp669.tag=1,((_tmp669.f1=(
unsigned long)((int)_tmp3E1->hd),((_tmp66A[0]=& _tmp669,Cyc_fprintf(Cyc_stderr,((
_tmp66B=".%d",_tag_dyneither(_tmp66B,sizeof(char),4))),_tag_dyneither(_tmp66A,
sizeof(void*),1)))))));}_tmp3E1=_tmp3E1->tl;}{const char*_tmp66E;void*_tmp66D;(
_tmp66D=0,Cyc_fprintf(Cyc_stderr,((_tmp66E=" ",_tag_dyneither(_tmp66E,sizeof(
char),2))),_tag_dyneither(_tmp66D,sizeof(void*),0)));};};}void Cyc_CfFlowInfo_print_list(
struct Cyc_List_List*x,void(*pr)(void*)){int _tmp3E7=1;while(x != 0){if(!_tmp3E7){{
const char*_tmp671;void*_tmp670;(_tmp670=0,Cyc_fprintf(Cyc_stderr,((_tmp671=", ",
_tag_dyneither(_tmp671,sizeof(char),3))),_tag_dyneither(_tmp670,sizeof(void*),0)));}
_tmp3E7=0;}pr((void*)x->hd);x=x->tl;}{const char*_tmp674;void*_tmp673;(_tmp673=0,
Cyc_fprintf(Cyc_stderr,((_tmp674="\n",_tag_dyneither(_tmp674,sizeof(char),2))),
_tag_dyneither(_tmp673,sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*)){struct _RegionHandle _tmp3EC=_new_region("r");
struct _RegionHandle*r=& _tmp3EC;_push_region(r);{struct _handler_cons _tmp3ED;
_push_handler(& _tmp3ED);{int _tmp3EF=0;if(setjmp(_tmp3ED.handler))_tmp3EF=1;if(!
_tmp3EF){{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple14 _tmp3F0=*((
struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;{const char*_tmp677;void*_tmp676;(_tmp676=0,Cyc_fprintf(Cyc_stderr,((
_tmp677="{ ",_tag_dyneither(_tmp677,sizeof(char),3))),_tag_dyneither(_tmp676,
sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(
iter,& _tmp3F0)){void*_tmp3F3=_tmp3F0.f1;if(!first){{const char*_tmp67A;void*
_tmp679;(_tmp679=0,Cyc_fprintf(Cyc_stderr,((_tmp67A=", ",_tag_dyneither(_tmp67A,
sizeof(char),3))),_tag_dyneither(_tmp679,sizeof(void*),0)));}first=0;}pr(_tmp3F3);}{
const char*_tmp67D;void*_tmp67C;(_tmp67C=0,Cyc_fprintf(Cyc_stderr,((_tmp67D="}\n",
_tag_dyneither(_tmp67D,sizeof(char),3))),_tag_dyneither(_tmp67C,sizeof(void*),0)));};};
_pop_handler();}else{void*_tmp3EE=(void*)_exn_thrown;void*_tmp3F9=_tmp3EE;_LL1CE:;
_LL1CF:{const char*_tmp680;void*_tmp67F;(_tmp67F=0,Cyc_fprintf(Cyc_stderr,((
_tmp680="{ }\n",_tag_dyneither(_tmp680,sizeof(char),5))),_tag_dyneither(_tmp67F,
sizeof(void*),0)));}_npop_handler(0);return;_LL1D0:;_LL1D1:(void)_throw(_tmp3F9);
_LL1CD:;}};};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval){void*
_tmp3FC=rval;enum Cyc_CfFlowInfo_InitLevel _tmp401;enum Cyc_CfFlowInfo_InitLevel
_tmp403;struct Cyc_CfFlowInfo_Place*_tmp405;struct Cyc_CfFlowInfo_UnionRInfo
_tmp407;int _tmp408;int _tmp409;struct _dyneither_ptr _tmp40A;struct Cyc_Absyn_Exp*
_tmp40C;int _tmp40D;void*_tmp40E;_LL1D3: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3FD=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3FC;if(_tmp3FD->tag != 0)goto
_LL1D5;}_LL1D4:{const char*_tmp683;void*_tmp682;(_tmp682=0,Cyc_fprintf(Cyc_stderr,((
_tmp683="Zero",_tag_dyneither(_tmp683,sizeof(char),5))),_tag_dyneither(_tmp682,
sizeof(void*),0)));}goto _LL1D2;_LL1D5: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3FE=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3FC;if(_tmp3FE->tag != 1)
goto _LL1D7;}_LL1D6:{const char*_tmp686;void*_tmp685;(_tmp685=0,Cyc_fprintf(Cyc_stderr,((
_tmp686="NotZeroAll",_tag_dyneither(_tmp686,sizeof(char),11))),_tag_dyneither(
_tmp685,sizeof(void*),0)));}goto _LL1D2;_LL1D7: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3FF=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3FC;if(_tmp3FF->tag != 2)
goto _LL1D9;}_LL1D8:{const char*_tmp689;void*_tmp688;(_tmp688=0,Cyc_fprintf(Cyc_stderr,((
_tmp689="NotZeroThis",_tag_dyneither(_tmp689,sizeof(char),12))),_tag_dyneither(
_tmp688,sizeof(void*),0)));}goto _LL1D2;_LL1D9: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp400=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3FC;if(_tmp400->tag != 3)goto
_LL1DB;else{_tmp401=_tmp400->f1;}}_LL1DA:{const char*_tmp68C;void*_tmp68B;(
_tmp68B=0,Cyc_fprintf(Cyc_stderr,((_tmp68C="Unknown(",_tag_dyneither(_tmp68C,
sizeof(char),9))),_tag_dyneither(_tmp68B,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp401);{const char*_tmp68F;void*_tmp68E;(_tmp68E=0,Cyc_fprintf(Cyc_stderr,((
_tmp68F=")",_tag_dyneither(_tmp68F,sizeof(char),2))),_tag_dyneither(_tmp68E,
sizeof(void*),0)));}goto _LL1D2;_LL1DB: {struct Cyc_CfFlowInfo_Esc_struct*_tmp402=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp3FC;if(_tmp402->tag != 4)goto _LL1DD;else{
_tmp403=_tmp402->f1;}}_LL1DC:{const char*_tmp692;void*_tmp691;(_tmp691=0,Cyc_fprintf(
Cyc_stderr,((_tmp692="Esc(",_tag_dyneither(_tmp692,sizeof(char),5))),
_tag_dyneither(_tmp691,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp403);{const char*_tmp695;void*_tmp694;(_tmp694=0,Cyc_fprintf(Cyc_stderr,((
_tmp695=")",_tag_dyneither(_tmp695,sizeof(char),2))),_tag_dyneither(_tmp694,
sizeof(void*),0)));}goto _LL1D2;_LL1DD: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp404=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3FC;if(_tmp404->tag != 5)goto
_LL1DF;else{_tmp405=_tmp404->f1;}}_LL1DE:{const char*_tmp698;void*_tmp697;(
_tmp697=0,Cyc_fprintf(Cyc_stderr,((_tmp698="AddrOf(",_tag_dyneither(_tmp698,
sizeof(char),8))),_tag_dyneither(_tmp697,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(
_tmp405);{const char*_tmp69B;void*_tmp69A;(_tmp69A=0,Cyc_fprintf(Cyc_stderr,((
_tmp69B=")",_tag_dyneither(_tmp69B,sizeof(char),2))),_tag_dyneither(_tmp69A,
sizeof(void*),0)));}goto _LL1D2;_LL1DF: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp406=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp3FC;if(_tmp406->tag != 6)goto
_LL1E1;else{_tmp407=_tmp406->f1;_tmp408=_tmp407.is_union;_tmp409=_tmp407.fieldnum;
_tmp40A=_tmp406->f2;}}_LL1E0: if(_tmp408){{const char*_tmp69E;void*_tmp69D;(
_tmp69D=0,Cyc_fprintf(Cyc_stderr,((_tmp69E="AggrUnion{",_tag_dyneither(_tmp69E,
sizeof(char),11))),_tag_dyneither(_tmp69D,sizeof(void*),0)));}if(_tmp409 != - 1){
const char*_tmp6A2;void*_tmp6A1[1];struct Cyc_Int_pa_struct _tmp6A0;(_tmp6A0.tag=1,((
_tmp6A0.f1=(unsigned long)_tmp409,((_tmp6A1[0]=& _tmp6A0,Cyc_fprintf(Cyc_stderr,((
_tmp6A2="tag == %d;",_tag_dyneither(_tmp6A2,sizeof(char),11))),_tag_dyneither(
_tmp6A1,sizeof(void*),1)))))));}}else{const char*_tmp6A5;void*_tmp6A4;(_tmp6A4=0,
Cyc_fprintf(Cyc_stderr,((_tmp6A5="AggrStruct{",_tag_dyneither(_tmp6A5,sizeof(
char),12))),_tag_dyneither(_tmp6A4,sizeof(void*),0)));}{int i=0;for(0;i < 
_get_dyneither_size(_tmp40A,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp40A.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp40A,sizeof(void*))){
const char*_tmp6A8;void*_tmp6A7;(_tmp6A7=0,Cyc_fprintf(Cyc_stderr,((_tmp6A8=",",
_tag_dyneither(_tmp6A8,sizeof(char),2))),_tag_dyneither(_tmp6A7,sizeof(void*),0)));}}}{
const char*_tmp6AB;void*_tmp6AA;(_tmp6AA=0,Cyc_fprintf(Cyc_stderr,((_tmp6AB="}",
_tag_dyneither(_tmp6AB,sizeof(char),2))),_tag_dyneither(_tmp6AA,sizeof(void*),0)));}
goto _LL1D2;_LL1E1: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp40B=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp3FC;if(_tmp40B->tag != 7)goto _LL1D2;else{_tmp40C=_tmp40B->f1;_tmp40D=_tmp40B->f2;
_tmp40E=(void*)_tmp40B->f3;}}_LL1E2:{const char*_tmp6B0;void*_tmp6AF[2];struct Cyc_String_pa_struct
_tmp6AE;struct Cyc_Int_pa_struct _tmp6AD;(_tmp6AD.tag=1,((_tmp6AD.f1=(
unsigned long)_tmp40D,((_tmp6AE.tag=0,((_tmp6AE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp40C)),((_tmp6AF[0]=& _tmp6AE,((
_tmp6AF[1]=& _tmp6AD,Cyc_fprintf(Cyc_stderr,((_tmp6B0="[Consumed(%s,%d,",
_tag_dyneither(_tmp6B0,sizeof(char),17))),_tag_dyneither(_tmp6AF,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp40E);{const char*_tmp6B3;void*_tmp6B2;(_tmp6B2=0,
Cyc_fprintf(Cyc_stderr,((_tmp6B3="]",_tag_dyneither(_tmp6B3,sizeof(char),2))),
_tag_dyneither(_tmp6B2,sizeof(void*),0)));};_LL1D2:;}static void Cyc_CfFlowInfo_print_flow_mapping(
void*root,void*rval){{const char*_tmp6B6;void*_tmp6B5;(_tmp6B5=0,Cyc_fprintf(Cyc_stderr,((
_tmp6B6="    ",_tag_dyneither(_tmp6B6,sizeof(char),5))),_tag_dyneither(_tmp6B5,
sizeof(void*),0)));}Cyc_CfFlowInfo_print_root(root);{const char*_tmp6B9;void*
_tmp6B8;(_tmp6B8=0,Cyc_fprintf(Cyc_stderr,((_tmp6B9=" --> ",_tag_dyneither(
_tmp6B9,sizeof(char),6))),_tag_dyneither(_tmp6B8,sizeof(void*),0)));}Cyc_CfFlowInfo_print_absrval(
rval);{const char*_tmp6BC;void*_tmp6BB;(_tmp6BB=0,Cyc_fprintf(Cyc_stderr,((
_tmp6BC="\n",_tag_dyneither(_tmp6BC,sizeof(char),2))),_tag_dyneither(_tmp6BB,
sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){((
void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,
d);}void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo
_tmp438=f;int _tmp439;struct _tuple12 _tmp43A;struct Cyc_Dict_Dict _tmp43B;_LL1E4: if((
_tmp438.BottomFL).tag != 1)goto _LL1E6;_tmp439=(int)(_tmp438.BottomFL).val;_LL1E5:{
const char*_tmp6BF;void*_tmp6BE;(_tmp6BE=0,Cyc_fprintf(Cyc_stderr,((_tmp6BF="  BottomFL\n",
_tag_dyneither(_tmp6BF,sizeof(char),12))),_tag_dyneither(_tmp6BE,sizeof(void*),0)));}
goto _LL1E3;_LL1E6: if((_tmp438.ReachableFL).tag != 2)goto _LL1E3;_tmp43A=(struct
_tuple12)(_tmp438.ReachableFL).val;_tmp43B=_tmp43A.f1;_LL1E7:{const char*_tmp6C2;
void*_tmp6C1;(_tmp6C1=0,Cyc_fprintf(Cyc_stderr,((_tmp6C2="  ReachableFL:\n",
_tag_dyneither(_tmp6C2,sizeof(char),16))),_tag_dyneither(_tmp6C1,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp43B);{const char*_tmp6C5;void*_tmp6C4;(_tmp6C4=
0,Cyc_fprintf(Cyc_stderr,((_tmp6C5="\n",_tag_dyneither(_tmp6C5,sizeof(char),2))),
_tag_dyneither(_tmp6C4,sizeof(void*),0)));}goto _LL1E3;_LL1E3:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t){void*_tmp442=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp44F;struct Cyc_Absyn_DatatypeInfo _tmp451;struct Cyc_List_List*_tmp452;struct
Cyc_List_List*_tmp454;struct Cyc_Absyn_AggrInfo _tmp456;struct Cyc_List_List*
_tmp457;struct Cyc_Absyn_DatatypeFieldInfo _tmp459;struct Cyc_List_List*_tmp45A;
struct Cyc_List_List*_tmp45C;struct Cyc_Absyn_PtrInfo _tmp45E;void*_tmp45F;struct
Cyc_Absyn_PtrAtts _tmp460;void*_tmp461;struct Cyc_List_List*_tmp464;struct Cyc_List_List*
_tmp466;struct Cyc_Absyn_ArrayInfo _tmp468;void*_tmp469;void*_tmp46B;void*_tmp46D;
void*_tmp46F;void*_tmp470;void*_tmp472;_LL1E9: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp443=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp442;if(_tmp443->tag != 26)goto
_LL1EB;}_LL1EA: goto _LL1EC;_LL1EB: {struct Cyc_Absyn_VoidType_struct*_tmp444=(
struct Cyc_Absyn_VoidType_struct*)_tmp442;if(_tmp444->tag != 0)goto _LL1ED;}_LL1EC:
goto _LL1EE;_LL1ED: {struct Cyc_Absyn_IntType_struct*_tmp445=(struct Cyc_Absyn_IntType_struct*)
_tmp442;if(_tmp445->tag != 6)goto _LL1EF;}_LL1EE: goto _LL1F0;_LL1EF: {struct Cyc_Absyn_FloatType_struct*
_tmp446=(struct Cyc_Absyn_FloatType_struct*)_tmp442;if(_tmp446->tag != 7)goto
_LL1F1;}_LL1F0: goto _LL1F2;_LL1F1: {struct Cyc_Absyn_EnumType_struct*_tmp447=(
struct Cyc_Absyn_EnumType_struct*)_tmp442;if(_tmp447->tag != 13)goto _LL1F3;}_LL1F2:
goto _LL1F4;_LL1F3: {struct Cyc_Absyn_AnonEnumType_struct*_tmp448=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp442;if(_tmp448->tag != 14)goto _LL1F5;}_LL1F4: goto _LL1F6;_LL1F5: {struct Cyc_Absyn_TagType_struct*
_tmp449=(struct Cyc_Absyn_TagType_struct*)_tmp442;if(_tmp449->tag != 19)goto _LL1F7;}
_LL1F6: goto _LL1F8;_LL1F7: {struct Cyc_Absyn_HeapRgn_struct*_tmp44A=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp442;if(_tmp44A->tag != 20)goto _LL1F9;}_LL1F8: goto _LL1FA;_LL1F9: {struct Cyc_Absyn_Evar_struct*
_tmp44B=(struct Cyc_Absyn_Evar_struct*)_tmp442;if(_tmp44B->tag != 1)goto _LL1FB;}
_LL1FA: return 0;_LL1FB: {struct Cyc_Absyn_UniqueRgn_struct*_tmp44C=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp442;if(_tmp44C->tag != 21)goto _LL1FD;}_LL1FC: {void*_tmp474=rgn;_LL220: {
struct Cyc_CfFlowInfo_UniqueRgn_k_struct*_tmp475=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)
_tmp474;if(_tmp475->tag != 0)goto _LL222;}_LL221: return 1;_LL222:;_LL223: return 0;
_LL21F:;}_LL1FD: {struct Cyc_Absyn_RefCntRgn_struct*_tmp44D=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp442;if(_tmp44D->tag != 22)goto _LL1FF;}_LL1FE: {void*_tmp476=rgn;_LL225: {
struct Cyc_CfFlowInfo_RefCntRgn_k_struct*_tmp477=(struct Cyc_CfFlowInfo_RefCntRgn_k_struct*)
_tmp476;if(_tmp477->tag != 1)goto _LL227;}_LL226: return 1;_LL227:;_LL228: return 0;
_LL224:;}_LL1FF: {struct Cyc_Absyn_VarType_struct*_tmp44E=(struct Cyc_Absyn_VarType_struct*)
_tmp442;if(_tmp44E->tag != 2)goto _LL201;else{_tmp44F=_tmp44E->f1;}}_LL200: {void*
_tmp478=rgn;struct Cyc_Absyn_Tvar*_tmp47C;_LL22A: {struct Cyc_CfFlowInfo_RefCntRgn_k_struct*
_tmp479=(struct Cyc_CfFlowInfo_RefCntRgn_k_struct*)_tmp478;if(_tmp479->tag != 1)
goto _LL22C;}_LL22B: goto _LL22D;_LL22C: {struct Cyc_CfFlowInfo_UniqueRgn_k_struct*
_tmp47A=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)_tmp478;if(_tmp47A->tag != 0)
goto _LL22E;}_LL22D: return 0;_LL22E: {struct Cyc_CfFlowInfo_Region_k_struct*_tmp47B=(
struct Cyc_CfFlowInfo_Region_k_struct*)_tmp478;if(_tmp47B->tag != 2)goto _LL229;
else{_tmp47C=_tmp47B->f1;}}_LL22F: return Cyc_Absyn_tvar_cmp(_tmp44F,_tmp47C)== 0;
_LL229:;}_LL201: {struct Cyc_Absyn_DatatypeType_struct*_tmp450=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp442;if(_tmp450->tag != 3)goto _LL203;else{_tmp451=_tmp450->f1;_tmp452=_tmp451.targs;}}
_LL202: _tmp454=_tmp452;goto _LL204;_LL203: {struct Cyc_Absyn_TypedefType_struct*
_tmp453=(struct Cyc_Absyn_TypedefType_struct*)_tmp442;if(_tmp453->tag != 17)goto
_LL205;else{_tmp454=_tmp453->f2;}}_LL204: _tmp457=_tmp454;goto _LL206;_LL205: {
struct Cyc_Absyn_AggrType_struct*_tmp455=(struct Cyc_Absyn_AggrType_struct*)
_tmp442;if(_tmp455->tag != 11)goto _LL207;else{_tmp456=_tmp455->f1;_tmp457=_tmp456.targs;}}
_LL206: _tmp45A=_tmp457;goto _LL208;_LL207: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp458=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp442;if(_tmp458->tag != 4)
goto _LL209;else{_tmp459=_tmp458->f1;_tmp45A=_tmp459.targs;}}_LL208: _tmp45C=
_tmp45A;goto _LL20A;_LL209: {struct Cyc_Absyn_JoinEff_struct*_tmp45B=(struct Cyc_Absyn_JoinEff_struct*)
_tmp442;if(_tmp45B->tag != 24)goto _LL20B;else{_tmp45C=_tmp45B->f1;}}_LL20A: return((
int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(
Cyc_CfFlowInfo_contains_region,rgn,_tmp45C);_LL20B: {struct Cyc_Absyn_PointerType_struct*
_tmp45D=(struct Cyc_Absyn_PointerType_struct*)_tmp442;if(_tmp45D->tag != 5)goto
_LL20D;else{_tmp45E=_tmp45D->f1;_tmp45F=_tmp45E.elt_typ;_tmp460=_tmp45E.ptr_atts;
_tmp461=_tmp460.rgn;}}_LL20C: return Cyc_CfFlowInfo_contains_region(rgn,_tmp461)
 || Cyc_CfFlowInfo_contains_region(rgn,_tmp45F);_LL20D: {struct Cyc_Absyn_FnType_struct*
_tmp462=(struct Cyc_Absyn_FnType_struct*)_tmp442;if(_tmp462->tag != 9)goto _LL20F;}
_LL20E: return 0;_LL20F: {struct Cyc_Absyn_TupleType_struct*_tmp463=(struct Cyc_Absyn_TupleType_struct*)
_tmp442;if(_tmp463->tag != 10)goto _LL211;else{_tmp464=_tmp463->f1;}}_LL210: for(0;
_tmp464 != 0;_tmp464=_tmp464->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((
struct _tuple16*)_tmp464->hd)).f2))return 1;}return 0;_LL211: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp465=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp442;if(_tmp465->tag != 12)goto
_LL213;else{_tmp466=_tmp465->f2;}}_LL212: for(0;_tmp466 != 0;_tmp466=_tmp466->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp466->hd)->type))
return 1;}return 0;_LL213: {struct Cyc_Absyn_ArrayType_struct*_tmp467=(struct Cyc_Absyn_ArrayType_struct*)
_tmp442;if(_tmp467->tag != 8)goto _LL215;else{_tmp468=_tmp467->f1;_tmp469=_tmp468.elt_type;}}
_LL214: _tmp46B=_tmp469;goto _LL216;_LL215: {struct Cyc_Absyn_AccessEff_struct*
_tmp46A=(struct Cyc_Absyn_AccessEff_struct*)_tmp442;if(_tmp46A->tag != 23)goto
_LL217;else{_tmp46B=(void*)_tmp46A->f1;}}_LL216: _tmp46D=_tmp46B;goto _LL218;
_LL217: {struct Cyc_Absyn_RgnsEff_struct*_tmp46C=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp442;if(_tmp46C->tag != 25)goto _LL219;else{_tmp46D=(void*)_tmp46C->f1;}}_LL218:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp46D);_LL219: {struct Cyc_Absyn_DynRgnType_struct*
_tmp46E=(struct Cyc_Absyn_DynRgnType_struct*)_tmp442;if(_tmp46E->tag != 16)goto
_LL21B;else{_tmp46F=(void*)_tmp46E->f1;_tmp470=(void*)_tmp46E->f2;}}_LL21A:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp46F) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp470);_LL21B: {struct Cyc_Absyn_RgnHandleType_struct*_tmp471=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp442;if(_tmp471->tag != 15)goto _LL21D;else{_tmp472=(void*)_tmp471->f1;}}_LL21C:
return 0;_LL21D: {struct Cyc_Absyn_ValueofType_struct*_tmp473=(struct Cyc_Absyn_ValueofType_struct*)
_tmp442;if(_tmp473->tag != 18)goto _LL1E8;}_LL21E: return 0;_LL1E8:;}struct _tuple24{
struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(
struct _tuple24*env,void*root,void*rval){struct _tuple24 _tmp47E;struct Cyc_CfFlowInfo_FlowEnv*
_tmp47F;struct Cyc_Dict_Dict _tmp480;struct Cyc_Dict_Dict*_tmp481;void*_tmp482;
struct _tuple24*_tmp47D=env;_tmp47E=*_tmp47D;_tmp47F=_tmp47E.f1;_tmp480=_tmp47E.f2;
_tmp481=(struct Cyc_Dict_Dict*)&(*_tmp47D).f2;_tmp482=_tmp47E.f3;{void*_tmp483=
root;struct Cyc_Absyn_Vardecl*_tmp485;void*_tmp487;_LL231: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp484=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp483;if(_tmp484->tag != 0)goto
_LL233;else{_tmp485=_tmp484->f1;}}_LL232: if(Cyc_CfFlowInfo_contains_region(
_tmp482,_tmp485->type))rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp47F,_tmp485->type,
0,_tmp47F->unknown_none);*_tmp481=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(*_tmp481,root,rval);goto _LL230;_LL233: {struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp486=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp483;if(_tmp486->tag != 1)goto _LL235;else{_tmp487=(void*)_tmp486->f2;}}_LL234:
if(!Cyc_CfFlowInfo_contains_region(_tmp482,_tmp487))*_tmp481=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp481,root,rval);goto
_LL230;_LL235: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp488=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp483;if(_tmp488->tag != 2)goto _LL230;}_LL236: goto _LL230;_LL230:;};}static
struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp489=Cyc_Tcutil_compress(rgn);
struct Cyc_Absyn_Tvar*_tmp48B;_LL238: {struct Cyc_Absyn_VarType_struct*_tmp48A=(
struct Cyc_Absyn_VarType_struct*)_tmp489;if(_tmp48A->tag != 2)goto _LL23A;else{
_tmp48B=_tmp48A->f1;}}_LL239: {struct Cyc_CfFlowInfo_Region_k_struct _tmp6C6;
struct Cyc_CfFlowInfo_Region_k_struct v=(_tmp6C6.tag=2,((_tmp6C6.f1=_tmp48B,
_tmp6C6)));void*v2=(void*)& v;struct _tuple24 _tmp6C7;struct _tuple24 env=(_tmp6C7.f1=
fenv,((_tmp6C7.f2=fenv->mt_flowdict,((_tmp6C7.f3=v2,_tmp6C7)))));((void(*)(void(*
f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL23A: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp48C=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp489;if(_tmp48C->tag != 21)goto
_LL23C;}_LL23B: {struct _tuple24 _tmp6C8;struct _tuple24 env=(_tmp6C8.f1=fenv,((
_tmp6C8.f2=fenv->mt_flowdict,((_tmp6C8.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,
_tmp6C8)))));((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return
env.f2;}_LL23C: {struct Cyc_Absyn_RefCntRgn_struct*_tmp48D=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp489;if(_tmp48D->tag != 22)goto _LL23E;}_LL23D: {struct _tuple24 _tmp6C9;struct
_tuple24 env=(_tmp6C9.f1=fenv,((_tmp6C9.f2=fenv->mt_flowdict,((_tmp6C9.f3=(void*)&
Cyc_CfFlowInfo_RefCntRgn_k_val,_tmp6C9)))));((void(*)(void(*f)(struct _tuple24*,
void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,&
env,fd);return env.f2;}_LL23E:;_LL23F: {struct Cyc_Core_Impossible_struct _tmp6CF;
const char*_tmp6CE;struct Cyc_Core_Impossible_struct*_tmp6CD;(int)_throw((void*)((
_tmp6CD=_cycalloc(sizeof(*_tmp6CD)),((_tmp6CD[0]=((_tmp6CF.tag=Cyc_Core_Impossible,((
_tmp6CF.f1=((_tmp6CE="kill_flowdict_region",_tag_dyneither(_tmp6CE,sizeof(char),
21))),_tmp6CF)))),_tmp6CD)))));}_LL237:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){union
Cyc_CfFlowInfo_FlowInfo _tmp495=f;int _tmp496;struct _tuple12 _tmp497;struct Cyc_Dict_Dict
_tmp498;struct Cyc_List_List*_tmp499;_LL241: if((_tmp495.BottomFL).tag != 1)goto
_LL243;_tmp496=(int)(_tmp495.BottomFL).val;_LL242: return f;_LL243: if((_tmp495.ReachableFL).tag
!= 2)goto _LL240;_tmp497=(struct _tuple12)(_tmp495.ReachableFL).val;_tmp498=
_tmp497.f1;_tmp499=_tmp497.f2;_LL244: {struct Cyc_Dict_Dict _tmp49A=Cyc_CfFlowInfo_kill_flowdict_region(
fenv,_tmp498,rgn);struct Cyc_List_List*_tmp49B=Cyc_CfFlowInfo_kill_relns_region(
_tmp499,rgn);return Cyc_CfFlowInfo_ReachableFL(_tmp49A,_tmp49B);}_LL240:;}
