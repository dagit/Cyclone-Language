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
Cyc_CfFlowInfo_AbsLVal _tmp47E;return((_tmp47E.PlaceL).val=x,(((_tmp47E.PlaceL).tag=
1,_tmp47E)));}union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){union Cyc_CfFlowInfo_AbsLVal
_tmp47F;return((_tmp47F.UnknownL).val=0,(((_tmp47F.UnknownL).tag=2,_tmp47F)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo
_tmp480;return((_tmp480.BottomFL).val=0,(((_tmp480.BottomFL).tag=1,_tmp480)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,
struct Cyc_List_List*r){struct _tuple11 _tmp483;union Cyc_CfFlowInfo_FlowInfo _tmp482;
return((_tmp482.ReachableFL).val=((_tmp483.f1=fd,((_tmp483.f2=r,_tmp483)))),(((
_tmp482.ReachableFL).tag=2,_tmp482)));}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,{.Null_c={1,0}}};static
struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp4BA;struct Cyc_CfFlowInfo_MallocPt_struct _tmp4B9;
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp4B8;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4B7;struct Cyc_CfFlowInfo_Esc_struct _tmp4B6;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4B5;struct Cyc_CfFlowInfo_Esc_struct _tmp4B4;struct Cyc_CfFlowInfo_Esc_struct*
_tmp4B3;struct Cyc_CfFlowInfo_Esc_struct _tmp4B2;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4B1;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4B0;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4AF;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4AE;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4AD;struct Cyc_CfFlowInfo_UnknownR_struct _tmp4AC;struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4AB;struct Cyc_CfFlowInfo_NotZeroThis_struct _tmp4AA;struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4A9;struct Cyc_CfFlowInfo_NotZeroAll_struct _tmp4A8;struct Cyc_CfFlowInfo_Zero_struct*
_tmp4A7;struct Cyc_CfFlowInfo_Zero_struct _tmp4A6;struct Cyc_CfFlowInfo_FlowEnv*
_tmp4A5;return(_tmp4A5=_region_malloc(r,sizeof(*_tmp4A5)),((_tmp4A5->r=r,((
_tmp4A5->zero=(void*)((_tmp4A7=_region_malloc(r,sizeof(*_tmp4A7)),((_tmp4A7[0]=((
_tmp4A6.tag=0,_tmp4A6)),_tmp4A7)))),((_tmp4A5->notzeroall=(void*)((_tmp4A9=
_region_malloc(r,sizeof(*_tmp4A9)),((_tmp4A9[0]=((_tmp4A8.tag=1,_tmp4A8)),
_tmp4A9)))),((_tmp4A5->notzerothis=(void*)((_tmp4AB=_region_malloc(r,sizeof(*
_tmp4AB)),((_tmp4AB[0]=((_tmp4AA.tag=2,_tmp4AA)),_tmp4AB)))),((_tmp4A5->unknown_none=(
void*)((_tmp4AD=_region_malloc(r,sizeof(*_tmp4AD)),((_tmp4AD[0]=((_tmp4AC.tag=3,((
_tmp4AC.f1=Cyc_CfFlowInfo_NoneIL,_tmp4AC)))),_tmp4AD)))),((_tmp4A5->unknown_this=(
void*)((_tmp4AF=_region_malloc(r,sizeof(*_tmp4AF)),((_tmp4AF[0]=((_tmp4AE.tag=3,((
_tmp4AE.f1=Cyc_CfFlowInfo_ThisIL,_tmp4AE)))),_tmp4AF)))),((_tmp4A5->unknown_all=(
void*)((_tmp4B1=_region_malloc(r,sizeof(*_tmp4B1)),((_tmp4B1[0]=((_tmp4B0.tag=3,((
_tmp4B0.f1=Cyc_CfFlowInfo_AllIL,_tmp4B0)))),_tmp4B1)))),((_tmp4A5->esc_none=(
void*)((_tmp4B3=_region_malloc(r,sizeof(*_tmp4B3)),((_tmp4B3[0]=((_tmp4B2.tag=4,((
_tmp4B2.f1=Cyc_CfFlowInfo_NoneIL,_tmp4B2)))),_tmp4B3)))),((_tmp4A5->esc_this=(
void*)((_tmp4B5=_region_malloc(r,sizeof(*_tmp4B5)),((_tmp4B5[0]=((_tmp4B4.tag=4,((
_tmp4B4.f1=Cyc_CfFlowInfo_ThisIL,_tmp4B4)))),_tmp4B5)))),((_tmp4A5->esc_all=(
void*)((_tmp4B7=_region_malloc(r,sizeof(*_tmp4B7)),((_tmp4B7[0]=((_tmp4B6.tag=4,((
_tmp4B6.f1=Cyc_CfFlowInfo_AllIL,_tmp4B6)))),_tmp4B7)))),((_tmp4A5->mt_flowdict=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(
r,Cyc_CfFlowInfo_root_cmp),((_tmp4A5->mt_place_set=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp4A5->dummy_place=((_tmp4BA=
_region_malloc(r,sizeof(*_tmp4BA)),((_tmp4BA->root=(void*)((_tmp4B8=
_region_malloc(r,sizeof(*_tmp4B8)),((_tmp4B8[0]=((_tmp4B9.tag=1,((_tmp4B9.f1=&
dummy_exp,((_tmp4B9.f2=(void*)((void*)& Cyc_Absyn_VoidType_val),_tmp4B9)))))),
_tmp4B8)))),((_tmp4BA->fields=0,_tmp4BA)))))),_tmp4A5)))))))))))))))))))))))))));}
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*
loc){if(set != 0){if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,
place))return 1;else{*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,
struct Cyc_Position_Segment*v))Cyc_Dict_insert)(*set,place,loc);}}return 0;}struct
_dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
void*_tmp1F;struct Cyc_List_List*_tmp20;struct Cyc_CfFlowInfo_Place _tmp1E=*place;
_tmp1F=_tmp1E.root;_tmp20=_tmp1E.fields;{void*_tmp21=_tmp1F;struct Cyc_Absyn_Vardecl*
_tmp23;_LL1: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp22=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp21;if(_tmp22->tag != 0)goto _LL3;else{_tmp23=_tmp22->f1;}}_LL2: if(_tmp20 == 0)
return Cyc_Absynpp_qvar2string(_tmp23->name);else{const char*_tmp4BE;void*_tmp4BD[
1];struct Cyc_String_pa_struct _tmp4BC;return(struct _dyneither_ptr)((_tmp4BC.tag=0,((
_tmp4BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp23->name)),((_tmp4BD[0]=& _tmp4BC,Cyc_aprintf(((_tmp4BE="reachable from %s",
_tag_dyneither(_tmp4BE,sizeof(char),18))),_tag_dyneither(_tmp4BD,sizeof(void*),1))))))));}
_LL3:;_LL4: {const char*_tmp4C1;void*_tmp4C0;(_tmp4C0=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4C1="error locations not for VarRoots",
_tag_dyneither(_tmp4C1,sizeof(char),33))),_tag_dyneither(_tmp4C0,sizeof(void*),0)));}
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
return n;n=n + 1;}{struct Cyc_Core_Impossible_struct _tmp4CE;const char*_tmp4CD;void*
_tmp4CC[1];struct Cyc_String_pa_struct _tmp4CB;struct Cyc_Core_Impossible_struct*
_tmp4CA;(int)_throw((void*)((_tmp4CA=_cycalloc(sizeof(*_tmp4CA)),((_tmp4CA[0]=((
_tmp4CE.tag=Cyc_Core_Impossible,((_tmp4CE.f1=(struct _dyneither_ptr)((_tmp4CB.tag=
0,((_tmp4CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp4CC[0]=&
_tmp4CB,Cyc_aprintf(((_tmp4CD="get_field_index_fs failed: %s",_tag_dyneither(
_tmp4CD,sizeof(char),30))),_tag_dyneither(_tmp4CC,sizeof(void*),1)))))))),
_tmp4CE)))),_tmp4CA)))));};}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f){void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp3D;union Cyc_Absyn_AggrInfoU _tmp3E;struct Cyc_List_List*_tmp40;_LLB: {struct
Cyc_Absyn_AggrType_struct*_tmp3C=(struct Cyc_Absyn_AggrType_struct*)_tmp3B;if(
_tmp3C->tag != 12)goto _LLD;else{_tmp3D=_tmp3C->f1;_tmp3E=_tmp3D.aggr_info;}}_LLC: {
struct Cyc_List_List*_tmp41=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(
_tmp3E))->impl))->fields;_tmp40=_tmp41;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp3F=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp3B;if(_tmp3F->tag != 13)goto _LLF;
else{_tmp40=_tmp3F->f2;}}_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp40,f);
_LLF:;_LL10: {struct Cyc_Core_Impossible_struct _tmp4DB;const char*_tmp4DA;void*
_tmp4D9[1];struct Cyc_String_pa_struct _tmp4D8;struct Cyc_Core_Impossible_struct*
_tmp4D7;(int)_throw((void*)((_tmp4D7=_cycalloc(sizeof(*_tmp4D7)),((_tmp4D7[0]=((
_tmp4DB.tag=Cyc_Core_Impossible,((_tmp4DB.f1=(struct _dyneither_ptr)((_tmp4D8.tag=
0,((_tmp4D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp4D9[0]=& _tmp4D8,Cyc_aprintf(((_tmp4DA="get_field_index failed: %s",
_tag_dyneither(_tmp4DA,sizeof(char),27))),_tag_dyneither(_tmp4D9,sizeof(void*),1)))))))),
_tmp4DB)))),_tmp4D7)))));}_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((
int)r1 == (int)r2)return 0;{struct _tuple0 _tmp4DC;struct _tuple0 _tmp48=(_tmp4DC.f1=
r1,((_tmp4DC.f2=r2,_tmp4DC)));void*_tmp49;struct Cyc_Absyn_Vardecl*_tmp4B;void*
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
else{_tmp6E=_tmp6D->f1;}}_LL22:{struct _dyneither_ptr*_tmp4E9;const char*_tmp4E8;
void*_tmp4E7[1];struct Cyc_String_pa_struct _tmp4E6;struct Cyc_List_List*_tmp4E5;sl=((
_tmp4E5=_cycalloc(sizeof(*_tmp4E5)),((_tmp4E5->hd=((_tmp4E9=_cycalloc(sizeof(*
_tmp4E9)),((_tmp4E9[0]=((_tmp4E6.tag=0,((_tmp4E6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp6E->name).f2),((_tmp4E7[0]=& _tmp4E6,Cyc_aprintf(((
_tmp4E8="%s",_tag_dyneither(_tmp4E8,sizeof(char),3))),_tag_dyneither(_tmp4E7,
sizeof(void*),1)))))))),_tmp4E9)))),((_tmp4E5->tl=sl,_tmp4E5))))));}goto _LL20;
_LL23: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp6F=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp6C;if(_tmp6F->tag != 1)goto _LL25;else{_tmp70=_tmp6F->f1;}}_LL24:{struct
_dyneither_ptr*_tmp4F6;const char*_tmp4F5;void*_tmp4F4[1];struct Cyc_Int_pa_struct
_tmp4F3;struct Cyc_List_List*_tmp4F2;sl=((_tmp4F2=_cycalloc(sizeof(*_tmp4F2)),((
_tmp4F2->hd=((_tmp4F6=_cycalloc(sizeof(*_tmp4F6)),((_tmp4F6[0]=((_tmp4F3.tag=1,((
_tmp4F3.f1=(unsigned long)((int)_tmp70),((_tmp4F4[0]=& _tmp4F3,Cyc_aprintf(((
_tmp4F5="mpt%d",_tag_dyneither(_tmp4F5,sizeof(char),6))),_tag_dyneither(_tmp4F4,
sizeof(void*),1)))))))),_tmp4F6)))),((_tmp4F2->tl=sl,_tmp4F2))))));}goto _LL20;
_LL25: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp71=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp6C;if(_tmp71->tag != 2)goto _LL20;else{_tmp72=_tmp71->f1;}}_LL26:{struct
_dyneither_ptr*_tmp503;const char*_tmp502;void*_tmp501[1];struct Cyc_Int_pa_struct
_tmp500;struct Cyc_List_List*_tmp4FF;sl=((_tmp4FF=_cycalloc(sizeof(*_tmp4FF)),((
_tmp4FF->hd=((_tmp503=_cycalloc(sizeof(*_tmp503)),((_tmp503[0]=((_tmp500.tag=1,((
_tmp500.f1=(unsigned long)_tmp72,((_tmp501[0]=& _tmp500,Cyc_aprintf(((_tmp502="param%d",
_tag_dyneither(_tmp502,sizeof(char),8))),_tag_dyneither(_tmp501,sizeof(void*),1)))))))),
_tmp503)))),((_tmp4FF->tl=sl,_tmp4FF))))));}goto _LL20;_LL20:;}for(0;_tmp6B != 0;
_tmp6B=_tmp6B->tl){struct _dyneither_ptr*_tmp510;const char*_tmp50F;void*_tmp50E[1];
struct Cyc_Int_pa_struct _tmp50D;struct Cyc_List_List*_tmp50C;sl=((_tmp50C=
_cycalloc(sizeof(*_tmp50C)),((_tmp50C->hd=((_tmp510=_cycalloc(sizeof(*_tmp510)),((
_tmp510[0]=((_tmp50D.tag=1,((_tmp50D.f1=(unsigned long)((int)_tmp6B->hd),((
_tmp50E[0]=& _tmp50D,Cyc_aprintf(((_tmp50F="%d",_tag_dyneither(_tmp50F,sizeof(
char),3))),_tag_dyneither(_tmp50E,sizeof(void*),1)))))))),_tmp510)))),((_tmp50C->tl=
sl,_tmp50C))))));}{struct Cyc_String_pa_struct _tmp51B;const char*_tmp51A;void*
_tmp519[1];const char*_tmp518;struct _dyneither_ptr*_tmp517;struct _dyneither_ptr*
_tmp87=(_tmp517=_cycalloc(sizeof(*_tmp517)),((_tmp517[0]=((_tmp51B.tag=0,((
_tmp51B.f1=(struct _dyneither_ptr)((_tmp51A="",_tag_dyneither(_tmp51A,sizeof(char),
1))),((_tmp519[0]=& _tmp51B,Cyc_aprintf(((_tmp518="%s",_tag_dyneither(_tmp518,
sizeof(char),3))),_tag_dyneither(_tmp519,sizeof(void*),1)))))))),_tmp517)));for(
0;sl != 0;sl=sl->tl){const char*_tmp520;void*_tmp51F[2];struct Cyc_String_pa_struct
_tmp51E;struct Cyc_String_pa_struct _tmp51D;*_tmp87=((_tmp51D.tag=0,((_tmp51D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp87),((_tmp51E.tag=0,((_tmp51E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((
_tmp51F[0]=& _tmp51E,((_tmp51F[1]=& _tmp51D,Cyc_aprintf(((_tmp520="%s.%s",
_tag_dyneither(_tmp520,sizeof(char),6))),_tag_dyneither(_tmp51F,sizeof(void*),2))))))))))))));}
return _tmp87;};}static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct
_dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp526(
struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp525,unsigned int*_tmp524,
void***_tmp522){for(*_tmp525=0;*_tmp525 < *_tmp524;(*_tmp525)++){(*_tmp522)[*
_tmp525]=(*fenv)->unknown_all;}}struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,
void*leafval){unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
fs);unsigned int _tmp525;unsigned int _tmp524;struct _dyneither_ptr _tmp523;void**
_tmp522;unsigned int _tmp521;struct _dyneither_ptr d=(_tmp521=sz,((_tmp522=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp521)),((_tmp523=
_tag_dyneither(_tmp522,sizeof(void*),_tmp521),((((_tmp524=_tmp521,_tmp526(& fenv,&
_tmp525,& _tmp524,& _tmp522))),_tmp523)))))));{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
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
_tmp9B,_tmp9C,f->type);struct Cyc_Absyn_Aggrfield*_tmp527;return(_tmp527=
_region_malloc(_tmp9B,sizeof(*_tmp527)),((_tmp527->name=f->name,((_tmp527->tq=f->tq,((
_tmp527->type=_tmp9D,((_tmp527->width=f->width,((_tmp527->attributes=f->attributes,
_tmp527)))))))))));};}static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*
targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){struct
_RegionHandle _tmp9F=_new_region("temp");struct _RegionHandle*temp=& _tmp9F;
_push_region(temp);{struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,tvs,targs);struct _tuple14 _tmp528;struct _tuple14
_tmpA0=(_tmp528.f1=temp,((_tmp528.f2=inst,_tmp528)));struct Cyc_List_List*subs_fs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(
struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmpA0,fs);struct
_dyneither_ptr _tmpA1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,
no_init_bits_only,leafval);_npop_handler(0);return _tmpA1;};_pop_region(temp);}
struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,
void*leafval);static void _tmp52E(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*
_tmp52D,unsigned int*_tmp52C,void***_tmp52A){for(*_tmp52D=0;*_tmp52D < *_tmp52C;(*
_tmp52D)++){(*_tmp52A)[*_tmp52D]=(*fenv)->unknown_all;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
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
_tmpAA);unsigned int _tmp52D;unsigned int _tmp52C;struct _dyneither_ptr _tmp52B;void**
_tmp52A;unsigned int _tmp529;struct _dyneither_ptr d=(_tmp529=sz,((_tmp52A=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp529)),((_tmp52B=
_tag_dyneither(_tmp52A,sizeof(void*),_tmp529),((((_tmp52C=_tmp529,_tmp52E(& fenv,&
_tmp52D,& _tmp52C,& _tmp52A))),_tmp52B)))))));{int i=0;for(0;i < sz;++ i){*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,no_init_bits_only,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(
_tmpAA))->hd)).f2,leafval);_tmpAA=_tmpAA->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp534;struct Cyc_CfFlowInfo_UnionRInfo _tmp533;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp532;return(void*)((_tmp532=_region_malloc(fenv->r,sizeof(*_tmp532)),((
_tmp532[0]=((_tmp534.tag=6,((_tmp534.f1=((_tmp533.is_union=0,((_tmp533.fieldnum=
- 1,_tmp533)))),((_tmp534.f2=d,_tmp534)))))),_tmp532))));};}_LL2C: {struct Cyc_Absyn_AggrType_struct*
_tmpAB=(struct Cyc_Absyn_AggrType_struct*)_tmpA3;if(_tmpAB->tag != 12)goto _LL2E;
else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAC.aggr_info;_tmpAE=_tmpAC.targs;}}_LL2D: {
struct Cyc_Absyn_Aggrdecl*_tmpC3=Cyc_Absyn_get_known_aggrdecl(_tmpAD);if(_tmpC3->impl
== 0)goto _LL27;{struct Cyc_List_List*_tmpC4=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpC3->impl))->fields;if(_tmpAE == 0){_tmpB0=_tmpC3->kind;_tmpB1=
_tmpC4;goto _LL2F;}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp53A;struct Cyc_CfFlowInfo_UnionRInfo
_tmp539;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp538;return(void*)((_tmp538=
_region_malloc(fenv->r,sizeof(*_tmp538)),((_tmp538[0]=((_tmp53A.tag=6,((_tmp53A.f1=((
_tmp539.is_union=_tmpC3->kind == Cyc_Absyn_UnionA,((_tmp539.fieldnum=- 1,_tmp539)))),((
_tmp53A.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpC3->tvs,
_tmpAE,_tmpC4,_tmpC3->kind == Cyc_Absyn_UnionA,leafval),_tmp53A)))))),_tmp538))));};};}
_LL2E: {struct Cyc_Absyn_AnonAggrType_struct*_tmpAF=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA3;if(_tmpAF->tag != 13)goto _LL30;else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpAF->f2;}}
_LL2F: {struct Cyc_CfFlowInfo_Aggregate_struct _tmp540;struct Cyc_CfFlowInfo_UnionRInfo
_tmp53F;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp53E;return(void*)((_tmp53E=
_region_malloc(fenv->r,sizeof(*_tmp53E)),((_tmp53E[0]=((_tmp540.tag=6,((_tmp540.f1=((
_tmp53F.is_union=_tmpB0 == Cyc_Absyn_UnionA,((_tmp53F.fieldnum=- 1,_tmp53F)))),((
_tmp540.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpB1,_tmpB0 == Cyc_Absyn_UnionA,
leafval),_tmp540)))))),_tmp53E))));}_LL30:{struct Cyc_Absyn_ArrayType_struct*
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
struct Cyc_Absyn_Exp*_tmpD2;int _tmpD3;void*_tmpD4;struct _dyneither_ptr _tmpD6;
_LL40: {struct Cyc_CfFlowInfo_Consumed_struct*_tmpD1=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpD0;if(_tmpD1->tag != 7)goto _LL42;else{_tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;
_tmpD4=(void*)_tmpD1->f3;}}_LL41: if(_tmpD2 == e  && _tmpD3 == env_iteration){*
needs_unconsume=1;return 0;}return 1;_LL42: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpD5=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpD0;if(_tmpD5->tag != 6)goto
_LL44;else{_tmpD6=_tmpD5->f2;}}_LL43: {unsigned int _tmpD7=_get_dyneither_size(
_tmpD6,sizeof(void*));{int i=0;for(0;i < _tmpD7;++ i){if(Cyc_CfFlowInfo_is_unique_consumed(
e,env_iteration,*((void**)_check_dyneither_subscript(_tmpD6,sizeof(void*),i)),
needs_unconsume))return 1;}}return 0;}_LL44:;_LL45: return 0;_LL3F:;}void*Cyc_CfFlowInfo_make_unique_unconsumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);static void _tmp546(struct _dyneither_ptr*
_tmpDF,unsigned int*_tmp545,unsigned int*_tmp544,void***_tmp542){for(*_tmp545=0;*
_tmp545 < *_tmp544;(*_tmp545)++){(*_tmp542)[*_tmp545]=*((void**)
_check_dyneither_subscript(*_tmpDF,sizeof(void*),(int)*_tmp545));}}void*Cyc_CfFlowInfo_make_unique_unconsumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){void*_tmpD8=r;struct Cyc_Absyn_Exp*
_tmpDA;int _tmpDB;void*_tmpDC;struct Cyc_CfFlowInfo_UnionRInfo _tmpDE;struct
_dyneither_ptr _tmpDF;_LL47: {struct Cyc_CfFlowInfo_Consumed_struct*_tmpD9=(struct
Cyc_CfFlowInfo_Consumed_struct*)_tmpD8;if(_tmpD9->tag != 7)goto _LL49;else{_tmpDA=
_tmpD9->f1;_tmpDB=_tmpD9->f2;_tmpDC=(void*)_tmpD9->f3;}}_LL48: return _tmpDC;_LL49: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmpDD=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpD8;if(_tmpDD->tag != 6)goto _LL4B;else{_tmpDE=_tmpDD->f1;_tmpDF=_tmpDD->f2;}}
_LL4A: {unsigned int _tmpE0=_get_dyneither_size(_tmpDF,sizeof(void*));int change=0;
unsigned int _tmp545;unsigned int _tmp544;struct _dyneither_ptr _tmp543;void**
_tmp542;unsigned int _tmp541;struct _dyneither_ptr d2=(_tmp541=_tmpE0,((_tmp542=(
void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp541)),((_tmp543=
_tag_dyneither(_tmp542,sizeof(void*),_tmp541),((((_tmp544=_tmp541,_tmp546(&
_tmpDF,& _tmp545,& _tmp544,& _tmp542))),_tmp543)))))));{int i=0;for(0;i < _tmpE0;++ i){*((
void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(
fenv,*((void**)_check_dyneither_subscript(_tmpDF,sizeof(void*),i)));if(*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmpDF,sizeof(void*),i)))change=1;}}if(change){struct
Cyc_CfFlowInfo_Aggregate_struct _tmp549;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp548;return(void*)((_tmp548=_region_malloc(fenv->r,sizeof(*_tmp548)),((
_tmp548[0]=((_tmp549.tag=6,((_tmp549.f1=_tmpDE,((_tmp549.f2=d2,_tmp549)))))),
_tmp548))));}else{return r;}}_LL4B:;_LL4C: return r;_LL46:;}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r);static void _tmp553(struct Cyc_CfFlowInfo_FlowEnv**fenv,
unsigned int*_tmp552,unsigned int*_tmp551,void***_tmp54F){for(*_tmp552=0;*
_tmp552 < *_tmp551;(*_tmp552)++){(*_tmp54F)[*_tmp552]=(*fenv)->unknown_all;}}
static void _tmp55C(unsigned int*_tmp55B,unsigned int*_tmp55A,void***_tmp558,
struct _dyneither_ptr*_tmp101){for(*_tmp55B=0;*_tmp55B < *_tmp55A;(*_tmp55B)++){(*
_tmp558)[*_tmp55B]=((void**)(*_tmp101).curr)[(int)*_tmp55B];}}void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*r){if(Cyc_Tcutil_is_noalias_pointer(t)){struct Cyc_CfFlowInfo_Consumed_struct
_tmp54C;struct Cyc_CfFlowInfo_Consumed_struct*_tmp54B;return(void*)((_tmp54B=
_region_malloc(fenv->r,sizeof(*_tmp54B)),((_tmp54B[0]=((_tmp54C.tag=7,((_tmp54C.f1=
consumer,((_tmp54C.f2=iteration,((_tmp54C.f3=(void*)r,_tmp54C)))))))),_tmp54B))));}{
struct _tuple0 _tmp54D;struct _tuple0 _tmpEA=(_tmp54D.f1=Cyc_Tcutil_compress(t),((
_tmp54D.f2=r,_tmp54D)));void*_tmpEB;struct Cyc_List_List*_tmpED;void*_tmpEE;
struct Cyc_CfFlowInfo_UnionRInfo _tmpF0;struct _dyneither_ptr _tmpF1;void*_tmpF2;
struct Cyc_Absyn_AggrInfo _tmpF4;union Cyc_Absyn_AggrInfoU _tmpF5;void*_tmpF6;struct
Cyc_CfFlowInfo_UnionRInfo _tmpF8;struct _dyneither_ptr _tmpF9;void*_tmpFA;enum Cyc_Absyn_AggrKind
_tmpFC;struct Cyc_List_List*_tmpFD;void*_tmpFE;struct Cyc_CfFlowInfo_UnionRInfo
_tmp100;struct _dyneither_ptr _tmp101;_LL4E: _tmpEB=_tmpEA.f1;{struct Cyc_Absyn_TupleType_struct*
_tmpEC=(struct Cyc_Absyn_TupleType_struct*)_tmpEB;if(_tmpEC->tag != 11)goto _LL50;
else{_tmpED=_tmpEC->f1;}};_tmpEE=_tmpEA.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpEF=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpEE;if(_tmpEF->tag != 6)goto
_LL50;else{_tmpF0=_tmpEF->f1;_tmpF1=_tmpEF->f2;}};_LL4F: {unsigned int _tmp102=
_get_dyneither_size(_tmpF1,sizeof(void*));unsigned int _tmp552;unsigned int
_tmp551;struct _dyneither_ptr _tmp550;void**_tmp54F;unsigned int _tmp54E;struct
_dyneither_ptr d2=(_tmp54E=_tmp102,((_tmp54F=(void**)_region_malloc(fenv->r,
_check_times(sizeof(void*),_tmp54E)),((_tmp550=_tag_dyneither(_tmp54F,sizeof(
void*),_tmp54E),((((_tmp551=_tmp54E,_tmp553(& fenv,& _tmp552,& _tmp551,& _tmp54F))),
_tmp550)))))));{int i=0;for(0;i < _tmp102;++ i){*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmpED))->hd)).f2,
consumer,iteration,*((void**)_check_dyneither_subscript(_tmpF1,sizeof(void*),i)));
_tmpED=_tmpED->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp556;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp555;return(void*)((_tmp555=_region_malloc(fenv->r,sizeof(*_tmp555)),((
_tmp555[0]=((_tmp556.tag=6,((_tmp556.f1=_tmpF0,((_tmp556.f2=d2,_tmp556)))))),
_tmp555))));};}_LL50: _tmpF2=_tmpEA.f1;{struct Cyc_Absyn_AggrType_struct*_tmpF3=(
struct Cyc_Absyn_AggrType_struct*)_tmpF2;if(_tmpF3->tag != 12)goto _LL52;else{
_tmpF4=_tmpF3->f1;_tmpF5=_tmpF4.aggr_info;}};_tmpF6=_tmpEA.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpF7=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpF6;if(_tmpF7->tag != 6)goto
_LL52;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;}};_LL51: {struct Cyc_Absyn_Aggrdecl*
_tmp109=Cyc_Absyn_get_known_aggrdecl(_tmpF5);if(_tmp109->impl == 0)return r;_tmpFC=
_tmp109->kind;_tmpFD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp109->impl))->fields;
_tmp100=_tmpF8;_tmp101=_tmpF9;goto _LL53;}_LL52: _tmpFA=_tmpEA.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmpFB=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpFA;if(_tmpFB->tag != 13)goto
_LL54;else{_tmpFC=_tmpFB->f1;_tmpFD=_tmpFB->f2;}};_tmpFE=_tmpEA.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpFF=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpFE;if(_tmpFF->tag != 6)goto
_LL54;else{_tmp100=_tmpFF->f1;_tmp101=_tmpFF->f2;}};_LL53: {unsigned int _tmp55B;
unsigned int _tmp55A;struct _dyneither_ptr _tmp559;void**_tmp558;unsigned int
_tmp557;struct _dyneither_ptr d2=(_tmp557=_get_dyneither_size(_tmp101,sizeof(void*)),((
_tmp558=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp557)),((
_tmp559=_tag_dyneither(_tmp558,sizeof(void*),_tmp557),((((_tmp55A=_tmp557,
_tmp55C(& _tmp55B,& _tmp55A,& _tmp558,& _tmp101))),_tmp559)))))));unsigned int sz=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFD);{int i=0;
for(0;i < sz;(i ++,_tmpFD=_tmpFD->tl)){struct Cyc_Absyn_Aggrfield _tmp10B;struct
_dyneither_ptr*_tmp10C;void*_tmp10D;struct Cyc_Absyn_Aggrfield*_tmp10A=(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_List_List*)_check_null(_tmpFD))->hd;_tmp10B=*_tmp10A;_tmp10C=_tmp10B.name;
_tmp10D=_tmp10B.type;if(_get_dyneither_size(*_tmp10C,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(
fenv,_tmp10D,consumer,iteration,*((void**)_check_dyneither_subscript(_tmp101,
sizeof(void*),i)));}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp55F;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp55E;return(void*)((_tmp55E=_region_malloc(fenv->r,sizeof(*_tmp55E)),((
_tmp55E[0]=((_tmp55F.tag=6,((_tmp55F.f1=_tmp100,((_tmp55F.f2=d2,_tmp55F)))))),
_tmp55E))));};}_LL54:;_LL55: return r;_LL4D:;};}struct _tuple16{struct Cyc_CfFlowInfo_Place*
f1;struct Cyc_Position_Segment*f2;};static int Cyc_CfFlowInfo_prefix_of_member(
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){struct _RegionHandle
_tmp114=_new_region("r");struct _RegionHandle*r=& _tmp114;_push_region(r);{struct
_tuple16 _tmp560;struct _tuple16 _tmp115=(_tmp560.f1=place,((_tmp560.f2=0,_tmp560)));
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct
Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple16*))Cyc_Iter_next)(iter,& _tmp115)){struct Cyc_CfFlowInfo_Place*
_tmp116=_tmp115.f1;void*_tmp118;struct Cyc_List_List*_tmp119;struct Cyc_CfFlowInfo_Place
_tmp117=*place;_tmp118=_tmp117.root;_tmp119=_tmp117.fields;{void*_tmp11B;struct
Cyc_List_List*_tmp11C;struct Cyc_CfFlowInfo_Place _tmp11A=*_tmp116;_tmp11B=_tmp11A.root;
_tmp11C=_tmp11A.fields;if(Cyc_CfFlowInfo_root_cmp(_tmp118,_tmp11B)!= 0)continue;
for(0;_tmp119 != 0  && _tmp11C != 0;(_tmp119=_tmp119->tl,_tmp11C=_tmp11C->tl)){if((
int)_tmp119->hd != (int)_tmp11C->hd)break;}if(_tmp119 == 0){int _tmp11D=1;
_npop_handler(0);return _tmp11D;}};}{int _tmp11E=0;_npop_handler(0);return _tmp11E;};};
_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*
places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,
struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){struct Cyc_List_List*
_tmp561;pile->places=((_tmp561=_region_malloc(pile->rgn,sizeof(*_tmp561)),((
_tmp561->hd=place,((_tmp561->tl=pile->places,_tmp561))))));}}static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*r){void*_tmp121=r;void*_tmp123;struct Cyc_CfFlowInfo_Place*
_tmp125;struct Cyc_CfFlowInfo_UnionRInfo _tmp127;struct _dyneither_ptr _tmp128;_LL57: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmp122=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp121;if(_tmp122->tag != 7)goto _LL59;else{_tmp123=(void*)_tmp122->f3;}}_LL58:
Cyc_CfFlowInfo_add_places(pile,_tmp123);return;_LL59: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp124=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp121;if(_tmp124->tag != 5)goto
_LL5B;else{_tmp125=_tmp124->f1;}}_LL5A: Cyc_CfFlowInfo_add_place(pile,_tmp125);
return;_LL5B: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp126=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp121;if(_tmp126->tag != 6)goto _LL5D;else{_tmp127=_tmp126->f1;_tmp128=_tmp126->f2;}}
_LL5C:{int i=0;for(0;i < _get_dyneither_size(_tmp128,sizeof(void*));++ i){Cyc_CfFlowInfo_add_places(
pile,((void**)_tmp128.curr)[i]);}}return;_LL5D:;_LL5E: return;_LL56:;}static void*
Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*
old_val);static void _tmp567(struct _RegionHandle**r,void**new_val,unsigned int*
_tmp566,unsigned int*_tmp565,void***_tmp563,struct _dyneither_ptr*_tmp12D){for(*
_tmp566=0;*_tmp566 < *_tmp565;(*_tmp566)++){(*_tmp563)[*_tmp566]=Cyc_CfFlowInfo_insert_place_inner(*
r,*new_val,((void**)(*_tmp12D).curr)[(int)*_tmp566]);}}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmp129=old_val;struct Cyc_CfFlowInfo_UnionRInfo
_tmp12B;int _tmp12C;struct _dyneither_ptr _tmp12D;_LL60: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp12A=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp129;if(_tmp12A->tag != 6)goto
_LL62;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12B.is_union;_tmp12D=_tmp12A->f2;}}
_LL61: {unsigned int _tmp566;unsigned int _tmp565;struct _dyneither_ptr _tmp564;void**
_tmp563;unsigned int _tmp562;struct _dyneither_ptr d2=(_tmp562=_get_dyneither_size(
_tmp12D,sizeof(void*)),((_tmp563=(void**)_region_malloc(r,_check_times(sizeof(
void*),_tmp562)),((_tmp564=_tag_dyneither(_tmp563,sizeof(void*),_tmp562),((((
_tmp565=_tmp562,_tmp567(& r,& new_val,& _tmp566,& _tmp565,& _tmp563,& _tmp12D))),
_tmp564)))))));struct Cyc_CfFlowInfo_Aggregate_struct _tmp56D;struct Cyc_CfFlowInfo_UnionRInfo
_tmp56C;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp56B;return(void*)((_tmp56B=
_region_malloc(r,sizeof(*_tmp56B)),((_tmp56B[0]=((_tmp56D.tag=6,((_tmp56D.f1=((
_tmp56C.is_union=_tmp12C,((_tmp56C.fieldnum=- 1,_tmp56C)))),((_tmp56D.f2=d2,
_tmp56D)))))),_tmp56B))));}_LL62:;_LL63: return new_val;_LL5F:;}static struct
_dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct
_dyneither_ptr d,int n,void*rval);static void _tmp573(struct _dyneither_ptr*d,
unsigned int*_tmp572,unsigned int*_tmp571,void***_tmp56F){for(*_tmp572=0;*
_tmp572 < *_tmp571;(*_tmp572)++){(*_tmp56F)[*_tmp572]=((void**)(*d).curr)[(int)*
_tmp572];}}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmp135=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmp135 == rval)return d;{
unsigned int _tmp572;unsigned int _tmp571;struct _dyneither_ptr _tmp570;void**
_tmp56F;unsigned int _tmp56E;struct _dyneither_ptr res=(_tmp56E=_get_dyneither_size(
d,sizeof(void*)),((_tmp56F=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp56E)),((_tmp570=_tag_dyneither(_tmp56F,sizeof(void*),_tmp56E),((((_tmp571=
_tmp56E,_tmp573(& d,& _tmp572,& _tmp571,& _tmp56F))),_tmp570)))))));*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;};}struct
_tuple17{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 
0)return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{struct _tuple17
_tmp574;struct _tuple17 _tmp13B=(_tmp574.f1=fs,((_tmp574.f2=old_val,_tmp574)));
struct Cyc_List_List*_tmp13C;struct Cyc_List_List _tmp13D;int _tmp13E;struct Cyc_List_List*
_tmp13F;void*_tmp140;struct Cyc_CfFlowInfo_UnionRInfo _tmp142;int _tmp143;struct
_dyneither_ptr _tmp144;_LL65: _tmp13C=_tmp13B.f1;if(_tmp13C == 0)goto _LL67;_tmp13D=*
_tmp13C;_tmp13E=(int)_tmp13D.hd;_tmp13F=_tmp13D.tl;_tmp140=_tmp13B.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp141=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp140;if(_tmp141->tag != 6)goto
_LL67;else{_tmp142=_tmp141->f1;_tmp143=_tmp142.is_union;_tmp144=_tmp141->f2;}};
_LL66: {void*_tmp145=Cyc_CfFlowInfo_insert_place_outer(r,_tmp13F,*((void**)
_check_dyneither_subscript(_tmp144,sizeof(void*),_tmp13E)),new_val);struct
_dyneither_ptr _tmp146=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp144,_tmp13E,_tmp145);
if((void**)_tmp146.curr == (void**)_tmp144.curr)return old_val;else{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp57A;struct Cyc_CfFlowInfo_UnionRInfo _tmp579;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp578;return(void*)((_tmp578=_region_malloc(r,sizeof(*_tmp578)),((_tmp578[0]=((
_tmp57A.tag=6,((_tmp57A.f1=((_tmp579.is_union=_tmp143,((_tmp579.fieldnum=- 1,
_tmp579)))),((_tmp57A.f2=_tmp146,_tmp57A)))))),_tmp578))));}}_LL67:;_LL68: {
struct Cyc_Core_Impossible_struct _tmp580;const char*_tmp57F;struct Cyc_Core_Impossible_struct*
_tmp57E;(int)_throw((void*)((_tmp57E=_cycalloc(sizeof(*_tmp57E)),((_tmp57E[0]=((
_tmp580.tag=Cyc_Core_Impossible,((_tmp580.f1=((_tmp57F="bad insert place",
_tag_dyneither(_tmp57F,sizeof(char),17))),_tmp580)))),_tmp57E)))));}_LL64:;};}
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict
d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*_tmp14D=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)
_check_null(pile->places))->tl;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,_tmp14D,0);{void*oldval;void*newval;{struct _handler_cons _tmp14E;
_push_handler(& _tmp14E);{int _tmp150=0;if(setjmp(_tmp14E.handler))_tmp150=1;if(!
_tmp150){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp14D);;_pop_handler();}else{
void*_tmp14F=(void*)_exn_thrown;void*_tmp152=_tmp14F;_LL6A: {struct Cyc_Dict_Absent_struct*
_tmp153=(struct Cyc_Dict_Absent_struct*)_tmp152;if(_tmp153->tag != Cyc_Dict_Absent)
goto _LL6C;}_LL6B: continue;_LL6C:;_LL6D:(void)_throw(_tmp152);_LL69:;}};}switch(
Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){case Cyc_CfFlowInfo_AllIL: _LL6E: newval=
fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL: _LL6F: newval=fenv->esc_this;break;
case Cyc_CfFlowInfo_NoneIL: _LL70: newval=fenv->esc_none;break;}Cyc_CfFlowInfo_add_places(
pile,oldval);{void*_tmp155;struct Cyc_List_List*_tmp156;struct Cyc_CfFlowInfo_Place
_tmp154=*_tmp14D;_tmp155=_tmp154.root;_tmp156=_tmp154.fields;d=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp155,Cyc_CfFlowInfo_insert_place_outer(
fenv->r,_tmp156,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,
_tmp155),newval));};};}return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct
_RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};static enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmp157=r;enum Cyc_CfFlowInfo_InitLevel
_tmp159;enum Cyc_CfFlowInfo_InitLevel _tmp15B;void*_tmp160;_LL73: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp158=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp157;if(_tmp158->tag != 3)goto
_LL75;else{_tmp159=_tmp158->f1;}}_LL74: return _tmp159;_LL75: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp15A=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp157;if(_tmp15A->tag != 4)goto _LL77;
else{_tmp15B=_tmp15A->f1;}}_LL76: return _tmp15B;_LL77: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp15C=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp157;if(_tmp15C->tag != 0)goto _LL79;}
_LL78: goto _LL7A;_LL79: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp15D=(struct
Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp157;if(_tmp15D->tag != 1)goto _LL7B;}_LL7A:
return Cyc_CfFlowInfo_AllIL;_LL7B: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp15E=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp157;if(_tmp15E->tag != 2)
goto _LL7D;}_LL7C: return Cyc_CfFlowInfo_ThisIL;_LL7D: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp15F=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp157;if(_tmp15F->tag != 7)goto
_LL7F;else{_tmp160=(void*)_tmp15F->f3;}}_LL7E: return Cyc_CfFlowInfo_NoneIL;_LL7F:;
_LL80: {struct Cyc_Core_Impossible_struct _tmp586;const char*_tmp585;struct Cyc_Core_Impossible_struct*
_tmp584;(int)_throw((void*)((_tmp584=_cycalloc(sizeof(*_tmp584)),((_tmp584[0]=((
_tmp586.tag=Cyc_Core_Impossible,((_tmp586.f1=((_tmp585="initlevel_approx",
_tag_dyneither(_tmp585,sizeof(char),17))),_tmp586)))),_tmp584)))));}_LL72:;}
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){enum Cyc_CfFlowInfo_InitLevel
this_ans;if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;{void*
_tmp164=r;struct Cyc_CfFlowInfo_UnionRInfo _tmp166;int _tmp167;int _tmp168;struct
_dyneither_ptr _tmp169;struct Cyc_CfFlowInfo_UnionRInfo _tmp16B;int _tmp16C;struct
_dyneither_ptr _tmp16D;struct Cyc_CfFlowInfo_Place*_tmp16F;_LL82: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp165=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp164;if(_tmp165->tag != 6)goto
_LL84;else{_tmp166=_tmp165->f1;_tmp167=_tmp166.is_union;if(_tmp167 != 1)goto _LL84;
_tmp168=_tmp166.fieldnum;_tmp169=_tmp165->f2;}}_LL83: {unsigned int _tmp170=
_get_dyneither_size(_tmp169,sizeof(void*));this_ans=Cyc_CfFlowInfo_NoneIL;if(
_tmp168 == - 1){int i=0;for(0;i < _tmp170;++ i){if(Cyc_CfFlowInfo_initlevel_rec(env,*((
void**)_check_dyneither_subscript(_tmp169,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL){this_ans=Cyc_CfFlowInfo_AllIL;break;}}}else{if(Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp169,sizeof(void*),_tmp168)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL)this_ans=Cyc_CfFlowInfo_AllIL;}goto _LL81;}_LL84: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp16A=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp164;if(_tmp16A->tag != 6)goto _LL86;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16B.is_union;
if(_tmp16C != 0)goto _LL86;_tmp16D=_tmp16A->f2;}}_LL85: {unsigned int _tmp171=
_get_dyneither_size(_tmp16D,sizeof(void*));this_ans=Cyc_CfFlowInfo_AllIL;{int i=0;
for(0;i < _tmp171;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)
_check_dyneither_subscript(_tmp16D,sizeof(void*),i)),this_ans);}}goto _LL81;}
_LL86: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp16E=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp164;if(_tmp16E->tag != 5)goto _LL88;else{_tmp16F=_tmp16E->f1;}}_LL87: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp16F))this_ans=Cyc_CfFlowInfo_AllIL;else{{struct Cyc_List_List*_tmp587;env->seen=((
_tmp587=_region_malloc(env->e,sizeof(*_tmp587)),((_tmp587->hd=_tmp16F,((_tmp587->tl=
env->seen,_tmp587))))));}this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(
env->d,_tmp16F),Cyc_CfFlowInfo_AllIL);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == Cyc_CfFlowInfo_NoneIL)this_ans=Cyc_CfFlowInfo_ThisIL;}
goto _LL81;_LL88:;_LL89: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL81:;}if(
this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;if(this_ans == Cyc_CfFlowInfo_ThisIL
 || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;return Cyc_CfFlowInfo_AllIL;}
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*
env,struct Cyc_Dict_Dict d,void*r){struct _RegionHandle*_tmp173=env->r;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp588;struct Cyc_CfFlowInfo_InitlevelEnv _tmp174=(_tmp588.e=_tmp173,((_tmp588.d=
d,((_tmp588.seen=0,_tmp588)))));return Cyc_CfFlowInfo_initlevel_rec(& _tmp174,r,
Cyc_CfFlowInfo_AllIL);}struct _tuple18{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){void*_tmp177;struct Cyc_List_List*
_tmp178;struct Cyc_CfFlowInfo_Place _tmp176=*place;_tmp177=_tmp176.root;_tmp178=
_tmp176.fields;{void*_tmp179=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,_tmp177);for(0;_tmp178 != 0;_tmp178=_tmp178->tl){struct _tuple18 _tmp589;struct
_tuple18 _tmp17B=(_tmp589.f1=_tmp179,((_tmp589.f2=(int)_tmp178->hd,_tmp589)));
void*_tmp17C;struct Cyc_CfFlowInfo_UnionRInfo _tmp17E;struct _dyneither_ptr _tmp17F;
int _tmp180;_LL8B: _tmp17C=_tmp17B.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp17D=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp17C;if(_tmp17D->tag != 6)goto
_LL8D;else{_tmp17E=_tmp17D->f1;_tmp17F=_tmp17D->f2;}};_tmp180=_tmp17B.f2;_LL8C:
_tmp179=*((void**)_check_dyneither_subscript(_tmp17F,sizeof(void*),_tmp180));
goto _LL8A;_LL8D:;_LL8E: {struct Cyc_Core_Impossible_struct _tmp58F;const char*
_tmp58E;struct Cyc_Core_Impossible_struct*_tmp58D;(int)_throw((void*)((_tmp58D=
_cycalloc(sizeof(*_tmp58D)),((_tmp58D[0]=((_tmp58F.tag=Cyc_Core_Impossible,((
_tmp58F.f1=((_tmp58E="bad lookup_place",_tag_dyneither(_tmp58E,sizeof(char),17))),
_tmp58F)))),_tmp58D)))));}_LL8A:;}return _tmp179;};}static int Cyc_CfFlowInfo_is_rval_unescaped(
void*rval){void*_tmp184=rval;void*_tmp187;struct Cyc_CfFlowInfo_UnionRInfo _tmp189;
int _tmp18A;int _tmp18B;struct _dyneither_ptr _tmp18C;_LL90: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp185=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp184;if(_tmp185->tag != 4)goto _LL92;}
_LL91: return 0;_LL92: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp186=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp184;if(_tmp186->tag != 7)goto _LL94;else{_tmp187=(void*)_tmp186->f3;}}_LL93:
return Cyc_CfFlowInfo_is_rval_unescaped(rval);_LL94: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp188=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp184;if(_tmp188->tag != 6)goto
_LL96;else{_tmp189=_tmp188->f1;_tmp18A=_tmp189.is_union;_tmp18B=_tmp189.fieldnum;
_tmp18C=_tmp188->f2;}}_LL95: if(_tmp18A  && _tmp18B != - 1)return !Cyc_CfFlowInfo_is_rval_unescaped(*((
void**)_check_dyneither_subscript(_tmp18C,sizeof(void*),_tmp18B)));else{
unsigned int sz=_get_dyneither_size(_tmp18C,sizeof(void*));{int i=0;for(0;i < sz;++
i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp18C.curr)[i]))return 0;}}
return 1;}_LL96:;_LL97: return 1;_LL8F:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(
d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct
_RegionHandle*_tmp18D=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp590;struct Cyc_CfFlowInfo_EscPile*
pile=(_tmp590=_region_malloc(_tmp18D,sizeof(*_tmp590)),((_tmp590->rgn=_tmp18D,((
_tmp590->places=0,_tmp590)))));Cyc_CfFlowInfo_add_places(pile,r);return Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*oldval,void*newval);static void _tmp59A(struct Cyc_CfFlowInfo_AssignEnv**
env,unsigned int*_tmp599,unsigned int*_tmp598,void***_tmp596,struct
_dyneither_ptr*_tmp19F,struct _dyneither_ptr*_tmp19B){for(*_tmp599=0;*_tmp599 < *
_tmp598;(*_tmp599)++){(*_tmp596)[*_tmp599]=Cyc_CfFlowInfo_assign_place_inner(*
env,((void**)(*_tmp19B).curr)[(int)*_tmp599],*((void**)
_check_dyneither_subscript(*_tmp19F,sizeof(void*),(int)*_tmp599)));}}static void*
Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,
void*newval){struct _tuple0 _tmp591;struct _tuple0 _tmp190=(_tmp591.f1=oldval,((
_tmp591.f2=newval,_tmp591)));void*_tmp191;void*_tmp193;struct Cyc_CfFlowInfo_Place*
_tmp195;void*_tmp196;void*_tmp198;struct Cyc_CfFlowInfo_UnionRInfo _tmp19A;struct
_dyneither_ptr _tmp19B;void*_tmp19C;struct Cyc_CfFlowInfo_UnionRInfo _tmp19E;struct
_dyneither_ptr _tmp19F;void*_tmp1A0;enum Cyc_CfFlowInfo_InitLevel _tmp1A2;_LL99:
_tmp191=_tmp190.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp192=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp191;if(_tmp192->tag != 4)goto _LL9B;};_tmp193=_tmp190.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp194=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp193;if(_tmp194->tag != 5)goto
_LL9B;else{_tmp195=_tmp194->f1;}};_LL9A: Cyc_CfFlowInfo_add_place(env->pile,
_tmp195);goto _LL9C;_LL9B: _tmp196=_tmp190.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp197=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp196;if(_tmp197->tag != 4)goto _LL9D;};
_LL9C: if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp594;void*_tmp593;(_tmp593=0,Cyc_Tcutil_terr(env->loc,((_tmp594="assignment puts possibly-uninitialized data in an escaped location",
_tag_dyneither(_tmp594,sizeof(char),67))),_tag_dyneither(_tmp593,sizeof(void*),0)));}
return(env->fenv)->esc_all;_LL9D: _tmp198=_tmp190.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp199=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp198;if(_tmp199->tag != 6)goto
_LL9F;else{_tmp19A=_tmp199->f1;_tmp19B=_tmp199->f2;}};_tmp19C=_tmp190.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp19D=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp19C;if(_tmp19D->tag != 6)goto _LL9F;else{_tmp19E=_tmp19D->f1;_tmp19F=_tmp19D->f2;}};
_LL9E: {unsigned int _tmp599;unsigned int _tmp598;struct _dyneither_ptr _tmp597;void**
_tmp596;unsigned int _tmp595;struct _dyneither_ptr new_d=(_tmp595=
_get_dyneither_size(_tmp19B,sizeof(void*)),((_tmp596=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp595)),((_tmp597=_tag_dyneither(_tmp596,sizeof(
void*),_tmp595),((((_tmp598=_tmp595,_tmp59A(& env,& _tmp599,& _tmp598,& _tmp596,&
_tmp19F,& _tmp19B))),_tmp597)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp19B,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp19B.curr)[i]){
change=1;break;}}}if(!change)return oldval;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp19B,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp19F,sizeof(void*),i))){change=1;break;}}}if(!
change)return newval;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp59D;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp59C;return(void*)((_tmp59C=_region_malloc((env->fenv)->r,sizeof(*_tmp59C)),((
_tmp59C[0]=((_tmp59D.tag=6,((_tmp59D.f1=_tmp19A,((_tmp59D.f2=new_d,_tmp59D)))))),
_tmp59C))));};}_LL9F: _tmp1A0=_tmp190.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp1A1=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp1A0;if(_tmp1A1->tag != 4)goto _LLA1;else{
_tmp1A2=_tmp1A1->f1;}};_LLA0: switch(_tmp1A2){case Cyc_CfFlowInfo_NoneIL: _LLA3:
return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LLA4: return(env->fenv)->unknown_this;
case Cyc_CfFlowInfo_AllIL: _LLA5: return(env->fenv)->unknown_all;}_LLA1:;_LLA2:
return newval;_LL98:;}static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(
env,oldval,newval);{struct _tuple17 _tmp59E;struct _tuple17 _tmp1AC=(_tmp59E.f1=fs,((
_tmp59E.f2=oldval,_tmp59E)));struct Cyc_List_List*_tmp1AD;struct Cyc_List_List
_tmp1AE;int _tmp1AF;struct Cyc_List_List*_tmp1B0;void*_tmp1B1;struct Cyc_CfFlowInfo_UnionRInfo
_tmp1B3;int _tmp1B4;int _tmp1B5;struct _dyneither_ptr _tmp1B6;_LLA8: _tmp1AD=_tmp1AC.f1;
if(_tmp1AD == 0)goto _LLAA;_tmp1AE=*_tmp1AD;_tmp1AF=(int)_tmp1AE.hd;_tmp1B0=
_tmp1AE.tl;_tmp1B1=_tmp1AC.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1B2=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1B1;if(_tmp1B2->tag != 6)goto _LLAA;
else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B3.is_union;_tmp1B5=_tmp1B3.fieldnum;
_tmp1B6=_tmp1B2->f2;}};_LLA9: {void*_tmp1B7=Cyc_CfFlowInfo_assign_place_outer(
env,_tmp1B0,*((void**)_check_dyneither_subscript(_tmp1B6,sizeof(void*),_tmp1AF)),
newval);struct _dyneither_ptr _tmp1B8=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,
_tmp1B6,_tmp1AF,_tmp1B7);if((void**)_tmp1B8.curr == (void**)_tmp1B6.curr  && (!
_tmp1B4  || _tmp1B5 == _tmp1AF))return oldval;{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp5A4;struct Cyc_CfFlowInfo_UnionRInfo _tmp5A3;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5A2;return(void*)((_tmp5A2=_region_malloc((env->fenv)->r,sizeof(*_tmp5A2)),((
_tmp5A2[0]=((_tmp5A4.tag=6,((_tmp5A4.f1=((_tmp5A3.is_union=_tmp1B4,((_tmp5A3.fieldnum=
_tmp1AF,_tmp5A3)))),((_tmp5A4.f2=_tmp1B8,_tmp5A4)))))),_tmp5A2))));};}_LLAA:;
_LLAB: {struct Cyc_Core_Impossible_struct _tmp5AA;const char*_tmp5A9;struct Cyc_Core_Impossible_struct*
_tmp5A8;(int)_throw((void*)((_tmp5A8=_cycalloc(sizeof(*_tmp5A8)),((_tmp5A8[0]=((
_tmp5AA.tag=Cyc_Core_Impossible,((_tmp5AA.f1=((_tmp5A9="bad assign place",
_tag_dyneither(_tmp5A9,sizeof(char),17))),_tmp5AA)))),_tmp5A8)))));}_LLA7:;};}
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,place,0);{struct _RegionHandle*_tmp1BF=fenv->r;struct Cyc_CfFlowInfo_Place
_tmp1C1;void*_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_CfFlowInfo_Place*
_tmp1C0=place;_tmp1C1=*_tmp1C0;_tmp1C2=_tmp1C1.root;_tmp1C3=_tmp1C1.fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp5AD;struct Cyc_CfFlowInfo_AssignEnv _tmp5AC;
struct Cyc_CfFlowInfo_AssignEnv env=(_tmp5AC.fenv=fenv,((_tmp5AC.pile=((_tmp5AD=
_region_malloc(_tmp1BF,sizeof(*_tmp5AD)),((_tmp5AD->rgn=_tmp1BF,((_tmp5AD->places=
0,_tmp5AD)))))),((_tmp5AC.d=d,((_tmp5AC.loc=loc,_tmp5AC)))))));void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp1C3,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1C2),
r);return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1C2,newval));};};}
struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict
chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);struct _tuple19{enum Cyc_CfFlowInfo_InitLevel f1;
enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct
Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp5BD(struct Cyc_CfFlowInfo_JoinEnv**
env,unsigned int*_tmp5BC,unsigned int*_tmp5BB,void***_tmp5B9,struct
_dyneither_ptr*_tmp210,struct _dyneither_ptr*_tmp20A){for(*_tmp5BC=0;*_tmp5BC < *
_tmp5BB;(*_tmp5BC)++){(*_tmp5B9)[*_tmp5BC]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp20A).curr)[(
int)*_tmp5BC],*((void**)_check_dyneither_subscript(*_tmp210,sizeof(void*),(int)*
_tmp5BC)));}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp1C6=(
env->fenv)->r;{struct _tuple0 _tmp5AE;struct _tuple0 _tmp1C8=(_tmp5AE.f1=r1,((
_tmp5AE.f2=r2,_tmp5AE)));void*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CB;int _tmp1CC;
void*_tmp1CD;void*_tmp1CE;struct Cyc_Absyn_Exp*_tmp1D0;int _tmp1D1;void*_tmp1D2;
void*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D5;int _tmp1D6;void*_tmp1D7;void*_tmp1D8;
struct Cyc_Absyn_Exp*_tmp1DA;int _tmp1DB;void*_tmp1DC;void*_tmp1DD;struct Cyc_CfFlowInfo_Place*
_tmp1DF;void*_tmp1E0;struct Cyc_CfFlowInfo_Place*_tmp1E2;void*_tmp1E3;struct Cyc_CfFlowInfo_Place*
_tmp1E5;void*_tmp1E6;void*_tmp1E8;void*_tmp1EA;struct Cyc_CfFlowInfo_Place*
_tmp1EC;void*_tmp1ED;struct Cyc_CfFlowInfo_Place*_tmp1EF;void*_tmp1F0;void*
_tmp1F2;void*_tmp1F4;struct Cyc_CfFlowInfo_Place*_tmp1F6;void*_tmp1F7;struct Cyc_CfFlowInfo_Place*
_tmp1F9;void*_tmp1FA;struct Cyc_CfFlowInfo_Place*_tmp1FC;void*_tmp1FD;void*
_tmp1FF;void*_tmp201;void*_tmp203;void*_tmp205;struct Cyc_CfFlowInfo_UnionRInfo
_tmp207;int _tmp208;int _tmp209;struct _dyneither_ptr _tmp20A;void*_tmp20B;struct Cyc_CfFlowInfo_UnionRInfo
_tmp20D;int _tmp20E;int _tmp20F;struct _dyneither_ptr _tmp210;_LLAD: _tmp1C9=_tmp1C8.f1;{
struct Cyc_CfFlowInfo_Consumed_struct*_tmp1CA=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1C9;if(_tmp1CA->tag != 7)goto _LLAF;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=_tmp1CA->f2;
_tmp1CD=(void*)_tmp1CA->f3;}};_tmp1CE=_tmp1C8.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1CF=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1CE;if(_tmp1CF->tag != 7)goto
_LLAF;else{_tmp1D0=_tmp1CF->f1;_tmp1D1=_tmp1CF->f2;_tmp1D2=(void*)_tmp1CF->f3;}};
_LLAE: if(_tmp1CB == _tmp1D0  && _tmp1CC == _tmp1D1){struct Cyc_CfFlowInfo_Consumed_struct
_tmp5B1;struct Cyc_CfFlowInfo_Consumed_struct*_tmp5B0;return(void*)((_tmp5B0=
_region_malloc(_tmp1C6,sizeof(*_tmp5B0)),((_tmp5B0[0]=((_tmp5B1.tag=7,((_tmp5B1.f1=
_tmp1CB,((_tmp5B1.f2=_tmp1CC,((_tmp5B1.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1CD,_tmp1D2),_tmp5B1)))))))),_tmp5B0))));}{void*_tmp213=_tmp1CD;struct Cyc_CfFlowInfo_Place*
_tmp215;_LLCA: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp214=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp213;if(_tmp214->tag != 5)goto _LLCC;else{_tmp215=_tmp214->f1;}}_LLCB: Cyc_CfFlowInfo_add_place(
env->pile,_tmp215);goto _LLC9;_LLCC:;_LLCD: goto _LLC9;_LLC9:;}{void*_tmp216=
_tmp1D2;struct Cyc_CfFlowInfo_Place*_tmp218;_LLCF: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp217=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp216;if(_tmp217->tag != 5)goto
_LLD1;else{_tmp218=_tmp217->f1;}}_LLD0: Cyc_CfFlowInfo_add_place(env->pile,
_tmp218);goto _LLCE;_LLD1:;_LLD2: goto _LLCE;_LLCE:;}goto _LLAC;_LLAF: _tmp1D3=
_tmp1C8.f1;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp1D4=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp1D3;if(_tmp1D4->tag != 7)goto _LLB1;else{_tmp1D5=_tmp1D4->f1;_tmp1D6=_tmp1D4->f2;
_tmp1D7=(void*)_tmp1D4->f3;}};_LLB0: {struct Cyc_CfFlowInfo_Consumed_struct
_tmp5B4;struct Cyc_CfFlowInfo_Consumed_struct*_tmp5B3;return(void*)((_tmp5B3=
_region_malloc(_tmp1C6,sizeof(*_tmp5B3)),((_tmp5B3[0]=((_tmp5B4.tag=7,((_tmp5B4.f1=
_tmp1D5,((_tmp5B4.f2=_tmp1D6,((_tmp5B4.f3=(void*)Cyc_CfFlowInfo_join_absRval(env,
a,_tmp1D7,r2),_tmp5B4)))))))),_tmp5B3))));}_LLB1: _tmp1D8=_tmp1C8.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp1D9=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp1D8;if(_tmp1D9->tag != 7)goto
_LLB3;else{_tmp1DA=_tmp1D9->f1;_tmp1DB=_tmp1D9->f2;_tmp1DC=(void*)_tmp1D9->f3;}};
_LLB2: {struct Cyc_CfFlowInfo_Consumed_struct _tmp5B7;struct Cyc_CfFlowInfo_Consumed_struct*
_tmp5B6;return(void*)((_tmp5B6=_region_malloc(_tmp1C6,sizeof(*_tmp5B6)),((
_tmp5B6[0]=((_tmp5B7.tag=7,((_tmp5B7.f1=_tmp1DA,((_tmp5B7.f2=_tmp1DB,((_tmp5B7.f3=(
void*)Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1DC),_tmp5B7)))))))),_tmp5B6))));}
_LLB3: _tmp1DD=_tmp1C8.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1DE=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1DD;if(_tmp1DE->tag != 5)goto _LLB5;else{
_tmp1DF=_tmp1DE->f1;}};_tmp1E0=_tmp1C8.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1E1=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1E0;if(_tmp1E1->tag != 5)goto
_LLB5;else{_tmp1E2=_tmp1E1->f1;}};_LLB4: if(Cyc_CfFlowInfo_place_cmp(_tmp1DF,
_tmp1E2)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp1DF);Cyc_CfFlowInfo_add_place(
env->pile,_tmp1E2);goto _LLAC;_LLB5: _tmp1E3=_tmp1C8.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1E4=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1E3;if(_tmp1E4->tag != 5)goto
_LLB7;else{_tmp1E5=_tmp1E4->f1;}};_tmp1E6=_tmp1C8.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1E7=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1E6;if(_tmp1E7->tag != 2)
goto _LLB7;};_LLB6: _tmp1EC=_tmp1E5;goto _LLB8;_LLB7: _tmp1E8=_tmp1C8.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1E9=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1E8;if(_tmp1E9->tag != 2)
goto _LLB9;};_tmp1EA=_tmp1C8.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1EB=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1EA;if(_tmp1EB->tag != 5)goto _LLB9;
else{_tmp1EC=_tmp1EB->f1;}};_LLB8: Cyc_CfFlowInfo_add_place(env->pile,_tmp1EC);
return(env->fenv)->notzerothis;_LLB9: _tmp1ED=_tmp1C8.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1EE=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1ED;if(_tmp1EE->tag != 5)goto
_LLBB;else{_tmp1EF=_tmp1EE->f1;}};_tmp1F0=_tmp1C8.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp1F1=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1F0;if(_tmp1F1->tag != 1)
goto _LLBB;};_LLBA: Cyc_CfFlowInfo_add_place(env->pile,_tmp1EF);switch(Cyc_CfFlowInfo_initlevel(
env->fenv,env->d1,r1)){case Cyc_CfFlowInfo_AllIL: _LLD3: return(env->fenv)->notzeroall;
default: _LLD4: return(env->fenv)->notzerothis;}_LLBB: _tmp1F2=_tmp1C8.f1;{struct
Cyc_CfFlowInfo_NotZeroAll_struct*_tmp1F3=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp1F2;if(_tmp1F3->tag != 1)goto _LLBD;};_tmp1F4=_tmp1C8.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp1F5=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F4;if(_tmp1F5->tag != 5)goto
_LLBD;else{_tmp1F6=_tmp1F5->f1;}};_LLBC: Cyc_CfFlowInfo_add_place(env->pile,
_tmp1F6);switch(Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2)){case Cyc_CfFlowInfo_AllIL:
_LLD6: return(env->fenv)->notzeroall;default: _LLD7: return(env->fenv)->notzerothis;}
_LLBD: _tmp1F7=_tmp1C8.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1F8=(struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F7;if(_tmp1F8->tag != 5)goto _LLBF;else{
_tmp1F9=_tmp1F8->f1;}};_LLBE: Cyc_CfFlowInfo_add_place(env->pile,_tmp1F9);goto
_LLAC;_LLBF: _tmp1FA=_tmp1C8.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp1FB=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1FA;if(_tmp1FB->tag != 5)goto _LLC1;
else{_tmp1FC=_tmp1FB->f1;}};_LLC0: Cyc_CfFlowInfo_add_place(env->pile,_tmp1FC);
goto _LLAC;_LLC1: _tmp1FD=_tmp1C8.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp1FE=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1FD;if(_tmp1FE->tag != 1)
goto _LLC3;};_tmp1FF=_tmp1C8.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp200=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1FF;if(_tmp200->tag != 2)goto _LLC3;};
_LLC2: goto _LLC4;_LLC3: _tmp201=_tmp1C8.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp202=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp201;if(_tmp202->tag != 2)
goto _LLC5;};_tmp203=_tmp1C8.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp204=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp203;if(_tmp204->tag != 1)goto _LLC5;};
_LLC4: return(env->fenv)->notzerothis;_LLC5: _tmp205=_tmp1C8.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp206=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp205;if(_tmp206->tag != 6)goto
_LLC7;else{_tmp207=_tmp206->f1;_tmp208=_tmp207.is_union;_tmp209=_tmp207.fieldnum;
_tmp20A=_tmp206->f2;}};_tmp20B=_tmp1C8.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp20C=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp20B;if(_tmp20C->tag != 6)goto
_LLC7;else{_tmp20D=_tmp20C->f1;_tmp20E=_tmp20D.is_union;_tmp20F=_tmp20D.fieldnum;
_tmp210=_tmp20C->f2;}};_LLC6: {unsigned int _tmp5BC;unsigned int _tmp5BB;struct
_dyneither_ptr _tmp5BA;void**_tmp5B9;unsigned int _tmp5B8;struct _dyneither_ptr
new_d=(_tmp5B8=_get_dyneither_size(_tmp20A,sizeof(void*)),((_tmp5B9=(void**)
_region_malloc(_tmp1C6,_check_times(sizeof(void*),_tmp5B8)),((_tmp5BA=
_tag_dyneither(_tmp5B9,sizeof(void*),_tmp5B8),((((_tmp5BB=_tmp5B8,_tmp5BD(& env,&
_tmp5BC,& _tmp5BB,& _tmp5B9,& _tmp210,& _tmp20A))),_tmp5BA)))))));int change=0;{int i=
0;for(0;i < _get_dyneither_size(_tmp20A,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp20A.curr)[i]){
change=1;break;}}}if(!change){if(!_tmp208)return r1;new_d=_tmp20A;}else{change=0;{
int i=0;for(0;i < _get_dyneither_size(_tmp20A,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp210,sizeof(void*),i))){change=1;break;}}}if(!
change){if(!_tmp208)return r2;new_d=_tmp210;}}{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp5C3;struct Cyc_CfFlowInfo_UnionRInfo _tmp5C2;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5C1;return(void*)((_tmp5C1=_region_malloc(_tmp1C6,sizeof(*_tmp5C1)),((
_tmp5C1[0]=((_tmp5C3.tag=6,((_tmp5C3.f1=((_tmp5C2.is_union=_tmp208,((_tmp5C2.fieldnum=
_tmp209 == _tmp20F?_tmp209: - 1,_tmp5C2)))),((_tmp5C3.f2=new_d,_tmp5C3)))))),
_tmp5C1))));};}_LLC7:;_LLC8: goto _LLAC;_LLAC:;}{enum Cyc_CfFlowInfo_InitLevel il1=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel il2=
Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);struct _tuple0 _tmp5C4;struct
_tuple0 _tmp225=(_tmp5C4.f1=r1,((_tmp5C4.f2=r2,_tmp5C4)));void*_tmp226;void*
_tmp228;_LLDA: _tmp226=_tmp225.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp227=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp226;if(_tmp227->tag != 4)goto _LLDC;};_LLDB:
goto _LLDD;_LLDC: _tmp228=_tmp225.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp229=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp228;if(_tmp229->tag != 4)goto _LLDE;};_LLDD: {
struct _tuple19 _tmp5C5;struct _tuple19 _tmp22B=(_tmp5C5.f1=il1,((_tmp5C5.f2=il2,
_tmp5C5)));enum Cyc_CfFlowInfo_InitLevel _tmp22C;enum Cyc_CfFlowInfo_InitLevel
_tmp22D;enum Cyc_CfFlowInfo_InitLevel _tmp22E;enum Cyc_CfFlowInfo_InitLevel
_tmp22F;_LLE1: _tmp22C=_tmp22B.f2;if(_tmp22C != Cyc_CfFlowInfo_NoneIL)goto _LLE3;
_LLE2: goto _LLE4;_LLE3: _tmp22D=_tmp22B.f1;if(_tmp22D != Cyc_CfFlowInfo_NoneIL)goto
_LLE5;_LLE4: return(env->fenv)->esc_none;_LLE5: _tmp22E=_tmp22B.f2;if(_tmp22E != 
Cyc_CfFlowInfo_ThisIL)goto _LLE7;_LLE6: goto _LLE8;_LLE7: _tmp22F=_tmp22B.f1;if(
_tmp22F != Cyc_CfFlowInfo_ThisIL)goto _LLE9;_LLE8: return(env->fenv)->esc_this;
_LLE9:;_LLEA: return(env->fenv)->esc_all;_LLE0:;}_LLDE:;_LLDF: {struct _tuple19
_tmp5C6;struct _tuple19 _tmp231=(_tmp5C6.f1=il1,((_tmp5C6.f2=il2,_tmp5C6)));enum 
Cyc_CfFlowInfo_InitLevel _tmp232;enum Cyc_CfFlowInfo_InitLevel _tmp233;enum Cyc_CfFlowInfo_InitLevel
_tmp234;enum Cyc_CfFlowInfo_InitLevel _tmp235;_LLEC: _tmp232=_tmp231.f2;if(_tmp232
!= Cyc_CfFlowInfo_NoneIL)goto _LLEE;_LLED: goto _LLEF;_LLEE: _tmp233=_tmp231.f1;if(
_tmp233 != Cyc_CfFlowInfo_NoneIL)goto _LLF0;_LLEF: return(env->fenv)->unknown_none;
_LLF0: _tmp234=_tmp231.f2;if(_tmp234 != Cyc_CfFlowInfo_ThisIL)goto _LLF2;_LLF1: goto
_LLF3;_LLF2: _tmp235=_tmp231.f1;if(_tmp235 != Cyc_CfFlowInfo_ThisIL)goto _LLF4;
_LLF3: return(env->fenv)->unknown_this;_LLF4:;_LLF5: return(env->fenv)->unknown_all;
_LLEB:;}_LLD9:;};};}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5C7;return((_tmp5C7.EqualConst).val=
i,(((_tmp5C7.EqualConst).tag=1,_tmp5C7)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(
struct Cyc_Absyn_Vardecl*v,void*t){struct _tuple10 _tmp5CA;union Cyc_CfFlowInfo_RelnOp
_tmp5C9;return((_tmp5C9.LessVar).val=((_tmp5CA.f1=v,((_tmp5CA.f2=t,_tmp5CA)))),(((
_tmp5C9.LessVar).tag=2,_tmp5C9)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(
struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp5CB;return((_tmp5CB.LessNumelts).val=
v,(((_tmp5CB.LessNumelts).tag=3,_tmp5CB)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5CC;return((_tmp5CC.LessConst).val=
i,(((_tmp5CC.LessConst).tag=4,_tmp5CC)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(
unsigned int i){union Cyc_CfFlowInfo_RelnOp _tmp5CD;return((_tmp5CD.LessEqConst).val=
i,(((_tmp5CD.LessEqConst).tag=6,_tmp5CD)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(
struct Cyc_Absyn_Vardecl*v){union Cyc_CfFlowInfo_RelnOp _tmp5CE;return((_tmp5CE.LessEqNumelts).val=
v,(((_tmp5CE.LessEqNumelts).tag=5,_tmp5CE)));}struct _tuple20{union Cyc_CfFlowInfo_RelnOp
f1;union Cyc_CfFlowInfo_RelnOp f2;};static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2){struct _tuple20 _tmp5CF;struct _tuple20 _tmp23E=(
_tmp5CF.f1=r1,((_tmp5CF.f2=r2,_tmp5CF)));union Cyc_CfFlowInfo_RelnOp _tmp23F;
unsigned int _tmp240;union Cyc_CfFlowInfo_RelnOp _tmp241;unsigned int _tmp242;union
Cyc_CfFlowInfo_RelnOp _tmp243;struct _tuple10 _tmp244;struct Cyc_Absyn_Vardecl*
_tmp245;union Cyc_CfFlowInfo_RelnOp _tmp246;struct _tuple10 _tmp247;struct Cyc_Absyn_Vardecl*
_tmp248;union Cyc_CfFlowInfo_RelnOp _tmp249;struct Cyc_Absyn_Vardecl*_tmp24A;union
Cyc_CfFlowInfo_RelnOp _tmp24B;struct Cyc_Absyn_Vardecl*_tmp24C;union Cyc_CfFlowInfo_RelnOp
_tmp24D;unsigned int _tmp24E;union Cyc_CfFlowInfo_RelnOp _tmp24F;unsigned int
_tmp250;union Cyc_CfFlowInfo_RelnOp _tmp251;unsigned int _tmp252;union Cyc_CfFlowInfo_RelnOp
_tmp253;unsigned int _tmp254;union Cyc_CfFlowInfo_RelnOp _tmp255;struct Cyc_Absyn_Vardecl*
_tmp256;union Cyc_CfFlowInfo_RelnOp _tmp257;struct Cyc_Absyn_Vardecl*_tmp258;_LLF7:
_tmp23F=_tmp23E.f1;if((_tmp23F.EqualConst).tag != 1)goto _LLF9;_tmp240=(
unsigned int)(_tmp23F.EqualConst).val;_tmp241=_tmp23E.f2;if((_tmp241.EqualConst).tag
!= 1)goto _LLF9;_tmp242=(unsigned int)(_tmp241.EqualConst).val;_LLF8: return
_tmp240 == _tmp242;_LLF9: _tmp243=_tmp23E.f1;if((_tmp243.LessVar).tag != 2)goto
_LLFB;_tmp244=(struct _tuple10)(_tmp243.LessVar).val;_tmp245=_tmp244.f1;_tmp246=
_tmp23E.f2;if((_tmp246.LessVar).tag != 2)goto _LLFB;_tmp247=(struct _tuple10)(
_tmp246.LessVar).val;_tmp248=_tmp247.f1;_LLFA: return _tmp245 == _tmp248;_LLFB:
_tmp249=_tmp23E.f1;if((_tmp249.LessNumelts).tag != 3)goto _LLFD;_tmp24A=(struct Cyc_Absyn_Vardecl*)(
_tmp249.LessNumelts).val;_tmp24B=_tmp23E.f2;if((_tmp24B.LessNumelts).tag != 3)
goto _LLFD;_tmp24C=(struct Cyc_Absyn_Vardecl*)(_tmp24B.LessNumelts).val;_LLFC:
return _tmp24A == _tmp24C;_LLFD: _tmp24D=_tmp23E.f1;if((_tmp24D.LessConst).tag != 4)
goto _LLFF;_tmp24E=(unsigned int)(_tmp24D.LessConst).val;_tmp24F=_tmp23E.f2;if((
_tmp24F.LessConst).tag != 4)goto _LLFF;_tmp250=(unsigned int)(_tmp24F.LessConst).val;
_LLFE: return _tmp24E == _tmp250;_LLFF: _tmp251=_tmp23E.f1;if((_tmp251.LessEqConst).tag
!= 6)goto _LL101;_tmp252=(unsigned int)(_tmp251.LessEqConst).val;_tmp253=_tmp23E.f2;
if((_tmp253.LessEqConst).tag != 6)goto _LL101;_tmp254=(unsigned int)(_tmp253.LessEqConst).val;
_LL100: return _tmp252 == _tmp254;_LL101: _tmp255=_tmp23E.f1;if((_tmp255.LessEqNumelts).tag
!= 5)goto _LL103;_tmp256=(struct Cyc_Absyn_Vardecl*)(_tmp255.LessEqNumelts).val;
_tmp257=_tmp23E.f2;if((_tmp257.LessEqNumelts).tag != 5)goto _LL103;_tmp258=(struct
Cyc_Absyn_Vardecl*)(_tmp257.LessEqNumelts).val;_LL102: return _tmp256 == _tmp258;
_LL103:;_LL104: return 0;_LLF6:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == 
r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*_tmp259=
r1s;for(0;_tmp259 != 0;_tmp259=_tmp259->tl){struct Cyc_CfFlowInfo_Reln*_tmp25A=(
struct Cyc_CfFlowInfo_Reln*)_tmp259->hd;int found=0;{struct Cyc_List_List*_tmp25B=
r2s;for(0;_tmp25B != 0;_tmp25B=_tmp25B->tl){struct Cyc_CfFlowInfo_Reln*_tmp25C=(
struct Cyc_CfFlowInfo_Reln*)_tmp25B->hd;if(_tmp25A == _tmp25C  || _tmp25A->vd == 
_tmp25C->vd  && Cyc_CfFlowInfo_same_relop(_tmp25A->rop,_tmp25C->rop)){{struct Cyc_List_List*
_tmp5D0;res=((_tmp5D0=_region_malloc(r,sizeof(*_tmp5D0)),((_tmp5D0->hd=_tmp25A,((
_tmp5D0->tl=res,_tmp5D0))))));}found=1;break;}}}if(!found)diff=1;}}if(!diff)res=
r1s;return res;};}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2){struct _tuple21 _tmp5D1;struct _tuple21 _tmp25F=(_tmp5D1.f1=f1,((_tmp5D1.f2=f2,
_tmp5D1)));union Cyc_CfFlowInfo_FlowInfo _tmp260;int _tmp261;union Cyc_CfFlowInfo_FlowInfo
_tmp262;int _tmp263;union Cyc_CfFlowInfo_FlowInfo _tmp264;struct _tuple11 _tmp265;
struct Cyc_Dict_Dict _tmp266;struct Cyc_List_List*_tmp267;union Cyc_CfFlowInfo_FlowInfo
_tmp268;struct _tuple11 _tmp269;struct Cyc_Dict_Dict _tmp26A;struct Cyc_List_List*
_tmp26B;_LL106: _tmp260=_tmp25F.f1;if((_tmp260.BottomFL).tag != 1)goto _LL108;
_tmp261=(int)(_tmp260.BottomFL).val;_LL107: return f2;_LL108: _tmp262=_tmp25F.f2;
if((_tmp262.BottomFL).tag != 1)goto _LL10A;_tmp263=(int)(_tmp262.BottomFL).val;
_LL109: return f1;_LL10A: _tmp264=_tmp25F.f1;if((_tmp264.ReachableFL).tag != 2)goto
_LL105;_tmp265=(struct _tuple11)(_tmp264.ReachableFL).val;_tmp266=_tmp265.f1;
_tmp267=_tmp265.f2;_tmp268=_tmp25F.f2;if((_tmp268.ReachableFL).tag != 2)goto
_LL105;_tmp269=(struct _tuple11)(_tmp268.ReachableFL).val;_tmp26A=_tmp269.f1;
_tmp26B=_tmp269.f2;_LL10B: if(_tmp266.t == _tmp26A.t  && _tmp267 == _tmp26B)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f2,f1))return f1;{struct _RegionHandle*_tmp26C=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp5D4;struct Cyc_CfFlowInfo_JoinEnv _tmp5D3;struct Cyc_CfFlowInfo_JoinEnv _tmp26D=(
_tmp5D3.fenv=fenv,((_tmp5D3.pile=((_tmp5D4=_region_malloc(_tmp26C,sizeof(*
_tmp5D4)),((_tmp5D4->rgn=_tmp26C,((_tmp5D4->places=0,_tmp5D4)))))),((_tmp5D3.d1=
_tmp266,((_tmp5D3.d2=_tmp26A,_tmp5D3)))))));struct Cyc_Dict_Dict _tmp26E=((struct
Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),
struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))
Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,
void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp26D,_tmp266,_tmp26A);struct Cyc_List_List*
_tmp26F=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp267,_tmp26B);return Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_escape_these(fenv,_tmp26D.pile,all_changed,_tmp26E),_tmp26F);};
_LL105:;}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;union Cyc_CfFlowInfo_FlowInfo f3;};struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple12
pr1,struct _tuple12 pr2){union Cyc_CfFlowInfo_FlowInfo _tmp273;void*_tmp274;struct
_tuple12 _tmp272=pr1;_tmp273=_tmp272.f1;_tmp274=_tmp272.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp276;void*_tmp277;struct _tuple12 _tmp275=pr2;_tmp276=_tmp275.f1;_tmp277=
_tmp275.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,
all_changed,_tmp273,_tmp276);struct _tuple22 _tmp5D5;struct _tuple22 _tmp279=(
_tmp5D5.f1=_tmp273,((_tmp5D5.f2=_tmp276,((_tmp5D5.f3=outflow,_tmp5D5)))));union
Cyc_CfFlowInfo_FlowInfo _tmp27A;int _tmp27B;union Cyc_CfFlowInfo_FlowInfo _tmp27C;
int _tmp27D;union Cyc_CfFlowInfo_FlowInfo _tmp27E;struct _tuple11 _tmp27F;struct Cyc_Dict_Dict
_tmp280;union Cyc_CfFlowInfo_FlowInfo _tmp281;struct _tuple11 _tmp282;struct Cyc_Dict_Dict
_tmp283;union Cyc_CfFlowInfo_FlowInfo _tmp284;struct _tuple11 _tmp285;struct Cyc_Dict_Dict
_tmp286;struct Cyc_List_List*_tmp287;_LL10D: _tmp27A=_tmp279.f1;if((_tmp27A.BottomFL).tag
!= 1)goto _LL10F;_tmp27B=(int)(_tmp27A.BottomFL).val;_LL10E: {struct _tuple12
_tmp5D6;return(_tmp5D6.f1=outflow,((_tmp5D6.f2=_tmp277,_tmp5D6)));}_LL10F:
_tmp27C=_tmp279.f2;if((_tmp27C.BottomFL).tag != 1)goto _LL111;_tmp27D=(int)(
_tmp27C.BottomFL).val;_LL110: {struct _tuple12 _tmp5D7;return(_tmp5D7.f1=outflow,((
_tmp5D7.f2=_tmp274,_tmp5D7)));}_LL111: _tmp27E=_tmp279.f1;if((_tmp27E.ReachableFL).tag
!= 2)goto _LL113;_tmp27F=(struct _tuple11)(_tmp27E.ReachableFL).val;_tmp280=
_tmp27F.f1;_tmp281=_tmp279.f2;if((_tmp281.ReachableFL).tag != 2)goto _LL113;
_tmp282=(struct _tuple11)(_tmp281.ReachableFL).val;_tmp283=_tmp282.f1;_tmp284=
_tmp279.f3;if((_tmp284.ReachableFL).tag != 2)goto _LL113;_tmp285=(struct _tuple11)(
_tmp284.ReachableFL).val;_tmp286=_tmp285.f1;_tmp287=_tmp285.f2;_LL112: if(((int(*)(
int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp274,
_tmp277)){struct _tuple12 _tmp5D8;return(_tmp5D8.f1=outflow,((_tmp5D8.f2=_tmp277,
_tmp5D8)));}if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp277,_tmp274)){struct _tuple12 _tmp5D9;return(_tmp5D9.f1=outflow,((_tmp5D9.f2=
_tmp274,_tmp5D9)));}{struct _RegionHandle*_tmp28C=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp5DC;struct Cyc_CfFlowInfo_JoinEnv _tmp5DB;struct Cyc_CfFlowInfo_JoinEnv _tmp28D=(
_tmp5DB.fenv=fenv,((_tmp5DB.pile=((_tmp5DC=_region_malloc(_tmp28C,sizeof(*
_tmp5DC)),((_tmp5DC->rgn=_tmp28C,((_tmp5DC->places=0,_tmp5DC)))))),((_tmp5DB.d1=
_tmp280,((_tmp5DB.d2=_tmp283,_tmp5DB)))))));void*_tmp28E=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp28D,0,_tmp274,
_tmp277);struct _tuple12 _tmp5DD;return(_tmp5DD.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp28D.pile,all_changed,_tmp286),_tmp287),((_tmp5DD.f2=_tmp28E,_tmp5DD)));};
_LL113:;_LL114: {struct Cyc_Core_Impossible_struct _tmp5E3;const char*_tmp5E2;
struct Cyc_Core_Impossible_struct*_tmp5E1;(int)_throw((void*)((_tmp5E1=_cycalloc(
sizeof(*_tmp5E1)),((_tmp5E1[0]=((_tmp5E3.tag=Cyc_Core_Impossible,((_tmp5E3.f1=((
_tmp5E2="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp5E2,sizeof(
char),37))),_tmp5E3)))),_tmp5E1)))));}_LL10C:;};};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp5EA(struct Cyc_CfFlowInfo_AfterEnv**
env,unsigned int*_tmp5E9,unsigned int*_tmp5E8,void***_tmp5E6,struct
_dyneither_ptr*_tmp2A2,struct _dyneither_ptr*_tmp29E){for(*_tmp5E9=0;*_tmp5E9 < *
_tmp5E8;(*_tmp5E9)++){(*_tmp5E6)[*_tmp5E9]=Cyc_CfFlowInfo_after_absRval_d(*env,(
int)*_tmp5E9,((void**)(*_tmp29E).curr)[(int)*_tmp5E9],*((void**)
_check_dyneither_subscript(*_tmp2A2,sizeof(void*),(int)*_tmp5E9)));}}static void*
Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);int
changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);if(
changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*
r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);if(changed1){if(
!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))return r1;env->changed=
Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,
env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct _tuple0 _tmp5E4;
struct _tuple0 _tmp296=(_tmp5E4.f1=r1,((_tmp5E4.f2=r2,_tmp5E4)));void*_tmp297;void*
_tmp299;void*_tmp29B;struct Cyc_CfFlowInfo_UnionRInfo _tmp29D;struct _dyneither_ptr
_tmp29E;void*_tmp29F;struct Cyc_CfFlowInfo_UnionRInfo _tmp2A1;struct _dyneither_ptr
_tmp2A2;_LL116: _tmp297=_tmp296.f1;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp298=(
struct Cyc_CfFlowInfo_Consumed_struct*)_tmp297;if(_tmp298->tag != 7)goto _LL118;};
_LL117: goto _LL119;_LL118: _tmp299=_tmp296.f2;{struct Cyc_CfFlowInfo_Consumed_struct*
_tmp29A=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp299;if(_tmp29A->tag != 7)goto
_LL11A;};_LL119: return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,
void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL11A: _tmp29B=
_tmp296.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp29C=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp29B;if(_tmp29C->tag != 6)goto _LL11C;else{_tmp29D=_tmp29C->f1;_tmp29E=_tmp29C->f2;}};
_tmp29F=_tmp296.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2A0=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp29F;if(_tmp2A0->tag != 6)goto _LL11C;else{_tmp2A1=_tmp2A0->f1;_tmp2A2=_tmp2A0->f2;}};
_LL11B: {unsigned int _tmp5E9;unsigned int _tmp5E8;struct _dyneither_ptr _tmp5E7;
void**_tmp5E6;unsigned int _tmp5E5;struct _dyneither_ptr new_d=(_tmp5E5=
_get_dyneither_size(_tmp29E,sizeof(void*)),((_tmp5E6=(void**)_region_malloc(((
env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp5E5)),((_tmp5E7=
_tag_dyneither(_tmp5E6,sizeof(void*),_tmp5E5),((((_tmp5E8=_tmp5E5,_tmp5EA(& env,&
_tmp5E9,& _tmp5E8,& _tmp5E6,& _tmp2A2,& _tmp29E))),_tmp5E7)))))));int change=0;{int i=
0;for(0;i < _get_dyneither_size(_tmp29E,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp29E.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp29E,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp2A2,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5ED;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5EC;return(void*)((_tmp5EC=_region_malloc(((env->joinenv).fenv)->r,sizeof(*
_tmp5EC)),((_tmp5EC[0]=((_tmp5ED.tag=6,((_tmp5ED.f1=_tmp29D,((_tmp5ED.f2=new_d,
_tmp5ED)))))),_tmp5EC))));};}_LL11C:;_LL11D: {struct Cyc_Core_Impossible_struct
_tmp5F3;const char*_tmp5F2;struct Cyc_Core_Impossible_struct*_tmp5F1;(int)_throw((
void*)((_tmp5F1=_cycalloc(sizeof(*_tmp5F1)),((_tmp5F1[0]=((_tmp5F3.tag=Cyc_Core_Impossible,((
_tmp5F3.f1=((_tmp5F2="after_absRval -- non-aggregates!",_tag_dyneither(_tmp5F2,
sizeof(char),33))),_tmp5F3)))),_tmp5F1)))));}_LL115:;};}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp2AC=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
_tmp2AD=env->changed;{struct Cyc_List_List*_tmp5F4;*env->last_field_cell=((
_tmp5F4=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp5F4)),((_tmp5F4->hd=(
void*)key,((_tmp5F4->tl=0,_tmp5F4))))));}env->last_field_cell=(struct Cyc_List_List**)&((
struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp2AF=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp2AC;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp2AD;return _tmp2AF;};};}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_CfFlowInfo_Place _tmp5F5;*env->curr_place=((_tmp5F5.root=root,((_tmp5F5.fields=
0,_tmp5F5))));}env->last_field_cell=(struct Cyc_List_List**)&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){struct _tuple21 _tmp5F6;struct
_tuple21 _tmp2B2=(_tmp5F6.f1=f1,((_tmp5F6.f2=f2,_tmp5F6)));union Cyc_CfFlowInfo_FlowInfo
_tmp2B3;int _tmp2B4;union Cyc_CfFlowInfo_FlowInfo _tmp2B5;int _tmp2B6;union Cyc_CfFlowInfo_FlowInfo
_tmp2B7;struct _tuple11 _tmp2B8;struct Cyc_Dict_Dict _tmp2B9;struct Cyc_List_List*
_tmp2BA;union Cyc_CfFlowInfo_FlowInfo _tmp2BB;struct _tuple11 _tmp2BC;struct Cyc_Dict_Dict
_tmp2BD;struct Cyc_List_List*_tmp2BE;_LL11F: _tmp2B3=_tmp2B2.f1;if((_tmp2B3.BottomFL).tag
!= 1)goto _LL121;_tmp2B4=(int)(_tmp2B3.BottomFL).val;_LL120: goto _LL122;_LL121:
_tmp2B5=_tmp2B2.f2;if((_tmp2B5.BottomFL).tag != 1)goto _LL123;_tmp2B6=(int)(
_tmp2B5.BottomFL).val;_LL122: return Cyc_CfFlowInfo_BottomFL();_LL123: _tmp2B7=
_tmp2B2.f1;if((_tmp2B7.ReachableFL).tag != 2)goto _LL11E;_tmp2B8=(struct _tuple11)(
_tmp2B7.ReachableFL).val;_tmp2B9=_tmp2B8.f1;_tmp2BA=_tmp2B8.f2;_tmp2BB=_tmp2B2.f2;
if((_tmp2BB.ReachableFL).tag != 2)goto _LL11E;_tmp2BC=(struct _tuple11)(_tmp2BB.ReachableFL).val;
_tmp2BD=_tmp2BC.f1;_tmp2BE=_tmp2BC.f2;_LL124: if(_tmp2B9.t == _tmp2BD.t  && _tmp2BA
== _tmp2BE)return f1;{struct _RegionHandle*_tmp2BF=fenv->r;struct Cyc_CfFlowInfo_Place*
_tmp2C0=fenv->dummy_place;struct Cyc_CfFlowInfo_JoinEnv _tmp5FC;struct Cyc_CfFlowInfo_EscPile*
_tmp5FB;struct Cyc_CfFlowInfo_AfterEnv _tmp5FA;struct Cyc_CfFlowInfo_AfterEnv
_tmp2C1=(_tmp5FA.joinenv=((_tmp5FC.fenv=fenv,((_tmp5FC.pile=((_tmp5FB=
_region_malloc(_tmp2BF,sizeof(*_tmp5FB)),((_tmp5FB->rgn=_tmp2BF,((_tmp5FB->places=
0,_tmp5FB)))))),((_tmp5FC.d1=_tmp2B9,((_tmp5FC.d2=_tmp2BD,_tmp5FC)))))))),((
_tmp5FA.chg1=chg1,((_tmp5FA.chg2=chg2,((_tmp5FA.curr_place=_tmp2C0,((_tmp5FA.last_field_cell=(
struct Cyc_List_List**)& _tmp2C0->fields,((_tmp5FA.changed=Cyc_CfFlowInfo_Neither,
_tmp5FA)))))))))));struct Cyc_Dict_Dict _tmp2C2=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp2C1,_tmp2B9,_tmp2BD);struct Cyc_List_List*_tmp2C3=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp2BA,_tmp2BE);return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,(_tmp2C1.joinenv).pile,all_changed,_tmp2C2),_tmp2C3);};_LL11E:;}static int
Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){if(r1 == r2)
return 1;{struct _tuple0 _tmp5FD;struct _tuple0 _tmp2C8=(_tmp5FD.f1=r1,((_tmp5FD.f2=
r2,_tmp5FD)));void*_tmp2C9;struct Cyc_CfFlowInfo_Place*_tmp2CB;void*_tmp2CC;
struct Cyc_CfFlowInfo_Place*_tmp2CE;void*_tmp2CF;void*_tmp2D1;void*_tmp2D3;struct
Cyc_CfFlowInfo_UnionRInfo _tmp2D5;struct _dyneither_ptr _tmp2D6;void*_tmp2D7;struct
Cyc_CfFlowInfo_UnionRInfo _tmp2D9;struct _dyneither_ptr _tmp2DA;void*_tmp2DB;void*
_tmp2DD;void*_tmp2DF;void*_tmp2E1;void*_tmp2E3;void*_tmp2E5;void*_tmp2E7;void*
_tmp2E9;_LL126: _tmp2C9=_tmp2C8.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp2CA=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2C9;if(_tmp2CA->tag != 5)goto _LL128;
else{_tmp2CB=_tmp2CA->f1;}};_tmp2CC=_tmp2C8.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2CD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2CC;if(_tmp2CD->tag != 5)goto
_LL128;else{_tmp2CE=_tmp2CD->f1;}};_LL127: return Cyc_CfFlowInfo_place_cmp(_tmp2CB,
_tmp2CE)== 0;_LL128: _tmp2CF=_tmp2C8.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2D0=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2CF;if(_tmp2D0->tag != 5)goto
_LL12A;};_LL129: goto _LL12B;_LL12A: _tmp2D1=_tmp2C8.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2D2=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2D1;if(_tmp2D2->tag != 5)goto
_LL12C;};_LL12B: return 0;_LL12C: _tmp2D3=_tmp2C8.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2D4=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2D3;if(_tmp2D4->tag != 6)goto
_LL12E;else{_tmp2D5=_tmp2D4->f1;_tmp2D6=_tmp2D4->f2;}};_tmp2D7=_tmp2C8.f2;{
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2D8=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2D7;if(_tmp2D8->tag != 6)goto _LL12E;else{_tmp2D9=_tmp2D8->f1;_tmp2DA=_tmp2D8->f2;}};
_LL12D: if((void**)_tmp2D6.curr == (void**)_tmp2DA.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp2D6,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2D6.curr)[i],*((
void**)_check_dyneither_subscript(_tmp2DA,sizeof(void*),i))))return 0;}}return 1;
_LL12E: _tmp2DB=_tmp2C8.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2DC=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp2DB;if(_tmp2DC->tag != 1)goto _LL130;};
_tmp2DD=_tmp2C8.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2DE=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2DD;if(_tmp2DE->tag != 2)goto _LL130;};_LL12F: return 1;_LL130: _tmp2DF=_tmp2C8.f2;{
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp2E0=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp2DF;if(_tmp2E0->tag != 2)goto _LL132;};_LL131: return 0;_LL132: _tmp2E1=_tmp2C8.f2;{
struct Cyc_CfFlowInfo_Zero_struct*_tmp2E2=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp2E1;if(_tmp2E2->tag != 0)goto _LL134;};_LL133: goto _LL135;_LL134: _tmp2E3=
_tmp2C8.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp2E4=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp2E3;if(_tmp2E4->tag != 1)goto _LL136;};_LL135: return 0;_LL136: _tmp2E5=_tmp2C8.f1;{
struct Cyc_CfFlowInfo_Esc_struct*_tmp2E6=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp2E5;if(_tmp2E6->tag != 4)goto _LL138;};_tmp2E7=_tmp2C8.f2;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp2E8=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2E7;if(_tmp2E8->tag != 4)goto _LL138;};
_LL137: goto _LL125;_LL138: _tmp2E9=_tmp2C8.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp2EA=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp2E9;if(_tmp2EA->tag != 4)goto _LL13A;};
_LL139: return 0;_LL13A:;_LL13B: goto _LL125;_LL125:;}{struct _tuple19 _tmp5FE;struct
_tuple19 _tmp2EC=(_tmp5FE.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp5FE.f2=Cyc_CfFlowInfo_initlevel_approx(
r2),_tmp5FE)));enum Cyc_CfFlowInfo_InitLevel _tmp2ED;enum Cyc_CfFlowInfo_InitLevel
_tmp2EE;enum Cyc_CfFlowInfo_InitLevel _tmp2EF;enum Cyc_CfFlowInfo_InitLevel
_tmp2F0;enum Cyc_CfFlowInfo_InitLevel _tmp2F1;enum Cyc_CfFlowInfo_InitLevel
_tmp2F2;_LL13D: _tmp2ED=_tmp2EC.f1;if(_tmp2ED != Cyc_CfFlowInfo_AllIL)goto _LL13F;
_tmp2EE=_tmp2EC.f2;if(_tmp2EE != Cyc_CfFlowInfo_AllIL)goto _LL13F;_LL13E: return 1;
_LL13F: _tmp2EF=_tmp2EC.f2;if(_tmp2EF != Cyc_CfFlowInfo_NoneIL)goto _LL141;_LL140:
return 1;_LL141: _tmp2F0=_tmp2EC.f1;if(_tmp2F0 != Cyc_CfFlowInfo_NoneIL)goto _LL143;
_LL142: return 0;_LL143: _tmp2F1=_tmp2EC.f2;if(_tmp2F1 != Cyc_CfFlowInfo_ThisIL)goto
_LL145;_LL144: return 1;_LL145: _tmp2F2=_tmp2EC.f1;if(_tmp2F2 != Cyc_CfFlowInfo_ThisIL)
goto _LL13C;_LL146: return 0;_LL13C:;};}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s != 0;
r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp2F3=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;
int found=0;{struct Cyc_List_List*_tmp2F4=r2s;for(0;_tmp2F4 != 0;_tmp2F4=_tmp2F4->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2F5=(struct Cyc_CfFlowInfo_Reln*)_tmp2F4->hd;if(
_tmp2F3 == _tmp2F5  || _tmp2F3->vd == _tmp2F5->vd  && Cyc_CfFlowInfo_same_relop(
_tmp2F3->rop,_tmp2F5->rop)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){struct _tuple21
_tmp5FF;struct _tuple21 _tmp2F7=(_tmp5FF.f1=f1,((_tmp5FF.f2=f2,_tmp5FF)));union Cyc_CfFlowInfo_FlowInfo
_tmp2F8;int _tmp2F9;union Cyc_CfFlowInfo_FlowInfo _tmp2FA;int _tmp2FB;union Cyc_CfFlowInfo_FlowInfo
_tmp2FC;struct _tuple11 _tmp2FD;struct Cyc_Dict_Dict _tmp2FE;struct Cyc_List_List*
_tmp2FF;union Cyc_CfFlowInfo_FlowInfo _tmp300;struct _tuple11 _tmp301;struct Cyc_Dict_Dict
_tmp302;struct Cyc_List_List*_tmp303;_LL148: _tmp2F8=_tmp2F7.f1;if((_tmp2F8.BottomFL).tag
!= 1)goto _LL14A;_tmp2F9=(int)(_tmp2F8.BottomFL).val;_LL149: return 1;_LL14A:
_tmp2FA=_tmp2F7.f2;if((_tmp2FA.BottomFL).tag != 1)goto _LL14C;_tmp2FB=(int)(
_tmp2FA.BottomFL).val;_LL14B: return 0;_LL14C: _tmp2FC=_tmp2F7.f1;if((_tmp2FC.ReachableFL).tag
!= 2)goto _LL147;_tmp2FD=(struct _tuple11)(_tmp2FC.ReachableFL).val;_tmp2FE=
_tmp2FD.f1;_tmp2FF=_tmp2FD.f2;_tmp300=_tmp2F7.f2;if((_tmp300.ReachableFL).tag != 
2)goto _LL147;_tmp301=(struct _tuple11)(_tmp300.ReachableFL).val;_tmp302=_tmp301.f1;
_tmp303=_tmp301.f2;_LL14D: if(_tmp2FE.t == _tmp302.t  && _tmp2FF == _tmp303)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp2FE,_tmp302) && Cyc_CfFlowInfo_relns_approx(_tmp2FF,_tmp303);_LL147:;}struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*
rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=0;for(p=rs;!found
 && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp304=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp304->vd == v){found=1;break;}{union Cyc_CfFlowInfo_RelnOp _tmp305=
_tmp304->rop;struct _tuple10 _tmp306;struct Cyc_Absyn_Vardecl*_tmp307;struct Cyc_Absyn_Vardecl*
_tmp308;struct Cyc_Absyn_Vardecl*_tmp309;_LL14F: if((_tmp305.LessVar).tag != 2)goto
_LL151;_tmp306=(struct _tuple10)(_tmp305.LessVar).val;_tmp307=_tmp306.f1;_LL150:
_tmp308=_tmp307;goto _LL152;_LL151: if((_tmp305.LessNumelts).tag != 3)goto _LL153;
_tmp308=(struct Cyc_Absyn_Vardecl*)(_tmp305.LessNumelts).val;_LL152: _tmp309=
_tmp308;goto _LL154;_LL153: if((_tmp305.LessEqNumelts).tag != 5)goto _LL155;_tmp309=(
struct Cyc_Absyn_Vardecl*)(_tmp305.LessEqNumelts).val;_LL154: if(v == _tmp309)found=
1;goto _LL14E;_LL155:;_LL156: goto _LL14E;_LL14E:;};}if(!found)return rs;{struct Cyc_List_List*
_tmp30A=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp30B=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp30B->vd != v){{union Cyc_CfFlowInfo_RelnOp _tmp30C=_tmp30B->rop;struct
_tuple10 _tmp30D;struct Cyc_Absyn_Vardecl*_tmp30E;struct Cyc_Absyn_Vardecl*_tmp30F;
struct Cyc_Absyn_Vardecl*_tmp310;_LL158: if((_tmp30C.LessVar).tag != 2)goto _LL15A;
_tmp30D=(struct _tuple10)(_tmp30C.LessVar).val;_tmp30E=_tmp30D.f1;_LL159: _tmp30F=
_tmp30E;goto _LL15B;_LL15A: if((_tmp30C.LessNumelts).tag != 3)goto _LL15C;_tmp30F=(
struct Cyc_Absyn_Vardecl*)(_tmp30C.LessNumelts).val;_LL15B: _tmp310=_tmp30F;goto
_LL15D;_LL15C: if((_tmp30C.LessEqNumelts).tag != 5)goto _LL15E;_tmp310=(struct Cyc_Absyn_Vardecl*)(
_tmp30C.LessEqNumelts).val;_LL15D: if(v == _tmp310)continue;goto _LL157;_LL15E:;
_LL15F: goto _LL157;_LL157:;}{struct Cyc_List_List*_tmp600;_tmp30A=((_tmp600=
_region_malloc(rgn,sizeof(*_tmp600)),((_tmp600->hd=_tmp30B,((_tmp600->tl=_tmp30A,
_tmp600))))));};}}return _tmp30A;};}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*
_tmp312=e->r;void*_tmp314;struct Cyc_Absyn_Vardecl*_tmp316;void*_tmp318;struct Cyc_Absyn_Vardecl*
_tmp31A;void*_tmp31C;struct Cyc_Absyn_Vardecl*_tmp31E;void*_tmp320;struct Cyc_Absyn_Vardecl*
_tmp322;_LL161: {struct Cyc_Absyn_Var_e_struct*_tmp313=(struct Cyc_Absyn_Var_e_struct*)
_tmp312;if(_tmp313->tag != 1)goto _LL163;else{_tmp314=(void*)_tmp313->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp315=(struct Cyc_Absyn_Global_b_struct*)_tmp314;if(
_tmp315->tag != 1)goto _LL163;else{_tmp316=_tmp315->f1;}};}}_LL162: _tmp31A=_tmp316;
goto _LL164;_LL163: {struct Cyc_Absyn_Var_e_struct*_tmp317=(struct Cyc_Absyn_Var_e_struct*)
_tmp312;if(_tmp317->tag != 1)goto _LL165;else{_tmp318=(void*)_tmp317->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp319=(struct Cyc_Absyn_Param_b_struct*)_tmp318;if(
_tmp319->tag != 3)goto _LL165;else{_tmp31A=_tmp319->f1;}};}}_LL164: _tmp31E=_tmp31A;
goto _LL166;_LL165: {struct Cyc_Absyn_Var_e_struct*_tmp31B=(struct Cyc_Absyn_Var_e_struct*)
_tmp312;if(_tmp31B->tag != 1)goto _LL167;else{_tmp31C=(void*)_tmp31B->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp31D=(struct Cyc_Absyn_Local_b_struct*)_tmp31C;if(
_tmp31D->tag != 4)goto _LL167;else{_tmp31E=_tmp31D->f1;}};}}_LL166: _tmp322=_tmp31E;
goto _LL168;_LL167: {struct Cyc_Absyn_Var_e_struct*_tmp31F=(struct Cyc_Absyn_Var_e_struct*)
_tmp312;if(_tmp31F->tag != 1)goto _LL169;else{_tmp320=(void*)_tmp31F->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp321=(struct Cyc_Absyn_Pat_b_struct*)_tmp320;if(_tmp321->tag
!= 5)goto _LL169;else{_tmp322=_tmp321->f1;}};}}_LL168: if(!_tmp322->escapes)return
Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp322);goto _LL160;_LL169:;_LL16A: goto _LL160;
_LL160:;}return r;}struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct
_RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){struct Cyc_CfFlowInfo_Reln*_tmp601;
return(_tmp601=_region_malloc(r2,sizeof(*_tmp601)),((_tmp601[0]=*r,_tmp601)));}
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*
relns){return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*
f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){for(
0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct Cyc_CfFlowInfo_Reln*_tmp324=(
struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*_tmp325=(struct Cyc_CfFlowInfo_Reln*)
r2->hd;if(_tmp324->vd != _tmp325->vd  || !Cyc_CfFlowInfo_same_relop(_tmp324->rop,
_tmp325->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(
rgn,r,v);{void*_tmp326=e->r;struct Cyc_Absyn_MallocInfo _tmp328;struct Cyc_Absyn_Exp*
_tmp329;int _tmp32A;_LL16C: {struct Cyc_Absyn_Malloc_e_struct*_tmp327=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp326;if(_tmp327->tag != 34)goto _LL16E;else{_tmp328=_tmp327->f1;_tmp329=_tmp328.num_elts;
_tmp32A=_tmp328.fat_result;if(_tmp32A != 1)goto _LL16E;}}_LL16D: malloc_loop: {void*
_tmp32B=_tmp329->r;struct Cyc_Absyn_Exp*_tmp32D;void*_tmp32F;struct Cyc_Absyn_Vardecl*
_tmp331;void*_tmp333;struct Cyc_Absyn_Vardecl*_tmp335;void*_tmp337;struct Cyc_Absyn_Vardecl*
_tmp339;void*_tmp33B;struct Cyc_Absyn_Vardecl*_tmp33D;_LL171: {struct Cyc_Absyn_Cast_e_struct*
_tmp32C=(struct Cyc_Absyn_Cast_e_struct*)_tmp32B;if(_tmp32C->tag != 15)goto _LL173;
else{_tmp32D=_tmp32C->f2;}}_LL172: _tmp329=_tmp32D;goto malloc_loop;_LL173: {
struct Cyc_Absyn_Var_e_struct*_tmp32E=(struct Cyc_Absyn_Var_e_struct*)_tmp32B;if(
_tmp32E->tag != 1)goto _LL175;else{_tmp32F=(void*)_tmp32E->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp330=(struct Cyc_Absyn_Pat_b_struct*)_tmp32F;if(_tmp330->tag != 5)goto _LL175;
else{_tmp331=_tmp330->f1;}};}}_LL174: _tmp335=_tmp331;goto _LL176;_LL175: {struct
Cyc_Absyn_Var_e_struct*_tmp332=(struct Cyc_Absyn_Var_e_struct*)_tmp32B;if(_tmp332->tag
!= 1)goto _LL177;else{_tmp333=(void*)_tmp332->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp334=(struct Cyc_Absyn_Local_b_struct*)_tmp333;if(_tmp334->tag != 4)goto _LL177;
else{_tmp335=_tmp334->f1;}};}}_LL176: _tmp339=_tmp335;goto _LL178;_LL177: {struct
Cyc_Absyn_Var_e_struct*_tmp336=(struct Cyc_Absyn_Var_e_struct*)_tmp32B;if(_tmp336->tag
!= 1)goto _LL179;else{_tmp337=(void*)_tmp336->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp338=(struct Cyc_Absyn_Param_b_struct*)_tmp337;if(_tmp338->tag != 3)goto _LL179;
else{_tmp339=_tmp338->f1;}};}}_LL178: _tmp33D=_tmp339;goto _LL17A;_LL179: {struct
Cyc_Absyn_Var_e_struct*_tmp33A=(struct Cyc_Absyn_Var_e_struct*)_tmp32B;if(_tmp33A->tag
!= 1)goto _LL17B;else{_tmp33B=(void*)_tmp33A->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp33C=(struct Cyc_Absyn_Global_b_struct*)_tmp33B;if(_tmp33C->tag != 1)goto _LL17B;
else{_tmp33D=_tmp33C->f1;}};}}_LL17A: if(_tmp33D->escapes)return r;{struct Cyc_CfFlowInfo_Reln*
_tmp604;struct Cyc_List_List*_tmp603;return(_tmp603=_region_malloc(rgn,sizeof(*
_tmp603)),((_tmp603->hd=((_tmp604=_region_malloc(rgn,sizeof(*_tmp604)),((_tmp604->vd=
_tmp33D,((_tmp604->rop=Cyc_CfFlowInfo_LessEqNumelts(v),_tmp604)))))),((_tmp603->tl=
r,_tmp603)))));};_LL17B:;_LL17C: return r;_LL170:;}_LL16E:;_LL16F: goto _LL16B;
_LL16B:;}{void*_tmp340=Cyc_Tcutil_compress(v->type);_LL17E: {struct Cyc_Absyn_IntType_struct*
_tmp341=(struct Cyc_Absyn_IntType_struct*)_tmp340;if(_tmp341->tag != 6)goto _LL180;}
_LL17F: goto _LL17D;_LL180:;_LL181: return r;_LL17D:;}loop: {void*_tmp342=e->r;
struct Cyc_Absyn_Exp*_tmp344;union Cyc_Absyn_Cnst _tmp346;struct _tuple6 _tmp347;int
_tmp348;enum Cyc_Absyn_Primop _tmp34A;struct Cyc_List_List*_tmp34B;struct Cyc_List_List
_tmp34C;struct Cyc_List_List*_tmp34D;struct Cyc_List_List _tmp34E;struct Cyc_Absyn_Exp*
_tmp34F;enum Cyc_Absyn_Primop _tmp351;struct Cyc_List_List*_tmp352;struct Cyc_List_List
_tmp353;struct Cyc_Absyn_Exp*_tmp354;_LL183: {struct Cyc_Absyn_Cast_e_struct*
_tmp343=(struct Cyc_Absyn_Cast_e_struct*)_tmp342;if(_tmp343->tag != 15)goto _LL185;
else{_tmp344=_tmp343->f2;}}_LL184: e=_tmp344;goto loop;_LL185: {struct Cyc_Absyn_Const_e_struct*
_tmp345=(struct Cyc_Absyn_Const_e_struct*)_tmp342;if(_tmp345->tag != 0)goto _LL187;
else{_tmp346=_tmp345->f1;if((_tmp346.Int_c).tag != 4)goto _LL187;_tmp347=(struct
_tuple6)(_tmp346.Int_c).val;_tmp348=_tmp347.f2;}}_LL186: {struct Cyc_CfFlowInfo_Reln*
_tmp607;struct Cyc_List_List*_tmp606;return(_tmp606=_region_malloc(rgn,sizeof(*
_tmp606)),((_tmp606->hd=((_tmp607=_region_malloc(rgn,sizeof(*_tmp607)),((_tmp607->vd=
v,((_tmp607->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp348),_tmp607)))))),((
_tmp606->tl=r,_tmp606)))));}_LL187: {struct Cyc_Absyn_Primop_e_struct*_tmp349=(
struct Cyc_Absyn_Primop_e_struct*)_tmp342;if(_tmp349->tag != 3)goto _LL189;else{
_tmp34A=_tmp349->f1;if(_tmp34A != Cyc_Absyn_Mod)goto _LL189;_tmp34B=_tmp349->f2;
if(_tmp34B == 0)goto _LL189;_tmp34C=*_tmp34B;_tmp34D=_tmp34C.tl;if(_tmp34D == 0)
goto _LL189;_tmp34E=*_tmp34D;_tmp34F=(struct Cyc_Absyn_Exp*)_tmp34E.hd;}}_LL188:{
void*_tmp357=_tmp34F->r;enum Cyc_Absyn_Primop _tmp359;struct Cyc_List_List*_tmp35A;
struct Cyc_List_List _tmp35B;struct Cyc_Absyn_Exp*_tmp35C;_LL18E: {struct Cyc_Absyn_Primop_e_struct*
_tmp358=(struct Cyc_Absyn_Primop_e_struct*)_tmp357;if(_tmp358->tag != 3)goto _LL190;
else{_tmp359=_tmp358->f1;_tmp35A=_tmp358->f2;if(_tmp35A == 0)goto _LL190;_tmp35B=*
_tmp35A;_tmp35C=(struct Cyc_Absyn_Exp*)_tmp35B.hd;}}_LL18F:{void*_tmp35D=_tmp35C->r;
void*_tmp35F;struct Cyc_Absyn_Vardecl*_tmp361;void*_tmp363;struct Cyc_Absyn_Vardecl*
_tmp365;void*_tmp367;struct Cyc_Absyn_Vardecl*_tmp369;void*_tmp36B;struct Cyc_Absyn_Vardecl*
_tmp36D;_LL193: {struct Cyc_Absyn_Var_e_struct*_tmp35E=(struct Cyc_Absyn_Var_e_struct*)
_tmp35D;if(_tmp35E->tag != 1)goto _LL195;else{_tmp35F=(void*)_tmp35E->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp360=(struct Cyc_Absyn_Global_b_struct*)_tmp35F;if(
_tmp360->tag != 1)goto _LL195;else{_tmp361=_tmp360->f1;}};}}_LL194: _tmp365=_tmp361;
goto _LL196;_LL195: {struct Cyc_Absyn_Var_e_struct*_tmp362=(struct Cyc_Absyn_Var_e_struct*)
_tmp35D;if(_tmp362->tag != 1)goto _LL197;else{_tmp363=(void*)_tmp362->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp364=(struct Cyc_Absyn_Local_b_struct*)_tmp363;if(
_tmp364->tag != 4)goto _LL197;else{_tmp365=_tmp364->f1;}};}}_LL196: _tmp369=_tmp365;
goto _LL198;_LL197: {struct Cyc_Absyn_Var_e_struct*_tmp366=(struct Cyc_Absyn_Var_e_struct*)
_tmp35D;if(_tmp366->tag != 1)goto _LL199;else{_tmp367=(void*)_tmp366->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp368=(struct Cyc_Absyn_Param_b_struct*)_tmp367;if(
_tmp368->tag != 3)goto _LL199;else{_tmp369=_tmp368->f1;}};}}_LL198: _tmp36D=_tmp369;
goto _LL19A;_LL199: {struct Cyc_Absyn_Var_e_struct*_tmp36A=(struct Cyc_Absyn_Var_e_struct*)
_tmp35D;if(_tmp36A->tag != 1)goto _LL19B;else{_tmp36B=(void*)_tmp36A->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp36C=(struct Cyc_Absyn_Pat_b_struct*)_tmp36B;if(_tmp36C->tag
!= 5)goto _LL19B;else{_tmp36D=_tmp36C->f1;}};}}_LL19A: if(_tmp36D->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp60A;struct Cyc_List_List*_tmp609;return(_tmp609=
_region_malloc(rgn,sizeof(*_tmp609)),((_tmp609->hd=((_tmp60A=_region_malloc(rgn,
sizeof(*_tmp60A)),((_tmp60A->vd=v,((_tmp60A->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp36D),_tmp60A)))))),((_tmp609->tl=r,_tmp609)))));};_LL19B:;_LL19C: goto _LL192;
_LL192:;}goto _LL18D;_LL190:;_LL191: goto _LL18D;_LL18D:;}goto _LL182;_LL189: {
struct Cyc_Absyn_Primop_e_struct*_tmp350=(struct Cyc_Absyn_Primop_e_struct*)
_tmp342;if(_tmp350->tag != 3)goto _LL18B;else{_tmp351=_tmp350->f1;_tmp352=_tmp350->f2;
if(_tmp352 == 0)goto _LL18B;_tmp353=*_tmp352;_tmp354=(struct Cyc_Absyn_Exp*)_tmp353.hd;}}
_LL18A:{void*_tmp370=_tmp354->r;void*_tmp372;struct Cyc_Absyn_Vardecl*_tmp374;
void*_tmp376;struct Cyc_Absyn_Vardecl*_tmp378;void*_tmp37A;struct Cyc_Absyn_Vardecl*
_tmp37C;void*_tmp37E;struct Cyc_Absyn_Vardecl*_tmp380;_LL19E: {struct Cyc_Absyn_Var_e_struct*
_tmp371=(struct Cyc_Absyn_Var_e_struct*)_tmp370;if(_tmp371->tag != 1)goto _LL1A0;
else{_tmp372=(void*)_tmp371->f2;{struct Cyc_Absyn_Global_b_struct*_tmp373=(struct
Cyc_Absyn_Global_b_struct*)_tmp372;if(_tmp373->tag != 1)goto _LL1A0;else{_tmp374=
_tmp373->f1;}};}}_LL19F: _tmp378=_tmp374;goto _LL1A1;_LL1A0: {struct Cyc_Absyn_Var_e_struct*
_tmp375=(struct Cyc_Absyn_Var_e_struct*)_tmp370;if(_tmp375->tag != 1)goto _LL1A2;
else{_tmp376=(void*)_tmp375->f2;{struct Cyc_Absyn_Local_b_struct*_tmp377=(struct
Cyc_Absyn_Local_b_struct*)_tmp376;if(_tmp377->tag != 4)goto _LL1A2;else{_tmp378=
_tmp377->f1;}};}}_LL1A1: _tmp37C=_tmp378;goto _LL1A3;_LL1A2: {struct Cyc_Absyn_Var_e_struct*
_tmp379=(struct Cyc_Absyn_Var_e_struct*)_tmp370;if(_tmp379->tag != 1)goto _LL1A4;
else{_tmp37A=(void*)_tmp379->f2;{struct Cyc_Absyn_Param_b_struct*_tmp37B=(struct
Cyc_Absyn_Param_b_struct*)_tmp37A;if(_tmp37B->tag != 3)goto _LL1A4;else{_tmp37C=
_tmp37B->f1;}};}}_LL1A3: _tmp380=_tmp37C;goto _LL1A5;_LL1A4: {struct Cyc_Absyn_Var_e_struct*
_tmp37D=(struct Cyc_Absyn_Var_e_struct*)_tmp370;if(_tmp37D->tag != 1)goto _LL1A6;
else{_tmp37E=(void*)_tmp37D->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp37F=(struct Cyc_Absyn_Pat_b_struct*)
_tmp37E;if(_tmp37F->tag != 5)goto _LL1A6;else{_tmp380=_tmp37F->f1;}};}}_LL1A5: if(
_tmp380->escapes)return r;{struct Cyc_CfFlowInfo_Reln*_tmp60D;struct Cyc_List_List*
_tmp60C;return(_tmp60C=_region_malloc(rgn,sizeof(*_tmp60C)),((_tmp60C->hd=((
_tmp60D=_region_malloc(rgn,sizeof(*_tmp60D)),((_tmp60D->vd=v,((_tmp60D->rop=Cyc_CfFlowInfo_LessEqNumelts(
_tmp380),_tmp60D)))))),((_tmp60C->tl=r,_tmp60C)))));};_LL1A6:;_LL1A7: goto _LL19D;
_LL19D:;}goto _LL182;_LL18B:;_LL18C: goto _LL182;_LL182:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp383=e1->r;void*_tmp385;
struct Cyc_Absyn_Vardecl*_tmp387;void*_tmp389;struct Cyc_Absyn_Vardecl*_tmp38B;
void*_tmp38D;struct Cyc_Absyn_Vardecl*_tmp38F;void*_tmp391;struct Cyc_Absyn_Vardecl*
_tmp393;_LL1A9: {struct Cyc_Absyn_Var_e_struct*_tmp384=(struct Cyc_Absyn_Var_e_struct*)
_tmp383;if(_tmp384->tag != 1)goto _LL1AB;else{_tmp385=(void*)_tmp384->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp386=(struct Cyc_Absyn_Global_b_struct*)_tmp385;if(
_tmp386->tag != 1)goto _LL1AB;else{_tmp387=_tmp386->f1;}};}}_LL1AA: _tmp38B=_tmp387;
goto _LL1AC;_LL1AB: {struct Cyc_Absyn_Var_e_struct*_tmp388=(struct Cyc_Absyn_Var_e_struct*)
_tmp383;if(_tmp388->tag != 1)goto _LL1AD;else{_tmp389=(void*)_tmp388->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp38A=(struct Cyc_Absyn_Param_b_struct*)_tmp389;if(
_tmp38A->tag != 3)goto _LL1AD;else{_tmp38B=_tmp38A->f1;}};}}_LL1AC: _tmp38F=_tmp38B;
goto _LL1AE;_LL1AD: {struct Cyc_Absyn_Var_e_struct*_tmp38C=(struct Cyc_Absyn_Var_e_struct*)
_tmp383;if(_tmp38C->tag != 1)goto _LL1AF;else{_tmp38D=(void*)_tmp38C->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp38E=(struct Cyc_Absyn_Local_b_struct*)_tmp38D;if(
_tmp38E->tag != 4)goto _LL1AF;else{_tmp38F=_tmp38E->f1;}};}}_LL1AE: _tmp393=_tmp38F;
goto _LL1B0;_LL1AF: {struct Cyc_Absyn_Var_e_struct*_tmp390=(struct Cyc_Absyn_Var_e_struct*)
_tmp383;if(_tmp390->tag != 1)goto _LL1B1;else{_tmp391=(void*)_tmp390->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp392=(struct Cyc_Absyn_Pat_b_struct*)_tmp391;if(_tmp392->tag
!= 5)goto _LL1B1;else{_tmp393=_tmp392->f1;}};}}_LL1B0: if(!_tmp393->escapes)return
Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp393,e2);goto _LL1A8;_LL1B1:;_LL1B2: goto
_LL1A8;_LL1A8:;}return r;}static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){{const char*_tmp611;void*_tmp610[1];struct Cyc_String_pa_struct _tmp60F;(_tmp60F.tag=
0,((_tmp60F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name)),((_tmp610[0]=& _tmp60F,Cyc_fprintf(Cyc_stderr,((_tmp611="%s",
_tag_dyneither(_tmp611,sizeof(char),3))),_tag_dyneither(_tmp610,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp397=r->rop;unsigned int _tmp398;struct _tuple10
_tmp399;struct Cyc_Absyn_Vardecl*_tmp39A;struct Cyc_Absyn_Vardecl*_tmp39B;
unsigned int _tmp39C;unsigned int _tmp39D;struct Cyc_Absyn_Vardecl*_tmp39E;_LL1B4:
if((_tmp397.EqualConst).tag != 1)goto _LL1B6;_tmp398=(unsigned int)(_tmp397.EqualConst).val;
_LL1B5:{const char*_tmp615;void*_tmp614[1];struct Cyc_Int_pa_struct _tmp613;(
_tmp613.tag=1,((_tmp613.f1=(unsigned long)((int)_tmp398),((_tmp614[0]=& _tmp613,
Cyc_fprintf(Cyc_stderr,((_tmp615="==%d",_tag_dyneither(_tmp615,sizeof(char),5))),
_tag_dyneither(_tmp614,sizeof(void*),1)))))));}goto _LL1B3;_LL1B6: if((_tmp397.LessVar).tag
!= 2)goto _LL1B8;_tmp399=(struct _tuple10)(_tmp397.LessVar).val;_tmp39A=_tmp399.f1;
_LL1B7:{const char*_tmp619;void*_tmp618[1];struct Cyc_String_pa_struct _tmp617;(
_tmp617.tag=0,((_tmp617.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp39A->name)),((_tmp618[0]=& _tmp617,Cyc_fprintf(Cyc_stderr,((_tmp619="<%s",
_tag_dyneither(_tmp619,sizeof(char),4))),_tag_dyneither(_tmp618,sizeof(void*),1)))))));}
goto _LL1B3;_LL1B8: if((_tmp397.LessNumelts).tag != 3)goto _LL1BA;_tmp39B=(struct Cyc_Absyn_Vardecl*)(
_tmp397.LessNumelts).val;_LL1B9:{const char*_tmp61D;void*_tmp61C[1];struct Cyc_String_pa_struct
_tmp61B;(_tmp61B.tag=0,((_tmp61B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp39B->name)),((_tmp61C[0]=& _tmp61B,Cyc_fprintf(Cyc_stderr,((
_tmp61D="<numelts(%s)",_tag_dyneither(_tmp61D,sizeof(char),13))),_tag_dyneither(
_tmp61C,sizeof(void*),1)))))));}goto _LL1B3;_LL1BA: if((_tmp397.LessConst).tag != 4)
goto _LL1BC;_tmp39C=(unsigned int)(_tmp397.LessConst).val;_LL1BB:{const char*
_tmp621;void*_tmp620[1];struct Cyc_Int_pa_struct _tmp61F;(_tmp61F.tag=1,((_tmp61F.f1=(
unsigned long)((int)_tmp39C),((_tmp620[0]=& _tmp61F,Cyc_fprintf(Cyc_stderr,((
_tmp621="<%d",_tag_dyneither(_tmp621,sizeof(char),4))),_tag_dyneither(_tmp620,
sizeof(void*),1)))))));}goto _LL1B3;_LL1BC: if((_tmp397.LessEqConst).tag != 6)goto
_LL1BE;_tmp39D=(unsigned int)(_tmp397.LessEqConst).val;_LL1BD:{const char*_tmp625;
void*_tmp624[1];struct Cyc_Int_pa_struct _tmp623;(_tmp623.tag=1,((_tmp623.f1=(
unsigned long)((int)_tmp39D),((_tmp624[0]=& _tmp623,Cyc_fprintf(Cyc_stderr,((
_tmp625="<=%d",_tag_dyneither(_tmp625,sizeof(char),5))),_tag_dyneither(_tmp624,
sizeof(void*),1)))))));}goto _LL1B3;_LL1BE: if((_tmp397.LessEqNumelts).tag != 5)
goto _LL1B3;_tmp39E=(struct Cyc_Absyn_Vardecl*)(_tmp397.LessEqNumelts).val;_LL1BF:{
const char*_tmp629;void*_tmp628[1];struct Cyc_String_pa_struct _tmp627;(_tmp627.tag=
0,((_tmp627.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp39E->name)),((_tmp628[0]=& _tmp627,Cyc_fprintf(Cyc_stderr,((_tmp629="<=numelts(%s)",
_tag_dyneither(_tmp629,sizeof(char),14))),_tag_dyneither(_tmp628,sizeof(void*),1)))))));}
goto _LL1B3;_LL1B3:;};}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(
0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);
if(r->tl != 0){const char*_tmp62C;void*_tmp62B;(_tmp62B=0,Cyc_fprintf(Cyc_stderr,((
_tmp62C=",",_tag_dyneither(_tmp62C,sizeof(char),2))),_tag_dyneither(_tmp62B,
sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel
il){switch(il){case Cyc_CfFlowInfo_NoneIL: _LL1C0:{const char*_tmp62F;void*_tmp62E;(
_tmp62E=0,Cyc_fprintf(Cyc_stderr,((_tmp62F="uninitialized",_tag_dyneither(
_tmp62F,sizeof(char),14))),_tag_dyneither(_tmp62E,sizeof(void*),0)));}break;case
Cyc_CfFlowInfo_ThisIL: _LL1C1:{const char*_tmp632;void*_tmp631;(_tmp631=0,Cyc_fprintf(
Cyc_stderr,((_tmp632="this-initialized",_tag_dyneither(_tmp632,sizeof(char),17))),
_tag_dyneither(_tmp631,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL1C2:{
const char*_tmp635;void*_tmp634;(_tmp634=0,Cyc_fprintf(Cyc_stderr,((_tmp635="all-initialized",
_tag_dyneither(_tmp635,sizeof(char),16))),_tag_dyneither(_tmp634,sizeof(void*),0)));}
break;}}void Cyc_CfFlowInfo_print_root(void*root){void*_tmp3B9=root;struct Cyc_Absyn_Vardecl*
_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BD;void*_tmp3BE;_LL1C5: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp3BA=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp3B9;if(_tmp3BA->tag != 0)goto
_LL1C7;else{_tmp3BB=_tmp3BA->f1;}}_LL1C6:{const char*_tmp639;void*_tmp638[1];
struct Cyc_String_pa_struct _tmp637;(_tmp637.tag=0,((_tmp637.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3BB->name)),((
_tmp638[0]=& _tmp637,Cyc_fprintf(Cyc_stderr,((_tmp639="Root(%s)",_tag_dyneither(
_tmp639,sizeof(char),9))),_tag_dyneither(_tmp638,sizeof(void*),1)))))));}goto
_LL1C4;_LL1C7: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp3BC=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp3B9;if(_tmp3BC->tag != 1)goto _LL1C9;else{_tmp3BD=_tmp3BC->f1;_tmp3BE=(void*)
_tmp3BC->f2;}}_LL1C8:{const char*_tmp63E;void*_tmp63D[2];struct Cyc_String_pa_struct
_tmp63C;struct Cyc_String_pa_struct _tmp63B;(_tmp63B.tag=0,((_tmp63B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3BE)),((_tmp63C.tag=
0,((_tmp63C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
_tmp3BD)),((_tmp63D[0]=& _tmp63C,((_tmp63D[1]=& _tmp63B,Cyc_fprintf(Cyc_stderr,((
_tmp63E="MallocPt(%s,%s)",_tag_dyneither(_tmp63E,sizeof(char),16))),
_tag_dyneither(_tmp63D,sizeof(void*),2)))))))))))));}goto _LL1C4;_LL1C9: {struct
Cyc_CfFlowInfo_InitParam_struct*_tmp3BF=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp3B9;if(_tmp3BF->tag != 2)goto _LL1C4;}_LL1CA:{const char*_tmp641;void*_tmp640;(
_tmp640=0,Cyc_fprintf(Cyc_stderr,((_tmp641="InitParam(_,_)",_tag_dyneither(
_tmp641,sizeof(char),15))),_tag_dyneither(_tmp640,sizeof(void*),0)));}goto _LL1C4;
_LL1C4:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){Cyc_CfFlowInfo_print_root(
p->root);{struct Cyc_List_List*_tmp3C9=p->fields;while(_tmp3C9 != 0){{const char*
_tmp645;void*_tmp644[1];struct Cyc_Int_pa_struct _tmp643;(_tmp643.tag=1,((_tmp643.f1=(
unsigned long)((int)_tmp3C9->hd),((_tmp644[0]=& _tmp643,Cyc_fprintf(Cyc_stderr,((
_tmp645=".%d",_tag_dyneither(_tmp645,sizeof(char),4))),_tag_dyneither(_tmp644,
sizeof(void*),1)))))));}_tmp3C9=_tmp3C9->tl;}{const char*_tmp648;void*_tmp647;(
_tmp647=0,Cyc_fprintf(Cyc_stderr,((_tmp648=" ",_tag_dyneither(_tmp648,sizeof(
char),2))),_tag_dyneither(_tmp647,sizeof(void*),0)));};};}void Cyc_CfFlowInfo_print_list(
struct Cyc_List_List*x,void(*pr)(void*)){int _tmp3CF=1;while(x != 0){if(!_tmp3CF){{
const char*_tmp64B;void*_tmp64A;(_tmp64A=0,Cyc_fprintf(Cyc_stderr,((_tmp64B=", ",
_tag_dyneither(_tmp64B,sizeof(char),3))),_tag_dyneither(_tmp64A,sizeof(void*),0)));}
_tmp3CF=0;}pr((void*)x->hd);x=x->tl;}{const char*_tmp64E;void*_tmp64D;(_tmp64D=0,
Cyc_fprintf(Cyc_stderr,((_tmp64E="\n",_tag_dyneither(_tmp64E,sizeof(char),2))),
_tag_dyneither(_tmp64D,sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*)){struct _RegionHandle _tmp3D4=_new_region("r");
struct _RegionHandle*r=& _tmp3D4;_push_region(r);{struct _handler_cons _tmp3D5;
_push_handler(& _tmp3D5);{int _tmp3D7=0;if(setjmp(_tmp3D5.handler))_tmp3D7=1;if(!
_tmp3D7){{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple13 _tmp3D8=*((
struct _tuple13*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;{const char*_tmp651;void*_tmp650;(_tmp650=0,Cyc_fprintf(Cyc_stderr,((
_tmp651="{ ",_tag_dyneither(_tmp651,sizeof(char),3))),_tag_dyneither(_tmp650,
sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,struct _tuple13*))Cyc_Iter_next)(
iter,& _tmp3D8)){void*_tmp3DB=_tmp3D8.f1;if(!first){{const char*_tmp654;void*
_tmp653;(_tmp653=0,Cyc_fprintf(Cyc_stderr,((_tmp654=", ",_tag_dyneither(_tmp654,
sizeof(char),3))),_tag_dyneither(_tmp653,sizeof(void*),0)));}first=0;}pr(_tmp3DB);}{
const char*_tmp657;void*_tmp656;(_tmp656=0,Cyc_fprintf(Cyc_stderr,((_tmp657="}\n",
_tag_dyneither(_tmp657,sizeof(char),3))),_tag_dyneither(_tmp656,sizeof(void*),0)));};};
_pop_handler();}else{void*_tmp3D6=(void*)_exn_thrown;void*_tmp3E1=_tmp3D6;_LL1CC:;
_LL1CD:{const char*_tmp65A;void*_tmp659;(_tmp659=0,Cyc_fprintf(Cyc_stderr,((
_tmp65A="{ }\n",_tag_dyneither(_tmp65A,sizeof(char),5))),_tag_dyneither(_tmp659,
sizeof(void*),0)));}_npop_handler(0);return;_LL1CE:;_LL1CF:(void)_throw(_tmp3E1);
_LL1CB:;}};};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval){void*
_tmp3E4=rval;enum Cyc_CfFlowInfo_InitLevel _tmp3E9;enum Cyc_CfFlowInfo_InitLevel
_tmp3EB;struct Cyc_CfFlowInfo_Place*_tmp3ED;struct Cyc_CfFlowInfo_UnionRInfo
_tmp3EF;int _tmp3F0;int _tmp3F1;struct _dyneither_ptr _tmp3F2;struct Cyc_Absyn_Exp*
_tmp3F4;int _tmp3F5;void*_tmp3F6;_LL1D1: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3E5=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3E4;if(_tmp3E5->tag != 0)goto
_LL1D3;}_LL1D2:{const char*_tmp65D;void*_tmp65C;(_tmp65C=0,Cyc_fprintf(Cyc_stderr,((
_tmp65D="Zero",_tag_dyneither(_tmp65D,sizeof(char),5))),_tag_dyneither(_tmp65C,
sizeof(void*),0)));}goto _LL1D0;_LL1D3: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3E6=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3E4;if(_tmp3E6->tag != 1)
goto _LL1D5;}_LL1D4:{const char*_tmp660;void*_tmp65F;(_tmp65F=0,Cyc_fprintf(Cyc_stderr,((
_tmp660="NotZeroAll",_tag_dyneither(_tmp660,sizeof(char),11))),_tag_dyneither(
_tmp65F,sizeof(void*),0)));}goto _LL1D0;_LL1D5: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3E7=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3E4;if(_tmp3E7->tag != 2)
goto _LL1D7;}_LL1D6:{const char*_tmp663;void*_tmp662;(_tmp662=0,Cyc_fprintf(Cyc_stderr,((
_tmp663="NotZeroThis",_tag_dyneither(_tmp663,sizeof(char),12))),_tag_dyneither(
_tmp662,sizeof(void*),0)));}goto _LL1D0;_LL1D7: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3E8=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3E4;if(_tmp3E8->tag != 3)goto
_LL1D9;else{_tmp3E9=_tmp3E8->f1;}}_LL1D8:{const char*_tmp666;void*_tmp665;(
_tmp665=0,Cyc_fprintf(Cyc_stderr,((_tmp666="Unknown(",_tag_dyneither(_tmp666,
sizeof(char),9))),_tag_dyneither(_tmp665,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3E9);{const char*_tmp669;void*_tmp668;(_tmp668=0,Cyc_fprintf(Cyc_stderr,((
_tmp669=")",_tag_dyneither(_tmp669,sizeof(char),2))),_tag_dyneither(_tmp668,
sizeof(void*),0)));}goto _LL1D0;_LL1D9: {struct Cyc_CfFlowInfo_Esc_struct*_tmp3EA=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp3E4;if(_tmp3EA->tag != 4)goto _LL1DB;else{
_tmp3EB=_tmp3EA->f1;}}_LL1DA:{const char*_tmp66C;void*_tmp66B;(_tmp66B=0,Cyc_fprintf(
Cyc_stderr,((_tmp66C="Esc(",_tag_dyneither(_tmp66C,sizeof(char),5))),
_tag_dyneither(_tmp66B,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3EB);{const char*_tmp66F;void*_tmp66E;(_tmp66E=0,Cyc_fprintf(Cyc_stderr,((
_tmp66F=")",_tag_dyneither(_tmp66F,sizeof(char),2))),_tag_dyneither(_tmp66E,
sizeof(void*),0)));}goto _LL1D0;_LL1DB: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3EC=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3E4;if(_tmp3EC->tag != 5)goto
_LL1DD;else{_tmp3ED=_tmp3EC->f1;}}_LL1DC:{const char*_tmp672;void*_tmp671;(
_tmp671=0,Cyc_fprintf(Cyc_stderr,((_tmp672="AddrOf(",_tag_dyneither(_tmp672,
sizeof(char),8))),_tag_dyneither(_tmp671,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(
_tmp3ED);{const char*_tmp675;void*_tmp674;(_tmp674=0,Cyc_fprintf(Cyc_stderr,((
_tmp675=")",_tag_dyneither(_tmp675,sizeof(char),2))),_tag_dyneither(_tmp674,
sizeof(void*),0)));}goto _LL1D0;_LL1DD: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp3EE=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp3E4;if(_tmp3EE->tag != 6)goto
_LL1DF;else{_tmp3EF=_tmp3EE->f1;_tmp3F0=_tmp3EF.is_union;_tmp3F1=_tmp3EF.fieldnum;
_tmp3F2=_tmp3EE->f2;}}_LL1DE: if(_tmp3F0){{const char*_tmp678;void*_tmp677;(
_tmp677=0,Cyc_fprintf(Cyc_stderr,((_tmp678="AggrUnion{",_tag_dyneither(_tmp678,
sizeof(char),11))),_tag_dyneither(_tmp677,sizeof(void*),0)));}if(_tmp3F1 != - 1){
const char*_tmp67C;void*_tmp67B[1];struct Cyc_Int_pa_struct _tmp67A;(_tmp67A.tag=1,((
_tmp67A.f1=(unsigned long)_tmp3F1,((_tmp67B[0]=& _tmp67A,Cyc_fprintf(Cyc_stderr,((
_tmp67C="tag == %d;",_tag_dyneither(_tmp67C,sizeof(char),11))),_tag_dyneither(
_tmp67B,sizeof(void*),1)))))));}}else{const char*_tmp67F;void*_tmp67E;(_tmp67E=0,
Cyc_fprintf(Cyc_stderr,((_tmp67F="AggrStruct{",_tag_dyneither(_tmp67F,sizeof(
char),12))),_tag_dyneither(_tmp67E,sizeof(void*),0)));}{int i=0;for(0;i < 
_get_dyneither_size(_tmp3F2,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp3F2.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp3F2,sizeof(void*))){
const char*_tmp682;void*_tmp681;(_tmp681=0,Cyc_fprintf(Cyc_stderr,((_tmp682=",",
_tag_dyneither(_tmp682,sizeof(char),2))),_tag_dyneither(_tmp681,sizeof(void*),0)));}}}{
const char*_tmp685;void*_tmp684;(_tmp684=0,Cyc_fprintf(Cyc_stderr,((_tmp685="}",
_tag_dyneither(_tmp685,sizeof(char),2))),_tag_dyneither(_tmp684,sizeof(void*),0)));}
goto _LL1D0;_LL1DF: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp3F3=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp3E4;if(_tmp3F3->tag != 7)goto _LL1D0;else{_tmp3F4=_tmp3F3->f1;_tmp3F5=_tmp3F3->f2;
_tmp3F6=(void*)_tmp3F3->f3;}}_LL1E0:{const char*_tmp68A;void*_tmp689[2];struct Cyc_String_pa_struct
_tmp688;struct Cyc_Int_pa_struct _tmp687;(_tmp687.tag=1,((_tmp687.f1=(
unsigned long)_tmp3F5,((_tmp688.tag=0,((_tmp688.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp3F4)),((_tmp689[0]=& _tmp688,((
_tmp689[1]=& _tmp687,Cyc_fprintf(Cyc_stderr,((_tmp68A="[Consumed(%s,%d,",
_tag_dyneither(_tmp68A,sizeof(char),17))),_tag_dyneither(_tmp689,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp3F6);{const char*_tmp68D;void*_tmp68C;(_tmp68C=0,
Cyc_fprintf(Cyc_stderr,((_tmp68D="]",_tag_dyneither(_tmp68D,sizeof(char),2))),
_tag_dyneither(_tmp68C,sizeof(void*),0)));};_LL1D0:;}static void Cyc_CfFlowInfo_print_flow_mapping(
void*root,void*rval){{const char*_tmp690;void*_tmp68F;(_tmp68F=0,Cyc_fprintf(Cyc_stderr,((
_tmp690="    ",_tag_dyneither(_tmp690,sizeof(char),5))),_tag_dyneither(_tmp68F,
sizeof(void*),0)));}Cyc_CfFlowInfo_print_root(root);{const char*_tmp693;void*
_tmp692;(_tmp692=0,Cyc_fprintf(Cyc_stderr,((_tmp693=" --> ",_tag_dyneither(
_tmp693,sizeof(char),6))),_tag_dyneither(_tmp692,sizeof(void*),0)));}Cyc_CfFlowInfo_print_absrval(
rval);{const char*_tmp696;void*_tmp695;(_tmp695=0,Cyc_fprintf(Cyc_stderr,((
_tmp696="\n",_tag_dyneither(_tmp696,sizeof(char),2))),_tag_dyneither(_tmp695,
sizeof(void*),0)));};}void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){((
void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,
d);}void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo
_tmp420=f;int _tmp421;struct _tuple11 _tmp422;struct Cyc_Dict_Dict _tmp423;_LL1E2: if((
_tmp420.BottomFL).tag != 1)goto _LL1E4;_tmp421=(int)(_tmp420.BottomFL).val;_LL1E3:{
const char*_tmp699;void*_tmp698;(_tmp698=0,Cyc_fprintf(Cyc_stderr,((_tmp699="  BottomFL\n",
_tag_dyneither(_tmp699,sizeof(char),12))),_tag_dyneither(_tmp698,sizeof(void*),0)));}
goto _LL1E1;_LL1E4: if((_tmp420.ReachableFL).tag != 2)goto _LL1E1;_tmp422=(struct
_tuple11)(_tmp420.ReachableFL).val;_tmp423=_tmp422.f1;_LL1E5:{const char*_tmp69C;
void*_tmp69B;(_tmp69B=0,Cyc_fprintf(Cyc_stderr,((_tmp69C="  ReachableFL:\n",
_tag_dyneither(_tmp69C,sizeof(char),16))),_tag_dyneither(_tmp69B,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp423);{const char*_tmp69F;void*_tmp69E;(_tmp69E=
0,Cyc_fprintf(Cyc_stderr,((_tmp69F="\n",_tag_dyneither(_tmp69F,sizeof(char),2))),
_tag_dyneither(_tmp69E,sizeof(void*),0)));}goto _LL1E1;_LL1E1:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t){void*_tmp42A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp436;struct Cyc_Absyn_DatatypeInfo _tmp438;struct Cyc_List_List*_tmp439;struct
Cyc_List_List*_tmp43B;struct Cyc_Absyn_AggrInfo _tmp43D;struct Cyc_List_List*
_tmp43E;struct Cyc_Absyn_DatatypeFieldInfo _tmp440;struct Cyc_List_List*_tmp441;
struct Cyc_List_List*_tmp443;struct Cyc_Absyn_PtrInfo _tmp445;void*_tmp446;struct
Cyc_Absyn_PtrAtts _tmp447;void*_tmp448;struct Cyc_List_List*_tmp44B;struct Cyc_List_List*
_tmp44D;struct Cyc_Absyn_ArrayInfo _tmp44F;void*_tmp450;void*_tmp452;void*_tmp454;
void*_tmp456;void*_tmp457;void*_tmp459;_LL1E7: {struct Cyc_Absyn_VoidType_struct*
_tmp42B=(struct Cyc_Absyn_VoidType_struct*)_tmp42A;if(_tmp42B->tag != 0)goto _LL1E9;}
_LL1E8: goto _LL1EA;_LL1E9: {struct Cyc_Absyn_IntType_struct*_tmp42C=(struct Cyc_Absyn_IntType_struct*)
_tmp42A;if(_tmp42C->tag != 6)goto _LL1EB;}_LL1EA: goto _LL1EC;_LL1EB: {struct Cyc_Absyn_FloatType_struct*
_tmp42D=(struct Cyc_Absyn_FloatType_struct*)_tmp42A;if(_tmp42D->tag != 7)goto
_LL1ED;}_LL1EC: goto _LL1EE;_LL1ED: {struct Cyc_Absyn_DoubleType_struct*_tmp42E=(
struct Cyc_Absyn_DoubleType_struct*)_tmp42A;if(_tmp42E->tag != 8)goto _LL1EF;}
_LL1EE: goto _LL1F0;_LL1EF: {struct Cyc_Absyn_EnumType_struct*_tmp42F=(struct Cyc_Absyn_EnumType_struct*)
_tmp42A;if(_tmp42F->tag != 14)goto _LL1F1;}_LL1F0: goto _LL1F2;_LL1F1: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp430=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp42A;if(_tmp430->tag != 15)goto
_LL1F3;}_LL1F2: goto _LL1F4;_LL1F3: {struct Cyc_Absyn_TagType_struct*_tmp431=(
struct Cyc_Absyn_TagType_struct*)_tmp42A;if(_tmp431->tag != 20)goto _LL1F5;}_LL1F4:
goto _LL1F6;_LL1F5: {struct Cyc_Absyn_HeapRgn_struct*_tmp432=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp42A;if(_tmp432->tag != 21)goto _LL1F7;}_LL1F6: goto _LL1F8;_LL1F7: {struct Cyc_Absyn_Evar_struct*
_tmp433=(struct Cyc_Absyn_Evar_struct*)_tmp42A;if(_tmp433->tag != 1)goto _LL1F9;}
_LL1F8: return 0;_LL1F9: {struct Cyc_Absyn_UniqueRgn_struct*_tmp434=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp42A;if(_tmp434->tag != 22)goto _LL1FB;}_LL1FA: {void*_tmp45B=rgn;_LL21C: {
struct Cyc_CfFlowInfo_UniqueRgn_k_struct*_tmp45C=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)
_tmp45B;if(_tmp45C->tag != 0)goto _LL21E;}_LL21D: return 1;_LL21E:;_LL21F: return 0;
_LL21B:;}_LL1FB: {struct Cyc_Absyn_VarType_struct*_tmp435=(struct Cyc_Absyn_VarType_struct*)
_tmp42A;if(_tmp435->tag != 2)goto _LL1FD;else{_tmp436=_tmp435->f1;}}_LL1FC: {void*
_tmp45D=rgn;struct Cyc_Absyn_Tvar*_tmp460;_LL221: {struct Cyc_CfFlowInfo_UniqueRgn_k_struct*
_tmp45E=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)_tmp45D;if(_tmp45E->tag != 0)
goto _LL223;}_LL222: return 0;_LL223: {struct Cyc_CfFlowInfo_Region_k_struct*_tmp45F=(
struct Cyc_CfFlowInfo_Region_k_struct*)_tmp45D;if(_tmp45F->tag != 1)goto _LL220;
else{_tmp460=_tmp45F->f1;}}_LL224: return Cyc_Absyn_tvar_cmp(_tmp436,_tmp460)== 0;
_LL220:;}_LL1FD: {struct Cyc_Absyn_DatatypeType_struct*_tmp437=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp42A;if(_tmp437->tag != 3)goto _LL1FF;else{_tmp438=_tmp437->f1;_tmp439=_tmp438.targs;}}
_LL1FE: _tmp43B=_tmp439;goto _LL200;_LL1FF: {struct Cyc_Absyn_TypedefType_struct*
_tmp43A=(struct Cyc_Absyn_TypedefType_struct*)_tmp42A;if(_tmp43A->tag != 18)goto
_LL201;else{_tmp43B=_tmp43A->f2;}}_LL200: _tmp43E=_tmp43B;goto _LL202;_LL201: {
struct Cyc_Absyn_AggrType_struct*_tmp43C=(struct Cyc_Absyn_AggrType_struct*)
_tmp42A;if(_tmp43C->tag != 12)goto _LL203;else{_tmp43D=_tmp43C->f1;_tmp43E=_tmp43D.targs;}}
_LL202: _tmp441=_tmp43E;goto _LL204;_LL203: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp43F=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp42A;if(_tmp43F->tag != 4)
goto _LL205;else{_tmp440=_tmp43F->f1;_tmp441=_tmp440.targs;}}_LL204: _tmp443=
_tmp441;goto _LL206;_LL205: {struct Cyc_Absyn_JoinEff_struct*_tmp442=(struct Cyc_Absyn_JoinEff_struct*)
_tmp42A;if(_tmp442->tag != 24)goto _LL207;else{_tmp443=_tmp442->f1;}}_LL206: return((
int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(
Cyc_CfFlowInfo_contains_region,rgn,_tmp443);_LL207: {struct Cyc_Absyn_PointerType_struct*
_tmp444=(struct Cyc_Absyn_PointerType_struct*)_tmp42A;if(_tmp444->tag != 5)goto
_LL209;else{_tmp445=_tmp444->f1;_tmp446=_tmp445.elt_typ;_tmp447=_tmp445.ptr_atts;
_tmp448=_tmp447.rgn;}}_LL208: return Cyc_CfFlowInfo_contains_region(rgn,_tmp448)
 || Cyc_CfFlowInfo_contains_region(rgn,_tmp446);_LL209: {struct Cyc_Absyn_FnType_struct*
_tmp449=(struct Cyc_Absyn_FnType_struct*)_tmp42A;if(_tmp449->tag != 10)goto _LL20B;}
_LL20A: return 0;_LL20B: {struct Cyc_Absyn_TupleType_struct*_tmp44A=(struct Cyc_Absyn_TupleType_struct*)
_tmp42A;if(_tmp44A->tag != 11)goto _LL20D;else{_tmp44B=_tmp44A->f1;}}_LL20C: for(0;
_tmp44B != 0;_tmp44B=_tmp44B->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((
struct _tuple15*)_tmp44B->hd)).f2))return 1;}return 0;_LL20D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp44C=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp42A;if(_tmp44C->tag != 13)goto
_LL20F;else{_tmp44D=_tmp44C->f2;}}_LL20E: for(0;_tmp44D != 0;_tmp44D=_tmp44D->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp44D->hd)->type))
return 1;}return 0;_LL20F: {struct Cyc_Absyn_ArrayType_struct*_tmp44E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp42A;if(_tmp44E->tag != 9)goto _LL211;else{_tmp44F=_tmp44E->f1;_tmp450=_tmp44F.elt_type;}}
_LL210: _tmp452=_tmp450;goto _LL212;_LL211: {struct Cyc_Absyn_AccessEff_struct*
_tmp451=(struct Cyc_Absyn_AccessEff_struct*)_tmp42A;if(_tmp451->tag != 23)goto
_LL213;else{_tmp452=(void*)_tmp451->f1;}}_LL212: _tmp454=_tmp452;goto _LL214;
_LL213: {struct Cyc_Absyn_RgnsEff_struct*_tmp453=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp42A;if(_tmp453->tag != 25)goto _LL215;else{_tmp454=(void*)_tmp453->f1;}}_LL214:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp454);_LL215: {struct Cyc_Absyn_DynRgnType_struct*
_tmp455=(struct Cyc_Absyn_DynRgnType_struct*)_tmp42A;if(_tmp455->tag != 17)goto
_LL217;else{_tmp456=(void*)_tmp455->f1;_tmp457=(void*)_tmp455->f2;}}_LL216:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp456) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp457);_LL217: {struct Cyc_Absyn_RgnHandleType_struct*_tmp458=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp42A;if(_tmp458->tag != 16)goto _LL219;else{_tmp459=(void*)_tmp458->f1;}}_LL218:
return 0;_LL219: {struct Cyc_Absyn_ValueofType_struct*_tmp45A=(struct Cyc_Absyn_ValueofType_struct*)
_tmp42A;if(_tmp45A->tag != 19)goto _LL1E6;}_LL21A: return 0;_LL1E6:;}struct _tuple23{
struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(
struct _tuple23*env,void*root,void*rval){struct _tuple23 _tmp462;struct Cyc_CfFlowInfo_FlowEnv*
_tmp463;struct Cyc_Dict_Dict _tmp464;struct Cyc_Dict_Dict*_tmp465;void*_tmp466;
struct _tuple23*_tmp461=env;_tmp462=*_tmp461;_tmp463=_tmp462.f1;_tmp464=_tmp462.f2;
_tmp465=(struct Cyc_Dict_Dict*)&(*_tmp461).f2;_tmp466=_tmp462.f3;{void*_tmp467=
root;struct Cyc_Absyn_Vardecl*_tmp469;void*_tmp46B;_LL226: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp468=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp467;if(_tmp468->tag != 0)goto
_LL228;else{_tmp469=_tmp468->f1;}}_LL227: if(Cyc_CfFlowInfo_contains_region(
_tmp466,_tmp469->type))rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp463,_tmp469->type,
0,_tmp463->unknown_none);*_tmp465=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(*_tmp465,root,rval);goto _LL225;_LL228: {struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp46A=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp467;if(_tmp46A->tag != 1)goto _LL22A;else{_tmp46B=(void*)_tmp46A->f2;}}_LL229:
if(!Cyc_CfFlowInfo_contains_region(_tmp466,_tmp46B))*_tmp465=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp465,root,rval);goto
_LL225;_LL22A: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp46C=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp467;if(_tmp46C->tag != 2)goto _LL225;}_LL22B: goto _LL225;_LL225:;};}static
struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp46D=Cyc_Tcutil_compress(rgn);
struct Cyc_Absyn_Tvar*_tmp46F;_LL22D: {struct Cyc_Absyn_VarType_struct*_tmp46E=(
struct Cyc_Absyn_VarType_struct*)_tmp46D;if(_tmp46E->tag != 2)goto _LL22F;else{
_tmp46F=_tmp46E->f1;}}_LL22E: {struct Cyc_CfFlowInfo_Region_k_struct _tmp6A0;
struct Cyc_CfFlowInfo_Region_k_struct v=(_tmp6A0.tag=1,((_tmp6A0.f1=_tmp46F,
_tmp6A0)));void*v2=(void*)& v;struct _tuple23 _tmp6A1;struct _tuple23 env=(_tmp6A1.f1=
fenv,((_tmp6A1.f2=fenv->mt_flowdict,((_tmp6A1.f3=v2,_tmp6A1)))));((void(*)(void(*
f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL22F: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp470=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp46D;if(_tmp470->tag != 22)goto
_LL231;}_LL230: {struct _tuple23 _tmp6A2;struct _tuple23 env=(_tmp6A2.f1=fenv,((
_tmp6A2.f2=fenv->mt_flowdict,((_tmp6A2.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,
_tmp6A2)))));((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return
env.f2;}_LL231:;_LL232: {struct Cyc_Core_Impossible_struct _tmp6A8;const char*
_tmp6A7;struct Cyc_Core_Impossible_struct*_tmp6A6;(int)_throw((void*)((_tmp6A6=
_cycalloc(sizeof(*_tmp6A6)),((_tmp6A6[0]=((_tmp6A8.tag=Cyc_Core_Impossible,((
_tmp6A8.f1=((_tmp6A7="kill_flowdict_region",_tag_dyneither(_tmp6A7,sizeof(char),
21))),_tmp6A8)))),_tmp6A6)))));}_LL22C:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){union
Cyc_CfFlowInfo_FlowInfo _tmp477=f;int _tmp478;struct _tuple11 _tmp479;struct Cyc_Dict_Dict
_tmp47A;struct Cyc_List_List*_tmp47B;_LL234: if((_tmp477.BottomFL).tag != 1)goto
_LL236;_tmp478=(int)(_tmp477.BottomFL).val;_LL235: return f;_LL236: if((_tmp477.ReachableFL).tag
!= 2)goto _LL233;_tmp479=(struct _tuple11)(_tmp477.ReachableFL).val;_tmp47A=
_tmp479.f1;_tmp47B=_tmp479.f2;_LL237: {struct Cyc_Dict_Dict _tmp47C=Cyc_CfFlowInfo_kill_flowdict_region(
fenv,_tmp47A,rgn);struct Cyc_List_List*_tmp47D=Cyc_CfFlowInfo_kill_relns_region(
_tmp47B,rgn);return Cyc_CfFlowInfo_ReachableFL(_tmp47C,_tmp47D);}_LL233:;}
