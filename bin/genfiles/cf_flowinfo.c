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
Cyc_CfFlowInfo_AbsLVal _tmp486;return((_tmp486.PlaceL).val=x,(((_tmp486.PlaceL).tag=
1,_tmp486)));}union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){union Cyc_CfFlowInfo_AbsLVal
_tmp487;return((_tmp487.UnknownL).val=0,(((_tmp487.UnknownL).tag=2,_tmp487)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo
_tmp488;return((_tmp488.BottomFL).val=0,(((_tmp488.BottomFL).tag=1,_tmp488)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,
struct Cyc_List_List*r){struct _tuple11 _tmp48B;union Cyc_CfFlowInfo_FlowInfo _tmp48A;
return((_tmp48A.ReachableFL).val=((_tmp48B.f1=fd,((_tmp48B.f2=r,_tmp48B)))),(((
_tmp48A.ReachableFL).tag=2,_tmp48A)));}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,{.Null_c={1,0}}};static
struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp4C2;struct Cyc_CfFlowInfo_MallocPt_struct _tmp4C1;
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp4C0;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4BF;struct Cyc_CfFlowInfo_Esc_struct _tmp4BE;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4BD;struct Cyc_CfFlowInfo_Esc_struct _tmp4BC;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4BB;struct Cyc_CfFlowInfo_Esc_struct _tmp4BA;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4B9;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4B8;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4B7;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4B6;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4B5;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4B4;struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4B3;struct Cyc_CfFlowInfo_NotZeroThis_struct _tmp4B2;struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4B1;struct Cyc_CfFlowInfo_NotZeroAll_struct _tmp4B0;struct Cyc_CfFlowInfo_Zero_struct*
_tmp4AF;struct Cyc_CfFlowInfo_Zero_struct _tmp4AE;struct Cyc_CfFlowInfo_FlowEnv*
_tmp4AD;return(_tmp4AD=_region_malloc(r,sizeof(*_tmp4AD)),((_tmp4AD->r=r,((
_tmp4AD->zero=(void*)((_tmp4AF=_region_malloc(r,sizeof(*_tmp4AF)),((_tmp4AF[0]=((
_tmp4AE.tag=0,_tmp4AE)),_tmp4AF)))),((_tmp4AD->notzeroall=(void*)((_tmp4B1=
_region_malloc(r,sizeof(*_tmp4B1)),((_tmp4B1[0]=((_tmp4B0.tag=1,_tmp4B0)),
_tmp4B1)))),((_tmp4AD->notzerothis=(void*)((_tmp4B3=_region_malloc(r,sizeof(*
_tmp4B3)),((_tmp4B3[0]=((_tmp4B2.tag=2,_tmp4B2)),_tmp4B3)))),((_tmp4AD->unknown_none=(
void*)((_tmp4B5=_region_malloc(r,sizeof(*_tmp4B5)),((_tmp4B5[0]=((_tmp4B4.tag=3,((
_tmp4B4.f1=Cyc_CfFlowInfo_NoneIL,_tmp4B4)))),_tmp4B5)))),((_tmp4AD->unknown_this=(
void*)((_tmp4B7=_region_malloc(r,sizeof(*_tmp4B7)),((_tmp4B7[0]=((_tmp4B6.tag=3,((
_tmp4B6.f1=Cyc_CfFlowInfo_ThisIL,_tmp4B6)))),_tmp4B7)))),((_tmp4AD->unknown_all=(
void*)((_tmp4B9=_region_malloc(r,sizeof(*_tmp4B9)),((_tmp4B9[0]=((_tmp4B8.tag=3,((
_tmp4B8.f1=Cyc_CfFlowInfo_AllIL,_tmp4B8)))),_tmp4B9)))),((_tmp4AD->esc_none=(
void*)((_tmp4BB=_region_malloc(r,sizeof(*_tmp4BB)),((_tmp4BB[0]=((_tmp4BA.tag=4,((
_tmp4BA.f1=Cyc_CfFlowInfo_NoneIL,_tmp4BA)))),_tmp4BB)))),((_tmp4AD->esc_this=(
void*)((_tmp4BD=_region_malloc(r,sizeof(*_tmp4BD)),((_tmp4BD[0]=((_tmp4BC.tag=4,((
_tmp4BC.f1=Cyc_CfFlowInfo_ThisIL,_tmp4BC)))),_tmp4BD)))),((_tmp4AD->esc_all=(
void*)((_tmp4BF=_region_malloc(r,sizeof(*_tmp4BF)),((_tmp4BF[0]=((_tmp4BE.tag=4,((
_tmp4BE.f1=Cyc_CfFlowInfo_AllIL,_tmp4BE)))),_tmp4BF)))),((_tmp4AD->mt_flowdict=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(
r,Cyc_CfFlowInfo_root_cmp),((_tmp4AD->mt_place_set=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp4AD->dummy_place=((_tmp4C2=
_region_malloc(r,sizeof(*_tmp4C2)),((_tmp4C2->root=(void*)((_tmp4C0=
_region_malloc(r,sizeof(*_tmp4C0)),((_tmp4C0[0]=((_tmp4C1.tag=1,((_tmp4C1.f1=&
dummy_exp,((_tmp4C1.f2=(void*)((void*)& Cyc_Absyn_VoidType_val),_tmp4C1)))))),
_tmp4C0)))),((_tmp4C2->fields=0,_tmp4C2)))))),_tmp4AD)))))))))))))))))))))))))));}
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*
loc){if(set != 0){if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,
place))return 1;else{*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,
struct Cyc_Position_Segment*v))Cyc_Dict_insert)(*set,place,loc);}}return 0;}struct
_dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
void*_tmp1F;struct Cyc_List_List*_tmp20;struct Cyc_CfFlowInfo_Place _tmp1E=*place;
_tmp1F=_tmp1E.root;_tmp20=_tmp1E.fields;{void*_tmp21=_tmp1F;struct Cyc_Absyn_Vardecl*
_tmp23;_LL1: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp22=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp21;if(_tmp22->tag != 0)goto _LL3;else{_tmp23=_tmp22->f1;}}_LL2: if(_tmp20 == 0)
return Cyc_Absynpp_qvar2string(_tmp23->name);else{const char*_tmp4C6;void*_tmp4C5[
1];struct Cyc_String_pa_struct _tmp4C4;return(struct _dyneither_ptr)((_tmp4C4.tag=0,((
_tmp4C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp23->name)),((_tmp4C5[0]=& _tmp4C4,Cyc_aprintf(((_tmp4C6="reachable from %s",
_tag_dyneither(_tmp4C6,sizeof(char),18))),_tag_dyneither(_tmp4C5,sizeof(void*),1))))))));}
_LL3:;_LL4: {const char*_tmp4C9;void*_tmp4C8;(_tmp4C8=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4C9="error locations not for VarRoots",
_tag_dyneither(_tmp4C9,sizeof(char),33))),_tag_dyneither(_tmp4C8,sizeof(void*),0)));}
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
return n;n=n + 1;}{struct Cyc_Core_Impossible_struct _tmp4D6;const char*_tmp4D5;void*
_tmp4D4[1];struct Cyc_String_pa_struct _tmp4D3;struct Cyc_Core_Impossible_struct*
_tmp4D2;(int)_throw((void*)((_tmp4D2=_cycalloc(sizeof(*_tmp4D2)),((_tmp4D2[0]=((
_tmp4D6.tag=Cyc_Core_Impossible,((_tmp4D6.f1=(struct _dyneither_ptr)((_tmp4D3.tag=
0,((_tmp4D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp4D4[0]=&
_tmp4D3,Cyc_aprintf(((_tmp4D5="get_field_index_fs failed: %s",_tag_dyneither(
_tmp4D5,sizeof(char),30))),_tag_dyneither(_tmp4D4,sizeof(void*),1)))))))),
_tmp4D6)))),_tmp4D2)))));};}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f){void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp3D;union Cyc_Absyn_AggrInfoU _tmp3E;struct Cyc_List_List*_tmp40;_LLB: {struct
Cyc_Absyn_AggrType_struct*_tmp3C=(struct Cyc_Absyn_AggrType_struct*)_tmp3B;if(
_tmp3C->tag != 12)goto _LLD;else{_tmp3D=_tmp3C->f1;_tmp3E=_tmp3D.aggr_info;}}_LLC: {
struct Cyc_List_List*_tmp41=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(
_tmp3E))->impl))->fields;_tmp40=_tmp41;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp3F=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp3B;if(_tmp3F->tag != 13)goto _LLF;
else{_tmp40=_tmp3F->f2;}}_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp40,f);
_LLF:;_LL10: {struct Cyc_Core_Impossible_struct _tmp4E3;const char*_tmp4E2;void*
_tmp4E1[1];struct Cyc_String_pa_struct _tmp4E0;struct Cyc_Core_Impossible_struct*
_tmp4DF;(int)_throw((void*)((_tmp4DF=_cycalloc(sizeof(*_tmp4DF)),((_tmp4DF[0]=((
_tmp4E3.tag=Cyc_Core_Impossible,((_tmp4E3.f1=(struct _dyneither_ptr)((_tmp4E0.tag=
0,((_tmp4E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp4E1[0]=& _tmp4E0,Cyc_aprintf(((_tmp4E2="get_field_index failed: %s",
_tag_dyneither(_tmp4E2,sizeof(char),27))),_tag_dyneither(_tmp4E1,sizeof(void*),1)))))))),
_tmp4E3)))),_tmp4DF)))));}_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((
int)r1 == (int)r2)return 0;{struct _tuple0 _tmp4E4;struct _tuple0 _tmp48=(_tmp4E4.f1=
r1,((_tmp4E4.f2=r2,_tmp4E4)));void*_tmp49;struct Cyc_Absyn_Vardecl*_tmp4B;void*
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
else{_tmp6E=_tmp6D->f1;}}_LL22:{struct _dyneither_ptr*_tmp4F1;const char*_tmp4F0;
void*_tmp4EF[1];struct Cyc_String_pa_struct _tmp4EE;struct Cyc_List_List*_tmp4ED;sl=((
_tmp4ED=_cycalloc(sizeof(*_tmp4ED)),((_tmp4ED->hd=((_tmp4F1=_cycalloc(sizeof(*
_tmp4F1)),((_tmp4F1[0]=((_tmp4EE.tag=0,((_tmp4EE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp6E->name).f2),((_tmp4EF[0]=& _tmp4EE,Cyc_aprintf(((
_tmp4F0="%s",_tag_dyneither(_tmp4F0,sizeof(char),3))),_tag_dyneither(_tmp4EF,
sizeof(void*),1)))))))),_tmp4F1)))),((_tmp4ED->tl=sl,_tmp4ED))))));}goto _LL20;
_LL23: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp6F=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp6C;if(_tmp6F->tag != 1)goto _LL25;else{_tmp70=_tmp6F->f1;}}_LL24:{struct
_dyneither_ptr*_tmp4FE;const char*_tmp4FD;void*_tmp4FC[1];struct Cyc_Int_pa_struct
_tmp4FB;struct Cyc_List_List*_tmp4FA;sl=((_tmp4FA=_cycalloc(sizeof(*_tmp4FA)),((
_tmp4FA->hd=((_tmp4FE=_cycalloc(sizeof(*_tmp4FE)),((_tmp4FE[0]=((_tmp4FB.tag=1,((
_tmp4FB.f1=(unsigned long)((int)_tmp70),((_tmp4FC[0]=& _tmp4FB,Cyc_aprintf(((
_tmp4FD="mpt%d",_tag_dyneither(_tmp4FD,sizeof(char),6))),_tag_dyneither(_tmp4FC,
sizeof(void*),1)))))))),_tmp4FE)))),((_tmp4FA->tl=sl,_tmp4FA))))));}goto _LL20;
_LL25: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp71=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp6C;if(_tmp71->tag != 2)goto _LL20;else{_tmp72=_tmp71->f1;}}_LL26:{struct
_dyneither_ptr*_tmp50B;const char*_tmp50A;void*_tmp509[1];struct Cyc_Int_pa_struct
_tmp508;struct Cyc_List_List*_tmp507;sl=((_tmp507=_cycalloc(sizeof(*_tmp507)),((
_tmp507->hd=((_tmp50B=_cycalloc(sizeof(*_tmp50B)),((_tmp50B[0]=((_tmp508.tag=1,((
_tmp508.f1=(unsigned long)_tmp72,((_tmp509[0]=& _tmp508,Cyc_aprintf(((_tmp50A="param%d",
_tag_dyneither(_tmp50A,sizeof(char),8))),_tag_dyneither(_tmp509,sizeof(void*),1)))))))),
_tmp50B)))),((_tmp507->tl=sl,_tmp507))))));}goto _LL20;_LL20:;}for(0;_tmp6B != 0;
_tmp6B=_tmp6B->tl){struct _dyneither_ptr*_tmp518;const char*_tmp517;void*_tmp516[1];
struct Cyc_Int_pa_struct _tmp515;struct Cyc_List_List*_tmp514;sl=((_tmp514=
_cycalloc(sizeof(*_tmp514)),((_tmp514->hd=((_tmp518=_cycalloc(sizeof(*_tmp518)),((
_tmp518[0]=((_tmp515.tag=1,((_tmp515.f1=(unsigned long)((int)_tmp6B->hd),((
_tmp516[0]=& _tmp515,Cyc_aprintf(((_tmp517="%d",_tag_dyneither(_tmp517,sizeof(
char),3))),_tag_dyneither(_tmp516,sizeof(void*),1)))))))),_tmp518)))),((_tmp514->tl=
sl,_tmp514))))));}{struct Cyc_String_pa_struct _tmp523;const char*_tmp522;void*
_tmp521[1];const char*_tmp520;struct _dyneither_ptr*_tmp51F;struct _dyneither_ptr*
_tmp87=(_tmp51F=_cycalloc(sizeof(*_tmp51F)),((_tmp51F[0]=((_tmp523.tag=0,((
_tmp523.f1=(struct _dyneither_ptr)((_tmp522="",_tag_dyneither(_tmp522,sizeof(char),
1))),((_tmp521[0]=& _tmp523,Cyc_aprintf(((_tmp520="%s",_tag_dyneither(_tmp520,
sizeof(char),3))),_tag_dyneither(_tmp521,sizeof(void*),1)))))))),_tmp51F)));for(
0;sl != 0;sl=sl->tl){const char*_tmp528;void*_tmp527[2];struct Cyc_String_pa_struct
_tmp526;struct Cyc_String_pa_struct _tmp525;*_tmp87=((_tmp525.tag=0,((_tmp525.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp87),((_tmp526.tag=0,((_tmp526.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((
_tmp527[0]=& _tmp526,((_tmp527[1]=& _tmp525,Cyc_aprintf(((_tmp528="%s.%s",
_tag_dyneither(_tmp528,sizeof(char),6))),_tag_dyneither(_tmp527,sizeof(void*),2))))))))))))));}
return _tmp87;};}static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct
_dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp52E(
struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp52D,unsigned int*_tmp52C,
void***_tmp52A){for(*_tmp52D=0;*_tmp52D < *_tmp52C;(*_tmp52D)++){(*_tmp52A)[*
_tmp52D]=(*fenv)->unknown_all;}}struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,
void*leafval){unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
fs);unsigned int _tmp52D;unsigned int _tmp52C;struct _dyneither_ptr _tmp52B;void**
_tmp52A;unsigned int _tmp529;struct _dyneither_ptr d=(_tmp529=sz,((_tmp52A=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp529)),((_tmp52B=
_tag_dyneither(_tmp52A,sizeof(void*),_tmp529),((((_tmp52C=_tmp529,_tmp52E(& fenv,&
_tmp52D,& _tmp52C,& _tmp52A))),_tmp52B)))))));{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
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
_tmp9B,_tmp9C,f->type);struct Cyc_Absyn_Aggrfield*_tmp52F;return(_tmp52F=
_region_malloc(_tmp9B,sizeof(*_tmp52F)),((_tmp52F->name=f->name,((_tmp52F->tq=f->tq,((
_tmp52F->type=_tmp9D,((_tmp52F->width=f->width,((_tmp52F->attributes=f->attributes,
_tmp52F)))))))))));};}static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*
targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){struct
_RegionHandle _tmp9F=_new_region("temp");struct _RegionHandle*temp=& _tmp9F;
_push_region(temp);{struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,tvs,targs);struct _tuple14 _tmp530;struct _tuple14
_tmpA0=(_tmp530.f1=temp,((_tmp530.f2=inst,_tmp530)));struct Cyc_List_List*subs_fs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(
struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmpA0,fs);struct
_dyneither_ptr _tmpA1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,
no_init_bits_only,leafval);_npop_handler(0);return _tmpA1;};_pop_region(temp);}
struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,
void*leafval);static void _tmp536(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*
_tmp535,unsigned int*_tmp534,void***_tmp532){for(*_tmp535=0;*_tmp535 < *_tmp534;(*
_tmp535)++){(*_tmp532)[*_tmp535]=(*fenv)->unknown_all;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
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
_tmpAA);unsigned int _tmp535;unsigned int _tmp534;struct _dyneither_ptr _tmp533;void**
_tmp532;unsigned int _tmp531;struct _dyneither_ptr d=(_tmp531=sz,((_tmp532=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp531)),((_tmp533=
_tag_dyneither(_tmp532,sizeof(void*),_tmp531),((((_tmp534=_tmp531,_tmp536(& fenv,&
_tmp535,& _tmp534,& _tmp532))),_tmp533)))))));{int i=0;for(0;i < sz;++ i){*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,no_init_bits_only,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(
_tmpAA))->hd)).f2,leafval);_tmpAA=_tmpAA->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp53C;struct Cyc_CfFlowInfo_UnionRInfo _tmp53B;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp53A;return(void*)((_tmp53A=_region_malloc(fenv->r,sizeof(*_tmp53A)),((
_tmp53A[0]=((_tmp53C.tag=6,((_tmp53C.f1=((_tmp53B.is_union=0,((_tmp53B.fieldnum=
- 1,_tmp53B)))),((_tmp53C.f2=d,_tmp53C)))))),_tmp53A))));};}_LL2C: {struct Cyc_Absyn_AggrType_struct*
_tmpAB=(struct Cyc_Absyn_AggrType_struct*)_tmpA3;if(_tmpAB->tag != 12)goto _LL2E;
else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAC.aggr_info;_tmpAE=_tmpAC.targs;}}_LL2D: {
struct Cyc_Absyn_Aggrdecl*_tmpC3=Cyc_Absyn_get_known_aggrdecl(_tmpAD);if(_tmpC3->impl
== 0)goto _LL27;{struct Cyc_List_List*_tmpC4=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpC3->impl))->fields;if(_tmpAE == 0){_tmpB0=_tmpC3->kind;_tmpB1=
_tmpC4;goto _LL2F;}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp542;struct Cyc_CfFlowInfo_UnionRInfo
_tmp541;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp540;return(void*)((_tmp540=
_region_malloc(fenv->r,sizeof(*_tmp540)),((_tmp540[0]=((_tmp542.tag=6,((_tmp542.f1=((
_tmp541.is_union=_tmpC3->kind == Cyc_Absyn_UnionA,((_tmp541.fieldnum=- 1,_tmp541)))),((
_tmp542.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpC3->tvs,
_tmpAE,_tmpC4,_tmpC3->kind == Cyc_Absyn_UnionA,leafval),_tmp542)))))),_tmp540))));};};}
_LL2E: {struct Cyc_Absyn_AnonAggrType_struct*_tmpAF=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA3;if(_tmpAF->tag != 13)goto _LL30;else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpAF->f2;}}
_LL2F: {struct Cyc_CfFlowInfo_Aggregate_struct _tmp548;struct Cyc_CfFlowInfo_UnionRInfo
_tmp547;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp546;return(void*)((_tmp546=
_region_malloc(fenv->r,sizeof(*_tmp546)),((_tmp546[0]=((_tmp548.tag=6,((_tmp548.f1=((
_tmp547.is_union=_tmpB0 == Cyc_Absyn_UnionA,((_tmp547.fieldnum=- 1,_tmp547)))),((
_tmp548.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpB1,_tmpB0 == Cyc_Absyn_UnionA,
leafval),_tmp548)))))),_tmp546))));}_LL30:{struct Cyc_Absyn_ArrayType_struct*
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
Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){void*_tmpD0=r;
struct Cyc_Absyn_Exp*_tmpD2;int _tmpD3;void*_tmpD4;struct Cyc_CfFlowInfo_UnionRInfo
_tmpD6;int _tmpD7;int _tmpD8;struct _dyneither_ptr _tmpD9;_LL40: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmpD1=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmpD0;if(_tmpD1->tag != 7)goto
_LL42;else{_tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;_tmpD4=(void*)_tmpD1->f3;}}_LL41:
if(_tmpD2 == e  && _tmpD3 == env_iteration){*needs_unconsume=1;return 0;}return 1;
_LL42: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmpD5=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpD0;if(_tmpD5->tag != 6)goto _LL44;else{_tmpD6=_tmpD5->f1;_tmpD7=_tmpD6.is_union;
_tmpD8=_tmpD6.fieldnum;_tmpD9=_tmpD5->f2;}}_LL43: if(!_tmpD7  || _tmpD8 == - 1){
unsigned int _tmpDA=_get_dyneither_size(_tmpD9,sizeof(void*));{int i=0;for(0;i < 
_tmpDA;++ i){if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)
_check_dyneither_subscript(_tmpD9,sizeof(void*),i)),needs_unconsume))return 1;}}
return 0;}else{return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)
_check_dyneither_subscript(_tmpD9,sizeof(void*),_tmpD8)),needs_unconsume);}_LL44:;
_LL45: return 0;_LL3F:;}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*
fenv,void*r);static void _tmp54E(struct _dyneither_ptr*_tmpE2,unsigned int*_tmp54D,
unsigned int*_tmp54C,void***_tmp54A){for(*_tmp54D=0;*_tmp54D < *_tmp54C;(*_tmp54D)
++){(*_tmp54A)[*_tmp54D]=*((void**)_check_dyneither_subscript(*_tmpE2,sizeof(
void*),(int)*_tmp54D));}}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*
fenv,void*r){void*_tmpDB=r;struct Cyc_Absyn_Exp*_tmpDD;int _tmpDE;void*_tmpDF;
struct Cyc_CfFlowInfo_UnionRInfo _tmpE1;struct _dyneither_ptr _tmpE2;_LL47: {struct
Cyc_CfFlowInfo_Consumed_struct*_tmpDC=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpDB;if(_tmpDC->tag != 7)goto _LL49;else{_tmpDD=_tmpDC->f1;_tmpDE=_tmpDC->f2;
_tmpDF=(void*)_tmpDC->f3;}}_LL48: return _tmpDF;_LL49: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpE0=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpDB;if(_tmpE0->tag != 6)goto
_LL4B;else{_tmpE1=_tmpE0->f1;_tmpE2=_tmpE0->f2;}}_LL4A: {unsigned int _tmpE3=
_get_dyneither_size(_tmpE2,sizeof(void*));int change=0;unsigned int _tmp54D;
unsigned int _tmp54C;struct _dyneither_ptr _tmp54B;void**_tmp54A;unsigned int
_tmp549;struct _dyneither_ptr d2=(_tmp549=_tmpE3,((_tmp54A=(void**)_region_malloc(
fenv->r,_check_times(sizeof(void*),_tmp549)),((_tmp54B=_tag_dyneither(_tmp54A,
sizeof(void*),_tmp549),((((_tmp54C=_tmp549,_tmp54E(& _tmpE2,& _tmp54D,& _tmp54C,&
_tmp54A))),_tmp54B)))))));{int i=0;for(0;i < _tmpE3;++ i){*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(
fenv,*((void**)_check_dyneither_subscript(_tmpE2,sizeof(void*),i)));if(*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmpE2,sizeof(void*),i)))change=1;}}if(change){struct
Cyc_CfFlowInfo_Aggregate_struct _tmp551;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp550;return(void*)((_tmp550=_region_malloc(fenv->r,sizeof(*_tmp550)),((
_tmp550[0]=((_tmp551.tag=6,((_tmp551.f1=_tmpE1,((_tmp551.f2=d2,_tmp551)))))),
_tmp550))));}else{return r;}}_LL4B:;_LL4C: return r;_LL46:;}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r);static void _tmp55B(struct Cyc_CfFlowInfo_FlowEnv**fenv,
unsigned int*_tmp55A,unsigned int*_tmp559,void***_tmp557){for(*_tmp55A=0;*
_tmp55A < *_tmp559;(*_tmp55A)++){(*_tmp557)[*_tmp55A]=(*fenv)->unknown_all;}}
static void _tmp564(unsigned int*_tmp563,unsigned int*_tmp562,void***_tmp560,
struct _dyneither_ptr*_tmp104){for(*_tmp563=0;*_tmp563 < *_tmp562;(*_tmp563)++){(*
_tmp560)[*_tmp563]=((void**)(*_tmp104).curr)[(int)*_tmp563];}}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r){if(Cyc_Tcutil_is_noalias_pointer(t)){struct Cyc_CfFlowInfo_Consumed_struct
_tmp554;struct Cyc_CfFlowInfo_Consumed_struct*_tmp553;return(void*)((_tmp553=
_region_malloc(fenv->r,sizeof(*_tmp553)),((_tmp553[0]=((_tmp554.tag=7,((_tmp554.f1=
consumer,((_tmp554.f2=iteration,((_tmp554.f3=(void*)r,_tmp554)))))))),_tmp553))));}{
struct _tuple0 _tmp555;struct _tuple0 _tmpED=(_tmp555.f1=Cyc_Tcutil_compress(t),((
_tmp555.f2=r,_tmp555)));void*_tmpEE;struct Cyc_List_List*_tmpF0;void*_tmpF1;
struct Cyc_CfFlowInfo_UnionRInfo _tmpF3;struct _dyneither_ptr _tmpF4;void*_tmpF5;
struct Cyc_Absyn_AggrInfo _tmpF7;union Cyc_Absyn_AggrInfoU _tmpF8;void*_tmpF9;struct
Cyc_CfFlowInfo_UnionRInfo _tmpFB;struct _dyneither_ptr _tmpFC;void*_tmpFD;enum Cyc_Absyn_AggrKind
_tmpFF;struct Cyc_List_List*_tmp100;void*_tmp101;struct Cyc_CfFlowInfo_UnionRInfo
_tmp103;struct _dyneither_ptr _tmp104;_LL4E: _tmpEE=_tmpED.f1;{struct Cyc_Absyn_TupleType_struct*
_tmpEF=(struct Cyc_Absyn_TupleType_struct*)_tmpEE;if(_tmpEF->tag != 11)goto _LL50;
else{_tmpF0=_tmpEF->f1;}};_tmpF1=_tmpED.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpF2=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpF1;if(_tmpF2->tag != 6)goto
_LL50;else{_tmpF3=_tmpF2->f1;_tmpF4=_tmpF2->f2;}};_LL4F: {unsigned int _tmp105=
_get_dyneither_size(_tmpF4,sizeof(void*));unsigned int _tmp55A;unsigned int
_tmp559;struct _dyneither_ptr _tmp558;void**_tmp557;unsigned int _tmp556;struct
_dyneither_ptr d2=(_tmp556=_tmp105,((_tmp557=(void**)_region_malloc(fenv->r,
_check_times(sizeof(void*),_tmp556)),((_tmp558=_tag_dyneither(_tmp557,sizeof(
void*),_tmp556),((((_tmp559=_tmp556,_tmp55B(& fenv,& _tmp55A,& _tmp559,& _tmp557))),
_tmp558)))))));{int i=0;for(0;i < _tmp105;++ i){*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmpF0))->hd)).f2,
consumer,iteration,*((void**)_check_dyneither_subscript(_tmpF4,sizeof(void*),i)));
_tmpF0=_tmpF0->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp55E;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp55D;return(void*)((_tmp55D=_region_malloc(fenv->r,sizeof(*_tmp55D)),((
_tmp55D[0]=((_tmp55E.tag=6,((_tmp55E.f1=_tmpF3,((_tmp55E.f2=d2,_tmp55E)))))),
_tmp55D))));};}_LL50: _tmpF5=_tmpED.f1;{struct Cyc_Absyn_AggrType_struct*_tmpF6=(
struct Cyc_Absyn_AggrType_struct*)_tmpF5;if(_tmpF6->tag != 12)goto _LL52;else{
_tmpF7=_tmpF6->f1;_tmpF8=_tmpF7.aggr_info;}};_tmpF9=_tmpED.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpFA=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpF9;if(_tmpFA->tag != 6)goto
_LL52;else{_tmpFB=_tmpFA->f1;_tmpFC=_tmpFA->f2;}};_LL51: {struct Cyc_Absyn_Aggrdecl*
_tmp10C=Cyc_Absyn_get_known_aggrdecl(_tmpF8);if(_tmp10C->impl == 0)return r;_tmpFF=
_tmp10C->kind;_tmp100=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp10C->impl))->fields;
_tmp103=_tmpFB;_tmp104=_tmpFC;goto _LL53;}_LL52: _tmpFD=_tmpED.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmpFE=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpFD;if(_tmpFE->tag != 13)goto
_LL54;else{_tmpFF=_tmpFE->f1;_tmp100=_tmpFE->f2;}};_tmp101=_tmpED.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp102=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp101;if(_tmp102->tag != 6)goto
_LL54;else{_tmp103=_tmp102->f1;_tmp104=_tmp102->f2;}};_LL53: {unsigned int
_tmp563;unsigned int _tmp562;struct _dyneither_ptr _tmp561;void**_tmp560;
unsigned int _tmp55F;struct _dyneither_ptr d2=(_tmp55F=_get_dyneither_size(_tmp104,
sizeof(void*)),((_tmp560=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),
_tmp55F)),((_tmp561=_tag_dyneither(_tmp560,sizeof(void*),_tmp55F),((((_tmp562=
_tmp55F,_tmp564(& _tmp563,& _tmp562,& _tmp560,& _tmp104))),_tmp561)))))));
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp100);{int i=0;for(0;i < sz;(i ++,_tmp100=_tmp100->tl)){struct Cyc_Absyn_Aggrfield
_tmp10E;struct _dyneither_ptr*_tmp10F;void*_tmp110;struct Cyc_Absyn_Aggrfield*
_tmp10D=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp100))->hd;
_tmp10E=*_tmp10D;_tmp10F=_tmp10E.name;_tmp110=_tmp10E.type;if(
_get_dyneither_size(*_tmp10F,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,_tmp110,consumer,iteration,*((void**)_check_dyneither_subscript(_tmp104,
sizeof(void*),i)));}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp567;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp566;return(void*)((_tmp566=_region_malloc(fenv->r,sizeof(*_tmp566)),((
_tmp566[0]=((_tmp567.tag=6,((_tmp567.f1=_tmp103,((_tmp567.f2=d2,_tmp567)))))),
_tmp566))));};}_LL54:;_LL55: return r;_LL4D:;};}struct _tuple16{struct Cyc_CfFlowInfo_Place*
f1;struct Cyc_Position_Segment*f2;};static int Cyc_CfFlowInfo_prefix_of_member(
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){struct _RegionHandle
_tmp117=_new_region("r");struct _RegionHandle*r=& _tmp117;_push_region(r);{struct
_tuple16 _tmp568;struct _tuple16 _tmp118=(_tmp568.f1=place,((_tmp568.f2=0,_tmp568)));
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct
Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple16*))Cyc_Iter_next)(iter,& _tmp118)){struct Cyc_CfFlowInfo_Place*
_tmp119=_tmp118.f1;void*_tmp11B;struct Cyc_List_List*_tmp11C;struct Cyc_CfFlowInfo_Place
_tmp11A=*place;_tmp11B=_tmp11A.root;_tmp11C=_tmp11A.fields;{void*_tmp11E;struct
Cyc_List_List*_tmp11F;struct Cyc_CfFlowInfo_Place _tmp11D=*_tmp119;_tmp11E=_tmp11D.root;
_tmp11F=_tmp11D.fields;if(Cyc_CfFlowInfo_root_cmp(_tmp11B,_tmp11E)!= 0)continue;
for(0;_tmp11C != 0  && _tmp11F != 0;(_tmp11C=_tmp11C->tl,_tmp11F=_tmp11F->tl)){if((
int)_tmp11C->hd != (int)_tmp11F->hd)break;}if(_tmp11C == 0){int _tmp120=1;
_npop_handler(0);return _tmp120;}};}{int _tmp121=0;_npop_handler(0);return _tmp121;};};
_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*
places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,
struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){struct Cyc_List_List*
_tmp569;pile->places=((_tmp569=_region_malloc(pile->rgn,sizeof(*_tmp569)),((
_tmp569->hd=place,((_tmp569->tl=pile->places,_tmp569))))));}}static void Cyc_CfFlowInfo_add_places(
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
old_val);static void _tmp56F(struct _RegionHandle**r,void**new_val,unsigned int*
_tmp56E,unsigned int*_tmp56D,void***_tmp56B,struct _dyneither_ptr*_tmp130){for(*
_tmp56E=0;*_tmp56E < *_tmp56D;(*_tmp56E)++){(*_tmp56B)[*_tmp56E]=Cyc_CfFlowInfo_insert_place_inner(*
r,*new_val,((void**)(*_tmp130).curr)[(int)*_tmp56E]);}}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmp12C=old_val;struct Cyc_CfFlowInfo_UnionRInfo
_tmp12E;int _tmp12F;struct _dyneither_ptr _tmp130;_LL60: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp12D=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp12C;if(_tmp12D->tag != 6)goto
_LL62;else{_tmp12E=_tmp12D->f1;_tmp12F=_tmp12E.is_union;_tmp130=_tmp12D->f2;}}
_LL61: {unsigned int _tmp56E;unsigned int _tmp56D;struct _dyneither_ptr _tmp56C;void**
_tmp56B;unsigned int _tmp56A;struct _dyneither_ptr d2=(_tmp56A=_get_dyneither_size(
_tmp130,sizeof(void*)),((_tmp56B=(void**)_region_malloc(r,_check_times(sizeof(
void*),_tmp56A)),((_tmp56C=_tag_dyneither(_tmp56B,sizeof(void*),_tmp56A),((((
_tmp56D=_tmp56A,_tmp56F(& r,& new_val,& _tmp56E,& _tmp56D,& _tmp56B,& _tmp130))),
_tmp56C)))))));struct Cyc_CfFlowInfo_Aggregate_struct _tmp575;struct Cyc_CfFlowInfo_UnionRInfo
_tmp574;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp573;return(void*)((_tmp573=
_region_malloc(r,sizeof(*_tmp573)),((_tmp573[0]=((_tmp575.tag=6,((_tmp575.f1=((
_tmp574.is_union=_tmp12F,((_tmp574.fieldnum=- 1,_tmp574)))),((_tmp575.f2=d2,
_tmp575)))))),_tmp573))));}_LL62:;_LL63: return new_val;_LL5F:;}static struct
_dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct
_dyneither_ptr d,int n,void*rval);static void _tmp57B(struct _dyneither_ptr*d,
unsigned int*_tmp57A,unsigned int*_tmp579,void***_tmp577){for(*_tmp57A=0;*
_tmp57A < *_tmp579;(*_tmp57A)++){(*_tmp577)[*_tmp57A]=((void**)(*d).curr)[(int)*
_tmp57A];}}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmp138=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmp138 == rval)return d;{
unsigned int _tmp57A;unsigned int _tmp579;struct _dyneither_ptr _tmp578;void**
_tmp577;unsigned int _tmp576;struct _dyneither_ptr res=(_tmp576=_get_dyneither_size(
d,sizeof(void*)),((_tmp577=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp576)),((_tmp578=_tag_dyneither(_tmp577,sizeof(void*),_tmp576),((((_tmp579=
_tmp576,_tmp57B(& d,& _tmp57A,& _tmp579,& _tmp577))),_tmp578)))))));*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;};}struct
_tuple17{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 
0)return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{struct _tuple17
_tmp57C;struct _tuple17 _tmp13E=(_tmp57C.f1=fs,((_tmp57C.f2=old_val,_tmp57C)));
struct Cyc_List_List*_tmp13F;struct Cyc_List_List _tmp140;int _tmp141;struct Cyc_List_List*
_tmp142;void*_tmp143;struct Cyc_CfFlowInfo_UnionRInfo _tmp145;int _tmp146;struct
_dyneither_ptr _tmp147;_LL65: _tmp13F=_tmp13E.f1;if(_tmp13F == 0)goto _LL67;_tmp140=*
_tmp13F;_tmp141=(int)_tmp140.hd;_tmp142=_tmp140.tl;_tmp143=_tmp13E.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp144=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp143;if(_tmp144->tag != 6)goto
_LL67;else{_tmp145=_tmp144->f1;_tmp146=_tmp145.is_union;_tmp147=_tmp144->f2;}};
_LL66: {void*_tmp148=Cyc_CfFlowInfo_insert_place_outer(r,_tmp142,*((void**)
_check_dyneither_subscript(_tmp147,sizeof(void*),_tmp141)),new_val);struct
_dyneither_ptr _tmp149=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp147,_tmp141,_tmp148);
if((void**)_tmp149.curr == (void**)_tmp147.curr)return old_val;else{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp582;struct Cyc_CfFlowInfo_UnionRInfo _tmp581;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp580;return(void*)((_tmp580=_region_malloc(r,sizeof(*_tmp580)),((_tmp580[0]=((
_tmp582.tag=6,((_tmp582.f1=((_tmp581.is_union=_tmp146,((_tmp581.fieldnum=- 1,
_tmp581)))),((_tmp582.f2=_tmp149,_tmp582)))))),_tmp580))));}}_LL67:;_LL68: {
struct Cyc_Core_Impossible_struct _tmp588;const char*_tmp587;struct Cyc_Core_Impossible_struct*
_tmp586;(int)_throw((void*)((_tmp586=_cycalloc(sizeof(*_tmp586)),((_tmp586[0]=((
_tmp588.tag=Cyc_Core_Impossible,((_tmp588.f1=((_tmp587="bad insert place",
_tag_dyneither(_tmp587,sizeof(char),17))),_tmp588)))),_tmp586)))));}_LL64:;};}
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict
d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*_tmp150=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)
_check_null(pile->places))->tl;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,_tmp150,0);{void*oldval;void*newval;{struct _handler_cons _tmp151;
_push_handler(& _tmp151);{int _tmp153=0;if(setjmp(_tmp151.handler))_tmp153=1;if(!
_tmp153){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp150);;_pop_handler();}else{
void*_tmp152=(void*)_exn_thrown;void*_tmp155=_tmp152;_LL6A: {struct Cyc_Dict_Absent_struct*
_tmp156=(struct Cyc_Dict_Absent_struct*)_tmp155;if(_tmp156->tag != Cyc_Dict_Absent)
goto _LL6C;}_LL6B: continue;_LL6C:;_LL6D:(void)_throw(_tmp155);_LL69:;}};}switch(
Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){case Cyc_CfFlowInfo_AllIL: _LL6E: newval=
fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL: _LL6F: newval=fenv->esc_this;break;
case Cyc_CfFlowInfo_NoneIL: _LL70: newval=fenv->esc_none;break;}Cyc_CfFlowInfo_add_places(
pile,oldval);{void*_tmp158;struct Cyc_List_List*_tmp159;struct Cyc_CfFlowInfo_Place
_tmp157=*_tmp150;_tmp158=_tmp157.root;_tmp159=_tmp157.fields;d=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp158,Cyc_CfFlowInfo_insert_place_outer(
fenv->r,_tmp159,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,
_tmp158),newval));};};}return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct
_RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};static enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmp15A=r;enum Cyc_CfFlowInfo_InitLevel
_tmp15C;enum Cyc_CfFlowInfo_InitLevel _tmp15E;void*_tmp163;_LL73: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp15B=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp15A;if(_tmp15B->tag != 3)goto
_LL75;else{_tmp15C=_tmp15B->f1;}}_LL74: return _tmp15C;_LL75: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp15D=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp15A;if(_tmp15D->tag != 4)goto _LL77;
else{_tmp15E=_tmp15D->f1;}}_LL76: return _tmp15E;_LL77: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp15F=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp15A;if(_tmp15F->tag != 0)goto _LL79;}
_LL78: goto _LL7A;_LL79: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp160=(struct
Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp15A;if(_tmp160->tag != 1)goto _LL7B;}_LL7A:
return Cyc_CfFlowInfo_AllIL;_LL7B: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp161=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp15A;if(_tmp161->tag != 2)
goto _LL7D;}_LL7C: return Cyc_CfFlowInfo_ThisIL;_LL7D: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp162=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp15A;if(_tmp162->tag != 7)goto
_LL7F;else{_tmp163=(void*)_tmp162->f3;}}_LL7E: return Cyc_CfFlowInfo_NoneIL;_LL7F:;
_LL80: {struct Cyc_Core_Impossible_struct _tmp58E;const char*_tmp58D;struct Cyc_Core_Impossible_struct*
_tmp58C;(int)_throw((void*)((_tmp58C=_cycalloc(sizeof(*_tmp58C)),((_tmp58C[0]=((
_tmp58E.tag=Cyc_Core_Impossible,((_tmp58E.f1=((_tmp58D="initlevel_approx",
_tag_dyneither(_tmp58D,sizeof(char),17))),_tmp58E)))),_tmp58C)))));}_LL72:;}
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){enum Cyc_CfFlowInfo_InitLevel
this_ans;if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;{void*
_tmp167=r;struct Cyc_CfFlowInfo_UnionRInfo _tmp169;int _tmp16A;int _tmp16B;struct
_dyneither_ptr _tmp16C;struct Cyc_CfFlowInfo_UnionRInfo _tmp16E;int _tmp16F;struct
_dyneither_ptr _tmp170;struct Cyc_CfFlowInfo_Place*_tmp172;_LL82: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp168=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp167;if(_tmp168->tag != 6)goto
_LL84;else{_tmp169=_tmp168->f1;_tmp16A=_tmp169.is_union;if(_tmp16A != 1)goto _LL84;
_tmp16B=_tmp169.fieldnum;_tmp16C=_tmp168->f2;}}_LL83: {unsigned int _tmp173=
_get_dyneither_size(_tmp16C,sizeof(void*));this_ans=Cyc_CfFlowInfo_NoneIL;if(
_tmp16B == - 1){int i=0;for(0;i < _tmp173;++ i){if(Cyc_CfFlowInfo_initlevel_rec(env,*((
void**)_check_dyneither_subscript(_tmp16C,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL){this_ans=Cyc_CfFlowInfo_AllIL;break;}}}else{if(Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp16C,sizeof(void*),_tmp16B)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL)this_ans=Cyc_CfFlowInfo_AllIL;}goto _LL81;}_LL84: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp16D=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp167;if(_tmp16D->tag != 6)goto _LL86;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16E.is_union;
if(_tmp16F != 0)goto _LL86;_tmp170=_tmp16D->f2;}}_LL85: {unsigned int _tmp174=
_get_dyneither_size(_tmp170,sizeof(void*));this_ans=Cyc_CfFlowInfo_AllIL;{int i=0;
for(0;i < _tmp174;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)
_check_dyneither_subscript(_tmp170,sizeof(void*),i)),this_ans);}}goto _LL81;}
_LL86: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp171=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp167;if(_tmp171->tag != 5)goto _LL88;else{_tmp172=_tmp171->f1;}}_LL87: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp172))this_ans=Cyc_CfFlowInfo_AllIL;else{{struct Cyc_List_List*_tmp58F;env->seen=((
_tmp58F=_region_malloc(env->e,sizeof(*_tmp58F)),((_tmp58F->hd=_tmp172,((_tmp58F->tl=
env->seen,_tmp58F))))));}this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(
env->d,_tmp172),Cyc_CfFlowInfo_AllIL);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == Cyc_CfFlowInfo_NoneIL)this_ans=Cyc_CfFlowInfo_ThisIL;}
goto _LL81;_LL88:;_LL89: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL81:;}if(
this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;if(this_ans == Cyc_CfFlowInfo_ThisIL
 || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;return Cyc_CfFlowInfo_AllIL;}
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*
env,struct Cyc_Dict_Dict d,void*r){struct _RegionHandle*_tmp176=env->r;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp590;struct Cyc_CfFlowInfo_InitlevelEnv _tmp177=(_tmp590.e=_tmp176,((_tmp590.d=
d,((_tmp590.seen=0,_tmp590)))));return Cyc_CfFlowInfo_initlevel_rec(& _tmp177,r,
Cyc_CfFlowInfo_AllIL);}struct _tuple18{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){void*_tmp17A;struct Cyc_List_List*
_tmp17B;struct Cyc_CfFlowInfo_Place _tmp179=*place;_tmp17A=_tmp179.root;_tmp17B=
_tmp179.fields;{void*_tmp17C=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,_tmp17A);for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){struct _tuple18 _tmp591;struct
_tuple18 _tmp17E=(_tmp591.f1=_tmp17C,((_tmp591.f2=(int)_tmp17B->hd,_tmp591)));
void*_tmp17F;struct Cyc_CfFlowInfo_UnionRInfo _tmp181;struct _dyneither_ptr _tmp182;
int _tmp183;_LL8B: _tmp17F=_tmp17E.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp180=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp17F;if(_tmp180->tag != 6)goto
_LL8D;else{_tmp181=_tmp180->f1;_tmp182=_tmp180->f2;}};_tmp183=_tmp17E.f2;_LL8C:
_tmp17C=*((void**)_check_dyneither_subscript(_tmp182,sizeof(void*),_tmp183));
goto _LL8A;_LL8D:;_LL8E: {struct Cyc_Core_Impossible_struct _tmp597;const char*
_tmp596;struct Cyc_Core_Impossible_struct*_tmp595;(int)_throw((void*)((_tmp595=
_cycalloc(sizeof(*_tmp595)),((_tmp595[0]=((_tmp597.tag=Cyc_Core_Impossible,((
_tmp597.f1=((_tmp596="bad lookup_place",_tag_dyneither(_tmp596,sizeof(char),17))),
_tmp597)))),_tmp595)))));}_LL8A:;}return _tmp17C;};}static int Cyc_CfFlowInfo_is_rval_unescaped(
void*rval){void*_tmp187=rval;void*_tmp18A;struct Cyc_CfFlowInfo_UnionRInfo _tmp18C;
int _tmp18D;int _tmp18E;struct _dyneither_ptr _tmp18F;_LL90: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp188=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp187;if(_tmp188->tag != 4)goto _LL92;}
_LL91: return 0;_LL92: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp189=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp187;if(_tmp189->tag != 7)goto _LL94;else{_tmp18A=(void*)_tmp189->f3;}}_LL93:
return Cyc_CfFlowInfo_is_rval_unescaped(rval);_LL94: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp18B=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp187;if(_tmp18B->tag != 6)goto
_LL96;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18C.is_union;_tmp18E=_tmp18C.fieldnum;
_tmp18F=_tmp18B->f2;}}_LL95: if(_tmp18D  && _tmp18E != - 1)return !Cyc_CfFlowInfo_is_rval_unescaped(*((
void**)_check_dyneither_subscript(_tmp18F,sizeof(void*),_tmp18E)));else{
unsigned int sz=_get_dyneither_size(_tmp18F,sizeof(void*));{int i=0;for(0;i < sz;++
i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp18F.curr)[i]))return 0;}}
return 1;}_LL96:;_LL97: return 1;_LL8F:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(
d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct
_RegionHandle*_tmp190=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp598;struct Cyc_CfFlowInfo_EscPile*
pile=(_tmp598=_region_malloc(_tmp190,sizeof(*_tmp598)),((_tmp598->rgn=_tmp190,((
_tmp598->places=0,_tmp598)))));Cyc_CfFlowInfo_add_places(pile,r);return Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*oldval,void*newval);static void _tmp5A2(struct Cyc_CfFlowInfo_AssignEnv**
env,unsigned int*_tmp5A1,unsigned int*_tmp5A0,void***_tmp59E,struct
_dyneither_ptr*_tmp1A2,struct _dyneither_ptr*_tmp19E){for(*_tmp5A1=0;*_tmp5A1 < *
_tmp5A0;(*_tmp5A1)++){(*_tmp59E)[*_tmp5A1]=Cyc_CfFlowInfo_assign_place_inner(*
env,((void**)(*_tmp19E).curr)[(int)*_tmp5A1],*((void**)
_check_dyneither_subscript(*_tmp1A2,sizeof(void*),(int)*_tmp5A1)));}}static void*
Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,
void*newval){struct _tuple0 _tmp599;struct _tuple0 _tmp193=(_tmp599.f1=oldval,((
_tmp599.f2=newval,_tmp599)));void*_tmp194;void*_tmp196;struct Cyc_CfFlowInfo_Place*
_tmp198;void*_tmp199;void*_tmp19B;struct Cyc_CfFlowInfo_UnionRInfo _tmp19D;struct
_dyneither_ptr _tmp19E;void*_tmp19F;struct Cyc_CfFlowInfo_UnionRInfo _tmp1A1;struct
_dyneither_ptr _tmp1A2;void*_tmp1A3;enum Cyc_CfFlowInfo_InitLevel _tmp1A5;_LL99:
_tmp194=_tmp193.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp195=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp194;if(_tmp195->tag != 4)goto _LL9B;};_tmp196=_tmp193.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp197=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp196;if(_tmp197->tag != 5)goto
_LL9B;else{_tmp198=_tmp197->f1;}};_LL9A: Cyc_CfFlowInfo_add_place(env->pile,
_tmp198);goto _LL9C;_LL9B: _tmp199=_tmp193.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp19A=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp199;if(_tmp19A->tag != 4)goto _LL9D;};
_LL9C: if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp59C;void*_tmp59B;(_tmp59B=0,Cyc_Tcutil_terr(env->loc,((_tmp59C="assignment puts possibly-uninitialized data in an escaped location",
_tag_dyneither(_tmp59C,sizeof(char),67))),_tag_dyneither(_tmp59B,sizeof(void*),0)));}
return(env->fenv)->esc_all;_LL9D: _tmp19B=_tmp193.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp19C=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp19B;if(_tmp19C->tag != 6)goto
_LL9F;else{_tmp19D=_tmp19C->f1;_tmp19E=_tmp19C->f2;}};_tmp19F=_tmp193.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp1A0=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp19F;if(_tmp1A0->tag != 6)goto _LL9F;else{_tmp1A1=_tmp1A0->f1;_tmp1A2=_tmp1A0->f2;}};
_LL9E: {unsigned int _tmp5A1;unsigned int _tmp5A0;struct _dyneither_ptr _tmp59F;void**
_tmp59E;unsigned int _tmp59D;struct _dyneither_ptr new_d=(_tmp59D=
_get_dyneither_size(_tmp19E,sizeof(void*)),((_tmp59E=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp59D)),((_tmp59F=_tag_dyneither(_tmp59E,sizeof(
void*),_tmp59D),((((_tmp5A0=_tmp59D,_tmp5A2(& env,& _tmp5A1,& _tmp5A0,& _tmp59E,&
_tmp1A2,& _tmp19E))),_tmp59F)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp19E,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp19E.curr)[i]){
change=1;break;}}}if(!change){if(!_tmp19D.is_union)return oldval;new_d=_tmp19E;}
else{change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp19E,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1A2,sizeof(void*),i))){change=1;break;}}}if(!
change){if(!_tmp19D.is_union)return newval;new_d=_tmp1A2;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp5A5;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5A4;return(void*)((_tmp5A4=
_region_malloc((env->fenv)->r,sizeof(*_tmp5A4)),((_tmp5A4[0]=((_tmp5A5.tag=6,((
_tmp5A5.f1=_tmp1A1,((_tmp5A5.f2=new_d,_tmp5A5)))))),_tmp5A4))));};}_LL9F: _tmp1A3=
_tmp193.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp1A4=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp1A3;if(_tmp1A4->tag != 4)goto _LLA1;else{_tmp1A5=_tmp1A4->f1;}};_LLA0: switch(
_tmp1A5){case Cyc_CfFlowInfo_NoneIL: _LLA3: return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL:
_LLA4: return(env->fenv)->unknown_this;case Cyc_CfFlowInfo_AllIL: _LLA5: return(env->fenv)->unknown_all;}
_LLA1:;_LLA2: return newval;_LL98:;}static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,void*oldval,void*
newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple17 _tmp5A6;struct _tuple17 _tmp1AF=(_tmp5A6.f1=fs,((_tmp5A6.f2=oldval,
_tmp5A6)));struct Cyc_List_List*_tmp1B0;struct Cyc_List_List _tmp1B1;int _tmp1B2;
struct Cyc_List_List*_tmp1B3;void*_tmp1B4;struct Cyc_CfFlowInfo_UnionRInfo _tmp1B6;
int _tmp1B7;int _tmp1B8;struct _dyneither_ptr _tmp1B9;_LLA8: _tmp1B0=_tmp1AF.f1;if(
_tmp1B0 == 0)goto _LLAA;_tmp1B1=*_tmp1B0;_tmp1B2=(int)_tmp1B1.hd;_tmp1B3=_tmp1B1.tl;
_tmp1B4=_tmp1AF.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1B5=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1B4;if(_tmp1B5->tag != 6)goto _LLAA;else{_tmp1B6=_tmp1B5->f1;_tmp1B7=_tmp1B6.is_union;
_tmp1B8=_tmp1B6.fieldnum;_tmp1B9=_tmp1B5->f2;}};_LLA9: {void*_tmp1BA=Cyc_CfFlowInfo_assign_place_outer(
env,_tmp1B3,*((void**)_check_dyneither_subscript(_tmp1B9,sizeof(void*),_tmp1B2)),
newval);struct _dyneither_ptr _tmp1BB=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,
_tmp1B9,_tmp1B2,_tmp1BA);if((void**)_tmp1BB.curr == (void**)_tmp1B9.curr  && (!
_tmp1B7  || _tmp1B8 == _tmp1B2))return oldval;{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp5AC;struct Cyc_CfFlowInfo_UnionRInfo _tmp5AB;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5AA;return(void*)((_tmp5AA=_region_malloc((env->fenv)->r,sizeof(*_tmp5AA)),((
_tmp5AA[0]=((_tmp5AC.tag=6,((_tmp5AC.f1=((_tmp5AB.is_union=_tmp1B7,((_tmp5AB.fieldnum=
_tmp1B2,_tmp5AB)))),((_tmp5AC.f2=_tmp1BB,_tmp5AC)))))),_tmp5AA))));};}_LLAA:;
_LLAB: {struct Cyc_Core_Impossible_struct _tmp5B2;const char*_tmp5B1;struct Cyc_Core_Impossible_struct*
_tmp5B0;(int)_throw((void*)((_tmp5B0=_cycalloc(sizeof(*_tmp5B0)),((_tmp5B0[0]=((
_tmp5B2.tag=Cyc_Core_Impossible,((_tmp5B2.f1=((_tmp5B1="bad assign place",
_tag_dyneither(_tmp5B1,sizeof(char),17))),_tmp5B2)))),_tmp5B0)))));}_LLA7:;};}
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,place,0);{struct _RegionHandle*_tmp1C2=fenv->r;struct Cyc_CfFlowInfo_Place
_tmp1C4;void*_tmp1C5;struct Cyc_List_List*_tmp1C6;struct Cyc_CfFlowInfo_Place*
_tmp1C3=place;_tmp1C4=*_tmp1C3;_tmp1C5=_tmp1C4.root;_tmp1C6=_tmp1C4.fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp5B5;struct Cyc_CfFlowInfo_AssignEnv _tmp5B4;
struct Cyc_CfFlowInfo_AssignEnv env=(_tmp5B4.fenv=fenv,((_tmp5B4.pile=((_tmp5B5=
_region_malloc(_tmp1C2,sizeof(*_tmp5B5)),((_tmp5B5->rgn=_tmp1C2,((_tmp5B5->places=
0,_tmp5B5)))))),((_tmp5B4.d=d,((_tmp5B4.loc=loc,_tmp5B4)))))));void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp1C6,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1C5),
r);return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1C5,newval));};};}
struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict
chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);struct _tuple19{enum Cyc_CfFlowInfo_InitLevel f1;
enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct
Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp5C5(struct Cyc_CfFlowInfo_JoinEnv**
env,unsigned int*_tmp5C4,unsigned int*_tmp5C3,void***_tmp5C1,struct
_dyneither_ptr*_tmp213,struct _dyneither_ptr*_tmp20D){for(*_tmp5C4=0;*_tmp5C4 < *
_tmp5C3;(*_tmp5C4)++){(*_tmp5C1)[*_tmp5C4]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp20D).curr)[(
int)*_tmp5C4],*((void**)_check_dyneither_subscript(*_tmp213,sizeof(void*),(int)*
_tmp5C4)));}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp1C9=(
env->fenv)->r;{struct _tuple0 _tmp5B6;struct _tuple0 _tmp1CB=(_tmp5B6.f1=r1,((
_tmp5B6.f2=r2,_tmp5B6)));void*_tmp1CC;struct Cyc_Absyn_Exp*_tmp1CE;int _tmp1CF;
void*_tmp1D0;void*_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D3;int _tmp1D4;void*_tmp1D5;
void*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D8;int _tmp1D9;void*_tmp1DA;void*_tmp1DB;
struct Cyc_Absyn_Exp*_tmp1DD;int _tmp1DE;void*_tmp1DF;void*_tmp1E0;struct Cyc_CfFlowInfo_Place*
_tmp1E2;void*_tmp1E3;struct Cyc_CfFlowInfo_Place*_tmp1E5;void*_tmp1E6;struct Cyc_CfFlowInfo_Place*
_tmp1E8;void*_tmp1E9;void*_tmp1EB;void*_tmp1ED;struct Cyc_CfFlowInfo_Place*
_tmp1EF;void*_tmp1F0;struct Cyc_CfFlowInfo_Place*_tmp1F2;void*_tmp1F3;void*
_tmp1F5;void*_tmp1F7;struct Cyc_CfFlowInfo_Place*_tmp1F9;void*_tmp1FA;struct Cyc_CfFlowInfo_Place*
_tmp1FC;void*_tmp1FD;struct Cyc_CfFlowInfo_Place*_tmp1FF;void*_tmp200;void*
_tmp202;void*_tmp204;void*_tmp206;void*_tmp208;struct Cyc_CfFlowInfo_UnionRInfo
_tmp20A;int _tmp20B;int _tmp20C;struct _dyneither_ptr _tmp20D;void*_tmp20E;struct Cyc_CfFlowInfo_UnionRInfo
_tmp210;int _tmp211;int _tmp212;struct _dyneither_ptr _tmp213;_LLAD: _tmp1CC=_tmp1CB.f1;{
struct Cyc_CfFlowInfo_Consumed_struct*_tmp1CD=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1CC;if(_tmp1CD->tag != 7)goto _LLAF;else{_tmp1CE=_tmp1CD->f1;_tmp1CF=_tmp1CD->f2;
_tmp1D0=(void*)_tmp1CD->f3;}};_tmp1D1=_tmp1CB.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1D2=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1D1;if(_tmp1D2->tag != 7)goto
_LLAF;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D2->f2;_tmp1D5=(void*)_tmp1D2->f3;}};
_LLAE: if(_tmp1CE == _tmp1D3  && _tmp1CF == _tmp1D4){struct Cyc_CfFlowInfo_Consumed_struct
_tmp5B9;struct Cyc_CfFlowInfo_Consumed_struct*_tmp5B8;return(void*)((_tmp5B8=
_region_malloc(_tmp1C9,sizeof(*_tmp5B8)),((_tmp5B8[0]=((_tmp5B9.tag=7,((_tmp5B9.f1=
_tmp1CE,((_tmp5B9.f2=_tmp1CF,((_tmp5B9.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1D0,_tmp1D5),_tmp5B9)))))))),_tmp5B8))));}{void*_tmp216=_tmp1D0;struct Cyc_CfFlowInfo_Place*
_tmp218;_LLCA: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp217=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp216;if(_tmp217->tag != 5)goto _LLCC;else{_tmp218=_tmp217->f1;}}_LLCB: Cyc_CfFlowInfo_add_place(
env->pile,_tmp218);goto _LLC9;_LLCC:;_LLCD: goto _LLC9;_LLC9:;}{void*_tmp219=
_tmp1D5;struct Cyc_CfFlowInfo_Place*_tmp21B;_LLCF: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp21A=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp219;if(_tmp21A->tag != 5)goto
_LLD1;else{_tmp21B=_tmp21A->f1;}}_LLD0: Cyc_CfFlowInfo_add_place(env->pile,
_tmp21B);goto _LLCE;_LLD1:;_LLD2: goto _LLCE;_LLCE:;}goto _LLAC;_LLAF: _tmp1D6=
_tmp1CB.f1;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp1D7=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1D6;if(_tmp1D7->tag != 7)goto _LLB1;else{_tmp1D8=_tmp1D7->f1;_tmp1D9=_tmp1D7->f2;
_tmp1DA=(void*)_tmp1D7->f3;}};_LLB0: {struct Cyc_CfFlowInfo_Consumed_struct
_tmp5BC;struct Cyc_CfFlowInfo_Consumed_struct*_tmp5BB;return(void*)((_tmp5BB=
_region_malloc(_tmp1C9,sizeof(*_tmp5BB)),((_tmp5BB[0]=((_tmp5BC.tag=7,((_tmp5BC.f1=
_tmp1D8,((_tmp5BC.f2=_tmp1D9,((_tmp5BC.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1DA,r2),_tmp5BC)))))))),_tmp5BB))));}_LLB1: _tmp1DB=_tmp1CB.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1DC=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1DB;if(_tmp1DC->tag != 7)goto
_LLB3;else{_tmp1DD=_tmp1DC->f1;_tmp1DE=_tmp1DC->f2;_tmp1DF=(void*)_tmp1DC->f3;}};
_LLB2: {struct Cyc_CfFlowInfo_Consumed_struct _tmp5BF;struct Cyc_CfFlowInfo_Consumed_struct*
_tmp5BE;return(void*)((_tmp5BE=_region_malloc(_tmp1C9,sizeof(*_tmp5BE)),((
_tmp5BE[0]=((_tmp5BF.tag=7,((_tmp5BF.f1=_tmp1DD,((_tmp5BF.f2=_tmp1DE,((_tmp5BF.f3=(
void*)Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1DF),_tmp5BF)))))))),_tmp5BE))));}
_LLB3: _tmp1E0=_tmp1CB.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1E1=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1E0;if(_tmp1E1->tag != 5)goto _LLB5;else{
_tmp1E2=_tmp1E1->f1;}};_tmp1E3=_tmp1CB.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1E4=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1E3;if(_tmp1E4->tag != 5)goto
_LLB5;else{_tmp1E5=_tmp1E4->f1;}};_LLB4: if(Cyc_CfFlowInfo_place_cmp(_tmp1E2,
_tmp1E5)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp1E2);Cyc_CfFlowInfo_add_place(
env->pile,_tmp1E5);goto _LLAC;_LLB5: _tmp1E6=_tmp1CB.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1E7=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1E6;if(_tmp1E7->tag != 5)goto
_LLB7;else{_tmp1E8=_tmp1E7->f1;}};_tmp1E9=_tmp1CB.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1EA=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1E9;if(_tmp1EA->tag != 2)
goto _LLB7;};_LLB6: _tmp1EF=_tmp1E8;goto _LLB8;_LLB7: _tmp1EB=_tmp1CB.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1EC=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1EB;if(_tmp1EC->tag != 2)
goto _LLB9;};_tmp1ED=_tmp1CB.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1EE=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1ED;if(_tmp1EE->tag != 5)goto _LLB9;
else{_tmp1EF=_tmp1EE->f1;}};_LLB8: Cyc_CfFlowInfo_add_place(env->pile,_tmp1EF);
return(env->fenv)->notzerothis;_LLB9: _tmp1F0=_tmp1CB.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1F1=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F0;if(_tmp1F1->tag != 5)goto
_LLBB;else{_tmp1F2=_tmp1F1->f1;}};_tmp1F3=_tmp1CB.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp1F4=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1F3;if(_tmp1F4->tag != 1)
goto _LLBB;};_LLBA: Cyc_CfFlowInfo_add_place(env->pile,_tmp1F2);switch(Cyc_CfFlowInfo_initlevel(
env->fenv,env->d1,r1)){case Cyc_CfFlowInfo_AllIL: _LLD3: return(env->fenv)->notzeroall;
default: _LLD4: return(env->fenv)->notzerothis;}_LLBB: _tmp1F5=_tmp1CB.f1;{struct
Cyc_CfFlowInfo_NotZeroAll_struct*_tmp1F6=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp1F5;if(_tmp1F6->tag != 1)goto _LLBD;};_tmp1F7=_tmp1CB.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1F8=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F7;if(_tmp1F8->tag != 5)goto
_LLBD;else{_tmp1F9=_tmp1F8->f1;}};_LLBC: Cyc_CfFlowInfo_add_place(env->pile,
_tmp1F9);switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2)){case Cyc_CfFlowInfo_AllIL:
_LLD6: return(env->fenv)->notzeroall;default: _LLD7: return(env->fenv)->notzerothis;}
_LLBD: _tmp1FA=_tmp1CB.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1FB=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1FA;if(_tmp1FB->tag != 5)goto _LLBF;else{
_tmp1FC=_tmp1FB->f1;}};_LLBE: Cyc_CfFlowInfo_add_place(env->pile,_tmp1FC);goto
_LLAC;_LLBF: _tmp1FD=_tmp1CB.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1FE=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1FD;if(_tmp1FE->tag != 5)goto _LLC1;
else{_tmp1FF=_tmp1FE->f1;}};_LLC0: Cyc_CfFlowInfo_add_place(env->pile,_tmp1FF);
goto _LLAC;_LLC1: _tmp200=_tmp1CB.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp201=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp200;if(_tmp201->tag != 1)
goto _LLC3;};_tmp202=_tmp1CB.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp203=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp202;if(_tmp203->tag != 2)goto _LLC3;};
_LLC2: goto _LLC4;_LLC3: _tmp204=_tmp1CB.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp205=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp204;if(_tmp205->tag != 2)
goto _LLC5;};_tmp206=_tmp1CB.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp207=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp206;if(_tmp207->tag != 1)goto _LLC5;};
_LLC4: return(env->fenv)->notzerothis;_LLC5: _tmp208=_tmp1CB.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp209=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp208;if(_tmp209->tag != 6)goto
_LLC7;else{_tmp20A=_tmp209->f1;_tmp20B=_tmp20A.is_union;_tmp20C=_tmp20A.fieldnum;
_tmp20D=_tmp209->f2;}};_tmp20E=_tmp1CB.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp20F=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp20E;if(_tmp20F->tag != 6)goto
_LLC7;else{_tmp210=_tmp20F->f1;_tmp211=_tmp210.is_union;_tmp212=_tmp210.fieldnum;
_tmp213=_tmp20F->f2;}};_LLC6: {unsigned int _tmp5C4;unsigned int _tmp5C3;struct
_dyneither_ptr _tmp5C2;void**_tmp5C1;unsigned int _tmp5C0;struct _dyneither_ptr
new_d=(_tmp5C0=_get_dyneither_size(_tmp20D,sizeof(void*)),((_tmp5C1=(void**)
_region_malloc(_tmp1C9,_check_times(sizeof(void*),_tmp5C0)),((_tmp5C2=
_tag_dyneither(_tmp5C1,sizeof(void*),_tmp5C0),((((_tmp5C3=_tmp5C0,_tmp5C5(& env,&
_tmp5C4,& _tmp5C3,& _tmp5C1,& _tmp213,& _tmp20D))),_tmp5C2)))))));int change=0;{int i=
0;for(0;i < _get_dyneither_size(_tmp20D,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp20D.curr)[i]){
change=1;break;}}}if(!change){if(!_tmp20B)return r1;new_d=_tmp20D;}else{change=0;{
int i=0;for(0;i < _get_dyneither_size(_tmp20D,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp213,sizeof(void*),i))){change=1;break;}}}if(!
change){if(!_tmp20B)return r2;new_d=_tmp213;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp5CB;struct Cyc_CfFlowInfo_UnionRInfo _tmp5CA;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5C9;return(void*)((_tmp5C9=_region_malloc(_tmp1C9,sizeof(*_tmp5C9)),((
_tmp5C9[0]=((_tmp5CB.tag=6,((_tmp5CB.f1=((_tmp5CA.is_union=_tmp20B,((_tmp5CA.fieldnum=
_tmp20C == _tmp212?_tmp20C: - 1,_tmp5CA)))),((_tmp5CB.f2=new_d,_tmp5CB)))))),
_tmp5C9))));};}_LLC7:;_LLC8: goto _LLAC;_LLAC:;}{enum Cyc_CfFlowInfo_InitLevel il1=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel il2=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);struct _tuple0 _tmp5CC;struct
_tuple0 _tmp228=(_tmp5CC.f1=r1,((_tmp5CC.f2=r2,_tmp5CC)));void*_tmp229;void*
_tmp22B;_LLDA: _tmp229=_tmp228.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp22A=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp229;if(_tmp22A->tag != 4)goto _LLDC;};_LLDB:
goto _LLDD;_LLDC: _tmp22B=_tmp228.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp22C=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp22B;if(_tmp22C->tag != 4)goto _LLDE;};_LLDD: {
struct _tuple19 _tmp5CD;struct _tuple19 _tmp22E=(_tmp5CD.f1=il1,((_tmp5CD.f2=il2,
_tmp5CD)));enum Cyc_CfFlowInfo_InitLevel _tmp22F;enum Cyc_CfFlowInfo_InitLevel
_tmp230;enum Cyc_CfFlowInfo_InitLevel _tmp231;enum Cyc_CfFlowInfo_InitLevel
_tmp232;_LLE1: _tmp22F=_tmp22E.f2;if(_tmp22F != Cyc_CfFlowInfo_NoneIL)goto _LLE3;
_LLE2: goto _LLE4;_LLE3: _tmp230=_tmp22E.f1;if(_tmp230 != Cyc_CfFlowInfo_NoneIL)goto
_LLE5;_LLE4: return(env->fenv)->esc_none;_LLE5: _tmp231=_tmp22E.f2;if(_tmp231 != 
Cyc_CfFlowInfo_ThisIL)goto _LLE7;_LLE6: goto _LLE8;_LLE7: _tmp232=_tmp22E.f1;if(
_tmp232 != Cyc_CfFlowInfo_ThisIL)goto _LLE9;_LLE8: return(env->fenv)->esc_this;
_LLE9:;_LLEA: return(env->fenv)->esc_all;_LLE0:;}_LLDE:;_LLDF: {struct _tuple19
_tmp5CE;struct _tuple19 _tmp234=(_tmp5CE.f1=il1,((_tmp5CE.f2=il2,_tmp5CE)));enum 
Cyc_CfFlowInfo_InitLevel _tmp235;enum Cyc_CfFlowInfo_InitLevel _tmp236;enum Cyc_CfFlowInfo_InitLevel
_tmp237;enum Cyc_CfFlowInfo_InitLevel _tmp238;_LLEC: _tmp235=_tmp234.f2;if(_tmp235
!= Cyc_CfFlowInfo_NoneIL)goto _LLEE;_LLED: goto _LLEF;_LLEE: _tmp236=_tmp234.f1;if(
_tmp236 != Cyc_CfFlowInfo_NoneIL)goto _LLF0;_LLEF: return(env->fenv)->unknown_none;
_LLF0: _tmp237=_tmp234.f2;if(_tmp237 != Cyc_CfFlowInfo_ThisIL)goto _LLF2;_LLF1: goto
_LLF3;_LLF2: _tmp238=_tmp234.f1;if(_tmp238 != Cyc_CfFlowInfo_ThisIL)goto _LLF4;
_LLF3: return(env->fenv)->unknown_this;_LLF4:;_LLF5: return(env->fenv)->unknown_all;
_LLEB:;}_LLD9:;};};}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5CF;return((_tmp5CF.EqualConst).val=
i,(((_tmp5CF.EqualConst).tag=1,_tmp5CF)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(
struct Cyc_Absyn_Vardecl*v,void*t){struct _tuple10 _tmp5D2;union Cyc_CfFlowInfo_RelnOp
_tmp5D1;return((_tmp5D1.LessVar).val=((_tmp5D2.f1=v,((_tmp5D2.f2=t,_tmp5D2)))),(((
_tmp5D1.LessVar).tag=2,_tmp5D1)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(
struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp5D3;return((_tmp5D3.LessNumelts).val=
v,(((_tmp5D3.LessNumelts).tag=3,_tmp5D3)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5D4;return((_tmp5D4.LessConst).val=
i,(((_tmp5D4.LessConst).tag=4,_tmp5D4)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5D5;return((_tmp5D5.LessEqConst).val=
i,(((_tmp5D5.LessEqConst).tag=6,_tmp5D5)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(
struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp5D6;return((_tmp5D6.LessEqNumelts).val=
v,(((_tmp5D6.LessEqNumelts).tag=5,_tmp5D6)));}struct _tuple20{union Cyc_CfFlowInfo_RelnOp
f1;union Cyc_CfFlowInfo_RelnOp f2;};static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2){struct _tuple20 _tmp5D7;struct _tuple20 _tmp241=(
_tmp5D7.f1=r1,((_tmp5D7.f2=r2,_tmp5D7)));union Cyc_CfFlowInfo_RelnOp _tmp242;
unsigned int _tmp243;union Cyc_CfFlowInfo_RelnOp _tmp244;unsigned int _tmp245;union
Cyc_CfFlowInfo_RelnOp _tmp246;struct _tuple10 _tmp247;struct Cyc_Absyn_Vardecl*
_tmp248;union Cyc_CfFlowInfo_RelnOp _tmp249;struct _tuple10 _tmp24A;struct Cyc_Absyn_Vardecl*
_tmp24B;union Cyc_CfFlowInfo_RelnOp _tmp24C;struct Cyc_Absyn_Vardecl*_tmp24D;union
Cyc_CfFlowInfo_RelnOp _tmp24E;struct Cyc_Absyn_Vardecl*_tmp24F;union Cyc_CfFlowInfo_RelnOp
_tmp250;unsigned int _tmp251;union Cyc_CfFlowInfo_RelnOp _tmp252;unsigned int
_tmp253;union Cyc_CfFlowInfo_RelnOp _tmp254;unsigned int _tmp255;union Cyc_CfFlowInfo_RelnOp
_tmp256;unsigned int _tmp257;union Cyc_CfFlowInfo_RelnOp _tmp258;struct Cyc_Absyn_Vardecl*
_tmp259;union Cyc_CfFlowInfo_RelnOp _tmp25A;struct Cyc_Absyn_Vardecl*_tmp25B;_LLF7:
_tmp242=_tmp241.f1;if((_tmp242.EqualConst).tag != 1)goto _LLF9;_tmp243=(
unsigned int)(_tmp242.EqualConst).val;_tmp244=_tmp241.f2;if((_tmp244.EqualConst).tag
!= 1)goto _LLF9;_tmp245=(unsigned int)(_tmp244.EqualConst).val;_LLF8: return
_tmp243 == _tmp245;_LLF9: _tmp246=_tmp241.f1;if((_tmp246.LessVar).tag != 2)goto
_LLFB;_tmp247=(struct _tuple10)(_tmp246.LessVar).val;_tmp248=_tmp247.f1;_tmp249=
_tmp241.f2;if((_tmp249.LessVar).tag != 2)goto _LLFB;_tmp24A=(struct _tuple10)(
_tmp249.LessVar).val;_tmp24B=_tmp24A.f1;_LLFA: return _tmp248 == _tmp24B;_LLFB:
_tmp24C=_tmp241.f1;if((_tmp24C.LessNumelts).tag != 3)goto _LLFD;_tmp24D=(struct Cyc_Absyn_Vardecl*)(
_tmp24C.LessNumelts).val;_tmp24E=_tmp241.f2;if((_tmp24E.LessNumelts).tag != 3)
goto _LLFD;_tmp24F=(struct Cyc_Absyn_Vardecl*)(_tmp24E.LessNumelts).val;_LLFC:
return _tmp24D == _tmp24F;_LLFD: _tmp250=_tmp241.f1;if((_tmp250.LessConst).tag != 4)
goto _LLFF;_tmp251=(unsigned int)(_tmp250.LessConst).val;_tmp252=_tmp241.f2;if((
_tmp252.LessConst).tag != 4)goto _LLFF;_tmp253=(unsigned int)(_tmp252.LessConst).val;
_LLFE: return _tmp251 == _tmp253;_LLFF: _tmp254=_tmp241.f1;if((_tmp254.LessEqConst).tag
!= 6)goto _LL101;_tmp255=(unsigned int)(_tmp254.LessEqConst).val;_tmp256=_tmp241.f2;
if((_tmp256.LessEqConst).tag != 6)goto _LL101;_tmp257=(unsigned int)(_tmp256.LessEqConst).val;
_LL100: return _tmp255 == _tmp257;_LL101: _tmp258=_tmp241.f1;if((_tmp258.LessEqNumelts).tag
!= 5)goto _LL103;_tmp259=(struct Cyc_Absyn_Vardecl*)(_tmp258.LessEqNumelts).val;
_tmp25A=_tmp241.f2;if((_tmp25A.LessEqNumelts).tag != 5)goto _LL103;_tmp25B=(struct
Cyc_Absyn_Vardecl*)(_tmp25A.LessEqNumelts).val;_LL102: return _tmp259 == _tmp25B;
_LL103:;_LL104: return 0;_LLF6:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == 
r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*_tmp25C=
r1s;for(0;_tmp25C != 0;_tmp25C=_tmp25C->tl){struct Cyc_CfFlowInfo_Reln*_tmp25D=(
struct Cyc_CfFlowInfo_Reln*)_tmp25C->hd;int found=0;{struct Cyc_List_List*_tmp25E=
r2s;for(0;_tmp25E != 0;_tmp25E=_tmp25E->tl){struct Cyc_CfFlowInfo_Reln*_tmp25F=(
struct Cyc_CfFlowInfo_Reln*)_tmp25E->hd;if(_tmp25D == _tmp25F  || _tmp25D->vd == 
_tmp25F->vd  && Cyc_CfFlowInfo_same_relop(_tmp25D->rop,_tmp25F->rop)){{struct Cyc_List_List*
_tmp5D8;res=((_tmp5D8=_region_malloc(r,sizeof(*_tmp5D8)),((_tmp5D8->hd=_tmp25D,((
_tmp5D8->tl=res,_tmp5D8))))));}found=1;break;}}}if(!found)diff=1;}}if(!diff)res=
r1s;return res;};}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2){struct _tuple21 _tmp5D9;struct _tuple21 _tmp262=(_tmp5D9.f1=f1,((_tmp5D9.f2=f2,
_tmp5D9)));union Cyc_CfFlowInfo_FlowInfo _tmp263;int _tmp264;union Cyc_CfFlowInfo_FlowInfo
_tmp265;int _tmp266;union Cyc_CfFlowInfo_FlowInfo _tmp267;struct _tuple11 _tmp268;
struct Cyc_Dict_Dict _tmp269;struct Cyc_List_List*_tmp26A;union Cyc_CfFlowInfo_FlowInfo
_tmp26B;struct _tuple11 _tmp26C;struct Cyc_Dict_Dict _tmp26D;struct Cyc_List_List*
_tmp26E;_LL106: _tmp263=_tmp262.f1;if((_tmp263.BottomFL).tag != 1)goto _LL108;
_tmp264=(int)(_tmp263.BottomFL).val;_LL107: return f2;_LL108: _tmp265=_tmp262.f2;
if((_tmp265.BottomFL).tag != 1)goto _LL10A;_tmp266=(int)(_tmp265.BottomFL).val;
_LL109: return f1;_LL10A: _tmp267=_tmp262.f1;if((_tmp267.ReachableFL).tag != 2)goto
_LL105;_tmp268=(struct _tuple11)(_tmp267.ReachableFL).val;_tmp269=_tmp268.f1;
_tmp26A=_tmp268.f2;_tmp26B=_tmp262.f2;if((_tmp26B.ReachableFL).tag != 2)goto
_LL105;_tmp26C=(struct _tuple11)(_tmp26B.ReachableFL).val;_tmp26D=_tmp26C.f1;
_tmp26E=_tmp26C.f2;_LL10B: if(_tmp269.t == _tmp26D.t  && _tmp26A == _tmp26E)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f2,f1))return f1;{struct _RegionHandle*_tmp26F=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp5DC;struct Cyc_CfFlowInfo_JoinEnv _tmp5DB;struct Cyc_CfFlowInfo_JoinEnv _tmp270=(
_tmp5DB.fenv=fenv,((_tmp5DB.pile=((_tmp5DC=_region_malloc(_tmp26F,sizeof(*
_tmp5DC)),((_tmp5DC->rgn=_tmp26F,((_tmp5DC->places=0,_tmp5DC)))))),((_tmp5DB.d1=
_tmp269,((_tmp5DB.d2=_tmp26D,_tmp5DB)))))));struct Cyc_Dict_Dict _tmp271=((struct
Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),
struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))
Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,
void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp270,_tmp269,_tmp26D);struct Cyc_List_List*
_tmp272=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp26A,_tmp26E);return Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_escape_these(fenv,_tmp270.pile,all_changed,_tmp271),_tmp272);};
_LL105:;}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;union Cyc_CfFlowInfo_FlowInfo f3;};struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple12
pr1,struct _tuple12 pr2){union Cyc_CfFlowInfo_FlowInfo _tmp276;void*_tmp277;struct
_tuple12 _tmp275=pr1;_tmp276=_tmp275.f1;_tmp277=_tmp275.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp279;void*_tmp27A;struct _tuple12 _tmp278=pr2;_tmp279=_tmp278.f1;_tmp27A=
_tmp278.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,
all_changed,_tmp276,_tmp279);struct _tuple22 _tmp5DD;struct _tuple22 _tmp27C=(
_tmp5DD.f1=_tmp276,((_tmp5DD.f2=_tmp279,((_tmp5DD.f3=outflow,_tmp5DD)))));union
Cyc_CfFlowInfo_FlowInfo _tmp27D;int _tmp27E;union Cyc_CfFlowInfo_FlowInfo _tmp27F;
int _tmp280;union Cyc_CfFlowInfo_FlowInfo _tmp281;struct _tuple11 _tmp282;struct Cyc_Dict_Dict
_tmp283;union Cyc_CfFlowInfo_FlowInfo _tmp284;struct _tuple11 _tmp285;struct Cyc_Dict_Dict
_tmp286;union Cyc_CfFlowInfo_FlowInfo _tmp287;struct _tuple11 _tmp288;struct Cyc_Dict_Dict
_tmp289;struct Cyc_List_List*_tmp28A;_LL10D: _tmp27D=_tmp27C.f1;if((_tmp27D.BottomFL).tag
!= 1)goto _LL10F;_tmp27E=(int)(_tmp27D.BottomFL).val;_LL10E: {struct _tuple12
_tmp5DE;return(_tmp5DE.f1=outflow,((_tmp5DE.f2=_tmp27A,_tmp5DE)));}_LL10F:
_tmp27F=_tmp27C.f2;if((_tmp27F.BottomFL).tag != 1)goto _LL111;_tmp280=(int)(
_tmp27F.BottomFL).val;_LL110: {struct _tuple12 _tmp5DF;return(_tmp5DF.f1=outflow,((
_tmp5DF.f2=_tmp277,_tmp5DF)));}_LL111: _tmp281=_tmp27C.f1;if((_tmp281.ReachableFL).tag
!= 2)goto _LL113;_tmp282=(struct _tuple11)(_tmp281.ReachableFL).val;_tmp283=
_tmp282.f1;_tmp284=_tmp27C.f2;if((_tmp284.ReachableFL).tag != 2)goto _LL113;
_tmp285=(struct _tuple11)(_tmp284.ReachableFL).val;_tmp286=_tmp285.f1;_tmp287=
_tmp27C.f3;if((_tmp287.ReachableFL).tag != 2)goto _LL113;_tmp288=(struct _tuple11)(
_tmp287.ReachableFL).val;_tmp289=_tmp288.f1;_tmp28A=_tmp288.f2;_LL112: if(((int(*)(
int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp277,
_tmp27A)){struct _tuple12 _tmp5E0;return(_tmp5E0.f1=outflow,((_tmp5E0.f2=_tmp27A,
_tmp5E0)));}if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp27A,_tmp277)){struct _tuple12 _tmp5E1;return(_tmp5E1.f1=outflow,((_tmp5E1.f2=
_tmp277,_tmp5E1)));}{struct _RegionHandle*_tmp28F=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp5E4;struct Cyc_CfFlowInfo_JoinEnv _tmp5E3;struct Cyc_CfFlowInfo_JoinEnv _tmp290=(
_tmp5E3.fenv=fenv,((_tmp5E3.pile=((_tmp5E4=_region_malloc(_tmp28F,sizeof(*
_tmp5E4)),((_tmp5E4->rgn=_tmp28F,((_tmp5E4->places=0,_tmp5E4)))))),((_tmp5E3.d1=
_tmp283,((_tmp5E3.d2=_tmp286,_tmp5E3)))))));void*_tmp291=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp290,0,_tmp277,
_tmp27A);struct _tuple12 _tmp5E5;return(_tmp5E5.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp290.pile,all_changed,_tmp289),_tmp28A),((_tmp5E5.f2=_tmp291,_tmp5E5)));};
_LL113:;_LL114: {struct Cyc_Core_Impossible_struct _tmp5EB;const char*_tmp5EA;
struct Cyc_Core_Impossible_struct*_tmp5E9;(int)_throw((void*)((_tmp5E9=_cycalloc(
sizeof(*_tmp5E9)),((_tmp5E9[0]=((_tmp5EB.tag=Cyc_Core_Impossible,((_tmp5EB.f1=((
_tmp5EA="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp5EA,sizeof(
char),37))),_tmp5EB)))),_tmp5E9)))));}_LL10C:;};};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp5F2(struct Cyc_CfFlowInfo_AfterEnv**
env,unsigned int*_tmp5F1,unsigned int*_tmp5F0,void***_tmp5EE,struct
_dyneither_ptr*_tmp2A9,struct _dyneither_ptr*_tmp2A3){for(*_tmp5F1=0;*_tmp5F1 < *
_tmp5F0;(*_tmp5F1)++){(*_tmp5EE)[*_tmp5F1]=Cyc_CfFlowInfo_after_absRval_d(*env,(
int)*_tmp5F1,((void**)(*_tmp2A3).curr)[(int)*_tmp5F1],*((void**)
_check_dyneither_subscript(*_tmp2A9,sizeof(void*),(int)*_tmp5F1)));}}static void*
Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);int
changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);if(
changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*
r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);if(changed1){if(
!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))return r1;env->changed=
Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,
env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct _tuple0 _tmp5EC;
struct _tuple0 _tmp299=(_tmp5EC.f1=r1,((_tmp5EC.f2=r2,_tmp5EC)));void*_tmp29A;void*
_tmp29C;void*_tmp29E;struct Cyc_CfFlowInfo_UnionRInfo _tmp2A0;int _tmp2A1;int
_tmp2A2;struct _dyneither_ptr _tmp2A3;void*_tmp2A4;struct Cyc_CfFlowInfo_UnionRInfo
_tmp2A6;int _tmp2A7;int _tmp2A8;struct _dyneither_ptr _tmp2A9;_LL116: _tmp29A=_tmp299.f1;{
struct Cyc_CfFlowInfo_Consumed_struct*_tmp29B=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp29A;if(_tmp29B->tag != 7)goto _LL118;};_LL117: goto _LL119;_LL118: _tmp29C=
_tmp299.f2;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp29D=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp29C;if(_tmp29D->tag != 7)goto _LL11A;};_LL119: return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
_LL11A: _tmp29E=_tmp299.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp29F=(struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmp29E;if(_tmp29F->tag != 6)goto _LL11C;else{
_tmp2A0=_tmp29F->f1;_tmp2A1=_tmp2A0.is_union;_tmp2A2=_tmp2A0.fieldnum;_tmp2A3=
_tmp29F->f2;}};_tmp2A4=_tmp299.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2A5=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2A4;if(_tmp2A5->tag != 6)goto _LL11C;
else{_tmp2A6=_tmp2A5->f1;_tmp2A7=_tmp2A6.is_union;_tmp2A8=_tmp2A6.fieldnum;
_tmp2A9=_tmp2A5->f2;}};_LL11B: {unsigned int _tmp5F1;unsigned int _tmp5F0;struct
_dyneither_ptr _tmp5EF;void**_tmp5EE;unsigned int _tmp5ED;struct _dyneither_ptr
new_d=(_tmp5ED=_get_dyneither_size(_tmp2A3,sizeof(void*)),((_tmp5EE=(void**)
_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp5ED)),((
_tmp5EF=_tag_dyneither(_tmp5EE,sizeof(void*),_tmp5ED),((((_tmp5F0=_tmp5ED,
_tmp5F2(& env,& _tmp5F1,& _tmp5F0,& _tmp5EE,& _tmp2A9,& _tmp2A3))),_tmp5EF)))))));int
change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp2A3,sizeof(void*));++ i){if(*((
void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp2A3.curr)[
i]){change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp2A3,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp2A9,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5F8;struct Cyc_CfFlowInfo_UnionRInfo
_tmp5F7;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5F6;return(void*)((_tmp5F6=
_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp5F6)),((_tmp5F6[0]=((_tmp5F8.tag=
6,((_tmp5F8.f1=((_tmp5F7.is_union=_tmp2A1,((_tmp5F7.fieldnum=_tmp2A2 == _tmp2A8?
_tmp2A2: - 1,_tmp5F7)))),((_tmp5F8.f2=new_d,_tmp5F8)))))),_tmp5F6))));};}_LL11C:;
_LL11D: {struct Cyc_Core_Impossible_struct _tmp5FE;const char*_tmp5FD;struct Cyc_Core_Impossible_struct*
_tmp5FC;(int)_throw((void*)((_tmp5FC=_cycalloc(sizeof(*_tmp5FC)),((_tmp5FC[0]=((
_tmp5FE.tag=Cyc_Core_Impossible,((_tmp5FE.f1=((_tmp5FD="after_absRval -- non-aggregates!",
_tag_dyneither(_tmp5FD,sizeof(char),33))),_tmp5FE)))),_tmp5FC)))));}_LL115:;};}
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int
key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**_tmp2B4=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp2B5=env->changed;{struct Cyc_List_List*
_tmp5FF;*env->last_field_cell=((_tmp5FF=_region_malloc(((env->joinenv).fenv)->r,
sizeof(*_tmp5FF)),((_tmp5FF->hd=(void*)key,((_tmp5FF->tl=0,_tmp5FF))))));}env->last_field_cell=(
struct Cyc_List_List**)&((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{
void*_tmp2B7=Cyc_CfFlowInfo_after_absRval(env,r1,r2);env->last_field_cell=
_tmp2B4;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=0;env->changed=
_tmp2B5;return _tmp2B7;};};}static void*Cyc_CfFlowInfo_after_root(struct Cyc_CfFlowInfo_AfterEnv*
env,void*root,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_CfFlowInfo_Place
_tmp600;*env->curr_place=((_tmp600.root=root,((_tmp600.fields=0,_tmp600))));}env->last_field_cell=(
struct Cyc_List_List**)&(env->curr_place)->fields;env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo
f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict
chg2){struct _tuple21 _tmp601;struct _tuple21 _tmp2BA=(_tmp601.f1=f1,((_tmp601.f2=f2,
_tmp601)));union Cyc_CfFlowInfo_FlowInfo _tmp2BB;int _tmp2BC;union Cyc_CfFlowInfo_FlowInfo
_tmp2BD;int _tmp2BE;union Cyc_CfFlowInfo_FlowInfo _tmp2BF;struct _tuple11 _tmp2C0;
struct Cyc_Dict_Dict _tmp2C1;struct Cyc_List_List*_tmp2C2;union Cyc_CfFlowInfo_FlowInfo
_tmp2C3;struct _tuple11 _tmp2C4;struct Cyc_Dict_Dict _tmp2C5;struct Cyc_List_List*
_tmp2C6;_LL11F: _tmp2BB=_tmp2BA.f1;if((_tmp2BB.BottomFL).tag != 1)goto _LL121;
_tmp2BC=(int)(_tmp2BB.BottomFL).val;_LL120: goto _LL122;_LL121: _tmp2BD=_tmp2BA.f2;
if((_tmp2BD.BottomFL).tag != 1)goto _LL123;_tmp2BE=(int)(_tmp2BD.BottomFL).val;
_LL122: return Cyc_CfFlowInfo_BottomFL();_LL123: _tmp2BF=_tmp2BA.f1;if((_tmp2BF.ReachableFL).tag
!= 2)goto _LL11E;_tmp2C0=(struct _tuple11)(_tmp2BF.ReachableFL).val;_tmp2C1=
_tmp2C0.f1;_tmp2C2=_tmp2C0.f2;_tmp2C3=_tmp2BA.f2;if((_tmp2C3.ReachableFL).tag != 
2)goto _LL11E;_tmp2C4=(struct _tuple11)(_tmp2C3.ReachableFL).val;_tmp2C5=_tmp2C4.f1;
_tmp2C6=_tmp2C4.f2;_LL124: if(_tmp2C1.t == _tmp2C5.t  && _tmp2C2 == _tmp2C6)return f1;{
struct _RegionHandle*_tmp2C7=fenv->r;struct Cyc_CfFlowInfo_Place*_tmp2C8=fenv->dummy_place;
struct Cyc_CfFlowInfo_JoinEnv _tmp607;struct Cyc_CfFlowInfo_EscPile*_tmp606;struct
Cyc_CfFlowInfo_AfterEnv _tmp605;struct Cyc_CfFlowInfo_AfterEnv _tmp2C9=(_tmp605.joinenv=((
_tmp607.fenv=fenv,((_tmp607.pile=((_tmp606=_region_malloc(_tmp2C7,sizeof(*
_tmp606)),((_tmp606->rgn=_tmp2C7,((_tmp606->places=0,_tmp606)))))),((_tmp607.d1=
_tmp2C1,((_tmp607.d2=_tmp2C5,_tmp607)))))))),((_tmp605.chg1=chg1,((_tmp605.chg2=
chg2,((_tmp605.curr_place=_tmp2C8,((_tmp605.last_field_cell=(struct Cyc_List_List**)&
_tmp2C8->fields,((_tmp605.changed=Cyc_CfFlowInfo_Neither,_tmp605)))))))))));
struct Cyc_Dict_Dict _tmp2CA=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,
void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,
struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp2C9,
_tmp2C1,_tmp2C5);struct Cyc_List_List*_tmp2CB=Cyc_CfFlowInfo_join_relns(fenv->r,
_tmp2C2,_tmp2C6);return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,(_tmp2C9.joinenv).pile,all_changed,_tmp2CA),_tmp2CB);};_LL11E:;}static int
Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){if(r1 == r2)
return 1;{struct _tuple0 _tmp608;struct _tuple0 _tmp2D0=(_tmp608.f1=r1,((_tmp608.f2=
r2,_tmp608)));void*_tmp2D1;struct Cyc_CfFlowInfo_Place*_tmp2D3;void*_tmp2D4;
struct Cyc_CfFlowInfo_Place*_tmp2D6;void*_tmp2D7;void*_tmp2D9;void*_tmp2DB;struct
Cyc_CfFlowInfo_UnionRInfo _tmp2DD;struct _dyneither_ptr _tmp2DE;void*_tmp2DF;struct
Cyc_CfFlowInfo_UnionRInfo _tmp2E1;struct _dyneither_ptr _tmp2E2;void*_tmp2E3;void*
_tmp2E5;void*_tmp2E7;void*_tmp2E9;void*_tmp2EB;void*_tmp2ED;void*_tmp2EF;void*
_tmp2F1;_LL126: _tmp2D1=_tmp2D0.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp2D2=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2D1;if(_tmp2D2->tag != 5)goto _LL128;
else{_tmp2D3=_tmp2D2->f1;}};_tmp2D4=_tmp2D0.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2D5=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2D4;if(_tmp2D5->tag != 5)goto
_LL128;else{_tmp2D6=_tmp2D5->f1;}};_LL127: return Cyc_CfFlowInfo_place_cmp(_tmp2D3,
_tmp2D6)== 0;_LL128: _tmp2D7=_tmp2D0.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2D8=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2D7;if(_tmp2D8->tag != 5)goto
_LL12A;};_LL129: goto _LL12B;_LL12A: _tmp2D9=_tmp2D0.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2DA=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2D9;if(_tmp2DA->tag != 5)goto
_LL12C;};_LL12B: return 0;_LL12C: _tmp2DB=_tmp2D0.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2DC=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2DB;if(_tmp2DC->tag != 6)goto
_LL12E;else{_tmp2DD=_tmp2DC->f1;_tmp2DE=_tmp2DC->f2;}};_tmp2DF=_tmp2D0.f2;{
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2E0=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2DF;if(_tmp2E0->tag != 6)goto _LL12E;else{_tmp2E1=_tmp2E0->f1;_tmp2E2=_tmp2E0->f2;}};
_LL12D: if((void**)_tmp2DE.curr == (void**)_tmp2E2.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp2DE,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2DE.curr)[i],*((
void**)_check_dyneither_subscript(_tmp2E2,sizeof(void*),i))))return 0;}}return 1;
_LL12E: _tmp2E3=_tmp2D0.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2E4=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp2E3;if(_tmp2E4->tag != 1)goto _LL130;};
_tmp2E5=_tmp2D0.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2E6=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2E5;if(_tmp2E6->tag != 2)goto _LL130;};_LL12F: return 1;_LL130: _tmp2E7=_tmp2D0.f2;{
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2E8=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2E7;if(_tmp2E8->tag != 2)goto _LL132;};_LL131: return 0;_LL132: _tmp2E9=_tmp2D0.f2;{
struct Cyc_CfFlowInfo_Zero_struct*_tmp2EA=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp2E9;if(_tmp2EA->tag != 0)goto _LL134;};_LL133: goto _LL135;_LL134: _tmp2EB=
_tmp2D0.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2EC=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp2EB;if(_tmp2EC->tag != 1)goto _LL136;};_LL135: return 0;_LL136: _tmp2ED=_tmp2D0.f1;{
struct Cyc_CfFlowInfo_Esc_struct*_tmp2EE=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp2ED;if(_tmp2EE->tag != 4)goto _LL138;};_tmp2EF=_tmp2D0.f2;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp2F0=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2EF;if(_tmp2F0->tag != 4)goto _LL138;};
_LL137: goto _LL125;_LL138: _tmp2F1=_tmp2D0.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp2F2=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2F1;if(_tmp2F2->tag != 4)goto _LL13A;};
_LL139: return 0;_LL13A:;_LL13B: goto _LL125;_LL125:;}{struct _tuple19 _tmp609;struct
_tuple19 _tmp2F4=(_tmp609.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp609.f2=Cyc_CfFlowInfo_initlevel_approx(
r2),_tmp609)));enum Cyc_CfFlowInfo_InitLevel _tmp2F5;enum Cyc_CfFlowInfo_InitLevel
_tmp2F6;enum Cyc_CfFlowInfo_InitLevel _tmp2F7;enum Cyc_CfFlowInfo_InitLevel
_tmp2F8;enum Cyc_CfFlowInfo_InitLevel _tmp2F9;enum Cyc_CfFlowInfo_InitLevel
_tmp2FA;_LL13D: _tmp2F5=_tmp2F4.f1;if(_tmp2F5 != Cyc_CfFlowInfo_AllIL)goto _LL13F;
_tmp2F6=_tmp2F4.f2;if(_tmp2F6 != Cyc_CfFlowInfo_AllIL)goto _LL13F;_LL13E: return 1;
_LL13F: _tmp2F7=_tmp2F4.f2;if(_tmp2F7 != Cyc_CfFlowInfo_NoneIL)goto _LL141;_LL140:
return 1;_LL141: _tmp2F8=_tmp2F4.f1;if(_tmp2F8 != Cyc_CfFlowInfo_NoneIL)goto _LL143;
_LL142: return 0;_LL143: _tmp2F9=_tmp2F4.f2;if(_tmp2F9 != Cyc_CfFlowInfo_ThisIL)goto
_LL145;_LL144: return 1;_LL145: _tmp2FA=_tmp2F4.f1;if(_tmp2FA != Cyc_CfFlowInfo_ThisIL)
goto _LL13C;_LL146: return 0;_LL13C:;};}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s != 0;
r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp2FB=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;
int found=0;{struct Cyc_List_List*_tmp2FC=r2s;for(0;_tmp2FC != 0;_tmp2FC=_tmp2FC->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2FD=(struct Cyc_CfFlowInfo_Reln*)_tmp2FC->hd;if(
_tmp2FB == _tmp2FD  || _tmp2FB->vd == _tmp2FD->vd  && Cyc_CfFlowInfo_same_relop(
_tmp2FB->rop,_tmp2FD->rop)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){struct _tuple21
_tmp60A;struct _tuple21 _tmp2FF=(_tmp60A.f1=f1,((_tmp60A.f2=f2,_tmp60A)));union Cyc_CfFlowInfo_FlowInfo
_tmp300;int _tmp301;union Cyc_CfFlowInfo_FlowInfo _tmp302;int _tmp303;union Cyc_CfFlowInfo_FlowInfo
_tmp304;struct _tuple11 _tmp305;struct Cyc_Dict_Dict _tmp306;struct Cyc_List_List*
_tmp307;union Cyc_CfFlowInfo_FlowInfo _tmp308;struct _tuple11 _tmp309;struct Cyc_Dict_Dict
_tmp30A;struct Cyc_List_List*_tmp30B;_LL148: _tmp300=_tmp2FF.f1;if((_tmp300.BottomFL).tag
!= 1)goto _LL14A;_tmp301=(int)(_tmp300.BottomFL).val;_LL149: return 1;_LL14A:
_tmp302=_tmp2FF.f2;if((_tmp302.BottomFL).tag != 1)goto _LL14C;_tmp303=(int)(
_tmp302.BottomFL).val;_LL14B: return 0;_LL14C: _tmp304=_tmp2FF.f1;if((_tmp304.ReachableFL).tag
!= 2)goto _LL147;_tmp305=(struct _tuple11)(_tmp304.ReachableFL).val;_tmp306=
_tmp305.f1;_tmp307=_tmp305.f2;_tmp308=_tmp2FF.f2;if((_tmp308.ReachableFL).tag != 
2)goto _LL147;_tmp309=(struct _tuple11)(_tmp308.ReachableFL).val;_tmp30A=_tmp309.f1;
_tmp30B=_tmp309.f2;_LL14D: if(_tmp306.t == _tmp30A.t  && _tmp307 == _tmp30B)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp306,_tmp30A) && Cyc_CfFlowInfo_relns_approx(_tmp307,_tmp30B);_LL147:;}struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*
rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=0;for(p=rs;!found
 && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp30C=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp30C->vd == v){found=1;break;}{union Cyc_CfFlowInfo_RelnOp _tmp30D=
_tmp30C->rop;struct _tuple10 _tmp30E;struct Cyc_Absyn_Vardecl*_tmp30F;struct Cyc_Absyn_Vardecl*
_tmp310;struct Cyc_Absyn_Vardecl*_tmp311;_LL14F: if((_tmp30D.LessVar).tag != 2)goto
_LL151;_tmp30E=(struct _tuple10)(_tmp30D.LessVar).val;_tmp30F=_tmp30E.f1;_LL150:
_tmp310=_tmp30F;goto _LL152;_LL151: if((_tmp30D.LessNumelts).tag != 3)goto _LL153;
_tmp310=(struct Cyc_Absyn_Vardecl*)(_tmp30D.LessNumelts).val;_LL152: _tmp311=
_tmp310;goto _LL154;_LL153: if((_tmp30D.LessEqNumelts).tag != 5)goto _LL155;_tmp311=(
struct Cyc_Absyn_Vardecl*)(_tmp30D.LessEqNumelts).val;_LL154: if(v == _tmp311)found=
1;goto _LL14E;_LL155:;_LL156: goto _LL14E;_LL14E:;};}if(!found)return rs;{struct Cyc_List_List*
_tmp312=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp313=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp313->vd != v){{union Cyc_CfFlowInfo_RelnOp _tmp314=_tmp313->rop;struct
_tuple10 _tmp315;struct Cyc_Absyn_Vardecl*_tmp316;struct Cyc_Absyn_Vardecl*_tmp317;
struct Cyc_Absyn_Vardecl*_tmp318;_LL158: if((_tmp314.LessVar).tag != 2)goto _LL15A;
_tmp315=(struct _tuple10)(_tmp314.LessVar).val;_tmp316=_tmp315.f1;_LL159: _tmp317=
_tmp316;goto _LL15B;_LL15A: if((_tmp314.LessNumelts).tag != 3)goto _LL15C;_tmp317=(
struct Cyc_Absyn_Vardecl*)(_tmp314.LessNumelts).val;_LL15B: _tmp318=_tmp317;goto
_LL15D;_LL15C: if((_tmp314.LessEqNumelts).tag != 5)goto _LL15E;_tmp318=(struct Cyc_Absyn_Vardecl*)(
_tmp314.LessEqNumelts).val;_LL15D: if(v == _tmp318)continue;goto _LL157;_LL15E:;
_LL15F: goto _LL157;_LL157:;}{struct Cyc_List_List*_tmp60B;_tmp312=((_tmp60B=
_region_malloc(rgn,sizeof(*_tmp60B)),((_tmp60B->hd=_tmp313,((_tmp60B->tl=_tmp312,
_tmp60B))))));};}}return _tmp312;};}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*
_tmp31A=e->r;void*_tmp31C;struct Cyc_Absyn_Vardecl*_tmp31E;void*_tmp320;struct Cyc_Absyn_Vardecl*
_tmp322;void*_tmp324;struct Cyc_Absyn_Vardecl*_tmp326;void*_tmp328;struct Cyc_Absyn_Vardecl*
_tmp32A;_LL161: {struct Cyc_Absyn_Var_e_struct*_tmp31B=(struct Cyc_Absyn_Var_e_struct*)
_tmp31A;if(_tmp31B->tag != 1)goto _LL163;else{_tmp31C=(void*)_tmp31B->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp31D=(struct Cyc_Absyn_Global_b_struct*)_tmp31C;if(
_tmp31D->tag != 1)goto _LL163;else{_tmp31E=_tmp31D->f1;}};}}_LL162: _tmp322=_tmp31E;
goto _LL164;_LL163: {struct Cyc_Absyn_Var_e_struct*_tmp31F=(struct Cyc_Absyn_Var_e_struct*)
_tmp31A;if(_tmp31F->tag != 1)goto _LL165;else{_tmp320=(void*)_tmp31F->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp321=(struct Cyc_Absyn_Param_b_struct*)_tmp320;if(
_tmp321->tag != 3)goto _LL165;else{_tmp322=_tmp321->f1;}};}}_LL164: _tmp326=_tmp322;
goto _LL166;_LL165: {struct Cyc_Absyn_Var_e_struct*_tmp323=(struct Cyc_Absyn_Var_e_struct*)
_tmp31A;if(_tmp323->tag != 1)goto _LL167;else{_tmp324=(void*)_tmp323->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp325=(struct Cyc_Absyn_Local_b_struct*)_tmp324;if(
_tmp325->tag != 4)goto _LL167;else{_tmp326=_tmp325->f1;}};}}_LL166: _tmp32A=_tmp326;
goto _LL168;_LL167: {struct Cyc_Absyn_Var_e_struct*_tmp327=(struct Cyc_Absyn_Var_e_struct*)
_tmp31A;if(_tmp327->tag != 1)goto _LL169;else{_tmp328=(void*)_tmp327->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp329=(struct Cyc_Absyn_Pat_b_struct*)_tmp328;if(_tmp329->tag
!= 5)goto _LL169;else{_tmp32A=_tmp329->f1;}};}}_LL168: if(!_tmp32A->escapes)return
Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp32A);goto _LL160;_LL169:;_LL16A: goto _LL160;
_LL160:;}return r;}struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct
_RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){struct Cyc_CfFlowInfo_Reln*_tmp60C;
return(_tmp60C=_region_malloc(r2,sizeof(*_tmp60C)),((_tmp60C[0]=*r,_tmp60C)));}
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*
relns){return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*
f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){for(
0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct Cyc_CfFlowInfo_Reln*_tmp32C=(
struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*_tmp32D=(struct Cyc_CfFlowInfo_Reln*)
r2->hd;if(_tmp32C->vd != _tmp32D->vd  || !Cyc_CfFlowInfo_same_relop(_tmp32C->rop,
_tmp32D->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(
rgn,r,v);{void*_tmp32E=e->r;struct Cyc_Absyn_MallocInfo _tmp330;struct Cyc_Absyn_Exp*
_tmp331;int _tmp332;_LL16C: {struct Cyc_Absyn_Malloc_e_struct*_tmp32F=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp32E;if(_tmp32F->tag != 34)goto _LL16E;else{_tmp330=_tmp32F->f1;_tmp331=_tmp330.num_elts;
_tmp332=_tmp330.fat_result;if(_tmp332 != 1)goto _LL16E;}}_LL16D: malloc_loop: {void*
_tmp333=_tmp331->r;struct Cyc_Absyn_Exp*_tmp335;void*_tmp337;struct Cyc_Absyn_Vardecl*
_tmp339;void*_tmp33B;struct Cyc_Absyn_Vardecl*_tmp33D;void*_tmp33F;struct Cyc_Absyn_Vardecl*
_tmp341;void*_tmp343;struct Cyc_Absyn_Vardecl*_tmp345;_LL171: {struct Cyc_Absyn_Cast_e_struct*
_tmp334=(struct Cyc_Absyn_Cast_e_struct*)_tmp333;if(_tmp334->tag != 15)goto _LL173;
else{_tmp335=_tmp334->f2;}}_LL172: _tmp331=_tmp335;goto malloc_loop;_LL173: {
struct Cyc_Absyn_Var_e_struct*_tmp336=(struct Cyc_Absyn_Var_e_struct*)_tmp333;if(
_tmp336->tag != 1)goto _LL175;else{_tmp337=(void*)_tmp336->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp338=(struct Cyc_Absyn_Pat_b_struct*)_tmp337;if(_tmp338->tag != 5)goto _LL175;
else{_tmp339=_tmp338->f1;}};}}_LL174: _tmp33D=_tmp339;goto _LL176;_LL175: {struct
Cyc_Absyn_Var_e_struct*_tmp33A=(struct Cyc_Absyn_Var_e_struct*)_tmp333;if(_tmp33A->tag
!= 1)goto _LL177;else{_tmp33B=(void*)_tmp33A->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp33C=(struct Cyc_Absyn_Local_b_struct*)_tmp33B;if(_tmp33C->tag != 4)goto _LL177;
else{_tmp33D=_tmp33C->f1;}};}}_LL176: _tmp341=_tmp33D;goto _LL178;_LL177: {struct
Cyc_Absyn_Var_e_struct*_tmp33E=(struct Cyc_Absyn_Var_e_struct*)_tmp333;if(_tmp33E->tag
!= 1)goto _LL179;else{_tmp33F=(void*)_tmp33E->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp340=(struct Cyc_Absyn_Param_b_struct*)_tmp33F;if(_tmp340->tag != 3)goto _LL179;
else{_tmp341=_tmp340->f1;}};}}_LL178: _tmp345=_tmp341;goto _LL17A;_LL179: {struct
Cyc_Absyn_Var_e_struct*_tmp342=(struct Cyc_Absyn_Var_e_struct*)_tmp333;if(_tmp342->tag
!= 1)goto _LL17B;else{_tmp343=(void*)_tmp342->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp344=(struct Cyc_Absyn_Global_b_struct*)_tmp343;if(_tmp344->tag != 1)goto _LL17B;
else{_tmp345=_tmp344->f1;}};}}_LL17A: if(_tmp345->escapes)return r;{struct Cyc_CfFlowInfo_Reln*
_tmp60F;struct Cyc_List_List*_tmp60E;return(_tmp60E=_region_malloc(rgn,sizeof(*
_tmp60E)),((_tmp60E->hd=((_tmp60F=_region_malloc(rgn,sizeof(*_tmp60F)),((_tmp60F->vd=
_tmp345,((_tmp60F->rop=Cyc_CfFlowInfo_LessEqNumelts(v),_tmp60F)))))),((_tmp60E->tl=
r,_tmp60E)))));};_LL17B:;_LL17C: return r;_LL170:;}_LL16E:;_LL16F: goto _LL16B;
_LL16B:;}{void*_tmp348=Cyc_Tcutil_compress(v->type);_LL17E: {struct Cyc_Absyn_IntType_struct*
_tmp349=(struct Cyc_Absyn_IntType_struct*)_tmp348;if(_tmp349->tag != 6)goto _LL180;}
_LL17F: goto _LL17D;_LL180:;_LL181: return r;_LL17D:;}loop: {void*_tmp34A=e->r;
struct Cyc_Absyn_Exp*_tmp34C;union Cyc_Absyn_Cnst _tmp34E;struct _tuple6 _tmp34F;int
_tmp350;enum Cyc_Absyn_Primop _tmp352;struct Cyc_List_List*_tmp353;struct Cyc_List_List
_tmp354;struct Cyc_List_List*_tmp355;struct Cyc_List_List _tmp356;struct Cyc_Absyn_Exp*
_tmp357;enum Cyc_Absyn_Primop _tmp359;struct Cyc_List_List*_tmp35A;struct Cyc_List_List
_tmp35B;struct Cyc_Absyn_Exp*_tmp35C;_LL183: {struct Cyc_Absyn_Cast_e_struct*
_tmp34B=(struct Cyc_Absyn_Cast_e_struct*)_tmp34A;if(_tmp34B->tag != 15)goto _LL185;
else{_tmp34C=_tmp34B->f2;}}_LL184: e=_tmp34C;goto loop;_LL185: {struct Cyc_Absyn_Const_e_struct*
_tmp34D=(struct Cyc_Absyn_Const_e_struct*)_tmp34A;if(_tmp34D->tag != 0)goto _LL187;
else{_tmp34E=_tmp34D->f1;if((_tmp34E.Int_c).tag != 4)goto _LL187;_tmp34F=(struct
_tuple6)(_tmp34E.Int_c).val;_tmp350=_tmp34F.f2;}}_LL186: {struct Cyc_CfFlowInfo_Reln*
_tmp612;struct Cyc_List_List*_tmp611;return(_tmp611=_region_malloc(rgn,sizeof(*
_tmp611)),((_tmp611->hd=((_tmp612=_region_malloc(rgn,sizeof(*_tmp612)),((_tmp612->vd=
v,((_tmp612->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp350),_tmp612)))))),((
_tmp611->tl=r,_tmp611)))));}_LL187: {struct Cyc_Absyn_Primop_e_struct*_tmp351=(
struct Cyc_Absyn_Primop_e_struct*)_tmp34A;if(_tmp351->tag != 3)goto _LL189;else{
_tmp352=_tmp351->f1;if(_tmp352 != Cyc_Absyn_Mod)goto _LL189;_tmp353=_tmp351->f2;
if(_tmp353 == 0)goto _LL189;_tmp354=*_tmp353;_tmp355=_tmp354.tl;if(_tmp355 == 0)
goto _LL189;_tmp356=*_tmp355;_tmp357=(struct Cyc_Absyn_Exp*)_tmp356.hd;}}_LL188:{
void*_tmp35F=_tmp357->r;enum Cyc_Absyn_Primop _tmp361;struct Cyc_List_List*_tmp362;
struct Cyc_List_List _tmp363;struct Cyc_Absyn_Exp*_tmp364;_LL18E: {struct Cyc_Absyn_Primop_e_struct*
_tmp360=(struct Cyc_Absyn_Primop_e_struct*)_tmp35F;if(_tmp360->tag != 3)goto _LL190;
else{_tmp361=_tmp360->f1;_tmp362=_tmp360->f2;if(_tmp362 == 0)goto _LL190;_tmp363=*
_tmp362;_tmp364=(struct Cyc_Absyn_Exp*)_tmp363.hd;}}_LL18F:{void*_tmp365=_tmp364->r;
void*_tmp367;struct Cyc_Absyn_Vardecl*_tmp369;void*_tmp36B;struct Cyc_Absyn_Vardecl*
_tmp36D;void*_tmp36F;struct Cyc_Absyn_Vardecl*_tmp371;void*_tmp373;struct Cyc_Absyn_Vardecl*
_tmp375;_LL193: {struct Cyc_Absyn_Var_e_struct*_tmp366=(struct Cyc_Absyn_Var_e_struct*)
_tmp365;if(_tmp366->tag != 1)goto _LL195;else{_tmp367=(void*)_tmp366->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp368=(struct Cyc_Absyn_Global_b_struct*)_tmp367;if(
_tmp368->tag != 1)goto _LL195;else{_tmp369=_tmp368->f1;}};}}_LL194: _tmp36D=_tmp369;
goto _LL196;_LL195: {struct Cyc_Absyn_Var_e_struct*_tmp36A=(struct Cyc_Absyn_Var_e_struct*)
_tmp365;if(_tmp36A->tag != 1)goto _LL197;else{_tmp36B=(void*)_tmp36A->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp36C=(struct Cyc_Absyn_Local_b_struct*)_tmp36B;if(
_tmp36C->tag != 4)goto _LL197;else{_tmp36D=_tmp36C->f1;}};}}_LL196: _tmp371=_tmp36D;
goto _LL198;_LL197: {struct Cyc_Absyn_Var_e_struct*_tmp36E=(struct Cyc_Absyn_Var_e_struct*)
_tmp365;if(_tmp36E->tag != 1)goto _LL199;else{_tmp36F=(void*)_tmp36E->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp370=(struct Cyc_Absyn_Param_b_struct*)_tmp36F;if(
_tmp370->tag != 3)goto _LL199;else{_tmp371=_tmp370->f1;}};}}_LL198: _tmp375=_tmp371;
goto _LL19A;_LL199: {struct Cyc_Absyn_Var_e_struct*_tmp372=(struct Cyc_Absyn_Var_e_struct*)
_tmp365;if(_tmp372->tag != 1)goto _LL19B;else{_tmp373=(void*)_tmp372->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp374=(struct Cyc_Absyn_Pat_b_struct*)_tmp373;if(_tmp374->tag
!= 5)goto _LL19B;else{_tmp375=_tmp374->f1;}};}}_LL19A: if(_tmp375->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp615;struct Cyc_List_List*_tmp614;return(_tmp614=
_region_malloc(rgn,sizeof(*_tmp614)),((_tmp614->hd=((_tmp615=_region_malloc(rgn,
sizeof(*_tmp615)),((_tmp615->vd=v,((_tmp615->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp375),_tmp615)))))),((_tmp614->tl=r,_tmp614)))));};_LL19B:;_LL19C: goto _LL192;
_LL192:;}goto _LL18D;_LL190:;_LL191: goto _LL18D;_LL18D:;}goto _LL182;_LL189: {
struct Cyc_Absyn_Primop_e_struct*_tmp358=(struct Cyc_Absyn_Primop_e_struct*)
_tmp34A;if(_tmp358->tag != 3)goto _LL18B;else{_tmp359=_tmp358->f1;_tmp35A=_tmp358->f2;
if(_tmp35A == 0)goto _LL18B;_tmp35B=*_tmp35A;_tmp35C=(struct Cyc_Absyn_Exp*)_tmp35B.hd;}}
_LL18A:{void*_tmp378=_tmp35C->r;void*_tmp37A;struct Cyc_Absyn_Vardecl*_tmp37C;
void*_tmp37E;struct Cyc_Absyn_Vardecl*_tmp380;void*_tmp382;struct Cyc_Absyn_Vardecl*
_tmp384;void*_tmp386;struct Cyc_Absyn_Vardecl*_tmp388;_LL19E: {struct Cyc_Absyn_Var_e_struct*
_tmp379=(struct Cyc_Absyn_Var_e_struct*)_tmp378;if(_tmp379->tag != 1)goto _LL1A0;
else{_tmp37A=(void*)_tmp379->f2;{struct Cyc_Absyn_Global_b_struct*_tmp37B=(struct
Cyc_Absyn_Global_b_struct*)_tmp37A;if(_tmp37B->tag != 1)goto _LL1A0;else{_tmp37C=
_tmp37B->f1;}};}}_LL19F: _tmp380=_tmp37C;goto _LL1A1;_LL1A0: {struct Cyc_Absyn_Var_e_struct*
_tmp37D=(struct Cyc_Absyn_Var_e_struct*)_tmp378;if(_tmp37D->tag != 1)goto _LL1A2;
else{_tmp37E=(void*)_tmp37D->f2;{struct Cyc_Absyn_Local_b_struct*_tmp37F=(struct
Cyc_Absyn_Local_b_struct*)_tmp37E;if(_tmp37F->tag != 4)goto _LL1A2;else{_tmp380=
_tmp37F->f1;}};}}_LL1A1: _tmp384=_tmp380;goto _LL1A3;_LL1A2: {struct Cyc_Absyn_Var_e_struct*
_tmp381=(struct Cyc_Absyn_Var_e_struct*)_tmp378;if(_tmp381->tag != 1)goto _LL1A4;
else{_tmp382=(void*)_tmp381->f2;{struct Cyc_Absyn_Param_b_struct*_tmp383=(struct
Cyc_Absyn_Param_b_struct*)_tmp382;if(_tmp383->tag != 3)goto _LL1A4;else{_tmp384=
_tmp383->f1;}};}}_LL1A3: _tmp388=_tmp384;goto _LL1A5;_LL1A4: {struct Cyc_Absyn_Var_e_struct*
_tmp385=(struct Cyc_Absyn_Var_e_struct*)_tmp378;if(_tmp385->tag != 1)goto _LL1A6;
else{_tmp386=(void*)_tmp385->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp387=(struct Cyc_Absyn_Pat_b_struct*)
_tmp386;if(_tmp387->tag != 5)goto _LL1A6;else{_tmp388=_tmp387->f1;}};}}_LL1A5: if(
_tmp388->escapes)return r;{struct Cyc_CfFlowInfo_Reln*_tmp618;struct Cyc_List_List*
_tmp617;return(_tmp617=_region_malloc(rgn,sizeof(*_tmp617)),((_tmp617->hd=((
_tmp618=_region_malloc(rgn,sizeof(*_tmp618)),((_tmp618->vd=v,((_tmp618->rop=Cyc_CfFlowInfo_LessEqNumelts(
_tmp388),_tmp618)))))),((_tmp617->tl=r,_tmp617)))));};_LL1A6:;_LL1A7: goto _LL19D;
_LL19D:;}goto _LL182;_LL18B:;_LL18C: goto _LL182;_LL182:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp38B=e1->r;void*_tmp38D;
struct Cyc_Absyn_Vardecl*_tmp38F;void*_tmp391;struct Cyc_Absyn_Vardecl*_tmp393;
void*_tmp395;struct Cyc_Absyn_Vardecl*_tmp397;void*_tmp399;struct Cyc_Absyn_Vardecl*
_tmp39B;_LL1A9: {struct Cyc_Absyn_Var_e_struct*_tmp38C=(struct Cyc_Absyn_Var_e_struct*)
_tmp38B;if(_tmp38C->tag != 1)goto _LL1AB;else{_tmp38D=(void*)_tmp38C->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp38E=(struct Cyc_Absyn_Global_b_struct*)_tmp38D;if(
_tmp38E->tag != 1)goto _LL1AB;else{_tmp38F=_tmp38E->f1;}};}}_LL1AA: _tmp393=_tmp38F;
goto _LL1AC;_LL1AB: {struct Cyc_Absyn_Var_e_struct*_tmp390=(struct Cyc_Absyn_Var_e_struct*)
_tmp38B;if(_tmp390->tag != 1)goto _LL1AD;else{_tmp391=(void*)_tmp390->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp392=(struct Cyc_Absyn_Param_b_struct*)_tmp391;if(
_tmp392->tag != 3)goto _LL1AD;else{_tmp393=_tmp392->f1;}};}}_LL1AC: _tmp397=_tmp393;
goto _LL1AE;_LL1AD: {struct Cyc_Absyn_Var_e_struct*_tmp394=(struct Cyc_Absyn_Var_e_struct*)
_tmp38B;if(_tmp394->tag != 1)goto _LL1AF;else{_tmp395=(void*)_tmp394->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp396=(struct Cyc_Absyn_Local_b_struct*)_tmp395;if(
_tmp396->tag != 4)goto _LL1AF;else{_tmp397=_tmp396->f1;}};}}_LL1AE: _tmp39B=_tmp397;
goto _LL1B0;_LL1AF: {struct Cyc_Absyn_Var_e_struct*_tmp398=(struct Cyc_Absyn_Var_e_struct*)
_tmp38B;if(_tmp398->tag != 1)goto _LL1B1;else{_tmp399=(void*)_tmp398->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp39A=(struct Cyc_Absyn_Pat_b_struct*)_tmp399;if(_tmp39A->tag
!= 5)goto _LL1B1;else{_tmp39B=_tmp39A->f1;}};}}_LL1B0: if(!_tmp39B->escapes)return
Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp39B,e2);goto _LL1A8;_LL1B1:;_LL1B2: goto
_LL1A8;_LL1A8:;}return r;}static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){{const char*_tmp61C;void*_tmp61B[1];struct Cyc_String_pa_struct _tmp61A;(_tmp61A.tag=
0,((_tmp61A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name)),((_tmp61B[0]=& _tmp61A,Cyc_fprintf(Cyc_stderr,((_tmp61C="%s",
_tag_dyneither(_tmp61C,sizeof(char),3))),_tag_dyneither(_tmp61B,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp39F=r->rop;unsigned int _tmp3A0;struct _tuple10
_tmp3A1;struct Cyc_Absyn_Vardecl*_tmp3A2;struct Cyc_Absyn_Vardecl*_tmp3A3;
unsigned int _tmp3A4;unsigned int _tmp3A5;struct Cyc_Absyn_Vardecl*_tmp3A6;_LL1B4:
if((_tmp39F.EqualConst).tag != 1)goto _LL1B6;_tmp3A0=(unsigned int)(_tmp39F.EqualConst).val;
_LL1B5:{const char*_tmp620;void*_tmp61F[1];struct Cyc_Int_pa_struct _tmp61E;(
_tmp61E.tag=1,((_tmp61E.f1=(unsigned long)((int)_tmp3A0),((_tmp61F[0]=& _tmp61E,
Cyc_fprintf(Cyc_stderr,((_tmp620="==%d",_tag_dyneither(_tmp620,sizeof(char),5))),
_tag_dyneither(_tmp61F,sizeof(void*),1)))))));}goto _LL1B3;_LL1B6: if((_tmp39F.LessVar).tag
!= 2)goto _LL1B8;_tmp3A1=(struct _tuple10)(_tmp39F.LessVar).val;_tmp3A2=_tmp3A1.f1;
_LL1B7:{const char*_tmp624;void*_tmp623[1];struct Cyc_String_pa_struct _tmp622;(
_tmp622.tag=0,((_tmp622.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp3A2->name)),((_tmp623[0]=& _tmp622,Cyc_fprintf(Cyc_stderr,((_tmp624="<%s",
_tag_dyneither(_tmp624,sizeof(char),4))),_tag_dyneither(_tmp623,sizeof(void*),1)))))));}
goto _LL1B3;_LL1B8: if((_tmp39F.LessNumelts).tag != 3)goto _LL1BA;_tmp3A3=(struct Cyc_Absyn_Vardecl*)(
_tmp39F.LessNumelts).val;_LL1B9:{const char*_tmp628;void*_tmp627[1];struct Cyc_String_pa_struct
_tmp626;(_tmp626.tag=0,((_tmp626.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp3A3->name)),((_tmp627[0]=& _tmp626,Cyc_fprintf(Cyc_stderr,((
_tmp628="<numelts(%s)",_tag_dyneither(_tmp628,sizeof(char),13))),_tag_dyneither(
_tmp627,sizeof(void*),1)))))));}goto _LL1B3;_LL1BA: if((_tmp39F.LessConst).tag != 4)
goto _LL1BC;_tmp3A4=(unsigned int)(_tmp39F.LessConst).val;_LL1BB:{const char*
_tmp62C;void*_tmp62B[1];struct Cyc_Int_pa_struct _tmp62A;(_tmp62A.tag=1,((_tmp62A.f1=(
unsigned long)((int)_tmp3A4),((_tmp62B[0]=& _tmp62A,Cyc_fprintf(Cyc_stderr,((
_tmp62C="<%d",_tag_dyneither(_tmp62C,sizeof(char),4))),_tag_dyneither(_tmp62B,
sizeof(void*),1)))))));}goto _LL1B3;_LL1BC: if((_tmp39F.LessEqConst).tag != 6)goto
_LL1BE;_tmp3A5=(unsigned int)(_tmp39F.LessEqConst).val;_LL1BD:{const char*_tmp630;
void*_tmp62F[1];struct Cyc_Int_pa_struct _tmp62E;(_tmp62E.tag=1,((_tmp62E.f1=(
unsigned long)((int)_tmp3A5),((_tmp62F[0]=& _tmp62E,Cyc_fprintf(Cyc_stderr,((
_tmp630="<=%d",_tag_dyneither(_tmp630,sizeof(char),5))),_tag_dyneither(_tmp62F,
sizeof(void*),1)))))));}goto _LL1B3;_LL1BE: if((_tmp39F.LessEqNumelts).tag != 5)
goto _LL1B3;_tmp3A6=(struct Cyc_Absyn_Vardecl*)(_tmp39F.LessEqNumelts).val;_LL1BF:{
const char*_tmp634;void*_tmp633[1];struct Cyc_String_pa_struct _tmp632;(_tmp632.tag=
0,((_tmp632.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp3A6->name)),((_tmp633[0]=& _tmp632,Cyc_fprintf(Cyc_stderr,((_tmp634="<=numelts(%s)",
_tag_dyneither(_tmp634,sizeof(char),14))),_tag_dyneither(_tmp633,sizeof(void*),1)))))));}
goto _LL1B3;_LL1B3:;};}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(
0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);
if(r->tl != 0){const char*_tmp637;void*_tmp636;(_tmp636=0,Cyc_fprintf(Cyc_stderr,((
_tmp637=",",_tag_dyneither(_tmp637,sizeof(char),2))),_tag_dyneither(_tmp636,
sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel
il){switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1C0:{const char*_tmp63A;void*_tmp639;(
_tmp639=0,Cyc_fprintf(Cyc_stderr,((_tmp63A="uninitialized",_tag_dyneither(
_tmp63A,sizeof(char),14))),_tag_dyneither(_tmp639,sizeof(void*),0)));}break;case
Cyc_CfFlowInfo_ThisIL: _LL1C1:{const char*_tmp63D;void*_tmp63C;(_tmp63C=0,Cyc_fprintf(
Cyc_stderr,((_tmp63D="this-initialized",_tag_dyneither(_tmp63D,sizeof(char),17))),
_tag_dyneither(_tmp63C,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1C2:{
const char*_tmp640;void*_tmp63F;(_tmp63F=0,Cyc_fprintf(Cyc_stderr,((_tmp640="all-initialized",
_tag_dyneither(_tmp640,sizeof(char),16))),_tag_dyneither(_tmp63F,sizeof(void*),0)));}
break;}}void Cyc_CfFlowInfo_print_root(void*root){void*_tmp3C1=root;struct Cyc_Absyn_Vardecl*
_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C5;void*_tmp3C6;_LL1C5: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp3C2=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp3C1;if(_tmp3C2->tag != 0)goto
_LL1C7;else{_tmp3C3=_tmp3C2->f1;}}_LL1C6:{const char*_tmp644;void*_tmp643[1];
struct Cyc_String_pa_struct _tmp642;(_tmp642.tag=0,((_tmp642.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3C3->name)),((
_tmp643[0]=& _tmp642,Cyc_fprintf(Cyc_stderr,((_tmp644="Root(%s)",_tag_dyneither(
_tmp644,sizeof(char),9))),_tag_dyneither(_tmp643,sizeof(void*),1)))))));}goto
_LL1C4;_LL1C7: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp3C4=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp3C1;if(_tmp3C4->tag != 1)goto _LL1C9;else{_tmp3C5=_tmp3C4->f1;_tmp3C6=(void*)
_tmp3C4->f2;}}_LL1C8:{const char*_tmp649;void*_tmp648[2];struct Cyc_String_pa_struct
_tmp647;struct Cyc_String_pa_struct _tmp646;(_tmp646.tag=0,((_tmp646.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3C6)),((_tmp647.tag=
0,((_tmp647.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
_tmp3C5)),((_tmp648[0]=& _tmp647,((_tmp648[1]=& _tmp646,Cyc_fprintf(Cyc_stderr,((
_tmp649="MallocPt(%s,%s)",_tag_dyneither(_tmp649,sizeof(char),16))),
_tag_dyneither(_tmp648,sizeof(void*),2)))))))))))));}goto _LL1C4;_LL1C9: {struct
Cyc_CfFlowInfo_InitParam_struct*_tmp3C7=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp3C1;if(_tmp3C7->tag != 2)goto _LL1C4;}_LL1CA:{const char*_tmp64C;void*_tmp64B;(
_tmp64B=0,Cyc_fprintf(Cyc_stderr,((_tmp64C="InitParam(_,_)",_tag_dyneither(
_tmp64C,sizeof(char),15))),_tag_dyneither(_tmp64B,sizeof(void*),0)));}goto _LL1C4;
_LL1C4:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){Cyc_CfFlowInfo_print_root(
p->root);{struct Cyc_List_List*_tmp3D1=p->fields;while(_tmp3D1 != 0){{const char*
_tmp650;void*_tmp64F[1];struct Cyc_Int_pa_struct _tmp64E;(_tmp64E.tag=1,((_tmp64E.f1=(
unsigned long)((int)_tmp3D1->hd),((_tmp64F[0]=& _tmp64E,Cyc_fprintf(Cyc_stderr,((
_tmp650=".%d",_tag_dyneither(_tmp650,sizeof(char),4))),_tag_dyneither(_tmp64F,
sizeof(void*),1)))))));}_tmp3D1=_tmp3D1->tl;}{const char*_tmp653;void*_tmp652;(
_tmp652=0,Cyc_fprintf(Cyc_stderr,((_tmp653=" ",_tag_dyneither(_tmp653,sizeof(
char),2))),_tag_dyneither(_tmp652,sizeof(void*),0)));};};}void Cyc_CfFlowInfo_print_list(
struct Cyc_List_List*x,void(*pr)(void*)){int _tmp3D7=1;while(x != 0){if(!_tmp3D7){{
const char*_tmp656;void*_tmp655;(_tmp655=0,Cyc_fprintf(Cyc_stderr,((_tmp656=", ",
_tag_dyneither(_tmp656,sizeof(char),3))),_tag_dyneither(_tmp655,sizeof(void*),0)));}
_tmp3D7=0;}pr((void*)x->hd);x=x->tl;}{const char*_tmp659;void*_tmp658;(_tmp658=0,
Cyc_fprintf(Cyc_stderr,((_tmp659="\n",_tag_dyneither(_tmp659,sizeof(char),2))),
_tag_dyneither(_tmp658,sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*)){struct _RegionHandle _tmp3DC=_new_region("r");
struct _RegionHandle*r=& _tmp3DC;_push_region(r);{struct _handler_cons _tmp3DD;
_push_handler(& _tmp3DD);{int _tmp3DF=0;if(setjmp(_tmp3DD.handler))_tmp3DF=1;if(!
_tmp3DF){{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple13 _tmp3E0=*((
struct _tuple13*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;{const char*_tmp65C;void*_tmp65B;(_tmp65B=0,Cyc_fprintf(Cyc_stderr,((
_tmp65C="{ ",_tag_dyneither(_tmp65C,sizeof(char),3))),_tag_dyneither(_tmp65B,
sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,struct _tuple13*))Cyc_Iter_next)(
iter,& _tmp3E0)){void*_tmp3E3=_tmp3E0.f1;if(!first){{const char*_tmp65F;void*
_tmp65E;(_tmp65E=0,Cyc_fprintf(Cyc_stderr,((_tmp65F=", ",_tag_dyneither(_tmp65F,
sizeof(char),3))),_tag_dyneither(_tmp65E,sizeof(void*),0)));}first=0;}pr(_tmp3E3);}{
const char*_tmp662;void*_tmp661;(_tmp661=0,Cyc_fprintf(Cyc_stderr,((_tmp662="}\n",
_tag_dyneither(_tmp662,sizeof(char),3))),_tag_dyneither(_tmp661,sizeof(void*),0)));};};
_pop_handler();}else{void*_tmp3DE=(void*)_exn_thrown;void*_tmp3E9=_tmp3DE;_LL1CC:;
_LL1CD:{const char*_tmp665;void*_tmp664;(_tmp664=0,Cyc_fprintf(Cyc_stderr,((
_tmp665="{ }\n",_tag_dyneither(_tmp665,sizeof(char),5))),_tag_dyneither(_tmp664,
sizeof(void*),0)));}_npop_handler(0);return;_LL1CE:;_LL1CF:(void)_throw(_tmp3E9);
_LL1CB:;}};};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval){void*
_tmp3EC=rval;enum Cyc_CfFlowInfo_InitLevel _tmp3F1;enum Cyc_CfFlowInfo_InitLevel
_tmp3F3;struct Cyc_CfFlowInfo_Place*_tmp3F5;struct Cyc_CfFlowInfo_UnionRInfo
_tmp3F7;int _tmp3F8;int _tmp3F9;struct _dyneither_ptr _tmp3FA;struct Cyc_Absyn_Exp*
_tmp3FC;int _tmp3FD;void*_tmp3FE;_LL1D1: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3ED=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3EC;if(_tmp3ED->tag != 0)goto
_LL1D3;}_LL1D2:{const char*_tmp668;void*_tmp667;(_tmp667=0,Cyc_fprintf(Cyc_stderr,((
_tmp668="Zero",_tag_dyneither(_tmp668,sizeof(char),5))),_tag_dyneither(_tmp667,
sizeof(void*),0)));}goto _LL1D0;_LL1D3: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3EE=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3EC;if(_tmp3EE->tag != 1)
goto _LL1D5;}_LL1D4:{const char*_tmp66B;void*_tmp66A;(_tmp66A=0,Cyc_fprintf(Cyc_stderr,((
_tmp66B="NotZeroAll",_tag_dyneither(_tmp66B,sizeof(char),11))),_tag_dyneither(
_tmp66A,sizeof(void*),0)));}goto _LL1D0;_LL1D5: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3EF=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3EC;if(_tmp3EF->tag != 2)
goto _LL1D7;}_LL1D6:{const char*_tmp66E;void*_tmp66D;(_tmp66D=0,Cyc_fprintf(Cyc_stderr,((
_tmp66E="NotZeroThis",_tag_dyneither(_tmp66E,sizeof(char),12))),_tag_dyneither(
_tmp66D,sizeof(void*),0)));}goto _LL1D0;_LL1D7: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3F0=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3EC;if(_tmp3F0->tag != 3)goto
_LL1D9;else{_tmp3F1=_tmp3F0->f1;}}_LL1D8:{const char*_tmp671;void*_tmp670;(
_tmp670=0,Cyc_fprintf(Cyc_stderr,((_tmp671="Unknown(",_tag_dyneither(_tmp671,
sizeof(char),9))),_tag_dyneither(_tmp670,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3F1);{const char*_tmp674;void*_tmp673;(_tmp673=0,Cyc_fprintf(Cyc_stderr,((
_tmp674=")",_tag_dyneither(_tmp674,sizeof(char),2))),_tag_dyneither(_tmp673,
sizeof(void*),0)));}goto _LL1D0;_LL1D9: {struct Cyc_CfFlowInfo_Esc_struct*_tmp3F2=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp3EC;if(_tmp3F2->tag != 4)goto _LL1DB;else{
_tmp3F3=_tmp3F2->f1;}}_LL1DA:{const char*_tmp677;void*_tmp676;(_tmp676=0,Cyc_fprintf(
Cyc_stderr,((_tmp677="Esc(",_tag_dyneither(_tmp677,sizeof(char),5))),
_tag_dyneither(_tmp676,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3F3);{const char*_tmp67A;void*_tmp679;(_tmp679=0,Cyc_fprintf(Cyc_stderr,((
_tmp67A=")",_tag_dyneither(_tmp67A,sizeof(char),2))),_tag_dyneither(_tmp679,
sizeof(void*),0)));}goto _LL1D0;_LL1DB: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3F4=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3EC;if(_tmp3F4->tag != 5)goto
_LL1DD;else{_tmp3F5=_tmp3F4->f1;}}_LL1DC:{const char*_tmp67D;void*_tmp67C;(
_tmp67C=0,Cyc_fprintf(Cyc_stderr,((_tmp67D="AddrOf(",_tag_dyneither(_tmp67D,
sizeof(char),8))),_tag_dyneither(_tmp67C,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(
_tmp3F5);{const char*_tmp680;void*_tmp67F;(_tmp67F=0,Cyc_fprintf(Cyc_stderr,((
_tmp680=")",_tag_dyneither(_tmp680,sizeof(char),2))),_tag_dyneither(_tmp67F,
sizeof(void*),0)));}goto _LL1D0;_LL1DD: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp3F6=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp3EC;if(_tmp3F6->tag != 6)goto
_LL1DF;else{_tmp3F7=_tmp3F6->f1;_tmp3F8=_tmp3F7.is_union;_tmp3F9=_tmp3F7.fieldnum;
_tmp3FA=_tmp3F6->f2;}}_LL1DE: if(_tmp3F8){{const char*_tmp683;void*_tmp682;(
_tmp682=0,Cyc_fprintf(Cyc_stderr,((_tmp683="AggrUnion{",_tag_dyneither(_tmp683,
sizeof(char),11))),_tag_dyneither(_tmp682,sizeof(void*),0)));}if(_tmp3F9 != - 1){
const char*_tmp687;void*_tmp686[1];struct Cyc_Int_pa_struct _tmp685;(_tmp685.tag=1,((
_tmp685.f1=(unsigned long)_tmp3F9,((_tmp686[0]=& _tmp685,Cyc_fprintf(Cyc_stderr,((
_tmp687="tag == %d;",_tag_dyneither(_tmp687,sizeof(char),11))),_tag_dyneither(
_tmp686,sizeof(void*),1)))))));}}else{const char*_tmp68A;void*_tmp689;(_tmp689=0,
Cyc_fprintf(Cyc_stderr,((_tmp68A="AggrStruct{",_tag_dyneither(_tmp68A,sizeof(
char),12))),_tag_dyneither(_tmp689,sizeof(void*),0)));}{int i=0;for(0;i < 
_get_dyneither_size(_tmp3FA,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp3FA.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp3FA,sizeof(void*))){
const char*_tmp68D;void*_tmp68C;(_tmp68C=0,Cyc_fprintf(Cyc_stderr,((_tmp68D=",",
_tag_dyneither(_tmp68D,sizeof(char),2))),_tag_dyneither(_tmp68C,sizeof(void*),0)));}}}{
const char*_tmp690;void*_tmp68F;(_tmp68F=0,Cyc_fprintf(Cyc_stderr,((_tmp690="}",
_tag_dyneither(_tmp690,sizeof(char),2))),_tag_dyneither(_tmp68F,sizeof(void*),0)));}
goto _LL1D0;_LL1DF: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp3FB=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp3EC;if(_tmp3FB->tag != 7)goto _LL1D0;else{_tmp3FC=_tmp3FB->f1;_tmp3FD=_tmp3FB->f2;
_tmp3FE=(void*)_tmp3FB->f3;}}_LL1E0:{const char*_tmp695;void*_tmp694[2];struct Cyc_String_pa_struct
_tmp693;struct Cyc_Int_pa_struct _tmp692;(_tmp692.tag=1,((_tmp692.f1=(
unsigned long)_tmp3FD,((_tmp693.tag=0,((_tmp693.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp3FC)),((_tmp694[0]=& _tmp693,((
_tmp694[1]=& _tmp692,Cyc_fprintf(Cyc_stderr,((_tmp695="[Consumed(%s,%d,",
_tag_dyneither(_tmp695,sizeof(char),17))),_tag_dyneither(_tmp694,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp3FE);{const char*_tmp698;void*_tmp697;(_tmp697=0,
Cyc_fprintf(Cyc_stderr,((_tmp698="]",_tag_dyneither(_tmp698,sizeof(char),2))),
_tag_dyneither(_tmp697,sizeof(void*),0)));};_LL1D0:;}static void Cyc_CfFlowInfo_print_flow_mapping(
void*root,void*rval){{const char*_tmp69B;void*_tmp69A;(_tmp69A=0,Cyc_fprintf(Cyc_stderr,((
_tmp69B="    ",_tag_dyneither(_tmp69B,sizeof(char),5))),_tag_dyneither(_tmp69A,
sizeof(void*),0)));}Cyc_CfFlowInfo_print_root(root);{const char*_tmp69E;void*
_tmp69D;(_tmp69D=0,Cyc_fprintf(Cyc_stderr,((_tmp69E=" --> ",_tag_dyneither(
_tmp69E,sizeof(char),6))),_tag_dyneither(_tmp69D,sizeof(void*),0)));}Cyc_CfFlowInfo_print_absrval(
rval);{const char*_tmp6A1;void*_tmp6A0;(_tmp6A0=0,Cyc_fprintf(Cyc_stderr,((
_tmp6A1="\n",_tag_dyneither(_tmp6A1,sizeof(char),2))),_tag_dyneither(_tmp6A0,
sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){((
void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,
d);}void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo
_tmp428=f;int _tmp429;struct _tuple11 _tmp42A;struct Cyc_Dict_Dict _tmp42B;_LL1E2: if((
_tmp428.BottomFL).tag != 1)goto _LL1E4;_tmp429=(int)(_tmp428.BottomFL).val;_LL1E3:{
const char*_tmp6A4;void*_tmp6A3;(_tmp6A3=0,Cyc_fprintf(Cyc_stderr,((_tmp6A4="  BottomFL\n",
_tag_dyneither(_tmp6A4,sizeof(char),12))),_tag_dyneither(_tmp6A3,sizeof(void*),0)));}
goto _LL1E1;_LL1E4: if((_tmp428.ReachableFL).tag != 2)goto _LL1E1;_tmp42A=(struct
_tuple11)(_tmp428.ReachableFL).val;_tmp42B=_tmp42A.f1;_LL1E5:{const char*_tmp6A7;
void*_tmp6A6;(_tmp6A6=0,Cyc_fprintf(Cyc_stderr,((_tmp6A7="  ReachableFL:\n",
_tag_dyneither(_tmp6A7,sizeof(char),16))),_tag_dyneither(_tmp6A6,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp42B);{const char*_tmp6AA;void*_tmp6A9;(_tmp6A9=
0,Cyc_fprintf(Cyc_stderr,((_tmp6AA="\n",_tag_dyneither(_tmp6AA,sizeof(char),2))),
_tag_dyneither(_tmp6A9,sizeof(void*),0)));}goto _LL1E1;_LL1E1:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t){void*_tmp432=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp43E;struct Cyc_Absyn_DatatypeInfo _tmp440;struct Cyc_List_List*_tmp441;struct
Cyc_List_List*_tmp443;struct Cyc_Absyn_AggrInfo _tmp445;struct Cyc_List_List*
_tmp446;struct Cyc_Absyn_DatatypeFieldInfo _tmp448;struct Cyc_List_List*_tmp449;
struct Cyc_List_List*_tmp44B;struct Cyc_Absyn_PtrInfo _tmp44D;void*_tmp44E;struct
Cyc_Absyn_PtrAtts _tmp44F;void*_tmp450;struct Cyc_List_List*_tmp453;struct Cyc_List_List*
_tmp455;struct Cyc_Absyn_ArrayInfo _tmp457;void*_tmp458;void*_tmp45A;void*_tmp45C;
void*_tmp45E;void*_tmp45F;void*_tmp461;_LL1E7: {struct Cyc_Absyn_VoidType_struct*
_tmp433=(struct Cyc_Absyn_VoidType_struct*)_tmp432;if(_tmp433->tag != 0)goto _LL1E9;}
_LL1E8: goto _LL1EA;_LL1E9: {struct Cyc_Absyn_IntType_struct*_tmp434=(struct Cyc_Absyn_IntType_struct*)
_tmp432;if(_tmp434->tag != 6)goto _LL1EB;}_LL1EA: goto _LL1EC;_LL1EB: {struct Cyc_Absyn_FloatType_struct*
_tmp435=(struct Cyc_Absyn_FloatType_struct*)_tmp432;if(_tmp435->tag != 7)goto
_LL1ED;}_LL1EC: goto _LL1EE;_LL1ED: {struct Cyc_Absyn_DoubleType_struct*_tmp436=(
struct Cyc_Absyn_DoubleType_struct*)_tmp432;if(_tmp436->tag != 8)goto _LL1EF;}
_LL1EE: goto _LL1F0;_LL1EF: {struct Cyc_Absyn_EnumType_struct*_tmp437=(struct Cyc_Absyn_EnumType_struct*)
_tmp432;if(_tmp437->tag != 14)goto _LL1F1;}_LL1F0: goto _LL1F2;_LL1F1: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp438=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp432;if(_tmp438->tag != 15)goto
_LL1F3;}_LL1F2: goto _LL1F4;_LL1F3: {struct Cyc_Absyn_TagType_struct*_tmp439=(
struct Cyc_Absyn_TagType_struct*)_tmp432;if(_tmp439->tag != 20)goto _LL1F5;}_LL1F4:
goto _LL1F6;_LL1F5: {struct Cyc_Absyn_HeapRgn_struct*_tmp43A=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp432;if(_tmp43A->tag != 21)goto _LL1F7;}_LL1F6: goto _LL1F8;_LL1F7: {struct Cyc_Absyn_Evar_struct*
_tmp43B=(struct Cyc_Absyn_Evar_struct*)_tmp432;if(_tmp43B->tag != 1)goto _LL1F9;}
_LL1F8: return 0;_LL1F9: {struct Cyc_Absyn_UniqueRgn_struct*_tmp43C=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp432;if(_tmp43C->tag != 22)goto _LL1FB;}_LL1FA: {void*_tmp463=rgn;_LL21C: {
struct Cyc_CfFlowInfo_UniqueRgn_k_struct*_tmp464=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)
_tmp463;if(_tmp464->tag != 0)goto _LL21E;}_LL21D: return 1;_LL21E:;_LL21F: return 0;
_LL21B:;}_LL1FB: {struct Cyc_Absyn_VarType_struct*_tmp43D=(struct Cyc_Absyn_VarType_struct*)
_tmp432;if(_tmp43D->tag != 2)goto _LL1FD;else{_tmp43E=_tmp43D->f1;}}_LL1FC: {void*
_tmp465=rgn;struct Cyc_Absyn_Tvar*_tmp468;_LL221: {struct Cyc_CfFlowInfo_UniqueRgn_k_struct*
_tmp466=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)_tmp465;if(_tmp466->tag != 0)
goto _LL223;}_LL222: return 0;_LL223: {struct Cyc_CfFlowInfo_Region_k_struct*_tmp467=(
struct Cyc_CfFlowInfo_Region_k_struct*)_tmp465;if(_tmp467->tag != 1)goto _LL220;
else{_tmp468=_tmp467->f1;}}_LL224: return Cyc_Absyn_tvar_cmp(_tmp43E,_tmp468)== 0;
_LL220:;}_LL1FD: {struct Cyc_Absyn_DatatypeType_struct*_tmp43F=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp432;if(_tmp43F->tag != 3)goto _LL1FF;else{_tmp440=_tmp43F->f1;_tmp441=_tmp440.targs;}}
_LL1FE: _tmp443=_tmp441;goto _LL200;_LL1FF: {struct Cyc_Absyn_TypedefType_struct*
_tmp442=(struct Cyc_Absyn_TypedefType_struct*)_tmp432;if(_tmp442->tag != 18)goto
_LL201;else{_tmp443=_tmp442->f2;}}_LL200: _tmp446=_tmp443;goto _LL202;_LL201: {
struct Cyc_Absyn_AggrType_struct*_tmp444=(struct Cyc_Absyn_AggrType_struct*)
_tmp432;if(_tmp444->tag != 12)goto _LL203;else{_tmp445=_tmp444->f1;_tmp446=_tmp445.targs;}}
_LL202: _tmp449=_tmp446;goto _LL204;_LL203: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp447=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp432;if(_tmp447->tag != 4)
goto _LL205;else{_tmp448=_tmp447->f1;_tmp449=_tmp448.targs;}}_LL204: _tmp44B=
_tmp449;goto _LL206;_LL205: {struct Cyc_Absyn_JoinEff_struct*_tmp44A=(struct Cyc_Absyn_JoinEff_struct*)
_tmp432;if(_tmp44A->tag != 24)goto _LL207;else{_tmp44B=_tmp44A->f1;}}_LL206: return((
int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(
Cyc_CfFlowInfo_contains_region,rgn,_tmp44B);_LL207: {struct Cyc_Absyn_PointerType_struct*
_tmp44C=(struct Cyc_Absyn_PointerType_struct*)_tmp432;if(_tmp44C->tag != 5)goto
_LL209;else{_tmp44D=_tmp44C->f1;_tmp44E=_tmp44D.elt_typ;_tmp44F=_tmp44D.ptr_atts;
_tmp450=_tmp44F.rgn;}}_LL208: return Cyc_CfFlowInfo_contains_region(rgn,_tmp450)
 || Cyc_CfFlowInfo_contains_region(rgn,_tmp44E);_LL209: {struct Cyc_Absyn_FnType_struct*
_tmp451=(struct Cyc_Absyn_FnType_struct*)_tmp432;if(_tmp451->tag != 10)goto _LL20B;}
_LL20A: return 0;_LL20B: {struct Cyc_Absyn_TupleType_struct*_tmp452=(struct Cyc_Absyn_TupleType_struct*)
_tmp432;if(_tmp452->tag != 11)goto _LL20D;else{_tmp453=_tmp452->f1;}}_LL20C: for(0;
_tmp453 != 0;_tmp453=_tmp453->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((
struct _tuple15*)_tmp453->hd)).f2))return 1;}return 0;_LL20D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp454=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp432;if(_tmp454->tag != 13)goto
_LL20F;else{_tmp455=_tmp454->f2;}}_LL20E: for(0;_tmp455 != 0;_tmp455=_tmp455->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp455->hd)->type))
return 1;}return 0;_LL20F: {struct Cyc_Absyn_ArrayType_struct*_tmp456=(struct Cyc_Absyn_ArrayType_struct*)
_tmp432;if(_tmp456->tag != 9)goto _LL211;else{_tmp457=_tmp456->f1;_tmp458=_tmp457.elt_type;}}
_LL210: _tmp45A=_tmp458;goto _LL212;_LL211: {struct Cyc_Absyn_AccessEff_struct*
_tmp459=(struct Cyc_Absyn_AccessEff_struct*)_tmp432;if(_tmp459->tag != 23)goto
_LL213;else{_tmp45A=(void*)_tmp459->f1;}}_LL212: _tmp45C=_tmp45A;goto _LL214;
_LL213: {struct Cyc_Absyn_RgnsEff_struct*_tmp45B=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp432;if(_tmp45B->tag != 25)goto _LL215;else{_tmp45C=(void*)_tmp45B->f1;}}_LL214:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp45C);_LL215: {struct Cyc_Absyn_DynRgnType_struct*
_tmp45D=(struct Cyc_Absyn_DynRgnType_struct*)_tmp432;if(_tmp45D->tag != 17)goto
_LL217;else{_tmp45E=(void*)_tmp45D->f1;_tmp45F=(void*)_tmp45D->f2;}}_LL216:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp45E) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp45F);_LL217: {struct Cyc_Absyn_RgnHandleType_struct*_tmp460=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp432;if(_tmp460->tag != 16)goto _LL219;else{_tmp461=(void*)_tmp460->f1;}}_LL218:
return 0;_LL219: {struct Cyc_Absyn_ValueofType_struct*_tmp462=(struct Cyc_Absyn_ValueofType_struct*)
_tmp432;if(_tmp462->tag != 19)goto _LL1E6;}_LL21A: return 0;_LL1E6:;}struct _tuple23{
struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(
struct _tuple23*env,void*root,void*rval){struct _tuple23 _tmp46A;struct Cyc_CfFlowInfo_FlowEnv*
_tmp46B;struct Cyc_Dict_Dict _tmp46C;struct Cyc_Dict_Dict*_tmp46D;void*_tmp46E;
struct _tuple23*_tmp469=env;_tmp46A=*_tmp469;_tmp46B=_tmp46A.f1;_tmp46C=_tmp46A.f2;
_tmp46D=(struct Cyc_Dict_Dict*)&(*_tmp469).f2;_tmp46E=_tmp46A.f3;{void*_tmp46F=
root;struct Cyc_Absyn_Vardecl*_tmp471;void*_tmp473;_LL226: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp470=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp46F;if(_tmp470->tag != 0)goto
_LL228;else{_tmp471=_tmp470->f1;}}_LL227: if(Cyc_CfFlowInfo_contains_region(
_tmp46E,_tmp471->type))rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp46B,_tmp471->type,
0,_tmp46B->unknown_none);*_tmp46D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(*_tmp46D,root,rval);goto _LL225;_LL228: {struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp472=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp46F;if(_tmp472->tag != 1)goto _LL22A;else{_tmp473=(void*)_tmp472->f2;}}_LL229:
if(!Cyc_CfFlowInfo_contains_region(_tmp46E,_tmp473))*_tmp46D=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp46D,root,rval);goto
_LL225;_LL22A: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp474=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp46F;if(_tmp474->tag != 2)goto _LL225;}_LL22B: goto _LL225;_LL225:;};}static
struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp475=Cyc_Tcutil_compress(rgn);
struct Cyc_Absyn_Tvar*_tmp477;_LL22D: {struct Cyc_Absyn_VarType_struct*_tmp476=(
struct Cyc_Absyn_VarType_struct*)_tmp475;if(_tmp476->tag != 2)goto _LL22F;else{
_tmp477=_tmp476->f1;}}_LL22E: {struct Cyc_CfFlowInfo_Region_k_struct _tmp6AB;
struct Cyc_CfFlowInfo_Region_k_struct v=(_tmp6AB.tag=1,((_tmp6AB.f1=_tmp477,
_tmp6AB)));void*v2=(void*)& v;struct _tuple23 _tmp6AC;struct _tuple23 env=(_tmp6AC.f1=
fenv,((_tmp6AC.f2=fenv->mt_flowdict,((_tmp6AC.f3=v2,_tmp6AC)))));((void(*)(void(*
f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL22F: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp478=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp475;if(_tmp478->tag != 22)goto
_LL231;}_LL230: {struct _tuple23 _tmp6AD;struct _tuple23 env=(_tmp6AD.f1=fenv,((
_tmp6AD.f2=fenv->mt_flowdict,((_tmp6AD.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,
_tmp6AD)))));((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return
env.f2;}_LL231:;_LL232: {struct Cyc_Core_Impossible_struct _tmp6B3;const char*
_tmp6B2;struct Cyc_Core_Impossible_struct*_tmp6B1;(int)_throw((void*)((_tmp6B1=
_cycalloc(sizeof(*_tmp6B1)),((_tmp6B1[0]=((_tmp6B3.tag=Cyc_Core_Impossible,((
_tmp6B3.f1=((_tmp6B2="kill_flowdict_region",_tag_dyneither(_tmp6B2,sizeof(char),
21))),_tmp6B3)))),_tmp6B1)))));}_LL22C:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){union
Cyc_CfFlowInfo_FlowInfo _tmp47F=f;int _tmp480;struct _tuple11 _tmp481;struct Cyc_Dict_Dict
_tmp482;struct Cyc_List_List*_tmp483;_LL234: if((_tmp47F.BottomFL).tag != 1)goto
_LL236;_tmp480=(int)(_tmp47F.BottomFL).val;_LL235: return f;_LL236: if((_tmp47F.ReachableFL).tag
!= 2)goto _LL233;_tmp481=(struct _tuple11)(_tmp47F.ReachableFL).val;_tmp482=
_tmp481.f1;_tmp483=_tmp481.f2;_LL237: {struct Cyc_Dict_Dict _tmp484=Cyc_CfFlowInfo_kill_flowdict_region(
fenv,_tmp482,rgn);struct Cyc_List_List*_tmp485=Cyc_CfFlowInfo_kill_relns_region(
_tmp483,rgn);return Cyc_CfFlowInfo_ReachableFL(_tmp484,_tmp485);}_LL233:;}
