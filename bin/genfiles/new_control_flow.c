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
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int
tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};typedef union Cyc_Absyn_Nmspace
Cyc_Absyn_nmspace_t;union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*
Cyc_Absyn_qvar_opt_t;typedef struct _tuple1*Cyc_Absyn_typedef_name_t;typedef struct
_tuple1*Cyc_Absyn_typedef_name_opt_t;typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;typedef void*Cyc_Absyn_kindbound_t;
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;typedef void*Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;typedef struct Cyc_Absyn_PtrInfo
Cyc_Absyn_ptr_info_t;typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;typedef struct Cyc_Absyn_DatatypeInfo
Cyc_Absyn_datatype_info_t;typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;typedef struct Cyc_Absyn_ArrayInfo
Cyc_Absyn_array_info_t;typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;
typedef void*Cyc_Absyn_type_opt_t;typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
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
struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct
Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual
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
struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;
struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{int tag;
struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int tag;
struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign
f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_struct{int tag;int f1;};
struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{
int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void*f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_RefCntRgn_struct{
int tag;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_TypeDeclType_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;
};struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};
struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;void*f4;struct Cyc_List_List*f5;};typedef void*Cyc_Absyn_funcparams_t;enum Cyc_Absyn_Format_Type{
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
f1;void*f2;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
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
int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
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
int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;
unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_struct{int tag;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;
};struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int
try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;
struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct
Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};
struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int
loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int
escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct
_tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;
struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind
kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};
struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
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
_tmp77B;struct Cyc_NewControlFlow_CFStmtAnnot _tmp77A;struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp779;enclosee->annot=(void*)((_tmp779=_cycalloc(sizeof(*_tmp779)),((_tmp779[0]=((
_tmp77B.tag=Cyc_NewControlFlow_CFAnnot,((_tmp77B.f1=((_tmp77A.encloser=encloser,((
_tmp77A.visited=0,_tmp77A)))),_tmp77B)))),_tmp779))));}struct Cyc_NewControlFlow_AnalEnv{
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
_LL4: {struct Cyc_Core_Impossible_struct _tmp781;const char*_tmp780;struct Cyc_Core_Impossible_struct*
_tmp77F;(int)_throw((void*)((_tmp77F=_cycalloc(sizeof(*_tmp77F)),((_tmp77F[0]=((
_tmp781.tag=Cyc_Core_Impossible,((_tmp781.f1=((_tmp780="ControlFlow -- wrong stmt annotation",
_tag_dyneither(_tmp780,sizeof(char),37))),_tmp781)))),_tmp77F)))));}_LL0:;}
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*_tmp782;union Cyc_CfFlowInfo_FlowInfo*
res=(_tmp782=_region_malloc(env->r,sizeof(*_tmp782)),((_tmp782[0]=Cyc_CfFlowInfo_BottomFL(),
_tmp782)));((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union
Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);return
res;}return*sflow;}struct _tuple17{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union
Cyc_CfFlowInfo_FlowInfo*f2;};static struct _tuple17 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*
_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);_tmpB->visited=
env->iteration_num;{struct _tuple17 _tmp783;return(_tmp783.f1=_tmpB,((_tmp783.f2=
_tmpC,_tmp783)));};}static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*
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
_tmp787;void*_tmp786[1];struct Cyc_String_pa_struct _tmp785;(_tmp785.tag=0,((
_tmp785.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp15->name)),((_tmp786[0]=& _tmp785,Cyc_Tcutil_warn(_tmp11,((_tmp787="unique pointer(s) reachable from %s may become unreachable.",
_tag_dyneither(_tmp787,sizeof(char),60))),_tag_dyneither(_tmp786,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}goto _LL5;}goto _LL5;_LL8:;_LL9: goto _LL5;_LL5:;};}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
flow,*_tmp1F);if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple16 _tmp788;
struct _tuple16 _tmp22=(_tmp788.f1=flow,((_tmp788.f2=_tmp20,_tmp788)));union Cyc_CfFlowInfo_FlowInfo
_tmp23;struct _tuple13 _tmp24;struct Cyc_Dict_Dict _tmp25;union Cyc_CfFlowInfo_FlowInfo
_tmp26;struct _tuple13 _tmp27;struct Cyc_Dict_Dict _tmp28;_LL14: _tmp23=_tmp22.f1;if((
_tmp23.ReachableFL).tag != 2)goto _LL16;_tmp24=(struct _tuple13)(_tmp23.ReachableFL).val;
_tmp25=_tmp24.f1;_tmp26=_tmp22.f2;if((_tmp26.ReachableFL).tag != 2)goto _LL16;
_tmp27=(struct _tuple13)(_tmp26.ReachableFL).val;_tmp28=_tmp27.f1;_LL15: {struct
_tuple18 _tmp789;struct _tuple18 _tmp29=(_tmp789.f1=env,((_tmp789.f2=s->loc,((
_tmp789.f3=_tmp28,_tmp789)))));((void(*)(void(*f)(struct _tuple18*,void*,void*),
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
_LL1C: for(0;vds != 0;vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct _tmp78C;
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp78B;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp30=(_tmp78B=_region_malloc(fenv->r,sizeof(*_tmp78B)),((_tmp78B[0]=((_tmp78C.tag=
0,((_tmp78C.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp78C)))),_tmp78B)));_tmp2E=((
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
const char*_tmp78F;void*_tmp78E;(_tmp78E=0,Cyc_Tcutil_terr(loc,((_tmp78F="expression may not be fully initialized",
_tag_dyneither(_tmp78F,sizeof(char),40))),_tag_dyneither(_tmp78E,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp3A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp36,env->all_changed,
r);if(_tmp36.t == _tmp3A.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp3B=Cyc_CfFlowInfo_ReachableFL(
_tmp3A,_tmp37);Cyc_NewControlFlow_update_tryflow(env,_tmp3B);return _tmp3B;};};
_LL1D:;}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_NewControlFlow_check_nounique(
struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){struct
_tuple0 _tmp790;struct _tuple0 _tmp3D=(_tmp790.f1=Cyc_Tcutil_compress(t),((_tmp790.f2=
r,_tmp790)));void*_tmp3E;enum Cyc_CfFlowInfo_InitLevel _tmp40;void*_tmp41;void*
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
if(!Cyc_Tcutil_is_noalias_pointer(t))goto _LL33;_LL32:{const char*_tmp793;void*
_tmp792;(_tmp792=0,Cyc_Tcutil_warn(loc,((_tmp793="argument may still contain unique pointers",
_tag_dyneither(_tmp793,sizeof(char),43))),_tag_dyneither(_tmp792,sizeof(void*),0)));}
return;_LL33:;_LL34: return;_LL22:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp6B=inflow;int _tmp6C;struct
_tuple13 _tmp6D;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*_tmp6F;_LL36: if((
_tmp6B.BottomFL).tag != 1)goto _LL38;_tmp6C=(int)(_tmp6B.BottomFL).val;_LL37:
return Cyc_CfFlowInfo_BottomFL();_LL38: if((_tmp6B.ReachableFL).tag != 2)goto _LL35;
_tmp6D=(struct _tuple13)(_tmp6B.ReachableFL).val;_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;
_LL39: if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp796;void*_tmp795;(
_tmp795=0,Cyc_Tcutil_terr(loc,((_tmp796="noliveunique attribute requires unique pointer",
_tag_dyneither(_tmp796,sizeof(char),47))),_tag_dyneither(_tmp795,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{void*_tmp72=Cyc_Tcutil_pointer_elt_type(t);{
void*_tmp73=r;struct Cyc_CfFlowInfo_Place*_tmp75;_LL3B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp74=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp73;if(_tmp74->tag != 5)goto
_LL3D;else{_tmp75=_tmp74->f1;}}_LL3C: Cyc_NewControlFlow_check_nounique(env,loc,
_tmp72,Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp75));goto _LL3A;_LL3D:;_LL3E: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,_tmp72)){const char*_tmp799;void*_tmp798;(_tmp798=0,Cyc_Tcutil_warn(
loc,((_tmp799="argument may contain live unique pointers",_tag_dyneither(_tmp799,
sizeof(char),42))),_tag_dyneither(_tmp798,sizeof(void*),0)));}return Cyc_NewControlFlow_use_Rval(
env,loc,inflow,r);_LL3A:;}{struct Cyc_Dict_Dict _tmp78=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp6E,env->all_changed,r);if(_tmp6E.t == _tmp78.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo _tmp79=Cyc_CfFlowInfo_ReachableFL(_tmp78,_tmp6F);Cyc_NewControlFlow_update_tryflow(
env,_tmp79);return _tmp79;};};};_LL35:;}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo
f1;struct Cyc_List_List*f2;};static struct _tuple20 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){if(es == 0){
struct _tuple20 _tmp79A;return(_tmp79A.f1=inflow,((_tmp79A.f2=0,_tmp79A)));}if(es->tl
== 0){union Cyc_CfFlowInfo_FlowInfo _tmp7C;void*_tmp7D;struct _tuple14 _tmp7B=Cyc_NewControlFlow_anal_Rexp(
env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp7C=_tmp7B.f1;_tmp7D=
_tmp7B.f2;{struct Cyc_List_List*_tmp79D;struct _tuple20 _tmp79C;return(_tmp79C.f1=
_tmp7C,((_tmp79C.f2=((_tmp79D=_region_malloc(rgn,sizeof(*_tmp79D)),((_tmp79D->hd=
_tmp7D,((_tmp79D->tl=0,_tmp79D)))))),_tmp79C)));};}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo
old_inflow;union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_List_List*rvals;do{
this_all_changed=(env->fenv)->mt_place_set;{struct Cyc_Dict_Dict*_tmp79E;env->all_changed=((
_tmp79E=_region_malloc(env->r,sizeof(*_tmp79E)),((_tmp79E[0]=(env->fenv)->mt_place_set,
_tmp79E))));}{union Cyc_CfFlowInfo_FlowInfo _tmp82;void*_tmp83;struct _tuple14
_tmp81=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;outflow=_tmp82;{struct Cyc_List_List*
_tmp79F;rvals=((_tmp79F=_region_malloc(rgn,sizeof(*_tmp79F)),((_tmp79F->hd=
_tmp83,((_tmp79F->tl=0,_tmp79F))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*
_tmp7A0;env->all_changed=((_tmp7A0=_region_malloc(env->r,sizeof(*_tmp7A0)),((
_tmp7A0[0]=(env->fenv)->mt_place_set,_tmp7A0))));}{union Cyc_CfFlowInfo_FlowInfo
_tmp87;void*_tmp88;struct _tuple14 _tmp86=Cyc_NewControlFlow_anal_Rexp(env,
others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp87=_tmp86.f1;_tmp88=
_tmp86.f2;{struct Cyc_List_List*_tmp7A1;rvals=((_tmp7A1=_region_malloc(rgn,
sizeof(*_tmp7A1)),((_tmp7A1->hd=_tmp88,((_tmp7A1->tl=rvals,_tmp7A1))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp87,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
old_inflow=inflow;inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,
inflow,outflow);};}while(!Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*_tmp7A2;env->all_changed=((
_tmp7A2=_region_malloc(env->r,sizeof(*_tmp7A2)),((_tmp7A2[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0),_tmp7A2))));}Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple20 _tmp7A3;return(_tmp7A3.f1=outflow,((_tmp7A3.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp7A3)));};};}
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
const char*_tmp7A6;void*_tmp7A5;(_tmp7A5=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp7A6="expression may not be initialized",
_tag_dyneither(_tmp7A6,sizeof(char),34))),_tag_dyneither(_tmp7A5,sizeof(void*),0)));}}
goto _LL3F;_LL42: if((_tmp90.BottomFL).tag != 1)goto _LL3F;_tmp93=(int)(_tmp90.BottomFL).val;
_LL43: goto _LL3F;_LL3F:;}{struct _tuple14 _tmp7A7;struct _tuple14 _tmp97=(_tmp7A7.f1=
_tmp8E,((_tmp7A7.f2=(env->fenv)->unknown_all,_tmp7A7)));_npop_handler(0);return
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
_tmpA1.BottomFL).val;_LL50: {struct _tuple16 _tmp7A8;return(_tmp7A8.f1=outflow,((
_tmp7A8.f2=outflow,_tmp7A8)));}_LL51: if((_tmpA1.ReachableFL).tag != 2)goto _LL4E;
_tmpA3=(struct _tuple13)(_tmpA1.ReachableFL).val;_tmpA4=_tmpA3.f1;_tmpA5=_tmpA3.f2;
_LL52: {union Cyc_CfFlowInfo_AbsLVal _tmpA7=(Cyc_NewControlFlow_anal_Lexp(env,
inflow,0,e)).f2;int _tmpA8;struct Cyc_CfFlowInfo_Place*_tmpA9;_LL54: if((_tmpA7.UnknownL).tag
!= 2)goto _LL56;_tmpA8=(int)(_tmpA7.UnknownL).val;_LL55: {struct _tuple16 _tmp7A9;
return(_tmp7A9.f1=outflow,((_tmp7A9.f2=outflow,_tmp7A9)));}_LL56: if((_tmpA7.PlaceL).tag
!= 1)goto _LL53;_tmpA9=(struct Cyc_CfFlowInfo_Place*)(_tmpA7.PlaceL).val;_LL57: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
struct _tuple16 _tmp7AA;return(_tmp7AA.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmpA4,env->all_changed,_tmpA9,nzval),_tmpA5),((_tmp7AA.f2=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA4,env->all_changed,_tmpA9,(env->fenv)->zero),
_tmpA5),_tmp7AA)));}_LL53:;}_LL4E:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple14 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
f,struct Cyc_Absyn_Exp*e,void*r){void*_tmpAE=Cyc_Tcutil_compress((void*)
_check_null(e->topt));struct Cyc_Absyn_PtrInfo _tmpB0;void*_tmpB1;struct Cyc_Absyn_PtrAtts
_tmpB2;union Cyc_Absyn_Constraint*_tmpB3;union Cyc_Absyn_Constraint*_tmpB4;_LL59: {
struct Cyc_Absyn_PointerType_struct*_tmpAF=(struct Cyc_Absyn_PointerType_struct*)
_tmpAE;if(_tmpAF->tag != 5)goto _LL5B;else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpB0.elt_typ;
_tmpB2=_tmpB0.ptr_atts;_tmpB3=_tmpB2.bounds;_tmpB4=_tmpB2.zero_term;}}_LL5A: {
union Cyc_CfFlowInfo_FlowInfo _tmpB5=f;int _tmpB6;struct _tuple13 _tmpB7;struct Cyc_Dict_Dict
_tmpB8;struct Cyc_List_List*_tmpB9;_LL5E: if((_tmpB5.BottomFL).tag != 1)goto _LL60;
_tmpB6=(int)(_tmpB5.BottomFL).val;_LL5F: {struct _tuple14 _tmp7AB;return(_tmp7AB.f1=
f,((_tmp7AB.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB1,0,(env->fenv)->unknown_all),
_tmp7AB)));}_LL60: if((_tmpB5.ReachableFL).tag != 2)goto _LL5D;_tmpB7=(struct
_tuple13)(_tmpB5.ReachableFL).val;_tmpB8=_tmpB7.f1;_tmpB9=_tmpB7.f2;_LL61: if(Cyc_Tcutil_is_bound_one(
_tmpB3)){void*_tmpBB=r;struct Cyc_CfFlowInfo_Place*_tmpBF;enum Cyc_CfFlowInfo_InitLevel
_tmpC2;_LL63: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmpBC=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmpBB;if(_tmpBC->tag != 1)goto _LL65;}_LL64: goto _LL66;_LL65: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmpBD=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmpBB;if(_tmpBD->tag != 2)goto
_LL67;}_LL66:{void*_tmpC3=e->annot;struct Cyc_List_List*_tmpC5;_LL70: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmpC4=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmpC3;if(_tmpC4->tag != Cyc_CfFlowInfo_NotZero)
goto _LL72;else{_tmpC5=_tmpC4->f1;}}_LL71: if(!Cyc_CfFlowInfo_same_relns(_tmpB9,
_tmpC5))goto _LL73;goto _LL6F;_LL72:;_LL73:{void*_tmpC6=e->r;_LL75: {struct Cyc_Absyn_Subscript_e_struct*
_tmpC7=(struct Cyc_Absyn_Subscript_e_struct*)_tmpC6;if(_tmpC7->tag != 22)goto _LL77;}
_LL76:{struct Cyc_CfFlowInfo_NotZero_struct _tmp7AE;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp7AD;e->annot=(void*)((_tmp7AD=_cycalloc(sizeof(*_tmp7AD)),((_tmp7AD[0]=((
_tmp7AE.tag=Cyc_CfFlowInfo_NotZero,((_tmp7AE.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB9),_tmp7AE)))),_tmp7AD))));}goto _LL74;_LL77:;_LL78: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL74;_LL74:;}goto _LL6F;_LL6F:;}goto _LL62;_LL67: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpBE=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpBB;if(_tmpBE->tag != 5)goto
_LL69;else{_tmpBF=_tmpBE->f1;}}_LL68:{void*_tmpCA=e->annot;struct Cyc_List_List*
_tmpCC;_LL7A: {struct Cyc_CfFlowInfo_NotZero_struct*_tmpCB=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpCA;if(_tmpCB->tag != Cyc_CfFlowInfo_NotZero)goto _LL7C;else{_tmpCC=_tmpCB->f1;}}
_LL7B: if(!Cyc_CfFlowInfo_same_relns(_tmpB9,_tmpCC))goto _LL7D;goto _LL79;_LL7C:;
_LL7D:{void*_tmpCD=e->r;_LL7F: {struct Cyc_Absyn_Subscript_e_struct*_tmpCE=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpCD;if(_tmpCE->tag != 22)goto _LL81;}_LL80:{
struct Cyc_CfFlowInfo_NotZero_struct _tmp7B1;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp7B0;e->annot=(void*)((_tmp7B0=_cycalloc(sizeof(*_tmp7B0)),((_tmp7B0[0]=((
_tmp7B1.tag=Cyc_CfFlowInfo_NotZero,((_tmp7B1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB9),_tmp7B1)))),_tmp7B0))));}goto _LL7E;_LL81:;_LL82: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL7E;_LL7E:;}goto _LL79;_LL79:;}{struct _tuple14 _tmp7B2;return(_tmp7B2.f1=f,((
_tmp7B2.f2=Cyc_CfFlowInfo_lookup_place(_tmpB8,_tmpBF),_tmp7B2)));};_LL69: {
struct Cyc_CfFlowInfo_Zero_struct*_tmpC0=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmpBB;if(_tmpC0->tag != 0)goto _LL6B;}_LL6A: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp7B3;return(_tmp7B3.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7B3.f2=
Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB1,0,(env->fenv)->unknown_all),
_tmp7B3)));};_LL6B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpC1=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpBB;if(_tmpC1->tag != 3)goto _LL6D;else{_tmpC2=_tmpC1->f1;}}_LL6C: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmpC2);goto _LL6E;_LL6D:;_LL6E:{void*_tmpD3=e->r;_LL84: {struct
Cyc_Absyn_Subscript_e_struct*_tmpD4=(struct Cyc_Absyn_Subscript_e_struct*)_tmpD3;
if(_tmpD4->tag != 22)goto _LL86;}_LL85:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp7B6;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp7B5;e->annot=(void*)((_tmp7B5=
_cycalloc(sizeof(*_tmp7B5)),((_tmp7B5[0]=((_tmp7B6.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp7B6.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpB9),_tmp7B6)))),
_tmp7B5))));}goto _LL83;_LL86:;_LL87: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL83;_LL83:;}goto _LL62;_LL62:;}else{void*_tmpD7=e->annot;struct Cyc_List_List*
_tmpD9;_LL89: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpD8=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmpD7;if(_tmpD8->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL8B;else{_tmpD9=_tmpD8->f1;}}
_LL8A: if(!Cyc_CfFlowInfo_same_relns(_tmpB9,_tmpD9))goto _LL8C;goto _LL88;_LL8B:;
_LL8C:{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp7B9;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp7B8;e->annot=(void*)((_tmp7B8=_cycalloc(sizeof(*_tmp7B8)),((_tmp7B8[0]=((
_tmp7B9.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7B9.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB9),_tmp7B9)))),_tmp7B8))));}goto _LL88;_LL88:;}switch(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmpB8,r)){case Cyc_CfFlowInfo_NoneIL: _LL8D:{void*_tmpDC=r;_LL90: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmpDD=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpDC;if(_tmpDD->tag != 7)goto _LL92;}_LL91:{const char*_tmp7BC;void*_tmp7BB;(
_tmp7BB=0,Cyc_Tcutil_terr(e->loc,((_tmp7BC="attempt to use a unique pointer that has already been copied",
_tag_dyneither(_tmp7BC,sizeof(char),61))),_tag_dyneither(_tmp7BB,sizeof(void*),0)));}
goto _LL8F;_LL92:;_LL93: {const char*_tmp7BF;void*_tmp7BE;(_tmp7BE=0,Cyc_Tcutil_terr(
e->loc,((_tmp7BF="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp7BF,sizeof(char),46))),_tag_dyneither(_tmp7BE,sizeof(void*),0)));}_LL8F:;}
goto _LL8E;case Cyc_CfFlowInfo_AllIL: _LL8E: {struct _tuple14 _tmp7C0;return(_tmp7C0.f1=
f,((_tmp7C0.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB1,0,(env->fenv)->unknown_all),
_tmp7C0)));}case Cyc_CfFlowInfo_ThisIL: _LL94: {struct _tuple14 _tmp7C1;return(
_tmp7C1.f1=f,((_tmp7C1.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB1,0,(env->fenv)->unknown_none),
_tmp7C1)));}}_LL5D:;}_LL5B:;_LL5C: {struct Cyc_Core_Impossible_struct _tmp7C7;
const char*_tmp7C6;struct Cyc_Core_Impossible_struct*_tmp7C5;(int)_throw((void*)((
_tmp7C5=_cycalloc(sizeof(*_tmp7C5)),((_tmp7C5[0]=((_tmp7C7.tag=Cyc_Core_Impossible,((
_tmp7C7.f1=((_tmp7C6="right deref of non-pointer-type",_tag_dyneither(_tmp7C6,
sizeof(char),32))),_tmp7C7)))),_tmp7C5)))));}_LL58:;}static struct Cyc_List_List*
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
_LLAF:;_LLB0: continue;_LLAC:;}}}if(!found){struct Cyc_CfFlowInfo_Reln*_tmp7CA;
struct Cyc_List_List*_tmp7C9;return(_tmp7C9=_region_malloc(rgn,sizeof(*_tmp7C9)),((
_tmp7C9->hd=((_tmp7CA=_region_malloc(rgn,sizeof(*_tmp7CA)),((_tmp7CA->vd=_tmp108,((
_tmp7CA->rop=Cyc_CfFlowInfo_LessNumelts(_tmpF7),_tmp7CA)))))),((_tmp7C9->tl=
relns,_tmp7C9)))));}}return relns;_LLAA:;_LLAB: return relns;_LLA1:;}else{return
relns;}_LL9F:;_LLA0: return relns;_LL96:;}static struct _tuple14 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,unsigned int loc){union Cyc_CfFlowInfo_FlowInfo _tmp10F=outflow;int
_tmp110;struct _tuple13 _tmp111;struct Cyc_Dict_Dict _tmp112;struct Cyc_List_List*
_tmp113;_LLB2: if((_tmp10F.BottomFL).tag != 1)goto _LLB4;_tmp110=(int)(_tmp10F.BottomFL).val;
_LLB3: {struct _tuple14 _tmp7CB;return(_tmp7CB.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp7CB.f2=rval,_tmp7CB)));}_LLB4: if((_tmp10F.ReachableFL).tag != 2)goto _LLB1;
_tmp111=(struct _tuple13)(_tmp10F.ReachableFL).val;_tmp112=_tmp111.f1;_tmp113=
_tmp111.f2;_LLB5: {union Cyc_CfFlowInfo_AbsLVal _tmp115=lval;struct Cyc_CfFlowInfo_Place*
_tmp116;int _tmp117;_LLB7: if((_tmp115.PlaceL).tag != 1)goto _LLB9;_tmp116=(struct
Cyc_CfFlowInfo_Place*)(_tmp115.PlaceL).val;_LLB8: {struct Cyc_Dict_Dict _tmp118=
Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp112,env->all_changed,_tmp116,rval);
_tmp113=Cyc_CfFlowInfo_reln_assign_exp(fenv->r,_tmp113,lexp,rexp);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp118,_tmp113);if(Cyc_NewControlFlow_warn_lose_unique  && Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,(void*)_check_null(lexp->topt))){void*_tmp119=Cyc_CfFlowInfo_lookup_place(
_tmp112,_tmp116);enum Cyc_CfFlowInfo_InitLevel _tmp11B;_LLBC: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp11A=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp119;if(_tmp11A->tag != 3)goto
_LLBE;else{_tmp11B=_tmp11A->f1;if(_tmp11B != Cyc_CfFlowInfo_NoneIL)goto _LLBE;}}
_LLBD: goto _LLBF;_LLBE: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp11C=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp119;if(_tmp11C->tag != 7)goto _LLC0;}_LLBF: goto _LLC1;_LLC0: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp11D=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp119;if(_tmp11D->tag != 0)goto _LLC2;}
_LLC1: goto _LLBB;_LLC2:;_LLC3:{const char*_tmp7CE;void*_tmp7CD;(_tmp7CD=0,Cyc_Tcutil_warn(
lexp->loc,((_tmp7CE="assignment may overwrite unique pointer(s)",_tag_dyneither(
_tmp7CE,sizeof(char),43))),_tag_dyneither(_tmp7CD,sizeof(void*),0)));}goto _LLBB;
_LLBB:;}Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple14 _tmp7CF;
return(_tmp7CF.f1=outflow,((_tmp7CF.f2=rval,_tmp7CF)));};}_LLB9: if((_tmp115.UnknownL).tag
!= 2)goto _LLB6;_tmp117=(int)(_tmp115.UnknownL).val;_LLBA: {struct _tuple14 _tmp7D0;
return(_tmp7D0.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((
_tmp7D0.f2=rval,_tmp7D0)));}_LLB6:;}_LLB1:;}static int Cyc_NewControlFlow_is_local_var_rooted_path(
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
_tmp122;if(_tmp12C->tag != 19)goto _LLCD;else{_tmp12D=_tmp12C->f1;}}_LLCC: _tmp12F=
_tmp12D;goto _LLCE;_LLCD: {struct Cyc_Absyn_AggrMember_e_struct*_tmp12E=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp122;if(_tmp12E->tag != 20)goto _LLCF;else{_tmp12F=_tmp12E->f1;}}_LLCE: _tmp131=
_tmp12F;goto _LLD0;_LLCF: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp130=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp122;if(_tmp130->tag != 21)goto _LLD1;else{_tmp131=_tmp130->f1;}}_LLD0: return
Cyc_NewControlFlow_is_local_var_rooted_path(_tmp131);_LLD1: {struct Cyc_Absyn_Subscript_e_struct*
_tmp132=(struct Cyc_Absyn_Subscript_e_struct*)_tmp122;if(_tmp132->tag != 22)goto
_LLD3;else{_tmp133=_tmp132->f1;}}_LLD2: {void*_tmp136=Cyc_Tcutil_compress((void*)
_check_null(_tmp133->topt));_LLD8: {struct Cyc_Absyn_TupleType_struct*_tmp137=(
struct Cyc_Absyn_TupleType_struct*)_tmp136;if(_tmp137->tag != 10)goto _LLDA;}_LLD9:
return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp133);_LLDA:;_LLDB: return 0;
_LLD7:;}_LLD3: {struct Cyc_Absyn_Cast_e_struct*_tmp134=(struct Cyc_Absyn_Cast_e_struct*)
_tmp122;if(_tmp134->tag != 13)goto _LLD5;else{_tmp135=_tmp134->f2;}}_LLD4: return
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
_tmp14E=_tmp14D.f1;_tmp14F=_tmp14D.f2;{struct _tuple15 _tmp7D1;struct _tuple15
_tmp151=(_tmp7D1.f1=_tmp14E,((_tmp7D1.f2=_tmp14F,_tmp7D1)));union Cyc_CfFlowInfo_FlowInfo
_tmp152;struct _tuple13 _tmp153;struct Cyc_Dict_Dict _tmp154;struct Cyc_List_List*
_tmp155;union Cyc_CfFlowInfo_AbsLVal _tmp156;struct Cyc_CfFlowInfo_Place*_tmp157;
union Cyc_CfFlowInfo_FlowInfo _tmp158;int _tmp159;_LLE8: _tmp152=_tmp151.f1;if((
_tmp152.ReachableFL).tag != 2)goto _LLEA;_tmp153=(struct _tuple13)(_tmp152.ReachableFL).val;
_tmp154=_tmp153.f1;_tmp155=_tmp153.f2;_tmp156=_tmp151.f2;if((_tmp156.PlaceL).tag
!= 1)goto _LLEA;_tmp157=(struct Cyc_CfFlowInfo_Place*)(_tmp156.PlaceL).val;_LLE9: {
void*_tmp15A=Cyc_CfFlowInfo_lookup_place(_tmp154,_tmp157);void*t=(void*)
_check_null(_tmp144->topt);{void*_tmp15B=_tmp144->r;struct Cyc_Absyn_Exp*_tmp15D;
_LLEF: {struct Cyc_Absyn_Cast_e_struct*_tmp15C=(struct Cyc_Absyn_Cast_e_struct*)
_tmp15B;if(_tmp15C->tag != 13)goto _LLF1;else{_tmp15D=_tmp15C->f2;}}_LLF0: t=(void*)
_check_null(_tmp15D->topt);goto _LLEE;_LLF1:;_LLF2: goto _LLEE;_LLEE:;}{void*
_tmp15E=Cyc_CfFlowInfo_make_unique_consumed(_tmp138,t,_tmp144,env->iteration_num,
_tmp15A);_tmp154=Cyc_CfFlowInfo_assign_place(_tmp138,_tmp144->loc,_tmp154,env->all_changed,
_tmp157,_tmp15E);_tmp14E=Cyc_CfFlowInfo_ReachableFL(_tmp154,_tmp155);{void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp138,_tmp154,_tmp15A)){case Cyc_CfFlowInfo_AllIL:
_LLF3: leaf=_tmp138->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LLF4: leaf=
_tmp138->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LLF5: leaf=_tmp138->unknown_this;
break;}{void*_tmp15F=Cyc_CfFlowInfo_typ_to_absrval(_tmp138,t,0,leaf);_tmp14E=Cyc_NewControlFlow_use_Rval(
env,_tmp144->loc,_tmp14E,_tmp15A);{struct Cyc_List_List _tmp7D2;struct Cyc_List_List
_tmp160=(_tmp7D2.hd=_tmp146,((_tmp7D2.tl=0,_tmp7D2)));_tmp14E=Cyc_NewControlFlow_add_vars(
_tmp138,_tmp14E,(struct Cyc_List_List*)& _tmp160,_tmp138->unknown_all,s->loc);{
union Cyc_CfFlowInfo_FlowInfo _tmp162;void*_tmp163;struct _tuple14 _tmp161=Cyc_NewControlFlow_anal_stmt_exp(
env,copy_ctxt,_tmp14E,_tmp147);_tmp162=_tmp161.f1;_tmp163=_tmp161.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp164=_tmp162;struct _tuple13 _tmp165;struct Cyc_Dict_Dict _tmp166;struct Cyc_List_List*
_tmp167;_LLF8: if((_tmp164.ReachableFL).tag != 2)goto _LLFA;_tmp165=(struct _tuple13)(
_tmp164.ReachableFL).val;_tmp166=_tmp165.f1;_tmp167=_tmp165.f2;_LLF9: _tmp166=Cyc_CfFlowInfo_assign_place(
_tmp138,s->loc,_tmp166,env->all_changed,_tmp157,_tmp15F);{union Cyc_CfFlowInfo_FlowInfo
_tmp168=Cyc_CfFlowInfo_ReachableFL(_tmp166,_tmp167);struct _tuple14 _tmp7D3;return(
_tmp7D3.f1=_tmp168,((_tmp7D3.f2=_tmp163,_tmp7D3)));};_LLFA:;_LLFB: {struct
_tuple14 _tmp7D4;return(_tmp7D4.f1=_tmp162,((_tmp7D4.f2=_tmp163,_tmp7D4)));}_LLF7:;};};};};};};}
_LLEA: _tmp158=_tmp151.f1;if((_tmp158.BottomFL).tag != 1)goto _LLEC;_tmp159=(int)(
_tmp158.BottomFL).val;_LLEB: {struct _tuple14 _tmp7D5;return(_tmp7D5.f1=_tmp14E,((
_tmp7D5.f2=_tmp138->unknown_all,_tmp7D5)));}_LLEC:;_LLED:{const char*_tmp7D8;void*
_tmp7D7;(_tmp7D7=0,Cyc_Tcutil_terr(_tmp144->loc,((_tmp7D8="bad alias expression--no unique path found",
_tag_dyneither(_tmp7D8,sizeof(char),43))),_tag_dyneither(_tmp7D7,sizeof(void*),0)));}{
struct _tuple14 _tmp7D9;return(_tmp7D9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7D9.f2=
_tmp138->unknown_all,_tmp7D9)));};_LLE7:;};}_LLE1: {struct Cyc_Absyn_Decl_s_struct*
_tmp148=(struct Cyc_Absyn_Decl_s_struct*)_tmp13B;if(_tmp148->tag != 12)goto _LLE3;
else{_tmp149=_tmp148->f1;_tmp14A=_tmp148->f2;}}_LLE2: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp149);s=_tmp14A;goto _LLDC;_LLE3: {struct Cyc_Absyn_Exp_s_struct*
_tmp14B=(struct Cyc_Absyn_Exp_s_struct*)_tmp13B;if(_tmp14B->tag != 1)goto _LLE5;
else{_tmp14C=_tmp14B->f1;}}_LLE4: return Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp14C);_LLE5:;_LLE6: {struct Cyc_Core_Impossible_struct _tmp7DF;
const char*_tmp7DE;struct Cyc_Core_Impossible_struct*_tmp7DD;(int)_throw((void*)((
_tmp7DD=_cycalloc(sizeof(*_tmp7DD)),((_tmp7DD[0]=((_tmp7DF.tag=Cyc_Core_Impossible,((
_tmp7DF.f1=((_tmp7DE="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp7DE,
sizeof(char),33))),_tmp7DF)))),_tmp7DD)))));}_LLDC:;};}}struct _tuple21{union Cyc_CfFlowInfo_AbsLVal
f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple22{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);
static void _tmp880(unsigned int*_tmp87F,unsigned int*_tmp87E,void***_tmp87C,
struct Cyc_List_List**_tmp361){for(*_tmp87F=0;*_tmp87F < *_tmp87E;(*_tmp87F)++){
void*_tmp87A;(*_tmp87C)[*_tmp87F]=((_tmp87A=(void*)((struct Cyc_List_List*)
_check_null(*_tmp361))->hd,((*_tmp361=(*_tmp361)->tl,_tmp87A))));}}static struct
_tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp173=env->fenv;struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;{union Cyc_CfFlowInfo_FlowInfo
_tmp174=inflow;int _tmp175;struct _tuple13 _tmp176;struct Cyc_Dict_Dict _tmp177;
struct Cyc_List_List*_tmp178;_LLFD: if((_tmp174.BottomFL).tag != 1)goto _LLFF;
_tmp175=(int)(_tmp174.BottomFL).val;_LLFE: {struct _tuple14 _tmp7E0;return(_tmp7E0.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp7E0.f2=_tmp173->unknown_all,_tmp7E0)));}_LLFF:
if((_tmp174.ReachableFL).tag != 2)goto _LLFC;_tmp176=(struct _tuple13)(_tmp174.ReachableFL).val;
_tmp177=_tmp176.f1;_tmp178=_tmp176.f2;_LL100: d=_tmp177;relns=_tmp178;_LLFC:;}if((
copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)_check_null(e->topt))){union Cyc_CfFlowInfo_FlowInfo _tmp17B;union
Cyc_CfFlowInfo_AbsLVal _tmp17C;struct _tuple15 _tmp17A=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,e);_tmp17B=_tmp17A.f1;_tmp17C=_tmp17A.f2;{struct _tuple15 _tmp7E1;
struct _tuple15 _tmp17E=(_tmp7E1.f1=_tmp17B,((_tmp7E1.f2=_tmp17C,_tmp7E1)));union
Cyc_CfFlowInfo_FlowInfo _tmp17F;struct _tuple13 _tmp180;struct Cyc_Dict_Dict _tmp181;
struct Cyc_List_List*_tmp182;union Cyc_CfFlowInfo_AbsLVal _tmp183;struct Cyc_CfFlowInfo_Place*
_tmp184;_LL102: _tmp17F=_tmp17E.f1;if((_tmp17F.ReachableFL).tag != 2)goto _LL104;
_tmp180=(struct _tuple13)(_tmp17F.ReachableFL).val;_tmp181=_tmp180.f1;_tmp182=
_tmp180.f2;_tmp183=_tmp17E.f2;if((_tmp183.PlaceL).tag != 1)goto _LL104;_tmp184=(
struct Cyc_CfFlowInfo_Place*)(_tmp183.PlaceL).val;_LL103: {void*_tmp185=Cyc_CfFlowInfo_lookup_place(
_tmp181,_tmp184);int needs_unconsume=0;if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,
_tmp185,& needs_unconsume)){{const char*_tmp7E4;void*_tmp7E3;(_tmp7E3=0,Cyc_Tcutil_terr(
e->loc,((_tmp7E4="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp7E4,sizeof(char),71))),_tag_dyneither(_tmp7E3,sizeof(void*),0)));}{
struct _tuple14 _tmp7E5;return(_tmp7E5.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7E5.f2=
_tmp173->unknown_all,_tmp7E5)));};}else{if(needs_unconsume){struct _tuple14
_tmp7E6;return(_tmp7E6.f1=_tmp17B,((_tmp7E6.f2=Cyc_CfFlowInfo_make_unique_unconsumed(
_tmp173,_tmp185),_tmp7E6)));}else{void*_tmp18A=Cyc_CfFlowInfo_make_unique_consumed(
_tmp173,(void*)_check_null(e->topt),e,env->iteration_num,_tmp185);struct Cyc_Dict_Dict
_tmp18B=Cyc_CfFlowInfo_assign_place(_tmp173,e->loc,_tmp181,env->all_changed,
_tmp184,_tmp18A);struct _tuple14 _tmp7E7;return(_tmp7E7.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp18B,_tmp182),((_tmp7E7.f2=_tmp185,_tmp7E7)));}}}_LL104:;_LL105: goto _LL101;
_LL101:;};}{void*_tmp18D=e->r;struct Cyc_Absyn_Exp*_tmp18F;enum Cyc_Absyn_Coercion
_tmp190;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*
_tmp196;union Cyc_Absyn_Cnst _tmp198;int _tmp199;union Cyc_Absyn_Cnst _tmp19B;struct
_tuple6 _tmp19C;int _tmp19D;union Cyc_Absyn_Cnst _tmp19F;struct _tuple6 _tmp1A0;void*
_tmp1A2;struct Cyc_List_List*_tmp1A5;void*_tmp1AD;void*_tmp1B0;struct Cyc_Absyn_Vardecl*
_tmp1B2;void*_tmp1B4;struct Cyc_Absyn_Vardecl*_tmp1B6;void*_tmp1B8;struct Cyc_Absyn_Vardecl*
_tmp1BA;enum Cyc_Absyn_Primop _tmp1BC;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_Exp*
_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Core_Opt*_tmp1C2;struct Cyc_Core_Opt
_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_Core_Opt*
_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1CA;struct Cyc_Absyn_Exp*
_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CF;struct Cyc_List_List*
_tmp1D0;struct Cyc_Absyn_MallocInfo _tmp1D2;int _tmp1D3;struct Cyc_Absyn_Exp*_tmp1D4;
void**_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D6;int _tmp1D7;struct Cyc_Absyn_Exp*_tmp1D9;
struct Cyc_Absyn_Exp*_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DC;struct Cyc_Absyn_Exp*
_tmp1DD;struct Cyc_Absyn_Exp*_tmp1DF;struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_Absyn_Exp*
_tmp1E3;struct _dyneither_ptr*_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E6;struct
_dyneither_ptr*_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E9;struct _dyneither_ptr*_tmp1EA;
struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Absyn_Exp*
_tmp1EE;struct Cyc_Absyn_Exp*_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_Absyn_Exp*
_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F4;struct Cyc_Absyn_Exp*_tmp1F6;struct Cyc_Absyn_Exp*
_tmp1F7;struct Cyc_List_List*_tmp1F9;struct Cyc_Absyn_Datatypedecl*_tmp1FA;struct
Cyc_List_List*_tmp1FC;struct Cyc_List_List*_tmp1FE;struct Cyc_List_List*_tmp200;
struct Cyc_Absyn_Aggrdecl*_tmp201;struct Cyc_Absyn_Aggrdecl _tmp202;enum Cyc_Absyn_AggrKind
_tmp203;struct Cyc_Absyn_AggrdeclImpl*_tmp204;struct Cyc_Absyn_AggrdeclImpl _tmp205;
struct Cyc_List_List*_tmp206;struct Cyc_List_List*_tmp209;struct Cyc_Absyn_Vardecl*
_tmp20B;struct Cyc_Absyn_Exp*_tmp20C;struct Cyc_Absyn_Exp*_tmp20D;int _tmp20E;
struct Cyc_Absyn_Stmt*_tmp210;void*_tmp212;_LL107: {struct Cyc_Absyn_Cast_e_struct*
_tmp18E=(struct Cyc_Absyn_Cast_e_struct*)_tmp18D;if(_tmp18E->tag != 13)goto _LL109;
else{_tmp18F=_tmp18E->f2;_tmp190=_tmp18E->f4;if(_tmp190 != Cyc_Absyn_NonNull_to_Null)
goto _LL109;}}_LL108: {union Cyc_CfFlowInfo_FlowInfo _tmp219;void*_tmp21A;struct
_tuple14 _tmp218=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp18F);
_tmp219=_tmp218.f1;_tmp21A=_tmp218.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp21C;void*
_tmp21D;struct _tuple14 _tmp21B=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp219,
_tmp18F,_tmp21A);_tmp21C=_tmp21B.f1;_tmp21D=_tmp21B.f2;{struct _tuple14 _tmp7E8;
return(_tmp7E8.f1=_tmp21C,((_tmp7E8.f2=_tmp21A,_tmp7E8)));};};}_LL109: {struct
Cyc_Absyn_Cast_e_struct*_tmp191=(struct Cyc_Absyn_Cast_e_struct*)_tmp18D;if(
_tmp191->tag != 13)goto _LL10B;else{_tmp192=_tmp191->f2;}}_LL10A: _tmp194=_tmp192;
goto _LL10C;_LL10B: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp193=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp18D;if(_tmp193->tag != 11)goto _LL10D;else{_tmp194=_tmp193->f1;}}_LL10C:
_tmp196=_tmp194;goto _LL10E;_LL10D: {struct Cyc_Absyn_Instantiate_e_struct*_tmp195=(
struct Cyc_Absyn_Instantiate_e_struct*)_tmp18D;if(_tmp195->tag != 12)goto _LL10F;
else{_tmp196=_tmp195->f1;}}_LL10E: return Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp196);_LL10F: {struct Cyc_Absyn_Const_e_struct*_tmp197=(
struct Cyc_Absyn_Const_e_struct*)_tmp18D;if(_tmp197->tag != 0)goto _LL111;else{
_tmp198=_tmp197->f1;if((_tmp198.Null_c).tag != 1)goto _LL111;_tmp199=(int)(_tmp198.Null_c).val;}}
_LL110: goto _LL112;_LL111: {struct Cyc_Absyn_Const_e_struct*_tmp19A=(struct Cyc_Absyn_Const_e_struct*)
_tmp18D;if(_tmp19A->tag != 0)goto _LL113;else{_tmp19B=_tmp19A->f1;if((_tmp19B.Int_c).tag
!= 5)goto _LL113;_tmp19C=(struct _tuple6)(_tmp19B.Int_c).val;_tmp19D=_tmp19C.f2;
if(_tmp19D != 0)goto _LL113;}}_LL112: {struct _tuple14 _tmp7E9;return(_tmp7E9.f1=
inflow,((_tmp7E9.f2=_tmp173->zero,_tmp7E9)));}_LL113: {struct Cyc_Absyn_Const_e_struct*
_tmp19E=(struct Cyc_Absyn_Const_e_struct*)_tmp18D;if(_tmp19E->tag != 0)goto _LL115;
else{_tmp19F=_tmp19E->f1;if((_tmp19F.Int_c).tag != 5)goto _LL115;_tmp1A0=(struct
_tuple6)(_tmp19F.Int_c).val;}}_LL114: goto _LL116;_LL115: {struct Cyc_Absyn_Var_e_struct*
_tmp1A1=(struct Cyc_Absyn_Var_e_struct*)_tmp18D;if(_tmp1A1->tag != 1)goto _LL117;
else{_tmp1A2=(void*)_tmp1A1->f2;{struct Cyc_Absyn_Funname_b_struct*_tmp1A3=(
struct Cyc_Absyn_Funname_b_struct*)_tmp1A2;if(_tmp1A3->tag != 2)goto _LL117;};}}
_LL116: {struct _tuple14 _tmp7EA;return(_tmp7EA.f1=inflow,((_tmp7EA.f2=_tmp173->notzeroall,
_tmp7EA)));}_LL117: {struct Cyc_Absyn_Datatype_e_struct*_tmp1A4=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp18D;if(_tmp1A4->tag != 29)goto _LL119;else{_tmp1A5=_tmp1A4->f1;if(_tmp1A5 != 0)
goto _LL119;}}_LL118: goto _LL11A;_LL119: {struct Cyc_Absyn_Const_e_struct*_tmp1A6=(
struct Cyc_Absyn_Const_e_struct*)_tmp18D;if(_tmp1A6->tag != 0)goto _LL11B;}_LL11A:
goto _LL11C;_LL11B: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp1A7=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp18D;if(_tmp1A7->tag != 17)goto _LL11D;}_LL11C: goto _LL11E;_LL11D: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp1A8=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp18D;if(_tmp1A8->tag != 16)goto
_LL11F;}_LL11E: goto _LL120;_LL11F: {struct Cyc_Absyn_Offsetof_e_struct*_tmp1A9=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp18D;if(_tmp1A9->tag != 18)goto _LL121;}
_LL120: goto _LL122;_LL121: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp1AA=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp18D;if(_tmp1AA->tag != 31)goto _LL123;}_LL122: goto _LL124;_LL123: {struct Cyc_Absyn_Enum_e_struct*
_tmp1AB=(struct Cyc_Absyn_Enum_e_struct*)_tmp18D;if(_tmp1AB->tag != 30)goto _LL125;}
_LL124: {struct _tuple14 _tmp7EB;return(_tmp7EB.f1=inflow,((_tmp7EB.f2=_tmp173->unknown_all,
_tmp7EB)));}_LL125: {struct Cyc_Absyn_Var_e_struct*_tmp1AC=(struct Cyc_Absyn_Var_e_struct*)
_tmp18D;if(_tmp1AC->tag != 1)goto _LL127;else{_tmp1AD=(void*)_tmp1AC->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp1AE=(struct Cyc_Absyn_Global_b_struct*)_tmp1AD;if(
_tmp1AE->tag != 1)goto _LL127;};}}_LL126: {struct _tuple14 _tmp7EC;return(_tmp7EC.f1=
inflow,((_tmp7EC.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)_check_null(e->topt),
0,_tmp173->unknown_all),_tmp7EC)));}_LL127: {struct Cyc_Absyn_Var_e_struct*
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
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp7F2;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp7F1;struct _tuple14 _tmp7F0;return(_tmp7F0.f1=inflow,((_tmp7F0.f2=((void*(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,(void*)((_tmp7F2=_region_malloc(
env->r,sizeof(*_tmp7F2)),((_tmp7F2[0]=((_tmp7F1.tag=0,((_tmp7F1.f1=_tmp1BA,
_tmp7F1)))),_tmp7F2))))),_tmp7F0)));}_LL12D: {struct Cyc_Absyn_Primop_e_struct*
_tmp1BB=(struct Cyc_Absyn_Primop_e_struct*)_tmp18D;if(_tmp1BB->tag != 2)goto _LL12F;
else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BB->f2;}}_LL12E: {union Cyc_CfFlowInfo_FlowInfo
_tmp227;void*_tmp228;struct _tuple14 _tmp226=Cyc_NewControlFlow_anal_use_ints(env,
inflow,_tmp1BD);_tmp227=_tmp226.f1;_tmp228=_tmp226.f2;{struct _tuple14 _tmp7F3;
return(_tmp7F3.f1=_tmp227,((_tmp7F3.f2=_tmp228,_tmp7F3)));};}_LL12F: {struct Cyc_Absyn_Increment_e_struct*
_tmp1BE=(struct Cyc_Absyn_Increment_e_struct*)_tmp18D;if(_tmp1BE->tag != 4)goto
_LL131;else{_tmp1BF=_tmp1BE->f1;}}_LL130: {struct Cyc_List_List _tmp7F4;struct Cyc_List_List
_tmp22A=(_tmp7F4.hd=_tmp1BF,((_tmp7F4.tl=0,_tmp7F4)));union Cyc_CfFlowInfo_FlowInfo
_tmp22C;struct _tuple14 _tmp22B=Cyc_NewControlFlow_anal_use_ints(env,inflow,(
struct Cyc_List_List*)& _tmp22A);_tmp22C=_tmp22B.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp22E;struct _tuple15 _tmp22D=Cyc_NewControlFlow_anal_Lexp(env,_tmp22C,0,_tmp1BF);
_tmp22E=_tmp22D.f2;{struct _tuple21 _tmp7F5;struct _tuple21 _tmp230=(_tmp7F5.f1=
_tmp22E,((_tmp7F5.f2=_tmp22C,_tmp7F5)));union Cyc_CfFlowInfo_AbsLVal _tmp231;
struct Cyc_CfFlowInfo_Place*_tmp232;union Cyc_CfFlowInfo_FlowInfo _tmp233;struct
_tuple13 _tmp234;struct Cyc_Dict_Dict _tmp235;struct Cyc_List_List*_tmp236;_LL16E:
_tmp231=_tmp230.f1;if((_tmp231.PlaceL).tag != 1)goto _LL170;_tmp232=(struct Cyc_CfFlowInfo_Place*)(
_tmp231.PlaceL).val;_tmp233=_tmp230.f2;if((_tmp233.ReachableFL).tag != 2)goto
_LL170;_tmp234=(struct _tuple13)(_tmp233.ReachableFL).val;_tmp235=_tmp234.f1;
_tmp236=_tmp234.f2;_LL16F: _tmp236=Cyc_CfFlowInfo_reln_kill_exp(_tmp173->r,
_tmp236,_tmp1BF);_tmp22C=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
_tmp173,_tmp1BF->loc,_tmp235,env->all_changed,_tmp232,_tmp173->unknown_all),
_tmp236);goto _LL16D;_LL170:;_LL171: goto _LL16D;_LL16D:;}{struct _tuple14 _tmp7F6;
return(_tmp7F6.f1=_tmp22C,((_tmp7F6.f2=_tmp173->unknown_all,_tmp7F6)));};};}
_LL131: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1C0=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp18D;if(_tmp1C0->tag != 3)goto _LL133;else{_tmp1C1=_tmp1C0->f1;_tmp1C2=_tmp1C0->f2;
if(_tmp1C2 == 0)goto _LL133;_tmp1C3=*_tmp1C2;_tmp1C4=_tmp1C0->f3;}}_LL132: if(
copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){{
const char*_tmp7F9;void*_tmp7F8;(_tmp7F8=0,Cyc_Tcutil_terr(e->loc,((_tmp7F9="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7F9,sizeof(char),58))),_tag_dyneither(_tmp7F8,sizeof(void*),0)));}{
struct _tuple14 _tmp7FA;return(_tmp7FA.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7FA.f2=
_tmp173->unknown_all,_tmp7FA)));};}{struct Cyc_List_List _tmp7FB;struct Cyc_List_List
_tmp23C=(_tmp7FB.hd=_tmp1C4,((_tmp7FB.tl=0,_tmp7FB)));struct Cyc_List_List _tmp7FC;
struct Cyc_List_List _tmp23D=(_tmp7FC.hd=_tmp1C1,((_tmp7FC.tl=(struct Cyc_List_List*)&
_tmp23C,_tmp7FC)));union Cyc_CfFlowInfo_FlowInfo _tmp23F;struct _tuple14 _tmp23E=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp23D);_tmp23F=_tmp23E.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp241;struct _tuple15 _tmp240=Cyc_NewControlFlow_anal_Lexp(env,_tmp23F,0,_tmp1C1);
_tmp241=_tmp240.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp242=_tmp23F;struct _tuple13
_tmp243;struct Cyc_Dict_Dict _tmp244;struct Cyc_List_List*_tmp245;_LL173: if((
_tmp242.ReachableFL).tag != 2)goto _LL175;_tmp243=(struct _tuple13)(_tmp242.ReachableFL).val;
_tmp244=_tmp243.f1;_tmp245=_tmp243.f2;_LL174:{union Cyc_CfFlowInfo_AbsLVal _tmp246=
_tmp241;struct Cyc_CfFlowInfo_Place*_tmp247;int _tmp248;_LL178: if((_tmp246.PlaceL).tag
!= 1)goto _LL17A;_tmp247=(struct Cyc_CfFlowInfo_Place*)(_tmp246.PlaceL).val;_LL179:
_tmp245=Cyc_CfFlowInfo_reln_kill_exp(_tmp173->r,_tmp245,_tmp1C1);_tmp244=Cyc_CfFlowInfo_assign_place(
_tmp173,_tmp1C1->loc,_tmp244,env->all_changed,_tmp247,_tmp173->unknown_all);
_tmp23F=Cyc_CfFlowInfo_ReachableFL(_tmp244,_tmp245);goto _LL177;_LL17A: if((
_tmp246.UnknownL).tag != 2)goto _LL177;_tmp248=(int)(_tmp246.UnknownL).val;_LL17B:
goto _LL177;_LL177:;}goto _LL172;_LL175:;_LL176: goto _LL172;_LL172:;}{struct
_tuple14 _tmp7FD;return(_tmp7FD.f1=_tmp23F,((_tmp7FD.f2=_tmp173->unknown_all,
_tmp7FD)));};};};_LL133: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1C5=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp18D;if(_tmp1C5->tag != 3)goto _LL135;else{_tmp1C6=_tmp1C5->f1;_tmp1C7=_tmp1C5->f2;
if(_tmp1C7 != 0)goto _LL135;_tmp1C8=_tmp1C5->f3;}}_LL134: if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)_check_null(e->topt))){{const char*_tmp800;void*_tmp7FF;(_tmp7FF=0,
Cyc_Tcutil_terr(e->loc,((_tmp800="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp800,sizeof(char),58))),_tag_dyneither(_tmp7FF,sizeof(void*),0)));}{
struct _tuple14 _tmp801;return(_tmp801.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp801.f2=
_tmp173->unknown_all,_tmp801)));};}{struct Cyc_Dict_Dict*_tmp24F=env->all_changed;
while(1){{struct Cyc_Dict_Dict*_tmp802;env->all_changed=((_tmp802=_region_malloc(
env->r,sizeof(*_tmp802)),((_tmp802[0]=_tmp173->mt_place_set,_tmp802))));}{union
Cyc_CfFlowInfo_FlowInfo _tmp252;union Cyc_CfFlowInfo_AbsLVal _tmp253;struct _tuple15
_tmp251=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1C6);_tmp252=_tmp251.f1;
_tmp253=_tmp251.f2;{struct Cyc_Dict_Dict _tmp254=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp803;env->all_changed=((
_tmp803=_region_malloc(env->r,sizeof(*_tmp803)),((_tmp803[0]=_tmp173->mt_place_set,
_tmp803))));}{union Cyc_CfFlowInfo_FlowInfo _tmp257;void*_tmp258;struct _tuple14
_tmp256=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C8);_tmp257=_tmp256.f1;
_tmp258=_tmp256.f2;{struct Cyc_Dict_Dict _tmp259=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo _tmp25A=Cyc_CfFlowInfo_after_flow(
_tmp173,(struct Cyc_Dict_Dict*)& _tmp254,_tmp252,_tmp257,_tmp254,_tmp259);union Cyc_CfFlowInfo_FlowInfo
_tmp25B=Cyc_CfFlowInfo_join_flow(_tmp173,_tmp24F,inflow,_tmp25A);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp25B,inflow)){if(_tmp24F == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp24F,Cyc_CfFlowInfo_union_place_set(
_tmp254,_tmp259,0),0);}return Cyc_NewControlFlow_do_assign(_tmp173,env,_tmp25A,
_tmp1C6,_tmp253,_tmp1C8,_tmp258,e->loc);}inflow=_tmp25B;};};};};}};_LL135: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp1C9=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp18D;if(_tmp1C9->tag != 8)goto _LL137;else{_tmp1CA=_tmp1C9->f1;_tmp1CB=_tmp1C9->f2;}}
_LL136: {union Cyc_CfFlowInfo_FlowInfo _tmp25D;void*_tmp25E;struct _tuple14 _tmp25C=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1CA);_tmp25D=_tmp25C.f1;_tmp25E=
_tmp25C.f2;return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp25D,_tmp1CB);}
_LL137: {struct Cyc_Absyn_Throw_e_struct*_tmp1CC=(struct Cyc_Absyn_Throw_e_struct*)
_tmp18D;if(_tmp1CC->tag != 10)goto _LL139;else{_tmp1CD=_tmp1CC->f1;}}_LL138: {
union Cyc_CfFlowInfo_FlowInfo _tmp260;void*_tmp261;struct _tuple14 _tmp25F=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1CD);_tmp260=_tmp25F.f1;_tmp261=_tmp25F.f2;Cyc_NewControlFlow_use_Rval(
env,_tmp1CD->loc,_tmp260,_tmp261);{struct _tuple14 _tmp804;return(_tmp804.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp804.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)_check_null(e->topt),0,
_tmp173->unknown_all),_tmp804)));};}_LL139: {struct Cyc_Absyn_FnCall_e_struct*
_tmp1CE=(struct Cyc_Absyn_FnCall_e_struct*)_tmp18D;if(_tmp1CE->tag != 9)goto _LL13B;
else{_tmp1CF=_tmp1CE->f1;_tmp1D0=_tmp1CE->f2;}}_LL13A: {struct _RegionHandle
_tmp263=_new_region("temp");struct _RegionHandle*temp=& _tmp263;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp266;struct Cyc_List_List*_tmp267;struct Cyc_List_List*
_tmp805;struct _tuple20 _tmp265=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,((_tmp805=_region_malloc(temp,sizeof(*_tmp805)),((_tmp805->hd=_tmp1CF,((
_tmp805->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))
Cyc_List_rcopy)(temp,_tmp1D0),_tmp805)))))),0,1);_tmp266=_tmp265.f1;_tmp267=
_tmp265.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp268=Cyc_NewControlFlow_use_Rval(env,
_tmp1CF->loc,_tmp266,(void*)((struct Cyc_List_List*)_check_null(_tmp267))->hd);
_tmp267=_tmp267->tl;{struct Cyc_List_List*init_params=0;struct Cyc_List_List*
nolive_unique_params=0;{void*_tmp269=Cyc_Tcutil_compress((void*)_check_null(
_tmp1CF->topt));struct Cyc_Absyn_PtrInfo _tmp26B;void*_tmp26C;_LL17D: {struct Cyc_Absyn_PointerType_struct*
_tmp26A=(struct Cyc_Absyn_PointerType_struct*)_tmp269;if(_tmp26A->tag != 5)goto
_LL17F;else{_tmp26B=_tmp26A->f1;_tmp26C=_tmp26B.elt_typ;}}_LL17E:{void*_tmp26D=
Cyc_Tcutil_compress(_tmp26C);struct Cyc_Absyn_FnInfo _tmp26F;struct Cyc_List_List*
_tmp270;_LL182: {struct Cyc_Absyn_FnType_struct*_tmp26E=(struct Cyc_Absyn_FnType_struct*)
_tmp26D;if(_tmp26E->tag != 9)goto _LL184;else{_tmp26F=_tmp26E->f1;_tmp270=_tmp26F.attributes;}}
_LL183: for(0;_tmp270 != 0;_tmp270=_tmp270->tl){void*_tmp271=(void*)_tmp270->hd;
int _tmp273;int _tmp275;_LL187: {struct Cyc_Absyn_Initializes_att_struct*_tmp272=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp271;if(_tmp272->tag != 20)goto _LL189;
else{_tmp273=_tmp272->f1;}}_LL188:{struct Cyc_List_List*_tmp806;init_params=((
_tmp806=_region_malloc(temp,sizeof(*_tmp806)),((_tmp806->hd=(void*)_tmp273,((
_tmp806->tl=init_params,_tmp806))))));}goto _LL186;_LL189: {struct Cyc_Absyn_Noliveunique_att_struct*
_tmp274=(struct Cyc_Absyn_Noliveunique_att_struct*)_tmp271;if(_tmp274->tag != 21)
goto _LL18B;else{_tmp275=_tmp274->f1;}}_LL18A:{struct Cyc_List_List*_tmp807;
nolive_unique_params=((_tmp807=_region_malloc(temp,sizeof(*_tmp807)),((_tmp807->hd=(
void*)_tmp275,((_tmp807->tl=nolive_unique_params,_tmp807))))));}goto _LL186;
_LL18B:;_LL18C: goto _LL186;_LL186:;}goto _LL181;_LL184:;_LL185: {const char*_tmp80A;
void*_tmp809;(_tmp809=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp80A="anal_Rexp: bad function type",_tag_dyneither(
_tmp80A,sizeof(char),29))),_tag_dyneither(_tmp809,sizeof(void*),0)));}_LL181:;}
goto _LL17C;_LL17F:;_LL180: {const char*_tmp80D;void*_tmp80C;(_tmp80C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp80D="anal_Rexp: bad function type",
_tag_dyneither(_tmp80D,sizeof(char),29))),_tag_dyneither(_tmp80C,sizeof(void*),0)));}
_LL17C:;}{int i=1;for(0;_tmp267 != 0;(((_tmp267=_tmp267->tl,_tmp1D0=((struct Cyc_List_List*)
_check_null(_tmp1D0))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(
init_params,i) && !((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(
nolive_unique_params,i)){_tmp268=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1D0))->hd)->loc,_tmp268,(void*)_tmp267->hd);
continue;}if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(
nolive_unique_params,i)){_tmp268=Cyc_NewControlFlow_use_nounique_Rval(env,((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1D0))->hd)->loc,(
void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp1D0))->hd)->topt),_tmp268,(void*)_tmp267->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo
_tmp27C=_tmp266;int _tmp27D;struct _tuple13 _tmp27E;struct Cyc_Dict_Dict _tmp27F;
_LL18E: if((_tmp27C.BottomFL).tag != 1)goto _LL190;_tmp27D=(int)(_tmp27C.BottomFL).val;
_LL18F: goto _LL18D;_LL190: if((_tmp27C.ReachableFL).tag != 2)goto _LL18D;_tmp27E=(
struct _tuple13)(_tmp27C.ReachableFL).val;_tmp27F=_tmp27E.f1;_LL191: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp27F,(void*)_tmp267->hd)== Cyc_CfFlowInfo_NoneIL){const char*_tmp810;
void*_tmp80F;(_tmp80F=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1D0))->hd)->loc,((_tmp810="expression may not be initialized",
_tag_dyneither(_tmp810,sizeof(char),34))),_tag_dyneither(_tmp80F,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp282=_tmp268;int _tmp283;struct _tuple13 _tmp284;
struct Cyc_Dict_Dict _tmp285;struct Cyc_List_List*_tmp286;_LL193: if((_tmp282.BottomFL).tag
!= 1)goto _LL195;_tmp283=(int)(_tmp282.BottomFL).val;_LL194: goto _LL192;_LL195: if((
_tmp282.ReachableFL).tag != 2)goto _LL192;_tmp284=(struct _tuple13)(_tmp282.ReachableFL).val;
_tmp285=_tmp284.f1;_tmp286=_tmp284.f2;_LL196: {struct Cyc_Dict_Dict _tmp287=Cyc_CfFlowInfo_escape_deref(
_tmp173,_tmp285,env->all_changed,(void*)_tmp267->hd);{void*_tmp288=(void*)
_tmp267->hd;struct Cyc_CfFlowInfo_Place*_tmp28A;_LL198: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp289=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp288;if(_tmp289->tag != 5)goto
_LL19A;else{_tmp28A=_tmp289->f1;}}_LL199:{void*_tmp28B=Cyc_Tcutil_compress((void*)
_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1D0))->hd)->topt));
struct Cyc_Absyn_PtrInfo _tmp28D;void*_tmp28E;_LL19D: {struct Cyc_Absyn_PointerType_struct*
_tmp28C=(struct Cyc_Absyn_PointerType_struct*)_tmp28B;if(_tmp28C->tag != 5)goto
_LL19F;else{_tmp28D=_tmp28C->f1;_tmp28E=_tmp28D.elt_typ;}}_LL19E: _tmp287=Cyc_CfFlowInfo_assign_place(
_tmp173,((struct Cyc_Absyn_Exp*)_tmp1D0->hd)->loc,_tmp287,env->all_changed,
_tmp28A,Cyc_CfFlowInfo_typ_to_absrval(_tmp173,_tmp28E,0,_tmp173->esc_all));goto
_LL19C;_LL19F:;_LL1A0: {const char*_tmp813;void*_tmp812;(_tmp812=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp813="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp813,sizeof(char),39))),_tag_dyneither(_tmp812,sizeof(void*),0)));}
_LL19C:;}goto _LL197;_LL19A:;_LL19B: goto _LL197;_LL197:;}_tmp268=Cyc_CfFlowInfo_ReachableFL(
_tmp287,_tmp286);goto _LL192;}_LL192:;}goto _LL18D;_LL18D:;};}}if(Cyc_Tcutil_is_noreturn((
void*)_check_null(_tmp1CF->topt))){struct _tuple14 _tmp814;struct _tuple14 _tmp292=(
_tmp814.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp814.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,(void*)_check_null(e->topt),0,_tmp173->unknown_all),_tmp814)));
_npop_handler(0);return _tmp292;}else{struct _tuple14 _tmp815;struct _tuple14 _tmp294=(
_tmp815.f1=_tmp268,((_tmp815.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)
_check_null(e->topt),0,_tmp173->unknown_all),_tmp815)));_npop_handler(0);return
_tmp294;}};};};_pop_region(temp);}_LL13B: {struct Cyc_Absyn_Malloc_e_struct*
_tmp1D1=(struct Cyc_Absyn_Malloc_e_struct*)_tmp18D;if(_tmp1D1->tag != 32)goto
_LL13D;else{_tmp1D2=_tmp1D1->f1;_tmp1D3=_tmp1D2.is_calloc;_tmp1D4=_tmp1D2.rgn;
_tmp1D5=_tmp1D2.elt_type;_tmp1D6=_tmp1D2.num_elts;_tmp1D7=_tmp1D2.fat_result;}}
_LL13C: {struct Cyc_CfFlowInfo_MallocPt_struct _tmp818;struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp817;void*root=(void*)((_tmp817=_region_malloc(_tmp173->r,sizeof(*_tmp817)),((
_tmp817[0]=((_tmp818.tag=1,((_tmp818.f1=_tmp1D6,((_tmp818.f2=(void*)((void*)
_check_null(e->topt)),_tmp818)))))),_tmp817))));struct Cyc_CfFlowInfo_Place*
_tmp819;struct Cyc_CfFlowInfo_Place*place=(_tmp819=_region_malloc(_tmp173->r,
sizeof(*_tmp819)),((_tmp819->root=root,((_tmp819->fields=0,_tmp819)))));struct
Cyc_CfFlowInfo_AddressOf_struct _tmp81C;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp81B;void*rval=(void*)((_tmp81B=_region_malloc(_tmp173->r,sizeof(*_tmp81B)),((
_tmp81B[0]=((_tmp81C.tag=5,((_tmp81C.f1=place,_tmp81C)))),_tmp81B))));void*
place_val;if(_tmp1D7)place_val=_tmp173->notzeroall;else{if(_tmp1D3)place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,*((void**)_check_null(_tmp1D5)),0,_tmp173->zero);else{place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,*((void**)_check_null(_tmp1D5)),0,_tmp173->unknown_none);}}{union Cyc_CfFlowInfo_FlowInfo
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,
unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);if(
_tmp1D4 != 0){struct _RegionHandle _tmp295=_new_region("temp");struct _RegionHandle*
temp=& _tmp295;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo _tmp298;struct Cyc_List_List*
_tmp299;struct Cyc_Absyn_Exp*_tmp81D[2];struct _tuple20 _tmp297=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp81D[1]=_tmp1D6,((_tmp81D[0]=(struct Cyc_Absyn_Exp*)_tmp1D4,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
temp,_tag_dyneither(_tmp81D,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp298=
_tmp297.f1;_tmp299=_tmp297.f2;outflow=_tmp298;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo
_tmp29B;struct _tuple14 _tmp29A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D6);
_tmp29B=_tmp29A.f1;outflow=_tmp29B;}{union Cyc_CfFlowInfo_FlowInfo _tmp29C=outflow;
int _tmp29D;struct _tuple13 _tmp29E;struct Cyc_Dict_Dict _tmp29F;struct Cyc_List_List*
_tmp2A0;_LL1A2: if((_tmp29C.BottomFL).tag != 1)goto _LL1A4;_tmp29D=(int)(_tmp29C.BottomFL).val;
_LL1A3: {struct _tuple14 _tmp81E;return(_tmp81E.f1=outflow,((_tmp81E.f2=rval,
_tmp81E)));}_LL1A4: if((_tmp29C.ReachableFL).tag != 2)goto _LL1A1;_tmp29E=(struct
_tuple13)(_tmp29C.ReachableFL).val;_tmp29F=_tmp29E.f1;_tmp2A0=_tmp29E.f2;_LL1A5: {
struct _tuple14 _tmp81F;return(_tmp81F.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp29F,root,place_val),
_tmp2A0),((_tmp81F.f2=rval,_tmp81F)));}_LL1A1:;};};}_LL13D: {struct Cyc_Absyn_Swap_e_struct*
_tmp1D8=(struct Cyc_Absyn_Swap_e_struct*)_tmp18D;if(_tmp1D8->tag != 33)goto _LL13F;
else{_tmp1D9=_tmp1D8->f1;_tmp1DA=_tmp1D8->f2;}}_LL13E: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle _tmp2A8=
_new_region("temp");struct _RegionHandle*temp=& _tmp2A8;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo _tmp2AB;struct Cyc_List_List*_tmp2AC;struct Cyc_Absyn_Exp*
_tmp820[2];struct _tuple20 _tmp2AA=Cyc_NewControlFlow_anal_unordered_Rexps(temp,
env,inflow,((_tmp820[1]=_tmp1DA,((_tmp820[0]=_tmp1D9,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp820,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp2AB=_tmp2AA.f1;_tmp2AC=
_tmp2AA.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AC))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AC->tl))->hd;outflow=
_tmp2AB;}{union Cyc_CfFlowInfo_AbsLVal _tmp2AE;struct _tuple15 _tmp2AD=Cyc_NewControlFlow_anal_Lexp(
env,outflow,0,_tmp1D9);_tmp2AE=_tmp2AD.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp2B0;
struct _tuple15 _tmp2AF=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1DA);
_tmp2B0=_tmp2AF.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2B1=outflow;struct _tuple13
_tmp2B2;struct Cyc_Dict_Dict _tmp2B3;struct Cyc_List_List*_tmp2B4;_LL1A7: if((
_tmp2B1.ReachableFL).tag != 2)goto _LL1A9;_tmp2B2=(struct _tuple13)(_tmp2B1.ReachableFL).val;
_tmp2B3=_tmp2B2.f1;_tmp2B4=_tmp2B2.f2;_LL1A8:{union Cyc_CfFlowInfo_AbsLVal _tmp2B5=
_tmp2AE;struct Cyc_CfFlowInfo_Place*_tmp2B6;int _tmp2B7;_LL1AC: if((_tmp2B5.PlaceL).tag
!= 1)goto _LL1AE;_tmp2B6=(struct Cyc_CfFlowInfo_Place*)(_tmp2B5.PlaceL).val;_LL1AD:
_tmp2B3=Cyc_CfFlowInfo_assign_place(_tmp173,_tmp1D9->loc,_tmp2B3,env->all_changed,
_tmp2B6,right_rval);goto _LL1AB;_LL1AE: if((_tmp2B5.UnknownL).tag != 2)goto _LL1AB;
_tmp2B7=(int)(_tmp2B5.UnknownL).val;_LL1AF: goto _LL1AB;_LL1AB:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp2B8=_tmp2B0;struct Cyc_CfFlowInfo_Place*_tmp2B9;int _tmp2BA;_LL1B1: if((_tmp2B8.PlaceL).tag
!= 1)goto _LL1B3;_tmp2B9=(struct Cyc_CfFlowInfo_Place*)(_tmp2B8.PlaceL).val;_LL1B2:
_tmp2B3=Cyc_CfFlowInfo_assign_place(_tmp173,_tmp1DA->loc,_tmp2B3,env->all_changed,
_tmp2B9,left_rval);goto _LL1B0;_LL1B3: if((_tmp2B8.UnknownL).tag != 2)goto _LL1B0;
_tmp2BA=(int)(_tmp2B8.UnknownL).val;_LL1B4: goto _LL1B0;_LL1B0:;}_tmp2B4=Cyc_CfFlowInfo_reln_kill_exp(
_tmp173->r,_tmp2B4,_tmp1D9);_tmp2B4=Cyc_CfFlowInfo_reln_kill_exp(_tmp173->r,
_tmp2B4,_tmp1DA);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2B3,_tmp2B4);goto _LL1A6;
_LL1A9:;_LL1AA: goto _LL1A6;_LL1A6:;}{struct _tuple14 _tmp821;struct _tuple14 _tmp2BC=(
_tmp821.f1=outflow,((_tmp821.f2=_tmp173->unknown_all,_tmp821)));_npop_handler(0);
return _tmp2BC;};};};;_pop_region(temp);}_LL13F: {struct Cyc_Absyn_New_e_struct*
_tmp1DB=(struct Cyc_Absyn_New_e_struct*)_tmp18D;if(_tmp1DB->tag != 15)goto _LL141;
else{_tmp1DC=_tmp1DB->f1;_tmp1DD=_tmp1DB->f2;}}_LL140: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp824;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp823;void*root=(void*)((_tmp823=
_region_malloc(_tmp173->r,sizeof(*_tmp823)),((_tmp823[0]=((_tmp824.tag=1,((
_tmp824.f1=_tmp1DD,((_tmp824.f2=(void*)((void*)_check_null(e->topt)),_tmp824)))))),
_tmp823))));struct Cyc_CfFlowInfo_Place*_tmp825;struct Cyc_CfFlowInfo_Place*place=(
_tmp825=_region_malloc(_tmp173->r,sizeof(*_tmp825)),((_tmp825->root=root,((
_tmp825->fields=0,_tmp825)))));struct Cyc_CfFlowInfo_AddressOf_struct _tmp828;
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp827;void*rval=(void*)((_tmp827=
_region_malloc(_tmp173->r,sizeof(*_tmp827)),((_tmp827[0]=((_tmp828.tag=5,((
_tmp828.f1=place,_tmp828)))),_tmp827))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;if(_tmp1DC != 0){
struct _RegionHandle _tmp2BD=_new_region("temp");struct _RegionHandle*temp=& _tmp2BD;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo _tmp2C0;struct Cyc_List_List*
_tmp2C1;struct Cyc_Absyn_Exp*_tmp829[2];struct _tuple20 _tmp2BF=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp829[1]=_tmp1DD,((_tmp829[0]=(struct Cyc_Absyn_Exp*)_tmp1DC,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
temp,_tag_dyneither(_tmp829,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp2C0=
_tmp2BF.f1;_tmp2C1=_tmp2BF.f2;outflow=_tmp2C0;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2C1))->tl))->hd;};_pop_region(
temp);}else{union Cyc_CfFlowInfo_FlowInfo _tmp2C3;void*_tmp2C4;struct _tuple14
_tmp2C2=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1DD);_tmp2C3=_tmp2C2.f1;
_tmp2C4=_tmp2C2.f2;outflow=_tmp2C3;place_val=_tmp2C4;}{union Cyc_CfFlowInfo_FlowInfo
_tmp2C5=outflow;int _tmp2C6;struct _tuple13 _tmp2C7;struct Cyc_Dict_Dict _tmp2C8;
struct Cyc_List_List*_tmp2C9;_LL1B6: if((_tmp2C5.BottomFL).tag != 1)goto _LL1B8;
_tmp2C6=(int)(_tmp2C5.BottomFL).val;_LL1B7: {struct _tuple14 _tmp82A;return(
_tmp82A.f1=outflow,((_tmp82A.f2=rval,_tmp82A)));}_LL1B8: if((_tmp2C5.ReachableFL).tag
!= 2)goto _LL1B5;_tmp2C7=(struct _tuple13)(_tmp2C5.ReachableFL).val;_tmp2C8=
_tmp2C7.f1;_tmp2C9=_tmp2C7.f2;_LL1B9: {struct _tuple14 _tmp82B;return(_tmp82B.f1=
Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,
void*v))Cyc_Dict_insert)(_tmp2C8,root,place_val),_tmp2C9),((_tmp82B.f2=rval,
_tmp82B)));}_LL1B5:;};};}_LL141: {struct Cyc_Absyn_Address_e_struct*_tmp1DE=(
struct Cyc_Absyn_Address_e_struct*)_tmp18D;if(_tmp1DE->tag != 14)goto _LL143;else{
_tmp1DF=_tmp1DE->f1;}}_LL142: {union Cyc_CfFlowInfo_FlowInfo _tmp2D2;union Cyc_CfFlowInfo_AbsLVal
_tmp2D3;struct _tuple15 _tmp2D1=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1DF);
_tmp2D2=_tmp2D1.f1;_tmp2D3=_tmp2D1.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp2D4=
_tmp2D3;int _tmp2D5;struct Cyc_CfFlowInfo_Place*_tmp2D6;_LL1BB: if((_tmp2D4.UnknownL).tag
!= 2)goto _LL1BD;_tmp2D5=(int)(_tmp2D4.UnknownL).val;_LL1BC: {struct _tuple14
_tmp82C;return(_tmp82C.f1=_tmp2D2,((_tmp82C.f2=_tmp173->notzeroall,_tmp82C)));}
_LL1BD: if((_tmp2D4.PlaceL).tag != 1)goto _LL1BA;_tmp2D6=(struct Cyc_CfFlowInfo_Place*)(
_tmp2D4.PlaceL).val;_LL1BE: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp832;
struct Cyc_CfFlowInfo_AddressOf_struct _tmp831;struct _tuple14 _tmp830;return(
_tmp830.f1=_tmp2D2,((_tmp830.f2=(void*)((_tmp832=_region_malloc(env->r,sizeof(*
_tmp832)),((_tmp832[0]=((_tmp831.tag=5,((_tmp831.f1=_tmp2D6,_tmp831)))),_tmp832)))),
_tmp830)));}_LL1BA:;};}_LL143: {struct Cyc_Absyn_Deref_e_struct*_tmp1E0=(struct
Cyc_Absyn_Deref_e_struct*)_tmp18D;if(_tmp1E0->tag != 19)goto _LL145;else{_tmp1E1=
_tmp1E0->f1;}}_LL144: {union Cyc_CfFlowInfo_FlowInfo _tmp2DC;void*_tmp2DD;struct
_tuple14 _tmp2DB=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1E1);_tmp2DC=
_tmp2DB.f1;_tmp2DD=_tmp2DB.f2;return Cyc_NewControlFlow_anal_derefR(env,inflow,
_tmp2DC,_tmp1E1,_tmp2DD);}_LL145: {struct Cyc_Absyn_AggrMember_e_struct*_tmp1E2=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp18D;if(_tmp1E2->tag != 20)goto _LL147;
else{_tmp1E3=_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;}}_LL146: {union Cyc_CfFlowInfo_FlowInfo
_tmp2DF;void*_tmp2E0;struct _tuple14 _tmp2DE=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1E3);_tmp2DF=_tmp2DE.f1;_tmp2E0=_tmp2DE.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)_check_null(_tmp1E3->topt))){struct _tuple14 _tmp833;return(_tmp833.f1=
_tmp2DF,((_tmp833.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)_check_null(e->topt),
0,_tmp173->unknown_all),_tmp833)));}{void*_tmp2E2=_tmp2E0;struct Cyc_CfFlowInfo_UnionRInfo
_tmp2E4;int _tmp2E5;int _tmp2E6;struct _dyneither_ptr _tmp2E7;_LL1C0: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2E3=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2E2;if(_tmp2E3->tag != 6)goto
_LL1C2;else{_tmp2E4=_tmp2E3->f1;_tmp2E5=_tmp2E4.is_union;_tmp2E6=_tmp2E4.fieldnum;
_tmp2E7=_tmp2E3->f2;}}_LL1C1: {int _tmp2E8=Cyc_CfFlowInfo_get_field_index((void*)
_check_null(_tmp1E3->topt),_tmp1E4);if((_tmp2E5  && _tmp2E6 != - 1) && _tmp2E6 != 
_tmp2E8){struct _tuple14 _tmp834;return(_tmp834.f1=_tmp2DF,((_tmp834.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,(void*)_check_null(e->topt),1,_tmp173->unknown_none),_tmp834)));}{struct
_tuple14 _tmp835;return(_tmp835.f1=_tmp2DF,((_tmp835.f2=*((void**)
_check_dyneither_subscript(_tmp2E7,sizeof(void*),_tmp2E8)),_tmp835)));};}_LL1C2:;
_LL1C3: {struct Cyc_Core_Impossible_struct _tmp842;const char*_tmp841;void*_tmp840[
1];struct Cyc_String_pa_struct _tmp83F;struct Cyc_Core_Impossible_struct*_tmp83E;(
int)_throw((void*)((_tmp83E=_cycalloc(sizeof(*_tmp83E)),((_tmp83E[0]=((_tmp842.tag=
Cyc_Core_Impossible,((_tmp842.f1=(struct _dyneither_ptr)((_tmp83F.tag=0,((_tmp83F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp840[
0]=& _tmp83F,Cyc_aprintf(((_tmp841="anal_Rexp: AggrMember: %s",_tag_dyneither(
_tmp841,sizeof(char),26))),_tag_dyneither(_tmp840,sizeof(void*),1)))))))),
_tmp842)))),_tmp83E)))));}_LL1BF:;};}_LL147: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1E5=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp18D;if(_tmp1E5->tag != 36)goto
_LL149;else{_tmp1E6=_tmp1E5->f1;_tmp1E7=_tmp1E5->f2;}}_LL148: {union Cyc_CfFlowInfo_FlowInfo
_tmp2F1;void*_tmp2F2;struct _tuple14 _tmp2F0=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1E6);_tmp2F1=_tmp2F0.f1;_tmp2F2=_tmp2F0.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)_check_null(_tmp1E6->topt))){struct _tuple14 _tmp843;return(_tmp843.f1=
_tmp2F1,((_tmp843.f2=_tmp173->unknown_all,_tmp843)));}{void*_tmp2F4=_tmp2F2;
struct Cyc_CfFlowInfo_UnionRInfo _tmp2F6;int _tmp2F7;int _tmp2F8;struct
_dyneither_ptr _tmp2F9;_LL1C5: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2F5=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2F4;if(_tmp2F5->tag != 6)goto _LL1C7;
else{_tmp2F6=_tmp2F5->f1;_tmp2F7=_tmp2F6.is_union;_tmp2F8=_tmp2F6.fieldnum;
_tmp2F9=_tmp2F5->f2;}}_LL1C6: {int _tmp2FA=Cyc_CfFlowInfo_get_field_index((void*)
_check_null(_tmp1E6->topt),_tmp1E7);if(_tmp2F7  && _tmp2F8 != - 1){if(_tmp2F8 != 
_tmp2FA){struct _tuple14 _tmp844;return(_tmp844.f1=_tmp2F1,((_tmp844.f2=_tmp173->zero,
_tmp844)));}else{struct _tuple14 _tmp845;return(_tmp845.f1=_tmp2F1,((_tmp845.f2=
_tmp173->notzeroall,_tmp845)));}}else{struct _tuple14 _tmp846;return(_tmp846.f1=
_tmp2F1,((_tmp846.f2=_tmp173->unknown_all,_tmp846)));}}_LL1C7:;_LL1C8: {struct
Cyc_Core_Impossible_struct _tmp853;const char*_tmp852;void*_tmp851[1];struct Cyc_String_pa_struct
_tmp850;struct Cyc_Core_Impossible_struct*_tmp84F;(int)_throw((void*)((_tmp84F=
_cycalloc(sizeof(*_tmp84F)),((_tmp84F[0]=((_tmp853.tag=Cyc_Core_Impossible,((
_tmp853.f1=(struct _dyneither_ptr)((_tmp850.tag=0,((_tmp850.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp851[0]=&
_tmp850,Cyc_aprintf(((_tmp852="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp852,
sizeof(char),26))),_tag_dyneither(_tmp851,sizeof(void*),1)))))))),_tmp853)))),
_tmp84F)))));}_LL1C4:;};}_LL149: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp1E8=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp18D;if(_tmp1E8->tag != 21)goto _LL14B;
else{_tmp1E9=_tmp1E8->f1;_tmp1EA=_tmp1E8->f2;}}_LL14A: {union Cyc_CfFlowInfo_FlowInfo
_tmp304;void*_tmp305;struct _tuple14 _tmp303=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1E9);_tmp304=_tmp303.f1;_tmp305=_tmp303.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp307;void*_tmp308;struct _tuple14 _tmp306=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp304,_tmp1E9,_tmp305);_tmp307=_tmp306.f1;_tmp308=_tmp306.f2;{void*
_tmp309=Cyc_Tcutil_compress((void*)_check_null(_tmp1E9->topt));struct Cyc_Absyn_PtrInfo
_tmp30B;void*_tmp30C;_LL1CA: {struct Cyc_Absyn_PointerType_struct*_tmp30A=(struct
Cyc_Absyn_PointerType_struct*)_tmp309;if(_tmp30A->tag != 5)goto _LL1CC;else{
_tmp30B=_tmp30A->f1;_tmp30C=_tmp30B.elt_typ;}}_LL1CB: if(Cyc_Absyn_is_nontagged_union_type(
_tmp30C)){struct _tuple14 _tmp854;return(_tmp854.f1=_tmp307,((_tmp854.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,(void*)_check_null(e->topt),0,_tmp173->unknown_all),_tmp854)));}{void*
_tmp30E=_tmp308;struct Cyc_CfFlowInfo_UnionRInfo _tmp310;int _tmp311;int _tmp312;
struct _dyneither_ptr _tmp313;_LL1CF: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp30F=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp30E;if(_tmp30F->tag != 6)goto
_LL1D1;else{_tmp310=_tmp30F->f1;_tmp311=_tmp310.is_union;_tmp312=_tmp310.fieldnum;
_tmp313=_tmp30F->f2;}}_LL1D0: {int _tmp314=Cyc_CfFlowInfo_get_field_index(_tmp30C,
_tmp1EA);if((_tmp311  && _tmp312 != - 1) && _tmp312 != _tmp314){struct _tuple14
_tmp855;return(_tmp855.f1=_tmp307,((_tmp855.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp173,(void*)_check_null(e->topt),1,_tmp173->unknown_none),_tmp855)));}{struct
_tuple14 _tmp856;return(_tmp856.f1=_tmp307,((_tmp856.f2=*((void**)
_check_dyneither_subscript(_tmp313,sizeof(void*),_tmp314)),_tmp856)));};}_LL1D1:;
_LL1D2: {struct Cyc_Core_Impossible_struct _tmp85C;const char*_tmp85B;struct Cyc_Core_Impossible_struct*
_tmp85A;(int)_throw((void*)((_tmp85A=_cycalloc(sizeof(*_tmp85A)),((_tmp85A[0]=((
_tmp85C.tag=Cyc_Core_Impossible,((_tmp85C.f1=((_tmp85B="anal_Rexp: AggrArrow",
_tag_dyneither(_tmp85B,sizeof(char),21))),_tmp85C)))),_tmp85A)))));}_LL1CE:;};
_LL1CC:;_LL1CD: {struct Cyc_Core_Impossible_struct _tmp862;const char*_tmp861;
struct Cyc_Core_Impossible_struct*_tmp860;(int)_throw((void*)((_tmp860=_cycalloc(
sizeof(*_tmp860)),((_tmp860[0]=((_tmp862.tag=Cyc_Core_Impossible,((_tmp862.f1=((
_tmp861="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp861,sizeof(char),25))),
_tmp862)))),_tmp860)))));}_LL1C9:;};};}_LL14B: {struct Cyc_Absyn_Conditional_e_struct*
_tmp1EB=(struct Cyc_Absyn_Conditional_e_struct*)_tmp18D;if(_tmp1EB->tag != 5)goto
_LL14D;else{_tmp1EC=_tmp1EB->f1;_tmp1ED=_tmp1EB->f2;_tmp1EE=_tmp1EB->f3;}}_LL14C: {
union Cyc_CfFlowInfo_FlowInfo _tmp31E;union Cyc_CfFlowInfo_FlowInfo _tmp31F;struct
_tuple16 _tmp31D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1EC);_tmp31E=_tmp31D.f1;
_tmp31F=_tmp31D.f2;{struct _tuple14 _tmp320=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp31E,_tmp1ED);struct _tuple14 _tmp321=Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,_tmp31F,_tmp1EE);return Cyc_CfFlowInfo_join_flow_and_rval(_tmp173,
env->all_changed,_tmp320,_tmp321);};}_LL14D: {struct Cyc_Absyn_And_e_struct*
_tmp1EF=(struct Cyc_Absyn_And_e_struct*)_tmp18D;if(_tmp1EF->tag != 6)goto _LL14F;
else{_tmp1F0=_tmp1EF->f1;_tmp1F1=_tmp1EF->f2;}}_LL14E: {union Cyc_CfFlowInfo_FlowInfo
_tmp323;union Cyc_CfFlowInfo_FlowInfo _tmp324;struct _tuple16 _tmp322=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1F0);_tmp323=_tmp322.f1;_tmp324=_tmp322.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp326;void*_tmp327;struct _tuple14 _tmp325=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp323,_tmp1F1);_tmp326=_tmp325.f1;_tmp327=_tmp325.f2;{struct _tuple14
_tmp863;struct _tuple14 _tmp328=(_tmp863.f1=_tmp326,((_tmp863.f2=_tmp327,_tmp863)));
struct _tuple14 _tmp864;struct _tuple14 _tmp329=(_tmp864.f1=_tmp324,((_tmp864.f2=
_tmp173->zero,_tmp864)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp173,env->all_changed,
_tmp328,_tmp329);};};}_LL14F: {struct Cyc_Absyn_Or_e_struct*_tmp1F2=(struct Cyc_Absyn_Or_e_struct*)
_tmp18D;if(_tmp1F2->tag != 7)goto _LL151;else{_tmp1F3=_tmp1F2->f1;_tmp1F4=_tmp1F2->f2;}}
_LL150: {union Cyc_CfFlowInfo_FlowInfo _tmp32D;union Cyc_CfFlowInfo_FlowInfo _tmp32E;
struct _tuple16 _tmp32C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1F3);_tmp32D=
_tmp32C.f1;_tmp32E=_tmp32C.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp330;void*_tmp331;
struct _tuple14 _tmp32F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp32E,_tmp1F4);
_tmp330=_tmp32F.f1;_tmp331=_tmp32F.f2;{struct _tuple14 _tmp865;struct _tuple14
_tmp332=(_tmp865.f1=_tmp330,((_tmp865.f2=_tmp331,_tmp865)));struct _tuple14
_tmp866;struct _tuple14 _tmp333=(_tmp866.f1=_tmp32D,((_tmp866.f2=_tmp173->notzeroall,
_tmp866)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp173,env->all_changed,
_tmp332,_tmp333);};};}_LL151: {struct Cyc_Absyn_Subscript_e_struct*_tmp1F5=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp18D;if(_tmp1F5->tag != 22)goto _LL153;
else{_tmp1F6=_tmp1F5->f1;_tmp1F7=_tmp1F5->f2;}}_LL152: {struct _RegionHandle
_tmp336=_new_region("temp");struct _RegionHandle*temp=& _tmp336;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp339;struct Cyc_List_List*_tmp33A;struct Cyc_Absyn_Exp*
_tmp867[2];struct _tuple20 _tmp338=Cyc_NewControlFlow_anal_unordered_Rexps(temp,
env,inflow,((_tmp867[1]=_tmp1F7,((_tmp867[0]=_tmp1F6,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp867,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp339=_tmp338.f1;_tmp33A=
_tmp338.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp33B=_tmp339;{union Cyc_CfFlowInfo_FlowInfo
_tmp33C=_tmp339;struct _tuple13 _tmp33D;struct Cyc_Dict_Dict _tmp33E;struct Cyc_List_List*
_tmp33F;_LL1D4: if((_tmp33C.ReachableFL).tag != 2)goto _LL1D6;_tmp33D=(struct
_tuple13)(_tmp33C.ReachableFL).val;_tmp33E=_tmp33D.f1;_tmp33F=_tmp33D.f2;_LL1D5:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp33E,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp33A))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp86A;void*_tmp869;(_tmp869=0,Cyc_Tcutil_terr(_tmp1F7->loc,((_tmp86A="expression may not be initialized",
_tag_dyneither(_tmp86A,sizeof(char),34))),_tag_dyneither(_tmp869,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp342=Cyc_NewControlFlow_add_subscript_reln(_tmp173->r,
_tmp33F,_tmp1F6,_tmp1F7);if(_tmp33F != _tmp342)_tmp33B=Cyc_CfFlowInfo_ReachableFL(
_tmp33E,_tmp342);goto _LL1D3;};_LL1D6:;_LL1D7: goto _LL1D3;_LL1D3:;}{void*_tmp343=
Cyc_Tcutil_compress((void*)_check_null(_tmp1F6->topt));struct Cyc_List_List*
_tmp345;struct Cyc_Absyn_PtrInfo _tmp347;struct Cyc_Absyn_PtrAtts _tmp348;union Cyc_Absyn_Constraint*
_tmp349;_LL1D9: {struct Cyc_Absyn_TupleType_struct*_tmp344=(struct Cyc_Absyn_TupleType_struct*)
_tmp343;if(_tmp344->tag != 10)goto _LL1DB;else{_tmp345=_tmp344->f1;}}_LL1DA: {void*
_tmp34A=(void*)((struct Cyc_List_List*)_check_null(_tmp33A))->hd;struct
_dyneither_ptr _tmp34C;_LL1E0: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp34B=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp34A;if(_tmp34B->tag != 6)goto _LL1E2;
else{_tmp34C=_tmp34B->f2;}}_LL1E1: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(
_tmp1F7)).f1;struct _tuple14 _tmp86B;struct _tuple14 _tmp34E=(_tmp86B.f1=_tmp33B,((
_tmp86B.f2=*((void**)_check_dyneither_subscript(_tmp34C,sizeof(void*),(int)i)),
_tmp86B)));_npop_handler(0);return _tmp34E;}_LL1E2:;_LL1E3: {struct Cyc_Core_Impossible_struct
_tmp871;const char*_tmp870;struct Cyc_Core_Impossible_struct*_tmp86F;(int)_throw((
void*)((_tmp86F=_cycalloc(sizeof(*_tmp86F)),((_tmp86F[0]=((_tmp871.tag=Cyc_Core_Impossible,((
_tmp871.f1=((_tmp870="anal_Rexp: Subscript",_tag_dyneither(_tmp870,sizeof(char),
21))),_tmp871)))),_tmp86F)))));}_LL1DF:;}_LL1DB: {struct Cyc_Absyn_PointerType_struct*
_tmp346=(struct Cyc_Absyn_PointerType_struct*)_tmp343;if(_tmp346->tag != 5)goto
_LL1DD;else{_tmp347=_tmp346->f1;_tmp348=_tmp347.ptr_atts;_tmp349=_tmp348.bounds;}}
_LL1DC: {union Cyc_CfFlowInfo_FlowInfo _tmp353;void*_tmp354;struct _tuple14 _tmp352=
Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp339,_tmp1F6,(void*)((struct Cyc_List_List*)
_check_null(_tmp33A))->hd);_tmp353=_tmp352.f1;_tmp354=_tmp352.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp355=_tmp353;int _tmp356;_LL1E5: if((_tmp355.BottomFL).tag != 1)goto _LL1E7;
_tmp356=(int)(_tmp355.BottomFL).val;_LL1E6: {struct _tuple14 _tmp872;struct
_tuple14 _tmp358=(_tmp872.f1=_tmp353,((_tmp872.f2=_tmp354,_tmp872)));
_npop_handler(0);return _tmp358;}_LL1E7:;_LL1E8: {struct _tuple14 _tmp873;struct
_tuple14 _tmp35A=(_tmp873.f1=_tmp33B,((_tmp873.f2=_tmp354,_tmp873)));
_npop_handler(0);return _tmp35A;}_LL1E4:;};}_LL1DD:;_LL1DE: {struct Cyc_Core_Impossible_struct
_tmp879;const char*_tmp878;struct Cyc_Core_Impossible_struct*_tmp877;(int)_throw((
void*)((_tmp877=_cycalloc(sizeof(*_tmp877)),((_tmp877[0]=((_tmp879.tag=Cyc_Core_Impossible,((
_tmp879.f1=((_tmp878="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp878,
sizeof(char),33))),_tmp879)))),_tmp877)))));}_LL1D8:;};};};_pop_region(temp);}
_LL153: {struct Cyc_Absyn_Datatype_e_struct*_tmp1F8=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp18D;if(_tmp1F8->tag != 29)goto _LL155;else{_tmp1F9=_tmp1F8->f1;_tmp1FA=_tmp1F8->f2;}}
_LL154: _tmp1FC=_tmp1F9;goto _LL156;_LL155: {struct Cyc_Absyn_Tuple_e_struct*
_tmp1FB=(struct Cyc_Absyn_Tuple_e_struct*)_tmp18D;if(_tmp1FB->tag != 23)goto _LL157;
else{_tmp1FC=_tmp1FB->f1;}}_LL156: {struct _RegionHandle _tmp35E=_new_region("temp");
struct _RegionHandle*temp=& _tmp35E;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo
_tmp360;struct Cyc_List_List*_tmp361;struct _tuple20 _tmp35F=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp1FC,1,1);_tmp360=_tmp35F.f1;_tmp361=_tmp35F.f2;{unsigned int
_tmp87F;unsigned int _tmp87E;struct _dyneither_ptr _tmp87D;void**_tmp87C;
unsigned int _tmp87B;struct _dyneither_ptr aggrdict=(_tmp87B=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1FC),((_tmp87C=(void**)
_region_malloc(env->r,_check_times(sizeof(void*),_tmp87B)),((_tmp87D=
_tag_dyneither(_tmp87C,sizeof(void*),_tmp87B),((((_tmp87E=_tmp87B,_tmp880(&
_tmp87F,& _tmp87E,& _tmp87C,& _tmp361))),_tmp87D)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp88A;struct Cyc_CfFlowInfo_UnionRInfo _tmp889;struct Cyc_CfFlowInfo_Aggregate_struct
_tmp888;struct _tuple14 _tmp887;struct _tuple14 _tmp366=(_tmp887.f1=_tmp360,((
_tmp887.f2=(void*)((_tmp88A=_region_malloc(env->r,sizeof(*_tmp88A)),((_tmp88A[0]=((
_tmp888.tag=6,((_tmp888.f1=((_tmp889.is_union=0,((_tmp889.fieldnum=- 1,_tmp889)))),((
_tmp888.f2=aggrdict,_tmp888)))))),_tmp88A)))),_tmp887)));_npop_handler(0);return
_tmp366;};};_pop_region(temp);}_LL157: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1FD=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp18D;if(_tmp1FD->tag != 28)goto
_LL159;else{_tmp1FE=_tmp1FD->f2;}}_LL158: {struct Cyc_List_List*fs;{void*_tmp36C=
Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_List_List*_tmp36E;
_LL1EA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp36D=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp36C;if(_tmp36D->tag != 12)goto _LL1EC;else{_tmp36E=_tmp36D->f2;}}_LL1EB: fs=
_tmp36E;goto _LL1E9;_LL1EC:;_LL1ED: {struct Cyc_Core_Impossible_struct _tmp890;
const char*_tmp88F;struct Cyc_Core_Impossible_struct*_tmp88E;(int)_throw((void*)((
_tmp88E=_cycalloc(sizeof(*_tmp88E)),((_tmp88E[0]=((_tmp890.tag=Cyc_Core_Impossible,((
_tmp890.f1=((_tmp88F="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp88F,
sizeof(char),35))),_tmp890)))),_tmp88E)))));}_LL1E9:;}_tmp200=_tmp1FE;_tmp203=
Cyc_Absyn_StructA;_tmp206=fs;goto _LL15A;}_LL159: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1FF=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp18D;if(_tmp1FF->tag != 27)goto
_LL15B;else{_tmp200=_tmp1FF->f3;_tmp201=_tmp1FF->f4;if(_tmp201 == 0)goto _LL15B;
_tmp202=*_tmp201;_tmp203=_tmp202.kind;_tmp204=_tmp202.impl;if(_tmp204 == 0)goto
_LL15B;_tmp205=*_tmp204;_tmp206=_tmp205.fields;}}_LL15A: {struct _RegionHandle
_tmp372=_new_region("temp");struct _RegionHandle*temp=& _tmp372;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo _tmp374;struct Cyc_List_List*_tmp375;struct _tuple20
_tmp373=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*
x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,
_tmp200),1,1);_tmp374=_tmp373.f1;_tmp375=_tmp373.f2;{struct _dyneither_ptr
aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp173,_tmp206,_tmp203 == Cyc_Absyn_UnionA,
_tmp203 == Cyc_Absyn_UnionA?_tmp173->unknown_none: _tmp173->unknown_all);int
field_no=- 1;{int i=0;for(0;_tmp375 != 0;(((_tmp375=_tmp375->tl,_tmp200=_tmp200->tl)),
++ i)){struct Cyc_List_List*ds=(*((struct _tuple22*)((struct Cyc_List_List*)
_check_null(_tmp200))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*_tmp376=(void*)ds->hd;
struct _dyneither_ptr*_tmp379;_LL1EF: {struct Cyc_Absyn_ArrayElement_struct*
_tmp377=(struct Cyc_Absyn_ArrayElement_struct*)_tmp376;if(_tmp377->tag != 0)goto
_LL1F1;}_LL1F0: {struct Cyc_Core_Impossible_struct _tmp896;const char*_tmp895;
struct Cyc_Core_Impossible_struct*_tmp894;(int)_throw((void*)((_tmp894=_cycalloc(
sizeof(*_tmp894)),((_tmp894[0]=((_tmp896.tag=Cyc_Core_Impossible,((_tmp896.f1=((
_tmp895="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp895,sizeof(char),22))),
_tmp896)))),_tmp894)))));}_LL1F1: {struct Cyc_Absyn_FieldName_struct*_tmp378=(
struct Cyc_Absyn_FieldName_struct*)_tmp376;if(_tmp378->tag != 1)goto _LL1EE;else{
_tmp379=_tmp378->f1;}}_LL1F2: field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp206,
_tmp379);*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(
void*)_tmp375->hd;if(_tmp203 == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*_tmp37D=(*((
struct _tuple22*)_tmp200->hd)).f2;_tmp374=Cyc_NewControlFlow_use_Rval(env,_tmp37D->loc,
_tmp374,(void*)_tmp375->hd);}_LL1EE:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp8A0;struct Cyc_CfFlowInfo_UnionRInfo _tmp89F;struct Cyc_CfFlowInfo_Aggregate_struct
_tmp89E;struct _tuple14 _tmp89D;struct _tuple14 _tmp382=(_tmp89D.f1=_tmp374,((
_tmp89D.f2=(void*)((_tmp8A0=_region_malloc(env->r,sizeof(*_tmp8A0)),((_tmp8A0[0]=((
_tmp89E.tag=6,((_tmp89E.f1=((_tmp89F.is_union=_tmp203 == Cyc_Absyn_UnionA,((
_tmp89F.fieldnum=field_no,_tmp89F)))),((_tmp89E.f2=aggrdict,_tmp89E)))))),
_tmp8A0)))),_tmp89D)));_npop_handler(0);return _tmp382;};};};_pop_region(temp);}
_LL15B: {struct Cyc_Absyn_Aggregate_e_struct*_tmp207=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp18D;if(_tmp207->tag != 27)goto _LL15D;}_LL15C: {struct Cyc_Core_Impossible_struct
_tmp8A6;const char*_tmp8A5;struct Cyc_Core_Impossible_struct*_tmp8A4;(int)_throw((
void*)((_tmp8A4=_cycalloc(sizeof(*_tmp8A4)),((_tmp8A4[0]=((_tmp8A6.tag=Cyc_Core_Impossible,((
_tmp8A6.f1=((_tmp8A5="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp8A5,
sizeof(char),31))),_tmp8A6)))),_tmp8A4)))));}_LL15D: {struct Cyc_Absyn_Array_e_struct*
_tmp208=(struct Cyc_Absyn_Array_e_struct*)_tmp18D;if(_tmp208->tag != 25)goto _LL15F;
else{_tmp209=_tmp208->f1;}}_LL15E: {struct _RegionHandle _tmp386=_new_region("temp");
struct _RegionHandle*temp=& _tmp386;_push_region(temp);{struct Cyc_List_List*
_tmp387=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple22*))Cyc_Core_snd,_tmp209);union Cyc_CfFlowInfo_FlowInfo _tmp389;
struct Cyc_List_List*_tmp38A;struct _tuple20 _tmp388=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp387,1,1);_tmp389=_tmp388.f1;_tmp38A=_tmp388.f2;for(0;_tmp38A
!= 0;(_tmp38A=_tmp38A->tl,_tmp387=_tmp387->tl)){_tmp389=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp387))->hd)->loc,
_tmp389,(void*)_tmp38A->hd);}{struct _tuple14 _tmp8A7;struct _tuple14 _tmp38C=(
_tmp8A7.f1=_tmp389,((_tmp8A7.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp173,(void*)
_check_null(e->topt),0,_tmp173->unknown_all),_tmp8A7)));_npop_handler(0);return
_tmp38C;};};_pop_region(temp);}_LL15F: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp20A=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp18D;if(_tmp20A->tag != 26)
goto _LL161;else{_tmp20B=_tmp20A->f1;_tmp20C=_tmp20A->f2;_tmp20D=_tmp20A->f3;
_tmp20E=_tmp20A->f4;}}_LL160: {union Cyc_CfFlowInfo_FlowInfo _tmp38E;void*_tmp38F;
struct _tuple14 _tmp38D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp20C);_tmp38E=
_tmp38D.f1;_tmp38F=_tmp38D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp390=_tmp38E;int
_tmp391;struct _tuple13 _tmp392;struct Cyc_Dict_Dict _tmp393;struct Cyc_List_List*
_tmp394;_LL1F4: if((_tmp390.BottomFL).tag != 1)goto _LL1F6;_tmp391=(int)(_tmp390.BottomFL).val;
_LL1F5: {struct _tuple14 _tmp8A8;return(_tmp8A8.f1=_tmp38E,((_tmp8A8.f2=_tmp173->unknown_all,
_tmp8A8)));}_LL1F6: if((_tmp390.ReachableFL).tag != 2)goto _LL1F3;_tmp392=(struct
_tuple13)(_tmp390.ReachableFL).val;_tmp393=_tmp392.f1;_tmp394=_tmp392.f2;_LL1F7:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp393,_tmp38F)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8AB;void*_tmp8AA;(_tmp8AA=0,Cyc_Tcutil_terr(_tmp20C->loc,((_tmp8AB="expression may not be initialized",
_tag_dyneither(_tmp8AB,sizeof(char),34))),_tag_dyneither(_tmp8AA,sizeof(void*),0)));}{
struct Cyc_List_List*new_relns=_tmp394;comp_loop: {void*_tmp398=_tmp20C->r;struct
Cyc_Absyn_Exp*_tmp39A;void*_tmp39C;struct Cyc_Absyn_Vardecl*_tmp39E;void*_tmp3A0;
struct Cyc_Absyn_Vardecl*_tmp3A2;void*_tmp3A4;struct Cyc_Absyn_Vardecl*_tmp3A6;
void*_tmp3A8;struct Cyc_Absyn_Vardecl*_tmp3AA;union Cyc_Absyn_Cnst _tmp3AC;struct
_tuple6 _tmp3AD;int _tmp3AE;enum Cyc_Absyn_Primop _tmp3B0;struct Cyc_List_List*
_tmp3B1;struct Cyc_List_List _tmp3B2;struct Cyc_Absyn_Exp*_tmp3B3;_LL1F9: {struct
Cyc_Absyn_Cast_e_struct*_tmp399=(struct Cyc_Absyn_Cast_e_struct*)_tmp398;if(
_tmp399->tag != 13)goto _LL1FB;else{_tmp39A=_tmp399->f2;}}_LL1FA: _tmp20C=_tmp39A;
goto comp_loop;_LL1FB:{struct Cyc_Absyn_Var_e_struct*_tmp39B=(struct Cyc_Absyn_Var_e_struct*)
_tmp398;if(_tmp39B->tag != 1)goto _LL1FD;else{_tmp39C=(void*)_tmp39B->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp39D=(struct Cyc_Absyn_Global_b_struct*)_tmp39C;if(
_tmp39D->tag != 1)goto _LL1FD;else{_tmp39E=_tmp39D->f1;}};}}if(!(!_tmp39E->escapes))
goto _LL1FD;_LL1FC: _tmp3A2=_tmp39E;goto _LL1FE;_LL1FD:{struct Cyc_Absyn_Var_e_struct*
_tmp39F=(struct Cyc_Absyn_Var_e_struct*)_tmp398;if(_tmp39F->tag != 1)goto _LL1FF;
else{_tmp3A0=(void*)_tmp39F->f2;{struct Cyc_Absyn_Local_b_struct*_tmp3A1=(struct
Cyc_Absyn_Local_b_struct*)_tmp3A0;if(_tmp3A1->tag != 4)goto _LL1FF;else{_tmp3A2=
_tmp3A1->f1;}};}}if(!(!_tmp3A2->escapes))goto _LL1FF;_LL1FE: _tmp3A6=_tmp3A2;goto
_LL200;_LL1FF:{struct Cyc_Absyn_Var_e_struct*_tmp3A3=(struct Cyc_Absyn_Var_e_struct*)
_tmp398;if(_tmp3A3->tag != 1)goto _LL201;else{_tmp3A4=(void*)_tmp3A3->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp3A5=(struct Cyc_Absyn_Pat_b_struct*)_tmp3A4;if(_tmp3A5->tag
!= 5)goto _LL201;else{_tmp3A6=_tmp3A5->f1;}};}}if(!(!_tmp3A6->escapes))goto _LL201;
_LL200: _tmp3AA=_tmp3A6;goto _LL202;_LL201:{struct Cyc_Absyn_Var_e_struct*_tmp3A7=(
struct Cyc_Absyn_Var_e_struct*)_tmp398;if(_tmp3A7->tag != 1)goto _LL203;else{
_tmp3A8=(void*)_tmp3A7->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3A9=(struct Cyc_Absyn_Param_b_struct*)
_tmp3A8;if(_tmp3A9->tag != 3)goto _LL203;else{_tmp3AA=_tmp3A9->f1;}};}}if(!(!
_tmp3AA->escapes))goto _LL203;_LL202:{struct Cyc_CfFlowInfo_Reln*_tmp8AE;struct Cyc_List_List*
_tmp8AD;new_relns=((_tmp8AD=_region_malloc(env->r,sizeof(*_tmp8AD)),((_tmp8AD->hd=((
_tmp8AE=_region_malloc(env->r,sizeof(*_tmp8AE)),((_tmp8AE->vd=_tmp20B,((_tmp8AE->rop=
Cyc_CfFlowInfo_LessVar(_tmp3AA,_tmp3AA->type),_tmp8AE)))))),((_tmp8AD->tl=
_tmp394,_tmp8AD))))));}goto _LL1F8;_LL203: {struct Cyc_Absyn_Const_e_struct*
_tmp3AB=(struct Cyc_Absyn_Const_e_struct*)_tmp398;if(_tmp3AB->tag != 0)goto _LL205;
else{_tmp3AC=_tmp3AB->f1;if((_tmp3AC.Int_c).tag != 5)goto _LL205;_tmp3AD=(struct
_tuple6)(_tmp3AC.Int_c).val;_tmp3AE=_tmp3AD.f2;}}_LL204:{struct Cyc_CfFlowInfo_Reln*
_tmp8B1;struct Cyc_List_List*_tmp8B0;new_relns=((_tmp8B0=_region_malloc(env->r,
sizeof(*_tmp8B0)),((_tmp8B0->hd=((_tmp8B1=_region_malloc(env->r,sizeof(*_tmp8B1)),((
_tmp8B1->vd=_tmp20B,((_tmp8B1->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp3AE),_tmp8B1)))))),((_tmp8B0->tl=_tmp394,_tmp8B0))))));}goto _LL1F8;_LL205: {
struct Cyc_Absyn_Primop_e_struct*_tmp3AF=(struct Cyc_Absyn_Primop_e_struct*)
_tmp398;if(_tmp3AF->tag != 2)goto _LL207;else{_tmp3B0=_tmp3AF->f1;_tmp3B1=_tmp3AF->f2;
if(_tmp3B1 == 0)goto _LL207;_tmp3B2=*_tmp3B1;_tmp3B3=(struct Cyc_Absyn_Exp*)_tmp3B2.hd;}}
_LL206:{void*_tmp3B8=_tmp3B3->r;void*_tmp3BA;struct Cyc_Absyn_Vardecl*_tmp3BC;
void*_tmp3BE;struct Cyc_Absyn_Vardecl*_tmp3C0;void*_tmp3C2;struct Cyc_Absyn_Vardecl*
_tmp3C4;void*_tmp3C6;struct Cyc_Absyn_Vardecl*_tmp3C8;_LL20A:{struct Cyc_Absyn_Var_e_struct*
_tmp3B9=(struct Cyc_Absyn_Var_e_struct*)_tmp3B8;if(_tmp3B9->tag != 1)goto _LL20C;
else{_tmp3BA=(void*)_tmp3B9->f2;{struct Cyc_Absyn_Global_b_struct*_tmp3BB=(struct
Cyc_Absyn_Global_b_struct*)_tmp3BA;if(_tmp3BB->tag != 1)goto _LL20C;else{_tmp3BC=
_tmp3BB->f1;}};}}if(!(!_tmp3BC->escapes))goto _LL20C;_LL20B: _tmp3C0=_tmp3BC;goto
_LL20D;_LL20C:{struct Cyc_Absyn_Var_e_struct*_tmp3BD=(struct Cyc_Absyn_Var_e_struct*)
_tmp3B8;if(_tmp3BD->tag != 1)goto _LL20E;else{_tmp3BE=(void*)_tmp3BD->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp3BF=(struct Cyc_Absyn_Local_b_struct*)_tmp3BE;if(
_tmp3BF->tag != 4)goto _LL20E;else{_tmp3C0=_tmp3BF->f1;}};}}if(!(!_tmp3C0->escapes))
goto _LL20E;_LL20D: _tmp3C4=_tmp3C0;goto _LL20F;_LL20E:{struct Cyc_Absyn_Var_e_struct*
_tmp3C1=(struct Cyc_Absyn_Var_e_struct*)_tmp3B8;if(_tmp3C1->tag != 1)goto _LL210;
else{_tmp3C2=(void*)_tmp3C1->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp3C3=(struct Cyc_Absyn_Pat_b_struct*)
_tmp3C2;if(_tmp3C3->tag != 5)goto _LL210;else{_tmp3C4=_tmp3C3->f1;}};}}if(!(!
_tmp3C4->escapes))goto _LL210;_LL20F: _tmp3C8=_tmp3C4;goto _LL211;_LL210:{struct Cyc_Absyn_Var_e_struct*
_tmp3C5=(struct Cyc_Absyn_Var_e_struct*)_tmp3B8;if(_tmp3C5->tag != 1)goto _LL212;
else{_tmp3C6=(void*)_tmp3C5->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3C7=(struct
Cyc_Absyn_Param_b_struct*)_tmp3C6;if(_tmp3C7->tag != 3)goto _LL212;else{_tmp3C8=
_tmp3C7->f1;}};}}if(!(!_tmp3C8->escapes))goto _LL212;_LL211:{struct Cyc_CfFlowInfo_Reln*
_tmp8B4;struct Cyc_List_List*_tmp8B3;new_relns=((_tmp8B3=_region_malloc(env->r,
sizeof(*_tmp8B3)),((_tmp8B3->hd=((_tmp8B4=_region_malloc(env->r,sizeof(*_tmp8B4)),((
_tmp8B4->vd=_tmp20B,((_tmp8B4->rop=Cyc_CfFlowInfo_LessNumelts(_tmp3C8),_tmp8B4)))))),((
_tmp8B3->tl=_tmp394,_tmp8B3))))));}goto _LL209;_LL212:;_LL213: goto _LL209;_LL209:;}
goto _LL1F8;_LL207:;_LL208: goto _LL1F8;_LL1F8:;}if(_tmp394 != new_relns)_tmp38E=Cyc_CfFlowInfo_ReachableFL(
_tmp393,new_relns);{void*_tmp3CB=_tmp38F;_LL215: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3CC=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3CB;if(_tmp3CC->tag != 0)goto
_LL217;}_LL216: {struct _tuple14 _tmp8B5;return(_tmp8B5.f1=_tmp38E,((_tmp8B5.f2=
_tmp173->unknown_all,_tmp8B5)));}_LL217: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3CD=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3CB;if(_tmp3CD->tag != 2)
goto _LL219;}_LL218: goto _LL21A;_LL219: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3CE=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3CB;if(_tmp3CE->tag != 1)
goto _LL21B;}_LL21A: goto _LL21C;_LL21B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3CF=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3CB;if(_tmp3CF->tag != 5)goto
_LL21D;}_LL21C: {struct Cyc_List_List _tmp8B6;struct Cyc_List_List _tmp3D1=(_tmp8B6.hd=
_tmp20B,((_tmp8B6.tl=0,_tmp8B6)));_tmp38E=Cyc_NewControlFlow_add_vars(_tmp173,
_tmp38E,(struct Cyc_List_List*)& _tmp3D1,_tmp173->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3D3;void*_tmp3D4;struct _tuple14 _tmp3D2=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp38E,_tmp20D);_tmp3D3=_tmp3D2.f1;_tmp3D4=_tmp3D2.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3D5=_tmp3D3;int _tmp3D6;struct _tuple13 _tmp3D7;struct Cyc_Dict_Dict _tmp3D8;
_LL220: if((_tmp3D5.BottomFL).tag != 1)goto _LL222;_tmp3D6=(int)(_tmp3D5.BottomFL).val;
_LL221: {struct _tuple14 _tmp8B7;return(_tmp8B7.f1=_tmp3D3,((_tmp8B7.f2=_tmp173->unknown_all,
_tmp8B7)));}_LL222: if((_tmp3D5.ReachableFL).tag != 2)goto _LL21F;_tmp3D7=(struct
_tuple13)(_tmp3D5.ReachableFL).val;_tmp3D8=_tmp3D7.f1;_LL223: if(Cyc_CfFlowInfo_initlevel(
_tmp173,_tmp3D8,_tmp3D4)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp8BA;void*_tmp8B9;(
_tmp8B9=0,Cyc_Tcutil_terr(_tmp20C->loc,((_tmp8BA="expression may not be initialized",
_tag_dyneither(_tmp8BA,sizeof(char),34))),_tag_dyneither(_tmp8B9,sizeof(void*),0)));}{
struct _tuple14 _tmp8BB;return(_tmp8BB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8BB.f2=
_tmp173->unknown_all,_tmp8BB)));};}_LL21F:;}_tmp38E=_tmp3D3;goto _LL21E;};}_LL21D:;
_LL21E: while(1){struct Cyc_List_List _tmp8BC;struct Cyc_List_List _tmp3DE=(_tmp8BC.hd=
_tmp20B,((_tmp8BC.tl=0,_tmp8BC)));_tmp38E=Cyc_NewControlFlow_add_vars(_tmp173,
_tmp38E,(struct Cyc_List_List*)& _tmp3DE,_tmp173->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3E0;void*_tmp3E1;struct _tuple14 _tmp3DF=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp38E,_tmp20D);_tmp3E0=_tmp3DF.f1;_tmp3E1=_tmp3DF.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3E2=_tmp3E0;int _tmp3E3;struct _tuple13 _tmp3E4;struct Cyc_Dict_Dict _tmp3E5;
_LL225: if((_tmp3E2.BottomFL).tag != 1)goto _LL227;_tmp3E3=(int)(_tmp3E2.BottomFL).val;
_LL226: goto _LL224;_LL227: if((_tmp3E2.ReachableFL).tag != 2)goto _LL224;_tmp3E4=(
struct _tuple13)(_tmp3E2.ReachableFL).val;_tmp3E5=_tmp3E4.f1;_LL228: if(Cyc_CfFlowInfo_initlevel(
_tmp173,_tmp3E5,_tmp3E1)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp8BF;void*_tmp8BE;(
_tmp8BE=0,Cyc_Tcutil_terr(_tmp20C->loc,((_tmp8BF="expression may not be initialized",
_tag_dyneither(_tmp8BF,sizeof(char),34))),_tag_dyneither(_tmp8BE,sizeof(void*),0)));}{
struct _tuple14 _tmp8C0;return(_tmp8C0.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8C0.f2=
_tmp173->unknown_all,_tmp8C0)));};}_LL224:;}{union Cyc_CfFlowInfo_FlowInfo _tmp3E9=
Cyc_CfFlowInfo_join_flow(_tmp173,env->all_changed,_tmp38E,_tmp3E0);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3E9,_tmp38E))break;_tmp38E=_tmp3E9;};};}{struct _tuple14 _tmp8C1;return(
_tmp8C1.f1=_tmp38E,((_tmp8C1.f2=_tmp173->unknown_all,_tmp8C1)));};_LL214:;};};
_LL1F3:;};}_LL161: {struct Cyc_Absyn_StmtExp_e_struct*_tmp20F=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp18D;if(_tmp20F->tag != 35)goto _LL163;else{_tmp210=_tmp20F->f1;}}_LL162: return
Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp210);_LL163: {struct
Cyc_Absyn_Var_e_struct*_tmp211=(struct Cyc_Absyn_Var_e_struct*)_tmp18D;if(_tmp211->tag
!= 1)goto _LL165;else{_tmp212=(void*)_tmp211->f2;{struct Cyc_Absyn_Unresolved_b_struct*
_tmp213=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp212;if(_tmp213->tag != 0)goto
_LL165;};}}_LL164: goto _LL166;_LL165: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp214=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp18D;if(_tmp214->tag != 34)
goto _LL167;}_LL166: goto _LL168;_LL167: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp215=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp18D;if(_tmp215->tag != 24)goto
_LL169;}_LL168: goto _LL16A;_LL169: {struct Cyc_Absyn_Valueof_e_struct*_tmp216=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp18D;if(_tmp216->tag != 37)goto _LL16B;}
_LL16A: goto _LL16C;_LL16B: {struct Cyc_Absyn_Asm_e_struct*_tmp217=(struct Cyc_Absyn_Asm_e_struct*)
_tmp18D;if(_tmp217->tag != 38)goto _LL106;}_LL16C: {struct Cyc_Core_Impossible_struct
_tmp8C7;const char*_tmp8C6;struct Cyc_Core_Impossible_struct*_tmp8C5;(int)_throw((
void*)((_tmp8C5=_cycalloc(sizeof(*_tmp8C5)),((_tmp8C5[0]=((_tmp8C7.tag=Cyc_Core_Impossible,((
_tmp8C7.f1=((_tmp8C6="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp8C6,
sizeof(char),31))),_tmp8C7)))),_tmp8C5)))));}_LL106:;};}static struct _tuple15 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3EF=env->fenv;void*_tmp3F0=Cyc_Tcutil_compress((
void*)_check_null(e->topt));struct Cyc_Absyn_PtrInfo _tmp3F2;void*_tmp3F3;struct
Cyc_Absyn_PtrAtts _tmp3F4;union Cyc_Absyn_Constraint*_tmp3F5;union Cyc_Absyn_Constraint*
_tmp3F6;_LL22A: {struct Cyc_Absyn_PointerType_struct*_tmp3F1=(struct Cyc_Absyn_PointerType_struct*)
_tmp3F0;if(_tmp3F1->tag != 5)goto _LL22C;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F2.elt_typ;
_tmp3F4=_tmp3F2.ptr_atts;_tmp3F5=_tmp3F4.bounds;_tmp3F6=_tmp3F4.zero_term;}}
_LL22B: {union Cyc_CfFlowInfo_FlowInfo _tmp3F7=f;int _tmp3F8;struct _tuple13 _tmp3F9;
struct Cyc_Dict_Dict _tmp3FA;struct Cyc_List_List*_tmp3FB;_LL22F: if((_tmp3F7.BottomFL).tag
!= 1)goto _LL231;_tmp3F8=(int)(_tmp3F7.BottomFL).val;_LL230: {struct _tuple15
_tmp8C8;return(_tmp8C8.f1=f,((_tmp8C8.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8C8)));}
_LL231: if((_tmp3F7.ReachableFL).tag != 2)goto _LL22E;_tmp3F9=(struct _tuple13)(
_tmp3F7.ReachableFL).val;_tmp3FA=_tmp3F9.f1;_tmp3FB=_tmp3F9.f2;_LL232: if(Cyc_Tcutil_is_bound_one(
_tmp3F5)){void*_tmp3FD=r;struct Cyc_CfFlowInfo_Place*_tmp401;struct Cyc_CfFlowInfo_Place
_tmp402;void*_tmp403;struct Cyc_List_List*_tmp404;enum Cyc_CfFlowInfo_InitLevel
_tmp407;_LL234: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp3FE=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp3FD;if(_tmp3FE->tag != 1)goto _LL236;}_LL235: goto _LL237;_LL236: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3FF=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3FD;if(_tmp3FF->tag != 2)
goto _LL238;}_LL237:{struct Cyc_CfFlowInfo_NotZero_struct _tmp8CB;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp8CA;e->annot=(void*)((_tmp8CA=_cycalloc(sizeof(*_tmp8CA)),((_tmp8CA[0]=((
_tmp8CB.tag=Cyc_CfFlowInfo_NotZero,((_tmp8CB.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3FB),_tmp8CB)))),_tmp8CA))));}goto _LL233;_LL238: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp400=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3FD;if(_tmp400->tag != 5)goto
_LL23A;else{_tmp401=_tmp400->f1;_tmp402=*_tmp401;_tmp403=_tmp402.root;_tmp404=
_tmp402.fields;}}_LL239:{struct Cyc_CfFlowInfo_NotZero_struct _tmp8CE;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp8CD;e->annot=(void*)((_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD[0]=((
_tmp8CE.tag=Cyc_CfFlowInfo_NotZero,((_tmp8CE.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3FB),_tmp8CE)))),_tmp8CD))));}{struct Cyc_CfFlowInfo_Place*_tmp8D1;struct
_tuple15 _tmp8D0;return(_tmp8D0.f1=f,((_tmp8D0.f2=Cyc_CfFlowInfo_PlaceL(((_tmp8D1=
_region_malloc(_tmp3EF->r,sizeof(*_tmp8D1)),((_tmp8D1->root=_tmp403,((_tmp8D1->fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(_tmp3EF->r,_tmp404,flds),_tmp8D1))))))),_tmp8D0)));};_LL23A: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp405=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp3FD;if(_tmp405->tag != 0)goto _LL23C;}_LL23B: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp8D2;return(_tmp8D2.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8D2.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp8D2)));};_LL23C: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp406=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3FD;if(_tmp406->tag != 3)goto
_LL23E;else{_tmp407=_tmp406->f1;}}_LL23D: f=Cyc_NewControlFlow_notzero(env,inflow,
f,e,_tmp407);goto _LL23F;_LL23E:;_LL23F: {struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp8D5;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp8D4;e->annot=(void*)((_tmp8D4=
_cycalloc(sizeof(*_tmp8D4)),((_tmp8D4[0]=((_tmp8D5.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp8D5.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3FB),_tmp8D5)))),
_tmp8D4))));}_LL233:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp8D8;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp8D7;e->annot=(void*)((_tmp8D7=_cycalloc(sizeof(*_tmp8D7)),((_tmp8D7[0]=((
_tmp8D8.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8D8.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3FB),_tmp8D8)))),_tmp8D7))));}if(Cyc_CfFlowInfo_initlevel(_tmp3EF,_tmp3FA,r)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp8DB;void*_tmp8DA;(_tmp8DA=0,Cyc_Tcutil_terr(
e->loc,((_tmp8DB="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp8DB,sizeof(char),46))),_tag_dyneither(_tmp8DA,sizeof(void*),0)));}{struct
_tuple15 _tmp8DC;return(_tmp8DC.f1=f,((_tmp8DC.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8DC)));};_LL22E:;}_LL22C:;_LL22D: {struct Cyc_Core_Impossible_struct _tmp8E2;
const char*_tmp8E1;struct Cyc_Core_Impossible_struct*_tmp8E0;(int)_throw((void*)((
_tmp8E0=_cycalloc(sizeof(*_tmp8E0)),((_tmp8E0[0]=((_tmp8E2.tag=Cyc_Core_Impossible,((
_tmp8E2.f1=((_tmp8E1="left deref of non-pointer-type",_tag_dyneither(_tmp8E1,
sizeof(char),31))),_tmp8E2)))),_tmp8E0)))));}_LL229:;}static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int
expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){struct Cyc_Dict_Dict
d;struct Cyc_CfFlowInfo_FlowEnv*_tmp419=env->fenv;{union Cyc_CfFlowInfo_FlowInfo
_tmp41A=inflow;int _tmp41B;struct _tuple13 _tmp41C;struct Cyc_Dict_Dict _tmp41D;
struct Cyc_List_List*_tmp41E;_LL241: if((_tmp41A.BottomFL).tag != 1)goto _LL243;
_tmp41B=(int)(_tmp41A.BottomFL).val;_LL242: {struct _tuple15 _tmp8E3;return(
_tmp8E3.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8E3.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8E3)));}_LL243: if((_tmp41A.ReachableFL).tag != 2)goto _LL240;_tmp41C=(struct
_tuple13)(_tmp41A.ReachableFL).val;_tmp41D=_tmp41C.f1;_tmp41E=_tmp41C.f2;_LL244:
d=_tmp41D;_LL240:;}{void*_tmp420=e->r;struct Cyc_Absyn_Exp*_tmp422;struct Cyc_Absyn_Exp*
_tmp424;struct Cyc_Absyn_Exp*_tmp426;void*_tmp428;struct Cyc_Absyn_Vardecl*_tmp42A;
void*_tmp42C;struct Cyc_Absyn_Vardecl*_tmp42E;void*_tmp430;struct Cyc_Absyn_Vardecl*
_tmp432;void*_tmp434;struct Cyc_Absyn_Vardecl*_tmp436;struct Cyc_Absyn_Exp*_tmp438;
struct _dyneither_ptr*_tmp439;struct Cyc_Absyn_Exp*_tmp43B;struct Cyc_Absyn_Exp*
_tmp43D;struct Cyc_Absyn_Exp*_tmp43E;struct Cyc_Absyn_Exp*_tmp440;struct
_dyneither_ptr*_tmp441;_LL246: {struct Cyc_Absyn_Cast_e_struct*_tmp421=(struct Cyc_Absyn_Cast_e_struct*)
_tmp420;if(_tmp421->tag != 13)goto _LL248;else{_tmp422=_tmp421->f2;}}_LL247:
_tmp424=_tmp422;goto _LL249;_LL248: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp423=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp420;if(_tmp423->tag != 11)
goto _LL24A;else{_tmp424=_tmp423->f1;}}_LL249: _tmp426=_tmp424;goto _LL24B;_LL24A: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp425=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp420;if(_tmp425->tag != 12)goto _LL24C;else{_tmp426=_tmp425->f1;}}_LL24B: return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp426,flds);_LL24C: {
struct Cyc_Absyn_Var_e_struct*_tmp427=(struct Cyc_Absyn_Var_e_struct*)_tmp420;if(
_tmp427->tag != 1)goto _LL24E;else{_tmp428=(void*)_tmp427->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp429=(struct Cyc_Absyn_Param_b_struct*)_tmp428;if(_tmp429->tag != 3)goto _LL24E;
else{_tmp42A=_tmp429->f1;}};}}_LL24D: _tmp42E=_tmp42A;goto _LL24F;_LL24E: {struct
Cyc_Absyn_Var_e_struct*_tmp42B=(struct Cyc_Absyn_Var_e_struct*)_tmp420;if(_tmp42B->tag
!= 1)goto _LL250;else{_tmp42C=(void*)_tmp42B->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp42D=(struct Cyc_Absyn_Local_b_struct*)_tmp42C;if(_tmp42D->tag != 4)goto _LL250;
else{_tmp42E=_tmp42D->f1;}};}}_LL24F: _tmp432=_tmp42E;goto _LL251;_LL250: {struct
Cyc_Absyn_Var_e_struct*_tmp42F=(struct Cyc_Absyn_Var_e_struct*)_tmp420;if(_tmp42F->tag
!= 1)goto _LL252;else{_tmp430=(void*)_tmp42F->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp431=(struct Cyc_Absyn_Pat_b_struct*)_tmp430;if(_tmp431->tag != 5)goto _LL252;
else{_tmp432=_tmp431->f1;}};}}_LL251: {struct Cyc_CfFlowInfo_Place*_tmp8ED;struct
Cyc_CfFlowInfo_VarRoot_struct _tmp8EC;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp8EB;
struct _tuple15 _tmp8EA;return(_tmp8EA.f1=inflow,((_tmp8EA.f2=Cyc_CfFlowInfo_PlaceL(((
_tmp8ED=_region_malloc(env->r,sizeof(*_tmp8ED)),((_tmp8ED->root=(void*)((_tmp8EB=
_region_malloc(env->r,sizeof(*_tmp8EB)),((_tmp8EB[0]=((_tmp8EC.tag=0,((_tmp8EC.f1=
_tmp432,_tmp8EC)))),_tmp8EB)))),((_tmp8ED->fields=flds,_tmp8ED))))))),_tmp8EA)));}
_LL252: {struct Cyc_Absyn_Var_e_struct*_tmp433=(struct Cyc_Absyn_Var_e_struct*)
_tmp420;if(_tmp433->tag != 1)goto _LL254;else{_tmp434=(void*)_tmp433->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp435=(struct Cyc_Absyn_Global_b_struct*)_tmp434;if(
_tmp435->tag != 1)goto _LL254;else{_tmp436=_tmp435->f1;}};}}_LL253: {struct
_tuple15 _tmp8EE;return(_tmp8EE.f1=inflow,((_tmp8EE.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8EE)));}_LL254: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp437=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp420;if(_tmp437->tag != 21)goto _LL256;else{_tmp438=_tmp437->f1;_tmp439=_tmp437->f2;}}
_LL255:{void*_tmp447=Cyc_Tcutil_compress((void*)_check_null(_tmp438->topt));
struct Cyc_Absyn_PtrInfo _tmp449;void*_tmp44A;_LL25F: {struct Cyc_Absyn_PointerType_struct*
_tmp448=(struct Cyc_Absyn_PointerType_struct*)_tmp447;if(_tmp448->tag != 5)goto
_LL261;else{_tmp449=_tmp448->f1;_tmp44A=_tmp449.elt_typ;}}_LL260: if(!Cyc_Absyn_is_nontagged_union_type(
_tmp44A)){struct Cyc_List_List*_tmp8EF;flds=((_tmp8EF=_region_malloc(env->r,
sizeof(*_tmp8EF)),((_tmp8EF->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp44A,
_tmp439),((_tmp8EF->tl=flds,_tmp8EF))))));}goto _LL25E;_LL261:;_LL262: {struct Cyc_Core_Impossible_struct
_tmp8F5;const char*_tmp8F4;struct Cyc_Core_Impossible_struct*_tmp8F3;(int)_throw((
void*)((_tmp8F3=_cycalloc(sizeof(*_tmp8F3)),((_tmp8F3[0]=((_tmp8F5.tag=Cyc_Core_Impossible,((
_tmp8F5.f1=((_tmp8F4="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp8F4,sizeof(
char),25))),_tmp8F5)))),_tmp8F3)))));}_LL25E:;}_tmp43B=_tmp438;goto _LL257;_LL256: {
struct Cyc_Absyn_Deref_e_struct*_tmp43A=(struct Cyc_Absyn_Deref_e_struct*)_tmp420;
if(_tmp43A->tag != 19)goto _LL258;else{_tmp43B=_tmp43A->f1;}}_LL257: if(
expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp43B->topt))){
union Cyc_CfFlowInfo_FlowInfo _tmp450;union Cyc_CfFlowInfo_AbsLVal _tmp451;struct
_tuple15 _tmp44F=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp43B);_tmp450=
_tmp44F.f1;_tmp451=_tmp44F.f2;{struct _tuple15 _tmp8F6;struct _tuple15 _tmp453=(
_tmp8F6.f1=_tmp450,((_tmp8F6.f2=_tmp451,_tmp8F6)));union Cyc_CfFlowInfo_FlowInfo
_tmp454;struct _tuple13 _tmp455;struct Cyc_Dict_Dict _tmp456;struct Cyc_List_List*
_tmp457;union Cyc_CfFlowInfo_AbsLVal _tmp458;struct Cyc_CfFlowInfo_Place*_tmp459;
_LL264: _tmp454=_tmp453.f1;if((_tmp454.ReachableFL).tag != 2)goto _LL266;_tmp455=(
struct _tuple13)(_tmp454.ReachableFL).val;_tmp456=_tmp455.f1;_tmp457=_tmp455.f2;
_tmp458=_tmp453.f2;if((_tmp458.PlaceL).tag != 1)goto _LL266;_tmp459=(struct Cyc_CfFlowInfo_Place*)(
_tmp458.PlaceL).val;_LL265: {void*_tmp45A=Cyc_CfFlowInfo_lookup_place(_tmp456,
_tmp459);void*_tmp45B=_tmp45A;_LL269: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp45C=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp45B;if(_tmp45C->tag != 5)goto
_LL26B;}_LL26A: return Cyc_NewControlFlow_anal_derefL(env,_tmp450,_tmp43B,_tmp450,
_tmp45A,flds);_LL26B:;_LL26C: {enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(
_tmp419,_tmp456,_tmp45A);void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp419->unknown_all:
_tmp419->unknown_none;void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp419,Cyc_Tcutil_pointer_elt_type((
void*)_check_null(_tmp43B->topt)),0,leaf);struct Cyc_CfFlowInfo_MallocPt_struct
_tmp8F9;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp8F8;void*new_root=(void*)((
_tmp8F8=_region_malloc(_tmp419->r,sizeof(*_tmp8F8)),((_tmp8F8[0]=((_tmp8F9.tag=1,((
_tmp8F9.f1=e,((_tmp8F9.f2=(void*)((void*)_check_null(e->topt)),_tmp8F9)))))),
_tmp8F8))));struct Cyc_CfFlowInfo_Place*_tmp8FA;struct Cyc_CfFlowInfo_Place*place=(
_tmp8FA=_region_malloc(_tmp419->r,sizeof(*_tmp8FA)),((_tmp8FA->root=new_root,((
_tmp8FA->fields=0,_tmp8FA)))));struct Cyc_CfFlowInfo_AddressOf_struct _tmp8FD;
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp8FC;void*res=(void*)((_tmp8FC=
_region_malloc(_tmp419->r,sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FD.tag=5,((
_tmp8FD.f1=place,_tmp8FD)))),_tmp8FC))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);_tmp456=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))
Cyc_Dict_insert)(_tmp456,new_root,new_rval);_tmp456=Cyc_CfFlowInfo_assign_place(
_tmp419,e->loc,_tmp456,env->all_changed,_tmp459,res);{union Cyc_CfFlowInfo_FlowInfo
_tmp45D=Cyc_CfFlowInfo_ReachableFL(_tmp456,_tmp457);return Cyc_NewControlFlow_anal_derefL(
env,_tmp45D,_tmp43B,_tmp45D,res,flds);};}_LL268:;}_LL266:;_LL267: goto _LL263;
_LL263:;};}{union Cyc_CfFlowInfo_FlowInfo _tmp464;void*_tmp465;struct _tuple14
_tmp463=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp43B);_tmp464=_tmp463.f1;
_tmp465=_tmp463.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp43B,
_tmp464,_tmp465,flds);};_LL258: {struct Cyc_Absyn_Subscript_e_struct*_tmp43C=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp420;if(_tmp43C->tag != 22)goto _LL25A;
else{_tmp43D=_tmp43C->f1;_tmp43E=_tmp43C->f2;}}_LL259: {void*_tmp466=Cyc_Tcutil_compress((
void*)_check_null(_tmp43D->topt));struct Cyc_Absyn_PtrInfo _tmp469;struct Cyc_Absyn_PtrAtts
_tmp46A;union Cyc_Absyn_Constraint*_tmp46B;_LL26E: {struct Cyc_Absyn_TupleType_struct*
_tmp467=(struct Cyc_Absyn_TupleType_struct*)_tmp466;if(_tmp467->tag != 10)goto
_LL270;}_LL26F: {unsigned int _tmp46C=(Cyc_Evexp_eval_const_uint_exp(_tmp43E)).f1;
struct Cyc_List_List*_tmp8FE;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,
expand_unique,_tmp43D,((_tmp8FE=_region_malloc(env->r,sizeof(*_tmp8FE)),((
_tmp8FE->hd=(void*)((int)_tmp46C),((_tmp8FE->tl=flds,_tmp8FE)))))));}_LL270: {
struct Cyc_Absyn_PointerType_struct*_tmp468=(struct Cyc_Absyn_PointerType_struct*)
_tmp466;if(_tmp468->tag != 5)goto _LL272;else{_tmp469=_tmp468->f1;_tmp46A=_tmp469.ptr_atts;
_tmp46B=_tmp46A.bounds;}}_LL271: {struct _RegionHandle _tmp46E=_new_region("temp");
struct _RegionHandle*temp=& _tmp46E;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo
_tmp471;struct Cyc_List_List*_tmp472;struct Cyc_Absyn_Exp*_tmp8FF[2];struct
_tuple20 _tmp470=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((
_tmp8FF[1]=_tmp43E,((_tmp8FF[0]=_tmp43D,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp8FF,
sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp471=_tmp470.f1;_tmp472=_tmp470.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp473=_tmp471;{union Cyc_CfFlowInfo_FlowInfo _tmp474=
_tmp471;struct _tuple13 _tmp475;struct Cyc_Dict_Dict _tmp476;struct Cyc_List_List*
_tmp477;_LL275: if((_tmp474.ReachableFL).tag != 2)goto _LL277;_tmp475=(struct
_tuple13)(_tmp474.ReachableFL).val;_tmp476=_tmp475.f1;_tmp477=_tmp475.f2;_LL276:
if(Cyc_CfFlowInfo_initlevel(_tmp419,_tmp476,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp472))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp902;void*_tmp901;(_tmp901=0,Cyc_Tcutil_terr(_tmp43E->loc,((_tmp902="expression may not be initialized",
_tag_dyneither(_tmp902,sizeof(char),34))),_tag_dyneither(_tmp901,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp47A=Cyc_NewControlFlow_add_subscript_reln(_tmp419->r,
_tmp477,_tmp43D,_tmp43E);if(_tmp477 != _tmp47A)_tmp473=Cyc_CfFlowInfo_ReachableFL(
_tmp476,_tmp47A);goto _LL274;};_LL277:;_LL278: goto _LL274;_LL274:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp47C;union Cyc_CfFlowInfo_AbsLVal _tmp47D;struct _tuple15 _tmp47B=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp43D,_tmp471,(void*)((struct Cyc_List_List*)_check_null(_tmp472))->hd,
flds);_tmp47C=_tmp47B.f1;_tmp47D=_tmp47B.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp47E=
_tmp47C;int _tmp47F;_LL27A: if((_tmp47E.BottomFL).tag != 1)goto _LL27C;_tmp47F=(int)(
_tmp47E.BottomFL).val;_LL27B: {struct _tuple15 _tmp903;struct _tuple15 _tmp481=(
_tmp903.f1=_tmp47C,((_tmp903.f2=_tmp47D,_tmp903)));_npop_handler(0);return
_tmp481;}_LL27C:;_LL27D: {struct _tuple15 _tmp904;struct _tuple15 _tmp483=(_tmp904.f1=
_tmp473,((_tmp904.f2=_tmp47D,_tmp904)));_npop_handler(0);return _tmp483;}_LL279:;};};};};
_pop_region(temp);}_LL272:;_LL273: {struct Cyc_Core_Impossible_struct _tmp90A;
const char*_tmp909;struct Cyc_Core_Impossible_struct*_tmp908;(int)_throw((void*)((
_tmp908=_cycalloc(sizeof(*_tmp908)),((_tmp908[0]=((_tmp90A.tag=Cyc_Core_Impossible,((
_tmp90A.f1=((_tmp909="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp909,
sizeof(char),33))),_tmp90A)))),_tmp908)))));}_LL26D:;}_LL25A: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp43F=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp420;if(_tmp43F->tag != 20)goto
_LL25C;else{_tmp440=_tmp43F->f1;_tmp441=_tmp43F->f2;}}_LL25B: if(Cyc_Absyn_is_nontagged_union_type((
void*)_check_null(_tmp440->topt))){struct _tuple15 _tmp90B;return(_tmp90B.f1=
inflow,((_tmp90B.f2=Cyc_CfFlowInfo_UnknownL(),_tmp90B)));}{struct Cyc_List_List*
_tmp90C;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp440,((
_tmp90C=_region_malloc(env->r,sizeof(*_tmp90C)),((_tmp90C->hd=(void*)Cyc_CfFlowInfo_get_field_index((
void*)_check_null(_tmp440->topt),_tmp441),((_tmp90C->tl=flds,_tmp90C)))))));};
_LL25C:;_LL25D: {struct _tuple15 _tmp90D;return(_tmp90D.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp90D.f2=Cyc_CfFlowInfo_UnknownL(),_tmp90D)));}_LL245:;};}static struct _tuple15
Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,int expand_unique,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo
_tmp48B;union Cyc_CfFlowInfo_AbsLVal _tmp48C;struct _tuple15 _tmp48A=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,e,0);_tmp48B=_tmp48A.f1;_tmp48C=_tmp48A.f2;{struct
_tuple15 _tmp90E;return(_tmp90E.f1=_tmp48B,((_tmp90E.f2=_tmp48C,_tmp90E)));};}
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp48E=env->fenv;{void*_tmp48F=e->r;struct Cyc_Absyn_Exp*_tmp491;struct Cyc_Absyn_Exp*
_tmp492;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Absyn_Exp*
_tmp496;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Exp*
_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;enum Cyc_Absyn_Primop _tmp49E;struct Cyc_List_List*
_tmp49F;struct Cyc_List_List _tmp4A0;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_List_List*
_tmp4A2;enum Cyc_Absyn_Primop _tmp4A4;struct Cyc_List_List*_tmp4A5;_LL27F: {struct
Cyc_Absyn_Conditional_e_struct*_tmp490=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp48F;if(_tmp490->tag != 5)goto _LL281;else{_tmp491=_tmp490->f1;_tmp492=_tmp490->f2;
_tmp493=_tmp490->f3;}}_LL280: {union Cyc_CfFlowInfo_FlowInfo _tmp4A7;union Cyc_CfFlowInfo_FlowInfo
_tmp4A8;struct _tuple16 _tmp4A6=Cyc_NewControlFlow_anal_test(env,inflow,_tmp491);
_tmp4A7=_tmp4A6.f1;_tmp4A8=_tmp4A6.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp4AA;union
Cyc_CfFlowInfo_FlowInfo _tmp4AB;struct _tuple16 _tmp4A9=Cyc_NewControlFlow_anal_test(
env,_tmp4A7,_tmp492);_tmp4AA=_tmp4A9.f1;_tmp4AB=_tmp4A9.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4AD;union Cyc_CfFlowInfo_FlowInfo _tmp4AE;struct _tuple16 _tmp4AC=Cyc_NewControlFlow_anal_test(
env,_tmp4A8,_tmp493);_tmp4AD=_tmp4AC.f1;_tmp4AE=_tmp4AC.f2;{struct _tuple16
_tmp90F;return(_tmp90F.f1=Cyc_CfFlowInfo_join_flow(_tmp48E,env->all_changed,
_tmp4AA,_tmp4AD),((_tmp90F.f2=Cyc_CfFlowInfo_join_flow(_tmp48E,env->all_changed,
_tmp4AB,_tmp4AE),_tmp90F)));};};};}_LL281: {struct Cyc_Absyn_And_e_struct*_tmp494=(
struct Cyc_Absyn_And_e_struct*)_tmp48F;if(_tmp494->tag != 6)goto _LL283;else{
_tmp495=_tmp494->f1;_tmp496=_tmp494->f2;}}_LL282: {union Cyc_CfFlowInfo_FlowInfo
_tmp4B1;union Cyc_CfFlowInfo_FlowInfo _tmp4B2;struct _tuple16 _tmp4B0=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp495);_tmp4B1=_tmp4B0.f1;_tmp4B2=_tmp4B0.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4B4;union Cyc_CfFlowInfo_FlowInfo _tmp4B5;struct _tuple16 _tmp4B3=Cyc_NewControlFlow_anal_test(
env,_tmp4B1,_tmp496);_tmp4B4=_tmp4B3.f1;_tmp4B5=_tmp4B3.f2;{struct _tuple16
_tmp910;return(_tmp910.f1=_tmp4B4,((_tmp910.f2=Cyc_CfFlowInfo_join_flow(_tmp48E,
env->all_changed,_tmp4B2,_tmp4B5),_tmp910)));};};}_LL283: {struct Cyc_Absyn_Or_e_struct*
_tmp497=(struct Cyc_Absyn_Or_e_struct*)_tmp48F;if(_tmp497->tag != 7)goto _LL285;
else{_tmp498=_tmp497->f1;_tmp499=_tmp497->f2;}}_LL284: {union Cyc_CfFlowInfo_FlowInfo
_tmp4B8;union Cyc_CfFlowInfo_FlowInfo _tmp4B9;struct _tuple16 _tmp4B7=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp498);_tmp4B8=_tmp4B7.f1;_tmp4B9=_tmp4B7.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4BB;union Cyc_CfFlowInfo_FlowInfo _tmp4BC;struct _tuple16 _tmp4BA=Cyc_NewControlFlow_anal_test(
env,_tmp4B9,_tmp499);_tmp4BB=_tmp4BA.f1;_tmp4BC=_tmp4BA.f2;{struct _tuple16
_tmp911;return(_tmp911.f1=Cyc_CfFlowInfo_join_flow(_tmp48E,env->all_changed,
_tmp4B8,_tmp4BB),((_tmp911.f2=_tmp4BC,_tmp911)));};};}_LL285: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp49A=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp48F;if(_tmp49A->tag != 8)goto _LL287;
else{_tmp49B=_tmp49A->f1;_tmp49C=_tmp49A->f2;}}_LL286: {union Cyc_CfFlowInfo_FlowInfo
_tmp4BF;void*_tmp4C0;struct _tuple14 _tmp4BE=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp49B);_tmp4BF=_tmp4BE.f1;_tmp4C0=_tmp4BE.f2;return Cyc_NewControlFlow_anal_test(
env,_tmp4BF,_tmp49C);}_LL287: {struct Cyc_Absyn_Primop_e_struct*_tmp49D=(struct
Cyc_Absyn_Primop_e_struct*)_tmp48F;if(_tmp49D->tag != 2)goto _LL289;else{_tmp49E=
_tmp49D->f1;if(_tmp49E != Cyc_Absyn_Not)goto _LL289;_tmp49F=_tmp49D->f2;if(_tmp49F
== 0)goto _LL289;_tmp4A0=*_tmp49F;_tmp4A1=(struct Cyc_Absyn_Exp*)_tmp4A0.hd;
_tmp4A2=_tmp4A0.tl;if(_tmp4A2 != 0)goto _LL289;}}_LL288: {union Cyc_CfFlowInfo_FlowInfo
_tmp4C2;union Cyc_CfFlowInfo_FlowInfo _tmp4C3;struct _tuple16 _tmp4C1=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp4A1);_tmp4C2=_tmp4C1.f1;_tmp4C3=_tmp4C1.f2;{struct _tuple16 _tmp912;
return(_tmp912.f1=_tmp4C3,((_tmp912.f2=_tmp4C2,_tmp912)));};}_LL289: {struct Cyc_Absyn_Primop_e_struct*
_tmp4A3=(struct Cyc_Absyn_Primop_e_struct*)_tmp48F;if(_tmp4A3->tag != 2)goto _LL28B;
else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A3->f2;}}_LL28A: {void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo
f;struct _RegionHandle _tmp4C5=_new_region("temp");struct _RegionHandle*temp=&
_tmp4C5;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo _tmp4C7;struct Cyc_List_List*
_tmp4C8;struct _tuple20 _tmp4C6=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,_tmp4A5,0,0);_tmp4C7=_tmp4C6.f1;_tmp4C8=_tmp4C6.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp4C8))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp4C8->tl))->hd;
f=_tmp4C7;}{union Cyc_CfFlowInfo_FlowInfo _tmp4C9=f;int _tmp4CA;struct _tuple13
_tmp4CB;struct Cyc_Dict_Dict _tmp4CC;struct Cyc_List_List*_tmp4CD;_LL28E: if((
_tmp4C9.BottomFL).tag != 1)goto _LL290;_tmp4CA=(int)(_tmp4C9.BottomFL).val;_LL28F: {
struct _tuple16 _tmp913;struct _tuple16 _tmp4CF=(_tmp913.f1=f,((_tmp913.f2=f,_tmp913)));
_npop_handler(0);return _tmp4CF;}_LL290: if((_tmp4C9.ReachableFL).tag != 2)goto
_LL28D;_tmp4CB=(struct _tuple13)(_tmp4C9.ReachableFL).val;_tmp4CC=_tmp4CB.f1;
_tmp4CD=_tmp4CB.f2;_LL291: {struct Cyc_Absyn_Exp*_tmp4D0=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A5))->hd;struct Cyc_Absyn_Exp*_tmp4D1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4A5->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp4CC,r1)== Cyc_CfFlowInfo_NoneIL){const char*_tmp916;void*_tmp915;(
_tmp915=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp4A5->hd)->loc,((_tmp916="expression may not be initialized",
_tag_dyneither(_tmp916,sizeof(char),34))),_tag_dyneither(_tmp915,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4CC,r2)== Cyc_CfFlowInfo_NoneIL){const
char*_tmp919;void*_tmp918;(_tmp918=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4A5->tl))->hd)->loc,((_tmp919="expression may not be initialized",
_tag_dyneither(_tmp919,sizeof(char),34))),_tag_dyneither(_tmp918,sizeof(void*),0)));}
if(_tmp4A4 == Cyc_Absyn_Eq  || _tmp4A4 == Cyc_Absyn_Neq){struct _tuple0 _tmp91A;
struct _tuple0 _tmp4D7=(_tmp91A.f1=r1,((_tmp91A.f2=r2,_tmp91A)));void*_tmp4D8;
enum Cyc_CfFlowInfo_InitLevel _tmp4DA;void*_tmp4DB;void*_tmp4DD;void*_tmp4DF;
enum Cyc_CfFlowInfo_InitLevel _tmp4E1;void*_tmp4E2;void*_tmp4E4;void*_tmp4E6;void*
_tmp4E8;void*_tmp4EA;void*_tmp4EC;void*_tmp4EE;void*_tmp4F0;void*_tmp4F2;void*
_tmp4F4;void*_tmp4F6;void*_tmp4F8;void*_tmp4FA;void*_tmp4FC;_LL293: _tmp4D8=
_tmp4D7.f1;{struct Cyc_CfFlowInfo_UnknownR_struct*_tmp4D9=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4D8;if(_tmp4D9->tag != 3)goto _LL295;else{_tmp4DA=_tmp4D9->f1;}};_tmp4DB=
_tmp4D7.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4DC=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4DB;if(_tmp4DC->tag != 0)goto _LL295;};_LL294: {union Cyc_CfFlowInfo_FlowInfo
_tmp4FF;union Cyc_CfFlowInfo_FlowInfo _tmp500;struct _tuple16 _tmp4FE=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4D0,_tmp4DA);_tmp4FF=_tmp4FE.f1;_tmp500=_tmp4FE.f2;switch(_tmp4A4){
case Cyc_Absyn_Eq: _LL2A7: {struct _tuple16 _tmp91B;struct _tuple16 _tmp502=(_tmp91B.f1=
_tmp500,((_tmp91B.f2=_tmp4FF,_tmp91B)));_npop_handler(0);return _tmp502;}case Cyc_Absyn_Neq:
_LL2A8: {struct _tuple16 _tmp91C;struct _tuple16 _tmp504=(_tmp91C.f1=_tmp4FF,((
_tmp91C.f2=_tmp500,_tmp91C)));_npop_handler(0);return _tmp504;}default: _LL2A9: {
struct Cyc_Core_Impossible_struct _tmp922;const char*_tmp921;struct Cyc_Core_Impossible_struct*
_tmp920;(int)_throw((void*)((_tmp920=_cycalloc(sizeof(*_tmp920)),((_tmp920[0]=((
_tmp922.tag=Cyc_Core_Impossible,((_tmp922.f1=((_tmp921="anal_test, zero-split",
_tag_dyneither(_tmp921,sizeof(char),22))),_tmp922)))),_tmp920)))));}}}_LL295:
_tmp4DD=_tmp4D7.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4DE=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4DD;if(_tmp4DE->tag != 0)goto _LL297;};_tmp4DF=_tmp4D7.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4E0=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4DF;if(_tmp4E0->tag != 3)goto
_LL297;else{_tmp4E1=_tmp4E0->f1;}};_LL296: {union Cyc_CfFlowInfo_FlowInfo _tmp509;
union Cyc_CfFlowInfo_FlowInfo _tmp50A;struct _tuple16 _tmp508=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4D1,_tmp4E1);_tmp509=_tmp508.f1;_tmp50A=_tmp508.f2;switch(_tmp4A4){
case Cyc_Absyn_Eq: _LL2AB: {struct _tuple16 _tmp923;struct _tuple16 _tmp50C=(_tmp923.f1=
_tmp50A,((_tmp923.f2=_tmp509,_tmp923)));_npop_handler(0);return _tmp50C;}case Cyc_Absyn_Neq:
_LL2AC: {struct _tuple16 _tmp924;struct _tuple16 _tmp50E=(_tmp924.f1=_tmp509,((
_tmp924.f2=_tmp50A,_tmp924)));_npop_handler(0);return _tmp50E;}default: _LL2AD: {
struct Cyc_Core_Impossible_struct _tmp92A;const char*_tmp929;struct Cyc_Core_Impossible_struct*
_tmp928;(int)_throw((void*)((_tmp928=_cycalloc(sizeof(*_tmp928)),((_tmp928[0]=((
_tmp92A.tag=Cyc_Core_Impossible,((_tmp92A.f1=((_tmp929="anal_test, zero-split",
_tag_dyneither(_tmp929,sizeof(char),22))),_tmp92A)))),_tmp928)))));}}}_LL297:
_tmp4E2=_tmp4D7.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4E3=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4E2;if(_tmp4E3->tag != 0)goto _LL299;};_tmp4E4=_tmp4D7.f2;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4E5=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4E4;if(_tmp4E5->tag != 0)goto
_LL299;};_LL298: if(_tmp4A4 == Cyc_Absyn_Eq){struct _tuple16 _tmp92B;struct _tuple16
_tmp513=(_tmp92B.f1=f,((_tmp92B.f2=Cyc_CfFlowInfo_BottomFL(),_tmp92B)));
_npop_handler(0);return _tmp513;}else{struct _tuple16 _tmp92C;struct _tuple16 _tmp515=(
_tmp92C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp92C.f2=f,_tmp92C)));_npop_handler(0);
return _tmp515;}_LL299: _tmp4E6=_tmp4D7.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4E7=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4E6;if(_tmp4E7->tag != 0)goto
_LL29B;};_tmp4E8=_tmp4D7.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp4E9=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4E8;if(_tmp4E9->tag != 1)goto _LL29B;};
_LL29A: goto _LL29C;_LL29B: _tmp4EA=_tmp4D7.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4EB=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4EA;if(_tmp4EB->tag != 0)goto
_LL29D;};_tmp4EC=_tmp4D7.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp4ED=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4EC;if(_tmp4ED->tag != 2)goto _LL29D;};
_LL29C: goto _LL29E;_LL29D: _tmp4EE=_tmp4D7.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4EF=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4EE;if(_tmp4EF->tag != 0)goto
_LL29F;};_tmp4F0=_tmp4D7.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp4F1=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4F0;if(_tmp4F1->tag != 5)goto _LL29F;};
_LL29E: goto _LL2A0;_LL29F: _tmp4F2=_tmp4D7.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4F3=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4F2;if(_tmp4F3->tag != 1)
goto _LL2A1;};_tmp4F4=_tmp4D7.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4F5=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4F4;if(_tmp4F5->tag != 0)goto _LL2A1;};
_LL2A0: goto _LL2A2;_LL2A1: _tmp4F6=_tmp4D7.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4F7=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4F6;if(_tmp4F7->tag != 2)
goto _LL2A3;};_tmp4F8=_tmp4D7.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4F9=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4F8;if(_tmp4F9->tag != 0)goto _LL2A3;};
_LL2A2: goto _LL2A4;_LL2A3: _tmp4FA=_tmp4D7.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp4FB=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4FA;if(_tmp4FB->tag != 5)goto
_LL2A5;};_tmp4FC=_tmp4D7.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4FD=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4FC;if(_tmp4FD->tag != 0)goto _LL2A5;};_LL2A4: if(_tmp4A4 == Cyc_Absyn_Neq){
struct _tuple16 _tmp92D;struct _tuple16 _tmp517=(_tmp92D.f1=f,((_tmp92D.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp92D)));_npop_handler(0);return _tmp517;}else{struct _tuple16 _tmp92E;struct
_tuple16 _tmp519=(_tmp92E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp92E.f2=f,_tmp92E)));
_npop_handler(0);return _tmp519;}_LL2A5:;_LL2A6: goto _LL292;_LL292:;}{struct
_tuple0 _tmp92F;struct _tuple0 _tmp51B=(_tmp92F.f1=Cyc_Tcutil_compress((void*)
_check_null(_tmp4D0->topt)),((_tmp92F.f2=Cyc_Tcutil_compress((void*)_check_null(
_tmp4D1->topt)),_tmp92F)));void*_tmp51C;enum Cyc_Absyn_Sign _tmp51E;void*_tmp51F;
enum Cyc_Absyn_Sign _tmp521;void*_tmp522;void*_tmp524;_LL2B0: _tmp51C=_tmp51B.f1;{
struct Cyc_Absyn_IntType_struct*_tmp51D=(struct Cyc_Absyn_IntType_struct*)_tmp51C;
if(_tmp51D->tag != 6)goto _LL2B2;else{_tmp51E=_tmp51D->f1;if(_tmp51E != Cyc_Absyn_Unsigned)
goto _LL2B2;}};_LL2B1: goto _LL2B3;_LL2B2: _tmp51F=_tmp51B.f2;{struct Cyc_Absyn_IntType_struct*
_tmp520=(struct Cyc_Absyn_IntType_struct*)_tmp51F;if(_tmp520->tag != 6)goto _LL2B4;
else{_tmp521=_tmp520->f1;if(_tmp521 != Cyc_Absyn_Unsigned)goto _LL2B4;}};_LL2B3:
goto _LL2B5;_LL2B4: _tmp522=_tmp51B.f1;{struct Cyc_Absyn_TagType_struct*_tmp523=(
struct Cyc_Absyn_TagType_struct*)_tmp522;if(_tmp523->tag != 19)goto _LL2B6;};_LL2B5:
goto _LL2B7;_LL2B6: _tmp524=_tmp51B.f2;{struct Cyc_Absyn_TagType_struct*_tmp525=(
struct Cyc_Absyn_TagType_struct*)_tmp524;if(_tmp525->tag != 19)goto _LL2B8;};_LL2B7:
goto _LL2AF;_LL2B8:;_LL2B9: {struct _tuple16 _tmp930;struct _tuple16 _tmp527=(_tmp930.f1=
f,((_tmp930.f2=f,_tmp930)));_npop_handler(0);return _tmp527;}_LL2AF:;}switch(
_tmp4A4){case Cyc_Absyn_Eq: _LL2BA: goto _LL2BB;case Cyc_Absyn_Neq: _LL2BB: goto _LL2BC;
case Cyc_Absyn_Gt: _LL2BC: goto _LL2BD;case Cyc_Absyn_Gte: _LL2BD: {struct _tuple16
_tmp931;struct _tuple16 _tmp529=(_tmp931.f1=f,((_tmp931.f2=f,_tmp931)));
_npop_handler(0);return _tmp529;}case Cyc_Absyn_Lt: _LL2BE: {union Cyc_CfFlowInfo_FlowInfo
_tmp52A=f;union Cyc_CfFlowInfo_FlowInfo _tmp52B=f;{union Cyc_CfFlowInfo_FlowInfo
_tmp52C=_tmp52A;int _tmp52D;struct _tuple13 _tmp52E;struct Cyc_Dict_Dict _tmp52F;
_LL2C1: if((_tmp52C.BottomFL).tag != 1)goto _LL2C3;_tmp52D=(int)(_tmp52C.BottomFL).val;
_LL2C2: {struct Cyc_Core_Impossible_struct _tmp937;const char*_tmp936;struct Cyc_Core_Impossible_struct*
_tmp935;(int)_throw((void*)((_tmp935=_cycalloc(sizeof(*_tmp935)),((_tmp935[0]=((
_tmp937.tag=Cyc_Core_Impossible,((_tmp937.f1=((_tmp936="anal_test, Lt",
_tag_dyneither(_tmp936,sizeof(char),14))),_tmp937)))),_tmp935)))));}_LL2C3: if((
_tmp52C.ReachableFL).tag != 2)goto _LL2C0;_tmp52E=(struct _tuple13)(_tmp52C.ReachableFL).val;
_tmp52F=_tmp52E.f1;_LL2C4: _tmp4CC=_tmp52F;_LL2C0:;}{void*_tmp533=_tmp4D0->r;void*
_tmp535;struct Cyc_Absyn_Vardecl*_tmp537;void*_tmp539;struct Cyc_Absyn_Vardecl*
_tmp53B;void*_tmp53D;struct Cyc_Absyn_Vardecl*_tmp53F;void*_tmp541;struct Cyc_Absyn_Vardecl*
_tmp543;_LL2C6:{struct Cyc_Absyn_Var_e_struct*_tmp534=(struct Cyc_Absyn_Var_e_struct*)
_tmp533;if(_tmp534->tag != 1)goto _LL2C8;else{_tmp535=(void*)_tmp534->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp536=(struct Cyc_Absyn_Global_b_struct*)_tmp535;if(
_tmp536->tag != 1)goto _LL2C8;else{_tmp537=_tmp536->f1;}};}}if(!(!_tmp537->escapes))
goto _LL2C8;_LL2C7: _tmp53B=_tmp537;goto _LL2C9;_LL2C8:{struct Cyc_Absyn_Var_e_struct*
_tmp538=(struct Cyc_Absyn_Var_e_struct*)_tmp533;if(_tmp538->tag != 1)goto _LL2CA;
else{_tmp539=(void*)_tmp538->f2;{struct Cyc_Absyn_Local_b_struct*_tmp53A=(struct
Cyc_Absyn_Local_b_struct*)_tmp539;if(_tmp53A->tag != 4)goto _LL2CA;else{_tmp53B=
_tmp53A->f1;}};}}if(!(!_tmp53B->escapes))goto _LL2CA;_LL2C9: _tmp53F=_tmp53B;goto
_LL2CB;_LL2CA:{struct Cyc_Absyn_Var_e_struct*_tmp53C=(struct Cyc_Absyn_Var_e_struct*)
_tmp533;if(_tmp53C->tag != 1)goto _LL2CC;else{_tmp53D=(void*)_tmp53C->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp53E=(struct Cyc_Absyn_Pat_b_struct*)_tmp53D;if(_tmp53E->tag
!= 5)goto _LL2CC;else{_tmp53F=_tmp53E->f1;}};}}if(!(!_tmp53F->escapes))goto _LL2CC;
_LL2CB: _tmp543=_tmp53F;goto _LL2CD;_LL2CC:{struct Cyc_Absyn_Var_e_struct*_tmp540=(
struct Cyc_Absyn_Var_e_struct*)_tmp533;if(_tmp540->tag != 1)goto _LL2CE;else{
_tmp541=(void*)_tmp540->f2;{struct Cyc_Absyn_Param_b_struct*_tmp542=(struct Cyc_Absyn_Param_b_struct*)
_tmp541;if(_tmp542->tag != 3)goto _LL2CE;else{_tmp543=_tmp542->f1;}};}}if(!(!
_tmp543->escapes))goto _LL2CE;_LL2CD: {void*_tmp544=_tmp4D1->r;void*_tmp546;
struct Cyc_Absyn_Vardecl*_tmp548;void*_tmp54A;struct Cyc_Absyn_Vardecl*_tmp54C;
void*_tmp54E;struct Cyc_Absyn_Vardecl*_tmp550;void*_tmp552;struct Cyc_Absyn_Vardecl*
_tmp554;union Cyc_Absyn_Cnst _tmp556;struct _tuple6 _tmp557;int _tmp558;struct Cyc_Absyn_Exp*
_tmp55A;struct Cyc_Absyn_Exp _tmp55B;void*_tmp55C;union Cyc_Absyn_Cnst _tmp55E;
struct _tuple6 _tmp55F;int _tmp560;enum Cyc_Absyn_Primop _tmp562;struct Cyc_List_List*
_tmp563;struct Cyc_List_List _tmp564;struct Cyc_Absyn_Exp*_tmp565;_LL2D1:{struct Cyc_Absyn_Var_e_struct*
_tmp545=(struct Cyc_Absyn_Var_e_struct*)_tmp544;if(_tmp545->tag != 1)goto _LL2D3;
else{_tmp546=(void*)_tmp545->f2;{struct Cyc_Absyn_Global_b_struct*_tmp547=(struct
Cyc_Absyn_Global_b_struct*)_tmp546;if(_tmp547->tag != 1)goto _LL2D3;else{_tmp548=
_tmp547->f1;}};}}if(!(!_tmp548->escapes))goto _LL2D3;_LL2D2: _tmp54C=_tmp548;goto
_LL2D4;_LL2D3:{struct Cyc_Absyn_Var_e_struct*_tmp549=(struct Cyc_Absyn_Var_e_struct*)
_tmp544;if(_tmp549->tag != 1)goto _LL2D5;else{_tmp54A=(void*)_tmp549->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp54B=(struct Cyc_Absyn_Local_b_struct*)_tmp54A;if(
_tmp54B->tag != 4)goto _LL2D5;else{_tmp54C=_tmp54B->f1;}};}}if(!(!_tmp54C->escapes))
goto _LL2D5;_LL2D4: _tmp550=_tmp54C;goto _LL2D6;_LL2D5:{struct Cyc_Absyn_Var_e_struct*
_tmp54D=(struct Cyc_Absyn_Var_e_struct*)_tmp544;if(_tmp54D->tag != 1)goto _LL2D7;
else{_tmp54E=(void*)_tmp54D->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp54F=(struct Cyc_Absyn_Pat_b_struct*)
_tmp54E;if(_tmp54F->tag != 5)goto _LL2D7;else{_tmp550=_tmp54F->f1;}};}}if(!(!
_tmp550->escapes))goto _LL2D7;_LL2D6: _tmp554=_tmp550;goto _LL2D8;_LL2D7:{struct Cyc_Absyn_Var_e_struct*
_tmp551=(struct Cyc_Absyn_Var_e_struct*)_tmp544;if(_tmp551->tag != 1)goto _LL2D9;
else{_tmp552=(void*)_tmp551->f2;{struct Cyc_Absyn_Param_b_struct*_tmp553=(struct
Cyc_Absyn_Param_b_struct*)_tmp552;if(_tmp553->tag != 3)goto _LL2D9;else{_tmp554=
_tmp553->f1;}};}}if(!(!_tmp554->escapes))goto _LL2D9;_LL2D8: {struct _RegionHandle*
_tmp566=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp93A;struct Cyc_List_List*
_tmp939;struct Cyc_List_List*_tmp567=(_tmp939=_region_malloc(_tmp566,sizeof(*
_tmp939)),((_tmp939->hd=((_tmp93A=_region_malloc(_tmp566,sizeof(*_tmp93A)),((
_tmp93A->vd=_tmp543,((_tmp93A->rop=Cyc_CfFlowInfo_LessVar(_tmp554,_tmp554->type),
_tmp93A)))))),((_tmp939->tl=_tmp4CD,_tmp939)))));struct _tuple16 _tmp93B;struct
_tuple16 _tmp569=(_tmp93B.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4CC,_tmp567),((
_tmp93B.f2=_tmp52B,_tmp93B)));_npop_handler(0);return _tmp569;}_LL2D9: {struct Cyc_Absyn_Const_e_struct*
_tmp555=(struct Cyc_Absyn_Const_e_struct*)_tmp544;if(_tmp555->tag != 0)goto _LL2DB;
else{_tmp556=_tmp555->f1;if((_tmp556.Int_c).tag != 5)goto _LL2DB;_tmp557=(struct
_tuple6)(_tmp556.Int_c).val;_tmp558=_tmp557.f2;}}_LL2DA: _tmp560=_tmp558;goto
_LL2DC;_LL2DB: {struct Cyc_Absyn_Cast_e_struct*_tmp559=(struct Cyc_Absyn_Cast_e_struct*)
_tmp544;if(_tmp559->tag != 13)goto _LL2DD;else{_tmp55A=_tmp559->f2;_tmp55B=*
_tmp55A;_tmp55C=_tmp55B.r;{struct Cyc_Absyn_Const_e_struct*_tmp55D=(struct Cyc_Absyn_Const_e_struct*)
_tmp55C;if(_tmp55D->tag != 0)goto _LL2DD;else{_tmp55E=_tmp55D->f1;if((_tmp55E.Int_c).tag
!= 5)goto _LL2DD;_tmp55F=(struct _tuple6)(_tmp55E.Int_c).val;_tmp560=_tmp55F.f2;}};}}
_LL2DC: {struct _RegionHandle*_tmp56C=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*
_tmp93E;struct Cyc_List_List*_tmp93D;struct Cyc_List_List*_tmp56D=(_tmp93D=
_region_malloc(_tmp56C,sizeof(*_tmp93D)),((_tmp93D->hd=((_tmp93E=_region_malloc(
_tmp56C,sizeof(*_tmp93E)),((_tmp93E->vd=_tmp543,((_tmp93E->rop=Cyc_CfFlowInfo_LessConst((
unsigned int)_tmp560),_tmp93E)))))),((_tmp93D->tl=_tmp4CD,_tmp93D)))));struct
_tuple16 _tmp93F;struct _tuple16 _tmp56F=(_tmp93F.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4CC,_tmp56D),((_tmp93F.f2=_tmp52B,_tmp93F)));_npop_handler(0);return _tmp56F;}
_LL2DD: {struct Cyc_Absyn_Primop_e_struct*_tmp561=(struct Cyc_Absyn_Primop_e_struct*)
_tmp544;if(_tmp561->tag != 2)goto _LL2DF;else{_tmp562=_tmp561->f1;_tmp563=_tmp561->f2;
if(_tmp563 == 0)goto _LL2DF;_tmp564=*_tmp563;_tmp565=(struct Cyc_Absyn_Exp*)_tmp564.hd;}}
_LL2DE: {void*_tmp572=_tmp565->r;void*_tmp574;struct Cyc_Absyn_Vardecl*_tmp576;
void*_tmp578;struct Cyc_Absyn_Vardecl*_tmp57A;void*_tmp57C;struct Cyc_Absyn_Vardecl*
_tmp57E;void*_tmp580;struct Cyc_Absyn_Vardecl*_tmp582;_LL2E2:{struct Cyc_Absyn_Var_e_struct*
_tmp573=(struct Cyc_Absyn_Var_e_struct*)_tmp572;if(_tmp573->tag != 1)goto _LL2E4;
else{_tmp574=(void*)_tmp573->f2;{struct Cyc_Absyn_Global_b_struct*_tmp575=(struct
Cyc_Absyn_Global_b_struct*)_tmp574;if(_tmp575->tag != 1)goto _LL2E4;else{_tmp576=
_tmp575->f1;}};}}if(!(!_tmp576->escapes))goto _LL2E4;_LL2E3: _tmp57A=_tmp576;goto
_LL2E5;_LL2E4:{struct Cyc_Absyn_Var_e_struct*_tmp577=(struct Cyc_Absyn_Var_e_struct*)
_tmp572;if(_tmp577->tag != 1)goto _LL2E6;else{_tmp578=(void*)_tmp577->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp579=(struct Cyc_Absyn_Local_b_struct*)_tmp578;if(
_tmp579->tag != 4)goto _LL2E6;else{_tmp57A=_tmp579->f1;}};}}if(!(!_tmp57A->escapes))
goto _LL2E6;_LL2E5: _tmp57E=_tmp57A;goto _LL2E7;_LL2E6:{struct Cyc_Absyn_Var_e_struct*
_tmp57B=(struct Cyc_Absyn_Var_e_struct*)_tmp572;if(_tmp57B->tag != 1)goto _LL2E8;
else{_tmp57C=(void*)_tmp57B->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp57D=(struct Cyc_Absyn_Pat_b_struct*)
_tmp57C;if(_tmp57D->tag != 5)goto _LL2E8;else{_tmp57E=_tmp57D->f1;}};}}if(!(!
_tmp57E->escapes))goto _LL2E8;_LL2E7: _tmp582=_tmp57E;goto _LL2E9;_LL2E8:{struct Cyc_Absyn_Var_e_struct*
_tmp57F=(struct Cyc_Absyn_Var_e_struct*)_tmp572;if(_tmp57F->tag != 1)goto _LL2EA;
else{_tmp580=(void*)_tmp57F->f2;{struct Cyc_Absyn_Param_b_struct*_tmp581=(struct
Cyc_Absyn_Param_b_struct*)_tmp580;if(_tmp581->tag != 3)goto _LL2EA;else{_tmp582=
_tmp581->f1;}};}}if(!(!_tmp582->escapes))goto _LL2EA;_LL2E9: {struct _RegionHandle*
_tmp583=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp942;struct Cyc_List_List*
_tmp941;struct Cyc_List_List*_tmp584=(_tmp941=_region_malloc(_tmp583,sizeof(*
_tmp941)),((_tmp941->hd=((_tmp942=_region_malloc(_tmp583,sizeof(*_tmp942)),((
_tmp942->vd=_tmp543,((_tmp942->rop=Cyc_CfFlowInfo_LessNumelts(_tmp582),_tmp942)))))),((
_tmp941->tl=_tmp4CD,_tmp941)))));struct _tuple16 _tmp943;struct _tuple16 _tmp586=(
_tmp943.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4CC,_tmp584),((_tmp943.f2=_tmp52B,
_tmp943)));_npop_handler(0);return _tmp586;}_LL2EA:;_LL2EB: {struct _tuple16
_tmp944;struct _tuple16 _tmp58A=(_tmp944.f1=_tmp52A,((_tmp944.f2=_tmp52B,_tmp944)));
_npop_handler(0);return _tmp58A;}_LL2E1:;}_LL2DF:;_LL2E0: {struct _tuple16 _tmp945;
struct _tuple16 _tmp58C=(_tmp945.f1=_tmp52A,((_tmp945.f2=_tmp52B,_tmp945)));
_npop_handler(0);return _tmp58C;}_LL2D0:;}_LL2CE:;_LL2CF: {struct _tuple16 _tmp946;
struct _tuple16 _tmp58E=(_tmp946.f1=_tmp52A,((_tmp946.f2=_tmp52B,_tmp946)));
_npop_handler(0);return _tmp58E;}_LL2C5:;};}case Cyc_Absyn_Lte: _LL2BF: {union Cyc_CfFlowInfo_FlowInfo
_tmp58F=f;union Cyc_CfFlowInfo_FlowInfo _tmp590=f;{union Cyc_CfFlowInfo_FlowInfo
_tmp591=_tmp58F;int _tmp592;struct _tuple13 _tmp593;struct Cyc_Dict_Dict _tmp594;
_LL2EE: if((_tmp591.BottomFL).tag != 1)goto _LL2F0;_tmp592=(int)(_tmp591.BottomFL).val;
_LL2EF: {struct Cyc_Core_Impossible_struct _tmp94C;const char*_tmp94B;struct Cyc_Core_Impossible_struct*
_tmp94A;(int)_throw((void*)((_tmp94A=_cycalloc(sizeof(*_tmp94A)),((_tmp94A[0]=((
_tmp94C.tag=Cyc_Core_Impossible,((_tmp94C.f1=((_tmp94B="anal_test, Lte",
_tag_dyneither(_tmp94B,sizeof(char),15))),_tmp94C)))),_tmp94A)))));}_LL2F0: if((
_tmp591.ReachableFL).tag != 2)goto _LL2ED;_tmp593=(struct _tuple13)(_tmp591.ReachableFL).val;
_tmp594=_tmp593.f1;_LL2F1: _tmp4CC=_tmp594;_LL2ED:;}{void*_tmp598=_tmp4D0->r;void*
_tmp59A;struct Cyc_Absyn_Vardecl*_tmp59C;void*_tmp59E;struct Cyc_Absyn_Vardecl*
_tmp5A0;void*_tmp5A2;struct Cyc_Absyn_Vardecl*_tmp5A4;void*_tmp5A6;struct Cyc_Absyn_Vardecl*
_tmp5A8;_LL2F3:{struct Cyc_Absyn_Var_e_struct*_tmp599=(struct Cyc_Absyn_Var_e_struct*)
_tmp598;if(_tmp599->tag != 1)goto _LL2F5;else{_tmp59A=(void*)_tmp599->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp59B=(struct Cyc_Absyn_Global_b_struct*)_tmp59A;if(
_tmp59B->tag != 1)goto _LL2F5;else{_tmp59C=_tmp59B->f1;}};}}if(!(!_tmp59C->escapes))
goto _LL2F5;_LL2F4: _tmp5A0=_tmp59C;goto _LL2F6;_LL2F5:{struct Cyc_Absyn_Var_e_struct*
_tmp59D=(struct Cyc_Absyn_Var_e_struct*)_tmp598;if(_tmp59D->tag != 1)goto _LL2F7;
else{_tmp59E=(void*)_tmp59D->f2;{struct Cyc_Absyn_Local_b_struct*_tmp59F=(struct
Cyc_Absyn_Local_b_struct*)_tmp59E;if(_tmp59F->tag != 4)goto _LL2F7;else{_tmp5A0=
_tmp59F->f1;}};}}if(!(!_tmp5A0->escapes))goto _LL2F7;_LL2F6: _tmp5A4=_tmp5A0;goto
_LL2F8;_LL2F7:{struct Cyc_Absyn_Var_e_struct*_tmp5A1=(struct Cyc_Absyn_Var_e_struct*)
_tmp598;if(_tmp5A1->tag != 1)goto _LL2F9;else{_tmp5A2=(void*)_tmp5A1->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp5A3=(struct Cyc_Absyn_Pat_b_struct*)_tmp5A2;if(_tmp5A3->tag
!= 5)goto _LL2F9;else{_tmp5A4=_tmp5A3->f1;}};}}if(!(!_tmp5A4->escapes))goto _LL2F9;
_LL2F8: _tmp5A8=_tmp5A4;goto _LL2FA;_LL2F9:{struct Cyc_Absyn_Var_e_struct*_tmp5A5=(
struct Cyc_Absyn_Var_e_struct*)_tmp598;if(_tmp5A5->tag != 1)goto _LL2FB;else{
_tmp5A6=(void*)_tmp5A5->f2;{struct Cyc_Absyn_Param_b_struct*_tmp5A7=(struct Cyc_Absyn_Param_b_struct*)
_tmp5A6;if(_tmp5A7->tag != 3)goto _LL2FB;else{_tmp5A8=_tmp5A7->f1;}};}}if(!(!
_tmp5A8->escapes))goto _LL2FB;_LL2FA: {void*_tmp5A9=_tmp4D1->r;union Cyc_Absyn_Cnst
_tmp5AB;struct _tuple6 _tmp5AC;int _tmp5AD;struct Cyc_Absyn_Exp*_tmp5AF;struct Cyc_Absyn_Exp
_tmp5B0;void*_tmp5B1;union Cyc_Absyn_Cnst _tmp5B3;struct _tuple6 _tmp5B4;int _tmp5B5;
enum Cyc_Absyn_Primop _tmp5B7;struct Cyc_List_List*_tmp5B8;struct Cyc_List_List
_tmp5B9;struct Cyc_Absyn_Exp*_tmp5BA;_LL2FE: {struct Cyc_Absyn_Const_e_struct*
_tmp5AA=(struct Cyc_Absyn_Const_e_struct*)_tmp5A9;if(_tmp5AA->tag != 0)goto _LL300;
else{_tmp5AB=_tmp5AA->f1;if((_tmp5AB.Int_c).tag != 5)goto _LL300;_tmp5AC=(struct
_tuple6)(_tmp5AB.Int_c).val;_tmp5AD=_tmp5AC.f2;}}_LL2FF: _tmp5B5=_tmp5AD;goto
_LL301;_LL300: {struct Cyc_Absyn_Cast_e_struct*_tmp5AE=(struct Cyc_Absyn_Cast_e_struct*)
_tmp5A9;if(_tmp5AE->tag != 13)goto _LL302;else{_tmp5AF=_tmp5AE->f2;_tmp5B0=*
_tmp5AF;_tmp5B1=_tmp5B0.r;{struct Cyc_Absyn_Const_e_struct*_tmp5B2=(struct Cyc_Absyn_Const_e_struct*)
_tmp5B1;if(_tmp5B2->tag != 0)goto _LL302;else{_tmp5B3=_tmp5B2->f1;if((_tmp5B3.Int_c).tag
!= 5)goto _LL302;_tmp5B4=(struct _tuple6)(_tmp5B3.Int_c).val;_tmp5B5=_tmp5B4.f2;}};}}
_LL301: {struct _RegionHandle*_tmp5BB=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*
_tmp94F;struct Cyc_List_List*_tmp94E;struct Cyc_List_List*_tmp5BC=(_tmp94E=
_region_malloc(_tmp5BB,sizeof(*_tmp94E)),((_tmp94E->hd=((_tmp94F=_region_malloc(
_tmp5BB,sizeof(*_tmp94F)),((_tmp94F->vd=_tmp5A8,((_tmp94F->rop=Cyc_CfFlowInfo_LessEqConst((
unsigned int)_tmp5B5),_tmp94F)))))),((_tmp94E->tl=_tmp4CD,_tmp94E)))));struct
_tuple16 _tmp950;struct _tuple16 _tmp5BE=(_tmp950.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4CC,_tmp5BC),((_tmp950.f2=_tmp590,_tmp950)));_npop_handler(0);return _tmp5BE;}
_LL302: {struct Cyc_Absyn_Primop_e_struct*_tmp5B6=(struct Cyc_Absyn_Primop_e_struct*)
_tmp5A9;if(_tmp5B6->tag != 2)goto _LL304;else{_tmp5B7=_tmp5B6->f1;_tmp5B8=_tmp5B6->f2;
if(_tmp5B8 == 0)goto _LL304;_tmp5B9=*_tmp5B8;_tmp5BA=(struct Cyc_Absyn_Exp*)_tmp5B9.hd;}}
_LL303: {void*_tmp5C1=_tmp5BA->r;void*_tmp5C3;struct Cyc_Absyn_Vardecl*_tmp5C5;
void*_tmp5C7;struct Cyc_Absyn_Vardecl*_tmp5C9;void*_tmp5CB;struct Cyc_Absyn_Vardecl*
_tmp5CD;void*_tmp5CF;struct Cyc_Absyn_Vardecl*_tmp5D1;_LL307:{struct Cyc_Absyn_Var_e_struct*
_tmp5C2=(struct Cyc_Absyn_Var_e_struct*)_tmp5C1;if(_tmp5C2->tag != 1)goto _LL309;
else{_tmp5C3=(void*)_tmp5C2->f2;{struct Cyc_Absyn_Global_b_struct*_tmp5C4=(struct
Cyc_Absyn_Global_b_struct*)_tmp5C3;if(_tmp5C4->tag != 1)goto _LL309;else{_tmp5C5=
_tmp5C4->f1;}};}}if(!(!_tmp5C5->escapes))goto _LL309;_LL308: _tmp5C9=_tmp5C5;goto
_LL30A;_LL309:{struct Cyc_Absyn_Var_e_struct*_tmp5C6=(struct Cyc_Absyn_Var_e_struct*)
_tmp5C1;if(_tmp5C6->tag != 1)goto _LL30B;else{_tmp5C7=(void*)_tmp5C6->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp5C8=(struct Cyc_Absyn_Local_b_struct*)_tmp5C7;if(
_tmp5C8->tag != 4)goto _LL30B;else{_tmp5C9=_tmp5C8->f1;}};}}if(!(!_tmp5C9->escapes))
goto _LL30B;_LL30A: _tmp5CD=_tmp5C9;goto _LL30C;_LL30B:{struct Cyc_Absyn_Var_e_struct*
_tmp5CA=(struct Cyc_Absyn_Var_e_struct*)_tmp5C1;if(_tmp5CA->tag != 1)goto _LL30D;
else{_tmp5CB=(void*)_tmp5CA->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp5CC=(struct Cyc_Absyn_Pat_b_struct*)
_tmp5CB;if(_tmp5CC->tag != 5)goto _LL30D;else{_tmp5CD=_tmp5CC->f1;}};}}if(!(!
_tmp5CD->escapes))goto _LL30D;_LL30C: _tmp5D1=_tmp5CD;goto _LL30E;_LL30D:{struct Cyc_Absyn_Var_e_struct*
_tmp5CE=(struct Cyc_Absyn_Var_e_struct*)_tmp5C1;if(_tmp5CE->tag != 1)goto _LL30F;
else{_tmp5CF=(void*)_tmp5CE->f2;{struct Cyc_Absyn_Param_b_struct*_tmp5D0=(struct
Cyc_Absyn_Param_b_struct*)_tmp5CF;if(_tmp5D0->tag != 3)goto _LL30F;else{_tmp5D1=
_tmp5D0->f1;}};}}if(!(!_tmp5D1->escapes))goto _LL30F;_LL30E: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp5D2=env->fenv;struct Cyc_CfFlowInfo_Reln*_tmp953;struct Cyc_List_List*_tmp952;
struct Cyc_List_List*_tmp5D3=(_tmp952=_region_malloc(_tmp5D2->r,sizeof(*_tmp952)),((
_tmp952->hd=((_tmp953=_region_malloc(_tmp5D2->r,sizeof(*_tmp953)),((_tmp953->vd=
_tmp5A8,((_tmp953->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp5D1),_tmp953)))))),((
_tmp952->tl=_tmp4CD,_tmp952)))));struct _tuple16 _tmp954;struct _tuple16 _tmp5D5=(
_tmp954.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4CC,_tmp5D3),((_tmp954.f2=_tmp590,
_tmp954)));_npop_handler(0);return _tmp5D5;}_LL30F:;_LL310: {struct _tuple16
_tmp955;struct _tuple16 _tmp5D9=(_tmp955.f1=_tmp58F,((_tmp955.f2=_tmp590,_tmp955)));
_npop_handler(0);return _tmp5D9;}_LL306:;}_LL304:;_LL305: {struct _tuple16 _tmp956;
struct _tuple16 _tmp5DB=(_tmp956.f1=_tmp58F,((_tmp956.f2=_tmp590,_tmp956)));
_npop_handler(0);return _tmp5DB;}_LL2FD:;}_LL2FB:;_LL2FC: {struct _tuple16 _tmp957;
struct _tuple16 _tmp5DD=(_tmp957.f1=_tmp58F,((_tmp957.f2=_tmp590,_tmp957)));
_npop_handler(0);return _tmp5DD;}_LL2F2:;};}default: _LL2EC: {struct _tuple16
_tmp958;struct _tuple16 _tmp5DF=(_tmp958.f1=f,((_tmp958.f2=f,_tmp958)));
_npop_handler(0);return _tmp5DF;}}}_LL28D:;};;_pop_region(temp);}_LL28B:;_LL28C:
goto _LL27E;_LL27E:;}{union Cyc_CfFlowInfo_FlowInfo _tmp5E1;void*_tmp5E2;struct
_tuple14 _tmp5E0=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);_tmp5E1=_tmp5E0.f1;
_tmp5E2=_tmp5E0.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp5E3=_tmp5E1;int _tmp5E4;
struct _tuple13 _tmp5E5;struct Cyc_Dict_Dict _tmp5E6;_LL313: if((_tmp5E3.BottomFL).tag
!= 1)goto _LL315;_tmp5E4=(int)(_tmp5E3.BottomFL).val;_LL314: {struct _tuple16
_tmp959;return(_tmp959.f1=_tmp5E1,((_tmp959.f2=_tmp5E1,_tmp959)));}_LL315: if((
_tmp5E3.ReachableFL).tag != 2)goto _LL312;_tmp5E5=(struct _tuple13)(_tmp5E3.ReachableFL).val;
_tmp5E6=_tmp5E5.f1;_LL316: {void*_tmp5E8=_tmp5E2;enum Cyc_CfFlowInfo_InitLevel
_tmp5EE;enum Cyc_CfFlowInfo_InitLevel _tmp5F0;enum Cyc_CfFlowInfo_InitLevel
_tmp5F3;_LL318: {struct Cyc_CfFlowInfo_Zero_struct*_tmp5E9=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp5E8;if(_tmp5E9->tag != 0)goto _LL31A;}_LL319: {struct _tuple16 _tmp95A;return(
_tmp95A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp95A.f2=_tmp5E1,_tmp95A)));}_LL31A: {
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp5EA=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp5E8;if(_tmp5EA->tag != 2)goto _LL31C;}_LL31B: goto _LL31D;_LL31C: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp5EB=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp5E8;if(_tmp5EB->tag != 1)
goto _LL31E;}_LL31D: goto _LL31F;_LL31E: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp5EC=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp5E8;if(_tmp5EC->tag != 5)goto
_LL320;}_LL31F: {struct _tuple16 _tmp95B;return(_tmp95B.f1=_tmp5E1,((_tmp95B.f2=
Cyc_CfFlowInfo_BottomFL(),_tmp95B)));}_LL320: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp5ED=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5E8;if(_tmp5ED->tag != 3)goto
_LL322;else{_tmp5EE=_tmp5ED->f1;if(_tmp5EE != Cyc_CfFlowInfo_NoneIL)goto _LL322;}}
_LL321: goto _LL323;_LL322: {struct Cyc_CfFlowInfo_Esc_struct*_tmp5EF=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp5E8;if(_tmp5EF->tag != 4)goto _LL324;else{_tmp5F0=_tmp5EF->f1;if(_tmp5F0 != Cyc_CfFlowInfo_NoneIL)
goto _LL324;}}_LL323: goto _LL325;_LL324: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp5F1=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp5E8;if(_tmp5F1->tag != 7)goto
_LL326;}_LL325:{const char*_tmp95E;void*_tmp95D;(_tmp95D=0,Cyc_Tcutil_terr(e->loc,((
_tmp95E="expression may not be initialized",_tag_dyneither(_tmp95E,sizeof(char),
34))),_tag_dyneither(_tmp95D,sizeof(void*),0)));}{struct _tuple16 _tmp95F;return(
_tmp95F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp95F.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp95F)));};_LL326: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5F2=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp5E8;if(_tmp5F2->tag != 3)goto _LL328;else{_tmp5F3=_tmp5F2->f1;}}_LL327: return
Cyc_NewControlFlow_splitzero(env,inflow,_tmp5E1,e,_tmp5F3);_LL328: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp5F4=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp5E8;if(_tmp5F4->tag != 4)goto _LL32A;}
_LL329: {struct _tuple16 _tmp960;return(_tmp960.f1=_tmp5E1,((_tmp960.f2=_tmp5E1,
_tmp960)));}_LL32A: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5F5=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp5E8;if(_tmp5F5->tag != 6)goto _LL317;}_LL32B: {struct Cyc_Core_Impossible_struct
_tmp966;const char*_tmp965;struct Cyc_Core_Impossible_struct*_tmp964;(int)_throw((
void*)((_tmp964=_cycalloc(sizeof(*_tmp964)),((_tmp964[0]=((_tmp966.tag=Cyc_Core_Impossible,((
_tmp966.f1=((_tmp965="anal_test",_tag_dyneither(_tmp965,sizeof(char),10))),
_tmp966)))),_tmp964)))));}_LL317:;}_LL312:;};};}struct _tuple23{unsigned int f1;
struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple23*ckenv,void*root,void*rval){struct _tuple23 _tmp600;unsigned int
_tmp601;struct Cyc_NewControlFlow_AnalEnv*_tmp602;struct Cyc_Dict_Dict _tmp603;
struct _tuple23*_tmp5FF=ckenv;_tmp600=*_tmp5FF;_tmp601=_tmp600.f1;_tmp602=_tmp600.f2;
_tmp603=_tmp600.f3;{void*_tmp604=root;struct Cyc_Absyn_Vardecl*_tmp606;_LL32D: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp605=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp604;if(_tmp605->tag != 0)goto _LL32F;else{_tmp606=_tmp605->f1;}}_LL32E: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp602->fenv)->r,_tmp606->type)){void*_tmp607=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp60B;_LL332: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp608=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp607;if(_tmp608->tag != 7)goto _LL334;}_LL333: goto _LL335;_LL334: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp609=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp607;if(_tmp609->tag != 0)goto
_LL336;}_LL335: goto _LL337;_LL336: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp60A=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp607;if(_tmp60A->tag != 3)goto _LL338;
else{_tmp60B=_tmp60A->f1;if(_tmp60B != Cyc_CfFlowInfo_NoneIL)goto _LL338;}}_LL337:
goto _LL331;_LL338:;_LL339:{const char*_tmp96A;void*_tmp969[1];struct Cyc_String_pa_struct
_tmp968;(_tmp968.tag=0,((_tmp968.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp606->name)),((_tmp969[0]=& _tmp968,Cyc_Tcutil_warn(
_tmp601,((_tmp96A="unique pointers reachable from %s may become unreachable",
_tag_dyneither(_tmp96A,sizeof(char),57))),_tag_dyneither(_tmp969,sizeof(void*),1)))))));}
goto _LL331;_LL331:;}goto _LL32C;_LL32F:;_LL330: goto _LL32C;_LL32C:;};}static void
Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo flow){union Cyc_CfFlowInfo_FlowInfo _tmp60F=flow;
int _tmp610;struct _tuple13 _tmp611;struct Cyc_Dict_Dict _tmp612;_LL33B: if((_tmp60F.BottomFL).tag
!= 1)goto _LL33D;_tmp610=(int)(_tmp60F.BottomFL).val;_LL33C: return;_LL33D: if((
_tmp60F.ReachableFL).tag != 2)goto _LL33A;_tmp611=(struct _tuple13)(_tmp60F.ReachableFL).val;
_tmp612=_tmp611.f1;_LL33E:{struct Cyc_List_List*_tmp613=env->param_roots;for(0;
_tmp613 != 0;_tmp613=_tmp613->tl){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp612,
Cyc_CfFlowInfo_lookup_place(_tmp612,(struct Cyc_CfFlowInfo_Place*)_tmp613->hd))!= 
Cyc_CfFlowInfo_AllIL){const char*_tmp96D;void*_tmp96C;(_tmp96C=0,Cyc_Tcutil_terr(
loc,((_tmp96D="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp96D,sizeof(char),76))),_tag_dyneither(_tmp96C,sizeof(void*),0)));}}}
if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple23 _tmp96E;struct _tuple23
_tmp616=(_tmp96E.f1=loc,((_tmp96E.f2=env,((_tmp96E.f3=_tmp612,_tmp96E)))));((
void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp616,_tmp612);}
return;_LL33A:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp618=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp61A;struct Cyc_Core_Opt*_tmp61B;
struct Cyc_Absyn_Exp*_tmp61C;struct Cyc_Absyn_Stmt*_tmp61D;unsigned int _tmp61E;
struct Cyc_Absyn_Switch_clause*_tmp619=(struct Cyc_Absyn_Switch_clause*)scs->hd;
_tmp61A=*_tmp619;_tmp61B=_tmp61A.pat_vars;_tmp61C=_tmp61A.where_clause;_tmp61D=
_tmp61A.body;_tmp61E=_tmp61A.loc;{union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_add_vars(
_tmp618,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp61B))->v,
_tmp618->unknown_all,_tmp61E);union Cyc_CfFlowInfo_FlowInfo clause_outflow;if(
_tmp61C != 0){struct Cyc_Absyn_Exp*wexp=(struct Cyc_Absyn_Exp*)_tmp61C;union Cyc_CfFlowInfo_FlowInfo
_tmp620;union Cyc_CfFlowInfo_FlowInfo _tmp621;struct _tuple16 _tmp61F=Cyc_NewControlFlow_anal_test(
env,clause_inflow,wexp);_tmp620=_tmp61F.f1;_tmp621=_tmp61F.f2;inflow=_tmp621;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp620,_tmp61D);}else{
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp61D);}{union
Cyc_CfFlowInfo_FlowInfo _tmp622=clause_outflow;int _tmp623;_LL340: if((_tmp622.BottomFL).tag
!= 1)goto _LL342;_tmp623=(int)(_tmp622.BottomFL).val;_LL341: goto _LL33F;_LL342:;
_LL343: if(scs->tl == 0)return clause_outflow;else{if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->pat_vars))->v != 0){const char*_tmp971;void*_tmp970;(
_tmp970=0,Cyc_Tcutil_terr(_tmp61D->loc,((_tmp971="switch clause may implicitly fallthru",
_tag_dyneither(_tmp971,sizeof(char),38))),_tag_dyneither(_tmp970,sizeof(void*),0)));}
else{const char*_tmp974;void*_tmp973;(_tmp973=0,Cyc_Tcutil_warn(_tmp61D->loc,((
_tmp974="switch clause may implicitly fallthru",_tag_dyneither(_tmp974,sizeof(
char),38))),_tag_dyneither(_tmp973,sizeof(void*),0)));}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL33F;_LL33F:;};};}return Cyc_CfFlowInfo_BottomFL();}struct
_tuple24{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int
f3;};static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple24*vdenv,
struct Cyc_Absyn_Vardecl*vd){struct _tuple24 _tmp629;struct Cyc_NewControlFlow_AnalEnv*
_tmp62A;struct Cyc_Dict_Dict _tmp62B;unsigned int _tmp62C;struct _tuple24*_tmp628=
vdenv;_tmp629=*_tmp628;_tmp62A=_tmp629.f1;_tmp62B=_tmp629.f2;_tmp62C=_tmp629.f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp62A->fenv)->r,vd->type)){struct Cyc_CfFlowInfo_VarRoot_struct
_tmp977;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp976;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp62D=(_tmp976=_region_malloc((_tmp62A->fenv)->r,sizeof(*_tmp976)),((_tmp976[0]=((
_tmp977.tag=0,((_tmp977.f1=vd,_tmp977)))),_tmp976)));void*_tmp62E=((void*(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(_tmp62B,(void*)_tmp62D);void*
_tmp62F=_tmp62E;enum Cyc_CfFlowInfo_InitLevel _tmp633;_LL345: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp630=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp62F;if(_tmp630->tag != 7)goto
_LL347;}_LL346: goto _LL348;_LL347: {struct Cyc_CfFlowInfo_Zero_struct*_tmp631=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp62F;if(_tmp631->tag != 0)goto _LL349;}_LL348:
goto _LL34A;_LL349: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp632=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp62F;if(_tmp632->tag != 3)goto _LL34B;else{_tmp633=_tmp632->f1;if(_tmp633 != Cyc_CfFlowInfo_NoneIL)
goto _LL34B;}}_LL34A: goto _LL344;_LL34B:;_LL34C:{const char*_tmp97B;void*_tmp97A[1];
struct Cyc_String_pa_struct _tmp979;(_tmp979.tag=0,((_tmp979.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((
_tmp97A[0]=& _tmp979,Cyc_Tcutil_warn(_tmp62C,((_tmp97B="unique pointers may still exist after variable %s goes out of scope",
_tag_dyneither(_tmp97B,sizeof(char),68))),_tag_dyneither(_tmp97A,sizeof(void*),1)))))));}
goto _LL344;_LL344:;}}static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){{union Cyc_CfFlowInfo_FlowInfo
_tmp639=inflow;struct _tuple13 _tmp63A;struct Cyc_Dict_Dict _tmp63B;_LL34E: if((
_tmp639.ReachableFL).tag != 2)goto _LL350;_tmp63A=(struct _tuple13)(_tmp639.ReachableFL).val;
_tmp63B=_tmp63A.f1;_LL34F: {struct _tuple24 _tmp97C;struct _tuple24 _tmp63C=(_tmp97C.f1=
env,((_tmp97C.f2=_tmp63B,((_tmp97C.f3=decl->loc,_tmp97C)))));struct Cyc_CfFlowInfo_FlowEnv*
_tmp63D=env->fenv;{void*_tmp63E=decl->r;struct Cyc_Absyn_Vardecl*_tmp640;struct
Cyc_Core_Opt*_tmp642;struct Cyc_Core_Opt _tmp643;struct Cyc_List_List*_tmp644;
struct Cyc_List_List*_tmp646;_LL353: {struct Cyc_Absyn_Var_d_struct*_tmp63F=(
struct Cyc_Absyn_Var_d_struct*)_tmp63E;if(_tmp63F->tag != 0)goto _LL355;else{
_tmp640=_tmp63F->f1;}}_LL354: Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp63C,
_tmp640);goto _LL352;_LL355: {struct Cyc_Absyn_Let_d_struct*_tmp641=(struct Cyc_Absyn_Let_d_struct*)
_tmp63E;if(_tmp641->tag != 2)goto _LL357;else{_tmp642=_tmp641->f2;if(_tmp642 == 0)
goto _LL357;_tmp643=*_tmp642;_tmp644=(struct Cyc_List_List*)_tmp643.v;}}_LL356:
_tmp646=_tmp644;goto _LL358;_LL357: {struct Cyc_Absyn_Letv_d_struct*_tmp645=(
struct Cyc_Absyn_Letv_d_struct*)_tmp63E;if(_tmp645->tag != 3)goto _LL359;else{
_tmp646=_tmp645->f1;}}_LL358:((void(*)(void(*f)(struct _tuple24*,struct Cyc_Absyn_Vardecl*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,&
_tmp63C,_tmp646);goto _LL352;_LL359:;_LL35A: goto _LL352;_LL352:;}goto _LL34D;}
_LL350:;_LL351: goto _LL34D;_LL34D:;}return;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp649;union Cyc_CfFlowInfo_FlowInfo*_tmp64A;struct _tuple17 _tmp648=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp649=_tmp648.f1;_tmp64A=_tmp648.f2;inflow=*_tmp64A;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp64B=env->fenv;void*_tmp64C=s->r;struct Cyc_Absyn_Exp*_tmp64F;struct Cyc_Absyn_Exp*
_tmp651;struct Cyc_Absyn_Exp*_tmp653;struct Cyc_Absyn_Stmt*_tmp655;struct Cyc_Absyn_Stmt*
_tmp656;struct Cyc_Absyn_Exp*_tmp658;struct Cyc_Absyn_Stmt*_tmp659;struct Cyc_Absyn_Stmt*
_tmp65A;struct _tuple10 _tmp65C;struct Cyc_Absyn_Exp*_tmp65D;struct Cyc_Absyn_Stmt*
_tmp65E;struct Cyc_Absyn_Stmt*_tmp65F;struct Cyc_Absyn_Stmt*_tmp661;struct _tuple10
_tmp662;struct Cyc_Absyn_Exp*_tmp663;struct Cyc_Absyn_Stmt*_tmp664;struct Cyc_Absyn_Exp*
_tmp666;struct _tuple10 _tmp667;struct Cyc_Absyn_Exp*_tmp668;struct Cyc_Absyn_Stmt*
_tmp669;struct _tuple10 _tmp66A;struct Cyc_Absyn_Exp*_tmp66B;struct Cyc_Absyn_Stmt*
_tmp66C;struct Cyc_Absyn_Stmt*_tmp66D;struct Cyc_Absyn_Stmt*_tmp66F;struct Cyc_List_List*
_tmp671;struct Cyc_Absyn_Switch_clause**_tmp672;struct Cyc_Absyn_Switch_clause*
_tmp673;struct Cyc_Absyn_Stmt*_tmp675;struct Cyc_Absyn_Stmt*_tmp677;struct Cyc_Absyn_Stmt*
_tmp679;struct Cyc_Absyn_Exp*_tmp67B;struct Cyc_List_List*_tmp67C;struct Cyc_Absyn_Stmt*
_tmp67E;struct Cyc_List_List*_tmp67F;struct Cyc_Absyn_Decl*_tmp681;struct Cyc_Absyn_Decl
_tmp682;void*_tmp683;struct Cyc_Absyn_Exp*_tmp685;struct Cyc_Absyn_Tvar*_tmp686;
struct Cyc_Absyn_Vardecl*_tmp687;struct Cyc_Absyn_Stmt*_tmp688;struct Cyc_Absyn_Decl*
_tmp68A;struct Cyc_Absyn_Stmt*_tmp68B;struct Cyc_Absyn_Stmt*_tmp68D;struct Cyc_Absyn_Exp*
_tmp68F;_LL35C: {struct Cyc_Absyn_Skip_s_struct*_tmp64D=(struct Cyc_Absyn_Skip_s_struct*)
_tmp64C;if(_tmp64D->tag != 0)goto _LL35E;}_LL35D: return inflow;_LL35E: {struct Cyc_Absyn_Return_s_struct*
_tmp64E=(struct Cyc_Absyn_Return_s_struct*)_tmp64C;if(_tmp64E->tag != 3)goto _LL360;
else{_tmp64F=_tmp64E->f1;if(_tmp64F != 0)goto _LL360;}}_LL35F: if(env->noreturn){
const char*_tmp97F;void*_tmp97E;(_tmp97E=0,Cyc_Tcutil_terr(s->loc,((_tmp97F="`noreturn' function might return",
_tag_dyneither(_tmp97F,sizeof(char),33))),_tag_dyneither(_tmp97E,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL360: {struct Cyc_Absyn_Return_s_struct*_tmp650=(struct Cyc_Absyn_Return_s_struct*)
_tmp64C;if(_tmp650->tag != 3)goto _LL362;else{_tmp651=_tmp650->f1;}}_LL361: if(env->noreturn){
const char*_tmp982;void*_tmp981;(_tmp981=0,Cyc_Tcutil_terr(s->loc,((_tmp982="`noreturn' function might return",
_tag_dyneither(_tmp982,sizeof(char),33))),_tag_dyneither(_tmp981,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp695;void*_tmp696;struct _tuple14 _tmp694=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp651));_tmp695=_tmp694.f1;
_tmp696=_tmp694.f2;_tmp695=Cyc_NewControlFlow_use_Rval(env,_tmp651->loc,_tmp695,
_tmp696);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp695);return Cyc_CfFlowInfo_BottomFL();};
_LL362: {struct Cyc_Absyn_Exp_s_struct*_tmp652=(struct Cyc_Absyn_Exp_s_struct*)
_tmp64C;if(_tmp652->tag != 1)goto _LL364;else{_tmp653=_tmp652->f1;}}_LL363: return(
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp653)).f1;_LL364: {struct Cyc_Absyn_Seq_s_struct*
_tmp654=(struct Cyc_Absyn_Seq_s_struct*)_tmp64C;if(_tmp654->tag != 2)goto _LL366;
else{_tmp655=_tmp654->f1;_tmp656=_tmp654->f2;}}_LL365: return Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp655),_tmp656);_LL366: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp657=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp64C;if(_tmp657->tag != 4)goto
_LL368;else{_tmp658=_tmp657->f1;_tmp659=_tmp657->f2;_tmp65A=_tmp657->f3;}}_LL367: {
union Cyc_CfFlowInfo_FlowInfo _tmp698;union Cyc_CfFlowInfo_FlowInfo _tmp699;struct
_tuple16 _tmp697=Cyc_NewControlFlow_anal_test(env,inflow,_tmp658);_tmp698=_tmp697.f1;
_tmp699=_tmp697.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp69A=Cyc_NewControlFlow_anal_stmt(
env,_tmp699,_tmp65A);union Cyc_CfFlowInfo_FlowInfo _tmp69B=Cyc_NewControlFlow_anal_stmt(
env,_tmp698,_tmp659);return Cyc_CfFlowInfo_join_flow(_tmp64B,env->all_changed,
_tmp69B,_tmp69A);};}_LL368: {struct Cyc_Absyn_While_s_struct*_tmp65B=(struct Cyc_Absyn_While_s_struct*)
_tmp64C;if(_tmp65B->tag != 5)goto _LL36A;else{_tmp65C=_tmp65B->f1;_tmp65D=_tmp65C.f1;
_tmp65E=_tmp65C.f2;_tmp65F=_tmp65B->f2;}}_LL369: {union Cyc_CfFlowInfo_FlowInfo*
_tmp69D;struct _tuple17 _tmp69C=Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp65E);_tmp69D=_tmp69C.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp69E=*_tmp69D;union
Cyc_CfFlowInfo_FlowInfo _tmp6A0;union Cyc_CfFlowInfo_FlowInfo _tmp6A1;struct
_tuple16 _tmp69F=Cyc_NewControlFlow_anal_test(env,_tmp69E,_tmp65D);_tmp6A0=
_tmp69F.f1;_tmp6A1=_tmp69F.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp6A2=Cyc_NewControlFlow_anal_stmt(
env,_tmp6A0,_tmp65F);Cyc_NewControlFlow_update_flow(env,_tmp65E,_tmp6A2);return
_tmp6A1;};};}_LL36A: {struct Cyc_Absyn_Do_s_struct*_tmp660=(struct Cyc_Absyn_Do_s_struct*)
_tmp64C;if(_tmp660->tag != 14)goto _LL36C;else{_tmp661=_tmp660->f1;_tmp662=_tmp660->f2;
_tmp663=_tmp662.f1;_tmp664=_tmp662.f2;}}_LL36B: {union Cyc_CfFlowInfo_FlowInfo
_tmp6A3=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp661);union Cyc_CfFlowInfo_FlowInfo*
_tmp6A5;struct _tuple17 _tmp6A4=Cyc_NewControlFlow_pre_stmt_check(env,_tmp6A3,
_tmp664);_tmp6A5=_tmp6A4.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp6A6=*_tmp6A5;union
Cyc_CfFlowInfo_FlowInfo _tmp6A8;union Cyc_CfFlowInfo_FlowInfo _tmp6A9;struct
_tuple16 _tmp6A7=Cyc_NewControlFlow_anal_test(env,_tmp6A6,_tmp663);_tmp6A8=
_tmp6A7.f1;_tmp6A9=_tmp6A7.f2;Cyc_NewControlFlow_update_flow(env,_tmp661,_tmp6A8);
return _tmp6A9;};}_LL36C: {struct Cyc_Absyn_For_s_struct*_tmp665=(struct Cyc_Absyn_For_s_struct*)
_tmp64C;if(_tmp665->tag != 9)goto _LL36E;else{_tmp666=_tmp665->f1;_tmp667=_tmp665->f2;
_tmp668=_tmp667.f1;_tmp669=_tmp667.f2;_tmp66A=_tmp665->f3;_tmp66B=_tmp66A.f1;
_tmp66C=_tmp66A.f2;_tmp66D=_tmp665->f4;}}_LL36D: {union Cyc_CfFlowInfo_FlowInfo
_tmp6AA=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp666)).f1;union Cyc_CfFlowInfo_FlowInfo*
_tmp6AC;struct _tuple17 _tmp6AB=Cyc_NewControlFlow_pre_stmt_check(env,_tmp6AA,
_tmp669);_tmp6AC=_tmp6AB.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp6AD=*_tmp6AC;union
Cyc_CfFlowInfo_FlowInfo _tmp6AF;union Cyc_CfFlowInfo_FlowInfo _tmp6B0;struct
_tuple16 _tmp6AE=Cyc_NewControlFlow_anal_test(env,_tmp6AD,_tmp668);_tmp6AF=
_tmp6AE.f1;_tmp6B0=_tmp6AE.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp6B1=Cyc_NewControlFlow_anal_stmt(
env,_tmp6AF,_tmp66D);union Cyc_CfFlowInfo_FlowInfo*_tmp6B3;struct _tuple17 _tmp6B2=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp6B1,_tmp66C);_tmp6B3=_tmp6B2.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp6B4=*_tmp6B3;union Cyc_CfFlowInfo_FlowInfo _tmp6B5=(Cyc_NewControlFlow_anal_Rexp(
env,0,_tmp6B4,_tmp66B)).f1;Cyc_NewControlFlow_update_flow(env,_tmp669,_tmp6B5);
return _tmp6B0;};};};}_LL36E: {struct Cyc_Absyn_Break_s_struct*_tmp66E=(struct Cyc_Absyn_Break_s_struct*)
_tmp64C;if(_tmp66E->tag != 6)goto _LL370;else{_tmp66F=_tmp66E->f1;if(_tmp66F != 0)
goto _LL370;}}_LL36F: return Cyc_CfFlowInfo_BottomFL();_LL370: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp670=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp64C;if(_tmp670->tag != 11)goto
_LL372;else{_tmp671=_tmp670->f1;_tmp672=_tmp670->f2;if(_tmp672 == 0)goto _LL372;
_tmp673=*_tmp672;}}_LL371: {struct _RegionHandle _tmp6B6=_new_region("temp");
struct _RegionHandle*temp=& _tmp6B6;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo
_tmp6B8;struct Cyc_List_List*_tmp6B9;struct _tuple20 _tmp6B7=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp671,1,1);_tmp6B8=_tmp6B7.f1;_tmp6B9=_tmp6B7.f2;for(0;_tmp6B9
!= 0;(_tmp6B9=_tmp6B9->tl,_tmp671=_tmp671->tl)){_tmp6B8=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp671))->hd)->loc,
_tmp6B8,(void*)_tmp6B9->hd);}_tmp6B8=Cyc_NewControlFlow_add_vars(_tmp64B,_tmp6B8,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp673->pat_vars))->v,
_tmp64B->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp673->body,_tmp6B8);{union Cyc_CfFlowInfo_FlowInfo _tmp6BA=Cyc_CfFlowInfo_BottomFL();
_npop_handler(0);return _tmp6BA;};};_pop_region(temp);}_LL372: {struct Cyc_Absyn_Break_s_struct*
_tmp674=(struct Cyc_Absyn_Break_s_struct*)_tmp64C;if(_tmp674->tag != 6)goto _LL374;
else{_tmp675=_tmp674->f1;}}_LL373: _tmp677=_tmp675;goto _LL375;_LL374: {struct Cyc_Absyn_Continue_s_struct*
_tmp676=(struct Cyc_Absyn_Continue_s_struct*)_tmp64C;if(_tmp676->tag != 7)goto
_LL376;else{_tmp677=_tmp676->f1;}}_LL375: _tmp679=_tmp677;goto _LL377;_LL376: {
struct Cyc_Absyn_Goto_s_struct*_tmp678=(struct Cyc_Absyn_Goto_s_struct*)_tmp64C;
if(_tmp678->tag != 8)goto _LL378;else{_tmp679=_tmp678->f2;}}_LL377: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp6BB=_tmp649->encloser;struct Cyc_Absyn_Stmt*
_tmp6BC=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp679)))->encloser;while(_tmp6BC != _tmp6BB){struct Cyc_Absyn_Stmt*_tmp6BD=(Cyc_NewControlFlow_get_stmt_annot(
_tmp6BB))->encloser;if(_tmp6BD == _tmp6BB){{const char*_tmp985;void*_tmp984;(
_tmp984=0,Cyc_Tcutil_terr(s->loc,((_tmp985="goto enters local scope or exception handler",
_tag_dyneither(_tmp985,sizeof(char),45))),_tag_dyneither(_tmp984,sizeof(void*),0)));}
break;}_tmp6BB=_tmp6BD;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp679),inflow);return Cyc_CfFlowInfo_BottomFL();_LL378: {struct Cyc_Absyn_Switch_s_struct*
_tmp67A=(struct Cyc_Absyn_Switch_s_struct*)_tmp64C;if(_tmp67A->tag != 10)goto
_LL37A;else{_tmp67B=_tmp67A->f1;_tmp67C=_tmp67A->f2;}}_LL379: {union Cyc_CfFlowInfo_FlowInfo
_tmp6C1;void*_tmp6C2;struct _tuple14 _tmp6C0=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp67B);_tmp6C1=_tmp6C0.f1;_tmp6C2=_tmp6C0.f2;_tmp6C1=Cyc_NewControlFlow_use_Rval(
env,_tmp67B->loc,_tmp6C1,_tmp6C2);return Cyc_NewControlFlow_anal_scs(env,_tmp6C1,
_tmp67C);}_LL37A: {struct Cyc_Absyn_TryCatch_s_struct*_tmp67D=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp64C;if(_tmp67D->tag != 15)goto _LL37C;else{_tmp67E=_tmp67D->f1;_tmp67F=_tmp67D->f2;}}
_LL37B: {int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp67E);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp67F);{union Cyc_CfFlowInfo_FlowInfo _tmp6C3=scs_outflow;int
_tmp6C4;_LL387: if((_tmp6C3.BottomFL).tag != 1)goto _LL389;_tmp6C4=(int)(_tmp6C3.BottomFL).val;
_LL388: goto _LL386;_LL389:;_LL38A: {const char*_tmp988;void*_tmp987;(_tmp987=0,Cyc_Tcutil_terr(
s->loc,((_tmp988="last catch clause may implicitly fallthru",_tag_dyneither(
_tmp988,sizeof(char),42))),_tag_dyneither(_tmp987,sizeof(void*),0)));}_LL386:;}
outflow=s1_outflow;return outflow;};};}_LL37C: {struct Cyc_Absyn_Decl_s_struct*
_tmp680=(struct Cyc_Absyn_Decl_s_struct*)_tmp64C;if(_tmp680->tag != 12)goto _LL37E;
else{_tmp681=_tmp680->f1;_tmp682=*_tmp681;_tmp683=_tmp682.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp684=(struct Cyc_Absyn_Alias_d_struct*)_tmp683;if(_tmp684->tag != 5)goto _LL37E;
else{_tmp685=_tmp684->f1;_tmp686=_tmp684->f2;_tmp687=_tmp684->f3;}};_tmp688=
_tmp680->f2;}}_LL37D: {union Cyc_CfFlowInfo_FlowInfo _tmp6C8;union Cyc_CfFlowInfo_AbsLVal
_tmp6C9;struct _tuple15 _tmp6C7=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp685);
_tmp6C8=_tmp6C7.f1;_tmp6C9=_tmp6C7.f2;{struct _tuple15 _tmp989;struct _tuple15
_tmp6CB=(_tmp989.f1=_tmp6C8,((_tmp989.f2=_tmp6C9,_tmp989)));union Cyc_CfFlowInfo_FlowInfo
_tmp6CC;struct _tuple13 _tmp6CD;struct Cyc_Dict_Dict _tmp6CE;struct Cyc_List_List*
_tmp6CF;union Cyc_CfFlowInfo_AbsLVal _tmp6D0;struct Cyc_CfFlowInfo_Place*_tmp6D1;
union Cyc_CfFlowInfo_FlowInfo _tmp6D2;int _tmp6D3;_LL38C: _tmp6CC=_tmp6CB.f1;if((
_tmp6CC.ReachableFL).tag != 2)goto _LL38E;_tmp6CD=(struct _tuple13)(_tmp6CC.ReachableFL).val;
_tmp6CE=_tmp6CD.f1;_tmp6CF=_tmp6CD.f2;_tmp6D0=_tmp6CB.f2;if((_tmp6D0.PlaceL).tag
!= 1)goto _LL38E;_tmp6D1=(struct Cyc_CfFlowInfo_Place*)(_tmp6D0.PlaceL).val;_LL38D: {
void*_tmp6D4=Cyc_CfFlowInfo_lookup_place(_tmp6CE,_tmp6D1);void*t=(void*)
_check_null(_tmp685->topt);{void*_tmp6D5=_tmp685->r;struct Cyc_Absyn_Exp*_tmp6D7;
_LL393: {struct Cyc_Absyn_Cast_e_struct*_tmp6D6=(struct Cyc_Absyn_Cast_e_struct*)
_tmp6D5;if(_tmp6D6->tag != 13)goto _LL395;else{_tmp6D7=_tmp6D6->f2;}}_LL394: t=(
void*)_check_null(_tmp6D7->topt);goto _LL392;_LL395:;_LL396: goto _LL392;_LL392:;}{
void*_tmp6D8=Cyc_CfFlowInfo_make_unique_consumed(_tmp64B,t,_tmp685,env->iteration_num,
_tmp6D4);_tmp6CE=Cyc_CfFlowInfo_assign_place(_tmp64B,_tmp685->loc,_tmp6CE,env->all_changed,
_tmp6D1,_tmp6D8);_tmp6C8=Cyc_CfFlowInfo_ReachableFL(_tmp6CE,_tmp6CF);{void*leaf;
switch(Cyc_CfFlowInfo_initlevel(_tmp64B,_tmp6CE,_tmp6D4)){case Cyc_CfFlowInfo_AllIL:
_LL397: leaf=_tmp64B->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL398: leaf=
_tmp64B->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL399: leaf=_tmp64B->unknown_this;
break;}{void*_tmp6D9=Cyc_CfFlowInfo_typ_to_absrval(_tmp64B,t,0,leaf);_tmp6C8=Cyc_NewControlFlow_use_Rval(
env,_tmp685->loc,_tmp6C8,_tmp6D4);{struct Cyc_List_List _tmp98A;struct Cyc_List_List
_tmp6DA=(_tmp98A.hd=_tmp687,((_tmp98A.tl=0,_tmp98A)));_tmp6C8=Cyc_NewControlFlow_add_vars(
_tmp64B,_tmp6C8,(struct Cyc_List_List*)& _tmp6DA,_tmp64B->unknown_all,s->loc);
_tmp6C8=Cyc_NewControlFlow_anal_stmt(env,_tmp6C8,_tmp688);{union Cyc_CfFlowInfo_FlowInfo
_tmp6DB=_tmp6C8;struct _tuple13 _tmp6DC;struct Cyc_Dict_Dict _tmp6DD;struct Cyc_List_List*
_tmp6DE;_LL39C: if((_tmp6DB.ReachableFL).tag != 2)goto _LL39E;_tmp6DC=(struct
_tuple13)(_tmp6DB.ReachableFL).val;_tmp6DD=_tmp6DC.f1;_tmp6DE=_tmp6DC.f2;_LL39D:
_tmp6DD=Cyc_CfFlowInfo_assign_place(_tmp64B,s->loc,_tmp6DD,env->all_changed,
_tmp6D1,_tmp6D9);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp6DD,_tmp6DE);return
outflow;_LL39E:;_LL39F: return _tmp6C8;_LL39B:;};};};};};}_LL38E: _tmp6D2=_tmp6CB.f1;
if((_tmp6D2.BottomFL).tag != 1)goto _LL390;_tmp6D3=(int)(_tmp6D2.BottomFL).val;
_LL38F: return _tmp6C8;_LL390:;_LL391:{const char*_tmp98D;void*_tmp98C;(_tmp98C=0,
Cyc_Tcutil_terr(_tmp685->loc,((_tmp98D="bad alias expression--no unique path found",
_tag_dyneither(_tmp98D,sizeof(char),43))),_tag_dyneither(_tmp98C,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL38B:;};}_LL37E: {struct Cyc_Absyn_Decl_s_struct*
_tmp689=(struct Cyc_Absyn_Decl_s_struct*)_tmp64C;if(_tmp689->tag != 12)goto _LL380;
else{_tmp68A=_tmp689->f1;_tmp68B=_tmp689->f2;}}_LL37F: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp68A),_tmp68B);if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp68A);return outflow;
_LL380: {struct Cyc_Absyn_Label_s_struct*_tmp68C=(struct Cyc_Absyn_Label_s_struct*)
_tmp64C;if(_tmp68C->tag != 13)goto _LL382;else{_tmp68D=_tmp68C->f2;}}_LL381: return
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp68D);_LL382: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp68E=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp64C;if(_tmp68E->tag != 16)goto
_LL384;else{_tmp68F=_tmp68E->f1;}}_LL383: {union Cyc_CfFlowInfo_FlowInfo _tmp6E3;
void*_tmp6E4;struct _tuple14 _tmp6E2=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,
_tmp68F);_tmp6E3=_tmp6E2.f1;_tmp6E4=_tmp6E2.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp6E5=Cyc_NewControlFlow_use_Rval(env,_tmp68F->loc,_tmp6E3,_tmp6E4);void*
_tmp6E6=Cyc_Tcutil_compress((void*)_check_null(_tmp68F->topt));void*_tmp6E8;
_LL3A1: {struct Cyc_Absyn_RgnHandleType_struct*_tmp6E7=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6E6;if(_tmp6E7->tag != 15)goto _LL3A3;else{_tmp6E8=(void*)_tmp6E7->f1;}}_LL3A2:
return Cyc_CfFlowInfo_kill_flow_region(_tmp64B,_tmp6E3,_tmp6E8);_LL3A3:;_LL3A4: {
struct Cyc_Core_Impossible_struct _tmp993;const char*_tmp992;struct Cyc_Core_Impossible_struct*
_tmp991;(int)_throw((void*)((_tmp991=_cycalloc(sizeof(*_tmp991)),((_tmp991[0]=((
_tmp993.tag=Cyc_Core_Impossible,((_tmp993.f1=((_tmp992="anal_stmt -- reset_region",
_tag_dyneither(_tmp992,sizeof(char),26))),_tmp993)))),_tmp991)))));}_LL3A0:;};}
_LL384:;_LL385: {struct Cyc_Core_Impossible_struct _tmp999;const char*_tmp998;
struct Cyc_Core_Impossible_struct*_tmp997;(int)_throw((void*)((_tmp997=_cycalloc(
sizeof(*_tmp997)),((_tmp997[0]=((_tmp999.tag=Cyc_Core_Impossible,((_tmp999.f1=((
_tmp998="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(
_tmp998,sizeof(char),56))),_tmp999)))),_tmp997)))));}_LL35B:;};}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*
_tmp6EF=env->fenv;void*_tmp6F0=decl->r;struct Cyc_Absyn_Vardecl*_tmp6F2;struct Cyc_Core_Opt*
_tmp6F4;struct Cyc_Core_Opt _tmp6F5;struct Cyc_List_List*_tmp6F6;struct Cyc_Absyn_Exp*
_tmp6F7;struct Cyc_List_List*_tmp6F9;struct Cyc_Absyn_Fndecl*_tmp6FB;struct Cyc_Absyn_Tvar*
_tmp6FD;struct Cyc_Absyn_Vardecl*_tmp6FE;int _tmp6FF;struct Cyc_Absyn_Exp*_tmp700;
_LL3A6: {struct Cyc_Absyn_Var_d_struct*_tmp6F1=(struct Cyc_Absyn_Var_d_struct*)
_tmp6F0;if(_tmp6F1->tag != 0)goto _LL3A8;else{_tmp6F2=_tmp6F1->f1;}}_LL3A7: {
struct Cyc_List_List _tmp99A;struct Cyc_List_List _tmp701=(_tmp99A.hd=_tmp6F2,((
_tmp99A.tl=0,_tmp99A)));inflow=Cyc_NewControlFlow_add_vars(_tmp6EF,inflow,(
struct Cyc_List_List*)& _tmp701,_tmp6EF->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*
_tmp702=_tmp6F2->initializer;if(_tmp702 == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo
_tmp704;void*_tmp705;struct _tuple14 _tmp703=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,(struct Cyc_Absyn_Exp*)_tmp702);_tmp704=_tmp703.f1;_tmp705=_tmp703.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp706=_tmp704;int _tmp707;struct _tuple13 _tmp708;
struct Cyc_Dict_Dict _tmp709;struct Cyc_List_List*_tmp70A;_LL3B3: if((_tmp706.BottomFL).tag
!= 1)goto _LL3B5;_tmp707=(int)(_tmp706.BottomFL).val;_LL3B4: return Cyc_CfFlowInfo_BottomFL();
_LL3B5: if((_tmp706.ReachableFL).tag != 2)goto _LL3B2;_tmp708=(struct _tuple13)(
_tmp706.ReachableFL).val;_tmp709=_tmp708.f1;_tmp70A=_tmp708.f2;_LL3B6:{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp9A0;struct Cyc_CfFlowInfo_VarRoot_struct _tmp99F;struct Cyc_CfFlowInfo_Place*
_tmp99E;_tmp709=Cyc_CfFlowInfo_assign_place(_tmp6EF,decl->loc,_tmp709,env->all_changed,((
_tmp99E=_region_malloc(env->r,sizeof(*_tmp99E)),((_tmp99E->root=(void*)((_tmp9A0=
_region_malloc(env->r,sizeof(*_tmp9A0)),((_tmp9A0[0]=((_tmp99F.tag=0,((_tmp99F.f1=
_tmp6F2,_tmp99F)))),_tmp9A0)))),((_tmp99E->fields=0,_tmp99E)))))),_tmp705);}
_tmp70A=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp70A,_tmp6F2,(struct Cyc_Absyn_Exp*)
_check_null(_tmp6F2->initializer));{union Cyc_CfFlowInfo_FlowInfo _tmp70E=Cyc_CfFlowInfo_ReachableFL(
_tmp709,_tmp70A);Cyc_NewControlFlow_update_tryflow(env,_tmp70E);return _tmp70E;};
_LL3B2:;};};};}_LL3A8: {struct Cyc_Absyn_Let_d_struct*_tmp6F3=(struct Cyc_Absyn_Let_d_struct*)
_tmp6F0;if(_tmp6F3->tag != 2)goto _LL3AA;else{_tmp6F4=_tmp6F3->f2;if(_tmp6F4 == 0)
goto _LL3AA;_tmp6F5=*_tmp6F4;_tmp6F6=(struct Cyc_List_List*)_tmp6F5.v;_tmp6F7=
_tmp6F3->f3;}}_LL3A9: {union Cyc_CfFlowInfo_FlowInfo _tmp711;void*_tmp712;struct
_tuple14 _tmp710=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6F7);_tmp711=
_tmp710.f1;_tmp712=_tmp710.f2;_tmp711=Cyc_NewControlFlow_use_Rval(env,_tmp6F7->loc,
_tmp711,_tmp712);return Cyc_NewControlFlow_add_vars(_tmp6EF,_tmp711,_tmp6F6,
_tmp6EF->unknown_all,decl->loc);}_LL3AA: {struct Cyc_Absyn_Letv_d_struct*_tmp6F8=(
struct Cyc_Absyn_Letv_d_struct*)_tmp6F0;if(_tmp6F8->tag != 3)goto _LL3AC;else{
_tmp6F9=_tmp6F8->f1;}}_LL3AB: return Cyc_NewControlFlow_add_vars(_tmp6EF,inflow,
_tmp6F9,_tmp6EF->unknown_none,decl->loc);_LL3AC: {struct Cyc_Absyn_Fn_d_struct*
_tmp6FA=(struct Cyc_Absyn_Fn_d_struct*)_tmp6F0;if(_tmp6FA->tag != 1)goto _LL3AE;
else{_tmp6FB=_tmp6FA->f1;}}_LL3AD: Cyc_NewControlFlow_check_nested_fun(_tmp6EF,
inflow,_tmp6FB);{void*t=(void*)_check_null(_tmp6FB->cached_typ);struct Cyc_Absyn_Vardecl*
_tmp713=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp6FB->fn_vardecl);struct Cyc_List_List*
_tmp9A1;return Cyc_NewControlFlow_add_vars(_tmp6EF,inflow,((_tmp9A1=
_region_malloc(env->r,sizeof(*_tmp9A1)),((_tmp9A1->hd=_tmp713,((_tmp9A1->tl=0,
_tmp9A1)))))),_tmp6EF->unknown_all,decl->loc);};_LL3AE: {struct Cyc_Absyn_Region_d_struct*
_tmp6FC=(struct Cyc_Absyn_Region_d_struct*)_tmp6F0;if(_tmp6FC->tag != 4)goto _LL3B0;
else{_tmp6FD=_tmp6FC->f1;_tmp6FE=_tmp6FC->f2;_tmp6FF=_tmp6FC->f3;_tmp700=_tmp6FC->f4;}}
_LL3AF: if(_tmp700 != 0){struct Cyc_Absyn_Exp*_tmp715=(struct Cyc_Absyn_Exp*)_tmp700;
union Cyc_CfFlowInfo_FlowInfo _tmp717;void*_tmp718;struct _tuple14 _tmp716=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp715);_tmp717=_tmp716.f1;_tmp718=_tmp716.f2;inflow=Cyc_NewControlFlow_use_Rval(
env,_tmp715->loc,_tmp717,_tmp718);}{struct Cyc_List_List _tmp9A2;struct Cyc_List_List
_tmp719=(_tmp9A2.hd=_tmp6FE,((_tmp9A2.tl=0,_tmp9A2)));return Cyc_NewControlFlow_add_vars(
_tmp6EF,inflow,(struct Cyc_List_List*)& _tmp719,_tmp6EF->unknown_all,decl->loc);};
_LL3B0:;_LL3B1: {struct Cyc_Core_Impossible_struct _tmp9A8;const char*_tmp9A7;
struct Cyc_Core_Impossible_struct*_tmp9A6;(int)_throw((void*)((_tmp9A6=_cycalloc(
sizeof(*_tmp9A6)),((_tmp9A6[0]=((_tmp9A8.tag=Cyc_Core_Impossible,((_tmp9A8.f1=((
_tmp9A7="anal_decl: unexpected decl variant",_tag_dyneither(_tmp9A7,sizeof(char),
35))),_tmp9A8)))),_tmp9A6)))));}_LL3A5:;}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){struct _RegionHandle _tmp71E=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp71E;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*
fenv=Cyc_CfFlowInfo_new_flow_env(frgn);Cyc_NewControlFlow_check_nested_fun(fenv,
Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);};_pop_region(frgn);}static
int Cyc_NewControlFlow_hash_ptr(void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle*_tmp71F=fenv->r;unsigned int _tmp720=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp720);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp721=inflow;int _tmp722;struct
_tuple13 _tmp723;struct Cyc_Dict_Dict _tmp724;struct Cyc_List_List*_tmp725;_LL3B8:
if((_tmp721.BottomFL).tag != 1)goto _LL3BA;_tmp722=(int)(_tmp721.BottomFL).val;
_LL3B9: {const char*_tmp9AB;void*_tmp9AA;(_tmp9AA=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9AB="check_fun",
_tag_dyneither(_tmp9AB,sizeof(char),10))),_tag_dyneither(_tmp9AA,sizeof(void*),0)));}
_LL3BA: if((_tmp721.ReachableFL).tag != 2)goto _LL3B7;_tmp723=(struct _tuple13)(
_tmp721.ReachableFL).val;_tmp724=_tmp723.f1;_tmp725=_tmp723.f2;_LL3BB: {struct
Cyc_List_List*atts;{void*_tmp728=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));
struct Cyc_Absyn_FnInfo _tmp72A;struct Cyc_List_List*_tmp72B;_LL3BD: {struct Cyc_Absyn_FnType_struct*
_tmp729=(struct Cyc_Absyn_FnType_struct*)_tmp728;if(_tmp729->tag != 9)goto _LL3BF;
else{_tmp72A=_tmp729->f1;_tmp72B=_tmp72A.attributes;}}_LL3BE: atts=_tmp72B;goto
_LL3BC;_LL3BF:;_LL3C0: {const char*_tmp9AE;void*_tmp9AD;(_tmp9AD=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9AE="check_fun: non-function type cached with fndecl_t",
_tag_dyneither(_tmp9AE,sizeof(char),50))),_tag_dyneither(_tmp9AD,sizeof(void*),0)));}
_LL3BC:;}for(0;atts != 0;atts=atts->tl){void*_tmp72E=(void*)atts->hd;int _tmp730;
int _tmp732;_LL3C2: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp72F=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp72E;if(_tmp72F->tag != 21)goto _LL3C4;else{_tmp730=_tmp72F->f1;}}_LL3C3: {
unsigned int j=(unsigned int)_tmp730;struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(
- 1,0);if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{const char*_tmp9B1;
void*_tmp9B0;(_tmp9B0=0,Cyc_Tcutil_terr(_tmp720,((_tmp9B1="noliveunique attribute exceeds number of parameters",
_tag_dyneither(_tmp9B1,sizeof(char),52))),_tag_dyneither(_tmp9B0,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp735=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));void*t=Cyc_Tcutil_compress(
_tmp735->type);if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp9B4;void*
_tmp9B3;(_tmp9B3=0,Cyc_Tcutil_terr(_tmp720,((_tmp9B4="noliveunique attribute requires unique pointer type",
_tag_dyneither(_tmp9B4,sizeof(char),52))),_tag_dyneither(_tmp9B3,sizeof(void*),0)));}
continue;}{void*elttype=Cyc_Tcutil_pointer_elt_type(t);void*_tmp738=Cyc_CfFlowInfo_make_unique_consumed(
fenv,elttype,bogus_exp,- 1,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_struct _tmp9B7;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp9B6;struct Cyc_CfFlowInfo_InitParam_struct*_tmp739=(_tmp9B6=_region_malloc(
_tmp71F,sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B7.tag=2,((_tmp9B7.f1=(int)j,((
_tmp9B7.f2=(void*)t,_tmp9B7)))))),_tmp9B6)));struct Cyc_CfFlowInfo_Place*_tmp9B8;
struct Cyc_CfFlowInfo_Place*_tmp73A=(_tmp9B8=_region_malloc(_tmp71F,sizeof(*
_tmp9B8)),((_tmp9B8->root=(void*)_tmp739,((_tmp9B8->fields=0,_tmp9B8)))));
_tmp724=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp724,(void*)_tmp739,_tmp738);{struct Cyc_CfFlowInfo_AddressOf_struct _tmp9BE;
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp9BD;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp9BB;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp9BA;_tmp724=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp724,(void*)((_tmp9BA=
_region_malloc(_tmp71F,sizeof(*_tmp9BA)),((_tmp9BA[0]=((_tmp9BB.tag=0,((_tmp9BB.f1=
_tmp735,_tmp9BB)))),_tmp9BA)))),(void*)((_tmp9BD=_region_malloc(_tmp71F,sizeof(*
_tmp9BD)),((_tmp9BD[0]=((_tmp9BE.tag=5,((_tmp9BE.f1=_tmp73A,_tmp9BE)))),_tmp9BD)))));}
goto _LL3C1;};};}_LL3C4: {struct Cyc_Absyn_Initializes_att_struct*_tmp731=(struct
Cyc_Absyn_Initializes_att_struct*)_tmp72E;if(_tmp731->tag != 20)goto _LL3C6;else{
_tmp732=_tmp731->f1;}}_LL3C5: {unsigned int j=(unsigned int)_tmp732;if(j > ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){{const char*_tmp9C1;void*_tmp9C0;(_tmp9C0=0,
Cyc_Tcutil_terr(_tmp720,((_tmp9C1="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp9C1,sizeof(char),51))),_tag_dyneither(_tmp9C0,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp744=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp745=Cyc_Tcutil_compress(
_tmp744->type);struct Cyc_Absyn_PtrInfo _tmp747;void*_tmp748;struct Cyc_Absyn_PtrAtts
_tmp749;union Cyc_Absyn_Constraint*_tmp74A;union Cyc_Absyn_Constraint*_tmp74B;
union Cyc_Absyn_Constraint*_tmp74C;_LL3C9: {struct Cyc_Absyn_PointerType_struct*
_tmp746=(struct Cyc_Absyn_PointerType_struct*)_tmp745;if(_tmp746->tag != 5)goto
_LL3CB;else{_tmp747=_tmp746->f1;_tmp748=_tmp747.elt_typ;_tmp749=_tmp747.ptr_atts;
_tmp74A=_tmp749.nullable;_tmp74B=_tmp749.bounds;_tmp74C=_tmp749.zero_term;}}
_LL3CA: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp74A)){
const char*_tmp9C4;void*_tmp9C3;(_tmp9C3=0,Cyc_Tcutil_terr(_tmp720,((_tmp9C4="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp9C4,sizeof(char),55))),_tag_dyneither(_tmp9C3,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp74B)){const char*_tmp9C7;void*_tmp9C6;(_tmp9C6=0,
Cyc_Tcutil_terr(_tmp720,((_tmp9C7="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp9C7,sizeof(char),57))),_tag_dyneither(_tmp9C6,sizeof(void*),0)));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp74C)){
const char*_tmp9CA;void*_tmp9C9;(_tmp9C9=0,Cyc_Tcutil_terr(_tmp720,((_tmp9CA="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp9CA,sizeof(char),77))),_tag_dyneither(_tmp9C9,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_struct _tmp9CD;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp9CC;struct Cyc_CfFlowInfo_InitParam_struct*_tmp753=(_tmp9CC=_region_malloc(
_tmp71F,sizeof(*_tmp9CC)),((_tmp9CC[0]=((_tmp9CD.tag=2,((_tmp9CD.f1=(int)j,((
_tmp9CD.f2=(void*)_tmp748,_tmp9CD)))))),_tmp9CC)));struct Cyc_CfFlowInfo_Place*
_tmp9CE;struct Cyc_CfFlowInfo_Place*_tmp754=(_tmp9CE=_region_malloc(_tmp71F,
sizeof(*_tmp9CE)),((_tmp9CE->root=(void*)_tmp753,((_tmp9CE->fields=0,_tmp9CE)))));
_tmp724=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp724,(void*)_tmp753,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp748,0,fenv->esc_none));{
struct Cyc_CfFlowInfo_AddressOf_struct _tmp9D4;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp9D3;struct Cyc_CfFlowInfo_VarRoot_struct _tmp9D1;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp9D0;_tmp724=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp724,(void*)((_tmp9D0=_region_malloc(_tmp71F,sizeof(*_tmp9D0)),((_tmp9D0[0]=((
_tmp9D1.tag=0,((_tmp9D1.f1=_tmp744,_tmp9D1)))),_tmp9D0)))),(void*)((_tmp9D3=
_region_malloc(_tmp71F,sizeof(*_tmp9D3)),((_tmp9D3[0]=((_tmp9D4.tag=5,((_tmp9D4.f1=
_tmp754,_tmp9D4)))),_tmp9D3)))));}{struct Cyc_List_List*_tmp9D5;param_roots=((
_tmp9D5=_region_malloc(_tmp71F,sizeof(*_tmp9D5)),((_tmp9D5->hd=_tmp754,((_tmp9D5->tl=
param_roots,_tmp9D5))))));}goto _LL3C8;};_LL3CB:;_LL3CC: {const char*_tmp9D8;void*
_tmp9D7;(_tmp9D7=0,Cyc_Tcutil_terr(_tmp720,((_tmp9D8="initializes attribute on non-pointer",
_tag_dyneither(_tmp9D8,sizeof(char),37))),_tag_dyneither(_tmp9D7,sizeof(void*),0)));}
_LL3C8:;}goto _LL3C1;};}_LL3C6:;_LL3C7: goto _LL3C1;_LL3C1:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp724,_tmp725);}_LL3B7:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp71F,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp9D9;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp9D9=_region_malloc(_tmp71F,sizeof(*_tmp9D9)),((
_tmp9D9->r=_tmp71F,((_tmp9D9->fenv=fenv,((_tmp9D9->iterate_again=1,((_tmp9D9->iteration_num=
0,((_tmp9D9->in_try=0,((_tmp9D9->tryflow=inflow,((_tmp9D9->all_changed=0,((
_tmp9D9->noreturn=noreturn,((_tmp9D9->param_roots=param_roots,((_tmp9D9->flow_table=
flow_table,_tmp9D9)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
union Cyc_CfFlowInfo_FlowInfo _tmp75F=outflow;int _tmp760;_LL3CE: if((_tmp75F.BottomFL).tag
!= 1)goto _LL3D0;_tmp760=(int)(_tmp75F.BottomFL).val;_LL3CF: goto _LL3CD;_LL3D0:;
_LL3D1: Cyc_NewControlFlow_check_init_params(_tmp720,env,outflow);if(noreturn){
const char*_tmp9DC;void*_tmp9DB;(_tmp9DB=0,Cyc_Tcutil_terr(_tmp720,((_tmp9DC="`noreturn' function might (implicitly) return",
_tag_dyneither(_tmp9DC,sizeof(char),46))),_tag_dyneither(_tmp9DB,sizeof(void*),0)));}
else{void*_tmp763=Cyc_Tcutil_compress(fd->ret_type);_LL3D3: {struct Cyc_Absyn_VoidType_struct*
_tmp764=(struct Cyc_Absyn_VoidType_struct*)_tmp763;if(_tmp764->tag != 0)goto _LL3D5;}
_LL3D4: goto _LL3D2;_LL3D5: {struct Cyc_Absyn_FloatType_struct*_tmp765=(struct Cyc_Absyn_FloatType_struct*)
_tmp763;if(_tmp765->tag != 7)goto _LL3D7;}_LL3D6: goto _LL3D8;_LL3D7: {struct Cyc_Absyn_IntType_struct*
_tmp766=(struct Cyc_Absyn_IntType_struct*)_tmp763;if(_tmp766->tag != 6)goto _LL3D9;}
_LL3D8:{const char*_tmp9DF;void*_tmp9DE;(_tmp9DE=0,Cyc_Tcutil_warn(_tmp720,((
_tmp9DF="function may not return a value",_tag_dyneither(_tmp9DF,sizeof(char),32))),
_tag_dyneither(_tmp9DE,sizeof(void*),0)));}goto _LL3D2;_LL3D9:;_LL3DA:{const char*
_tmp9E2;void*_tmp9E1;(_tmp9E1=0,Cyc_Tcutil_terr(_tmp720,((_tmp9E2="function may not return a value",
_tag_dyneither(_tmp9E2,sizeof(char),32))),_tag_dyneither(_tmp9E1,sizeof(void*),0)));}
goto _LL3D2;_LL3D2:;}goto _LL3CD;_LL3CD:;};};};}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp76C=((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp76E;struct Cyc_List_List*_tmp770;struct Cyc_List_List*
_tmp772;struct Cyc_List_List*_tmp774;_LL3DC: {struct Cyc_Absyn_Fn_d_struct*_tmp76D=(
struct Cyc_Absyn_Fn_d_struct*)_tmp76C;if(_tmp76D->tag != 1)goto _LL3DE;else{_tmp76E=
_tmp76D->f1;}}_LL3DD: Cyc_NewControlFlow_check_fun(_tmp76E);goto _LL3DB;_LL3DE: {
struct Cyc_Absyn_ExternC_d_struct*_tmp76F=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp76C;if(_tmp76F->tag != 12)goto _LL3E0;else{_tmp770=_tmp76F->f1;}}_LL3DF:
_tmp772=_tmp770;goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Using_d_struct*_tmp771=(
struct Cyc_Absyn_Using_d_struct*)_tmp76C;if(_tmp771->tag != 11)goto _LL3E2;else{
_tmp772=_tmp771->f2;}}_LL3E1: _tmp774=_tmp772;goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Namespace_d_struct*
_tmp773=(struct Cyc_Absyn_Namespace_d_struct*)_tmp76C;if(_tmp773->tag != 10)goto
_LL3E4;else{_tmp774=_tmp773->f2;}}_LL3E3: Cyc_NewControlFlow_cf_check(_tmp774);
goto _LL3DB;_LL3E4: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp775=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp76C;if(_tmp775->tag != 13)goto _LL3E6;}_LL3E5: goto _LL3DB;_LL3E6:;_LL3E7: goto
_LL3DB;_LL3DB:;}}
