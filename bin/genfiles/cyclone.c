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
 void exit(int);struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct
Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_feof(struct Cyc___cycFILE*);
int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc___cycFILE*
Cyc_fopen(const char*,const char*);int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);unsigned long Cyc_fread(struct _dyneither_ptr,
unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
unsigned long Cyc_fwrite(struct _dyneither_ptr,unsigned long,unsigned long,struct
Cyc___cycFILE*);int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);int
remove(const char*);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc___cycFILE*Cyc_file_open(struct
_dyneither_ptr,struct _dyneither_ptr);void Cyc_file_close(struct Cyc___cycFILE*);
int isspace(int);char*getenv(const char*);int system(const char*);struct Cyc_Core_Opt{
void*v;};extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_unique_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct _dyneither_ptr Cstring_to_string(char*);inline static void* arrcast(struct
_dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || 
dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}struct
Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,
struct _dyneither_ptr);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,
void*x);struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[4];
struct Cyc_Arg_Bad_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[
6];struct Cyc_Arg_Error_struct{char*tag;};struct Cyc_Arg_Unit_spec_struct{int tag;
void(*f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;int*
f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};extern int Cyc_Arg_current;void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct
_dyneither_ptr args);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(
struct _dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct
_dyneither_ptr*s1,struct _dyneither_ptr*s2);int Cyc_strncmp(struct _dyneither_ptr s1,
struct _dyneither_ptr s2,unsigned long len);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_strconcat_l(
struct Cyc_List_List*);struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,
struct _dyneither_ptr);struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct
Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{
char*tag;};struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct
_dyneither_ptr);struct _dyneither_ptr Cyc_Filename_chop_extension(struct
_dyneither_ptr);struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);int Cyc_Filename_check_suffix(
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};void Cyc_Position_reset_position(
struct _dyneither_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};extern int Cyc_Position_use_gcc_style_location;
extern int Cyc_Position_max_errors;int Cyc_Position_error_p();struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
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
char*tag;};extern int Cyc_Absyn_porting_c_code;extern int Cyc_Absyn_no_regions;
extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int
lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;
int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(
struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc
 = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,
Cyc_Abstract_sc  = 6};struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple9{
int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple9
val;};struct _tuple10{struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;
union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple10*
val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple11{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple11*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple12{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple12*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec*val;};struct
_tuple13{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator*val;};struct
_union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope
val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};
struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple14{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple7*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple15{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple15*val;};
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
first_line;int first_column;int last_line;int last_column;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;
int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;
int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;
int print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs);
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*
f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tcutil_flush_warnings();extern
int Cyc_Tcutil_warn_alias_coerce;void Cyc_Tcutil_check_no_qual(struct Cyc_Position_Segment*
loc,void*t);void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int
var_default_init,struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds);extern int Cyc_Tovc_elim_array_initializers;struct Cyc_List_List*
Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple16{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple16 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;union Cyc_CfFlowInfo_RelnOp rop;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct
Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{
int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;enum Cyc_CfFlowInfo_InitLevel
f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};
struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};
struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;
struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{int tag;struct Cyc_Absyn_Exp*
f1;int f2;void*f3;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple17{
struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{
int tag;struct _tuple17 val;};union Cyc_CfFlowInfo_FlowInfo{struct
_union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);extern int Cyc_NewControlFlow_warn_lose_unique;
struct Cyc_Interface_I;struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*
Cyc_Interface_final();struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict
ae);struct _tuple18{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple18*info);struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_parse(struct
Cyc___cycFILE*);void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);extern int Cyc_Cyclone_tovc_r;
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};extern
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;void Cyc_Port_port(struct Cyc_List_List*);
void Cyc_Lex_lex_init(int use_cyclone_keywords);int Cyc_Toc_warn_bounds_checks;int
Cyc_Toc_warn_all_null_deref;static int Cyc_pp_r=0;int Cyc_noexpand_r=0;static int Cyc_noshake_r=
0;static int Cyc_stop_after_cpp_r=0;static int Cyc_parseonly_r=0;static int Cyc_tc_r=0;
static int Cyc_cf_r=0;static int Cyc_noprint_r=0;static int Cyc_ic_r=0;static int Cyc_toc_r=
0;static int Cyc_stop_after_objectfile_r=0;static int Cyc_stop_after_asmfile_r=0;
static int Cyc_inline_functions_r=0;static int Cyc_elim_se_r=0;static int Cyc_v_r=0;
static int Cyc_save_temps_r=0;static int Cyc_save_c_r=0;static int Cyc_nogc_r=0;static
int Cyc_pa_r=0;static int Cyc_pg_r=0;static int Cyc_nocheck_r=0;static int Cyc_add_cyc_namespace_r=
1;static int Cyc_generate_line_directives_r=0;static int Cyc_print_full_evars_r=0;
static int Cyc_print_all_tvars_r=0;static int Cyc_print_all_kinds_r=0;static int Cyc_print_all_effects_r=
0;static int Cyc_nocyc_setjmp_r=0;static int Cyc_compile_for_boot_r=0;static struct
Cyc_List_List*Cyc_ccargs=0;static void Cyc_add_ccarg(struct _dyneither_ptr s){struct
_dyneither_ptr*_tmp3B6;struct Cyc_List_List*_tmp3B5;Cyc_ccargs=((_tmp3B5=
_cycalloc(sizeof(*_tmp3B5)),((_tmp3B5->hd=((_tmp3B6=_cycalloc(sizeof(*_tmp3B6)),((
_tmp3B6[0]=s,_tmp3B6)))),((_tmp3B5->tl=Cyc_ccargs,_tmp3B5))))));}void Cyc_set_c_compiler(
struct _dyneither_ptr s){const char*_tmp3B7;if(Cyc_strcmp((struct _dyneither_ptr)s,((
_tmp3B7="vc",_tag_dyneither(_tmp3B7,sizeof(char),3))))== 0){Cyc_Cyclone_c_compiler=
Cyc_Cyclone_Vc_c;{const char*_tmp3B8;Cyc_add_ccarg(((_tmp3B8="-DVC_C",
_tag_dyneither(_tmp3B8,sizeof(char),7))));};}else{const char*_tmp3B9;if(Cyc_strcmp((
struct _dyneither_ptr)s,((_tmp3B9="gcc",_tag_dyneither(_tmp3B9,sizeof(char),4))))
== 0){Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{const char*_tmp3BA;Cyc_add_ccarg(((
_tmp3BA="-DGCC_C",_tag_dyneither(_tmp3BA,sizeof(char),8))));};}}}void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;Cyc_Position_max_errors=65535;Cyc_save_c_r=1;Cyc_parseonly_r=
1;}static struct _dyneither_ptr*Cyc_output_file=0;static void Cyc_set_output_file(
struct _dyneither_ptr s){struct _dyneither_ptr*_tmp3BB;Cyc_output_file=((_tmp3BB=
_cycalloc(sizeof(*_tmp3BB)),((_tmp3BB[0]=s,_tmp3BB))));}extern char*Cdef_inc_path;
extern char*Cdef_lib_path;extern char*Carch;extern char*Ccomp;extern char*Cversion;
static char _tmp7[1]="";static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
static void Cyc_set_cpp(struct _dyneither_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*
Cyc_cppargs=0;static void Cyc_add_cpparg(struct _dyneither_ptr s){struct
_dyneither_ptr*_tmp3BE;struct Cyc_List_List*_tmp3BD;Cyc_cppargs=((_tmp3BD=
_cycalloc(sizeof(*_tmp3BD)),((_tmp3BD->hd=((_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((
_tmp3BE[0]=s,_tmp3BE)))),((_tmp3BD->tl=Cyc_cppargs,_tmp3BD))))));}static void Cyc_add_iprefix(
struct _dyneither_ptr s){const char*_tmp3C2;void*_tmp3C1[1];struct Cyc_String_pa_struct
_tmp3C0;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3C0.tag=0,((_tmp3C0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3C1[0]=& _tmp3C0,Cyc_aprintf(((
_tmp3C2="-iprefix %s",_tag_dyneither(_tmp3C2,sizeof(char),12))),_tag_dyneither(
_tmp3C1,sizeof(void*),1)))))))));}static void Cyc_add_iwithprefix(struct
_dyneither_ptr s){const char*_tmp3C6;void*_tmp3C5[1];struct Cyc_String_pa_struct
_tmp3C4;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3C4.tag=0,((_tmp3C4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3C5[0]=& _tmp3C4,Cyc_aprintf(((
_tmp3C6="-iwithprefix %s",_tag_dyneither(_tmp3C6,sizeof(char),16))),
_tag_dyneither(_tmp3C5,sizeof(void*),1)))))))));}static void Cyc_add_iwithprefixbefore(
struct _dyneither_ptr s){const char*_tmp3CA;void*_tmp3C9[1];struct Cyc_String_pa_struct
_tmp3C8;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3C8.tag=0,((_tmp3C8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3C9[0]=& _tmp3C8,Cyc_aprintf(((
_tmp3CA="-iwithprefixbefore %s",_tag_dyneither(_tmp3CA,sizeof(char),22))),
_tag_dyneither(_tmp3C9,sizeof(void*),1)))))))));}static void Cyc_add_isystem(
struct _dyneither_ptr s){const char*_tmp3CE;void*_tmp3CD[1];struct Cyc_String_pa_struct
_tmp3CC;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3CC.tag=0,((_tmp3CC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3CD[0]=& _tmp3CC,Cyc_aprintf(((
_tmp3CE="-isystem %s",_tag_dyneither(_tmp3CE,sizeof(char),12))),_tag_dyneither(
_tmp3CD,sizeof(void*),1)))))))));}static void Cyc_add_idirafter(struct
_dyneither_ptr s){const char*_tmp3D2;void*_tmp3D1[1];struct Cyc_String_pa_struct
_tmp3D0;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3D0.tag=0,((_tmp3D0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3D1[0]=& _tmp3D0,Cyc_aprintf(((
_tmp3D2="-idirafter %s",_tag_dyneither(_tmp3D2,sizeof(char),14))),_tag_dyneither(
_tmp3D1,sizeof(void*),1)))))))));}static struct _dyneither_ptr Cyc_target_arch=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};static void Cyc_set_target_arch(
struct _dyneither_ptr s){Cyc_toc_r=1;Cyc_target_arch=s;}static void Cyc_print_version(){{
const char*_tmp3D6;void*_tmp3D5[1];struct Cyc_String_pa_struct _tmp3D4;(_tmp3D4.tag=
0,((_tmp3D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cversion)),((_tmp3D5[0]=& _tmp3D4,Cyc_printf(((_tmp3D6="The Cyclone compiler, version %s\n",
_tag_dyneither(_tmp3D6,sizeof(char),34))),_tag_dyneither(_tmp3D5,sizeof(void*),1)))))));}{
const char*_tmp3DB;void*_tmp3DA[2];struct Cyc_String_pa_struct _tmp3D9;struct Cyc_String_pa_struct
_tmp3D8;(_tmp3D8.tag=0,((_tmp3D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp3D9.tag=0,((_tmp3D9.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3DA[0]=& _tmp3D9,((_tmp3DA[1]=&
_tmp3D8,Cyc_printf(((_tmp3DB="Compiled for architecture: %s CC=\"%s\"\n",
_tag_dyneither(_tmp3DB,sizeof(char),39))),_tag_dyneither(_tmp3DA,sizeof(void*),2)))))))))))));}{
const char*_tmp3DF;void*_tmp3DE[1];struct Cyc_String_pa_struct _tmp3DD;(_tmp3DD.tag=
0,((_tmp3DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_lib_path)),((_tmp3DE[0]=& _tmp3DD,Cyc_printf(((_tmp3DF="Standard library directory: %s\n",
_tag_dyneither(_tmp3DF,sizeof(char),32))),_tag_dyneither(_tmp3DE,sizeof(void*),1)))))));}{
const char*_tmp3E3;void*_tmp3E2[1];struct Cyc_String_pa_struct _tmp3E1;(_tmp3E1.tag=
0,((_tmp3E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_inc_path)),((_tmp3E2[0]=& _tmp3E1,Cyc_printf(((_tmp3E3="Standard include directory: %s\n",
_tag_dyneither(_tmp3E3,sizeof(char),32))),_tag_dyneither(_tmp3E2,sizeof(void*),1)))))));}
exit(0);}static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){unsigned long
_tmp26=Cyc_strlen((struct _dyneither_ptr)s);if(_tmp26 <= 4)return 0;else{const char*
_tmp3E4;return Cyc_strcmp((struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(
int)(_tmp26 - 4)),((_tmp3E4=".cyc",_tag_dyneither(_tmp3E4,sizeof(char),5))))== 0;}}
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;static void Cyc_add_cyclone_exec_path(
struct _dyneither_ptr s){unsigned long _tmp28=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp28 <= 2)return;{struct _dyneither_ptr _tmp29=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)s,2,_tmp28 - 2);struct _dyneither_ptr*_tmp3E7;struct Cyc_List_List*
_tmp3E6;Cyc_cyclone_exec_path=((_tmp3E6=_cycalloc(sizeof(*_tmp3E6)),((_tmp3E6->hd=((
_tmp3E7=_cycalloc(sizeof(*_tmp3E7)),((_tmp3E7[0]=_tmp29,_tmp3E7)))),((_tmp3E6->tl=
Cyc_cyclone_exec_path,_tmp3E6))))));};}static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s){const char*_tmp3E8;if(Cyc_strcmp((
struct _dyneither_ptr)s,((_tmp3E8="-lxml",_tag_dyneither(_tmp3E8,sizeof(char),6))))
== 0)Cyc_add_ccarg(s);else{struct _dyneither_ptr*_tmp3EB;struct Cyc_List_List*
_tmp3EA;Cyc_libargs=((_tmp3EA=_cycalloc(sizeof(*_tmp3EA)),((_tmp3EA->hd=((
_tmp3EB=_cycalloc(sizeof(*_tmp3EB)),((_tmp3EB[0]=s,_tmp3EB)))),((_tmp3EA->tl=Cyc_libargs,
_tmp3EA))))));}}static void Cyc_add_marg(struct _dyneither_ptr s){Cyc_add_ccarg(s);}
static void Cyc_set_save_temps(){Cyc_save_temps_r=1;{const char*_tmp3EC;Cyc_add_ccarg(((
_tmp3EC="-save-temps",_tag_dyneither(_tmp3EC,sizeof(char),12))));};}static int Cyc_produce_dependencies=
0;static void Cyc_set_produce_dependencies(){Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=
1;{const char*_tmp3ED;Cyc_add_cpparg(((_tmp3ED="-M",_tag_dyneither(_tmp3ED,
sizeof(char),3))));};}static struct _dyneither_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){struct
_dyneither_ptr*_tmp3EE;Cyc_dependencies_target=((_tmp3EE=_cycalloc(sizeof(*
_tmp3EE)),((_tmp3EE[0]=s,_tmp3EE))));}static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;{const char*_tmp3EF;Cyc_add_ccarg(((_tmp3EF="-c",
_tag_dyneither(_tmp3EF,sizeof(char),3))));};}static void Cyc_set_nocppprecomp(){{
const char*_tmp3F0;Cyc_add_cpparg(((_tmp3F0="-no-cpp-precomp",_tag_dyneither(
_tmp3F0,sizeof(char),16))));}{const char*_tmp3F1;Cyc_add_ccarg(((_tmp3F1="-no-cpp-precomp",
_tag_dyneither(_tmp3F1,sizeof(char),16))));};}static void Cyc_set_lineno(){Cyc_generate_line_directives_r=
1;Cyc_set_save_temps();}static void Cyc_set_inline_functions(){Cyc_inline_functions_r=
1;}static void Cyc_set_elim_se(){Cyc_elim_se_r=1;Cyc_set_inline_functions();}
static void Cyc_set_tovc(){Cyc_Cyclone_tovc_r=1;{const char*_tmp3F2;Cyc_add_ccarg(((
_tmp3F2="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp3F2,sizeof(char),18))));}Cyc_set_elim_se();}
static void Cyc_set_noboundschecks(){const char*_tmp3F3;Cyc_add_ccarg(((_tmp3F3="-DNO_CYC_BOUNDS_CHECKS",
_tag_dyneither(_tmp3F3,sizeof(char),23))));}static void Cyc_set_nonullchecks(){
const char*_tmp3F4;Cyc_add_ccarg(((_tmp3F4="-DNO_CYC_NULL_CHECKS",_tag_dyneither(
_tmp3F4,sizeof(char),21))));}static void Cyc_set_nochecks(){Cyc_set_noboundschecks();
Cyc_set_nonullchecks();Cyc_nocheck_r=1;}static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=
0;{const char*_tmp3F5;Cyc_add_ccarg(((_tmp3F5="-DNO_CYC_PREFIX",_tag_dyneither(
_tmp3F5,sizeof(char),16))));};}static void Cyc_set_pa(){Cyc_pa_r=1;{const char*
_tmp3F6;Cyc_add_ccarg(((_tmp3F6="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp3F6,
sizeof(char),21))));}{const char*_tmp3F7;Cyc_add_cpparg(((_tmp3F7="-DCYC_REGION_PROFILE",
_tag_dyneither(_tmp3F7,sizeof(char),21))));};}static void Cyc_set_pg(){Cyc_pg_r=1;{
const char*_tmp3F8;Cyc_add_ccarg(((_tmp3F8="-pg",_tag_dyneither(_tmp3F8,sizeof(
char),4))));};}static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=1;{
const char*_tmp3F9;Cyc_add_ccarg(((_tmp3F9="-S",_tag_dyneither(_tmp3F9,sizeof(
char),3))));};}static void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=1;Cyc_Toc_warn_all_null_deref=
1;Cyc_NewControlFlow_warn_lose_unique=1;}enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,
Cyc_CYCLONEFILE  = 1};static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;static
void Cyc_set_inputtype(struct _dyneither_ptr s){const char*_tmp3FA;if(Cyc_strcmp((
struct _dyneither_ptr)s,((_tmp3FA="cyc",_tag_dyneither(_tmp3FA,sizeof(char),4))))
== 0)Cyc_intype=Cyc_CYCLONEFILE;else{const char*_tmp3FB;if(Cyc_strcmp((struct
_dyneither_ptr)s,((_tmp3FB="none",_tag_dyneither(_tmp3FB,sizeof(char),5))))== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{const char*_tmp3FF;void*_tmp3FE[1];struct Cyc_String_pa_struct
_tmp3FD;(_tmp3FD.tag=0,((_tmp3FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
s),((_tmp3FE[0]=& _tmp3FD,Cyc_fprintf(Cyc_stderr,((_tmp3FF="Input type '%s' not supported\n",
_tag_dyneither(_tmp3FF,sizeof(char),31))),_tag_dyneither(_tmp3FE,sizeof(void*),1)))))));}}}
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct
_dyneither_ptr*output_file){struct _dyneither_ptr _tmp42=Cyc_Filename_dirname(
output_file == 0?s:*output_file);struct _dyneither_ptr _tmp43=Cyc_Filename_chop_extension((
struct _dyneither_ptr)Cyc_Filename_basename(s));struct _dyneither_ptr _tmp44=Cyc_strlen((
struct _dyneither_ptr)_tmp42)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp42,(
struct _dyneither_ptr)_tmp43): _tmp43;return(struct _dyneither_ptr)_tmp44;}static
struct Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(struct
_dyneither_ptr s){if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){{
struct _dyneither_ptr*_tmp402;struct Cyc_List_List*_tmp401;Cyc_cyclone_files=((
_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401->hd=((_tmp402=_cycalloc(sizeof(*
_tmp402)),((_tmp402[0]=s,_tmp402)))),((_tmp401->tl=Cyc_cyclone_files,_tmp401))))));}{
struct _dyneither_ptr _tmp47=Cyc_make_base_filename(s,Cyc_output_file);const char*
_tmp403;struct _dyneither_ptr _tmp48=Cyc_strconcat((struct _dyneither_ptr)_tmp47,((
_tmp403=".c",_tag_dyneither(_tmp403,sizeof(char),3))));Cyc_add_ccarg((struct
_dyneither_ptr)_tmp48);};}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(
struct _dyneither_ptr s){if(Cyc_save_temps_r)return;else{remove((const char*)
_untag_dyneither_ptr(s,sizeof(char),1));}}struct _tuple19{struct _dyneither_ptr*f1;
struct _dyneither_ptr*f2;};struct Cyc_List_List*Cyc_read_specs(struct
_dyneither_ptr file){struct Cyc_List_List*_tmp4A=0;int c;int i;char strname[256];char
strvalue[4096];struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(file,sizeof(char),1),(const char*)"r");if(spec_file == 0){{
const char*_tmp407;void*_tmp406[1];struct Cyc_String_pa_struct _tmp405;(_tmp405.tag=
0,((_tmp405.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp406[0]=&
_tmp405,Cyc_fprintf(Cyc_stderr,((_tmp407="Error opening spec file %s\n",
_tag_dyneither(_tmp407,sizeof(char),28))),_tag_dyneither(_tmp406,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;}while(1){while(1){c=Cyc_fgetc((
struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;if(c == '*')break;if(c != -
1){{const char*_tmp40C;void*_tmp40B[2];struct Cyc_String_pa_struct _tmp40A;struct
Cyc_Int_pa_struct _tmp409;(_tmp409.tag=1,((_tmp409.f1=(unsigned long)c,((_tmp40A.tag=
0,((_tmp40A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp40B[0]=&
_tmp40A,((_tmp40B[1]=& _tmp409,Cyc_fprintf(Cyc_stderr,((_tmp40C="Error reading spec file %s: unexpected character '%c'\n",
_tag_dyneither(_tmp40C,sizeof(char),55))),_tag_dyneither(_tmp40B,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}goto CLEANUP_AND_RETURN;}
JUST_AFTER_STAR: i=0;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(c == 
- 1){{const char*_tmp410;void*_tmp40F[1];struct Cyc_String_pa_struct _tmp40E;(
_tmp40E.tag=0,((_tmp40E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((
_tmp40F[0]=& _tmp40E,Cyc_fprintf(Cyc_stderr,((_tmp410="Error reading spec file %s: unexpected EOF\n",
_tag_dyneither(_tmp410,sizeof(char),44))),_tag_dyneither(_tmp40F,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';break;}strname[
_check_known_subscript_notnull(256,i)]=(char)c;++ i;if(i >= 256){{const char*
_tmp414;void*_tmp413[1];struct Cyc_String_pa_struct _tmp412;(_tmp412.tag=0,((
_tmp412.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp413[0]=&
_tmp412,Cyc_fprintf(Cyc_stderr,((_tmp414="Error reading spec file %s: string name too long\n",
_tag_dyneither(_tmp414,sizeof(char),50))),_tag_dyneither(_tmp413,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}while(1){c=
Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;break;}if(c == '*'){{
struct _tuple19*_tmp421;struct _dyneither_ptr*_tmp420;const char*_tmp41F;struct
_dyneither_ptr*_tmp41E;struct Cyc_List_List*_tmp41D;_tmp4A=((_tmp41D=_cycalloc(
sizeof(*_tmp41D)),((_tmp41D->hd=((_tmp421=_cycalloc(sizeof(*_tmp421)),((_tmp421->f1=((
_tmp420=_cycalloc(sizeof(*_tmp420)),((_tmp420[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp420)))),((_tmp421->f2=((_tmp41E=
_cycalloc(sizeof(*_tmp41E)),((_tmp41E[0]=(struct _dyneither_ptr)Cyc_strdup(((
_tmp41F="",_tag_dyneither(_tmp41F,sizeof(char),1)))),_tmp41E)))),_tmp421)))))),((
_tmp41D->tl=_tmp4A,_tmp41D))))));}goto JUST_AFTER_STAR;}strvalue[0]=(char)c;i=1;
while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if((c == - 1  || c == '\n')
 || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;++ i;if(i >= 4096){{const
char*_tmp426;void*_tmp425[2];struct Cyc_String_pa_struct _tmp424;struct Cyc_String_pa_struct
_tmp423;(_tmp423.tag=0,((_tmp423.f1=(struct _dyneither_ptr)_tag_dyneither(strname,
sizeof(char),256),((_tmp424.tag=0,((_tmp424.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)file),((_tmp425[0]=& _tmp424,((_tmp425[1]=& _tmp423,Cyc_fprintf(Cyc_stderr,((
_tmp426="Error reading spec file %s: value of %s too long\n",_tag_dyneither(
_tmp426,sizeof(char),50))),_tag_dyneither(_tmp425,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}{struct
_tuple19*_tmp42F;struct _dyneither_ptr*_tmp42E;struct _dyneither_ptr*_tmp42D;
struct Cyc_List_List*_tmp42C;_tmp4A=((_tmp42C=_cycalloc(sizeof(*_tmp42C)),((
_tmp42C->hd=((_tmp42F=_cycalloc(sizeof(*_tmp42F)),((_tmp42F->f1=((_tmp42E=
_cycalloc(sizeof(*_tmp42E)),((_tmp42E[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp42E)))),((_tmp42F->f2=((_tmp42D=
_cycalloc(sizeof(*_tmp42D)),((_tmp42D[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strvalue,sizeof(char),4096)),_tmp42D)))),_tmp42F)))))),((_tmp42C->tl=
_tmp4A,_tmp42C))))));}if(c == - 1)goto CLEANUP_AND_RETURN;}CLEANUP_AND_RETURN: Cyc_fclose((
struct Cyc___cycFILE*)spec_file);return _tmp4A;}struct _dyneither_ptr Cyc_split_specs(
struct _dyneither_ptr cmdline);static void _tmp439(struct _dyneither_ptr*_tmp6B,
unsigned int*_tmp438,unsigned int*_tmp437,struct _dyneither_ptr**_tmp435){for(*
_tmp438=0;*_tmp438 < *_tmp437;(*_tmp438)++){(*_tmp435)[*_tmp438]=*((struct
_dyneither_ptr**)(*_tmp6B).curr)[(int)*_tmp438];}}struct _dyneither_ptr Cyc_split_specs(
struct _dyneither_ptr cmdline){if((char*)cmdline.curr == (char*)(_tag_dyneither(0,0,
0)).curr)return _tag_dyneither(0,0,0);{unsigned long _tmp65=Cyc_strlen((struct
_dyneither_ptr)cmdline);struct Cyc_List_List*_tmp66=0;char buf[4096];int i=0;int j=0;
if(_tmp65 > 4096)goto DONE;while(1){while(1){if(i >= _tmp65)goto DONE;if(*((const
char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;if(!
isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))
break;++ i;}j=0;while(1){if(i >= _tmp65)break;if(*((const char*)
_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;if(isspace((int)*((
const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;if(*((
const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){++ i;if(i
>= _tmp65)break;if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),
i))== 0)break;buf[_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dyneither_subscript(cmdline,sizeof(char),i));++ j;}else{buf[
_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(
cmdline,sizeof(char),i));++ j;}++ i;}if(j < 4096)buf[_check_known_subscript_notnull(
4096,j)]='\000';{struct _dyneither_ptr*_tmp432;struct Cyc_List_List*_tmp431;_tmp66=((
_tmp431=_cycalloc(sizeof(*_tmp431)),((_tmp431->hd=((_tmp432=_cycalloc(sizeof(*
_tmp432)),((_tmp432[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,
sizeof(char),4096)),_tmp432)))),((_tmp431->tl=_tmp66,_tmp431))))));};}DONE:
_tmp66=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp66);{
struct Cyc_List_List*_tmp433;_tmp66=((_tmp433=_cycalloc(sizeof(*_tmp433)),((
_tmp433->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",
sizeof(char),1),((_tmp433->tl=_tmp66,_tmp433))))));}{struct _dyneither_ptr _tmp6B=((
struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp66);
unsigned int _tmp438;unsigned int _tmp437;struct _dyneither_ptr _tmp436;struct
_dyneither_ptr*_tmp435;unsigned int _tmp434;struct _dyneither_ptr _tmp6C=(_tmp434=
_get_dyneither_size(_tmp6B,sizeof(struct _dyneither_ptr*)),((_tmp435=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp434)),((
_tmp436=_tag_dyneither(_tmp435,sizeof(struct _dyneither_ptr),_tmp434),((((_tmp437=
_tmp434,_tmp439(& _tmp6B,& _tmp438,& _tmp437,& _tmp435))),_tmp436)))))));return
_tmp6C;};};}int Cyc_compile_failure=0;struct Cyc___cycFILE*Cyc_try_file_open(
struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr
msg_part){struct _handler_cons _tmp71;_push_handler(& _tmp71);{int _tmp73=0;if(
setjmp(_tmp71.handler))_tmp73=1;if(!_tmp73){{struct Cyc___cycFILE*_tmp74=(struct
Cyc___cycFILE*)Cyc_file_open(filename,mode);_npop_handler(0);return _tmp74;};
_pop_handler();}else{void*_tmp72=(void*)_exn_thrown;void*_tmp76=_tmp72;_LL1:;
_LL2: Cyc_compile_failure=1;{const char*_tmp43E;void*_tmp43D[2];struct Cyc_String_pa_struct
_tmp43C;struct Cyc_String_pa_struct _tmp43B;(_tmp43B.tag=0,((_tmp43B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp43C.tag=0,((_tmp43C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp43D[0]=& _tmp43C,((
_tmp43D[1]=& _tmp43B,Cyc_fprintf(Cyc_stderr,((_tmp43E="\nError: couldn't open %s %s\n",
_tag_dyneither(_tmp43E,sizeof(char),29))),_tag_dyneither(_tmp43D,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(
_tmp76);_LL0:;}};}struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr
stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct
Cyc_List_List*tds){const char*_tmp43F;struct _dyneither_ptr exn_string=(_tmp43F="",
_tag_dyneither(_tmp43F,sizeof(char),1));const char*_tmp440;struct _dyneither_ptr
explain_string=(_tmp440="",_tag_dyneither(_tmp440,sizeof(char),1));int other_exn=
0;struct Cyc_Core_Impossible_struct _tmp446;const char*_tmp445;struct Cyc_Core_Impossible_struct*
_tmp444;void*ex=(void*)((_tmp444=_cycalloc(sizeof(*_tmp444)),((_tmp444[0]=((
_tmp446.tag=Cyc_Core_Impossible,((_tmp446.f1=((_tmp445="",_tag_dyneither(_tmp445,
sizeof(char),1))),_tmp446)))),_tmp444))));struct Cyc_List_List*_tmp7B=0;{struct
_handler_cons _tmp7C;_push_handler(& _tmp7C);{int _tmp7E=0;if(setjmp(_tmp7C.handler))
_tmp7E=1;if(!_tmp7E){_tmp7B=f(env,tds);;_pop_handler();}else{void*_tmp7D=(void*)
_exn_thrown;void*_tmp80=_tmp7D;struct _dyneither_ptr _tmp82;struct _dyneither_ptr
_tmp85;struct _dyneither_ptr _tmp87;_LL6: {struct Cyc_Core_Impossible_struct*_tmp81=(
struct Cyc_Core_Impossible_struct*)_tmp80;if(_tmp81->tag != Cyc_Core_Impossible)
goto _LL8;else{_tmp82=_tmp81->f1;}}_LL7:{const char*_tmp447;exn_string=((_tmp447="Exception Core::Impossible",
_tag_dyneither(_tmp447,sizeof(char),27)));}explain_string=_tmp82;goto _LL5;_LL8: {
struct Cyc_Dict_Absent_struct*_tmp83=(struct Cyc_Dict_Absent_struct*)_tmp80;if(
_tmp83->tag != Cyc_Dict_Absent)goto _LLA;}_LL9:{const char*_tmp448;exn_string=((
_tmp448="Exception Dict::Absent",_tag_dyneither(_tmp448,sizeof(char),23)));}goto
_LL5;_LLA: {struct Cyc_Core_Invalid_argument_struct*_tmp84=(struct Cyc_Core_Invalid_argument_struct*)
_tmp80;if(_tmp84->tag != Cyc_Core_Invalid_argument)goto _LLC;else{_tmp85=_tmp84->f1;}}
_LLB:{const char*_tmp449;exn_string=((_tmp449="Exception Core::Invalid_argument",
_tag_dyneither(_tmp449,sizeof(char),33)));}explain_string=_tmp85;goto _LL5;_LLC: {
struct Cyc_Core_Failure_struct*_tmp86=(struct Cyc_Core_Failure_struct*)_tmp80;if(
_tmp86->tag != Cyc_Core_Failure)goto _LLE;else{_tmp87=_tmp86->f1;}}_LLD:{const char*
_tmp44A;exn_string=((_tmp44A="Exception Core::Failure",_tag_dyneither(_tmp44A,
sizeof(char),24)));}explain_string=_tmp87;goto _LL5;_LLE:;_LLF: ex=_tmp80;
other_exn=1;{const char*_tmp44B;exn_string=((_tmp44B="Uncaught exception",
_tag_dyneither(_tmp44B,sizeof(char),19)));}goto _LL5;_LL10:;_LL11:(void)_throw(
_tmp80);_LL5:;}};}if(Cyc_Position_error_p())Cyc_compile_failure=1;{const char*
_tmp44C;if(Cyc_strcmp((struct _dyneither_ptr)exn_string,((_tmp44C="",
_tag_dyneither(_tmp44C,sizeof(char),1))))!= 0){Cyc_compile_failure=1;{const char*
_tmp452;void*_tmp451[3];struct Cyc_String_pa_struct _tmp450;struct Cyc_String_pa_struct
_tmp44F;struct Cyc_String_pa_struct _tmp44E;(_tmp44E.tag=0,((_tmp44E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)explain_string),((_tmp44F.tag=0,((_tmp44F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp450.tag=0,((
_tmp450.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)exn_string),((_tmp451[0]=&
_tmp450,((_tmp451[1]=& _tmp44F,((_tmp451[2]=& _tmp44E,Cyc_fprintf(Cyc_stderr,((
_tmp452="\n%s thrown during %s: %s",_tag_dyneither(_tmp452,sizeof(char),25))),
_tag_dyneither(_tmp451,sizeof(void*),3)))))))))))))))))));};}}if(Cyc_compile_failure){{
const char*_tmp455;void*_tmp454;(_tmp454=0,Cyc_fprintf(Cyc_stderr,((_tmp455="\nCOMPILATION FAILED!\n",
_tag_dyneither(_tmp455,sizeof(char),22))),_tag_dyneither(_tmp454,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);if(other_exn)(int)_throw(ex);return
_tmp7B;}else{if(Cyc_v_r){{const char*_tmp459;void*_tmp458[1];struct Cyc_String_pa_struct
_tmp457;(_tmp457.tag=0,((_tmp457.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
stage_name),((_tmp458[0]=& _tmp457,Cyc_fprintf(Cyc_stderr,((_tmp459="%s completed.\n",
_tag_dyneither(_tmp459,sizeof(char),15))),_tag_dyneither(_tmp458,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return _tmp7B;}}return _tmp7B;}struct
Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init(
1);{struct Cyc_List_List*_tmp9D=Cyc_Parse_parse_file(f);Cyc_Lex_lex_init(1);
return _tmp9D;};}struct _tuple20{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple20*tcenv,struct Cyc_List_List*
tds){struct _RegionHandle*_tmp9F;struct Cyc_Tcenv_Tenv*_tmpA0;struct _tuple20 _tmp9E=*
tcenv;_tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;Cyc_Tc_tc(_tmp9F,_tmpA0,1,tds);if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(_tmpA0,tds);return tds;}struct Cyc_List_List*Cyc_do_cfcheck(
int ignore,struct Cyc_List_List*tds){Cyc_NewControlFlow_cf_check(tds);return tds;}
struct _tuple21{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*
f3;};struct Cyc_List_List*Cyc_do_interface(struct _tuple21*params,struct Cyc_List_List*
tds){struct _tuple21 _tmpA2;struct Cyc_Tcenv_Tenv*_tmpA3;struct Cyc___cycFILE*_tmpA4;
struct Cyc___cycFILE*_tmpA5;struct _tuple21*_tmpA1=params;_tmpA2=*_tmpA1;_tmpA3=
_tmpA2.f1;_tmpA4=_tmpA2.f2;_tmpA5=_tmpA2.f3;{struct Cyc_Interface_I*_tmpA6=Cyc_Interface_extract(
_tmpA3->ae);if(_tmpA4 == 0)Cyc_Interface_save(_tmpA6,_tmpA5);else{struct Cyc_Interface_I*
_tmpA7=Cyc_Interface_parse((struct Cyc___cycFILE*)_tmpA4);const char*_tmp45E;const
char*_tmp45D;struct _tuple18*_tmp45C;if(!Cyc_Interface_is_subinterface(_tmpA7,
_tmpA6,((_tmp45C=_cycalloc(sizeof(*_tmp45C)),((_tmp45C->f1=((_tmp45D="written interface",
_tag_dyneither(_tmp45D,sizeof(char),18))),((_tmp45C->f2=((_tmp45E="maximal interface",
_tag_dyneither(_tmp45E,sizeof(char),18))),_tmp45C))))))))Cyc_compile_failure=1;
else{Cyc_Interface_save(_tmpA7,_tmpA5);}}return tds;};}struct Cyc_List_List*Cyc_do_translate(
int ignore,struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*
Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){Cyc_Tovc_elim_array_initializers=
Cyc_Cyclone_tovc_r;return Cyc_Tovc_tovc(tds);}static struct _dyneither_ptr Cyc_cyc_setjmp=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};static struct
_dyneither_ptr Cyc_cyc_include=(struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0
+ 0)};static void Cyc_set_cyc_include(struct _dyneither_ptr f){Cyc_cyc_include=f;}
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr){{const char*_tmp461;
void*_tmp460;(_tmp460=0,Cyc_fprintf(Cyc_stderr,((_tmp461="Internal error: copy_internal_file called with NULL\n",
_tag_dyneither(_tmp461,sizeof(char),53))),_tag_dyneither(_tmp460,sizeof(void*),0)));}
return 1;}{const char*_tmp463;const char*_tmp462;struct Cyc___cycFILE*_tmpAD=Cyc_try_file_open(
file,((_tmp462="r",_tag_dyneither(_tmp462,sizeof(char),2))),((_tmp463="internal compiler file",
_tag_dyneither(_tmp463,sizeof(char),23))));if(_tmpAD == 0)return 1;{unsigned long
n_read=1024;unsigned long n_written;char buf[1024];{unsigned int _tmpB4=1024;
unsigned int i;for(i=0;i < _tmpB4;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=
Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)
_tmpAD);if(n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmpAD)){Cyc_compile_failure=
1;{const char*_tmp467;void*_tmp466[1];struct Cyc_String_pa_struct _tmp465;(_tmp465.tag=
0,((_tmp465.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp466[0]=&
_tmp465,Cyc_fprintf(Cyc_stderr,((_tmp467="\nError: problem copying %s\n",
_tag_dyneither(_tmp467,sizeof(char),28))),_tag_dyneither(_tmp466,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}n_written=Cyc_fwrite(
_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);if(n_read != n_written){
Cyc_compile_failure=1;{const char*_tmp46B;void*_tmp46A[1];struct Cyc_String_pa_struct
_tmp469;(_tmp469.tag=0,((_tmp469.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
file),((_tmp46A[0]=& _tmp469,Cyc_fprintf(Cyc_stderr,((_tmp46B="\nError: problem copying %s\n",
_tag_dyneither(_tmp46B,sizeof(char),28))),_tag_dyneither(_tmp46A,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}}Cyc_fclose((struct Cyc___cycFILE*)
_tmpAD);return 0;};};}struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*
out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params _tmpB7=Cyc_tc_r?Cyc_Absynpp_cyc_params_r:
Cyc_Absynpp_c_params_r;_tmpB7.expand_typedefs=!Cyc_noexpand_r;_tmpB7.to_VC=Cyc_Cyclone_tovc_r;
_tmpB7.add_cyc_prefix=Cyc_add_cyc_namespace_r;_tmpB7.generate_line_directives=
Cyc_generate_line_directives_r;_tmpB7.print_full_evars=Cyc_print_full_evars_r;
_tmpB7.print_all_tvars=Cyc_print_all_tvars_r;_tmpB7.print_all_kinds=Cyc_print_all_kinds_r;
_tmpB7.print_all_effects=Cyc_print_all_effects_r;if(Cyc_inline_functions_r){
const char*_tmp46E;void*_tmp46D;(_tmp46D=0,Cyc_fprintf(out_file,((_tmp46E="#define _INLINE_FUNCTIONS\n",
_tag_dyneither(_tmp46E,sizeof(char),27))),_tag_dyneither(_tmp46D,sizeof(void*),0)));}
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){if(!Cyc_nocyc_setjmp_r){if(Cyc_compile_for_boot_r){
const char*_tmp471;void*_tmp470;(_tmp470=0,Cyc_fprintf(out_file,((_tmp471="#include <setjmp.h>\n",
_tag_dyneither(_tmp471,sizeof(char),21))),_tag_dyneither(_tmp470,sizeof(void*),0)));}
else{if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}if(Cyc_copy_internal_file(
Cyc_cyc_include,out_file))return tds;}if(Cyc_pp_r){Cyc_Absynpp_set_params(& _tmpB7);
Cyc_Absynpp_decllist2file(tds,out_file);}else{Cyc_Absyndump_set_params(& _tmpB7);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}Cyc_fflush((struct Cyc___cycFILE*)
out_file);return tds;}static struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)
_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((struct _dyneither_ptr*)((struct
Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}static struct Cyc_List_List*Cyc_split_by_char(
struct _dyneither_ptr s,char c){if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)
return 0;{struct Cyc_List_List*_tmpBC=0;unsigned long _tmpBD=Cyc_strlen((struct
_dyneither_ptr)s);while(_tmpBD > 0){struct _dyneither_ptr _tmpBE=Cyc_strchr(s,c);
if((char*)_tmpBE.curr == (char*)(_tag_dyneither(0,0,0)).curr){{struct
_dyneither_ptr*_tmp474;struct Cyc_List_List*_tmp473;_tmpBC=((_tmp473=_cycalloc(
sizeof(*_tmp473)),((_tmp473->hd=((_tmp474=_cycalloc(sizeof(*_tmp474)),((_tmp474[
0]=s,_tmp474)))),((_tmp473->tl=_tmpBC,_tmp473))))));}break;}else{{struct
_dyneither_ptr*_tmp477;struct Cyc_List_List*_tmp476;_tmpBC=((_tmp476=_cycalloc(
sizeof(*_tmp476)),((_tmp476->hd=((_tmp477=_cycalloc(sizeof(*_tmp477)),((_tmp477[
0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((
struct _dyneither_ptr)_tmpBE).curr - s.curr)/ sizeof(char))),_tmp477)))),((_tmp476->tl=
_tmpBC,_tmp476))))));}_tmpBD -=(((struct _dyneither_ptr)_tmpBE).curr - s.curr)/ 
sizeof(char);s=_dyneither_ptr_plus(_tmpBE,sizeof(char),1);}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBC);};}static int Cyc_file_exists(
struct _dyneither_ptr file){struct Cyc___cycFILE*f=0;{struct _handler_cons _tmpC3;
_push_handler(& _tmpC3);{int _tmpC5=0;if(setjmp(_tmpC3.handler))_tmpC5=1;if(!
_tmpC5){{const char*_tmp478;f=(struct Cyc___cycFILE*)Cyc_file_open(file,((_tmp478="r",
_tag_dyneither(_tmp478,sizeof(char),2))));};_pop_handler();}else{void*_tmpC4=(
void*)_exn_thrown;void*_tmpC8=_tmpC4;_LL13:;_LL14: goto _LL12;_LL15:;_LL16:(void)
_throw(_tmpC8);_LL12:;}};}if(f == 0)return 0;else{Cyc_fclose((struct Cyc___cycFILE*)
f);return 1;}}static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
const char*_tmp479;struct _dyneither_ptr tmp=(_tmp479="",_tag_dyneither(_tmp479,
sizeof(char),1));for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr _tmpC9=*((
struct _dyneither_ptr*)dirs->hd);if((char*)_tmpC9.curr == (char*)(_tag_dyneither(0,
0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpC9)== 0)continue;{const char*
_tmp47A;_tmpC9=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpC9,((
_tmp47A=":",_tag_dyneither(_tmp47A,sizeof(char),2))));}tmp=(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)_tmpC9,(struct _dyneither_ptr)tmp);}return tmp;}
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct
_dyneither_ptr file){if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr)
return 0;for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr _tmpCC=*((struct
_dyneither_ptr*)dirs->hd);if((char*)_tmpCC.curr == (char*)(_tag_dyneither(0,0,0)).curr
 || Cyc_strlen((struct _dyneither_ptr)_tmpCC)== 0)continue;{struct _dyneither_ptr s=(
struct _dyneither_ptr)Cyc_Filename_concat(_tmpCC,file);if(Cyc_file_exists(s)){
struct _dyneither_ptr*_tmp47B;return(_tmp47B=_cycalloc(sizeof(*_tmp47B)),((
_tmp47B[0]=s,_tmp47B)));}};}return 0;}static struct _dyneither_ptr Cyc_do_find(
struct Cyc_List_List*dirs,struct _dyneither_ptr file){struct _dyneither_ptr*_tmpCE=
Cyc_find(dirs,file);if(_tmpCE == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{
const char*_tmp480;void*_tmp47F[2];struct Cyc_String_pa_struct _tmp47E;struct Cyc_String_pa_struct
_tmp47D;(_tmp47D.tag=0,((_tmp47D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs)),((_tmp47E.tag=0,((_tmp47E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)file),((_tmp47F[0]=& _tmp47E,((_tmp47F[1]=& _tmp47D,Cyc_fprintf(
Cyc_stderr,((_tmp480="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp480,sizeof(char),56))),_tag_dyneither(_tmp47F,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Failure_struct
_tmp490;const char*_tmp48F;void*_tmp48E[2];struct Cyc_String_pa_struct _tmp48D;
struct Cyc_String_pa_struct _tmp48C;struct Cyc_Core_Failure_struct*_tmp48B;(int)
_throw((void*)((_tmp48B=_cycalloc(sizeof(*_tmp48B)),((_tmp48B[0]=((_tmp490.tag=
Cyc_Core_Failure,((_tmp490.f1=(struct _dyneither_ptr)((_tmp48C.tag=0,((_tmp48C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp48D.tag=
0,((_tmp48D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp48E[0]=&
_tmp48D,((_tmp48E[1]=& _tmp48C,Cyc_aprintf(((_tmp48F="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp48F,sizeof(char),56))),_tag_dyneither(_tmp48E,sizeof(void*),2)))))))))))))),
_tmp490)))),_tmp48B)))));};}return*_tmpCE;}static struct Cyc_List_List*Cyc_add_subdir(
struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*_tmpD9=
0;for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr*_tmp493;struct Cyc_List_List*
_tmp492;_tmpD9=((_tmp492=_cycalloc(sizeof(*_tmp492)),((_tmp492->hd=((_tmp493=
_cycalloc(sizeof(*_tmp493)),((_tmp493[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((
struct _dyneither_ptr*)dirs->hd),subdir),_tmp493)))),((_tmp492->tl=_tmpD9,_tmp492))))));}
_tmpD9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD9);
return _tmpD9;}static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,
struct _dyneither_ptr subdir){struct Cyc_List_List*_tmpDC=0;for(0;dirs != 0;dirs=
dirs->tl){{struct Cyc_List_List*_tmp494;_tmpDC=((_tmp494=_cycalloc(sizeof(*
_tmp494)),((_tmp494->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp494->tl=_tmpDC,
_tmp494))))));}{struct _dyneither_ptr*_tmp497;struct Cyc_List_List*_tmp496;_tmpDC=((
_tmp496=_cycalloc(sizeof(*_tmp496)),((_tmp496->hd=((_tmp497=_cycalloc(sizeof(*
_tmp497)),((_tmp497[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct
_dyneither_ptr*)dirs->hd),subdir),_tmp497)))),((_tmp496->tl=_tmpDC,_tmp496))))));};}
_tmpDC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDC);
return _tmpDC;}static int Cyc_is_other_special(char c){switch(c){case '\\': _LL17: goto
_LL18;case '"': _LL18: goto _LL19;case ';': _LL19: goto _LL1A;case '&': _LL1A: goto _LL1B;
case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto _LL1D;case '|': _LL1D: goto _LL1E;case '^':
_LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;case '>': _LL20: goto _LL21;case '\n': _LL21:
goto _LL22;case '\t': _LL22: return 1;default: _LL23: return 0;}}static struct
_dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp4A0(
unsigned int*_tmp49F,unsigned int*_tmp49E,char**_tmp49C){for(*_tmp49F=0;*_tmp49F
< *_tmp49E;(*_tmp49F)++){(*_tmp49C)[*_tmp49F]='\000';}}static struct
_dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){unsigned long _tmpE0=
Cyc_strlen((struct _dyneither_ptr)s);int _tmpE1=0;int _tmpE2=0;{int i=0;for(0;i < 
_tmpE0;++ i){char _tmpE3=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpE3 == '\'')++ _tmpE1;else{if(Cyc_is_other_special(_tmpE3))++ _tmpE2;}}}if(
_tmpE1 == 0  && _tmpE2 == 0)return s;if(_tmpE1 == 0){struct _dyneither_ptr*_tmp49A;
struct _dyneither_ptr*_tmp499[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((
_tmp499[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",
sizeof(char),2),((_tmp499[1]=((_tmp49A=_cycalloc(sizeof(*_tmp49A)),((_tmp49A[0]=(
struct _dyneither_ptr)s,_tmp49A)))),((_tmp499[0]=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp499,sizeof(struct
_dyneither_ptr*),3)))))))));}{unsigned long _tmpE8=(_tmpE0 + _tmpE1)+ _tmpE2;
unsigned int _tmp49F;unsigned int _tmp49E;struct _dyneither_ptr _tmp49D;char*_tmp49C;
unsigned int _tmp49B;struct _dyneither_ptr s2=(_tmp49B=_tmpE8 + 1,((_tmp49C=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp49B + 1)),((_tmp49D=_tag_dyneither(
_tmp49C,sizeof(char),_tmp49B + 1),((((_tmp49E=_tmp49B,((_tmp4A0(& _tmp49F,& _tmp49E,&
_tmp49C),_tmp49C[_tmp49E]=(char)0)))),_tmp49D)))))));int _tmpE9=0;int _tmpEA=0;
for(0;_tmpE9 < _tmpE0;++ _tmpE9){char _tmpEB=*((const char*)
_check_dyneither_subscript(s,sizeof(char),_tmpE9));if(_tmpEB == '\''  || Cyc_is_other_special(
_tmpEB)){char _tmp4A3;char _tmp4A2;struct _dyneither_ptr _tmp4A1;(_tmp4A1=
_dyneither_ptr_plus(s2,sizeof(char),_tmpEA ++),((_tmp4A2=*((char*)
_check_dyneither_subscript(_tmp4A1,sizeof(char),0)),((_tmp4A3='\\',((
_get_dyneither_size(_tmp4A1,sizeof(char))== 1  && (_tmp4A2 == '\000'  && _tmp4A3 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp4A1.curr)=_tmp4A3)))))));}{char _tmp4A6;char
_tmp4A5;struct _dyneither_ptr _tmp4A4;(_tmp4A4=_dyneither_ptr_plus(s2,sizeof(char),
_tmpEA ++),((_tmp4A5=*((char*)_check_dyneither_subscript(_tmp4A4,sizeof(char),0)),((
_tmp4A6=_tmpEB,((_get_dyneither_size(_tmp4A4,sizeof(char))== 1  && (_tmp4A5 == '\000'
 && _tmp4A6 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4A4.curr)=_tmp4A6)))))));};}
return(struct _dyneither_ptr)s2;};}static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp){struct _dyneither_ptr*_tmp4A7;return(_tmp4A7=_cycalloc(
sizeof(*_tmp4A7)),((_tmp4A7[0]=Cyc_sh_escape_string(*sp),_tmp4A7)));}static void
Cyc_process_file(struct _dyneither_ptr filename){struct _dyneither_ptr _tmpF7=Cyc_make_base_filename(
filename,Cyc_output_file);const char*_tmp4A8;struct _dyneither_ptr _tmpF8=Cyc_strconcat((
struct _dyneither_ptr)_tmpF7,((_tmp4A8=".cyp",_tag_dyneither(_tmp4A8,sizeof(char),
5))));const char*_tmp4A9;struct _dyneither_ptr _tmpF9=Cyc_strconcat((struct
_dyneither_ptr)_tmpF7,((_tmp4A9=".cyci",_tag_dyneither(_tmp4A9,sizeof(char),6))));
const char*_tmp4AA;struct _dyneither_ptr _tmpFA=Cyc_strconcat((struct _dyneither_ptr)
_tmpF7,((_tmp4AA=".cycio",_tag_dyneither(_tmp4AA,sizeof(char),7))));const char*
_tmp4AB;struct _dyneither_ptr _tmpFB=Cyc_strconcat((struct _dyneither_ptr)_tmpF7,((
_tmp4AB=".c",_tag_dyneither(_tmp4AB,sizeof(char),3))));if(Cyc_v_r){const char*
_tmp4AF;void*_tmp4AE[1];struct Cyc_String_pa_struct _tmp4AD;(_tmp4AD.tag=0,((
_tmp4AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp4AE[0]=&
_tmp4AD,Cyc_fprintf(Cyc_stderr,((_tmp4AF="Compiling %s\n",_tag_dyneither(_tmp4AF,
sizeof(char),14))),_tag_dyneither(_tmp4AE,sizeof(void*),1)))))));}{const char*
_tmp4B1;const char*_tmp4B0;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((
_tmp4B0="r",_tag_dyneither(_tmp4B0,sizeof(char),2))),((_tmp4B1="input file",
_tag_dyneither(_tmp4B1,sizeof(char),11))));if(Cyc_compile_failure  || !((
unsigned int)f0))return;Cyc_fclose((struct Cyc___cycFILE*)f0);{const char*_tmp4B8;
struct _dyneither_ptr*_tmp4B7;const char*_tmp4B6;struct Cyc_List_List*_tmp4B5;
struct _dyneither_ptr _tmpFF=Cyc_str_sepstr(((_tmp4B5=_cycalloc(sizeof(*_tmp4B5)),((
_tmp4B5->hd=((_tmp4B7=_cycalloc(sizeof(*_tmp4B7)),((_tmp4B7[0]=(struct
_dyneither_ptr)((_tmp4B6="",_tag_dyneither(_tmp4B6,sizeof(char),1))),_tmp4B7)))),((
_tmp4B5->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),
_tmp4B5)))))),((_tmp4B8=" ",_tag_dyneither(_tmp4B8,sizeof(char),2))));struct
_dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmp100=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
const char*_tmp4B9;_tmp100=Cyc_add_subdir(_tmp100,((_tmp4B9="include",
_tag_dyneither(_tmp4B9,sizeof(char),8))));}if(Cyc_strlen((struct _dyneither_ptr)
def_inc_path)> 0){struct _dyneither_ptr*_tmp4BC;struct Cyc_List_List*_tmp4BB;
_tmp100=((_tmp4BB=_cycalloc(sizeof(*_tmp4BB)),((_tmp4BB->hd=((_tmp4BC=_cycalloc(
sizeof(*_tmp4BC)),((_tmp4BC[0]=def_inc_path,_tmp4BC)))),((_tmp4BB->tl=_tmp100,
_tmp4BB))))));}{char*_tmp104=getenv((const char*)"CYCLONE_INCLUDE_PATH");if((char*)
_tmp104 != 0){char*_tmp4BD;_tmp100=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp4BD=_tmp104,
_tag_dyneither(_tmp4BD,sizeof(char),_get_zero_arr_size_char((void*)_tmp4BD,1)))),':'),
_tmp100);}{const char*_tmp4C4;struct _dyneither_ptr*_tmp4C3;const char*_tmp4C2;
struct Cyc_List_List*_tmp4C1;struct _dyneither_ptr stdinc_string=(struct
_dyneither_ptr)Cyc_str_sepstr(((_tmp4C1=_cycalloc(sizeof(*_tmp4C1)),((_tmp4C1->hd=((
_tmp4C3=_cycalloc(sizeof(*_tmp4C3)),((_tmp4C3[0]=(struct _dyneither_ptr)((_tmp4C2="",
_tag_dyneither(_tmp4C2,sizeof(char),1))),_tmp4C3)))),((_tmp4C1->tl=((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmp100),_tmp4C1)))))),((_tmp4C4=" -I",_tag_dyneither(
_tmp4C4,sizeof(char),4))));struct _dyneither_ptr ofile_string;if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){const char*_tmp4C8;void*_tmp4C7[1];struct Cyc_String_pa_struct
_tmp4C6;ofile_string=(struct _dyneither_ptr)((_tmp4C6.tag=0,((_tmp4C6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((
_tmp4C7[0]=& _tmp4C6,Cyc_aprintf(((_tmp4C8=" > %s",_tag_dyneither(_tmp4C8,sizeof(
char),6))),_tag_dyneither(_tmp4C7,sizeof(void*),1))))))));}else{const char*
_tmp4C9;ofile_string=((_tmp4C9="",_tag_dyneither(_tmp4C9,sizeof(char),1)));}}
else{const char*_tmp4CD;void*_tmp4CC[1];struct Cyc_String_pa_struct _tmp4CB;
ofile_string=(struct _dyneither_ptr)((_tmp4CB.tag=0,((_tmp4CB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)
_tmpF8)),((_tmp4CC[0]=& _tmp4CB,Cyc_aprintf(((_tmp4CD=" > %s",_tag_dyneither(
_tmp4CD,sizeof(char),6))),_tag_dyneither(_tmp4CC,sizeof(void*),1))))))));}{
struct _dyneither_ptr fixup_string;if(Cyc_produce_dependencies){if(Cyc_dependencies_target
== 0){const char*_tmp4CE;fixup_string=((_tmp4CE=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
_tag_dyneither(_tmp4CE,sizeof(char),35)));}else{const char*_tmp4D2;void*_tmp4D1[1];
struct Cyc_String_pa_struct _tmp4D0;fixup_string=(struct _dyneither_ptr)((_tmp4D0.tag=
0,((_tmp4D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4D1[0]=& _tmp4D0,Cyc_aprintf(((
_tmp4D2=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4D2,sizeof(char),29))),
_tag_dyneither(_tmp4D1,sizeof(void*),1))))))));}}else{const char*_tmp4D3;
fixup_string=((_tmp4D3="",_tag_dyneither(_tmp4D3,sizeof(char),1)));}{const char*
_tmp4DC;void*_tmp4DB[6];struct Cyc_String_pa_struct _tmp4DA;struct Cyc_String_pa_struct
_tmp4D9;struct Cyc_String_pa_struct _tmp4D8;struct Cyc_String_pa_struct _tmp4D7;
struct Cyc_String_pa_struct _tmp4D6;struct Cyc_String_pa_struct _tmp4D5;struct
_dyneither_ptr _tmp112=(_tmp4D5.tag=0,((_tmp4D5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ofile_string),((_tmp4D6.tag=0,((_tmp4D6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)fixup_string),((_tmp4D7.tag=0,((_tmp4D7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(filename)),((_tmp4D8.tag=
0,((_tmp4D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((
_tmp4D9.tag=0,((_tmp4D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFF),((
_tmp4DA.tag=0,((_tmp4DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((
_tmp4DB[0]=& _tmp4DA,((_tmp4DB[1]=& _tmp4D9,((_tmp4DB[2]=& _tmp4D8,((_tmp4DB[3]=&
_tmp4D7,((_tmp4DB[4]=& _tmp4D6,((_tmp4DB[5]=& _tmp4D5,Cyc_aprintf(((_tmp4DC="%s %s%s %s%s%s",
_tag_dyneither(_tmp4DC,sizeof(char),15))),_tag_dyneither(_tmp4DB,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
if(Cyc_v_r){const char*_tmp4E0;void*_tmp4DF[1];struct Cyc_String_pa_struct _tmp4DE;(
_tmp4DE.tag=0,((_tmp4DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp112),((
_tmp4DF[0]=& _tmp4DE,Cyc_fprintf(Cyc_stderr,((_tmp4E0="%s\n",_tag_dyneither(
_tmp4E0,sizeof(char),4))),_tag_dyneither(_tmp4DF,sizeof(void*),1)))))));}if(
system((const char*)_untag_dyneither_ptr(_tmp112,sizeof(char),1))!= 0){Cyc_compile_failure=
1;{const char*_tmp4E3;void*_tmp4E2;(_tmp4E2=0,Cyc_fprintf(Cyc_stderr,((_tmp4E3="\nError: preprocessing\n",
_tag_dyneither(_tmp4E3,sizeof(char),23))),_tag_dyneither(_tmp4E2,sizeof(void*),0)));}
return;}if(Cyc_stop_after_cpp_r)return;Cyc_Position_reset_position((struct
_dyneither_ptr)_tmpF8);{const char*_tmp4E5;const char*_tmp4E4;struct Cyc___cycFILE*
in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF8,((_tmp4E4="r",
_tag_dyneither(_tmp4E4,sizeof(char),2))),((_tmp4E5="file",_tag_dyneither(_tmp4E5,
sizeof(char),5))));if(Cyc_compile_failure)return;{struct Cyc_List_List*tds=0;{
struct _handler_cons _tmp118;_push_handler(& _tmp118);{int _tmp11A=0;if(setjmp(
_tmp118.handler))_tmp11A=1;if(!_tmp11A){{const char*_tmp4E6;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4E6="parsing",_tag_dyneither(_tmp4E6,sizeof(char),8))),Cyc_do_parse,(struct
Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp119=(
void*)_exn_thrown;void*_tmp11D=_tmp119;_LL26:;_LL27: Cyc_file_close((struct Cyc___cycFILE*)
_check_null(in_file));Cyc_remove_file((struct _dyneither_ptr)_tmpF8);(int)_throw(
_tmp11D);_LL28:;_LL29:(void)_throw(_tmp11D);_LL25:;}};}Cyc_file_close((struct Cyc___cycFILE*)
in_file);if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpF8);
return;}if(Cyc_Absyn_porting_c_code){Cyc_Port_port(tds);return;}{struct
_RegionHandle _tmp11E=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp11E;
_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp11F=Cyc_Tcenv_tc_init(tc_rgn);if(
Cyc_parseonly_r)goto PRINTC;{struct _tuple20 _tmp4E7;struct _tuple20 _tmp120=(_tmp4E7.f1=
tc_rgn,((_tmp4E7.f2=_tmp11F,_tmp4E7)));{const char*_tmp4E8;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple20*,struct
Cyc_List_List*),struct _tuple20*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4E8="type checking",_tag_dyneither(_tmp4E8,sizeof(char),14))),Cyc_do_typecheck,&
_tmp120,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)
_tmpF8);_npop_handler(0);return;}if(Cyc_tc_r)goto PRINTC;{const char*_tmp4E9;tds=((
struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(
int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4E9="control-flow checking",_tag_dyneither(_tmp4E9,sizeof(char),22))),Cyc_do_cfcheck,
1,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpF8);
_npop_handler(0);return;}if(Cyc_ic_r){struct Cyc___cycFILE*inter_file=Cyc_fopen((
const char*)((char*)_untag_dyneither_ptr(_tmpF9,sizeof(char),1)),(const char*)"r");
const char*_tmp4EB;const char*_tmp4EA;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((
struct _dyneither_ptr)_tmpFA,((_tmp4EA="w",_tag_dyneither(_tmp4EA,sizeof(char),2))),((
_tmp4EB="interface object file",_tag_dyneither(_tmp4EB,sizeof(char),22))));if(
inter_objfile == 0){Cyc_compile_failure=1;_npop_handler(0);return;}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmpF9);{struct _tuple21 _tmp4EC;struct _tuple21 _tmp123=(
_tmp4EC.f1=_tmp11F,((_tmp4EC.f2=inter_file,((_tmp4EC.f3=(struct Cyc___cycFILE*)
inter_objfile,_tmp4EC)))));{const char*_tmp4ED;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple21*,struct
Cyc_List_List*),struct _tuple21*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4ED="interface checking",_tag_dyneither(_tmp4ED,sizeof(char),19))),Cyc_do_interface,&
_tmp123,tds);}if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)inter_file);
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);};}};}if(Cyc_cf_r)goto PRINTC;{
const char*_tmp4EE;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,
struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(((_tmp4EE="translation to C",_tag_dyneither(_tmp4EE,sizeof(
char),17))),Cyc_do_translate,1,tds);}if(Cyc_compile_failure){Cyc_remove_file((
struct _dyneither_ptr)_tmpF8);_npop_handler(0);return;}if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct _dyneither_ptr)_tmpF8);if(Cyc_compile_failure){
_npop_handler(0);return;}if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){const char*
_tmp4EF;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4EF="post-pass to VC",_tag_dyneither(_tmp4EF,sizeof(char),16))),Cyc_do_tovc,
1,tds);}if(Cyc_compile_failure){_npop_handler(0);return;}PRINTC: {struct Cyc___cycFILE*
out_file;if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){if(Cyc_output_file != 0){
const char*_tmp4F1;const char*_tmp4F0;out_file=Cyc_try_file_open(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4F0="w",_tag_dyneither(
_tmp4F0,sizeof(char),2))),((_tmp4F1="output file",_tag_dyneither(_tmp4F1,sizeof(
char),12))));}else{out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}else{if(Cyc_toc_r
 && Cyc_output_file != 0){const char*_tmp4F3;const char*_tmp4F2;out_file=Cyc_try_file_open(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4F2="w",_tag_dyneither(
_tmp4F2,sizeof(char),2))),((_tmp4F3="output file",_tag_dyneither(_tmp4F3,sizeof(
char),12))));}else{const char*_tmp4F5;const char*_tmp4F4;out_file=Cyc_try_file_open((
struct _dyneither_ptr)_tmpFB,((_tmp4F4="w",_tag_dyneither(_tmp4F4,sizeof(char),2))),((
_tmp4F5="output file",_tag_dyneither(_tmp4F5,sizeof(char),12))));}}if(Cyc_compile_failure
 || !((unsigned int)out_file)){_npop_handler(0);return;}{struct _handler_cons
_tmp131;_push_handler(& _tmp131);{int _tmp133=0;if(setjmp(_tmp131.handler))_tmp133=
1;if(!_tmp133){if(!Cyc_noprint_r){const char*_tmp4F6;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4F6="printing",_tag_dyneither(_tmp4F6,sizeof(char),9))),Cyc_do_print,(struct
Cyc___cycFILE*)out_file,tds);};_pop_handler();}else{void*_tmp132=(void*)
_exn_thrown;void*_tmp136=_tmp132;_LL2B:;_LL2C: Cyc_compile_failure=1;Cyc_file_close((
struct Cyc___cycFILE*)out_file);{struct _dyneither_ptr*_tmp4F9;struct Cyc_List_List*
_tmp4F8;Cyc_cfiles=((_tmp4F8=_cycalloc(sizeof(*_tmp4F8)),((_tmp4F8->hd=((_tmp4F9=
_cycalloc(sizeof(*_tmp4F9)),((_tmp4F9[0]=(struct _dyneither_ptr)_tmpFB,_tmp4F9)))),((
_tmp4F8->tl=Cyc_cfiles,_tmp4F8))))));}(int)_throw(_tmp136);_LL2D:;_LL2E:(void)
_throw(_tmp136);_LL2A:;}};}Cyc_file_close((struct Cyc___cycFILE*)out_file);{
struct _dyneither_ptr*_tmp4FC;struct Cyc_List_List*_tmp4FB;Cyc_cfiles=((_tmp4FB=
_cycalloc(sizeof(*_tmp4FB)),((_tmp4FB->hd=((_tmp4FC=_cycalloc(sizeof(*_tmp4FC)),((
_tmp4FC[0]=(struct _dyneither_ptr)_tmpFB,_tmp4FC)))),((_tmp4FB->tl=Cyc_cfiles,
_tmp4FB))))));};};_pop_region(tc_rgn);};};};};};};};};};}static char _tmp153[8]="<final>";
static struct _dyneither_ptr Cyc_final_str={_tmp153,_tmp153,_tmp153 + 8};static
struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;static struct Cyc_Interface_I*
Cyc_read_cycio(struct _dyneither_ptr*n){if(n == (struct _dyneither_ptr*)Cyc_final_strptr)
return Cyc_Interface_final();{struct _dyneither_ptr basename;{struct _handler_cons
_tmp154;_push_handler(& _tmp154);{int _tmp156=0;if(setjmp(_tmp154.handler))_tmp156=
1;if(!_tmp156){basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;
_pop_handler();}else{void*_tmp155=(void*)_exn_thrown;void*_tmp158=_tmp155;_LL30: {
struct Cyc_Core_Invalid_argument_struct*_tmp159=(struct Cyc_Core_Invalid_argument_struct*)
_tmp158;if(_tmp159->tag != Cyc_Core_Invalid_argument)goto _LL32;}_LL31: basename=*n;
goto _LL2F;_LL32:;_LL33:(void)_throw(_tmp158);_LL2F:;}};}{const char*_tmp4FD;
struct _dyneither_ptr _tmp15A=Cyc_strconcat((struct _dyneither_ptr)basename,((
_tmp4FD=".cycio",_tag_dyneither(_tmp4FD,sizeof(char),7))));const char*_tmp4FF;
const char*_tmp4FE;struct Cyc___cycFILE*_tmp15B=Cyc_try_file_open((struct
_dyneither_ptr)_tmp15A,((_tmp4FE="rb",_tag_dyneither(_tmp4FE,sizeof(char),3))),((
_tmp4FF="interface object file",_tag_dyneither(_tmp4FF,sizeof(char),22))));if(
_tmp15B == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();exit(1);}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmp15A);{struct Cyc_Interface_I*_tmp15C=Cyc_Interface_load((
struct Cyc___cycFILE*)_tmp15B);Cyc_file_close((struct Cyc___cycFILE*)_tmp15B);
return _tmp15C;};};};}static int Cyc_is_cfile(struct _dyneither_ptr*n){return*((
const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}void
GC_blacklist_warn_clear();struct _tuple22{struct _dyneither_ptr f1;int f2;struct
_dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct
_dyneither_ptr argv);static void _tmp505(unsigned int*_tmp504,unsigned int*_tmp503,
int**_tmp501){for(*_tmp504=0;*_tmp504 < *_tmp503;(*_tmp504)++){(*_tmp501)[*
_tmp504]=0;}}static void _tmp50D(unsigned int*_tmp50C,unsigned int*_tmp50B,struct
_dyneither_ptr**_tmp509){for(*_tmp50C=0;*_tmp50C < *_tmp50B;(*_tmp50C)++){(*
_tmp509)[*_tmp50C]=(struct _dyneither_ptr)_tag_dyneither(0,0,0);}}static void
_tmp513(unsigned int*_tmp512,unsigned int*_tmp511,struct _dyneither_ptr**_tmp50F){
for(*_tmp512=0;*_tmp512 < *_tmp511;(*_tmp512)++){(*_tmp50F)[*_tmp512]=(struct
_dyneither_ptr)_tag_dyneither(0,0,0);}}int Cyc_main(int argc,struct _dyneither_ptr
argv){GC_blacklist_warn_clear();{struct Cyc_List_List*cyclone_arch_path;struct
_dyneither_ptr def_lib_path;struct _dyneither_ptr comp=(struct _dyneither_ptr)
Cstring_to_string(Ccomp);struct _RegionHandle _tmp160=_new_region("r");struct
_RegionHandle*r=& _tmp160;_push_region(r);{unsigned int _tmp504;unsigned int
_tmp503;struct _dyneither_ptr _tmp502;int*_tmp501;unsigned int _tmp500;struct
_dyneither_ptr _tmp161=(_tmp500=(unsigned int)argc,((_tmp501=(int*)
_cycalloc_atomic(_check_times(sizeof(int),_tmp500)),((_tmp502=_tag_dyneither(
_tmp501,sizeof(int),_tmp500),((((_tmp503=_tmp500,_tmp505(& _tmp504,& _tmp503,&
_tmp501))),_tmp502)))))));int _tmp162=0;int i;int j;int k;for(i=1;i < argc;++ i){const
char*_tmp506;if(Cyc_strncmp(((_tmp506="-B",_tag_dyneither(_tmp506,sizeof(char),3))),(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,
sizeof(struct _dyneither_ptr),i)),2)== 0){*((int*)_check_dyneither_subscript(
_tmp161,sizeof(int),i))=1;++ _tmp162;}else{const char*_tmp507;if(Cyc_strcmp(((
_tmp507="-b",_tag_dyneither(_tmp507,sizeof(char),3))),(struct _dyneither_ptr)*((
struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i)))== 0){*((int*)_check_dyneither_subscript(_tmp161,sizeof(int),
i))=1;++ _tmp162;++ i;if(i >= argc)break;*((int*)_check_dyneither_subscript(_tmp161,
sizeof(int),i))=1;++ _tmp162;}}}{unsigned int _tmp50C;unsigned int _tmp50B;struct
_dyneither_ptr _tmp50A;struct _dyneither_ptr*_tmp509;unsigned int _tmp508;struct
_dyneither_ptr _tmp165=(_tmp508=(unsigned int)(_tmp162 + 1),((_tmp509=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp508)),((
_tmp50A=_tag_dyneither(_tmp509,sizeof(struct _dyneither_ptr),_tmp508),((((_tmp50B=
_tmp508,_tmp50D(& _tmp50C,& _tmp50B,& _tmp509))),_tmp50A)))))));unsigned int _tmp512;
unsigned int _tmp511;struct _dyneither_ptr _tmp510;struct _dyneither_ptr*_tmp50F;
unsigned int _tmp50E;struct _dyneither_ptr _tmp166=(_tmp50E=(unsigned int)(argc - 
_tmp162),((_tmp50F=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct
_dyneither_ptr),_tmp50E)),((_tmp510=_tag_dyneither(_tmp50F,sizeof(struct
_dyneither_ptr),_tmp50E),((((_tmp511=_tmp50E,_tmp513(& _tmp512,& _tmp511,& _tmp50F))),
_tmp510)))))));*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp165,
sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)
_check_dyneither_subscript(_tmp166,sizeof(struct _dyneither_ptr),0))=*((struct
_dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){if(*((int*)_check_dyneither_subscript(_tmp161,
sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp165,
sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)
_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{*((struct
_dyneither_ptr*)_check_dyneither_subscript(_tmp166,sizeof(struct _dyneither_ptr),
k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i));}}{struct _tuple22*_tmpA36;const char*_tmpA35;const char*
_tmpA34;struct Cyc_Arg_Set_spec_struct _tmpA33;struct Cyc_Arg_Set_spec_struct*
_tmpA32;const char*_tmpA31;struct _tuple22*_tmpA30;const char*_tmpA2F;const char*
_tmpA2E;struct Cyc_Arg_Unit_spec_struct _tmpA2D;struct Cyc_Arg_Unit_spec_struct*
_tmpA2C;const char*_tmpA2B;struct _tuple22*_tmpA2A;const char*_tmpA29;const char*
_tmpA28;struct Cyc_Arg_String_spec_struct _tmpA27;struct Cyc_Arg_String_spec_struct*
_tmpA26;const char*_tmpA25;struct _tuple22*_tmpA24;const char*_tmpA23;const char*
_tmpA22;struct Cyc_Arg_Flag_spec_struct _tmpA21;struct Cyc_Arg_Flag_spec_struct*
_tmpA20;const char*_tmpA1F;struct _tuple22*_tmpA1E;const char*_tmpA1D;const char*
_tmpA1C;struct Cyc_Arg_Flag_spec_struct _tmpA1B;struct Cyc_Arg_Flag_spec_struct*
_tmpA1A;const char*_tmpA19;struct _tuple22*_tmpA18;const char*_tmpA17;const char*
_tmpA16;struct Cyc_Arg_Flag_spec_struct _tmpA15;struct Cyc_Arg_Flag_spec_struct*
_tmpA14;const char*_tmpA13;struct _tuple22*_tmpA12;const char*_tmpA11;const char*
_tmpA10;struct Cyc_Arg_Flag_spec_struct _tmpA0F;struct Cyc_Arg_Flag_spec_struct*
_tmpA0E;const char*_tmpA0D;struct _tuple22*_tmpA0C;const char*_tmpA0B;const char*
_tmpA0A;struct Cyc_Arg_Flag_spec_struct _tmpA09;struct Cyc_Arg_Flag_spec_struct*
_tmpA08;const char*_tmpA07;struct _tuple22*_tmpA06;const char*_tmpA05;const char*
_tmpA04;struct Cyc_Arg_Flag_spec_struct _tmpA03;struct Cyc_Arg_Flag_spec_struct*
_tmpA02;const char*_tmpA01;struct _tuple22*_tmpA00;const char*_tmp9FF;const char*
_tmp9FE;struct Cyc_Arg_Unit_spec_struct _tmp9FD;struct Cyc_Arg_Unit_spec_struct*
_tmp9FC;const char*_tmp9FB;struct _tuple22*_tmp9FA;const char*_tmp9F9;const char*
_tmp9F8;struct Cyc_Arg_String_spec_struct _tmp9F7;struct Cyc_Arg_String_spec_struct*
_tmp9F6;const char*_tmp9F5;struct _tuple22*_tmp9F4;const char*_tmp9F3;const char*
_tmp9F2;struct Cyc_Arg_Flag_spec_struct _tmp9F1;struct Cyc_Arg_Flag_spec_struct*
_tmp9F0;const char*_tmp9EF;struct _tuple22*_tmp9EE;const char*_tmp9ED;const char*
_tmp9EC;struct Cyc_Arg_Flag_spec_struct _tmp9EB;struct Cyc_Arg_Flag_spec_struct*
_tmp9EA;const char*_tmp9E9;struct _tuple22*_tmp9E8;const char*_tmp9E7;const char*
_tmp9E6;struct Cyc_Arg_Flag_spec_struct _tmp9E5;struct Cyc_Arg_Flag_spec_struct*
_tmp9E4;const char*_tmp9E3;struct _tuple22*_tmp9E2;const char*_tmp9E1;const char*
_tmp9E0;struct Cyc_Arg_Flag_spec_struct _tmp9DF;struct Cyc_Arg_Flag_spec_struct*
_tmp9DE;const char*_tmp9DD;struct _tuple22*_tmp9DC;const char*_tmp9DB;const char*
_tmp9DA;struct Cyc_Arg_Flag_spec_struct _tmp9D9;struct Cyc_Arg_Flag_spec_struct*
_tmp9D8;const char*_tmp9D7;struct _tuple22*_tmp9D6;const char*_tmp9D5;const char*
_tmp9D4;struct Cyc_Arg_Flag_spec_struct _tmp9D3;struct Cyc_Arg_Flag_spec_struct*
_tmp9D2;const char*_tmp9D1;struct _tuple22*_tmp9D0;const char*_tmp9CF;const char*
_tmp9CE;struct Cyc_Arg_Flag_spec_struct _tmp9CD;struct Cyc_Arg_Flag_spec_struct*
_tmp9CC;const char*_tmp9CB;struct _tuple22*_tmp9CA;const char*_tmp9C9;const char*
_tmp9C8;struct Cyc_Arg_Flag_spec_struct _tmp9C7;struct Cyc_Arg_Flag_spec_struct*
_tmp9C6;const char*_tmp9C5;struct _tuple22*_tmp9C4;const char*_tmp9C3;const char*
_tmp9C2;struct Cyc_Arg_Unit_spec_struct _tmp9C1;struct Cyc_Arg_Unit_spec_struct*
_tmp9C0;const char*_tmp9BF;struct _tuple22*_tmp9BE;const char*_tmp9BD;const char*
_tmp9BC;struct Cyc_Arg_Unit_spec_struct _tmp9BB;struct Cyc_Arg_Unit_spec_struct*
_tmp9BA;const char*_tmp9B9;struct _tuple22*_tmp9B8;const char*_tmp9B7;const char*
_tmp9B6;struct Cyc_Arg_Unit_spec_struct _tmp9B5;struct Cyc_Arg_Unit_spec_struct*
_tmp9B4;const char*_tmp9B3;struct _tuple22*_tmp9B2;const char*_tmp9B1;const char*
_tmp9B0;struct Cyc_Arg_Unit_spec_struct _tmp9AF;struct Cyc_Arg_Unit_spec_struct*
_tmp9AE;const char*_tmp9AD;struct _tuple22*_tmp9AC;const char*_tmp9AB;const char*
_tmp9AA;struct Cyc_Arg_Flag_spec_struct _tmp9A9;struct Cyc_Arg_Flag_spec_struct*
_tmp9A8;const char*_tmp9A7;struct _tuple22*_tmp9A6;const char*_tmp9A5;const char*
_tmp9A4;struct Cyc_Arg_String_spec_struct _tmp9A3;struct Cyc_Arg_String_spec_struct*
_tmp9A2;const char*_tmp9A1;struct _tuple22*_tmp9A0;const char*_tmp99F;const char*
_tmp99E;struct Cyc_Arg_String_spec_struct _tmp99D;struct Cyc_Arg_String_spec_struct*
_tmp99C;const char*_tmp99B;struct _tuple22*_tmp99A;const char*_tmp999;const char*
_tmp998;struct Cyc_Arg_Set_spec_struct _tmp997;struct Cyc_Arg_Set_spec_struct*
_tmp996;const char*_tmp995;struct _tuple22*_tmp994;const char*_tmp993;const char*
_tmp992;struct Cyc_Arg_Unit_spec_struct _tmp991;struct Cyc_Arg_Unit_spec_struct*
_tmp990;const char*_tmp98F;struct _tuple22*_tmp98E;const char*_tmp98D;const char*
_tmp98C;struct Cyc_Arg_Set_spec_struct _tmp98B;struct Cyc_Arg_Set_spec_struct*
_tmp98A;const char*_tmp989;struct _tuple22*_tmp988;const char*_tmp987;const char*
_tmp986;struct Cyc_Arg_Set_spec_struct _tmp985;struct Cyc_Arg_Set_spec_struct*
_tmp984;const char*_tmp983;struct _tuple22*_tmp982;const char*_tmp981;const char*
_tmp980;struct Cyc_Arg_Set_spec_struct _tmp97F;struct Cyc_Arg_Set_spec_struct*
_tmp97E;const char*_tmp97D;struct _tuple22*_tmp97C;const char*_tmp97B;const char*
_tmp97A;struct Cyc_Arg_Set_spec_struct _tmp979;struct Cyc_Arg_Set_spec_struct*
_tmp978;const char*_tmp977;struct _tuple22*_tmp976;const char*_tmp975;const char*
_tmp974;struct Cyc_Arg_Set_spec_struct _tmp973;struct Cyc_Arg_Set_spec_struct*
_tmp972;const char*_tmp971;struct _tuple22*_tmp970;const char*_tmp96F;const char*
_tmp96E;struct Cyc_Arg_Set_spec_struct _tmp96D;struct Cyc_Arg_Set_spec_struct*
_tmp96C;const char*_tmp96B;struct _tuple22*_tmp96A;const char*_tmp969;const char*
_tmp968;struct Cyc_Arg_Set_spec_struct _tmp967;struct Cyc_Arg_Set_spec_struct*
_tmp966;const char*_tmp965;struct _tuple22*_tmp964;const char*_tmp963;const char*
_tmp962;struct Cyc_Arg_Set_spec_struct _tmp961;struct Cyc_Arg_Set_spec_struct*
_tmp960;const char*_tmp95F;struct _tuple22*_tmp95E;const char*_tmp95D;const char*
_tmp95C;struct Cyc_Arg_Clear_spec_struct _tmp95B;struct Cyc_Arg_Clear_spec_struct*
_tmp95A;const char*_tmp959;struct _tuple22*_tmp958;const char*_tmp957;const char*
_tmp956;struct Cyc_Arg_Set_spec_struct _tmp955;struct Cyc_Arg_Set_spec_struct*
_tmp954;const char*_tmp953;struct _tuple22*_tmp952;const char*_tmp951;const char*
_tmp950;struct Cyc_Arg_Clear_spec_struct _tmp94F;struct Cyc_Arg_Clear_spec_struct*
_tmp94E;const char*_tmp94D;struct _tuple22*_tmp94C;const char*_tmp94B;const char*
_tmp94A;struct Cyc_Arg_Unit_spec_struct _tmp949;struct Cyc_Arg_Unit_spec_struct*
_tmp948;const char*_tmp947;struct _tuple22*_tmp946;const char*_tmp945;const char*
_tmp944;struct Cyc_Arg_String_spec_struct _tmp943;struct Cyc_Arg_String_spec_struct*
_tmp942;const char*_tmp941;struct _tuple22*_tmp940;const char*_tmp93F;const char*
_tmp93E;struct Cyc_Arg_Unit_spec_struct _tmp93D;struct Cyc_Arg_Unit_spec_struct*
_tmp93C;const char*_tmp93B;struct _tuple22*_tmp93A;const char*_tmp939;const char*
_tmp938;struct Cyc_Arg_Set_spec_struct _tmp937;struct Cyc_Arg_Set_spec_struct*
_tmp936;const char*_tmp935;struct _tuple22*_tmp934;const char*_tmp933;const char*
_tmp932;struct Cyc_Arg_Unit_spec_struct _tmp931;struct Cyc_Arg_Unit_spec_struct*
_tmp930;const char*_tmp92F;struct _tuple22*_tmp92E;const char*_tmp92D;const char*
_tmp92C;struct Cyc_Arg_Unit_spec_struct _tmp92B;struct Cyc_Arg_Unit_spec_struct*
_tmp92A;const char*_tmp929;struct _tuple22*_tmp928;const char*_tmp927;const char*
_tmp926;struct Cyc_Arg_Unit_spec_struct _tmp925;struct Cyc_Arg_Unit_spec_struct*
_tmp924;const char*_tmp923;struct _tuple22*_tmp922;const char*_tmp921;const char*
_tmp920;struct Cyc_Arg_Unit_spec_struct _tmp91F;struct Cyc_Arg_Unit_spec_struct*
_tmp91E;const char*_tmp91D;struct _tuple22*_tmp91C;const char*_tmp91B;const char*
_tmp91A;struct Cyc_Arg_Unit_spec_struct _tmp919;struct Cyc_Arg_Unit_spec_struct*
_tmp918;const char*_tmp917;struct _tuple22*_tmp916;const char*_tmp915;const char*
_tmp914;struct Cyc_Arg_String_spec_struct _tmp913;struct Cyc_Arg_String_spec_struct*
_tmp912;const char*_tmp911;struct _tuple22*_tmp910;const char*_tmp90F;const char*
_tmp90E;struct Cyc_Arg_Unit_spec_struct _tmp90D;struct Cyc_Arg_Unit_spec_struct*
_tmp90C;const char*_tmp90B;struct _tuple22*_tmp90A;const char*_tmp909;const char*
_tmp908;struct Cyc_Arg_Unit_spec_struct _tmp907;struct Cyc_Arg_Unit_spec_struct*
_tmp906;const char*_tmp905;struct _tuple22*_tmp904;const char*_tmp903;const char*
_tmp902;struct Cyc_Arg_Set_spec_struct _tmp901;struct Cyc_Arg_Set_spec_struct*
_tmp900;const char*_tmp8FF;struct _tuple22*_tmp8FE;const char*_tmp8FD;const char*
_tmp8FC;struct Cyc_Arg_Set_spec_struct _tmp8FB;struct Cyc_Arg_Set_spec_struct*
_tmp8FA;const char*_tmp8F9;struct _tuple22*_tmp8F8;const char*_tmp8F7;const char*
_tmp8F6;struct Cyc_Arg_Set_spec_struct _tmp8F5;struct Cyc_Arg_Set_spec_struct*
_tmp8F4;const char*_tmp8F3;struct _tuple22*_tmp8F2;const char*_tmp8F1;const char*
_tmp8F0;struct Cyc_Arg_Set_spec_struct _tmp8EF;struct Cyc_Arg_Set_spec_struct*
_tmp8EE;const char*_tmp8ED;struct _tuple22*_tmp8EC;const char*_tmp8EB;const char*
_tmp8EA;struct Cyc_Arg_Set_spec_struct _tmp8E9;struct Cyc_Arg_Set_spec_struct*
_tmp8E8;const char*_tmp8E7;struct _tuple22*_tmp8E6;const char*_tmp8E5;const char*
_tmp8E4;struct Cyc_Arg_Set_spec_struct _tmp8E3;struct Cyc_Arg_Set_spec_struct*
_tmp8E2;const char*_tmp8E1;struct _tuple22*_tmp8E0;const char*_tmp8DF;const char*
_tmp8DE;struct Cyc_Arg_Set_spec_struct _tmp8DD;struct Cyc_Arg_Set_spec_struct*
_tmp8DC;const char*_tmp8DB;struct _tuple22*_tmp8DA;const char*_tmp8D9;const char*
_tmp8D8;struct Cyc_Arg_Set_spec_struct _tmp8D7;struct Cyc_Arg_Set_spec_struct*
_tmp8D6;const char*_tmp8D5;struct _tuple22*_tmp8D4;const char*_tmp8D3;const char*
_tmp8D2;struct Cyc_Arg_Set_spec_struct _tmp8D1;struct Cyc_Arg_Set_spec_struct*
_tmp8D0;const char*_tmp8CF;struct _tuple22*_tmp8CE;const char*_tmp8CD;const char*
_tmp8CC;struct Cyc_Arg_String_spec_struct _tmp8CB;struct Cyc_Arg_String_spec_struct*
_tmp8CA;const char*_tmp8C9;struct _tuple22*_tmp8C8;const char*_tmp8C7;const char*
_tmp8C6;struct Cyc_Arg_Set_spec_struct _tmp8C5;struct Cyc_Arg_Set_spec_struct*
_tmp8C4;const char*_tmp8C3;struct _tuple22*_tmp8C2;const char*_tmp8C1;const char*
_tmp8C0;struct Cyc_Arg_Set_spec_struct _tmp8BF;struct Cyc_Arg_Set_spec_struct*
_tmp8BE;const char*_tmp8BD;struct _tuple22*_tmp8BC;const char*_tmp8BB;const char*
_tmp8BA;struct Cyc_Arg_Set_spec_struct _tmp8B9;struct Cyc_Arg_Set_spec_struct*
_tmp8B8;const char*_tmp8B7;struct _tuple22*_tmp8B6;const char*_tmp8B5;const char*
_tmp8B4;struct Cyc_Arg_Set_spec_struct _tmp8B3;struct Cyc_Arg_Set_spec_struct*
_tmp8B2;const char*_tmp8B1;struct _tuple22*_tmp8B0;const char*_tmp8AF;const char*
_tmp8AE;struct Cyc_Arg_Clear_spec_struct _tmp8AD;struct Cyc_Arg_Clear_spec_struct*
_tmp8AC;const char*_tmp8AB;struct _tuple22*_tmp8AA;const char*_tmp8A9;const char*
_tmp8A8;struct Cyc_Arg_Unit_spec_struct _tmp8A7;struct Cyc_Arg_Unit_spec_struct*
_tmp8A6;const char*_tmp8A5;struct _tuple22*_tmp8A4;const char*_tmp8A3;const char*
_tmp8A2;struct Cyc_Arg_Set_spec_struct _tmp8A1;struct Cyc_Arg_Set_spec_struct*
_tmp8A0;const char*_tmp89F;struct _tuple22*_tmp89E;const char*_tmp89D;const char*
_tmp89C;struct Cyc_Arg_String_spec_struct _tmp89B;struct Cyc_Arg_String_spec_struct*
_tmp89A;const char*_tmp899;struct _tuple22*_tmp898;const char*_tmp897;const char*
_tmp896;struct Cyc_Arg_String_spec_struct _tmp895;struct Cyc_Arg_String_spec_struct*
_tmp894;const char*_tmp893;struct _tuple22*_tmp892;const char*_tmp891;const char*
_tmp890;struct Cyc_Arg_String_spec_struct _tmp88F;struct Cyc_Arg_String_spec_struct*
_tmp88E;const char*_tmp88D;struct _tuple22*_tmp88C;const char*_tmp88B;const char*
_tmp88A;struct Cyc_Arg_String_spec_struct _tmp889;struct Cyc_Arg_String_spec_struct*
_tmp888;const char*_tmp887;struct _tuple22*_tmp886;const char*_tmp885;const char*
_tmp884;struct Cyc_Arg_String_spec_struct _tmp883;struct Cyc_Arg_String_spec_struct*
_tmp882;const char*_tmp881;struct _tuple22*_tmp880[73];struct Cyc_List_List*options=(
_tmp880[72]=((_tmp886=_region_malloc(r,sizeof(*_tmp886)),((_tmp886->f1=((_tmp885="-idirafter",
_tag_dyneither(_tmp885,sizeof(char),11))),((_tmp886->f2=1,((_tmp886->f3=((
_tmp884="<dir>",_tag_dyneither(_tmp884,sizeof(char),6))),((_tmp886->f4=(void*)((
_tmp882=_region_malloc(r,sizeof(*_tmp882)),((_tmp882[0]=((_tmp883.tag=5,((
_tmp883.f1=Cyc_add_idirafter,_tmp883)))),_tmp882)))),((_tmp886->f5=((_tmp881="Add the directory to the second include path.",
_tag_dyneither(_tmp881,sizeof(char),46))),_tmp886)))))))))))),((_tmp880[71]=((
_tmp88C=_region_malloc(r,sizeof(*_tmp88C)),((_tmp88C->f1=((_tmp88B="-isystem",
_tag_dyneither(_tmp88B,sizeof(char),9))),((_tmp88C->f2=1,((_tmp88C->f3=((_tmp88A="<dir>",
_tag_dyneither(_tmp88A,sizeof(char),6))),((_tmp88C->f4=(void*)((_tmp888=
_region_malloc(r,sizeof(*_tmp888)),((_tmp888[0]=((_tmp889.tag=5,((_tmp889.f1=Cyc_add_isystem,
_tmp889)))),_tmp888)))),((_tmp88C->f5=((_tmp887="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",
_tag_dyneither(_tmp887,sizeof(char),90))),_tmp88C)))))))))))),((_tmp880[70]=((
_tmp892=_region_malloc(r,sizeof(*_tmp892)),((_tmp892->f1=((_tmp891="-iwithprefixbefore",
_tag_dyneither(_tmp891,sizeof(char),19))),((_tmp892->f2=1,((_tmp892->f3=((
_tmp890="<dir>",_tag_dyneither(_tmp890,sizeof(char),6))),((_tmp892->f4=(void*)((
_tmp88E=_region_malloc(r,sizeof(*_tmp88E)),((_tmp88E[0]=((_tmp88F.tag=5,((
_tmp88F.f1=Cyc_add_iwithprefixbefore,_tmp88F)))),_tmp88E)))),((_tmp892->f5=((
_tmp88D="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp88D,
sizeof(char),45))),_tmp892)))))))))))),((_tmp880[69]=((_tmp898=_region_malloc(r,
sizeof(*_tmp898)),((_tmp898->f1=((_tmp897="-iwithprefix",_tag_dyneither(_tmp897,
sizeof(char),13))),((_tmp898->f2=1,((_tmp898->f3=((_tmp896="<dir>",
_tag_dyneither(_tmp896,sizeof(char),6))),((_tmp898->f4=(void*)((_tmp894=
_region_malloc(r,sizeof(*_tmp894)),((_tmp894[0]=((_tmp895.tag=5,((_tmp895.f1=Cyc_add_iwithprefix,
_tmp895)))),_tmp894)))),((_tmp898->f5=((_tmp893="Add <prefix>/<dir> to the second include path.",
_tag_dyneither(_tmp893,sizeof(char),47))),_tmp898)))))))))))),((_tmp880[68]=((
_tmp89E=_region_malloc(r,sizeof(*_tmp89E)),((_tmp89E->f1=((_tmp89D="-iprefix",
_tag_dyneither(_tmp89D,sizeof(char),9))),((_tmp89E->f2=1,((_tmp89E->f3=((_tmp89C="<prefix>",
_tag_dyneither(_tmp89C,sizeof(char),9))),((_tmp89E->f4=(void*)((_tmp89A=
_region_malloc(r,sizeof(*_tmp89A)),((_tmp89A[0]=((_tmp89B.tag=5,((_tmp89B.f1=Cyc_add_iprefix,
_tmp89B)))),_tmp89A)))),((_tmp89E->f5=((_tmp899="Specify <prefix> as the prefix for subsequent -iwithprefix options",
_tag_dyneither(_tmp899,sizeof(char),67))),_tmp89E)))))))))))),((_tmp880[67]=((
_tmp8A4=_region_malloc(r,sizeof(*_tmp8A4)),((_tmp8A4->f1=((_tmp8A3="-noregions",
_tag_dyneither(_tmp8A3,sizeof(char),11))),((_tmp8A4->f2=0,((_tmp8A4->f3=((
_tmp8A2="",_tag_dyneither(_tmp8A2,sizeof(char),1))),((_tmp8A4->f4=(void*)((
_tmp8A0=_region_malloc(r,sizeof(*_tmp8A0)),((_tmp8A0[0]=((_tmp8A1.tag=3,((
_tmp8A1.f1=& Cyc_Absyn_no_regions,_tmp8A1)))),_tmp8A0)))),((_tmp8A4->f5=((_tmp89F="Generate code that doesn't use regions",
_tag_dyneither(_tmp89F,sizeof(char),39))),_tmp8A4)))))))))))),((_tmp880[66]=((
_tmp8AA=_region_malloc(r,sizeof(*_tmp8AA)),((_tmp8AA->f1=((_tmp8A9="-port",
_tag_dyneither(_tmp8A9,sizeof(char),6))),((_tmp8AA->f2=0,((_tmp8AA->f3=((_tmp8A8="",
_tag_dyneither(_tmp8A8,sizeof(char),1))),((_tmp8AA->f4=(void*)((_tmp8A6=
_region_malloc(r,sizeof(*_tmp8A6)),((_tmp8A6[0]=((_tmp8A7.tag=0,((_tmp8A7.f1=Cyc_set_port_c_code,
_tmp8A7)))),_tmp8A6)))),((_tmp8AA->f5=((_tmp8A5="Suggest how to port C code to Cyclone",
_tag_dyneither(_tmp8A5,sizeof(char),38))),_tmp8AA)))))))))))),((_tmp880[65]=((
_tmp8B0=_region_malloc(r,sizeof(*_tmp8B0)),((_tmp8B0->f1=((_tmp8AF="-detailedlocation",
_tag_dyneither(_tmp8AF,sizeof(char),18))),((_tmp8B0->f2=0,((_tmp8B0->f3=((
_tmp8AE="",_tag_dyneither(_tmp8AE,sizeof(char),1))),((_tmp8B0->f4=(void*)((
_tmp8AC=_region_malloc(r,sizeof(*_tmp8AC)),((_tmp8AC[0]=((_tmp8AD.tag=4,((
_tmp8AD.f1=& Cyc_Position_use_gcc_style_location,_tmp8AD)))),_tmp8AC)))),((
_tmp8B0->f5=((_tmp8AB="Try to give more detailed location information for errors",
_tag_dyneither(_tmp8AB,sizeof(char),58))),_tmp8B0)))))))))))),((_tmp880[64]=((
_tmp8B6=_region_malloc(r,sizeof(*_tmp8B6)),((_tmp8B6->f1=((_tmp8B5="-noregister",
_tag_dyneither(_tmp8B5,sizeof(char),12))),((_tmp8B6->f2=0,((_tmp8B6->f3=((
_tmp8B4="",_tag_dyneither(_tmp8B4,sizeof(char),1))),((_tmp8B6->f4=(void*)((
_tmp8B2=_region_malloc(r,sizeof(*_tmp8B2)),((_tmp8B2[0]=((_tmp8B3.tag=3,((
_tmp8B3.f1=& Cyc_Parse_no_register,_tmp8B3)))),_tmp8B2)))),((_tmp8B6->f5=((
_tmp8B1="Treat register storage class as public",_tag_dyneither(_tmp8B1,sizeof(
char),39))),_tmp8B6)))))))))))),((_tmp880[63]=((_tmp8BC=_region_malloc(r,sizeof(*
_tmp8BC)),((_tmp8BC->f1=((_tmp8BB="-warnaliascoercion",_tag_dyneither(_tmp8BB,
sizeof(char),19))),((_tmp8BC->f2=0,((_tmp8BC->f3=((_tmp8BA="",_tag_dyneither(
_tmp8BA,sizeof(char),1))),((_tmp8BC->f4=(void*)((_tmp8B8=_region_malloc(r,
sizeof(*_tmp8B8)),((_tmp8B8[0]=((_tmp8B9.tag=3,((_tmp8B9.f1=& Cyc_Tcutil_warn_alias_coerce,
_tmp8B9)))),_tmp8B8)))),((_tmp8BC->f5=((_tmp8B7="Warn when any alias coercion is inserted",
_tag_dyneither(_tmp8B7,sizeof(char),41))),_tmp8BC)))))))))))),((_tmp880[62]=((
_tmp8C2=_region_malloc(r,sizeof(*_tmp8C2)),((_tmp8C2->f1=((_tmp8C1="-warnnullchecks",
_tag_dyneither(_tmp8C1,sizeof(char),16))),((_tmp8C2->f2=0,((_tmp8C2->f3=((
_tmp8C0="",_tag_dyneither(_tmp8C0,sizeof(char),1))),((_tmp8C2->f4=(void*)((
_tmp8BE=_region_malloc(r,sizeof(*_tmp8BE)),((_tmp8BE[0]=((_tmp8BF.tag=3,((
_tmp8BF.f1=& Cyc_Toc_warn_all_null_deref,_tmp8BF)))),_tmp8BE)))),((_tmp8C2->f5=((
_tmp8BD="Warn when any null check can't be eliminated",_tag_dyneither(_tmp8BD,
sizeof(char),45))),_tmp8C2)))))))))))),((_tmp880[61]=((_tmp8C8=_region_malloc(r,
sizeof(*_tmp8C8)),((_tmp8C8->f1=((_tmp8C7="-warnboundschecks",_tag_dyneither(
_tmp8C7,sizeof(char),18))),((_tmp8C8->f2=0,((_tmp8C8->f3=((_tmp8C6="",
_tag_dyneither(_tmp8C6,sizeof(char),1))),((_tmp8C8->f4=(void*)((_tmp8C4=
_region_malloc(r,sizeof(*_tmp8C4)),((_tmp8C4[0]=((_tmp8C5.tag=3,((_tmp8C5.f1=&
Cyc_Toc_warn_bounds_checks,_tmp8C5)))),_tmp8C4)))),((_tmp8C8->f5=((_tmp8C3="Warn when bounds checks can't be eliminated",
_tag_dyneither(_tmp8C3,sizeof(char),44))),_tmp8C8)))))))))))),((_tmp880[60]=((
_tmp8CE=_region_malloc(r,sizeof(*_tmp8CE)),((_tmp8CE->f1=((_tmp8CD="-CI",
_tag_dyneither(_tmp8CD,sizeof(char),4))),((_tmp8CE->f2=0,((_tmp8CE->f3=((_tmp8CC=" <file>",
_tag_dyneither(_tmp8CC,sizeof(char),8))),((_tmp8CE->f4=(void*)((_tmp8CA=
_region_malloc(r,sizeof(*_tmp8CA)),((_tmp8CA[0]=((_tmp8CB.tag=5,((_tmp8CB.f1=Cyc_set_cyc_include,
_tmp8CB)))),_tmp8CA)))),((_tmp8CE->f5=((_tmp8C9="Set cyc_include.h to be <file>",
_tag_dyneither(_tmp8C9,sizeof(char),31))),_tmp8CE)))))))))))),((_tmp880[59]=((
_tmp8D4=_region_malloc(r,sizeof(*_tmp8D4)),((_tmp8D4->f1=((_tmp8D3="-compile-for-boot",
_tag_dyneither(_tmp8D3,sizeof(char),18))),((_tmp8D4->f2=0,((_tmp8D4->f3=((
_tmp8D2="",_tag_dyneither(_tmp8D2,sizeof(char),1))),((_tmp8D4->f4=(void*)((
_tmp8D0=_region_malloc(r,sizeof(*_tmp8D0)),((_tmp8D0[0]=((_tmp8D1.tag=3,((
_tmp8D1.f1=& Cyc_compile_for_boot_r,_tmp8D1)))),_tmp8D0)))),((_tmp8D4->f5=((
_tmp8CF="Compile using the special boot library instead of the standard library",
_tag_dyneither(_tmp8CF,sizeof(char),71))),_tmp8D4)))))))))))),((_tmp880[58]=((
_tmp8DA=_region_malloc(r,sizeof(*_tmp8DA)),((_tmp8DA->f1=((_tmp8D9="-nocyc_setjmp",
_tag_dyneither(_tmp8D9,sizeof(char),14))),((_tmp8DA->f2=0,((_tmp8DA->f3=((
_tmp8D8="",_tag_dyneither(_tmp8D8,sizeof(char),1))),((_tmp8DA->f4=(void*)((
_tmp8D6=_region_malloc(r,sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D7.tag=3,((
_tmp8D7.f1=& Cyc_nocyc_setjmp_r,_tmp8D7)))),_tmp8D6)))),((_tmp8DA->f5=((_tmp8D5="Do not use compiler special file cyc_setjmp.h",
_tag_dyneither(_tmp8D5,sizeof(char),46))),_tmp8DA)))))))))))),((_tmp880[57]=((
_tmp8E0=_region_malloc(r,sizeof(*_tmp8E0)),((_tmp8E0->f1=((_tmp8DF="-printalleffects",
_tag_dyneither(_tmp8DF,sizeof(char),17))),((_tmp8E0->f2=0,((_tmp8E0->f3=((
_tmp8DE="",_tag_dyneither(_tmp8DE,sizeof(char),1))),((_tmp8E0->f4=(void*)((
_tmp8DC=_region_malloc(r,sizeof(*_tmp8DC)),((_tmp8DC[0]=((_tmp8DD.tag=3,((
_tmp8DD.f1=& Cyc_print_all_effects_r,_tmp8DD)))),_tmp8DC)))),((_tmp8E0->f5=((
_tmp8DB="Print effects for functions (type debugging)",_tag_dyneither(_tmp8DB,
sizeof(char),45))),_tmp8E0)))))))))))),((_tmp880[56]=((_tmp8E6=_region_malloc(r,
sizeof(*_tmp8E6)),((_tmp8E6->f1=((_tmp8E5="-printfullevars",_tag_dyneither(
_tmp8E5,sizeof(char),16))),((_tmp8E6->f2=0,((_tmp8E6->f3=((_tmp8E4="",
_tag_dyneither(_tmp8E4,sizeof(char),1))),((_tmp8E6->f4=(void*)((_tmp8E2=
_region_malloc(r,sizeof(*_tmp8E2)),((_tmp8E2[0]=((_tmp8E3.tag=3,((_tmp8E3.f1=&
Cyc_print_full_evars_r,_tmp8E3)))),_tmp8E2)))),((_tmp8E6->f5=((_tmp8E1="Print full information for evars (type debugging)",
_tag_dyneither(_tmp8E1,sizeof(char),50))),_tmp8E6)))))))))))),((_tmp880[55]=((
_tmp8EC=_region_malloc(r,sizeof(*_tmp8EC)),((_tmp8EC->f1=((_tmp8EB="-printallkinds",
_tag_dyneither(_tmp8EB,sizeof(char),15))),((_tmp8EC->f2=0,((_tmp8EC->f3=((
_tmp8EA="",_tag_dyneither(_tmp8EA,sizeof(char),1))),((_tmp8EC->f4=(void*)((
_tmp8E8=_region_malloc(r,sizeof(*_tmp8E8)),((_tmp8E8[0]=((_tmp8E9.tag=3,((
_tmp8E9.f1=& Cyc_print_all_kinds_r,_tmp8E9)))),_tmp8E8)))),((_tmp8EC->f5=((
_tmp8E7="Always print kinds of type variables",_tag_dyneither(_tmp8E7,sizeof(
char),37))),_tmp8EC)))))))))))),((_tmp880[54]=((_tmp8F2=_region_malloc(r,sizeof(*
_tmp8F2)),((_tmp8F2->f1=((_tmp8F1="-printalltvars",_tag_dyneither(_tmp8F1,
sizeof(char),15))),((_tmp8F2->f2=0,((_tmp8F2->f3=((_tmp8F0="",_tag_dyneither(
_tmp8F0,sizeof(char),1))),((_tmp8F2->f4=(void*)((_tmp8EE=_region_malloc(r,
sizeof(*_tmp8EE)),((_tmp8EE[0]=((_tmp8EF.tag=3,((_tmp8EF.f1=& Cyc_print_all_tvars_r,
_tmp8EF)))),_tmp8EE)))),((_tmp8F2->f5=((_tmp8ED="Print all type variables (even implicit default effects)",
_tag_dyneither(_tmp8ED,sizeof(char),57))),_tmp8F2)))))))))))),((_tmp880[53]=((
_tmp8F8=_region_malloc(r,sizeof(*_tmp8F8)),((_tmp8F8->f1=((_tmp8F7="-noexpandtypedefs",
_tag_dyneither(_tmp8F7,sizeof(char),18))),((_tmp8F8->f2=0,((_tmp8F8->f3=((
_tmp8F6="",_tag_dyneither(_tmp8F6,sizeof(char),1))),((_tmp8F8->f4=(void*)((
_tmp8F4=_region_malloc(r,sizeof(*_tmp8F4)),((_tmp8F4[0]=((_tmp8F5.tag=3,((
_tmp8F5.f1=& Cyc_noexpand_r,_tmp8F5)))),_tmp8F4)))),((_tmp8F8->f5=((_tmp8F3="Don't expand typedefs in pretty printing",
_tag_dyneither(_tmp8F3,sizeof(char),41))),_tmp8F8)))))))))))),((_tmp880[52]=((
_tmp8FE=_region_malloc(r,sizeof(*_tmp8FE)),((_tmp8FE->f1=((_tmp8FD="-noremoveunused",
_tag_dyneither(_tmp8FD,sizeof(char),16))),((_tmp8FE->f2=0,((_tmp8FE->f3=((
_tmp8FC="",_tag_dyneither(_tmp8FC,sizeof(char),1))),((_tmp8FE->f4=(void*)((
_tmp8FA=_region_malloc(r,sizeof(*_tmp8FA)),((_tmp8FA[0]=((_tmp8FB.tag=3,((
_tmp8FB.f1=& Cyc_noshake_r,_tmp8FB)))),_tmp8FA)))),((_tmp8FE->f5=((_tmp8F9="Don't remove externed variables that aren't used",
_tag_dyneither(_tmp8F9,sizeof(char),49))),_tmp8FE)))))))))))),((_tmp880[51]=((
_tmp904=_region_malloc(r,sizeof(*_tmp904)),((_tmp904->f1=((_tmp903="-nogc",
_tag_dyneither(_tmp903,sizeof(char),6))),((_tmp904->f2=0,((_tmp904->f3=((_tmp902="",
_tag_dyneither(_tmp902,sizeof(char),1))),((_tmp904->f4=(void*)((_tmp900=
_region_malloc(r,sizeof(*_tmp900)),((_tmp900[0]=((_tmp901.tag=3,((_tmp901.f1=&
Cyc_nogc_r,_tmp901)))),_tmp900)))),((_tmp904->f5=((_tmp8FF="Don't link in the garbage collector",
_tag_dyneither(_tmp8FF,sizeof(char),36))),_tmp904)))))))))))),((_tmp880[50]=((
_tmp90A=_region_malloc(r,sizeof(*_tmp90A)),((_tmp90A->f1=((_tmp909="-nocyc",
_tag_dyneither(_tmp909,sizeof(char),7))),((_tmp90A->f2=0,((_tmp90A->f3=((_tmp908="",
_tag_dyneither(_tmp908,sizeof(char),1))),((_tmp90A->f4=(void*)((_tmp906=
_region_malloc(r,sizeof(*_tmp906)),((_tmp906[0]=((_tmp907.tag=0,((_tmp907.f1=Cyc_set_nocyc,
_tmp907)))),_tmp906)))),((_tmp90A->f5=((_tmp905="Don't add implicit namespace Cyc",
_tag_dyneither(_tmp905,sizeof(char),33))),_tmp90A)))))))))))),((_tmp880[49]=((
_tmp910=_region_malloc(r,sizeof(*_tmp910)),((_tmp910->f1=((_tmp90F="-no-cpp-precomp",
_tag_dyneither(_tmp90F,sizeof(char),16))),((_tmp910->f2=0,((_tmp910->f3=((
_tmp90E="",_tag_dyneither(_tmp90E,sizeof(char),1))),((_tmp910->f4=(void*)((
_tmp90C=_region_malloc(r,sizeof(*_tmp90C)),((_tmp90C[0]=((_tmp90D.tag=0,((
_tmp90D.f1=Cyc_set_nocppprecomp,_tmp90D)))),_tmp90C)))),((_tmp910->f5=((_tmp90B="Don't do smart preprocessing (mac only)",
_tag_dyneither(_tmp90B,sizeof(char),40))),_tmp910)))))))))))),((_tmp880[48]=((
_tmp916=_region_malloc(r,sizeof(*_tmp916)),((_tmp916->f1=((_tmp915="-use-cpp",
_tag_dyneither(_tmp915,sizeof(char),9))),((_tmp916->f2=0,((_tmp916->f3=((_tmp914="<path>",
_tag_dyneither(_tmp914,sizeof(char),7))),((_tmp916->f4=(void*)((_tmp912=
_region_malloc(r,sizeof(*_tmp912)),((_tmp912[0]=((_tmp913.tag=5,((_tmp913.f1=Cyc_set_cpp,
_tmp913)))),_tmp912)))),((_tmp916->f5=((_tmp911="Indicate which preprocessor to use",
_tag_dyneither(_tmp911,sizeof(char),35))),_tmp916)))))))))))),((_tmp880[47]=((
_tmp91C=_region_malloc(r,sizeof(*_tmp91C)),((_tmp91C->f1=((_tmp91B="--inline-checks",
_tag_dyneither(_tmp91B,sizeof(char),16))),((_tmp91C->f2=0,((_tmp91C->f3=((
_tmp91A="",_tag_dyneither(_tmp91A,sizeof(char),1))),((_tmp91C->f4=(void*)((
_tmp918=_region_malloc(r,sizeof(*_tmp918)),((_tmp918[0]=((_tmp919.tag=0,((
_tmp919.f1=Cyc_set_inline_functions,_tmp919)))),_tmp918)))),((_tmp91C->f5=((
_tmp917="Inline bounds checks instead of #define",_tag_dyneither(_tmp917,sizeof(
char),40))),_tmp91C)))))))))))),((_tmp880[46]=((_tmp922=_region_malloc(r,sizeof(*
_tmp922)),((_tmp922->f1=((_tmp921="--noboundschecks",_tag_dyneither(_tmp921,
sizeof(char),17))),((_tmp922->f2=0,((_tmp922->f3=((_tmp920="",_tag_dyneither(
_tmp920,sizeof(char),1))),((_tmp922->f4=(void*)((_tmp91E=_region_malloc(r,
sizeof(*_tmp91E)),((_tmp91E[0]=((_tmp91F.tag=0,((_tmp91F.f1=Cyc_set_noboundschecks,
_tmp91F)))),_tmp91E)))),((_tmp922->f5=((_tmp91D="Disable bounds checks",
_tag_dyneither(_tmp91D,sizeof(char),22))),_tmp922)))))))))))),((_tmp880[45]=((
_tmp928=_region_malloc(r,sizeof(*_tmp928)),((_tmp928->f1=((_tmp927="--nonullchecks",
_tag_dyneither(_tmp927,sizeof(char),15))),((_tmp928->f2=0,((_tmp928->f3=((
_tmp926="",_tag_dyneither(_tmp926,sizeof(char),1))),((_tmp928->f4=(void*)((
_tmp924=_region_malloc(r,sizeof(*_tmp924)),((_tmp924[0]=((_tmp925.tag=0,((
_tmp925.f1=Cyc_set_nonullchecks,_tmp925)))),_tmp924)))),((_tmp928->f5=((_tmp923="Disable null checks",
_tag_dyneither(_tmp923,sizeof(char),20))),_tmp928)))))))))))),((_tmp880[44]=((
_tmp92E=_region_malloc(r,sizeof(*_tmp92E)),((_tmp92E->f1=((_tmp92D="--nochecks",
_tag_dyneither(_tmp92D,sizeof(char),11))),((_tmp92E->f2=0,((_tmp92E->f3=((
_tmp92C="",_tag_dyneither(_tmp92C,sizeof(char),1))),((_tmp92E->f4=(void*)((
_tmp92A=_region_malloc(r,sizeof(*_tmp92A)),((_tmp92A[0]=((_tmp92B.tag=0,((
_tmp92B.f1=Cyc_set_nochecks,_tmp92B)))),_tmp92A)))),((_tmp92E->f5=((_tmp929="Disable bounds/null checks",
_tag_dyneither(_tmp929,sizeof(char),27))),_tmp92E)))))))))))),((_tmp880[43]=((
_tmp934=_region_malloc(r,sizeof(*_tmp934)),((_tmp934->f1=((_tmp933="--lineno",
_tag_dyneither(_tmp933,sizeof(char),9))),((_tmp934->f2=0,((_tmp934->f3=((_tmp932="",
_tag_dyneither(_tmp932,sizeof(char),1))),((_tmp934->f4=(void*)((_tmp930=
_region_malloc(r,sizeof(*_tmp930)),((_tmp930[0]=((_tmp931.tag=0,((_tmp931.f1=Cyc_set_lineno,
_tmp931)))),_tmp930)))),((_tmp934->f5=((_tmp92F="Generate line numbers for debugging",
_tag_dyneither(_tmp92F,sizeof(char),36))),_tmp934)))))))))))),((_tmp880[42]=((
_tmp93A=_region_malloc(r,sizeof(*_tmp93A)),((_tmp93A->f1=((_tmp939="-save-c",
_tag_dyneither(_tmp939,sizeof(char),8))),((_tmp93A->f2=0,((_tmp93A->f3=((_tmp938="",
_tag_dyneither(_tmp938,sizeof(char),1))),((_tmp93A->f4=(void*)((_tmp936=
_region_malloc(r,sizeof(*_tmp936)),((_tmp936[0]=((_tmp937.tag=3,((_tmp937.f1=&
Cyc_save_c_r,_tmp937)))),_tmp936)))),((_tmp93A->f5=((_tmp935="Don't delete temporary C files",
_tag_dyneither(_tmp935,sizeof(char),31))),_tmp93A)))))))))))),((_tmp880[41]=((
_tmp940=_region_malloc(r,sizeof(*_tmp940)),((_tmp940->f1=((_tmp93F="-save-temps",
_tag_dyneither(_tmp93F,sizeof(char),12))),((_tmp940->f2=0,((_tmp940->f3=((
_tmp93E="",_tag_dyneither(_tmp93E,sizeof(char),1))),((_tmp940->f4=(void*)((
_tmp93C=_region_malloc(r,sizeof(*_tmp93C)),((_tmp93C[0]=((_tmp93D.tag=0,((
_tmp93D.f1=Cyc_set_save_temps,_tmp93D)))),_tmp93C)))),((_tmp940->f5=((_tmp93B="Don't delete temporary files",
_tag_dyneither(_tmp93B,sizeof(char),29))),_tmp940)))))))))))),((_tmp880[40]=((
_tmp946=_region_malloc(r,sizeof(*_tmp946)),((_tmp946->f1=((_tmp945="-c-comp",
_tag_dyneither(_tmp945,sizeof(char),8))),((_tmp946->f2=0,((_tmp946->f3=((_tmp944=" <compiler>",
_tag_dyneither(_tmp944,sizeof(char),12))),((_tmp946->f4=(void*)((_tmp942=
_region_malloc(r,sizeof(*_tmp942)),((_tmp942[0]=((_tmp943.tag=5,((_tmp943.f1=Cyc_set_c_compiler,
_tmp943)))),_tmp942)))),((_tmp946->f5=((_tmp941="Produce C output for the given compiler",
_tag_dyneither(_tmp941,sizeof(char),40))),_tmp946)))))))))))),((_tmp880[39]=((
_tmp94C=_region_malloc(r,sizeof(*_tmp94C)),((_tmp94C->f1=((_tmp94B="-un-gcc",
_tag_dyneither(_tmp94B,sizeof(char),8))),((_tmp94C->f2=0,((_tmp94C->f3=((_tmp94A="",
_tag_dyneither(_tmp94A,sizeof(char),1))),((_tmp94C->f4=(void*)((_tmp948=
_region_malloc(r,sizeof(*_tmp948)),((_tmp948[0]=((_tmp949.tag=0,((_tmp949.f1=Cyc_set_tovc,
_tmp949)))),_tmp948)))),((_tmp94C->f5=((_tmp947="Avoid gcc extensions in C output (except for attributes)",
_tag_dyneither(_tmp947,sizeof(char),57))),_tmp94C)))))))))))),((_tmp880[38]=((
_tmp952=_region_malloc(r,sizeof(*_tmp952)),((_tmp952->f1=((_tmp951="-no-elim-statement-expressions",
_tag_dyneither(_tmp951,sizeof(char),31))),((_tmp952->f2=0,((_tmp952->f3=((
_tmp950="",_tag_dyneither(_tmp950,sizeof(char),1))),((_tmp952->f4=(void*)((
_tmp94E=_region_malloc(r,sizeof(*_tmp94E)),((_tmp94E[0]=((_tmp94F.tag=4,((
_tmp94F.f1=& Cyc_elim_se_r,_tmp94F)))),_tmp94E)))),((_tmp952->f5=((_tmp94D="Do not avoid statement expressions in C output",
_tag_dyneither(_tmp94D,sizeof(char),47))),_tmp952)))))))))))),((_tmp880[37]=((
_tmp958=_region_malloc(r,sizeof(*_tmp958)),((_tmp958->f1=((_tmp957="-elim-statement-expressions",
_tag_dyneither(_tmp957,sizeof(char),28))),((_tmp958->f2=0,((_tmp958->f3=((
_tmp956="",_tag_dyneither(_tmp956,sizeof(char),1))),((_tmp958->f4=(void*)((
_tmp954=_region_malloc(r,sizeof(*_tmp954)),((_tmp954[0]=((_tmp955.tag=3,((
_tmp955.f1=& Cyc_elim_se_r,_tmp955)))),_tmp954)))),((_tmp958->f5=((_tmp953="Avoid statement expressions in C output (implies --inline-checks)",
_tag_dyneither(_tmp953,sizeof(char),66))),_tmp958)))))))))))),((_tmp880[36]=((
_tmp95E=_region_malloc(r,sizeof(*_tmp95E)),((_tmp95E->f1=((_tmp95D="-up",
_tag_dyneither(_tmp95D,sizeof(char),4))),((_tmp95E->f2=0,((_tmp95E->f3=((_tmp95C="",
_tag_dyneither(_tmp95C,sizeof(char),1))),((_tmp95E->f4=(void*)((_tmp95A=
_region_malloc(r,sizeof(*_tmp95A)),((_tmp95A[0]=((_tmp95B.tag=4,((_tmp95B.f1=&
Cyc_pp_r,_tmp95B)))),_tmp95A)))),((_tmp95E->f5=((_tmp959="Ugly print the C code that Cyclone generates (default)",
_tag_dyneither(_tmp959,sizeof(char),55))),_tmp95E)))))))))))),((_tmp880[35]=((
_tmp964=_region_malloc(r,sizeof(*_tmp964)),((_tmp964->f1=((_tmp963="-pp",
_tag_dyneither(_tmp963,sizeof(char),4))),((_tmp964->f2=0,((_tmp964->f3=((_tmp962="",
_tag_dyneither(_tmp962,sizeof(char),1))),((_tmp964->f4=(void*)((_tmp960=
_region_malloc(r,sizeof(*_tmp960)),((_tmp960[0]=((_tmp961.tag=3,((_tmp961.f1=&
Cyc_pp_r,_tmp961)))),_tmp960)))),((_tmp964->f5=((_tmp95F="Pretty print the C code that Cyclone generates",
_tag_dyneither(_tmp95F,sizeof(char),47))),_tmp964)))))))))))),((_tmp880[34]=((
_tmp96A=_region_malloc(r,sizeof(*_tmp96A)),((_tmp96A->f1=((_tmp969="-ic",
_tag_dyneither(_tmp969,sizeof(char),4))),((_tmp96A->f2=0,((_tmp96A->f3=((_tmp968="",
_tag_dyneither(_tmp968,sizeof(char),1))),((_tmp96A->f4=(void*)((_tmp966=
_region_malloc(r,sizeof(*_tmp966)),((_tmp966[0]=((_tmp967.tag=3,((_tmp967.f1=&
Cyc_ic_r,_tmp967)))),_tmp966)))),((_tmp96A->f5=((_tmp965="Activate the link-checker",
_tag_dyneither(_tmp965,sizeof(char),26))),_tmp96A)))))))))))),((_tmp880[33]=((
_tmp970=_region_malloc(r,sizeof(*_tmp970)),((_tmp970->f1=((_tmp96F="-stopafter-toc",
_tag_dyneither(_tmp96F,sizeof(char),15))),((_tmp970->f2=0,((_tmp970->f3=((
_tmp96E="",_tag_dyneither(_tmp96E,sizeof(char),1))),((_tmp970->f4=(void*)((
_tmp96C=_region_malloc(r,sizeof(*_tmp96C)),((_tmp96C[0]=((_tmp96D.tag=3,((
_tmp96D.f1=& Cyc_toc_r,_tmp96D)))),_tmp96C)))),((_tmp970->f5=((_tmp96B="Stop after translation to C",
_tag_dyneither(_tmp96B,sizeof(char),28))),_tmp970)))))))))))),((_tmp880[32]=((
_tmp976=_region_malloc(r,sizeof(*_tmp976)),((_tmp976->f1=((_tmp975="-stopafter-cf",
_tag_dyneither(_tmp975,sizeof(char),14))),((_tmp976->f2=0,((_tmp976->f3=((
_tmp974="",_tag_dyneither(_tmp974,sizeof(char),1))),((_tmp976->f4=(void*)((
_tmp972=_region_malloc(r,sizeof(*_tmp972)),((_tmp972[0]=((_tmp973.tag=3,((
_tmp973.f1=& Cyc_cf_r,_tmp973)))),_tmp972)))),((_tmp976->f5=((_tmp971="Stop after control-flow checking",
_tag_dyneither(_tmp971,sizeof(char),33))),_tmp976)))))))))))),((_tmp880[31]=((
_tmp97C=_region_malloc(r,sizeof(*_tmp97C)),((_tmp97C->f1=((_tmp97B="-noprint",
_tag_dyneither(_tmp97B,sizeof(char),9))),((_tmp97C->f2=0,((_tmp97C->f3=((_tmp97A="",
_tag_dyneither(_tmp97A,sizeof(char),1))),((_tmp97C->f4=(void*)((_tmp978=
_region_malloc(r,sizeof(*_tmp978)),((_tmp978[0]=((_tmp979.tag=3,((_tmp979.f1=&
Cyc_noprint_r,_tmp979)))),_tmp978)))),((_tmp97C->f5=((_tmp977="Do not print output (when stopping early)",
_tag_dyneither(_tmp977,sizeof(char),42))),_tmp97C)))))))))))),((_tmp880[30]=((
_tmp982=_region_malloc(r,sizeof(*_tmp982)),((_tmp982->f1=((_tmp981="-stopafter-tc",
_tag_dyneither(_tmp981,sizeof(char),14))),((_tmp982->f2=0,((_tmp982->f3=((
_tmp980="",_tag_dyneither(_tmp980,sizeof(char),1))),((_tmp982->f4=(void*)((
_tmp97E=_region_malloc(r,sizeof(*_tmp97E)),((_tmp97E[0]=((_tmp97F.tag=3,((
_tmp97F.f1=& Cyc_tc_r,_tmp97F)))),_tmp97E)))),((_tmp982->f5=((_tmp97D="Stop after type checking",
_tag_dyneither(_tmp97D,sizeof(char),25))),_tmp982)))))))))))),((_tmp880[29]=((
_tmp988=_region_malloc(r,sizeof(*_tmp988)),((_tmp988->f1=((_tmp987="-stopafter-parse",
_tag_dyneither(_tmp987,sizeof(char),17))),((_tmp988->f2=0,((_tmp988->f3=((
_tmp986="",_tag_dyneither(_tmp986,sizeof(char),1))),((_tmp988->f4=(void*)((
_tmp984=_region_malloc(r,sizeof(*_tmp984)),((_tmp984[0]=((_tmp985.tag=3,((
_tmp985.f1=& Cyc_parseonly_r,_tmp985)))),_tmp984)))),((_tmp988->f5=((_tmp983="Stop after parsing",
_tag_dyneither(_tmp983,sizeof(char),19))),_tmp988)))))))))))),((_tmp880[28]=((
_tmp98E=_region_malloc(r,sizeof(*_tmp98E)),((_tmp98E->f1=((_tmp98D="-E",
_tag_dyneither(_tmp98D,sizeof(char),3))),((_tmp98E->f2=0,((_tmp98E->f3=((_tmp98C="",
_tag_dyneither(_tmp98C,sizeof(char),1))),((_tmp98E->f4=(void*)((_tmp98A=
_region_malloc(r,sizeof(*_tmp98A)),((_tmp98A[0]=((_tmp98B.tag=3,((_tmp98B.f1=&
Cyc_stop_after_cpp_r,_tmp98B)))),_tmp98A)))),((_tmp98E->f5=((_tmp989="Stop after preprocessing",
_tag_dyneither(_tmp989,sizeof(char),25))),_tmp98E)))))))))))),((_tmp880[27]=((
_tmp994=_region_malloc(r,sizeof(*_tmp994)),((_tmp994->f1=((_tmp993="-Wall",
_tag_dyneither(_tmp993,sizeof(char),6))),((_tmp994->f2=0,((_tmp994->f3=((_tmp992="",
_tag_dyneither(_tmp992,sizeof(char),1))),((_tmp994->f4=(void*)((_tmp990=
_region_malloc(r,sizeof(*_tmp990)),((_tmp990[0]=((_tmp991.tag=0,((_tmp991.f1=Cyc_set_all_warnings,
_tmp991)))),_tmp990)))),((_tmp994->f5=((_tmp98F="Turn on all warnings",
_tag_dyneither(_tmp98F,sizeof(char),21))),_tmp994)))))))))))),((_tmp880[26]=((
_tmp99A=_region_malloc(r,sizeof(*_tmp99A)),((_tmp99A->f1=((_tmp999="-Wlose-unique",
_tag_dyneither(_tmp999,sizeof(char),14))),((_tmp99A->f2=0,((_tmp99A->f3=((
_tmp998="",_tag_dyneither(_tmp998,sizeof(char),1))),((_tmp99A->f4=(void*)((
_tmp996=_region_malloc(r,sizeof(*_tmp996)),((_tmp996[0]=((_tmp997.tag=3,((
_tmp997.f1=& Cyc_NewControlFlow_warn_lose_unique,_tmp997)))),_tmp996)))),((
_tmp99A->f5=((_tmp995="Try to warn when a unique pointer might get lost",
_tag_dyneither(_tmp995,sizeof(char),49))),_tmp99A)))))))))))),((_tmp880[25]=((
_tmp9A0=_region_malloc(r,sizeof(*_tmp9A0)),((_tmp9A0->f1=((_tmp99F="-b",
_tag_dyneither(_tmp99F,sizeof(char),3))),((_tmp9A0->f2=0,((_tmp9A0->f3=((_tmp99E="<arch>",
_tag_dyneither(_tmp99E,sizeof(char),7))),((_tmp9A0->f4=(void*)((_tmp99C=
_region_malloc(r,sizeof(*_tmp99C)),((_tmp99C[0]=((_tmp99D.tag=5,((_tmp99D.f1=Cyc_set_target_arch,
_tmp99D)))),_tmp99C)))),((_tmp9A0->f5=((_tmp99B="Set target architecture; implies -stopafter-toc",
_tag_dyneither(_tmp99B,sizeof(char),48))),_tmp9A0)))))))))))),((_tmp880[24]=((
_tmp9A6=_region_malloc(r,sizeof(*_tmp9A6)),((_tmp9A6->f1=((_tmp9A5="-MT",
_tag_dyneither(_tmp9A5,sizeof(char),4))),((_tmp9A6->f2=0,((_tmp9A6->f3=((_tmp9A4=" <target>",
_tag_dyneither(_tmp9A4,sizeof(char),10))),((_tmp9A6->f4=(void*)((_tmp9A2=
_region_malloc(r,sizeof(*_tmp9A2)),((_tmp9A2[0]=((_tmp9A3.tag=5,((_tmp9A3.f1=Cyc_set_dependencies_target,
_tmp9A3)))),_tmp9A2)))),((_tmp9A6->f5=((_tmp9A1="Give target for dependencies",
_tag_dyneither(_tmp9A1,sizeof(char),29))),_tmp9A6)))))))))))),((_tmp880[23]=((
_tmp9AC=_region_malloc(r,sizeof(*_tmp9AC)),((_tmp9AC->f1=((_tmp9AB="-MG",
_tag_dyneither(_tmp9AB,sizeof(char),4))),((_tmp9AC->f2=0,((_tmp9AC->f3=((_tmp9AA="",
_tag_dyneither(_tmp9AA,sizeof(char),1))),((_tmp9AC->f4=(void*)((_tmp9A8=
_region_malloc(r,sizeof(*_tmp9A8)),((_tmp9A8[0]=((_tmp9A9.tag=1,((_tmp9A9.f1=Cyc_add_cpparg,
_tmp9A9)))),_tmp9A8)))),((_tmp9AC->f5=((_tmp9A7="When producing dependencies assume that missing files are generated",
_tag_dyneither(_tmp9A7,sizeof(char),68))),_tmp9AC)))))))))))),((_tmp880[22]=((
_tmp9B2=_region_malloc(r,sizeof(*_tmp9B2)),((_tmp9B2->f1=((_tmp9B1="-M",
_tag_dyneither(_tmp9B1,sizeof(char),3))),((_tmp9B2->f2=0,((_tmp9B2->f3=((_tmp9B0="",
_tag_dyneither(_tmp9B0,sizeof(char),1))),((_tmp9B2->f4=(void*)((_tmp9AE=
_region_malloc(r,sizeof(*_tmp9AE)),((_tmp9AE[0]=((_tmp9AF.tag=0,((_tmp9AF.f1=Cyc_set_produce_dependencies,
_tmp9AF)))),_tmp9AE)))),((_tmp9B2->f5=((_tmp9AD="Produce dependencies",
_tag_dyneither(_tmp9AD,sizeof(char),21))),_tmp9B2)))))))))))),((_tmp880[21]=((
_tmp9B8=_region_malloc(r,sizeof(*_tmp9B8)),((_tmp9B8->f1=((_tmp9B7="-S",
_tag_dyneither(_tmp9B7,sizeof(char),3))),((_tmp9B8->f2=0,((_tmp9B8->f3=((_tmp9B6="",
_tag_dyneither(_tmp9B6,sizeof(char),1))),((_tmp9B8->f4=(void*)((_tmp9B4=
_region_malloc(r,sizeof(*_tmp9B4)),((_tmp9B4[0]=((_tmp9B5.tag=0,((_tmp9B5.f1=Cyc_set_stop_after_asmfile,
_tmp9B5)))),_tmp9B4)))),((_tmp9B8->f5=((_tmp9B3="Stop after producing assembly code",
_tag_dyneither(_tmp9B3,sizeof(char),35))),_tmp9B8)))))))))))),((_tmp880[20]=((
_tmp9BE=_region_malloc(r,sizeof(*_tmp9BE)),((_tmp9BE->f1=((_tmp9BD="-pa",
_tag_dyneither(_tmp9BD,sizeof(char),4))),((_tmp9BE->f2=0,((_tmp9BE->f3=((_tmp9BC="",
_tag_dyneither(_tmp9BC,sizeof(char),1))),((_tmp9BE->f4=(void*)((_tmp9BA=
_region_malloc(r,sizeof(*_tmp9BA)),((_tmp9BA[0]=((_tmp9BB.tag=0,((_tmp9BB.f1=Cyc_set_pa,
_tmp9BB)))),_tmp9BA)))),((_tmp9BE->f5=((_tmp9B9="Compile for profiling with aprof",
_tag_dyneither(_tmp9B9,sizeof(char),33))),_tmp9BE)))))))))))),((_tmp880[19]=((
_tmp9C4=_region_malloc(r,sizeof(*_tmp9C4)),((_tmp9C4->f1=((_tmp9C3="-pg",
_tag_dyneither(_tmp9C3,sizeof(char),4))),((_tmp9C4->f2=0,((_tmp9C4->f3=((_tmp9C2="",
_tag_dyneither(_tmp9C2,sizeof(char),1))),((_tmp9C4->f4=(void*)((_tmp9C0=
_region_malloc(r,sizeof(*_tmp9C0)),((_tmp9C0[0]=((_tmp9C1.tag=0,((_tmp9C1.f1=Cyc_set_pg,
_tmp9C1)))),_tmp9C0)))),((_tmp9C4->f5=((_tmp9BF="Compile for profiling with gprof",
_tag_dyneither(_tmp9BF,sizeof(char),33))),_tmp9C4)))))))))))),((_tmp880[18]=((
_tmp9CA=_region_malloc(r,sizeof(*_tmp9CA)),((_tmp9CA->f1=((_tmp9C9="-p",
_tag_dyneither(_tmp9C9,sizeof(char),3))),((_tmp9CA->f2=0,((_tmp9CA->f3=((_tmp9C8="",
_tag_dyneither(_tmp9C8,sizeof(char),1))),((_tmp9CA->f4=(void*)((_tmp9C6=
_region_malloc(r,sizeof(*_tmp9C6)),((_tmp9C6[0]=((_tmp9C7.tag=1,((_tmp9C7.f1=Cyc_add_ccarg,
_tmp9C7)))),_tmp9C6)))),((_tmp9CA->f5=((_tmp9C5="Compile for profiling with prof",
_tag_dyneither(_tmp9C5,sizeof(char),32))),_tmp9CA)))))))))))),((_tmp880[17]=((
_tmp9D0=_region_malloc(r,sizeof(*_tmp9D0)),((_tmp9D0->f1=((_tmp9CF="-g",
_tag_dyneither(_tmp9CF,sizeof(char),3))),((_tmp9D0->f2=0,((_tmp9D0->f3=((_tmp9CE="",
_tag_dyneither(_tmp9CE,sizeof(char),1))),((_tmp9D0->f4=(void*)((_tmp9CC=
_region_malloc(r,sizeof(*_tmp9CC)),((_tmp9CC[0]=((_tmp9CD.tag=1,((_tmp9CD.f1=Cyc_add_ccarg,
_tmp9CD)))),_tmp9CC)))),((_tmp9D0->f5=((_tmp9CB="Compile for debugging",
_tag_dyneither(_tmp9CB,sizeof(char),22))),_tmp9D0)))))))))))),((_tmp880[16]=((
_tmp9D6=_region_malloc(r,sizeof(*_tmp9D6)),((_tmp9D6->f1=((_tmp9D5="-fomit-frame-pointer",
_tag_dyneither(_tmp9D5,sizeof(char),21))),((_tmp9D6->f2=0,((_tmp9D6->f3=((
_tmp9D4="",_tag_dyneither(_tmp9D4,sizeof(char),1))),((_tmp9D6->f4=(void*)((
_tmp9D2=_region_malloc(r,sizeof(*_tmp9D2)),((_tmp9D2[0]=((_tmp9D3.tag=1,((
_tmp9D3.f1=Cyc_add_ccarg,_tmp9D3)))),_tmp9D2)))),((_tmp9D6->f5=((_tmp9D1="Omit frame pointer",
_tag_dyneither(_tmp9D1,sizeof(char),19))),_tmp9D6)))))))))))),((_tmp880[15]=((
_tmp9DC=_region_malloc(r,sizeof(*_tmp9DC)),((_tmp9DC->f1=((_tmp9DB="-O3",
_tag_dyneither(_tmp9DB,sizeof(char),4))),((_tmp9DC->f2=0,((_tmp9DC->f3=((_tmp9DA="",
_tag_dyneither(_tmp9DA,sizeof(char),1))),((_tmp9DC->f4=(void*)((_tmp9D8=
_region_malloc(r,sizeof(*_tmp9D8)),((_tmp9D8[0]=((_tmp9D9.tag=1,((_tmp9D9.f1=Cyc_add_ccarg,
_tmp9D9)))),_tmp9D8)))),((_tmp9DC->f5=((_tmp9D7="Even more optimization",
_tag_dyneither(_tmp9D7,sizeof(char),23))),_tmp9DC)))))))))))),((_tmp880[14]=((
_tmp9E2=_region_malloc(r,sizeof(*_tmp9E2)),((_tmp9E2->f1=((_tmp9E1="-O2",
_tag_dyneither(_tmp9E1,sizeof(char),4))),((_tmp9E2->f2=0,((_tmp9E2->f3=((_tmp9E0="",
_tag_dyneither(_tmp9E0,sizeof(char),1))),((_tmp9E2->f4=(void*)((_tmp9DE=
_region_malloc(r,sizeof(*_tmp9DE)),((_tmp9DE[0]=((_tmp9DF.tag=1,((_tmp9DF.f1=Cyc_add_ccarg,
_tmp9DF)))),_tmp9DE)))),((_tmp9E2->f5=((_tmp9DD="A higher level of optimization",
_tag_dyneither(_tmp9DD,sizeof(char),31))),_tmp9E2)))))))))))),((_tmp880[13]=((
_tmp9E8=_region_malloc(r,sizeof(*_tmp9E8)),((_tmp9E8->f1=((_tmp9E7="-O",
_tag_dyneither(_tmp9E7,sizeof(char),3))),((_tmp9E8->f2=0,((_tmp9E8->f3=((_tmp9E6="",
_tag_dyneither(_tmp9E6,sizeof(char),1))),((_tmp9E8->f4=(void*)((_tmp9E4=
_region_malloc(r,sizeof(*_tmp9E4)),((_tmp9E4[0]=((_tmp9E5.tag=1,((_tmp9E5.f1=Cyc_add_ccarg,
_tmp9E5)))),_tmp9E4)))),((_tmp9E8->f5=((_tmp9E3="Optimize",_tag_dyneither(
_tmp9E3,sizeof(char),9))),_tmp9E8)))))))))))),((_tmp880[12]=((_tmp9EE=
_region_malloc(r,sizeof(*_tmp9EE)),((_tmp9EE->f1=((_tmp9ED="-O0",_tag_dyneither(
_tmp9ED,sizeof(char),4))),((_tmp9EE->f2=0,((_tmp9EE->f3=((_tmp9EC="",
_tag_dyneither(_tmp9EC,sizeof(char),1))),((_tmp9EE->f4=(void*)((_tmp9EA=
_region_malloc(r,sizeof(*_tmp9EA)),((_tmp9EA[0]=((_tmp9EB.tag=1,((_tmp9EB.f1=Cyc_add_ccarg,
_tmp9EB)))),_tmp9EA)))),((_tmp9EE->f5=((_tmp9E9="Don't optimize",_tag_dyneither(
_tmp9E9,sizeof(char),15))),_tmp9EE)))))))))))),((_tmp880[11]=((_tmp9F4=
_region_malloc(r,sizeof(*_tmp9F4)),((_tmp9F4->f1=((_tmp9F3="-s",_tag_dyneither(
_tmp9F3,sizeof(char),3))),((_tmp9F4->f2=0,((_tmp9F4->f3=((_tmp9F2="",
_tag_dyneither(_tmp9F2,sizeof(char),1))),((_tmp9F4->f4=(void*)((_tmp9F0=
_region_malloc(r,sizeof(*_tmp9F0)),((_tmp9F0[0]=((_tmp9F1.tag=1,((_tmp9F1.f1=Cyc_add_ccarg,
_tmp9F1)))),_tmp9F0)))),((_tmp9F4->f5=((_tmp9EF="Remove all symbol table and relocation info from executable",
_tag_dyneither(_tmp9EF,sizeof(char),60))),_tmp9F4)))))))))))),((_tmp880[10]=((
_tmp9FA=_region_malloc(r,sizeof(*_tmp9FA)),((_tmp9FA->f1=((_tmp9F9="-x",
_tag_dyneither(_tmp9F9,sizeof(char),3))),((_tmp9FA->f2=0,((_tmp9FA->f3=((_tmp9F8=" <language>",
_tag_dyneither(_tmp9F8,sizeof(char),12))),((_tmp9FA->f4=(void*)((_tmp9F6=
_region_malloc(r,sizeof(*_tmp9F6)),((_tmp9F6[0]=((_tmp9F7.tag=5,((_tmp9F7.f1=Cyc_set_inputtype,
_tmp9F7)))),_tmp9F6)))),((_tmp9FA->f5=((_tmp9F5="Specify <language> for the following input files",
_tag_dyneither(_tmp9F5,sizeof(char),49))),_tmp9FA)))))))))))),((_tmp880[9]=((
_tmpA00=_region_malloc(r,sizeof(*_tmpA00)),((_tmpA00->f1=((_tmp9FF="-c",
_tag_dyneither(_tmp9FF,sizeof(char),3))),((_tmpA00->f2=0,((_tmpA00->f3=((_tmp9FE="",
_tag_dyneither(_tmp9FE,sizeof(char),1))),((_tmpA00->f4=(void*)((_tmp9FC=
_region_malloc(r,sizeof(*_tmp9FC)),((_tmp9FC[0]=((_tmp9FD.tag=0,((_tmp9FD.f1=Cyc_set_stop_after_objectfile,
_tmp9FD)))),_tmp9FC)))),((_tmpA00->f5=((_tmp9FB="Produce an object file instead of an executable; do not link",
_tag_dyneither(_tmp9FB,sizeof(char),61))),_tmpA00)))))))))))),((_tmp880[8]=((
_tmpA06=_region_malloc(r,sizeof(*_tmpA06)),((_tmpA06->f1=((_tmpA05="-m",
_tag_dyneither(_tmpA05,sizeof(char),3))),((_tmpA06->f2=1,((_tmpA06->f3=((_tmpA04="<option>",
_tag_dyneither(_tmpA04,sizeof(char),9))),((_tmpA06->f4=(void*)((_tmpA02=
_region_malloc(r,sizeof(*_tmpA02)),((_tmpA02[0]=((_tmpA03.tag=1,((_tmpA03.f1=Cyc_add_marg,
_tmpA03)))),_tmpA02)))),((_tmpA06->f5=((_tmpA01="GCC specific: pass machine-dependent flag on to gcc",
_tag_dyneither(_tmpA01,sizeof(char),52))),_tmpA06)))))))))))),((_tmp880[7]=((
_tmpA0C=_region_malloc(r,sizeof(*_tmpA0C)),((_tmpA0C->f1=((_tmpA0B="-l",
_tag_dyneither(_tmpA0B,sizeof(char),3))),((_tmpA0C->f2=1,((_tmpA0C->f3=((_tmpA0A="<libname>",
_tag_dyneither(_tmpA0A,sizeof(char),10))),((_tmpA0C->f4=(void*)((_tmpA08=
_region_malloc(r,sizeof(*_tmpA08)),((_tmpA08[0]=((_tmpA09.tag=1,((_tmpA09.f1=Cyc_add_libarg,
_tmpA09)))),_tmpA08)))),((_tmpA0C->f5=((_tmpA07="Library file",_tag_dyneither(
_tmpA07,sizeof(char),13))),_tmpA0C)))))))))))),((_tmp880[6]=((_tmpA12=
_region_malloc(r,sizeof(*_tmpA12)),((_tmpA12->f1=((_tmpA11="-L",_tag_dyneither(
_tmpA11,sizeof(char),3))),((_tmpA12->f2=1,((_tmpA12->f3=((_tmpA10="<dir>",
_tag_dyneither(_tmpA10,sizeof(char),6))),((_tmpA12->f4=(void*)((_tmpA0E=
_region_malloc(r,sizeof(*_tmpA0E)),((_tmpA0E[0]=((_tmpA0F.tag=1,((_tmpA0F.f1=Cyc_add_ccarg,
_tmpA0F)))),_tmpA0E)))),((_tmpA12->f5=((_tmpA0D="Add to the list of directories for -l",
_tag_dyneither(_tmpA0D,sizeof(char),38))),_tmpA12)))))))))))),((_tmp880[5]=((
_tmpA18=_region_malloc(r,sizeof(*_tmpA18)),((_tmpA18->f1=((_tmpA17="-I",
_tag_dyneither(_tmpA17,sizeof(char),3))),((_tmpA18->f2=1,((_tmpA18->f3=((_tmpA16="<dir>",
_tag_dyneither(_tmpA16,sizeof(char),6))),((_tmpA18->f4=(void*)((_tmpA14=
_region_malloc(r,sizeof(*_tmpA14)),((_tmpA14[0]=((_tmpA15.tag=1,((_tmpA15.f1=Cyc_add_cpparg,
_tmpA15)))),_tmpA14)))),((_tmpA18->f5=((_tmpA13="Add to the list of directories to search for include files",
_tag_dyneither(_tmpA13,sizeof(char),59))),_tmpA18)))))))))))),((_tmp880[4]=((
_tmpA1E=_region_malloc(r,sizeof(*_tmpA1E)),((_tmpA1E->f1=((_tmpA1D="-B",
_tag_dyneither(_tmpA1D,sizeof(char),3))),((_tmpA1E->f2=1,((_tmpA1E->f3=((_tmpA1C="<file>",
_tag_dyneither(_tmpA1C,sizeof(char),7))),((_tmpA1E->f4=(void*)((_tmpA1A=
_region_malloc(r,sizeof(*_tmpA1A)),((_tmpA1A[0]=((_tmpA1B.tag=1,((_tmpA1B.f1=Cyc_add_cyclone_exec_path,
_tmpA1B)))),_tmpA1A)))),((_tmpA1E->f5=((_tmpA19="Add to the list of directories to search for compiler files",
_tag_dyneither(_tmpA19,sizeof(char),60))),_tmpA1E)))))))))))),((_tmp880[3]=((
_tmpA24=_region_malloc(r,sizeof(*_tmpA24)),((_tmpA24->f1=((_tmpA23="-D",
_tag_dyneither(_tmpA23,sizeof(char),3))),((_tmpA24->f2=1,((_tmpA24->f3=((_tmpA22="<name>[=<value>]",
_tag_dyneither(_tmpA22,sizeof(char),17))),((_tmpA24->f4=(void*)((_tmpA20=
_region_malloc(r,sizeof(*_tmpA20)),((_tmpA20[0]=((_tmpA21.tag=1,((_tmpA21.f1=Cyc_add_cpparg,
_tmpA21)))),_tmpA20)))),((_tmpA24->f5=((_tmpA1F="Pass definition to preprocessor",
_tag_dyneither(_tmpA1F,sizeof(char),32))),_tmpA24)))))))))))),((_tmp880[2]=((
_tmpA2A=_region_malloc(r,sizeof(*_tmpA2A)),((_tmpA2A->f1=((_tmpA29="-o",
_tag_dyneither(_tmpA29,sizeof(char),3))),((_tmpA2A->f2=0,((_tmpA2A->f3=((_tmpA28=" <file>",
_tag_dyneither(_tmpA28,sizeof(char),8))),((_tmpA2A->f4=(void*)((_tmpA26=
_region_malloc(r,sizeof(*_tmpA26)),((_tmpA26[0]=((_tmpA27.tag=5,((_tmpA27.f1=Cyc_set_output_file,
_tmpA27)))),_tmpA26)))),((_tmpA2A->f5=((_tmpA25="Set the output file name to <file>",
_tag_dyneither(_tmpA25,sizeof(char),35))),_tmpA2A)))))))))))),((_tmp880[1]=((
_tmpA30=_region_malloc(r,sizeof(*_tmpA30)),((_tmpA30->f1=((_tmpA2F="--version",
_tag_dyneither(_tmpA2F,sizeof(char),10))),((_tmpA30->f2=0,((_tmpA30->f3=((
_tmpA2E="",_tag_dyneither(_tmpA2E,sizeof(char),1))),((_tmpA30->f4=(void*)((
_tmpA2C=_region_malloc(r,sizeof(*_tmpA2C)),((_tmpA2C[0]=((_tmpA2D.tag=0,((
_tmpA2D.f1=Cyc_print_version,_tmpA2D)))),_tmpA2C)))),((_tmpA30->f5=((_tmpA2B="Print version information and exit",
_tag_dyneither(_tmpA2B,sizeof(char),35))),_tmpA30)))))))))))),((_tmp880[0]=((
_tmpA36=_region_malloc(r,sizeof(*_tmpA36)),((_tmpA36->f1=((_tmpA35="-v",
_tag_dyneither(_tmpA35,sizeof(char),3))),((_tmpA36->f2=0,((_tmpA36->f3=((_tmpA34="",
_tag_dyneither(_tmpA34,sizeof(char),1))),((_tmpA36->f4=(void*)((_tmpA32=
_region_malloc(r,sizeof(*_tmpA32)),((_tmpA32[0]=((_tmpA33.tag=3,((_tmpA33.f1=&
Cyc_v_r,_tmpA33)))),_tmpA32)))),((_tmpA36->f5=((_tmpA31="Print compilation stages verbosely",
_tag_dyneither(_tmpA31,sizeof(char),35))),_tmpA36)))))))))))),((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(
_tmp880,sizeof(struct _tuple22*),73)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
const char*_tmpA37;struct _dyneither_ptr optstring=(_tmpA37="Options:",
_tag_dyneither(_tmpA37,sizeof(char),9));Cyc_Arg_current=0;Cyc_Arg_parse(options,
Cyc_add_other,optstring,_tmp165);if((char*)Cyc_target_arch.curr == (char*)(
_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)
Cstring_to_string(Carch);{char*_tmpA38;struct _dyneither_ptr cyclone_exec_prefix=(
_tmpA38=getenv((const char*)"CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmpA38,sizeof(
char),_get_zero_arr_size_char((void*)_tmpA38,1)));if((char*)cyclone_exec_prefix.curr
!= (char*)(_tag_dyneither(0,0,0)).curr){struct _dyneither_ptr*_tmpA3B;struct Cyc_List_List*
_tmpA3A;Cyc_cyclone_exec_path=((_tmpA3A=_cycalloc(sizeof(*_tmpA3A)),((_tmpA3A->hd=((
_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B[0]=cyclone_exec_prefix,_tmpA3B)))),((
_tmpA3A->tl=Cyc_cyclone_exec_path,_tmpA3A))))));}def_lib_path=(struct
_dyneither_ptr)Cstring_to_string(Cdef_lib_path);if(Cyc_strlen((struct
_dyneither_ptr)def_lib_path)> 0){struct _dyneither_ptr*_tmpA41;const char*_tmpA40;
struct Cyc_List_List*_tmpA3F;Cyc_cyclone_exec_path=((_tmpA3F=_cycalloc(sizeof(*
_tmpA3F)),((_tmpA3F->hd=((_tmpA41=_cycalloc(sizeof(*_tmpA41)),((_tmpA41[0]=(
struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmpA40="cyc-lib",
_tag_dyneither(_tmpA40,sizeof(char),8)))),_tmpA41)))),((_tmpA3F->tl=Cyc_cyclone_exec_path,
_tmpA3F))))));}Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);cyclone_arch_path=Cyc_also_subdir(
Cyc_cyclone_exec_path,Cyc_target_arch);{const char*_tmpA42;struct _dyneither_ptr
_tmp16C=Cyc_do_find(cyclone_arch_path,((_tmpA42="cycspecs",_tag_dyneither(
_tmpA42,sizeof(char),9))));if(Cyc_v_r){const char*_tmpA46;void*_tmpA45[1];struct
Cyc_String_pa_struct _tmpA44;(_tmpA44.tag=0,((_tmpA44.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp16C),((_tmpA45[0]=& _tmpA44,Cyc_fprintf(Cyc_stderr,((
_tmpA46="Reading from specs file %s\n",_tag_dyneither(_tmpA46,sizeof(char),28))),
_tag_dyneither(_tmpA45,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp170=Cyc_read_specs(
_tmp16C);struct _dyneither_ptr _tmp171=_tag_dyneither(0,0,0);{struct _handler_cons
_tmp172;_push_handler(& _tmp172);{int _tmp174=0;if(setjmp(_tmp172.handler))_tmp174=
1;if(!_tmp174){{struct _dyneither_ptr _tmp175=*((struct _dyneither_ptr*(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp170,_init_dyneither_ptr(
_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));_tmp171=Cyc_split_specs(
_tmp175);};_pop_handler();}else{void*_tmp173=(void*)_exn_thrown;void*_tmp178=
_tmp173;_LL35: {struct Cyc_Core_Not_found_struct*_tmp179=(struct Cyc_Core_Not_found_struct*)
_tmp178;if(_tmp179->tag != Cyc_Core_Not_found)goto _LL37;}_LL36: goto _LL34;_LL37:;
_LL38:(void)_throw(_tmp178);_LL34:;}};}if((struct _dyneither_ptr*)_tmp171.curr != (
struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp171);}Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp166);{const char*_tmpA47;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_cpp,((_tmpA47="",_tag_dyneither(_tmpA47,sizeof(char),1))))
== 0){const char*_tmp17B=Cyc_produce_dependencies?"":(const char*)" -E";const char*
_tmpA4F;void*_tmpA4E[3];struct Cyc_String_pa_struct _tmpA4D;const char*_tmpA4C;
struct Cyc_String_pa_struct _tmpA4B;struct Cyc_String_pa_struct _tmpA4A;Cyc_set_cpp((
struct _dyneither_ptr)((_tmpA4A.tag=0,((_tmpA4A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp16C),((_tmpA4B.tag=0,((_tmpA4B.f1=(struct _dyneither_ptr)((
_tmpA4C=_tmp17B,_tag_dyneither(_tmpA4C,sizeof(char),_get_zero_arr_size_char((
void*)_tmpA4C,1)))),((_tmpA4D.tag=0,((_tmpA4D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)comp),((_tmpA4E[0]=& _tmpA4D,((_tmpA4E[1]=& _tmpA4B,((_tmpA4E[2]=&
_tmpA4A,Cyc_aprintf(((_tmpA4F="%s -w -x c%s -specs %s",_tag_dyneither(_tmpA4F,
sizeof(char),23))),_tag_dyneither(_tmpA4E,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){{const char*_tmpA52;void*_tmpA51;(
_tmpA51=0,Cyc_fprintf(Cyc_stderr,((_tmpA52="missing file\n",_tag_dyneither(
_tmpA52,sizeof(char),14))),_tag_dyneither(_tmpA51,sizeof(void*),0)));}exit(1);}
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){const char*_tmpA53;Cyc_cyc_setjmp=
Cyc_do_find(cyclone_arch_path,((_tmpA53="cyc_setjmp.h",_tag_dyneither(_tmpA53,
sizeof(char),13))));}if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct
_dyneither_ptr)Cyc_cyc_include)== 0){const char*_tmpA54;Cyc_cyc_include=Cyc_do_find(
Cyc_cyclone_exec_path,((_tmpA54="cyc_include.h",_tag_dyneither(_tmpA54,sizeof(
char),14))));}{struct Cyc_List_List*_tmp34C=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp34C != 0;_tmp34C=_tmp34C->tl){Cyc_process_file(*((
struct _dyneither_ptr*)_tmp34C->hd));if(Cyc_compile_failure){int _tmp34D=1;
_npop_handler(0);return _tmp34D;}}}if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r)
 || Cyc_tc_r) || Cyc_toc_r){int _tmp34E=0;_npop_handler(0);return _tmp34E;}if(Cyc_ccargs
== 0){int _tmp34F=0;_npop_handler(0);return _tmp34F;}{const char*_tmpA55;Cyc_add_ccarg((
struct _dyneither_ptr)Cyc_strconcat(((_tmpA55="-L",_tag_dyneither(_tmpA55,sizeof(
char),3))),(struct _dyneither_ptr)def_lib_path));}Cyc_ccargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{const char*_tmpA56;struct
_dyneither_ptr _tmp351=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct
_dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpA56=" ",_tag_dyneither(_tmpA56,sizeof(
char),2))));Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_libargs);{const char*_tmpA58;struct Cyc_List_List*_tmpA57;struct _dyneither_ptr
_tmp352=Cyc_str_sepstr(((_tmpA57=_cycalloc(sizeof(*_tmpA57)),((_tmpA57->hd=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((
_tmpA57->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_libargs),_tmpA57)))))),((_tmpA58=" ",_tag_dyneither(_tmpA58,sizeof(char),2))));
struct Cyc_List_List*stdlib;struct _dyneither_ptr stdlib_string;const char*_tmpA5A;
const char*_tmpA59;int _tmp353=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r)
 || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)
_check_null(Cyc_output_file)),((_tmpA5A=".a",_tag_dyneither(_tmpA5A,sizeof(char),
3))))) || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpA59=".lib",_tag_dyneither(
_tmpA59,sizeof(char),5))));if(_tmp353){stdlib=0;{const char*_tmpA5B;stdlib_string=(
struct _dyneither_ptr)((_tmpA5B="",_tag_dyneither(_tmpA5B,sizeof(char),1)));};}
else{struct _dyneither_ptr libcyc_flag;struct _dyneither_ptr nogc_filename;struct
_dyneither_ptr runtime_filename;struct _dyneither_ptr gc_filename;if(Cyc_pa_r){{
const char*_tmpA5C;libcyc_flag=((_tmpA5C="-lcyc_a",_tag_dyneither(_tmpA5C,sizeof(
char),8)));}{const char*_tmpA5D;nogc_filename=((_tmpA5D="nogc_a.a",_tag_dyneither(
_tmpA5D,sizeof(char),9)));}{const char*_tmpA5E;runtime_filename=((_tmpA5E="runtime_cyc_a.o",
_tag_dyneither(_tmpA5E,sizeof(char),16)));};}else{if(Cyc_nocheck_r){{const char*
_tmpA5F;libcyc_flag=((_tmpA5F="-lcyc_nocheck",_tag_dyneither(_tmpA5F,sizeof(char),
14)));}{const char*_tmpA60;nogc_filename=((_tmpA60="nogc.a",_tag_dyneither(
_tmpA60,sizeof(char),7)));}{const char*_tmpA61;runtime_filename=((_tmpA61="runtime_cyc.o",
_tag_dyneither(_tmpA61,sizeof(char),14)));};}else{if(Cyc_pg_r){{const char*
_tmpA62;libcyc_flag=((_tmpA62="-lcyc_pg",_tag_dyneither(_tmpA62,sizeof(char),9)));}{
const char*_tmpA63;runtime_filename=((_tmpA63="runtime_cyc_pg.o",_tag_dyneither(
_tmpA63,sizeof(char),17)));}{const char*_tmpA64;nogc_filename=((_tmpA64="nogc_pg.a",
_tag_dyneither(_tmpA64,sizeof(char),10)));};}else{if(Cyc_compile_for_boot_r){{
const char*_tmpA65;libcyc_flag=((_tmpA65="-lcycboot",_tag_dyneither(_tmpA65,
sizeof(char),10)));}{const char*_tmpA66;nogc_filename=((_tmpA66="nogc.a",
_tag_dyneither(_tmpA66,sizeof(char),7)));}{const char*_tmpA67;runtime_filename=((
_tmpA67="runtime_cyc.o",_tag_dyneither(_tmpA67,sizeof(char),14)));};}else{{const
char*_tmpA68;libcyc_flag=((_tmpA68="-lcyc",_tag_dyneither(_tmpA68,sizeof(char),6)));}{
const char*_tmpA69;nogc_filename=((_tmpA69="nogc.a",_tag_dyneither(_tmpA69,
sizeof(char),7)));}{const char*_tmpA6A;runtime_filename=((_tmpA6A="runtime_cyc.o",
_tag_dyneither(_tmpA6A,sizeof(char),14)));};}}}}{const char*_tmpA6B;gc_filename=((
_tmpA6B="gc.a",_tag_dyneither(_tmpA6B,sizeof(char),5)));}{struct _dyneither_ptr
_tmp365=Cyc_nogc_r?Cyc_do_find(cyclone_arch_path,nogc_filename): Cyc_do_find(
cyclone_arch_path,gc_filename);struct _dyneither_ptr _tmp366=Cyc_do_find(
cyclone_arch_path,runtime_filename);stdlib=0;{const char*_tmpA71;void*_tmpA70[3];
struct Cyc_String_pa_struct _tmpA6F;struct Cyc_String_pa_struct _tmpA6E;struct Cyc_String_pa_struct
_tmpA6D;stdlib_string=(struct _dyneither_ptr)((_tmpA6D.tag=0,((_tmpA6D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp365),((_tmpA6E.tag=0,((_tmpA6E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpA6F.tag=0,((
_tmpA6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp366),((_tmpA70[0]=&
_tmpA6F,((_tmpA70[1]=& _tmpA6E,((_tmpA70[2]=& _tmpA6D,Cyc_aprintf(((_tmpA71=" %s %s %s",
_tag_dyneither(_tmpA71,sizeof(char),10))),_tag_dyneither(_tmpA70,sizeof(void*),3))))))))))))))))))));};};}
if(Cyc_ic_r){struct _handler_cons _tmp36C;_push_handler(& _tmp36C);{int _tmp36E=0;
if(setjmp(_tmp36C.handler))_tmp36E=1;if(!_tmp36E){Cyc_ccargs=((struct Cyc_List_List*(*)(
int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,
Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*
_tmp36F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(!_tmp353){struct
Cyc_List_List*_tmpA72;_tmp36F=((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72->hd=
Cyc_final_strptr,((_tmpA72->tl=_tmp36F,_tmpA72))))));}{struct Cyc_Interface_I*
_tmp371=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp36F,_tmp36F);if(_tmp371 == 0){{const char*_tmpA75;void*_tmpA74;(
_tmpA74=0,Cyc_fprintf(Cyc_stderr,((_tmpA75="Error: interfaces incompatible\n",
_tag_dyneither(_tmpA75,sizeof(char),32))),_tag_dyneither(_tmpA74,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp374=1;_npop_handler(1);return
_tmp374;};}if(_tmp353){if(Cyc_output_file != 0){const char*_tmpA79;void*_tmpA78[1];
struct Cyc_String_pa_struct _tmpA77;struct _dyneither_ptr _tmp375=(_tmpA77.tag=0,((
_tmpA77.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpA78[0]=& _tmpA77,Cyc_aprintf(((
_tmpA79="%s.cycio",_tag_dyneither(_tmpA79,sizeof(char),9))),_tag_dyneither(
_tmpA78,sizeof(void*),1)))))));const char*_tmpA7B;const char*_tmpA7A;struct Cyc___cycFILE*
_tmp376=Cyc_try_file_open((struct _dyneither_ptr)_tmp375,((_tmpA7A="wb",
_tag_dyneither(_tmpA7A,sizeof(char),3))),((_tmpA7B="interface object file",
_tag_dyneither(_tmpA7B,sizeof(char),22))));if(_tmp376 == 0){Cyc_compile_failure=1;
Cyc_remove_cfiles();{int _tmp377=1;_npop_handler(1);return _tmp377;};}Cyc_Interface_save((
struct Cyc_Interface_I*)_tmp371,(struct Cyc___cycFILE*)_tmp376);Cyc_file_close((
struct Cyc___cycFILE*)_tmp376);}}else{const char*_tmpA80;const char*_tmpA7F;struct
_tuple18*_tmpA7E;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp371,((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E->f1=((
_tmpA7F="empty interface",_tag_dyneither(_tmpA7F,sizeof(char),16))),((_tmpA7E->f2=((
_tmpA80="global interface",_tag_dyneither(_tmpA80,sizeof(char),17))),_tmpA7E)))))))){{
const char*_tmpA83;void*_tmpA82;(_tmpA82=0,Cyc_fprintf(Cyc_stderr,((_tmpA83="Error: some objects are still undefined\n",
_tag_dyneither(_tmpA83,sizeof(char),41))),_tag_dyneither(_tmpA82,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp382=1;_npop_handler(1);return
_tmp382;};}}};};;_pop_handler();}else{void*_tmp36D=(void*)_exn_thrown;void*
_tmp384=_tmp36D;_LL3A:;_LL3B:{void*_tmp385=_tmp384;struct _dyneither_ptr _tmp387;
struct _dyneither_ptr _tmp389;struct _dyneither_ptr _tmp38C;_LL3F: {struct Cyc_Core_Failure_struct*
_tmp386=(struct Cyc_Core_Failure_struct*)_tmp385;if(_tmp386->tag != Cyc_Core_Failure)
goto _LL41;else{_tmp387=_tmp386->f1;}}_LL40:{const char*_tmpA87;void*_tmpA86[1];
struct Cyc_String_pa_struct _tmpA85;(_tmpA85.tag=0,((_tmpA85.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp387),((_tmpA86[0]=& _tmpA85,Cyc_fprintf(
Cyc_stderr,((_tmpA87="Exception Core::Failure %s\n",_tag_dyneither(_tmpA87,
sizeof(char),28))),_tag_dyneither(_tmpA86,sizeof(void*),1)))))));}goto _LL3E;
_LL41: {struct Cyc_Core_Impossible_struct*_tmp388=(struct Cyc_Core_Impossible_struct*)
_tmp385;if(_tmp388->tag != Cyc_Core_Impossible)goto _LL43;else{_tmp389=_tmp388->f1;}}
_LL42:{const char*_tmpA8B;void*_tmpA8A[1];struct Cyc_String_pa_struct _tmpA89;(
_tmpA89.tag=0,((_tmpA89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp389),((
_tmpA8A[0]=& _tmpA89,Cyc_fprintf(Cyc_stderr,((_tmpA8B="Exception Core::Impossible %s\n",
_tag_dyneither(_tmpA8B,sizeof(char),31))),_tag_dyneither(_tmpA8A,sizeof(void*),1)))))));}
goto _LL3E;_LL43: {struct Cyc_Dict_Absent_struct*_tmp38A=(struct Cyc_Dict_Absent_struct*)
_tmp385;if(_tmp38A->tag != Cyc_Dict_Absent)goto _LL45;}_LL44:{const char*_tmpA8E;
void*_tmpA8D;(_tmpA8D=0,Cyc_fprintf(Cyc_stderr,((_tmpA8E="Exception Dict::Absent\n",
_tag_dyneither(_tmpA8E,sizeof(char),24))),_tag_dyneither(_tmpA8D,sizeof(void*),0)));}
goto _LL3E;_LL45: {struct Cyc_Core_Invalid_argument_struct*_tmp38B=(struct Cyc_Core_Invalid_argument_struct*)
_tmp385;if(_tmp38B->tag != Cyc_Core_Invalid_argument)goto _LL47;else{_tmp38C=
_tmp38B->f1;}}_LL46:{const char*_tmpA92;void*_tmpA91[1];struct Cyc_String_pa_struct
_tmpA90;(_tmpA90.tag=0,((_tmpA90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp38C),((_tmpA91[0]=& _tmpA90,Cyc_fprintf(Cyc_stderr,((_tmpA92="Exception Core::Invalid_argument %s\n",
_tag_dyneither(_tmpA92,sizeof(char),37))),_tag_dyneither(_tmpA91,sizeof(void*),1)))))));}
goto _LL3E;_LL47:;_LL48:{const char*_tmpA95;void*_tmpA94;(_tmpA94=0,Cyc_fprintf(
Cyc_stderr,((_tmpA95="Uncaught exception\n",_tag_dyneither(_tmpA95,sizeof(char),
20))),_tag_dyneither(_tmpA94,sizeof(void*),0)));}goto _LL3E;_LL3E:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();{int _tmp39A=1;_npop_handler(0);return _tmp39A;};_LL3C:;
_LL3D:(void)_throw(_tmp384);_LL39:;}};}{const char*_tmpA96;struct _dyneither_ptr
outfile_str=(_tmpA96="",_tag_dyneither(_tmpA96,sizeof(char),1));if(Cyc_output_file
!= 0){const char*_tmpA9A;void*_tmpA99[1];struct Cyc_String_pa_struct _tmpA98;
outfile_str=(struct _dyneither_ptr)((_tmpA98.tag=0,((_tmpA98.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpA99[0]=& _tmpA98,Cyc_aprintf(((
_tmpA9A=" -o %s",_tag_dyneither(_tmpA9A,sizeof(char),7))),_tag_dyneither(_tmpA99,
sizeof(void*),1))))))));}{const char*_tmpAA2;void*_tmpAA1[5];struct Cyc_String_pa_struct
_tmpAA0;struct Cyc_String_pa_struct _tmpA9F;struct Cyc_String_pa_struct _tmpA9E;
struct Cyc_String_pa_struct _tmpA9D;struct Cyc_String_pa_struct _tmpA9C;struct
_dyneither_ptr _tmp39E=(_tmpA9C.tag=0,((_tmpA9C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp352),((_tmpA9D.tag=0,((_tmpA9D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)stdlib_string),((_tmpA9E.tag=0,((_tmpA9E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp351),((_tmpA9F.tag=0,((_tmpA9F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpAA0.tag=0,((
_tmpAA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpAA1[0]=&
_tmpAA0,((_tmpAA1[1]=& _tmpA9F,((_tmpAA1[2]=& _tmpA9E,((_tmpAA1[3]=& _tmpA9D,((
_tmpAA1[4]=& _tmpA9C,Cyc_aprintf(((_tmpAA2="%s %s %s%s%s",_tag_dyneither(_tmpAA2,
sizeof(char),13))),_tag_dyneither(_tmpAA1,sizeof(void*),5)))))))))))))))))))))))))))))));
if(Cyc_v_r){{const char*_tmpAA6;void*_tmpAA5[1];struct Cyc_String_pa_struct _tmpAA4;(
_tmpAA4.tag=0,((_tmpAA4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp39E),((
_tmpAA5[0]=& _tmpAA4,Cyc_fprintf(Cyc_stderr,((_tmpAA6="%s\n",_tag_dyneither(
_tmpAA6,sizeof(char),4))),_tag_dyneither(_tmpAA5,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)_untag_dyneither_ptr(
_tmp39E,sizeof(char),1))!= 0){{const char*_tmpAA9;void*_tmpAA8;(_tmpAA8=0,Cyc_fprintf(
Cyc_stderr,((_tmpAA9="Error: C compiler failed\n",_tag_dyneither(_tmpAA9,sizeof(
char),26))),_tag_dyneither(_tmpAA8,sizeof(void*),0)));}Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp3A4=1;_npop_handler(0);return _tmp3A4;};}Cyc_remove_cfiles();{int _tmp3A5=
Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp3A5;};};};};};;_pop_region(r);};}
