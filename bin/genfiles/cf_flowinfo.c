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
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(int,int);
extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,
unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * 
sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
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
l2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_rempty(struct
_RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,
void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void Cyc_Dict_iter(void(*f)(
void*,void*),struct Cyc_Dict_Dict d);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(
void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;}
;struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
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
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
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
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{
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
struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};
struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple7 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
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
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple8*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
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
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{
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
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);int
Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple10{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple10 val;};struct _union_RelnOp_LessNumelts{
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
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11
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
env_iteration,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
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
struct _tuple12{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,
struct _tuple12 pr2);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union
Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};extern struct Cyc_CfFlowInfo_UniqueRgn_k_struct
Cyc_CfFlowInfo_UniqueRgn_k_val;int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*
place);struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
struct Cyc_CfFlowInfo_UniqueRgn_k_struct Cyc_CfFlowInfo_UniqueRgn_k_val={0};union
Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){union
Cyc_CfFlowInfo_AbsLVal _tmp46F;return((_tmp46F.PlaceL).val=x,(((_tmp46F.PlaceL).tag=
1,_tmp46F)));}union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){union Cyc_CfFlowInfo_AbsLVal
_tmp470;return((_tmp470.UnknownL).val=0,(((_tmp470.UnknownL).tag=2,_tmp470)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo
_tmp471;return((_tmp471.BottomFL).val=0,(((_tmp471.BottomFL).tag=1,_tmp471)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,
struct Cyc_List_List*r){struct _tuple11 _tmp474;union Cyc_CfFlowInfo_FlowInfo _tmp473;
return((_tmp473.ReachableFL).val=((_tmp474.f1=fd,((_tmp474.f2=r,_tmp474)))),(((
_tmp473.ReachableFL).tag=2,_tmp473)));}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,{.Null_c={1,0}}};static
struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp4AB;struct Cyc_CfFlowInfo_MallocPt_struct _tmp4AA;
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp4A9;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4A8;struct Cyc_CfFlowInfo_Esc_struct _tmp4A7;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4A6;struct Cyc_CfFlowInfo_Esc_struct _tmp4A5;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4A4;struct Cyc_CfFlowInfo_Esc_struct _tmp4A3;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4A2;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4A1;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4A0;struct Cyc_CfFlowInfo_UnknownR_struct _tmp49F;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp49E;struct Cyc_CfFlowInfo_UnknownR_struct _tmp49D;struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp49C;struct Cyc_CfFlowInfo_NotZeroThis_struct _tmp49B;struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp49A;struct Cyc_CfFlowInfo_NotZeroAll_struct _tmp499;struct Cyc_CfFlowInfo_Zero_struct*
_tmp498;struct Cyc_CfFlowInfo_Zero_struct _tmp497;struct Cyc_CfFlowInfo_FlowEnv*
_tmp496;return(_tmp496=_region_malloc(r,sizeof(*_tmp496)),((_tmp496->r=r,((
_tmp496->zero=(void*)((_tmp498=_region_malloc(r,sizeof(*_tmp498)),((_tmp498[0]=((
_tmp497.tag=0,_tmp497)),_tmp498)))),((_tmp496->notzeroall=(void*)((_tmp49A=
_region_malloc(r,sizeof(*_tmp49A)),((_tmp49A[0]=((_tmp499.tag=1,_tmp499)),
_tmp49A)))),((_tmp496->notzerothis=(void*)((_tmp49C=_region_malloc(r,sizeof(*
_tmp49C)),((_tmp49C[0]=((_tmp49B.tag=2,_tmp49B)),_tmp49C)))),((_tmp496->unknown_none=(
void*)((_tmp49E=_region_malloc(r,sizeof(*_tmp49E)),((_tmp49E[0]=((_tmp49D.tag=3,((
_tmp49D.f1=Cyc_CfFlowInfo_NoneIL,_tmp49D)))),_tmp49E)))),((_tmp496->unknown_this=(
void*)((_tmp4A0=_region_malloc(r,sizeof(*_tmp4A0)),((_tmp4A0[0]=((_tmp49F.tag=3,((
_tmp49F.f1=Cyc_CfFlowInfo_ThisIL,_tmp49F)))),_tmp4A0)))),((_tmp496->unknown_all=(
void*)((_tmp4A2=_region_malloc(r,sizeof(*_tmp4A2)),((_tmp4A2[0]=((_tmp4A1.tag=3,((
_tmp4A1.f1=Cyc_CfFlowInfo_AllIL,_tmp4A1)))),_tmp4A2)))),((_tmp496->esc_none=(
void*)((_tmp4A4=_region_malloc(r,sizeof(*_tmp4A4)),((_tmp4A4[0]=((_tmp4A3.tag=4,((
_tmp4A3.f1=Cyc_CfFlowInfo_NoneIL,_tmp4A3)))),_tmp4A4)))),((_tmp496->esc_this=(
void*)((_tmp4A6=_region_malloc(r,sizeof(*_tmp4A6)),((_tmp4A6[0]=((_tmp4A5.tag=4,((
_tmp4A5.f1=Cyc_CfFlowInfo_ThisIL,_tmp4A5)))),_tmp4A6)))),((_tmp496->esc_all=(
void*)((_tmp4A8=_region_malloc(r,sizeof(*_tmp4A8)),((_tmp4A8[0]=((_tmp4A7.tag=4,((
_tmp4A7.f1=Cyc_CfFlowInfo_AllIL,_tmp4A7)))),_tmp4A8)))),((_tmp496->mt_flowdict=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(
r,Cyc_CfFlowInfo_root_cmp),((_tmp496->mt_place_set=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp496->dummy_place=((_tmp4AB=
_region_malloc(r,sizeof(*_tmp4AB)),((_tmp4AB->root=(void*)((_tmp4A9=
_region_malloc(r,sizeof(*_tmp4A9)),((_tmp4A9[0]=((_tmp4AA.tag=1,((_tmp4AA.f1=&
dummy_exp,((_tmp4AA.f2=(void*)((void*)& Cyc_Absyn_VoidType_val),_tmp4AA)))))),
_tmp4A9)))),((_tmp4AB->fields=0,_tmp4AB)))))),_tmp496)))))))))))))))))))))))))));}
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*
loc){if(set != 0){if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,
place))return 1;else{*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,
struct Cyc_Position_Segment*v))Cyc_Dict_insert)(*set,place,loc);}}return 0;}struct
_dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
void*_tmp1F;struct Cyc_List_List*_tmp20;struct Cyc_CfFlowInfo_Place _tmp1E=*place;
_tmp1F=_tmp1E.root;_tmp20=_tmp1E.fields;{void*_tmp21=_tmp1F;struct Cyc_Absyn_Vardecl*
_tmp23;_LL1: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp22=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp21;if(_tmp22->tag != 0)goto _LL3;else{_tmp23=_tmp22->f1;}}_LL2: if(_tmp20 == 0)
return Cyc_Absynpp_qvar2string(_tmp23->name);else{const char*_tmp4AF;void*_tmp4AE[
1];struct Cyc_String_pa_struct _tmp4AD;return(struct _dyneither_ptr)((_tmp4AD.tag=0,((
_tmp4AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp23->name)),((_tmp4AE[0]=& _tmp4AD,Cyc_aprintf(((_tmp4AF="reachable from %s",
_tag_dyneither(_tmp4AF,sizeof(char),18))),_tag_dyneither(_tmp4AE,sizeof(void*),1))))))));}
_LL3:;_LL4: {const char*_tmp4B2;void*_tmp4B1;(_tmp4B1=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4B2="error locations not for VarRoots",
_tag_dyneither(_tmp4B2,sizeof(char),33))),_tag_dyneither(_tmp4B1,sizeof(void*),0)));}
_LL0:;};}static struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(void*
env,void*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*loc2){
return loc1;}struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2,int disjoint){int _tmp29=0;struct Cyc_Dict_Dict _tmp2A=((
struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*f)(int*,struct Cyc_CfFlowInfo_Place*,
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*),int*env,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(
int*env,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc1,struct
Cyc_Position_Segment*loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp29,s1,s2);
return _tmp2A;}struct _tuple13{void*f1;struct Cyc_Position_Segment*f2;};int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2B;_push_handler(& _tmp2B);{int _tmp2D=0;if(setjmp(_tmp2B.handler))
_tmp2D=1;if(!_tmp2D){{struct _RegionHandle _tmp2E=_new_region("temp");struct
_RegionHandle*temp=& _tmp2E;_push_region(temp);{struct Cyc_Iter_Iter iter=((struct
Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
temp,s1);struct _tuple13 _tmp2F=*((struct _tuple13*(*)(struct _RegionHandle*,struct
Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple13*))Cyc_Iter_next)(iter,& _tmp2F)){void*_tmp30=_tmp2F.f1;if(!((int(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp30)){int _tmp31=0;
_npop_handler(1);return _tmp31;}}{int _tmp32=1;_npop_handler(1);return _tmp32;};};
_pop_region(temp);};_pop_handler();}else{void*_tmp2C=(void*)_exn_thrown;void*
_tmp34=_tmp2C;_LL6:;_LL7: return 1;_LL8:;_LL9:(void)_throw(_tmp34);_LL5:;}};};}int
Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct
_dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=fs->tl){struct _dyneither_ptr*
_tmp35=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;if(Cyc_strptrcmp(_tmp35,f)== 0)
return n;n=n + 1;}{struct Cyc_Core_Impossible_struct _tmp4BF;const char*_tmp4BE;void*
_tmp4BD[1];struct Cyc_String_pa_struct _tmp4BC;struct Cyc_Core_Impossible_struct*
_tmp4BB;(int)_throw((void*)((_tmp4BB=_cycalloc(sizeof(*_tmp4BB)),((_tmp4BB[0]=((
_tmp4BF.tag=Cyc_Core_Impossible,((_tmp4BF.f1=(struct _dyneither_ptr)((_tmp4BC.tag=
0,((_tmp4BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp4BD[0]=&
_tmp4BC,Cyc_aprintf(((_tmp4BE="get_field_index_fs failed: %s",_tag_dyneither(
_tmp4BE,sizeof(char),30))),_tag_dyneither(_tmp4BD,sizeof(void*),1)))))))),
_tmp4BF)))),_tmp4BB)))));};}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f){void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp3D;union Cyc_Absyn_AggrInfoU _tmp3E;struct Cyc_List_List*_tmp40;_LLB: {struct
Cyc_Absyn_AggrType_struct*_tmp3C=(struct Cyc_Absyn_AggrType_struct*)_tmp3B;if(
_tmp3C->tag != 12)goto _LLD;else{_tmp3D=_tmp3C->f1;_tmp3E=_tmp3D.aggr_info;}}_LLC: {
struct Cyc_List_List*_tmp41=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(
_tmp3E))->impl))->fields;_tmp40=_tmp41;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp3F=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp3B;if(_tmp3F->tag != 13)goto _LLF;
else{_tmp40=_tmp3F->f2;}}_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp40,f);
_LLF:;_LL10: {struct Cyc_Core_Impossible_struct _tmp4CC;const char*_tmp4CB;void*
_tmp4CA[1];struct Cyc_String_pa_struct _tmp4C9;struct Cyc_Core_Impossible_struct*
_tmp4C8;(int)_throw((void*)((_tmp4C8=_cycalloc(sizeof(*_tmp4C8)),((_tmp4C8[0]=((
_tmp4CC.tag=Cyc_Core_Impossible,((_tmp4CC.f1=(struct _dyneither_ptr)((_tmp4C9.tag=
0,((_tmp4C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp4CA[0]=& _tmp4C9,Cyc_aprintf(((_tmp4CB="get_field_index failed: %s",
_tag_dyneither(_tmp4CB,sizeof(char),27))),_tag_dyneither(_tmp4CA,sizeof(void*),1)))))))),
_tmp4CC)))),_tmp4C8)))));}_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((
int)r1 == (int)r2)return 0;{struct _tuple0 _tmp4CD;struct _tuple0 _tmp48=(_tmp4CD.f1=
r1,((_tmp4CD.f2=r2,_tmp4CD)));void*_tmp49;struct Cyc_Absyn_Vardecl*_tmp4B;void*
_tmp4C;struct Cyc_Absyn_Vardecl*_tmp4E;void*_tmp4F;void*_tmp51;void*_tmp53;struct
Cyc_Absyn_Exp*_tmp55;void*_tmp56;struct Cyc_Absyn_Exp*_tmp58;void*_tmp59;void*
_tmp5B;void*_tmp5D;int _tmp5F;void*_tmp60;int _tmp62;_LL12: _tmp49=_tmp48.f1;{
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp4A=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp49;if(_tmp4A->tag != 0)goto _LL14;else{_tmp4B=_tmp4A->f1;}};_tmp4C=_tmp48.f2;{
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp4D=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp4C;if(_tmp4D->tag != 0)goto _LL14;else{_tmp4E=_tmp4D->f1;}};_LL13: return(int)
_tmp4B - (int)_tmp4E;_LL14: _tmp4F=_tmp48.f1;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp50=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp4F;if(_tmp50->tag != 0)goto _LL16;};
_LL15: return - 1;_LL16: _tmp51=_tmp48.f2;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp52=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp51;if(_tmp52->tag != 0)goto _LL18;};
_LL17: return 1;_LL18: _tmp53=_tmp48.f1;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp54=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp53;if(_tmp54->tag != 1)goto
_LL1A;else{_tmp55=_tmp54->f1;}};_tmp56=_tmp48.f2;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp57=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp56;if(_tmp57->tag != 1)goto
_LL1A;else{_tmp58=_tmp57->f1;}};_LL19: return(int)_tmp55 - (int)_tmp58;_LL1A:
_tmp59=_tmp48.f1;{struct Cyc_CfFlowInfo_MallocPt_struct*_tmp5A=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp59;if(_tmp5A->tag != 1)goto _LL1C;};_LL1B: return - 1;_LL1C: _tmp5B=_tmp48.f2;{
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp5C=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp5B;if(_tmp5C->tag != 1)goto _LL1E;};_LL1D: return 1;_LL1E: _tmp5D=_tmp48.f1;{
struct Cyc_CfFlowInfo_InitParam_struct*_tmp5E=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp5D;if(_tmp5E->tag != 2)goto _LL11;else{_tmp5F=_tmp5E->f1;}};_tmp60=_tmp48.f2;{
struct Cyc_CfFlowInfo_InitParam_struct*_tmp61=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp60;if(_tmp61->tag != 2)goto _LL11;else{_tmp62=_tmp61->f1;}};_LL1F: return _tmp5F
- _tmp62;_LL11:;};}static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,
struct Cyc_List_List*list2){return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}int
Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*
p2){if((int)p1 == (int)p2)return 0;{void*_tmp64;struct Cyc_List_List*_tmp65;struct
Cyc_CfFlowInfo_Place _tmp63=*p1;_tmp64=_tmp63.root;_tmp65=_tmp63.fields;{void*
_tmp67;struct Cyc_List_List*_tmp68;struct Cyc_CfFlowInfo_Place _tmp66=*p2;_tmp67=
_tmp66.root;_tmp68=_tmp66.fields;{int i=Cyc_CfFlowInfo_root_cmp(_tmp64,_tmp67);
if(i != 0)return i;return Cyc_CfFlowInfo_intlist_cmp(_tmp65,_tmp68);};};};}static
struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_List_List*sl=0;void*_tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_CfFlowInfo_Place
_tmp69=*p;_tmp6A=_tmp69.root;_tmp6B=_tmp69.fields;{void*_tmp6C=_tmp6A;struct Cyc_Absyn_Vardecl*
_tmp6E;struct Cyc_Absyn_Exp*_tmp70;int _tmp72;_LL21: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp6D=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp6C;if(_tmp6D->tag != 0)goto _LL23;
else{_tmp6E=_tmp6D->f1;}}_LL22:{struct _dyneither_ptr*_tmp4DA;const char*_tmp4D9;
void*_tmp4D8[1];struct Cyc_String_pa_struct _tmp4D7;struct Cyc_List_List*_tmp4D6;sl=((
_tmp4D6=_cycalloc(sizeof(*_tmp4D6)),((_tmp4D6->hd=((_tmp4DA=_cycalloc(sizeof(*
_tmp4DA)),((_tmp4DA[0]=((_tmp4D7.tag=0,((_tmp4D7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp6E->name).f2),((_tmp4D8[0]=& _tmp4D7,Cyc_aprintf(((
_tmp4D9="%s",_tag_dyneither(_tmp4D9,sizeof(char),3))),_tag_dyneither(_tmp4D8,
sizeof(void*),1)))))))),_tmp4DA)))),((_tmp4D6->tl=sl,_tmp4D6))))));}goto _LL20;
_LL23: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp6F=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp6C;if(_tmp6F->tag != 1)goto _LL25;else{_tmp70=_tmp6F->f1;}}_LL24:{struct
_dyneither_ptr*_tmp4E7;const char*_tmp4E6;void*_tmp4E5[1];struct Cyc_Int_pa_struct
_tmp4E4;struct Cyc_List_List*_tmp4E3;sl=((_tmp4E3=_cycalloc(sizeof(*_tmp4E3)),((
_tmp4E3->hd=((_tmp4E7=_cycalloc(sizeof(*_tmp4E7)),((_tmp4E7[0]=((_tmp4E4.tag=1,((
_tmp4E4.f1=(unsigned long)((int)_tmp70),((_tmp4E5[0]=& _tmp4E4,Cyc_aprintf(((
_tmp4E6="mpt%d",_tag_dyneither(_tmp4E6,sizeof(char),6))),_tag_dyneither(_tmp4E5,
sizeof(void*),1)))))))),_tmp4E7)))),((_tmp4E3->tl=sl,_tmp4E3))))));}goto _LL20;
_LL25: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp71=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp6C;if(_tmp71->tag != 2)goto _LL20;else{_tmp72=_tmp71->f1;}}_LL26:{struct
_dyneither_ptr*_tmp4F4;const char*_tmp4F3;void*_tmp4F2[1];struct Cyc_Int_pa_struct
_tmp4F1;struct Cyc_List_List*_tmp4F0;sl=((_tmp4F0=_cycalloc(sizeof(*_tmp4F0)),((
_tmp4F0->hd=((_tmp4F4=_cycalloc(sizeof(*_tmp4F4)),((_tmp4F4[0]=((_tmp4F1.tag=1,((
_tmp4F1.f1=(unsigned long)_tmp72,((_tmp4F2[0]=& _tmp4F1,Cyc_aprintf(((_tmp4F3="param%d",
_tag_dyneither(_tmp4F3,sizeof(char),8))),_tag_dyneither(_tmp4F2,sizeof(void*),1)))))))),
_tmp4F4)))),((_tmp4F0->tl=sl,_tmp4F0))))));}goto _LL20;_LL20:;}for(0;_tmp6B != 0;
_tmp6B=_tmp6B->tl){struct _dyneither_ptr*_tmp501;const char*_tmp500;void*_tmp4FF[1];
struct Cyc_Int_pa_struct _tmp4FE;struct Cyc_List_List*_tmp4FD;sl=((_tmp4FD=
_cycalloc(sizeof(*_tmp4FD)),((_tmp4FD->hd=((_tmp501=_cycalloc(sizeof(*_tmp501)),((
_tmp501[0]=((_tmp4FE.tag=1,((_tmp4FE.f1=(unsigned long)((int)_tmp6B->hd),((
_tmp4FF[0]=& _tmp4FE,Cyc_aprintf(((_tmp500="%d",_tag_dyneither(_tmp500,sizeof(
char),3))),_tag_dyneither(_tmp4FF,sizeof(void*),1)))))))),_tmp501)))),((_tmp4FD->tl=
sl,_tmp4FD))))));}{struct Cyc_String_pa_struct _tmp50C;const char*_tmp50B;void*
_tmp50A[1];const char*_tmp509;struct _dyneither_ptr*_tmp508;struct _dyneither_ptr*
_tmp87=(_tmp508=_cycalloc(sizeof(*_tmp508)),((_tmp508[0]=((_tmp50C.tag=0,((
_tmp50C.f1=(struct _dyneither_ptr)((_tmp50B="",_tag_dyneither(_tmp50B,sizeof(char),
1))),((_tmp50A[0]=& _tmp50C,Cyc_aprintf(((_tmp509="%s",_tag_dyneither(_tmp509,
sizeof(char),3))),_tag_dyneither(_tmp50A,sizeof(void*),1)))))))),_tmp508)));for(
0;sl != 0;sl=sl->tl){const char*_tmp511;void*_tmp510[2];struct Cyc_String_pa_struct
_tmp50F;struct Cyc_String_pa_struct _tmp50E;*_tmp87=((_tmp50E.tag=0,((_tmp50E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp87),((_tmp50F.tag=0,((_tmp50F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((
_tmp510[0]=& _tmp50F,((_tmp510[1]=& _tmp50E,Cyc_aprintf(((_tmp511="%s.%s",
_tag_dyneither(_tmp511,sizeof(char),6))),_tag_dyneither(_tmp510,sizeof(void*),2))))))))))))));}
return _tmp87;};}static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct
_dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp517(
struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp516,unsigned int*_tmp515,
void***_tmp513){for(*_tmp516=0;*_tmp516 < *_tmp515;(*_tmp516)++){(*_tmp513)[*
_tmp516]=(*fenv)->unknown_all;}}struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,
void*leafval){unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
fs);unsigned int _tmp516;unsigned int _tmp515;struct _dyneither_ptr _tmp514;void**
_tmp513;unsigned int _tmp512;struct _dyneither_ptr d=(_tmp512=sz,((_tmp513=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp512)),((_tmp514=
_tag_dyneither(_tmp513,sizeof(void*),_tmp512),((((_tmp515=_tmp512,_tmp517(& fenv,&
_tmp516,& _tmp515,& _tmp513))),_tmp514)))))));{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield _tmp92;struct _dyneither_ptr*_tmp93;void*_tmp94;struct
Cyc_Absyn_Aggrfield*_tmp91=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)
_check_null(fs))->hd;_tmp92=*_tmp91;_tmp93=_tmp92.name;_tmp94=_tmp92.type;if(
_get_dyneither_size(*_tmp93,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,no_init_bits_only,_tmp94,leafval);}}return d;}struct _tuple14{struct
_RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(
struct _tuple14*env,struct Cyc_Absyn_Aggrfield*f){struct _tuple14 _tmp9A;struct
_RegionHandle*_tmp9B;struct Cyc_List_List*_tmp9C;struct _tuple14*_tmp99=env;_tmp9A=*
_tmp99;_tmp9B=_tmp9A.f1;_tmp9C=_tmp9A.f2;{void*_tmp9D=Cyc_Tcutil_rsubstitute(
_tmp9B,_tmp9C,f->type);struct Cyc_Absyn_Aggrfield*_tmp518;return(_tmp518=
_region_malloc(_tmp9B,sizeof(*_tmp518)),((_tmp518->name=f->name,((_tmp518->tq=f->tq,((
_tmp518->type=_tmp9D,((_tmp518->width=f->width,((_tmp518->attributes=f->attributes,
_tmp518)))))))))));};}static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*
targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){struct
_RegionHandle _tmp9F=_new_region("temp");struct _RegionHandle*temp=& _tmp9F;
_push_region(temp);{struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,tvs,targs);struct _tuple14 _tmp519;struct _tuple14
_tmpA0=(_tmp519.f1=temp,((_tmp519.f2=inst,_tmp519)));struct Cyc_List_List*subs_fs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(
struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmpA0,fs);struct
_dyneither_ptr _tmpA1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,
no_init_bits_only,leafval);_npop_handler(0);return _tmpA1;};_pop_region(temp);}
struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,
void*leafval);static void _tmp51F(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*
_tmp51E,unsigned int*_tmp51D,void***_tmp51B){for(*_tmp51E=0;*_tmp51E < *_tmp51D;(*
_tmp51E)++){(*_tmp51B)[*_tmp51E]=(*fenv)->unknown_all;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,
void*leafval){if(Cyc_Absyn_is_nontagged_union_type(t))return fenv->unknown_all;{
void*_tmpA3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_DatatypeFieldInfo _tmpA5;union
Cyc_Absyn_DatatypeFieldInfoU _tmpA6;struct _tuple2 _tmpA7;struct Cyc_Absyn_Datatypefield*
_tmpA8;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_AggrInfo _tmpAC;union Cyc_Absyn_AggrInfoU
_tmpAD;struct Cyc_List_List*_tmpAE;enum Cyc_Absyn_AggrKind _tmpB0;struct Cyc_List_List*
_tmpB1;struct Cyc_Absyn_ArrayInfo _tmpB3;void*_tmpB4;union Cyc_Absyn_Constraint*
_tmpB5;void*_tmpB7;struct Cyc_Absyn_PtrInfo _tmpB9;struct Cyc_Absyn_PtrAtts _tmpBA;
union Cyc_Absyn_Constraint*_tmpBB;_LL28: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpA4=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpA3;if(_tmpA4->tag != 4)goto
_LL2A;else{_tmpA5=_tmpA4->f1;_tmpA6=_tmpA5.field_info;if((_tmpA6.KnownDatatypefield).tag
!= 2)goto _LL2A;_tmpA7=(struct _tuple2)(_tmpA6.KnownDatatypefield).val;_tmpA8=
_tmpA7.f2;}}_LL29: if(_tmpA8->typs == 0)return leafval;_tmpAA=_tmpA8->typs;goto
_LL2B;_LL2A: {struct Cyc_Absyn_TupleType_struct*_tmpA9=(struct Cyc_Absyn_TupleType_struct*)
_tmpA3;if(_tmpA9->tag != 11)goto _LL2C;else{_tmpAA=_tmpA9->f1;}}_LL2B: {
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpAA);unsigned int _tmp51E;unsigned int _tmp51D;struct _dyneither_ptr _tmp51C;void**
_tmp51B;unsigned int _tmp51A;struct _dyneither_ptr d=(_tmp51A=sz,((_tmp51B=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp51A)),((_tmp51C=
_tag_dyneither(_tmp51B,sizeof(void*),_tmp51A),((((_tmp51D=_tmp51A,_tmp51F(& fenv,&
_tmp51E,& _tmp51D,& _tmp51B))),_tmp51C)))))));{int i=0;for(0;i < sz;++ i){*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,no_init_bits_only,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(
_tmpAA))->hd)).f2,leafval);_tmpAA=_tmpAA->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp525;struct Cyc_CfFlowInfo_UnionRInfo _tmp524;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp523;return(void*)((_tmp523=_region_malloc(fenv->r,sizeof(*_tmp523)),((
_tmp523[0]=((_tmp525.tag=6,((_tmp525.f1=((_tmp524.is_union=0,((_tmp524.fieldnum=
- 1,_tmp524)))),((_tmp525.f2=d,_tmp525)))))),_tmp523))));};}_LL2C: {struct Cyc_Absyn_AggrType_struct*
_tmpAB=(struct Cyc_Absyn_AggrType_struct*)_tmpA3;if(_tmpAB->tag != 12)goto _LL2E;
else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAC.aggr_info;_tmpAE=_tmpAC.targs;}}_LL2D: {
struct Cyc_Absyn_Aggrdecl*_tmpC3=Cyc_Absyn_get_known_aggrdecl(_tmpAD);if(_tmpC3->impl
== 0)goto _LL27;{struct Cyc_List_List*_tmpC4=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpC3->impl))->fields;if(_tmpAE == 0){_tmpB0=_tmpC3->kind;_tmpB1=
_tmpC4;goto _LL2F;}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp52B;struct Cyc_CfFlowInfo_UnionRInfo
_tmp52A;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp529;return(void*)((_tmp529=
_region_malloc(fenv->r,sizeof(*_tmp529)),((_tmp529[0]=((_tmp52B.tag=6,((_tmp52B.f1=((
_tmp52A.is_union=_tmpC3->kind == Cyc_Absyn_UnionA,((_tmp52A.fieldnum=- 1,_tmp52A)))),((
_tmp52B.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpC3->tvs,
_tmpAE,_tmpC4,_tmpC3->kind == Cyc_Absyn_UnionA,leafval),_tmp52B)))))),_tmp529))));};};}
_LL2E: {struct Cyc_Absyn_AnonAggrType_struct*_tmpAF=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA3;if(_tmpAF->tag != 13)goto _LL30;else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpAF->f2;}}
_LL2F: {struct Cyc_CfFlowInfo_Aggregate_struct _tmp531;struct Cyc_CfFlowInfo_UnionRInfo
_tmp530;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp52F;return(void*)((_tmp52F=
_region_malloc(fenv->r,sizeof(*_tmp52F)),((_tmp52F[0]=((_tmp531.tag=6,((_tmp531.f1=((
_tmp530.is_union=_tmpB0 == Cyc_Absyn_UnionA,((_tmp530.fieldnum=- 1,_tmp530)))),((
_tmp531.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpB1,_tmpB0 == Cyc_Absyn_UnionA,
leafval),_tmp531)))))),_tmp52F))));}_LL30:{struct Cyc_Absyn_ArrayType_struct*
_tmpB2=(struct Cyc_Absyn_ArrayType_struct*)_tmpA3;if(_tmpB2->tag != 9)goto _LL32;
else{_tmpB3=_tmpB2->f1;_tmpB4=_tmpB3.elt_type;_tmpB5=_tmpB3.zero_term;}}if(!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB5))goto
_LL32;_LL31: return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(
_tmpB4)?fenv->unknown_all: leafval;_LL32: {struct Cyc_Absyn_TagType_struct*_tmpB6=(
struct Cyc_Absyn_TagType_struct*)_tmpA3;if(_tmpB6->tag != 20)goto _LL34;else{_tmpB7=(
void*)_tmpB6->f1;}}_LL33: return leafval;_LL34:{struct Cyc_Absyn_PointerType_struct*
_tmpB8=(struct Cyc_Absyn_PointerType_struct*)_tmpA3;if(_tmpB8->tag != 5)goto _LL36;
else{_tmpB9=_tmpB8->f1;_tmpBA=_tmpB9.ptr_atts;_tmpBB=_tmpBA.nullable;}}if(!(!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBB)))goto
_LL36;_LL35:{void*_tmpCB=leafval;enum Cyc_CfFlowInfo_InitLevel _tmpCD;enum Cyc_CfFlowInfo_InitLevel
_tmpCF;_LL39: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpCC=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpCB;if(_tmpCC->tag != 3)goto _LL3B;else{_tmpCD=_tmpCC->f1;if(_tmpCD != Cyc_CfFlowInfo_ThisIL)
goto _LL3B;}}_LL3A: return fenv->notzerothis;_LL3B: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpCE=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpCB;if(_tmpCE->tag != 3)goto
_LL3D;else{_tmpCF=_tmpCE->f1;if(_tmpCF != Cyc_CfFlowInfo_AllIL)goto _LL3D;}}_LL3C:
return fenv->notzeroall;_LL3D:;_LL3E: goto _LL38;_LL38:;}goto _LL27;_LL36:;_LL37:
goto _LL27;_LL27:;}return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all:
leafval;}void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
void*t,int no_init_bits_only,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,1,no_init_bits_only,t,leafval);}int Cyc_CfFlowInfo_is_unique_consumed(struct
Cyc_Absyn_Exp*e,int env_iteration,void*r){void*_tmpD0=r;struct Cyc_Absyn_Exp*
_tmpD2;int _tmpD3;void*_tmpD4;struct _dyneither_ptr _tmpD6;_LL40: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmpD1=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmpD0;if(_tmpD1->tag != 7)goto
_LL42;else{_tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;_tmpD4=(void*)_tmpD1->f3;}}_LL41:
return !(_tmpD2 == e  && _tmpD3 == env_iteration);_LL42: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpD5=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpD0;if(_tmpD5->tag != 6)goto
_LL44;else{_tmpD6=_tmpD5->f2;}}_LL43: {unsigned int _tmpD7=_get_dyneither_size(
_tmpD6,sizeof(void*));{int i=0;for(0;i < _tmpD7;++ i){if(Cyc_CfFlowInfo_is_unique_consumed(
e,env_iteration,*((void**)_check_dyneither_subscript(_tmpD6,sizeof(void*),i))))
return 1;}}return 0;}_LL44:;_LL45: return 0;_LL3F:;}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r);static void _tmp53B(struct Cyc_CfFlowInfo_FlowEnv**fenv,
unsigned int*_tmp53A,unsigned int*_tmp539,void***_tmp537){for(*_tmp53A=0;*
_tmp53A < *_tmp539;(*_tmp53A)++){(*_tmp537)[*_tmp53A]=(*fenv)->unknown_all;}}
static void _tmp544(struct _dyneither_ptr*_tmpF2,unsigned int*_tmp543,unsigned int*
_tmp542,void***_tmp540){for(*_tmp543=0;*_tmp543 < *_tmp542;(*_tmp543)++){(*
_tmp540)[*_tmp543]=((void**)(*_tmpF2).curr)[(int)*_tmp543];}}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r){if(Cyc_Tcutil_is_noalias_pointer(t)){struct Cyc_CfFlowInfo_Consumed_struct
_tmp534;struct Cyc_CfFlowInfo_Consumed_struct*_tmp533;return(void*)((_tmp533=
_region_malloc(fenv->r,sizeof(*_tmp533)),((_tmp533[0]=((_tmp534.tag=7,((_tmp534.f1=
consumer,((_tmp534.f2=iteration,((_tmp534.f3=(void*)r,_tmp534)))))))),_tmp533))));}{
struct _tuple0 _tmp535;struct _tuple0 _tmpDB=(_tmp535.f1=Cyc_Tcutil_compress(t),((
_tmp535.f2=r,_tmp535)));void*_tmpDC;struct Cyc_List_List*_tmpDE;void*_tmpDF;
struct Cyc_CfFlowInfo_UnionRInfo _tmpE1;struct _dyneither_ptr _tmpE2;void*_tmpE3;
struct Cyc_Absyn_AggrInfo _tmpE5;union Cyc_Absyn_AggrInfoU _tmpE6;void*_tmpE7;struct
Cyc_CfFlowInfo_UnionRInfo _tmpE9;struct _dyneither_ptr _tmpEA;void*_tmpEB;enum Cyc_Absyn_AggrKind
_tmpED;struct Cyc_List_List*_tmpEE;void*_tmpEF;struct Cyc_CfFlowInfo_UnionRInfo
_tmpF1;struct _dyneither_ptr _tmpF2;_LL47: _tmpDC=_tmpDB.f1;{struct Cyc_Absyn_TupleType_struct*
_tmpDD=(struct Cyc_Absyn_TupleType_struct*)_tmpDC;if(_tmpDD->tag != 11)goto _LL49;
else{_tmpDE=_tmpDD->f1;}};_tmpDF=_tmpDB.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpE0=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpDF;if(_tmpE0->tag != 6)goto
_LL49;else{_tmpE1=_tmpE0->f1;_tmpE2=_tmpE0->f2;}};_LL48: {unsigned int _tmpF3=
_get_dyneither_size(_tmpE2,sizeof(void*));unsigned int _tmp53A;unsigned int
_tmp539;struct _dyneither_ptr _tmp538;void**_tmp537;unsigned int _tmp536;struct
_dyneither_ptr d2=(_tmp536=_tmpF3,((_tmp537=(void**)_region_malloc(fenv->r,
_check_times(sizeof(void*),_tmp536)),((_tmp538=_tag_dyneither(_tmp537,sizeof(
void*),_tmp536),((((_tmp539=_tmp536,_tmp53B(& fenv,& _tmp53A,& _tmp539,& _tmp537))),
_tmp538)))))));{int i=0;for(0;i < _tmpF3;++ i){*((void**)_check_dyneither_subscript(
d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple15*)((
struct Cyc_List_List*)_check_null(_tmpDE))->hd)).f2,consumer,iteration,*((void**)
_check_dyneither_subscript(_tmpE2,sizeof(void*),i)));_tmpDE=_tmpDE->tl;}}{struct
Cyc_CfFlowInfo_Aggregate_struct _tmp53E;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp53D;return(void*)((_tmp53D=_region_malloc(fenv->r,sizeof(*_tmp53D)),((
_tmp53D[0]=((_tmp53E.tag=6,((_tmp53E.f1=_tmpE1,((_tmp53E.f2=d2,_tmp53E)))))),
_tmp53D))));};}_LL49: _tmpE3=_tmpDB.f1;{struct Cyc_Absyn_AggrType_struct*_tmpE4=(
struct Cyc_Absyn_AggrType_struct*)_tmpE3;if(_tmpE4->tag != 12)goto _LL4B;else{
_tmpE5=_tmpE4->f1;_tmpE6=_tmpE5.aggr_info;}};_tmpE7=_tmpDB.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpE8=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpE7;if(_tmpE8->tag != 6)goto
_LL4B;else{_tmpE9=_tmpE8->f1;_tmpEA=_tmpE8->f2;}};_LL4A: {struct Cyc_Absyn_Aggrdecl*
_tmpFA=Cyc_Absyn_get_known_aggrdecl(_tmpE6);if(_tmpFA->impl == 0)return r;_tmpED=
_tmpFA->kind;_tmpEE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFA->impl))->fields;
_tmpF1=_tmpE9;_tmpF2=_tmpEA;goto _LL4C;}_LL4B: _tmpEB=_tmpDB.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmpEC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpEB;if(_tmpEC->tag != 13)goto
_LL4D;else{_tmpED=_tmpEC->f1;_tmpEE=_tmpEC->f2;}};_tmpEF=_tmpDB.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpF0=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpEF;if(_tmpF0->tag != 6)goto
_LL4D;else{_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;}};_LL4C: {unsigned int _tmp543;
unsigned int _tmp542;struct _dyneither_ptr _tmp541;void**_tmp540;unsigned int
_tmp53F;struct _dyneither_ptr d2=(_tmp53F=_get_dyneither_size(_tmpF2,sizeof(void*)),((
_tmp540=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp53F)),((
_tmp541=_tag_dyneither(_tmp540,sizeof(void*),_tmp53F),((((_tmp542=_tmp53F,
_tmp544(& _tmpF2,& _tmp543,& _tmp542,& _tmp540))),_tmp541)))))));unsigned int sz=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpEE);{int i=0;
for(0;i < sz;(i ++,_tmpEE=_tmpEE->tl)){struct Cyc_Absyn_Aggrfield _tmpFC;struct
_dyneither_ptr*_tmpFD;void*_tmpFE;struct Cyc_Absyn_Aggrfield*_tmpFB=(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_List_List*)_check_null(_tmpEE))->hd;_tmpFC=*_tmpFB;_tmpFD=_tmpFC.name;
_tmpFE=_tmpFC.type;if(_get_dyneither_size(*_tmpFD,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,_tmpFE,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpF2,
sizeof(void*),i)));}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp547;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp546;return(void*)((_tmp546=_region_malloc(fenv->r,sizeof(*_tmp546)),((
_tmp546[0]=((_tmp547.tag=6,((_tmp547.f1=_tmpF1,((_tmp547.f2=d2,_tmp547)))))),
_tmp546))));};}_LL4D:;_LL4E: return r;_LL46:;};}struct _tuple16{struct Cyc_CfFlowInfo_Place*
f1;struct Cyc_Position_Segment*f2;};static int Cyc_CfFlowInfo_prefix_of_member(
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){struct _RegionHandle
_tmp105=_new_region("r");struct _RegionHandle*r=& _tmp105;_push_region(r);{struct
_tuple16 _tmp548;struct _tuple16 _tmp106=(_tmp548.f1=place,((_tmp548.f2=0,_tmp548)));
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct
Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple16*))Cyc_Iter_next)(iter,& _tmp106)){struct Cyc_CfFlowInfo_Place*
_tmp107=_tmp106.f1;void*_tmp109;struct Cyc_List_List*_tmp10A;struct Cyc_CfFlowInfo_Place
_tmp108=*place;_tmp109=_tmp108.root;_tmp10A=_tmp108.fields;{void*_tmp10C;struct
Cyc_List_List*_tmp10D;struct Cyc_CfFlowInfo_Place _tmp10B=*_tmp107;_tmp10C=_tmp10B.root;
_tmp10D=_tmp10B.fields;if(Cyc_CfFlowInfo_root_cmp(_tmp109,_tmp10C)!= 0)continue;
for(0;_tmp10A != 0  && _tmp10D != 0;(_tmp10A=_tmp10A->tl,_tmp10D=_tmp10D->tl)){if((
int)_tmp10A->hd != (int)_tmp10D->hd)break;}if(_tmp10A == 0){int _tmp10E=1;
_npop_handler(0);return _tmp10E;}};}{int _tmp10F=0;_npop_handler(0);return _tmp10F;};};
_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*
places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,
struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){struct Cyc_List_List*
_tmp549;pile->places=((_tmp549=_region_malloc(pile->rgn,sizeof(*_tmp549)),((
_tmp549->hd=place,((_tmp549->tl=pile->places,_tmp549))))));}}static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*r){void*_tmp112=r;void*_tmp114;struct Cyc_CfFlowInfo_Place*
_tmp116;struct Cyc_CfFlowInfo_UnionRInfo _tmp118;struct _dyneither_ptr _tmp119;_LL50: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmp113=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp112;if(_tmp113->tag != 7)goto _LL52;else{_tmp114=(void*)_tmp113->f3;}}_LL51:
Cyc_CfFlowInfo_add_places(pile,_tmp114);return;_LL52: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp115=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp112;if(_tmp115->tag != 5)goto
_LL54;else{_tmp116=_tmp115->f1;}}_LL53: Cyc_CfFlowInfo_add_place(pile,_tmp116);
return;_LL54: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp117=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp112;if(_tmp117->tag != 6)goto _LL56;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}
_LL55:{int i=0;for(0;i < _get_dyneither_size(_tmp119,sizeof(void*));++ i){Cyc_CfFlowInfo_add_places(
pile,((void**)_tmp119.curr)[i]);}}return;_LL56:;_LL57: return;_LL4F:;}static void*
Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*
old_val);static void _tmp54F(struct _RegionHandle**r,void**new_val,unsigned int*
_tmp54E,unsigned int*_tmp54D,void***_tmp54B,struct _dyneither_ptr*_tmp11E){for(*
_tmp54E=0;*_tmp54E < *_tmp54D;(*_tmp54E)++){(*_tmp54B)[*_tmp54E]=Cyc_CfFlowInfo_insert_place_inner(*
r,*new_val,((void**)(*_tmp11E).curr)[(int)*_tmp54E]);}}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmp11A=old_val;struct Cyc_CfFlowInfo_UnionRInfo
_tmp11C;int _tmp11D;struct _dyneither_ptr _tmp11E;_LL59: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp11B=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp11A;if(_tmp11B->tag != 6)goto
_LL5B;else{_tmp11C=_tmp11B->f1;_tmp11D=_tmp11C.is_union;_tmp11E=_tmp11B->f2;}}
_LL5A: {unsigned int _tmp54E;unsigned int _tmp54D;struct _dyneither_ptr _tmp54C;void**
_tmp54B;unsigned int _tmp54A;struct _dyneither_ptr d2=(_tmp54A=_get_dyneither_size(
_tmp11E,sizeof(void*)),((_tmp54B=(void**)_region_malloc(r,_check_times(sizeof(
void*),_tmp54A)),((_tmp54C=_tag_dyneither(_tmp54B,sizeof(void*),_tmp54A),((((
_tmp54D=_tmp54A,_tmp54F(& r,& new_val,& _tmp54E,& _tmp54D,& _tmp54B,& _tmp11E))),
_tmp54C)))))));struct Cyc_CfFlowInfo_Aggregate_struct _tmp555;struct Cyc_CfFlowInfo_UnionRInfo
_tmp554;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp553;return(void*)((_tmp553=
_region_malloc(r,sizeof(*_tmp553)),((_tmp553[0]=((_tmp555.tag=6,((_tmp555.f1=((
_tmp554.is_union=_tmp11D,((_tmp554.fieldnum=- 1,_tmp554)))),((_tmp555.f2=d2,
_tmp555)))))),_tmp553))));}_LL5B:;_LL5C: return new_val;_LL58:;}static struct
_dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct
_dyneither_ptr d,int n,void*rval);static void _tmp55B(struct _dyneither_ptr*d,
unsigned int*_tmp55A,unsigned int*_tmp559,void***_tmp557){for(*_tmp55A=0;*
_tmp55A < *_tmp559;(*_tmp55A)++){(*_tmp557)[*_tmp55A]=((void**)(*d).curr)[(int)*
_tmp55A];}}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmp126=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmp126 == rval)return d;{
unsigned int _tmp55A;unsigned int _tmp559;struct _dyneither_ptr _tmp558;void**
_tmp557;unsigned int _tmp556;struct _dyneither_ptr res=(_tmp556=_get_dyneither_size(
d,sizeof(void*)),((_tmp557=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp556)),((_tmp558=_tag_dyneither(_tmp557,sizeof(void*),_tmp556),((((_tmp559=
_tmp556,_tmp55B(& d,& _tmp55A,& _tmp559,& _tmp557))),_tmp558)))))));*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;};}struct
_tuple17{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 
0)return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{struct _tuple17
_tmp55C;struct _tuple17 _tmp12C=(_tmp55C.f1=fs,((_tmp55C.f2=old_val,_tmp55C)));
struct Cyc_List_List*_tmp12D;struct Cyc_List_List _tmp12E;int _tmp12F;struct Cyc_List_List*
_tmp130;void*_tmp131;struct Cyc_CfFlowInfo_UnionRInfo _tmp133;int _tmp134;struct
_dyneither_ptr _tmp135;_LL5E: _tmp12D=_tmp12C.f1;if(_tmp12D == 0)goto _LL60;_tmp12E=*
_tmp12D;_tmp12F=(int)_tmp12E.hd;_tmp130=_tmp12E.tl;_tmp131=_tmp12C.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp132=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp131;if(_tmp132->tag != 6)goto
_LL60;else{_tmp133=_tmp132->f1;_tmp134=_tmp133.is_union;_tmp135=_tmp132->f2;}};
_LL5F: {void*_tmp136=Cyc_CfFlowInfo_insert_place_outer(r,_tmp130,*((void**)
_check_dyneither_subscript(_tmp135,sizeof(void*),_tmp12F)),new_val);struct
_dyneither_ptr _tmp137=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp135,_tmp12F,_tmp136);
if((void**)_tmp137.curr == (void**)_tmp135.curr)return old_val;else{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp562;struct Cyc_CfFlowInfo_UnionRInfo _tmp561;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp560;return(void*)((_tmp560=_region_malloc(r,sizeof(*_tmp560)),((_tmp560[0]=((
_tmp562.tag=6,((_tmp562.f1=((_tmp561.is_union=_tmp134,((_tmp561.fieldnum=- 1,
_tmp561)))),((_tmp562.f2=_tmp137,_tmp562)))))),_tmp560))));}}_LL60:;_LL61: {
struct Cyc_Core_Impossible_struct _tmp568;const char*_tmp567;struct Cyc_Core_Impossible_struct*
_tmp566;(int)_throw((void*)((_tmp566=_cycalloc(sizeof(*_tmp566)),((_tmp566[0]=((
_tmp568.tag=Cyc_Core_Impossible,((_tmp568.f1=((_tmp567="bad insert place",
_tag_dyneither(_tmp567,sizeof(char),17))),_tmp568)))),_tmp566)))));}_LL5D:;};}
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict
d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*_tmp13E=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)
_check_null(pile->places))->tl;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,_tmp13E,0);{void*oldval;void*newval;{struct _handler_cons _tmp13F;
_push_handler(& _tmp13F);{int _tmp141=0;if(setjmp(_tmp13F.handler))_tmp141=1;if(!
_tmp141){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp13E);;_pop_handler();}else{
void*_tmp140=(void*)_exn_thrown;void*_tmp143=_tmp140;_LL63: {struct Cyc_Dict_Absent_struct*
_tmp144=(struct Cyc_Dict_Absent_struct*)_tmp143;if(_tmp144->tag != Cyc_Dict_Absent)
goto _LL65;}_LL64: continue;_LL65:;_LL66:(void)_throw(_tmp143);_LL62:;}};}switch(
Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){case Cyc_CfFlowInfo_AllIL: _LL67: newval=
fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL: _LL68: newval=fenv->esc_this;break;
case Cyc_CfFlowInfo_NoneIL: _LL69: newval=fenv->esc_none;break;}Cyc_CfFlowInfo_add_places(
pile,oldval);{void*_tmp146;struct Cyc_List_List*_tmp147;struct Cyc_CfFlowInfo_Place
_tmp145=*_tmp13E;_tmp146=_tmp145.root;_tmp147=_tmp145.fields;d=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp146,Cyc_CfFlowInfo_insert_place_outer(
fenv->r,_tmp147,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,
_tmp146),newval));};};}return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct
_RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};static enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmp148=r;enum Cyc_CfFlowInfo_InitLevel
_tmp14A;enum Cyc_CfFlowInfo_InitLevel _tmp14C;void*_tmp151;_LL6C: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp149=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp148;if(_tmp149->tag != 3)goto
_LL6E;else{_tmp14A=_tmp149->f1;}}_LL6D: return _tmp14A;_LL6E: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp14B=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp148;if(_tmp14B->tag != 4)goto _LL70;
else{_tmp14C=_tmp14B->f1;}}_LL6F: return _tmp14C;_LL70: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp14D=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp148;if(_tmp14D->tag != 0)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp14E=(struct
Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp148;if(_tmp14E->tag != 1)goto _LL74;}_LL73:
return Cyc_CfFlowInfo_AllIL;_LL74: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp14F=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp148;if(_tmp14F->tag != 2)
goto _LL76;}_LL75: return Cyc_CfFlowInfo_ThisIL;_LL76: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp150=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp148;if(_tmp150->tag != 7)goto
_LL78;else{_tmp151=(void*)_tmp150->f3;}}_LL77: return Cyc_CfFlowInfo_NoneIL;_LL78:;
_LL79: {struct Cyc_Core_Impossible_struct _tmp56E;const char*_tmp56D;struct Cyc_Core_Impossible_struct*
_tmp56C;(int)_throw((void*)((_tmp56C=_cycalloc(sizeof(*_tmp56C)),((_tmp56C[0]=((
_tmp56E.tag=Cyc_Core_Impossible,((_tmp56E.f1=((_tmp56D="initlevel_approx",
_tag_dyneither(_tmp56D,sizeof(char),17))),_tmp56E)))),_tmp56C)))));}_LL6B:;}
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){enum Cyc_CfFlowInfo_InitLevel
this_ans;if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;{void*
_tmp155=r;struct Cyc_CfFlowInfo_UnionRInfo _tmp157;int _tmp158;int _tmp159;struct
_dyneither_ptr _tmp15A;struct Cyc_CfFlowInfo_UnionRInfo _tmp15C;int _tmp15D;struct
_dyneither_ptr _tmp15E;struct Cyc_CfFlowInfo_Place*_tmp160;_LL7B: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp156=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp155;if(_tmp156->tag != 6)goto
_LL7D;else{_tmp157=_tmp156->f1;_tmp158=_tmp157.is_union;if(_tmp158 != 1)goto _LL7D;
_tmp159=_tmp157.fieldnum;_tmp15A=_tmp156->f2;}}_LL7C: {unsigned int _tmp161=
_get_dyneither_size(_tmp15A,sizeof(void*));this_ans=Cyc_CfFlowInfo_NoneIL;if(
_tmp159 == - 1){int i=0;for(0;i < _tmp161;++ i){if(Cyc_CfFlowInfo_initlevel_rec(env,*((
void**)_check_dyneither_subscript(_tmp15A,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL){this_ans=Cyc_CfFlowInfo_AllIL;break;}}}else{if(Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp15A,sizeof(void*),_tmp159)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL)this_ans=Cyc_CfFlowInfo_AllIL;}goto _LL7A;}_LL7D: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp15B=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp155;if(_tmp15B->tag != 6)goto _LL7F;else{_tmp15C=_tmp15B->f1;_tmp15D=_tmp15C.is_union;
if(_tmp15D != 0)goto _LL7F;_tmp15E=_tmp15B->f2;}}_LL7E: {unsigned int _tmp162=
_get_dyneither_size(_tmp15E,sizeof(void*));this_ans=Cyc_CfFlowInfo_AllIL;{int i=0;
for(0;i < _tmp162;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)
_check_dyneither_subscript(_tmp15E,sizeof(void*),i)),this_ans);}}goto _LL7A;}
_LL7F: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp15F=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp155;if(_tmp15F->tag != 5)goto _LL81;else{_tmp160=_tmp15F->f1;}}_LL80: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp160))this_ans=Cyc_CfFlowInfo_AllIL;else{{struct Cyc_List_List*_tmp56F;env->seen=((
_tmp56F=_region_malloc(env->e,sizeof(*_tmp56F)),((_tmp56F->hd=_tmp160,((_tmp56F->tl=
env->seen,_tmp56F))))));}this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(
env->d,_tmp160),Cyc_CfFlowInfo_AllIL);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == Cyc_CfFlowInfo_NoneIL)this_ans=Cyc_CfFlowInfo_ThisIL;}
goto _LL7A;_LL81:;_LL82: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL7A:;}if(
this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;if(this_ans == Cyc_CfFlowInfo_ThisIL
 || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;return Cyc_CfFlowInfo_AllIL;}
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*
env,struct Cyc_Dict_Dict d,void*r){struct _RegionHandle*_tmp164=env->r;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp570;struct Cyc_CfFlowInfo_InitlevelEnv _tmp165=(_tmp570.e=_tmp164,((_tmp570.d=
d,((_tmp570.seen=0,_tmp570)))));return Cyc_CfFlowInfo_initlevel_rec(& _tmp165,r,
Cyc_CfFlowInfo_AllIL);}struct _tuple18{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){void*_tmp168;struct Cyc_List_List*
_tmp169;struct Cyc_CfFlowInfo_Place _tmp167=*place;_tmp168=_tmp167.root;_tmp169=
_tmp167.fields;{void*_tmp16A=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,_tmp168);for(0;_tmp169 != 0;_tmp169=_tmp169->tl){struct _tuple18 _tmp571;struct
_tuple18 _tmp16C=(_tmp571.f1=_tmp16A,((_tmp571.f2=(int)_tmp169->hd,_tmp571)));
void*_tmp16D;struct Cyc_CfFlowInfo_UnionRInfo _tmp16F;struct _dyneither_ptr _tmp170;
int _tmp171;_LL84: _tmp16D=_tmp16C.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp16E=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp16D;if(_tmp16E->tag != 6)goto
_LL86;else{_tmp16F=_tmp16E->f1;_tmp170=_tmp16E->f2;}};_tmp171=_tmp16C.f2;_LL85:
_tmp16A=*((void**)_check_dyneither_subscript(_tmp170,sizeof(void*),_tmp171));
goto _LL83;_LL86:;_LL87: {struct Cyc_Core_Impossible_struct _tmp577;const char*
_tmp576;struct Cyc_Core_Impossible_struct*_tmp575;(int)_throw((void*)((_tmp575=
_cycalloc(sizeof(*_tmp575)),((_tmp575[0]=((_tmp577.tag=Cyc_Core_Impossible,((
_tmp577.f1=((_tmp576="bad lookup_place",_tag_dyneither(_tmp576,sizeof(char),17))),
_tmp577)))),_tmp575)))));}_LL83:;}return _tmp16A;};}static int Cyc_CfFlowInfo_is_rval_unescaped(
void*rval){void*_tmp175=rval;void*_tmp178;struct Cyc_CfFlowInfo_UnionRInfo _tmp17A;
int _tmp17B;int _tmp17C;struct _dyneither_ptr _tmp17D;_LL89: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp176=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp175;if(_tmp176->tag != 4)goto _LL8B;}
_LL8A: return 0;_LL8B: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp177=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp175;if(_tmp177->tag != 7)goto _LL8D;else{_tmp178=(void*)_tmp177->f3;}}_LL8C:
return Cyc_CfFlowInfo_is_rval_unescaped(rval);_LL8D: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp179=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp175;if(_tmp179->tag != 6)goto
_LL8F;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp17A.is_union;_tmp17C=_tmp17A.fieldnum;
_tmp17D=_tmp179->f2;}}_LL8E: if(_tmp17B  && _tmp17C != - 1)return !Cyc_CfFlowInfo_is_rval_unescaped(*((
void**)_check_dyneither_subscript(_tmp17D,sizeof(void*),_tmp17C)));else{
unsigned int sz=_get_dyneither_size(_tmp17D,sizeof(void*));{int i=0;for(0;i < sz;++
i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp17D.curr)[i]))return 0;}}
return 1;}_LL8F:;_LL90: return 1;_LL88:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(
d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct
_RegionHandle*_tmp17E=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp578;struct Cyc_CfFlowInfo_EscPile*
pile=(_tmp578=_region_malloc(_tmp17E,sizeof(*_tmp578)),((_tmp578->rgn=_tmp17E,((
_tmp578->places=0,_tmp578)))));Cyc_CfFlowInfo_add_places(pile,r);return Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*oldval,void*newval);static void _tmp582(struct Cyc_CfFlowInfo_AssignEnv**
env,unsigned int*_tmp581,unsigned int*_tmp580,void***_tmp57E,struct
_dyneither_ptr*_tmp190,struct _dyneither_ptr*_tmp18C){for(*_tmp581=0;*_tmp581 < *
_tmp580;(*_tmp581)++){(*_tmp57E)[*_tmp581]=Cyc_CfFlowInfo_assign_place_inner(*
env,((void**)(*_tmp18C).curr)[(int)*_tmp581],*((void**)
_check_dyneither_subscript(*_tmp190,sizeof(void*),(int)*_tmp581)));}}static void*
Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,
void*newval){struct _tuple0 _tmp579;struct _tuple0 _tmp181=(_tmp579.f1=oldval,((
_tmp579.f2=newval,_tmp579)));void*_tmp182;void*_tmp184;struct Cyc_CfFlowInfo_Place*
_tmp186;void*_tmp187;void*_tmp189;struct Cyc_CfFlowInfo_UnionRInfo _tmp18B;struct
_dyneither_ptr _tmp18C;void*_tmp18D;struct Cyc_CfFlowInfo_UnionRInfo _tmp18F;struct
_dyneither_ptr _tmp190;void*_tmp191;enum Cyc_CfFlowInfo_InitLevel _tmp193;_LL92:
_tmp182=_tmp181.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp183=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp182;if(_tmp183->tag != 4)goto _LL94;};_tmp184=_tmp181.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp185=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp184;if(_tmp185->tag != 5)goto
_LL94;else{_tmp186=_tmp185->f1;}};_LL93: Cyc_CfFlowInfo_add_place(env->pile,
_tmp186);goto _LL95;_LL94: _tmp187=_tmp181.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp188=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp187;if(_tmp188->tag != 4)goto _LL96;};
_LL95: if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp57C;void*_tmp57B;(_tmp57B=0,Cyc_Tcutil_terr(env->loc,((_tmp57C="assignment puts possibly-uninitialized data in an escaped location",
_tag_dyneither(_tmp57C,sizeof(char),67))),_tag_dyneither(_tmp57B,sizeof(void*),0)));}
return(env->fenv)->esc_all;_LL96: _tmp189=_tmp181.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp18A=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp189;if(_tmp18A->tag != 6)goto
_LL98;else{_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;}};_tmp18D=_tmp181.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp18E=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp18D;if(_tmp18E->tag != 6)goto _LL98;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;}};
_LL97: {unsigned int _tmp581;unsigned int _tmp580;struct _dyneither_ptr _tmp57F;void**
_tmp57E;unsigned int _tmp57D;struct _dyneither_ptr new_d=(_tmp57D=
_get_dyneither_size(_tmp18C,sizeof(void*)),((_tmp57E=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp57D)),((_tmp57F=_tag_dyneither(_tmp57E,sizeof(
void*),_tmp57D),((((_tmp580=_tmp57D,_tmp582(& env,& _tmp581,& _tmp580,& _tmp57E,&
_tmp190,& _tmp18C))),_tmp57F)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp18C,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp18C.curr)[i]){
change=1;break;}}}if(!change)return oldval;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp18C,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp190,sizeof(void*),i))){change=1;break;}}}if(!
change)return newval;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp585;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp584;return(void*)((_tmp584=_region_malloc((env->fenv)->r,sizeof(*_tmp584)),((
_tmp584[0]=((_tmp585.tag=6,((_tmp585.f1=_tmp18B,((_tmp585.f2=new_d,_tmp585)))))),
_tmp584))));};}_LL98: _tmp191=_tmp181.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp192=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp191;if(_tmp192->tag != 4)goto _LL9A;else{
_tmp193=_tmp192->f1;}};_LL99: switch(_tmp193){case Cyc_CfFlowInfo_NoneIL: _LL9C:
return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LL9D: return(env->fenv)->unknown_this;
case Cyc_CfFlowInfo_AllIL: _LL9E: return(env->fenv)->unknown_all;}_LL9A:;_LL9B:
return newval;_LL91:;}static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(
env,oldval,newval);{struct _tuple17 _tmp586;struct _tuple17 _tmp19D=(_tmp586.f1=fs,((
_tmp586.f2=oldval,_tmp586)));struct Cyc_List_List*_tmp19E;struct Cyc_List_List
_tmp19F;int _tmp1A0;struct Cyc_List_List*_tmp1A1;void*_tmp1A2;struct Cyc_CfFlowInfo_UnionRInfo
_tmp1A4;int _tmp1A5;int _tmp1A6;struct _dyneither_ptr _tmp1A7;_LLA1: _tmp19E=_tmp19D.f1;
if(_tmp19E == 0)goto _LLA3;_tmp19F=*_tmp19E;_tmp1A0=(int)_tmp19F.hd;_tmp1A1=
_tmp19F.tl;_tmp1A2=_tmp19D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1A3=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1A2;if(_tmp1A3->tag != 6)goto _LLA3;
else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A4.is_union;_tmp1A6=_tmp1A4.fieldnum;
_tmp1A7=_tmp1A3->f2;}};_LLA2: {void*_tmp1A8=Cyc_CfFlowInfo_assign_place_outer(
env,_tmp1A1,*((void**)_check_dyneither_subscript(_tmp1A7,sizeof(void*),_tmp1A0)),
newval);struct _dyneither_ptr _tmp1A9=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,
_tmp1A7,_tmp1A0,_tmp1A8);if((void**)_tmp1A9.curr == (void**)_tmp1A7.curr  && (!
_tmp1A5  || _tmp1A6 == _tmp1A0))return oldval;{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp58C;struct Cyc_CfFlowInfo_UnionRInfo _tmp58B;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp58A;return(void*)((_tmp58A=_region_malloc((env->fenv)->r,sizeof(*_tmp58A)),((
_tmp58A[0]=((_tmp58C.tag=6,((_tmp58C.f1=((_tmp58B.is_union=_tmp1A5,((_tmp58B.fieldnum=
_tmp1A0,_tmp58B)))),((_tmp58C.f2=_tmp1A9,_tmp58C)))))),_tmp58A))));};}_LLA3:;
_LLA4: {struct Cyc_Core_Impossible_struct _tmp592;const char*_tmp591;struct Cyc_Core_Impossible_struct*
_tmp590;(int)_throw((void*)((_tmp590=_cycalloc(sizeof(*_tmp590)),((_tmp590[0]=((
_tmp592.tag=Cyc_Core_Impossible,((_tmp592.f1=((_tmp591="bad assign place",
_tag_dyneither(_tmp591,sizeof(char),17))),_tmp592)))),_tmp590)))));}_LLA0:;};}
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,place,0);{struct _RegionHandle*_tmp1B0=fenv->r;struct Cyc_CfFlowInfo_Place
_tmp1B2;void*_tmp1B3;struct Cyc_List_List*_tmp1B4;struct Cyc_CfFlowInfo_Place*
_tmp1B1=place;_tmp1B2=*_tmp1B1;_tmp1B3=_tmp1B2.root;_tmp1B4=_tmp1B2.fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp595;struct Cyc_CfFlowInfo_AssignEnv _tmp594;
struct Cyc_CfFlowInfo_AssignEnv env=(_tmp594.fenv=fenv,((_tmp594.pile=((_tmp595=
_region_malloc(_tmp1B0,sizeof(*_tmp595)),((_tmp595->rgn=_tmp1B0,((_tmp595->places=
0,_tmp595)))))),((_tmp594.d=d,((_tmp594.loc=loc,_tmp594)))))));void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp1B4,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1B3),
r);return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1B3,newval));};};}
struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict
chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);struct _tuple19{enum Cyc_CfFlowInfo_InitLevel f1;
enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct
Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp5A5(struct Cyc_CfFlowInfo_JoinEnv**
env,unsigned int*_tmp5A4,unsigned int*_tmp5A3,void***_tmp5A1,struct
_dyneither_ptr*_tmp201,struct _dyneither_ptr*_tmp1FB){for(*_tmp5A4=0;*_tmp5A4 < *
_tmp5A3;(*_tmp5A4)++){(*_tmp5A1)[*_tmp5A4]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp1FB).curr)[(
int)*_tmp5A4],*((void**)_check_dyneither_subscript(*_tmp201,sizeof(void*),(int)*
_tmp5A4)));}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp1B7=(
env->fenv)->r;{struct _tuple0 _tmp596;struct _tuple0 _tmp1B9=(_tmp596.f1=r1,((
_tmp596.f2=r2,_tmp596)));void*_tmp1BA;struct Cyc_Absyn_Exp*_tmp1BC;int _tmp1BD;
void*_tmp1BE;void*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C1;int _tmp1C2;void*_tmp1C3;
void*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C6;int _tmp1C7;void*_tmp1C8;void*_tmp1C9;
struct Cyc_Absyn_Exp*_tmp1CB;int _tmp1CC;void*_tmp1CD;void*_tmp1CE;struct Cyc_CfFlowInfo_Place*
_tmp1D0;void*_tmp1D1;struct Cyc_CfFlowInfo_Place*_tmp1D3;void*_tmp1D4;struct Cyc_CfFlowInfo_Place*
_tmp1D6;void*_tmp1D7;void*_tmp1D9;void*_tmp1DB;struct Cyc_CfFlowInfo_Place*
_tmp1DD;void*_tmp1DE;struct Cyc_CfFlowInfo_Place*_tmp1E0;void*_tmp1E1;void*
_tmp1E3;void*_tmp1E5;struct Cyc_CfFlowInfo_Place*_tmp1E7;void*_tmp1E8;struct Cyc_CfFlowInfo_Place*
_tmp1EA;void*_tmp1EB;struct Cyc_CfFlowInfo_Place*_tmp1ED;void*_tmp1EE;void*
_tmp1F0;void*_tmp1F2;void*_tmp1F4;void*_tmp1F6;struct Cyc_CfFlowInfo_UnionRInfo
_tmp1F8;int _tmp1F9;int _tmp1FA;struct _dyneither_ptr _tmp1FB;void*_tmp1FC;struct Cyc_CfFlowInfo_UnionRInfo
_tmp1FE;int _tmp1FF;int _tmp200;struct _dyneither_ptr _tmp201;_LLA6: _tmp1BA=_tmp1B9.f1;{
struct Cyc_CfFlowInfo_Consumed_struct*_tmp1BB=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1BA;if(_tmp1BB->tag != 7)goto _LLA8;else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BB->f2;
_tmp1BE=(void*)_tmp1BB->f3;}};_tmp1BF=_tmp1B9.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1C0=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1BF;if(_tmp1C0->tag != 7)goto
_LLA8;else{_tmp1C1=_tmp1C0->f1;_tmp1C2=_tmp1C0->f2;_tmp1C3=(void*)_tmp1C0->f3;}};
_LLA7: if(_tmp1BC == _tmp1C1  && _tmp1BD == _tmp1C2){struct Cyc_CfFlowInfo_Consumed_struct
_tmp599;struct Cyc_CfFlowInfo_Consumed_struct*_tmp598;return(void*)((_tmp598=
_region_malloc(_tmp1B7,sizeof(*_tmp598)),((_tmp598[0]=((_tmp599.tag=7,((_tmp599.f1=
_tmp1BC,((_tmp599.f2=_tmp1BD,((_tmp599.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1BE,_tmp1C3),_tmp599)))))))),_tmp598))));}{void*_tmp204=_tmp1BE;struct Cyc_CfFlowInfo_Place*
_tmp206;_LLC3: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp205=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp204;if(_tmp205->tag != 5)goto _LLC5;else{_tmp206=_tmp205->f1;}}_LLC4: Cyc_CfFlowInfo_add_place(
env->pile,_tmp206);goto _LLC2;_LLC5:;_LLC6: goto _LLC2;_LLC2:;}{void*_tmp207=
_tmp1C3;struct Cyc_CfFlowInfo_Place*_tmp209;_LLC8: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp208=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp207;if(_tmp208->tag != 5)goto
_LLCA;else{_tmp209=_tmp208->f1;}}_LLC9: Cyc_CfFlowInfo_add_place(env->pile,
_tmp209);goto _LLC7;_LLCA:;_LLCB: goto _LLC7;_LLC7:;}goto _LLA5;_LLA8: _tmp1C4=
_tmp1B9.f1;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp1C5=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1C4;if(_tmp1C5->tag != 7)goto _LLAA;else{_tmp1C6=_tmp1C5->f1;_tmp1C7=_tmp1C5->f2;
_tmp1C8=(void*)_tmp1C5->f3;}};_LLA9: {struct Cyc_CfFlowInfo_Consumed_struct
_tmp59C;struct Cyc_CfFlowInfo_Consumed_struct*_tmp59B;return(void*)((_tmp59B=
_region_malloc(_tmp1B7,sizeof(*_tmp59B)),((_tmp59B[0]=((_tmp59C.tag=7,((_tmp59C.f1=
_tmp1C6,((_tmp59C.f2=_tmp1C7,((_tmp59C.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1C8,r2),_tmp59C)))))))),_tmp59B))));}_LLAA: _tmp1C9=_tmp1B9.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1CA=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1C9;if(_tmp1CA->tag != 7)goto
_LLAC;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=_tmp1CA->f2;_tmp1CD=(void*)_tmp1CA->f3;}};
_LLAB: {struct Cyc_CfFlowInfo_Consumed_struct _tmp59F;struct Cyc_CfFlowInfo_Consumed_struct*
_tmp59E;return(void*)((_tmp59E=_region_malloc(_tmp1B7,sizeof(*_tmp59E)),((
_tmp59E[0]=((_tmp59F.tag=7,((_tmp59F.f1=_tmp1CB,((_tmp59F.f2=_tmp1CC,((_tmp59F.f3=(
void*)Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1CD),_tmp59F)))))))),_tmp59E))));}
_LLAC: _tmp1CE=_tmp1B9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1CF=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1CE;if(_tmp1CF->tag != 5)goto _LLAE;else{
_tmp1D0=_tmp1CF->f1;}};_tmp1D1=_tmp1B9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1D2=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1D1;if(_tmp1D2->tag != 5)goto
_LLAE;else{_tmp1D3=_tmp1D2->f1;}};_LLAD: if(Cyc_CfFlowInfo_place_cmp(_tmp1D0,
_tmp1D3)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp1D0);Cyc_CfFlowInfo_add_place(
env->pile,_tmp1D3);goto _LLA5;_LLAE: _tmp1D4=_tmp1B9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1D5=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1D4;if(_tmp1D5->tag != 5)goto
_LLB0;else{_tmp1D6=_tmp1D5->f1;}};_tmp1D7=_tmp1B9.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1D8=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1D7;if(_tmp1D8->tag != 2)
goto _LLB0;};_LLAF: _tmp1DD=_tmp1D6;goto _LLB1;_LLB0: _tmp1D9=_tmp1B9.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1DA=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1D9;if(_tmp1DA->tag != 2)
goto _LLB2;};_tmp1DB=_tmp1B9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1DC=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1DB;if(_tmp1DC->tag != 5)goto _LLB2;
else{_tmp1DD=_tmp1DC->f1;}};_LLB1: Cyc_CfFlowInfo_add_place(env->pile,_tmp1DD);
return(env->fenv)->notzerothis;_LLB2: _tmp1DE=_tmp1B9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1DF=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1DE;if(_tmp1DF->tag != 5)goto
_LLB4;else{_tmp1E0=_tmp1DF->f1;}};_tmp1E1=_tmp1B9.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp1E2=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1E1;if(_tmp1E2->tag != 1)
goto _LLB4;};_LLB3: Cyc_CfFlowInfo_add_place(env->pile,_tmp1E0);switch(Cyc_CfFlowInfo_initlevel(
env->fenv,env->d1,r1)){case Cyc_CfFlowInfo_AllIL: _LLCC: return(env->fenv)->notzeroall;
default: _LLCD: return(env->fenv)->notzerothis;}_LLB4: _tmp1E3=_tmp1B9.f1;{struct
Cyc_CfFlowInfo_NotZeroAll_struct*_tmp1E4=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp1E3;if(_tmp1E4->tag != 1)goto _LLB6;};_tmp1E5=_tmp1B9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1E6=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1E5;if(_tmp1E6->tag != 5)goto
_LLB6;else{_tmp1E7=_tmp1E6->f1;}};_LLB5: Cyc_CfFlowInfo_add_place(env->pile,
_tmp1E7);switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2)){case Cyc_CfFlowInfo_AllIL:
_LLCF: return(env->fenv)->notzeroall;default: _LLD0: return(env->fenv)->notzerothis;}
_LLB6: _tmp1E8=_tmp1B9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1E9=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1E8;if(_tmp1E9->tag != 5)goto _LLB8;else{
_tmp1EA=_tmp1E9->f1;}};_LLB7: Cyc_CfFlowInfo_add_place(env->pile,_tmp1EA);goto
_LLA5;_LLB8: _tmp1EB=_tmp1B9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1EC=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1EB;if(_tmp1EC->tag != 5)goto _LLBA;
else{_tmp1ED=_tmp1EC->f1;}};_LLB9: Cyc_CfFlowInfo_add_place(env->pile,_tmp1ED);
goto _LLA5;_LLBA: _tmp1EE=_tmp1B9.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp1EF=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1EE;if(_tmp1EF->tag != 1)
goto _LLBC;};_tmp1F0=_tmp1B9.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp1F1=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1F0;if(_tmp1F1->tag != 2)goto _LLBC;};
_LLBB: goto _LLBD;_LLBC: _tmp1F2=_tmp1B9.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1F3=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1F2;if(_tmp1F3->tag != 2)
goto _LLBE;};_tmp1F4=_tmp1B9.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp1F5=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1F4;if(_tmp1F5->tag != 1)goto _LLBE;};
_LLBD: return(env->fenv)->notzerothis;_LLBE: _tmp1F6=_tmp1B9.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp1F7=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1F6;if(_tmp1F7->tag != 6)goto
_LLC0;else{_tmp1F8=_tmp1F7->f1;_tmp1F9=_tmp1F8.is_union;_tmp1FA=_tmp1F8.fieldnum;
_tmp1FB=_tmp1F7->f2;}};_tmp1FC=_tmp1B9.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp1FD=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1FC;if(_tmp1FD->tag != 6)goto
_LLC0;else{_tmp1FE=_tmp1FD->f1;_tmp1FF=_tmp1FE.is_union;_tmp200=_tmp1FE.fieldnum;
_tmp201=_tmp1FD->f2;}};_LLBF: {unsigned int _tmp5A4;unsigned int _tmp5A3;struct
_dyneither_ptr _tmp5A2;void**_tmp5A1;unsigned int _tmp5A0;struct _dyneither_ptr
new_d=(_tmp5A0=_get_dyneither_size(_tmp1FB,sizeof(void*)),((_tmp5A1=(void**)
_region_malloc(_tmp1B7,_check_times(sizeof(void*),_tmp5A0)),((_tmp5A2=
_tag_dyneither(_tmp5A1,sizeof(void*),_tmp5A0),((((_tmp5A3=_tmp5A0,_tmp5A5(& env,&
_tmp5A4,& _tmp5A3,& _tmp5A1,& _tmp201,& _tmp1FB))),_tmp5A2)))))));int change=0;{int i=
0;for(0;i < _get_dyneither_size(_tmp1FB,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1FB.curr)[i]){
change=1;break;}}}if(!change){if(!_tmp1F9)return r1;new_d=_tmp1FB;}else{change=0;{
int i=0;for(0;i < _get_dyneither_size(_tmp1FB,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp201,sizeof(void*),i))){change=1;break;}}}if(!
change){if(!_tmp1F9)return r2;new_d=_tmp201;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp5AB;struct Cyc_CfFlowInfo_UnionRInfo _tmp5AA;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5A9;return(void*)((_tmp5A9=_region_malloc(_tmp1B7,sizeof(*_tmp5A9)),((
_tmp5A9[0]=((_tmp5AB.tag=6,((_tmp5AB.f1=((_tmp5AA.is_union=_tmp1F9,((_tmp5AA.fieldnum=
_tmp1FA == _tmp200?_tmp1FA: - 1,_tmp5AA)))),((_tmp5AB.f2=new_d,_tmp5AB)))))),
_tmp5A9))));};}_LLC0:;_LLC1: goto _LLA5;_LLA5:;}{enum Cyc_CfFlowInfo_InitLevel il1=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel il2=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);struct _tuple0 _tmp5AC;struct
_tuple0 _tmp216=(_tmp5AC.f1=r1,((_tmp5AC.f2=r2,_tmp5AC)));void*_tmp217;void*
_tmp219;_LLD3: _tmp217=_tmp216.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp218=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp217;if(_tmp218->tag != 4)goto _LLD5;};_LLD4:
goto _LLD6;_LLD5: _tmp219=_tmp216.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp21A=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp219;if(_tmp21A->tag != 4)goto _LLD7;};_LLD6: {
struct _tuple19 _tmp5AD;struct _tuple19 _tmp21C=(_tmp5AD.f1=il1,((_tmp5AD.f2=il2,
_tmp5AD)));enum Cyc_CfFlowInfo_InitLevel _tmp21D;enum Cyc_CfFlowInfo_InitLevel
_tmp21E;enum Cyc_CfFlowInfo_InitLevel _tmp21F;enum Cyc_CfFlowInfo_InitLevel
_tmp220;_LLDA: _tmp21D=_tmp21C.f2;if(_tmp21D != Cyc_CfFlowInfo_NoneIL)goto _LLDC;
_LLDB: goto _LLDD;_LLDC: _tmp21E=_tmp21C.f1;if(_tmp21E != Cyc_CfFlowInfo_NoneIL)goto
_LLDE;_LLDD: return(env->fenv)->esc_none;_LLDE: _tmp21F=_tmp21C.f2;if(_tmp21F != 
Cyc_CfFlowInfo_ThisIL)goto _LLE0;_LLDF: goto _LLE1;_LLE0: _tmp220=_tmp21C.f1;if(
_tmp220 != Cyc_CfFlowInfo_ThisIL)goto _LLE2;_LLE1: return(env->fenv)->esc_this;
_LLE2:;_LLE3: return(env->fenv)->esc_all;_LLD9:;}_LLD7:;_LLD8: {struct _tuple19
_tmp5AE;struct _tuple19 _tmp222=(_tmp5AE.f1=il1,((_tmp5AE.f2=il2,_tmp5AE)));enum 
Cyc_CfFlowInfo_InitLevel _tmp223;enum Cyc_CfFlowInfo_InitLevel _tmp224;enum Cyc_CfFlowInfo_InitLevel
_tmp225;enum Cyc_CfFlowInfo_InitLevel _tmp226;_LLE5: _tmp223=_tmp222.f2;if(_tmp223
!= Cyc_CfFlowInfo_NoneIL)goto _LLE7;_LLE6: goto _LLE8;_LLE7: _tmp224=_tmp222.f1;if(
_tmp224 != Cyc_CfFlowInfo_NoneIL)goto _LLE9;_LLE8: return(env->fenv)->unknown_none;
_LLE9: _tmp225=_tmp222.f2;if(_tmp225 != Cyc_CfFlowInfo_ThisIL)goto _LLEB;_LLEA: goto
_LLEC;_LLEB: _tmp226=_tmp222.f1;if(_tmp226 != Cyc_CfFlowInfo_ThisIL)goto _LLED;
_LLEC: return(env->fenv)->unknown_this;_LLED:;_LLEE: return(env->fenv)->unknown_all;
_LLE4:;}_LLD2:;};};}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5AF;return((_tmp5AF.EqualConst).val=
i,(((_tmp5AF.EqualConst).tag=1,_tmp5AF)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(
struct Cyc_Absyn_Vardecl*v,void*t){struct _tuple10 _tmp5B2;union Cyc_CfFlowInfo_RelnOp
_tmp5B1;return((_tmp5B1.LessVar).val=((_tmp5B2.f1=v,((_tmp5B2.f2=t,_tmp5B2)))),(((
_tmp5B1.LessVar).tag=2,_tmp5B1)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(
struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp5B3;return((_tmp5B3.LessNumelts).val=
v,(((_tmp5B3.LessNumelts).tag=3,_tmp5B3)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5B4;return((_tmp5B4.LessConst).val=
i,(((_tmp5B4.LessConst).tag=4,_tmp5B4)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5B5;return((_tmp5B5.LessEqConst).val=
i,(((_tmp5B5.LessEqConst).tag=6,_tmp5B5)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(
struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp5B6;return((_tmp5B6.LessEqNumelts).val=
v,(((_tmp5B6.LessEqNumelts).tag=5,_tmp5B6)));}struct _tuple20{union Cyc_CfFlowInfo_RelnOp
f1;union Cyc_CfFlowInfo_RelnOp f2;};static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2){struct _tuple20 _tmp5B7;struct _tuple20 _tmp22F=(
_tmp5B7.f1=r1,((_tmp5B7.f2=r2,_tmp5B7)));union Cyc_CfFlowInfo_RelnOp _tmp230;
unsigned int _tmp231;union Cyc_CfFlowInfo_RelnOp _tmp232;unsigned int _tmp233;union
Cyc_CfFlowInfo_RelnOp _tmp234;struct _tuple10 _tmp235;struct Cyc_Absyn_Vardecl*
_tmp236;union Cyc_CfFlowInfo_RelnOp _tmp237;struct _tuple10 _tmp238;struct Cyc_Absyn_Vardecl*
_tmp239;union Cyc_CfFlowInfo_RelnOp _tmp23A;struct Cyc_Absyn_Vardecl*_tmp23B;union
Cyc_CfFlowInfo_RelnOp _tmp23C;struct Cyc_Absyn_Vardecl*_tmp23D;union Cyc_CfFlowInfo_RelnOp
_tmp23E;unsigned int _tmp23F;union Cyc_CfFlowInfo_RelnOp _tmp240;unsigned int
_tmp241;union Cyc_CfFlowInfo_RelnOp _tmp242;unsigned int _tmp243;union Cyc_CfFlowInfo_RelnOp
_tmp244;unsigned int _tmp245;union Cyc_CfFlowInfo_RelnOp _tmp246;struct Cyc_Absyn_Vardecl*
_tmp247;union Cyc_CfFlowInfo_RelnOp _tmp248;struct Cyc_Absyn_Vardecl*_tmp249;_LLF0:
_tmp230=_tmp22F.f1;if((_tmp230.EqualConst).tag != 1)goto _LLF2;_tmp231=(
unsigned int)(_tmp230.EqualConst).val;_tmp232=_tmp22F.f2;if((_tmp232.EqualConst).tag
!= 1)goto _LLF2;_tmp233=(unsigned int)(_tmp232.EqualConst).val;_LLF1: return
_tmp231 == _tmp233;_LLF2: _tmp234=_tmp22F.f1;if((_tmp234.LessVar).tag != 2)goto
_LLF4;_tmp235=(struct _tuple10)(_tmp234.LessVar).val;_tmp236=_tmp235.f1;_tmp237=
_tmp22F.f2;if((_tmp237.LessVar).tag != 2)goto _LLF4;_tmp238=(struct _tuple10)(
_tmp237.LessVar).val;_tmp239=_tmp238.f1;_LLF3: return _tmp236 == _tmp239;_LLF4:
_tmp23A=_tmp22F.f1;if((_tmp23A.LessNumelts).tag != 3)goto _LLF6;_tmp23B=(struct Cyc_Absyn_Vardecl*)(
_tmp23A.LessNumelts).val;_tmp23C=_tmp22F.f2;if((_tmp23C.LessNumelts).tag != 3)
goto _LLF6;_tmp23D=(struct Cyc_Absyn_Vardecl*)(_tmp23C.LessNumelts).val;_LLF5:
return _tmp23B == _tmp23D;_LLF6: _tmp23E=_tmp22F.f1;if((_tmp23E.LessConst).tag != 4)
goto _LLF8;_tmp23F=(unsigned int)(_tmp23E.LessConst).val;_tmp240=_tmp22F.f2;if((
_tmp240.LessConst).tag != 4)goto _LLF8;_tmp241=(unsigned int)(_tmp240.LessConst).val;
_LLF7: return _tmp23F == _tmp241;_LLF8: _tmp242=_tmp22F.f1;if((_tmp242.LessEqConst).tag
!= 6)goto _LLFA;_tmp243=(unsigned int)(_tmp242.LessEqConst).val;_tmp244=_tmp22F.f2;
if((_tmp244.LessEqConst).tag != 6)goto _LLFA;_tmp245=(unsigned int)(_tmp244.LessEqConst).val;
_LLF9: return _tmp243 == _tmp245;_LLFA: _tmp246=_tmp22F.f1;if((_tmp246.LessEqNumelts).tag
!= 5)goto _LLFC;_tmp247=(struct Cyc_Absyn_Vardecl*)(_tmp246.LessEqNumelts).val;
_tmp248=_tmp22F.f2;if((_tmp248.LessEqNumelts).tag != 5)goto _LLFC;_tmp249=(struct
Cyc_Absyn_Vardecl*)(_tmp248.LessEqNumelts).val;_LLFB: return _tmp247 == _tmp249;
_LLFC:;_LLFD: return 0;_LLEF:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == 
r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*_tmp24A=
r1s;for(0;_tmp24A != 0;_tmp24A=_tmp24A->tl){struct Cyc_CfFlowInfo_Reln*_tmp24B=(
struct Cyc_CfFlowInfo_Reln*)_tmp24A->hd;int found=0;{struct Cyc_List_List*_tmp24C=
r2s;for(0;_tmp24C != 0;_tmp24C=_tmp24C->tl){struct Cyc_CfFlowInfo_Reln*_tmp24D=(
struct Cyc_CfFlowInfo_Reln*)_tmp24C->hd;if(_tmp24B == _tmp24D  || _tmp24B->vd == 
_tmp24D->vd  && Cyc_CfFlowInfo_same_relop(_tmp24B->rop,_tmp24D->rop)){{struct Cyc_List_List*
_tmp5B8;res=((_tmp5B8=_region_malloc(r,sizeof(*_tmp5B8)),((_tmp5B8->hd=_tmp24B,((
_tmp5B8->tl=res,_tmp5B8))))));}found=1;break;}}}if(!found)diff=1;}}if(!diff)res=
r1s;return res;};}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2){struct _tuple21 _tmp5B9;struct _tuple21 _tmp250=(_tmp5B9.f1=f1,((_tmp5B9.f2=f2,
_tmp5B9)));union Cyc_CfFlowInfo_FlowInfo _tmp251;int _tmp252;union Cyc_CfFlowInfo_FlowInfo
_tmp253;int _tmp254;union Cyc_CfFlowInfo_FlowInfo _tmp255;struct _tuple11 _tmp256;
struct Cyc_Dict_Dict _tmp257;struct Cyc_List_List*_tmp258;union Cyc_CfFlowInfo_FlowInfo
_tmp259;struct _tuple11 _tmp25A;struct Cyc_Dict_Dict _tmp25B;struct Cyc_List_List*
_tmp25C;_LLFF: _tmp251=_tmp250.f1;if((_tmp251.BottomFL).tag != 1)goto _LL101;
_tmp252=(int)(_tmp251.BottomFL).val;_LL100: return f2;_LL101: _tmp253=_tmp250.f2;
if((_tmp253.BottomFL).tag != 1)goto _LL103;_tmp254=(int)(_tmp253.BottomFL).val;
_LL102: return f1;_LL103: _tmp255=_tmp250.f1;if((_tmp255.ReachableFL).tag != 2)goto
_LLFE;_tmp256=(struct _tuple11)(_tmp255.ReachableFL).val;_tmp257=_tmp256.f1;
_tmp258=_tmp256.f2;_tmp259=_tmp250.f2;if((_tmp259.ReachableFL).tag != 2)goto _LLFE;
_tmp25A=(struct _tuple11)(_tmp259.ReachableFL).val;_tmp25B=_tmp25A.f1;_tmp25C=
_tmp25A.f2;_LL104: if(_tmp257.t == _tmp25B.t  && _tmp258 == _tmp25C)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle*_tmp25D=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp5BC;struct Cyc_CfFlowInfo_JoinEnv
_tmp5BB;struct Cyc_CfFlowInfo_JoinEnv _tmp25E=(_tmp5BB.fenv=fenv,((_tmp5BB.pile=((
_tmp5BC=_region_malloc(_tmp25D,sizeof(*_tmp5BC)),((_tmp5BC->rgn=_tmp25D,((
_tmp5BC->places=0,_tmp5BC)))))),((_tmp5BB.d1=_tmp257,((_tmp5BB.d2=_tmp25B,
_tmp5BB)))))));struct Cyc_Dict_Dict _tmp25F=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,&
_tmp25E,_tmp257,_tmp25B);struct Cyc_List_List*_tmp260=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp258,_tmp25C);return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp25E.pile,all_changed,_tmp25F),_tmp260);};_LLFE:;}struct _tuple22{union
Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo
f3;};struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple12 pr1,struct _tuple12 pr2){union
Cyc_CfFlowInfo_FlowInfo _tmp264;void*_tmp265;struct _tuple12 _tmp263=pr1;_tmp264=
_tmp263.f1;_tmp265=_tmp263.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp267;void*_tmp268;
struct _tuple12 _tmp266=pr2;_tmp267=_tmp266.f1;_tmp268=_tmp266.f2;{union Cyc_CfFlowInfo_FlowInfo
outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp264,_tmp267);struct
_tuple22 _tmp5BD;struct _tuple22 _tmp26A=(_tmp5BD.f1=_tmp264,((_tmp5BD.f2=_tmp267,((
_tmp5BD.f3=outflow,_tmp5BD)))));union Cyc_CfFlowInfo_FlowInfo _tmp26B;int _tmp26C;
union Cyc_CfFlowInfo_FlowInfo _tmp26D;int _tmp26E;union Cyc_CfFlowInfo_FlowInfo
_tmp26F;struct _tuple11 _tmp270;struct Cyc_Dict_Dict _tmp271;union Cyc_CfFlowInfo_FlowInfo
_tmp272;struct _tuple11 _tmp273;struct Cyc_Dict_Dict _tmp274;union Cyc_CfFlowInfo_FlowInfo
_tmp275;struct _tuple11 _tmp276;struct Cyc_Dict_Dict _tmp277;struct Cyc_List_List*
_tmp278;_LL106: _tmp26B=_tmp26A.f1;if((_tmp26B.BottomFL).tag != 1)goto _LL108;
_tmp26C=(int)(_tmp26B.BottomFL).val;_LL107: {struct _tuple12 _tmp5BE;return(
_tmp5BE.f1=outflow,((_tmp5BE.f2=_tmp268,_tmp5BE)));}_LL108: _tmp26D=_tmp26A.f2;
if((_tmp26D.BottomFL).tag != 1)goto _LL10A;_tmp26E=(int)(_tmp26D.BottomFL).val;
_LL109: {struct _tuple12 _tmp5BF;return(_tmp5BF.f1=outflow,((_tmp5BF.f2=_tmp265,
_tmp5BF)));}_LL10A: _tmp26F=_tmp26A.f1;if((_tmp26F.ReachableFL).tag != 2)goto
_LL10C;_tmp270=(struct _tuple11)(_tmp26F.ReachableFL).val;_tmp271=_tmp270.f1;
_tmp272=_tmp26A.f2;if((_tmp272.ReachableFL).tag != 2)goto _LL10C;_tmp273=(struct
_tuple11)(_tmp272.ReachableFL).val;_tmp274=_tmp273.f1;_tmp275=_tmp26A.f3;if((
_tmp275.ReachableFL).tag != 2)goto _LL10C;_tmp276=(struct _tuple11)(_tmp275.ReachableFL).val;
_tmp277=_tmp276.f1;_tmp278=_tmp276.f2;_LL10B: if(((int(*)(int ignore,void*r1,void*
r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp265,_tmp268)){struct _tuple12
_tmp5C0;return(_tmp5C0.f1=outflow,((_tmp5C0.f2=_tmp268,_tmp5C0)));}if(((int(*)(
int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp268,
_tmp265)){struct _tuple12 _tmp5C1;return(_tmp5C1.f1=outflow,((_tmp5C1.f2=_tmp265,
_tmp5C1)));}{struct _RegionHandle*_tmp27D=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp5C4;struct Cyc_CfFlowInfo_JoinEnv _tmp5C3;struct Cyc_CfFlowInfo_JoinEnv _tmp27E=(
_tmp5C3.fenv=fenv,((_tmp5C3.pile=((_tmp5C4=_region_malloc(_tmp27D,sizeof(*
_tmp5C4)),((_tmp5C4->rgn=_tmp27D,((_tmp5C4->places=0,_tmp5C4)))))),((_tmp5C3.d1=
_tmp271,((_tmp5C3.d2=_tmp274,_tmp5C3)))))));void*_tmp27F=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp27E,0,_tmp265,
_tmp268);struct _tuple12 _tmp5C5;return(_tmp5C5.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp27E.pile,all_changed,_tmp277),_tmp278),((_tmp5C5.f2=_tmp27F,_tmp5C5)));};
_LL10C:;_LL10D: {struct Cyc_Core_Impossible_struct _tmp5CB;const char*_tmp5CA;
struct Cyc_Core_Impossible_struct*_tmp5C9;(int)_throw((void*)((_tmp5C9=_cycalloc(
sizeof(*_tmp5C9)),((_tmp5C9[0]=((_tmp5CB.tag=Cyc_Core_Impossible,((_tmp5CB.f1=((
_tmp5CA="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp5CA,sizeof(
char),37))),_tmp5CB)))),_tmp5C9)))));}_LL105:;};};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp5D2(struct Cyc_CfFlowInfo_AfterEnv**
env,unsigned int*_tmp5D1,unsigned int*_tmp5D0,void***_tmp5CE,struct
_dyneither_ptr*_tmp293,struct _dyneither_ptr*_tmp28F){for(*_tmp5D1=0;*_tmp5D1 < *
_tmp5D0;(*_tmp5D1)++){(*_tmp5CE)[*_tmp5D1]=Cyc_CfFlowInfo_after_absRval_d(*env,(
int)*_tmp5D1,((void**)(*_tmp28F).curr)[(int)*_tmp5D1],*((void**)
_check_dyneither_subscript(*_tmp293,sizeof(void*),(int)*_tmp5D1)));}}static void*
Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);int
changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);if(
changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*
r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);if(changed1){if(
!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))return r1;env->changed=
Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,
env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct _tuple0 _tmp5CC;
struct _tuple0 _tmp287=(_tmp5CC.f1=r1,((_tmp5CC.f2=r2,_tmp5CC)));void*_tmp288;void*
_tmp28A;void*_tmp28C;struct Cyc_CfFlowInfo_UnionRInfo _tmp28E;struct _dyneither_ptr
_tmp28F;void*_tmp290;struct Cyc_CfFlowInfo_UnionRInfo _tmp292;struct _dyneither_ptr
_tmp293;_LL10F: _tmp288=_tmp287.f1;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp289=(
struct Cyc_CfFlowInfo_Consumed_struct*)_tmp288;if(_tmp289->tag != 7)goto _LL111;};
_LL110: goto _LL112;_LL111: _tmp28A=_tmp287.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp28B=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp28A;if(_tmp28B->tag != 7)goto
_LL113;};_LL112: return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,
void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL113: _tmp28C=
_tmp287.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp28D=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp28C;if(_tmp28D->tag != 6)goto _LL115;else{_tmp28E=_tmp28D->f1;_tmp28F=_tmp28D->f2;}};
_tmp290=_tmp287.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp291=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp290;if(_tmp291->tag != 6)goto _LL115;else{_tmp292=_tmp291->f1;_tmp293=_tmp291->f2;}};
_LL114: {unsigned int _tmp5D1;unsigned int _tmp5D0;struct _dyneither_ptr _tmp5CF;
void**_tmp5CE;unsigned int _tmp5CD;struct _dyneither_ptr new_d=(_tmp5CD=
_get_dyneither_size(_tmp28F,sizeof(void*)),((_tmp5CE=(void**)_region_malloc(((
env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp5CD)),((_tmp5CF=
_tag_dyneither(_tmp5CE,sizeof(void*),_tmp5CD),((((_tmp5D0=_tmp5CD,_tmp5D2(& env,&
_tmp5D1,& _tmp5D0,& _tmp5CE,& _tmp293,& _tmp28F))),_tmp5CF)))))));int change=0;{int i=
0;for(0;i < _get_dyneither_size(_tmp28F,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp28F.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp28F,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp293,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5D5;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5D4;return(void*)((_tmp5D4=_region_malloc(((env->joinenv).fenv)->r,sizeof(*
_tmp5D4)),((_tmp5D4[0]=((_tmp5D5.tag=6,((_tmp5D5.f1=_tmp28E,((_tmp5D5.f2=new_d,
_tmp5D5)))))),_tmp5D4))));};}_LL115:;_LL116: {struct Cyc_Core_Impossible_struct
_tmp5DB;const char*_tmp5DA;struct Cyc_Core_Impossible_struct*_tmp5D9;(int)_throw((
void*)((_tmp5D9=_cycalloc(sizeof(*_tmp5D9)),((_tmp5D9[0]=((_tmp5DB.tag=Cyc_Core_Impossible,((
_tmp5DB.f1=((_tmp5DA="after_absRval -- non-aggregates!",_tag_dyneither(_tmp5DA,
sizeof(char),33))),_tmp5DB)))),_tmp5D9)))));}_LL10E:;};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp29D=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
_tmp29E=env->changed;{struct Cyc_List_List*_tmp5DC;*env->last_field_cell=((
_tmp5DC=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp5DC)),((_tmp5DC->hd=(
void*)key,((_tmp5DC->tl=0,_tmp5DC))))));}env->last_field_cell=(struct Cyc_List_List**)&((
struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp2A0=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp29D;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp29E;return _tmp2A0;};};}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_CfFlowInfo_Place _tmp5DD;*env->curr_place=((_tmp5DD.root=root,((_tmp5DD.fields=
0,_tmp5DD))));}env->last_field_cell=(struct Cyc_List_List**)&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){struct _tuple21 _tmp5DE;struct
_tuple21 _tmp2A3=(_tmp5DE.f1=f1,((_tmp5DE.f2=f2,_tmp5DE)));union Cyc_CfFlowInfo_FlowInfo
_tmp2A4;int _tmp2A5;union Cyc_CfFlowInfo_FlowInfo _tmp2A6;int _tmp2A7;union Cyc_CfFlowInfo_FlowInfo
_tmp2A8;struct _tuple11 _tmp2A9;struct Cyc_Dict_Dict _tmp2AA;struct Cyc_List_List*
_tmp2AB;union Cyc_CfFlowInfo_FlowInfo _tmp2AC;struct _tuple11 _tmp2AD;struct Cyc_Dict_Dict
_tmp2AE;struct Cyc_List_List*_tmp2AF;_LL118: _tmp2A4=_tmp2A3.f1;if((_tmp2A4.BottomFL).tag
!= 1)goto _LL11A;_tmp2A5=(int)(_tmp2A4.BottomFL).val;_LL119: goto _LL11B;_LL11A:
_tmp2A6=_tmp2A3.f2;if((_tmp2A6.BottomFL).tag != 1)goto _LL11C;_tmp2A7=(int)(
_tmp2A6.BottomFL).val;_LL11B: return Cyc_CfFlowInfo_BottomFL();_LL11C: _tmp2A8=
_tmp2A3.f1;if((_tmp2A8.ReachableFL).tag != 2)goto _LL117;_tmp2A9=(struct _tuple11)(
_tmp2A8.ReachableFL).val;_tmp2AA=_tmp2A9.f1;_tmp2AB=_tmp2A9.f2;_tmp2AC=_tmp2A3.f2;
if((_tmp2AC.ReachableFL).tag != 2)goto _LL117;_tmp2AD=(struct _tuple11)(_tmp2AC.ReachableFL).val;
_tmp2AE=_tmp2AD.f1;_tmp2AF=_tmp2AD.f2;_LL11D: if(_tmp2AA.t == _tmp2AE.t  && _tmp2AB
== _tmp2AF)return f1;{struct _RegionHandle*_tmp2B0=fenv->r;struct Cyc_CfFlowInfo_Place*
_tmp2B1=fenv->dummy_place;struct Cyc_CfFlowInfo_JoinEnv _tmp5E4;struct Cyc_CfFlowInfo_EscPile*
_tmp5E3;struct Cyc_CfFlowInfo_AfterEnv _tmp5E2;struct Cyc_CfFlowInfo_AfterEnv
_tmp2B2=(_tmp5E2.joinenv=((_tmp5E4.fenv=fenv,((_tmp5E4.pile=((_tmp5E3=
_region_malloc(_tmp2B0,sizeof(*_tmp5E3)),((_tmp5E3->rgn=_tmp2B0,((_tmp5E3->places=
0,_tmp5E3)))))),((_tmp5E4.d1=_tmp2AA,((_tmp5E4.d2=_tmp2AE,_tmp5E4)))))))),((
_tmp5E2.chg1=chg1,((_tmp5E2.chg2=chg2,((_tmp5E2.curr_place=_tmp2B1,((_tmp5E2.last_field_cell=(
struct Cyc_List_List**)& _tmp2B1->fields,((_tmp5E2.changed=Cyc_CfFlowInfo_Neither,
_tmp5E2)))))))))));struct Cyc_Dict_Dict _tmp2B3=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp2B2,_tmp2AA,_tmp2AE);struct Cyc_List_List*_tmp2B4=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp2AB,_tmp2AF);return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,(_tmp2B2.joinenv).pile,all_changed,_tmp2B3),_tmp2B4);};_LL117:;}static int
Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){if(r1 == r2)
return 1;{struct _tuple0 _tmp5E5;struct _tuple0 _tmp2B9=(_tmp5E5.f1=r1,((_tmp5E5.f2=
r2,_tmp5E5)));void*_tmp2BA;struct Cyc_CfFlowInfo_Place*_tmp2BC;void*_tmp2BD;
struct Cyc_CfFlowInfo_Place*_tmp2BF;void*_tmp2C0;void*_tmp2C2;void*_tmp2C4;struct
Cyc_CfFlowInfo_UnionRInfo _tmp2C6;struct _dyneither_ptr _tmp2C7;void*_tmp2C8;struct
Cyc_CfFlowInfo_UnionRInfo _tmp2CA;struct _dyneither_ptr _tmp2CB;void*_tmp2CC;void*
_tmp2CE;void*_tmp2D0;void*_tmp2D2;void*_tmp2D4;void*_tmp2D6;void*_tmp2D8;void*
_tmp2DA;_LL11F: _tmp2BA=_tmp2B9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp2BB=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2BA;if(_tmp2BB->tag != 5)goto _LL121;
else{_tmp2BC=_tmp2BB->f1;}};_tmp2BD=_tmp2B9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2BE=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2BD;if(_tmp2BE->tag != 5)goto
_LL121;else{_tmp2BF=_tmp2BE->f1;}};_LL120: return Cyc_CfFlowInfo_place_cmp(_tmp2BC,
_tmp2BF)== 0;_LL121: _tmp2C0=_tmp2B9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2C1=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2C0;if(_tmp2C1->tag != 5)goto
_LL123;};_LL122: goto _LL124;_LL123: _tmp2C2=_tmp2B9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2C3=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2C2;if(_tmp2C3->tag != 5)goto
_LL125;};_LL124: return 0;_LL125: _tmp2C4=_tmp2B9.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2C5=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2C4;if(_tmp2C5->tag != 6)goto
_LL127;else{_tmp2C6=_tmp2C5->f1;_tmp2C7=_tmp2C5->f2;}};_tmp2C8=_tmp2B9.f2;{
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2C9=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2C8;if(_tmp2C9->tag != 6)goto _LL127;else{_tmp2CA=_tmp2C9->f1;_tmp2CB=_tmp2C9->f2;}};
_LL126: if((void**)_tmp2C7.curr == (void**)_tmp2CB.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp2C7,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2C7.curr)[i],*((
void**)_check_dyneither_subscript(_tmp2CB,sizeof(void*),i))))return 0;}}return 1;
_LL127: _tmp2CC=_tmp2B9.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2CD=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp2CC;if(_tmp2CD->tag != 1)goto _LL129;};
_tmp2CE=_tmp2B9.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2CF=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2CE;if(_tmp2CF->tag != 2)goto _LL129;};_LL128: return 1;_LL129: _tmp2D0=_tmp2B9.f2;{
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2D1=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2D0;if(_tmp2D1->tag != 2)goto _LL12B;};_LL12A: return 0;_LL12B: _tmp2D2=_tmp2B9.f2;{
struct Cyc_CfFlowInfo_Zero_struct*_tmp2D3=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp2D2;if(_tmp2D3->tag != 0)goto _LL12D;};_LL12C: goto _LL12E;_LL12D: _tmp2D4=
_tmp2B9.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2D5=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp2D4;if(_tmp2D5->tag != 1)goto _LL12F;};_LL12E: return 0;_LL12F: _tmp2D6=_tmp2B9.f1;{
struct Cyc_CfFlowInfo_Esc_struct*_tmp2D7=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp2D6;if(_tmp2D7->tag != 4)goto _LL131;};_tmp2D8=_tmp2B9.f2;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp2D9=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2D8;if(_tmp2D9->tag != 4)goto _LL131;};
_LL130: goto _LL11E;_LL131: _tmp2DA=_tmp2B9.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp2DB=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2DA;if(_tmp2DB->tag != 4)goto _LL133;};
_LL132: return 0;_LL133:;_LL134: goto _LL11E;_LL11E:;}{struct _tuple19 _tmp5E6;struct
_tuple19 _tmp2DD=(_tmp5E6.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp5E6.f2=Cyc_CfFlowInfo_initlevel_approx(
r2),_tmp5E6)));enum Cyc_CfFlowInfo_InitLevel _tmp2DE;enum Cyc_CfFlowInfo_InitLevel
_tmp2DF;enum Cyc_CfFlowInfo_InitLevel _tmp2E0;enum Cyc_CfFlowInfo_InitLevel
_tmp2E1;enum Cyc_CfFlowInfo_InitLevel _tmp2E2;enum Cyc_CfFlowInfo_InitLevel
_tmp2E3;_LL136: _tmp2DE=_tmp2DD.f1;if(_tmp2DE != Cyc_CfFlowInfo_AllIL)goto _LL138;
_tmp2DF=_tmp2DD.f2;if(_tmp2DF != Cyc_CfFlowInfo_AllIL)goto _LL138;_LL137: return 1;
_LL138: _tmp2E0=_tmp2DD.f2;if(_tmp2E0 != Cyc_CfFlowInfo_NoneIL)goto _LL13A;_LL139:
return 1;_LL13A: _tmp2E1=_tmp2DD.f1;if(_tmp2E1 != Cyc_CfFlowInfo_NoneIL)goto _LL13C;
_LL13B: return 0;_LL13C: _tmp2E2=_tmp2DD.f2;if(_tmp2E2 != Cyc_CfFlowInfo_ThisIL)goto
_LL13E;_LL13D: return 1;_LL13E: _tmp2E3=_tmp2DD.f1;if(_tmp2E3 != Cyc_CfFlowInfo_ThisIL)
goto _LL135;_LL13F: return 0;_LL135:;};}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s != 0;
r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp2E4=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;
int found=0;{struct Cyc_List_List*_tmp2E5=r2s;for(0;_tmp2E5 != 0;_tmp2E5=_tmp2E5->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2E6=(struct Cyc_CfFlowInfo_Reln*)_tmp2E5->hd;if(
_tmp2E4 == _tmp2E6  || _tmp2E4->vd == _tmp2E6->vd  && Cyc_CfFlowInfo_same_relop(
_tmp2E4->rop,_tmp2E6->rop)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){struct _tuple21
_tmp5E7;struct _tuple21 _tmp2E8=(_tmp5E7.f1=f1,((_tmp5E7.f2=f2,_tmp5E7)));union Cyc_CfFlowInfo_FlowInfo
_tmp2E9;int _tmp2EA;union Cyc_CfFlowInfo_FlowInfo _tmp2EB;int _tmp2EC;union Cyc_CfFlowInfo_FlowInfo
_tmp2ED;struct _tuple11 _tmp2EE;struct Cyc_Dict_Dict _tmp2EF;struct Cyc_List_List*
_tmp2F0;union Cyc_CfFlowInfo_FlowInfo _tmp2F1;struct _tuple11 _tmp2F2;struct Cyc_Dict_Dict
_tmp2F3;struct Cyc_List_List*_tmp2F4;_LL141: _tmp2E9=_tmp2E8.f1;if((_tmp2E9.BottomFL).tag
!= 1)goto _LL143;_tmp2EA=(int)(_tmp2E9.BottomFL).val;_LL142: return 1;_LL143:
_tmp2EB=_tmp2E8.f2;if((_tmp2EB.BottomFL).tag != 1)goto _LL145;_tmp2EC=(int)(
_tmp2EB.BottomFL).val;_LL144: return 0;_LL145: _tmp2ED=_tmp2E8.f1;if((_tmp2ED.ReachableFL).tag
!= 2)goto _LL140;_tmp2EE=(struct _tuple11)(_tmp2ED.ReachableFL).val;_tmp2EF=
_tmp2EE.f1;_tmp2F0=_tmp2EE.f2;_tmp2F1=_tmp2E8.f2;if((_tmp2F1.ReachableFL).tag != 
2)goto _LL140;_tmp2F2=(struct _tuple11)(_tmp2F1.ReachableFL).val;_tmp2F3=_tmp2F2.f1;
_tmp2F4=_tmp2F2.f2;_LL146: if(_tmp2EF.t == _tmp2F3.t  && _tmp2F0 == _tmp2F4)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp2EF,_tmp2F3) && Cyc_CfFlowInfo_relns_approx(_tmp2F0,_tmp2F4);_LL140:;}struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*
rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=0;for(p=rs;!found
 && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp2F5=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp2F5->vd == v){found=1;break;}{union Cyc_CfFlowInfo_RelnOp _tmp2F6=
_tmp2F5->rop;struct _tuple10 _tmp2F7;struct Cyc_Absyn_Vardecl*_tmp2F8;struct Cyc_Absyn_Vardecl*
_tmp2F9;struct Cyc_Absyn_Vardecl*_tmp2FA;_LL148: if((_tmp2F6.LessVar).tag != 2)goto
_LL14A;_tmp2F7=(struct _tuple10)(_tmp2F6.LessVar).val;_tmp2F8=_tmp2F7.f1;_LL149:
_tmp2F9=_tmp2F8;goto _LL14B;_LL14A: if((_tmp2F6.LessNumelts).tag != 3)goto _LL14C;
_tmp2F9=(struct Cyc_Absyn_Vardecl*)(_tmp2F6.LessNumelts).val;_LL14B: _tmp2FA=
_tmp2F9;goto _LL14D;_LL14C: if((_tmp2F6.LessEqNumelts).tag != 5)goto _LL14E;_tmp2FA=(
struct Cyc_Absyn_Vardecl*)(_tmp2F6.LessEqNumelts).val;_LL14D: if(v == _tmp2FA)found=
1;goto _LL147;_LL14E:;_LL14F: goto _LL147;_LL147:;};}if(!found)return rs;{struct Cyc_List_List*
_tmp2FB=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp2FC=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp2FC->vd != v){{union Cyc_CfFlowInfo_RelnOp _tmp2FD=_tmp2FC->rop;struct
_tuple10 _tmp2FE;struct Cyc_Absyn_Vardecl*_tmp2FF;struct Cyc_Absyn_Vardecl*_tmp300;
struct Cyc_Absyn_Vardecl*_tmp301;_LL151: if((_tmp2FD.LessVar).tag != 2)goto _LL153;
_tmp2FE=(struct _tuple10)(_tmp2FD.LessVar).val;_tmp2FF=_tmp2FE.f1;_LL152: _tmp300=
_tmp2FF;goto _LL154;_LL153: if((_tmp2FD.LessNumelts).tag != 3)goto _LL155;_tmp300=(
struct Cyc_Absyn_Vardecl*)(_tmp2FD.LessNumelts).val;_LL154: _tmp301=_tmp300;goto
_LL156;_LL155: if((_tmp2FD.LessEqNumelts).tag != 5)goto _LL157;_tmp301=(struct Cyc_Absyn_Vardecl*)(
_tmp2FD.LessEqNumelts).val;_LL156: if(v == _tmp301)continue;goto _LL150;_LL157:;
_LL158: goto _LL150;_LL150:;}{struct Cyc_List_List*_tmp5E8;_tmp2FB=((_tmp5E8=
_region_malloc(rgn,sizeof(*_tmp5E8)),((_tmp5E8->hd=_tmp2FC,((_tmp5E8->tl=_tmp2FB,
_tmp5E8))))));};}}return _tmp2FB;};}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*
_tmp303=e->r;void*_tmp305;struct Cyc_Absyn_Vardecl*_tmp307;void*_tmp309;struct Cyc_Absyn_Vardecl*
_tmp30B;void*_tmp30D;struct Cyc_Absyn_Vardecl*_tmp30F;void*_tmp311;struct Cyc_Absyn_Vardecl*
_tmp313;_LL15A: {struct Cyc_Absyn_Var_e_struct*_tmp304=(struct Cyc_Absyn_Var_e_struct*)
_tmp303;if(_tmp304->tag != 1)goto _LL15C;else{_tmp305=(void*)_tmp304->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp306=(struct Cyc_Absyn_Global_b_struct*)_tmp305;if(
_tmp306->tag != 1)goto _LL15C;else{_tmp307=_tmp306->f1;}};}}_LL15B: _tmp30B=_tmp307;
goto _LL15D;_LL15C: {struct Cyc_Absyn_Var_e_struct*_tmp308=(struct Cyc_Absyn_Var_e_struct*)
_tmp303;if(_tmp308->tag != 1)goto _LL15E;else{_tmp309=(void*)_tmp308->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp30A=(struct Cyc_Absyn_Param_b_struct*)_tmp309;if(
_tmp30A->tag != 3)goto _LL15E;else{_tmp30B=_tmp30A->f1;}};}}_LL15D: _tmp30F=_tmp30B;
goto _LL15F;_LL15E: {struct Cyc_Absyn_Var_e_struct*_tmp30C=(struct Cyc_Absyn_Var_e_struct*)
_tmp303;if(_tmp30C->tag != 1)goto _LL160;else{_tmp30D=(void*)_tmp30C->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp30E=(struct Cyc_Absyn_Local_b_struct*)_tmp30D;if(
_tmp30E->tag != 4)goto _LL160;else{_tmp30F=_tmp30E->f1;}};}}_LL15F: _tmp313=_tmp30F;
goto _LL161;_LL160: {struct Cyc_Absyn_Var_e_struct*_tmp310=(struct Cyc_Absyn_Var_e_struct*)
_tmp303;if(_tmp310->tag != 1)goto _LL162;else{_tmp311=(void*)_tmp310->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp312=(struct Cyc_Absyn_Pat_b_struct*)_tmp311;if(_tmp312->tag
!= 5)goto _LL162;else{_tmp313=_tmp312->f1;}};}}_LL161: if(!_tmp313->escapes)return
Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp313);goto _LL159;_LL162:;_LL163: goto _LL159;
_LL159:;}return r;}struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct
_RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){struct Cyc_CfFlowInfo_Reln*_tmp5E9;
return(_tmp5E9=_region_malloc(r2,sizeof(*_tmp5E9)),((_tmp5E9[0]=*r,_tmp5E9)));}
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*
relns){return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*
f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){for(
0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct Cyc_CfFlowInfo_Reln*_tmp315=(
struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*_tmp316=(struct Cyc_CfFlowInfo_Reln*)
r2->hd;if(_tmp315->vd != _tmp316->vd  || !Cyc_CfFlowInfo_same_relop(_tmp315->rop,
_tmp316->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(
rgn,r,v);{void*_tmp317=e->r;struct Cyc_Absyn_MallocInfo _tmp319;struct Cyc_Absyn_Exp*
_tmp31A;int _tmp31B;_LL165: {struct Cyc_Absyn_Malloc_e_struct*_tmp318=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp317;if(_tmp318->tag != 34)goto _LL167;else{_tmp319=_tmp318->f1;_tmp31A=_tmp319.num_elts;
_tmp31B=_tmp319.fat_result;if(_tmp31B != 1)goto _LL167;}}_LL166: malloc_loop: {void*
_tmp31C=_tmp31A->r;struct Cyc_Absyn_Exp*_tmp31E;void*_tmp320;struct Cyc_Absyn_Vardecl*
_tmp322;void*_tmp324;struct Cyc_Absyn_Vardecl*_tmp326;void*_tmp328;struct Cyc_Absyn_Vardecl*
_tmp32A;void*_tmp32C;struct Cyc_Absyn_Vardecl*_tmp32E;_LL16A: {struct Cyc_Absyn_Cast_e_struct*
_tmp31D=(struct Cyc_Absyn_Cast_e_struct*)_tmp31C;if(_tmp31D->tag != 15)goto _LL16C;
else{_tmp31E=_tmp31D->f2;}}_LL16B: _tmp31A=_tmp31E;goto malloc_loop;_LL16C: {
struct Cyc_Absyn_Var_e_struct*_tmp31F=(struct Cyc_Absyn_Var_e_struct*)_tmp31C;if(
_tmp31F->tag != 1)goto _LL16E;else{_tmp320=(void*)_tmp31F->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp321=(struct Cyc_Absyn_Pat_b_struct*)_tmp320;if(_tmp321->tag != 5)goto _LL16E;
else{_tmp322=_tmp321->f1;}};}}_LL16D: _tmp326=_tmp322;goto _LL16F;_LL16E: {struct
Cyc_Absyn_Var_e_struct*_tmp323=(struct Cyc_Absyn_Var_e_struct*)_tmp31C;if(_tmp323->tag
!= 1)goto _LL170;else{_tmp324=(void*)_tmp323->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp325=(struct Cyc_Absyn_Local_b_struct*)_tmp324;if(_tmp325->tag != 4)goto _LL170;
else{_tmp326=_tmp325->f1;}};}}_LL16F: _tmp32A=_tmp326;goto _LL171;_LL170: {struct
Cyc_Absyn_Var_e_struct*_tmp327=(struct Cyc_Absyn_Var_e_struct*)_tmp31C;if(_tmp327->tag
!= 1)goto _LL172;else{_tmp328=(void*)_tmp327->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp329=(struct Cyc_Absyn_Param_b_struct*)_tmp328;if(_tmp329->tag != 3)goto _LL172;
else{_tmp32A=_tmp329->f1;}};}}_LL171: _tmp32E=_tmp32A;goto _LL173;_LL172: {struct
Cyc_Absyn_Var_e_struct*_tmp32B=(struct Cyc_Absyn_Var_e_struct*)_tmp31C;if(_tmp32B->tag
!= 1)goto _LL174;else{_tmp32C=(void*)_tmp32B->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp32D=(struct Cyc_Absyn_Global_b_struct*)_tmp32C;if(_tmp32D->tag != 1)goto _LL174;
else{_tmp32E=_tmp32D->f1;}};}}_LL173: if(_tmp32E->escapes)return r;{struct Cyc_CfFlowInfo_Reln*
_tmp5EC;struct Cyc_List_List*_tmp5EB;return(_tmp5EB=_region_malloc(rgn,sizeof(*
_tmp5EB)),((_tmp5EB->hd=((_tmp5EC=_region_malloc(rgn,sizeof(*_tmp5EC)),((_tmp5EC->vd=
_tmp32E,((_tmp5EC->rop=Cyc_CfFlowInfo_LessEqNumelts(v),_tmp5EC)))))),((_tmp5EB->tl=
r,_tmp5EB)))));};_LL174:;_LL175: return r;_LL169:;}_LL167:;_LL168: goto _LL164;
_LL164:;}{void*_tmp331=Cyc_Tcutil_compress(v->type);_LL177: {struct Cyc_Absyn_IntType_struct*
_tmp332=(struct Cyc_Absyn_IntType_struct*)_tmp331;if(_tmp332->tag != 6)goto _LL179;}
_LL178: goto _LL176;_LL179:;_LL17A: return r;_LL176:;}loop: {void*_tmp333=e->r;
struct Cyc_Absyn_Exp*_tmp335;union Cyc_Absyn_Cnst _tmp337;struct _tuple6 _tmp338;int
_tmp339;enum Cyc_Absyn_Primop _tmp33B;struct Cyc_List_List*_tmp33C;struct Cyc_List_List
_tmp33D;struct Cyc_List_List*_tmp33E;struct Cyc_List_List _tmp33F;struct Cyc_Absyn_Exp*
_tmp340;enum Cyc_Absyn_Primop _tmp342;struct Cyc_List_List*_tmp343;struct Cyc_List_List
_tmp344;struct Cyc_Absyn_Exp*_tmp345;_LL17C: {struct Cyc_Absyn_Cast_e_struct*
_tmp334=(struct Cyc_Absyn_Cast_e_struct*)_tmp333;if(_tmp334->tag != 15)goto _LL17E;
else{_tmp335=_tmp334->f2;}}_LL17D: e=_tmp335;goto loop;_LL17E: {struct Cyc_Absyn_Const_e_struct*
_tmp336=(struct Cyc_Absyn_Const_e_struct*)_tmp333;if(_tmp336->tag != 0)goto _LL180;
else{_tmp337=_tmp336->f1;if((_tmp337.Int_c).tag != 4)goto _LL180;_tmp338=(struct
_tuple6)(_tmp337.Int_c).val;_tmp339=_tmp338.f2;}}_LL17F: {struct Cyc_CfFlowInfo_Reln*
_tmp5EF;struct Cyc_List_List*_tmp5EE;return(_tmp5EE=_region_malloc(rgn,sizeof(*
_tmp5EE)),((_tmp5EE->hd=((_tmp5EF=_region_malloc(rgn,sizeof(*_tmp5EF)),((_tmp5EF->vd=
v,((_tmp5EF->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp339),_tmp5EF)))))),((
_tmp5EE->tl=r,_tmp5EE)))));}_LL180: {struct Cyc_Absyn_Primop_e_struct*_tmp33A=(
struct Cyc_Absyn_Primop_e_struct*)_tmp333;if(_tmp33A->tag != 3)goto _LL182;else{
_tmp33B=_tmp33A->f1;if(_tmp33B != Cyc_Absyn_Mod)goto _LL182;_tmp33C=_tmp33A->f2;
if(_tmp33C == 0)goto _LL182;_tmp33D=*_tmp33C;_tmp33E=_tmp33D.tl;if(_tmp33E == 0)
goto _LL182;_tmp33F=*_tmp33E;_tmp340=(struct Cyc_Absyn_Exp*)_tmp33F.hd;}}_LL181:{
void*_tmp348=_tmp340->r;enum Cyc_Absyn_Primop _tmp34A;struct Cyc_List_List*_tmp34B;
struct Cyc_List_List _tmp34C;struct Cyc_Absyn_Exp*_tmp34D;_LL187: {struct Cyc_Absyn_Primop_e_struct*
_tmp349=(struct Cyc_Absyn_Primop_e_struct*)_tmp348;if(_tmp349->tag != 3)goto _LL189;
else{_tmp34A=_tmp349->f1;_tmp34B=_tmp349->f2;if(_tmp34B == 0)goto _LL189;_tmp34C=*
_tmp34B;_tmp34D=(struct Cyc_Absyn_Exp*)_tmp34C.hd;}}_LL188:{void*_tmp34E=_tmp34D->r;
void*_tmp350;struct Cyc_Absyn_Vardecl*_tmp352;void*_tmp354;struct Cyc_Absyn_Vardecl*
_tmp356;void*_tmp358;struct Cyc_Absyn_Vardecl*_tmp35A;void*_tmp35C;struct Cyc_Absyn_Vardecl*
_tmp35E;_LL18C: {struct Cyc_Absyn_Var_e_struct*_tmp34F=(struct Cyc_Absyn_Var_e_struct*)
_tmp34E;if(_tmp34F->tag != 1)goto _LL18E;else{_tmp350=(void*)_tmp34F->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp351=(struct Cyc_Absyn_Global_b_struct*)_tmp350;if(
_tmp351->tag != 1)goto _LL18E;else{_tmp352=_tmp351->f1;}};}}_LL18D: _tmp356=_tmp352;
goto _LL18F;_LL18E: {struct Cyc_Absyn_Var_e_struct*_tmp353=(struct Cyc_Absyn_Var_e_struct*)
_tmp34E;if(_tmp353->tag != 1)goto _LL190;else{_tmp354=(void*)_tmp353->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp355=(struct Cyc_Absyn_Local_b_struct*)_tmp354;if(
_tmp355->tag != 4)goto _LL190;else{_tmp356=_tmp355->f1;}};}}_LL18F: _tmp35A=_tmp356;
goto _LL191;_LL190: {struct Cyc_Absyn_Var_e_struct*_tmp357=(struct Cyc_Absyn_Var_e_struct*)
_tmp34E;if(_tmp357->tag != 1)goto _LL192;else{_tmp358=(void*)_tmp357->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp359=(struct Cyc_Absyn_Param_b_struct*)_tmp358;if(
_tmp359->tag != 3)goto _LL192;else{_tmp35A=_tmp359->f1;}};}}_LL191: _tmp35E=_tmp35A;
goto _LL193;_LL192: {struct Cyc_Absyn_Var_e_struct*_tmp35B=(struct Cyc_Absyn_Var_e_struct*)
_tmp34E;if(_tmp35B->tag != 1)goto _LL194;else{_tmp35C=(void*)_tmp35B->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp35D=(struct Cyc_Absyn_Pat_b_struct*)_tmp35C;if(_tmp35D->tag
!= 5)goto _LL194;else{_tmp35E=_tmp35D->f1;}};}}_LL193: if(_tmp35E->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp5F2;struct Cyc_List_List*_tmp5F1;return(_tmp5F1=
_region_malloc(rgn,sizeof(*_tmp5F1)),((_tmp5F1->hd=((_tmp5F2=_region_malloc(rgn,
sizeof(*_tmp5F2)),((_tmp5F2->vd=v,((_tmp5F2->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp35E),_tmp5F2)))))),((_tmp5F1->tl=r,_tmp5F1)))));};_LL194:;_LL195: goto _LL18B;
_LL18B:;}goto _LL186;_LL189:;_LL18A: goto _LL186;_LL186:;}goto _LL17B;_LL182: {
struct Cyc_Absyn_Primop_e_struct*_tmp341=(struct Cyc_Absyn_Primop_e_struct*)
_tmp333;if(_tmp341->tag != 3)goto _LL184;else{_tmp342=_tmp341->f1;_tmp343=_tmp341->f2;
if(_tmp343 == 0)goto _LL184;_tmp344=*_tmp343;_tmp345=(struct Cyc_Absyn_Exp*)_tmp344.hd;}}
_LL183:{void*_tmp361=_tmp345->r;void*_tmp363;struct Cyc_Absyn_Vardecl*_tmp365;
void*_tmp367;struct Cyc_Absyn_Vardecl*_tmp369;void*_tmp36B;struct Cyc_Absyn_Vardecl*
_tmp36D;void*_tmp36F;struct Cyc_Absyn_Vardecl*_tmp371;_LL197: {struct Cyc_Absyn_Var_e_struct*
_tmp362=(struct Cyc_Absyn_Var_e_struct*)_tmp361;if(_tmp362->tag != 1)goto _LL199;
else{_tmp363=(void*)_tmp362->f2;{struct Cyc_Absyn_Global_b_struct*_tmp364=(struct
Cyc_Absyn_Global_b_struct*)_tmp363;if(_tmp364->tag != 1)goto _LL199;else{_tmp365=
_tmp364->f1;}};}}_LL198: _tmp369=_tmp365;goto _LL19A;_LL199: {struct Cyc_Absyn_Var_e_struct*
_tmp366=(struct Cyc_Absyn_Var_e_struct*)_tmp361;if(_tmp366->tag != 1)goto _LL19B;
else{_tmp367=(void*)_tmp366->f2;{struct Cyc_Absyn_Local_b_struct*_tmp368=(struct
Cyc_Absyn_Local_b_struct*)_tmp367;if(_tmp368->tag != 4)goto _LL19B;else{_tmp369=
_tmp368->f1;}};}}_LL19A: _tmp36D=_tmp369;goto _LL19C;_LL19B: {struct Cyc_Absyn_Var_e_struct*
_tmp36A=(struct Cyc_Absyn_Var_e_struct*)_tmp361;if(_tmp36A->tag != 1)goto _LL19D;
else{_tmp36B=(void*)_tmp36A->f2;{struct Cyc_Absyn_Param_b_struct*_tmp36C=(struct
Cyc_Absyn_Param_b_struct*)_tmp36B;if(_tmp36C->tag != 3)goto _LL19D;else{_tmp36D=
_tmp36C->f1;}};}}_LL19C: _tmp371=_tmp36D;goto _LL19E;_LL19D: {struct Cyc_Absyn_Var_e_struct*
_tmp36E=(struct Cyc_Absyn_Var_e_struct*)_tmp361;if(_tmp36E->tag != 1)goto _LL19F;
else{_tmp36F=(void*)_tmp36E->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp370=(struct Cyc_Absyn_Pat_b_struct*)
_tmp36F;if(_tmp370->tag != 5)goto _LL19F;else{_tmp371=_tmp370->f1;}};}}_LL19E: if(
_tmp371->escapes)return r;{struct Cyc_CfFlowInfo_Reln*_tmp5F5;struct Cyc_List_List*
_tmp5F4;return(_tmp5F4=_region_malloc(rgn,sizeof(*_tmp5F4)),((_tmp5F4->hd=((
_tmp5F5=_region_malloc(rgn,sizeof(*_tmp5F5)),((_tmp5F5->vd=v,((_tmp5F5->rop=Cyc_CfFlowInfo_LessEqNumelts(
_tmp371),_tmp5F5)))))),((_tmp5F4->tl=r,_tmp5F4)))));};_LL19F:;_LL1A0: goto _LL196;
_LL196:;}goto _LL17B;_LL184:;_LL185: goto _LL17B;_LL17B:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp374=e1->r;void*_tmp376;
struct Cyc_Absyn_Vardecl*_tmp378;void*_tmp37A;struct Cyc_Absyn_Vardecl*_tmp37C;
void*_tmp37E;struct Cyc_Absyn_Vardecl*_tmp380;void*_tmp382;struct Cyc_Absyn_Vardecl*
_tmp384;_LL1A2: {struct Cyc_Absyn_Var_e_struct*_tmp375=(struct Cyc_Absyn_Var_e_struct*)
_tmp374;if(_tmp375->tag != 1)goto _LL1A4;else{_tmp376=(void*)_tmp375->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp377=(struct Cyc_Absyn_Global_b_struct*)_tmp376;if(
_tmp377->tag != 1)goto _LL1A4;else{_tmp378=_tmp377->f1;}};}}_LL1A3: _tmp37C=_tmp378;
goto _LL1A5;_LL1A4: {struct Cyc_Absyn_Var_e_struct*_tmp379=(struct Cyc_Absyn_Var_e_struct*)
_tmp374;if(_tmp379->tag != 1)goto _LL1A6;else{_tmp37A=(void*)_tmp379->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp37B=(struct Cyc_Absyn_Param_b_struct*)_tmp37A;if(
_tmp37B->tag != 3)goto _LL1A6;else{_tmp37C=_tmp37B->f1;}};}}_LL1A5: _tmp380=_tmp37C;
goto _LL1A7;_LL1A6: {struct Cyc_Absyn_Var_e_struct*_tmp37D=(struct Cyc_Absyn_Var_e_struct*)
_tmp374;if(_tmp37D->tag != 1)goto _LL1A8;else{_tmp37E=(void*)_tmp37D->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp37F=(struct Cyc_Absyn_Local_b_struct*)_tmp37E;if(
_tmp37F->tag != 4)goto _LL1A8;else{_tmp380=_tmp37F->f1;}};}}_LL1A7: _tmp384=_tmp380;
goto _LL1A9;_LL1A8: {struct Cyc_Absyn_Var_e_struct*_tmp381=(struct Cyc_Absyn_Var_e_struct*)
_tmp374;if(_tmp381->tag != 1)goto _LL1AA;else{_tmp382=(void*)_tmp381->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp383=(struct Cyc_Absyn_Pat_b_struct*)_tmp382;if(_tmp383->tag
!= 5)goto _LL1AA;else{_tmp384=_tmp383->f1;}};}}_LL1A9: if(!_tmp384->escapes)return
Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp384,e2);goto _LL1A1;_LL1AA:;_LL1AB: goto
_LL1A1;_LL1A1:;}return r;}static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){{const char*_tmp5F9;void*_tmp5F8[1];struct Cyc_String_pa_struct _tmp5F7;(_tmp5F7.tag=
0,((_tmp5F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name)),((_tmp5F8[0]=& _tmp5F7,Cyc_fprintf(Cyc_stderr,((_tmp5F9="%s",
_tag_dyneither(_tmp5F9,sizeof(char),3))),_tag_dyneither(_tmp5F8,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp388=r->rop;unsigned int _tmp389;struct _tuple10
_tmp38A;struct Cyc_Absyn_Vardecl*_tmp38B;struct Cyc_Absyn_Vardecl*_tmp38C;
unsigned int _tmp38D;unsigned int _tmp38E;struct Cyc_Absyn_Vardecl*_tmp38F;_LL1AD:
if((_tmp388.EqualConst).tag != 1)goto _LL1AF;_tmp389=(unsigned int)(_tmp388.EqualConst).val;
_LL1AE:{const char*_tmp5FD;void*_tmp5FC[1];struct Cyc_Int_pa_struct _tmp5FB;(
_tmp5FB.tag=1,((_tmp5FB.f1=(unsigned long)((int)_tmp389),((_tmp5FC[0]=& _tmp5FB,
Cyc_fprintf(Cyc_stderr,((_tmp5FD="==%d",_tag_dyneither(_tmp5FD,sizeof(char),5))),
_tag_dyneither(_tmp5FC,sizeof(void*),1)))))));}goto _LL1AC;_LL1AF: if((_tmp388.LessVar).tag
!= 2)goto _LL1B1;_tmp38A=(struct _tuple10)(_tmp388.LessVar).val;_tmp38B=_tmp38A.f1;
_LL1B0:{const char*_tmp601;void*_tmp600[1];struct Cyc_String_pa_struct _tmp5FF;(
_tmp5FF.tag=0,((_tmp5FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp38B->name)),((_tmp600[0]=& _tmp5FF,Cyc_fprintf(Cyc_stderr,((_tmp601="<%s",
_tag_dyneither(_tmp601,sizeof(char),4))),_tag_dyneither(_tmp600,sizeof(void*),1)))))));}
goto _LL1AC;_LL1B1: if((_tmp388.LessNumelts).tag != 3)goto _LL1B3;_tmp38C=(struct Cyc_Absyn_Vardecl*)(
_tmp388.LessNumelts).val;_LL1B2:{const char*_tmp605;void*_tmp604[1];struct Cyc_String_pa_struct
_tmp603;(_tmp603.tag=0,((_tmp603.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp38C->name)),((_tmp604[0]=& _tmp603,Cyc_fprintf(Cyc_stderr,((
_tmp605="<numelts(%s)",_tag_dyneither(_tmp605,sizeof(char),13))),_tag_dyneither(
_tmp604,sizeof(void*),1)))))));}goto _LL1AC;_LL1B3: if((_tmp388.LessConst).tag != 4)
goto _LL1B5;_tmp38D=(unsigned int)(_tmp388.LessConst).val;_LL1B4:{const char*
_tmp609;void*_tmp608[1];struct Cyc_Int_pa_struct _tmp607;(_tmp607.tag=1,((_tmp607.f1=(
unsigned long)((int)_tmp38D),((_tmp608[0]=& _tmp607,Cyc_fprintf(Cyc_stderr,((
_tmp609="<%d",_tag_dyneither(_tmp609,sizeof(char),4))),_tag_dyneither(_tmp608,
sizeof(void*),1)))))));}goto _LL1AC;_LL1B5: if((_tmp388.LessEqConst).tag != 6)goto
_LL1B7;_tmp38E=(unsigned int)(_tmp388.LessEqConst).val;_LL1B6:{const char*_tmp60D;
void*_tmp60C[1];struct Cyc_Int_pa_struct _tmp60B;(_tmp60B.tag=1,((_tmp60B.f1=(
unsigned long)((int)_tmp38E),((_tmp60C[0]=& _tmp60B,Cyc_fprintf(Cyc_stderr,((
_tmp60D="<=%d",_tag_dyneither(_tmp60D,sizeof(char),5))),_tag_dyneither(_tmp60C,
sizeof(void*),1)))))));}goto _LL1AC;_LL1B7: if((_tmp388.LessEqNumelts).tag != 5)
goto _LL1AC;_tmp38F=(struct Cyc_Absyn_Vardecl*)(_tmp388.LessEqNumelts).val;_LL1B8:{
const char*_tmp611;void*_tmp610[1];struct Cyc_String_pa_struct _tmp60F;(_tmp60F.tag=
0,((_tmp60F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp38F->name)),((_tmp610[0]=& _tmp60F,Cyc_fprintf(Cyc_stderr,((_tmp611="<=numelts(%s)",
_tag_dyneither(_tmp611,sizeof(char),14))),_tag_dyneither(_tmp610,sizeof(void*),1)))))));}
goto _LL1AC;_LL1AC:;};}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(
0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);
if(r->tl != 0){const char*_tmp614;void*_tmp613;(_tmp613=0,Cyc_fprintf(Cyc_stderr,((
_tmp614=",",_tag_dyneither(_tmp614,sizeof(char),2))),_tag_dyneither(_tmp613,
sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel
il){switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1B9:{const char*_tmp617;void*_tmp616;(
_tmp616=0,Cyc_fprintf(Cyc_stderr,((_tmp617="uninitialized",_tag_dyneither(
_tmp617,sizeof(char),14))),_tag_dyneither(_tmp616,sizeof(void*),0)));}break;case
Cyc_CfFlowInfo_ThisIL: _LL1BA:{const char*_tmp61A;void*_tmp619;(_tmp619=0,Cyc_fprintf(
Cyc_stderr,((_tmp61A="this-initialized",_tag_dyneither(_tmp61A,sizeof(char),17))),
_tag_dyneither(_tmp619,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1BB:{
const char*_tmp61D;void*_tmp61C;(_tmp61C=0,Cyc_fprintf(Cyc_stderr,((_tmp61D="all-initialized",
_tag_dyneither(_tmp61D,sizeof(char),16))),_tag_dyneither(_tmp61C,sizeof(void*),0)));}
break;}}void Cyc_CfFlowInfo_print_root(void*root){void*_tmp3AA=root;struct Cyc_Absyn_Vardecl*
_tmp3AC;struct Cyc_Absyn_Exp*_tmp3AE;void*_tmp3AF;_LL1BE: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp3AB=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp3AA;if(_tmp3AB->tag != 0)goto
_LL1C0;else{_tmp3AC=_tmp3AB->f1;}}_LL1BF:{const char*_tmp621;void*_tmp620[1];
struct Cyc_String_pa_struct _tmp61F;(_tmp61F.tag=0,((_tmp61F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3AC->name)),((
_tmp620[0]=& _tmp61F,Cyc_fprintf(Cyc_stderr,((_tmp621="Root(%s)",_tag_dyneither(
_tmp621,sizeof(char),9))),_tag_dyneither(_tmp620,sizeof(void*),1)))))));}goto
_LL1BD;_LL1C0: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp3AD=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp3AA;if(_tmp3AD->tag != 1)goto _LL1C2;else{_tmp3AE=_tmp3AD->f1;_tmp3AF=(void*)
_tmp3AD->f2;}}_LL1C1:{const char*_tmp626;void*_tmp625[2];struct Cyc_String_pa_struct
_tmp624;struct Cyc_String_pa_struct _tmp623;(_tmp623.tag=0,((_tmp623.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3AF)),((_tmp624.tag=
0,((_tmp624.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
_tmp3AE)),((_tmp625[0]=& _tmp624,((_tmp625[1]=& _tmp623,Cyc_fprintf(Cyc_stderr,((
_tmp626="MallocPt(%s,%s)",_tag_dyneither(_tmp626,sizeof(char),16))),
_tag_dyneither(_tmp625,sizeof(void*),2)))))))))))));}goto _LL1BD;_LL1C2: {struct
Cyc_CfFlowInfo_InitParam_struct*_tmp3B0=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp3AA;if(_tmp3B0->tag != 2)goto _LL1BD;}_LL1C3:{const char*_tmp629;void*_tmp628;(
_tmp628=0,Cyc_fprintf(Cyc_stderr,((_tmp629="InitParam(_,_)",_tag_dyneither(
_tmp629,sizeof(char),15))),_tag_dyneither(_tmp628,sizeof(void*),0)));}goto _LL1BD;
_LL1BD:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){Cyc_CfFlowInfo_print_root(
p->root);{struct Cyc_List_List*_tmp3BA=p->fields;while(_tmp3BA != 0){{const char*
_tmp62D;void*_tmp62C[1];struct Cyc_Int_pa_struct _tmp62B;(_tmp62B.tag=1,((_tmp62B.f1=(
unsigned long)((int)_tmp3BA->hd),((_tmp62C[0]=& _tmp62B,Cyc_fprintf(Cyc_stderr,((
_tmp62D=".%d",_tag_dyneither(_tmp62D,sizeof(char),4))),_tag_dyneither(_tmp62C,
sizeof(void*),1)))))));}_tmp3BA=_tmp3BA->tl;}{const char*_tmp630;void*_tmp62F;(
_tmp62F=0,Cyc_fprintf(Cyc_stderr,((_tmp630=" ",_tag_dyneither(_tmp630,sizeof(
char),2))),_tag_dyneither(_tmp62F,sizeof(void*),0)));};};}void Cyc_CfFlowInfo_print_list(
struct Cyc_List_List*x,void(*pr)(void*)){int _tmp3C0=1;while(x != 0){if(!_tmp3C0){{
const char*_tmp633;void*_tmp632;(_tmp632=0,Cyc_fprintf(Cyc_stderr,((_tmp633=", ",
_tag_dyneither(_tmp633,sizeof(char),3))),_tag_dyneither(_tmp632,sizeof(void*),0)));}
_tmp3C0=0;}pr((void*)x->hd);x=x->tl;}{const char*_tmp636;void*_tmp635;(_tmp635=0,
Cyc_fprintf(Cyc_stderr,((_tmp636="\n",_tag_dyneither(_tmp636,sizeof(char),2))),
_tag_dyneither(_tmp635,sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*)){struct _RegionHandle _tmp3C5=_new_region("r");
struct _RegionHandle*r=& _tmp3C5;_push_region(r);{struct _handler_cons _tmp3C6;
_push_handler(& _tmp3C6);{int _tmp3C8=0;if(setjmp(_tmp3C6.handler))_tmp3C8=1;if(!
_tmp3C8){{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple13 _tmp3C9=*((
struct _tuple13*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;{const char*_tmp639;void*_tmp638;(_tmp638=0,Cyc_fprintf(Cyc_stderr,((
_tmp639="{ ",_tag_dyneither(_tmp639,sizeof(char),3))),_tag_dyneither(_tmp638,
sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,struct _tuple13*))Cyc_Iter_next)(
iter,& _tmp3C9)){void*_tmp3CC=_tmp3C9.f1;if(!first){{const char*_tmp63C;void*
_tmp63B;(_tmp63B=0,Cyc_fprintf(Cyc_stderr,((_tmp63C=", ",_tag_dyneither(_tmp63C,
sizeof(char),3))),_tag_dyneither(_tmp63B,sizeof(void*),0)));}first=0;}pr(_tmp3CC);}{
const char*_tmp63F;void*_tmp63E;(_tmp63E=0,Cyc_fprintf(Cyc_stderr,((_tmp63F="}\n",
_tag_dyneither(_tmp63F,sizeof(char),3))),_tag_dyneither(_tmp63E,sizeof(void*),0)));};};
_pop_handler();}else{void*_tmp3C7=(void*)_exn_thrown;void*_tmp3D2=_tmp3C7;_LL1C5:;
_LL1C6:{const char*_tmp642;void*_tmp641;(_tmp641=0,Cyc_fprintf(Cyc_stderr,((
_tmp642="{ }\n",_tag_dyneither(_tmp642,sizeof(char),5))),_tag_dyneither(_tmp641,
sizeof(void*),0)));}_npop_handler(0);return;_LL1C7:;_LL1C8:(void)_throw(_tmp3D2);
_LL1C4:;}};};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval){void*
_tmp3D5=rval;enum Cyc_CfFlowInfo_InitLevel _tmp3DA;enum Cyc_CfFlowInfo_InitLevel
_tmp3DC;struct Cyc_CfFlowInfo_Place*_tmp3DE;struct Cyc_CfFlowInfo_UnionRInfo
_tmp3E0;int _tmp3E1;int _tmp3E2;struct _dyneither_ptr _tmp3E3;struct Cyc_Absyn_Exp*
_tmp3E5;int _tmp3E6;void*_tmp3E7;_LL1CA: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3D6=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3D5;if(_tmp3D6->tag != 0)goto
_LL1CC;}_LL1CB:{const char*_tmp645;void*_tmp644;(_tmp644=0,Cyc_fprintf(Cyc_stderr,((
_tmp645="Zero",_tag_dyneither(_tmp645,sizeof(char),5))),_tag_dyneither(_tmp644,
sizeof(void*),0)));}goto _LL1C9;_LL1CC: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3D7=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3D5;if(_tmp3D7->tag != 1)
goto _LL1CE;}_LL1CD:{const char*_tmp648;void*_tmp647;(_tmp647=0,Cyc_fprintf(Cyc_stderr,((
_tmp648="NotZeroAll",_tag_dyneither(_tmp648,sizeof(char),11))),_tag_dyneither(
_tmp647,sizeof(void*),0)));}goto _LL1C9;_LL1CE: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3D8=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3D5;if(_tmp3D8->tag != 2)
goto _LL1D0;}_LL1CF:{const char*_tmp64B;void*_tmp64A;(_tmp64A=0,Cyc_fprintf(Cyc_stderr,((
_tmp64B="NotZeroThis",_tag_dyneither(_tmp64B,sizeof(char),12))),_tag_dyneither(
_tmp64A,sizeof(void*),0)));}goto _LL1C9;_LL1D0: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3D9=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3D5;if(_tmp3D9->tag != 3)goto
_LL1D2;else{_tmp3DA=_tmp3D9->f1;}}_LL1D1:{const char*_tmp64E;void*_tmp64D;(
_tmp64D=0,Cyc_fprintf(Cyc_stderr,((_tmp64E="Unknown(",_tag_dyneither(_tmp64E,
sizeof(char),9))),_tag_dyneither(_tmp64D,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3DA);{const char*_tmp651;void*_tmp650;(_tmp650=0,Cyc_fprintf(Cyc_stderr,((
_tmp651=")",_tag_dyneither(_tmp651,sizeof(char),2))),_tag_dyneither(_tmp650,
sizeof(void*),0)));}goto _LL1C9;_LL1D2: {struct Cyc_CfFlowInfo_Esc_struct*_tmp3DB=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp3D5;if(_tmp3DB->tag != 4)goto _LL1D4;else{
_tmp3DC=_tmp3DB->f1;}}_LL1D3:{const char*_tmp654;void*_tmp653;(_tmp653=0,Cyc_fprintf(
Cyc_stderr,((_tmp654="Esc(",_tag_dyneither(_tmp654,sizeof(char),5))),
_tag_dyneither(_tmp653,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3DC);{const char*_tmp657;void*_tmp656;(_tmp656=0,Cyc_fprintf(Cyc_stderr,((
_tmp657=")",_tag_dyneither(_tmp657,sizeof(char),2))),_tag_dyneither(_tmp656,
sizeof(void*),0)));}goto _LL1C9;_LL1D4: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3DD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3D5;if(_tmp3DD->tag != 5)goto
_LL1D6;else{_tmp3DE=_tmp3DD->f1;}}_LL1D5:{const char*_tmp65A;void*_tmp659;(
_tmp659=0,Cyc_fprintf(Cyc_stderr,((_tmp65A="AddrOf(",_tag_dyneither(_tmp65A,
sizeof(char),8))),_tag_dyneither(_tmp659,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(
_tmp3DE);{const char*_tmp65D;void*_tmp65C;(_tmp65C=0,Cyc_fprintf(Cyc_stderr,((
_tmp65D=")",_tag_dyneither(_tmp65D,sizeof(char),2))),_tag_dyneither(_tmp65C,
sizeof(void*),0)));}goto _LL1C9;_LL1D6: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp3DF=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp3D5;if(_tmp3DF->tag != 6)goto
_LL1D8;else{_tmp3E0=_tmp3DF->f1;_tmp3E1=_tmp3E0.is_union;_tmp3E2=_tmp3E0.fieldnum;
_tmp3E3=_tmp3DF->f2;}}_LL1D7: if(_tmp3E1){{const char*_tmp660;void*_tmp65F;(
_tmp65F=0,Cyc_fprintf(Cyc_stderr,((_tmp660="AggrUnion{",_tag_dyneither(_tmp660,
sizeof(char),11))),_tag_dyneither(_tmp65F,sizeof(void*),0)));}if(_tmp3E2 != - 1){
const char*_tmp664;void*_tmp663[1];struct Cyc_Int_pa_struct _tmp662;(_tmp662.tag=1,((
_tmp662.f1=(unsigned long)_tmp3E2,((_tmp663[0]=& _tmp662,Cyc_fprintf(Cyc_stderr,((
_tmp664="tag == %d;",_tag_dyneither(_tmp664,sizeof(char),11))),_tag_dyneither(
_tmp663,sizeof(void*),1)))))));}}else{const char*_tmp667;void*_tmp666;(_tmp666=0,
Cyc_fprintf(Cyc_stderr,((_tmp667="AggrStruct{",_tag_dyneither(_tmp667,sizeof(
char),12))),_tag_dyneither(_tmp666,sizeof(void*),0)));}{int i=0;for(0;i < 
_get_dyneither_size(_tmp3E3,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp3E3.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp3E3,sizeof(void*))){
const char*_tmp66A;void*_tmp669;(_tmp669=0,Cyc_fprintf(Cyc_stderr,((_tmp66A=",",
_tag_dyneither(_tmp66A,sizeof(char),2))),_tag_dyneither(_tmp669,sizeof(void*),0)));}}}{
const char*_tmp66D;void*_tmp66C;(_tmp66C=0,Cyc_fprintf(Cyc_stderr,((_tmp66D="}",
_tag_dyneither(_tmp66D,sizeof(char),2))),_tag_dyneither(_tmp66C,sizeof(void*),0)));}
goto _LL1C9;_LL1D8: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp3E4=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp3D5;if(_tmp3E4->tag != 7)goto _LL1C9;else{_tmp3E5=_tmp3E4->f1;_tmp3E6=_tmp3E4->f2;
_tmp3E7=(void*)_tmp3E4->f3;}}_LL1D9:{const char*_tmp672;void*_tmp671[2];struct Cyc_String_pa_struct
_tmp670;struct Cyc_Int_pa_struct _tmp66F;(_tmp66F.tag=1,((_tmp66F.f1=(
unsigned long)_tmp3E6,((_tmp670.tag=0,((_tmp670.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp3E5)),((_tmp671[0]=& _tmp670,((
_tmp671[1]=& _tmp66F,Cyc_fprintf(Cyc_stderr,((_tmp672="[Consumed(%s,%d,",
_tag_dyneither(_tmp672,sizeof(char),17))),_tag_dyneither(_tmp671,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp3E7);{const char*_tmp675;void*_tmp674;(_tmp674=0,
Cyc_fprintf(Cyc_stderr,((_tmp675="]",_tag_dyneither(_tmp675,sizeof(char),2))),
_tag_dyneither(_tmp674,sizeof(void*),0)));};_LL1C9:;}static void Cyc_CfFlowInfo_print_flow_mapping(
void*root,void*rval){{const char*_tmp678;void*_tmp677;(_tmp677=0,Cyc_fprintf(Cyc_stderr,((
_tmp678="    ",_tag_dyneither(_tmp678,sizeof(char),5))),_tag_dyneither(_tmp677,
sizeof(void*),0)));}Cyc_CfFlowInfo_print_root(root);{const char*_tmp67B;void*
_tmp67A;(_tmp67A=0,Cyc_fprintf(Cyc_stderr,((_tmp67B=" --> ",_tag_dyneither(
_tmp67B,sizeof(char),6))),_tag_dyneither(_tmp67A,sizeof(void*),0)));}Cyc_CfFlowInfo_print_absrval(
rval);{const char*_tmp67E;void*_tmp67D;(_tmp67D=0,Cyc_fprintf(Cyc_stderr,((
_tmp67E="\n",_tag_dyneither(_tmp67E,sizeof(char),2))),_tag_dyneither(_tmp67D,
sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){((
void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,
d);}void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo
_tmp411=f;int _tmp412;struct _tuple11 _tmp413;struct Cyc_Dict_Dict _tmp414;_LL1DB: if((
_tmp411.BottomFL).tag != 1)goto _LL1DD;_tmp412=(int)(_tmp411.BottomFL).val;_LL1DC:{
const char*_tmp681;void*_tmp680;(_tmp680=0,Cyc_fprintf(Cyc_stderr,((_tmp681="  BottomFL\n",
_tag_dyneither(_tmp681,sizeof(char),12))),_tag_dyneither(_tmp680,sizeof(void*),0)));}
goto _LL1DA;_LL1DD: if((_tmp411.ReachableFL).tag != 2)goto _LL1DA;_tmp413=(struct
_tuple11)(_tmp411.ReachableFL).val;_tmp414=_tmp413.f1;_LL1DE:{const char*_tmp684;
void*_tmp683;(_tmp683=0,Cyc_fprintf(Cyc_stderr,((_tmp684="  ReachableFL:\n",
_tag_dyneither(_tmp684,sizeof(char),16))),_tag_dyneither(_tmp683,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp414);{const char*_tmp687;void*_tmp686;(_tmp686=
0,Cyc_fprintf(Cyc_stderr,((_tmp687="\n",_tag_dyneither(_tmp687,sizeof(char),2))),
_tag_dyneither(_tmp686,sizeof(void*),0)));}goto _LL1DA;_LL1DA:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t){void*_tmp41B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp427;struct Cyc_Absyn_DatatypeInfo _tmp429;struct Cyc_List_List*_tmp42A;struct
Cyc_List_List*_tmp42C;struct Cyc_Absyn_AggrInfo _tmp42E;struct Cyc_List_List*
_tmp42F;struct Cyc_Absyn_DatatypeFieldInfo _tmp431;struct Cyc_List_List*_tmp432;
struct Cyc_List_List*_tmp434;struct Cyc_Absyn_PtrInfo _tmp436;void*_tmp437;struct
Cyc_Absyn_PtrAtts _tmp438;void*_tmp439;struct Cyc_List_List*_tmp43C;struct Cyc_List_List*
_tmp43E;struct Cyc_Absyn_ArrayInfo _tmp440;void*_tmp441;void*_tmp443;void*_tmp445;
void*_tmp447;void*_tmp448;void*_tmp44A;_LL1E0: {struct Cyc_Absyn_VoidType_struct*
_tmp41C=(struct Cyc_Absyn_VoidType_struct*)_tmp41B;if(_tmp41C->tag != 0)goto _LL1E2;}
_LL1E1: goto _LL1E3;_LL1E2: {struct Cyc_Absyn_IntType_struct*_tmp41D=(struct Cyc_Absyn_IntType_struct*)
_tmp41B;if(_tmp41D->tag != 6)goto _LL1E4;}_LL1E3: goto _LL1E5;_LL1E4: {struct Cyc_Absyn_FloatType_struct*
_tmp41E=(struct Cyc_Absyn_FloatType_struct*)_tmp41B;if(_tmp41E->tag != 7)goto
_LL1E6;}_LL1E5: goto _LL1E7;_LL1E6: {struct Cyc_Absyn_DoubleType_struct*_tmp41F=(
struct Cyc_Absyn_DoubleType_struct*)_tmp41B;if(_tmp41F->tag != 8)goto _LL1E8;}
_LL1E7: goto _LL1E9;_LL1E8: {struct Cyc_Absyn_EnumType_struct*_tmp420=(struct Cyc_Absyn_EnumType_struct*)
_tmp41B;if(_tmp420->tag != 14)goto _LL1EA;}_LL1E9: goto _LL1EB;_LL1EA: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp421=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp41B;if(_tmp421->tag != 15)goto
_LL1EC;}_LL1EB: goto _LL1ED;_LL1EC: {struct Cyc_Absyn_TagType_struct*_tmp422=(
struct Cyc_Absyn_TagType_struct*)_tmp41B;if(_tmp422->tag != 20)goto _LL1EE;}_LL1ED:
goto _LL1EF;_LL1EE: {struct Cyc_Absyn_HeapRgn_struct*_tmp423=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp41B;if(_tmp423->tag != 21)goto _LL1F0;}_LL1EF: goto _LL1F1;_LL1F0: {struct Cyc_Absyn_Evar_struct*
_tmp424=(struct Cyc_Absyn_Evar_struct*)_tmp41B;if(_tmp424->tag != 1)goto _LL1F2;}
_LL1F1: return 0;_LL1F2: {struct Cyc_Absyn_UniqueRgn_struct*_tmp425=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp41B;if(_tmp425->tag != 22)goto _LL1F4;}_LL1F3: {void*_tmp44C=rgn;_LL215: {
struct Cyc_CfFlowInfo_UniqueRgn_k_struct*_tmp44D=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)
_tmp44C;if(_tmp44D->tag != 0)goto _LL217;}_LL216: return 1;_LL217:;_LL218: return 0;
_LL214:;}_LL1F4: {struct Cyc_Absyn_VarType_struct*_tmp426=(struct Cyc_Absyn_VarType_struct*)
_tmp41B;if(_tmp426->tag != 2)goto _LL1F6;else{_tmp427=_tmp426->f1;}}_LL1F5: {void*
_tmp44E=rgn;struct Cyc_Absyn_Tvar*_tmp451;_LL21A: {struct Cyc_CfFlowInfo_UniqueRgn_k_struct*
_tmp44F=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)_tmp44E;if(_tmp44F->tag != 0)
goto _LL21C;}_LL21B: return 0;_LL21C: {struct Cyc_CfFlowInfo_Region_k_struct*_tmp450=(
struct Cyc_CfFlowInfo_Region_k_struct*)_tmp44E;if(_tmp450->tag != 1)goto _LL219;
else{_tmp451=_tmp450->f1;}}_LL21D: return Cyc_Absyn_tvar_cmp(_tmp427,_tmp451)== 0;
_LL219:;}_LL1F6: {struct Cyc_Absyn_DatatypeType_struct*_tmp428=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp41B;if(_tmp428->tag != 3)goto _LL1F8;else{_tmp429=_tmp428->f1;_tmp42A=_tmp429.targs;}}
_LL1F7: _tmp42C=_tmp42A;goto _LL1F9;_LL1F8: {struct Cyc_Absyn_TypedefType_struct*
_tmp42B=(struct Cyc_Absyn_TypedefType_struct*)_tmp41B;if(_tmp42B->tag != 18)goto
_LL1FA;else{_tmp42C=_tmp42B->f2;}}_LL1F9: _tmp42F=_tmp42C;goto _LL1FB;_LL1FA: {
struct Cyc_Absyn_AggrType_struct*_tmp42D=(struct Cyc_Absyn_AggrType_struct*)
_tmp41B;if(_tmp42D->tag != 12)goto _LL1FC;else{_tmp42E=_tmp42D->f1;_tmp42F=_tmp42E.targs;}}
_LL1FB: _tmp432=_tmp42F;goto _LL1FD;_LL1FC: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp430=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp41B;if(_tmp430->tag != 4)
goto _LL1FE;else{_tmp431=_tmp430->f1;_tmp432=_tmp431.targs;}}_LL1FD: _tmp434=
_tmp432;goto _LL1FF;_LL1FE: {struct Cyc_Absyn_JoinEff_struct*_tmp433=(struct Cyc_Absyn_JoinEff_struct*)
_tmp41B;if(_tmp433->tag != 24)goto _LL200;else{_tmp434=_tmp433->f1;}}_LL1FF: return((
int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(
Cyc_CfFlowInfo_contains_region,rgn,_tmp434);_LL200: {struct Cyc_Absyn_PointerType_struct*
_tmp435=(struct Cyc_Absyn_PointerType_struct*)_tmp41B;if(_tmp435->tag != 5)goto
_LL202;else{_tmp436=_tmp435->f1;_tmp437=_tmp436.elt_typ;_tmp438=_tmp436.ptr_atts;
_tmp439=_tmp438.rgn;}}_LL201: return Cyc_CfFlowInfo_contains_region(rgn,_tmp439)
 || Cyc_CfFlowInfo_contains_region(rgn,_tmp437);_LL202: {struct Cyc_Absyn_FnType_struct*
_tmp43A=(struct Cyc_Absyn_FnType_struct*)_tmp41B;if(_tmp43A->tag != 10)goto _LL204;}
_LL203: return 0;_LL204: {struct Cyc_Absyn_TupleType_struct*_tmp43B=(struct Cyc_Absyn_TupleType_struct*)
_tmp41B;if(_tmp43B->tag != 11)goto _LL206;else{_tmp43C=_tmp43B->f1;}}_LL205: for(0;
_tmp43C != 0;_tmp43C=_tmp43C->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((
struct _tuple15*)_tmp43C->hd)).f2))return 1;}return 0;_LL206: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp43D=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp41B;if(_tmp43D->tag != 13)goto
_LL208;else{_tmp43E=_tmp43D->f2;}}_LL207: for(0;_tmp43E != 0;_tmp43E=_tmp43E->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp43E->hd)->type))
return 1;}return 0;_LL208: {struct Cyc_Absyn_ArrayType_struct*_tmp43F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp41B;if(_tmp43F->tag != 9)goto _LL20A;else{_tmp440=_tmp43F->f1;_tmp441=_tmp440.elt_type;}}
_LL209: _tmp443=_tmp441;goto _LL20B;_LL20A: {struct Cyc_Absyn_AccessEff_struct*
_tmp442=(struct Cyc_Absyn_AccessEff_struct*)_tmp41B;if(_tmp442->tag != 23)goto
_LL20C;else{_tmp443=(void*)_tmp442->f1;}}_LL20B: _tmp445=_tmp443;goto _LL20D;
_LL20C: {struct Cyc_Absyn_RgnsEff_struct*_tmp444=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp41B;if(_tmp444->tag != 25)goto _LL20E;else{_tmp445=(void*)_tmp444->f1;}}_LL20D:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp445);_LL20E: {struct Cyc_Absyn_DynRgnType_struct*
_tmp446=(struct Cyc_Absyn_DynRgnType_struct*)_tmp41B;if(_tmp446->tag != 17)goto
_LL210;else{_tmp447=(void*)_tmp446->f1;_tmp448=(void*)_tmp446->f2;}}_LL20F:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp447) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp448);_LL210: {struct Cyc_Absyn_RgnHandleType_struct*_tmp449=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp41B;if(_tmp449->tag != 16)goto _LL212;else{_tmp44A=(void*)_tmp449->f1;}}_LL211:
return 0;_LL212: {struct Cyc_Absyn_ValueofType_struct*_tmp44B=(struct Cyc_Absyn_ValueofType_struct*)
_tmp41B;if(_tmp44B->tag != 19)goto _LL1DF;}_LL213: return 0;_LL1DF:;}struct _tuple23{
struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(
struct _tuple23*env,void*root,void*rval){struct _tuple23 _tmp453;struct Cyc_CfFlowInfo_FlowEnv*
_tmp454;struct Cyc_Dict_Dict _tmp455;struct Cyc_Dict_Dict*_tmp456;void*_tmp457;
struct _tuple23*_tmp452=env;_tmp453=*_tmp452;_tmp454=_tmp453.f1;_tmp455=_tmp453.f2;
_tmp456=(struct Cyc_Dict_Dict*)&(*_tmp452).f2;_tmp457=_tmp453.f3;{void*_tmp458=
root;struct Cyc_Absyn_Vardecl*_tmp45A;void*_tmp45C;_LL21F: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp459=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp458;if(_tmp459->tag != 0)goto
_LL221;else{_tmp45A=_tmp459->f1;}}_LL220: if(Cyc_CfFlowInfo_contains_region(
_tmp457,_tmp45A->type))rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp454,_tmp45A->type,
0,_tmp454->unknown_none);*_tmp456=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(*_tmp456,root,rval);goto _LL21E;_LL221: {struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp45B=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp458;if(_tmp45B->tag != 1)goto _LL223;else{_tmp45C=(void*)_tmp45B->f2;}}_LL222:
if(!Cyc_CfFlowInfo_contains_region(_tmp457,_tmp45C))*_tmp456=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp456,root,rval);goto
_LL21E;_LL223: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp45D=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp458;if(_tmp45D->tag != 2)goto _LL21E;}_LL224: goto _LL21E;_LL21E:;};}static
struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp45E=Cyc_Tcutil_compress(rgn);
struct Cyc_Absyn_Tvar*_tmp460;_LL226: {struct Cyc_Absyn_VarType_struct*_tmp45F=(
struct Cyc_Absyn_VarType_struct*)_tmp45E;if(_tmp45F->tag != 2)goto _LL228;else{
_tmp460=_tmp45F->f1;}}_LL227: {struct Cyc_CfFlowInfo_Region_k_struct _tmp688;
struct Cyc_CfFlowInfo_Region_k_struct v=(_tmp688.tag=1,((_tmp688.f1=_tmp460,
_tmp688)));void*v2=(void*)& v;struct _tuple23 _tmp689;struct _tuple23 env=(_tmp689.f1=
fenv,((_tmp689.f2=fenv->mt_flowdict,((_tmp689.f3=v2,_tmp689)))));((void(*)(void(*
f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL228: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp461=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp45E;if(_tmp461->tag != 22)goto
_LL22A;}_LL229: {struct _tuple23 _tmp68A;struct _tuple23 env=(_tmp68A.f1=fenv,((
_tmp68A.f2=fenv->mt_flowdict,((_tmp68A.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,
_tmp68A)))));((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return
env.f2;}_LL22A:;_LL22B: {struct Cyc_Core_Impossible_struct _tmp690;const char*
_tmp68F;struct Cyc_Core_Impossible_struct*_tmp68E;(int)_throw((void*)((_tmp68E=
_cycalloc(sizeof(*_tmp68E)),((_tmp68E[0]=((_tmp690.tag=Cyc_Core_Impossible,((
_tmp690.f1=((_tmp68F="kill_flowdict_region",_tag_dyneither(_tmp68F,sizeof(char),
21))),_tmp690)))),_tmp68E)))));}_LL225:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){union
Cyc_CfFlowInfo_FlowInfo _tmp468=f;int _tmp469;struct _tuple11 _tmp46A;struct Cyc_Dict_Dict
_tmp46B;struct Cyc_List_List*_tmp46C;_LL22D: if((_tmp468.BottomFL).tag != 1)goto
_LL22F;_tmp469=(int)(_tmp468.BottomFL).val;_LL22E: return f;_LL22F: if((_tmp468.ReachableFL).tag
!= 2)goto _LL22C;_tmp46A=(struct _tuple11)(_tmp468.ReachableFL).val;_tmp46B=
_tmp46A.f1;_tmp46C=_tmp46A.f2;_LL230: {struct Cyc_Dict_Dict _tmp46D=Cyc_CfFlowInfo_kill_flowdict_region(
fenv,_tmp46B,rgn);struct Cyc_List_List*_tmp46E=Cyc_CfFlowInfo_kill_relns_region(
_tmp46C,rgn);return Cyc_CfFlowInfo_ReachableFL(_tmp46D,_tmp46E);}_LL22C:;}
