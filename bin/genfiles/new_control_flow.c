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
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
void*Cyc_Core_snd(struct _tuple0*);int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < 
dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rcopy(
struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(
struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*
tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,
void**ans);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict
d);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(
struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(
struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;struct
Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};struct
Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};int Cyc_Position_error_p();struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n
Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(
struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*
ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
char*tag;};void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
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
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_rsubstitute(struct
_RegionHandle*,struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
int Cyc_Tcutil_is_noreturn(void*);struct _tuple10{unsigned int f1;int f2;};struct
_tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
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
_union_RelnOp_LessEqConst LessEqConst;};union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(
struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(
struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int);struct Cyc_CfFlowInfo_Reln{
struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};extern char Cyc_CfFlowInfo_IsZero[
7];struct Cyc_CfFlowInfo_IsZero_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[
8];struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;
struct Cyc_List_List*f1;};extern struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val;
struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
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
struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct
Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int
tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{struct
_union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{
struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*
f);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_List_List*,int no_init_bits_only,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);void*Cyc_CfFlowInfo_make_unique_consumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int
iteration,void*);int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int
env_iteration,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct
Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
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
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple1*);int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;};static char Cyc_NewControlFlow_CFAnnot[
8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser){struct Cyc_NewControlFlow_CFAnnot_struct _tmp763;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp762;struct Cyc_NewControlFlow_CFAnnot_struct*_tmp761;enclosee->annot=(void*)((
_tmp761=_cycalloc(sizeof(*_tmp761)),((_tmp761[0]=((_tmp763.tag=Cyc_NewControlFlow_CFAnnot,((
_tmp763.f1=((_tmp762.encloser=encloser,((_tmp762.visited=0,_tmp762)))),_tmp763)))),
_tmp761))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*
fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo
tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*
param_roots;struct Cyc_Hashtable_Table*flow_table;};static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,
struct Cyc_Absyn_Stmt*);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);
struct _tuple14{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo,int expand_unique,struct Cyc_Absyn_Exp*);static struct
_tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple15{
union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};static struct
_tuple15 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,
struct Cyc_Absyn_Exp*);static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);static
struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*
s){void*_tmp3=s->annot;struct Cyc_NewControlFlow_CFStmtAnnot _tmp5;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp6;_LL1: {struct Cyc_NewControlFlow_CFAnnot_struct*_tmp4=(struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp5=_tmp4->f1;
_tmp6=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp4->f1;}}_LL2: return _tmp6;_LL3:;
_LL4: {struct Cyc_Core_Impossible_struct _tmp769;const char*_tmp768;struct Cyc_Core_Impossible_struct*
_tmp767;(int)_throw((void*)((_tmp767=_cycalloc(sizeof(*_tmp767)),((_tmp767[0]=((
_tmp769.tag=Cyc_Core_Impossible,((_tmp769.f1=((_tmp768="ControlFlow -- wrong stmt annotation",
_tag_dyneither(_tmp768,sizeof(char),37))),_tmp769)))),_tmp767)))));}_LL0:;}
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*_tmp76A;union Cyc_CfFlowInfo_FlowInfo*
res=(_tmp76A=_region_malloc(env->r,sizeof(*_tmp76A)),((_tmp76A[0]=Cyc_CfFlowInfo_BottomFL(),
_tmp76A)));((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union
Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);return
res;}return*sflow;}struct _tuple16{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union
Cyc_CfFlowInfo_FlowInfo*f2;};static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*
_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);_tmpB->visited=
env->iteration_num;{struct _tuple16 _tmp76B;return(_tmp76B.f1=_tmpB,((_tmp76B.f2=
_tmpC,_tmp76B)));};}static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo new_flow){if(env->in_try)env->tryflow=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple17{struct Cyc_NewControlFlow_AnalEnv*
f1;struct Cyc_Position_Segment*f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_unique_root(
struct _tuple17*ckenv,void*root,void*rval){struct _tuple17 _tmpF;struct Cyc_NewControlFlow_AnalEnv*
_tmp10;struct Cyc_Position_Segment*_tmp11;struct Cyc_Dict_Dict _tmp12;struct
_tuple17*_tmpE=ckenv;_tmpF=*_tmpE;_tmp10=_tmpF.f1;_tmp11=_tmpF.f2;_tmp12=_tmpF.f3;{
void*_tmp13=root;struct Cyc_Absyn_Vardecl*_tmp15;_LL6: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp14=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp13;if(_tmp14->tag != 0)goto _LL8;
else{_tmp15=_tmp14->f1;}}_LL7: if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))
Cyc_Dict_lookup_bool)(_tmp12,root,& rval) && Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp10->fenv)->r,_tmp15->type)){{void*_tmp16=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp19;_LLB: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp17=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp16;if(_tmp17->tag != 7)goto _LLD;}_LLC: goto _LLE;_LLD: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp18=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp16;if(_tmp18->tag != 3)goto _LLF;
else{_tmp19=_tmp18->f1;if(_tmp19 != Cyc_CfFlowInfo_NoneIL)goto _LLF;}}_LLE: goto
_LL10;_LLF: {struct Cyc_CfFlowInfo_Zero_struct*_tmp1A=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp16;if(_tmp1A->tag != 0)goto _LL11;}_LL10: goto _LLA;_LL11:;_LL12:{const char*
_tmp76F;void*_tmp76E[1];struct Cyc_String_pa_struct _tmp76D;(_tmp76D.tag=0,((
_tmp76D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp15->name)),((_tmp76E[0]=& _tmp76D,Cyc_Tcutil_warn(_tmp11,((_tmp76F="unique pointer(s) reachable from %s may become unreachable.",
_tag_dyneither(_tmp76F,sizeof(char),60))),_tag_dyneither(_tmp76E,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}goto _LL5;}goto _LL5;_LL8:;_LL9: goto _LL5;_LL5:;};}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
flow,*_tmp1F);if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple15 _tmp770;
struct _tuple15 _tmp22=(_tmp770.f1=flow,((_tmp770.f2=_tmp20,_tmp770)));union Cyc_CfFlowInfo_FlowInfo
_tmp23;struct _tuple12 _tmp24;struct Cyc_Dict_Dict _tmp25;union Cyc_CfFlowInfo_FlowInfo
_tmp26;struct _tuple12 _tmp27;struct Cyc_Dict_Dict _tmp28;_LL14: _tmp23=_tmp22.f1;if((
_tmp23.ReachableFL).tag != 2)goto _LL16;_tmp24=(struct _tuple12)(_tmp23.ReachableFL).val;
_tmp25=_tmp24.f1;_tmp26=_tmp22.f2;if((_tmp26.ReachableFL).tag != 2)goto _LL16;
_tmp27=(struct _tuple12)(_tmp26.ReachableFL).val;_tmp28=_tmp27.f1;_LL15: {struct
_tuple17 _tmp771;struct _tuple17 _tmp29=(_tmp771.f1=env,((_tmp771.f2=s->loc,((
_tmp771.f3=_tmp28,_tmp771)))));((void(*)(void(*f)(struct _tuple17*,void*,void*),
struct _tuple17*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,&
_tmp29,_tmp25);goto _LL13;}_LL16:;_LL17: goto _LL13;_LL13:;}if(!Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp20,*_tmp1F)){*_tmp1F=_tmp20;if(_tmp1E->visited == env->iteration_num)env->iterate_again=
1;}}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,
struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo _tmp2B=inflow;int
_tmp2C;struct _tuple12 _tmp2D;struct Cyc_Dict_Dict _tmp2E;struct Cyc_List_List*_tmp2F;
_LL19: if((_tmp2B.BottomFL).tag != 1)goto _LL1B;_tmp2C=(int)(_tmp2B.BottomFL).val;
_LL1A: return Cyc_CfFlowInfo_BottomFL();_LL1B: if((_tmp2B.ReachableFL).tag != 2)goto
_LL18;_tmp2D=(struct _tuple12)(_tmp2B.ReachableFL).val;_tmp2E=_tmp2D.f1;_tmp2F=
_tmp2D.f2;_LL1C: for(0;vds != 0;vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct
_tmp774;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp773;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp30=(_tmp773=_region_malloc(fenv->r,sizeof(*_tmp773)),((_tmp773[0]=((_tmp774.tag=
0,((_tmp774.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp774)))),_tmp773)));_tmp2E=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp2E,(void*)_tmp30,Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)
vds->hd)->type,0,leafval));}return Cyc_CfFlowInfo_ReachableFL(_tmp2E,_tmp2F);
_LL18:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp33=inflow;int _tmp34;struct _tuple12 _tmp35;struct
Cyc_Dict_Dict _tmp36;struct Cyc_List_List*_tmp37;_LL1E: if((_tmp33.BottomFL).tag != 
1)goto _LL20;_tmp34=(int)(_tmp33.BottomFL).val;_LL1F: return Cyc_CfFlowInfo_BottomFL();
_LL20: if((_tmp33.ReachableFL).tag != 2)goto _LL1D;_tmp35=(struct _tuple12)(_tmp33.ReachableFL).val;
_tmp36=_tmp35.f1;_tmp37=_tmp35.f2;_LL21: if(Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp36,r)!= Cyc_CfFlowInfo_AllIL){const char*_tmp777;void*_tmp776;(_tmp776=0,Cyc_Tcutil_terr(
loc,((_tmp777="expression may not be fully initialized",_tag_dyneither(_tmp777,
sizeof(char),40))),_tag_dyneither(_tmp776,sizeof(void*),0)));}{struct Cyc_Dict_Dict
_tmp3A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp36,env->all_changed,r);if(
_tmp36.t == _tmp3A.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp3B=Cyc_CfFlowInfo_ReachableFL(
_tmp3A,_tmp37);Cyc_NewControlFlow_update_tryflow(env,_tmp3B);return _tmp3B;};};
_LL1D:;}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_NewControlFlow_check_nounique(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,void*
r){struct _tuple0 _tmp778;struct _tuple0 _tmp3D=(_tmp778.f1=Cyc_Tcutil_compress(t),((
_tmp778.f2=r,_tmp778)));void*_tmp3E;enum Cyc_CfFlowInfo_InitLevel _tmp40;void*
_tmp41;void*_tmp43;void*_tmp45;struct Cyc_Absyn_DatatypeFieldInfo _tmp47;union Cyc_Absyn_DatatypeFieldInfoU
_tmp48;struct _tuple2 _tmp49;struct Cyc_Absyn_Datatypefield*_tmp4A;void*_tmp4B;
struct _dyneither_ptr _tmp4D;void*_tmp4E;struct Cyc_List_List*_tmp50;void*_tmp51;
struct _dyneither_ptr _tmp53;void*_tmp54;struct Cyc_Absyn_AggrInfo _tmp56;union Cyc_Absyn_AggrInfoU
_tmp57;struct Cyc_List_List*_tmp58;void*_tmp59;struct _dyneither_ptr _tmp5B;void*
_tmp5C;enum Cyc_Absyn_AggrKind _tmp5E;struct Cyc_List_List*_tmp5F;void*_tmp60;
struct _dyneither_ptr _tmp62;void*_tmp63;_LL23: _tmp3E=_tmp3D.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3F=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3E;if(_tmp3F->tag != 3)goto
_LL25;else{_tmp40=_tmp3F->f1;if(_tmp40 != Cyc_CfFlowInfo_NoneIL)goto _LL25;}};
_LL24: return;_LL25: _tmp41=_tmp3D.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp42=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp41;if(_tmp42->tag != 0)goto _LL27;};_LL26:
return;_LL27: _tmp43=_tmp3D.f2;{struct Cyc_CfFlowInfo_Consumed_struct*_tmp44=(
struct Cyc_CfFlowInfo_Consumed_struct*)_tmp43;if(_tmp44->tag != 7)goto _LL29;};
_LL28: return;_LL29: _tmp45=_tmp3D.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp46=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp45;if(_tmp46->tag != 4)goto
_LL2B;else{_tmp47=_tmp46->f1;_tmp48=_tmp47.field_info;if((_tmp48.KnownDatatypefield).tag
!= 2)goto _LL2B;_tmp49=(struct _tuple2)(_tmp48.KnownDatatypefield).val;_tmp4A=
_tmp49.f2;}};_tmp4B=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp4C=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp4B;if(_tmp4C->tag != 6)goto _LL2B;else{
_tmp4D=_tmp4C->f2;}};_LL2A: if(_tmp4A->typs == 0)return;_tmp50=_tmp4A->typs;_tmp53=
_tmp4D;goto _LL2C;_LL2B: _tmp4E=_tmp3D.f1;{struct Cyc_Absyn_TupleType_struct*_tmp4F=(
struct Cyc_Absyn_TupleType_struct*)_tmp4E;if(_tmp4F->tag != 11)goto _LL2D;else{
_tmp50=_tmp4F->f1;}};_tmp51=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp52=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp51;if(_tmp52->tag != 6)goto
_LL2D;else{_tmp53=_tmp52->f2;}};_LL2C: {unsigned int sz=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp50);{int i=0;for(0;i < sz;++ i){Cyc_NewControlFlow_check_nounique(
env,loc,(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp50))->hd)).f2,*((
void**)_check_dyneither_subscript(_tmp53,sizeof(void*),i)));}}return;}_LL2D:
_tmp54=_tmp3D.f1;{struct Cyc_Absyn_AggrType_struct*_tmp55=(struct Cyc_Absyn_AggrType_struct*)
_tmp54;if(_tmp55->tag != 12)goto _LL2F;else{_tmp56=_tmp55->f1;_tmp57=_tmp56.aggr_info;
_tmp58=_tmp56.targs;}};_tmp59=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5A=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp59;if(_tmp5A->tag != 6)goto
_LL2F;else{_tmp5B=_tmp5A->f2;}};_LL2E: {struct Cyc_Absyn_Aggrdecl*_tmp65=Cyc_Absyn_get_known_aggrdecl(
_tmp57);if(_tmp65->impl == 0)return;{struct Cyc_List_List*_tmp66=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp65->impl))->fields;struct _RegionHandle _tmp67=_new_region("temp");
struct _RegionHandle*temp=& _tmp67;_push_region(temp);{struct Cyc_List_List*_tmp68=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp65->tvs,_tmp58);{int i=0;
for(0;i < _get_dyneither_size(_tmp5B,sizeof(void*));(i ++,_tmp66=_tmp66->tl)){void*
t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp66))->hd)->type;
if(_tmp68 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp68,t);Cyc_NewControlFlow_check_nounique(
env,loc,t,((void**)_tmp5B.curr)[i]);}}_npop_handler(0);return;};_pop_region(temp);};}
_LL2F: _tmp5C=_tmp3D.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmp5D=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5C;if(_tmp5D->tag != 13)goto _LL31;else{_tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;}};
_tmp60=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp61=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp60;if(_tmp61->tag != 6)goto _LL31;else{_tmp62=_tmp61->f2;}};_LL30:{int i=0;for(
0;i < _get_dyneither_size(_tmp62,sizeof(void*));(i ++,_tmp5F=_tmp5F->tl)){Cyc_NewControlFlow_check_nounique(
env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)->type,((
void**)_tmp62.curr)[i]);}}return;_LL31: _tmp63=_tmp3D.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp64=(struct Cyc_Absyn_PointerType_struct*)_tmp63;if(_tmp64->tag != 5)goto _LL33;};
if(!Cyc_Tcutil_is_noalias_pointer(t))goto _LL33;_LL32:{const char*_tmp77B;void*
_tmp77A;(_tmp77A=0,Cyc_Tcutil_warn(loc,((_tmp77B="argument may still contain unique pointers",
_tag_dyneither(_tmp77B,sizeof(char),43))),_tag_dyneither(_tmp77A,sizeof(void*),0)));}
return;_LL33:;_LL34: return;_LL22:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp6B=inflow;
int _tmp6C;struct _tuple12 _tmp6D;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*
_tmp6F;_LL36: if((_tmp6B.BottomFL).tag != 1)goto _LL38;_tmp6C=(int)(_tmp6B.BottomFL).val;
_LL37: return Cyc_CfFlowInfo_BottomFL();_LL38: if((_tmp6B.ReachableFL).tag != 2)goto
_LL35;_tmp6D=(struct _tuple12)(_tmp6B.ReachableFL).val;_tmp6E=_tmp6D.f1;_tmp6F=
_tmp6D.f2;_LL39: if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp77E;void*
_tmp77D;(_tmp77D=0,Cyc_Tcutil_terr(loc,((_tmp77E="noliveunique attribute requires unique pointer",
_tag_dyneither(_tmp77E,sizeof(char),47))),_tag_dyneither(_tmp77D,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{void*_tmp72=Cyc_Tcutil_pointer_elt_type(t);{
void*_tmp73=r;struct Cyc_CfFlowInfo_Place*_tmp75;_LL3B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp74=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp73;if(_tmp74->tag != 5)goto
_LL3D;else{_tmp75=_tmp74->f1;}}_LL3C: Cyc_NewControlFlow_check_nounique(env,loc,
_tmp72,Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp75));goto _LL3A;_LL3D:;_LL3E: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,_tmp72)){const char*_tmp781;void*_tmp780;(_tmp780=0,Cyc_Tcutil_warn(
loc,((_tmp781="argument may contain live unique pointers",_tag_dyneither(_tmp781,
sizeof(char),42))),_tag_dyneither(_tmp780,sizeof(void*),0)));}return Cyc_NewControlFlow_use_Rval(
env,loc,inflow,r);_LL3A:;}{struct Cyc_Dict_Dict _tmp78=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp6E,env->all_changed,r);if(_tmp6E.t == _tmp78.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo _tmp79=Cyc_CfFlowInfo_ReachableFL(_tmp78,_tmp6F);Cyc_NewControlFlow_update_tryflow(
env,_tmp79);return _tmp79;};};};_LL35:;}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo
f1;struct Cyc_List_List*f2;};static struct _tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){if(es == 0){
struct _tuple19 _tmp782;return(_tmp782.f1=inflow,((_tmp782.f2=0,_tmp782)));}if(es->tl
== 0){union Cyc_CfFlowInfo_FlowInfo _tmp7C;void*_tmp7D;struct _tuple13 _tmp7B=Cyc_NewControlFlow_anal_Rexp(
env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp7C=_tmp7B.f1;_tmp7D=
_tmp7B.f2;{struct Cyc_List_List*_tmp785;struct _tuple19 _tmp784;return(_tmp784.f1=
_tmp7C,((_tmp784.f2=((_tmp785=_region_malloc(rgn,sizeof(*_tmp785)),((_tmp785->hd=
_tmp7D,((_tmp785->tl=0,_tmp785)))))),_tmp784)));};}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo
old_inflow;union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_List_List*rvals;do{
this_all_changed=(env->fenv)->mt_place_set;{struct Cyc_Dict_Dict*_tmp786;env->all_changed=((
_tmp786=_region_malloc(env->r,sizeof(*_tmp786)),((_tmp786[0]=(env->fenv)->mt_place_set,
_tmp786))));}{union Cyc_CfFlowInfo_FlowInfo _tmp82;void*_tmp83;struct _tuple13
_tmp81=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;outflow=_tmp82;{struct Cyc_List_List*
_tmp787;rvals=((_tmp787=_region_malloc(rgn,sizeof(*_tmp787)),((_tmp787->hd=
_tmp83,((_tmp787->tl=0,_tmp787))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*
_tmp788;env->all_changed=((_tmp788=_region_malloc(env->r,sizeof(*_tmp788)),((
_tmp788[0]=(env->fenv)->mt_place_set,_tmp788))));}{union Cyc_CfFlowInfo_FlowInfo
_tmp87;void*_tmp88;struct _tuple13 _tmp86=Cyc_NewControlFlow_anal_Rexp(env,
others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp87=_tmp86.f1;_tmp88=
_tmp86.f2;{struct Cyc_List_List*_tmp789;rvals=((_tmp789=_region_malloc(rgn,
sizeof(*_tmp789)),((_tmp789->hd=_tmp88,((_tmp789->tl=rvals,_tmp789))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp87,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
old_inflow=inflow;inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,
inflow,outflow);};}while(!Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*_tmp78A;env->all_changed=((
_tmp78A=_region_malloc(env->r,sizeof(*_tmp78A)),((_tmp78A[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0),_tmp78A))));}Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple19 _tmp78B;return(_tmp78B.f1=outflow,((_tmp78B.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp78B)));};};}
static struct _tuple13 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){struct
_RegionHandle*_tmp8C=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp8E;struct Cyc_List_List*
_tmp8F;struct _tuple19 _tmp8D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp8C,env,
inflow,es,0,0);_tmp8E=_tmp8D.f1;_tmp8F=_tmp8D.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp90=_tmp8E;struct _tuple12 _tmp91;struct Cyc_Dict_Dict _tmp92;int _tmp93;_LL40: if((
_tmp90.ReachableFL).tag != 2)goto _LL42;_tmp91=(struct _tuple12)(_tmp90.ReachableFL).val;
_tmp92=_tmp91.f1;_LL41: for(0;_tmp8F != 0;(_tmp8F=_tmp8F->tl,es=((struct Cyc_List_List*)
_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp92,(void*)
_tmp8F->hd)== Cyc_CfFlowInfo_NoneIL){const char*_tmp78E;void*_tmp78D;(_tmp78D=0,
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((
_tmp78E="expression may not be initialized",_tag_dyneither(_tmp78E,sizeof(char),
34))),_tag_dyneither(_tmp78D,sizeof(void*),0)));}}goto _LL3F;_LL42: if((_tmp90.BottomFL).tag
!= 1)goto _LL3F;_tmp93=(int)(_tmp90.BottomFL).val;_LL43: goto _LL3F;_LL3F:;}{struct
_tuple13 _tmp78F;return(_tmp78F.f1=_tmp8E,((_tmp78F.f2=(env->fenv)->unknown_all,
_tmp78F)));};}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmp97=outflow;int _tmp98;struct _tuple12 _tmp99;struct Cyc_Dict_Dict _tmp9A;struct
Cyc_List_List*_tmp9B;_LL45: if((_tmp97.BottomFL).tag != 1)goto _LL47;_tmp98=(int)(
_tmp97.BottomFL).val;_LL46: return outflow;_LL47: if((_tmp97.ReachableFL).tag != 2)
goto _LL44;_tmp99=(struct _tuple12)(_tmp97.ReachableFL).val;_tmp9A=_tmp99.f1;
_tmp9B=_tmp99.f2;_LL48: {union Cyc_CfFlowInfo_AbsLVal _tmp9C=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,e)).f2;int _tmp9D;struct Cyc_CfFlowInfo_Place*_tmp9E;_LL4A: if((_tmp9C.UnknownL).tag
!= 2)goto _LL4C;_tmp9D=(int)(_tmp9C.UnknownL).val;_LL4B: return outflow;_LL4C: if((
_tmp9C.PlaceL).tag != 1)goto _LL49;_tmp9E=(struct Cyc_CfFlowInfo_Place*)(_tmp9C.PlaceL).val;
_LL4D: {void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
union Cyc_CfFlowInfo_FlowInfo _tmp9F=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp9A,env->all_changed,_tmp9E,nzval),_tmp9B);return _tmp9F;}
_LL49:;}_LL44:;}static struct _tuple15 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,
struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmpA0=outflow;int _tmpA1;struct _tuple12 _tmpA2;struct Cyc_Dict_Dict _tmpA3;struct
Cyc_List_List*_tmpA4;_LL4F: if((_tmpA0.BottomFL).tag != 1)goto _LL51;_tmpA1=(int)(
_tmpA0.BottomFL).val;_LL50: {struct _tuple15 _tmp790;return(_tmp790.f1=outflow,((
_tmp790.f2=outflow,_tmp790)));}_LL51: if((_tmpA0.ReachableFL).tag != 2)goto _LL4E;
_tmpA2=(struct _tuple12)(_tmpA0.ReachableFL).val;_tmpA3=_tmpA2.f1;_tmpA4=_tmpA2.f2;
_LL52: {union Cyc_CfFlowInfo_AbsLVal _tmpA6=(Cyc_NewControlFlow_anal_Lexp(env,
inflow,0,e)).f2;int _tmpA7;struct Cyc_CfFlowInfo_Place*_tmpA8;_LL54: if((_tmpA6.UnknownL).tag
!= 2)goto _LL56;_tmpA7=(int)(_tmpA6.UnknownL).val;_LL55: {struct _tuple15 _tmp791;
return(_tmp791.f1=outflow,((_tmp791.f2=outflow,_tmp791)));}_LL56: if((_tmpA6.PlaceL).tag
!= 1)goto _LL53;_tmpA8=(struct Cyc_CfFlowInfo_Place*)(_tmpA6.PlaceL).val;_LL57: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
struct _tuple15 _tmp792;return(_tmp792.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,nzval),_tmpA4),((_tmp792.f2=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,(env->fenv)->zero),
_tmpA4),_tmp792)));}_LL53:;}_LL4E:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple13 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
f,struct Cyc_Absyn_Exp*e,void*r){void*_tmpAD=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAF;void*
_tmpB0;struct Cyc_Absyn_PtrAtts _tmpB1;union Cyc_Absyn_Constraint*_tmpB2;union Cyc_Absyn_Constraint*
_tmpB3;_LL59: {struct Cyc_Absyn_PointerType_struct*_tmpAE=(struct Cyc_Absyn_PointerType_struct*)
_tmpAD;if(_tmpAE->tag != 5)goto _LL5B;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAF.elt_typ;
_tmpB1=_tmpAF.ptr_atts;_tmpB2=_tmpB1.bounds;_tmpB3=_tmpB1.zero_term;}}_LL5A: {
union Cyc_CfFlowInfo_FlowInfo _tmpB4=f;int _tmpB5;struct _tuple12 _tmpB6;struct Cyc_Dict_Dict
_tmpB7;struct Cyc_List_List*_tmpB8;_LL5E: if((_tmpB4.BottomFL).tag != 1)goto _LL60;
_tmpB5=(int)(_tmpB4.BottomFL).val;_LL5F: {struct _tuple13 _tmp793;return(_tmp793.f1=
f,((_tmp793.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,0,(env->fenv)->unknown_all),
_tmp793)));}_LL60: if((_tmpB4.ReachableFL).tag != 2)goto _LL5D;_tmpB6=(struct
_tuple12)(_tmpB4.ReachableFL).val;_tmpB7=_tmpB6.f1;_tmpB8=_tmpB6.f2;_LL61: if(Cyc_Tcutil_is_bound_one(
_tmpB2)){void*_tmpBA=r;struct Cyc_CfFlowInfo_Place*_tmpBE;enum Cyc_CfFlowInfo_InitLevel
_tmpC1;_LL63: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmpBB=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmpBA;if(_tmpBB->tag != 1)goto _LL65;}_LL64: goto _LL66;_LL65: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmpBC=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmpBA;if(_tmpBC->tag != 2)goto
_LL67;}_LL66:{void*_tmpC2=e->annot;struct Cyc_List_List*_tmpC4;_LL70: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmpC3=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmpC2;if(_tmpC3->tag != Cyc_CfFlowInfo_NotZero)
goto _LL72;else{_tmpC4=_tmpC3->f1;}}_LL71: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,
_tmpC4))goto _LL73;goto _LL6F;_LL72:;_LL73:{void*_tmpC5=e->r;_LL75: {struct Cyc_Absyn_Subscript_e_struct*
_tmpC6=(struct Cyc_Absyn_Subscript_e_struct*)_tmpC5;if(_tmpC6->tag != 24)goto _LL77;}
_LL76:{struct Cyc_CfFlowInfo_NotZero_struct _tmp796;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp795;e->annot=(void*)((_tmp795=_cycalloc(sizeof(*_tmp795)),((_tmp795[0]=((
_tmp796.tag=Cyc_CfFlowInfo_NotZero,((_tmp796.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp796)))),_tmp795))));}goto _LL74;_LL77:;_LL78: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL74;_LL74:;}goto _LL6F;_LL6F:;}goto _LL62;_LL67: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpBD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpBA;if(_tmpBD->tag != 5)goto
_LL69;else{_tmpBE=_tmpBD->f1;}}_LL68:{void*_tmpC9=e->annot;struct Cyc_List_List*
_tmpCB;_LL7A: {struct Cyc_CfFlowInfo_NotZero_struct*_tmpCA=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpC9;if(_tmpCA->tag != Cyc_CfFlowInfo_NotZero)goto _LL7C;else{_tmpCB=_tmpCA->f1;}}
_LL7B: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpCB))goto _LL7D;goto _LL79;_LL7C:;
_LL7D:{void*_tmpCC=e->r;_LL7F: {struct Cyc_Absyn_Subscript_e_struct*_tmpCD=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpCC;if(_tmpCD->tag != 24)goto _LL81;}_LL80:{
struct Cyc_CfFlowInfo_NotZero_struct _tmp799;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp798;e->annot=(void*)((_tmp798=_cycalloc(sizeof(*_tmp798)),((_tmp798[0]=((
_tmp799.tag=Cyc_CfFlowInfo_NotZero,((_tmp799.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp799)))),_tmp798))));}goto _LL7E;_LL81:;_LL82: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL7E;_LL7E:;}goto _LL79;_LL79:;}{struct _tuple13 _tmp79A;return(_tmp79A.f1=f,((
_tmp79A.f2=Cyc_CfFlowInfo_lookup_place(_tmpB7,_tmpBE),_tmp79A)));};_LL69: {
struct Cyc_CfFlowInfo_Zero_struct*_tmpBF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmpBA;if(_tmpBF->tag != 0)goto _LL6B;}_LL6A: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple13 _tmp79B;return(_tmp79B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp79B.f2=
Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,0,(env->fenv)->unknown_all),
_tmp79B)));};_LL6B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpC0=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpBA;if(_tmpC0->tag != 3)goto _LL6D;else{_tmpC1=_tmpC0->f1;}}_LL6C: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmpC1);goto _LL6E;_LL6D:;_LL6E:{void*_tmpD2=e->r;_LL84: {struct
Cyc_Absyn_Subscript_e_struct*_tmpD3=(struct Cyc_Absyn_Subscript_e_struct*)_tmpD2;
if(_tmpD3->tag != 24)goto _LL86;}_LL85:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp79E;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp79D;e->annot=(void*)((_tmp79D=
_cycalloc(sizeof(*_tmp79D)),((_tmp79D[0]=((_tmp79E.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp79E.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpB8),_tmp79E)))),
_tmp79D))));}goto _LL83;_LL86:;_LL87: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL83;_LL83:;}goto _LL62;_LL62:;}else{void*_tmpD6=e->annot;struct Cyc_List_List*
_tmpD8;_LL89: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpD7=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmpD6;if(_tmpD7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL8B;else{_tmpD8=_tmpD7->f1;}}
_LL8A: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpD8))goto _LL8C;goto _LL88;_LL8B:;
_LL8C:{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp7A1;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp7A0;e->annot=(void*)((_tmp7A0=_cycalloc(sizeof(*_tmp7A0)),((_tmp7A0[0]=((
_tmp7A1.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7A1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp7A1)))),_tmp7A0))));}goto _LL88;_LL88:;}switch(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmpB7,r)){case Cyc_CfFlowInfo_NoneIL: _LL8D:{void*_tmpDB=r;_LL90: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmpDC=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpDB;if(_tmpDC->tag != 7)goto _LL92;}_LL91:{const char*_tmp7A4;void*_tmp7A3;(
_tmp7A3=0,Cyc_Tcutil_terr(e->loc,((_tmp7A4="attempt to use a unique pointer that has already been copied",
_tag_dyneither(_tmp7A4,sizeof(char),61))),_tag_dyneither(_tmp7A3,sizeof(void*),0)));}
goto _LL8F;_LL92:;_LL93: {const char*_tmp7A7;void*_tmp7A6;(_tmp7A6=0,Cyc_Tcutil_terr(
e->loc,((_tmp7A7="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp7A7,sizeof(char),46))),_tag_dyneither(_tmp7A6,sizeof(void*),0)));}_LL8F:;}
goto _LL8E;case Cyc_CfFlowInfo_AllIL: _LL8E: {struct _tuple13 _tmp7A8;return(_tmp7A8.f1=
f,((_tmp7A8.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,0,(env->fenv)->unknown_all),
_tmp7A8)));}case Cyc_CfFlowInfo_ThisIL: _LL94: {struct _tuple13 _tmp7A9;return(
_tmp7A9.f1=f,((_tmp7A9.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,0,(env->fenv)->unknown_none),
_tmp7A9)));}}_LL5D:;}_LL5B:;_LL5C: {struct Cyc_Core_Impossible_struct _tmp7AF;
const char*_tmp7AE;struct Cyc_Core_Impossible_struct*_tmp7AD;(int)_throw((void*)((
_tmp7AD=_cycalloc(sizeof(*_tmp7AD)),((_tmp7AD[0]=((_tmp7AF.tag=Cyc_Core_Impossible,((
_tmp7AF.f1=((_tmp7AE="right deref of non-pointer-type",_tag_dyneither(_tmp7AE,
sizeof(char),32))),_tmp7AF)))),_tmp7AD)))));}_LL58:;}static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpE6=e1->r;void*
_tmpE8;struct Cyc_Absyn_Vardecl*_tmpEA;void*_tmpEC;struct Cyc_Absyn_Vardecl*_tmpEE;
void*_tmpF0;struct Cyc_Absyn_Vardecl*_tmpF2;void*_tmpF4;struct Cyc_Absyn_Vardecl*
_tmpF6;_LL97: {struct Cyc_Absyn_Var_e_struct*_tmpE7=(struct Cyc_Absyn_Var_e_struct*)
_tmpE6;if(_tmpE7->tag != 1)goto _LL99;else{_tmpE8=(void*)_tmpE7->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmpE9=(struct Cyc_Absyn_Pat_b_struct*)_tmpE8;if(_tmpE9->tag != 5)goto _LL99;else{
_tmpEA=_tmpE9->f1;}};}}_LL98: _tmpEE=_tmpEA;goto _LL9A;_LL99: {struct Cyc_Absyn_Var_e_struct*
_tmpEB=(struct Cyc_Absyn_Var_e_struct*)_tmpE6;if(_tmpEB->tag != 1)goto _LL9B;else{
_tmpEC=(void*)_tmpEB->f2;{struct Cyc_Absyn_Local_b_struct*_tmpED=(struct Cyc_Absyn_Local_b_struct*)
_tmpEC;if(_tmpED->tag != 4)goto _LL9B;else{_tmpEE=_tmpED->f1;}};}}_LL9A: _tmpF2=
_tmpEE;goto _LL9C;_LL9B: {struct Cyc_Absyn_Var_e_struct*_tmpEF=(struct Cyc_Absyn_Var_e_struct*)
_tmpE6;if(_tmpEF->tag != 1)goto _LL9D;else{_tmpF0=(void*)_tmpEF->f2;{struct Cyc_Absyn_Param_b_struct*
_tmpF1=(struct Cyc_Absyn_Param_b_struct*)_tmpF0;if(_tmpF1->tag != 3)goto _LL9D;
else{_tmpF2=_tmpF1->f1;}};}}_LL9C: _tmpF6=_tmpF2;goto _LL9E;_LL9D: {struct Cyc_Absyn_Var_e_struct*
_tmpF3=(struct Cyc_Absyn_Var_e_struct*)_tmpE6;if(_tmpF3->tag != 1)goto _LL9F;else{
_tmpF4=(void*)_tmpF3->f2;{struct Cyc_Absyn_Global_b_struct*_tmpF5=(struct Cyc_Absyn_Global_b_struct*)
_tmpF4;if(_tmpF5->tag != 1)goto _LL9F;else{_tmpF6=_tmpF5->f1;}};}}_LL9E: if(!_tmpF6->escapes){
void*_tmpF7=e2->r;void*_tmpF9;struct Cyc_Absyn_Vardecl*_tmpFB;void*_tmpFD;struct
Cyc_Absyn_Vardecl*_tmpFF;void*_tmp101;struct Cyc_Absyn_Vardecl*_tmp103;void*
_tmp105;struct Cyc_Absyn_Vardecl*_tmp107;_LLA2: {struct Cyc_Absyn_Var_e_struct*
_tmpF8=(struct Cyc_Absyn_Var_e_struct*)_tmpF7;if(_tmpF8->tag != 1)goto _LLA4;else{
_tmpF9=(void*)_tmpF8->f2;{struct Cyc_Absyn_Pat_b_struct*_tmpFA=(struct Cyc_Absyn_Pat_b_struct*)
_tmpF9;if(_tmpFA->tag != 5)goto _LLA4;else{_tmpFB=_tmpFA->f1;}};}}_LLA3: _tmpFF=
_tmpFB;goto _LLA5;_LLA4: {struct Cyc_Absyn_Var_e_struct*_tmpFC=(struct Cyc_Absyn_Var_e_struct*)
_tmpF7;if(_tmpFC->tag != 1)goto _LLA6;else{_tmpFD=(void*)_tmpFC->f2;{struct Cyc_Absyn_Local_b_struct*
_tmpFE=(struct Cyc_Absyn_Local_b_struct*)_tmpFD;if(_tmpFE->tag != 4)goto _LLA6;
else{_tmpFF=_tmpFE->f1;}};}}_LLA5: _tmp103=_tmpFF;goto _LLA7;_LLA6: {struct Cyc_Absyn_Var_e_struct*
_tmp100=(struct Cyc_Absyn_Var_e_struct*)_tmpF7;if(_tmp100->tag != 1)goto _LLA8;
else{_tmp101=(void*)_tmp100->f2;{struct Cyc_Absyn_Param_b_struct*_tmp102=(struct
Cyc_Absyn_Param_b_struct*)_tmp101;if(_tmp102->tag != 3)goto _LLA8;else{_tmp103=
_tmp102->f1;}};}}_LLA7: _tmp107=_tmp103;goto _LLA9;_LLA8: {struct Cyc_Absyn_Var_e_struct*
_tmp104=(struct Cyc_Absyn_Var_e_struct*)_tmpF7;if(_tmp104->tag != 1)goto _LLAA;
else{_tmp105=(void*)_tmp104->f2;{struct Cyc_Absyn_Global_b_struct*_tmp106=(struct
Cyc_Absyn_Global_b_struct*)_tmp105;if(_tmp106->tag != 1)goto _LLAA;else{_tmp107=
_tmp106->f1;}};}}_LLA9: if(!_tmp107->escapes){int found=0;{struct Cyc_List_List*
_tmp108=relns;for(0;_tmp108 != 0;_tmp108=_tmp108->tl){struct Cyc_CfFlowInfo_Reln*
_tmp109=(struct Cyc_CfFlowInfo_Reln*)_tmp108->hd;if(_tmp109->vd == _tmp107){union
Cyc_CfFlowInfo_RelnOp _tmp10A=_tmp109->rop;struct Cyc_Absyn_Vardecl*_tmp10B;_LLAD:
if((_tmp10A.LessNumelts).tag != 3)goto _LLAF;_tmp10B=(struct Cyc_Absyn_Vardecl*)(
_tmp10A.LessNumelts).val;if(!(_tmp10B == _tmpF6))goto _LLAF;_LLAE: return relns;
_LLAF:;_LLB0: continue;_LLAC:;}}}if(!found){struct Cyc_CfFlowInfo_Reln*_tmp7B2;
struct Cyc_List_List*_tmp7B1;return(_tmp7B1=_region_malloc(rgn,sizeof(*_tmp7B1)),((
_tmp7B1->hd=((_tmp7B2=_region_malloc(rgn,sizeof(*_tmp7B2)),((_tmp7B2->vd=_tmp107,((
_tmp7B2->rop=Cyc_CfFlowInfo_LessNumelts(_tmpF6),_tmp7B2)))))),((_tmp7B1->tl=
relns,_tmp7B1)))));}}return relns;_LLAA:;_LLAB: return relns;_LLA1:;}else{return
relns;}_LL9F:;_LLA0: return relns;_LL96:;}static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo
_tmp10E=outflow;int _tmp10F;struct _tuple12 _tmp110;struct Cyc_Dict_Dict _tmp111;
struct Cyc_List_List*_tmp112;_LLB2: if((_tmp10E.BottomFL).tag != 1)goto _LLB4;
_tmp10F=(int)(_tmp10E.BottomFL).val;_LLB3: {struct _tuple13 _tmp7B3;return(_tmp7B3.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp7B3.f2=rval,_tmp7B3)));}_LLB4: if((_tmp10E.ReachableFL).tag
!= 2)goto _LLB1;_tmp110=(struct _tuple12)(_tmp10E.ReachableFL).val;_tmp111=_tmp110.f1;
_tmp112=_tmp110.f2;_LLB5: {union Cyc_CfFlowInfo_AbsLVal _tmp114=lval;struct Cyc_CfFlowInfo_Place*
_tmp115;int _tmp116;_LLB7: if((_tmp114.PlaceL).tag != 1)goto _LLB9;_tmp115=(struct
Cyc_CfFlowInfo_Place*)(_tmp114.PlaceL).val;_LLB8: {struct Cyc_Dict_Dict _tmp117=
Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp111,env->all_changed,_tmp115,rval);
_tmp112=Cyc_CfFlowInfo_reln_assign_exp(fenv->r,_tmp112,lexp,rexp);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp117,_tmp112);if(Cyc_NewControlFlow_warn_lose_unique  && Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,(void*)((struct Cyc_Core_Opt*)_check_null(lexp->topt))->v)){void*
_tmp118=Cyc_CfFlowInfo_lookup_place(_tmp111,_tmp115);enum Cyc_CfFlowInfo_InitLevel
_tmp11A;_LLBC: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp119=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp118;if(_tmp119->tag != 3)goto _LLBE;else{_tmp11A=_tmp119->f1;if(_tmp11A != Cyc_CfFlowInfo_NoneIL)
goto _LLBE;}}_LLBD: goto _LLBF;_LLBE: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp11B=(
struct Cyc_CfFlowInfo_Consumed_struct*)_tmp118;if(_tmp11B->tag != 7)goto _LLC0;}
_LLBF: goto _LLC1;_LLC0: {struct Cyc_CfFlowInfo_Zero_struct*_tmp11C=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp118;if(_tmp11C->tag != 0)goto _LLC2;}_LLC1: goto _LLBB;_LLC2:;_LLC3:{const char*
_tmp7B6;void*_tmp7B5;(_tmp7B5=0,Cyc_Tcutil_warn(lexp->loc,((_tmp7B6="assignment may overwrite unique pointer(s)",
_tag_dyneither(_tmp7B6,sizeof(char),43))),_tag_dyneither(_tmp7B5,sizeof(void*),0)));}
goto _LLBB;_LLBB:;}Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple13
_tmp7B7;return(_tmp7B7.f1=outflow,((_tmp7B7.f2=rval,_tmp7B7)));};}_LLB9: if((
_tmp114.UnknownL).tag != 2)goto _LLB6;_tmp116=(int)(_tmp114.UnknownL).val;_LLBA: {
struct _tuple13 _tmp7B8;return(_tmp7B8.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,
outflow,rval),((_tmp7B8.f2=rval,_tmp7B8)));}_LLB6:;}_LLB1:;}static int Cyc_NewControlFlow_is_local_var_rooted_path(
struct Cyc_Absyn_Exp*e){void*_tmp121=e->r;void*_tmp123;void*_tmp126;void*_tmp129;
struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*
_tmp130;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp134;_LLC5: {struct
Cyc_Absyn_Var_e_struct*_tmp122=(struct Cyc_Absyn_Var_e_struct*)_tmp121;if(_tmp122->tag
!= 1)goto _LLC7;else{_tmp123=(void*)_tmp122->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp124=(struct Cyc_Absyn_Param_b_struct*)_tmp123;if(_tmp124->tag != 3)goto _LLC7;};}}
_LLC6: goto _LLC8;_LLC7: {struct Cyc_Absyn_Var_e_struct*_tmp125=(struct Cyc_Absyn_Var_e_struct*)
_tmp121;if(_tmp125->tag != 1)goto _LLC9;else{_tmp126=(void*)_tmp125->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp127=(struct Cyc_Absyn_Local_b_struct*)_tmp126;if(_tmp127->tag != 4)goto _LLC9;};}}
_LLC8: goto _LLCA;_LLC9: {struct Cyc_Absyn_Var_e_struct*_tmp128=(struct Cyc_Absyn_Var_e_struct*)
_tmp121;if(_tmp128->tag != 1)goto _LLCB;else{_tmp129=(void*)_tmp128->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp12A=(struct Cyc_Absyn_Pat_b_struct*)_tmp129;if(_tmp12A->tag != 5)goto _LLCB;};}}
_LLCA: return 1;_LLCB: {struct Cyc_Absyn_Deref_e_struct*_tmp12B=(struct Cyc_Absyn_Deref_e_struct*)
_tmp121;if(_tmp12B->tag != 21)goto _LLCD;else{_tmp12C=_tmp12B->f1;}}_LLCC: _tmp12E=
_tmp12C;goto _LLCE;_LLCD: {struct Cyc_Absyn_AggrMember_e_struct*_tmp12D=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp121;if(_tmp12D->tag != 22)goto _LLCF;else{_tmp12E=_tmp12D->f1;}}_LLCE: _tmp130=
_tmp12E;goto _LLD0;_LLCF: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp12F=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp121;if(_tmp12F->tag != 23)goto _LLD1;else{_tmp130=_tmp12F->f1;}}_LLD0: return
Cyc_NewControlFlow_is_local_var_rooted_path(_tmp130);_LLD1: {struct Cyc_Absyn_Subscript_e_struct*
_tmp131=(struct Cyc_Absyn_Subscript_e_struct*)_tmp121;if(_tmp131->tag != 24)goto
_LLD3;else{_tmp132=_tmp131->f1;}}_LLD2: {void*_tmp135=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp132->topt))->v);_LLD8: {struct Cyc_Absyn_TupleType_struct*
_tmp136=(struct Cyc_Absyn_TupleType_struct*)_tmp135;if(_tmp136->tag != 11)goto
_LLDA;}_LLD9: return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp132);_LLDA:;
_LLDB: return 0;_LLD7:;}_LLD3: {struct Cyc_Absyn_Cast_e_struct*_tmp133=(struct Cyc_Absyn_Cast_e_struct*)
_tmp121;if(_tmp133->tag != 15)goto _LLD5;else{_tmp134=_tmp133->f2;}}_LLD4: return
Cyc_NewControlFlow_is_local_var_rooted_path(_tmp134);_LLD5:;_LLD6: return 0;_LLC4:;}
static struct _tuple13 Cyc_NewControlFlow_anal_stmt_exp(struct Cyc_NewControlFlow_AnalEnv*
env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_CfFlowInfo_FlowEnv*_tmp137=env->fenv;while(1){union Cyc_CfFlowInfo_FlowInfo*
_tmp139;struct _tuple16 _tmp138=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);
_tmp139=_tmp138.f2;inflow=*_tmp139;{void*_tmp13A=s->r;struct Cyc_Absyn_Stmt*
_tmp13C;struct Cyc_Absyn_Stmt*_tmp13D;struct Cyc_Absyn_Decl*_tmp13F;struct Cyc_Absyn_Decl
_tmp140;void*_tmp141;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Tvar*_tmp144;
struct Cyc_Absyn_Vardecl*_tmp145;struct Cyc_Absyn_Stmt*_tmp146;struct Cyc_Absyn_Decl*
_tmp148;struct Cyc_Absyn_Stmt*_tmp149;struct Cyc_Absyn_Exp*_tmp14B;_LLDD: {struct
Cyc_Absyn_Seq_s_struct*_tmp13B=(struct Cyc_Absyn_Seq_s_struct*)_tmp13A;if(_tmp13B->tag
!= 2)goto _LLDF;else{_tmp13C=_tmp13B->f1;_tmp13D=_tmp13B->f2;}}_LLDE: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp13C);s=_tmp13D;goto _LLDC;_LLDF: {struct Cyc_Absyn_Decl_s_struct*
_tmp13E=(struct Cyc_Absyn_Decl_s_struct*)_tmp13A;if(_tmp13E->tag != 12)goto _LLE1;
else{_tmp13F=_tmp13E->f1;_tmp140=*_tmp13F;_tmp141=_tmp140.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp142=(struct Cyc_Absyn_Alias_d_struct*)_tmp141;if(_tmp142->tag != 5)goto _LLE1;
else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;_tmp145=_tmp142->f3;}};_tmp146=
_tmp13E->f2;}}_LLE0: {union Cyc_CfFlowInfo_FlowInfo _tmp14D;union Cyc_CfFlowInfo_AbsLVal
_tmp14E;struct _tuple14 _tmp14C=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp143);
_tmp14D=_tmp14C.f1;_tmp14E=_tmp14C.f2;{struct _tuple14 _tmp7B9;struct _tuple14
_tmp150=(_tmp7B9.f1=_tmp14D,((_tmp7B9.f2=_tmp14E,_tmp7B9)));union Cyc_CfFlowInfo_FlowInfo
_tmp151;struct _tuple12 _tmp152;struct Cyc_Dict_Dict _tmp153;struct Cyc_List_List*
_tmp154;union Cyc_CfFlowInfo_AbsLVal _tmp155;struct Cyc_CfFlowInfo_Place*_tmp156;
union Cyc_CfFlowInfo_FlowInfo _tmp157;int _tmp158;_LLE8: _tmp151=_tmp150.f1;if((
_tmp151.ReachableFL).tag != 2)goto _LLEA;_tmp152=(struct _tuple12)(_tmp151.ReachableFL).val;
_tmp153=_tmp152.f1;_tmp154=_tmp152.f2;_tmp155=_tmp150.f2;if((_tmp155.PlaceL).tag
!= 1)goto _LLEA;_tmp156=(struct Cyc_CfFlowInfo_Place*)(_tmp155.PlaceL).val;_LLE9: {
void*_tmp159=Cyc_CfFlowInfo_lookup_place(_tmp153,_tmp156);void*t=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp143->topt))->v;{void*_tmp15A=_tmp143->r;struct Cyc_Absyn_Exp*
_tmp15C;_LLEF: {struct Cyc_Absyn_Cast_e_struct*_tmp15B=(struct Cyc_Absyn_Cast_e_struct*)
_tmp15A;if(_tmp15B->tag != 15)goto _LLF1;else{_tmp15C=_tmp15B->f2;}}_LLF0: t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp15C->topt))->v;goto _LLEE;_LLF1:;_LLF2: goto
_LLEE;_LLEE:;}{void*_tmp15D=Cyc_CfFlowInfo_make_unique_consumed(_tmp137,t,
_tmp143,env->iteration_num,_tmp159);_tmp153=Cyc_CfFlowInfo_assign_place(_tmp137,
_tmp143->loc,_tmp153,env->all_changed,_tmp156,_tmp15D);_tmp14D=Cyc_CfFlowInfo_ReachableFL(
_tmp153,_tmp154);{void*leaf;switch(Cyc_CfFlowInfo_initlevel(_tmp137,_tmp153,
_tmp159)){case Cyc_CfFlowInfo_AllIL: _LLF3: leaf=_tmp137->unknown_all;break;case Cyc_CfFlowInfo_NoneIL:
_LLF4: leaf=_tmp137->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LLF5: leaf=
_tmp137->unknown_this;break;}{void*_tmp15E=Cyc_CfFlowInfo_typ_to_absrval(_tmp137,
t,0,leaf);_tmp14D=Cyc_NewControlFlow_use_Rval(env,_tmp143->loc,_tmp14D,_tmp159);{
struct Cyc_List_List _tmp7BA;struct Cyc_List_List _tmp15F=(_tmp7BA.hd=_tmp145,((
_tmp7BA.tl=0,_tmp7BA)));_tmp14D=Cyc_NewControlFlow_add_vars(_tmp137,_tmp14D,(
struct Cyc_List_List*)& _tmp15F,_tmp137->unknown_all,s->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp161;void*_tmp162;struct _tuple13 _tmp160=Cyc_NewControlFlow_anal_stmt_exp(env,
copy_ctxt,_tmp14D,_tmp146);_tmp161=_tmp160.f1;_tmp162=_tmp160.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp163=_tmp161;struct _tuple12 _tmp164;struct Cyc_Dict_Dict _tmp165;struct Cyc_List_List*
_tmp166;_LLF8: if((_tmp163.ReachableFL).tag != 2)goto _LLFA;_tmp164=(struct _tuple12)(
_tmp163.ReachableFL).val;_tmp165=_tmp164.f1;_tmp166=_tmp164.f2;_LLF9: _tmp165=Cyc_CfFlowInfo_assign_place(
_tmp137,s->loc,_tmp165,env->all_changed,_tmp156,_tmp15E);{union Cyc_CfFlowInfo_FlowInfo
_tmp167=Cyc_CfFlowInfo_ReachableFL(_tmp165,_tmp166);struct _tuple13 _tmp7BB;return(
_tmp7BB.f1=_tmp167,((_tmp7BB.f2=_tmp162,_tmp7BB)));};_LLFA:;_LLFB: {struct
_tuple13 _tmp7BC;return(_tmp7BC.f1=_tmp161,((_tmp7BC.f2=_tmp162,_tmp7BC)));}_LLF7:;};};};};};};}
_LLEA: _tmp157=_tmp150.f1;if((_tmp157.BottomFL).tag != 1)goto _LLEC;_tmp158=(int)(
_tmp157.BottomFL).val;_LLEB: {struct _tuple13 _tmp7BD;return(_tmp7BD.f1=_tmp14D,((
_tmp7BD.f2=_tmp137->unknown_all,_tmp7BD)));}_LLEC:;_LLED:{const char*_tmp7C0;void*
_tmp7BF;(_tmp7BF=0,Cyc_Tcutil_terr(_tmp143->loc,((_tmp7C0="bad alias expression--no unique path found",
_tag_dyneither(_tmp7C0,sizeof(char),43))),_tag_dyneither(_tmp7BF,sizeof(void*),0)));}{
struct _tuple13 _tmp7C1;return(_tmp7C1.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7C1.f2=
_tmp137->unknown_all,_tmp7C1)));};_LLE7:;};}_LLE1: {struct Cyc_Absyn_Decl_s_struct*
_tmp147=(struct Cyc_Absyn_Decl_s_struct*)_tmp13A;if(_tmp147->tag != 12)goto _LLE3;
else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}_LLE2: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp148);s=_tmp149;goto _LLDC;_LLE3: {struct Cyc_Absyn_Exp_s_struct*
_tmp14A=(struct Cyc_Absyn_Exp_s_struct*)_tmp13A;if(_tmp14A->tag != 1)goto _LLE5;
else{_tmp14B=_tmp14A->f1;}}_LLE4: return Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp14B);_LLE5:;_LLE6: {struct Cyc_Core_Impossible_struct _tmp7C7;
const char*_tmp7C6;struct Cyc_Core_Impossible_struct*_tmp7C5;(int)_throw((void*)((
_tmp7C5=_cycalloc(sizeof(*_tmp7C5)),((_tmp7C5[0]=((_tmp7C7.tag=Cyc_Core_Impossible,((
_tmp7C7.f1=((_tmp7C6="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp7C6,
sizeof(char),33))),_tmp7C7)))),_tmp7C5)))));}_LLDC:;};}}struct _tuple20{union Cyc_CfFlowInfo_AbsLVal
f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple21{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);
static void _tmp86C(unsigned int*_tmp86B,unsigned int*_tmp86A,void***_tmp868,
struct Cyc_List_List**_tmp364){for(*_tmp86B=0;*_tmp86B < *_tmp86A;(*_tmp86B)++){
void*_tmp866;(*_tmp868)[*_tmp86B]=((_tmp866=(void*)((struct Cyc_List_List*)
_check_null(*_tmp364))->hd,((*_tmp364=(*_tmp364)->tl,_tmp866))));}}static struct
_tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp172=env->fenv;struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;{union Cyc_CfFlowInfo_FlowInfo
_tmp173=inflow;int _tmp174;struct _tuple12 _tmp175;struct Cyc_Dict_Dict _tmp176;
struct Cyc_List_List*_tmp177;_LLFD: if((_tmp173.BottomFL).tag != 1)goto _LLFF;
_tmp174=(int)(_tmp173.BottomFL).val;_LLFE: {struct _tuple13 _tmp7C8;return(_tmp7C8.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp7C8.f2=_tmp172->unknown_all,_tmp7C8)));}_LLFF:
if((_tmp173.ReachableFL).tag != 2)goto _LLFC;_tmp175=(struct _tuple12)(_tmp173.ReachableFL).val;
_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;_LL100: d=_tmp176;relns=_tmp177;_LLFC:;}if((
copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){union Cyc_CfFlowInfo_FlowInfo
_tmp17A;union Cyc_CfFlowInfo_AbsLVal _tmp17B;struct _tuple14 _tmp179=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,e);_tmp17A=_tmp179.f1;_tmp17B=_tmp179.f2;{struct _tuple14 _tmp7C9;
struct _tuple14 _tmp17D=(_tmp7C9.f1=_tmp17A,((_tmp7C9.f2=_tmp17B,_tmp7C9)));union
Cyc_CfFlowInfo_FlowInfo _tmp17E;struct _tuple12 _tmp17F;struct Cyc_Dict_Dict _tmp180;
struct Cyc_List_List*_tmp181;union Cyc_CfFlowInfo_AbsLVal _tmp182;struct Cyc_CfFlowInfo_Place*
_tmp183;_LL102: _tmp17E=_tmp17D.f1;if((_tmp17E.ReachableFL).tag != 2)goto _LL104;
_tmp17F=(struct _tuple12)(_tmp17E.ReachableFL).val;_tmp180=_tmp17F.f1;_tmp181=
_tmp17F.f2;_tmp182=_tmp17D.f2;if((_tmp182.PlaceL).tag != 1)goto _LL104;_tmp183=(
struct Cyc_CfFlowInfo_Place*)(_tmp182.PlaceL).val;_LL103: {void*_tmp184=Cyc_CfFlowInfo_lookup_place(
_tmp180,_tmp183);{void*_tmp185=_tmp184;struct Cyc_Absyn_Exp*_tmp187;int _tmp188;
void*_tmp189;_LL107: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp186=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp185;if(_tmp186->tag != 7)goto _LL109;else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;
_tmp189=(void*)_tmp186->f3;}}_LL108: if(_tmp187 == e  && _tmp188 == env->iteration_num){
struct _tuple13 _tmp7CA;return(_tmp7CA.f1=_tmp17A,((_tmp7CA.f2=_tmp189,_tmp7CA)));}{
const char*_tmp7CD;void*_tmp7CC;(_tmp7CC=0,Cyc_Tcutil_terr(e->loc,((_tmp7CD="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp7CD,sizeof(char),71))),_tag_dyneither(_tmp7CC,sizeof(void*),0)));}{
struct _tuple13 _tmp7CE;return(_tmp7CE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7CE.f2=
_tmp172->unknown_all,_tmp7CE)));};_LL109:;_LL10A: if(Cyc_CfFlowInfo_is_unique_consumed(
e,env->iteration_num,_tmp184)){{const char*_tmp7D1;void*_tmp7D0;(_tmp7D0=0,Cyc_Tcutil_terr(
e->loc,((_tmp7D1="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp7D1,sizeof(char),71))),_tag_dyneither(_tmp7D0,sizeof(void*),0)));}{
struct _tuple13 _tmp7D2;return(_tmp7D2.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7D2.f2=
_tmp172->unknown_all,_tmp7D2)));};}goto _LL106;_LL106:;}{void*_tmp191=Cyc_CfFlowInfo_make_unique_consumed(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,e,env->iteration_num,
_tmp184);struct Cyc_Dict_Dict _tmp192=Cyc_CfFlowInfo_assign_place(_tmp172,e->loc,
_tmp180,env->all_changed,_tmp183,_tmp191);struct _tuple13 _tmp7D3;return(_tmp7D3.f1=
Cyc_CfFlowInfo_ReachableFL(_tmp192,_tmp181),((_tmp7D3.f2=_tmp184,_tmp7D3)));};}
_LL104:;_LL105: goto _LL101;_LL101:;};}{void*_tmp194=e->r;struct Cyc_Absyn_Exp*
_tmp196;enum Cyc_Absyn_Coercion _tmp197;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*
_tmp19B;struct Cyc_Absyn_Exp*_tmp19D;union Cyc_Absyn_Cnst _tmp19F;int _tmp1A0;union
Cyc_Absyn_Cnst _tmp1A2;struct _tuple6 _tmp1A3;int _tmp1A4;union Cyc_Absyn_Cnst _tmp1A6;
struct _tuple6 _tmp1A7;void*_tmp1A9;struct Cyc_List_List*_tmp1AC;void*_tmp1B4;void*
_tmp1B7;struct Cyc_Absyn_Vardecl*_tmp1B9;void*_tmp1BB;struct Cyc_Absyn_Vardecl*
_tmp1BD;void*_tmp1BF;struct Cyc_Absyn_Vardecl*_tmp1C1;enum Cyc_Absyn_Primop
_tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_Absyn_Exp*
_tmp1C8;struct Cyc_Core_Opt*_tmp1C9;struct Cyc_Core_Opt _tmp1CA;struct Cyc_Absyn_Exp*
_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CD;struct Cyc_Core_Opt*_tmp1CE;struct Cyc_Absyn_Exp*
_tmp1CF;struct Cyc_Absyn_Exp*_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Exp*
_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D6;struct Cyc_List_List*_tmp1D7;struct Cyc_Absyn_MallocInfo
_tmp1D9;int _tmp1DA;struct Cyc_Absyn_Exp*_tmp1DB;void**_tmp1DC;struct Cyc_Absyn_Exp*
_tmp1DD;int _tmp1DE;struct Cyc_Absyn_Exp*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E1;
struct Cyc_Absyn_Exp*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_Absyn_Exp*
_tmp1E6;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1EA;struct
_dyneither_ptr*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1ED;struct _dyneither_ptr*_tmp1EE;
struct Cyc_Absyn_Exp*_tmp1F0;struct _dyneither_ptr*_tmp1F1;struct Cyc_Absyn_Exp*
_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F4;struct Cyc_Absyn_Exp*_tmp1F5;struct Cyc_Absyn_Exp*
_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F8;struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_Absyn_Exp*
_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FD;struct Cyc_Absyn_Exp*_tmp1FE;struct Cyc_List_List*
_tmp200;struct Cyc_Absyn_Datatypedecl*_tmp201;struct Cyc_List_List*_tmp203;struct
Cyc_List_List*_tmp205;struct Cyc_List_List*_tmp207;struct Cyc_Absyn_Aggrdecl*
_tmp208;struct Cyc_Absyn_Aggrdecl _tmp209;enum Cyc_Absyn_AggrKind _tmp20A;struct Cyc_Absyn_AggrdeclImpl*
_tmp20B;struct Cyc_Absyn_AggrdeclImpl _tmp20C;struct Cyc_List_List*_tmp20D;struct
Cyc_List_List*_tmp210;struct Cyc_Absyn_Vardecl*_tmp212;struct Cyc_Absyn_Exp*
_tmp213;struct Cyc_Absyn_Exp*_tmp214;int _tmp215;struct Cyc_Absyn_Stmt*_tmp217;void*
_tmp219;_LL10C: {struct Cyc_Absyn_Cast_e_struct*_tmp195=(struct Cyc_Absyn_Cast_e_struct*)
_tmp194;if(_tmp195->tag != 15)goto _LL10E;else{_tmp196=_tmp195->f2;_tmp197=_tmp195->f4;
if(_tmp197 != Cyc_Absyn_NonNull_to_Null)goto _LL10E;}}_LL10D: {union Cyc_CfFlowInfo_FlowInfo
_tmp222;void*_tmp223;struct _tuple13 _tmp221=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp196);_tmp222=_tmp221.f1;_tmp223=_tmp221.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp225;void*_tmp226;struct _tuple13 _tmp224=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp222,_tmp196,_tmp223);_tmp225=_tmp224.f1;_tmp226=_tmp224.f2;{struct
_tuple13 _tmp7D4;return(_tmp7D4.f1=_tmp225,((_tmp7D4.f2=_tmp223,_tmp7D4)));};};}
_LL10E: {struct Cyc_Absyn_Cast_e_struct*_tmp198=(struct Cyc_Absyn_Cast_e_struct*)
_tmp194;if(_tmp198->tag != 15)goto _LL110;else{_tmp199=_tmp198->f2;}}_LL10F:
_tmp19B=_tmp199;goto _LL111;_LL110: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp19A=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp194;if(_tmp19A->tag != 13)
goto _LL112;else{_tmp19B=_tmp19A->f1;}}_LL111: _tmp19D=_tmp19B;goto _LL113;_LL112: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp19C=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp194;if(_tmp19C->tag != 14)goto _LL114;else{_tmp19D=_tmp19C->f1;}}_LL113: return
Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp19D);_LL114: {struct Cyc_Absyn_Const_e_struct*
_tmp19E=(struct Cyc_Absyn_Const_e_struct*)_tmp194;if(_tmp19E->tag != 0)goto _LL116;
else{_tmp19F=_tmp19E->f1;if((_tmp19F.Null_c).tag != 1)goto _LL116;_tmp1A0=(int)(
_tmp19F.Null_c).val;}}_LL115: goto _LL117;_LL116: {struct Cyc_Absyn_Const_e_struct*
_tmp1A1=(struct Cyc_Absyn_Const_e_struct*)_tmp194;if(_tmp1A1->tag != 0)goto _LL118;
else{_tmp1A2=_tmp1A1->f1;if((_tmp1A2.Int_c).tag != 4)goto _LL118;_tmp1A3=(struct
_tuple6)(_tmp1A2.Int_c).val;_tmp1A4=_tmp1A3.f2;if(_tmp1A4 != 0)goto _LL118;}}
_LL117: {struct _tuple13 _tmp7D5;return(_tmp7D5.f1=inflow,((_tmp7D5.f2=_tmp172->zero,
_tmp7D5)));}_LL118: {struct Cyc_Absyn_Const_e_struct*_tmp1A5=(struct Cyc_Absyn_Const_e_struct*)
_tmp194;if(_tmp1A5->tag != 0)goto _LL11A;else{_tmp1A6=_tmp1A5->f1;if((_tmp1A6.Int_c).tag
!= 4)goto _LL11A;_tmp1A7=(struct _tuple6)(_tmp1A6.Int_c).val;}}_LL119: goto _LL11B;
_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp1A8=(struct Cyc_Absyn_Var_e_struct*)
_tmp194;if(_tmp1A8->tag != 1)goto _LL11C;else{_tmp1A9=(void*)_tmp1A8->f2;{struct
Cyc_Absyn_Funname_b_struct*_tmp1AA=(struct Cyc_Absyn_Funname_b_struct*)_tmp1A9;
if(_tmp1AA->tag != 2)goto _LL11C;};}}_LL11B: {struct _tuple13 _tmp7D6;return(_tmp7D6.f1=
inflow,((_tmp7D6.f2=_tmp172->notzeroall,_tmp7D6)));}_LL11C: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1AB=(struct Cyc_Absyn_Datatype_e_struct*)_tmp194;if(_tmp1AB->tag != 31)goto
_LL11E;else{_tmp1AC=_tmp1AB->f1;if(_tmp1AC != 0)goto _LL11E;}}_LL11D: goto _LL11F;
_LL11E: {struct Cyc_Absyn_Const_e_struct*_tmp1AD=(struct Cyc_Absyn_Const_e_struct*)
_tmp194;if(_tmp1AD->tag != 0)goto _LL120;}_LL11F: goto _LL121;_LL120: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp1AE=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp194;if(_tmp1AE->tag != 19)goto
_LL122;}_LL121: goto _LL123;_LL122: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp1AF=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp194;if(_tmp1AF->tag != 18)goto _LL124;}
_LL123: goto _LL125;_LL124: {struct Cyc_Absyn_Offsetof_e_struct*_tmp1B0=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp194;if(_tmp1B0->tag != 20)goto _LL126;}_LL125: goto _LL127;_LL126: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp1B1=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp194;if(_tmp1B1->tag != 33)goto
_LL128;}_LL127: goto _LL129;_LL128: {struct Cyc_Absyn_Enum_e_struct*_tmp1B2=(struct
Cyc_Absyn_Enum_e_struct*)_tmp194;if(_tmp1B2->tag != 32)goto _LL12A;}_LL129: {
struct _tuple13 _tmp7D7;return(_tmp7D7.f1=inflow,((_tmp7D7.f2=_tmp172->unknown_all,
_tmp7D7)));}_LL12A: {struct Cyc_Absyn_Var_e_struct*_tmp1B3=(struct Cyc_Absyn_Var_e_struct*)
_tmp194;if(_tmp1B3->tag != 1)goto _LL12C;else{_tmp1B4=(void*)_tmp1B3->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp1B5=(struct Cyc_Absyn_Global_b_struct*)_tmp1B4;if(
_tmp1B5->tag != 1)goto _LL12C;};}}_LL12B: {struct _tuple13 _tmp7D8;return(_tmp7D8.f1=
inflow,((_tmp7D8.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0,_tmp172->unknown_all),_tmp7D8)));}_LL12C: {struct Cyc_Absyn_Var_e_struct*
_tmp1B6=(struct Cyc_Absyn_Var_e_struct*)_tmp194;if(_tmp1B6->tag != 1)goto _LL12E;
else{_tmp1B7=(void*)_tmp1B6->f2;{struct Cyc_Absyn_Param_b_struct*_tmp1B8=(struct
Cyc_Absyn_Param_b_struct*)_tmp1B7;if(_tmp1B8->tag != 3)goto _LL12E;else{_tmp1B9=
_tmp1B8->f1;}};}}_LL12D: _tmp1BD=_tmp1B9;goto _LL12F;_LL12E: {struct Cyc_Absyn_Var_e_struct*
_tmp1BA=(struct Cyc_Absyn_Var_e_struct*)_tmp194;if(_tmp1BA->tag != 1)goto _LL130;
else{_tmp1BB=(void*)_tmp1BA->f2;{struct Cyc_Absyn_Local_b_struct*_tmp1BC=(struct
Cyc_Absyn_Local_b_struct*)_tmp1BB;if(_tmp1BC->tag != 4)goto _LL130;else{_tmp1BD=
_tmp1BC->f1;}};}}_LL12F: _tmp1C1=_tmp1BD;goto _LL131;_LL130: {struct Cyc_Absyn_Var_e_struct*
_tmp1BE=(struct Cyc_Absyn_Var_e_struct*)_tmp194;if(_tmp1BE->tag != 1)goto _LL132;
else{_tmp1BF=(void*)_tmp1BE->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp1C0=(struct Cyc_Absyn_Pat_b_struct*)
_tmp1BF;if(_tmp1C0->tag != 5)goto _LL132;else{_tmp1C1=_tmp1C0->f1;}};}}_LL131: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp7DE;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp7DD;struct _tuple13 _tmp7DC;return(_tmp7DC.f1=inflow,((_tmp7DC.f2=((void*(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,(void*)((_tmp7DE=_region_malloc(
env->r,sizeof(*_tmp7DE)),((_tmp7DE[0]=((_tmp7DD.tag=0,((_tmp7DD.f1=_tmp1C1,
_tmp7DD)))),_tmp7DE))))),_tmp7DC)));}_LL132: {struct Cyc_Absyn_Primop_e_struct*
_tmp1C2=(struct Cyc_Absyn_Primop_e_struct*)_tmp194;if(_tmp1C2->tag != 3)goto _LL134;
else{_tmp1C3=_tmp1C2->f1;_tmp1C4=_tmp1C2->f2;}}_LL133: {union Cyc_CfFlowInfo_FlowInfo
_tmp230;void*_tmp231;struct _tuple13 _tmp22F=Cyc_NewControlFlow_anal_use_ints(env,
inflow,_tmp1C4);_tmp230=_tmp22F.f1;_tmp231=_tmp22F.f2;{struct _tuple13 _tmp7DF;
return(_tmp7DF.f1=_tmp230,((_tmp7DF.f2=_tmp231,_tmp7DF)));};}_LL134: {struct Cyc_Absyn_Increment_e_struct*
_tmp1C5=(struct Cyc_Absyn_Increment_e_struct*)_tmp194;if(_tmp1C5->tag != 5)goto
_LL136;else{_tmp1C6=_tmp1C5->f1;}}_LL135: {struct Cyc_List_List _tmp7E0;struct Cyc_List_List
_tmp233=(_tmp7E0.hd=_tmp1C6,((_tmp7E0.tl=0,_tmp7E0)));union Cyc_CfFlowInfo_FlowInfo
_tmp235;struct _tuple13 _tmp234=Cyc_NewControlFlow_anal_use_ints(env,inflow,(
struct Cyc_List_List*)& _tmp233);_tmp235=_tmp234.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp237;struct _tuple14 _tmp236=Cyc_NewControlFlow_anal_Lexp(env,_tmp235,0,_tmp1C6);
_tmp237=_tmp236.f2;{struct _tuple20 _tmp7E1;struct _tuple20 _tmp239=(_tmp7E1.f1=
_tmp237,((_tmp7E1.f2=_tmp235,_tmp7E1)));union Cyc_CfFlowInfo_AbsLVal _tmp23A;
struct Cyc_CfFlowInfo_Place*_tmp23B;union Cyc_CfFlowInfo_FlowInfo _tmp23C;struct
_tuple12 _tmp23D;struct Cyc_Dict_Dict _tmp23E;struct Cyc_List_List*_tmp23F;_LL177:
_tmp23A=_tmp239.f1;if((_tmp23A.PlaceL).tag != 1)goto _LL179;_tmp23B=(struct Cyc_CfFlowInfo_Place*)(
_tmp23A.PlaceL).val;_tmp23C=_tmp239.f2;if((_tmp23C.ReachableFL).tag != 2)goto
_LL179;_tmp23D=(struct _tuple12)(_tmp23C.ReachableFL).val;_tmp23E=_tmp23D.f1;
_tmp23F=_tmp23D.f2;_LL178: _tmp23F=Cyc_CfFlowInfo_reln_kill_exp(_tmp172->r,
_tmp23F,_tmp1C6);_tmp235=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
_tmp172,_tmp1C6->loc,_tmp23E,env->all_changed,_tmp23B,_tmp172->unknown_all),
_tmp23F);goto _LL176;_LL179:;_LL17A: goto _LL176;_LL176:;}{struct _tuple13 _tmp7E2;
return(_tmp7E2.f1=_tmp235,((_tmp7E2.f2=_tmp172->unknown_all,_tmp7E2)));};};}
_LL136: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1C7=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp194;if(_tmp1C7->tag != 4)goto _LL138;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C7->f2;
if(_tmp1C9 == 0)goto _LL138;_tmp1CA=*_tmp1C9;_tmp1CB=_tmp1C7->f3;}}_LL137: if(
copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){{const char*_tmp7E5;void*_tmp7E4;(_tmp7E4=0,Cyc_Tcutil_terr(
e->loc,((_tmp7E5="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7E5,sizeof(char),58))),_tag_dyneither(_tmp7E4,sizeof(void*),0)));}{
struct _tuple13 _tmp7E6;return(_tmp7E6.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7E6.f2=
_tmp172->unknown_all,_tmp7E6)));};}{struct Cyc_List_List _tmp7E7;struct Cyc_List_List
_tmp245=(_tmp7E7.hd=_tmp1CB,((_tmp7E7.tl=0,_tmp7E7)));struct Cyc_List_List _tmp7E8;
struct Cyc_List_List _tmp246=(_tmp7E8.hd=_tmp1C8,((_tmp7E8.tl=(struct Cyc_List_List*)&
_tmp245,_tmp7E8)));union Cyc_CfFlowInfo_FlowInfo _tmp248;struct _tuple13 _tmp247=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp246);_tmp248=_tmp247.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp24A;struct _tuple14 _tmp249=Cyc_NewControlFlow_anal_Lexp(env,_tmp248,0,_tmp1C8);
_tmp24A=_tmp249.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp24B=_tmp248;struct _tuple12
_tmp24C;struct Cyc_Dict_Dict _tmp24D;struct Cyc_List_List*_tmp24E;_LL17C: if((
_tmp24B.ReachableFL).tag != 2)goto _LL17E;_tmp24C=(struct _tuple12)(_tmp24B.ReachableFL).val;
_tmp24D=_tmp24C.f1;_tmp24E=_tmp24C.f2;_LL17D:{union Cyc_CfFlowInfo_AbsLVal _tmp24F=
_tmp24A;struct Cyc_CfFlowInfo_Place*_tmp250;int _tmp251;_LL181: if((_tmp24F.PlaceL).tag
!= 1)goto _LL183;_tmp250=(struct Cyc_CfFlowInfo_Place*)(_tmp24F.PlaceL).val;_LL182:
_tmp24E=Cyc_CfFlowInfo_reln_kill_exp(_tmp172->r,_tmp24E,_tmp1C8);_tmp24D=Cyc_CfFlowInfo_assign_place(
_tmp172,_tmp1C8->loc,_tmp24D,env->all_changed,_tmp250,_tmp172->unknown_all);
_tmp248=Cyc_CfFlowInfo_ReachableFL(_tmp24D,_tmp24E);goto _LL180;_LL183: if((
_tmp24F.UnknownL).tag != 2)goto _LL180;_tmp251=(int)(_tmp24F.UnknownL).val;_LL184:
goto _LL180;_LL180:;}goto _LL17B;_LL17E:;_LL17F: goto _LL17B;_LL17B:;}{struct
_tuple13 _tmp7E9;return(_tmp7E9.f1=_tmp248,((_tmp7E9.f2=_tmp172->unknown_all,
_tmp7E9)));};};};_LL138: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1CC=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp194;if(_tmp1CC->tag != 4)goto _LL13A;else{_tmp1CD=_tmp1CC->f1;_tmp1CE=_tmp1CC->f2;
if(_tmp1CE != 0)goto _LL13A;_tmp1CF=_tmp1CC->f3;}}_LL139: if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){{const char*
_tmp7EC;void*_tmp7EB;(_tmp7EB=0,Cyc_Tcutil_terr(e->loc,((_tmp7EC="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7EC,sizeof(char),58))),_tag_dyneither(_tmp7EB,sizeof(void*),0)));}{
struct _tuple13 _tmp7ED;return(_tmp7ED.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7ED.f2=
_tmp172->unknown_all,_tmp7ED)));};}{struct Cyc_Dict_Dict*_tmp258=env->all_changed;
while(1){{struct Cyc_Dict_Dict*_tmp7EE;env->all_changed=((_tmp7EE=_region_malloc(
env->r,sizeof(*_tmp7EE)),((_tmp7EE[0]=_tmp172->mt_place_set,_tmp7EE))));}{union
Cyc_CfFlowInfo_FlowInfo _tmp25B;union Cyc_CfFlowInfo_AbsLVal _tmp25C;struct _tuple14
_tmp25A=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1CD);_tmp25B=_tmp25A.f1;
_tmp25C=_tmp25A.f2;{struct Cyc_Dict_Dict _tmp25D=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp7EF;env->all_changed=((
_tmp7EF=_region_malloc(env->r,sizeof(*_tmp7EF)),((_tmp7EF[0]=_tmp172->mt_place_set,
_tmp7EF))));}{union Cyc_CfFlowInfo_FlowInfo _tmp260;void*_tmp261;struct _tuple13
_tmp25F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1CF);_tmp260=_tmp25F.f1;
_tmp261=_tmp25F.f2;{struct Cyc_Dict_Dict _tmp262=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo _tmp263=Cyc_CfFlowInfo_after_flow(
_tmp172,(struct Cyc_Dict_Dict*)& _tmp25D,_tmp25B,_tmp260,_tmp25D,_tmp262);union Cyc_CfFlowInfo_FlowInfo
_tmp264=Cyc_CfFlowInfo_join_flow(_tmp172,_tmp258,inflow,_tmp263);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp264,inflow)){if(_tmp258 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp258,Cyc_CfFlowInfo_union_place_set(
_tmp25D,_tmp262,0),0);}return Cyc_NewControlFlow_do_assign(_tmp172,env,_tmp263,
_tmp1CD,_tmp25C,_tmp1CF,_tmp261,e->loc);}inflow=_tmp264;};};};};}};_LL13A: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp1D0=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp194;if(_tmp1D0->tag != 9)goto _LL13C;else{_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D0->f2;}}
_LL13B: {union Cyc_CfFlowInfo_FlowInfo _tmp266;void*_tmp267;struct _tuple13 _tmp265=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1D1);_tmp266=_tmp265.f1;_tmp267=
_tmp265.f2;return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp266,_tmp1D2);}
_LL13C: {struct Cyc_Absyn_Throw_e_struct*_tmp1D3=(struct Cyc_Absyn_Throw_e_struct*)
_tmp194;if(_tmp1D3->tag != 12)goto _LL13E;else{_tmp1D4=_tmp1D3->f1;}}_LL13D: {
union Cyc_CfFlowInfo_FlowInfo _tmp269;void*_tmp26A;struct _tuple13 _tmp268=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1D4);_tmp269=_tmp268.f1;_tmp26A=_tmp268.f2;Cyc_NewControlFlow_use_Rval(
env,_tmp1D4->loc,_tmp269,_tmp26A);{struct _tuple13 _tmp7F0;return(_tmp7F0.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp7F0.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0,_tmp172->unknown_all),_tmp7F0)));};}_LL13E: {struct
Cyc_Absyn_FnCall_e_struct*_tmp1D5=(struct Cyc_Absyn_FnCall_e_struct*)_tmp194;if(
_tmp1D5->tag != 11)goto _LL140;else{_tmp1D6=_tmp1D5->f1;_tmp1D7=_tmp1D5->f2;}}
_LL13F: {struct _RegionHandle*_tmp26C=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp26F;
struct Cyc_List_List*_tmp270;struct Cyc_List_List*_tmp7F1;struct _tuple19 _tmp26E=
Cyc_NewControlFlow_anal_unordered_Rexps(_tmp26C,env,inflow,((_tmp7F1=
_region_malloc(_tmp26C,sizeof(*_tmp7F1)),((_tmp7F1->hd=_tmp1D6,((_tmp7F1->tl=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(
_tmp26C,_tmp1D7),_tmp7F1)))))),0,1);_tmp26F=_tmp26E.f1;_tmp270=_tmp26E.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp271=Cyc_NewControlFlow_use_Rval(env,_tmp1D6->loc,
_tmp26F,(void*)((struct Cyc_List_List*)_check_null(_tmp270))->hd);_tmp270=_tmp270->tl;{
struct Cyc_List_List*init_params=0;struct Cyc_List_List*nolive_unique_params=0;{
void*_tmp272=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1D6->topt))->v);struct Cyc_Absyn_PtrInfo _tmp274;void*_tmp275;_LL186: {struct
Cyc_Absyn_PointerType_struct*_tmp273=(struct Cyc_Absyn_PointerType_struct*)
_tmp272;if(_tmp273->tag != 5)goto _LL188;else{_tmp274=_tmp273->f1;_tmp275=_tmp274.elt_typ;}}
_LL187:{void*_tmp276=Cyc_Tcutil_compress(_tmp275);struct Cyc_Absyn_FnInfo _tmp278;
struct Cyc_List_List*_tmp279;_LL18B: {struct Cyc_Absyn_FnType_struct*_tmp277=(
struct Cyc_Absyn_FnType_struct*)_tmp276;if(_tmp277->tag != 10)goto _LL18D;else{
_tmp278=_tmp277->f1;_tmp279=_tmp278.attributes;}}_LL18C: for(0;_tmp279 != 0;
_tmp279=_tmp279->tl){void*_tmp27A=(void*)_tmp279->hd;int _tmp27C;int _tmp27E;
_LL190: {struct Cyc_Absyn_Initializes_att_struct*_tmp27B=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp27A;if(_tmp27B->tag != 20)goto _LL192;else{_tmp27C=_tmp27B->f1;}}_LL191:{
struct Cyc_List_List*_tmp7F2;init_params=((_tmp7F2=_region_malloc(_tmp26C,sizeof(*
_tmp7F2)),((_tmp7F2->hd=(void*)_tmp27C,((_tmp7F2->tl=init_params,_tmp7F2))))));}
goto _LL18F;_LL192: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp27D=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp27A;if(_tmp27D->tag != 21)goto _LL194;else{_tmp27E=_tmp27D->f1;}}_LL193:{
struct Cyc_List_List*_tmp7F3;nolive_unique_params=((_tmp7F3=_region_malloc(
_tmp26C,sizeof(*_tmp7F3)),((_tmp7F3->hd=(void*)_tmp27E,((_tmp7F3->tl=
nolive_unique_params,_tmp7F3))))));}goto _LL18F;_LL194:;_LL195: goto _LL18F;_LL18F:;}
goto _LL18A;_LL18D:;_LL18E: {const char*_tmp7F6;void*_tmp7F5;(_tmp7F5=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7F6="anal_Rexp: bad function type",
_tag_dyneither(_tmp7F6,sizeof(char),29))),_tag_dyneither(_tmp7F5,sizeof(void*),0)));}
_LL18A:;}goto _LL185;_LL188:;_LL189: {const char*_tmp7F9;void*_tmp7F8;(_tmp7F8=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp7F9="anal_Rexp: bad function type",_tag_dyneither(_tmp7F9,sizeof(char),29))),
_tag_dyneither(_tmp7F8,sizeof(void*),0)));}_LL185:;}{int i=1;for(0;_tmp270 != 0;(((
_tmp270=_tmp270->tl,_tmp1D7=((struct Cyc_List_List*)_check_null(_tmp1D7))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i) && 
!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){
_tmp271=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D7))->hd)->loc,_tmp271,(void*)_tmp270->hd);continue;}if(((int(*)(
struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){_tmp271=Cyc_NewControlFlow_use_nounique_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1D7))->hd)->loc,(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D7))->hd)->topt))->v,_tmp271,(void*)_tmp270->hd);continue;}{
union Cyc_CfFlowInfo_FlowInfo _tmp285=_tmp26F;int _tmp286;struct _tuple12 _tmp287;
struct Cyc_Dict_Dict _tmp288;_LL197: if((_tmp285.BottomFL).tag != 1)goto _LL199;
_tmp286=(int)(_tmp285.BottomFL).val;_LL198: goto _LL196;_LL199: if((_tmp285.ReachableFL).tag
!= 2)goto _LL196;_tmp287=(struct _tuple12)(_tmp285.ReachableFL).val;_tmp288=
_tmp287.f1;_LL19A: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp288,(void*)_tmp270->hd)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp7FC;void*_tmp7FB;(_tmp7FB=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1D7))->hd)->loc,((
_tmp7FC="expression may not be initialized",_tag_dyneither(_tmp7FC,sizeof(char),
34))),_tag_dyneither(_tmp7FB,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo
_tmp28B=_tmp271;int _tmp28C;struct _tuple12 _tmp28D;struct Cyc_Dict_Dict _tmp28E;
struct Cyc_List_List*_tmp28F;_LL19C: if((_tmp28B.BottomFL).tag != 1)goto _LL19E;
_tmp28C=(int)(_tmp28B.BottomFL).val;_LL19D: goto _LL19B;_LL19E: if((_tmp28B.ReachableFL).tag
!= 2)goto _LL19B;_tmp28D=(struct _tuple12)(_tmp28B.ReachableFL).val;_tmp28E=
_tmp28D.f1;_tmp28F=_tmp28D.f2;_LL19F: {struct Cyc_Dict_Dict _tmp290=Cyc_CfFlowInfo_escape_deref(
_tmp172,_tmp28E,env->all_changed,(void*)_tmp270->hd);{void*_tmp291=(void*)
_tmp270->hd;struct Cyc_CfFlowInfo_Place*_tmp293;_LL1A1: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp292=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp291;if(_tmp292->tag != 5)goto
_LL1A3;else{_tmp293=_tmp292->f1;}}_LL1A2:{void*_tmp294=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D7))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp296;void*
_tmp297;_LL1A6: {struct Cyc_Absyn_PointerType_struct*_tmp295=(struct Cyc_Absyn_PointerType_struct*)
_tmp294;if(_tmp295->tag != 5)goto _LL1A8;else{_tmp296=_tmp295->f1;_tmp297=_tmp296.elt_typ;}}
_LL1A7: _tmp290=Cyc_CfFlowInfo_assign_place(_tmp172,((struct Cyc_Absyn_Exp*)
_tmp1D7->hd)->loc,_tmp290,env->all_changed,_tmp293,Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,_tmp297,0,_tmp172->esc_all));goto _LL1A5;_LL1A8:;_LL1A9: {const char*
_tmp7FF;void*_tmp7FE;(_tmp7FE=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7FF="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp7FF,sizeof(char),39))),_tag_dyneither(_tmp7FE,sizeof(void*),0)));}
_LL1A5:;}goto _LL1A0;_LL1A3:;_LL1A4: goto _LL1A0;_LL1A0:;}_tmp271=Cyc_CfFlowInfo_ReachableFL(
_tmp290,_tmp28F);goto _LL19B;}_LL19B:;}goto _LL196;_LL196:;};}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1D6->topt))->v)){struct _tuple13
_tmp800;return(_tmp800.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp800.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0,_tmp172->unknown_all),
_tmp800)));}else{struct _tuple13 _tmp801;return(_tmp801.f1=_tmp271,((_tmp801.f2=
Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
0,_tmp172->unknown_all),_tmp801)));}};};}_LL140: {struct Cyc_Absyn_Malloc_e_struct*
_tmp1D8=(struct Cyc_Absyn_Malloc_e_struct*)_tmp194;if(_tmp1D8->tag != 34)goto
_LL142;else{_tmp1D9=_tmp1D8->f1;_tmp1DA=_tmp1D9.is_calloc;_tmp1DB=_tmp1D9.rgn;
_tmp1DC=_tmp1D9.elt_type;_tmp1DD=_tmp1D9.num_elts;_tmp1DE=_tmp1D9.fat_result;}}
_LL141: {struct Cyc_CfFlowInfo_MallocPt_struct _tmp804;struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp803;void*root=(void*)((_tmp803=_region_malloc(_tmp172->r,sizeof(*_tmp803)),((
_tmp803[0]=((_tmp804.tag=1,((_tmp804.f1=_tmp1DD,((_tmp804.f2=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp804)))))),_tmp803))));struct
Cyc_CfFlowInfo_Place*_tmp805;struct Cyc_CfFlowInfo_Place*place=(_tmp805=
_region_malloc(_tmp172->r,sizeof(*_tmp805)),((_tmp805->root=root,((_tmp805->fields=
0,_tmp805)))));struct Cyc_CfFlowInfo_AddressOf_struct _tmp808;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp807;void*rval=(void*)((_tmp807=_region_malloc(_tmp172->r,sizeof(*_tmp807)),((
_tmp807[0]=((_tmp808.tag=5,((_tmp808.f1=place,_tmp808)))),_tmp807))));void*
place_val;if(_tmp1DE)place_val=_tmp172->notzeroall;else{if(_tmp1DA)place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,*((void**)_check_null(_tmp1DC)),0,_tmp172->zero);else{place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,*((void**)_check_null(_tmp1DC)),0,_tmp172->unknown_none);}}{union Cyc_CfFlowInfo_FlowInfo
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);if(_tmp1DB != 0){struct _RegionHandle*_tmp29C=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp29F;struct Cyc_List_List*_tmp2A0;struct Cyc_Absyn_Exp*_tmp809[2];struct
_tuple19 _tmp29E=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp29C,env,inflow,((
_tmp809[1]=_tmp1DD,((_tmp809[0]=(struct Cyc_Absyn_Exp*)_tmp1DB,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp29C,
_tag_dyneither(_tmp809,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp29F=_tmp29E.f1;
_tmp2A0=_tmp29E.f2;outflow=_tmp29F;}else{union Cyc_CfFlowInfo_FlowInfo _tmp2A2;
struct _tuple13 _tmp2A1=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1DD);_tmp2A2=
_tmp2A1.f1;outflow=_tmp2A2;}{union Cyc_CfFlowInfo_FlowInfo _tmp2A3=outflow;int
_tmp2A4;struct _tuple12 _tmp2A5;struct Cyc_Dict_Dict _tmp2A6;struct Cyc_List_List*
_tmp2A7;_LL1AB: if((_tmp2A3.BottomFL).tag != 1)goto _LL1AD;_tmp2A4=(int)(_tmp2A3.BottomFL).val;
_LL1AC: {struct _tuple13 _tmp80A;return(_tmp80A.f1=outflow,((_tmp80A.f2=rval,
_tmp80A)));}_LL1AD: if((_tmp2A3.ReachableFL).tag != 2)goto _LL1AA;_tmp2A5=(struct
_tuple12)(_tmp2A3.ReachableFL).val;_tmp2A6=_tmp2A5.f1;_tmp2A7=_tmp2A5.f2;_LL1AE: {
struct _tuple13 _tmp80B;return(_tmp80B.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2A6,root,place_val),
_tmp2A7),((_tmp80B.f2=rval,_tmp80B)));}_LL1AA:;};};}_LL142: {struct Cyc_Absyn_Swap_e_struct*
_tmp1DF=(struct Cyc_Absyn_Swap_e_struct*)_tmp194;if(_tmp1DF->tag != 35)goto _LL144;
else{_tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}}_LL143: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle*_tmp2AF=env->r;{
union Cyc_CfFlowInfo_FlowInfo _tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_Absyn_Exp*
_tmp80C[2];struct _tuple19 _tmp2B1=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2AF,
env,inflow,((_tmp80C[1]=_tmp1E1,((_tmp80C[0]=_tmp1E0,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2AF,
_tag_dyneither(_tmp80C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp2B2=_tmp2B1.f1;
_tmp2B3=_tmp2B1.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2B3))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2B3->tl))->hd;outflow=
_tmp2B2;}{union Cyc_CfFlowInfo_AbsLVal _tmp2B5;struct _tuple14 _tmp2B4=Cyc_NewControlFlow_anal_Lexp(
env,outflow,0,_tmp1E0);_tmp2B5=_tmp2B4.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp2B7;
struct _tuple14 _tmp2B6=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1E1);
_tmp2B7=_tmp2B6.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2B8=outflow;struct _tuple12
_tmp2B9;struct Cyc_Dict_Dict _tmp2BA;struct Cyc_List_List*_tmp2BB;_LL1B0: if((
_tmp2B8.ReachableFL).tag != 2)goto _LL1B2;_tmp2B9=(struct _tuple12)(_tmp2B8.ReachableFL).val;
_tmp2BA=_tmp2B9.f1;_tmp2BB=_tmp2B9.f2;_LL1B1:{union Cyc_CfFlowInfo_AbsLVal _tmp2BC=
_tmp2B5;struct Cyc_CfFlowInfo_Place*_tmp2BD;int _tmp2BE;_LL1B5: if((_tmp2BC.PlaceL).tag
!= 1)goto _LL1B7;_tmp2BD=(struct Cyc_CfFlowInfo_Place*)(_tmp2BC.PlaceL).val;_LL1B6:
_tmp2BA=Cyc_CfFlowInfo_assign_place(_tmp172,_tmp1E0->loc,_tmp2BA,env->all_changed,
_tmp2BD,right_rval);goto _LL1B4;_LL1B7: if((_tmp2BC.UnknownL).tag != 2)goto _LL1B4;
_tmp2BE=(int)(_tmp2BC.UnknownL).val;_LL1B8: goto _LL1B4;_LL1B4:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp2BF=_tmp2B7;struct Cyc_CfFlowInfo_Place*_tmp2C0;int _tmp2C1;_LL1BA: if((_tmp2BF.PlaceL).tag
!= 1)goto _LL1BC;_tmp2C0=(struct Cyc_CfFlowInfo_Place*)(_tmp2BF.PlaceL).val;_LL1BB:
_tmp2BA=Cyc_CfFlowInfo_assign_place(_tmp172,_tmp1E1->loc,_tmp2BA,env->all_changed,
_tmp2C0,left_rval);goto _LL1B9;_LL1BC: if((_tmp2BF.UnknownL).tag != 2)goto _LL1B9;
_tmp2C1=(int)(_tmp2BF.UnknownL).val;_LL1BD: goto _LL1B9;_LL1B9:;}_tmp2BB=Cyc_CfFlowInfo_reln_kill_exp(
_tmp172->r,_tmp2BB,_tmp1E0);_tmp2BB=Cyc_CfFlowInfo_reln_kill_exp(_tmp172->r,
_tmp2BB,_tmp1E1);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2BA,_tmp2BB);goto _LL1AF;
_LL1B2:;_LL1B3: goto _LL1AF;_LL1AF:;}{struct _tuple13 _tmp80D;return(_tmp80D.f1=
outflow,((_tmp80D.f2=_tmp172->unknown_all,_tmp80D)));};};};}_LL144: {struct Cyc_Absyn_New_e_struct*
_tmp1E2=(struct Cyc_Absyn_New_e_struct*)_tmp194;if(_tmp1E2->tag != 17)goto _LL146;
else{_tmp1E3=_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;}}_LL145: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp810;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp80F;void*root=(void*)((_tmp80F=
_region_malloc(_tmp172->r,sizeof(*_tmp80F)),((_tmp80F[0]=((_tmp810.tag=1,((
_tmp810.f1=_tmp1E4,((_tmp810.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp810)))))),_tmp80F))));struct Cyc_CfFlowInfo_Place*_tmp811;
struct Cyc_CfFlowInfo_Place*place=(_tmp811=_region_malloc(_tmp172->r,sizeof(*
_tmp811)),((_tmp811->root=root,((_tmp811->fields=0,_tmp811)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp814;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp813;void*rval=(void*)((_tmp813=
_region_malloc(_tmp172->r,sizeof(*_tmp813)),((_tmp813[0]=((_tmp814.tag=5,((
_tmp814.f1=place,_tmp814)))),_tmp813))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;
if(_tmp1E3 != 0){struct _RegionHandle*_tmp2C3=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp2C6;struct Cyc_List_List*_tmp2C7;struct Cyc_Absyn_Exp*_tmp815[2];struct
_tuple19 _tmp2C5=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2C3,env,inflow,((
_tmp815[1]=_tmp1E4,((_tmp815[0]=(struct Cyc_Absyn_Exp*)_tmp1E3,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2C3,
_tag_dyneither(_tmp815,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp2C6=_tmp2C5.f1;
_tmp2C7=_tmp2C5.f2;outflow=_tmp2C6;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2C7))->tl))->hd;}else{union
Cyc_CfFlowInfo_FlowInfo _tmp2C9;void*_tmp2CA;struct _tuple13 _tmp2C8=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1E4);_tmp2C9=_tmp2C8.f1;_tmp2CA=_tmp2C8.f2;outflow=_tmp2C9;
place_val=_tmp2CA;}{union Cyc_CfFlowInfo_FlowInfo _tmp2CB=outflow;int _tmp2CC;
struct _tuple12 _tmp2CD;struct Cyc_Dict_Dict _tmp2CE;struct Cyc_List_List*_tmp2CF;
_LL1BF: if((_tmp2CB.BottomFL).tag != 1)goto _LL1C1;_tmp2CC=(int)(_tmp2CB.BottomFL).val;
_LL1C0: {struct _tuple13 _tmp816;return(_tmp816.f1=outflow,((_tmp816.f2=rval,
_tmp816)));}_LL1C1: if((_tmp2CB.ReachableFL).tag != 2)goto _LL1BE;_tmp2CD=(struct
_tuple12)(_tmp2CB.ReachableFL).val;_tmp2CE=_tmp2CD.f1;_tmp2CF=_tmp2CD.f2;_LL1C2: {
struct _tuple13 _tmp817;return(_tmp817.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2CE,root,place_val),
_tmp2CF),((_tmp817.f2=rval,_tmp817)));}_LL1BE:;};};}_LL146: {struct Cyc_Absyn_Address_e_struct*
_tmp1E5=(struct Cyc_Absyn_Address_e_struct*)_tmp194;if(_tmp1E5->tag != 16)goto
_LL148;else{_tmp1E6=_tmp1E5->f1;}}_LL147: {union Cyc_CfFlowInfo_FlowInfo _tmp2D8;
union Cyc_CfFlowInfo_AbsLVal _tmp2D9;struct _tuple14 _tmp2D7=Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,_tmp1E6);_tmp2D8=_tmp2D7.f1;_tmp2D9=_tmp2D7.f2;{union Cyc_CfFlowInfo_AbsLVal
_tmp2DA=_tmp2D9;int _tmp2DB;struct Cyc_CfFlowInfo_Place*_tmp2DC;_LL1C4: if((_tmp2DA.UnknownL).tag
!= 2)goto _LL1C6;_tmp2DB=(int)(_tmp2DA.UnknownL).val;_LL1C5: {struct _tuple13
_tmp818;return(_tmp818.f1=_tmp2D8,((_tmp818.f2=_tmp172->notzeroall,_tmp818)));}
_LL1C6: if((_tmp2DA.PlaceL).tag != 1)goto _LL1C3;_tmp2DC=(struct Cyc_CfFlowInfo_Place*)(
_tmp2DA.PlaceL).val;_LL1C7: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp81E;
struct Cyc_CfFlowInfo_AddressOf_struct _tmp81D;struct _tuple13 _tmp81C;return(
_tmp81C.f1=_tmp2D8,((_tmp81C.f2=(void*)((_tmp81E=_region_malloc(env->r,sizeof(*
_tmp81E)),((_tmp81E[0]=((_tmp81D.tag=5,((_tmp81D.f1=_tmp2DC,_tmp81D)))),_tmp81E)))),
_tmp81C)));}_LL1C3:;};}_LL148: {struct Cyc_Absyn_Deref_e_struct*_tmp1E7=(struct
Cyc_Absyn_Deref_e_struct*)_tmp194;if(_tmp1E7->tag != 21)goto _LL14A;else{_tmp1E8=
_tmp1E7->f1;}}_LL149: {union Cyc_CfFlowInfo_FlowInfo _tmp2E2;void*_tmp2E3;struct
_tuple13 _tmp2E1=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1E8);_tmp2E2=
_tmp2E1.f1;_tmp2E3=_tmp2E1.f2;return Cyc_NewControlFlow_anal_derefR(env,inflow,
_tmp2E2,_tmp1E8,_tmp2E3);}_LL14A: {struct Cyc_Absyn_AggrMember_e_struct*_tmp1E9=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp194;if(_tmp1E9->tag != 22)goto _LL14C;
else{_tmp1EA=_tmp1E9->f1;_tmp1EB=_tmp1E9->f2;}}_LL14B: {union Cyc_CfFlowInfo_FlowInfo
_tmp2E5;void*_tmp2E6;struct _tuple13 _tmp2E4=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1EA);_tmp2E5=_tmp2E4.f1;_tmp2E6=_tmp2E4.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1EA->topt))->v)){struct _tuple13
_tmp81F;return(_tmp81F.f1=_tmp2E5,((_tmp81F.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0,_tmp172->unknown_all),
_tmp81F)));}{void*_tmp2E8=_tmp2E6;struct Cyc_CfFlowInfo_UnionRInfo _tmp2EA;int
_tmp2EB;int _tmp2EC;struct _dyneither_ptr _tmp2ED;_LL1C9: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2E9=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2E8;if(_tmp2E9->tag != 6)goto
_LL1CB;else{_tmp2EA=_tmp2E9->f1;_tmp2EB=_tmp2EA.is_union;_tmp2EC=_tmp2EA.fieldnum;
_tmp2ED=_tmp2E9->f2;}}_LL1CA: {int _tmp2EE=Cyc_CfFlowInfo_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1EA->topt))->v,_tmp1EB);if((_tmp2EB  && 
_tmp2EC != - 1) && _tmp2EC != _tmp2EE){struct _tuple13 _tmp820;return(_tmp820.f1=
_tmp2E5,((_tmp820.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,1,_tmp172->unknown_none),_tmp820)));}{struct _tuple13
_tmp821;return(_tmp821.f1=_tmp2E5,((_tmp821.f2=*((void**)
_check_dyneither_subscript(_tmp2ED,sizeof(void*),_tmp2EE)),_tmp821)));};}_LL1CB:;
_LL1CC: {struct Cyc_Core_Impossible_struct _tmp82E;const char*_tmp82D;void*_tmp82C[
1];struct Cyc_String_pa_struct _tmp82B;struct Cyc_Core_Impossible_struct*_tmp82A;(
int)_throw((void*)((_tmp82A=_cycalloc(sizeof(*_tmp82A)),((_tmp82A[0]=((_tmp82E.tag=
Cyc_Core_Impossible,((_tmp82E.f1=(struct _dyneither_ptr)((_tmp82B.tag=0,((_tmp82B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp82C[
0]=& _tmp82B,Cyc_aprintf(((_tmp82D="anal_Rexp: AggrMember: %s",_tag_dyneither(
_tmp82D,sizeof(char),26))),_tag_dyneither(_tmp82C,sizeof(void*),1)))))))),
_tmp82E)))),_tmp82A)))));}_LL1C8:;};}_LL14C: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1EC=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp194;if(_tmp1EC->tag != 38)goto
_LL14E;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=_tmp1EC->f2;}}_LL14D: {union Cyc_CfFlowInfo_FlowInfo
_tmp2F7;void*_tmp2F8;struct _tuple13 _tmp2F6=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1ED);_tmp2F7=_tmp2F6.f1;_tmp2F8=_tmp2F6.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1ED->topt))->v)){struct _tuple13
_tmp82F;return(_tmp82F.f1=_tmp2F7,((_tmp82F.f2=_tmp172->unknown_all,_tmp82F)));}{
void*_tmp2FA=_tmp2F8;struct Cyc_CfFlowInfo_UnionRInfo _tmp2FC;int _tmp2FD;int
_tmp2FE;struct _dyneither_ptr _tmp2FF;_LL1CE: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2FB=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2FA;if(_tmp2FB->tag != 6)goto
_LL1D0;else{_tmp2FC=_tmp2FB->f1;_tmp2FD=_tmp2FC.is_union;_tmp2FE=_tmp2FC.fieldnum;
_tmp2FF=_tmp2FB->f2;}}_LL1CF: {int _tmp300=Cyc_CfFlowInfo_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1ED->topt))->v,_tmp1EE);if(_tmp2FD  && 
_tmp2FE != - 1){if(_tmp2FE != _tmp300){struct _tuple13 _tmp830;return(_tmp830.f1=
_tmp2F7,((_tmp830.f2=_tmp172->zero,_tmp830)));}else{struct _tuple13 _tmp831;return(
_tmp831.f1=_tmp2F7,((_tmp831.f2=_tmp172->notzeroall,_tmp831)));}}else{struct
_tuple13 _tmp832;return(_tmp832.f1=_tmp2F7,((_tmp832.f2=_tmp172->unknown_all,
_tmp832)));}}_LL1D0:;_LL1D1: {struct Cyc_Core_Impossible_struct _tmp83F;const char*
_tmp83E;void*_tmp83D[1];struct Cyc_String_pa_struct _tmp83C;struct Cyc_Core_Impossible_struct*
_tmp83B;(int)_throw((void*)((_tmp83B=_cycalloc(sizeof(*_tmp83B)),((_tmp83B[0]=((
_tmp83F.tag=Cyc_Core_Impossible,((_tmp83F.f1=(struct _dyneither_ptr)((_tmp83C.tag=
0,((_tmp83C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp83D[0]=& _tmp83C,Cyc_aprintf(((_tmp83E="anal_Rexp: TagCheck_e: %s",
_tag_dyneither(_tmp83E,sizeof(char),26))),_tag_dyneither(_tmp83D,sizeof(void*),1)))))))),
_tmp83F)))),_tmp83B)))));}_LL1CD:;};}_LL14E: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp1EF=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp194;if(_tmp1EF->tag != 23)goto
_LL150;else{_tmp1F0=_tmp1EF->f1;_tmp1F1=_tmp1EF->f2;}}_LL14F: {union Cyc_CfFlowInfo_FlowInfo
_tmp30A;void*_tmp30B;struct _tuple13 _tmp309=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1F0);_tmp30A=_tmp309.f1;_tmp30B=_tmp309.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp30D;void*_tmp30E;struct _tuple13 _tmp30C=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp30A,_tmp1F0,_tmp30B);_tmp30D=_tmp30C.f1;_tmp30E=_tmp30C.f2;{void*
_tmp30F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1F0->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp311;void*_tmp312;_LL1D3: {struct Cyc_Absyn_PointerType_struct*
_tmp310=(struct Cyc_Absyn_PointerType_struct*)_tmp30F;if(_tmp310->tag != 5)goto
_LL1D5;else{_tmp311=_tmp310->f1;_tmp312=_tmp311.elt_typ;}}_LL1D4: if(Cyc_Absyn_is_nontagged_union_type(
_tmp312)){struct _tuple13 _tmp840;return(_tmp840.f1=_tmp30D,((_tmp840.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0,_tmp172->unknown_all),
_tmp840)));}{void*_tmp314=_tmp30E;struct Cyc_CfFlowInfo_UnionRInfo _tmp316;int
_tmp317;int _tmp318;struct _dyneither_ptr _tmp319;_LL1D8: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp315=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp314;if(_tmp315->tag != 6)goto
_LL1DA;else{_tmp316=_tmp315->f1;_tmp317=_tmp316.is_union;_tmp318=_tmp316.fieldnum;
_tmp319=_tmp315->f2;}}_LL1D9: {int _tmp31A=Cyc_CfFlowInfo_get_field_index(_tmp312,
_tmp1F1);if((_tmp317  && _tmp318 != - 1) && _tmp318 != _tmp31A){struct _tuple13
_tmp841;return(_tmp841.f1=_tmp30D,((_tmp841.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,1,_tmp172->unknown_none),
_tmp841)));}{struct _tuple13 _tmp842;return(_tmp842.f1=_tmp30D,((_tmp842.f2=*((
void**)_check_dyneither_subscript(_tmp319,sizeof(void*),_tmp31A)),_tmp842)));};}
_LL1DA:;_LL1DB: {struct Cyc_Core_Impossible_struct _tmp848;const char*_tmp847;
struct Cyc_Core_Impossible_struct*_tmp846;(int)_throw((void*)((_tmp846=_cycalloc(
sizeof(*_tmp846)),((_tmp846[0]=((_tmp848.tag=Cyc_Core_Impossible,((_tmp848.f1=((
_tmp847="anal_Rexp: AggrArrow",_tag_dyneither(_tmp847,sizeof(char),21))),_tmp848)))),
_tmp846)))));}_LL1D7:;};_LL1D5:;_LL1D6: {struct Cyc_Core_Impossible_struct _tmp84E;
const char*_tmp84D;struct Cyc_Core_Impossible_struct*_tmp84C;(int)_throw((void*)((
_tmp84C=_cycalloc(sizeof(*_tmp84C)),((_tmp84C[0]=((_tmp84E.tag=Cyc_Core_Impossible,((
_tmp84E.f1=((_tmp84D="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp84D,sizeof(
char),25))),_tmp84E)))),_tmp84C)))));}_LL1D2:;};};}_LL150: {struct Cyc_Absyn_Conditional_e_struct*
_tmp1F2=(struct Cyc_Absyn_Conditional_e_struct*)_tmp194;if(_tmp1F2->tag != 6)goto
_LL152;else{_tmp1F3=_tmp1F2->f1;_tmp1F4=_tmp1F2->f2;_tmp1F5=_tmp1F2->f3;}}_LL151: {
union Cyc_CfFlowInfo_FlowInfo _tmp324;union Cyc_CfFlowInfo_FlowInfo _tmp325;struct
_tuple15 _tmp323=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1F3);_tmp324=_tmp323.f1;
_tmp325=_tmp323.f2;{struct _tuple13 _tmp326=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp324,_tmp1F4);struct _tuple13 _tmp327=Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,_tmp325,_tmp1F5);return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,
env->all_changed,_tmp326,_tmp327);};}_LL152: {struct Cyc_Absyn_And_e_struct*
_tmp1F6=(struct Cyc_Absyn_And_e_struct*)_tmp194;if(_tmp1F6->tag != 7)goto _LL154;
else{_tmp1F7=_tmp1F6->f1;_tmp1F8=_tmp1F6->f2;}}_LL153: {union Cyc_CfFlowInfo_FlowInfo
_tmp329;union Cyc_CfFlowInfo_FlowInfo _tmp32A;struct _tuple15 _tmp328=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1F7);_tmp329=_tmp328.f1;_tmp32A=_tmp328.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp32C;void*_tmp32D;struct _tuple13 _tmp32B=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp329,_tmp1F8);_tmp32C=_tmp32B.f1;_tmp32D=_tmp32B.f2;{struct _tuple13
_tmp84F;struct _tuple13 _tmp32E=(_tmp84F.f1=_tmp32C,((_tmp84F.f2=_tmp32D,_tmp84F)));
struct _tuple13 _tmp850;struct _tuple13 _tmp32F=(_tmp850.f1=_tmp32A,((_tmp850.f2=
_tmp172->zero,_tmp850)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,env->all_changed,
_tmp32E,_tmp32F);};};}_LL154: {struct Cyc_Absyn_Or_e_struct*_tmp1F9=(struct Cyc_Absyn_Or_e_struct*)
_tmp194;if(_tmp1F9->tag != 8)goto _LL156;else{_tmp1FA=_tmp1F9->f1;_tmp1FB=_tmp1F9->f2;}}
_LL155: {union Cyc_CfFlowInfo_FlowInfo _tmp333;union Cyc_CfFlowInfo_FlowInfo _tmp334;
struct _tuple15 _tmp332=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1FA);_tmp333=
_tmp332.f1;_tmp334=_tmp332.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp336;void*_tmp337;
struct _tuple13 _tmp335=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp334,_tmp1FB);
_tmp336=_tmp335.f1;_tmp337=_tmp335.f2;{struct _tuple13 _tmp851;struct _tuple13
_tmp338=(_tmp851.f1=_tmp336,((_tmp851.f2=_tmp337,_tmp851)));struct _tuple13
_tmp852;struct _tuple13 _tmp339=(_tmp852.f1=_tmp333,((_tmp852.f2=_tmp172->notzeroall,
_tmp852)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,env->all_changed,
_tmp338,_tmp339);};};}_LL156: {struct Cyc_Absyn_Subscript_e_struct*_tmp1FC=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp194;if(_tmp1FC->tag != 24)goto _LL158;
else{_tmp1FD=_tmp1FC->f1;_tmp1FE=_tmp1FC->f2;}}_LL157: {struct _RegionHandle*
_tmp33C=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp33F;struct Cyc_List_List*_tmp340;
struct Cyc_Absyn_Exp*_tmp853[2];struct _tuple19 _tmp33E=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp33C,env,inflow,((_tmp853[1]=_tmp1FE,((_tmp853[0]=_tmp1FD,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp33C,
_tag_dyneither(_tmp853,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp33F=_tmp33E.f1;
_tmp340=_tmp33E.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp341=_tmp33F;{union Cyc_CfFlowInfo_FlowInfo
_tmp342=_tmp33F;struct _tuple12 _tmp343;struct Cyc_Dict_Dict _tmp344;struct Cyc_List_List*
_tmp345;_LL1DD: if((_tmp342.ReachableFL).tag != 2)goto _LL1DF;_tmp343=(struct
_tuple12)(_tmp342.ReachableFL).val;_tmp344=_tmp343.f1;_tmp345=_tmp343.f2;_LL1DE:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp344,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp340))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp856;void*_tmp855;(_tmp855=0,Cyc_Tcutil_terr(_tmp1FE->loc,((_tmp856="expression may not be initialized",
_tag_dyneither(_tmp856,sizeof(char),34))),_tag_dyneither(_tmp855,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp348=Cyc_NewControlFlow_add_subscript_reln(_tmp172->r,
_tmp345,_tmp1FD,_tmp1FE);if(_tmp345 != _tmp348)_tmp341=Cyc_CfFlowInfo_ReachableFL(
_tmp344,_tmp348);goto _LL1DC;};_LL1DF:;_LL1E0: goto _LL1DC;_LL1DC:;}{void*_tmp349=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1FD->topt))->v);
struct Cyc_List_List*_tmp34B;struct Cyc_Absyn_PtrInfo _tmp34D;struct Cyc_Absyn_PtrAtts
_tmp34E;union Cyc_Absyn_Constraint*_tmp34F;_LL1E2: {struct Cyc_Absyn_TupleType_struct*
_tmp34A=(struct Cyc_Absyn_TupleType_struct*)_tmp349;if(_tmp34A->tag != 11)goto
_LL1E4;else{_tmp34B=_tmp34A->f1;}}_LL1E3: {void*_tmp350=(void*)((struct Cyc_List_List*)
_check_null(_tmp340))->hd;struct _dyneither_ptr _tmp352;_LL1E9: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp351=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp350;if(_tmp351->tag != 6)goto
_LL1EB;else{_tmp352=_tmp351->f2;}}_LL1EA: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(
_tmp1FE)).f1;struct _tuple13 _tmp857;return(_tmp857.f1=_tmp341,((_tmp857.f2=*((
void**)_check_dyneither_subscript(_tmp352,sizeof(void*),(int)i)),_tmp857)));}
_LL1EB:;_LL1EC: {struct Cyc_Core_Impossible_struct _tmp85D;const char*_tmp85C;
struct Cyc_Core_Impossible_struct*_tmp85B;(int)_throw((void*)((_tmp85B=_cycalloc(
sizeof(*_tmp85B)),((_tmp85B[0]=((_tmp85D.tag=Cyc_Core_Impossible,((_tmp85D.f1=((
_tmp85C="anal_Rexp: Subscript",_tag_dyneither(_tmp85C,sizeof(char),21))),_tmp85D)))),
_tmp85B)))));}_LL1E8:;}_LL1E4: {struct Cyc_Absyn_PointerType_struct*_tmp34C=(
struct Cyc_Absyn_PointerType_struct*)_tmp349;if(_tmp34C->tag != 5)goto _LL1E6;else{
_tmp34D=_tmp34C->f1;_tmp34E=_tmp34D.ptr_atts;_tmp34F=_tmp34E.bounds;}}_LL1E5: {
union Cyc_CfFlowInfo_FlowInfo _tmp358;void*_tmp359;struct _tuple13 _tmp357=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp33F,_tmp1FD,(void*)((struct Cyc_List_List*)_check_null(_tmp340))->hd);
_tmp358=_tmp357.f1;_tmp359=_tmp357.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp35A=
_tmp358;int _tmp35B;_LL1EE: if((_tmp35A.BottomFL).tag != 1)goto _LL1F0;_tmp35B=(int)(
_tmp35A.BottomFL).val;_LL1EF: {struct _tuple13 _tmp85E;return(_tmp85E.f1=_tmp358,((
_tmp85E.f2=_tmp359,_tmp85E)));}_LL1F0:;_LL1F1: {struct _tuple13 _tmp85F;return(
_tmp85F.f1=_tmp341,((_tmp85F.f2=_tmp359,_tmp85F)));}_LL1ED:;};}_LL1E6:;_LL1E7: {
struct Cyc_Core_Impossible_struct _tmp865;const char*_tmp864;struct Cyc_Core_Impossible_struct*
_tmp863;(int)_throw((void*)((_tmp863=_cycalloc(sizeof(*_tmp863)),((_tmp863[0]=((
_tmp865.tag=Cyc_Core_Impossible,((_tmp865.f1=((_tmp864="anal_Rexp: Subscript -- bad type",
_tag_dyneither(_tmp864,sizeof(char),33))),_tmp865)))),_tmp863)))));}_LL1E1:;};};}
_LL158: {struct Cyc_Absyn_Datatype_e_struct*_tmp1FF=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp194;if(_tmp1FF->tag != 31)goto _LL15A;else{_tmp200=_tmp1FF->f1;_tmp201=_tmp1FF->f2;}}
_LL159: _tmp203=_tmp200;goto _LL15B;_LL15A: {struct Cyc_Absyn_Tuple_e_struct*
_tmp202=(struct Cyc_Absyn_Tuple_e_struct*)_tmp194;if(_tmp202->tag != 25)goto _LL15C;
else{_tmp203=_tmp202->f1;}}_LL15B: {struct _RegionHandle*_tmp361=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp363;struct Cyc_List_List*_tmp364;struct _tuple19 _tmp362=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp361,env,inflow,_tmp203,1,1);_tmp363=_tmp362.f1;_tmp364=_tmp362.f2;{
unsigned int _tmp86B;unsigned int _tmp86A;struct _dyneither_ptr _tmp869;void**
_tmp868;unsigned int _tmp867;struct _dyneither_ptr aggrdict=(_tmp867=(unsigned int)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp203),((_tmp868=(void**)
_region_malloc(env->r,_check_times(sizeof(void*),_tmp867)),((_tmp869=
_tag_dyneither(_tmp868,sizeof(void*),_tmp867),((((_tmp86A=_tmp867,_tmp86C(&
_tmp86B,& _tmp86A,& _tmp868,& _tmp364))),_tmp869)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp876;struct Cyc_CfFlowInfo_UnionRInfo _tmp875;struct Cyc_CfFlowInfo_Aggregate_struct
_tmp874;struct _tuple13 _tmp873;return(_tmp873.f1=_tmp363,((_tmp873.f2=(void*)((
_tmp876=_region_malloc(env->r,sizeof(*_tmp876)),((_tmp876[0]=((_tmp874.tag=6,((
_tmp874.f1=((_tmp875.is_union=0,((_tmp875.fieldnum=- 1,_tmp875)))),((_tmp874.f2=
aggrdict,_tmp874)))))),_tmp876)))),_tmp873)));};}_LL15C: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp204=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp194;if(_tmp204->tag != 30)goto
_LL15E;else{_tmp205=_tmp204->f2;}}_LL15D: {struct Cyc_List_List*fs;{void*_tmp36E=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_List_List*_tmp370;_LL1F3: {struct Cyc_Absyn_AnonAggrType_struct*_tmp36F=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp36E;if(_tmp36F->tag != 13)goto _LL1F5;
else{_tmp370=_tmp36F->f2;}}_LL1F4: fs=_tmp370;goto _LL1F2;_LL1F5:;_LL1F6: {struct
Cyc_Core_Impossible_struct _tmp87C;const char*_tmp87B;struct Cyc_Core_Impossible_struct*
_tmp87A;(int)_throw((void*)((_tmp87A=_cycalloc(sizeof(*_tmp87A)),((_tmp87A[0]=((
_tmp87C.tag=Cyc_Core_Impossible,((_tmp87C.f1=((_tmp87B="anal_Rexp:anon struct has bad type",
_tag_dyneither(_tmp87B,sizeof(char),35))),_tmp87C)))),_tmp87A)))));}_LL1F2:;}
_tmp207=_tmp205;_tmp20A=Cyc_Absyn_StructA;_tmp20D=fs;goto _LL15F;}_LL15E: {struct
Cyc_Absyn_Aggregate_e_struct*_tmp206=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp194;if(_tmp206->tag != 29)goto _LL160;else{_tmp207=_tmp206->f3;_tmp208=_tmp206->f4;
if(_tmp208 == 0)goto _LL160;_tmp209=*_tmp208;_tmp20A=_tmp209.kind;_tmp20B=_tmp209.impl;
if(_tmp20B == 0)goto _LL160;_tmp20C=*_tmp20B;_tmp20D=_tmp20C.fields;}}_LL15F: {
struct _RegionHandle*_tmp374=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp376;struct Cyc_List_List*
_tmp377;struct _tuple19 _tmp375=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp374,
env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp374,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp207),1,1);_tmp376=_tmp375.f1;_tmp377=_tmp375.f2;{
struct _dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp172,
_tmp20D,_tmp20A == Cyc_Absyn_UnionA,_tmp20A == Cyc_Absyn_UnionA?_tmp172->unknown_none:
_tmp172->unknown_all);int field_no=- 1;{int i=0;for(0;_tmp377 != 0;(((_tmp377=
_tmp377->tl,_tmp207=_tmp207->tl)),++ i)){struct Cyc_List_List*ds=(*((struct
_tuple21*)((struct Cyc_List_List*)_check_null(_tmp207))->hd)).f1;for(0;ds != 0;ds=
ds->tl){void*_tmp378=(void*)ds->hd;struct _dyneither_ptr*_tmp37B;_LL1F8: {struct
Cyc_Absyn_ArrayElement_struct*_tmp379=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp378;if(_tmp379->tag != 0)goto _LL1FA;}_LL1F9: {struct Cyc_Core_Impossible_struct
_tmp882;const char*_tmp881;struct Cyc_Core_Impossible_struct*_tmp880;(int)_throw((
void*)((_tmp880=_cycalloc(sizeof(*_tmp880)),((_tmp880[0]=((_tmp882.tag=Cyc_Core_Impossible,((
_tmp882.f1=((_tmp881="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp881,sizeof(char),
22))),_tmp882)))),_tmp880)))));}_LL1FA: {struct Cyc_Absyn_FieldName_struct*
_tmp37A=(struct Cyc_Absyn_FieldName_struct*)_tmp378;if(_tmp37A->tag != 1)goto
_LL1F7;else{_tmp37B=_tmp37A->f1;}}_LL1FB: field_no=Cyc_CfFlowInfo_get_field_index_fs(
_tmp20D,_tmp37B);*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),
field_no))=(void*)_tmp377->hd;if(_tmp20A == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*
_tmp37F=(*((struct _tuple21*)_tmp207->hd)).f2;_tmp376=Cyc_NewControlFlow_use_Rval(
env,_tmp37F->loc,_tmp376,(void*)_tmp377->hd);}_LL1F7:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp88C;struct Cyc_CfFlowInfo_UnionRInfo _tmp88B;struct Cyc_CfFlowInfo_Aggregate_struct
_tmp88A;struct _tuple13 _tmp889;return(_tmp889.f1=_tmp376,((_tmp889.f2=(void*)((
_tmp88C=_region_malloc(env->r,sizeof(*_tmp88C)),((_tmp88C[0]=((_tmp88A.tag=6,((
_tmp88A.f1=((_tmp88B.is_union=_tmp20A == Cyc_Absyn_UnionA,((_tmp88B.fieldnum=
field_no,_tmp88B)))),((_tmp88A.f2=aggrdict,_tmp88A)))))),_tmp88C)))),_tmp889)));};};}
_LL160: {struct Cyc_Absyn_Aggregate_e_struct*_tmp20E=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp194;if(_tmp20E->tag != 29)goto _LL162;}_LL161: {struct Cyc_Core_Impossible_struct
_tmp892;const char*_tmp891;struct Cyc_Core_Impossible_struct*_tmp890;(int)_throw((
void*)((_tmp890=_cycalloc(sizeof(*_tmp890)),((_tmp890[0]=((_tmp892.tag=Cyc_Core_Impossible,((
_tmp892.f1=((_tmp891="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp891,
sizeof(char),31))),_tmp892)))),_tmp890)))));}_LL162: {struct Cyc_Absyn_Array_e_struct*
_tmp20F=(struct Cyc_Absyn_Array_e_struct*)_tmp194;if(_tmp20F->tag != 27)goto _LL164;
else{_tmp210=_tmp20F->f1;}}_LL163: {struct _RegionHandle*_tmp387=env->r;struct Cyc_List_List*
_tmp388=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp387,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp210);union Cyc_CfFlowInfo_FlowInfo _tmp38A;
struct Cyc_List_List*_tmp38B;struct _tuple19 _tmp389=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp387,env,inflow,_tmp388,1,1);_tmp38A=_tmp389.f1;_tmp38B=_tmp389.f2;for(0;
_tmp38B != 0;(_tmp38B=_tmp38B->tl,_tmp388=_tmp388->tl)){_tmp38A=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp388))->hd)->loc,
_tmp38A,(void*)_tmp38B->hd);}{struct _tuple13 _tmp893;return(_tmp893.f1=_tmp38A,((
_tmp893.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0,_tmp172->unknown_all),_tmp893)));};}_LL164: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp211=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp194;if(_tmp211->tag != 28)goto _LL166;else{_tmp212=_tmp211->f1;_tmp213=_tmp211->f2;
_tmp214=_tmp211->f3;_tmp215=_tmp211->f4;}}_LL165: {union Cyc_CfFlowInfo_FlowInfo
_tmp38E;void*_tmp38F;struct _tuple13 _tmp38D=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp213);_tmp38E=_tmp38D.f1;_tmp38F=_tmp38D.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp390=_tmp38E;int _tmp391;struct _tuple12 _tmp392;struct Cyc_Dict_Dict _tmp393;
struct Cyc_List_List*_tmp394;_LL1FD: if((_tmp390.BottomFL).tag != 1)goto _LL1FF;
_tmp391=(int)(_tmp390.BottomFL).val;_LL1FE: {struct _tuple13 _tmp894;return(
_tmp894.f1=_tmp38E,((_tmp894.f2=_tmp172->unknown_all,_tmp894)));}_LL1FF: if((
_tmp390.ReachableFL).tag != 2)goto _LL1FC;_tmp392=(struct _tuple12)(_tmp390.ReachableFL).val;
_tmp393=_tmp392.f1;_tmp394=_tmp392.f2;_LL200: if(Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp393,_tmp38F)== Cyc_CfFlowInfo_NoneIL){const char*_tmp897;void*_tmp896;(
_tmp896=0,Cyc_Tcutil_terr(_tmp213->loc,((_tmp897="expression may not be initialized",
_tag_dyneither(_tmp897,sizeof(char),34))),_tag_dyneither(_tmp896,sizeof(void*),0)));}{
struct Cyc_List_List*new_relns=_tmp394;comp_loop: {void*_tmp398=_tmp213->r;struct
Cyc_Absyn_Exp*_tmp39A;void*_tmp39C;struct Cyc_Absyn_Vardecl*_tmp39E;void*_tmp3A0;
struct Cyc_Absyn_Vardecl*_tmp3A2;void*_tmp3A4;struct Cyc_Absyn_Vardecl*_tmp3A6;
void*_tmp3A8;struct Cyc_Absyn_Vardecl*_tmp3AA;union Cyc_Absyn_Cnst _tmp3AC;struct
_tuple6 _tmp3AD;int _tmp3AE;enum Cyc_Absyn_Primop _tmp3B0;struct Cyc_List_List*
_tmp3B1;struct Cyc_List_List _tmp3B2;struct Cyc_Absyn_Exp*_tmp3B3;_LL202: {struct
Cyc_Absyn_Cast_e_struct*_tmp399=(struct Cyc_Absyn_Cast_e_struct*)_tmp398;if(
_tmp399->tag != 15)goto _LL204;else{_tmp39A=_tmp399->f2;}}_LL203: _tmp213=_tmp39A;
goto comp_loop;_LL204:{struct Cyc_Absyn_Var_e_struct*_tmp39B=(struct Cyc_Absyn_Var_e_struct*)
_tmp398;if(_tmp39B->tag != 1)goto _LL206;else{_tmp39C=(void*)_tmp39B->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp39D=(struct Cyc_Absyn_Global_b_struct*)_tmp39C;if(
_tmp39D->tag != 1)goto _LL206;else{_tmp39E=_tmp39D->f1;}};}}if(!(!_tmp39E->escapes))
goto _LL206;_LL205: _tmp3A2=_tmp39E;goto _LL207;_LL206:{struct Cyc_Absyn_Var_e_struct*
_tmp39F=(struct Cyc_Absyn_Var_e_struct*)_tmp398;if(_tmp39F->tag != 1)goto _LL208;
else{_tmp3A0=(void*)_tmp39F->f2;{struct Cyc_Absyn_Local_b_struct*_tmp3A1=(struct
Cyc_Absyn_Local_b_struct*)_tmp3A0;if(_tmp3A1->tag != 4)goto _LL208;else{_tmp3A2=
_tmp3A1->f1;}};}}if(!(!_tmp3A2->escapes))goto _LL208;_LL207: _tmp3A6=_tmp3A2;goto
_LL209;_LL208:{struct Cyc_Absyn_Var_e_struct*_tmp3A3=(struct Cyc_Absyn_Var_e_struct*)
_tmp398;if(_tmp3A3->tag != 1)goto _LL20A;else{_tmp3A4=(void*)_tmp3A3->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp3A5=(struct Cyc_Absyn_Pat_b_struct*)_tmp3A4;if(_tmp3A5->tag
!= 5)goto _LL20A;else{_tmp3A6=_tmp3A5->f1;}};}}if(!(!_tmp3A6->escapes))goto _LL20A;
_LL209: _tmp3AA=_tmp3A6;goto _LL20B;_LL20A:{struct Cyc_Absyn_Var_e_struct*_tmp3A7=(
struct Cyc_Absyn_Var_e_struct*)_tmp398;if(_tmp3A7->tag != 1)goto _LL20C;else{
_tmp3A8=(void*)_tmp3A7->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3A9=(struct Cyc_Absyn_Param_b_struct*)
_tmp3A8;if(_tmp3A9->tag != 3)goto _LL20C;else{_tmp3AA=_tmp3A9->f1;}};}}if(!(!
_tmp3AA->escapes))goto _LL20C;_LL20B:{struct Cyc_CfFlowInfo_Reln*_tmp89A;struct Cyc_List_List*
_tmp899;new_relns=((_tmp899=_region_malloc(env->r,sizeof(*_tmp899)),((_tmp899->hd=((
_tmp89A=_region_malloc(env->r,sizeof(*_tmp89A)),((_tmp89A->vd=_tmp212,((_tmp89A->rop=
Cyc_CfFlowInfo_LessVar(_tmp3AA,_tmp3AA->type),_tmp89A)))))),((_tmp899->tl=
_tmp394,_tmp899))))));}goto _LL201;_LL20C: {struct Cyc_Absyn_Const_e_struct*
_tmp3AB=(struct Cyc_Absyn_Const_e_struct*)_tmp398;if(_tmp3AB->tag != 0)goto _LL20E;
else{_tmp3AC=_tmp3AB->f1;if((_tmp3AC.Int_c).tag != 4)goto _LL20E;_tmp3AD=(struct
_tuple6)(_tmp3AC.Int_c).val;_tmp3AE=_tmp3AD.f2;}}_LL20D:{struct Cyc_CfFlowInfo_Reln*
_tmp89D;struct Cyc_List_List*_tmp89C;new_relns=((_tmp89C=_region_malloc(env->r,
sizeof(*_tmp89C)),((_tmp89C->hd=((_tmp89D=_region_malloc(env->r,sizeof(*_tmp89D)),((
_tmp89D->vd=_tmp212,((_tmp89D->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp3AE),_tmp89D)))))),((_tmp89C->tl=_tmp394,_tmp89C))))));}goto _LL201;_LL20E: {
struct Cyc_Absyn_Primop_e_struct*_tmp3AF=(struct Cyc_Absyn_Primop_e_struct*)
_tmp398;if(_tmp3AF->tag != 3)goto _LL210;else{_tmp3B0=_tmp3AF->f1;_tmp3B1=_tmp3AF->f2;
if(_tmp3B1 == 0)goto _LL210;_tmp3B2=*_tmp3B1;_tmp3B3=(struct Cyc_Absyn_Exp*)_tmp3B2.hd;}}
_LL20F:{void*_tmp3B8=_tmp3B3->r;void*_tmp3BA;struct Cyc_Absyn_Vardecl*_tmp3BC;
void*_tmp3BE;struct Cyc_Absyn_Vardecl*_tmp3C0;void*_tmp3C2;struct Cyc_Absyn_Vardecl*
_tmp3C4;void*_tmp3C6;struct Cyc_Absyn_Vardecl*_tmp3C8;_LL213:{struct Cyc_Absyn_Var_e_struct*
_tmp3B9=(struct Cyc_Absyn_Var_e_struct*)_tmp3B8;if(_tmp3B9->tag != 1)goto _LL215;
else{_tmp3BA=(void*)_tmp3B9->f2;{struct Cyc_Absyn_Global_b_struct*_tmp3BB=(struct
Cyc_Absyn_Global_b_struct*)_tmp3BA;if(_tmp3BB->tag != 1)goto _LL215;else{_tmp3BC=
_tmp3BB->f1;}};}}if(!(!_tmp3BC->escapes))goto _LL215;_LL214: _tmp3C0=_tmp3BC;goto
_LL216;_LL215:{struct Cyc_Absyn_Var_e_struct*_tmp3BD=(struct Cyc_Absyn_Var_e_struct*)
_tmp3B8;if(_tmp3BD->tag != 1)goto _LL217;else{_tmp3BE=(void*)_tmp3BD->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp3BF=(struct Cyc_Absyn_Local_b_struct*)_tmp3BE;if(
_tmp3BF->tag != 4)goto _LL217;else{_tmp3C0=_tmp3BF->f1;}};}}if(!(!_tmp3C0->escapes))
goto _LL217;_LL216: _tmp3C4=_tmp3C0;goto _LL218;_LL217:{struct Cyc_Absyn_Var_e_struct*
_tmp3C1=(struct Cyc_Absyn_Var_e_struct*)_tmp3B8;if(_tmp3C1->tag != 1)goto _LL219;
else{_tmp3C2=(void*)_tmp3C1->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp3C3=(struct Cyc_Absyn_Pat_b_struct*)
_tmp3C2;if(_tmp3C3->tag != 5)goto _LL219;else{_tmp3C4=_tmp3C3->f1;}};}}if(!(!
_tmp3C4->escapes))goto _LL219;_LL218: _tmp3C8=_tmp3C4;goto _LL21A;_LL219:{struct Cyc_Absyn_Var_e_struct*
_tmp3C5=(struct Cyc_Absyn_Var_e_struct*)_tmp3B8;if(_tmp3C5->tag != 1)goto _LL21B;
else{_tmp3C6=(void*)_tmp3C5->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3C7=(struct
Cyc_Absyn_Param_b_struct*)_tmp3C6;if(_tmp3C7->tag != 3)goto _LL21B;else{_tmp3C8=
_tmp3C7->f1;}};}}if(!(!_tmp3C8->escapes))goto _LL21B;_LL21A:{struct Cyc_CfFlowInfo_Reln*
_tmp8A0;struct Cyc_List_List*_tmp89F;new_relns=((_tmp89F=_region_malloc(env->r,
sizeof(*_tmp89F)),((_tmp89F->hd=((_tmp8A0=_region_malloc(env->r,sizeof(*_tmp8A0)),((
_tmp8A0->vd=_tmp212,((_tmp8A0->rop=Cyc_CfFlowInfo_LessNumelts(_tmp3C8),_tmp8A0)))))),((
_tmp89F->tl=_tmp394,_tmp89F))))));}goto _LL212;_LL21B:;_LL21C: goto _LL212;_LL212:;}
goto _LL201;_LL210:;_LL211: goto _LL201;_LL201:;}if(_tmp394 != new_relns)_tmp38E=Cyc_CfFlowInfo_ReachableFL(
_tmp393,new_relns);{void*_tmp3CB=_tmp38F;_LL21E: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3CC=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3CB;if(_tmp3CC->tag != 0)goto
_LL220;}_LL21F: {struct _tuple13 _tmp8A1;return(_tmp8A1.f1=_tmp38E,((_tmp8A1.f2=
_tmp172->unknown_all,_tmp8A1)));}_LL220: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3CD=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3CB;if(_tmp3CD->tag != 2)
goto _LL222;}_LL221: goto _LL223;_LL222: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3CE=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3CB;if(_tmp3CE->tag != 1)
goto _LL224;}_LL223: goto _LL225;_LL224: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3CF=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3CB;if(_tmp3CF->tag != 5)goto
_LL226;}_LL225: {struct Cyc_List_List _tmp8A2;struct Cyc_List_List _tmp3D1=(_tmp8A2.hd=
_tmp212,((_tmp8A2.tl=0,_tmp8A2)));_tmp38E=Cyc_NewControlFlow_add_vars(_tmp172,
_tmp38E,(struct Cyc_List_List*)& _tmp3D1,_tmp172->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3D3;void*_tmp3D4;struct _tuple13 _tmp3D2=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp38E,_tmp214);_tmp3D3=_tmp3D2.f1;_tmp3D4=_tmp3D2.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3D5=_tmp3D3;int _tmp3D6;struct _tuple12 _tmp3D7;struct Cyc_Dict_Dict _tmp3D8;
_LL229: if((_tmp3D5.BottomFL).tag != 1)goto _LL22B;_tmp3D6=(int)(_tmp3D5.BottomFL).val;
_LL22A: {struct _tuple13 _tmp8A3;return(_tmp8A3.f1=_tmp3D3,((_tmp8A3.f2=_tmp172->unknown_all,
_tmp8A3)));}_LL22B: if((_tmp3D5.ReachableFL).tag != 2)goto _LL228;_tmp3D7=(struct
_tuple12)(_tmp3D5.ReachableFL).val;_tmp3D8=_tmp3D7.f1;_LL22C: if(Cyc_CfFlowInfo_initlevel(
_tmp172,_tmp3D8,_tmp3D4)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp8A6;void*_tmp8A5;(
_tmp8A5=0,Cyc_Tcutil_terr(_tmp213->loc,((_tmp8A6="expression may not be initialized",
_tag_dyneither(_tmp8A6,sizeof(char),34))),_tag_dyneither(_tmp8A5,sizeof(void*),0)));}{
struct _tuple13 _tmp8A7;return(_tmp8A7.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8A7.f2=
_tmp172->unknown_all,_tmp8A7)));};}_LL228:;}_tmp38E=_tmp3D3;goto _LL227;};}_LL226:;
_LL227: while(1){struct Cyc_List_List _tmp8A8;struct Cyc_List_List _tmp3DE=(_tmp8A8.hd=
_tmp212,((_tmp8A8.tl=0,_tmp8A8)));_tmp38E=Cyc_NewControlFlow_add_vars(_tmp172,
_tmp38E,(struct Cyc_List_List*)& _tmp3DE,_tmp172->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3E0;void*_tmp3E1;struct _tuple13 _tmp3DF=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp38E,_tmp214);_tmp3E0=_tmp3DF.f1;_tmp3E1=_tmp3DF.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3E2=_tmp3E0;int _tmp3E3;struct _tuple12 _tmp3E4;struct Cyc_Dict_Dict _tmp3E5;
_LL22E: if((_tmp3E2.BottomFL).tag != 1)goto _LL230;_tmp3E3=(int)(_tmp3E2.BottomFL).val;
_LL22F: goto _LL22D;_LL230: if((_tmp3E2.ReachableFL).tag != 2)goto _LL22D;_tmp3E4=(
struct _tuple12)(_tmp3E2.ReachableFL).val;_tmp3E5=_tmp3E4.f1;_LL231: if(Cyc_CfFlowInfo_initlevel(
_tmp172,_tmp3E5,_tmp3E1)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp8AB;void*_tmp8AA;(
_tmp8AA=0,Cyc_Tcutil_terr(_tmp213->loc,((_tmp8AB="expression may not be initialized",
_tag_dyneither(_tmp8AB,sizeof(char),34))),_tag_dyneither(_tmp8AA,sizeof(void*),0)));}{
struct _tuple13 _tmp8AC;return(_tmp8AC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8AC.f2=
_tmp172->unknown_all,_tmp8AC)));};}_LL22D:;}{union Cyc_CfFlowInfo_FlowInfo _tmp3E9=
Cyc_CfFlowInfo_join_flow(_tmp172,env->all_changed,_tmp38E,_tmp3E0);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3E9,_tmp38E))break;_tmp38E=_tmp3E9;};};}{struct _tuple13 _tmp8AD;return(
_tmp8AD.f1=_tmp38E,((_tmp8AD.f2=_tmp172->unknown_all,_tmp8AD)));};_LL21D:;};};
_LL1FC:;};}_LL166: {struct Cyc_Absyn_StmtExp_e_struct*_tmp216=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp194;if(_tmp216->tag != 37)goto _LL168;else{_tmp217=_tmp216->f1;}}_LL167: return
Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp217);_LL168: {struct
Cyc_Absyn_Var_e_struct*_tmp218=(struct Cyc_Absyn_Var_e_struct*)_tmp194;if(_tmp218->tag
!= 1)goto _LL16A;else{_tmp219=(void*)_tmp218->f2;{struct Cyc_Absyn_Unresolved_b_struct*
_tmp21A=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp219;if(_tmp21A->tag != 0)goto
_LL16A;};}}_LL169: goto _LL16B;_LL16A: {struct Cyc_Absyn_UnknownId_e_struct*_tmp21B=(
struct Cyc_Absyn_UnknownId_e_struct*)_tmp194;if(_tmp21B->tag != 2)goto _LL16C;}
_LL16B: goto _LL16D;_LL16C: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp21C=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp194;if(_tmp21C->tag != 10)goto _LL16E;}_LL16D:
goto _LL16F;_LL16E: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp21D=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp194;if(_tmp21D->tag != 36)goto _LL170;}_LL16F: goto _LL171;_LL170: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp21E=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp194;if(_tmp21E->tag != 26)goto
_LL172;}_LL171: goto _LL173;_LL172: {struct Cyc_Absyn_Valueof_e_struct*_tmp21F=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp194;if(_tmp21F->tag != 39)goto _LL174;}
_LL173: goto _LL175;_LL174: {struct Cyc_Absyn_Asm_e_struct*_tmp220=(struct Cyc_Absyn_Asm_e_struct*)
_tmp194;if(_tmp220->tag != 40)goto _LL10B;}_LL175: {struct Cyc_Core_Impossible_struct
_tmp8B3;const char*_tmp8B2;struct Cyc_Core_Impossible_struct*_tmp8B1;(int)_throw((
void*)((_tmp8B1=_cycalloc(sizeof(*_tmp8B1)),((_tmp8B1[0]=((_tmp8B3.tag=Cyc_Core_Impossible,((
_tmp8B3.f1=((_tmp8B2="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp8B2,
sizeof(char),31))),_tmp8B3)))),_tmp8B1)))));}_LL10B:;};}static struct _tuple14 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3EF=env->fenv;void*_tmp3F0=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp3F2;void*_tmp3F3;struct Cyc_Absyn_PtrAtts _tmp3F4;union Cyc_Absyn_Constraint*
_tmp3F5;union Cyc_Absyn_Constraint*_tmp3F6;_LL233: {struct Cyc_Absyn_PointerType_struct*
_tmp3F1=(struct Cyc_Absyn_PointerType_struct*)_tmp3F0;if(_tmp3F1->tag != 5)goto
_LL235;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F2.elt_typ;_tmp3F4=_tmp3F2.ptr_atts;
_tmp3F5=_tmp3F4.bounds;_tmp3F6=_tmp3F4.zero_term;}}_LL234: {union Cyc_CfFlowInfo_FlowInfo
_tmp3F7=f;int _tmp3F8;struct _tuple12 _tmp3F9;struct Cyc_Dict_Dict _tmp3FA;struct Cyc_List_List*
_tmp3FB;_LL238: if((_tmp3F7.BottomFL).tag != 1)goto _LL23A;_tmp3F8=(int)(_tmp3F7.BottomFL).val;
_LL239: {struct _tuple14 _tmp8B4;return(_tmp8B4.f1=f,((_tmp8B4.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8B4)));}_LL23A: if((_tmp3F7.ReachableFL).tag != 2)goto _LL237;_tmp3F9=(struct
_tuple12)(_tmp3F7.ReachableFL).val;_tmp3FA=_tmp3F9.f1;_tmp3FB=_tmp3F9.f2;_LL23B:
if(Cyc_Tcutil_is_bound_one(_tmp3F5)){void*_tmp3FD=r;struct Cyc_CfFlowInfo_Place*
_tmp401;struct Cyc_CfFlowInfo_Place _tmp402;void*_tmp403;struct Cyc_List_List*
_tmp404;enum Cyc_CfFlowInfo_InitLevel _tmp407;_LL23D: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3FE=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3FD;if(_tmp3FE->tag != 1)
goto _LL23F;}_LL23E: goto _LL240;_LL23F: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3FF=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3FD;if(_tmp3FF->tag != 2)
goto _LL241;}_LL240:{struct Cyc_CfFlowInfo_NotZero_struct _tmp8B7;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp8B6;e->annot=(void*)((_tmp8B6=_cycalloc(sizeof(*_tmp8B6)),((_tmp8B6[0]=((
_tmp8B7.tag=Cyc_CfFlowInfo_NotZero,((_tmp8B7.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3FB),_tmp8B7)))),_tmp8B6))));}goto _LL23C;_LL241: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp400=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3FD;if(_tmp400->tag != 5)goto
_LL243;else{_tmp401=_tmp400->f1;_tmp402=*_tmp401;_tmp403=_tmp402.root;_tmp404=
_tmp402.fields;}}_LL242:{struct Cyc_CfFlowInfo_NotZero_struct _tmp8BA;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp8B9;e->annot=(void*)((_tmp8B9=_cycalloc(sizeof(*_tmp8B9)),((_tmp8B9[0]=((
_tmp8BA.tag=Cyc_CfFlowInfo_NotZero,((_tmp8BA.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3FB),_tmp8BA)))),_tmp8B9))));}{struct Cyc_CfFlowInfo_Place*_tmp8BD;struct
_tuple14 _tmp8BC;return(_tmp8BC.f1=f,((_tmp8BC.f2=Cyc_CfFlowInfo_PlaceL(((_tmp8BD=
_region_malloc(_tmp3EF->r,sizeof(*_tmp8BD)),((_tmp8BD->root=_tmp403,((_tmp8BD->fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(_tmp3EF->r,_tmp404,flds),_tmp8BD))))))),_tmp8BC)));};_LL243: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp405=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp3FD;if(_tmp405->tag != 0)goto _LL245;}_LL244: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp8BE;return(_tmp8BE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8BE.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp8BE)));};_LL245: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp406=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3FD;if(_tmp406->tag != 3)goto
_LL247;else{_tmp407=_tmp406->f1;}}_LL246: f=Cyc_NewControlFlow_notzero(env,inflow,
f,e,_tmp407);goto _LL248;_LL247:;_LL248: {struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp8C1;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp8C0;e->annot=(void*)((_tmp8C0=
_cycalloc(sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8C1.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp8C1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3FB),_tmp8C1)))),
_tmp8C0))));}_LL23C:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp8C4;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp8C3;e->annot=(void*)((_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3[0]=((
_tmp8C4.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8C4.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3FB),_tmp8C4)))),_tmp8C3))));}if(Cyc_CfFlowInfo_initlevel(_tmp3EF,_tmp3FA,r)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp8C7;void*_tmp8C6;(_tmp8C6=0,Cyc_Tcutil_terr(
e->loc,((_tmp8C7="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp8C7,sizeof(char),46))),_tag_dyneither(_tmp8C6,sizeof(void*),0)));}{struct
_tuple14 _tmp8C8;return(_tmp8C8.f1=f,((_tmp8C8.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8C8)));};_LL237:;}_LL235:;_LL236: {struct Cyc_Core_Impossible_struct _tmp8CE;
const char*_tmp8CD;struct Cyc_Core_Impossible_struct*_tmp8CC;(int)_throw((void*)((
_tmp8CC=_cycalloc(sizeof(*_tmp8CC)),((_tmp8CC[0]=((_tmp8CE.tag=Cyc_Core_Impossible,((
_tmp8CE.f1=((_tmp8CD="left deref of non-pointer-type",_tag_dyneither(_tmp8CD,
sizeof(char),31))),_tmp8CE)))),_tmp8CC)))));}_LL232:;}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int
expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){struct Cyc_Dict_Dict
d;struct Cyc_CfFlowInfo_FlowEnv*_tmp419=env->fenv;{union Cyc_CfFlowInfo_FlowInfo
_tmp41A=inflow;int _tmp41B;struct _tuple12 _tmp41C;struct Cyc_Dict_Dict _tmp41D;
struct Cyc_List_List*_tmp41E;_LL24A: if((_tmp41A.BottomFL).tag != 1)goto _LL24C;
_tmp41B=(int)(_tmp41A.BottomFL).val;_LL24B: {struct _tuple14 _tmp8CF;return(
_tmp8CF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8CF.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8CF)));}_LL24C: if((_tmp41A.ReachableFL).tag != 2)goto _LL249;_tmp41C=(struct
_tuple12)(_tmp41A.ReachableFL).val;_tmp41D=_tmp41C.f1;_tmp41E=_tmp41C.f2;_LL24D:
d=_tmp41D;_LL249:;}{void*_tmp420=e->r;struct Cyc_Absyn_Exp*_tmp422;struct Cyc_Absyn_Exp*
_tmp424;struct Cyc_Absyn_Exp*_tmp426;void*_tmp428;struct Cyc_Absyn_Vardecl*_tmp42A;
void*_tmp42C;struct Cyc_Absyn_Vardecl*_tmp42E;void*_tmp430;struct Cyc_Absyn_Vardecl*
_tmp432;void*_tmp434;struct Cyc_Absyn_Vardecl*_tmp436;struct Cyc_Absyn_Exp*_tmp438;
struct _dyneither_ptr*_tmp439;struct Cyc_Absyn_Exp*_tmp43B;struct Cyc_Absyn_Exp*
_tmp43D;struct Cyc_Absyn_Exp*_tmp43E;struct Cyc_Absyn_Exp*_tmp440;struct
_dyneither_ptr*_tmp441;_LL24F: {struct Cyc_Absyn_Cast_e_struct*_tmp421=(struct Cyc_Absyn_Cast_e_struct*)
_tmp420;if(_tmp421->tag != 15)goto _LL251;else{_tmp422=_tmp421->f2;}}_LL250:
_tmp424=_tmp422;goto _LL252;_LL251: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp423=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp420;if(_tmp423->tag != 13)
goto _LL253;else{_tmp424=_tmp423->f1;}}_LL252: _tmp426=_tmp424;goto _LL254;_LL253: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp425=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp420;if(_tmp425->tag != 14)goto _LL255;else{_tmp426=_tmp425->f1;}}_LL254: return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp426,flds);_LL255: {
struct Cyc_Absyn_Var_e_struct*_tmp427=(struct Cyc_Absyn_Var_e_struct*)_tmp420;if(
_tmp427->tag != 1)goto _LL257;else{_tmp428=(void*)_tmp427->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp429=(struct Cyc_Absyn_Param_b_struct*)_tmp428;if(_tmp429->tag != 3)goto _LL257;
else{_tmp42A=_tmp429->f1;}};}}_LL256: _tmp42E=_tmp42A;goto _LL258;_LL257: {struct
Cyc_Absyn_Var_e_struct*_tmp42B=(struct Cyc_Absyn_Var_e_struct*)_tmp420;if(_tmp42B->tag
!= 1)goto _LL259;else{_tmp42C=(void*)_tmp42B->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp42D=(struct Cyc_Absyn_Local_b_struct*)_tmp42C;if(_tmp42D->tag != 4)goto _LL259;
else{_tmp42E=_tmp42D->f1;}};}}_LL258: _tmp432=_tmp42E;goto _LL25A;_LL259: {struct
Cyc_Absyn_Var_e_struct*_tmp42F=(struct Cyc_Absyn_Var_e_struct*)_tmp420;if(_tmp42F->tag
!= 1)goto _LL25B;else{_tmp430=(void*)_tmp42F->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp431=(struct Cyc_Absyn_Pat_b_struct*)_tmp430;if(_tmp431->tag != 5)goto _LL25B;
else{_tmp432=_tmp431->f1;}};}}_LL25A: {struct Cyc_CfFlowInfo_Place*_tmp8D9;struct
Cyc_CfFlowInfo_VarRoot_struct _tmp8D8;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp8D7;
struct _tuple14 _tmp8D6;return(_tmp8D6.f1=inflow,((_tmp8D6.f2=Cyc_CfFlowInfo_PlaceL(((
_tmp8D9=_region_malloc(env->r,sizeof(*_tmp8D9)),((_tmp8D9->root=(void*)((_tmp8D7=
_region_malloc(env->r,sizeof(*_tmp8D7)),((_tmp8D7[0]=((_tmp8D8.tag=0,((_tmp8D8.f1=
_tmp432,_tmp8D8)))),_tmp8D7)))),((_tmp8D9->fields=flds,_tmp8D9))))))),_tmp8D6)));}
_LL25B: {struct Cyc_Absyn_Var_e_struct*_tmp433=(struct Cyc_Absyn_Var_e_struct*)
_tmp420;if(_tmp433->tag != 1)goto _LL25D;else{_tmp434=(void*)_tmp433->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp435=(struct Cyc_Absyn_Global_b_struct*)_tmp434;if(
_tmp435->tag != 1)goto _LL25D;else{_tmp436=_tmp435->f1;}};}}_LL25C: {struct
_tuple14 _tmp8DA;return(_tmp8DA.f1=inflow,((_tmp8DA.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8DA)));}_LL25D: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp437=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp420;if(_tmp437->tag != 23)goto _LL25F;else{_tmp438=_tmp437->f1;_tmp439=_tmp437->f2;}}
_LL25E:{void*_tmp447=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp438->topt))->v);struct Cyc_Absyn_PtrInfo _tmp449;void*_tmp44A;
_LL268: {struct Cyc_Absyn_PointerType_struct*_tmp448=(struct Cyc_Absyn_PointerType_struct*)
_tmp447;if(_tmp448->tag != 5)goto _LL26A;else{_tmp449=_tmp448->f1;_tmp44A=_tmp449.elt_typ;}}
_LL269: if(!Cyc_Absyn_is_nontagged_union_type(_tmp44A)){struct Cyc_List_List*
_tmp8DB;flds=((_tmp8DB=_region_malloc(env->r,sizeof(*_tmp8DB)),((_tmp8DB->hd=(
void*)Cyc_CfFlowInfo_get_field_index(_tmp44A,_tmp439),((_tmp8DB->tl=flds,_tmp8DB))))));}
goto _LL267;_LL26A:;_LL26B: {struct Cyc_Core_Impossible_struct _tmp8E1;const char*
_tmp8E0;struct Cyc_Core_Impossible_struct*_tmp8DF;(int)_throw((void*)((_tmp8DF=
_cycalloc(sizeof(*_tmp8DF)),((_tmp8DF[0]=((_tmp8E1.tag=Cyc_Core_Impossible,((
_tmp8E1.f1=((_tmp8E0="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp8E0,sizeof(
char),25))),_tmp8E1)))),_tmp8DF)))));}_LL267:;}_tmp43B=_tmp438;goto _LL260;_LL25F: {
struct Cyc_Absyn_Deref_e_struct*_tmp43A=(struct Cyc_Absyn_Deref_e_struct*)_tmp420;
if(_tmp43A->tag != 21)goto _LL261;else{_tmp43B=_tmp43A->f1;}}_LL260: if(
expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp43B->topt))->v)){union Cyc_CfFlowInfo_FlowInfo _tmp450;union Cyc_CfFlowInfo_AbsLVal
_tmp451;struct _tuple14 _tmp44F=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp43B);
_tmp450=_tmp44F.f1;_tmp451=_tmp44F.f2;{struct _tuple14 _tmp8E2;struct _tuple14
_tmp453=(_tmp8E2.f1=_tmp450,((_tmp8E2.f2=_tmp451,_tmp8E2)));union Cyc_CfFlowInfo_FlowInfo
_tmp454;struct _tuple12 _tmp455;struct Cyc_Dict_Dict _tmp456;struct Cyc_List_List*
_tmp457;union Cyc_CfFlowInfo_AbsLVal _tmp458;struct Cyc_CfFlowInfo_Place*_tmp459;
_LL26D: _tmp454=_tmp453.f1;if((_tmp454.ReachableFL).tag != 2)goto _LL26F;_tmp455=(
struct _tuple12)(_tmp454.ReachableFL).val;_tmp456=_tmp455.f1;_tmp457=_tmp455.f2;
_tmp458=_tmp453.f2;if((_tmp458.PlaceL).tag != 1)goto _LL26F;_tmp459=(struct Cyc_CfFlowInfo_Place*)(
_tmp458.PlaceL).val;_LL26E: {void*_tmp45A=Cyc_CfFlowInfo_lookup_place(_tmp456,
_tmp459);void*_tmp45B=_tmp45A;_LL272: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp45C=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp45B;if(_tmp45C->tag != 5)goto
_LL274;}_LL273: return Cyc_NewControlFlow_anal_derefL(env,_tmp450,_tmp43B,_tmp450,
_tmp45A,flds);_LL274:;_LL275: {enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(
_tmp419,_tmp456,_tmp45A);void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp419->unknown_all:
_tmp419->unknown_none;void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp419,Cyc_Tcutil_pointer_elt_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp43B->topt))->v),0,leaf);struct Cyc_CfFlowInfo_MallocPt_struct
_tmp8E5;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp8E4;void*new_root=(void*)((
_tmp8E4=_region_malloc(_tmp419->r,sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E5.tag=1,((
_tmp8E5.f1=e,((_tmp8E5.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp8E5)))))),_tmp8E4))));struct Cyc_CfFlowInfo_Place*_tmp8E6;struct Cyc_CfFlowInfo_Place*
place=(_tmp8E6=_region_malloc(_tmp419->r,sizeof(*_tmp8E6)),((_tmp8E6->root=
new_root,((_tmp8E6->fields=0,_tmp8E6)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp8E9;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp8E8;void*res=(void*)((_tmp8E8=
_region_malloc(_tmp419->r,sizeof(*_tmp8E8)),((_tmp8E8[0]=((_tmp8E9.tag=5,((
_tmp8E9.f1=place,_tmp8E9)))),_tmp8E8))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);_tmp456=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(_tmp456,new_root,new_rval);_tmp456=Cyc_CfFlowInfo_assign_place(
_tmp419,e->loc,_tmp456,env->all_changed,_tmp459,res);{union Cyc_CfFlowInfo_FlowInfo
_tmp45D=Cyc_CfFlowInfo_ReachableFL(_tmp456,_tmp457);return Cyc_NewControlFlow_anal_derefL(
env,_tmp45D,_tmp43B,_tmp45D,res,flds);};}_LL271:;}_LL26F:;_LL270: goto _LL26C;
_LL26C:;};}{union Cyc_CfFlowInfo_FlowInfo _tmp464;void*_tmp465;struct _tuple13
_tmp463=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp43B);_tmp464=_tmp463.f1;
_tmp465=_tmp463.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp43B,
_tmp464,_tmp465,flds);};_LL261: {struct Cyc_Absyn_Subscript_e_struct*_tmp43C=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp420;if(_tmp43C->tag != 24)goto _LL263;
else{_tmp43D=_tmp43C->f1;_tmp43E=_tmp43C->f2;}}_LL262: {void*_tmp466=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp43D->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp469;struct Cyc_Absyn_PtrAtts _tmp46A;union Cyc_Absyn_Constraint*_tmp46B;_LL277: {
struct Cyc_Absyn_TupleType_struct*_tmp467=(struct Cyc_Absyn_TupleType_struct*)
_tmp466;if(_tmp467->tag != 11)goto _LL279;}_LL278: {unsigned int _tmp46C=(Cyc_Evexp_eval_const_uint_exp(
_tmp43E)).f1;struct Cyc_List_List*_tmp8EA;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,_tmp43D,((_tmp8EA=_region_malloc(env->r,sizeof(*_tmp8EA)),((
_tmp8EA->hd=(void*)((int)_tmp46C),((_tmp8EA->tl=flds,_tmp8EA)))))));}_LL279: {
struct Cyc_Absyn_PointerType_struct*_tmp468=(struct Cyc_Absyn_PointerType_struct*)
_tmp466;if(_tmp468->tag != 5)goto _LL27B;else{_tmp469=_tmp468->f1;_tmp46A=_tmp469.ptr_atts;
_tmp46B=_tmp46A.bounds;}}_LL27A: {struct _RegionHandle*_tmp46E=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp471;struct Cyc_List_List*_tmp472;struct Cyc_Absyn_Exp*_tmp8EB[2];struct
_tuple19 _tmp470=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp46E,env,inflow,((
_tmp8EB[1]=_tmp43E,((_tmp8EB[0]=_tmp43D,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp46E,_tag_dyneither(
_tmp8EB,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp471=_tmp470.f1;_tmp472=
_tmp470.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp473=_tmp471;{union Cyc_CfFlowInfo_FlowInfo
_tmp474=_tmp471;struct _tuple12 _tmp475;struct Cyc_Dict_Dict _tmp476;struct Cyc_List_List*
_tmp477;_LL27E: if((_tmp474.ReachableFL).tag != 2)goto _LL280;_tmp475=(struct
_tuple12)(_tmp474.ReachableFL).val;_tmp476=_tmp475.f1;_tmp477=_tmp475.f2;_LL27F:
if(Cyc_CfFlowInfo_initlevel(_tmp419,_tmp476,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp472))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8EE;void*_tmp8ED;(_tmp8ED=0,Cyc_Tcutil_terr(_tmp43E->loc,((_tmp8EE="expression may not be initialized",
_tag_dyneither(_tmp8EE,sizeof(char),34))),_tag_dyneither(_tmp8ED,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp47A=Cyc_NewControlFlow_add_subscript_reln(_tmp419->r,
_tmp477,_tmp43D,_tmp43E);if(_tmp477 != _tmp47A)_tmp473=Cyc_CfFlowInfo_ReachableFL(
_tmp476,_tmp47A);goto _LL27D;};_LL280:;_LL281: goto _LL27D;_LL27D:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp47C;union Cyc_CfFlowInfo_AbsLVal _tmp47D;struct _tuple14 _tmp47B=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp43D,_tmp471,(void*)((struct Cyc_List_List*)_check_null(_tmp472))->hd,
flds);_tmp47C=_tmp47B.f1;_tmp47D=_tmp47B.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp47E=
_tmp47C;int _tmp47F;_LL283: if((_tmp47E.BottomFL).tag != 1)goto _LL285;_tmp47F=(int)(
_tmp47E.BottomFL).val;_LL284: {struct _tuple14 _tmp8EF;return(_tmp8EF.f1=_tmp47C,((
_tmp8EF.f2=_tmp47D,_tmp8EF)));}_LL285:;_LL286: {struct _tuple14 _tmp8F0;return(
_tmp8F0.f1=_tmp473,((_tmp8F0.f2=_tmp47D,_tmp8F0)));}_LL282:;};};};}_LL27B:;
_LL27C: {struct Cyc_Core_Impossible_struct _tmp8F6;const char*_tmp8F5;struct Cyc_Core_Impossible_struct*
_tmp8F4;(int)_throw((void*)((_tmp8F4=_cycalloc(sizeof(*_tmp8F4)),((_tmp8F4[0]=((
_tmp8F6.tag=Cyc_Core_Impossible,((_tmp8F6.f1=((_tmp8F5="anal_Lexp: Subscript -- bad type",
_tag_dyneither(_tmp8F5,sizeof(char),33))),_tmp8F6)))),_tmp8F4)))));}_LL276:;}
_LL263: {struct Cyc_Absyn_AggrMember_e_struct*_tmp43F=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp420;if(_tmp43F->tag != 22)goto _LL265;else{_tmp440=_tmp43F->f1;_tmp441=_tmp43F->f2;}}
_LL264: if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp440->topt))->v)){struct _tuple14 _tmp8F7;return(_tmp8F7.f1=inflow,((
_tmp8F7.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8F7)));}{struct Cyc_List_List*_tmp8F8;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp440,((
_tmp8F8=_region_malloc(env->r,sizeof(*_tmp8F8)),((_tmp8F8->hd=(void*)Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp440->topt))->v,_tmp441),((_tmp8F8->tl=
flds,_tmp8F8)))))));};_LL265:;_LL266: {struct _tuple14 _tmp8F9;return(_tmp8F9.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp8F9.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8F9)));}
_LL24E:;};}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
union Cyc_CfFlowInfo_FlowInfo _tmp489;union Cyc_CfFlowInfo_AbsLVal _tmp48A;struct
_tuple14 _tmp488=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);
_tmp489=_tmp488.f1;_tmp48A=_tmp488.f2;{struct _tuple14 _tmp8FA;return(_tmp8FA.f1=
_tmp489,((_tmp8FA.f2=_tmp48A,_tmp8FA)));};}static struct _tuple15 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp48C=env->fenv;{void*_tmp48D=e->r;
struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Absyn_Exp*
_tmp491;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*_tmp494;struct Cyc_Absyn_Exp*
_tmp496;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Exp*
_tmp49A;enum Cyc_Absyn_Primop _tmp49C;struct Cyc_List_List*_tmp49D;struct Cyc_List_List
_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_List_List*_tmp4A0;enum Cyc_Absyn_Primop
_tmp4A2;struct Cyc_List_List*_tmp4A3;_LL288: {struct Cyc_Absyn_Conditional_e_struct*
_tmp48E=(struct Cyc_Absyn_Conditional_e_struct*)_tmp48D;if(_tmp48E->tag != 6)goto
_LL28A;else{_tmp48F=_tmp48E->f1;_tmp490=_tmp48E->f2;_tmp491=_tmp48E->f3;}}_LL289: {
union Cyc_CfFlowInfo_FlowInfo _tmp4A5;union Cyc_CfFlowInfo_FlowInfo _tmp4A6;struct
_tuple15 _tmp4A4=Cyc_NewControlFlow_anal_test(env,inflow,_tmp48F);_tmp4A5=_tmp4A4.f1;
_tmp4A6=_tmp4A4.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp4A8;union Cyc_CfFlowInfo_FlowInfo
_tmp4A9;struct _tuple15 _tmp4A7=Cyc_NewControlFlow_anal_test(env,_tmp4A5,_tmp490);
_tmp4A8=_tmp4A7.f1;_tmp4A9=_tmp4A7.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp4AB;union
Cyc_CfFlowInfo_FlowInfo _tmp4AC;struct _tuple15 _tmp4AA=Cyc_NewControlFlow_anal_test(
env,_tmp4A6,_tmp491);_tmp4AB=_tmp4AA.f1;_tmp4AC=_tmp4AA.f2;{struct _tuple15
_tmp8FB;return(_tmp8FB.f1=Cyc_CfFlowInfo_join_flow(_tmp48C,env->all_changed,
_tmp4A8,_tmp4AB),((_tmp8FB.f2=Cyc_CfFlowInfo_join_flow(_tmp48C,env->all_changed,
_tmp4A9,_tmp4AC),_tmp8FB)));};};};}_LL28A: {struct Cyc_Absyn_And_e_struct*_tmp492=(
struct Cyc_Absyn_And_e_struct*)_tmp48D;if(_tmp492->tag != 7)goto _LL28C;else{
_tmp493=_tmp492->f1;_tmp494=_tmp492->f2;}}_LL28B: {union Cyc_CfFlowInfo_FlowInfo
_tmp4AF;union Cyc_CfFlowInfo_FlowInfo _tmp4B0;struct _tuple15 _tmp4AE=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp493);_tmp4AF=_tmp4AE.f1;_tmp4B0=_tmp4AE.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4B2;union Cyc_CfFlowInfo_FlowInfo _tmp4B3;struct _tuple15 _tmp4B1=Cyc_NewControlFlow_anal_test(
env,_tmp4AF,_tmp494);_tmp4B2=_tmp4B1.f1;_tmp4B3=_tmp4B1.f2;{struct _tuple15
_tmp8FC;return(_tmp8FC.f1=_tmp4B2,((_tmp8FC.f2=Cyc_CfFlowInfo_join_flow(_tmp48C,
env->all_changed,_tmp4B0,_tmp4B3),_tmp8FC)));};};}_LL28C: {struct Cyc_Absyn_Or_e_struct*
_tmp495=(struct Cyc_Absyn_Or_e_struct*)_tmp48D;if(_tmp495->tag != 8)goto _LL28E;
else{_tmp496=_tmp495->f1;_tmp497=_tmp495->f2;}}_LL28D: {union Cyc_CfFlowInfo_FlowInfo
_tmp4B6;union Cyc_CfFlowInfo_FlowInfo _tmp4B7;struct _tuple15 _tmp4B5=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp496);_tmp4B6=_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4B9;union Cyc_CfFlowInfo_FlowInfo _tmp4BA;struct _tuple15 _tmp4B8=Cyc_NewControlFlow_anal_test(
env,_tmp4B7,_tmp497);_tmp4B9=_tmp4B8.f1;_tmp4BA=_tmp4B8.f2;{struct _tuple15
_tmp8FD;return(_tmp8FD.f1=Cyc_CfFlowInfo_join_flow(_tmp48C,env->all_changed,
_tmp4B6,_tmp4B9),((_tmp8FD.f2=_tmp4BA,_tmp8FD)));};};}_LL28E: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp498=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp48D;if(_tmp498->tag != 9)goto _LL290;
else{_tmp499=_tmp498->f1;_tmp49A=_tmp498->f2;}}_LL28F: {union Cyc_CfFlowInfo_FlowInfo
_tmp4BD;void*_tmp4BE;struct _tuple13 _tmp4BC=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp499);_tmp4BD=_tmp4BC.f1;_tmp4BE=_tmp4BC.f2;return Cyc_NewControlFlow_anal_test(
env,_tmp4BD,_tmp49A);}_LL290: {struct Cyc_Absyn_Primop_e_struct*_tmp49B=(struct
Cyc_Absyn_Primop_e_struct*)_tmp48D;if(_tmp49B->tag != 3)goto _LL292;else{_tmp49C=
_tmp49B->f1;if(_tmp49C != Cyc_Absyn_Not)goto _LL292;_tmp49D=_tmp49B->f2;if(_tmp49D
== 0)goto _LL292;_tmp49E=*_tmp49D;_tmp49F=(struct Cyc_Absyn_Exp*)_tmp49E.hd;
_tmp4A0=_tmp49E.tl;if(_tmp4A0 != 0)goto _LL292;}}_LL291: {union Cyc_CfFlowInfo_FlowInfo
_tmp4C0;union Cyc_CfFlowInfo_FlowInfo _tmp4C1;struct _tuple15 _tmp4BF=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp49F);_tmp4C0=_tmp4BF.f1;_tmp4C1=_tmp4BF.f2;{struct _tuple15 _tmp8FE;
return(_tmp8FE.f1=_tmp4C1,((_tmp8FE.f2=_tmp4C0,_tmp8FE)));};}_LL292: {struct Cyc_Absyn_Primop_e_struct*
_tmp4A1=(struct Cyc_Absyn_Primop_e_struct*)_tmp48D;if(_tmp4A1->tag != 3)goto _LL294;
else{_tmp4A2=_tmp4A1->f1;_tmp4A3=_tmp4A1->f2;}}_LL293: {void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo
f;struct _RegionHandle*_tmp4C3=env->r;{union Cyc_CfFlowInfo_FlowInfo _tmp4C5;struct
Cyc_List_List*_tmp4C6;struct _tuple19 _tmp4C4=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp4C3,env,inflow,_tmp4A3,0,0);_tmp4C5=_tmp4C4.f1;_tmp4C6=_tmp4C4.f2;r1=(void*)((
struct Cyc_List_List*)_check_null(_tmp4C6))->hd;r2=(void*)((struct Cyc_List_List*)
_check_null(_tmp4C6->tl))->hd;f=_tmp4C5;}{union Cyc_CfFlowInfo_FlowInfo _tmp4C7=f;
int _tmp4C8;struct _tuple12 _tmp4C9;struct Cyc_Dict_Dict _tmp4CA;struct Cyc_List_List*
_tmp4CB;_LL297: if((_tmp4C7.BottomFL).tag != 1)goto _LL299;_tmp4C8=(int)(_tmp4C7.BottomFL).val;
_LL298: {struct _tuple15 _tmp8FF;return(_tmp8FF.f1=f,((_tmp8FF.f2=f,_tmp8FF)));}
_LL299: if((_tmp4C7.ReachableFL).tag != 2)goto _LL296;_tmp4C9=(struct _tuple12)(
_tmp4C7.ReachableFL).val;_tmp4CA=_tmp4C9.f1;_tmp4CB=_tmp4C9.f2;_LL29A: {struct
Cyc_Absyn_Exp*_tmp4CD=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp4A3))->hd;struct Cyc_Absyn_Exp*_tmp4CE=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp4A3->tl))->hd;if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4CA,r1)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp902;void*_tmp901;(_tmp901=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp4A3->hd)->loc,((_tmp902="expression may not be initialized",
_tag_dyneither(_tmp902,sizeof(char),34))),_tag_dyneither(_tmp901,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4CA,r2)== Cyc_CfFlowInfo_NoneIL){const
char*_tmp905;void*_tmp904;(_tmp904=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A3->tl))->hd)->loc,((_tmp905="expression may not be initialized",
_tag_dyneither(_tmp905,sizeof(char),34))),_tag_dyneither(_tmp904,sizeof(void*),0)));}
if(_tmp4A2 == Cyc_Absyn_Eq  || _tmp4A2 == Cyc_Absyn_Neq){struct _tuple0 _tmp906;
struct _tuple0 _tmp4D4=(_tmp906.f1=r1,((_tmp906.f2=r2,_tmp906)));void*_tmp4D5;
enum Cyc_CfFlowInfo_InitLevel _tmp4D7;void*_tmp4D8;void*_tmp4DA;void*_tmp4DC;
enum Cyc_CfFlowInfo_InitLevel _tmp4DE;void*_tmp4DF;void*_tmp4E1;void*_tmp4E3;void*
_tmp4E5;void*_tmp4E7;void*_tmp4E9;void*_tmp4EB;void*_tmp4ED;void*_tmp4EF;void*
_tmp4F1;void*_tmp4F3;void*_tmp4F5;void*_tmp4F7;void*_tmp4F9;_LL29C: _tmp4D5=
_tmp4D4.f1;{struct Cyc_CfFlowInfo_UnknownR_struct*_tmp4D6=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4D5;if(_tmp4D6->tag != 3)goto _LL29E;else{_tmp4D7=_tmp4D6->f1;}};_tmp4D8=
_tmp4D4.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D9=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4D8;if(_tmp4D9->tag != 0)goto _LL29E;};_LL29D: {union Cyc_CfFlowInfo_FlowInfo
_tmp4FC;union Cyc_CfFlowInfo_FlowInfo _tmp4FD;struct _tuple15 _tmp4FB=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4CD,_tmp4D7);_tmp4FC=_tmp4FB.f1;_tmp4FD=_tmp4FB.f2;switch(_tmp4A2){
case Cyc_Absyn_Eq: _LL2B0: {struct _tuple15 _tmp907;return(_tmp907.f1=_tmp4FD,((
_tmp907.f2=_tmp4FC,_tmp907)));}case Cyc_Absyn_Neq: _LL2B1: {struct _tuple15 _tmp908;
return(_tmp908.f1=_tmp4FC,((_tmp908.f2=_tmp4FD,_tmp908)));}default: _LL2B2: {
struct Cyc_Core_Impossible_struct _tmp90E;const char*_tmp90D;struct Cyc_Core_Impossible_struct*
_tmp90C;(int)_throw((void*)((_tmp90C=_cycalloc(sizeof(*_tmp90C)),((_tmp90C[0]=((
_tmp90E.tag=Cyc_Core_Impossible,((_tmp90E.f1=((_tmp90D="anal_test, zero-split",
_tag_dyneither(_tmp90D,sizeof(char),22))),_tmp90E)))),_tmp90C)))));}}}_LL29E:
_tmp4DA=_tmp4D4.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4DB=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4DA;if(_tmp4DB->tag != 0)goto _LL2A0;};_tmp4DC=_tmp4D4.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4DD=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4DC;if(_tmp4DD->tag != 3)goto
_LL2A0;else{_tmp4DE=_tmp4DD->f1;}};_LL29F: {union Cyc_CfFlowInfo_FlowInfo _tmp504;
union Cyc_CfFlowInfo_FlowInfo _tmp505;struct _tuple15 _tmp503=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4CE,_tmp4DE);_tmp504=_tmp503.f1;_tmp505=_tmp503.f2;switch(_tmp4A2){
case Cyc_Absyn_Eq: _LL2B4: {struct _tuple15 _tmp90F;return(_tmp90F.f1=_tmp505,((
_tmp90F.f2=_tmp504,_tmp90F)));}case Cyc_Absyn_Neq: _LL2B5: {struct _tuple15 _tmp910;
return(_tmp910.f1=_tmp504,((_tmp910.f2=_tmp505,_tmp910)));}default: _LL2B6: {
struct Cyc_Core_Impossible_struct _tmp916;const char*_tmp915;struct Cyc_Core_Impossible_struct*
_tmp914;(int)_throw((void*)((_tmp914=_cycalloc(sizeof(*_tmp914)),((_tmp914[0]=((
_tmp916.tag=Cyc_Core_Impossible,((_tmp916.f1=((_tmp915="anal_test, zero-split",
_tag_dyneither(_tmp915,sizeof(char),22))),_tmp916)))),_tmp914)))));}}}_LL2A0:
_tmp4DF=_tmp4D4.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4E0=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4DF;if(_tmp4E0->tag != 0)goto _LL2A2;};_tmp4E1=_tmp4D4.f2;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4E2=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4E1;if(_tmp4E2->tag != 0)goto
_LL2A2;};_LL2A1: if(_tmp4A2 == Cyc_Absyn_Eq){struct _tuple15 _tmp917;return(_tmp917.f1=
f,((_tmp917.f2=Cyc_CfFlowInfo_BottomFL(),_tmp917)));}else{struct _tuple15 _tmp918;
return(_tmp918.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp918.f2=f,_tmp918)));}_LL2A2:
_tmp4E3=_tmp4D4.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4E4=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4E3;if(_tmp4E4->tag != 0)goto _LL2A4;};_tmp4E5=_tmp4D4.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4E6=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4E5;if(_tmp4E6->tag != 1)
goto _LL2A4;};_LL2A3: goto _LL2A5;_LL2A4: _tmp4E7=_tmp4D4.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4E8=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4E7;if(_tmp4E8->tag != 0)goto
_LL2A6;};_tmp4E9=_tmp4D4.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp4EA=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4E9;if(_tmp4EA->tag != 2)goto _LL2A6;};
_LL2A5: goto _LL2A7;_LL2A6: _tmp4EB=_tmp4D4.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4EC=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4EB;if(_tmp4EC->tag != 0)goto
_LL2A8;};_tmp4ED=_tmp4D4.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp4EE=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4ED;if(_tmp4EE->tag != 5)goto _LL2A8;};
_LL2A7: goto _LL2A9;_LL2A8: _tmp4EF=_tmp4D4.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4F0=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4EF;if(_tmp4F0->tag != 1)
goto _LL2AA;};_tmp4F1=_tmp4D4.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4F2=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4F1;if(_tmp4F2->tag != 0)goto _LL2AA;};
_LL2A9: goto _LL2AB;_LL2AA: _tmp4F3=_tmp4D4.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4F4=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4F3;if(_tmp4F4->tag != 2)
goto _LL2AC;};_tmp4F5=_tmp4D4.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4F6=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4F5;if(_tmp4F6->tag != 0)goto _LL2AC;};
_LL2AB: goto _LL2AD;_LL2AC: _tmp4F7=_tmp4D4.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp4F8=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4F7;if(_tmp4F8->tag != 5)goto
_LL2AE;};_tmp4F9=_tmp4D4.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4FA=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4F9;if(_tmp4FA->tag != 0)goto _LL2AE;};_LL2AD: if(_tmp4A2 == Cyc_Absyn_Neq){
struct _tuple15 _tmp919;return(_tmp919.f1=f,((_tmp919.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp919)));}else{struct _tuple15 _tmp91A;return(_tmp91A.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp91A.f2=f,_tmp91A)));}_LL2AE:;_LL2AF: goto _LL29B;_LL29B:;}{struct _tuple0
_tmp91B;struct _tuple0 _tmp510=(_tmp91B.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4CD->topt))->v),((_tmp91B.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4CE->topt))->v),_tmp91B)));void*_tmp511;enum Cyc_Absyn_Sign
_tmp513;void*_tmp514;enum Cyc_Absyn_Sign _tmp516;void*_tmp517;void*_tmp519;_LL2B9:
_tmp511=_tmp510.f1;{struct Cyc_Absyn_IntType_struct*_tmp512=(struct Cyc_Absyn_IntType_struct*)
_tmp511;if(_tmp512->tag != 6)goto _LL2BB;else{_tmp513=_tmp512->f1;if(_tmp513 != Cyc_Absyn_Unsigned)
goto _LL2BB;}};_LL2BA: goto _LL2BC;_LL2BB: _tmp514=_tmp510.f2;{struct Cyc_Absyn_IntType_struct*
_tmp515=(struct Cyc_Absyn_IntType_struct*)_tmp514;if(_tmp515->tag != 6)goto _LL2BD;
else{_tmp516=_tmp515->f1;if(_tmp516 != Cyc_Absyn_Unsigned)goto _LL2BD;}};_LL2BC:
goto _LL2BE;_LL2BD: _tmp517=_tmp510.f1;{struct Cyc_Absyn_TagType_struct*_tmp518=(
struct Cyc_Absyn_TagType_struct*)_tmp517;if(_tmp518->tag != 20)goto _LL2BF;};_LL2BE:
goto _LL2C0;_LL2BF: _tmp519=_tmp510.f2;{struct Cyc_Absyn_TagType_struct*_tmp51A=(
struct Cyc_Absyn_TagType_struct*)_tmp519;if(_tmp51A->tag != 20)goto _LL2C1;};_LL2C0:
goto _LL2B8;_LL2C1:;_LL2C2: {struct _tuple15 _tmp91C;return(_tmp91C.f1=f,((_tmp91C.f2=
f,_tmp91C)));}_LL2B8:;}switch(_tmp4A2){case Cyc_Absyn_Eq: _LL2C3: goto _LL2C4;case
Cyc_Absyn_Neq: _LL2C4: goto _LL2C5;case Cyc_Absyn_Gt: _LL2C5: goto _LL2C6;case Cyc_Absyn_Gte:
_LL2C6: {struct _tuple15 _tmp91D;return(_tmp91D.f1=f,((_tmp91D.f2=f,_tmp91D)));}
case Cyc_Absyn_Lt: _LL2C7: {union Cyc_CfFlowInfo_FlowInfo _tmp51D=f;union Cyc_CfFlowInfo_FlowInfo
_tmp51E=f;{union Cyc_CfFlowInfo_FlowInfo _tmp51F=_tmp51D;int _tmp520;struct _tuple12
_tmp521;struct Cyc_Dict_Dict _tmp522;_LL2CA: if((_tmp51F.BottomFL).tag != 1)goto
_LL2CC;_tmp520=(int)(_tmp51F.BottomFL).val;_LL2CB: {struct Cyc_Core_Impossible_struct
_tmp923;const char*_tmp922;struct Cyc_Core_Impossible_struct*_tmp921;(int)_throw((
void*)((_tmp921=_cycalloc(sizeof(*_tmp921)),((_tmp921[0]=((_tmp923.tag=Cyc_Core_Impossible,((
_tmp923.f1=((_tmp922="anal_test, Lt",_tag_dyneither(_tmp922,sizeof(char),14))),
_tmp923)))),_tmp921)))));}_LL2CC: if((_tmp51F.ReachableFL).tag != 2)goto _LL2C9;
_tmp521=(struct _tuple12)(_tmp51F.ReachableFL).val;_tmp522=_tmp521.f1;_LL2CD:
_tmp4CA=_tmp522;_LL2C9:;}{void*_tmp526=_tmp4CD->r;void*_tmp528;struct Cyc_Absyn_Vardecl*
_tmp52A;void*_tmp52C;struct Cyc_Absyn_Vardecl*_tmp52E;void*_tmp530;struct Cyc_Absyn_Vardecl*
_tmp532;void*_tmp534;struct Cyc_Absyn_Vardecl*_tmp536;_LL2CF:{struct Cyc_Absyn_Var_e_struct*
_tmp527=(struct Cyc_Absyn_Var_e_struct*)_tmp526;if(_tmp527->tag != 1)goto _LL2D1;
else{_tmp528=(void*)_tmp527->f2;{struct Cyc_Absyn_Global_b_struct*_tmp529=(struct
Cyc_Absyn_Global_b_struct*)_tmp528;if(_tmp529->tag != 1)goto _LL2D1;else{_tmp52A=
_tmp529->f1;}};}}if(!(!_tmp52A->escapes))goto _LL2D1;_LL2D0: _tmp52E=_tmp52A;goto
_LL2D2;_LL2D1:{struct Cyc_Absyn_Var_e_struct*_tmp52B=(struct Cyc_Absyn_Var_e_struct*)
_tmp526;if(_tmp52B->tag != 1)goto _LL2D3;else{_tmp52C=(void*)_tmp52B->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp52D=(struct Cyc_Absyn_Local_b_struct*)_tmp52C;if(
_tmp52D->tag != 4)goto _LL2D3;else{_tmp52E=_tmp52D->f1;}};}}if(!(!_tmp52E->escapes))
goto _LL2D3;_LL2D2: _tmp532=_tmp52E;goto _LL2D4;_LL2D3:{struct Cyc_Absyn_Var_e_struct*
_tmp52F=(struct Cyc_Absyn_Var_e_struct*)_tmp526;if(_tmp52F->tag != 1)goto _LL2D5;
else{_tmp530=(void*)_tmp52F->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp531=(struct Cyc_Absyn_Pat_b_struct*)
_tmp530;if(_tmp531->tag != 5)goto _LL2D5;else{_tmp532=_tmp531->f1;}};}}if(!(!
_tmp532->escapes))goto _LL2D5;_LL2D4: _tmp536=_tmp532;goto _LL2D6;_LL2D5:{struct Cyc_Absyn_Var_e_struct*
_tmp533=(struct Cyc_Absyn_Var_e_struct*)_tmp526;if(_tmp533->tag != 1)goto _LL2D7;
else{_tmp534=(void*)_tmp533->f2;{struct Cyc_Absyn_Param_b_struct*_tmp535=(struct
Cyc_Absyn_Param_b_struct*)_tmp534;if(_tmp535->tag != 3)goto _LL2D7;else{_tmp536=
_tmp535->f1;}};}}if(!(!_tmp536->escapes))goto _LL2D7;_LL2D6: {void*_tmp537=
_tmp4CE->r;void*_tmp539;struct Cyc_Absyn_Vardecl*_tmp53B;void*_tmp53D;struct Cyc_Absyn_Vardecl*
_tmp53F;void*_tmp541;struct Cyc_Absyn_Vardecl*_tmp543;void*_tmp545;struct Cyc_Absyn_Vardecl*
_tmp547;union Cyc_Absyn_Cnst _tmp549;struct _tuple6 _tmp54A;int _tmp54B;struct Cyc_Absyn_Exp*
_tmp54D;struct Cyc_Absyn_Exp _tmp54E;void*_tmp54F;union Cyc_Absyn_Cnst _tmp551;
struct _tuple6 _tmp552;int _tmp553;enum Cyc_Absyn_Primop _tmp555;struct Cyc_List_List*
_tmp556;struct Cyc_List_List _tmp557;struct Cyc_Absyn_Exp*_tmp558;_LL2DA:{struct Cyc_Absyn_Var_e_struct*
_tmp538=(struct Cyc_Absyn_Var_e_struct*)_tmp537;if(_tmp538->tag != 1)goto _LL2DC;
else{_tmp539=(void*)_tmp538->f2;{struct Cyc_Absyn_Global_b_struct*_tmp53A=(struct
Cyc_Absyn_Global_b_struct*)_tmp539;if(_tmp53A->tag != 1)goto _LL2DC;else{_tmp53B=
_tmp53A->f1;}};}}if(!(!_tmp53B->escapes))goto _LL2DC;_LL2DB: _tmp53F=_tmp53B;goto
_LL2DD;_LL2DC:{struct Cyc_Absyn_Var_e_struct*_tmp53C=(struct Cyc_Absyn_Var_e_struct*)
_tmp537;if(_tmp53C->tag != 1)goto _LL2DE;else{_tmp53D=(void*)_tmp53C->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp53E=(struct Cyc_Absyn_Local_b_struct*)_tmp53D;if(
_tmp53E->tag != 4)goto _LL2DE;else{_tmp53F=_tmp53E->f1;}};}}if(!(!_tmp53F->escapes))
goto _LL2DE;_LL2DD: _tmp543=_tmp53F;goto _LL2DF;_LL2DE:{struct Cyc_Absyn_Var_e_struct*
_tmp540=(struct Cyc_Absyn_Var_e_struct*)_tmp537;if(_tmp540->tag != 1)goto _LL2E0;
else{_tmp541=(void*)_tmp540->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp542=(struct Cyc_Absyn_Pat_b_struct*)
_tmp541;if(_tmp542->tag != 5)goto _LL2E0;else{_tmp543=_tmp542->f1;}};}}if(!(!
_tmp543->escapes))goto _LL2E0;_LL2DF: _tmp547=_tmp543;goto _LL2E1;_LL2E0:{struct Cyc_Absyn_Var_e_struct*
_tmp544=(struct Cyc_Absyn_Var_e_struct*)_tmp537;if(_tmp544->tag != 1)goto _LL2E2;
else{_tmp545=(void*)_tmp544->f2;{struct Cyc_Absyn_Param_b_struct*_tmp546=(struct
Cyc_Absyn_Param_b_struct*)_tmp545;if(_tmp546->tag != 3)goto _LL2E2;else{_tmp547=
_tmp546->f1;}};}}if(!(!_tmp547->escapes))goto _LL2E2;_LL2E1: {struct _RegionHandle*
_tmp559=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp926;struct Cyc_List_List*
_tmp925;struct Cyc_List_List*_tmp55A=(_tmp925=_region_malloc(_tmp559,sizeof(*
_tmp925)),((_tmp925->hd=((_tmp926=_region_malloc(_tmp559,sizeof(*_tmp926)),((
_tmp926->vd=_tmp536,((_tmp926->rop=Cyc_CfFlowInfo_LessVar(_tmp547,_tmp547->type),
_tmp926)))))),((_tmp925->tl=_tmp4CB,_tmp925)))));struct _tuple15 _tmp927;return(
_tmp927.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4CA,_tmp55A),((_tmp927.f2=_tmp51E,
_tmp927)));}_LL2E2: {struct Cyc_Absyn_Const_e_struct*_tmp548=(struct Cyc_Absyn_Const_e_struct*)
_tmp537;if(_tmp548->tag != 0)goto _LL2E4;else{_tmp549=_tmp548->f1;if((_tmp549.Int_c).tag
!= 4)goto _LL2E4;_tmp54A=(struct _tuple6)(_tmp549.Int_c).val;_tmp54B=_tmp54A.f2;}}
_LL2E3: _tmp553=_tmp54B;goto _LL2E5;_LL2E4: {struct Cyc_Absyn_Cast_e_struct*_tmp54C=(
struct Cyc_Absyn_Cast_e_struct*)_tmp537;if(_tmp54C->tag != 15)goto _LL2E6;else{
_tmp54D=_tmp54C->f2;_tmp54E=*_tmp54D;_tmp54F=_tmp54E.r;{struct Cyc_Absyn_Const_e_struct*
_tmp550=(struct Cyc_Absyn_Const_e_struct*)_tmp54F;if(_tmp550->tag != 0)goto _LL2E6;
else{_tmp551=_tmp550->f1;if((_tmp551.Int_c).tag != 4)goto _LL2E6;_tmp552=(struct
_tuple6)(_tmp551.Int_c).val;_tmp553=_tmp552.f2;}};}}_LL2E5: {struct _RegionHandle*
_tmp55E=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp92A;struct Cyc_List_List*
_tmp929;struct Cyc_List_List*_tmp55F=(_tmp929=_region_malloc(_tmp55E,sizeof(*
_tmp929)),((_tmp929->hd=((_tmp92A=_region_malloc(_tmp55E,sizeof(*_tmp92A)),((
_tmp92A->vd=_tmp536,((_tmp92A->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp553),_tmp92A)))))),((_tmp929->tl=_tmp4CB,_tmp929)))));struct _tuple15 _tmp92B;
return(_tmp92B.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4CA,_tmp55F),((_tmp92B.f2=
_tmp51E,_tmp92B)));}_LL2E6: {struct Cyc_Absyn_Primop_e_struct*_tmp554=(struct Cyc_Absyn_Primop_e_struct*)
_tmp537;if(_tmp554->tag != 3)goto _LL2E8;else{_tmp555=_tmp554->f1;_tmp556=_tmp554->f2;
if(_tmp556 == 0)goto _LL2E8;_tmp557=*_tmp556;_tmp558=(struct Cyc_Absyn_Exp*)_tmp557.hd;}}
_LL2E7: {void*_tmp563=_tmp558->r;void*_tmp565;struct Cyc_Absyn_Vardecl*_tmp567;
void*_tmp569;struct Cyc_Absyn_Vardecl*_tmp56B;void*_tmp56D;struct Cyc_Absyn_Vardecl*
_tmp56F;void*_tmp571;struct Cyc_Absyn_Vardecl*_tmp573;_LL2EB:{struct Cyc_Absyn_Var_e_struct*
_tmp564=(struct Cyc_Absyn_Var_e_struct*)_tmp563;if(_tmp564->tag != 1)goto _LL2ED;
else{_tmp565=(void*)_tmp564->f2;{struct Cyc_Absyn_Global_b_struct*_tmp566=(struct
Cyc_Absyn_Global_b_struct*)_tmp565;if(_tmp566->tag != 1)goto _LL2ED;else{_tmp567=
_tmp566->f1;}};}}if(!(!_tmp567->escapes))goto _LL2ED;_LL2EC: _tmp56B=_tmp567;goto
_LL2EE;_LL2ED:{struct Cyc_Absyn_Var_e_struct*_tmp568=(struct Cyc_Absyn_Var_e_struct*)
_tmp563;if(_tmp568->tag != 1)goto _LL2EF;else{_tmp569=(void*)_tmp568->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp56A=(struct Cyc_Absyn_Local_b_struct*)_tmp569;if(
_tmp56A->tag != 4)goto _LL2EF;else{_tmp56B=_tmp56A->f1;}};}}if(!(!_tmp56B->escapes))
goto _LL2EF;_LL2EE: _tmp56F=_tmp56B;goto _LL2F0;_LL2EF:{struct Cyc_Absyn_Var_e_struct*
_tmp56C=(struct Cyc_Absyn_Var_e_struct*)_tmp563;if(_tmp56C->tag != 1)goto _LL2F1;
else{_tmp56D=(void*)_tmp56C->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp56E=(struct Cyc_Absyn_Pat_b_struct*)
_tmp56D;if(_tmp56E->tag != 5)goto _LL2F1;else{_tmp56F=_tmp56E->f1;}};}}if(!(!
_tmp56F->escapes))goto _LL2F1;_LL2F0: _tmp573=_tmp56F;goto _LL2F2;_LL2F1:{struct Cyc_Absyn_Var_e_struct*
_tmp570=(struct Cyc_Absyn_Var_e_struct*)_tmp563;if(_tmp570->tag != 1)goto _LL2F3;
else{_tmp571=(void*)_tmp570->f2;{struct Cyc_Absyn_Param_b_struct*_tmp572=(struct
Cyc_Absyn_Param_b_struct*)_tmp571;if(_tmp572->tag != 3)goto _LL2F3;else{_tmp573=
_tmp572->f1;}};}}if(!(!_tmp573->escapes))goto _LL2F3;_LL2F2: {struct _RegionHandle*
_tmp574=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp92E;struct Cyc_List_List*
_tmp92D;struct Cyc_List_List*_tmp575=(_tmp92D=_region_malloc(_tmp574,sizeof(*
_tmp92D)),((_tmp92D->hd=((_tmp92E=_region_malloc(_tmp574,sizeof(*_tmp92E)),((
_tmp92E->vd=_tmp536,((_tmp92E->rop=Cyc_CfFlowInfo_LessNumelts(_tmp573),_tmp92E)))))),((
_tmp92D->tl=_tmp4CB,_tmp92D)))));struct _tuple15 _tmp92F;return(_tmp92F.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4CA,_tmp575),((_tmp92F.f2=_tmp51E,_tmp92F)));}_LL2F3:;_LL2F4: {struct
_tuple15 _tmp930;return(_tmp930.f1=_tmp51D,((_tmp930.f2=_tmp51E,_tmp930)));}
_LL2EA:;}_LL2E8:;_LL2E9: {struct _tuple15 _tmp931;return(_tmp931.f1=_tmp51D,((
_tmp931.f2=_tmp51E,_tmp931)));}_LL2D9:;}_LL2D7:;_LL2D8: {struct _tuple15 _tmp932;
return(_tmp932.f1=_tmp51D,((_tmp932.f2=_tmp51E,_tmp932)));}_LL2CE:;};}case Cyc_Absyn_Lte:
_LL2C8: {union Cyc_CfFlowInfo_FlowInfo _tmp57C=f;union Cyc_CfFlowInfo_FlowInfo
_tmp57D=f;{union Cyc_CfFlowInfo_FlowInfo _tmp57E=_tmp57C;int _tmp57F;struct _tuple12
_tmp580;struct Cyc_Dict_Dict _tmp581;_LL2F7: if((_tmp57E.BottomFL).tag != 1)goto
_LL2F9;_tmp57F=(int)(_tmp57E.BottomFL).val;_LL2F8: {struct Cyc_Core_Impossible_struct
_tmp938;const char*_tmp937;struct Cyc_Core_Impossible_struct*_tmp936;(int)_throw((
void*)((_tmp936=_cycalloc(sizeof(*_tmp936)),((_tmp936[0]=((_tmp938.tag=Cyc_Core_Impossible,((
_tmp938.f1=((_tmp937="anal_test, Lte",_tag_dyneither(_tmp937,sizeof(char),15))),
_tmp938)))),_tmp936)))));}_LL2F9: if((_tmp57E.ReachableFL).tag != 2)goto _LL2F6;
_tmp580=(struct _tuple12)(_tmp57E.ReachableFL).val;_tmp581=_tmp580.f1;_LL2FA:
_tmp4CA=_tmp581;_LL2F6:;}{void*_tmp585=_tmp4CD->r;void*_tmp587;struct Cyc_Absyn_Vardecl*
_tmp589;void*_tmp58B;struct Cyc_Absyn_Vardecl*_tmp58D;void*_tmp58F;struct Cyc_Absyn_Vardecl*
_tmp591;void*_tmp593;struct Cyc_Absyn_Vardecl*_tmp595;_LL2FC:{struct Cyc_Absyn_Var_e_struct*
_tmp586=(struct Cyc_Absyn_Var_e_struct*)_tmp585;if(_tmp586->tag != 1)goto _LL2FE;
else{_tmp587=(void*)_tmp586->f2;{struct Cyc_Absyn_Global_b_struct*_tmp588=(struct
Cyc_Absyn_Global_b_struct*)_tmp587;if(_tmp588->tag != 1)goto _LL2FE;else{_tmp589=
_tmp588->f1;}};}}if(!(!_tmp589->escapes))goto _LL2FE;_LL2FD: _tmp58D=_tmp589;goto
_LL2FF;_LL2FE:{struct Cyc_Absyn_Var_e_struct*_tmp58A=(struct Cyc_Absyn_Var_e_struct*)
_tmp585;if(_tmp58A->tag != 1)goto _LL300;else{_tmp58B=(void*)_tmp58A->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp58C=(struct Cyc_Absyn_Local_b_struct*)_tmp58B;if(
_tmp58C->tag != 4)goto _LL300;else{_tmp58D=_tmp58C->f1;}};}}if(!(!_tmp58D->escapes))
goto _LL300;_LL2FF: _tmp591=_tmp58D;goto _LL301;_LL300:{struct Cyc_Absyn_Var_e_struct*
_tmp58E=(struct Cyc_Absyn_Var_e_struct*)_tmp585;if(_tmp58E->tag != 1)goto _LL302;
else{_tmp58F=(void*)_tmp58E->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp590=(struct Cyc_Absyn_Pat_b_struct*)
_tmp58F;if(_tmp590->tag != 5)goto _LL302;else{_tmp591=_tmp590->f1;}};}}if(!(!
_tmp591->escapes))goto _LL302;_LL301: _tmp595=_tmp591;goto _LL303;_LL302:{struct Cyc_Absyn_Var_e_struct*
_tmp592=(struct Cyc_Absyn_Var_e_struct*)_tmp585;if(_tmp592->tag != 1)goto _LL304;
else{_tmp593=(void*)_tmp592->f2;{struct Cyc_Absyn_Param_b_struct*_tmp594=(struct
Cyc_Absyn_Param_b_struct*)_tmp593;if(_tmp594->tag != 3)goto _LL304;else{_tmp595=
_tmp594->f1;}};}}if(!(!_tmp595->escapes))goto _LL304;_LL303: {void*_tmp596=
_tmp4CE->r;union Cyc_Absyn_Cnst _tmp598;struct _tuple6 _tmp599;int _tmp59A;struct Cyc_Absyn_Exp*
_tmp59C;struct Cyc_Absyn_Exp _tmp59D;void*_tmp59E;union Cyc_Absyn_Cnst _tmp5A0;
struct _tuple6 _tmp5A1;int _tmp5A2;enum Cyc_Absyn_Primop _tmp5A4;struct Cyc_List_List*
_tmp5A5;struct Cyc_List_List _tmp5A6;struct Cyc_Absyn_Exp*_tmp5A7;_LL307: {struct
Cyc_Absyn_Const_e_struct*_tmp597=(struct Cyc_Absyn_Const_e_struct*)_tmp596;if(
_tmp597->tag != 0)goto _LL309;else{_tmp598=_tmp597->f1;if((_tmp598.Int_c).tag != 4)
goto _LL309;_tmp599=(struct _tuple6)(_tmp598.Int_c).val;_tmp59A=_tmp599.f2;}}
_LL308: _tmp5A2=_tmp59A;goto _LL30A;_LL309: {struct Cyc_Absyn_Cast_e_struct*_tmp59B=(
struct Cyc_Absyn_Cast_e_struct*)_tmp596;if(_tmp59B->tag != 15)goto _LL30B;else{
_tmp59C=_tmp59B->f2;_tmp59D=*_tmp59C;_tmp59E=_tmp59D.r;{struct Cyc_Absyn_Const_e_struct*
_tmp59F=(struct Cyc_Absyn_Const_e_struct*)_tmp59E;if(_tmp59F->tag != 0)goto _LL30B;
else{_tmp5A0=_tmp59F->f1;if((_tmp5A0.Int_c).tag != 4)goto _LL30B;_tmp5A1=(struct
_tuple6)(_tmp5A0.Int_c).val;_tmp5A2=_tmp5A1.f2;}};}}_LL30A: {struct _RegionHandle*
_tmp5A8=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp93B;struct Cyc_List_List*
_tmp93A;struct Cyc_List_List*_tmp5A9=(_tmp93A=_region_malloc(_tmp5A8,sizeof(*
_tmp93A)),((_tmp93A->hd=((_tmp93B=_region_malloc(_tmp5A8,sizeof(*_tmp93B)),((
_tmp93B->vd=_tmp595,((_tmp93B->rop=Cyc_CfFlowInfo_LessEqConst((unsigned int)
_tmp5A2),_tmp93B)))))),((_tmp93A->tl=_tmp4CB,_tmp93A)))));struct _tuple15 _tmp93C;
return(_tmp93C.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4CA,_tmp5A9),((_tmp93C.f2=
_tmp57D,_tmp93C)));}_LL30B: {struct Cyc_Absyn_Primop_e_struct*_tmp5A3=(struct Cyc_Absyn_Primop_e_struct*)
_tmp596;if(_tmp5A3->tag != 3)goto _LL30D;else{_tmp5A4=_tmp5A3->f1;_tmp5A5=_tmp5A3->f2;
if(_tmp5A5 == 0)goto _LL30D;_tmp5A6=*_tmp5A5;_tmp5A7=(struct Cyc_Absyn_Exp*)_tmp5A6.hd;}}
_LL30C: {void*_tmp5AD=_tmp5A7->r;void*_tmp5AF;struct Cyc_Absyn_Vardecl*_tmp5B1;
void*_tmp5B3;struct Cyc_Absyn_Vardecl*_tmp5B5;void*_tmp5B7;struct Cyc_Absyn_Vardecl*
_tmp5B9;void*_tmp5BB;struct Cyc_Absyn_Vardecl*_tmp5BD;_LL310:{struct Cyc_Absyn_Var_e_struct*
_tmp5AE=(struct Cyc_Absyn_Var_e_struct*)_tmp5AD;if(_tmp5AE->tag != 1)goto _LL312;
else{_tmp5AF=(void*)_tmp5AE->f2;{struct Cyc_Absyn_Global_b_struct*_tmp5B0=(struct
Cyc_Absyn_Global_b_struct*)_tmp5AF;if(_tmp5B0->tag != 1)goto _LL312;else{_tmp5B1=
_tmp5B0->f1;}};}}if(!(!_tmp5B1->escapes))goto _LL312;_LL311: _tmp5B5=_tmp5B1;goto
_LL313;_LL312:{struct Cyc_Absyn_Var_e_struct*_tmp5B2=(struct Cyc_Absyn_Var_e_struct*)
_tmp5AD;if(_tmp5B2->tag != 1)goto _LL314;else{_tmp5B3=(void*)_tmp5B2->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp5B4=(struct Cyc_Absyn_Local_b_struct*)_tmp5B3;if(
_tmp5B4->tag != 4)goto _LL314;else{_tmp5B5=_tmp5B4->f1;}};}}if(!(!_tmp5B5->escapes))
goto _LL314;_LL313: _tmp5B9=_tmp5B5;goto _LL315;_LL314:{struct Cyc_Absyn_Var_e_struct*
_tmp5B6=(struct Cyc_Absyn_Var_e_struct*)_tmp5AD;if(_tmp5B6->tag != 1)goto _LL316;
else{_tmp5B7=(void*)_tmp5B6->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp5B8=(struct Cyc_Absyn_Pat_b_struct*)
_tmp5B7;if(_tmp5B8->tag != 5)goto _LL316;else{_tmp5B9=_tmp5B8->f1;}};}}if(!(!
_tmp5B9->escapes))goto _LL316;_LL315: _tmp5BD=_tmp5B9;goto _LL317;_LL316:{struct Cyc_Absyn_Var_e_struct*
_tmp5BA=(struct Cyc_Absyn_Var_e_struct*)_tmp5AD;if(_tmp5BA->tag != 1)goto _LL318;
else{_tmp5BB=(void*)_tmp5BA->f2;{struct Cyc_Absyn_Param_b_struct*_tmp5BC=(struct
Cyc_Absyn_Param_b_struct*)_tmp5BB;if(_tmp5BC->tag != 3)goto _LL318;else{_tmp5BD=
_tmp5BC->f1;}};}}if(!(!_tmp5BD->escapes))goto _LL318;_LL317: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp5BE=env->fenv;struct Cyc_CfFlowInfo_Reln*_tmp93F;struct Cyc_List_List*_tmp93E;
struct Cyc_List_List*_tmp5BF=(_tmp93E=_region_malloc(_tmp5BE->r,sizeof(*_tmp93E)),((
_tmp93E->hd=((_tmp93F=_region_malloc(_tmp5BE->r,sizeof(*_tmp93F)),((_tmp93F->vd=
_tmp595,((_tmp93F->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp5BD),_tmp93F)))))),((
_tmp93E->tl=_tmp4CB,_tmp93E)))));struct _tuple15 _tmp940;return(_tmp940.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4CA,_tmp5BF),((_tmp940.f2=_tmp57D,_tmp940)));}_LL318:;_LL319: {struct
_tuple15 _tmp941;return(_tmp941.f1=_tmp57C,((_tmp941.f2=_tmp57D,_tmp941)));}
_LL30F:;}_LL30D:;_LL30E: {struct _tuple15 _tmp942;return(_tmp942.f1=_tmp57C,((
_tmp942.f2=_tmp57D,_tmp942)));}_LL306:;}_LL304:;_LL305: {struct _tuple15 _tmp943;
return(_tmp943.f1=_tmp57C,((_tmp943.f2=_tmp57D,_tmp943)));}_LL2FB:;};}default:
_LL2F5: {struct _tuple15 _tmp944;return(_tmp944.f1=f,((_tmp944.f2=f,_tmp944)));}}}
_LL296:;};}_LL294:;_LL295: goto _LL287;_LL287:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp5C8;void*_tmp5C9;struct _tuple13 _tmp5C7=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,e);_tmp5C8=_tmp5C7.f1;_tmp5C9=_tmp5C7.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp5CA=_tmp5C8;int _tmp5CB;struct _tuple12 _tmp5CC;struct Cyc_Dict_Dict _tmp5CD;
_LL31C: if((_tmp5CA.BottomFL).tag != 1)goto _LL31E;_tmp5CB=(int)(_tmp5CA.BottomFL).val;
_LL31D: {struct _tuple15 _tmp945;return(_tmp945.f1=_tmp5C8,((_tmp945.f2=_tmp5C8,
_tmp945)));}_LL31E: if((_tmp5CA.ReachableFL).tag != 2)goto _LL31B;_tmp5CC=(struct
_tuple12)(_tmp5CA.ReachableFL).val;_tmp5CD=_tmp5CC.f1;_LL31F: {void*_tmp5CF=
_tmp5C9;enum Cyc_CfFlowInfo_InitLevel _tmp5D5;enum Cyc_CfFlowInfo_InitLevel
_tmp5D7;enum Cyc_CfFlowInfo_InitLevel _tmp5DA;_LL321: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5D0=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5CF;if(_tmp5D0->tag != 0)goto
_LL323;}_LL322: {struct _tuple15 _tmp946;return(_tmp946.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp946.f2=_tmp5C8,_tmp946)));}_LL323: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp5D1=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp5CF;if(_tmp5D1->tag != 2)
goto _LL325;}_LL324: goto _LL326;_LL325: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp5D2=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp5CF;if(_tmp5D2->tag != 1)
goto _LL327;}_LL326: goto _LL328;_LL327: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp5D3=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp5CF;if(_tmp5D3->tag != 5)goto
_LL329;}_LL328: {struct _tuple15 _tmp947;return(_tmp947.f1=_tmp5C8,((_tmp947.f2=
Cyc_CfFlowInfo_BottomFL(),_tmp947)));}_LL329: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp5D4=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5CF;if(_tmp5D4->tag != 3)goto
_LL32B;else{_tmp5D5=_tmp5D4->f1;if(_tmp5D5 != Cyc_CfFlowInfo_NoneIL)goto _LL32B;}}
_LL32A: goto _LL32C;_LL32B: {struct Cyc_CfFlowInfo_Esc_struct*_tmp5D6=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp5CF;if(_tmp5D6->tag != 4)goto _LL32D;else{_tmp5D7=_tmp5D6->f1;if(_tmp5D7 != Cyc_CfFlowInfo_NoneIL)
goto _LL32D;}}_LL32C: goto _LL32E;_LL32D: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp5D8=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp5CF;if(_tmp5D8->tag != 7)goto
_LL32F;}_LL32E:{const char*_tmp94A;void*_tmp949;(_tmp949=0,Cyc_Tcutil_terr(e->loc,((
_tmp94A="expression may not be initialized",_tag_dyneither(_tmp94A,sizeof(char),
34))),_tag_dyneither(_tmp949,sizeof(void*),0)));}{struct _tuple15 _tmp94B;return(
_tmp94B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp94B.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp94B)));};_LL32F: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5D9=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp5CF;if(_tmp5D9->tag != 3)goto _LL331;else{_tmp5DA=_tmp5D9->f1;}}_LL330: return
Cyc_NewControlFlow_splitzero(env,inflow,_tmp5C8,e,_tmp5DA);_LL331: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp5DB=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp5CF;if(_tmp5DB->tag != 4)goto _LL333;}
_LL332: {struct _tuple15 _tmp94C;return(_tmp94C.f1=_tmp5C8,((_tmp94C.f2=_tmp5C8,
_tmp94C)));}_LL333: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5DC=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp5CF;if(_tmp5DC->tag != 6)goto _LL320;}_LL334: {struct Cyc_Core_Impossible_struct
_tmp952;const char*_tmp951;struct Cyc_Core_Impossible_struct*_tmp950;(int)_throw((
void*)((_tmp950=_cycalloc(sizeof(*_tmp950)),((_tmp950[0]=((_tmp952.tag=Cyc_Core_Impossible,((
_tmp952.f1=((_tmp951="anal_test",_tag_dyneither(_tmp951,sizeof(char),10))),
_tmp952)))),_tmp950)))));}_LL320:;}_LL31B:;};};}struct _tuple22{struct Cyc_Position_Segment*
f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple22*ckenv,void*root,void*rval){struct _tuple22 _tmp5E7;struct Cyc_Position_Segment*
_tmp5E8;struct Cyc_NewControlFlow_AnalEnv*_tmp5E9;struct Cyc_Dict_Dict _tmp5EA;
struct _tuple22*_tmp5E6=ckenv;_tmp5E7=*_tmp5E6;_tmp5E8=_tmp5E7.f1;_tmp5E9=_tmp5E7.f2;
_tmp5EA=_tmp5E7.f3;{void*_tmp5EB=root;struct Cyc_Absyn_Vardecl*_tmp5ED;_LL336: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp5EC=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp5EB;if(_tmp5EC->tag != 0)goto _LL338;else{_tmp5ED=_tmp5EC->f1;}}_LL337: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp5E9->fenv)->r,_tmp5ED->type)){void*_tmp5EE=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp5F2;_LL33B: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp5EF=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp5EE;if(_tmp5EF->tag != 7)goto _LL33D;}_LL33C: goto _LL33E;_LL33D: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5F0=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5EE;if(_tmp5F0->tag != 0)goto
_LL33F;}_LL33E: goto _LL340;_LL33F: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5F1=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5EE;if(_tmp5F1->tag != 3)goto _LL341;
else{_tmp5F2=_tmp5F1->f1;if(_tmp5F2 != Cyc_CfFlowInfo_NoneIL)goto _LL341;}}_LL340:
goto _LL33A;_LL341:;_LL342:{const char*_tmp956;void*_tmp955[1];struct Cyc_String_pa_struct
_tmp954;(_tmp954.tag=0,((_tmp954.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp5ED->name)),((_tmp955[0]=& _tmp954,Cyc_Tcutil_warn(
_tmp5E8,((_tmp956="unique pointers reachable from %s may become unreachable",
_tag_dyneither(_tmp956,sizeof(char),57))),_tag_dyneither(_tmp955,sizeof(void*),1)))))));}
goto _LL33A;_LL33A:;}goto _LL335;_LL338:;_LL339: goto _LL335;_LL335:;};}static void
Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo flow){union Cyc_CfFlowInfo_FlowInfo _tmp5F6=flow;
int _tmp5F7;struct _tuple12 _tmp5F8;struct Cyc_Dict_Dict _tmp5F9;_LL344: if((_tmp5F6.BottomFL).tag
!= 1)goto _LL346;_tmp5F7=(int)(_tmp5F6.BottomFL).val;_LL345: return;_LL346: if((
_tmp5F6.ReachableFL).tag != 2)goto _LL343;_tmp5F8=(struct _tuple12)(_tmp5F6.ReachableFL).val;
_tmp5F9=_tmp5F8.f1;_LL347:{struct Cyc_List_List*_tmp5FA=env->param_roots;for(0;
_tmp5FA != 0;_tmp5FA=_tmp5FA->tl){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp5F9,
Cyc_CfFlowInfo_lookup_place(_tmp5F9,(struct Cyc_CfFlowInfo_Place*)_tmp5FA->hd))!= 
Cyc_CfFlowInfo_AllIL){const char*_tmp959;void*_tmp958;(_tmp958=0,Cyc_Tcutil_terr(
loc,((_tmp959="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp959,sizeof(char),76))),_tag_dyneither(_tmp958,sizeof(void*),0)));}}}
if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple22 _tmp95A;struct _tuple22
_tmp5FD=(_tmp95A.f1=loc,((_tmp95A.f2=env,((_tmp95A.f3=_tmp5F9,_tmp95A)))));((
void(*)(void(*f)(struct _tuple22*,void*,void*),struct _tuple22*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp5FD,_tmp5F9);}
return;_LL343:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp5FF=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp601;struct Cyc_Core_Opt*_tmp602;
struct Cyc_Absyn_Exp*_tmp603;struct Cyc_Absyn_Stmt*_tmp604;struct Cyc_Position_Segment*
_tmp605;struct Cyc_Absyn_Switch_clause*_tmp600=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp601=*_tmp600;_tmp602=_tmp601.pat_vars;_tmp603=_tmp601.where_clause;
_tmp604=_tmp601.body;_tmp605=_tmp601.loc;{union Cyc_CfFlowInfo_FlowInfo
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp5FF,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp602))->v,_tmp5FF->unknown_all,_tmp605);union
Cyc_CfFlowInfo_FlowInfo clause_outflow;if(_tmp603 != 0){struct Cyc_Absyn_Exp*wexp=(
struct Cyc_Absyn_Exp*)_tmp603;union Cyc_CfFlowInfo_FlowInfo _tmp607;union Cyc_CfFlowInfo_FlowInfo
_tmp608;struct _tuple15 _tmp606=Cyc_NewControlFlow_anal_test(env,clause_inflow,
wexp);_tmp607=_tmp606.f1;_tmp608=_tmp606.f2;inflow=_tmp608;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp607,_tmp604);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp604);}{union Cyc_CfFlowInfo_FlowInfo _tmp609=clause_outflow;int
_tmp60A;_LL349: if((_tmp609.BottomFL).tag != 1)goto _LL34B;_tmp60A=(int)(_tmp609.BottomFL).val;
_LL34A: goto _LL348;_LL34B:;_LL34C: if(scs->tl == 0)return clause_outflow;else{if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){const char*
_tmp95D;void*_tmp95C;(_tmp95C=0,Cyc_Tcutil_terr(_tmp604->loc,((_tmp95D="switch clause may implicitly fallthru",
_tag_dyneither(_tmp95D,sizeof(char),38))),_tag_dyneither(_tmp95C,sizeof(void*),0)));}
else{const char*_tmp960;void*_tmp95F;(_tmp95F=0,Cyc_Tcutil_warn(_tmp604->loc,((
_tmp960="switch clause may implicitly fallthru",_tag_dyneither(_tmp960,sizeof(
char),38))),_tag_dyneither(_tmp95F,sizeof(void*),0)));}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL348;_LL348:;};};}return Cyc_CfFlowInfo_BottomFL();}struct
_tuple23{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;struct Cyc_Position_Segment*
f3;};static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple23*vdenv,
struct Cyc_Absyn_Vardecl*vd){struct _tuple23 _tmp610;struct Cyc_NewControlFlow_AnalEnv*
_tmp611;struct Cyc_Dict_Dict _tmp612;struct Cyc_Position_Segment*_tmp613;struct
_tuple23*_tmp60F=vdenv;_tmp610=*_tmp60F;_tmp611=_tmp610.f1;_tmp612=_tmp610.f2;
_tmp613=_tmp610.f3;if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp611->fenv)->r,
vd->type)){struct Cyc_CfFlowInfo_VarRoot_struct _tmp963;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp962;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp614=(_tmp962=_region_malloc((
_tmp611->fenv)->r,sizeof(*_tmp962)),((_tmp962[0]=((_tmp963.tag=0,((_tmp963.f1=vd,
_tmp963)))),_tmp962)));void*_tmp615=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
_tmp612,(void*)_tmp614);void*_tmp616=_tmp615;enum Cyc_CfFlowInfo_InitLevel
_tmp61A;_LL34E: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp617=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp616;if(_tmp617->tag != 7)goto _LL350;}_LL34F: goto _LL351;_LL350: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp618=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp616;if(_tmp618->tag != 0)goto
_LL352;}_LL351: goto _LL353;_LL352: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp619=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp616;if(_tmp619->tag != 3)goto _LL354;
else{_tmp61A=_tmp619->f1;if(_tmp61A != Cyc_CfFlowInfo_NoneIL)goto _LL354;}}_LL353:
goto _LL34D;_LL354:;_LL355:{const char*_tmp967;void*_tmp966[1];struct Cyc_String_pa_struct
_tmp965;(_tmp965.tag=0,((_tmp965.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp966[0]=& _tmp965,Cyc_Tcutil_warn(_tmp613,((
_tmp967="unique pointers may still exist after variable %s goes out of scope",
_tag_dyneither(_tmp967,sizeof(char),68))),_tag_dyneither(_tmp966,sizeof(void*),1)))))));}
goto _LL34D;_LL34D:;}}static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){{union Cyc_CfFlowInfo_FlowInfo
_tmp620=inflow;struct _tuple12 _tmp621;struct Cyc_Dict_Dict _tmp622;_LL357: if((
_tmp620.ReachableFL).tag != 2)goto _LL359;_tmp621=(struct _tuple12)(_tmp620.ReachableFL).val;
_tmp622=_tmp621.f1;_LL358: {struct _tuple23 _tmp968;struct _tuple23 _tmp623=(_tmp968.f1=
env,((_tmp968.f2=_tmp622,((_tmp968.f3=decl->loc,_tmp968)))));struct Cyc_CfFlowInfo_FlowEnv*
_tmp624=env->fenv;{void*_tmp625=decl->r;struct Cyc_Absyn_Vardecl*_tmp627;struct
Cyc_Core_Opt*_tmp629;struct Cyc_Core_Opt _tmp62A;struct Cyc_List_List*_tmp62B;
struct Cyc_List_List*_tmp62D;_LL35C: {struct Cyc_Absyn_Var_d_struct*_tmp626=(
struct Cyc_Absyn_Var_d_struct*)_tmp625;if(_tmp626->tag != 0)goto _LL35E;else{
_tmp627=_tmp626->f1;}}_LL35D: Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp623,
_tmp627);goto _LL35B;_LL35E: {struct Cyc_Absyn_Let_d_struct*_tmp628=(struct Cyc_Absyn_Let_d_struct*)
_tmp625;if(_tmp628->tag != 2)goto _LL360;else{_tmp629=_tmp628->f2;if(_tmp629 == 0)
goto _LL360;_tmp62A=*_tmp629;_tmp62B=(struct Cyc_List_List*)_tmp62A.v;}}_LL35F:
_tmp62D=_tmp62B;goto _LL361;_LL360: {struct Cyc_Absyn_Letv_d_struct*_tmp62C=(
struct Cyc_Absyn_Letv_d_struct*)_tmp625;if(_tmp62C->tag != 3)goto _LL362;else{
_tmp62D=_tmp62C->f1;}}_LL361:((void(*)(void(*f)(struct _tuple23*,struct Cyc_Absyn_Vardecl*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,&
_tmp623,_tmp62D);goto _LL35B;_LL362:;_LL363: goto _LL35B;_LL35B:;}goto _LL356;}
_LL359:;_LL35A: goto _LL356;_LL356:;}return;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp630;union Cyc_CfFlowInfo_FlowInfo*_tmp631;struct _tuple16 _tmp62F=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp630=_tmp62F.f1;_tmp631=_tmp62F.f2;inflow=*_tmp631;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp632=env->fenv;void*_tmp633=s->r;struct Cyc_Absyn_Exp*_tmp636;struct Cyc_Absyn_Exp*
_tmp638;struct Cyc_Absyn_Exp*_tmp63A;struct Cyc_Absyn_Stmt*_tmp63C;struct Cyc_Absyn_Stmt*
_tmp63D;struct Cyc_Absyn_Exp*_tmp63F;struct Cyc_Absyn_Stmt*_tmp640;struct Cyc_Absyn_Stmt*
_tmp641;struct _tuple9 _tmp643;struct Cyc_Absyn_Exp*_tmp644;struct Cyc_Absyn_Stmt*
_tmp645;struct Cyc_Absyn_Stmt*_tmp646;struct Cyc_Absyn_Stmt*_tmp648;struct _tuple9
_tmp649;struct Cyc_Absyn_Exp*_tmp64A;struct Cyc_Absyn_Stmt*_tmp64B;struct Cyc_Absyn_Exp*
_tmp64D;struct _tuple9 _tmp64E;struct Cyc_Absyn_Exp*_tmp64F;struct Cyc_Absyn_Stmt*
_tmp650;struct _tuple9 _tmp651;struct Cyc_Absyn_Exp*_tmp652;struct Cyc_Absyn_Stmt*
_tmp653;struct Cyc_Absyn_Stmt*_tmp654;struct Cyc_Absyn_Stmt*_tmp656;struct Cyc_List_List*
_tmp658;struct Cyc_Absyn_Switch_clause**_tmp659;struct Cyc_Absyn_Switch_clause*
_tmp65A;struct Cyc_Absyn_Stmt*_tmp65C;struct Cyc_Absyn_Stmt*_tmp65E;struct Cyc_Absyn_Stmt*
_tmp660;struct Cyc_Absyn_Exp*_tmp662;struct Cyc_List_List*_tmp663;struct Cyc_Absyn_Stmt*
_tmp665;struct Cyc_List_List*_tmp666;struct Cyc_Absyn_Decl*_tmp668;struct Cyc_Absyn_Decl
_tmp669;void*_tmp66A;struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_Absyn_Tvar*_tmp66D;
struct Cyc_Absyn_Vardecl*_tmp66E;struct Cyc_Absyn_Stmt*_tmp66F;struct Cyc_Absyn_Decl*
_tmp671;struct Cyc_Absyn_Stmt*_tmp672;struct Cyc_Absyn_Stmt*_tmp674;struct Cyc_Absyn_Exp*
_tmp676;_LL365: {struct Cyc_Absyn_Skip_s_struct*_tmp634=(struct Cyc_Absyn_Skip_s_struct*)
_tmp633;if(_tmp634->tag != 0)goto _LL367;}_LL366: return inflow;_LL367: {struct Cyc_Absyn_Return_s_struct*
_tmp635=(struct Cyc_Absyn_Return_s_struct*)_tmp633;if(_tmp635->tag != 3)goto _LL369;
else{_tmp636=_tmp635->f1;if(_tmp636 != 0)goto _LL369;}}_LL368: if(env->noreturn){
const char*_tmp96B;void*_tmp96A;(_tmp96A=0,Cyc_Tcutil_terr(s->loc,((_tmp96B="`noreturn' function might return",
_tag_dyneither(_tmp96B,sizeof(char),33))),_tag_dyneither(_tmp96A,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL369: {struct Cyc_Absyn_Return_s_struct*_tmp637=(struct Cyc_Absyn_Return_s_struct*)
_tmp633;if(_tmp637->tag != 3)goto _LL36B;else{_tmp638=_tmp637->f1;}}_LL36A: if(env->noreturn){
const char*_tmp96E;void*_tmp96D;(_tmp96D=0,Cyc_Tcutil_terr(s->loc,((_tmp96E="`noreturn' function might return",
_tag_dyneither(_tmp96E,sizeof(char),33))),_tag_dyneither(_tmp96D,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp67C;void*_tmp67D;struct _tuple13 _tmp67B=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp638));_tmp67C=_tmp67B.f1;
_tmp67D=_tmp67B.f2;_tmp67C=Cyc_NewControlFlow_use_Rval(env,_tmp638->loc,_tmp67C,
_tmp67D);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp67C);return Cyc_CfFlowInfo_BottomFL();};
_LL36B: {struct Cyc_Absyn_Exp_s_struct*_tmp639=(struct Cyc_Absyn_Exp_s_struct*)
_tmp633;if(_tmp639->tag != 1)goto _LL36D;else{_tmp63A=_tmp639->f1;}}_LL36C: return(
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp63A)).f1;_LL36D: {struct Cyc_Absyn_Seq_s_struct*
_tmp63B=(struct Cyc_Absyn_Seq_s_struct*)_tmp633;if(_tmp63B->tag != 2)goto _LL36F;
else{_tmp63C=_tmp63B->f1;_tmp63D=_tmp63B->f2;}}_LL36E: return Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp63C),_tmp63D);_LL36F: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp63E=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp633;if(_tmp63E->tag != 4)goto
_LL371;else{_tmp63F=_tmp63E->f1;_tmp640=_tmp63E->f2;_tmp641=_tmp63E->f3;}}_LL370: {
union Cyc_CfFlowInfo_FlowInfo _tmp67F;union Cyc_CfFlowInfo_FlowInfo _tmp680;struct
_tuple15 _tmp67E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp63F);_tmp67F=_tmp67E.f1;
_tmp680=_tmp67E.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp681=Cyc_NewControlFlow_anal_stmt(
env,_tmp680,_tmp641);union Cyc_CfFlowInfo_FlowInfo _tmp682=Cyc_NewControlFlow_anal_stmt(
env,_tmp67F,_tmp640);return Cyc_CfFlowInfo_join_flow(_tmp632,env->all_changed,
_tmp682,_tmp681);};}_LL371: {struct Cyc_Absyn_While_s_struct*_tmp642=(struct Cyc_Absyn_While_s_struct*)
_tmp633;if(_tmp642->tag != 5)goto _LL373;else{_tmp643=_tmp642->f1;_tmp644=_tmp643.f1;
_tmp645=_tmp643.f2;_tmp646=_tmp642->f2;}}_LL372: {union Cyc_CfFlowInfo_FlowInfo*
_tmp684;struct _tuple16 _tmp683=Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp645);_tmp684=_tmp683.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp685=*_tmp684;union
Cyc_CfFlowInfo_FlowInfo _tmp687;union Cyc_CfFlowInfo_FlowInfo _tmp688;struct
_tuple15 _tmp686=Cyc_NewControlFlow_anal_test(env,_tmp685,_tmp644);_tmp687=
_tmp686.f1;_tmp688=_tmp686.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp689=Cyc_NewControlFlow_anal_stmt(
env,_tmp687,_tmp646);Cyc_NewControlFlow_update_flow(env,_tmp645,_tmp689);return
_tmp688;};};}_LL373: {struct Cyc_Absyn_Do_s_struct*_tmp647=(struct Cyc_Absyn_Do_s_struct*)
_tmp633;if(_tmp647->tag != 14)goto _LL375;else{_tmp648=_tmp647->f1;_tmp649=_tmp647->f2;
_tmp64A=_tmp649.f1;_tmp64B=_tmp649.f2;}}_LL374: {union Cyc_CfFlowInfo_FlowInfo
_tmp68A=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp648);union Cyc_CfFlowInfo_FlowInfo*
_tmp68C;struct _tuple16 _tmp68B=Cyc_NewControlFlow_pre_stmt_check(env,_tmp68A,
_tmp64B);_tmp68C=_tmp68B.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp68D=*_tmp68C;union
Cyc_CfFlowInfo_FlowInfo _tmp68F;union Cyc_CfFlowInfo_FlowInfo _tmp690;struct
_tuple15 _tmp68E=Cyc_NewControlFlow_anal_test(env,_tmp68D,_tmp64A);_tmp68F=
_tmp68E.f1;_tmp690=_tmp68E.f2;Cyc_NewControlFlow_update_flow(env,_tmp648,_tmp68F);
return _tmp690;};}_LL375: {struct Cyc_Absyn_For_s_struct*_tmp64C=(struct Cyc_Absyn_For_s_struct*)
_tmp633;if(_tmp64C->tag != 9)goto _LL377;else{_tmp64D=_tmp64C->f1;_tmp64E=_tmp64C->f2;
_tmp64F=_tmp64E.f1;_tmp650=_tmp64E.f2;_tmp651=_tmp64C->f3;_tmp652=_tmp651.f1;
_tmp653=_tmp651.f2;_tmp654=_tmp64C->f4;}}_LL376: {union Cyc_CfFlowInfo_FlowInfo
_tmp691=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp64D)).f1;union Cyc_CfFlowInfo_FlowInfo*
_tmp693;struct _tuple16 _tmp692=Cyc_NewControlFlow_pre_stmt_check(env,_tmp691,
_tmp650);_tmp693=_tmp692.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp694=*_tmp693;union
Cyc_CfFlowInfo_FlowInfo _tmp696;union Cyc_CfFlowInfo_FlowInfo _tmp697;struct
_tuple15 _tmp695=Cyc_NewControlFlow_anal_test(env,_tmp694,_tmp64F);_tmp696=
_tmp695.f1;_tmp697=_tmp695.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp698=Cyc_NewControlFlow_anal_stmt(
env,_tmp696,_tmp654);union Cyc_CfFlowInfo_FlowInfo*_tmp69A;struct _tuple16 _tmp699=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp698,_tmp653);_tmp69A=_tmp699.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp69B=*_tmp69A;union Cyc_CfFlowInfo_FlowInfo _tmp69C=(Cyc_NewControlFlow_anal_Rexp(
env,0,_tmp69B,_tmp652)).f1;Cyc_NewControlFlow_update_flow(env,_tmp650,_tmp69C);
return _tmp697;};};};}_LL377: {struct Cyc_Absyn_Break_s_struct*_tmp655=(struct Cyc_Absyn_Break_s_struct*)
_tmp633;if(_tmp655->tag != 6)goto _LL379;else{_tmp656=_tmp655->f1;if(_tmp656 != 0)
goto _LL379;}}_LL378: return Cyc_CfFlowInfo_BottomFL();_LL379: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp657=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp633;if(_tmp657->tag != 11)goto
_LL37B;else{_tmp658=_tmp657->f1;_tmp659=_tmp657->f2;if(_tmp659 == 0)goto _LL37B;
_tmp65A=*_tmp659;}}_LL37A: {struct _RegionHandle*_tmp69D=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp69F;struct Cyc_List_List*_tmp6A0;struct _tuple19 _tmp69E=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp69D,env,inflow,_tmp658,1,1);_tmp69F=_tmp69E.f1;_tmp6A0=_tmp69E.f2;for(0;
_tmp6A0 != 0;(_tmp6A0=_tmp6A0->tl,_tmp658=_tmp658->tl)){_tmp69F=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp658))->hd)->loc,
_tmp69F,(void*)_tmp6A0->hd);}_tmp69F=Cyc_NewControlFlow_add_vars(_tmp632,_tmp69F,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp65A->pat_vars))->v,
_tmp632->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp65A->body,_tmp69F);return Cyc_CfFlowInfo_BottomFL();}_LL37B: {struct Cyc_Absyn_Break_s_struct*
_tmp65B=(struct Cyc_Absyn_Break_s_struct*)_tmp633;if(_tmp65B->tag != 6)goto _LL37D;
else{_tmp65C=_tmp65B->f1;}}_LL37C: _tmp65E=_tmp65C;goto _LL37E;_LL37D: {struct Cyc_Absyn_Continue_s_struct*
_tmp65D=(struct Cyc_Absyn_Continue_s_struct*)_tmp633;if(_tmp65D->tag != 7)goto
_LL37F;else{_tmp65E=_tmp65D->f1;}}_LL37E: _tmp660=_tmp65E;goto _LL380;_LL37F: {
struct Cyc_Absyn_Goto_s_struct*_tmp65F=(struct Cyc_Absyn_Goto_s_struct*)_tmp633;
if(_tmp65F->tag != 8)goto _LL381;else{_tmp660=_tmp65F->f2;}}_LL380: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp6A1=_tmp630->encloser;struct Cyc_Absyn_Stmt*
_tmp6A2=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp660)))->encloser;while(_tmp6A2 != _tmp6A1){struct Cyc_Absyn_Stmt*_tmp6A3=(Cyc_NewControlFlow_get_stmt_annot(
_tmp6A1))->encloser;if(_tmp6A3 == _tmp6A1){{const char*_tmp971;void*_tmp970;(
_tmp970=0,Cyc_Tcutil_terr(s->loc,((_tmp971="goto enters local scope or exception handler",
_tag_dyneither(_tmp971,sizeof(char),45))),_tag_dyneither(_tmp970,sizeof(void*),0)));}
break;}_tmp6A1=_tmp6A3;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp660),inflow);return Cyc_CfFlowInfo_BottomFL();_LL381: {struct Cyc_Absyn_Switch_s_struct*
_tmp661=(struct Cyc_Absyn_Switch_s_struct*)_tmp633;if(_tmp661->tag != 10)goto
_LL383;else{_tmp662=_tmp661->f1;_tmp663=_tmp661->f2;}}_LL382: {union Cyc_CfFlowInfo_FlowInfo
_tmp6A7;void*_tmp6A8;struct _tuple13 _tmp6A6=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp662);_tmp6A7=_tmp6A6.f1;_tmp6A8=_tmp6A6.f2;_tmp6A7=Cyc_NewControlFlow_use_Rval(
env,_tmp662->loc,_tmp6A7,_tmp6A8);return Cyc_NewControlFlow_anal_scs(env,_tmp6A7,
_tmp663);}_LL383: {struct Cyc_Absyn_TryCatch_s_struct*_tmp664=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp633;if(_tmp664->tag != 15)goto _LL385;else{_tmp665=_tmp664->f1;_tmp666=_tmp664->f2;}}
_LL384: {int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp665);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp666);{union Cyc_CfFlowInfo_FlowInfo _tmp6A9=scs_outflow;int
_tmp6AA;_LL390: if((_tmp6A9.BottomFL).tag != 1)goto _LL392;_tmp6AA=(int)(_tmp6A9.BottomFL).val;
_LL391: goto _LL38F;_LL392:;_LL393: {const char*_tmp974;void*_tmp973;(_tmp973=0,Cyc_Tcutil_terr(
s->loc,((_tmp974="last catch clause may implicitly fallthru",_tag_dyneither(
_tmp974,sizeof(char),42))),_tag_dyneither(_tmp973,sizeof(void*),0)));}_LL38F:;}
outflow=s1_outflow;return outflow;};};}_LL385: {struct Cyc_Absyn_Decl_s_struct*
_tmp667=(struct Cyc_Absyn_Decl_s_struct*)_tmp633;if(_tmp667->tag != 12)goto _LL387;
else{_tmp668=_tmp667->f1;_tmp669=*_tmp668;_tmp66A=_tmp669.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp66B=(struct Cyc_Absyn_Alias_d_struct*)_tmp66A;if(_tmp66B->tag != 5)goto _LL387;
else{_tmp66C=_tmp66B->f1;_tmp66D=_tmp66B->f2;_tmp66E=_tmp66B->f3;}};_tmp66F=
_tmp667->f2;}}_LL386: {union Cyc_CfFlowInfo_FlowInfo _tmp6AE;union Cyc_CfFlowInfo_AbsLVal
_tmp6AF;struct _tuple14 _tmp6AD=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp66C);
_tmp6AE=_tmp6AD.f1;_tmp6AF=_tmp6AD.f2;{struct _tuple14 _tmp975;struct _tuple14
_tmp6B1=(_tmp975.f1=_tmp6AE,((_tmp975.f2=_tmp6AF,_tmp975)));union Cyc_CfFlowInfo_FlowInfo
_tmp6B2;struct _tuple12 _tmp6B3;struct Cyc_Dict_Dict _tmp6B4;struct Cyc_List_List*
_tmp6B5;union Cyc_CfFlowInfo_AbsLVal _tmp6B6;struct Cyc_CfFlowInfo_Place*_tmp6B7;
union Cyc_CfFlowInfo_FlowInfo _tmp6B8;int _tmp6B9;_LL395: _tmp6B2=_tmp6B1.f1;if((
_tmp6B2.ReachableFL).tag != 2)goto _LL397;_tmp6B3=(struct _tuple12)(_tmp6B2.ReachableFL).val;
_tmp6B4=_tmp6B3.f1;_tmp6B5=_tmp6B3.f2;_tmp6B6=_tmp6B1.f2;if((_tmp6B6.PlaceL).tag
!= 1)goto _LL397;_tmp6B7=(struct Cyc_CfFlowInfo_Place*)(_tmp6B6.PlaceL).val;_LL396: {
void*_tmp6BA=Cyc_CfFlowInfo_lookup_place(_tmp6B4,_tmp6B7);void*t=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp66C->topt))->v;{void*_tmp6BB=_tmp66C->r;struct Cyc_Absyn_Exp*
_tmp6BD;_LL39C: {struct Cyc_Absyn_Cast_e_struct*_tmp6BC=(struct Cyc_Absyn_Cast_e_struct*)
_tmp6BB;if(_tmp6BC->tag != 15)goto _LL39E;else{_tmp6BD=_tmp6BC->f2;}}_LL39D: t=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6BD->topt))->v;goto _LL39B;_LL39E:;
_LL39F: goto _LL39B;_LL39B:;}{void*_tmp6BE=Cyc_CfFlowInfo_make_unique_consumed(
_tmp632,t,_tmp66C,env->iteration_num,_tmp6BA);_tmp6B4=Cyc_CfFlowInfo_assign_place(
_tmp632,_tmp66C->loc,_tmp6B4,env->all_changed,_tmp6B7,_tmp6BE);_tmp6AE=Cyc_CfFlowInfo_ReachableFL(
_tmp6B4,_tmp6B5);{void*leaf;switch(Cyc_CfFlowInfo_initlevel(_tmp632,_tmp6B4,
_tmp6BA)){case Cyc_CfFlowInfo_AllIL: _LL3A0: leaf=_tmp632->unknown_all;break;case
Cyc_CfFlowInfo_NoneIL: _LL3A1: leaf=_tmp632->unknown_none;break;case Cyc_CfFlowInfo_ThisIL:
_LL3A2: leaf=_tmp632->unknown_this;break;}{void*_tmp6BF=Cyc_CfFlowInfo_typ_to_absrval(
_tmp632,t,0,leaf);_tmp6AE=Cyc_NewControlFlow_use_Rval(env,_tmp66C->loc,_tmp6AE,
_tmp6BA);{struct Cyc_List_List _tmp976;struct Cyc_List_List _tmp6C0=(_tmp976.hd=
_tmp66E,((_tmp976.tl=0,_tmp976)));_tmp6AE=Cyc_NewControlFlow_add_vars(_tmp632,
_tmp6AE,(struct Cyc_List_List*)& _tmp6C0,_tmp632->unknown_all,s->loc);_tmp6AE=Cyc_NewControlFlow_anal_stmt(
env,_tmp6AE,_tmp66F);{union Cyc_CfFlowInfo_FlowInfo _tmp6C1=_tmp6AE;struct _tuple12
_tmp6C2;struct Cyc_Dict_Dict _tmp6C3;struct Cyc_List_List*_tmp6C4;_LL3A5: if((
_tmp6C1.ReachableFL).tag != 2)goto _LL3A7;_tmp6C2=(struct _tuple12)(_tmp6C1.ReachableFL).val;
_tmp6C3=_tmp6C2.f1;_tmp6C4=_tmp6C2.f2;_LL3A6: _tmp6C3=Cyc_CfFlowInfo_assign_place(
_tmp632,s->loc,_tmp6C3,env->all_changed,_tmp6B7,_tmp6BF);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp6C3,_tmp6C4);return outflow;_LL3A7:;_LL3A8: return _tmp6AE;_LL3A4:;};};};};};}
_LL397: _tmp6B8=_tmp6B1.f1;if((_tmp6B8.BottomFL).tag != 1)goto _LL399;_tmp6B9=(int)(
_tmp6B8.BottomFL).val;_LL398: return _tmp6AE;_LL399:;_LL39A:{const char*_tmp979;
void*_tmp978;(_tmp978=0,Cyc_Tcutil_terr(_tmp66C->loc,((_tmp979="bad alias expression--no unique path found",
_tag_dyneither(_tmp979,sizeof(char),43))),_tag_dyneither(_tmp978,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL394:;};}_LL387: {struct Cyc_Absyn_Decl_s_struct*
_tmp670=(struct Cyc_Absyn_Decl_s_struct*)_tmp633;if(_tmp670->tag != 12)goto _LL389;
else{_tmp671=_tmp670->f1;_tmp672=_tmp670->f2;}}_LL388: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp671),_tmp672);if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp671);return outflow;
_LL389: {struct Cyc_Absyn_Label_s_struct*_tmp673=(struct Cyc_Absyn_Label_s_struct*)
_tmp633;if(_tmp673->tag != 13)goto _LL38B;else{_tmp674=_tmp673->f2;}}_LL38A: return
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp674);_LL38B: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp675=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp633;if(_tmp675->tag != 16)goto
_LL38D;else{_tmp676=_tmp675->f1;}}_LL38C: {union Cyc_CfFlowInfo_FlowInfo _tmp6C9;
void*_tmp6CA;struct _tuple13 _tmp6C8=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,
_tmp676);_tmp6C9=_tmp6C8.f1;_tmp6CA=_tmp6C8.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp6CB=Cyc_NewControlFlow_use_Rval(env,_tmp676->loc,_tmp6C9,_tmp6CA);void*
_tmp6CC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp676->topt))->v);
void*_tmp6CE;_LL3AA: {struct Cyc_Absyn_RgnHandleType_struct*_tmp6CD=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6CC;if(_tmp6CD->tag != 16)goto _LL3AC;else{_tmp6CE=(void*)_tmp6CD->f1;}}_LL3AB:
return Cyc_CfFlowInfo_kill_flow_region(_tmp632,_tmp6C9,_tmp6CE);_LL3AC:;_LL3AD: {
struct Cyc_Core_Impossible_struct _tmp97F;const char*_tmp97E;struct Cyc_Core_Impossible_struct*
_tmp97D;(int)_throw((void*)((_tmp97D=_cycalloc(sizeof(*_tmp97D)),((_tmp97D[0]=((
_tmp97F.tag=Cyc_Core_Impossible,((_tmp97F.f1=((_tmp97E="anal_stmt -- reset_region",
_tag_dyneither(_tmp97E,sizeof(char),26))),_tmp97F)))),_tmp97D)))));}_LL3A9:;};}
_LL38D:;_LL38E: {struct Cyc_Core_Impossible_struct _tmp985;const char*_tmp984;
struct Cyc_Core_Impossible_struct*_tmp983;(int)_throw((void*)((_tmp983=_cycalloc(
sizeof(*_tmp983)),((_tmp983[0]=((_tmp985.tag=Cyc_Core_Impossible,((_tmp985.f1=((
_tmp984="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(
_tmp984,sizeof(char),56))),_tmp985)))),_tmp983)))));}_LL364:;};}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*
_tmp6D5=env->fenv;void*_tmp6D6=decl->r;struct Cyc_Absyn_Vardecl*_tmp6D8;struct Cyc_Core_Opt*
_tmp6DA;struct Cyc_Core_Opt _tmp6DB;struct Cyc_List_List*_tmp6DC;struct Cyc_Absyn_Exp*
_tmp6DD;struct Cyc_List_List*_tmp6DF;struct Cyc_Absyn_Fndecl*_tmp6E1;struct Cyc_Absyn_Tvar*
_tmp6E3;struct Cyc_Absyn_Vardecl*_tmp6E4;int _tmp6E5;struct Cyc_Absyn_Exp*_tmp6E6;
_LL3AF: {struct Cyc_Absyn_Var_d_struct*_tmp6D7=(struct Cyc_Absyn_Var_d_struct*)
_tmp6D6;if(_tmp6D7->tag != 0)goto _LL3B1;else{_tmp6D8=_tmp6D7->f1;}}_LL3B0: {
struct Cyc_List_List _tmp986;struct Cyc_List_List _tmp6E7=(_tmp986.hd=_tmp6D8,((
_tmp986.tl=0,_tmp986)));inflow=Cyc_NewControlFlow_add_vars(_tmp6D5,inflow,(
struct Cyc_List_List*)& _tmp6E7,_tmp6D5->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*
_tmp6E8=_tmp6D8->initializer;if(_tmp6E8 == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo
_tmp6EA;void*_tmp6EB;struct _tuple13 _tmp6E9=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,(struct Cyc_Absyn_Exp*)_tmp6E8);_tmp6EA=_tmp6E9.f1;_tmp6EB=_tmp6E9.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6EC=_tmp6EA;int _tmp6ED;struct _tuple12 _tmp6EE;
struct Cyc_Dict_Dict _tmp6EF;struct Cyc_List_List*_tmp6F0;_LL3BC: if((_tmp6EC.BottomFL).tag
!= 1)goto _LL3BE;_tmp6ED=(int)(_tmp6EC.BottomFL).val;_LL3BD: return Cyc_CfFlowInfo_BottomFL();
_LL3BE: if((_tmp6EC.ReachableFL).tag != 2)goto _LL3BB;_tmp6EE=(struct _tuple12)(
_tmp6EC.ReachableFL).val;_tmp6EF=_tmp6EE.f1;_tmp6F0=_tmp6EE.f2;_LL3BF:{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp98C;struct Cyc_CfFlowInfo_VarRoot_struct _tmp98B;struct Cyc_CfFlowInfo_Place*
_tmp98A;_tmp6EF=Cyc_CfFlowInfo_assign_place(_tmp6D5,decl->loc,_tmp6EF,env->all_changed,((
_tmp98A=_region_malloc(env->r,sizeof(*_tmp98A)),((_tmp98A->root=(void*)((_tmp98C=
_region_malloc(env->r,sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98B.tag=0,((_tmp98B.f1=
_tmp6D8,_tmp98B)))),_tmp98C)))),((_tmp98A->fields=0,_tmp98A)))))),_tmp6EB);}
_tmp6F0=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp6F0,_tmp6D8,(struct Cyc_Absyn_Exp*)
_check_null(_tmp6D8->initializer));{union Cyc_CfFlowInfo_FlowInfo _tmp6F4=Cyc_CfFlowInfo_ReachableFL(
_tmp6EF,_tmp6F0);Cyc_NewControlFlow_update_tryflow(env,_tmp6F4);return _tmp6F4;};
_LL3BB:;};};};}_LL3B1: {struct Cyc_Absyn_Let_d_struct*_tmp6D9=(struct Cyc_Absyn_Let_d_struct*)
_tmp6D6;if(_tmp6D9->tag != 2)goto _LL3B3;else{_tmp6DA=_tmp6D9->f2;if(_tmp6DA == 0)
goto _LL3B3;_tmp6DB=*_tmp6DA;_tmp6DC=(struct Cyc_List_List*)_tmp6DB.v;_tmp6DD=
_tmp6D9->f3;}}_LL3B2: {union Cyc_CfFlowInfo_FlowInfo _tmp6F7;void*_tmp6F8;struct
_tuple13 _tmp6F6=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6DD);_tmp6F7=
_tmp6F6.f1;_tmp6F8=_tmp6F6.f2;_tmp6F7=Cyc_NewControlFlow_use_Rval(env,_tmp6DD->loc,
_tmp6F7,_tmp6F8);return Cyc_NewControlFlow_add_vars(_tmp6D5,_tmp6F7,_tmp6DC,
_tmp6D5->unknown_all,decl->loc);}_LL3B3: {struct Cyc_Absyn_Letv_d_struct*_tmp6DE=(
struct Cyc_Absyn_Letv_d_struct*)_tmp6D6;if(_tmp6DE->tag != 3)goto _LL3B5;else{
_tmp6DF=_tmp6DE->f1;}}_LL3B4: return Cyc_NewControlFlow_add_vars(_tmp6D5,inflow,
_tmp6DF,_tmp6D5->unknown_none,decl->loc);_LL3B5: {struct Cyc_Absyn_Fn_d_struct*
_tmp6E0=(struct Cyc_Absyn_Fn_d_struct*)_tmp6D6;if(_tmp6E0->tag != 1)goto _LL3B7;
else{_tmp6E1=_tmp6E0->f1;}}_LL3B6: Cyc_NewControlFlow_check_nested_fun(_tmp6D5,
inflow,_tmp6E1);{void*_tmp6F9=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp6E1->cached_typ))->v;
struct Cyc_Absyn_Vardecl*_tmp6FA=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp6E1->fn_vardecl);
struct Cyc_List_List*_tmp98D;return Cyc_NewControlFlow_add_vars(_tmp6D5,inflow,((
_tmp98D=_region_malloc(env->r,sizeof(*_tmp98D)),((_tmp98D->hd=_tmp6FA,((_tmp98D->tl=
0,_tmp98D)))))),_tmp6D5->unknown_all,decl->loc);};_LL3B7: {struct Cyc_Absyn_Region_d_struct*
_tmp6E2=(struct Cyc_Absyn_Region_d_struct*)_tmp6D6;if(_tmp6E2->tag != 4)goto _LL3B9;
else{_tmp6E3=_tmp6E2->f1;_tmp6E4=_tmp6E2->f2;_tmp6E5=_tmp6E2->f3;_tmp6E6=_tmp6E2->f4;}}
_LL3B8: if(_tmp6E6 != 0){struct Cyc_Absyn_Exp*_tmp6FC=(struct Cyc_Absyn_Exp*)_tmp6E6;
union Cyc_CfFlowInfo_FlowInfo _tmp6FE;void*_tmp6FF;struct _tuple13 _tmp6FD=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp6FC);_tmp6FE=_tmp6FD.f1;_tmp6FF=_tmp6FD.f2;inflow=Cyc_NewControlFlow_use_Rval(
env,_tmp6FC->loc,_tmp6FE,_tmp6FF);}{struct Cyc_List_List _tmp98E;struct Cyc_List_List
_tmp700=(_tmp98E.hd=_tmp6E4,((_tmp98E.tl=0,_tmp98E)));return Cyc_NewControlFlow_add_vars(
_tmp6D5,inflow,(struct Cyc_List_List*)& _tmp700,_tmp6D5->unknown_all,decl->loc);};
_LL3B9:;_LL3BA: {struct Cyc_Core_Impossible_struct _tmp994;const char*_tmp993;
struct Cyc_Core_Impossible_struct*_tmp992;(int)_throw((void*)((_tmp992=_cycalloc(
sizeof(*_tmp992)),((_tmp992[0]=((_tmp994.tag=Cyc_Core_Impossible,((_tmp994.f1=((
_tmp993="anal_decl: unexpected decl variant",_tag_dyneither(_tmp993,sizeof(char),
35))),_tmp994)))),_tmp992)))));}_LL3AE:;}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){struct _RegionHandle _tmp705=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp705;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*
fenv=Cyc_CfFlowInfo_new_flow_env(frgn);Cyc_NewControlFlow_check_nested_fun(fenv,
Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);};_pop_region(frgn);}static
int Cyc_NewControlFlow_hash_ptr(void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle*_tmp706=fenv->r;struct Cyc_Position_Segment*_tmp707=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp707);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp708=inflow;int _tmp709;struct
_tuple12 _tmp70A;struct Cyc_Dict_Dict _tmp70B;struct Cyc_List_List*_tmp70C;_LL3C1:
if((_tmp708.BottomFL).tag != 1)goto _LL3C3;_tmp709=(int)(_tmp708.BottomFL).val;
_LL3C2: {const char*_tmp997;void*_tmp996;(_tmp996=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp997="check_fun",
_tag_dyneither(_tmp997,sizeof(char),10))),_tag_dyneither(_tmp996,sizeof(void*),0)));}
_LL3C3: if((_tmp708.ReachableFL).tag != 2)goto _LL3C0;_tmp70A=(struct _tuple12)(
_tmp708.ReachableFL).val;_tmp70B=_tmp70A.f1;_tmp70C=_tmp70A.f2;_LL3C4: {struct
Cyc_List_List*atts;{void*_tmp70F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp711;struct Cyc_List_List*
_tmp712;_LL3C6: {struct Cyc_Absyn_FnType_struct*_tmp710=(struct Cyc_Absyn_FnType_struct*)
_tmp70F;if(_tmp710->tag != 10)goto _LL3C8;else{_tmp711=_tmp710->f1;_tmp712=_tmp711.attributes;}}
_LL3C7: atts=_tmp712;goto _LL3C5;_LL3C8:;_LL3C9: {const char*_tmp99A;void*_tmp999;(
_tmp999=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp99A="check_fun: non-function type cached with fndecl_t",_tag_dyneither(
_tmp99A,sizeof(char),50))),_tag_dyneither(_tmp999,sizeof(void*),0)));}_LL3C5:;}
for(0;atts != 0;atts=atts->tl){void*_tmp715=(void*)atts->hd;int _tmp717;int _tmp719;
_LL3CB: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp716=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp715;if(_tmp716->tag != 21)goto _LL3CD;else{_tmp717=_tmp716->f1;}}_LL3CC: {
unsigned int j=(unsigned int)_tmp717;struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(
- 1,0);if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{const char*_tmp99D;
void*_tmp99C;(_tmp99C=0,Cyc_Tcutil_terr(_tmp707,((_tmp99D="noliveunique attribute exceeds number of parameters",
_tag_dyneither(_tmp99D,sizeof(char),52))),_tag_dyneither(_tmp99C,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp71C=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));void*t=Cyc_Tcutil_compress(
_tmp71C->type);if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp9A0;void*
_tmp99F;(_tmp99F=0,Cyc_Tcutil_terr(_tmp707,((_tmp9A0="noliveunique attribute requires unique pointer type",
_tag_dyneither(_tmp9A0,sizeof(char),52))),_tag_dyneither(_tmp99F,sizeof(void*),0)));}
continue;}{void*elttype=Cyc_Tcutil_pointer_elt_type(t);void*_tmp71F=Cyc_CfFlowInfo_make_unique_consumed(
fenv,elttype,bogus_exp,- 1,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_struct _tmp9A3;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp9A2;struct Cyc_CfFlowInfo_InitParam_struct*_tmp720=(_tmp9A2=_region_malloc(
_tmp706,sizeof(*_tmp9A2)),((_tmp9A2[0]=((_tmp9A3.tag=2,((_tmp9A3.f1=(int)j,((
_tmp9A3.f2=(void*)t,_tmp9A3)))))),_tmp9A2)));struct Cyc_CfFlowInfo_Place*_tmp9A4;
struct Cyc_CfFlowInfo_Place*_tmp721=(_tmp9A4=_region_malloc(_tmp706,sizeof(*
_tmp9A4)),((_tmp9A4->root=(void*)_tmp720,((_tmp9A4->fields=0,_tmp9A4)))));
_tmp70B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp70B,(void*)_tmp720,_tmp71F);{struct Cyc_CfFlowInfo_AddressOf_struct _tmp9AA;
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp9A9;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp9A7;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp9A6;_tmp70B=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp70B,(void*)((_tmp9A6=
_region_malloc(_tmp706,sizeof(*_tmp9A6)),((_tmp9A6[0]=((_tmp9A7.tag=0,((_tmp9A7.f1=
_tmp71C,_tmp9A7)))),_tmp9A6)))),(void*)((_tmp9A9=_region_malloc(_tmp706,sizeof(*
_tmp9A9)),((_tmp9A9[0]=((_tmp9AA.tag=5,((_tmp9AA.f1=_tmp721,_tmp9AA)))),_tmp9A9)))));}
goto _LL3CA;};};}_LL3CD: {struct Cyc_Absyn_Initializes_att_struct*_tmp718=(struct
Cyc_Absyn_Initializes_att_struct*)_tmp715;if(_tmp718->tag != 20)goto _LL3CF;else{
_tmp719=_tmp718->f1;}}_LL3CE: {unsigned int j=(unsigned int)_tmp719;if(j > ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){{const char*_tmp9AD;void*_tmp9AC;(_tmp9AC=0,
Cyc_Tcutil_terr(_tmp707,((_tmp9AD="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp9AD,sizeof(char),51))),_tag_dyneither(_tmp9AC,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp72B=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp72C=Cyc_Tcutil_compress(
_tmp72B->type);struct Cyc_Absyn_PtrInfo _tmp72E;void*_tmp72F;struct Cyc_Absyn_PtrAtts
_tmp730;union Cyc_Absyn_Constraint*_tmp731;union Cyc_Absyn_Constraint*_tmp732;
union Cyc_Absyn_Constraint*_tmp733;_LL3D2: {struct Cyc_Absyn_PointerType_struct*
_tmp72D=(struct Cyc_Absyn_PointerType_struct*)_tmp72C;if(_tmp72D->tag != 5)goto
_LL3D4;else{_tmp72E=_tmp72D->f1;_tmp72F=_tmp72E.elt_typ;_tmp730=_tmp72E.ptr_atts;
_tmp731=_tmp730.nullable;_tmp732=_tmp730.bounds;_tmp733=_tmp730.zero_term;}}
_LL3D3: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp731)){
const char*_tmp9B0;void*_tmp9AF;(_tmp9AF=0,Cyc_Tcutil_terr(_tmp707,((_tmp9B0="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp9B0,sizeof(char),55))),_tag_dyneither(_tmp9AF,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp732)){const char*_tmp9B3;void*_tmp9B2;(_tmp9B2=0,
Cyc_Tcutil_terr(_tmp707,((_tmp9B3="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp9B3,sizeof(char),57))),_tag_dyneither(_tmp9B2,sizeof(void*),0)));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp733)){
const char*_tmp9B6;void*_tmp9B5;(_tmp9B5=0,Cyc_Tcutil_terr(_tmp707,((_tmp9B6="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp9B6,sizeof(char),77))),_tag_dyneither(_tmp9B5,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_struct _tmp9B9;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp9B8;struct Cyc_CfFlowInfo_InitParam_struct*_tmp73A=(_tmp9B8=_region_malloc(
_tmp706,sizeof(*_tmp9B8)),((_tmp9B8[0]=((_tmp9B9.tag=2,((_tmp9B9.f1=(int)j,((
_tmp9B9.f2=(void*)_tmp72F,_tmp9B9)))))),_tmp9B8)));struct Cyc_CfFlowInfo_Place*
_tmp9BA;struct Cyc_CfFlowInfo_Place*_tmp73B=(_tmp9BA=_region_malloc(_tmp706,
sizeof(*_tmp9BA)),((_tmp9BA->root=(void*)_tmp73A,((_tmp9BA->fields=0,_tmp9BA)))));
_tmp70B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp70B,(void*)_tmp73A,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp72F,0,fenv->esc_none));{
struct Cyc_CfFlowInfo_AddressOf_struct _tmp9C0;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp9BF;struct Cyc_CfFlowInfo_VarRoot_struct _tmp9BD;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp9BC;_tmp70B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp70B,(void*)((_tmp9BC=_region_malloc(_tmp706,sizeof(*_tmp9BC)),((_tmp9BC[0]=((
_tmp9BD.tag=0,((_tmp9BD.f1=_tmp72B,_tmp9BD)))),_tmp9BC)))),(void*)((_tmp9BF=
_region_malloc(_tmp706,sizeof(*_tmp9BF)),((_tmp9BF[0]=((_tmp9C0.tag=5,((_tmp9C0.f1=
_tmp73B,_tmp9C0)))),_tmp9BF)))));}{struct Cyc_List_List*_tmp9C1;param_roots=((
_tmp9C1=_region_malloc(_tmp706,sizeof(*_tmp9C1)),((_tmp9C1->hd=_tmp73B,((_tmp9C1->tl=
param_roots,_tmp9C1))))));}goto _LL3D1;};_LL3D4:;_LL3D5: {const char*_tmp9C4;void*
_tmp9C3;(_tmp9C3=0,Cyc_Tcutil_terr(_tmp707,((_tmp9C4="initializes attribute on non-pointer",
_tag_dyneither(_tmp9C4,sizeof(char),37))),_tag_dyneither(_tmp9C3,sizeof(void*),0)));}
_LL3D1:;}goto _LL3CA;};}_LL3CF:;_LL3D0: goto _LL3CA;_LL3CA:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp70B,_tmp70C);}_LL3C0:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp706,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp9C5;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp9C5=_region_malloc(_tmp706,sizeof(*_tmp9C5)),((
_tmp9C5->r=_tmp706,((_tmp9C5->fenv=fenv,((_tmp9C5->iterate_again=1,((_tmp9C5->iteration_num=
0,((_tmp9C5->in_try=0,((_tmp9C5->tryflow=inflow,((_tmp9C5->all_changed=0,((
_tmp9C5->noreturn=noreturn,((_tmp9C5->param_roots=param_roots,((_tmp9C5->flow_table=
flow_table,_tmp9C5)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
union Cyc_CfFlowInfo_FlowInfo _tmp746=outflow;int _tmp747;_LL3D7: if((_tmp746.BottomFL).tag
!= 1)goto _LL3D9;_tmp747=(int)(_tmp746.BottomFL).val;_LL3D8: goto _LL3D6;_LL3D9:;
_LL3DA: Cyc_NewControlFlow_check_init_params(_tmp707,env,outflow);if(noreturn){
const char*_tmp9C8;void*_tmp9C7;(_tmp9C7=0,Cyc_Tcutil_terr(_tmp707,((_tmp9C8="`noreturn' function might (implicitly) return",
_tag_dyneither(_tmp9C8,sizeof(char),46))),_tag_dyneither(_tmp9C7,sizeof(void*),0)));}
else{void*_tmp74A=Cyc_Tcutil_compress(fd->ret_type);_LL3DC: {struct Cyc_Absyn_VoidType_struct*
_tmp74B=(struct Cyc_Absyn_VoidType_struct*)_tmp74A;if(_tmp74B->tag != 0)goto _LL3DE;}
_LL3DD: goto _LL3DB;_LL3DE: {struct Cyc_Absyn_DoubleType_struct*_tmp74C=(struct Cyc_Absyn_DoubleType_struct*)
_tmp74A;if(_tmp74C->tag != 8)goto _LL3E0;}_LL3DF: goto _LL3E1;_LL3E0: {struct Cyc_Absyn_FloatType_struct*
_tmp74D=(struct Cyc_Absyn_FloatType_struct*)_tmp74A;if(_tmp74D->tag != 7)goto
_LL3E2;}_LL3E1: goto _LL3E3;_LL3E2: {struct Cyc_Absyn_IntType_struct*_tmp74E=(
struct Cyc_Absyn_IntType_struct*)_tmp74A;if(_tmp74E->tag != 6)goto _LL3E4;}_LL3E3:{
const char*_tmp9CB;void*_tmp9CA;(_tmp9CA=0,Cyc_Tcutil_warn(_tmp707,((_tmp9CB="function may not return a value",
_tag_dyneither(_tmp9CB,sizeof(char),32))),_tag_dyneither(_tmp9CA,sizeof(void*),0)));}
goto _LL3DB;_LL3E4:;_LL3E5:{const char*_tmp9CE;void*_tmp9CD;(_tmp9CD=0,Cyc_Tcutil_terr(
_tmp707,((_tmp9CE="function may not return a value",_tag_dyneither(_tmp9CE,
sizeof(char),32))),_tag_dyneither(_tmp9CD,sizeof(void*),0)));}goto _LL3DB;_LL3DB:;}
goto _LL3D6;_LL3D6:;};};};}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){void*_tmp754=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct
Cyc_Absyn_Fndecl*_tmp756;struct Cyc_List_List*_tmp758;struct Cyc_List_List*_tmp75A;
struct Cyc_List_List*_tmp75C;_LL3E7: {struct Cyc_Absyn_Fn_d_struct*_tmp755=(struct
Cyc_Absyn_Fn_d_struct*)_tmp754;if(_tmp755->tag != 1)goto _LL3E9;else{_tmp756=
_tmp755->f1;}}_LL3E8: Cyc_NewControlFlow_check_fun(_tmp756);goto _LL3E6;_LL3E9: {
struct Cyc_Absyn_ExternC_d_struct*_tmp757=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp754;if(_tmp757->tag != 12)goto _LL3EB;else{_tmp758=_tmp757->f1;}}_LL3EA:
_tmp75A=_tmp758;goto _LL3EC;_LL3EB: {struct Cyc_Absyn_Using_d_struct*_tmp759=(
struct Cyc_Absyn_Using_d_struct*)_tmp754;if(_tmp759->tag != 11)goto _LL3ED;else{
_tmp75A=_tmp759->f2;}}_LL3EC: _tmp75C=_tmp75A;goto _LL3EE;_LL3ED: {struct Cyc_Absyn_Namespace_d_struct*
_tmp75B=(struct Cyc_Absyn_Namespace_d_struct*)_tmp754;if(_tmp75B->tag != 10)goto
_LL3EF;else{_tmp75C=_tmp75B->f2;}}_LL3EE: Cyc_NewControlFlow_cf_check(_tmp75C);
goto _LL3E6;_LL3EF: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp75D=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp754;if(_tmp75D->tag != 13)goto _LL3F1;}_LL3F0: goto _LL3E6;_LL3F1:;_LL3F2: goto
_LL3E6;_LL3E6:;}}
