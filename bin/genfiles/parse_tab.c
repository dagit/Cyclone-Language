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
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
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
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
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
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum Cyc_Absyn_Scope
sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*
fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;
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
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
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
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
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
_tmp84C;struct Cyc_Position_Exit_struct*_tmp84B;(int)_throw((void*)((_tmp84B=
_cycalloc_atomic(sizeof(*_tmp84B)),((_tmp84B[0]=((_tmp84C.tag=Cyc_Position_Exit,
_tmp84C)),_tmp84B)))));};}static void Cyc_Parse_unimp(struct _dyneither_ptr msg,
struct Cyc_Position_Segment*sg){{const char*_tmp851;void*_tmp850[2];struct Cyc_String_pa_struct
_tmp84F;struct Cyc_String_pa_struct _tmp84E;(_tmp84E.tag=0,((_tmp84E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp84F.tag=0,((_tmp84F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(sg)),((
_tmp850[0]=& _tmp84F,((_tmp850[1]=& _tmp84E,Cyc_fprintf(Cyc_stderr,((_tmp851="%s: Warning: Cyclone does not yet support %s\n",
_tag_dyneither(_tmp851,sizeof(char),46))),_tag_dyneither(_tmp850,sizeof(void*),2)))))))))))));}
return;}struct _tuple11{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;
union Cyc_Absyn_Constraint*f3;void*f4;};static struct _tuple11 Cyc_Parse_collapse_pointer_quals(
struct Cyc_Position_Segment*loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*
bound,void*rgn,struct Cyc_List_List*pqs){union Cyc_Absyn_Constraint*zeroterm=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();for(0;pqs != 0;pqs=pqs->tl){
void*_tmp6=(void*)pqs->hd;struct Cyc_Absyn_Exp*_tmpE;void*_tmp10;_LL1: {struct Cyc_Absyn_Zeroterm_ptrqual_struct*
_tmp7=(struct Cyc_Absyn_Zeroterm_ptrqual_struct*)_tmp6;if(_tmp7->tag != 4)goto _LL3;}
_LL2: zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: {struct Cyc_Absyn_Nozeroterm_ptrqual_struct*
_tmp8=(struct Cyc_Absyn_Nozeroterm_ptrqual_struct*)_tmp6;if(_tmp8->tag != 5)goto
_LL5;}_LL4: zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: {struct Cyc_Absyn_Nullable_ptrqual_struct*
_tmp9=(struct Cyc_Absyn_Nullable_ptrqual_struct*)_tmp6;if(_tmp9->tag != 7)goto _LL7;}
_LL6: nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: {struct Cyc_Absyn_Notnull_ptrqual_struct*
_tmpA=(struct Cyc_Absyn_Notnull_ptrqual_struct*)_tmp6;if(_tmpA->tag != 6)goto _LL9;}
_LL8: nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: {struct Cyc_Absyn_Fat_ptrqual_struct*
_tmpB=(struct Cyc_Absyn_Fat_ptrqual_struct*)_tmp6;if(_tmpB->tag != 3)goto _LLB;}
_LLA: bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB: {struct Cyc_Absyn_Thin_ptrqual_struct*
_tmpC=(struct Cyc_Absyn_Thin_ptrqual_struct*)_tmp6;if(_tmpC->tag != 2)goto _LLD;}
_LLC: bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD: {struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmpD=(struct Cyc_Absyn_Numelts_ptrqual_struct*)_tmp6;if(_tmpD->tag != 0)goto _LLF;
else{_tmpE=_tmpD->f1;}}_LLE:{struct Cyc_Absyn_Upper_b_struct _tmp854;struct Cyc_Absyn_Upper_b_struct*
_tmp853;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmp853=_cycalloc(sizeof(*_tmp853)),((_tmp853[0]=((_tmp854.tag=1,((
_tmp854.f1=_tmpE,_tmp854)))),_tmp853)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_struct*
_tmpF=(struct Cyc_Absyn_Region_ptrqual_struct*)_tmp6;if(_tmpF->tag != 1)goto _LL0;
else{_tmp10=(void*)_tmpF->f1;}}_LL10: rgn=_tmp10;goto _LL0;_LL0:;}{struct _tuple11
_tmp855;return(_tmp855.f1=nullable,((_tmp855.f2=bound,((_tmp855.f3=zeroterm,((
_tmp855.f4=rgn,_tmp855)))))));};}struct _tuple1*Cyc_Parse_gensym_enum(){static int
enum_counter=0;struct _dyneither_ptr*_tmp862;const char*_tmp861;void*_tmp860[1];
struct Cyc_Int_pa_struct _tmp85F;struct _tuple1*_tmp85E;return(_tmp85E=_cycalloc(
sizeof(*_tmp85E)),((_tmp85E->f1=Cyc_Absyn_Rel_n(0),((_tmp85E->f2=((_tmp862=
_cycalloc(sizeof(*_tmp862)),((_tmp862[0]=(struct _dyneither_ptr)((_tmp85F.tag=1,((
_tmp85F.f1=(unsigned long)enum_counter ++,((_tmp860[0]=& _tmp85F,Cyc_aprintf(((
_tmp861="__anonymous_enum_%d__",_tag_dyneither(_tmp861,sizeof(char),22))),
_tag_dyneither(_tmp860,sizeof(void*),1)))))))),_tmp862)))),_tmp85E)))));}struct
_tuple12{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;
struct Cyc_List_List*f5;};struct _tuple13{struct _tuple12*f1;struct Cyc_Absyn_Exp*f2;
};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*
loc,struct _tuple13*field_info){struct _tuple13 _tmp1A;struct _tuple12*_tmp1B;struct
_tuple12 _tmp1C;struct _tuple1*_tmp1D;struct Cyc_Absyn_Tqual _tmp1E;void*_tmp1F;
struct Cyc_List_List*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_Absyn_Exp*_tmp22;
struct _tuple13*_tmp19=field_info;_tmp1A=*_tmp19;_tmp1B=_tmp1A.f1;_tmp1C=*_tmp1B;
_tmp1D=_tmp1C.f1;_tmp1E=_tmp1C.f2;_tmp1F=_tmp1C.f3;_tmp20=_tmp1C.f4;_tmp21=
_tmp1C.f5;_tmp22=_tmp1A.f2;if(_tmp20 != 0){const char*_tmp863;Cyc_Parse_err(((
_tmp863="bad type params in struct field",_tag_dyneither(_tmp863,sizeof(char),32))),
loc);}if(Cyc_Absyn_is_qvar_qualified(_tmp1D)){const char*_tmp864;Cyc_Parse_err(((
_tmp864="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp864,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp865;return(_tmp865=_cycalloc(sizeof(*_tmp865)),((_tmp865->name=(*_tmp1D).f2,((
_tmp865->tq=_tmp1E,((_tmp865->type=_tmp1F,((_tmp865->width=_tmp22,((_tmp865->attributes=
_tmp21,_tmp865)))))))))));};}static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*
loc){struct Cyc_Parse_Type_spec_struct _tmp868;struct Cyc_Parse_Type_spec_struct*
_tmp867;return(void*)((_tmp867=_cycalloc(sizeof(*_tmp867)),((_tmp867[0]=((
_tmp868.tag=4,((_tmp868.f1=(void*)t,((_tmp868.f2=loc,_tmp868)))))),_tmp867))));}
static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp28=t;struct Cyc_Absyn_ArrayInfo
_tmp2A;void*_tmp2B;struct Cyc_Absyn_Tqual _tmp2C;struct Cyc_Absyn_Exp*_tmp2D;union
Cyc_Absyn_Constraint*_tmp2E;struct Cyc_Position_Segment*_tmp2F;_LL12: {struct Cyc_Absyn_ArrayType_struct*
_tmp29=(struct Cyc_Absyn_ArrayType_struct*)_tmp28;if(_tmp29->tag != 9)goto _LL14;
else{_tmp2A=_tmp29->f1;_tmp2B=_tmp2A.elt_type;_tmp2C=_tmp2A.tq;_tmp2D=_tmp2A.num_elts;
_tmp2E=_tmp2A.zero_term;_tmp2F=_tmp2A.zt_loc;}}_LL13: {struct Cyc_Absyn_Upper_b_struct
_tmp86B;struct Cyc_Absyn_Upper_b_struct*_tmp86A;return Cyc_Absyn_starb_typ(_tmp2B,
argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,
_tmp2C,_tmp2D == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((void*)((_tmp86A=
_cycalloc(sizeof(*_tmp86A)),((_tmp86A[0]=((_tmp86B.tag=1,((_tmp86B.f1=(struct Cyc_Absyn_Exp*)
_tmp2D,_tmp86B)))),_tmp86A))))),_tmp2E);}_LL14:;_LL15: return t;_LL11:;}struct
_tuple14{struct _dyneither_ptr*f1;void*f2;};static struct Cyc_List_List*Cyc_Parse_get_arg_tags(
struct Cyc_List_List*x){struct Cyc_List_List*_tmp32=0;for(0;x != 0;x=x->tl){struct
_tuple8*_tmp33=(struct _tuple8*)x->hd;struct _tuple8 _tmp34;struct Cyc_Core_Opt*
_tmp35;struct Cyc_Core_Opt _tmp36;struct _dyneither_ptr*_tmp37;void*_tmp38;void*
_tmp3A;struct _tuple8 _tmp3B;struct Cyc_Core_Opt*_tmp3C;struct Cyc_Core_Opt _tmp3D;
struct _dyneither_ptr*_tmp3E;void*_tmp3F;void*_tmp41;struct Cyc_Core_Opt*_tmp43;
struct Cyc_Core_Opt**_tmp44;_LL17: _tmp34=*_tmp33;_tmp35=_tmp34.f1;if(_tmp35 == 0)
goto _LL19;_tmp36=*_tmp35;_tmp37=(struct _dyneither_ptr*)_tmp36.v;_tmp38=_tmp34.f3;{
struct Cyc_Absyn_TagType_struct*_tmp39=(struct Cyc_Absyn_TagType_struct*)_tmp38;
if(_tmp39->tag != 20)goto _LL19;else{_tmp3A=(void*)_tmp39->f1;}};_LL18:{void*
_tmp45=_tmp3A;struct Cyc_Core_Opt*_tmp47;struct Cyc_Core_Opt**_tmp48;_LL1E: {
struct Cyc_Absyn_Evar_struct*_tmp46=(struct Cyc_Absyn_Evar_struct*)_tmp45;if(
_tmp46->tag != 1)goto _LL20;else{_tmp47=_tmp46->f2;_tmp48=(struct Cyc_Core_Opt**)&
_tmp46->f2;}}_LL1F: {struct Cyc_String_pa_struct _tmp873;void*_tmp872[1];const char*
_tmp871;struct _dyneither_ptr*_tmp870;struct _dyneither_ptr*nm=(_tmp870=_cycalloc(
sizeof(*_tmp870)),((_tmp870[0]=(struct _dyneither_ptr)((_tmp873.tag=0,((_tmp873.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp37),((_tmp872[0]=& _tmp873,Cyc_aprintf(((
_tmp871="`%s",_tag_dyneither(_tmp871,sizeof(char),4))),_tag_dyneither(_tmp872,
sizeof(void*),1)))))))),_tmp870)));{struct Cyc_Absyn_VarType_struct*_tmp888;
struct Cyc_Absyn_Tvar*_tmp887;struct Cyc_Absyn_Eq_kb_struct _tmp886;struct Cyc_Absyn_Eq_kb_struct*
_tmp885;struct Cyc_Absyn_VarType_struct _tmp884;struct Cyc_Core_Opt*_tmp883;*_tmp48=((
_tmp883=_cycalloc(sizeof(*_tmp883)),((_tmp883->v=(void*)((_tmp888=_cycalloc(
sizeof(*_tmp888)),((_tmp888[0]=((_tmp884.tag=2,((_tmp884.f1=((_tmp887=_cycalloc(
sizeof(*_tmp887)),((_tmp887->name=nm,((_tmp887->identity=- 1,((_tmp887->kind=(
void*)((_tmp885=_cycalloc(sizeof(*_tmp885)),((_tmp885[0]=((_tmp886.tag=0,((
_tmp886.f1=& Cyc_Tcutil_ik,_tmp886)))),_tmp885)))),_tmp887)))))))),_tmp884)))),
_tmp888)))),_tmp883))));}goto _LL1D;}_LL20:;_LL21: goto _LL1D;_LL1D:;}{struct
_tuple14*_tmp88B;struct Cyc_List_List*_tmp88A;_tmp32=((_tmp88A=_cycalloc(sizeof(*
_tmp88A)),((_tmp88A->hd=((_tmp88B=_cycalloc(sizeof(*_tmp88B)),((_tmp88B->f1=
_tmp37,((_tmp88B->f2=_tmp3A,_tmp88B)))))),((_tmp88A->tl=_tmp32,_tmp88A))))));}
goto _LL16;_LL19: _tmp3B=*_tmp33;_tmp3C=_tmp3B.f1;if(_tmp3C == 0)goto _LL1B;_tmp3D=*
_tmp3C;_tmp3E=(struct _dyneither_ptr*)_tmp3D.v;_tmp3F=_tmp3B.f3;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp40=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp3F;if(_tmp40->tag != 16)goto
_LL1B;else{_tmp41=(void*)_tmp40->f1;{struct Cyc_Absyn_Evar_struct*_tmp42=(struct
Cyc_Absyn_Evar_struct*)_tmp41;if(_tmp42->tag != 1)goto _LL1B;else{_tmp43=_tmp42->f2;
_tmp44=(struct Cyc_Core_Opt**)& _tmp42->f2;}};}};_LL1A: {struct Cyc_String_pa_struct
_tmp893;void*_tmp892[1];const char*_tmp891;struct _dyneither_ptr*_tmp890;struct
_dyneither_ptr*nm=(_tmp890=_cycalloc(sizeof(*_tmp890)),((_tmp890[0]=(struct
_dyneither_ptr)((_tmp893.tag=0,((_tmp893.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp3E),((_tmp892[0]=& _tmp893,Cyc_aprintf(((_tmp891="`%s",
_tag_dyneither(_tmp891,sizeof(char),4))),_tag_dyneither(_tmp892,sizeof(void*),1)))))))),
_tmp890)));{struct Cyc_Absyn_VarType_struct*_tmp8A8;struct Cyc_Absyn_Tvar*_tmp8A7;
struct Cyc_Absyn_Eq_kb_struct _tmp8A6;struct Cyc_Absyn_Eq_kb_struct*_tmp8A5;struct
Cyc_Absyn_VarType_struct _tmp8A4;struct Cyc_Core_Opt*_tmp8A3;*_tmp44=((_tmp8A3=
_cycalloc(sizeof(*_tmp8A3)),((_tmp8A3->v=(void*)((_tmp8A8=_cycalloc(sizeof(*
_tmp8A8)),((_tmp8A8[0]=((_tmp8A4.tag=2,((_tmp8A4.f1=((_tmp8A7=_cycalloc(sizeof(*
_tmp8A7)),((_tmp8A7->name=nm,((_tmp8A7->identity=- 1,((_tmp8A7->kind=(void*)((
_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5[0]=((_tmp8A6.tag=0,((_tmp8A6.f1=&
Cyc_Tcutil_rk,_tmp8A6)))),_tmp8A5)))),_tmp8A7)))))))),_tmp8A4)))),_tmp8A8)))),
_tmp8A3))));}goto _LL16;}_LL1B:;_LL1C: goto _LL16;_LL16:;}return _tmp32;}static
struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){struct
Cyc_List_List*_tmp5F=0;for(0;x != 0;x=x->tl){void*_tmp60=((struct Cyc_Absyn_Aggrfield*)
x->hd)->type;void*_tmp62;_LL23: {struct Cyc_Absyn_TagType_struct*_tmp61=(struct
Cyc_Absyn_TagType_struct*)_tmp60;if(_tmp61->tag != 20)goto _LL25;else{_tmp62=(void*)
_tmp61->f1;}}_LL24:{struct _tuple14*_tmp8AB;struct Cyc_List_List*_tmp8AA;_tmp5F=((
_tmp8AA=_cycalloc(sizeof(*_tmp8AA)),((_tmp8AA->hd=((_tmp8AB=_cycalloc(sizeof(*
_tmp8AB)),((_tmp8AB->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8AB->f2=
_tmp62,_tmp8AB)))))),((_tmp8AA->tl=_tmp5F,_tmp8AA))))));}goto _LL22;_LL25:;_LL26:
goto _LL22;_LL22:;}return _tmp5F;}static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(
struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){{void*_tmp65=e->r;struct _tuple1*
_tmp67;struct _tuple1 _tmp68;union Cyc_Absyn_Nmspace _tmp69;struct Cyc_List_List*
_tmp6A;struct _dyneither_ptr*_tmp6B;struct _tuple1*_tmp6D;struct _tuple1 _tmp6E;
union Cyc_Absyn_Nmspace _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr*
_tmp71;_LL28: {struct Cyc_Absyn_Var_e_struct*_tmp66=(struct Cyc_Absyn_Var_e_struct*)
_tmp65;if(_tmp66->tag != 1)goto _LL2A;else{_tmp67=_tmp66->f1;_tmp68=*_tmp67;_tmp69=
_tmp68.f1;if((_tmp69.Rel_n).tag != 1)goto _LL2A;_tmp6A=(struct Cyc_List_List*)(
_tmp69.Rel_n).val;if(_tmp6A != 0)goto _LL2A;_tmp6B=_tmp68.f2;}}_LL29: _tmp71=_tmp6B;
goto _LL2B;_LL2A: {struct Cyc_Absyn_UnknownId_e_struct*_tmp6C=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp65;if(_tmp6C->tag != 2)goto _LL2C;else{_tmp6D=_tmp6C->f1;_tmp6E=*_tmp6D;_tmp6F=
_tmp6E.f1;if((_tmp6F.Rel_n).tag != 1)goto _LL2C;_tmp70=(struct Cyc_List_List*)(
_tmp6F.Rel_n).val;if(_tmp70 != 0)goto _LL2C;_tmp71=_tmp6E.f2;}}_LL2B:{struct Cyc_List_List*
_tmp72=tags;for(0;_tmp72 != 0;_tmp72=_tmp72->tl){struct _tuple14 _tmp74;struct
_dyneither_ptr*_tmp75;void*_tmp76;struct _tuple14*_tmp73=(struct _tuple14*)_tmp72->hd;
_tmp74=*_tmp73;_tmp75=_tmp74.f1;_tmp76=_tmp74.f2;if(Cyc_strptrcmp(_tmp75,_tmp71)
== 0){struct Cyc_Absyn_Valueof_e_struct _tmp8AE;struct Cyc_Absyn_Valueof_e_struct*
_tmp8AD;return Cyc_Absyn_new_exp((void*)((_tmp8AD=_cycalloc(sizeof(*_tmp8AD)),((
_tmp8AD[0]=((_tmp8AE.tag=39,((_tmp8AE.f1=(void*)Cyc_Tcutil_copy_type(_tmp76),
_tmp8AE)))),_tmp8AD)))),e->loc);}}}goto _LL27;_LL2C:;_LL2D: goto _LL27;_LL27:;}
return e;}static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){{
void*_tmp79=t;struct Cyc_Absyn_ArrayInfo _tmp7B;void*_tmp7C;struct Cyc_Absyn_Tqual
_tmp7D;struct Cyc_Absyn_Exp*_tmp7E;union Cyc_Absyn_Constraint*_tmp7F;struct Cyc_Position_Segment*
_tmp80;struct Cyc_Absyn_PtrInfo _tmp82;void*_tmp83;struct Cyc_Absyn_Tqual _tmp84;
struct Cyc_Absyn_PtrAtts _tmp85;void*_tmp86;union Cyc_Absyn_Constraint*_tmp87;union
Cyc_Absyn_Constraint*_tmp88;union Cyc_Absyn_Constraint*_tmp89;struct Cyc_Absyn_PtrLoc*
_tmp8A;_LL2F: {struct Cyc_Absyn_ArrayType_struct*_tmp7A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp79;if(_tmp7A->tag != 9)goto _LL31;else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.elt_type;
_tmp7D=_tmp7B.tq;_tmp7E=_tmp7B.num_elts;_tmp7F=_tmp7B.zero_term;_tmp80=_tmp7B.zt_loc;}}
_LL30: {struct Cyc_Absyn_Exp*nelts2=_tmp7E;if(_tmp7E != 0)nelts2=(struct Cyc_Absyn_Exp*)
Cyc_Parse_substitute_tags_exp(tags,(struct Cyc_Absyn_Exp*)_tmp7E);{void*_tmp8B=
Cyc_Parse_substitute_tags(tags,_tmp7C);if(_tmp7E != nelts2  || _tmp7C != _tmp8B){
struct Cyc_Absyn_ArrayType_struct _tmp8B4;struct Cyc_Absyn_ArrayInfo _tmp8B3;struct
Cyc_Absyn_ArrayType_struct*_tmp8B2;return(void*)((_tmp8B2=_cycalloc(sizeof(*
_tmp8B2)),((_tmp8B2[0]=((_tmp8B4.tag=9,((_tmp8B4.f1=((_tmp8B3.elt_type=_tmp8B,((
_tmp8B3.tq=_tmp7D,((_tmp8B3.num_elts=nelts2,((_tmp8B3.zero_term=_tmp7F,((_tmp8B3.zt_loc=
_tmp80,_tmp8B3)))))))))),_tmp8B4)))),_tmp8B2))));}goto _LL2E;};}_LL31: {struct Cyc_Absyn_PointerType_struct*
_tmp81=(struct Cyc_Absyn_PointerType_struct*)_tmp79;if(_tmp81->tag != 5)goto _LL33;
else{_tmp82=_tmp81->f1;_tmp83=_tmp82.elt_typ;_tmp84=_tmp82.elt_tq;_tmp85=_tmp82.ptr_atts;
_tmp86=_tmp85.rgn;_tmp87=_tmp85.nullable;_tmp88=_tmp85.bounds;_tmp89=_tmp85.zero_term;
_tmp8A=_tmp85.ptrloc;}}_LL32: {void*_tmp8F=Cyc_Parse_substitute_tags(tags,_tmp83);
union Cyc_Absyn_Constraint*_tmp90=_tmp88;{union Cyc_Absyn_Constraint _tmp91=*_tmp88;
void*_tmp92;struct Cyc_Absyn_Exp*_tmp94;_LL36: if((_tmp91.Eq_constr).tag != 1)goto
_LL38;_tmp92=(void*)(_tmp91.Eq_constr).val;{struct Cyc_Absyn_Upper_b_struct*
_tmp93=(struct Cyc_Absyn_Upper_b_struct*)_tmp92;if(_tmp93->tag != 1)goto _LL38;
else{_tmp94=_tmp93->f1;}};_LL37: _tmp94=Cyc_Parse_substitute_tags_exp(tags,_tmp94);{
struct Cyc_Absyn_Upper_b_struct*_tmp8BA;struct Cyc_Absyn_Upper_b_struct _tmp8B9;
union Cyc_Absyn_Constraint*_tmp8B8;_tmp90=((_tmp8B8=_cycalloc(sizeof(*_tmp8B8)),(((
_tmp8B8->Eq_constr).val=(void*)((_tmp8BA=_cycalloc(sizeof(*_tmp8BA)),((_tmp8BA[0]=((
_tmp8B9.tag=1,((_tmp8B9.f1=_tmp94,_tmp8B9)))),_tmp8BA)))),(((_tmp8B8->Eq_constr).tag=
1,_tmp8B8))))));}goto _LL35;_LL38:;_LL39: goto _LL35;_LL35:;}if(_tmp8F != _tmp83  || 
_tmp90 != _tmp88){struct Cyc_Absyn_PointerType_struct _tmp8C4;struct Cyc_Absyn_PtrAtts
_tmp8C3;struct Cyc_Absyn_PtrInfo _tmp8C2;struct Cyc_Absyn_PointerType_struct*
_tmp8C1;return(void*)((_tmp8C1=_cycalloc(sizeof(*_tmp8C1)),((_tmp8C1[0]=((
_tmp8C4.tag=5,((_tmp8C4.f1=((_tmp8C2.elt_typ=_tmp8F,((_tmp8C2.elt_tq=_tmp84,((
_tmp8C2.ptr_atts=((_tmp8C3.rgn=_tmp86,((_tmp8C3.nullable=_tmp87,((_tmp8C3.bounds=
_tmp90,((_tmp8C3.zero_term=_tmp89,((_tmp8C3.ptrloc=_tmp8A,_tmp8C3)))))))))),
_tmp8C2)))))),_tmp8C4)))),_tmp8C1))));}goto _LL2E;}_LL33:;_LL34: goto _LL2E;_LL2E:;}
return t;}static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,
struct Cyc_Absyn_Aggrfield*x){x->type=Cyc_Parse_substitute_tags(tags,x->type);}
struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple15*Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment*loc,struct _tuple8*t){struct _tuple15*_tmp8C5;return(
_tmp8C5=_cycalloc(sizeof(*_tmp8C5)),((_tmp8C5->f1=(*t).f2,((_tmp8C5->f2=(*t).f3,
_tmp8C5)))));}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*
d){void*_tmp9D=d->r;struct Cyc_Absyn_Vardecl*_tmp9F;_LL3B: {struct Cyc_Absyn_Var_d_struct*
_tmp9E=(struct Cyc_Absyn_Var_d_struct*)_tmp9D;if(_tmp9E->tag != 0)goto _LL3D;else{
_tmp9F=_tmp9E->f1;}}_LL3C: return _tmp9F;_LL3D:;_LL3E: {const char*_tmp8C8;void*
_tmp8C7;(_tmp8C7=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((_tmp8C8="bad declaration in `forarray' statement",
_tag_dyneither(_tmp8C8,sizeof(char),40))),_tag_dyneither(_tmp8C7,sizeof(void*),0)));}
_LL3A:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmpA2=tm;_LL40: {struct
Cyc_Absyn_TypeParams_mod_struct*_tmpA3=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpA2;if(_tmpA3->tag != 4)goto _LL42;}_LL41: return 1;_LL42:;_LL43: return 0;_LL3F:;}
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){const char*_tmp8C9;if(
Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8C9="`H",_tag_dyneither(_tmp8C9,
sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_HeapRgn_val;else{const char*
_tmp8CA;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8CA="`U",_tag_dyneither(
_tmp8CA,sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_UniqueRgn_val;else{struct
Cyc_Absyn_VarType_struct _tmp8D4;struct _dyneither_ptr*_tmp8D3;struct Cyc_Absyn_Tvar*
_tmp8D2;struct Cyc_Absyn_VarType_struct*_tmp8D1;return(void*)((_tmp8D1=_cycalloc(
sizeof(*_tmp8D1)),((_tmp8D1[0]=((_tmp8D4.tag=2,((_tmp8D4.f1=((_tmp8D2=_cycalloc(
sizeof(*_tmp8D2)),((_tmp8D2->name=((_tmp8D3=_cycalloc(sizeof(*_tmp8D3)),((
_tmp8D3[0]=s,_tmp8D3)))),((_tmp8D2->identity=- 1,((_tmp8D2->kind=k,_tmp8D2)))))))),
_tmp8D4)))),_tmp8D1))));}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct
Cyc_Absyn_Tvar*t){void*k;{void*_tmpAA=Cyc_Absyn_compress_kb(t->kind);struct Cyc_Absyn_Kind*
_tmpAD;_LL45: {struct Cyc_Absyn_Unknown_kb_struct*_tmpAB=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpAA;if(_tmpAB->tag != 1)goto _LL47;}_LL46:{struct Cyc_Absyn_Unknown_kb_struct
_tmp8D7;struct Cyc_Absyn_Unknown_kb_struct*_tmp8D6;k=(void*)((_tmp8D6=_cycalloc(
sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D7.tag=1,((_tmp8D7.f1=0,_tmp8D7)))),
_tmp8D6))));}goto _LL44;_LL47: {struct Cyc_Absyn_Less_kb_struct*_tmpAC=(struct Cyc_Absyn_Less_kb_struct*)
_tmpAA;if(_tmpAC->tag != 2)goto _LL49;else{_tmpAD=_tmpAC->f2;}}_LL48:{struct Cyc_Absyn_Less_kb_struct
_tmp8DA;struct Cyc_Absyn_Less_kb_struct*_tmp8D9;k=(void*)((_tmp8D9=_cycalloc(
sizeof(*_tmp8D9)),((_tmp8D9[0]=((_tmp8DA.tag=2,((_tmp8DA.f1=0,((_tmp8DA.f2=
_tmpAD,_tmp8DA)))))),_tmp8D9))));}goto _LL44;_LL49:;_LL4A: k=_tmpAA;goto _LL44;
_LL44:;}{struct Cyc_Absyn_Tvar*_tmp8DB;return(_tmp8DB=_cycalloc(sizeof(*_tmp8DB)),((
_tmp8DB->name=t->name,((_tmp8DB->identity=- 1,((_tmp8DB->kind=k,_tmp8DB)))))));};}
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,
void*t){void*_tmpB3=t;struct Cyc_Absyn_Tvar*_tmpB5;_LL4C: {struct Cyc_Absyn_VarType_struct*
_tmpB4=(struct Cyc_Absyn_VarType_struct*)_tmpB3;if(_tmpB4->tag != 2)goto _LL4E;
else{_tmpB5=_tmpB4->f1;}}_LL4D: return _tmpB5;_LL4E:;_LL4F: {const char*_tmp8DE;
void*_tmp8DD;(_tmp8DD=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp8DE="expecting a list of type variables, not types",
_tag_dyneither(_tmp8DE,sizeof(char),46))),_tag_dyneither(_tmp8DD,sizeof(void*),0)));}
_LL4B:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct
_tmp8E1;struct Cyc_Absyn_VarType_struct*_tmp8E0;return(void*)((_tmp8E0=_cycalloc(
sizeof(*_tmp8E0)),((_tmp8E0[0]=((_tmp8E1.tag=2,((_tmp8E1.f1=pr,_tmp8E1)))),
_tmp8E0))));}static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,
int leq){void*_tmpBA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpBC;struct Cyc_Absyn_Tvar
_tmpBD;void*_tmpBE;void**_tmpBF;_LL51: {struct Cyc_Absyn_VarType_struct*_tmpBB=(
struct Cyc_Absyn_VarType_struct*)_tmpBA;if(_tmpBB->tag != 2)goto _LL53;else{_tmpBC=
_tmpBB->f1;_tmpBD=*_tmpBC;_tmpBE=_tmpBD.kind;_tmpBF=(void**)&(*_tmpBB->f1).kind;}}
_LL52: {void*_tmpC0=Cyc_Absyn_compress_kb(*_tmpBF);_LL56: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpC1=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC0;if(_tmpC1->tag != 1)goto _LL58;}
_LL57: if(!leq)*_tmpBF=Cyc_Tcutil_kind_to_bound(k);else{struct Cyc_Absyn_Less_kb_struct
_tmp8E4;struct Cyc_Absyn_Less_kb_struct*_tmp8E3;*_tmpBF=(void*)((_tmp8E3=
_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E4.tag=2,((_tmp8E4.f1=0,((
_tmp8E4.f2=k,_tmp8E4)))))),_tmp8E3))));}return;_LL58:;_LL59: return;_LL55:;}_LL53:;
_LL54: return;_LL50:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmpC4=(void*)tms->hd;void*_tmpC6;
_LL5B: {struct Cyc_Absyn_Function_mod_struct*_tmpC5=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC4;if(_tmpC5->tag != 3)goto _LL5D;else{_tmpC6=(void*)_tmpC5->f1;}}_LL5C: if(tms->tl
== 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)
 && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){void*_tmpC7=_tmpC6;
struct Cyc_List_List*_tmpCA;_LL60: {struct Cyc_Absyn_WithTypes_struct*_tmpC8=(
struct Cyc_Absyn_WithTypes_struct*)_tmpC7;if(_tmpC8->tag != 1)goto _LL62;}_LL61:{
const char*_tmp8E7;void*_tmp8E6;(_tmp8E6=0,Cyc_Tcutil_warn(loc,((_tmp8E7="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp8E7,sizeof(char),93))),_tag_dyneither(_tmp8E6,sizeof(void*),0)));}
return tms;_LL62: {struct Cyc_Absyn_NoTypes_struct*_tmpC9=(struct Cyc_Absyn_NoTypes_struct*)
_tmpC7;if(_tmpC9->tag != 0)goto _LL5F;else{_tmpCA=_tmpC9->f1;}}_LL63: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpCA)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds)){const char*_tmp8EA;void*_tmp8E9;(_tmp8E9=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
loc,((_tmp8EA="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp8EA,sizeof(char),73))),_tag_dyneither(_tmp8E9,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpCF=0;for(0;_tmpCA != 0;_tmpCA=_tmpCA->tl){struct Cyc_List_List*
_tmpD0=tds;for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){struct Cyc_Absyn_Decl*_tmpD1=(
struct Cyc_Absyn_Decl*)_tmpD0->hd;void*_tmpD2=_tmpD1->r;struct Cyc_Absyn_Vardecl*
_tmpD4;_LL65: {struct Cyc_Absyn_Var_d_struct*_tmpD3=(struct Cyc_Absyn_Var_d_struct*)
_tmpD2;if(_tmpD3->tag != 0)goto _LL67;else{_tmpD4=_tmpD3->f1;}}_LL66: if(Cyc_zstrptrcmp((*
_tmpD4->name).f2,(struct _dyneither_ptr*)_tmpCA->hd)!= 0)continue;if(_tmpD4->initializer
!= 0){const char*_tmp8ED;void*_tmp8EC;(_tmp8EC=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,((
_tmp8ED="initializer found in parameter declaration",_tag_dyneither(_tmp8ED,
sizeof(char),43))),_tag_dyneither(_tmp8EC,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmpD4->name)){const char*_tmp8F0;void*_tmp8EF;(_tmp8EF=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,((
_tmp8F0="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8F0,
sizeof(char),47))),_tag_dyneither(_tmp8EF,sizeof(void*),0)));}{struct _tuple8*
_tmp8F6;struct Cyc_Core_Opt*_tmp8F5;struct Cyc_List_List*_tmp8F4;_tmpCF=((_tmp8F4=
_cycalloc(sizeof(*_tmp8F4)),((_tmp8F4->hd=((_tmp8F6=_cycalloc(sizeof(*_tmp8F6)),((
_tmp8F6->f1=((_tmp8F5=_cycalloc(sizeof(*_tmp8F5)),((_tmp8F5->v=(*_tmpD4->name).f2,
_tmp8F5)))),((_tmp8F6->f2=_tmpD4->tq,((_tmp8F6->f3=_tmpD4->type,_tmp8F6)))))))),((
_tmp8F4->tl=_tmpCF,_tmp8F4))))));}goto L;_LL67:;_LL68: {const char*_tmp8F9;void*
_tmp8F8;(_tmp8F8=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,((_tmp8F9="nonvariable declaration in parameter type",
_tag_dyneither(_tmp8F9,sizeof(char),42))),_tag_dyneither(_tmp8F8,sizeof(void*),0)));}
_LL64:;}L: if(_tmpD0 == 0){const char*_tmp8FD;void*_tmp8FC[1];struct Cyc_String_pa_struct
_tmp8FB;(_tmp8FB.tag=0,((_tmp8FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmpCA->hd)),((_tmp8FC[0]=& _tmp8FB,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp8FD="%s is not given a type",_tag_dyneither(_tmp8FD,sizeof(char),23))),
_tag_dyneither(_tmp8FC,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp90C;struct Cyc_Absyn_WithTypes_struct*_tmp90B;struct Cyc_Absyn_WithTypes_struct
_tmp90A;struct Cyc_Absyn_Function_mod_struct _tmp909;struct Cyc_List_List*_tmp908;
return(_tmp908=_cycalloc(sizeof(*_tmp908)),((_tmp908->hd=(void*)((_tmp90C=
_cycalloc(sizeof(*_tmp90C)),((_tmp90C[0]=((_tmp909.tag=3,((_tmp909.f1=(void*)((
void*)((_tmp90B=_cycalloc(sizeof(*_tmp90B)),((_tmp90B[0]=((_tmp90A.tag=1,((
_tmp90A.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpCF),((_tmp90A.f2=0,((_tmp90A.f3=0,((_tmp90A.f4=0,((_tmp90A.f5=0,_tmp90A)))))))))))),
_tmp90B))))),_tmp909)))),_tmp90C)))),((_tmp908->tl=0,_tmp908)))));};};_LL5F:;}
goto _LL5E;_LL5D:;_LL5E: {struct Cyc_List_List*_tmp90D;return(_tmp90D=_cycalloc(
sizeof(*_tmp90D)),((_tmp90D->hd=(void*)tms->hd,((_tmp90D->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp90D)))));}_LL5A:;};}struct _tuple16{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){if(tds != 0){struct Cyc_Parse_Declarator*
_tmp90E;d=((_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E->id=d->id,((_tmp90E->tms=
Cyc_Parse_oldstyle2newstyle(d->tms,tds,loc),_tmp90E))))));}{enum Cyc_Absyn_Scope
sc=Cyc_Absyn_Public;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(
0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0)
switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v){case Cyc_Parse_Extern_sc: _LL69: sc=
Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL6A: sc=Cyc_Absyn_Static;break;
default: _LL6B:{const char*_tmp90F;Cyc_Parse_err(((_tmp90F="bad storage class on function",
_tag_dyneither(_tmp90F,sizeof(char),30))),loc);}break;}}{void*_tmpE9=Cyc_Parse_collapse_type_specifiers(
tss,loc);struct Cyc_Absyn_Tqual _tmpEB;void*_tmpEC;struct Cyc_List_List*_tmpED;
struct Cyc_List_List*_tmpEE;struct _tuple10 _tmpEA=Cyc_Parse_apply_tms(tq,_tmpE9,
atts,d->tms);_tmpEB=_tmpEA.f1;_tmpEC=_tmpEA.f2;_tmpED=_tmpEA.f3;_tmpEE=_tmpEA.f4;
if(_tmpED != 0){const char*_tmp912;void*_tmp911;(_tmp911=0,Cyc_Tcutil_warn(loc,((
_tmp912="bad type params, ignoring",_tag_dyneither(_tmp912,sizeof(char),26))),
_tag_dyneither(_tmp911,sizeof(void*),0)));}{void*_tmpF1=_tmpEC;struct Cyc_Absyn_FnInfo
_tmpF3;struct Cyc_List_List*_tmpF4;struct Cyc_Core_Opt*_tmpF5;void*_tmpF6;struct
Cyc_List_List*_tmpF7;int _tmpF8;struct Cyc_Absyn_VarargInfo*_tmpF9;struct Cyc_List_List*
_tmpFA;struct Cyc_List_List*_tmpFB;_LL6E: {struct Cyc_Absyn_FnType_struct*_tmpF2=(
struct Cyc_Absyn_FnType_struct*)_tmpF1;if(_tmpF2->tag != 10)goto _LL70;else{_tmpF3=
_tmpF2->f1;_tmpF4=_tmpF3.tvars;_tmpF5=_tmpF3.effect;_tmpF6=_tmpF3.ret_typ;_tmpF7=
_tmpF3.args;_tmpF8=_tmpF3.c_varargs;_tmpF9=_tmpF3.cyc_varargs;_tmpFA=_tmpF3.rgn_po;
_tmpFB=_tmpF3.attributes;}}_LL6F: {struct Cyc_List_List*_tmpFC=0;{struct Cyc_List_List*
_tmpFD=_tmpF7;for(0;_tmpFD != 0;_tmpFD=_tmpFD->tl){struct _tuple8 _tmpFF;struct Cyc_Core_Opt*
_tmp100;struct Cyc_Absyn_Tqual _tmp101;void*_tmp102;struct _tuple8*_tmpFE=(struct
_tuple8*)_tmpFD->hd;_tmpFF=*_tmpFE;_tmp100=_tmpFF.f1;_tmp101=_tmpFF.f2;_tmp102=
_tmpFF.f3;if(_tmp100 == 0){{const char*_tmp913;Cyc_Parse_err(((_tmp913="missing argument variable in function prototype",
_tag_dyneither(_tmp913,sizeof(char),48))),loc);}{struct _tuple16*_tmp916;struct
Cyc_List_List*_tmp915;_tmpFC=((_tmp915=_cycalloc(sizeof(*_tmp915)),((_tmp915->hd=((
_tmp916=_cycalloc(sizeof(*_tmp916)),((_tmp916->f1=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp916->f2=_tmp101,((
_tmp916->f3=_tmp102,_tmp916)))))))),((_tmp915->tl=_tmpFC,_tmp915))))));};}else{
struct _tuple16*_tmp919;struct Cyc_List_List*_tmp918;_tmpFC=((_tmp918=_cycalloc(
sizeof(*_tmp918)),((_tmp918->hd=((_tmp919=_cycalloc(sizeof(*_tmp919)),((_tmp919->f1=(
struct _dyneither_ptr*)_tmp100->v,((_tmp919->f2=_tmp101,((_tmp919->f3=_tmp102,
_tmp919)))))))),((_tmp918->tl=_tmpFC,_tmp918))))));}}}{struct Cyc_Absyn_Fndecl*
_tmp91A;return(_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A->sc=sc,((_tmp91A->name=
d->id,((_tmp91A->tvs=_tmpF4,((_tmp91A->is_inline=is_inline,((_tmp91A->effect=
_tmpF5,((_tmp91A->ret_type=_tmpF6,((_tmp91A->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpFC),((_tmp91A->c_varargs=_tmpF8,((
_tmp91A->cyc_varargs=_tmpF9,((_tmp91A->rgn_po=_tmpFA,((_tmp91A->body=body,((
_tmp91A->cached_typ=0,((_tmp91A->param_vardecls=0,((_tmp91A->fn_vardecl=0,((
_tmp91A->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmpFB,_tmpEE),_tmp91A)))))))))))))))))))))))))))))));};}
_LL70:;_LL71: {const char*_tmp91D;void*_tmp91C;(_tmp91C=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp91D="declarator is not a function prototype",_tag_dyneither(_tmp91D,sizeof(
char),39))),_tag_dyneither(_tmp91C,sizeof(void*),0)));}_LL6D:;};};};}static char
_tmp10C[52]="at most one type may appear within a type specifier";static struct
_dyneither_ptr Cyc_Parse_msg1={_tmp10C,_tmp10C,_tmp10C + 52};static char _tmp10D[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp10D,_tmp10D,_tmp10D + 63};static
char _tmp10E[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp10E,_tmp10E,_tmp10E + 50};static char _tmp10F[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp10F,_tmp10F,_tmp10F + 60};static
void*Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){int seen_type=0;int seen_sign=0;int seen_size=0;void*t=(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp110=(
void*)ts->hd;void*_tmp112;struct Cyc_Position_Segment*_tmp113;struct Cyc_Position_Segment*
_tmp115;struct Cyc_Position_Segment*_tmp117;struct Cyc_Position_Segment*_tmp119;
struct Cyc_Position_Segment*_tmp11B;_LL73: {struct Cyc_Parse_Type_spec_struct*
_tmp111=(struct Cyc_Parse_Type_spec_struct*)_tmp110;if(_tmp111->tag != 4)goto _LL75;
else{_tmp112=(void*)_tmp111->f1;_tmp113=_tmp111->f2;}}_LL74: if(seen_type)Cyc_Parse_err(
Cyc_Parse_msg1,_tmp113);last_loc=_tmp113;seen_type=1;t=_tmp112;goto _LL72;_LL75: {
struct Cyc_Parse_Signed_spec_struct*_tmp114=(struct Cyc_Parse_Signed_spec_struct*)
_tmp110;if(_tmp114->tag != 0)goto _LL77;else{_tmp115=_tmp114->f1;}}_LL76: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp115);if(seen_type){const char*_tmp91E;
Cyc_Parse_err(((_tmp91E="signed qualifier must come before type",_tag_dyneither(
_tmp91E,sizeof(char),39))),_tmp115);}last_loc=_tmp115;seen_sign=1;sgn=Cyc_Absyn_Signed;
goto _LL72;_LL77: {struct Cyc_Parse_Unsigned_spec_struct*_tmp116=(struct Cyc_Parse_Unsigned_spec_struct*)
_tmp110;if(_tmp116->tag != 1)goto _LL79;else{_tmp117=_tmp116->f1;}}_LL78: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp117);if(seen_type){const char*_tmp91F;
Cyc_Parse_err(((_tmp91F="signed qualifier must come before type",_tag_dyneither(
_tmp91F,sizeof(char),39))),_tmp117);}last_loc=_tmp117;seen_sign=1;sgn=Cyc_Absyn_Unsigned;
goto _LL72;_LL79: {struct Cyc_Parse_Short_spec_struct*_tmp118=(struct Cyc_Parse_Short_spec_struct*)
_tmp110;if(_tmp118->tag != 2)goto _LL7B;else{_tmp119=_tmp118->f1;}}_LL7A: if(
seen_size){const char*_tmp920;Cyc_Parse_err(((_tmp920="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp920,sizeof(char),59))),_tmp119);}if(seen_type){const char*
_tmp921;Cyc_Parse_err(((_tmp921="short modifier must come before base type",
_tag_dyneither(_tmp921,sizeof(char),42))),_tmp119);}last_loc=_tmp119;seen_size=1;
sz=Cyc_Absyn_Short_sz;goto _LL72;_LL7B: {struct Cyc_Parse_Long_spec_struct*_tmp11A=(
struct Cyc_Parse_Long_spec_struct*)_tmp110;if(_tmp11A->tag != 3)goto _LL72;else{
_tmp11B=_tmp11A->f1;}}_LL7C: if(seen_type){const char*_tmp922;Cyc_Parse_err(((
_tmp922="long modifier must come before base type",_tag_dyneither(_tmp922,
sizeof(char),41))),_tmp11B);}if(seen_size)switch(sz){case Cyc_Absyn_Long_sz: _LL7D:
sz=Cyc_Absyn_LongLong_sz;break;default: _LL7E:{const char*_tmp923;Cyc_Parse_err(((
_tmp923="extra long in type specifier",_tag_dyneither(_tmp923,sizeof(char),29))),
_tmp11B);}break;}else{sz=Cyc_Absyn_Long_sz;}last_loc=_tmp11B;seen_size=1;goto
_LL72;_LL72:;}if(!seen_type){if(!seen_sign  && !seen_size){const char*_tmp926;void*
_tmp925;(_tmp925=0,Cyc_Tcutil_warn(last_loc,((_tmp926="missing type within specifier",
_tag_dyneither(_tmp926,sizeof(char),30))),_tag_dyneither(_tmp925,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{if(seen_sign){void*_tmp124=t;enum Cyc_Absyn_Sign
_tmp126;enum Cyc_Absyn_Size_of _tmp127;_LL81: {struct Cyc_Absyn_IntType_struct*
_tmp125=(struct Cyc_Absyn_IntType_struct*)_tmp124;if(_tmp125->tag != 6)goto _LL83;
else{_tmp126=_tmp125->f1;_tmp127=_tmp125->f2;}}_LL82: if(_tmp126 != sgn)t=Cyc_Absyn_int_typ(
sgn,_tmp127);goto _LL80;_LL83:;_LL84:{const char*_tmp927;Cyc_Parse_err(((_tmp927="sign specification on non-integral type",
_tag_dyneither(_tmp927,sizeof(char),40))),last_loc);}goto _LL80;_LL80:;}if(
seen_size){void*_tmp129=t;enum Cyc_Absyn_Sign _tmp12B;enum Cyc_Absyn_Size_of
_tmp12C;_LL86: {struct Cyc_Absyn_IntType_struct*_tmp12A=(struct Cyc_Absyn_IntType_struct*)
_tmp129;if(_tmp12A->tag != 6)goto _LL88;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12A->f2;}}
_LL87: if(_tmp12C != sz)t=Cyc_Absyn_int_typ(_tmp12B,sz);goto _LL85;_LL88: {struct
Cyc_Absyn_DoubleType_struct*_tmp12D=(struct Cyc_Absyn_DoubleType_struct*)_tmp129;
if(_tmp12D->tag != 8)goto _LL8A;}_LL89: t=Cyc_Absyn_double_typ(1);goto _LL85;_LL8A:;
_LL8B:{const char*_tmp928;Cyc_Parse_err(((_tmp928="size qualifier on non-integral type",
_tag_dyneither(_tmp928,sizeof(char),36))),last_loc);}goto _LL85;_LL85:;}}return t;}
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)
return 0;{struct Cyc_Parse_Declarator*_tmp12F=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmp130=_tmp12F->id;struct Cyc_Absyn_Tqual _tmp132;void*_tmp133;
struct Cyc_List_List*_tmp134;struct Cyc_List_List*_tmp135;struct _tuple10 _tmp131=
Cyc_Parse_apply_tms(tq,t,shared_atts,_tmp12F->tms);_tmp132=_tmp131.f1;_tmp133=
_tmp131.f2;_tmp134=_tmp131.f3;_tmp135=_tmp131.f4;if(ds->tl == 0){struct _tuple12*
_tmp92B;struct Cyc_List_List*_tmp92A;return(_tmp92A=_region_malloc(r,sizeof(*
_tmp92A)),((_tmp92A->hd=((_tmp92B=_region_malloc(r,sizeof(*_tmp92B)),((_tmp92B->f1=
_tmp130,((_tmp92B->f2=_tmp132,((_tmp92B->f3=_tmp133,((_tmp92B->f4=_tmp134,((
_tmp92B->f5=_tmp135,_tmp92B)))))))))))),((_tmp92A->tl=0,_tmp92A)))));}else{
struct _tuple12*_tmp92E;struct Cyc_List_List*_tmp92D;return(_tmp92D=_region_malloc(
r,sizeof(*_tmp92D)),((_tmp92D->hd=((_tmp92E=_region_malloc(r,sizeof(*_tmp92E)),((
_tmp92E->f1=_tmp130,((_tmp92E->f2=_tmp132,((_tmp92E->f3=_tmp133,((_tmp92E->f4=
_tmp134,((_tmp92E->f5=_tmp135,_tmp92E)))))))))))),((_tmp92D->tl=Cyc_Parse_apply_tmss(
r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp92D)))));}};}static struct
_tuple10 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0){struct _tuple10 _tmp92F;return(_tmp92F.f1=
tq,((_tmp92F.f2=t,((_tmp92F.f3=0,((_tmp92F.f4=atts,_tmp92F)))))));}{void*_tmp13B=(
void*)tms->hd;union Cyc_Absyn_Constraint*_tmp13D;struct Cyc_Position_Segment*
_tmp13E;struct Cyc_Absyn_Exp*_tmp140;union Cyc_Absyn_Constraint*_tmp141;struct Cyc_Position_Segment*
_tmp142;void*_tmp144;struct Cyc_List_List*_tmp146;struct Cyc_Position_Segment*
_tmp147;struct Cyc_Absyn_PtrAtts _tmp149;struct Cyc_Absyn_Tqual _tmp14A;struct Cyc_Position_Segment*
_tmp14C;struct Cyc_List_List*_tmp14D;_LL8D: {struct Cyc_Absyn_Carray_mod_struct*
_tmp13C=(struct Cyc_Absyn_Carray_mod_struct*)_tmp13B;if(_tmp13C->tag != 0)goto
_LL8F;else{_tmp13D=_tmp13C->f1;_tmp13E=_tmp13C->f2;}}_LL8E: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmp13D,_tmp13E),atts,tms->tl);
_LL8F: {struct Cyc_Absyn_ConstArray_mod_struct*_tmp13F=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp13B;if(_tmp13F->tag != 1)goto _LL91;else{_tmp140=_tmp13F->f1;_tmp141=_tmp13F->f2;
_tmp142=_tmp13F->f3;}}_LL90: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmp140,_tmp141,_tmp142),atts,tms->tl);
_LL91: {struct Cyc_Absyn_Function_mod_struct*_tmp143=(struct Cyc_Absyn_Function_mod_struct*)
_tmp13B;if(_tmp143->tag != 3)goto _LL93;else{_tmp144=(void*)_tmp143->f1;}}_LL92: {
void*_tmp14E=_tmp144;struct Cyc_List_List*_tmp150;int _tmp151;struct Cyc_Absyn_VarargInfo*
_tmp152;struct Cyc_Core_Opt*_tmp153;struct Cyc_List_List*_tmp154;struct Cyc_Position_Segment*
_tmp156;_LL9A: {struct Cyc_Absyn_WithTypes_struct*_tmp14F=(struct Cyc_Absyn_WithTypes_struct*)
_tmp14E;if(_tmp14F->tag != 1)goto _LL9C;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;
_tmp152=_tmp14F->f3;_tmp153=_tmp14F->f4;_tmp154=_tmp14F->f5;}}_LL9B: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp930;fn_atts=((_tmp930=_cycalloc(sizeof(*_tmp930)),((
_tmp930->hd=(void*)as->hd,((_tmp930->tl=fn_atts,_tmp930))))));}else{struct Cyc_List_List*
_tmp931;new_atts=((_tmp931=_cycalloc(sizeof(*_tmp931)),((_tmp931->hd=(void*)as->hd,((
_tmp931->tl=new_atts,_tmp931))))));}}}if(tms->tl != 0){void*_tmp159=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;struct Cyc_List_List*_tmp15B;_LL9F: {
struct Cyc_Absyn_TypeParams_mod_struct*_tmp15A=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp159;if(_tmp15A->tag != 4)goto _LLA1;else{_tmp15B=_tmp15A->f1;}}_LLA0: typvars=
_tmp15B;tms=tms->tl;goto _LL9E;_LLA1:;_LLA2: goto _LL9E;_LL9E:;}if(((((!_tmp151  && 
_tmp152 == 0) && _tmp150 != 0) && _tmp150->tl == 0) && (*((struct _tuple8*)_tmp150->hd)).f1
== 0) && (*((struct _tuple8*)_tmp150->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
_tmp150=0;{struct Cyc_List_List*_tmp15C=Cyc_Parse_get_arg_tags(_tmp150);if(
_tmp15C != 0)t=Cyc_Parse_substitute_tags(_tmp15C,t);t=Cyc_Parse_array2ptr(t,0);{
struct Cyc_List_List*_tmp15D=_tmp150;for(0;_tmp15D != 0;_tmp15D=_tmp15D->tl){
struct _tuple8 _tmp15F;struct Cyc_Core_Opt*_tmp160;struct Cyc_Absyn_Tqual _tmp161;
void*_tmp162;void**_tmp163;struct _tuple8*_tmp15E=(struct _tuple8*)_tmp15D->hd;
_tmp15F=*_tmp15E;_tmp160=_tmp15F.f1;_tmp161=_tmp15F.f2;_tmp162=_tmp15F.f3;
_tmp163=(void**)&(*_tmp15E).f3;if(_tmp15C != 0)*_tmp163=Cyc_Parse_substitute_tags(
_tmp15C,*_tmp163);*_tmp163=Cyc_Parse_array2ptr(*_tmp163,1);}}return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(tq.loc),Cyc_Absyn_function_typ(typvars,_tmp153,t,_tmp150,
_tmp151,_tmp152,_tmp154,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}
_LL9C: {struct Cyc_Absyn_NoTypes_struct*_tmp155=(struct Cyc_Absyn_NoTypes_struct*)
_tmp14E;if(_tmp155->tag != 0)goto _LL99;else{_tmp156=_tmp155->f2;}}_LL9D: {const
char*_tmp934;void*_tmp933;(_tmp933=0,((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp156,((
_tmp934="function declaration without parameter types",_tag_dyneither(_tmp934,
sizeof(char),45))),_tag_dyneither(_tmp933,sizeof(void*),0)));}_LL99:;}_LL93: {
struct Cyc_Absyn_TypeParams_mod_struct*_tmp145=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp13B;if(_tmp145->tag != 4)goto _LL95;else{_tmp146=_tmp145->f1;_tmp147=_tmp145->f2;}}
_LL94: if(tms->tl == 0){struct _tuple10 _tmp935;return(_tmp935.f1=tq,((_tmp935.f2=t,((
_tmp935.f3=_tmp146,((_tmp935.f4=atts,_tmp935)))))));}{const char*_tmp938;void*
_tmp937;(_tmp937=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp147,((_tmp938="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp938,sizeof(char),68))),_tag_dyneither(_tmp937,sizeof(void*),0)));};
_LL95: {struct Cyc_Absyn_Pointer_mod_struct*_tmp148=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp13B;if(_tmp148->tag != 2)goto _LL97;else{_tmp149=_tmp148->f1;_tmp14A=_tmp148->f2;}}
_LL96: {struct Cyc_Absyn_PointerType_struct _tmp93E;struct Cyc_Absyn_PtrInfo _tmp93D;
struct Cyc_Absyn_PointerType_struct*_tmp93C;return Cyc_Parse_apply_tms(_tmp14A,(
void*)((_tmp93C=_cycalloc(sizeof(*_tmp93C)),((_tmp93C[0]=((_tmp93E.tag=5,((
_tmp93E.f1=((_tmp93D.elt_typ=t,((_tmp93D.elt_tq=tq,((_tmp93D.ptr_atts=_tmp149,
_tmp93D)))))),_tmp93E)))),_tmp93C)))),atts,tms->tl);}_LL97: {struct Cyc_Absyn_Attributes_mod_struct*
_tmp14B=(struct Cyc_Absyn_Attributes_mod_struct*)_tmp13B;if(_tmp14B->tag != 5)goto
_LL8C;else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14B->f2;}}_LL98: return Cyc_Parse_apply_tms(
tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
atts,_tmp14D),tms->tl);_LL8C:;};}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*
tss,struct Cyc_Position_Segment*loc){return Cyc_Parse_collapse_type_specifiers(tss,
loc);}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple12*t){struct _tuple12 _tmp16D;struct _tuple1*_tmp16E;struct Cyc_Absyn_Tqual
_tmp16F;void*_tmp170;struct Cyc_List_List*_tmp171;struct Cyc_List_List*_tmp172;
struct _tuple12*_tmp16C=t;_tmp16D=*_tmp16C;_tmp16E=_tmp16D.f1;_tmp16F=_tmp16D.f2;
_tmp170=_tmp16D.f3;_tmp171=_tmp16D.f4;_tmp172=_tmp16D.f5;Cyc_Lex_register_typedef(
_tmp16E);{struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*type;{void*_tmp173=_tmp170;
struct Cyc_Core_Opt*_tmp175;_LLA4: {struct Cyc_Absyn_Evar_struct*_tmp174=(struct
Cyc_Absyn_Evar_struct*)_tmp173;if(_tmp174->tag != 1)goto _LLA6;else{_tmp175=
_tmp174->f1;}}_LLA5: type=0;if(_tmp175 == 0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;
else{kind=_tmp175;}goto _LLA3;_LLA6:;_LLA7: kind=0;{struct Cyc_Core_Opt*_tmp93F;
type=((_tmp93F=_cycalloc(sizeof(*_tmp93F)),((_tmp93F->v=_tmp170,_tmp93F))));}
goto _LLA3;_LLA3:;}{struct Cyc_Absyn_Typedef_d_struct _tmp945;struct Cyc_Absyn_Typedefdecl*
_tmp944;struct Cyc_Absyn_Typedef_d_struct*_tmp943;return Cyc_Absyn_new_decl((void*)((
_tmp943=_cycalloc(sizeof(*_tmp943)),((_tmp943[0]=((_tmp945.tag=9,((_tmp945.f1=((
_tmp944=_cycalloc(sizeof(*_tmp944)),((_tmp944->name=_tmp16E,((_tmp944->tvs=
_tmp171,((_tmp944->kind=kind,((_tmp944->defn=type,((_tmp944->atts=_tmp172,((
_tmp944->tq=_tmp16F,_tmp944)))))))))))))),_tmp945)))),_tmp943)))),loc);};};}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct
Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Decl_s_struct _tmp948;struct Cyc_Absyn_Decl_s_struct*
_tmp947;return Cyc_Absyn_new_stmt((void*)((_tmp947=_cycalloc(sizeof(*_tmp947)),((
_tmp947[0]=((_tmp948.tag=12,((_tmp948.f1=d,((_tmp948.f2=s,_tmp948)))))),_tmp947)))),
d->loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*
ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*
f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*
accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*
Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*
ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*loc){struct
_RegionHandle _tmp17C=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp17C;
_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp17E;struct Cyc_Absyn_Tqual
_tmp17F;struct Cyc_List_List*_tmp180;struct Cyc_List_List*_tmp181;struct Cyc_Parse_Declaration_spec*
_tmp17D=ds;_tmp17E=*_tmp17D;_tmp17F=_tmp17E.tq;_tmp180=_tmp17E.type_specs;
_tmp181=_tmp17E.attributes;if(_tmp17F.loc == 0)_tmp17F.loc=tqual_loc;if(ds->is_inline){
const char*_tmp949;Cyc_Parse_err(((_tmp949="inline is allowed only on function definitions",
_tag_dyneither(_tmp949,sizeof(char),47))),loc);}if(_tmp180 == 0){{const char*
_tmp94C;void*_tmp94B;(_tmp94B=0,Cyc_Tcutil_warn(loc,((_tmp94C="missing type specifiers in declaration, assuming int",
_tag_dyneither(_tmp94C,sizeof(char),53))),_tag_dyneither(_tmp94B,sizeof(void*),0)));}{
void*_tmp94D[1];_tmp180=((_tmp94D[0]=Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,loc),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp94D,sizeof(void*),1))));};}{enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;int
istypedef=0;if(ds->sc != 0)switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v){case Cyc_Parse_Typedef_sc: _LLA8: istypedef=1;break;case
Cyc_Parse_Extern_sc: _LLA9: s=Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc:
_LLAA: s=Cyc_Absyn_ExternC;break;case Cyc_Parse_Static_sc: _LLAB: s=Cyc_Absyn_Static;
break;case Cyc_Parse_Auto_sc: _LLAC: s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc:
_LLAD: if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}
break;case Cyc_Parse_Abstract_sc: _LLAE: s=Cyc_Absyn_Abstract;break;}{struct Cyc_List_List*
_tmp187;struct Cyc_List_List*_tmp188;struct _tuple0 _tmp186=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
mkrgn,mkrgn,ids);_tmp187=_tmp186.f1;_tmp188=_tmp186.f2;{int exps_empty=1;{struct
Cyc_List_List*es=_tmp188;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd
!= 0){exps_empty=0;break;}}}{void*_tmp189=Cyc_Parse_collapse_type_specifiers(
_tmp180,loc);if(_tmp187 == 0){void*_tmp18A=_tmp189;struct Cyc_Absyn_TypeDecl*
_tmp18C;struct Cyc_Absyn_TypeDecl _tmp18D;void*_tmp18E;struct Cyc_Absyn_Aggrdecl*
_tmp190;struct Cyc_Absyn_TypeDecl*_tmp192;struct Cyc_Absyn_TypeDecl _tmp193;void*
_tmp194;struct Cyc_Absyn_Enumdecl*_tmp196;struct Cyc_Absyn_TypeDecl*_tmp198;struct
Cyc_Absyn_TypeDecl _tmp199;void*_tmp19A;struct Cyc_Absyn_Datatypedecl*_tmp19C;
struct Cyc_Absyn_AggrInfo _tmp19E;union Cyc_Absyn_AggrInfoU _tmp19F;struct _tuple3
_tmp1A0;enum Cyc_Absyn_AggrKind _tmp1A1;struct _tuple1*_tmp1A2;struct Cyc_List_List*
_tmp1A3;struct Cyc_Absyn_DatatypeInfo _tmp1A5;union Cyc_Absyn_DatatypeInfoU _tmp1A6;
struct Cyc_Absyn_Datatypedecl**_tmp1A7;struct Cyc_Absyn_DatatypeInfo _tmp1A9;union
Cyc_Absyn_DatatypeInfoU _tmp1AA;struct Cyc_Absyn_UnknownDatatypeInfo _tmp1AB;struct
_tuple1*_tmp1AC;int _tmp1AD;struct Cyc_List_List*_tmp1AE;struct _tuple1*_tmp1B0;
struct Cyc_List_List*_tmp1B2;_LLB1: {struct Cyc_Absyn_TypeDeclType_struct*_tmp18B=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp18A;if(_tmp18B->tag != 26)goto _LLB3;
else{_tmp18C=_tmp18B->f1;_tmp18D=*_tmp18C;_tmp18E=_tmp18D.r;{struct Cyc_Absyn_Aggr_td_struct*
_tmp18F=(struct Cyc_Absyn_Aggr_td_struct*)_tmp18E;if(_tmp18F->tag != 0)goto _LLB3;
else{_tmp190=_tmp18F->f1;}};}}_LLB2: _tmp190->attributes=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp190->attributes,
_tmp181);{struct Cyc_Absyn_Aggr_d_struct*_tmp953;struct Cyc_Absyn_Aggr_d_struct
_tmp952;struct Cyc_List_List*_tmp951;struct Cyc_List_List*_tmp1B6=(_tmp951=
_cycalloc(sizeof(*_tmp951)),((_tmp951->hd=Cyc_Absyn_new_decl((void*)((_tmp953=
_cycalloc(sizeof(*_tmp953)),((_tmp953[0]=((_tmp952.tag=6,((_tmp952.f1=_tmp190,
_tmp952)))),_tmp953)))),loc),((_tmp951->tl=0,_tmp951)))));_npop_handler(0);
return _tmp1B6;};_LLB3: {struct Cyc_Absyn_TypeDeclType_struct*_tmp191=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp18A;if(_tmp191->tag != 26)goto _LLB5;else{_tmp192=_tmp191->f1;_tmp193=*_tmp192;
_tmp194=_tmp193.r;{struct Cyc_Absyn_Enum_td_struct*_tmp195=(struct Cyc_Absyn_Enum_td_struct*)
_tmp194;if(_tmp195->tag != 1)goto _LLB5;else{_tmp196=_tmp195->f1;}};}}_LLB4: if(
_tmp181 != 0){const char*_tmp954;Cyc_Parse_err(((_tmp954="attributes on enum not supported",
_tag_dyneither(_tmp954,sizeof(char),33))),loc);}{struct Cyc_Absyn_Enum_d_struct*
_tmp95A;struct Cyc_Absyn_Enum_d_struct _tmp959;struct Cyc_List_List*_tmp958;struct
Cyc_List_List*_tmp1BB=(_tmp958=_cycalloc(sizeof(*_tmp958)),((_tmp958->hd=Cyc_Absyn_new_decl((
void*)((_tmp95A=_cycalloc(sizeof(*_tmp95A)),((_tmp95A[0]=((_tmp959.tag=8,((
_tmp959.f1=_tmp196,_tmp959)))),_tmp95A)))),loc),((_tmp958->tl=0,_tmp958)))));
_npop_handler(0);return _tmp1BB;};_LLB5: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp197=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp18A;if(_tmp197->tag != 26)goto
_LLB7;else{_tmp198=_tmp197->f1;_tmp199=*_tmp198;_tmp19A=_tmp199.r;{struct Cyc_Absyn_Datatype_td_struct*
_tmp19B=(struct Cyc_Absyn_Datatype_td_struct*)_tmp19A;if(_tmp19B->tag != 2)goto
_LLB7;else{_tmp19C=_tmp19B->f1;}};}}_LLB6: if(_tmp181 != 0){const char*_tmp95B;Cyc_Parse_err(((
_tmp95B="attributes on datatypes not supported",_tag_dyneither(_tmp95B,sizeof(
char),38))),loc);}{struct Cyc_Absyn_Datatype_d_struct*_tmp961;struct Cyc_Absyn_Datatype_d_struct
_tmp960;struct Cyc_List_List*_tmp95F;struct Cyc_List_List*_tmp1C0=(_tmp95F=
_cycalloc(sizeof(*_tmp95F)),((_tmp95F->hd=Cyc_Absyn_new_decl((void*)((_tmp961=
_cycalloc(sizeof(*_tmp961)),((_tmp961[0]=((_tmp960.tag=7,((_tmp960.f1=_tmp19C,
_tmp960)))),_tmp961)))),loc),((_tmp95F->tl=0,_tmp95F)))));_npop_handler(0);
return _tmp1C0;};_LLB7: {struct Cyc_Absyn_AggrType_struct*_tmp19D=(struct Cyc_Absyn_AggrType_struct*)
_tmp18A;if(_tmp19D->tag != 12)goto _LLB9;else{_tmp19E=_tmp19D->f1;_tmp19F=_tmp19E.aggr_info;
if((_tmp19F.UnknownAggr).tag != 1)goto _LLB9;_tmp1A0=(struct _tuple3)(_tmp19F.UnknownAggr).val;
_tmp1A1=_tmp1A0.f1;_tmp1A2=_tmp1A0.f2;_tmp1A3=_tmp19E.targs;}}_LLB8: {struct Cyc_List_List*
_tmp1C1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp1A3);struct Cyc_Absyn_Aggrdecl*_tmp962;struct Cyc_Absyn_Aggrdecl*_tmp1C2=(
_tmp962=_cycalloc(sizeof(*_tmp962)),((_tmp962->kind=_tmp1A1,((_tmp962->sc=s,((
_tmp962->name=_tmp1A2,((_tmp962->tvs=_tmp1C1,((_tmp962->impl=0,((_tmp962->attributes=
0,_tmp962)))))))))))));if(_tmp181 != 0){const char*_tmp963;Cyc_Parse_err(((_tmp963="bad attributes on type declaration",
_tag_dyneither(_tmp963,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp969;struct Cyc_Absyn_Aggr_d_struct _tmp968;struct Cyc_List_List*_tmp967;struct
Cyc_List_List*_tmp1C7=(_tmp967=_cycalloc(sizeof(*_tmp967)),((_tmp967->hd=Cyc_Absyn_new_decl((
void*)((_tmp969=_cycalloc(sizeof(*_tmp969)),((_tmp969[0]=((_tmp968.tag=6,((
_tmp968.f1=_tmp1C2,_tmp968)))),_tmp969)))),loc),((_tmp967->tl=0,_tmp967)))));
_npop_handler(0);return _tmp1C7;};}_LLB9: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1A4=(struct Cyc_Absyn_DatatypeType_struct*)_tmp18A;if(_tmp1A4->tag != 3)goto
_LLBB;else{_tmp1A5=_tmp1A4->f1;_tmp1A6=_tmp1A5.datatype_info;if((_tmp1A6.KnownDatatype).tag
!= 2)goto _LLBB;_tmp1A7=(struct Cyc_Absyn_Datatypedecl**)(_tmp1A6.KnownDatatype).val;}}
_LLBA: if(_tmp181 != 0){const char*_tmp96A;Cyc_Parse_err(((_tmp96A="bad attributes on datatype",
_tag_dyneither(_tmp96A,sizeof(char),27))),loc);}{struct Cyc_Absyn_Datatype_d_struct*
_tmp970;struct Cyc_Absyn_Datatype_d_struct _tmp96F;struct Cyc_List_List*_tmp96E;
struct Cyc_List_List*_tmp1CD=(_tmp96E=_cycalloc(sizeof(*_tmp96E)),((_tmp96E->hd=
Cyc_Absyn_new_decl((void*)((_tmp970=_cycalloc(sizeof(*_tmp970)),((_tmp970[0]=((
_tmp96F.tag=7,((_tmp96F.f1=*_tmp1A7,_tmp96F)))),_tmp970)))),loc),((_tmp96E->tl=0,
_tmp96E)))));_npop_handler(0);return _tmp1CD;};_LLBB: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1A8=(struct Cyc_Absyn_DatatypeType_struct*)_tmp18A;if(_tmp1A8->tag != 3)goto
_LLBD;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A9.datatype_info;if((_tmp1AA.UnknownDatatype).tag
!= 1)goto _LLBD;_tmp1AB=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1AA.UnknownDatatype).val;
_tmp1AC=_tmp1AB.name;_tmp1AD=_tmp1AB.is_extensible;_tmp1AE=_tmp1A9.targs;}}_LLBC: {
struct Cyc_List_List*_tmp1CE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp1AE);struct Cyc_Absyn_Decl*_tmp1CF=
Cyc_Absyn_datatype_decl(s,_tmp1AC,_tmp1CE,0,_tmp1AD,loc);if(_tmp181 != 0){const
char*_tmp971;Cyc_Parse_err(((_tmp971="bad attributes on datatype",_tag_dyneither(
_tmp971,sizeof(char),27))),loc);}{struct Cyc_List_List*_tmp972;struct Cyc_List_List*
_tmp1D2=(_tmp972=_cycalloc(sizeof(*_tmp972)),((_tmp972->hd=_tmp1CF,((_tmp972->tl=
0,_tmp972)))));_npop_handler(0);return _tmp1D2;};}_LLBD: {struct Cyc_Absyn_EnumType_struct*
_tmp1AF=(struct Cyc_Absyn_EnumType_struct*)_tmp18A;if(_tmp1AF->tag != 14)goto _LLBF;
else{_tmp1B0=_tmp1AF->f1;}}_LLBE: {struct Cyc_Absyn_Enumdecl*_tmp973;struct Cyc_Absyn_Enumdecl*
_tmp1D3=(_tmp973=_cycalloc(sizeof(*_tmp973)),((_tmp973->sc=s,((_tmp973->name=
_tmp1B0,((_tmp973->fields=0,_tmp973)))))));if(_tmp181 != 0){const char*_tmp974;Cyc_Parse_err(((
_tmp974="bad attributes on enum",_tag_dyneither(_tmp974,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp97E;struct Cyc_Absyn_Enum_d_struct _tmp97D;struct Cyc_Absyn_Enum_d_struct*
_tmp97C;struct Cyc_List_List*_tmp97B;struct Cyc_List_List*_tmp1D9=(_tmp97B=
_cycalloc(sizeof(*_tmp97B)),((_tmp97B->hd=((_tmp97E=_cycalloc(sizeof(*_tmp97E)),((
_tmp97E->r=(void*)((_tmp97C=_cycalloc(sizeof(*_tmp97C)),((_tmp97C[0]=((_tmp97D.tag=
8,((_tmp97D.f1=_tmp1D3,_tmp97D)))),_tmp97C)))),((_tmp97E->loc=loc,_tmp97E)))))),((
_tmp97B->tl=0,_tmp97B)))));_npop_handler(0);return _tmp1D9;};}_LLBF: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1B1=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp18A;if(_tmp1B1->tag != 15)goto
_LLC1;else{_tmp1B2=_tmp1B1->f1;}}_LLC0: {struct Cyc_Core_Opt*_tmp981;struct Cyc_Absyn_Enumdecl*
_tmp980;struct Cyc_Absyn_Enumdecl*_tmp1DB=(_tmp980=_cycalloc(sizeof(*_tmp980)),((
_tmp980->sc=s,((_tmp980->name=Cyc_Parse_gensym_enum(),((_tmp980->fields=((
_tmp981=_cycalloc(sizeof(*_tmp981)),((_tmp981->v=_tmp1B2,_tmp981)))),_tmp980)))))));
if(_tmp181 != 0){const char*_tmp982;Cyc_Parse_err(((_tmp982="bad attributes on enum",
_tag_dyneither(_tmp982,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp98C;
struct Cyc_Absyn_Enum_d_struct _tmp98B;struct Cyc_Absyn_Enum_d_struct*_tmp98A;
struct Cyc_List_List*_tmp989;struct Cyc_List_List*_tmp1E1=(_tmp989=_cycalloc(
sizeof(*_tmp989)),((_tmp989->hd=((_tmp98C=_cycalloc(sizeof(*_tmp98C)),((_tmp98C->r=(
void*)((_tmp98A=_cycalloc(sizeof(*_tmp98A)),((_tmp98A[0]=((_tmp98B.tag=8,((
_tmp98B.f1=_tmp1DB,_tmp98B)))),_tmp98A)))),((_tmp98C->loc=loc,_tmp98C)))))),((
_tmp989->tl=0,_tmp989)))));_npop_handler(0);return _tmp1E1;};}_LLC1:;_LLC2:{const
char*_tmp98D;Cyc_Parse_err(((_tmp98D="missing declarator",_tag_dyneither(_tmp98D,
sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1E5=0;_npop_handler(0);return
_tmp1E5;};_LLB0:;}else{struct Cyc_List_List*_tmp1E6=Cyc_Parse_apply_tmss(mkrgn,
_tmp17F,_tmp189,_tmp187,_tmp181);if(istypedef){if(!exps_empty){const char*_tmp98E;
Cyc_Parse_err(((_tmp98E="initializer in typedef declaration",_tag_dyneither(
_tmp98E,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp1E6);struct Cyc_List_List*_tmp1E8=decls;_npop_handler(0);return _tmp1E8;};}
else{struct Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp1E9=_tmp1E6;for(0;
_tmp1E9 != 0;(_tmp1E9=_tmp1E9->tl,_tmp188=_tmp188->tl)){struct _tuple12 _tmp1EB;
struct _tuple1*_tmp1EC;struct Cyc_Absyn_Tqual _tmp1ED;void*_tmp1EE;struct Cyc_List_List*
_tmp1EF;struct Cyc_List_List*_tmp1F0;struct _tuple12*_tmp1EA=(struct _tuple12*)
_tmp1E9->hd;_tmp1EB=*_tmp1EA;_tmp1EC=_tmp1EB.f1;_tmp1ED=_tmp1EB.f2;_tmp1EE=
_tmp1EB.f3;_tmp1EF=_tmp1EB.f4;_tmp1F0=_tmp1EB.f5;if(_tmp1EF != 0){const char*
_tmp991;void*_tmp990;(_tmp990=0,Cyc_Tcutil_warn(loc,((_tmp991="bad type params, ignoring",
_tag_dyneither(_tmp991,sizeof(char),26))),_tag_dyneither(_tmp990,sizeof(void*),0)));}
if(_tmp188 == 0){const char*_tmp994;void*_tmp993;(_tmp993=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp994="unexpected NULL in parse!",_tag_dyneither(_tmp994,sizeof(char),26))),
_tag_dyneither(_tmp993,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*_tmp1F5=(struct
Cyc_Absyn_Exp*)_tmp188->hd;struct Cyc_Absyn_Vardecl*_tmp1F6=Cyc_Absyn_new_vardecl(
_tmp1EC,_tmp1EE,_tmp1F5);_tmp1F6->tq=_tmp1ED;_tmp1F6->sc=s;_tmp1F6->attributes=
_tmp1F0;{struct Cyc_Absyn_Var_d_struct*_tmp99A;struct Cyc_Absyn_Var_d_struct
_tmp999;struct Cyc_Absyn_Decl*_tmp998;struct Cyc_Absyn_Decl*_tmp1F7=(_tmp998=
_cycalloc(sizeof(*_tmp998)),((_tmp998->r=(void*)((_tmp99A=_cycalloc(sizeof(*
_tmp99A)),((_tmp99A[0]=((_tmp999.tag=0,((_tmp999.f1=_tmp1F6,_tmp999)))),_tmp99A)))),((
_tmp998->loc=loc,_tmp998)))));struct Cyc_List_List*_tmp99B;decls=((_tmp99B=
_cycalloc(sizeof(*_tmp99B)),((_tmp99B->hd=_tmp1F7,((_tmp99B->tl=decls,_tmp99B))))));};};}}{
struct Cyc_List_List*_tmp1FC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
decls);_npop_handler(0);return _tmp1FC;};}}};};};};};_pop_region(mkrgn);}static
struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,struct Cyc_Position_Segment*
loc){if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct
_dyneither_ptr)s)== 2)switch(*((const char*)_check_dyneither_subscript(s,sizeof(
char),0))){case 'A': _LLC3: return& Cyc_Tcutil_ak;case 'M': _LLC4: return& Cyc_Tcutil_mk;
case 'B': _LLC5: return& Cyc_Tcutil_bk;case 'R': _LLC6: return& Cyc_Tcutil_rk;case 'E':
_LLC7: return& Cyc_Tcutil_ek;case 'I': _LLC8: return& Cyc_Tcutil_ik;case 'U': _LLC9:
switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))){case 'R':
_LLCB: return& Cyc_Tcutil_urk;case 'A': _LLCC: return& Cyc_Tcutil_uak;case 'M': _LLCD:
return& Cyc_Tcutil_umk;case 'B': _LLCE: return& Cyc_Tcutil_ubk;default: _LLCF: break;}
break;case 'T': _LLCA: switch(*((const char*)_check_dyneither_subscript(s,sizeof(
char),1))){case 'R': _LLD2: return& Cyc_Tcutil_trk;case 'A': _LLD3: return& Cyc_Tcutil_tak;
case 'M': _LLD4: return& Cyc_Tcutil_tmk;case 'B': _LLD5: return& Cyc_Tcutil_tbk;default:
_LLD6: break;}break;default: _LLD1: break;}{const char*_tmp9A0;void*_tmp99F[2];
struct Cyc_String_pa_struct _tmp99E;struct Cyc_Int_pa_struct _tmp99D;Cyc_Parse_err((
struct _dyneither_ptr)((_tmp99D.tag=1,((_tmp99D.f1=(unsigned long)((int)Cyc_strlen((
struct _dyneither_ptr)s)),((_tmp99E.tag=0,((_tmp99E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s),((_tmp99F[0]=& _tmp99E,((_tmp99F[1]=& _tmp99D,Cyc_aprintf(((
_tmp9A0="bad kind: %s; strlen=%d",_tag_dyneither(_tmp9A0,sizeof(char),24))),
_tag_dyneither(_tmp99F,sizeof(void*),2)))))))))))))),loc);}return& Cyc_Tcutil_bk;}
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*_tmp201=
p->r;struct _tuple1*_tmp203;struct Cyc_Absyn_Vardecl*_tmp205;struct Cyc_Absyn_Pat*
_tmp206;struct Cyc_Absyn_Pat _tmp207;void*_tmp208;struct Cyc_Absyn_Pat*_tmp20B;
enum Cyc_Absyn_Sign _tmp20E;int _tmp20F;char _tmp211;struct _dyneither_ptr _tmp213;
struct _tuple1*_tmp215;struct Cyc_List_List*_tmp216;int _tmp217;struct Cyc_Absyn_Exp*
_tmp219;_LLDA: {struct Cyc_Absyn_UnknownId_p_struct*_tmp202=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp201;if(_tmp202->tag != 14)goto _LLDC;else{_tmp203=_tmp202->f1;}}_LLDB: {struct
Cyc_Absyn_UnknownId_e_struct _tmp9A3;struct Cyc_Absyn_UnknownId_e_struct*_tmp9A2;
return Cyc_Absyn_new_exp((void*)((_tmp9A2=_cycalloc(sizeof(*_tmp9A2)),((_tmp9A2[0]=((
_tmp9A3.tag=2,((_tmp9A3.f1=_tmp203,_tmp9A3)))),_tmp9A2)))),p->loc);}_LLDC: {
struct Cyc_Absyn_Reference_p_struct*_tmp204=(struct Cyc_Absyn_Reference_p_struct*)
_tmp201;if(_tmp204->tag != 2)goto _LLDE;else{_tmp205=_tmp204->f1;_tmp206=_tmp204->f2;
_tmp207=*_tmp206;_tmp208=_tmp207.r;{struct Cyc_Absyn_Wild_p_struct*_tmp209=(
struct Cyc_Absyn_Wild_p_struct*)_tmp208;if(_tmp209->tag != 0)goto _LLDE;};}}_LLDD: {
struct Cyc_Absyn_UnknownId_e_struct _tmp9A6;struct Cyc_Absyn_UnknownId_e_struct*
_tmp9A5;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmp9A5=_cycalloc(
sizeof(*_tmp9A5)),((_tmp9A5[0]=((_tmp9A6.tag=2,((_tmp9A6.f1=_tmp205->name,
_tmp9A6)))),_tmp9A5)))),p->loc),p->loc);}_LLDE: {struct Cyc_Absyn_Pointer_p_struct*
_tmp20A=(struct Cyc_Absyn_Pointer_p_struct*)_tmp201;if(_tmp20A->tag != 5)goto _LLE0;
else{_tmp20B=_tmp20A->f1;}}_LLDF: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(
_tmp20B),p->loc);_LLE0: {struct Cyc_Absyn_Null_p_struct*_tmp20C=(struct Cyc_Absyn_Null_p_struct*)
_tmp201;if(_tmp20C->tag != 8)goto _LLE2;}_LLE1: return Cyc_Absyn_null_exp(p->loc);
_LLE2: {struct Cyc_Absyn_Int_p_struct*_tmp20D=(struct Cyc_Absyn_Int_p_struct*)
_tmp201;if(_tmp20D->tag != 9)goto _LLE4;else{_tmp20E=_tmp20D->f1;_tmp20F=_tmp20D->f2;}}
_LLE3: return Cyc_Absyn_int_exp(_tmp20E,_tmp20F,p->loc);_LLE4: {struct Cyc_Absyn_Char_p_struct*
_tmp210=(struct Cyc_Absyn_Char_p_struct*)_tmp201;if(_tmp210->tag != 10)goto _LLE6;
else{_tmp211=_tmp210->f1;}}_LLE5: return Cyc_Absyn_char_exp(_tmp211,p->loc);_LLE6: {
struct Cyc_Absyn_Float_p_struct*_tmp212=(struct Cyc_Absyn_Float_p_struct*)_tmp201;
if(_tmp212->tag != 11)goto _LLE8;else{_tmp213=_tmp212->f1;}}_LLE7: return Cyc_Absyn_float_exp(
_tmp213,p->loc);_LLE8: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp214=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp201;if(_tmp214->tag != 15)goto _LLEA;else{_tmp215=_tmp214->f1;_tmp216=_tmp214->f2;
_tmp217=_tmp214->f3;if(_tmp217 != 0)goto _LLEA;}}_LLE9: {struct Cyc_Absyn_UnknownId_e_struct
_tmp9A9;struct Cyc_Absyn_UnknownId_e_struct*_tmp9A8;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmp9A8=_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8[0]=((_tmp9A9.tag=2,((
_tmp9A9.f1=_tmp215,_tmp9A9)))),_tmp9A8)))),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp216);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLEA: {struct Cyc_Absyn_Exp_p_struct*_tmp218=(struct Cyc_Absyn_Exp_p_struct*)
_tmp201;if(_tmp218->tag != 16)goto _LLEC;else{_tmp219=_tmp218->f1;}}_LLEB: return
_tmp219;_LLEC:;_LLED:{const char*_tmp9AA;Cyc_Parse_err(((_tmp9AA="cannot mix patterns and expressions in case",
_tag_dyneither(_tmp9AA,sizeof(char),44))),p->loc);}return Cyc_Absyn_null_exp(p->loc);
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
_union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp222[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp222,
_tmp222,_tmp222 + 14}};struct _tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){struct
_tuple6 yyzzz;{union Cyc_YYSTYPE _tmp223=yy1;struct _tuple6 _tmp224;_LLEF: if((_tmp223.Int_tok).tag
!= 1)goto _LLF1;_tmp224=(struct _tuple6)(_tmp223.Int_tok).val;_LLF0: yyzzz=_tmp224;
goto _LLEE;_LLF1:;_LLF2:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLEE:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){union Cyc_YYSTYPE _tmp9AB;
return((_tmp9AB.Int_tok).val=yy1,(((_tmp9AB.Int_tok).tag=1,_tmp9AB)));}static
char _tmp227[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp227,_tmp227,_tmp227 + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE
yy1){char yyzzz;{union Cyc_YYSTYPE _tmp228=yy1;char _tmp229;_LLF4: if((_tmp228.Char_tok).tag
!= 2)goto _LLF6;_tmp229=(char)(_tmp228.Char_tok).val;_LLF5: yyzzz=_tmp229;goto
_LLF3;_LLF6:;_LLF7:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLF3:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9AC;return((_tmp9AC.Char_tok).val=
yy1,(((_tmp9AC.Char_tok).tag=2,_tmp9AC)));}static char _tmp22C[13]="string_t<`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{
_tmp22C,_tmp22C,_tmp22C + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp22D=yy1;struct _dyneither_ptr
_tmp22E;_LLF9: if((_tmp22D.String_tok).tag != 3)goto _LLFB;_tmp22E=(struct
_dyneither_ptr)(_tmp22D.String_tok).val;_LLFA: yyzzz=_tmp22E;goto _LLF8;_LLFB:;
_LLFC:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLF8:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9AD;return((_tmp9AD.String_tok).val=
yy1,(((_tmp9AD.String_tok).tag=3,_tmp9AD)));}static char _tmp231[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp231,
_tmp231,_tmp231 + 56}};struct _tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct
_tuple18*yyzzz;{union Cyc_YYSTYPE _tmp232=yy1;struct _tuple18*_tmp233;_LLFE: if((
_tmp232.YY1).tag != 7)goto _LL100;_tmp233=(struct _tuple18*)(_tmp232.YY1).val;_LLFF:
yyzzz=_tmp233;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)& Cyc_yyfail_YY1);_LLFD:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY1(struct _tuple18*yy1){union Cyc_YYSTYPE _tmp9AE;
return((_tmp9AE.YY1).val=yy1,(((_tmp9AE.YY1).tag=7,_tmp9AE)));}static char _tmp236[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp236,_tmp236,_tmp236 + 19}};union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE
yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp237=yy1;union Cyc_Absyn_Constraint*
_tmp238;_LL103: if((_tmp237.YY2).tag != 8)goto _LL105;_tmp238=(union Cyc_Absyn_Constraint*)(
_tmp237.YY2).val;_LL104: yyzzz=_tmp238;goto _LL102;_LL105:;_LL106:(int)_throw((
void*)& Cyc_yyfail_YY2);_LL102:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*
yy1){union Cyc_YYSTYPE _tmp9AF;return((_tmp9AF.YY2).val=yy1,(((_tmp9AF.YY2).tag=8,
_tmp9AF)));}static char _tmp23B[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp23B,_tmp23B,_tmp23B + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE _tmp23C=yy1;
struct Cyc_Absyn_Exp*_tmp23D;_LL108: if((_tmp23C.YY3).tag != 9)goto _LL10A;_tmp23D=(
struct Cyc_Absyn_Exp*)(_tmp23C.YY3).val;_LL109: yyzzz=_tmp23D;goto _LL107;_LL10A:;
_LL10B:(int)_throw((void*)& Cyc_yyfail_YY3);_LL107:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY3(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9B0;return((_tmp9B0.YY3).val=
yy1,(((_tmp9B0.YY3).tag=9,_tmp9B0)));}static char _tmp240[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp240,_tmp240,
_tmp240 + 10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE _tmp241=yy1;struct Cyc_Absyn_Exp*_tmp242;_LL10D: if((
_tmp241.YY4).tag != 10)goto _LL10F;_tmp242=(struct Cyc_Absyn_Exp*)(_tmp241.YY4).val;
_LL10E: yyzzz=_tmp242;goto _LL10C;_LL10F:;_LL110:(int)_throw((void*)& Cyc_yyfail_YY4);
_LL10C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE
_tmp9B1;return((_tmp9B1.YY4).val=yy1,(((_tmp9B1.YY4).tag=10,_tmp9B1)));}static
char _tmp245[17]="list_t<exp_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={
Cyc_Core_Failure,{_tmp245,_tmp245,_tmp245 + 17}};struct Cyc_List_List*Cyc_yyget_YY5(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp246=yy1;
struct Cyc_List_List*_tmp247;_LL112: if((_tmp246.YY5).tag != 11)goto _LL114;_tmp247=(
struct Cyc_List_List*)(_tmp246.YY5).val;_LL113: yyzzz=_tmp247;goto _LL111;_LL114:;
_LL115:(int)_throw((void*)& Cyc_yyfail_YY5);_LL111:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B2;return((_tmp9B2.YY5).val=
yy1,(((_tmp9B2.YY5).tag=11,_tmp9B2)));}static char _tmp24A[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp24A,
_tmp24A,_tmp24A + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp24B=yy1;struct Cyc_List_List*
_tmp24C;_LL117: if((_tmp24B.YY6).tag != 12)goto _LL119;_tmp24C=(struct Cyc_List_List*)(
_tmp24B.YY6).val;_LL118: yyzzz=_tmp24C;goto _LL116;_LL119:;_LL11A:(int)_throw((
void*)& Cyc_yyfail_YY6);_LL116:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9B3;return((_tmp9B3.YY6).val=yy1,(((_tmp9B3.YY6).tag=12,
_tmp9B3)));}static char _tmp24F[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp24F,_tmp24F,_tmp24F + 9}};enum Cyc_Absyn_Primop
Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Primop yyzzz;{union Cyc_YYSTYPE
_tmp250=yy1;enum Cyc_Absyn_Primop _tmp251;_LL11C: if((_tmp250.YY7).tag != 13)goto
_LL11E;_tmp251=(enum Cyc_Absyn_Primop)(_tmp250.YY7).val;_LL11D: yyzzz=_tmp251;
goto _LL11B;_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY7);_LL11B:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9B4;
return((_tmp9B4.YY7).val=yy1,(((_tmp9B4.YY7).tag=13,_tmp9B4)));}static char
_tmp254[19]="opt_t<primop_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={
Cyc_Core_Failure,{_tmp254,_tmp254,_tmp254 + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(
union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp255=yy1;
struct Cyc_Core_Opt*_tmp256;_LL121: if((_tmp255.YY8).tag != 14)goto _LL123;_tmp256=(
struct Cyc_Core_Opt*)(_tmp255.YY8).val;_LL122: yyzzz=_tmp256;goto _LL120;_LL123:;
_LL124:(int)_throw((void*)& Cyc_yyfail_YY8);_LL120:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY8(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9B5;return((_tmp9B5.YY8).val=
yy1,(((_tmp9B5.YY8).tag=14,_tmp9B5)));}static char _tmp259[7]="qvar_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp259,
_tmp259,_tmp259 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp25A=yy1;struct _tuple1*_tmp25B;_LL126:
if((_tmp25A.QualId_tok).tag != 5)goto _LL128;_tmp25B=(struct _tuple1*)(_tmp25A.QualId_tok).val;
_LL127: yyzzz=_tmp25B;goto _LL125;_LL128:;_LL129:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL125:;}return yyzzz;}union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple1*yy1){union Cyc_YYSTYPE
_tmp9B6;return((_tmp9B6.QualId_tok).val=yy1,(((_tmp9B6.QualId_tok).tag=5,_tmp9B6)));}
static char _tmp25E[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY9={
Cyc_Core_Failure,{_tmp25E,_tmp25E,_tmp25E + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE _tmp25F=yy1;
struct Cyc_Absyn_Stmt*_tmp260;_LL12B: if((_tmp25F.YY9).tag != 15)goto _LL12D;_tmp260=(
struct Cyc_Absyn_Stmt*)(_tmp25F.YY9).val;_LL12C: yyzzz=_tmp260;goto _LL12A;_LL12D:;
_LL12E:(int)_throw((void*)& Cyc_yyfail_YY9);_LL12A:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9B7;return((_tmp9B7.YY9).val=
yy1,(((_tmp9B7.YY9).tag=15,_tmp9B7)));}static char _tmp263[27]="list_t<switch_clause_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp263,
_tmp263,_tmp263 + 27}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp264=yy1;struct Cyc_List_List*
_tmp265;_LL130: if((_tmp264.YY10).tag != 16)goto _LL132;_tmp265=(struct Cyc_List_List*)(
_tmp264.YY10).val;_LL131: yyzzz=_tmp265;goto _LL12F;_LL132:;_LL133:(int)_throw((
void*)& Cyc_yyfail_YY10);_LL12F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9B8;return((_tmp9B8.YY10).val=yy1,(((_tmp9B8.YY10).tag=
16,_tmp9B8)));}static char _tmp268[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp268,_tmp268,_tmp268 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE
_tmp269=yy1;struct Cyc_Absyn_Pat*_tmp26A;_LL135: if((_tmp269.YY11).tag != 17)goto
_LL137;_tmp26A=(struct Cyc_Absyn_Pat*)(_tmp269.YY11).val;_LL136: yyzzz=_tmp26A;
goto _LL134;_LL137:;_LL138:(int)_throw((void*)& Cyc_yyfail_YY11);_LL134:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9B9;
return((_tmp9B9.YY11).val=yy1,(((_tmp9B9.YY11).tag=17,_tmp9B9)));}static char
_tmp26D[28]="$(list_t<pat_t,`H>,bool)@`H";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={
Cyc_Core_Failure,{_tmp26D,_tmp26D,_tmp26D + 28}};struct _tuple19*Cyc_yyget_YY12(
union Cyc_YYSTYPE yy1){struct _tuple19*yyzzz;{union Cyc_YYSTYPE _tmp26E=yy1;struct
_tuple19*_tmp26F;_LL13A: if((_tmp26E.YY12).tag != 18)goto _LL13C;_tmp26F=(struct
_tuple19*)(_tmp26E.YY12).val;_LL13B: yyzzz=_tmp26F;goto _LL139;_LL13C:;_LL13D:(int)
_throw((void*)& Cyc_yyfail_YY12);_LL139:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY12(
struct _tuple19*yy1){union Cyc_YYSTYPE _tmp9BA;return((_tmp9BA.YY12).val=yy1,(((
_tmp9BA.YY12).tag=18,_tmp9BA)));}static char _tmp272[17]="list_t<pat_t,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp272,_tmp272,
_tmp272 + 17}};struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp273=yy1;struct Cyc_List_List*_tmp274;_LL13F: if((
_tmp273.YY13).tag != 19)goto _LL141;_tmp274=(struct Cyc_List_List*)(_tmp273.YY13).val;
_LL140: yyzzz=_tmp274;goto _LL13E;_LL141:;_LL142:(int)_throw((void*)& Cyc_yyfail_YY13);
_LL13E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9BB;return((_tmp9BB.YY13).val=yy1,(((_tmp9BB.YY13).tag=19,_tmp9BB)));}static
char _tmp277[36]="$(list_t<designator_t,`H>,pat_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp277,_tmp277,_tmp277 + 36}};struct _tuple20*
Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){struct _tuple20*yyzzz;{union Cyc_YYSTYPE
_tmp278=yy1;struct _tuple20*_tmp279;_LL144: if((_tmp278.YY14).tag != 20)goto _LL146;
_tmp279=(struct _tuple20*)(_tmp278.YY14).val;_LL145: yyzzz=_tmp279;goto _LL143;
_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY14);_LL143:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY14(struct _tuple20*yy1){union Cyc_YYSTYPE _tmp9BC;return((_tmp9BC.YY14).val=
yy1,(((_tmp9BC.YY14).tag=20,_tmp9BC)));}static char _tmp27C[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp27C,
_tmp27C,_tmp27C + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp27D=yy1;struct Cyc_List_List*
_tmp27E;_LL149: if((_tmp27D.YY15).tag != 21)goto _LL14B;_tmp27E=(struct Cyc_List_List*)(
_tmp27D.YY15).val;_LL14A: yyzzz=_tmp27E;goto _LL148;_LL14B:;_LL14C:(int)_throw((
void*)& Cyc_yyfail_YY15);_LL148:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9BD;return((_tmp9BD.YY15).val=yy1,(((_tmp9BD.YY15).tag=
21,_tmp9BD)));}static char _tmp281[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp281,
_tmp281,_tmp281 + 58}};struct _tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct
_tuple19*yyzzz;{union Cyc_YYSTYPE _tmp282=yy1;struct _tuple19*_tmp283;_LL14E: if((
_tmp282.YY16).tag != 22)goto _LL150;_tmp283=(struct _tuple19*)(_tmp282.YY16).val;
_LL14F: yyzzz=_tmp283;goto _LL14D;_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY16);
_LL14D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY16(struct _tuple19*yy1){union Cyc_YYSTYPE
_tmp9BE;return((_tmp9BE.YY16).val=yy1,(((_tmp9BE.YY16).tag=22,_tmp9BE)));}static
char _tmp286[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp286,_tmp286,_tmp286 + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp287=yy1;
struct Cyc_Absyn_Fndecl*_tmp288;_LL153: if((_tmp287.YY17).tag != 23)goto _LL155;
_tmp288=(struct Cyc_Absyn_Fndecl*)(_tmp287.YY17).val;_LL154: yyzzz=_tmp288;goto
_LL152;_LL155:;_LL156:(int)_throw((void*)& Cyc_yyfail_YY17);_LL152:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9BF;
return((_tmp9BF.YY17).val=yy1,(((_tmp9BF.YY17).tag=23,_tmp9BF)));}static char
_tmp28B[18]="list_t<decl_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp28B,_tmp28B,_tmp28B + 18}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp28C=yy1;
struct Cyc_List_List*_tmp28D;_LL158: if((_tmp28C.YY18).tag != 24)goto _LL15A;_tmp28D=(
struct Cyc_List_List*)(_tmp28C.YY18).val;_LL159: yyzzz=_tmp28D;goto _LL157;_LL15A:;
_LL15B:(int)_throw((void*)& Cyc_yyfail_YY18);_LL157:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C0;return((_tmp9C0.YY18).val=
yy1,(((_tmp9C0.YY18).tag=24,_tmp9C0)));}static char _tmp290[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp290,
_tmp290,_tmp290 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp291=yy1;struct
Cyc_Parse_Declaration_spec*_tmp292;_LL15D: if((_tmp291.YY19).tag != 25)goto _LL15F;
_tmp292=(struct Cyc_Parse_Declaration_spec*)(_tmp291.YY19).val;_LL15E: yyzzz=
_tmp292;goto _LL15C;_LL15F:;_LL160:(int)_throw((void*)& Cyc_yyfail_YY19);_LL15C:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){
union Cyc_YYSTYPE _tmp9C1;return((_tmp9C1.YY19).val=yy1,(((_tmp9C1.YY19).tag=25,
_tmp9C1)));}static char _tmp295[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp295,_tmp295,_tmp295 + 29}};struct _tuple21*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE
_tmp296=yy1;struct _tuple21*_tmp297;_LL162: if((_tmp296.YY20).tag != 26)goto _LL164;
_tmp297=(struct _tuple21*)(_tmp296.YY20).val;_LL163: yyzzz=_tmp297;goto _LL161;
_LL164:;_LL165:(int)_throw((void*)& Cyc_yyfail_YY20);_LL161:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY20(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9C2;return((_tmp9C2.YY20).val=
yy1,(((_tmp9C2.YY20).tag=26,_tmp9C2)));}static char _tmp29A[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp29A,
_tmp29A,_tmp29A + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp29B=yy1;struct Cyc_List_List*
_tmp29C;_LL167: if((_tmp29B.YY21).tag != 27)goto _LL169;_tmp29C=(struct Cyc_List_List*)(
_tmp29B.YY21).val;_LL168: yyzzz=_tmp29C;goto _LL166;_LL169:;_LL16A:(int)_throw((
void*)& Cyc_yyfail_YY21);_LL166:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9C3;return((_tmp9C3.YY21).val=yy1,(((_tmp9C3.YY21).tag=
27,_tmp9C3)));}static char _tmp29F[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp29F,_tmp29F,_tmp29F + 16}};enum Cyc_Parse_Storage_class
Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){enum Cyc_Parse_Storage_class yyzzz;{union Cyc_YYSTYPE
_tmp2A0=yy1;enum Cyc_Parse_Storage_class _tmp2A1;_LL16C: if((_tmp2A0.YY22).tag != 
28)goto _LL16E;_tmp2A1=(enum Cyc_Parse_Storage_class)(_tmp2A0.YY22).val;_LL16D:
yyzzz=_tmp2A1;goto _LL16B;_LL16E:;_LL16F:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL16B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class yy1){
union Cyc_YYSTYPE _tmp9C4;return((_tmp9C4.YY22).val=yy1,(((_tmp9C4.YY22).tag=28,
_tmp9C4)));}static char _tmp2A4[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp2A4,_tmp2A4,_tmp2A4 + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2A5=yy1;void*_tmp2A6;_LL171:
if((_tmp2A5.YY23).tag != 29)goto _LL173;_tmp2A6=(void*)(_tmp2A5.YY23).val;_LL172:
yyzzz=_tmp2A6;goto _LL170;_LL173:;_LL174:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL170:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(void*yy1){union Cyc_YYSTYPE _tmp9C5;
return((_tmp9C5.YY23).val=yy1,(((_tmp9C5.YY23).tag=29,_tmp9C5)));}static char
_tmp2A9[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={
Cyc_Core_Failure,{_tmp2A9,_tmp2A9,_tmp2A9 + 12}};enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(
union Cyc_YYSTYPE yy1){enum Cyc_Absyn_AggrKind yyzzz;{union Cyc_YYSTYPE _tmp2AA=yy1;
enum Cyc_Absyn_AggrKind _tmp2AB;_LL176: if((_tmp2AA.YY24).tag != 30)goto _LL178;
_tmp2AB=(enum Cyc_Absyn_AggrKind)(_tmp2AA.YY24).val;_LL177: yyzzz=_tmp2AB;goto
_LL175;_LL178:;_LL179:(int)_throw((void*)& Cyc_yyfail_YY24);_LL175:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9C6;
return((_tmp9C6.YY24).val=yy1,(((_tmp9C6.YY24).tag=30,_tmp9C6)));}static char
_tmp2AE[8]="tqual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{
_tmp2AE,_tmp2AE,_tmp2AE + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE _tmp2AF=yy1;struct Cyc_Absyn_Tqual
_tmp2B0;_LL17B: if((_tmp2AF.YY25).tag != 31)goto _LL17D;_tmp2B0=(struct Cyc_Absyn_Tqual)(
_tmp2AF.YY25).val;_LL17C: yyzzz=_tmp2B0;goto _LL17A;_LL17D:;_LL17E:(int)_throw((
void*)& Cyc_yyfail_YY25);_LL17A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual
yy1){union Cyc_YYSTYPE _tmp9C7;return((_tmp9C7.YY25).val=yy1,(((_tmp9C7.YY25).tag=
31,_tmp9C7)));}static char _tmp2B3[23]="list_t<aggrfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp2B3,_tmp2B3,_tmp2B3 + 23}};struct Cyc_List_List*
Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2B4=yy1;struct Cyc_List_List*_tmp2B5;_LL180: if((_tmp2B4.YY26).tag != 32)goto
_LL182;_tmp2B5=(struct Cyc_List_List*)(_tmp2B4.YY26).val;_LL181: yyzzz=_tmp2B5;
goto _LL17F;_LL182:;_LL183:(int)_throw((void*)& Cyc_yyfail_YY26);_LL17F:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C8;
return((_tmp9C8.YY26).val=yy1,(((_tmp9C8.YY26).tag=32,_tmp9C8)));}static char
_tmp2B8[34]="list_t<list_t<aggrfield_t,`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2B8,_tmp2B8,_tmp2B8 + 34}};struct Cyc_List_List*
Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2B9=yy1;struct Cyc_List_List*_tmp2BA;_LL185: if((_tmp2B9.YY27).tag != 33)goto
_LL187;_tmp2BA=(struct Cyc_List_List*)(_tmp2B9.YY27).val;_LL186: yyzzz=_tmp2BA;
goto _LL184;_LL187:;_LL188:(int)_throw((void*)& Cyc_yyfail_YY27);_LL184:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C9;
return((_tmp9C9.YY27).val=yy1,(((_tmp9C9.YY27).tag=33,_tmp9C9)));}static char
_tmp2BD[31]="list_t<type_modifier_t<`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2BD,_tmp2BD,_tmp2BD + 31}};struct Cyc_List_List*
Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2BE=yy1;struct Cyc_List_List*_tmp2BF;_LL18A: if((_tmp2BE.YY28).tag != 34)goto
_LL18C;_tmp2BF=(struct Cyc_List_List*)(_tmp2BE.YY28).val;_LL18B: yyzzz=_tmp2BF;
goto _LL189;_LL18C:;_LL18D:(int)_throw((void*)& Cyc_yyfail_YY28);_LL189:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CA;
return((_tmp9CA.YY28).val=yy1,(((_tmp9CA.YY28).tag=34,_tmp9CA)));}static char
_tmp2C2[13]="declarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={
Cyc_Core_Failure,{_tmp2C2,_tmp2C2,_tmp2C2 + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE _tmp2C3=
yy1;struct Cyc_Parse_Declarator*_tmp2C4;_LL18F: if((_tmp2C3.YY29).tag != 35)goto
_LL191;_tmp2C4=(struct Cyc_Parse_Declarator*)(_tmp2C3.YY29).val;_LL190: yyzzz=
_tmp2C4;goto _LL18E;_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY29);_LL18E:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){union Cyc_YYSTYPE
_tmp9CB;return((_tmp9CB.YY29).val=yy1,(((_tmp9CB.YY29).tag=35,_tmp9CB)));}static
char _tmp2C7[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={
Cyc_Core_Failure,{_tmp2C7,_tmp2C7,_tmp2C7 + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{
union Cyc_YYSTYPE _tmp2C8=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp2C9;_LL194:
if((_tmp2C8.YY30).tag != 36)goto _LL196;_tmp2C9=(struct Cyc_Parse_Abstractdeclarator*)(
_tmp2C8.YY30).val;_LL195: yyzzz=_tmp2C9;goto _LL193;_LL196:;_LL197:(int)_throw((
void*)& Cyc_yyfail_YY30);_LL193:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1){union Cyc_YYSTYPE _tmp9CC;return((_tmp9CC.YY30).val=yy1,(((_tmp9CC.YY30).tag=
36,_tmp9CC)));}static char _tmp2CC[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2CC,_tmp2CC,_tmp2CC + 5}};int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp2CD=yy1;int _tmp2CE;_LL199: if((
_tmp2CD.YY31).tag != 37)goto _LL19B;_tmp2CE=(int)(_tmp2CD.YY31).val;_LL19A: yyzzz=
_tmp2CE;goto _LL198;_LL19B:;_LL19C:(int)_throw((void*)& Cyc_yyfail_YY31);_LL198:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp9CD;return((
_tmp9CD.YY31).val=yy1,(((_tmp9CD.YY31).tag=37,_tmp9CD)));}static char _tmp2D1[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2D1,
_tmp2D1,_tmp2D1 + 8}};enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;{union Cyc_YYSTYPE _tmp2D2=yy1;enum Cyc_Absyn_Scope
_tmp2D3;_LL19E: if((_tmp2D2.YY32).tag != 38)goto _LL1A0;_tmp2D3=(enum Cyc_Absyn_Scope)(
_tmp2D2.YY32).val;_LL19F: yyzzz=_tmp2D3;goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((
void*)& Cyc_yyfail_YY32);_LL19D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope
yy1){union Cyc_YYSTYPE _tmp9CE;return((_tmp9CE.YY32).val=yy1,(((_tmp9CE.YY32).tag=
38,_tmp9CE)));}static char _tmp2D6[16]="datatypefield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2D6,_tmp2D6,_tmp2D6 + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union
Cyc_YYSTYPE _tmp2D7=yy1;struct Cyc_Absyn_Datatypefield*_tmp2D8;_LL1A3: if((_tmp2D7.YY33).tag
!= 39)goto _LL1A5;_tmp2D8=(struct Cyc_Absyn_Datatypefield*)(_tmp2D7.YY33).val;
_LL1A4: yyzzz=_tmp2D8;goto _LL1A2;_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL1A2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){
union Cyc_YYSTYPE _tmp9CF;return((_tmp9CF.YY33).val=yy1,(((_tmp9CF.YY33).tag=39,
_tmp9CF)));}static char _tmp2DB[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2DB,_tmp2DB,_tmp2DB + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2DC=yy1;struct Cyc_List_List*_tmp2DD;_LL1A8: if((_tmp2DC.YY34).tag != 40)goto
_LL1AA;_tmp2DD=(struct Cyc_List_List*)(_tmp2DC.YY34).val;_LL1A9: yyzzz=_tmp2DD;
goto _LL1A7;_LL1AA:;_LL1AB:(int)_throw((void*)& Cyc_yyfail_YY34);_LL1A7:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D0;
return((_tmp9D0.YY34).val=yy1,(((_tmp9D0.YY34).tag=40,_tmp9D0)));}static char
_tmp2E0[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2E0,_tmp2E0,
_tmp2E0 + 55}};struct _tuple22*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple22*
yyzzz;{union Cyc_YYSTYPE _tmp2E1=yy1;struct _tuple22*_tmp2E2;_LL1AD: if((_tmp2E1.YY35).tag
!= 41)goto _LL1AF;_tmp2E2=(struct _tuple22*)(_tmp2E1.YY35).val;_LL1AE: yyzzz=
_tmp2E2;goto _LL1AC;_LL1AF:;_LL1B0:(int)_throw((void*)& Cyc_yyfail_YY35);_LL1AC:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY35(struct _tuple22*yy1){union Cyc_YYSTYPE
_tmp9D1;return((_tmp9D1.YY35).val=yy1,(((_tmp9D1.YY35).tag=41,_tmp9D1)));}static
char _tmp2E5[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={
Cyc_Core_Failure,{_tmp2E5,_tmp2E5,_tmp2E5 + 17}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2E6=yy1;
struct Cyc_List_List*_tmp2E7;_LL1B2: if((_tmp2E6.YY36).tag != 42)goto _LL1B4;_tmp2E7=(
struct Cyc_List_List*)(_tmp2E6.YY36).val;_LL1B3: yyzzz=_tmp2E7;goto _LL1B1;_LL1B4:;
_LL1B5:(int)_throw((void*)& Cyc_yyfail_YY36);_LL1B1:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D2;return((_tmp9D2.YY36).val=
yy1,(((_tmp9D2.YY36).tag=42,_tmp9D2)));}static char _tmp2EA[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2EA,
_tmp2EA,_tmp2EA + 37}};struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){struct
_tuple8*yyzzz;{union Cyc_YYSTYPE _tmp2EB=yy1;struct _tuple8*_tmp2EC;_LL1B7: if((
_tmp2EB.YY37).tag != 43)goto _LL1B9;_tmp2EC=(struct _tuple8*)(_tmp2EB.YY37).val;
_LL1B8: yyzzz=_tmp2EC;goto _LL1B6;_LL1B9:;_LL1BA:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL1B6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE
_tmp9D3;return((_tmp9D3.YY37).val=yy1,(((_tmp9D3.YY37).tag=43,_tmp9D3)));}static
char _tmp2EF[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2EF,_tmp2EF,
_tmp2EF + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2F0=yy1;struct Cyc_List_List*_tmp2F1;_LL1BC: if((
_tmp2F0.YY38).tag != 44)goto _LL1BE;_tmp2F1=(struct Cyc_List_List*)(_tmp2F0.YY38).val;
_LL1BD: yyzzz=_tmp2F1;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1BB:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9D4;return((_tmp9D4.YY38).val=yy1,(((_tmp9D4.YY38).tag=44,_tmp9D4)));}static
char _tmp2F4[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2F4,
_tmp2F4,_tmp2F4 + 127}};struct _tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct
_tuple23*yyzzz;{union Cyc_YYSTYPE _tmp2F5=yy1;struct _tuple23*_tmp2F6;_LL1C1: if((
_tmp2F5.YY39).tag != 45)goto _LL1C3;_tmp2F6=(struct _tuple23*)(_tmp2F5.YY39).val;
_LL1C2: yyzzz=_tmp2F6;goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY39);
_LL1C0:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY39(struct _tuple23*yy1){union Cyc_YYSTYPE
_tmp9D5;return((_tmp9D5.YY39).val=yy1,(((_tmp9D5.YY39).tag=45,_tmp9D5)));}static
char _tmp2F9[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={
Cyc_Core_Failure,{_tmp2F9,_tmp2F9,_tmp2F9 + 18}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2FA=yy1;
struct Cyc_List_List*_tmp2FB;_LL1C6: if((_tmp2FA.YY40).tag != 46)goto _LL1C8;_tmp2FB=(
struct Cyc_List_List*)(_tmp2FA.YY40).val;_LL1C7: yyzzz=_tmp2FB;goto _LL1C5;_LL1C8:;
_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY40);_LL1C5:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D6;return((_tmp9D6.YY40).val=
yy1,(((_tmp9D6.YY40).tag=46,_tmp9D6)));}static char _tmp2FE[24]="list_t<designator_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2FE,
_tmp2FE,_tmp2FE + 24}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2FF=yy1;struct Cyc_List_List*
_tmp300;_LL1CB: if((_tmp2FF.YY41).tag != 47)goto _LL1CD;_tmp300=(struct Cyc_List_List*)(
_tmp2FF.YY41).val;_LL1CC: yyzzz=_tmp300;goto _LL1CA;_LL1CD:;_LL1CE:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1CA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9D7;return((_tmp9D7.YY41).val=yy1,(((_tmp9D7.YY41).tag=
47,_tmp9D7)));}static char _tmp303[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp303,_tmp303,_tmp303 + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp304=yy1;void*_tmp305;_LL1D0:
if((_tmp304.YY42).tag != 48)goto _LL1D2;_tmp305=(void*)(_tmp304.YY42).val;_LL1D1:
yyzzz=_tmp305;goto _LL1CF;_LL1D2:;_LL1D3:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1CF:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp9D8;
return((_tmp9D8.YY42).val=yy1,(((_tmp9D8.YY42).tag=48,_tmp9D8)));}static char
_tmp308[7]="kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{
_tmp308,_tmp308,_tmp308 + 7}};struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Kind*yyzzz;{union Cyc_YYSTYPE _tmp309=yy1;struct Cyc_Absyn_Kind*
_tmp30A;_LL1D5: if((_tmp309.YY43).tag != 49)goto _LL1D7;_tmp30A=(struct Cyc_Absyn_Kind*)(
_tmp309.YY43).val;_LL1D6: yyzzz=_tmp30A;goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((
void*)& Cyc_yyfail_YY43);_LL1D4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*
yy1){union Cyc_YYSTYPE _tmp9D9;return((_tmp9D9.YY43).val=yy1,(((_tmp9D9.YY43).tag=
49,_tmp9D9)));}static char _tmp30D[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp30D,_tmp30D,_tmp30D + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp30E=yy1;void*_tmp30F;_LL1DA:
if((_tmp30E.YY44).tag != 50)goto _LL1DC;_tmp30F=(void*)(_tmp30E.YY44).val;_LL1DB:
yyzzz=_tmp30F;goto _LL1D9;_LL1DC:;_LL1DD:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1D9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmp9DA;
return((_tmp9DA.YY44).val=yy1,(((_tmp9DA.YY44).tag=50,_tmp9DA)));}static char
_tmp312[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY45={
Cyc_Core_Failure,{_tmp312,_tmp312,_tmp312 + 23}};struct Cyc_List_List*Cyc_yyget_YY45(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp313=yy1;
struct Cyc_List_List*_tmp314;_LL1DF: if((_tmp313.YY45).tag != 51)goto _LL1E1;_tmp314=(
struct Cyc_List_List*)(_tmp313.YY45).val;_LL1E0: yyzzz=_tmp314;goto _LL1DE;_LL1E1:;
_LL1E2:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1DE:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DB;return((_tmp9DB.YY45).val=
yy1,(((_tmp9DB.YY45).tag=51,_tmp9DB)));}static char _tmp317[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp317,
_tmp317,_tmp317 + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union
Cyc_YYSTYPE _tmp318=yy1;void*_tmp319;_LL1E4: if((_tmp318.YY46).tag != 52)goto _LL1E6;
_tmp319=(void*)(_tmp318.YY46).val;_LL1E5: yyzzz=_tmp319;goto _LL1E3;_LL1E6:;_LL1E7:(
int)_throw((void*)& Cyc_yyfail_YY46);_LL1E3:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1){union Cyc_YYSTYPE _tmp9DC;return((_tmp9DC.YY46).val=yy1,(((_tmp9DC.YY46).tag=
52,_tmp9DC)));}static char _tmp31C[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp31C,_tmp31C,_tmp31C + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE
_tmp31D=yy1;struct Cyc_Absyn_Enumfield*_tmp31E;_LL1E9: if((_tmp31D.YY47).tag != 53)
goto _LL1EB;_tmp31E=(struct Cyc_Absyn_Enumfield*)(_tmp31D.YY47).val;_LL1EA: yyzzz=
_tmp31E;goto _LL1E8;_LL1EB:;_LL1EC:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1E8:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE
_tmp9DD;return((_tmp9DD.YY47).val=yy1,(((_tmp9DD.YY47).tag=53,_tmp9DD)));}static
char _tmp321[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={
Cyc_Core_Failure,{_tmp321,_tmp321,_tmp321 + 23}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp322=yy1;
struct Cyc_List_List*_tmp323;_LL1EE: if((_tmp322.YY48).tag != 54)goto _LL1F0;_tmp323=(
struct Cyc_List_List*)(_tmp322.YY48).val;_LL1EF: yyzzz=_tmp323;goto _LL1ED;_LL1F0:;
_LL1F1:(int)_throw((void*)& Cyc_yyfail_YY48);_LL1ED:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DE;return((_tmp9DE.YY48).val=
yy1,(((_tmp9DE.YY48).tag=54,_tmp9DE)));}static char _tmp326[17]="opt_t<type_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp326,
_tmp326,_tmp326 + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp327=yy1;struct Cyc_Core_Opt*_tmp328;
_LL1F3: if((_tmp327.YY49).tag != 55)goto _LL1F5;_tmp328=(struct Cyc_Core_Opt*)(
_tmp327.YY49).val;_LL1F4: yyzzz=_tmp328;goto _LL1F2;_LL1F5:;_LL1F6:(int)_throw((
void*)& Cyc_yyfail_YY49);_LL1F2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*
yy1){union Cyc_YYSTYPE _tmp9DF;return((_tmp9DF.YY49).val=yy1,(((_tmp9DF.YY49).tag=
55,_tmp9DF)));}static char _tmp32B[31]="list_t<$(type_t,type_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp32B,_tmp32B,
_tmp32B + 31}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp32C=yy1;struct Cyc_List_List*_tmp32D;_LL1F8: if((
_tmp32C.YY50).tag != 56)goto _LL1FA;_tmp32D=(struct Cyc_List_List*)(_tmp32C.YY50).val;
_LL1F9: yyzzz=_tmp32D;goto _LL1F7;_LL1FA:;_LL1FB:(int)_throw((void*)& Cyc_yyfail_YY50);
_LL1F7:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9E0;return((_tmp9E0.YY50).val=yy1,(((_tmp9E0.YY50).tag=56,_tmp9E0)));}static
char _tmp330[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY51={
Cyc_Core_Failure,{_tmp330,_tmp330,_tmp330 + 15}};union Cyc_Absyn_Constraint*Cyc_yyget_YY51(
union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp331=
yy1;union Cyc_Absyn_Constraint*_tmp332;_LL1FD: if((_tmp331.YY51).tag != 57)goto
_LL1FF;_tmp332=(union Cyc_Absyn_Constraint*)(_tmp331.YY51).val;_LL1FE: yyzzz=
_tmp332;goto _LL1FC;_LL1FF:;_LL200:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1FC:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE
_tmp9E1;return((_tmp9E1.YY51).val=yy1,(((_tmp9E1.YY51).tag=57,_tmp9E1)));}static
char _tmp335[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp335,_tmp335,_tmp335 + 45}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp336=yy1;struct Cyc_List_List*_tmp337;_LL202: if((_tmp336.YY52).tag != 58)goto
_LL204;_tmp337=(struct Cyc_List_List*)(_tmp336.YY52).val;_LL203: yyzzz=_tmp337;
goto _LL201;_LL204:;_LL205:(int)_throw((void*)& Cyc_yyfail_YY52);_LL201:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E2;
return((_tmp9E2.YY52).val=yy1,(((_tmp9E2.YY52).tag=58,_tmp9E2)));}static char
_tmp33A[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp33A,_tmp33A,_tmp33A + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp33B=yy1;void*_tmp33C;_LL207: if((_tmp33B.YY53).tag
!= 59)goto _LL209;_tmp33C=(void*)(_tmp33B.YY53).val;_LL208: yyzzz=_tmp33C;goto
_LL206;_LL209:;_LL20A:(int)_throw((void*)& Cyc_yyfail_YY53);_LL206:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmp9E3;return((_tmp9E3.YY53).val=
yy1,(((_tmp9E3.YY53).tag=59,_tmp9E3)));}static char _tmp33F[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp33F,
_tmp33F,_tmp33F + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp340=yy1;struct Cyc_List_List*
_tmp341;_LL20C: if((_tmp340.YY54).tag != 60)goto _LL20E;_tmp341=(struct Cyc_List_List*)(
_tmp340.YY54).val;_LL20D: yyzzz=_tmp341;goto _LL20B;_LL20E:;_LL20F:(int)_throw((
void*)& Cyc_yyfail_YY54);_LL20B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9E4;return((_tmp9E4.YY54).val=yy1,(((_tmp9E4.YY54).tag=
60,_tmp9E4)));}static char _tmp344[21]="$(bool,string_t<`H>)";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp344,_tmp344,_tmp344 + 21}};struct
_tuple17 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){struct _tuple17 yyzzz;{union Cyc_YYSTYPE
_tmp345=yy1;struct _tuple17 _tmp346;_LL211: if((_tmp345.Asm_tok).tag != 6)goto _LL213;
_tmp346=(struct _tuple17)(_tmp345.Asm_tok).val;_LL212: yyzzz=_tmp346;goto _LL210;
_LL213:;_LL214:(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL210:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple17 yy1){union Cyc_YYSTYPE _tmp9E5;return((
_tmp9E5.Asm_tok).val=yy1,(((_tmp9E5.Asm_tok).tag=6,_tmp9E5)));}struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_Yyltype Cyc_yynewloc(){struct Cyc_Yyltype _tmp9E6;return(_tmp9E6.timestamp=0,((
_tmp9E6.first_line=0,((_tmp9E6.first_column=0,((_tmp9E6.last_line=0,((_tmp9E6.last_column=
0,_tmp9E6)))))))));}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[
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
104,105,106,107,108,109,110,111,112,113,114,115,116};static char _tmp349[2]="$";
static char _tmp34A[6]="error";static char _tmp34B[12]="$undefined.";static char
_tmp34C[5]="AUTO";static char _tmp34D[9]="REGISTER";static char _tmp34E[7]="STATIC";
static char _tmp34F[7]="EXTERN";static char _tmp350[8]="TYPEDEF";static char _tmp351[5]="VOID";
static char _tmp352[5]="CHAR";static char _tmp353[6]="SHORT";static char _tmp354[4]="INT";
static char _tmp355[5]="LONG";static char _tmp356[6]="FLOAT";static char _tmp357[7]="DOUBLE";
static char _tmp358[7]="SIGNED";static char _tmp359[9]="UNSIGNED";static char _tmp35A[
6]="CONST";static char _tmp35B[9]="VOLATILE";static char _tmp35C[9]="RESTRICT";
static char _tmp35D[7]="STRUCT";static char _tmp35E[6]="UNION";static char _tmp35F[5]="CASE";
static char _tmp360[8]="DEFAULT";static char _tmp361[7]="INLINE";static char _tmp362[7]="SIZEOF";
static char _tmp363[9]="OFFSETOF";static char _tmp364[3]="IF";static char _tmp365[5]="ELSE";
static char _tmp366[7]="SWITCH";static char _tmp367[6]="WHILE";static char _tmp368[3]="DO";
static char _tmp369[4]="FOR";static char _tmp36A[5]="GOTO";static char _tmp36B[9]="CONTINUE";
static char _tmp36C[6]="BREAK";static char _tmp36D[7]="RETURN";static char _tmp36E[5]="ENUM";
static char _tmp36F[8]="NULL_kw";static char _tmp370[4]="LET";static char _tmp371[6]="THROW";
static char _tmp372[4]="TRY";static char _tmp373[6]="CATCH";static char _tmp374[7]="EXPORT";
static char _tmp375[4]="NEW";static char _tmp376[9]="ABSTRACT";static char _tmp377[9]="FALLTHRU";
static char _tmp378[6]="USING";static char _tmp379[10]="NAMESPACE";static char _tmp37A[
9]="DATATYPE";static char _tmp37B[8]="XTUNION";static char _tmp37C[7]="TUNION";
static char _tmp37D[7]="MALLOC";static char _tmp37E[8]="RMALLOC";static char _tmp37F[7]="CALLOC";
static char _tmp380[8]="RCALLOC";static char _tmp381[5]="SWAP";static char _tmp382[9]="REGION_T";
static char _tmp383[6]="TAG_T";static char _tmp384[7]="REGION";static char _tmp385[5]="RNEW";
static char _tmp386[8]="REGIONS";static char _tmp387[13]="RESET_REGION";static char
_tmp388[16]="NOZEROTERM_QUAL";static char _tmp389[14]="ZEROTERM_QUAL";static char
_tmp38A[12]="REGION_QUAL";static char _tmp38B[7]="PORTON";static char _tmp38C[8]="PORTOFF";
static char _tmp38D[12]="DYNREGION_T";static char _tmp38E[6]="ALIAS";static char
_tmp38F[8]="NUMELTS";static char _tmp390[8]="VALUEOF";static char _tmp391[10]="VALUEOF_T";
static char _tmp392[9]="TAGCHECK";static char _tmp393[13]="NUMELTS_QUAL";static char
_tmp394[10]="THIN_QUAL";static char _tmp395[9]="FAT_QUAL";static char _tmp396[13]="NOTNULL_QUAL";
static char _tmp397[14]="NULLABLE_QUAL";static char _tmp398[12]="TAGGED_QUAL";static
char _tmp399[16]="EXTENSIBLE_QUAL";static char _tmp39A[15]="RESETABLE_QUAL";static
char _tmp39B[7]="PTR_OP";static char _tmp39C[7]="INC_OP";static char _tmp39D[7]="DEC_OP";
static char _tmp39E[8]="LEFT_OP";static char _tmp39F[9]="RIGHT_OP";static char _tmp3A0[
6]="LE_OP";static char _tmp3A1[6]="GE_OP";static char _tmp3A2[6]="EQ_OP";static char
_tmp3A3[6]="NE_OP";static char _tmp3A4[7]="AND_OP";static char _tmp3A5[6]="OR_OP";
static char _tmp3A6[11]="MUL_ASSIGN";static char _tmp3A7[11]="DIV_ASSIGN";static char
_tmp3A8[11]="MOD_ASSIGN";static char _tmp3A9[11]="ADD_ASSIGN";static char _tmp3AA[11]="SUB_ASSIGN";
static char _tmp3AB[12]="LEFT_ASSIGN";static char _tmp3AC[13]="RIGHT_ASSIGN";static
char _tmp3AD[11]="AND_ASSIGN";static char _tmp3AE[11]="XOR_ASSIGN";static char
_tmp3AF[10]="OR_ASSIGN";static char _tmp3B0[9]="ELLIPSIS";static char _tmp3B1[11]="LEFT_RIGHT";
static char _tmp3B2[12]="COLON_COLON";static char _tmp3B3[11]="IDENTIFIER";static
char _tmp3B4[17]="INTEGER_CONSTANT";static char _tmp3B5[7]="STRING";static char
_tmp3B6[19]="CHARACTER_CONSTANT";static char _tmp3B7[18]="FLOATING_CONSTANT";
static char _tmp3B8[9]="TYPE_VAR";static char _tmp3B9[13]="TYPEDEF_NAME";static char
_tmp3BA[16]="QUAL_IDENTIFIER";static char _tmp3BB[18]="QUAL_TYPEDEF_NAME";static
char _tmp3BC[10]="ATTRIBUTE";static char _tmp3BD[4]="ASM";static char _tmp3BE[4]="';'";
static char _tmp3BF[4]="'{'";static char _tmp3C0[4]="'}'";static char _tmp3C1[4]="','";
static char _tmp3C2[4]="'='";static char _tmp3C3[4]="'<'";static char _tmp3C4[4]="'>'";
static char _tmp3C5[4]="'('";static char _tmp3C6[4]="')'";static char _tmp3C7[4]="'_'";
static char _tmp3C8[4]="'$'";static char _tmp3C9[4]="':'";static char _tmp3CA[4]="'.'";
static char _tmp3CB[4]="'['";static char _tmp3CC[4]="']'";static char _tmp3CD[4]="'*'";
static char _tmp3CE[4]="'@'";static char _tmp3CF[4]="'?'";static char _tmp3D0[4]="'+'";
static char _tmp3D1[4]="'|'";static char _tmp3D2[4]="'^'";static char _tmp3D3[4]="'&'";
static char _tmp3D4[4]="'-'";static char _tmp3D5[4]="'/'";static char _tmp3D6[4]="'%'";
static char _tmp3D7[4]="'~'";static char _tmp3D8[4]="'!'";static char _tmp3D9[5]="prog";
static char _tmp3DA[17]="translation_unit";static char _tmp3DB[12]="export_list";
static char _tmp3DC[19]="export_list_values";static char _tmp3DD[21]="external_declaration";
static char _tmp3DE[20]="function_definition";static char _tmp3DF[21]="function_definition2";
static char _tmp3E0[13]="using_action";static char _tmp3E1[15]="unusing_action";
static char _tmp3E2[17]="namespace_action";static char _tmp3E3[19]="unnamespace_action";
static char _tmp3E4[12]="declaration";static char _tmp3E5[19]="resetable_qual_opt";
static char _tmp3E6[17]="declaration_list";static char _tmp3E7[23]="declaration_specifiers";
static char _tmp3E8[24]="storage_class_specifier";static char _tmp3E9[15]="attributes_opt";
static char _tmp3EA[11]="attributes";static char _tmp3EB[15]="attribute_list";static
char _tmp3EC[10]="attribute";static char _tmp3ED[15]="type_specifier";static char
_tmp3EE[25]="type_specifier_notypedef";static char _tmp3EF[5]="kind";static char
_tmp3F0[15]="type_qualifier";static char _tmp3F1[15]="enum_specifier";static char
_tmp3F2[11]="enum_field";static char _tmp3F3[22]="enum_declaration_list";static
char _tmp3F4[26]="struct_or_union_specifier";static char _tmp3F5[16]="type_params_opt";
static char _tmp3F6[16]="struct_or_union";static char _tmp3F7[24]="struct_declaration_list";
static char _tmp3F8[25]="struct_declaration_list0";static char _tmp3F9[21]="init_declarator_list";
static char _tmp3FA[22]="init_declarator_list0";static char _tmp3FB[16]="init_declarator";
static char _tmp3FC[19]="struct_declaration";static char _tmp3FD[25]="specifier_qualifier_list";
static char _tmp3FE[35]="notypedef_specifier_qualifier_list";static char _tmp3FF[23]="struct_declarator_list";
static char _tmp400[24]="struct_declarator_list0";static char _tmp401[18]="struct_declarator";
static char _tmp402[19]="datatype_specifier";static char _tmp403[14]="qual_datatype";
static char _tmp404[19]="datatypefield_list";static char _tmp405[20]="datatypefield_scope";
static char _tmp406[14]="datatypefield";static char _tmp407[11]="declarator";static
char _tmp408[23]="declarator_withtypedef";static char _tmp409[18]="direct_declarator";
static char _tmp40A[30]="direct_declarator_withtypedef";static char _tmp40B[8]="pointer";
static char _tmp40C[12]="one_pointer";static char _tmp40D[14]="pointer_quals";static
char _tmp40E[13]="pointer_qual";static char _tmp40F[23]="pointer_null_and_bound";
static char _tmp410[14]="pointer_bound";static char _tmp411[18]="zeroterm_qual_opt";
static char _tmp412[8]="rgn_opt";static char _tmp413[11]="tqual_list";static char
_tmp414[20]="parameter_type_list";static char _tmp415[9]="type_var";static char
_tmp416[16]="optional_effect";static char _tmp417[19]="optional_rgn_order";static
char _tmp418[10]="rgn_order";static char _tmp419[16]="optional_inject";static char
_tmp41A[11]="effect_set";static char _tmp41B[14]="atomic_effect";static char _tmp41C[
11]="region_set";static char _tmp41D[15]="parameter_list";static char _tmp41E[22]="parameter_declaration";
static char _tmp41F[16]="identifier_list";static char _tmp420[17]="identifier_list0";
static char _tmp421[12]="initializer";static char _tmp422[18]="array_initializer";
static char _tmp423[17]="initializer_list";static char _tmp424[12]="designation";
static char _tmp425[16]="designator_list";static char _tmp426[11]="designator";
static char _tmp427[10]="type_name";static char _tmp428[14]="any_type_name";static
char _tmp429[15]="type_name_list";static char _tmp42A[20]="abstract_declarator";
static char _tmp42B[27]="direct_abstract_declarator";static char _tmp42C[10]="statement";
static char _tmp42D[13]="open_exp_opt";static char _tmp42E[18]="labeled_statement";
static char _tmp42F[21]="expression_statement";static char _tmp430[19]="compound_statement";
static char _tmp431[16]="block_item_list";static char _tmp432[20]="selection_statement";
static char _tmp433[15]="switch_clauses";static char _tmp434[20]="iteration_statement";
static char _tmp435[15]="jump_statement";static char _tmp436[12]="exp_pattern";
static char _tmp437[20]="conditional_pattern";static char _tmp438[19]="logical_or_pattern";
static char _tmp439[20]="logical_and_pattern";static char _tmp43A[21]="inclusive_or_pattern";
static char _tmp43B[21]="exclusive_or_pattern";static char _tmp43C[12]="and_pattern";
static char _tmp43D[17]="equality_pattern";static char _tmp43E[19]="relational_pattern";
static char _tmp43F[14]="shift_pattern";static char _tmp440[17]="additive_pattern";
static char _tmp441[23]="multiplicative_pattern";static char _tmp442[13]="cast_pattern";
static char _tmp443[14]="unary_pattern";static char _tmp444[16]="postfix_pattern";
static char _tmp445[16]="primary_pattern";static char _tmp446[8]="pattern";static
char _tmp447[19]="tuple_pattern_list";static char _tmp448[20]="tuple_pattern_list0";
static char _tmp449[14]="field_pattern";static char _tmp44A[19]="field_pattern_list";
static char _tmp44B[20]="field_pattern_list0";static char _tmp44C[11]="expression";
static char _tmp44D[22]="assignment_expression";static char _tmp44E[20]="assignment_operator";
static char _tmp44F[23]="conditional_expression";static char _tmp450[20]="constant_expression";
static char _tmp451[22]="logical_or_expression";static char _tmp452[23]="logical_and_expression";
static char _tmp453[24]="inclusive_or_expression";static char _tmp454[24]="exclusive_or_expression";
static char _tmp455[15]="and_expression";static char _tmp456[20]="equality_expression";
static char _tmp457[22]="relational_expression";static char _tmp458[17]="shift_expression";
static char _tmp459[20]="additive_expression";static char _tmp45A[26]="multiplicative_expression";
static char _tmp45B[16]="cast_expression";static char _tmp45C[17]="unary_expression";
static char _tmp45D[15]="unary_operator";static char _tmp45E[19]="postfix_expression";
static char _tmp45F[19]="primary_expression";static char _tmp460[25]="argument_expression_list";
static char _tmp461[26]="argument_expression_list0";static char _tmp462[9]="constant";
static char _tmp463[20]="qual_opt_identifier";static char _tmp464[17]="qual_opt_typedef";
static char _tmp465[18]="struct_union_name";static char _tmp466[11]="field_name";
static char _tmp467[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[287]={{
_tmp349,_tmp349,_tmp349 + 2},{_tmp34A,_tmp34A,_tmp34A + 6},{_tmp34B,_tmp34B,
_tmp34B + 12},{_tmp34C,_tmp34C,_tmp34C + 5},{_tmp34D,_tmp34D,_tmp34D + 9},{_tmp34E,
_tmp34E,_tmp34E + 7},{_tmp34F,_tmp34F,_tmp34F + 7},{_tmp350,_tmp350,_tmp350 + 8},{
_tmp351,_tmp351,_tmp351 + 5},{_tmp352,_tmp352,_tmp352 + 5},{_tmp353,_tmp353,
_tmp353 + 6},{_tmp354,_tmp354,_tmp354 + 4},{_tmp355,_tmp355,_tmp355 + 5},{_tmp356,
_tmp356,_tmp356 + 6},{_tmp357,_tmp357,_tmp357 + 7},{_tmp358,_tmp358,_tmp358 + 7},{
_tmp359,_tmp359,_tmp359 + 9},{_tmp35A,_tmp35A,_tmp35A + 6},{_tmp35B,_tmp35B,
_tmp35B + 9},{_tmp35C,_tmp35C,_tmp35C + 9},{_tmp35D,_tmp35D,_tmp35D + 7},{_tmp35E,
_tmp35E,_tmp35E + 6},{_tmp35F,_tmp35F,_tmp35F + 5},{_tmp360,_tmp360,_tmp360 + 8},{
_tmp361,_tmp361,_tmp361 + 7},{_tmp362,_tmp362,_tmp362 + 7},{_tmp363,_tmp363,
_tmp363 + 9},{_tmp364,_tmp364,_tmp364 + 3},{_tmp365,_tmp365,_tmp365 + 5},{_tmp366,
_tmp366,_tmp366 + 7},{_tmp367,_tmp367,_tmp367 + 6},{_tmp368,_tmp368,_tmp368 + 3},{
_tmp369,_tmp369,_tmp369 + 4},{_tmp36A,_tmp36A,_tmp36A + 5},{_tmp36B,_tmp36B,
_tmp36B + 9},{_tmp36C,_tmp36C,_tmp36C + 6},{_tmp36D,_tmp36D,_tmp36D + 7},{_tmp36E,
_tmp36E,_tmp36E + 5},{_tmp36F,_tmp36F,_tmp36F + 8},{_tmp370,_tmp370,_tmp370 + 4},{
_tmp371,_tmp371,_tmp371 + 6},{_tmp372,_tmp372,_tmp372 + 4},{_tmp373,_tmp373,
_tmp373 + 6},{_tmp374,_tmp374,_tmp374 + 7},{_tmp375,_tmp375,_tmp375 + 4},{_tmp376,
_tmp376,_tmp376 + 9},{_tmp377,_tmp377,_tmp377 + 9},{_tmp378,_tmp378,_tmp378 + 6},{
_tmp379,_tmp379,_tmp379 + 10},{_tmp37A,_tmp37A,_tmp37A + 9},{_tmp37B,_tmp37B,
_tmp37B + 8},{_tmp37C,_tmp37C,_tmp37C + 7},{_tmp37D,_tmp37D,_tmp37D + 7},{_tmp37E,
_tmp37E,_tmp37E + 8},{_tmp37F,_tmp37F,_tmp37F + 7},{_tmp380,_tmp380,_tmp380 + 8},{
_tmp381,_tmp381,_tmp381 + 5},{_tmp382,_tmp382,_tmp382 + 9},{_tmp383,_tmp383,
_tmp383 + 6},{_tmp384,_tmp384,_tmp384 + 7},{_tmp385,_tmp385,_tmp385 + 5},{_tmp386,
_tmp386,_tmp386 + 8},{_tmp387,_tmp387,_tmp387 + 13},{_tmp388,_tmp388,_tmp388 + 16},{
_tmp389,_tmp389,_tmp389 + 14},{_tmp38A,_tmp38A,_tmp38A + 12},{_tmp38B,_tmp38B,
_tmp38B + 7},{_tmp38C,_tmp38C,_tmp38C + 8},{_tmp38D,_tmp38D,_tmp38D + 12},{_tmp38E,
_tmp38E,_tmp38E + 6},{_tmp38F,_tmp38F,_tmp38F + 8},{_tmp390,_tmp390,_tmp390 + 8},{
_tmp391,_tmp391,_tmp391 + 10},{_tmp392,_tmp392,_tmp392 + 9},{_tmp393,_tmp393,
_tmp393 + 13},{_tmp394,_tmp394,_tmp394 + 10},{_tmp395,_tmp395,_tmp395 + 9},{_tmp396,
_tmp396,_tmp396 + 13},{_tmp397,_tmp397,_tmp397 + 14},{_tmp398,_tmp398,_tmp398 + 12},{
_tmp399,_tmp399,_tmp399 + 16},{_tmp39A,_tmp39A,_tmp39A + 15},{_tmp39B,_tmp39B,
_tmp39B + 7},{_tmp39C,_tmp39C,_tmp39C + 7},{_tmp39D,_tmp39D,_tmp39D + 7},{_tmp39E,
_tmp39E,_tmp39E + 8},{_tmp39F,_tmp39F,_tmp39F + 9},{_tmp3A0,_tmp3A0,_tmp3A0 + 6},{
_tmp3A1,_tmp3A1,_tmp3A1 + 6},{_tmp3A2,_tmp3A2,_tmp3A2 + 6},{_tmp3A3,_tmp3A3,
_tmp3A3 + 6},{_tmp3A4,_tmp3A4,_tmp3A4 + 7},{_tmp3A5,_tmp3A5,_tmp3A5 + 6},{_tmp3A6,
_tmp3A6,_tmp3A6 + 11},{_tmp3A7,_tmp3A7,_tmp3A7 + 11},{_tmp3A8,_tmp3A8,_tmp3A8 + 11},{
_tmp3A9,_tmp3A9,_tmp3A9 + 11},{_tmp3AA,_tmp3AA,_tmp3AA + 11},{_tmp3AB,_tmp3AB,
_tmp3AB + 12},{_tmp3AC,_tmp3AC,_tmp3AC + 13},{_tmp3AD,_tmp3AD,_tmp3AD + 11},{
_tmp3AE,_tmp3AE,_tmp3AE + 11},{_tmp3AF,_tmp3AF,_tmp3AF + 10},{_tmp3B0,_tmp3B0,
_tmp3B0 + 9},{_tmp3B1,_tmp3B1,_tmp3B1 + 11},{_tmp3B2,_tmp3B2,_tmp3B2 + 12},{_tmp3B3,
_tmp3B3,_tmp3B3 + 11},{_tmp3B4,_tmp3B4,_tmp3B4 + 17},{_tmp3B5,_tmp3B5,_tmp3B5 + 7},{
_tmp3B6,_tmp3B6,_tmp3B6 + 19},{_tmp3B7,_tmp3B7,_tmp3B7 + 18},{_tmp3B8,_tmp3B8,
_tmp3B8 + 9},{_tmp3B9,_tmp3B9,_tmp3B9 + 13},{_tmp3BA,_tmp3BA,_tmp3BA + 16},{_tmp3BB,
_tmp3BB,_tmp3BB + 18},{_tmp3BC,_tmp3BC,_tmp3BC + 10},{_tmp3BD,_tmp3BD,_tmp3BD + 4},{
_tmp3BE,_tmp3BE,_tmp3BE + 4},{_tmp3BF,_tmp3BF,_tmp3BF + 4},{_tmp3C0,_tmp3C0,
_tmp3C0 + 4},{_tmp3C1,_tmp3C1,_tmp3C1 + 4},{_tmp3C2,_tmp3C2,_tmp3C2 + 4},{_tmp3C3,
_tmp3C3,_tmp3C3 + 4},{_tmp3C4,_tmp3C4,_tmp3C4 + 4},{_tmp3C5,_tmp3C5,_tmp3C5 + 4},{
_tmp3C6,_tmp3C6,_tmp3C6 + 4},{_tmp3C7,_tmp3C7,_tmp3C7 + 4},{_tmp3C8,_tmp3C8,
_tmp3C8 + 4},{_tmp3C9,_tmp3C9,_tmp3C9 + 4},{_tmp3CA,_tmp3CA,_tmp3CA + 4},{_tmp3CB,
_tmp3CB,_tmp3CB + 4},{_tmp3CC,_tmp3CC,_tmp3CC + 4},{_tmp3CD,_tmp3CD,_tmp3CD + 4},{
_tmp3CE,_tmp3CE,_tmp3CE + 4},{_tmp3CF,_tmp3CF,_tmp3CF + 4},{_tmp3D0,_tmp3D0,
_tmp3D0 + 4},{_tmp3D1,_tmp3D1,_tmp3D1 + 4},{_tmp3D2,_tmp3D2,_tmp3D2 + 4},{_tmp3D3,
_tmp3D3,_tmp3D3 + 4},{_tmp3D4,_tmp3D4,_tmp3D4 + 4},{_tmp3D5,_tmp3D5,_tmp3D5 + 4},{
_tmp3D6,_tmp3D6,_tmp3D6 + 4},{_tmp3D7,_tmp3D7,_tmp3D7 + 4},{_tmp3D8,_tmp3D8,
_tmp3D8 + 4},{_tmp3D9,_tmp3D9,_tmp3D9 + 5},{_tmp3DA,_tmp3DA,_tmp3DA + 17},{_tmp3DB,
_tmp3DB,_tmp3DB + 12},{_tmp3DC,_tmp3DC,_tmp3DC + 19},{_tmp3DD,_tmp3DD,_tmp3DD + 21},{
_tmp3DE,_tmp3DE,_tmp3DE + 20},{_tmp3DF,_tmp3DF,_tmp3DF + 21},{_tmp3E0,_tmp3E0,
_tmp3E0 + 13},{_tmp3E1,_tmp3E1,_tmp3E1 + 15},{_tmp3E2,_tmp3E2,_tmp3E2 + 17},{
_tmp3E3,_tmp3E3,_tmp3E3 + 19},{_tmp3E4,_tmp3E4,_tmp3E4 + 12},{_tmp3E5,_tmp3E5,
_tmp3E5 + 19},{_tmp3E6,_tmp3E6,_tmp3E6 + 17},{_tmp3E7,_tmp3E7,_tmp3E7 + 23},{
_tmp3E8,_tmp3E8,_tmp3E8 + 24},{_tmp3E9,_tmp3E9,_tmp3E9 + 15},{_tmp3EA,_tmp3EA,
_tmp3EA + 11},{_tmp3EB,_tmp3EB,_tmp3EB + 15},{_tmp3EC,_tmp3EC,_tmp3EC + 10},{
_tmp3ED,_tmp3ED,_tmp3ED + 15},{_tmp3EE,_tmp3EE,_tmp3EE + 25},{_tmp3EF,_tmp3EF,
_tmp3EF + 5},{_tmp3F0,_tmp3F0,_tmp3F0 + 15},{_tmp3F1,_tmp3F1,_tmp3F1 + 15},{_tmp3F2,
_tmp3F2,_tmp3F2 + 11},{_tmp3F3,_tmp3F3,_tmp3F3 + 22},{_tmp3F4,_tmp3F4,_tmp3F4 + 26},{
_tmp3F5,_tmp3F5,_tmp3F5 + 16},{_tmp3F6,_tmp3F6,_tmp3F6 + 16},{_tmp3F7,_tmp3F7,
_tmp3F7 + 24},{_tmp3F8,_tmp3F8,_tmp3F8 + 25},{_tmp3F9,_tmp3F9,_tmp3F9 + 21},{
_tmp3FA,_tmp3FA,_tmp3FA + 22},{_tmp3FB,_tmp3FB,_tmp3FB + 16},{_tmp3FC,_tmp3FC,
_tmp3FC + 19},{_tmp3FD,_tmp3FD,_tmp3FD + 25},{_tmp3FE,_tmp3FE,_tmp3FE + 35},{
_tmp3FF,_tmp3FF,_tmp3FF + 23},{_tmp400,_tmp400,_tmp400 + 24},{_tmp401,_tmp401,
_tmp401 + 18},{_tmp402,_tmp402,_tmp402 + 19},{_tmp403,_tmp403,_tmp403 + 14},{
_tmp404,_tmp404,_tmp404 + 19},{_tmp405,_tmp405,_tmp405 + 20},{_tmp406,_tmp406,
_tmp406 + 14},{_tmp407,_tmp407,_tmp407 + 11},{_tmp408,_tmp408,_tmp408 + 23},{
_tmp409,_tmp409,_tmp409 + 18},{_tmp40A,_tmp40A,_tmp40A + 30},{_tmp40B,_tmp40B,
_tmp40B + 8},{_tmp40C,_tmp40C,_tmp40C + 12},{_tmp40D,_tmp40D,_tmp40D + 14},{_tmp40E,
_tmp40E,_tmp40E + 13},{_tmp40F,_tmp40F,_tmp40F + 23},{_tmp410,_tmp410,_tmp410 + 14},{
_tmp411,_tmp411,_tmp411 + 18},{_tmp412,_tmp412,_tmp412 + 8},{_tmp413,_tmp413,
_tmp413 + 11},{_tmp414,_tmp414,_tmp414 + 20},{_tmp415,_tmp415,_tmp415 + 9},{_tmp416,
_tmp416,_tmp416 + 16},{_tmp417,_tmp417,_tmp417 + 19},{_tmp418,_tmp418,_tmp418 + 10},{
_tmp419,_tmp419,_tmp419 + 16},{_tmp41A,_tmp41A,_tmp41A + 11},{_tmp41B,_tmp41B,
_tmp41B + 14},{_tmp41C,_tmp41C,_tmp41C + 11},{_tmp41D,_tmp41D,_tmp41D + 15},{
_tmp41E,_tmp41E,_tmp41E + 22},{_tmp41F,_tmp41F,_tmp41F + 16},{_tmp420,_tmp420,
_tmp420 + 17},{_tmp421,_tmp421,_tmp421 + 12},{_tmp422,_tmp422,_tmp422 + 18},{
_tmp423,_tmp423,_tmp423 + 17},{_tmp424,_tmp424,_tmp424 + 12},{_tmp425,_tmp425,
_tmp425 + 16},{_tmp426,_tmp426,_tmp426 + 11},{_tmp427,_tmp427,_tmp427 + 10},{
_tmp428,_tmp428,_tmp428 + 14},{_tmp429,_tmp429,_tmp429 + 15},{_tmp42A,_tmp42A,
_tmp42A + 20},{_tmp42B,_tmp42B,_tmp42B + 27},{_tmp42C,_tmp42C,_tmp42C + 10},{
_tmp42D,_tmp42D,_tmp42D + 13},{_tmp42E,_tmp42E,_tmp42E + 18},{_tmp42F,_tmp42F,
_tmp42F + 21},{_tmp430,_tmp430,_tmp430 + 19},{_tmp431,_tmp431,_tmp431 + 16},{
_tmp432,_tmp432,_tmp432 + 20},{_tmp433,_tmp433,_tmp433 + 15},{_tmp434,_tmp434,
_tmp434 + 20},{_tmp435,_tmp435,_tmp435 + 15},{_tmp436,_tmp436,_tmp436 + 12},{
_tmp437,_tmp437,_tmp437 + 20},{_tmp438,_tmp438,_tmp438 + 19},{_tmp439,_tmp439,
_tmp439 + 20},{_tmp43A,_tmp43A,_tmp43A + 21},{_tmp43B,_tmp43B,_tmp43B + 21},{
_tmp43C,_tmp43C,_tmp43C + 12},{_tmp43D,_tmp43D,_tmp43D + 17},{_tmp43E,_tmp43E,
_tmp43E + 19},{_tmp43F,_tmp43F,_tmp43F + 14},{_tmp440,_tmp440,_tmp440 + 17},{
_tmp441,_tmp441,_tmp441 + 23},{_tmp442,_tmp442,_tmp442 + 13},{_tmp443,_tmp443,
_tmp443 + 14},{_tmp444,_tmp444,_tmp444 + 16},{_tmp445,_tmp445,_tmp445 + 16},{
_tmp446,_tmp446,_tmp446 + 8},{_tmp447,_tmp447,_tmp447 + 19},{_tmp448,_tmp448,
_tmp448 + 20},{_tmp449,_tmp449,_tmp449 + 14},{_tmp44A,_tmp44A,_tmp44A + 19},{
_tmp44B,_tmp44B,_tmp44B + 20},{_tmp44C,_tmp44C,_tmp44C + 11},{_tmp44D,_tmp44D,
_tmp44D + 22},{_tmp44E,_tmp44E,_tmp44E + 20},{_tmp44F,_tmp44F,_tmp44F + 23},{
_tmp450,_tmp450,_tmp450 + 20},{_tmp451,_tmp451,_tmp451 + 22},{_tmp452,_tmp452,
_tmp452 + 23},{_tmp453,_tmp453,_tmp453 + 24},{_tmp454,_tmp454,_tmp454 + 24},{
_tmp455,_tmp455,_tmp455 + 15},{_tmp456,_tmp456,_tmp456 + 20},{_tmp457,_tmp457,
_tmp457 + 22},{_tmp458,_tmp458,_tmp458 + 17},{_tmp459,_tmp459,_tmp459 + 20},{
_tmp45A,_tmp45A,_tmp45A + 26},{_tmp45B,_tmp45B,_tmp45B + 16},{_tmp45C,_tmp45C,
_tmp45C + 17},{_tmp45D,_tmp45D,_tmp45D + 15},{_tmp45E,_tmp45E,_tmp45E + 19},{
_tmp45F,_tmp45F,_tmp45F + 19},{_tmp460,_tmp460,_tmp460 + 25},{_tmp461,_tmp461,
_tmp461 + 26},{_tmp462,_tmp462,_tmp462 + 9},{_tmp463,_tmp463,_tmp463 + 20},{_tmp464,
_tmp464,_tmp464 + 17},{_tmp465,_tmp465,_tmp465 + 18},{_tmp466,_tmp466,_tmp466 + 11},{
_tmp467,_tmp467,_tmp467 + 12}};static short Cyc_yyr1[513]={0,144,145,145,145,145,
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
struct _dyneither_ptr f1;void*f2;};static char _tmp505[8]="stdcall";static char
_tmp506[6]="cdecl";static char _tmp507[9]="fastcall";static char _tmp508[9]="noreturn";
static char _tmp509[6]="const";static char _tmp50A[8]="aligned";static char _tmp50B[7]="packed";
static char _tmp50C[7]="shared";static char _tmp50D[7]="unused";static char _tmp50E[5]="weak";
static char _tmp50F[10]="dllimport";static char _tmp510[10]="dllexport";static char
_tmp511[23]="no_instrument_function";static char _tmp512[12]="constructor";static
char _tmp513[11]="destructor";static char _tmp514[22]="no_check_memory_usage";
static char _tmp515[5]="pure";struct _tuple26{void*f1;void*f2;};struct _tuple27{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*
yylex_buf);static void _tmp9EB(unsigned int*_tmp9EA,unsigned int*_tmp9E9,union Cyc_YYSTYPE**
_tmp9E8){for(*_tmp9EA=0;*_tmp9EA < *_tmp9E9;(*_tmp9EA)++){(*_tmp9E8)[*_tmp9EA]=
Cyc_yylval;}}static void _tmp9F0(unsigned int*_tmp9EF,unsigned int*_tmp9EE,struct
Cyc_Yyltype**_tmp9ED){for(*_tmp9EF=0;*_tmp9EF < *_tmp9EE;(*_tmp9EF)++){(*_tmp9ED)[*
_tmp9EF]=Cyc_yynewloc();}}static void _tmpEE0(unsigned int*_tmpEDF,unsigned int*
_tmpEDE,char**_tmpEDC){for(*_tmpEDF=0;*_tmpEDF < *_tmpEDE;(*_tmpEDF)++){(*_tmpEDC)[*
_tmpEDF]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct
_RegionHandle _tmp469=_new_region("yyregion");struct _RegionHandle*yyregion=&
_tmp469;_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;
int yyssp_offset;short*yyss=(short*)_region_malloc(yyregion,sizeof(short)* 10000);
int yyvsp_offset;unsigned int _tmp9EA;unsigned int _tmp9E9;union Cyc_YYSTYPE*_tmp9E8;
unsigned int _tmp9E7;union Cyc_YYSTYPE*yyvs=(union Cyc_YYSTYPE*)((_tmp9E7=10000,((
_tmp9E8=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),
_tmp9E7)),((((_tmp9E9=_tmp9E7,_tmp9EB(& _tmp9EA,& _tmp9E9,& _tmp9E8))),_tmp9E8))))));
int yylsp_offset;unsigned int _tmp9EF;unsigned int _tmp9EE;struct Cyc_Yyltype*
_tmp9ED;unsigned int _tmp9EC;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)((_tmp9EC=
10000,((_tmp9ED=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(
struct Cyc_Yyltype),_tmp9EC)),((((_tmp9EE=_tmp9EC,_tmp9F0(& _tmp9EF,& _tmp9EE,&
_tmp9ED))),_tmp9ED))))));int yystacksize=10000;union Cyc_YYSTYPE yyval=Cyc_yylval;
int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;
yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(
10000,++ yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(
yystacksize >= 10000){{const char*_tmp9F1;Cyc_yyerror(((_tmp9F1="parser stack overflow",
_tag_dyneither(_tmp9F1,sizeof(char),22))));}(int)_throw((void*)& Cyc_Yystack_overflow_val);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1028,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 371?(int)Cyc_yytranslate[_check_known_subscript_notnull(372,Cyc_yychar)]:
287;}yyn +=yychar1;if((yyn < 0  || yyn > 6244) || Cyc_yycheck[
_check_known_subscript_notnull(6245,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6245,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1027){int
_tmp46B=0;_npop_handler(0);return _tmp46B;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1028,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(513,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL215: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL216: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp46C;_push_handler(& _tmp46C);{int _tmp46E=0;if(setjmp(_tmp46C.handler))_tmp46E=
1;if(!_tmp46E){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp46D=(void*)_exn_thrown;void*
_tmp470=_tmp46D;_LL219: {struct Cyc_Core_Failure_struct*_tmp471=(struct Cyc_Core_Failure_struct*)
_tmp470;if(_tmp471->tag != Cyc_Core_Failure)goto _LL21B;}_LL21A: x=0;goto _LL218;
_LL21B:;_LL21C:(void)_throw(_tmp470);_LL218:;}};}{struct _handler_cons _tmp472;
_push_handler(& _tmp472);{int _tmp474=0;if(setjmp(_tmp472.handler))_tmp474=1;if(!
_tmp474){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;
_pop_handler();}else{void*_tmp473=(void*)_exn_thrown;void*_tmp476=_tmp473;_LL21E: {
struct Cyc_Core_Failure_struct*_tmp477=(struct Cyc_Core_Failure_struct*)_tmp476;
if(_tmp477->tag != Cyc_Core_Failure)goto _LL220;}_LL21F: y=0;goto _LL21D;_LL220:;
_LL221:(void)_throw(_tmp476);_LL21D:;}};}yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));break;}
case 3: _LL217:{struct Cyc_Absyn_Decl*_tmp9FB;struct Cyc_Absyn_Using_d_struct _tmp9FA;
struct Cyc_Absyn_Using_d_struct*_tmp9F9;struct Cyc_List_List*_tmp9F8;yyval=Cyc_YY18(((
_tmp9F8=_cycalloc(sizeof(*_tmp9F8)),((_tmp9F8->hd=((_tmp9FB=_cycalloc(sizeof(*
_tmp9FB)),((_tmp9FB->r=(void*)((_tmp9F9=_cycalloc(sizeof(*_tmp9F9)),((_tmp9F9[0]=((
_tmp9FA.tag=11,((_tmp9FA.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp9FA.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp9FA)))))),_tmp9F9)))),((
_tmp9FB->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmp9FB)))))),((_tmp9F8->tl=0,_tmp9F8)))))));}Cyc_Lex_leave_using();
break;case 4: _LL222:{struct Cyc_Absyn_Decl*_tmpA05;struct Cyc_Absyn_Using_d_struct
_tmpA04;struct Cyc_Absyn_Using_d_struct*_tmpA03;struct Cyc_List_List*_tmpA02;yyval=
Cyc_YY18(((_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02->hd=((_tmpA05=_cycalloc(
sizeof(*_tmpA05)),((_tmpA05->r=(void*)((_tmpA03=_cycalloc(sizeof(*_tmpA03)),((
_tmpA03[0]=((_tmpA04.tag=11,((_tmpA04.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpA04.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA04)))))),
_tmpA03)))),((_tmpA05->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA05)))))),((
_tmpA02->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA02)))))));}break;case 5: _LL223:{struct Cyc_Absyn_Decl*_tmpA14;
struct Cyc_Absyn_Namespace_d_struct _tmpA13;struct _dyneither_ptr*_tmpA12;struct Cyc_Absyn_Namespace_d_struct*
_tmpA11;struct Cyc_List_List*_tmpA10;yyval=Cyc_YY18(((_tmpA10=_cycalloc(sizeof(*
_tmpA10)),((_tmpA10->hd=((_tmpA14=_cycalloc(sizeof(*_tmpA14)),((_tmpA14->r=(void*)((
_tmpA11=_cycalloc(sizeof(*_tmpA11)),((_tmpA11[0]=((_tmpA13.tag=10,((_tmpA13.f1=((
_tmpA12=_cycalloc(sizeof(*_tmpA12)),((_tmpA12[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA12)))),((_tmpA13.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA13)))))),
_tmpA11)))),((_tmpA14->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA14)))))),((
_tmpA10->tl=0,_tmpA10)))))));}Cyc_Lex_leave_namespace();break;case 6: _LL224: {
struct _dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp485;_push_handler(& _tmp485);{int _tmp487=0;if(setjmp(_tmp485.handler))
_tmp487=1;if(!_tmp487){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp486=(void*)_exn_thrown;void*_tmp489=_tmp486;_LL227: {struct Cyc_Core_Failure_struct*
_tmp48A=(struct Cyc_Core_Failure_struct*)_tmp489;if(_tmp48A->tag != Cyc_Core_Failure)
goto _LL229;}_LL228:{const char*_tmpA15;nspace=((_tmpA15="",_tag_dyneither(_tmpA15,
sizeof(char),1)));}goto _LL226;_LL229:;_LL22A:(void)_throw(_tmp489);_LL226:;}};}{
struct _handler_cons _tmp48C;_push_handler(& _tmp48C);{int _tmp48E=0;if(setjmp(
_tmp48C.handler))_tmp48E=1;if(!_tmp48E){x=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp48D=(void*)_exn_thrown;void*_tmp490=_tmp48D;_LL22C: {struct Cyc_Core_Failure_struct*
_tmp491=(struct Cyc_Core_Failure_struct*)_tmp490;if(_tmp491->tag != Cyc_Core_Failure)
goto _LL22E;}_LL22D: x=0;goto _LL22B;_LL22E:;_LL22F:(void)_throw(_tmp490);_LL22B:;}};}{
struct _handler_cons _tmp492;_push_handler(& _tmp492);{int _tmp494=0;if(setjmp(
_tmp492.handler))_tmp494=1;if(!_tmp494){y=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{void*
_tmp493=(void*)_exn_thrown;void*_tmp496=_tmp493;_LL231: {struct Cyc_Core_Failure_struct*
_tmp497=(struct Cyc_Core_Failure_struct*)_tmp496;if(_tmp497->tag != Cyc_Core_Failure)
goto _LL233;}_LL232: y=0;goto _LL230;_LL233:;_LL234:(void)_throw(_tmp496);_LL230:;}};}{
struct Cyc_Absyn_Decl*_tmpA24;struct Cyc_Absyn_Namespace_d_struct _tmpA23;struct
_dyneither_ptr*_tmpA22;struct Cyc_Absyn_Namespace_d_struct*_tmpA21;struct Cyc_List_List*
_tmpA20;yyval=Cyc_YY18(((_tmpA20=_cycalloc(sizeof(*_tmpA20)),((_tmpA20->hd=((
_tmpA24=_cycalloc(sizeof(*_tmpA24)),((_tmpA24->r=(void*)((_tmpA21=_cycalloc(
sizeof(*_tmpA21)),((_tmpA21[0]=((_tmpA23.tag=10,((_tmpA23.f1=((_tmpA22=_cycalloc(
sizeof(*_tmpA22)),((_tmpA22[0]=nspace,_tmpA22)))),((_tmpA23.f2=x,_tmpA23)))))),
_tmpA21)))),((_tmpA24->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA24)))))),((
_tmpA20->tl=y,_tmpA20)))))));}break;}case 7: _LL225:{const char*_tmpA25;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),((_tmpA25="C",_tag_dyneither(_tmpA25,sizeof(char),2))))
== 0){struct Cyc_Absyn_Decl*_tmpA2F;struct Cyc_Absyn_ExternC_d_struct _tmpA2E;
struct Cyc_Absyn_ExternC_d_struct*_tmpA2D;struct Cyc_List_List*_tmpA2C;yyval=Cyc_YY18(((
_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C->hd=((_tmpA2F=_cycalloc(sizeof(*
_tmpA2F)),((_tmpA2F->r=(void*)((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D[0]=((
_tmpA2E.tag=12,((_tmpA2E.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpA2E)))),_tmpA2D)))),((_tmpA2F->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA2F)))))),((
_tmpA2C->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA2C)))))));}else{{const char*_tmpA30;if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),((_tmpA30="C include",_tag_dyneither(_tmpA30,sizeof(char),10))))
!= 0){const char*_tmpA31;Cyc_Parse_err(((_tmpA31="expecting \"C\" or \"C include\"",
_tag_dyneither(_tmpA31,sizeof(char),29))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}}{struct Cyc_Absyn_Decl*
_tmpA3B;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA3A;struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpA39;struct Cyc_List_List*_tmpA38;yyval=Cyc_YY18(((_tmpA38=_cycalloc(sizeof(*
_tmpA38)),((_tmpA38->hd=((_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B->r=(void*)((
_tmpA39=_cycalloc(sizeof(*_tmpA39)),((_tmpA39[0]=((_tmpA3A.tag=13,((_tmpA3A.f1=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpA3A.f2=0,_tmpA3A)))))),_tmpA39)))),((_tmpA3B->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA3B)))))),((
_tmpA38->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA38)))))));};}}break;case 8: _LL235:{const char*_tmpA3C;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]),((_tmpA3C="C include",_tag_dyneither(_tmpA3C,sizeof(
char),10))))!= 0){const char*_tmpA3D;Cyc_Parse_err(((_tmpA3D="expecting \"C include\"",
_tag_dyneither(_tmpA3D,sizeof(char),22))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_Absyn_Decl*_tmpA47;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA46;
struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA45;struct Cyc_List_List*_tmpA44;
yyval=Cyc_YY18(((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44->hd=((_tmpA47=
_cycalloc(sizeof(*_tmpA47)),((_tmpA47->r=(void*)((_tmpA45=_cycalloc(sizeof(*
_tmpA45)),((_tmpA45[0]=((_tmpA46.tag=13,((_tmpA46.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA46.f2=exs,_tmpA46)))))),
_tmpA45)))),((_tmpA47->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA47)))))),((
_tmpA44->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA44)))))));}break;};case 9: _LL236:{struct Cyc_Absyn_Decl*
_tmpA4A;struct Cyc_List_List*_tmpA49;yyval=Cyc_YY18(((_tmpA49=_cycalloc(sizeof(*
_tmpA49)),((_tmpA49->hd=((_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A->r=(void*)&
Cyc_Absyn_Porton_d_val,((_tmpA4A->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA4A)))))),((
_tmpA49->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA49)))))));}break;case 10: _LL237:{struct Cyc_Absyn_Decl*
_tmpA4D;struct Cyc_List_List*_tmpA4C;yyval=Cyc_YY18(((_tmpA4C=_cycalloc(sizeof(*
_tmpA4C)),((_tmpA4C->hd=((_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D->r=(void*)&
Cyc_Absyn_Portoff_d_val,((_tmpA4D->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA4D)))))),((
_tmpA4C->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA4C)))))));}break;case 11: _LL238: yyval=Cyc_YY18(0);break;case
12: _LL239: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 13: _LL23A: yyval=Cyc_YY52(0);break;case 14: _LL23B:{struct _tuple24*
_tmpA50;struct Cyc_List_List*_tmpA4F;yyval=Cyc_YY52(((_tmpA4F=_cycalloc(sizeof(*
_tmpA4F)),((_tmpA4F->hd=((_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpA50->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpA50->f3=0,_tmpA50)))))))),((_tmpA4F->tl=0,_tmpA4F)))))));}break;case 15:
_LL23C:{struct _tuple24*_tmpA53;struct Cyc_List_List*_tmpA52;yyval=Cyc_YY52(((
_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->hd=((_tmpA53=_cycalloc(sizeof(*
_tmpA53)),((_tmpA53->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpA53->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpA53->f3=0,_tmpA53)))))))),((_tmpA52->tl=0,_tmpA52)))))));}break;case 16:
_LL23D:{struct _tuple24*_tmpA56;struct Cyc_List_List*_tmpA55;yyval=Cyc_YY52(((
_tmpA55=_cycalloc(sizeof(*_tmpA55)),((_tmpA55->hd=((_tmpA56=_cycalloc(sizeof(*
_tmpA56)),((_tmpA56->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpA56->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpA56->f3=0,_tmpA56)))))))),((_tmpA55->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA55)))))));}break;case
17: _LL23E:{struct Cyc_Absyn_Fn_d_struct*_tmpA5C;struct Cyc_Absyn_Fn_d_struct
_tmpA5B;struct Cyc_List_List*_tmpA5A;yyval=Cyc_YY18(((_tmpA5A=_cycalloc(sizeof(*
_tmpA5A)),((_tmpA5A->hd=Cyc_Absyn_new_decl((void*)((_tmpA5C=_cycalloc(sizeof(*
_tmpA5C)),((_tmpA5C[0]=((_tmpA5B.tag=1,((_tmpA5B.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA5B)))),_tmpA5C)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA5A->tl=0,
_tmpA5A)))))));}break;case 18: _LL23F: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 19: _LL240: yyval=Cyc_YY18(0);break;case 20: _LL241:
yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 21:
_LL242:{struct Cyc_Core_Opt*_tmpA5D;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA5D)))),Cyc_yyget_YY29(
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
_LL244:{struct Cyc_Core_Opt*_tmpA5E;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA5E)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 24:
_LL245:{struct Cyc_Core_Opt*_tmpA5F;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((_tmpA5F->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA5F)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 25:
_LL246:{struct Cyc_Core_Opt*_tmpA60;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA60=_cycalloc(sizeof(*_tmpA60)),((_tmpA60->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA60)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 26:
_LL247: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 27: _LL248: Cyc_Lex_leave_using();
break;case 28: _LL249:{struct _dyneither_ptr*_tmpA61;Cyc_Lex_enter_namespace(((
_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA61)))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 29: _LL24A: Cyc_Lex_leave_namespace();
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
_LL24D:{struct Cyc_List_List*_tmpA62;yyval=Cyc_YY18(((_tmpA62=_cycalloc(sizeof(*
_tmpA62)),((_tmpA62->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA62->tl=0,
_tmpA62)))))));}break;case 33: _LL24E: {struct Cyc_List_List*_tmp4C1=0;{struct Cyc_List_List*
_tmp4C2=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
for(0;_tmp4C2 != 0;_tmp4C2=_tmp4C2->tl){struct _dyneither_ptr*_tmp4C3=(struct
_dyneither_ptr*)_tmp4C2->hd;struct _tuple1*_tmpA63;struct _tuple1*qv=(_tmpA63=
_cycalloc(sizeof(*_tmpA63)),((_tmpA63->f1=Cyc_Absyn_Rel_n(0),((_tmpA63->f2=
_tmp4C3,_tmpA63)))));struct Cyc_Absyn_Vardecl*_tmp4C4=Cyc_Absyn_new_vardecl(qv,
Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*_tmpA64;_tmp4C1=((_tmpA64=_cycalloc(
sizeof(*_tmpA64)),((_tmpA64->hd=_tmp4C4,((_tmpA64->tl=_tmp4C1,_tmpA64))))));}}
_tmp4C1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4C1);{struct Cyc_List_List*_tmpA65;yyval=Cyc_YY18(((_tmpA65=_cycalloc(sizeof(*
_tmpA65)),((_tmpA65->hd=Cyc_Absyn_letv_decl(_tmp4C1,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA65->tl=0,
_tmpA65)))))));}break;}case 34: _LL24F:{const char*_tmpA66;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpA66="`H",_tag_dyneither(_tmpA66,sizeof(char),3))))== 0){
const char*_tmpA69;void*_tmpA68;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA68=0,
Cyc_aprintf(((_tmpA69="bad occurrence of heap region",_tag_dyneither(_tmpA69,
sizeof(char),30))),_tag_dyneither(_tmpA68,sizeof(void*),0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{const char*
_tmpA6A;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA6A="`U",
_tag_dyneither(_tmpA6A,sizeof(char),3))))== 0){const char*_tmpA6D;void*_tmpA6C;
Cyc_Parse_err((struct _dyneither_ptr)((_tmpA6C=0,Cyc_aprintf(((_tmpA6D="bad occurrence of unique region",
_tag_dyneither(_tmpA6D,sizeof(char),32))),_tag_dyneither(_tmpA6C,sizeof(void*),0)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));}}{struct _dyneither_ptr*_tmpA70;struct Cyc_Absyn_Tvar*
_tmpA6F;struct Cyc_Absyn_Tvar*tv=(_tmpA6F=_cycalloc(sizeof(*_tmpA6F)),((_tmpA6F->name=((
_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA70)))),((_tmpA6F->identity=
- 1,((_tmpA6F->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA6F)))))));
struct Cyc_Absyn_VarType_struct _tmpA73;struct Cyc_Absyn_VarType_struct*_tmpA72;
void*t=(void*)((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72[0]=((_tmpA73.tag=2,((
_tmpA73.f1=tv,_tmpA73)))),_tmpA72))));struct Cyc_Absyn_RgnHandleType_struct
_tmpA79;struct Cyc_Absyn_RgnHandleType_struct*_tmpA78;struct _dyneither_ptr*
_tmpA76;struct _tuple1*_tmpA75;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpA75=_cycalloc(sizeof(*_tmpA75)),((_tmpA75->f1=Cyc_Absyn_Loc_n,((_tmpA75->f2=((
_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA76)))),_tmpA75)))))),(
void*)((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78[0]=((_tmpA79.tag=16,((
_tmpA79.f1=(void*)t,_tmpA79)))),_tmpA78)))),0);{struct Cyc_List_List*_tmpA7A;
yyval=Cyc_YY18(((_tmpA7A=_cycalloc(sizeof(*_tmpA7A)),((_tmpA7A->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpA7A->tl=0,_tmpA7A)))))));}break;};case 35:
_LL250:{const char*_tmpA7B;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA7B="H",
_tag_dyneither(_tmpA7B,sizeof(char),2))))== 0){const char*_tmpA7C;Cyc_Parse_err(((
_tmpA7C="bad occurrence of heap region `H",_tag_dyneither(_tmpA7C,sizeof(char),
33))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));}}{const char*_tmpA7D;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmpA7D="U",_tag_dyneither(_tmpA7D,sizeof(char),2))))== 0){
const char*_tmpA7E;Cyc_Parse_err(((_tmpA7E="bad occurrence of unique region `U",
_tag_dyneither(_tmpA7E,sizeof(char),35))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));}}if(Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){const char*
_tmpA7F;Cyc_Parse_err(((_tmpA7F="open regions cannot be @resetable",
_tag_dyneither(_tmpA7F,sizeof(char),34))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}{struct
_dyneither_ptr*_tmpA8C;const char*_tmpA8B;void*_tmpA8A[1];struct Cyc_String_pa_struct
_tmpA89;struct Cyc_Absyn_Tvar*_tmpA88;struct Cyc_Absyn_Tvar*tv=(_tmpA88=_cycalloc(
sizeof(*_tmpA88)),((_tmpA88->name=((_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((
_tmpA8C[0]=(struct _dyneither_ptr)((_tmpA89.tag=0,((_tmpA89.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpA8A[0]=& _tmpA89,
Cyc_aprintf(((_tmpA8B="`%s",_tag_dyneither(_tmpA8B,sizeof(char),4))),
_tag_dyneither(_tmpA8A,sizeof(void*),1)))))))),_tmpA8C)))),((_tmpA88->identity=-
1,((_tmpA88->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA88)))))));struct
Cyc_Absyn_VarType_struct _tmpA8F;struct Cyc_Absyn_VarType_struct*_tmpA8E;void*t=(
void*)((_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E[0]=((_tmpA8F.tag=2,((
_tmpA8F.f1=tv,_tmpA8F)))),_tmpA8E))));struct Cyc_Absyn_RgnHandleType_struct
_tmpA95;struct Cyc_Absyn_RgnHandleType_struct*_tmpA94;struct _dyneither_ptr*
_tmpA92;struct _tuple1*_tmpA91;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91->f1=Cyc_Absyn_Loc_n,((_tmpA91->f2=((
_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA92)))),_tmpA91)))))),(
void*)((_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94[0]=((_tmpA95.tag=16,((
_tmpA95.f1=(void*)t,_tmpA95)))),_tmpA94)))),0);{struct Cyc_List_List*_tmpA96;
yyval=Cyc_YY18(((_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA96->tl=0,
_tmpA96)))))));}break;};case 36: _LL251: {struct Cyc_Absyn_Eq_kb_struct*_tmpA9E;
struct Cyc_Absyn_Eq_kb_struct _tmpA9D;struct _dyneither_ptr*_tmpA9C;struct Cyc_Absyn_Tvar*
_tmpA9B;struct Cyc_Absyn_Tvar*tv=(_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((_tmpA9B->name=((
_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpA9C)))),((_tmpA9B->identity=
- 1,((_tmpA9B->kind=(void*)((_tmpA9E=_cycalloc(sizeof(*_tmpA9E)),((_tmpA9E[0]=((
_tmpA9D.tag=0,((_tmpA9D.f1=& Cyc_Tcutil_rk,_tmpA9D)))),_tmpA9E)))),_tmpA9B)))))));
struct _dyneither_ptr*_tmpAA1;struct _tuple1*_tmpAA0;struct Cyc_Absyn_Vardecl*vd=
Cyc_Absyn_new_vardecl(((_tmpAA0=_cycalloc(sizeof(*_tmpAA0)),((_tmpAA0->f1=Cyc_Absyn_Loc_n,((
_tmpAA0->f2=((_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpAA1)))),_tmpAA0)))))),(
void*)& Cyc_Absyn_VoidType_val,0);{struct Cyc_List_List*_tmpAA2;yyval=Cyc_YY18(((
_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAA2->tl=0,
_tmpAA2)))))));}break;}case 37: _LL252: yyval=Cyc_YY31(0);break;case 38: _LL253: yyval=
Cyc_YY31(1);break;case 39: _LL254: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 40: _LL255: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 41: _LL256:{
struct Cyc_Core_Opt*_tmpAA5;struct Cyc_Parse_Declaration_spec*_tmpAA4;yyval=Cyc_YY19(((
_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4->sc=((_tmpAA5=_cycalloc_atomic(
sizeof(*_tmpAA5)),((_tmpAA5->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAA5)))),((_tmpAA4->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAA4->type_specs=
0,((_tmpAA4->is_inline=0,((_tmpAA4->attributes=0,_tmpAA4)))))))))))));}break;
case 42: _LL257: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc != 0){const char*_tmpAA8;void*_tmpAA7;(_tmpAA7=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpAA8="Only one storage class is allowed in a declaration",
_tag_dyneither(_tmpAA8,sizeof(char),51))),_tag_dyneither(_tmpAA7,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmpAAB;struct Cyc_Parse_Declaration_spec*_tmpAAA;yyval=Cyc_YY19(((
_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA->sc=((_tmpAAB=_cycalloc_atomic(
sizeof(*_tmpAAB)),((_tmpAAB->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpAAB)))),((_tmpAAA->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAAA->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAAA->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpAAA->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,
_tmpAAA)))))))))))));}break;case 43: _LL258:{struct Cyc_List_List*_tmpAAE;struct Cyc_Parse_Declaration_spec*
_tmpAAD;yyval=Cyc_YY19(((_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD->sc=0,((
_tmpAAD->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAAD->type_specs=((
_tmpAAE=_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAAE->tl=0,_tmpAAE)))))),((
_tmpAAD->is_inline=0,((_tmpAAD->attributes=0,_tmpAAD)))))))))))));}break;case 44:
_LL259:{struct Cyc_List_List*_tmpAB1;struct Cyc_Parse_Declaration_spec*_tmpAB0;
yyval=Cyc_YY19(((_tmpAB0=_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAB0->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAB0->type_specs=((_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpAB1->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,_tmpAB1)))))),((
_tmpAB0->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpAB0->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAB0)))))))))))));}
break;case 45: _LL25A:{struct Cyc_Parse_Declaration_spec*_tmpAB2;yyval=Cyc_YY19(((
_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2->sc=0,((_tmpAB2->tq=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAB2->type_specs=
0,((_tmpAB2->is_inline=0,((_tmpAB2->attributes=0,_tmpAB2)))))))))))));}break;
case 46: _LL25B:{struct Cyc_Parse_Declaration_spec*_tmpAB3;yyval=Cyc_YY19(((_tmpAB3=
_cycalloc(sizeof(*_tmpAB3)),((_tmpAB3->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAB3->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq),((_tmpAB3->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpAB3->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpAB3->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAB3)))))))))))));}
break;case 47: _LL25C:{struct Cyc_Parse_Declaration_spec*_tmpAB4;yyval=Cyc_YY19(((
_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4->sc=0,((_tmpAB4->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAB4->type_specs=0,((_tmpAB4->is_inline=1,((
_tmpAB4->attributes=0,_tmpAB4)))))))))))));}break;case 48: _LL25D:{struct Cyc_Parse_Declaration_spec*
_tmpAB5;yyval=Cyc_YY19(((_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAB5->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAB5->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAB5->is_inline=1,((_tmpAB5->attributes=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAB5)))))))))))));}
break;case 49: _LL25E:{struct Cyc_Parse_Declaration_spec*_tmpAB6;yyval=Cyc_YY19(((
_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6->sc=0,((_tmpAB6->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAB6->type_specs=0,((_tmpAB6->is_inline=0,((
_tmpAB6->attributes=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpAB6)))))))))))));}break;case 50: _LL25F:{struct Cyc_Parse_Declaration_spec*
_tmpAB7;yyval=Cyc_YY19(((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAB7->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAB7->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAB7->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpAB7->attributes=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes),_tmpAB7)))))))))))));}
break;case 51: _LL260: yyval=Cyc_YY22(Cyc_Parse_Auto_sc);break;case 52: _LL261: yyval=
Cyc_YY22(Cyc_Parse_Register_sc);break;case 53: _LL262: yyval=Cyc_YY22(Cyc_Parse_Static_sc);
break;case 54: _LL263: yyval=Cyc_YY22(Cyc_Parse_Extern_sc);break;case 55: _LL264:{
const char*_tmpAB8;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAB8="C",
_tag_dyneither(_tmpAB8,sizeof(char),2))))!= 0){const char*_tmpAB9;Cyc_Parse_err(((
_tmpAB9="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAB9,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}}yyval=Cyc_YY22(Cyc_Parse_ExternC_sc);break;case 56:
_LL265: yyval=Cyc_YY22(Cyc_Parse_Typedef_sc);break;case 57: _LL266: yyval=Cyc_YY22(
Cyc_Parse_Abstract_sc);break;case 58: _LL267: yyval=Cyc_YY45(0);break;case 59: _LL268:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 60:
_LL269: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 61: _LL26A:{struct Cyc_List_List*_tmpABA;yyval=Cyc_YY45(((_tmpABA=_cycalloc(
sizeof(*_tmpABA)),((_tmpABA->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpABA->tl=0,_tmpABA)))))));}
break;case 62: _LL26B:{struct Cyc_List_List*_tmpABB;yyval=Cyc_YY45(((_tmpABB=
_cycalloc(sizeof(*_tmpABB)),((_tmpABB->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpABB->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpABB)))))));}break;
case 63: _LL26C: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={6,- 1};
static struct _tuple25 att_map[17]={{{_tmp505,_tmp505,_tmp505 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{
_tmp506,_tmp506,_tmp506 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp507,_tmp507,
_tmp507 + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp508,_tmp508,_tmp508 + 9},(
void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp509,_tmp509,_tmp509 + 6},(void*)& Cyc_Absyn_Const_att_val},{{
_tmp50A,_tmp50A,_tmp50A + 8},(void*)& att_aligned},{{_tmp50B,_tmp50B,_tmp50B + 7},(
void*)& Cyc_Absyn_Packed_att_val},{{_tmp50C,_tmp50C,_tmp50C + 7},(void*)& Cyc_Absyn_Shared_att_val},{{
_tmp50D,_tmp50D,_tmp50D + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp50E,_tmp50E,
_tmp50E + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp50F,_tmp50F,_tmp50F + 10},(void*)&
Cyc_Absyn_Dllimport_att_val},{{_tmp510,_tmp510,_tmp510 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{
_tmp511,_tmp511,_tmp511 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{
_tmp512,_tmp512,_tmp512 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp513,
_tmp513,_tmp513 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp514,_tmp514,
_tmp514 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp515,_tmp515,
_tmp515 + 5},(void*)& Cyc_Absyn_Pure_att_val}};struct _dyneither_ptr _tmp503=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp503,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp503,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp503,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp503,sizeof(char),(int)(_get_dyneither_size(
_tmp503,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp503,sizeof(char),(int)(_get_dyneither_size(_tmp503,sizeof(char))- 3)))== '_')
_tmp503=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp503,2,
_get_dyneither_size(_tmp503,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp503,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){{const char*_tmpABC;Cyc_Parse_err(((
_tmpABC="unrecognized attribute",_tag_dyneither(_tmpABC,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=Cyc_YY46((
void*)& Cyc_Absyn_Cdecl_att_val);}break;};}case 64: _LL26D: yyval=Cyc_YY46((void*)&
Cyc_Absyn_Const_att_val);break;case 65: _LL26E: {struct _dyneither_ptr _tmp517=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp519;struct
_tuple6 _tmp518=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp519=_tmp518.f2;{void*a;{const char*_tmpABE;const char*
_tmpABD;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpABE="regparm",
_tag_dyneither(_tmpABE,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp517,((_tmpABD="__regparm__",_tag_dyneither(_tmpABD,sizeof(
char),12))))== 0){if(_tmp519 < 0  || _tmp519 > 3){const char*_tmpABF;Cyc_Parse_err(((
_tmpABF="regparm requires value between 0 and 3",_tag_dyneither(_tmpABF,sizeof(
char),39))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Regparm_att_struct _tmpAC2;
struct Cyc_Absyn_Regparm_att_struct*_tmpAC1;a=(void*)((_tmpAC1=_cycalloc_atomic(
sizeof(*_tmpAC1)),((_tmpAC1[0]=((_tmpAC2.tag=0,((_tmpAC2.f1=_tmp519,_tmpAC2)))),
_tmpAC1))));};}else{const char*_tmpAC4;const char*_tmpAC3;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp517,((_tmpAC4="aligned",_tag_dyneither(_tmpAC4,sizeof(char),8))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpAC3="__aligned__",
_tag_dyneither(_tmpAC3,sizeof(char),12))))== 0){if(_tmp519 < 0){const char*_tmpAC5;
Cyc_Parse_err(((_tmpAC5="aligned requires positive power of two",_tag_dyneither(
_tmpAC5,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp519;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
const char*_tmpAC6;Cyc_Parse_err(((_tmpAC6="aligned requires positive power of two",
_tag_dyneither(_tmpAC6,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmpAC9;struct Cyc_Absyn_Aligned_att_struct*_tmpAC8;a=(void*)((_tmpAC8=
_cycalloc_atomic(sizeof(*_tmpAC8)),((_tmpAC8[0]=((_tmpAC9.tag=6,((_tmpAC9.f1=
_tmp519,_tmpAC9)))),_tmpAC8))));};};}else{const char*_tmpACB;const char*_tmpACA;
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpACB="initializes",
_tag_dyneither(_tmpACB,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp517,((_tmpACA="__initializes__",_tag_dyneither(_tmpACA,
sizeof(char),16))))== 0){struct Cyc_Absyn_Initializes_att_struct _tmpACE;struct Cyc_Absyn_Initializes_att_struct*
_tmpACD;a=(void*)((_tmpACD=_cycalloc_atomic(sizeof(*_tmpACD)),((_tmpACD[0]=((
_tmpACE.tag=20,((_tmpACE.f1=_tmp519,_tmpACE)))),_tmpACD))));}else{const char*
_tmpAD0;const char*_tmpACF;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpAD0="noliveunique",
_tag_dyneither(_tmpAD0,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp517,((_tmpACF="__noliveunique__",_tag_dyneither(_tmpACF,
sizeof(char),17))))== 0){struct Cyc_Absyn_Noliveunique_att_struct _tmpAD3;struct
Cyc_Absyn_Noliveunique_att_struct*_tmpAD2;a=(void*)((_tmpAD2=_cycalloc_atomic(
sizeof(*_tmpAD2)),((_tmpAD2[0]=((_tmpAD3.tag=21,((_tmpAD3.f1=_tmp519,_tmpAD3)))),
_tmpAD2))));}else{{const char*_tmpAD4;Cyc_Parse_err(((_tmpAD4="unrecognized attribute",
_tag_dyneither(_tmpAD4,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}}}}yyval=Cyc_YY46(a);break;};}case 66: _LL26F: {struct
_dyneither_ptr _tmp52E=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp52F=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpAD6;const char*_tmpAD5;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52E,((_tmpAD6="section",
_tag_dyneither(_tmpAD6,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp52E,((_tmpAD5="__section__",_tag_dyneither(_tmpAD5,sizeof(
char),12))))== 0){struct Cyc_Absyn_Section_att_struct _tmpAD9;struct Cyc_Absyn_Section_att_struct*
_tmpAD8;a=(void*)((_tmpAD8=_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8[0]=((_tmpAD9.tag=
8,((_tmpAD9.f1=_tmp52F,_tmpAD9)))),_tmpAD8))));}else{{const char*_tmpADA;Cyc_Parse_err(((
_tmpADA="unrecognized attribute",_tag_dyneither(_tmpADA,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 67: _LL270: {struct
_dyneither_ptr _tmp535=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp536=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpADB;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp535,((_tmpADB="__mode__",
_tag_dyneither(_tmpADB,sizeof(char),9))))== 0){struct Cyc_Absyn_Mode_att_struct
_tmpADE;struct Cyc_Absyn_Mode_att_struct*_tmpADD;a=(void*)((_tmpADD=_cycalloc(
sizeof(*_tmpADD)),((_tmpADD[0]=((_tmpADE.tag=23,((_tmpADE.f1=_tmp536,_tmpADE)))),
_tmpADD))));}else{{const char*_tmpADF;Cyc_Parse_err(((_tmpADF="unrecognized attribute",
_tag_dyneither(_tmpADF,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 68: _LL271: {struct
_dyneither_ptr _tmp53B=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp53C=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp53E;struct _tuple6
_tmp53D=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp53E=_tmp53D.f2;{int _tmp540;struct _tuple6 _tmp53F=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp540=_tmp53F.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;{const char*_tmpAE1;const char*_tmpAE0;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp53B,((_tmpAE1="format",_tag_dyneither(
_tmpAE1,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp53B,((
_tmpAE0="__format__",_tag_dyneither(_tmpAE0,sizeof(char),11))))== 0){const char*
_tmpAE2;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp53C,((_tmpAE2="printf",
_tag_dyneither(_tmpAE2,sizeof(char),7))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpAE5;struct Cyc_Absyn_Format_att_struct*_tmpAE4;a=(void*)((_tmpAE4=
_cycalloc_atomic(sizeof(*_tmpAE4)),((_tmpAE4[0]=((_tmpAE5.tag=19,((_tmpAE5.f1=
Cyc_Absyn_Printf_ft,((_tmpAE5.f2=_tmp53E,((_tmpAE5.f3=_tmp540,_tmpAE5)))))))),
_tmpAE4))));}else{const char*_tmpAE6;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp53C,((
_tmpAE6="scanf",_tag_dyneither(_tmpAE6,sizeof(char),6))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpAE9;struct Cyc_Absyn_Format_att_struct*_tmpAE8;a=(void*)((_tmpAE8=
_cycalloc_atomic(sizeof(*_tmpAE8)),((_tmpAE8[0]=((_tmpAE9.tag=19,((_tmpAE9.f1=
Cyc_Absyn_Scanf_ft,((_tmpAE9.f2=_tmp53E,((_tmpAE9.f3=_tmp540,_tmpAE9)))))))),
_tmpAE8))));}else{const char*_tmpAEA;Cyc_Parse_err(((_tmpAEA="unrecognized format type",
_tag_dyneither(_tmpAEA,sizeof(char),25))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{
const char*_tmpAEB;Cyc_Parse_err(((_tmpAEB="unrecognized attribute",
_tag_dyneither(_tmpAEB,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY46(
a);break;};};}case 69: _LL272: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 70: _LL273:{struct Cyc_Absyn_TypedefType_struct _tmpAEE;
struct Cyc_Absyn_TypedefType_struct*_tmpAED;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED[0]=((_tmpAEE.tag=18,((
_tmpAEE.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpAEE.f2=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAEE.f3=0,((_tmpAEE.f4=
0,_tmpAEE)))))))))),_tmpAED)))),Cyc_Position_segment_of_abs((yyls[
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
_LL278:{struct Cyc_Parse_Short_spec_struct _tmpAF1;struct Cyc_Parse_Short_spec_struct*
_tmpAF0;yyval=Cyc_YY23((void*)((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0[0]=((
_tmpAF1.tag=2,((_tmpAF1.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpAF1)))),
_tmpAF0)))));}break;case 76: _LL279: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 77: _LL27A:{struct Cyc_Parse_Long_spec_struct
_tmpAF4;struct Cyc_Parse_Long_spec_struct*_tmpAF3;yyval=Cyc_YY23((void*)((_tmpAF3=
_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3[0]=((_tmpAF4.tag=3,((_tmpAF4.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpAF4)))),
_tmpAF3)))));}break;case 78: _LL27B: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 79: _LL27C: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 80:
_LL27D:{struct Cyc_Parse_Signed_spec_struct _tmpAF7;struct Cyc_Parse_Signed_spec_struct*
_tmpAF6;yyval=Cyc_YY23((void*)((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6[0]=((
_tmpAF7.tag=0,((_tmpAF7.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpAF7)))),
_tmpAF6)))));}break;case 81: _LL27E:{struct Cyc_Parse_Unsigned_spec_struct _tmpAFA;
struct Cyc_Parse_Unsigned_spec_struct*_tmpAF9;yyval=Cyc_YY23((void*)((_tmpAF9=
_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=((_tmpAFA.tag=1,((_tmpAFA.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpAFA)))),
_tmpAF9)))));}break;case 82: _LL27F: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 83: _LL280: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84: _LL281: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 85: _LL282:
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 86:
_LL283:{struct Cyc_Absyn_TupleType_struct _tmpAFD;struct Cyc_Absyn_TupleType_struct*
_tmpAFC;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAFC=_cycalloc(sizeof(*
_tmpAFC)),((_tmpAFC[0]=((_tmpAFD.tag=11,((_tmpAFD.f1=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))),_tmpAFD)))),_tmpAFC)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 87: _LL284:{struct Cyc_Absyn_RgnHandleType_struct
_tmpB00;struct Cyc_Absyn_RgnHandleType_struct*_tmpAFF;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpAFF=_cycalloc(sizeof(*_tmpAFF)),((_tmpAFF[0]=((_tmpB00.tag=16,((
_tmpB00.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpB00)))),_tmpAFF)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 88:
_LL285:{struct Cyc_Absyn_RgnHandleType_struct _tmpB03;struct Cyc_Absyn_RgnHandleType_struct*
_tmpB02;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB02=_cycalloc(sizeof(*
_tmpB02)),((_tmpB02[0]=((_tmpB03.tag=16,((_tmpB03.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0),_tmpB03)))),_tmpB02)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 89:
_LL286: {void*_tmp55B=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0);{
struct Cyc_Absyn_DynRgnType_struct _tmpB06;struct Cyc_Absyn_DynRgnType_struct*
_tmpB05;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB05=_cycalloc(sizeof(*
_tmpB05)),((_tmpB05[0]=((_tmpB06.tag=17,((_tmpB06.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB06.f2=(void*)
_tmp55B,_tmpB06)))))),_tmpB05)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case 90:
_LL287:{struct Cyc_Absyn_DynRgnType_struct _tmpB09;struct Cyc_Absyn_DynRgnType_struct*
_tmpB08;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB08=_cycalloc(sizeof(*
_tmpB08)),((_tmpB08[0]=((_tmpB09.tag=17,((_tmpB09.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB09.f2=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB09)))))),
_tmpB08)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 91: _LL288:{struct Cyc_Absyn_TagType_struct
_tmpB0C;struct Cyc_Absyn_TagType_struct*_tmpB0B;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B[0]=((_tmpB0C.tag=20,((
_tmpB0C.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpB0C)))),_tmpB0B)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 92:
_LL289:{struct Cyc_Absyn_TagType_struct _tmpB0F;struct Cyc_Absyn_TagType_struct*
_tmpB0E;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB0E=_cycalloc(sizeof(*
_tmpB0E)),((_tmpB0E[0]=((_tmpB0F.tag=20,((_tmpB0F.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_iko,0),_tmpB0F)))),_tmpB0E)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 93:
_LL28A:{struct Cyc_Absyn_ValueofType_struct _tmpB12;struct Cyc_Absyn_ValueofType_struct*
_tmpB11;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB11=_cycalloc(sizeof(*
_tmpB11)),((_tmpB11[0]=((_tmpB12.tag=19,((_tmpB12.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB12)))),_tmpB11)))),
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
_tmpB13;yyval=Cyc_YY25(((_tmpB13.print_const=1,((_tmpB13.q_volatile=0,((_tmpB13.q_restrict=
0,((_tmpB13.real_const=1,((_tmpB13.loc=loc,_tmpB13)))))))))));}break;}case 96:
_LL28D:{struct Cyc_Absyn_Tqual _tmpB14;yyval=Cyc_YY25(((_tmpB14.print_const=0,((
_tmpB14.q_volatile=1,((_tmpB14.q_restrict=0,((_tmpB14.real_const=0,((_tmpB14.loc=
0,_tmpB14)))))))))));}break;case 97: _LL28E:{struct Cyc_Absyn_Tqual _tmpB15;yyval=
Cyc_YY25(((_tmpB15.print_const=0,((_tmpB15.q_volatile=0,((_tmpB15.q_restrict=1,((
_tmpB15.real_const=0,((_tmpB15.loc=0,_tmpB15)))))))))));}break;case 98: _LL28F: {
struct Cyc_Absyn_Enum_td_struct*_tmpB24;struct Cyc_Absyn_Enumdecl*_tmpB23;struct
Cyc_Core_Opt*_tmpB22;struct Cyc_Absyn_Enum_td_struct _tmpB21;struct Cyc_Absyn_TypeDecl*
_tmpB20;struct Cyc_Absyn_TypeDecl*_tmp569=(_tmpB20=_cycalloc(sizeof(*_tmpB20)),((
_tmpB20->r=(void*)((_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24[0]=((_tmpB21.tag=
1,((_tmpB21.f1=((_tmpB23=_cycalloc(sizeof(*_tmpB23)),((_tmpB23->sc=Cyc_Absyn_Public,((
_tmpB23->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpB23->fields=((_tmpB22=_cycalloc(sizeof(*_tmpB22)),((
_tmpB22->v=Cyc_yyget_YY48(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),_tmpB22)))),_tmpB23)))))))),_tmpB21)))),_tmpB24)))),((_tmpB20->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB20)))));{
struct Cyc_Absyn_TypeDeclType_struct _tmpB27;struct Cyc_Absyn_TypeDeclType_struct*
_tmpB26;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB26=_cycalloc(sizeof(*
_tmpB26)),((_tmpB26[0]=((_tmpB27.tag=26,((_tmpB27.f1=_tmp569,((_tmpB27.f2=0,
_tmpB27)))))),_tmpB26)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case 99:
_LL290:{struct Cyc_Absyn_EnumType_struct _tmpB2A;struct Cyc_Absyn_EnumType_struct*
_tmpB29;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB29=_cycalloc(sizeof(*
_tmpB29)),((_tmpB29[0]=((_tmpB2A.tag=14,((_tmpB2A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB2A.f2=0,_tmpB2A)))))),
_tmpB29)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 100: _LL291:{struct Cyc_Absyn_AnonEnumType_struct
_tmpB2D;struct Cyc_Absyn_AnonEnumType_struct*_tmpB2C;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB2C=_cycalloc(sizeof(*_tmpB2C)),((_tmpB2C[0]=((_tmpB2D.tag=15,((
_tmpB2D.f1=Cyc_yyget_YY48(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),_tmpB2D)))),_tmpB2C)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 101:
_LL292:{struct Cyc_Absyn_Enumfield*_tmpB2E;yyval=Cyc_YY47(((_tmpB2E=_cycalloc(
sizeof(*_tmpB2E)),((_tmpB2E->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB2E->tag=0,((_tmpB2E->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB2E)))))))));}break;case 102: _LL293:{struct Cyc_Absyn_Enumfield*
_tmpB2F;yyval=Cyc_YY47(((_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB2F->tag=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB2F->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB2F)))))))));}
break;case 103: _LL294:{struct Cyc_List_List*_tmpB30;yyval=Cyc_YY48(((_tmpB30=
_cycalloc(sizeof(*_tmpB30)),((_tmpB30->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB30->tl=0,_tmpB30)))))));}
break;case 104: _LL295:{struct Cyc_List_List*_tmpB31;yyval=Cyc_YY48(((_tmpB31=
_cycalloc(sizeof(*_tmpB31)),((_tmpB31->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB31->tl=0,_tmpB31)))))));}
break;case 105: _LL296:{struct Cyc_List_List*_tmpB32;yyval=Cyc_YY48(((_tmpB32=
_cycalloc(sizeof(*_tmpB32)),((_tmpB32->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB32->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB32)))))));}break;
case 106: _LL297:{struct Cyc_Absyn_AnonAggrType_struct _tmpB35;struct Cyc_Absyn_AnonAggrType_struct*
_tmpB34;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB34=_cycalloc(sizeof(*
_tmpB34)),((_tmpB34[0]=((_tmpB35.tag=13,((_tmpB35.f1=Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB35.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB35)))))),
_tmpB34)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 107: _LL298: {struct Cyc_List_List*_tmp57C=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp57D=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));struct Cyc_Absyn_TypeDecl*
_tmp57E=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp57C,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp57D,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_TypeDeclType_struct
_tmpB38;struct Cyc_Absyn_TypeDeclType_struct*_tmpB37;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37[0]=((_tmpB38.tag=26,((
_tmpB38.f1=_tmp57E,((_tmpB38.f2=0,_tmpB38)))))),_tmpB37)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
108: _LL299: {struct Cyc_List_List*_tmp581=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp582=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));struct Cyc_Absyn_TypeDecl*_tmp583=Cyc_Absyn_aggr_tdecl(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tmp581,(struct Cyc_Absyn_AggrdeclImpl*)Cyc_Absyn_aggrdecl_impl(_tmp582,Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_TypeDeclType_struct
_tmpB3B;struct Cyc_Absyn_TypeDeclType_struct*_tmpB3A;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB3A=_cycalloc(sizeof(*_tmpB3A)),((_tmpB3A[0]=((_tmpB3B.tag=26,((
_tmpB3B.f1=_tmp583,((_tmpB3B.f2=0,_tmpB3B)))))),_tmpB3A)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
109: _LL29A:{struct Cyc_Absyn_AggrType_struct _tmpB45;struct Cyc_Core_Opt*_tmpB44;
struct Cyc_Absyn_AggrInfo _tmpB43;struct Cyc_Absyn_AggrType_struct*_tmpB42;yyval=
Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB42=_cycalloc(sizeof(*_tmpB42)),((
_tmpB42[0]=((_tmpB45.tag=12,((_tmpB45.f1=((_tmpB43.aggr_info=Cyc_Absyn_UnknownAggr(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB44=
_cycalloc_atomic(sizeof(*_tmpB44)),((_tmpB44->v=(void*)1,_tmpB44))))),((_tmpB43.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB43)))),
_tmpB45)))),_tmpB42)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 110:
_LL29B:{struct Cyc_Absyn_AggrType_struct _tmpB4B;struct Cyc_Absyn_AggrInfo _tmpB4A;
struct Cyc_Absyn_AggrType_struct*_tmpB49;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((
_tmpB49=_cycalloc(sizeof(*_tmpB49)),((_tmpB49[0]=((_tmpB4B.tag=12,((_tmpB4B.f1=((
_tmpB4A.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),((_tmpB4A.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB4A)))),
_tmpB4B)))),_tmpB49)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 111:
_LL29C: yyval=Cyc_YY40(0);break;case 112: _LL29D: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 113: _LL29E:
yyval=Cyc_YY24(Cyc_Absyn_StructA);break;case 114: _LL29F: yyval=Cyc_YY24(Cyc_Absyn_UnionA);
break;case 115: _LL2A0: yyval=Cyc_YY26(0);break;case 116: _LL2A1: {struct Cyc_List_List*
_tmp58D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));struct Cyc_List_List*
_tmp58E=Cyc_Parse_get_argrfield_tags(_tmp58D);if(_tmp58E != 0)((void(*)(void(*f)(
struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct
Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp58E,
_tmp58D);yyval=Cyc_YY26(_tmp58D);break;}case 117: _LL2A2:{struct Cyc_List_List*
_tmpB4C;yyval=Cyc_YY27(((_tmpB4C=_cycalloc(sizeof(*_tmpB4C)),((_tmpB4C->hd=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB4C->tl=0,
_tmpB4C)))))));}break;case 118: _LL2A3:{struct Cyc_List_List*_tmpB4D;yyval=Cyc_YY27(((
_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB4D->tl=Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB4D)))))));}
break;case 119: _LL2A4: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 120: _LL2A5:{struct Cyc_List_List*_tmpB4E;yyval=Cyc_YY21(((
_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB4E->tl=0,_tmpB4E)))))));}
break;case 121: _LL2A6:{struct Cyc_List_List*_tmpB4F;yyval=Cyc_YY21(((_tmpB4F=
_cycalloc(sizeof(*_tmpB4F)),((_tmpB4F->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB4F->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB4F)))))));}
break;case 122: _LL2A7:{struct _tuple21*_tmpB50;yyval=Cyc_YY20(((_tmpB50=_cycalloc(
sizeof(*_tmpB50)),((_tmpB50->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB50->f2=0,_tmpB50)))))));}
break;case 123: _LL2A8:{struct _tuple21*_tmpB51;yyval=Cyc_YY20(((_tmpB51=_cycalloc(
sizeof(*_tmpB51)),((_tmpB51->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB51->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB51)))))));}
break;case 124: _LL2A9: {struct _RegionHandle _tmp595=_new_region("temp");struct
_RegionHandle*temp=& _tmp595;_push_region(temp);{struct _tuple22 _tmp597;struct Cyc_Absyn_Tqual
_tmp598;struct Cyc_List_List*_tmp599;struct Cyc_List_List*_tmp59A;struct _tuple22*
_tmp596=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp597=*_tmp596;_tmp598=_tmp597.f1;_tmp599=_tmp597.f2;_tmp59A=_tmp597.f3;if(
_tmp598.loc == 0)_tmp598.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);{struct Cyc_List_List*
_tmp59C;struct Cyc_List_List*_tmp59D;struct _tuple0 _tmp59B=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
temp,temp,Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]));_tmp59C=_tmp59B.f1;_tmp59D=_tmp59B.f2;{void*_tmp59E=Cyc_Parse_speclist2typ(
_tmp599,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp59F=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(temp,_tmp598,_tmp59E,_tmp59C,
_tmp59A),_tmp59D);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple13*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp59F));};};}
_npop_handler(0);break;;_pop_region(temp);}case 125: _LL2AA:{struct Cyc_List_List*
_tmpB54;struct _tuple22*_tmpB53;yyval=Cyc_YY35(((_tmpB53=_cycalloc(sizeof(*
_tmpB53)),((_tmpB53->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB53->f2=((
_tmpB54=_cycalloc(sizeof(*_tmpB54)),((_tmpB54->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB54->tl=0,_tmpB54)))))),((
_tmpB53->f3=0,_tmpB53)))))))));}break;case 126: _LL2AB:{struct Cyc_List_List*
_tmpB57;struct _tuple22*_tmpB56;yyval=Cyc_YY35(((_tmpB56=_cycalloc(sizeof(*
_tmpB56)),((_tmpB56->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpB56->f2=((_tmpB57=_cycalloc(sizeof(*_tmpB57)),((
_tmpB57->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB57->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpB57)))))),((_tmpB56->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB56)))))))));}break;case 127: _LL2AC:{struct _tuple22*_tmpB58;yyval=Cyc_YY35(((
_tmpB58=_cycalloc(sizeof(*_tmpB58)),((_tmpB58->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB58->f2=0,((_tmpB58->f3=
0,_tmpB58)))))))));}break;case 128: _LL2AD:{struct _tuple22*_tmpB59;yyval=Cyc_YY35(((
_tmpB59=_cycalloc(sizeof(*_tmpB59)),((_tmpB59->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpB59->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpB59->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpB59)))))))));}break;case 129: _LL2AE:{struct _tuple22*
_tmpB5A;yyval=Cyc_YY35(((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB5A->f2=0,((_tmpB5A->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB5A)))))))));}break;
case 130: _LL2AF:{struct _tuple22*_tmpB5B;yyval=Cyc_YY35(((_tmpB5B=_cycalloc(
sizeof(*_tmpB5B)),((_tmpB5B->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpB5B->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpB5B->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpB5B)))))))));}break;case 131: _LL2B0:{struct Cyc_List_List*_tmpB5E;struct
_tuple22*_tmpB5D;yyval=Cyc_YY35(((_tmpB5D=_cycalloc(sizeof(*_tmpB5D)),((_tmpB5D->f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB5D->f2=((
_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((_tmpB5E->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB5E->tl=0,_tmpB5E)))))),((
_tmpB5D->f3=0,_tmpB5D)))))))));}break;case 132: _LL2B1:{struct Cyc_List_List*
_tmpB61;struct _tuple22*_tmpB60;yyval=Cyc_YY35(((_tmpB60=_cycalloc(sizeof(*
_tmpB60)),((_tmpB60->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpB60->f2=((_tmpB61=_cycalloc(sizeof(*_tmpB61)),((
_tmpB61->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB61->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpB61)))))),((_tmpB60->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB60)))))))));}break;case 133: _LL2B2:{struct _tuple22*_tmpB62;yyval=Cyc_YY35(((
_tmpB62=_cycalloc(sizeof(*_tmpB62)),((_tmpB62->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB62->f2=0,((_tmpB62->f3=
0,_tmpB62)))))))));}break;case 134: _LL2B3:{struct _tuple22*_tmpB63;yyval=Cyc_YY35(((
_tmpB63=_cycalloc(sizeof(*_tmpB63)),((_tmpB63->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpB63->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpB63->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpB63)))))))));}break;case 135: _LL2B4:{struct _tuple22*
_tmpB64;yyval=Cyc_YY35(((_tmpB64=_cycalloc(sizeof(*_tmpB64)),((_tmpB64->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB64->f2=0,((_tmpB64->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB64)))))))));}break;
case 136: _LL2B5:{struct _tuple22*_tmpB65;yyval=Cyc_YY35(((_tmpB65=_cycalloc(
sizeof(*_tmpB65)),((_tmpB65->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpB65->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpB65->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpB65)))))))));}break;case 137: _LL2B6: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 138: _LL2B7:{
struct Cyc_List_List*_tmpB66;yyval=Cyc_YY21(((_tmpB66=_cycalloc(sizeof(*_tmpB66)),((
_tmpB66->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB66->tl=0,_tmpB66)))))));}break;case 139: _LL2B8:{struct Cyc_List_List*
_tmpB67;yyval=Cyc_YY21(((_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB67->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB67)))))));}
break;case 140: _LL2B9:{struct _tuple21*_tmpB68;yyval=Cyc_YY20(((_tmpB68=_cycalloc(
sizeof(*_tmpB68)),((_tmpB68->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB68->f2=0,_tmpB68)))))));}
break;case 141: _LL2BA:{struct Cyc_Parse_Declarator*_tmpB6E;struct _tuple1*_tmpB6D;
struct _tuple21*_tmpB6C;yyval=Cyc_YY20(((_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((
_tmpB6C->f1=((_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((_tmpB6E->id=((_tmpB6D=
_cycalloc(sizeof(*_tmpB6D)),((_tmpB6D->f1=Cyc_Absyn_Rel_n(0),((_tmpB6D->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),
_tmpB6D)))))),((_tmpB6E->tms=0,_tmpB6E)))))),((_tmpB6C->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB6C)))))));}
break;case 142: _LL2BB:{struct Cyc_Parse_Declarator*_tmpB74;struct _tuple1*_tmpB73;
struct _tuple21*_tmpB72;yyval=Cyc_YY20(((_tmpB72=_cycalloc(sizeof(*_tmpB72)),((
_tmpB72->f1=((_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->id=((_tmpB73=
_cycalloc(sizeof(*_tmpB73)),((_tmpB73->f1=Cyc_Absyn_Rel_n(0),((_tmpB73->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),
_tmpB73)))))),((_tmpB74->tms=0,_tmpB74)))))),((_tmpB72->f2=0,_tmpB72)))))));}
break;case 143: _LL2BC:{struct _tuple21*_tmpB75;yyval=Cyc_YY20(((_tmpB75=_cycalloc(
sizeof(*_tmpB75)),((_tmpB75->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB75->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB75)))))));}
break;case 144: _LL2BD: {int _tmp5BC=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);struct Cyc_List_List*
_tmp5BD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));struct Cyc_Core_Opt*_tmpB76;struct Cyc_Absyn_TypeDecl*
_tmp5BE=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp5BD,((_tmpB76=
_cycalloc(sizeof(*_tmpB76)),((_tmpB76->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB76)))),_tmp5BC,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_TypeDeclType_struct
_tmpB79;struct Cyc_Absyn_TypeDeclType_struct*_tmpB78;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78[0]=((_tmpB79.tag=26,((
_tmpB79.f1=_tmp5BE,((_tmpB79.f2=0,_tmpB79)))))),_tmpB78)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
145: _LL2BE: {int _tmp5C2=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);{struct Cyc_Absyn_DatatypeType_struct _tmpB83;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpB82;struct Cyc_Absyn_DatatypeInfo _tmpB81;struct Cyc_Absyn_DatatypeType_struct*
_tmpB80;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB80=_cycalloc(sizeof(*
_tmpB80)),((_tmpB80[0]=((_tmpB83.tag=3,((_tmpB83.f1=((_tmpB81.datatype_info=Cyc_Absyn_UnknownDatatype(((
_tmpB82.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB82.is_extensible=_tmp5C2,_tmpB82))))),((_tmpB81.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB81)))),
_tmpB83)))),_tmpB80)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
146: _LL2BF: {int _tmp5C7=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);{struct Cyc_Absyn_DatatypeFieldType_struct _tmpB8D;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpB8C;struct Cyc_Absyn_DatatypeFieldInfo _tmpB8B;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpB8A;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB8A=_cycalloc(sizeof(*
_tmpB8A)),((_tmpB8A[0]=((_tmpB8D.tag=4,((_tmpB8D.f1=((_tmpB8B.field_info=Cyc_Absyn_UnknownDatatypefield(((
_tmpB8C.datatype_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmpB8C.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB8C.is_extensible=
_tmp5C7,_tmpB8C))))))),((_tmpB8B.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB8B)))),_tmpB8D)))),
_tmpB8A)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 147: _LL2C0: yyval=Cyc_YY31(0);break;case
148: _LL2C1: yyval=Cyc_YY31(1);break;case 149: _LL2C2:{struct Cyc_List_List*_tmpB8E;
yyval=Cyc_YY34(((_tmpB8E=_cycalloc(sizeof(*_tmpB8E)),((_tmpB8E->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB8E->tl=0,
_tmpB8E)))))));}break;case 150: _LL2C3:{struct Cyc_List_List*_tmpB8F;yyval=Cyc_YY34(((
_tmpB8F=_cycalloc(sizeof(*_tmpB8F)),((_tmpB8F->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB8F->tl=0,_tmpB8F)))))));}
break;case 151: _LL2C4:{struct Cyc_List_List*_tmpB90;yyval=Cyc_YY34(((_tmpB90=
_cycalloc(sizeof(*_tmpB90)),((_tmpB90->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB90->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB90)))))));}break;
case 152: _LL2C5:{struct Cyc_List_List*_tmpB91;yyval=Cyc_YY34(((_tmpB91=_cycalloc(
sizeof(*_tmpB91)),((_tmpB91->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB91->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB91)))))));}break;
case 153: _LL2C6: yyval=Cyc_YY32(Cyc_Absyn_Public);break;case 154: _LL2C7: yyval=Cyc_YY32(
Cyc_Absyn_Extern);break;case 155: _LL2C8: yyval=Cyc_YY32(Cyc_Absyn_Static);break;
case 156: _LL2C9:{struct Cyc_Absyn_Datatypefield*_tmpB92;yyval=Cyc_YY33(((_tmpB92=
_cycalloc(sizeof(*_tmpB92)),((_tmpB92->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB92->typs=0,((_tmpB92->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpB92->sc=Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB92)))))))))));}
break;case 157: _LL2CA: {struct Cyc_List_List*_tmp5D1=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Absyn_Datatypefield*
_tmpB93;yyval=Cyc_YY33(((_tmpB93=_cycalloc(sizeof(*_tmpB93)),((_tmpB93->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB93->typs=
_tmp5D1,((_tmpB93->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpB93->sc=
Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmpB93)))))))))));}break;}case 158: _LL2CB: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 159: _LL2CC:{struct
Cyc_Parse_Declarator*_tmpB94;yyval=Cyc_YY29(((_tmpB94=_cycalloc(sizeof(*_tmpB94)),((
_tmpB94->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id,((_tmpB94->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpB94)))))));}
break;case 160: _LL2CD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 161: _LL2CE:{struct Cyc_Parse_Declarator*_tmpB95;yyval=
Cyc_YY29(((_tmpB95=_cycalloc(sizeof(*_tmpB95)),((_tmpB95->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->id,((_tmpB95->tms=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpB95)))))));}
break;case 162: _LL2CF:{struct Cyc_Parse_Declarator*_tmpB96;yyval=Cyc_YY29(((
_tmpB96=_cycalloc(sizeof(*_tmpB96)),((_tmpB96->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB96->tms=0,_tmpB96)))))));}
break;case 163: _LL2D0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 164: _LL2D1: {struct Cyc_Parse_Declarator*_tmp5D6=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpB9C;struct Cyc_Absyn_Attributes_mod_struct _tmpB9B;struct Cyc_List_List*_tmpB9A;
_tmp5D6->tms=((_tmpB9A=_cycalloc(sizeof(*_tmpB9A)),((_tmpB9A->hd=(void*)((
_tmpB9C=_cycalloc(sizeof(*_tmpB9C)),((_tmpB9C[0]=((_tmpB9B.tag=5,((_tmpB9B.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpB9B.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB9B)))))),_tmpB9C)))),((
_tmpB9A->tl=_tmp5D6->tms,_tmpB9A))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 165: _LL2D2:{
struct Cyc_List_List*_tmpBA6;struct Cyc_Absyn_Carray_mod_struct _tmpBA5;struct Cyc_Absyn_Carray_mod_struct*
_tmpBA4;struct Cyc_Parse_Declarator*_tmpBA3;yyval=Cyc_YY29(((_tmpBA3=_cycalloc(
sizeof(*_tmpBA3)),((_tmpBA3->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBA3->tms=((
_tmpBA6=_cycalloc(sizeof(*_tmpBA6)),((_tmpBA6->hd=(void*)((_tmpBA4=_cycalloc(
sizeof(*_tmpBA4)),((_tmpBA4[0]=((_tmpBA5.tag=0,((_tmpBA5.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBA5.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBA5)))))),
_tmpBA4)))),((_tmpBA6->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpBA6)))))),_tmpBA3)))))));}break;case 166:
_LL2D3:{struct Cyc_List_List*_tmpBB0;struct Cyc_Absyn_ConstArray_mod_struct _tmpBAF;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpBAE;struct Cyc_Parse_Declarator*_tmpBAD;
yyval=Cyc_YY29(((_tmpBAD=_cycalloc(sizeof(*_tmpBAD)),((_tmpBAD->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpBAD->tms=((
_tmpBB0=_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0->hd=(void*)((_tmpBAE=_cycalloc(
sizeof(*_tmpBAE)),((_tmpBAE[0]=((_tmpBAF.tag=1,((_tmpBAF.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBAF.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBAF.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBAF)))))))),
_tmpBAE)))),((_tmpBB0->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpBB0)))))),_tmpBAD)))))));}break;case 167:
_LL2D4: {struct _tuple23 _tmp5E3;struct Cyc_List_List*_tmp5E4;int _tmp5E5;struct Cyc_Absyn_VarargInfo*
_tmp5E6;struct Cyc_Core_Opt*_tmp5E7;struct Cyc_List_List*_tmp5E8;struct _tuple23*
_tmp5E2=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5E3=*_tmp5E2;_tmp5E4=_tmp5E3.f1;_tmp5E5=_tmp5E3.f2;_tmp5E6=_tmp5E3.f3;
_tmp5E7=_tmp5E3.f4;_tmp5E8=_tmp5E3.f5;{struct Cyc_List_List*_tmpBC5;struct Cyc_Absyn_Function_mod_struct
_tmpBC4;struct Cyc_Absyn_WithTypes_struct _tmpBC3;struct Cyc_Absyn_WithTypes_struct*
_tmpBC2;struct Cyc_Absyn_Function_mod_struct*_tmpBC1;struct Cyc_Parse_Declarator*
_tmpBC0;yyval=Cyc_YY29(((_tmpBC0=_cycalloc(sizeof(*_tmpBC0)),((_tmpBC0->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBC0->tms=((
_tmpBC5=_cycalloc(sizeof(*_tmpBC5)),((_tmpBC5->hd=(void*)((_tmpBC1=_cycalloc(
sizeof(*_tmpBC1)),((_tmpBC1[0]=((_tmpBC4.tag=3,((_tmpBC4.f1=(void*)((void*)((
_tmpBC2=_cycalloc(sizeof(*_tmpBC2)),((_tmpBC2[0]=((_tmpBC3.tag=1,((_tmpBC3.f1=
_tmp5E4,((_tmpBC3.f2=_tmp5E5,((_tmpBC3.f3=_tmp5E6,((_tmpBC3.f4=_tmp5E7,((_tmpBC3.f5=
_tmp5E8,_tmpBC3)))))))))))),_tmpBC2))))),_tmpBC4)))),_tmpBC1)))),((_tmpBC5->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpBC5)))))),_tmpBC0)))))));}break;}case 168: _LL2D5:{struct Cyc_List_List*_tmpBDA;
struct Cyc_Absyn_Function_mod_struct _tmpBD9;struct Cyc_Absyn_WithTypes_struct
_tmpBD8;struct Cyc_Absyn_WithTypes_struct*_tmpBD7;struct Cyc_Absyn_Function_mod_struct*
_tmpBD6;struct Cyc_Parse_Declarator*_tmpBD5;yyval=Cyc_YY29(((_tmpBD5=_cycalloc(
sizeof(*_tmpBD5)),((_tmpBD5->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpBD5->tms=((
_tmpBDA=_cycalloc(sizeof(*_tmpBDA)),((_tmpBDA->hd=(void*)((_tmpBD6=_cycalloc(
sizeof(*_tmpBD6)),((_tmpBD6[0]=((_tmpBD9.tag=3,((_tmpBD9.f1=(void*)((void*)((
_tmpBD7=_cycalloc(sizeof(*_tmpBD7)),((_tmpBD7[0]=((_tmpBD8.tag=1,((_tmpBD8.f1=0,((
_tmpBD8.f2=0,((_tmpBD8.f3=0,((_tmpBD8.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBD8.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBD8)))))))))))),
_tmpBD7))))),_tmpBD9)))),_tmpBD6)))),((_tmpBDA->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpBDA)))))),
_tmpBD5)))))));}break;case 169: _LL2D6:{struct Cyc_List_List*_tmpBEF;struct Cyc_Absyn_Function_mod_struct
_tmpBEE;struct Cyc_Absyn_NoTypes_struct _tmpBED;struct Cyc_Absyn_NoTypes_struct*
_tmpBEC;struct Cyc_Absyn_Function_mod_struct*_tmpBEB;struct Cyc_Parse_Declarator*
_tmpBEA;yyval=Cyc_YY29(((_tmpBEA=_cycalloc(sizeof(*_tmpBEA)),((_tmpBEA->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBEA->tms=((
_tmpBEF=_cycalloc(sizeof(*_tmpBEF)),((_tmpBEF->hd=(void*)((_tmpBEB=_cycalloc(
sizeof(*_tmpBEB)),((_tmpBEB[0]=((_tmpBEE.tag=3,((_tmpBEE.f1=(void*)((void*)((
_tmpBEC=_cycalloc(sizeof(*_tmpBEC)),((_tmpBEC[0]=((_tmpBED.tag=0,((_tmpBED.f1=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpBED.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpBED)))))),_tmpBEC))))),_tmpBEE)))),_tmpBEB)))),((
_tmpBEF->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms,_tmpBEF)))))),_tmpBEA)))))));}break;case 170: _LL2D7: {
struct Cyc_List_List*_tmp5FB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpBF9;struct Cyc_Absyn_TypeParams_mod_struct _tmpBF8;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpBF7;struct Cyc_Parse_Declarator*_tmpBF6;yyval=Cyc_YY29(((_tmpBF6=_cycalloc(
sizeof(*_tmpBF6)),((_tmpBF6->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBF6->tms=((
_tmpBF9=_cycalloc(sizeof(*_tmpBF9)),((_tmpBF9->hd=(void*)((_tmpBF7=_cycalloc(
sizeof(*_tmpBF7)),((_tmpBF7[0]=((_tmpBF8.tag=4,((_tmpBF8.f1=_tmp5FB,((_tmpBF8.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpBF8.f3=0,_tmpBF8)))))))),_tmpBF7)))),((_tmpBF9->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpBF9)))))),_tmpBF6)))))));}break;}case 171: _LL2D8:{struct Cyc_List_List*_tmpC03;
struct Cyc_Absyn_Attributes_mod_struct _tmpC02;struct Cyc_Absyn_Attributes_mod_struct*
_tmpC01;struct Cyc_Parse_Declarator*_tmpC00;yyval=Cyc_YY29(((_tmpC00=_cycalloc(
sizeof(*_tmpC00)),((_tmpC00->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpC00->tms=((
_tmpC03=_cycalloc(sizeof(*_tmpC03)),((_tmpC03->hd=(void*)((_tmpC01=_cycalloc(
sizeof(*_tmpC01)),((_tmpC01[0]=((_tmpC02.tag=5,((_tmpC02.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC02.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC02)))))),_tmpC01)))),((
_tmpC03->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpC03)))))),_tmpC00)))))));}break;case 172: _LL2D9:{
struct Cyc_Parse_Declarator*_tmpC04;yyval=Cyc_YY29(((_tmpC04=_cycalloc(sizeof(*
_tmpC04)),((_tmpC04->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC04->tms=0,_tmpC04)))))));}break;case 173: _LL2DA:{
struct Cyc_Parse_Declarator*_tmpC05;yyval=Cyc_YY29(((_tmpC05=_cycalloc(sizeof(*
_tmpC05)),((_tmpC05->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC05->tms=0,_tmpC05)))))));}break;case 174: _LL2DB:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 175:
_LL2DC: {struct Cyc_Parse_Declarator*_tmp606=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpC0B;struct Cyc_Absyn_Attributes_mod_struct _tmpC0A;struct Cyc_List_List*_tmpC09;
_tmp606->tms=((_tmpC09=_cycalloc(sizeof(*_tmpC09)),((_tmpC09->hd=(void*)((
_tmpC0B=_cycalloc(sizeof(*_tmpC0B)),((_tmpC0B[0]=((_tmpC0A.tag=5,((_tmpC0A.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpC0A.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC0A)))))),_tmpC0B)))),((
_tmpC09->tl=_tmp606->tms,_tmpC09))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 176: _LL2DD:{
struct Cyc_List_List*_tmpC15;struct Cyc_Absyn_Carray_mod_struct _tmpC14;struct Cyc_Absyn_Carray_mod_struct*
_tmpC13;struct Cyc_Parse_Declarator*_tmpC12;yyval=Cyc_YY29(((_tmpC12=_cycalloc(
sizeof(*_tmpC12)),((_tmpC12->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC12->tms=((
_tmpC15=_cycalloc(sizeof(*_tmpC15)),((_tmpC15->hd=(void*)((_tmpC13=_cycalloc(
sizeof(*_tmpC13)),((_tmpC13[0]=((_tmpC14.tag=0,((_tmpC14.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC14.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC14)))))),
_tmpC13)))),((_tmpC15->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpC15)))))),_tmpC12)))))));}break;case 177:
_LL2DE:{struct Cyc_List_List*_tmpC1F;struct Cyc_Absyn_ConstArray_mod_struct _tmpC1E;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpC1D;struct Cyc_Parse_Declarator*_tmpC1C;
yyval=Cyc_YY29(((_tmpC1C=_cycalloc(sizeof(*_tmpC1C)),((_tmpC1C->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpC1C->tms=((
_tmpC1F=_cycalloc(sizeof(*_tmpC1F)),((_tmpC1F->hd=(void*)((_tmpC1D=_cycalloc(
sizeof(*_tmpC1D)),((_tmpC1D[0]=((_tmpC1E.tag=1,((_tmpC1E.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC1E.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC1E.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC1E)))))))),
_tmpC1D)))),((_tmpC1F->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpC1F)))))),_tmpC1C)))))));}break;case 178:
_LL2DF: {struct _tuple23 _tmp613;struct Cyc_List_List*_tmp614;int _tmp615;struct Cyc_Absyn_VarargInfo*
_tmp616;struct Cyc_Core_Opt*_tmp617;struct Cyc_List_List*_tmp618;struct _tuple23*
_tmp612=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp613=*_tmp612;_tmp614=_tmp613.f1;_tmp615=_tmp613.f2;_tmp616=_tmp613.f3;
_tmp617=_tmp613.f4;_tmp618=_tmp613.f5;{struct Cyc_List_List*_tmpC34;struct Cyc_Absyn_Function_mod_struct
_tmpC33;struct Cyc_Absyn_WithTypes_struct _tmpC32;struct Cyc_Absyn_WithTypes_struct*
_tmpC31;struct Cyc_Absyn_Function_mod_struct*_tmpC30;struct Cyc_Parse_Declarator*
_tmpC2F;yyval=Cyc_YY29(((_tmpC2F=_cycalloc(sizeof(*_tmpC2F)),((_tmpC2F->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC2F->tms=((
_tmpC34=_cycalloc(sizeof(*_tmpC34)),((_tmpC34->hd=(void*)((_tmpC30=_cycalloc(
sizeof(*_tmpC30)),((_tmpC30[0]=((_tmpC33.tag=3,((_tmpC33.f1=(void*)((void*)((
_tmpC31=_cycalloc(sizeof(*_tmpC31)),((_tmpC31[0]=((_tmpC32.tag=1,((_tmpC32.f1=
_tmp614,((_tmpC32.f2=_tmp615,((_tmpC32.f3=_tmp616,((_tmpC32.f4=_tmp617,((_tmpC32.f5=
_tmp618,_tmpC32)))))))))))),_tmpC31))))),_tmpC33)))),_tmpC30)))),((_tmpC34->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC34)))))),_tmpC2F)))))));}break;}case 179: _LL2E0:{struct Cyc_List_List*_tmpC49;
struct Cyc_Absyn_Function_mod_struct _tmpC48;struct Cyc_Absyn_WithTypes_struct
_tmpC47;struct Cyc_Absyn_WithTypes_struct*_tmpC46;struct Cyc_Absyn_Function_mod_struct*
_tmpC45;struct Cyc_Parse_Declarator*_tmpC44;yyval=Cyc_YY29(((_tmpC44=_cycalloc(
sizeof(*_tmpC44)),((_tmpC44->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpC44->tms=((
_tmpC49=_cycalloc(sizeof(*_tmpC49)),((_tmpC49->hd=(void*)((_tmpC45=_cycalloc(
sizeof(*_tmpC45)),((_tmpC45[0]=((_tmpC48.tag=3,((_tmpC48.f1=(void*)((void*)((
_tmpC46=_cycalloc(sizeof(*_tmpC46)),((_tmpC46[0]=((_tmpC47.tag=1,((_tmpC47.f1=0,((
_tmpC47.f2=0,((_tmpC47.f3=0,((_tmpC47.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC47.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC47)))))))))))),
_tmpC46))))),_tmpC48)))),_tmpC45)))),((_tmpC49->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpC49)))))),
_tmpC44)))))));}break;case 180: _LL2E1:{struct Cyc_List_List*_tmpC5E;struct Cyc_Absyn_Function_mod_struct
_tmpC5D;struct Cyc_Absyn_NoTypes_struct _tmpC5C;struct Cyc_Absyn_NoTypes_struct*
_tmpC5B;struct Cyc_Absyn_Function_mod_struct*_tmpC5A;struct Cyc_Parse_Declarator*
_tmpC59;yyval=Cyc_YY29(((_tmpC59=_cycalloc(sizeof(*_tmpC59)),((_tmpC59->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC59->tms=((
_tmpC5E=_cycalloc(sizeof(*_tmpC5E)),((_tmpC5E->hd=(void*)((_tmpC5A=_cycalloc(
sizeof(*_tmpC5A)),((_tmpC5A[0]=((_tmpC5D.tag=3,((_tmpC5D.f1=(void*)((void*)((
_tmpC5B=_cycalloc(sizeof(*_tmpC5B)),((_tmpC5B[0]=((_tmpC5C.tag=0,((_tmpC5C.f1=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpC5C.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpC5C)))))),_tmpC5B))))),_tmpC5D)))),_tmpC5A)))),((
_tmpC5E->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms,_tmpC5E)))))),_tmpC59)))))));}break;case 181: _LL2E2: {
struct Cyc_List_List*_tmp62B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpC68;struct Cyc_Absyn_TypeParams_mod_struct _tmpC67;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpC66;struct Cyc_Parse_Declarator*_tmpC65;yyval=Cyc_YY29(((_tmpC65=_cycalloc(
sizeof(*_tmpC65)),((_tmpC65->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC65->tms=((
_tmpC68=_cycalloc(sizeof(*_tmpC68)),((_tmpC68->hd=(void*)((_tmpC66=_cycalloc(
sizeof(*_tmpC66)),((_tmpC66[0]=((_tmpC67.tag=4,((_tmpC67.f1=_tmp62B,((_tmpC67.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpC67.f3=0,_tmpC67)))))))),_tmpC66)))),((_tmpC68->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC68)))))),_tmpC65)))))));}break;}case 182: _LL2E3:{struct Cyc_List_List*_tmpC72;
struct Cyc_Absyn_Attributes_mod_struct _tmpC71;struct Cyc_Absyn_Attributes_mod_struct*
_tmpC70;struct Cyc_Parse_Declarator*_tmpC6F;yyval=Cyc_YY29(((_tmpC6F=_cycalloc(
sizeof(*_tmpC6F)),((_tmpC6F->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpC6F->tms=((
_tmpC72=_cycalloc(sizeof(*_tmpC72)),((_tmpC72->hd=(void*)((_tmpC70=_cycalloc(
sizeof(*_tmpC70)),((_tmpC70[0]=((_tmpC71.tag=5,((_tmpC71.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC71.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC71)))))),_tmpC70)))),((
_tmpC72->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpC72)))))),_tmpC6F)))))));}break;case 183: _LL2E4:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 184:
_LL2E5: yyval=Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 185: _LL2E6: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){struct Cyc_Absyn_Attributes_mod_struct*
_tmpC78;struct Cyc_Absyn_Attributes_mod_struct _tmpC77;struct Cyc_List_List*_tmpC76;
ans=((_tmpC76=_cycalloc(sizeof(*_tmpC76)),((_tmpC76->hd=(void*)((_tmpC78=
_cycalloc(sizeof(*_tmpC78)),((_tmpC78[0]=((_tmpC77.tag=5,((_tmpC77.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpC77.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpC77)))))),_tmpC78)))),((_tmpC76->tl=ans,_tmpC76))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;struct Cyc_Position_Segment*_tmp638;union Cyc_Absyn_Constraint*_tmp639;
union Cyc_Absyn_Constraint*_tmp63A;struct _tuple18 _tmp637=*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp638=_tmp637.f1;
_tmp639=_tmp637.f2;_tmp63A=_tmp637.f3;if(Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*
_tmpC79;ptrloc=((_tmpC79=_cycalloc(sizeof(*_tmpC79)),((_tmpC79->ptr_loc=_tmp638,((
_tmpC79->rgn_loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpC79->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),_tmpC79))))))));}{union Cyc_Absyn_Constraint*
_tmp63D;union Cyc_Absyn_Constraint*_tmp63E;union Cyc_Absyn_Constraint*_tmp63F;void*
_tmp640;struct _tuple11 _tmp63C=Cyc_Parse_collapse_pointer_quals(_tmp638,_tmp639,
_tmp63A,Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY54(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
_tmp63D=_tmp63C.f1;_tmp63E=_tmp63C.f2;_tmp63F=_tmp63C.f3;_tmp640=_tmp63C.f4;{
struct Cyc_Absyn_Pointer_mod_struct*_tmpC83;struct Cyc_Absyn_PtrAtts _tmpC82;struct
Cyc_Absyn_Pointer_mod_struct _tmpC81;struct Cyc_List_List*_tmpC80;ans=((_tmpC80=
_cycalloc(sizeof(*_tmpC80)),((_tmpC80->hd=(void*)((_tmpC83=_cycalloc(sizeof(*
_tmpC83)),((_tmpC83[0]=((_tmpC81.tag=2,((_tmpC81.f1=((_tmpC82.rgn=_tmp640,((
_tmpC82.nullable=_tmp63D,((_tmpC82.bounds=_tmp63E,((_tmpC82.zero_term=_tmp63F,((
_tmpC82.ptrloc=ptrloc,_tmpC82)))))))))),((_tmpC81.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC81)))))),_tmpC83)))),((
_tmpC80->tl=ans,_tmpC80))))));}yyval=Cyc_YY28(ans);break;};};}case 186: _LL2E7:
yyval=Cyc_YY54(0);break;case 187: _LL2E8:{struct Cyc_List_List*_tmpC84;yyval=Cyc_YY54(((
_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84->hd=Cyc_yyget_YY53(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC84->tl=Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC84)))))));}break;
case 188: _LL2E9:{struct Cyc_Absyn_Numelts_ptrqual_struct _tmpC87;struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmpC86;yyval=Cyc_YY53((void*)((_tmpC86=_cycalloc(sizeof(*_tmpC86)),((_tmpC86[0]=((
_tmpC87.tag=0,((_tmpC87.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpC87)))),_tmpC86)))));}break;case 189: _LL2EA:{struct
Cyc_Absyn_Region_ptrqual_struct _tmpC8A;struct Cyc_Absyn_Region_ptrqual_struct*
_tmpC89;yyval=Cyc_YY53((void*)((_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89[0]=((
_tmpC8A.tag=1,((_tmpC8A.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC8A)))),_tmpC89)))));}
break;case 190: _LL2EB:{struct Cyc_Absyn_Thin_ptrqual_struct _tmpC8D;struct Cyc_Absyn_Thin_ptrqual_struct*
_tmpC8C;yyval=Cyc_YY53((void*)((_tmpC8C=_cycalloc_atomic(sizeof(*_tmpC8C)),((
_tmpC8C[0]=((_tmpC8D.tag=2,_tmpC8D)),_tmpC8C)))));}break;case 191: _LL2EC:{struct
Cyc_Absyn_Fat_ptrqual_struct _tmpC90;struct Cyc_Absyn_Fat_ptrqual_struct*_tmpC8F;
yyval=Cyc_YY53((void*)((_tmpC8F=_cycalloc_atomic(sizeof(*_tmpC8F)),((_tmpC8F[0]=((
_tmpC90.tag=3,_tmpC90)),_tmpC8F)))));}break;case 192: _LL2ED:{struct Cyc_Absyn_Zeroterm_ptrqual_struct
_tmpC93;struct Cyc_Absyn_Zeroterm_ptrqual_struct*_tmpC92;yyval=Cyc_YY53((void*)((
_tmpC92=_cycalloc_atomic(sizeof(*_tmpC92)),((_tmpC92[0]=((_tmpC93.tag=4,_tmpC93)),
_tmpC92)))));}break;case 193: _LL2EE:{struct Cyc_Absyn_Nozeroterm_ptrqual_struct
_tmpC96;struct Cyc_Absyn_Nozeroterm_ptrqual_struct*_tmpC95;yyval=Cyc_YY53((void*)((
_tmpC95=_cycalloc_atomic(sizeof(*_tmpC95)),((_tmpC95[0]=((_tmpC96.tag=5,_tmpC96)),
_tmpC95)))));}break;case 194: _LL2EF:{struct Cyc_Absyn_Notnull_ptrqual_struct
_tmpC99;struct Cyc_Absyn_Notnull_ptrqual_struct*_tmpC98;yyval=Cyc_YY53((void*)((
_tmpC98=_cycalloc_atomic(sizeof(*_tmpC98)),((_tmpC98[0]=((_tmpC99.tag=6,_tmpC99)),
_tmpC98)))));}break;case 195: _LL2F0:{struct Cyc_Absyn_Nullable_ptrqual_struct
_tmpC9C;struct Cyc_Absyn_Nullable_ptrqual_struct*_tmpC9B;yyval=Cyc_YY53((void*)((
_tmpC9B=_cycalloc_atomic(sizeof(*_tmpC9B)),((_tmpC9B[0]=((_tmpC9C.tag=7,_tmpC9C)),
_tmpC9B)))));}break;case 196: _LL2F1: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple18*_tmpC9D;yyval=Cyc_YY1(((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->f1=
loc,((_tmpC9D->f2=Cyc_Absyn_true_conref,((_tmpC9D->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC9D)))))))));}break;}
case 197: _LL2F2: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple18*_tmpC9E;yyval=Cyc_YY1(((_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E->f1=
loc,((_tmpC9E->f2=Cyc_Absyn_false_conref,((_tmpC9E->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC9E)))))))));}break;}
case 198: _LL2F3: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{struct _tuple18*
_tmpC9F;yyval=Cyc_YY1(((_tmpC9F=_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F->f1=loc,((
_tmpC9F->f2=Cyc_Absyn_true_conref,((_tmpC9F->f3=Cyc_Absyn_bounds_dyneither_conref,
_tmpC9F)))))))));}break;}case 199: _LL2F4: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 200: _LL2F5:{struct Cyc_Absyn_Upper_b_struct _tmpCA2;struct Cyc_Absyn_Upper_b_struct*
_tmpCA1;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmpCA1=_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1[0]=((_tmpCA2.tag=1,((
_tmpCA2.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpCA2)))),_tmpCA1))))));}break;case 201: _LL2F6: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)());break;case 202: _LL2F7: yyval=Cyc_YY51(Cyc_Absyn_true_conref);
break;case 203: _LL2F8: yyval=Cyc_YY51(Cyc_Absyn_false_conref);break;case 204: _LL2F9:
yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));
break;case 205: _LL2FA: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_trk,1);yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 206: _LL2FB:
yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));
break;case 207: _LL2FC: yyval=Cyc_YY25(Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)));break;case
208: _LL2FD: yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 209: _LL2FE:{
struct _tuple23*_tmpCA3;yyval=Cyc_YY39(((_tmpCA3=_cycalloc(sizeof(*_tmpCA3)),((
_tmpCA3->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((
_tmpCA3->f2=0,((_tmpCA3->f3=0,((_tmpCA3->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCA3->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCA3)))))))))))));}
break;case 210: _LL2FF:{struct _tuple23*_tmpCA4;yyval=Cyc_YY39(((_tmpCA4=_cycalloc(
sizeof(*_tmpCA4)),((_tmpCA4->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),((_tmpCA4->f2=1,((_tmpCA4->f3=0,((_tmpCA4->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCA4->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCA4)))))))))))));}
break;case 211: _LL300: {struct _tuple8 _tmp65E;struct Cyc_Core_Opt*_tmp65F;struct Cyc_Absyn_Tqual
_tmp660;void*_tmp661;struct _tuple8*_tmp65D=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp65E=*_tmp65D;_tmp65F=
_tmp65E.f1;_tmp660=_tmp65E.f2;_tmp661=_tmp65E.f3;{struct Cyc_Absyn_VarargInfo*
_tmpCA5;struct Cyc_Absyn_VarargInfo*_tmp662=(_tmpCA5=_cycalloc(sizeof(*_tmpCA5)),((
_tmpCA5->name=_tmp65F,((_tmpCA5->tq=_tmp660,((_tmpCA5->type=_tmp661,((_tmpCA5->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpCA5)))))))));{struct _tuple23*_tmpCA6;yyval=Cyc_YY39(((_tmpCA6=_cycalloc(
sizeof(*_tmpCA6)),((_tmpCA6->f1=0,((_tmpCA6->f2=0,((_tmpCA6->f3=_tmp662,((
_tmpCA6->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpCA6->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCA6)))))))))))));}
break;};}case 212: _LL301: {struct _tuple8 _tmp666;struct Cyc_Core_Opt*_tmp667;struct
Cyc_Absyn_Tqual _tmp668;void*_tmp669;struct _tuple8*_tmp665=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp666=*_tmp665;_tmp667=
_tmp666.f1;_tmp668=_tmp666.f2;_tmp669=_tmp666.f3;{struct Cyc_Absyn_VarargInfo*
_tmpCA7;struct Cyc_Absyn_VarargInfo*_tmp66A=(_tmpCA7=_cycalloc(sizeof(*_tmpCA7)),((
_tmpCA7->name=_tmp667,((_tmpCA7->tq=_tmp668,((_tmpCA7->type=_tmp669,((_tmpCA7->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpCA7)))))))));{struct _tuple23*_tmpCA8;yyval=Cyc_YY39(((_tmpCA8=_cycalloc(
sizeof(*_tmpCA8)),((_tmpCA8->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)])),((_tmpCA8->f2=0,((_tmpCA8->f3=_tmp66A,((_tmpCA8->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCA8->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCA8)))))))))))));}
break;};}case 213: _LL302:{struct Cyc_Absyn_Unknown_kb_struct _tmpCAB;struct Cyc_Absyn_Unknown_kb_struct*
_tmpCAA;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)((_tmpCAA=_cycalloc(
sizeof(*_tmpCAA)),((_tmpCAA[0]=((_tmpCAB.tag=1,((_tmpCAB.f1=0,_tmpCAB)))),
_tmpCAA))))));}break;case 214: _LL303: yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))));
break;case 215: _LL304: yyval=Cyc_YY49(0);break;case 216: _LL305:{struct Cyc_Absyn_JoinEff_struct*
_tmpCB1;struct Cyc_Absyn_JoinEff_struct _tmpCB0;struct Cyc_Core_Opt*_tmpCAF;yyval=
Cyc_YY49(((_tmpCAF=_cycalloc(sizeof(*_tmpCAF)),((_tmpCAF->v=(void*)((_tmpCB1=
_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1[0]=((_tmpCB0.tag=24,((_tmpCB0.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCB0)))),_tmpCB1)))),
_tmpCAF)))));}break;case 217: _LL306: yyval=Cyc_YY50(0);break;case 218: _LL307: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 219: _LL308: {
struct Cyc_Absyn_Less_kb_struct _tmpCB4;struct Cyc_Absyn_Less_kb_struct*_tmpCB3;
struct Cyc_Absyn_Less_kb_struct*_tmp672=(_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((
_tmpCB3[0]=((_tmpCB4.tag=2,((_tmpCB4.f1=0,((_tmpCB4.f2=& Cyc_Tcutil_trk,_tmpCB4)))))),
_tmpCB3)));struct _dyneither_ptr _tmp673=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp674=Cyc_Parse_id2type(
_tmp673,(void*)_tmp672);{struct _tuple26*_tmpCBE;struct Cyc_Absyn_JoinEff_struct
_tmpCBD;struct Cyc_Absyn_JoinEff_struct*_tmpCBC;struct Cyc_List_List*_tmpCBB;yyval=
Cyc_YY50(((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB->hd=((_tmpCBE=_cycalloc(
sizeof(*_tmpCBE)),((_tmpCBE->f1=(void*)((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((
_tmpCBC[0]=((_tmpCBD.tag=24,((_tmpCBD.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCBD)))),_tmpCBC)))),((
_tmpCBE->f2=_tmp674,_tmpCBE)))))),((_tmpCBB->tl=0,_tmpCBB)))))));}break;}case 220:
_LL309: {struct Cyc_Absyn_Less_kb_struct _tmpCC1;struct Cyc_Absyn_Less_kb_struct*
_tmpCC0;struct Cyc_Absyn_Less_kb_struct*_tmp67B=(_tmpCC0=_cycalloc(sizeof(*
_tmpCC0)),((_tmpCC0[0]=((_tmpCC1.tag=2,((_tmpCC1.f1=0,((_tmpCC1.f2=& Cyc_Tcutil_trk,
_tmpCC1)))))),_tmpCC0)));struct _dyneither_ptr _tmp67C=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp67D=Cyc_Parse_id2type(
_tmp67C,(void*)_tmp67B);{struct _tuple26*_tmpCCB;struct Cyc_Absyn_JoinEff_struct
_tmpCCA;struct Cyc_Absyn_JoinEff_struct*_tmpCC9;struct Cyc_List_List*_tmpCC8;yyval=
Cyc_YY50(((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8->hd=((_tmpCCB=_cycalloc(
sizeof(*_tmpCCB)),((_tmpCCB->f1=(void*)((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((
_tmpCC9[0]=((_tmpCCA.tag=24,((_tmpCCA.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmpCCA)))),_tmpCC9)))),((
_tmpCCB->f2=_tmp67D,_tmpCCB)))))),((_tmpCC8->tl=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCC8)))))));}break;}case
221: _LL30A: yyval=Cyc_YY31(0);break;case 222: _LL30B:{const char*_tmpCCC;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpCCC="inject",_tag_dyneither(_tmpCCC,sizeof(char),7))))
!= 0){const char*_tmpCCD;Cyc_Parse_err(((_tmpCCD="missing type in function declaration",
_tag_dyneither(_tmpCCD,sizeof(char),37))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY31(
1);break;case 223: _LL30C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 224: _LL30D: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 225: _LL30E:
yyval=Cyc_YY40(0);break;case 226: _LL30F: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 227: _LL310:{struct Cyc_Absyn_RgnsEff_struct*
_tmpCD3;struct Cyc_Absyn_RgnsEff_struct _tmpCD2;struct Cyc_List_List*_tmpCD1;yyval=
Cyc_YY40(((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->hd=(void*)((_tmpCD3=
_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3[0]=((_tmpCD2.tag=25,((_tmpCD2.f1=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpCD2)))),_tmpCD3)))),((_tmpCD1->tl=0,_tmpCD1)))))));}break;case 228: _LL311: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_ek,
0);{struct Cyc_List_List*_tmpCD4;yyval=Cyc_YY40(((_tmpCD4=_cycalloc(sizeof(*
_tmpCD4)),((_tmpCD4->hd=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpCD4->tl=0,_tmpCD4)))))));}break;case 229: _LL312: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_trk,
1);{struct Cyc_Absyn_AccessEff_struct*_tmpCDA;struct Cyc_Absyn_AccessEff_struct
_tmpCD9;struct Cyc_List_List*_tmpCD8;yyval=Cyc_YY40(((_tmpCD8=_cycalloc(sizeof(*
_tmpCD8)),((_tmpCD8->hd=(void*)((_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA[0]=((
_tmpCD9.tag=23,((_tmpCD9.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCD9)))),_tmpCDA)))),((
_tmpCD8->tl=0,_tmpCD8)))))));}break;case 230: _LL313: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),& Cyc_Tcutil_trk,1);{
struct Cyc_Absyn_AccessEff_struct*_tmpCE0;struct Cyc_Absyn_AccessEff_struct _tmpCDF;
struct Cyc_List_List*_tmpCDE;yyval=Cyc_YY40(((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((
_tmpCDE->hd=(void*)((_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0[0]=((_tmpCDF.tag=
23,((_tmpCDF.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),_tmpCDF)))),_tmpCE0)))),((_tmpCDE->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCDE)))))));}break;case
231: _LL314:{struct Cyc_List_List*_tmpCE1;yyval=Cyc_YY38(((_tmpCE1=_cycalloc(
sizeof(*_tmpCE1)),((_tmpCE1->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCE1->tl=0,_tmpCE1)))))));}
break;case 232: _LL315:{struct Cyc_List_List*_tmpCE2;yyval=Cyc_YY38(((_tmpCE2=
_cycalloc(sizeof(*_tmpCE2)),((_tmpCE2->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCE2->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCE2)))))));}
break;case 233: _LL316: {struct _tuple22 _tmp693;struct Cyc_Absyn_Tqual _tmp694;struct
Cyc_List_List*_tmp695;struct Cyc_List_List*_tmp696;struct _tuple22*_tmp692=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp693=*_tmp692;
_tmp694=_tmp693.f1;_tmp695=_tmp693.f2;_tmp696=_tmp693.f3;if(_tmp694.loc == 0)
_tmp694.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp698;struct _tuple1*
_tmp699;struct Cyc_List_List*_tmp69A;struct Cyc_Parse_Declarator*_tmp697=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp698=*_tmp697;
_tmp699=_tmp698.id;_tmp69A=_tmp698.tms;{void*_tmp69B=Cyc_Parse_speclist2typ(
_tmp695,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp69D;void*_tmp69E;struct
Cyc_List_List*_tmp69F;struct Cyc_List_List*_tmp6A0;struct _tuple10 _tmp69C=Cyc_Parse_apply_tms(
_tmp694,_tmp69B,_tmp696,_tmp69A);_tmp69D=_tmp69C.f1;_tmp69E=_tmp69C.f2;_tmp69F=
_tmp69C.f3;_tmp6A0=_tmp69C.f4;if(_tmp69F != 0){const char*_tmpCE3;Cyc_Parse_err(((
_tmpCE3="parameter with bad type params",_tag_dyneither(_tmpCE3,sizeof(char),31))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp699)){const char*
_tmpCE4;Cyc_Parse_err(((_tmpCE4="parameter cannot be qualified with a namespace",
_tag_dyneither(_tmpCE4,sizeof(char),47))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*
_tmpCE5;struct Cyc_Core_Opt*_tmp6A3=(struct Cyc_Core_Opt*)((_tmpCE5=_cycalloc(
sizeof(*_tmpCE5)),((_tmpCE5->v=(*_tmp699).f2,_tmpCE5))));if(_tmp6A0 != 0){const
char*_tmpCE8;void*_tmpCE7;(_tmpCE7=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpCE8="extra attributes on parameter, ignoring",
_tag_dyneither(_tmpCE8,sizeof(char),40))),_tag_dyneither(_tmpCE7,sizeof(void*),0)));}{
struct _tuple8*_tmpCE9;yyval=Cyc_YY37(((_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((
_tmpCE9->f1=_tmp6A3,((_tmpCE9->f2=_tmp69D,((_tmpCE9->f3=_tmp69E,_tmpCE9)))))))));}
break;};};};}case 234: _LL317: {struct _tuple22 _tmp6A9;struct Cyc_Absyn_Tqual _tmp6AA;
struct Cyc_List_List*_tmp6AB;struct Cyc_List_List*_tmp6AC;struct _tuple22*_tmp6A8=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A9=*
_tmp6A8;_tmp6AA=_tmp6A9.f1;_tmp6AB=_tmp6A9.f2;_tmp6AC=_tmp6A9.f3;if(_tmp6AA.loc
== 0)_tmp6AA.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{void*_tmp6AD=
Cyc_Parse_speclist2typ(_tmp6AB,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6AC != 0){
const char*_tmpCEC;void*_tmpCEB;(_tmpCEB=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpCEC="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpCEC,sizeof(char),38))),_tag_dyneither(_tmpCEB,sizeof(void*),0)));}{
struct _tuple8*_tmpCED;yyval=Cyc_YY37(((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((
_tmpCED->f1=0,((_tmpCED->f2=_tmp6AA,((_tmpCED->f3=_tmp6AD,_tmpCED)))))))));}
break;};}case 235: _LL318: {struct _tuple22 _tmp6B2;struct Cyc_Absyn_Tqual _tmp6B3;
struct Cyc_List_List*_tmp6B4;struct Cyc_List_List*_tmp6B5;struct _tuple22*_tmp6B1=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6B2=*_tmp6B1;_tmp6B3=_tmp6B2.f1;_tmp6B4=_tmp6B2.f2;_tmp6B5=_tmp6B2.f3;if(
_tmp6B3.loc == 0)_tmp6B3.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{void*_tmp6B6=
Cyc_Parse_speclist2typ(_tmp6B4,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6B7=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp6B9;void*_tmp6BA;struct Cyc_List_List*_tmp6BB;struct Cyc_List_List*
_tmp6BC;struct _tuple10 _tmp6B8=Cyc_Parse_apply_tms(_tmp6B3,_tmp6B6,_tmp6B5,
_tmp6B7);_tmp6B9=_tmp6B8.f1;_tmp6BA=_tmp6B8.f2;_tmp6BB=_tmp6B8.f3;_tmp6BC=
_tmp6B8.f4;if(_tmp6BB != 0){const char*_tmpCF0;void*_tmpCEF;(_tmpCEF=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpCF0="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpCF0,sizeof(char),49))),_tag_dyneither(_tmpCEF,sizeof(void*),0)));}
if(_tmp6BC != 0){const char*_tmpCF3;void*_tmpCF2;(_tmpCF2=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpCF3="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpCF3,sizeof(char),38))),_tag_dyneither(_tmpCF2,sizeof(void*),0)));}{
struct _tuple8*_tmpCF4;yyval=Cyc_YY37(((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((
_tmpCF4->f1=0,((_tmpCF4->f2=_tmp6B9,((_tmpCF4->f3=_tmp6BA,_tmpCF4)))))))));}
break;};}case 236: _LL319: yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 237: _LL31A:{struct _dyneither_ptr*_tmpCF7;struct Cyc_List_List*
_tmpCF6;yyval=Cyc_YY36(((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->hd=((
_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCF7)))),((_tmpCF6->tl=0,
_tmpCF6)))))));}break;case 238: _LL31B:{struct _dyneither_ptr*_tmpCFA;struct Cyc_List_List*
_tmpCF9;yyval=Cyc_YY36(((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9->hd=((
_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCFA)))),((_tmpCF9->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpCF9)))))));}break;case 239: _LL31C: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 240: _LL31D: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 241: _LL31E:{struct
Cyc_Absyn_UnresolvedMem_e_struct _tmpCFD;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpCFC;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpCFC=_cycalloc(sizeof(*
_tmpCFC)),((_tmpCFC[0]=((_tmpCFD.tag=36,((_tmpCFD.f1=0,((_tmpCFD.f2=0,_tmpCFD)))))),
_tmpCFC)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 242: _LL31F:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD00;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpCFF;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpCFF=_cycalloc(sizeof(*_tmpCFF)),((_tmpCFF[0]=((_tmpD00.tag=36,((
_tmpD00.f1=0,((_tmpD00.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),
_tmpD00)))))),_tmpCFF)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 243:
_LL320:{struct Cyc_Absyn_UnresolvedMem_e_struct _tmpD03;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpD02;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD02=_cycalloc(sizeof(*
_tmpD02)),((_tmpD02[0]=((_tmpD03.tag=36,((_tmpD03.f1=0,((_tmpD03.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),_tmpD03)))))),_tmpD02)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 244: _LL321: {struct _dyneither_ptr*
_tmpD06;struct _tuple1*_tmpD05;struct Cyc_Absyn_Vardecl*_tmp6CC=Cyc_Absyn_new_vardecl(((
_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05->f1=Cyc_Absyn_Loc_n,((_tmpD05->f2=((
_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpD06)))),_tmpD05)))))),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp6CC->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct _tmpD09;struct Cyc_Absyn_Comprehension_e_struct*
_tmpD08;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD08=_cycalloc(sizeof(*
_tmpD08)),((_tmpD08[0]=((_tmpD09.tag=28,((_tmpD09.f1=_tmp6CC,((_tmpD09.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpD09.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD09.f4=0,
_tmpD09)))))))))),_tmpD08)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
245: _LL322:{struct _tuple27*_tmpD0C;struct Cyc_List_List*_tmpD0B;yyval=Cyc_YY6(((
_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B->hd=((_tmpD0C=_cycalloc(sizeof(*
_tmpD0C)),((_tmpD0C->f1=0,((_tmpD0C->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD0C)))))),((_tmpD0B->tl=
0,_tmpD0B)))))));}break;case 246: _LL323:{struct _tuple27*_tmpD0F;struct Cyc_List_List*
_tmpD0E;yyval=Cyc_YY6(((_tmpD0E=_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E->hd=((
_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD0F->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD0F)))))),((
_tmpD0E->tl=0,_tmpD0E)))))));}break;case 247: _LL324:{struct _tuple27*_tmpD12;
struct Cyc_List_List*_tmpD11;yyval=Cyc_YY6(((_tmpD11=_cycalloc(sizeof(*_tmpD11)),((
_tmpD11->hd=((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12->f1=0,((_tmpD12->f2=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD12)))))),((
_tmpD11->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),_tmpD11)))))));}break;case 248: _LL325:{struct _tuple27*_tmpD15;struct Cyc_List_List*
_tmpD14;yyval=Cyc_YY6(((_tmpD14=_cycalloc(sizeof(*_tmpD14)),((_tmpD14->hd=((
_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD15->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD15)))))),((
_tmpD14->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]),_tmpD14)))))));}break;case 249: _LL326: yyval=Cyc_YY41(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 250: _LL327:{
struct Cyc_List_List*_tmpD16;yyval=Cyc_YY41(((_tmpD16=_cycalloc(sizeof(*_tmpD16)),((
_tmpD16->hd=Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD16->tl=0,_tmpD16)))))));}break;case 251: _LL328:{struct Cyc_List_List*
_tmpD17;yyval=Cyc_YY41(((_tmpD17=_cycalloc(sizeof(*_tmpD17)),((_tmpD17->hd=Cyc_yyget_YY42(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD17->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD17)))))));}
break;case 252: _LL329:{struct Cyc_Absyn_ArrayElement_struct _tmpD1A;struct Cyc_Absyn_ArrayElement_struct*
_tmpD19;yyval=Cyc_YY42((void*)((_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19[0]=((
_tmpD1A.tag=0,((_tmpD1A.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpD1A)))),_tmpD19)))));}break;case 253: _LL32A:{struct
Cyc_Absyn_FieldName_struct _tmpD20;struct _dyneither_ptr*_tmpD1F;struct Cyc_Absyn_FieldName_struct*
_tmpD1E;yyval=Cyc_YY42((void*)((_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((_tmpD1E[0]=((
_tmpD20.tag=1,((_tmpD20.f1=((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpD1F)))),_tmpD20)))),_tmpD1E)))));}break;case 254: _LL32B: {struct _tuple22
_tmp6E1;struct Cyc_Absyn_Tqual _tmp6E2;struct Cyc_List_List*_tmp6E3;struct Cyc_List_List*
_tmp6E4;struct _tuple22*_tmp6E0=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E1=*_tmp6E0;_tmp6E2=
_tmp6E1.f1;_tmp6E3=_tmp6E1.f2;_tmp6E4=_tmp6E1.f3;{void*_tmp6E5=Cyc_Parse_speclist2typ(
_tmp6E3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp6E4 != 0){const char*_tmpD23;void*_tmpD22;(
_tmpD22=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD23="ignoring attributes in type",
_tag_dyneither(_tmpD23,sizeof(char),28))),_tag_dyneither(_tmpD22,sizeof(void*),0)));}{
struct _tuple8*_tmpD24;yyval=Cyc_YY37(((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((
_tmpD24->f1=0,((_tmpD24->f2=_tmp6E2,((_tmpD24->f3=_tmp6E5,_tmpD24)))))))));}
break;};}case 255: _LL32C: {struct _tuple22 _tmp6EA;struct Cyc_Absyn_Tqual _tmp6EB;
struct Cyc_List_List*_tmp6EC;struct Cyc_List_List*_tmp6ED;struct _tuple22*_tmp6E9=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6EA=*_tmp6E9;_tmp6EB=_tmp6EA.f1;_tmp6EC=_tmp6EA.f2;_tmp6ED=_tmp6EA.f3;{void*
_tmp6EE=Cyc_Parse_speclist2typ(_tmp6EC,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6EF=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple10 _tmp6F0=Cyc_Parse_apply_tms(_tmp6EB,_tmp6EE,_tmp6ED,_tmp6EF);if(
_tmp6F0.f3 != 0){const char*_tmpD27;void*_tmpD26;(_tmpD26=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD27="bad type params, ignoring",
_tag_dyneither(_tmpD27,sizeof(char),26))),_tag_dyneither(_tmpD26,sizeof(void*),0)));}
if(_tmp6F0.f4 != 0){const char*_tmpD2A;void*_tmpD29;(_tmpD29=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD2A="bad specifiers, ignoring",
_tag_dyneither(_tmpD2A,sizeof(char),25))),_tag_dyneither(_tmpD29,sizeof(void*),0)));}{
struct _tuple8*_tmpD2B;yyval=Cyc_YY37(((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((
_tmpD2B->f1=0,((_tmpD2B->f2=_tmp6F0.f1,((_tmpD2B->f3=_tmp6F0.f2,_tmpD2B)))))))));}
break;};}case 256: _LL32D: yyval=Cyc_YY44((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);break;case 257: _LL32E:{
struct Cyc_Absyn_JoinEff_struct _tmpD2E;struct Cyc_Absyn_JoinEff_struct*_tmpD2D;
yyval=Cyc_YY44((void*)((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D[0]=((
_tmpD2E.tag=24,((_tmpD2E.f1=0,_tmpD2E)))),_tmpD2D)))));}break;case 258: _LL32F:{
struct Cyc_Absyn_JoinEff_struct _tmpD31;struct Cyc_Absyn_JoinEff_struct*_tmpD30;
yyval=Cyc_YY44((void*)((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30[0]=((
_tmpD31.tag=24,((_tmpD31.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpD31)))),_tmpD30)))));}break;case 259: _LL330:{struct
Cyc_Absyn_RgnsEff_struct _tmpD34;struct Cyc_Absyn_RgnsEff_struct*_tmpD33;yyval=Cyc_YY44((
void*)((_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33[0]=((_tmpD34.tag=25,((
_tmpD34.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpD34)))),_tmpD33)))));}break;case 260: _LL331:{struct Cyc_Absyn_JoinEff_struct
_tmpD3A;struct Cyc_List_List*_tmpD39;struct Cyc_Absyn_JoinEff_struct*_tmpD38;yyval=
Cyc_YY44((void*)((_tmpD38=_cycalloc(sizeof(*_tmpD38)),((_tmpD38[0]=((_tmpD3A.tag=
24,((_tmpD3A.f1=((_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39->hd=Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD39->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD39)))))),_tmpD3A)))),
_tmpD38)))));}break;case 261: _LL332:{struct Cyc_List_List*_tmpD3B;yyval=Cyc_YY40(((
_tmpD3B=_cycalloc(sizeof(*_tmpD3B)),((_tmpD3B->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD3B->tl=0,_tmpD3B)))))));}
break;case 262: _LL333:{struct Cyc_List_List*_tmpD3C;yyval=Cyc_YY40(((_tmpD3C=
_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD3C->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD3C)))))));}
break;case 263: _LL334:{struct Cyc_Parse_Abstractdeclarator*_tmpD3D;yyval=Cyc_YY30(((
_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD3D)))));}break;case 264:
_LL335: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
265: _LL336:{struct Cyc_Parse_Abstractdeclarator*_tmpD3E;yyval=Cyc_YY30(((_tmpD3E=
_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpD3E)))));}break;
case 266: _LL337: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 267: _LL338:{struct Cyc_List_List*_tmpD48;struct Cyc_Absyn_Carray_mod_struct
_tmpD47;struct Cyc_Absyn_Carray_mod_struct*_tmpD46;struct Cyc_Parse_Abstractdeclarator*
_tmpD45;yyval=Cyc_YY30(((_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45->tms=((
_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->hd=(void*)((_tmpD46=_cycalloc(
sizeof(*_tmpD46)),((_tmpD46[0]=((_tmpD47.tag=0,((_tmpD47.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD47.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD47)))))),
_tmpD46)))),((_tmpD48->tl=0,_tmpD48)))))),_tmpD45)))));}break;case 268: _LL339:{
struct Cyc_List_List*_tmpD52;struct Cyc_Absyn_Carray_mod_struct _tmpD51;struct Cyc_Absyn_Carray_mod_struct*
_tmpD50;struct Cyc_Parse_Abstractdeclarator*_tmpD4F;yyval=Cyc_YY30(((_tmpD4F=
_cycalloc(sizeof(*_tmpD4F)),((_tmpD4F->tms=((_tmpD52=_cycalloc(sizeof(*_tmpD52)),((
_tmpD52->hd=(void*)((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50[0]=((_tmpD51.tag=
0,((_tmpD51.f1=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD51.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD51)))))),
_tmpD50)))),((_tmpD52->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD52)))))),_tmpD4F)))));}break;case 269: _LL33A:{
struct Cyc_List_List*_tmpD5C;struct Cyc_Absyn_ConstArray_mod_struct _tmpD5B;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD5A;struct Cyc_Parse_Abstractdeclarator*
_tmpD59;yyval=Cyc_YY30(((_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59->tms=((
_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C->hd=(void*)((_tmpD5A=_cycalloc(
sizeof(*_tmpD5A)),((_tmpD5A[0]=((_tmpD5B.tag=1,((_tmpD5B.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD5B.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD5B.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD5B)))))))),
_tmpD5A)))),((_tmpD5C->tl=0,_tmpD5C)))))),_tmpD59)))));}break;case 270: _LL33B:{
struct Cyc_List_List*_tmpD66;struct Cyc_Absyn_ConstArray_mod_struct _tmpD65;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD64;struct Cyc_Parse_Abstractdeclarator*
_tmpD63;yyval=Cyc_YY30(((_tmpD63=_cycalloc(sizeof(*_tmpD63)),((_tmpD63->tms=((
_tmpD66=_cycalloc(sizeof(*_tmpD66)),((_tmpD66->hd=(void*)((_tmpD64=_cycalloc(
sizeof(*_tmpD64)),((_tmpD64[0]=((_tmpD65.tag=1,((_tmpD65.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD65.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD65.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD65)))))))),
_tmpD64)))),((_tmpD66->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpD66)))))),_tmpD63)))));}break;case 271: _LL33C:{
struct Cyc_List_List*_tmpD7B;struct Cyc_Absyn_Function_mod_struct _tmpD7A;struct Cyc_Absyn_WithTypes_struct
_tmpD79;struct Cyc_Absyn_WithTypes_struct*_tmpD78;struct Cyc_Absyn_Function_mod_struct*
_tmpD77;struct Cyc_Parse_Abstractdeclarator*_tmpD76;yyval=Cyc_YY30(((_tmpD76=
_cycalloc(sizeof(*_tmpD76)),((_tmpD76->tms=((_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((
_tmpD7B->hd=(void*)((_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77[0]=((_tmpD7A.tag=
3,((_tmpD7A.f1=(void*)((void*)((_tmpD78=_cycalloc(sizeof(*_tmpD78)),((_tmpD78[0]=((
_tmpD79.tag=1,((_tmpD79.f1=0,((_tmpD79.f2=0,((_tmpD79.f3=0,((_tmpD79.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD79.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD79)))))))))))),
_tmpD78))))),_tmpD7A)))),_tmpD77)))),((_tmpD7B->tl=0,_tmpD7B)))))),_tmpD76)))));}
break;case 272: _LL33D: {struct _tuple23 _tmp71A;struct Cyc_List_List*_tmp71B;int
_tmp71C;struct Cyc_Absyn_VarargInfo*_tmp71D;struct Cyc_Core_Opt*_tmp71E;struct Cyc_List_List*
_tmp71F;struct _tuple23*_tmp719=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp71A=*_tmp719;_tmp71B=
_tmp71A.f1;_tmp71C=_tmp71A.f2;_tmp71D=_tmp71A.f3;_tmp71E=_tmp71A.f4;_tmp71F=
_tmp71A.f5;{struct Cyc_List_List*_tmpD90;struct Cyc_Absyn_Function_mod_struct
_tmpD8F;struct Cyc_Absyn_WithTypes_struct _tmpD8E;struct Cyc_Absyn_WithTypes_struct*
_tmpD8D;struct Cyc_Absyn_Function_mod_struct*_tmpD8C;struct Cyc_Parse_Abstractdeclarator*
_tmpD8B;yyval=Cyc_YY30(((_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B->tms=((
_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90->hd=(void*)((_tmpD8C=_cycalloc(
sizeof(*_tmpD8C)),((_tmpD8C[0]=((_tmpD8F.tag=3,((_tmpD8F.f1=(void*)((void*)((
_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D[0]=((_tmpD8E.tag=1,((_tmpD8E.f1=
_tmp71B,((_tmpD8E.f2=_tmp71C,((_tmpD8E.f3=_tmp71D,((_tmpD8E.f4=_tmp71E,((_tmpD8E.f5=
_tmp71F,_tmpD8E)))))))))))),_tmpD8D))))),_tmpD8F)))),_tmpD8C)))),((_tmpD90->tl=0,
_tmpD90)))))),_tmpD8B)))));}break;}case 273: _LL33E:{struct Cyc_List_List*_tmpDA5;
struct Cyc_Absyn_Function_mod_struct _tmpDA4;struct Cyc_Absyn_WithTypes_struct
_tmpDA3;struct Cyc_Absyn_WithTypes_struct*_tmpDA2;struct Cyc_Absyn_Function_mod_struct*
_tmpDA1;struct Cyc_Parse_Abstractdeclarator*_tmpDA0;yyval=Cyc_YY30(((_tmpDA0=
_cycalloc(sizeof(*_tmpDA0)),((_tmpDA0->tms=((_tmpDA5=_cycalloc(sizeof(*_tmpDA5)),((
_tmpDA5->hd=(void*)((_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1[0]=((_tmpDA4.tag=
3,((_tmpDA4.f1=(void*)((void*)((_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2[0]=((
_tmpDA3.tag=1,((_tmpDA3.f1=0,((_tmpDA3.f2=0,((_tmpDA3.f3=0,((_tmpDA3.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDA3.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDA3)))))))))))),
_tmpDA2))))),_tmpDA4)))),_tmpDA1)))),((_tmpDA5->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpDA5)))))),
_tmpDA0)))));}break;case 274: _LL33F: {struct _tuple23 _tmp72D;struct Cyc_List_List*
_tmp72E;int _tmp72F;struct Cyc_Absyn_VarargInfo*_tmp730;struct Cyc_Core_Opt*_tmp731;
struct Cyc_List_List*_tmp732;struct _tuple23*_tmp72C=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp72D=*_tmp72C;_tmp72E=
_tmp72D.f1;_tmp72F=_tmp72D.f2;_tmp730=_tmp72D.f3;_tmp731=_tmp72D.f4;_tmp732=
_tmp72D.f5;{struct Cyc_List_List*_tmpDBA;struct Cyc_Absyn_Function_mod_struct
_tmpDB9;struct Cyc_Absyn_WithTypes_struct _tmpDB8;struct Cyc_Absyn_WithTypes_struct*
_tmpDB7;struct Cyc_Absyn_Function_mod_struct*_tmpDB6;struct Cyc_Parse_Abstractdeclarator*
_tmpDB5;yyval=Cyc_YY30(((_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->tms=((
_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->hd=(void*)((_tmpDB6=_cycalloc(
sizeof(*_tmpDB6)),((_tmpDB6[0]=((_tmpDB9.tag=3,((_tmpDB9.f1=(void*)((void*)((
_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7[0]=((_tmpDB8.tag=1,((_tmpDB8.f1=
_tmp72E,((_tmpDB8.f2=_tmp72F,((_tmpDB8.f3=_tmp730,((_tmpDB8.f4=_tmp731,((_tmpDB8.f5=
_tmp732,_tmpDB8)))))))))))),_tmpDB7))))),_tmpDB9)))),_tmpDB6)))),((_tmpDBA->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpDBA)))))),_tmpDB5)))));}break;}case 275: _LL340: {struct Cyc_List_List*_tmp739=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{
struct Cyc_List_List*_tmpDC4;struct Cyc_Absyn_TypeParams_mod_struct _tmpDC3;struct
Cyc_Absyn_TypeParams_mod_struct*_tmpDC2;struct Cyc_Parse_Abstractdeclarator*
_tmpDC1;yyval=Cyc_YY30(((_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->tms=((
_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4->hd=(void*)((_tmpDC2=_cycalloc(
sizeof(*_tmpDC2)),((_tmpDC2[0]=((_tmpDC3.tag=4,((_tmpDC3.f1=_tmp739,((_tmpDC3.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpDC3.f3=0,_tmpDC3)))))))),_tmpDC2)))),((_tmpDC4->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpDC4)))))),_tmpDC1)))));}break;}case 276: _LL341:{struct Cyc_List_List*_tmpDCE;
struct Cyc_Absyn_Attributes_mod_struct _tmpDCD;struct Cyc_Absyn_Attributes_mod_struct*
_tmpDCC;struct Cyc_Parse_Abstractdeclarator*_tmpDCB;yyval=Cyc_YY30(((_tmpDCB=
_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB->tms=((_tmpDCE=_cycalloc(sizeof(*_tmpDCE)),((
_tmpDCE->hd=(void*)((_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC[0]=((_tmpDCD.tag=
5,((_tmpDCD.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpDCD.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDCD)))))),_tmpDCC)))),((
_tmpDCE->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpDCE)))))),_tmpDCB)))));}break;case 277: _LL342: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 278: _LL343:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 279:
_LL344: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
280: _LL345: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 281: _LL346: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 282: _LL347: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 283: _LL348:{struct Cyc_Absyn_ResetRegion_s_struct _tmpDD1;
struct Cyc_Absyn_ResetRegion_s_struct*_tmpDD0;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((
void*)((_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0[0]=((_tmpDD1.tag=16,((
_tmpDD1.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),_tmpDD1)))),_tmpDD0)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 284:
_LL349: yyval=Cyc_YY4(0);break;case 285: _LL34A:{const char*_tmpDD2;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmpDD2="open",_tag_dyneither(_tmpDD2,sizeof(char),5))))
!= 0){const char*_tmpDD3;Cyc_Parse_err(((_tmpDD3="expecting `open'",
_tag_dyneither(_tmpDD3,sizeof(char),17))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}yyval=Cyc_YY4((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));break;case 286: _LL34B:{struct Cyc_Absyn_Label_s_struct _tmpDD9;
struct _dyneither_ptr*_tmpDD8;struct Cyc_Absyn_Label_s_struct*_tmpDD7;yyval=Cyc_YY9(
Cyc_Absyn_new_stmt((void*)((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7[0]=((
_tmpDD9.tag=13,((_tmpDD9.f1=((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpDD8)))),((_tmpDD9.f2=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpDD9)))))),_tmpDD7)))),Cyc_Position_segment_of_abs((yyls[
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
_LL354:{struct Cyc_Absyn_Fn_d_struct _tmpDDC;struct Cyc_Absyn_Fn_d_struct*_tmpDDB;
yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDDB=
_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB[0]=((_tmpDDC.tag=1,((_tmpDDC.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDDC)))),_tmpDDB)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0)));}break;case 296: _LL355:{
struct Cyc_Absyn_Fn_d_struct _tmpDDF;struct Cyc_Absyn_Fn_d_struct*_tmpDDE;yyval=Cyc_YY9(
Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDDE=_cycalloc(sizeof(*
_tmpDDE)),((_tmpDDE[0]=((_tmpDDF.tag=1,((_tmpDDF.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDDF)))),_tmpDDE)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));}break;case 297: _LL356: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 298: _LL357: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 299:
_LL358: yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 300:
_LL359: {struct Cyc_Absyn_UnknownId_e_struct _tmpDE2;struct Cyc_Absyn_UnknownId_e_struct*
_tmpDE1;struct Cyc_Absyn_Exp*_tmp74D=Cyc_Absyn_new_exp((void*)((_tmpDE1=_cycalloc(
sizeof(*_tmpDE1)),((_tmpDE1[0]=((_tmpDE2.tag=2,((_tmpDE2.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpDE2)))),_tmpDE1)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp74D,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 301:
_LL35A: {struct Cyc_Absyn_Exp*_tmp750=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(
Cyc_Absyn_switch_stmt(_tmp750,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 302:
_LL35B: yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 303:
_LL35C: yyval=Cyc_YY10(0);break;case 304: _LL35D:{struct Cyc_Absyn_Switch_clause*
_tmpDE5;struct Cyc_List_List*_tmpDE4;yyval=Cyc_YY10(((_tmpDE4=_cycalloc(sizeof(*
_tmpDE4)),((_tmpDE4->hd=((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->pattern=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)),((_tmpDE5->pat_vars=
0,((_tmpDE5->where_clause=0,((_tmpDE5->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDE5->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDE5)))))))))))),((
_tmpDE4->tl=0,_tmpDE4)))))));}break;case 305: _LL35E:{struct Cyc_Absyn_Switch_clause*
_tmpDE8;struct Cyc_List_List*_tmpDE7;yyval=Cyc_YY10(((_tmpDE7=_cycalloc(sizeof(*
_tmpDE7)),((_tmpDE7->hd=((_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpDE8->pat_vars=0,((_tmpDE8->where_clause=0,((_tmpDE8->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpDE8->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDE8)))))))))))),((
_tmpDE7->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpDE7)))))));}break;case 306: _LL35F:{struct Cyc_Absyn_Switch_clause*
_tmpDEB;struct Cyc_List_List*_tmpDEA;yyval=Cyc_YY10(((_tmpDEA=_cycalloc(sizeof(*
_tmpDEA)),((_tmpDEA->hd=((_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpDEB->pat_vars=0,((_tmpDEB->where_clause=0,((_tmpDEB->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpDEB->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpDEB)))))))))))),((
_tmpDEA->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpDEA)))))));}break;case 307: _LL360:{struct Cyc_Absyn_Switch_clause*
_tmpDEE;struct Cyc_List_List*_tmpDED;yyval=Cyc_YY10(((_tmpDED=_cycalloc(sizeof(*
_tmpDED)),((_tmpDED->hd=((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((
_tmpDEE->pat_vars=0,((_tmpDEE->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDEE->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpDEE->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDEE)))))))))))),((
_tmpDED->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpDED)))))));}break;case 308: _LL361:{struct Cyc_Absyn_Switch_clause*
_tmpDF1;struct Cyc_List_List*_tmpDF0;yyval=Cyc_YY10(((_tmpDF0=_cycalloc(sizeof(*
_tmpDF0)),((_tmpDF0->hd=((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((
_tmpDF1->pat_vars=0,((_tmpDF1->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpDF1->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpDF1->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDF1)))))))))))),((
_tmpDF0->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpDF0)))))));}break;case 309: _LL362: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
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
_LL36C: {struct Cyc_List_List*_tmp75B=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp75C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp75B,_tmp75C));break;}case 320: _LL36D: {struct
Cyc_List_List*_tmp75D=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp75E=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp75D,
_tmp75E));break;}case 321: _LL36E: {struct Cyc_List_List*_tmp75F=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp760=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp75F,_tmp760));break;}case 322: _LL36F: {struct
Cyc_List_List*_tmp761=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp762=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp761,_tmp762));break;}case 323: _LL370:{struct
_dyneither_ptr*_tmpDF2;yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((_tmpDF2=_cycalloc(
sizeof(*_tmpDF2)),((_tmpDF2[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDF2)))),Cyc_Position_segment_of_abs((
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
363: _LL398: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 364: _LL399: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 365: _LL39A: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));break;case 366: _LL39B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 367: _LL39C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 368:
_LL39D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 369:
_LL39E:{struct Cyc_Absyn_StructField_struct _tmpDF8;struct _dyneither_ptr*_tmpDF7;
struct Cyc_Absyn_StructField_struct*_tmpDF6;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6[0]=((_tmpDF8.tag=0,((
_tmpDF8.f1=((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDF7)))),_tmpDF8)))),
_tmpDF6)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 370: _LL39F:{struct Cyc_Absyn_TupleIndex_struct
_tmpDFB;struct Cyc_Absyn_TupleIndex_struct*_tmpDFA;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)((_tmpDFA=_cycalloc_atomic(sizeof(*_tmpDFA)),((
_tmpDFA[0]=((_tmpDFB.tag=1,((_tmpDFB.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,_tmpDFB)))),_tmpDFA)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 371: _LL3A0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 372: _LL3A1: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 373: _LL3A2:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 374:
_LL3A3: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 375: _LL3A4: {
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);{void*_tmp769=e->r;union Cyc_Absyn_Cnst _tmp76B;struct _tuple4
_tmp76C;enum Cyc_Absyn_Sign _tmp76D;char _tmp76E;union Cyc_Absyn_Cnst _tmp770;struct
_tuple5 _tmp771;enum Cyc_Absyn_Sign _tmp772;short _tmp773;union Cyc_Absyn_Cnst
_tmp775;struct _tuple6 _tmp776;enum Cyc_Absyn_Sign _tmp777;int _tmp778;union Cyc_Absyn_Cnst
_tmp77A;struct _dyneither_ptr _tmp77B;union Cyc_Absyn_Cnst _tmp77D;int _tmp77E;union
Cyc_Absyn_Cnst _tmp780;struct _dyneither_ptr _tmp781;union Cyc_Absyn_Cnst _tmp783;
struct _tuple7 _tmp784;_LL3A7: {struct Cyc_Absyn_Const_e_struct*_tmp76A=(struct Cyc_Absyn_Const_e_struct*)
_tmp769;if(_tmp76A->tag != 0)goto _LL3A9;else{_tmp76B=_tmp76A->f1;if((_tmp76B.Char_c).tag
!= 2)goto _LL3A9;_tmp76C=(struct _tuple4)(_tmp76B.Char_c).val;_tmp76D=_tmp76C.f1;
_tmp76E=_tmp76C.f2;}}_LL3A8:{struct Cyc_Absyn_Char_p_struct _tmpDFE;struct Cyc_Absyn_Char_p_struct*
_tmpDFD;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDFD=_cycalloc_atomic(
sizeof(*_tmpDFD)),((_tmpDFD[0]=((_tmpDFE.tag=10,((_tmpDFE.f1=_tmp76E,_tmpDFE)))),
_tmpDFD)))),e->loc));}goto _LL3A6;_LL3A9: {struct Cyc_Absyn_Const_e_struct*_tmp76F=(
struct Cyc_Absyn_Const_e_struct*)_tmp769;if(_tmp76F->tag != 0)goto _LL3AB;else{
_tmp770=_tmp76F->f1;if((_tmp770.Short_c).tag != 3)goto _LL3AB;_tmp771=(struct
_tuple5)(_tmp770.Short_c).val;_tmp772=_tmp771.f1;_tmp773=_tmp771.f2;}}_LL3AA:{
struct Cyc_Absyn_Int_p_struct _tmpE01;struct Cyc_Absyn_Int_p_struct*_tmpE00;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE00=_cycalloc_atomic(sizeof(*_tmpE00)),((
_tmpE00[0]=((_tmpE01.tag=9,((_tmpE01.f1=_tmp772,((_tmpE01.f2=(int)_tmp773,
_tmpE01)))))),_tmpE00)))),e->loc));}goto _LL3A6;_LL3AB: {struct Cyc_Absyn_Const_e_struct*
_tmp774=(struct Cyc_Absyn_Const_e_struct*)_tmp769;if(_tmp774->tag != 0)goto _LL3AD;
else{_tmp775=_tmp774->f1;if((_tmp775.Int_c).tag != 4)goto _LL3AD;_tmp776=(struct
_tuple6)(_tmp775.Int_c).val;_tmp777=_tmp776.f1;_tmp778=_tmp776.f2;}}_LL3AC:{
struct Cyc_Absyn_Int_p_struct _tmpE04;struct Cyc_Absyn_Int_p_struct*_tmpE03;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE03=_cycalloc_atomic(sizeof(*_tmpE03)),((
_tmpE03[0]=((_tmpE04.tag=9,((_tmpE04.f1=_tmp777,((_tmpE04.f2=_tmp778,_tmpE04)))))),
_tmpE03)))),e->loc));}goto _LL3A6;_LL3AD: {struct Cyc_Absyn_Const_e_struct*_tmp779=(
struct Cyc_Absyn_Const_e_struct*)_tmp769;if(_tmp779->tag != 0)goto _LL3AF;else{
_tmp77A=_tmp779->f1;if((_tmp77A.Float_c).tag != 6)goto _LL3AF;_tmp77B=(struct
_dyneither_ptr)(_tmp77A.Float_c).val;}}_LL3AE:{struct Cyc_Absyn_Float_p_struct
_tmpE07;struct Cyc_Absyn_Float_p_struct*_tmpE06;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06[0]=((_tmpE07.tag=11,((
_tmpE07.f1=_tmp77B,_tmpE07)))),_tmpE06)))),e->loc));}goto _LL3A6;_LL3AF: {struct
Cyc_Absyn_Const_e_struct*_tmp77C=(struct Cyc_Absyn_Const_e_struct*)_tmp769;if(
_tmp77C->tag != 0)goto _LL3B1;else{_tmp77D=_tmp77C->f1;if((_tmp77D.Null_c).tag != 1)
goto _LL3B1;_tmp77E=(int)(_tmp77D.Null_c).val;}}_LL3B0: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A6;_LL3B1: {struct Cyc_Absyn_Const_e_struct*
_tmp77F=(struct Cyc_Absyn_Const_e_struct*)_tmp769;if(_tmp77F->tag != 0)goto _LL3B3;
else{_tmp780=_tmp77F->f1;if((_tmp780.String_c).tag != 7)goto _LL3B3;_tmp781=(
struct _dyneither_ptr)(_tmp780.String_c).val;}}_LL3B2:{const char*_tmpE08;Cyc_Parse_err(((
_tmpE08="strings cannot occur within patterns",_tag_dyneither(_tmpE08,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL3A6;_LL3B3: {struct Cyc_Absyn_Const_e_struct*
_tmp782=(struct Cyc_Absyn_Const_e_struct*)_tmp769;if(_tmp782->tag != 0)goto _LL3B5;
else{_tmp783=_tmp782->f1;if((_tmp783.LongLong_c).tag != 5)goto _LL3B5;_tmp784=(
struct _tuple7)(_tmp783.LongLong_c).val;}}_LL3B4:{const char*_tmpE09;Cyc_Parse_unimp(((
_tmpE09="long long's in patterns",_tag_dyneither(_tmpE09,sizeof(char),24))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}goto _LL3A6;
_LL3B5:;_LL3B6: {const char*_tmpE0A;Cyc_Parse_err(((_tmpE0A="bad constant in case",
_tag_dyneither(_tmpE0A,sizeof(char),21))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}_LL3A6:;}
break;}case 376: _LL3A5:{struct Cyc_Absyn_UnknownId_p_struct _tmpE0D;struct Cyc_Absyn_UnknownId_p_struct*
_tmpE0C;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE0C=_cycalloc(sizeof(*
_tmpE0C)),((_tmpE0C[0]=((_tmpE0D.tag=14,((_tmpE0D.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE0D)))),_tmpE0C)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 377:
_LL3B7:{const char*_tmpE0E;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE0E="as",
_tag_dyneither(_tmpE0E,sizeof(char),3))))!= 0){const char*_tmpE0F;Cyc_Parse_err(((
_tmpE0F="expecting `as'",_tag_dyneither(_tmpE0F,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Var_p_struct
_tmpE19;struct _dyneither_ptr*_tmpE18;struct _tuple1*_tmpE17;struct Cyc_Absyn_Var_p_struct*
_tmpE16;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE16=_cycalloc(sizeof(*
_tmpE16)),((_tmpE16[0]=((_tmpE19.tag=1,((_tmpE19.f1=Cyc_Absyn_new_vardecl(((
_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17->f1=Cyc_Absyn_Loc_n,((_tmpE17->f2=((
_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE18)))),_tmpE17)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE19.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE19)))))),_tmpE16)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 378: _LL3B8: {struct Cyc_List_List*
_tmp799;int _tmp79A;struct _tuple19 _tmp798=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp799=_tmp798.f1;
_tmp79A=_tmp798.f2;{struct Cyc_Absyn_Tuple_p_struct _tmpE1C;struct Cyc_Absyn_Tuple_p_struct*
_tmpE1B;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE1B=_cycalloc(sizeof(*
_tmpE1B)),((_tmpE1B[0]=((_tmpE1C.tag=4,((_tmpE1C.f1=_tmp799,((_tmpE1C.f2=_tmp79A,
_tmpE1C)))))),_tmpE1B)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
379: _LL3B9: {struct Cyc_List_List*_tmp79E;int _tmp79F;struct _tuple19 _tmp79D=*Cyc_yyget_YY12(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp79E=_tmp79D.f1;
_tmp79F=_tmp79D.f2;{struct Cyc_Absyn_UnknownCall_p_struct _tmpE1F;struct Cyc_Absyn_UnknownCall_p_struct*
_tmpE1E;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE1E=_cycalloc(sizeof(*
_tmpE1E)),((_tmpE1E[0]=((_tmpE1F.tag=15,((_tmpE1F.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpE1F.f2=_tmp79E,((
_tmpE1F.f3=_tmp79F,_tmpE1F)))))))),_tmpE1E)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
380: _LL3BA: {struct Cyc_List_List*_tmp7A3;int _tmp7A4;struct _tuple19 _tmp7A2=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A3=_tmp7A2.f1;
_tmp7A4=_tmp7A2.f2;{struct Cyc_List_List*_tmp7A5=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpE25;struct Cyc_Absyn_AggrInfo*_tmpE24;struct Cyc_Absyn_Aggr_p_struct*_tmpE23;
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE23=_cycalloc(sizeof(*_tmpE23)),((
_tmpE23[0]=((_tmpE25.tag=6,((_tmpE25.f1=((_tmpE24=_cycalloc(sizeof(*_tmpE24)),((
_tmpE24->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),0),((_tmpE24->targs=
0,_tmpE24)))))),((_tmpE25.f2=_tmp7A5,((_tmpE25.f3=_tmp7A3,((_tmpE25.f4=_tmp7A4,
_tmpE25)))))))))),_tmpE23)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;};}case
381: _LL3BB: {struct Cyc_List_List*_tmp7AA;int _tmp7AB;struct _tuple19 _tmp7A9=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7AA=_tmp7A9.f1;
_tmp7AB=_tmp7A9.f2;{struct Cyc_List_List*_tmp7AC=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpE28;struct Cyc_Absyn_Aggr_p_struct*_tmpE27;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE27=_cycalloc(sizeof(*_tmpE27)),((_tmpE27[0]=((_tmpE28.tag=6,((
_tmpE28.f1=0,((_tmpE28.f2=_tmp7AC,((_tmpE28.f3=_tmp7AA,((_tmpE28.f4=_tmp7AB,
_tmpE28)))))))))),_tmpE27)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;};}case
382: _LL3BC:{struct Cyc_Absyn_Pointer_p_struct _tmpE2B;struct Cyc_Absyn_Pointer_p_struct*
_tmpE2A;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE2A=_cycalloc(sizeof(*
_tmpE2A)),((_tmpE2A[0]=((_tmpE2B.tag=5,((_tmpE2B.f1=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE2B)))),_tmpE2A)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 383:
_LL3BD:{struct Cyc_Absyn_Pointer_p_struct _tmpE35;struct Cyc_Absyn_Pointer_p_struct
_tmpE34;struct Cyc_Absyn_Pointer_p_struct*_tmpE33;struct Cyc_Absyn_Pointer_p_struct*
_tmpE32;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE32=_cycalloc(sizeof(*
_tmpE32)),((_tmpE32[0]=((_tmpE35.tag=5,((_tmpE35.f1=Cyc_Absyn_new_pat((void*)((
_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33[0]=((_tmpE34.tag=5,((_tmpE34.f1=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE34)))),
_tmpE33)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpE35)))),_tmpE32)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 384:
_LL3BE:{struct Cyc_Absyn_Reference_p_struct _tmpE3F;struct _dyneither_ptr*_tmpE3E;
struct _tuple1*_tmpE3D;struct Cyc_Absyn_Reference_p_struct*_tmpE3C;yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)((_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C[0]=((
_tmpE3F.tag=2,((_tmpE3F.f1=Cyc_Absyn_new_vardecl(((_tmpE3D=_cycalloc(sizeof(*
_tmpE3D)),((_tmpE3D->f1=Cyc_Absyn_Loc_n,((_tmpE3D->f2=((_tmpE3E=_cycalloc(
sizeof(*_tmpE3E)),((_tmpE3E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE3E)))),_tmpE3D)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE3F.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpE3F)))))),_tmpE3C)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 385:
_LL3BF:{const char*_tmpE40;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE40="as",
_tag_dyneither(_tmpE40,sizeof(char),3))))!= 0){const char*_tmpE41;Cyc_Parse_err(((
_tmpE41="expecting `as'",_tag_dyneither(_tmpE41,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Reference_p_struct
_tmpE4B;struct _dyneither_ptr*_tmpE4A;struct _tuple1*_tmpE49;struct Cyc_Absyn_Reference_p_struct*
_tmpE48;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE48=_cycalloc(sizeof(*
_tmpE48)),((_tmpE48[0]=((_tmpE4B.tag=2,((_tmpE4B.f1=Cyc_Absyn_new_vardecl(((
_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49->f1=Cyc_Absyn_Loc_n,((_tmpE49->f2=((
_tmpE4A=_cycalloc(sizeof(*_tmpE4A)),((_tmpE4A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE4A)))),_tmpE49)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE4B.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE4B)))))),_tmpE48)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 386: _LL3C0: {void*_tmp7BF=Cyc_Parse_id2type(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct
_tmpE5C;struct Cyc_Absyn_TagType_struct _tmpE5B;struct Cyc_Absyn_TagType_struct*
_tmpE5A;struct _dyneither_ptr*_tmpE59;struct _tuple1*_tmpE58;struct Cyc_Absyn_TagInt_p_struct*
_tmpE57;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE57=_cycalloc(sizeof(*
_tmpE57)),((_tmpE57[0]=((_tmpE5C.tag=3,((_tmpE5C.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp7BF),((
_tmpE5C.f2=Cyc_Absyn_new_vardecl(((_tmpE58=_cycalloc(sizeof(*_tmpE58)),((_tmpE58->f1=
Cyc_Absyn_Loc_n,((_tmpE58->f2=((_tmpE59=_cycalloc(sizeof(*_tmpE59)),((_tmpE59[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpE59)))),_tmpE58)))))),(void*)((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A[
0]=((_tmpE5B.tag=20,((_tmpE5B.f1=(void*)_tmp7BF,_tmpE5B)))),_tmpE5A)))),0),
_tmpE5C)))))),_tmpE57)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
387: _LL3C1: {struct Cyc_Absyn_Tvar*_tmp7C6=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(&
Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct _tmpE78;struct Cyc_Absyn_TagType_struct
_tmpE77;struct Cyc_Absyn_VarType_struct _tmpE76;struct Cyc_Absyn_VarType_struct*
_tmpE75;struct Cyc_Absyn_TagType_struct*_tmpE74;struct _dyneither_ptr*_tmpE73;
struct _tuple1*_tmpE72;struct Cyc_Absyn_TagInt_p_struct*_tmpE71;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71[0]=((_tmpE78.tag=3,((
_tmpE78.f1=_tmp7C6,((_tmpE78.f2=Cyc_Absyn_new_vardecl(((_tmpE72=_cycalloc(
sizeof(*_tmpE72)),((_tmpE72->f1=Cyc_Absyn_Loc_n,((_tmpE72->f2=((_tmpE73=
_cycalloc(sizeof(*_tmpE73)),((_tmpE73[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpE73)))),_tmpE72)))))),(
void*)((_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74[0]=((_tmpE77.tag=20,((
_tmpE77.f1=(void*)((void*)((_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75[0]=((
_tmpE76.tag=2,((_tmpE76.f1=_tmp7C6,_tmpE76)))),_tmpE75))))),_tmpE77)))),_tmpE74)))),
0),_tmpE78)))))),_tmpE71)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
388: _LL3C2:{struct _tuple19*_tmpE79;yyval=Cyc_YY12(((_tmpE79=_cycalloc(sizeof(*
_tmpE79)),((_tmpE79->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpE79->f2=0,_tmpE79)))))));}break;case 389: _LL3C3:{struct _tuple19*_tmpE7A;yyval=
Cyc_YY12(((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpE7A->f2=1,_tmpE7A)))))));}
break;case 390: _LL3C4:{struct _tuple19*_tmpE7B;yyval=Cyc_YY12(((_tmpE7B=_cycalloc(
sizeof(*_tmpE7B)),((_tmpE7B->f1=0,((_tmpE7B->f2=1,_tmpE7B)))))));}break;case 391:
_LL3C5:{struct Cyc_List_List*_tmpE7C;yyval=Cyc_YY13(((_tmpE7C=_cycalloc(sizeof(*
_tmpE7C)),((_tmpE7C->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpE7C->tl=0,_tmpE7C)))))));}break;case 392: _LL3C6:{struct Cyc_List_List*
_tmpE7D;yyval=Cyc_YY13(((_tmpE7D=_cycalloc(sizeof(*_tmpE7D)),((_tmpE7D->hd=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpE7D->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE7D)))))));}
break;case 393: _LL3C7:{struct _tuple20*_tmpE7E;yyval=Cyc_YY14(((_tmpE7E=_cycalloc(
sizeof(*_tmpE7E)),((_tmpE7E->f1=0,((_tmpE7E->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE7E)))))));}break;case
394: _LL3C8:{struct _tuple20*_tmpE7F;yyval=Cyc_YY14(((_tmpE7F=_cycalloc(sizeof(*
_tmpE7F)),((_tmpE7F->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpE7F->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE7F)))))));}break;case
395: _LL3C9:{struct _tuple19*_tmpE80;yyval=Cyc_YY16(((_tmpE80=_cycalloc(sizeof(*
_tmpE80)),((_tmpE80->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpE80->f2=0,_tmpE80)))))));}break;case 396: _LL3CA:{struct _tuple19*_tmpE81;yyval=
Cyc_YY16(((_tmpE81=_cycalloc(sizeof(*_tmpE81)),((_tmpE81->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpE81->f2=1,_tmpE81)))))));}
break;case 397: _LL3CB:{struct _tuple19*_tmpE82;yyval=Cyc_YY16(((_tmpE82=_cycalloc(
sizeof(*_tmpE82)),((_tmpE82->f1=0,((_tmpE82->f2=1,_tmpE82)))))));}break;case 398:
_LL3CC:{struct Cyc_List_List*_tmpE83;yyval=Cyc_YY15(((_tmpE83=_cycalloc(sizeof(*
_tmpE83)),((_tmpE83->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpE83->tl=0,_tmpE83)))))));}break;case 399: _LL3CD:{struct Cyc_List_List*
_tmpE84;yyval=Cyc_YY15(((_tmpE84=_cycalloc(sizeof(*_tmpE84)),((_tmpE84->hd=Cyc_yyget_YY14(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpE84->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE84)))))));}
break;case 400: _LL3CE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 401: _LL3CF: yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 402:
_LL3D0: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
403: _LL3D1: yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 404:
_LL3D2: yyval=Cyc_YY3(Cyc_Absyn_swap_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 405:
_LL3D3: yyval=Cyc_YY8(0);break;case 406: _LL3D4:{struct Cyc_Core_Opt*_tmpE85;yyval=
Cyc_YY8(((_tmpE85=_cycalloc_atomic(sizeof(*_tmpE85)),((_tmpE85->v=(void*)Cyc_Absyn_Times,
_tmpE85)))));}break;case 407: _LL3D5:{struct Cyc_Core_Opt*_tmpE86;yyval=Cyc_YY8(((
_tmpE86=_cycalloc_atomic(sizeof(*_tmpE86)),((_tmpE86->v=(void*)Cyc_Absyn_Div,
_tmpE86)))));}break;case 408: _LL3D6:{struct Cyc_Core_Opt*_tmpE87;yyval=Cyc_YY8(((
_tmpE87=_cycalloc_atomic(sizeof(*_tmpE87)),((_tmpE87->v=(void*)Cyc_Absyn_Mod,
_tmpE87)))));}break;case 409: _LL3D7:{struct Cyc_Core_Opt*_tmpE88;yyval=Cyc_YY8(((
_tmpE88=_cycalloc_atomic(sizeof(*_tmpE88)),((_tmpE88->v=(void*)Cyc_Absyn_Plus,
_tmpE88)))));}break;case 410: _LL3D8:{struct Cyc_Core_Opt*_tmpE89;yyval=Cyc_YY8(((
_tmpE89=_cycalloc_atomic(sizeof(*_tmpE89)),((_tmpE89->v=(void*)Cyc_Absyn_Minus,
_tmpE89)))));}break;case 411: _LL3D9:{struct Cyc_Core_Opt*_tmpE8A;yyval=Cyc_YY8(((
_tmpE8A=_cycalloc_atomic(sizeof(*_tmpE8A)),((_tmpE8A->v=(void*)Cyc_Absyn_Bitlshift,
_tmpE8A)))));}break;case 412: _LL3DA:{struct Cyc_Core_Opt*_tmpE8B;yyval=Cyc_YY8(((
_tmpE8B=_cycalloc_atomic(sizeof(*_tmpE8B)),((_tmpE8B->v=(void*)Cyc_Absyn_Bitlrshift,
_tmpE8B)))));}break;case 413: _LL3DB:{struct Cyc_Core_Opt*_tmpE8C;yyval=Cyc_YY8(((
_tmpE8C=_cycalloc_atomic(sizeof(*_tmpE8C)),((_tmpE8C->v=(void*)Cyc_Absyn_Bitand,
_tmpE8C)))));}break;case 414: _LL3DC:{struct Cyc_Core_Opt*_tmpE8D;yyval=Cyc_YY8(((
_tmpE8D=_cycalloc_atomic(sizeof(*_tmpE8D)),((_tmpE8D->v=(void*)Cyc_Absyn_Bitxor,
_tmpE8D)))));}break;case 415: _LL3DD:{struct Cyc_Core_Opt*_tmpE8E;yyval=Cyc_YY8(((
_tmpE8E=_cycalloc_atomic(sizeof(*_tmpE8E)),((_tmpE8E->v=(void*)Cyc_Absyn_Bitor,
_tmpE8E)))));}break;case 416: _LL3DE: yyval=yyvs[_check_known_subscript_notnull(
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
453: _LL403: yyval=Cyc_YY3(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 454: _LL404: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 455: _LL405: yyval=
Cyc_YY3(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
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
_LL40B: yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 462:
_LL40C: yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 463:
_LL40D:{struct Cyc_Absyn_StructField_struct _tmpE94;struct _dyneither_ptr*_tmpE93;
struct Cyc_Absyn_StructField_struct*_tmpE92;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpE92=_cycalloc(sizeof(*_tmpE92)),((_tmpE92[0]=((_tmpE94.tag=0,((
_tmpE94.f1=((_tmpE93=_cycalloc(sizeof(*_tmpE93)),((_tmpE93[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE93)))),_tmpE94)))),
_tmpE92)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 464: _LL40E:{struct Cyc_Absyn_TupleIndex_struct
_tmpE97;struct Cyc_Absyn_TupleIndex_struct*_tmpE96;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpE96=_cycalloc_atomic(sizeof(*_tmpE96)),((_tmpE96[0]=((_tmpE97.tag=1,((
_tmpE97.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2,_tmpE97)))),_tmpE96)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 465:
_LL40F:{struct Cyc_Absyn_Malloc_e_struct _tmpE9D;struct Cyc_Absyn_MallocInfo _tmpE9C;
struct Cyc_Absyn_Malloc_e_struct*_tmpE9B;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpE9B=_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B[0]=((_tmpE9D.tag=34,((_tmpE9D.f1=((
_tmpE9C.is_calloc=0,((_tmpE9C.rgn=0,((_tmpE9C.elt_type=0,((_tmpE9C.num_elts=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE9C.fat_result=
0,_tmpE9C)))))))))),_tmpE9D)))),_tmpE9B)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 466:
_LL410:{struct Cyc_Absyn_Malloc_e_struct _tmpEA3;struct Cyc_Absyn_MallocInfo _tmpEA2;
struct Cyc_Absyn_Malloc_e_struct*_tmpEA1;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpEA1=_cycalloc(sizeof(*_tmpEA1)),((_tmpEA1[0]=((_tmpEA3.tag=34,((_tmpEA3.f1=((
_tmpEA2.is_calloc=0,((_tmpEA2.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpEA2.elt_type=0,((
_tmpEA2.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpEA2.fat_result=0,_tmpEA2)))))))))),_tmpEA3)))),_tmpEA1)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 467: _LL411: {void*_tmp7F1;struct _tuple8
_tmp7F0=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp7F1=_tmp7F0.f3;{struct Cyc_Absyn_Malloc_e_struct _tmpEAD;void**_tmpEAC;
struct Cyc_Absyn_MallocInfo _tmpEAB;struct Cyc_Absyn_Malloc_e_struct*_tmpEAA;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA[
0]=((_tmpEAD.tag=34,((_tmpEAD.f1=((_tmpEAB.is_calloc=1,((_tmpEAB.rgn=0,((_tmpEAB.elt_type=((
_tmpEAC=_cycalloc(sizeof(*_tmpEAC)),((_tmpEAC[0]=_tmp7F1,_tmpEAC)))),((_tmpEAB.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((
_tmpEAB.fat_result=0,_tmpEAB)))))))))),_tmpEAD)))),_tmpEAA)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
468: _LL412: {void*_tmp7F7;struct _tuple8 _tmp7F6=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7F7=_tmp7F6.f3;{
struct Cyc_Absyn_Malloc_e_struct _tmpEB7;void**_tmpEB6;struct Cyc_Absyn_MallocInfo
_tmpEB5;struct Cyc_Absyn_Malloc_e_struct*_tmpEB4;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpEB4=_cycalloc(sizeof(*_tmpEB4)),((_tmpEB4[0]=((_tmpEB7.tag=34,((
_tmpEB7.f1=((_tmpEB5.is_calloc=1,((_tmpEB5.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),((_tmpEB5.elt_type=((
_tmpEB6=_cycalloc(sizeof(*_tmpEB6)),((_tmpEB6[0]=_tmp7F7,_tmpEB6)))),((_tmpEB5.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((
_tmpEB5.fat_result=0,_tmpEB5)))))))))),_tmpEB7)))),_tmpEB4)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
469: _LL413:{struct Cyc_Absyn_Exp*_tmpEB8[1];yyval=Cyc_YY3(Cyc_Absyn_primop_exp(
Cyc_Absyn_Numelts,((_tmpEB8[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpEB8,sizeof(struct Cyc_Absyn_Exp*),1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 470:
_LL414:{struct Cyc_Absyn_Tagcheck_e_struct _tmpEBE;struct _dyneither_ptr*_tmpEBD;
struct Cyc_Absyn_Tagcheck_e_struct*_tmpEBC;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC[0]=((_tmpEBE.tag=38,((_tmpEBE.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpEBE.f2=((_tmpEBD=_cycalloc(sizeof(*_tmpEBD)),((_tmpEBD[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpEBD)))),_tmpEBE)))))),
_tmpEBC)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 471: _LL415:{struct Cyc_Absyn_Tagcheck_e_struct
_tmpEC4;struct _dyneither_ptr*_tmpEC3;struct Cyc_Absyn_Tagcheck_e_struct*_tmpEC2;
yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((
_tmpEC2[0]=((_tmpEC4.tag=38,((_tmpEC4.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)),((_tmpEC4.f2=((
_tmpEC3=_cycalloc(sizeof(*_tmpEC3)),((_tmpEC3[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpEC3)))),_tmpEC4)))))),
_tmpEC2)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 472: _LL416: {void*_tmp804;struct _tuple8
_tmp803=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp804=_tmp803.f3;yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(_tmp804,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 473:
_LL417: {int _tmp806;struct _dyneither_ptr _tmp807;struct _tuple17 _tmp805=Cyc_yyget_Asm_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp806=_tmp805.f1;
_tmp807=_tmp805.f2;yyval=Cyc_YY3(Cyc_Absyn_asm_exp(_tmp806,_tmp807,Cyc_Position_segment_of_abs((
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
_LL41F:{struct _dyneither_ptr*_tmpEC5;yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpEC5=_cycalloc(
sizeof(*_tmpEC5)),((_tmpEC5[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEC5)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 482:
_LL420:{struct _dyneither_ptr*_tmpEC6;yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpEC6=_cycalloc(
sizeof(*_tmpEC6)),((_tmpEC6[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEC6)))),Cyc_Position_segment_of_abs((
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
_LL423:{struct Cyc_Absyn_CompoundLit_e_struct _tmpEC9;struct Cyc_Absyn_CompoundLit_e_struct*
_tmpEC8;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEC8=_cycalloc(sizeof(*
_tmpEC8)),((_tmpEC8[0]=((_tmpEC9.tag=26,((_tmpEC9.f1=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpEC9.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmpEC9)))))),_tmpEC8)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 486: _LL424:{struct Cyc_Absyn_CompoundLit_e_struct
_tmpECC;struct Cyc_Absyn_CompoundLit_e_struct*_tmpECB;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpECB=_cycalloc(sizeof(*_tmpECB)),((_tmpECB[0]=((_tmpECC.tag=26,((
_tmpECC.f1=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 5)]),((_tmpECC.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),
_tmpECC)))))),_tmpECB)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 487:
_LL425:{struct Cyc_Absyn_UnknownId_e_struct _tmpECF;struct Cyc_Absyn_UnknownId_e_struct*
_tmpECE;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpECE=_cycalloc(sizeof(*
_tmpECE)),((_tmpECE[0]=((_tmpECF.tag=2,((_tmpECF.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpECF)))),_tmpECE)))),Cyc_Position_segment_of_abs((
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
_LL42C:{struct Cyc_Absyn_Aggregate_e_struct _tmpED2;struct Cyc_Absyn_Aggregate_e_struct*
_tmpED1;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpED1=_cycalloc(sizeof(*
_tmpED1)),((_tmpED1[0]=((_tmpED2.tag=29,((_tmpED2.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpED2.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpED2.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmpED2.f4=0,
_tmpED2)))))))))),_tmpED1)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 495:
_LL42D: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 496:
_LL42E: yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));break;
case 497: _LL42F:{struct Cyc_List_List*_tmpED3;yyval=Cyc_YY5(((_tmpED3=_cycalloc(
sizeof(*_tmpED3)),((_tmpED3->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpED3->tl=0,_tmpED3)))))));}
break;case 498: _LL430:{struct Cyc_List_List*_tmpED4;yyval=Cyc_YY5(((_tmpED4=
_cycalloc(sizeof(*_tmpED4)),((_tmpED4->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpED4->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpED4)))))));}
break;case 499: _LL431: yyval=Cyc_YY3(Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
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
_LL435:{struct _dyneither_ptr*_tmpED7;struct _tuple1*_tmpED6;yyval=Cyc_QualId_tok(((
_tmpED6=_cycalloc(sizeof(*_tmpED6)),((_tmpED6->f1=Cyc_Absyn_Rel_n(0),((_tmpED6->f2=((
_tmpED7=_cycalloc(sizeof(*_tmpED7)),((_tmpED7[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpED7)))),_tmpED6)))))));}
break;case 504: _LL436: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 505: _LL437:{struct _dyneither_ptr*_tmpEDA;struct _tuple1*
_tmpED9;yyval=Cyc_QualId_tok(((_tmpED9=_cycalloc(sizeof(*_tmpED9)),((_tmpED9->f1=
Cyc_Absyn_Rel_n(0),((_tmpED9->f2=((_tmpEDA=_cycalloc(sizeof(*_tmpEDA)),((_tmpEDA[
0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpEDA)))),_tmpED9)))))));}break;case 506: _LL438: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 507: _LL439: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 508: _LL43A:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 509:
_LL43B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
510: _LL43C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 511: _LL43D: break;case 512: _LL43E: yylex_buf->lex_curr_pos -=1;break;default:
_LL43F: break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
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
_check_known_subscript_notnull(287,x)])+ 15,count ++);}{unsigned int _tmpEDF;
unsigned int _tmpEDE;struct _dyneither_ptr _tmpEDD;char*_tmpEDC;unsigned int _tmpEDB;
msg=((_tmpEDB=(unsigned int)(sze + 15),((_tmpEDC=(char*)_region_malloc(yyregion,
_check_times(sizeof(char),_tmpEDB + 1)),((_tmpEDD=_tag_dyneither(_tmpEDC,sizeof(
char),_tmpEDB + 1),((((_tmpEDE=_tmpEDB,((_tmpEE0(& _tmpEDF,& _tmpEDE,& _tmpEDC),
_tmpEDC[_tmpEDE]=(char)0)))),_tmpEDD))))))));}{const char*_tmpEE1;Cyc_strcpy(msg,((
_tmpEE1="parse error",_tag_dyneither(_tmpEE1,sizeof(char),12))));}if(count < 5){
count=0;for(x=yyn < 0?- yyn: 0;x < 287 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6245,x + yyn)]== x){{const char*_tmpEE3;const char*
_tmpEE2;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)((
_tmpEE3=", expecting `",_tag_dyneither(_tmpEE3,sizeof(char),14))):(struct
_dyneither_ptr)((_tmpEE2=" or `",_tag_dyneither(_tmpEE2,sizeof(char),6)))));}Cyc_strcat(
msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(287,x)]);{
const char*_tmpEE4;Cyc_strcat(msg,((_tmpEE4="'",_tag_dyneither(_tmpEE4,sizeof(
char),2))));}++ count;}}}Cyc_yyerror((struct _dyneither_ptr)msg);}else{const char*
_tmpEE5;Cyc_yyerror(((_tmpEE5="parse error",_tag_dyneither(_tmpEE5,sizeof(char),
12))));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int
_tmp821=1;_npop_handler(0);return _tmp821;}Cyc_yychar=- 2;}yyerrstatus=3;goto
yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp822=1;
_npop_handler(0);return _tmp822;}-- yyvsp_offset;yystate=(int)yyss[
_check_known_subscript_notnull(10000,-- yyssp_offset)];-- yylsp_offset;yyerrhandle:
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1028,yystate)];if(yyn == -
32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6244) || Cyc_yycheck[
_check_known_subscript_notnull(6245,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6245,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 1027){int
_tmp823=0;_npop_handler(0);return _tmp823;}yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE _tmp82A=v;struct _tuple6 _tmp82B;int
_tmp82C;char _tmp82D;struct _dyneither_ptr _tmp82E;struct _tuple1*_tmp82F;struct
_tuple1 _tmp830;union Cyc_Absyn_Nmspace _tmp831;struct _dyneither_ptr*_tmp832;_LL442:
if((_tmp82A.Int_tok).tag != 1)goto _LL444;_tmp82B=(struct _tuple6)(_tmp82A.Int_tok).val;
_tmp82C=_tmp82B.f2;_LL443:{const char*_tmpEE9;void*_tmpEE8[1];struct Cyc_Int_pa_struct
_tmpEE7;(_tmpEE7.tag=1,((_tmpEE7.f1=(unsigned long)_tmp82C,((_tmpEE8[0]=& _tmpEE7,
Cyc_fprintf(Cyc_stderr,((_tmpEE9="%d",_tag_dyneither(_tmpEE9,sizeof(char),3))),
_tag_dyneither(_tmpEE8,sizeof(void*),1)))))));}goto _LL441;_LL444: if((_tmp82A.Char_tok).tag
!= 2)goto _LL446;_tmp82D=(char)(_tmp82A.Char_tok).val;_LL445:{const char*_tmpEED;
void*_tmpEEC[1];struct Cyc_Int_pa_struct _tmpEEB;(_tmpEEB.tag=1,((_tmpEEB.f1=(
unsigned long)((int)_tmp82D),((_tmpEEC[0]=& _tmpEEB,Cyc_fprintf(Cyc_stderr,((
_tmpEED="%c",_tag_dyneither(_tmpEED,sizeof(char),3))),_tag_dyneither(_tmpEEC,
sizeof(void*),1)))))));}goto _LL441;_LL446: if((_tmp82A.String_tok).tag != 3)goto
_LL448;_tmp82E=(struct _dyneither_ptr)(_tmp82A.String_tok).val;_LL447:{const char*
_tmpEF1;void*_tmpEF0[1];struct Cyc_String_pa_struct _tmpEEF;(_tmpEEF.tag=0,((
_tmpEEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp82E),((_tmpEF0[0]=&
_tmpEEF,Cyc_fprintf(Cyc_stderr,((_tmpEF1="\"%s\"",_tag_dyneither(_tmpEF1,sizeof(
char),5))),_tag_dyneither(_tmpEF0,sizeof(void*),1)))))));}goto _LL441;_LL448: if((
_tmp82A.QualId_tok).tag != 5)goto _LL44A;_tmp82F=(struct _tuple1*)(_tmp82A.QualId_tok).val;
_tmp830=*_tmp82F;_tmp831=_tmp830.f1;_tmp832=_tmp830.f2;_LL449: {struct Cyc_List_List*
_tmp83C=0;{union Cyc_Absyn_Nmspace _tmp83D=_tmp831;struct Cyc_List_List*_tmp83E;
struct Cyc_List_List*_tmp83F;struct Cyc_List_List*_tmp840;int _tmp841;_LL44D: if((
_tmp83D.Rel_n).tag != 1)goto _LL44F;_tmp83E=(struct Cyc_List_List*)(_tmp83D.Rel_n).val;
_LL44E: _tmp83C=_tmp83E;goto _LL44C;_LL44F: if((_tmp83D.Abs_n).tag != 2)goto _LL451;
_tmp83F=(struct Cyc_List_List*)(_tmp83D.Abs_n).val;_LL450: _tmp83C=_tmp83F;goto
_LL44C;_LL451: if((_tmp83D.C_n).tag != 3)goto _LL453;_tmp840=(struct Cyc_List_List*)(
_tmp83D.C_n).val;_LL452: _tmp83C=_tmp840;goto _LL44C;_LL453: if((_tmp83D.Loc_n).tag
!= 4)goto _LL44C;_tmp841=(int)(_tmp83D.Loc_n).val;_LL454: goto _LL44C;_LL44C:;}for(
0;_tmp83C != 0;_tmp83C=_tmp83C->tl){const char*_tmpEF5;void*_tmpEF4[1];struct Cyc_String_pa_struct
_tmpEF3;(_tmpEF3.tag=0,((_tmpEF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp83C->hd)),((_tmpEF4[0]=& _tmpEF3,Cyc_fprintf(Cyc_stderr,((
_tmpEF5="%s::",_tag_dyneither(_tmpEF5,sizeof(char),5))),_tag_dyneither(_tmpEF4,
sizeof(void*),1)))))));}{const char*_tmpEF9;void*_tmpEF8[1];struct Cyc_String_pa_struct
_tmpEF7;(_tmpEF7.tag=0,((_tmpEF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp832),((_tmpEF8[0]=& _tmpEF7,Cyc_fprintf(Cyc_stderr,((_tmpEF9="%s::",
_tag_dyneither(_tmpEF9,sizeof(char),5))),_tag_dyneither(_tmpEF8,sizeof(void*),1)))))));}
goto _LL441;}_LL44A:;_LL44B:{const char*_tmpEFC;void*_tmpEFB;(_tmpEFB=0,Cyc_fprintf(
Cyc_stderr,((_tmpEFC="?",_tag_dyneither(_tmpEFC,sizeof(char),2))),_tag_dyneither(
_tmpEFB,sizeof(void*),0)));}goto _LL441;_LL441:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
