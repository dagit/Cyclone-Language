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
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
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
struct Cyc_Core_NewRegion Cyc_Core__new_dynregion(const char*file,int lineno);extern
char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};extern
char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};void
Cyc_Core_free_dynregion(struct _DynRegionHandle*);inline static void* arrcast(
struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd >> 20  || sz >> 12)
return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr == 
0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*
tag;};extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{char*tag;
struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int
lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;
int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(
struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);char
Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);int Cyc_Lexing_lexeme_start(
struct Cyc_Lexing_lexbuf*);int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*
cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);void Cyc_Set_iter(void(*f)(void*),
struct Cyc_Set_Set*s);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_zstrptrcmp(
struct _dyneither_ptr*,struct _dyneither_ptr*);struct _dyneither_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dyneither_ptr);struct _dyneither_ptr Cyc_zstrncpy(
struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);struct _dyneither_ptr Cyc_substring(
struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Xarray_Xarray{struct
_RegionHandle*r;struct _dyneither_ptr elmts;int num_elmts;};void*Cyc_Xarray_get(
struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct
_RegionHandle*,int,void*);void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);int
Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;
struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(int,int);struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment*,struct _dyneither_ptr);struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[
10];struct Cyc_Position_Nocontext_struct{char*tag;};void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
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
char*tag;};int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,
Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
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
first_line;int first_column;int last_line;int last_column;};extern struct Cyc_Yyltype
Cyc_yylloc;extern union Cyc_YYSTYPE Cyc_yylval;struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern
char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[
7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_empty(int(*
cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*
k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_check_no_qual(struct Cyc_Position_Segment*loc,void*t);void Cyc_yyerror(
struct _dyneither_ptr s){Cyc_Position_post_error(Cyc_Position_mk_err_parse(Cyc_Position_segment_of_abs(
Cyc_yylloc.first_line,Cyc_yylloc.last_line),s));}struct Cyc_Lex_Trie;struct
_union_TrieChildren_Zero{int tag;int val;};struct _tuple16{int f1;struct Cyc_Lex_Trie*
f2;};struct _union_TrieChildren_One{int tag;struct _tuple16 val;};struct
_union_TrieChildren_Many{int tag;struct Cyc_Lex_Trie**val;};union Cyc_Lex_TrieChildren{
struct _union_TrieChildren_Zero Zero;struct _union_TrieChildren_One One;struct
_union_TrieChildren_Many Many;};union Cyc_Lex_TrieChildren Cyc_Lex_Zero(){union Cyc_Lex_TrieChildren
_tmp112;return((_tmp112.Zero).val=0,(((_tmp112.Zero).tag=1,_tmp112)));}union Cyc_Lex_TrieChildren
Cyc_Lex_One(int i,struct Cyc_Lex_Trie*t){struct _tuple16 _tmp115;union Cyc_Lex_TrieChildren
_tmp114;return((_tmp114.One).val=((_tmp115.f1=i,((_tmp115.f2=t,_tmp115)))),(((
_tmp114.One).tag=2,_tmp114)));}union Cyc_Lex_TrieChildren Cyc_Lex_Many(struct Cyc_Lex_Trie**
ts){union Cyc_Lex_TrieChildren _tmp116;return((_tmp116.Many).val=ts,(((_tmp116.Many).tag=
3,_tmp116)));}struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren children;int shared_str;
};static int Cyc_Lex_num_kws=0;static struct _dyneither_ptr Cyc_Lex_kw_nums={(void*)0,(
void*)0,(void*)(0 + 0)};struct Cyc_Lex_DynTrie{struct _DynRegionHandle*dyn;struct
Cyc_Lex_Trie*t;};struct Cyc_Lex_DynTrieSymbols{struct _DynRegionHandle*dyn;struct
Cyc_Lex_Trie*t;struct Cyc_Xarray_Xarray*symbols;};static struct Cyc_Lex_DynTrieSymbols*
Cyc_Lex_ids_trie=0;static struct Cyc_Lex_DynTrie*Cyc_Lex_typedefs_trie=0;static int
Cyc_Lex_comment_depth=0;static struct _tuple5 Cyc_Lex_token_int={Cyc_Absyn_Signed,0};
static char _tmp4[8]="*bogus*";static struct _dyneither_ptr Cyc_Lex_bogus_string={
_tmp4,_tmp4,_tmp4 + 8};static struct _tuple0 Cyc_Lex_token_id_pair={{.Loc_n={4,0}},&
Cyc_Lex_bogus_string};static char Cyc_Lex_token_char='\000';static char _tmp5[1]="";
static struct _dyneither_ptr Cyc_Lex_token_string={_tmp5,_tmp5,_tmp5 + 1};static
struct _tuple0*Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;static char _tmp6[1]="";
static struct _tuple9 Cyc_Lex_token_asm={0,{_tmp6,_tmp6,_tmp6 + 1}};static int Cyc_Lex_runaway_start=
0;static int Cyc_Lex_paren_depth=0;static void Cyc_Lex_err(struct _dyneither_ptr msg,
struct Cyc_Lexing_lexbuf*lb){struct Cyc_Position_Segment*s=Cyc_Position_segment_of_abs(
Cyc_Lexing_lexeme_start(lb),Cyc_Lexing_lexeme_end(lb));Cyc_Position_post_error(
Cyc_Position_mk_err_lex(s,msg));}static void Cyc_Lex_runaway_err(struct
_dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){struct Cyc_Position_Segment*s=Cyc_Position_segment_of_abs(
Cyc_Lex_runaway_start,Cyc_Lexing_lexeme_start(lb));Cyc_Position_post_error(Cyc_Position_mk_err_lex(
s,msg));}struct _tuple17{struct _dyneither_ptr f1;short f2;int f3;};static char _tmp7[
14]="__attribute__";static char _tmp8[9]="abstract";static char _tmp9[6]="alias";
static char _tmpA[5]="auto";static char _tmpB[6]="break";static char _tmpC[7]="calloc";
static char _tmpD[5]="case";static char _tmpE[6]="catch";static char _tmpF[5]="char";
static char _tmp10[6]="const";static char _tmp11[9]="continue";static char _tmp12[9]="datatype";
static char _tmp13[8]="default";static char _tmp14[3]="do";static char _tmp15[7]="double";
static char _tmp16[12]="dynregion_t";static char _tmp17[5]="else";static char _tmp18[5]="enum";
static char _tmp19[7]="export";static char _tmp1A[7]="extern";static char _tmp1B[9]="fallthru";
static char _tmp1C[6]="float";static char _tmp1D[4]="for";static char _tmp1E[5]="goto";
static char _tmp1F[3]="if";static char _tmp20[7]="inline";static char _tmp21[11]="__inline__";
static char _tmp22[4]="int";static char _tmp23[4]="let";static char _tmp24[5]="long";
static char _tmp25[7]="malloc";static char _tmp26[10]="namespace";static char _tmp27[4]="new";
static char _tmp28[11]="NOZEROTERM";static char _tmp29[5]="NULL";static char _tmp2A[8]="numelts";
static char _tmp2B[9]="offsetof";static char _tmp2C[20]="__cyclone_port_on__";static
char _tmp2D[21]="__cyclone_port_off__";static char _tmp2E[8]="rcalloc";static char
_tmp2F[9]="region_t";static char _tmp30[7]="region";static char _tmp31[8]="regions";
static char _tmp32[9]="register";static char _tmp33[13]="reset_region";static char
_tmp34[9]="restrict";static char _tmp35[7]="return";static char _tmp36[8]="rmalloc";
static char _tmp37[5]="rnew";static char _tmp38[6]="short";static char _tmp39[7]="signed";
static char _tmp3A[7]="sizeof";static char _tmp3B[7]="static";static char _tmp3C[7]="struct";
static char _tmp3D[7]="switch";static char _tmp3E[9]="tagcheck";static char _tmp3F[6]="tag_t";
static char _tmp40[6]="throw";static char _tmp41[4]="try";static char _tmp42[7]="tunion";
static char _tmp43[8]="typedef";static char _tmp44[6]="union";static char _tmp45[9]="unsigned";
static char _tmp46[6]="using";static char _tmp47[8]="valueof";static char _tmp48[10]="valueof_t";
static char _tmp49[5]="void";static char _tmp4A[9]="volatile";static char _tmp4B[6]="while";
static char _tmp4C[8]="xtunion";static char _tmp4D[9]="ZEROTERM";static struct
_tuple17 Cyc_Lex_rw_array[71]={{{_tmp7,_tmp7,_tmp7 + 14},370,1},{{_tmp8,_tmp8,
_tmp8 + 9},300,0},{{_tmp9,_tmp9,_tmp9 + 6},324,0},{{_tmpA,_tmpA,_tmpA + 5},258,1},{{
_tmpB,_tmpB,_tmpB + 6},290,1},{{_tmpC,_tmpC,_tmpC + 7},309,0},{{_tmpD,_tmpD,_tmpD + 
5},277,1},{{_tmpE,_tmpE,_tmpE + 6},297,1},{{_tmpF,_tmpF,_tmpF + 5},264,1},{{_tmp10,
_tmp10,_tmp10 + 6},272,1},{{_tmp11,_tmp11,_tmp11 + 9},289,1},{{_tmp12,_tmp12,
_tmp12 + 9},304,0},{{_tmp13,_tmp13,_tmp13 + 8},278,1},{{_tmp14,_tmp14,_tmp14 + 3},
286,1},{{_tmp15,_tmp15,_tmp15 + 7},269,1},{{_tmp16,_tmp16,_tmp16 + 12},323,0},{{
_tmp17,_tmp17,_tmp17 + 5},283,1},{{_tmp18,_tmp18,_tmp18 + 5},292,1},{{_tmp19,
_tmp19,_tmp19 + 7},298,0},{{_tmp1A,_tmp1A,_tmp1A + 7},261,1},{{_tmp1B,_tmp1B,
_tmp1B + 9},301,0},{{_tmp1C,_tmp1C,_tmp1C + 6},268,1},{{_tmp1D,_tmp1D,_tmp1D + 4},
287,1},{{_tmp1E,_tmp1E,_tmp1E + 5},288,1},{{_tmp1F,_tmp1F,_tmp1F + 3},282,1},{{
_tmp20,_tmp20,_tmp20 + 7},279,1},{{_tmp21,_tmp21,_tmp21 + 11},279,1},{{_tmp22,
_tmp22,_tmp22 + 4},266,1},{{_tmp23,_tmp23,_tmp23 + 4},294,0},{{_tmp24,_tmp24,
_tmp24 + 5},267,1},{{_tmp25,_tmp25,_tmp25 + 7},307,0},{{_tmp26,_tmp26,_tmp26 + 10},
303,0},{{_tmp27,_tmp27,_tmp27 + 4},299,0},{{_tmp28,_tmp28,_tmp28 + 11},318,0},{{
_tmp29,_tmp29,_tmp29 + 5},293,0},{{_tmp2A,_tmp2A,_tmp2A + 8},325,0},{{_tmp2B,
_tmp2B,_tmp2B + 9},281,1},{{_tmp2C,_tmp2C,_tmp2C + 20},321,0},{{_tmp2D,_tmp2D,
_tmp2D + 21},322,0},{{_tmp2E,_tmp2E,_tmp2E + 8},310,0},{{_tmp2F,_tmp2F,_tmp2F + 9},
312,0},{{_tmp30,_tmp30,_tmp30 + 7},314,0},{{_tmp31,_tmp31,_tmp31 + 8},316,0},{{
_tmp32,_tmp32,_tmp32 + 9},259,1},{{_tmp33,_tmp33,_tmp33 + 13},317,0},{{_tmp34,
_tmp34,_tmp34 + 9},274,1},{{_tmp35,_tmp35,_tmp35 + 7},291,1},{{_tmp36,_tmp36,
_tmp36 + 8},308,0},{{_tmp37,_tmp37,_tmp37 + 5},315,0},{{_tmp38,_tmp38,_tmp38 + 6},
265,1},{{_tmp39,_tmp39,_tmp39 + 7},270,1},{{_tmp3A,_tmp3A,_tmp3A + 7},280,1},{{
_tmp3B,_tmp3B,_tmp3B + 7},260,1},{{_tmp3C,_tmp3C,_tmp3C + 7},275,1},{{_tmp3D,
_tmp3D,_tmp3D + 7},284,1},{{_tmp3E,_tmp3E,_tmp3E + 9},328,0},{{_tmp3F,_tmp3F,
_tmp3F + 6},313,0},{{_tmp40,_tmp40,_tmp40 + 6},295,0},{{_tmp41,_tmp41,_tmp41 + 4},
296,0},{{_tmp42,_tmp42,_tmp42 + 7},306,0},{{_tmp43,_tmp43,_tmp43 + 8},262,1},{{
_tmp44,_tmp44,_tmp44 + 6},276,1},{{_tmp45,_tmp45,_tmp45 + 9},271,1},{{_tmp46,
_tmp46,_tmp46 + 6},302,0},{{_tmp47,_tmp47,_tmp47 + 8},326,0},{{_tmp48,_tmp48,
_tmp48 + 10},327,0},{{_tmp49,_tmp49,_tmp49 + 5},263,1},{{_tmp4A,_tmp4A,_tmp4A + 9},
273,1},{{_tmp4B,_tmp4B,_tmp4B + 6},285,1},{{_tmp4C,_tmp4C,_tmp4C + 8},305,0},{{
_tmp4D,_tmp4D,_tmp4D + 9},319,0}};static int Cyc_Lex_num_keywords(int
include_cyclone_keywords){int sum=0;{unsigned int i=0;for(0;i < 71;++ i){if(
include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)i]).f3)++ sum;}}return sum;}
static int Cyc_Lex_trie_char(int c){if(c >= 95)return c - 59;else{if(c > 64)return c - 55;}
return c - 48;}static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,
struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len);static void
_tmp11C(unsigned int*_tmp11B,unsigned int*_tmp11A,struct Cyc_Lex_Trie***_tmp119){
for(*_tmp11B=0;*_tmp11B < *_tmp11A;(*_tmp11B)++){(*_tmp119)[*_tmp11B]=0;}}static
struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*
t,struct _dyneither_ptr buff,int offset,int len){int i=offset;int last=(offset + len)- 
1;while(i <= last){union Cyc_Lex_TrieChildren _tmp4E=((struct Cyc_Lex_Trie*)
_check_null(t))->children;struct Cyc_Lex_Trie**_tmp4F;struct _tuple16 _tmp50;int
_tmp51;struct Cyc_Lex_Trie*_tmp52;int _tmp53;_LL1: if((_tmp4E.Many).tag != 3)goto
_LL3;_tmp4F=(struct Cyc_Lex_Trie**)(_tmp4E.Many).val;_LL2: {int ch=Cyc_Lex_trie_char((
int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));if(_tmp4F[
_check_known_subscript_notnull(64,ch)]== 0){struct Cyc_Lex_Trie*_tmp117;_tmp4F[
_check_known_subscript_notnull(64,ch)]=((_tmp117=_region_malloc(r,sizeof(*
_tmp117)),((_tmp117->children=Cyc_Lex_Zero(),((_tmp117->shared_str=0,_tmp117))))));}
t=_tmp4F[_check_known_subscript_notnull(64,ch)];++ i;goto _LL0;}_LL3: if((_tmp4E.One).tag
!= 2)goto _LL5;_tmp50=(struct _tuple16)(_tmp4E.One).val;_tmp51=_tmp50.f1;_tmp52=
_tmp50.f2;_LL4: if(_tmp51 == *((const char*)_check_dyneither_subscript(buff,sizeof(
char),i)))t=_tmp52;else{unsigned int _tmp11B;unsigned int _tmp11A;struct Cyc_Lex_Trie**
_tmp119;unsigned int _tmp118;struct Cyc_Lex_Trie**_tmp55=(_tmp118=64,((_tmp119=(
struct Cyc_Lex_Trie**)_region_malloc(r,_check_times(sizeof(struct Cyc_Lex_Trie*),
_tmp118)),((((_tmp11A=_tmp118,_tmp11C(& _tmp11B,& _tmp11A,& _tmp119))),_tmp119)))));
_tmp55[_check_known_subscript_notnull(64,Cyc_Lex_trie_char(_tmp51))]=_tmp52;{int
_tmp56=Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,
sizeof(char),i)));{struct Cyc_Lex_Trie*_tmp11D;_tmp55[
_check_known_subscript_notnull(64,_tmp56)]=((_tmp11D=_region_malloc(r,sizeof(*
_tmp11D)),((_tmp11D->children=Cyc_Lex_Zero(),((_tmp11D->shared_str=0,_tmp11D))))));}
t->children=Cyc_Lex_Many(_tmp55);t=_tmp55[_check_known_subscript_notnull(64,
_tmp56)];};}++ i;goto _LL0;_LL5: if((_tmp4E.Zero).tag != 1)goto _LL0;_tmp53=(int)(
_tmp4E.Zero).val;_LL6: while(i <= last){struct Cyc_Lex_Trie*_tmp11E;struct Cyc_Lex_Trie*
_tmp5B=(_tmp11E=_region_malloc(r,sizeof(*_tmp11E)),((_tmp11E->children=Cyc_Lex_Zero(),((
_tmp11E->shared_str=0,_tmp11E)))));((struct Cyc_Lex_Trie*)_check_null(t))->children=
Cyc_Lex_One((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i ++)),
_tmp5B);t=_tmp5B;}return t;_LL0:;}return t;}static int Cyc_Lex_str_index(struct
_dyneither_ptr buff,int offset,int len){struct _DynRegionHandle*_tmp5E;struct Cyc_Lex_Trie*
_tmp5F;struct Cyc_Xarray_Xarray*_tmp60;struct Cyc_Lex_DynTrieSymbols _tmp5D=*((
struct Cyc_Lex_DynTrieSymbols*)_check_null(Cyc_Lex_ids_trie));_tmp5E=_tmp5D.dyn;
_tmp5F=_tmp5D.t;_tmp60=_tmp5D.symbols;{struct _DynRegionFrame _tmp61;struct
_RegionHandle*d=_open_dynregion(& _tmp61,_tmp5E);{struct Cyc_Lex_Trie*_tmp62=Cyc_Lex_trie_lookup(
d,_tmp5F,buff,offset,len);if(_tmp62->shared_str == 0){struct _dyneither_ptr _tmp63=
Cyc_Core_new_string((unsigned int)(len + 1));Cyc_zstrncpy(
_dyneither_ptr_decrease_size(_tmp63,sizeof(char),1),(struct _dyneither_ptr)
_dyneither_ptr_plus(buff,sizeof(char),offset),(unsigned long)len);{struct
_dyneither_ptr*_tmp11F;int ans=((int(*)(struct Cyc_Xarray_Xarray*,struct
_dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp60,((_tmp11F=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp11F)),((_tmp11F[0]=(struct _dyneither_ptr)_tmp63,_tmp11F)))));_tmp62->shared_str=
ans;};}{int _tmp65=_tmp62->shared_str;_npop_handler(0);return _tmp65;};};
_pop_dynregion(d);};}static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*
lbuf){return Cyc_Lex_str_index((struct _dyneither_ptr)lbuf->lex_buffer,lbuf->lex_start_pos,
lbuf->lex_curr_pos - lbuf->lex_start_pos);}static void Cyc_Lex_insert_typedef(
struct _dyneither_ptr*sptr){struct _dyneither_ptr _tmp66=*sptr;struct
_DynRegionHandle*_tmp68;struct Cyc_Lex_Trie*_tmp69;struct Cyc_Lex_DynTrie _tmp67=*((
struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));_tmp68=_tmp67.dyn;
_tmp69=_tmp67.t;{struct _DynRegionFrame _tmp6A;struct _RegionHandle*d=
_open_dynregion(& _tmp6A,_tmp68);{struct Cyc_Lex_Trie*_tmp6B=Cyc_Lex_trie_lookup(d,
_tmp69,_tmp66,0,(int)(_get_dyneither_size(_tmp66,sizeof(char))- 1));((struct Cyc_Lex_Trie*)
_check_null(_tmp6B))->shared_str=1;}_npop_handler(0);return;;_pop_dynregion(d);};}
static struct _dyneither_ptr*Cyc_Lex_get_symbol(int symbol_num){struct
_DynRegionHandle*_tmp6D;struct Cyc_Lex_Trie*_tmp6E;struct Cyc_Xarray_Xarray*_tmp6F;
struct Cyc_Lex_DynTrieSymbols _tmp6C=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(
Cyc_Lex_ids_trie));_tmp6D=_tmp6C.dyn;_tmp6E=_tmp6C.t;_tmp6F=_tmp6C.symbols;{
struct _DynRegionFrame _tmp70;struct _RegionHandle*d=_open_dynregion(& _tmp70,_tmp6D);{
struct _dyneither_ptr*_tmp71=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp6F,symbol_num);_npop_handler(0);return _tmp71;};
_pop_dynregion(d);};}static int Cyc_Lex_int_of_char(char c){if('0' <= c  && c <= '9')
return c - '0';else{if('a' <= c  && c <= 'f')return(10 + c)- 'a';else{if('A' <= c  && c
<= 'F')return(10 + c)- 'A';else{struct Cyc_Core_Invalid_argument_struct _tmp125;
const char*_tmp124;struct Cyc_Core_Invalid_argument_struct*_tmp123;(int)_throw((
void*)((_tmp123=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp123)),((
_tmp123[0]=((_tmp125.tag=Cyc_Core_Invalid_argument,((_tmp125.f1=((_tmp124="string to integer conversion",
_tag_dyneither(_tmp124,sizeof(char),29))),_tmp125)))),_tmp123)))));}}}}static
struct _tuple5 Cyc_Lex_intconst(struct Cyc_Lexing_lexbuf*lbuf,int start,int end,int
base){unsigned int n=0;int end2=lbuf->lex_curr_pos - end;struct _dyneither_ptr buff=
lbuf->lex_buffer;int i=start + lbuf->lex_start_pos;{int i=start + lbuf->lex_start_pos;
for(0;i < end2;++ i){char c=*((char*)_check_dyneither_subscript(buff,sizeof(char),i));
switch(c){case 'u': _LL7: goto _LL8;case 'U': _LL8: {struct _tuple5 _tmp126;return(
_tmp126.f1=Cyc_Absyn_Unsigned,((_tmp126.f2=(int)n,_tmp126)));}case 'l': _LL9:
break;case 'L': _LLA: break;default: _LLB: n=n * base + (unsigned int)Cyc_Lex_int_of_char(
c);break;}}}{struct _tuple5 _tmp127;return(_tmp127.f1=Cyc_Absyn_Signed,((_tmp127.f2=(
int)n,_tmp127)));};}char Cyc_Lex_string_buffer_v[11]={'x','x','x','x','x','x','x','x','x','x','\000'};
struct _dyneither_ptr Cyc_Lex_string_buffer={(void*)Cyc_Lex_string_buffer_v,(void*)
Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v + 11)};int Cyc_Lex_string_pos=
0;void Cyc_Lex_store_string_char(char c);static void _tmp12D(int*sz,unsigned int*
_tmp12C,unsigned int*_tmp12B,char**_tmp129){for(*_tmp12C=0;*_tmp12C < *_tmp12B;(*
_tmp12C)++){(*_tmp129)[*_tmp12C]=*_tmp12C < *sz?*((char*)
_check_dyneither_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)*_tmp12C)):'\000';}}
void Cyc_Lex_store_string_char(char c){int sz=(int)(_get_dyneither_size(Cyc_Lex_string_buffer,
sizeof(char))- 1);if(Cyc_Lex_string_pos >= sz){int newsz=sz;while(Cyc_Lex_string_pos
>= newsz){newsz=newsz * 2;}{unsigned int _tmp12C;unsigned int _tmp12B;struct
_dyneither_ptr _tmp12A;char*_tmp129;unsigned int _tmp128;struct _dyneither_ptr str=(
_tmp128=(unsigned int)newsz,((_tmp129=(char*)_region_malloc(Cyc_Core_unique_region,
_check_times(sizeof(char),_tmp128 + 1)),((_tmp12A=_tag_dyneither(_tmp129,sizeof(
char),_tmp128 + 1),((((_tmp12B=_tmp128,((_tmp12D(& sz,& _tmp12C,& _tmp12B,& _tmp129),
_tmp129[_tmp12B]=(char)0)))),_tmp12A)))))));Cyc_Lex_string_buffer=str;};}{char
_tmp130;char _tmp12F;struct _dyneither_ptr _tmp12E;(_tmp12E=_dyneither_ptr_plus(Cyc_Lex_string_buffer,
sizeof(char),Cyc_Lex_string_pos),((_tmp12F=*((char*)_check_dyneither_subscript(
_tmp12E,sizeof(char),0)),((_tmp130=c,((_get_dyneither_size(_tmp12E,sizeof(char))
== 1  && (_tmp12F == '\000'  && _tmp130 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp12E.curr)=_tmp130)))))));}++ Cyc_Lex_string_pos;}struct _dyneither_ptr Cyc_Lex_get_stored_string(){
struct _dyneither_ptr str=Cyc_substring((struct _dyneither_ptr)Cyc_Lex_string_buffer,
0,(unsigned long)Cyc_Lex_string_pos);Cyc_Lex_string_pos=0;return str;}struct Cyc_Lex_Ldecls{
struct Cyc_Set_Set*typedefs;struct Cyc_Set_Set*namespaces;};struct Cyc_Lex_Lvis{
struct Cyc_List_List*current_namespace;struct Cyc_List_List*imported_namespaces;};
struct Cyc_Lex_Lstate{struct Cyc_List_List*lstack;struct Cyc_Dict_Dict decls;};
static struct Cyc_Core_Opt*Cyc_Lex_lstate=0;static void Cyc_Lex_typedef_init(){
struct Cyc_Lex_Lvis*_tmp131;struct Cyc_Lex_Lvis*_tmp7E=(_tmp131=_cycalloc(sizeof(*
_tmp131)),((_tmp131->current_namespace=0,((_tmp131->imported_namespaces=0,
_tmp131)))));struct Cyc_List_List*_tmp132;struct Cyc_List_List*_tmp7F=(_tmp132=
_cycalloc(sizeof(*_tmp132)),((_tmp132->hd=_tmp7E,((_tmp132->tl=0,_tmp132)))));
struct Cyc_Lex_Ldecls*_tmp133;struct Cyc_Dict_Dict init_decls=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp),0,((_tmp133=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp133)),((_tmp133->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),((_tmp133->namespaces=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Set_empty)(Cyc_zstrptrcmp),_tmp133)))))));struct Cyc_Lex_Lstate*_tmp136;
struct Cyc_Core_Opt*_tmp135;Cyc_Lex_lstate=((_tmp135=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp135)),((_tmp135->v=((_tmp136=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp136)),((_tmp136->lstack=_tmp7F,((_tmp136->decls=init_decls,_tmp136)))))),
_tmp135))));}static struct Cyc_List_List*Cyc_Lex_get_absolute_namespace(struct Cyc_List_List*
ns){struct _dyneither_ptr*n=(struct _dyneither_ptr*)ns->hd;{struct Cyc_List_List*ls=(
struct Cyc_List_List*)((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){struct Cyc_Lex_Lvis*lv=(
struct Cyc_Lex_Lvis*)ls->hd;struct Cyc_List_List*_tmp137;struct Cyc_List_List*x=(
_tmp137=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp137)),((_tmp137->hd=lv->current_namespace,((
_tmp137->tl=lv->imported_namespaces,_tmp137)))));for(0;x != 0;x=x->tl){struct Cyc_Lex_Ldecls*
ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(
struct Cyc_List_List*)x->hd);if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_member)(ld->namespaces,n))return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)((struct Cyc_List_List*)x->hd,(struct Cyc_List_List*)
ns);}}}{const char*_tmp13D;void*_tmp13C[1];const char*_tmp13B;struct Cyc_String_pa_struct
_tmp13A;Cyc_yyerror((struct _dyneither_ptr)((_tmp13A.tag=0,((_tmp13A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_str_sepstr((struct Cyc_List_List*)ns,((
_tmp13B="::",_tag_dyneither(_tmp13B,sizeof(char),3))))),((_tmp13C[0]=& _tmp13A,
Cyc_aprintf(((_tmp13D="undeclared namespace %s",_tag_dyneither(_tmp13D,sizeof(
char),24))),_tag_dyneither(_tmp13C,sizeof(void*),1)))))))));}return 0;}static void
Cyc_Lex_recompute_typedefs(){struct _DynRegionHandle*_tmp8B;struct Cyc_Lex_Trie*
_tmp8C;struct Cyc_Lex_DynTrie _tmp8A=*((struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));
_tmp8B=_tmp8A.dyn;_tmp8C=_tmp8A.t;Cyc_Core_free_dynregion(_tmp8B);{struct
_DynRegionHandle*_tmp8E;struct Cyc_Core_NewRegion _tmp8D=Cyc_Core__new_dynregion((
const char*)"internal-error",0);_tmp8E=_tmp8D.dynregion;{struct _DynRegionFrame
_tmp8F;struct _RegionHandle*d2=_open_dynregion(& _tmp8F,_tmp8E);{struct Cyc_Lex_Trie*
_tmp140;struct Cyc_Lex_DynTrie*_tmp13F;Cyc_Lex_typedefs_trie=((_tmp13F=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13F)),((_tmp13F->dyn=(struct
_DynRegionHandle*)_tmp8E,((_tmp13F->t=(struct Cyc_Lex_Trie*)((_tmp140=
_region_malloc(d2,sizeof(*_tmp140)),((_tmp140->children=Cyc_Lex_Zero(),((_tmp140->shared_str=
0,_tmp140)))))),_tmp13F))))));}{struct Cyc_List_List*ls=(struct Cyc_List_List*)((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;
for(0;ls != 0;ls=ls->tl){struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;struct
Cyc_List_List*_tmp141;struct Cyc_List_List*x=(_tmp141=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp141)),((_tmp141->hd=lv->current_namespace,((_tmp141->tl=lv->imported_namespaces,
_tmp141)))));for(0;x != 0;x=x->tl){struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)
x->hd);((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(
Cyc_Lex_insert_typedef,ld->typedefs);}}};;_pop_dynregion(d2);};};}static int Cyc_Lex_is_typedef_in_namespace(
struct Cyc_List_List*ns,struct _dyneither_ptr*v){struct Cyc_List_List*ans=Cyc_Lex_get_absolute_namespace(
ns);struct _handler_cons _tmp93;_push_handler(& _tmp93);{int _tmp95=0;if(setjmp(
_tmp93.handler))_tmp95=1;if(!_tmp95){{struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ans);int _tmp96=((int(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->typedefs,v);
_npop_handler(0);return _tmp96;};_pop_handler();}else{void*_tmp94=(void*)
_exn_thrown;void*_tmp98=_tmp94;_LLE: {struct Cyc_Dict_Absent_struct*_tmp99=(
struct Cyc_Dict_Absent_struct*)_tmp98;if(_tmp99->tag != Cyc_Dict_Absent)goto _LL10;}
_LLF: return 0;_LL10:;_LL11:(void)_throw(_tmp98);_LLD:;}};}static int Cyc_Lex_is_typedef(
struct Cyc_List_List*ns,struct _dyneither_ptr*v){if(ns != 0)return Cyc_Lex_is_typedef_in_namespace((
struct Cyc_List_List*)ns,v);{struct _dyneither_ptr _tmp9A=*v;int len=(int)(
_get_dyneither_size(_tmp9A,sizeof(char))- 1);struct _DynRegionHandle*_tmp9C;
struct Cyc_Lex_Trie*_tmp9D;struct Cyc_Lex_DynTrie _tmp9B=*((struct Cyc_Lex_DynTrie*)
_check_null(Cyc_Lex_typedefs_trie));_tmp9C=_tmp9B.dyn;_tmp9D=_tmp9B.t;{struct
_DynRegionFrame _tmp9E;struct _RegionHandle*d=_open_dynregion(& _tmp9E,_tmp9C);{int
i=0;for(0;i < len;++ i){union Cyc_Lex_TrieChildren _tmp9F=((struct Cyc_Lex_Trie*)
_check_null(_tmp9D))->children;int _tmpA0;struct _tuple16 _tmpA1;int _tmpA2;struct
Cyc_Lex_Trie*_tmpA3;struct _tuple16 _tmpA4;struct Cyc_Lex_Trie*_tmpA5;struct Cyc_Lex_Trie**
_tmpA6;_LL13: if((_tmp9F.Zero).tag != 1)goto _LL15;_tmpA0=(int)(_tmp9F.Zero).val;
_LL14: {int _tmpA7=0;_npop_handler(0);return _tmpA7;}_LL15: if((_tmp9F.One).tag != 2)
goto _LL17;_tmpA1=(struct _tuple16)(_tmp9F.One).val;_tmpA2=_tmpA1.f1;_tmpA3=_tmpA1.f2;
if(!(_tmpA2 != *((const char*)_check_dyneither_subscript(_tmp9A,sizeof(char),i))))
goto _LL17;_LL16: {int _tmpA8=0;_npop_handler(0);return _tmpA8;}_LL17: if((_tmp9F.One).tag
!= 2)goto _LL19;_tmpA4=(struct _tuple16)(_tmp9F.One).val;_tmpA5=_tmpA4.f2;_LL18:
_tmp9D=_tmpA5;goto _LL12;_LL19: if((_tmp9F.Many).tag != 3)goto _LL12;_tmpA6=(struct
Cyc_Lex_Trie**)(_tmp9F.Many).val;_LL1A: {struct Cyc_Lex_Trie*_tmpA9=_tmpA6[
_check_known_subscript_notnull(64,Cyc_Lex_trie_char((int)*((const char*)
_check_dyneither_subscript(_tmp9A,sizeof(char),i))))];if(_tmpA9 == 0){int _tmpAA=0;
_npop_handler(0);return _tmpAA;}_tmp9D=_tmpA9;goto _LL12;}_LL12:;}}{int _tmpAB=((
struct Cyc_Lex_Trie*)_check_null(_tmp9D))->shared_str;_npop_handler(0);return
_tmpAB;};;_pop_dynregion(d);};};}void Cyc_Lex_enter_namespace(struct
_dyneither_ptr*s){struct Cyc_List_List*ns=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_List_List*_tmp142;struct Cyc_List_List*new_ns=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ns,((_tmp142=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp142)),((_tmp142->hd=s,((
_tmp142->tl=0,_tmp142)))))));{struct Cyc_Lex_Lvis*_tmp145;struct Cyc_List_List*
_tmp144;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=((
_tmp144=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp144)),((_tmp144->hd=((
_tmp145=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp145)),((_tmp145->current_namespace=
new_ns,((_tmp145->imported_namespaces=0,_tmp145)))))),((_tmp144->tl=(struct Cyc_List_List*)((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack,
_tmp144))))));}{struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls,ns);if(!((int(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_member)(ld->namespaces,s)){{struct Cyc_Lex_Ldecls*
_tmp146;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*
v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->decls,ns,((_tmp146=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp146)),((_tmp146->typedefs=ld->typedefs,((_tmp146->namespaces=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(
ld->namespaces,s),_tmp146)))))));}{struct Cyc_Lex_Ldecls*_tmp147;((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
new_ns,((_tmp147=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp147)),((
_tmp147->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),((_tmp147->namespaces=((struct
Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(
Cyc_zstrptrcmp),_tmp147)))))));};}((void(*)(void(*f)(struct _dyneither_ptr*),
struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,new_ns))->typedefs);};}
void Cyc_Lex_leave_namespace(){((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack=(struct Cyc_List_List*)_check_null((((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->tl);
Cyc_Lex_recompute_typedefs();}void Cyc_Lex_enter_using(struct _tuple0*q){struct Cyc_List_List*
ns;{union Cyc_Absyn_Nmspace _tmpB1=(*q).f1;int _tmpB2;struct Cyc_List_List*_tmpB3;
struct Cyc_List_List*_tmpB4;struct Cyc_List_List*_tmpB5;_LL1C: if((_tmpB1.Loc_n).tag
!= 4)goto _LL1E;_tmpB2=(int)(_tmpB1.Loc_n).val;_LL1D:{struct Cyc_List_List*_tmp148;
ns=(struct Cyc_List_List*)((_tmp148=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp148)),((_tmp148->hd=(*q).f2,((_tmp148->tl=0,_tmp148))))));}goto _LL1B;_LL1E:
if((_tmpB1.Rel_n).tag != 1)goto _LL20;_tmpB3=(struct Cyc_List_List*)(_tmpB1.Rel_n).val;
_LL1F: _tmpB4=_tmpB3;goto _LL21;_LL20: if((_tmpB1.C_n).tag != 3)goto _LL22;_tmpB4=(
struct Cyc_List_List*)(_tmpB1.C_n).val;_LL21: _tmpB5=_tmpB4;goto _LL23;_LL22: if((
_tmpB1.Abs_n).tag != 2)goto _LL1B;_tmpB5=(struct Cyc_List_List*)(_tmpB1.Abs_n).val;
_LL23:{struct Cyc_List_List*_tmp149;ns=(struct Cyc_List_List*)_check_null(((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmpB5,((_tmp149=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp149)),((
_tmp149->hd=(*q).f2,((_tmp149->tl=0,_tmp149))))))));}goto _LL1B;_LL1B:;}{struct
Cyc_List_List*_tmpB8=Cyc_Lex_get_absolute_namespace(ns);struct Cyc_List_List*
_tmpB9=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;{struct Cyc_List_List*
_tmp14A;((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((_tmp14A=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp14A)),((_tmp14A->hd=_tmpB8,((
_tmp14A->tl=_tmpB9,_tmp14A))))));}((void(*)(void(*f)(struct _dyneither_ptr*),
struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,_tmpB8))->typedefs);};}
void Cyc_Lex_leave_using(){struct Cyc_List_List*_tmpBB=((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((
struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((struct Cyc_List_List*)
_check_null(_tmpBB))->tl;Cyc_Lex_recompute_typedefs();}void Cyc_Lex_register_typedef(
struct _tuple0*q){struct Cyc_List_List*_tmpBC=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict _tmpBD=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls;struct Cyc_Lex_Ldecls*_tmpBE=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmpBD,_tmpBC);
struct Cyc_Lex_Ldecls*_tmp14B;struct Cyc_Lex_Ldecls*_tmpBF=(_tmp14B=_cycalloc(
sizeof(*_tmp14B)),((_tmp14B->typedefs=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_insert)(_tmpBE->typedefs,(*q).f2),((_tmp14B->namespaces=
_tmpBE->namespaces,_tmp14B)))));((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(_tmpBD,_tmpBC,
_tmpBF);Cyc_Lex_insert_typedef((*q).f2);}static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*
lbuf){int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);if(symbol_num <= Cyc_Lex_num_kws){
short res=(short)*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),
symbol_num - 1));switch(res){case 319: _LL24:{const char*_tmp14E;void*_tmp14D;(
_tmp14D=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs(Cyc_yylloc.first_line,Cyc_yylloc.last_line),((
_tmp14E="deprecated ZEROTERM, use @zeroterm",_tag_dyneither(_tmp14E,sizeof(char),
35))),_tag_dyneither(_tmp14D,sizeof(void*),0)));}break;case 318: _LL25:{const char*
_tmp151;void*_tmp150;(_tmp150=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs(Cyc_yylloc.first_line,
Cyc_yylloc.last_line),((_tmp151="deprecated NOZEROTERM, use @nozeroterm",
_tag_dyneither(_tmp151,sizeof(char),39))),_tag_dyneither(_tmp150,sizeof(void*),0)));}
break;case 306: _LL26:{const char*_tmp154;void*_tmp153;(_tmp153=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs(Cyc_yylloc.first_line,Cyc_yylloc.last_line),((
_tmp154="deprecated tunion, use datatype",_tag_dyneither(_tmp154,sizeof(char),32))),
_tag_dyneither(_tmp153,sizeof(void*),0)));}break;case 305: _LL27:{const char*
_tmp157;void*_tmp156;(_tmp156=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs(Cyc_yylloc.first_line,
Cyc_yylloc.last_line),((_tmp157="deprecated xtunion, use @extensible datatype",
_tag_dyneither(_tmp157,sizeof(char),45))),_tag_dyneither(_tmp156,sizeof(void*),0)));}
break;default: _LL28: break;}return(short)*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,
sizeof(int),symbol_num - 1));}{struct _dyneither_ptr*_tmpC9=Cyc_Lex_get_symbol(
symbol_num);Cyc_Lex_token_string=*_tmpC9;if(Cyc_Lex_is_typedef(0,_tmpC9))return
367;return 361;};}static short Cyc_Lex_process_qual_id(struct Cyc_Lexing_lexbuf*lbuf){
int i=lbuf->lex_start_pos;int end=lbuf->lex_curr_pos;struct _dyneither_ptr s=lbuf->lex_buffer;
struct Cyc_List_List*rev_vs=0;while(i < end){int start=i;for(0;i < end  && *((char*)
_check_dyneither_subscript(s,sizeof(char),i))!= ':';++ i){;}if(start == i){struct
Cyc_Core_Impossible_struct _tmp15D;const char*_tmp15C;struct Cyc_Core_Impossible_struct*
_tmp15B;(int)_throw((void*)((_tmp15B=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp15B)),((_tmp15B[0]=((_tmp15D.tag=Cyc_Core_Impossible,((_tmp15D.f1=((
_tmp15C="bad namespace",_tag_dyneither(_tmp15C,sizeof(char),14))),_tmp15D)))),
_tmp15B)))));}{int vlen=i - start;struct _dyneither_ptr*v=Cyc_Lex_get_symbol(Cyc_Lex_str_index((
struct _dyneither_ptr)s,start,vlen));{struct Cyc_List_List*_tmp15E;rev_vs=((
_tmp15E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp15E)),((_tmp15E->hd=v,((
_tmp15E->tl=rev_vs,_tmp15E))))));}i +=2;};}if(rev_vs == 0){struct Cyc_Core_Impossible_struct
_tmp164;const char*_tmp163;struct Cyc_Core_Impossible_struct*_tmp162;(int)_throw((
void*)((_tmp162=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp162)),((
_tmp162[0]=((_tmp164.tag=Cyc_Core_Impossible,((_tmp164.f1=((_tmp163="bad namespace",
_tag_dyneither(_tmp163,sizeof(char),14))),_tmp164)))),_tmp162)))));}{struct
_dyneither_ptr*v=(struct _dyneither_ptr*)rev_vs->hd;struct Cyc_List_List*vs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_vs->tl);{
struct _tuple0*_tmp165;Cyc_Lex_token_qvar=((_tmp165=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp165)),((_tmp165->f1=Cyc_Absyn_Rel_n(vs),((_tmp165->f2=v,_tmp165))))));}
if(Cyc_Lex_is_typedef(vs,v))return 369;return 368;};}int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*);int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_cyc_qual(struct Cyc_Lexing_lexbuf*);int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*);static union Cyc_YYSTYPE Cyc_String_tok(
struct _dyneither_ptr s){union Cyc_YYSTYPE _tmp166;return((_tmp166.String_tok).val=s,(((
_tmp166.String_tok).tag=3,_tmp166)));}static union Cyc_YYSTYPE Cyc_Stringopt_tok(
struct Cyc_Core_Opt*s){union Cyc_YYSTYPE _tmp167;return((_tmp167.Stringopt_tok).val=
s,(((_tmp167.Stringopt_tok).tag=4,_tmp167)));}static union Cyc_YYSTYPE Cyc_Int_tok(
struct _tuple5 s){union Cyc_YYSTYPE _tmp168;return((_tmp168.Int_tok).val=s,(((
_tmp168.Int_tok).tag=1,_tmp168)));}static union Cyc_YYSTYPE Cyc_Char_tok(char s){
union Cyc_YYSTYPE _tmp169;return((_tmp169.Char_tok).val=s,(((_tmp169.Char_tok).tag=
2,_tmp169)));}static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*s){union Cyc_YYSTYPE
_tmp16A;return((_tmp16A.QualId_tok).val=s,(((_tmp16A.QualId_tok).tag=5,_tmp16A)));}
static union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple9 p){union Cyc_YYSTYPE _tmp16B;return((
_tmp16B.Asm_tok).val=p,(((_tmp16B.Asm_tok).tag=6,_tmp16B)));}int Cyc_yylex(struct
Cyc_Lexing_lexbuf*lbuf){int ans=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lex_token)(
lbuf);Cyc_yylloc.first_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_start)(
lbuf);Cyc_yylloc.last_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)(
lbuf);switch(ans){case 361: _LL2A: Cyc_yylval=Cyc_String_tok(Cyc_Lex_token_string);
break;case 367: _LL2B: Cyc_yylval=Cyc_String_tok(Cyc_Lex_token_string);break;case
368: _LL2C: Cyc_yylval=Cyc_QualId_tok(Cyc_Lex_token_qvar);break;case 369: _LL2D: Cyc_yylval=
Cyc_QualId_tok(Cyc_Lex_token_qvar);break;case 366: _LL2E: Cyc_yylval=Cyc_String_tok(
Cyc_Lex_token_string);break;case 362: _LL2F: Cyc_yylval=Cyc_Int_tok(Cyc_Lex_token_int);
break;case 364: _LL30: Cyc_yylval=Cyc_Char_tok(Cyc_Lex_token_char);break;case 365:
_LL31: Cyc_yylval=Cyc_String_tok(Cyc_Lex_token_string);break;case 363: _LL32: Cyc_yylval=
Cyc_String_tok(Cyc_Lex_token_string);break;case 371: _LL33: Cyc_yylval=Cyc_Asm_tok(
Cyc_Lex_token_asm);break;default: _LL34: break;}return ans;}const int Cyc_Lex_lex_base[
254]=(const int[254]){0,24,117,81,132,113,- 7,- 1,4,- 4,- 5,2,- 6,- 2,5,- 3,6,146,- 1,18,
23,35,17,30,28,36,156,128,12,21,- 17,- 18,9,167,171,- 19,10,- 13,- 12,163,- 14,- 11,- 4,
- 5,- 8,- 9,- 10,244,309,187,- 15,218,- 18,- 23,19,52,62,49,85,51,47,51,47,70,60,74,347,
82,82,80,422,88,91,93,497,91,90,96,82,103,104,95,103,572,96,98,647,127,121,124,
132,136,129,131,722,150,150,143,152,797,144,154,143,147,143,161,151,157,872,150,
162,163,947,156,1022,158,174,166,162,173,181,174,182,1097,- 59,293,30,- 43,305,31,
246,1149,32,247,271,337,275,1174,1217,262,266,225,268,- 7,1309,227,1384,1442,1517,
258,- 58,- 25,- 31,1592,263,1667,1742,1800,1875,1950,266,2025,2100,2175,2250,2325,
2400,2475,2550,- 26,- 17,270,- 29,273,- 16,- 34,- 28,- 37,277,- 38,2625,2654,1255,253,
263,299,2664,2694,1287,1297,2727,2758,2796,255,405,2866,2904,397,407,399,410,471,
481,- 9,- 42,995,- 41,11,321,2836,- 36,- 21,- 35,- 20,- 22,1,2944,2,329,2974,341,342,366,
399,400,401,402,404,441,3017,3101,- 56,- 50,- 49,- 48,- 47,- 46,- 45,- 44,- 51,- 54,- 55,
3185,446,- 52,- 53,- 57,- 30,- 27,- 24,996,- 39,12,1223};const int Cyc_Lex_lex_backtrk[
254]=(const int[254]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,- 1,- 1,6,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,5,3,4,- 1,- 1,16,1,19,- 1,2,- 1,- 1,14,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,
14,- 1,- 1,- 1,- 1,22,22,22,22,22,22,- 1,- 1,- 1,- 1,- 1,- 1,12,- 1,- 1,- 1,18,- 1,- 1,- 1,11,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,16,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,21,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,13,- 1,- 1,- 1,20,- 1,19,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,14,- 1,39,58,- 1,58,58,
58,58,58,58,58,58,58,8,10,58,58,58,58,- 1,2,58,58,58,2,58,- 1,- 1,- 1,2,- 1,2,0,- 1,4,
4,- 1,5,3,3,3,3,3,3,1,- 1,- 1,32,- 1,31,- 1,- 1,- 1,- 1,- 1,- 1,12,10,- 1,10,10,- 1,11,12,- 1,
- 1,12,8,9,8,8,- 1,7,7,7,9,9,8,8,- 1,- 1,- 1,- 1,40,- 1,12,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,38,39};const int Cyc_Lex_lex_default[254]=(const int[254]){124,53,
30,27,- 1,6,0,0,14,0,0,- 1,0,0,14,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,27,- 1,- 1,0,0,-
1,- 1,- 1,0,- 1,0,0,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,0,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,250,- 1,- 1,215,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,
- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,0,
- 1,0,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,0,0,205,0,- 1,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,0,250,0,- 1,- 1};const int Cyc_Lex_lex_trans[
3442]=(const int[3442]){0,0,0,0,0,0,0,0,0,125,125,125,125,125,15,15,15,16,16,52,
15,206,251,0,0,0,0,0,0,0,0,0,125,126,127,128,0,129,130,131,246,245,132,133,13,
134,135,136,137,138,138,138,138,138,138,138,138,138,139,13,140,141,142,18,143,
144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,
144,144,144,144,144,144,50,249,214,145,146,147,148,144,144,144,144,144,144,144,
144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,28,149,
54,55,31,29,20,32,21,22,23,56,24,25,26,57,115,58,17,17,17,17,17,59,7,8,113,75,33,
60,9,10,17,17,17,17,17,11,61,62,63,17,26,26,26,26,26,- 1,64,18,87,65,- 1,51,51,17,
88,51,15,67,66,36,71,18,69,26,68,70,72,73,74,76,84,13,78,79,51,80,18,81,82,83,37,
77,85,86,34,38,49,49,49,49,49,49,49,49,39,39,39,39,39,39,39,39,51,51,89,90,51,95,
91,40,50,50,50,50,50,50,50,50,100,92,93,94,96,97,101,51,98,18,99,109,102,150,103,
104,105,- 1,- 1,106,41,19,107,108,110,42,43,111,112,114,12,116,117,118,119,120,121,
- 1,6,122,123,247,44,40,45,169,46,52,47,48,48,48,48,48,48,48,48,48,48,253,253,253,
253,253,248,213,48,48,48,48,48,48,251,35,204,252,151,177,157,205,178,157,253,173,
174,175,170,171,172,211,212,176,179,53,15,41,41,45,48,48,48,48,48,48,186,186,186,
186,186,186,186,186,186,186,48,48,48,48,48,48,48,48,48,48,210,244,12,41,41,45,35,
48,48,48,48,48,48,240,239,152,208,- 1,209,209,209,209,209,209,209,209,209,209,13,
13,13,13,13,13,13,13,13,13,238,48,48,48,48,48,48,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,237,236,235,234,13,233,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,44,44,44,44,
44,44,44,44,44,44,232,45,44,44,46,241,46,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,45,44,44,46,44,46,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,18,18,18,18,18,18,18,18,18,
18,0,203,203,0,0,0,- 1,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,203,203,0,0,18,0,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,18,18,18,18,43,43,43,43,43,43,43,43,43,43,0,0,0,0,0,0,0,
43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,0,
0,0,0,43,0,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,
43,43,43,12,12,12,12,12,12,12,12,12,12,0,0,0,0,0,0,0,12,12,12,12,12,12,12,12,12,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,0,0,0,0,12,0,12,12,12,12,12,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,6,6,6,6,6,6,6,6,6,
6,0,0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,0,0,0,0,6,0,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,41,41,41,41,41,41,41,41,41,
41,0,0,0,0,0,0,0,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,
41,41,41,41,41,0,0,0,0,41,0,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,
41,41,41,41,41,41,41,41,41,15,15,15,15,15,15,15,15,15,15,0,0,0,0,0,0,0,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,0,0,0,0,15,
0,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
46,46,46,46,46,46,46,46,46,46,206,251,0,207,252,0,0,46,46,46,46,46,46,46,46,46,
46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,0,0,0,0,46,0,46,46,46,46,46,
46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,45,45,45,45,45,45,
45,45,45,45,0,0,0,0,0,0,0,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,
45,45,45,45,45,45,45,45,0,0,0,0,45,0,45,45,45,45,45,45,45,45,45,45,45,45,45,45,
45,45,45,45,45,45,45,45,45,45,45,45,42,42,42,42,42,42,42,42,42,42,0,0,0,0,- 1,0,0,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,- 1,
0,0,0,42,0,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,180,0,191,191,191,191,191,191,191,191,192,192,253,253,253,253,253,0,0,0,
0,216,0,182,0,0,0,0,0,0,193,- 1,- 1,0,0,253,0,0,0,194,0,0,195,180,0,181,181,181,
181,181,181,181,181,181,181,182,0,0,0,0,0,0,193,0,0,0,182,0,0,0,0,194,0,183,195,
0,0,0,185,0,185,0,184,186,186,186,186,186,186,186,186,186,186,0,0,0,0,0,182,0,0,
0,0,0,0,183,0,0,0,0,189,0,189,0,184,190,190,190,190,190,190,190,190,190,190,190,
190,190,190,190,190,190,190,190,190,0,0,153,153,153,153,153,153,153,153,153,153,
154,0,0,0,0,0,0,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,153,153,153,153,153,0,0,0,0,153,- 1,153,153,153,153,153,153,
153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
162,162,162,162,162,162,162,162,162,162,0,0,0,0,0,0,0,162,162,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
0,0,0,0,163,0,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,161,161,161,161,161,161,161,161,161,161,
161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,0,0,0,0,161,0,
161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,
161,161,161,161,161,161,153,153,153,153,153,153,153,153,153,153,154,0,0,0,0,0,0,
153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,153,0,0,0,0,153,0,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,153,153,153,155,153,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,153,154,0,0,0,0,0,0,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,0,0,0,0,153,0,
153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,154,0,0,0,0,0,0,
153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,153,0,0,0,0,153,0,153,153,153,153,153,153,153,153,153,153,
153,153,156,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,153,154,0,0,0,0,0,0,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,0,0,0,0,153,0,
153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,153,158,158,158,158,158,158,158,158,158,158,158,158,158,158,
158,158,158,158,158,158,158,158,158,158,158,158,0,0,0,0,159,0,158,158,158,158,
158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,
158,158,158,158,158,158,158,158,158,158,158,158,160,0,0,0,0,0,0,158,158,158,158,
158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,
158,158,0,0,0,0,158,0,158,158,158,158,158,158,158,158,158,158,158,158,158,158,
158,158,158,158,158,158,158,158,158,158,158,158,159,159,159,159,159,159,159,159,
159,159,0,0,0,0,0,0,0,159,159,159,159,159,159,159,159,159,159,159,159,159,159,
159,159,159,159,159,159,159,159,159,159,159,159,0,0,0,0,159,0,159,159,159,159,
159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,
159,159,161,161,161,161,161,161,161,161,161,161,0,0,0,0,0,0,0,161,161,161,161,
161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,
161,161,0,0,0,0,161,0,161,161,161,161,161,161,161,161,161,161,161,161,161,161,
161,161,161,161,161,161,161,161,161,161,161,161,162,162,162,162,162,162,162,162,
162,162,0,0,0,0,0,0,0,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,0,0,0,0,162,0,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,0,0,0,0,0,0,0,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,0,0,0,0,162,0,164,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,0,0,0,0,0,0,0,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,0,0,0,0,162,0,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,162,162,165,162,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,0,0,0,0,0,0,0,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,0,0,0,0,162,0,162,162,162,162,162,162,162,162,162,162,162,162,166,162,
162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,0,0,0,0,0,0,0,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,0,0,0,0,167,0,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,0,0,0,0,0,0,0,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,0,0,0,0,168,0,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,0,0,0,0,0,0,0,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,0,0,0,0,162,0,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,187,187,187,187,187,187,187,187,187,187,0,0,0,0,0,0,0,0,0,0,0,188,37,0,0,
0,0,180,37,181,181,181,181,181,181,181,181,181,181,186,186,186,186,186,186,186,
186,186,186,0,182,0,0,188,37,0,0,183,0,0,37,38,0,0,0,0,184,38,0,187,187,187,187,
187,187,187,187,187,187,0,0,0,182,0,0,0,0,0,0,183,188,37,0,38,0,0,0,37,184,38,0,
0,190,190,190,190,190,190,190,190,190,190,0,0,0,0,0,0,0,0,0,0,188,37,37,0,0,0,0,
37,37,180,0,191,191,191,191,191,191,191,191,192,192,0,0,0,0,0,0,0,0,0,0,0,182,0,
37,0,0,0,0,201,37,0,0,0,0,0,0,180,202,192,192,192,192,192,192,192,192,192,192,0,
0,0,0,0,182,0,0,0,0,0,182,201,0,0,0,0,0,199,0,0,202,0,0,0,0,0,200,0,0,209,209,
209,209,209,209,209,209,209,209,0,0,0,182,0,0,0,0,0,0,199,188,37,0,0,0,0,0,37,
200,196,196,196,196,196,196,196,196,196,196,0,0,0,0,0,0,0,196,196,196,196,196,
196,188,37,0,0,0,0,0,37,0,0,0,0,0,0,0,196,196,196,196,196,196,196,196,196,196,0,
196,196,196,196,196,196,196,196,196,196,196,196,0,0,0,217,0,197,0,0,218,0,0,0,0,
0,198,0,0,219,219,219,219,219,219,219,219,0,196,196,196,196,196,196,220,0,0,0,0,
197,241,0,0,0,0,0,0,0,198,242,242,242,242,242,242,242,242,0,0,0,0,0,0,221,0,0,0,
0,222,223,0,0,0,224,0,0,0,0,0,0,0,225,0,0,0,226,0,227,0,228,0,229,230,230,230,
230,230,230,230,230,230,230,0,0,0,0,0,0,0,230,230,230,230,230,230,230,230,230,
230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,0,0,0,0,0,0,
230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,
230,230,230,230,230,230,231,0,0,0,0,0,0,0,0,230,230,230,230,230,230,230,230,230,
230,0,0,0,0,0,0,0,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,
230,230,230,230,230,230,230,230,230,230,230,0,0,0,0,0,0,230,230,230,230,230,230,
230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,
241,0,0,0,0,0,0,0,0,243,243,243,243,243,243,243,243,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};const int Cyc_Lex_lex_check[
3442]=(const int[3442]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,8,14,16,8,14,32,36,
207,252,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,0,0,0,215,217,0,0,11,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,28,0,0,0,29,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
126,129,132,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,1,1,2,
3,19,2,20,21,22,1,23,24,25,1,54,1,4,4,4,4,4,1,5,5,55,57,2,59,5,5,17,17,17,17,17,
5,60,61,62,4,26,26,26,26,26,27,63,4,56,64,27,33,33,17,56,33,34,58,65,34,67,17,68,
26,58,69,71,72,73,75,76,26,77,78,33,79,33,80,81,82,34,75,84,85,2,34,39,39,39,39,
39,39,39,39,34,34,34,34,34,34,34,34,51,51,88,88,51,89,90,34,49,49,49,49,49,49,49,
49,87,91,92,93,95,96,87,51,97,51,98,100,101,0,102,103,104,8,14,105,34,17,106,107,
109,34,34,110,111,113,34,115,116,117,118,119,120,1,34,121,122,130,34,141,34,145,
34,133,34,47,47,47,47,47,47,47,47,47,47,125,125,125,125,125,130,133,47,47,47,47,
47,47,128,134,136,128,149,139,154,136,139,160,125,140,140,140,142,142,171,134,
134,173,178,136,3,183,184,193,47,47,47,47,47,47,185,185,185,185,185,185,185,185,
185,185,48,48,48,48,48,48,48,48,48,48,208,218,5,183,184,193,2,48,48,48,48,48,48,
220,221,149,135,27,135,135,135,135,135,135,135,135,135,135,66,66,66,66,66,66,66,
66,66,66,222,48,48,48,48,48,48,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
66,66,66,66,66,66,66,66,66,66,223,224,225,226,66,227,66,66,66,66,66,66,66,66,66,
66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,70,70,70,70,70,70,70,70,70,70,
228,194,197,198,199,243,200,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,
70,70,70,70,70,70,70,70,70,194,197,198,199,70,200,70,70,70,70,70,70,70,70,70,70,
70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,74,74,74,74,74,74,74,74,74,74,- 1,
201,202,- 1,- 1,- 1,128,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,74,
74,74,74,74,74,74,201,202,- 1,- 1,74,- 1,74,74,74,74,74,74,74,74,74,74,74,74,74,74,
74,74,74,74,74,74,74,74,74,74,74,74,83,83,83,83,83,83,83,83,83,83,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,
83,- 1,- 1,- 1,- 1,83,- 1,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,83,
83,83,83,83,83,83,86,86,86,86,86,86,86,86,86,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,86,86,86,
86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,- 1,- 1,- 1,- 1,86,
- 1,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,
94,94,94,94,94,94,94,94,94,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,94,94,94,94,94,94,94,94,94,
94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,- 1,- 1,- 1,- 1,94,- 1,94,94,94,94,94,
94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,99,99,99,99,99,99,
99,99,99,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,
99,99,99,99,99,99,99,99,99,99,- 1,- 1,- 1,- 1,99,- 1,99,99,99,99,99,99,99,99,99,99,99,
99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,108,108,108,108,108,108,108,108,108,
108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,108,- 1,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,112,112,112,112,112,112,112,112,112,112,205,250,- 1,205,250,- 1,- 1,112,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,
112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,114,114,114,114,
114,114,114,114,114,114,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,114,114,114,114,114,114,114,114,
114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,- 1,- 1,- 1,- 1,
114,- 1,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,
114,114,114,114,114,114,114,114,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,
- 1,131,- 1,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,131,- 1,- 1,- 1,123,- 1,123,123,123,123,123,123,
123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
137,- 1,137,137,137,137,137,137,137,137,137,137,253,253,253,253,253,- 1,- 1,- 1,- 1,
131,- 1,137,- 1,- 1,- 1,- 1,- 1,- 1,137,205,250,- 1,- 1,253,- 1,- 1,- 1,137,- 1,- 1,137,138,- 1,
138,138,138,138,138,138,138,138,138,138,137,- 1,- 1,- 1,- 1,- 1,- 1,137,- 1,- 1,- 1,138,-
1,- 1,- 1,- 1,137,- 1,138,137,- 1,- 1,- 1,182,- 1,182,- 1,138,182,182,182,182,182,182,182,
182,182,182,- 1,- 1,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,188,- 1,188,- 1,
138,188,188,188,188,188,188,188,188,188,188,189,189,189,189,189,189,189,189,189,
189,- 1,- 1,144,144,144,144,144,144,144,144,144,144,144,- 1,- 1,- 1,- 1,- 1,- 1,144,144,
144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,
144,144,144,144,- 1,- 1,- 1,- 1,144,131,144,144,144,144,144,144,144,144,144,144,144,
144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,146,146,146,146,146,
146,146,146,146,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,146,146,146,146,146,146,146,146,146,146,
146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,- 1,- 1,- 1,- 1,146,
- 1,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,
146,146,146,146,146,146,146,147,147,147,147,147,147,147,147,147,147,147,147,147,
147,147,147,147,147,147,147,147,147,147,147,147,147,- 1,- 1,- 1,- 1,147,- 1,147,147,
147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,
147,147,147,147,148,148,148,148,148,148,148,148,148,148,148,- 1,- 1,- 1,- 1,- 1,- 1,
148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,
148,148,148,148,148,148,- 1,- 1,- 1,- 1,148,- 1,148,148,148,148,148,148,148,148,148,
148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,153,153,153,
153,153,153,153,153,153,153,153,- 1,- 1,- 1,- 1,- 1,- 1,153,153,153,153,153,153,153,
153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,- 1,-
1,- 1,- 1,153,- 1,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,153,153,153,153,153,155,155,155,155,155,155,155,155,155,155,
155,- 1,- 1,- 1,- 1,- 1,- 1,155,155,155,155,155,155,155,155,155,155,155,155,155,155,
155,155,155,155,155,155,155,155,155,155,155,155,- 1,- 1,- 1,- 1,155,- 1,155,155,155,
155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,
155,155,155,156,156,156,156,156,156,156,156,156,156,156,- 1,- 1,- 1,- 1,- 1,- 1,156,
156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,
156,156,156,156,156,- 1,- 1,- 1,- 1,156,- 1,156,156,156,156,156,156,156,156,156,156,
156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,157,157,157,157,
157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,
157,157,- 1,- 1,- 1,- 1,157,- 1,157,157,157,157,157,157,157,157,157,157,157,157,157,
157,157,157,157,157,157,157,157,157,157,157,157,157,158,158,158,158,158,158,158,
158,158,158,158,- 1,- 1,- 1,- 1,- 1,- 1,158,158,158,158,158,158,158,158,158,158,158,
158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,- 1,- 1,- 1,- 1,158,- 1,
158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,
158,158,158,158,158,158,159,159,159,159,159,159,159,159,159,159,- 1,- 1,- 1,- 1,- 1,-
1,- 1,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,
159,159,159,159,159,159,159,- 1,- 1,- 1,- 1,159,- 1,159,159,159,159,159,159,159,159,
159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,161,161,
161,161,161,161,161,161,161,161,- 1,- 1,- 1,- 1,- 1,- 1,- 1,161,161,161,161,161,161,161,
161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,- 1,-
1,- 1,- 1,161,- 1,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,
161,161,161,161,161,161,161,161,161,161,162,162,162,162,162,162,162,162,162,162,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,- 1,- 1,- 1,- 1,162,- 1,162,162,162,162,
162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
162,162,163,163,163,163,163,163,163,163,163,163,- 1,- 1,- 1,- 1,- 1,- 1,- 1,163,163,163,
163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,
163,163,163,- 1,- 1,- 1,- 1,163,- 1,163,163,163,163,163,163,163,163,163,163,163,163,
163,163,163,163,163,163,163,163,163,163,163,163,163,163,164,164,164,164,164,164,
164,164,164,164,- 1,- 1,- 1,- 1,- 1,- 1,- 1,164,164,164,164,164,164,164,164,164,164,164,
164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,- 1,- 1,- 1,- 1,164,- 1,
164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,
164,164,164,164,164,164,165,165,165,165,165,165,165,165,165,165,- 1,- 1,- 1,- 1,- 1,-
1,- 1,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,
165,165,165,165,165,165,165,- 1,- 1,- 1,- 1,165,- 1,165,165,165,165,165,165,165,165,
165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,166,166,
166,166,166,166,166,166,166,166,- 1,- 1,- 1,- 1,- 1,- 1,- 1,166,166,166,166,166,166,166,
166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,- 1,-
1,- 1,- 1,166,- 1,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,
166,166,166,166,166,166,166,166,166,166,167,167,167,167,167,167,167,167,167,167,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,
167,167,167,167,167,167,167,167,167,167,167,- 1,- 1,- 1,- 1,167,- 1,167,167,167,167,
167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,
167,167,168,168,168,168,168,168,168,168,168,168,- 1,- 1,- 1,- 1,- 1,- 1,- 1,168,168,168,
168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,
168,168,168,- 1,- 1,- 1,- 1,168,- 1,168,168,168,168,168,168,168,168,168,168,168,168,
168,168,168,168,168,168,168,168,168,168,168,168,168,168,180,180,180,180,180,180,
180,180,180,180,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,180,180,- 1,- 1,- 1,- 1,181,180,181,
181,181,181,181,181,181,181,181,181,186,186,186,186,186,186,186,186,186,186,- 1,
181,- 1,- 1,180,180,- 1,- 1,181,- 1,- 1,180,186,- 1,- 1,- 1,- 1,181,186,- 1,187,187,187,187,
187,187,187,187,187,187,- 1,- 1,- 1,181,- 1,- 1,- 1,- 1,- 1,- 1,181,187,187,- 1,186,- 1,- 1,
- 1,187,181,186,- 1,- 1,190,190,190,190,190,190,190,190,190,190,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,187,187,190,- 1,- 1,- 1,- 1,187,190,191,- 1,191,191,191,191,191,191,191,
191,191,191,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,191,- 1,190,- 1,- 1,- 1,- 1,191,190,- 1,-
1,- 1,- 1,- 1,- 1,192,191,192,192,192,192,192,192,192,192,192,192,- 1,- 1,- 1,- 1,- 1,191,
- 1,- 1,- 1,- 1,- 1,192,191,- 1,- 1,- 1,- 1,- 1,192,- 1,- 1,191,- 1,- 1,- 1,- 1,- 1,192,- 1,- 1,209,
209,209,209,209,209,209,209,209,209,- 1,- 1,- 1,192,- 1,- 1,- 1,- 1,- 1,- 1,192,209,209,-
1,- 1,- 1,- 1,- 1,209,192,195,195,195,195,195,195,195,195,195,195,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,195,195,195,195,195,195,209,209,- 1,- 1,- 1,- 1,- 1,209,- 1,- 1,- 1,- 1,- 1,- 1,- 1,196,
196,196,196,196,196,196,196,196,196,- 1,195,195,195,195,195,195,196,196,196,196,
196,196,- 1,- 1,- 1,216,- 1,196,- 1,- 1,216,- 1,- 1,- 1,- 1,- 1,196,- 1,- 1,216,216,216,216,
216,216,216,216,- 1,196,196,196,196,196,196,216,- 1,- 1,- 1,- 1,196,219,- 1,- 1,- 1,- 1,-
1,- 1,- 1,196,219,219,219,219,219,219,219,219,- 1,- 1,- 1,- 1,- 1,- 1,216,- 1,- 1,- 1,- 1,
216,216,- 1,- 1,- 1,216,- 1,- 1,- 1,- 1,- 1,- 1,- 1,216,- 1,- 1,- 1,216,- 1,216,- 1,216,- 1,216,
229,229,229,229,229,229,229,229,229,229,- 1,- 1,- 1,- 1,- 1,- 1,- 1,229,229,229,229,229,
229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,
229,- 1,- 1,- 1,- 1,- 1,- 1,229,229,229,229,229,229,229,229,229,229,229,229,229,229,
229,229,229,229,229,229,229,229,229,229,229,229,230,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,230,
230,230,230,230,230,230,230,230,230,- 1,- 1,- 1,- 1,- 1,- 1,- 1,230,230,230,230,230,230,
230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,
- 1,- 1,- 1,- 1,- 1,- 1,230,230,230,230,230,230,230,230,230,230,230,230,230,230,230,
230,230,230,230,230,230,230,230,230,230,230,242,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,242,242,
242,242,242,242,242,242,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_Lex_lex_engine(
int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;
state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){base=Cyc_Lex_lex_base[
_check_known_subscript_notnull(254,state)];if(base < 0)return(- base)- 1;backtrk=
Cyc_Lex_lex_backtrk[_check_known_subscript_notnull(254,state)];if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos
>= lbuf->lex_buffer_len){if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}
else{c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),
lbuf->lex_curr_pos ++));if(c == - 1)c=256;}if(Cyc_Lex_lex_check[
_check_known_subscript_notnull(3442,base + c)]== state)state=Cyc_Lex_lex_trans[
_check_known_subscript_notnull(3442,base + c)];else{state=Cyc_Lex_lex_default[
_check_known_subscript_notnull(254,state)];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){struct Cyc_Lexing_Error_struct _tmp171;const char*
_tmp170;struct Cyc_Lexing_Error_struct*_tmp16F;(int)_throw((void*)((_tmp16F=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp16F)),((_tmp16F[0]=((_tmp171.tag=
Cyc_Lexing_Error,((_tmp171.f1=((_tmp170="empty token",_tag_dyneither(_tmp170,
sizeof(char),12))),_tmp171)))),_tmp16F)))));}else{return lbuf->lex_last_action;}}
else{if(c == 256)lbuf->lex_eof_reached=0;}}}int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL36: return Cyc_Lex_snarf_asm(lexbuf);case 1: _LL37: return Cyc_Lex_snarf_asm(
lexbuf);case 2: _LL38: return(int)Cyc_Lex_process_id(lexbuf);case 3: _LL39: return(int)
Cyc_Lex_process_id(lexbuf);case 4: _LL3A: return(int)Cyc_Lex_process_qual_id(lexbuf);
case 5: _LL3B: Cyc_Lex_token_string=*Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(
lexbuf));return 366;case 6: _LL3C: return Cyc_Lex_cyc_qual(lexbuf);case 7: _LL3D: Cyc_Lex_token_int=
Cyc_Lex_intconst(lexbuf,2,0,16);return 362;case 8: _LL3E: Cyc_Lex_token_int=Cyc_Lex_intconst(
lexbuf,0,0,8);return 362;case 9: _LL3F: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,
0,10);return 362;case 10: _LL40: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 362;case 11: _LL41: Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf);return 365;case 12: _LL42: Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf);return 365;case 13: _LL43: return 344;case 14: _LL44: return 345;case 15: _LL45:
return 342;case 16: _LL46: return 343;case 17: _LL47: return 338;case 18: _LL48: return 339;
case 19: _LL49: return 351;case 20: _LL4A: return 352;case 21: _LL4B: return 348;case 22:
_LL4C: return 349;case 23: _LL4D: return 350;case 24: _LL4E: return 357;case 25: _LL4F:
return 356;case 26: _LL50: return 355;case 27: _LL51: return 353;case 28: _LL52: return 354;
case 29: _LL53: return 346;case 30: _LL54: return 347;case 31: _LL55: return 340;case 32:
_LL56: return 341;case 33: _LL57: return 359;case 34: _LL58: return 337;case 35: _LL59:
return 358;case 36: _LL5A: return 360;case 37: _LL5B: return 311;case 38: _LL5C: return Cyc_Lex_token(
lexbuf);case 39: _LL5D: return Cyc_Lex_token(lexbuf);case 40: _LL5E: return Cyc_Lex_token(
lexbuf);case 41: _LL5F: Cyc_Lex_comment_depth=1;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(
lexbuf);Cyc_Lex_comment(lexbuf);return Cyc_Lex_token(lexbuf);case 42: _LL60: Cyc_Lex_string_pos=
0;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);while(Cyc_Lex_strng(
lexbuf)){;}Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 363;case 43: _LL61: Cyc_Lex_token_char='\a';return 364;case 44: _LL62: Cyc_Lex_token_char='\b';
return 364;case 45: _LL63: Cyc_Lex_token_char='\f';return 364;case 46: _LL64: Cyc_Lex_token_char='\n';
return 364;case 47: _LL65: Cyc_Lex_token_char='\r';return 364;case 48: _LL66: Cyc_Lex_token_char='\t';
return 364;case 49: _LL67: Cyc_Lex_token_char='\v';return 364;case 50: _LL68: Cyc_Lex_token_char='\\';
return 364;case 51: _LL69: Cyc_Lex_token_char='\'';return 364;case 52: _LL6A: Cyc_Lex_token_char='"';
return 364;case 53: _LL6B: Cyc_Lex_token_char='?';return 364;case 54: _LL6C: Cyc_Lex_token_char=(
char)(Cyc_Lex_intconst(lexbuf,2,1,8)).f2;return 364;case 55: _LL6D: Cyc_Lex_token_char=(
char)(Cyc_Lex_intconst(lexbuf,3,1,16)).f2;return 364;case 56: _LL6E: Cyc_Lex_token_char=
Cyc_Lexing_lexeme_char(lexbuf,1);return 364;case 57: _LL6F: return - 1;case 58: _LL70:
return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL71:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_token_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp177;const char*_tmp176;struct Cyc_Lexing_Error_struct*_tmp175;(int)_throw((
void*)((_tmp175=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp175)),((
_tmp175[0]=((_tmp177.tag=Cyc_Lexing_Error,((_tmp177.f1=((_tmp176="some action didn't return!",
_tag_dyneither(_tmp176,sizeof(char),27))),_tmp177)))),_tmp175)))));};}int Cyc_Lex_token(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}int Cyc_Lex_cyc_qual_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL73:{const char*_tmp178;lexbuf->lex_curr_pos -=
Cyc_strlen(((_tmp178="taggedZ",_tag_dyneither(_tmp178,sizeof(char),8))));}return(
int)'@';case 1: _LL74:{const char*_tmp179;lexbuf->lex_curr_pos -=Cyc_strlen(((
_tmp179="zerotermZ",_tag_dyneither(_tmp179,sizeof(char),10))));}return(int)'@';
case 2: _LL75:{const char*_tmp17A;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp17A="nozerotermZ",
_tag_dyneither(_tmp17A,sizeof(char),12))));}return(int)'@';case 3: _LL76:{const
char*_tmp17B;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp17B="extensibleZ",
_tag_dyneither(_tmp17B,sizeof(char),12))));}return(int)'@';case 4: _LL77:{const
char*_tmp17C;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp17C="resetableZ",
_tag_dyneither(_tmp17C,sizeof(char),11))));}return(int)'@';case 5: _LL78:{const
char*_tmp17D;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp17D="regionZ",_tag_dyneither(
_tmp17D,sizeof(char),8))));}return(int)'@';case 6: _LL79:{const char*_tmp17E;lexbuf->lex_curr_pos
-=Cyc_strlen(((_tmp17E="numeltsZ",_tag_dyneither(_tmp17E,sizeof(char),9))));}
return(int)'@';case 7: _LL7A:{const char*_tmp17F;lexbuf->lex_curr_pos -=Cyc_strlen(((
_tmp17F="thinZ",_tag_dyneither(_tmp17F,sizeof(char),6))));}return(int)'@';case 8:
_LL7B:{const char*_tmp180;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp180="fatZ",
_tag_dyneither(_tmp180,sizeof(char),5))));}return(int)'@';case 9: _LL7C:{const char*
_tmp181;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp181="notnullZ",_tag_dyneither(
_tmp181,sizeof(char),9))));}return(int)'@';case 10: _LL7D:{const char*_tmp182;
lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp182="nullableZ",_tag_dyneither(_tmp182,
sizeof(char),10))));}return(int)'@';case 11: _LL7E: return 334;case 12: _LL7F: return
319;case 13: _LL80: return 318;case 14: _LL81: return 335;case 15: _LL82: return 336;case 16:
_LL83: return 320;case 17: _LL84: return 329;case 18: _LL85: return 330;case 19: _LL86:
return 331;case 20: _LL87: return 332;case 21: _LL88: return 333;case 22: _LL89: lexbuf->lex_curr_pos
-=1;return(int)'@';default: _LL8A:(lexbuf->refill_buff)(lexbuf);return Cyc_Lex_cyc_qual_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp188;const char*_tmp187;struct
Cyc_Lexing_Error_struct*_tmp186;(int)_throw((void*)((_tmp186=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp186)),((_tmp186[0]=((_tmp188.tag=Cyc_Lexing_Error,((_tmp188.f1=((
_tmp187="some action didn't return!",_tag_dyneither(_tmp187,sizeof(char),27))),
_tmp188)))),_tmp186)))));};}int Cyc_Lex_cyc_qual(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_Lex_cyc_qual_rec(lexbuf,1);}int Cyc_Lex_strng_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL8C: return 1;case 1: _LL8D: return 0;case 2: _LL8E: return 1;case 3: _LL8F: Cyc_Lex_store_string_char('\a');
return 1;case 4: _LL90: Cyc_Lex_store_string_char('\b');return 1;case 5: _LL91: Cyc_Lex_store_string_char('\f');
return 1;case 6: _LL92: Cyc_Lex_store_string_char('\n');return 1;case 7: _LL93: Cyc_Lex_store_string_char('\r');
return 1;case 8: _LL94: Cyc_Lex_store_string_char('\t');return 1;case 9: _LL95: Cyc_Lex_store_string_char('\v');
return 1;case 10: _LL96: Cyc_Lex_store_string_char('\\');return 1;case 11: _LL97: Cyc_Lex_store_string_char('\'');
return 1;case 12: _LL98: Cyc_Lex_store_string_char('"');return 1;case 13: _LL99: Cyc_Lex_store_string_char('?');
return 1;case 14: _LL9A: Cyc_Lex_store_string_char((char)(Cyc_Lex_intconst(lexbuf,1,
0,8)).f2);return 1;case 15: _LL9B: Cyc_Lex_store_string_char((char)(Cyc_Lex_intconst(
lexbuf,2,0,16)).f2);return 1;case 16: _LL9C: Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(
lexbuf,0));return 1;case 17: _LL9D:{const char*_tmp189;Cyc_Lex_runaway_err(((_tmp189="string ends in newline",
_tag_dyneither(_tmp189,sizeof(char),23))),lexbuf);}return 0;case 18: _LL9E:{const
char*_tmp18A;Cyc_Lex_runaway_err(((_tmp18A="unterminated string",_tag_dyneither(
_tmp18A,sizeof(char),20))),lexbuf);}return 0;case 19: _LL9F:{const char*_tmp18B;Cyc_Lex_err(((
_tmp18B="bad character following backslash in string",_tag_dyneither(_tmp18B,
sizeof(char),44))),lexbuf);}return 1;default: _LLA0:(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp191;
const char*_tmp190;struct Cyc_Lexing_Error_struct*_tmp18F;(int)_throw((void*)((
_tmp18F=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp18F)),((_tmp18F[0]=((
_tmp191.tag=Cyc_Lexing_Error,((_tmp191.f1=((_tmp190="some action didn't return!",
_tag_dyneither(_tmp190,sizeof(char),27))),_tmp191)))),_tmp18F)))));};}int Cyc_Lex_strng(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(lexbuf,2);}int Cyc_Lex_comment_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLA2: ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(
lexbuf);case 1: _LLA3: -- Cyc_Lex_comment_depth;if(Cyc_Lex_comment_depth > 0)return
Cyc_Lex_comment(lexbuf);return 0;case 2: _LLA4:{const char*_tmp192;Cyc_Lex_runaway_err(((
_tmp192="unterminated comment",_tag_dyneither(_tmp192,sizeof(char),21))),lexbuf);}
return 0;case 3: _LLA5: return Cyc_Lex_comment(lexbuf);case 4: _LLA6: return Cyc_Lex_comment(
lexbuf);case 5: _LLA7: return Cyc_Lex_comment(lexbuf);default: _LLA8:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_comment_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp198;const char*_tmp197;struct Cyc_Lexing_Error_struct*_tmp196;(int)_throw((
void*)((_tmp196=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp196)),((
_tmp196[0]=((_tmp198.tag=Cyc_Lexing_Error,((_tmp198.f1=((_tmp197="some action didn't return!",
_tag_dyneither(_tmp197,sizeof(char),27))),_tmp198)))),_tmp196)))));};}int Cyc_Lex_comment(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(lexbuf,3);}int Cyc_Lex_snarf_asm_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLAA: Cyc_Lex_token_asm.f1=0;Cyc_Lex_paren_depth=
1;Cyc_Lex_string_pos=0;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}Cyc_Lex_token_asm.f2=(struct
_dyneither_ptr)Cyc_Lex_get_stored_string();return 371;case 1: _LLAB: Cyc_Lex_token_asm.f1=
1;Cyc_Lex_paren_depth=1;Cyc_Lex_string_pos=0;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(
lexbuf);while(Cyc_Lex_snarf_asm_body(lexbuf)){;}Cyc_Lex_token_asm.f2=(struct
_dyneither_ptr)Cyc_Lex_get_stored_string();return 371;default: _LLAC:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_snarf_asm_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp19E;const char*_tmp19D;struct Cyc_Lexing_Error_struct*_tmp19C;(int)_throw((
void*)((_tmp19C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp19C)),((
_tmp19C[0]=((_tmp19E.tag=Cyc_Lexing_Error,((_tmp19E.f1=((_tmp19D="some action didn't return!",
_tag_dyneither(_tmp19D,sizeof(char),27))),_tmp19E)))),_tmp19C)))));};}int Cyc_Lex_snarf_asm(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_rec(lexbuf,4);}int Cyc_Lex_snarf_asm_body_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLAE: Cyc_Lex_store_string_char('"');while(Cyc_Lex_strng(
lexbuf)){;}Cyc_Lex_store_string_char('"');return 1;case 1: _LLAF: Cyc_Lex_comment_depth=
1;Cyc_Lex_comment(lexbuf);Cyc_Lex_store_string_char(' ');return 1;case 2: _LLB0: Cyc_Lex_store_string_char('\n');
return 1;case 3: _LLB1: Cyc_Lex_paren_depth +=1;Cyc_Lex_store_string_char('(');return
1;case 4: _LLB2: Cyc_Lex_paren_depth -=1;if(Cyc_Lex_paren_depth == 0)return 0;else{Cyc_Lex_store_string_char(')');
return 1;}case 5: _LLB3:{const char*_tmp19F;Cyc_Lex_runaway_err(((_tmp19F="unterminated asm",
_tag_dyneither(_tmp19F,sizeof(char),17))),lexbuf);}return 0;case 6: _LLB4: Cyc_Lex_store_string_char(
Cyc_Lexing_lexeme_char(lexbuf,0));return 1;default: _LLB5:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_snarf_asm_body_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp1A5;const char*_tmp1A4;struct Cyc_Lexing_Error_struct*_tmp1A3;(int)_throw((
void*)((_tmp1A3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1A3)),((
_tmp1A3[0]=((_tmp1A5.tag=Cyc_Lexing_Error,((_tmp1A5.f1=((_tmp1A4="some action didn't return!",
_tag_dyneither(_tmp1A4,sizeof(char),27))),_tmp1A5)))),_tmp1A3)))));};}int Cyc_Lex_snarf_asm_body(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_body_rec(lexbuf,5);}void
Cyc_Lex_lex_init(int include_cyclone_keywords);static void _tmp1B4(unsigned int*
_tmp1B3,unsigned int*_tmp1B2,int**_tmp1B0){for(*_tmp1B3=0;*_tmp1B3 < *_tmp1B2;(*
_tmp1B3)++){(*_tmp1B0)[*_tmp1B3]=0;}}void Cyc_Lex_lex_init(int
include_cyclone_keywords){if(Cyc_Lex_ids_trie != 0){struct _DynRegionHandle*_tmpFE;
struct Cyc_Lex_DynTrieSymbols _tmpFD=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(
Cyc_Lex_ids_trie));_tmpFE=_tmpFD.dyn;Cyc_Core_free_dynregion(_tmpFE);}if(Cyc_Lex_typedefs_trie
!= 0){struct _DynRegionHandle*_tmp100;struct Cyc_Lex_DynTrie _tmpFF=*((struct Cyc_Lex_DynTrie*)
_check_null(Cyc_Lex_typedefs_trie));_tmp100=_tmpFF.dyn;Cyc_Core_free_dynregion(
_tmp100);}{struct _DynRegionHandle*_tmp102;struct Cyc_Core_NewRegion _tmp101=Cyc_Core__new_dynregion((
const char*)"internal-error",0);_tmp102=_tmp101.dynregion;{struct _DynRegionFrame
_tmp103;struct _RegionHandle*id_rgn=_open_dynregion(& _tmp103,_tmp102);{const char*
_tmp1A8;struct _dyneither_ptr*_tmp1A7;struct Cyc_Xarray_Xarray*symbols=((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*,int,struct _dyneither_ptr*))Cyc_Xarray_rcreate)(id_rgn,101,((
_tmp1A7=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1A7)),((_tmp1A7[0]=(
struct _dyneither_ptr)((_tmp1A8="",_tag_dyneither(_tmp1A8,sizeof(char),1))),
_tmp1A7)))));((void(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add)(
symbols,& Cyc_Lex_bogus_string);{struct Cyc_Lex_Trie*_tmp1AB;struct Cyc_Lex_DynTrieSymbols*
_tmp1AA;Cyc_Lex_ids_trie=((_tmp1AA=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1AA)),((_tmp1AA->dyn=(struct _DynRegionHandle*)_tmp102,((_tmp1AA->t=(struct
Cyc_Lex_Trie*)((_tmp1AB=_region_malloc(id_rgn,sizeof(*_tmp1AB)),((_tmp1AB->children=
Cyc_Lex_Zero(),((_tmp1AB->shared_str=0,_tmp1AB)))))),((_tmp1AA->symbols=(struct
Cyc_Xarray_Xarray*)symbols,_tmp1AA))))))));};}{struct _DynRegionHandle*_tmp109;
struct Cyc_Core_NewRegion _tmp108=Cyc_Core__new_dynregion((const char*)"internal-error",
0);_tmp109=_tmp108.dynregion;{struct _DynRegionFrame _tmp10A;struct _RegionHandle*
typedefs_rgn=_open_dynregion(& _tmp10A,_tmp109);{struct Cyc_Lex_Trie*_tmp1AE;
struct Cyc_Lex_DynTrie*_tmp1AD;Cyc_Lex_typedefs_trie=((_tmp1AD=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1AD)),((_tmp1AD->dyn=(struct _DynRegionHandle*)_tmp109,((_tmp1AD->t=(
struct Cyc_Lex_Trie*)((_tmp1AE=_region_malloc(typedefs_rgn,sizeof(*_tmp1AE)),((
_tmp1AE->children=Cyc_Lex_Zero(),((_tmp1AE->shared_str=0,_tmp1AE)))))),_tmp1AD))))));}
Cyc_Lex_num_kws=Cyc_Lex_num_keywords(include_cyclone_keywords);{unsigned int
_tmp1B3;unsigned int _tmp1B2;struct _dyneither_ptr _tmp1B1;int*_tmp1B0;unsigned int
_tmp1AF;Cyc_Lex_kw_nums=((_tmp1AF=(unsigned int)Cyc_Lex_num_kws,((_tmp1B0=(int*)
_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(int),_tmp1AF)),((
_tmp1B1=_tag_dyneither(_tmp1B0,sizeof(int),_tmp1AF),((((_tmp1B2=_tmp1AF,_tmp1B4(&
_tmp1B3,& _tmp1B2,& _tmp1B0))),_tmp1B1))))))));}{unsigned int i=0;unsigned int rwsze=
71;{unsigned int j=0;for(0;j < rwsze;++ j){if(include_cyclone_keywords  || (Cyc_Lex_rw_array[(
int)j]).f3){struct _dyneither_ptr _tmp111=(Cyc_Lex_rw_array[(int)j]).f1;Cyc_Lex_str_index(
_tmp111,0,(int)Cyc_strlen((struct _dyneither_ptr)_tmp111));*((int*)
_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),(int)i))=(int)(Cyc_Lex_rw_array[(
int)j]).f2;++ i;}}}Cyc_Lex_typedef_init();Cyc_Lex_comment_depth=0;};;
_pop_dynregion(typedefs_rgn);};};;_pop_dynregion(id_rgn);};};}
