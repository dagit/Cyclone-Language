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
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[
12];struct Cyc_Core_Open_Region_struct{char*tag;};extern char Cyc_Core_Free_Region[
12];struct Cyc_Core_Free_Region_struct{char*tag;};inline static void* arrcast(
struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd >> 20  || sz >> 12)
return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr == 
0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_rlist(
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
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*
tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct
Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{
char*tag;};struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct
Cyc_Dict_Dict d,void*k,void**ans);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_Dict d);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*
hash)(void*));void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*
val);void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct
Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;};
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};int Cyc_Position_error_p();struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
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
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
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
encloser){struct Cyc_NewControlFlow_CFAnnot_struct _tmp75B;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp75A;struct Cyc_NewControlFlow_CFAnnot_struct*_tmp759;enclosee->annot=(void*)((
_tmp759=_cycalloc(sizeof(*_tmp759)),((_tmp759[0]=((_tmp75B.tag=Cyc_NewControlFlow_CFAnnot,((
_tmp75B.f1=((_tmp75A.encloser=encloser,((_tmp75A.visited=0,_tmp75A)))),_tmp75B)))),
_tmp759))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*
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
_LL4: {struct Cyc_Core_Impossible_struct _tmp761;const char*_tmp760;struct Cyc_Core_Impossible_struct*
_tmp75F;(int)_throw((void*)((_tmp75F=_cycalloc(sizeof(*_tmp75F)),((_tmp75F[0]=((
_tmp761.tag=Cyc_Core_Impossible,((_tmp761.f1=((_tmp760="ControlFlow -- wrong stmt annotation",
_tag_dyneither(_tmp760,sizeof(char),37))),_tmp761)))),_tmp75F)))));}_LL0:;}
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*_tmp762;union Cyc_CfFlowInfo_FlowInfo*
res=(_tmp762=_region_malloc(env->r,sizeof(*_tmp762)),((_tmp762[0]=Cyc_CfFlowInfo_BottomFL(),
_tmp762)));((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union
Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);return
res;}return*sflow;}struct _tuple16{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union
Cyc_CfFlowInfo_FlowInfo*f2;};static struct _tuple16 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*
_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);_tmpB->visited=
env->iteration_num;{struct _tuple16 _tmp763;return(_tmp763.f1=_tmpB,((_tmp763.f2=
_tmpC,_tmp763)));};}static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*
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
_tmp767;void*_tmp766[1];struct Cyc_String_pa_struct _tmp765;(_tmp765.tag=0,((
_tmp765.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp15->name)),((_tmp766[0]=& _tmp765,Cyc_Tcutil_warn(_tmp11,((_tmp767="unique pointer(s) reachable from %s may become unreachable.",
_tag_dyneither(_tmp767,sizeof(char),60))),_tag_dyneither(_tmp766,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}goto _LL5;}goto _LL5;_LL8:;_LL9: goto _LL5;_LL5:;};}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
flow,*_tmp1F);if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple15 _tmp768;
struct _tuple15 _tmp22=(_tmp768.f1=flow,((_tmp768.f2=_tmp20,_tmp768)));union Cyc_CfFlowInfo_FlowInfo
_tmp23;struct _tuple12 _tmp24;struct Cyc_Dict_Dict _tmp25;union Cyc_CfFlowInfo_FlowInfo
_tmp26;struct _tuple12 _tmp27;struct Cyc_Dict_Dict _tmp28;_LL14: _tmp23=_tmp22.f1;if((
_tmp23.ReachableFL).tag != 2)goto _LL16;_tmp24=(struct _tuple12)(_tmp23.ReachableFL).val;
_tmp25=_tmp24.f1;_tmp26=_tmp22.f2;if((_tmp26.ReachableFL).tag != 2)goto _LL16;
_tmp27=(struct _tuple12)(_tmp26.ReachableFL).val;_tmp28=_tmp27.f1;_LL15: {struct
_tuple17 _tmp769;struct _tuple17 _tmp29=(_tmp769.f1=env,((_tmp769.f2=s->loc,((
_tmp769.f3=_tmp28,_tmp769)))));((void(*)(void(*f)(struct _tuple17*,void*,void*),
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
_tmp76C;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp76B;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp30=(_tmp76B=_region_malloc(fenv->r,sizeof(*_tmp76B)),((_tmp76B[0]=((_tmp76C.tag=
0,((_tmp76C.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp76C)))),_tmp76B)));_tmp2E=((
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
_tmp36,r)!= Cyc_CfFlowInfo_AllIL){const char*_tmp76F;void*_tmp76E;(_tmp76E=0,Cyc_Tcutil_terr(
loc,((_tmp76F="expression may not be fully initialized",_tag_dyneither(_tmp76F,
sizeof(char),40))),_tag_dyneither(_tmp76E,sizeof(void*),0)));}{struct Cyc_Dict_Dict
_tmp3A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp36,env->all_changed,r);if(
_tmp36.t == _tmp3A.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp3B=Cyc_CfFlowInfo_ReachableFL(
_tmp3A,_tmp37);Cyc_NewControlFlow_update_tryflow(env,_tmp3B);return _tmp3B;};};
_LL1D:;}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_NewControlFlow_check_nounique(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,void*
r){struct _tuple0 _tmp770;struct _tuple0 _tmp3D=(_tmp770.f1=Cyc_Tcutil_compress(t),((
_tmp770.f2=r,_tmp770)));void*_tmp3E;enum Cyc_CfFlowInfo_InitLevel _tmp40;void*
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
if(!Cyc_Tcutil_is_noalias_pointer(t))goto _LL33;_LL32:{const char*_tmp773;void*
_tmp772;(_tmp772=0,Cyc_Tcutil_warn(loc,((_tmp773="argument may still contain unique pointers",
_tag_dyneither(_tmp773,sizeof(char),43))),_tag_dyneither(_tmp772,sizeof(void*),0)));}
return;_LL33:;_LL34: return;_LL22:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*t,union
Cyc_CfFlowInfo_FlowInfo inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp6B=inflow;
int _tmp6C;struct _tuple12 _tmp6D;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*
_tmp6F;_LL36: if((_tmp6B.BottomFL).tag != 1)goto _LL38;_tmp6C=(int)(_tmp6B.BottomFL).val;
_LL37: return Cyc_CfFlowInfo_BottomFL();_LL38: if((_tmp6B.ReachableFL).tag != 2)goto
_LL35;_tmp6D=(struct _tuple12)(_tmp6B.ReachableFL).val;_tmp6E=_tmp6D.f1;_tmp6F=
_tmp6D.f2;_LL39: if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp776;void*
_tmp775;(_tmp775=0,Cyc_Tcutil_terr(loc,((_tmp776="noliveunique attribute requires unique pointer",
_tag_dyneither(_tmp776,sizeof(char),47))),_tag_dyneither(_tmp775,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{void*_tmp72=Cyc_Tcutil_pointer_elt_type(t);{
void*_tmp73=r;struct Cyc_CfFlowInfo_Place*_tmp75;_LL3B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp74=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp73;if(_tmp74->tag != 5)goto
_LL3D;else{_tmp75=_tmp74->f1;}}_LL3C: Cyc_NewControlFlow_check_nounique(env,loc,
_tmp72,Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp75));goto _LL3A;_LL3D:;_LL3E: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
env->fenv)->r,_tmp72)){const char*_tmp779;void*_tmp778;(_tmp778=0,Cyc_Tcutil_warn(
loc,((_tmp779="argument may contain live unique pointers",_tag_dyneither(_tmp779,
sizeof(char),42))),_tag_dyneither(_tmp778,sizeof(void*),0)));}return Cyc_NewControlFlow_use_Rval(
env,loc,inflow,r);_LL3A:;}{struct Cyc_Dict_Dict _tmp78=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp6E,env->all_changed,r);if(_tmp6E.t == _tmp78.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo _tmp79=Cyc_CfFlowInfo_ReachableFL(_tmp78,_tmp6F);Cyc_NewControlFlow_update_tryflow(
env,_tmp79);return _tmp79;};};};_LL35:;}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo
f1;struct Cyc_List_List*f2;};static struct _tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){if(es == 0){
struct _tuple19 _tmp77A;return(_tmp77A.f1=inflow,((_tmp77A.f2=0,_tmp77A)));}if(es->tl
== 0){union Cyc_CfFlowInfo_FlowInfo _tmp7C;void*_tmp7D;struct _tuple13 _tmp7B=Cyc_NewControlFlow_anal_Rexp(
env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp7C=_tmp7B.f1;_tmp7D=
_tmp7B.f2;{struct Cyc_List_List*_tmp77D;struct _tuple19 _tmp77C;return(_tmp77C.f1=
_tmp7C,((_tmp77C.f2=((_tmp77D=_region_malloc(rgn,sizeof(*_tmp77D)),((_tmp77D->hd=
_tmp7D,((_tmp77D->tl=0,_tmp77D)))))),_tmp77C)));};}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo
old_inflow;union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_List_List*rvals;do{
this_all_changed=(env->fenv)->mt_place_set;{struct Cyc_Dict_Dict*_tmp77E;env->all_changed=((
_tmp77E=_region_malloc(env->r,sizeof(*_tmp77E)),((_tmp77E[0]=(env->fenv)->mt_place_set,
_tmp77E))));}{union Cyc_CfFlowInfo_FlowInfo _tmp82;void*_tmp83;struct _tuple13
_tmp81=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;outflow=_tmp82;{struct Cyc_List_List*
_tmp77F;rvals=((_tmp77F=_region_malloc(rgn,sizeof(*_tmp77F)),((_tmp77F->hd=
_tmp83,((_tmp77F->tl=0,_tmp77F))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*
_tmp780;env->all_changed=((_tmp780=_region_malloc(env->r,sizeof(*_tmp780)),((
_tmp780[0]=(env->fenv)->mt_place_set,_tmp780))));}{union Cyc_CfFlowInfo_FlowInfo
_tmp87;void*_tmp88;struct _tuple13 _tmp86=Cyc_NewControlFlow_anal_Rexp(env,
others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmp87=_tmp86.f1;_tmp88=
_tmp86.f2;{struct Cyc_List_List*_tmp781;rvals=((_tmp781=_region_malloc(rgn,
sizeof(*_tmp781)),((_tmp781->hd=_tmp88,((_tmp781->tl=rvals,_tmp781))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp87,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
old_inflow=inflow;inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,
inflow,outflow);};}while(!Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*_tmp782;env->all_changed=((
_tmp782=_region_malloc(env->r,sizeof(*_tmp782)),((_tmp782[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0),_tmp782))));}Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple19 _tmp783;return(_tmp783.f1=outflow,((_tmp783.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp783)));};};}
static struct _tuple13 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){struct
_RegionHandle*_tmp8C=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp8E;struct Cyc_List_List*
_tmp8F;struct _tuple19 _tmp8D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp8C,env,
inflow,es,0,0);_tmp8E=_tmp8D.f1;_tmp8F=_tmp8D.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp90=_tmp8E;struct _tuple12 _tmp91;struct Cyc_Dict_Dict _tmp92;int _tmp93;_LL40: if((
_tmp90.ReachableFL).tag != 2)goto _LL42;_tmp91=(struct _tuple12)(_tmp90.ReachableFL).val;
_tmp92=_tmp91.f1;_LL41: for(0;_tmp8F != 0;(_tmp8F=_tmp8F->tl,es=((struct Cyc_List_List*)
_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp92,(void*)
_tmp8F->hd)== Cyc_CfFlowInfo_NoneIL){const char*_tmp786;void*_tmp785;(_tmp785=0,
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((
_tmp786="expression may not be initialized",_tag_dyneither(_tmp786,sizeof(char),
34))),_tag_dyneither(_tmp785,sizeof(void*),0)));}}goto _LL3F;_LL42: if((_tmp90.BottomFL).tag
!= 1)goto _LL3F;_tmp93=(int)(_tmp90.BottomFL).val;_LL43: goto _LL3F;_LL3F:;}{struct
_tuple13 _tmp787;return(_tmp787.f1=_tmp8E,((_tmp787.f2=(env->fenv)->unknown_all,
_tmp787)));};}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(
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
_tmpA0.BottomFL).val;_LL50: {struct _tuple15 _tmp788;return(_tmp788.f1=outflow,((
_tmp788.f2=outflow,_tmp788)));}_LL51: if((_tmpA0.ReachableFL).tag != 2)goto _LL4E;
_tmpA2=(struct _tuple12)(_tmpA0.ReachableFL).val;_tmpA3=_tmpA2.f1;_tmpA4=_tmpA2.f2;
_LL52: {union Cyc_CfFlowInfo_AbsLVal _tmpA6=(Cyc_NewControlFlow_anal_Lexp(env,
inflow,0,e)).f2;int _tmpA7;struct Cyc_CfFlowInfo_Place*_tmpA8;_LL54: if((_tmpA6.UnknownL).tag
!= 2)goto _LL56;_tmpA7=(int)(_tmpA6.UnknownL).val;_LL55: {struct _tuple15 _tmp789;
return(_tmp789.f1=outflow,((_tmp789.f2=outflow,_tmp789)));}_LL56: if((_tmpA6.PlaceL).tag
!= 1)goto _LL53;_tmpA8=(struct Cyc_CfFlowInfo_Place*)(_tmpA6.PlaceL).val;_LL57: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
struct _tuple15 _tmp78A;return(_tmp78A.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,nzval),_tmpA4),((_tmp78A.f2=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA3,env->all_changed,_tmpA8,(env->fenv)->zero),
_tmpA4),_tmp78A)));}_LL53:;}_LL4E:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
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
_tmpB5=(int)(_tmpB4.BottomFL).val;_LL5F: {struct _tuple13 _tmp78B;return(_tmp78B.f1=
f,((_tmp78B.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,0,(env->fenv)->unknown_all),
_tmp78B)));}_LL60: if((_tmpB4.ReachableFL).tag != 2)goto _LL5D;_tmpB6=(struct
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
_LL76:{struct Cyc_CfFlowInfo_NotZero_struct _tmp78E;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp78D;e->annot=(void*)((_tmp78D=_cycalloc(sizeof(*_tmp78D)),((_tmp78D[0]=((
_tmp78E.tag=Cyc_CfFlowInfo_NotZero,((_tmp78E.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp78E)))),_tmp78D))));}goto _LL74;_LL77:;_LL78: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL74;_LL74:;}goto _LL6F;_LL6F:;}goto _LL62;_LL67: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpBD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpBA;if(_tmpBD->tag != 5)goto
_LL69;else{_tmpBE=_tmpBD->f1;}}_LL68:{void*_tmpC9=e->annot;struct Cyc_List_List*
_tmpCB;_LL7A: {struct Cyc_CfFlowInfo_NotZero_struct*_tmpCA=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpC9;if(_tmpCA->tag != Cyc_CfFlowInfo_NotZero)goto _LL7C;else{_tmpCB=_tmpCA->f1;}}
_LL7B: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpCB))goto _LL7D;goto _LL79;_LL7C:;
_LL7D:{void*_tmpCC=e->r;_LL7F: {struct Cyc_Absyn_Subscript_e_struct*_tmpCD=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpCC;if(_tmpCD->tag != 24)goto _LL81;}_LL80:{
struct Cyc_CfFlowInfo_NotZero_struct _tmp791;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp790;e->annot=(void*)((_tmp790=_cycalloc(sizeof(*_tmp790)),((_tmp790[0]=((
_tmp791.tag=Cyc_CfFlowInfo_NotZero,((_tmp791.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp791)))),_tmp790))));}goto _LL7E;_LL81:;_LL82: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL7E;_LL7E:;}goto _LL79;_LL79:;}{struct _tuple13 _tmp792;return(_tmp792.f1=f,((
_tmp792.f2=Cyc_CfFlowInfo_lookup_place(_tmpB7,_tmpBE),_tmp792)));};_LL69: {
struct Cyc_CfFlowInfo_Zero_struct*_tmpBF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmpBA;if(_tmpBF->tag != 0)goto _LL6B;}_LL6A: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple13 _tmp793;return(_tmp793.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp793.f2=
Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,0,(env->fenv)->unknown_all),
_tmp793)));};_LL6B: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpC0=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpBA;if(_tmpC0->tag != 3)goto _LL6D;else{_tmpC1=_tmpC0->f1;}}_LL6C: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmpC1);goto _LL6E;_LL6D:;_LL6E:{void*_tmpD2=e->r;_LL84: {struct
Cyc_Absyn_Subscript_e_struct*_tmpD3=(struct Cyc_Absyn_Subscript_e_struct*)_tmpD2;
if(_tmpD3->tag != 24)goto _LL86;}_LL85:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp796;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp795;e->annot=(void*)((_tmp795=
_cycalloc(sizeof(*_tmp795)),((_tmp795[0]=((_tmp796.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp796.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpB8),_tmp796)))),
_tmp795))));}goto _LL83;_LL86:;_LL87: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL83;_LL83:;}goto _LL62;_LL62:;}else{void*_tmpD6=e->annot;struct Cyc_List_List*
_tmpD8;_LL89: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpD7=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmpD6;if(_tmpD7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL8B;else{_tmpD8=_tmpD7->f1;}}
_LL8A: if(!Cyc_CfFlowInfo_same_relns(_tmpB8,_tmpD8))goto _LL8C;goto _LL88;_LL8B:;
_LL8C:{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp799;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp798;e->annot=(void*)((_tmp798=_cycalloc(sizeof(*_tmp798)),((_tmp798[0]=((
_tmp799.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp799.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpB8),_tmp799)))),_tmp798))));}goto _LL88;_LL88:;}switch(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmpB7,r)){case Cyc_CfFlowInfo_NoneIL: _LL8D:{void*_tmpDB=r;_LL90: {
struct Cyc_CfFlowInfo_Consumed_struct*_tmpDC=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmpDB;if(_tmpDC->tag != 7)goto _LL92;}_LL91:{const char*_tmp79C;void*_tmp79B;(
_tmp79B=0,Cyc_Tcutil_terr(e->loc,((_tmp79C="attempt to use a unique pointer that has already been copied",
_tag_dyneither(_tmp79C,sizeof(char),61))),_tag_dyneither(_tmp79B,sizeof(void*),0)));}
goto _LL8F;_LL92:;_LL93: {const char*_tmp79F;void*_tmp79E;(_tmp79E=0,Cyc_Tcutil_terr(
e->loc,((_tmp79F="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp79F,sizeof(char),46))),_tag_dyneither(_tmp79E,sizeof(void*),0)));}_LL8F:;}
goto _LL8E;case Cyc_CfFlowInfo_AllIL: _LL8E: {struct _tuple13 _tmp7A0;return(_tmp7A0.f1=
f,((_tmp7A0.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,0,(env->fenv)->unknown_all),
_tmp7A0)));}case Cyc_CfFlowInfo_ThisIL: _LL94: {struct _tuple13 _tmp7A1;return(
_tmp7A1.f1=f,((_tmp7A1.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB0,0,(env->fenv)->unknown_none),
_tmp7A1)));}}_LL5D:;}_LL5B:;_LL5C: {struct Cyc_Core_Impossible_struct _tmp7A7;
const char*_tmp7A6;struct Cyc_Core_Impossible_struct*_tmp7A5;(int)_throw((void*)((
_tmp7A5=_cycalloc(sizeof(*_tmp7A5)),((_tmp7A5[0]=((_tmp7A7.tag=Cyc_Core_Impossible,((
_tmp7A7.f1=((_tmp7A6="right deref of non-pointer-type",_tag_dyneither(_tmp7A6,
sizeof(char),32))),_tmp7A7)))),_tmp7A5)))));}_LL58:;}static struct Cyc_List_List*
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
_LLAF:;_LLB0: continue;_LLAC:;}}}if(!found){struct Cyc_CfFlowInfo_Reln*_tmp7AA;
struct Cyc_List_List*_tmp7A9;return(_tmp7A9=_region_malloc(rgn,sizeof(*_tmp7A9)),((
_tmp7A9->hd=((_tmp7AA=_region_malloc(rgn,sizeof(*_tmp7AA)),((_tmp7AA->vd=_tmp107,((
_tmp7AA->rop=Cyc_CfFlowInfo_LessNumelts(_tmpF6),_tmp7AA)))))),((_tmp7A9->tl=
relns,_tmp7A9)))));}}return relns;_LLAA:;_LLAB: return relns;_LLA1:;}else{return
relns;}_LL9F:;_LLA0: return relns;_LL96:;}static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo
_tmp10E=outflow;int _tmp10F;struct _tuple12 _tmp110;struct Cyc_Dict_Dict _tmp111;
struct Cyc_List_List*_tmp112;_LLB2: if((_tmp10E.BottomFL).tag != 1)goto _LLB4;
_tmp10F=(int)(_tmp10E.BottomFL).val;_LLB3: {struct _tuple13 _tmp7AB;return(_tmp7AB.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp7AB.f2=rval,_tmp7AB)));}_LLB4: if((_tmp10E.ReachableFL).tag
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
_tmp7AE;void*_tmp7AD;(_tmp7AD=0,Cyc_Tcutil_warn(lexp->loc,((_tmp7AE="assignment may overwrite unique pointer(s)",
_tag_dyneither(_tmp7AE,sizeof(char),43))),_tag_dyneither(_tmp7AD,sizeof(void*),0)));}
goto _LLBB;_LLBB:;}Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple13
_tmp7AF;return(_tmp7AF.f1=outflow,((_tmp7AF.f2=rval,_tmp7AF)));};}_LLB9: if((
_tmp114.UnknownL).tag != 2)goto _LLB6;_tmp116=(int)(_tmp114.UnknownL).val;_LLBA: {
struct _tuple13 _tmp7B0;return(_tmp7B0.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,
outflow,rval),((_tmp7B0.f2=rval,_tmp7B0)));}_LLB6:;}_LLB1:;}static int Cyc_NewControlFlow_is_local_var_rooted_path(
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
_tmp14D=_tmp14C.f1;_tmp14E=_tmp14C.f2;{struct _tuple14 _tmp7B1;struct _tuple14
_tmp150=(_tmp7B1.f1=_tmp14D,((_tmp7B1.f2=_tmp14E,_tmp7B1)));union Cyc_CfFlowInfo_FlowInfo
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
struct Cyc_List_List _tmp7B2;struct Cyc_List_List _tmp15F=(_tmp7B2.hd=_tmp145,((
_tmp7B2.tl=0,_tmp7B2)));_tmp14D=Cyc_NewControlFlow_add_vars(_tmp137,_tmp14D,(
struct Cyc_List_List*)& _tmp15F,_tmp137->unknown_all,s->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp161;void*_tmp162;struct _tuple13 _tmp160=Cyc_NewControlFlow_anal_stmt_exp(env,
copy_ctxt,_tmp14D,_tmp146);_tmp161=_tmp160.f1;_tmp162=_tmp160.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp163=_tmp161;struct _tuple12 _tmp164;struct Cyc_Dict_Dict _tmp165;struct Cyc_List_List*
_tmp166;_LLF8: if((_tmp163.ReachableFL).tag != 2)goto _LLFA;_tmp164=(struct _tuple12)(
_tmp163.ReachableFL).val;_tmp165=_tmp164.f1;_tmp166=_tmp164.f2;_LLF9: _tmp165=Cyc_CfFlowInfo_assign_place(
_tmp137,s->loc,_tmp165,env->all_changed,_tmp156,_tmp15E);{union Cyc_CfFlowInfo_FlowInfo
_tmp167=Cyc_CfFlowInfo_ReachableFL(_tmp165,_tmp166);struct _tuple13 _tmp7B3;return(
_tmp7B3.f1=_tmp167,((_tmp7B3.f2=_tmp162,_tmp7B3)));};_LLFA:;_LLFB: {struct
_tuple13 _tmp7B4;return(_tmp7B4.f1=_tmp161,((_tmp7B4.f2=_tmp162,_tmp7B4)));}_LLF7:;};};};};};};}
_LLEA: _tmp157=_tmp150.f1;if((_tmp157.BottomFL).tag != 1)goto _LLEC;_tmp158=(int)(
_tmp157.BottomFL).val;_LLEB: {struct _tuple13 _tmp7B5;return(_tmp7B5.f1=_tmp14D,((
_tmp7B5.f2=_tmp137->unknown_all,_tmp7B5)));}_LLEC:;_LLED:{const char*_tmp7B8;void*
_tmp7B7;(_tmp7B7=0,Cyc_Tcutil_terr(_tmp143->loc,((_tmp7B8="bad alias expression--no unique path found",
_tag_dyneither(_tmp7B8,sizeof(char),43))),_tag_dyneither(_tmp7B7,sizeof(void*),0)));}{
struct _tuple13 _tmp7B9;return(_tmp7B9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7B9.f2=
_tmp137->unknown_all,_tmp7B9)));};_LLE7:;};}_LLE1: {struct Cyc_Absyn_Decl_s_struct*
_tmp147=(struct Cyc_Absyn_Decl_s_struct*)_tmp13A;if(_tmp147->tag != 12)goto _LLE3;
else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}_LLE2: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp148);s=_tmp149;goto _LLDC;_LLE3: {struct Cyc_Absyn_Exp_s_struct*
_tmp14A=(struct Cyc_Absyn_Exp_s_struct*)_tmp13A;if(_tmp14A->tag != 1)goto _LLE5;
else{_tmp14B=_tmp14A->f1;}}_LLE4: return Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp14B);_LLE5:;_LLE6: {struct Cyc_Core_Impossible_struct _tmp7BF;
const char*_tmp7BE;struct Cyc_Core_Impossible_struct*_tmp7BD;(int)_throw((void*)((
_tmp7BD=_cycalloc(sizeof(*_tmp7BD)),((_tmp7BD[0]=((_tmp7BF.tag=Cyc_Core_Impossible,((
_tmp7BF.f1=((_tmp7BE="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp7BE,
sizeof(char),33))),_tmp7BF)))),_tmp7BD)))));}_LLDC:;};}}struct _tuple20{union Cyc_CfFlowInfo_AbsLVal
f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple21{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);
static void _tmp860(unsigned int*_tmp85F,unsigned int*_tmp85E,void***_tmp85C,
struct Cyc_List_List**_tmp35C){for(*_tmp85F=0;*_tmp85F < *_tmp85E;(*_tmp85F)++){
void*_tmp85A;(*_tmp85C)[*_tmp85F]=((_tmp85A=(void*)((struct Cyc_List_List*)
_check_null(*_tmp35C))->hd,((*_tmp35C=(*_tmp35C)->tl,_tmp85A))));}}static struct
_tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int
copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp172=env->fenv;struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;{union Cyc_CfFlowInfo_FlowInfo
_tmp173=inflow;int _tmp174;struct _tuple12 _tmp175;struct Cyc_Dict_Dict _tmp176;
struct Cyc_List_List*_tmp177;_LLFD: if((_tmp173.BottomFL).tag != 1)goto _LLFF;
_tmp174=(int)(_tmp173.BottomFL).val;_LLFE: {struct _tuple13 _tmp7C0;return(_tmp7C0.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp7C0.f2=_tmp172->unknown_all,_tmp7C0)));}_LLFF:
if((_tmp173.ReachableFL).tag != 2)goto _LLFC;_tmp175=(struct _tuple12)(_tmp173.ReachableFL).val;
_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;_LL100: d=_tmp176;relns=_tmp177;_LLFC:;}if((
copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){union Cyc_CfFlowInfo_FlowInfo
_tmp17A;union Cyc_CfFlowInfo_AbsLVal _tmp17B;struct _tuple14 _tmp179=Cyc_NewControlFlow_anal_Lexp(
env,inflow,1,e);_tmp17A=_tmp179.f1;_tmp17B=_tmp179.f2;{struct _tuple14 _tmp7C1;
struct _tuple14 _tmp17D=(_tmp7C1.f1=_tmp17A,((_tmp7C1.f2=_tmp17B,_tmp7C1)));union
Cyc_CfFlowInfo_FlowInfo _tmp17E;struct _tuple12 _tmp17F;struct Cyc_Dict_Dict _tmp180;
struct Cyc_List_List*_tmp181;union Cyc_CfFlowInfo_AbsLVal _tmp182;struct Cyc_CfFlowInfo_Place*
_tmp183;_LL102: _tmp17E=_tmp17D.f1;if((_tmp17E.ReachableFL).tag != 2)goto _LL104;
_tmp17F=(struct _tuple12)(_tmp17E.ReachableFL).val;_tmp180=_tmp17F.f1;_tmp181=
_tmp17F.f2;_tmp182=_tmp17D.f2;if((_tmp182.PlaceL).tag != 1)goto _LL104;_tmp183=(
struct Cyc_CfFlowInfo_Place*)(_tmp182.PlaceL).val;_LL103: {void*_tmp184=Cyc_CfFlowInfo_lookup_place(
_tmp180,_tmp183);int needs_unconsume=0;if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,
_tmp184,& needs_unconsume)){{const char*_tmp7C4;void*_tmp7C3;(_tmp7C3=0,Cyc_Tcutil_terr(
e->loc,((_tmp7C4="expression attempts to copy unique value that may be copied elsewhere.",
_tag_dyneither(_tmp7C4,sizeof(char),71))),_tag_dyneither(_tmp7C3,sizeof(void*),0)));}{
struct _tuple13 _tmp7C5;return(_tmp7C5.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7C5.f2=
_tmp172->unknown_all,_tmp7C5)));};}else{if(needs_unconsume){struct _tuple13
_tmp7C6;return(_tmp7C6.f1=_tmp17A,((_tmp7C6.f2=Cyc_CfFlowInfo_make_unique_unconsumed(
_tmp172,_tmp184),_tmp7C6)));}else{void*_tmp189=Cyc_CfFlowInfo_make_unique_consumed(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,e,env->iteration_num,
_tmp184);struct Cyc_Dict_Dict _tmp18A=Cyc_CfFlowInfo_assign_place(_tmp172,e->loc,
_tmp180,env->all_changed,_tmp183,_tmp189);struct _tuple13 _tmp7C7;return(_tmp7C7.f1=
Cyc_CfFlowInfo_ReachableFL(_tmp18A,_tmp181),((_tmp7C7.f2=_tmp184,_tmp7C7)));}}}
_LL104:;_LL105: goto _LL101;_LL101:;};}{void*_tmp18C=e->r;struct Cyc_Absyn_Exp*
_tmp18E;enum Cyc_Absyn_Coercion _tmp18F;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*
_tmp193;struct Cyc_Absyn_Exp*_tmp195;union Cyc_Absyn_Cnst _tmp197;int _tmp198;union
Cyc_Absyn_Cnst _tmp19A;struct _tuple6 _tmp19B;int _tmp19C;union Cyc_Absyn_Cnst _tmp19E;
struct _tuple6 _tmp19F;void*_tmp1A1;struct Cyc_List_List*_tmp1A4;void*_tmp1AC;void*
_tmp1AF;struct Cyc_Absyn_Vardecl*_tmp1B1;void*_tmp1B3;struct Cyc_Absyn_Vardecl*
_tmp1B5;void*_tmp1B7;struct Cyc_Absyn_Vardecl*_tmp1B9;enum Cyc_Absyn_Primop
_tmp1BB;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_Absyn_Exp*
_tmp1C0;struct Cyc_Core_Opt*_tmp1C1;struct Cyc_Core_Opt _tmp1C2;struct Cyc_Absyn_Exp*
_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C5;struct Cyc_Core_Opt*_tmp1C6;struct Cyc_Absyn_Exp*
_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CA;struct Cyc_Absyn_Exp*
_tmp1CC;struct Cyc_Absyn_Exp*_tmp1CE;struct Cyc_List_List*_tmp1CF;struct Cyc_Absyn_MallocInfo
_tmp1D1;int _tmp1D2;struct Cyc_Absyn_Exp*_tmp1D3;void**_tmp1D4;struct Cyc_Absyn_Exp*
_tmp1D5;int _tmp1D6;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_Absyn_Exp*_tmp1D9;
struct Cyc_Absyn_Exp*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DC;struct Cyc_Absyn_Exp*
_tmp1DE;struct Cyc_Absyn_Exp*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E2;struct
_dyneither_ptr*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E5;struct _dyneither_ptr*_tmp1E6;
struct Cyc_Absyn_Exp*_tmp1E8;struct _dyneither_ptr*_tmp1E9;struct Cyc_Absyn_Exp*
_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Absyn_Exp*
_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_Absyn_Exp*
_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F5;struct Cyc_Absyn_Exp*_tmp1F6;struct Cyc_List_List*
_tmp1F8;struct Cyc_Absyn_Datatypedecl*_tmp1F9;struct Cyc_List_List*_tmp1FB;struct
Cyc_List_List*_tmp1FD;struct Cyc_List_List*_tmp1FF;struct Cyc_Absyn_Aggrdecl*
_tmp200;struct Cyc_Absyn_Aggrdecl _tmp201;enum Cyc_Absyn_AggrKind _tmp202;struct Cyc_Absyn_AggrdeclImpl*
_tmp203;struct Cyc_Absyn_AggrdeclImpl _tmp204;struct Cyc_List_List*_tmp205;struct
Cyc_List_List*_tmp208;struct Cyc_Absyn_Vardecl*_tmp20A;struct Cyc_Absyn_Exp*
_tmp20B;struct Cyc_Absyn_Exp*_tmp20C;int _tmp20D;struct Cyc_Absyn_Stmt*_tmp20F;void*
_tmp211;_LL107: {struct Cyc_Absyn_Cast_e_struct*_tmp18D=(struct Cyc_Absyn_Cast_e_struct*)
_tmp18C;if(_tmp18D->tag != 15)goto _LL109;else{_tmp18E=_tmp18D->f2;_tmp18F=_tmp18D->f4;
if(_tmp18F != Cyc_Absyn_NonNull_to_Null)goto _LL109;}}_LL108: {union Cyc_CfFlowInfo_FlowInfo
_tmp21A;void*_tmp21B;struct _tuple13 _tmp219=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,inflow,_tmp18E);_tmp21A=_tmp219.f1;_tmp21B=_tmp219.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp21D;void*_tmp21E;struct _tuple13 _tmp21C=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp21A,_tmp18E,_tmp21B);_tmp21D=_tmp21C.f1;_tmp21E=_tmp21C.f2;{struct
_tuple13 _tmp7C8;return(_tmp7C8.f1=_tmp21D,((_tmp7C8.f2=_tmp21B,_tmp7C8)));};};}
_LL109: {struct Cyc_Absyn_Cast_e_struct*_tmp190=(struct Cyc_Absyn_Cast_e_struct*)
_tmp18C;if(_tmp190->tag != 15)goto _LL10B;else{_tmp191=_tmp190->f2;}}_LL10A:
_tmp193=_tmp191;goto _LL10C;_LL10B: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp192=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp18C;if(_tmp192->tag != 13)
goto _LL10D;else{_tmp193=_tmp192->f1;}}_LL10C: _tmp195=_tmp193;goto _LL10E;_LL10D: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp194=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp18C;if(_tmp194->tag != 14)goto _LL10F;else{_tmp195=_tmp194->f1;}}_LL10E: return
Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp195);_LL10F: {struct Cyc_Absyn_Const_e_struct*
_tmp196=(struct Cyc_Absyn_Const_e_struct*)_tmp18C;if(_tmp196->tag != 0)goto _LL111;
else{_tmp197=_tmp196->f1;if((_tmp197.Null_c).tag != 1)goto _LL111;_tmp198=(int)(
_tmp197.Null_c).val;}}_LL110: goto _LL112;_LL111: {struct Cyc_Absyn_Const_e_struct*
_tmp199=(struct Cyc_Absyn_Const_e_struct*)_tmp18C;if(_tmp199->tag != 0)goto _LL113;
else{_tmp19A=_tmp199->f1;if((_tmp19A.Int_c).tag != 4)goto _LL113;_tmp19B=(struct
_tuple6)(_tmp19A.Int_c).val;_tmp19C=_tmp19B.f2;if(_tmp19C != 0)goto _LL113;}}
_LL112: {struct _tuple13 _tmp7C9;return(_tmp7C9.f1=inflow,((_tmp7C9.f2=_tmp172->zero,
_tmp7C9)));}_LL113: {struct Cyc_Absyn_Const_e_struct*_tmp19D=(struct Cyc_Absyn_Const_e_struct*)
_tmp18C;if(_tmp19D->tag != 0)goto _LL115;else{_tmp19E=_tmp19D->f1;if((_tmp19E.Int_c).tag
!= 4)goto _LL115;_tmp19F=(struct _tuple6)(_tmp19E.Int_c).val;}}_LL114: goto _LL116;
_LL115: {struct Cyc_Absyn_Var_e_struct*_tmp1A0=(struct Cyc_Absyn_Var_e_struct*)
_tmp18C;if(_tmp1A0->tag != 1)goto _LL117;else{_tmp1A1=(void*)_tmp1A0->f2;{struct
Cyc_Absyn_Funname_b_struct*_tmp1A2=(struct Cyc_Absyn_Funname_b_struct*)_tmp1A1;
if(_tmp1A2->tag != 2)goto _LL117;};}}_LL116: {struct _tuple13 _tmp7CA;return(_tmp7CA.f1=
inflow,((_tmp7CA.f2=_tmp172->notzeroall,_tmp7CA)));}_LL117: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1A3=(struct Cyc_Absyn_Datatype_e_struct*)_tmp18C;if(_tmp1A3->tag != 31)goto
_LL119;else{_tmp1A4=_tmp1A3->f1;if(_tmp1A4 != 0)goto _LL119;}}_LL118: goto _LL11A;
_LL119: {struct Cyc_Absyn_Const_e_struct*_tmp1A5=(struct Cyc_Absyn_Const_e_struct*)
_tmp18C;if(_tmp1A5->tag != 0)goto _LL11B;}_LL11A: goto _LL11C;_LL11B: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp1A6=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp18C;if(_tmp1A6->tag != 19)goto
_LL11D;}_LL11C: goto _LL11E;_LL11D: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp1A7=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp18C;if(_tmp1A7->tag != 18)goto _LL11F;}
_LL11E: goto _LL120;_LL11F: {struct Cyc_Absyn_Offsetof_e_struct*_tmp1A8=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp18C;if(_tmp1A8->tag != 20)goto _LL121;}_LL120: goto _LL122;_LL121: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp1A9=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp18C;if(_tmp1A9->tag != 33)goto
_LL123;}_LL122: goto _LL124;_LL123: {struct Cyc_Absyn_Enum_e_struct*_tmp1AA=(struct
Cyc_Absyn_Enum_e_struct*)_tmp18C;if(_tmp1AA->tag != 32)goto _LL125;}_LL124: {
struct _tuple13 _tmp7CB;return(_tmp7CB.f1=inflow,((_tmp7CB.f2=_tmp172->unknown_all,
_tmp7CB)));}_LL125: {struct Cyc_Absyn_Var_e_struct*_tmp1AB=(struct Cyc_Absyn_Var_e_struct*)
_tmp18C;if(_tmp1AB->tag != 1)goto _LL127;else{_tmp1AC=(void*)_tmp1AB->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp1AD=(struct Cyc_Absyn_Global_b_struct*)_tmp1AC;if(
_tmp1AD->tag != 1)goto _LL127;};}}_LL126: {struct _tuple13 _tmp7CC;return(_tmp7CC.f1=
inflow,((_tmp7CC.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0,_tmp172->unknown_all),_tmp7CC)));}_LL127: {struct Cyc_Absyn_Var_e_struct*
_tmp1AE=(struct Cyc_Absyn_Var_e_struct*)_tmp18C;if(_tmp1AE->tag != 1)goto _LL129;
else{_tmp1AF=(void*)_tmp1AE->f2;{struct Cyc_Absyn_Param_b_struct*_tmp1B0=(struct
Cyc_Absyn_Param_b_struct*)_tmp1AF;if(_tmp1B0->tag != 3)goto _LL129;else{_tmp1B1=
_tmp1B0->f1;}};}}_LL128: _tmp1B5=_tmp1B1;goto _LL12A;_LL129: {struct Cyc_Absyn_Var_e_struct*
_tmp1B2=(struct Cyc_Absyn_Var_e_struct*)_tmp18C;if(_tmp1B2->tag != 1)goto _LL12B;
else{_tmp1B3=(void*)_tmp1B2->f2;{struct Cyc_Absyn_Local_b_struct*_tmp1B4=(struct
Cyc_Absyn_Local_b_struct*)_tmp1B3;if(_tmp1B4->tag != 4)goto _LL12B;else{_tmp1B5=
_tmp1B4->f1;}};}}_LL12A: _tmp1B9=_tmp1B5;goto _LL12C;_LL12B: {struct Cyc_Absyn_Var_e_struct*
_tmp1B6=(struct Cyc_Absyn_Var_e_struct*)_tmp18C;if(_tmp1B6->tag != 1)goto _LL12D;
else{_tmp1B7=(void*)_tmp1B6->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp1B8=(struct Cyc_Absyn_Pat_b_struct*)
_tmp1B7;if(_tmp1B8->tag != 5)goto _LL12D;else{_tmp1B9=_tmp1B8->f1;}};}}_LL12C: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp7D2;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp7D1;struct _tuple13 _tmp7D0;return(_tmp7D0.f1=inflow,((_tmp7D0.f2=((void*(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,(void*)((_tmp7D2=_region_malloc(
env->r,sizeof(*_tmp7D2)),((_tmp7D2[0]=((_tmp7D1.tag=0,((_tmp7D1.f1=_tmp1B9,
_tmp7D1)))),_tmp7D2))))),_tmp7D0)));}_LL12D: {struct Cyc_Absyn_Primop_e_struct*
_tmp1BA=(struct Cyc_Absyn_Primop_e_struct*)_tmp18C;if(_tmp1BA->tag != 3)goto _LL12F;
else{_tmp1BB=_tmp1BA->f1;_tmp1BC=_tmp1BA->f2;}}_LL12E: {union Cyc_CfFlowInfo_FlowInfo
_tmp228;void*_tmp229;struct _tuple13 _tmp227=Cyc_NewControlFlow_anal_use_ints(env,
inflow,_tmp1BC);_tmp228=_tmp227.f1;_tmp229=_tmp227.f2;{struct _tuple13 _tmp7D3;
return(_tmp7D3.f1=_tmp228,((_tmp7D3.f2=_tmp229,_tmp7D3)));};}_LL12F: {struct Cyc_Absyn_Increment_e_struct*
_tmp1BD=(struct Cyc_Absyn_Increment_e_struct*)_tmp18C;if(_tmp1BD->tag != 5)goto
_LL131;else{_tmp1BE=_tmp1BD->f1;}}_LL130: {struct Cyc_List_List _tmp7D4;struct Cyc_List_List
_tmp22B=(_tmp7D4.hd=_tmp1BE,((_tmp7D4.tl=0,_tmp7D4)));union Cyc_CfFlowInfo_FlowInfo
_tmp22D;struct _tuple13 _tmp22C=Cyc_NewControlFlow_anal_use_ints(env,inflow,(
struct Cyc_List_List*)& _tmp22B);_tmp22D=_tmp22C.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp22F;struct _tuple14 _tmp22E=Cyc_NewControlFlow_anal_Lexp(env,_tmp22D,0,_tmp1BE);
_tmp22F=_tmp22E.f2;{struct _tuple20 _tmp7D5;struct _tuple20 _tmp231=(_tmp7D5.f1=
_tmp22F,((_tmp7D5.f2=_tmp22D,_tmp7D5)));union Cyc_CfFlowInfo_AbsLVal _tmp232;
struct Cyc_CfFlowInfo_Place*_tmp233;union Cyc_CfFlowInfo_FlowInfo _tmp234;struct
_tuple12 _tmp235;struct Cyc_Dict_Dict _tmp236;struct Cyc_List_List*_tmp237;_LL172:
_tmp232=_tmp231.f1;if((_tmp232.PlaceL).tag != 1)goto _LL174;_tmp233=(struct Cyc_CfFlowInfo_Place*)(
_tmp232.PlaceL).val;_tmp234=_tmp231.f2;if((_tmp234.ReachableFL).tag != 2)goto
_LL174;_tmp235=(struct _tuple12)(_tmp234.ReachableFL).val;_tmp236=_tmp235.f1;
_tmp237=_tmp235.f2;_LL173: _tmp237=Cyc_CfFlowInfo_reln_kill_exp(_tmp172->r,
_tmp237,_tmp1BE);_tmp22D=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
_tmp172,_tmp1BE->loc,_tmp236,env->all_changed,_tmp233,_tmp172->unknown_all),
_tmp237);goto _LL171;_LL174:;_LL175: goto _LL171;_LL171:;}{struct _tuple13 _tmp7D6;
return(_tmp7D6.f1=_tmp22D,((_tmp7D6.f2=_tmp172->unknown_all,_tmp7D6)));};};}
_LL131: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1BF=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp18C;if(_tmp1BF->tag != 4)goto _LL133;else{_tmp1C0=_tmp1BF->f1;_tmp1C1=_tmp1BF->f2;
if(_tmp1C1 == 0)goto _LL133;_tmp1C2=*_tmp1C1;_tmp1C3=_tmp1BF->f3;}}_LL132: if(
copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){{const char*_tmp7D9;void*_tmp7D8;(_tmp7D8=0,Cyc_Tcutil_terr(
e->loc,((_tmp7D9="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7D9,sizeof(char),58))),_tag_dyneither(_tmp7D8,sizeof(void*),0)));}{
struct _tuple13 _tmp7DA;return(_tmp7DA.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7DA.f2=
_tmp172->unknown_all,_tmp7DA)));};}{struct Cyc_List_List _tmp7DB;struct Cyc_List_List
_tmp23D=(_tmp7DB.hd=_tmp1C3,((_tmp7DB.tl=0,_tmp7DB)));struct Cyc_List_List _tmp7DC;
struct Cyc_List_List _tmp23E=(_tmp7DC.hd=_tmp1C0,((_tmp7DC.tl=(struct Cyc_List_List*)&
_tmp23D,_tmp7DC)));union Cyc_CfFlowInfo_FlowInfo _tmp240;struct _tuple13 _tmp23F=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp23E);_tmp240=_tmp23F.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp242;struct _tuple14 _tmp241=Cyc_NewControlFlow_anal_Lexp(env,_tmp240,0,_tmp1C0);
_tmp242=_tmp241.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp243=_tmp240;struct _tuple12
_tmp244;struct Cyc_Dict_Dict _tmp245;struct Cyc_List_List*_tmp246;_LL177: if((
_tmp243.ReachableFL).tag != 2)goto _LL179;_tmp244=(struct _tuple12)(_tmp243.ReachableFL).val;
_tmp245=_tmp244.f1;_tmp246=_tmp244.f2;_LL178:{union Cyc_CfFlowInfo_AbsLVal _tmp247=
_tmp242;struct Cyc_CfFlowInfo_Place*_tmp248;int _tmp249;_LL17C: if((_tmp247.PlaceL).tag
!= 1)goto _LL17E;_tmp248=(struct Cyc_CfFlowInfo_Place*)(_tmp247.PlaceL).val;_LL17D:
_tmp246=Cyc_CfFlowInfo_reln_kill_exp(_tmp172->r,_tmp246,_tmp1C0);_tmp245=Cyc_CfFlowInfo_assign_place(
_tmp172,_tmp1C0->loc,_tmp245,env->all_changed,_tmp248,_tmp172->unknown_all);
_tmp240=Cyc_CfFlowInfo_ReachableFL(_tmp245,_tmp246);goto _LL17B;_LL17E: if((
_tmp247.UnknownL).tag != 2)goto _LL17B;_tmp249=(int)(_tmp247.UnknownL).val;_LL17F:
goto _LL17B;_LL17B:;}goto _LL176;_LL179:;_LL17A: goto _LL176;_LL176:;}{struct
_tuple13 _tmp7DD;return(_tmp7DD.f1=_tmp240,((_tmp7DD.f2=_tmp172->unknown_all,
_tmp7DD)));};};};_LL133: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1C4=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp18C;if(_tmp1C4->tag != 4)goto _LL135;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C4->f2;
if(_tmp1C6 != 0)goto _LL135;_tmp1C7=_tmp1C4->f3;}}_LL134: if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(
env->r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){{const char*
_tmp7E0;void*_tmp7DF;(_tmp7DF=0,Cyc_Tcutil_terr(e->loc,((_tmp7E0="cannot track unique pointers through multiple assignments",
_tag_dyneither(_tmp7E0,sizeof(char),58))),_tag_dyneither(_tmp7DF,sizeof(void*),0)));}{
struct _tuple13 _tmp7E1;return(_tmp7E1.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7E1.f2=
_tmp172->unknown_all,_tmp7E1)));};}{struct Cyc_Dict_Dict*_tmp250=env->all_changed;
while(1){{struct Cyc_Dict_Dict*_tmp7E2;env->all_changed=((_tmp7E2=_region_malloc(
env->r,sizeof(*_tmp7E2)),((_tmp7E2[0]=_tmp172->mt_place_set,_tmp7E2))));}{union
Cyc_CfFlowInfo_FlowInfo _tmp253;union Cyc_CfFlowInfo_AbsLVal _tmp254;struct _tuple14
_tmp252=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1C5);_tmp253=_tmp252.f1;
_tmp254=_tmp252.f2;{struct Cyc_Dict_Dict _tmp255=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp7E3;env->all_changed=((
_tmp7E3=_region_malloc(env->r,sizeof(*_tmp7E3)),((_tmp7E3[0]=_tmp172->mt_place_set,
_tmp7E3))));}{union Cyc_CfFlowInfo_FlowInfo _tmp258;void*_tmp259;struct _tuple13
_tmp257=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C7);_tmp258=_tmp257.f1;
_tmp259=_tmp257.f2;{struct Cyc_Dict_Dict _tmp25A=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo _tmp25B=Cyc_CfFlowInfo_after_flow(
_tmp172,(struct Cyc_Dict_Dict*)& _tmp255,_tmp253,_tmp258,_tmp255,_tmp25A);union Cyc_CfFlowInfo_FlowInfo
_tmp25C=Cyc_CfFlowInfo_join_flow(_tmp172,_tmp250,inflow,_tmp25B);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp25C,inflow)){if(_tmp250 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp250,Cyc_CfFlowInfo_union_place_set(
_tmp255,_tmp25A,0),0);}return Cyc_NewControlFlow_do_assign(_tmp172,env,_tmp25B,
_tmp1C5,_tmp254,_tmp1C7,_tmp259,e->loc);}inflow=_tmp25C;};};};};}};_LL135: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp1C8=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp18C;if(_tmp1C8->tag != 9)goto _LL137;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=_tmp1C8->f2;}}
_LL136: {union Cyc_CfFlowInfo_FlowInfo _tmp25E;void*_tmp25F;struct _tuple13 _tmp25D=
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1C9);_tmp25E=_tmp25D.f1;_tmp25F=
_tmp25D.f2;return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp25E,_tmp1CA);}
_LL137: {struct Cyc_Absyn_Throw_e_struct*_tmp1CB=(struct Cyc_Absyn_Throw_e_struct*)
_tmp18C;if(_tmp1CB->tag != 12)goto _LL139;else{_tmp1CC=_tmp1CB->f1;}}_LL138: {
union Cyc_CfFlowInfo_FlowInfo _tmp261;void*_tmp262;struct _tuple13 _tmp260=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1CC);_tmp261=_tmp260.f1;_tmp262=_tmp260.f2;Cyc_NewControlFlow_use_Rval(
env,_tmp1CC->loc,_tmp261,_tmp262);{struct _tuple13 _tmp7E4;return(_tmp7E4.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp7E4.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0,_tmp172->unknown_all),_tmp7E4)));};}_LL139: {struct
Cyc_Absyn_FnCall_e_struct*_tmp1CD=(struct Cyc_Absyn_FnCall_e_struct*)_tmp18C;if(
_tmp1CD->tag != 11)goto _LL13B;else{_tmp1CE=_tmp1CD->f1;_tmp1CF=_tmp1CD->f2;}}
_LL13A: {struct _RegionHandle*_tmp264=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp267;
struct Cyc_List_List*_tmp268;struct Cyc_List_List*_tmp7E5;struct _tuple19 _tmp266=
Cyc_NewControlFlow_anal_unordered_Rexps(_tmp264,env,inflow,((_tmp7E5=
_region_malloc(_tmp264,sizeof(*_tmp7E5)),((_tmp7E5->hd=_tmp1CE,((_tmp7E5->tl=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(
_tmp264,_tmp1CF),_tmp7E5)))))),0,1);_tmp267=_tmp266.f1;_tmp268=_tmp266.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp269=Cyc_NewControlFlow_use_Rval(env,_tmp1CE->loc,
_tmp267,(void*)((struct Cyc_List_List*)_check_null(_tmp268))->hd);_tmp268=_tmp268->tl;{
struct Cyc_List_List*init_params=0;struct Cyc_List_List*nolive_unique_params=0;{
void*_tmp26A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1CE->topt))->v);struct Cyc_Absyn_PtrInfo _tmp26C;void*_tmp26D;_LL181: {struct
Cyc_Absyn_PointerType_struct*_tmp26B=(struct Cyc_Absyn_PointerType_struct*)
_tmp26A;if(_tmp26B->tag != 5)goto _LL183;else{_tmp26C=_tmp26B->f1;_tmp26D=_tmp26C.elt_typ;}}
_LL182:{void*_tmp26E=Cyc_Tcutil_compress(_tmp26D);struct Cyc_Absyn_FnInfo _tmp270;
struct Cyc_List_List*_tmp271;_LL186: {struct Cyc_Absyn_FnType_struct*_tmp26F=(
struct Cyc_Absyn_FnType_struct*)_tmp26E;if(_tmp26F->tag != 10)goto _LL188;else{
_tmp270=_tmp26F->f1;_tmp271=_tmp270.attributes;}}_LL187: for(0;_tmp271 != 0;
_tmp271=_tmp271->tl){void*_tmp272=(void*)_tmp271->hd;int _tmp274;int _tmp276;
_LL18B: {struct Cyc_Absyn_Initializes_att_struct*_tmp273=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp272;if(_tmp273->tag != 20)goto _LL18D;else{_tmp274=_tmp273->f1;}}_LL18C:{
struct Cyc_List_List*_tmp7E6;init_params=((_tmp7E6=_region_malloc(_tmp264,sizeof(*
_tmp7E6)),((_tmp7E6->hd=(void*)_tmp274,((_tmp7E6->tl=init_params,_tmp7E6))))));}
goto _LL18A;_LL18D: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp275=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp272;if(_tmp275->tag != 21)goto _LL18F;else{_tmp276=_tmp275->f1;}}_LL18E:{
struct Cyc_List_List*_tmp7E7;nolive_unique_params=((_tmp7E7=_region_malloc(
_tmp264,sizeof(*_tmp7E7)),((_tmp7E7->hd=(void*)_tmp276,((_tmp7E7->tl=
nolive_unique_params,_tmp7E7))))));}goto _LL18A;_LL18F:;_LL190: goto _LL18A;_LL18A:;}
goto _LL185;_LL188:;_LL189: {const char*_tmp7EA;void*_tmp7E9;(_tmp7E9=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7EA="anal_Rexp: bad function type",
_tag_dyneither(_tmp7EA,sizeof(char),29))),_tag_dyneither(_tmp7E9,sizeof(void*),0)));}
_LL185:;}goto _LL180;_LL183:;_LL184: {const char*_tmp7ED;void*_tmp7EC;(_tmp7EC=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp7ED="anal_Rexp: bad function type",_tag_dyneither(_tmp7ED,sizeof(char),29))),
_tag_dyneither(_tmp7EC,sizeof(void*),0)));}_LL180:;}{int i=1;for(0;_tmp268 != 0;(((
_tmp268=_tmp268->tl,_tmp1CF=((struct Cyc_List_List*)_check_null(_tmp1CF))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i) && 
!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){
_tmp269=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1CF))->hd)->loc,_tmp269,(void*)_tmp268->hd);continue;}if(((int(*)(
struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i)){_tmp269=Cyc_NewControlFlow_use_nounique_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1CF))->hd)->loc,(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1CF))->hd)->topt))->v,_tmp269,(void*)_tmp268->hd);continue;}{
union Cyc_CfFlowInfo_FlowInfo _tmp27D=_tmp267;int _tmp27E;struct _tuple12 _tmp27F;
struct Cyc_Dict_Dict _tmp280;_LL192: if((_tmp27D.BottomFL).tag != 1)goto _LL194;
_tmp27E=(int)(_tmp27D.BottomFL).val;_LL193: goto _LL191;_LL194: if((_tmp27D.ReachableFL).tag
!= 2)goto _LL191;_tmp27F=(struct _tuple12)(_tmp27D.ReachableFL).val;_tmp280=
_tmp27F.f1;_LL195: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp280,(void*)_tmp268->hd)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp7F0;void*_tmp7EF;(_tmp7EF=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1CF))->hd)->loc,((
_tmp7F0="expression may not be initialized",_tag_dyneither(_tmp7F0,sizeof(char),
34))),_tag_dyneither(_tmp7EF,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo
_tmp283=_tmp269;int _tmp284;struct _tuple12 _tmp285;struct Cyc_Dict_Dict _tmp286;
struct Cyc_List_List*_tmp287;_LL197: if((_tmp283.BottomFL).tag != 1)goto _LL199;
_tmp284=(int)(_tmp283.BottomFL).val;_LL198: goto _LL196;_LL199: if((_tmp283.ReachableFL).tag
!= 2)goto _LL196;_tmp285=(struct _tuple12)(_tmp283.ReachableFL).val;_tmp286=
_tmp285.f1;_tmp287=_tmp285.f2;_LL19A: {struct Cyc_Dict_Dict _tmp288=Cyc_CfFlowInfo_escape_deref(
_tmp172,_tmp286,env->all_changed,(void*)_tmp268->hd);{void*_tmp289=(void*)
_tmp268->hd;struct Cyc_CfFlowInfo_Place*_tmp28B;_LL19C: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp28A=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp289;if(_tmp28A->tag != 5)goto
_LL19E;else{_tmp28B=_tmp28A->f1;}}_LL19D:{void*_tmp28C=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1CF))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp28E;void*
_tmp28F;_LL1A1: {struct Cyc_Absyn_PointerType_struct*_tmp28D=(struct Cyc_Absyn_PointerType_struct*)
_tmp28C;if(_tmp28D->tag != 5)goto _LL1A3;else{_tmp28E=_tmp28D->f1;_tmp28F=_tmp28E.elt_typ;}}
_LL1A2: _tmp288=Cyc_CfFlowInfo_assign_place(_tmp172,((struct Cyc_Absyn_Exp*)
_tmp1CF->hd)->loc,_tmp288,env->all_changed,_tmp28B,Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,_tmp28F,0,_tmp172->esc_all));goto _LL1A0;_LL1A3:;_LL1A4: {const char*
_tmp7F3;void*_tmp7F2;(_tmp7F2=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7F3="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp7F3,sizeof(char),39))),_tag_dyneither(_tmp7F2,sizeof(void*),0)));}
_LL1A0:;}goto _LL19B;_LL19E:;_LL19F: goto _LL19B;_LL19B:;}_tmp269=Cyc_CfFlowInfo_ReachableFL(
_tmp288,_tmp287);goto _LL196;}_LL196:;}goto _LL191;_LL191:;};}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1CE->topt))->v)){struct _tuple13
_tmp7F4;return(_tmp7F4.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F4.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0,_tmp172->unknown_all),
_tmp7F4)));}else{struct _tuple13 _tmp7F5;return(_tmp7F5.f1=_tmp269,((_tmp7F5.f2=
Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
0,_tmp172->unknown_all),_tmp7F5)));}};};}_LL13B: {struct Cyc_Absyn_Malloc_e_struct*
_tmp1D0=(struct Cyc_Absyn_Malloc_e_struct*)_tmp18C;if(_tmp1D0->tag != 34)goto
_LL13D;else{_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D1.is_calloc;_tmp1D3=_tmp1D1.rgn;
_tmp1D4=_tmp1D1.elt_type;_tmp1D5=_tmp1D1.num_elts;_tmp1D6=_tmp1D1.fat_result;}}
_LL13C: {struct Cyc_CfFlowInfo_MallocPt_struct _tmp7F8;struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp7F7;void*root=(void*)((_tmp7F7=_region_malloc(_tmp172->r,sizeof(*_tmp7F7)),((
_tmp7F7[0]=((_tmp7F8.tag=1,((_tmp7F8.f1=_tmp1D5,((_tmp7F8.f2=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp7F8)))))),_tmp7F7))));struct
Cyc_CfFlowInfo_Place*_tmp7F9;struct Cyc_CfFlowInfo_Place*place=(_tmp7F9=
_region_malloc(_tmp172->r,sizeof(*_tmp7F9)),((_tmp7F9->root=root,((_tmp7F9->fields=
0,_tmp7F9)))));struct Cyc_CfFlowInfo_AddressOf_struct _tmp7FC;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp7FB;void*rval=(void*)((_tmp7FB=_region_malloc(_tmp172->r,sizeof(*_tmp7FB)),((
_tmp7FB[0]=((_tmp7FC.tag=5,((_tmp7FC.f1=place,_tmp7FC)))),_tmp7FB))));void*
place_val;if(_tmp1D6)place_val=_tmp172->notzeroall;else{if(_tmp1D2)place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,*((void**)_check_null(_tmp1D4)),0,_tmp172->zero);else{place_val=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,*((void**)_check_null(_tmp1D4)),0,_tmp172->unknown_none);}}{union Cyc_CfFlowInfo_FlowInfo
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);if(_tmp1D3 != 0){struct _RegionHandle*_tmp294=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp297;struct Cyc_List_List*_tmp298;struct Cyc_Absyn_Exp*_tmp7FD[2];struct
_tuple19 _tmp296=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp294,env,inflow,((
_tmp7FD[1]=_tmp1D5,((_tmp7FD[0]=(struct Cyc_Absyn_Exp*)_tmp1D3,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp294,
_tag_dyneither(_tmp7FD,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp297=_tmp296.f1;
_tmp298=_tmp296.f2;outflow=_tmp297;}else{union Cyc_CfFlowInfo_FlowInfo _tmp29A;
struct _tuple13 _tmp299=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D5);_tmp29A=
_tmp299.f1;outflow=_tmp29A;}{union Cyc_CfFlowInfo_FlowInfo _tmp29B=outflow;int
_tmp29C;struct _tuple12 _tmp29D;struct Cyc_Dict_Dict _tmp29E;struct Cyc_List_List*
_tmp29F;_LL1A6: if((_tmp29B.BottomFL).tag != 1)goto _LL1A8;_tmp29C=(int)(_tmp29B.BottomFL).val;
_LL1A7: {struct _tuple13 _tmp7FE;return(_tmp7FE.f1=outflow,((_tmp7FE.f2=rval,
_tmp7FE)));}_LL1A8: if((_tmp29B.ReachableFL).tag != 2)goto _LL1A5;_tmp29D=(struct
_tuple12)(_tmp29B.ReachableFL).val;_tmp29E=_tmp29D.f1;_tmp29F=_tmp29D.f2;_LL1A9: {
struct _tuple13 _tmp7FF;return(_tmp7FF.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp29E,root,place_val),
_tmp29F),((_tmp7FF.f2=rval,_tmp7FF)));}_LL1A5:;};};}_LL13D: {struct Cyc_Absyn_Swap_e_struct*
_tmp1D7=(struct Cyc_Absyn_Swap_e_struct*)_tmp18C;if(_tmp1D7->tag != 35)goto _LL13F;
else{_tmp1D8=_tmp1D7->f1;_tmp1D9=_tmp1D7->f2;}}_LL13E: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle*_tmp2A7=env->r;{
union Cyc_CfFlowInfo_FlowInfo _tmp2AA;struct Cyc_List_List*_tmp2AB;struct Cyc_Absyn_Exp*
_tmp800[2];struct _tuple19 _tmp2A9=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2A7,
env,inflow,((_tmp800[1]=_tmp1D9,((_tmp800[0]=_tmp1D8,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2A7,
_tag_dyneither(_tmp800,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp2AA=_tmp2A9.f1;
_tmp2AB=_tmp2A9.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AB))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AB->tl))->hd;outflow=
_tmp2AA;}{union Cyc_CfFlowInfo_AbsLVal _tmp2AD;struct _tuple14 _tmp2AC=Cyc_NewControlFlow_anal_Lexp(
env,outflow,0,_tmp1D8);_tmp2AD=_tmp2AC.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp2AF;
struct _tuple14 _tmp2AE=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp1D9);
_tmp2AF=_tmp2AE.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2B0=outflow;struct _tuple12
_tmp2B1;struct Cyc_Dict_Dict _tmp2B2;struct Cyc_List_List*_tmp2B3;_LL1AB: if((
_tmp2B0.ReachableFL).tag != 2)goto _LL1AD;_tmp2B1=(struct _tuple12)(_tmp2B0.ReachableFL).val;
_tmp2B2=_tmp2B1.f1;_tmp2B3=_tmp2B1.f2;_LL1AC:{union Cyc_CfFlowInfo_AbsLVal _tmp2B4=
_tmp2AD;struct Cyc_CfFlowInfo_Place*_tmp2B5;int _tmp2B6;_LL1B0: if((_tmp2B4.PlaceL).tag
!= 1)goto _LL1B2;_tmp2B5=(struct Cyc_CfFlowInfo_Place*)(_tmp2B4.PlaceL).val;_LL1B1:
_tmp2B2=Cyc_CfFlowInfo_assign_place(_tmp172,_tmp1D8->loc,_tmp2B2,env->all_changed,
_tmp2B5,right_rval);goto _LL1AF;_LL1B2: if((_tmp2B4.UnknownL).tag != 2)goto _LL1AF;
_tmp2B6=(int)(_tmp2B4.UnknownL).val;_LL1B3: goto _LL1AF;_LL1AF:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp2B7=_tmp2AF;struct Cyc_CfFlowInfo_Place*_tmp2B8;int _tmp2B9;_LL1B5: if((_tmp2B7.PlaceL).tag
!= 1)goto _LL1B7;_tmp2B8=(struct Cyc_CfFlowInfo_Place*)(_tmp2B7.PlaceL).val;_LL1B6:
_tmp2B2=Cyc_CfFlowInfo_assign_place(_tmp172,_tmp1D9->loc,_tmp2B2,env->all_changed,
_tmp2B8,left_rval);goto _LL1B4;_LL1B7: if((_tmp2B7.UnknownL).tag != 2)goto _LL1B4;
_tmp2B9=(int)(_tmp2B7.UnknownL).val;_LL1B8: goto _LL1B4;_LL1B4:;}_tmp2B3=Cyc_CfFlowInfo_reln_kill_exp(
_tmp172->r,_tmp2B3,_tmp1D8);_tmp2B3=Cyc_CfFlowInfo_reln_kill_exp(_tmp172->r,
_tmp2B3,_tmp1D9);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2B2,_tmp2B3);goto _LL1AA;
_LL1AD:;_LL1AE: goto _LL1AA;_LL1AA:;}{struct _tuple13 _tmp801;return(_tmp801.f1=
outflow,((_tmp801.f2=_tmp172->unknown_all,_tmp801)));};};};}_LL13F: {struct Cyc_Absyn_New_e_struct*
_tmp1DA=(struct Cyc_Absyn_New_e_struct*)_tmp18C;if(_tmp1DA->tag != 17)goto _LL141;
else{_tmp1DB=_tmp1DA->f1;_tmp1DC=_tmp1DA->f2;}}_LL140: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp804;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp803;void*root=(void*)((_tmp803=
_region_malloc(_tmp172->r,sizeof(*_tmp803)),((_tmp803[0]=((_tmp804.tag=1,((
_tmp804.f1=_tmp1DC,((_tmp804.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp804)))))),_tmp803))));struct Cyc_CfFlowInfo_Place*_tmp805;
struct Cyc_CfFlowInfo_Place*place=(_tmp805=_region_malloc(_tmp172->r,sizeof(*
_tmp805)),((_tmp805->root=root,((_tmp805->fields=0,_tmp805)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp808;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp807;void*rval=(void*)((_tmp807=
_region_malloc(_tmp172->r,sizeof(*_tmp807)),((_tmp807[0]=((_tmp808.tag=5,((
_tmp808.f1=place,_tmp808)))),_tmp807))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;
if(_tmp1DB != 0){struct _RegionHandle*_tmp2BB=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp2BE;struct Cyc_List_List*_tmp2BF;struct Cyc_Absyn_Exp*_tmp809[2];struct
_tuple19 _tmp2BD=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2BB,env,inflow,((
_tmp809[1]=_tmp1DC,((_tmp809[0]=(struct Cyc_Absyn_Exp*)_tmp1DB,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2BB,
_tag_dyneither(_tmp809,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp2BE=_tmp2BD.f1;
_tmp2BF=_tmp2BD.f2;outflow=_tmp2BE;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2BF))->tl))->hd;}else{union
Cyc_CfFlowInfo_FlowInfo _tmp2C1;void*_tmp2C2;struct _tuple13 _tmp2C0=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp1DC);_tmp2C1=_tmp2C0.f1;_tmp2C2=_tmp2C0.f2;outflow=_tmp2C1;
place_val=_tmp2C2;}{union Cyc_CfFlowInfo_FlowInfo _tmp2C3=outflow;int _tmp2C4;
struct _tuple12 _tmp2C5;struct Cyc_Dict_Dict _tmp2C6;struct Cyc_List_List*_tmp2C7;
_LL1BA: if((_tmp2C3.BottomFL).tag != 1)goto _LL1BC;_tmp2C4=(int)(_tmp2C3.BottomFL).val;
_LL1BB: {struct _tuple13 _tmp80A;return(_tmp80A.f1=outflow,((_tmp80A.f2=rval,
_tmp80A)));}_LL1BC: if((_tmp2C3.ReachableFL).tag != 2)goto _LL1B9;_tmp2C5=(struct
_tuple12)(_tmp2C3.ReachableFL).val;_tmp2C6=_tmp2C5.f1;_tmp2C7=_tmp2C5.f2;_LL1BD: {
struct _tuple13 _tmp80B;return(_tmp80B.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2C6,root,place_val),
_tmp2C7),((_tmp80B.f2=rval,_tmp80B)));}_LL1B9:;};};}_LL141: {struct Cyc_Absyn_Address_e_struct*
_tmp1DD=(struct Cyc_Absyn_Address_e_struct*)_tmp18C;if(_tmp1DD->tag != 16)goto
_LL143;else{_tmp1DE=_tmp1DD->f1;}}_LL142: {union Cyc_CfFlowInfo_FlowInfo _tmp2D0;
union Cyc_CfFlowInfo_AbsLVal _tmp2D1;struct _tuple14 _tmp2CF=Cyc_NewControlFlow_anal_Lexp(
env,inflow,0,_tmp1DE);_tmp2D0=_tmp2CF.f1;_tmp2D1=_tmp2CF.f2;{union Cyc_CfFlowInfo_AbsLVal
_tmp2D2=_tmp2D1;int _tmp2D3;struct Cyc_CfFlowInfo_Place*_tmp2D4;_LL1BF: if((_tmp2D2.UnknownL).tag
!= 2)goto _LL1C1;_tmp2D3=(int)(_tmp2D2.UnknownL).val;_LL1C0: {struct _tuple13
_tmp80C;return(_tmp80C.f1=_tmp2D0,((_tmp80C.f2=_tmp172->notzeroall,_tmp80C)));}
_LL1C1: if((_tmp2D2.PlaceL).tag != 1)goto _LL1BE;_tmp2D4=(struct Cyc_CfFlowInfo_Place*)(
_tmp2D2.PlaceL).val;_LL1C2: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp812;
struct Cyc_CfFlowInfo_AddressOf_struct _tmp811;struct _tuple13 _tmp810;return(
_tmp810.f1=_tmp2D0,((_tmp810.f2=(void*)((_tmp812=_region_malloc(env->r,sizeof(*
_tmp812)),((_tmp812[0]=((_tmp811.tag=5,((_tmp811.f1=_tmp2D4,_tmp811)))),_tmp812)))),
_tmp810)));}_LL1BE:;};}_LL143: {struct Cyc_Absyn_Deref_e_struct*_tmp1DF=(struct
Cyc_Absyn_Deref_e_struct*)_tmp18C;if(_tmp1DF->tag != 21)goto _LL145;else{_tmp1E0=
_tmp1DF->f1;}}_LL144: {union Cyc_CfFlowInfo_FlowInfo _tmp2DA;void*_tmp2DB;struct
_tuple13 _tmp2D9=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1E0);_tmp2DA=
_tmp2D9.f1;_tmp2DB=_tmp2D9.f2;return Cyc_NewControlFlow_anal_derefR(env,inflow,
_tmp2DA,_tmp1E0,_tmp2DB);}_LL145: {struct Cyc_Absyn_AggrMember_e_struct*_tmp1E1=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp18C;if(_tmp1E1->tag != 22)goto _LL147;
else{_tmp1E2=_tmp1E1->f1;_tmp1E3=_tmp1E1->f2;}}_LL146: {union Cyc_CfFlowInfo_FlowInfo
_tmp2DD;void*_tmp2DE;struct _tuple13 _tmp2DC=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1E2);_tmp2DD=_tmp2DC.f1;_tmp2DE=_tmp2DC.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1E2->topt))->v)){struct _tuple13
_tmp813;return(_tmp813.f1=_tmp2DD,((_tmp813.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0,_tmp172->unknown_all),
_tmp813)));}{void*_tmp2E0=_tmp2DE;struct Cyc_CfFlowInfo_UnionRInfo _tmp2E2;int
_tmp2E3;int _tmp2E4;struct _dyneither_ptr _tmp2E5;_LL1C4: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2E1=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2E0;if(_tmp2E1->tag != 6)goto
_LL1C6;else{_tmp2E2=_tmp2E1->f1;_tmp2E3=_tmp2E2.is_union;_tmp2E4=_tmp2E2.fieldnum;
_tmp2E5=_tmp2E1->f2;}}_LL1C5: {int _tmp2E6=Cyc_CfFlowInfo_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E2->topt))->v,_tmp1E3);if((_tmp2E3  && 
_tmp2E4 != - 1) && _tmp2E4 != _tmp2E6){struct _tuple13 _tmp814;return(_tmp814.f1=
_tmp2DD,((_tmp814.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,1,_tmp172->unknown_none),_tmp814)));}{struct _tuple13
_tmp815;return(_tmp815.f1=_tmp2DD,((_tmp815.f2=*((void**)
_check_dyneither_subscript(_tmp2E5,sizeof(void*),_tmp2E6)),_tmp815)));};}_LL1C6:;
_LL1C7: {struct Cyc_Core_Impossible_struct _tmp822;const char*_tmp821;void*_tmp820[
1];struct Cyc_String_pa_struct _tmp81F;struct Cyc_Core_Impossible_struct*_tmp81E;(
int)_throw((void*)((_tmp81E=_cycalloc(sizeof(*_tmp81E)),((_tmp81E[0]=((_tmp822.tag=
Cyc_Core_Impossible,((_tmp822.f1=(struct _dyneither_ptr)((_tmp81F.tag=0,((_tmp81F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp820[
0]=& _tmp81F,Cyc_aprintf(((_tmp821="anal_Rexp: AggrMember: %s",_tag_dyneither(
_tmp821,sizeof(char),26))),_tag_dyneither(_tmp820,sizeof(void*),1)))))))),
_tmp822)))),_tmp81E)))));}_LL1C3:;};}_LL147: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1E4=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp18C;if(_tmp1E4->tag != 38)goto
_LL149;else{_tmp1E5=_tmp1E4->f1;_tmp1E6=_tmp1E4->f2;}}_LL148: {union Cyc_CfFlowInfo_FlowInfo
_tmp2EF;void*_tmp2F0;struct _tuple13 _tmp2EE=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1E5);_tmp2EF=_tmp2EE.f1;_tmp2F0=_tmp2EE.f2;if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1E5->topt))->v)){struct _tuple13
_tmp823;return(_tmp823.f1=_tmp2EF,((_tmp823.f2=_tmp172->unknown_all,_tmp823)));}{
void*_tmp2F2=_tmp2F0;struct Cyc_CfFlowInfo_UnionRInfo _tmp2F4;int _tmp2F5;int
_tmp2F6;struct _dyneither_ptr _tmp2F7;_LL1C9: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2F3=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2F2;if(_tmp2F3->tag != 6)goto
_LL1CB;else{_tmp2F4=_tmp2F3->f1;_tmp2F5=_tmp2F4.is_union;_tmp2F6=_tmp2F4.fieldnum;
_tmp2F7=_tmp2F3->f2;}}_LL1CA: {int _tmp2F8=Cyc_CfFlowInfo_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E5->topt))->v,_tmp1E6);if(_tmp2F5  && 
_tmp2F6 != - 1){if(_tmp2F6 != _tmp2F8){struct _tuple13 _tmp824;return(_tmp824.f1=
_tmp2EF,((_tmp824.f2=_tmp172->zero,_tmp824)));}else{struct _tuple13 _tmp825;return(
_tmp825.f1=_tmp2EF,((_tmp825.f2=_tmp172->notzeroall,_tmp825)));}}else{struct
_tuple13 _tmp826;return(_tmp826.f1=_tmp2EF,((_tmp826.f2=_tmp172->unknown_all,
_tmp826)));}}_LL1CB:;_LL1CC: {struct Cyc_Core_Impossible_struct _tmp833;const char*
_tmp832;void*_tmp831[1];struct Cyc_String_pa_struct _tmp830;struct Cyc_Core_Impossible_struct*
_tmp82F;(int)_throw((void*)((_tmp82F=_cycalloc(sizeof(*_tmp82F)),((_tmp82F[0]=((
_tmp833.tag=Cyc_Core_Impossible,((_tmp833.f1=(struct _dyneither_ptr)((_tmp830.tag=
0,((_tmp830.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp831[0]=& _tmp830,Cyc_aprintf(((_tmp832="anal_Rexp: TagCheck_e: %s",
_tag_dyneither(_tmp832,sizeof(char),26))),_tag_dyneither(_tmp831,sizeof(void*),1)))))))),
_tmp833)))),_tmp82F)))));}_LL1C8:;};}_LL149: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp1E7=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp18C;if(_tmp1E7->tag != 23)goto
_LL14B;else{_tmp1E8=_tmp1E7->f1;_tmp1E9=_tmp1E7->f2;}}_LL14A: {union Cyc_CfFlowInfo_FlowInfo
_tmp302;void*_tmp303;struct _tuple13 _tmp301=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp1E8);_tmp302=_tmp301.f1;_tmp303=_tmp301.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp305;void*_tmp306;struct _tuple13 _tmp304=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp302,_tmp1E8,_tmp303);_tmp305=_tmp304.f1;_tmp306=_tmp304.f2;{void*
_tmp307=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1E8->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp309;void*_tmp30A;_LL1CE: {struct Cyc_Absyn_PointerType_struct*
_tmp308=(struct Cyc_Absyn_PointerType_struct*)_tmp307;if(_tmp308->tag != 5)goto
_LL1D0;else{_tmp309=_tmp308->f1;_tmp30A=_tmp309.elt_typ;}}_LL1CF: if(Cyc_Absyn_is_nontagged_union_type(
_tmp30A)){struct _tuple13 _tmp834;return(_tmp834.f1=_tmp305,((_tmp834.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0,_tmp172->unknown_all),
_tmp834)));}{void*_tmp30C=_tmp306;struct Cyc_CfFlowInfo_UnionRInfo _tmp30E;int
_tmp30F;int _tmp310;struct _dyneither_ptr _tmp311;_LL1D3: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp30D=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp30C;if(_tmp30D->tag != 6)goto
_LL1D5;else{_tmp30E=_tmp30D->f1;_tmp30F=_tmp30E.is_union;_tmp310=_tmp30E.fieldnum;
_tmp311=_tmp30D->f2;}}_LL1D4: {int _tmp312=Cyc_CfFlowInfo_get_field_index(_tmp30A,
_tmp1E9);if((_tmp30F  && _tmp310 != - 1) && _tmp310 != _tmp312){struct _tuple13
_tmp835;return(_tmp835.f1=_tmp305,((_tmp835.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp172,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,1,_tmp172->unknown_none),
_tmp835)));}{struct _tuple13 _tmp836;return(_tmp836.f1=_tmp305,((_tmp836.f2=*((
void**)_check_dyneither_subscript(_tmp311,sizeof(void*),_tmp312)),_tmp836)));};}
_LL1D5:;_LL1D6: {struct Cyc_Core_Impossible_struct _tmp83C;const char*_tmp83B;
struct Cyc_Core_Impossible_struct*_tmp83A;(int)_throw((void*)((_tmp83A=_cycalloc(
sizeof(*_tmp83A)),((_tmp83A[0]=((_tmp83C.tag=Cyc_Core_Impossible,((_tmp83C.f1=((
_tmp83B="anal_Rexp: AggrArrow",_tag_dyneither(_tmp83B,sizeof(char),21))),_tmp83C)))),
_tmp83A)))));}_LL1D2:;};_LL1D0:;_LL1D1: {struct Cyc_Core_Impossible_struct _tmp842;
const char*_tmp841;struct Cyc_Core_Impossible_struct*_tmp840;(int)_throw((void*)((
_tmp840=_cycalloc(sizeof(*_tmp840)),((_tmp840[0]=((_tmp842.tag=Cyc_Core_Impossible,((
_tmp842.f1=((_tmp841="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp841,sizeof(
char),25))),_tmp842)))),_tmp840)))));}_LL1CD:;};};}_LL14B: {struct Cyc_Absyn_Conditional_e_struct*
_tmp1EA=(struct Cyc_Absyn_Conditional_e_struct*)_tmp18C;if(_tmp1EA->tag != 6)goto
_LL14D;else{_tmp1EB=_tmp1EA->f1;_tmp1EC=_tmp1EA->f2;_tmp1ED=_tmp1EA->f3;}}_LL14C: {
union Cyc_CfFlowInfo_FlowInfo _tmp31C;union Cyc_CfFlowInfo_FlowInfo _tmp31D;struct
_tuple15 _tmp31B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1EB);_tmp31C=_tmp31B.f1;
_tmp31D=_tmp31B.f2;{struct _tuple13 _tmp31E=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp31C,_tmp1EC);struct _tuple13 _tmp31F=Cyc_NewControlFlow_anal_Rexp(
env,copy_ctxt,_tmp31D,_tmp1ED);return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,
env->all_changed,_tmp31E,_tmp31F);};}_LL14D: {struct Cyc_Absyn_And_e_struct*
_tmp1EE=(struct Cyc_Absyn_And_e_struct*)_tmp18C;if(_tmp1EE->tag != 7)goto _LL14F;
else{_tmp1EF=_tmp1EE->f1;_tmp1F0=_tmp1EE->f2;}}_LL14E: {union Cyc_CfFlowInfo_FlowInfo
_tmp321;union Cyc_CfFlowInfo_FlowInfo _tmp322;struct _tuple15 _tmp320=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1EF);_tmp321=_tmp320.f1;_tmp322=_tmp320.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp324;void*_tmp325;struct _tuple13 _tmp323=Cyc_NewControlFlow_anal_Rexp(env,
copy_ctxt,_tmp321,_tmp1F0);_tmp324=_tmp323.f1;_tmp325=_tmp323.f2;{struct _tuple13
_tmp843;struct _tuple13 _tmp326=(_tmp843.f1=_tmp324,((_tmp843.f2=_tmp325,_tmp843)));
struct _tuple13 _tmp844;struct _tuple13 _tmp327=(_tmp844.f1=_tmp322,((_tmp844.f2=
_tmp172->zero,_tmp844)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,env->all_changed,
_tmp326,_tmp327);};};}_LL14F: {struct Cyc_Absyn_Or_e_struct*_tmp1F1=(struct Cyc_Absyn_Or_e_struct*)
_tmp18C;if(_tmp1F1->tag != 8)goto _LL151;else{_tmp1F2=_tmp1F1->f1;_tmp1F3=_tmp1F1->f2;}}
_LL150: {union Cyc_CfFlowInfo_FlowInfo _tmp32B;union Cyc_CfFlowInfo_FlowInfo _tmp32C;
struct _tuple15 _tmp32A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1F2);_tmp32B=
_tmp32A.f1;_tmp32C=_tmp32A.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp32E;void*_tmp32F;
struct _tuple13 _tmp32D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp32C,_tmp1F3);
_tmp32E=_tmp32D.f1;_tmp32F=_tmp32D.f2;{struct _tuple13 _tmp845;struct _tuple13
_tmp330=(_tmp845.f1=_tmp32E,((_tmp845.f2=_tmp32F,_tmp845)));struct _tuple13
_tmp846;struct _tuple13 _tmp331=(_tmp846.f1=_tmp32B,((_tmp846.f2=_tmp172->notzeroall,
_tmp846)));return Cyc_CfFlowInfo_join_flow_and_rval(_tmp172,env->all_changed,
_tmp330,_tmp331);};};}_LL151: {struct Cyc_Absyn_Subscript_e_struct*_tmp1F4=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp18C;if(_tmp1F4->tag != 24)goto _LL153;
else{_tmp1F5=_tmp1F4->f1;_tmp1F6=_tmp1F4->f2;}}_LL152: {struct _RegionHandle*
_tmp334=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp337;struct Cyc_List_List*_tmp338;
struct Cyc_Absyn_Exp*_tmp847[2];struct _tuple19 _tmp336=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp334,env,inflow,((_tmp847[1]=_tmp1F6,((_tmp847[0]=_tmp1F5,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp334,
_tag_dyneither(_tmp847,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp337=_tmp336.f1;
_tmp338=_tmp336.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp339=_tmp337;{union Cyc_CfFlowInfo_FlowInfo
_tmp33A=_tmp337;struct _tuple12 _tmp33B;struct Cyc_Dict_Dict _tmp33C;struct Cyc_List_List*
_tmp33D;_LL1D8: if((_tmp33A.ReachableFL).tag != 2)goto _LL1DA;_tmp33B=(struct
_tuple12)(_tmp33A.ReachableFL).val;_tmp33C=_tmp33B.f1;_tmp33D=_tmp33B.f2;_LL1D9:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp33C,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp338))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp84A;void*_tmp849;(_tmp849=0,Cyc_Tcutil_terr(_tmp1F6->loc,((_tmp84A="expression may not be initialized",
_tag_dyneither(_tmp84A,sizeof(char),34))),_tag_dyneither(_tmp849,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp340=Cyc_NewControlFlow_add_subscript_reln(_tmp172->r,
_tmp33D,_tmp1F5,_tmp1F6);if(_tmp33D != _tmp340)_tmp339=Cyc_CfFlowInfo_ReachableFL(
_tmp33C,_tmp340);goto _LL1D7;};_LL1DA:;_LL1DB: goto _LL1D7;_LL1D7:;}{void*_tmp341=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1F5->topt))->v);
struct Cyc_List_List*_tmp343;struct Cyc_Absyn_PtrInfo _tmp345;struct Cyc_Absyn_PtrAtts
_tmp346;union Cyc_Absyn_Constraint*_tmp347;_LL1DD: {struct Cyc_Absyn_TupleType_struct*
_tmp342=(struct Cyc_Absyn_TupleType_struct*)_tmp341;if(_tmp342->tag != 11)goto
_LL1DF;else{_tmp343=_tmp342->f1;}}_LL1DE: {void*_tmp348=(void*)((struct Cyc_List_List*)
_check_null(_tmp338))->hd;struct _dyneither_ptr _tmp34A;_LL1E4: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp349=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp348;if(_tmp349->tag != 6)goto
_LL1E6;else{_tmp34A=_tmp349->f2;}}_LL1E5: {unsigned int i=(Cyc_Evexp_eval_const_uint_exp(
_tmp1F6)).f1;struct _tuple13 _tmp84B;return(_tmp84B.f1=_tmp339,((_tmp84B.f2=*((
void**)_check_dyneither_subscript(_tmp34A,sizeof(void*),(int)i)),_tmp84B)));}
_LL1E6:;_LL1E7: {struct Cyc_Core_Impossible_struct _tmp851;const char*_tmp850;
struct Cyc_Core_Impossible_struct*_tmp84F;(int)_throw((void*)((_tmp84F=_cycalloc(
sizeof(*_tmp84F)),((_tmp84F[0]=((_tmp851.tag=Cyc_Core_Impossible,((_tmp851.f1=((
_tmp850="anal_Rexp: Subscript",_tag_dyneither(_tmp850,sizeof(char),21))),_tmp851)))),
_tmp84F)))));}_LL1E3:;}_LL1DF: {struct Cyc_Absyn_PointerType_struct*_tmp344=(
struct Cyc_Absyn_PointerType_struct*)_tmp341;if(_tmp344->tag != 5)goto _LL1E1;else{
_tmp345=_tmp344->f1;_tmp346=_tmp345.ptr_atts;_tmp347=_tmp346.bounds;}}_LL1E0: {
union Cyc_CfFlowInfo_FlowInfo _tmp350;void*_tmp351;struct _tuple13 _tmp34F=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp337,_tmp1F5,(void*)((struct Cyc_List_List*)_check_null(_tmp338))->hd);
_tmp350=_tmp34F.f1;_tmp351=_tmp34F.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp352=
_tmp350;int _tmp353;_LL1E9: if((_tmp352.BottomFL).tag != 1)goto _LL1EB;_tmp353=(int)(
_tmp352.BottomFL).val;_LL1EA: {struct _tuple13 _tmp852;return(_tmp852.f1=_tmp350,((
_tmp852.f2=_tmp351,_tmp852)));}_LL1EB:;_LL1EC: {struct _tuple13 _tmp853;return(
_tmp853.f1=_tmp339,((_tmp853.f2=_tmp351,_tmp853)));}_LL1E8:;};}_LL1E1:;_LL1E2: {
struct Cyc_Core_Impossible_struct _tmp859;const char*_tmp858;struct Cyc_Core_Impossible_struct*
_tmp857;(int)_throw((void*)((_tmp857=_cycalloc(sizeof(*_tmp857)),((_tmp857[0]=((
_tmp859.tag=Cyc_Core_Impossible,((_tmp859.f1=((_tmp858="anal_Rexp: Subscript -- bad type",
_tag_dyneither(_tmp858,sizeof(char),33))),_tmp859)))),_tmp857)))));}_LL1DC:;};};}
_LL153: {struct Cyc_Absyn_Datatype_e_struct*_tmp1F7=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp18C;if(_tmp1F7->tag != 31)goto _LL155;else{_tmp1F8=_tmp1F7->f1;_tmp1F9=_tmp1F7->f2;}}
_LL154: _tmp1FB=_tmp1F8;goto _LL156;_LL155: {struct Cyc_Absyn_Tuple_e_struct*
_tmp1FA=(struct Cyc_Absyn_Tuple_e_struct*)_tmp18C;if(_tmp1FA->tag != 25)goto _LL157;
else{_tmp1FB=_tmp1FA->f1;}}_LL156: {struct _RegionHandle*_tmp359=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp35B;struct Cyc_List_List*_tmp35C;struct _tuple19 _tmp35A=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp359,env,inflow,_tmp1FB,1,1);_tmp35B=_tmp35A.f1;_tmp35C=_tmp35A.f2;{
unsigned int _tmp85F;unsigned int _tmp85E;struct _dyneither_ptr _tmp85D;void**
_tmp85C;unsigned int _tmp85B;struct _dyneither_ptr aggrdict=(_tmp85B=(unsigned int)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1FB),((_tmp85C=(void**)
_region_malloc(env->r,_check_times(sizeof(void*),_tmp85B)),((_tmp85D=
_tag_dyneither(_tmp85C,sizeof(void*),_tmp85B),((((_tmp85E=_tmp85B,_tmp860(&
_tmp85F,& _tmp85E,& _tmp85C,& _tmp35C))),_tmp85D)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp86A;struct Cyc_CfFlowInfo_UnionRInfo _tmp869;struct Cyc_CfFlowInfo_Aggregate_struct
_tmp868;struct _tuple13 _tmp867;return(_tmp867.f1=_tmp35B,((_tmp867.f2=(void*)((
_tmp86A=_region_malloc(env->r,sizeof(*_tmp86A)),((_tmp86A[0]=((_tmp868.tag=6,((
_tmp868.f1=((_tmp869.is_union=0,((_tmp869.fieldnum=- 1,_tmp869)))),((_tmp868.f2=
aggrdict,_tmp868)))))),_tmp86A)))),_tmp867)));};}_LL157: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1FC=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp18C;if(_tmp1FC->tag != 30)goto
_LL159;else{_tmp1FD=_tmp1FC->f2;}}_LL158: {struct Cyc_List_List*fs;{void*_tmp366=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_List_List*_tmp368;_LL1EE: {struct Cyc_Absyn_AnonAggrType_struct*_tmp367=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp366;if(_tmp367->tag != 13)goto _LL1F0;
else{_tmp368=_tmp367->f2;}}_LL1EF: fs=_tmp368;goto _LL1ED;_LL1F0:;_LL1F1: {struct
Cyc_Core_Impossible_struct _tmp870;const char*_tmp86F;struct Cyc_Core_Impossible_struct*
_tmp86E;(int)_throw((void*)((_tmp86E=_cycalloc(sizeof(*_tmp86E)),((_tmp86E[0]=((
_tmp870.tag=Cyc_Core_Impossible,((_tmp870.f1=((_tmp86F="anal_Rexp:anon struct has bad type",
_tag_dyneither(_tmp86F,sizeof(char),35))),_tmp870)))),_tmp86E)))));}_LL1ED:;}
_tmp1FF=_tmp1FD;_tmp202=Cyc_Absyn_StructA;_tmp205=fs;goto _LL15A;}_LL159: {struct
Cyc_Absyn_Aggregate_e_struct*_tmp1FE=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp18C;if(_tmp1FE->tag != 29)goto _LL15B;else{_tmp1FF=_tmp1FE->f3;_tmp200=_tmp1FE->f4;
if(_tmp200 == 0)goto _LL15B;_tmp201=*_tmp200;_tmp202=_tmp201.kind;_tmp203=_tmp201.impl;
if(_tmp203 == 0)goto _LL15B;_tmp204=*_tmp203;_tmp205=_tmp204.fields;}}_LL15A: {
struct _RegionHandle*_tmp36C=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp36E;struct Cyc_List_List*
_tmp36F;struct _tuple19 _tmp36D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp36C,
env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp36C,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp1FF),1,1);_tmp36E=_tmp36D.f1;_tmp36F=_tmp36D.f2;{
struct _dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp172,
_tmp205,_tmp202 == Cyc_Absyn_UnionA,_tmp202 == Cyc_Absyn_UnionA?_tmp172->unknown_none:
_tmp172->unknown_all);int field_no=- 1;{int i=0;for(0;_tmp36F != 0;(((_tmp36F=
_tmp36F->tl,_tmp1FF=_tmp1FF->tl)),++ i)){struct Cyc_List_List*ds=(*((struct
_tuple21*)((struct Cyc_List_List*)_check_null(_tmp1FF))->hd)).f1;for(0;ds != 0;ds=
ds->tl){void*_tmp370=(void*)ds->hd;struct _dyneither_ptr*_tmp373;_LL1F3: {struct
Cyc_Absyn_ArrayElement_struct*_tmp371=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp370;if(_tmp371->tag != 0)goto _LL1F5;}_LL1F4: {struct Cyc_Core_Impossible_struct
_tmp876;const char*_tmp875;struct Cyc_Core_Impossible_struct*_tmp874;(int)_throw((
void*)((_tmp874=_cycalloc(sizeof(*_tmp874)),((_tmp874[0]=((_tmp876.tag=Cyc_Core_Impossible,((
_tmp876.f1=((_tmp875="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp875,sizeof(char),
22))),_tmp876)))),_tmp874)))));}_LL1F5: {struct Cyc_Absyn_FieldName_struct*
_tmp372=(struct Cyc_Absyn_FieldName_struct*)_tmp370;if(_tmp372->tag != 1)goto
_LL1F2;else{_tmp373=_tmp372->f1;}}_LL1F6: field_no=Cyc_CfFlowInfo_get_field_index_fs(
_tmp205,_tmp373);*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),
field_no))=(void*)_tmp36F->hd;if(_tmp202 == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*
_tmp377=(*((struct _tuple21*)_tmp1FF->hd)).f2;_tmp36E=Cyc_NewControlFlow_use_Rval(
env,_tmp377->loc,_tmp36E,(void*)_tmp36F->hd);}_LL1F2:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp880;struct Cyc_CfFlowInfo_UnionRInfo _tmp87F;struct Cyc_CfFlowInfo_Aggregate_struct
_tmp87E;struct _tuple13 _tmp87D;return(_tmp87D.f1=_tmp36E,((_tmp87D.f2=(void*)((
_tmp880=_region_malloc(env->r,sizeof(*_tmp880)),((_tmp880[0]=((_tmp87E.tag=6,((
_tmp87E.f1=((_tmp87F.is_union=_tmp202 == Cyc_Absyn_UnionA,((_tmp87F.fieldnum=
field_no,_tmp87F)))),((_tmp87E.f2=aggrdict,_tmp87E)))))),_tmp880)))),_tmp87D)));};};}
_LL15B: {struct Cyc_Absyn_Aggregate_e_struct*_tmp206=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp18C;if(_tmp206->tag != 29)goto _LL15D;}_LL15C: {struct Cyc_Core_Impossible_struct
_tmp886;const char*_tmp885;struct Cyc_Core_Impossible_struct*_tmp884;(int)_throw((
void*)((_tmp884=_cycalloc(sizeof(*_tmp884)),((_tmp884[0]=((_tmp886.tag=Cyc_Core_Impossible,((
_tmp886.f1=((_tmp885="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp885,
sizeof(char),31))),_tmp886)))),_tmp884)))));}_LL15D: {struct Cyc_Absyn_Array_e_struct*
_tmp207=(struct Cyc_Absyn_Array_e_struct*)_tmp18C;if(_tmp207->tag != 27)goto _LL15F;
else{_tmp208=_tmp207->f1;}}_LL15E: {struct _RegionHandle*_tmp37F=env->r;struct Cyc_List_List*
_tmp380=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple21*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp37F,(struct Cyc_Absyn_Exp*(*)(
struct _tuple21*))Cyc_Core_snd,_tmp208);union Cyc_CfFlowInfo_FlowInfo _tmp382;
struct Cyc_List_List*_tmp383;struct _tuple19 _tmp381=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp37F,env,inflow,_tmp380,1,1);_tmp382=_tmp381.f1;_tmp383=_tmp381.f2;for(0;
_tmp383 != 0;(_tmp383=_tmp383->tl,_tmp380=_tmp380->tl)){_tmp382=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp380))->hd)->loc,
_tmp382,(void*)_tmp383->hd);}{struct _tuple13 _tmp887;return(_tmp887.f1=_tmp382,((
_tmp887.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp172,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0,_tmp172->unknown_all),_tmp887)));};}_LL15F: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp209=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp18C;if(_tmp209->tag != 28)goto _LL161;else{_tmp20A=_tmp209->f1;_tmp20B=_tmp209->f2;
_tmp20C=_tmp209->f3;_tmp20D=_tmp209->f4;}}_LL160: {union Cyc_CfFlowInfo_FlowInfo
_tmp386;void*_tmp387;struct _tuple13 _tmp385=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp20B);_tmp386=_tmp385.f1;_tmp387=_tmp385.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp388=_tmp386;int _tmp389;struct _tuple12 _tmp38A;struct Cyc_Dict_Dict _tmp38B;
struct Cyc_List_List*_tmp38C;_LL1F8: if((_tmp388.BottomFL).tag != 1)goto _LL1FA;
_tmp389=(int)(_tmp388.BottomFL).val;_LL1F9: {struct _tuple13 _tmp888;return(
_tmp888.f1=_tmp386,((_tmp888.f2=_tmp172->unknown_all,_tmp888)));}_LL1FA: if((
_tmp388.ReachableFL).tag != 2)goto _LL1F7;_tmp38A=(struct _tuple12)(_tmp388.ReachableFL).val;
_tmp38B=_tmp38A.f1;_tmp38C=_tmp38A.f2;_LL1FB: if(Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp38B,_tmp387)== Cyc_CfFlowInfo_NoneIL){const char*_tmp88B;void*_tmp88A;(
_tmp88A=0,Cyc_Tcutil_terr(_tmp20B->loc,((_tmp88B="expression may not be initialized",
_tag_dyneither(_tmp88B,sizeof(char),34))),_tag_dyneither(_tmp88A,sizeof(void*),0)));}{
struct Cyc_List_List*new_relns=_tmp38C;comp_loop: {void*_tmp390=_tmp20B->r;struct
Cyc_Absyn_Exp*_tmp392;void*_tmp394;struct Cyc_Absyn_Vardecl*_tmp396;void*_tmp398;
struct Cyc_Absyn_Vardecl*_tmp39A;void*_tmp39C;struct Cyc_Absyn_Vardecl*_tmp39E;
void*_tmp3A0;struct Cyc_Absyn_Vardecl*_tmp3A2;union Cyc_Absyn_Cnst _tmp3A4;struct
_tuple6 _tmp3A5;int _tmp3A6;enum Cyc_Absyn_Primop _tmp3A8;struct Cyc_List_List*
_tmp3A9;struct Cyc_List_List _tmp3AA;struct Cyc_Absyn_Exp*_tmp3AB;_LL1FD: {struct
Cyc_Absyn_Cast_e_struct*_tmp391=(struct Cyc_Absyn_Cast_e_struct*)_tmp390;if(
_tmp391->tag != 15)goto _LL1FF;else{_tmp392=_tmp391->f2;}}_LL1FE: _tmp20B=_tmp392;
goto comp_loop;_LL1FF:{struct Cyc_Absyn_Var_e_struct*_tmp393=(struct Cyc_Absyn_Var_e_struct*)
_tmp390;if(_tmp393->tag != 1)goto _LL201;else{_tmp394=(void*)_tmp393->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp395=(struct Cyc_Absyn_Global_b_struct*)_tmp394;if(
_tmp395->tag != 1)goto _LL201;else{_tmp396=_tmp395->f1;}};}}if(!(!_tmp396->escapes))
goto _LL201;_LL200: _tmp39A=_tmp396;goto _LL202;_LL201:{struct Cyc_Absyn_Var_e_struct*
_tmp397=(struct Cyc_Absyn_Var_e_struct*)_tmp390;if(_tmp397->tag != 1)goto _LL203;
else{_tmp398=(void*)_tmp397->f2;{struct Cyc_Absyn_Local_b_struct*_tmp399=(struct
Cyc_Absyn_Local_b_struct*)_tmp398;if(_tmp399->tag != 4)goto _LL203;else{_tmp39A=
_tmp399->f1;}};}}if(!(!_tmp39A->escapes))goto _LL203;_LL202: _tmp39E=_tmp39A;goto
_LL204;_LL203:{struct Cyc_Absyn_Var_e_struct*_tmp39B=(struct Cyc_Absyn_Var_e_struct*)
_tmp390;if(_tmp39B->tag != 1)goto _LL205;else{_tmp39C=(void*)_tmp39B->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp39D=(struct Cyc_Absyn_Pat_b_struct*)_tmp39C;if(_tmp39D->tag
!= 5)goto _LL205;else{_tmp39E=_tmp39D->f1;}};}}if(!(!_tmp39E->escapes))goto _LL205;
_LL204: _tmp3A2=_tmp39E;goto _LL206;_LL205:{struct Cyc_Absyn_Var_e_struct*_tmp39F=(
struct Cyc_Absyn_Var_e_struct*)_tmp390;if(_tmp39F->tag != 1)goto _LL207;else{
_tmp3A0=(void*)_tmp39F->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3A1=(struct Cyc_Absyn_Param_b_struct*)
_tmp3A0;if(_tmp3A1->tag != 3)goto _LL207;else{_tmp3A2=_tmp3A1->f1;}};}}if(!(!
_tmp3A2->escapes))goto _LL207;_LL206:{struct Cyc_CfFlowInfo_Reln*_tmp88E;struct Cyc_List_List*
_tmp88D;new_relns=((_tmp88D=_region_malloc(env->r,sizeof(*_tmp88D)),((_tmp88D->hd=((
_tmp88E=_region_malloc(env->r,sizeof(*_tmp88E)),((_tmp88E->vd=_tmp20A,((_tmp88E->rop=
Cyc_CfFlowInfo_LessVar(_tmp3A2,_tmp3A2->type),_tmp88E)))))),((_tmp88D->tl=
_tmp38C,_tmp88D))))));}goto _LL1FC;_LL207: {struct Cyc_Absyn_Const_e_struct*
_tmp3A3=(struct Cyc_Absyn_Const_e_struct*)_tmp390;if(_tmp3A3->tag != 0)goto _LL209;
else{_tmp3A4=_tmp3A3->f1;if((_tmp3A4.Int_c).tag != 4)goto _LL209;_tmp3A5=(struct
_tuple6)(_tmp3A4.Int_c).val;_tmp3A6=_tmp3A5.f2;}}_LL208:{struct Cyc_CfFlowInfo_Reln*
_tmp891;struct Cyc_List_List*_tmp890;new_relns=((_tmp890=_region_malloc(env->r,
sizeof(*_tmp890)),((_tmp890->hd=((_tmp891=_region_malloc(env->r,sizeof(*_tmp891)),((
_tmp891->vd=_tmp20A,((_tmp891->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp3A6),_tmp891)))))),((_tmp890->tl=_tmp38C,_tmp890))))));}goto _LL1FC;_LL209: {
struct Cyc_Absyn_Primop_e_struct*_tmp3A7=(struct Cyc_Absyn_Primop_e_struct*)
_tmp390;if(_tmp3A7->tag != 3)goto _LL20B;else{_tmp3A8=_tmp3A7->f1;_tmp3A9=_tmp3A7->f2;
if(_tmp3A9 == 0)goto _LL20B;_tmp3AA=*_tmp3A9;_tmp3AB=(struct Cyc_Absyn_Exp*)_tmp3AA.hd;}}
_LL20A:{void*_tmp3B0=_tmp3AB->r;void*_tmp3B2;struct Cyc_Absyn_Vardecl*_tmp3B4;
void*_tmp3B6;struct Cyc_Absyn_Vardecl*_tmp3B8;void*_tmp3BA;struct Cyc_Absyn_Vardecl*
_tmp3BC;void*_tmp3BE;struct Cyc_Absyn_Vardecl*_tmp3C0;_LL20E:{struct Cyc_Absyn_Var_e_struct*
_tmp3B1=(struct Cyc_Absyn_Var_e_struct*)_tmp3B0;if(_tmp3B1->tag != 1)goto _LL210;
else{_tmp3B2=(void*)_tmp3B1->f2;{struct Cyc_Absyn_Global_b_struct*_tmp3B3=(struct
Cyc_Absyn_Global_b_struct*)_tmp3B2;if(_tmp3B3->tag != 1)goto _LL210;else{_tmp3B4=
_tmp3B3->f1;}};}}if(!(!_tmp3B4->escapes))goto _LL210;_LL20F: _tmp3B8=_tmp3B4;goto
_LL211;_LL210:{struct Cyc_Absyn_Var_e_struct*_tmp3B5=(struct Cyc_Absyn_Var_e_struct*)
_tmp3B0;if(_tmp3B5->tag != 1)goto _LL212;else{_tmp3B6=(void*)_tmp3B5->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp3B7=(struct Cyc_Absyn_Local_b_struct*)_tmp3B6;if(
_tmp3B7->tag != 4)goto _LL212;else{_tmp3B8=_tmp3B7->f1;}};}}if(!(!_tmp3B8->escapes))
goto _LL212;_LL211: _tmp3BC=_tmp3B8;goto _LL213;_LL212:{struct Cyc_Absyn_Var_e_struct*
_tmp3B9=(struct Cyc_Absyn_Var_e_struct*)_tmp3B0;if(_tmp3B9->tag != 1)goto _LL214;
else{_tmp3BA=(void*)_tmp3B9->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp3BB=(struct Cyc_Absyn_Pat_b_struct*)
_tmp3BA;if(_tmp3BB->tag != 5)goto _LL214;else{_tmp3BC=_tmp3BB->f1;}};}}if(!(!
_tmp3BC->escapes))goto _LL214;_LL213: _tmp3C0=_tmp3BC;goto _LL215;_LL214:{struct Cyc_Absyn_Var_e_struct*
_tmp3BD=(struct Cyc_Absyn_Var_e_struct*)_tmp3B0;if(_tmp3BD->tag != 1)goto _LL216;
else{_tmp3BE=(void*)_tmp3BD->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3BF=(struct
Cyc_Absyn_Param_b_struct*)_tmp3BE;if(_tmp3BF->tag != 3)goto _LL216;else{_tmp3C0=
_tmp3BF->f1;}};}}if(!(!_tmp3C0->escapes))goto _LL216;_LL215:{struct Cyc_CfFlowInfo_Reln*
_tmp894;struct Cyc_List_List*_tmp893;new_relns=((_tmp893=_region_malloc(env->r,
sizeof(*_tmp893)),((_tmp893->hd=((_tmp894=_region_malloc(env->r,sizeof(*_tmp894)),((
_tmp894->vd=_tmp20A,((_tmp894->rop=Cyc_CfFlowInfo_LessNumelts(_tmp3C0),_tmp894)))))),((
_tmp893->tl=_tmp38C,_tmp893))))));}goto _LL20D;_LL216:;_LL217: goto _LL20D;_LL20D:;}
goto _LL1FC;_LL20B:;_LL20C: goto _LL1FC;_LL1FC:;}if(_tmp38C != new_relns)_tmp386=Cyc_CfFlowInfo_ReachableFL(
_tmp38B,new_relns);{void*_tmp3C3=_tmp387;_LL219: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3C4=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3C3;if(_tmp3C4->tag != 0)goto
_LL21B;}_LL21A: {struct _tuple13 _tmp895;return(_tmp895.f1=_tmp386,((_tmp895.f2=
_tmp172->unknown_all,_tmp895)));}_LL21B: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3C5=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3C3;if(_tmp3C5->tag != 2)
goto _LL21D;}_LL21C: goto _LL21E;_LL21D: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3C6=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3C3;if(_tmp3C6->tag != 1)
goto _LL21F;}_LL21E: goto _LL220;_LL21F: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3C7=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3C3;if(_tmp3C7->tag != 5)goto
_LL221;}_LL220: {struct Cyc_List_List _tmp896;struct Cyc_List_List _tmp3C9=(_tmp896.hd=
_tmp20A,((_tmp896.tl=0,_tmp896)));_tmp386=Cyc_NewControlFlow_add_vars(_tmp172,
_tmp386,(struct Cyc_List_List*)& _tmp3C9,_tmp172->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3CB;void*_tmp3CC;struct _tuple13 _tmp3CA=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp386,_tmp20C);_tmp3CB=_tmp3CA.f1;_tmp3CC=_tmp3CA.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3CD=_tmp3CB;int _tmp3CE;struct _tuple12 _tmp3CF;struct Cyc_Dict_Dict _tmp3D0;
_LL224: if((_tmp3CD.BottomFL).tag != 1)goto _LL226;_tmp3CE=(int)(_tmp3CD.BottomFL).val;
_LL225: {struct _tuple13 _tmp897;return(_tmp897.f1=_tmp3CB,((_tmp897.f2=_tmp172->unknown_all,
_tmp897)));}_LL226: if((_tmp3CD.ReachableFL).tag != 2)goto _LL223;_tmp3CF=(struct
_tuple12)(_tmp3CD.ReachableFL).val;_tmp3D0=_tmp3CF.f1;_LL227: if(Cyc_CfFlowInfo_initlevel(
_tmp172,_tmp3D0,_tmp3CC)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp89A;void*_tmp899;(
_tmp899=0,Cyc_Tcutil_terr(_tmp20B->loc,((_tmp89A="expression may not be initialized",
_tag_dyneither(_tmp89A,sizeof(char),34))),_tag_dyneither(_tmp899,sizeof(void*),0)));}{
struct _tuple13 _tmp89B;return(_tmp89B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp89B.f2=
_tmp172->unknown_all,_tmp89B)));};}_LL223:;}_tmp386=_tmp3CB;goto _LL222;};}_LL221:;
_LL222: while(1){struct Cyc_List_List _tmp89C;struct Cyc_List_List _tmp3D6=(_tmp89C.hd=
_tmp20A,((_tmp89C.tl=0,_tmp89C)));_tmp386=Cyc_NewControlFlow_add_vars(_tmp172,
_tmp386,(struct Cyc_List_List*)& _tmp3D6,_tmp172->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3D8;void*_tmp3D9;struct _tuple13 _tmp3D7=Cyc_NewControlFlow_anal_Rexp(env,1,
_tmp386,_tmp20C);_tmp3D8=_tmp3D7.f1;_tmp3D9=_tmp3D7.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp3DA=_tmp3D8;int _tmp3DB;struct _tuple12 _tmp3DC;struct Cyc_Dict_Dict _tmp3DD;
_LL229: if((_tmp3DA.BottomFL).tag != 1)goto _LL22B;_tmp3DB=(int)(_tmp3DA.BottomFL).val;
_LL22A: goto _LL228;_LL22B: if((_tmp3DA.ReachableFL).tag != 2)goto _LL228;_tmp3DC=(
struct _tuple12)(_tmp3DA.ReachableFL).val;_tmp3DD=_tmp3DC.f1;_LL22C: if(Cyc_CfFlowInfo_initlevel(
_tmp172,_tmp3DD,_tmp3D9)!= Cyc_CfFlowInfo_AllIL){{const char*_tmp89F;void*_tmp89E;(
_tmp89E=0,Cyc_Tcutil_terr(_tmp20B->loc,((_tmp89F="expression may not be initialized",
_tag_dyneither(_tmp89F,sizeof(char),34))),_tag_dyneither(_tmp89E,sizeof(void*),0)));}{
struct _tuple13 _tmp8A0;return(_tmp8A0.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8A0.f2=
_tmp172->unknown_all,_tmp8A0)));};}_LL228:;}{union Cyc_CfFlowInfo_FlowInfo _tmp3E1=
Cyc_CfFlowInfo_join_flow(_tmp172,env->all_changed,_tmp386,_tmp3D8);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3E1,_tmp386))break;_tmp386=_tmp3E1;};};}{struct _tuple13 _tmp8A1;return(
_tmp8A1.f1=_tmp386,((_tmp8A1.f2=_tmp172->unknown_all,_tmp8A1)));};_LL218:;};};
_LL1F7:;};}_LL161: {struct Cyc_Absyn_StmtExp_e_struct*_tmp20E=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp18C;if(_tmp20E->tag != 37)goto _LL163;else{_tmp20F=_tmp20E->f1;}}_LL162: return
Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp20F);_LL163: {struct
Cyc_Absyn_Var_e_struct*_tmp210=(struct Cyc_Absyn_Var_e_struct*)_tmp18C;if(_tmp210->tag
!= 1)goto _LL165;else{_tmp211=(void*)_tmp210->f2;{struct Cyc_Absyn_Unresolved_b_struct*
_tmp212=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp211;if(_tmp212->tag != 0)goto
_LL165;};}}_LL164: goto _LL166;_LL165: {struct Cyc_Absyn_UnknownId_e_struct*_tmp213=(
struct Cyc_Absyn_UnknownId_e_struct*)_tmp18C;if(_tmp213->tag != 2)goto _LL167;}
_LL166: goto _LL168;_LL167: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp214=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp18C;if(_tmp214->tag != 10)goto _LL169;}_LL168:
goto _LL16A;_LL169: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp215=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp18C;if(_tmp215->tag != 36)goto _LL16B;}_LL16A: goto _LL16C;_LL16B: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp216=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp18C;if(_tmp216->tag != 26)goto
_LL16D;}_LL16C: goto _LL16E;_LL16D: {struct Cyc_Absyn_Valueof_e_struct*_tmp217=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp18C;if(_tmp217->tag != 39)goto _LL16F;}
_LL16E: goto _LL170;_LL16F: {struct Cyc_Absyn_Asm_e_struct*_tmp218=(struct Cyc_Absyn_Asm_e_struct*)
_tmp18C;if(_tmp218->tag != 40)goto _LL106;}_LL170: {struct Cyc_Core_Impossible_struct
_tmp8A7;const char*_tmp8A6;struct Cyc_Core_Impossible_struct*_tmp8A5;(int)_throw((
void*)((_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5[0]=((_tmp8A7.tag=Cyc_Core_Impossible,((
_tmp8A7.f1=((_tmp8A6="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp8A6,
sizeof(char),31))),_tmp8A7)))),_tmp8A5)))));}_LL106:;};}static struct _tuple14 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3E7=env->fenv;void*_tmp3E8=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp3EA;void*_tmp3EB;struct Cyc_Absyn_PtrAtts _tmp3EC;union Cyc_Absyn_Constraint*
_tmp3ED;union Cyc_Absyn_Constraint*_tmp3EE;_LL22E: {struct Cyc_Absyn_PointerType_struct*
_tmp3E9=(struct Cyc_Absyn_PointerType_struct*)_tmp3E8;if(_tmp3E9->tag != 5)goto
_LL230;else{_tmp3EA=_tmp3E9->f1;_tmp3EB=_tmp3EA.elt_typ;_tmp3EC=_tmp3EA.ptr_atts;
_tmp3ED=_tmp3EC.bounds;_tmp3EE=_tmp3EC.zero_term;}}_LL22F: {union Cyc_CfFlowInfo_FlowInfo
_tmp3EF=f;int _tmp3F0;struct _tuple12 _tmp3F1;struct Cyc_Dict_Dict _tmp3F2;struct Cyc_List_List*
_tmp3F3;_LL233: if((_tmp3EF.BottomFL).tag != 1)goto _LL235;_tmp3F0=(int)(_tmp3EF.BottomFL).val;
_LL234: {struct _tuple14 _tmp8A8;return(_tmp8A8.f1=f,((_tmp8A8.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8A8)));}_LL235: if((_tmp3EF.ReachableFL).tag != 2)goto _LL232;_tmp3F1=(struct
_tuple12)(_tmp3EF.ReachableFL).val;_tmp3F2=_tmp3F1.f1;_tmp3F3=_tmp3F1.f2;_LL236:
if(Cyc_Tcutil_is_bound_one(_tmp3ED)){void*_tmp3F5=r;struct Cyc_CfFlowInfo_Place*
_tmp3F9;struct Cyc_CfFlowInfo_Place _tmp3FA;void*_tmp3FB;struct Cyc_List_List*
_tmp3FC;enum Cyc_CfFlowInfo_InitLevel _tmp3FF;_LL238: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3F6=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3F5;if(_tmp3F6->tag != 1)
goto _LL23A;}_LL239: goto _LL23B;_LL23A: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3F7=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3F5;if(_tmp3F7->tag != 2)
goto _LL23C;}_LL23B:{struct Cyc_CfFlowInfo_NotZero_struct _tmp8AB;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp8AA;e->annot=(void*)((_tmp8AA=_cycalloc(sizeof(*_tmp8AA)),((_tmp8AA[0]=((
_tmp8AB.tag=Cyc_CfFlowInfo_NotZero,((_tmp8AB.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3F3),_tmp8AB)))),_tmp8AA))));}goto _LL237;_LL23C: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3F8=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3F5;if(_tmp3F8->tag != 5)goto
_LL23E;else{_tmp3F9=_tmp3F8->f1;_tmp3FA=*_tmp3F9;_tmp3FB=_tmp3FA.root;_tmp3FC=
_tmp3FA.fields;}}_LL23D:{struct Cyc_CfFlowInfo_NotZero_struct _tmp8AE;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp8AD;e->annot=(void*)((_tmp8AD=_cycalloc(sizeof(*_tmp8AD)),((_tmp8AD[0]=((
_tmp8AE.tag=Cyc_CfFlowInfo_NotZero,((_tmp8AE.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3F3),_tmp8AE)))),_tmp8AD))));}{struct Cyc_CfFlowInfo_Place*_tmp8B1;struct
_tuple14 _tmp8B0;return(_tmp8B0.f1=f,((_tmp8B0.f2=Cyc_CfFlowInfo_PlaceL(((_tmp8B1=
_region_malloc(_tmp3E7->r,sizeof(*_tmp8B1)),((_tmp8B1->root=_tmp3FB,((_tmp8B1->fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(_tmp3E7->r,_tmp3FC,flds),_tmp8B1))))))),_tmp8B0)));};_LL23E: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp3FD=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp3F5;if(_tmp3FD->tag != 0)goto _LL240;}_LL23F: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple14 _tmp8B2;return(_tmp8B2.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8B2.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp8B2)));};_LL240: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3FE=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3F5;if(_tmp3FE->tag != 3)goto
_LL242;else{_tmp3FF=_tmp3FE->f1;}}_LL241: f=Cyc_NewControlFlow_notzero(env,inflow,
f,e,_tmp3FF);goto _LL243;_LL242:;_LL243: {struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp8B5;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp8B4;e->annot=(void*)((_tmp8B4=
_cycalloc(sizeof(*_tmp8B4)),((_tmp8B4[0]=((_tmp8B5.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp8B5.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3F3),_tmp8B5)))),
_tmp8B4))));}_LL237:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp8B8;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp8B7;e->annot=(void*)((_tmp8B7=_cycalloc(sizeof(*_tmp8B7)),((_tmp8B7[0]=((
_tmp8B8.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8B8.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3F3),_tmp8B8)))),_tmp8B7))));}if(Cyc_CfFlowInfo_initlevel(_tmp3E7,_tmp3F2,r)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp8BB;void*_tmp8BA;(_tmp8BA=0,Cyc_Tcutil_terr(
e->loc,((_tmp8BB="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp8BB,sizeof(char),46))),_tag_dyneither(_tmp8BA,sizeof(void*),0)));}{struct
_tuple14 _tmp8BC;return(_tmp8BC.f1=f,((_tmp8BC.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8BC)));};_LL232:;}_LL230:;_LL231: {struct Cyc_Core_Impossible_struct _tmp8C2;
const char*_tmp8C1;struct Cyc_Core_Impossible_struct*_tmp8C0;(int)_throw((void*)((
_tmp8C0=_cycalloc(sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8C2.tag=Cyc_Core_Impossible,((
_tmp8C2.f1=((_tmp8C1="left deref of non-pointer-type",_tag_dyneither(_tmp8C1,
sizeof(char),31))),_tmp8C2)))),_tmp8C0)))));}_LL22D:;}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int
expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){struct Cyc_Dict_Dict
d;struct Cyc_CfFlowInfo_FlowEnv*_tmp411=env->fenv;{union Cyc_CfFlowInfo_FlowInfo
_tmp412=inflow;int _tmp413;struct _tuple12 _tmp414;struct Cyc_Dict_Dict _tmp415;
struct Cyc_List_List*_tmp416;_LL245: if((_tmp412.BottomFL).tag != 1)goto _LL247;
_tmp413=(int)(_tmp412.BottomFL).val;_LL246: {struct _tuple14 _tmp8C3;return(
_tmp8C3.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8C3.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8C3)));}_LL247: if((_tmp412.ReachableFL).tag != 2)goto _LL244;_tmp414=(struct
_tuple12)(_tmp412.ReachableFL).val;_tmp415=_tmp414.f1;_tmp416=_tmp414.f2;_LL248:
d=_tmp415;_LL244:;}{void*_tmp418=e->r;struct Cyc_Absyn_Exp*_tmp41A;struct Cyc_Absyn_Exp*
_tmp41C;struct Cyc_Absyn_Exp*_tmp41E;void*_tmp420;struct Cyc_Absyn_Vardecl*_tmp422;
void*_tmp424;struct Cyc_Absyn_Vardecl*_tmp426;void*_tmp428;struct Cyc_Absyn_Vardecl*
_tmp42A;void*_tmp42C;struct Cyc_Absyn_Vardecl*_tmp42E;struct Cyc_Absyn_Exp*_tmp430;
struct _dyneither_ptr*_tmp431;struct Cyc_Absyn_Exp*_tmp433;struct Cyc_Absyn_Exp*
_tmp435;struct Cyc_Absyn_Exp*_tmp436;struct Cyc_Absyn_Exp*_tmp438;struct
_dyneither_ptr*_tmp439;_LL24A: {struct Cyc_Absyn_Cast_e_struct*_tmp419=(struct Cyc_Absyn_Cast_e_struct*)
_tmp418;if(_tmp419->tag != 15)goto _LL24C;else{_tmp41A=_tmp419->f2;}}_LL24B:
_tmp41C=_tmp41A;goto _LL24D;_LL24C: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp41B=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp418;if(_tmp41B->tag != 13)
goto _LL24E;else{_tmp41C=_tmp41B->f1;}}_LL24D: _tmp41E=_tmp41C;goto _LL24F;_LL24E: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp41D=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp418;if(_tmp41D->tag != 14)goto _LL250;else{_tmp41E=_tmp41D->f1;}}_LL24F: return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp41E,flds);_LL250: {
struct Cyc_Absyn_Var_e_struct*_tmp41F=(struct Cyc_Absyn_Var_e_struct*)_tmp418;if(
_tmp41F->tag != 1)goto _LL252;else{_tmp420=(void*)_tmp41F->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp421=(struct Cyc_Absyn_Param_b_struct*)_tmp420;if(_tmp421->tag != 3)goto _LL252;
else{_tmp422=_tmp421->f1;}};}}_LL251: _tmp426=_tmp422;goto _LL253;_LL252: {struct
Cyc_Absyn_Var_e_struct*_tmp423=(struct Cyc_Absyn_Var_e_struct*)_tmp418;if(_tmp423->tag
!= 1)goto _LL254;else{_tmp424=(void*)_tmp423->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp425=(struct Cyc_Absyn_Local_b_struct*)_tmp424;if(_tmp425->tag != 4)goto _LL254;
else{_tmp426=_tmp425->f1;}};}}_LL253: _tmp42A=_tmp426;goto _LL255;_LL254: {struct
Cyc_Absyn_Var_e_struct*_tmp427=(struct Cyc_Absyn_Var_e_struct*)_tmp418;if(_tmp427->tag
!= 1)goto _LL256;else{_tmp428=(void*)_tmp427->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp429=(struct Cyc_Absyn_Pat_b_struct*)_tmp428;if(_tmp429->tag != 5)goto _LL256;
else{_tmp42A=_tmp429->f1;}};}}_LL255: {struct Cyc_CfFlowInfo_Place*_tmp8CD;struct
Cyc_CfFlowInfo_VarRoot_struct _tmp8CC;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp8CB;
struct _tuple14 _tmp8CA;return(_tmp8CA.f1=inflow,((_tmp8CA.f2=Cyc_CfFlowInfo_PlaceL(((
_tmp8CD=_region_malloc(env->r,sizeof(*_tmp8CD)),((_tmp8CD->root=(void*)((_tmp8CB=
_region_malloc(env->r,sizeof(*_tmp8CB)),((_tmp8CB[0]=((_tmp8CC.tag=0,((_tmp8CC.f1=
_tmp42A,_tmp8CC)))),_tmp8CB)))),((_tmp8CD->fields=flds,_tmp8CD))))))),_tmp8CA)));}
_LL256: {struct Cyc_Absyn_Var_e_struct*_tmp42B=(struct Cyc_Absyn_Var_e_struct*)
_tmp418;if(_tmp42B->tag != 1)goto _LL258;else{_tmp42C=(void*)_tmp42B->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp42D=(struct Cyc_Absyn_Global_b_struct*)_tmp42C;if(
_tmp42D->tag != 1)goto _LL258;else{_tmp42E=_tmp42D->f1;}};}}_LL257: {struct
_tuple14 _tmp8CE;return(_tmp8CE.f1=inflow,((_tmp8CE.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp8CE)));}_LL258: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp42F=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp418;if(_tmp42F->tag != 23)goto _LL25A;else{_tmp430=_tmp42F->f1;_tmp431=_tmp42F->f2;}}
_LL259:{void*_tmp43F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp430->topt))->v);struct Cyc_Absyn_PtrInfo _tmp441;void*_tmp442;
_LL263: {struct Cyc_Absyn_PointerType_struct*_tmp440=(struct Cyc_Absyn_PointerType_struct*)
_tmp43F;if(_tmp440->tag != 5)goto _LL265;else{_tmp441=_tmp440->f1;_tmp442=_tmp441.elt_typ;}}
_LL264: if(!Cyc_Absyn_is_nontagged_union_type(_tmp442)){struct Cyc_List_List*
_tmp8CF;flds=((_tmp8CF=_region_malloc(env->r,sizeof(*_tmp8CF)),((_tmp8CF->hd=(
void*)Cyc_CfFlowInfo_get_field_index(_tmp442,_tmp431),((_tmp8CF->tl=flds,_tmp8CF))))));}
goto _LL262;_LL265:;_LL266: {struct Cyc_Core_Impossible_struct _tmp8D5;const char*
_tmp8D4;struct Cyc_Core_Impossible_struct*_tmp8D3;(int)_throw((void*)((_tmp8D3=
_cycalloc(sizeof(*_tmp8D3)),((_tmp8D3[0]=((_tmp8D5.tag=Cyc_Core_Impossible,((
_tmp8D5.f1=((_tmp8D4="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp8D4,sizeof(
char),25))),_tmp8D5)))),_tmp8D3)))));}_LL262:;}_tmp433=_tmp430;goto _LL25B;_LL25A: {
struct Cyc_Absyn_Deref_e_struct*_tmp432=(struct Cyc_Absyn_Deref_e_struct*)_tmp418;
if(_tmp432->tag != 21)goto _LL25C;else{_tmp433=_tmp432->f1;}}_LL25B: if(
expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp433->topt))->v)){union Cyc_CfFlowInfo_FlowInfo _tmp448;union Cyc_CfFlowInfo_AbsLVal
_tmp449;struct _tuple14 _tmp447=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp433);
_tmp448=_tmp447.f1;_tmp449=_tmp447.f2;{struct _tuple14 _tmp8D6;struct _tuple14
_tmp44B=(_tmp8D6.f1=_tmp448,((_tmp8D6.f2=_tmp449,_tmp8D6)));union Cyc_CfFlowInfo_FlowInfo
_tmp44C;struct _tuple12 _tmp44D;struct Cyc_Dict_Dict _tmp44E;struct Cyc_List_List*
_tmp44F;union Cyc_CfFlowInfo_AbsLVal _tmp450;struct Cyc_CfFlowInfo_Place*_tmp451;
_LL268: _tmp44C=_tmp44B.f1;if((_tmp44C.ReachableFL).tag != 2)goto _LL26A;_tmp44D=(
struct _tuple12)(_tmp44C.ReachableFL).val;_tmp44E=_tmp44D.f1;_tmp44F=_tmp44D.f2;
_tmp450=_tmp44B.f2;if((_tmp450.PlaceL).tag != 1)goto _LL26A;_tmp451=(struct Cyc_CfFlowInfo_Place*)(
_tmp450.PlaceL).val;_LL269: {void*_tmp452=Cyc_CfFlowInfo_lookup_place(_tmp44E,
_tmp451);void*_tmp453=_tmp452;_LL26D: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp454=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp453;if(_tmp454->tag != 5)goto
_LL26F;}_LL26E: return Cyc_NewControlFlow_anal_derefL(env,_tmp448,_tmp433,_tmp448,
_tmp452,flds);_LL26F:;_LL270: {enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(
_tmp411,_tmp44E,_tmp452);void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp411->unknown_all:
_tmp411->unknown_none;void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp411,Cyc_Tcutil_pointer_elt_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp433->topt))->v),0,leaf);struct Cyc_CfFlowInfo_MallocPt_struct
_tmp8D9;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp8D8;void*new_root=(void*)((
_tmp8D8=_region_malloc(_tmp411->r,sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8D9.tag=1,((
_tmp8D9.f1=e,((_tmp8D9.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp8D9)))))),_tmp8D8))));struct Cyc_CfFlowInfo_Place*_tmp8DA;struct Cyc_CfFlowInfo_Place*
place=(_tmp8DA=_region_malloc(_tmp411->r,sizeof(*_tmp8DA)),((_tmp8DA->root=
new_root,((_tmp8DA->fields=0,_tmp8DA)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp8DD;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp8DC;void*res=(void*)((_tmp8DC=
_region_malloc(_tmp411->r,sizeof(*_tmp8DC)),((_tmp8DC[0]=((_tmp8DD.tag=5,((
_tmp8DD.f1=place,_tmp8DD)))),_tmp8DC))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);_tmp44E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(_tmp44E,new_root,new_rval);_tmp44E=Cyc_CfFlowInfo_assign_place(
_tmp411,e->loc,_tmp44E,env->all_changed,_tmp451,res);{union Cyc_CfFlowInfo_FlowInfo
_tmp455=Cyc_CfFlowInfo_ReachableFL(_tmp44E,_tmp44F);return Cyc_NewControlFlow_anal_derefL(
env,_tmp455,_tmp433,_tmp455,res,flds);};}_LL26C:;}_LL26A:;_LL26B: goto _LL267;
_LL267:;};}{union Cyc_CfFlowInfo_FlowInfo _tmp45C;void*_tmp45D;struct _tuple13
_tmp45B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp433);_tmp45C=_tmp45B.f1;
_tmp45D=_tmp45B.f2;return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp433,
_tmp45C,_tmp45D,flds);};_LL25C: {struct Cyc_Absyn_Subscript_e_struct*_tmp434=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp418;if(_tmp434->tag != 24)goto _LL25E;
else{_tmp435=_tmp434->f1;_tmp436=_tmp434->f2;}}_LL25D: {void*_tmp45E=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp435->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp461;struct Cyc_Absyn_PtrAtts _tmp462;union Cyc_Absyn_Constraint*_tmp463;_LL272: {
struct Cyc_Absyn_TupleType_struct*_tmp45F=(struct Cyc_Absyn_TupleType_struct*)
_tmp45E;if(_tmp45F->tag != 11)goto _LL274;}_LL273: {unsigned int _tmp464=(Cyc_Evexp_eval_const_uint_exp(
_tmp436)).f1;struct Cyc_List_List*_tmp8DE;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,expand_unique,_tmp435,((_tmp8DE=_region_malloc(env->r,sizeof(*_tmp8DE)),((
_tmp8DE->hd=(void*)((int)_tmp464),((_tmp8DE->tl=flds,_tmp8DE)))))));}_LL274: {
struct Cyc_Absyn_PointerType_struct*_tmp460=(struct Cyc_Absyn_PointerType_struct*)
_tmp45E;if(_tmp460->tag != 5)goto _LL276;else{_tmp461=_tmp460->f1;_tmp462=_tmp461.ptr_atts;
_tmp463=_tmp462.bounds;}}_LL275: {struct _RegionHandle*_tmp466=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp469;struct Cyc_List_List*_tmp46A;struct Cyc_Absyn_Exp*_tmp8DF[2];struct
_tuple19 _tmp468=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp466,env,inflow,((
_tmp8DF[1]=_tmp436,((_tmp8DF[0]=_tmp435,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp466,_tag_dyneither(
_tmp8DF,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp469=_tmp468.f1;_tmp46A=
_tmp468.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp46B=_tmp469;{union Cyc_CfFlowInfo_FlowInfo
_tmp46C=_tmp469;struct _tuple12 _tmp46D;struct Cyc_Dict_Dict _tmp46E;struct Cyc_List_List*
_tmp46F;_LL279: if((_tmp46C.ReachableFL).tag != 2)goto _LL27B;_tmp46D=(struct
_tuple12)(_tmp46C.ReachableFL).val;_tmp46E=_tmp46D.f1;_tmp46F=_tmp46D.f2;_LL27A:
if(Cyc_CfFlowInfo_initlevel(_tmp411,_tmp46E,(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp46A))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8E2;void*_tmp8E1;(_tmp8E1=0,Cyc_Tcutil_terr(_tmp436->loc,((_tmp8E2="expression may not be initialized",
_tag_dyneither(_tmp8E2,sizeof(char),34))),_tag_dyneither(_tmp8E1,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp472=Cyc_NewControlFlow_add_subscript_reln(_tmp411->r,
_tmp46F,_tmp435,_tmp436);if(_tmp46F != _tmp472)_tmp46B=Cyc_CfFlowInfo_ReachableFL(
_tmp46E,_tmp472);goto _LL278;};_LL27B:;_LL27C: goto _LL278;_LL278:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp474;union Cyc_CfFlowInfo_AbsLVal _tmp475;struct _tuple14 _tmp473=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp435,_tmp469,(void*)((struct Cyc_List_List*)_check_null(_tmp46A))->hd,
flds);_tmp474=_tmp473.f1;_tmp475=_tmp473.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp476=
_tmp474;int _tmp477;_LL27E: if((_tmp476.BottomFL).tag != 1)goto _LL280;_tmp477=(int)(
_tmp476.BottomFL).val;_LL27F: {struct _tuple14 _tmp8E3;return(_tmp8E3.f1=_tmp474,((
_tmp8E3.f2=_tmp475,_tmp8E3)));}_LL280:;_LL281: {struct _tuple14 _tmp8E4;return(
_tmp8E4.f1=_tmp46B,((_tmp8E4.f2=_tmp475,_tmp8E4)));}_LL27D:;};};};}_LL276:;
_LL277: {struct Cyc_Core_Impossible_struct _tmp8EA;const char*_tmp8E9;struct Cyc_Core_Impossible_struct*
_tmp8E8;(int)_throw((void*)((_tmp8E8=_cycalloc(sizeof(*_tmp8E8)),((_tmp8E8[0]=((
_tmp8EA.tag=Cyc_Core_Impossible,((_tmp8EA.f1=((_tmp8E9="anal_Lexp: Subscript -- bad type",
_tag_dyneither(_tmp8E9,sizeof(char),33))),_tmp8EA)))),_tmp8E8)))));}_LL271:;}
_LL25E: {struct Cyc_Absyn_AggrMember_e_struct*_tmp437=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp418;if(_tmp437->tag != 22)goto _LL260;else{_tmp438=_tmp437->f1;_tmp439=_tmp437->f2;}}
_LL25F: if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp438->topt))->v)){struct _tuple14 _tmp8EB;return(_tmp8EB.f1=inflow,((
_tmp8EB.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8EB)));}{struct Cyc_List_List*_tmp8EC;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp438,((
_tmp8EC=_region_malloc(env->r,sizeof(*_tmp8EC)),((_tmp8EC->hd=(void*)Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp438->topt))->v,_tmp439),((_tmp8EC->tl=
flds,_tmp8EC)))))));};_LL260:;_LL261: {struct _tuple14 _tmp8ED;return(_tmp8ED.f1=
Cyc_CfFlowInfo_BottomFL(),((_tmp8ED.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8ED)));}
_LL249:;};}static struct _tuple14 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
union Cyc_CfFlowInfo_FlowInfo _tmp481;union Cyc_CfFlowInfo_AbsLVal _tmp482;struct
_tuple14 _tmp480=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);
_tmp481=_tmp480.f1;_tmp482=_tmp480.f2;{struct _tuple14 _tmp8EE;return(_tmp8EE.f1=
_tmp481,((_tmp8EE.f2=_tmp482,_tmp8EE)));};}static struct _tuple15 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp484=env->fenv;{void*_tmp485=e->r;
struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Exp*_tmp488;struct Cyc_Absyn_Exp*
_tmp489;struct Cyc_Absyn_Exp*_tmp48B;struct Cyc_Absyn_Exp*_tmp48C;struct Cyc_Absyn_Exp*
_tmp48E;struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_Absyn_Exp*_tmp491;struct Cyc_Absyn_Exp*
_tmp492;enum Cyc_Absyn_Primop _tmp494;struct Cyc_List_List*_tmp495;struct Cyc_List_List
_tmp496;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_List_List*_tmp498;enum Cyc_Absyn_Primop
_tmp49A;struct Cyc_List_List*_tmp49B;_LL283: {struct Cyc_Absyn_Conditional_e_struct*
_tmp486=(struct Cyc_Absyn_Conditional_e_struct*)_tmp485;if(_tmp486->tag != 6)goto
_LL285;else{_tmp487=_tmp486->f1;_tmp488=_tmp486->f2;_tmp489=_tmp486->f3;}}_LL284: {
union Cyc_CfFlowInfo_FlowInfo _tmp49D;union Cyc_CfFlowInfo_FlowInfo _tmp49E;struct
_tuple15 _tmp49C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp487);_tmp49D=_tmp49C.f1;
_tmp49E=_tmp49C.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp4A0;union Cyc_CfFlowInfo_FlowInfo
_tmp4A1;struct _tuple15 _tmp49F=Cyc_NewControlFlow_anal_test(env,_tmp49D,_tmp488);
_tmp4A0=_tmp49F.f1;_tmp4A1=_tmp49F.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp4A3;union
Cyc_CfFlowInfo_FlowInfo _tmp4A4;struct _tuple15 _tmp4A2=Cyc_NewControlFlow_anal_test(
env,_tmp49E,_tmp489);_tmp4A3=_tmp4A2.f1;_tmp4A4=_tmp4A2.f2;{struct _tuple15
_tmp8EF;return(_tmp8EF.f1=Cyc_CfFlowInfo_join_flow(_tmp484,env->all_changed,
_tmp4A0,_tmp4A3),((_tmp8EF.f2=Cyc_CfFlowInfo_join_flow(_tmp484,env->all_changed,
_tmp4A1,_tmp4A4),_tmp8EF)));};};};}_LL285: {struct Cyc_Absyn_And_e_struct*_tmp48A=(
struct Cyc_Absyn_And_e_struct*)_tmp485;if(_tmp48A->tag != 7)goto _LL287;else{
_tmp48B=_tmp48A->f1;_tmp48C=_tmp48A->f2;}}_LL286: {union Cyc_CfFlowInfo_FlowInfo
_tmp4A7;union Cyc_CfFlowInfo_FlowInfo _tmp4A8;struct _tuple15 _tmp4A6=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp48B);_tmp4A7=_tmp4A6.f1;_tmp4A8=_tmp4A6.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4AA;union Cyc_CfFlowInfo_FlowInfo _tmp4AB;struct _tuple15 _tmp4A9=Cyc_NewControlFlow_anal_test(
env,_tmp4A7,_tmp48C);_tmp4AA=_tmp4A9.f1;_tmp4AB=_tmp4A9.f2;{struct _tuple15
_tmp8F0;return(_tmp8F0.f1=_tmp4AA,((_tmp8F0.f2=Cyc_CfFlowInfo_join_flow(_tmp484,
env->all_changed,_tmp4A8,_tmp4AB),_tmp8F0)));};};}_LL287: {struct Cyc_Absyn_Or_e_struct*
_tmp48D=(struct Cyc_Absyn_Or_e_struct*)_tmp485;if(_tmp48D->tag != 8)goto _LL289;
else{_tmp48E=_tmp48D->f1;_tmp48F=_tmp48D->f2;}}_LL288: {union Cyc_CfFlowInfo_FlowInfo
_tmp4AE;union Cyc_CfFlowInfo_FlowInfo _tmp4AF;struct _tuple15 _tmp4AD=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp48E);_tmp4AE=_tmp4AD.f1;_tmp4AF=_tmp4AD.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp4B1;union Cyc_CfFlowInfo_FlowInfo _tmp4B2;struct _tuple15 _tmp4B0=Cyc_NewControlFlow_anal_test(
env,_tmp4AF,_tmp48F);_tmp4B1=_tmp4B0.f1;_tmp4B2=_tmp4B0.f2;{struct _tuple15
_tmp8F1;return(_tmp8F1.f1=Cyc_CfFlowInfo_join_flow(_tmp484,env->all_changed,
_tmp4AE,_tmp4B1),((_tmp8F1.f2=_tmp4B2,_tmp8F1)));};};}_LL289: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp490=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp485;if(_tmp490->tag != 9)goto _LL28B;
else{_tmp491=_tmp490->f1;_tmp492=_tmp490->f2;}}_LL28A: {union Cyc_CfFlowInfo_FlowInfo
_tmp4B5;void*_tmp4B6;struct _tuple13 _tmp4B4=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,_tmp491);_tmp4B5=_tmp4B4.f1;_tmp4B6=_tmp4B4.f2;return Cyc_NewControlFlow_anal_test(
env,_tmp4B5,_tmp492);}_LL28B: {struct Cyc_Absyn_Primop_e_struct*_tmp493=(struct
Cyc_Absyn_Primop_e_struct*)_tmp485;if(_tmp493->tag != 3)goto _LL28D;else{_tmp494=
_tmp493->f1;if(_tmp494 != Cyc_Absyn_Not)goto _LL28D;_tmp495=_tmp493->f2;if(_tmp495
== 0)goto _LL28D;_tmp496=*_tmp495;_tmp497=(struct Cyc_Absyn_Exp*)_tmp496.hd;
_tmp498=_tmp496.tl;if(_tmp498 != 0)goto _LL28D;}}_LL28C: {union Cyc_CfFlowInfo_FlowInfo
_tmp4B8;union Cyc_CfFlowInfo_FlowInfo _tmp4B9;struct _tuple15 _tmp4B7=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp497);_tmp4B8=_tmp4B7.f1;_tmp4B9=_tmp4B7.f2;{struct _tuple15 _tmp8F2;
return(_tmp8F2.f1=_tmp4B9,((_tmp8F2.f2=_tmp4B8,_tmp8F2)));};}_LL28D: {struct Cyc_Absyn_Primop_e_struct*
_tmp499=(struct Cyc_Absyn_Primop_e_struct*)_tmp485;if(_tmp499->tag != 3)goto _LL28F;
else{_tmp49A=_tmp499->f1;_tmp49B=_tmp499->f2;}}_LL28E: {void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo
f;struct _RegionHandle*_tmp4BB=env->r;{union Cyc_CfFlowInfo_FlowInfo _tmp4BD;struct
Cyc_List_List*_tmp4BE;struct _tuple19 _tmp4BC=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp4BB,env,inflow,_tmp49B,0,0);_tmp4BD=_tmp4BC.f1;_tmp4BE=_tmp4BC.f2;r1=(void*)((
struct Cyc_List_List*)_check_null(_tmp4BE))->hd;r2=(void*)((struct Cyc_List_List*)
_check_null(_tmp4BE->tl))->hd;f=_tmp4BD;}{union Cyc_CfFlowInfo_FlowInfo _tmp4BF=f;
int _tmp4C0;struct _tuple12 _tmp4C1;struct Cyc_Dict_Dict _tmp4C2;struct Cyc_List_List*
_tmp4C3;_LL292: if((_tmp4BF.BottomFL).tag != 1)goto _LL294;_tmp4C0=(int)(_tmp4BF.BottomFL).val;
_LL293: {struct _tuple15 _tmp8F3;return(_tmp8F3.f1=f,((_tmp8F3.f2=f,_tmp8F3)));}
_LL294: if((_tmp4BF.ReachableFL).tag != 2)goto _LL291;_tmp4C1=(struct _tuple12)(
_tmp4BF.ReachableFL).val;_tmp4C2=_tmp4C1.f1;_tmp4C3=_tmp4C1.f2;_LL295: {struct
Cyc_Absyn_Exp*_tmp4C5=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp49B))->hd;struct Cyc_Absyn_Exp*_tmp4C6=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp49B->tl))->hd;if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4C2,r1)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp8F6;void*_tmp8F5;(_tmp8F5=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp49B->hd)->loc,((_tmp8F6="expression may not be initialized",
_tag_dyneither(_tmp8F6,sizeof(char),34))),_tag_dyneither(_tmp8F5,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4C2,r2)== Cyc_CfFlowInfo_NoneIL){const
char*_tmp8F9;void*_tmp8F8;(_tmp8F8=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp49B->tl))->hd)->loc,((_tmp8F9="expression may not be initialized",
_tag_dyneither(_tmp8F9,sizeof(char),34))),_tag_dyneither(_tmp8F8,sizeof(void*),0)));}
if(_tmp49A == Cyc_Absyn_Eq  || _tmp49A == Cyc_Absyn_Neq){struct _tuple0 _tmp8FA;
struct _tuple0 _tmp4CC=(_tmp8FA.f1=r1,((_tmp8FA.f2=r2,_tmp8FA)));void*_tmp4CD;
enum Cyc_CfFlowInfo_InitLevel _tmp4CF;void*_tmp4D0;void*_tmp4D2;void*_tmp4D4;
enum Cyc_CfFlowInfo_InitLevel _tmp4D6;void*_tmp4D7;void*_tmp4D9;void*_tmp4DB;void*
_tmp4DD;void*_tmp4DF;void*_tmp4E1;void*_tmp4E3;void*_tmp4E5;void*_tmp4E7;void*
_tmp4E9;void*_tmp4EB;void*_tmp4ED;void*_tmp4EF;void*_tmp4F1;_LL297: _tmp4CD=
_tmp4CC.f1;{struct Cyc_CfFlowInfo_UnknownR_struct*_tmp4CE=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4CD;if(_tmp4CE->tag != 3)goto _LL299;else{_tmp4CF=_tmp4CE->f1;}};_tmp4D0=
_tmp4CC.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D1=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4D0;if(_tmp4D1->tag != 0)goto _LL299;};_LL298: {union Cyc_CfFlowInfo_FlowInfo
_tmp4F4;union Cyc_CfFlowInfo_FlowInfo _tmp4F5;struct _tuple15 _tmp4F3=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4C5,_tmp4CF);_tmp4F4=_tmp4F3.f1;_tmp4F5=_tmp4F3.f2;switch(_tmp49A){
case Cyc_Absyn_Eq: _LL2AB: {struct _tuple15 _tmp8FB;return(_tmp8FB.f1=_tmp4F5,((
_tmp8FB.f2=_tmp4F4,_tmp8FB)));}case Cyc_Absyn_Neq: _LL2AC: {struct _tuple15 _tmp8FC;
return(_tmp8FC.f1=_tmp4F4,((_tmp8FC.f2=_tmp4F5,_tmp8FC)));}default: _LL2AD: {
struct Cyc_Core_Impossible_struct _tmp902;const char*_tmp901;struct Cyc_Core_Impossible_struct*
_tmp900;(int)_throw((void*)((_tmp900=_cycalloc(sizeof(*_tmp900)),((_tmp900[0]=((
_tmp902.tag=Cyc_Core_Impossible,((_tmp902.f1=((_tmp901="anal_test, zero-split",
_tag_dyneither(_tmp901,sizeof(char),22))),_tmp902)))),_tmp900)))));}}}_LL299:
_tmp4D2=_tmp4CC.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D3=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4D2;if(_tmp4D3->tag != 0)goto _LL29B;};_tmp4D4=_tmp4CC.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4D5=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4D4;if(_tmp4D5->tag != 3)goto
_LL29B;else{_tmp4D6=_tmp4D5->f1;}};_LL29A: {union Cyc_CfFlowInfo_FlowInfo _tmp4FC;
union Cyc_CfFlowInfo_FlowInfo _tmp4FD;struct _tuple15 _tmp4FB=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4C6,_tmp4D6);_tmp4FC=_tmp4FB.f1;_tmp4FD=_tmp4FB.f2;switch(_tmp49A){
case Cyc_Absyn_Eq: _LL2AF: {struct _tuple15 _tmp903;return(_tmp903.f1=_tmp4FD,((
_tmp903.f2=_tmp4FC,_tmp903)));}case Cyc_Absyn_Neq: _LL2B0: {struct _tuple15 _tmp904;
return(_tmp904.f1=_tmp4FC,((_tmp904.f2=_tmp4FD,_tmp904)));}default: _LL2B1: {
struct Cyc_Core_Impossible_struct _tmp90A;const char*_tmp909;struct Cyc_Core_Impossible_struct*
_tmp908;(int)_throw((void*)((_tmp908=_cycalloc(sizeof(*_tmp908)),((_tmp908[0]=((
_tmp90A.tag=Cyc_Core_Impossible,((_tmp90A.f1=((_tmp909="anal_test, zero-split",
_tag_dyneither(_tmp909,sizeof(char),22))),_tmp90A)))),_tmp908)))));}}}_LL29B:
_tmp4D7=_tmp4CC.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D8=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4D7;if(_tmp4D8->tag != 0)goto _LL29D;};_tmp4D9=_tmp4CC.f2;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4DA=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4D9;if(_tmp4DA->tag != 0)goto
_LL29D;};_LL29C: if(_tmp49A == Cyc_Absyn_Eq){struct _tuple15 _tmp90B;return(_tmp90B.f1=
f,((_tmp90B.f2=Cyc_CfFlowInfo_BottomFL(),_tmp90B)));}else{struct _tuple15 _tmp90C;
return(_tmp90C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp90C.f2=f,_tmp90C)));}_LL29D:
_tmp4DB=_tmp4CC.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4DC=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4DB;if(_tmp4DC->tag != 0)goto _LL29F;};_tmp4DD=_tmp4CC.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4DE=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4DD;if(_tmp4DE->tag != 1)
goto _LL29F;};_LL29E: goto _LL2A0;_LL29F: _tmp4DF=_tmp4CC.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4E0=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4DF;if(_tmp4E0->tag != 0)goto
_LL2A1;};_tmp4E1=_tmp4CC.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp4E2=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4E1;if(_tmp4E2->tag != 2)goto _LL2A1;};
_LL2A0: goto _LL2A2;_LL2A1: _tmp4E3=_tmp4CC.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4E4=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4E3;if(_tmp4E4->tag != 0)goto
_LL2A3;};_tmp4E5=_tmp4CC.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp4E6=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4E5;if(_tmp4E6->tag != 5)goto _LL2A3;};
_LL2A2: goto _LL2A4;_LL2A3: _tmp4E7=_tmp4CC.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4E8=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4E7;if(_tmp4E8->tag != 1)
goto _LL2A5;};_tmp4E9=_tmp4CC.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4EA=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4E9;if(_tmp4EA->tag != 0)goto _LL2A5;};
_LL2A4: goto _LL2A6;_LL2A5: _tmp4EB=_tmp4CC.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4EC=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4EB;if(_tmp4EC->tag != 2)
goto _LL2A7;};_tmp4ED=_tmp4CC.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4EE=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4ED;if(_tmp4EE->tag != 0)goto _LL2A7;};
_LL2A6: goto _LL2A8;_LL2A7: _tmp4EF=_tmp4CC.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp4F0=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4EF;if(_tmp4F0->tag != 5)goto
_LL2A9;};_tmp4F1=_tmp4CC.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4F2=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4F1;if(_tmp4F2->tag != 0)goto _LL2A9;};_LL2A8: if(_tmp49A == Cyc_Absyn_Neq){
struct _tuple15 _tmp90D;return(_tmp90D.f1=f,((_tmp90D.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp90D)));}else{struct _tuple15 _tmp90E;return(_tmp90E.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp90E.f2=f,_tmp90E)));}_LL2A9:;_LL2AA: goto _LL296;_LL296:;}{struct _tuple0
_tmp90F;struct _tuple0 _tmp508=(_tmp90F.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4C5->topt))->v),((_tmp90F.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4C6->topt))->v),_tmp90F)));void*_tmp509;enum Cyc_Absyn_Sign
_tmp50B;void*_tmp50C;enum Cyc_Absyn_Sign _tmp50E;void*_tmp50F;void*_tmp511;_LL2B4:
_tmp509=_tmp508.f1;{struct Cyc_Absyn_IntType_struct*_tmp50A=(struct Cyc_Absyn_IntType_struct*)
_tmp509;if(_tmp50A->tag != 6)goto _LL2B6;else{_tmp50B=_tmp50A->f1;if(_tmp50B != Cyc_Absyn_Unsigned)
goto _LL2B6;}};_LL2B5: goto _LL2B7;_LL2B6: _tmp50C=_tmp508.f2;{struct Cyc_Absyn_IntType_struct*
_tmp50D=(struct Cyc_Absyn_IntType_struct*)_tmp50C;if(_tmp50D->tag != 6)goto _LL2B8;
else{_tmp50E=_tmp50D->f1;if(_tmp50E != Cyc_Absyn_Unsigned)goto _LL2B8;}};_LL2B7:
goto _LL2B9;_LL2B8: _tmp50F=_tmp508.f1;{struct Cyc_Absyn_TagType_struct*_tmp510=(
struct Cyc_Absyn_TagType_struct*)_tmp50F;if(_tmp510->tag != 20)goto _LL2BA;};_LL2B9:
goto _LL2BB;_LL2BA: _tmp511=_tmp508.f2;{struct Cyc_Absyn_TagType_struct*_tmp512=(
struct Cyc_Absyn_TagType_struct*)_tmp511;if(_tmp512->tag != 20)goto _LL2BC;};_LL2BB:
goto _LL2B3;_LL2BC:;_LL2BD: {struct _tuple15 _tmp910;return(_tmp910.f1=f,((_tmp910.f2=
f,_tmp910)));}_LL2B3:;}switch(_tmp49A){case Cyc_Absyn_Eq: _LL2BE: goto _LL2BF;case
Cyc_Absyn_Neq: _LL2BF: goto _LL2C0;case Cyc_Absyn_Gt: _LL2C0: goto _LL2C1;case Cyc_Absyn_Gte:
_LL2C1: {struct _tuple15 _tmp911;return(_tmp911.f1=f,((_tmp911.f2=f,_tmp911)));}
case Cyc_Absyn_Lt: _LL2C2: {union Cyc_CfFlowInfo_FlowInfo _tmp515=f;union Cyc_CfFlowInfo_FlowInfo
_tmp516=f;{union Cyc_CfFlowInfo_FlowInfo _tmp517=_tmp515;int _tmp518;struct _tuple12
_tmp519;struct Cyc_Dict_Dict _tmp51A;_LL2C5: if((_tmp517.BottomFL).tag != 1)goto
_LL2C7;_tmp518=(int)(_tmp517.BottomFL).val;_LL2C6: {struct Cyc_Core_Impossible_struct
_tmp917;const char*_tmp916;struct Cyc_Core_Impossible_struct*_tmp915;(int)_throw((
void*)((_tmp915=_cycalloc(sizeof(*_tmp915)),((_tmp915[0]=((_tmp917.tag=Cyc_Core_Impossible,((
_tmp917.f1=((_tmp916="anal_test, Lt",_tag_dyneither(_tmp916,sizeof(char),14))),
_tmp917)))),_tmp915)))));}_LL2C7: if((_tmp517.ReachableFL).tag != 2)goto _LL2C4;
_tmp519=(struct _tuple12)(_tmp517.ReachableFL).val;_tmp51A=_tmp519.f1;_LL2C8:
_tmp4C2=_tmp51A;_LL2C4:;}{void*_tmp51E=_tmp4C5->r;void*_tmp520;struct Cyc_Absyn_Vardecl*
_tmp522;void*_tmp524;struct Cyc_Absyn_Vardecl*_tmp526;void*_tmp528;struct Cyc_Absyn_Vardecl*
_tmp52A;void*_tmp52C;struct Cyc_Absyn_Vardecl*_tmp52E;_LL2CA:{struct Cyc_Absyn_Var_e_struct*
_tmp51F=(struct Cyc_Absyn_Var_e_struct*)_tmp51E;if(_tmp51F->tag != 1)goto _LL2CC;
else{_tmp520=(void*)_tmp51F->f2;{struct Cyc_Absyn_Global_b_struct*_tmp521=(struct
Cyc_Absyn_Global_b_struct*)_tmp520;if(_tmp521->tag != 1)goto _LL2CC;else{_tmp522=
_tmp521->f1;}};}}if(!(!_tmp522->escapes))goto _LL2CC;_LL2CB: _tmp526=_tmp522;goto
_LL2CD;_LL2CC:{struct Cyc_Absyn_Var_e_struct*_tmp523=(struct Cyc_Absyn_Var_e_struct*)
_tmp51E;if(_tmp523->tag != 1)goto _LL2CE;else{_tmp524=(void*)_tmp523->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp525=(struct Cyc_Absyn_Local_b_struct*)_tmp524;if(
_tmp525->tag != 4)goto _LL2CE;else{_tmp526=_tmp525->f1;}};}}if(!(!_tmp526->escapes))
goto _LL2CE;_LL2CD: _tmp52A=_tmp526;goto _LL2CF;_LL2CE:{struct Cyc_Absyn_Var_e_struct*
_tmp527=(struct Cyc_Absyn_Var_e_struct*)_tmp51E;if(_tmp527->tag != 1)goto _LL2D0;
else{_tmp528=(void*)_tmp527->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp529=(struct Cyc_Absyn_Pat_b_struct*)
_tmp528;if(_tmp529->tag != 5)goto _LL2D0;else{_tmp52A=_tmp529->f1;}};}}if(!(!
_tmp52A->escapes))goto _LL2D0;_LL2CF: _tmp52E=_tmp52A;goto _LL2D1;_LL2D0:{struct Cyc_Absyn_Var_e_struct*
_tmp52B=(struct Cyc_Absyn_Var_e_struct*)_tmp51E;if(_tmp52B->tag != 1)goto _LL2D2;
else{_tmp52C=(void*)_tmp52B->f2;{struct Cyc_Absyn_Param_b_struct*_tmp52D=(struct
Cyc_Absyn_Param_b_struct*)_tmp52C;if(_tmp52D->tag != 3)goto _LL2D2;else{_tmp52E=
_tmp52D->f1;}};}}if(!(!_tmp52E->escapes))goto _LL2D2;_LL2D1: {void*_tmp52F=
_tmp4C6->r;void*_tmp531;struct Cyc_Absyn_Vardecl*_tmp533;void*_tmp535;struct Cyc_Absyn_Vardecl*
_tmp537;void*_tmp539;struct Cyc_Absyn_Vardecl*_tmp53B;void*_tmp53D;struct Cyc_Absyn_Vardecl*
_tmp53F;union Cyc_Absyn_Cnst _tmp541;struct _tuple6 _tmp542;int _tmp543;struct Cyc_Absyn_Exp*
_tmp545;struct Cyc_Absyn_Exp _tmp546;void*_tmp547;union Cyc_Absyn_Cnst _tmp549;
struct _tuple6 _tmp54A;int _tmp54B;enum Cyc_Absyn_Primop _tmp54D;struct Cyc_List_List*
_tmp54E;struct Cyc_List_List _tmp54F;struct Cyc_Absyn_Exp*_tmp550;_LL2D5:{struct Cyc_Absyn_Var_e_struct*
_tmp530=(struct Cyc_Absyn_Var_e_struct*)_tmp52F;if(_tmp530->tag != 1)goto _LL2D7;
else{_tmp531=(void*)_tmp530->f2;{struct Cyc_Absyn_Global_b_struct*_tmp532=(struct
Cyc_Absyn_Global_b_struct*)_tmp531;if(_tmp532->tag != 1)goto _LL2D7;else{_tmp533=
_tmp532->f1;}};}}if(!(!_tmp533->escapes))goto _LL2D7;_LL2D6: _tmp537=_tmp533;goto
_LL2D8;_LL2D7:{struct Cyc_Absyn_Var_e_struct*_tmp534=(struct Cyc_Absyn_Var_e_struct*)
_tmp52F;if(_tmp534->tag != 1)goto _LL2D9;else{_tmp535=(void*)_tmp534->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp536=(struct Cyc_Absyn_Local_b_struct*)_tmp535;if(
_tmp536->tag != 4)goto _LL2D9;else{_tmp537=_tmp536->f1;}};}}if(!(!_tmp537->escapes))
goto _LL2D9;_LL2D8: _tmp53B=_tmp537;goto _LL2DA;_LL2D9:{struct Cyc_Absyn_Var_e_struct*
_tmp538=(struct Cyc_Absyn_Var_e_struct*)_tmp52F;if(_tmp538->tag != 1)goto _LL2DB;
else{_tmp539=(void*)_tmp538->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp53A=(struct Cyc_Absyn_Pat_b_struct*)
_tmp539;if(_tmp53A->tag != 5)goto _LL2DB;else{_tmp53B=_tmp53A->f1;}};}}if(!(!
_tmp53B->escapes))goto _LL2DB;_LL2DA: _tmp53F=_tmp53B;goto _LL2DC;_LL2DB:{struct Cyc_Absyn_Var_e_struct*
_tmp53C=(struct Cyc_Absyn_Var_e_struct*)_tmp52F;if(_tmp53C->tag != 1)goto _LL2DD;
else{_tmp53D=(void*)_tmp53C->f2;{struct Cyc_Absyn_Param_b_struct*_tmp53E=(struct
Cyc_Absyn_Param_b_struct*)_tmp53D;if(_tmp53E->tag != 3)goto _LL2DD;else{_tmp53F=
_tmp53E->f1;}};}}if(!(!_tmp53F->escapes))goto _LL2DD;_LL2DC: {struct _RegionHandle*
_tmp551=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp91A;struct Cyc_List_List*
_tmp919;struct Cyc_List_List*_tmp552=(_tmp919=_region_malloc(_tmp551,sizeof(*
_tmp919)),((_tmp919->hd=((_tmp91A=_region_malloc(_tmp551,sizeof(*_tmp91A)),((
_tmp91A->vd=_tmp52E,((_tmp91A->rop=Cyc_CfFlowInfo_LessVar(_tmp53F,_tmp53F->type),
_tmp91A)))))),((_tmp919->tl=_tmp4C3,_tmp919)))));struct _tuple15 _tmp91B;return(
_tmp91B.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4C2,_tmp552),((_tmp91B.f2=_tmp516,
_tmp91B)));}_LL2DD: {struct Cyc_Absyn_Const_e_struct*_tmp540=(struct Cyc_Absyn_Const_e_struct*)
_tmp52F;if(_tmp540->tag != 0)goto _LL2DF;else{_tmp541=_tmp540->f1;if((_tmp541.Int_c).tag
!= 4)goto _LL2DF;_tmp542=(struct _tuple6)(_tmp541.Int_c).val;_tmp543=_tmp542.f2;}}
_LL2DE: _tmp54B=_tmp543;goto _LL2E0;_LL2DF: {struct Cyc_Absyn_Cast_e_struct*_tmp544=(
struct Cyc_Absyn_Cast_e_struct*)_tmp52F;if(_tmp544->tag != 15)goto _LL2E1;else{
_tmp545=_tmp544->f2;_tmp546=*_tmp545;_tmp547=_tmp546.r;{struct Cyc_Absyn_Const_e_struct*
_tmp548=(struct Cyc_Absyn_Const_e_struct*)_tmp547;if(_tmp548->tag != 0)goto _LL2E1;
else{_tmp549=_tmp548->f1;if((_tmp549.Int_c).tag != 4)goto _LL2E1;_tmp54A=(struct
_tuple6)(_tmp549.Int_c).val;_tmp54B=_tmp54A.f2;}};}}_LL2E0: {struct _RegionHandle*
_tmp556=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp91E;struct Cyc_List_List*
_tmp91D;struct Cyc_List_List*_tmp557=(_tmp91D=_region_malloc(_tmp556,sizeof(*
_tmp91D)),((_tmp91D->hd=((_tmp91E=_region_malloc(_tmp556,sizeof(*_tmp91E)),((
_tmp91E->vd=_tmp52E,((_tmp91E->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp54B),_tmp91E)))))),((_tmp91D->tl=_tmp4C3,_tmp91D)))));struct _tuple15 _tmp91F;
return(_tmp91F.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4C2,_tmp557),((_tmp91F.f2=
_tmp516,_tmp91F)));}_LL2E1: {struct Cyc_Absyn_Primop_e_struct*_tmp54C=(struct Cyc_Absyn_Primop_e_struct*)
_tmp52F;if(_tmp54C->tag != 3)goto _LL2E3;else{_tmp54D=_tmp54C->f1;_tmp54E=_tmp54C->f2;
if(_tmp54E == 0)goto _LL2E3;_tmp54F=*_tmp54E;_tmp550=(struct Cyc_Absyn_Exp*)_tmp54F.hd;}}
_LL2E2: {void*_tmp55B=_tmp550->r;void*_tmp55D;struct Cyc_Absyn_Vardecl*_tmp55F;
void*_tmp561;struct Cyc_Absyn_Vardecl*_tmp563;void*_tmp565;struct Cyc_Absyn_Vardecl*
_tmp567;void*_tmp569;struct Cyc_Absyn_Vardecl*_tmp56B;_LL2E6:{struct Cyc_Absyn_Var_e_struct*
_tmp55C=(struct Cyc_Absyn_Var_e_struct*)_tmp55B;if(_tmp55C->tag != 1)goto _LL2E8;
else{_tmp55D=(void*)_tmp55C->f2;{struct Cyc_Absyn_Global_b_struct*_tmp55E=(struct
Cyc_Absyn_Global_b_struct*)_tmp55D;if(_tmp55E->tag != 1)goto _LL2E8;else{_tmp55F=
_tmp55E->f1;}};}}if(!(!_tmp55F->escapes))goto _LL2E8;_LL2E7: _tmp563=_tmp55F;goto
_LL2E9;_LL2E8:{struct Cyc_Absyn_Var_e_struct*_tmp560=(struct Cyc_Absyn_Var_e_struct*)
_tmp55B;if(_tmp560->tag != 1)goto _LL2EA;else{_tmp561=(void*)_tmp560->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp562=(struct Cyc_Absyn_Local_b_struct*)_tmp561;if(
_tmp562->tag != 4)goto _LL2EA;else{_tmp563=_tmp562->f1;}};}}if(!(!_tmp563->escapes))
goto _LL2EA;_LL2E9: _tmp567=_tmp563;goto _LL2EB;_LL2EA:{struct Cyc_Absyn_Var_e_struct*
_tmp564=(struct Cyc_Absyn_Var_e_struct*)_tmp55B;if(_tmp564->tag != 1)goto _LL2EC;
else{_tmp565=(void*)_tmp564->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp566=(struct Cyc_Absyn_Pat_b_struct*)
_tmp565;if(_tmp566->tag != 5)goto _LL2EC;else{_tmp567=_tmp566->f1;}};}}if(!(!
_tmp567->escapes))goto _LL2EC;_LL2EB: _tmp56B=_tmp567;goto _LL2ED;_LL2EC:{struct Cyc_Absyn_Var_e_struct*
_tmp568=(struct Cyc_Absyn_Var_e_struct*)_tmp55B;if(_tmp568->tag != 1)goto _LL2EE;
else{_tmp569=(void*)_tmp568->f2;{struct Cyc_Absyn_Param_b_struct*_tmp56A=(struct
Cyc_Absyn_Param_b_struct*)_tmp569;if(_tmp56A->tag != 3)goto _LL2EE;else{_tmp56B=
_tmp56A->f1;}};}}if(!(!_tmp56B->escapes))goto _LL2EE;_LL2ED: {struct _RegionHandle*
_tmp56C=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp922;struct Cyc_List_List*
_tmp921;struct Cyc_List_List*_tmp56D=(_tmp921=_region_malloc(_tmp56C,sizeof(*
_tmp921)),((_tmp921->hd=((_tmp922=_region_malloc(_tmp56C,sizeof(*_tmp922)),((
_tmp922->vd=_tmp52E,((_tmp922->rop=Cyc_CfFlowInfo_LessNumelts(_tmp56B),_tmp922)))))),((
_tmp921->tl=_tmp4C3,_tmp921)))));struct _tuple15 _tmp923;return(_tmp923.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4C2,_tmp56D),((_tmp923.f2=_tmp516,_tmp923)));}_LL2EE:;_LL2EF: {struct
_tuple15 _tmp924;return(_tmp924.f1=_tmp515,((_tmp924.f2=_tmp516,_tmp924)));}
_LL2E5:;}_LL2E3:;_LL2E4: {struct _tuple15 _tmp925;return(_tmp925.f1=_tmp515,((
_tmp925.f2=_tmp516,_tmp925)));}_LL2D4:;}_LL2D2:;_LL2D3: {struct _tuple15 _tmp926;
return(_tmp926.f1=_tmp515,((_tmp926.f2=_tmp516,_tmp926)));}_LL2C9:;};}case Cyc_Absyn_Lte:
_LL2C3: {union Cyc_CfFlowInfo_FlowInfo _tmp574=f;union Cyc_CfFlowInfo_FlowInfo
_tmp575=f;{union Cyc_CfFlowInfo_FlowInfo _tmp576=_tmp574;int _tmp577;struct _tuple12
_tmp578;struct Cyc_Dict_Dict _tmp579;_LL2F2: if((_tmp576.BottomFL).tag != 1)goto
_LL2F4;_tmp577=(int)(_tmp576.BottomFL).val;_LL2F3: {struct Cyc_Core_Impossible_struct
_tmp92C;const char*_tmp92B;struct Cyc_Core_Impossible_struct*_tmp92A;(int)_throw((
void*)((_tmp92A=_cycalloc(sizeof(*_tmp92A)),((_tmp92A[0]=((_tmp92C.tag=Cyc_Core_Impossible,((
_tmp92C.f1=((_tmp92B="anal_test, Lte",_tag_dyneither(_tmp92B,sizeof(char),15))),
_tmp92C)))),_tmp92A)))));}_LL2F4: if((_tmp576.ReachableFL).tag != 2)goto _LL2F1;
_tmp578=(struct _tuple12)(_tmp576.ReachableFL).val;_tmp579=_tmp578.f1;_LL2F5:
_tmp4C2=_tmp579;_LL2F1:;}{void*_tmp57D=_tmp4C5->r;void*_tmp57F;struct Cyc_Absyn_Vardecl*
_tmp581;void*_tmp583;struct Cyc_Absyn_Vardecl*_tmp585;void*_tmp587;struct Cyc_Absyn_Vardecl*
_tmp589;void*_tmp58B;struct Cyc_Absyn_Vardecl*_tmp58D;_LL2F7:{struct Cyc_Absyn_Var_e_struct*
_tmp57E=(struct Cyc_Absyn_Var_e_struct*)_tmp57D;if(_tmp57E->tag != 1)goto _LL2F9;
else{_tmp57F=(void*)_tmp57E->f2;{struct Cyc_Absyn_Global_b_struct*_tmp580=(struct
Cyc_Absyn_Global_b_struct*)_tmp57F;if(_tmp580->tag != 1)goto _LL2F9;else{_tmp581=
_tmp580->f1;}};}}if(!(!_tmp581->escapes))goto _LL2F9;_LL2F8: _tmp585=_tmp581;goto
_LL2FA;_LL2F9:{struct Cyc_Absyn_Var_e_struct*_tmp582=(struct Cyc_Absyn_Var_e_struct*)
_tmp57D;if(_tmp582->tag != 1)goto _LL2FB;else{_tmp583=(void*)_tmp582->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp584=(struct Cyc_Absyn_Local_b_struct*)_tmp583;if(
_tmp584->tag != 4)goto _LL2FB;else{_tmp585=_tmp584->f1;}};}}if(!(!_tmp585->escapes))
goto _LL2FB;_LL2FA: _tmp589=_tmp585;goto _LL2FC;_LL2FB:{struct Cyc_Absyn_Var_e_struct*
_tmp586=(struct Cyc_Absyn_Var_e_struct*)_tmp57D;if(_tmp586->tag != 1)goto _LL2FD;
else{_tmp587=(void*)_tmp586->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp588=(struct Cyc_Absyn_Pat_b_struct*)
_tmp587;if(_tmp588->tag != 5)goto _LL2FD;else{_tmp589=_tmp588->f1;}};}}if(!(!
_tmp589->escapes))goto _LL2FD;_LL2FC: _tmp58D=_tmp589;goto _LL2FE;_LL2FD:{struct Cyc_Absyn_Var_e_struct*
_tmp58A=(struct Cyc_Absyn_Var_e_struct*)_tmp57D;if(_tmp58A->tag != 1)goto _LL2FF;
else{_tmp58B=(void*)_tmp58A->f2;{struct Cyc_Absyn_Param_b_struct*_tmp58C=(struct
Cyc_Absyn_Param_b_struct*)_tmp58B;if(_tmp58C->tag != 3)goto _LL2FF;else{_tmp58D=
_tmp58C->f1;}};}}if(!(!_tmp58D->escapes))goto _LL2FF;_LL2FE: {void*_tmp58E=
_tmp4C6->r;union Cyc_Absyn_Cnst _tmp590;struct _tuple6 _tmp591;int _tmp592;struct Cyc_Absyn_Exp*
_tmp594;struct Cyc_Absyn_Exp _tmp595;void*_tmp596;union Cyc_Absyn_Cnst _tmp598;
struct _tuple6 _tmp599;int _tmp59A;enum Cyc_Absyn_Primop _tmp59C;struct Cyc_List_List*
_tmp59D;struct Cyc_List_List _tmp59E;struct Cyc_Absyn_Exp*_tmp59F;_LL302: {struct
Cyc_Absyn_Const_e_struct*_tmp58F=(struct Cyc_Absyn_Const_e_struct*)_tmp58E;if(
_tmp58F->tag != 0)goto _LL304;else{_tmp590=_tmp58F->f1;if((_tmp590.Int_c).tag != 4)
goto _LL304;_tmp591=(struct _tuple6)(_tmp590.Int_c).val;_tmp592=_tmp591.f2;}}
_LL303: _tmp59A=_tmp592;goto _LL305;_LL304: {struct Cyc_Absyn_Cast_e_struct*_tmp593=(
struct Cyc_Absyn_Cast_e_struct*)_tmp58E;if(_tmp593->tag != 15)goto _LL306;else{
_tmp594=_tmp593->f2;_tmp595=*_tmp594;_tmp596=_tmp595.r;{struct Cyc_Absyn_Const_e_struct*
_tmp597=(struct Cyc_Absyn_Const_e_struct*)_tmp596;if(_tmp597->tag != 0)goto _LL306;
else{_tmp598=_tmp597->f1;if((_tmp598.Int_c).tag != 4)goto _LL306;_tmp599=(struct
_tuple6)(_tmp598.Int_c).val;_tmp59A=_tmp599.f2;}};}}_LL305: {struct _RegionHandle*
_tmp5A0=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp92F;struct Cyc_List_List*
_tmp92E;struct Cyc_List_List*_tmp5A1=(_tmp92E=_region_malloc(_tmp5A0,sizeof(*
_tmp92E)),((_tmp92E->hd=((_tmp92F=_region_malloc(_tmp5A0,sizeof(*_tmp92F)),((
_tmp92F->vd=_tmp58D,((_tmp92F->rop=Cyc_CfFlowInfo_LessEqConst((unsigned int)
_tmp59A),_tmp92F)))))),((_tmp92E->tl=_tmp4C3,_tmp92E)))));struct _tuple15 _tmp930;
return(_tmp930.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4C2,_tmp5A1),((_tmp930.f2=
_tmp575,_tmp930)));}_LL306: {struct Cyc_Absyn_Primop_e_struct*_tmp59B=(struct Cyc_Absyn_Primop_e_struct*)
_tmp58E;if(_tmp59B->tag != 3)goto _LL308;else{_tmp59C=_tmp59B->f1;_tmp59D=_tmp59B->f2;
if(_tmp59D == 0)goto _LL308;_tmp59E=*_tmp59D;_tmp59F=(struct Cyc_Absyn_Exp*)_tmp59E.hd;}}
_LL307: {void*_tmp5A5=_tmp59F->r;void*_tmp5A7;struct Cyc_Absyn_Vardecl*_tmp5A9;
void*_tmp5AB;struct Cyc_Absyn_Vardecl*_tmp5AD;void*_tmp5AF;struct Cyc_Absyn_Vardecl*
_tmp5B1;void*_tmp5B3;struct Cyc_Absyn_Vardecl*_tmp5B5;_LL30B:{struct Cyc_Absyn_Var_e_struct*
_tmp5A6=(struct Cyc_Absyn_Var_e_struct*)_tmp5A5;if(_tmp5A6->tag != 1)goto _LL30D;
else{_tmp5A7=(void*)_tmp5A6->f2;{struct Cyc_Absyn_Global_b_struct*_tmp5A8=(struct
Cyc_Absyn_Global_b_struct*)_tmp5A7;if(_tmp5A8->tag != 1)goto _LL30D;else{_tmp5A9=
_tmp5A8->f1;}};}}if(!(!_tmp5A9->escapes))goto _LL30D;_LL30C: _tmp5AD=_tmp5A9;goto
_LL30E;_LL30D:{struct Cyc_Absyn_Var_e_struct*_tmp5AA=(struct Cyc_Absyn_Var_e_struct*)
_tmp5A5;if(_tmp5AA->tag != 1)goto _LL30F;else{_tmp5AB=(void*)_tmp5AA->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp5AC=(struct Cyc_Absyn_Local_b_struct*)_tmp5AB;if(
_tmp5AC->tag != 4)goto _LL30F;else{_tmp5AD=_tmp5AC->f1;}};}}if(!(!_tmp5AD->escapes))
goto _LL30F;_LL30E: _tmp5B1=_tmp5AD;goto _LL310;_LL30F:{struct Cyc_Absyn_Var_e_struct*
_tmp5AE=(struct Cyc_Absyn_Var_e_struct*)_tmp5A5;if(_tmp5AE->tag != 1)goto _LL311;
else{_tmp5AF=(void*)_tmp5AE->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp5B0=(struct Cyc_Absyn_Pat_b_struct*)
_tmp5AF;if(_tmp5B0->tag != 5)goto _LL311;else{_tmp5B1=_tmp5B0->f1;}};}}if(!(!
_tmp5B1->escapes))goto _LL311;_LL310: _tmp5B5=_tmp5B1;goto _LL312;_LL311:{struct Cyc_Absyn_Var_e_struct*
_tmp5B2=(struct Cyc_Absyn_Var_e_struct*)_tmp5A5;if(_tmp5B2->tag != 1)goto _LL313;
else{_tmp5B3=(void*)_tmp5B2->f2;{struct Cyc_Absyn_Param_b_struct*_tmp5B4=(struct
Cyc_Absyn_Param_b_struct*)_tmp5B3;if(_tmp5B4->tag != 3)goto _LL313;else{_tmp5B5=
_tmp5B4->f1;}};}}if(!(!_tmp5B5->escapes))goto _LL313;_LL312: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp5B6=env->fenv;struct Cyc_CfFlowInfo_Reln*_tmp933;struct Cyc_List_List*_tmp932;
struct Cyc_List_List*_tmp5B7=(_tmp932=_region_malloc(_tmp5B6->r,sizeof(*_tmp932)),((
_tmp932->hd=((_tmp933=_region_malloc(_tmp5B6->r,sizeof(*_tmp933)),((_tmp933->vd=
_tmp58D,((_tmp933->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp5B5),_tmp933)))))),((
_tmp932->tl=_tmp4C3,_tmp932)))));struct _tuple15 _tmp934;return(_tmp934.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4C2,_tmp5B7),((_tmp934.f2=_tmp575,_tmp934)));}_LL313:;_LL314: {struct
_tuple15 _tmp935;return(_tmp935.f1=_tmp574,((_tmp935.f2=_tmp575,_tmp935)));}
_LL30A:;}_LL308:;_LL309: {struct _tuple15 _tmp936;return(_tmp936.f1=_tmp574,((
_tmp936.f2=_tmp575,_tmp936)));}_LL301:;}_LL2FF:;_LL300: {struct _tuple15 _tmp937;
return(_tmp937.f1=_tmp574,((_tmp937.f2=_tmp575,_tmp937)));}_LL2F6:;};}default:
_LL2F0: {struct _tuple15 _tmp938;return(_tmp938.f1=f,((_tmp938.f2=f,_tmp938)));}}}
_LL291:;};}_LL28F:;_LL290: goto _LL282;_LL282:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp5C0;void*_tmp5C1;struct _tuple13 _tmp5BF=Cyc_NewControlFlow_anal_Rexp(env,0,
inflow,e);_tmp5C0=_tmp5BF.f1;_tmp5C1=_tmp5BF.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp5C2=_tmp5C0;int _tmp5C3;struct _tuple12 _tmp5C4;struct Cyc_Dict_Dict _tmp5C5;
_LL317: if((_tmp5C2.BottomFL).tag != 1)goto _LL319;_tmp5C3=(int)(_tmp5C2.BottomFL).val;
_LL318: {struct _tuple15 _tmp939;return(_tmp939.f1=_tmp5C0,((_tmp939.f2=_tmp5C0,
_tmp939)));}_LL319: if((_tmp5C2.ReachableFL).tag != 2)goto _LL316;_tmp5C4=(struct
_tuple12)(_tmp5C2.ReachableFL).val;_tmp5C5=_tmp5C4.f1;_LL31A: {void*_tmp5C7=
_tmp5C1;enum Cyc_CfFlowInfo_InitLevel _tmp5CD;enum Cyc_CfFlowInfo_InitLevel
_tmp5CF;enum Cyc_CfFlowInfo_InitLevel _tmp5D2;_LL31C: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5C8=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5C7;if(_tmp5C8->tag != 0)goto
_LL31E;}_LL31D: {struct _tuple15 _tmp93A;return(_tmp93A.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp93A.f2=_tmp5C0,_tmp93A)));}_LL31E: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp5C9=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp5C7;if(_tmp5C9->tag != 2)
goto _LL320;}_LL31F: goto _LL321;_LL320: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp5CA=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp5C7;if(_tmp5CA->tag != 1)
goto _LL322;}_LL321: goto _LL323;_LL322: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp5CB=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp5C7;if(_tmp5CB->tag != 5)goto
_LL324;}_LL323: {struct _tuple15 _tmp93B;return(_tmp93B.f1=_tmp5C0,((_tmp93B.f2=
Cyc_CfFlowInfo_BottomFL(),_tmp93B)));}_LL324: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp5CC=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5C7;if(_tmp5CC->tag != 3)goto
_LL326;else{_tmp5CD=_tmp5CC->f1;if(_tmp5CD != Cyc_CfFlowInfo_NoneIL)goto _LL326;}}
_LL325: goto _LL327;_LL326: {struct Cyc_CfFlowInfo_Esc_struct*_tmp5CE=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp5C7;if(_tmp5CE->tag != 4)goto _LL328;else{_tmp5CF=_tmp5CE->f1;if(_tmp5CF != Cyc_CfFlowInfo_NoneIL)
goto _LL328;}}_LL327: goto _LL329;_LL328: {struct Cyc_CfFlowInfo_Consumed_struct*
_tmp5D0=(struct Cyc_CfFlowInfo_Consumed_struct*)_tmp5C7;if(_tmp5D0->tag != 7)goto
_LL32A;}_LL329:{const char*_tmp93E;void*_tmp93D;(_tmp93D=0,Cyc_Tcutil_terr(e->loc,((
_tmp93E="expression may not be initialized",_tag_dyneither(_tmp93E,sizeof(char),
34))),_tag_dyneither(_tmp93D,sizeof(void*),0)));}{struct _tuple15 _tmp93F;return(
_tmp93F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp93F.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp93F)));};_LL32A: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5D1=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp5C7;if(_tmp5D1->tag != 3)goto _LL32C;else{_tmp5D2=_tmp5D1->f1;}}_LL32B: return
Cyc_NewControlFlow_splitzero(env,inflow,_tmp5C0,e,_tmp5D2);_LL32C: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp5D3=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp5C7;if(_tmp5D3->tag != 4)goto _LL32E;}
_LL32D: {struct _tuple15 _tmp940;return(_tmp940.f1=_tmp5C0,((_tmp940.f2=_tmp5C0,
_tmp940)));}_LL32E: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5D4=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp5C7;if(_tmp5D4->tag != 6)goto _LL31B;}_LL32F: {struct Cyc_Core_Impossible_struct
_tmp946;const char*_tmp945;struct Cyc_Core_Impossible_struct*_tmp944;(int)_throw((
void*)((_tmp944=_cycalloc(sizeof(*_tmp944)),((_tmp944[0]=((_tmp946.tag=Cyc_Core_Impossible,((
_tmp946.f1=((_tmp945="anal_test",_tag_dyneither(_tmp945,sizeof(char),10))),
_tmp946)))),_tmp944)))));}_LL31B:;}_LL316:;};};}struct _tuple22{struct Cyc_Position_Segment*
f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};static void Cyc_NewControlFlow_check_for_unused_unique(
struct _tuple22*ckenv,void*root,void*rval){struct _tuple22 _tmp5DF;struct Cyc_Position_Segment*
_tmp5E0;struct Cyc_NewControlFlow_AnalEnv*_tmp5E1;struct Cyc_Dict_Dict _tmp5E2;
struct _tuple22*_tmp5DE=ckenv;_tmp5DF=*_tmp5DE;_tmp5E0=_tmp5DF.f1;_tmp5E1=_tmp5DF.f2;
_tmp5E2=_tmp5DF.f3;{void*_tmp5E3=root;struct Cyc_Absyn_Vardecl*_tmp5E5;_LL331: {
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp5E4=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp5E3;if(_tmp5E4->tag != 0)goto _LL333;else{_tmp5E5=_tmp5E4->f1;}}_LL332: if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
_tmp5E1->fenv)->r,_tmp5E5->type)){void*_tmp5E6=rval;enum Cyc_CfFlowInfo_InitLevel
_tmp5EA;_LL336: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp5E7=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp5E6;if(_tmp5E7->tag != 7)goto _LL338;}_LL337: goto _LL339;_LL338: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp5E8=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp5E6;if(_tmp5E8->tag != 0)goto
_LL33A;}_LL339: goto _LL33B;_LL33A: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5E9=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5E6;if(_tmp5E9->tag != 3)goto _LL33C;
else{_tmp5EA=_tmp5E9->f1;if(_tmp5EA != Cyc_CfFlowInfo_NoneIL)goto _LL33C;}}_LL33B:
goto _LL335;_LL33C:;_LL33D:{const char*_tmp94A;void*_tmp949[1];struct Cyc_String_pa_struct
_tmp948;(_tmp948.tag=0,((_tmp948.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp5E5->name)),((_tmp949[0]=& _tmp948,Cyc_Tcutil_warn(
_tmp5E0,((_tmp94A="unique pointers reachable from %s may become unreachable",
_tag_dyneither(_tmp94A,sizeof(char),57))),_tag_dyneither(_tmp949,sizeof(void*),1)))))));}
goto _LL335;_LL335:;}goto _LL330;_LL333:;_LL334: goto _LL330;_LL330:;};}static void
Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo flow){union Cyc_CfFlowInfo_FlowInfo _tmp5EE=flow;
int _tmp5EF;struct _tuple12 _tmp5F0;struct Cyc_Dict_Dict _tmp5F1;_LL33F: if((_tmp5EE.BottomFL).tag
!= 1)goto _LL341;_tmp5EF=(int)(_tmp5EE.BottomFL).val;_LL340: return;_LL341: if((
_tmp5EE.ReachableFL).tag != 2)goto _LL33E;_tmp5F0=(struct _tuple12)(_tmp5EE.ReachableFL).val;
_tmp5F1=_tmp5F0.f1;_LL342:{struct Cyc_List_List*_tmp5F2=env->param_roots;for(0;
_tmp5F2 != 0;_tmp5F2=_tmp5F2->tl){if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp5F1,
Cyc_CfFlowInfo_lookup_place(_tmp5F1,(struct Cyc_CfFlowInfo_Place*)_tmp5F2->hd))!= 
Cyc_CfFlowInfo_AllIL){const char*_tmp94D;void*_tmp94C;(_tmp94C=0,Cyc_Tcutil_terr(
loc,((_tmp94D="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp94D,sizeof(char),76))),_tag_dyneither(_tmp94C,sizeof(void*),0)));}}}
if(Cyc_NewControlFlow_warn_lose_unique){struct _tuple22 _tmp94E;struct _tuple22
_tmp5F5=(_tmp94E.f1=loc,((_tmp94E.f2=env,((_tmp94E.f3=_tmp5F1,_tmp94E)))));((
void(*)(void(*f)(struct _tuple22*,void*,void*),struct _tuple22*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp5F5,_tmp5F1);}
return;_LL33E:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp5F7=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp5F9;struct Cyc_Core_Opt*_tmp5FA;
struct Cyc_Absyn_Exp*_tmp5FB;struct Cyc_Absyn_Stmt*_tmp5FC;struct Cyc_Position_Segment*
_tmp5FD;struct Cyc_Absyn_Switch_clause*_tmp5F8=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp5F9=*_tmp5F8;_tmp5FA=_tmp5F9.pat_vars;_tmp5FB=_tmp5F9.where_clause;
_tmp5FC=_tmp5F9.body;_tmp5FD=_tmp5F9.loc;{union Cyc_CfFlowInfo_FlowInfo
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp5F7,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp5FA))->v,_tmp5F7->unknown_all,_tmp5FD);union
Cyc_CfFlowInfo_FlowInfo clause_outflow;if(_tmp5FB != 0){struct Cyc_Absyn_Exp*wexp=(
struct Cyc_Absyn_Exp*)_tmp5FB;union Cyc_CfFlowInfo_FlowInfo _tmp5FF;union Cyc_CfFlowInfo_FlowInfo
_tmp600;struct _tuple15 _tmp5FE=Cyc_NewControlFlow_anal_test(env,clause_inflow,
wexp);_tmp5FF=_tmp5FE.f1;_tmp600=_tmp5FE.f2;inflow=_tmp600;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp5FF,_tmp5FC);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp5FC);}{union Cyc_CfFlowInfo_FlowInfo _tmp601=clause_outflow;int
_tmp602;_LL344: if((_tmp601.BottomFL).tag != 1)goto _LL346;_tmp602=(int)(_tmp601.BottomFL).val;
_LL345: goto _LL343;_LL346:;_LL347: if(scs->tl == 0)return clause_outflow;else{if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){const char*
_tmp951;void*_tmp950;(_tmp950=0,Cyc_Tcutil_terr(_tmp5FC->loc,((_tmp951="switch clause may implicitly fallthru",
_tag_dyneither(_tmp951,sizeof(char),38))),_tag_dyneither(_tmp950,sizeof(void*),0)));}
else{const char*_tmp954;void*_tmp953;(_tmp953=0,Cyc_Tcutil_warn(_tmp5FC->loc,((
_tmp954="switch clause may implicitly fallthru",_tag_dyneither(_tmp954,sizeof(
char),38))),_tag_dyneither(_tmp953,sizeof(void*),0)));}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL343;_LL343:;};};}return Cyc_CfFlowInfo_BottomFL();}struct
_tuple23{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;struct Cyc_Position_Segment*
f3;};static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple23*vdenv,
struct Cyc_Absyn_Vardecl*vd){struct _tuple23 _tmp608;struct Cyc_NewControlFlow_AnalEnv*
_tmp609;struct Cyc_Dict_Dict _tmp60A;struct Cyc_Position_Segment*_tmp60B;struct
_tuple23*_tmp607=vdenv;_tmp608=*_tmp607;_tmp609=_tmp608.f1;_tmp60A=_tmp608.f2;
_tmp60B=_tmp608.f3;if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp609->fenv)->r,
vd->type)){struct Cyc_CfFlowInfo_VarRoot_struct _tmp957;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp956;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp60C=(_tmp956=_region_malloc((
_tmp609->fenv)->r,sizeof(*_tmp956)),((_tmp956[0]=((_tmp957.tag=0,((_tmp957.f1=vd,
_tmp957)))),_tmp956)));void*_tmp60D=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
_tmp60A,(void*)_tmp60C);void*_tmp60E=_tmp60D;enum Cyc_CfFlowInfo_InitLevel
_tmp612;_LL349: {struct Cyc_CfFlowInfo_Consumed_struct*_tmp60F=(struct Cyc_CfFlowInfo_Consumed_struct*)
_tmp60E;if(_tmp60F->tag != 7)goto _LL34B;}_LL34A: goto _LL34C;_LL34B: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp610=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp60E;if(_tmp610->tag != 0)goto
_LL34D;}_LL34C: goto _LL34E;_LL34D: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp611=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp60E;if(_tmp611->tag != 3)goto _LL34F;
else{_tmp612=_tmp611->f1;if(_tmp612 != Cyc_CfFlowInfo_NoneIL)goto _LL34F;}}_LL34E:
goto _LL348;_LL34F:;_LL350:{const char*_tmp95B;void*_tmp95A[1];struct Cyc_String_pa_struct
_tmp959;(_tmp959.tag=0,((_tmp959.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp95A[0]=& _tmp959,Cyc_Tcutil_warn(_tmp60B,((
_tmp95B="unique pointers may still exist after variable %s goes out of scope",
_tag_dyneither(_tmp95B,sizeof(char),68))),_tag_dyneither(_tmp95A,sizeof(void*),1)))))));}
goto _LL348;_LL348:;}}static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){{union Cyc_CfFlowInfo_FlowInfo
_tmp618=inflow;struct _tuple12 _tmp619;struct Cyc_Dict_Dict _tmp61A;_LL352: if((
_tmp618.ReachableFL).tag != 2)goto _LL354;_tmp619=(struct _tuple12)(_tmp618.ReachableFL).val;
_tmp61A=_tmp619.f1;_LL353: {struct _tuple23 _tmp95C;struct _tuple23 _tmp61B=(_tmp95C.f1=
env,((_tmp95C.f2=_tmp61A,((_tmp95C.f3=decl->loc,_tmp95C)))));struct Cyc_CfFlowInfo_FlowEnv*
_tmp61C=env->fenv;{void*_tmp61D=decl->r;struct Cyc_Absyn_Vardecl*_tmp61F;struct
Cyc_Core_Opt*_tmp621;struct Cyc_Core_Opt _tmp622;struct Cyc_List_List*_tmp623;
struct Cyc_List_List*_tmp625;_LL357: {struct Cyc_Absyn_Var_d_struct*_tmp61E=(
struct Cyc_Absyn_Var_d_struct*)_tmp61D;if(_tmp61E->tag != 0)goto _LL359;else{
_tmp61F=_tmp61E->f1;}}_LL358: Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp61B,
_tmp61F);goto _LL356;_LL359: {struct Cyc_Absyn_Let_d_struct*_tmp620=(struct Cyc_Absyn_Let_d_struct*)
_tmp61D;if(_tmp620->tag != 2)goto _LL35B;else{_tmp621=_tmp620->f2;if(_tmp621 == 0)
goto _LL35B;_tmp622=*_tmp621;_tmp623=(struct Cyc_List_List*)_tmp622.v;}}_LL35A:
_tmp625=_tmp623;goto _LL35C;_LL35B: {struct Cyc_Absyn_Letv_d_struct*_tmp624=(
struct Cyc_Absyn_Letv_d_struct*)_tmp61D;if(_tmp624->tag != 3)goto _LL35D;else{
_tmp625=_tmp624->f1;}}_LL35C:((void(*)(void(*f)(struct _tuple23*,struct Cyc_Absyn_Vardecl*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,&
_tmp61B,_tmp625);goto _LL356;_LL35D:;_LL35E: goto _LL356;_LL356:;}goto _LL351;}
_LL354:;_LL355: goto _LL351;_LL351:;}return;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp628;union Cyc_CfFlowInfo_FlowInfo*_tmp629;struct _tuple16 _tmp627=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp628=_tmp627.f1;_tmp629=_tmp627.f2;inflow=*_tmp629;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp62A=env->fenv;void*_tmp62B=s->r;struct Cyc_Absyn_Exp*_tmp62E;struct Cyc_Absyn_Exp*
_tmp630;struct Cyc_Absyn_Exp*_tmp632;struct Cyc_Absyn_Stmt*_tmp634;struct Cyc_Absyn_Stmt*
_tmp635;struct Cyc_Absyn_Exp*_tmp637;struct Cyc_Absyn_Stmt*_tmp638;struct Cyc_Absyn_Stmt*
_tmp639;struct _tuple9 _tmp63B;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_Absyn_Stmt*
_tmp63D;struct Cyc_Absyn_Stmt*_tmp63E;struct Cyc_Absyn_Stmt*_tmp640;struct _tuple9
_tmp641;struct Cyc_Absyn_Exp*_tmp642;struct Cyc_Absyn_Stmt*_tmp643;struct Cyc_Absyn_Exp*
_tmp645;struct _tuple9 _tmp646;struct Cyc_Absyn_Exp*_tmp647;struct Cyc_Absyn_Stmt*
_tmp648;struct _tuple9 _tmp649;struct Cyc_Absyn_Exp*_tmp64A;struct Cyc_Absyn_Stmt*
_tmp64B;struct Cyc_Absyn_Stmt*_tmp64C;struct Cyc_Absyn_Stmt*_tmp64E;struct Cyc_List_List*
_tmp650;struct Cyc_Absyn_Switch_clause**_tmp651;struct Cyc_Absyn_Switch_clause*
_tmp652;struct Cyc_Absyn_Stmt*_tmp654;struct Cyc_Absyn_Stmt*_tmp656;struct Cyc_Absyn_Stmt*
_tmp658;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_List_List*_tmp65B;struct Cyc_Absyn_Stmt*
_tmp65D;struct Cyc_List_List*_tmp65E;struct Cyc_Absyn_Decl*_tmp660;struct Cyc_Absyn_Decl
_tmp661;void*_tmp662;struct Cyc_Absyn_Exp*_tmp664;struct Cyc_Absyn_Tvar*_tmp665;
struct Cyc_Absyn_Vardecl*_tmp666;struct Cyc_Absyn_Stmt*_tmp667;struct Cyc_Absyn_Decl*
_tmp669;struct Cyc_Absyn_Stmt*_tmp66A;struct Cyc_Absyn_Stmt*_tmp66C;struct Cyc_Absyn_Exp*
_tmp66E;_LL360: {struct Cyc_Absyn_Skip_s_struct*_tmp62C=(struct Cyc_Absyn_Skip_s_struct*)
_tmp62B;if(_tmp62C->tag != 0)goto _LL362;}_LL361: return inflow;_LL362: {struct Cyc_Absyn_Return_s_struct*
_tmp62D=(struct Cyc_Absyn_Return_s_struct*)_tmp62B;if(_tmp62D->tag != 3)goto _LL364;
else{_tmp62E=_tmp62D->f1;if(_tmp62E != 0)goto _LL364;}}_LL363: if(env->noreturn){
const char*_tmp95F;void*_tmp95E;(_tmp95E=0,Cyc_Tcutil_terr(s->loc,((_tmp95F="`noreturn' function might return",
_tag_dyneither(_tmp95F,sizeof(char),33))),_tag_dyneither(_tmp95E,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL364: {struct Cyc_Absyn_Return_s_struct*_tmp62F=(struct Cyc_Absyn_Return_s_struct*)
_tmp62B;if(_tmp62F->tag != 3)goto _LL366;else{_tmp630=_tmp62F->f1;}}_LL365: if(env->noreturn){
const char*_tmp962;void*_tmp961;(_tmp961=0,Cyc_Tcutil_terr(s->loc,((_tmp962="`noreturn' function might return",
_tag_dyneither(_tmp962,sizeof(char),33))),_tag_dyneither(_tmp961,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp674;void*_tmp675;struct _tuple13 _tmp673=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp630));_tmp674=_tmp673.f1;
_tmp675=_tmp673.f2;_tmp674=Cyc_NewControlFlow_use_Rval(env,_tmp630->loc,_tmp674,
_tmp675);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp674);return Cyc_CfFlowInfo_BottomFL();};
_LL366: {struct Cyc_Absyn_Exp_s_struct*_tmp631=(struct Cyc_Absyn_Exp_s_struct*)
_tmp62B;if(_tmp631->tag != 1)goto _LL368;else{_tmp632=_tmp631->f1;}}_LL367: return(
Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp632)).f1;_LL368: {struct Cyc_Absyn_Seq_s_struct*
_tmp633=(struct Cyc_Absyn_Seq_s_struct*)_tmp62B;if(_tmp633->tag != 2)goto _LL36A;
else{_tmp634=_tmp633->f1;_tmp635=_tmp633->f2;}}_LL369: return Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp634),_tmp635);_LL36A: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp636=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp62B;if(_tmp636->tag != 4)goto
_LL36C;else{_tmp637=_tmp636->f1;_tmp638=_tmp636->f2;_tmp639=_tmp636->f3;}}_LL36B: {
union Cyc_CfFlowInfo_FlowInfo _tmp677;union Cyc_CfFlowInfo_FlowInfo _tmp678;struct
_tuple15 _tmp676=Cyc_NewControlFlow_anal_test(env,inflow,_tmp637);_tmp677=_tmp676.f1;
_tmp678=_tmp676.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp679=Cyc_NewControlFlow_anal_stmt(
env,_tmp678,_tmp639);union Cyc_CfFlowInfo_FlowInfo _tmp67A=Cyc_NewControlFlow_anal_stmt(
env,_tmp677,_tmp638);return Cyc_CfFlowInfo_join_flow(_tmp62A,env->all_changed,
_tmp67A,_tmp679);};}_LL36C: {struct Cyc_Absyn_While_s_struct*_tmp63A=(struct Cyc_Absyn_While_s_struct*)
_tmp62B;if(_tmp63A->tag != 5)goto _LL36E;else{_tmp63B=_tmp63A->f1;_tmp63C=_tmp63B.f1;
_tmp63D=_tmp63B.f2;_tmp63E=_tmp63A->f2;}}_LL36D: {union Cyc_CfFlowInfo_FlowInfo*
_tmp67C;struct _tuple16 _tmp67B=Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp63D);_tmp67C=_tmp67B.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp67D=*_tmp67C;union
Cyc_CfFlowInfo_FlowInfo _tmp67F;union Cyc_CfFlowInfo_FlowInfo _tmp680;struct
_tuple15 _tmp67E=Cyc_NewControlFlow_anal_test(env,_tmp67D,_tmp63C);_tmp67F=
_tmp67E.f1;_tmp680=_tmp67E.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp681=Cyc_NewControlFlow_anal_stmt(
env,_tmp67F,_tmp63E);Cyc_NewControlFlow_update_flow(env,_tmp63D,_tmp681);return
_tmp680;};};}_LL36E: {struct Cyc_Absyn_Do_s_struct*_tmp63F=(struct Cyc_Absyn_Do_s_struct*)
_tmp62B;if(_tmp63F->tag != 14)goto _LL370;else{_tmp640=_tmp63F->f1;_tmp641=_tmp63F->f2;
_tmp642=_tmp641.f1;_tmp643=_tmp641.f2;}}_LL36F: {union Cyc_CfFlowInfo_FlowInfo
_tmp682=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp640);union Cyc_CfFlowInfo_FlowInfo*
_tmp684;struct _tuple16 _tmp683=Cyc_NewControlFlow_pre_stmt_check(env,_tmp682,
_tmp643);_tmp684=_tmp683.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp685=*_tmp684;union
Cyc_CfFlowInfo_FlowInfo _tmp687;union Cyc_CfFlowInfo_FlowInfo _tmp688;struct
_tuple15 _tmp686=Cyc_NewControlFlow_anal_test(env,_tmp685,_tmp642);_tmp687=
_tmp686.f1;_tmp688=_tmp686.f2;Cyc_NewControlFlow_update_flow(env,_tmp640,_tmp687);
return _tmp688;};}_LL370: {struct Cyc_Absyn_For_s_struct*_tmp644=(struct Cyc_Absyn_For_s_struct*)
_tmp62B;if(_tmp644->tag != 9)goto _LL372;else{_tmp645=_tmp644->f1;_tmp646=_tmp644->f2;
_tmp647=_tmp646.f1;_tmp648=_tmp646.f2;_tmp649=_tmp644->f3;_tmp64A=_tmp649.f1;
_tmp64B=_tmp649.f2;_tmp64C=_tmp644->f4;}}_LL371: {union Cyc_CfFlowInfo_FlowInfo
_tmp689=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp645)).f1;union Cyc_CfFlowInfo_FlowInfo*
_tmp68B;struct _tuple16 _tmp68A=Cyc_NewControlFlow_pre_stmt_check(env,_tmp689,
_tmp648);_tmp68B=_tmp68A.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp68C=*_tmp68B;union
Cyc_CfFlowInfo_FlowInfo _tmp68E;union Cyc_CfFlowInfo_FlowInfo _tmp68F;struct
_tuple15 _tmp68D=Cyc_NewControlFlow_anal_test(env,_tmp68C,_tmp647);_tmp68E=
_tmp68D.f1;_tmp68F=_tmp68D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp690=Cyc_NewControlFlow_anal_stmt(
env,_tmp68E,_tmp64C);union Cyc_CfFlowInfo_FlowInfo*_tmp692;struct _tuple16 _tmp691=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp690,_tmp64B);_tmp692=_tmp691.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp693=*_tmp692;union Cyc_CfFlowInfo_FlowInfo _tmp694=(Cyc_NewControlFlow_anal_Rexp(
env,0,_tmp693,_tmp64A)).f1;Cyc_NewControlFlow_update_flow(env,_tmp648,_tmp694);
return _tmp68F;};};};}_LL372: {struct Cyc_Absyn_Break_s_struct*_tmp64D=(struct Cyc_Absyn_Break_s_struct*)
_tmp62B;if(_tmp64D->tag != 6)goto _LL374;else{_tmp64E=_tmp64D->f1;if(_tmp64E != 0)
goto _LL374;}}_LL373: return Cyc_CfFlowInfo_BottomFL();_LL374: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp64F=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp62B;if(_tmp64F->tag != 11)goto
_LL376;else{_tmp650=_tmp64F->f1;_tmp651=_tmp64F->f2;if(_tmp651 == 0)goto _LL376;
_tmp652=*_tmp651;}}_LL375: {struct _RegionHandle*_tmp695=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp697;struct Cyc_List_List*_tmp698;struct _tuple19 _tmp696=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp695,env,inflow,_tmp650,1,1);_tmp697=_tmp696.f1;_tmp698=_tmp696.f2;for(0;
_tmp698 != 0;(_tmp698=_tmp698->tl,_tmp650=_tmp650->tl)){_tmp697=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp650))->hd)->loc,
_tmp697,(void*)_tmp698->hd);}_tmp697=Cyc_NewControlFlow_add_vars(_tmp62A,_tmp697,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp652->pat_vars))->v,
_tmp62A->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp652->body,_tmp697);return Cyc_CfFlowInfo_BottomFL();}_LL376: {struct Cyc_Absyn_Break_s_struct*
_tmp653=(struct Cyc_Absyn_Break_s_struct*)_tmp62B;if(_tmp653->tag != 6)goto _LL378;
else{_tmp654=_tmp653->f1;}}_LL377: _tmp656=_tmp654;goto _LL379;_LL378: {struct Cyc_Absyn_Continue_s_struct*
_tmp655=(struct Cyc_Absyn_Continue_s_struct*)_tmp62B;if(_tmp655->tag != 7)goto
_LL37A;else{_tmp656=_tmp655->f1;}}_LL379: _tmp658=_tmp656;goto _LL37B;_LL37A: {
struct Cyc_Absyn_Goto_s_struct*_tmp657=(struct Cyc_Absyn_Goto_s_struct*)_tmp62B;
if(_tmp657->tag != 8)goto _LL37C;else{_tmp658=_tmp657->f2;}}_LL37B: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp699=_tmp628->encloser;struct Cyc_Absyn_Stmt*
_tmp69A=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp658)))->encloser;while(_tmp69A != _tmp699){struct Cyc_Absyn_Stmt*_tmp69B=(Cyc_NewControlFlow_get_stmt_annot(
_tmp699))->encloser;if(_tmp69B == _tmp699){{const char*_tmp965;void*_tmp964;(
_tmp964=0,Cyc_Tcutil_terr(s->loc,((_tmp965="goto enters local scope or exception handler",
_tag_dyneither(_tmp965,sizeof(char),45))),_tag_dyneither(_tmp964,sizeof(void*),0)));}
break;}_tmp699=_tmp69B;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp658),inflow);return Cyc_CfFlowInfo_BottomFL();_LL37C: {struct Cyc_Absyn_Switch_s_struct*
_tmp659=(struct Cyc_Absyn_Switch_s_struct*)_tmp62B;if(_tmp659->tag != 10)goto
_LL37E;else{_tmp65A=_tmp659->f1;_tmp65B=_tmp659->f2;}}_LL37D: {union Cyc_CfFlowInfo_FlowInfo
_tmp69F;void*_tmp6A0;struct _tuple13 _tmp69E=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,_tmp65A);_tmp69F=_tmp69E.f1;_tmp6A0=_tmp69E.f2;_tmp69F=Cyc_NewControlFlow_use_Rval(
env,_tmp65A->loc,_tmp69F,_tmp6A0);return Cyc_NewControlFlow_anal_scs(env,_tmp69F,
_tmp65B);}_LL37E: {struct Cyc_Absyn_TryCatch_s_struct*_tmp65C=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp62B;if(_tmp65C->tag != 15)goto _LL380;else{_tmp65D=_tmp65C->f1;_tmp65E=_tmp65C->f2;}}
_LL37F: {int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp65D);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp65E);{union Cyc_CfFlowInfo_FlowInfo _tmp6A1=scs_outflow;int
_tmp6A2;_LL38B: if((_tmp6A1.BottomFL).tag != 1)goto _LL38D;_tmp6A2=(int)(_tmp6A1.BottomFL).val;
_LL38C: goto _LL38A;_LL38D:;_LL38E: {const char*_tmp968;void*_tmp967;(_tmp967=0,Cyc_Tcutil_terr(
s->loc,((_tmp968="last catch clause may implicitly fallthru",_tag_dyneither(
_tmp968,sizeof(char),42))),_tag_dyneither(_tmp967,sizeof(void*),0)));}_LL38A:;}
outflow=s1_outflow;return outflow;};};}_LL380: {struct Cyc_Absyn_Decl_s_struct*
_tmp65F=(struct Cyc_Absyn_Decl_s_struct*)_tmp62B;if(_tmp65F->tag != 12)goto _LL382;
else{_tmp660=_tmp65F->f1;_tmp661=*_tmp660;_tmp662=_tmp661.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp663=(struct Cyc_Absyn_Alias_d_struct*)_tmp662;if(_tmp663->tag != 5)goto _LL382;
else{_tmp664=_tmp663->f1;_tmp665=_tmp663->f2;_tmp666=_tmp663->f3;}};_tmp667=
_tmp65F->f2;}}_LL381: {union Cyc_CfFlowInfo_FlowInfo _tmp6A6;union Cyc_CfFlowInfo_AbsLVal
_tmp6A7;struct _tuple14 _tmp6A5=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp664);
_tmp6A6=_tmp6A5.f1;_tmp6A7=_tmp6A5.f2;{struct _tuple14 _tmp969;struct _tuple14
_tmp6A9=(_tmp969.f1=_tmp6A6,((_tmp969.f2=_tmp6A7,_tmp969)));union Cyc_CfFlowInfo_FlowInfo
_tmp6AA;struct _tuple12 _tmp6AB;struct Cyc_Dict_Dict _tmp6AC;struct Cyc_List_List*
_tmp6AD;union Cyc_CfFlowInfo_AbsLVal _tmp6AE;struct Cyc_CfFlowInfo_Place*_tmp6AF;
union Cyc_CfFlowInfo_FlowInfo _tmp6B0;int _tmp6B1;_LL390: _tmp6AA=_tmp6A9.f1;if((
_tmp6AA.ReachableFL).tag != 2)goto _LL392;_tmp6AB=(struct _tuple12)(_tmp6AA.ReachableFL).val;
_tmp6AC=_tmp6AB.f1;_tmp6AD=_tmp6AB.f2;_tmp6AE=_tmp6A9.f2;if((_tmp6AE.PlaceL).tag
!= 1)goto _LL392;_tmp6AF=(struct Cyc_CfFlowInfo_Place*)(_tmp6AE.PlaceL).val;_LL391: {
void*_tmp6B2=Cyc_CfFlowInfo_lookup_place(_tmp6AC,_tmp6AF);void*t=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp664->topt))->v;{void*_tmp6B3=_tmp664->r;struct Cyc_Absyn_Exp*
_tmp6B5;_LL397: {struct Cyc_Absyn_Cast_e_struct*_tmp6B4=(struct Cyc_Absyn_Cast_e_struct*)
_tmp6B3;if(_tmp6B4->tag != 15)goto _LL399;else{_tmp6B5=_tmp6B4->f2;}}_LL398: t=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6B5->topt))->v;goto _LL396;_LL399:;
_LL39A: goto _LL396;_LL396:;}{void*_tmp6B6=Cyc_CfFlowInfo_make_unique_consumed(
_tmp62A,t,_tmp664,env->iteration_num,_tmp6B2);_tmp6AC=Cyc_CfFlowInfo_assign_place(
_tmp62A,_tmp664->loc,_tmp6AC,env->all_changed,_tmp6AF,_tmp6B6);_tmp6A6=Cyc_CfFlowInfo_ReachableFL(
_tmp6AC,_tmp6AD);{void*leaf;switch(Cyc_CfFlowInfo_initlevel(_tmp62A,_tmp6AC,
_tmp6B2)){case Cyc_CfFlowInfo_AllIL: _LL39B: leaf=_tmp62A->unknown_all;break;case
Cyc_CfFlowInfo_NoneIL: _LL39C: leaf=_tmp62A->unknown_none;break;case Cyc_CfFlowInfo_ThisIL:
_LL39D: leaf=_tmp62A->unknown_this;break;}{void*_tmp6B7=Cyc_CfFlowInfo_typ_to_absrval(
_tmp62A,t,0,leaf);_tmp6A6=Cyc_NewControlFlow_use_Rval(env,_tmp664->loc,_tmp6A6,
_tmp6B2);{struct Cyc_List_List _tmp96A;struct Cyc_List_List _tmp6B8=(_tmp96A.hd=
_tmp666,((_tmp96A.tl=0,_tmp96A)));_tmp6A6=Cyc_NewControlFlow_add_vars(_tmp62A,
_tmp6A6,(struct Cyc_List_List*)& _tmp6B8,_tmp62A->unknown_all,s->loc);_tmp6A6=Cyc_NewControlFlow_anal_stmt(
env,_tmp6A6,_tmp667);{union Cyc_CfFlowInfo_FlowInfo _tmp6B9=_tmp6A6;struct _tuple12
_tmp6BA;struct Cyc_Dict_Dict _tmp6BB;struct Cyc_List_List*_tmp6BC;_LL3A0: if((
_tmp6B9.ReachableFL).tag != 2)goto _LL3A2;_tmp6BA=(struct _tuple12)(_tmp6B9.ReachableFL).val;
_tmp6BB=_tmp6BA.f1;_tmp6BC=_tmp6BA.f2;_LL3A1: _tmp6BB=Cyc_CfFlowInfo_assign_place(
_tmp62A,s->loc,_tmp6BB,env->all_changed,_tmp6AF,_tmp6B7);outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp6BB,_tmp6BC);return outflow;_LL3A2:;_LL3A3: return _tmp6A6;_LL39F:;};};};};};}
_LL392: _tmp6B0=_tmp6A9.f1;if((_tmp6B0.BottomFL).tag != 1)goto _LL394;_tmp6B1=(int)(
_tmp6B0.BottomFL).val;_LL393: return _tmp6A6;_LL394:;_LL395:{const char*_tmp96D;
void*_tmp96C;(_tmp96C=0,Cyc_Tcutil_terr(_tmp664->loc,((_tmp96D="bad alias expression--no unique path found",
_tag_dyneither(_tmp96D,sizeof(char),43))),_tag_dyneither(_tmp96C,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();_LL38F:;};}_LL382: {struct Cyc_Absyn_Decl_s_struct*
_tmp668=(struct Cyc_Absyn_Decl_s_struct*)_tmp62B;if(_tmp668->tag != 12)goto _LL384;
else{_tmp669=_tmp668->f1;_tmp66A=_tmp668->f2;}}_LL383: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp669),_tmp66A);if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp669);return outflow;
_LL384: {struct Cyc_Absyn_Label_s_struct*_tmp66B=(struct Cyc_Absyn_Label_s_struct*)
_tmp62B;if(_tmp66B->tag != 13)goto _LL386;else{_tmp66C=_tmp66B->f2;}}_LL385: return
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp66C);_LL386: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp66D=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp62B;if(_tmp66D->tag != 16)goto
_LL388;else{_tmp66E=_tmp66D->f1;}}_LL387: {union Cyc_CfFlowInfo_FlowInfo _tmp6C1;
void*_tmp6C2;struct _tuple13 _tmp6C0=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,
_tmp66E);_tmp6C1=_tmp6C0.f1;_tmp6C2=_tmp6C0.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp6C3=Cyc_NewControlFlow_use_Rval(env,_tmp66E->loc,_tmp6C1,_tmp6C2);void*
_tmp6C4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp66E->topt))->v);
void*_tmp6C6;_LL3A5: {struct Cyc_Absyn_RgnHandleType_struct*_tmp6C5=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6C4;if(_tmp6C5->tag != 16)goto _LL3A7;else{_tmp6C6=(void*)_tmp6C5->f1;}}_LL3A6:
return Cyc_CfFlowInfo_kill_flow_region(_tmp62A,_tmp6C1,_tmp6C6);_LL3A7:;_LL3A8: {
struct Cyc_Core_Impossible_struct _tmp973;const char*_tmp972;struct Cyc_Core_Impossible_struct*
_tmp971;(int)_throw((void*)((_tmp971=_cycalloc(sizeof(*_tmp971)),((_tmp971[0]=((
_tmp973.tag=Cyc_Core_Impossible,((_tmp973.f1=((_tmp972="anal_stmt -- reset_region",
_tag_dyneither(_tmp972,sizeof(char),26))),_tmp973)))),_tmp971)))));}_LL3A4:;};}
_LL388:;_LL389: {struct Cyc_Core_Impossible_struct _tmp979;const char*_tmp978;
struct Cyc_Core_Impossible_struct*_tmp977;(int)_throw((void*)((_tmp977=_cycalloc(
sizeof(*_tmp977)),((_tmp977[0]=((_tmp979.tag=Cyc_Core_Impossible,((_tmp979.f1=((
_tmp978="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(
_tmp978,sizeof(char),56))),_tmp979)))),_tmp977)))));}_LL35F:;};}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*
_tmp6CD=env->fenv;void*_tmp6CE=decl->r;struct Cyc_Absyn_Vardecl*_tmp6D0;struct Cyc_Core_Opt*
_tmp6D2;struct Cyc_Core_Opt _tmp6D3;struct Cyc_List_List*_tmp6D4;struct Cyc_Absyn_Exp*
_tmp6D5;struct Cyc_List_List*_tmp6D7;struct Cyc_Absyn_Fndecl*_tmp6D9;struct Cyc_Absyn_Tvar*
_tmp6DB;struct Cyc_Absyn_Vardecl*_tmp6DC;int _tmp6DD;struct Cyc_Absyn_Exp*_tmp6DE;
_LL3AA: {struct Cyc_Absyn_Var_d_struct*_tmp6CF=(struct Cyc_Absyn_Var_d_struct*)
_tmp6CE;if(_tmp6CF->tag != 0)goto _LL3AC;else{_tmp6D0=_tmp6CF->f1;}}_LL3AB: {
struct Cyc_List_List _tmp97A;struct Cyc_List_List _tmp6DF=(_tmp97A.hd=_tmp6D0,((
_tmp97A.tl=0,_tmp97A)));inflow=Cyc_NewControlFlow_add_vars(_tmp6CD,inflow,(
struct Cyc_List_List*)& _tmp6DF,_tmp6CD->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*
_tmp6E0=_tmp6D0->initializer;if(_tmp6E0 == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo
_tmp6E2;void*_tmp6E3;struct _tuple13 _tmp6E1=Cyc_NewControlFlow_anal_Rexp(env,1,
inflow,(struct Cyc_Absyn_Exp*)_tmp6E0);_tmp6E2=_tmp6E1.f1;_tmp6E3=_tmp6E1.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6E4=_tmp6E2;int _tmp6E5;struct _tuple12 _tmp6E6;
struct Cyc_Dict_Dict _tmp6E7;struct Cyc_List_List*_tmp6E8;_LL3B7: if((_tmp6E4.BottomFL).tag
!= 1)goto _LL3B9;_tmp6E5=(int)(_tmp6E4.BottomFL).val;_LL3B8: return Cyc_CfFlowInfo_BottomFL();
_LL3B9: if((_tmp6E4.ReachableFL).tag != 2)goto _LL3B6;_tmp6E6=(struct _tuple12)(
_tmp6E4.ReachableFL).val;_tmp6E7=_tmp6E6.f1;_tmp6E8=_tmp6E6.f2;_LL3BA:{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp980;struct Cyc_CfFlowInfo_VarRoot_struct _tmp97F;struct Cyc_CfFlowInfo_Place*
_tmp97E;_tmp6E7=Cyc_CfFlowInfo_assign_place(_tmp6CD,decl->loc,_tmp6E7,env->all_changed,((
_tmp97E=_region_malloc(env->r,sizeof(*_tmp97E)),((_tmp97E->root=(void*)((_tmp980=
_region_malloc(env->r,sizeof(*_tmp980)),((_tmp980[0]=((_tmp97F.tag=0,((_tmp97F.f1=
_tmp6D0,_tmp97F)))),_tmp980)))),((_tmp97E->fields=0,_tmp97E)))))),_tmp6E3);}
_tmp6E8=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp6E8,_tmp6D0,(struct Cyc_Absyn_Exp*)
_check_null(_tmp6D0->initializer));{union Cyc_CfFlowInfo_FlowInfo _tmp6EC=Cyc_CfFlowInfo_ReachableFL(
_tmp6E7,_tmp6E8);Cyc_NewControlFlow_update_tryflow(env,_tmp6EC);return _tmp6EC;};
_LL3B6:;};};};}_LL3AC: {struct Cyc_Absyn_Let_d_struct*_tmp6D1=(struct Cyc_Absyn_Let_d_struct*)
_tmp6CE;if(_tmp6D1->tag != 2)goto _LL3AE;else{_tmp6D2=_tmp6D1->f2;if(_tmp6D2 == 0)
goto _LL3AE;_tmp6D3=*_tmp6D2;_tmp6D4=(struct Cyc_List_List*)_tmp6D3.v;_tmp6D5=
_tmp6D1->f3;}}_LL3AD: {union Cyc_CfFlowInfo_FlowInfo _tmp6EF;void*_tmp6F0;struct
_tuple13 _tmp6EE=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6D5);_tmp6EF=
_tmp6EE.f1;_tmp6F0=_tmp6EE.f2;_tmp6EF=Cyc_NewControlFlow_use_Rval(env,_tmp6D5->loc,
_tmp6EF,_tmp6F0);return Cyc_NewControlFlow_add_vars(_tmp6CD,_tmp6EF,_tmp6D4,
_tmp6CD->unknown_all,decl->loc);}_LL3AE: {struct Cyc_Absyn_Letv_d_struct*_tmp6D6=(
struct Cyc_Absyn_Letv_d_struct*)_tmp6CE;if(_tmp6D6->tag != 3)goto _LL3B0;else{
_tmp6D7=_tmp6D6->f1;}}_LL3AF: return Cyc_NewControlFlow_add_vars(_tmp6CD,inflow,
_tmp6D7,_tmp6CD->unknown_none,decl->loc);_LL3B0: {struct Cyc_Absyn_Fn_d_struct*
_tmp6D8=(struct Cyc_Absyn_Fn_d_struct*)_tmp6CE;if(_tmp6D8->tag != 1)goto _LL3B2;
else{_tmp6D9=_tmp6D8->f1;}}_LL3B1: Cyc_NewControlFlow_check_nested_fun(_tmp6CD,
inflow,_tmp6D9);{void*_tmp6F1=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp6D9->cached_typ))->v;
struct Cyc_Absyn_Vardecl*_tmp6F2=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp6D9->fn_vardecl);
struct Cyc_List_List*_tmp981;return Cyc_NewControlFlow_add_vars(_tmp6CD,inflow,((
_tmp981=_region_malloc(env->r,sizeof(*_tmp981)),((_tmp981->hd=_tmp6F2,((_tmp981->tl=
0,_tmp981)))))),_tmp6CD->unknown_all,decl->loc);};_LL3B2: {struct Cyc_Absyn_Region_d_struct*
_tmp6DA=(struct Cyc_Absyn_Region_d_struct*)_tmp6CE;if(_tmp6DA->tag != 4)goto _LL3B4;
else{_tmp6DB=_tmp6DA->f1;_tmp6DC=_tmp6DA->f2;_tmp6DD=_tmp6DA->f3;_tmp6DE=_tmp6DA->f4;}}
_LL3B3: if(_tmp6DE != 0){struct Cyc_Absyn_Exp*_tmp6F4=(struct Cyc_Absyn_Exp*)_tmp6DE;
union Cyc_CfFlowInfo_FlowInfo _tmp6F6;void*_tmp6F7;struct _tuple13 _tmp6F5=Cyc_NewControlFlow_anal_Rexp(
env,1,inflow,_tmp6F4);_tmp6F6=_tmp6F5.f1;_tmp6F7=_tmp6F5.f2;inflow=Cyc_NewControlFlow_use_Rval(
env,_tmp6F4->loc,_tmp6F6,_tmp6F7);}{struct Cyc_List_List _tmp982;struct Cyc_List_List
_tmp6F8=(_tmp982.hd=_tmp6DC,((_tmp982.tl=0,_tmp982)));return Cyc_NewControlFlow_add_vars(
_tmp6CD,inflow,(struct Cyc_List_List*)& _tmp6F8,_tmp6CD->unknown_all,decl->loc);};
_LL3B4:;_LL3B5: {struct Cyc_Core_Impossible_struct _tmp988;const char*_tmp987;
struct Cyc_Core_Impossible_struct*_tmp986;(int)_throw((void*)((_tmp986=_cycalloc(
sizeof(*_tmp986)),((_tmp986[0]=((_tmp988.tag=Cyc_Core_Impossible,((_tmp988.f1=((
_tmp987="anal_decl: unexpected decl variant",_tag_dyneither(_tmp987,sizeof(char),
35))),_tmp988)))),_tmp986)))));}_LL3A9:;}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){struct _RegionHandle _tmp6FD=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp6FD;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*
fenv=Cyc_CfFlowInfo_new_flow_env(frgn);Cyc_NewControlFlow_check_nested_fun(fenv,
Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);};_pop_region(frgn);}static
int Cyc_NewControlFlow_hash_ptr(void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle*_tmp6FE=fenv->r;struct Cyc_Position_Segment*_tmp6FF=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp6FF);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp700=inflow;int _tmp701;struct
_tuple12 _tmp702;struct Cyc_Dict_Dict _tmp703;struct Cyc_List_List*_tmp704;_LL3BC:
if((_tmp700.BottomFL).tag != 1)goto _LL3BE;_tmp701=(int)(_tmp700.BottomFL).val;
_LL3BD: {const char*_tmp98B;void*_tmp98A;(_tmp98A=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp98B="check_fun",
_tag_dyneither(_tmp98B,sizeof(char),10))),_tag_dyneither(_tmp98A,sizeof(void*),0)));}
_LL3BE: if((_tmp700.ReachableFL).tag != 2)goto _LL3BB;_tmp702=(struct _tuple12)(
_tmp700.ReachableFL).val;_tmp703=_tmp702.f1;_tmp704=_tmp702.f2;_LL3BF: {struct
Cyc_List_List*atts;{void*_tmp707=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp709;struct Cyc_List_List*
_tmp70A;_LL3C1: {struct Cyc_Absyn_FnType_struct*_tmp708=(struct Cyc_Absyn_FnType_struct*)
_tmp707;if(_tmp708->tag != 10)goto _LL3C3;else{_tmp709=_tmp708->f1;_tmp70A=_tmp709.attributes;}}
_LL3C2: atts=_tmp70A;goto _LL3C0;_LL3C3:;_LL3C4: {const char*_tmp98E;void*_tmp98D;(
_tmp98D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp98E="check_fun: non-function type cached with fndecl_t",_tag_dyneither(
_tmp98E,sizeof(char),50))),_tag_dyneither(_tmp98D,sizeof(void*),0)));}_LL3C0:;}
for(0;atts != 0;atts=atts->tl){void*_tmp70D=(void*)atts->hd;int _tmp70F;int _tmp711;
_LL3C6: {struct Cyc_Absyn_Noliveunique_att_struct*_tmp70E=(struct Cyc_Absyn_Noliveunique_att_struct*)
_tmp70D;if(_tmp70E->tag != 21)goto _LL3C8;else{_tmp70F=_tmp70E->f1;}}_LL3C7: {
unsigned int j=(unsigned int)_tmp70F;struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(
- 1,0);if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{const char*_tmp991;
void*_tmp990;(_tmp990=0,Cyc_Tcutil_terr(_tmp6FF,((_tmp991="noliveunique attribute exceeds number of parameters",
_tag_dyneither(_tmp991,sizeof(char),52))),_tag_dyneither(_tmp990,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp714=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));void*t=Cyc_Tcutil_compress(
_tmp714->type);if(!Cyc_Tcutil_is_noalias_pointer(t)){{const char*_tmp994;void*
_tmp993;(_tmp993=0,Cyc_Tcutil_terr(_tmp6FF,((_tmp994="noliveunique attribute requires unique pointer type",
_tag_dyneither(_tmp994,sizeof(char),52))),_tag_dyneither(_tmp993,sizeof(void*),0)));}
continue;}{void*elttype=Cyc_Tcutil_pointer_elt_type(t);void*_tmp717=Cyc_CfFlowInfo_make_unique_consumed(
fenv,elttype,bogus_exp,- 1,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_struct _tmp997;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp996;struct Cyc_CfFlowInfo_InitParam_struct*_tmp718=(_tmp996=_region_malloc(
_tmp6FE,sizeof(*_tmp996)),((_tmp996[0]=((_tmp997.tag=2,((_tmp997.f1=(int)j,((
_tmp997.f2=(void*)t,_tmp997)))))),_tmp996)));struct Cyc_CfFlowInfo_Place*_tmp998;
struct Cyc_CfFlowInfo_Place*_tmp719=(_tmp998=_region_malloc(_tmp6FE,sizeof(*
_tmp998)),((_tmp998->root=(void*)_tmp718,((_tmp998->fields=0,_tmp998)))));
_tmp703=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp703,(void*)_tmp718,_tmp717);{struct Cyc_CfFlowInfo_AddressOf_struct _tmp99E;
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp99D;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp99B;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp99A;_tmp703=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp703,(void*)((_tmp99A=
_region_malloc(_tmp6FE,sizeof(*_tmp99A)),((_tmp99A[0]=((_tmp99B.tag=0,((_tmp99B.f1=
_tmp714,_tmp99B)))),_tmp99A)))),(void*)((_tmp99D=_region_malloc(_tmp6FE,sizeof(*
_tmp99D)),((_tmp99D[0]=((_tmp99E.tag=5,((_tmp99E.f1=_tmp719,_tmp99E)))),_tmp99D)))));}
goto _LL3C5;};};}_LL3C8: {struct Cyc_Absyn_Initializes_att_struct*_tmp710=(struct
Cyc_Absyn_Initializes_att_struct*)_tmp70D;if(_tmp710->tag != 20)goto _LL3CA;else{
_tmp711=_tmp710->f1;}}_LL3C9: {unsigned int j=(unsigned int)_tmp711;if(j > ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){{const char*_tmp9A1;void*_tmp9A0;(_tmp9A0=0,
Cyc_Tcutil_terr(_tmp6FF,((_tmp9A1="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp9A1,sizeof(char),51))),_tag_dyneither(_tmp9A0,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp723=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp724=Cyc_Tcutil_compress(
_tmp723->type);struct Cyc_Absyn_PtrInfo _tmp726;void*_tmp727;struct Cyc_Absyn_PtrAtts
_tmp728;union Cyc_Absyn_Constraint*_tmp729;union Cyc_Absyn_Constraint*_tmp72A;
union Cyc_Absyn_Constraint*_tmp72B;_LL3CD: {struct Cyc_Absyn_PointerType_struct*
_tmp725=(struct Cyc_Absyn_PointerType_struct*)_tmp724;if(_tmp725->tag != 5)goto
_LL3CF;else{_tmp726=_tmp725->f1;_tmp727=_tmp726.elt_typ;_tmp728=_tmp726.ptr_atts;
_tmp729=_tmp728.nullable;_tmp72A=_tmp728.bounds;_tmp72B=_tmp728.zero_term;}}
_LL3CE: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp729)){
const char*_tmp9A4;void*_tmp9A3;(_tmp9A3=0,Cyc_Tcutil_terr(_tmp6FF,((_tmp9A4="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp9A4,sizeof(char),55))),_tag_dyneither(_tmp9A3,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp72A)){const char*_tmp9A7;void*_tmp9A6;(_tmp9A6=0,
Cyc_Tcutil_terr(_tmp6FF,((_tmp9A7="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp9A7,sizeof(char),57))),_tag_dyneither(_tmp9A6,sizeof(void*),0)));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp72B)){
const char*_tmp9AA;void*_tmp9A9;(_tmp9A9=0,Cyc_Tcutil_terr(_tmp6FF,((_tmp9AA="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp9AA,sizeof(char),77))),_tag_dyneither(_tmp9A9,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_struct _tmp9AD;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp9AC;struct Cyc_CfFlowInfo_InitParam_struct*_tmp732=(_tmp9AC=_region_malloc(
_tmp6FE,sizeof(*_tmp9AC)),((_tmp9AC[0]=((_tmp9AD.tag=2,((_tmp9AD.f1=(int)j,((
_tmp9AD.f2=(void*)_tmp727,_tmp9AD)))))),_tmp9AC)));struct Cyc_CfFlowInfo_Place*
_tmp9AE;struct Cyc_CfFlowInfo_Place*_tmp733=(_tmp9AE=_region_malloc(_tmp6FE,
sizeof(*_tmp9AE)),((_tmp9AE->root=(void*)_tmp732,((_tmp9AE->fields=0,_tmp9AE)))));
_tmp703=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp703,(void*)_tmp732,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp727,0,fenv->esc_none));{
struct Cyc_CfFlowInfo_AddressOf_struct _tmp9B4;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp9B3;struct Cyc_CfFlowInfo_VarRoot_struct _tmp9B1;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp9B0;_tmp703=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp703,(void*)((_tmp9B0=_region_malloc(_tmp6FE,sizeof(*_tmp9B0)),((_tmp9B0[0]=((
_tmp9B1.tag=0,((_tmp9B1.f1=_tmp723,_tmp9B1)))),_tmp9B0)))),(void*)((_tmp9B3=
_region_malloc(_tmp6FE,sizeof(*_tmp9B3)),((_tmp9B3[0]=((_tmp9B4.tag=5,((_tmp9B4.f1=
_tmp733,_tmp9B4)))),_tmp9B3)))));}{struct Cyc_List_List*_tmp9B5;param_roots=((
_tmp9B5=_region_malloc(_tmp6FE,sizeof(*_tmp9B5)),((_tmp9B5->hd=_tmp733,((_tmp9B5->tl=
param_roots,_tmp9B5))))));}goto _LL3CC;};_LL3CF:;_LL3D0: {const char*_tmp9B8;void*
_tmp9B7;(_tmp9B7=0,Cyc_Tcutil_terr(_tmp6FF,((_tmp9B8="initializes attribute on non-pointer",
_tag_dyneither(_tmp9B8,sizeof(char),37))),_tag_dyneither(_tmp9B7,sizeof(void*),0)));}
_LL3CC:;}goto _LL3C5;};}_LL3CA:;_LL3CB: goto _LL3C5;_LL3C5:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp703,_tmp704);}_LL3BB:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp6FE,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp9B9;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp9B9=_region_malloc(_tmp6FE,sizeof(*_tmp9B9)),((
_tmp9B9->r=_tmp6FE,((_tmp9B9->fenv=fenv,((_tmp9B9->iterate_again=1,((_tmp9B9->iteration_num=
0,((_tmp9B9->in_try=0,((_tmp9B9->tryflow=inflow,((_tmp9B9->all_changed=0,((
_tmp9B9->noreturn=noreturn,((_tmp9B9->param_roots=param_roots,((_tmp9B9->flow_table=
flow_table,_tmp9B9)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
union Cyc_CfFlowInfo_FlowInfo _tmp73E=outflow;int _tmp73F;_LL3D2: if((_tmp73E.BottomFL).tag
!= 1)goto _LL3D4;_tmp73F=(int)(_tmp73E.BottomFL).val;_LL3D3: goto _LL3D1;_LL3D4:;
_LL3D5: Cyc_NewControlFlow_check_init_params(_tmp6FF,env,outflow);if(noreturn){
const char*_tmp9BC;void*_tmp9BB;(_tmp9BB=0,Cyc_Tcutil_terr(_tmp6FF,((_tmp9BC="`noreturn' function might (implicitly) return",
_tag_dyneither(_tmp9BC,sizeof(char),46))),_tag_dyneither(_tmp9BB,sizeof(void*),0)));}
else{void*_tmp742=Cyc_Tcutil_compress(fd->ret_type);_LL3D7: {struct Cyc_Absyn_VoidType_struct*
_tmp743=(struct Cyc_Absyn_VoidType_struct*)_tmp742;if(_tmp743->tag != 0)goto _LL3D9;}
_LL3D8: goto _LL3D6;_LL3D9: {struct Cyc_Absyn_DoubleType_struct*_tmp744=(struct Cyc_Absyn_DoubleType_struct*)
_tmp742;if(_tmp744->tag != 8)goto _LL3DB;}_LL3DA: goto _LL3DC;_LL3DB: {struct Cyc_Absyn_FloatType_struct*
_tmp745=(struct Cyc_Absyn_FloatType_struct*)_tmp742;if(_tmp745->tag != 7)goto
_LL3DD;}_LL3DC: goto _LL3DE;_LL3DD: {struct Cyc_Absyn_IntType_struct*_tmp746=(
struct Cyc_Absyn_IntType_struct*)_tmp742;if(_tmp746->tag != 6)goto _LL3DF;}_LL3DE:{
const char*_tmp9BF;void*_tmp9BE;(_tmp9BE=0,Cyc_Tcutil_warn(_tmp6FF,((_tmp9BF="function may not return a value",
_tag_dyneither(_tmp9BF,sizeof(char),32))),_tag_dyneither(_tmp9BE,sizeof(void*),0)));}
goto _LL3D6;_LL3DF:;_LL3E0:{const char*_tmp9C2;void*_tmp9C1;(_tmp9C1=0,Cyc_Tcutil_terr(
_tmp6FF,((_tmp9C2="function may not return a value",_tag_dyneither(_tmp9C2,
sizeof(char),32))),_tag_dyneither(_tmp9C1,sizeof(void*),0)));}goto _LL3D6;_LL3D6:;}
goto _LL3D1;_LL3D1:;};};};}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){void*_tmp74C=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct
Cyc_Absyn_Fndecl*_tmp74E;struct Cyc_List_List*_tmp750;struct Cyc_List_List*_tmp752;
struct Cyc_List_List*_tmp754;_LL3E2: {struct Cyc_Absyn_Fn_d_struct*_tmp74D=(struct
Cyc_Absyn_Fn_d_struct*)_tmp74C;if(_tmp74D->tag != 1)goto _LL3E4;else{_tmp74E=
_tmp74D->f1;}}_LL3E3: Cyc_NewControlFlow_check_fun(_tmp74E);goto _LL3E1;_LL3E4: {
struct Cyc_Absyn_ExternC_d_struct*_tmp74F=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp74C;if(_tmp74F->tag != 12)goto _LL3E6;else{_tmp750=_tmp74F->f1;}}_LL3E5:
_tmp752=_tmp750;goto _LL3E7;_LL3E6: {struct Cyc_Absyn_Using_d_struct*_tmp751=(
struct Cyc_Absyn_Using_d_struct*)_tmp74C;if(_tmp751->tag != 11)goto _LL3E8;else{
_tmp752=_tmp751->f2;}}_LL3E7: _tmp754=_tmp752;goto _LL3E9;_LL3E8: {struct Cyc_Absyn_Namespace_d_struct*
_tmp753=(struct Cyc_Absyn_Namespace_d_struct*)_tmp74C;if(_tmp753->tag != 10)goto
_LL3EA;else{_tmp754=_tmp753->f2;}}_LL3E9: Cyc_NewControlFlow_cf_check(_tmp754);
goto _LL3E1;_LL3EA: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp755=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp74C;if(_tmp755->tag != 13)goto _LL3EC;}_LL3EB: goto _LL3E1;_LL3EC:;_LL3ED: goto
_LL3E1;_LL3E1:;}}
