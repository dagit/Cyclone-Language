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
_dyneither_ptr Cyc_mbuffer_t;typedef int Cyc_bool;void exit(int);struct Cyc_Core_NewRegion{
struct _DynRegionHandle*dynregion;};typedef unsigned long Cyc_size_t;typedef
unsigned short Cyc_mode_t;struct Cyc___cycFILE;typedef struct Cyc___cycFILE Cyc_FILE;
extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*Cyc_stderr;struct
Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int
tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};typedef void*Cyc_parg_t;struct _dyneither_ptr Cyc_aprintf(
struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_fclose(struct Cyc___cycFILE*);
int Cyc_feof(struct Cyc___cycFILE*);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fgetc(
struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int
Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
unsigned long Cyc_fread(struct _dyneither_ptr,unsigned long,unsigned long,struct
Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
typedef void*Cyc_sarg_t;unsigned long Cyc_fwrite(struct _dyneither_ptr,
unsigned long,unsigned long,struct Cyc___cycFILE*);int Cyc_printf(struct
_dyneither_ptr,struct _dyneither_ptr);int remove(const char*);extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc___cycFILE*
Cyc_file_open(struct _dyneither_ptr,struct _dyneither_ptr);void Cyc_file_close(
struct Cyc___cycFILE*);int isspace(int);char*getenv(const char*);int system(const
char*);typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};typedef
struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct
Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[
8];struct Cyc_Core_Failure_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[
11];struct Cyc_Core_Impossible_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{char*tag;};extern char
Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_unique_region;extern char
Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};extern char
Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};struct
_dyneither_ptr Cstring_to_string(char*);inline static void* arrcast(struct
_dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || 
dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}struct
Cyc_List_List{void*hd;struct Cyc_List_List*tl;};typedef struct Cyc_List_List*Cyc_List_list_t;
typedef struct Cyc_List_List*Cyc_List_List_t;struct Cyc_List_List*Cyc_List_list(
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
_dyneither_ptr);};typedef void*Cyc_Arg_spec_t;typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
extern int Cyc_Arg_current;void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*
anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr
args);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);int Cyc_strncmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2,unsigned long len);struct _dyneither_ptr Cyc_strconcat(struct
_dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_strconcat_l(struct
Cyc_List_List*);struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct
_dyneither_ptr);struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);struct
_dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct
_dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;typedef const struct
Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};extern
char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct
_dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);struct
_dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);struct _dyneither_ptr Cyc_Filename_basename(
struct _dyneither_ptr);int Cyc_Filename_check_suffix(struct _dyneither_ptr,struct
_dyneither_ptr);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};void Cyc_Position_reset_position(
struct _dyneither_ptr);struct Cyc_Position_Segment;typedef struct Cyc_Position_Segment*
Cyc_Position_seg_t;struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};typedef void*Cyc_Position_error_kind_t;
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};typedef struct Cyc_Position_Error*Cyc_Position_error_t;
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};extern int Cyc_Position_use_gcc_style_location;extern int Cyc_Position_max_errors;
int Cyc_Position_error_p();typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
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
No_constr;};typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_struct{
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
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_Aggr_td_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};
struct Cyc_Absyn_Enum_td_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;};typedef void*Cyc_Absyn_raw_type_decl_t;
struct Cyc_Absyn_TypeDecl{void*r;struct Cyc_Position_Segment*loc;};typedef struct
Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_struct{int tag;
};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;
int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};
struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
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
Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;union
Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*f3;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};typedef void*Cyc_Absyn_type_modifier_t;
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
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_struct{
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
int lex_eof_reached;};typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};typedef
struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct
Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr
lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct
_dyneither_ptr lex_check;};typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);extern int Cyc_Parse_no_register;
typedef void*Cyc_type_specifier_t;enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc
 = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,
Cyc_Abstract_sc  = 6};typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec;
typedef struct Cyc_Declaration_spec*Cyc_decl_spec_t;struct Cyc_Declarator;typedef
struct Cyc_Declarator*Cyc_declarator_t;struct Cyc_Abstractdeclarator;typedef struct
Cyc_Abstractdeclarator*Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int
tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct
_union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct
_union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct
_union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple10{int f1;struct
_dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple10 val;};
struct _tuple11{struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;union
Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple11*val;};
struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
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
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};
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
first_line;int first_column;int last_line;int last_column;};typedef struct Cyc_Yyltype
Cyc_yyltype;struct Cyc_PP_Ppstate;typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;
struct Cyc_PP_Out;typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;typedef
struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int
qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int
print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs);
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*
f);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[
7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;typedef struct
Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tcutil_flush_warnings();
extern int Cyc_Tcutil_warn_alias_coerce;void Cyc_Tcutil_check_no_qual(struct Cyc_Position_Segment*
loc,void*t);void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int
var_default_init,struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds);extern int Cyc_Tovc_elim_array_initializers;struct Cyc_List_List*
Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 
1,Cyc_CfFlowInfo_AllIL  = 2};typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple17{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple17 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp
rop;};typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;typedef struct Cyc_List_List*
Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;typedef struct _dyneither_ptr
Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int
fieldnum;};typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;
struct Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;
struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{
int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple18{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple18
val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct
_union_FlowInfo_ReachableFL ReachableFL;};typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};typedef struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};
struct Cyc_CfFlowInfo_RefCntRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};typedef void*Cyc_CfFlowInfo_killrgn_t;void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds);extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Interface_I;
typedef struct Cyc_Interface_I*Cyc_Interface_i_t;struct Cyc_Interface_I*Cyc_Interface_empty();
struct Cyc_Interface_I*Cyc_Interface_final();struct Cyc_Interface_I*Cyc_Interface_extract(
struct Cyc_Dict_Dict ae);struct _tuple19{struct _dyneither_ptr f1;struct
_dyneither_ptr f2;};int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,
struct Cyc_Interface_I*i2,struct _tuple19*info);struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*
linfo);struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);void Cyc_Interface_save(
struct Cyc_Interface_I*,struct Cyc___cycFILE*);struct Cyc_Interface_I*Cyc_Interface_load(
struct Cyc___cycFILE*);typedef struct Cyc_Interface_I*Cyc_interface_t;extern int Cyc_Cyclone_tovc_r;
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
_dyneither_ptr*_tmp3B7;struct Cyc_List_List*_tmp3B6;Cyc_ccargs=((_tmp3B6=
_cycalloc(sizeof(*_tmp3B6)),((_tmp3B6->hd=((_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((
_tmp3B7[0]=s,_tmp3B7)))),((_tmp3B6->tl=Cyc_ccargs,_tmp3B6))))));}void Cyc_set_c_compiler(
struct _dyneither_ptr s){const char*_tmp3B8;if(Cyc_strcmp((struct _dyneither_ptr)s,((
_tmp3B8="vc",_tag_dyneither(_tmp3B8,sizeof(char),3))))== 0){Cyc_Cyclone_c_compiler=
Cyc_Cyclone_Vc_c;{const char*_tmp3B9;Cyc_add_ccarg(((_tmp3B9="-DVC_C",
_tag_dyneither(_tmp3B9,sizeof(char),7))));};}else{const char*_tmp3BA;if(Cyc_strcmp((
struct _dyneither_ptr)s,((_tmp3BA="gcc",_tag_dyneither(_tmp3BA,sizeof(char),4))))
== 0){Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{const char*_tmp3BB;Cyc_add_ccarg(((
_tmp3BB="-DGCC_C",_tag_dyneither(_tmp3BB,sizeof(char),8))));};}}}void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;Cyc_Position_max_errors=65535;Cyc_save_c_r=1;Cyc_parseonly_r=
1;}static struct _dyneither_ptr*Cyc_output_file=0;static void Cyc_set_output_file(
struct _dyneither_ptr s){struct _dyneither_ptr*_tmp3BC;Cyc_output_file=((_tmp3BC=
_cycalloc(sizeof(*_tmp3BC)),((_tmp3BC[0]=s,_tmp3BC))));}extern char*Cdef_inc_path;
extern char*Cdef_lib_path;extern char*Carch;extern char*Ccomp;extern char*Cversion;
static char _tmp7[1]="";static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
static void Cyc_set_cpp(struct _dyneither_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*
Cyc_cppargs=0;static void Cyc_add_cpparg(struct _dyneither_ptr s){struct
_dyneither_ptr*_tmp3BF;struct Cyc_List_List*_tmp3BE;Cyc_cppargs=((_tmp3BE=
_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE->hd=((_tmp3BF=_cycalloc(sizeof(*_tmp3BF)),((
_tmp3BF[0]=s,_tmp3BF)))),((_tmp3BE->tl=Cyc_cppargs,_tmp3BE))))));}static void Cyc_add_iprefix(
struct _dyneither_ptr s){const char*_tmp3C3;void*_tmp3C2[1];struct Cyc_String_pa_struct
_tmp3C1;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3C1.tag=0,((_tmp3C1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3C2[0]=& _tmp3C1,Cyc_aprintf(((
_tmp3C3="-iprefix %s",_tag_dyneither(_tmp3C3,sizeof(char),12))),_tag_dyneither(
_tmp3C2,sizeof(void*),1)))))))));}static void Cyc_add_iwithprefix(struct
_dyneither_ptr s){const char*_tmp3C7;void*_tmp3C6[1];struct Cyc_String_pa_struct
_tmp3C5;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3C5.tag=0,((_tmp3C5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3C6[0]=& _tmp3C5,Cyc_aprintf(((
_tmp3C7="-iwithprefix %s",_tag_dyneither(_tmp3C7,sizeof(char),16))),
_tag_dyneither(_tmp3C6,sizeof(void*),1)))))))));}static void Cyc_add_iwithprefixbefore(
struct _dyneither_ptr s){const char*_tmp3CB;void*_tmp3CA[1];struct Cyc_String_pa_struct
_tmp3C9;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3C9.tag=0,((_tmp3C9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3CA[0]=& _tmp3C9,Cyc_aprintf(((
_tmp3CB="-iwithprefixbefore %s",_tag_dyneither(_tmp3CB,sizeof(char),22))),
_tag_dyneither(_tmp3CA,sizeof(void*),1)))))))));}static void Cyc_add_isystem(
struct _dyneither_ptr s){const char*_tmp3CF;void*_tmp3CE[1];struct Cyc_String_pa_struct
_tmp3CD;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3CD.tag=0,((_tmp3CD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3CE[0]=& _tmp3CD,Cyc_aprintf(((
_tmp3CF="-isystem %s",_tag_dyneither(_tmp3CF,sizeof(char),12))),_tag_dyneither(
_tmp3CE,sizeof(void*),1)))))))));}static void Cyc_add_idirafter(struct
_dyneither_ptr s){const char*_tmp3D3;void*_tmp3D2[1];struct Cyc_String_pa_struct
_tmp3D1;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3D1.tag=0,((_tmp3D1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3D2[0]=& _tmp3D1,Cyc_aprintf(((
_tmp3D3="-idirafter %s",_tag_dyneither(_tmp3D3,sizeof(char),14))),_tag_dyneither(
_tmp3D2,sizeof(void*),1)))))))));}static struct _dyneither_ptr Cyc_target_arch=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};static void Cyc_set_target_arch(
struct _dyneither_ptr s){Cyc_toc_r=1;Cyc_target_arch=s;}static void Cyc_print_version(){{
const char*_tmp3D7;void*_tmp3D6[1];struct Cyc_String_pa_struct _tmp3D5;(_tmp3D5.tag=
0,((_tmp3D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cversion)),((_tmp3D6[0]=& _tmp3D5,Cyc_printf(((_tmp3D7="The Cyclone compiler, version %s\n",
_tag_dyneither(_tmp3D7,sizeof(char),34))),_tag_dyneither(_tmp3D6,sizeof(void*),1)))))));}{
const char*_tmp3DC;void*_tmp3DB[2];struct Cyc_String_pa_struct _tmp3DA;struct Cyc_String_pa_struct
_tmp3D9;(_tmp3D9.tag=0,((_tmp3D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp3DA.tag=0,((_tmp3DA.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3DB[0]=& _tmp3DA,((_tmp3DB[1]=&
_tmp3D9,Cyc_printf(((_tmp3DC="Compiled for architecture: %s CC=\"%s\"\n",
_tag_dyneither(_tmp3DC,sizeof(char),39))),_tag_dyneither(_tmp3DB,sizeof(void*),2)))))))))))));}{
const char*_tmp3E0;void*_tmp3DF[1];struct Cyc_String_pa_struct _tmp3DE;(_tmp3DE.tag=
0,((_tmp3DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_lib_path)),((_tmp3DF[0]=& _tmp3DE,Cyc_printf(((_tmp3E0="Standard library directory: %s\n",
_tag_dyneither(_tmp3E0,sizeof(char),32))),_tag_dyneither(_tmp3DF,sizeof(void*),1)))))));}{
const char*_tmp3E4;void*_tmp3E3[1];struct Cyc_String_pa_struct _tmp3E2;(_tmp3E2.tag=
0,((_tmp3E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_inc_path)),((_tmp3E3[0]=& _tmp3E2,Cyc_printf(((_tmp3E4="Standard include directory: %s\n",
_tag_dyneither(_tmp3E4,sizeof(char),32))),_tag_dyneither(_tmp3E3,sizeof(void*),1)))))));}
exit(0);}static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){unsigned long
_tmp26=Cyc_strlen((struct _dyneither_ptr)s);if(_tmp26 <= 4)return 0;else{const char*
_tmp3E5;return Cyc_strcmp((struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(
int)(_tmp26 - 4)),((_tmp3E5=".cyc",_tag_dyneither(_tmp3E5,sizeof(char),5))))== 0;}}
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;static void Cyc_add_cyclone_exec_path(
struct _dyneither_ptr s){unsigned long _tmp28=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp28 <= 2)return;{struct _dyneither_ptr _tmp29=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)s,2,_tmp28 - 2);struct _dyneither_ptr*_tmp3E8;struct Cyc_List_List*
_tmp3E7;Cyc_cyclone_exec_path=((_tmp3E7=_cycalloc(sizeof(*_tmp3E7)),((_tmp3E7->hd=((
_tmp3E8=_cycalloc(sizeof(*_tmp3E8)),((_tmp3E8[0]=_tmp29,_tmp3E8)))),((_tmp3E7->tl=
Cyc_cyclone_exec_path,_tmp3E7))))));};}static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s){const char*_tmp3E9;if(Cyc_strcmp((
struct _dyneither_ptr)s,((_tmp3E9="-lxml",_tag_dyneither(_tmp3E9,sizeof(char),6))))
== 0){if(!Cyc_pa_r)Cyc_add_ccarg(s);else{const char*_tmp3EA;Cyc_add_ccarg(((
_tmp3EA="-lxml_a",_tag_dyneither(_tmp3EA,sizeof(char),8))));}}else{struct
_dyneither_ptr*_tmp3ED;struct Cyc_List_List*_tmp3EC;Cyc_libargs=((_tmp3EC=
_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC->hd=((_tmp3ED=_cycalloc(sizeof(*_tmp3ED)),((
_tmp3ED[0]=s,_tmp3ED)))),((_tmp3EC->tl=Cyc_libargs,_tmp3EC))))));}}static void Cyc_add_marg(
struct _dyneither_ptr s){Cyc_add_ccarg(s);}static void Cyc_set_save_temps(){Cyc_save_temps_r=
1;{const char*_tmp3EE;Cyc_add_ccarg(((_tmp3EE="-save-temps",_tag_dyneither(
_tmp3EE,sizeof(char),12))));};}static int Cyc_produce_dependencies=0;static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=1;{const char*_tmp3EF;Cyc_add_cpparg(((
_tmp3EF="-M",_tag_dyneither(_tmp3EF,sizeof(char),3))));};}static struct
_dyneither_ptr*Cyc_dependencies_target=0;static void Cyc_set_dependencies_target(
struct _dyneither_ptr s){struct _dyneither_ptr*_tmp3F0;Cyc_dependencies_target=((
_tmp3F0=_cycalloc(sizeof(*_tmp3F0)),((_tmp3F0[0]=s,_tmp3F0))));}static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;{const char*_tmp3F1;Cyc_add_ccarg(((_tmp3F1="-c",
_tag_dyneither(_tmp3F1,sizeof(char),3))));};}static void Cyc_set_nocppprecomp(){{
const char*_tmp3F2;Cyc_add_cpparg(((_tmp3F2="-no-cpp-precomp",_tag_dyneither(
_tmp3F2,sizeof(char),16))));}{const char*_tmp3F3;Cyc_add_ccarg(((_tmp3F3="-no-cpp-precomp",
_tag_dyneither(_tmp3F3,sizeof(char),16))));};}static void Cyc_set_lineno(){Cyc_generate_line_directives_r=
1;Cyc_set_save_temps();}static void Cyc_set_inline_functions(){Cyc_inline_functions_r=
1;}static void Cyc_set_elim_se(){Cyc_elim_se_r=1;Cyc_set_inline_functions();}
static void Cyc_set_tovc(){Cyc_Cyclone_tovc_r=1;{const char*_tmp3F4;Cyc_add_ccarg(((
_tmp3F4="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp3F4,sizeof(char),18))));}Cyc_set_elim_se();}
static void Cyc_set_noboundschecks(){const char*_tmp3F5;Cyc_add_ccarg(((_tmp3F5="-DNO_CYC_BOUNDS_CHECKS",
_tag_dyneither(_tmp3F5,sizeof(char),23))));}static void Cyc_set_nonullchecks(){
const char*_tmp3F6;Cyc_add_ccarg(((_tmp3F6="-DNO_CYC_NULL_CHECKS",_tag_dyneither(
_tmp3F6,sizeof(char),21))));}static void Cyc_set_nochecks(){Cyc_set_noboundschecks();
Cyc_set_nonullchecks();Cyc_nocheck_r=1;}static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=
0;{const char*_tmp3F7;Cyc_add_ccarg(((_tmp3F7="-DNO_CYC_PREFIX",_tag_dyneither(
_tmp3F7,sizeof(char),16))));};}static void Cyc_set_pa(){Cyc_pa_r=1;{const char*
_tmp3F8;Cyc_add_ccarg(((_tmp3F8="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp3F8,
sizeof(char),21))));}{const char*_tmp3F9;Cyc_add_cpparg(((_tmp3F9="-DCYC_REGION_PROFILE",
_tag_dyneither(_tmp3F9,sizeof(char),21))));};}static void Cyc_set_pg(){Cyc_pg_r=1;{
const char*_tmp3FA;Cyc_add_ccarg(((_tmp3FA="-pg",_tag_dyneither(_tmp3FA,sizeof(
char),4))));};}static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=1;{
const char*_tmp3FB;Cyc_add_ccarg(((_tmp3FB="-S",_tag_dyneither(_tmp3FB,sizeof(
char),3))));};}static void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=1;Cyc_Toc_warn_all_null_deref=
1;Cyc_NewControlFlow_warn_lose_unique=1;}enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,
Cyc_CYCLONEFILE  = 1};typedef enum Cyc_inputtype Cyc_inputtype_t;static enum Cyc_inputtype
Cyc_intype=Cyc_DEFAULTINPUT;static void Cyc_set_inputtype(struct _dyneither_ptr s){
const char*_tmp3FC;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3FC="cyc",
_tag_dyneither(_tmp3FC,sizeof(char),4))))== 0)Cyc_intype=Cyc_CYCLONEFILE;else{
const char*_tmp3FD;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3FD="none",
_tag_dyneither(_tmp3FD,sizeof(char),5))))== 0)Cyc_intype=Cyc_DEFAULTINPUT;else{
const char*_tmp401;void*_tmp400[1];struct Cyc_String_pa_struct _tmp3FF;(_tmp3FF.tag=
0,((_tmp3FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp400[0]=&
_tmp3FF,Cyc_fprintf(Cyc_stderr,((_tmp401="Input type '%s' not supported\n",
_tag_dyneither(_tmp401,sizeof(char),31))),_tag_dyneither(_tmp400,sizeof(void*),1)))))));}}}
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct
_dyneither_ptr*output_file){struct _dyneither_ptr _tmp43=Cyc_Filename_dirname(
output_file == 0?s:*output_file);struct _dyneither_ptr _tmp44=Cyc_Filename_chop_extension((
struct _dyneither_ptr)Cyc_Filename_basename(s));struct _dyneither_ptr _tmp45=Cyc_strlen((
struct _dyneither_ptr)_tmp43)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp43,(
struct _dyneither_ptr)_tmp44): _tmp44;return(struct _dyneither_ptr)_tmp45;}static
struct Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(struct
_dyneither_ptr s){if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){{
struct _dyneither_ptr*_tmp404;struct Cyc_List_List*_tmp403;Cyc_cyclone_files=((
_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403->hd=((_tmp404=_cycalloc(sizeof(*
_tmp404)),((_tmp404[0]=s,_tmp404)))),((_tmp403->tl=Cyc_cyclone_files,_tmp403))))));}{
struct _dyneither_ptr _tmp48=Cyc_make_base_filename(s,Cyc_output_file);const char*
_tmp405;struct _dyneither_ptr _tmp49=Cyc_strconcat((struct _dyneither_ptr)_tmp48,((
_tmp405=".c",_tag_dyneither(_tmp405,sizeof(char),3))));Cyc_add_ccarg((struct
_dyneither_ptr)_tmp49);};}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(
struct _dyneither_ptr s){if(Cyc_save_temps_r)return;else{remove((const char*)
_untag_dyneither_ptr(s,sizeof(char),1));}}struct _tuple20{struct _dyneither_ptr*f1;
struct _dyneither_ptr*f2;};struct Cyc_List_List*Cyc_read_specs(struct
_dyneither_ptr file){struct Cyc_List_List*_tmp4B=0;int c;int i;char strname[256];char
strvalue[4096];struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(file,sizeof(char),1),(const char*)"r");if(spec_file == 0){{
const char*_tmp409;void*_tmp408[1];struct Cyc_String_pa_struct _tmp407;(_tmp407.tag=
0,((_tmp407.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp408[0]=&
_tmp407,Cyc_fprintf(Cyc_stderr,((_tmp409="Error opening spec file %s\n",
_tag_dyneither(_tmp409,sizeof(char),28))),_tag_dyneither(_tmp408,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;}while(1){while(1){c=Cyc_fgetc((
struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;if(c == '*')break;if(c != -
1){{const char*_tmp40E;void*_tmp40D[2];struct Cyc_String_pa_struct _tmp40C;struct
Cyc_Int_pa_struct _tmp40B;(_tmp40B.tag=1,((_tmp40B.f1=(unsigned long)c,((_tmp40C.tag=
0,((_tmp40C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp40D[0]=&
_tmp40C,((_tmp40D[1]=& _tmp40B,Cyc_fprintf(Cyc_stderr,((_tmp40E="Error reading spec file %s: unexpected character '%c'\n",
_tag_dyneither(_tmp40E,sizeof(char),55))),_tag_dyneither(_tmp40D,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}goto CLEANUP_AND_RETURN;}
JUST_AFTER_STAR: i=0;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(c == 
- 1){{const char*_tmp412;void*_tmp411[1];struct Cyc_String_pa_struct _tmp410;(
_tmp410.tag=0,((_tmp410.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((
_tmp411[0]=& _tmp410,Cyc_fprintf(Cyc_stderr,((_tmp412="Error reading spec file %s: unexpected EOF\n",
_tag_dyneither(_tmp412,sizeof(char),44))),_tag_dyneither(_tmp411,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';break;}strname[
_check_known_subscript_notnull(256,i)]=(char)c;++ i;if(i >= 256){{const char*
_tmp416;void*_tmp415[1];struct Cyc_String_pa_struct _tmp414;(_tmp414.tag=0,((
_tmp414.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp415[0]=&
_tmp414,Cyc_fprintf(Cyc_stderr,((_tmp416="Error reading spec file %s: string name too long\n",
_tag_dyneither(_tmp416,sizeof(char),50))),_tag_dyneither(_tmp415,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}while(1){c=
Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;break;}if(c == '*'){{
struct _tuple20*_tmp423;struct _dyneither_ptr*_tmp422;const char*_tmp421;struct
_dyneither_ptr*_tmp420;struct Cyc_List_List*_tmp41F;_tmp4B=((_tmp41F=_cycalloc(
sizeof(*_tmp41F)),((_tmp41F->hd=((_tmp423=_cycalloc(sizeof(*_tmp423)),((_tmp423->f1=((
_tmp422=_cycalloc(sizeof(*_tmp422)),((_tmp422[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp422)))),((_tmp423->f2=((_tmp420=
_cycalloc(sizeof(*_tmp420)),((_tmp420[0]=(struct _dyneither_ptr)Cyc_strdup(((
_tmp421="",_tag_dyneither(_tmp421,sizeof(char),1)))),_tmp420)))),_tmp423)))))),((
_tmp41F->tl=_tmp4B,_tmp41F))))));}goto JUST_AFTER_STAR;}strvalue[0]=(char)c;i=1;
while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if((c == - 1  || c == '\n')
 || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;++ i;if(i >= 4096){{const
char*_tmp428;void*_tmp427[2];struct Cyc_String_pa_struct _tmp426;struct Cyc_String_pa_struct
_tmp425;(_tmp425.tag=0,((_tmp425.f1=(struct _dyneither_ptr)_tag_dyneither(strname,
sizeof(char),256),((_tmp426.tag=0,((_tmp426.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)file),((_tmp427[0]=& _tmp426,((_tmp427[1]=& _tmp425,Cyc_fprintf(Cyc_stderr,((
_tmp428="Error reading spec file %s: value of %s too long\n",_tag_dyneither(
_tmp428,sizeof(char),50))),_tag_dyneither(_tmp427,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}{struct
_tuple20*_tmp431;struct _dyneither_ptr*_tmp430;struct _dyneither_ptr*_tmp42F;
struct Cyc_List_List*_tmp42E;_tmp4B=((_tmp42E=_cycalloc(sizeof(*_tmp42E)),((
_tmp42E->hd=((_tmp431=_cycalloc(sizeof(*_tmp431)),((_tmp431->f1=((_tmp430=
_cycalloc(sizeof(*_tmp430)),((_tmp430[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp430)))),((_tmp431->f2=((_tmp42F=
_cycalloc(sizeof(*_tmp42F)),((_tmp42F[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strvalue,sizeof(char),4096)),_tmp42F)))),_tmp431)))))),((_tmp42E->tl=
_tmp4B,_tmp42E))))));}if(c == - 1)goto CLEANUP_AND_RETURN;}CLEANUP_AND_RETURN: Cyc_fclose((
struct Cyc___cycFILE*)spec_file);return _tmp4B;}struct _dyneither_ptr Cyc_split_specs(
struct _dyneither_ptr cmdline);static void _tmp43B(struct _dyneither_ptr*_tmp6C,
unsigned int*_tmp43A,unsigned int*_tmp439,struct _dyneither_ptr**_tmp437){for(*
_tmp43A=0;*_tmp43A < *_tmp439;(*_tmp43A)++){(*_tmp437)[*_tmp43A]=*((struct
_dyneither_ptr**)(*_tmp6C).curr)[(int)*_tmp43A];}}struct _dyneither_ptr Cyc_split_specs(
struct _dyneither_ptr cmdline){if((char*)cmdline.curr == (char*)(_tag_dyneither(0,0,
0)).curr)return _tag_dyneither(0,0,0);{unsigned long _tmp66=Cyc_strlen((struct
_dyneither_ptr)cmdline);struct Cyc_List_List*_tmp67=0;char buf[4096];int i=0;int j=0;
if(_tmp66 > 4096)goto DONE;while(1){while(1){if(i >= _tmp66)goto DONE;if(*((const
char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;if(!
isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))
break;++ i;}j=0;while(1){if(i >= _tmp66)break;if(*((const char*)
_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;if(isspace((int)*((
const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;if(*((
const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){++ i;if(i
>= _tmp66)break;if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),
i))== 0)break;buf[_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dyneither_subscript(cmdline,sizeof(char),i));++ j;}else{buf[
_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(
cmdline,sizeof(char),i));++ j;}++ i;}if(j < 4096)buf[_check_known_subscript_notnull(
4096,j)]='\000';{struct _dyneither_ptr*_tmp434;struct Cyc_List_List*_tmp433;_tmp67=((
_tmp433=_cycalloc(sizeof(*_tmp433)),((_tmp433->hd=((_tmp434=_cycalloc(sizeof(*
_tmp434)),((_tmp434[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,
sizeof(char),4096)),_tmp434)))),((_tmp433->tl=_tmp67,_tmp433))))));};}DONE:
_tmp67=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp67);{
struct Cyc_List_List*_tmp435;_tmp67=((_tmp435=_cycalloc(sizeof(*_tmp435)),((
_tmp435->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",
sizeof(char),1),((_tmp435->tl=_tmp67,_tmp435))))));}{struct _dyneither_ptr _tmp6C=((
struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp67);
unsigned int _tmp43A;unsigned int _tmp439;struct _dyneither_ptr _tmp438;struct
_dyneither_ptr*_tmp437;unsigned int _tmp436;struct _dyneither_ptr _tmp6D=(_tmp436=
_get_dyneither_size(_tmp6C,sizeof(struct _dyneither_ptr*)),((_tmp437=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp436)),((
_tmp438=_tag_dyneither(_tmp437,sizeof(struct _dyneither_ptr),_tmp436),((((_tmp439=
_tmp436,_tmp43B(& _tmp6C,& _tmp43A,& _tmp439,& _tmp437))),_tmp438)))))));return
_tmp6D;};};}int Cyc_compile_failure=0;struct Cyc___cycFILE*Cyc_try_file_open(
struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr
msg_part){struct _handler_cons _tmp72;_push_handler(& _tmp72);{int _tmp74=0;if(
setjmp(_tmp72.handler))_tmp74=1;if(!_tmp74){{struct Cyc___cycFILE*_tmp75=(struct
Cyc___cycFILE*)Cyc_file_open(filename,mode);_npop_handler(0);return _tmp75;};
_pop_handler();}else{void*_tmp73=(void*)_exn_thrown;void*_tmp77=_tmp73;_LL1:;
_LL2: Cyc_compile_failure=1;{const char*_tmp440;void*_tmp43F[2];struct Cyc_String_pa_struct
_tmp43E;struct Cyc_String_pa_struct _tmp43D;(_tmp43D.tag=0,((_tmp43D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp43E.tag=0,((_tmp43E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp43F[0]=& _tmp43E,((
_tmp43F[1]=& _tmp43D,Cyc_fprintf(Cyc_stderr,((_tmp440="\nError: couldn't open %s %s\n",
_tag_dyneither(_tmp440,sizeof(char),29))),_tag_dyneither(_tmp43F,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(
_tmp77);_LL0:;}};}struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr
stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct
Cyc_List_List*tds){const char*_tmp441;struct _dyneither_ptr exn_string=(_tmp441="",
_tag_dyneither(_tmp441,sizeof(char),1));const char*_tmp442;struct _dyneither_ptr
explain_string=(_tmp442="",_tag_dyneither(_tmp442,sizeof(char),1));int other_exn=
0;struct Cyc_Core_Impossible_struct _tmp448;const char*_tmp447;struct Cyc_Core_Impossible_struct*
_tmp446;void*ex=(void*)((_tmp446=_cycalloc(sizeof(*_tmp446)),((_tmp446[0]=((
_tmp448.tag=Cyc_Core_Impossible,((_tmp448.f1=((_tmp447="",_tag_dyneither(_tmp447,
sizeof(char),1))),_tmp448)))),_tmp446))));struct Cyc_List_List*_tmp7C=0;{struct
_handler_cons _tmp7D;_push_handler(& _tmp7D);{int _tmp7F=0;if(setjmp(_tmp7D.handler))
_tmp7F=1;if(!_tmp7F){_tmp7C=f(env,tds);;_pop_handler();}else{void*_tmp7E=(void*)
_exn_thrown;void*_tmp81=_tmp7E;struct _dyneither_ptr _tmp83;struct _dyneither_ptr
_tmp86;struct _dyneither_ptr _tmp88;_LL6: {struct Cyc_Core_Impossible_struct*_tmp82=(
struct Cyc_Core_Impossible_struct*)_tmp81;if(_tmp82->tag != Cyc_Core_Impossible)
goto _LL8;else{_tmp83=_tmp82->f1;}}_LL7:{const char*_tmp449;exn_string=((_tmp449="Exception Core::Impossible",
_tag_dyneither(_tmp449,sizeof(char),27)));}explain_string=_tmp83;goto _LL5;_LL8: {
struct Cyc_Dict_Absent_struct*_tmp84=(struct Cyc_Dict_Absent_struct*)_tmp81;if(
_tmp84->tag != Cyc_Dict_Absent)goto _LLA;}_LL9:{const char*_tmp44A;exn_string=((
_tmp44A="Exception Dict::Absent",_tag_dyneither(_tmp44A,sizeof(char),23)));}goto
_LL5;_LLA: {struct Cyc_Core_Invalid_argument_struct*_tmp85=(struct Cyc_Core_Invalid_argument_struct*)
_tmp81;if(_tmp85->tag != Cyc_Core_Invalid_argument)goto _LLC;else{_tmp86=_tmp85->f1;}}
_LLB:{const char*_tmp44B;exn_string=((_tmp44B="Exception Core::Invalid_argument",
_tag_dyneither(_tmp44B,sizeof(char),33)));}explain_string=_tmp86;goto _LL5;_LLC: {
struct Cyc_Core_Failure_struct*_tmp87=(struct Cyc_Core_Failure_struct*)_tmp81;if(
_tmp87->tag != Cyc_Core_Failure)goto _LLE;else{_tmp88=_tmp87->f1;}}_LLD:{const char*
_tmp44C;exn_string=((_tmp44C="Exception Core::Failure",_tag_dyneither(_tmp44C,
sizeof(char),24)));}explain_string=_tmp88;goto _LL5;_LLE:;_LLF: ex=_tmp81;
other_exn=1;{const char*_tmp44D;exn_string=((_tmp44D="Uncaught exception",
_tag_dyneither(_tmp44D,sizeof(char),19)));}goto _LL5;_LL10:;_LL11:(void)_throw(
_tmp81);_LL5:;}};}if(Cyc_Position_error_p())Cyc_compile_failure=1;{const char*
_tmp44E;if(Cyc_strcmp((struct _dyneither_ptr)exn_string,((_tmp44E="",
_tag_dyneither(_tmp44E,sizeof(char),1))))!= 0){Cyc_compile_failure=1;{const char*
_tmp454;void*_tmp453[3];struct Cyc_String_pa_struct _tmp452;struct Cyc_String_pa_struct
_tmp451;struct Cyc_String_pa_struct _tmp450;(_tmp450.tag=0,((_tmp450.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)explain_string),((_tmp451.tag=0,((_tmp451.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp452.tag=0,((
_tmp452.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)exn_string),((_tmp453[0]=&
_tmp452,((_tmp453[1]=& _tmp451,((_tmp453[2]=& _tmp450,Cyc_fprintf(Cyc_stderr,((
_tmp454="\n%s thrown during %s: %s",_tag_dyneither(_tmp454,sizeof(char),25))),
_tag_dyneither(_tmp453,sizeof(void*),3)))))))))))))))))));};}}if(Cyc_compile_failure){{
const char*_tmp457;void*_tmp456;(_tmp456=0,Cyc_fprintf(Cyc_stderr,((_tmp457="\nCOMPILATION FAILED!\n",
_tag_dyneither(_tmp457,sizeof(char),22))),_tag_dyneither(_tmp456,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);if(other_exn)(int)_throw(ex);return
_tmp7C;}else{if(Cyc_v_r){{const char*_tmp45B;void*_tmp45A[1];struct Cyc_String_pa_struct
_tmp459;(_tmp459.tag=0,((_tmp459.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
stage_name),((_tmp45A[0]=& _tmp459,Cyc_fprintf(Cyc_stderr,((_tmp45B="%s completed.\n",
_tag_dyneither(_tmp45B,sizeof(char),15))),_tag_dyneither(_tmp45A,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return _tmp7C;}}return _tmp7C;}struct
Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init(
1);{struct Cyc_List_List*_tmp9E=Cyc_Parse_parse_file(f);Cyc_Lex_lex_init(1);
return _tmp9E;};}struct _tuple21{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple21*tcenv,struct Cyc_List_List*
tds){struct _RegionHandle*_tmpA0;struct Cyc_Tcenv_Tenv*_tmpA1;struct _tuple21 _tmp9F=*
tcenv;_tmpA0=_tmp9F.f1;_tmpA1=_tmp9F.f2;Cyc_Tc_tc(_tmpA0,_tmpA1,1,tds);if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(_tmpA1,tds);return tds;}struct Cyc_List_List*Cyc_do_cfcheck(
int ignore,struct Cyc_List_List*tds){Cyc_NewControlFlow_cf_check(tds);return tds;}
struct _tuple22{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*
f3;};struct Cyc_List_List*Cyc_do_interface(struct _tuple22*params,struct Cyc_List_List*
tds){struct _tuple22 _tmpA3;struct Cyc_Tcenv_Tenv*_tmpA4;struct Cyc___cycFILE*_tmpA5;
struct Cyc___cycFILE*_tmpA6;struct _tuple22*_tmpA2=params;_tmpA3=*_tmpA2;_tmpA4=
_tmpA3.f1;_tmpA5=_tmpA3.f2;_tmpA6=_tmpA3.f3;{struct Cyc_Interface_I*_tmpA7=Cyc_Interface_extract(
_tmpA4->ae);if(_tmpA5 == 0)Cyc_Interface_save(_tmpA7,_tmpA6);else{struct Cyc_Interface_I*
_tmpA8=Cyc_Interface_parse((struct Cyc___cycFILE*)_tmpA5);const char*_tmp460;const
char*_tmp45F;struct _tuple19*_tmp45E;if(!Cyc_Interface_is_subinterface(_tmpA8,
_tmpA7,((_tmp45E=_cycalloc(sizeof(*_tmp45E)),((_tmp45E->f1=((_tmp45F="written interface",
_tag_dyneither(_tmp45F,sizeof(char),18))),((_tmp45E->f2=((_tmp460="maximal interface",
_tag_dyneither(_tmp460,sizeof(char),18))),_tmp45E))))))))Cyc_compile_failure=1;
else{Cyc_Interface_save(_tmpA8,_tmpA6);}}return tds;};}struct Cyc_List_List*Cyc_do_translate(
int ignore,struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*
Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){Cyc_Tovc_elim_array_initializers=
Cyc_Cyclone_tovc_r;return Cyc_Tovc_tovc(tds);}static struct _dyneither_ptr Cyc_cyc_setjmp=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};static struct
_dyneither_ptr Cyc_cyc_include=(struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0
+ 0)};static void Cyc_set_cyc_include(struct _dyneither_ptr f){Cyc_cyc_include=f;}
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr){{const char*_tmp463;
void*_tmp462;(_tmp462=0,Cyc_fprintf(Cyc_stderr,((_tmp463="Internal error: copy_internal_file called with NULL\n",
_tag_dyneither(_tmp463,sizeof(char),53))),_tag_dyneither(_tmp462,sizeof(void*),0)));}
return 1;}{const char*_tmp465;const char*_tmp464;struct Cyc___cycFILE*_tmpAE=Cyc_try_file_open(
file,((_tmp464="r",_tag_dyneither(_tmp464,sizeof(char),2))),((_tmp465="internal compiler file",
_tag_dyneither(_tmp465,sizeof(char),23))));if(_tmpAE == 0)return 1;{unsigned long
n_read=1024;unsigned long n_written;char buf[1024];{unsigned int _tmpB5=1024;
unsigned int i;for(i=0;i < _tmpB5;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=
Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)
_tmpAE);if(n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmpAE)){Cyc_compile_failure=
1;{const char*_tmp469;void*_tmp468[1];struct Cyc_String_pa_struct _tmp467;(_tmp467.tag=
0,((_tmp467.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp468[0]=&
_tmp467,Cyc_fprintf(Cyc_stderr,((_tmp469="\nError: problem copying %s\n",
_tag_dyneither(_tmp469,sizeof(char),28))),_tag_dyneither(_tmp468,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}n_written=Cyc_fwrite(
_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);if(n_read != n_written){
Cyc_compile_failure=1;{const char*_tmp46D;void*_tmp46C[1];struct Cyc_String_pa_struct
_tmp46B;(_tmp46B.tag=0,((_tmp46B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
file),((_tmp46C[0]=& _tmp46B,Cyc_fprintf(Cyc_stderr,((_tmp46D="\nError: problem copying %s\n",
_tag_dyneither(_tmp46D,sizeof(char),28))),_tag_dyneither(_tmp46C,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}}Cyc_fclose((struct Cyc___cycFILE*)
_tmpAE);return 0;};};}struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*
out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params _tmpB8=Cyc_tc_r?Cyc_Absynpp_cyc_params_r:
Cyc_Absynpp_c_params_r;_tmpB8.expand_typedefs=!Cyc_noexpand_r;_tmpB8.to_VC=Cyc_Cyclone_tovc_r;
_tmpB8.add_cyc_prefix=Cyc_add_cyc_namespace_r;_tmpB8.generate_line_directives=
Cyc_generate_line_directives_r;_tmpB8.print_full_evars=Cyc_print_full_evars_r;
_tmpB8.print_all_tvars=Cyc_print_all_tvars_r;_tmpB8.print_all_kinds=Cyc_print_all_kinds_r;
_tmpB8.print_all_effects=Cyc_print_all_effects_r;if(Cyc_inline_functions_r){
const char*_tmp470;void*_tmp46F;(_tmp46F=0,Cyc_fprintf(out_file,((_tmp470="#define _INLINE_FUNCTIONS\n",
_tag_dyneither(_tmp470,sizeof(char),27))),_tag_dyneither(_tmp46F,sizeof(void*),0)));}
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){if(!Cyc_nocyc_setjmp_r){if(Cyc_compile_for_boot_r){
const char*_tmp473;void*_tmp472;(_tmp472=0,Cyc_fprintf(out_file,((_tmp473="#include <setjmp.h>\n",
_tag_dyneither(_tmp473,sizeof(char),21))),_tag_dyneither(_tmp472,sizeof(void*),0)));}
else{if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}if(Cyc_copy_internal_file(
Cyc_cyc_include,out_file))return tds;}if(Cyc_pp_r){Cyc_Absynpp_set_params(& _tmpB8);
Cyc_Absynpp_decllist2file(tds,out_file);}else{Cyc_Absyndump_set_params(& _tmpB8);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}Cyc_fflush((struct Cyc___cycFILE*)
out_file);return tds;}static struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)
_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((struct _dyneither_ptr*)((struct
Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}static struct Cyc_List_List*Cyc_split_by_char(
struct _dyneither_ptr s,char c){if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)
return 0;{struct Cyc_List_List*_tmpBD=0;unsigned long _tmpBE=Cyc_strlen((struct
_dyneither_ptr)s);while(_tmpBE > 0){struct _dyneither_ptr _tmpBF=Cyc_strchr(s,c);
if((char*)_tmpBF.curr == (char*)(_tag_dyneither(0,0,0)).curr){{struct
_dyneither_ptr*_tmp476;struct Cyc_List_List*_tmp475;_tmpBD=((_tmp475=_cycalloc(
sizeof(*_tmp475)),((_tmp475->hd=((_tmp476=_cycalloc(sizeof(*_tmp476)),((_tmp476[
0]=s,_tmp476)))),((_tmp475->tl=_tmpBD,_tmp475))))));}break;}else{{struct
_dyneither_ptr*_tmp479;struct Cyc_List_List*_tmp478;_tmpBD=((_tmp478=_cycalloc(
sizeof(*_tmp478)),((_tmp478->hd=((_tmp479=_cycalloc(sizeof(*_tmp479)),((_tmp479[
0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((
struct _dyneither_ptr)_tmpBF).curr - s.curr)/ sizeof(char))),_tmp479)))),((_tmp478->tl=
_tmpBD,_tmp478))))));}_tmpBE -=(((struct _dyneither_ptr)_tmpBF).curr - s.curr)/ 
sizeof(char);s=_dyneither_ptr_plus(_tmpBF,sizeof(char),1);}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBD);};}static int Cyc_file_exists(
struct _dyneither_ptr file){struct Cyc___cycFILE*f=0;{struct _handler_cons _tmpC4;
_push_handler(& _tmpC4);{int _tmpC6=0;if(setjmp(_tmpC4.handler))_tmpC6=1;if(!
_tmpC6){{const char*_tmp47A;f=(struct Cyc___cycFILE*)Cyc_file_open(file,((_tmp47A="r",
_tag_dyneither(_tmp47A,sizeof(char),2))));};_pop_handler();}else{void*_tmpC5=(
void*)_exn_thrown;void*_tmpC9=_tmpC5;_LL13:;_LL14: goto _LL12;_LL15:;_LL16:(void)
_throw(_tmpC9);_LL12:;}};}if(f == 0)return 0;else{Cyc_fclose((struct Cyc___cycFILE*)
f);return 1;}}static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
const char*_tmp47B;struct _dyneither_ptr tmp=(_tmp47B="",_tag_dyneither(_tmp47B,
sizeof(char),1));for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr _tmpCA=*((
struct _dyneither_ptr*)dirs->hd);if((char*)_tmpCA.curr == (char*)(_tag_dyneither(0,
0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpCA)== 0)continue;{const char*
_tmp47C;_tmpCA=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpCA,((
_tmp47C=":",_tag_dyneither(_tmp47C,sizeof(char),2))));}tmp=(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)_tmpCA,(struct _dyneither_ptr)tmp);}return tmp;}
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct
_dyneither_ptr file){if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr)
return 0;for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr _tmpCD=*((struct
_dyneither_ptr*)dirs->hd);if((char*)_tmpCD.curr == (char*)(_tag_dyneither(0,0,0)).curr
 || Cyc_strlen((struct _dyneither_ptr)_tmpCD)== 0)continue;{struct _dyneither_ptr s=(
struct _dyneither_ptr)Cyc_Filename_concat(_tmpCD,file);if(Cyc_file_exists(s)){
struct _dyneither_ptr*_tmp47D;return(_tmp47D=_cycalloc(sizeof(*_tmp47D)),((
_tmp47D[0]=s,_tmp47D)));}};}return 0;}static struct _dyneither_ptr Cyc_do_find(
struct Cyc_List_List*dirs,struct _dyneither_ptr file){struct _dyneither_ptr*_tmpCF=
Cyc_find(dirs,file);if(_tmpCF == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{
const char*_tmp482;void*_tmp481[2];struct Cyc_String_pa_struct _tmp480;struct Cyc_String_pa_struct
_tmp47F;(_tmp47F.tag=0,((_tmp47F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs)),((_tmp480.tag=0,((_tmp480.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)file),((_tmp481[0]=& _tmp480,((_tmp481[1]=& _tmp47F,Cyc_fprintf(
Cyc_stderr,((_tmp482="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp482,sizeof(char),56))),_tag_dyneither(_tmp481,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Failure_struct
_tmp492;const char*_tmp491;void*_tmp490[2];struct Cyc_String_pa_struct _tmp48F;
struct Cyc_String_pa_struct _tmp48E;struct Cyc_Core_Failure_struct*_tmp48D;(int)
_throw((void*)((_tmp48D=_cycalloc(sizeof(*_tmp48D)),((_tmp48D[0]=((_tmp492.tag=
Cyc_Core_Failure,((_tmp492.f1=(struct _dyneither_ptr)((_tmp48E.tag=0,((_tmp48E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp48F.tag=
0,((_tmp48F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp490[0]=&
_tmp48F,((_tmp490[1]=& _tmp48E,Cyc_aprintf(((_tmp491="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp491,sizeof(char),56))),_tag_dyneither(_tmp490,sizeof(void*),2)))))))))))))),
_tmp492)))),_tmp48D)))));};}return*_tmpCF;}static struct Cyc_List_List*Cyc_add_subdir(
struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*_tmpDA=
0;for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr*_tmp495;struct Cyc_List_List*
_tmp494;_tmpDA=((_tmp494=_cycalloc(sizeof(*_tmp494)),((_tmp494->hd=((_tmp495=
_cycalloc(sizeof(*_tmp495)),((_tmp495[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((
struct _dyneither_ptr*)dirs->hd),subdir),_tmp495)))),((_tmp494->tl=_tmpDA,_tmp494))))));}
_tmpDA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDA);
return _tmpDA;}static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,
struct _dyneither_ptr subdir){struct Cyc_List_List*_tmpDD=0;for(0;dirs != 0;dirs=
dirs->tl){{struct Cyc_List_List*_tmp496;_tmpDD=((_tmp496=_cycalloc(sizeof(*
_tmp496)),((_tmp496->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp496->tl=_tmpDD,
_tmp496))))));}{struct _dyneither_ptr*_tmp499;struct Cyc_List_List*_tmp498;_tmpDD=((
_tmp498=_cycalloc(sizeof(*_tmp498)),((_tmp498->hd=((_tmp499=_cycalloc(sizeof(*
_tmp499)),((_tmp499[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct
_dyneither_ptr*)dirs->hd),subdir),_tmp499)))),((_tmp498->tl=_tmpDD,_tmp498))))));};}
_tmpDD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDD);
return _tmpDD;}static int Cyc_is_other_special(char c){switch(c){case '\\': _LL17: goto
_LL18;case '"': _LL18: goto _LL19;case ';': _LL19: goto _LL1A;case '&': _LL1A: goto _LL1B;
case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto _LL1D;case '|': _LL1D: goto _LL1E;case '^':
_LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;case '>': _LL20: goto _LL21;case '\n': _LL21:
goto _LL22;case '\t': _LL22: return 1;default: _LL23: return 0;}}static struct
_dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp4A2(
unsigned int*_tmp4A1,unsigned int*_tmp4A0,char**_tmp49E){for(*_tmp4A1=0;*_tmp4A1
< *_tmp4A0;(*_tmp4A1)++){(*_tmp49E)[*_tmp4A1]='\000';}}static struct
_dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){unsigned long _tmpE1=
Cyc_strlen((struct _dyneither_ptr)s);int _tmpE2=0;int _tmpE3=0;{int i=0;for(0;i < 
_tmpE1;++ i){char _tmpE4=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpE4 == '\'')++ _tmpE2;else{if(Cyc_is_other_special(_tmpE4))++ _tmpE3;}}}if(
_tmpE2 == 0  && _tmpE3 == 0)return s;if(_tmpE2 == 0){struct _dyneither_ptr*_tmp49C;
struct _dyneither_ptr*_tmp49B[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((
_tmp49B[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",
sizeof(char),2),((_tmp49B[1]=((_tmp49C=_cycalloc(sizeof(*_tmp49C)),((_tmp49C[0]=(
struct _dyneither_ptr)s,_tmp49C)))),((_tmp49B[0]=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp49B,sizeof(struct
_dyneither_ptr*),3)))))))));}{unsigned long _tmpE9=(_tmpE1 + _tmpE2)+ _tmpE3;
unsigned int _tmp4A1;unsigned int _tmp4A0;struct _dyneither_ptr _tmp49F;char*_tmp49E;
unsigned int _tmp49D;struct _dyneither_ptr s2=(_tmp49D=_tmpE9 + 1,((_tmp49E=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp49D + 1)),((_tmp49F=_tag_dyneither(
_tmp49E,sizeof(char),_tmp49D + 1),((((_tmp4A0=_tmp49D,((_tmp4A2(& _tmp4A1,& _tmp4A0,&
_tmp49E),_tmp49E[_tmp4A0]=(char)0)))),_tmp49F)))))));int _tmpEA=0;int _tmpEB=0;
for(0;_tmpEA < _tmpE1;++ _tmpEA){char _tmpEC=*((const char*)
_check_dyneither_subscript(s,sizeof(char),_tmpEA));if(_tmpEC == '\''  || Cyc_is_other_special(
_tmpEC)){char _tmp4A5;char _tmp4A4;struct _dyneither_ptr _tmp4A3;(_tmp4A3=
_dyneither_ptr_plus(s2,sizeof(char),_tmpEB ++),((_tmp4A4=*((char*)
_check_dyneither_subscript(_tmp4A3,sizeof(char),0)),((_tmp4A5='\\',((
_get_dyneither_size(_tmp4A3,sizeof(char))== 1  && (_tmp4A4 == '\000'  && _tmp4A5 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp4A3.curr)=_tmp4A5)))))));}{char _tmp4A8;char
_tmp4A7;struct _dyneither_ptr _tmp4A6;(_tmp4A6=_dyneither_ptr_plus(s2,sizeof(char),
_tmpEB ++),((_tmp4A7=*((char*)_check_dyneither_subscript(_tmp4A6,sizeof(char),0)),((
_tmp4A8=_tmpEC,((_get_dyneither_size(_tmp4A6,sizeof(char))== 1  && (_tmp4A7 == '\000'
 && _tmp4A8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4A6.curr)=_tmp4A8)))))));};}
return(struct _dyneither_ptr)s2;};}static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp){struct _dyneither_ptr*_tmp4A9;return(_tmp4A9=_cycalloc(
sizeof(*_tmp4A9)),((_tmp4A9[0]=Cyc_sh_escape_string(*sp),_tmp4A9)));}static void
Cyc_process_file(struct _dyneither_ptr filename){struct _dyneither_ptr _tmpF8=Cyc_make_base_filename(
filename,Cyc_output_file);const char*_tmp4AA;struct _dyneither_ptr _tmpF9=Cyc_strconcat((
struct _dyneither_ptr)_tmpF8,((_tmp4AA=".cyp",_tag_dyneither(_tmp4AA,sizeof(char),
5))));const char*_tmp4AB;struct _dyneither_ptr _tmpFA=Cyc_strconcat((struct
_dyneither_ptr)_tmpF8,((_tmp4AB=".cyci",_tag_dyneither(_tmp4AB,sizeof(char),6))));
const char*_tmp4AC;struct _dyneither_ptr _tmpFB=Cyc_strconcat((struct _dyneither_ptr)
_tmpF8,((_tmp4AC=".cycio",_tag_dyneither(_tmp4AC,sizeof(char),7))));const char*
_tmp4AD;struct _dyneither_ptr _tmpFC=Cyc_strconcat((struct _dyneither_ptr)_tmpF8,((
_tmp4AD=".c",_tag_dyneither(_tmp4AD,sizeof(char),3))));if(Cyc_v_r){const char*
_tmp4B1;void*_tmp4B0[1];struct Cyc_String_pa_struct _tmp4AF;(_tmp4AF.tag=0,((
_tmp4AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp4B0[0]=&
_tmp4AF,Cyc_fprintf(Cyc_stderr,((_tmp4B1="Compiling %s\n",_tag_dyneither(_tmp4B1,
sizeof(char),14))),_tag_dyneither(_tmp4B0,sizeof(void*),1)))))));}{const char*
_tmp4B3;const char*_tmp4B2;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((
_tmp4B2="r",_tag_dyneither(_tmp4B2,sizeof(char),2))),((_tmp4B3="input file",
_tag_dyneither(_tmp4B3,sizeof(char),11))));if(Cyc_compile_failure  || !((
unsigned int)f0))return;Cyc_fclose((struct Cyc___cycFILE*)f0);{const char*_tmp4BA;
struct _dyneither_ptr*_tmp4B9;const char*_tmp4B8;struct Cyc_List_List*_tmp4B7;
struct _dyneither_ptr _tmp100=Cyc_str_sepstr(((_tmp4B7=_cycalloc(sizeof(*_tmp4B7)),((
_tmp4B7->hd=((_tmp4B9=_cycalloc(sizeof(*_tmp4B9)),((_tmp4B9[0]=(struct
_dyneither_ptr)((_tmp4B8="",_tag_dyneither(_tmp4B8,sizeof(char),1))),_tmp4B9)))),((
_tmp4B7->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),
_tmp4B7)))))),((_tmp4BA=" ",_tag_dyneither(_tmp4BA,sizeof(char),2))));struct
_dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmp101=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
const char*_tmp4BB;_tmp101=Cyc_add_subdir(_tmp101,((_tmp4BB="include",
_tag_dyneither(_tmp4BB,sizeof(char),8))));}if(Cyc_strlen((struct _dyneither_ptr)
def_inc_path)> 0){struct _dyneither_ptr*_tmp4BE;struct Cyc_List_List*_tmp4BD;
_tmp101=((_tmp4BD=_cycalloc(sizeof(*_tmp4BD)),((_tmp4BD->hd=((_tmp4BE=_cycalloc(
sizeof(*_tmp4BE)),((_tmp4BE[0]=def_inc_path,_tmp4BE)))),((_tmp4BD->tl=_tmp101,
_tmp4BD))))));}{char*_tmp105=getenv((const char*)"CYCLONE_INCLUDE_PATH");if((char*)
_tmp105 != 0){char*_tmp4BF;_tmp101=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp4BF=_tmp105,
_tag_dyneither(_tmp4BF,sizeof(char),_get_zero_arr_size_char((void*)_tmp4BF,1)))),':'),
_tmp101);}{const char*_tmp4C6;struct _dyneither_ptr*_tmp4C5;const char*_tmp4C4;
struct Cyc_List_List*_tmp4C3;struct _dyneither_ptr stdinc_string=(struct
_dyneither_ptr)Cyc_str_sepstr(((_tmp4C3=_cycalloc(sizeof(*_tmp4C3)),((_tmp4C3->hd=((
_tmp4C5=_cycalloc(sizeof(*_tmp4C5)),((_tmp4C5[0]=(struct _dyneither_ptr)((_tmp4C4="",
_tag_dyneither(_tmp4C4,sizeof(char),1))),_tmp4C5)))),((_tmp4C3->tl=((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmp101),_tmp4C3)))))),((_tmp4C6=" -I",_tag_dyneither(
_tmp4C6,sizeof(char),4))));struct _dyneither_ptr ofile_string;if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){const char*_tmp4CA;void*_tmp4C9[1];struct Cyc_String_pa_struct
_tmp4C8;ofile_string=(struct _dyneither_ptr)((_tmp4C8.tag=0,((_tmp4C8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((
_tmp4C9[0]=& _tmp4C8,Cyc_aprintf(((_tmp4CA=" > %s",_tag_dyneither(_tmp4CA,sizeof(
char),6))),_tag_dyneither(_tmp4C9,sizeof(void*),1))))))));}else{const char*
_tmp4CB;ofile_string=((_tmp4CB="",_tag_dyneither(_tmp4CB,sizeof(char),1)));}}
else{const char*_tmp4CF;void*_tmp4CE[1];struct Cyc_String_pa_struct _tmp4CD;
ofile_string=(struct _dyneither_ptr)((_tmp4CD.tag=0,((_tmp4CD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)
_tmpF9)),((_tmp4CE[0]=& _tmp4CD,Cyc_aprintf(((_tmp4CF=" > %s",_tag_dyneither(
_tmp4CF,sizeof(char),6))),_tag_dyneither(_tmp4CE,sizeof(void*),1))))))));}{
struct _dyneither_ptr fixup_string;if(Cyc_produce_dependencies){if(Cyc_dependencies_target
== 0){const char*_tmp4D0;fixup_string=((_tmp4D0=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
_tag_dyneither(_tmp4D0,sizeof(char),35)));}else{const char*_tmp4D4;void*_tmp4D3[1];
struct Cyc_String_pa_struct _tmp4D2;fixup_string=(struct _dyneither_ptr)((_tmp4D2.tag=
0,((_tmp4D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4D3[0]=& _tmp4D2,Cyc_aprintf(((
_tmp4D4=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4D4,sizeof(char),29))),
_tag_dyneither(_tmp4D3,sizeof(void*),1))))))));}}else{const char*_tmp4D5;
fixup_string=((_tmp4D5="",_tag_dyneither(_tmp4D5,sizeof(char),1)));}{const char*
_tmp4DE;void*_tmp4DD[6];struct Cyc_String_pa_struct _tmp4DC;struct Cyc_String_pa_struct
_tmp4DB;struct Cyc_String_pa_struct _tmp4DA;struct Cyc_String_pa_struct _tmp4D9;
struct Cyc_String_pa_struct _tmp4D8;struct Cyc_String_pa_struct _tmp4D7;struct
_dyneither_ptr _tmp113=(_tmp4D7.tag=0,((_tmp4D7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ofile_string),((_tmp4D8.tag=0,((_tmp4D8.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)fixup_string),((_tmp4D9.tag=0,((_tmp4D9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(filename)),((_tmp4DA.tag=
0,((_tmp4DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((
_tmp4DB.tag=0,((_tmp4DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp100),((
_tmp4DC.tag=0,((_tmp4DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((
_tmp4DD[0]=& _tmp4DC,((_tmp4DD[1]=& _tmp4DB,((_tmp4DD[2]=& _tmp4DA,((_tmp4DD[3]=&
_tmp4D9,((_tmp4DD[4]=& _tmp4D8,((_tmp4DD[5]=& _tmp4D7,Cyc_aprintf(((_tmp4DE="%s %s%s %s%s%s",
_tag_dyneither(_tmp4DE,sizeof(char),15))),_tag_dyneither(_tmp4DD,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
if(Cyc_v_r){const char*_tmp4E2;void*_tmp4E1[1];struct Cyc_String_pa_struct _tmp4E0;(
_tmp4E0.tag=0,((_tmp4E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp113),((
_tmp4E1[0]=& _tmp4E0,Cyc_fprintf(Cyc_stderr,((_tmp4E2="%s\n",_tag_dyneither(
_tmp4E2,sizeof(char),4))),_tag_dyneither(_tmp4E1,sizeof(void*),1)))))));}if(
system((const char*)_untag_dyneither_ptr(_tmp113,sizeof(char),1))!= 0){Cyc_compile_failure=
1;{const char*_tmp4E5;void*_tmp4E4;(_tmp4E4=0,Cyc_fprintf(Cyc_stderr,((_tmp4E5="\nError: preprocessing\n",
_tag_dyneither(_tmp4E5,sizeof(char),23))),_tag_dyneither(_tmp4E4,sizeof(void*),0)));}
return;}if(Cyc_stop_after_cpp_r)return;Cyc_Position_reset_position((struct
_dyneither_ptr)_tmpF9);{const char*_tmp4E7;const char*_tmp4E6;struct Cyc___cycFILE*
in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF9,((_tmp4E6="r",
_tag_dyneither(_tmp4E6,sizeof(char),2))),((_tmp4E7="file",_tag_dyneither(_tmp4E7,
sizeof(char),5))));if(Cyc_compile_failure)return;{struct Cyc_List_List*tds=0;{
struct _handler_cons _tmp119;_push_handler(& _tmp119);{int _tmp11B=0;if(setjmp(
_tmp119.handler))_tmp11B=1;if(!_tmp11B){{const char*_tmp4E8;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4E8="parsing",_tag_dyneither(_tmp4E8,sizeof(char),8))),Cyc_do_parse,(struct
Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp11A=(
void*)_exn_thrown;void*_tmp11E=_tmp11A;_LL26:;_LL27: Cyc_file_close((struct Cyc___cycFILE*)
_check_null(in_file));Cyc_remove_file((struct _dyneither_ptr)_tmpF9);(int)_throw(
_tmp11E);_LL28:;_LL29:(void)_throw(_tmp11E);_LL25:;}};}Cyc_file_close((struct Cyc___cycFILE*)
in_file);if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
return;}if(Cyc_Absyn_porting_c_code){Cyc_Port_port(tds);return;}{struct
_RegionHandle _tmp11F=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp11F;
_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp120=Cyc_Tcenv_tc_init(tc_rgn);if(
Cyc_parseonly_r)goto PRINTC;{struct _tuple21 _tmp4E9;struct _tuple21 _tmp121=(_tmp4E9.f1=
tc_rgn,((_tmp4E9.f2=_tmp120,_tmp4E9)));{const char*_tmp4EA;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple21*,struct
Cyc_List_List*),struct _tuple21*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4EA="type checking",_tag_dyneither(_tmp4EA,sizeof(char),14))),Cyc_do_typecheck,&
_tmp121,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)
_tmpF9);_npop_handler(0);return;}if(Cyc_tc_r)goto PRINTC;{const char*_tmp4EB;tds=((
struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(
int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4EB="control-flow checking",_tag_dyneither(_tmp4EB,sizeof(char),22))),Cyc_do_cfcheck,
1,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
_npop_handler(0);return;}if(Cyc_ic_r){struct Cyc___cycFILE*inter_file=Cyc_fopen((
const char*)((char*)_untag_dyneither_ptr(_tmpFA,sizeof(char),1)),(const char*)"r");
const char*_tmp4ED;const char*_tmp4EC;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((
struct _dyneither_ptr)_tmpFB,((_tmp4EC="w",_tag_dyneither(_tmp4EC,sizeof(char),2))),((
_tmp4ED="interface object file",_tag_dyneither(_tmp4ED,sizeof(char),22))));if(
inter_objfile == 0){Cyc_compile_failure=1;_npop_handler(0);return;}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmpFA);{struct _tuple22 _tmp4EE;struct _tuple22 _tmp124=(
_tmp4EE.f1=_tmp120,((_tmp4EE.f2=inter_file,((_tmp4EE.f3=(struct Cyc___cycFILE*)
inter_objfile,_tmp4EE)))));{const char*_tmp4EF;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple22*,struct
Cyc_List_List*),struct _tuple22*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4EF="interface checking",_tag_dyneither(_tmp4EF,sizeof(char),19))),Cyc_do_interface,&
_tmp124,tds);}if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)inter_file);
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);};}};}if(Cyc_cf_r)goto PRINTC;{
const char*_tmp4F0;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,
struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(((_tmp4F0="translation to C",_tag_dyneither(_tmp4F0,sizeof(
char),17))),Cyc_do_translate,1,tds);}if(Cyc_compile_failure){Cyc_remove_file((
struct _dyneither_ptr)_tmpF9);_npop_handler(0);return;}if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct _dyneither_ptr)_tmpF9);if(Cyc_compile_failure){
_npop_handler(0);return;}if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){const char*
_tmp4F1;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4F1="post-pass to VC",_tag_dyneither(_tmp4F1,sizeof(char),16))),Cyc_do_tovc,
1,tds);}if(Cyc_compile_failure){_npop_handler(0);return;}PRINTC: {struct Cyc___cycFILE*
out_file;if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){if(Cyc_output_file != 0){
const char*_tmp4F3;const char*_tmp4F2;out_file=Cyc_try_file_open(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4F2="w",_tag_dyneither(
_tmp4F2,sizeof(char),2))),((_tmp4F3="output file",_tag_dyneither(_tmp4F3,sizeof(
char),12))));}else{out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}else{if(Cyc_toc_r
 && Cyc_output_file != 0){const char*_tmp4F5;const char*_tmp4F4;out_file=Cyc_try_file_open(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4F4="w",_tag_dyneither(
_tmp4F4,sizeof(char),2))),((_tmp4F5="output file",_tag_dyneither(_tmp4F5,sizeof(
char),12))));}else{const char*_tmp4F7;const char*_tmp4F6;out_file=Cyc_try_file_open((
struct _dyneither_ptr)_tmpFC,((_tmp4F6="w",_tag_dyneither(_tmp4F6,sizeof(char),2))),((
_tmp4F7="output file",_tag_dyneither(_tmp4F7,sizeof(char),12))));}}if(Cyc_compile_failure
 || !((unsigned int)out_file)){_npop_handler(0);return;}{struct _handler_cons
_tmp132;_push_handler(& _tmp132);{int _tmp134=0;if(setjmp(_tmp132.handler))_tmp134=
1;if(!_tmp134){if(!Cyc_noprint_r){const char*_tmp4F8;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp4F8="printing",_tag_dyneither(_tmp4F8,sizeof(char),9))),Cyc_do_print,(struct
Cyc___cycFILE*)out_file,tds);};_pop_handler();}else{void*_tmp133=(void*)
_exn_thrown;void*_tmp137=_tmp133;_LL2B:;_LL2C: Cyc_compile_failure=1;Cyc_file_close((
struct Cyc___cycFILE*)out_file);{struct _dyneither_ptr*_tmp4FB;struct Cyc_List_List*
_tmp4FA;Cyc_cfiles=((_tmp4FA=_cycalloc(sizeof(*_tmp4FA)),((_tmp4FA->hd=((_tmp4FB=
_cycalloc(sizeof(*_tmp4FB)),((_tmp4FB[0]=(struct _dyneither_ptr)_tmpFC,_tmp4FB)))),((
_tmp4FA->tl=Cyc_cfiles,_tmp4FA))))));}(int)_throw(_tmp137);_LL2D:;_LL2E:(void)
_throw(_tmp137);_LL2A:;}};}Cyc_file_close((struct Cyc___cycFILE*)out_file);{
struct _dyneither_ptr*_tmp4FE;struct Cyc_List_List*_tmp4FD;Cyc_cfiles=((_tmp4FD=
_cycalloc(sizeof(*_tmp4FD)),((_tmp4FD->hd=((_tmp4FE=_cycalloc(sizeof(*_tmp4FE)),((
_tmp4FE[0]=(struct _dyneither_ptr)_tmpFC,_tmp4FE)))),((_tmp4FD->tl=Cyc_cfiles,
_tmp4FD))))));};};_pop_region(tc_rgn);};};};};};};};};};}static char _tmp154[8]="<final>";
static struct _dyneither_ptr Cyc_final_str={_tmp154,_tmp154,_tmp154 + 8};static
struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;static struct Cyc_Interface_I*
Cyc_read_cycio(struct _dyneither_ptr*n){if(n == (struct _dyneither_ptr*)Cyc_final_strptr)
return Cyc_Interface_final();{struct _dyneither_ptr basename;{struct _handler_cons
_tmp155;_push_handler(& _tmp155);{int _tmp157=0;if(setjmp(_tmp155.handler))_tmp157=
1;if(!_tmp157){basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;
_pop_handler();}else{void*_tmp156=(void*)_exn_thrown;void*_tmp159=_tmp156;_LL30: {
struct Cyc_Core_Invalid_argument_struct*_tmp15A=(struct Cyc_Core_Invalid_argument_struct*)
_tmp159;if(_tmp15A->tag != Cyc_Core_Invalid_argument)goto _LL32;}_LL31: basename=*n;
goto _LL2F;_LL32:;_LL33:(void)_throw(_tmp159);_LL2F:;}};}{const char*_tmp4FF;
struct _dyneither_ptr _tmp15B=Cyc_strconcat((struct _dyneither_ptr)basename,((
_tmp4FF=".cycio",_tag_dyneither(_tmp4FF,sizeof(char),7))));const char*_tmp501;
const char*_tmp500;struct Cyc___cycFILE*_tmp15C=Cyc_try_file_open((struct
_dyneither_ptr)_tmp15B,((_tmp500="rb",_tag_dyneither(_tmp500,sizeof(char),3))),((
_tmp501="interface object file",_tag_dyneither(_tmp501,sizeof(char),22))));if(
_tmp15C == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();exit(1);}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmp15B);{struct Cyc_Interface_I*_tmp15D=Cyc_Interface_load((
struct Cyc___cycFILE*)_tmp15C);Cyc_file_close((struct Cyc___cycFILE*)_tmp15C);
return _tmp15D;};};};}static int Cyc_is_cfile(struct _dyneither_ptr*n){return*((
const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}void
GC_blacklist_warn_clear();struct _tuple23{struct _dyneither_ptr f1;int f2;struct
_dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct
_dyneither_ptr argv);static void _tmp507(unsigned int*_tmp506,unsigned int*_tmp505,
int**_tmp503){for(*_tmp506=0;*_tmp506 < *_tmp505;(*_tmp506)++){(*_tmp503)[*
_tmp506]=0;}}static void _tmp50F(unsigned int*_tmp50E,unsigned int*_tmp50D,struct
_dyneither_ptr**_tmp50B){for(*_tmp50E=0;*_tmp50E < *_tmp50D;(*_tmp50E)++){(*
_tmp50B)[*_tmp50E]=(struct _dyneither_ptr)_tag_dyneither(0,0,0);}}static void
_tmp515(unsigned int*_tmp514,unsigned int*_tmp513,struct _dyneither_ptr**_tmp511){
for(*_tmp514=0;*_tmp514 < *_tmp513;(*_tmp514)++){(*_tmp511)[*_tmp514]=(struct
_dyneither_ptr)_tag_dyneither(0,0,0);}}int Cyc_main(int argc,struct _dyneither_ptr
argv){GC_blacklist_warn_clear();{struct Cyc_List_List*cyclone_arch_path;struct
_dyneither_ptr def_lib_path;struct _dyneither_ptr comp=(struct _dyneither_ptr)
Cstring_to_string(Ccomp);struct _RegionHandle _tmp161=_new_region("r");struct
_RegionHandle*r=& _tmp161;_push_region(r);{unsigned int _tmp506;unsigned int
_tmp505;struct _dyneither_ptr _tmp504;int*_tmp503;unsigned int _tmp502;struct
_dyneither_ptr _tmp162=(_tmp502=(unsigned int)argc,((_tmp503=(int*)
_cycalloc_atomic(_check_times(sizeof(int),_tmp502)),((_tmp504=_tag_dyneither(
_tmp503,sizeof(int),_tmp502),((((_tmp505=_tmp502,_tmp507(& _tmp506,& _tmp505,&
_tmp503))),_tmp504)))))));int _tmp163=0;int i;int j;int k;for(i=1;i < argc;++ i){const
char*_tmp508;if(Cyc_strncmp(((_tmp508="-B",_tag_dyneither(_tmp508,sizeof(char),3))),(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,
sizeof(struct _dyneither_ptr),i)),2)== 0){*((int*)_check_dyneither_subscript(
_tmp162,sizeof(int),i))=1;++ _tmp163;}else{const char*_tmp509;if(Cyc_strcmp(((
_tmp509="-b",_tag_dyneither(_tmp509,sizeof(char),3))),(struct _dyneither_ptr)*((
struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i)))== 0){*((int*)_check_dyneither_subscript(_tmp162,sizeof(int),
i))=1;++ _tmp163;++ i;if(i >= argc)break;*((int*)_check_dyneither_subscript(_tmp162,
sizeof(int),i))=1;++ _tmp163;}}}{unsigned int _tmp50E;unsigned int _tmp50D;struct
_dyneither_ptr _tmp50C;struct _dyneither_ptr*_tmp50B;unsigned int _tmp50A;struct
_dyneither_ptr _tmp166=(_tmp50A=(unsigned int)(_tmp163 + 1),((_tmp50B=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp50A)),((
_tmp50C=_tag_dyneither(_tmp50B,sizeof(struct _dyneither_ptr),_tmp50A),((((_tmp50D=
_tmp50A,_tmp50F(& _tmp50E,& _tmp50D,& _tmp50B))),_tmp50C)))))));unsigned int _tmp514;
unsigned int _tmp513;struct _dyneither_ptr _tmp512;struct _dyneither_ptr*_tmp511;
unsigned int _tmp510;struct _dyneither_ptr _tmp167=(_tmp510=(unsigned int)(argc - 
_tmp163),((_tmp511=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct
_dyneither_ptr),_tmp510)),((_tmp512=_tag_dyneither(_tmp511,sizeof(struct
_dyneither_ptr),_tmp510),((((_tmp513=_tmp510,_tmp515(& _tmp514,& _tmp513,& _tmp511))),
_tmp512)))))));*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp166,
sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)
_check_dyneither_subscript(_tmp167,sizeof(struct _dyneither_ptr),0))=*((struct
_dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){if(*((int*)_check_dyneither_subscript(_tmp162,
sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp166,
sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)
_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{*((struct
_dyneither_ptr*)_check_dyneither_subscript(_tmp167,sizeof(struct _dyneither_ptr),
k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i));}}{struct _tuple23*_tmpA38;const char*_tmpA37;const char*
_tmpA36;struct Cyc_Arg_Set_spec_struct _tmpA35;struct Cyc_Arg_Set_spec_struct*
_tmpA34;const char*_tmpA33;struct _tuple23*_tmpA32;const char*_tmpA31;const char*
_tmpA30;struct Cyc_Arg_Unit_spec_struct _tmpA2F;struct Cyc_Arg_Unit_spec_struct*
_tmpA2E;const char*_tmpA2D;struct _tuple23*_tmpA2C;const char*_tmpA2B;const char*
_tmpA2A;struct Cyc_Arg_String_spec_struct _tmpA29;struct Cyc_Arg_String_spec_struct*
_tmpA28;const char*_tmpA27;struct _tuple23*_tmpA26;const char*_tmpA25;const char*
_tmpA24;struct Cyc_Arg_Flag_spec_struct _tmpA23;struct Cyc_Arg_Flag_spec_struct*
_tmpA22;const char*_tmpA21;struct _tuple23*_tmpA20;const char*_tmpA1F;const char*
_tmpA1E;struct Cyc_Arg_Flag_spec_struct _tmpA1D;struct Cyc_Arg_Flag_spec_struct*
_tmpA1C;const char*_tmpA1B;struct _tuple23*_tmpA1A;const char*_tmpA19;const char*
_tmpA18;struct Cyc_Arg_Flag_spec_struct _tmpA17;struct Cyc_Arg_Flag_spec_struct*
_tmpA16;const char*_tmpA15;struct _tuple23*_tmpA14;const char*_tmpA13;const char*
_tmpA12;struct Cyc_Arg_Flag_spec_struct _tmpA11;struct Cyc_Arg_Flag_spec_struct*
_tmpA10;const char*_tmpA0F;struct _tuple23*_tmpA0E;const char*_tmpA0D;const char*
_tmpA0C;struct Cyc_Arg_Flag_spec_struct _tmpA0B;struct Cyc_Arg_Flag_spec_struct*
_tmpA0A;const char*_tmpA09;struct _tuple23*_tmpA08;const char*_tmpA07;const char*
_tmpA06;struct Cyc_Arg_Flag_spec_struct _tmpA05;struct Cyc_Arg_Flag_spec_struct*
_tmpA04;const char*_tmpA03;struct _tuple23*_tmpA02;const char*_tmpA01;const char*
_tmpA00;struct Cyc_Arg_Unit_spec_struct _tmp9FF;struct Cyc_Arg_Unit_spec_struct*
_tmp9FE;const char*_tmp9FD;struct _tuple23*_tmp9FC;const char*_tmp9FB;const char*
_tmp9FA;struct Cyc_Arg_String_spec_struct _tmp9F9;struct Cyc_Arg_String_spec_struct*
_tmp9F8;const char*_tmp9F7;struct _tuple23*_tmp9F6;const char*_tmp9F5;const char*
_tmp9F4;struct Cyc_Arg_Flag_spec_struct _tmp9F3;struct Cyc_Arg_Flag_spec_struct*
_tmp9F2;const char*_tmp9F1;struct _tuple23*_tmp9F0;const char*_tmp9EF;const char*
_tmp9EE;struct Cyc_Arg_Flag_spec_struct _tmp9ED;struct Cyc_Arg_Flag_spec_struct*
_tmp9EC;const char*_tmp9EB;struct _tuple23*_tmp9EA;const char*_tmp9E9;const char*
_tmp9E8;struct Cyc_Arg_Flag_spec_struct _tmp9E7;struct Cyc_Arg_Flag_spec_struct*
_tmp9E6;const char*_tmp9E5;struct _tuple23*_tmp9E4;const char*_tmp9E3;const char*
_tmp9E2;struct Cyc_Arg_Flag_spec_struct _tmp9E1;struct Cyc_Arg_Flag_spec_struct*
_tmp9E0;const char*_tmp9DF;struct _tuple23*_tmp9DE;const char*_tmp9DD;const char*
_tmp9DC;struct Cyc_Arg_Flag_spec_struct _tmp9DB;struct Cyc_Arg_Flag_spec_struct*
_tmp9DA;const char*_tmp9D9;struct _tuple23*_tmp9D8;const char*_tmp9D7;const char*
_tmp9D6;struct Cyc_Arg_Flag_spec_struct _tmp9D5;struct Cyc_Arg_Flag_spec_struct*
_tmp9D4;const char*_tmp9D3;struct _tuple23*_tmp9D2;const char*_tmp9D1;const char*
_tmp9D0;struct Cyc_Arg_Flag_spec_struct _tmp9CF;struct Cyc_Arg_Flag_spec_struct*
_tmp9CE;const char*_tmp9CD;struct _tuple23*_tmp9CC;const char*_tmp9CB;const char*
_tmp9CA;struct Cyc_Arg_Flag_spec_struct _tmp9C9;struct Cyc_Arg_Flag_spec_struct*
_tmp9C8;const char*_tmp9C7;struct _tuple23*_tmp9C6;const char*_tmp9C5;const char*
_tmp9C4;struct Cyc_Arg_Unit_spec_struct _tmp9C3;struct Cyc_Arg_Unit_spec_struct*
_tmp9C2;const char*_tmp9C1;struct _tuple23*_tmp9C0;const char*_tmp9BF;const char*
_tmp9BE;struct Cyc_Arg_Unit_spec_struct _tmp9BD;struct Cyc_Arg_Unit_spec_struct*
_tmp9BC;const char*_tmp9BB;struct _tuple23*_tmp9BA;const char*_tmp9B9;const char*
_tmp9B8;struct Cyc_Arg_Unit_spec_struct _tmp9B7;struct Cyc_Arg_Unit_spec_struct*
_tmp9B6;const char*_tmp9B5;struct _tuple23*_tmp9B4;const char*_tmp9B3;const char*
_tmp9B2;struct Cyc_Arg_Unit_spec_struct _tmp9B1;struct Cyc_Arg_Unit_spec_struct*
_tmp9B0;const char*_tmp9AF;struct _tuple23*_tmp9AE;const char*_tmp9AD;const char*
_tmp9AC;struct Cyc_Arg_Flag_spec_struct _tmp9AB;struct Cyc_Arg_Flag_spec_struct*
_tmp9AA;const char*_tmp9A9;struct _tuple23*_tmp9A8;const char*_tmp9A7;const char*
_tmp9A6;struct Cyc_Arg_String_spec_struct _tmp9A5;struct Cyc_Arg_String_spec_struct*
_tmp9A4;const char*_tmp9A3;struct _tuple23*_tmp9A2;const char*_tmp9A1;const char*
_tmp9A0;struct Cyc_Arg_String_spec_struct _tmp99F;struct Cyc_Arg_String_spec_struct*
_tmp99E;const char*_tmp99D;struct _tuple23*_tmp99C;const char*_tmp99B;const char*
_tmp99A;struct Cyc_Arg_Set_spec_struct _tmp999;struct Cyc_Arg_Set_spec_struct*
_tmp998;const char*_tmp997;struct _tuple23*_tmp996;const char*_tmp995;const char*
_tmp994;struct Cyc_Arg_Unit_spec_struct _tmp993;struct Cyc_Arg_Unit_spec_struct*
_tmp992;const char*_tmp991;struct _tuple23*_tmp990;const char*_tmp98F;const char*
_tmp98E;struct Cyc_Arg_Set_spec_struct _tmp98D;struct Cyc_Arg_Set_spec_struct*
_tmp98C;const char*_tmp98B;struct _tuple23*_tmp98A;const char*_tmp989;const char*
_tmp988;struct Cyc_Arg_Set_spec_struct _tmp987;struct Cyc_Arg_Set_spec_struct*
_tmp986;const char*_tmp985;struct _tuple23*_tmp984;const char*_tmp983;const char*
_tmp982;struct Cyc_Arg_Set_spec_struct _tmp981;struct Cyc_Arg_Set_spec_struct*
_tmp980;const char*_tmp97F;struct _tuple23*_tmp97E;const char*_tmp97D;const char*
_tmp97C;struct Cyc_Arg_Set_spec_struct _tmp97B;struct Cyc_Arg_Set_spec_struct*
_tmp97A;const char*_tmp979;struct _tuple23*_tmp978;const char*_tmp977;const char*
_tmp976;struct Cyc_Arg_Set_spec_struct _tmp975;struct Cyc_Arg_Set_spec_struct*
_tmp974;const char*_tmp973;struct _tuple23*_tmp972;const char*_tmp971;const char*
_tmp970;struct Cyc_Arg_Set_spec_struct _tmp96F;struct Cyc_Arg_Set_spec_struct*
_tmp96E;const char*_tmp96D;struct _tuple23*_tmp96C;const char*_tmp96B;const char*
_tmp96A;struct Cyc_Arg_Set_spec_struct _tmp969;struct Cyc_Arg_Set_spec_struct*
_tmp968;const char*_tmp967;struct _tuple23*_tmp966;const char*_tmp965;const char*
_tmp964;struct Cyc_Arg_Set_spec_struct _tmp963;struct Cyc_Arg_Set_spec_struct*
_tmp962;const char*_tmp961;struct _tuple23*_tmp960;const char*_tmp95F;const char*
_tmp95E;struct Cyc_Arg_Clear_spec_struct _tmp95D;struct Cyc_Arg_Clear_spec_struct*
_tmp95C;const char*_tmp95B;struct _tuple23*_tmp95A;const char*_tmp959;const char*
_tmp958;struct Cyc_Arg_Set_spec_struct _tmp957;struct Cyc_Arg_Set_spec_struct*
_tmp956;const char*_tmp955;struct _tuple23*_tmp954;const char*_tmp953;const char*
_tmp952;struct Cyc_Arg_Clear_spec_struct _tmp951;struct Cyc_Arg_Clear_spec_struct*
_tmp950;const char*_tmp94F;struct _tuple23*_tmp94E;const char*_tmp94D;const char*
_tmp94C;struct Cyc_Arg_Unit_spec_struct _tmp94B;struct Cyc_Arg_Unit_spec_struct*
_tmp94A;const char*_tmp949;struct _tuple23*_tmp948;const char*_tmp947;const char*
_tmp946;struct Cyc_Arg_String_spec_struct _tmp945;struct Cyc_Arg_String_spec_struct*
_tmp944;const char*_tmp943;struct _tuple23*_tmp942;const char*_tmp941;const char*
_tmp940;struct Cyc_Arg_Unit_spec_struct _tmp93F;struct Cyc_Arg_Unit_spec_struct*
_tmp93E;const char*_tmp93D;struct _tuple23*_tmp93C;const char*_tmp93B;const char*
_tmp93A;struct Cyc_Arg_Set_spec_struct _tmp939;struct Cyc_Arg_Set_spec_struct*
_tmp938;const char*_tmp937;struct _tuple23*_tmp936;const char*_tmp935;const char*
_tmp934;struct Cyc_Arg_Unit_spec_struct _tmp933;struct Cyc_Arg_Unit_spec_struct*
_tmp932;const char*_tmp931;struct _tuple23*_tmp930;const char*_tmp92F;const char*
_tmp92E;struct Cyc_Arg_Unit_spec_struct _tmp92D;struct Cyc_Arg_Unit_spec_struct*
_tmp92C;const char*_tmp92B;struct _tuple23*_tmp92A;const char*_tmp929;const char*
_tmp928;struct Cyc_Arg_Unit_spec_struct _tmp927;struct Cyc_Arg_Unit_spec_struct*
_tmp926;const char*_tmp925;struct _tuple23*_tmp924;const char*_tmp923;const char*
_tmp922;struct Cyc_Arg_Unit_spec_struct _tmp921;struct Cyc_Arg_Unit_spec_struct*
_tmp920;const char*_tmp91F;struct _tuple23*_tmp91E;const char*_tmp91D;const char*
_tmp91C;struct Cyc_Arg_Unit_spec_struct _tmp91B;struct Cyc_Arg_Unit_spec_struct*
_tmp91A;const char*_tmp919;struct _tuple23*_tmp918;const char*_tmp917;const char*
_tmp916;struct Cyc_Arg_String_spec_struct _tmp915;struct Cyc_Arg_String_spec_struct*
_tmp914;const char*_tmp913;struct _tuple23*_tmp912;const char*_tmp911;const char*
_tmp910;struct Cyc_Arg_Unit_spec_struct _tmp90F;struct Cyc_Arg_Unit_spec_struct*
_tmp90E;const char*_tmp90D;struct _tuple23*_tmp90C;const char*_tmp90B;const char*
_tmp90A;struct Cyc_Arg_Unit_spec_struct _tmp909;struct Cyc_Arg_Unit_spec_struct*
_tmp908;const char*_tmp907;struct _tuple23*_tmp906;const char*_tmp905;const char*
_tmp904;struct Cyc_Arg_Set_spec_struct _tmp903;struct Cyc_Arg_Set_spec_struct*
_tmp902;const char*_tmp901;struct _tuple23*_tmp900;const char*_tmp8FF;const char*
_tmp8FE;struct Cyc_Arg_Set_spec_struct _tmp8FD;struct Cyc_Arg_Set_spec_struct*
_tmp8FC;const char*_tmp8FB;struct _tuple23*_tmp8FA;const char*_tmp8F9;const char*
_tmp8F8;struct Cyc_Arg_Set_spec_struct _tmp8F7;struct Cyc_Arg_Set_spec_struct*
_tmp8F6;const char*_tmp8F5;struct _tuple23*_tmp8F4;const char*_tmp8F3;const char*
_tmp8F2;struct Cyc_Arg_Set_spec_struct _tmp8F1;struct Cyc_Arg_Set_spec_struct*
_tmp8F0;const char*_tmp8EF;struct _tuple23*_tmp8EE;const char*_tmp8ED;const char*
_tmp8EC;struct Cyc_Arg_Set_spec_struct _tmp8EB;struct Cyc_Arg_Set_spec_struct*
_tmp8EA;const char*_tmp8E9;struct _tuple23*_tmp8E8;const char*_tmp8E7;const char*
_tmp8E6;struct Cyc_Arg_Set_spec_struct _tmp8E5;struct Cyc_Arg_Set_spec_struct*
_tmp8E4;const char*_tmp8E3;struct _tuple23*_tmp8E2;const char*_tmp8E1;const char*
_tmp8E0;struct Cyc_Arg_Set_spec_struct _tmp8DF;struct Cyc_Arg_Set_spec_struct*
_tmp8DE;const char*_tmp8DD;struct _tuple23*_tmp8DC;const char*_tmp8DB;const char*
_tmp8DA;struct Cyc_Arg_Set_spec_struct _tmp8D9;struct Cyc_Arg_Set_spec_struct*
_tmp8D8;const char*_tmp8D7;struct _tuple23*_tmp8D6;const char*_tmp8D5;const char*
_tmp8D4;struct Cyc_Arg_Set_spec_struct _tmp8D3;struct Cyc_Arg_Set_spec_struct*
_tmp8D2;const char*_tmp8D1;struct _tuple23*_tmp8D0;const char*_tmp8CF;const char*
_tmp8CE;struct Cyc_Arg_String_spec_struct _tmp8CD;struct Cyc_Arg_String_spec_struct*
_tmp8CC;const char*_tmp8CB;struct _tuple23*_tmp8CA;const char*_tmp8C9;const char*
_tmp8C8;struct Cyc_Arg_Set_spec_struct _tmp8C7;struct Cyc_Arg_Set_spec_struct*
_tmp8C6;const char*_tmp8C5;struct _tuple23*_tmp8C4;const char*_tmp8C3;const char*
_tmp8C2;struct Cyc_Arg_Set_spec_struct _tmp8C1;struct Cyc_Arg_Set_spec_struct*
_tmp8C0;const char*_tmp8BF;struct _tuple23*_tmp8BE;const char*_tmp8BD;const char*
_tmp8BC;struct Cyc_Arg_Set_spec_struct _tmp8BB;struct Cyc_Arg_Set_spec_struct*
_tmp8BA;const char*_tmp8B9;struct _tuple23*_tmp8B8;const char*_tmp8B7;const char*
_tmp8B6;struct Cyc_Arg_Set_spec_struct _tmp8B5;struct Cyc_Arg_Set_spec_struct*
_tmp8B4;const char*_tmp8B3;struct _tuple23*_tmp8B2;const char*_tmp8B1;const char*
_tmp8B0;struct Cyc_Arg_Clear_spec_struct _tmp8AF;struct Cyc_Arg_Clear_spec_struct*
_tmp8AE;const char*_tmp8AD;struct _tuple23*_tmp8AC;const char*_tmp8AB;const char*
_tmp8AA;struct Cyc_Arg_Unit_spec_struct _tmp8A9;struct Cyc_Arg_Unit_spec_struct*
_tmp8A8;const char*_tmp8A7;struct _tuple23*_tmp8A6;const char*_tmp8A5;const char*
_tmp8A4;struct Cyc_Arg_Set_spec_struct _tmp8A3;struct Cyc_Arg_Set_spec_struct*
_tmp8A2;const char*_tmp8A1;struct _tuple23*_tmp8A0;const char*_tmp89F;const char*
_tmp89E;struct Cyc_Arg_String_spec_struct _tmp89D;struct Cyc_Arg_String_spec_struct*
_tmp89C;const char*_tmp89B;struct _tuple23*_tmp89A;const char*_tmp899;const char*
_tmp898;struct Cyc_Arg_String_spec_struct _tmp897;struct Cyc_Arg_String_spec_struct*
_tmp896;const char*_tmp895;struct _tuple23*_tmp894;const char*_tmp893;const char*
_tmp892;struct Cyc_Arg_String_spec_struct _tmp891;struct Cyc_Arg_String_spec_struct*
_tmp890;const char*_tmp88F;struct _tuple23*_tmp88E;const char*_tmp88D;const char*
_tmp88C;struct Cyc_Arg_String_spec_struct _tmp88B;struct Cyc_Arg_String_spec_struct*
_tmp88A;const char*_tmp889;struct _tuple23*_tmp888;const char*_tmp887;const char*
_tmp886;struct Cyc_Arg_String_spec_struct _tmp885;struct Cyc_Arg_String_spec_struct*
_tmp884;const char*_tmp883;struct _tuple23*_tmp882[73];struct Cyc_List_List*options=(
_tmp882[72]=((_tmp888=_region_malloc(r,sizeof(*_tmp888)),((_tmp888->f1=((_tmp887="-idirafter",
_tag_dyneither(_tmp887,sizeof(char),11))),((_tmp888->f2=1,((_tmp888->f3=((
_tmp886="<dir>",_tag_dyneither(_tmp886,sizeof(char),6))),((_tmp888->f4=(void*)((
_tmp884=_region_malloc(r,sizeof(*_tmp884)),((_tmp884[0]=((_tmp885.tag=5,((
_tmp885.f1=Cyc_add_idirafter,_tmp885)))),_tmp884)))),((_tmp888->f5=((_tmp883="Add the directory to the second include path.",
_tag_dyneither(_tmp883,sizeof(char),46))),_tmp888)))))))))))),((_tmp882[71]=((
_tmp88E=_region_malloc(r,sizeof(*_tmp88E)),((_tmp88E->f1=((_tmp88D="-isystem",
_tag_dyneither(_tmp88D,sizeof(char),9))),((_tmp88E->f2=1,((_tmp88E->f3=((_tmp88C="<dir>",
_tag_dyneither(_tmp88C,sizeof(char),6))),((_tmp88E->f4=(void*)((_tmp88A=
_region_malloc(r,sizeof(*_tmp88A)),((_tmp88A[0]=((_tmp88B.tag=5,((_tmp88B.f1=Cyc_add_isystem,
_tmp88B)))),_tmp88A)))),((_tmp88E->f5=((_tmp889="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",
_tag_dyneither(_tmp889,sizeof(char),90))),_tmp88E)))))))))))),((_tmp882[70]=((
_tmp894=_region_malloc(r,sizeof(*_tmp894)),((_tmp894->f1=((_tmp893="-iwithprefixbefore",
_tag_dyneither(_tmp893,sizeof(char),19))),((_tmp894->f2=1,((_tmp894->f3=((
_tmp892="<dir>",_tag_dyneither(_tmp892,sizeof(char),6))),((_tmp894->f4=(void*)((
_tmp890=_region_malloc(r,sizeof(*_tmp890)),((_tmp890[0]=((_tmp891.tag=5,((
_tmp891.f1=Cyc_add_iwithprefixbefore,_tmp891)))),_tmp890)))),((_tmp894->f5=((
_tmp88F="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp88F,
sizeof(char),45))),_tmp894)))))))))))),((_tmp882[69]=((_tmp89A=_region_malloc(r,
sizeof(*_tmp89A)),((_tmp89A->f1=((_tmp899="-iwithprefix",_tag_dyneither(_tmp899,
sizeof(char),13))),((_tmp89A->f2=1,((_tmp89A->f3=((_tmp898="<dir>",
_tag_dyneither(_tmp898,sizeof(char),6))),((_tmp89A->f4=(void*)((_tmp896=
_region_malloc(r,sizeof(*_tmp896)),((_tmp896[0]=((_tmp897.tag=5,((_tmp897.f1=Cyc_add_iwithprefix,
_tmp897)))),_tmp896)))),((_tmp89A->f5=((_tmp895="Add <prefix>/<dir> to the second include path.",
_tag_dyneither(_tmp895,sizeof(char),47))),_tmp89A)))))))))))),((_tmp882[68]=((
_tmp8A0=_region_malloc(r,sizeof(*_tmp8A0)),((_tmp8A0->f1=((_tmp89F="-iprefix",
_tag_dyneither(_tmp89F,sizeof(char),9))),((_tmp8A0->f2=1,((_tmp8A0->f3=((_tmp89E="<prefix>",
_tag_dyneither(_tmp89E,sizeof(char),9))),((_tmp8A0->f4=(void*)((_tmp89C=
_region_malloc(r,sizeof(*_tmp89C)),((_tmp89C[0]=((_tmp89D.tag=5,((_tmp89D.f1=Cyc_add_iprefix,
_tmp89D)))),_tmp89C)))),((_tmp8A0->f5=((_tmp89B="Specify <prefix> as the prefix for subsequent -iwithprefix options",
_tag_dyneither(_tmp89B,sizeof(char),67))),_tmp8A0)))))))))))),((_tmp882[67]=((
_tmp8A6=_region_malloc(r,sizeof(*_tmp8A6)),((_tmp8A6->f1=((_tmp8A5="-noregions",
_tag_dyneither(_tmp8A5,sizeof(char),11))),((_tmp8A6->f2=0,((_tmp8A6->f3=((
_tmp8A4="",_tag_dyneither(_tmp8A4,sizeof(char),1))),((_tmp8A6->f4=(void*)((
_tmp8A2=_region_malloc(r,sizeof(*_tmp8A2)),((_tmp8A2[0]=((_tmp8A3.tag=3,((
_tmp8A3.f1=& Cyc_Absyn_no_regions,_tmp8A3)))),_tmp8A2)))),((_tmp8A6->f5=((_tmp8A1="Generate code that doesn't use regions",
_tag_dyneither(_tmp8A1,sizeof(char),39))),_tmp8A6)))))))))))),((_tmp882[66]=((
_tmp8AC=_region_malloc(r,sizeof(*_tmp8AC)),((_tmp8AC->f1=((_tmp8AB="-port",
_tag_dyneither(_tmp8AB,sizeof(char),6))),((_tmp8AC->f2=0,((_tmp8AC->f3=((_tmp8AA="",
_tag_dyneither(_tmp8AA,sizeof(char),1))),((_tmp8AC->f4=(void*)((_tmp8A8=
_region_malloc(r,sizeof(*_tmp8A8)),((_tmp8A8[0]=((_tmp8A9.tag=0,((_tmp8A9.f1=Cyc_set_port_c_code,
_tmp8A9)))),_tmp8A8)))),((_tmp8AC->f5=((_tmp8A7="Suggest how to port C code to Cyclone",
_tag_dyneither(_tmp8A7,sizeof(char),38))),_tmp8AC)))))))))))),((_tmp882[65]=((
_tmp8B2=_region_malloc(r,sizeof(*_tmp8B2)),((_tmp8B2->f1=((_tmp8B1="-detailedlocation",
_tag_dyneither(_tmp8B1,sizeof(char),18))),((_tmp8B2->f2=0,((_tmp8B2->f3=((
_tmp8B0="",_tag_dyneither(_tmp8B0,sizeof(char),1))),((_tmp8B2->f4=(void*)((
_tmp8AE=_region_malloc(r,sizeof(*_tmp8AE)),((_tmp8AE[0]=((_tmp8AF.tag=4,((
_tmp8AF.f1=& Cyc_Position_use_gcc_style_location,_tmp8AF)))),_tmp8AE)))),((
_tmp8B2->f5=((_tmp8AD="Try to give more detailed location information for errors",
_tag_dyneither(_tmp8AD,sizeof(char),58))),_tmp8B2)))))))))))),((_tmp882[64]=((
_tmp8B8=_region_malloc(r,sizeof(*_tmp8B8)),((_tmp8B8->f1=((_tmp8B7="-noregister",
_tag_dyneither(_tmp8B7,sizeof(char),12))),((_tmp8B8->f2=0,((_tmp8B8->f3=((
_tmp8B6="",_tag_dyneither(_tmp8B6,sizeof(char),1))),((_tmp8B8->f4=(void*)((
_tmp8B4=_region_malloc(r,sizeof(*_tmp8B4)),((_tmp8B4[0]=((_tmp8B5.tag=3,((
_tmp8B5.f1=& Cyc_Parse_no_register,_tmp8B5)))),_tmp8B4)))),((_tmp8B8->f5=((
_tmp8B3="Treat register storage class as public",_tag_dyneither(_tmp8B3,sizeof(
char),39))),_tmp8B8)))))))))))),((_tmp882[63]=((_tmp8BE=_region_malloc(r,sizeof(*
_tmp8BE)),((_tmp8BE->f1=((_tmp8BD="-warnaliascoercion",_tag_dyneither(_tmp8BD,
sizeof(char),19))),((_tmp8BE->f2=0,((_tmp8BE->f3=((_tmp8BC="",_tag_dyneither(
_tmp8BC,sizeof(char),1))),((_tmp8BE->f4=(void*)((_tmp8BA=_region_malloc(r,
sizeof(*_tmp8BA)),((_tmp8BA[0]=((_tmp8BB.tag=3,((_tmp8BB.f1=& Cyc_Tcutil_warn_alias_coerce,
_tmp8BB)))),_tmp8BA)))),((_tmp8BE->f5=((_tmp8B9="Warn when any alias coercion is inserted",
_tag_dyneither(_tmp8B9,sizeof(char),41))),_tmp8BE)))))))))))),((_tmp882[62]=((
_tmp8C4=_region_malloc(r,sizeof(*_tmp8C4)),((_tmp8C4->f1=((_tmp8C3="-warnnullchecks",
_tag_dyneither(_tmp8C3,sizeof(char),16))),((_tmp8C4->f2=0,((_tmp8C4->f3=((
_tmp8C2="",_tag_dyneither(_tmp8C2,sizeof(char),1))),((_tmp8C4->f4=(void*)((
_tmp8C0=_region_malloc(r,sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8C1.tag=3,((
_tmp8C1.f1=& Cyc_Toc_warn_all_null_deref,_tmp8C1)))),_tmp8C0)))),((_tmp8C4->f5=((
_tmp8BF="Warn when any null check can't be eliminated",_tag_dyneither(_tmp8BF,
sizeof(char),45))),_tmp8C4)))))))))))),((_tmp882[61]=((_tmp8CA=_region_malloc(r,
sizeof(*_tmp8CA)),((_tmp8CA->f1=((_tmp8C9="-warnboundschecks",_tag_dyneither(
_tmp8C9,sizeof(char),18))),((_tmp8CA->f2=0,((_tmp8CA->f3=((_tmp8C8="",
_tag_dyneither(_tmp8C8,sizeof(char),1))),((_tmp8CA->f4=(void*)((_tmp8C6=
_region_malloc(r,sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C7.tag=3,((_tmp8C7.f1=&
Cyc_Toc_warn_bounds_checks,_tmp8C7)))),_tmp8C6)))),((_tmp8CA->f5=((_tmp8C5="Warn when bounds checks can't be eliminated",
_tag_dyneither(_tmp8C5,sizeof(char),44))),_tmp8CA)))))))))))),((_tmp882[60]=((
_tmp8D0=_region_malloc(r,sizeof(*_tmp8D0)),((_tmp8D0->f1=((_tmp8CF="-CI",
_tag_dyneither(_tmp8CF,sizeof(char),4))),((_tmp8D0->f2=0,((_tmp8D0->f3=((_tmp8CE=" <file>",
_tag_dyneither(_tmp8CE,sizeof(char),8))),((_tmp8D0->f4=(void*)((_tmp8CC=
_region_malloc(r,sizeof(*_tmp8CC)),((_tmp8CC[0]=((_tmp8CD.tag=5,((_tmp8CD.f1=Cyc_set_cyc_include,
_tmp8CD)))),_tmp8CC)))),((_tmp8D0->f5=((_tmp8CB="Set cyc_include.h to be <file>",
_tag_dyneither(_tmp8CB,sizeof(char),31))),_tmp8D0)))))))))))),((_tmp882[59]=((
_tmp8D6=_region_malloc(r,sizeof(*_tmp8D6)),((_tmp8D6->f1=((_tmp8D5="-compile-for-boot",
_tag_dyneither(_tmp8D5,sizeof(char),18))),((_tmp8D6->f2=0,((_tmp8D6->f3=((
_tmp8D4="",_tag_dyneither(_tmp8D4,sizeof(char),1))),((_tmp8D6->f4=(void*)((
_tmp8D2=_region_malloc(r,sizeof(*_tmp8D2)),((_tmp8D2[0]=((_tmp8D3.tag=3,((
_tmp8D3.f1=& Cyc_compile_for_boot_r,_tmp8D3)))),_tmp8D2)))),((_tmp8D6->f5=((
_tmp8D1="Compile using the special boot library instead of the standard library",
_tag_dyneither(_tmp8D1,sizeof(char),71))),_tmp8D6)))))))))))),((_tmp882[58]=((
_tmp8DC=_region_malloc(r,sizeof(*_tmp8DC)),((_tmp8DC->f1=((_tmp8DB="-nocyc_setjmp",
_tag_dyneither(_tmp8DB,sizeof(char),14))),((_tmp8DC->f2=0,((_tmp8DC->f3=((
_tmp8DA="",_tag_dyneither(_tmp8DA,sizeof(char),1))),((_tmp8DC->f4=(void*)((
_tmp8D8=_region_malloc(r,sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8D9.tag=3,((
_tmp8D9.f1=& Cyc_nocyc_setjmp_r,_tmp8D9)))),_tmp8D8)))),((_tmp8DC->f5=((_tmp8D7="Do not use compiler special file cyc_setjmp.h",
_tag_dyneither(_tmp8D7,sizeof(char),46))),_tmp8DC)))))))))))),((_tmp882[57]=((
_tmp8E2=_region_malloc(r,sizeof(*_tmp8E2)),((_tmp8E2->f1=((_tmp8E1="-printalleffects",
_tag_dyneither(_tmp8E1,sizeof(char),17))),((_tmp8E2->f2=0,((_tmp8E2->f3=((
_tmp8E0="",_tag_dyneither(_tmp8E0,sizeof(char),1))),((_tmp8E2->f4=(void*)((
_tmp8DE=_region_malloc(r,sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8DF.tag=3,((
_tmp8DF.f1=& Cyc_print_all_effects_r,_tmp8DF)))),_tmp8DE)))),((_tmp8E2->f5=((
_tmp8DD="Print effects for functions (type debugging)",_tag_dyneither(_tmp8DD,
sizeof(char),45))),_tmp8E2)))))))))))),((_tmp882[56]=((_tmp8E8=_region_malloc(r,
sizeof(*_tmp8E8)),((_tmp8E8->f1=((_tmp8E7="-printfullevars",_tag_dyneither(
_tmp8E7,sizeof(char),16))),((_tmp8E8->f2=0,((_tmp8E8->f3=((_tmp8E6="",
_tag_dyneither(_tmp8E6,sizeof(char),1))),((_tmp8E8->f4=(void*)((_tmp8E4=
_region_malloc(r,sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E5.tag=3,((_tmp8E5.f1=&
Cyc_print_full_evars_r,_tmp8E5)))),_tmp8E4)))),((_tmp8E8->f5=((_tmp8E3="Print full information for evars (type debugging)",
_tag_dyneither(_tmp8E3,sizeof(char),50))),_tmp8E8)))))))))))),((_tmp882[55]=((
_tmp8EE=_region_malloc(r,sizeof(*_tmp8EE)),((_tmp8EE->f1=((_tmp8ED="-printallkinds",
_tag_dyneither(_tmp8ED,sizeof(char),15))),((_tmp8EE->f2=0,((_tmp8EE->f3=((
_tmp8EC="",_tag_dyneither(_tmp8EC,sizeof(char),1))),((_tmp8EE->f4=(void*)((
_tmp8EA=_region_malloc(r,sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8EB.tag=3,((
_tmp8EB.f1=& Cyc_print_all_kinds_r,_tmp8EB)))),_tmp8EA)))),((_tmp8EE->f5=((
_tmp8E9="Always print kinds of type variables",_tag_dyneither(_tmp8E9,sizeof(
char),37))),_tmp8EE)))))))))))),((_tmp882[54]=((_tmp8F4=_region_malloc(r,sizeof(*
_tmp8F4)),((_tmp8F4->f1=((_tmp8F3="-printalltvars",_tag_dyneither(_tmp8F3,
sizeof(char),15))),((_tmp8F4->f2=0,((_tmp8F4->f3=((_tmp8F2="",_tag_dyneither(
_tmp8F2,sizeof(char),1))),((_tmp8F4->f4=(void*)((_tmp8F0=_region_malloc(r,
sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8F1.tag=3,((_tmp8F1.f1=& Cyc_print_all_tvars_r,
_tmp8F1)))),_tmp8F0)))),((_tmp8F4->f5=((_tmp8EF="Print all type variables (even implicit default effects)",
_tag_dyneither(_tmp8EF,sizeof(char),57))),_tmp8F4)))))))))))),((_tmp882[53]=((
_tmp8FA=_region_malloc(r,sizeof(*_tmp8FA)),((_tmp8FA->f1=((_tmp8F9="-noexpandtypedefs",
_tag_dyneither(_tmp8F9,sizeof(char),18))),((_tmp8FA->f2=0,((_tmp8FA->f3=((
_tmp8F8="",_tag_dyneither(_tmp8F8,sizeof(char),1))),((_tmp8FA->f4=(void*)((
_tmp8F6=_region_malloc(r,sizeof(*_tmp8F6)),((_tmp8F6[0]=((_tmp8F7.tag=3,((
_tmp8F7.f1=& Cyc_noexpand_r,_tmp8F7)))),_tmp8F6)))),((_tmp8FA->f5=((_tmp8F5="Don't expand typedefs in pretty printing",
_tag_dyneither(_tmp8F5,sizeof(char),41))),_tmp8FA)))))))))))),((_tmp882[52]=((
_tmp900=_region_malloc(r,sizeof(*_tmp900)),((_tmp900->f1=((_tmp8FF="-noremoveunused",
_tag_dyneither(_tmp8FF,sizeof(char),16))),((_tmp900->f2=0,((_tmp900->f3=((
_tmp8FE="",_tag_dyneither(_tmp8FE,sizeof(char),1))),((_tmp900->f4=(void*)((
_tmp8FC=_region_malloc(r,sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FD.tag=3,((
_tmp8FD.f1=& Cyc_noshake_r,_tmp8FD)))),_tmp8FC)))),((_tmp900->f5=((_tmp8FB="Don't remove externed variables that aren't used",
_tag_dyneither(_tmp8FB,sizeof(char),49))),_tmp900)))))))))))),((_tmp882[51]=((
_tmp906=_region_malloc(r,sizeof(*_tmp906)),((_tmp906->f1=((_tmp905="-nogc",
_tag_dyneither(_tmp905,sizeof(char),6))),((_tmp906->f2=0,((_tmp906->f3=((_tmp904="",
_tag_dyneither(_tmp904,sizeof(char),1))),((_tmp906->f4=(void*)((_tmp902=
_region_malloc(r,sizeof(*_tmp902)),((_tmp902[0]=((_tmp903.tag=3,((_tmp903.f1=&
Cyc_nogc_r,_tmp903)))),_tmp902)))),((_tmp906->f5=((_tmp901="Don't link in the garbage collector",
_tag_dyneither(_tmp901,sizeof(char),36))),_tmp906)))))))))))),((_tmp882[50]=((
_tmp90C=_region_malloc(r,sizeof(*_tmp90C)),((_tmp90C->f1=((_tmp90B="-nocyc",
_tag_dyneither(_tmp90B,sizeof(char),7))),((_tmp90C->f2=0,((_tmp90C->f3=((_tmp90A="",
_tag_dyneither(_tmp90A,sizeof(char),1))),((_tmp90C->f4=(void*)((_tmp908=
_region_malloc(r,sizeof(*_tmp908)),((_tmp908[0]=((_tmp909.tag=0,((_tmp909.f1=Cyc_set_nocyc,
_tmp909)))),_tmp908)))),((_tmp90C->f5=((_tmp907="Don't add implicit namespace Cyc",
_tag_dyneither(_tmp907,sizeof(char),33))),_tmp90C)))))))))))),((_tmp882[49]=((
_tmp912=_region_malloc(r,sizeof(*_tmp912)),((_tmp912->f1=((_tmp911="-no-cpp-precomp",
_tag_dyneither(_tmp911,sizeof(char),16))),((_tmp912->f2=0,((_tmp912->f3=((
_tmp910="",_tag_dyneither(_tmp910,sizeof(char),1))),((_tmp912->f4=(void*)((
_tmp90E=_region_malloc(r,sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90F.tag=0,((
_tmp90F.f1=Cyc_set_nocppprecomp,_tmp90F)))),_tmp90E)))),((_tmp912->f5=((_tmp90D="Don't do smart preprocessing (mac only)",
_tag_dyneither(_tmp90D,sizeof(char),40))),_tmp912)))))))))))),((_tmp882[48]=((
_tmp918=_region_malloc(r,sizeof(*_tmp918)),((_tmp918->f1=((_tmp917="-use-cpp",
_tag_dyneither(_tmp917,sizeof(char),9))),((_tmp918->f2=0,((_tmp918->f3=((_tmp916="<path>",
_tag_dyneither(_tmp916,sizeof(char),7))),((_tmp918->f4=(void*)((_tmp914=
_region_malloc(r,sizeof(*_tmp914)),((_tmp914[0]=((_tmp915.tag=5,((_tmp915.f1=Cyc_set_cpp,
_tmp915)))),_tmp914)))),((_tmp918->f5=((_tmp913="Indicate which preprocessor to use",
_tag_dyneither(_tmp913,sizeof(char),35))),_tmp918)))))))))))),((_tmp882[47]=((
_tmp91E=_region_malloc(r,sizeof(*_tmp91E)),((_tmp91E->f1=((_tmp91D="--inline-checks",
_tag_dyneither(_tmp91D,sizeof(char),16))),((_tmp91E->f2=0,((_tmp91E->f3=((
_tmp91C="",_tag_dyneither(_tmp91C,sizeof(char),1))),((_tmp91E->f4=(void*)((
_tmp91A=_region_malloc(r,sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp91B.tag=0,((
_tmp91B.f1=Cyc_set_inline_functions,_tmp91B)))),_tmp91A)))),((_tmp91E->f5=((
_tmp919="Inline bounds checks instead of #define",_tag_dyneither(_tmp919,sizeof(
char),40))),_tmp91E)))))))))))),((_tmp882[46]=((_tmp924=_region_malloc(r,sizeof(*
_tmp924)),((_tmp924->f1=((_tmp923="--noboundschecks",_tag_dyneither(_tmp923,
sizeof(char),17))),((_tmp924->f2=0,((_tmp924->f3=((_tmp922="",_tag_dyneither(
_tmp922,sizeof(char),1))),((_tmp924->f4=(void*)((_tmp920=_region_malloc(r,
sizeof(*_tmp920)),((_tmp920[0]=((_tmp921.tag=0,((_tmp921.f1=Cyc_set_noboundschecks,
_tmp921)))),_tmp920)))),((_tmp924->f5=((_tmp91F="Disable bounds checks",
_tag_dyneither(_tmp91F,sizeof(char),22))),_tmp924)))))))))))),((_tmp882[45]=((
_tmp92A=_region_malloc(r,sizeof(*_tmp92A)),((_tmp92A->f1=((_tmp929="--nonullchecks",
_tag_dyneither(_tmp929,sizeof(char),15))),((_tmp92A->f2=0,((_tmp92A->f3=((
_tmp928="",_tag_dyneither(_tmp928,sizeof(char),1))),((_tmp92A->f4=(void*)((
_tmp926=_region_malloc(r,sizeof(*_tmp926)),((_tmp926[0]=((_tmp927.tag=0,((
_tmp927.f1=Cyc_set_nonullchecks,_tmp927)))),_tmp926)))),((_tmp92A->f5=((_tmp925="Disable null checks",
_tag_dyneither(_tmp925,sizeof(char),20))),_tmp92A)))))))))))),((_tmp882[44]=((
_tmp930=_region_malloc(r,sizeof(*_tmp930)),((_tmp930->f1=((_tmp92F="--nochecks",
_tag_dyneither(_tmp92F,sizeof(char),11))),((_tmp930->f2=0,((_tmp930->f3=((
_tmp92E="",_tag_dyneither(_tmp92E,sizeof(char),1))),((_tmp930->f4=(void*)((
_tmp92C=_region_malloc(r,sizeof(*_tmp92C)),((_tmp92C[0]=((_tmp92D.tag=0,((
_tmp92D.f1=Cyc_set_nochecks,_tmp92D)))),_tmp92C)))),((_tmp930->f5=((_tmp92B="Disable bounds/null checks",
_tag_dyneither(_tmp92B,sizeof(char),27))),_tmp930)))))))))))),((_tmp882[43]=((
_tmp936=_region_malloc(r,sizeof(*_tmp936)),((_tmp936->f1=((_tmp935="--lineno",
_tag_dyneither(_tmp935,sizeof(char),9))),((_tmp936->f2=0,((_tmp936->f3=((_tmp934="",
_tag_dyneither(_tmp934,sizeof(char),1))),((_tmp936->f4=(void*)((_tmp932=
_region_malloc(r,sizeof(*_tmp932)),((_tmp932[0]=((_tmp933.tag=0,((_tmp933.f1=Cyc_set_lineno,
_tmp933)))),_tmp932)))),((_tmp936->f5=((_tmp931="Generate line numbers for debugging",
_tag_dyneither(_tmp931,sizeof(char),36))),_tmp936)))))))))))),((_tmp882[42]=((
_tmp93C=_region_malloc(r,sizeof(*_tmp93C)),((_tmp93C->f1=((_tmp93B="-save-c",
_tag_dyneither(_tmp93B,sizeof(char),8))),((_tmp93C->f2=0,((_tmp93C->f3=((_tmp93A="",
_tag_dyneither(_tmp93A,sizeof(char),1))),((_tmp93C->f4=(void*)((_tmp938=
_region_malloc(r,sizeof(*_tmp938)),((_tmp938[0]=((_tmp939.tag=3,((_tmp939.f1=&
Cyc_save_c_r,_tmp939)))),_tmp938)))),((_tmp93C->f5=((_tmp937="Don't delete temporary C files",
_tag_dyneither(_tmp937,sizeof(char),31))),_tmp93C)))))))))))),((_tmp882[41]=((
_tmp942=_region_malloc(r,sizeof(*_tmp942)),((_tmp942->f1=((_tmp941="-save-temps",
_tag_dyneither(_tmp941,sizeof(char),12))),((_tmp942->f2=0,((_tmp942->f3=((
_tmp940="",_tag_dyneither(_tmp940,sizeof(char),1))),((_tmp942->f4=(void*)((
_tmp93E=_region_malloc(r,sizeof(*_tmp93E)),((_tmp93E[0]=((_tmp93F.tag=0,((
_tmp93F.f1=Cyc_set_save_temps,_tmp93F)))),_tmp93E)))),((_tmp942->f5=((_tmp93D="Don't delete temporary files",
_tag_dyneither(_tmp93D,sizeof(char),29))),_tmp942)))))))))))),((_tmp882[40]=((
_tmp948=_region_malloc(r,sizeof(*_tmp948)),((_tmp948->f1=((_tmp947="-c-comp",
_tag_dyneither(_tmp947,sizeof(char),8))),((_tmp948->f2=0,((_tmp948->f3=((_tmp946=" <compiler>",
_tag_dyneither(_tmp946,sizeof(char),12))),((_tmp948->f4=(void*)((_tmp944=
_region_malloc(r,sizeof(*_tmp944)),((_tmp944[0]=((_tmp945.tag=5,((_tmp945.f1=Cyc_set_c_compiler,
_tmp945)))),_tmp944)))),((_tmp948->f5=((_tmp943="Produce C output for the given compiler",
_tag_dyneither(_tmp943,sizeof(char),40))),_tmp948)))))))))))),((_tmp882[39]=((
_tmp94E=_region_malloc(r,sizeof(*_tmp94E)),((_tmp94E->f1=((_tmp94D="-un-gcc",
_tag_dyneither(_tmp94D,sizeof(char),8))),((_tmp94E->f2=0,((_tmp94E->f3=((_tmp94C="",
_tag_dyneither(_tmp94C,sizeof(char),1))),((_tmp94E->f4=(void*)((_tmp94A=
_region_malloc(r,sizeof(*_tmp94A)),((_tmp94A[0]=((_tmp94B.tag=0,((_tmp94B.f1=Cyc_set_tovc,
_tmp94B)))),_tmp94A)))),((_tmp94E->f5=((_tmp949="Avoid gcc extensions in C output (except for attributes)",
_tag_dyneither(_tmp949,sizeof(char),57))),_tmp94E)))))))))))),((_tmp882[38]=((
_tmp954=_region_malloc(r,sizeof(*_tmp954)),((_tmp954->f1=((_tmp953="-no-elim-statement-expressions",
_tag_dyneither(_tmp953,sizeof(char),31))),((_tmp954->f2=0,((_tmp954->f3=((
_tmp952="",_tag_dyneither(_tmp952,sizeof(char),1))),((_tmp954->f4=(void*)((
_tmp950=_region_malloc(r,sizeof(*_tmp950)),((_tmp950[0]=((_tmp951.tag=4,((
_tmp951.f1=& Cyc_elim_se_r,_tmp951)))),_tmp950)))),((_tmp954->f5=((_tmp94F="Do not avoid statement expressions in C output",
_tag_dyneither(_tmp94F,sizeof(char),47))),_tmp954)))))))))))),((_tmp882[37]=((
_tmp95A=_region_malloc(r,sizeof(*_tmp95A)),((_tmp95A->f1=((_tmp959="-elim-statement-expressions",
_tag_dyneither(_tmp959,sizeof(char),28))),((_tmp95A->f2=0,((_tmp95A->f3=((
_tmp958="",_tag_dyneither(_tmp958,sizeof(char),1))),((_tmp95A->f4=(void*)((
_tmp956=_region_malloc(r,sizeof(*_tmp956)),((_tmp956[0]=((_tmp957.tag=3,((
_tmp957.f1=& Cyc_elim_se_r,_tmp957)))),_tmp956)))),((_tmp95A->f5=((_tmp955="Avoid statement expressions in C output (implies --inline-checks)",
_tag_dyneither(_tmp955,sizeof(char),66))),_tmp95A)))))))))))),((_tmp882[36]=((
_tmp960=_region_malloc(r,sizeof(*_tmp960)),((_tmp960->f1=((_tmp95F="-up",
_tag_dyneither(_tmp95F,sizeof(char),4))),((_tmp960->f2=0,((_tmp960->f3=((_tmp95E="",
_tag_dyneither(_tmp95E,sizeof(char),1))),((_tmp960->f4=(void*)((_tmp95C=
_region_malloc(r,sizeof(*_tmp95C)),((_tmp95C[0]=((_tmp95D.tag=4,((_tmp95D.f1=&
Cyc_pp_r,_tmp95D)))),_tmp95C)))),((_tmp960->f5=((_tmp95B="Ugly print the C code that Cyclone generates (default)",
_tag_dyneither(_tmp95B,sizeof(char),55))),_tmp960)))))))))))),((_tmp882[35]=((
_tmp966=_region_malloc(r,sizeof(*_tmp966)),((_tmp966->f1=((_tmp965="-pp",
_tag_dyneither(_tmp965,sizeof(char),4))),((_tmp966->f2=0,((_tmp966->f3=((_tmp964="",
_tag_dyneither(_tmp964,sizeof(char),1))),((_tmp966->f4=(void*)((_tmp962=
_region_malloc(r,sizeof(*_tmp962)),((_tmp962[0]=((_tmp963.tag=3,((_tmp963.f1=&
Cyc_pp_r,_tmp963)))),_tmp962)))),((_tmp966->f5=((_tmp961="Pretty print the C code that Cyclone generates",
_tag_dyneither(_tmp961,sizeof(char),47))),_tmp966)))))))))))),((_tmp882[34]=((
_tmp96C=_region_malloc(r,sizeof(*_tmp96C)),((_tmp96C->f1=((_tmp96B="-ic",
_tag_dyneither(_tmp96B,sizeof(char),4))),((_tmp96C->f2=0,((_tmp96C->f3=((_tmp96A="",
_tag_dyneither(_tmp96A,sizeof(char),1))),((_tmp96C->f4=(void*)((_tmp968=
_region_malloc(r,sizeof(*_tmp968)),((_tmp968[0]=((_tmp969.tag=3,((_tmp969.f1=&
Cyc_ic_r,_tmp969)))),_tmp968)))),((_tmp96C->f5=((_tmp967="Activate the link-checker",
_tag_dyneither(_tmp967,sizeof(char),26))),_tmp96C)))))))))))),((_tmp882[33]=((
_tmp972=_region_malloc(r,sizeof(*_tmp972)),((_tmp972->f1=((_tmp971="-stopafter-toc",
_tag_dyneither(_tmp971,sizeof(char),15))),((_tmp972->f2=0,((_tmp972->f3=((
_tmp970="",_tag_dyneither(_tmp970,sizeof(char),1))),((_tmp972->f4=(void*)((
_tmp96E=_region_malloc(r,sizeof(*_tmp96E)),((_tmp96E[0]=((_tmp96F.tag=3,((
_tmp96F.f1=& Cyc_toc_r,_tmp96F)))),_tmp96E)))),((_tmp972->f5=((_tmp96D="Stop after translation to C",
_tag_dyneither(_tmp96D,sizeof(char),28))),_tmp972)))))))))))),((_tmp882[32]=((
_tmp978=_region_malloc(r,sizeof(*_tmp978)),((_tmp978->f1=((_tmp977="-stopafter-cf",
_tag_dyneither(_tmp977,sizeof(char),14))),((_tmp978->f2=0,((_tmp978->f3=((
_tmp976="",_tag_dyneither(_tmp976,sizeof(char),1))),((_tmp978->f4=(void*)((
_tmp974=_region_malloc(r,sizeof(*_tmp974)),((_tmp974[0]=((_tmp975.tag=3,((
_tmp975.f1=& Cyc_cf_r,_tmp975)))),_tmp974)))),((_tmp978->f5=((_tmp973="Stop after control-flow checking",
_tag_dyneither(_tmp973,sizeof(char),33))),_tmp978)))))))))))),((_tmp882[31]=((
_tmp97E=_region_malloc(r,sizeof(*_tmp97E)),((_tmp97E->f1=((_tmp97D="-noprint",
_tag_dyneither(_tmp97D,sizeof(char),9))),((_tmp97E->f2=0,((_tmp97E->f3=((_tmp97C="",
_tag_dyneither(_tmp97C,sizeof(char),1))),((_tmp97E->f4=(void*)((_tmp97A=
_region_malloc(r,sizeof(*_tmp97A)),((_tmp97A[0]=((_tmp97B.tag=3,((_tmp97B.f1=&
Cyc_noprint_r,_tmp97B)))),_tmp97A)))),((_tmp97E->f5=((_tmp979="Do not print output (when stopping early)",
_tag_dyneither(_tmp979,sizeof(char),42))),_tmp97E)))))))))))),((_tmp882[30]=((
_tmp984=_region_malloc(r,sizeof(*_tmp984)),((_tmp984->f1=((_tmp983="-stopafter-tc",
_tag_dyneither(_tmp983,sizeof(char),14))),((_tmp984->f2=0,((_tmp984->f3=((
_tmp982="",_tag_dyneither(_tmp982,sizeof(char),1))),((_tmp984->f4=(void*)((
_tmp980=_region_malloc(r,sizeof(*_tmp980)),((_tmp980[0]=((_tmp981.tag=3,((
_tmp981.f1=& Cyc_tc_r,_tmp981)))),_tmp980)))),((_tmp984->f5=((_tmp97F="Stop after type checking",
_tag_dyneither(_tmp97F,sizeof(char),25))),_tmp984)))))))))))),((_tmp882[29]=((
_tmp98A=_region_malloc(r,sizeof(*_tmp98A)),((_tmp98A->f1=((_tmp989="-stopafter-parse",
_tag_dyneither(_tmp989,sizeof(char),17))),((_tmp98A->f2=0,((_tmp98A->f3=((
_tmp988="",_tag_dyneither(_tmp988,sizeof(char),1))),((_tmp98A->f4=(void*)((
_tmp986=_region_malloc(r,sizeof(*_tmp986)),((_tmp986[0]=((_tmp987.tag=3,((
_tmp987.f1=& Cyc_parseonly_r,_tmp987)))),_tmp986)))),((_tmp98A->f5=((_tmp985="Stop after parsing",
_tag_dyneither(_tmp985,sizeof(char),19))),_tmp98A)))))))))))),((_tmp882[28]=((
_tmp990=_region_malloc(r,sizeof(*_tmp990)),((_tmp990->f1=((_tmp98F="-E",
_tag_dyneither(_tmp98F,sizeof(char),3))),((_tmp990->f2=0,((_tmp990->f3=((_tmp98E="",
_tag_dyneither(_tmp98E,sizeof(char),1))),((_tmp990->f4=(void*)((_tmp98C=
_region_malloc(r,sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98D.tag=3,((_tmp98D.f1=&
Cyc_stop_after_cpp_r,_tmp98D)))),_tmp98C)))),((_tmp990->f5=((_tmp98B="Stop after preprocessing",
_tag_dyneither(_tmp98B,sizeof(char),25))),_tmp990)))))))))))),((_tmp882[27]=((
_tmp996=_region_malloc(r,sizeof(*_tmp996)),((_tmp996->f1=((_tmp995="-Wall",
_tag_dyneither(_tmp995,sizeof(char),6))),((_tmp996->f2=0,((_tmp996->f3=((_tmp994="",
_tag_dyneither(_tmp994,sizeof(char),1))),((_tmp996->f4=(void*)((_tmp992=
_region_malloc(r,sizeof(*_tmp992)),((_tmp992[0]=((_tmp993.tag=0,((_tmp993.f1=Cyc_set_all_warnings,
_tmp993)))),_tmp992)))),((_tmp996->f5=((_tmp991="Turn on all warnings",
_tag_dyneither(_tmp991,sizeof(char),21))),_tmp996)))))))))))),((_tmp882[26]=((
_tmp99C=_region_malloc(r,sizeof(*_tmp99C)),((_tmp99C->f1=((_tmp99B="-Wlose-unique",
_tag_dyneither(_tmp99B,sizeof(char),14))),((_tmp99C->f2=0,((_tmp99C->f3=((
_tmp99A="",_tag_dyneither(_tmp99A,sizeof(char),1))),((_tmp99C->f4=(void*)((
_tmp998=_region_malloc(r,sizeof(*_tmp998)),((_tmp998[0]=((_tmp999.tag=3,((
_tmp999.f1=& Cyc_NewControlFlow_warn_lose_unique,_tmp999)))),_tmp998)))),((
_tmp99C->f5=((_tmp997="Try to warn when a unique pointer might get lost",
_tag_dyneither(_tmp997,sizeof(char),49))),_tmp99C)))))))))))),((_tmp882[25]=((
_tmp9A2=_region_malloc(r,sizeof(*_tmp9A2)),((_tmp9A2->f1=((_tmp9A1="-b",
_tag_dyneither(_tmp9A1,sizeof(char),3))),((_tmp9A2->f2=0,((_tmp9A2->f3=((_tmp9A0="<arch>",
_tag_dyneither(_tmp9A0,sizeof(char),7))),((_tmp9A2->f4=(void*)((_tmp99E=
_region_malloc(r,sizeof(*_tmp99E)),((_tmp99E[0]=((_tmp99F.tag=5,((_tmp99F.f1=Cyc_set_target_arch,
_tmp99F)))),_tmp99E)))),((_tmp9A2->f5=((_tmp99D="Set target architecture; implies -stopafter-toc",
_tag_dyneither(_tmp99D,sizeof(char),48))),_tmp9A2)))))))))))),((_tmp882[24]=((
_tmp9A8=_region_malloc(r,sizeof(*_tmp9A8)),((_tmp9A8->f1=((_tmp9A7="-MT",
_tag_dyneither(_tmp9A7,sizeof(char),4))),((_tmp9A8->f2=0,((_tmp9A8->f3=((_tmp9A6=" <target>",
_tag_dyneither(_tmp9A6,sizeof(char),10))),((_tmp9A8->f4=(void*)((_tmp9A4=
_region_malloc(r,sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=5,((_tmp9A5.f1=Cyc_set_dependencies_target,
_tmp9A5)))),_tmp9A4)))),((_tmp9A8->f5=((_tmp9A3="Give target for dependencies",
_tag_dyneither(_tmp9A3,sizeof(char),29))),_tmp9A8)))))))))))),((_tmp882[23]=((
_tmp9AE=_region_malloc(r,sizeof(*_tmp9AE)),((_tmp9AE->f1=((_tmp9AD="-MG",
_tag_dyneither(_tmp9AD,sizeof(char),4))),((_tmp9AE->f2=0,((_tmp9AE->f3=((_tmp9AC="",
_tag_dyneither(_tmp9AC,sizeof(char),1))),((_tmp9AE->f4=(void*)((_tmp9AA=
_region_malloc(r,sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AB.tag=1,((_tmp9AB.f1=Cyc_add_cpparg,
_tmp9AB)))),_tmp9AA)))),((_tmp9AE->f5=((_tmp9A9="When producing dependencies assume that missing files are generated",
_tag_dyneither(_tmp9A9,sizeof(char),68))),_tmp9AE)))))))))))),((_tmp882[22]=((
_tmp9B4=_region_malloc(r,sizeof(*_tmp9B4)),((_tmp9B4->f1=((_tmp9B3="-M",
_tag_dyneither(_tmp9B3,sizeof(char),3))),((_tmp9B4->f2=0,((_tmp9B4->f3=((_tmp9B2="",
_tag_dyneither(_tmp9B2,sizeof(char),1))),((_tmp9B4->f4=(void*)((_tmp9B0=
_region_malloc(r,sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=0,((_tmp9B1.f1=Cyc_set_produce_dependencies,
_tmp9B1)))),_tmp9B0)))),((_tmp9B4->f5=((_tmp9AF="Produce dependencies",
_tag_dyneither(_tmp9AF,sizeof(char),21))),_tmp9B4)))))))))))),((_tmp882[21]=((
_tmp9BA=_region_malloc(r,sizeof(*_tmp9BA)),((_tmp9BA->f1=((_tmp9B9="-S",
_tag_dyneither(_tmp9B9,sizeof(char),3))),((_tmp9BA->f2=0,((_tmp9BA->f3=((_tmp9B8="",
_tag_dyneither(_tmp9B8,sizeof(char),1))),((_tmp9BA->f4=(void*)((_tmp9B6=
_region_malloc(r,sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B7.tag=0,((_tmp9B7.f1=Cyc_set_stop_after_asmfile,
_tmp9B7)))),_tmp9B6)))),((_tmp9BA->f5=((_tmp9B5="Stop after producing assembly code",
_tag_dyneither(_tmp9B5,sizeof(char),35))),_tmp9BA)))))))))))),((_tmp882[20]=((
_tmp9C0=_region_malloc(r,sizeof(*_tmp9C0)),((_tmp9C0->f1=((_tmp9BF="-pa",
_tag_dyneither(_tmp9BF,sizeof(char),4))),((_tmp9C0->f2=0,((_tmp9C0->f3=((_tmp9BE="",
_tag_dyneither(_tmp9BE,sizeof(char),1))),((_tmp9C0->f4=(void*)((_tmp9BC=
_region_malloc(r,sizeof(*_tmp9BC)),((_tmp9BC[0]=((_tmp9BD.tag=0,((_tmp9BD.f1=Cyc_set_pa,
_tmp9BD)))),_tmp9BC)))),((_tmp9C0->f5=((_tmp9BB="Compile for profiling with aprof",
_tag_dyneither(_tmp9BB,sizeof(char),33))),_tmp9C0)))))))))))),((_tmp882[19]=((
_tmp9C6=_region_malloc(r,sizeof(*_tmp9C6)),((_tmp9C6->f1=((_tmp9C5="-pg",
_tag_dyneither(_tmp9C5,sizeof(char),4))),((_tmp9C6->f2=0,((_tmp9C6->f3=((_tmp9C4="",
_tag_dyneither(_tmp9C4,sizeof(char),1))),((_tmp9C6->f4=(void*)((_tmp9C2=
_region_malloc(r,sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C3.tag=0,((_tmp9C3.f1=Cyc_set_pg,
_tmp9C3)))),_tmp9C2)))),((_tmp9C6->f5=((_tmp9C1="Compile for profiling with gprof",
_tag_dyneither(_tmp9C1,sizeof(char),33))),_tmp9C6)))))))))))),((_tmp882[18]=((
_tmp9CC=_region_malloc(r,sizeof(*_tmp9CC)),((_tmp9CC->f1=((_tmp9CB="-p",
_tag_dyneither(_tmp9CB,sizeof(char),3))),((_tmp9CC->f2=0,((_tmp9CC->f3=((_tmp9CA="",
_tag_dyneither(_tmp9CA,sizeof(char),1))),((_tmp9CC->f4=(void*)((_tmp9C8=
_region_malloc(r,sizeof(*_tmp9C8)),((_tmp9C8[0]=((_tmp9C9.tag=1,((_tmp9C9.f1=Cyc_add_ccarg,
_tmp9C9)))),_tmp9C8)))),((_tmp9CC->f5=((_tmp9C7="Compile for profiling with prof",
_tag_dyneither(_tmp9C7,sizeof(char),32))),_tmp9CC)))))))))))),((_tmp882[17]=((
_tmp9D2=_region_malloc(r,sizeof(*_tmp9D2)),((_tmp9D2->f1=((_tmp9D1="-g",
_tag_dyneither(_tmp9D1,sizeof(char),3))),((_tmp9D2->f2=0,((_tmp9D2->f3=((_tmp9D0="",
_tag_dyneither(_tmp9D0,sizeof(char),1))),((_tmp9D2->f4=(void*)((_tmp9CE=
_region_malloc(r,sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9CF.tag=1,((_tmp9CF.f1=Cyc_add_ccarg,
_tmp9CF)))),_tmp9CE)))),((_tmp9D2->f5=((_tmp9CD="Compile for debugging",
_tag_dyneither(_tmp9CD,sizeof(char),22))),_tmp9D2)))))))))))),((_tmp882[16]=((
_tmp9D8=_region_malloc(r,sizeof(*_tmp9D8)),((_tmp9D8->f1=((_tmp9D7="-fomit-frame-pointer",
_tag_dyneither(_tmp9D7,sizeof(char),21))),((_tmp9D8->f2=0,((_tmp9D8->f3=((
_tmp9D6="",_tag_dyneither(_tmp9D6,sizeof(char),1))),((_tmp9D8->f4=(void*)((
_tmp9D4=_region_malloc(r,sizeof(*_tmp9D4)),((_tmp9D4[0]=((_tmp9D5.tag=1,((
_tmp9D5.f1=Cyc_add_ccarg,_tmp9D5)))),_tmp9D4)))),((_tmp9D8->f5=((_tmp9D3="Omit frame pointer",
_tag_dyneither(_tmp9D3,sizeof(char),19))),_tmp9D8)))))))))))),((_tmp882[15]=((
_tmp9DE=_region_malloc(r,sizeof(*_tmp9DE)),((_tmp9DE->f1=((_tmp9DD="-O3",
_tag_dyneither(_tmp9DD,sizeof(char),4))),((_tmp9DE->f2=0,((_tmp9DE->f3=((_tmp9DC="",
_tag_dyneither(_tmp9DC,sizeof(char),1))),((_tmp9DE->f4=(void*)((_tmp9DA=
_region_malloc(r,sizeof(*_tmp9DA)),((_tmp9DA[0]=((_tmp9DB.tag=1,((_tmp9DB.f1=Cyc_add_ccarg,
_tmp9DB)))),_tmp9DA)))),((_tmp9DE->f5=((_tmp9D9="Even more optimization",
_tag_dyneither(_tmp9D9,sizeof(char),23))),_tmp9DE)))))))))))),((_tmp882[14]=((
_tmp9E4=_region_malloc(r,sizeof(*_tmp9E4)),((_tmp9E4->f1=((_tmp9E3="-O2",
_tag_dyneither(_tmp9E3,sizeof(char),4))),((_tmp9E4->f2=0,((_tmp9E4->f3=((_tmp9E2="",
_tag_dyneither(_tmp9E2,sizeof(char),1))),((_tmp9E4->f4=(void*)((_tmp9E0=
_region_malloc(r,sizeof(*_tmp9E0)),((_tmp9E0[0]=((_tmp9E1.tag=1,((_tmp9E1.f1=Cyc_add_ccarg,
_tmp9E1)))),_tmp9E0)))),((_tmp9E4->f5=((_tmp9DF="A higher level of optimization",
_tag_dyneither(_tmp9DF,sizeof(char),31))),_tmp9E4)))))))))))),((_tmp882[13]=((
_tmp9EA=_region_malloc(r,sizeof(*_tmp9EA)),((_tmp9EA->f1=((_tmp9E9="-O",
_tag_dyneither(_tmp9E9,sizeof(char),3))),((_tmp9EA->f2=0,((_tmp9EA->f3=((_tmp9E8="",
_tag_dyneither(_tmp9E8,sizeof(char),1))),((_tmp9EA->f4=(void*)((_tmp9E6=
_region_malloc(r,sizeof(*_tmp9E6)),((_tmp9E6[0]=((_tmp9E7.tag=1,((_tmp9E7.f1=Cyc_add_ccarg,
_tmp9E7)))),_tmp9E6)))),((_tmp9EA->f5=((_tmp9E5="Optimize",_tag_dyneither(
_tmp9E5,sizeof(char),9))),_tmp9EA)))))))))))),((_tmp882[12]=((_tmp9F0=
_region_malloc(r,sizeof(*_tmp9F0)),((_tmp9F0->f1=((_tmp9EF="-O0",_tag_dyneither(
_tmp9EF,sizeof(char),4))),((_tmp9F0->f2=0,((_tmp9F0->f3=((_tmp9EE="",
_tag_dyneither(_tmp9EE,sizeof(char),1))),((_tmp9F0->f4=(void*)((_tmp9EC=
_region_malloc(r,sizeof(*_tmp9EC)),((_tmp9EC[0]=((_tmp9ED.tag=1,((_tmp9ED.f1=Cyc_add_ccarg,
_tmp9ED)))),_tmp9EC)))),((_tmp9F0->f5=((_tmp9EB="Don't optimize",_tag_dyneither(
_tmp9EB,sizeof(char),15))),_tmp9F0)))))))))))),((_tmp882[11]=((_tmp9F6=
_region_malloc(r,sizeof(*_tmp9F6)),((_tmp9F6->f1=((_tmp9F5="-s",_tag_dyneither(
_tmp9F5,sizeof(char),3))),((_tmp9F6->f2=0,((_tmp9F6->f3=((_tmp9F4="",
_tag_dyneither(_tmp9F4,sizeof(char),1))),((_tmp9F6->f4=(void*)((_tmp9F2=
_region_malloc(r,sizeof(*_tmp9F2)),((_tmp9F2[0]=((_tmp9F3.tag=1,((_tmp9F3.f1=Cyc_add_ccarg,
_tmp9F3)))),_tmp9F2)))),((_tmp9F6->f5=((_tmp9F1="Remove all symbol table and relocation info from executable",
_tag_dyneither(_tmp9F1,sizeof(char),60))),_tmp9F6)))))))))))),((_tmp882[10]=((
_tmp9FC=_region_malloc(r,sizeof(*_tmp9FC)),((_tmp9FC->f1=((_tmp9FB="-x",
_tag_dyneither(_tmp9FB,sizeof(char),3))),((_tmp9FC->f2=0,((_tmp9FC->f3=((_tmp9FA=" <language>",
_tag_dyneither(_tmp9FA,sizeof(char),12))),((_tmp9FC->f4=(void*)((_tmp9F8=
_region_malloc(r,sizeof(*_tmp9F8)),((_tmp9F8[0]=((_tmp9F9.tag=5,((_tmp9F9.f1=Cyc_set_inputtype,
_tmp9F9)))),_tmp9F8)))),((_tmp9FC->f5=((_tmp9F7="Specify <language> for the following input files",
_tag_dyneither(_tmp9F7,sizeof(char),49))),_tmp9FC)))))))))))),((_tmp882[9]=((
_tmpA02=_region_malloc(r,sizeof(*_tmpA02)),((_tmpA02->f1=((_tmpA01="-c",
_tag_dyneither(_tmpA01,sizeof(char),3))),((_tmpA02->f2=0,((_tmpA02->f3=((_tmpA00="",
_tag_dyneither(_tmpA00,sizeof(char),1))),((_tmpA02->f4=(void*)((_tmp9FE=
_region_malloc(r,sizeof(*_tmp9FE)),((_tmp9FE[0]=((_tmp9FF.tag=0,((_tmp9FF.f1=Cyc_set_stop_after_objectfile,
_tmp9FF)))),_tmp9FE)))),((_tmpA02->f5=((_tmp9FD="Produce an object file instead of an executable; do not link",
_tag_dyneither(_tmp9FD,sizeof(char),61))),_tmpA02)))))))))))),((_tmp882[8]=((
_tmpA08=_region_malloc(r,sizeof(*_tmpA08)),((_tmpA08->f1=((_tmpA07="-m",
_tag_dyneither(_tmpA07,sizeof(char),3))),((_tmpA08->f2=1,((_tmpA08->f3=((_tmpA06="<option>",
_tag_dyneither(_tmpA06,sizeof(char),9))),((_tmpA08->f4=(void*)((_tmpA04=
_region_malloc(r,sizeof(*_tmpA04)),((_tmpA04[0]=((_tmpA05.tag=1,((_tmpA05.f1=Cyc_add_marg,
_tmpA05)))),_tmpA04)))),((_tmpA08->f5=((_tmpA03="GCC specific: pass machine-dependent flag on to gcc",
_tag_dyneither(_tmpA03,sizeof(char),52))),_tmpA08)))))))))))),((_tmp882[7]=((
_tmpA0E=_region_malloc(r,sizeof(*_tmpA0E)),((_tmpA0E->f1=((_tmpA0D="-l",
_tag_dyneither(_tmpA0D,sizeof(char),3))),((_tmpA0E->f2=1,((_tmpA0E->f3=((_tmpA0C="<libname>",
_tag_dyneither(_tmpA0C,sizeof(char),10))),((_tmpA0E->f4=(void*)((_tmpA0A=
_region_malloc(r,sizeof(*_tmpA0A)),((_tmpA0A[0]=((_tmpA0B.tag=1,((_tmpA0B.f1=Cyc_add_libarg,
_tmpA0B)))),_tmpA0A)))),((_tmpA0E->f5=((_tmpA09="Library file",_tag_dyneither(
_tmpA09,sizeof(char),13))),_tmpA0E)))))))))))),((_tmp882[6]=((_tmpA14=
_region_malloc(r,sizeof(*_tmpA14)),((_tmpA14->f1=((_tmpA13="-L",_tag_dyneither(
_tmpA13,sizeof(char),3))),((_tmpA14->f2=1,((_tmpA14->f3=((_tmpA12="<dir>",
_tag_dyneither(_tmpA12,sizeof(char),6))),((_tmpA14->f4=(void*)((_tmpA10=
_region_malloc(r,sizeof(*_tmpA10)),((_tmpA10[0]=((_tmpA11.tag=1,((_tmpA11.f1=Cyc_add_ccarg,
_tmpA11)))),_tmpA10)))),((_tmpA14->f5=((_tmpA0F="Add to the list of directories for -l",
_tag_dyneither(_tmpA0F,sizeof(char),38))),_tmpA14)))))))))))),((_tmp882[5]=((
_tmpA1A=_region_malloc(r,sizeof(*_tmpA1A)),((_tmpA1A->f1=((_tmpA19="-I",
_tag_dyneither(_tmpA19,sizeof(char),3))),((_tmpA1A->f2=1,((_tmpA1A->f3=((_tmpA18="<dir>",
_tag_dyneither(_tmpA18,sizeof(char),6))),((_tmpA1A->f4=(void*)((_tmpA16=
_region_malloc(r,sizeof(*_tmpA16)),((_tmpA16[0]=((_tmpA17.tag=1,((_tmpA17.f1=Cyc_add_cpparg,
_tmpA17)))),_tmpA16)))),((_tmpA1A->f5=((_tmpA15="Add to the list of directories to search for include files",
_tag_dyneither(_tmpA15,sizeof(char),59))),_tmpA1A)))))))))))),((_tmp882[4]=((
_tmpA20=_region_malloc(r,sizeof(*_tmpA20)),((_tmpA20->f1=((_tmpA1F="-B",
_tag_dyneither(_tmpA1F,sizeof(char),3))),((_tmpA20->f2=1,((_tmpA20->f3=((_tmpA1E="<file>",
_tag_dyneither(_tmpA1E,sizeof(char),7))),((_tmpA20->f4=(void*)((_tmpA1C=
_region_malloc(r,sizeof(*_tmpA1C)),((_tmpA1C[0]=((_tmpA1D.tag=1,((_tmpA1D.f1=Cyc_add_cyclone_exec_path,
_tmpA1D)))),_tmpA1C)))),((_tmpA20->f5=((_tmpA1B="Add to the list of directories to search for compiler files",
_tag_dyneither(_tmpA1B,sizeof(char),60))),_tmpA20)))))))))))),((_tmp882[3]=((
_tmpA26=_region_malloc(r,sizeof(*_tmpA26)),((_tmpA26->f1=((_tmpA25="-D",
_tag_dyneither(_tmpA25,sizeof(char),3))),((_tmpA26->f2=1,((_tmpA26->f3=((_tmpA24="<name>[=<value>]",
_tag_dyneither(_tmpA24,sizeof(char),17))),((_tmpA26->f4=(void*)((_tmpA22=
_region_malloc(r,sizeof(*_tmpA22)),((_tmpA22[0]=((_tmpA23.tag=1,((_tmpA23.f1=Cyc_add_cpparg,
_tmpA23)))),_tmpA22)))),((_tmpA26->f5=((_tmpA21="Pass definition to preprocessor",
_tag_dyneither(_tmpA21,sizeof(char),32))),_tmpA26)))))))))))),((_tmp882[2]=((
_tmpA2C=_region_malloc(r,sizeof(*_tmpA2C)),((_tmpA2C->f1=((_tmpA2B="-o",
_tag_dyneither(_tmpA2B,sizeof(char),3))),((_tmpA2C->f2=0,((_tmpA2C->f3=((_tmpA2A=" <file>",
_tag_dyneither(_tmpA2A,sizeof(char),8))),((_tmpA2C->f4=(void*)((_tmpA28=
_region_malloc(r,sizeof(*_tmpA28)),((_tmpA28[0]=((_tmpA29.tag=5,((_tmpA29.f1=Cyc_set_output_file,
_tmpA29)))),_tmpA28)))),((_tmpA2C->f5=((_tmpA27="Set the output file name to <file>",
_tag_dyneither(_tmpA27,sizeof(char),35))),_tmpA2C)))))))))))),((_tmp882[1]=((
_tmpA32=_region_malloc(r,sizeof(*_tmpA32)),((_tmpA32->f1=((_tmpA31="--version",
_tag_dyneither(_tmpA31,sizeof(char),10))),((_tmpA32->f2=0,((_tmpA32->f3=((
_tmpA30="",_tag_dyneither(_tmpA30,sizeof(char),1))),((_tmpA32->f4=(void*)((
_tmpA2E=_region_malloc(r,sizeof(*_tmpA2E)),((_tmpA2E[0]=((_tmpA2F.tag=0,((
_tmpA2F.f1=Cyc_print_version,_tmpA2F)))),_tmpA2E)))),((_tmpA32->f5=((_tmpA2D="Print version information and exit",
_tag_dyneither(_tmpA2D,sizeof(char),35))),_tmpA32)))))))))))),((_tmp882[0]=((
_tmpA38=_region_malloc(r,sizeof(*_tmpA38)),((_tmpA38->f1=((_tmpA37="-v",
_tag_dyneither(_tmpA37,sizeof(char),3))),((_tmpA38->f2=0,((_tmpA38->f3=((_tmpA36="",
_tag_dyneither(_tmpA36,sizeof(char),1))),((_tmpA38->f4=(void*)((_tmpA34=
_region_malloc(r,sizeof(*_tmpA34)),((_tmpA34[0]=((_tmpA35.tag=3,((_tmpA35.f1=&
Cyc_v_r,_tmpA35)))),_tmpA34)))),((_tmpA38->f5=((_tmpA33="Print compilation stages verbosely",
_tag_dyneither(_tmpA33,sizeof(char),35))),_tmpA38)))))))))))),((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(
_tmp882,sizeof(struct _tuple23*),73)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
const char*_tmpA39;struct _dyneither_ptr optstring=(_tmpA39="Options:",
_tag_dyneither(_tmpA39,sizeof(char),9));Cyc_Arg_current=0;Cyc_Arg_parse(options,
Cyc_add_other,optstring,_tmp166);if((char*)Cyc_target_arch.curr == (char*)(
_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)
Cstring_to_string(Carch);{char*_tmpA3A;struct _dyneither_ptr cyclone_exec_prefix=(
_tmpA3A=getenv((const char*)"CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmpA3A,sizeof(
char),_get_zero_arr_size_char((void*)_tmpA3A,1)));if((char*)cyclone_exec_prefix.curr
!= (char*)(_tag_dyneither(0,0,0)).curr){struct _dyneither_ptr*_tmpA3D;struct Cyc_List_List*
_tmpA3C;Cyc_cyclone_exec_path=((_tmpA3C=_cycalloc(sizeof(*_tmpA3C)),((_tmpA3C->hd=((
_tmpA3D=_cycalloc(sizeof(*_tmpA3D)),((_tmpA3D[0]=cyclone_exec_prefix,_tmpA3D)))),((
_tmpA3C->tl=Cyc_cyclone_exec_path,_tmpA3C))))));}def_lib_path=(struct
_dyneither_ptr)Cstring_to_string(Cdef_lib_path);if(Cyc_strlen((struct
_dyneither_ptr)def_lib_path)> 0){struct _dyneither_ptr*_tmpA43;const char*_tmpA42;
struct Cyc_List_List*_tmpA41;Cyc_cyclone_exec_path=((_tmpA41=_cycalloc(sizeof(*
_tmpA41)),((_tmpA41->hd=((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43[0]=(
struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmpA42="cyc-lib",
_tag_dyneither(_tmpA42,sizeof(char),8)))),_tmpA43)))),((_tmpA41->tl=Cyc_cyclone_exec_path,
_tmpA41))))));}Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);cyclone_arch_path=Cyc_also_subdir(
Cyc_cyclone_exec_path,Cyc_target_arch);{const char*_tmpA44;struct _dyneither_ptr
_tmp16D=Cyc_do_find(cyclone_arch_path,((_tmpA44="cycspecs",_tag_dyneither(
_tmpA44,sizeof(char),9))));if(Cyc_v_r){const char*_tmpA48;void*_tmpA47[1];struct
Cyc_String_pa_struct _tmpA46;(_tmpA46.tag=0,((_tmpA46.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp16D),((_tmpA47[0]=& _tmpA46,Cyc_fprintf(Cyc_stderr,((
_tmpA48="Reading from specs file %s\n",_tag_dyneither(_tmpA48,sizeof(char),28))),
_tag_dyneither(_tmpA47,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp171=Cyc_read_specs(
_tmp16D);struct _dyneither_ptr _tmp172=_tag_dyneither(0,0,0);{struct _handler_cons
_tmp173;_push_handler(& _tmp173);{int _tmp175=0;if(setjmp(_tmp173.handler))_tmp175=
1;if(!_tmp175){{struct _dyneither_ptr _tmp176=*((struct _dyneither_ptr*(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp171,_init_dyneither_ptr(
_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));_tmp172=Cyc_split_specs(
_tmp176);};_pop_handler();}else{void*_tmp174=(void*)_exn_thrown;void*_tmp179=
_tmp174;_LL35: {struct Cyc_Core_Not_found_struct*_tmp17A=(struct Cyc_Core_Not_found_struct*)
_tmp179;if(_tmp17A->tag != Cyc_Core_Not_found)goto _LL37;}_LL36: goto _LL34;_LL37:;
_LL38:(void)_throw(_tmp179);_LL34:;}};}if((struct _dyneither_ptr*)_tmp172.curr != (
struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp172);}Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp167);{const char*_tmpA49;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_cpp,((_tmpA49="",_tag_dyneither(_tmpA49,sizeof(char),1))))
== 0){const char*_tmp17C=Cyc_produce_dependencies?"":(const char*)" -E";const char*
_tmpA51;void*_tmpA50[3];struct Cyc_String_pa_struct _tmpA4F;const char*_tmpA4E;
struct Cyc_String_pa_struct _tmpA4D;struct Cyc_String_pa_struct _tmpA4C;Cyc_set_cpp((
struct _dyneither_ptr)((_tmpA4C.tag=0,((_tmpA4C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp16D),((_tmpA4D.tag=0,((_tmpA4D.f1=(struct _dyneither_ptr)((
_tmpA4E=_tmp17C,_tag_dyneither(_tmpA4E,sizeof(char),_get_zero_arr_size_char((
void*)_tmpA4E,1)))),((_tmpA4F.tag=0,((_tmpA4F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)comp),((_tmpA50[0]=& _tmpA4F,((_tmpA50[1]=& _tmpA4D,((_tmpA50[2]=&
_tmpA4C,Cyc_aprintf(((_tmpA51="%s -w -x c%s -specs %s",_tag_dyneither(_tmpA51,
sizeof(char),23))),_tag_dyneither(_tmpA50,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){{const char*_tmpA54;void*_tmpA53;(
_tmpA53=0,Cyc_fprintf(Cyc_stderr,((_tmpA54="missing file\n",_tag_dyneither(
_tmpA54,sizeof(char),14))),_tag_dyneither(_tmpA53,sizeof(void*),0)));}exit(1);}
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){const char*_tmpA55;Cyc_cyc_setjmp=
Cyc_do_find(cyclone_arch_path,((_tmpA55="cyc_setjmp.h",_tag_dyneither(_tmpA55,
sizeof(char),13))));}if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct
_dyneither_ptr)Cyc_cyc_include)== 0){const char*_tmpA56;Cyc_cyc_include=Cyc_do_find(
Cyc_cyclone_exec_path,((_tmpA56="cyc_include.h",_tag_dyneither(_tmpA56,sizeof(
char),14))));}{struct Cyc_List_List*_tmp34D=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp34D != 0;_tmp34D=_tmp34D->tl){Cyc_process_file(*((
struct _dyneither_ptr*)_tmp34D->hd));if(Cyc_compile_failure){int _tmp34E=1;
_npop_handler(0);return _tmp34E;}}}if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r)
 || Cyc_tc_r) || Cyc_toc_r){int _tmp34F=0;_npop_handler(0);return _tmp34F;}if(Cyc_ccargs
== 0){int _tmp350=0;_npop_handler(0);return _tmp350;}{const char*_tmpA57;Cyc_add_ccarg((
struct _dyneither_ptr)Cyc_strconcat(((_tmpA57="-L",_tag_dyneither(_tmpA57,sizeof(
char),3))),(struct _dyneither_ptr)def_lib_path));}Cyc_ccargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{const char*_tmpA58;struct
_dyneither_ptr _tmp352=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct
_dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpA58=" ",_tag_dyneither(_tmpA58,sizeof(
char),2))));Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_libargs);{const char*_tmpA5A;struct Cyc_List_List*_tmpA59;struct _dyneither_ptr
_tmp353=Cyc_str_sepstr(((_tmpA59=_cycalloc(sizeof(*_tmpA59)),((_tmpA59->hd=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((
_tmpA59->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_libargs),_tmpA59)))))),((_tmpA5A=" ",_tag_dyneither(_tmpA5A,sizeof(char),2))));
struct Cyc_List_List*stdlib;struct _dyneither_ptr stdlib_string;const char*_tmpA5C;
const char*_tmpA5B;int _tmp354=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r)
 || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)
_check_null(Cyc_output_file)),((_tmpA5C=".a",_tag_dyneither(_tmpA5C,sizeof(char),
3))))) || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpA5B=".lib",_tag_dyneither(
_tmpA5B,sizeof(char),5))));if(_tmp354){stdlib=0;{const char*_tmpA5D;stdlib_string=(
struct _dyneither_ptr)((_tmpA5D="",_tag_dyneither(_tmpA5D,sizeof(char),1)));};}
else{struct _dyneither_ptr libcyc_flag;struct _dyneither_ptr nogc_filename;struct
_dyneither_ptr runtime_filename;struct _dyneither_ptr gc_filename;if(Cyc_pa_r){{
const char*_tmpA5E;libcyc_flag=((_tmpA5E="-lcyc_a",_tag_dyneither(_tmpA5E,sizeof(
char),8)));}{const char*_tmpA5F;nogc_filename=((_tmpA5F="nogc_a.a",_tag_dyneither(
_tmpA5F,sizeof(char),9)));}{const char*_tmpA60;runtime_filename=((_tmpA60="runtime_cyc_a.o",
_tag_dyneither(_tmpA60,sizeof(char),16)));};}else{if(Cyc_nocheck_r){{const char*
_tmpA61;libcyc_flag=((_tmpA61="-lcyc_nocheck",_tag_dyneither(_tmpA61,sizeof(char),
14)));}{const char*_tmpA62;nogc_filename=((_tmpA62="nogc.a",_tag_dyneither(
_tmpA62,sizeof(char),7)));}{const char*_tmpA63;runtime_filename=((_tmpA63="runtime_cyc.o",
_tag_dyneither(_tmpA63,sizeof(char),14)));};}else{if(Cyc_pg_r){{const char*
_tmpA64;libcyc_flag=((_tmpA64="-lcyc_pg",_tag_dyneither(_tmpA64,sizeof(char),9)));}{
const char*_tmpA65;runtime_filename=((_tmpA65="runtime_cyc_pg.o",_tag_dyneither(
_tmpA65,sizeof(char),17)));}{const char*_tmpA66;nogc_filename=((_tmpA66="nogc_pg.a",
_tag_dyneither(_tmpA66,sizeof(char),10)));};}else{if(Cyc_compile_for_boot_r){{
const char*_tmpA67;libcyc_flag=((_tmpA67="-lcycboot",_tag_dyneither(_tmpA67,
sizeof(char),10)));}{const char*_tmpA68;nogc_filename=((_tmpA68="nogc.a",
_tag_dyneither(_tmpA68,sizeof(char),7)));}{const char*_tmpA69;runtime_filename=((
_tmpA69="runtime_cyc.o",_tag_dyneither(_tmpA69,sizeof(char),14)));};}else{{const
char*_tmpA6A;libcyc_flag=((_tmpA6A="-lcyc",_tag_dyneither(_tmpA6A,sizeof(char),6)));}{
const char*_tmpA6B;nogc_filename=((_tmpA6B="nogc.a",_tag_dyneither(_tmpA6B,
sizeof(char),7)));}{const char*_tmpA6C;runtime_filename=((_tmpA6C="runtime_cyc.o",
_tag_dyneither(_tmpA6C,sizeof(char),14)));};}}}}{const char*_tmpA6D;gc_filename=((
_tmpA6D="gc.a",_tag_dyneither(_tmpA6D,sizeof(char),5)));}{struct _dyneither_ptr
_tmp366=Cyc_nogc_r?Cyc_do_find(cyclone_arch_path,nogc_filename): Cyc_do_find(
cyclone_arch_path,gc_filename);struct _dyneither_ptr _tmp367=Cyc_do_find(
cyclone_arch_path,runtime_filename);stdlib=0;{const char*_tmpA73;void*_tmpA72[3];
struct Cyc_String_pa_struct _tmpA71;struct Cyc_String_pa_struct _tmpA70;struct Cyc_String_pa_struct
_tmpA6F;stdlib_string=(struct _dyneither_ptr)((_tmpA6F.tag=0,((_tmpA6F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp366),((_tmpA70.tag=0,((_tmpA70.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpA71.tag=0,((
_tmpA71.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp367),((_tmpA72[0]=&
_tmpA71,((_tmpA72[1]=& _tmpA70,((_tmpA72[2]=& _tmpA6F,Cyc_aprintf(((_tmpA73=" %s %s %s",
_tag_dyneither(_tmpA73,sizeof(char),10))),_tag_dyneither(_tmpA72,sizeof(void*),3))))))))))))))))))));};};}
if(Cyc_ic_r){struct _handler_cons _tmp36D;_push_handler(& _tmp36D);{int _tmp36F=0;
if(setjmp(_tmp36D.handler))_tmp36F=1;if(!_tmp36F){Cyc_ccargs=((struct Cyc_List_List*(*)(
int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,
Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*
_tmp370=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(!_tmp354){struct
Cyc_List_List*_tmpA74;_tmp370=((_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74->hd=
Cyc_final_strptr,((_tmpA74->tl=_tmp370,_tmpA74))))));}{struct Cyc_Interface_I*
_tmp372=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp370,_tmp370);if(_tmp372 == 0){{const char*_tmpA77;void*_tmpA76;(
_tmpA76=0,Cyc_fprintf(Cyc_stderr,((_tmpA77="Error: interfaces incompatible\n",
_tag_dyneither(_tmpA77,sizeof(char),32))),_tag_dyneither(_tmpA76,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp375=1;_npop_handler(1);return
_tmp375;};}if(_tmp354){if(Cyc_output_file != 0){const char*_tmpA7B;void*_tmpA7A[1];
struct Cyc_String_pa_struct _tmpA79;struct _dyneither_ptr _tmp376=(_tmpA79.tag=0,((
_tmpA79.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpA7A[0]=& _tmpA79,Cyc_aprintf(((
_tmpA7B="%s.cycio",_tag_dyneither(_tmpA7B,sizeof(char),9))),_tag_dyneither(
_tmpA7A,sizeof(void*),1)))))));const char*_tmpA7D;const char*_tmpA7C;struct Cyc___cycFILE*
_tmp377=Cyc_try_file_open((struct _dyneither_ptr)_tmp376,((_tmpA7C="wb",
_tag_dyneither(_tmpA7C,sizeof(char),3))),((_tmpA7D="interface object file",
_tag_dyneither(_tmpA7D,sizeof(char),22))));if(_tmp377 == 0){Cyc_compile_failure=1;
Cyc_remove_cfiles();{int _tmp378=1;_npop_handler(1);return _tmp378;};}Cyc_Interface_save((
struct Cyc_Interface_I*)_tmp372,(struct Cyc___cycFILE*)_tmp377);Cyc_file_close((
struct Cyc___cycFILE*)_tmp377);}}else{const char*_tmpA82;const char*_tmpA81;struct
_tuple19*_tmpA80;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp372,((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80->f1=((
_tmpA81="empty interface",_tag_dyneither(_tmpA81,sizeof(char),16))),((_tmpA80->f2=((
_tmpA82="global interface",_tag_dyneither(_tmpA82,sizeof(char),17))),_tmpA80)))))))){{
const char*_tmpA85;void*_tmpA84;(_tmpA84=0,Cyc_fprintf(Cyc_stderr,((_tmpA85="Error: some objects are still undefined\n",
_tag_dyneither(_tmpA85,sizeof(char),41))),_tag_dyneither(_tmpA84,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp383=1;_npop_handler(1);return
_tmp383;};}}};};;_pop_handler();}else{void*_tmp36E=(void*)_exn_thrown;void*
_tmp385=_tmp36E;_LL3A:;_LL3B:{void*_tmp386=_tmp385;struct _dyneither_ptr _tmp388;
struct _dyneither_ptr _tmp38A;struct _dyneither_ptr _tmp38D;_LL3F: {struct Cyc_Core_Failure_struct*
_tmp387=(struct Cyc_Core_Failure_struct*)_tmp386;if(_tmp387->tag != Cyc_Core_Failure)
goto _LL41;else{_tmp388=_tmp387->f1;}}_LL40:{const char*_tmpA89;void*_tmpA88[1];
struct Cyc_String_pa_struct _tmpA87;(_tmpA87.tag=0,((_tmpA87.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp388),((_tmpA88[0]=& _tmpA87,Cyc_fprintf(
Cyc_stderr,((_tmpA89="Exception Core::Failure %s\n",_tag_dyneither(_tmpA89,
sizeof(char),28))),_tag_dyneither(_tmpA88,sizeof(void*),1)))))));}goto _LL3E;
_LL41: {struct Cyc_Core_Impossible_struct*_tmp389=(struct Cyc_Core_Impossible_struct*)
_tmp386;if(_tmp389->tag != Cyc_Core_Impossible)goto _LL43;else{_tmp38A=_tmp389->f1;}}
_LL42:{const char*_tmpA8D;void*_tmpA8C[1];struct Cyc_String_pa_struct _tmpA8B;(
_tmpA8B.tag=0,((_tmpA8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp38A),((
_tmpA8C[0]=& _tmpA8B,Cyc_fprintf(Cyc_stderr,((_tmpA8D="Exception Core::Impossible %s\n",
_tag_dyneither(_tmpA8D,sizeof(char),31))),_tag_dyneither(_tmpA8C,sizeof(void*),1)))))));}
goto _LL3E;_LL43: {struct Cyc_Dict_Absent_struct*_tmp38B=(struct Cyc_Dict_Absent_struct*)
_tmp386;if(_tmp38B->tag != Cyc_Dict_Absent)goto _LL45;}_LL44:{const char*_tmpA90;
void*_tmpA8F;(_tmpA8F=0,Cyc_fprintf(Cyc_stderr,((_tmpA90="Exception Dict::Absent\n",
_tag_dyneither(_tmpA90,sizeof(char),24))),_tag_dyneither(_tmpA8F,sizeof(void*),0)));}
goto _LL3E;_LL45: {struct Cyc_Core_Invalid_argument_struct*_tmp38C=(struct Cyc_Core_Invalid_argument_struct*)
_tmp386;if(_tmp38C->tag != Cyc_Core_Invalid_argument)goto _LL47;else{_tmp38D=
_tmp38C->f1;}}_LL46:{const char*_tmpA94;void*_tmpA93[1];struct Cyc_String_pa_struct
_tmpA92;(_tmpA92.tag=0,((_tmpA92.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp38D),((_tmpA93[0]=& _tmpA92,Cyc_fprintf(Cyc_stderr,((_tmpA94="Exception Core::Invalid_argument %s\n",
_tag_dyneither(_tmpA94,sizeof(char),37))),_tag_dyneither(_tmpA93,sizeof(void*),1)))))));}
goto _LL3E;_LL47:;_LL48:{const char*_tmpA97;void*_tmpA96;(_tmpA96=0,Cyc_fprintf(
Cyc_stderr,((_tmpA97="Uncaught exception\n",_tag_dyneither(_tmpA97,sizeof(char),
20))),_tag_dyneither(_tmpA96,sizeof(void*),0)));}goto _LL3E;_LL3E:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();{int _tmp39B=1;_npop_handler(0);return _tmp39B;};_LL3C:;
_LL3D:(void)_throw(_tmp385);_LL39:;}};}{const char*_tmpA98;struct _dyneither_ptr
outfile_str=(_tmpA98="",_tag_dyneither(_tmpA98,sizeof(char),1));if(Cyc_output_file
!= 0){const char*_tmpA9C;void*_tmpA9B[1];struct Cyc_String_pa_struct _tmpA9A;
outfile_str=(struct _dyneither_ptr)((_tmpA9A.tag=0,((_tmpA9A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpA9B[0]=& _tmpA9A,Cyc_aprintf(((
_tmpA9C=" -o %s",_tag_dyneither(_tmpA9C,sizeof(char),7))),_tag_dyneither(_tmpA9B,
sizeof(void*),1))))))));}{const char*_tmpAA4;void*_tmpAA3[5];struct Cyc_String_pa_struct
_tmpAA2;struct Cyc_String_pa_struct _tmpAA1;struct Cyc_String_pa_struct _tmpAA0;
struct Cyc_String_pa_struct _tmpA9F;struct Cyc_String_pa_struct _tmpA9E;struct
_dyneither_ptr _tmp39F=(_tmpA9E.tag=0,((_tmpA9E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp353),((_tmpA9F.tag=0,((_tmpA9F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)stdlib_string),((_tmpAA0.tag=0,((_tmpAA0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp352),((_tmpAA1.tag=0,((_tmpAA1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpAA2.tag=0,((
_tmpAA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpAA3[0]=&
_tmpAA2,((_tmpAA3[1]=& _tmpAA1,((_tmpAA3[2]=& _tmpAA0,((_tmpAA3[3]=& _tmpA9F,((
_tmpAA3[4]=& _tmpA9E,Cyc_aprintf(((_tmpAA4="%s %s %s%s%s",_tag_dyneither(_tmpAA4,
sizeof(char),13))),_tag_dyneither(_tmpAA3,sizeof(void*),5)))))))))))))))))))))))))))))));
if(Cyc_v_r){{const char*_tmpAA8;void*_tmpAA7[1];struct Cyc_String_pa_struct _tmpAA6;(
_tmpAA6.tag=0,((_tmpAA6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp39F),((
_tmpAA7[0]=& _tmpAA6,Cyc_fprintf(Cyc_stderr,((_tmpAA8="%s\n",_tag_dyneither(
_tmpAA8,sizeof(char),4))),_tag_dyneither(_tmpAA7,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)_untag_dyneither_ptr(
_tmp39F,sizeof(char),1))!= 0){{const char*_tmpAAB;void*_tmpAAA;(_tmpAAA=0,Cyc_fprintf(
Cyc_stderr,((_tmpAAB="Error: C compiler failed\n",_tag_dyneither(_tmpAAB,sizeof(
char),26))),_tag_dyneither(_tmpAAA,sizeof(void*),0)));}Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp3A5=1;_npop_handler(0);return _tmp3A5;};}Cyc_remove_cfiles();{int _tmp3A6=
Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp3A6;};};};};};;_pop_region(r);};}
