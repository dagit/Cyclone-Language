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
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
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
extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int
lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;
int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(
struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*x);void*Cyc_List_fold_right(void*(*f)(
void*,void*),struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0
Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);int Cyc_zstrcmp(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;
int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct _dyneither_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct
Cyc_Position_Segment*,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
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
int tag;struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;
struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
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
struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct
Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct
Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;
struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3
val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};
union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct
_union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{
union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_Aggr_td_struct{int
tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_struct{int tag;struct
Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;};struct Cyc_Absyn_TypeDecl{void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};
struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
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
};extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct
Cyc_Absyn_UniqueRgn_val;extern struct Cyc_Absyn_RefCntRgn_struct Cyc_Absyn_RefCntRgn_val;
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
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};extern
struct Cyc_Absyn_Stdcall_att_struct Cyc_Absyn_Stdcall_att_val;extern struct Cyc_Absyn_Cdecl_att_struct
Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_struct Cyc_Absyn_Noreturn_att_val;extern struct
Cyc_Absyn_Const_att_struct Cyc_Absyn_Const_att_val;extern struct Cyc_Absyn_Packed_att_struct
Cyc_Absyn_Packed_att_val;extern struct Cyc_Absyn_Shared_att_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_struct Cyc_Absyn_Unused_att_val;extern struct Cyc_Absyn_Weak_att_struct
Cyc_Absyn_Weak_att_val;extern struct Cyc_Absyn_Dllimport_att_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_struct Cyc_Absyn_Dllexport_att_val;extern
struct Cyc_Absyn_No_instrument_function_att_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_struct Cyc_Absyn_Constructor_att_val;extern
struct Cyc_Absyn_Destructor_att_struct Cyc_Absyn_Destructor_att_val;extern struct
Cyc_Absyn_No_check_memory_usage_att_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_struct{
int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct
_union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
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
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*
r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope
sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;struct Cyc_Absyn_Tqual ret_tqual;
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
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};extern struct Cyc_Absyn_Porton_d_struct
Cyc_Absyn_Porton_d_val;extern struct Cyc_Absyn_Portoff_d_struct Cyc_Absyn_Portoff_d_val;
struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_is_qvar_qualified(struct _tuple1*);struct Cyc_Absyn_Tqual
Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(
void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();extern union Cyc_Absyn_Constraint*
Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_starb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
enum Cyc_Absyn_Sign,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(
enum Cyc_Absyn_Primop,struct Cyc_List_List*es,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int
volatile_kw,struct _dyneither_ptr body,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*
exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);struct Cyc_Absyn_TypeDecl*
Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct
_tuple1*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(
enum Cyc_Absyn_Scope s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(
enum Cyc_Absyn_Scope s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,struct Cyc_Absyn_Tqual
ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(
void*a);extern int Cyc_Absyn_porting_c_code;struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*
Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_compress(void*t);extern struct Cyc_Absyn_Kind
Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct Cyc_Absyn_Kind
Cyc_Tcutil_bk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;extern struct Cyc_Absyn_Kind
Cyc_Tcutil_ek;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;extern struct Cyc_Absyn_Kind
Cyc_Tcutil_trk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;extern struct Cyc_Absyn_Kind
Cyc_Tcutil_tbk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;extern struct Cyc_Absyn_Kind
Cyc_Tcutil_urk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;extern struct Cyc_Absyn_Kind
Cyc_Tcutil_ubk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;extern struct Cyc_Core_Opt
Cyc_Tcutil_rko;extern struct Cyc_Core_Opt Cyc_Tcutil_bko;extern struct Cyc_Core_Opt
Cyc_Tcutil_iko;extern struct Cyc_Core_Opt Cyc_Tcutil_trko;struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(
struct Cyc_Absyn_Kind*k);void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int
add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;
int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};void Cyc_Lex_register_typedef(struct _tuple1*s);void Cyc_Lex_enter_namespace(
struct _dyneither_ptr*);void Cyc_Lex_leave_namespace();void Cyc_Lex_enter_using(
struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=0;struct Cyc_Parse_Signed_spec_struct{
int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Unsigned_spec_struct{int
tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Short_spec_struct{int tag;
struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Long_spec_struct{int tag;struct
Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{int tag;void*f1;struct
Cyc_Position_Segment*f2;};enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0,
Cyc_Parse_Extern_sc  = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc  = 3,Cyc_Parse_Auto_sc
 = 4,Cyc_Parse_Register_sc  = 5,Cyc_Parse_Abstract_sc  = 6};struct Cyc_Parse_Declaration_spec{
struct Cyc_Core_Opt*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int
is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Declarator{struct
_tuple1*id;struct Cyc_List_List*tms;};struct Cyc_Parse_Abstractdeclarator{struct
Cyc_List_List*tms;};static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*
ts,struct Cyc_Position_Segment*loc);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*
f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple10 Cyc_Parse_apply_tms(
struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);static
struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(struct
_dyneither_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(Cyc_Position_mk_err_parse(
sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Parse_err((struct _dyneither_ptr)
Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);{struct Cyc_Position_Exit_struct
_tmp858;struct Cyc_Position_Exit_struct*_tmp857;(int)_throw((void*)((_tmp857=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp857)),((_tmp857[0]=((_tmp858.tag=
Cyc_Position_Exit,_tmp858)),_tmp857)))));};}static void Cyc_Parse_unimp(struct
_dyneither_ptr msg,struct Cyc_Position_Segment*sg){{const char*_tmp85D;void*_tmp85C[
2];struct Cyc_String_pa_struct _tmp85B;struct Cyc_String_pa_struct _tmp85A;(_tmp85A.tag=
0,((_tmp85A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp85B.tag=0,((
_tmp85B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
sg)),((_tmp85C[0]=& _tmp85B,((_tmp85C[1]=& _tmp85A,Cyc_fprintf(Cyc_stderr,((
_tmp85D="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp85D,
sizeof(char),46))),_tag_dyneither(_tmp85C,sizeof(void*),2)))))))))))));}return;}
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,struct Cyc_Position_Segment*
loc){struct _tuple8 _tmp7;struct Cyc_Absyn_Tqual _tmp8;void*_tmp9;struct _tuple8*
_tmp6=tqt;_tmp7=*_tmp6;_tmp8=_tmp7.f2;_tmp9=_tmp7.f3;if((_tmp8.print_const  || 
_tmp8.q_volatile) || _tmp8.q_restrict){if(_tmp8.loc != 0)loc=_tmp8.loc;{const char*
_tmp860;void*_tmp85F;(_tmp85F=0,Cyc_Tcutil_warn(loc,((_tmp860="qualifier on type is ignored",
_tag_dyneither(_tmp860,sizeof(char),29))),_tag_dyneither(_tmp85F,sizeof(void*),0)));};}
return _tmp9;}struct _tuple11{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*
f2;union Cyc_Absyn_Constraint*f3;void*f4;};static struct _tuple11 Cyc_Parse_collapse_pointer_quals(
struct Cyc_Position_Segment*loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*
bound,void*rgn,struct Cyc_List_List*pqs){union Cyc_Absyn_Constraint*zeroterm=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();for(0;pqs != 0;pqs=pqs->tl){
void*_tmpC=(void*)pqs->hd;struct Cyc_Absyn_Exp*_tmp14;void*_tmp16;_LL1: {struct
Cyc_Absyn_Zeroterm_ptrqual_struct*_tmpD=(struct Cyc_Absyn_Zeroterm_ptrqual_struct*)
_tmpC;if(_tmpD->tag != 4)goto _LL3;}_LL2: zeroterm=Cyc_Absyn_true_conref;goto _LL0;
_LL3: {struct Cyc_Absyn_Nozeroterm_ptrqual_struct*_tmpE=(struct Cyc_Absyn_Nozeroterm_ptrqual_struct*)
_tmpC;if(_tmpE->tag != 5)goto _LL5;}_LL4: zeroterm=Cyc_Absyn_false_conref;goto _LL0;
_LL5: {struct Cyc_Absyn_Nullable_ptrqual_struct*_tmpF=(struct Cyc_Absyn_Nullable_ptrqual_struct*)
_tmpC;if(_tmpF->tag != 7)goto _LL7;}_LL6: nullable=Cyc_Absyn_true_conref;goto _LL0;
_LL7: {struct Cyc_Absyn_Notnull_ptrqual_struct*_tmp10=(struct Cyc_Absyn_Notnull_ptrqual_struct*)
_tmpC;if(_tmp10->tag != 6)goto _LL9;}_LL8: nullable=Cyc_Absyn_false_conref;goto _LL0;
_LL9: {struct Cyc_Absyn_Fat_ptrqual_struct*_tmp11=(struct Cyc_Absyn_Fat_ptrqual_struct*)
_tmpC;if(_tmp11->tag != 3)goto _LLB;}_LLA: bound=Cyc_Absyn_bounds_dyneither_conref;
goto _LL0;_LLB: {struct Cyc_Absyn_Thin_ptrqual_struct*_tmp12=(struct Cyc_Absyn_Thin_ptrqual_struct*)
_tmpC;if(_tmp12->tag != 2)goto _LLD;}_LLC: bound=Cyc_Absyn_bounds_one_conref;goto
_LL0;_LLD: {struct Cyc_Absyn_Numelts_ptrqual_struct*_tmp13=(struct Cyc_Absyn_Numelts_ptrqual_struct*)
_tmpC;if(_tmp13->tag != 0)goto _LLF;else{_tmp14=_tmp13->f1;}}_LLE:{struct Cyc_Absyn_Upper_b_struct
_tmp863;struct Cyc_Absyn_Upper_b_struct*_tmp862;bound=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp862=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp862)),((_tmp862[0]=((_tmp863.tag=1,((_tmp863.f1=_tmp14,_tmp863)))),
_tmp862)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_struct*_tmp15=(
struct Cyc_Absyn_Region_ptrqual_struct*)_tmpC;if(_tmp15->tag != 1)goto _LL0;else{
_tmp16=(void*)_tmp15->f1;}}_LL10: rgn=_tmp16;goto _LL0;_LL0:;}{struct _tuple11
_tmp864;return(_tmp864.f1=nullable,((_tmp864.f2=bound,((_tmp864.f3=zeroterm,((
_tmp864.f4=rgn,_tmp864)))))));};}struct _tuple1*Cyc_Parse_gensym_enum(){static int
enum_counter=0;struct _dyneither_ptr*_tmp871;const char*_tmp870;void*_tmp86F[1];
struct Cyc_Int_pa_struct _tmp86E;struct _tuple1*_tmp86D;return(_tmp86D=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp86D)),((_tmp86D->f1=Cyc_Absyn_Rel_n(
0),((_tmp86D->f2=((_tmp871=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp871)),((
_tmp871[0]=(struct _dyneither_ptr)((_tmp86E.tag=1,((_tmp86E.f1=(unsigned long)
enum_counter ++,((_tmp86F[0]=& _tmp86E,Cyc_aprintf(((_tmp870="__anonymous_enum_%d__",
_tag_dyneither(_tmp870,sizeof(char),22))),_tag_dyneither(_tmp86F,sizeof(void*),1)))))))),
_tmp871)))),_tmp86D)))));}struct _tuple12{struct _tuple1*f1;struct Cyc_Absyn_Tqual
f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple13{struct
_tuple12*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple13*field_info){struct _tuple13 _tmp20;
struct _tuple12*_tmp21;struct _tuple12 _tmp22;struct _tuple1*_tmp23;struct Cyc_Absyn_Tqual
_tmp24;void*_tmp25;struct Cyc_List_List*_tmp26;struct Cyc_List_List*_tmp27;struct
Cyc_Absyn_Exp*_tmp28;struct _tuple13*_tmp1F=field_info;_tmp20=*_tmp1F;_tmp21=
_tmp20.f1;_tmp22=*_tmp21;_tmp23=_tmp22.f1;_tmp24=_tmp22.f2;_tmp25=_tmp22.f3;
_tmp26=_tmp22.f4;_tmp27=_tmp22.f5;_tmp28=_tmp20.f2;if(_tmp26 != 0){const char*
_tmp872;Cyc_Parse_err(((_tmp872="bad type params in struct field",_tag_dyneither(
_tmp872,sizeof(char),32))),loc);}if(Cyc_Absyn_is_qvar_qualified(_tmp23)){const
char*_tmp873;Cyc_Parse_err(((_tmp873="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp873,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp874;return(_tmp874=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp874)),((
_tmp874->name=(*_tmp23).f2,((_tmp874->tq=_tmp24,((_tmp874->type=_tmp25,((_tmp874->width=
_tmp28,((_tmp874->attributes=_tmp27,_tmp874)))))))))));};}static void*Cyc_Parse_type_spec(
void*t,struct Cyc_Position_Segment*loc){struct Cyc_Parse_Type_spec_struct _tmp877;
struct Cyc_Parse_Type_spec_struct*_tmp876;return(void*)((_tmp876=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp876)),((_tmp876[0]=((_tmp877.tag=4,((_tmp877.f1=(
void*)t,((_tmp877.f2=loc,_tmp877)))))),_tmp876))));}static void*Cyc_Parse_array2ptr(
void*t,int argposn){void*_tmp2E=t;struct Cyc_Absyn_ArrayInfo _tmp30;void*_tmp31;
struct Cyc_Absyn_Tqual _tmp32;struct Cyc_Absyn_Exp*_tmp33;union Cyc_Absyn_Constraint*
_tmp34;struct Cyc_Position_Segment*_tmp35;_LL12: {struct Cyc_Absyn_ArrayType_struct*
_tmp2F=(struct Cyc_Absyn_ArrayType_struct*)_tmp2E;if(_tmp2F->tag != 9)goto _LL14;
else{_tmp30=_tmp2F->f1;_tmp31=_tmp30.elt_type;_tmp32=_tmp30.tq;_tmp33=_tmp30.num_elts;
_tmp34=_tmp30.zero_term;_tmp35=_tmp30.zt_loc;}}_LL13: {struct Cyc_Absyn_Upper_b_struct
_tmp87A;struct Cyc_Absyn_Upper_b_struct*_tmp879;return Cyc_Absyn_starb_typ(_tmp31,
argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,
_tmp32,_tmp33 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((void*)((_tmp879=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp879)),((_tmp879[0]=((_tmp87A.tag=
1,((_tmp87A.f1=(struct Cyc_Absyn_Exp*)_tmp33,_tmp87A)))),_tmp879))))),_tmp34);}
_LL14:;_LL15: return t;_LL11:;}struct _tuple14{struct _dyneither_ptr*f1;void*f2;};
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){struct
Cyc_List_List*_tmp38=0;for(0;x != 0;x=x->tl){struct _tuple8*_tmp39=(struct _tuple8*)
x->hd;struct _tuple8 _tmp3A;struct Cyc_Core_Opt*_tmp3B;struct Cyc_Core_Opt _tmp3C;
struct _dyneither_ptr*_tmp3D;void*_tmp3E;void*_tmp40;struct _tuple8 _tmp41;struct
Cyc_Core_Opt*_tmp42;struct Cyc_Core_Opt _tmp43;struct _dyneither_ptr*_tmp44;void*
_tmp45;void*_tmp47;struct Cyc_Core_Opt*_tmp49;struct Cyc_Core_Opt**_tmp4A;_LL17:
_tmp3A=*_tmp39;_tmp3B=_tmp3A.f1;if(_tmp3B == 0)goto _LL19;_tmp3C=*_tmp3B;_tmp3D=(
struct _dyneither_ptr*)_tmp3C.v;_tmp3E=_tmp3A.f3;{struct Cyc_Absyn_TagType_struct*
_tmp3F=(struct Cyc_Absyn_TagType_struct*)_tmp3E;if(_tmp3F->tag != 20)goto _LL19;
else{_tmp40=(void*)_tmp3F->f1;}};_LL18:{void*_tmp4B=_tmp40;struct Cyc_Core_Opt*
_tmp4D;struct Cyc_Core_Opt**_tmp4E;_LL1E: {struct Cyc_Absyn_Evar_struct*_tmp4C=(
struct Cyc_Absyn_Evar_struct*)_tmp4B;if(_tmp4C->tag != 1)goto _LL20;else{_tmp4D=
_tmp4C->f2;_tmp4E=(struct Cyc_Core_Opt**)& _tmp4C->f2;}}_LL1F: {struct Cyc_String_pa_struct
_tmp882;void*_tmp881[1];const char*_tmp880;struct _dyneither_ptr*_tmp87F;struct
_dyneither_ptr*nm=(_tmp87F=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp87F)),((
_tmp87F[0]=(struct _dyneither_ptr)((_tmp882.tag=0,((_tmp882.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3D),((_tmp881[0]=& _tmp882,Cyc_aprintf(((
_tmp880="`%s",_tag_dyneither(_tmp880,sizeof(char),4))),_tag_dyneither(_tmp881,
sizeof(void*),1)))))))),_tmp87F)));{struct Cyc_Absyn_VarType_struct*_tmp897;
struct Cyc_Absyn_Tvar*_tmp896;struct Cyc_Absyn_Eq_kb_struct _tmp895;struct Cyc_Absyn_Eq_kb_struct*
_tmp894;struct Cyc_Absyn_VarType_struct _tmp893;struct Cyc_Core_Opt*_tmp892;*_tmp4E=((
_tmp892=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp892)),((_tmp892->v=(
void*)((_tmp897=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp897)),((
_tmp897[0]=((_tmp893.tag=2,((_tmp893.f1=((_tmp896=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp896)),((_tmp896->name=nm,((_tmp896->identity=- 1,((_tmp896->kind=(
void*)((_tmp894=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp894)),((
_tmp894[0]=((_tmp895.tag=0,((_tmp895.f1=& Cyc_Tcutil_ik,_tmp895)))),_tmp894)))),
_tmp896)))))))),_tmp893)))),_tmp897)))),_tmp892))));}goto _LL1D;}_LL20:;_LL21:
goto _LL1D;_LL1D:;}{struct _tuple14*_tmp89A;struct Cyc_List_List*_tmp899;_tmp38=((
_tmp899=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp899)),((_tmp899->hd=((
_tmp89A=_cycalloc(sizeof(*_tmp89A)),((_tmp89A->f1=_tmp3D,((_tmp89A->f2=_tmp40,
_tmp89A)))))),((_tmp899->tl=_tmp38,_tmp899))))));}goto _LL16;_LL19: _tmp41=*_tmp39;
_tmp42=_tmp41.f1;if(_tmp42 == 0)goto _LL1B;_tmp43=*_tmp42;_tmp44=(struct
_dyneither_ptr*)_tmp43.v;_tmp45=_tmp41.f3;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp46=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp45;if(_tmp46->tag != 16)goto
_LL1B;else{_tmp47=(void*)_tmp46->f1;{struct Cyc_Absyn_Evar_struct*_tmp48=(struct
Cyc_Absyn_Evar_struct*)_tmp47;if(_tmp48->tag != 1)goto _LL1B;else{_tmp49=_tmp48->f2;
_tmp4A=(struct Cyc_Core_Opt**)& _tmp48->f2;}};}};_LL1A: {struct Cyc_String_pa_struct
_tmp8A2;void*_tmp8A1[1];const char*_tmp8A0;struct _dyneither_ptr*_tmp89F;struct
_dyneither_ptr*nm=(_tmp89F=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp89F)),((
_tmp89F[0]=(struct _dyneither_ptr)((_tmp8A2.tag=0,((_tmp8A2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp44),((_tmp8A1[0]=& _tmp8A2,Cyc_aprintf(((
_tmp8A0="`%s",_tag_dyneither(_tmp8A0,sizeof(char),4))),_tag_dyneither(_tmp8A1,
sizeof(void*),1)))))))),_tmp89F)));{struct Cyc_Absyn_VarType_struct*_tmp8B7;
struct Cyc_Absyn_Tvar*_tmp8B6;struct Cyc_Absyn_Eq_kb_struct _tmp8B5;struct Cyc_Absyn_Eq_kb_struct*
_tmp8B4;struct Cyc_Absyn_VarType_struct _tmp8B3;struct Cyc_Core_Opt*_tmp8B2;*_tmp4A=((
_tmp8B2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8B2)),((_tmp8B2->v=(
void*)((_tmp8B7=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8B7)),((
_tmp8B7[0]=((_tmp8B3.tag=2,((_tmp8B3.f1=((_tmp8B6=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp8B6)),((_tmp8B6->name=nm,((_tmp8B6->identity=- 1,((_tmp8B6->kind=(
void*)((_tmp8B4=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8B4)),((
_tmp8B4[0]=((_tmp8B5.tag=0,((_tmp8B5.f1=& Cyc_Tcutil_rk,_tmp8B5)))),_tmp8B4)))),
_tmp8B6)))))))),_tmp8B3)))),_tmp8B7)))),_tmp8B2))));}goto _LL16;}_LL1B:;_LL1C:
goto _LL16;_LL16:;}return _tmp38;}static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(
struct Cyc_List_List*x){struct Cyc_List_List*_tmp65=0;for(0;x != 0;x=x->tl){void*
_tmp66=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp68;_LL23: {struct Cyc_Absyn_TagType_struct*
_tmp67=(struct Cyc_Absyn_TagType_struct*)_tmp66;if(_tmp67->tag != 20)goto _LL25;
else{_tmp68=(void*)_tmp67->f1;}}_LL24:{struct _tuple14*_tmp8BA;struct Cyc_List_List*
_tmp8B9;_tmp65=((_tmp8B9=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8B9)),((
_tmp8B9->hd=((_tmp8BA=_cycalloc(sizeof(*_tmp8BA)),((_tmp8BA->f1=((struct Cyc_Absyn_Aggrfield*)
x->hd)->name,((_tmp8BA->f2=_tmp68,_tmp8BA)))))),((_tmp8B9->tl=_tmp65,_tmp8B9))))));}
goto _LL22;_LL25:;_LL26: goto _LL22;_LL22:;}return _tmp65;}static struct Cyc_Absyn_Exp*
Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){{
void*_tmp6B=e->r;struct _tuple1*_tmp6D;struct _tuple1 _tmp6E;union Cyc_Absyn_Nmspace
_tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr*_tmp71;struct _tuple1*
_tmp73;struct _tuple1 _tmp74;union Cyc_Absyn_Nmspace _tmp75;struct Cyc_List_List*
_tmp76;struct _dyneither_ptr*_tmp77;_LL28: {struct Cyc_Absyn_Var_e_struct*_tmp6C=(
struct Cyc_Absyn_Var_e_struct*)_tmp6B;if(_tmp6C->tag != 1)goto _LL2A;else{_tmp6D=
_tmp6C->f1;_tmp6E=*_tmp6D;_tmp6F=_tmp6E.f1;if((_tmp6F.Rel_n).tag != 1)goto _LL2A;
_tmp70=(struct Cyc_List_List*)(_tmp6F.Rel_n).val;if(_tmp70 != 0)goto _LL2A;_tmp71=
_tmp6E.f2;}}_LL29: _tmp77=_tmp71;goto _LL2B;_LL2A: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp72=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp6B;if(_tmp72->tag != 2)goto _LL2C;
else{_tmp73=_tmp72->f1;_tmp74=*_tmp73;_tmp75=_tmp74.f1;if((_tmp75.Rel_n).tag != 1)
goto _LL2C;_tmp76=(struct Cyc_List_List*)(_tmp75.Rel_n).val;if(_tmp76 != 0)goto
_LL2C;_tmp77=_tmp74.f2;}}_LL2B:{struct Cyc_List_List*_tmp78=tags;for(0;_tmp78 != 0;
_tmp78=_tmp78->tl){struct _tuple14 _tmp7A;struct _dyneither_ptr*_tmp7B;void*_tmp7C;
struct _tuple14*_tmp79=(struct _tuple14*)_tmp78->hd;_tmp7A=*_tmp79;_tmp7B=_tmp7A.f1;
_tmp7C=_tmp7A.f2;if(Cyc_strptrcmp(_tmp7B,_tmp77)== 0){struct Cyc_Absyn_Valueof_e_struct
_tmp8BD;struct Cyc_Absyn_Valueof_e_struct*_tmp8BC;return Cyc_Absyn_new_exp((void*)((
_tmp8BC=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8BC)),((_tmp8BC[0]=((
_tmp8BD.tag=39,((_tmp8BD.f1=(void*)Cyc_Tcutil_copy_type(_tmp7C),_tmp8BD)))),
_tmp8BC)))),e->loc);}}}goto _LL27;_LL2C:;_LL2D: goto _LL27;_LL27:;}return e;}static
void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){{void*_tmp7F=t;
struct Cyc_Absyn_ArrayInfo _tmp81;void*_tmp82;struct Cyc_Absyn_Tqual _tmp83;struct
Cyc_Absyn_Exp*_tmp84;union Cyc_Absyn_Constraint*_tmp85;struct Cyc_Position_Segment*
_tmp86;struct Cyc_Absyn_PtrInfo _tmp88;void*_tmp89;struct Cyc_Absyn_Tqual _tmp8A;
struct Cyc_Absyn_PtrAtts _tmp8B;void*_tmp8C;union Cyc_Absyn_Constraint*_tmp8D;union
Cyc_Absyn_Constraint*_tmp8E;union Cyc_Absyn_Constraint*_tmp8F;struct Cyc_Absyn_PtrLoc*
_tmp90;_LL2F: {struct Cyc_Absyn_ArrayType_struct*_tmp80=(struct Cyc_Absyn_ArrayType_struct*)
_tmp7F;if(_tmp80->tag != 9)goto _LL31;else{_tmp81=_tmp80->f1;_tmp82=_tmp81.elt_type;
_tmp83=_tmp81.tq;_tmp84=_tmp81.num_elts;_tmp85=_tmp81.zero_term;_tmp86=_tmp81.zt_loc;}}
_LL30: {struct Cyc_Absyn_Exp*nelts2=_tmp84;if(_tmp84 != 0)nelts2=(struct Cyc_Absyn_Exp*)
Cyc_Parse_substitute_tags_exp(tags,(struct Cyc_Absyn_Exp*)_tmp84);{void*_tmp91=
Cyc_Parse_substitute_tags(tags,_tmp82);if(_tmp84 != nelts2  || _tmp82 != _tmp91){
struct Cyc_Absyn_ArrayType_struct _tmp8C3;struct Cyc_Absyn_ArrayInfo _tmp8C2;struct
Cyc_Absyn_ArrayType_struct*_tmp8C1;return(void*)((_tmp8C1=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp8C1)),((_tmp8C1[0]=((_tmp8C3.tag=9,((_tmp8C3.f1=((_tmp8C2.elt_type=
_tmp91,((_tmp8C2.tq=_tmp83,((_tmp8C2.num_elts=nelts2,((_tmp8C2.zero_term=_tmp85,((
_tmp8C2.zt_loc=_tmp86,_tmp8C2)))))))))),_tmp8C3)))),_tmp8C1))));}goto _LL2E;};}
_LL31: {struct Cyc_Absyn_PointerType_struct*_tmp87=(struct Cyc_Absyn_PointerType_struct*)
_tmp7F;if(_tmp87->tag != 5)goto _LL33;else{_tmp88=_tmp87->f1;_tmp89=_tmp88.elt_typ;
_tmp8A=_tmp88.elt_tq;_tmp8B=_tmp88.ptr_atts;_tmp8C=_tmp8B.rgn;_tmp8D=_tmp8B.nullable;
_tmp8E=_tmp8B.bounds;_tmp8F=_tmp8B.zero_term;_tmp90=_tmp8B.ptrloc;}}_LL32: {void*
_tmp95=Cyc_Parse_substitute_tags(tags,_tmp89);union Cyc_Absyn_Constraint*_tmp96=
_tmp8E;{union Cyc_Absyn_Constraint _tmp97=*_tmp8E;void*_tmp98;struct Cyc_Absyn_Exp*
_tmp9A;_LL36: if((_tmp97.Eq_constr).tag != 1)goto _LL38;_tmp98=(void*)(_tmp97.Eq_constr).val;{
struct Cyc_Absyn_Upper_b_struct*_tmp99=(struct Cyc_Absyn_Upper_b_struct*)_tmp98;
if(_tmp99->tag != 1)goto _LL38;else{_tmp9A=_tmp99->f1;}};_LL37: _tmp9A=Cyc_Parse_substitute_tags_exp(
tags,_tmp9A);{struct Cyc_Absyn_Upper_b_struct*_tmp8C9;struct Cyc_Absyn_Upper_b_struct
_tmp8C8;union Cyc_Absyn_Constraint*_tmp8C7;_tmp96=((_tmp8C7=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp8C7)),(((_tmp8C7->Eq_constr).val=(void*)((_tmp8C9=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp8C9)),((_tmp8C9[0]=((_tmp8C8.tag=1,((_tmp8C8.f1=_tmp9A,_tmp8C8)))),
_tmp8C9)))),(((_tmp8C7->Eq_constr).tag=1,_tmp8C7))))));}goto _LL35;_LL38:;_LL39:
goto _LL35;_LL35:;}if(_tmp95 != _tmp89  || _tmp96 != _tmp8E){struct Cyc_Absyn_PointerType_struct
_tmp8D3;struct Cyc_Absyn_PtrAtts _tmp8D2;struct Cyc_Absyn_PtrInfo _tmp8D1;struct Cyc_Absyn_PointerType_struct*
_tmp8D0;return(void*)((_tmp8D0=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp8D0)),((_tmp8D0[0]=((_tmp8D3.tag=5,((_tmp8D3.f1=((_tmp8D1.elt_typ=_tmp95,((
_tmp8D1.elt_tq=_tmp8A,((_tmp8D1.ptr_atts=((_tmp8D2.rgn=_tmp8C,((_tmp8D2.nullable=
_tmp8D,((_tmp8D2.bounds=_tmp96,((_tmp8D2.zero_term=_tmp8F,((_tmp8D2.ptrloc=
_tmp90,_tmp8D2)))))))))),_tmp8D1)))))),_tmp8D3)))),_tmp8D0))));}goto _LL2E;}_LL33:;
_LL34: goto _LL2E;_LL2E:;}return t;}static void Cyc_Parse_substitute_aggrfield_tags(
struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){x->type=Cyc_Parse_substitute_tags(
tags,x->type);}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};static struct
_tuple15*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple8*t){
struct _tuple15*_tmp8D4;return(_tmp8D4=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp8D4)),((_tmp8D4->f1=(*t).f2,((_tmp8D4->f2=(*t).f3,_tmp8D4)))));}
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmpA3=d->r;struct Cyc_Absyn_Vardecl*_tmpA5;_LL3B: {struct Cyc_Absyn_Var_d_struct*
_tmpA4=(struct Cyc_Absyn_Var_d_struct*)_tmpA3;if(_tmpA4->tag != 0)goto _LL3D;else{
_tmpA5=_tmpA4->f1;}}_LL3C: return _tmpA5;_LL3D:;_LL3E: {const char*_tmp8D7;void*
_tmp8D6;(_tmp8D6=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((_tmp8D7="bad declaration in `forarray' statement",
_tag_dyneither(_tmp8D7,sizeof(char),40))),_tag_dyneither(_tmp8D6,sizeof(void*),0)));}
_LL3A:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmpA8=tm;_LL40: {struct
Cyc_Absyn_TypeParams_mod_struct*_tmpA9=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpA8;if(_tmpA9->tag != 4)goto _LL42;}_LL41: return 1;_LL42:;_LL43: return 0;_LL3F:;}
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){const char*_tmp8D8;if(
Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8D8="`H",_tag_dyneither(_tmp8D8,
sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_HeapRgn_val;else{const char*
_tmp8D9;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8D9="`U",_tag_dyneither(
_tmp8D9,sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_UniqueRgn_val;else{const
char*_tmp8DA;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8DA="`RC",
_tag_dyneither(_tmp8DA,sizeof(char),4))))== 0)return(void*)& Cyc_Absyn_RefCntRgn_val;
else{struct Cyc_Absyn_VarType_struct _tmp8E4;struct _dyneither_ptr*_tmp8E3;struct
Cyc_Absyn_Tvar*_tmp8E2;struct Cyc_Absyn_VarType_struct*_tmp8E1;return(void*)((
_tmp8E1=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8E1)),((_tmp8E1[0]=((
_tmp8E4.tag=2,((_tmp8E4.f1=((_tmp8E2=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp8E2)),((_tmp8E2->name=((_tmp8E3=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp8E3)),((_tmp8E3[0]=s,_tmp8E3)))),((_tmp8E2->identity=- 1,((_tmp8E2->kind=
k,_tmp8E2)))))))),_tmp8E4)))),_tmp8E1))));}}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar*t){void*k;{void*_tmpB1=Cyc_Absyn_compress_kb(t->kind);
struct Cyc_Absyn_Kind*_tmpB4;_LL45: {struct Cyc_Absyn_Unknown_kb_struct*_tmpB2=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpB1;if(_tmpB2->tag != 1)goto _LL47;}_LL46:{
struct Cyc_Absyn_Unknown_kb_struct _tmp8E7;struct Cyc_Absyn_Unknown_kb_struct*
_tmp8E6;k=(void*)((_tmp8E6=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8E6)),((
_tmp8E6[0]=((_tmp8E7.tag=1,((_tmp8E7.f1=0,_tmp8E7)))),_tmp8E6))));}goto _LL44;
_LL47: {struct Cyc_Absyn_Less_kb_struct*_tmpB3=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB1;if(_tmpB3->tag != 2)goto _LL49;else{_tmpB4=_tmpB3->f2;}}_LL48:{struct Cyc_Absyn_Less_kb_struct
_tmp8EA;struct Cyc_Absyn_Less_kb_struct*_tmp8E9;k=(void*)((_tmp8E9=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp8E9)),((_tmp8E9[0]=((_tmp8EA.tag=2,((_tmp8EA.f1=
0,((_tmp8EA.f2=_tmpB4,_tmp8EA)))))),_tmp8E9))));}goto _LL44;_LL49:;_LL4A: k=_tmpB1;
goto _LL44;_LL44:;}{struct Cyc_Absyn_Tvar*_tmp8EB;return(_tmp8EB=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp8EB)),((_tmp8EB->name=t->name,((_tmp8EB->identity=
- 1,((_tmp8EB->kind=k,_tmp8EB)))))));};}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(
struct Cyc_Position_Segment*loc,void*t){void*_tmpBA=t;struct Cyc_Absyn_Tvar*_tmpBC;
_LL4C: {struct Cyc_Absyn_VarType_struct*_tmpBB=(struct Cyc_Absyn_VarType_struct*)
_tmpBA;if(_tmpBB->tag != 2)goto _LL4E;else{_tmpBC=_tmpBB->f1;}}_LL4D: return _tmpBC;
_LL4E:;_LL4F: {const char*_tmp8EE;void*_tmp8ED;(_tmp8ED=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp8EE="expecting a list of type variables, not types",_tag_dyneither(_tmp8EE,
sizeof(char),46))),_tag_dyneither(_tmp8ED,sizeof(void*),0)));}_LL4B:;}static void*
Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct
_tmp8F1;struct Cyc_Absyn_VarType_struct*_tmp8F0;return(void*)((_tmp8F0=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8F1.tag=
2,((_tmp8F1.f1=pr,_tmp8F1)))),_tmp8F0))));}static void Cyc_Parse_set_vartyp_kind(
void*t,struct Cyc_Absyn_Kind*k,int leq){void*_tmpC1=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_Tvar*_tmpC3;struct Cyc_Absyn_Tvar _tmpC4;void*_tmpC5;void**_tmpC6;_LL51: {
struct Cyc_Absyn_VarType_struct*_tmpC2=(struct Cyc_Absyn_VarType_struct*)_tmpC1;
if(_tmpC2->tag != 2)goto _LL53;else{_tmpC3=_tmpC2->f1;_tmpC4=*_tmpC3;_tmpC5=_tmpC4.kind;
_tmpC6=(void**)&(*_tmpC2->f1).kind;}}_LL52: {void*_tmpC7=Cyc_Absyn_compress_kb(*
_tmpC6);_LL56: {struct Cyc_Absyn_Unknown_kb_struct*_tmpC8=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC7;if(_tmpC8->tag != 1)goto _LL58;}_LL57: if(!leq)*_tmpC6=Cyc_Tcutil_kind_to_bound(
k);else{struct Cyc_Absyn_Less_kb_struct _tmp8F4;struct Cyc_Absyn_Less_kb_struct*
_tmp8F3;*_tmpC6=(void*)((_tmp8F3=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp8F3)),((_tmp8F3[0]=((_tmp8F4.tag=2,((_tmp8F4.f1=0,((_tmp8F4.f2=k,_tmp8F4)))))),
_tmp8F3))));}return;_LL58:;_LL59: return;_LL55:;}_LL53:;_LL54: return;_LL50:;}
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct Cyc_List_List*tms,
struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){if(tds == 0)return tms;if(
tms == 0)return 0;{void*_tmpCB=(void*)tms->hd;void*_tmpCD;_LL5B: {struct Cyc_Absyn_Function_mod_struct*
_tmpCC=(struct Cyc_Absyn_Function_mod_struct*)_tmpCB;if(_tmpCC->tag != 3)goto _LL5D;
else{_tmpCD=(void*)_tmpCC->f1;}}_LL5C: if(tms->tl == 0  || Cyc_Parse_is_typeparam((
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)
_check_null(tms->tl))->tl == 0){void*_tmpCE=_tmpCD;struct Cyc_List_List*_tmpD1;
_LL60: {struct Cyc_Absyn_WithTypes_struct*_tmpCF=(struct Cyc_Absyn_WithTypes_struct*)
_tmpCE;if(_tmpCF->tag != 1)goto _LL62;}_LL61:{const char*_tmp8F7;void*_tmp8F6;(
_tmp8F6=0,Cyc_Tcutil_warn(loc,((_tmp8F7="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp8F7,sizeof(char),93))),_tag_dyneither(_tmp8F6,sizeof(void*),0)));}
return tms;_LL62: {struct Cyc_Absyn_NoTypes_struct*_tmpD0=(struct Cyc_Absyn_NoTypes_struct*)
_tmpCE;if(_tmpD0->tag != 0)goto _LL5F;else{_tmpD1=_tmpD0->f1;}}_LL63: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpD1)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds)){const char*_tmp8FA;void*_tmp8F9;(_tmp8F9=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
loc,((_tmp8FA="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp8FA,sizeof(char),73))),_tag_dyneither(_tmp8F9,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpD6=0;for(0;_tmpD1 != 0;_tmpD1=_tmpD1->tl){struct Cyc_List_List*
_tmpD7=tds;for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){struct Cyc_Absyn_Decl*_tmpD8=(
struct Cyc_Absyn_Decl*)_tmpD7->hd;void*_tmpD9=_tmpD8->r;struct Cyc_Absyn_Vardecl*
_tmpDB;_LL65: {struct Cyc_Absyn_Var_d_struct*_tmpDA=(struct Cyc_Absyn_Var_d_struct*)
_tmpD9;if(_tmpDA->tag != 0)goto _LL67;else{_tmpDB=_tmpDA->f1;}}_LL66: if(Cyc_zstrptrcmp((*
_tmpDB->name).f2,(struct _dyneither_ptr*)_tmpD1->hd)!= 0)continue;if(_tmpDB->initializer
!= 0){const char*_tmp8FD;void*_tmp8FC;(_tmp8FC=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD8->loc,((
_tmp8FD="initializer found in parameter declaration",_tag_dyneither(_tmp8FD,
sizeof(char),43))),_tag_dyneither(_tmp8FC,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmpDB->name)){const char*_tmp900;void*_tmp8FF;(_tmp8FF=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD8->loc,((
_tmp900="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp900,
sizeof(char),47))),_tag_dyneither(_tmp8FF,sizeof(void*),0)));}{struct _tuple8*
_tmp906;struct Cyc_Core_Opt*_tmp905;struct Cyc_List_List*_tmp904;_tmpD6=((_tmp904=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp904)),((_tmp904->hd=((_tmp906=
_cycalloc(sizeof(*_tmp906)),((_tmp906->f1=((_tmp905=_cycalloc(sizeof(*_tmp905)),((
_tmp905->v=(*_tmpDB->name).f2,_tmp905)))),((_tmp906->f2=_tmpDB->tq,((_tmp906->f3=
_tmpDB->type,_tmp906)))))))),((_tmp904->tl=_tmpD6,_tmp904))))));}goto L;_LL67:;
_LL68: {const char*_tmp909;void*_tmp908;(_tmp908=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD8->loc,((
_tmp909="nonvariable declaration in parameter type",_tag_dyneither(_tmp909,
sizeof(char),42))),_tag_dyneither(_tmp908,sizeof(void*),0)));}_LL64:;}L: if(
_tmpD7 == 0){const char*_tmp90D;void*_tmp90C[1];struct Cyc_String_pa_struct _tmp90B;(
_tmp90B.tag=0,((_tmp90B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmpD1->hd)),((_tmp90C[0]=& _tmp90B,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp90D="%s is not given a type",_tag_dyneither(_tmp90D,sizeof(char),23))),
_tag_dyneither(_tmp90C,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp91C;struct Cyc_Absyn_WithTypes_struct*_tmp91B;struct Cyc_Absyn_WithTypes_struct
_tmp91A;struct Cyc_Absyn_Function_mod_struct _tmp919;struct Cyc_List_List*_tmp918;
return(_tmp918=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp918)),((_tmp918->hd=(
void*)((_tmp91C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp91C)),((
_tmp91C[0]=((_tmp919.tag=3,((_tmp919.f1=(void*)((void*)((_tmp91B=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp91B)),((_tmp91B[0]=((_tmp91A.tag=1,((_tmp91A.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD6),((
_tmp91A.f2=0,((_tmp91A.f3=0,((_tmp91A.f4=0,((_tmp91A.f5=0,_tmp91A)))))))))))),
_tmp91B))))),_tmp919)))),_tmp91C)))),((_tmp918->tl=0,_tmp918)))));};};_LL5F:;}
goto _LL5E;_LL5D:;_LL5E: {struct Cyc_List_List*_tmp91D;return(_tmp91D=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp91D)),((_tmp91D->hd=(void*)tms->hd,((
_tmp91D->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc),_tmp91D)))));}_LL5A:;};}
struct _tuple16{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*
d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*
loc){if(tds != 0){struct Cyc_Parse_Declarator*_tmp91E;d=((_tmp91E=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp91E)),((_tmp91E->id=d->id,((_tmp91E->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc),_tmp91E))))));}{enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;struct
Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);int
is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0)
switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v){case Cyc_Parse_Extern_sc: _LL69: sc=
Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL6A: sc=Cyc_Absyn_Static;break;
default: _LL6B:{const char*_tmp91F;Cyc_Parse_err(((_tmp91F="bad storage class on function",
_tag_dyneither(_tmp91F,sizeof(char),30))),loc);}break;}}{void*_tmpF0=Cyc_Parse_collapse_type_specifiers(
tss,loc);struct Cyc_Absyn_Tqual _tmpF2;void*_tmpF3;struct Cyc_List_List*_tmpF4;
struct Cyc_List_List*_tmpF5;struct _tuple10 _tmpF1=Cyc_Parse_apply_tms(tq,_tmpF0,
atts,d->tms);_tmpF2=_tmpF1.f1;_tmpF3=_tmpF1.f2;_tmpF4=_tmpF1.f3;_tmpF5=_tmpF1.f4;
if(_tmpF4 != 0){const char*_tmp922;void*_tmp921;(_tmp921=0,Cyc_Tcutil_warn(loc,((
_tmp922="bad type params, ignoring",_tag_dyneither(_tmp922,sizeof(char),26))),
_tag_dyneither(_tmp921,sizeof(void*),0)));}{void*_tmpF8=_tmpF3;struct Cyc_Absyn_FnInfo
_tmpFA;struct Cyc_List_List*_tmpFB;struct Cyc_Core_Opt*_tmpFC;struct Cyc_Absyn_Tqual
_tmpFD;void*_tmpFE;struct Cyc_List_List*_tmpFF;int _tmp100;struct Cyc_Absyn_VarargInfo*
_tmp101;struct Cyc_List_List*_tmp102;struct Cyc_List_List*_tmp103;_LL6E: {struct
Cyc_Absyn_FnType_struct*_tmpF9=(struct Cyc_Absyn_FnType_struct*)_tmpF8;if(_tmpF9->tag
!= 10)goto _LL70;else{_tmpFA=_tmpF9->f1;_tmpFB=_tmpFA.tvars;_tmpFC=_tmpFA.effect;
_tmpFD=_tmpFA.ret_tqual;_tmpFE=_tmpFA.ret_typ;_tmpFF=_tmpFA.args;_tmp100=_tmpFA.c_varargs;
_tmp101=_tmpFA.cyc_varargs;_tmp102=_tmpFA.rgn_po;_tmp103=_tmpFA.attributes;}}
_LL6F: {struct Cyc_List_List*_tmp104=0;{struct Cyc_List_List*_tmp105=_tmpFF;for(0;
_tmp105 != 0;_tmp105=_tmp105->tl){struct _tuple8 _tmp107;struct Cyc_Core_Opt*_tmp108;
struct Cyc_Absyn_Tqual _tmp109;void*_tmp10A;struct _tuple8*_tmp106=(struct _tuple8*)
_tmp105->hd;_tmp107=*_tmp106;_tmp108=_tmp107.f1;_tmp109=_tmp107.f2;_tmp10A=
_tmp107.f3;if(_tmp108 == 0){{const char*_tmp923;Cyc_Parse_err(((_tmp923="missing argument variable in function prototype",
_tag_dyneither(_tmp923,sizeof(char),48))),loc);}{struct _tuple16*_tmp926;struct
Cyc_List_List*_tmp925;_tmp104=((_tmp925=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp925)),((_tmp925->hd=((_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926->f1=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((
_tmp926->f2=_tmp109,((_tmp926->f3=_tmp10A,_tmp926)))))))),((_tmp925->tl=_tmp104,
_tmp925))))));};}else{struct _tuple16*_tmp929;struct Cyc_List_List*_tmp928;_tmp104=((
_tmp928=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp928)),((_tmp928->hd=((
_tmp929=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp929)),((_tmp929->f1=(
struct _dyneither_ptr*)_tmp108->v,((_tmp929->f2=_tmp109,((_tmp929->f3=_tmp10A,
_tmp929)))))))),((_tmp928->tl=_tmp104,_tmp928))))));}}}{struct Cyc_Absyn_Fndecl*
_tmp92A;return(_tmp92A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp92A)),((
_tmp92A->sc=sc,((_tmp92A->name=d->id,((_tmp92A->tvs=_tmpFB,((_tmp92A->is_inline=
is_inline,((_tmp92A->effect=_tmpFC,((_tmp92A->ret_tqual=_tmpFD,((_tmp92A->ret_type=
_tmpFE,((_tmp92A->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp104),((_tmp92A->c_varargs=_tmp100,((_tmp92A->cyc_varargs=_tmp101,((_tmp92A->rgn_po=
_tmp102,((_tmp92A->body=body,((_tmp92A->cached_typ=0,((_tmp92A->param_vardecls=0,((
_tmp92A->fn_vardecl=0,((_tmp92A->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp103,_tmpF5),_tmp92A)))))))))))))))))))))))))))))))));};}
_LL70:;_LL71: {const char*_tmp92D;void*_tmp92C;(_tmp92C=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp92D="declarator is not a function prototype",_tag_dyneither(_tmp92D,sizeof(
char),39))),_tag_dyneither(_tmp92C,sizeof(void*),0)));}_LL6D:;};};};}static char
_tmp114[52]="at most one type may appear within a type specifier";static struct
_dyneither_ptr Cyc_Parse_msg1={_tmp114,_tmp114,_tmp114 + 52};static char _tmp115[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp115,_tmp115,_tmp115 + 63};static
char _tmp116[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp116,_tmp116,_tmp116 + 50};static char _tmp117[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp117,_tmp117,_tmp117 + 60};static
void*Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){int seen_type=0;int seen_sign=0;int seen_size=0;void*t=(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp118=(
void*)ts->hd;void*_tmp11A;struct Cyc_Position_Segment*_tmp11B;struct Cyc_Position_Segment*
_tmp11D;struct Cyc_Position_Segment*_tmp11F;struct Cyc_Position_Segment*_tmp121;
struct Cyc_Position_Segment*_tmp123;_LL73: {struct Cyc_Parse_Type_spec_struct*
_tmp119=(struct Cyc_Parse_Type_spec_struct*)_tmp118;if(_tmp119->tag != 4)goto _LL75;
else{_tmp11A=(void*)_tmp119->f1;_tmp11B=_tmp119->f2;}}_LL74: if(seen_type)Cyc_Parse_err(
Cyc_Parse_msg1,_tmp11B);last_loc=_tmp11B;seen_type=1;t=_tmp11A;goto _LL72;_LL75: {
struct Cyc_Parse_Signed_spec_struct*_tmp11C=(struct Cyc_Parse_Signed_spec_struct*)
_tmp118;if(_tmp11C->tag != 0)goto _LL77;else{_tmp11D=_tmp11C->f1;}}_LL76: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp11D);if(seen_type){const char*_tmp92E;
Cyc_Parse_err(((_tmp92E="signed qualifier must come before type",_tag_dyneither(
_tmp92E,sizeof(char),39))),_tmp11D);}last_loc=_tmp11D;seen_sign=1;sgn=Cyc_Absyn_Signed;
goto _LL72;_LL77: {struct Cyc_Parse_Unsigned_spec_struct*_tmp11E=(struct Cyc_Parse_Unsigned_spec_struct*)
_tmp118;if(_tmp11E->tag != 1)goto _LL79;else{_tmp11F=_tmp11E->f1;}}_LL78: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp11F);if(seen_type){const char*_tmp92F;
Cyc_Parse_err(((_tmp92F="signed qualifier must come before type",_tag_dyneither(
_tmp92F,sizeof(char),39))),_tmp11F);}last_loc=_tmp11F;seen_sign=1;sgn=Cyc_Absyn_Unsigned;
goto _LL72;_LL79: {struct Cyc_Parse_Short_spec_struct*_tmp120=(struct Cyc_Parse_Short_spec_struct*)
_tmp118;if(_tmp120->tag != 2)goto _LL7B;else{_tmp121=_tmp120->f1;}}_LL7A: if(
seen_size){const char*_tmp930;Cyc_Parse_err(((_tmp930="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp930,sizeof(char),59))),_tmp121);}if(seen_type){const char*
_tmp931;Cyc_Parse_err(((_tmp931="short modifier must come before base type",
_tag_dyneither(_tmp931,sizeof(char),42))),_tmp121);}last_loc=_tmp121;seen_size=1;
sz=Cyc_Absyn_Short_sz;goto _LL72;_LL7B: {struct Cyc_Parse_Long_spec_struct*_tmp122=(
struct Cyc_Parse_Long_spec_struct*)_tmp118;if(_tmp122->tag != 3)goto _LL72;else{
_tmp123=_tmp122->f1;}}_LL7C: if(seen_type){const char*_tmp932;Cyc_Parse_err(((
_tmp932="long modifier must come before base type",_tag_dyneither(_tmp932,
sizeof(char),41))),_tmp123);}if(seen_size)switch(sz){case Cyc_Absyn_Long_sz: _LL7D:
sz=Cyc_Absyn_LongLong_sz;break;default: _LL7E:{const char*_tmp933;Cyc_Parse_err(((
_tmp933="extra long in type specifier",_tag_dyneither(_tmp933,sizeof(char),29))),
_tmp123);}break;}else{sz=Cyc_Absyn_Long_sz;}last_loc=_tmp123;seen_size=1;goto
_LL72;_LL72:;}if(!seen_type){if(!seen_sign  && !seen_size){const char*_tmp936;void*
_tmp935;(_tmp935=0,Cyc_Tcutil_warn(last_loc,((_tmp936="missing type within specifier",
_tag_dyneither(_tmp936,sizeof(char),30))),_tag_dyneither(_tmp935,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{if(seen_sign){void*_tmp12C=t;enum Cyc_Absyn_Sign
_tmp12E;enum Cyc_Absyn_Size_of _tmp12F;_LL81: {struct Cyc_Absyn_IntType_struct*
_tmp12D=(struct Cyc_Absyn_IntType_struct*)_tmp12C;if(_tmp12D->tag != 6)goto _LL83;
else{_tmp12E=_tmp12D->f1;_tmp12F=_tmp12D->f2;}}_LL82: if(_tmp12E != sgn)t=Cyc_Absyn_int_typ(
sgn,_tmp12F);goto _LL80;_LL83:;_LL84:{const char*_tmp937;Cyc_Parse_err(((_tmp937="sign specification on non-integral type",
_tag_dyneither(_tmp937,sizeof(char),40))),last_loc);}goto _LL80;_LL80:;}if(
seen_size){void*_tmp131=t;enum Cyc_Absyn_Sign _tmp133;enum Cyc_Absyn_Size_of
_tmp134;_LL86: {struct Cyc_Absyn_IntType_struct*_tmp132=(struct Cyc_Absyn_IntType_struct*)
_tmp131;if(_tmp132->tag != 6)goto _LL88;else{_tmp133=_tmp132->f1;_tmp134=_tmp132->f2;}}
_LL87: if(_tmp134 != sz)t=Cyc_Absyn_int_typ(_tmp133,sz);goto _LL85;_LL88: {struct
Cyc_Absyn_DoubleType_struct*_tmp135=(struct Cyc_Absyn_DoubleType_struct*)_tmp131;
if(_tmp135->tag != 8)goto _LL8A;}_LL89: t=Cyc_Absyn_double_typ(1);goto _LL85;_LL8A:;
_LL8B:{const char*_tmp938;Cyc_Parse_err(((_tmp938="size qualifier on non-integral type",
_tag_dyneither(_tmp938,sizeof(char),36))),last_loc);}goto _LL85;_LL85:;}}return t;}
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)
return 0;{struct Cyc_Parse_Declarator*_tmp137=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmp138=_tmp137->id;struct Cyc_Absyn_Tqual _tmp13A;void*_tmp13B;
struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13D;struct _tuple10 _tmp139=
Cyc_Parse_apply_tms(tq,t,shared_atts,_tmp137->tms);_tmp13A=_tmp139.f1;_tmp13B=
_tmp139.f2;_tmp13C=_tmp139.f3;_tmp13D=_tmp139.f4;if(ds->tl == 0){struct _tuple12*
_tmp93B;struct Cyc_List_List*_tmp93A;return(_tmp93A=_region_malloc(r,sizeof(*
_tmp93A)),((_tmp93A->hd=((_tmp93B=_region_malloc(r,sizeof(*_tmp93B)),((_tmp93B->f1=
_tmp138,((_tmp93B->f2=_tmp13A,((_tmp93B->f3=_tmp13B,((_tmp93B->f4=_tmp13C,((
_tmp93B->f5=_tmp13D,_tmp93B)))))))))))),((_tmp93A->tl=0,_tmp93A)))));}else{
struct _tuple12*_tmp93E;struct Cyc_List_List*_tmp93D;return(_tmp93D=_region_malloc(
r,sizeof(*_tmp93D)),((_tmp93D->hd=((_tmp93E=_region_malloc(r,sizeof(*_tmp93E)),((
_tmp93E->f1=_tmp138,((_tmp93E->f2=_tmp13A,((_tmp93E->f3=_tmp13B,((_tmp93E->f4=
_tmp13C,((_tmp93E->f5=_tmp13D,_tmp93E)))))))))))),((_tmp93D->tl=Cyc_Parse_apply_tmss(
r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp93D)))));}};}static struct
_tuple10 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0){struct _tuple10 _tmp93F;return(_tmp93F.f1=
tq,((_tmp93F.f2=t,((_tmp93F.f3=0,((_tmp93F.f4=atts,_tmp93F)))))));}{void*_tmp143=(
void*)tms->hd;union Cyc_Absyn_Constraint*_tmp145;struct Cyc_Position_Segment*
_tmp146;struct Cyc_Absyn_Exp*_tmp148;union Cyc_Absyn_Constraint*_tmp149;struct Cyc_Position_Segment*
_tmp14A;void*_tmp14C;struct Cyc_List_List*_tmp14E;struct Cyc_Position_Segment*
_tmp14F;struct Cyc_Absyn_PtrAtts _tmp151;struct Cyc_Absyn_Tqual _tmp152;struct Cyc_Position_Segment*
_tmp154;struct Cyc_List_List*_tmp155;_LL8D: {struct Cyc_Absyn_Carray_mod_struct*
_tmp144=(struct Cyc_Absyn_Carray_mod_struct*)_tmp143;if(_tmp144->tag != 0)goto
_LL8F;else{_tmp145=_tmp144->f1;_tmp146=_tmp144->f2;}}_LL8E: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmp145,_tmp146),atts,tms->tl);
_LL8F: {struct Cyc_Absyn_ConstArray_mod_struct*_tmp147=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp143;if(_tmp147->tag != 1)goto _LL91;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;
_tmp14A=_tmp147->f3;}}_LL90: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmp148,_tmp149,_tmp14A),atts,tms->tl);
_LL91: {struct Cyc_Absyn_Function_mod_struct*_tmp14B=(struct Cyc_Absyn_Function_mod_struct*)
_tmp143;if(_tmp14B->tag != 3)goto _LL93;else{_tmp14C=(void*)_tmp14B->f1;}}_LL92: {
void*_tmp156=_tmp14C;struct Cyc_List_List*_tmp158;int _tmp159;struct Cyc_Absyn_VarargInfo*
_tmp15A;struct Cyc_Core_Opt*_tmp15B;struct Cyc_List_List*_tmp15C;struct Cyc_Position_Segment*
_tmp15E;_LL9A: {struct Cyc_Absyn_WithTypes_struct*_tmp157=(struct Cyc_Absyn_WithTypes_struct*)
_tmp156;if(_tmp157->tag != 1)goto _LL9C;else{_tmp158=_tmp157->f1;_tmp159=_tmp157->f2;
_tmp15A=_tmp157->f3;_tmp15B=_tmp157->f4;_tmp15C=_tmp157->f5;}}_LL9B: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp940;fn_atts=((_tmp940=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp940)),((_tmp940->hd=(void*)as->hd,((_tmp940->tl=fn_atts,_tmp940))))));}
else{struct Cyc_List_List*_tmp941;new_atts=((_tmp941=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp941)),((_tmp941->hd=(void*)as->hd,((_tmp941->tl=new_atts,_tmp941))))));}}}
if(tms->tl != 0){void*_tmp161=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;
struct Cyc_List_List*_tmp163;_LL9F: {struct Cyc_Absyn_TypeParams_mod_struct*
_tmp162=(struct Cyc_Absyn_TypeParams_mod_struct*)_tmp161;if(_tmp162->tag != 4)goto
_LLA1;else{_tmp163=_tmp162->f1;}}_LLA0: typvars=_tmp163;tms=tms->tl;goto _LL9E;
_LLA1:;_LLA2: goto _LL9E;_LL9E:;}if(((((!_tmp159  && _tmp15A == 0) && _tmp158 != 0)
 && _tmp158->tl == 0) && (*((struct _tuple8*)_tmp158->hd)).f1 == 0) && (*((struct
_tuple8*)_tmp158->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)_tmp158=0;{struct Cyc_List_List*
_tmp164=Cyc_Parse_get_arg_tags(_tmp158);if(_tmp164 != 0)t=Cyc_Parse_substitute_tags(
_tmp164,t);t=Cyc_Parse_array2ptr(t,0);{struct Cyc_List_List*_tmp165=_tmp158;for(0;
_tmp165 != 0;_tmp165=_tmp165->tl){struct _tuple8 _tmp167;struct Cyc_Core_Opt*_tmp168;
struct Cyc_Absyn_Tqual _tmp169;void*_tmp16A;void**_tmp16B;struct _tuple8*_tmp166=(
struct _tuple8*)_tmp165->hd;_tmp167=*_tmp166;_tmp168=_tmp167.f1;_tmp169=_tmp167.f2;
_tmp16A=_tmp167.f3;_tmp16B=(void**)&(*_tmp166).f3;if(_tmp164 != 0)*_tmp16B=Cyc_Parse_substitute_tags(
_tmp164,*_tmp16B);*_tmp16B=Cyc_Parse_array2ptr(*_tmp16B,1);}}return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(tq.loc),Cyc_Absyn_function_typ(typvars,_tmp15B,tq,t,
_tmp158,_tmp159,_tmp15A,_tmp15C,fn_atts),new_atts,((struct Cyc_List_List*)
_check_null(tms))->tl);};}_LL9C: {struct Cyc_Absyn_NoTypes_struct*_tmp15D=(struct
Cyc_Absyn_NoTypes_struct*)_tmp156;if(_tmp15D->tag != 0)goto _LL99;else{_tmp15E=
_tmp15D->f2;}}_LL9D: {const char*_tmp944;void*_tmp943;(_tmp943=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
_tmp15E,((_tmp944="function declaration without parameter types",_tag_dyneither(
_tmp944,sizeof(char),45))),_tag_dyneither(_tmp943,sizeof(void*),0)));}_LL99:;}
_LL93: {struct Cyc_Absyn_TypeParams_mod_struct*_tmp14D=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp143;if(_tmp14D->tag != 4)goto _LL95;else{_tmp14E=_tmp14D->f1;_tmp14F=_tmp14D->f2;}}
_LL94: if(tms->tl == 0){struct _tuple10 _tmp945;return(_tmp945.f1=tq,((_tmp945.f2=t,((
_tmp945.f3=_tmp14E,((_tmp945.f4=atts,_tmp945)))))));}{const char*_tmp948;void*
_tmp947;(_tmp947=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp14F,((_tmp948="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp948,sizeof(char),68))),_tag_dyneither(_tmp947,sizeof(void*),0)));};
_LL95: {struct Cyc_Absyn_Pointer_mod_struct*_tmp150=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp143;if(_tmp150->tag != 2)goto _LL97;else{_tmp151=_tmp150->f1;_tmp152=_tmp150->f2;}}
_LL96: {struct Cyc_Absyn_PointerType_struct _tmp94E;struct Cyc_Absyn_PtrInfo _tmp94D;
struct Cyc_Absyn_PointerType_struct*_tmp94C;return Cyc_Parse_apply_tms(_tmp152,(
void*)((_tmp94C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp94C)),((
_tmp94C[0]=((_tmp94E.tag=5,((_tmp94E.f1=((_tmp94D.elt_typ=t,((_tmp94D.elt_tq=tq,((
_tmp94D.ptr_atts=_tmp151,_tmp94D)))))),_tmp94E)))),_tmp94C)))),atts,tms->tl);}
_LL97: {struct Cyc_Absyn_Attributes_mod_struct*_tmp153=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmp143;if(_tmp153->tag != 5)goto _LL8C;else{_tmp154=_tmp153->f1;_tmp155=_tmp153->f2;}}
_LL98: return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp155),tms->tl);_LL8C:;};}void*
Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}static struct Cyc_Absyn_Decl*
Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*loc,struct _tuple12*t){
struct _tuple12 _tmp175;struct _tuple1*_tmp176;struct Cyc_Absyn_Tqual _tmp177;void*
_tmp178;struct Cyc_List_List*_tmp179;struct Cyc_List_List*_tmp17A;struct _tuple12*
_tmp174=t;_tmp175=*_tmp174;_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;_tmp178=_tmp175.f3;
_tmp179=_tmp175.f4;_tmp17A=_tmp175.f5;Cyc_Lex_register_typedef(_tmp176);{struct
Cyc_Core_Opt*kind;struct Cyc_Core_Opt*type;{void*_tmp17B=_tmp178;struct Cyc_Core_Opt*
_tmp17D;_LLA4: {struct Cyc_Absyn_Evar_struct*_tmp17C=(struct Cyc_Absyn_Evar_struct*)
_tmp17B;if(_tmp17C->tag != 1)goto _LLA6;else{_tmp17D=_tmp17C->f1;}}_LLA5: type=0;
if(_tmp17D == 0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;else{kind=_tmp17D;}goto
_LLA3;_LLA6:;_LLA7: kind=0;{struct Cyc_Core_Opt*_tmp94F;type=((_tmp94F=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp94F)),((_tmp94F->v=_tmp178,
_tmp94F))));}goto _LLA3;_LLA3:;}{struct Cyc_Absyn_Typedef_d_struct _tmp955;struct
Cyc_Absyn_Typedefdecl*_tmp954;struct Cyc_Absyn_Typedef_d_struct*_tmp953;return Cyc_Absyn_new_decl((
void*)((_tmp953=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp953)),((
_tmp953[0]=((_tmp955.tag=9,((_tmp955.f1=((_tmp954=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp954)),((_tmp954->name=_tmp176,((_tmp954->tvs=_tmp179,((_tmp954->kind=
kind,((_tmp954->defn=type,((_tmp954->atts=_tmp17A,((_tmp954->tq=_tmp177,_tmp954)))))))))))))),
_tmp955)))),_tmp953)))),loc);};};}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Decl_s_struct
_tmp958;struct Cyc_Absyn_Decl_s_struct*_tmp957;return Cyc_Absyn_new_stmt((void*)((
_tmp957=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp957)),((_tmp957[0]=((
_tmp958.tag=12,((_tmp958.f1=d,((_tmp958.f2=s,_tmp958)))))),_tmp957)))),d->loc);}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,
struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*
f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*
accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*
Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*
ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*loc){struct
_RegionHandle _tmp184=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp184;
_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp186;struct Cyc_Absyn_Tqual
_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_List_List*_tmp189;struct Cyc_Parse_Declaration_spec*
_tmp185=ds;_tmp186=*_tmp185;_tmp187=_tmp186.tq;_tmp188=_tmp186.type_specs;
_tmp189=_tmp186.attributes;if(_tmp187.loc == 0)_tmp187.loc=tqual_loc;if(ds->is_inline){
const char*_tmp959;Cyc_Parse_err(((_tmp959="inline is allowed only on function definitions",
_tag_dyneither(_tmp959,sizeof(char),47))),loc);}if(_tmp188 == 0){{const char*
_tmp95C;void*_tmp95B;(_tmp95B=0,Cyc_Tcutil_warn(loc,((_tmp95C="missing type specifiers in declaration, assuming int",
_tag_dyneither(_tmp95C,sizeof(char),53))),_tag_dyneither(_tmp95B,sizeof(void*),0)));}{
void*_tmp95D[1];_tmp188=((_tmp95D[0]=Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,loc),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp95D,sizeof(void*),1))));};}{enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;int
istypedef=0;if(ds->sc != 0)switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v){case Cyc_Parse_Typedef_sc: _LLA8: istypedef=1;break;case
Cyc_Parse_Extern_sc: _LLA9: s=Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc:
_LLAA: s=Cyc_Absyn_ExternC;break;case Cyc_Parse_Static_sc: _LLAB: s=Cyc_Absyn_Static;
break;case Cyc_Parse_Auto_sc: _LLAC: s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc:
_LLAD: if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}
break;case Cyc_Parse_Abstract_sc: _LLAE: s=Cyc_Absyn_Abstract;break;}{struct Cyc_List_List*
_tmp18F;struct Cyc_List_List*_tmp190;struct _tuple0 _tmp18E=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
mkrgn,mkrgn,ids);_tmp18F=_tmp18E.f1;_tmp190=_tmp18E.f2;{int exps_empty=1;{struct
Cyc_List_List*es=_tmp190;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd
!= 0){exps_empty=0;break;}}}{void*_tmp191=Cyc_Parse_collapse_type_specifiers(
_tmp188,loc);if(_tmp18F == 0){void*_tmp192=_tmp191;struct Cyc_Absyn_TypeDecl*
_tmp194;struct Cyc_Absyn_TypeDecl _tmp195;void*_tmp196;struct Cyc_Absyn_Aggrdecl*
_tmp198;struct Cyc_Absyn_TypeDecl*_tmp19A;struct Cyc_Absyn_TypeDecl _tmp19B;void*
_tmp19C;struct Cyc_Absyn_Enumdecl*_tmp19E;struct Cyc_Absyn_TypeDecl*_tmp1A0;struct
Cyc_Absyn_TypeDecl _tmp1A1;void*_tmp1A2;struct Cyc_Absyn_Datatypedecl*_tmp1A4;
struct Cyc_Absyn_AggrInfo _tmp1A6;union Cyc_Absyn_AggrInfoU _tmp1A7;struct _tuple3
_tmp1A8;enum Cyc_Absyn_AggrKind _tmp1A9;struct _tuple1*_tmp1AA;struct Cyc_List_List*
_tmp1AB;struct Cyc_Absyn_DatatypeInfo _tmp1AD;union Cyc_Absyn_DatatypeInfoU _tmp1AE;
struct Cyc_Absyn_Datatypedecl**_tmp1AF;struct Cyc_Absyn_DatatypeInfo _tmp1B1;union
Cyc_Absyn_DatatypeInfoU _tmp1B2;struct Cyc_Absyn_UnknownDatatypeInfo _tmp1B3;struct
_tuple1*_tmp1B4;int _tmp1B5;struct Cyc_List_List*_tmp1B6;struct _tuple1*_tmp1B8;
struct Cyc_List_List*_tmp1BA;_LLB1: {struct Cyc_Absyn_TypeDeclType_struct*_tmp193=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp192;if(_tmp193->tag != 27)goto _LLB3;
else{_tmp194=_tmp193->f1;_tmp195=*_tmp194;_tmp196=_tmp195.r;{struct Cyc_Absyn_Aggr_td_struct*
_tmp197=(struct Cyc_Absyn_Aggr_td_struct*)_tmp196;if(_tmp197->tag != 0)goto _LLB3;
else{_tmp198=_tmp197->f1;}};}}_LLB2: _tmp198->attributes=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp198->attributes,
_tmp189);{struct Cyc_Absyn_Aggr_d_struct*_tmp963;struct Cyc_Absyn_Aggr_d_struct
_tmp962;struct Cyc_List_List*_tmp961;struct Cyc_List_List*_tmp1BE=(_tmp961=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp961)),((_tmp961->hd=Cyc_Absyn_new_decl((
void*)((_tmp963=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp963)),((
_tmp963[0]=((_tmp962.tag=6,((_tmp962.f1=_tmp198,_tmp962)))),_tmp963)))),loc),((
_tmp961->tl=0,_tmp961)))));_npop_handler(0);return _tmp1BE;};_LLB3: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp199=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp192;if(_tmp199->tag != 27)goto
_LLB5;else{_tmp19A=_tmp199->f1;_tmp19B=*_tmp19A;_tmp19C=_tmp19B.r;{struct Cyc_Absyn_Enum_td_struct*
_tmp19D=(struct Cyc_Absyn_Enum_td_struct*)_tmp19C;if(_tmp19D->tag != 1)goto _LLB5;
else{_tmp19E=_tmp19D->f1;}};}}_LLB4: if(_tmp189 != 0){const char*_tmp964;Cyc_Parse_err(((
_tmp964="attributes on enum not supported",_tag_dyneither(_tmp964,sizeof(char),
33))),loc);}{struct Cyc_Absyn_Enum_d_struct*_tmp96A;struct Cyc_Absyn_Enum_d_struct
_tmp969;struct Cyc_List_List*_tmp968;struct Cyc_List_List*_tmp1C3=(_tmp968=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp968)),((_tmp968->hd=Cyc_Absyn_new_decl((
void*)((_tmp96A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp96A)),((
_tmp96A[0]=((_tmp969.tag=8,((_tmp969.f1=_tmp19E,_tmp969)))),_tmp96A)))),loc),((
_tmp968->tl=0,_tmp968)))));_npop_handler(0);return _tmp1C3;};_LLB5: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp19F=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp192;if(_tmp19F->tag != 27)goto
_LLB7;else{_tmp1A0=_tmp19F->f1;_tmp1A1=*_tmp1A0;_tmp1A2=_tmp1A1.r;{struct Cyc_Absyn_Datatype_td_struct*
_tmp1A3=(struct Cyc_Absyn_Datatype_td_struct*)_tmp1A2;if(_tmp1A3->tag != 2)goto
_LLB7;else{_tmp1A4=_tmp1A3->f1;}};}}_LLB6: if(_tmp189 != 0){const char*_tmp96B;Cyc_Parse_err(((
_tmp96B="attributes on datatypes not supported",_tag_dyneither(_tmp96B,sizeof(
char),38))),loc);}{struct Cyc_Absyn_Datatype_d_struct*_tmp971;struct Cyc_Absyn_Datatype_d_struct
_tmp970;struct Cyc_List_List*_tmp96F;struct Cyc_List_List*_tmp1C8=(_tmp96F=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp96F)),((_tmp96F->hd=Cyc_Absyn_new_decl((
void*)((_tmp971=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp971)),((
_tmp971[0]=((_tmp970.tag=7,((_tmp970.f1=_tmp1A4,_tmp970)))),_tmp971)))),loc),((
_tmp96F->tl=0,_tmp96F)))));_npop_handler(0);return _tmp1C8;};_LLB7: {struct Cyc_Absyn_AggrType_struct*
_tmp1A5=(struct Cyc_Absyn_AggrType_struct*)_tmp192;if(_tmp1A5->tag != 12)goto _LLB9;
else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A6.aggr_info;if((_tmp1A7.UnknownAggr).tag
!= 1)goto _LLB9;_tmp1A8=(struct _tuple3)(_tmp1A7.UnknownAggr).val;_tmp1A9=_tmp1A8.f1;
_tmp1AA=_tmp1A8.f2;_tmp1AB=_tmp1A6.targs;}}_LLB8: {struct Cyc_List_List*_tmp1C9=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp1AB);struct Cyc_Absyn_Aggrdecl*_tmp972;struct Cyc_Absyn_Aggrdecl*_tmp1CA=(
_tmp972=_cycalloc(sizeof(*_tmp972)),((_tmp972->kind=_tmp1A9,((_tmp972->sc=s,((
_tmp972->name=_tmp1AA,((_tmp972->tvs=_tmp1C9,((_tmp972->impl=0,((_tmp972->attributes=
0,_tmp972)))))))))))));if(_tmp189 != 0){const char*_tmp973;Cyc_Parse_err(((_tmp973="bad attributes on type declaration",
_tag_dyneither(_tmp973,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp979;struct Cyc_Absyn_Aggr_d_struct _tmp978;struct Cyc_List_List*_tmp977;struct
Cyc_List_List*_tmp1CF=(_tmp977=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp977)),((_tmp977->hd=Cyc_Absyn_new_decl((void*)((_tmp979=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp979)),((_tmp979[0]=((_tmp978.tag=6,((_tmp978.f1=_tmp1CA,_tmp978)))),
_tmp979)))),loc),((_tmp977->tl=0,_tmp977)))));_npop_handler(0);return _tmp1CF;};}
_LLB9: {struct Cyc_Absyn_DatatypeType_struct*_tmp1AC=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp192;if(_tmp1AC->tag != 3)goto _LLBB;else{_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AD.datatype_info;
if((_tmp1AE.KnownDatatype).tag != 2)goto _LLBB;_tmp1AF=(struct Cyc_Absyn_Datatypedecl**)(
_tmp1AE.KnownDatatype).val;}}_LLBA: if(_tmp189 != 0){const char*_tmp97A;Cyc_Parse_err(((
_tmp97A="bad attributes on datatype",_tag_dyneither(_tmp97A,sizeof(char),27))),
loc);}{struct Cyc_Absyn_Datatype_d_struct*_tmp980;struct Cyc_Absyn_Datatype_d_struct
_tmp97F;struct Cyc_List_List*_tmp97E;struct Cyc_List_List*_tmp1D5=(_tmp97E=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp97E)),((_tmp97E->hd=Cyc_Absyn_new_decl((
void*)((_tmp980=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp980)),((
_tmp980[0]=((_tmp97F.tag=7,((_tmp97F.f1=*_tmp1AF,_tmp97F)))),_tmp980)))),loc),((
_tmp97E->tl=0,_tmp97E)))));_npop_handler(0);return _tmp1D5;};_LLBB: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1B0=(struct Cyc_Absyn_DatatypeType_struct*)_tmp192;if(_tmp1B0->tag != 3)goto
_LLBD;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B1.datatype_info;if((_tmp1B2.UnknownDatatype).tag
!= 1)goto _LLBD;_tmp1B3=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B2.UnknownDatatype).val;
_tmp1B4=_tmp1B3.name;_tmp1B5=_tmp1B3.is_extensible;_tmp1B6=_tmp1B1.targs;}}_LLBC: {
struct Cyc_List_List*_tmp1D6=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp1B6);struct Cyc_Absyn_Decl*_tmp1D7=
Cyc_Absyn_datatype_decl(s,_tmp1B4,_tmp1D6,0,_tmp1B5,loc);if(_tmp189 != 0){const
char*_tmp981;Cyc_Parse_err(((_tmp981="bad attributes on datatype",_tag_dyneither(
_tmp981,sizeof(char),27))),loc);}{struct Cyc_List_List*_tmp982;struct Cyc_List_List*
_tmp1DA=(_tmp982=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp982)),((
_tmp982->hd=_tmp1D7,((_tmp982->tl=0,_tmp982)))));_npop_handler(0);return _tmp1DA;};}
_LLBD: {struct Cyc_Absyn_EnumType_struct*_tmp1B7=(struct Cyc_Absyn_EnumType_struct*)
_tmp192;if(_tmp1B7->tag != 14)goto _LLBF;else{_tmp1B8=_tmp1B7->f1;}}_LLBE: {struct
Cyc_Absyn_Enumdecl*_tmp983;struct Cyc_Absyn_Enumdecl*_tmp1DB=(_tmp983=_cycalloc(
sizeof(*_tmp983)),((_tmp983->sc=s,((_tmp983->name=_tmp1B8,((_tmp983->fields=0,
_tmp983)))))));if(_tmp189 != 0){const char*_tmp984;Cyc_Parse_err(((_tmp984="bad attributes on enum",
_tag_dyneither(_tmp984,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp98E;
struct Cyc_Absyn_Enum_d_struct _tmp98D;struct Cyc_Absyn_Enum_d_struct*_tmp98C;
struct Cyc_List_List*_tmp98B;struct Cyc_List_List*_tmp1E1=(_tmp98B=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp98B)),((_tmp98B->hd=((_tmp98E=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp98E)),((_tmp98E->r=(void*)((_tmp98C=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98D.tag=
8,((_tmp98D.f1=_tmp1DB,_tmp98D)))),_tmp98C)))),((_tmp98E->loc=loc,_tmp98E)))))),((
_tmp98B->tl=0,_tmp98B)))));_npop_handler(0);return _tmp1E1;};}_LLBF: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1B9=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp192;if(_tmp1B9->tag != 15)goto
_LLC1;else{_tmp1BA=_tmp1B9->f1;}}_LLC0: {struct Cyc_Core_Opt*_tmp991;struct Cyc_Absyn_Enumdecl*
_tmp990;struct Cyc_Absyn_Enumdecl*_tmp1E3=(_tmp990=_cycalloc(sizeof(*_tmp990)),((
_tmp990->sc=s,((_tmp990->name=Cyc_Parse_gensym_enum(),((_tmp990->fields=((
_tmp991=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp991)),((_tmp991->v=
_tmp1BA,_tmp991)))),_tmp990)))))));if(_tmp189 != 0){const char*_tmp992;Cyc_Parse_err(((
_tmp992="bad attributes on enum",_tag_dyneither(_tmp992,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp99C;struct Cyc_Absyn_Enum_d_struct _tmp99B;struct Cyc_Absyn_Enum_d_struct*
_tmp99A;struct Cyc_List_List*_tmp999;struct Cyc_List_List*_tmp1E9=(_tmp999=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp999)),((_tmp999->hd=((_tmp99C=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp99C)),((_tmp99C->r=(void*)((
_tmp99A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp99A)),((_tmp99A[0]=((
_tmp99B.tag=8,((_tmp99B.f1=_tmp1E3,_tmp99B)))),_tmp99A)))),((_tmp99C->loc=loc,
_tmp99C)))))),((_tmp999->tl=0,_tmp999)))));_npop_handler(0);return _tmp1E9;};}
_LLC1:;_LLC2:{const char*_tmp99D;Cyc_Parse_err(((_tmp99D="missing declarator",
_tag_dyneither(_tmp99D,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1ED=0;
_npop_handler(0);return _tmp1ED;};_LLB0:;}else{struct Cyc_List_List*_tmp1EE=Cyc_Parse_apply_tmss(
mkrgn,_tmp187,_tmp191,_tmp18F,_tmp189);if(istypedef){if(!exps_empty){const char*
_tmp99E;Cyc_Parse_err(((_tmp99E="initializer in typedef declaration",
_tag_dyneither(_tmp99E,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,
struct _tuple12*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_v_typ_to_typedef,loc,_tmp1EE);struct Cyc_List_List*_tmp1F0=decls;
_npop_handler(0);return _tmp1F0;};}else{struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp1F1=_tmp1EE;for(0;_tmp1F1 != 0;(_tmp1F1=_tmp1F1->tl,_tmp190=_tmp190->tl)){
struct _tuple12 _tmp1F3;struct _tuple1*_tmp1F4;struct Cyc_Absyn_Tqual _tmp1F5;void*
_tmp1F6;struct Cyc_List_List*_tmp1F7;struct Cyc_List_List*_tmp1F8;struct _tuple12*
_tmp1F2=(struct _tuple12*)_tmp1F1->hd;_tmp1F3=*_tmp1F2;_tmp1F4=_tmp1F3.f1;_tmp1F5=
_tmp1F3.f2;_tmp1F6=_tmp1F3.f3;_tmp1F7=_tmp1F3.f4;_tmp1F8=_tmp1F3.f5;if(_tmp1F7 != 
0){const char*_tmp9A1;void*_tmp9A0;(_tmp9A0=0,Cyc_Tcutil_warn(loc,((_tmp9A1="bad type params, ignoring",
_tag_dyneither(_tmp9A1,sizeof(char),26))),_tag_dyneither(_tmp9A0,sizeof(void*),0)));}
if(_tmp190 == 0){const char*_tmp9A4;void*_tmp9A3;(_tmp9A3=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp9A4="unexpected NULL in parse!",_tag_dyneither(_tmp9A4,sizeof(char),26))),
_tag_dyneither(_tmp9A3,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*_tmp1FD=(struct
Cyc_Absyn_Exp*)_tmp190->hd;struct Cyc_Absyn_Vardecl*_tmp1FE=Cyc_Absyn_new_vardecl(
_tmp1F4,_tmp1F6,_tmp1FD);_tmp1FE->tq=_tmp1F5;_tmp1FE->sc=s;_tmp1FE->attributes=
_tmp1F8;{struct Cyc_Absyn_Var_d_struct*_tmp9AA;struct Cyc_Absyn_Var_d_struct
_tmp9A9;struct Cyc_Absyn_Decl*_tmp9A8;struct Cyc_Absyn_Decl*_tmp1FF=(_tmp9A8=
_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8->r=(void*)((_tmp9AA=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9A9.tag=0,((_tmp9A9.f1=_tmp1FE,_tmp9A9)))),
_tmp9AA)))),((_tmp9A8->loc=loc,_tmp9A8)))));struct Cyc_List_List*_tmp9AB;decls=((
_tmp9AB=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp9AB)),((_tmp9AB->hd=
_tmp1FF,((_tmp9AB->tl=decls,_tmp9AB))))));};};}}{struct Cyc_List_List*_tmp204=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);
_npop_handler(0);return _tmp204;};}}};};};};};_pop_region(mkrgn);}static struct Cyc_Absyn_Kind*
Cyc_Parse_id_to_kind(struct _dyneither_ptr s,struct Cyc_Position_Segment*loc){if(
Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 
2)switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),0))){case 'A':
_LLC3: return& Cyc_Tcutil_ak;case 'M': _LLC4: return& Cyc_Tcutil_mk;case 'B': _LLC5:
return& Cyc_Tcutil_bk;case 'R': _LLC6: return& Cyc_Tcutil_rk;case 'E': _LLC7: return& Cyc_Tcutil_ek;
case 'I': _LLC8: return& Cyc_Tcutil_ik;case 'U': _LLC9: switch(*((const char*)
_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLCB: return& Cyc_Tcutil_urk;
case 'A': _LLCC: return& Cyc_Tcutil_uak;case 'M': _LLCD: return& Cyc_Tcutil_umk;case 'B':
_LLCE: return& Cyc_Tcutil_ubk;default: _LLCF: break;}break;case 'T': _LLCA: switch(*((
const char*)_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLD2: return&
Cyc_Tcutil_trk;case 'A': _LLD3: return& Cyc_Tcutil_tak;case 'M': _LLD4: return& Cyc_Tcutil_tmk;
case 'B': _LLD5: return& Cyc_Tcutil_tbk;default: _LLD6: break;}break;default: _LLD1:
break;}{const char*_tmp9B0;void*_tmp9AF[2];struct Cyc_String_pa_struct _tmp9AE;
struct Cyc_Int_pa_struct _tmp9AD;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9AD.tag=
1,((_tmp9AD.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((
_tmp9AE.tag=0,((_tmp9AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp9AF[0]=& _tmp9AE,((_tmp9AF[1]=& _tmp9AD,Cyc_aprintf(((_tmp9B0="bad kind: %s; strlen=%d",
_tag_dyneither(_tmp9B0,sizeof(char),24))),_tag_dyneither(_tmp9AF,sizeof(void*),2)))))))))))))),
loc);}return& Cyc_Tcutil_bk;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct
Cyc_Absyn_Pat*p){void*_tmp209=p->r;struct _tuple1*_tmp20B;struct Cyc_Absyn_Vardecl*
_tmp20D;struct Cyc_Absyn_Pat*_tmp20E;struct Cyc_Absyn_Pat _tmp20F;void*_tmp210;
struct Cyc_Absyn_Pat*_tmp213;enum Cyc_Absyn_Sign _tmp216;int _tmp217;char _tmp219;
struct _dyneither_ptr _tmp21B;struct _tuple1*_tmp21D;struct Cyc_List_List*_tmp21E;
int _tmp21F;struct Cyc_Absyn_Exp*_tmp221;_LLDA: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp20A=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp209;if(_tmp20A->tag != 14)goto
_LLDC;else{_tmp20B=_tmp20A->f1;}}_LLDB: {struct Cyc_Absyn_UnknownId_e_struct
_tmp9B3;struct Cyc_Absyn_UnknownId_e_struct*_tmp9B2;return Cyc_Absyn_new_exp((void*)((
_tmp9B2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp9B2)),((_tmp9B2[0]=((
_tmp9B3.tag=2,((_tmp9B3.f1=_tmp20B,_tmp9B3)))),_tmp9B2)))),p->loc);}_LLDC: {
struct Cyc_Absyn_Reference_p_struct*_tmp20C=(struct Cyc_Absyn_Reference_p_struct*)
_tmp209;if(_tmp20C->tag != 2)goto _LLDE;else{_tmp20D=_tmp20C->f1;_tmp20E=_tmp20C->f2;
_tmp20F=*_tmp20E;_tmp210=_tmp20F.r;{struct Cyc_Absyn_Wild_p_struct*_tmp211=(
struct Cyc_Absyn_Wild_p_struct*)_tmp210;if(_tmp211->tag != 0)goto _LLDE;};}}_LLDD: {
struct Cyc_Absyn_UnknownId_e_struct _tmp9B6;struct Cyc_Absyn_UnknownId_e_struct*
_tmp9B5;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmp9B5=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp9B5)),((_tmp9B5[0]=((_tmp9B6.tag=
2,((_tmp9B6.f1=_tmp20D->name,_tmp9B6)))),_tmp9B5)))),p->loc),p->loc);}_LLDE: {
struct Cyc_Absyn_Pointer_p_struct*_tmp212=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp209;if(_tmp212->tag != 5)goto _LLE0;else{_tmp213=_tmp212->f1;}}_LLDF: return Cyc_Absyn_address_exp(
Cyc_Parse_pat2exp(_tmp213),p->loc);_LLE0: {struct Cyc_Absyn_Null_p_struct*_tmp214=(
struct Cyc_Absyn_Null_p_struct*)_tmp209;if(_tmp214->tag != 8)goto _LLE2;}_LLE1:
return Cyc_Absyn_null_exp(p->loc);_LLE2: {struct Cyc_Absyn_Int_p_struct*_tmp215=(
struct Cyc_Absyn_Int_p_struct*)_tmp209;if(_tmp215->tag != 9)goto _LLE4;else{_tmp216=
_tmp215->f1;_tmp217=_tmp215->f2;}}_LLE3: return Cyc_Absyn_int_exp(_tmp216,_tmp217,
p->loc);_LLE4: {struct Cyc_Absyn_Char_p_struct*_tmp218=(struct Cyc_Absyn_Char_p_struct*)
_tmp209;if(_tmp218->tag != 10)goto _LLE6;else{_tmp219=_tmp218->f1;}}_LLE5: return
Cyc_Absyn_char_exp(_tmp219,p->loc);_LLE6: {struct Cyc_Absyn_Float_p_struct*
_tmp21A=(struct Cyc_Absyn_Float_p_struct*)_tmp209;if(_tmp21A->tag != 11)goto _LLE8;
else{_tmp21B=_tmp21A->f1;}}_LLE7: return Cyc_Absyn_float_exp(_tmp21B,p->loc);_LLE8: {
struct Cyc_Absyn_UnknownCall_p_struct*_tmp21C=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp209;if(_tmp21C->tag != 15)goto _LLEA;else{_tmp21D=_tmp21C->f1;_tmp21E=_tmp21C->f2;
_tmp21F=_tmp21C->f3;if(_tmp21F != 0)goto _LLEA;}}_LLE9: {struct Cyc_Absyn_UnknownId_e_struct
_tmp9B9;struct Cyc_Absyn_UnknownId_e_struct*_tmp9B8;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmp9B8=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp9B8)),((
_tmp9B8[0]=((_tmp9B9.tag=2,((_tmp9B9.f1=_tmp21D,_tmp9B9)))),_tmp9B8)))),p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct
Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp21E);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}_LLEA: {struct Cyc_Absyn_Exp_p_struct*
_tmp220=(struct Cyc_Absyn_Exp_p_struct*)_tmp209;if(_tmp220->tag != 16)goto _LLEC;
else{_tmp221=_tmp220->f1;}}_LLEB: return _tmp221;_LLEC:;_LLED:{const char*_tmp9BA;
Cyc_Parse_err(((_tmp9BA="cannot mix patterns and expressions in case",
_tag_dyneither(_tmp9BA,sizeof(char),44))),p->loc);}return Cyc_Absyn_null_exp(p->loc);
_LLD9:;}union Cyc_YYSTYPE;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple6 val;};
struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{
int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;
struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*
val;};struct _tuple17{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{
int tag;struct _tuple17 val;};struct _tuple18{struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*
f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple18*
val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple19{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple19*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple20*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Parse_Declaration_spec*val;};
struct _tuple21{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Parse_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Parse_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator*val;};
struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum 
Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*
val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple22{
struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple23{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple23*val;};
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
_union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp22A[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp22A,
_tmp22A,_tmp22A + 14}};struct _tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){struct
_tuple6 yyzzz;{union Cyc_YYSTYPE _tmp22B=yy1;struct _tuple6 _tmp22C;_LLEF: if((_tmp22B.Int_tok).tag
!= 1)goto _LLF1;_tmp22C=(struct _tuple6)(_tmp22B.Int_tok).val;_LLF0: yyzzz=_tmp22C;
goto _LLEE;_LLF1:;_LLF2:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLEE:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){union Cyc_YYSTYPE _tmp9BB;
return((_tmp9BB.Int_tok).val=yy1,(((_tmp9BB.Int_tok).tag=1,_tmp9BB)));}static
char _tmp22F[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp22F,_tmp22F,_tmp22F + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE
yy1){char yyzzz;{union Cyc_YYSTYPE _tmp230=yy1;char _tmp231;_LLF4: if((_tmp230.Char_tok).tag
!= 2)goto _LLF6;_tmp231=(char)(_tmp230.Char_tok).val;_LLF5: yyzzz=_tmp231;goto
_LLF3;_LLF6:;_LLF7:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLF3:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9BC;return((_tmp9BC.Char_tok).val=
yy1,(((_tmp9BC.Char_tok).tag=2,_tmp9BC)));}static char _tmp234[13]="string_t<`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{
_tmp234,_tmp234,_tmp234 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp235=yy1;struct _dyneither_ptr
_tmp236;_LLF9: if((_tmp235.String_tok).tag != 3)goto _LLFB;_tmp236=(struct
_dyneither_ptr)(_tmp235.String_tok).val;_LLFA: yyzzz=_tmp236;goto _LLF8;_LLFB:;
_LLFC:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLF8:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9BD;return((_tmp9BD.String_tok).val=
yy1,(((_tmp9BD.String_tok).tag=3,_tmp9BD)));}static char _tmp239[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp239,
_tmp239,_tmp239 + 56}};struct _tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct
_tuple18*yyzzz;{union Cyc_YYSTYPE _tmp23A=yy1;struct _tuple18*_tmp23B;_LLFE: if((
_tmp23A.YY1).tag != 7)goto _LL100;_tmp23B=(struct _tuple18*)(_tmp23A.YY1).val;_LLFF:
yyzzz=_tmp23B;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)& Cyc_yyfail_YY1);_LLFD:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY1(struct _tuple18*yy1){union Cyc_YYSTYPE _tmp9BE;
return((_tmp9BE.YY1).val=yy1,(((_tmp9BE.YY1).tag=7,_tmp9BE)));}static char _tmp23E[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp23E,_tmp23E,_tmp23E + 19}};union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE
yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp23F=yy1;union Cyc_Absyn_Constraint*
_tmp240;_LL103: if((_tmp23F.YY2).tag != 8)goto _LL105;_tmp240=(union Cyc_Absyn_Constraint*)(
_tmp23F.YY2).val;_LL104: yyzzz=_tmp240;goto _LL102;_LL105:;_LL106:(int)_throw((
void*)& Cyc_yyfail_YY2);_LL102:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*
yy1){union Cyc_YYSTYPE _tmp9BF;return((_tmp9BF.YY2).val=yy1,(((_tmp9BF.YY2).tag=8,
_tmp9BF)));}static char _tmp243[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE _tmp244=yy1;
struct Cyc_Absyn_Exp*_tmp245;_LL108: if((_tmp244.YY3).tag != 9)goto _LL10A;_tmp245=(
struct Cyc_Absyn_Exp*)(_tmp244.YY3).val;_LL109: yyzzz=_tmp245;goto _LL107;_LL10A:;
_LL10B:(int)_throw((void*)& Cyc_yyfail_YY3);_LL107:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY3(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9C0;return((_tmp9C0.YY3).val=
yy1,(((_tmp9C0.YY3).tag=9,_tmp9C0)));}static char _tmp248[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp248,_tmp248,
_tmp248 + 10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE _tmp249=yy1;struct Cyc_Absyn_Exp*_tmp24A;_LL10D: if((
_tmp249.YY4).tag != 10)goto _LL10F;_tmp24A=(struct Cyc_Absyn_Exp*)(_tmp249.YY4).val;
_LL10E: yyzzz=_tmp24A;goto _LL10C;_LL10F:;_LL110:(int)_throw((void*)& Cyc_yyfail_YY4);
_LL10C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE
_tmp9C1;return((_tmp9C1.YY4).val=yy1,(((_tmp9C1.YY4).tag=10,_tmp9C1)));}static
char _tmp24D[17]="list_t<exp_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={
Cyc_Core_Failure,{_tmp24D,_tmp24D,_tmp24D + 17}};struct Cyc_List_List*Cyc_yyget_YY5(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp24E=yy1;
struct Cyc_List_List*_tmp24F;_LL112: if((_tmp24E.YY5).tag != 11)goto _LL114;_tmp24F=(
struct Cyc_List_List*)(_tmp24E.YY5).val;_LL113: yyzzz=_tmp24F;goto _LL111;_LL114:;
_LL115:(int)_throw((void*)& Cyc_yyfail_YY5);_LL111:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C2;return((_tmp9C2.YY5).val=
yy1,(((_tmp9C2.YY5).tag=11,_tmp9C2)));}static char _tmp252[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp252,
_tmp252,_tmp252 + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp253=yy1;struct Cyc_List_List*
_tmp254;_LL117: if((_tmp253.YY6).tag != 12)goto _LL119;_tmp254=(struct Cyc_List_List*)(
_tmp253.YY6).val;_LL118: yyzzz=_tmp254;goto _LL116;_LL119:;_LL11A:(int)_throw((
void*)& Cyc_yyfail_YY6);_LL116:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9C3;return((_tmp9C3.YY6).val=yy1,(((_tmp9C3.YY6).tag=12,
_tmp9C3)));}static char _tmp257[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp257,_tmp257,_tmp257 + 9}};enum Cyc_Absyn_Primop
Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Primop yyzzz;{union Cyc_YYSTYPE
_tmp258=yy1;enum Cyc_Absyn_Primop _tmp259;_LL11C: if((_tmp258.YY7).tag != 13)goto
_LL11E;_tmp259=(enum Cyc_Absyn_Primop)(_tmp258.YY7).val;_LL11D: yyzzz=_tmp259;
goto _LL11B;_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY7);_LL11B:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9C4;
return((_tmp9C4.YY7).val=yy1,(((_tmp9C4.YY7).tag=13,_tmp9C4)));}static char
_tmp25C[19]="opt_t<primop_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={
Cyc_Core_Failure,{_tmp25C,_tmp25C,_tmp25C + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(
union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp25D=yy1;
struct Cyc_Core_Opt*_tmp25E;_LL121: if((_tmp25D.YY8).tag != 14)goto _LL123;_tmp25E=(
struct Cyc_Core_Opt*)(_tmp25D.YY8).val;_LL122: yyzzz=_tmp25E;goto _LL120;_LL123:;
_LL124:(int)_throw((void*)& Cyc_yyfail_YY8);_LL120:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY8(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9C5;return((_tmp9C5.YY8).val=
yy1,(((_tmp9C5.YY8).tag=14,_tmp9C5)));}static char _tmp261[7]="qvar_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp261,
_tmp261,_tmp261 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp262=yy1;struct _tuple1*_tmp263;_LL126:
if((_tmp262.QualId_tok).tag != 5)goto _LL128;_tmp263=(struct _tuple1*)(_tmp262.QualId_tok).val;
_LL127: yyzzz=_tmp263;goto _LL125;_LL128:;_LL129:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL125:;}return yyzzz;}union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple1*yy1){union Cyc_YYSTYPE
_tmp9C6;return((_tmp9C6.QualId_tok).val=yy1,(((_tmp9C6.QualId_tok).tag=5,_tmp9C6)));}
static char _tmp266[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY9={
Cyc_Core_Failure,{_tmp266,_tmp266,_tmp266 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE _tmp267=yy1;
struct Cyc_Absyn_Stmt*_tmp268;_LL12B: if((_tmp267.YY9).tag != 15)goto _LL12D;_tmp268=(
struct Cyc_Absyn_Stmt*)(_tmp267.YY9).val;_LL12C: yyzzz=_tmp268;goto _LL12A;_LL12D:;
_LL12E:(int)_throw((void*)& Cyc_yyfail_YY9);_LL12A:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9C7;return((_tmp9C7.YY9).val=
yy1,(((_tmp9C7.YY9).tag=15,_tmp9C7)));}static char _tmp26B[27]="list_t<switch_clause_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp26B,
_tmp26B,_tmp26B + 27}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp26C=yy1;struct Cyc_List_List*
_tmp26D;_LL130: if((_tmp26C.YY10).tag != 16)goto _LL132;_tmp26D=(struct Cyc_List_List*)(
_tmp26C.YY10).val;_LL131: yyzzz=_tmp26D;goto _LL12F;_LL132:;_LL133:(int)_throw((
void*)& Cyc_yyfail_YY10);_LL12F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9C8;return((_tmp9C8.YY10).val=yy1,(((_tmp9C8.YY10).tag=
16,_tmp9C8)));}static char _tmp270[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp270,_tmp270,_tmp270 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE
_tmp271=yy1;struct Cyc_Absyn_Pat*_tmp272;_LL135: if((_tmp271.YY11).tag != 17)goto
_LL137;_tmp272=(struct Cyc_Absyn_Pat*)(_tmp271.YY11).val;_LL136: yyzzz=_tmp272;
goto _LL134;_LL137:;_LL138:(int)_throw((void*)& Cyc_yyfail_YY11);_LL134:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9C9;
return((_tmp9C9.YY11).val=yy1,(((_tmp9C9.YY11).tag=17,_tmp9C9)));}static char
_tmp275[28]="$(list_t<pat_t,`H>,bool)@`H";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={
Cyc_Core_Failure,{_tmp275,_tmp275,_tmp275 + 28}};struct _tuple19*Cyc_yyget_YY12(
union Cyc_YYSTYPE yy1){struct _tuple19*yyzzz;{union Cyc_YYSTYPE _tmp276=yy1;struct
_tuple19*_tmp277;_LL13A: if((_tmp276.YY12).tag != 18)goto _LL13C;_tmp277=(struct
_tuple19*)(_tmp276.YY12).val;_LL13B: yyzzz=_tmp277;goto _LL139;_LL13C:;_LL13D:(int)
_throw((void*)& Cyc_yyfail_YY12);_LL139:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY12(
struct _tuple19*yy1){union Cyc_YYSTYPE _tmp9CA;return((_tmp9CA.YY12).val=yy1,(((
_tmp9CA.YY12).tag=18,_tmp9CA)));}static char _tmp27A[17]="list_t<pat_t,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp27A,_tmp27A,
_tmp27A + 17}};struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp27B=yy1;struct Cyc_List_List*_tmp27C;_LL13F: if((
_tmp27B.YY13).tag != 19)goto _LL141;_tmp27C=(struct Cyc_List_List*)(_tmp27B.YY13).val;
_LL140: yyzzz=_tmp27C;goto _LL13E;_LL141:;_LL142:(int)_throw((void*)& Cyc_yyfail_YY13);
_LL13E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9CB;return((_tmp9CB.YY13).val=yy1,(((_tmp9CB.YY13).tag=19,_tmp9CB)));}static
char _tmp27F[36]="$(list_t<designator_t,`H>,pat_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp27F,_tmp27F,_tmp27F + 36}};struct _tuple20*
Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){struct _tuple20*yyzzz;{union Cyc_YYSTYPE
_tmp280=yy1;struct _tuple20*_tmp281;_LL144: if((_tmp280.YY14).tag != 20)goto _LL146;
_tmp281=(struct _tuple20*)(_tmp280.YY14).val;_LL145: yyzzz=_tmp281;goto _LL143;
_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY14);_LL143:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY14(struct _tuple20*yy1){union Cyc_YYSTYPE _tmp9CC;return((_tmp9CC.YY14).val=
yy1,(((_tmp9CC.YY14).tag=20,_tmp9CC)));}static char _tmp284[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp284,
_tmp284,_tmp284 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp285=yy1;struct Cyc_List_List*
_tmp286;_LL149: if((_tmp285.YY15).tag != 21)goto _LL14B;_tmp286=(struct Cyc_List_List*)(
_tmp285.YY15).val;_LL14A: yyzzz=_tmp286;goto _LL148;_LL14B:;_LL14C:(int)_throw((
void*)& Cyc_yyfail_YY15);_LL148:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9CD;return((_tmp9CD.YY15).val=yy1,(((_tmp9CD.YY15).tag=
21,_tmp9CD)));}static char _tmp289[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp289,
_tmp289,_tmp289 + 58}};struct _tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct
_tuple19*yyzzz;{union Cyc_YYSTYPE _tmp28A=yy1;struct _tuple19*_tmp28B;_LL14E: if((
_tmp28A.YY16).tag != 22)goto _LL150;_tmp28B=(struct _tuple19*)(_tmp28A.YY16).val;
_LL14F: yyzzz=_tmp28B;goto _LL14D;_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY16);
_LL14D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY16(struct _tuple19*yy1){union Cyc_YYSTYPE
_tmp9CE;return((_tmp9CE.YY16).val=yy1,(((_tmp9CE.YY16).tag=22,_tmp9CE)));}static
char _tmp28E[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp28E,_tmp28E,_tmp28E + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp28F=yy1;
struct Cyc_Absyn_Fndecl*_tmp290;_LL153: if((_tmp28F.YY17).tag != 23)goto _LL155;
_tmp290=(struct Cyc_Absyn_Fndecl*)(_tmp28F.YY17).val;_LL154: yyzzz=_tmp290;goto
_LL152;_LL155:;_LL156:(int)_throw((void*)& Cyc_yyfail_YY17);_LL152:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9CF;
return((_tmp9CF.YY17).val=yy1,(((_tmp9CF.YY17).tag=23,_tmp9CF)));}static char
_tmp293[18]="list_t<decl_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp293,_tmp293,_tmp293 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp294=yy1;
struct Cyc_List_List*_tmp295;_LL158: if((_tmp294.YY18).tag != 24)goto _LL15A;_tmp295=(
struct Cyc_List_List*)(_tmp294.YY18).val;_LL159: yyzzz=_tmp295;goto _LL157;_LL15A:;
_LL15B:(int)_throw((void*)& Cyc_yyfail_YY18);_LL157:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D0;return((_tmp9D0.YY18).val=
yy1,(((_tmp9D0.YY18).tag=24,_tmp9D0)));}static char _tmp298[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp298,
_tmp298,_tmp298 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp299=yy1;struct
Cyc_Parse_Declaration_spec*_tmp29A;_LL15D: if((_tmp299.YY19).tag != 25)goto _LL15F;
_tmp29A=(struct Cyc_Parse_Declaration_spec*)(_tmp299.YY19).val;_LL15E: yyzzz=
_tmp29A;goto _LL15C;_LL15F:;_LL160:(int)_throw((void*)& Cyc_yyfail_YY19);_LL15C:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){
union Cyc_YYSTYPE _tmp9D1;return((_tmp9D1.YY19).val=yy1,(((_tmp9D1.YY19).tag=25,
_tmp9D1)));}static char _tmp29D[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp29D,_tmp29D,_tmp29D + 29}};struct _tuple21*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE
_tmp29E=yy1;struct _tuple21*_tmp29F;_LL162: if((_tmp29E.YY20).tag != 26)goto _LL164;
_tmp29F=(struct _tuple21*)(_tmp29E.YY20).val;_LL163: yyzzz=_tmp29F;goto _LL161;
_LL164:;_LL165:(int)_throw((void*)& Cyc_yyfail_YY20);_LL161:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY20(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9D2;return((_tmp9D2.YY20).val=
yy1,(((_tmp9D2.YY20).tag=26,_tmp9D2)));}static char _tmp2A2[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp2A2,
_tmp2A2,_tmp2A2 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2A3=yy1;struct Cyc_List_List*
_tmp2A4;_LL167: if((_tmp2A3.YY21).tag != 27)goto _LL169;_tmp2A4=(struct Cyc_List_List*)(
_tmp2A3.YY21).val;_LL168: yyzzz=_tmp2A4;goto _LL166;_LL169:;_LL16A:(int)_throw((
void*)& Cyc_yyfail_YY21);_LL166:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9D3;return((_tmp9D3.YY21).val=yy1,(((_tmp9D3.YY21).tag=
27,_tmp9D3)));}static char _tmp2A7[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp2A7,_tmp2A7,_tmp2A7 + 16}};enum Cyc_Parse_Storage_class
Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){enum Cyc_Parse_Storage_class yyzzz;{union Cyc_YYSTYPE
_tmp2A8=yy1;enum Cyc_Parse_Storage_class _tmp2A9;_LL16C: if((_tmp2A8.YY22).tag != 
28)goto _LL16E;_tmp2A9=(enum Cyc_Parse_Storage_class)(_tmp2A8.YY22).val;_LL16D:
yyzzz=_tmp2A9;goto _LL16B;_LL16E:;_LL16F:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL16B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class yy1){
union Cyc_YYSTYPE _tmp9D4;return((_tmp9D4.YY22).val=yy1,(((_tmp9D4.YY22).tag=28,
_tmp9D4)));}static char _tmp2AC[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp2AC,_tmp2AC,_tmp2AC + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2AD=yy1;void*_tmp2AE;_LL171:
if((_tmp2AD.YY23).tag != 29)goto _LL173;_tmp2AE=(void*)(_tmp2AD.YY23).val;_LL172:
yyzzz=_tmp2AE;goto _LL170;_LL173:;_LL174:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL170:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(void*yy1){union Cyc_YYSTYPE _tmp9D5;
return((_tmp9D5.YY23).val=yy1,(((_tmp9D5.YY23).tag=29,_tmp9D5)));}static char
_tmp2B1[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={
Cyc_Core_Failure,{_tmp2B1,_tmp2B1,_tmp2B1 + 12}};enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(
union Cyc_YYSTYPE yy1){enum Cyc_Absyn_AggrKind yyzzz;{union Cyc_YYSTYPE _tmp2B2=yy1;
enum Cyc_Absyn_AggrKind _tmp2B3;_LL176: if((_tmp2B2.YY24).tag != 30)goto _LL178;
_tmp2B3=(enum Cyc_Absyn_AggrKind)(_tmp2B2.YY24).val;_LL177: yyzzz=_tmp2B3;goto
_LL175;_LL178:;_LL179:(int)_throw((void*)& Cyc_yyfail_YY24);_LL175:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9D6;
return((_tmp9D6.YY24).val=yy1,(((_tmp9D6.YY24).tag=30,_tmp9D6)));}static char
_tmp2B6[8]="tqual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{
_tmp2B6,_tmp2B6,_tmp2B6 + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE _tmp2B7=yy1;struct Cyc_Absyn_Tqual
_tmp2B8;_LL17B: if((_tmp2B7.YY25).tag != 31)goto _LL17D;_tmp2B8=(struct Cyc_Absyn_Tqual)(
_tmp2B7.YY25).val;_LL17C: yyzzz=_tmp2B8;goto _LL17A;_LL17D:;_LL17E:(int)_throw((
void*)& Cyc_yyfail_YY25);_LL17A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual
yy1){union Cyc_YYSTYPE _tmp9D7;return((_tmp9D7.YY25).val=yy1,(((_tmp9D7.YY25).tag=
31,_tmp9D7)));}static char _tmp2BB[23]="list_t<aggrfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp2BB,_tmp2BB,_tmp2BB + 23}};struct Cyc_List_List*
Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2BC=yy1;struct Cyc_List_List*_tmp2BD;_LL180: if((_tmp2BC.YY26).tag != 32)goto
_LL182;_tmp2BD=(struct Cyc_List_List*)(_tmp2BC.YY26).val;_LL181: yyzzz=_tmp2BD;
goto _LL17F;_LL182:;_LL183:(int)_throw((void*)& Cyc_yyfail_YY26);_LL17F:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D8;
return((_tmp9D8.YY26).val=yy1,(((_tmp9D8.YY26).tag=32,_tmp9D8)));}static char
_tmp2C0[34]="list_t<list_t<aggrfield_t,`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2C0,_tmp2C0,_tmp2C0 + 34}};struct Cyc_List_List*
Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2C1=yy1;struct Cyc_List_List*_tmp2C2;_LL185: if((_tmp2C1.YY27).tag != 33)goto
_LL187;_tmp2C2=(struct Cyc_List_List*)(_tmp2C1.YY27).val;_LL186: yyzzz=_tmp2C2;
goto _LL184;_LL187:;_LL188:(int)_throw((void*)& Cyc_yyfail_YY27);_LL184:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D9;
return((_tmp9D9.YY27).val=yy1,(((_tmp9D9.YY27).tag=33,_tmp9D9)));}static char
_tmp2C5[31]="list_t<type_modifier_t<`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2C5,_tmp2C5,_tmp2C5 + 31}};struct Cyc_List_List*
Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2C6=yy1;struct Cyc_List_List*_tmp2C7;_LL18A: if((_tmp2C6.YY28).tag != 34)goto
_LL18C;_tmp2C7=(struct Cyc_List_List*)(_tmp2C6.YY28).val;_LL18B: yyzzz=_tmp2C7;
goto _LL189;_LL18C:;_LL18D:(int)_throw((void*)& Cyc_yyfail_YY28);_LL189:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DA;
return((_tmp9DA.YY28).val=yy1,(((_tmp9DA.YY28).tag=34,_tmp9DA)));}static char
_tmp2CA[13]="declarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={
Cyc_Core_Failure,{_tmp2CA,_tmp2CA,_tmp2CA + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE _tmp2CB=
yy1;struct Cyc_Parse_Declarator*_tmp2CC;_LL18F: if((_tmp2CB.YY29).tag != 35)goto
_LL191;_tmp2CC=(struct Cyc_Parse_Declarator*)(_tmp2CB.YY29).val;_LL190: yyzzz=
_tmp2CC;goto _LL18E;_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY29);_LL18E:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){union Cyc_YYSTYPE
_tmp9DB;return((_tmp9DB.YY29).val=yy1,(((_tmp9DB.YY29).tag=35,_tmp9DB)));}static
char _tmp2CF[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={
Cyc_Core_Failure,{_tmp2CF,_tmp2CF,_tmp2CF + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{
union Cyc_YYSTYPE _tmp2D0=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp2D1;_LL194:
if((_tmp2D0.YY30).tag != 36)goto _LL196;_tmp2D1=(struct Cyc_Parse_Abstractdeclarator*)(
_tmp2D0.YY30).val;_LL195: yyzzz=_tmp2D1;goto _LL193;_LL196:;_LL197:(int)_throw((
void*)& Cyc_yyfail_YY30);_LL193:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1){union Cyc_YYSTYPE _tmp9DC;return((_tmp9DC.YY30).val=yy1,(((_tmp9DC.YY30).tag=
36,_tmp9DC)));}static char _tmp2D4[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2D4,_tmp2D4,_tmp2D4 + 5}};int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp2D5=yy1;int _tmp2D6;_LL199: if((
_tmp2D5.YY31).tag != 37)goto _LL19B;_tmp2D6=(int)(_tmp2D5.YY31).val;_LL19A: yyzzz=
_tmp2D6;goto _LL198;_LL19B:;_LL19C:(int)_throw((void*)& Cyc_yyfail_YY31);_LL198:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp9DD;return((
_tmp9DD.YY31).val=yy1,(((_tmp9DD.YY31).tag=37,_tmp9DD)));}static char _tmp2D9[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2D9,
_tmp2D9,_tmp2D9 + 8}};enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;{union Cyc_YYSTYPE _tmp2DA=yy1;enum Cyc_Absyn_Scope
_tmp2DB;_LL19E: if((_tmp2DA.YY32).tag != 38)goto _LL1A0;_tmp2DB=(enum Cyc_Absyn_Scope)(
_tmp2DA.YY32).val;_LL19F: yyzzz=_tmp2DB;goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((
void*)& Cyc_yyfail_YY32);_LL19D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope
yy1){union Cyc_YYSTYPE _tmp9DE;return((_tmp9DE.YY32).val=yy1,(((_tmp9DE.YY32).tag=
38,_tmp9DE)));}static char _tmp2DE[16]="datatypefield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2DE,_tmp2DE,_tmp2DE + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union
Cyc_YYSTYPE _tmp2DF=yy1;struct Cyc_Absyn_Datatypefield*_tmp2E0;_LL1A3: if((_tmp2DF.YY33).tag
!= 39)goto _LL1A5;_tmp2E0=(struct Cyc_Absyn_Datatypefield*)(_tmp2DF.YY33).val;
_LL1A4: yyzzz=_tmp2E0;goto _LL1A2;_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL1A2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){
union Cyc_YYSTYPE _tmp9DF;return((_tmp9DF.YY33).val=yy1,(((_tmp9DF.YY33).tag=39,
_tmp9DF)));}static char _tmp2E3[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2E3,_tmp2E3,_tmp2E3 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2E4=yy1;struct Cyc_List_List*_tmp2E5;_LL1A8: if((_tmp2E4.YY34).tag != 40)goto
_LL1AA;_tmp2E5=(struct Cyc_List_List*)(_tmp2E4.YY34).val;_LL1A9: yyzzz=_tmp2E5;
goto _LL1A7;_LL1AA:;_LL1AB:(int)_throw((void*)& Cyc_yyfail_YY34);_LL1A7:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E0;
return((_tmp9E0.YY34).val=yy1,(((_tmp9E0.YY34).tag=40,_tmp9E0)));}static char
_tmp2E8[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2E8,_tmp2E8,
_tmp2E8 + 55}};struct _tuple22*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple22*
yyzzz;{union Cyc_YYSTYPE _tmp2E9=yy1;struct _tuple22*_tmp2EA;_LL1AD: if((_tmp2E9.YY35).tag
!= 41)goto _LL1AF;_tmp2EA=(struct _tuple22*)(_tmp2E9.YY35).val;_LL1AE: yyzzz=
_tmp2EA;goto _LL1AC;_LL1AF:;_LL1B0:(int)_throw((void*)& Cyc_yyfail_YY35);_LL1AC:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY35(struct _tuple22*yy1){union Cyc_YYSTYPE
_tmp9E1;return((_tmp9E1.YY35).val=yy1,(((_tmp9E1.YY35).tag=41,_tmp9E1)));}static
char _tmp2ED[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={
Cyc_Core_Failure,{_tmp2ED,_tmp2ED,_tmp2ED + 17}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2EE=yy1;
struct Cyc_List_List*_tmp2EF;_LL1B2: if((_tmp2EE.YY36).tag != 42)goto _LL1B4;_tmp2EF=(
struct Cyc_List_List*)(_tmp2EE.YY36).val;_LL1B3: yyzzz=_tmp2EF;goto _LL1B1;_LL1B4:;
_LL1B5:(int)_throw((void*)& Cyc_yyfail_YY36);_LL1B1:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E2;return((_tmp9E2.YY36).val=
yy1,(((_tmp9E2.YY36).tag=42,_tmp9E2)));}static char _tmp2F2[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2F2,
_tmp2F2,_tmp2F2 + 37}};struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){struct
_tuple8*yyzzz;{union Cyc_YYSTYPE _tmp2F3=yy1;struct _tuple8*_tmp2F4;_LL1B7: if((
_tmp2F3.YY37).tag != 43)goto _LL1B9;_tmp2F4=(struct _tuple8*)(_tmp2F3.YY37).val;
_LL1B8: yyzzz=_tmp2F4;goto _LL1B6;_LL1B9:;_LL1BA:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL1B6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE
_tmp9E3;return((_tmp9E3.YY37).val=yy1,(((_tmp9E3.YY37).tag=43,_tmp9E3)));}static
char _tmp2F7[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2F7,_tmp2F7,
_tmp2F7 + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2F8=yy1;struct Cyc_List_List*_tmp2F9;_LL1BC: if((
_tmp2F8.YY38).tag != 44)goto _LL1BE;_tmp2F9=(struct Cyc_List_List*)(_tmp2F8.YY38).val;
_LL1BD: yyzzz=_tmp2F9;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1BB:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9E4;return((_tmp9E4.YY38).val=yy1,(((_tmp9E4.YY38).tag=44,_tmp9E4)));}static
char _tmp2FC[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2FC,
_tmp2FC,_tmp2FC + 127}};struct _tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct
_tuple23*yyzzz;{union Cyc_YYSTYPE _tmp2FD=yy1;struct _tuple23*_tmp2FE;_LL1C1: if((
_tmp2FD.YY39).tag != 45)goto _LL1C3;_tmp2FE=(struct _tuple23*)(_tmp2FD.YY39).val;
_LL1C2: yyzzz=_tmp2FE;goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY39);
_LL1C0:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY39(struct _tuple23*yy1){union Cyc_YYSTYPE
_tmp9E5;return((_tmp9E5.YY39).val=yy1,(((_tmp9E5.YY39).tag=45,_tmp9E5)));}static
char _tmp301[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={
Cyc_Core_Failure,{_tmp301,_tmp301,_tmp301 + 18}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp302=yy1;
struct Cyc_List_List*_tmp303;_LL1C6: if((_tmp302.YY40).tag != 46)goto _LL1C8;_tmp303=(
struct Cyc_List_List*)(_tmp302.YY40).val;_LL1C7: yyzzz=_tmp303;goto _LL1C5;_LL1C8:;
_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY40);_LL1C5:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E6;return((_tmp9E6.YY40).val=
yy1,(((_tmp9E6.YY40).tag=46,_tmp9E6)));}static char _tmp306[24]="list_t<designator_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp306,
_tmp306,_tmp306 + 24}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp307=yy1;struct Cyc_List_List*
_tmp308;_LL1CB: if((_tmp307.YY41).tag != 47)goto _LL1CD;_tmp308=(struct Cyc_List_List*)(
_tmp307.YY41).val;_LL1CC: yyzzz=_tmp308;goto _LL1CA;_LL1CD:;_LL1CE:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1CA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9E7;return((_tmp9E7.YY41).val=yy1,(((_tmp9E7.YY41).tag=
47,_tmp9E7)));}static char _tmp30B[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp30B,_tmp30B,_tmp30B + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp30C=yy1;void*_tmp30D;_LL1D0:
if((_tmp30C.YY42).tag != 48)goto _LL1D2;_tmp30D=(void*)(_tmp30C.YY42).val;_LL1D1:
yyzzz=_tmp30D;goto _LL1CF;_LL1D2:;_LL1D3:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1CF:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp9E8;
return((_tmp9E8.YY42).val=yy1,(((_tmp9E8.YY42).tag=48,_tmp9E8)));}static char
_tmp310[7]="kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{
_tmp310,_tmp310,_tmp310 + 7}};struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Kind*yyzzz;{union Cyc_YYSTYPE _tmp311=yy1;struct Cyc_Absyn_Kind*
_tmp312;_LL1D5: if((_tmp311.YY43).tag != 49)goto _LL1D7;_tmp312=(struct Cyc_Absyn_Kind*)(
_tmp311.YY43).val;_LL1D6: yyzzz=_tmp312;goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((
void*)& Cyc_yyfail_YY43);_LL1D4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*
yy1){union Cyc_YYSTYPE _tmp9E9;return((_tmp9E9.YY43).val=yy1,(((_tmp9E9.YY43).tag=
49,_tmp9E9)));}static char _tmp315[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp315,_tmp315,_tmp315 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp316=yy1;void*_tmp317;_LL1DA:
if((_tmp316.YY44).tag != 50)goto _LL1DC;_tmp317=(void*)(_tmp316.YY44).val;_LL1DB:
yyzzz=_tmp317;goto _LL1D9;_LL1DC:;_LL1DD:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1D9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmp9EA;
return((_tmp9EA.YY44).val=yy1,(((_tmp9EA.YY44).tag=50,_tmp9EA)));}static char
_tmp31A[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY45={
Cyc_Core_Failure,{_tmp31A,_tmp31A,_tmp31A + 23}};struct Cyc_List_List*Cyc_yyget_YY45(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp31B=yy1;
struct Cyc_List_List*_tmp31C;_LL1DF: if((_tmp31B.YY45).tag != 51)goto _LL1E1;_tmp31C=(
struct Cyc_List_List*)(_tmp31B.YY45).val;_LL1E0: yyzzz=_tmp31C;goto _LL1DE;_LL1E1:;
_LL1E2:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1DE:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EB;return((_tmp9EB.YY45).val=
yy1,(((_tmp9EB.YY45).tag=51,_tmp9EB)));}static char _tmp31F[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp31F,
_tmp31F,_tmp31F + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union
Cyc_YYSTYPE _tmp320=yy1;void*_tmp321;_LL1E4: if((_tmp320.YY46).tag != 52)goto _LL1E6;
_tmp321=(void*)(_tmp320.YY46).val;_LL1E5: yyzzz=_tmp321;goto _LL1E3;_LL1E6:;_LL1E7:(
int)_throw((void*)& Cyc_yyfail_YY46);_LL1E3:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1){union Cyc_YYSTYPE _tmp9EC;return((_tmp9EC.YY46).val=yy1,(((_tmp9EC.YY46).tag=
52,_tmp9EC)));}static char _tmp324[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp324,_tmp324,_tmp324 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE
_tmp325=yy1;struct Cyc_Absyn_Enumfield*_tmp326;_LL1E9: if((_tmp325.YY47).tag != 53)
goto _LL1EB;_tmp326=(struct Cyc_Absyn_Enumfield*)(_tmp325.YY47).val;_LL1EA: yyzzz=
_tmp326;goto _LL1E8;_LL1EB:;_LL1EC:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1E8:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE
_tmp9ED;return((_tmp9ED.YY47).val=yy1,(((_tmp9ED.YY47).tag=53,_tmp9ED)));}static
char _tmp329[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={
Cyc_Core_Failure,{_tmp329,_tmp329,_tmp329 + 23}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp32A=yy1;
struct Cyc_List_List*_tmp32B;_LL1EE: if((_tmp32A.YY48).tag != 54)goto _LL1F0;_tmp32B=(
struct Cyc_List_List*)(_tmp32A.YY48).val;_LL1EF: yyzzz=_tmp32B;goto _LL1ED;_LL1F0:;
_LL1F1:(int)_throw((void*)& Cyc_yyfail_YY48);_LL1ED:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EE;return((_tmp9EE.YY48).val=
yy1,(((_tmp9EE.YY48).tag=54,_tmp9EE)));}static char _tmp32E[17]="opt_t<type_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp32E,
_tmp32E,_tmp32E + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp32F=yy1;struct Cyc_Core_Opt*_tmp330;
_LL1F3: if((_tmp32F.YY49).tag != 55)goto _LL1F5;_tmp330=(struct Cyc_Core_Opt*)(
_tmp32F.YY49).val;_LL1F4: yyzzz=_tmp330;goto _LL1F2;_LL1F5:;_LL1F6:(int)_throw((
void*)& Cyc_yyfail_YY49);_LL1F2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*
yy1){union Cyc_YYSTYPE _tmp9EF;return((_tmp9EF.YY49).val=yy1,(((_tmp9EF.YY49).tag=
55,_tmp9EF)));}static char _tmp333[31]="list_t<$(type_t,type_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp333,_tmp333,
_tmp333 + 31}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp334=yy1;struct Cyc_List_List*_tmp335;_LL1F8: if((
_tmp334.YY50).tag != 56)goto _LL1FA;_tmp335=(struct Cyc_List_List*)(_tmp334.YY50).val;
_LL1F9: yyzzz=_tmp335;goto _LL1F7;_LL1FA:;_LL1FB:(int)_throw((void*)& Cyc_yyfail_YY50);
_LL1F7:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9F0;return((_tmp9F0.YY50).val=yy1,(((_tmp9F0.YY50).tag=56,_tmp9F0)));}static
char _tmp338[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY51={
Cyc_Core_Failure,{_tmp338,_tmp338,_tmp338 + 15}};union Cyc_Absyn_Constraint*Cyc_yyget_YY51(
union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp339=
yy1;union Cyc_Absyn_Constraint*_tmp33A;_LL1FD: if((_tmp339.YY51).tag != 57)goto
_LL1FF;_tmp33A=(union Cyc_Absyn_Constraint*)(_tmp339.YY51).val;_LL1FE: yyzzz=
_tmp33A;goto _LL1FC;_LL1FF:;_LL200:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1FC:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE
_tmp9F1;return((_tmp9F1.YY51).val=yy1,(((_tmp9F1.YY51).tag=57,_tmp9F1)));}static
char _tmp33D[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp33D,_tmp33D,_tmp33D + 45}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp33E=yy1;struct Cyc_List_List*_tmp33F;_LL202: if((_tmp33E.YY52).tag != 58)goto
_LL204;_tmp33F=(struct Cyc_List_List*)(_tmp33E.YY52).val;_LL203: yyzzz=_tmp33F;
goto _LL201;_LL204:;_LL205:(int)_throw((void*)& Cyc_yyfail_YY52);_LL201:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F2;
return((_tmp9F2.YY52).val=yy1,(((_tmp9F2.YY52).tag=58,_tmp9F2)));}static char
_tmp342[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp342,_tmp342,_tmp342 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp343=yy1;void*_tmp344;_LL207: if((_tmp343.YY53).tag
!= 59)goto _LL209;_tmp344=(void*)(_tmp343.YY53).val;_LL208: yyzzz=_tmp344;goto
_LL206;_LL209:;_LL20A:(int)_throw((void*)& Cyc_yyfail_YY53);_LL206:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmp9F3;return((_tmp9F3.YY53).val=
yy1,(((_tmp9F3.YY53).tag=59,_tmp9F3)));}static char _tmp347[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp347,
_tmp347,_tmp347 + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp348=yy1;struct Cyc_List_List*
_tmp349;_LL20C: if((_tmp348.YY54).tag != 60)goto _LL20E;_tmp349=(struct Cyc_List_List*)(
_tmp348.YY54).val;_LL20D: yyzzz=_tmp349;goto _LL20B;_LL20E:;_LL20F:(int)_throw((
void*)& Cyc_yyfail_YY54);_LL20B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9F4;return((_tmp9F4.YY54).val=yy1,(((_tmp9F4.YY54).tag=
60,_tmp9F4)));}static char _tmp34C[21]="$(bool,string_t<`H>)";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp34C,_tmp34C,_tmp34C + 21}};struct
_tuple17 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){struct _tuple17 yyzzz;{union Cyc_YYSTYPE
_tmp34D=yy1;struct _tuple17 _tmp34E;_LL211: if((_tmp34D.Asm_tok).tag != 6)goto _LL213;
_tmp34E=(struct _tuple17)(_tmp34D.Asm_tok).val;_LL212: yyzzz=_tmp34E;goto _LL210;
_LL213:;_LL214:(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL210:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple17 yy1){union Cyc_YYSTYPE _tmp9F5;return((
_tmp9F5.Asm_tok).val=yy1,(((_tmp9F5.Asm_tok).tag=6,_tmp9F5)));}struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_Yyltype Cyc_yynewloc(){struct Cyc_Yyltype _tmp9F6;return(_tmp9F6.timestamp=0,((
_tmp9F6.first_line=0,((_tmp9F6.first_column=0,((_tmp9F6.last_line=0,((_tmp9F6.last_column=
0,_tmp9F6)))))))));}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[
372]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,143,2,2,
127,141,138,2,124,125,132,135,120,139,129,140,2,2,2,2,2,2,2,2,2,2,128,117,122,
121,123,134,133,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,130,2,131,
137,126,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,118,136,119,142,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,
79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,
104,105,106,107,108,109,110,111,112,113,114,115,116};static char _tmp351[2]="$";
static char _tmp352[6]="error";static char _tmp353[12]="$undefined.";static char
_tmp354[5]="AUTO";static char _tmp355[9]="REGISTER";static char _tmp356[7]="STATIC";
static char _tmp357[7]="EXTERN";static char _tmp358[8]="TYPEDEF";static char _tmp359[5]="VOID";
static char _tmp35A[5]="CHAR";static char _tmp35B[6]="SHORT";static char _tmp35C[4]="INT";
static char _tmp35D[5]="LONG";static char _tmp35E[6]="FLOAT";static char _tmp35F[7]="DOUBLE";
static char _tmp360[7]="SIGNED";static char _tmp361[9]="UNSIGNED";static char _tmp362[
6]="CONST";static char _tmp363[9]="VOLATILE";static char _tmp364[9]="RESTRICT";
static char _tmp365[7]="STRUCT";static char _tmp366[6]="UNION";static char _tmp367[5]="CASE";
static char _tmp368[8]="DEFAULT";static char _tmp369[7]="INLINE";static char _tmp36A[7]="SIZEOF";
static char _tmp36B[9]="OFFSETOF";static char _tmp36C[3]="IF";static char _tmp36D[5]="ELSE";
static char _tmp36E[7]="SWITCH";static char _tmp36F[6]="WHILE";static char _tmp370[3]="DO";
static char _tmp371[4]="FOR";static char _tmp372[5]="GOTO";static char _tmp373[9]="CONTINUE";
static char _tmp374[6]="BREAK";static char _tmp375[7]="RETURN";static char _tmp376[5]="ENUM";
static char _tmp377[8]="NULL_kw";static char _tmp378[4]="LET";static char _tmp379[6]="THROW";
static char _tmp37A[4]="TRY";static char _tmp37B[6]="CATCH";static char _tmp37C[7]="EXPORT";
static char _tmp37D[4]="NEW";static char _tmp37E[9]="ABSTRACT";static char _tmp37F[9]="FALLTHRU";
static char _tmp380[6]="USING";static char _tmp381[10]="NAMESPACE";static char _tmp382[
9]="DATATYPE";static char _tmp383[8]="XTUNION";static char _tmp384[7]="TUNION";
static char _tmp385[7]="MALLOC";static char _tmp386[8]="RMALLOC";static char _tmp387[7]="CALLOC";
static char _tmp388[8]="RCALLOC";static char _tmp389[5]="SWAP";static char _tmp38A[9]="REGION_T";
static char _tmp38B[6]="TAG_T";static char _tmp38C[7]="REGION";static char _tmp38D[5]="RNEW";
static char _tmp38E[8]="REGIONS";static char _tmp38F[13]="RESET_REGION";static char
_tmp390[16]="NOZEROTERM_QUAL";static char _tmp391[14]="ZEROTERM_QUAL";static char
_tmp392[12]="REGION_QUAL";static char _tmp393[7]="PORTON";static char _tmp394[8]="PORTOFF";
static char _tmp395[12]="DYNREGION_T";static char _tmp396[6]="ALIAS";static char
_tmp397[8]="NUMELTS";static char _tmp398[8]="VALUEOF";static char _tmp399[10]="VALUEOF_T";
static char _tmp39A[9]="TAGCHECK";static char _tmp39B[13]="NUMELTS_QUAL";static char
_tmp39C[10]="THIN_QUAL";static char _tmp39D[9]="FAT_QUAL";static char _tmp39E[13]="NOTNULL_QUAL";
static char _tmp39F[14]="NULLABLE_QUAL";static char _tmp3A0[12]="TAGGED_QUAL";static
char _tmp3A1[16]="EXTENSIBLE_QUAL";static char _tmp3A2[15]="RESETABLE_QUAL";static
char _tmp3A3[7]="PTR_OP";static char _tmp3A4[7]="INC_OP";static char _tmp3A5[7]="DEC_OP";
static char _tmp3A6[8]="LEFT_OP";static char _tmp3A7[9]="RIGHT_OP";static char _tmp3A8[
6]="LE_OP";static char _tmp3A9[6]="GE_OP";static char _tmp3AA[6]="EQ_OP";static char
_tmp3AB[6]="NE_OP";static char _tmp3AC[7]="AND_OP";static char _tmp3AD[6]="OR_OP";
static char _tmp3AE[11]="MUL_ASSIGN";static char _tmp3AF[11]="DIV_ASSIGN";static char
_tmp3B0[11]="MOD_ASSIGN";static char _tmp3B1[11]="ADD_ASSIGN";static char _tmp3B2[11]="SUB_ASSIGN";
static char _tmp3B3[12]="LEFT_ASSIGN";static char _tmp3B4[13]="RIGHT_ASSIGN";static
char _tmp3B5[11]="AND_ASSIGN";static char _tmp3B6[11]="XOR_ASSIGN";static char
_tmp3B7[10]="OR_ASSIGN";static char _tmp3B8[9]="ELLIPSIS";static char _tmp3B9[11]="LEFT_RIGHT";
static char _tmp3BA[12]="COLON_COLON";static char _tmp3BB[11]="IDENTIFIER";static
char _tmp3BC[17]="INTEGER_CONSTANT";static char _tmp3BD[7]="STRING";static char
_tmp3BE[19]="CHARACTER_CONSTANT";static char _tmp3BF[18]="FLOATING_CONSTANT";
static char _tmp3C0[9]="TYPE_VAR";static char _tmp3C1[13]="TYPEDEF_NAME";static char
_tmp3C2[16]="QUAL_IDENTIFIER";static char _tmp3C3[18]="QUAL_TYPEDEF_NAME";static
char _tmp3C4[10]="ATTRIBUTE";static char _tmp3C5[4]="ASM";static char _tmp3C6[4]="';'";
static char _tmp3C7[4]="'{'";static char _tmp3C8[4]="'}'";static char _tmp3C9[4]="','";
static char _tmp3CA[4]="'='";static char _tmp3CB[4]="'<'";static char _tmp3CC[4]="'>'";
static char _tmp3CD[4]="'('";static char _tmp3CE[4]="')'";static char _tmp3CF[4]="'_'";
static char _tmp3D0[4]="'$'";static char _tmp3D1[4]="':'";static char _tmp3D2[4]="'.'";
static char _tmp3D3[4]="'['";static char _tmp3D4[4]="']'";static char _tmp3D5[4]="'*'";
static char _tmp3D6[4]="'@'";static char _tmp3D7[4]="'?'";static char _tmp3D8[4]="'+'";
static char _tmp3D9[4]="'|'";static char _tmp3DA[4]="'^'";static char _tmp3DB[4]="'&'";
static char _tmp3DC[4]="'-'";static char _tmp3DD[4]="'/'";static char _tmp3DE[4]="'%'";
static char _tmp3DF[4]="'~'";static char _tmp3E0[4]="'!'";static char _tmp3E1[5]="prog";
static char _tmp3E2[17]="translation_unit";static char _tmp3E3[12]="export_list";
static char _tmp3E4[19]="export_list_values";static char _tmp3E5[21]="external_declaration";
static char _tmp3E6[20]="function_definition";static char _tmp3E7[21]="function_definition2";
static char _tmp3E8[13]="using_action";static char _tmp3E9[15]="unusing_action";
static char _tmp3EA[17]="namespace_action";static char _tmp3EB[19]="unnamespace_action";
static char _tmp3EC[12]="declaration";static char _tmp3ED[19]="resetable_qual_opt";
static char _tmp3EE[17]="declaration_list";static char _tmp3EF[23]="declaration_specifiers";
static char _tmp3F0[24]="storage_class_specifier";static char _tmp3F1[15]="attributes_opt";
static char _tmp3F2[11]="attributes";static char _tmp3F3[15]="attribute_list";static
char _tmp3F4[10]="attribute";static char _tmp3F5[15]="type_specifier";static char
_tmp3F6[25]="type_specifier_notypedef";static char _tmp3F7[5]="kind";static char
_tmp3F8[15]="type_qualifier";static char _tmp3F9[15]="enum_specifier";static char
_tmp3FA[11]="enum_field";static char _tmp3FB[22]="enum_declaration_list";static
char _tmp3FC[26]="struct_or_union_specifier";static char _tmp3FD[16]="type_params_opt";
static char _tmp3FE[16]="struct_or_union";static char _tmp3FF[24]="struct_declaration_list";
static char _tmp400[25]="struct_declaration_list0";static char _tmp401[21]="init_declarator_list";
static char _tmp402[22]="init_declarator_list0";static char _tmp403[16]="init_declarator";
static char _tmp404[19]="struct_declaration";static char _tmp405[25]="specifier_qualifier_list";
static char _tmp406[35]="notypedef_specifier_qualifier_list";static char _tmp407[23]="struct_declarator_list";
static char _tmp408[24]="struct_declarator_list0";static char _tmp409[18]="struct_declarator";
static char _tmp40A[19]="datatype_specifier";static char _tmp40B[14]="qual_datatype";
static char _tmp40C[19]="datatypefield_list";static char _tmp40D[20]="datatypefield_scope";
static char _tmp40E[14]="datatypefield";static char _tmp40F[11]="declarator";static
char _tmp410[23]="declarator_withtypedef";static char _tmp411[18]="direct_declarator";
static char _tmp412[30]="direct_declarator_withtypedef";static char _tmp413[8]="pointer";
static char _tmp414[12]="one_pointer";static char _tmp415[14]="pointer_quals";static
char _tmp416[13]="pointer_qual";static char _tmp417[23]="pointer_null_and_bound";
static char _tmp418[14]="pointer_bound";static char _tmp419[18]="zeroterm_qual_opt";
static char _tmp41A[8]="rgn_opt";static char _tmp41B[11]="tqual_list";static char
_tmp41C[20]="parameter_type_list";static char _tmp41D[9]="type_var";static char
_tmp41E[16]="optional_effect";static char _tmp41F[19]="optional_rgn_order";static
char _tmp420[10]="rgn_order";static char _tmp421[16]="optional_inject";static char
_tmp422[11]="effect_set";static char _tmp423[14]="atomic_effect";static char _tmp424[
11]="region_set";static char _tmp425[15]="parameter_list";static char _tmp426[22]="parameter_declaration";
static char _tmp427[16]="identifier_list";static char _tmp428[17]="identifier_list0";
static char _tmp429[12]="initializer";static char _tmp42A[18]="array_initializer";
static char _tmp42B[17]="initializer_list";static char _tmp42C[12]="designation";
static char _tmp42D[16]="designator_list";static char _tmp42E[11]="designator";
static char _tmp42F[10]="type_name";static char _tmp430[14]="any_type_name";static
char _tmp431[15]="type_name_list";static char _tmp432[20]="abstract_declarator";
static char _tmp433[27]="direct_abstract_declarator";static char _tmp434[10]="statement";
static char _tmp435[13]="open_exp_opt";static char _tmp436[18]="labeled_statement";
static char _tmp437[21]="expression_statement";static char _tmp438[19]="compound_statement";
static char _tmp439[16]="block_item_list";static char _tmp43A[20]="selection_statement";
static char _tmp43B[15]="switch_clauses";static char _tmp43C[20]="iteration_statement";
static char _tmp43D[15]="jump_statement";static char _tmp43E[12]="exp_pattern";
static char _tmp43F[20]="conditional_pattern";static char _tmp440[19]="logical_or_pattern";
static char _tmp441[20]="logical_and_pattern";static char _tmp442[21]="inclusive_or_pattern";
static char _tmp443[21]="exclusive_or_pattern";static char _tmp444[12]="and_pattern";
static char _tmp445[17]="equality_pattern";static char _tmp446[19]="relational_pattern";
static char _tmp447[14]="shift_pattern";static char _tmp448[17]="additive_pattern";
static char _tmp449[23]="multiplicative_pattern";static char _tmp44A[13]="cast_pattern";
static char _tmp44B[14]="unary_pattern";static char _tmp44C[16]="postfix_pattern";
static char _tmp44D[16]="primary_pattern";static char _tmp44E[8]="pattern";static
char _tmp44F[19]="tuple_pattern_list";static char _tmp450[20]="tuple_pattern_list0";
static char _tmp451[14]="field_pattern";static char _tmp452[19]="field_pattern_list";
static char _tmp453[20]="field_pattern_list0";static char _tmp454[11]="expression";
static char _tmp455[22]="assignment_expression";static char _tmp456[20]="assignment_operator";
static char _tmp457[23]="conditional_expression";static char _tmp458[20]="constant_expression";
static char _tmp459[22]="logical_or_expression";static char _tmp45A[23]="logical_and_expression";
static char _tmp45B[24]="inclusive_or_expression";static char _tmp45C[24]="exclusive_or_expression";
static char _tmp45D[15]="and_expression";static char _tmp45E[20]="equality_expression";
static char _tmp45F[22]="relational_expression";static char _tmp460[17]="shift_expression";
static char _tmp461[20]="additive_expression";static char _tmp462[26]="multiplicative_expression";
static char _tmp463[16]="cast_expression";static char _tmp464[17]="unary_expression";
static char _tmp465[15]="unary_operator";static char _tmp466[19]="postfix_expression";
static char _tmp467[19]="primary_expression";static char _tmp468[25]="argument_expression_list";
static char _tmp469[26]="argument_expression_list0";static char _tmp46A[9]="constant";
static char _tmp46B[20]="qual_opt_identifier";static char _tmp46C[17]="qual_opt_typedef";
static char _tmp46D[18]="struct_union_name";static char _tmp46E[11]="field_name";
static char _tmp46F[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[287]={{
_tmp351,_tmp351,_tmp351 + 2},{_tmp352,_tmp352,_tmp352 + 6},{_tmp353,_tmp353,
_tmp353 + 12},{_tmp354,_tmp354,_tmp354 + 5},{_tmp355,_tmp355,_tmp355 + 9},{_tmp356,
_tmp356,_tmp356 + 7},{_tmp357,_tmp357,_tmp357 + 7},{_tmp358,_tmp358,_tmp358 + 8},{
_tmp359,_tmp359,_tmp359 + 5},{_tmp35A,_tmp35A,_tmp35A + 5},{_tmp35B,_tmp35B,
_tmp35B + 6},{_tmp35C,_tmp35C,_tmp35C + 4},{_tmp35D,_tmp35D,_tmp35D + 5},{_tmp35E,
_tmp35E,_tmp35E + 6},{_tmp35F,_tmp35F,_tmp35F + 7},{_tmp360,_tmp360,_tmp360 + 7},{
_tmp361,_tmp361,_tmp361 + 9},{_tmp362,_tmp362,_tmp362 + 6},{_tmp363,_tmp363,
_tmp363 + 9},{_tmp364,_tmp364,_tmp364 + 9},{_tmp365,_tmp365,_tmp365 + 7},{_tmp366,
_tmp366,_tmp366 + 6},{_tmp367,_tmp367,_tmp367 + 5},{_tmp368,_tmp368,_tmp368 + 8},{
_tmp369,_tmp369,_tmp369 + 7},{_tmp36A,_tmp36A,_tmp36A + 7},{_tmp36B,_tmp36B,
_tmp36B + 9},{_tmp36C,_tmp36C,_tmp36C + 3},{_tmp36D,_tmp36D,_tmp36D + 5},{_tmp36E,
_tmp36E,_tmp36E + 7},{_tmp36F,_tmp36F,_tmp36F + 6},{_tmp370,_tmp370,_tmp370 + 3},{
_tmp371,_tmp371,_tmp371 + 4},{_tmp372,_tmp372,_tmp372 + 5},{_tmp373,_tmp373,
_tmp373 + 9},{_tmp374,_tmp374,_tmp374 + 6},{_tmp375,_tmp375,_tmp375 + 7},{_tmp376,
_tmp376,_tmp376 + 5},{_tmp377,_tmp377,_tmp377 + 8},{_tmp378,_tmp378,_tmp378 + 4},{
_tmp379,_tmp379,_tmp379 + 6},{_tmp37A,_tmp37A,_tmp37A + 4},{_tmp37B,_tmp37B,
_tmp37B + 6},{_tmp37C,_tmp37C,_tmp37C + 7},{_tmp37D,_tmp37D,_tmp37D + 4},{_tmp37E,
_tmp37E,_tmp37E + 9},{_tmp37F,_tmp37F,_tmp37F + 9},{_tmp380,_tmp380,_tmp380 + 6},{
_tmp381,_tmp381,_tmp381 + 10},{_tmp382,_tmp382,_tmp382 + 9},{_tmp383,_tmp383,
_tmp383 + 8},{_tmp384,_tmp384,_tmp384 + 7},{_tmp385,_tmp385,_tmp385 + 7},{_tmp386,
_tmp386,_tmp386 + 8},{_tmp387,_tmp387,_tmp387 + 7},{_tmp388,_tmp388,_tmp388 + 8},{
_tmp389,_tmp389,_tmp389 + 5},{_tmp38A,_tmp38A,_tmp38A + 9},{_tmp38B,_tmp38B,
_tmp38B + 6},{_tmp38C,_tmp38C,_tmp38C + 7},{_tmp38D,_tmp38D,_tmp38D + 5},{_tmp38E,
_tmp38E,_tmp38E + 8},{_tmp38F,_tmp38F,_tmp38F + 13},{_tmp390,_tmp390,_tmp390 + 16},{
_tmp391,_tmp391,_tmp391 + 14},{_tmp392,_tmp392,_tmp392 + 12},{_tmp393,_tmp393,
_tmp393 + 7},{_tmp394,_tmp394,_tmp394 + 8},{_tmp395,_tmp395,_tmp395 + 12},{_tmp396,
_tmp396,_tmp396 + 6},{_tmp397,_tmp397,_tmp397 + 8},{_tmp398,_tmp398,_tmp398 + 8},{
_tmp399,_tmp399,_tmp399 + 10},{_tmp39A,_tmp39A,_tmp39A + 9},{_tmp39B,_tmp39B,
_tmp39B + 13},{_tmp39C,_tmp39C,_tmp39C + 10},{_tmp39D,_tmp39D,_tmp39D + 9},{_tmp39E,
_tmp39E,_tmp39E + 13},{_tmp39F,_tmp39F,_tmp39F + 14},{_tmp3A0,_tmp3A0,_tmp3A0 + 12},{
_tmp3A1,_tmp3A1,_tmp3A1 + 16},{_tmp3A2,_tmp3A2,_tmp3A2 + 15},{_tmp3A3,_tmp3A3,
_tmp3A3 + 7},{_tmp3A4,_tmp3A4,_tmp3A4 + 7},{_tmp3A5,_tmp3A5,_tmp3A5 + 7},{_tmp3A6,
_tmp3A6,_tmp3A6 + 8},{_tmp3A7,_tmp3A7,_tmp3A7 + 9},{_tmp3A8,_tmp3A8,_tmp3A8 + 6},{
_tmp3A9,_tmp3A9,_tmp3A9 + 6},{_tmp3AA,_tmp3AA,_tmp3AA + 6},{_tmp3AB,_tmp3AB,
_tmp3AB + 6},{_tmp3AC,_tmp3AC,_tmp3AC + 7},{_tmp3AD,_tmp3AD,_tmp3AD + 6},{_tmp3AE,
_tmp3AE,_tmp3AE + 11},{_tmp3AF,_tmp3AF,_tmp3AF + 11},{_tmp3B0,_tmp3B0,_tmp3B0 + 11},{
_tmp3B1,_tmp3B1,_tmp3B1 + 11},{_tmp3B2,_tmp3B2,_tmp3B2 + 11},{_tmp3B3,_tmp3B3,
_tmp3B3 + 12},{_tmp3B4,_tmp3B4,_tmp3B4 + 13},{_tmp3B5,_tmp3B5,_tmp3B5 + 11},{
_tmp3B6,_tmp3B6,_tmp3B6 + 11},{_tmp3B7,_tmp3B7,_tmp3B7 + 10},{_tmp3B8,_tmp3B8,
_tmp3B8 + 9},{_tmp3B9,_tmp3B9,_tmp3B9 + 11},{_tmp3BA,_tmp3BA,_tmp3BA + 12},{_tmp3BB,
_tmp3BB,_tmp3BB + 11},{_tmp3BC,_tmp3BC,_tmp3BC + 17},{_tmp3BD,_tmp3BD,_tmp3BD + 7},{
_tmp3BE,_tmp3BE,_tmp3BE + 19},{_tmp3BF,_tmp3BF,_tmp3BF + 18},{_tmp3C0,_tmp3C0,
_tmp3C0 + 9},{_tmp3C1,_tmp3C1,_tmp3C1 + 13},{_tmp3C2,_tmp3C2,_tmp3C2 + 16},{_tmp3C3,
_tmp3C3,_tmp3C3 + 18},{_tmp3C4,_tmp3C4,_tmp3C4 + 10},{_tmp3C5,_tmp3C5,_tmp3C5 + 4},{
_tmp3C6,_tmp3C6,_tmp3C6 + 4},{_tmp3C7,_tmp3C7,_tmp3C7 + 4},{_tmp3C8,_tmp3C8,
_tmp3C8 + 4},{_tmp3C9,_tmp3C9,_tmp3C9 + 4},{_tmp3CA,_tmp3CA,_tmp3CA + 4},{_tmp3CB,
_tmp3CB,_tmp3CB + 4},{_tmp3CC,_tmp3CC,_tmp3CC + 4},{_tmp3CD,_tmp3CD,_tmp3CD + 4},{
_tmp3CE,_tmp3CE,_tmp3CE + 4},{_tmp3CF,_tmp3CF,_tmp3CF + 4},{_tmp3D0,_tmp3D0,
_tmp3D0 + 4},{_tmp3D1,_tmp3D1,_tmp3D1 + 4},{_tmp3D2,_tmp3D2,_tmp3D2 + 4},{_tmp3D3,
_tmp3D3,_tmp3D3 + 4},{_tmp3D4,_tmp3D4,_tmp3D4 + 4},{_tmp3D5,_tmp3D5,_tmp3D5 + 4},{
_tmp3D6,_tmp3D6,_tmp3D6 + 4},{_tmp3D7,_tmp3D7,_tmp3D7 + 4},{_tmp3D8,_tmp3D8,
_tmp3D8 + 4},{_tmp3D9,_tmp3D9,_tmp3D9 + 4},{_tmp3DA,_tmp3DA,_tmp3DA + 4},{_tmp3DB,
_tmp3DB,_tmp3DB + 4},{_tmp3DC,_tmp3DC,_tmp3DC + 4},{_tmp3DD,_tmp3DD,_tmp3DD + 4},{
_tmp3DE,_tmp3DE,_tmp3DE + 4},{_tmp3DF,_tmp3DF,_tmp3DF + 4},{_tmp3E0,_tmp3E0,
_tmp3E0 + 4},{_tmp3E1,_tmp3E1,_tmp3E1 + 5},{_tmp3E2,_tmp3E2,_tmp3E2 + 17},{_tmp3E3,
_tmp3E3,_tmp3E3 + 12},{_tmp3E4,_tmp3E4,_tmp3E4 + 19},{_tmp3E5,_tmp3E5,_tmp3E5 + 21},{
_tmp3E6,_tmp3E6,_tmp3E6 + 20},{_tmp3E7,_tmp3E7,_tmp3E7 + 21},{_tmp3E8,_tmp3E8,
_tmp3E8 + 13},{_tmp3E9,_tmp3E9,_tmp3E9 + 15},{_tmp3EA,_tmp3EA,_tmp3EA + 17},{
_tmp3EB,_tmp3EB,_tmp3EB + 19},{_tmp3EC,_tmp3EC,_tmp3EC + 12},{_tmp3ED,_tmp3ED,
_tmp3ED + 19},{_tmp3EE,_tmp3EE,_tmp3EE + 17},{_tmp3EF,_tmp3EF,_tmp3EF + 23},{
_tmp3F0,_tmp3F0,_tmp3F0 + 24},{_tmp3F1,_tmp3F1,_tmp3F1 + 15},{_tmp3F2,_tmp3F2,
_tmp3F2 + 11},{_tmp3F3,_tmp3F3,_tmp3F3 + 15},{_tmp3F4,_tmp3F4,_tmp3F4 + 10},{
_tmp3F5,_tmp3F5,_tmp3F5 + 15},{_tmp3F6,_tmp3F6,_tmp3F6 + 25},{_tmp3F7,_tmp3F7,
_tmp3F7 + 5},{_tmp3F8,_tmp3F8,_tmp3F8 + 15},{_tmp3F9,_tmp3F9,_tmp3F9 + 15},{_tmp3FA,
_tmp3FA,_tmp3FA + 11},{_tmp3FB,_tmp3FB,_tmp3FB + 22},{_tmp3FC,_tmp3FC,_tmp3FC + 26},{
_tmp3FD,_tmp3FD,_tmp3FD + 16},{_tmp3FE,_tmp3FE,_tmp3FE + 16},{_tmp3FF,_tmp3FF,
_tmp3FF + 24},{_tmp400,_tmp400,_tmp400 + 25},{_tmp401,_tmp401,_tmp401 + 21},{
_tmp402,_tmp402,_tmp402 + 22},{_tmp403,_tmp403,_tmp403 + 16},{_tmp404,_tmp404,
_tmp404 + 19},{_tmp405,_tmp405,_tmp405 + 25},{_tmp406,_tmp406,_tmp406 + 35},{
_tmp407,_tmp407,_tmp407 + 23},{_tmp408,_tmp408,_tmp408 + 24},{_tmp409,_tmp409,
_tmp409 + 18},{_tmp40A,_tmp40A,_tmp40A + 19},{_tmp40B,_tmp40B,_tmp40B + 14},{
_tmp40C,_tmp40C,_tmp40C + 19},{_tmp40D,_tmp40D,_tmp40D + 20},{_tmp40E,_tmp40E,
_tmp40E + 14},{_tmp40F,_tmp40F,_tmp40F + 11},{_tmp410,_tmp410,_tmp410 + 23},{
_tmp411,_tmp411,_tmp411 + 18},{_tmp412,_tmp412,_tmp412 + 30},{_tmp413,_tmp413,
_tmp413 + 8},{_tmp414,_tmp414,_tmp414 + 12},{_tmp415,_tmp415,_tmp415 + 14},{_tmp416,
_tmp416,_tmp416 + 13},{_tmp417,_tmp417,_tmp417 + 23},{_tmp418,_tmp418,_tmp418 + 14},{
_tmp419,_tmp419,_tmp419 + 18},{_tmp41A,_tmp41A,_tmp41A + 8},{_tmp41B,_tmp41B,
_tmp41B + 11},{_tmp41C,_tmp41C,_tmp41C + 20},{_tmp41D,_tmp41D,_tmp41D + 9},{_tmp41E,
_tmp41E,_tmp41E + 16},{_tmp41F,_tmp41F,_tmp41F + 19},{_tmp420,_tmp420,_tmp420 + 10},{
_tmp421,_tmp421,_tmp421 + 16},{_tmp422,_tmp422,_tmp422 + 11},{_tmp423,_tmp423,
_tmp423 + 14},{_tmp424,_tmp424,_tmp424 + 11},{_tmp425,_tmp425,_tmp425 + 15},{
_tmp426,_tmp426,_tmp426 + 22},{_tmp427,_tmp427,_tmp427 + 16},{_tmp428,_tmp428,
_tmp428 + 17},{_tmp429,_tmp429,_tmp429 + 12},{_tmp42A,_tmp42A,_tmp42A + 18},{
_tmp42B,_tmp42B,_tmp42B + 17},{_tmp42C,_tmp42C,_tmp42C + 12},{_tmp42D,_tmp42D,
_tmp42D + 16},{_tmp42E,_tmp42E,_tmp42E + 11},{_tmp42F,_tmp42F,_tmp42F + 10},{
_tmp430,_tmp430,_tmp430 + 14},{_tmp431,_tmp431,_tmp431 + 15},{_tmp432,_tmp432,
_tmp432 + 20},{_tmp433,_tmp433,_tmp433 + 27},{_tmp434,_tmp434,_tmp434 + 10},{
_tmp435,_tmp435,_tmp435 + 13},{_tmp436,_tmp436,_tmp436 + 18},{_tmp437,_tmp437,
_tmp437 + 21},{_tmp438,_tmp438,_tmp438 + 19},{_tmp439,_tmp439,_tmp439 + 16},{
_tmp43A,_tmp43A,_tmp43A + 20},{_tmp43B,_tmp43B,_tmp43B + 15},{_tmp43C,_tmp43C,
_tmp43C + 20},{_tmp43D,_tmp43D,_tmp43D + 15},{_tmp43E,_tmp43E,_tmp43E + 12},{
_tmp43F,_tmp43F,_tmp43F + 20},{_tmp440,_tmp440,_tmp440 + 19},{_tmp441,_tmp441,
_tmp441 + 20},{_tmp442,_tmp442,_tmp442 + 21},{_tmp443,_tmp443,_tmp443 + 21},{
_tmp444,_tmp444,_tmp444 + 12},{_tmp445,_tmp445,_tmp445 + 17},{_tmp446,_tmp446,
_tmp446 + 19},{_tmp447,_tmp447,_tmp447 + 14},{_tmp448,_tmp448,_tmp448 + 17},{
_tmp449,_tmp449,_tmp449 + 23},{_tmp44A,_tmp44A,_tmp44A + 13},{_tmp44B,_tmp44B,
_tmp44B + 14},{_tmp44C,_tmp44C,_tmp44C + 16},{_tmp44D,_tmp44D,_tmp44D + 16},{
_tmp44E,_tmp44E,_tmp44E + 8},{_tmp44F,_tmp44F,_tmp44F + 19},{_tmp450,_tmp450,
_tmp450 + 20},{_tmp451,_tmp451,_tmp451 + 14},{_tmp452,_tmp452,_tmp452 + 19},{
_tmp453,_tmp453,_tmp453 + 20},{_tmp454,_tmp454,_tmp454 + 11},{_tmp455,_tmp455,
_tmp455 + 22},{_tmp456,_tmp456,_tmp456 + 20},{_tmp457,_tmp457,_tmp457 + 23},{
_tmp458,_tmp458,_tmp458 + 20},{_tmp459,_tmp459,_tmp459 + 22},{_tmp45A,_tmp45A,
_tmp45A + 23},{_tmp45B,_tmp45B,_tmp45B + 24},{_tmp45C,_tmp45C,_tmp45C + 24},{
_tmp45D,_tmp45D,_tmp45D + 15},{_tmp45E,_tmp45E,_tmp45E + 20},{_tmp45F,_tmp45F,
_tmp45F + 22},{_tmp460,_tmp460,_tmp460 + 17},{_tmp461,_tmp461,_tmp461 + 20},{
_tmp462,_tmp462,_tmp462 + 26},{_tmp463,_tmp463,_tmp463 + 16},{_tmp464,_tmp464,
_tmp464 + 17},{_tmp465,_tmp465,_tmp465 + 15},{_tmp466,_tmp466,_tmp466 + 19},{
_tmp467,_tmp467,_tmp467 + 19},{_tmp468,_tmp468,_tmp468 + 25},{_tmp469,_tmp469,
_tmp469 + 26},{_tmp46A,_tmp46A,_tmp46A + 9},{_tmp46B,_tmp46B,_tmp46B + 20},{_tmp46C,
_tmp46C,_tmp46C + 17},{_tmp46D,_tmp46D,_tmp46D + 18},{_tmp46E,_tmp46E,_tmp46E + 11},{
_tmp46F,_tmp46F,_tmp46F + 12}};static short Cyc_yyr1[513]={0,144,145,145,145,145,
145,145,145,145,145,145,146,146,147,147,147,148,148,148,149,149,149,149,150,150,
151,152,153,154,155,155,155,155,155,155,155,156,156,157,157,158,158,158,158,158,
158,158,158,158,158,159,159,159,159,159,159,159,160,160,161,162,162,163,163,163,
163,163,163,164,164,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,
165,165,165,165,165,165,165,165,166,167,167,167,168,168,168,169,169,170,170,170,
171,171,171,171,171,172,172,173,173,174,174,175,175,176,177,177,178,178,179,180,
180,180,180,180,180,181,181,181,181,181,181,182,183,183,184,184,184,184,185,185,
185,186,186,187,187,187,187,188,188,188,189,189,190,190,191,191,192,192,192,192,
192,192,192,192,192,192,193,193,193,193,193,193,193,193,193,193,193,194,194,195,
196,196,197,197,197,197,197,197,197,197,198,198,198,199,199,200,200,200,201,201,
201,202,202,203,203,203,203,204,204,205,205,206,206,207,207,208,208,209,209,210,
210,210,210,211,211,212,212,213,213,213,214,215,215,216,216,217,217,217,217,218,
218,218,218,219,220,220,221,221,222,222,223,223,223,223,223,224,224,225,225,225,
226,226,226,226,226,226,226,226,226,226,226,227,227,227,227,227,227,227,228,228,
229,230,230,231,231,232,232,232,232,232,232,233,233,233,233,233,233,234,234,234,
234,234,234,235,235,235,235,235,235,235,235,235,235,235,235,235,235,236,236,236,
236,236,236,236,236,237,238,238,239,239,240,240,241,241,242,242,243,243,244,244,
244,245,245,245,245,245,246,246,246,247,247,247,248,248,248,248,249,249,250,250,
250,250,250,250,250,251,252,253,253,253,253,253,253,253,253,253,253,253,253,253,
253,253,254,254,254,255,255,256,256,257,257,257,258,258,259,259,260,260,260,261,
261,261,261,261,261,261,261,261,261,261,262,262,262,262,262,262,262,263,264,264,
265,265,266,266,267,267,268,268,269,269,269,270,270,270,270,270,271,271,271,272,
272,272,273,273,273,273,274,274,275,275,275,275,275,275,275,275,275,275,275,275,
275,275,275,275,275,275,275,275,276,276,276,277,277,277,277,277,277,277,277,277,
277,278,278,278,278,278,278,278,278,278,279,280,280,281,281,281,281,282,282,283,
283,284,284,285,285,286,286};static short Cyc_yyr2[513]={0,1,2,3,5,3,5,6,7,3,3,0,4,
3,1,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,8,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,
1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,
4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,
1,2,1,2,1,1,3,1,2,0,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,
1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,
1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,
2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,
1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,
5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,
1,3,1,1,3,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,
1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,
4,1,2,2,2,2,2,2,4,2,6,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,
4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[1028]={0,19,51,
52,53,54,56,71,74,75,76,77,78,79,80,81,95,96,97,113,114,47,0,0,57,0,0,147,88,92,
0,0,0,0,0,0,0,38,503,213,505,504,506,0,0,72,0,199,199,198,1,0,17,0,0,18,0,0,41,
49,43,69,45,82,83,0,84,0,37,158,0,183,186,85,162,111,55,54,48,0,99,502,0,503,499,
500,501,111,0,373,0,0,0,0,236,0,375,376,26,28,0,0,0,0,0,0,0,0,148,0,0,0,0,0,0,0,
196,197,2,0,0,0,0,0,30,0,119,120,122,42,50,44,46,115,507,508,111,111,37,39,37,0,
20,0,215,0,171,159,184,193,192,0,0,190,191,194,195,204,186,0,70,0,55,103,0,101,0,
503,383,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,489,473,0,0,0,0,0,476,474,475,0,400,
402,416,424,426,428,430,432,434,437,442,445,448,452,0,454,477,488,487,0,384,382,
33,0,0,111,0,0,0,129,125,127,254,256,0,0,9,10,0,0,0,111,509,510,214,94,0,0,163,
73,234,0,231,0,3,0,5,0,284,0,31,0,0,37,21,0,116,117,142,110,0,145,0,0,0,0,0,0,0,
0,0,0,0,0,503,287,289,0,295,291,0,293,277,278,279,0,280,281,282,0,40,22,122,261,
0,221,237,0,0,217,215,0,201,0,0,0,206,58,205,187,0,0,104,100,0,0,377,0,0,397,0,0,
0,0,250,393,398,0,395,0,462,0,418,452,0,419,420,0,0,0,0,0,0,0,0,0,455,456,37,0,0,
0,458,459,457,0,374,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,406,407,408,409,410,
411,412,413,414,415,405,0,460,0,483,484,0,0,0,491,0,111,390,391,0,388,0,238,0,0,
0,0,257,229,0,130,135,131,133,126,128,215,0,263,255,264,512,511,0,87,91,0,89,0,
93,109,64,63,0,61,164,215,233,160,263,235,172,173,0,86,200,27,0,29,0,0,0,0,121,
123,240,239,23,106,118,0,0,0,137,138,140,0,111,111,153,0,0,0,0,0,0,37,0,324,325,
326,0,0,328,0,0,0,0,296,292,122,294,290,288,0,170,222,0,0,0,228,216,223,167,0,0,
0,217,169,203,202,165,201,0,0,207,59,112,0,105,423,102,98,386,387,253,0,394,249,
251,381,0,0,0,0,241,245,0,0,0,0,0,0,0,0,0,0,0,0,0,490,497,0,496,401,425,0,427,
429,431,433,435,436,440,441,438,439,443,444,446,447,449,450,451,404,403,482,479,
0,481,0,0,0,378,0,385,32,0,379,0,0,258,136,132,134,0,217,0,201,0,265,0,215,0,276,
260,0,0,111,0,0,0,129,0,111,0,215,0,182,161,232,4,6,0,35,0,0,141,124,142,0,217,
146,155,154,0,0,149,0,0,0,303,0,0,0,0,0,323,327,0,0,0,0,286,37,24,262,215,0,225,
0,0,218,0,168,221,209,166,189,188,207,185,0,7,0,252,396,399,461,0,0,242,0,246,
465,0,0,0,0,469,472,0,0,0,0,0,453,493,0,0,480,478,0,0,389,392,380,259,230,272,0,
266,267,201,0,0,217,201,0,90,0,217,0,0,0,60,62,0,174,0,0,217,0,201,0,0,0,139,143,
115,144,156,153,153,0,0,0,0,0,0,0,0,0,0,0,0,0,303,329,0,0,25,217,0,226,224,0,217,
0,208,0,8,0,0,0,243,247,0,0,0,0,421,422,482,481,495,0,498,417,492,494,0,271,269,
275,274,0,268,201,0,115,0,67,65,66,175,181,178,0,180,176,201,0,34,0,0,152,151,
297,303,0,0,0,0,0,0,331,332,334,336,338,340,342,344,347,352,355,358,362,364,371,
372,0,37,300,309,0,0,0,0,0,0,0,0,0,0,330,283,211,227,219,210,215,13,0,14,464,463,
0,248,466,0,0,471,470,485,0,273,270,36,0,0,179,177,285,108,0,0,0,303,0,368,0,0,
365,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,366,304,0,311,0,0,0,319,0,0,0,0,0,
0,302,0,217,12,15,0,0,0,0,486,107,0,157,298,299,0,0,0,0,303,305,335,0,337,339,
341,343,345,346,350,351,348,349,353,354,356,357,359,360,361,0,310,312,313,0,321,
320,0,315,0,0,0,220,212,16,0,0,0,0,301,367,0,363,306,0,37,314,322,316,317,0,244,
467,0,68,0,0,333,303,307,318,0,370,369,308,468,0,0,0};static short Cyc_yydefgoto[
143]={1025,50,683,873,51,52,285,53,456,54,458,55,56,140,57,58,524,226,442,443,
227,61,241,228,63,163,164,64,160,65,262,263,125,126,127,264,229,423,471,472,473,
66,67,644,645,646,68,474,69,447,70,71,157,158,72,116,520,314,680,604,73,605,514,
671,506,510,511,418,307,249,93,94,545,464,546,329,330,331,230,300,301,606,429,
288,460,289,290,291,292,293,758,294,295,834,835,836,837,838,839,840,841,842,843,
844,845,846,847,848,849,332,408,409,333,334,335,296,197,395,198,530,199,200,201,
202,203,204,205,206,207,208,209,210,211,212,213,561,562,214,215,75,874,242,433};
static short Cyc_yypact[1028]={2598,- -32768,- -32768,- -32768,- -32768,- 73,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,3102,322,2292,- -32768,- 65,- 59,- -32768,- 68,- 17,84,
154,17,151,179,361,226,- -32768,- -32768,200,- -32768,- -32768,- -32768,269,486,245,
279,287,287,- -32768,- -32768,2463,- -32768,324,359,- -32768,356,712,3102,3102,3102,
- -32768,3102,- -32768,- -32768,404,- -32768,- 65,3023,338,198,752,972,- -32768,-
-32768,308,334,387,- -32768,- 65,388,- -32768,2427,270,- -32768,- -32768,- -32768,308,
5684,- -32768,415,456,2427,454,459,460,- -32768,49,- -32768,- -32768,3248,3248,2463,
2463,3248,484,5684,437,- -32768,86,465,862,500,86,3467,5684,- -32768,- -32768,-
-32768,2463,2732,2463,2732,113,- -32768,525,512,- -32768,2943,- -32768,- -32768,-
-32768,- -32768,3467,- -32768,- -32768,308,130,1489,- -32768,3023,712,- -32768,3248,
3175,4088,- -32768,338,- -32768,- -32768,- -32768,522,524,- -32768,- -32768,- -32768,-
-32768,76,972,3248,- -32768,2732,- -32768,540,551,568,- 65,150,- -32768,2427,186,
2771,5860,567,5684,5734,592,604,625,634,655,685,687,711,5936,5936,- -32768,-
-32768,2048,713,5986,5986,5986,- -32768,- -32768,- -32768,164,- -32768,- -32768,- 22,
748,705,717,720,430,126,595,89,343,- -32768,599,5986,234,66,- -32768,746,770,743,-
-32768,- -32768,754,5684,308,770,757,- 33,3467,3540,3467,579,- -32768,131,131,-
-32768,- -32768,135,772,319,308,- -32768,- -32768,- -32768,- -32768,25,768,- -32768,-
-32768,700,329,- -32768,781,- -32768,784,- -32768,792,798,812,- -32768,862,4184,3023,
- -32768,816,3467,- -32768,859,834,- 65,836,837,310,839,3608,840,852,853,864,4280,
3608,285,845,848,- -32768,- -32768,854,1630,1630,712,1630,- -32768,- -32768,- -32768,
863,- -32768,- -32768,- -32768,232,- -32768,- -32768,867,849,102,879,- -32768,68,873,
869,455,874,641,858,3248,5684,- -32768,886,- -32768,- -32768,102,884,- 65,- -32768,
5684,887,- -32768,882,885,- -32768,86,5684,2427,358,- -32768,- -32768,- -32768,891,
905,2048,- -32768,3467,- -32768,- -32768,3704,- -32768,910,5684,5684,5684,5684,5684,
5684,3467,99,2048,- -32768,- -32768,1771,902,413,5684,- -32768,- -32768,- -32768,5684,
- -32768,5986,5684,5986,5986,5986,5986,5986,5986,5986,5986,5986,5986,5986,5986,
5986,5986,5986,5986,5986,5684,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,5684,- -32768,86,- -32768,- -32768,4373,86,
5684,- -32768,906,308,- -32768,- -32768,904,911,2427,- -32768,485,2771,913,3248,-
-32768,919,922,- -32768,3540,3540,3540,- -32768,- -32768,2811,4466,203,- -32768,350,
- -32768,- -32768,68,- -32768,- -32768,3248,- -32768,924,- -32768,925,- -32768,920,926,
933,- -32768,1003,- -32768,528,804,- -32768,- -32768,- -32768,3467,- -32768,- -32768,-
-32768,2463,- -32768,2463,939,937,940,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,1066,5684,945,944,- -32768,938,517,308,308,735,5684,5684,941,949,
5684,1040,1912,960,- -32768,- -32768,- -32768,549,1037,- -32768,4562,5684,3608,2184,
- -32768,- -32768,2943,- -32768,- -32768,- -32768,3248,- -32768,- -32768,3467,956,190,-
-32768,- -32768,946,- -32768,68,959,3394,869,- -32768,- -32768,- -32768,- -32768,641,
56,961,872,- -32768,- -32768,2328,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,954,- -32768,- -32768,- -32768,- -32768,3258,962,968,983,- -32768,- -32768,701,
4184,421,971,973,974,478,488,970,624,975,978,5810,- -32768,- -32768,977,979,-
-32768,748,209,705,717,720,430,126,126,595,595,595,595,89,89,343,343,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,980,- -32768,28,3248,3992,- -32768,
3404,- -32768,- -32768,984,- -32768,65,987,- -32768,- -32768,- -32768,- -32768,982,869,
985,641,981,350,3248,3321,4655,- -32768,- -32768,131,990,308,823,988,25,1148,994,
308,3248,3175,4751,- -32768,528,- -32768,- -32768,- -32768,997,- -32768,998,1070,-
-32768,- -32768,859,5684,869,- -32768,- -32768,- -32768,989,- 65,562,491,502,5684,883,
515,999,4847,4940,702,- -32768,- -32768,1004,1007,1000,534,- -32768,3023,- -32768,
849,1009,3248,- -32768,1012,68,- -32768,1005,- -32768,229,- -32768,- -32768,- -32768,-
-32768,872,- -32768,1014,- -32768,2463,- -32768,- -32768,- -32768,1020,260,1018,-
-32768,3800,- -32768,- -32768,5684,1116,5684,5734,- -32768,- -32768,86,86,1020,1017,
3992,- -32768,- -32768,5684,5684,- -32768,- -32768,102,794,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,1019,- -32768,- -32768,641,102,1021,869,641,1022,- -32768,
5684,869,552,1023,1024,- -32768,- -32768,1026,- -32768,102,1027,869,1029,641,1039,
5684,1038,- -32768,- -32768,3467,- -32768,1049,32,735,3608,1025,1052,6101,1058,1068,
3608,5684,5033,731,5126,736,5219,883,- -32768,1071,1072,- -32768,869,95,- -32768,-
-32768,1080,869,3467,- -32768,617,- -32768,1067,1076,5684,- -32768,- -32768,4184,614,
1069,1087,- -32768,910,1083,1084,- -32768,813,- -32768,- -32768,- -32768,- -32768,3992,
- -32768,- -32768,- -32768,- -32768,1085,- -32768,641,742,3467,1089,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,1086,- -32768,- -32768,641,631,- -32768,1094,3467,-
-32768,- -32768,1186,883,1097,6062,1093,2184,5986,1090,- -32768,- 18,- -32768,1131,
1095,800,855,208,861,118,357,- -32768,- -32768,- -32768,- -32768,1134,5986,1771,-
-32768,- -32768,656,3608,660,5312,3608,662,5405,5498,751,1111,- -32768,- -32768,-
-32768,- -32768,1112,- -32768,1009,- -32768,1114,758,- -32768,- -32768,233,- -32768,-
-32768,3467,1209,- -32768,- -32768,- -32768,3896,- -32768,- -32768,- -32768,1117,1115,
- -32768,- -32768,- -32768,- -32768,666,3608,1119,883,2048,- -32768,3467,1120,- -32768,
1348,5986,5684,5986,5986,5986,5986,5986,5986,5986,5986,5986,5986,5986,5986,5986,
5986,5986,5986,5986,5684,- -32768,- -32768,1122,- -32768,3608,3608,674,- -32768,3608,
3608,677,3608,690,5591,- -32768,68,869,- -32768,- -32768,437,5684,1121,1118,- -32768,
- -32768,1133,- -32768,- -32768,- -32768,1128,1123,1129,5986,883,- -32768,748,250,705,
717,717,430,126,126,595,595,595,595,89,89,343,343,- -32768,- -32768,- -32768,304,-
-32768,- -32768,- -32768,3608,- -32768,- -32768,3608,- -32768,3608,3608,703,- -32768,-
-32768,- -32768,830,1125,3467,1127,- -32768,1020,402,- -32768,- -32768,5684,1348,-
-32768,- -32768,- -32768,- -32768,3608,- -32768,- -32768,1130,- -32768,1132,1139,-
-32768,883,- -32768,- -32768,1140,- -32768,- -32768,- -32768,- -32768,1241,1253,-
-32768};static short Cyc_yypgoto[143]={- -32768,159,- -32768,323,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- 49,- -32768,- 104,22,- -32768,- -32768,0,646,
- -32768,55,- 177,1155,5,- -32768,- -32768,- 130,- -32768,110,1234,- 699,- -32768,-
-32768,- -32768,1015,1013,447,520,- -32768,- -32768,632,- -32768,- -32768,205,- -32768,
- -32768,127,- 227,1207,- 387,- 14,- -32768,1126,- -32768,- -32768,1230,- 403,- -32768,
600,- 133,- 140,- 129,7,345,609,616,- 409,- 469,- 105,- 406,- 131,- -32768,- 234,- 171,- 558,
- 240,- -32768,957,- 156,- 70,- 119,- 75,- 361,- 9,- -32768,- -32768,- -32768,4,- 278,-
-32768,- 492,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
71,1065,- -32768,749,877,- -32768,313,443,- -32768,- 172,- 301,- 169,- 352,- 350,- 353,
923,- 347,- 342,- 277,- 324,- 286,- 56,393,536,942,- -32768,- 382,- -32768,60,277,- 62,34,
- 326,- 67};static short Cyc_yytable[6245]={59,534,339,135,342,62,343,497,498,248,
500,305,564,308,567,306,566,315,587,139,446,59,569,614,260,463,62,535,570,571,
231,232,356,712,235,76,322,642,643,669,317,38,440,78,111,135,629,99,41,823,421,
59,576,577,100,60,62,148,59,59,59,628,59,62,62,62,609,62,59,146,364,585,142,62,
905,588,60,557,39,139,129,130,131,96,132,417,416,609,628,286,141,297,578,579,95,
572,573,574,575,136,666,547,59,59,672,101,60,62,62,629,889,660,365,60,60,60,906,
60,676,59,59,59,59,60,62,62,62,62,59,507,717,441,261,62,359,360,361,81,59,104,59,
238,96,62,298,62,795,146,362,529,141,- 150,96,168,428,396,529,60,60,710,287,59,
141,218,509,434,62,222,436,636,403,112,449,223,60,60,60,60,516,39,541,677,542,60,
128,451,508,39,430,528,716,432,239,60,554,60,556,171,240,404,432,102,313,451,721,
38,84,186,85,86,118,297,41,372,373,427,60,430,622,255,868,430,503,352,378,431,
189,420,379,96,432,96,422,448,504,256,286,286,244,286,323,522,622,421,421,421,
266,268,374,375,526,475,159,919,431,435,169,920,431,267,59,233,234,484,466,62,
432,755,299,491,432,103,170,105,864,108,96,74,251,252,253,254,141,96,362,59,59,
407,59,363,62,62,509,62,407,913,914,324,529,80,97,39,98,106,38,109,286,287,287,
668,287,41,325,692,525,60,397,398,399,802,318,74,44,806,580,581,582,425,74,362,
915,916,413,426,74,505,897,708,747,819,60,60,134,60,137,598,304,74,439,502,113,
547,362,362,- 215,59,165,- 215,400,97,62,945,782,401,402,615,239,781,417,97,362,
871,240,509,792,793,169,287,1003,74,74,19,20,623,134,299,451,- 237,74,96,- 237,248,
170,110,736,663,74,74,74,74,536,196,492,114,887,115,954,451,736,493,60,427,959,
451,499,123,38,892,74,237,420,420,420,41,362,422,422,422,38,613,159,448,1004,665,
480,41,654,481,74,362,79,119,120,165,438,621,97,627,97,452,139,786,161,43,453,
475,59,785,59,417,143,62,144,62,547,43,1002,529,145,635,96,711,610,96,611,380,
121,122,724,537,612,594,725,381,382,59,662,327,328,921,62,723,739,97,741,162,740,
922,923,59,97,357,705,664,62,738,166,141,239,1014,38,60,1018,60,240,591,40,41,42,
370,371,141,133,675,450,1023,790,59,791,679,509,672,62,362,412,74,797,771,559,
216,60,362,450,38,477,776,693,482,728,40,41,42,878,960,60,963,964,962,250,623,
786,247,217,965,74,337,785,340,340,966,967,219,304,451,926,515,451,353,354,220,
265,221,60,340,340,340,640,641,310,243,490,247,38,972,973,236,165,772,362,41,96,
43,595,697,340,362,97,475,362,613,44,362,719,698,297,630,753,631,47,48,49,475,
362,38,475,245,958,754,627,40,41,42,258,974,975,362,968,969,970,971,759,469,257,
43,798,786,311,748,312,357,624,785,625,96,362,383,803,548,626,769,319,552,553,59,
714,357,657,770,62,362,320,815,810,419,1015,424,902,811,565,751,376,377,752,59,
679,141,682,97,62,321,97,338,384,385,386,387,388,389,390,391,392,393,465,425,518,
519,700,398,399,426,265,47,48,49,340,589,344,135,60,895,394,340,450,38,946,450,
1017,730,345,40,41,42,805,74,362,74,872,809,60,879,642,643,941,955,827,956,450,
817,400,346,854,362,450,701,402,523,893,340,347,340,340,340,340,340,340,340,340,
340,340,340,340,340,340,340,340,340,362,903,867,348,362,927,362,870,465,929,452,
933,549,550,551,951,647,648,362,925,651,362,655,983,509,560,986,286,74,563,38,
661,81,349,362,350,40,41,42,988,96,97,38,765,690,691,362,362,445,41,583,850,1009,
124,426,1016,47,48,49,351,44,358,584,366,1012,367,780,560,47,48,49,928,858,410,
932,362,59,861,368,286,362,62,369,888,411,82,362,340,405,976,977,978,938,608,97,
362,247,406,287,943,167,84,944,85,86,415,135,41,47,48,49,952,87,16,17,18,247,444,
88,437,89,90,450,247,454,1001,91,455,59,756,757,60,92,62,38,457,450,799,800,450,
40,41,42,459,981,982,750,461,984,985,287,987,445,731,732,733,884,885,426,467,560,
909,910,560,601,602,603,247,911,912,917,918,992,1010,362,340,476,247,478,286,825,
826,486,60,74,479,247,483,485,38,762,764,38,494,487,40,41,42,1005,41,495,1006,
496,1007,1008,488,501,469,432,505,44,470,259,521,465,47,48,49,47,48,49,513,512,
517,1019,43,364,527,59,532,531,787,533,62,539,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,540,287,558,590,592,616,593,340,97,465,149,150,151,597,599,22,600,808,617,
618,632,152,153,154,155,156,619,27,620,633,727,134,821,247,60,28,29,637,634,638,
649,639,650,419,743,652,32,247,855,857,34,860,656,863,658,667,670,35,36,673,684,
678,687,688,689,340,694,560,695,696,699,877,703,39,707,702,340,706,715,745,709,
302,718,749,38,720,729,722,734,39,40,41,42,43,737,304,744,766,760,767,768,304,
445,775,45,46,773,779,426,465,47,48,49,704,789,783,788,794,828,801,196,804,465,
812,813,796,814,816,807,818,822,7,8,9,10,11,12,13,14,15,16,17,18,19,20,820,931,
38,824,935,937,38,829,40,41,42,43,40,41,42,22,852,853,865,866,469,869,875,880,
469,265,890,27,47,48,49,876,47,48,49,28,29,881,882,883,886,891,357,894,896,898,
32,901,904,961,34,134,907,900,247,924,340,35,36,465,939,908,940,942,947,950,949,
979,953,980,997,1026,996,465,340,957,995,998,999,1000,1011,990,1013,1027,38,1020,
265,1021,994,39,40,41,42,43,1022,1024,735,993,246,107,746,247,469,462,45,46,468,
147,117,778,47,48,49,777,316,991,774,538,414,686,596,568,851,555,0,0,0,0,340,0,
340,340,340,340,340,340,340,340,340,340,340,340,340,340,340,340,340,0,0,0,0,0,0,
0,0,0,0,0,465,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,340,2,3,4,77,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,756,757,21,172,173,269,0,270,271,272,273,274,275,
276,277,22,81,23,174,278,0,0,175,24,279,0,340,27,0,0,176,177,178,179,0,28,29,0,
180,0,280,0,0,0,0,0,32,33,181,182,34,183,0,0,0,0,0,35,36,37,0,184,185,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,281,84,186,85,86,39,40,41,42,43,187,282,138,- 303,
0,0,0,0,188,0,45,284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,77,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,269,0,270,271,272,273,274,275,
276,277,22,81,23,174,278,0,0,175,24,279,0,0,27,0,0,176,177,178,179,0,28,29,0,180,
0,280,0,0,0,0,0,32,33,181,182,34,183,0,0,0,0,0,35,36,37,0,184,185,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,281,84,186,85,86,39,40,41,42,43,187,282,138,283,0,0,
0,0,188,0,45,284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,77,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,269,0,270,271,272,273,274,275,276,
277,22,81,23,174,278,0,0,175,24,279,0,0,27,0,0,176,177,178,179,0,28,29,- 37,180,0,
280,0,0,0,0,0,32,33,181,182,34,183,0,0,0,0,0,35,36,37,0,184,185,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,281,84,186,85,86,39,40,41,42,43,187,282,138,0,0,0,0,0,
188,0,45,284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,77,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,21,172,173,269,0,270,271,272,273,274,275,276,277,
22,81,23,174,278,0,0,175,24,279,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,280,0,
0,0,0,0,32,33,181,182,34,183,0,0,0,0,0,35,36,37,0,184,185,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,281,84,186,85,86,39,40,41,42,43,187,282,138,0,0,0,0,0,188,0,
45,284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,77,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,21,172,173,0,0,0,0,0,0,0,0,0,0,22,81,23,174,0,0,0,175,
24,0,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,0,0,0,0,0,0,32,33,181,182,34,183,
0,0,0,0,0,35,36,37,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,
85,86,39,40,41,42,43,187,653,0,0,0,0,0,0,188,0,45,284,0,0,0,0,190,0,0,191,0,0,
192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,172,173,0,0,0,0,
0,0,0,0,0,0,22,81,0,174,0,0,0,175,0,0,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,
0,0,0,0,0,0,32,0,181,182,34,183,0,0,0,0,0,35,36,0,0,184,185,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,39,40,41,42,43,187,0,355,0,0,0,0,0,188,0,
45,284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,0,172,173,0,0,0,0,0,0,0,0,0,0,22,81,0,174,0,0,0,175,0,0,0,0,27,0,0,
176,177,178,179,0,28,29,0,180,0,0,0,0,0,0,0,32,0,181,182,34,183,0,0,0,0,0,35,36,
0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,39,40,41,
42,43,187,0,0,0,0,0,0,0,188,0,45,284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,
- 11,1,81,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,
0,0,0,0,22,0,23,0,0,0,681,0,24,0,25,26,27,0,0,0,0,0,82,0,28,29,- 37,0,0,0,0,0,0,
30,31,32,33,83,84,34,85,86,0,0,41,0,35,36,37,87,0,0,0,0,0,88,0,89,90,0,0,0,0,91,
0,0,0,0,0,92,0,0,0,38,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,
0,47,48,49,- 11,1,81,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,
0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,82,0,28,29,- 37,0,0,
0,0,0,0,30,31,32,33,167,84,34,85,86,0,0,41,0,35,36,37,87,0,0,0,0,0,88,0,89,90,0,
0,0,0,91,0,0,0,0,0,92,0,0,0,38,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,
46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,-
37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,44,0,45,46,0,0,0,
0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,
0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,
0,30,31,32,33,0,0,34,0,0,0,0,81,0,35,36,37,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,0,38,0,0,0,0,39,40,41,42,43,22,0,0,- 11,0,0,0,0,44,0,45,46,27,
0,82,0,47,48,49,0,28,29,0,0,0,0,326,0,0,167,84,32,85,86,0,34,41,0,0,0,0,87,35,36,
0,0,0,88,0,89,90,0,327,328,0,91,0,0,0,0,0,92,0,0,0,0,302,0,0,0,0,0,0,0,39,40,0,
42,43,0,304,0,0,0,0,0,0,425,0,45,46,0,0,426,0,47,48,49,2,3,4,77,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,
0,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,
2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,
43,0,22,138,23,0,259,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,
32,33,0,0,34,0,0,0,0,0,0,35,36,37,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,0,138,0,0,0,0,0,24,0,45,46,27,0,0,0,0,0,
0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,39,40,0,42,43,0,0,0,0,0,0,27,
0,0,0,45,46,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,302,0,0,303,0,0,0,22,39,40,0,42,43,
0,304,0,0,0,81,27,0,0,0,45,46,0,0,28,29,0,0,224,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,
0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,82,0,0,0,0,0,0,0,0,
22,39,40,685,42,43,167,84,225,85,86,0,27,41,0,0,45,46,87,0,28,29,0,0,88,0,89,90,
0,327,328,32,91,0,0,34,0,0,92,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,0,0,0,0,0,0,302,0,0,0,0,0,0,22,39,40,0,42,43,0,304,0,0,0,81,27,0,0,0,45,46,0,
0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,0,0,0,82,0,674,0,0,0,0,0,0,22,39,40,713,42,43,167,84,0,85,
86,0,27,41,0,0,45,46,87,0,28,29,0,0,88,0,89,90,0,0,0,32,91,0,0,34,0,0,92,0,0,0,
35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,39,
40,0,42,43,0,0,0,0,0,0,27,0,0,0,45,46,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,
0,0,0,0,0,35,36,0,0,0,0,0,0,0,0,0,0,0,0,172,173,269,0,270,271,272,273,274,275,
276,277,0,81,0,174,278,0,39,175,0,279,43,0,0,0,0,176,177,178,179,0,0,45,46,180,0,
280,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,281,84,186,85,86,0,0,41,0,0,187,282,138,0,0,172,173,0,188,0,
0,189,543,0,0,0,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,
0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,0,341,544,0,172,
173,0,188,0,0,189,0,327,328,0,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,
176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,
184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,
0,341,784,0,172,173,0,188,0,0,189,0,327,328,0,190,0,81,191,174,0,192,193,175,0,
194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,
0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,
0,0,41,0,0,187,0,341,948,0,172,173,0,188,0,0,189,0,327,328,0,190,0,81,191,174,0,
192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,
182,0,183,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,186,85,86,0,0,41,0,0,187,0,341,0,0,172,173,0,188,0,0,189,0,327,328,0,190,0,81,
191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,
0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,0,0,0,0,172,173,0,188,0,0,189,0,0,0,309,
190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,
0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,0,341,0,0,172,173,0,188,0,0,189,0,
0,0,0,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,
180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,489,172,173,0,0,0,0,188,0,
0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,
180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,0,172,173,0,0,0,0,188,586,
0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,
180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,0,0,0,0,172,173,0,188,0,0,
189,0,0,0,607,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,
0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,0,172,173,0,0,0,0,
188,659,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,
0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,0,0,0,0,172,173,0,
188,0,0,189,0,0,0,726,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,
178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,0,0,0,0,
172,173,0,188,0,0,189,0,0,0,742,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,
176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,
184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,
761,172,173,0,0,0,0,188,0,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,
0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,
184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,
763,172,173,0,0,0,0,188,0,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,
0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,
184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,
0,172,173,0,0,0,0,188,856,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,
0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,
184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,
0,172,173,0,0,0,0,188,859,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,
0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,
184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,
862,172,173,0,0,0,0,188,0,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,
0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,
184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,
0,172,173,0,0,0,0,188,930,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,
0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,
184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,
0,172,173,0,0,0,0,188,934,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,
0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,
184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,
0,172,173,0,0,0,0,188,936,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,
0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,
184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,187,
0,172,173,0,0,0,0,188,989,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,
0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,172,173,0,0,0,0,
0,0,184,185,0,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,176,177,178,179,38,84,186,85,86,0,
0,41,0,0,187,0,0,0,181,182,0,183,188,0,0,189,0,0,0,0,190,184,185,191,0,0,192,193,
0,0,194,195,0,0,0,0,0,0,0,172,173,0,0,0,38,84,186,85,86,0,0,41,81,0,187,0,341,0,
0,0,0,0,188,0,0,189,176,177,178,179,190,0,0,191,0,0,192,193,0,0,194,195,0,0,181,
182,0,183,0,172,173,0,0,0,0,0,0,184,185,0,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,176,
177,178,179,38,84,186,85,86,0,0,41,0,0,187,0,704,0,181,182,0,183,188,0,0,189,0,0,
0,0,190,184,185,191,0,0,192,193,0,0,194,195,0,0,0,0,0,0,0,172,173,0,0,0,38,84,
186,85,86,0,0,41,81,0,187,0,0,0,0,0,0,0,336,0,0,189,176,177,178,179,190,0,0,191,
0,0,192,193,0,0,194,195,0,0,181,182,0,183,0,172,173,0,0,0,0,0,0,184,185,0,0,0,81,
0,0,0,0,0,0,0,0,0,0,0,0,0,176,177,178,179,38,84,186,85,86,0,0,41,0,0,187,0,0,0,
181,182,0,183,352,0,0,189,0,0,0,0,190,184,185,191,0,0,192,193,0,0,194,195,0,0,0,
0,0,0,0,172,173,0,0,0,38,84,186,85,86,0,0,41,81,0,187,0,0,0,0,0,0,0,188,0,0,189,
176,177,178,179,190,0,0,191,0,0,192,193,830,831,194,195,0,0,181,182,0,183,0,0,0,
81,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,
0,41,0,0,187,0,0,0,0,0,0,0,899,0,0,189,0,0,82,0,190,0,0,191,0,0,192,193,0,0,194,
195,0,167,84,0,85,86,0,0,41,0,0,0,0,87,0,0,0,0,0,832,0,89,90,0,0,0,0,91,0,0,833,
0,0,92,193,0,0,194,195};static short Cyc_yycheck[6245]={0,327,174,65,175,0,175,285,
286,114,288,144,364,144,367,144,366,157,400,68,247,21,369,432,128,259,21,328,370,
371,100,101,188,591,104,108,166,5,6,508,159,106,17,21,44,107,452,106,113,748,227,
51,376,377,122,0,51,71,58,59,60,448,62,58,59,60,427,62,68,69,92,397,68,68,92,401,
21,355,111,128,58,59,60,23,62,225,119,448,475,138,68,140,378,379,23,372,373,374,
375,65,506,341,102,103,513,122,51,102,103,515,809,493,134,58,59,60,134,62,521,
119,120,121,122,68,119,120,121,122,128,61,599,106,128,128,190,191,192,38,138,122,
140,107,82,138,140,140,704,147,120,321,128,119,92,82,229,211,328,102,103,131,138,
161,140,92,304,232,161,118,235,470,104,44,247,124,119,120,121,122,307,111,336,
125,338,128,57,247,118,111,86,319,125,135,106,138,350,140,352,87,112,133,135,117,
126,265,607,106,107,108,109,110,51,260,113,87,88,229,161,86,445,106,125,86,120,
124,135,123,127,227,139,169,135,171,227,247,301,122,285,286,111,288,169,311,469,
420,421,422,136,137,122,123,317,265,122,135,123,120,106,139,123,129,260,102,103,
272,260,260,135,649,141,278,135,117,122,122,766,49,216,0,119,120,121,122,260,223,
120,285,286,216,288,125,285,286,432,288,223,87,88,111,470,22,23,111,25,124,106,
105,355,285,286,119,288,113,126,547,314,260,82,83,84,722,161,44,124,726,380,381,
382,124,51,120,122,123,222,130,57,106,828,128,639,742,285,286,65,288,67,415,117,
70,238,117,105,591,120,120,125,355,79,128,124,82,355,128,688,129,130,435,106,107,
508,92,120,777,112,513,700,701,106,355,128,102,103,20,21,445,107,258,448,117,111,
329,120,496,122,124,621,499,119,120,121,122,329,88,117,124,807,118,898,469,635,
124,355,425,904,475,287,59,106,820,141,106,420,421,422,113,120,420,421,422,106,
429,122,445,128,503,124,113,485,127,161,120,118,117,118,166,125,445,169,447,171,
120,499,691,118,115,125,469,456,691,458,599,122,456,124,458,704,115,958,639,130,
469,410,590,122,413,124,132,117,118,611,121,130,410,611,140,141,485,495,129,130,
132,485,610,625,216,625,108,625,140,141,499,223,188,558,499,499,624,118,485,106,
107,106,456,1004,458,112,405,112,113,114,89,90,499,118,516,247,1017,697,527,697,
524,670,940,527,120,221,258,708,666,125,124,485,120,265,106,267,674,125,270,615,
112,113,114,786,905,499,908,909,907,115,621,800,114,106,910,287,172,800,174,175,
911,912,117,117,635,852,120,638,184,185,120,133,121,527,190,191,192,476,477,145,
124,277,144,106,917,918,111,319,667,120,113,540,115,117,125,211,120,329,621,120,
609,124,120,605,125,663,456,125,458,132,133,134,635,120,106,638,125,904,125,628,
112,113,114,120,919,920,120,913,914,915,916,125,124,117,115,711,885,124,640,124,
336,122,885,124,593,120,56,723,344,130,125,120,348,349,663,593,352,117,663,663,
120,119,738,120,226,1000,228,832,125,365,117,85,86,120,683,679,663,527,410,683,
121,413,124,93,94,95,96,97,98,99,100,101,102,259,124,63,64,82,83,84,130,263,132,
133,134,321,402,124,779,663,824,121,328,445,106,880,448,1004,617,124,112,113,114,
725,456,120,458,119,730,683,125,5,6,871,899,753,901,469,740,124,124,759,120,475,
129,130,312,125,364,124,366,367,368,369,370,371,372,373,374,375,376,377,378,379,
380,381,382,120,833,771,124,120,125,120,776,341,125,120,125,345,346,347,125,479,
480,120,851,483,120,485,125,940,358,125,852,527,362,106,494,38,124,120,124,112,
113,114,125,756,540,106,117,119,120,120,120,124,113,383,756,125,117,130,1003,132,
133,134,124,124,124,395,91,996,136,683,400,132,133,134,856,117,106,859,120,852,
117,137,904,120,852,138,117,106,91,120,470,118,921,922,923,117,426,593,120,425,
103,852,117,106,107,120,109,110,124,944,113,132,133,134,896,118,17,18,19,445,125,
124,123,126,127,621,452,119,957,132,119,904,22,23,852,138,904,106,119,635,119,
120,638,112,113,114,121,929,930,645,111,933,934,904,936,124,106,107,108,119,120,
130,119,493,137,138,496,420,421,422,496,89,90,85,86,941,119,120,558,118,506,118,
1004,751,752,106,904,683,124,515,124,124,106,653,654,106,124,117,112,113,114,983,
113,128,986,124,988,989,117,119,124,135,106,124,128,121,131,547,132,133,134,132,
133,134,128,125,125,1009,115,92,119,1004,123,119,694,123,1004,119,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,120,1004,125,122,125,106,120,639,756,591,63,64,65,125,
120,37,119,729,118,124,106,74,75,76,77,78,125,49,120,117,612,779,744,611,1004,57,
58,117,123,120,124,128,118,621,626,30,68,625,760,761,72,763,117,765,42,124,135,
79,80,125,131,125,125,120,106,697,120,649,120,120,125,783,119,111,120,125,708,
125,119,106,125,103,125,119,106,125,121,131,125,111,112,113,114,115,125,117,124,
118,124,117,125,117,124,123,126,127,119,118,130,691,132,133,134,118,696,122,25,
125,118,125,832,125,704,125,125,707,125,125,131,125,117,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,131,858,106,124,861,862,106,125,112,113,114,115,112,113,114,37,
128,119,117,117,124,111,125,124,124,748,107,49,132,133,134,125,132,133,134,57,58,
120,125,125,125,125,899,119,28,118,68,124,128,906,72,944,91,830,777,91,833,79,80,
786,119,136,120,119,25,120,119,924,119,117,107,0,124,800,851,125,125,119,125,120,
125,938,125,0,106,125,809,125,945,111,112,113,114,115,125,125,620,944,113,35,638,
824,124,258,126,127,263,70,48,679,132,133,134,674,158,940,670,330,223,540,413,
368,756,351,- 1,- 1,- 1,- 1,905,- 1,907,908,909,910,911,912,913,914,915,916,917,918,
919,920,921,922,923,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,885,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,957,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,
45,46,- 1,1003,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,
71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,
- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,
32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,
- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,
111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,-
1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,
- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,
- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,-
1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,
35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,
62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,
113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,
- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,
- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,
- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,
126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,
- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,
- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,- 1,118,
- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,
143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,
112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,
135,- 1,- 1,138,139,- 1,- 1,142,143,0,1,38,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,
- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,91,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,106,107,
72,109,110,- 1,- 1,113,- 1,79,80,81,118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,
132,- 1,- 1,- 1,- 1,- 1,138,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,112,113,114,115,- 1,- 1,- 1,119,
- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,0,1,38,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,
- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,91,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,
67,68,69,106,107,72,109,110,- 1,- 1,113,- 1,79,80,81,118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,
127,- 1,- 1,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,138,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,112,113,114,
115,- 1,- 1,- 1,119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,0,1,- 1,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,
- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,112,
113,114,115,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,
59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,38,- 1,79,80,81,- 1,- 1,- 1,- 1,
- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,
112,113,114,115,37,- 1,- 1,119,- 1,- 1,- 1,- 1,124,- 1,126,127,49,- 1,91,- 1,132,133,134,
- 1,57,58,- 1,- 1,- 1,- 1,103,- 1,- 1,106,107,68,109,110,- 1,72,113,- 1,- 1,- 1,- 1,118,79,
80,- 1,- 1,- 1,124,- 1,126,127,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,103,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,115,- 1,117,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,
- 1,- 1,130,- 1,132,133,134,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,49,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,
- 1,79,80,81,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,
- 1,- 1,- 1,- 1,111,112,- 1,114,115,- 1,37,118,39,- 1,121,- 1,- 1,- 1,45,126,127,- 1,49,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,
- 1,79,80,81,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,111,112,- 1,114,115,37,- 1,118,- 1,- 1,- 1,- 1,- 1,45,- 1,126,127,49,- 1,- 1,-
1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,
79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,37,111,112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,126,127,- 1,- 1,57,
58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,
111,112,- 1,114,115,- 1,117,- 1,- 1,- 1,38,49,- 1,- 1,- 1,126,127,- 1,- 1,57,58,- 1,- 1,61,-
1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,111,112,103,114,
115,106,107,118,109,110,- 1,49,113,- 1,- 1,126,127,118,- 1,57,58,- 1,- 1,124,- 1,126,
127,- 1,129,130,68,132,- 1,- 1,72,- 1,- 1,138,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,111,112,- 1,114,
115,- 1,117,- 1,- 1,- 1,38,49,- 1,- 1,- 1,126,127,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,- 1,- 1,- 1,- 1,91,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,111,112,103,114,115,106,107,- 1,
109,110,- 1,49,113,- 1,- 1,126,127,118,- 1,57,58,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,68,
132,- 1,- 1,72,- 1,- 1,138,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,111,112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,-
1,49,- 1,- 1,- 1,126,127,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,-
1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,
33,34,35,36,- 1,38,- 1,40,41,- 1,111,44,- 1,46,115,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,126,
127,60,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,
110,- 1,- 1,113,- 1,- 1,116,117,118,- 1,- 1,25,26,- 1,124,- 1,- 1,127,32,- 1,- 1,- 1,132,- 1,
38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,
- 1,116,- 1,118,119,- 1,25,26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,
138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,
118,119,- 1,25,26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,138,139,44,-
1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,
71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,118,119,- 1,25,
26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,-
1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,118,- 1,- 1,25,26,- 1,124,- 1,- 1,
127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,
108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,- 1,- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,131,
132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,
113,- 1,- 1,116,- 1,118,- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,- 1,132,- 1,38,135,40,-
1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,117,
25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,
142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,25,26,- 1,- 1,- 1,- 1,124,125,
- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,-
1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,
84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,
109,110,- 1,- 1,113,- 1,- 1,116,- 1,- 1,- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,131,132,
- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,
- 1,- 1,116,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,
138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,- 1,- 1,- 1,
25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,
- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,- 1,- 1,- 1,25,26,- 1,124,- 1,- 1,
127,- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,
108,109,110,- 1,- 1,113,- 1,- 1,116,117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,
132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,
116,117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,
- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,
73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,25,26,- 1,- 1,- 1,- 1,
124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,
108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,
132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,
116,117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,
- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,
73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,25,26,- 1,- 1,- 1,- 1,
124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,
108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,
132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,
116,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,
- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,
73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,25,26,- 1,- 1,- 1,- 1,
124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,25,26,- 1,- 1,- 1,- 1,- 1,
- 1,83,84,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,106,107,
108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,- 1,- 1,70,71,- 1,73,124,- 1,- 1,127,- 1,- 1,- 1,- 1,
132,83,84,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,
106,107,108,109,110,- 1,- 1,113,38,- 1,116,- 1,118,- 1,- 1,- 1,- 1,- 1,124,- 1,- 1,127,52,
53,54,55,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,70,71,- 1,73,- 1,25,26,- 1,
- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,
55,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,118,- 1,70,71,- 1,73,124,- 1,- 1,127,-
1,- 1,- 1,- 1,132,83,84,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,
- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,38,- 1,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,- 1,
127,52,53,54,55,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,70,71,- 1,73,- 1,
25,26,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
52,53,54,55,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,- 1,- 1,70,71,- 1,73,124,- 1,
- 1,127,- 1,- 1,- 1,- 1,132,83,84,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,- 1,- 1,- 1,- 1,-
1,25,26,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,38,- 1,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
124,- 1,- 1,127,52,53,54,55,132,- 1,- 1,135,- 1,- 1,138,139,25,26,142,143,- 1,- 1,70,71,
- 1,73,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,- 1,- 1,- 1,- 1,- 1,- 1,-
1,124,- 1,- 1,127,- 1,- 1,91,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,106,107,
- 1,109,110,- 1,- 1,113,- 1,- 1,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,
132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143};char Cyc_Yystack_overflow[17]="Yystack_overflow";
struct Cyc_Yystack_overflow_struct{char*tag;int f1;};struct Cyc_Yystack_overflow_struct
Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};void Cyc_yyerror(struct
_dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(int)'\000';
union Cyc_YYSTYPE Cyc_yylval={.YYINITIALSVAL={61,0}};static int Cyc_yynerrs=0;struct
_tuple24{struct Cyc_Position_Segment*f1;struct _tuple1*f2;int f3;};struct _tuple25{
struct _dyneither_ptr f1;void*f2;};static char _tmp50D[8]="stdcall";static char
_tmp50E[6]="cdecl";static char _tmp50F[9]="fastcall";static char _tmp510[9]="noreturn";
static char _tmp511[6]="const";static char _tmp512[8]="aligned";static char _tmp513[7]="packed";
static char _tmp514[7]="shared";static char _tmp515[7]="unused";static char _tmp516[5]="weak";
static char _tmp517[10]="dllimport";static char _tmp518[10]="dllexport";static char
_tmp519[23]="no_instrument_function";static char _tmp51A[12]="constructor";static
char _tmp51B[11]="destructor";static char _tmp51C[22]="no_check_memory_usage";
static char _tmp51D[5]="pure";struct _tuple26{void*f1;void*f2;};struct _tuple27{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*
yylex_buf);static void _tmp9FB(unsigned int*_tmp9FA,unsigned int*_tmp9F9,union Cyc_YYSTYPE**
_tmp9F8){for(*_tmp9FA=0;*_tmp9FA < *_tmp9F9;(*_tmp9FA)++){(*_tmp9F8)[*_tmp9FA]=
Cyc_yylval;}}static void _tmpA00(unsigned int*_tmp9FF,unsigned int*_tmp9FE,struct
Cyc_Yyltype**_tmp9FD){for(*_tmp9FF=0;*_tmp9FF < *_tmp9FE;(*_tmp9FF)++){(*_tmp9FD)[*
_tmp9FF]=Cyc_yynewloc();}}static void _tmpEF0(unsigned int*_tmpEEF,unsigned int*
_tmpEEE,char**_tmpEEC){for(*_tmpEEF=0;*_tmpEEF < *_tmpEEE;(*_tmpEEF)++){(*_tmpEEC)[*
_tmpEEF]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct
_RegionHandle _tmp471=_new_region("yyregion");struct _RegionHandle*yyregion=&
_tmp471;_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;
int yyssp_offset;short*yyss=(short*)_region_malloc(yyregion,sizeof(short)* 10000);
int yyvsp_offset;unsigned int _tmp9FA;unsigned int _tmp9F9;union Cyc_YYSTYPE*_tmp9F8;
unsigned int _tmp9F7;union Cyc_YYSTYPE*yyvs=(union Cyc_YYSTYPE*)((_tmp9F7=10000,((
_tmp9F8=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),
_tmp9F7)),((((_tmp9F9=_tmp9F7,_tmp9FB(& _tmp9FA,& _tmp9F9,& _tmp9F8))),_tmp9F8))))));
int yylsp_offset;unsigned int _tmp9FF;unsigned int _tmp9FE;struct Cyc_Yyltype*
_tmp9FD;unsigned int _tmp9FC;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)((_tmp9FC=
10000,((_tmp9FD=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(
struct Cyc_Yyltype),_tmp9FC)),((((_tmp9FE=_tmp9FC,_tmpA00(& _tmp9FF,& _tmp9FE,&
_tmp9FD))),_tmp9FD))))));int yystacksize=10000;union Cyc_YYSTYPE yyval=Cyc_yylval;
int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;
yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(
10000,++ yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(
yystacksize >= 10000){{const char*_tmpA01;Cyc_yyerror(((_tmpA01="parser stack overflow",
_tag_dyneither(_tmpA01,sizeof(char),22))));}(int)_throw((void*)& Cyc_Yystack_overflow_val);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1028,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 371?(int)Cyc_yytranslate[_check_known_subscript_notnull(372,Cyc_yychar)]:
287;}yyn +=yychar1;if((yyn < 0  || yyn > 6244) || Cyc_yycheck[
_check_known_subscript_notnull(6245,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6245,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1027){int
_tmp473=0;_npop_handler(0);return _tmp473;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1028,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(513,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL215: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL216: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp474;_push_handler(& _tmp474);{int _tmp476=0;if(setjmp(_tmp474.handler))_tmp476=
1;if(!_tmp476){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp475=(void*)_exn_thrown;void*
_tmp478=_tmp475;_LL219: {struct Cyc_Core_Failure_struct*_tmp479=(struct Cyc_Core_Failure_struct*)
_tmp478;if(_tmp479->tag != Cyc_Core_Failure)goto _LL21B;}_LL21A: x=0;goto _LL218;
_LL21B:;_LL21C:(void)_throw(_tmp478);_LL218:;}};}{struct _handler_cons _tmp47A;
_push_handler(& _tmp47A);{int _tmp47C=0;if(setjmp(_tmp47A.handler))_tmp47C=1;if(!
_tmp47C){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;
_pop_handler();}else{void*_tmp47B=(void*)_exn_thrown;void*_tmp47E=_tmp47B;_LL21E: {
struct Cyc_Core_Failure_struct*_tmp47F=(struct Cyc_Core_Failure_struct*)_tmp47E;
if(_tmp47F->tag != Cyc_Core_Failure)goto _LL220;}_LL21F: y=0;goto _LL21D;_LL220:;
_LL221:(void)_throw(_tmp47E);_LL21D:;}};}yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));break;}
case 3: _LL217:{struct Cyc_Absyn_Decl*_tmpA0B;struct Cyc_Absyn_Using_d_struct _tmpA0A;
struct Cyc_Absyn_Using_d_struct*_tmpA09;struct Cyc_List_List*_tmpA08;yyval=Cyc_YY18(((
_tmpA08=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA08)),((_tmpA08->hd=((
_tmpA0B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA0B)),((_tmpA0B->r=(
void*)((_tmpA09=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA09)),((
_tmpA09[0]=((_tmpA0A.tag=11,((_tmpA0A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA0A.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA0A)))))),_tmpA09)))),((
_tmpA0B->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA0B)))))),((_tmpA08->tl=0,_tmpA08)))))));}Cyc_Lex_leave_using();
break;case 4: _LL222:{struct Cyc_Absyn_Decl*_tmpA15;struct Cyc_Absyn_Using_d_struct
_tmpA14;struct Cyc_Absyn_Using_d_struct*_tmpA13;struct Cyc_List_List*_tmpA12;yyval=
Cyc_YY18(((_tmpA12=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA12)),((
_tmpA12->hd=((_tmpA15=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA15)),((
_tmpA15->r=(void*)((_tmpA13=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA13)),((_tmpA13[0]=((_tmpA14.tag=11,((_tmpA14.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpA14.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA14)))))),
_tmpA13)))),((_tmpA15->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA15)))))),((
_tmpA12->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA12)))))));}break;case 5: _LL223:{struct Cyc_Absyn_Decl*_tmpA24;
struct Cyc_Absyn_Namespace_d_struct _tmpA23;struct _dyneither_ptr*_tmpA22;struct Cyc_Absyn_Namespace_d_struct*
_tmpA21;struct Cyc_List_List*_tmpA20;yyval=Cyc_YY18(((_tmpA20=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA20)),((_tmpA20->hd=((_tmpA24=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA24)),((_tmpA24->r=(void*)((_tmpA21=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA21)),((_tmpA21[0]=((_tmpA23.tag=10,((_tmpA23.f1=((_tmpA22=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA22)),((_tmpA22[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA22)))),((
_tmpA23.f2=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpA23)))))),_tmpA21)))),((_tmpA24->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA24)))))),((
_tmpA20->tl=0,_tmpA20)))))));}Cyc_Lex_leave_namespace();break;case 6: _LL224: {
struct _dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp48D;_push_handler(& _tmp48D);{int _tmp48F=0;if(setjmp(_tmp48D.handler))
_tmp48F=1;if(!_tmp48F){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp48E=(void*)_exn_thrown;void*_tmp491=_tmp48E;_LL227: {struct Cyc_Core_Failure_struct*
_tmp492=(struct Cyc_Core_Failure_struct*)_tmp491;if(_tmp492->tag != Cyc_Core_Failure)
goto _LL229;}_LL228:{const char*_tmpA25;nspace=((_tmpA25="",_tag_dyneither(_tmpA25,
sizeof(char),1)));}goto _LL226;_LL229:;_LL22A:(void)_throw(_tmp491);_LL226:;}};}{
struct _handler_cons _tmp494;_push_handler(& _tmp494);{int _tmp496=0;if(setjmp(
_tmp494.handler))_tmp496=1;if(!_tmp496){x=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp495=(void*)_exn_thrown;void*_tmp498=_tmp495;_LL22C: {struct Cyc_Core_Failure_struct*
_tmp499=(struct Cyc_Core_Failure_struct*)_tmp498;if(_tmp499->tag != Cyc_Core_Failure)
goto _LL22E;}_LL22D: x=0;goto _LL22B;_LL22E:;_LL22F:(void)_throw(_tmp498);_LL22B:;}};}{
struct _handler_cons _tmp49A;_push_handler(& _tmp49A);{int _tmp49C=0;if(setjmp(
_tmp49A.handler))_tmp49C=1;if(!_tmp49C){y=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{void*
_tmp49B=(void*)_exn_thrown;void*_tmp49E=_tmp49B;_LL231: {struct Cyc_Core_Failure_struct*
_tmp49F=(struct Cyc_Core_Failure_struct*)_tmp49E;if(_tmp49F->tag != Cyc_Core_Failure)
goto _LL233;}_LL232: y=0;goto _LL230;_LL233:;_LL234:(void)_throw(_tmp49E);_LL230:;}};}{
struct Cyc_Absyn_Decl*_tmpA34;struct Cyc_Absyn_Namespace_d_struct _tmpA33;struct
_dyneither_ptr*_tmpA32;struct Cyc_Absyn_Namespace_d_struct*_tmpA31;struct Cyc_List_List*
_tmpA30;yyval=Cyc_YY18(((_tmpA30=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA30)),((_tmpA30->hd=((_tmpA34=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA34)),((_tmpA34->r=(void*)((_tmpA31=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA31)),((_tmpA31[0]=((_tmpA33.tag=10,((_tmpA33.f1=((_tmpA32=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA32)),((_tmpA32[0]=nspace,
_tmpA32)))),((_tmpA33.f2=x,_tmpA33)))))),_tmpA31)))),((_tmpA34->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA34)))))),((
_tmpA30->tl=y,_tmpA30)))))));}break;}case 7: _LL225:{const char*_tmpA35;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),((_tmpA35="C",_tag_dyneither(_tmpA35,sizeof(char),2))))
== 0){struct Cyc_Absyn_Decl*_tmpA3F;struct Cyc_Absyn_ExternC_d_struct _tmpA3E;
struct Cyc_Absyn_ExternC_d_struct*_tmpA3D;struct Cyc_List_List*_tmpA3C;yyval=Cyc_YY18(((
_tmpA3C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA3C)),((_tmpA3C->hd=((
_tmpA3F=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA3F)),((_tmpA3F->r=(
void*)((_tmpA3D=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA3D)),((
_tmpA3D[0]=((_tmpA3E.tag=12,((_tmpA3E.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA3E)))),_tmpA3D)))),((
_tmpA3F->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),_tmpA3F)))))),((_tmpA3C->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA3C)))))));}else{{const
char*_tmpA40;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpA40="C include",
_tag_dyneither(_tmpA40,sizeof(char),10))))!= 0){const char*_tmpA41;Cyc_Parse_err(((
_tmpA41="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA41,sizeof(char),
29))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));}}{struct Cyc_Absyn_Decl*_tmpA4B;struct Cyc_Absyn_ExternCinclude_d_struct
_tmpA4A;struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA49;struct Cyc_List_List*
_tmpA48;yyval=Cyc_YY18(((_tmpA48=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA48)),((_tmpA48->hd=((_tmpA4B=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA4B)),((_tmpA4B->r=(void*)((_tmpA49=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA49)),((_tmpA49[0]=((_tmpA4A.tag=13,((_tmpA4A.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA4A.f2=0,_tmpA4A)))))),
_tmpA49)))),((_tmpA4B->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA4B)))))),((
_tmpA48->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA48)))))));};}}break;case 8: _LL235:{const char*_tmpA4C;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]),((_tmpA4C="C include",_tag_dyneither(_tmpA4C,sizeof(
char),10))))!= 0){const char*_tmpA4D;Cyc_Parse_err(((_tmpA4D="expecting \"C include\"",
_tag_dyneither(_tmpA4D,sizeof(char),22))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_Absyn_Decl*_tmpA57;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA56;
struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA55;struct Cyc_List_List*_tmpA54;
yyval=Cyc_YY18(((_tmpA54=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA54)),((
_tmpA54->hd=((_tmpA57=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA57)),((
_tmpA57->r=(void*)((_tmpA55=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA55)),((_tmpA55[0]=((_tmpA56.tag=13,((_tmpA56.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA56.f2=exs,_tmpA56)))))),
_tmpA55)))),((_tmpA57->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA57)))))),((
_tmpA54->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA54)))))));}break;};case 9: _LL236:{struct Cyc_Absyn_Decl*
_tmpA5A;struct Cyc_List_List*_tmpA59;yyval=Cyc_YY18(((_tmpA59=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA59)),((_tmpA59->hd=((_tmpA5A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA5A)),((_tmpA5A->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA5A->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA5A)))))),((
_tmpA59->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA59)))))));}break;case 10: _LL237:{struct Cyc_Absyn_Decl*
_tmpA5D;struct Cyc_List_List*_tmpA5C;yyval=Cyc_YY18(((_tmpA5C=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA5C)),((_tmpA5C->hd=((_tmpA5D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA5D)),((_tmpA5D->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA5D->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),_tmpA5D)))))),((_tmpA5C->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA5C)))))));}break;case
11: _LL238: yyval=Cyc_YY18(0);break;case 12: _LL239: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 13: _LL23A: yyval=
Cyc_YY52(0);break;case 14: _LL23B:{struct _tuple24*_tmpA60;struct Cyc_List_List*
_tmpA5F;yyval=Cyc_YY52(((_tmpA5F=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA5F)),((_tmpA5F->hd=((_tmpA60=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA60)),((_tmpA60->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpA60->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpA60->f3=0,_tmpA60)))))))),((_tmpA5F->tl=0,_tmpA5F)))))));}break;case 15:
_LL23C:{struct _tuple24*_tmpA63;struct Cyc_List_List*_tmpA62;yyval=Cyc_YY52(((
_tmpA62=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA62)),((_tmpA62->hd=((
_tmpA63=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA63)),((_tmpA63->f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),((_tmpA63->f2=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpA63->f3=0,_tmpA63)))))))),((
_tmpA62->tl=0,_tmpA62)))))));}break;case 16: _LL23D:{struct _tuple24*_tmpA66;struct
Cyc_List_List*_tmpA65;yyval=Cyc_YY52(((_tmpA65=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA65)),((_tmpA65->hd=((_tmpA66=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA66)),((_tmpA66->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpA66->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpA66->f3=0,_tmpA66)))))))),((_tmpA65->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA65)))))));}break;case
17: _LL23E:{struct Cyc_Absyn_Fn_d_struct*_tmpA6C;struct Cyc_Absyn_Fn_d_struct
_tmpA6B;struct Cyc_List_List*_tmpA6A;yyval=Cyc_YY18(((_tmpA6A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA6A)),((_tmpA6A->hd=Cyc_Absyn_new_decl((void*)((_tmpA6C=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA6C)),((_tmpA6C[0]=((_tmpA6B.tag=
1,((_tmpA6B.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA6B)))),_tmpA6C)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA6A->tl=0,
_tmpA6A)))))));}break;case 18: _LL23F: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 19: _LL240: yyval=Cyc_YY18(0);break;case 20: _LL241:
yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 21:
_LL242:{struct Cyc_Core_Opt*_tmpA6D;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA6D=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA6D)),((_tmpA6D->v=Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA6D)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 22:
_LL243: yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 23:
_LL244:{struct Cyc_Core_Opt*_tmpA6E;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA6E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA6E)),((_tmpA6E->v=Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA6E)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 24:
_LL245:{struct Cyc_Core_Opt*_tmpA6F;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA6F=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA6F)),((_tmpA6F->v=Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA6F)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 25:
_LL246:{struct Cyc_Core_Opt*_tmpA70;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA70=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA70)),((_tmpA70->v=Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA70)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 26:
_LL247: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 27: _LL248: Cyc_Lex_leave_using();
break;case 28: _LL249:{struct _dyneither_ptr*_tmpA71;Cyc_Lex_enter_namespace(((
_tmpA71=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA71)),((_tmpA71[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA71)))));}yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 29: _LL24A: Cyc_Lex_leave_namespace();
break;case 30: _LL24B: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
31: _LL24C: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 32:
_LL24D:{struct Cyc_List_List*_tmpA72;yyval=Cyc_YY18(((_tmpA72=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA72)),((_tmpA72->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA72->tl=0,
_tmpA72)))))));}break;case 33: _LL24E: {struct Cyc_List_List*_tmp4C9=0;{struct Cyc_List_List*
_tmp4CA=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
for(0;_tmp4CA != 0;_tmp4CA=_tmp4CA->tl){struct _dyneither_ptr*_tmp4CB=(struct
_dyneither_ptr*)_tmp4CA->hd;struct _tuple1*_tmpA73;struct _tuple1*qv=(_tmpA73=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA73)),((_tmpA73->f1=Cyc_Absyn_Rel_n(
0),((_tmpA73->f2=_tmp4CB,_tmpA73)))));struct Cyc_Absyn_Vardecl*_tmp4CC=Cyc_Absyn_new_vardecl(
qv,Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*_tmpA74;_tmp4C9=((_tmpA74=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA74)),((_tmpA74->hd=_tmp4CC,((
_tmpA74->tl=_tmp4C9,_tmpA74))))));}}_tmp4C9=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp4C9);{struct Cyc_List_List*_tmpA75;yyval=Cyc_YY18(((
_tmpA75=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA75)),((_tmpA75->hd=
Cyc_Absyn_letv_decl(_tmp4C9,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA75->tl=0,
_tmpA75)))))));}break;}case 34: _LL24F:{const char*_tmpA76;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpA76="`H",_tag_dyneither(_tmpA76,sizeof(char),3))))== 0){
const char*_tmpA79;void*_tmpA78;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA78=0,
Cyc_aprintf(((_tmpA79="bad occurrence of heap region",_tag_dyneither(_tmpA79,
sizeof(char),30))),_tag_dyneither(_tmpA78,sizeof(void*),0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{const char*
_tmpA7A;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA7A="`U",
_tag_dyneither(_tmpA7A,sizeof(char),3))))== 0){const char*_tmpA7D;void*_tmpA7C;
Cyc_Parse_err((struct _dyneither_ptr)((_tmpA7C=0,Cyc_aprintf(((_tmpA7D="bad occurrence of unique region",
_tag_dyneither(_tmpA7D,sizeof(char),32))),_tag_dyneither(_tmpA7C,sizeof(void*),0)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));}}{struct _dyneither_ptr*_tmpA80;struct Cyc_Absyn_Tvar*
_tmpA7F;struct Cyc_Absyn_Tvar*tv=(_tmpA7F=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA7F)),((_tmpA7F->name=((_tmpA80=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA80)),((_tmpA80[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA80)))),((_tmpA7F->identity=
- 1,((_tmpA7F->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA7F)))))));
struct Cyc_Absyn_VarType_struct _tmpA83;struct Cyc_Absyn_VarType_struct*_tmpA82;
void*t=(void*)((_tmpA82=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA82)),((
_tmpA82[0]=((_tmpA83.tag=2,((_tmpA83.f1=tv,_tmpA83)))),_tmpA82))));struct Cyc_Absyn_RgnHandleType_struct
_tmpA89;struct Cyc_Absyn_RgnHandleType_struct*_tmpA88;struct _dyneither_ptr*
_tmpA86;struct _tuple1*_tmpA85;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpA85=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA85)),((_tmpA85->f1=
Cyc_Absyn_Loc_n,((_tmpA85->f2=((_tmpA86=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA86)),((_tmpA86[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA86)))),_tmpA85)))))),(
void*)((_tmpA88=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA88)),((
_tmpA88[0]=((_tmpA89.tag=16,((_tmpA89.f1=(void*)t,_tmpA89)))),_tmpA88)))),0);{
struct Cyc_List_List*_tmpA8A;yyval=Cyc_YY18(((_tmpA8A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpA8A)),((_tmpA8A->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA8A->tl=0,
_tmpA8A)))))));}break;};case 35: _LL250:{const char*_tmpA8B;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmpA8B="H",_tag_dyneither(_tmpA8B,sizeof(char),2))))== 0){
const char*_tmpA8C;Cyc_Parse_err(((_tmpA8C="bad occurrence of heap region `H",
_tag_dyneither(_tmpA8C,sizeof(char),33))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));}}{const char*
_tmpA8D;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA8D="U",
_tag_dyneither(_tmpA8D,sizeof(char),2))))== 0){const char*_tmpA8E;Cyc_Parse_err(((
_tmpA8E="bad occurrence of unique region `U",_tag_dyneither(_tmpA8E,sizeof(char),
35))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));}}if(Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){const char*_tmpA8F;
Cyc_Parse_err(((_tmpA8F="open regions cannot be @resetable",_tag_dyneither(
_tmpA8F,sizeof(char),34))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}{struct
_dyneither_ptr*_tmpA9C;const char*_tmpA9B;void*_tmpA9A[1];struct Cyc_String_pa_struct
_tmpA99;struct Cyc_Absyn_Tvar*_tmpA98;struct Cyc_Absyn_Tvar*tv=(_tmpA98=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA98)),((_tmpA98->name=((
_tmpA9C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpA9C)),((_tmpA9C[0]=(
struct _dyneither_ptr)((_tmpA99.tag=0,((_tmpA99.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),((_tmpA9A[0]=& _tmpA99,Cyc_aprintf(((_tmpA9B="`%s",
_tag_dyneither(_tmpA9B,sizeof(char),4))),_tag_dyneither(_tmpA9A,sizeof(void*),1)))))))),
_tmpA9C)))),((_tmpA98->identity=- 1,((_tmpA98->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),
_tmpA98)))))));struct Cyc_Absyn_VarType_struct _tmpA9F;struct Cyc_Absyn_VarType_struct*
_tmpA9E;void*t=(void*)((_tmpA9E=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpA9E)),((_tmpA9E[0]=((_tmpA9F.tag=2,((_tmpA9F.f1=tv,_tmpA9F)))),_tmpA9E))));
struct Cyc_Absyn_RgnHandleType_struct _tmpAA5;struct Cyc_Absyn_RgnHandleType_struct*
_tmpAA4;struct _dyneither_ptr*_tmpAA2;struct _tuple1*_tmpAA1;struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(((_tmpAA1=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpAA1)),((_tmpAA1->f1=Cyc_Absyn_Loc_n,((_tmpAA1->f2=((_tmpAA2=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpAA2)),((_tmpAA2[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAA2)))),_tmpAA1)))))),(
void*)((_tmpAA4=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAA4)),((
_tmpAA4[0]=((_tmpAA5.tag=16,((_tmpAA5.f1=(void*)t,_tmpAA5)))),_tmpAA4)))),0);{
struct Cyc_List_List*_tmpAA6;yyval=Cyc_YY18(((_tmpAA6=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAA6)),((_tmpAA6->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAA6->tl=0,
_tmpAA6)))))));}break;};case 36: _LL251: {struct Cyc_Absyn_Eq_kb_struct*_tmpAAE;
struct Cyc_Absyn_Eq_kb_struct _tmpAAD;struct _dyneither_ptr*_tmpAAC;struct Cyc_Absyn_Tvar*
_tmpAAB;struct Cyc_Absyn_Tvar*tv=(_tmpAAB=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAAB)),((_tmpAAB->name=((_tmpAAC=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAAC)),((_tmpAAC[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpAAC)))),((_tmpAAB->identity=
- 1,((_tmpAAB->kind=(void*)((_tmpAAE=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAAE)),((_tmpAAE[0]=((_tmpAAD.tag=0,((_tmpAAD.f1=& Cyc_Tcutil_rk,
_tmpAAD)))),_tmpAAE)))),_tmpAAB)))))));struct _dyneither_ptr*_tmpAB1;struct
_tuple1*_tmpAB0;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAB0=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAB0)),((_tmpAB0->f1=Cyc_Absyn_Loc_n,((
_tmpAB0->f2=((_tmpAB1=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAB1)),((
_tmpAB1[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),_tmpAB1)))),_tmpAB0)))))),(void*)& Cyc_Absyn_VoidType_val,0);{
struct Cyc_List_List*_tmpAB2;yyval=Cyc_YY18(((_tmpAB2=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAB2)),((_tmpAB2->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAB2->tl=0,
_tmpAB2)))))));}break;}case 37: _LL252: yyval=Cyc_YY31(0);break;case 38: _LL253: yyval=
Cyc_YY31(1);break;case 39: _LL254: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 40: _LL255: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 41: _LL256:{
struct Cyc_Core_Opt*_tmpAB5;struct Cyc_Parse_Declaration_spec*_tmpAB4;yyval=Cyc_YY19(((
_tmpAB4=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAB4)),((_tmpAB4->sc=((
_tmpAB5=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAB5)),((_tmpAB5->v=(
void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpAB5)))),((_tmpAB4->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAB4->type_specs=
0,((_tmpAB4->is_inline=0,((_tmpAB4->attributes=0,_tmpAB4)))))))))))));}break;
case 42: _LL257: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc != 0){const char*_tmpAB8;void*_tmpAB7;(_tmpAB7=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpAB8="Only one storage class is allowed in a declaration",
_tag_dyneither(_tmpAB8,sizeof(char),51))),_tag_dyneither(_tmpAB7,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmpABB;struct Cyc_Parse_Declaration_spec*_tmpABA;yyval=Cyc_YY19(((
_tmpABA=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpABA)),((_tmpABA->sc=((
_tmpABB=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpABB)),((_tmpABB->v=(
void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpABB)))),((_tmpABA->tq=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->tq,((_tmpABA->type_specs=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((_tmpABA->is_inline=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((
_tmpABA->attributes=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->attributes,_tmpABA)))))))))))));}break;case 43: _LL258:{struct
Cyc_List_List*_tmpABE;struct Cyc_Parse_Declaration_spec*_tmpABD;yyval=Cyc_YY19(((
_tmpABD=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpABD)),((_tmpABD->sc=0,((
_tmpABD->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpABD->type_specs=((
_tmpABE=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpABE)),((_tmpABE->hd=
Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpABE->tl=0,_tmpABE)))))),((_tmpABD->is_inline=0,((_tmpABD->attributes=0,
_tmpABD)))))))))))));}break;case 44: _LL259:{struct Cyc_List_List*_tmpAC1;struct Cyc_Parse_Declaration_spec*
_tmpAC0;yyval=Cyc_YY19(((_tmpAC0=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpAC0)),((_tmpAC0->sc=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]))->sc,((_tmpAC0->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((_tmpAC0->type_specs=((
_tmpAC1=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAC1)),((_tmpAC1->hd=
Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpAC1->tl=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,_tmpAC1)))))),((_tmpAC0->is_inline=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpAC0->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,
_tmpAC0)))))))))))));}break;case 45: _LL25A:{struct Cyc_Parse_Declaration_spec*
_tmpAC2;yyval=Cyc_YY19(((_tmpAC2=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpAC2)),((_tmpAC2->sc=0,((_tmpAC2->tq=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAC2->type_specs=0,((
_tmpAC2->is_inline=0,((_tmpAC2->attributes=0,_tmpAC2)))))))))))));}break;case 46:
_LL25B:{struct Cyc_Parse_Declaration_spec*_tmpAC3;yyval=Cyc_YY19(((_tmpAC3=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAC3)),((_tmpAC3->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAC3->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq),((_tmpAC3->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpAC3->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpAC3->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAC3)))))))))))));}
break;case 47: _LL25C:{struct Cyc_Parse_Declaration_spec*_tmpAC4;yyval=Cyc_YY19(((
_tmpAC4=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAC4)),((_tmpAC4->sc=0,((
_tmpAC4->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAC4->type_specs=
0,((_tmpAC4->is_inline=1,((_tmpAC4->attributes=0,_tmpAC4)))))))))))));}break;
case 48: _LL25D:{struct Cyc_Parse_Declaration_spec*_tmpAC5;yyval=Cyc_YY19(((_tmpAC5=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAC5)),((_tmpAC5->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAC5->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAC5->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAC5->is_inline=1,((_tmpAC5->attributes=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAC5)))))))))))));}
break;case 49: _LL25E:{struct Cyc_Parse_Declaration_spec*_tmpAC6;yyval=Cyc_YY19(((
_tmpAC6=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAC6)),((_tmpAC6->sc=0,((
_tmpAC6->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAC6->type_specs=
0,((_tmpAC6->is_inline=0,((_tmpAC6->attributes=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAC6)))))))))))));}
break;case 50: _LL25F:{struct Cyc_Parse_Declaration_spec*_tmpAC7;yyval=Cyc_YY19(((
_tmpAC7=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAC7)),((_tmpAC7->sc=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((
_tmpAC7->tq=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->tq,((_tmpAC7->type_specs=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((_tmpAC7->is_inline=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((
_tmpAC7->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->attributes),_tmpAC7)))))))))))));}break;case 51: _LL260: yyval=
Cyc_YY22(Cyc_Parse_Auto_sc);break;case 52: _LL261: yyval=Cyc_YY22(Cyc_Parse_Register_sc);
break;case 53: _LL262: yyval=Cyc_YY22(Cyc_Parse_Static_sc);break;case 54: _LL263:
yyval=Cyc_YY22(Cyc_Parse_Extern_sc);break;case 55: _LL264:{const char*_tmpAC8;if(
Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAC8="C",
_tag_dyneither(_tmpAC8,sizeof(char),2))))!= 0){const char*_tmpAC9;Cyc_Parse_err(((
_tmpAC9="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAC9,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}}yyval=Cyc_YY22(Cyc_Parse_ExternC_sc);break;case 56:
_LL265: yyval=Cyc_YY22(Cyc_Parse_Typedef_sc);break;case 57: _LL266: yyval=Cyc_YY22(
Cyc_Parse_Abstract_sc);break;case 58: _LL267: yyval=Cyc_YY45(0);break;case 59: _LL268:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 60:
_LL269: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 61: _LL26A:{struct Cyc_List_List*_tmpACA;yyval=Cyc_YY45(((_tmpACA=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpACA)),((_tmpACA->hd=Cyc_yyget_YY46(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpACA->tl=0,
_tmpACA)))))));}break;case 62: _LL26B:{struct Cyc_List_List*_tmpACB;yyval=Cyc_YY45(((
_tmpACB=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpACB)),((_tmpACB->hd=
Cyc_yyget_YY46(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpACB->tl=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpACB)))))));}break;case 63: _LL26C: {static struct Cyc_Absyn_Aligned_att_struct
att_aligned={6,- 1};static struct _tuple25 att_map[17]={{{_tmp50D,_tmp50D,_tmp50D + 8},(
void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp50E,_tmp50E,_tmp50E + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{
_tmp50F,_tmp50F,_tmp50F + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp510,
_tmp510,_tmp510 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp511,_tmp511,
_tmp511 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp512,_tmp512,_tmp512 + 8},(void*)&
att_aligned},{{_tmp513,_tmp513,_tmp513 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{
_tmp514,_tmp514,_tmp514 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp515,_tmp515,
_tmp515 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp516,_tmp516,_tmp516 + 5},(
void*)& Cyc_Absyn_Weak_att_val},{{_tmp517,_tmp517,_tmp517 + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{
_tmp518,_tmp518,_tmp518 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp519,
_tmp519,_tmp519 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp51A,
_tmp51A,_tmp51A + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp51B,_tmp51B,
_tmp51B + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp51C,_tmp51C,_tmp51C + 22},(
void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp51D,_tmp51D,_tmp51D + 5},(
void*)& Cyc_Absyn_Pure_att_val}};struct _dyneither_ptr _tmp50B=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp50B,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp50B,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp50B,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp50B,sizeof(char),(int)(_get_dyneither_size(
_tmp50B,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp50B,sizeof(char),(int)(_get_dyneither_size(_tmp50B,sizeof(char))- 3)))== '_')
_tmp50B=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp50B,2,
_get_dyneither_size(_tmp50B,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp50B,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){{const char*_tmpACC;Cyc_Parse_err(((
_tmpACC="unrecognized attribute",_tag_dyneither(_tmpACC,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=Cyc_YY46((
void*)& Cyc_Absyn_Cdecl_att_val);}break;};}case 64: _LL26D: yyval=Cyc_YY46((void*)&
Cyc_Absyn_Const_att_val);break;case 65: _LL26E: {struct _dyneither_ptr _tmp51F=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp521;struct
_tuple6 _tmp520=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp521=_tmp520.f2;{void*a;{const char*_tmpACE;const char*
_tmpACD;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51F,((_tmpACE="regparm",
_tag_dyneither(_tmpACE,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp51F,((_tmpACD="__regparm__",_tag_dyneither(_tmpACD,sizeof(
char),12))))== 0){if(_tmp521 < 0  || _tmp521 > 3){const char*_tmpACF;Cyc_Parse_err(((
_tmpACF="regparm requires value between 0 and 3",_tag_dyneither(_tmpACF,sizeof(
char),39))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Regparm_att_struct _tmpAD2;
struct Cyc_Absyn_Regparm_att_struct*_tmpAD1;a=(void*)((_tmpAD1=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAD1)),((_tmpAD1[0]=((_tmpAD2.tag=0,((_tmpAD2.f1=_tmp521,_tmpAD2)))),
_tmpAD1))));};}else{const char*_tmpAD4;const char*_tmpAD3;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp51F,((_tmpAD4="aligned",_tag_dyneither(_tmpAD4,sizeof(char),8))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp51F,((_tmpAD3="__aligned__",
_tag_dyneither(_tmpAD3,sizeof(char),12))))== 0){if(_tmp521 < 0){const char*_tmpAD5;
Cyc_Parse_err(((_tmpAD5="aligned requires positive power of two",_tag_dyneither(
_tmpAD5,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp521;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
const char*_tmpAD6;Cyc_Parse_err(((_tmpAD6="aligned requires positive power of two",
_tag_dyneither(_tmpAD6,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmpAD9;struct Cyc_Absyn_Aligned_att_struct*_tmpAD8;a=(void*)((_tmpAD8=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAD8)),((_tmpAD8[0]=((_tmpAD9.tag=
6,((_tmpAD9.f1=_tmp521,_tmpAD9)))),_tmpAD8))));};};}else{const char*_tmpADB;const
char*_tmpADA;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51F,((_tmpADB="initializes",
_tag_dyneither(_tmpADB,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp51F,((_tmpADA="__initializes__",_tag_dyneither(_tmpADA,
sizeof(char),16))))== 0){struct Cyc_Absyn_Initializes_att_struct _tmpADE;struct Cyc_Absyn_Initializes_att_struct*
_tmpADD;a=(void*)((_tmpADD=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpADD)),((
_tmpADD[0]=((_tmpADE.tag=20,((_tmpADE.f1=_tmp521,_tmpADE)))),_tmpADD))));}else{
const char*_tmpAE0;const char*_tmpADF;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51F,((
_tmpAE0="noliveunique",_tag_dyneither(_tmpAE0,sizeof(char),13))))== 0  || Cyc_zstrcmp((
struct _dyneither_ptr)_tmp51F,((_tmpADF="__noliveunique__",_tag_dyneither(_tmpADF,
sizeof(char),17))))== 0){struct Cyc_Absyn_Noliveunique_att_struct _tmpAE3;struct
Cyc_Absyn_Noliveunique_att_struct*_tmpAE2;a=(void*)((_tmpAE2=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpAE2)),((_tmpAE2[0]=((_tmpAE3.tag=21,((_tmpAE3.f1=_tmp521,_tmpAE3)))),
_tmpAE2))));}else{{const char*_tmpAE4;Cyc_Parse_err(((_tmpAE4="unrecognized attribute",
_tag_dyneither(_tmpAE4,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}}}}yyval=Cyc_YY46(a);break;};}case 66: _LL26F: {struct
_dyneither_ptr _tmp536=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp537=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpAE6;const char*_tmpAE5;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp536,((_tmpAE6="section",
_tag_dyneither(_tmpAE6,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp536,((_tmpAE5="__section__",_tag_dyneither(_tmpAE5,sizeof(
char),12))))== 0){struct Cyc_Absyn_Section_att_struct _tmpAE9;struct Cyc_Absyn_Section_att_struct*
_tmpAE8;a=(void*)((_tmpAE8=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAE8)),((
_tmpAE8[0]=((_tmpAE9.tag=8,((_tmpAE9.f1=_tmp537,_tmpAE9)))),_tmpAE8))));}else{{
const char*_tmpAEA;Cyc_Parse_err(((_tmpAEA="unrecognized attribute",
_tag_dyneither(_tmpAEA,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 67: _LL270: {struct
_dyneither_ptr _tmp53D=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp53E=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpAEB;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp53D,((_tmpAEB="__mode__",
_tag_dyneither(_tmpAEB,sizeof(char),9))))== 0){struct Cyc_Absyn_Mode_att_struct
_tmpAEE;struct Cyc_Absyn_Mode_att_struct*_tmpAED;a=(void*)((_tmpAED=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAED)),((_tmpAED[0]=((_tmpAEE.tag=
23,((_tmpAEE.f1=_tmp53E,_tmpAEE)))),_tmpAED))));}else{{const char*_tmpAEF;Cyc_Parse_err(((
_tmpAEF="unrecognized attribute",_tag_dyneither(_tmpAEF,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 68: _LL271: {struct
_dyneither_ptr _tmp543=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp544=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp546;struct _tuple6
_tmp545=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp546=_tmp545.f2;{int _tmp548;struct _tuple6 _tmp547=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp548=_tmp547.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;{const char*_tmpAF1;const char*_tmpAF0;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp543,((_tmpAF1="format",_tag_dyneither(
_tmpAF1,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp543,((
_tmpAF0="__format__",_tag_dyneither(_tmpAF0,sizeof(char),11))))== 0){const char*
_tmpAF2;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp544,((_tmpAF2="printf",
_tag_dyneither(_tmpAF2,sizeof(char),7))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpAF5;struct Cyc_Absyn_Format_att_struct*_tmpAF4;a=(void*)((_tmpAF4=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAF4)),((_tmpAF4[0]=((_tmpAF5.tag=
19,((_tmpAF5.f1=Cyc_Absyn_Printf_ft,((_tmpAF5.f2=_tmp546,((_tmpAF5.f3=_tmp548,
_tmpAF5)))))))),_tmpAF4))));}else{const char*_tmpAF6;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp544,((_tmpAF6="scanf",_tag_dyneither(_tmpAF6,sizeof(char),6))))
== 0){struct Cyc_Absyn_Format_att_struct _tmpAF9;struct Cyc_Absyn_Format_att_struct*
_tmpAF8;a=(void*)((_tmpAF8=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAF8)),((
_tmpAF8[0]=((_tmpAF9.tag=19,((_tmpAF9.f1=Cyc_Absyn_Scanf_ft,((_tmpAF9.f2=_tmp546,((
_tmpAF9.f3=_tmp548,_tmpAF9)))))))),_tmpAF8))));}else{const char*_tmpAFA;Cyc_Parse_err(((
_tmpAFA="unrecognized format type",_tag_dyneither(_tmpAFA,sizeof(char),25))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{
const char*_tmpAFB;Cyc_Parse_err(((_tmpAFB="unrecognized attribute",
_tag_dyneither(_tmpAFB,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY46(
a);break;};};}case 69: _LL272: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 70: _LL273:{struct Cyc_Absyn_TypedefType_struct _tmpAFE;
struct Cyc_Absyn_TypedefType_struct*_tmpAFD;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpAFD=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpAFD)),((
_tmpAFD[0]=((_tmpAFE.tag=18,((_tmpAFE.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpAFE.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAFE.f3=0,((
_tmpAFE.f4=0,_tmpAFE)))))))))),_tmpAFD)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 71:
_LL274: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 72:
_LL275: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 73:
_LL276: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 74:
_LL277: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 75:
_LL278:{struct Cyc_Parse_Short_spec_struct _tmpB01;struct Cyc_Parse_Short_spec_struct*
_tmpB00;yyval=Cyc_YY23((void*)((_tmpB00=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB00)),((_tmpB00[0]=((_tmpB01.tag=2,((_tmpB01.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB01)))),
_tmpB00)))));}break;case 76: _LL279: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 77: _LL27A:{struct Cyc_Parse_Long_spec_struct
_tmpB04;struct Cyc_Parse_Long_spec_struct*_tmpB03;yyval=Cyc_YY23((void*)((_tmpB03=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB03)),((_tmpB03[0]=((_tmpB04.tag=
3,((_tmpB04.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB04)))),_tmpB03)))));}break;case 78: _LL27B: yyval=
Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 79:
_LL27C: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 80:
_LL27D:{struct Cyc_Parse_Signed_spec_struct _tmpB07;struct Cyc_Parse_Signed_spec_struct*
_tmpB06;yyval=Cyc_YY23((void*)((_tmpB06=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB06)),((_tmpB06[0]=((_tmpB07.tag=0,((_tmpB07.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB07)))),
_tmpB06)))));}break;case 81: _LL27E:{struct Cyc_Parse_Unsigned_spec_struct _tmpB0A;
struct Cyc_Parse_Unsigned_spec_struct*_tmpB09;yyval=Cyc_YY23((void*)((_tmpB09=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB09)),((_tmpB09[0]=((_tmpB0A.tag=
1,((_tmpB0A.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB0A)))),_tmpB09)))));}break;case 82: _LL27F: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 83: _LL280:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84:
_LL281: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
85: _LL282: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 86:
_LL283:{struct Cyc_Absyn_TupleType_struct _tmpB0D;struct Cyc_Absyn_TupleType_struct*
_tmpB0C;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB0C=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB0C)),((_tmpB0C[0]=((_tmpB0D.tag=11,((_tmpB0D.f1=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))),_tmpB0D)))),_tmpB0C)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 87: _LL284:{struct Cyc_Absyn_RgnHandleType_struct
_tmpB10;struct Cyc_Absyn_RgnHandleType_struct*_tmpB0F;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB0F=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB0F)),((
_tmpB0F[0]=((_tmpB10.tag=16,((_tmpB10.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB10)))),_tmpB0F)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 88: _LL285:{struct Cyc_Absyn_RgnHandleType_struct
_tmpB13;struct Cyc_Absyn_RgnHandleType_struct*_tmpB12;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB12=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB12)),((
_tmpB12[0]=((_tmpB13.tag=16,((_tmpB13.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rko,0),_tmpB13)))),_tmpB12)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 89:
_LL286: {void*_tmp563=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0);{
struct Cyc_Absyn_DynRgnType_struct _tmpB16;struct Cyc_Absyn_DynRgnType_struct*
_tmpB15;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB15=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB15)),((_tmpB15[0]=((_tmpB16.tag=17,((_tmpB16.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB16.f2=(void*)
_tmp563,_tmpB16)))))),_tmpB15)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case 90:
_LL287:{struct Cyc_Absyn_DynRgnType_struct _tmpB19;struct Cyc_Absyn_DynRgnType_struct*
_tmpB18;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB18=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB18)),((_tmpB18[0]=((_tmpB19.tag=17,((_tmpB19.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB19.f2=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpB19)))))),_tmpB18)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 91:
_LL288:{struct Cyc_Absyn_TagType_struct _tmpB1C;struct Cyc_Absyn_TagType_struct*
_tmpB1B;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB1B=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB1B)),((_tmpB1B[0]=((_tmpB1C.tag=20,((_tmpB1C.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB1C)))),_tmpB1B)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 92: _LL289:{struct Cyc_Absyn_TagType_struct
_tmpB1F;struct Cyc_Absyn_TagType_struct*_tmpB1E;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB1E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB1E)),((
_tmpB1E[0]=((_tmpB1F.tag=20,((_tmpB1F.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_iko,0),_tmpB1F)))),_tmpB1E)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 93:
_LL28A:{struct Cyc_Absyn_ValueofType_struct _tmpB22;struct Cyc_Absyn_ValueofType_struct*
_tmpB21;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB21=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB21)),((_tmpB21[0]=((_tmpB22.tag=19,((_tmpB22.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB22)))),_tmpB21)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 94: _LL28B: yyval=Cyc_YY43(Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 95: _LL28C: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;{struct Cyc_Absyn_Tqual
_tmpB23;yyval=Cyc_YY25(((_tmpB23.print_const=1,((_tmpB23.q_volatile=0,((_tmpB23.q_restrict=
0,((_tmpB23.real_const=1,((_tmpB23.loc=loc,_tmpB23)))))))))));}break;}case 96:
_LL28D:{struct Cyc_Absyn_Tqual _tmpB24;yyval=Cyc_YY25(((_tmpB24.print_const=0,((
_tmpB24.q_volatile=1,((_tmpB24.q_restrict=0,((_tmpB24.real_const=0,((_tmpB24.loc=
0,_tmpB24)))))))))));}break;case 97: _LL28E:{struct Cyc_Absyn_Tqual _tmpB25;yyval=
Cyc_YY25(((_tmpB25.print_const=0,((_tmpB25.q_volatile=0,((_tmpB25.q_restrict=1,((
_tmpB25.real_const=0,((_tmpB25.loc=0,_tmpB25)))))))))));}break;case 98: _LL28F: {
struct Cyc_Absyn_Enum_td_struct*_tmpB34;struct Cyc_Absyn_Enumdecl*_tmpB33;struct
Cyc_Core_Opt*_tmpB32;struct Cyc_Absyn_Enum_td_struct _tmpB31;struct Cyc_Absyn_TypeDecl*
_tmpB30;struct Cyc_Absyn_TypeDecl*_tmp571=(_tmpB30=_cycalloc(sizeof(*_tmpB30)),((
_tmpB30->r=(void*)((_tmpB34=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpB34)),((_tmpB34[0]=((_tmpB31.tag=1,((_tmpB31.f1=((_tmpB33=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB33)),((_tmpB33->sc=Cyc_Absyn_Public,((_tmpB33->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB33->fields=((
_tmpB32=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB32)),((_tmpB32->v=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB32)))),_tmpB33)))))))),
_tmpB31)))),_tmpB34)))),((_tmpB30->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB30)))));{
struct Cyc_Absyn_TypeDeclType_struct _tmpB37;struct Cyc_Absyn_TypeDeclType_struct*
_tmpB36;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB36=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB36)),((_tmpB36[0]=((_tmpB37.tag=27,((_tmpB37.f1=_tmp571,((_tmpB37.f2=
0,_tmpB37)))))),_tmpB36)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case 99:
_LL290:{struct Cyc_Absyn_EnumType_struct _tmpB3A;struct Cyc_Absyn_EnumType_struct*
_tmpB39;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB39=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB39)),((_tmpB39[0]=((_tmpB3A.tag=14,((_tmpB3A.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB3A.f2=0,_tmpB3A)))))),
_tmpB39)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 100: _LL291:{struct Cyc_Absyn_AnonEnumType_struct
_tmpB3D;struct Cyc_Absyn_AnonEnumType_struct*_tmpB3C;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB3C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB3C)),((
_tmpB3C[0]=((_tmpB3D.tag=15,((_tmpB3D.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB3D)))),_tmpB3C)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 101: _LL292:{struct Cyc_Absyn_Enumfield*
_tmpB3E;yyval=Cyc_YY47(((_tmpB3E=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpB3E)),((_tmpB3E->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB3E->tag=0,((_tmpB3E->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB3E)))))))));}break;case 102: _LL293:{struct Cyc_Absyn_Enumfield*
_tmpB3F;yyval=Cyc_YY47(((_tmpB3F=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpB3F)),((_tmpB3F->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB3F->tag=(struct
Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB3F->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB3F)))))))));}
break;case 103: _LL294:{struct Cyc_List_List*_tmpB40;yyval=Cyc_YY48(((_tmpB40=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB40)),((_tmpB40->hd=Cyc_yyget_YY47(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB40->tl=0,
_tmpB40)))))));}break;case 104: _LL295:{struct Cyc_List_List*_tmpB41;yyval=Cyc_YY48(((
_tmpB41=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB41)),((_tmpB41->hd=
Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpB41->tl=0,_tmpB41)))))));}break;case 105: _LL296:{struct Cyc_List_List*_tmpB42;
yyval=Cyc_YY48(((_tmpB42=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB42)),((
_tmpB42->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmpB42->tl=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB42)))))));}break;case
106: _LL297:{struct Cyc_Absyn_AnonAggrType_struct _tmpB45;struct Cyc_Absyn_AnonAggrType_struct*
_tmpB44;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB44=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB44)),((_tmpB44[0]=((_tmpB45.tag=13,((_tmpB45.f1=Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB45.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB45)))))),
_tmpB44)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 107: _LL298: {struct Cyc_List_List*_tmp584=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp585=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));struct Cyc_Absyn_TypeDecl*
_tmp586=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp584,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp585,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_TypeDeclType_struct
_tmpB48;struct Cyc_Absyn_TypeDeclType_struct*_tmpB47;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB47=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB47)),((
_tmpB47[0]=((_tmpB48.tag=27,((_tmpB48.f1=_tmp586,((_tmpB48.f2=0,_tmpB48)))))),
_tmpB47)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 8)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 108: _LL299: {struct Cyc_List_List*
_tmp589=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp58A=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));struct Cyc_Absyn_TypeDecl*
_tmp58B=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp589,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp58A,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_TypeDeclType_struct
_tmpB4B;struct Cyc_Absyn_TypeDeclType_struct*_tmpB4A;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB4A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB4A)),((
_tmpB4A[0]=((_tmpB4B.tag=27,((_tmpB4B.f1=_tmp58B,((_tmpB4B.f2=0,_tmpB4B)))))),
_tmpB4A)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 7)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 109: _LL29A:{struct Cyc_Absyn_AggrType_struct
_tmpB55;struct Cyc_Core_Opt*_tmpB54;struct Cyc_Absyn_AggrInfo _tmpB53;struct Cyc_Absyn_AggrType_struct*
_tmpB52;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB52=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB52)),((_tmpB52[0]=((_tmpB55.tag=12,((_tmpB55.f1=((_tmpB53.aggr_info=
Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpB54=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpB54)),((_tmpB54->v=(void*)1,_tmpB54))))),((_tmpB53.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB53)))),_tmpB55)))),
_tmpB52)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 110: _LL29B:{struct Cyc_Absyn_AggrType_struct
_tmpB5B;struct Cyc_Absyn_AggrInfo _tmpB5A;struct Cyc_Absyn_AggrType_struct*_tmpB59;
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB59=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB59)),((_tmpB59[0]=((_tmpB5B.tag=12,((_tmpB5B.f1=((_tmpB5A.aggr_info=
Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),0),((_tmpB5A.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB5A)))),_tmpB5B)))),
_tmpB59)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 111: _LL29C: yyval=Cyc_YY40(0);break;case
112: _LL29D: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
break;case 113: _LL29E: yyval=Cyc_YY24(Cyc_Absyn_StructA);break;case 114: _LL29F:
yyval=Cyc_YY24(Cyc_Absyn_UnionA);break;case 115: _LL2A0: yyval=Cyc_YY26(0);break;
case 116: _LL2A1: {struct Cyc_List_List*_tmp595=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));
struct Cyc_List_List*_tmp596=Cyc_Parse_get_argrfield_tags(_tmp595);if(_tmp596 != 0)((
void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,
_tmp596,_tmp595);yyval=Cyc_YY26(_tmp595);break;}case 117: _LL2A2:{struct Cyc_List_List*
_tmpB5C;yyval=Cyc_YY27(((_tmpB5C=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpB5C)),((_tmpB5C->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB5C->tl=0,_tmpB5C)))))));}break;case 118: _LL2A3:{struct Cyc_List_List*
_tmpB5D;yyval=Cyc_YY27(((_tmpB5D=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpB5D)),((_tmpB5D->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB5D->tl=Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpB5D)))))));}break;case 119: _LL2A4: yyval=Cyc_YY21(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 120: _LL2A5:{
struct Cyc_List_List*_tmpB5E;yyval=Cyc_YY21(((_tmpB5E=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB5E)),((_tmpB5E->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB5E->tl=0,_tmpB5E)))))));}
break;case 121: _LL2A6:{struct Cyc_List_List*_tmpB5F;yyval=Cyc_YY21(((_tmpB5F=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB5F)),((_tmpB5F->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB5F->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB5F)))))));}
break;case 122: _LL2A7:{struct _tuple21*_tmpB60;yyval=Cyc_YY20(((_tmpB60=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB60)),((_tmpB60->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB60->f2=0,
_tmpB60)))))));}break;case 123: _LL2A8:{struct _tuple21*_tmpB61;yyval=Cyc_YY20(((
_tmpB61=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB61)),((_tmpB61->f1=
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpB61->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB61)))))));}break;case
124: _LL2A9: {struct _RegionHandle _tmp59D=_new_region("temp");struct _RegionHandle*
temp=& _tmp59D;_push_region(temp);{struct _tuple22 _tmp59F;struct Cyc_Absyn_Tqual
_tmp5A0;struct Cyc_List_List*_tmp5A1;struct Cyc_List_List*_tmp5A2;struct _tuple22*
_tmp59E=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp59F=*_tmp59E;_tmp5A0=_tmp59F.f1;_tmp5A1=_tmp59F.f2;_tmp5A2=_tmp59F.f3;if(
_tmp5A0.loc == 0)_tmp5A0.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);{struct Cyc_List_List*
_tmp5A4;struct Cyc_List_List*_tmp5A5;struct _tuple0 _tmp5A3=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
temp,temp,Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]));_tmp5A4=_tmp5A3.f1;_tmp5A5=_tmp5A3.f2;{void*_tmp5A6=Cyc_Parse_speclist2typ(
_tmp5A1,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp5A7=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(temp,_tmp5A0,_tmp5A6,_tmp5A4,
_tmp5A2),_tmp5A5);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple13*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp5A7));};};}
_npop_handler(0);break;;_pop_region(temp);}case 125: _LL2AA:{struct Cyc_List_List*
_tmpB64;struct _tuple22*_tmpB63;yyval=Cyc_YY35(((_tmpB63=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB63)),((_tmpB63->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB63->f2=((
_tmpB64=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB64)),((_tmpB64->hd=
Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpB64->tl=0,_tmpB64)))))),((_tmpB63->f3=0,_tmpB63)))))))));}break;case 126:
_LL2AB:{struct Cyc_List_List*_tmpB67;struct _tuple22*_tmpB66;yyval=Cyc_YY35(((
_tmpB66=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB66)),((_tmpB66->f1=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((
_tmpB66->f2=((_tmpB67=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB67)),((
_tmpB67->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB67->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpB67)))))),((_tmpB66->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB66)))))))));}break;case 127: _LL2AC:{struct _tuple22*_tmpB68;yyval=Cyc_YY35(((
_tmpB68=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB68)),((_tmpB68->f1=
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpB68->f2=0,((_tmpB68->f3=0,_tmpB68)))))))));}break;case 128: _LL2AD:{struct
_tuple22*_tmpB69;yyval=Cyc_YY35(((_tmpB69=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB69)),((_tmpB69->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpB69->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpB69->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB69)))))))));}break;case 129: _LL2AE:{struct _tuple22*_tmpB6A;yyval=Cyc_YY35(((
_tmpB6A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB6A)),((_tmpB6A->f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB6A->f2=0,((
_tmpB6A->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpB6A)))))))));}break;case 130: _LL2AF:{struct _tuple22*_tmpB6B;
yyval=Cyc_YY35(((_tmpB6B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB6B)),((
_tmpB6B->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1,((_tmpB6B->f2=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpB6B->f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpB6B)))))))));}break;case 131: _LL2B0:{struct Cyc_List_List*_tmpB6E;struct
_tuple22*_tmpB6D;yyval=Cyc_YY35(((_tmpB6D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB6D)),((_tmpB6D->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB6D->f2=((
_tmpB6E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB6E)),((_tmpB6E->hd=
Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpB6E->tl=0,_tmpB6E)))))),((_tmpB6D->f3=0,_tmpB6D)))))))));}break;case 132:
_LL2B1:{struct Cyc_List_List*_tmpB71;struct _tuple22*_tmpB70;yyval=Cyc_YY35(((
_tmpB70=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB70)),((_tmpB70->f1=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((
_tmpB70->f2=((_tmpB71=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB71)),((
_tmpB71->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB71->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpB71)))))),((_tmpB70->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB70)))))))));}break;case 133: _LL2B2:{struct _tuple22*_tmpB72;yyval=Cyc_YY35(((
_tmpB72=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB72)),((_tmpB72->f1=
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpB72->f2=0,((_tmpB72->f3=0,_tmpB72)))))))));}break;case 134: _LL2B3:{struct
_tuple22*_tmpB73;yyval=Cyc_YY35(((_tmpB73=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB73)),((_tmpB73->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpB73->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpB73->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB73)))))))));}break;case 135: _LL2B4:{struct _tuple22*_tmpB74;yyval=Cyc_YY35(((
_tmpB74=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB74)),((_tmpB74->f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB74->f2=0,((
_tmpB74->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpB74)))))))));}break;case 136: _LL2B5:{struct _tuple22*_tmpB75;
yyval=Cyc_YY35(((_tmpB75=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB75)),((
_tmpB75->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1,((_tmpB75->f2=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpB75->f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpB75)))))))));}break;case 137: _LL2B6: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 138: _LL2B7:{
struct Cyc_List_List*_tmpB76;yyval=Cyc_YY21(((_tmpB76=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB76)),((_tmpB76->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB76->tl=0,_tmpB76)))))));}
break;case 139: _LL2B8:{struct Cyc_List_List*_tmpB77;yyval=Cyc_YY21(((_tmpB77=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB77)),((_tmpB77->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB77->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB77)))))));}
break;case 140: _LL2B9:{struct _tuple21*_tmpB78;yyval=Cyc_YY20(((_tmpB78=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB78)),((_tmpB78->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB78->f2=0,
_tmpB78)))))));}break;case 141: _LL2BA:{struct Cyc_Parse_Declarator*_tmpB7E;struct
_tuple1*_tmpB7D;struct _tuple21*_tmpB7C;yyval=Cyc_YY20(((_tmpB7C=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpB7C)),((_tmpB7C->f1=((_tmpB7E=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpB7E)),((_tmpB7E->id=((_tmpB7D=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpB7D)),((_tmpB7D->f1=Cyc_Absyn_Rel_n(0),((
_tmpB7D->f2=_init_dyneither_ptr(_region_malloc(Cyc_Core_unique_region,sizeof(
struct _dyneither_ptr)),"",sizeof(char),1),_tmpB7D)))))),((_tmpB7E->tms=0,_tmpB7E)))))),((
_tmpB7C->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB7C)))))));}break;case
142: _LL2BB:{struct Cyc_Parse_Declarator*_tmpB84;struct _tuple1*_tmpB83;struct
_tuple21*_tmpB82;yyval=Cyc_YY20(((_tmpB82=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB82)),((_tmpB82->f1=((_tmpB84=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB84)),((_tmpB84->id=((_tmpB83=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB83)),((_tmpB83->f1=Cyc_Absyn_Rel_n(0),((_tmpB83->f2=
_init_dyneither_ptr(_region_malloc(Cyc_Core_unique_region,sizeof(struct
_dyneither_ptr)),"",sizeof(char),1),_tmpB83)))))),((_tmpB84->tms=0,_tmpB84)))))),((
_tmpB82->f2=0,_tmpB82)))))));}break;case 143: _LL2BC:{struct _tuple21*_tmpB85;yyval=
Cyc_YY20(((_tmpB85=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB85)),((
_tmpB85->f1=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmpB85->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB85)))))));}break;case
144: _LL2BD: {int _tmp5C4=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_List_List*_tmp5C5=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));struct Cyc_Core_Opt*
_tmpB86;struct Cyc_Absyn_TypeDecl*_tmp5C6=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmp5C5,((_tmpB86=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB86)),((
_tmpB86->v=Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),_tmpB86)))),_tmp5C4,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_TypeDeclType_struct
_tmpB89;struct Cyc_Absyn_TypeDeclType_struct*_tmpB88;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB88=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB88)),((
_tmpB88[0]=((_tmpB89.tag=27,((_tmpB89.f1=_tmp5C6,((_tmpB89.f2=0,_tmpB89)))))),
_tmpB88)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 145: _LL2BE: {int _tmp5CA=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);{struct Cyc_Absyn_DatatypeType_struct
_tmpB93;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB92;struct Cyc_Absyn_DatatypeInfo
_tmpB91;struct Cyc_Absyn_DatatypeType_struct*_tmpB90;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB90=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB90)),((
_tmpB90[0]=((_tmpB93.tag=3,((_tmpB93.f1=((_tmpB91.datatype_info=Cyc_Absyn_UnknownDatatype(((
_tmpB92.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB92.is_extensible=_tmp5CA,_tmpB92))))),((_tmpB91.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB91)))),
_tmpB93)))),_tmpB90)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
146: _LL2BF: {int _tmp5CF=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);{struct Cyc_Absyn_DatatypeFieldType_struct _tmpB9D;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpB9C;struct Cyc_Absyn_DatatypeFieldInfo _tmpB9B;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpB9A;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB9A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpB9A)),((_tmpB9A[0]=((_tmpB9D.tag=4,((_tmpB9D.f1=((_tmpB9B.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB9C.datatype_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB9C.field_name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB9C.is_extensible=
_tmp5CF,_tmpB9C))))))),((_tmpB9B.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB9B)))),_tmpB9D)))),
_tmpB9A)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 147: _LL2C0: yyval=Cyc_YY31(0);break;case
148: _LL2C1: yyval=Cyc_YY31(1);break;case 149: _LL2C2:{struct Cyc_List_List*_tmpB9E;
yyval=Cyc_YY34(((_tmpB9E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpB9E)),((
_tmpB9E->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB9E->tl=0,_tmpB9E)))))));}break;case 150: _LL2C3:{struct Cyc_List_List*
_tmpB9F;yyval=Cyc_YY34(((_tmpB9F=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpB9F)),((_tmpB9F->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB9F->tl=0,_tmpB9F)))))));}break;case 151: _LL2C4:{struct
Cyc_List_List*_tmpBA0;yyval=Cyc_YY34(((_tmpBA0=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBA0)),((_tmpBA0->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBA0->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBA0)))))));}break;
case 152: _LL2C5:{struct Cyc_List_List*_tmpBA1;yyval=Cyc_YY34(((_tmpBA1=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBA1)),((_tmpBA1->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBA1->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBA1)))))));}break;
case 153: _LL2C6: yyval=Cyc_YY32(Cyc_Absyn_Public);break;case 154: _LL2C7: yyval=Cyc_YY32(
Cyc_Absyn_Extern);break;case 155: _LL2C8: yyval=Cyc_YY32(Cyc_Absyn_Static);break;
case 156: _LL2C9:{struct Cyc_Absyn_Datatypefield*_tmpBA2;yyval=Cyc_YY33(((_tmpBA2=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBA2)),((_tmpBA2->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBA2->typs=0,((
_tmpBA2->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpBA2->sc=Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBA2)))))))))));}
break;case 157: _LL2CA: {struct Cyc_List_List*_tmp5D9=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Absyn_Datatypefield*
_tmpBA3;yyval=Cyc_YY33(((_tmpBA3=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpBA3)),((_tmpBA3->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpBA3->typs=_tmp5D9,((
_tmpBA3->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpBA3->sc=Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmpBA3)))))))))));}
break;}case 158: _LL2CB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 159: _LL2CC:{struct Cyc_Parse_Declarator*_tmpBA4;yyval=
Cyc_YY29(((_tmpBA4=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBA4)),((
_tmpBA4->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id,((_tmpBA4->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpBA4)))))));}
break;case 160: _LL2CD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 161: _LL2CE:{struct Cyc_Parse_Declarator*_tmpBA5;yyval=
Cyc_YY29(((_tmpBA5=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBA5)),((
_tmpBA5->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id,((_tmpBA5->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpBA5)))))));}
break;case 162: _LL2CF:{struct Cyc_Parse_Declarator*_tmpBA6;yyval=Cyc_YY29(((
_tmpBA6=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBA6)),((_tmpBA6->id=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpBA6->tms=0,_tmpBA6)))))));}break;case 163: _LL2D0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 164: _LL2D1: {
struct Cyc_Parse_Declarator*_tmp5DE=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpBAC;struct Cyc_Absyn_Attributes_mod_struct _tmpBAB;struct Cyc_List_List*_tmpBAA;
_tmp5DE->tms=((_tmpBAA=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBAA)),((
_tmpBAA->hd=(void*)((_tmpBAC=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpBAC)),((_tmpBAC[0]=((_tmpBAB.tag=5,((_tmpBAB.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpBAB.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpBAB)))))),_tmpBAC)))),((_tmpBAA->tl=_tmp5DE->tms,_tmpBAA))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 165: _LL2D2:{
struct Cyc_List_List*_tmpBB6;struct Cyc_Absyn_Carray_mod_struct _tmpBB5;struct Cyc_Absyn_Carray_mod_struct*
_tmpBB4;struct Cyc_Parse_Declarator*_tmpBB3;yyval=Cyc_YY29(((_tmpBB3=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBB3)),((_tmpBB3->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBB3->tms=((
_tmpBB6=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBB6)),((_tmpBB6->hd=(
void*)((_tmpBB4=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBB4)),((
_tmpBB4[0]=((_tmpBB5.tag=0,((_tmpBB5.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBB5.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBB5)))))),
_tmpBB4)))),((_tmpBB6->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpBB6)))))),_tmpBB3)))))));}break;case 166:
_LL2D3:{struct Cyc_List_List*_tmpBC0;struct Cyc_Absyn_ConstArray_mod_struct _tmpBBF;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpBBE;struct Cyc_Parse_Declarator*_tmpBBD;
yyval=Cyc_YY29(((_tmpBBD=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBBD)),((
_tmpBBD->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id,((_tmpBBD->tms=((_tmpBC0=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBC0)),((_tmpBC0->hd=(void*)((_tmpBBE=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBBE)),((_tmpBBE[0]=((_tmpBBF.tag=1,((_tmpBBF.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBBF.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBBF.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBBF)))))))),
_tmpBBE)))),((_tmpBC0->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpBC0)))))),_tmpBBD)))))));}break;case 167:
_LL2D4: {struct _tuple23 _tmp5EB;struct Cyc_List_List*_tmp5EC;int _tmp5ED;struct Cyc_Absyn_VarargInfo*
_tmp5EE;struct Cyc_Core_Opt*_tmp5EF;struct Cyc_List_List*_tmp5F0;struct _tuple23*
_tmp5EA=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5EB=*_tmp5EA;_tmp5EC=_tmp5EB.f1;_tmp5ED=_tmp5EB.f2;_tmp5EE=_tmp5EB.f3;
_tmp5EF=_tmp5EB.f4;_tmp5F0=_tmp5EB.f5;{struct Cyc_List_List*_tmpBD5;struct Cyc_Absyn_Function_mod_struct
_tmpBD4;struct Cyc_Absyn_WithTypes_struct _tmpBD3;struct Cyc_Absyn_WithTypes_struct*
_tmpBD2;struct Cyc_Absyn_Function_mod_struct*_tmpBD1;struct Cyc_Parse_Declarator*
_tmpBD0;yyval=Cyc_YY29(((_tmpBD0=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpBD0)),((_tmpBD0->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->id,((_tmpBD0->tms=((_tmpBD5=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBD5)),((_tmpBD5->hd=(void*)((_tmpBD1=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBD1)),((_tmpBD1[0]=((_tmpBD4.tag=3,((_tmpBD4.f1=(void*)((void*)((
_tmpBD2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBD2)),((_tmpBD2[0]=((
_tmpBD3.tag=1,((_tmpBD3.f1=_tmp5EC,((_tmpBD3.f2=_tmp5ED,((_tmpBD3.f3=_tmp5EE,((
_tmpBD3.f4=_tmp5EF,((_tmpBD3.f5=_tmp5F0,_tmpBD3)))))))))))),_tmpBD2))))),_tmpBD4)))),
_tmpBD1)))),((_tmpBD5->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpBD5)))))),_tmpBD0)))))));}break;}case 168:
_LL2D5:{struct Cyc_List_List*_tmpBEA;struct Cyc_Absyn_Function_mod_struct _tmpBE9;
struct Cyc_Absyn_WithTypes_struct _tmpBE8;struct Cyc_Absyn_WithTypes_struct*_tmpBE7;
struct Cyc_Absyn_Function_mod_struct*_tmpBE6;struct Cyc_Parse_Declarator*_tmpBE5;
yyval=Cyc_YY29(((_tmpBE5=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBE5)),((
_tmpBE5->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id,((_tmpBE5->tms=((_tmpBEA=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBEA)),((_tmpBEA->hd=(void*)((_tmpBE6=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBE6)),((_tmpBE6[0]=((_tmpBE9.tag=3,((_tmpBE9.f1=(void*)((void*)((
_tmpBE7=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBE7)),((_tmpBE7[0]=((
_tmpBE8.tag=1,((_tmpBE8.f1=0,((_tmpBE8.f2=0,((_tmpBE8.f3=0,((_tmpBE8.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBE8.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBE8)))))))))))),
_tmpBE7))))),_tmpBE9)))),_tmpBE6)))),((_tmpBEA->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpBEA)))))),
_tmpBE5)))))));}break;case 169: _LL2D6:{struct Cyc_List_List*_tmpBFF;struct Cyc_Absyn_Function_mod_struct
_tmpBFE;struct Cyc_Absyn_NoTypes_struct _tmpBFD;struct Cyc_Absyn_NoTypes_struct*
_tmpBFC;struct Cyc_Absyn_Function_mod_struct*_tmpBFB;struct Cyc_Parse_Declarator*
_tmpBFA;yyval=Cyc_YY29(((_tmpBFA=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpBFA)),((_tmpBFA->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->id,((_tmpBFA->tms=((_tmpBFF=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBFF)),((_tmpBFF->hd=(void*)((_tmpBFB=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpBFB)),((_tmpBFB[0]=((_tmpBFE.tag=3,((_tmpBFE.f1=(void*)((void*)((
_tmpBFC=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpBFC)),((_tmpBFC[0]=((
_tmpBFD.tag=0,((_tmpBFD.f1=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpBFD.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBFD)))))),
_tmpBFC))))),_tmpBFE)))),_tmpBFB)))),((_tmpBFF->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpBFF)))))),
_tmpBFA)))))));}break;case 170: _LL2D7: {struct Cyc_List_List*_tmp603=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpC09;struct Cyc_Absyn_TypeParams_mod_struct _tmpC08;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpC07;struct Cyc_Parse_Declarator*_tmpC06;yyval=Cyc_YY29(((_tmpC06=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC06)),((_tmpC06->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC06->tms=((
_tmpC09=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC09)),((_tmpC09->hd=(
void*)((_tmpC07=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC07)),((
_tmpC07[0]=((_tmpC08.tag=4,((_tmpC08.f1=_tmp603,((_tmpC08.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC08.f3=0,
_tmpC08)))))))),_tmpC07)))),((_tmpC09->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpC09)))))),
_tmpC06)))))));}break;}case 171: _LL2D8:{struct Cyc_List_List*_tmpC13;struct Cyc_Absyn_Attributes_mod_struct
_tmpC12;struct Cyc_Absyn_Attributes_mod_struct*_tmpC11;struct Cyc_Parse_Declarator*
_tmpC10;yyval=Cyc_YY29(((_tmpC10=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpC10)),((_tmpC10->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]))->id,((_tmpC10->tms=((_tmpC13=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC13)),((_tmpC13->hd=(void*)((_tmpC11=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC11)),((_tmpC11[0]=((_tmpC12.tag=5,((_tmpC12.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC12.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC12)))))),_tmpC11)))),((
_tmpC13->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpC13)))))),_tmpC10)))))));}break;case 172: _LL2D9:{
struct Cyc_Parse_Declarator*_tmpC14;yyval=Cyc_YY29(((_tmpC14=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC14)),((_tmpC14->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC14->tms=0,_tmpC14)))))));}
break;case 173: _LL2DA:{struct Cyc_Parse_Declarator*_tmpC15;yyval=Cyc_YY29(((
_tmpC15=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC15)),((_tmpC15->id=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpC15->tms=0,_tmpC15)))))));}break;case 174: _LL2DB: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 175: _LL2DC: {
struct Cyc_Parse_Declarator*_tmp60E=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpC1B;struct Cyc_Absyn_Attributes_mod_struct _tmpC1A;struct Cyc_List_List*_tmpC19;
_tmp60E->tms=((_tmpC19=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC19)),((
_tmpC19->hd=(void*)((_tmpC1B=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpC1B)),((_tmpC1B[0]=((_tmpC1A.tag=5,((_tmpC1A.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpC1A.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpC1A)))))),_tmpC1B)))),((_tmpC19->tl=_tmp60E->tms,_tmpC19))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 176: _LL2DD:{
struct Cyc_List_List*_tmpC25;struct Cyc_Absyn_Carray_mod_struct _tmpC24;struct Cyc_Absyn_Carray_mod_struct*
_tmpC23;struct Cyc_Parse_Declarator*_tmpC22;yyval=Cyc_YY29(((_tmpC22=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC22)),((_tmpC22->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC22->tms=((
_tmpC25=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC25)),((_tmpC25->hd=(
void*)((_tmpC23=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC23)),((
_tmpC23[0]=((_tmpC24.tag=0,((_tmpC24.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC24.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC24)))))),
_tmpC23)))),((_tmpC25->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpC25)))))),_tmpC22)))))));}break;case 177:
_LL2DE:{struct Cyc_List_List*_tmpC2F;struct Cyc_Absyn_ConstArray_mod_struct _tmpC2E;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpC2D;struct Cyc_Parse_Declarator*_tmpC2C;
yyval=Cyc_YY29(((_tmpC2C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC2C)),((
_tmpC2C->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id,((_tmpC2C->tms=((_tmpC2F=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC2F)),((_tmpC2F->hd=(void*)((_tmpC2D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC2D)),((_tmpC2D[0]=((_tmpC2E.tag=1,((_tmpC2E.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC2E.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC2E.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC2E)))))))),
_tmpC2D)))),((_tmpC2F->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpC2F)))))),_tmpC2C)))))));}break;case 178:
_LL2DF: {struct _tuple23 _tmp61B;struct Cyc_List_List*_tmp61C;int _tmp61D;struct Cyc_Absyn_VarargInfo*
_tmp61E;struct Cyc_Core_Opt*_tmp61F;struct Cyc_List_List*_tmp620;struct _tuple23*
_tmp61A=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp61B=*_tmp61A;_tmp61C=_tmp61B.f1;_tmp61D=_tmp61B.f2;_tmp61E=_tmp61B.f3;
_tmp61F=_tmp61B.f4;_tmp620=_tmp61B.f5;{struct Cyc_List_List*_tmpC44;struct Cyc_Absyn_Function_mod_struct
_tmpC43;struct Cyc_Absyn_WithTypes_struct _tmpC42;struct Cyc_Absyn_WithTypes_struct*
_tmpC41;struct Cyc_Absyn_Function_mod_struct*_tmpC40;struct Cyc_Parse_Declarator*
_tmpC3F;yyval=Cyc_YY29(((_tmpC3F=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpC3F)),((_tmpC3F->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->id,((_tmpC3F->tms=((_tmpC44=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC44)),((_tmpC44->hd=(void*)((_tmpC40=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC40)),((_tmpC40[0]=((_tmpC43.tag=3,((_tmpC43.f1=(void*)((void*)((
_tmpC41=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC41)),((_tmpC41[0]=((
_tmpC42.tag=1,((_tmpC42.f1=_tmp61C,((_tmpC42.f2=_tmp61D,((_tmpC42.f3=_tmp61E,((
_tmpC42.f4=_tmp61F,((_tmpC42.f5=_tmp620,_tmpC42)))))))))))),_tmpC41))))),_tmpC43)))),
_tmpC40)))),((_tmpC44->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpC44)))))),_tmpC3F)))))));}break;}case 179:
_LL2E0:{struct Cyc_List_List*_tmpC59;struct Cyc_Absyn_Function_mod_struct _tmpC58;
struct Cyc_Absyn_WithTypes_struct _tmpC57;struct Cyc_Absyn_WithTypes_struct*_tmpC56;
struct Cyc_Absyn_Function_mod_struct*_tmpC55;struct Cyc_Parse_Declarator*_tmpC54;
yyval=Cyc_YY29(((_tmpC54=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC54)),((
_tmpC54->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id,((_tmpC54->tms=((_tmpC59=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC59)),((_tmpC59->hd=(void*)((_tmpC55=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC55)),((_tmpC55[0]=((_tmpC58.tag=3,((_tmpC58.f1=(void*)((void*)((
_tmpC56=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC56)),((_tmpC56[0]=((
_tmpC57.tag=1,((_tmpC57.f1=0,((_tmpC57.f2=0,((_tmpC57.f3=0,((_tmpC57.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC57.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC57)))))))))))),
_tmpC56))))),_tmpC58)))),_tmpC55)))),((_tmpC59->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpC59)))))),
_tmpC54)))))));}break;case 180: _LL2E1:{struct Cyc_List_List*_tmpC6E;struct Cyc_Absyn_Function_mod_struct
_tmpC6D;struct Cyc_Absyn_NoTypes_struct _tmpC6C;struct Cyc_Absyn_NoTypes_struct*
_tmpC6B;struct Cyc_Absyn_Function_mod_struct*_tmpC6A;struct Cyc_Parse_Declarator*
_tmpC69;yyval=Cyc_YY29(((_tmpC69=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpC69)),((_tmpC69->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->id,((_tmpC69->tms=((_tmpC6E=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC6E)),((_tmpC6E->hd=(void*)((_tmpC6A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC6A)),((_tmpC6A[0]=((_tmpC6D.tag=3,((_tmpC6D.f1=(void*)((void*)((
_tmpC6B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC6B)),((_tmpC6B[0]=((
_tmpC6C.tag=0,((_tmpC6C.f1=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpC6C.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC6C)))))),
_tmpC6B))))),_tmpC6D)))),_tmpC6A)))),((_tmpC6E->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpC6E)))))),
_tmpC69)))))));}break;case 181: _LL2E2: {struct Cyc_List_List*_tmp633=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpC78;struct Cyc_Absyn_TypeParams_mod_struct _tmpC77;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpC76;struct Cyc_Parse_Declarator*_tmpC75;yyval=Cyc_YY29(((_tmpC75=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC75)),((_tmpC75->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC75->tms=((
_tmpC78=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC78)),((_tmpC78->hd=(
void*)((_tmpC76=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC76)),((
_tmpC76[0]=((_tmpC77.tag=4,((_tmpC77.f1=_tmp633,((_tmpC77.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC77.f3=0,
_tmpC77)))))))),_tmpC76)))),((_tmpC78->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpC78)))))),
_tmpC75)))))));}break;}case 182: _LL2E3:{struct Cyc_List_List*_tmpC82;struct Cyc_Absyn_Attributes_mod_struct
_tmpC81;struct Cyc_Absyn_Attributes_mod_struct*_tmpC80;struct Cyc_Parse_Declarator*
_tmpC7F;yyval=Cyc_YY29(((_tmpC7F=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpC7F)),((_tmpC7F->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]))->id,((_tmpC7F->tms=((_tmpC82=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC82)),((_tmpC82->hd=(void*)((_tmpC80=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC80)),((_tmpC80[0]=((_tmpC81.tag=5,((_tmpC81.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC81.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC81)))))),_tmpC80)))),((
_tmpC82->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpC82)))))),_tmpC7F)))))));}break;case 183: _LL2E4:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 184:
_LL2E5: yyval=Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 185: _LL2E6: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){struct Cyc_Absyn_Attributes_mod_struct*
_tmpC88;struct Cyc_Absyn_Attributes_mod_struct _tmpC87;struct Cyc_List_List*_tmpC86;
ans=((_tmpC86=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC86)),((_tmpC86->hd=(
void*)((_tmpC88=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC88)),((
_tmpC88[0]=((_tmpC87.tag=5,((_tmpC87.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpC87.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpC87)))))),_tmpC88)))),((_tmpC86->tl=ans,_tmpC86))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;struct Cyc_Position_Segment*_tmp640;union Cyc_Absyn_Constraint*_tmp641;
union Cyc_Absyn_Constraint*_tmp642;struct _tuple18 _tmp63F=*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp640=_tmp63F.f1;
_tmp641=_tmp63F.f2;_tmp642=_tmp63F.f3;if(Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*
_tmpC89;ptrloc=((_tmpC89=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC89)),((
_tmpC89->ptr_loc=_tmp640,((_tmpC89->rgn_loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpC89->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),_tmpC89))))))));}{union Cyc_Absyn_Constraint*
_tmp645;union Cyc_Absyn_Constraint*_tmp646;union Cyc_Absyn_Constraint*_tmp647;void*
_tmp648;struct _tuple11 _tmp644=Cyc_Parse_collapse_pointer_quals(_tmp640,_tmp641,
_tmp642,Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY54(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
_tmp645=_tmp644.f1;_tmp646=_tmp644.f2;_tmp647=_tmp644.f3;_tmp648=_tmp644.f4;{
struct Cyc_Absyn_Pointer_mod_struct*_tmpC93;struct Cyc_Absyn_PtrAtts _tmpC92;struct
Cyc_Absyn_Pointer_mod_struct _tmpC91;struct Cyc_List_List*_tmpC90;ans=((_tmpC90=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC90)),((_tmpC90->hd=(void*)((
_tmpC93=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC93)),((_tmpC93[0]=((
_tmpC91.tag=2,((_tmpC91.f1=((_tmpC92.rgn=_tmp648,((_tmpC92.nullable=_tmp645,((
_tmpC92.bounds=_tmp646,((_tmpC92.zero_term=_tmp647,((_tmpC92.ptrloc=ptrloc,
_tmpC92)))))))))),((_tmpC91.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC91)))))),_tmpC93)))),((
_tmpC90->tl=ans,_tmpC90))))));}yyval=Cyc_YY28(ans);break;};};}case 186: _LL2E7:
yyval=Cyc_YY54(0);break;case 187: _LL2E8:{struct Cyc_List_List*_tmpC94;yyval=Cyc_YY54(((
_tmpC94=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC94)),((_tmpC94->hd=
Cyc_yyget_YY53(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpC94->tl=Cyc_yyget_YY54(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpC94)))))));}break;case 188: _LL2E9:{struct Cyc_Absyn_Numelts_ptrqual_struct
_tmpC97;struct Cyc_Absyn_Numelts_ptrqual_struct*_tmpC96;yyval=Cyc_YY53((void*)((
_tmpC96=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpC96)),((_tmpC96[0]=((
_tmpC97.tag=0,((_tmpC97.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpC97)))),_tmpC96)))));}break;case 189: _LL2EA:{struct
Cyc_Absyn_Region_ptrqual_struct _tmpC9A;struct Cyc_Absyn_Region_ptrqual_struct*
_tmpC99;yyval=Cyc_YY53((void*)((_tmpC99=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC99)),((_tmpC99[0]=((_tmpC9A.tag=1,((_tmpC9A.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC9A)))),_tmpC99)))));}
break;case 190: _LL2EB:{struct Cyc_Absyn_Thin_ptrqual_struct _tmpC9D;struct Cyc_Absyn_Thin_ptrqual_struct*
_tmpC9C;yyval=Cyc_YY53((void*)((_tmpC9C=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC9C)),((_tmpC9C[0]=((_tmpC9D.tag=2,_tmpC9D)),_tmpC9C)))));}break;
case 191: _LL2EC:{struct Cyc_Absyn_Fat_ptrqual_struct _tmpCA0;struct Cyc_Absyn_Fat_ptrqual_struct*
_tmpC9F;yyval=Cyc_YY53((void*)((_tmpC9F=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpC9F)),((_tmpC9F[0]=((_tmpCA0.tag=3,_tmpCA0)),_tmpC9F)))));}break;
case 192: _LL2ED:{struct Cyc_Absyn_Zeroterm_ptrqual_struct _tmpCA3;struct Cyc_Absyn_Zeroterm_ptrqual_struct*
_tmpCA2;yyval=Cyc_YY53((void*)((_tmpCA2=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCA2)),((_tmpCA2[0]=((_tmpCA3.tag=4,_tmpCA3)),_tmpCA2)))));}break;
case 193: _LL2EE:{struct Cyc_Absyn_Nozeroterm_ptrqual_struct _tmpCA6;struct Cyc_Absyn_Nozeroterm_ptrqual_struct*
_tmpCA5;yyval=Cyc_YY53((void*)((_tmpCA5=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCA5)),((_tmpCA5[0]=((_tmpCA6.tag=5,_tmpCA6)),_tmpCA5)))));}break;
case 194: _LL2EF:{struct Cyc_Absyn_Notnull_ptrqual_struct _tmpCA9;struct Cyc_Absyn_Notnull_ptrqual_struct*
_tmpCA8;yyval=Cyc_YY53((void*)((_tmpCA8=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCA8)),((_tmpCA8[0]=((_tmpCA9.tag=6,_tmpCA9)),_tmpCA8)))));}break;
case 195: _LL2F0:{struct Cyc_Absyn_Nullable_ptrqual_struct _tmpCAC;struct Cyc_Absyn_Nullable_ptrqual_struct*
_tmpCAB;yyval=Cyc_YY53((void*)((_tmpCAB=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCAB)),((_tmpCAB[0]=((_tmpCAC.tag=7,_tmpCAC)),_tmpCAB)))));}break;
case 196: _LL2F1: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple18*_tmpCAD;yyval=Cyc_YY1(((_tmpCAD=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCAD)),((_tmpCAD->f1=loc,((_tmpCAD->f2=Cyc_Absyn_true_conref,((
_tmpCAD->f3=Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpCAD)))))))));}break;}case 197: _LL2F2: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple18*_tmpCAE;yyval=Cyc_YY1(((_tmpCAE=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCAE)),((_tmpCAE->f1=loc,((_tmpCAE->f2=Cyc_Absyn_false_conref,((
_tmpCAE->f3=Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpCAE)))))))));}break;}case 198: _LL2F3: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{struct _tuple18*
_tmpCAF;yyval=Cyc_YY1(((_tmpCAF=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpCAF)),((_tmpCAF->f1=loc,((_tmpCAF->f2=Cyc_Absyn_true_conref,((_tmpCAF->f3=
Cyc_Absyn_bounds_dyneither_conref,_tmpCAF)))))))));}break;}case 199: _LL2F4: yyval=
Cyc_YY2(Cyc_Absyn_bounds_one_conref);break;case 200: _LL2F5:{struct Cyc_Absyn_Upper_b_struct
_tmpCB2;struct Cyc_Absyn_Upper_b_struct*_tmpCB1;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmpCB1=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCB1)),((_tmpCB1[0]=((_tmpCB2.tag=1,((_tmpCB2.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCB2)))),_tmpCB1))))));}
break;case 201: _LL2F6: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 202: _LL2F7: yyval=Cyc_YY51(Cyc_Absyn_true_conref);break;case 203: _LL2F8:
yyval=Cyc_YY51(Cyc_Absyn_false_conref);break;case 204: _LL2F9: yyval=Cyc_YY44(Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));break;case 205: _LL2FA: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_trk,
1);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 206:
_LL2FB: yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));
break;case 207: _LL2FC: yyval=Cyc_YY25(Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)));break;case
208: _LL2FD: yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 209: _LL2FE:{
struct _tuple23*_tmpCB3;yyval=Cyc_YY39(((_tmpCB3=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCB3)),((_tmpCB3->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),((_tmpCB3->f2=0,((_tmpCB3->f3=0,((_tmpCB3->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCB3->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCB3)))))))))))));}
break;case 210: _LL2FF:{struct _tuple23*_tmpCB4;yyval=Cyc_YY39(((_tmpCB4=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCB4)),((_tmpCB4->f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),((_tmpCB4->f2=1,((
_tmpCB4->f3=0,((_tmpCB4->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpCB4->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCB4)))))))))))));}
break;case 211: _LL300: {struct _tuple8 _tmp666;struct Cyc_Core_Opt*_tmp667;struct Cyc_Absyn_Tqual
_tmp668;void*_tmp669;struct _tuple8*_tmp665=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp666=*_tmp665;_tmp667=
_tmp666.f1;_tmp668=_tmp666.f2;_tmp669=_tmp666.f3;{struct Cyc_Absyn_VarargInfo*
_tmpCB5;struct Cyc_Absyn_VarargInfo*_tmp66A=(_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((
_tmpCB5->name=_tmp667,((_tmpCB5->tq=_tmp668,((_tmpCB5->type=_tmp669,((_tmpCB5->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpCB5)))))))));{struct _tuple23*_tmpCB6;yyval=Cyc_YY39(((_tmpCB6=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpCB6)),((_tmpCB6->f1=0,((_tmpCB6->f2=0,((
_tmpCB6->f3=_tmp66A,((_tmpCB6->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCB6->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCB6)))))))))))));}
break;};}case 212: _LL301: {struct _tuple8 _tmp66E;struct Cyc_Core_Opt*_tmp66F;struct
Cyc_Absyn_Tqual _tmp670;void*_tmp671;struct _tuple8*_tmp66D=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp66E=*_tmp66D;_tmp66F=
_tmp66E.f1;_tmp670=_tmp66E.f2;_tmp671=_tmp66E.f3;{struct Cyc_Absyn_VarargInfo*
_tmpCB7;struct Cyc_Absyn_VarargInfo*_tmp672=(_tmpCB7=_cycalloc(sizeof(*_tmpCB7)),((
_tmpCB7->name=_tmp66F,((_tmpCB7->tq=_tmp670,((_tmpCB7->type=_tmp671,((_tmpCB7->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpCB7)))))))));{struct _tuple23*_tmpCB8;yyval=Cyc_YY39(((_tmpCB8=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpCB8)),((_tmpCB8->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)])),((_tmpCB8->f2=0,((
_tmpCB8->f3=_tmp672,((_tmpCB8->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCB8->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCB8)))))))))))));}
break;};}case 213: _LL302:{struct Cyc_Absyn_Unknown_kb_struct _tmpCBB;struct Cyc_Absyn_Unknown_kb_struct*
_tmpCBA;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)((_tmpCBA=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCBA)),((_tmpCBA[0]=((_tmpCBB.tag=
1,((_tmpCBB.f1=0,_tmpCBB)))),_tmpCBA))))));}break;case 214: _LL303: yyval=Cyc_YY44(
Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))));break;case 215: _LL304:
yyval=Cyc_YY49(0);break;case 216: _LL305:{struct Cyc_Absyn_JoinEff_struct*_tmpCC1;
struct Cyc_Absyn_JoinEff_struct _tmpCC0;struct Cyc_Core_Opt*_tmpCBF;yyval=Cyc_YY49(((
_tmpCBF=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCBF)),((_tmpCBF->v=(
void*)((_tmpCC1=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCC1)),((
_tmpCC1[0]=((_tmpCC0.tag=25,((_tmpCC0.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCC0)))),_tmpCC1)))),
_tmpCBF)))));}break;case 217: _LL306: yyval=Cyc_YY50(0);break;case 218: _LL307: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 219: _LL308: {
struct Cyc_Absyn_Less_kb_struct _tmpCC4;struct Cyc_Absyn_Less_kb_struct*_tmpCC3;
struct Cyc_Absyn_Less_kb_struct*_tmp67A=(_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((
_tmpCC3[0]=((_tmpCC4.tag=2,((_tmpCC4.f1=0,((_tmpCC4.f2=& Cyc_Tcutil_trk,_tmpCC4)))))),
_tmpCC3)));struct _dyneither_ptr _tmp67B=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp67C=Cyc_Parse_id2type(
_tmp67B,(void*)_tmp67A);{struct _tuple26*_tmpCCE;struct Cyc_Absyn_JoinEff_struct
_tmpCCD;struct Cyc_Absyn_JoinEff_struct*_tmpCCC;struct Cyc_List_List*_tmpCCB;yyval=
Cyc_YY50(((_tmpCCB=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCCB)),((
_tmpCCB->hd=((_tmpCCE=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCCE)),((
_tmpCCE->f1=(void*)((_tmpCCC=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpCCC)),((_tmpCCC[0]=((_tmpCCD.tag=25,((_tmpCCD.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCCD)))),_tmpCCC)))),((
_tmpCCE->f2=_tmp67C,_tmpCCE)))))),((_tmpCCB->tl=0,_tmpCCB)))))));}break;}case 220:
_LL309: {struct Cyc_Absyn_Less_kb_struct _tmpCD1;struct Cyc_Absyn_Less_kb_struct*
_tmpCD0;struct Cyc_Absyn_Less_kb_struct*_tmp683=(_tmpCD0=_cycalloc(sizeof(*
_tmpCD0)),((_tmpCD0[0]=((_tmpCD1.tag=2,((_tmpCD1.f1=0,((_tmpCD1.f2=& Cyc_Tcutil_trk,
_tmpCD1)))))),_tmpCD0)));struct _dyneither_ptr _tmp684=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp685=Cyc_Parse_id2type(
_tmp684,(void*)_tmp683);{struct _tuple26*_tmpCDB;struct Cyc_Absyn_JoinEff_struct
_tmpCDA;struct Cyc_Absyn_JoinEff_struct*_tmpCD9;struct Cyc_List_List*_tmpCD8;yyval=
Cyc_YY50(((_tmpCD8=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCD8)),((
_tmpCD8->hd=((_tmpCDB=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCDB)),((
_tmpCDB->f1=(void*)((_tmpCD9=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpCD9)),((_tmpCD9[0]=((_tmpCDA.tag=25,((_tmpCDA.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmpCDA)))),_tmpCD9)))),((
_tmpCDB->f2=_tmp685,_tmpCDB)))))),((_tmpCD8->tl=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCD8)))))));}break;}case
221: _LL30A: yyval=Cyc_YY31(0);break;case 222: _LL30B:{const char*_tmpCDC;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpCDC="inject",_tag_dyneither(_tmpCDC,sizeof(char),7))))
!= 0){const char*_tmpCDD;Cyc_Parse_err(((_tmpCDD="missing type in function declaration",
_tag_dyneither(_tmpCDD,sizeof(char),37))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY31(
1);break;case 223: _LL30C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 224: _LL30D: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 225: _LL30E:
yyval=Cyc_YY40(0);break;case 226: _LL30F: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 227: _LL310:{struct Cyc_Absyn_RgnsEff_struct*
_tmpCE3;struct Cyc_Absyn_RgnsEff_struct _tmpCE2;struct Cyc_List_List*_tmpCE1;yyval=
Cyc_YY40(((_tmpCE1=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpCE1)),((
_tmpCE1->hd=(void*)((_tmpCE3=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpCE3)),((_tmpCE3[0]=((_tmpCE2.tag=26,((_tmpCE2.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCE2)))),_tmpCE3)))),((
_tmpCE1->tl=0,_tmpCE1)))))));}break;case 228: _LL311: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_ek,0);{
struct Cyc_List_List*_tmpCE4;yyval=Cyc_YY40(((_tmpCE4=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCE4)),((_tmpCE4->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCE4->tl=0,_tmpCE4)))))));}
break;case 229: _LL312: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_trk,1);{struct
Cyc_Absyn_AccessEff_struct*_tmpCEA;struct Cyc_Absyn_AccessEff_struct _tmpCE9;
struct Cyc_List_List*_tmpCE8;yyval=Cyc_YY40(((_tmpCE8=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCE8)),((_tmpCE8->hd=(void*)((_tmpCEA=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCEA)),((_tmpCEA[0]=((_tmpCE9.tag=24,((_tmpCE9.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCE9)))),_tmpCEA)))),((
_tmpCE8->tl=0,_tmpCE8)))))));}break;case 230: _LL313: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),& Cyc_Tcutil_trk,1);{
struct Cyc_Absyn_AccessEff_struct*_tmpCF0;struct Cyc_Absyn_AccessEff_struct _tmpCEF;
struct Cyc_List_List*_tmpCEE;yyval=Cyc_YY40(((_tmpCEE=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCEE)),((_tmpCEE->hd=(void*)((_tmpCF0=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCF0)),((_tmpCF0[0]=((_tmpCEF.tag=24,((_tmpCEF.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCEF)))),_tmpCF0)))),((
_tmpCEE->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpCEE)))))));}break;case 231: _LL314:{struct Cyc_List_List*
_tmpCF1;yyval=Cyc_YY38(((_tmpCF1=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpCF1)),((_tmpCF1->hd=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpCF1->tl=0,_tmpCF1)))))));}break;case 232: _LL315:{struct Cyc_List_List*
_tmpCF2;yyval=Cyc_YY38(((_tmpCF2=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpCF2)),((_tmpCF2->hd=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpCF2->tl=Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpCF2)))))));}break;case 233: _LL316: {struct _tuple22
_tmp69B;struct Cyc_Absyn_Tqual _tmp69C;struct Cyc_List_List*_tmp69D;struct Cyc_List_List*
_tmp69E;struct _tuple22*_tmp69A=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp69B=*_tmp69A;_tmp69C=
_tmp69B.f1;_tmp69D=_tmp69B.f2;_tmp69E=_tmp69B.f3;if(_tmp69C.loc == 0)_tmp69C.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp6A0;struct _tuple1*
_tmp6A1;struct Cyc_List_List*_tmp6A2;struct Cyc_Parse_Declarator*_tmp69F=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A0=*_tmp69F;
_tmp6A1=_tmp6A0.id;_tmp6A2=_tmp6A0.tms;{void*_tmp6A3=Cyc_Parse_speclist2typ(
_tmp69D,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp6A5;void*_tmp6A6;struct
Cyc_List_List*_tmp6A7;struct Cyc_List_List*_tmp6A8;struct _tuple10 _tmp6A4=Cyc_Parse_apply_tms(
_tmp69C,_tmp6A3,_tmp69E,_tmp6A2);_tmp6A5=_tmp6A4.f1;_tmp6A6=_tmp6A4.f2;_tmp6A7=
_tmp6A4.f3;_tmp6A8=_tmp6A4.f4;if(_tmp6A7 != 0){const char*_tmpCF3;Cyc_Parse_err(((
_tmpCF3="parameter with bad type params",_tag_dyneither(_tmpCF3,sizeof(char),31))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp6A1)){const char*
_tmpCF4;Cyc_Parse_err(((_tmpCF4="parameter cannot be qualified with a namespace",
_tag_dyneither(_tmpCF4,sizeof(char),47))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*
_tmpCF5;struct Cyc_Core_Opt*_tmp6AB=(struct Cyc_Core_Opt*)((_tmpCF5=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpCF5)),((_tmpCF5->v=(*_tmp6A1).f2,_tmpCF5))));
if(_tmp6A8 != 0){const char*_tmpCF8;void*_tmpCF7;(_tmpCF7=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpCF8="extra attributes on parameter, ignoring",
_tag_dyneither(_tmpCF8,sizeof(char),40))),_tag_dyneither(_tmpCF7,sizeof(void*),0)));}{
struct _tuple8*_tmpCF9;yyval=Cyc_YY37(((_tmpCF9=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCF9)),((_tmpCF9->f1=_tmp6AB,((_tmpCF9->f2=_tmp6A5,((_tmpCF9->f3=
_tmp6A6,_tmpCF9)))))))));}break;};};};}case 234: _LL317: {struct _tuple22 _tmp6B1;
struct Cyc_Absyn_Tqual _tmp6B2;struct Cyc_List_List*_tmp6B3;struct Cyc_List_List*
_tmp6B4;struct _tuple22*_tmp6B0=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B1=*_tmp6B0;_tmp6B2=
_tmp6B1.f1;_tmp6B3=_tmp6B1.f2;_tmp6B4=_tmp6B1.f3;if(_tmp6B2.loc == 0)_tmp6B2.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp6B5=Cyc_Parse_speclist2typ(_tmp6B3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6B4 != 0){
const char*_tmpCFC;void*_tmpCFB;(_tmpCFB=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpCFC="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpCFC,sizeof(char),38))),_tag_dyneither(_tmpCFB,sizeof(void*),0)));}{
struct _tuple8*_tmpCFD;yyval=Cyc_YY37(((_tmpCFD=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpCFD)),((_tmpCFD->f1=0,((_tmpCFD->f2=_tmp6B2,((_tmpCFD->f3=_tmp6B5,
_tmpCFD)))))))));}break;};}case 235: _LL318: {struct _tuple22 _tmp6BA;struct Cyc_Absyn_Tqual
_tmp6BB;struct Cyc_List_List*_tmp6BC;struct Cyc_List_List*_tmp6BD;struct _tuple22*
_tmp6B9=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6BA=*_tmp6B9;_tmp6BB=_tmp6BA.f1;_tmp6BC=_tmp6BA.f2;_tmp6BD=_tmp6BA.f3;if(
_tmp6BB.loc == 0)_tmp6BB.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{void*_tmp6BE=
Cyc_Parse_speclist2typ(_tmp6BC,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6BF=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp6C1;void*_tmp6C2;struct Cyc_List_List*_tmp6C3;struct Cyc_List_List*
_tmp6C4;struct _tuple10 _tmp6C0=Cyc_Parse_apply_tms(_tmp6BB,_tmp6BE,_tmp6BD,
_tmp6BF);_tmp6C1=_tmp6C0.f1;_tmp6C2=_tmp6C0.f2;_tmp6C3=_tmp6C0.f3;_tmp6C4=
_tmp6C0.f4;if(_tmp6C3 != 0){const char*_tmpD00;void*_tmpCFF;(_tmpCFF=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpD00="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpD00,sizeof(char),49))),_tag_dyneither(_tmpCFF,sizeof(void*),0)));}
if(_tmp6C4 != 0){const char*_tmpD03;void*_tmpD02;(_tmpD02=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD03="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpD03,sizeof(char),38))),_tag_dyneither(_tmpD02,sizeof(void*),0)));}{
struct _tuple8*_tmpD04;yyval=Cyc_YY37(((_tmpD04=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD04)),((_tmpD04->f1=0,((_tmpD04->f2=_tmp6C1,((_tmpD04->f3=_tmp6C2,
_tmpD04)))))))));}break;};}case 236: _LL319: yyval=Cyc_YY36(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 237: _LL31A:{
struct _dyneither_ptr*_tmpD07;struct Cyc_List_List*_tmpD06;yyval=Cyc_YY36(((
_tmpD06=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD06)),((_tmpD06->hd=((
_tmpD07=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD07)),((_tmpD07[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD07)))),((_tmpD06->tl=
0,_tmpD06)))))));}break;case 238: _LL31B:{struct _dyneither_ptr*_tmpD0A;struct Cyc_List_List*
_tmpD09;yyval=Cyc_YY36(((_tmpD09=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD09)),((_tmpD09->hd=((_tmpD0A=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD0A)),((_tmpD0A[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),_tmpD0A)))),((_tmpD09->tl=Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD09)))))));}break;
case 239: _LL31C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 240: _LL31D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 241: _LL31E:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD0D;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD0C;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD0C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD0C)),((
_tmpD0C[0]=((_tmpD0D.tag=36,((_tmpD0D.f1=0,((_tmpD0D.f2=0,_tmpD0D)))))),_tmpD0C)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 242: _LL31F:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD10;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD0F;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD0F=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD0F)),((
_tmpD0F[0]=((_tmpD10.tag=36,((_tmpD10.f1=0,((_tmpD10.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmpD10)))))),_tmpD0F)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 243: _LL320:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD13;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD12;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD12=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD12)),((
_tmpD12[0]=((_tmpD13.tag=36,((_tmpD13.f1=0,((_tmpD13.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),_tmpD13)))))),_tmpD12)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 244: _LL321: {struct _dyneither_ptr*
_tmpD16;struct _tuple1*_tmpD15;struct Cyc_Absyn_Vardecl*_tmp6D4=Cyc_Absyn_new_vardecl(((
_tmpD15=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD15)),((_tmpD15->f1=
Cyc_Absyn_Loc_n,((_tmpD15->f2=((_tmpD16=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD16)),((_tmpD16[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpD16)))),_tmpD15)))))),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp6D4->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct _tmpD19;struct Cyc_Absyn_Comprehension_e_struct*
_tmpD18;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD18=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD18)),((_tmpD18[0]=((_tmpD19.tag=28,((_tmpD19.f1=_tmp6D4,((_tmpD19.f2=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpD19.f3=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),((_tmpD19.f4=0,_tmpD19)))))))))),_tmpD18)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
245: _LL322:{struct _tuple27*_tmpD1C;struct Cyc_List_List*_tmpD1B;yyval=Cyc_YY6(((
_tmpD1B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD1B)),((_tmpD1B->hd=((
_tmpD1C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD1C)),((_tmpD1C->f1=0,((
_tmpD1C->f2=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpD1C)))))),((_tmpD1B->tl=0,_tmpD1B)))))));}break;case 246: _LL323:{struct
_tuple27*_tmpD1F;struct Cyc_List_List*_tmpD1E;yyval=Cyc_YY6(((_tmpD1E=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD1E)),((_tmpD1E->hd=((_tmpD1F=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD1F)),((_tmpD1F->f1=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD1F->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD1F)))))),((
_tmpD1E->tl=0,_tmpD1E)))))));}break;case 247: _LL324:{struct _tuple27*_tmpD22;
struct Cyc_List_List*_tmpD21;yyval=Cyc_YY6(((_tmpD21=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD21)),((_tmpD21->hd=((_tmpD22=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD22)),((_tmpD22->f1=0,((_tmpD22->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD22)))))),((_tmpD21->tl=
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpD21)))))));}break;case 248: _LL325:{struct _tuple27*_tmpD25;struct Cyc_List_List*
_tmpD24;yyval=Cyc_YY6(((_tmpD24=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD24)),((_tmpD24->hd=((_tmpD25=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD25)),((_tmpD25->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpD25->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD25)))))),((_tmpD24->tl=
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpD24)))))));}break;case 249: _LL326: yyval=Cyc_YY41(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 250: _LL327:{
struct Cyc_List_List*_tmpD26;yyval=Cyc_YY41(((_tmpD26=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD26)),((_tmpD26->hd=Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD26->tl=0,_tmpD26)))))));}
break;case 251: _LL328:{struct Cyc_List_List*_tmpD27;yyval=Cyc_YY41(((_tmpD27=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD27)),((_tmpD27->hd=Cyc_yyget_YY42(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD27->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD27)))))));}
break;case 252: _LL329:{struct Cyc_Absyn_ArrayElement_struct _tmpD2A;struct Cyc_Absyn_ArrayElement_struct*
_tmpD29;yyval=Cyc_YY42((void*)((_tmpD29=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD29)),((_tmpD29[0]=((_tmpD2A.tag=0,((_tmpD2A.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD2A)))),_tmpD29)))));}
break;case 253: _LL32A:{struct Cyc_Absyn_FieldName_struct _tmpD30;struct
_dyneither_ptr*_tmpD2F;struct Cyc_Absyn_FieldName_struct*_tmpD2E;yyval=Cyc_YY42((
void*)((_tmpD2E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD2E)),((
_tmpD2E[0]=((_tmpD30.tag=1,((_tmpD30.f1=((_tmpD2F=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD2F)),((_tmpD2F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD2F)))),_tmpD30)))),
_tmpD2E)))));}break;case 254: _LL32B: {struct _tuple22 _tmp6E9;struct Cyc_Absyn_Tqual
_tmp6EA;struct Cyc_List_List*_tmp6EB;struct Cyc_List_List*_tmp6EC;struct _tuple22*
_tmp6E8=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp6E9=*_tmp6E8;_tmp6EA=_tmp6E9.f1;_tmp6EB=_tmp6E9.f2;_tmp6EC=_tmp6E9.f3;{void*
_tmp6ED=Cyc_Parse_speclist2typ(_tmp6EB,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6EC != 0){
const char*_tmpD33;void*_tmpD32;(_tmpD32=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD33="ignoring attributes in type",
_tag_dyneither(_tmpD33,sizeof(char),28))),_tag_dyneither(_tmpD32,sizeof(void*),0)));}{
struct _tuple8*_tmpD34;yyval=Cyc_YY37(((_tmpD34=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD34)),((_tmpD34->f1=0,((_tmpD34->f2=_tmp6EA,((_tmpD34->f3=_tmp6ED,
_tmpD34)))))))));}break;};}case 255: _LL32C: {struct _tuple22 _tmp6F2;struct Cyc_Absyn_Tqual
_tmp6F3;struct Cyc_List_List*_tmp6F4;struct Cyc_List_List*_tmp6F5;struct _tuple22*
_tmp6F1=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6F2=*_tmp6F1;_tmp6F3=_tmp6F2.f1;_tmp6F4=_tmp6F2.f2;_tmp6F5=_tmp6F2.f3;{void*
_tmp6F6=Cyc_Parse_speclist2typ(_tmp6F4,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6F7=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple10 _tmp6F8=Cyc_Parse_apply_tms(_tmp6F3,_tmp6F6,_tmp6F5,_tmp6F7);if(
_tmp6F8.f3 != 0){const char*_tmpD37;void*_tmpD36;(_tmpD36=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD37="bad type params, ignoring",
_tag_dyneither(_tmpD37,sizeof(char),26))),_tag_dyneither(_tmpD36,sizeof(void*),0)));}
if(_tmp6F8.f4 != 0){const char*_tmpD3A;void*_tmpD39;(_tmpD39=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD3A="bad specifiers, ignoring",
_tag_dyneither(_tmpD3A,sizeof(char),25))),_tag_dyneither(_tmpD39,sizeof(void*),0)));}{
struct _tuple8*_tmpD3B;yyval=Cyc_YY37(((_tmpD3B=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD3B)),((_tmpD3B->f1=0,((_tmpD3B->f2=_tmp6F8.f1,((_tmpD3B->f3=_tmp6F8.f2,
_tmpD3B)))))))));}break;};}case 256: _LL32D: yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 257:
_LL32E:{struct Cyc_Absyn_JoinEff_struct _tmpD3E;struct Cyc_Absyn_JoinEff_struct*
_tmpD3D;yyval=Cyc_YY44((void*)((_tmpD3D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD3D)),((_tmpD3D[0]=((_tmpD3E.tag=25,((_tmpD3E.f1=0,_tmpD3E)))),
_tmpD3D)))));}break;case 258: _LL32F:{struct Cyc_Absyn_JoinEff_struct _tmpD41;struct
Cyc_Absyn_JoinEff_struct*_tmpD40;yyval=Cyc_YY44((void*)((_tmpD40=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpD40)),((_tmpD40[0]=((_tmpD41.tag=25,((_tmpD41.f1=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpD41)))),_tmpD40)))));}break;case 259: _LL330:{struct Cyc_Absyn_RgnsEff_struct
_tmpD44;struct Cyc_Absyn_RgnsEff_struct*_tmpD43;yyval=Cyc_YY44((void*)((_tmpD43=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD43)),((_tmpD43[0]=((_tmpD44.tag=
26,((_tmpD44.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpD44)))),_tmpD43)))));}break;case 260: _LL331:{struct Cyc_Absyn_JoinEff_struct
_tmpD4A;struct Cyc_List_List*_tmpD49;struct Cyc_Absyn_JoinEff_struct*_tmpD48;yyval=
Cyc_YY44((void*)((_tmpD48=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD48)),((
_tmpD48[0]=((_tmpD4A.tag=25,((_tmpD4A.f1=((_tmpD49=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD49)),((_tmpD49->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD49->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD49)))))),_tmpD4A)))),
_tmpD48)))));}break;case 261: _LL332:{struct Cyc_List_List*_tmpD4B;yyval=Cyc_YY40(((
_tmpD4B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD4B)),((_tmpD4B->hd=
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpD4B->tl=0,_tmpD4B)))))));}break;case 262: _LL333:{struct Cyc_List_List*_tmpD4C;
yyval=Cyc_YY40(((_tmpD4C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD4C)),((
_tmpD4C->hd=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD4C->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpD4C)))))));}break;case 263: _LL334:{struct Cyc_Parse_Abstractdeclarator*
_tmpD4D;yyval=Cyc_YY30(((_tmpD4D=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD4D)),((_tmpD4D->tms=Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),_tmpD4D)))));}break;case 264: _LL335: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 265: _LL336:{struct
Cyc_Parse_Abstractdeclarator*_tmpD4E;yyval=Cyc_YY30(((_tmpD4E=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD4E)),((_tmpD4E->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpD4E)))));}break;
case 266: _LL337: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 267: _LL338:{struct Cyc_List_List*_tmpD58;struct Cyc_Absyn_Carray_mod_struct
_tmpD57;struct Cyc_Absyn_Carray_mod_struct*_tmpD56;struct Cyc_Parse_Abstractdeclarator*
_tmpD55;yyval=Cyc_YY30(((_tmpD55=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD55)),((_tmpD55->tms=((_tmpD58=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD58)),((_tmpD58->hd=(void*)((_tmpD56=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD56)),((_tmpD56[0]=((_tmpD57.tag=0,((_tmpD57.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD57.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD57)))))),
_tmpD56)))),((_tmpD58->tl=0,_tmpD58)))))),_tmpD55)))));}break;case 268: _LL339:{
struct Cyc_List_List*_tmpD62;struct Cyc_Absyn_Carray_mod_struct _tmpD61;struct Cyc_Absyn_Carray_mod_struct*
_tmpD60;struct Cyc_Parse_Abstractdeclarator*_tmpD5F;yyval=Cyc_YY30(((_tmpD5F=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD5F)),((_tmpD5F->tms=((_tmpD62=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD62)),((_tmpD62->hd=(void*)((
_tmpD60=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD60)),((_tmpD60[0]=((
_tmpD61.tag=0,((_tmpD61.f1=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpD61.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD61)))))),
_tmpD60)))),((_tmpD62->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD62)))))),_tmpD5F)))));}break;case 269: _LL33A:{
struct Cyc_List_List*_tmpD6C;struct Cyc_Absyn_ConstArray_mod_struct _tmpD6B;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD6A;struct Cyc_Parse_Abstractdeclarator*
_tmpD69;yyval=Cyc_YY30(((_tmpD69=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD69)),((_tmpD69->tms=((_tmpD6C=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD6C)),((_tmpD6C->hd=(void*)((_tmpD6A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD6A)),((_tmpD6A[0]=((_tmpD6B.tag=1,((_tmpD6B.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD6B.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD6B.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD6B)))))))),
_tmpD6A)))),((_tmpD6C->tl=0,_tmpD6C)))))),_tmpD69)))));}break;case 270: _LL33B:{
struct Cyc_List_List*_tmpD76;struct Cyc_Absyn_ConstArray_mod_struct _tmpD75;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD74;struct Cyc_Parse_Abstractdeclarator*
_tmpD73;yyval=Cyc_YY30(((_tmpD73=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD73)),((_tmpD73->tms=((_tmpD76=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD76)),((_tmpD76->hd=(void*)((_tmpD74=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD74)),((_tmpD74[0]=((_tmpD75.tag=1,((_tmpD75.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD75.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD75.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD75)))))))),
_tmpD74)))),((_tmpD76->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpD76)))))),_tmpD73)))));}break;case 271: _LL33C:{
struct Cyc_List_List*_tmpD8B;struct Cyc_Absyn_Function_mod_struct _tmpD8A;struct Cyc_Absyn_WithTypes_struct
_tmpD89;struct Cyc_Absyn_WithTypes_struct*_tmpD88;struct Cyc_Absyn_Function_mod_struct*
_tmpD87;struct Cyc_Parse_Abstractdeclarator*_tmpD86;yyval=Cyc_YY30(((_tmpD86=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD86)),((_tmpD86->tms=((_tmpD8B=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD8B)),((_tmpD8B->hd=(void*)((
_tmpD87=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD87)),((_tmpD87[0]=((
_tmpD8A.tag=3,((_tmpD8A.f1=(void*)((void*)((_tmpD88=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD88)),((_tmpD88[0]=((_tmpD89.tag=1,((_tmpD89.f1=0,((_tmpD89.f2=0,((
_tmpD89.f3=0,((_tmpD89.f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),((_tmpD89.f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD89)))))))))))),
_tmpD88))))),_tmpD8A)))),_tmpD87)))),((_tmpD8B->tl=0,_tmpD8B)))))),_tmpD86)))));}
break;case 272: _LL33D: {struct _tuple23 _tmp722;struct Cyc_List_List*_tmp723;int
_tmp724;struct Cyc_Absyn_VarargInfo*_tmp725;struct Cyc_Core_Opt*_tmp726;struct Cyc_List_List*
_tmp727;struct _tuple23*_tmp721=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp722=*_tmp721;_tmp723=
_tmp722.f1;_tmp724=_tmp722.f2;_tmp725=_tmp722.f3;_tmp726=_tmp722.f4;_tmp727=
_tmp722.f5;{struct Cyc_List_List*_tmpDA0;struct Cyc_Absyn_Function_mod_struct
_tmpD9F;struct Cyc_Absyn_WithTypes_struct _tmpD9E;struct Cyc_Absyn_WithTypes_struct*
_tmpD9D;struct Cyc_Absyn_Function_mod_struct*_tmpD9C;struct Cyc_Parse_Abstractdeclarator*
_tmpD9B;yyval=Cyc_YY30(((_tmpD9B=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpD9B)),((_tmpD9B->tms=((_tmpDA0=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpDA0)),((_tmpDA0->hd=(void*)((_tmpD9C=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpD9C)),((_tmpD9C[0]=((_tmpD9F.tag=3,((_tmpD9F.f1=(void*)((void*)((
_tmpD9D=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD9D)),((_tmpD9D[0]=((
_tmpD9E.tag=1,((_tmpD9E.f1=_tmp723,((_tmpD9E.f2=_tmp724,((_tmpD9E.f3=_tmp725,((
_tmpD9E.f4=_tmp726,((_tmpD9E.f5=_tmp727,_tmpD9E)))))))))))),_tmpD9D))))),_tmpD9F)))),
_tmpD9C)))),((_tmpDA0->tl=0,_tmpDA0)))))),_tmpD9B)))));}break;}case 273: _LL33E:{
struct Cyc_List_List*_tmpDB5;struct Cyc_Absyn_Function_mod_struct _tmpDB4;struct Cyc_Absyn_WithTypes_struct
_tmpDB3;struct Cyc_Absyn_WithTypes_struct*_tmpDB2;struct Cyc_Absyn_Function_mod_struct*
_tmpDB1;struct Cyc_Parse_Abstractdeclarator*_tmpDB0;yyval=Cyc_YY30(((_tmpDB0=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDB0)),((_tmpDB0->tms=((_tmpDB5=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDB5)),((_tmpDB5->hd=(void*)((
_tmpDB1=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDB1)),((_tmpDB1[0]=((
_tmpDB4.tag=3,((_tmpDB4.f1=(void*)((void*)((_tmpDB2=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDB2)),((_tmpDB2[0]=((_tmpDB3.tag=1,((_tmpDB3.f1=0,((_tmpDB3.f2=0,((
_tmpDB3.f3=0,((_tmpDB3.f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),((_tmpDB3.f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDB3)))))))))))),
_tmpDB2))))),_tmpDB4)))),_tmpDB1)))),((_tmpDB5->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpDB5)))))),
_tmpDB0)))));}break;case 274: _LL33F: {struct _tuple23 _tmp735;struct Cyc_List_List*
_tmp736;int _tmp737;struct Cyc_Absyn_VarargInfo*_tmp738;struct Cyc_Core_Opt*_tmp739;
struct Cyc_List_List*_tmp73A;struct _tuple23*_tmp734=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp735=*_tmp734;_tmp736=
_tmp735.f1;_tmp737=_tmp735.f2;_tmp738=_tmp735.f3;_tmp739=_tmp735.f4;_tmp73A=
_tmp735.f5;{struct Cyc_List_List*_tmpDCA;struct Cyc_Absyn_Function_mod_struct
_tmpDC9;struct Cyc_Absyn_WithTypes_struct _tmpDC8;struct Cyc_Absyn_WithTypes_struct*
_tmpDC7;struct Cyc_Absyn_Function_mod_struct*_tmpDC6;struct Cyc_Parse_Abstractdeclarator*
_tmpDC5;yyval=Cyc_YY30(((_tmpDC5=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpDC5)),((_tmpDC5->tms=((_tmpDCA=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpDCA)),((_tmpDCA->hd=(void*)((_tmpDC6=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDC6)),((_tmpDC6[0]=((_tmpDC9.tag=3,((_tmpDC9.f1=(void*)((void*)((
_tmpDC7=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDC7)),((_tmpDC7[0]=((
_tmpDC8.tag=1,((_tmpDC8.f1=_tmp736,((_tmpDC8.f2=_tmp737,((_tmpDC8.f3=_tmp738,((
_tmpDC8.f4=_tmp739,((_tmpDC8.f5=_tmp73A,_tmpDC8)))))))))))),_tmpDC7))))),_tmpDC9)))),
_tmpDC6)))),((_tmpDCA->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpDCA)))))),_tmpDC5)))));}break;}case 275: _LL340: {
struct Cyc_List_List*_tmp741=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpDD4;struct Cyc_Absyn_TypeParams_mod_struct _tmpDD3;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpDD2;struct Cyc_Parse_Abstractdeclarator*_tmpDD1;yyval=Cyc_YY30(((_tmpDD1=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDD1)),((_tmpDD1->tms=((_tmpDD4=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDD4)),((_tmpDD4->hd=(void*)((
_tmpDD2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDD2)),((_tmpDD2[0]=((
_tmpDD3.tag=4,((_tmpDD3.f1=_tmp741,((_tmpDD3.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpDD3.f3=0,
_tmpDD3)))))))),_tmpDD2)))),((_tmpDD4->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpDD4)))))),
_tmpDD1)))));}break;}case 276: _LL341:{struct Cyc_List_List*_tmpDDE;struct Cyc_Absyn_Attributes_mod_struct
_tmpDDD;struct Cyc_Absyn_Attributes_mod_struct*_tmpDDC;struct Cyc_Parse_Abstractdeclarator*
_tmpDDB;yyval=Cyc_YY30(((_tmpDDB=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpDDB)),((_tmpDDB->tms=((_tmpDDE=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpDDE)),((_tmpDDE->hd=(void*)((_tmpDDC=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDDC)),((_tmpDDC[0]=((_tmpDDD.tag=5,((_tmpDDD.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpDDD.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDDD)))))),_tmpDDC)))),((
_tmpDDE->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpDDE)))))),_tmpDDB)))));}break;case 277: _LL342: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 278: _LL343:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 279:
_LL344: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
280: _LL345: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 281: _LL346: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 282: _LL347: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 283: _LL348:{struct Cyc_Absyn_ResetRegion_s_struct _tmpDE1;
struct Cyc_Absyn_ResetRegion_s_struct*_tmpDE0;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((
void*)((_tmpDE0=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDE0)),((
_tmpDE0[0]=((_tmpDE1.tag=16,((_tmpDE1.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpDE1)))),_tmpDE0)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 284: _LL349: yyval=Cyc_YY4(0);break;case
285: _LL34A:{const char*_tmpDE2;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpDE2="open",
_tag_dyneither(_tmpDE2,sizeof(char),5))))!= 0){const char*_tmpDE3;Cyc_Parse_err(((
_tmpDE3="expecting `open'",_tag_dyneither(_tmpDE3,sizeof(char),17))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}yyval=Cyc_YY4((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));break;case 286: _LL34B:{struct Cyc_Absyn_Label_s_struct _tmpDE9;
struct _dyneither_ptr*_tmpDE8;struct Cyc_Absyn_Label_s_struct*_tmpDE7;yyval=Cyc_YY9(
Cyc_Absyn_new_stmt((void*)((_tmpDE7=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDE7)),((_tmpDE7[0]=((_tmpDE9.tag=13,((_tmpDE9.f1=((_tmpDE8=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDE8)),((_tmpDE8[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpDE8)))),((
_tmpDE9.f2=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpDE9)))))),_tmpDE7)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 287:
_LL34C: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 288:
_LL34D: yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 289:
_LL34E: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 290:
_LL34F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 291: _LL350: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0)));
break;case 292: _LL351: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 293: _LL352:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 294:
_LL353: yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 295:
_LL354:{struct Cyc_Absyn_Fn_d_struct _tmpDEC;struct Cyc_Absyn_Fn_d_struct*_tmpDEB;
yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDEB=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDEB)),((_tmpDEB[0]=((_tmpDEC.tag=
1,((_tmpDEC.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpDEC)))),_tmpDEB)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(
0)));}break;case 296: _LL355:{struct Cyc_Absyn_Fn_d_struct _tmpDEF;struct Cyc_Absyn_Fn_d_struct*
_tmpDEE;yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDEE=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDEE)),((_tmpDEE[0]=((_tmpDEF.tag=
1,((_tmpDEF.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpDEF)))),_tmpDEE)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));}break;case 297:
_LL356: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 298:
_LL357: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 299:
_LL358: yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 300:
_LL359: {struct Cyc_Absyn_UnknownId_e_struct _tmpDF2;struct Cyc_Absyn_UnknownId_e_struct*
_tmpDF1;struct Cyc_Absyn_Exp*_tmp755=Cyc_Absyn_new_exp((void*)((_tmpDF1=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF2.tag=
2,((_tmpDF2.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),_tmpDF2)))),_tmpDF1)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(
Cyc_Absyn_switch_stmt(_tmp755,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 301:
_LL35A: {struct Cyc_Absyn_Exp*_tmp758=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(
Cyc_Absyn_switch_stmt(_tmp758,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 302:
_LL35B: yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 303:
_LL35C: yyval=Cyc_YY10(0);break;case 304: _LL35D:{struct Cyc_Absyn_Switch_clause*
_tmpDF5;struct Cyc_List_List*_tmpDF4;yyval=Cyc_YY10(((_tmpDF4=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDF4)),((_tmpDF4->hd=((_tmpDF5=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDF5)),((_tmpDF5->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)),((_tmpDF5->pat_vars=0,((_tmpDF5->where_clause=0,((
_tmpDF5->body=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpDF5->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDF5)))))))))))),((
_tmpDF4->tl=0,_tmpDF4)))))));}break;case 305: _LL35E:{struct Cyc_Absyn_Switch_clause*
_tmpDF8;struct Cyc_List_List*_tmpDF7;yyval=Cyc_YY10(((_tmpDF7=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDF7)),((_tmpDF7->hd=((_tmpDF8=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpDF8)),((_tmpDF8->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDF8->pat_vars=0,((
_tmpDF8->where_clause=0,((_tmpDF8->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),((_tmpDF8->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpDF8)))))))))))),((_tmpDF7->tl=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDF7)))))));}break;case
306: _LL35F:{struct Cyc_Absyn_Switch_clause*_tmpDFB;struct Cyc_List_List*_tmpDFA;
yyval=Cyc_YY10(((_tmpDFA=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDFA)),((
_tmpDFA->hd=((_tmpDFB=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDFB)),((
_tmpDFB->pattern=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpDFB->pat_vars=0,((_tmpDFB->where_clause=0,((_tmpDFB->body=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpDFB->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmpDFB)))))))))))),((_tmpDFA->tl=Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDFA)))))));}break;
case 307: _LL360:{struct Cyc_Absyn_Switch_clause*_tmpDFE;struct Cyc_List_List*
_tmpDFD;yyval=Cyc_YY10(((_tmpDFD=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpDFD)),((_tmpDFD->hd=((_tmpDFE=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpDFE)),((_tmpDFE->pattern=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),((_tmpDFE->pat_vars=0,((_tmpDFE->where_clause=(struct
Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmpDFE->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),((_tmpDFE->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpDFE)))))))))))),((_tmpDFD->tl=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDFD)))))));}break;case
308: _LL361:{struct Cyc_Absyn_Switch_clause*_tmpE01;struct Cyc_List_List*_tmpE00;
yyval=Cyc_YY10(((_tmpE00=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE00)),((
_tmpE00->hd=((_tmpE01=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE01)),((
_tmpE01->pattern=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),((_tmpE01->pat_vars=0,((_tmpE01->where_clause=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpE01->body=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpE01->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE01)))))))))))),((
_tmpE00->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE00)))))));}break;case 309: _LL362: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 310:
_LL363: yyval=Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 311:
_LL364: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 312:
_LL365: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 313:
_LL366: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 314:
_LL367: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
315: _LL368: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 316: _LL369: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 317:
_LL36A: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 318:
_LL36B: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 319:
_LL36C: {struct Cyc_List_List*_tmp763=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp764=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp763,_tmp764));break;}case 320: _LL36D: {struct
Cyc_List_List*_tmp765=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp766=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp765,
_tmp766));break;}case 321: _LL36E: {struct Cyc_List_List*_tmp767=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp768=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp767,_tmp768));break;}case 322: _LL36F: {struct
Cyc_List_List*_tmp769=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp76A=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp769,_tmp76A));break;}case 323: _LL370:{struct
_dyneither_ptr*_tmpE02;yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((_tmpE02=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE02)),((_tmpE02[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE02)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));}break;case
324: _LL371: yyval=Cyc_YY9(Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
325: _LL372: yyval=Cyc_YY9(Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
326: _LL373: yyval=Cyc_YY9(Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
327: _LL374: yyval=Cyc_YY9(Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
328: _LL375: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
329: _LL376: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)));break;case
330: _LL377: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)));break;case
331: _LL378: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 332: _LL379: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 333: _LL37A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 334: _LL37B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 335: _LL37C: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 336:
_LL37D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
337: _LL37E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 338:
_LL37F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
339: _LL380: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 340: _LL381: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 341: _LL382: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 342:
_LL383: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
343: _LL384: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 344: _LL385: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 345: _LL386: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 346:
_LL387: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 347:
_LL388: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
348: _LL389: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 349:
_LL38A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 350:
_LL38B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 351:
_LL38C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 352:
_LL38D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
353: _LL38E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 354: _LL38F: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 355:
_LL390: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
356: _LL391: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 357: _LL392: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 358:
_LL393: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
359: _LL394: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 360: _LL395: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 361:
_LL396: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 362:
_LL397: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
363: _LL398: {void*_tmp76C=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));yyval=Cyc_YY11(
Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp76C,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;}case 364: _LL399: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 365: _LL39A: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));break;case 366: _LL39B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 367: _LL39C: {void*_tmp76D=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY11(
Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp76D,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;}case
368: _LL39D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 369:
_LL39E:{struct Cyc_Absyn_StructField_struct _tmpE08;struct _dyneither_ptr*_tmpE07;
struct Cyc_Absyn_StructField_struct*_tmpE06;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpE06=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE06)),((
_tmpE06[0]=((_tmpE08.tag=0,((_tmpE08.f1=((_tmpE07=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE07)),((_tmpE07[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE07)))),_tmpE08)))),
_tmpE06)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 370: _LL39F: {void*_tmp771=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_TupleIndex_struct
_tmpE0B;struct Cyc_Absyn_TupleIndex_struct*_tmpE0A;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp(_tmp771,(void*)((_tmpE0A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE0A)),((_tmpE0A[0]=((_tmpE0B.tag=1,((_tmpE0B.f1=(unsigned int)(Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,_tmpE0B)))),
_tmpE0A)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;}case 371: _LL3A0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 372: _LL3A1: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 373: _LL3A2:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 374:
_LL3A3: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 375: _LL3A4: {
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);{void*_tmp774=e->r;union Cyc_Absyn_Cnst _tmp776;struct _tuple4
_tmp777;enum Cyc_Absyn_Sign _tmp778;char _tmp779;union Cyc_Absyn_Cnst _tmp77B;struct
_tuple5 _tmp77C;enum Cyc_Absyn_Sign _tmp77D;short _tmp77E;union Cyc_Absyn_Cnst
_tmp780;struct _tuple6 _tmp781;enum Cyc_Absyn_Sign _tmp782;int _tmp783;union Cyc_Absyn_Cnst
_tmp785;struct _dyneither_ptr _tmp786;union Cyc_Absyn_Cnst _tmp788;int _tmp789;union
Cyc_Absyn_Cnst _tmp78B;struct _dyneither_ptr _tmp78C;union Cyc_Absyn_Cnst _tmp78E;
struct _tuple7 _tmp78F;_LL3A7: {struct Cyc_Absyn_Const_e_struct*_tmp775=(struct Cyc_Absyn_Const_e_struct*)
_tmp774;if(_tmp775->tag != 0)goto _LL3A9;else{_tmp776=_tmp775->f1;if((_tmp776.Char_c).tag
!= 2)goto _LL3A9;_tmp777=(struct _tuple4)(_tmp776.Char_c).val;_tmp778=_tmp777.f1;
_tmp779=_tmp777.f2;}}_LL3A8:{struct Cyc_Absyn_Char_p_struct _tmpE0E;struct Cyc_Absyn_Char_p_struct*
_tmpE0D;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE0D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE0D)),((_tmpE0D[0]=((_tmpE0E.tag=10,((_tmpE0E.f1=_tmp779,_tmpE0E)))),
_tmpE0D)))),e->loc));}goto _LL3A6;_LL3A9: {struct Cyc_Absyn_Const_e_struct*_tmp77A=(
struct Cyc_Absyn_Const_e_struct*)_tmp774;if(_tmp77A->tag != 0)goto _LL3AB;else{
_tmp77B=_tmp77A->f1;if((_tmp77B.Short_c).tag != 3)goto _LL3AB;_tmp77C=(struct
_tuple5)(_tmp77B.Short_c).val;_tmp77D=_tmp77C.f1;_tmp77E=_tmp77C.f2;}}_LL3AA:{
struct Cyc_Absyn_Int_p_struct _tmpE11;struct Cyc_Absyn_Int_p_struct*_tmpE10;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE10=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE10)),((_tmpE10[0]=((_tmpE11.tag=9,((_tmpE11.f1=_tmp77D,((_tmpE11.f2=(
int)_tmp77E,_tmpE11)))))),_tmpE10)))),e->loc));}goto _LL3A6;_LL3AB: {struct Cyc_Absyn_Const_e_struct*
_tmp77F=(struct Cyc_Absyn_Const_e_struct*)_tmp774;if(_tmp77F->tag != 0)goto _LL3AD;
else{_tmp780=_tmp77F->f1;if((_tmp780.Int_c).tag != 4)goto _LL3AD;_tmp781=(struct
_tuple6)(_tmp780.Int_c).val;_tmp782=_tmp781.f1;_tmp783=_tmp781.f2;}}_LL3AC:{
struct Cyc_Absyn_Int_p_struct _tmpE14;struct Cyc_Absyn_Int_p_struct*_tmpE13;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE13=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE13)),((_tmpE13[0]=((_tmpE14.tag=9,((_tmpE14.f1=_tmp782,((_tmpE14.f2=
_tmp783,_tmpE14)))))),_tmpE13)))),e->loc));}goto _LL3A6;_LL3AD: {struct Cyc_Absyn_Const_e_struct*
_tmp784=(struct Cyc_Absyn_Const_e_struct*)_tmp774;if(_tmp784->tag != 0)goto _LL3AF;
else{_tmp785=_tmp784->f1;if((_tmp785.Float_c).tag != 6)goto _LL3AF;_tmp786=(struct
_dyneither_ptr)(_tmp785.Float_c).val;}}_LL3AE:{struct Cyc_Absyn_Float_p_struct
_tmpE17;struct Cyc_Absyn_Float_p_struct*_tmpE16;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE16=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE16)),((
_tmpE16[0]=((_tmpE17.tag=11,((_tmpE17.f1=_tmp786,_tmpE17)))),_tmpE16)))),e->loc));}
goto _LL3A6;_LL3AF: {struct Cyc_Absyn_Const_e_struct*_tmp787=(struct Cyc_Absyn_Const_e_struct*)
_tmp774;if(_tmp787->tag != 0)goto _LL3B1;else{_tmp788=_tmp787->f1;if((_tmp788.Null_c).tag
!= 1)goto _LL3B1;_tmp789=(int)(_tmp788.Null_c).val;}}_LL3B0: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A6;_LL3B1: {struct Cyc_Absyn_Const_e_struct*
_tmp78A=(struct Cyc_Absyn_Const_e_struct*)_tmp774;if(_tmp78A->tag != 0)goto _LL3B3;
else{_tmp78B=_tmp78A->f1;if((_tmp78B.String_c).tag != 7)goto _LL3B3;_tmp78C=(
struct _dyneither_ptr)(_tmp78B.String_c).val;}}_LL3B2:{const char*_tmpE18;Cyc_Parse_err(((
_tmpE18="strings cannot occur within patterns",_tag_dyneither(_tmpE18,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL3A6;_LL3B3: {struct Cyc_Absyn_Const_e_struct*
_tmp78D=(struct Cyc_Absyn_Const_e_struct*)_tmp774;if(_tmp78D->tag != 0)goto _LL3B5;
else{_tmp78E=_tmp78D->f1;if((_tmp78E.LongLong_c).tag != 5)goto _LL3B5;_tmp78F=(
struct _tuple7)(_tmp78E.LongLong_c).val;}}_LL3B4:{const char*_tmpE19;Cyc_Parse_unimp(((
_tmpE19="long long's in patterns",_tag_dyneither(_tmpE19,sizeof(char),24))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}goto _LL3A6;
_LL3B5:;_LL3B6: {const char*_tmpE1A;Cyc_Parse_err(((_tmpE1A="bad constant in case",
_tag_dyneither(_tmpE1A,sizeof(char),21))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}_LL3A6:;}
break;}case 376: _LL3A5:{struct Cyc_Absyn_UnknownId_p_struct _tmpE1D;struct Cyc_Absyn_UnknownId_p_struct*
_tmpE1C;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE1C=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE1C)),((_tmpE1C[0]=((_tmpE1D.tag=14,((_tmpE1D.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE1D)))),_tmpE1C)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 377: _LL3B7:{const char*_tmpE1E;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpE1E="as",_tag_dyneither(_tmpE1E,sizeof(char),3))))
!= 0){const char*_tmpE1F;Cyc_Parse_err(((_tmpE1F="expecting `as'",_tag_dyneither(
_tmpE1F,sizeof(char),15))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Var_p_struct
_tmpE29;struct _dyneither_ptr*_tmpE28;struct _tuple1*_tmpE27;struct Cyc_Absyn_Var_p_struct*
_tmpE26;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE26=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE26)),((_tmpE26[0]=((_tmpE29.tag=1,((_tmpE29.f1=Cyc_Absyn_new_vardecl(((
_tmpE27=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE27)),((_tmpE27->f1=
Cyc_Absyn_Loc_n,((_tmpE27->f2=((_tmpE28=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE28)),((_tmpE28[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE28)))),_tmpE27)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE29.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE29)))))),_tmpE26)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 378: _LL3B8: {struct Cyc_List_List*
_tmp7A4;int _tmp7A5;struct _tuple19 _tmp7A3=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A4=_tmp7A3.f1;
_tmp7A5=_tmp7A3.f2;{struct Cyc_Absyn_Tuple_p_struct _tmpE2C;struct Cyc_Absyn_Tuple_p_struct*
_tmpE2B;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE2B=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE2B)),((_tmpE2B[0]=((_tmpE2C.tag=4,((_tmpE2C.f1=_tmp7A4,((_tmpE2C.f2=
_tmp7A5,_tmpE2C)))))),_tmpE2B)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
379: _LL3B9: {struct Cyc_List_List*_tmp7A9;int _tmp7AA;struct _tuple19 _tmp7A8=*Cyc_yyget_YY12(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A9=_tmp7A8.f1;
_tmp7AA=_tmp7A8.f2;{struct Cyc_Absyn_UnknownCall_p_struct _tmpE2F;struct Cyc_Absyn_UnknownCall_p_struct*
_tmpE2E;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE2E=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE2E)),((_tmpE2E[0]=((_tmpE2F.tag=15,((_tmpE2F.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpE2F.f2=
_tmp7A9,((_tmpE2F.f3=_tmp7AA,_tmpE2F)))))))),_tmpE2E)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
380: _LL3BA: {struct Cyc_List_List*_tmp7AE;int _tmp7AF;struct _tuple19 _tmp7AD=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7AE=_tmp7AD.f1;
_tmp7AF=_tmp7AD.f2;{struct Cyc_List_List*_tmp7B0=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpE35;struct Cyc_Absyn_AggrInfo*_tmpE34;struct Cyc_Absyn_Aggr_p_struct*_tmpE33;
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE33=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE33)),((_tmpE33[0]=((_tmpE35.tag=6,((_tmpE35.f1=((_tmpE34=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE34)),((_tmpE34->aggr_info=Cyc_Absyn_UnknownAggr(
Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),0),((_tmpE34->targs=0,_tmpE34)))))),((_tmpE35.f2=_tmp7B0,((
_tmpE35.f3=_tmp7AE,((_tmpE35.f4=_tmp7AF,_tmpE35)))))))))),_tmpE33)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;};}case
381: _LL3BB: {struct Cyc_List_List*_tmp7B5;int _tmp7B6;struct _tuple19 _tmp7B4=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7B5=_tmp7B4.f1;
_tmp7B6=_tmp7B4.f2;{struct Cyc_List_List*_tmp7B7=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpE38;struct Cyc_Absyn_Aggr_p_struct*_tmpE37;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE37=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE37)),((
_tmpE37[0]=((_tmpE38.tag=6,((_tmpE38.f1=0,((_tmpE38.f2=_tmp7B7,((_tmpE38.f3=
_tmp7B5,((_tmpE38.f4=_tmp7B6,_tmpE38)))))))))),_tmpE37)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;};}case
382: _LL3BC:{struct Cyc_Absyn_Pointer_p_struct _tmpE3B;struct Cyc_Absyn_Pointer_p_struct*
_tmpE3A;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE3A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE3A)),((_tmpE3A[0]=((_tmpE3B.tag=5,((_tmpE3B.f1=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE3B)))),_tmpE3A)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 383:
_LL3BD:{struct Cyc_Absyn_Pointer_p_struct _tmpE45;struct Cyc_Absyn_Pointer_p_struct
_tmpE44;struct Cyc_Absyn_Pointer_p_struct*_tmpE43;struct Cyc_Absyn_Pointer_p_struct*
_tmpE42;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE42=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE42)),((_tmpE42[0]=((_tmpE45.tag=5,((_tmpE45.f1=Cyc_Absyn_new_pat((
void*)((_tmpE43=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE43)),((
_tmpE43[0]=((_tmpE44.tag=5,((_tmpE44.f1=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE44)))),_tmpE43)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpE45)))),
_tmpE42)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 384: _LL3BE:{struct Cyc_Absyn_Reference_p_struct
_tmpE4F;struct _dyneither_ptr*_tmpE4E;struct _tuple1*_tmpE4D;struct Cyc_Absyn_Reference_p_struct*
_tmpE4C;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE4C=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE4C)),((_tmpE4C[0]=((_tmpE4F.tag=2,((_tmpE4F.f1=Cyc_Absyn_new_vardecl(((
_tmpE4D=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE4D)),((_tmpE4D->f1=
Cyc_Absyn_Loc_n,((_tmpE4D->f2=((_tmpE4E=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE4E)),((_tmpE4E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE4E)))),_tmpE4D)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE4F.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpE4F)))))),_tmpE4C)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 385:
_LL3BF:{const char*_tmpE50;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE50="as",
_tag_dyneither(_tmpE50,sizeof(char),3))))!= 0){const char*_tmpE51;Cyc_Parse_err(((
_tmpE51="expecting `as'",_tag_dyneither(_tmpE51,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Reference_p_struct
_tmpE5B;struct _dyneither_ptr*_tmpE5A;struct _tuple1*_tmpE59;struct Cyc_Absyn_Reference_p_struct*
_tmpE58;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE58=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE58)),((_tmpE58[0]=((_tmpE5B.tag=2,((_tmpE5B.f1=Cyc_Absyn_new_vardecl(((
_tmpE59=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE59)),((_tmpE59->f1=
Cyc_Absyn_Loc_n,((_tmpE59->f2=((_tmpE5A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE5A)),((_tmpE5A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE5A)))),_tmpE59)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE5B.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE5B)))))),_tmpE58)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 386: _LL3C0: {void*_tmp7CA=Cyc_Parse_id2type(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct
_tmpE6C;struct Cyc_Absyn_TagType_struct _tmpE6B;struct Cyc_Absyn_TagType_struct*
_tmpE6A;struct _dyneither_ptr*_tmpE69;struct _tuple1*_tmpE68;struct Cyc_Absyn_TagInt_p_struct*
_tmpE67;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE67=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE67)),((_tmpE67[0]=((_tmpE6C.tag=3,((_tmpE6C.f1=Cyc_Parse_typ2tvar(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp7CA),((_tmpE6C.f2=Cyc_Absyn_new_vardecl(((
_tmpE68=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE68)),((_tmpE68->f1=
Cyc_Absyn_Loc_n,((_tmpE68->f2=((_tmpE69=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE69)),((_tmpE69[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpE69)))),_tmpE68)))))),(
void*)((_tmpE6A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE6A)),((
_tmpE6A[0]=((_tmpE6B.tag=20,((_tmpE6B.f1=(void*)_tmp7CA,_tmpE6B)))),_tmpE6A)))),
0),_tmpE6C)))))),_tmpE67)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
387: _LL3C1: {struct Cyc_Absyn_Tvar*_tmp7D1=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(&
Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct _tmpE88;struct Cyc_Absyn_TagType_struct
_tmpE87;struct Cyc_Absyn_VarType_struct _tmpE86;struct Cyc_Absyn_VarType_struct*
_tmpE85;struct Cyc_Absyn_TagType_struct*_tmpE84;struct _dyneither_ptr*_tmpE83;
struct _tuple1*_tmpE82;struct Cyc_Absyn_TagInt_p_struct*_tmpE81;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE81=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE81)),((
_tmpE81[0]=((_tmpE88.tag=3,((_tmpE88.f1=_tmp7D1,((_tmpE88.f2=Cyc_Absyn_new_vardecl(((
_tmpE82=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE82)),((_tmpE82->f1=
Cyc_Absyn_Loc_n,((_tmpE82->f2=((_tmpE83=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE83)),((_tmpE83[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpE83)))),_tmpE82)))))),(
void*)((_tmpE84=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE84)),((
_tmpE84[0]=((_tmpE87.tag=20,((_tmpE87.f1=(void*)((void*)((_tmpE85=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpE85)),((_tmpE85[0]=((_tmpE86.tag=2,((_tmpE86.f1=
_tmp7D1,_tmpE86)))),_tmpE85))))),_tmpE87)))),_tmpE84)))),0),_tmpE88)))))),
_tmpE81)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 388: _LL3C2:{struct _tuple19*_tmpE89;
yyval=Cyc_YY12(((_tmpE89=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE89)),((
_tmpE89->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((_tmpE89->f2=0,
_tmpE89)))))));}break;case 389: _LL3C3:{struct _tuple19*_tmpE8A;yyval=Cyc_YY12(((
_tmpE8A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE8A)),((_tmpE8A->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpE8A->f2=1,_tmpE8A)))))));}
break;case 390: _LL3C4:{struct _tuple19*_tmpE8B;yyval=Cyc_YY12(((_tmpE8B=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE8B)),((_tmpE8B->f1=0,((
_tmpE8B->f2=1,_tmpE8B)))))));}break;case 391: _LL3C5:{struct Cyc_List_List*_tmpE8C;
yyval=Cyc_YY13(((_tmpE8C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE8C)),((
_tmpE8C->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpE8C->tl=0,_tmpE8C)))))));}break;case 392: _LL3C6:{struct Cyc_List_List*
_tmpE8D;yyval=Cyc_YY13(((_tmpE8D=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpE8D)),((_tmpE8D->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpE8D->tl=Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpE8D)))))));}break;case 393: _LL3C7:{struct _tuple20*
_tmpE8E;yyval=Cyc_YY14(((_tmpE8E=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpE8E)),((_tmpE8E->f1=0,((_tmpE8E->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE8E)))))));}break;case
394: _LL3C8:{struct _tuple20*_tmpE8F;yyval=Cyc_YY14(((_tmpE8F=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE8F)),((_tmpE8F->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE8F->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE8F)))))));}break;
case 395: _LL3C9:{struct _tuple19*_tmpE90;yyval=Cyc_YY16(((_tmpE90=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpE90)),((_tmpE90->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),((_tmpE90->f2=0,_tmpE90)))))));}
break;case 396: _LL3CA:{struct _tuple19*_tmpE91;yyval=Cyc_YY16(((_tmpE91=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE91)),((_tmpE91->f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpE91->f2=1,_tmpE91)))))));}
break;case 397: _LL3CB:{struct _tuple19*_tmpE92;yyval=Cyc_YY16(((_tmpE92=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE92)),((_tmpE92->f1=0,((
_tmpE92->f2=1,_tmpE92)))))));}break;case 398: _LL3CC:{struct Cyc_List_List*_tmpE93;
yyval=Cyc_YY15(((_tmpE93=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE93)),((
_tmpE93->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpE93->tl=0,_tmpE93)))))));}break;case 399: _LL3CD:{struct Cyc_List_List*
_tmpE94;yyval=Cyc_YY15(((_tmpE94=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpE94)),((_tmpE94->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpE94->tl=Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpE94)))))));}break;case 400: _LL3CE: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 401: _LL3CF: yyval=
Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 402: _LL3D0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 403: _LL3D1: yyval=
Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 404: _LL3D2: yyval=Cyc_YY3(Cyc_Absyn_swap_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 405:
_LL3D3: yyval=Cyc_YY8(0);break;case 406: _LL3D4:{struct Cyc_Core_Opt*_tmpE95;yyval=
Cyc_YY8(((_tmpE95=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE95)),((
_tmpE95->v=(void*)Cyc_Absyn_Times,_tmpE95)))));}break;case 407: _LL3D5:{struct Cyc_Core_Opt*
_tmpE96;yyval=Cyc_YY8(((_tmpE96=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpE96)),((_tmpE96->v=(void*)Cyc_Absyn_Div,_tmpE96)))));}break;case 408: _LL3D6:{
struct Cyc_Core_Opt*_tmpE97;yyval=Cyc_YY8(((_tmpE97=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE97)),((_tmpE97->v=(void*)Cyc_Absyn_Mod,_tmpE97)))));}break;case 409:
_LL3D7:{struct Cyc_Core_Opt*_tmpE98;yyval=Cyc_YY8(((_tmpE98=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE98)),((_tmpE98->v=(void*)Cyc_Absyn_Plus,_tmpE98)))));}break;case 410:
_LL3D8:{struct Cyc_Core_Opt*_tmpE99;yyval=Cyc_YY8(((_tmpE99=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE99)),((_tmpE99->v=(void*)Cyc_Absyn_Minus,_tmpE99)))));}break;case
411: _LL3D9:{struct Cyc_Core_Opt*_tmpE9A;yyval=Cyc_YY8(((_tmpE9A=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpE9A)),((_tmpE9A->v=(void*)Cyc_Absyn_Bitlshift,
_tmpE9A)))));}break;case 412: _LL3DA:{struct Cyc_Core_Opt*_tmpE9B;yyval=Cyc_YY8(((
_tmpE9B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE9B)),((_tmpE9B->v=(
void*)Cyc_Absyn_Bitlrshift,_tmpE9B)))));}break;case 413: _LL3DB:{struct Cyc_Core_Opt*
_tmpE9C;yyval=Cyc_YY8(((_tmpE9C=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmpE9C)),((_tmpE9C->v=(void*)Cyc_Absyn_Bitand,_tmpE9C)))));}break;case 414:
_LL3DC:{struct Cyc_Core_Opt*_tmpE9D;yyval=Cyc_YY8(((_tmpE9D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpE9D)),((_tmpE9D->v=(void*)Cyc_Absyn_Bitxor,_tmpE9D)))));}break;case
415: _LL3DD:{struct Cyc_Core_Opt*_tmpE9E;yyval=Cyc_YY8(((_tmpE9E=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmpE9E)),((_tmpE9E->v=(void*)Cyc_Absyn_Bitor,
_tmpE9E)))));}break;case 416: _LL3DE: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 417: _LL3DF: yyval=Cyc_YY3(Cyc_Absyn_conditional_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 418:
_LL3E0: yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 419:
_LL3E1: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 420:
_LL3E2: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 421:
_LL3E3: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 422:
_LL3E4: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 423:
_LL3E5: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
424: _LL3E6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 425: _LL3E7: yyval=Cyc_YY3(Cyc_Absyn_or_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 426:
_LL3E8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
427: _LL3E9: yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 428:
_LL3EA: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
429: _LL3EB: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 430:
_LL3EC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
431: _LL3ED: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 432:
_LL3EE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
433: _LL3EF: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 434:
_LL3F0: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
435: _LL3F1: yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 436:
_LL3F2: yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 437:
_LL3F3: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
438: _LL3F4: yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 439:
_LL3F5: yyval=Cyc_YY3(Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 440:
_LL3F6: yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 441:
_LL3F7: yyval=Cyc_YY3(Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 442:
_LL3F8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
443: _LL3F9: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 444:
_LL3FA: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 445:
_LL3FB: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
446: _LL3FC: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 447:
_LL3FD: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 448:
_LL3FE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
449: _LL3FF: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 450:
_LL400: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 451:
_LL401: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 452:
_LL402: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
453: _LL403: {void*_tmp7F0=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));yyval=Cyc_YY3(
Cyc_Absyn_cast_exp(_tmp7F0,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 454:
_LL404: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
455: _LL405: yyval=Cyc_YY3(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 456:
_LL406: yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 457:
_LL407: yyval=Cyc_YY3(Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 458:
_LL408: yyval=Cyc_YY3(Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 459:
_LL409: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
460: _LL40A: yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 461:
_LL40B: {void*_tmp7F1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY3(
Cyc_Absyn_sizeoftyp_exp(_tmp7F1,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 462:
_LL40C: yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 463:
_LL40D: {void*_tmp7F2=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_StructField_struct
_tmpEA4;struct _dyneither_ptr*_tmpEA3;struct Cyc_Absyn_StructField_struct*_tmpEA2;
yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(_tmp7F2,(void*)((_tmpEA2=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpEA2)),((_tmpEA2[0]=((_tmpEA4.tag=0,((_tmpEA4.f1=((_tmpEA3=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpEA3)),((_tmpEA3[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpEA3)))),_tmpEA4)))),
_tmpEA2)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 464: _LL40E: {void*_tmp7F6=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_TupleIndex_struct
_tmpEA7;struct Cyc_Absyn_TupleIndex_struct*_tmpEA6;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(
_tmp7F6,(void*)((_tmpEA6=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpEA6)),((
_tmpEA6[0]=((_tmpEA7.tag=1,((_tmpEA7.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,_tmpEA7)))),_tmpEA6)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 465: _LL40F:{struct Cyc_Absyn_Malloc_e_struct
_tmpEAD;struct Cyc_Absyn_MallocInfo _tmpEAC;struct Cyc_Absyn_Malloc_e_struct*
_tmpEAB;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEAB=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpEAB)),((_tmpEAB[0]=((_tmpEAD.tag=34,((_tmpEAD.f1=((_tmpEAC.is_calloc=
0,((_tmpEAC.rgn=0,((_tmpEAC.elt_type=0,((_tmpEAC.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpEAC.fat_result=0,
_tmpEAC)))))))))),_tmpEAD)))),_tmpEAB)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 466:
_LL410:{struct Cyc_Absyn_Malloc_e_struct _tmpEB3;struct Cyc_Absyn_MallocInfo _tmpEB2;
struct Cyc_Absyn_Malloc_e_struct*_tmpEB1;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpEB1=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpEB1)),((_tmpEB1[0]=((
_tmpEB3.tag=34,((_tmpEB3.f1=((_tmpEB2.is_calloc=0,((_tmpEB2.rgn=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpEB2.elt_type=0,((_tmpEB2.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpEB2.fat_result=0,
_tmpEB2)))))))))),_tmpEB3)))),_tmpEB1)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 467:
_LL411: {void*_tmp7FF=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));{struct Cyc_Absyn_Malloc_e_struct
_tmpEBD;void**_tmpEBC;struct Cyc_Absyn_MallocInfo _tmpEBB;struct Cyc_Absyn_Malloc_e_struct*
_tmpEBA;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEBA=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpEBA)),((_tmpEBA[0]=((_tmpEBD.tag=34,((_tmpEBD.f1=((_tmpEBB.is_calloc=
1,((_tmpEBB.rgn=0,((_tmpEBB.elt_type=((_tmpEBC=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpEBC)),((_tmpEBC[0]=_tmp7FF,_tmpEBC)))),((_tmpEBB.num_elts=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((_tmpEBB.fat_result=
0,_tmpEBB)))))))))),_tmpEBD)))),_tmpEBA)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
468: _LL412: {void*_tmp804=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));{struct Cyc_Absyn_Malloc_e_struct
_tmpEC7;void**_tmpEC6;struct Cyc_Absyn_MallocInfo _tmpEC5;struct Cyc_Absyn_Malloc_e_struct*
_tmpEC4;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEC4=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpEC4)),((_tmpEC4[0]=((_tmpEC7.tag=34,((_tmpEC7.f1=((_tmpEC5.is_calloc=
1,((_tmpEC5.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),((_tmpEC5.elt_type=((
_tmpEC6=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpEC6)),((_tmpEC6[0]=
_tmp804,_tmpEC6)))),((_tmpEC5.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((_tmpEC5.fat_result=0,
_tmpEC5)))))))))),_tmpEC7)))),_tmpEC4)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
469: _LL413:{struct Cyc_Absyn_Exp*_tmpEC8[1];yyval=Cyc_YY3(Cyc_Absyn_primop_exp(
Cyc_Absyn_Numelts,((_tmpEC8[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpEC8,sizeof(struct Cyc_Absyn_Exp*),1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 470:
_LL414:{struct Cyc_Absyn_Tagcheck_e_struct _tmpECE;struct _dyneither_ptr*_tmpECD;
struct Cyc_Absyn_Tagcheck_e_struct*_tmpECC;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpECC=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpECC)),((_tmpECC[0]=((
_tmpECE.tag=38,((_tmpECE.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmpECE.f2=((_tmpECD=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpECD)),((_tmpECD[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpECD)))),_tmpECE)))))),
_tmpECC)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 471: _LL415:{struct Cyc_Absyn_Tagcheck_e_struct
_tmpED4;struct _dyneither_ptr*_tmpED3;struct Cyc_Absyn_Tagcheck_e_struct*_tmpED2;
yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpED2=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpED2)),((_tmpED2[0]=((_tmpED4.tag=38,((_tmpED4.f1=Cyc_Absyn_deref_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)),((_tmpED4.f2=((
_tmpED3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpED3)),((_tmpED3[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpED3)))),_tmpED4)))))),
_tmpED2)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 472: _LL416: {void*_tmp810=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY3(
Cyc_Absyn_valueof_exp(_tmp810,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 473:
_LL417: {int _tmp812;struct _dyneither_ptr _tmp813;struct _tuple17 _tmp811=Cyc_yyget_Asm_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp812=_tmp811.f1;
_tmp813=_tmp811.f2;yyval=Cyc_YY3(Cyc_Absyn_asm_exp(_tmp812,_tmp813,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 474:
_LL418: yyval=Cyc_YY7(Cyc_Absyn_Bitnot);break;case 475: _LL419: yyval=Cyc_YY7(Cyc_Absyn_Not);
break;case 476: _LL41A: yyval=Cyc_YY7(Cyc_Absyn_Minus);break;case 477: _LL41B: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 478: _LL41C:
yyval=Cyc_YY3(Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 479:
_LL41D: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 480:
_LL41E: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 481:
_LL41F:{struct _dyneither_ptr*_tmpED5;yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpED5=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpED5)),((_tmpED5[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpED5)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 482:
_LL420:{struct _dyneither_ptr*_tmpED6;yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpED6=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpED6)),((_tmpED6[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpED6)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 483:
_LL421: yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 484:
_LL422: yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 485:
_LL423:{struct Cyc_Absyn_CompoundLit_e_struct _tmpED9;struct Cyc_Absyn_CompoundLit_e_struct*
_tmpED8;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpED8=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpED8)),((_tmpED8[0]=((_tmpED9.tag=26,((_tmpED9.f1=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpED9.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmpED9)))))),_tmpED8)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 486: _LL424:{struct Cyc_Absyn_CompoundLit_e_struct
_tmpEDC;struct Cyc_Absyn_CompoundLit_e_struct*_tmpEDB;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpEDB=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpEDB)),((
_tmpEDB[0]=((_tmpEDC.tag=26,((_tmpEDC.f1=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((_tmpEDC.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),_tmpEDC)))))),_tmpEDB)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 487: _LL425:{struct Cyc_Absyn_UnknownId_e_struct
_tmpEDF;struct Cyc_Absyn_UnknownId_e_struct*_tmpEDE;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpEDE=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpEDE)),((
_tmpEDE[0]=((_tmpEDF.tag=2,((_tmpEDF.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEDF)))),_tmpEDE)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 488:
_LL426: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
489: _LL427: yyval=Cyc_YY3(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 490:
_LL428: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 491: _LL429: yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 492:
_LL42A: yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 493:
_LL42B: yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 494:
_LL42C:{struct Cyc_Absyn_Aggregate_e_struct _tmpEE2;struct Cyc_Absyn_Aggregate_e_struct*
_tmpEE1;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEE1=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpEE1)),((_tmpEE1[0]=((_tmpEE2.tag=29,((_tmpEE2.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpEE2.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpEE2.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmpEE2.f4=0,
_tmpEE2)))))))))),_tmpEE1)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 495:
_LL42D: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 496:
_LL42E: yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));break;
case 497: _LL42F:{struct Cyc_List_List*_tmpEE3;yyval=Cyc_YY5(((_tmpEE3=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpEE3)),((_tmpEE3->hd=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEE3->tl=0,
_tmpEE3)))))));}break;case 498: _LL430:{struct Cyc_List_List*_tmpEE4;yyval=Cyc_YY5(((
_tmpEE4=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpEE4)),((_tmpEE4->hd=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpEE4->tl=Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),_tmpEE4)))))));}break;case 499: _LL431: yyval=Cyc_YY3(Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 500:
_LL432: yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 501:
_LL433: yyval=Cyc_YY3(Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 502:
_LL434: yyval=Cyc_YY3(Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 503:
_LL435:{struct _dyneither_ptr*_tmpEE7;struct _tuple1*_tmpEE6;yyval=Cyc_QualId_tok(((
_tmpEE6=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpEE6)),((_tmpEE6->f1=
Cyc_Absyn_Rel_n(0),((_tmpEE6->f2=((_tmpEE7=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpEE7)),((_tmpEE7[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEE7)))),_tmpEE6)))))));}
break;case 504: _LL436: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 505: _LL437:{struct _dyneither_ptr*_tmpEEA;struct _tuple1*
_tmpEE9;yyval=Cyc_QualId_tok(((_tmpEE9=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmpEE9)),((_tmpEE9->f1=Cyc_Absyn_Rel_n(0),((_tmpEE9->f2=((_tmpEEA=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpEEA)),((_tmpEEA[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEEA)))),_tmpEE9)))))));}
break;case 506: _LL438: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 507: _LL439: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 508: _LL43A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 509: _LL43B: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 510: _LL43C:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 511:
_LL43D: break;case 512: _LL43E: yylex_buf->lex_curr_pos -=1;break;default: _LL43F:
break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;++ yylsp_offset;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(513,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(143,yyn - 144)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6244) && Cyc_yycheck[_check_known_subscript_notnull(6245,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6245,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(143,yyn - 144)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1028,yystate)];if(yyn > - 32768  && yyn < 6244){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 287 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6245,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(287,x)])+ 15,count ++);}{unsigned int _tmpEEF;
unsigned int _tmpEEE;struct _dyneither_ptr _tmpEED;char*_tmpEEC;unsigned int _tmpEEB;
msg=((_tmpEEB=(unsigned int)(sze + 15),((_tmpEEC=(char*)_region_malloc(yyregion,
_check_times(sizeof(char),_tmpEEB + 1)),((_tmpEED=_tag_dyneither(_tmpEEC,sizeof(
char),_tmpEEB + 1),((((_tmpEEE=_tmpEEB,((_tmpEF0(& _tmpEEF,& _tmpEEE,& _tmpEEC),
_tmpEEC[_tmpEEE]=(char)0)))),_tmpEED))))))));}{const char*_tmpEF1;Cyc_strcpy(msg,((
_tmpEF1="parse error",_tag_dyneither(_tmpEF1,sizeof(char),12))));}if(count < 5){
count=0;for(x=yyn < 0?- yyn: 0;x < 287 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6245,x + yyn)]== x){{const char*_tmpEF3;const char*
_tmpEF2;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)((
_tmpEF3=", expecting `",_tag_dyneither(_tmpEF3,sizeof(char),14))):(struct
_dyneither_ptr)((_tmpEF2=" or `",_tag_dyneither(_tmpEF2,sizeof(char),6)))));}Cyc_strcat(
msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(287,x)]);{
const char*_tmpEF4;Cyc_strcat(msg,((_tmpEF4="'",_tag_dyneither(_tmpEF4,sizeof(
char),2))));}++ count;}}}Cyc_yyerror((struct _dyneither_ptr)msg);}else{const char*
_tmpEF5;Cyc_yyerror(((_tmpEF5="parse error",_tag_dyneither(_tmpEF5,sizeof(char),
12))));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int
_tmp82D=1;_npop_handler(0);return _tmp82D;}Cyc_yychar=- 2;}yyerrstatus=3;goto
yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp82E=1;
_npop_handler(0);return _tmp82E;}-- yyvsp_offset;yystate=(int)yyss[
_check_known_subscript_notnull(10000,-- yyssp_offset)];-- yylsp_offset;yyerrhandle:
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1028,yystate)];if(yyn == -
32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6244) || Cyc_yycheck[
_check_known_subscript_notnull(6245,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6245,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 1027){int
_tmp82F=0;_npop_handler(0);return _tmp82F;}yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE _tmp836=v;struct _tuple6 _tmp837;int
_tmp838;char _tmp839;struct _dyneither_ptr _tmp83A;struct _tuple1*_tmp83B;struct
_tuple1 _tmp83C;union Cyc_Absyn_Nmspace _tmp83D;struct _dyneither_ptr*_tmp83E;_LL442:
if((_tmp836.Int_tok).tag != 1)goto _LL444;_tmp837=(struct _tuple6)(_tmp836.Int_tok).val;
_tmp838=_tmp837.f2;_LL443:{const char*_tmpEF9;void*_tmpEF8[1];struct Cyc_Int_pa_struct
_tmpEF7;(_tmpEF7.tag=1,((_tmpEF7.f1=(unsigned long)_tmp838,((_tmpEF8[0]=& _tmpEF7,
Cyc_fprintf(Cyc_stderr,((_tmpEF9="%d",_tag_dyneither(_tmpEF9,sizeof(char),3))),
_tag_dyneither(_tmpEF8,sizeof(void*),1)))))));}goto _LL441;_LL444: if((_tmp836.Char_tok).tag
!= 2)goto _LL446;_tmp839=(char)(_tmp836.Char_tok).val;_LL445:{const char*_tmpEFD;
void*_tmpEFC[1];struct Cyc_Int_pa_struct _tmpEFB;(_tmpEFB.tag=1,((_tmpEFB.f1=(
unsigned long)((int)_tmp839),((_tmpEFC[0]=& _tmpEFB,Cyc_fprintf(Cyc_stderr,((
_tmpEFD="%c",_tag_dyneither(_tmpEFD,sizeof(char),3))),_tag_dyneither(_tmpEFC,
sizeof(void*),1)))))));}goto _LL441;_LL446: if((_tmp836.String_tok).tag != 3)goto
_LL448;_tmp83A=(struct _dyneither_ptr)(_tmp836.String_tok).val;_LL447:{const char*
_tmpF01;void*_tmpF00[1];struct Cyc_String_pa_struct _tmpEFF;(_tmpEFF.tag=0,((
_tmpEFF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp83A),((_tmpF00[0]=&
_tmpEFF,Cyc_fprintf(Cyc_stderr,((_tmpF01="\"%s\"",_tag_dyneither(_tmpF01,sizeof(
char),5))),_tag_dyneither(_tmpF00,sizeof(void*),1)))))));}goto _LL441;_LL448: if((
_tmp836.QualId_tok).tag != 5)goto _LL44A;_tmp83B=(struct _tuple1*)(_tmp836.QualId_tok).val;
_tmp83C=*_tmp83B;_tmp83D=_tmp83C.f1;_tmp83E=_tmp83C.f2;_LL449: {struct Cyc_List_List*
_tmp848=0;{union Cyc_Absyn_Nmspace _tmp849=_tmp83D;struct Cyc_List_List*_tmp84A;
struct Cyc_List_List*_tmp84B;struct Cyc_List_List*_tmp84C;int _tmp84D;_LL44D: if((
_tmp849.Rel_n).tag != 1)goto _LL44F;_tmp84A=(struct Cyc_List_List*)(_tmp849.Rel_n).val;
_LL44E: _tmp848=_tmp84A;goto _LL44C;_LL44F: if((_tmp849.Abs_n).tag != 2)goto _LL451;
_tmp84B=(struct Cyc_List_List*)(_tmp849.Abs_n).val;_LL450: _tmp848=_tmp84B;goto
_LL44C;_LL451: if((_tmp849.C_n).tag != 3)goto _LL453;_tmp84C=(struct Cyc_List_List*)(
_tmp849.C_n).val;_LL452: _tmp848=_tmp84C;goto _LL44C;_LL453: if((_tmp849.Loc_n).tag
!= 4)goto _LL44C;_tmp84D=(int)(_tmp849.Loc_n).val;_LL454: goto _LL44C;_LL44C:;}for(
0;_tmp848 != 0;_tmp848=_tmp848->tl){const char*_tmpF05;void*_tmpF04[1];struct Cyc_String_pa_struct
_tmpF03;(_tmpF03.tag=0,((_tmpF03.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp848->hd)),((_tmpF04[0]=& _tmpF03,Cyc_fprintf(Cyc_stderr,((
_tmpF05="%s::",_tag_dyneither(_tmpF05,sizeof(char),5))),_tag_dyneither(_tmpF04,
sizeof(void*),1)))))));}{const char*_tmpF09;void*_tmpF08[1];struct Cyc_String_pa_struct
_tmpF07;(_tmpF07.tag=0,((_tmpF07.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp83E),((_tmpF08[0]=& _tmpF07,Cyc_fprintf(Cyc_stderr,((_tmpF09="%s::",
_tag_dyneither(_tmpF09,sizeof(char),5))),_tag_dyneither(_tmpF08,sizeof(void*),1)))))));}
goto _LL441;}_LL44A:;_LL44B:{const char*_tmpF0C;void*_tmpF0B;(_tmpF0B=0,Cyc_fprintf(
Cyc_stderr,((_tmpF0C="?",_tag_dyneither(_tmpF0C,sizeof(char),2))),_tag_dyneither(
_tmpF0B,sizeof(void*),0)));}goto _LL441;_LL441:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
