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
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};typedef void*Cyc_parg_t;struct _dyneither_ptr Cyc_aprintf(
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};typedef
unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17];struct
Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[
8];struct Cyc_Core_Failure_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[
11];struct Cyc_Core_Impossible_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{char*tag;};extern char
Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern struct
_RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct
Cyc_Core_Open_Region_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct
Cyc_Core_Free_Region_struct{char*tag;};inline static void* arrcast(struct
_dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || 
dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}struct
Cyc_List_List{void*hd;struct Cyc_List_List*tl;};typedef struct Cyc_List_List*Cyc_List_list_t;
typedef struct Cyc_List_List*Cyc_List_List_t;struct Cyc_List_List*Cyc_List_rlist(
struct _RegionHandle*,struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct
Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;int
Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*
Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};
struct Cyc_Dict_T;typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};typedef
struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,
void**ans);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict
d);struct Cyc_Hashtable_Table;typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*
cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*
t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,
void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};typedef
unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};typedef void*Cyc_Position_error_kind_t;
struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;
struct _dyneither_ptr desc;};typedef struct Cyc_Position_Error*Cyc_Position_error_t;
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};int Cyc_Position_error_p();typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
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
};struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};
struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};typedef void*Cyc_Absyn_funcparams_t;
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};
struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_struct{
int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};struct Cyc_Absyn_Fastcall_att_struct{
int tag;};struct Cyc_Absyn_Noreturn_att_struct{int tag;};struct Cyc_Absyn_Const_att_struct{
int tag;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{
int tag;};struct Cyc_Absyn_Section_att_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_Nocommon_att_struct{int tag;};struct Cyc_Absyn_Shared_att_struct{
int tag;};struct Cyc_Absyn_Unused_att_struct{int tag;};struct Cyc_Absyn_Weak_att_struct{
int tag;};struct Cyc_Absyn_Dllimport_att_struct{int tag;};struct Cyc_Absyn_Dllexport_att_struct{
int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{int tag;};struct Cyc_Absyn_Constructor_att_struct{
int tag;};struct Cyc_Absyn_Destructor_att_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{
int tag;};struct Cyc_Absyn_Format_att_struct{int tag;enum Cyc_Absyn_Format_Type f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_struct{
int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{int tag;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};
struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual
f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{
int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;unsigned int f1;struct Cyc_List_List*f2;};typedef void*Cyc_Absyn_type_modifier_t;
struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;
char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct
_union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};
struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct
_union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;
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
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple1*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop
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
char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{char*tag;};void*
Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*
y,union Cyc_Absyn_Constraint*x);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,
unsigned int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU
info);int Cyc_Absyn_is_nontagged_union_type(void*);struct Cyc_RgnOrder_RgnPO;
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);struct
Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);struct Cyc_RgnOrder_RgnPO*
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
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(
unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(
void*t);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);int Cyc_Tcutil_is_noreturn(void*);void Cyc_Tcutil_check_no_qual(
unsigned int loc,void*t);struct _tuple11{unsigned int f1;int f2;};struct _tuple11 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};typedef void*Cyc_CfFlowInfo_root_t;
struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};typedef struct
Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL
 = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};typedef enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;
};struct _tuple12{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple12 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,void*);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessEqConst(unsigned int);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;union Cyc_CfFlowInfo_RelnOp rop;};typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[
7];struct Cyc_CfFlowInfo_IsZero_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[
8];struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;
struct Cyc_List_List*f1;};extern struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val;
struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
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
unsigned int loc);typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct
Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int
tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct
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
struct Cyc_List_List*fs,struct _dyneither_ptr*f);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int
no_init_bits_only,void*leafval);void*Cyc_CfFlowInfo_make_unique_consumed(struct
Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,
void*);int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int
env_iteration,void*r,int*needs_unconsume);void*Cyc_CfFlowInfo_make_unique_unconsumed(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
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
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
struct _tuple14{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple14 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple14 pr1,
struct _tuple14 pr2);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union
Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
typedef void*Cyc_CfFlowInfo_killrgn_t;void Cyc_NewControlFlow_set_encloser(struct
Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds);extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;typedef struct Cyc_PP_Out*
Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{
int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int
rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;
int print_using_stmts;int print_externC_stmts;int print_full_evars;int
print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);typedef struct Cyc_Dict_Dict
Cyc_NewControlFlow_dict_t;typedef unsigned int Cyc_NewControlFlow_seg_t;int Cyc_NewControlFlow_warn_lose_unique=
0;struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;
};typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;
static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_struct{
char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){struct Cyc_NewControlFlow_CFAnnot_struct
_tmp77E;struct Cyc_NewControlFlow_CFStmtAnnot _tmp77D;struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp77C;enclosee->annot=(void*)((_tmp77C=_cycalloc(sizeof(*_tmp77C)),((_tmp77C[0]=((
_tmp77E.tag=Cyc_NewControlFlow_CFAnnot,((_tmp77E.f1=((_tmp77D.encloser=encloser,((
_tmp77D.visited=0,_tmp77D)))),_tmp77E)))),_tmp77C))));}struct Cyc_NewControlFlow_AnalEnv{
struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*
flow_table;};typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*);static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,
struct Cyc_Absyn_Decl*);struct _tuple15{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal
f2;};static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo,int expand_unique,struct Cyc_Absyn_Exp*);static struct
_tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple16{
union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};static struct
_tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,
struct Cyc_Absyn_Exp*);static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);static
struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*
s){void*_tmp3=s->annot;struct Cyc_NewControlFlow_CFStmtAnnot _tmp5;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp6;_LL1: {struct Cyc_NewControlFlow_CFAnnot_struct*_tmp4=(struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp5=_tmp4->f1;
_tmp6=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp4->f1;}}_LL2: return _tmp6;_LL3:;
_LL4: {struct Cyc_Core_Impossible_struct _tmp784;const char*_tmp783;struct Cyc_Core_Impossible_struct*
_tmp782;(int)_throw((void*)((_tmp782=_cycalloc(sizeof(*_tmp782)),((_tmp782[0]=((
_tmp784.tag=Cyc_Core_Impossible,((_tmp784.f1=((_tmp783="ControlFlow -- wrong stmt annotation",
_tag_dyneither(_tmp783,sizeof(char),37))),_tmp784)))),_tmp782)))));}_LL0:;}
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*_tmp785;union Cyc_CfFlowInfo_FlowInfo*
res=(_tmp785=_region_malloc(env->r,sizeof(*_tmp785)),((_tmp785[0]=Cyc_CfFlowInfo_BottomFL(),
_tmp785)));((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union
Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);return
res;}return*sflow;}struct _tuple17{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union
Cyc_CfFlowInfo_FlowInfo*f2;};static struct _tuple17 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*
_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);_tmpB->visited=
env->iteration_num;{struct _tuple17 _tmp786;return(_tmp786.f1=_tmpB,((_tmp786.f2=
_tmpC,_tmp786)));};}static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo new_flow){if(env->in_try)env->tryflow=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple18{struct Cyc_NewControlFlow_AnalEnv*
f1;unsigned int f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_unique_root(
struct _tuple18*ckenv,void*root,void*rval){struct _tuple18 _tmpF;struct Cyc_NewControlFlow_AnalEnv*
_tmp10;unsigned int _tmp11;struct Cyc_Dict_Dict _tmp12;struct _tuple18*_tmpE=ckenv;
_tmpF=*_tmpE;_tmp10=_tmpF.f1;_tmp11=_tmpF.f2;_tmp12=_tmpF.f3;{void*_tmp13=root;
struct Cyc_Absyn_Vardecl*_tmp15;_LL6: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp14=(
struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp13;if(_tmp14->tag != 0)goto _LL8;else{
_tmp15=_tmp14->f1;}}_LL7: if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(
_tmp12,root,& rval) && Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp10->fenv)->r,
_tmp15->type)){{void*_tmp16=rval;enum Cyc_CfFlowInfo_InitLevel _tmp19;_LLB: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmp17=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp16;if(_tmp17->tag != 7)goto _LLD;}_LLC: goto _LLE;_LLD: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp18=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp16;if(_tmp18->tag != 3)goto _LLF;
else{_tmp19=_tmp18->f1;if(_tmp19 != Cyc_CfFlowInfo_NoneIL)goto _LLF;}}_LLE: goto
_LL10;_LLF: {struct Cyc_CfFlowInfo_Zero_struct*_tmp1A=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp16;if(_tmp1A->tag != 0)goto _LL11;}_LL10: goto _LLA;_LL11:;_LL12:{const char*
_tmp78A;void*_tmp789[1];struct Cyc_String_pa_struct _tmp788;(_tmp788.tag=0,((
_tmp788.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp15->name)),((_tmp789[0]=& _tmp788,Cyc_Tcutil_warn(_tmp11,((_tmp78A="unique pointer(s) reachable from %s may become unreachable.",
_tag_dyneither(_tmp78A,sizeof(char),60))),_tag_dyneither(_tmp789,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}goto _LL5;}goto _LL5;_LL8:;_LL9: goto _LL5;_LL5:;};}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
flow,*_tmp1F);if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple16 _tmp78B;
struct _tuple16 _tmp22=(_tmp78B.f1=flow,((_tmp78B.f2=_tmp20,_tmp78B)));union Cyc_CfFlowInfo_FlowInfo
_tmp23;struct _tuple13 _tmp24;struct Cyc_Dict_Dict _tmp25;union Cyc_CfFlowInfo_FlowInfo
_tmp26;struct _tuple13 _tmp27;struct Cyc_Dict_Dict _tmp28;_LL14: _tmp23=_tmp22.f1;if((
_tmp23.ReachableFL).tag != 2)goto _LL16;_tmp24=(struct _tuple13)(_tmp23.ReachableFL).val;
_tmp25=_tmp24.f1;_tmp26=_tmp22.f2;if((_tmp26.ReachableFL).tag != 2)goto _LL16;
_tmp27=(struct _tuple13)(_tmp26.ReachableFL).val;_tmp28=_tmp27.f1;_LL15: {struct
_tuple18 _tmp78C;struct _tuple18 _tmp29=(_tmp78C.f1=env,((_tmp78C.f2=s->loc,((
_tmp78C.f3=_tmp28,_tmp78C)))));((void(*)(void(*f)(struct _tuple18*,void*,void*),
struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,&
_tmp29,_tmp25);goto _LL13;}_LL16:;_LL17: goto _LL13;_LL13:;}if(!Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp20,*_tmp1F)){*_tmp1F=_tmp20;if(_tmp1E->visited == env->iteration_num)env->iterate_again=
1;}}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,
unsigned int loc){union Cyc_CfFlowInfo_FlowInfo _tmp2B=inflow;int _tmp2C;struct
_tuple13 _tmp2D;struct Cyc_Dict_Dict _tmp2E;struct Cyc_List_List*_tmp2F;_LL19: if((
_tmp2B.BottomFL).tag != 1)goto _LL1B;_tmp2C=(int)(_tmp2B.BottomFL).val;_LL1A:
return Cyc_CfFlowInfo_BottomFL();_LL1B: if((_tmp2B.ReachableFL).tag != 2)goto _LL18;
_tmp2D=(struct _tuple13)(_tmp2B.ReachableFL).val;_tmp2E=_tmp2D.f1;_tmp2F=_tmp2D.f2;
_LL1C: for(0;vds != 0;vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct _tmp78F;
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp78E;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp30=(_tmp78E=_region_malloc(fenv->r,sizeof(*_tmp78E)),((_tmp78E[0]=((_tmp78F.tag=
0,((_tmp78F.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp78F)))),_tmp78E)));_tmp2E=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp2E,(void*)_tmp30,Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)
vds->hd)->type,0,leafval));}return Cyc_CfFlowInfo_ReachableFL(_tmp2E,_tmp2F);
_LL18:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*
env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){union Cyc_CfFlowInfo_FlowInfo
_tmp33=inflow;int _tmp34;struct _tuple13 _tmp35;struct Cyc_Dict_Dict _tmp36;struct Cyc_List_List*
_tmp37;_LL1E: if((_tmp33.BottomFL).tag != 1)goto _LL20;_tmp34=(int)(_tmp33.BottomFL).val;
_LL1F: return Cyc_CfFlowInfo_BottomFL();_LL20: if((_tmp33.ReachableFL).tag != 2)goto
_LL1D;_tmp35=(struct _tuple13)(_tmp33.ReachableFL).val;_tmp36=_tmp35.f1;_tmp37=
_tmp35.f2;_LL21: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp36,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp792;void*_tmp791;(_tmp791=0,Cyc_Tcutil_terr(loc,((_tmp792="expression may not be fully initialized",
_tag_dyneither(_tmp792,sizeof(char),40))),_tag_dyneither(_tmp791,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp3A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp36,env->all_changed,
r);if(_tmp36.t == _tmp3A.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp3B=Cyc_CfFlowInfo_ReachableFL(
_tmp3A,_tmp37);Cyc_NewControlFlow_update_tryflow(env,_tmp3B);return _tmp3B;};};
_LL1D:;}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_NewControlFlow_check_nounique(
struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){struct
_tuple0 _tmp793;struct _tuple0 _tmp3D=(_tmp793.f1=Cyc_Tcutil_compress(t),((_tmp793.f2=
r,_tmp793)));void*_tmp3E;enum Cyc_CfFlowInfo_InitLevel _tmp40;void*_tmp41;void*
_tmp43;void*_tmp45;struct Cyc_Absyn_DatatypeFieldInfo _tmp47;union Cyc_Absyn_DatatypeFieldInfoU
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
struct Cyc_Absyn_TupleType_struct*)_tmp4E;if(_tmp4F->tag != 10)goto _LL2D;else{
_tmp50=_tmp4F->f1;}};_tmp51=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp52=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp51;if(_tmp52->tag != 6)goto
_LL2D;else{_tmp53=_tmp52->f2;}};_LL2C: {unsigned int sz=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp50);{int i=0;for(0;i < sz;++ i){Cyc_NewControlFlow_check_nounique(
env,loc,(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp50))->hd)).f2,*((
void**)_check_dyneither_subscript(_tmp53,sizeof(void*),i)));}}return;}_LL2D:
_tmp54=_tmp3D.f1;{struct Cyc_Absyn_AggrType_struct*_tmp55=(struct Cyc_Absyn_AggrType_struct*)
_tmp54;if(_tmp55->tag != 11)goto _LL2F;else{_tmp56=_tmp55->f1;_tmp57=_tmp56.aggr_info;
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
_tmp5C;if(_tmp5D->tag != 12)goto _LL31;else{_tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;}};
_tmp60=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp61=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp60;if(_tmp61->tag != 6)goto _LL31;else{_tmp62=_tmp61->f2;}};_LL30:{int i=0;for(
0;i < _get_dyneither_size(_tmp62,sizeof(void*));(i ++,_tmp5F=_tmp5F->tl)){Cyc_NewControlFlow_check_nounique(
env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)->type,((
void**)_tmp62.curr)[i]);}}return;_LL31: _tmp63=_tmp3D.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp64=(struct Cyc_Absyn_PointerType_struct*)_tmp63;if(_tmp64->tag != 5)goto _LL33;};
if(!Cyc_Tcutil_is_noalias_pointer(t))goto _LL33;_LL32:{const char*_tmp796;void*
_tmp795;(_tmp795=0,Cyc_Tcutil_warn(loc,((_tmp796="argument may still contain unique pointers",
_tag_dyneither(_tmp796,sizeof(char),43))),_tag_dyneither(_tmp795,sizeof(void*),0)));}
return;_LL33:;_LL34: return;_LL22:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp6B=inflow;int _tmp6C;struct
_tuple13 _tmp6D;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*_tmp6F;_LL36: if((
_tmp6B.BottomFL).tag != 1)goto _LL38;_tmp6C=(int)(_tmp6B.BottomFL).val;_LL37:
return Cyc_CfFlowInfo_BottomFL();_LL38: if((_tmp6B.ReachableFL).tag != 2)goto _LL35;
_tmp6D=(struct _tuple13)(_tmp6B.ReachableFL).val;_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;
_LL39: if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp799;void*_tmp798;(
_tmp798=0,Cyc_Tcutil_terr(loc,((_tmp799="noliveunique attribute requires unique pointer",
_tag_dyneither(_tmp799,sizeof(char),47))),_tag_dyneither(_tmp798,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{void*_tmp72=Cyc_Tcutil_pointer_elt_type(t);{
void*_tmp73=r;struct Cyc_CfFlowInfo_Place*_tmp75;_LL3B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp74=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp73;if(_tmp74->tag != 5)goto
_LL3D;else{_tmp75=_tmp74->f1;}}_LL3C: Cyc_NewControlFlow_check_nounique(env,loc,
_tmp72,Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp75));goto _LL3A;_LL3D:;_LL3E: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,_tmp72)){const char*_tmp79C;void*_tmp79B;(_tmp79B=0,Cyc_Tcutil_warn(
loc,((_tmp79C="argument may contain live unique pointers",_tag_dyneither(_tmp79C,
sizeof(char),42))),_tag_dyneither(_tmp79B,sizeof(void*),0)));}return Cyc_NewControlFlow_use_Rval(
env,loc,inflow,r);_LL3A:;}{struct Cyc_Dict_Dict _tmp78=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp6E,env->all_changed,r);if(_tmp6E.t == _tmp78.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo _tmp79=Cyc_CfFlowInfo_ReachableFL(_tmp78,_tmp6F);Cyc_NewControlFlow_update_tryflow(
env,_tmp79);return _tmp79;};};};_LL35:;}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo
f1;struct Cyc_List_List*f2;};static struct _tuple20 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){if(es == 0){
struct _tuple20 _tmp79D;return(_tmp79D.f1=inflow,((_tmp79D.f2=0,_tmp79D)));}if(es->tl
== 0){union Cyc_CfFlowInfo_FlowInfo _tmp7C;void*_tmp7D;struct _tuple14 _tmp7B=Cyc_NewControlFlow_anal_Rexp(
env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp7C=_tmp7B.f1;_tmp7D=
_tmp7B.f2;{struct Cyc_List_List*_tmp7A0;struct _tuple20 _tmp79F;return(_tmp79F.f1=
_tmp7C,((_tmp79F.f2=((_tmp7A0=_region_malloc(rgn,sizeof(*_tmp7A0)),((_tmp7A0->hd=
_tmp7D,((_tmp7A0->tl=0,_tmp7A0)))))),_tmp79F)));};}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo
old_inflow;union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_List_List*rvals;do{
this_all_changed=(env->fenv)->mt_place_set;{struct Cyc_Dict_Dict*_tmp7A1;env->all_changed=((
_tmp7A1=_region_malloc(env->r,sizeof(*_tmp7A1)),((_tmp7A1[0]=(env->fenv)->mt_place_set,
_tmp7A1))));}{union Cyc_CfFlowInfo_FlowInfo _tmp82;void*_tmp83;struct _tuple14
_tmp81=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;outflow=_tmp82;{struct Cyc_List_List*
_tmp7A2;rvals=((_tmp7A2=_region_malloc(rgn,sizeof(*_tmp7A2)),((_tmp7A2->hd=
_tmp83,((_tmp7A2->tl=0,_tmp7A2))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*
_tmp7A3;env->all_changed=((_tmp7A3=_region_malloc(env->r,sizeof(*_tmp7A3)),((
_tmp7A3[0]=(env->fenv)->mt_place_set,_tmp7A3))));}{union Cyc_CfFlowInfo_FlowInfo
_tmp87;void*_tmp88;struct _tuple14 _tmp86=Cyc_NewControlFlow_anal_Rexp(env,
others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp87=_tmp86.f1;_tmp88=
_tmp86.f2;{struct Cyc_List_List*_tmp7A4;rvals=((_tmp7A4=_region_malloc(rgn,
sizeof(*_tmp7A4)),((_tmp7A4->hd=_tmp88,((_tmp7A4->tl=rvals,_tmp7A4))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp87,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
old_inflow=inflow;inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,
inflow,outflow);};}while(!Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*_tmp7A5;env->all_changed=((
_tmp7A5=_region_malloc(env->r,sizeof(*_tmp7A5)),((_tmp7A5[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0),_tmp7A5))));}Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple20 _tmp7A6;return(_tmp7A6.f1=outflow,((_tmp7A6.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp7A6)));};};}
static struct _tuple14 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){struct
_RegionHandle _tmp8C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp8C;
_push_region(rgn);{union Cyc_CfFlowInfo_FlowInfo _tmp8E;struct Cyc_List_List*_tmp8F;
struct _tuple20 _tmp8D=Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,
0);_tmp8E=_tmp8D.f1;_tmp8F=_tmp8D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp90=_tmp8E;
struct _tuple13 _tmp91;struct Cyc_Dict_Dict _tmp92;int _tmp93;_LL40: if((_tmp90.ReachableFL).tag
!= 2)goto _LL42;_tmp91=(struct _tuple13)(_tmp90.ReachableFL).val;_tmp92=_tmp91.f1;
_LL41: for(0;_tmp8F != 0;(_tmp8F=_tmp8F->tl,es=((struct Cyc_List_List*)_check_null(
es))->tl)){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp92,(void*)_tmp8F->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7A9;void*_tmp7A8;(_tmp7A8=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp7A9="expression may not be initialized",
_tag_dyneither(_tmp7A9,sizeof(char),34))),_tag_dyneither(_tmp7A8,sizeof(void*),0)));}}
goto _LL3F;_LL42: if((_tmp90.BottomFL).tag != 1)goto _LL3F;_tmp93=(int)(_tmp90.BottomFL).val;
_LL43: goto _LL3F;_LL3F:;}{struct _tuple14 _tmp7AA;struct _tuple14 _tmp97=(_tmp7AA.f1=
_tmp8E,((_tmp7AA.f2=(env->fenv)->unknown_all,_tmp7AA)));_npop_handler(0);return
_tmp97;};};_pop_region(rgn);}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmp98=outflow;int _tmp99;struct _tuple13 _tmp9A;struct Cyc_Dict_Dict _tmp9B;struct
Cyc_List_List*_tmp9C;_LL45: if((_tmp98.BottomFL).tag != 1)goto _LL47;_tmp99=(int)(
_tmp98.BottomFL).val;_LL46: return outflow;_LL47: if((_tmp98.ReachableFL).tag != 2)
goto _LL44;_tmp9A=(struct _tuple13)(_tmp98.ReachableFL).val;_tmp9B=_tmp9A.f1;
_tmp9C=_tmp9A.f2;_LL48: {union Cyc_CfFlowInfo_AbsLVal _tmp9D=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,e)).f2;int _tmp9E;struct Cyc_CfFlowInfo_Place*_tmp9F;_LL4A: if((_tmp9D.UnknownL).tag
!= 2)goto _LL4C;_tmp9E=(int)(_tmp9D.UnknownL).val;_LL4B: return outflow;_LL4C: if((
_tmp9D.PlaceL).tag != 1)goto _LL49;_tmp9F=(struct Cyc_CfFlowInfo_Place*)(_tmp9D.PlaceL).val;
_LL4D: {void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
union Cyc_CfFlowInfo_FlowInfo _tmpA0=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp9B,env->all_changed,_tmp9F,nzval),_tmp9C);return _tmpA0;}
_LL49:;}_LL44:;}static struct _tuple16 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,
struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmpA1=outflow;int _tmpA2;struct _tuple13 _tmpA3;struct Cyc_Dict_Dict _tmpA4;struct
Cyc_List_List*_tmpA5;_LL4F: if((_tmpA1.BottomFL).tag != 1)goto _LL51;_tmpA2=(int)(
_tmpA1.BottomFL).val;_LL50: {struct _tuple16 _tmp7AB;return(_tmp7AB.f1=outflow,((
_tmp7AB.f2=outflow,_tmp7AB)));}_LL51: if((_tmpA1.ReachableFL).tag != 2)goto _LL4E;
_tmpA3=(struct _tuple13)(_tmpA1.ReachableFL).val;_tmpA4=_tmpA3.f1;_tmpA5=_tmpA3.f2;
_LL52: {union Cyc_CfFlowInfo_AbsLVal _tmpA7=(Cyc_NewControlFlow_anal_Lexp(env,
inflow,0,e)).f2;int _tmpA8;struct Cyc_CfFlowInfo_Place*_tmpA9;_LL54: if((_tmpA7.UnknownL).tag
!= 2)goto _LL56;_tmpA8=(int)(_tmpA7.UnknownL).val;_LL55: {struct _tuple16 _tmp7AC;
return(_tmp7AC.f1=outflow,((_tmp7AC.f2=outflow,_tmp7AC)));}_LL56: if((_tmpA7.PlaceL).tag
!= 1)goto _LL53;_tmpA9=(struct Cyc_CfFlowInfo_Place*)(_tmpA7.PlaceL).val;_LL57: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
struct _tuple16 _tmp7AD;return(_tmp7AD.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmpA4,env->all_changed,_tmpA9,nzval),_tmpA5),((_tmp7AD.f2=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA4,env->all_changed,_tmpA9,(env->fenv)->zero),
_tmpA5),_tmp7AD)));}_LL53:;}_LL4E:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple14 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
f,struct Cyc_Absyn_Exp*e,void*r){void*_tmpAE=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmpB0;void*
_tmpB1;struct Cyc_Absyn_PtrAtts _tmpB2;union Cyc_Absyn_Constraint*_tmpB3;union Cyc_Absyn_Constraint*
_tmpB4;_LL59: {struct Cyc_Absyn_PointerType_struct*_tmpAF=(struct Cyc_Absyn_PointerType_struct*)
_tmpAE;if(_tmpAF->tag != 5)goto _LL5B;else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpB0.elt_typ;
_tmpB2=_tmpB0.ptr_atts;_tmpB3=_tmpB2.bounds;_tmpB4=_tmpB2.zero_term;}}_LL5A: {
union Cyc_CfFlowInfo_FlowInfo _tmpB5=f;int _tmpB6;struct _tuple13 _tmpB7;struct Cyc_Dict_Dict
_tmpB8;struct Cyc_List_List*_tmpB9;_LL5E: if((_tmpB5.BottomFL).tag != 1)goto _LL60;
_tmpB6=(int)(_tmpB5.BottomFL).val;_LL5F: {struct _tuple14 _tmp7AE;return(_tmp7AE.f1=
f,((_tmp7AE.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB1,0,(env->fenv)->unknown_all),
_tmp7AE)));}_LL60: if((_tmpB5.ReachableFL).tag != 2)goto _LL5D;_tmpB7=(struct
_tuple13)(_tmpB5.ReachableFL).val;_tmpB8=_tmpB7.f1;_tmpB9=_tmpB7.f2;_LL61: if(Cyc_Tcutil_is_bound_one(
_tmpB3)){void*_tmpBB=r;struct Cyc_CfFlowInfo_Place*_tmpBF;enum Cyc_CfFlowInfo_InitLevel
_tmpC2;_LL63: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmpBC=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmpBB;if(_tmpBC->tag != 1)goto _LL65;}_LL64: goto _LL66;_LL65: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmpBD=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmpBB;if(_tmpBD->tag != 2)goto
_LL67;}_LL66:{void*_tmpC3=e->annot;struct Cyc_List_List*_tmpC5;_LL70: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmpC4=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmpC3;if(_tmpC4->tag != Cyc_CfFlowInfo_NotZero)
goto _LL72;else{_tmpC5=_tmpC4->f1;}}_LL71: if(!Cyc_CfFlowInfo_same_relns(_tmpB9,
_tmpC5))goto _LL73;goto _LL6F;_LL72:;_LL73:{void*_tmpC6=e->r;_LL75: {struct Cyc_Absyn_Subscript_e_struct*
_tmpC7=(struct Cyc_Absyn_Subscript_e_struct*)_tmpC6;if(_tmpC7->tag != 24)goto _LL77;}
_LL76:{struct Cyc_CfFlowInfo_NotZero_struct _tmp7B1;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp7B0;e->annot=(void*)((_tmp7B0=_cycalloc(sizeof(*_tmp7B0)),((_tmp7B0[0]=((
_tmp7B1.tag=Cyc_CfFlowInfo_NotZero,((_tmp7B1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB9),_tmp7B1)))),_tmp7B0))));}goto _LL74;_LL77:;_LL78: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL74;_LL74:;}goto _LL6F;_LL6F:;}goto _LL62;_LL67: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpBE=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpBB;if(_tmpBE->tag != 5)goto
_LL69;else{_tmpBF=_tmpBE->f1;}}_LL68:{void*_tmpCA=e->annot;struct Cyc_List_List*
_tmpCC;_LL7A: {struct Cyc_CfFlowInfo_NotZero_struct*_tmpCB=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpCA;if(_tmpCB->tag != Cyc_CfFlowInfo_NotZero)goto _LL7C;else{_tmpCC=_tmpCB->f1;}}
_LL7B: if(!Cyc_CfFlowInfo_same_relns(_tmpB9,_tmpCC))goto _LL7D;goto _LL79;_LL7C:;
_LL7D:{void*_tmpCD=e->r;_LL7F: {struct Cyc_Absyn_Subscript_e_struct*_tmpCE=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpCD;if(_tmpCE->tag != 24)goto _LL81;}_LL80:{
struct Cyc_CfFlowInfo_NotZero_struct _tmp7B4;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp7B3;e->annot=(void*)((_tmp7B3=_cycalloc(sizeof(*_tmp7B3)),((_tmp7B3[0]=((
_tmp7B4.tag=Cyc_CfFlowInfo_NotZero,((_tmp7B4.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB9),_tmp7B4)))),_tmp7B3))));}goto _LL7E;_LL81:;_LL82: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL7E;_LL7E:;}goto _LL79;_LL79:;}{struct _tuple14 _tmp7B5;return(_tmp7B5.f1=f,((
_tmp7B5.f2=Cyc_CfFlowInfo_lookup_place(_tmpB8,_tmpBF),_tmp7B5)));};_LL69: {
struct Cyc_CfFlowInfo_Zero_struct*_tmpC0=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmpBB;if(_tmpC0->tag != 0)goto _LL6B;}_LL6A: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp7B6;return(_tmp7B6.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7B6.f2=
Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB1,0,(env->fenv)->unknown_all),
_tmp7B6)));};_LL6B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpC1=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpBB;if(_tmpC1->tag != 3)goto _LL6D;else{_tmpC2=_tmpC1->f1;}}_LL6C: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmpC2);goto _LL6E;_LL6D:;_LL6E:{void*_tmpD3=e->r;_LL84: {struct
Cyc_Absyn_Subscript_e_struct*_tmpD4=(struct Cyc_Absyn_Subscript_e_struct*)_tmpD3;
if(_tmpD4->tag != 24)goto _LL86;}_LL85:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp7B9;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp7B8;e->annot=(void*)((_tmp7B8=
_cycalloc(sizeof(*_tmp7B8)),((_tmp7B8[0]=((_tmp7B9.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp7B9.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpB9),_tmp7B9)))),
_tmp7B8))));}goto _LL83;_LL86:;_LL87: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL83;_LL83:;}goto _LL62;_LL62:;}else{void*_tmpD7=e->annot;struct Cyc_List_List*
_tmpD9;_LL89: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpD8=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmpD7;if(_tmpD8->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL8B;else{_tmpD9=_tmpD8->f1;}}
_LL8A: if(!Cyc_CfFlowInfo_same_relns(_tmpB9,_tmpD9))goto _LL8C;goto _LL88;_LL8B:;
_LL8C:{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp7BC;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp7BB;e->annot=(void*)((_tmp7BB=_cycalloc(sizeof(*_tmp7BB)),((_tmp7BB[0]=((
_tmp7BC.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7BC.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB9),_tmp7BC)))),_tmp7BB))));}goto _LL88;_LL88:;}switch(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmpB8,r)){case Cyc_CfFlowInfo_NoneIL: _LL8D:{void*_tmpDC=r;_LL90: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmpDD=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpDC;if(_tmpDD->tag != 7)goto _LL92;}_LL91:{const char*_tmp7BF;void*_tmp7BE;(
_tmp7BE=0,Cyc_Tcutil_terr(e->loc,((_tmp7BF="attempt to use a unique pointer that has already been copied",
_tag_dyneither(_tmp7BF,sizeof(char),61))),_tag_dyneither(_tmp7BE,sizeof(void*),0)));}
goto _LL8F;_LL92:;_LL93: {const char*_tmp7C2;void*_tmp7C1;(_tmp7C1=0,Cyc_Tcutil_terr(
e->loc,((_tmp7C2="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp7C2,sizeof(char),46))),_tag_dyneither(_tmp7C1,sizeof(void*),0)));}_LL8F:;}
goto _LL8E;case Cyc_CfFlowInfo_AllIL: _LL8E: {struct _tuple14 _tmp7C3;return(_tmp7C3.f1=
f,((_tmp7C3.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB1,0,(env->fenv)->unknown_all),
_tmp7C3)));}case Cyc_CfFlowInfo_ThisIL: _LL94: {struct _tuple14 _tmp7C4;return(
_tmp7C4.f1=f,((_tmp7C4.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB1,0,(env->fenv)->unknown_none),
_tmp7C4)));}}_LL5D:;}_LL5B:;_LL5C: {struct Cyc_Core_Impossible_struct _tmp7CA;
const char*_tmp7C9;struct Cyc_Core_Impossible_struct*_tmp7C8;(int)_throw((void*)((
_tmp7C8=_cycalloc(sizeof(*_tmp7C8)),((_tmp7C8[0]=((_tmp7CA.tag=Cyc_Core_Impossible,((
_tmp7CA.f1=((_tmp7C9="right deref of non-pointer-type",_tag_dyneither(_tmp7C9,
sizeof(char),32))),_tmp7CA)))),_tmp7C8)))));}_LL58:;}static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpE7=e1->r;void*
_tmpE9;struct Cyc_Absyn_Vardecl*_tmpEB;void*_tmpED;struct Cyc_Absyn_Vardecl*_tmpEF;
void*_tmpF1;struct Cyc_Absyn_Vardecl*_tmpF3;void*_tmpF5;struct Cyc_Absyn_Vardecl*
_tmpF7;_LL97: {struct Cyc_Absyn_Var_e_struct*_tmpE8=(struct Cyc_Absyn_Var_e_struct*)
_tmpE7;if(_tmpE8->tag != 1)goto _LL99;else{_tmpE9=(void*)_tmpE8->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmpEA=(struct Cyc_Absyn_Pat_b_struct*)_tmpE9;if(_tmpEA->tag != 5)goto _LL99;else{
_tmpEB=_tmpEA->f1;}};}}_LL98: _tmpEF=_tmpEB;goto _LL9A;_LL99: {struct Cyc_Absyn_Var_e_struct*
_tmpEC=(struct Cyc_Absyn_Var_e_struct*)_tmpE7;if(_tmpEC->tag != 1)goto _LL9B;else{
_tmpED=(void*)_tmpEC->f2;{struct Cyc_Absyn_Local_b_struct*_tmpEE=(struct Cyc_Absyn_Local_b_struct*)
_tmpED;if(_tmpEE->tag != 4)goto _LL9B;else{_tmpEF=_tmpEE->f1;}};}}_LL9A: _tmpF3=
_tmpEF;goto _LL9C;_LL9B: {struct Cyc_Absyn_Var_e_struct*_tmpF0=(struct Cyc_Absyn_Var_e_struct*)
_tmpE7;if(_tmpF0->tag != 1)goto _LL9D;else{_tmpF1=(void*)_tmpF0->f2;{struct Cyc_Absyn_Param_b_struct*
_tmpF2=(struct Cyc_Absyn_Param_b_struct*)_tmpF1;if(_tmpF2->tag != 3)goto _LL9D;
else{_tmpF3=_tmpF2->f1;}};}}_LL9C: _tmpF7=_tmpF3;goto _LL9E;_LL9D: {struct Cyc_Absyn_Var_e_struct*
_tmpF4=(struct Cyc_Absyn_Var_e_struct*)_tmpE7;if(_tmpF4->tag != 1)goto _LL9F;else{
_tmpF5=(void*)_tmpF4->f2;{struct Cyc_Absyn_Global_b_struct*_tmpF6=(struct Cyc_Absyn_Global_b_struct*)
_tmpF5;if(_tmpF6->tag != 1)goto _LL9F;else{_tmpF7=_tmpF6->f1;}};}}_LL9E: if(!_tmpF7->escapes){
void*_tmpF8=e2->r;void*_tmpFA;struct Cyc_Absyn_Vardecl*_tmpFC;void*_tmpFE;struct
Cyc_Absyn_Vardecl*_tmp100;void*_tmp102;struct Cyc_Absyn_Vardecl*_tmp104;void*
_tmp106;struct Cyc_Absyn_Vardecl*_tmp108;_LLA2: {struct Cyc_Absyn_Var_e_struct*
_tmpF9=(struct Cyc_Absyn_Var_e_struct*)_tmpF8;if(_tmpF9->tag != 1)goto _LLA4;else{
_tmpFA=(void*)_tmpF9->f2;{struct Cyc_Absyn_Pat_b_struct*_tmpFB=(struct Cyc_Absyn_Pat_b_struct*)
_tmpFA;if(_tmpFB->tag != 5)goto _LLA4;else{_tmpFC=_tmpFB->f1;}};}}_LLA3: _tmp100=
_tmpFC;goto _LLA5;_LLA4: {struct Cyc_Absyn_Var_e_struct*_tmpFD=(struct Cyc_Absyn_Var_e_struct*)
_tmpF8;if(_tmpFD->tag != 1)goto _LLA6;else{_tmpFE=(void*)_tmpFD->f2;{struct Cyc_Absyn_Local_b_struct*
_tmpFF=(struct Cyc_Absyn_Local_b_struct*)_tmpFE;if(_tmpFF->tag != 4)goto _LLA6;
else{_tmp100=_tmpFF->f1;}};}}_LLA5: _tmp104=_tmp100;goto _LLA7;_LLA6: {struct Cyc_Absyn_Var_e_struct*
_tmp101=(struct Cyc_Absyn_Var_e_struct*)_tmpF8;if(_tmp101->tag != 1)goto _LLA8;
else{_tmp102=(void*)_tmp101->f2;{struct Cyc_Absyn_Param_b_struct*_tmp103=(struct
Cyc_Absyn_Param_b_struct*)_tmp102;if(_tmp103->tag != 3)goto _LLA8;else{_tmp104=
_tmp103->f1;}};}}_LLA7: _tmp108=_tmp104;goto _LLA9;_LLA8: {struct Cyc_Absyn_Var_e_struct*
_tmp105=(struct Cyc_Absyn_Var_e_struct*)_tmpF8;if(_tmp105->tag != 1)goto _LLAA;
else{_tmp106=(void*)_tmp105->f2;{struct Cyc_Absyn_Global_b_struct*_tmp107=(struct
Cyc_Absyn_Global_b_struct*)_tmp106;if(_tmp107->tag != 1)goto _LLAA;else{_tmp108=
_tmp107->f1;}};}}_LLA9: if(!_tmp108->escapes){int found=0;{struct Cyc_List_List*
_tmp109=relns;for(0;_tmp109 != 0;_tmp109=_tmp109->tl){struct Cyc_CfFlowInfo_Reln*
_tmp10A=(struct Cyc_CfFlowInfo_Reln*)_tmp109->hd;if(_tmp10A->vd == _tmp108){union
Cyc_CfFlowInfo_RelnOp _tmp10B=_tmp10A->rop;struct Cyc_Absyn_Vardecl*_tmp10C;_LLAD:
if((_tmp10B.LessNumelts).tag != 3)goto _LLAF;_tmp10C=(struct Cyc_Absyn_Vardecl*)(
_tmp10B.LessNumelts).val;if(!(_tmp10C == _tmpF7))goto _LLAF;_LLAE: return relns;
_LLAF:;_LLB0: continue;_LLAC:;}}}if(!found){struct Cyc_CfFlowInfo_Reln*_tmp7CD;
struct Cyc_List_List*_tmp7CC;return(_tmp7CC=_region_malloc(rgn,sizeof(*_tmp7CC)),((
_tmp7CC->hd=((_tmp7CD=_region_malloc(rgn,sizeof(*_tmp7CD)),((_tmp7CD->vd=_tmp108,((
_tmp7CD->rop=Cyc_CfFlowInfo_LessNumelts(_tmpF7),_tmp7CD)))))),((_tmp7CC->tl=
relns,_tmp7CC)))));}}return relns;_LLAA:;_LLAB: return relns;_LLA1:;}else{return
relns;}_LL9F:;_LLA0: return relns;_LL96:;}static struct _tuple14 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,unsigned int loc){union Cyc_CfFlowInfo_FlowInfo _tmp10F=outflow;int
_tmp110;struct _tuple13 _tmp111;struct Cyc_Dict_Dict _tmp112;struct Cyc_List_List*
_tmp113;_LLB2: if((_tmp10F.BottomFL).tag != 1)goto _LLB4;_tmp110=(int)(_tmp10F.BottomFL).val;
_LLB3: {struct _tuple14 _tmp7CE;return(_tmp7CE.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp7CE.f2=rval,_tmp7CE)));}_LLB4: if((_tmp10F.ReachableFL).tag != 2)goto _LLB1;
_tmp111=(struct _tuple13)(_tmp10F.ReachableFL).val;_tmp112=_tmp111.f1;_tmp113=
_tmp111.f2;_LLB5: {union Cyc_CfFlowInfo_AbsLVal _tmp115=lval;struct Cyc_CfFlowInfo_Place*
_tmp116;int _tmp117;_LLB7: if((_tmp115.PlaceL).tag != 1)goto _LLB9;_tmp116=(struct
Cyc_CfFlowInfo_Place*)(_tmp115.PlaceL).val;_LLB8: {struct Cyc_Dict_Dict _tmp118=
Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp112,env->all_changed,_tmp116,rval);
_tmp113=Cyc_CfFlowInfo_reln_assign_exp(fenv->r,_tmp113,lexp,rexp);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp118,_tmp113);if(Cyc_NewControlFlow_warn_lose_unique  && Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,(void*)((struct Cyc_Core_Opt*)_check_null(lexp->topt))->v)){void*
_tmp119=Cyc_CfFlowInfo_lookup_place(_tmp112,_tmp116);enum Cyc_CfFlowInfo_InitLevel
_tmp11B;_LLBC: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp11A=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp119;if(_tmp11A->tag != 3)goto _LLBE;else{_tmp11B=_tmp11A->f1;if(_tmp11B != Cyc_CfFlowInfo_NoneIL)
goto _LLBE;}}_LLBD: goto _LLBF;_LLBE: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp11C=(
struct Cyc_CfFlowInfo_Consumed_struct*)_tmp119;if(_tmp11C->tag != 7)goto _LLC0;}
_LLBF: goto _LLC1;_LLC0: {struct Cyc_CfFlowInfo_Zero_struct*_tmp11D=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp119;if(_tmp11D->tag != 0)goto _LLC2;}_LLC1: goto _LLBB;_LLC2:;_LLC3:{const char*
_tmp7D1;void*_tmp7D0;(_tmp7D0=0,Cyc_Tcutil_warn(lexp->loc,((_tmp7D1="assignment may overwrite unique pointer(s)",
_tag_dyneither(_tmp7D1,sizeof(char),43))),_tag_dyneither(_tmp7D0,sizeof(void*),0)));}
goto _LLBB;_LLBB:;}Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple14
_tmp7D2;return(_tmp7D2.f1=outflow,((_tmp7D2.f2=rval,_tmp7D2)));};}_LLB9: if((
_tmp115.UnknownL).tag != 2)goto _LLB6;_tmp117=(int)(_tmp115.UnknownL).val;_LLBA: {
struct _tuple14 _tmp7D3;return(_tmp7D3.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,
outflow,rval),((_tmp7D3.f2=rval,_tmp7D3)));}_LLB6:;}_LLB1:;}static int Cyc_NewControlFlow_is_local_var_rooted_path(
struct Cyc_Absyn_Exp*e){void*_tmp122=e->r;void*_tmp124;void*_tmp127;void*_tmp12A;
struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*
_tmp131;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp135;_LLC5: {struct
Cyc_Absyn_Var_e_struct*_tmp123=(struct Cyc_Absyn_Var_e_struct*)_tmp122;if(_tmp123->tag
!= 1)goto _LLC7;else{_tmp124=(void*)_tmp123->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp125=(struct Cyc_Absyn_Param_b_struct*)_tmp124;if(_tmp125->tag != 3)goto _LLC7;};}}
_LLC6: goto _LLC8;_LLC7: {struct Cyc_Absyn_Var_e_struct*_tmp126=(struct Cyc_Absyn_Var_e_struct*)
_tmp122;if(_tmp126->tag != 1)goto _LLC9;else{_tmp127=(void*)_tmp126->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp128=(struct Cyc_Absyn_Local_b_struct*)_tmp127;if(_tmp128->tag != 4)goto _LLC9;};}}
_LLC8: goto _LLCA;_LLC9: {struct Cyc_Absyn_Var_e_struct*_tmp129=(struct Cyc_Absyn_Var_e_struct*)
_tmp122;if(_tmp129->tag != 1)goto _LLCB;else{_tmp12A=(void*)_tmp129->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp12B=(struct Cyc_Absyn_Pat_b_struct*)_tmp12A;if(_tmp12B->tag != 5)goto _LLCB;};}}
_LLCA: return 1;_LLCB: {struct Cyc_Absyn_Deref_e_struct*_tmp12C=(struct Cyc_Absyn_Deref_e_struct*)
_tmp122;if(_tmp12C->tag != 21)goto _LLCD;else{_tmp12D=_tmp12C->f1;}}_LLCC: _tmp12F=
_tmp12D;goto _LLCE;_LLCD: {struct Cyc_Absyn_AggrMember_e_struct*_tmp12E=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp122;if(_tmp12E->tag != 22)goto _LLCF;else{_tmp12F=_tmp12E->f1;}}_LLCE: _tmp131=
_tmp12F;goto _LLD0;_LLCF: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp130=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp122;if(_tmp130->tag != 23)goto _LLD1;else{_tmp131=_tmp130->f1;}}_LLD0: return
Cyc_NewControlFlow_is_local_var_rooted_path(_tmp131);_LLD1: {struct Cyc_Absyn_Subscript_e_struct*
_tmp132=(struct Cyc_Absyn_Subscript_e_struct*)_tmp122;if(_tmp132->tag != 24)goto
_LLD3;else{_tmp133=_tmp132->f1;}}_LLD2: {void*_tmp136=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp133->topt))->v);_LLD8: {struct Cyc_Absyn_TupleType_struct*
_tmp137=(struct Cyc_Absyn_TupleType_struct*)_tmp136;if(_tmp137->tag != 10)goto
_LLDA;}_LLD9: return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp133);_LLDA:;
_LLDB: return 0;_LLD7:;}_LLD3: {struct Cyc_Absyn_Cast_e_struct*_tmp134=(struct Cyc_Absyn_Cast_e_struct*)
_tmp122;if(_tmp134->tag != 15)goto _LLD5;else{_tmp135=_tmp134->f2;}}_LLD4: return
Cyc_NewControlFlow_is_local_var_rooted_path(_tmp135);_LLD5:;_LLD6: return 0;_LLC4:;}
static struct _tuple14 Cyc_NewControlFlow_anal_stmt_exp(struct Cyc_NewControlFlow_AnalEnv*
env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_CfFlowInfo_FlowEnv*_tmp138=env->fenv;while(1){union Cyc_CfFlowInfo_FlowInfo*
_tmp13A;struct _tuple17 _tmp139=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);
_tmp13A=_tmp139.f2;inflow=*_tmp13A;{void*_tmp13B=s->r;struct Cyc_Absyn_Stmt*
_tmp13D;struct Cyc_Absyn_Stmt*_tmp13E;struct Cyc_Absyn_Decl*_tmp140;struct Cyc_Absyn_Decl
_tmp141;void*_tmp142;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Tvar*_tmp145;
struct Cyc_Absyn_Vardecl*_tmp146;struct Cyc_Absyn_Stmt*_tmp147;struct Cyc_Absyn_Decl*
_tmp149;struct Cyc_Absyn_Stmt*_tmp14A;struct Cyc_Absyn_Exp*_tmp14C;_LLDD: {struct
Cyc_Absyn_Seq_s_struct*_tmp13C=(struct Cyc_Absyn_Seq_s_struct*)_tmp13B;if(_tmp13C->tag
!= 2)goto _LLDF;else{_tmp13D=_tmp13C->f1;_tmp13E=_tmp13C->f2;}}_LLDE: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp13D);s=_tmp13E;goto _LLDC;_LLDF: {struct Cyc_Absyn_Decl_s_struct*
_tmp13F=(struct Cyc_Absyn_Decl_s_struct*)_tmp13B;if(_tmp13F->tag != 12)goto _LLE1;
else{_tmp140=_tmp13F->f1;_tmp141=*_tmp140;_tmp142=_tmp141.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp143=(struct Cyc_Absyn_Alias_d_struct*)_tmp142;if(_tmp143->tag != 5)goto _LLE1;
else{_tmp144=_tmp143->f1;_tmp145=_tmp143->f2;_tmp146=_tmp143->f3;}};_tmp147=
_tmp13F->f2;}}_LLE0: {union Cyc_CfFlowInfo_FlowInfo _tmp14E;union Cyc_CfFlowInfo_AbsLVal
_tmp14F;struct _tuple15 _tmp14D=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp144);
_tmp14E=_tmp14D.f1;_tmp14F=_tmp14D.f2;{struct _tuple15 _tmp7D4;struct _tuple15
_tmp151=(_tmp7D4.f1=_tmp14E,((_tmp7D4.f2=_tmp14F,_tmp7D4)));union Cyc_CfFlowInfo_FlowInfo
_tmp152;struct _tuple13 _tmp153;struct Cyc_Dict_Dict _tmp154;struct Cyc_List_List*
_tmp155;union Cyc_CfFlowInfo_AbsLVal _tmp156;struct Cyc_CfFlowInfo_Place*_tmp157;
union Cyc_CfFlowInfo_FlowInfo _tmp158;int _tmp159;_LLE8: _tmp152=_tmp151.f1;if((
_tmp152.ReachableFL).tag != 2)goto _LLEA;_tmp153=(struct _tuple13)(_tmp152.ReachableFL).val;
_tmp154=_tmp153.f1;_tmp155=_tmp153.f2;_tmp156=_tmp151.f2;if((_tmp156.PlaceL).tag
!= 1)goto _LLEA;_tmp157=(struct Cyc_CfFlowInfo_Place*)(_tmp156.PlaceL).val;_LLE9: {
void*_tmp15A=Cyc_CfFlowInfo_lookup_place(_tmp154,_tmp157);void*t=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp144->topt))->v;{void*_tmp15B=_tmp144->r;struct Cyc_Absyn_Exp*
_tmp15D;_LLEF: {struct Cyc_Absyn_Cast_e_struct*_tmp15C=(struct Cyc_Absyn_Cast_e_struct*)
_tmp15B;if(_tmp15C->tag != 15)goto _LLF1;else{_tmp15D=_tmp15C->f2;}}_LLF0: t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp15D->topt))->v;goto _LLEE;_LLF1:;_LLF2: goto
_LLEE;_LLEE:;}{void*_tmp15E=Cyc_CfFlowInfo_make_unique_consumed(_tmp138,t,
_tmp144,env->iteration_num,_tmp15A);_tmp154=Cyc_CfFlowInfo_assign_place(_tmp138,
_tmp144->loc,_tmp154,env->all_changed,_tmp157,_tmp15E);_tmp14E=Cyc_CfFlowInfo_ReachableFL(
_tmp154,_tmp155);{void*leaf;switch(Cyc_CfFlowInfo_initlevel(_tmp138,_tmp154,
_tmp15A)){case Cyc_CfFlowInfo_AllIL: _LLF3: leaf=_tmp138->unknown_all;break;case Cyc_CfFlowInfo_NoneIL:
_LLF4: leaf=_tmp138->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LLF5: leaf=
_tmp138->unknown_this;break;}{void*_tmp15F=Cyc_CfFlowInfo_typ_to_absrval(_tmp138,
t,0,leaf);_tmp14E=Cyc_NewControlFlow_use_Rval(env,_tmp144->loc,_tmp14E,_tmp15A);{
struct Cyc_List_List _tmp7D5;struct Cyc_List_List _tmp160=(_tmp7D5.hd=_tmp146,((
_tmp7D5.tl=0,_tmp7D5)));_tmp14E=Cyc_NewControlFlow_add_vars(_tmp138,_tmp14E,(
struct Cyc_List_List*)& _tmp160,_tmp138->unknown_all,s->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp162;void*_tmp163;struct _tuple14 _tmp161=Cyc_NewControlFlow_anal_stmt_exp(env,
copy_ctxt,_tmp14E,_tmp147);_tmp162=_tmp161.f1;_tmp163=_tmp161.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp164=_tmp162;struct _tuple13 _tmp165;struct Cyc_Dict_Dict _tmp166;struct Cyc_List_List*
_tmp167;_LLF8: if((_tmp164.ReachableFL).tag != 2)goto _LLFA;_tmp165=(struct _tuple13)(
_tmp164.ReachableFL).val;_tmp166=_tmp165.f1;_tmp167=_tmp165.f2;_LLF9: _tmp166=Cyc_CfFlowInfo_assign_place(
_tmp138,s->loc,_tmp166,env->all_changed,_tmp157,_tmp15F);{union Cyc_CfFlowInfo_FlowInfo
_tmp168=Cyc_CfFlowInfo_ReachableFL(_tmp166,_tmp167);struct _tuple14 _tmp7D6;return(
_tmp7D6.f1=_tmp168,((_tmp7D6.f2=_tmp163,_tmp7D6)));};_LLFA:;_LLFB: {struct
_tuple14 _tmp7D7;return(_tmp7D7.f1=_tmp162,((_tmp7D7.f2=_tmp163,_tmp7D7)));}_LLF7:;};};};};};};}
_LLEA: _tmp158=_tmp151.f1;if((_tmp158.BottomFL).tag != 1)goto _LLEC;_tmp159=(int)(
_tmp158.BottomFL).val;_LLEB: {struct _tuple14 _tmp7D8;return(_tmp7D8.f1=_tmp14E,((
_tmp7D8.f2=_tmp138->unknown_all,_tmp7D8)));}_LLEC:;_LLED:{const char*_tmp7DB;void*
_tmp7DA;(_tmp7DA=0,Cyc_Tcutil_terr(_tmp144->loc,((_tmp7DB="bad alias expression--no unique path found",
_tag_dyneither(_tmp7DB,sizeof(char),43))),_tag_dyneither(_tmp7DA,sizeof(void*),0)));}{
struct _tuple14 _tmp7DC;return(_tmp7DC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7DC.f2=
_tmp138->unknown_all,_tmp7DC)));};_LLE7:;};}_LLE1: {struct Cyc_Absyn_Decl_s_struct*
_tmp148=(struct Cyc_Absyn_Decl_s_struct*)_tmp13B;if(_tmp148->tag != 12)goto _LLE3;
else{_tmp149=_tmp148->f1;_tmp14A=_tmp148->f2;}}_LLE2: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp149);s=_tmp14A;goto _LLDC;_LLE3: {struct Cyc_Absyn_Exp_s_struct*
_tmp14B=(struct Cyc_Absyn_Exp_s_struct*)_tmp13B;if(_tmp14B->tag != 1)goto _LLE5;
else{_tmp14C=_tmp14B->f1;}}_LLE4: return Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp14C);_LLE5:;_LLE6: {struct Cyc_Core_Impossible_struct _tmp7E2;
const char*_tmp7E1;struct Cyc_Core_Impossible_struct*_tmp7E0;(int)_throw((void*)((
_tmp7E0=_cycalloc(sizeof(*_tmp7E0)),((_tmp7E0[0]=((_tmp7E2.tag=Cyc_Core_Impossible,((
_tmp7E2.f1=((_tmp7E1="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp7E1,
sizeof(char),33))),_tmp7E2)))),_tmp7E0)))));}_LLDC:;};}}struct _tuple21{union Cyc_CfFlowInfo_AbsLVal
f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple22{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);
static void _tmp883(unsigned int*_tmp882,unsigned int*_tmp881,void***_tmp87F,
struct Cyc_List_List**_tmp363){for(*_tmp882=0;*_tmp882 < *_tmp881;(*_tmp882)++){
void*_tmp87D;(*_tmp87F)[*_tmp882]=((_tmp87D=(void*)((struct Cyc_List_List*)
_check_null(*_tmp363))->hd,((*_tmp363=(*_tmp363)->tl,_tmp87D))));}}static struct
_tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp173=env->fenv;struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;{union Cyc_CfFlowInfo_FlowInfo
_tmp174=inflow;int _tmp175;struct _tuple13 _tmp176;struct Cyc_Dict_Dict _tmp177;
struct Cyc_List_List*_tmp178;_LLFD: if((_tmp174.BottomFL).tag != 1)goto _LLFF;
_tmp175=(int)(_tmp174.BottomFL).val;_LLFE: {struct _tuple14 _tmp7E3;return(_tmp7E3.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp7E3.f2=_tmp173->unknown_all,_tmp7E3)));}_LLFF:
if((_tmp174.ReachableFL).tag != 2)goto _LLFC;_tmp176=(struct _tuple13)(_tmp174.ReachableFL).val;
_tmp177=_tmp176.f1;_tmp178=_tmp176.f2;_LL100: d=_tmp177;relns=_tmp178;_LLFC:;}if((
copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){union Cyc_CfFlowInfo_FlowInfo
_tmp17B;union Cyc_CfFlowInfo_AbsLVal _tmp17C;struct _tuple15 _tmp17A=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,e);_tmp17B=_tmp17A.f1;_tmp17C=_tmp17A.f2;{struct _tuple15 _tmp7E4;
struct _tuple15 _tmp17E=(_tmp7E4.f1=_tmp17B,((_tmp7E4.f2=_tmp17C,_tmp7E4)));union
Cyc_CfFlowInfo_FlowInfo _tmp17F;struct _tuple13 _tmp180;struct Cyc_Dict_Dict _tmp181;
struct Cyc_List_List*_tmp182;union Cyc_CfFlowInfo_AbsLVal _tmp183;struct Cyc_CfFlowInfo_Place*
_tmp184;_LL102: _tmp17F=_tmp17E.f1;if((_tmp17F.ReachableFL).tag != 2)goto _LL104;
_tmp180=(struct _tuple13)(_tmp17F.ReachableFL).val;_tmp181=_tmp180.f1;_tmp182=
_tmp180.f2;_tmp183=_tmp17E.f2;if((_tmp183.PlaceL).tag != 1)goto _LL104;_tmp184=(
struct Cyc_CfFlowInfo_Place*)(_tmp183.PlaceL).val;_LL103: {void*_tmp185=Cyc_CfFlowInfo_lookup_place(
_tmp181,_tmp184);int needs_unconsume=0;if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,
_tmp185,& needs_unconsume)){{const char*_tmp7E7;void*_tmp7E6;(_tmp7E6=0,Cyc_Tcutil_terr(
e->loc,((_tmp7E7="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp7E7,sizeof(char),71))),_tag_dyneither(_tmp7E6,sizeof(void*),0)));}{
struct _tuple14 _tmp7E8;return(_tmp7E8.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7E8.f2=
_tmp173->unknown_all,_tmp7E8)));};}else{if(needs_unconsume){struct _tuple14
_tmp7E9;return(_tmp7E9.f1=_tmp17B,((_tmp7E9.f2=Cyc_CfFlowInfo_make_unique_unconsumed(
_tmp173,_tmp185),_tmp7E9)));}else{void*_tmp18A=Cyc_CfFlowInfo_make_unique_consumed(
_tmp173,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,e,env->iteration_num,
_tmp185);struct Cyc_Dict_Dict _tmp18B=Cyc_CfFlowInfo_assign_place(_tmp173,e->loc,
_tmp181,env->all_changed,_tmp184,_tmp18A);struct _tuple14 _tmp7EA;return(_tmp7EA.f1=
Cyc_CfFlowInfo_ReachableFL(_tmp18B,_tmp182),((_tmp7EA.f2=_tmp185,_tmp7EA)));}}}
_LL104:;_LL105: goto _LL101;_LL101:;};}{void*_tmp18D=e->r;struct Cyc_Absyn_Exp*
_tmp18F;enum Cyc_Absyn_Coercion _tmp190;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*
_tmp194;struct Cyc_Absyn_Exp*_tmp196;union Cyc_Absyn_Cnst _tmp198;int _tmp199;union
Cyc_Absyn_Cnst _tmp19B;struct _tuple6 _tmp19C;int _tmp19D;union Cyc_Absyn_Cnst _tmp19F;
struct _tuple6 _tmp1A0;void*_tmp1A2;struct Cyc_List_List*_tmp1A5;void*_tmp1AD;void*
_tmp1B0;struct Cyc_Absyn_Vardecl*_tmp1B2;void*_tmp1B4;struct Cyc_Absyn_Vardecl*
_tmp1B6;void*_tmp1B8;struct Cyc_Absyn_Vardecl*_tmp1BA;enum Cyc_Absyn_Primop
_tmp1BC;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Exp*
_tmp1C1;struct Cyc_Core_Opt*_tmp1C2;struct Cyc_Core_Opt _tmp1C3;struct Cyc_Absyn_Exp*
_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_Core_Opt*_tmp1C7;struct Cyc_Absyn_Exp*
_tmp1C8;struct Cyc_Absyn_Exp*_tmp1CA;struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_Absyn_Exp*
_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CF;struct Cyc_List_List*_tmp1D0;struct Cyc_Absyn_MallocInfo
_tmp1D2;int _tmp1D3;struct Cyc_Absyn_Exp*_tmp1D4;void**_tmp1D5;struct Cyc_Absyn_Exp*
_tmp1D6;int _tmp1D7;struct Cyc_Absyn_Exp*_tmp1D9;struct Cyc_Absyn_Exp*_tmp1DA;
struct Cyc_Absyn_Exp*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DD;struct Cyc_Absyn_Exp*
_tmp1DF;struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_Absyn_Exp*_tmp1E3;struct
_dyneither_ptr*_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E6;struct _dyneither_ptr*_tmp1E7;
struct Cyc_Absyn_Exp*_tmp1E9;struct _dyneither_ptr*_tmp1EA;struct Cyc_Absyn_Exp*
_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*
_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F3;struct Cyc_Absyn_Exp*
_tmp1F4;struct Cyc_Absyn_Exp*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F7;struct Cyc_List_List*
_tmp1F9;struct Cyc_Absyn_Datatypedecl*_tmp1FA;struct Cyc_List_List*_tmp1FC;struct
Cyc_List_List*_tmp1FE;struct Cyc_List_List*_tmp200;struct Cyc_Absyn_Aggrdecl*
_tmp201;struct Cyc_Absyn_Aggrdecl _tmp202;enum Cyc_Absyn_AggrKind _tmp203;struct Cyc_Absyn_AggrdeclImpl*
_tmp204;struct Cyc_Absyn_AggrdeclImpl _tmp205;struct Cyc_List_List*_tmp206;struct
Cyc_List_List*_tmp209;struct Cyc_Absyn_Vardecl*_tmp20B;struct Cyc_Absyn_Exp*
_tmp20C;struct Cyc_Absyn_Exp*_tmp20D;int _tmp20E;struct Cyc_Absyn_Stmt*_tmp210;void*
_tmp212;_LL107: {struct Cyc_Absyn_Cast_e_struct*_tmp18E=(struct Cyc_Absyn_Cast_e_struct*)
_tmp18D;if(_tmp18E->tag != 15)goto _LL109;else{_tmp18F=_tmp18E->f2;_tmp190=_tmp18E->f4;
if(_tmp190 != Cyc_Absyn_NonNull_to_Null)goto _LL109;}}_LL108: {union Cyc_CfFlowInfo_FlowInfo
_tmp21B;void*_tmp21C;struct _tuple14 _tmp21A=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp18F);_tmp21B=_tmp21A.f1;_tmp21C=_tmp21A.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp21E;void*_tmp21F;struct _tuple14 _tmp21D=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp21B,_tmp18F,_tmp21C);_tmp21E=_tmp21D.f1;_tmp21F=_tmp21D.f2;{struct
_tuple14 _tmp7EB;return(_tmp7EB.f1=_tmp21E,((_tmp7EB.f2=_tmp21C,_tmp7EB)));};};}
_LL109: {struct Cyc_Absyn_Cast_e_struct*_tmp191=(struct Cyc_Absyn_Cast_e_struct*)
_tmp18D;if(_tmp191->tag != 15)goto _LL10B;else{_tmp192=_tmp191->f2;}}_LL10A:
_tmp194=_tmp192;goto _LL10C;_LL10B: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp193=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp18D;if(_tmp193->tag != 13)
goto _LL10D;else{_tmp194=_tmp193->f1;}}_LL10C: _tmp196=_tmp194;goto _LL10E;_LL10D: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp195=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp18D;if(_tmp195->tag != 14)goto _LL10F;else{_tmp196=_tmp195->f1;}}_LL10E: return
Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp196);_LL10F: {struct Cyc_Absyn_Const_e_struct*
_tmp197=(struct Cyc_Absyn_Const_e_struct*)_tmp18D;if(_tmp197->tag != 0)goto _LL111;
else{_tmp198=_tmp197->f1;if((_tmp198.Null_c).tag != 1)goto _LL111;_tmp199=(int)(
_tmp198.Null_c).val;}}_LL110: goto _LL112;_LL111: {struct Cyc_Absyn_Const_e_struct*
_tmp19A=(struct Cyc_Absyn_Const_e_struct*)_tmp18D;if(_tmp19A->tag != 0)goto _LL113;
else{_tmp19B=_tmp19A->f1;if((_tmp19B.Int_c).tag != 5)goto _LL113;_tmp19C=(struct
_tuple6)(_tmp19B.Int_c).val;_tmp19D=_tmp19C.f2;if(_tmp19D != 0)goto _LL113;}}
_LL112: {struct _tuple14 _tmp7EC;return(_tmp7EC.f1=inflow,((_tmp7EC.f2=_tmp173->zero,
_tmp7EC)));}_LL113: {struct Cyc_Absyn_Const_e_struct*_tmp19E=(struct Cyc_Absyn_Const_e_struct*)
_tmp18D;if(_tmp19E->tag != 0)goto _LL115;else{_tmp19F=_tmp19E->f1;if((_tmp19F.Int_c).tag
!= 5)goto _LL115;_tmp1A0=(struct _tuple6)(_tmp19F.Int_c).val;}}_LL114: goto _LL116;
_LL115: {struct Cyc_Absyn_Var_e_struct*_tmp1A1=(struct Cyc_Absyn_Var_e_struct*)
_tmp18D;if(_tmp1A1->tag != 1)goto _LL117;else{_tmp1A2=(void*)_tmp1A1->f2;{struct
Cyc_Absyn_Funname_b_struct*_tmp1A3=(struct Cyc_Absyn_Funname_b_struct*)_tmp1A2;
if(_tmp1A3->tag != 2)goto _LL117;};}}_LL116: {struct _tuple14 _tmp7ED;return(_tmp7ED.f1=
inflow,((_tmp7ED.f2=_tmp173->notzeroall,_tmp7ED)));}_LL117: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1A4=(struct Cyc_Absyn_Datatype_e_struct*)_tmp18D;if(_tmp1A4->tag != 31)goto
_LL119;else{_tmp1A5=_tmp1A4->f1;if(_tmp1A5 != 0)goto _LL119;}}_LL118: goto _LL11A;
_LL119: {struct Cyc_Absyn_Const_e_struct*_tmp1A6=(struct Cyc_Absyn_Const_e_struct*)
_tmp18D;if(_tmp1A6->tag != 0)goto _LL11B;}_LL11A: goto _LL11C;_LL11B: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp1A7=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp18D;if(_tmp1A7->tag != 19)goto
_LL11D;}_LL11C: goto _LL11E;_LL11D: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp1A8=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp18D;if(_tmp1A8->tag != 18)goto _LL11F;}
_LL11E: goto _LL120;_LL11F: {struct Cyc_Absyn_Offsetof_e_struct*_tmp1A9=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp18D;if(_tmp1A9->tag != 20)goto _LL121;}_LL120: goto _LL122;_LL121: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp1AA=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp18D;if(_tmp1AA->tag != 33)goto
_LL123;}_LL122: goto _LL124;_LL123: {struct Cyc_Absyn_Enum_e_struct*_tmp1AB=(struct
Cyc_Absyn_Enum_e_struct*)_tmp18D;if(_tmp1AB->tag != 32)goto _LL125;}_LL124: {
struct _tuple14 _tmp7EE;return(_tmp7EE.f1=inflow,((_tmp7EE.f2=_tmp173->unknown_all,
_tmp7EE)));}_LL125: {struct Cyc_Absyn_Var_e_struct*_tmp1AC=(struct Cyc_Absyn_Var_e_struct*)
_tmp18D;if(_tmp1AC->tag != 1)goto _LL127;else{_tmp1AD=(void*)_tmp1AC->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp1AE=(struct Cyc_Absyn_Global_b_struct*)_tmp1AD;if(
_tmp1AE->tag != 1)goto _LL127;};}}_LL126: {struct _tuple14 _tmp7EF;return(_tmp7EF.f1=
inflow,((_tmp7EF.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0,_tmp173->unknown_all),_tmp7EF)));}_LL127: {struct Cyc_Absyn_Var_e_struct*
_tmp1AF=(struct Cyc_Absyn_Var_e_struct*)_tmp18D;if(_tmp1AF->tag != 1)goto _LL129;
else{_tmp1B0=(void*)_tmp1AF->f2;{struct Cyc_Absyn_Param_b_struct*_tmp1B1=(struct
Cyc_Absyn_Param_b_struct*)_tmp1B0;if(_tmp1B1->tag != 3)goto _LL129;else{_tmp1B2=
_tmp1B1->f1;}};}}_LL128: _tmp1B6=_tmp1B2;goto _LL12A;_LL129: {struct Cyc_Absyn_Var_e_struct*
_tmp1B3=(struct Cyc_Absyn_Var_e_struct*)_tmp18D;if(_tmp1B3->tag != 1)goto _LL12B;
else{_tmp1B4=(void*)_tmp1B3->f2;{struct Cyc_Absyn_Local_b_struct*_tmp1B5=(struct
Cyc_Absyn_Local_b_struct*)_tmp1B4;if(_tmp1B5->tag != 4)goto _LL12B;else{_tmp1B6=
_tmp1B5->f1;}};}}_LL12A: _tmp1BA=_tmp1B6;goto _LL12C;_LL12B: {struct Cyc_Absyn_Var_e_struct*
_tmp1B7=(struct Cyc_Absyn_Var_e_struct*)_tmp18D;if(_tmp1B7->tag != 1)goto _LL12D;
else{_tmp1B8=(void*)_tmp1B7->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp1B9=(struct Cyc_Absyn_Pat_b_struct*)
_tmp1B8;if(_tmp1B9->tag != 5)goto _LL12D;else{_tmp1BA=_tmp1B9->f1;}};}}_LL12C: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp7F5;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp7F4;struct _tuple14 _tmp7F3;return(_tmp7F3.f1=inflow,((_tmp7F3.f2=((void*(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,(void*)((_tmp7F5=_region_malloc(
env->r,sizeof(*_tmp7F5)),((_tmp7F5[0]=((_tmp7F4.tag=0,((_tmp7F4.f1=_tmp1BA,
_tmp7F4)))),_tmp7F5))))),_tmp7F3)));}_LL12D: {struct Cyc_Absyn_Primop_e_struct*
_tmp1BB=(struct Cyc_Absyn_Primop_e_struct*)_tmp18D;if(_tmp1BB->tag != 3)goto _LL12F;
else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BB->f2;}}_LL12E: {union Cyc_CfFlowInfo_FlowInfo
_tmp229;void*_tmp22A;struct _tuple14 _tmp228=Cyc_NewControlFlow_anal_use_ints(env,
inflow,_tmp1BD);_tmp229=_tmp228.f1;_tmp22A=_tmp228.f2;{struct _tuple14 _tmp7F6;
return(_tmp7F6.f1=_tmp229,((_tmp7F6.f2=_tmp22A,_tmp7F6)));};}_LL12F: {struct Cyc_Absyn_Increment_e_struct*
_tmp1BE=(struct Cyc_Absyn_Increment_e_struct*)_tmp18D;if(_tmp1BE->tag != 5)goto
_LL131;else{_tmp1BF=_tmp1BE->f1;}}_LL130: {struct Cyc_List_List _tmp7F7;struct Cyc_List_List
_tmp22C=(_tmp7F7.hd=_tmp1BF,((_tmp7F7.tl=0,_tmp7F7)));union Cyc_CfFlowInfo_FlowInfo
_tmp22E;struct _tuple14 _tmp22D=Cyc_NewControlFlow_anal_use_ints(env,inflow,(
struct Cyc_List_List*)& _tmp22C);_tmp22E=_tmp22D.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp230;struct _tuple15 _tmp22F=Cyc_NewControlFlow_anal_Lexp(env,_tmp22E,0,_tmp1BF);
_tmp230=_tmp22F.f2;{struct _tuple21 _tmp7F8;struct _tuple21 _tmp232=(_tmp7F8.f1=
_tmp230,((_tmp7F8.f2=_tmp22E,_tmp7F8)));union Cyc_CfFlowInfo_AbsLVal _tmp233;
struct Cyc_CfFlowInfo_Place*_tmp234;union Cyc_CfFlowInfo_FlowInfo _tmp235;struct
_tuple13 _tmp236;struct Cyc_Dict_Dict _tmp237;struct Cyc_List_List*_tmp238;_LL172:
_tmp233=_tmp232.f1;if((_tmp233.PlaceL).tag != 1)goto _LL174;_tmp234=(struct Cyc_CfFlowInfo_Place*)(
_tmp233.PlaceL).val;_tmp235=_tmp232.f2;if((_tmp235.ReachableFL).tag != 2)goto
_LL174;_tmp236=(struct _tuple13)(_tmp235.ReachableFL).val;_tmp237=_tmp236.f1;
_tmp238=_tmp236.f2;_LL173: _tmp238=Cyc_CfFlowInfo_reln_kill_exp(_tmp173->r,
_tmp238,_tmp1BF);_tmp22E=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
_tmp173,_tmp1BF->loc,_tmp237,env->all_changed,_tmp234,_tmp173->unknown_all),
_tmp238);goto _LL171;_LL174:;_LL175: goto _LL171;_LL171:;}{struct _tuple14 _tmp7F9;
return(_tmp7F9.f1=_tmp22E,((_tmp7F9.f2=_tmp173->unknown_all,_tmp7F9)));};};}
_LL131: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1C0=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp18D;if(_tmp1C0->tag != 4)goto _LL133;else{_tmp1C1=_tmp1C0->f1;_tmp1C2=_tmp1C0->f2;
if(_tmp1C2 == 0)goto _LL133;_tmp1C3=*_tmp1C2;_tmp1C4=_tmp1C0->f3;}}_LL132: if(
copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){{const char*_tmp7FC;void*_tmp7FB;(_tmp7FB=0,Cyc_Tcutil_terr(
e->loc,((_tmp7FC="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7FC,sizeof(char),58))),_tag_dyneither(_tmp7FB,sizeof(void*),0)));}{
struct _tuple14 _tmp7FD;return(_tmp7FD.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7FD.f2=
_tmp173->unknown_all,_tmp7FD)));};}{struct Cyc_List_List _tmp7FE;struct Cyc_List_List
_tmp23E=(_tmp7FE.hd=_tmp1C4,((_tmp7FE.tl=0,_tmp7FE)));struct Cyc_List_List _tmp7FF;
struct Cyc_List_List _tmp23F=(_tmp7FF.hd=_tmp1C1,((_tmp7FF.tl=(struct Cyc_List_List*)&
_tmp23E,_tmp7FF)));union Cyc_CfFlowInfo_FlowInfo _tmp241;struct _tuple14 _tmp240=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp23F);_tmp241=_tmp240.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp243;struct _tuple15 _tmp242=Cyc_NewControlFlow_anal_Lexp(env,_tmp241,0,_tmp1C1);
_tmp243=_tmp242.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp244=_tmp241;struct _tuple13
_tmp245;struct Cyc_Dict_Dict _tmp246;struct Cyc_List_List*_tmp247;_LL177: if((
_tmp244.ReachableFL).tag != 2)goto _LL179;_tmp245=(struct _tuple13)(_tmp244.ReachableFL).val;
_tmp246=_tmp245.f1;_tmp247=_tmp245.f2;_LL178:{union Cyc_CfFlowInfo_AbsLVal _tmp248=
_tmp243;struct Cyc_CfFlowInfo_Place*_tmp249;int _tmp24A;_LL17C: if((_tmp248.PlaceL).tag
!= 1)goto _LL17E;_tmp249=(struct Cyc_CfFlowInfo_Place*)(_tmp248.PlaceL).val;_LL17D:
_tmp247=Cyc_CfFlowInfo_reln_kill_exp(_tmp173->r,_tmp247,_tmp1C1);_tmp246=Cyc_CfFlowInfo_assign_place(
_tmp173,_tmp1C1->loc,_tmp246,env->all_changed,_tmp249,_tmp173->unknown_all);
_tmp241=Cyc_CfFlowInfo_ReachableFL(_tmp246,_tmp247);goto _LL17B;_LL17E: if((
_tmp248.UnknownL).tag != 2)goto _LL17B;_tmp24A=(int)(_tmp248.UnknownL).val;_LL17F:
goto _LL17B;_LL17B:;}goto _LL176;_LL179:;_LL17A: goto _LL176;_LL176:;}{struct
_tuple14 _tmp800;return(_tmp800.f1=_tmp241,((_tmp800.f2=_tmp173->unknown_all,
_tmp800)));};};};_LL133: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1C5=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp18D;if(_tmp1C5->tag != 4)goto _LL135;else{_tmp1C6=_tmp1C5->f1;_tmp1C7=_tmp1C5->f2;
if(_tmp1C7 != 0)goto _LL135;_tmp1C8=_tmp1C5->f3;}}_LL134: if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){{const char*
_tmp803;void*_tmp802;(_tmp802=0,Cyc_Tcutil_terr(e->loc,((_tmp803="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp803,sizeof(char),58))),_tag_dyneither(_tmp802,sizeof(void*),0)));}{
struct _tuple14 _tmp804;return(_tmp804.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp804.f2=
_tmp173->unknown_all,_tmp804)));};}{struct Cyc_Dict_Dict*_tmp251=env->all_changed;
while(1){{struct Cyc_Dict_Dict*_tmp805;env->all_changed=((_tmp805=_region_malloc(
env->r,sizeof(*_tmp805)),((_tmp805[0]=_tmp173->mt_place_set,_tmp805))));}{union
Cyc_CfFlowInfo_FlowInfo _tmp254;union Cyc_CfFlowInfo_AbsLVal _tmp255;struct _tuple15
_tmp253=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1C6);_tmp254=_tmp253.f1;
_tmp255=_tmp253.f2;{struct Cyc_Dict_Dict _tmp256=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp806;env->all_changed=((
_tmp806=_region_malloc(env->r,sizeof(*_tmp806)),((_tmp806[0]=_tmp173->mt_place_set,
_tmp806))));}{union Cyc_CfFlowInfo_FlowInfo _tmp259;void*_tmp25A;struct _tuple14
_tmp258=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C8);_tmp259=_tmp258.f1;
_tmp25A=_tmp258.f2;{struct Cyc_Dict_Dict _tmp25B=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo _tmp25C=Cyc_CfFlowInfo_after_flow(
_tmp173,(struct Cyc_Dict_Dict*)& _tmp256,_tmp254,_tmp259,_tmp256,_tmp25B);union Cyc_CfFlowInfo_FlowInfo
_tmp25D=Cyc_CfFlowInfo_join_flow(_tmp173,_tmp251,inflow,_tmp25C);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp25D,inflow)){if(_tmp251 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp251,Cyc_CfFlowInfo_union_place_set(
_tmp256,_tmp25B,0),0);}return Cyc_NewControlFlow_do_assign(_tmp173,env,_tmp25C,
_tmp1C6,_tmp255,_tmp1C8,_tmp25A,e->loc);}inflow=_tmp25D;};};};};}};_LL135: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp1C9=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp18D;if(_tmp1C9->tag != 9)goto _LL137;else{_tmp1CA=_tmp1C9->f1;_tmp1CB=_tmp1C9->f2;}}
_LL136: {union Cyc_CfFlowInfo_FlowInfo _tmp25F;void*_tmp260;struct _tuple14 _tmp25E=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1CA);_tmp25F=_tmp25E.f1;_tmp260=
_tmp25E.f2;return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp25F,_tmp1CB);}
_LL137: {struct Cyc_Absyn_Throw_e_struct*_tmp1CC=(struct Cyc_Absyn_Throw_e_struct*)
_tmp18D;if(_tmp1CC->tag != 12)goto _LL139;else{_tmp1CD=_tmp1CC->f1;}}_LL138: {
union Cyc_CfFlowInfo_FlowInfo _tmp262;void*_tmp263;struct _tuple14 _tmp261=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1CD);_tmp262=_tmp261.f1;_tmp263=_tmp261.f2;Cyc_NewControlFlow_use_Rval(
env,_tmp1CD->loc,_tmp262,_tmp263);{struct _tuple14 _tmp807;return(_tmp807.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp807.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0,_tmp173->unknown_all),_tmp807)));};}_LL139: {struct
Cyc_Absyn_FnCall_e_struct*_tmp1CE=(struct Cyc_Absyn_FnCall_e_struct*)_tmp18D;if(
_tmp1CE->tag != 11)goto _LL13B;else{_tmp1CF=_tmp1CE->f1;_tmp1D0=_tmp1CE->f2;}}
_LL13A: {struct _RegionHandle _tmp265=_new_region("temp");struct _RegionHandle*temp=&
_tmp265;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo _tmp268;struct Cyc_List_List*
_tmp269;struct Cyc_List_List*_tmp808;struct _tuple20 _tmp267=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp808=_region_malloc(temp,sizeof(*_tmp808)),((_tmp808->hd=
_tmp1CF,((_tmp808->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(temp,_tmp1D0),_tmp808)))))),0,1);_tmp268=_tmp267.f1;_tmp269=
_tmp267.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp26A=Cyc_NewControlFlow_use_Rval(env,
_tmp1CF->loc,_tmp268,(void*)((struct Cyc_List_List*)_check_null(_tmp269))->hd);
_tmp269=_tmp269->tl;{struct Cyc_List_List*init_params=0;struct Cyc_List_List*
nolive_unique_params=0;{void*_tmp26B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1CF->topt))->v);struct Cyc_Absyn_PtrInfo _tmp26D;void*_tmp26E;
_LL181: {struct Cyc_Absyn_PointerType_struct*_tmp26C=(struct Cyc_Absyn_PointerType_struct*)
_tmp26B;if(_tmp26C->tag != 5)goto _LL183;else{_tmp26D=_tmp26C->f1;_tmp26E=_tmp26D.elt_typ;}}
_LL182:{void*_tmp26F=Cyc_Tcutil_compress(_tmp26E);struct Cyc_Absyn_FnInfo _tmp271;
struct Cyc_List_List*_tmp272;_LL186: {struct Cyc_Absyn_FnType_struct*_tmp270=(
struct Cyc_Absyn_FnType_struct*)_tmp26F;if(_tmp270->tag != 9)goto _LL188;else{
_tmp271=_tmp270->f1;_tmp272=_tmp271.attributes;}}_LL187: for(0;_tmp272 != 0;
_tmp272=_tmp272->tl){void*_tmp273=(void*)_tmp272->hd;int _tmp275;int _tmp277;
_LL18B: {struct Cyc_Absyn_Initializes_att_struct*_tmp274=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp273;if(_tmp274->tag != 20)goto _LL18D;else{_tmp275=_tmp274->f1;}}_LL18C:{
struct Cyc_List_List*_tmp809;init_params=((_tmp809=_region_malloc(temp,sizeof(*
_tmp809)),((_tmp809->hd=(void*)_tmp275,((_tmp809->tl=init_params,_tmp809))))));}
goto _LL18A;_LL18D: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp276=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp273;if(_tmp276->tag != 21)goto _LL18F;else{_tmp277=_tmp276->f1;}}_LL18E:{
struct Cyc_List_List*_tmp80A;nolive_unique_params=((_tmp80A=_region_malloc(temp,
sizeof(*_tmp80A)),((_tmp80A->hd=(void*)_tmp277,((_tmp80A->tl=
nolive_unique_params,_tmp80A))))));}goto _LL18A;_LL18F:;_LL190: goto _LL18A;_LL18A:;}
goto _LL185;_LL188:;_LL189: {const char*_tmp80D;void*_tmp80C;(_tmp80C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp80D="anal_Rexp: bad function type",
_tag_dyneither(_tmp80D,sizeof(char),29))),_tag_dyneither(_tmp80C,sizeof(void*),0)));}
_LL185:;}goto _LL180;_LL183:;_LL184: {const char*_tmp810;void*_tmp80F;(_tmp80F=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp810="anal_Rexp: bad function type",_tag_dyneither(_tmp810,sizeof(char),29))),
_tag_dyneither(_tmp80F,sizeof(void*),0)));}_LL180:;}{int i=1;for(0;_tmp269 != 0;(((
_tmp269=_tmp269->tl,_tmp1D0=((struct Cyc_List_List*)_check_null(_tmp1D0))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i) && 
!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){
_tmp26A=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D0))->hd)->loc,_tmp26A,(void*)_tmp269->hd);continue;}if(((int(*)(
struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){_tmp26A=Cyc_NewControlFlow_use_nounique_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1D0))->hd)->loc,(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D0))->hd)->topt))->v,_tmp26A,(void*)_tmp269->hd);continue;}{
union Cyc_CfFlowInfo_FlowInfo _tmp27E=_tmp268;int _tmp27F;struct _tuple13 _tmp280;
struct Cyc_Dict_Dict _tmp281;_LL192: if((_tmp27E.BottomFL).tag != 1)goto _LL194;
_tmp27F=(int)(_tmp27E.BottomFL).val;_LL193: goto _LL191;_LL194: if((_tmp27E.ReachableFL).tag
!= 2)goto _LL191;_tmp280=(struct _tuple13)(_tmp27E.ReachableFL).val;_tmp281=
_tmp280.f1;_LL195: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp281,(void*)_tmp269->hd)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp813;void*_tmp812;(_tmp812=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1D0))->hd)->loc,((
_tmp813="expression may not be initialized",_tag_dyneither(_tmp813,sizeof(char),
34))),_tag_dyneither(_tmp812,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo
_tmp284=_tmp26A;int _tmp285;struct _tuple13 _tmp286;struct Cyc_Dict_Dict _tmp287;
struct Cyc_List_List*_tmp288;_LL197: if((_tmp284.BottomFL).tag != 1)goto _LL199;
_tmp285=(int)(_tmp284.BottomFL).val;_LL198: goto _LL196;_LL199: if((_tmp284.ReachableFL).tag
!= 2)goto _LL196;_tmp286=(struct _tuple13)(_tmp284.ReachableFL).val;_tmp287=
_tmp286.f1;_tmp288=_tmp286.f2;_LL19A: {struct Cyc_Dict_Dict _tmp289=Cyc_CfFlowInfo_escape_deref(
_tmp173,_tmp287,env->all_changed,(void*)_tmp269->hd);{void*_tmp28A=(void*)
_tmp269->hd;struct Cyc_CfFlowInfo_Place*_tmp28C;_LL19C: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp28B=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp28A;if(_tmp28B->tag != 5)goto
_LL19E;else{_tmp28C=_tmp28B->f1;}}_LL19D:{void*_tmp28D=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D0))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp28F;void*
_tmp290;_LL1A1: {struct Cyc_Absyn_PointerType_struct*_tmp28E=(struct Cyc_Absyn_PointerType_struct*)
_tmp28D;if(_tmp28E->tag != 5)goto _LL1A3;else{_tmp28F=_tmp28E->f1;_tmp290=_tmp28F.elt_typ;}}
_LL1A2: _tmp289=Cyc_CfFlowInfo_assign_place(_tmp173,((struct Cyc_Absyn_Exp*)
_tmp1D0->hd)->loc,_tmp289,env->all_changed,_tmp28C,Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,_tmp290,0,_tmp173->esc_all));goto _LL1A0;_LL1A3:;_LL1A4: {const char*
_tmp816;void*_tmp815;(_tmp815=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp816="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp816,sizeof(char),39))),_tag_dyneither(_tmp815,sizeof(void*),0)));}
_LL1A0:;}goto _LL19B;_LL19E:;_LL19F: goto _LL19B;_LL19B:;}_tmp26A=Cyc_CfFlowInfo_ReachableFL(
_tmp289,_tmp288);goto _LL196;}_LL196:;}goto _LL191;_LL191:;};}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1CF->topt))->v)){struct _tuple14
_tmp817;struct _tuple14 _tmp294=(_tmp817.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp817.f2=
Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
0,_tmp173->unknown_all),_tmp817)));_npop_handler(0);return _tmp294;}else{struct
_tuple14 _tmp818;struct _tuple14 _tmp296=(_tmp818.f1=_tmp26A,((_tmp818.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0,_tmp173->unknown_all),
_tmp818)));_npop_handler(0);return _tmp296;}};};};_pop_region(temp);}_LL13B: {
struct Cyc_Absyn_Malloc_e_struct*_tmp1D1=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp18D;if(_tmp1D1->tag != 34)goto _LL13D;else{_tmp1D2=_tmp1D1->f1;_tmp1D3=_tmp1D2.is_calloc;
_tmp1D4=_tmp1D2.rgn;_tmp1D5=_tmp1D2.elt_type;_tmp1D6=_tmp1D2.num_elts;_tmp1D7=
_tmp1D2.fat_result;}}_LL13C: {struct Cyc_CfFlowInfo_MallocPt_struct _tmp81B;struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp81A;void*root=(void*)((_tmp81A=_region_malloc(
_tmp173->r,sizeof(*_tmp81A)),((_tmp81A[0]=((_tmp81B.tag=1,((_tmp81B.f1=_tmp1D6,((
_tmp81B.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp81B)))))),_tmp81A))));struct Cyc_CfFlowInfo_Place*_tmp81C;struct Cyc_CfFlowInfo_Place*
place=(_tmp81C=_region_malloc(_tmp173->r,sizeof(*_tmp81C)),((_tmp81C->root=root,((
_tmp81C->fields=0,_tmp81C)))));struct Cyc_CfFlowInfo_AddressOf_struct _tmp81F;
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp81E;void*rval=(void*)((_tmp81E=
_region_malloc(_tmp173->r,sizeof(*_tmp81E)),((_tmp81E[0]=((_tmp81F.tag=5,((
_tmp81F.f1=place,_tmp81F)))),_tmp81E))));void*place_val;if(_tmp1D7)place_val=
_tmp173->notzeroall;else{if(_tmp1D3)place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,*((void**)_check_null(_tmp1D5)),0,_tmp173->zero);else{place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,*((void**)_check_null(_tmp1D5)),0,_tmp173->unknown_none);}}{union Cyc_CfFlowInfo_FlowInfo
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,
unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);if(
_tmp1D4 != 0){struct _RegionHandle _tmp297=_new_region("temp");struct _RegionHandle*
temp=& _tmp297;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo _tmp29A;struct Cyc_List_List*
_tmp29B;struct Cyc_Absyn_Exp*_tmp820[2];struct _tuple20 _tmp299=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp820[1]=_tmp1D6,((_tmp820[0]=(struct Cyc_Absyn_Exp*)_tmp1D4,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
temp,_tag_dyneither(_tmp820,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp29A=
_tmp299.f1;_tmp29B=_tmp299.f2;outflow=_tmp29A;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo
_tmp29D;struct _tuple14 _tmp29C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D6);
_tmp29D=_tmp29C.f1;outflow=_tmp29D;}{union Cyc_CfFlowInfo_FlowInfo _tmp29E=outflow;
int _tmp29F;struct _tuple13 _tmp2A0;struct Cyc_Dict_Dict _tmp2A1;struct Cyc_List_List*
_tmp2A2;_LL1A6: if((_tmp29E.BottomFL).tag != 1)goto _LL1A8;_tmp29F=(int)(_tmp29E.BottomFL).val;
_LL1A7: {struct _tuple14 _tmp821;return(_tmp821.f1=outflow,((_tmp821.f2=rval,
_tmp821)));}_LL1A8: if((_tmp29E.ReachableFL).tag != 2)goto _LL1A5;_tmp2A0=(struct
_tuple13)(_tmp29E.ReachableFL).val;_tmp2A1=_tmp2A0.f1;_tmp2A2=_tmp2A0.f2;_LL1A9: {
struct _tuple14 _tmp822;return(_tmp822.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2A1,root,place_val),
_tmp2A2),((_tmp822.f2=rval,_tmp822)));}_LL1A5:;};};}_LL13D: {struct Cyc_Absyn_Swap_e_struct*
_tmp1D8=(struct Cyc_Absyn_Swap_e_struct*)_tmp18D;if(_tmp1D8->tag != 35)goto _LL13F;
else{_tmp1D9=_tmp1D8->f1;_tmp1DA=_tmp1D8->f2;}}_LL13E: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle _tmp2AA=
_new_region("temp");struct _RegionHandle*temp=& _tmp2AA;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo _tmp2AD;struct Cyc_List_List*_tmp2AE;struct Cyc_Absyn_Exp*
_tmp823[2];struct _tuple20 _tmp2AC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,
env,inflow,((_tmp823[1]=_tmp1DA,((_tmp823[0]=_tmp1D9,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp823,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp2AD=_tmp2AC.f1;_tmp2AE=
_tmp2AC.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AE))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AE->tl))->hd;outflow=
_tmp2AD;}{union Cyc_CfFlowInfo_AbsLVal _tmp2B0;struct _tuple15 _tmp2AF=Cyc_NewControlFlow_anal_Lexp(
env,outflow,0,_tmp1D9);_tmp2B0=_tmp2AF.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp2B2;
struct _tuple15 _tmp2B1=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1DA);
_tmp2B2=_tmp2B1.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2B3=outflow;struct _tuple13
_tmp2B4;struct Cyc_Dict_Dict _tmp2B5;struct Cyc_List_List*_tmp2B6;_LL1AB: if((
_tmp2B3.ReachableFL).tag != 2)goto _LL1AD;_tmp2B4=(struct _tuple13)(_tmp2B3.ReachableFL).val;
_tmp2B5=_tmp2B4.f1;_tmp2B6=_tmp2B4.f2;_LL1AC:{union Cyc_CfFlowInfo_AbsLVal _tmp2B7=
_tmp2B0;struct Cyc_CfFlowInfo_Place*_tmp2B8;int _tmp2B9;_LL1B0: if((_tmp2B7.PlaceL).tag
!= 1)goto _LL1B2;_tmp2B8=(struct Cyc_CfFlowInfo_Place*)(_tmp2B7.PlaceL).val;_LL1B1:
_tmp2B5=Cyc_CfFlowInfo_assign_place(_tmp173,_tmp1D9->loc,_tmp2B5,env->all_changed,
_tmp2B8,right_rval);goto _LL1AF;_LL1B2: if((_tmp2B7.UnknownL).tag != 2)goto _LL1AF;
_tmp2B9=(int)(_tmp2B7.UnknownL).val;_LL1B3: goto _LL1AF;_LL1AF:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp2BA=_tmp2B2;struct Cyc_CfFlowInfo_Place*_tmp2BB;int _tmp2BC;_LL1B5: if((_tmp2BA.PlaceL).tag
!= 1)goto _LL1B7;_tmp2BB=(struct Cyc_CfFlowInfo_Place*)(_tmp2BA.PlaceL).val;_LL1B6:
_tmp2B5=Cyc_CfFlowInfo_assign_place(_tmp173,_tmp1DA->loc,_tmp2B5,env->all_changed,
_tmp2BB,left_rval);goto _LL1B4;_LL1B7: if((_tmp2BA.UnknownL).tag != 2)goto _LL1B4;
_tmp2BC=(int)(_tmp2BA.UnknownL).val;_LL1B8: goto _LL1B4;_LL1B4:;}_tmp2B6=Cyc_CfFlowInfo_reln_kill_exp(
_tmp173->r,_tmp2B6,_tmp1D9);_tmp2B6=Cyc_CfFlowInfo_reln_kill_exp(_tmp173->r,
_tmp2B6,_tmp1DA);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2B5,_tmp2B6);goto _LL1AA;
_LL1AD:;_LL1AE: goto _LL1AA;_LL1AA:;}{struct _tuple14 _tmp824;struct _tuple14 _tmp2BE=(
_tmp824.f1=outflow,((_tmp824.f2=_tmp173->unknown_all,_tmp824)));_npop_handler(0);
return _tmp2BE;};};};;_pop_region(temp);}_LL13F: {struct Cyc_Absyn_New_e_struct*
_tmp1DB=(struct Cyc_Absyn_New_e_struct*)_tmp18D;if(_tmp1DB->tag != 17)goto _LL141;
else{_tmp1DC=_tmp1DB->f1;_tmp1DD=_tmp1DB->f2;}}_LL140: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp827;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp826;void*root=(void*)((_tmp826=
_region_malloc(_tmp173->r,sizeof(*_tmp826)),((_tmp826[0]=((_tmp827.tag=1,((
_tmp827.f1=_tmp1DD,((_tmp827.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp827)))))),_tmp826))));struct Cyc_CfFlowInfo_Place*_tmp828;
struct Cyc_CfFlowInfo_Place*place=(_tmp828=_region_malloc(_tmp173->r,sizeof(*
_tmp828)),((_tmp828->root=root,((_tmp828->fields=0,_tmp828)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp82B;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp82A;void*rval=(void*)((_tmp82A=
_region_malloc(_tmp173->r,sizeof(*_tmp82A)),((_tmp82A[0]=((_tmp82B.tag=5,((
_tmp82B.f1=place,_tmp82B)))),_tmp82A))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;if(_tmp1DC != 0){
struct _RegionHandle _tmp2BF=_new_region("temp");struct _RegionHandle*temp=& _tmp2BF;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo _tmp2C2;struct Cyc_List_List*
_tmp2C3;struct Cyc_Absyn_Exp*_tmp82C[2];struct _tuple20 _tmp2C1=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp82C[1]=_tmp1DD,((_tmp82C[0]=(struct Cyc_Absyn_Exp*)_tmp1DC,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
temp,_tag_dyneither(_tmp82C,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp2C2=
_tmp2C1.f1;_tmp2C3=_tmp2C1.f2;outflow=_tmp2C2;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2C3))->tl))->hd;};_pop_region(
temp);}else{union Cyc_CfFlowInfo_FlowInfo _tmp2C5;void*_tmp2C6;struct _tuple14
_tmp2C4=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1DD);_tmp2C5=_tmp2C4.f1;
_tmp2C6=_tmp2C4.f2;outflow=_tmp2C5;place_val=_tmp2C6;}{union Cyc_CfFlowInfo_FlowInfo
_tmp2C7=outflow;int _tmp2C8;struct _tuple13 _tmp2C9;struct Cyc_Dict_Dict _tmp2CA;
struct Cyc_List_List*_tmp2CB;_LL1BA: if((_tmp2C7.BottomFL).tag != 1)goto _LL1BC;
_tmp2C8=(int)(_tmp2C7.BottomFL).val;_LL1BB: {struct _tuple14 _tmp82D;return(
_tmp82D.f1=outflow,((_tmp82D.f2=rval,_tmp82D)));}_LL1BC: if((_tmp2C7.ReachableFL).tag
!= 2)goto _LL1B9;_tmp2C9=(struct _tuple13)(_tmp2C7.ReachableFL).val;_tmp2CA=
_tmp2C9.f1;_tmp2CB=_tmp2C9.f2;_LL1BD: {struct _tuple14 _tmp82E;return(_tmp82E.f1=
Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,
void*v))Cyc_Dict_insert)(_tmp2CA,root,place_val),_tmp2CB),((_tmp82E.f2=rval,
_tmp82E)));}_LL1B9:;};};}_LL141: {struct Cyc_Absyn_Address_e_struct*_tmp1DE=(
struct Cyc_Absyn_Address_e_struct*)_tmp18D;if(_tmp1DE->tag != 16)goto _LL143;else{
_tmp1DF=_tmp1DE->f1;}}_LL142: {union Cyc_CfFlowInfo_FlowInfo _tmp2D4;union Cyc_CfFlowInfo_AbsLVal
_tmp2D5;struct _tuple15 _tmp2D3=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1DF);
_tmp2D4=_tmp2D3.f1;_tmp2D5=_tmp2D3.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp2D6=
_tmp2D5;int _tmp2D7;struct Cyc_CfFlowInfo_Place*_tmp2D8;_LL1BF: if((_tmp2D6.UnknownL).tag
!= 2)goto _LL1C1;_tmp2D7=(int)(_tmp2D6.UnknownL).val;_LL1C0: {struct _tuple14
_tmp82F;return(_tmp82F.f1=_tmp2D4,((_tmp82F.f2=_tmp173->notzeroall,_tmp82F)));}
_LL1C1: if((_tmp2D6.PlaceL).tag != 1)goto _LL1BE;_tmp2D8=(struct Cyc_CfFlowInfo_Place*)(
_tmp2D6.PlaceL).val;_LL1C2: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp835;
struct Cyc_CfFlowInfo_AddressOf_struct _tmp834;struct _tuple14 _tmp833;return(
_tmp833.f1=_tmp2D4,((_tmp833.f2=(void*)((_tmp835=_region_malloc(env->r,sizeof(*
_tmp835)),((_tmp835[0]=((_tmp834.tag=5,((_tmp834.f1=_tmp2D8,_tmp834)))),_tmp835)))),
_tmp833)));}_LL1BE:;};}_LL143: {struct Cyc_Absyn_Deref_e_struct*_tmp1E0=(struct
Cyc_Absyn_Deref_e_struct*)_tmp18D;if(_tmp1E0->tag != 21)goto _LL145;else{_tmp1E1=
_tmp1E0->f1;}}_LL144: {union Cyc_CfFlowInfo_FlowInfo _tmp2DE;void*_tmp2DF;struct
_tuple14 _tmp2DD=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1E1);_tmp2DE=
_tmp2DD.f1;_tmp2DF=_tmp2DD.f2;return Cyc_NewControlFlow_anal_derefR(env,inflow,
_tmp2DE,_tmp1E1,_tmp2DF);}_LL145: {struct Cyc_Absyn_AggrMember_e_struct*_tmp1E2=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp18D;if(_tmp1E2->tag != 22)goto _LL147;
else{_tmp1E3=_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;}}_LL146: {union Cyc_CfFlowInfo_FlowInfo
_tmp2E1;void*_tmp2E2;struct _tuple14 _tmp2E0=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1E3);_tmp2E1=_tmp2E0.f1;_tmp2E2=_tmp2E0.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1E3->topt))->v)){struct _tuple14
_tmp836;return(_tmp836.f1=_tmp2E1,((_tmp836.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0,_tmp173->unknown_all),
_tmp836)));}{void*_tmp2E4=_tmp2E2;struct Cyc_CfFlowInfo_UnionRInfo _tmp2E6;int
_tmp2E7;int _tmp2E8;struct _dyneither_ptr _tmp2E9;_LL1C4: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2E5=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2E4;if(_tmp2E5->tag != 6)goto
_LL1C6;else{_tmp2E6=_tmp2E5->f1;_tmp2E7=_tmp2E6.is_union;_tmp2E8=_tmp2E6.fieldnum;
_tmp2E9=_tmp2E5->f2;}}_LL1C5: {int _tmp2EA=Cyc_CfFlowInfo_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E3->topt))->v,_tmp1E4);if((_tmp2E7  && 
_tmp2E8 != - 1) && _tmp2E8 != _tmp2EA){struct _tuple14 _tmp837;return(_tmp837.f1=
_tmp2E1,((_tmp837.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,1,_tmp173->unknown_none),_tmp837)));}{struct _tuple14
_tmp838;return(_tmp838.f1=_tmp2E1,((_tmp838.f2=*((void**)
_check_dyneither_subscript(_tmp2E9,sizeof(void*),_tmp2EA)),_tmp838)));};}_LL1C6:;
_LL1C7: {struct Cyc_Core_Impossible_struct _tmp845;const char*_tmp844;void*_tmp843[
1];struct Cyc_String_pa_struct _tmp842;struct Cyc_Core_Impossible_struct*_tmp841;(
int)_throw((void*)((_tmp841=_cycalloc(sizeof(*_tmp841)),((_tmp841[0]=((_tmp845.tag=
Cyc_Core_Impossible,((_tmp845.f1=(struct _dyneither_ptr)((_tmp842.tag=0,((_tmp842.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp843[
0]=& _tmp842,Cyc_aprintf(((_tmp844="anal_Rexp: AggrMember: %s",_tag_dyneither(
_tmp844,sizeof(char),26))),_tag_dyneither(_tmp843,sizeof(void*),1)))))))),
_tmp845)))),_tmp841)))));}_LL1C3:;};}_LL147: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1E5=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp18D;if(_tmp1E5->tag != 38)goto
_LL149;else{_tmp1E6=_tmp1E5->f1;_tmp1E7=_tmp1E5->f2;}}_LL148: {union Cyc_CfFlowInfo_FlowInfo
_tmp2F3;void*_tmp2F4;struct _tuple14 _tmp2F2=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1E6);_tmp2F3=_tmp2F2.f1;_tmp2F4=_tmp2F2.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1E6->topt))->v)){struct _tuple14
_tmp846;return(_tmp846.f1=_tmp2F3,((_tmp846.f2=_tmp173->unknown_all,_tmp846)));}{
void*_tmp2F6=_tmp2F4;struct Cyc_CfFlowInfo_UnionRInfo _tmp2F8;int _tmp2F9;int
_tmp2FA;struct _dyneither_ptr _tmp2FB;_LL1C9: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2F7=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2F6;if(_tmp2F7->tag != 6)goto
_LL1CB;else{_tmp2F8=_tmp2F7->f1;_tmp2F9=_tmp2F8.is_union;_tmp2FA=_tmp2F8.fieldnum;
_tmp2FB=_tmp2F7->f2;}}_LL1CA: {int _tmp2FC=Cyc_CfFlowInfo_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E6->topt))->v,_tmp1E7);if(_tmp2F9  && 
_tmp2FA != - 1){if(_tmp2FA != _tmp2FC){struct _tuple14 _tmp847;return(_tmp847.f1=
_tmp2F3,((_tmp847.f2=_tmp173->zero,_tmp847)));}else{struct _tuple14 _tmp848;return(
_tmp848.f1=_tmp2F3,((_tmp848.f2=_tmp173->notzeroall,_tmp848)));}}else{struct
_tuple14 _tmp849;return(_tmp849.f1=_tmp2F3,((_tmp849.f2=_tmp173->unknown_all,
_tmp849)));}}_LL1CB:;_LL1CC: {struct Cyc_Core_Impossible_struct _tmp856;const char*
_tmp855;void*_tmp854[1];struct Cyc_String_pa_struct _tmp853;struct Cyc_Core_Impossible_struct*
_tmp852;(int)_throw((void*)((_tmp852=_cycalloc(sizeof(*_tmp852)),((_tmp852[0]=((
_tmp856.tag=Cyc_Core_Impossible,((_tmp856.f1=(struct _dyneither_ptr)((_tmp853.tag=
0,((_tmp853.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp854[0]=& _tmp853,Cyc_aprintf(((_tmp855="anal_Rexp: TagCheck_e: %s",
_tag_dyneither(_tmp855,sizeof(char),26))),_tag_dyneither(_tmp854,sizeof(void*),1)))))))),
_tmp856)))),_tmp852)))));}_LL1C8:;};}_LL149: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp1E8=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp18D;if(_tmp1E8->tag != 23)goto
_LL14B;else{_tmp1E9=_tmp1E8->f1;_tmp1EA=_tmp1E8->f2;}}_LL14A: {union Cyc_CfFlowInfo_FlowInfo
_tmp306;void*_tmp307;struct _tuple14 _tmp305=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1E9);_tmp306=_tmp305.f1;_tmp307=_tmp305.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp309;void*_tmp30A;struct _tuple14 _tmp308=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp306,_tmp1E9,_tmp307);_tmp309=_tmp308.f1;_tmp30A=_tmp308.f2;{void*
_tmp30B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1E9->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp30D;void*_tmp30E;_LL1CE: {struct Cyc_Absyn_PointerType_struct*
_tmp30C=(struct Cyc_Absyn_PointerType_struct*)_tmp30B;if(_tmp30C->tag != 5)goto
_LL1D0;else{_tmp30D=_tmp30C->f1;_tmp30E=_tmp30D.elt_typ;}}_LL1CF: if(Cyc_Absyn_is_nontagged_union_type(
_tmp30E)){struct _tuple14 _tmp857;return(_tmp857.f1=_tmp309,((_tmp857.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0,_tmp173->unknown_all),
_tmp857)));}{void*_tmp310=_tmp30A;struct Cyc_CfFlowInfo_UnionRInfo _tmp312;int
_tmp313;int _tmp314;struct _dyneither_ptr _tmp315;_LL1D3: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp311=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp310;if(_tmp311->tag != 6)goto
_LL1D5;else{_tmp312=_tmp311->f1;_tmp313=_tmp312.is_union;_tmp314=_tmp312.fieldnum;
_tmp315=_tmp311->f2;}}_LL1D4: {int _tmp316=Cyc_CfFlowInfo_get_field_index(_tmp30E,
_tmp1EA);if((_tmp313  && _tmp314 != - 1) && _tmp314 != _tmp316){struct _tuple14
_tmp858;return(_tmp858.f1=_tmp309,((_tmp858.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,1,_tmp173->unknown_none),
_tmp858)));}{struct _tuple14 _tmp859;return(_tmp859.f1=_tmp309,((_tmp859.f2=*((
void**)_check_dyneither_subscript(_tmp315,sizeof(void*),_tmp316)),_tmp859)));};}
_LL1D5:;_LL1D6: {struct Cyc_Core_Impossible_struct _tmp85F;const char*_tmp85E;
struct Cyc_Core_Impossible_struct*_tmp85D;(int)_throw((void*)((_tmp85D=_cycalloc(
sizeof(*_tmp85D)),((_tmp85D[0]=((_tmp85F.tag=Cyc_Core_Impossible,((_tmp85F.f1=((
_tmp85E="anal_Rexp: AggrArrow",_tag_dyneither(_tmp85E,sizeof(char),21))),_tmp85F)))),
_tmp85D)))));}_LL1D2:;};_LL1D0:;_LL1D1: {struct Cyc_Core_Impossible_struct _tmp865;
const char*_tmp864;struct Cyc_Core_Impossible_struct*_tmp863;(int)_throw((void*)((
_tmp863=_cycalloc(sizeof(*_tmp863)),((_tmp863[0]=((_tmp865.tag=Cyc_Core_Impossible,((
_tmp865.f1=((_tmp864="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp864,sizeof(
char),25))),_tmp865)))),_tmp863)))));}_LL1CD:;};};}_LL14B: {struct Cyc_Absyn_Conditional_e_struct*
_tmp1EB=(struct Cyc_Absyn_Conditional_e_struct*)_tmp18D;if(_tmp1EB->tag != 6)goto
_LL14D;else{_tmp1EC=_tmp1EB->f1;_tmp1ED=_tmp1EB->f2;_tmp1EE=_tmp1EB->f3;}}_LL14C: {
union Cyc_CfFlowInfo_FlowInfo _tmp320;union Cyc_CfFlowInfo_FlowInfo _tmp321;struct
_tuple16 _tmp31F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1EC);_tmp320=_tmp31F.f1;
_tmp321=_tmp31F.f2;{struct _tuple14 _tmp322=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp320,_tmp1ED);struct _tuple14 _tmp323=Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,_tmp321,_tmp1EE);return Cyc_CfFlowInfo_join_flow_and_rval(_tmp173,
env->all_changed,_tmp322,_tmp323);};}_LL14D: {struct Cyc_Absyn_And_e_struct*
_tmp1EF=(struct Cyc_Absyn_And_e_struct*)_tmp18D;if(_tmp1EF->tag != 7)goto _LL14F;
else{_tmp1F0=_tmp1EF->f1;_tmp1F1=_tmp1EF->f2;}}_LL14E: {union Cyc_CfFlowInfo_FlowInfo
_tmp325;union Cyc_CfFlowInfo_FlowInfo _tmp326;struct _tuple16 _tmp324=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1F0);_tmp325=_tmp324.f1;_tmp326=_tmp324.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp328;void*_tmp329;struct _tuple14 _tmp327=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp325,_tmp1F1);_tmp328=_tmp327.f1;_tmp329=_tmp327.f2;{struct _tuple14
_tmp866;struct _tuple14 _tmp32A=(_tmp866.f1=_tmp328,((_tmp866.f2=_tmp329,_tmp866)));
struct _tuple14 _tmp867;struct _tuple14 _tmp32B=(_tmp867.f1=_tmp326,((_tmp867.f2=
_tmp173->zero,_tmp867)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp173,env->all_changed,
_tmp32A,_tmp32B);};};}_LL14F: {struct Cyc_Absyn_Or_e_struct*_tmp1F2=(struct Cyc_Absyn_Or_e_struct*)
_tmp18D;if(_tmp1F2->tag != 8)goto _LL151;else{_tmp1F3=_tmp1F2->f1;_tmp1F4=_tmp1F2->f2;}}
_LL150: {union Cyc_CfFlowInfo_FlowInfo _tmp32F;union Cyc_CfFlowInfo_FlowInfo _tmp330;
struct _tuple16 _tmp32E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1F3);_tmp32F=
_tmp32E.f1;_tmp330=_tmp32E.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp332;void*_tmp333;
struct _tuple14 _tmp331=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp330,_tmp1F4);
_tmp332=_tmp331.f1;_tmp333=_tmp331.f2;{struct _tuple14 _tmp868;struct _tuple14
_tmp334=(_tmp868.f1=_tmp332,((_tmp868.f2=_tmp333,_tmp868)));struct _tuple14
_tmp869;struct _tuple14 _tmp335=(_tmp869.f1=_tmp32F,((_tmp869.f2=_tmp173->notzeroall,
_tmp869)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp173,env->all_changed,
_tmp334,_tmp335);};};}_LL151: {struct Cyc_Absyn_Subscript_e_struct*_tmp1F5=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp18D;if(_tmp1F5->tag != 24)goto _LL153;
else{_tmp1F6=_tmp1F5->f1;_tmp1F7=_tmp1F5->f2;}}_LL152: {struct _RegionHandle
_tmp338=_new_region("temp");struct _RegionHandle*temp=& _tmp338;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp33B;struct Cyc_List_List*_tmp33C;struct Cyc_Absyn_Exp*
_tmp86A[2];struct _tuple20 _tmp33A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,
env,inflow,((_tmp86A[1]=_tmp1F7,((_tmp86A[0]=_tmp1F6,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp86A,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp33B=_tmp33A.f1;_tmp33C=
_tmp33A.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp33D=_tmp33B;{union Cyc_CfFlowInfo_FlowInfo
_tmp33E=_tmp33B;struct _tuple13 _tmp33F;struct Cyc_Dict_Dict _tmp340;struct Cyc_List_List*
_tmp341;_LL1D8: if((_tmp33E.ReachableFL).tag != 2)goto _LL1DA;_tmp33F=(struct
_tuple13)(_tmp33E.ReachableFL).val;_tmp340=_tmp33F.f1;_tmp341=_tmp33F.f2;_LL1D9:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp340,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp33C))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp86D;void*_tmp86C;(_tmp86C=0,Cyc_Tcutil_terr(_tmp1F7->loc,((_tmp86D="expression may not be initialized",
_tag_dyneither(_tmp86D,sizeof(char),34))),_tag_dyneither(_tmp86C,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp344=Cyc_NewControlFlow_add_subscript_reln(_tmp173->r,
_tmp341,_tmp1F6,_tmp1F7);if(_tmp341 != _tmp344)_tmp33D=Cyc_CfFlowInfo_ReachableFL(
_tmp340,_tmp344);goto _LL1D7;};_LL1DA:;_LL1DB: goto _LL1D7;_LL1D7:;}{void*_tmp345=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1F6->topt))->v);
struct Cyc_List_List*_tmp347;struct Cyc_Absyn_PtrInfo _tmp349;struct Cyc_Absyn_PtrAtts
_tmp34A;union Cyc_Absyn_Constraint*_tmp34B;_LL1DD: {struct Cyc_Absyn_TupleType_struct*
_tmp346=(struct Cyc_Absyn_TupleType_struct*)_tmp345;if(_tmp346->tag != 10)goto
_LL1DF;else{_tmp347=_tmp346->f1;}}_LL1DE: {void*_tmp34C=(void*)((struct Cyc_List_List*)
_check_null(_tmp33C))->hd;struct _dyneither_ptr _tmp34E;_LL1E4: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp34D=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp34C;if(_tmp34D->tag != 6)goto
_LL1E6;else{_tmp34E=_tmp34D->f2;}}_LL1E5: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(
_tmp1F7)).f1;struct _tuple14 _tmp86E;struct _tuple14 _tmp350=(_tmp86E.f1=_tmp33D,((
_tmp86E.f2=*((void**)_check_dyneither_subscript(_tmp34E,sizeof(void*),(int)i)),
_tmp86E)));_npop_handler(0);return _tmp350;}_LL1E6:;_LL1E7: {struct Cyc_Core_Impossible_struct
_tmp874;const char*_tmp873;struct Cyc_Core_Impossible_struct*_tmp872;(int)_throw((
void*)((_tmp872=_cycalloc(sizeof(*_tmp872)),((_tmp872[0]=((_tmp874.tag=Cyc_Core_Impossible,((
_tmp874.f1=((_tmp873="anal_Rexp: Subscript",_tag_dyneither(_tmp873,sizeof(char),
21))),_tmp874)))),_tmp872)))));}_LL1E3:;}_LL1DF: {struct Cyc_Absyn_PointerType_struct*
_tmp348=(struct Cyc_Absyn_PointerType_struct*)_tmp345;if(_tmp348->tag != 5)goto
_LL1E1;else{_tmp349=_tmp348->f1;_tmp34A=_tmp349.ptr_atts;_tmp34B=_tmp34A.bounds;}}
_LL1E0: {union Cyc_CfFlowInfo_FlowInfo _tmp355;void*_tmp356;struct _tuple14 _tmp354=
Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp33B,_tmp1F6,(void*)((struct Cyc_List_List*)
_check_null(_tmp33C))->hd);_tmp355=_tmp354.f1;_tmp356=_tmp354.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp357=_tmp355;int _tmp358;_LL1E9: if((_tmp357.BottomFL).tag != 1)goto _LL1EB;
_tmp358=(int)(_tmp357.BottomFL).val;_LL1EA: {struct _tuple14 _tmp875;struct
_tuple14 _tmp35A=(_tmp875.f1=_tmp355,((_tmp875.f2=_tmp356,_tmp875)));
_npop_handler(0);return _tmp35A;}_LL1EB:;_LL1EC: {struct _tuple14 _tmp876;struct
_tuple14 _tmp35C=(_tmp876.f1=_tmp33D,((_tmp876.f2=_tmp356,_tmp876)));
_npop_handler(0);return _tmp35C;}_LL1E8:;};}_LL1E1:;_LL1E2: {struct Cyc_Core_Impossible_struct
_tmp87C;const char*_tmp87B;struct Cyc_Core_Impossible_struct*_tmp87A;(int)_throw((
void*)((_tmp87A=_cycalloc(sizeof(*_tmp87A)),((_tmp87A[0]=((_tmp87C.tag=Cyc_Core_Impossible,((
_tmp87C.f1=((_tmp87B="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp87B,
sizeof(char),33))),_tmp87C)))),_tmp87A)))));}_LL1DC:;};};};_pop_region(temp);}
_LL153: {struct Cyc_Absyn_Datatype_e_struct*_tmp1F8=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp18D;if(_tmp1F8->tag != 31)goto _LL155;else{_tmp1F9=_tmp1F8->f1;_tmp1FA=_tmp1F8->f2;}}
_LL154: _tmp1FC=_tmp1F9;goto _LL156;_LL155: {struct Cyc_Absyn_Tuple_e_struct*
_tmp1FB=(struct Cyc_Absyn_Tuple_e_struct*)_tmp18D;if(_tmp1FB->tag != 25)goto _LL157;
else{_tmp1FC=_tmp1FB->f1;}}_LL156: {struct _RegionHandle _tmp360=_new_region("temp");
struct _RegionHandle*temp=& _tmp360;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo
_tmp362;struct Cyc_List_List*_tmp363;struct _tuple20 _tmp361=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp1FC,1,1);_tmp362=_tmp361.f1;_tmp363=_tmp361.f2;{unsigned int
_tmp882;unsigned int _tmp881;struct _dyneither_ptr _tmp880;void**_tmp87F;
unsigned int _tmp87E;struct _dyneither_ptr aggrdict=(_tmp87E=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1FC),((_tmp87F=(void**)
_region_malloc(env->r,_check_times(sizeof(void*),_tmp87E)),((_tmp880=
_tag_dyneither(_tmp87F,sizeof(void*),_tmp87E),((((_tmp881=_tmp87E,_tmp883(&
_tmp882,& _tmp881,& _tmp87F,& _tmp363))),_tmp880)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp88D;struct Cyc_CfFlowInfo_UnionRInfo _tmp88C;struct Cyc_CfFlowInfo_Aggregate_struct
_tmp88B;struct _tuple14 _tmp88A;struct _tuple14 _tmp368=(_tmp88A.f1=_tmp362,((
_tmp88A.f2=(void*)((_tmp88D=_region_malloc(env->r,sizeof(*_tmp88D)),((_tmp88D[0]=((
_tmp88B.tag=6,((_tmp88B.f1=((_tmp88C.is_union=0,((_tmp88C.fieldnum=- 1,_tmp88C)))),((
_tmp88B.f2=aggrdict,_tmp88B)))))),_tmp88D)))),_tmp88A)));_npop_handler(0);return
_tmp368;};};_pop_region(temp);}_LL157: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1FD=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp18D;if(_tmp1FD->tag != 30)goto
_LL159;else{_tmp1FE=_tmp1FD->f2;}}_LL158: {struct Cyc_List_List*fs;{void*_tmp36E=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_List_List*_tmp370;_LL1EE: {struct Cyc_Absyn_AnonAggrType_struct*_tmp36F=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp36E;if(_tmp36F->tag != 12)goto _LL1F0;
else{_tmp370=_tmp36F->f2;}}_LL1EF: fs=_tmp370;goto _LL1ED;_LL1F0:;_LL1F1: {struct
Cyc_Core_Impossible_struct _tmp893;const char*_tmp892;struct Cyc_Core_Impossible_struct*
_tmp891;(int)_throw((void*)((_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891[0]=((
_tmp893.tag=Cyc_Core_Impossible,((_tmp893.f1=((_tmp892="anal_Rexp:anon struct has bad type",
_tag_dyneither(_tmp892,sizeof(char),35))),_tmp893)))),_tmp891)))));}_LL1ED:;}
_tmp200=_tmp1FE;_tmp203=Cyc_Absyn_StructA;_tmp206=fs;goto _LL15A;}_LL159: {struct
Cyc_Absyn_Aggregate_e_struct*_tmp1FF=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp18D;if(_tmp1FF->tag != 29)goto _LL15B;else{_tmp200=_tmp1FF->f3;_tmp201=_tmp1FF->f4;
if(_tmp201 == 0)goto _LL15B;_tmp202=*_tmp201;_tmp203=_tmp202.kind;_tmp204=_tmp202.impl;
if(_tmp204 == 0)goto _LL15B;_tmp205=*_tmp204;_tmp206=_tmp205.fields;}}_LL15A: {
struct _RegionHandle _tmp374=_new_region("temp");struct _RegionHandle*temp=& _tmp374;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo _tmp376;struct Cyc_List_List*
_tmp377;struct _tuple20 _tmp375=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple22*))Cyc_Core_snd,_tmp200),1,1);_tmp376=_tmp375.f1;_tmp377=_tmp375.f2;{
struct _dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp173,
_tmp206,_tmp203 == Cyc_Absyn_UnionA,_tmp203 == Cyc_Absyn_UnionA?_tmp173->unknown_none:
_tmp173->unknown_all);int field_no=- 1;{int i=0;for(0;_tmp377 != 0;(((_tmp377=
_tmp377->tl,_tmp200=_tmp200->tl)),++ i)){struct Cyc_List_List*ds=(*((struct
_tuple22*)((struct Cyc_List_List*)_check_null(_tmp200))->hd)).f1;for(0;ds != 0;ds=
ds->tl){void*_tmp378=(void*)ds->hd;struct _dyneither_ptr*_tmp37B;_LL1F3: {struct
Cyc_Absyn_ArrayElement_struct*_tmp379=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp378;if(_tmp379->tag != 0)goto _LL1F5;}_LL1F4: {struct Cyc_Core_Impossible_struct
_tmp899;const char*_tmp898;struct Cyc_Core_Impossible_struct*_tmp897;(int)_throw((
void*)((_tmp897=_cycalloc(sizeof(*_tmp897)),((_tmp897[0]=((_tmp899.tag=Cyc_Core_Impossible,((
_tmp899.f1=((_tmp898="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp898,sizeof(char),
22))),_tmp899)))),_tmp897)))));}_LL1F5: {struct Cyc_Absyn_FieldName_struct*
_tmp37A=(struct Cyc_Absyn_FieldName_struct*)_tmp378;if(_tmp37A->tag != 1)goto
_LL1F2;else{_tmp37B=_tmp37A->f1;}}_LL1F6: field_no=Cyc_CfFlowInfo_get_field_index_fs(
_tmp206,_tmp37B);*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),
field_no))=(void*)_tmp377->hd;if(_tmp203 == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*
_tmp37F=(*((struct _tuple22*)_tmp200->hd)).f2;_tmp376=Cyc_NewControlFlow_use_Rval(
env,_tmp37F->loc,_tmp376,(void*)_tmp377->hd);}_LL1F2:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp8A3;struct Cyc_CfFlowInfo_UnionRInfo _tmp8A2;struct Cyc_CfFlowInfo_Aggregate_struct
_tmp8A1;struct _tuple14 _tmp8A0;struct _tuple14 _tmp384=(_tmp8A0.f1=_tmp376,((
_tmp8A0.f2=(void*)((_tmp8A3=_region_malloc(env->r,sizeof(*_tmp8A3)),((_tmp8A3[0]=((
_tmp8A1.tag=6,((_tmp8A1.f1=((_tmp8A2.is_union=_tmp203 == Cyc_Absyn_UnionA,((
_tmp8A2.fieldnum=field_no,_tmp8A2)))),((_tmp8A1.f2=aggrdict,_tmp8A1)))))),
_tmp8A3)))),_tmp8A0)));_npop_handler(0);return _tmp384;};};};_pop_region(temp);}
_LL15B: {struct Cyc_Absyn_Aggregate_e_struct*_tmp207=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp18D;if(_tmp207->tag != 29)goto _LL15D;}_LL15C: {struct Cyc_Core_Impossible_struct
_tmp8A9;const char*_tmp8A8;struct Cyc_Core_Impossible_struct*_tmp8A7;(int)_throw((
void*)((_tmp8A7=_cycalloc(sizeof(*_tmp8A7)),((_tmp8A7[0]=((_tmp8A9.tag=Cyc_Core_Impossible,((
_tmp8A9.f1=((_tmp8A8="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp8A8,
sizeof(char),31))),_tmp8A9)))),_tmp8A7)))));}_LL15D: {struct Cyc_Absyn_Array_e_struct*
_tmp208=(struct Cyc_Absyn_Array_e_struct*)_tmp18D;if(_tmp208->tag != 27)goto _LL15F;
else{_tmp209=_tmp208->f1;}}_LL15E: {struct _RegionHandle _tmp388=_new_region("temp");
struct _RegionHandle*temp=& _tmp388;_push_region(temp);{struct Cyc_List_List*
_tmp389=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple22*))Cyc_Core_snd,_tmp209);union Cyc_CfFlowInfo_FlowInfo _tmp38B;
struct Cyc_List_List*_tmp38C;struct _tuple20 _tmp38A=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp389,1,1);_tmp38B=_tmp38A.f1;_tmp38C=_tmp38A.f2;for(0;_tmp38C
!= 0;(_tmp38C=_tmp38C->tl,_tmp389=_tmp389->tl)){_tmp38B=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp389))->hd)->loc,
_tmp38B,(void*)_tmp38C->hd);}{struct _tuple14 _tmp8AA;struct _tuple14 _tmp38E=(
_tmp8AA.f1=_tmp38B,((_tmp8AA.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,0,_tmp173->unknown_all),_tmp8AA)));
_npop_handler(0);return _tmp38E;};};_pop_region(temp);}_LL15F: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp20A=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp18D;if(_tmp20A->tag != 28)
goto _LL161;else{_tmp20B=_tmp20A->f1;_tmp20C=_tmp20A->f2;_tmp20D=_tmp20A->f3;
_tmp20E=_tmp20A->f4;}}_LL160: {union Cyc_CfFlowInfo_FlowInfo _tmp390;void*_tmp391;
struct _tuple14 _tmp38F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp20C);_tmp390=
_tmp38F.f1;_tmp391=_tmp38F.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp392=_tmp390;int
_tmp393;struct _tuple13 _tmp394;struct Cyc_Dict_Dict _tmp395;struct Cyc_List_List*
_tmp396;_LL1F8: if((_tmp392.BottomFL).tag != 1)goto _LL1FA;_tmp393=(int)(_tmp392.BottomFL).val;
_LL1F9: {struct _tuple14 _tmp8AB;return(_tmp8AB.f1=_tmp390,((_tmp8AB.f2=_tmp173->unknown_all,
_tmp8AB)));}_LL1FA: if((_tmp392.ReachableFL).tag != 2)goto _LL1F7;_tmp394=(struct
_tuple13)(_tmp392.ReachableFL).val;_tmp395=_tmp394.f1;_tmp396=_tmp394.f2;_LL1FB:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp395,_tmp391)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8AE;void*_tmp8AD;(_tmp8AD=0,Cyc_Tcutil_terr(_tmp20C->loc,((_tmp8AE="expression may not be initialized",
_tag_dyneither(_tmp8AE,sizeof(char),34))),_tag_dyneither(_tmp8AD,sizeof(void*),0)));}{
struct Cyc_List_List*new_relns=_tmp396;comp_loop: {void*_tmp39A=_tmp20C->r;struct
Cyc_Absyn_Exp*_tmp39C;void*_tmp39E;struct Cyc_Absyn_Vardecl*_tmp3A0;void*_tmp3A2;
struct Cyc_Absyn_Vardecl*_tmp3A4;void*_tmp3A6;struct Cyc_Absyn_Vardecl*_tmp3A8;
void*_tmp3AA;struct Cyc_Absyn_Vardecl*_tmp3AC;union Cyc_Absyn_Cnst _tmp3AE;struct
_tuple6 _tmp3AF;int _tmp3B0;enum Cyc_Absyn_Primop _tmp3B2;struct Cyc_List_List*
_tmp3B3;struct Cyc_List_List _tmp3B4;struct Cyc_Absyn_Exp*_tmp3B5;_LL1FD: {struct
Cyc_Absyn_Cast_e_struct*_tmp39B=(struct Cyc_Absyn_Cast_e_struct*)_tmp39A;if(
_tmp39B->tag != 15)goto _LL1FF;else{_tmp39C=_tmp39B->f2;}}_LL1FE: _tmp20C=_tmp39C;
goto comp_loop;_LL1FF:{struct Cyc_Absyn_Var_e_struct*_tmp39D=(struct Cyc_Absyn_Var_e_struct*)
_tmp39A;if(_tmp39D->tag != 1)goto _LL201;else{_tmp39E=(void*)_tmp39D->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp39F=(struct Cyc_Absyn_Global_b_struct*)_tmp39E;if(
_tmp39F->tag != 1)goto _LL201;else{_tmp3A0=_tmp39F->f1;}};}}if(!(!_tmp3A0->escapes))
goto _LL201;_LL200: _tmp3A4=_tmp3A0;goto _LL202;_LL201:{struct Cyc_Absyn_Var_e_struct*
_tmp3A1=(struct Cyc_Absyn_Var_e_struct*)_tmp39A;if(_tmp3A1->tag != 1)goto _LL203;
else{_tmp3A2=(void*)_tmp3A1->f2;{struct Cyc_Absyn_Local_b_struct*_tmp3A3=(struct
Cyc_Absyn_Local_b_struct*)_tmp3A2;if(_tmp3A3->tag != 4)goto _LL203;else{_tmp3A4=
_tmp3A3->f1;}};}}if(!(!_tmp3A4->escapes))goto _LL203;_LL202: _tmp3A8=_tmp3A4;goto
_LL204;_LL203:{struct Cyc_Absyn_Var_e_struct*_tmp3A5=(struct Cyc_Absyn_Var_e_struct*)
_tmp39A;if(_tmp3A5->tag != 1)goto _LL205;else{_tmp3A6=(void*)_tmp3A5->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp3A7=(struct Cyc_Absyn_Pat_b_struct*)_tmp3A6;if(_tmp3A7->tag
!= 5)goto _LL205;else{_tmp3A8=_tmp3A7->f1;}};}}if(!(!_tmp3A8->escapes))goto _LL205;
_LL204: _tmp3AC=_tmp3A8;goto _LL206;_LL205:{struct Cyc_Absyn_Var_e_struct*_tmp3A9=(
struct Cyc_Absyn_Var_e_struct*)_tmp39A;if(_tmp3A9->tag != 1)goto _LL207;else{
_tmp3AA=(void*)_tmp3A9->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3AB=(struct Cyc_Absyn_Param_b_struct*)
_tmp3AA;if(_tmp3AB->tag != 3)goto _LL207;else{_tmp3AC=_tmp3AB->f1;}};}}if(!(!
_tmp3AC->escapes))goto _LL207;_LL206:{struct Cyc_CfFlowInfo_Reln*_tmp8B1;struct Cyc_List_List*
_tmp8B0;new_relns=((_tmp8B0=_region_malloc(env->r,sizeof(*_tmp8B0)),((_tmp8B0->hd=((
_tmp8B1=_region_malloc(env->r,sizeof(*_tmp8B1)),((_tmp8B1->vd=_tmp20B,((_tmp8B1->rop=
Cyc_CfFlowInfo_LessVar(_tmp3AC,_tmp3AC->type),_tmp8B1)))))),((_tmp8B0->tl=
_tmp396,_tmp8B0))))));}goto _LL1FC;_LL207: {struct Cyc_Absyn_Const_e_struct*
_tmp3AD=(struct Cyc_Absyn_Const_e_struct*)_tmp39A;if(_tmp3AD->tag != 0)goto _LL209;
else{_tmp3AE=_tmp3AD->f1;if((_tmp3AE.Int_c).tag != 5)goto _LL209;_tmp3AF=(struct
_tuple6)(_tmp3AE.Int_c).val;_tmp3B0=_tmp3AF.f2;}}_LL208:{struct Cyc_CfFlowInfo_Reln*
_tmp8B4;struct Cyc_List_List*_tmp8B3;new_relns=((_tmp8B3=_region_malloc(env->r,
sizeof(*_tmp8B3)),((_tmp8B3->hd=((_tmp8B4=_region_malloc(env->r,sizeof(*_tmp8B4)),((
_tmp8B4->vd=_tmp20B,((_tmp8B4->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp3B0),_tmp8B4)))))),((_tmp8B3->tl=_tmp396,_tmp8B3))))));}goto _LL1FC;_LL209: {
struct Cyc_Absyn_Primop_e_struct*_tmp3B1=(struct Cyc_Absyn_Primop_e_struct*)
_tmp39A;if(_tmp3B1->tag != 3)goto _LL20B;else{_tmp3B2=_tmp3B1->f1;_tmp3B3=_tmp3B1->f2;
if(_tmp3B3 == 0)goto _LL20B;_tmp3B4=*_tmp3B3;_tmp3B5=(struct Cyc_Absyn_Exp*)_tmp3B4.hd;}}
_LL20A:{void*_tmp3BA=_tmp3B5->r;void*_tmp3BC;struct Cyc_Absyn_Vardecl*_tmp3BE;
void*_tmp3C0;struct Cyc_Absyn_Vardecl*_tmp3C2;void*_tmp3C4;struct Cyc_Absyn_Vardecl*
_tmp3C6;void*_tmp3C8;struct Cyc_Absyn_Vardecl*_tmp3CA;_LL20E:{struct Cyc_Absyn_Var_e_struct*
_tmp3BB=(struct Cyc_Absyn_Var_e_struct*)_tmp3BA;if(_tmp3BB->tag != 1)goto _LL210;
else{_tmp3BC=(void*)_tmp3BB->f2;{struct Cyc_Absyn_Global_b_struct*_tmp3BD=(struct
Cyc_Absyn_Global_b_struct*)_tmp3BC;if(_tmp3BD->tag != 1)goto _LL210;else{_tmp3BE=
_tmp3BD->f1;}};}}if(!(!_tmp3BE->escapes))goto _LL210;_LL20F: _tmp3C2=_tmp3BE;goto
_LL211;_LL210:{struct Cyc_Absyn_Var_e_struct*_tmp3BF=(struct Cyc_Absyn_Var_e_struct*)
_tmp3BA;if(_tmp3BF->tag != 1)goto _LL212;else{_tmp3C0=(void*)_tmp3BF->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp3C1=(struct Cyc_Absyn_Local_b_struct*)_tmp3C0;if(
_tmp3C1->tag != 4)goto _LL212;else{_tmp3C2=_tmp3C1->f1;}};}}if(!(!_tmp3C2->escapes))
goto _LL212;_LL211: _tmp3C6=_tmp3C2;goto _LL213;_LL212:{struct Cyc_Absyn_Var_e_struct*
_tmp3C3=(struct Cyc_Absyn_Var_e_struct*)_tmp3BA;if(_tmp3C3->tag != 1)goto _LL214;
else{_tmp3C4=(void*)_tmp3C3->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp3C5=(struct Cyc_Absyn_Pat_b_struct*)
_tmp3C4;if(_tmp3C5->tag != 5)goto _LL214;else{_tmp3C6=_tmp3C5->f1;}};}}if(!(!
_tmp3C6->escapes))goto _LL214;_LL213: _tmp3CA=_tmp3C6;goto _LL215;_LL214:{struct Cyc_Absyn_Var_e_struct*
_tmp3C7=(struct Cyc_Absyn_Var_e_struct*)_tmp3BA;if(_tmp3C7->tag != 1)goto _LL216;
else{_tmp3C8=(void*)_tmp3C7->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3C9=(struct
Cyc_Absyn_Param_b_struct*)_tmp3C8;if(_tmp3C9->tag != 3)goto _LL216;else{_tmp3CA=
_tmp3C9->f1;}};}}if(!(!_tmp3CA->escapes))goto _LL216;_LL215:{struct Cyc_CfFlowInfo_Reln*
_tmp8B7;struct Cyc_List_List*_tmp8B6;new_relns=((_tmp8B6=_region_malloc(env->r,
sizeof(*_tmp8B6)),((_tmp8B6->hd=((_tmp8B7=_region_malloc(env->r,sizeof(*_tmp8B7)),((
_tmp8B7->vd=_tmp20B,((_tmp8B7->rop=Cyc_CfFlowInfo_LessNumelts(_tmp3CA),_tmp8B7)))))),((
_tmp8B6->tl=_tmp396,_tmp8B6))))));}goto _LL20D;_LL216:;_LL217: goto _LL20D;_LL20D:;}
goto _LL1FC;_LL20B:;_LL20C: goto _LL1FC;_LL1FC:;}if(_tmp396 != new_relns)_tmp390=Cyc_CfFlowInfo_ReachableFL(
_tmp395,new_relns);{void*_tmp3CD=_tmp391;_LL219: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3CE=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3CD;if(_tmp3CE->tag != 0)goto
_LL21B;}_LL21A: {struct _tuple14 _tmp8B8;return(_tmp8B8.f1=_tmp390,((_tmp8B8.f2=
_tmp173->unknown_all,_tmp8B8)));}_LL21B: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3CF=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3CD;if(_tmp3CF->tag != 2)
goto _LL21D;}_LL21C: goto _LL21E;_LL21D: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3D0=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3CD;if(_tmp3D0->tag != 1)
goto _LL21F;}_LL21E: goto _LL220;_LL21F: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3D1=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3CD;if(_tmp3D1->tag != 5)goto
_LL221;}_LL220: {struct Cyc_List_List _tmp8B9;struct Cyc_List_List _tmp3D3=(_tmp8B9.hd=
_tmp20B,((_tmp8B9.tl=0,_tmp8B9)));_tmp390=Cyc_NewControlFlow_add_vars(_tmp173,
_tmp390,(struct Cyc_List_List*)& _tmp3D3,_tmp173->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3D5;void*_tmp3D6;struct _tuple14 _tmp3D4=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp390,_tmp20D);_tmp3D5=_tmp3D4.f1;_tmp3D6=_tmp3D4.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3D7=_tmp3D5;int _tmp3D8;struct _tuple13 _tmp3D9;struct Cyc_Dict_Dict _tmp3DA;
_LL224: if((_tmp3D7.BottomFL).tag != 1)goto _LL226;_tmp3D8=(int)(_tmp3D7.BottomFL).val;
_LL225: {struct _tuple14 _tmp8BA;return(_tmp8BA.f1=_tmp3D5,((_tmp8BA.f2=_tmp173->unknown_all,
_tmp8BA)));}_LL226: if((_tmp3D7.ReachableFL).tag != 2)goto _LL223;_tmp3D9=(struct
_tuple13)(_tmp3D7.ReachableFL).val;_tmp3DA=_tmp3D9.f1;_LL227: if(Cyc_CfFlowInfo_initlevel(
_tmp173,_tmp3DA,_tmp3D6)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp8BD;void*_tmp8BC;(
_tmp8BC=0,Cyc_Tcutil_terr(_tmp20C->loc,((_tmp8BD="expression may not be initialized",
_tag_dyneither(_tmp8BD,sizeof(char),34))),_tag_dyneither(_tmp8BC,sizeof(void*),0)));}{
struct _tuple14 _tmp8BE;return(_tmp8BE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8BE.f2=
_tmp173->unknown_all,_tmp8BE)));};}_LL223:;}_tmp390=_tmp3D5;goto _LL222;};}_LL221:;
_LL222: while(1){struct Cyc_List_List _tmp8BF;struct Cyc_List_List _tmp3E0=(_tmp8BF.hd=
_tmp20B,((_tmp8BF.tl=0,_tmp8BF)));_tmp390=Cyc_NewControlFlow_add_vars(_tmp173,
_tmp390,(struct Cyc_List_List*)& _tmp3E0,_tmp173->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3E2;void*_tmp3E3;struct _tuple14 _tmp3E1=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp390,_tmp20D);_tmp3E2=_tmp3E1.f1;_tmp3E3=_tmp3E1.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3E4=_tmp3E2;int _tmp3E5;struct _tuple13 _tmp3E6;struct Cyc_Dict_Dict _tmp3E7;
_LL229: if((_tmp3E4.BottomFL).tag != 1)goto _LL22B;_tmp3E5=(int)(_tmp3E4.BottomFL).val;
_LL22A: goto _LL228;_LL22B: if((_tmp3E4.ReachableFL).tag != 2)goto _LL228;_tmp3E6=(
struct _tuple13)(_tmp3E4.ReachableFL).val;_tmp3E7=_tmp3E6.f1;_LL22C: if(Cyc_CfFlowInfo_initlevel(
_tmp173,_tmp3E7,_tmp3E3)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp8C2;void*_tmp8C1;(
_tmp8C1=0,Cyc_Tcutil_terr(_tmp20C->loc,((_tmp8C2="expression may not be initialized",
_tag_dyneither(_tmp8C2,sizeof(char),34))),_tag_dyneither(_tmp8C1,sizeof(void*),0)));}{
struct _tuple14 _tmp8C3;return(_tmp8C3.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8C3.f2=
_tmp173->unknown_all,_tmp8C3)));};}_LL228:;}{union Cyc_CfFlowInfo_FlowInfo _tmp3EB=
Cyc_CfFlowInfo_join_flow(_tmp173,env->all_changed,_tmp390,_tmp3E2);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3EB,_tmp390))break;_tmp390=_tmp3EB;};};}{struct _tuple14 _tmp8C4;return(
_tmp8C4.f1=_tmp390,((_tmp8C4.f2=_tmp173->unknown_all,_tmp8C4)));};_LL218:;};};
_LL1F7:;};}_LL161: {struct Cyc_Absyn_StmtExp_e_struct*_tmp20F=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp18D;if(_tmp20F->tag != 37)goto _LL163;else{_tmp210=_tmp20F->f1;}}_LL162: return
Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp210);_LL163: {struct
Cyc_Absyn_Var_e_struct*_tmp211=(struct Cyc_Absyn_Var_e_struct*)_tmp18D;if(_tmp211->tag
!= 1)goto _LL165;else{_tmp212=(void*)_tmp211->f2;{struct Cyc_Absyn_Unresolved_b_struct*
_tmp213=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp212;if(_tmp213->tag != 0)goto
_LL165;};}}_LL164: goto _LL166;_LL165: {struct Cyc_Absyn_UnknownId_e_struct*_tmp214=(
struct Cyc_Absyn_UnknownId_e_struct*)_tmp18D;if(_tmp214->tag != 2)goto _LL167;}
_LL166: goto _LL168;_LL167: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp215=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp18D;if(_tmp215->tag != 10)goto _LL169;}_LL168:
goto _LL16A;_LL169: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp216=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp18D;if(_tmp216->tag != 36)goto _LL16B;}_LL16A: goto _LL16C;_LL16B: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp217=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp18D;if(_tmp217->tag != 26)goto
_LL16D;}_LL16C: goto _LL16E;_LL16D: {struct Cyc_Absyn_Valueof_e_struct*_tmp218=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp18D;if(_tmp218->tag != 39)goto _LL16F;}
_LL16E: goto _LL170;_LL16F: {struct Cyc_Absyn_Asm_e_struct*_tmp219=(struct Cyc_Absyn_Asm_e_struct*)
_tmp18D;if(_tmp219->tag != 40)goto _LL106;}_LL170: {struct Cyc_Core_Impossible_struct
_tmp8CA;const char*_tmp8C9;struct Cyc_Core_Impossible_struct*_tmp8C8;(int)_throw((
void*)((_tmp8C8=_cycalloc(sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8CA.tag=Cyc_Core_Impossible,((
_tmp8CA.f1=((_tmp8C9="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp8C9,
sizeof(char),31))),_tmp8CA)))),_tmp8C8)))));}_LL106:;};}static struct _tuple15 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3F1=env->fenv;void*_tmp3F2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp3F4;void*_tmp3F5;struct Cyc_Absyn_PtrAtts _tmp3F6;union Cyc_Absyn_Constraint*
_tmp3F7;union Cyc_Absyn_Constraint*_tmp3F8;_LL22E: {struct Cyc_Absyn_PointerType_struct*
_tmp3F3=(struct Cyc_Absyn_PointerType_struct*)_tmp3F2;if(_tmp3F3->tag != 5)goto
_LL230;else{_tmp3F4=_tmp3F3->f1;_tmp3F5=_tmp3F4.elt_typ;_tmp3F6=_tmp3F4.ptr_atts;
_tmp3F7=_tmp3F6.bounds;_tmp3F8=_tmp3F6.zero_term;}}_LL22F: {union Cyc_CfFlowInfo_FlowInfo
_tmp3F9=f;int _tmp3FA;struct _tuple13 _tmp3FB;struct Cyc_Dict_Dict _tmp3FC;struct Cyc_List_List*
_tmp3FD;_LL233: if((_tmp3F9.BottomFL).tag != 1)goto _LL235;_tmp3FA=(int)(_tmp3F9.BottomFL).val;
_LL234: {struct _tuple15 _tmp8CB;return(_tmp8CB.f1=f,((_tmp8CB.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8CB)));}_LL235: if((_tmp3F9.ReachableFL).tag != 2)goto _LL232;_tmp3FB=(struct
_tuple13)(_tmp3F9.ReachableFL).val;_tmp3FC=_tmp3FB.f1;_tmp3FD=_tmp3FB.f2;_LL236:
if(Cyc_Tcutil_is_bound_one(_tmp3F7)){void*_tmp3FF=r;struct Cyc_CfFlowInfo_Place*
_tmp403;struct Cyc_CfFlowInfo_Place _tmp404;void*_tmp405;struct Cyc_List_List*
_tmp406;enum Cyc_CfFlowInfo_InitLevel _tmp409;_LL238: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp400=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3FF;if(_tmp400->tag != 1)
goto _LL23A;}_LL239: goto _LL23B;_LL23A: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp401=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3FF;if(_tmp401->tag != 2)
goto _LL23C;}_LL23B:{struct Cyc_CfFlowInfo_NotZero_struct _tmp8CE;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp8CD;e->annot=(void*)((_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD[0]=((
_tmp8CE.tag=Cyc_CfFlowInfo_NotZero,((_tmp8CE.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3FD),_tmp8CE)))),_tmp8CD))));}goto _LL237;_LL23C: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp402=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3FF;if(_tmp402->tag != 5)goto
_LL23E;else{_tmp403=_tmp402->f1;_tmp404=*_tmp403;_tmp405=_tmp404.root;_tmp406=
_tmp404.fields;}}_LL23D:{struct Cyc_CfFlowInfo_NotZero_struct _tmp8D1;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp8D0;e->annot=(void*)((_tmp8D0=_cycalloc(sizeof(*_tmp8D0)),((_tmp8D0[0]=((
_tmp8D1.tag=Cyc_CfFlowInfo_NotZero,((_tmp8D1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3FD),_tmp8D1)))),_tmp8D0))));}{struct Cyc_CfFlowInfo_Place*_tmp8D4;struct
_tuple15 _tmp8D3;return(_tmp8D3.f1=f,((_tmp8D3.f2=Cyc_CfFlowInfo_PlaceL(((_tmp8D4=
_region_malloc(_tmp3F1->r,sizeof(*_tmp8D4)),((_tmp8D4->root=_tmp405,((_tmp8D4->fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(_tmp3F1->r,_tmp406,flds),_tmp8D4))))))),_tmp8D3)));};_LL23E: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp407=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp3FF;if(_tmp407->tag != 0)goto _LL240;}_LL23F: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp8D5;return(_tmp8D5.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8D5.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp8D5)));};_LL240: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp408=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3FF;if(_tmp408->tag != 3)goto
_LL242;else{_tmp409=_tmp408->f1;}}_LL241: f=Cyc_NewControlFlow_notzero(env,inflow,
f,e,_tmp409);goto _LL243;_LL242:;_LL243: {struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp8D8;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp8D7;e->annot=(void*)((_tmp8D7=
_cycalloc(sizeof(*_tmp8D7)),((_tmp8D7[0]=((_tmp8D8.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp8D8.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3FD),_tmp8D8)))),
_tmp8D7))));}_LL237:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp8DB;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp8DA;e->annot=(void*)((_tmp8DA=_cycalloc(sizeof(*_tmp8DA)),((_tmp8DA[0]=((
_tmp8DB.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8DB.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3FD),_tmp8DB)))),_tmp8DA))));}if(Cyc_CfFlowInfo_initlevel(_tmp3F1,_tmp3FC,r)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp8DE;void*_tmp8DD;(_tmp8DD=0,Cyc_Tcutil_terr(
e->loc,((_tmp8DE="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp8DE,sizeof(char),46))),_tag_dyneither(_tmp8DD,sizeof(void*),0)));}{struct
_tuple15 _tmp8DF;return(_tmp8DF.f1=f,((_tmp8DF.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8DF)));};_LL232:;}_LL230:;_LL231: {struct Cyc_Core_Impossible_struct _tmp8E5;
const char*_tmp8E4;struct Cyc_Core_Impossible_struct*_tmp8E3;(int)_throw((void*)((
_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E5.tag=Cyc_Core_Impossible,((
_tmp8E5.f1=((_tmp8E4="left deref of non-pointer-type",_tag_dyneither(_tmp8E4,
sizeof(char),31))),_tmp8E5)))),_tmp8E3)))));}_LL22D:;}static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int
expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){struct Cyc_Dict_Dict
d;struct Cyc_CfFlowInfo_FlowEnv*_tmp41B=env->fenv;{union Cyc_CfFlowInfo_FlowInfo
_tmp41C=inflow;int _tmp41D;struct _tuple13 _tmp41E;struct Cyc_Dict_Dict _tmp41F;
struct Cyc_List_List*_tmp420;_LL245: if((_tmp41C.BottomFL).tag != 1)goto _LL247;
_tmp41D=(int)(_tmp41C.BottomFL).val;_LL246: {struct _tuple15 _tmp8E6;return(
_tmp8E6.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8E6.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8E6)));}_LL247: if((_tmp41C.ReachableFL).tag != 2)goto _LL244;_tmp41E=(struct
_tuple13)(_tmp41C.ReachableFL).val;_tmp41F=_tmp41E.f1;_tmp420=_tmp41E.f2;_LL248:
d=_tmp41F;_LL244:;}{void*_tmp422=e->r;struct Cyc_Absyn_Exp*_tmp424;struct Cyc_Absyn_Exp*
_tmp426;struct Cyc_Absyn_Exp*_tmp428;void*_tmp42A;struct Cyc_Absyn_Vardecl*_tmp42C;
void*_tmp42E;struct Cyc_Absyn_Vardecl*_tmp430;void*_tmp432;struct Cyc_Absyn_Vardecl*
_tmp434;void*_tmp436;struct Cyc_Absyn_Vardecl*_tmp438;struct Cyc_Absyn_Exp*_tmp43A;
struct _dyneither_ptr*_tmp43B;struct Cyc_Absyn_Exp*_tmp43D;struct Cyc_Absyn_Exp*
_tmp43F;struct Cyc_Absyn_Exp*_tmp440;struct Cyc_Absyn_Exp*_tmp442;struct
_dyneither_ptr*_tmp443;_LL24A: {struct Cyc_Absyn_Cast_e_struct*_tmp423=(struct Cyc_Absyn_Cast_e_struct*)
_tmp422;if(_tmp423->tag != 15)goto _LL24C;else{_tmp424=_tmp423->f2;}}_LL24B:
_tmp426=_tmp424;goto _LL24D;_LL24C: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp425=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp422;if(_tmp425->tag != 13)
goto _LL24E;else{_tmp426=_tmp425->f1;}}_LL24D: _tmp428=_tmp426;goto _LL24F;_LL24E: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp427=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp422;if(_tmp427->tag != 14)goto _LL250;else{_tmp428=_tmp427->f1;}}_LL24F: return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp428,flds);_LL250: {
struct Cyc_Absyn_Var_e_struct*_tmp429=(struct Cyc_Absyn_Var_e_struct*)_tmp422;if(
_tmp429->tag != 1)goto _LL252;else{_tmp42A=(void*)_tmp429->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp42B=(struct Cyc_Absyn_Param_b_struct*)_tmp42A;if(_tmp42B->tag != 3)goto _LL252;
else{_tmp42C=_tmp42B->f1;}};}}_LL251: _tmp430=_tmp42C;goto _LL253;_LL252: {struct
Cyc_Absyn_Var_e_struct*_tmp42D=(struct Cyc_Absyn_Var_e_struct*)_tmp422;if(_tmp42D->tag
!= 1)goto _LL254;else{_tmp42E=(void*)_tmp42D->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp42F=(struct Cyc_Absyn_Local_b_struct*)_tmp42E;if(_tmp42F->tag != 4)goto _LL254;
else{_tmp430=_tmp42F->f1;}};}}_LL253: _tmp434=_tmp430;goto _LL255;_LL254: {struct
Cyc_Absyn_Var_e_struct*_tmp431=(struct Cyc_Absyn_Var_e_struct*)_tmp422;if(_tmp431->tag
!= 1)goto _LL256;else{_tmp432=(void*)_tmp431->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp433=(struct Cyc_Absyn_Pat_b_struct*)_tmp432;if(_tmp433->tag != 5)goto _LL256;
else{_tmp434=_tmp433->f1;}};}}_LL255: {struct Cyc_CfFlowInfo_Place*_tmp8F0;struct
Cyc_CfFlowInfo_VarRoot_struct _tmp8EF;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp8EE;
struct _tuple15 _tmp8ED;return(_tmp8ED.f1=inflow,((_tmp8ED.f2=Cyc_CfFlowInfo_PlaceL(((
_tmp8F0=_region_malloc(env->r,sizeof(*_tmp8F0)),((_tmp8F0->root=(void*)((_tmp8EE=
_region_malloc(env->r,sizeof(*_tmp8EE)),((_tmp8EE[0]=((_tmp8EF.tag=0,((_tmp8EF.f1=
_tmp434,_tmp8EF)))),_tmp8EE)))),((_tmp8F0->fields=flds,_tmp8F0))))))),_tmp8ED)));}
_LL256: {struct Cyc_Absyn_Var_e_struct*_tmp435=(struct Cyc_Absyn_Var_e_struct*)
_tmp422;if(_tmp435->tag != 1)goto _LL258;else{_tmp436=(void*)_tmp435->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp437=(struct Cyc_Absyn_Global_b_struct*)_tmp436;if(
_tmp437->tag != 1)goto _LL258;else{_tmp438=_tmp437->f1;}};}}_LL257: {struct
_tuple15 _tmp8F1;return(_tmp8F1.f1=inflow,((_tmp8F1.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8F1)));}_LL258: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp439=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp422;if(_tmp439->tag != 23)goto _LL25A;else{_tmp43A=_tmp439->f1;_tmp43B=_tmp439->f2;}}
_LL259:{void*_tmp449=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp43A->topt))->v);struct Cyc_Absyn_PtrInfo _tmp44B;void*_tmp44C;
_LL263: {struct Cyc_Absyn_PointerType_struct*_tmp44A=(struct Cyc_Absyn_PointerType_struct*)
_tmp449;if(_tmp44A->tag != 5)goto _LL265;else{_tmp44B=_tmp44A->f1;_tmp44C=_tmp44B.elt_typ;}}
_LL264: if(!Cyc_Absyn_is_nontagged_union_type(_tmp44C)){struct Cyc_List_List*
_tmp8F2;flds=((_tmp8F2=_region_malloc(env->r,sizeof(*_tmp8F2)),((_tmp8F2->hd=(
void*)Cyc_CfFlowInfo_get_field_index(_tmp44C,_tmp43B),((_tmp8F2->tl=flds,_tmp8F2))))));}
goto _LL262;_LL265:;_LL266: {struct Cyc_Core_Impossible_struct _tmp8F8;const char*
_tmp8F7;struct Cyc_Core_Impossible_struct*_tmp8F6;(int)_throw((void*)((_tmp8F6=
_cycalloc(sizeof(*_tmp8F6)),((_tmp8F6[0]=((_tmp8F8.tag=Cyc_Core_Impossible,((
_tmp8F8.f1=((_tmp8F7="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp8F7,sizeof(
char),25))),_tmp8F8)))),_tmp8F6)))));}_LL262:;}_tmp43D=_tmp43A;goto _LL25B;_LL25A: {
struct Cyc_Absyn_Deref_e_struct*_tmp43C=(struct Cyc_Absyn_Deref_e_struct*)_tmp422;
if(_tmp43C->tag != 21)goto _LL25C;else{_tmp43D=_tmp43C->f1;}}_LL25B: if(
expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp43D->topt))->v)){union Cyc_CfFlowInfo_FlowInfo _tmp452;union Cyc_CfFlowInfo_AbsLVal
_tmp453;struct _tuple15 _tmp451=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp43D);
_tmp452=_tmp451.f1;_tmp453=_tmp451.f2;{struct _tuple15 _tmp8F9;struct _tuple15
_tmp455=(_tmp8F9.f1=_tmp452,((_tmp8F9.f2=_tmp453,_tmp8F9)));union Cyc_CfFlowInfo_FlowInfo
_tmp456;struct _tuple13 _tmp457;struct Cyc_Dict_Dict _tmp458;struct Cyc_List_List*
_tmp459;union Cyc_CfFlowInfo_AbsLVal _tmp45A;struct Cyc_CfFlowInfo_Place*_tmp45B;
_LL268: _tmp456=_tmp455.f1;if((_tmp456.ReachableFL).tag != 2)goto _LL26A;_tmp457=(
struct _tuple13)(_tmp456.ReachableFL).val;_tmp458=_tmp457.f1;_tmp459=_tmp457.f2;
_tmp45A=_tmp455.f2;if((_tmp45A.PlaceL).tag != 1)goto _LL26A;_tmp45B=(struct Cyc_CfFlowInfo_Place*)(
_tmp45A.PlaceL).val;_LL269: {void*_tmp45C=Cyc_CfFlowInfo_lookup_place(_tmp458,
_tmp45B);void*_tmp45D=_tmp45C;_LL26D: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp45E=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp45D;if(_tmp45E->tag != 5)goto
_LL26F;}_LL26E: return Cyc_NewControlFlow_anal_derefL(env,_tmp452,_tmp43D,_tmp452,
_tmp45C,flds);_LL26F:;_LL270: {enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(
_tmp41B,_tmp458,_tmp45C);void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp41B->unknown_all:
_tmp41B->unknown_none;void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp41B,Cyc_Tcutil_pointer_elt_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp43D->topt))->v),0,leaf);struct Cyc_CfFlowInfo_MallocPt_struct
_tmp8FC;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp8FB;void*new_root=(void*)((
_tmp8FB=_region_malloc(_tmp41B->r,sizeof(*_tmp8FB)),((_tmp8FB[0]=((_tmp8FC.tag=1,((
_tmp8FC.f1=e,((_tmp8FC.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp8FC)))))),_tmp8FB))));struct Cyc_CfFlowInfo_Place*_tmp8FD;struct Cyc_CfFlowInfo_Place*
place=(_tmp8FD=_region_malloc(_tmp41B->r,sizeof(*_tmp8FD)),((_tmp8FD->root=
new_root,((_tmp8FD->fields=0,_tmp8FD)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp900;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp8FF;void*res=(void*)((_tmp8FF=
_region_malloc(_tmp41B->r,sizeof(*_tmp8FF)),((_tmp8FF[0]=((_tmp900.tag=5,((
_tmp900.f1=place,_tmp900)))),_tmp8FF))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);_tmp458=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))
Cyc_Dict_insert)(_tmp458,new_root,new_rval);_tmp458=Cyc_CfFlowInfo_assign_place(
_tmp41B,e->loc,_tmp458,env->all_changed,_tmp45B,res);{union Cyc_CfFlowInfo_FlowInfo
_tmp45F=Cyc_CfFlowInfo_ReachableFL(_tmp458,_tmp459);return Cyc_NewControlFlow_anal_derefL(
env,_tmp45F,_tmp43D,_tmp45F,res,flds);};}_LL26C:;}_LL26A:;_LL26B: goto _LL267;
_LL267:;};}{union Cyc_CfFlowInfo_FlowInfo _tmp466;void*_tmp467;struct _tuple14
_tmp465=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp43D);_tmp466=_tmp465.f1;
_tmp467=_tmp465.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp43D,
_tmp466,_tmp467,flds);};_LL25C: {struct Cyc_Absyn_Subscript_e_struct*_tmp43E=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp422;if(_tmp43E->tag != 24)goto _LL25E;
else{_tmp43F=_tmp43E->f1;_tmp440=_tmp43E->f2;}}_LL25D: {void*_tmp468=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp43F->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp46B;struct Cyc_Absyn_PtrAtts _tmp46C;union Cyc_Absyn_Constraint*_tmp46D;_LL272: {
struct Cyc_Absyn_TupleType_struct*_tmp469=(struct Cyc_Absyn_TupleType_struct*)
_tmp468;if(_tmp469->tag != 10)goto _LL274;}_LL273: {unsigned int _tmp46E=(Cyc_Evexp_eval_const_uint_exp(
_tmp440)).f1;struct Cyc_List_List*_tmp901;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,_tmp43F,((_tmp901=_region_malloc(env->r,sizeof(*_tmp901)),((
_tmp901->hd=(void*)((int)_tmp46E),((_tmp901->tl=flds,_tmp901)))))));}_LL274: {
struct Cyc_Absyn_PointerType_struct*_tmp46A=(struct Cyc_Absyn_PointerType_struct*)
_tmp468;if(_tmp46A->tag != 5)goto _LL276;else{_tmp46B=_tmp46A->f1;_tmp46C=_tmp46B.ptr_atts;
_tmp46D=_tmp46C.bounds;}}_LL275: {struct _RegionHandle _tmp470=_new_region("temp");
struct _RegionHandle*temp=& _tmp470;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo
_tmp473;struct Cyc_List_List*_tmp474;struct Cyc_Absyn_Exp*_tmp902[2];struct
_tuple20 _tmp472=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((
_tmp902[1]=_tmp440,((_tmp902[0]=_tmp43F,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp902,
sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp473=_tmp472.f1;_tmp474=_tmp472.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp475=_tmp473;{union Cyc_CfFlowInfo_FlowInfo _tmp476=
_tmp473;struct _tuple13 _tmp477;struct Cyc_Dict_Dict _tmp478;struct Cyc_List_List*
_tmp479;_LL279: if((_tmp476.ReachableFL).tag != 2)goto _LL27B;_tmp477=(struct
_tuple13)(_tmp476.ReachableFL).val;_tmp478=_tmp477.f1;_tmp479=_tmp477.f2;_LL27A:
if(Cyc_CfFlowInfo_initlevel(_tmp41B,_tmp478,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp474))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp905;void*_tmp904;(_tmp904=0,Cyc_Tcutil_terr(_tmp440->loc,((_tmp905="expression may not be initialized",
_tag_dyneither(_tmp905,sizeof(char),34))),_tag_dyneither(_tmp904,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp47C=Cyc_NewControlFlow_add_subscript_reln(_tmp41B->r,
_tmp479,_tmp43F,_tmp440);if(_tmp479 != _tmp47C)_tmp475=Cyc_CfFlowInfo_ReachableFL(
_tmp478,_tmp47C);goto _LL278;};_LL27B:;_LL27C: goto _LL278;_LL278:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp47E;union Cyc_CfFlowInfo_AbsLVal _tmp47F;struct _tuple15 _tmp47D=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp43F,_tmp473,(void*)((struct Cyc_List_List*)_check_null(_tmp474))->hd,
flds);_tmp47E=_tmp47D.f1;_tmp47F=_tmp47D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp480=
_tmp47E;int _tmp481;_LL27E: if((_tmp480.BottomFL).tag != 1)goto _LL280;_tmp481=(int)(
_tmp480.BottomFL).val;_LL27F: {struct _tuple15 _tmp906;struct _tuple15 _tmp483=(
_tmp906.f1=_tmp47E,((_tmp906.f2=_tmp47F,_tmp906)));_npop_handler(0);return
_tmp483;}_LL280:;_LL281: {struct _tuple15 _tmp907;struct _tuple15 _tmp485=(_tmp907.f1=
_tmp475,((_tmp907.f2=_tmp47F,_tmp907)));_npop_handler(0);return _tmp485;}_LL27D:;};};};};
_pop_region(temp);}_LL276:;_LL277: {struct Cyc_Core_Impossible_struct _tmp90D;
const char*_tmp90C;struct Cyc_Core_Impossible_struct*_tmp90B;(int)_throw((void*)((
_tmp90B=_cycalloc(sizeof(*_tmp90B)),((_tmp90B[0]=((_tmp90D.tag=Cyc_Core_Impossible,((
_tmp90D.f1=((_tmp90C="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp90C,
sizeof(char),33))),_tmp90D)))),_tmp90B)))));}_LL271:;}_LL25E: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp441=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp422;if(_tmp441->tag != 22)goto
_LL260;else{_tmp442=_tmp441->f1;_tmp443=_tmp441->f2;}}_LL25F: if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp442->topt))->v)){struct _tuple15
_tmp90E;return(_tmp90E.f1=inflow,((_tmp90E.f2=Cyc_CfFlowInfo_UnknownL(),_tmp90E)));}{
struct Cyc_List_List*_tmp90F;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,
expand_unique,_tmp442,((_tmp90F=_region_malloc(env->r,sizeof(*_tmp90F)),((
_tmp90F->hd=(void*)Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp442->topt))->v,_tmp443),((_tmp90F->tl=flds,_tmp90F)))))));};
_LL260:;_LL261: {struct _tuple15 _tmp910;return(_tmp910.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp910.f2=Cyc_CfFlowInfo_UnknownL(),_tmp910)));}_LL249:;};}static struct _tuple15
Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,int expand_unique,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo
_tmp48D;union Cyc_CfFlowInfo_AbsLVal _tmp48E;struct _tuple15 _tmp48C=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,e,0);_tmp48D=_tmp48C.f1;_tmp48E=_tmp48C.f2;{struct
_tuple15 _tmp911;return(_tmp911.f1=_tmp48D,((_tmp911.f2=_tmp48E,_tmp911)));};}
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp490=env->fenv;{void*_tmp491=e->r;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*
_tmp494;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*
_tmp498;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*
_tmp49D;struct Cyc_Absyn_Exp*_tmp49E;enum Cyc_Absyn_Primop _tmp4A0;struct Cyc_List_List*
_tmp4A1;struct Cyc_List_List _tmp4A2;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_List_List*
_tmp4A4;enum Cyc_Absyn_Primop _tmp4A6;struct Cyc_List_List*_tmp4A7;_LL283: {struct
Cyc_Absyn_Conditional_e_struct*_tmp492=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp491;if(_tmp492->tag != 6)goto _LL285;else{_tmp493=_tmp492->f1;_tmp494=_tmp492->f2;
_tmp495=_tmp492->f3;}}_LL284: {union Cyc_CfFlowInfo_FlowInfo _tmp4A9;union Cyc_CfFlowInfo_FlowInfo
_tmp4AA;struct _tuple16 _tmp4A8=Cyc_NewControlFlow_anal_test(env,inflow,_tmp493);
_tmp4A9=_tmp4A8.f1;_tmp4AA=_tmp4A8.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp4AC;union
Cyc_CfFlowInfo_FlowInfo _tmp4AD;struct _tuple16 _tmp4AB=Cyc_NewControlFlow_anal_test(
env,_tmp4A9,_tmp494);_tmp4AC=_tmp4AB.f1;_tmp4AD=_tmp4AB.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4AF;union Cyc_CfFlowInfo_FlowInfo _tmp4B0;struct _tuple16 _tmp4AE=Cyc_NewControlFlow_anal_test(
env,_tmp4AA,_tmp495);_tmp4AF=_tmp4AE.f1;_tmp4B0=_tmp4AE.f2;{struct _tuple16
_tmp912;return(_tmp912.f1=Cyc_CfFlowInfo_join_flow(_tmp490,env->all_changed,
_tmp4AC,_tmp4AF),((_tmp912.f2=Cyc_CfFlowInfo_join_flow(_tmp490,env->all_changed,
_tmp4AD,_tmp4B0),_tmp912)));};};};}_LL285: {struct Cyc_Absyn_And_e_struct*_tmp496=(
struct Cyc_Absyn_And_e_struct*)_tmp491;if(_tmp496->tag != 7)goto _LL287;else{
_tmp497=_tmp496->f1;_tmp498=_tmp496->f2;}}_LL286: {union Cyc_CfFlowInfo_FlowInfo
_tmp4B3;union Cyc_CfFlowInfo_FlowInfo _tmp4B4;struct _tuple16 _tmp4B2=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp497);_tmp4B3=_tmp4B2.f1;_tmp4B4=_tmp4B2.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4B6;union Cyc_CfFlowInfo_FlowInfo _tmp4B7;struct _tuple16 _tmp4B5=Cyc_NewControlFlow_anal_test(
env,_tmp4B3,_tmp498);_tmp4B6=_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;{struct _tuple16
_tmp913;return(_tmp913.f1=_tmp4B6,((_tmp913.f2=Cyc_CfFlowInfo_join_flow(_tmp490,
env->all_changed,_tmp4B4,_tmp4B7),_tmp913)));};};}_LL287: {struct Cyc_Absyn_Or_e_struct*
_tmp499=(struct Cyc_Absyn_Or_e_struct*)_tmp491;if(_tmp499->tag != 8)goto _LL289;
else{_tmp49A=_tmp499->f1;_tmp49B=_tmp499->f2;}}_LL288: {union Cyc_CfFlowInfo_FlowInfo
_tmp4BA;union Cyc_CfFlowInfo_FlowInfo _tmp4BB;struct _tuple16 _tmp4B9=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp49A);_tmp4BA=_tmp4B9.f1;_tmp4BB=_tmp4B9.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4BD;union Cyc_CfFlowInfo_FlowInfo _tmp4BE;struct _tuple16 _tmp4BC=Cyc_NewControlFlow_anal_test(
env,_tmp4BB,_tmp49B);_tmp4BD=_tmp4BC.f1;_tmp4BE=_tmp4BC.f2;{struct _tuple16
_tmp914;return(_tmp914.f1=Cyc_CfFlowInfo_join_flow(_tmp490,env->all_changed,
_tmp4BA,_tmp4BD),((_tmp914.f2=_tmp4BE,_tmp914)));};};}_LL289: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp49C=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp491;if(_tmp49C->tag != 9)goto _LL28B;
else{_tmp49D=_tmp49C->f1;_tmp49E=_tmp49C->f2;}}_LL28A: {union Cyc_CfFlowInfo_FlowInfo
_tmp4C1;void*_tmp4C2;struct _tuple14 _tmp4C0=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp49D);_tmp4C1=_tmp4C0.f1;_tmp4C2=_tmp4C0.f2;return Cyc_NewControlFlow_anal_test(
env,_tmp4C1,_tmp49E);}_LL28B: {struct Cyc_Absyn_Primop_e_struct*_tmp49F=(struct
Cyc_Absyn_Primop_e_struct*)_tmp491;if(_tmp49F->tag != 3)goto _LL28D;else{_tmp4A0=
_tmp49F->f1;if(_tmp4A0 != Cyc_Absyn_Not)goto _LL28D;_tmp4A1=_tmp49F->f2;if(_tmp4A1
== 0)goto _LL28D;_tmp4A2=*_tmp4A1;_tmp4A3=(struct Cyc_Absyn_Exp*)_tmp4A2.hd;
_tmp4A4=_tmp4A2.tl;if(_tmp4A4 != 0)goto _LL28D;}}_LL28C: {union Cyc_CfFlowInfo_FlowInfo
_tmp4C4;union Cyc_CfFlowInfo_FlowInfo _tmp4C5;struct _tuple16 _tmp4C3=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp4A3);_tmp4C4=_tmp4C3.f1;_tmp4C5=_tmp4C3.f2;{struct _tuple16 _tmp915;
return(_tmp915.f1=_tmp4C5,((_tmp915.f2=_tmp4C4,_tmp915)));};}_LL28D: {struct Cyc_Absyn_Primop_e_struct*
_tmp4A5=(struct Cyc_Absyn_Primop_e_struct*)_tmp491;if(_tmp4A5->tag != 3)goto _LL28F;
else{_tmp4A6=_tmp4A5->f1;_tmp4A7=_tmp4A5->f2;}}_LL28E: {void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo
f;struct _RegionHandle _tmp4C7=_new_region("temp");struct _RegionHandle*temp=&
_tmp4C7;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo _tmp4C9;struct Cyc_List_List*
_tmp4CA;struct _tuple20 _tmp4C8=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,_tmp4A7,0,0);_tmp4C9=_tmp4C8.f1;_tmp4CA=_tmp4C8.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp4CA))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp4CA->tl))->hd;
f=_tmp4C9;}{union Cyc_CfFlowInfo_FlowInfo _tmp4CB=f;int _tmp4CC;struct _tuple13
_tmp4CD;struct Cyc_Dict_Dict _tmp4CE;struct Cyc_List_List*_tmp4CF;_LL292: if((
_tmp4CB.BottomFL).tag != 1)goto _LL294;_tmp4CC=(int)(_tmp4CB.BottomFL).val;_LL293: {
struct _tuple16 _tmp916;struct _tuple16 _tmp4D1=(_tmp916.f1=f,((_tmp916.f2=f,_tmp916)));
_npop_handler(0);return _tmp4D1;}_LL294: if((_tmp4CB.ReachableFL).tag != 2)goto
_LL291;_tmp4CD=(struct _tuple13)(_tmp4CB.ReachableFL).val;_tmp4CE=_tmp4CD.f1;
_tmp4CF=_tmp4CD.f2;_LL295: {struct Cyc_Absyn_Exp*_tmp4D2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A7))->hd;struct Cyc_Absyn_Exp*_tmp4D3=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4A7->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp4CE,r1)== Cyc_CfFlowInfo_NoneIL){const char*_tmp919;void*_tmp918;(
_tmp918=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp4A7->hd)->loc,((_tmp919="expression may not be initialized",
_tag_dyneither(_tmp919,sizeof(char),34))),_tag_dyneither(_tmp918,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4CE,r2)== Cyc_CfFlowInfo_NoneIL){const
char*_tmp91C;void*_tmp91B;(_tmp91B=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A7->tl))->hd)->loc,((_tmp91C="expression may not be initialized",
_tag_dyneither(_tmp91C,sizeof(char),34))),_tag_dyneither(_tmp91B,sizeof(void*),0)));}
if(_tmp4A6 == Cyc_Absyn_Eq  || _tmp4A6 == Cyc_Absyn_Neq){struct _tuple0 _tmp91D;
struct _tuple0 _tmp4D9=(_tmp91D.f1=r1,((_tmp91D.f2=r2,_tmp91D)));void*_tmp4DA;
enum Cyc_CfFlowInfo_InitLevel _tmp4DC;void*_tmp4DD;void*_tmp4DF;void*_tmp4E1;
enum Cyc_CfFlowInfo_InitLevel _tmp4E3;void*_tmp4E4;void*_tmp4E6;void*_tmp4E8;void*
_tmp4EA;void*_tmp4EC;void*_tmp4EE;void*_tmp4F0;void*_tmp4F2;void*_tmp4F4;void*
_tmp4F6;void*_tmp4F8;void*_tmp4FA;void*_tmp4FC;void*_tmp4FE;_LL297: _tmp4DA=
_tmp4D9.f1;{struct Cyc_CfFlowInfo_UnknownR_struct*_tmp4DB=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4DA;if(_tmp4DB->tag != 3)goto _LL299;else{_tmp4DC=_tmp4DB->f1;}};_tmp4DD=
_tmp4D9.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4DE=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4DD;if(_tmp4DE->tag != 0)goto _LL299;};_LL298: {union Cyc_CfFlowInfo_FlowInfo
_tmp501;union Cyc_CfFlowInfo_FlowInfo _tmp502;struct _tuple16 _tmp500=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4D2,_tmp4DC);_tmp501=_tmp500.f1;_tmp502=_tmp500.f2;switch(_tmp4A6){
case Cyc_Absyn_Eq: _LL2AB: {struct _tuple16 _tmp91E;struct _tuple16 _tmp504=(_tmp91E.f1=
_tmp502,((_tmp91E.f2=_tmp501,_tmp91E)));_npop_handler(0);return _tmp504;}case Cyc_Absyn_Neq:
_LL2AC: {struct _tuple16 _tmp91F;struct _tuple16 _tmp506=(_tmp91F.f1=_tmp501,((
_tmp91F.f2=_tmp502,_tmp91F)));_npop_handler(0);return _tmp506;}default: _LL2AD: {
struct Cyc_Core_Impossible_struct _tmp925;const char*_tmp924;struct Cyc_Core_Impossible_struct*
_tmp923;(int)_throw((void*)((_tmp923=_cycalloc(sizeof(*_tmp923)),((_tmp923[0]=((
_tmp925.tag=Cyc_Core_Impossible,((_tmp925.f1=((_tmp924="anal_test, zero-split",
_tag_dyneither(_tmp924,sizeof(char),22))),_tmp925)))),_tmp923)))));}}}_LL299:
_tmp4DF=_tmp4D9.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4E0=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4DF;if(_tmp4E0->tag != 0)goto _LL29B;};_tmp4E1=_tmp4D9.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4E2=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4E1;if(_tmp4E2->tag != 3)goto
_LL29B;else{_tmp4E3=_tmp4E2->f1;}};_LL29A: {union Cyc_CfFlowInfo_FlowInfo _tmp50B;
union Cyc_CfFlowInfo_FlowInfo _tmp50C;struct _tuple16 _tmp50A=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4D3,_tmp4E3);_tmp50B=_tmp50A.f1;_tmp50C=_tmp50A.f2;switch(_tmp4A6){
case Cyc_Absyn_Eq: _LL2AF: {struct _tuple16 _tmp926;struct _tuple16 _tmp50E=(_tmp926.f1=
_tmp50C,((_tmp926.f2=_tmp50B,_tmp926)));_npop_handler(0);return _tmp50E;}case Cyc_Absyn_Neq:
_LL2B0: {struct _tuple16 _tmp927;struct _tuple16 _tmp510=(_tmp927.f1=_tmp50B,((
_tmp927.f2=_tmp50C,_tmp927)));_npop_handler(0);return _tmp510;}default: _LL2B1: {
struct Cyc_Core_Impossible_struct _tmp92D;const char*_tmp92C;struct Cyc_Core_Impossible_struct*
_tmp92B;(int)_throw((void*)((_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B[0]=((
_tmp92D.tag=Cyc_Core_Impossible,((_tmp92D.f1=((_tmp92C="anal_test, zero-split",
_tag_dyneither(_tmp92C,sizeof(char),22))),_tmp92D)))),_tmp92B)))));}}}_LL29B:
_tmp4E4=_tmp4D9.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4E5=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4E4;if(_tmp4E5->tag != 0)goto _LL29D;};_tmp4E6=_tmp4D9.f2;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4E7=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4E6;if(_tmp4E7->tag != 0)goto
_LL29D;};_LL29C: if(_tmp4A6 == Cyc_Absyn_Eq){struct _tuple16 _tmp92E;struct _tuple16
_tmp515=(_tmp92E.f1=f,((_tmp92E.f2=Cyc_CfFlowInfo_BottomFL(),_tmp92E)));
_npop_handler(0);return _tmp515;}else{struct _tuple16 _tmp92F;struct _tuple16 _tmp517=(
_tmp92F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp92F.f2=f,_tmp92F)));_npop_handler(0);
return _tmp517;}_LL29D: _tmp4E8=_tmp4D9.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4E9=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4E8;if(_tmp4E9->tag != 0)goto
_LL29F;};_tmp4EA=_tmp4D9.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp4EB=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4EA;if(_tmp4EB->tag != 1)goto _LL29F;};
_LL29E: goto _LL2A0;_LL29F: _tmp4EC=_tmp4D9.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4ED=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4EC;if(_tmp4ED->tag != 0)goto
_LL2A1;};_tmp4EE=_tmp4D9.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp4EF=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4EE;if(_tmp4EF->tag != 2)goto _LL2A1;};
_LL2A0: goto _LL2A2;_LL2A1: _tmp4F0=_tmp4D9.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4F1=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4F0;if(_tmp4F1->tag != 0)goto
_LL2A3;};_tmp4F2=_tmp4D9.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp4F3=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4F2;if(_tmp4F3->tag != 5)goto _LL2A3;};
_LL2A2: goto _LL2A4;_LL2A3: _tmp4F4=_tmp4D9.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4F5=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4F4;if(_tmp4F5->tag != 1)
goto _LL2A5;};_tmp4F6=_tmp4D9.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4F7=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4F6;if(_tmp4F7->tag != 0)goto _LL2A5;};
_LL2A4: goto _LL2A6;_LL2A5: _tmp4F8=_tmp4D9.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4F9=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4F8;if(_tmp4F9->tag != 2)
goto _LL2A7;};_tmp4FA=_tmp4D9.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4FB=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4FA;if(_tmp4FB->tag != 0)goto _LL2A7;};
_LL2A6: goto _LL2A8;_LL2A7: _tmp4FC=_tmp4D9.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp4FD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4FC;if(_tmp4FD->tag != 5)goto
_LL2A9;};_tmp4FE=_tmp4D9.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4FF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4FE;if(_tmp4FF->tag != 0)goto _LL2A9;};_LL2A8: if(_tmp4A6 == Cyc_Absyn_Neq){
struct _tuple16 _tmp930;struct _tuple16 _tmp519=(_tmp930.f1=f,((_tmp930.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp930)));_npop_handler(0);return _tmp519;}else{struct _tuple16 _tmp931;struct
_tuple16 _tmp51B=(_tmp931.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp931.f2=f,_tmp931)));
_npop_handler(0);return _tmp51B;}_LL2A9:;_LL2AA: goto _LL296;_LL296:;}{struct
_tuple0 _tmp932;struct _tuple0 _tmp51D=(_tmp932.f1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4D2->topt))->v),((_tmp932.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D3->topt))->v),_tmp932)));void*
_tmp51E;enum Cyc_Absyn_Sign _tmp520;void*_tmp521;enum Cyc_Absyn_Sign _tmp523;void*
_tmp524;void*_tmp526;_LL2B4: _tmp51E=_tmp51D.f1;{struct Cyc_Absyn_IntType_struct*
_tmp51F=(struct Cyc_Absyn_IntType_struct*)_tmp51E;if(_tmp51F->tag != 6)goto _LL2B6;
else{_tmp520=_tmp51F->f1;if(_tmp520 != Cyc_Absyn_Unsigned)goto _LL2B6;}};_LL2B5:
goto _LL2B7;_LL2B6: _tmp521=_tmp51D.f2;{struct Cyc_Absyn_IntType_struct*_tmp522=(
struct Cyc_Absyn_IntType_struct*)_tmp521;if(_tmp522->tag != 6)goto _LL2B8;else{
_tmp523=_tmp522->f1;if(_tmp523 != Cyc_Absyn_Unsigned)goto _LL2B8;}};_LL2B7: goto
_LL2B9;_LL2B8: _tmp524=_tmp51D.f1;{struct Cyc_Absyn_TagType_struct*_tmp525=(struct
Cyc_Absyn_TagType_struct*)_tmp524;if(_tmp525->tag != 19)goto _LL2BA;};_LL2B9: goto
_LL2BB;_LL2BA: _tmp526=_tmp51D.f2;{struct Cyc_Absyn_TagType_struct*_tmp527=(struct
Cyc_Absyn_TagType_struct*)_tmp526;if(_tmp527->tag != 19)goto _LL2BC;};_LL2BB: goto
_LL2B3;_LL2BC:;_LL2BD: {struct _tuple16 _tmp933;struct _tuple16 _tmp529=(_tmp933.f1=
f,((_tmp933.f2=f,_tmp933)));_npop_handler(0);return _tmp529;}_LL2B3:;}switch(
_tmp4A6){case Cyc_Absyn_Eq: _LL2BE: goto _LL2BF;case Cyc_Absyn_Neq: _LL2BF: goto _LL2C0;
case Cyc_Absyn_Gt: _LL2C0: goto _LL2C1;case Cyc_Absyn_Gte: _LL2C1: {struct _tuple16
_tmp934;struct _tuple16 _tmp52B=(_tmp934.f1=f,((_tmp934.f2=f,_tmp934)));
_npop_handler(0);return _tmp52B;}case Cyc_Absyn_Lt: _LL2C2: {union Cyc_CfFlowInfo_FlowInfo
_tmp52C=f;union Cyc_CfFlowInfo_FlowInfo _tmp52D=f;{union Cyc_CfFlowInfo_FlowInfo
_tmp52E=_tmp52C;int _tmp52F;struct _tuple13 _tmp530;struct Cyc_Dict_Dict _tmp531;
_LL2C5: if((_tmp52E.BottomFL).tag != 1)goto _LL2C7;_tmp52F=(int)(_tmp52E.BottomFL).val;
_LL2C6: {struct Cyc_Core_Impossible_struct _tmp93A;const char*_tmp939;struct Cyc_Core_Impossible_struct*
_tmp938;(int)_throw((void*)((_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938[0]=((
_tmp93A.tag=Cyc_Core_Impossible,((_tmp93A.f1=((_tmp939="anal_test, Lt",
_tag_dyneither(_tmp939,sizeof(char),14))),_tmp93A)))),_tmp938)))));}_LL2C7: if((
_tmp52E.ReachableFL).tag != 2)goto _LL2C4;_tmp530=(struct _tuple13)(_tmp52E.ReachableFL).val;
_tmp531=_tmp530.f1;_LL2C8: _tmp4CE=_tmp531;_LL2C4:;}{void*_tmp535=_tmp4D2->r;void*
_tmp537;struct Cyc_Absyn_Vardecl*_tmp539;void*_tmp53B;struct Cyc_Absyn_Vardecl*
_tmp53D;void*_tmp53F;struct Cyc_Absyn_Vardecl*_tmp541;void*_tmp543;struct Cyc_Absyn_Vardecl*
_tmp545;_LL2CA:{struct Cyc_Absyn_Var_e_struct*_tmp536=(struct Cyc_Absyn_Var_e_struct*)
_tmp535;if(_tmp536->tag != 1)goto _LL2CC;else{_tmp537=(void*)_tmp536->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp538=(struct Cyc_Absyn_Global_b_struct*)_tmp537;if(
_tmp538->tag != 1)goto _LL2CC;else{_tmp539=_tmp538->f1;}};}}if(!(!_tmp539->escapes))
goto _LL2CC;_LL2CB: _tmp53D=_tmp539;goto _LL2CD;_LL2CC:{struct Cyc_Absyn_Var_e_struct*
_tmp53A=(struct Cyc_Absyn_Var_e_struct*)_tmp535;if(_tmp53A->tag != 1)goto _LL2CE;
else{_tmp53B=(void*)_tmp53A->f2;{struct Cyc_Absyn_Local_b_struct*_tmp53C=(struct
Cyc_Absyn_Local_b_struct*)_tmp53B;if(_tmp53C->tag != 4)goto _LL2CE;else{_tmp53D=
_tmp53C->f1;}};}}if(!(!_tmp53D->escapes))goto _LL2CE;_LL2CD: _tmp541=_tmp53D;goto
_LL2CF;_LL2CE:{struct Cyc_Absyn_Var_e_struct*_tmp53E=(struct Cyc_Absyn_Var_e_struct*)
_tmp535;if(_tmp53E->tag != 1)goto _LL2D0;else{_tmp53F=(void*)_tmp53E->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp540=(struct Cyc_Absyn_Pat_b_struct*)_tmp53F;if(_tmp540->tag
!= 5)goto _LL2D0;else{_tmp541=_tmp540->f1;}};}}if(!(!_tmp541->escapes))goto _LL2D0;
_LL2CF: _tmp545=_tmp541;goto _LL2D1;_LL2D0:{struct Cyc_Absyn_Var_e_struct*_tmp542=(
struct Cyc_Absyn_Var_e_struct*)_tmp535;if(_tmp542->tag != 1)goto _LL2D2;else{
_tmp543=(void*)_tmp542->f2;{struct Cyc_Absyn_Param_b_struct*_tmp544=(struct Cyc_Absyn_Param_b_struct*)
_tmp543;if(_tmp544->tag != 3)goto _LL2D2;else{_tmp545=_tmp544->f1;}};}}if(!(!
_tmp545->escapes))goto _LL2D2;_LL2D1: {void*_tmp546=_tmp4D3->r;void*_tmp548;
struct Cyc_Absyn_Vardecl*_tmp54A;void*_tmp54C;struct Cyc_Absyn_Vardecl*_tmp54E;
void*_tmp550;struct Cyc_Absyn_Vardecl*_tmp552;void*_tmp554;struct Cyc_Absyn_Vardecl*
_tmp556;union Cyc_Absyn_Cnst _tmp558;struct _tuple6 _tmp559;int _tmp55A;struct Cyc_Absyn_Exp*
_tmp55C;struct Cyc_Absyn_Exp _tmp55D;void*_tmp55E;union Cyc_Absyn_Cnst _tmp560;
struct _tuple6 _tmp561;int _tmp562;enum Cyc_Absyn_Primop _tmp564;struct Cyc_List_List*
_tmp565;struct Cyc_List_List _tmp566;struct Cyc_Absyn_Exp*_tmp567;_LL2D5:{struct Cyc_Absyn_Var_e_struct*
_tmp547=(struct Cyc_Absyn_Var_e_struct*)_tmp546;if(_tmp547->tag != 1)goto _LL2D7;
else{_tmp548=(void*)_tmp547->f2;{struct Cyc_Absyn_Global_b_struct*_tmp549=(struct
Cyc_Absyn_Global_b_struct*)_tmp548;if(_tmp549->tag != 1)goto _LL2D7;else{_tmp54A=
_tmp549->f1;}};}}if(!(!_tmp54A->escapes))goto _LL2D7;_LL2D6: _tmp54E=_tmp54A;goto
_LL2D8;_LL2D7:{struct Cyc_Absyn_Var_e_struct*_tmp54B=(struct Cyc_Absyn_Var_e_struct*)
_tmp546;if(_tmp54B->tag != 1)goto _LL2D9;else{_tmp54C=(void*)_tmp54B->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp54D=(struct Cyc_Absyn_Local_b_struct*)_tmp54C;if(
_tmp54D->tag != 4)goto _LL2D9;else{_tmp54E=_tmp54D->f1;}};}}if(!(!_tmp54E->escapes))
goto _LL2D9;_LL2D8: _tmp552=_tmp54E;goto _LL2DA;_LL2D9:{struct Cyc_Absyn_Var_e_struct*
_tmp54F=(struct Cyc_Absyn_Var_e_struct*)_tmp546;if(_tmp54F->tag != 1)goto _LL2DB;
else{_tmp550=(void*)_tmp54F->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp551=(struct Cyc_Absyn_Pat_b_struct*)
_tmp550;if(_tmp551->tag != 5)goto _LL2DB;else{_tmp552=_tmp551->f1;}};}}if(!(!
_tmp552->escapes))goto _LL2DB;_LL2DA: _tmp556=_tmp552;goto _LL2DC;_LL2DB:{struct Cyc_Absyn_Var_e_struct*
_tmp553=(struct Cyc_Absyn_Var_e_struct*)_tmp546;if(_tmp553->tag != 1)goto _LL2DD;
else{_tmp554=(void*)_tmp553->f2;{struct Cyc_Absyn_Param_b_struct*_tmp555=(struct
Cyc_Absyn_Param_b_struct*)_tmp554;if(_tmp555->tag != 3)goto _LL2DD;else{_tmp556=
_tmp555->f1;}};}}if(!(!_tmp556->escapes))goto _LL2DD;_LL2DC: {struct _RegionHandle*
_tmp568=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp93D;struct Cyc_List_List*
_tmp93C;struct Cyc_List_List*_tmp569=(_tmp93C=_region_malloc(_tmp568,sizeof(*
_tmp93C)),((_tmp93C->hd=((_tmp93D=_region_malloc(_tmp568,sizeof(*_tmp93D)),((
_tmp93D->vd=_tmp545,((_tmp93D->rop=Cyc_CfFlowInfo_LessVar(_tmp556,_tmp556->type),
_tmp93D)))))),((_tmp93C->tl=_tmp4CF,_tmp93C)))));struct _tuple16 _tmp93E;struct
_tuple16 _tmp56B=(_tmp93E.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4CE,_tmp569),((
_tmp93E.f2=_tmp52D,_tmp93E)));_npop_handler(0);return _tmp56B;}_LL2DD: {struct Cyc_Absyn_Const_e_struct*
_tmp557=(struct Cyc_Absyn_Const_e_struct*)_tmp546;if(_tmp557->tag != 0)goto _LL2DF;
else{_tmp558=_tmp557->f1;if((_tmp558.Int_c).tag != 5)goto _LL2DF;_tmp559=(struct
_tuple6)(_tmp558.Int_c).val;_tmp55A=_tmp559.f2;}}_LL2DE: _tmp562=_tmp55A;goto
_LL2E0;_LL2DF: {struct Cyc_Absyn_Cast_e_struct*_tmp55B=(struct Cyc_Absyn_Cast_e_struct*)
_tmp546;if(_tmp55B->tag != 15)goto _LL2E1;else{_tmp55C=_tmp55B->f2;_tmp55D=*
_tmp55C;_tmp55E=_tmp55D.r;{struct Cyc_Absyn_Const_e_struct*_tmp55F=(struct Cyc_Absyn_Const_e_struct*)
_tmp55E;if(_tmp55F->tag != 0)goto _LL2E1;else{_tmp560=_tmp55F->f1;if((_tmp560.Int_c).tag
!= 5)goto _LL2E1;_tmp561=(struct _tuple6)(_tmp560.Int_c).val;_tmp562=_tmp561.f2;}};}}
_LL2E0: {struct _RegionHandle*_tmp56E=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*
_tmp941;struct Cyc_List_List*_tmp940;struct Cyc_List_List*_tmp56F=(_tmp940=
_region_malloc(_tmp56E,sizeof(*_tmp940)),((_tmp940->hd=((_tmp941=_region_malloc(
_tmp56E,sizeof(*_tmp941)),((_tmp941->vd=_tmp545,((_tmp941->rop=Cyc_CfFlowInfo_LessConst((
unsigned int)_tmp562),_tmp941)))))),((_tmp940->tl=_tmp4CF,_tmp940)))));struct
_tuple16 _tmp942;struct _tuple16 _tmp571=(_tmp942.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4CE,_tmp56F),((_tmp942.f2=_tmp52D,_tmp942)));_npop_handler(0);return _tmp571;}
_LL2E1: {struct Cyc_Absyn_Primop_e_struct*_tmp563=(struct Cyc_Absyn_Primop_e_struct*)
_tmp546;if(_tmp563->tag != 3)goto _LL2E3;else{_tmp564=_tmp563->f1;_tmp565=_tmp563->f2;
if(_tmp565 == 0)goto _LL2E3;_tmp566=*_tmp565;_tmp567=(struct Cyc_Absyn_Exp*)_tmp566.hd;}}
_LL2E2: {void*_tmp574=_tmp567->r;void*_tmp576;struct Cyc_Absyn_Vardecl*_tmp578;
void*_tmp57A;struct Cyc_Absyn_Vardecl*_tmp57C;void*_tmp57E;struct Cyc_Absyn_Vardecl*
_tmp580;void*_tmp582;struct Cyc_Absyn_Vardecl*_tmp584;_LL2E6:{struct Cyc_Absyn_Var_e_struct*
_tmp575=(struct Cyc_Absyn_Var_e_struct*)_tmp574;if(_tmp575->tag != 1)goto _LL2E8;
else{_tmp576=(void*)_tmp575->f2;{struct Cyc_Absyn_Global_b_struct*_tmp577=(struct
Cyc_Absyn_Global_b_struct*)_tmp576;if(_tmp577->tag != 1)goto _LL2E8;else{_tmp578=
_tmp577->f1;}};}}if(!(!_tmp578->escapes))goto _LL2E8;_LL2E7: _tmp57C=_tmp578;goto
_LL2E9;_LL2E8:{struct Cyc_Absyn_Var_e_struct*_tmp579=(struct Cyc_Absyn_Var_e_struct*)
_tmp574;if(_tmp579->tag != 1)goto _LL2EA;else{_tmp57A=(void*)_tmp579->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp57B=(struct Cyc_Absyn_Local_b_struct*)_tmp57A;if(
_tmp57B->tag != 4)goto _LL2EA;else{_tmp57C=_tmp57B->f1;}};}}if(!(!_tmp57C->escapes))
goto _LL2EA;_LL2E9: _tmp580=_tmp57C;goto _LL2EB;_LL2EA:{struct Cyc_Absyn_Var_e_struct*
_tmp57D=(struct Cyc_Absyn_Var_e_struct*)_tmp574;if(_tmp57D->tag != 1)goto _LL2EC;
else{_tmp57E=(void*)_tmp57D->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp57F=(struct Cyc_Absyn_Pat_b_struct*)
_tmp57E;if(_tmp57F->tag != 5)goto _LL2EC;else{_tmp580=_tmp57F->f1;}};}}if(!(!
_tmp580->escapes))goto _LL2EC;_LL2EB: _tmp584=_tmp580;goto _LL2ED;_LL2EC:{struct Cyc_Absyn_Var_e_struct*
_tmp581=(struct Cyc_Absyn_Var_e_struct*)_tmp574;if(_tmp581->tag != 1)goto _LL2EE;
else{_tmp582=(void*)_tmp581->f2;{struct Cyc_Absyn_Param_b_struct*_tmp583=(struct
Cyc_Absyn_Param_b_struct*)_tmp582;if(_tmp583->tag != 3)goto _LL2EE;else{_tmp584=
_tmp583->f1;}};}}if(!(!_tmp584->escapes))goto _LL2EE;_LL2ED: {struct _RegionHandle*
_tmp585=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp945;struct Cyc_List_List*
_tmp944;struct Cyc_List_List*_tmp586=(_tmp944=_region_malloc(_tmp585,sizeof(*
_tmp944)),((_tmp944->hd=((_tmp945=_region_malloc(_tmp585,sizeof(*_tmp945)),((
_tmp945->vd=_tmp545,((_tmp945->rop=Cyc_CfFlowInfo_LessNumelts(_tmp584),_tmp945)))))),((
_tmp944->tl=_tmp4CF,_tmp944)))));struct _tuple16 _tmp946;struct _tuple16 _tmp588=(
_tmp946.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4CE,_tmp586),((_tmp946.f2=_tmp52D,
_tmp946)));_npop_handler(0);return _tmp588;}_LL2EE:;_LL2EF: {struct _tuple16
_tmp947;struct _tuple16 _tmp58C=(_tmp947.f1=_tmp52C,((_tmp947.f2=_tmp52D,_tmp947)));
_npop_handler(0);return _tmp58C;}_LL2E5:;}_LL2E3:;_LL2E4: {struct _tuple16 _tmp948;
struct _tuple16 _tmp58E=(_tmp948.f1=_tmp52C,((_tmp948.f2=_tmp52D,_tmp948)));
_npop_handler(0);return _tmp58E;}_LL2D4:;}_LL2D2:;_LL2D3: {struct _tuple16 _tmp949;
struct _tuple16 _tmp590=(_tmp949.f1=_tmp52C,((_tmp949.f2=_tmp52D,_tmp949)));
_npop_handler(0);return _tmp590;}_LL2C9:;};}case Cyc_Absyn_Lte: _LL2C3: {union Cyc_CfFlowInfo_FlowInfo
_tmp591=f;union Cyc_CfFlowInfo_FlowInfo _tmp592=f;{union Cyc_CfFlowInfo_FlowInfo
_tmp593=_tmp591;int _tmp594;struct _tuple13 _tmp595;struct Cyc_Dict_Dict _tmp596;
_LL2F2: if((_tmp593.BottomFL).tag != 1)goto _LL2F4;_tmp594=(int)(_tmp593.BottomFL).val;
_LL2F3: {struct Cyc_Core_Impossible_struct _tmp94F;const char*_tmp94E;struct Cyc_Core_Impossible_struct*
_tmp94D;(int)_throw((void*)((_tmp94D=_cycalloc(sizeof(*_tmp94D)),((_tmp94D[0]=((
_tmp94F.tag=Cyc_Core_Impossible,((_tmp94F.f1=((_tmp94E="anal_test, Lte",
_tag_dyneither(_tmp94E,sizeof(char),15))),_tmp94F)))),_tmp94D)))));}_LL2F4: if((
_tmp593.ReachableFL).tag != 2)goto _LL2F1;_tmp595=(struct _tuple13)(_tmp593.ReachableFL).val;
_tmp596=_tmp595.f1;_LL2F5: _tmp4CE=_tmp596;_LL2F1:;}{void*_tmp59A=_tmp4D2->r;void*
_tmp59C;struct Cyc_Absyn_Vardecl*_tmp59E;void*_tmp5A0;struct Cyc_Absyn_Vardecl*
_tmp5A2;void*_tmp5A4;struct Cyc_Absyn_Vardecl*_tmp5A6;void*_tmp5A8;struct Cyc_Absyn_Vardecl*
_tmp5AA;_LL2F7:{struct Cyc_Absyn_Var_e_struct*_tmp59B=(struct Cyc_Absyn_Var_e_struct*)
_tmp59A;if(_tmp59B->tag != 1)goto _LL2F9;else{_tmp59C=(void*)_tmp59B->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp59D=(struct Cyc_Absyn_Global_b_struct*)_tmp59C;if(
_tmp59D->tag != 1)goto _LL2F9;else{_tmp59E=_tmp59D->f1;}};}}if(!(!_tmp59E->escapes))
goto _LL2F9;_LL2F8: _tmp5A2=_tmp59E;goto _LL2FA;_LL2F9:{struct Cyc_Absyn_Var_e_struct*
_tmp59F=(struct Cyc_Absyn_Var_e_struct*)_tmp59A;if(_tmp59F->tag != 1)goto _LL2FB;
else{_tmp5A0=(void*)_tmp59F->f2;{struct Cyc_Absyn_Local_b_struct*_tmp5A1=(struct
Cyc_Absyn_Local_b_struct*)_tmp5A0;if(_tmp5A1->tag != 4)goto _LL2FB;else{_tmp5A2=
_tmp5A1->f1;}};}}if(!(!_tmp5A2->escapes))goto _LL2FB;_LL2FA: _tmp5A6=_tmp5A2;goto
_LL2FC;_LL2FB:{struct Cyc_Absyn_Var_e_struct*_tmp5A3=(struct Cyc_Absyn_Var_e_struct*)
_tmp59A;if(_tmp5A3->tag != 1)goto _LL2FD;else{_tmp5A4=(void*)_tmp5A3->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp5A5=(struct Cyc_Absyn_Pat_b_struct*)_tmp5A4;if(_tmp5A5->tag
!= 5)goto _LL2FD;else{_tmp5A6=_tmp5A5->f1;}};}}if(!(!_tmp5A6->escapes))goto _LL2FD;
_LL2FC: _tmp5AA=_tmp5A6;goto _LL2FE;_LL2FD:{struct Cyc_Absyn_Var_e_struct*_tmp5A7=(
struct Cyc_Absyn_Var_e_struct*)_tmp59A;if(_tmp5A7->tag != 1)goto _LL2FF;else{
_tmp5A8=(void*)_tmp5A7->f2;{struct Cyc_Absyn_Param_b_struct*_tmp5A9=(struct Cyc_Absyn_Param_b_struct*)
_tmp5A8;if(_tmp5A9->tag != 3)goto _LL2FF;else{_tmp5AA=_tmp5A9->f1;}};}}if(!(!
_tmp5AA->escapes))goto _LL2FF;_LL2FE: {void*_tmp5AB=_tmp4D3->r;union Cyc_Absyn_Cnst
_tmp5AD;struct _tuple6 _tmp5AE;int _tmp5AF;struct Cyc_Absyn_Exp*_tmp5B1;struct Cyc_Absyn_Exp
_tmp5B2;void*_tmp5B3;union Cyc_Absyn_Cnst _tmp5B5;struct _tuple6 _tmp5B6;int _tmp5B7;
enum Cyc_Absyn_Primop _tmp5B9;struct Cyc_List_List*_tmp5BA;struct Cyc_List_List
_tmp5BB;struct Cyc_Absyn_Exp*_tmp5BC;_LL302: {struct Cyc_Absyn_Const_e_struct*
_tmp5AC=(struct Cyc_Absyn_Const_e_struct*)_tmp5AB;if(_tmp5AC->tag != 0)goto _LL304;
else{_tmp5AD=_tmp5AC->f1;if((_tmp5AD.Int_c).tag != 5)goto _LL304;_tmp5AE=(struct
_tuple6)(_tmp5AD.Int_c).val;_tmp5AF=_tmp5AE.f2;}}_LL303: _tmp5B7=_tmp5AF;goto
_LL305;_LL304: {struct Cyc_Absyn_Cast_e_struct*_tmp5B0=(struct Cyc_Absyn_Cast_e_struct*)
_tmp5AB;if(_tmp5B0->tag != 15)goto _LL306;else{_tmp5B1=_tmp5B0->f2;_tmp5B2=*
_tmp5B1;_tmp5B3=_tmp5B2.r;{struct Cyc_Absyn_Const_e_struct*_tmp5B4=(struct Cyc_Absyn_Const_e_struct*)
_tmp5B3;if(_tmp5B4->tag != 0)goto _LL306;else{_tmp5B5=_tmp5B4->f1;if((_tmp5B5.Int_c).tag
!= 5)goto _LL306;_tmp5B6=(struct _tuple6)(_tmp5B5.Int_c).val;_tmp5B7=_tmp5B6.f2;}};}}
_LL305: {struct _RegionHandle*_tmp5BD=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*
_tmp952;struct Cyc_List_List*_tmp951;struct Cyc_List_List*_tmp5BE=(_tmp951=
_region_malloc(_tmp5BD,sizeof(*_tmp951)),((_tmp951->hd=((_tmp952=_region_malloc(
_tmp5BD,sizeof(*_tmp952)),((_tmp952->vd=_tmp5AA,((_tmp952->rop=Cyc_CfFlowInfo_LessEqConst((
unsigned int)_tmp5B7),_tmp952)))))),((_tmp951->tl=_tmp4CF,_tmp951)))));struct
_tuple16 _tmp953;struct _tuple16 _tmp5C0=(_tmp953.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4CE,_tmp5BE),((_tmp953.f2=_tmp592,_tmp953)));_npop_handler(0);return _tmp5C0;}
_LL306: {struct Cyc_Absyn_Primop_e_struct*_tmp5B8=(struct Cyc_Absyn_Primop_e_struct*)
_tmp5AB;if(_tmp5B8->tag != 3)goto _LL308;else{_tmp5B9=_tmp5B8->f1;_tmp5BA=_tmp5B8->f2;
if(_tmp5BA == 0)goto _LL308;_tmp5BB=*_tmp5BA;_tmp5BC=(struct Cyc_Absyn_Exp*)_tmp5BB.hd;}}
_LL307: {void*_tmp5C3=_tmp5BC->r;void*_tmp5C5;struct Cyc_Absyn_Vardecl*_tmp5C7;
void*_tmp5C9;struct Cyc_Absyn_Vardecl*_tmp5CB;void*_tmp5CD;struct Cyc_Absyn_Vardecl*
_tmp5CF;void*_tmp5D1;struct Cyc_Absyn_Vardecl*_tmp5D3;_LL30B:{struct Cyc_Absyn_Var_e_struct*
_tmp5C4=(struct Cyc_Absyn_Var_e_struct*)_tmp5C3;if(_tmp5C4->tag != 1)goto _LL30D;
else{_tmp5C5=(void*)_tmp5C4->f2;{struct Cyc_Absyn_Global_b_struct*_tmp5C6=(struct
Cyc_Absyn_Global_b_struct*)_tmp5C5;if(_tmp5C6->tag != 1)goto _LL30D;else{_tmp5C7=
_tmp5C6->f1;}};}}if(!(!_tmp5C7->escapes))goto _LL30D;_LL30C: _tmp5CB=_tmp5C7;goto
_LL30E;_LL30D:{struct Cyc_Absyn_Var_e_struct*_tmp5C8=(struct Cyc_Absyn_Var_e_struct*)
_tmp5C3;if(_tmp5C8->tag != 1)goto _LL30F;else{_tmp5C9=(void*)_tmp5C8->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp5CA=(struct Cyc_Absyn_Local_b_struct*)_tmp5C9;if(
_tmp5CA->tag != 4)goto _LL30F;else{_tmp5CB=_tmp5CA->f1;}};}}if(!(!_tmp5CB->escapes))
goto _LL30F;_LL30E: _tmp5CF=_tmp5CB;goto _LL310;_LL30F:{struct Cyc_Absyn_Var_e_struct*
_tmp5CC=(struct Cyc_Absyn_Var_e_struct*)_tmp5C3;if(_tmp5CC->tag != 1)goto _LL311;
else{_tmp5CD=(void*)_tmp5CC->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp5CE=(struct Cyc_Absyn_Pat_b_struct*)
_tmp5CD;if(_tmp5CE->tag != 5)goto _LL311;else{_tmp5CF=_tmp5CE->f1;}};}}if(!(!
_tmp5CF->escapes))goto _LL311;_LL310: _tmp5D3=_tmp5CF;goto _LL312;_LL311:{struct Cyc_Absyn_Var_e_struct*
_tmp5D0=(struct Cyc_Absyn_Var_e_struct*)_tmp5C3;if(_tmp5D0->tag != 1)goto _LL313;
else{_tmp5D1=(void*)_tmp5D0->f2;{struct Cyc_Absyn_Param_b_struct*_tmp5D2=(struct
Cyc_Absyn_Param_b_struct*)_tmp5D1;if(_tmp5D2->tag != 3)goto _LL313;else{_tmp5D3=
_tmp5D2->f1;}};}}if(!(!_tmp5D3->escapes))goto _LL313;_LL312: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp5D4=env->fenv;struct Cyc_CfFlowInfo_Reln*_tmp956;struct Cyc_List_List*_tmp955;
struct Cyc_List_List*_tmp5D5=(_tmp955=_region_malloc(_tmp5D4->r,sizeof(*_tmp955)),((
_tmp955->hd=((_tmp956=_region_malloc(_tmp5D4->r,sizeof(*_tmp956)),((_tmp956->vd=
_tmp5AA,((_tmp956->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp5D3),_tmp956)))))),((
_tmp955->tl=_tmp4CF,_tmp955)))));struct _tuple16 _tmp957;struct _tuple16 _tmp5D7=(
_tmp957.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4CE,_tmp5D5),((_tmp957.f2=_tmp592,
_tmp957)));_npop_handler(0);return _tmp5D7;}_LL313:;_LL314: {struct _tuple16
_tmp958;struct _tuple16 _tmp5DB=(_tmp958.f1=_tmp591,((_tmp958.f2=_tmp592,_tmp958)));
_npop_handler(0);return _tmp5DB;}_LL30A:;}_LL308:;_LL309: {struct _tuple16 _tmp959;
struct _tuple16 _tmp5DD=(_tmp959.f1=_tmp591,((_tmp959.f2=_tmp592,_tmp959)));
_npop_handler(0);return _tmp5DD;}_LL301:;}_LL2FF:;_LL300: {struct _tuple16 _tmp95A;
struct _tuple16 _tmp5DF=(_tmp95A.f1=_tmp591,((_tmp95A.f2=_tmp592,_tmp95A)));
_npop_handler(0);return _tmp5DF;}_LL2F6:;};}default: _LL2F0: {struct _tuple16
_tmp95B;struct _tuple16 _tmp5E1=(_tmp95B.f1=f,((_tmp95B.f2=f,_tmp95B)));
_npop_handler(0);return _tmp5E1;}}}_LL291:;};;_pop_region(temp);}_LL28F:;_LL290:
goto _LL282;_LL282:;}{union Cyc_CfFlowInfo_FlowInfo _tmp5E3;void*_tmp5E4;struct
_tuple14 _tmp5E2=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);_tmp5E3=_tmp5E2.f1;
_tmp5E4=_tmp5E2.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp5E5=_tmp5E3;int _tmp5E6;
struct _tuple13 _tmp5E7;struct Cyc_Dict_Dict _tmp5E8;_LL317: if((_tmp5E5.BottomFL).tag
!= 1)goto _LL319;_tmp5E6=(int)(_tmp5E5.BottomFL).val;_LL318: {struct _tuple16
_tmp95C;return(_tmp95C.f1=_tmp5E3,((_tmp95C.f2=_tmp5E3,_tmp95C)));}_LL319: if((
_tmp5E5.ReachableFL).tag != 2)goto _LL316;_tmp5E7=(struct _tuple13)(_tmp5E5.ReachableFL).val;
_tmp5E8=_tmp5E7.f1;_LL31A: {void*_tmp5EA=_tmp5E4;enum Cyc_CfFlowInfo_InitLevel
_tmp5F0;enum Cyc_CfFlowInfo_InitLevel _tmp5F2;enum Cyc_CfFlowInfo_InitLevel
_tmp5F5;_LL31C: {struct Cyc_CfFlowInfo_Zero_struct*_tmp5EB=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp5EA;if(_tmp5EB->tag != 0)goto _LL31E;}_LL31D: {struct _tuple16 _tmp95D;return(
_tmp95D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp95D.f2=_tmp5E3,_tmp95D)));}_LL31E: {
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp5EC=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp5EA;if(_tmp5EC->tag != 2)goto _LL320;}_LL31F: goto _LL321;_LL320: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp5ED=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp5EA;if(_tmp5ED->tag != 1)
goto _LL322;}_LL321: goto _LL323;_LL322: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp5EE=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp5EA;if(_tmp5EE->tag != 5)goto
_LL324;}_LL323: {struct _tuple16 _tmp95E;return(_tmp95E.f1=_tmp5E3,((_tmp95E.f2=
Cyc_CfFlowInfo_BottomFL(),_tmp95E)));}_LL324: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp5EF=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5EA;if(_tmp5EF->tag != 3)goto
_LL326;else{_tmp5F0=_tmp5EF->f1;if(_tmp5F0 != Cyc_CfFlowInfo_NoneIL)goto _LL326;}}
_LL325: goto _LL327;_LL326: {struct Cyc_CfFlowInfo_Esc_struct*_tmp5F1=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp5EA;if(_tmp5F1->tag != 4)goto _LL328;else{_tmp5F2=_tmp5F1->f1;if(_tmp5F2 != Cyc_CfFlowInfo_NoneIL)
goto _LL328;}}_LL327: goto _LL329;_LL328: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp5F3=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp5EA;if(_tmp5F3->tag != 7)goto
_LL32A;}_LL329:{const char*_tmp961;void*_tmp960;(_tmp960=0,Cyc_Tcutil_terr(e->loc,((
_tmp961="expression may not be initialized",_tag_dyneither(_tmp961,sizeof(char),
34))),_tag_dyneither(_tmp960,sizeof(void*),0)));}{struct _tuple16 _tmp962;return(
_tmp962.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp962.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp962)));};_LL32A: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5F4=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp5EA;if(_tmp5F4->tag != 3)goto _LL32C;else{_tmp5F5=_tmp5F4->f1;}}_LL32B: return
Cyc_NewControlFlow_splitzero(env,inflow,_tmp5E3,e,_tmp5F5);_LL32C: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp5F6=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp5EA;if(_tmp5F6->tag != 4)goto _LL32E;}
_LL32D: {struct _tuple16 _tmp963;return(_tmp963.f1=_tmp5E3,((_tmp963.f2=_tmp5E3,
_tmp963)));}_LL32E: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5F7=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp5EA;if(_tmp5F7->tag != 6)goto _LL31B;}_LL32F: {struct Cyc_Core_Impossible_struct
_tmp969;const char*_tmp968;struct Cyc_Core_Impossible_struct*_tmp967;(int)_throw((
void*)((_tmp967=_cycalloc(sizeof(*_tmp967)),((_tmp967[0]=((_tmp969.tag=Cyc_Core_Impossible,((
_tmp969.f1=((_tmp968="anal_test",_tag_dyneither(_tmp968,sizeof(char),10))),
_tmp969)))),_tmp967)))));}_LL31B:;}_LL316:;};};}struct _tuple23{unsigned int f1;
struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple23*ckenv,void*root,void*rval){struct _tuple23 _tmp602;unsigned int
_tmp603;struct Cyc_NewControlFlow_AnalEnv*_tmp604;struct Cyc_Dict_Dict _tmp605;
struct _tuple23*_tmp601=ckenv;_tmp602=*_tmp601;_tmp603=_tmp602.f1;_tmp604=_tmp602.f2;
_tmp605=_tmp602.f3;{void*_tmp606=root;struct Cyc_Absyn_Vardecl*_tmp608;_LL331: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp607=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp606;if(_tmp607->tag != 0)goto _LL333;else{_tmp608=_tmp607->f1;}}_LL332: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp604->fenv)->r,_tmp608->type)){void*_tmp609=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp60D;_LL336: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp60A=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp609;if(_tmp60A->tag != 7)goto _LL338;}_LL337: goto _LL339;_LL338: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp60B=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp609;if(_tmp60B->tag != 0)goto
_LL33A;}_LL339: goto _LL33B;_LL33A: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp60C=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp609;if(_tmp60C->tag != 3)goto _LL33C;
else{_tmp60D=_tmp60C->f1;if(_tmp60D != Cyc_CfFlowInfo_NoneIL)goto _LL33C;}}_LL33B:
goto _LL335;_LL33C:;_LL33D:{const char*_tmp96D;void*_tmp96C[1];struct Cyc_String_pa_struct
_tmp96B;(_tmp96B.tag=0,((_tmp96B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp608->name)),((_tmp96C[0]=& _tmp96B,Cyc_Tcutil_warn(
_tmp603,((_tmp96D="unique pointers reachable from %s may become unreachable",
_tag_dyneither(_tmp96D,sizeof(char),57))),_tag_dyneither(_tmp96C,sizeof(void*),1)))))));}
goto _LL335;_LL335:;}goto _LL330;_LL333:;_LL334: goto _LL330;_LL330:;};}static void
Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo flow){union Cyc_CfFlowInfo_FlowInfo _tmp611=flow;
int _tmp612;struct _tuple13 _tmp613;struct Cyc_Dict_Dict _tmp614;_LL33F: if((_tmp611.BottomFL).tag
!= 1)goto _LL341;_tmp612=(int)(_tmp611.BottomFL).val;_LL340: return;_LL341: if((
_tmp611.ReachableFL).tag != 2)goto _LL33E;_tmp613=(struct _tuple13)(_tmp611.ReachableFL).val;
_tmp614=_tmp613.f1;_LL342:{struct Cyc_List_List*_tmp615=env->param_roots;for(0;
_tmp615 != 0;_tmp615=_tmp615->tl){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp614,
Cyc_CfFlowInfo_lookup_place(_tmp614,(struct Cyc_CfFlowInfo_Place*)_tmp615->hd))!= 
Cyc_CfFlowInfo_AllIL){const char*_tmp970;void*_tmp96F;(_tmp96F=0,Cyc_Tcutil_terr(
loc,((_tmp970="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp970,sizeof(char),76))),_tag_dyneither(_tmp96F,sizeof(void*),0)));}}}
if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple23 _tmp971;struct _tuple23
_tmp618=(_tmp971.f1=loc,((_tmp971.f2=env,((_tmp971.f3=_tmp614,_tmp971)))));((
void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp618,_tmp614);}
return;_LL33E:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp61A=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp61C;struct Cyc_Core_Opt*_tmp61D;
struct Cyc_Absyn_Exp*_tmp61E;struct Cyc_Absyn_Stmt*_tmp61F;unsigned int _tmp620;
struct Cyc_Absyn_Switch_clause*_tmp61B=(struct Cyc_Absyn_Switch_clause*)scs->hd;
_tmp61C=*_tmp61B;_tmp61D=_tmp61C.pat_vars;_tmp61E=_tmp61C.where_clause;_tmp61F=
_tmp61C.body;_tmp620=_tmp61C.loc;{union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_add_vars(
_tmp61A,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp61D))->v,
_tmp61A->unknown_all,_tmp620);union Cyc_CfFlowInfo_FlowInfo clause_outflow;if(
_tmp61E != 0){struct Cyc_Absyn_Exp*wexp=(struct Cyc_Absyn_Exp*)_tmp61E;union Cyc_CfFlowInfo_FlowInfo
_tmp622;union Cyc_CfFlowInfo_FlowInfo _tmp623;struct _tuple16 _tmp621=Cyc_NewControlFlow_anal_test(
env,clause_inflow,wexp);_tmp622=_tmp621.f1;_tmp623=_tmp621.f2;inflow=_tmp623;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp622,_tmp61F);}else{
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp61F);}{union
Cyc_CfFlowInfo_FlowInfo _tmp624=clause_outflow;int _tmp625;_LL344: if((_tmp624.BottomFL).tag
!= 1)goto _LL346;_tmp625=(int)(_tmp624.BottomFL).val;_LL345: goto _LL343;_LL346:;
_LL347: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0){const char*_tmp974;void*_tmp973;(
_tmp973=0,Cyc_Tcutil_terr(_tmp61F->loc,((_tmp974="switch clause may implicitly fallthru",
_tag_dyneither(_tmp974,sizeof(char),38))),_tag_dyneither(_tmp973,sizeof(void*),0)));}
else{const char*_tmp977;void*_tmp976;(_tmp976=0,Cyc_Tcutil_warn(_tmp61F->loc,((
_tmp977="switch clause may implicitly fallthru",_tag_dyneither(_tmp977,sizeof(
char),38))),_tag_dyneither(_tmp976,sizeof(void*),0)));}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL343;_LL343:;};};}return Cyc_CfFlowInfo_BottomFL();}struct
_tuple24{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int
f3;};static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple24*vdenv,
struct Cyc_Absyn_Vardecl*vd){struct _tuple24 _tmp62B;struct Cyc_NewControlFlow_AnalEnv*
_tmp62C;struct Cyc_Dict_Dict _tmp62D;unsigned int _tmp62E;struct _tuple24*_tmp62A=
vdenv;_tmp62B=*_tmp62A;_tmp62C=_tmp62B.f1;_tmp62D=_tmp62B.f2;_tmp62E=_tmp62B.f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp62C->fenv)->r,vd->type)){struct Cyc_CfFlowInfo_VarRoot_struct
_tmp97A;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp979;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp62F=(_tmp979=_region_malloc((_tmp62C->fenv)->r,sizeof(*_tmp979)),((_tmp979[0]=((
_tmp97A.tag=0,((_tmp97A.f1=vd,_tmp97A)))),_tmp979)));void*_tmp630=((void*(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(_tmp62D,(void*)_tmp62F);void*
_tmp631=_tmp630;enum Cyc_CfFlowInfo_InitLevel _tmp635;_LL349: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp632=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp631;if(_tmp632->tag != 7)goto
_LL34B;}_LL34A: goto _LL34C;_LL34B: {struct Cyc_CfFlowInfo_Zero_struct*_tmp633=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp631;if(_tmp633->tag != 0)goto _LL34D;}_LL34C:
goto _LL34E;_LL34D: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp634=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp631;if(_tmp634->tag != 3)goto _LL34F;else{_tmp635=_tmp634->f1;if(_tmp635 != Cyc_CfFlowInfo_NoneIL)
goto _LL34F;}}_LL34E: goto _LL348;_LL34F:;_LL350:{const char*_tmp97E;void*_tmp97D[1];
struct Cyc_String_pa_struct _tmp97C;(_tmp97C.tag=0,((_tmp97C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((
_tmp97D[0]=& _tmp97C,Cyc_Tcutil_warn(_tmp62E,((_tmp97E="unique pointers may still exist after variable %s goes out of scope",
_tag_dyneither(_tmp97E,sizeof(char),68))),_tag_dyneither(_tmp97D,sizeof(void*),1)))))));}
goto _LL348;_LL348:;}}static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){{union Cyc_CfFlowInfo_FlowInfo
_tmp63B=inflow;struct _tuple13 _tmp63C;struct Cyc_Dict_Dict _tmp63D;_LL352: if((
_tmp63B.ReachableFL).tag != 2)goto _LL354;_tmp63C=(struct _tuple13)(_tmp63B.ReachableFL).val;
_tmp63D=_tmp63C.f1;_LL353: {struct _tuple24 _tmp97F;struct _tuple24 _tmp63E=(_tmp97F.f1=
env,((_tmp97F.f2=_tmp63D,((_tmp97F.f3=decl->loc,_tmp97F)))));struct Cyc_CfFlowInfo_FlowEnv*
_tmp63F=env->fenv;{void*_tmp640=decl->r;struct Cyc_Absyn_Vardecl*_tmp642;struct
Cyc_Core_Opt*_tmp644;struct Cyc_Core_Opt _tmp645;struct Cyc_List_List*_tmp646;
struct Cyc_List_List*_tmp648;_LL357: {struct Cyc_Absyn_Var_d_struct*_tmp641=(
struct Cyc_Absyn_Var_d_struct*)_tmp640;if(_tmp641->tag != 0)goto _LL359;else{
_tmp642=_tmp641->f1;}}_LL358: Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp63E,
_tmp642);goto _LL356;_LL359: {struct Cyc_Absyn_Let_d_struct*_tmp643=(struct Cyc_Absyn_Let_d_struct*)
_tmp640;if(_tmp643->tag != 2)goto _LL35B;else{_tmp644=_tmp643->f2;if(_tmp644 == 0)
goto _LL35B;_tmp645=*_tmp644;_tmp646=(struct Cyc_List_List*)_tmp645.v;}}_LL35A:
_tmp648=_tmp646;goto _LL35C;_LL35B: {struct Cyc_Absyn_Letv_d_struct*_tmp647=(
struct Cyc_Absyn_Letv_d_struct*)_tmp640;if(_tmp647->tag != 3)goto _LL35D;else{
_tmp648=_tmp647->f1;}}_LL35C:((void(*)(void(*f)(struct _tuple24*,struct Cyc_Absyn_Vardecl*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,&
_tmp63E,_tmp648);goto _LL356;_LL35D:;_LL35E: goto _LL356;_LL356:;}goto _LL351;}
_LL354:;_LL355: goto _LL351;_LL351:;}return;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp64B;union Cyc_CfFlowInfo_FlowInfo*_tmp64C;struct _tuple17 _tmp64A=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp64B=_tmp64A.f1;_tmp64C=_tmp64A.f2;inflow=*_tmp64C;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp64D=env->fenv;void*_tmp64E=s->r;struct Cyc_Absyn_Exp*_tmp651;struct Cyc_Absyn_Exp*
_tmp653;struct Cyc_Absyn_Exp*_tmp655;struct Cyc_Absyn_Stmt*_tmp657;struct Cyc_Absyn_Stmt*
_tmp658;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_Absyn_Stmt*_tmp65B;struct Cyc_Absyn_Stmt*
_tmp65C;struct _tuple10 _tmp65E;struct Cyc_Absyn_Exp*_tmp65F;struct Cyc_Absyn_Stmt*
_tmp660;struct Cyc_Absyn_Stmt*_tmp661;struct Cyc_Absyn_Stmt*_tmp663;struct _tuple10
_tmp664;struct Cyc_Absyn_Exp*_tmp665;struct Cyc_Absyn_Stmt*_tmp666;struct Cyc_Absyn_Exp*
_tmp668;struct _tuple10 _tmp669;struct Cyc_Absyn_Exp*_tmp66A;struct Cyc_Absyn_Stmt*
_tmp66B;struct _tuple10 _tmp66C;struct Cyc_Absyn_Exp*_tmp66D;struct Cyc_Absyn_Stmt*
_tmp66E;struct Cyc_Absyn_Stmt*_tmp66F;struct Cyc_Absyn_Stmt*_tmp671;struct Cyc_List_List*
_tmp673;struct Cyc_Absyn_Switch_clause**_tmp674;struct Cyc_Absyn_Switch_clause*
_tmp675;struct Cyc_Absyn_Stmt*_tmp677;struct Cyc_Absyn_Stmt*_tmp679;struct Cyc_Absyn_Stmt*
_tmp67B;struct Cyc_Absyn_Exp*_tmp67D;struct Cyc_List_List*_tmp67E;struct Cyc_Absyn_Stmt*
_tmp680;struct Cyc_List_List*_tmp681;struct Cyc_Absyn_Decl*_tmp683;struct Cyc_Absyn_Decl
_tmp684;void*_tmp685;struct Cyc_Absyn_Exp*_tmp687;struct Cyc_Absyn_Tvar*_tmp688;
struct Cyc_Absyn_Vardecl*_tmp689;struct Cyc_Absyn_Stmt*_tmp68A;struct Cyc_Absyn_Decl*
_tmp68C;struct Cyc_Absyn_Stmt*_tmp68D;struct Cyc_Absyn_Stmt*_tmp68F;struct Cyc_Absyn_Exp*
_tmp691;_LL360: {struct Cyc_Absyn_Skip_s_struct*_tmp64F=(struct Cyc_Absyn_Skip_s_struct*)
_tmp64E;if(_tmp64F->tag != 0)goto _LL362;}_LL361: return inflow;_LL362: {struct Cyc_Absyn_Return_s_struct*
_tmp650=(struct Cyc_Absyn_Return_s_struct*)_tmp64E;if(_tmp650->tag != 3)goto _LL364;
else{_tmp651=_tmp650->f1;if(_tmp651 != 0)goto _LL364;}}_LL363: if(env->noreturn){
const char*_tmp982;void*_tmp981;(_tmp981=0,Cyc_Tcutil_terr(s->loc,((_tmp982="`noreturn' function might return",
_tag_dyneither(_tmp982,sizeof(char),33))),_tag_dyneither(_tmp981,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL364: {struct Cyc_Absyn_Return_s_struct*_tmp652=(struct Cyc_Absyn_Return_s_struct*)
_tmp64E;if(_tmp652->tag != 3)goto _LL366;else{_tmp653=_tmp652->f1;}}_LL365: if(env->noreturn){
const char*_tmp985;void*_tmp984;(_tmp984=0,Cyc_Tcutil_terr(s->loc,((_tmp985="`noreturn' function might return",
_tag_dyneither(_tmp985,sizeof(char),33))),_tag_dyneither(_tmp984,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp697;void*_tmp698;struct _tuple14 _tmp696=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp653));_tmp697=_tmp696.f1;
_tmp698=_tmp696.f2;_tmp697=Cyc_NewControlFlow_use_Rval(env,_tmp653->loc,_tmp697,
_tmp698);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp697);return Cyc_CfFlowInfo_BottomFL();};
_LL366: {struct Cyc_Absyn_Exp_s_struct*_tmp654=(struct Cyc_Absyn_Exp_s_struct*)
_tmp64E;if(_tmp654->tag != 1)goto _LL368;else{_tmp655=_tmp654->f1;}}_LL367: return(
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp655)).f1;_LL368: {struct Cyc_Absyn_Seq_s_struct*
_tmp656=(struct Cyc_Absyn_Seq_s_struct*)_tmp64E;if(_tmp656->tag != 2)goto _LL36A;
else{_tmp657=_tmp656->f1;_tmp658=_tmp656->f2;}}_LL369: return Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp657),_tmp658);_LL36A: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp659=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp64E;if(_tmp659->tag != 4)goto
_LL36C;else{_tmp65A=_tmp659->f1;_tmp65B=_tmp659->f2;_tmp65C=_tmp659->f3;}}_LL36B: {
union Cyc_CfFlowInfo_FlowInfo _tmp69A;union Cyc_CfFlowInfo_FlowInfo _tmp69B;struct
_tuple16 _tmp699=Cyc_NewControlFlow_anal_test(env,inflow,_tmp65A);_tmp69A=_tmp699.f1;
_tmp69B=_tmp699.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp69C=Cyc_NewControlFlow_anal_stmt(
env,_tmp69B,_tmp65C);union Cyc_CfFlowInfo_FlowInfo _tmp69D=Cyc_NewControlFlow_anal_stmt(
env,_tmp69A,_tmp65B);return Cyc_CfFlowInfo_join_flow(_tmp64D,env->all_changed,
_tmp69D,_tmp69C);};}_LL36C: {struct Cyc_Absyn_While_s_struct*_tmp65D=(struct Cyc_Absyn_While_s_struct*)
_tmp64E;if(_tmp65D->tag != 5)goto _LL36E;else{_tmp65E=_tmp65D->f1;_tmp65F=_tmp65E.f1;
_tmp660=_tmp65E.f2;_tmp661=_tmp65D->f2;}}_LL36D: {union Cyc_CfFlowInfo_FlowInfo*
_tmp69F;struct _tuple17 _tmp69E=Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp660);_tmp69F=_tmp69E.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp6A0=*_tmp69F;union
Cyc_CfFlowInfo_FlowInfo _tmp6A2;union Cyc_CfFlowInfo_FlowInfo _tmp6A3;struct
_tuple16 _tmp6A1=Cyc_NewControlFlow_anal_test(env,_tmp6A0,_tmp65F);_tmp6A2=
_tmp6A1.f1;_tmp6A3=_tmp6A1.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp6A4=Cyc_NewControlFlow_anal_stmt(
env,_tmp6A2,_tmp661);Cyc_NewControlFlow_update_flow(env,_tmp660,_tmp6A4);return
_tmp6A3;};};}_LL36E: {struct Cyc_Absyn_Do_s_struct*_tmp662=(struct Cyc_Absyn_Do_s_struct*)
_tmp64E;if(_tmp662->tag != 14)goto _LL370;else{_tmp663=_tmp662->f1;_tmp664=_tmp662->f2;
_tmp665=_tmp664.f1;_tmp666=_tmp664.f2;}}_LL36F: {union Cyc_CfFlowInfo_FlowInfo
_tmp6A5=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp663);union Cyc_CfFlowInfo_FlowInfo*
_tmp6A7;struct _tuple17 _tmp6A6=Cyc_NewControlFlow_pre_stmt_check(env,_tmp6A5,
_tmp666);_tmp6A7=_tmp6A6.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp6A8=*_tmp6A7;union
Cyc_CfFlowInfo_FlowInfo _tmp6AA;union Cyc_CfFlowInfo_FlowInfo _tmp6AB;struct
_tuple16 _tmp6A9=Cyc_NewControlFlow_anal_test(env,_tmp6A8,_tmp665);_tmp6AA=
_tmp6A9.f1;_tmp6AB=_tmp6A9.f2;Cyc_NewControlFlow_update_flow(env,_tmp663,_tmp6AA);
return _tmp6AB;};}_LL370: {struct Cyc_Absyn_For_s_struct*_tmp667=(struct Cyc_Absyn_For_s_struct*)
_tmp64E;if(_tmp667->tag != 9)goto _LL372;else{_tmp668=_tmp667->f1;_tmp669=_tmp667->f2;
_tmp66A=_tmp669.f1;_tmp66B=_tmp669.f2;_tmp66C=_tmp667->f3;_tmp66D=_tmp66C.f1;
_tmp66E=_tmp66C.f2;_tmp66F=_tmp667->f4;}}_LL371: {union Cyc_CfFlowInfo_FlowInfo
_tmp6AC=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp668)).f1;union Cyc_CfFlowInfo_FlowInfo*
_tmp6AE;struct _tuple17 _tmp6AD=Cyc_NewControlFlow_pre_stmt_check(env,_tmp6AC,
_tmp66B);_tmp6AE=_tmp6AD.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp6AF=*_tmp6AE;union
Cyc_CfFlowInfo_FlowInfo _tmp6B1;union Cyc_CfFlowInfo_FlowInfo _tmp6B2;struct
_tuple16 _tmp6B0=Cyc_NewControlFlow_anal_test(env,_tmp6AF,_tmp66A);_tmp6B1=
_tmp6B0.f1;_tmp6B2=_tmp6B0.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp6B3=Cyc_NewControlFlow_anal_stmt(
env,_tmp6B1,_tmp66F);union Cyc_CfFlowInfo_FlowInfo*_tmp6B5;struct _tuple17 _tmp6B4=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp6B3,_tmp66E);_tmp6B5=_tmp6B4.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp6B6=*_tmp6B5;union Cyc_CfFlowInfo_FlowInfo _tmp6B7=(Cyc_NewControlFlow_anal_Rexp(
env,0,_tmp6B6,_tmp66D)).f1;Cyc_NewControlFlow_update_flow(env,_tmp66B,_tmp6B7);
return _tmp6B2;};};};}_LL372: {struct Cyc_Absyn_Break_s_struct*_tmp670=(struct Cyc_Absyn_Break_s_struct*)
_tmp64E;if(_tmp670->tag != 6)goto _LL374;else{_tmp671=_tmp670->f1;if(_tmp671 != 0)
goto _LL374;}}_LL373: return Cyc_CfFlowInfo_BottomFL();_LL374: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp672=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp64E;if(_tmp672->tag != 11)goto
_LL376;else{_tmp673=_tmp672->f1;_tmp674=_tmp672->f2;if(_tmp674 == 0)goto _LL376;
_tmp675=*_tmp674;}}_LL375: {struct _RegionHandle _tmp6B8=_new_region("temp");
struct _RegionHandle*temp=& _tmp6B8;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo
_tmp6BA;struct Cyc_List_List*_tmp6BB;struct _tuple20 _tmp6B9=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp673,1,1);_tmp6BA=_tmp6B9.f1;_tmp6BB=_tmp6B9.f2;for(0;_tmp6BB
!= 0;(_tmp6BB=_tmp6BB->tl,_tmp673=_tmp673->tl)){_tmp6BA=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp673))->hd)->loc,
_tmp6BA,(void*)_tmp6BB->hd);}_tmp6BA=Cyc_NewControlFlow_add_vars(_tmp64D,_tmp6BA,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp675->pat_vars))->v,
_tmp64D->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp675->body,_tmp6BA);{union Cyc_CfFlowInfo_FlowInfo _tmp6BC=Cyc_CfFlowInfo_BottomFL();
_npop_handler(0);return _tmp6BC;};};_pop_region(temp);}_LL376: {struct Cyc_Absyn_Break_s_struct*
_tmp676=(struct Cyc_Absyn_Break_s_struct*)_tmp64E;if(_tmp676->tag != 6)goto _LL378;
else{_tmp677=_tmp676->f1;}}_LL377: _tmp679=_tmp677;goto _LL379;_LL378: {struct Cyc_Absyn_Continue_s_struct*
_tmp678=(struct Cyc_Absyn_Continue_s_struct*)_tmp64E;if(_tmp678->tag != 7)goto
_LL37A;else{_tmp679=_tmp678->f1;}}_LL379: _tmp67B=_tmp679;goto _LL37B;_LL37A: {
struct Cyc_Absyn_Goto_s_struct*_tmp67A=(struct Cyc_Absyn_Goto_s_struct*)_tmp64E;
if(_tmp67A->tag != 8)goto _LL37C;else{_tmp67B=_tmp67A->f2;}}_LL37B: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp6BD=_tmp64B->encloser;struct Cyc_Absyn_Stmt*
_tmp6BE=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp67B)))->encloser;while(_tmp6BE != _tmp6BD){struct Cyc_Absyn_Stmt*_tmp6BF=(Cyc_NewControlFlow_get_stmt_annot(
_tmp6BD))->encloser;if(_tmp6BF == _tmp6BD){{const char*_tmp988;void*_tmp987;(
_tmp987=0,Cyc_Tcutil_terr(s->loc,((_tmp988="goto enters local scope or exception handler",
_tag_dyneither(_tmp988,sizeof(char),45))),_tag_dyneither(_tmp987,sizeof(void*),0)));}
break;}_tmp6BD=_tmp6BF;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp67B),inflow);return Cyc_CfFlowInfo_BottomFL();_LL37C: {struct Cyc_Absyn_Switch_s_struct*
_tmp67C=(struct Cyc_Absyn_Switch_s_struct*)_tmp64E;if(_tmp67C->tag != 10)goto
_LL37E;else{_tmp67D=_tmp67C->f1;_tmp67E=_tmp67C->f2;}}_LL37D: {union Cyc_CfFlowInfo_FlowInfo
_tmp6C3;void*_tmp6C4;struct _tuple14 _tmp6C2=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp67D);_tmp6C3=_tmp6C2.f1;_tmp6C4=_tmp6C2.f2;_tmp6C3=Cyc_NewControlFlow_use_Rval(
env,_tmp67D->loc,_tmp6C3,_tmp6C4);return Cyc_NewControlFlow_anal_scs(env,_tmp6C3,
_tmp67E);}_LL37E: {struct Cyc_Absyn_TryCatch_s_struct*_tmp67F=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp64E;if(_tmp67F->tag != 15)goto _LL380;else{_tmp680=_tmp67F->f1;_tmp681=_tmp67F->f2;}}
_LL37F: {int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp680);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp681);{union Cyc_CfFlowInfo_FlowInfo _tmp6C5=scs_outflow;int
_tmp6C6;_LL38B: if((_tmp6C5.BottomFL).tag != 1)goto _LL38D;_tmp6C6=(int)(_tmp6C5.BottomFL).val;
_LL38C: goto _LL38A;_LL38D:;_LL38E: {const char*_tmp98B;void*_tmp98A;(_tmp98A=0,Cyc_Tcutil_terr(
s->loc,((_tmp98B="last catch clause may implicitly fallthru",_tag_dyneither(
_tmp98B,sizeof(char),42))),_tag_dyneither(_tmp98A,sizeof(void*),0)));}_LL38A:;}
outflow=s1_outflow;return outflow;};};}_LL380: {struct Cyc_Absyn_Decl_s_struct*
_tmp682=(struct Cyc_Absyn_Decl_s_struct*)_tmp64E;if(_tmp682->tag != 12)goto _LL382;
else{_tmp683=_tmp682->f1;_tmp684=*_tmp683;_tmp685=_tmp684.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp686=(struct Cyc_Absyn_Alias_d_struct*)_tmp685;if(_tmp686->tag != 5)goto _LL382;
else{_tmp687=_tmp686->f1;_tmp688=_tmp686->f2;_tmp689=_tmp686->f3;}};_tmp68A=
_tmp682->f2;}}_LL381: {union Cyc_CfFlowInfo_FlowInfo _tmp6CA;union Cyc_CfFlowInfo_AbsLVal
_tmp6CB;struct _tuple15 _tmp6C9=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp687);
_tmp6CA=_tmp6C9.f1;_tmp6CB=_tmp6C9.f2;{struct _tuple15 _tmp98C;struct _tuple15
_tmp6CD=(_tmp98C.f1=_tmp6CA,((_tmp98C.f2=_tmp6CB,_tmp98C)));union Cyc_CfFlowInfo_FlowInfo
_tmp6CE;struct _tuple13 _tmp6CF;struct Cyc_Dict_Dict _tmp6D0;struct Cyc_List_List*
_tmp6D1;union Cyc_CfFlowInfo_AbsLVal _tmp6D2;struct Cyc_CfFlowInfo_Place*_tmp6D3;
union Cyc_CfFlowInfo_FlowInfo _tmp6D4;int _tmp6D5;_LL390: _tmp6CE=_tmp6CD.f1;if((
_tmp6CE.ReachableFL).tag != 2)goto _LL392;_tmp6CF=(struct _tuple13)(_tmp6CE.ReachableFL).val;
_tmp6D0=_tmp6CF.f1;_tmp6D1=_tmp6CF.f2;_tmp6D2=_tmp6CD.f2;if((_tmp6D2.PlaceL).tag
!= 1)goto _LL392;_tmp6D3=(struct Cyc_CfFlowInfo_Place*)(_tmp6D2.PlaceL).val;_LL391: {
void*_tmp6D6=Cyc_CfFlowInfo_lookup_place(_tmp6D0,_tmp6D3);void*t=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp687->topt))->v;{void*_tmp6D7=_tmp687->r;struct Cyc_Absyn_Exp*
_tmp6D9;_LL397: {struct Cyc_Absyn_Cast_e_struct*_tmp6D8=(struct Cyc_Absyn_Cast_e_struct*)
_tmp6D7;if(_tmp6D8->tag != 15)goto _LL399;else{_tmp6D9=_tmp6D8->f2;}}_LL398: t=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6D9->topt))->v;goto _LL396;_LL399:;
_LL39A: goto _LL396;_LL396:;}{void*_tmp6DA=Cyc_CfFlowInfo_make_unique_consumed(
_tmp64D,t,_tmp687,env->iteration_num,_tmp6D6);_tmp6D0=Cyc_CfFlowInfo_assign_place(
_tmp64D,_tmp687->loc,_tmp6D0,env->all_changed,_tmp6D3,_tmp6DA);_tmp6CA=Cyc_CfFlowInfo_ReachableFL(
_tmp6D0,_tmp6D1);{void*leaf;switch(Cyc_CfFlowInfo_initlevel(_tmp64D,_tmp6D0,
_tmp6D6)){case Cyc_CfFlowInfo_AllIL: _LL39B: leaf=_tmp64D->unknown_all;break;case
Cyc_CfFlowInfo_NoneIL: _LL39C: leaf=_tmp64D->unknown_none;break;case Cyc_CfFlowInfo_ThisIL:
_LL39D: leaf=_tmp64D->unknown_this;break;}{void*_tmp6DB=Cyc_CfFlowInfo_typ_to_absrval(
_tmp64D,t,0,leaf);_tmp6CA=Cyc_NewControlFlow_use_Rval(env,_tmp687->loc,_tmp6CA,
_tmp6D6);{struct Cyc_List_List _tmp98D;struct Cyc_List_List _tmp6DC=(_tmp98D.hd=
_tmp689,((_tmp98D.tl=0,_tmp98D)));_tmp6CA=Cyc_NewControlFlow_add_vars(_tmp64D,
_tmp6CA,(struct Cyc_List_List*)& _tmp6DC,_tmp64D->unknown_all,s->loc);_tmp6CA=Cyc_NewControlFlow_anal_stmt(
env,_tmp6CA,_tmp68A);{union Cyc_CfFlowInfo_FlowInfo _tmp6DD=_tmp6CA;struct _tuple13
_tmp6DE;struct Cyc_Dict_Dict _tmp6DF;struct Cyc_List_List*_tmp6E0;_LL3A0: if((
_tmp6DD.ReachableFL).tag != 2)goto _LL3A2;_tmp6DE=(struct _tuple13)(_tmp6DD.ReachableFL).val;
_tmp6DF=_tmp6DE.f1;_tmp6E0=_tmp6DE.f2;_LL3A1: _tmp6DF=Cyc_CfFlowInfo_assign_place(
_tmp64D,s->loc,_tmp6DF,env->all_changed,_tmp6D3,_tmp6DB);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp6DF,_tmp6E0);return outflow;_LL3A2:;_LL3A3: return _tmp6CA;_LL39F:;};};};};};}
_LL392: _tmp6D4=_tmp6CD.f1;if((_tmp6D4.BottomFL).tag != 1)goto _LL394;_tmp6D5=(int)(
_tmp6D4.BottomFL).val;_LL393: return _tmp6CA;_LL394:;_LL395:{const char*_tmp990;
void*_tmp98F;(_tmp98F=0,Cyc_Tcutil_terr(_tmp687->loc,((_tmp990="bad alias expression--no unique path found",
_tag_dyneither(_tmp990,sizeof(char),43))),_tag_dyneither(_tmp98F,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL38F:;};}_LL382: {struct Cyc_Absyn_Decl_s_struct*
_tmp68B=(struct Cyc_Absyn_Decl_s_struct*)_tmp64E;if(_tmp68B->tag != 12)goto _LL384;
else{_tmp68C=_tmp68B->f1;_tmp68D=_tmp68B->f2;}}_LL383: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp68C),_tmp68D);if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp68C);return outflow;
_LL384: {struct Cyc_Absyn_Label_s_struct*_tmp68E=(struct Cyc_Absyn_Label_s_struct*)
_tmp64E;if(_tmp68E->tag != 13)goto _LL386;else{_tmp68F=_tmp68E->f2;}}_LL385: return
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp68F);_LL386: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp690=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp64E;if(_tmp690->tag != 16)goto
_LL388;else{_tmp691=_tmp690->f1;}}_LL387: {union Cyc_CfFlowInfo_FlowInfo _tmp6E5;
void*_tmp6E6;struct _tuple14 _tmp6E4=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,
_tmp691);_tmp6E5=_tmp6E4.f1;_tmp6E6=_tmp6E4.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp6E7=Cyc_NewControlFlow_use_Rval(env,_tmp691->loc,_tmp6E5,_tmp6E6);void*
_tmp6E8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp691->topt))->v);
void*_tmp6EA;_LL3A5: {struct Cyc_Absyn_RgnHandleType_struct*_tmp6E9=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6E8;if(_tmp6E9->tag != 15)goto _LL3A7;else{_tmp6EA=(void*)_tmp6E9->f1;}}_LL3A6:
return Cyc_CfFlowInfo_kill_flow_region(_tmp64D,_tmp6E5,_tmp6EA);_LL3A7:;_LL3A8: {
struct Cyc_Core_Impossible_struct _tmp996;const char*_tmp995;struct Cyc_Core_Impossible_struct*
_tmp994;(int)_throw((void*)((_tmp994=_cycalloc(sizeof(*_tmp994)),((_tmp994[0]=((
_tmp996.tag=Cyc_Core_Impossible,((_tmp996.f1=((_tmp995="anal_stmt -- reset_region",
_tag_dyneither(_tmp995,sizeof(char),26))),_tmp996)))),_tmp994)))));}_LL3A4:;};}
_LL388:;_LL389: {struct Cyc_Core_Impossible_struct _tmp99C;const char*_tmp99B;
struct Cyc_Core_Impossible_struct*_tmp99A;(int)_throw((void*)((_tmp99A=_cycalloc(
sizeof(*_tmp99A)),((_tmp99A[0]=((_tmp99C.tag=Cyc_Core_Impossible,((_tmp99C.f1=((
_tmp99B="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(
_tmp99B,sizeof(char),56))),_tmp99C)))),_tmp99A)))));}_LL35F:;};}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*
_tmp6F1=env->fenv;void*_tmp6F2=decl->r;struct Cyc_Absyn_Vardecl*_tmp6F4;struct Cyc_Core_Opt*
_tmp6F6;struct Cyc_Core_Opt _tmp6F7;struct Cyc_List_List*_tmp6F8;struct Cyc_Absyn_Exp*
_tmp6F9;struct Cyc_List_List*_tmp6FB;struct Cyc_Absyn_Fndecl*_tmp6FD;struct Cyc_Absyn_Tvar*
_tmp6FF;struct Cyc_Absyn_Vardecl*_tmp700;int _tmp701;struct Cyc_Absyn_Exp*_tmp702;
_LL3AA: {struct Cyc_Absyn_Var_d_struct*_tmp6F3=(struct Cyc_Absyn_Var_d_struct*)
_tmp6F2;if(_tmp6F3->tag != 0)goto _LL3AC;else{_tmp6F4=_tmp6F3->f1;}}_LL3AB: {
struct Cyc_List_List _tmp99D;struct Cyc_List_List _tmp703=(_tmp99D.hd=_tmp6F4,((
_tmp99D.tl=0,_tmp99D)));inflow=Cyc_NewControlFlow_add_vars(_tmp6F1,inflow,(
struct Cyc_List_List*)& _tmp703,_tmp6F1->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*
_tmp704=_tmp6F4->initializer;if(_tmp704 == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo
_tmp706;void*_tmp707;struct _tuple14 _tmp705=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,(struct Cyc_Absyn_Exp*)_tmp704);_tmp706=_tmp705.f1;_tmp707=_tmp705.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp708=_tmp706;int _tmp709;struct _tuple13 _tmp70A;
struct Cyc_Dict_Dict _tmp70B;struct Cyc_List_List*_tmp70C;_LL3B7: if((_tmp708.BottomFL).tag
!= 1)goto _LL3B9;_tmp709=(int)(_tmp708.BottomFL).val;_LL3B8: return Cyc_CfFlowInfo_BottomFL();
_LL3B9: if((_tmp708.ReachableFL).tag != 2)goto _LL3B6;_tmp70A=(struct _tuple13)(
_tmp708.ReachableFL).val;_tmp70B=_tmp70A.f1;_tmp70C=_tmp70A.f2;_LL3BA:{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp9A3;struct Cyc_CfFlowInfo_VarRoot_struct _tmp9A2;struct Cyc_CfFlowInfo_Place*
_tmp9A1;_tmp70B=Cyc_CfFlowInfo_assign_place(_tmp6F1,decl->loc,_tmp70B,env->all_changed,((
_tmp9A1=_region_malloc(env->r,sizeof(*_tmp9A1)),((_tmp9A1->root=(void*)((_tmp9A3=
_region_malloc(env->r,sizeof(*_tmp9A3)),((_tmp9A3[0]=((_tmp9A2.tag=0,((_tmp9A2.f1=
_tmp6F4,_tmp9A2)))),_tmp9A3)))),((_tmp9A1->fields=0,_tmp9A1)))))),_tmp707);}
_tmp70C=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp70C,_tmp6F4,(struct Cyc_Absyn_Exp*)
_check_null(_tmp6F4->initializer));{union Cyc_CfFlowInfo_FlowInfo _tmp710=Cyc_CfFlowInfo_ReachableFL(
_tmp70B,_tmp70C);Cyc_NewControlFlow_update_tryflow(env,_tmp710);return _tmp710;};
_LL3B6:;};};};}_LL3AC: {struct Cyc_Absyn_Let_d_struct*_tmp6F5=(struct Cyc_Absyn_Let_d_struct*)
_tmp6F2;if(_tmp6F5->tag != 2)goto _LL3AE;else{_tmp6F6=_tmp6F5->f2;if(_tmp6F6 == 0)
goto _LL3AE;_tmp6F7=*_tmp6F6;_tmp6F8=(struct Cyc_List_List*)_tmp6F7.v;_tmp6F9=
_tmp6F5->f3;}}_LL3AD: {union Cyc_CfFlowInfo_FlowInfo _tmp713;void*_tmp714;struct
_tuple14 _tmp712=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6F9);_tmp713=
_tmp712.f1;_tmp714=_tmp712.f2;_tmp713=Cyc_NewControlFlow_use_Rval(env,_tmp6F9->loc,
_tmp713,_tmp714);return Cyc_NewControlFlow_add_vars(_tmp6F1,_tmp713,_tmp6F8,
_tmp6F1->unknown_all,decl->loc);}_LL3AE: {struct Cyc_Absyn_Letv_d_struct*_tmp6FA=(
struct Cyc_Absyn_Letv_d_struct*)_tmp6F2;if(_tmp6FA->tag != 3)goto _LL3B0;else{
_tmp6FB=_tmp6FA->f1;}}_LL3AF: return Cyc_NewControlFlow_add_vars(_tmp6F1,inflow,
_tmp6FB,_tmp6F1->unknown_none,decl->loc);_LL3B0: {struct Cyc_Absyn_Fn_d_struct*
_tmp6FC=(struct Cyc_Absyn_Fn_d_struct*)_tmp6F2;if(_tmp6FC->tag != 1)goto _LL3B2;
else{_tmp6FD=_tmp6FC->f1;}}_LL3B1: Cyc_NewControlFlow_check_nested_fun(_tmp6F1,
inflow,_tmp6FD);{void*_tmp715=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp6FD->cached_typ))->v;
struct Cyc_Absyn_Vardecl*_tmp716=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp6FD->fn_vardecl);
struct Cyc_List_List*_tmp9A4;return Cyc_NewControlFlow_add_vars(_tmp6F1,inflow,((
_tmp9A4=_region_malloc(env->r,sizeof(*_tmp9A4)),((_tmp9A4->hd=_tmp716,((_tmp9A4->tl=
0,_tmp9A4)))))),_tmp6F1->unknown_all,decl->loc);};_LL3B2: {struct Cyc_Absyn_Region_d_struct*
_tmp6FE=(struct Cyc_Absyn_Region_d_struct*)_tmp6F2;if(_tmp6FE->tag != 4)goto _LL3B4;
else{_tmp6FF=_tmp6FE->f1;_tmp700=_tmp6FE->f2;_tmp701=_tmp6FE->f3;_tmp702=_tmp6FE->f4;}}
_LL3B3: if(_tmp702 != 0){struct Cyc_Absyn_Exp*_tmp718=(struct Cyc_Absyn_Exp*)_tmp702;
union Cyc_CfFlowInfo_FlowInfo _tmp71A;void*_tmp71B;struct _tuple14 _tmp719=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp718);_tmp71A=_tmp719.f1;_tmp71B=_tmp719.f2;inflow=Cyc_NewControlFlow_use_Rval(
env,_tmp718->loc,_tmp71A,_tmp71B);}{struct Cyc_List_List _tmp9A5;struct Cyc_List_List
_tmp71C=(_tmp9A5.hd=_tmp700,((_tmp9A5.tl=0,_tmp9A5)));return Cyc_NewControlFlow_add_vars(
_tmp6F1,inflow,(struct Cyc_List_List*)& _tmp71C,_tmp6F1->unknown_all,decl->loc);};
_LL3B4:;_LL3B5: {struct Cyc_Core_Impossible_struct _tmp9AB;const char*_tmp9AA;
struct Cyc_Core_Impossible_struct*_tmp9A9;(int)_throw((void*)((_tmp9A9=_cycalloc(
sizeof(*_tmp9A9)),((_tmp9A9[0]=((_tmp9AB.tag=Cyc_Core_Impossible,((_tmp9AB.f1=((
_tmp9AA="anal_decl: unexpected decl variant",_tag_dyneither(_tmp9AA,sizeof(char),
35))),_tmp9AB)))),_tmp9A9)))));}_LL3A9:;}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){struct _RegionHandle _tmp721=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp721;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*
fenv=Cyc_CfFlowInfo_new_flow_env(frgn);Cyc_NewControlFlow_check_nested_fun(fenv,
Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);};_pop_region(frgn);}static
int Cyc_NewControlFlow_hash_ptr(void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle*_tmp722=fenv->r;unsigned int _tmp723=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp723);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp724=inflow;int _tmp725;struct
_tuple13 _tmp726;struct Cyc_Dict_Dict _tmp727;struct Cyc_List_List*_tmp728;_LL3BC:
if((_tmp724.BottomFL).tag != 1)goto _LL3BE;_tmp725=(int)(_tmp724.BottomFL).val;
_LL3BD: {const char*_tmp9AE;void*_tmp9AD;(_tmp9AD=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9AE="check_fun",
_tag_dyneither(_tmp9AE,sizeof(char),10))),_tag_dyneither(_tmp9AD,sizeof(void*),0)));}
_LL3BE: if((_tmp724.ReachableFL).tag != 2)goto _LL3BB;_tmp726=(struct _tuple13)(
_tmp724.ReachableFL).val;_tmp727=_tmp726.f1;_tmp728=_tmp726.f2;_LL3BF: {struct
Cyc_List_List*atts;{void*_tmp72B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp72D;struct Cyc_List_List*
_tmp72E;_LL3C1: {struct Cyc_Absyn_FnType_struct*_tmp72C=(struct Cyc_Absyn_FnType_struct*)
_tmp72B;if(_tmp72C->tag != 9)goto _LL3C3;else{_tmp72D=_tmp72C->f1;_tmp72E=_tmp72D.attributes;}}
_LL3C2: atts=_tmp72E;goto _LL3C0;_LL3C3:;_LL3C4: {const char*_tmp9B1;void*_tmp9B0;(
_tmp9B0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp9B1="check_fun: non-function type cached with fndecl_t",_tag_dyneither(
_tmp9B1,sizeof(char),50))),_tag_dyneither(_tmp9B0,sizeof(void*),0)));}_LL3C0:;}
for(0;atts != 0;atts=atts->tl){void*_tmp731=(void*)atts->hd;int _tmp733;int _tmp735;
_LL3C6: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp732=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp731;if(_tmp732->tag != 21)goto _LL3C8;else{_tmp733=_tmp732->f1;}}_LL3C7: {
unsigned int j=(unsigned int)_tmp733;struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(
- 1,0);if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{const char*_tmp9B4;
void*_tmp9B3;(_tmp9B3=0,Cyc_Tcutil_terr(_tmp723,((_tmp9B4="noliveunique attribute exceeds number of parameters",
_tag_dyneither(_tmp9B4,sizeof(char),52))),_tag_dyneither(_tmp9B3,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp738=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));void*t=Cyc_Tcutil_compress(
_tmp738->type);if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp9B7;void*
_tmp9B6;(_tmp9B6=0,Cyc_Tcutil_terr(_tmp723,((_tmp9B7="noliveunique attribute requires unique pointer type",
_tag_dyneither(_tmp9B7,sizeof(char),52))),_tag_dyneither(_tmp9B6,sizeof(void*),0)));}
continue;}{void*elttype=Cyc_Tcutil_pointer_elt_type(t);void*_tmp73B=Cyc_CfFlowInfo_make_unique_consumed(
fenv,elttype,bogus_exp,- 1,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_struct _tmp9BA;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp9B9;struct Cyc_CfFlowInfo_InitParam_struct*_tmp73C=(_tmp9B9=_region_malloc(
_tmp722,sizeof(*_tmp9B9)),((_tmp9B9[0]=((_tmp9BA.tag=2,((_tmp9BA.f1=(int)j,((
_tmp9BA.f2=(void*)t,_tmp9BA)))))),_tmp9B9)));struct Cyc_CfFlowInfo_Place*_tmp9BB;
struct Cyc_CfFlowInfo_Place*_tmp73D=(_tmp9BB=_region_malloc(_tmp722,sizeof(*
_tmp9BB)),((_tmp9BB->root=(void*)_tmp73C,((_tmp9BB->fields=0,_tmp9BB)))));
_tmp727=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp727,(void*)_tmp73C,_tmp73B);{struct Cyc_CfFlowInfo_AddressOf_struct _tmp9C1;
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp9C0;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp9BE;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp9BD;_tmp727=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp727,(void*)((_tmp9BD=
_region_malloc(_tmp722,sizeof(*_tmp9BD)),((_tmp9BD[0]=((_tmp9BE.tag=0,((_tmp9BE.f1=
_tmp738,_tmp9BE)))),_tmp9BD)))),(void*)((_tmp9C0=_region_malloc(_tmp722,sizeof(*
_tmp9C0)),((_tmp9C0[0]=((_tmp9C1.tag=5,((_tmp9C1.f1=_tmp73D,_tmp9C1)))),_tmp9C0)))));}
goto _LL3C5;};};}_LL3C8: {struct Cyc_Absyn_Initializes_att_struct*_tmp734=(struct
Cyc_Absyn_Initializes_att_struct*)_tmp731;if(_tmp734->tag != 20)goto _LL3CA;else{
_tmp735=_tmp734->f1;}}_LL3C9: {unsigned int j=(unsigned int)_tmp735;if(j > ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){{const char*_tmp9C4;void*_tmp9C3;(_tmp9C3=0,
Cyc_Tcutil_terr(_tmp723,((_tmp9C4="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp9C4,sizeof(char),51))),_tag_dyneither(_tmp9C3,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp747=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp748=Cyc_Tcutil_compress(
_tmp747->type);struct Cyc_Absyn_PtrInfo _tmp74A;void*_tmp74B;struct Cyc_Absyn_PtrAtts
_tmp74C;union Cyc_Absyn_Constraint*_tmp74D;union Cyc_Absyn_Constraint*_tmp74E;
union Cyc_Absyn_Constraint*_tmp74F;_LL3CD: {struct Cyc_Absyn_PointerType_struct*
_tmp749=(struct Cyc_Absyn_PointerType_struct*)_tmp748;if(_tmp749->tag != 5)goto
_LL3CF;else{_tmp74A=_tmp749->f1;_tmp74B=_tmp74A.elt_typ;_tmp74C=_tmp74A.ptr_atts;
_tmp74D=_tmp74C.nullable;_tmp74E=_tmp74C.bounds;_tmp74F=_tmp74C.zero_term;}}
_LL3CE: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp74D)){
const char*_tmp9C7;void*_tmp9C6;(_tmp9C6=0,Cyc_Tcutil_terr(_tmp723,((_tmp9C7="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp9C7,sizeof(char),55))),_tag_dyneither(_tmp9C6,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp74E)){const char*_tmp9CA;void*_tmp9C9;(_tmp9C9=0,
Cyc_Tcutil_terr(_tmp723,((_tmp9CA="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp9CA,sizeof(char),57))),_tag_dyneither(_tmp9C9,sizeof(void*),0)));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp74F)){
const char*_tmp9CD;void*_tmp9CC;(_tmp9CC=0,Cyc_Tcutil_terr(_tmp723,((_tmp9CD="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp9CD,sizeof(char),77))),_tag_dyneither(_tmp9CC,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_struct _tmp9D0;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp9CF;struct Cyc_CfFlowInfo_InitParam_struct*_tmp756=(_tmp9CF=_region_malloc(
_tmp722,sizeof(*_tmp9CF)),((_tmp9CF[0]=((_tmp9D0.tag=2,((_tmp9D0.f1=(int)j,((
_tmp9D0.f2=(void*)_tmp74B,_tmp9D0)))))),_tmp9CF)));struct Cyc_CfFlowInfo_Place*
_tmp9D1;struct Cyc_CfFlowInfo_Place*_tmp757=(_tmp9D1=_region_malloc(_tmp722,
sizeof(*_tmp9D1)),((_tmp9D1->root=(void*)_tmp756,((_tmp9D1->fields=0,_tmp9D1)))));
_tmp727=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp727,(void*)_tmp756,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp74B,0,fenv->esc_none));{
struct Cyc_CfFlowInfo_AddressOf_struct _tmp9D7;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp9D6;struct Cyc_CfFlowInfo_VarRoot_struct _tmp9D4;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp9D3;_tmp727=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp727,(void*)((_tmp9D3=_region_malloc(_tmp722,sizeof(*_tmp9D3)),((_tmp9D3[0]=((
_tmp9D4.tag=0,((_tmp9D4.f1=_tmp747,_tmp9D4)))),_tmp9D3)))),(void*)((_tmp9D6=
_region_malloc(_tmp722,sizeof(*_tmp9D6)),((_tmp9D6[0]=((_tmp9D7.tag=5,((_tmp9D7.f1=
_tmp757,_tmp9D7)))),_tmp9D6)))));}{struct Cyc_List_List*_tmp9D8;param_roots=((
_tmp9D8=_region_malloc(_tmp722,sizeof(*_tmp9D8)),((_tmp9D8->hd=_tmp757,((_tmp9D8->tl=
param_roots,_tmp9D8))))));}goto _LL3CC;};_LL3CF:;_LL3D0: {const char*_tmp9DB;void*
_tmp9DA;(_tmp9DA=0,Cyc_Tcutil_terr(_tmp723,((_tmp9DB="initializes attribute on non-pointer",
_tag_dyneither(_tmp9DB,sizeof(char),37))),_tag_dyneither(_tmp9DA,sizeof(void*),0)));}
_LL3CC:;}goto _LL3C5;};}_LL3CA:;_LL3CB: goto _LL3C5;_LL3C5:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp727,_tmp728);}_LL3BB:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp722,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp9DC;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp9DC=_region_malloc(_tmp722,sizeof(*_tmp9DC)),((
_tmp9DC->r=_tmp722,((_tmp9DC->fenv=fenv,((_tmp9DC->iterate_again=1,((_tmp9DC->iteration_num=
0,((_tmp9DC->in_try=0,((_tmp9DC->tryflow=inflow,((_tmp9DC->all_changed=0,((
_tmp9DC->noreturn=noreturn,((_tmp9DC->param_roots=param_roots,((_tmp9DC->flow_table=
flow_table,_tmp9DC)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
union Cyc_CfFlowInfo_FlowInfo _tmp762=outflow;int _tmp763;_LL3D2: if((_tmp762.BottomFL).tag
!= 1)goto _LL3D4;_tmp763=(int)(_tmp762.BottomFL).val;_LL3D3: goto _LL3D1;_LL3D4:;
_LL3D5: Cyc_NewControlFlow_check_init_params(_tmp723,env,outflow);if(noreturn){
const char*_tmp9DF;void*_tmp9DE;(_tmp9DE=0,Cyc_Tcutil_terr(_tmp723,((_tmp9DF="`noreturn' function might (implicitly) return",
_tag_dyneither(_tmp9DF,sizeof(char),46))),_tag_dyneither(_tmp9DE,sizeof(void*),0)));}
else{void*_tmp766=Cyc_Tcutil_compress(fd->ret_type);_LL3D7: {struct Cyc_Absyn_VoidType_struct*
_tmp767=(struct Cyc_Absyn_VoidType_struct*)_tmp766;if(_tmp767->tag != 0)goto _LL3D9;}
_LL3D8: goto _LL3D6;_LL3D9: {struct Cyc_Absyn_FloatType_struct*_tmp768=(struct Cyc_Absyn_FloatType_struct*)
_tmp766;if(_tmp768->tag != 7)goto _LL3DB;}_LL3DA: goto _LL3DC;_LL3DB: {struct Cyc_Absyn_IntType_struct*
_tmp769=(struct Cyc_Absyn_IntType_struct*)_tmp766;if(_tmp769->tag != 6)goto _LL3DD;}
_LL3DC:{const char*_tmp9E2;void*_tmp9E1;(_tmp9E1=0,Cyc_Tcutil_warn(_tmp723,((
_tmp9E2="function may not return a value",_tag_dyneither(_tmp9E2,sizeof(char),32))),
_tag_dyneither(_tmp9E1,sizeof(void*),0)));}goto _LL3D6;_LL3DD:;_LL3DE:{const char*
_tmp9E5;void*_tmp9E4;(_tmp9E4=0,Cyc_Tcutil_terr(_tmp723,((_tmp9E5="function may not return a value",
_tag_dyneither(_tmp9E5,sizeof(char),32))),_tag_dyneither(_tmp9E4,sizeof(void*),0)));}
goto _LL3D6;_LL3D6:;}goto _LL3D1;_LL3D1:;};};};}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp76F=((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp771;struct Cyc_List_List*_tmp773;struct Cyc_List_List*
_tmp775;struct Cyc_List_List*_tmp777;_LL3E0: {struct Cyc_Absyn_Fn_d_struct*_tmp770=(
struct Cyc_Absyn_Fn_d_struct*)_tmp76F;if(_tmp770->tag != 1)goto _LL3E2;else{_tmp771=
_tmp770->f1;}}_LL3E1: Cyc_NewControlFlow_check_fun(_tmp771);goto _LL3DF;_LL3E2: {
struct Cyc_Absyn_ExternC_d_struct*_tmp772=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp76F;if(_tmp772->tag != 12)goto _LL3E4;else{_tmp773=_tmp772->f1;}}_LL3E3:
_tmp775=_tmp773;goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Using_d_struct*_tmp774=(
struct Cyc_Absyn_Using_d_struct*)_tmp76F;if(_tmp774->tag != 11)goto _LL3E6;else{
_tmp775=_tmp774->f2;}}_LL3E5: _tmp777=_tmp775;goto _LL3E7;_LL3E6: {struct Cyc_Absyn_Namespace_d_struct*
_tmp776=(struct Cyc_Absyn_Namespace_d_struct*)_tmp76F;if(_tmp776->tag != 10)goto
_LL3E8;else{_tmp777=_tmp776->f2;}}_LL3E7: Cyc_NewControlFlow_cf_check(_tmp777);
goto _LL3DF;_LL3E8: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp778=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp76F;if(_tmp778->tag != 13)goto _LL3EA;}_LL3E9: goto _LL3DF;_LL3EA:;_LL3EB: goto
_LL3DF;_LL3DF:;}}
