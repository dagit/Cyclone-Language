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
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};typedef void*Cyc_parg_t;struct _dyneither_ptr Cyc_aprintf(struct
_dyneither_ptr,struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};typedef void*Cyc_sarg_t;struct _dyneither_ptr Cyc_vrprintf(
struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};typedef
unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
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
int lex_eof_reached;};typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};typedef
struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct
Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr
lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct
_dyneither_ptr lex_check;};typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};typedef struct Cyc_List_List*Cyc_List_list_t;
typedef struct Cyc_List_List*Cyc_List_List_t;struct Cyc_List_List*Cyc_List_list(
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
int line_no;int col;};typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_struct{char*tag;};typedef unsigned int Cyc_Position_seg_t;
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct
_dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
typedef struct Cyc_Position_Error*Cyc_Position_error_t;struct Cyc_Position_Error*
Cyc_Position_mk_err_parse(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[
10];struct Cyc_Position_Nocontext_struct{char*tag;};void Cyc_Position_post_error(
struct Cyc_Position_Error*);typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
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
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{
unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{
void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;
union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{
void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};
struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{
int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{
int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};typedef void*Cyc_Absyn_pointer_qual_t;typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;
struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*
type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct Cyc_Core_Opt*
effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct
_tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int
tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
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
zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Enum_td_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct
Cyc_Absyn_Datatype_td_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};typedef
void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};
struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
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
};struct Cyc_Absyn_RefCntRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{int
tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_struct Cyc_Absyn_RefCntRgn_val;extern struct Cyc_Absyn_VoidType_struct
Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};typedef void*Cyc_Absyn_funcparams_t;enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft
 = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};
struct Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{int
tag;};struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
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
int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_struct{
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
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*
r;struct Cyc_Core_Opt*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct
Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_struct{
int tag;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
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
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};extern struct Cyc_Absyn_Porton_d_struct
Cyc_Absyn_Porton_d_val;extern struct Cyc_Absyn_Portoff_d_struct Cyc_Absyn_Portoff_d_val;
struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_is_qvar_qualified(struct _tuple1*);struct Cyc_Absyn_Tqual
Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(unsigned int);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(
void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();extern union Cyc_Absyn_Constraint*
Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_array_typ(void*
elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,unsigned int ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,
int,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);struct
Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(
struct _dyneither_ptr s,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(
enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp(void*,void*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct
Cyc_Absyn_Stmt*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct
_dyneither_ptr body,unsigned int);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*
e,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*
s,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);struct Cyc_Absyn_Pat*
Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,unsigned int loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*
p,struct Cyc_Absyn_Exp*e,unsigned int loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,unsigned int loc);struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs,int tagged);struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum 
Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple1*n,struct Cyc_List_List*ts,
struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);struct
Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple1*n,
struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct
_tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,
unsigned int loc);void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*
eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,
struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter
Cyc_Iter_iter_t;int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_Dict_T;typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};typedef
struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_RgnOrder_RgnPO;typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,unsigned int);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int
Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*
tl;};typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;typedef const struct Cyc_Tcenv_CList*
const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
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
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_compress(void*t);extern struct
Cyc_Absyn_Kind Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct
Cyc_Absyn_Kind Cyc_Tcutil_bk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;extern struct
Cyc_Absyn_Kind Cyc_Tcutil_ek;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;extern struct
Cyc_Absyn_Kind Cyc_Tcutil_trk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;extern
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);void Cyc_Tcutil_check_no_qual(
unsigned int loc,void*t);struct Cyc_PP_Ppstate;typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;
struct Cyc_PP_Out;typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;typedef
struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int
qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int
print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};void Cyc_Lex_register_typedef(struct _tuple1*s);void Cyc_Lex_enter_namespace(
struct _dyneither_ptr*);void Cyc_Lex_leave_namespace();void Cyc_Lex_enter_using(
struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=0;struct Cyc_Parse_Signed_spec_struct{
int tag;unsigned int f1;};struct Cyc_Parse_Unsigned_spec_struct{int tag;unsigned int
f1;};struct Cyc_Parse_Short_spec_struct{int tag;unsigned int f1;};struct Cyc_Parse_Long_spec_struct{
int tag;unsigned int f1;};struct Cyc_Parse_Type_spec_struct{int tag;void*f1;
unsigned int f2;};typedef void*Cyc_Parse_type_specifier_t;enum Cyc_Parse_Storage_class{
Cyc_Parse_Typedef_sc  = 0,Cyc_Parse_Extern_sc  = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc
 = 3,Cyc_Parse_Auto_sc  = 4,Cyc_Parse_Register_sc  = 5,Cyc_Parse_Abstract_sc  = 6};
typedef enum Cyc_Parse_Storage_class Cyc_Parse_storage_class_t;struct Cyc_Parse_Declaration_spec{
struct Cyc_Core_Opt*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int
is_inline;struct Cyc_List_List*attributes;};typedef struct Cyc_Parse_Declaration_spec*
Cyc_Parse_decl_spec_t;struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*
tms;};typedef struct Cyc_Parse_Declarator*Cyc_Parse_declarator_t;struct Cyc_Parse_Abstractdeclarator{
struct Cyc_List_List*tms;};typedef struct Cyc_Parse_Abstractdeclarator*Cyc_Parse_abstractdeclarator_t;
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,
unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;};static struct _tuple11 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,
void*,struct Cyc_List_List*,struct Cyc_List_List*);static struct Cyc_List_List*Cyc_Parse_parse_result=
0;static void Cyc_Parse_err(struct _dyneither_ptr msg,unsigned int sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(unsigned int sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Parse_err((struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);{struct Cyc_Position_Exit_struct
_tmp889;struct Cyc_Position_Exit_struct*_tmp888;(int)_throw((void*)((_tmp888=
_cycalloc_atomic(sizeof(*_tmp888)),((_tmp888[0]=((_tmp889.tag=Cyc_Position_Exit,
_tmp889)),_tmp888)))));};}static void Cyc_Parse_unimp(struct _dyneither_ptr msg,
unsigned int sg){{const char*_tmp88E;void*_tmp88D[2];struct Cyc_String_pa_struct
_tmp88C;struct Cyc_String_pa_struct _tmp88B;(_tmp88B.tag=0,((_tmp88B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp88C.tag=0,((_tmp88C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(sg)),((
_tmp88D[0]=& _tmp88C,((_tmp88D[1]=& _tmp88B,Cyc_fprintf(Cyc_stderr,((_tmp88E="%s: Warning: Cyclone does not yet support %s\n",
_tag_dyneither(_tmp88E,sizeof(char),46))),_tag_dyneither(_tmp88D,sizeof(void*),2)))))))))))));}
return;}static void*Cyc_Parse_type_name_to_type(struct _tuple9*tqt,unsigned int loc){
struct _tuple9 _tmp7;struct Cyc_Absyn_Tqual _tmp8;void*_tmp9;struct _tuple9*_tmp6=tqt;
_tmp7=*_tmp6;_tmp8=_tmp7.f2;_tmp9=_tmp7.f3;if((_tmp8.print_const  || _tmp8.q_volatile)
 || _tmp8.q_restrict){if(_tmp8.loc != 0)loc=_tmp8.loc;{const char*_tmp891;void*
_tmp890;(_tmp890=0,Cyc_Tcutil_warn(loc,((_tmp891="qualifier on type is ignored",
_tag_dyneither(_tmp891,sizeof(char),29))),_tag_dyneither(_tmp890,sizeof(void*),0)));};}
return _tmp9;}struct _tuple12{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*
f2;union Cyc_Absyn_Constraint*f3;void*f4;};static struct _tuple12 Cyc_Parse_collapse_pointer_quals(
unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*
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
_tmp894;struct Cyc_Absyn_Upper_b_struct*_tmp893;bound=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp893=_cycalloc(sizeof(*_tmp893)),((
_tmp893[0]=((_tmp894.tag=1,((_tmp894.f1=_tmp14,_tmp894)))),_tmp893)))));}goto
_LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_struct*_tmp15=(struct Cyc_Absyn_Region_ptrqual_struct*)
_tmpC;if(_tmp15->tag != 1)goto _LL0;else{_tmp16=(void*)_tmp15->f1;}}_LL10: rgn=
_tmp16;goto _LL0;_LL0:;}{struct _tuple12 _tmp895;return(_tmp895.f1=nullable,((
_tmp895.f2=bound,((_tmp895.f3=zeroterm,((_tmp895.f4=rgn,_tmp895)))))));};}struct
_tuple1*Cyc_Parse_gensym_enum(){static int enum_counter=0;struct _dyneither_ptr*
_tmp8A2;const char*_tmp8A1;void*_tmp8A0[1];struct Cyc_Int_pa_struct _tmp89F;struct
_tuple1*_tmp89E;return(_tmp89E=_cycalloc(sizeof(*_tmp89E)),((_tmp89E->f1=Cyc_Absyn_Rel_n(
0),((_tmp89E->f2=((_tmp8A2=_cycalloc(sizeof(*_tmp8A2)),((_tmp8A2[0]=(struct
_dyneither_ptr)((_tmp89F.tag=1,((_tmp89F.f1=(unsigned long)enum_counter ++,((
_tmp8A0[0]=& _tmp89F,Cyc_aprintf(((_tmp8A1="__anonymous_enum_%d__",_tag_dyneither(
_tmp8A1,sizeof(char),22))),_tag_dyneither(_tmp8A0,sizeof(void*),1)))))))),
_tmp8A2)))),_tmp89E)))));}struct _tuple13{struct _tuple1*f1;struct Cyc_Absyn_Tqual
f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple14{struct
_tuple13*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
unsigned int loc,struct _tuple14*field_info){struct _tuple14 _tmp20;struct _tuple13*
_tmp21;struct _tuple13 _tmp22;struct _tuple1*_tmp23;struct Cyc_Absyn_Tqual _tmp24;
void*_tmp25;struct Cyc_List_List*_tmp26;struct Cyc_List_List*_tmp27;struct Cyc_Absyn_Exp*
_tmp28;struct _tuple14*_tmp1F=field_info;_tmp20=*_tmp1F;_tmp21=_tmp20.f1;_tmp22=*
_tmp21;_tmp23=_tmp22.f1;_tmp24=_tmp22.f2;_tmp25=_tmp22.f3;_tmp26=_tmp22.f4;
_tmp27=_tmp22.f5;_tmp28=_tmp20.f2;if(_tmp26 != 0){const char*_tmp8A3;Cyc_Parse_err(((
_tmp8A3="bad type params in struct field",_tag_dyneither(_tmp8A3,sizeof(char),32))),
loc);}if(Cyc_Absyn_is_qvar_qualified(_tmp23)){const char*_tmp8A4;Cyc_Parse_err(((
_tmp8A4="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp8A4,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp8A5;return(_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5->name=(*_tmp23).f2,((
_tmp8A5->tq=_tmp24,((_tmp8A5->type=_tmp25,((_tmp8A5->width=_tmp28,((_tmp8A5->attributes=
_tmp27,_tmp8A5)))))))))));};}static void*Cyc_Parse_type_spec(void*t,unsigned int
loc){struct Cyc_Parse_Type_spec_struct _tmp8A8;struct Cyc_Parse_Type_spec_struct*
_tmp8A7;return(void*)((_tmp8A7=_cycalloc(sizeof(*_tmp8A7)),((_tmp8A7[0]=((
_tmp8A8.tag=4,((_tmp8A8.f1=(void*)t,((_tmp8A8.f2=loc,_tmp8A8)))))),_tmp8A7))));}
static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp2E=t;struct Cyc_Absyn_ArrayInfo
_tmp30;void*_tmp31;struct Cyc_Absyn_Tqual _tmp32;struct Cyc_Absyn_Exp*_tmp33;union
Cyc_Absyn_Constraint*_tmp34;unsigned int _tmp35;_LL12: {struct Cyc_Absyn_ArrayType_struct*
_tmp2F=(struct Cyc_Absyn_ArrayType_struct*)_tmp2E;if(_tmp2F->tag != 8)goto _LL14;
else{_tmp30=_tmp2F->f1;_tmp31=_tmp30.elt_type;_tmp32=_tmp30.tq;_tmp33=_tmp30.num_elts;
_tmp34=_tmp30.zero_term;_tmp35=_tmp30.zt_loc;}}_LL13: {struct Cyc_Absyn_Upper_b_struct
_tmp8AB;struct Cyc_Absyn_Upper_b_struct*_tmp8AA;return Cyc_Absyn_starb_typ(_tmp31,
argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,
_tmp32,_tmp33 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((void*)((_tmp8AA=
_cycalloc(sizeof(*_tmp8AA)),((_tmp8AA[0]=((_tmp8AB.tag=1,((_tmp8AB.f1=(struct Cyc_Absyn_Exp*)
_tmp33,_tmp8AB)))),_tmp8AA))))),_tmp34);}_LL14:;_LL15: return t;_LL11:;}struct
_tuple15{struct _dyneither_ptr*f1;void*f2;};static struct Cyc_List_List*Cyc_Parse_get_arg_tags(
struct Cyc_List_List*x){struct Cyc_List_List*_tmp38=0;for(0;x != 0;x=x->tl){struct
_tuple9*_tmp39=(struct _tuple9*)x->hd;struct _tuple9 _tmp3A;struct Cyc_Core_Opt*
_tmp3B;struct Cyc_Core_Opt _tmp3C;struct _dyneither_ptr*_tmp3D;void*_tmp3E;void*
_tmp40;struct _tuple9 _tmp41;struct Cyc_Core_Opt*_tmp42;struct Cyc_Core_Opt _tmp43;
struct _dyneither_ptr*_tmp44;void*_tmp45;void*_tmp47;struct Cyc_Core_Opt*_tmp49;
struct Cyc_Core_Opt**_tmp4A;_LL17: _tmp3A=*_tmp39;_tmp3B=_tmp3A.f1;if(_tmp3B == 0)
goto _LL19;_tmp3C=*_tmp3B;_tmp3D=(struct _dyneither_ptr*)_tmp3C.v;_tmp3E=_tmp3A.f3;{
struct Cyc_Absyn_TagType_struct*_tmp3F=(struct Cyc_Absyn_TagType_struct*)_tmp3E;
if(_tmp3F->tag != 19)goto _LL19;else{_tmp40=(void*)_tmp3F->f1;}};_LL18:{void*
_tmp4B=_tmp40;struct Cyc_Core_Opt*_tmp4D;struct Cyc_Core_Opt**_tmp4E;_LL1E: {
struct Cyc_Absyn_Evar_struct*_tmp4C=(struct Cyc_Absyn_Evar_struct*)_tmp4B;if(
_tmp4C->tag != 1)goto _LL20;else{_tmp4D=_tmp4C->f2;_tmp4E=(struct Cyc_Core_Opt**)&
_tmp4C->f2;}}_LL1F: {struct Cyc_String_pa_struct _tmp8B3;void*_tmp8B2[1];const char*
_tmp8B1;struct _dyneither_ptr*_tmp8B0;struct _dyneither_ptr*nm=(_tmp8B0=_cycalloc(
sizeof(*_tmp8B0)),((_tmp8B0[0]=(struct _dyneither_ptr)((_tmp8B3.tag=0,((_tmp8B3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3D),((_tmp8B2[0]=& _tmp8B3,Cyc_aprintf(((
_tmp8B1="`%s",_tag_dyneither(_tmp8B1,sizeof(char),4))),_tag_dyneither(_tmp8B2,
sizeof(void*),1)))))))),_tmp8B0)));{struct Cyc_Absyn_VarType_struct*_tmp8C8;
struct Cyc_Absyn_Tvar*_tmp8C7;struct Cyc_Absyn_Eq_kb_struct _tmp8C6;struct Cyc_Absyn_Eq_kb_struct*
_tmp8C5;struct Cyc_Absyn_VarType_struct _tmp8C4;struct Cyc_Core_Opt*_tmp8C3;*_tmp4E=((
_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3->v=(void*)((_tmp8C8=_cycalloc(
sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8C4.tag=2,((_tmp8C4.f1=((_tmp8C7=_cycalloc(
sizeof(*_tmp8C7)),((_tmp8C7->name=nm,((_tmp8C7->identity=- 1,((_tmp8C7->kind=(
void*)((_tmp8C5=_cycalloc(sizeof(*_tmp8C5)),((_tmp8C5[0]=((_tmp8C6.tag=0,((
_tmp8C6.f1=& Cyc_Tcutil_ik,_tmp8C6)))),_tmp8C5)))),_tmp8C7)))))))),_tmp8C4)))),
_tmp8C8)))),_tmp8C3))));}goto _LL1D;}_LL20:;_LL21: goto _LL1D;_LL1D:;}{struct
_tuple15*_tmp8CB;struct Cyc_List_List*_tmp8CA;_tmp38=((_tmp8CA=_cycalloc(sizeof(*
_tmp8CA)),((_tmp8CA->hd=((_tmp8CB=_cycalloc(sizeof(*_tmp8CB)),((_tmp8CB->f1=
_tmp3D,((_tmp8CB->f2=_tmp40,_tmp8CB)))))),((_tmp8CA->tl=_tmp38,_tmp8CA))))));}
goto _LL16;_LL19: _tmp41=*_tmp39;_tmp42=_tmp41.f1;if(_tmp42 == 0)goto _LL1B;_tmp43=*
_tmp42;_tmp44=(struct _dyneither_ptr*)_tmp43.v;_tmp45=_tmp41.f3;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp46=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp45;if(_tmp46->tag != 15)goto
_LL1B;else{_tmp47=(void*)_tmp46->f1;{struct Cyc_Absyn_Evar_struct*_tmp48=(struct
Cyc_Absyn_Evar_struct*)_tmp47;if(_tmp48->tag != 1)goto _LL1B;else{_tmp49=_tmp48->f2;
_tmp4A=(struct Cyc_Core_Opt**)& _tmp48->f2;}};}};_LL1A: {struct Cyc_String_pa_struct
_tmp8D3;void*_tmp8D2[1];const char*_tmp8D1;struct _dyneither_ptr*_tmp8D0;struct
_dyneither_ptr*nm=(_tmp8D0=_cycalloc(sizeof(*_tmp8D0)),((_tmp8D0[0]=(struct
_dyneither_ptr)((_tmp8D3.tag=0,((_tmp8D3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp44),((_tmp8D2[0]=& _tmp8D3,Cyc_aprintf(((_tmp8D1="`%s",
_tag_dyneither(_tmp8D1,sizeof(char),4))),_tag_dyneither(_tmp8D2,sizeof(void*),1)))))))),
_tmp8D0)));{struct Cyc_Absyn_VarType_struct*_tmp8E8;struct Cyc_Absyn_Tvar*_tmp8E7;
struct Cyc_Absyn_Eq_kb_struct _tmp8E6;struct Cyc_Absyn_Eq_kb_struct*_tmp8E5;struct
Cyc_Absyn_VarType_struct _tmp8E4;struct Cyc_Core_Opt*_tmp8E3;*_tmp4A=((_tmp8E3=
_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3->v=(void*)((_tmp8E8=_cycalloc(sizeof(*
_tmp8E8)),((_tmp8E8[0]=((_tmp8E4.tag=2,((_tmp8E4.f1=((_tmp8E7=_cycalloc(sizeof(*
_tmp8E7)),((_tmp8E7->name=nm,((_tmp8E7->identity=- 1,((_tmp8E7->kind=(void*)((
_tmp8E5=_cycalloc(sizeof(*_tmp8E5)),((_tmp8E5[0]=((_tmp8E6.tag=0,((_tmp8E6.f1=&
Cyc_Tcutil_rk,_tmp8E6)))),_tmp8E5)))),_tmp8E7)))))))),_tmp8E4)))),_tmp8E8)))),
_tmp8E3))));}goto _LL16;}_LL1B:;_LL1C: goto _LL16;_LL16:;}return _tmp38;}static
struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){struct
Cyc_List_List*_tmp65=0;for(0;x != 0;x=x->tl){void*_tmp66=((struct Cyc_Absyn_Aggrfield*)
x->hd)->type;void*_tmp68;_LL23: {struct Cyc_Absyn_TagType_struct*_tmp67=(struct
Cyc_Absyn_TagType_struct*)_tmp66;if(_tmp67->tag != 19)goto _LL25;else{_tmp68=(void*)
_tmp67->f1;}}_LL24:{struct _tuple15*_tmp8EB;struct Cyc_List_List*_tmp8EA;_tmp65=((
_tmp8EA=_cycalloc(sizeof(*_tmp8EA)),((_tmp8EA->hd=((_tmp8EB=_cycalloc(sizeof(*
_tmp8EB)),((_tmp8EB->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8EB->f2=
_tmp68,_tmp8EB)))))),((_tmp8EA->tl=_tmp65,_tmp8EA))))));}goto _LL22;_LL25:;_LL26:
goto _LL22;_LL22:;}return _tmp65;}static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(
struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){{void*_tmp6B=e->r;struct _tuple1*
_tmp6D;struct _tuple1 _tmp6E;union Cyc_Absyn_Nmspace _tmp6F;struct Cyc_List_List*
_tmp70;struct _dyneither_ptr*_tmp71;struct _tuple1*_tmp73;struct _tuple1 _tmp74;
union Cyc_Absyn_Nmspace _tmp75;struct Cyc_List_List*_tmp76;struct _dyneither_ptr*
_tmp77;_LL28: {struct Cyc_Absyn_Var_e_struct*_tmp6C=(struct Cyc_Absyn_Var_e_struct*)
_tmp6B;if(_tmp6C->tag != 1)goto _LL2A;else{_tmp6D=_tmp6C->f1;_tmp6E=*_tmp6D;_tmp6F=
_tmp6E.f1;if((_tmp6F.Rel_n).tag != 1)goto _LL2A;_tmp70=(struct Cyc_List_List*)(
_tmp6F.Rel_n).val;if(_tmp70 != 0)goto _LL2A;_tmp71=_tmp6E.f2;}}_LL29: _tmp77=_tmp71;
goto _LL2B;_LL2A: {struct Cyc_Absyn_UnknownId_e_struct*_tmp72=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp6B;if(_tmp72->tag != 2)goto _LL2C;else{_tmp73=_tmp72->f1;_tmp74=*_tmp73;_tmp75=
_tmp74.f1;if((_tmp75.Rel_n).tag != 1)goto _LL2C;_tmp76=(struct Cyc_List_List*)(
_tmp75.Rel_n).val;if(_tmp76 != 0)goto _LL2C;_tmp77=_tmp74.f2;}}_LL2B:{struct Cyc_List_List*
_tmp78=tags;for(0;_tmp78 != 0;_tmp78=_tmp78->tl){struct _tuple15 _tmp7A;struct
_dyneither_ptr*_tmp7B;void*_tmp7C;struct _tuple15*_tmp79=(struct _tuple15*)_tmp78->hd;
_tmp7A=*_tmp79;_tmp7B=_tmp7A.f1;_tmp7C=_tmp7A.f2;if(Cyc_strptrcmp(_tmp7B,_tmp77)
== 0){struct Cyc_Absyn_Valueof_e_struct _tmp8EE;struct Cyc_Absyn_Valueof_e_struct*
_tmp8ED;return Cyc_Absyn_new_exp((void*)((_tmp8ED=_cycalloc(sizeof(*_tmp8ED)),((
_tmp8ED[0]=((_tmp8EE.tag=39,((_tmp8EE.f1=(void*)Cyc_Tcutil_copy_type(_tmp7C),
_tmp8EE)))),_tmp8ED)))),e->loc);}}}goto _LL27;_LL2C:;_LL2D: goto _LL27;_LL27:;}
return e;}static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){{
void*_tmp7F=t;struct Cyc_Absyn_ArrayInfo _tmp81;void*_tmp82;struct Cyc_Absyn_Tqual
_tmp83;struct Cyc_Absyn_Exp*_tmp84;union Cyc_Absyn_Constraint*_tmp85;unsigned int
_tmp86;struct Cyc_Absyn_PtrInfo _tmp88;void*_tmp89;struct Cyc_Absyn_Tqual _tmp8A;
struct Cyc_Absyn_PtrAtts _tmp8B;void*_tmp8C;union Cyc_Absyn_Constraint*_tmp8D;union
Cyc_Absyn_Constraint*_tmp8E;union Cyc_Absyn_Constraint*_tmp8F;struct Cyc_Absyn_PtrLoc*
_tmp90;_LL2F: {struct Cyc_Absyn_ArrayType_struct*_tmp80=(struct Cyc_Absyn_ArrayType_struct*)
_tmp7F;if(_tmp80->tag != 8)goto _LL31;else{_tmp81=_tmp80->f1;_tmp82=_tmp81.elt_type;
_tmp83=_tmp81.tq;_tmp84=_tmp81.num_elts;_tmp85=_tmp81.zero_term;_tmp86=_tmp81.zt_loc;}}
_LL30: {struct Cyc_Absyn_Exp*nelts2=_tmp84;if(_tmp84 != 0)nelts2=(struct Cyc_Absyn_Exp*)
Cyc_Parse_substitute_tags_exp(tags,(struct Cyc_Absyn_Exp*)_tmp84);{void*_tmp91=
Cyc_Parse_substitute_tags(tags,_tmp82);if(_tmp84 != nelts2  || _tmp82 != _tmp91){
struct Cyc_Absyn_ArrayType_struct _tmp8F4;struct Cyc_Absyn_ArrayInfo _tmp8F3;struct
Cyc_Absyn_ArrayType_struct*_tmp8F2;return(void*)((_tmp8F2=_cycalloc(sizeof(*
_tmp8F2)),((_tmp8F2[0]=((_tmp8F4.tag=8,((_tmp8F4.f1=((_tmp8F3.elt_type=_tmp91,((
_tmp8F3.tq=_tmp83,((_tmp8F3.num_elts=nelts2,((_tmp8F3.zero_term=_tmp85,((_tmp8F3.zt_loc=
_tmp86,_tmp8F3)))))))))),_tmp8F4)))),_tmp8F2))));}goto _LL2E;};}_LL31: {struct Cyc_Absyn_PointerType_struct*
_tmp87=(struct Cyc_Absyn_PointerType_struct*)_tmp7F;if(_tmp87->tag != 5)goto _LL33;
else{_tmp88=_tmp87->f1;_tmp89=_tmp88.elt_typ;_tmp8A=_tmp88.elt_tq;_tmp8B=_tmp88.ptr_atts;
_tmp8C=_tmp8B.rgn;_tmp8D=_tmp8B.nullable;_tmp8E=_tmp8B.bounds;_tmp8F=_tmp8B.zero_term;
_tmp90=_tmp8B.ptrloc;}}_LL32: {void*_tmp95=Cyc_Parse_substitute_tags(tags,_tmp89);
union Cyc_Absyn_Constraint*_tmp96=_tmp8E;{union Cyc_Absyn_Constraint _tmp97=*_tmp8E;
void*_tmp98;struct Cyc_Absyn_Exp*_tmp9A;_LL36: if((_tmp97.Eq_constr).tag != 1)goto
_LL38;_tmp98=(void*)(_tmp97.Eq_constr).val;{struct Cyc_Absyn_Upper_b_struct*
_tmp99=(struct Cyc_Absyn_Upper_b_struct*)_tmp98;if(_tmp99->tag != 1)goto _LL38;
else{_tmp9A=_tmp99->f1;}};_LL37: _tmp9A=Cyc_Parse_substitute_tags_exp(tags,_tmp9A);{
struct Cyc_Absyn_Upper_b_struct*_tmp8FA;struct Cyc_Absyn_Upper_b_struct _tmp8F9;
union Cyc_Absyn_Constraint*_tmp8F8;_tmp96=((_tmp8F8=_cycalloc(sizeof(*_tmp8F8)),(((
_tmp8F8->Eq_constr).val=(void*)((_tmp8FA=_cycalloc(sizeof(*_tmp8FA)),((_tmp8FA[0]=((
_tmp8F9.tag=1,((_tmp8F9.f1=_tmp9A,_tmp8F9)))),_tmp8FA)))),(((_tmp8F8->Eq_constr).tag=
1,_tmp8F8))))));}goto _LL35;_LL38:;_LL39: goto _LL35;_LL35:;}if(_tmp95 != _tmp89  || 
_tmp96 != _tmp8E){struct Cyc_Absyn_PointerType_struct _tmp904;struct Cyc_Absyn_PtrAtts
_tmp903;struct Cyc_Absyn_PtrInfo _tmp902;struct Cyc_Absyn_PointerType_struct*
_tmp901;return(void*)((_tmp901=_cycalloc(sizeof(*_tmp901)),((_tmp901[0]=((
_tmp904.tag=5,((_tmp904.f1=((_tmp902.elt_typ=_tmp95,((_tmp902.elt_tq=_tmp8A,((
_tmp902.ptr_atts=((_tmp903.rgn=_tmp8C,((_tmp903.nullable=_tmp8D,((_tmp903.bounds=
_tmp96,((_tmp903.zero_term=_tmp8F,((_tmp903.ptrloc=_tmp90,_tmp903)))))))))),
_tmp902)))))),_tmp904)))),_tmp901))));}goto _LL2E;}_LL33:;_LL34: goto _LL2E;_LL2E:;}
return t;}static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,
struct Cyc_Absyn_Aggrfield*x){x->type=Cyc_Parse_substitute_tags(tags,x->type);}
struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple16*Cyc_Parse_get_tqual_typ(
unsigned int loc,struct _tuple9*t){struct _tuple16*_tmp905;return(_tmp905=_cycalloc(
sizeof(*_tmp905)),((_tmp905->f1=(*t).f2,((_tmp905->f2=(*t).f3,_tmp905)))));}
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmpA3=d->r;struct Cyc_Absyn_Vardecl*_tmpA5;_LL3B: {struct Cyc_Absyn_Var_d_struct*
_tmpA4=(struct Cyc_Absyn_Var_d_struct*)_tmpA3;if(_tmpA4->tag != 0)goto _LL3D;else{
_tmpA5=_tmpA4->f1;}}_LL3C: return _tmpA5;_LL3D:;_LL3E: {const char*_tmp908;void*
_tmp907;(_tmp907=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((_tmp908="bad declaration in `forarray' statement",
_tag_dyneither(_tmp908,sizeof(char),40))),_tag_dyneither(_tmp907,sizeof(void*),0)));}
_LL3A:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmpA8=tm;_LL40: {struct
Cyc_Absyn_TypeParams_mod_struct*_tmpA9=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpA8;if(_tmpA9->tag != 4)goto _LL42;}_LL41: return 1;_LL42:;_LL43: return 0;_LL3F:;}
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){const char*_tmp909;if(
Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp909="`H",_tag_dyneither(_tmp909,
sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_HeapRgn_val;else{const char*
_tmp90A;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp90A="`U",_tag_dyneither(
_tmp90A,sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_UniqueRgn_val;else{const
char*_tmp90B;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp90B="`RC",
_tag_dyneither(_tmp90B,sizeof(char),4))))== 0)return(void*)& Cyc_Absyn_RefCntRgn_val;
else{struct Cyc_Absyn_VarType_struct _tmp915;struct _dyneither_ptr*_tmp914;struct
Cyc_Absyn_Tvar*_tmp913;struct Cyc_Absyn_VarType_struct*_tmp912;return(void*)((
_tmp912=_cycalloc(sizeof(*_tmp912)),((_tmp912[0]=((_tmp915.tag=2,((_tmp915.f1=((
_tmp913=_cycalloc(sizeof(*_tmp913)),((_tmp913->name=((_tmp914=_cycalloc(sizeof(*
_tmp914)),((_tmp914[0]=s,_tmp914)))),((_tmp913->identity=- 1,((_tmp913->kind=k,
_tmp913)))))))),_tmp915)))),_tmp912))));}}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar*t){void*k;{void*_tmpB1=Cyc_Absyn_compress_kb(t->kind);
struct Cyc_Absyn_Kind*_tmpB4;_LL45: {struct Cyc_Absyn_Unknown_kb_struct*_tmpB2=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpB1;if(_tmpB2->tag != 1)goto _LL47;}_LL46:{
struct Cyc_Absyn_Unknown_kb_struct _tmp918;struct Cyc_Absyn_Unknown_kb_struct*
_tmp917;k=(void*)((_tmp917=_cycalloc(sizeof(*_tmp917)),((_tmp917[0]=((_tmp918.tag=
1,((_tmp918.f1=0,_tmp918)))),_tmp917))));}goto _LL44;_LL47: {struct Cyc_Absyn_Less_kb_struct*
_tmpB3=(struct Cyc_Absyn_Less_kb_struct*)_tmpB1;if(_tmpB3->tag != 2)goto _LL49;
else{_tmpB4=_tmpB3->f2;}}_LL48:{struct Cyc_Absyn_Less_kb_struct _tmp91B;struct Cyc_Absyn_Less_kb_struct*
_tmp91A;k=(void*)((_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp91B.tag=
2,((_tmp91B.f1=0,((_tmp91B.f2=_tmpB4,_tmp91B)))))),_tmp91A))));}goto _LL44;_LL49:;
_LL4A: k=_tmpB1;goto _LL44;_LL44:;}{struct Cyc_Absyn_Tvar*_tmp91C;return(_tmp91C=
_cycalloc(sizeof(*_tmp91C)),((_tmp91C->name=t->name,((_tmp91C->identity=- 1,((
_tmp91C->kind=k,_tmp91C)))))));};}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(
unsigned int loc,void*t){void*_tmpBA=t;struct Cyc_Absyn_Tvar*_tmpBC;_LL4C: {struct
Cyc_Absyn_VarType_struct*_tmpBB=(struct Cyc_Absyn_VarType_struct*)_tmpBA;if(
_tmpBB->tag != 2)goto _LL4E;else{_tmpBC=_tmpBB->f1;}}_LL4D: return _tmpBC;_LL4E:;
_LL4F: {const char*_tmp91F;void*_tmp91E;(_tmp91E=0,((int(*)(unsigned int sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp91F="expecting a list of type variables, not types",
_tag_dyneither(_tmp91F,sizeof(char),46))),_tag_dyneither(_tmp91E,sizeof(void*),0)));}
_LL4B:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct
_tmp922;struct Cyc_Absyn_VarType_struct*_tmp921;return(void*)((_tmp921=_cycalloc(
sizeof(*_tmp921)),((_tmp921[0]=((_tmp922.tag=2,((_tmp922.f1=pr,_tmp922)))),
_tmp921))));}static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,
int leq){void*_tmpC1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpC3;struct Cyc_Absyn_Tvar
_tmpC4;void*_tmpC5;void**_tmpC6;_LL51: {struct Cyc_Absyn_VarType_struct*_tmpC2=(
struct Cyc_Absyn_VarType_struct*)_tmpC1;if(_tmpC2->tag != 2)goto _LL53;else{_tmpC3=
_tmpC2->f1;_tmpC4=*_tmpC3;_tmpC5=_tmpC4.kind;_tmpC6=(void**)&(*_tmpC2->f1).kind;}}
_LL52: {void*_tmpC7=Cyc_Absyn_compress_kb(*_tmpC6);_LL56: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpC8=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC7;if(_tmpC8->tag != 1)goto _LL58;}
_LL57: if(!leq)*_tmpC6=Cyc_Tcutil_kind_to_bound(k);else{struct Cyc_Absyn_Less_kb_struct
_tmp925;struct Cyc_Absyn_Less_kb_struct*_tmp924;*_tmpC6=(void*)((_tmp924=
_cycalloc(sizeof(*_tmp924)),((_tmp924[0]=((_tmp925.tag=2,((_tmp925.f1=0,((
_tmp925.f2=k,_tmp925)))))),_tmp924))));}return;_LL58:;_LL59: return;_LL55:;}_LL53:;
_LL54: return;_LL50:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){if(tds == 0)
return tms;if(tms == 0)return 0;{void*_tmpCB=(void*)tms->hd;void*_tmpCD;_LL5B: {
struct Cyc_Absyn_Function_mod_struct*_tmpCC=(struct Cyc_Absyn_Function_mod_struct*)
_tmpCB;if(_tmpCC->tag != 3)goto _LL5D;else{_tmpCD=(void*)_tmpCC->f1;}}_LL5C: if(tms->tl
== 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)
 && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){void*_tmpCE=_tmpCD;
struct Cyc_List_List*_tmpD1;_LL60: {struct Cyc_Absyn_WithTypes_struct*_tmpCF=(
struct Cyc_Absyn_WithTypes_struct*)_tmpCE;if(_tmpCF->tag != 1)goto _LL62;}_LL61:{
const char*_tmp928;void*_tmp927;(_tmp927=0,Cyc_Tcutil_warn(loc,((_tmp928="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp928,sizeof(char),93))),_tag_dyneither(_tmp927,sizeof(void*),0)));}
return tms;_LL62: {struct Cyc_Absyn_NoTypes_struct*_tmpD0=(struct Cyc_Absyn_NoTypes_struct*)
_tmpCE;if(_tmpD0->tag != 0)goto _LL5F;else{_tmpD1=_tmpD0->f1;}}_LL63: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpD1)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds)){const char*_tmp92B;void*_tmp92A;(_tmp92A=0,((int(*)(
unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
loc,((_tmp92B="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp92B,sizeof(char),73))),_tag_dyneither(_tmp92A,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpD6=0;for(0;_tmpD1 != 0;_tmpD1=_tmpD1->tl){struct Cyc_List_List*
_tmpD7=tds;for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){struct Cyc_Absyn_Decl*_tmpD8=(
struct Cyc_Absyn_Decl*)_tmpD7->hd;void*_tmpD9=_tmpD8->r;struct Cyc_Absyn_Vardecl*
_tmpDB;_LL65: {struct Cyc_Absyn_Var_d_struct*_tmpDA=(struct Cyc_Absyn_Var_d_struct*)
_tmpD9;if(_tmpDA->tag != 0)goto _LL67;else{_tmpDB=_tmpDA->f1;}}_LL66: if(Cyc_zstrptrcmp((*
_tmpDB->name).f2,(struct _dyneither_ptr*)_tmpD1->hd)!= 0)continue;if(_tmpDB->initializer
!= 0){const char*_tmp92E;void*_tmp92D;(_tmp92D=0,((int(*)(unsigned int sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD8->loc,((_tmp92E="initializer found in parameter declaration",
_tag_dyneither(_tmp92E,sizeof(char),43))),_tag_dyneither(_tmp92D,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpDB->name)){const char*_tmp931;void*_tmp930;(
_tmp930=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Parse_abort)(_tmpD8->loc,((_tmp931="namespaces forbidden in parameter declarations",
_tag_dyneither(_tmp931,sizeof(char),47))),_tag_dyneither(_tmp930,sizeof(void*),0)));}{
struct _tuple9*_tmp937;struct Cyc_Core_Opt*_tmp936;struct Cyc_List_List*_tmp935;
_tmpD6=((_tmp935=_cycalloc(sizeof(*_tmp935)),((_tmp935->hd=((_tmp937=_cycalloc(
sizeof(*_tmp937)),((_tmp937->f1=((_tmp936=_cycalloc(sizeof(*_tmp936)),((_tmp936->v=(*
_tmpDB->name).f2,_tmp936)))),((_tmp937->f2=_tmpDB->tq,((_tmp937->f3=_tmpDB->type,
_tmp937)))))))),((_tmp935->tl=_tmpD6,_tmp935))))));}goto L;_LL67:;_LL68: {const
char*_tmp93A;void*_tmp939;(_tmp939=0,((int(*)(unsigned int sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD8->loc,((_tmp93A="nonvariable declaration in parameter type",
_tag_dyneither(_tmp93A,sizeof(char),42))),_tag_dyneither(_tmp939,sizeof(void*),0)));}
_LL64:;}L: if(_tmpD7 == 0){const char*_tmp93E;void*_tmp93D[1];struct Cyc_String_pa_struct
_tmp93C;(_tmp93C.tag=0,((_tmp93C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmpD1->hd)),((_tmp93D[0]=& _tmp93C,((int(*)(unsigned int sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp93E="%s is not given a type",
_tag_dyneither(_tmp93E,sizeof(char),23))),_tag_dyneither(_tmp93D,sizeof(void*),1)))))));}}{
struct Cyc_Absyn_Function_mod_struct*_tmp94D;struct Cyc_Absyn_WithTypes_struct*
_tmp94C;struct Cyc_Absyn_WithTypes_struct _tmp94B;struct Cyc_Absyn_Function_mod_struct
_tmp94A;struct Cyc_List_List*_tmp949;return(_tmp949=_cycalloc(sizeof(*_tmp949)),((
_tmp949->hd=(void*)((_tmp94D=_cycalloc(sizeof(*_tmp94D)),((_tmp94D[0]=((_tmp94A.tag=
3,((_tmp94A.f1=(void*)((void*)((_tmp94C=_cycalloc(sizeof(*_tmp94C)),((_tmp94C[0]=((
_tmp94B.tag=1,((_tmp94B.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpD6),((_tmp94B.f2=0,((_tmp94B.f3=0,((_tmp94B.f4=0,((_tmp94B.f5=0,_tmp94B)))))))))))),
_tmp94C))))),_tmp94A)))),_tmp94D)))),((_tmp949->tl=0,_tmp949)))));};};_LL5F:;}
goto _LL5E;_LL5D:;_LL5E: {struct Cyc_List_List*_tmp94E;return(_tmp94E=_cycalloc(
sizeof(*_tmp94E)),((_tmp94E->hd=(void*)tms->hd,((_tmp94E->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp94E)))));}_LL5A:;};}struct _tuple17{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,unsigned int loc){if(tds != 0){struct Cyc_Parse_Declarator*
_tmp94F;d=((_tmp94F=_cycalloc(sizeof(*_tmp94F)),((_tmp94F->id=d->id,((_tmp94F->tms=
Cyc_Parse_oldstyle2newstyle(d->tms,tds,loc),_tmp94F))))));}{enum Cyc_Absyn_Scope
sc=Cyc_Absyn_Public;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(
0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0)
switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v){case Cyc_Parse_Extern_sc: _LL69: sc=
Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL6A: sc=Cyc_Absyn_Static;break;
default: _LL6B:{const char*_tmp950;Cyc_Parse_err(((_tmp950="bad storage class on function",
_tag_dyneither(_tmp950,sizeof(char),30))),loc);}break;}}{void*_tmpF0=Cyc_Parse_collapse_type_specifiers(
tss,loc);struct Cyc_Absyn_Tqual _tmpF2;void*_tmpF3;struct Cyc_List_List*_tmpF4;
struct Cyc_List_List*_tmpF5;struct _tuple11 _tmpF1=Cyc_Parse_apply_tms(tq,_tmpF0,
atts,d->tms);_tmpF2=_tmpF1.f1;_tmpF3=_tmpF1.f2;_tmpF4=_tmpF1.f3;_tmpF5=_tmpF1.f4;
if(_tmpF4 != 0){const char*_tmp953;void*_tmp952;(_tmp952=0,Cyc_Tcutil_warn(loc,((
_tmp953="bad type params, ignoring",_tag_dyneither(_tmp953,sizeof(char),26))),
_tag_dyneither(_tmp952,sizeof(void*),0)));}{void*_tmpF8=_tmpF3;struct Cyc_Absyn_FnInfo
_tmpFA;struct Cyc_List_List*_tmpFB;struct Cyc_Core_Opt*_tmpFC;struct Cyc_Absyn_Tqual
_tmpFD;void*_tmpFE;struct Cyc_List_List*_tmpFF;int _tmp100;struct Cyc_Absyn_VarargInfo*
_tmp101;struct Cyc_List_List*_tmp102;struct Cyc_List_List*_tmp103;_LL6E: {struct
Cyc_Absyn_FnType_struct*_tmpF9=(struct Cyc_Absyn_FnType_struct*)_tmpF8;if(_tmpF9->tag
!= 9)goto _LL70;else{_tmpFA=_tmpF9->f1;_tmpFB=_tmpFA.tvars;_tmpFC=_tmpFA.effect;
_tmpFD=_tmpFA.ret_tqual;_tmpFE=_tmpFA.ret_typ;_tmpFF=_tmpFA.args;_tmp100=_tmpFA.c_varargs;
_tmp101=_tmpFA.cyc_varargs;_tmp102=_tmpFA.rgn_po;_tmp103=_tmpFA.attributes;}}
_LL6F: {struct Cyc_List_List*_tmp104=0;{struct Cyc_List_List*_tmp105=_tmpFF;for(0;
_tmp105 != 0;_tmp105=_tmp105->tl){struct _tuple9 _tmp107;struct Cyc_Core_Opt*_tmp108;
struct Cyc_Absyn_Tqual _tmp109;void*_tmp10A;struct _tuple9*_tmp106=(struct _tuple9*)
_tmp105->hd;_tmp107=*_tmp106;_tmp108=_tmp107.f1;_tmp109=_tmp107.f2;_tmp10A=
_tmp107.f3;if(_tmp108 == 0){{const char*_tmp954;Cyc_Parse_err(((_tmp954="missing argument variable in function prototype",
_tag_dyneither(_tmp954,sizeof(char),48))),loc);}{struct _tuple17*_tmp957;struct
Cyc_List_List*_tmp956;_tmp104=((_tmp956=_cycalloc(sizeof(*_tmp956)),((_tmp956->hd=((
_tmp957=_cycalloc(sizeof(*_tmp957)),((_tmp957->f1=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp957->f2=_tmp109,((
_tmp957->f3=_tmp10A,_tmp957)))))))),((_tmp956->tl=_tmp104,_tmp956))))));};}else{
struct _tuple17*_tmp95A;struct Cyc_List_List*_tmp959;_tmp104=((_tmp959=_cycalloc(
sizeof(*_tmp959)),((_tmp959->hd=((_tmp95A=_cycalloc(sizeof(*_tmp95A)),((_tmp95A->f1=(
struct _dyneither_ptr*)_tmp108->v,((_tmp95A->f2=_tmp109,((_tmp95A->f3=_tmp10A,
_tmp95A)))))))),((_tmp959->tl=_tmp104,_tmp959))))));}}}{struct Cyc_Absyn_Fndecl*
_tmp95B;return(_tmp95B=_cycalloc(sizeof(*_tmp95B)),((_tmp95B->sc=sc,((_tmp95B->name=
d->id,((_tmp95B->tvs=_tmpFB,((_tmp95B->is_inline=is_inline,((_tmp95B->effect=
_tmpFC,((_tmp95B->ret_tqual=_tmpFD,((_tmp95B->ret_type=_tmpFE,((_tmp95B->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp104),((
_tmp95B->c_varargs=_tmp100,((_tmp95B->cyc_varargs=_tmp101,((_tmp95B->rgn_po=
_tmp102,((_tmp95B->body=body,((_tmp95B->cached_typ=0,((_tmp95B->param_vardecls=0,((
_tmp95B->fn_vardecl=0,((_tmp95B->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp103,_tmpF5),_tmp95B)))))))))))))))))))))))))))))))));};}
_LL70:;_LL71: {const char*_tmp95E;void*_tmp95D;(_tmp95D=0,((int(*)(unsigned int sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp95E="declarator is not a function prototype",
_tag_dyneither(_tmp95E,sizeof(char),39))),_tag_dyneither(_tmp95D,sizeof(void*),0)));}
_LL6D:;};};};}static char _tmp114[52]="at most one type may appear within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg1={_tmp114,_tmp114,_tmp114 + 52};static
char _tmp115[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp115,_tmp115,_tmp115 + 63};static
char _tmp116[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp116,_tmp116,_tmp116 + 50};static char _tmp117[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp117,_tmp117,_tmp117 + 60};static
void*Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,unsigned int loc){
int seen_type=0;int seen_sign=0;int seen_size=0;void*t=(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
unsigned int last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp118=(void*)ts->hd;void*
_tmp11A;unsigned int _tmp11B;unsigned int _tmp11D;unsigned int _tmp11F;unsigned int
_tmp121;unsigned int _tmp123;_LL73: {struct Cyc_Parse_Type_spec_struct*_tmp119=(
struct Cyc_Parse_Type_spec_struct*)_tmp118;if(_tmp119->tag != 4)goto _LL75;else{
_tmp11A=(void*)_tmp119->f1;_tmp11B=_tmp119->f2;}}_LL74: if(seen_type)Cyc_Parse_err(
Cyc_Parse_msg1,_tmp11B);last_loc=_tmp11B;seen_type=1;t=_tmp11A;goto _LL72;_LL75: {
struct Cyc_Parse_Signed_spec_struct*_tmp11C=(struct Cyc_Parse_Signed_spec_struct*)
_tmp118;if(_tmp11C->tag != 0)goto _LL77;else{_tmp11D=_tmp11C->f1;}}_LL76: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp11D);if(seen_type){const char*_tmp95F;
Cyc_Parse_err(((_tmp95F="signed qualifier must come before type",_tag_dyneither(
_tmp95F,sizeof(char),39))),_tmp11D);}last_loc=_tmp11D;seen_sign=1;sgn=Cyc_Absyn_Signed;
goto _LL72;_LL77: {struct Cyc_Parse_Unsigned_spec_struct*_tmp11E=(struct Cyc_Parse_Unsigned_spec_struct*)
_tmp118;if(_tmp11E->tag != 1)goto _LL79;else{_tmp11F=_tmp11E->f1;}}_LL78: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp11F);if(seen_type){const char*_tmp960;
Cyc_Parse_err(((_tmp960="signed qualifier must come before type",_tag_dyneither(
_tmp960,sizeof(char),39))),_tmp11F);}last_loc=_tmp11F;seen_sign=1;sgn=Cyc_Absyn_Unsigned;
goto _LL72;_LL79: {struct Cyc_Parse_Short_spec_struct*_tmp120=(struct Cyc_Parse_Short_spec_struct*)
_tmp118;if(_tmp120->tag != 2)goto _LL7B;else{_tmp121=_tmp120->f1;}}_LL7A: if(
seen_size){const char*_tmp961;Cyc_Parse_err(((_tmp961="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp961,sizeof(char),59))),_tmp121);}if(seen_type){const char*
_tmp962;Cyc_Parse_err(((_tmp962="short modifier must come before base type",
_tag_dyneither(_tmp962,sizeof(char),42))),_tmp121);}last_loc=_tmp121;seen_size=1;
sz=Cyc_Absyn_Short_sz;goto _LL72;_LL7B: {struct Cyc_Parse_Long_spec_struct*_tmp122=(
struct Cyc_Parse_Long_spec_struct*)_tmp118;if(_tmp122->tag != 3)goto _LL72;else{
_tmp123=_tmp122->f1;}}_LL7C: if(seen_type){const char*_tmp963;Cyc_Parse_err(((
_tmp963="long modifier must come before base type",_tag_dyneither(_tmp963,
sizeof(char),41))),_tmp123);}if(seen_size)switch(sz){case Cyc_Absyn_Long_sz: _LL7D:
sz=Cyc_Absyn_LongLong_sz;break;default: _LL7E:{const char*_tmp964;Cyc_Parse_err(((
_tmp964="extra long in type specifier",_tag_dyneither(_tmp964,sizeof(char),29))),
_tmp123);}break;}else{sz=Cyc_Absyn_Long_sz;}last_loc=_tmp123;seen_size=1;goto
_LL72;_LL72:;}if(!seen_type){if(!seen_sign  && !seen_size){const char*_tmp967;void*
_tmp966;(_tmp966=0,Cyc_Tcutil_warn(last_loc,((_tmp967="missing type within specifier",
_tag_dyneither(_tmp967,sizeof(char),30))),_tag_dyneither(_tmp966,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{if(seen_sign){void*_tmp12C=t;enum Cyc_Absyn_Sign
_tmp12E;enum Cyc_Absyn_Size_of _tmp12F;_LL81: {struct Cyc_Absyn_IntType_struct*
_tmp12D=(struct Cyc_Absyn_IntType_struct*)_tmp12C;if(_tmp12D->tag != 6)goto _LL83;
else{_tmp12E=_tmp12D->f1;_tmp12F=_tmp12D->f2;}}_LL82: if(_tmp12E != sgn)t=Cyc_Absyn_int_typ(
sgn,_tmp12F);goto _LL80;_LL83:;_LL84:{const char*_tmp968;Cyc_Parse_err(((_tmp968="sign specification on non-integral type",
_tag_dyneither(_tmp968,sizeof(char),40))),last_loc);}goto _LL80;_LL80:;}if(
seen_size){void*_tmp131=t;enum Cyc_Absyn_Sign _tmp133;enum Cyc_Absyn_Size_of
_tmp134;_LL86: {struct Cyc_Absyn_IntType_struct*_tmp132=(struct Cyc_Absyn_IntType_struct*)
_tmp131;if(_tmp132->tag != 6)goto _LL88;else{_tmp133=_tmp132->f1;_tmp134=_tmp132->f2;}}
_LL87: if(_tmp134 != sz)t=Cyc_Absyn_int_typ(_tmp133,sz);goto _LL85;_LL88: {struct
Cyc_Absyn_FloatType_struct*_tmp135=(struct Cyc_Absyn_FloatType_struct*)_tmp131;
if(_tmp135->tag != 7)goto _LL8A;}_LL89: t=Cyc_Absyn_float_typ(2);goto _LL85;_LL8A:;
_LL8B:{const char*_tmp969;Cyc_Parse_err(((_tmp969="size qualifier on non-integral type",
_tag_dyneither(_tmp969,sizeof(char),36))),last_loc);}goto _LL85;_LL85:;}}return t;}
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)
return 0;{struct Cyc_Parse_Declarator*_tmp137=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmp138=_tmp137->id;struct Cyc_Absyn_Tqual _tmp13A;void*_tmp13B;
struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13D;struct _tuple11 _tmp139=
Cyc_Parse_apply_tms(tq,t,shared_atts,_tmp137->tms);_tmp13A=_tmp139.f1;_tmp13B=
_tmp139.f2;_tmp13C=_tmp139.f3;_tmp13D=_tmp139.f4;if(ds->tl == 0){struct _tuple13*
_tmp96C;struct Cyc_List_List*_tmp96B;return(_tmp96B=_region_malloc(r,sizeof(*
_tmp96B)),((_tmp96B->hd=((_tmp96C=_region_malloc(r,sizeof(*_tmp96C)),((_tmp96C->f1=
_tmp138,((_tmp96C->f2=_tmp13A,((_tmp96C->f3=_tmp13B,((_tmp96C->f4=_tmp13C,((
_tmp96C->f5=_tmp13D,_tmp96C)))))))))))),((_tmp96B->tl=0,_tmp96B)))));}else{
struct _tuple13*_tmp96F;struct Cyc_List_List*_tmp96E;return(_tmp96E=_region_malloc(
r,sizeof(*_tmp96E)),((_tmp96E->hd=((_tmp96F=_region_malloc(r,sizeof(*_tmp96F)),((
_tmp96F->f1=_tmp138,((_tmp96F->f2=_tmp13A,((_tmp96F->f3=_tmp13B,((_tmp96F->f4=
_tmp13C,((_tmp96F->f5=_tmp13D,_tmp96F)))))))))))),((_tmp96E->tl=Cyc_Parse_apply_tmss(
r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp96E)))));}};}static struct
_tuple11 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0){struct _tuple11 _tmp970;return(_tmp970.f1=
tq,((_tmp970.f2=t,((_tmp970.f3=0,((_tmp970.f4=atts,_tmp970)))))));}{void*_tmp143=(
void*)tms->hd;union Cyc_Absyn_Constraint*_tmp145;unsigned int _tmp146;struct Cyc_Absyn_Exp*
_tmp148;union Cyc_Absyn_Constraint*_tmp149;unsigned int _tmp14A;void*_tmp14C;
struct Cyc_List_List*_tmp14E;unsigned int _tmp14F;struct Cyc_Absyn_PtrAtts _tmp151;
struct Cyc_Absyn_Tqual _tmp152;unsigned int _tmp154;struct Cyc_List_List*_tmp155;
_LL8D: {struct Cyc_Absyn_Carray_mod_struct*_tmp144=(struct Cyc_Absyn_Carray_mod_struct*)
_tmp143;if(_tmp144->tag != 0)goto _LL8F;else{_tmp145=_tmp144->f1;_tmp146=_tmp144->f2;}}
_LL8E: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,
tq,0,_tmp145,_tmp146),atts,tms->tl);_LL8F: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmp147=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmp143;if(_tmp147->tag != 1)goto
_LL91;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;_tmp14A=_tmp147->f3;}}_LL90:
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(
struct Cyc_Absyn_Exp*)_tmp148,_tmp149,_tmp14A),atts,tms->tl);_LL91: {struct Cyc_Absyn_Function_mod_struct*
_tmp14B=(struct Cyc_Absyn_Function_mod_struct*)_tmp143;if(_tmp14B->tag != 3)goto
_LL93;else{_tmp14C=(void*)_tmp14B->f1;}}_LL92: {void*_tmp156=_tmp14C;struct Cyc_List_List*
_tmp158;int _tmp159;struct Cyc_Absyn_VarargInfo*_tmp15A;struct Cyc_Core_Opt*_tmp15B;
struct Cyc_List_List*_tmp15C;unsigned int _tmp15E;_LL9A: {struct Cyc_Absyn_WithTypes_struct*
_tmp157=(struct Cyc_Absyn_WithTypes_struct*)_tmp156;if(_tmp157->tag != 1)goto _LL9C;
else{_tmp158=_tmp157->f1;_tmp159=_tmp157->f2;_tmp15A=_tmp157->f3;_tmp15B=_tmp157->f4;
_tmp15C=_tmp157->f5;}}_LL9B: {struct Cyc_List_List*typvars=0;struct Cyc_List_List*
fn_atts=0;struct Cyc_List_List*new_atts=0;{struct Cyc_List_List*as=atts;for(0;as != 
0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd)){struct Cyc_List_List*_tmp971;
fn_atts=((_tmp971=_cycalloc(sizeof(*_tmp971)),((_tmp971->hd=(void*)as->hd,((
_tmp971->tl=fn_atts,_tmp971))))));}else{struct Cyc_List_List*_tmp972;new_atts=((
_tmp972=_cycalloc(sizeof(*_tmp972)),((_tmp972->hd=(void*)as->hd,((_tmp972->tl=
new_atts,_tmp972))))));}}}if(tms->tl != 0){void*_tmp161=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmp163;_LL9F: {struct Cyc_Absyn_TypeParams_mod_struct*
_tmp162=(struct Cyc_Absyn_TypeParams_mod_struct*)_tmp161;if(_tmp162->tag != 4)goto
_LLA1;else{_tmp163=_tmp162->f1;}}_LLA0: typvars=_tmp163;tms=tms->tl;goto _LL9E;
_LLA1:;_LLA2: goto _LL9E;_LL9E:;}if(((((!_tmp159  && _tmp15A == 0) && _tmp158 != 0)
 && _tmp158->tl == 0) && (*((struct _tuple9*)_tmp158->hd)).f1 == 0) && (*((struct
_tuple9*)_tmp158->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)_tmp158=0;{struct Cyc_List_List*
_tmp164=Cyc_Parse_get_arg_tags(_tmp158);if(_tmp164 != 0)t=Cyc_Parse_substitute_tags(
_tmp164,t);t=Cyc_Parse_array2ptr(t,0);{struct Cyc_List_List*_tmp165=_tmp158;for(0;
_tmp165 != 0;_tmp165=_tmp165->tl){struct _tuple9 _tmp167;struct Cyc_Core_Opt*_tmp168;
struct Cyc_Absyn_Tqual _tmp169;void*_tmp16A;void**_tmp16B;struct _tuple9*_tmp166=(
struct _tuple9*)_tmp165->hd;_tmp167=*_tmp166;_tmp168=_tmp167.f1;_tmp169=_tmp167.f2;
_tmp16A=_tmp167.f3;_tmp16B=(void**)&(*_tmp166).f3;if(_tmp164 != 0)*_tmp16B=Cyc_Parse_substitute_tags(
_tmp164,*_tmp16B);*_tmp16B=Cyc_Parse_array2ptr(*_tmp16B,1);}}return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(tq.loc),Cyc_Absyn_function_typ(typvars,_tmp15B,tq,t,
_tmp158,_tmp159,_tmp15A,_tmp15C,fn_atts),new_atts,((struct Cyc_List_List*)
_check_null(tms))->tl);};}_LL9C: {struct Cyc_Absyn_NoTypes_struct*_tmp15D=(struct
Cyc_Absyn_NoTypes_struct*)_tmp156;if(_tmp15D->tag != 0)goto _LL99;else{_tmp15E=
_tmp15D->f2;}}_LL9D: {const char*_tmp975;void*_tmp974;(_tmp974=0,((int(*)(
unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
_tmp15E,((_tmp975="function declaration without parameter types",_tag_dyneither(
_tmp975,sizeof(char),45))),_tag_dyneither(_tmp974,sizeof(void*),0)));}_LL99:;}
_LL93: {struct Cyc_Absyn_TypeParams_mod_struct*_tmp14D=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp143;if(_tmp14D->tag != 4)goto _LL95;else{_tmp14E=_tmp14D->f1;_tmp14F=_tmp14D->f2;}}
_LL94: if(tms->tl == 0){struct _tuple11 _tmp976;return(_tmp976.f1=tq,((_tmp976.f2=t,((
_tmp976.f3=_tmp14E,((_tmp976.f4=atts,_tmp976)))))));}{const char*_tmp979;void*
_tmp978;(_tmp978=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(_tmp14F,((_tmp979="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp979,sizeof(char),68))),_tag_dyneither(_tmp978,sizeof(void*),0)));};
_LL95: {struct Cyc_Absyn_Pointer_mod_struct*_tmp150=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp143;if(_tmp150->tag != 2)goto _LL97;else{_tmp151=_tmp150->f1;_tmp152=_tmp150->f2;}}
_LL96: {struct Cyc_Absyn_PointerType_struct _tmp97F;struct Cyc_Absyn_PtrInfo _tmp97E;
struct Cyc_Absyn_PointerType_struct*_tmp97D;return Cyc_Parse_apply_tms(_tmp152,(
void*)((_tmp97D=_cycalloc(sizeof(*_tmp97D)),((_tmp97D[0]=((_tmp97F.tag=5,((
_tmp97F.f1=((_tmp97E.elt_typ=t,((_tmp97E.elt_tq=tq,((_tmp97E.ptr_atts=_tmp151,
_tmp97E)))))),_tmp97F)))),_tmp97D)))),atts,tms->tl);}_LL97: {struct Cyc_Absyn_Attributes_mod_struct*
_tmp153=(struct Cyc_Absyn_Attributes_mod_struct*)_tmp143;if(_tmp153->tag != 5)goto
_LL8C;else{_tmp154=_tmp153->f1;_tmp155=_tmp153->f2;}}_LL98: return Cyc_Parse_apply_tms(
tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
atts,_tmp155),tms->tl);_LL8C:;};}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*
tss,unsigned int loc){return Cyc_Parse_collapse_type_specifiers(tss,loc);}static
struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple13*t){
struct _tuple13 _tmp175;struct _tuple1*_tmp176;struct Cyc_Absyn_Tqual _tmp177;void*
_tmp178;struct Cyc_List_List*_tmp179;struct Cyc_List_List*_tmp17A;struct _tuple13*
_tmp174=t;_tmp175=*_tmp174;_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;_tmp178=_tmp175.f3;
_tmp179=_tmp175.f4;_tmp17A=_tmp175.f5;Cyc_Lex_register_typedef(_tmp176);{struct
Cyc_Core_Opt*kind;struct Cyc_Core_Opt*type;{void*_tmp17B=_tmp178;struct Cyc_Core_Opt*
_tmp17D;_LLA4: {struct Cyc_Absyn_Evar_struct*_tmp17C=(struct Cyc_Absyn_Evar_struct*)
_tmp17B;if(_tmp17C->tag != 1)goto _LLA6;else{_tmp17D=_tmp17C->f1;}}_LLA5: type=0;
if(_tmp17D == 0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;else{kind=_tmp17D;}goto
_LLA3;_LLA6:;_LLA7: kind=0;{struct Cyc_Core_Opt*_tmp980;type=((_tmp980=_cycalloc(
sizeof(*_tmp980)),((_tmp980->v=_tmp178,_tmp980))));}goto _LLA3;_LLA3:;}{struct Cyc_Absyn_Typedef_d_struct
_tmp986;struct Cyc_Absyn_Typedefdecl*_tmp985;struct Cyc_Absyn_Typedef_d_struct*
_tmp984;return Cyc_Absyn_new_decl((void*)((_tmp984=_cycalloc(sizeof(*_tmp984)),((
_tmp984[0]=((_tmp986.tag=9,((_tmp986.f1=((_tmp985=_cycalloc(sizeof(*_tmp985)),((
_tmp985->name=_tmp176,((_tmp985->tvs=_tmp179,((_tmp985->kind=kind,((_tmp985->defn=
type,((_tmp985->atts=_tmp17A,((_tmp985->tq=_tmp177,_tmp985)))))))))))))),_tmp986)))),
_tmp984)))),loc);};};}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Decl_s_struct _tmp989;
struct Cyc_Absyn_Decl_s_struct*_tmp988;return Cyc_Absyn_new_stmt((void*)((_tmp988=
_cycalloc(sizeof(*_tmp988)),((_tmp988[0]=((_tmp989.tag=12,((_tmp989.f1=d,((
_tmp989.f2=s,_tmp989)))))),_tmp988)))),d->loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,unsigned int tqual_loc,unsigned int loc){struct
_RegionHandle _tmp184=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp184;
_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp186;struct Cyc_Absyn_Tqual
_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_List_List*_tmp189;struct Cyc_Parse_Declaration_spec*
_tmp185=ds;_tmp186=*_tmp185;_tmp187=_tmp186.tq;_tmp188=_tmp186.type_specs;
_tmp189=_tmp186.attributes;if(_tmp187.loc == 0)_tmp187.loc=tqual_loc;if(ds->is_inline){
const char*_tmp98A;Cyc_Parse_err(((_tmp98A="inline is allowed only on function definitions",
_tag_dyneither(_tmp98A,sizeof(char),47))),loc);}if(_tmp188 == 0){{const char*
_tmp98D;void*_tmp98C;(_tmp98C=0,Cyc_Tcutil_warn(loc,((_tmp98D="missing type specifiers in declaration, assuming int",
_tag_dyneither(_tmp98D,sizeof(char),53))),_tag_dyneither(_tmp98C,sizeof(void*),0)));}{
void*_tmp98E[1];_tmp188=((_tmp98E[0]=Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,loc),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp98E,sizeof(void*),1))));};}{enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;int
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
struct Cyc_Absyn_TypeDeclType_struct*)_tmp192;if(_tmp193->tag != 26)goto _LLB3;
else{_tmp194=_tmp193->f1;_tmp195=*_tmp194;_tmp196=_tmp195.r;{struct Cyc_Absyn_Aggr_td_struct*
_tmp197=(struct Cyc_Absyn_Aggr_td_struct*)_tmp196;if(_tmp197->tag != 0)goto _LLB3;
else{_tmp198=_tmp197->f1;}};}}_LLB2: _tmp198->attributes=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp198->attributes,
_tmp189);{struct Cyc_Absyn_Aggr_d_struct*_tmp994;struct Cyc_Absyn_Aggr_d_struct
_tmp993;struct Cyc_List_List*_tmp992;struct Cyc_List_List*_tmp1BE=(_tmp992=
_cycalloc(sizeof(*_tmp992)),((_tmp992->hd=Cyc_Absyn_new_decl((void*)((_tmp994=
_cycalloc(sizeof(*_tmp994)),((_tmp994[0]=((_tmp993.tag=6,((_tmp993.f1=_tmp198,
_tmp993)))),_tmp994)))),loc),((_tmp992->tl=0,_tmp992)))));_npop_handler(0);
return _tmp1BE;};_LLB3: {struct Cyc_Absyn_TypeDeclType_struct*_tmp199=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp192;if(_tmp199->tag != 26)goto _LLB5;else{_tmp19A=_tmp199->f1;_tmp19B=*_tmp19A;
_tmp19C=_tmp19B.r;{struct Cyc_Absyn_Enum_td_struct*_tmp19D=(struct Cyc_Absyn_Enum_td_struct*)
_tmp19C;if(_tmp19D->tag != 1)goto _LLB5;else{_tmp19E=_tmp19D->f1;}};}}_LLB4: if(
_tmp189 != 0){const char*_tmp995;Cyc_Parse_err(((_tmp995="attributes on enum not supported",
_tag_dyneither(_tmp995,sizeof(char),33))),loc);}{struct Cyc_Absyn_Enum_d_struct*
_tmp99B;struct Cyc_Absyn_Enum_d_struct _tmp99A;struct Cyc_List_List*_tmp999;struct
Cyc_List_List*_tmp1C3=(_tmp999=_cycalloc(sizeof(*_tmp999)),((_tmp999->hd=Cyc_Absyn_new_decl((
void*)((_tmp99B=_cycalloc(sizeof(*_tmp99B)),((_tmp99B[0]=((_tmp99A.tag=8,((
_tmp99A.f1=_tmp19E,_tmp99A)))),_tmp99B)))),loc),((_tmp999->tl=0,_tmp999)))));
_npop_handler(0);return _tmp1C3;};_LLB5: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp19F=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp192;if(_tmp19F->tag != 26)goto
_LLB7;else{_tmp1A0=_tmp19F->f1;_tmp1A1=*_tmp1A0;_tmp1A2=_tmp1A1.r;{struct Cyc_Absyn_Datatype_td_struct*
_tmp1A3=(struct Cyc_Absyn_Datatype_td_struct*)_tmp1A2;if(_tmp1A3->tag != 2)goto
_LLB7;else{_tmp1A4=_tmp1A3->f1;}};}}_LLB6: if(_tmp189 != 0){const char*_tmp99C;Cyc_Parse_err(((
_tmp99C="attributes on datatypes not supported",_tag_dyneither(_tmp99C,sizeof(
char),38))),loc);}{struct Cyc_Absyn_Datatype_d_struct*_tmp9A2;struct Cyc_Absyn_Datatype_d_struct
_tmp9A1;struct Cyc_List_List*_tmp9A0;struct Cyc_List_List*_tmp1C8=(_tmp9A0=
_cycalloc(sizeof(*_tmp9A0)),((_tmp9A0->hd=Cyc_Absyn_new_decl((void*)((_tmp9A2=
_cycalloc(sizeof(*_tmp9A2)),((_tmp9A2[0]=((_tmp9A1.tag=7,((_tmp9A1.f1=_tmp1A4,
_tmp9A1)))),_tmp9A2)))),loc),((_tmp9A0->tl=0,_tmp9A0)))));_npop_handler(0);
return _tmp1C8;};_LLB7: {struct Cyc_Absyn_AggrType_struct*_tmp1A5=(struct Cyc_Absyn_AggrType_struct*)
_tmp192;if(_tmp1A5->tag != 11)goto _LLB9;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A6.aggr_info;
if((_tmp1A7.UnknownAggr).tag != 1)goto _LLB9;_tmp1A8=(struct _tuple3)(_tmp1A7.UnknownAggr).val;
_tmp1A9=_tmp1A8.f1;_tmp1AA=_tmp1A8.f2;_tmp1AB=_tmp1A6.targs;}}_LLB8: {struct Cyc_List_List*
_tmp1C9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),
unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,
_tmp1AB);struct Cyc_Absyn_Aggrdecl*_tmp9A3;struct Cyc_Absyn_Aggrdecl*_tmp1CA=(
_tmp9A3=_cycalloc(sizeof(*_tmp9A3)),((_tmp9A3->kind=_tmp1A9,((_tmp9A3->sc=s,((
_tmp9A3->name=_tmp1AA,((_tmp9A3->tvs=_tmp1C9,((_tmp9A3->impl=0,((_tmp9A3->attributes=
0,_tmp9A3)))))))))))));if(_tmp189 != 0){const char*_tmp9A4;Cyc_Parse_err(((_tmp9A4="bad attributes on type declaration",
_tag_dyneither(_tmp9A4,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp9AA;struct Cyc_Absyn_Aggr_d_struct _tmp9A9;struct Cyc_List_List*_tmp9A8;struct
Cyc_List_List*_tmp1CF=(_tmp9A8=_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8->hd=Cyc_Absyn_new_decl((
void*)((_tmp9AA=_cycalloc(sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9A9.tag=6,((
_tmp9A9.f1=_tmp1CA,_tmp9A9)))),_tmp9AA)))),loc),((_tmp9A8->tl=0,_tmp9A8)))));
_npop_handler(0);return _tmp1CF;};}_LLB9: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1AC=(struct Cyc_Absyn_DatatypeType_struct*)_tmp192;if(_tmp1AC->tag != 3)goto
_LLBB;else{_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AD.datatype_info;if((_tmp1AE.KnownDatatype).tag
!= 2)goto _LLBB;_tmp1AF=(struct Cyc_Absyn_Datatypedecl**)(_tmp1AE.KnownDatatype).val;}}
_LLBA: if(_tmp189 != 0){const char*_tmp9AB;Cyc_Parse_err(((_tmp9AB="bad attributes on datatype",
_tag_dyneither(_tmp9AB,sizeof(char),27))),loc);}{struct Cyc_Absyn_Datatype_d_struct*
_tmp9B1;struct Cyc_Absyn_Datatype_d_struct _tmp9B0;struct Cyc_List_List*_tmp9AF;
struct Cyc_List_List*_tmp1D5=(_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((_tmp9AF->hd=
Cyc_Absyn_new_decl((void*)((_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1[0]=((
_tmp9B0.tag=7,((_tmp9B0.f1=*_tmp1AF,_tmp9B0)))),_tmp9B1)))),loc),((_tmp9AF->tl=0,
_tmp9AF)))));_npop_handler(0);return _tmp1D5;};_LLBB: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1B0=(struct Cyc_Absyn_DatatypeType_struct*)_tmp192;if(_tmp1B0->tag != 3)goto
_LLBD;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B1.datatype_info;if((_tmp1B2.UnknownDatatype).tag
!= 1)goto _LLBD;_tmp1B3=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B2.UnknownDatatype).val;
_tmp1B4=_tmp1B3.name;_tmp1B5=_tmp1B3.is_extensible;_tmp1B6=_tmp1B1.targs;}}_LLBC: {
struct Cyc_List_List*_tmp1D6=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp1B6);struct Cyc_Absyn_Decl*_tmp1D7=Cyc_Absyn_datatype_decl(s,_tmp1B4,
_tmp1D6,0,_tmp1B5,loc);if(_tmp189 != 0){const char*_tmp9B2;Cyc_Parse_err(((_tmp9B2="bad attributes on datatype",
_tag_dyneither(_tmp9B2,sizeof(char),27))),loc);}{struct Cyc_List_List*_tmp9B3;
struct Cyc_List_List*_tmp1DA=(_tmp9B3=_cycalloc(sizeof(*_tmp9B3)),((_tmp9B3->hd=
_tmp1D7,((_tmp9B3->tl=0,_tmp9B3)))));_npop_handler(0);return _tmp1DA;};}_LLBD: {
struct Cyc_Absyn_EnumType_struct*_tmp1B7=(struct Cyc_Absyn_EnumType_struct*)
_tmp192;if(_tmp1B7->tag != 13)goto _LLBF;else{_tmp1B8=_tmp1B7->f1;}}_LLBE: {struct
Cyc_Absyn_Enumdecl*_tmp9B4;struct Cyc_Absyn_Enumdecl*_tmp1DB=(_tmp9B4=_cycalloc(
sizeof(*_tmp9B4)),((_tmp9B4->sc=s,((_tmp9B4->name=_tmp1B8,((_tmp9B4->fields=0,
_tmp9B4)))))));if(_tmp189 != 0){const char*_tmp9B5;Cyc_Parse_err(((_tmp9B5="bad attributes on enum",
_tag_dyneither(_tmp9B5,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp9BF;
struct Cyc_Absyn_Enum_d_struct _tmp9BE;struct Cyc_Absyn_Enum_d_struct*_tmp9BD;
struct Cyc_List_List*_tmp9BC;struct Cyc_List_List*_tmp1E1=(_tmp9BC=_cycalloc(
sizeof(*_tmp9BC)),((_tmp9BC->hd=((_tmp9BF=_cycalloc(sizeof(*_tmp9BF)),((_tmp9BF->r=(
void*)((_tmp9BD=_cycalloc(sizeof(*_tmp9BD)),((_tmp9BD[0]=((_tmp9BE.tag=8,((
_tmp9BE.f1=_tmp1DB,_tmp9BE)))),_tmp9BD)))),((_tmp9BF->loc=loc,_tmp9BF)))))),((
_tmp9BC->tl=0,_tmp9BC)))));_npop_handler(0);return _tmp1E1;};}_LLBF: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1B9=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp192;if(_tmp1B9->tag != 14)goto
_LLC1;else{_tmp1BA=_tmp1B9->f1;}}_LLC0: {struct Cyc_Core_Opt*_tmp9C2;struct Cyc_Absyn_Enumdecl*
_tmp9C1;struct Cyc_Absyn_Enumdecl*_tmp1E3=(_tmp9C1=_cycalloc(sizeof(*_tmp9C1)),((
_tmp9C1->sc=s,((_tmp9C1->name=Cyc_Parse_gensym_enum(),((_tmp9C1->fields=((
_tmp9C2=_cycalloc(sizeof(*_tmp9C2)),((_tmp9C2->v=_tmp1BA,_tmp9C2)))),_tmp9C1)))))));
if(_tmp189 != 0){const char*_tmp9C3;Cyc_Parse_err(((_tmp9C3="bad attributes on enum",
_tag_dyneither(_tmp9C3,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp9CD;
struct Cyc_Absyn_Enum_d_struct _tmp9CC;struct Cyc_Absyn_Enum_d_struct*_tmp9CB;
struct Cyc_List_List*_tmp9CA;struct Cyc_List_List*_tmp1E9=(_tmp9CA=_cycalloc(
sizeof(*_tmp9CA)),((_tmp9CA->hd=((_tmp9CD=_cycalloc(sizeof(*_tmp9CD)),((_tmp9CD->r=(
void*)((_tmp9CB=_cycalloc(sizeof(*_tmp9CB)),((_tmp9CB[0]=((_tmp9CC.tag=8,((
_tmp9CC.f1=_tmp1E3,_tmp9CC)))),_tmp9CB)))),((_tmp9CD->loc=loc,_tmp9CD)))))),((
_tmp9CA->tl=0,_tmp9CA)))));_npop_handler(0);return _tmp1E9;};}_LLC1:;_LLC2:{const
char*_tmp9CE;Cyc_Parse_err(((_tmp9CE="missing declarator",_tag_dyneither(_tmp9CE,
sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1ED=0;_npop_handler(0);return
_tmp1ED;};_LLB0:;}else{struct Cyc_List_List*_tmp1EE=Cyc_Parse_apply_tmss(mkrgn,
_tmp187,_tmp191,_tmp18F,_tmp189);if(istypedef){if(!exps_empty){const char*_tmp9CF;
Cyc_Parse_err(((_tmp9CF="initializer in typedef declaration",_tag_dyneither(
_tmp9CF,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple13*),unsigned int env,struct
Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1EE);struct
Cyc_List_List*_tmp1F0=decls;_npop_handler(0);return _tmp1F0;};}else{struct Cyc_List_List*
decls=0;{struct Cyc_List_List*_tmp1F1=_tmp1EE;for(0;_tmp1F1 != 0;(_tmp1F1=_tmp1F1->tl,
_tmp190=_tmp190->tl)){struct _tuple13 _tmp1F3;struct _tuple1*_tmp1F4;struct Cyc_Absyn_Tqual
_tmp1F5;void*_tmp1F6;struct Cyc_List_List*_tmp1F7;struct Cyc_List_List*_tmp1F8;
struct _tuple13*_tmp1F2=(struct _tuple13*)_tmp1F1->hd;_tmp1F3=*_tmp1F2;_tmp1F4=
_tmp1F3.f1;_tmp1F5=_tmp1F3.f2;_tmp1F6=_tmp1F3.f3;_tmp1F7=_tmp1F3.f4;_tmp1F8=
_tmp1F3.f5;if(_tmp1F7 != 0){const char*_tmp9D2;void*_tmp9D1;(_tmp9D1=0,Cyc_Tcutil_warn(
loc,((_tmp9D2="bad type params, ignoring",_tag_dyneither(_tmp9D2,sizeof(char),26))),
_tag_dyneither(_tmp9D1,sizeof(void*),0)));}if(_tmp190 == 0){const char*_tmp9D5;
void*_tmp9D4;(_tmp9D4=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp9D5="unexpected NULL in parse!",
_tag_dyneither(_tmp9D5,sizeof(char),26))),_tag_dyneither(_tmp9D4,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1FD=(struct Cyc_Absyn_Exp*)_tmp190->hd;struct Cyc_Absyn_Vardecl*
_tmp1FE=Cyc_Absyn_new_vardecl(_tmp1F4,_tmp1F6,_tmp1FD);_tmp1FE->tq=_tmp1F5;
_tmp1FE->sc=s;_tmp1FE->attributes=_tmp1F8;{struct Cyc_Absyn_Var_d_struct*_tmp9DB;
struct Cyc_Absyn_Var_d_struct _tmp9DA;struct Cyc_Absyn_Decl*_tmp9D9;struct Cyc_Absyn_Decl*
_tmp1FF=(_tmp9D9=_cycalloc(sizeof(*_tmp9D9)),((_tmp9D9->r=(void*)((_tmp9DB=
_cycalloc(sizeof(*_tmp9DB)),((_tmp9DB[0]=((_tmp9DA.tag=0,((_tmp9DA.f1=_tmp1FE,
_tmp9DA)))),_tmp9DB)))),((_tmp9D9->loc=loc,_tmp9D9)))));struct Cyc_List_List*
_tmp9DC;decls=((_tmp9DC=_cycalloc(sizeof(*_tmp9DC)),((_tmp9DC->hd=_tmp1FF,((
_tmp9DC->tl=decls,_tmp9DC))))));};};}}{struct Cyc_List_List*_tmp204=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp204;};}}};};};};};
_pop_region(mkrgn);}static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct
_dyneither_ptr s,unsigned int loc){if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || 
Cyc_strlen((struct _dyneither_ptr)s)== 2)switch(*((const char*)
_check_dyneither_subscript(s,sizeof(char),0))){case 'A': _LLC3: return& Cyc_Tcutil_ak;
case 'M': _LLC4: return& Cyc_Tcutil_mk;case 'B': _LLC5: return& Cyc_Tcutil_bk;case 'R':
_LLC6: return& Cyc_Tcutil_rk;case 'E': _LLC7: return& Cyc_Tcutil_ek;case 'I': _LLC8:
return& Cyc_Tcutil_ik;case 'U': _LLC9: switch(*((const char*)
_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLCB: return& Cyc_Tcutil_urk;
case 'A': _LLCC: return& Cyc_Tcutil_uak;case 'M': _LLCD: return& Cyc_Tcutil_umk;case 'B':
_LLCE: return& Cyc_Tcutil_ubk;default: _LLCF: break;}break;case 'T': _LLCA: switch(*((
const char*)_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLD2: return&
Cyc_Tcutil_trk;case 'A': _LLD3: return& Cyc_Tcutil_tak;case 'M': _LLD4: return& Cyc_Tcutil_tmk;
case 'B': _LLD5: return& Cyc_Tcutil_tbk;default: _LLD6: break;}break;default: _LLD1:
break;}{const char*_tmp9E1;void*_tmp9E0[2];struct Cyc_String_pa_struct _tmp9DF;
struct Cyc_Int_pa_struct _tmp9DE;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9DE.tag=
1,((_tmp9DE.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((
_tmp9DF.tag=0,((_tmp9DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp9E0[0]=& _tmp9DF,((_tmp9E0[1]=& _tmp9DE,Cyc_aprintf(((_tmp9E1="bad kind: %s; strlen=%d",
_tag_dyneither(_tmp9E1,sizeof(char),24))),_tag_dyneither(_tmp9E0,sizeof(void*),2)))))))))))))),
loc);}return& Cyc_Tcutil_bk;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct
Cyc_Absyn_Pat*p){void*_tmp209=p->r;struct _tuple1*_tmp20B;struct Cyc_Absyn_Vardecl*
_tmp20D;struct Cyc_Absyn_Pat*_tmp20E;struct Cyc_Absyn_Pat _tmp20F;void*_tmp210;
struct Cyc_Absyn_Pat*_tmp213;enum Cyc_Absyn_Sign _tmp216;int _tmp217;char _tmp219;
struct _dyneither_ptr _tmp21B;int _tmp21C;struct _tuple1*_tmp21E;struct Cyc_List_List*
_tmp21F;int _tmp220;struct Cyc_Absyn_Exp*_tmp222;_LLDA: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp20A=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp209;if(_tmp20A->tag != 14)goto
_LLDC;else{_tmp20B=_tmp20A->f1;}}_LLDB: {struct Cyc_Absyn_UnknownId_e_struct
_tmp9E4;struct Cyc_Absyn_UnknownId_e_struct*_tmp9E3;return Cyc_Absyn_new_exp((void*)((
_tmp9E3=_cycalloc(sizeof(*_tmp9E3)),((_tmp9E3[0]=((_tmp9E4.tag=2,((_tmp9E4.f1=
_tmp20B,_tmp9E4)))),_tmp9E3)))),p->loc);}_LLDC: {struct Cyc_Absyn_Reference_p_struct*
_tmp20C=(struct Cyc_Absyn_Reference_p_struct*)_tmp209;if(_tmp20C->tag != 2)goto
_LLDE;else{_tmp20D=_tmp20C->f1;_tmp20E=_tmp20C->f2;_tmp20F=*_tmp20E;_tmp210=
_tmp20F.r;{struct Cyc_Absyn_Wild_p_struct*_tmp211=(struct Cyc_Absyn_Wild_p_struct*)
_tmp210;if(_tmp211->tag != 0)goto _LLDE;};}}_LLDD: {struct Cyc_Absyn_UnknownId_e_struct
_tmp9E7;struct Cyc_Absyn_UnknownId_e_struct*_tmp9E6;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((
void*)((_tmp9E6=_cycalloc(sizeof(*_tmp9E6)),((_tmp9E6[0]=((_tmp9E7.tag=2,((
_tmp9E7.f1=_tmp20D->name,_tmp9E7)))),_tmp9E6)))),p->loc),p->loc);}_LLDE: {struct
Cyc_Absyn_Pointer_p_struct*_tmp212=(struct Cyc_Absyn_Pointer_p_struct*)_tmp209;
if(_tmp212->tag != 5)goto _LLE0;else{_tmp213=_tmp212->f1;}}_LLDF: return Cyc_Absyn_address_exp(
Cyc_Parse_pat2exp(_tmp213),p->loc);_LLE0: {struct Cyc_Absyn_Null_p_struct*_tmp214=(
struct Cyc_Absyn_Null_p_struct*)_tmp209;if(_tmp214->tag != 8)goto _LLE2;}_LLE1:
return Cyc_Absyn_null_exp(p->loc);_LLE2: {struct Cyc_Absyn_Int_p_struct*_tmp215=(
struct Cyc_Absyn_Int_p_struct*)_tmp209;if(_tmp215->tag != 9)goto _LLE4;else{_tmp216=
_tmp215->f1;_tmp217=_tmp215->f2;}}_LLE3: return Cyc_Absyn_int_exp(_tmp216,_tmp217,
p->loc);_LLE4: {struct Cyc_Absyn_Char_p_struct*_tmp218=(struct Cyc_Absyn_Char_p_struct*)
_tmp209;if(_tmp218->tag != 10)goto _LLE6;else{_tmp219=_tmp218->f1;}}_LLE5: return
Cyc_Absyn_char_exp(_tmp219,p->loc);_LLE6: {struct Cyc_Absyn_Float_p_struct*
_tmp21A=(struct Cyc_Absyn_Float_p_struct*)_tmp209;if(_tmp21A->tag != 11)goto _LLE8;
else{_tmp21B=_tmp21A->f1;_tmp21C=_tmp21A->f2;}}_LLE7: return Cyc_Absyn_float_exp(
_tmp21B,_tmp21C,p->loc);_LLE8: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp21D=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp209;if(_tmp21D->tag != 15)goto _LLEA;
else{_tmp21E=_tmp21D->f1;_tmp21F=_tmp21D->f2;_tmp220=_tmp21D->f3;if(_tmp220 != 0)
goto _LLEA;}}_LLE9: {struct Cyc_Absyn_UnknownId_e_struct _tmp9EA;struct Cyc_Absyn_UnknownId_e_struct*
_tmp9E9;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)((_tmp9E9=_cycalloc(
sizeof(*_tmp9E9)),((_tmp9E9[0]=((_tmp9EA.tag=2,((_tmp9EA.f1=_tmp21E,_tmp9EA)))),
_tmp9E9)))),p->loc);struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,
_tmp21F);return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}_LLEA: {struct Cyc_Absyn_Exp_p_struct*
_tmp221=(struct Cyc_Absyn_Exp_p_struct*)_tmp209;if(_tmp221->tag != 16)goto _LLEC;
else{_tmp222=_tmp221->f1;}}_LLEB: return _tmp222;_LLEC:;_LLED:{const char*_tmp9EB;
Cyc_Parse_err(((_tmp9EB="cannot mix patterns and expressions in case",
_tag_dyneither(_tmp9EB,sizeof(char),44))),p->loc);}return Cyc_Absyn_null_exp(p->loc);
_LLD9:;}struct _union_YYSTYPE_Int_tok{int tag;struct _tuple6 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*val;};struct _tuple18{
int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct
_tuple18 val;};struct _tuple19{unsigned int f1;union Cyc_Absyn_Constraint*f2;union
Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple19*val;};
struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple20{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple20*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple21{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple21*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple20*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Parse_Declaration_spec*val;};
struct _tuple22{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Parse_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Parse_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator*val;};
struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum 
Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*
val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple23{
struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple9*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple24{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple24*val;};
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
_union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp22B[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp22B,
_tmp22B,_tmp22B + 14}};struct _tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){struct
_tuple6 yyzzz;{union Cyc_YYSTYPE _tmp22C=yy1;struct _tuple6 _tmp22D;_LLEF: if((_tmp22C.Int_tok).tag
!= 1)goto _LLF1;_tmp22D=(struct _tuple6)(_tmp22C.Int_tok).val;_LLF0: yyzzz=_tmp22D;
goto _LLEE;_LLF1:;_LLF2:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLEE:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){union Cyc_YYSTYPE _tmp9EC;
return((_tmp9EC.Int_tok).val=yy1,(((_tmp9EC.Int_tok).tag=1,_tmp9EC)));}static
char _tmp230[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp230,_tmp230,_tmp230 + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE
yy1){char yyzzz;{union Cyc_YYSTYPE _tmp231=yy1;char _tmp232;_LLF4: if((_tmp231.Char_tok).tag
!= 2)goto _LLF6;_tmp232=(char)(_tmp231.Char_tok).val;_LLF5: yyzzz=_tmp232;goto
_LLF3;_LLF6:;_LLF7:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLF3:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9ED;return((_tmp9ED.Char_tok).val=
yy1,(((_tmp9ED.Char_tok).tag=2,_tmp9ED)));}static char _tmp235[13]="string_t<`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{
_tmp235,_tmp235,_tmp235 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp236=yy1;struct _dyneither_ptr
_tmp237;_LLF9: if((_tmp236.String_tok).tag != 3)goto _LLFB;_tmp237=(struct
_dyneither_ptr)(_tmp236.String_tok).val;_LLFA: yyzzz=_tmp237;goto _LLF8;_LLFB:;
_LLFC:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLF8:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9EE;return((_tmp9EE.String_tok).val=
yy1,(((_tmp9EE.String_tok).tag=3,_tmp9EE)));}static char _tmp23A[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp23A,
_tmp23A,_tmp23A + 56}};struct _tuple19*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct
_tuple19*yyzzz;{union Cyc_YYSTYPE _tmp23B=yy1;struct _tuple19*_tmp23C;_LLFE: if((
_tmp23B.YY1).tag != 7)goto _LL100;_tmp23C=(struct _tuple19*)(_tmp23B.YY1).val;_LLFF:
yyzzz=_tmp23C;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)& Cyc_yyfail_YY1);_LLFD:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY1(struct _tuple19*yy1){union Cyc_YYSTYPE _tmp9EF;
return((_tmp9EF.YY1).val=yy1,(((_tmp9EF.YY1).tag=7,_tmp9EF)));}static char _tmp23F[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp23F,_tmp23F,_tmp23F + 19}};union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE
yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp240=yy1;union Cyc_Absyn_Constraint*
_tmp241;_LL103: if((_tmp240.YY2).tag != 8)goto _LL105;_tmp241=(union Cyc_Absyn_Constraint*)(
_tmp240.YY2).val;_LL104: yyzzz=_tmp241;goto _LL102;_LL105:;_LL106:(int)_throw((
void*)& Cyc_yyfail_YY2);_LL102:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*
yy1){union Cyc_YYSTYPE _tmp9F0;return((_tmp9F0.YY2).val=yy1,(((_tmp9F0.YY2).tag=8,
_tmp9F0)));}static char _tmp244[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp244,_tmp244,_tmp244 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE _tmp245=yy1;
struct Cyc_Absyn_Exp*_tmp246;_LL108: if((_tmp245.YY3).tag != 9)goto _LL10A;_tmp246=(
struct Cyc_Absyn_Exp*)(_tmp245.YY3).val;_LL109: yyzzz=_tmp246;goto _LL107;_LL10A:;
_LL10B:(int)_throw((void*)& Cyc_yyfail_YY3);_LL107:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY3(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9F1;return((_tmp9F1.YY3).val=
yy1,(((_tmp9F1.YY3).tag=9,_tmp9F1)));}static char _tmp249[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp249,_tmp249,
_tmp249 + 10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE _tmp24A=yy1;struct Cyc_Absyn_Exp*_tmp24B;_LL10D: if((
_tmp24A.YY4).tag != 10)goto _LL10F;_tmp24B=(struct Cyc_Absyn_Exp*)(_tmp24A.YY4).val;
_LL10E: yyzzz=_tmp24B;goto _LL10C;_LL10F:;_LL110:(int)_throw((void*)& Cyc_yyfail_YY4);
_LL10C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE
_tmp9F2;return((_tmp9F2.YY4).val=yy1,(((_tmp9F2.YY4).tag=10,_tmp9F2)));}static
char _tmp24E[17]="list_t<exp_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={
Cyc_Core_Failure,{_tmp24E,_tmp24E,_tmp24E + 17}};struct Cyc_List_List*Cyc_yyget_YY5(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp24F=yy1;
struct Cyc_List_List*_tmp250;_LL112: if((_tmp24F.YY5).tag != 11)goto _LL114;_tmp250=(
struct Cyc_List_List*)(_tmp24F.YY5).val;_LL113: yyzzz=_tmp250;goto _LL111;_LL114:;
_LL115:(int)_throw((void*)& Cyc_yyfail_YY5);_LL111:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F3;return((_tmp9F3.YY5).val=
yy1,(((_tmp9F3.YY5).tag=11,_tmp9F3)));}static char _tmp253[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp253,
_tmp253,_tmp253 + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp254=yy1;struct Cyc_List_List*
_tmp255;_LL117: if((_tmp254.YY6).tag != 12)goto _LL119;_tmp255=(struct Cyc_List_List*)(
_tmp254.YY6).val;_LL118: yyzzz=_tmp255;goto _LL116;_LL119:;_LL11A:(int)_throw((
void*)& Cyc_yyfail_YY6);_LL116:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9F4;return((_tmp9F4.YY6).val=yy1,(((_tmp9F4.YY6).tag=12,
_tmp9F4)));}static char _tmp258[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp258,_tmp258,_tmp258 + 9}};enum Cyc_Absyn_Primop
Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Primop yyzzz;{union Cyc_YYSTYPE
_tmp259=yy1;enum Cyc_Absyn_Primop _tmp25A;_LL11C: if((_tmp259.YY7).tag != 13)goto
_LL11E;_tmp25A=(enum Cyc_Absyn_Primop)(_tmp259.YY7).val;_LL11D: yyzzz=_tmp25A;
goto _LL11B;_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY7);_LL11B:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9F5;
return((_tmp9F5.YY7).val=yy1,(((_tmp9F5.YY7).tag=13,_tmp9F5)));}static char
_tmp25D[19]="opt_t<primop_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={
Cyc_Core_Failure,{_tmp25D,_tmp25D,_tmp25D + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(
union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp25E=yy1;
struct Cyc_Core_Opt*_tmp25F;_LL121: if((_tmp25E.YY8).tag != 14)goto _LL123;_tmp25F=(
struct Cyc_Core_Opt*)(_tmp25E.YY8).val;_LL122: yyzzz=_tmp25F;goto _LL120;_LL123:;
_LL124:(int)_throw((void*)& Cyc_yyfail_YY8);_LL120:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY8(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9F6;return((_tmp9F6.YY8).val=
yy1,(((_tmp9F6.YY8).tag=14,_tmp9F6)));}static char _tmp262[7]="qvar_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp262,
_tmp262,_tmp262 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp263=yy1;struct _tuple1*_tmp264;_LL126:
if((_tmp263.QualId_tok).tag != 5)goto _LL128;_tmp264=(struct _tuple1*)(_tmp263.QualId_tok).val;
_LL127: yyzzz=_tmp264;goto _LL125;_LL128:;_LL129:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL125:;}return yyzzz;}union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple1*yy1){union Cyc_YYSTYPE
_tmp9F7;return((_tmp9F7.QualId_tok).val=yy1,(((_tmp9F7.QualId_tok).tag=5,_tmp9F7)));}
static char _tmp267[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY9={
Cyc_Core_Failure,{_tmp267,_tmp267,_tmp267 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE _tmp268=yy1;
struct Cyc_Absyn_Stmt*_tmp269;_LL12B: if((_tmp268.YY9).tag != 15)goto _LL12D;_tmp269=(
struct Cyc_Absyn_Stmt*)(_tmp268.YY9).val;_LL12C: yyzzz=_tmp269;goto _LL12A;_LL12D:;
_LL12E:(int)_throw((void*)& Cyc_yyfail_YY9);_LL12A:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9F8;return((_tmp9F8.YY9).val=
yy1,(((_tmp9F8.YY9).tag=15,_tmp9F8)));}static char _tmp26C[27]="list_t<switch_clause_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp26C,
_tmp26C,_tmp26C + 27}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp26D=yy1;struct Cyc_List_List*
_tmp26E;_LL130: if((_tmp26D.YY10).tag != 16)goto _LL132;_tmp26E=(struct Cyc_List_List*)(
_tmp26D.YY10).val;_LL131: yyzzz=_tmp26E;goto _LL12F;_LL132:;_LL133:(int)_throw((
void*)& Cyc_yyfail_YY10);_LL12F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9F9;return((_tmp9F9.YY10).val=yy1,(((_tmp9F9.YY10).tag=
16,_tmp9F9)));}static char _tmp271[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp271,_tmp271,_tmp271 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE
_tmp272=yy1;struct Cyc_Absyn_Pat*_tmp273;_LL135: if((_tmp272.YY11).tag != 17)goto
_LL137;_tmp273=(struct Cyc_Absyn_Pat*)(_tmp272.YY11).val;_LL136: yyzzz=_tmp273;
goto _LL134;_LL137:;_LL138:(int)_throw((void*)& Cyc_yyfail_YY11);_LL134:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9FA;
return((_tmp9FA.YY11).val=yy1,(((_tmp9FA.YY11).tag=17,_tmp9FA)));}static char
_tmp276[28]="$(list_t<pat_t,`H>,bool)@`H";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={
Cyc_Core_Failure,{_tmp276,_tmp276,_tmp276 + 28}};struct _tuple20*Cyc_yyget_YY12(
union Cyc_YYSTYPE yy1){struct _tuple20*yyzzz;{union Cyc_YYSTYPE _tmp277=yy1;struct
_tuple20*_tmp278;_LL13A: if((_tmp277.YY12).tag != 18)goto _LL13C;_tmp278=(struct
_tuple20*)(_tmp277.YY12).val;_LL13B: yyzzz=_tmp278;goto _LL139;_LL13C:;_LL13D:(int)
_throw((void*)& Cyc_yyfail_YY12);_LL139:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY12(
struct _tuple20*yy1){union Cyc_YYSTYPE _tmp9FB;return((_tmp9FB.YY12).val=yy1,(((
_tmp9FB.YY12).tag=18,_tmp9FB)));}static char _tmp27B[17]="list_t<pat_t,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp27B,_tmp27B,
_tmp27B + 17}};struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp27C=yy1;struct Cyc_List_List*_tmp27D;_LL13F: if((
_tmp27C.YY13).tag != 19)goto _LL141;_tmp27D=(struct Cyc_List_List*)(_tmp27C.YY13).val;
_LL140: yyzzz=_tmp27D;goto _LL13E;_LL141:;_LL142:(int)_throw((void*)& Cyc_yyfail_YY13);
_LL13E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9FC;return((_tmp9FC.YY13).val=yy1,(((_tmp9FC.YY13).tag=19,_tmp9FC)));}static
char _tmp280[36]="$(list_t<designator_t,`H>,pat_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp280,_tmp280,_tmp280 + 36}};struct _tuple21*
Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE
_tmp281=yy1;struct _tuple21*_tmp282;_LL144: if((_tmp281.YY14).tag != 20)goto _LL146;
_tmp282=(struct _tuple21*)(_tmp281.YY14).val;_LL145: yyzzz=_tmp282;goto _LL143;
_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY14);_LL143:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY14(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9FD;return((_tmp9FD.YY14).val=
yy1,(((_tmp9FD.YY14).tag=20,_tmp9FD)));}static char _tmp285[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp285,
_tmp285,_tmp285 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp286=yy1;struct Cyc_List_List*
_tmp287;_LL149: if((_tmp286.YY15).tag != 21)goto _LL14B;_tmp287=(struct Cyc_List_List*)(
_tmp286.YY15).val;_LL14A: yyzzz=_tmp287;goto _LL148;_LL14B:;_LL14C:(int)_throw((
void*)& Cyc_yyfail_YY15);_LL148:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9FE;return((_tmp9FE.YY15).val=yy1,(((_tmp9FE.YY15).tag=
21,_tmp9FE)));}static char _tmp28A[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp28A,
_tmp28A,_tmp28A + 58}};struct _tuple20*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct
_tuple20*yyzzz;{union Cyc_YYSTYPE _tmp28B=yy1;struct _tuple20*_tmp28C;_LL14E: if((
_tmp28B.YY16).tag != 22)goto _LL150;_tmp28C=(struct _tuple20*)(_tmp28B.YY16).val;
_LL14F: yyzzz=_tmp28C;goto _LL14D;_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY16);
_LL14D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY16(struct _tuple20*yy1){union Cyc_YYSTYPE
_tmp9FF;return((_tmp9FF.YY16).val=yy1,(((_tmp9FF.YY16).tag=22,_tmp9FF)));}static
char _tmp28F[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp28F,_tmp28F,_tmp28F + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp290=yy1;
struct Cyc_Absyn_Fndecl*_tmp291;_LL153: if((_tmp290.YY17).tag != 23)goto _LL155;
_tmp291=(struct Cyc_Absyn_Fndecl*)(_tmp290.YY17).val;_LL154: yyzzz=_tmp291;goto
_LL152;_LL155:;_LL156:(int)_throw((void*)& Cyc_yyfail_YY17);_LL152:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmpA00;
return((_tmpA00.YY17).val=yy1,(((_tmpA00.YY17).tag=23,_tmpA00)));}static char
_tmp294[18]="list_t<decl_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp294,_tmp294,_tmp294 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp295=yy1;
struct Cyc_List_List*_tmp296;_LL158: if((_tmp295.YY18).tag != 24)goto _LL15A;_tmp296=(
struct Cyc_List_List*)(_tmp295.YY18).val;_LL159: yyzzz=_tmp296;goto _LL157;_LL15A:;
_LL15B:(int)_throw((void*)& Cyc_yyfail_YY18);_LL157:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA01;return((_tmpA01.YY18).val=
yy1,(((_tmpA01.YY18).tag=24,_tmpA01)));}static char _tmp299[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp299,
_tmp299,_tmp299 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp29A=yy1;struct
Cyc_Parse_Declaration_spec*_tmp29B;_LL15D: if((_tmp29A.YY19).tag != 25)goto _LL15F;
_tmp29B=(struct Cyc_Parse_Declaration_spec*)(_tmp29A.YY19).val;_LL15E: yyzzz=
_tmp29B;goto _LL15C;_LL15F:;_LL160:(int)_throw((void*)& Cyc_yyfail_YY19);_LL15C:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){
union Cyc_YYSTYPE _tmpA02;return((_tmpA02.YY19).val=yy1,(((_tmpA02.YY19).tag=25,
_tmpA02)));}static char _tmp29E[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp29E,_tmp29E,_tmp29E + 29}};struct _tuple22*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){struct _tuple22*yyzzz;{union Cyc_YYSTYPE
_tmp29F=yy1;struct _tuple22*_tmp2A0;_LL162: if((_tmp29F.YY20).tag != 26)goto _LL164;
_tmp2A0=(struct _tuple22*)(_tmp29F.YY20).val;_LL163: yyzzz=_tmp2A0;goto _LL161;
_LL164:;_LL165:(int)_throw((void*)& Cyc_yyfail_YY20);_LL161:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY20(struct _tuple22*yy1){union Cyc_YYSTYPE _tmpA03;return((_tmpA03.YY20).val=
yy1,(((_tmpA03.YY20).tag=26,_tmpA03)));}static char _tmp2A3[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp2A3,
_tmp2A3,_tmp2A3 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2A4=yy1;struct Cyc_List_List*
_tmp2A5;_LL167: if((_tmp2A4.YY21).tag != 27)goto _LL169;_tmp2A5=(struct Cyc_List_List*)(
_tmp2A4.YY21).val;_LL168: yyzzz=_tmp2A5;goto _LL166;_LL169:;_LL16A:(int)_throw((
void*)& Cyc_yyfail_YY21);_LL166:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmpA04;return((_tmpA04.YY21).val=yy1,(((_tmpA04.YY21).tag=
27,_tmpA04)));}static char _tmp2A8[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp2A8,_tmp2A8,_tmp2A8 + 16}};enum Cyc_Parse_Storage_class
Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){enum Cyc_Parse_Storage_class yyzzz;{union Cyc_YYSTYPE
_tmp2A9=yy1;enum Cyc_Parse_Storage_class _tmp2AA;_LL16C: if((_tmp2A9.YY22).tag != 
28)goto _LL16E;_tmp2AA=(enum Cyc_Parse_Storage_class)(_tmp2A9.YY22).val;_LL16D:
yyzzz=_tmp2AA;goto _LL16B;_LL16E:;_LL16F:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL16B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class yy1){
union Cyc_YYSTYPE _tmpA05;return((_tmpA05.YY22).val=yy1,(((_tmpA05.YY22).tag=28,
_tmpA05)));}static char _tmp2AD[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp2AD,_tmp2AD,_tmp2AD + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2AE=yy1;void*_tmp2AF;_LL171:
if((_tmp2AE.YY23).tag != 29)goto _LL173;_tmp2AF=(void*)(_tmp2AE.YY23).val;_LL172:
yyzzz=_tmp2AF;goto _LL170;_LL173:;_LL174:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL170:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(void*yy1){union Cyc_YYSTYPE _tmpA06;
return((_tmpA06.YY23).val=yy1,(((_tmpA06.YY23).tag=29,_tmpA06)));}static char
_tmp2B2[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={
Cyc_Core_Failure,{_tmp2B2,_tmp2B2,_tmp2B2 + 12}};enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(
union Cyc_YYSTYPE yy1){enum Cyc_Absyn_AggrKind yyzzz;{union Cyc_YYSTYPE _tmp2B3=yy1;
enum Cyc_Absyn_AggrKind _tmp2B4;_LL176: if((_tmp2B3.YY24).tag != 30)goto _LL178;
_tmp2B4=(enum Cyc_Absyn_AggrKind)(_tmp2B3.YY24).val;_LL177: yyzzz=_tmp2B4;goto
_LL175;_LL178:;_LL179:(int)_throw((void*)& Cyc_yyfail_YY24);_LL175:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmpA07;
return((_tmpA07.YY24).val=yy1,(((_tmpA07.YY24).tag=30,_tmpA07)));}static char
_tmp2B7[8]="tqual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{
_tmp2B7,_tmp2B7,_tmp2B7 + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE _tmp2B8=yy1;struct Cyc_Absyn_Tqual
_tmp2B9;_LL17B: if((_tmp2B8.YY25).tag != 31)goto _LL17D;_tmp2B9=(struct Cyc_Absyn_Tqual)(
_tmp2B8.YY25).val;_LL17C: yyzzz=_tmp2B9;goto _LL17A;_LL17D:;_LL17E:(int)_throw((
void*)& Cyc_yyfail_YY25);_LL17A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual
yy1){union Cyc_YYSTYPE _tmpA08;return((_tmpA08.YY25).val=yy1,(((_tmpA08.YY25).tag=
31,_tmpA08)));}static char _tmp2BC[23]="list_t<aggrfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp2BC,_tmp2BC,_tmp2BC + 23}};struct Cyc_List_List*
Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2BD=yy1;struct Cyc_List_List*_tmp2BE;_LL180: if((_tmp2BD.YY26).tag != 32)goto
_LL182;_tmp2BE=(struct Cyc_List_List*)(_tmp2BD.YY26).val;_LL181: yyzzz=_tmp2BE;
goto _LL17F;_LL182:;_LL183:(int)_throw((void*)& Cyc_yyfail_YY26);_LL17F:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA09;
return((_tmpA09.YY26).val=yy1,(((_tmpA09.YY26).tag=32,_tmpA09)));}static char
_tmp2C1[34]="list_t<list_t<aggrfield_t,`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2C1,_tmp2C1,_tmp2C1 + 34}};struct Cyc_List_List*
Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2C2=yy1;struct Cyc_List_List*_tmp2C3;_LL185: if((_tmp2C2.YY27).tag != 33)goto
_LL187;_tmp2C3=(struct Cyc_List_List*)(_tmp2C2.YY27).val;_LL186: yyzzz=_tmp2C3;
goto _LL184;_LL187:;_LL188:(int)_throw((void*)& Cyc_yyfail_YY27);_LL184:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA0A;
return((_tmpA0A.YY27).val=yy1,(((_tmpA0A.YY27).tag=33,_tmpA0A)));}static char
_tmp2C6[31]="list_t<type_modifier_t<`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2C6,_tmp2C6,_tmp2C6 + 31}};struct Cyc_List_List*
Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2C7=yy1;struct Cyc_List_List*_tmp2C8;_LL18A: if((_tmp2C7.YY28).tag != 34)goto
_LL18C;_tmp2C8=(struct Cyc_List_List*)(_tmp2C7.YY28).val;_LL18B: yyzzz=_tmp2C8;
goto _LL189;_LL18C:;_LL18D:(int)_throw((void*)& Cyc_yyfail_YY28);_LL189:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA0B;
return((_tmpA0B.YY28).val=yy1,(((_tmpA0B.YY28).tag=34,_tmpA0B)));}static char
_tmp2CB[13]="declarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={
Cyc_Core_Failure,{_tmp2CB,_tmp2CB,_tmp2CB + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE _tmp2CC=
yy1;struct Cyc_Parse_Declarator*_tmp2CD;_LL18F: if((_tmp2CC.YY29).tag != 35)goto
_LL191;_tmp2CD=(struct Cyc_Parse_Declarator*)(_tmp2CC.YY29).val;_LL190: yyzzz=
_tmp2CD;goto _LL18E;_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY29);_LL18E:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){union Cyc_YYSTYPE
_tmpA0C;return((_tmpA0C.YY29).val=yy1,(((_tmpA0C.YY29).tag=35,_tmpA0C)));}static
char _tmp2D0[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={
Cyc_Core_Failure,{_tmp2D0,_tmp2D0,_tmp2D0 + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{
union Cyc_YYSTYPE _tmp2D1=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp2D2;_LL194:
if((_tmp2D1.YY30).tag != 36)goto _LL196;_tmp2D2=(struct Cyc_Parse_Abstractdeclarator*)(
_tmp2D1.YY30).val;_LL195: yyzzz=_tmp2D2;goto _LL193;_LL196:;_LL197:(int)_throw((
void*)& Cyc_yyfail_YY30);_LL193:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1){union Cyc_YYSTYPE _tmpA0D;return((_tmpA0D.YY30).val=yy1,(((_tmpA0D.YY30).tag=
36,_tmpA0D)));}static char _tmp2D5[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2D5,_tmp2D5,_tmp2D5 + 5}};int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp2D6=yy1;int _tmp2D7;_LL199: if((
_tmp2D6.YY31).tag != 37)goto _LL19B;_tmp2D7=(int)(_tmp2D6.YY31).val;_LL19A: yyzzz=
_tmp2D7;goto _LL198;_LL19B:;_LL19C:(int)_throw((void*)& Cyc_yyfail_YY31);_LL198:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmpA0E;return((
_tmpA0E.YY31).val=yy1,(((_tmpA0E.YY31).tag=37,_tmpA0E)));}static char _tmp2DA[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2DA,
_tmp2DA,_tmp2DA + 8}};enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;{union Cyc_YYSTYPE _tmp2DB=yy1;enum Cyc_Absyn_Scope
_tmp2DC;_LL19E: if((_tmp2DB.YY32).tag != 38)goto _LL1A0;_tmp2DC=(enum Cyc_Absyn_Scope)(
_tmp2DB.YY32).val;_LL19F: yyzzz=_tmp2DC;goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((
void*)& Cyc_yyfail_YY32);_LL19D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope
yy1){union Cyc_YYSTYPE _tmpA0F;return((_tmpA0F.YY32).val=yy1,(((_tmpA0F.YY32).tag=
38,_tmpA0F)));}static char _tmp2DF[16]="datatypefield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2DF,_tmp2DF,_tmp2DF + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union
Cyc_YYSTYPE _tmp2E0=yy1;struct Cyc_Absyn_Datatypefield*_tmp2E1;_LL1A3: if((_tmp2E0.YY33).tag
!= 39)goto _LL1A5;_tmp2E1=(struct Cyc_Absyn_Datatypefield*)(_tmp2E0.YY33).val;
_LL1A4: yyzzz=_tmp2E1;goto _LL1A2;_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL1A2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){
union Cyc_YYSTYPE _tmpA10;return((_tmpA10.YY33).val=yy1,(((_tmpA10.YY33).tag=39,
_tmpA10)));}static char _tmp2E4[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2E4,_tmp2E4,_tmp2E4 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2E5=yy1;struct Cyc_List_List*_tmp2E6;_LL1A8: if((_tmp2E5.YY34).tag != 40)goto
_LL1AA;_tmp2E6=(struct Cyc_List_List*)(_tmp2E5.YY34).val;_LL1A9: yyzzz=_tmp2E6;
goto _LL1A7;_LL1AA:;_LL1AB:(int)_throw((void*)& Cyc_yyfail_YY34);_LL1A7:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA11;
return((_tmpA11.YY34).val=yy1,(((_tmpA11.YY34).tag=40,_tmpA11)));}static char
_tmp2E9[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2E9,_tmp2E9,
_tmp2E9 + 55}};struct _tuple23*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple23*
yyzzz;{union Cyc_YYSTYPE _tmp2EA=yy1;struct _tuple23*_tmp2EB;_LL1AD: if((_tmp2EA.YY35).tag
!= 41)goto _LL1AF;_tmp2EB=(struct _tuple23*)(_tmp2EA.YY35).val;_LL1AE: yyzzz=
_tmp2EB;goto _LL1AC;_LL1AF:;_LL1B0:(int)_throw((void*)& Cyc_yyfail_YY35);_LL1AC:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY35(struct _tuple23*yy1){union Cyc_YYSTYPE
_tmpA12;return((_tmpA12.YY35).val=yy1,(((_tmpA12.YY35).tag=41,_tmpA12)));}static
char _tmp2EE[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={
Cyc_Core_Failure,{_tmp2EE,_tmp2EE,_tmp2EE + 17}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2EF=yy1;
struct Cyc_List_List*_tmp2F0;_LL1B2: if((_tmp2EF.YY36).tag != 42)goto _LL1B4;_tmp2F0=(
struct Cyc_List_List*)(_tmp2EF.YY36).val;_LL1B3: yyzzz=_tmp2F0;goto _LL1B1;_LL1B4:;
_LL1B5:(int)_throw((void*)& Cyc_yyfail_YY36);_LL1B1:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA13;return((_tmpA13.YY36).val=
yy1,(((_tmpA13.YY36).tag=42,_tmpA13)));}static char _tmp2F3[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2F3,
_tmp2F3,_tmp2F3 + 37}};struct _tuple9*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){struct
_tuple9*yyzzz;{union Cyc_YYSTYPE _tmp2F4=yy1;struct _tuple9*_tmp2F5;_LL1B7: if((
_tmp2F4.YY37).tag != 43)goto _LL1B9;_tmp2F5=(struct _tuple9*)(_tmp2F4.YY37).val;
_LL1B8: yyzzz=_tmp2F5;goto _LL1B6;_LL1B9:;_LL1BA:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL1B6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple9*yy1){union Cyc_YYSTYPE
_tmpA14;return((_tmpA14.YY37).val=yy1,(((_tmpA14.YY37).tag=43,_tmpA14)));}static
char _tmp2F8[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2F8,_tmp2F8,
_tmp2F8 + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2F9=yy1;struct Cyc_List_List*_tmp2FA;_LL1BC: if((
_tmp2F9.YY38).tag != 44)goto _LL1BE;_tmp2FA=(struct Cyc_List_List*)(_tmp2F9.YY38).val;
_LL1BD: yyzzz=_tmp2FA;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1BB:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmpA15;return((_tmpA15.YY38).val=yy1,(((_tmpA15.YY38).tag=44,_tmpA15)));}static
char _tmp2FD[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2FD,
_tmp2FD,_tmp2FD + 127}};struct _tuple24*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct
_tuple24*yyzzz;{union Cyc_YYSTYPE _tmp2FE=yy1;struct _tuple24*_tmp2FF;_LL1C1: if((
_tmp2FE.YY39).tag != 45)goto _LL1C3;_tmp2FF=(struct _tuple24*)(_tmp2FE.YY39).val;
_LL1C2: yyzzz=_tmp2FF;goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY39);
_LL1C0:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY39(struct _tuple24*yy1){union Cyc_YYSTYPE
_tmpA16;return((_tmpA16.YY39).val=yy1,(((_tmpA16.YY39).tag=45,_tmpA16)));}static
char _tmp302[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={
Cyc_Core_Failure,{_tmp302,_tmp302,_tmp302 + 18}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp303=yy1;
struct Cyc_List_List*_tmp304;_LL1C6: if((_tmp303.YY40).tag != 46)goto _LL1C8;_tmp304=(
struct Cyc_List_List*)(_tmp303.YY40).val;_LL1C7: yyzzz=_tmp304;goto _LL1C5;_LL1C8:;
_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY40);_LL1C5:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA17;return((_tmpA17.YY40).val=
yy1,(((_tmpA17.YY40).tag=46,_tmpA17)));}static char _tmp307[24]="list_t<designator_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp307,
_tmp307,_tmp307 + 24}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp308=yy1;struct Cyc_List_List*
_tmp309;_LL1CB: if((_tmp308.YY41).tag != 47)goto _LL1CD;_tmp309=(struct Cyc_List_List*)(
_tmp308.YY41).val;_LL1CC: yyzzz=_tmp309;goto _LL1CA;_LL1CD:;_LL1CE:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1CA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmpA18;return((_tmpA18.YY41).val=yy1,(((_tmpA18.YY41).tag=
47,_tmpA18)));}static char _tmp30C[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp30C,_tmp30C,_tmp30C + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp30D=yy1;void*_tmp30E;_LL1D0:
if((_tmp30D.YY42).tag != 48)goto _LL1D2;_tmp30E=(void*)(_tmp30D.YY42).val;_LL1D1:
yyzzz=_tmp30E;goto _LL1CF;_LL1D2:;_LL1D3:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1CF:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmpA19;
return((_tmpA19.YY42).val=yy1,(((_tmpA19.YY42).tag=48,_tmpA19)));}static char
_tmp311[7]="kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{
_tmp311,_tmp311,_tmp311 + 7}};struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Kind*yyzzz;{union Cyc_YYSTYPE _tmp312=yy1;struct Cyc_Absyn_Kind*
_tmp313;_LL1D5: if((_tmp312.YY43).tag != 49)goto _LL1D7;_tmp313=(struct Cyc_Absyn_Kind*)(
_tmp312.YY43).val;_LL1D6: yyzzz=_tmp313;goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((
void*)& Cyc_yyfail_YY43);_LL1D4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*
yy1){union Cyc_YYSTYPE _tmpA1A;return((_tmpA1A.YY43).val=yy1,(((_tmpA1A.YY43).tag=
49,_tmpA1A)));}static char _tmp316[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp316,_tmp316,_tmp316 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp317=yy1;void*_tmp318;_LL1DA:
if((_tmp317.YY44).tag != 50)goto _LL1DC;_tmp318=(void*)(_tmp317.YY44).val;_LL1DB:
yyzzz=_tmp318;goto _LL1D9;_LL1DC:;_LL1DD:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1D9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmpA1B;
return((_tmpA1B.YY44).val=yy1,(((_tmpA1B.YY44).tag=50,_tmpA1B)));}static char
_tmp31B[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY45={
Cyc_Core_Failure,{_tmp31B,_tmp31B,_tmp31B + 23}};struct Cyc_List_List*Cyc_yyget_YY45(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp31C=yy1;
struct Cyc_List_List*_tmp31D;_LL1DF: if((_tmp31C.YY45).tag != 51)goto _LL1E1;_tmp31D=(
struct Cyc_List_List*)(_tmp31C.YY45).val;_LL1E0: yyzzz=_tmp31D;goto _LL1DE;_LL1E1:;
_LL1E2:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1DE:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA1C;return((_tmpA1C.YY45).val=
yy1,(((_tmpA1C.YY45).tag=51,_tmpA1C)));}static char _tmp320[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp320,
_tmp320,_tmp320 + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union
Cyc_YYSTYPE _tmp321=yy1;void*_tmp322;_LL1E4: if((_tmp321.YY46).tag != 52)goto _LL1E6;
_tmp322=(void*)(_tmp321.YY46).val;_LL1E5: yyzzz=_tmp322;goto _LL1E3;_LL1E6:;_LL1E7:(
int)_throw((void*)& Cyc_yyfail_YY46);_LL1E3:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1){union Cyc_YYSTYPE _tmpA1D;return((_tmpA1D.YY46).val=yy1,(((_tmpA1D.YY46).tag=
52,_tmpA1D)));}static char _tmp325[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp325,_tmp325,_tmp325 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE
_tmp326=yy1;struct Cyc_Absyn_Enumfield*_tmp327;_LL1E9: if((_tmp326.YY47).tag != 53)
goto _LL1EB;_tmp327=(struct Cyc_Absyn_Enumfield*)(_tmp326.YY47).val;_LL1EA: yyzzz=
_tmp327;goto _LL1E8;_LL1EB:;_LL1EC:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1E8:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE
_tmpA1E;return((_tmpA1E.YY47).val=yy1,(((_tmpA1E.YY47).tag=53,_tmpA1E)));}static
char _tmp32A[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={
Cyc_Core_Failure,{_tmp32A,_tmp32A,_tmp32A + 23}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp32B=yy1;
struct Cyc_List_List*_tmp32C;_LL1EE: if((_tmp32B.YY48).tag != 54)goto _LL1F0;_tmp32C=(
struct Cyc_List_List*)(_tmp32B.YY48).val;_LL1EF: yyzzz=_tmp32C;goto _LL1ED;_LL1F0:;
_LL1F1:(int)_throw((void*)& Cyc_yyfail_YY48);_LL1ED:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA1F;return((_tmpA1F.YY48).val=
yy1,(((_tmpA1F.YY48).tag=54,_tmpA1F)));}static char _tmp32F[17]="opt_t<type_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp32F,
_tmp32F,_tmp32F + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp330=yy1;struct Cyc_Core_Opt*_tmp331;
_LL1F3: if((_tmp330.YY49).tag != 55)goto _LL1F5;_tmp331=(struct Cyc_Core_Opt*)(
_tmp330.YY49).val;_LL1F4: yyzzz=_tmp331;goto _LL1F2;_LL1F5:;_LL1F6:(int)_throw((
void*)& Cyc_yyfail_YY49);_LL1F2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*
yy1){union Cyc_YYSTYPE _tmpA20;return((_tmpA20.YY49).val=yy1,(((_tmpA20.YY49).tag=
55,_tmpA20)));}static char _tmp334[31]="list_t<$(type_t,type_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp334,_tmp334,
_tmp334 + 31}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp335=yy1;struct Cyc_List_List*_tmp336;_LL1F8: if((
_tmp335.YY50).tag != 56)goto _LL1FA;_tmp336=(struct Cyc_List_List*)(_tmp335.YY50).val;
_LL1F9: yyzzz=_tmp336;goto _LL1F7;_LL1FA:;_LL1FB:(int)_throw((void*)& Cyc_yyfail_YY50);
_LL1F7:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmpA21;return((_tmpA21.YY50).val=yy1,(((_tmpA21.YY50).tag=56,_tmpA21)));}static
char _tmp339[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY51={
Cyc_Core_Failure,{_tmp339,_tmp339,_tmp339 + 15}};union Cyc_Absyn_Constraint*Cyc_yyget_YY51(
union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp33A=
yy1;union Cyc_Absyn_Constraint*_tmp33B;_LL1FD: if((_tmp33A.YY51).tag != 57)goto
_LL1FF;_tmp33B=(union Cyc_Absyn_Constraint*)(_tmp33A.YY51).val;_LL1FE: yyzzz=
_tmp33B;goto _LL1FC;_LL1FF:;_LL200:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1FC:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE
_tmpA22;return((_tmpA22.YY51).val=yy1,(((_tmpA22.YY51).tag=57,_tmpA22)));}static
char _tmp33E[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp33E,_tmp33E,_tmp33E + 45}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp33F=yy1;struct Cyc_List_List*_tmp340;_LL202: if((_tmp33F.YY52).tag != 58)goto
_LL204;_tmp340=(struct Cyc_List_List*)(_tmp33F.YY52).val;_LL203: yyzzz=_tmp340;
goto _LL201;_LL204:;_LL205:(int)_throw((void*)& Cyc_yyfail_YY52);_LL201:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA23;
return((_tmpA23.YY52).val=yy1,(((_tmpA23.YY52).tag=58,_tmpA23)));}static char
_tmp343[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp343,_tmp343,_tmp343 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp344=yy1;void*_tmp345;_LL207: if((_tmp344.YY53).tag
!= 59)goto _LL209;_tmp345=(void*)(_tmp344.YY53).val;_LL208: yyzzz=_tmp345;goto
_LL206;_LL209:;_LL20A:(int)_throw((void*)& Cyc_yyfail_YY53);_LL206:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmpA24;return((_tmpA24.YY53).val=
yy1,(((_tmpA24.YY53).tag=59,_tmpA24)));}static char _tmp348[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp348,
_tmp348,_tmp348 + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp349=yy1;struct Cyc_List_List*
_tmp34A;_LL20C: if((_tmp349.YY54).tag != 60)goto _LL20E;_tmp34A=(struct Cyc_List_List*)(
_tmp349.YY54).val;_LL20D: yyzzz=_tmp34A;goto _LL20B;_LL20E:;_LL20F:(int)_throw((
void*)& Cyc_yyfail_YY54);_LL20B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmpA25;return((_tmpA25.YY54).val=yy1,(((_tmpA25.YY54).tag=
60,_tmpA25)));}static char _tmp34D[21]="$(bool,string_t<`H>)";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp34D,_tmp34D,_tmp34D + 21}};struct
_tuple18 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){struct _tuple18 yyzzz;{union Cyc_YYSTYPE
_tmp34E=yy1;struct _tuple18 _tmp34F;_LL211: if((_tmp34E.Asm_tok).tag != 6)goto _LL213;
_tmp34F=(struct _tuple18)(_tmp34E.Asm_tok).val;_LL212: yyzzz=_tmp34F;goto _LL210;
_LL213:;_LL214:(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL210:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple18 yy1){union Cyc_YYSTYPE _tmpA26;return((
_tmpA26.Asm_tok).val=yy1,(((_tmpA26.Asm_tok).tag=6,_tmpA26)));}struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};typedef
struct Cyc_Yyltype Cyc_yyltype;struct Cyc_Yyltype Cyc_yynewloc(){struct Cyc_Yyltype
_tmpA27;return(_tmpA27.timestamp=0,((_tmpA27.first_line=0,((_tmpA27.first_column=
0,((_tmpA27.last_line=0,((_tmpA27.last_column=0,_tmpA27)))))))));}struct Cyc_Yyltype
Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[374]={0,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,145,2,2,129,143,140,2,126,127,134,137,
122,141,131,142,2,2,2,2,2,2,2,2,2,2,130,119,124,123,125,136,135,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,133,139,128,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,120,138,121,144,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,
37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,
91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,
113,114,115,116,117,118};static char _tmp352[2]="$";static char _tmp353[6]="error";
static char _tmp354[12]="$undefined.";static char _tmp355[5]="AUTO";static char
_tmp356[9]="REGISTER";static char _tmp357[7]="STATIC";static char _tmp358[7]="EXTERN";
static char _tmp359[8]="TYPEDEF";static char _tmp35A[5]="VOID";static char _tmp35B[5]="CHAR";
static char _tmp35C[6]="SHORT";static char _tmp35D[4]="INT";static char _tmp35E[5]="LONG";
static char _tmp35F[6]="FLOAT";static char _tmp360[7]="DOUBLE";static char _tmp361[7]="SIGNED";
static char _tmp362[9]="UNSIGNED";static char _tmp363[6]="CONST";static char _tmp364[9]="VOLATILE";
static char _tmp365[9]="RESTRICT";static char _tmp366[7]="STRUCT";static char _tmp367[
6]="UNION";static char _tmp368[5]="CASE";static char _tmp369[8]="DEFAULT";static char
_tmp36A[7]="INLINE";static char _tmp36B[7]="SIZEOF";static char _tmp36C[9]="OFFSETOF";
static char _tmp36D[3]="IF";static char _tmp36E[5]="ELSE";static char _tmp36F[7]="SWITCH";
static char _tmp370[6]="WHILE";static char _tmp371[3]="DO";static char _tmp372[4]="FOR";
static char _tmp373[5]="GOTO";static char _tmp374[9]="CONTINUE";static char _tmp375[6]="BREAK";
static char _tmp376[7]="RETURN";static char _tmp377[5]="ENUM";static char _tmp378[8]="NULL_kw";
static char _tmp379[4]="LET";static char _tmp37A[6]="THROW";static char _tmp37B[4]="TRY";
static char _tmp37C[6]="CATCH";static char _tmp37D[7]="EXPORT";static char _tmp37E[4]="NEW";
static char _tmp37F[9]="ABSTRACT";static char _tmp380[9]="FALLTHRU";static char
_tmp381[6]="USING";static char _tmp382[10]="NAMESPACE";static char _tmp383[9]="DATATYPE";
static char _tmp384[8]="XTUNION";static char _tmp385[7]="TUNION";static char _tmp386[7]="MALLOC";
static char _tmp387[8]="RMALLOC";static char _tmp388[7]="CALLOC";static char _tmp389[8]="RCALLOC";
static char _tmp38A[5]="SWAP";static char _tmp38B[9]="REGION_T";static char _tmp38C[6]="TAG_T";
static char _tmp38D[7]="REGION";static char _tmp38E[5]="RNEW";static char _tmp38F[8]="REGIONS";
static char _tmp390[13]="RESET_REGION";static char _tmp391[16]="NOZEROTERM_QUAL";
static char _tmp392[14]="ZEROTERM_QUAL";static char _tmp393[12]="REGION_QUAL";static
char _tmp394[7]="PORTON";static char _tmp395[8]="PORTOFF";static char _tmp396[12]="DYNREGION_T";
static char _tmp397[6]="ALIAS";static char _tmp398[8]="NUMELTS";static char _tmp399[8]="VALUEOF";
static char _tmp39A[10]="VALUEOF_T";static char _tmp39B[9]="TAGCHECK";static char
_tmp39C[13]="NUMELTS_QUAL";static char _tmp39D[10]="THIN_QUAL";static char _tmp39E[9]="FAT_QUAL";
static char _tmp39F[13]="NOTNULL_QUAL";static char _tmp3A0[14]="NULLABLE_QUAL";
static char _tmp3A1[12]="TAGGED_QUAL";static char _tmp3A2[16]="EXTENSIBLE_QUAL";
static char _tmp3A3[15]="RESETABLE_QUAL";static char _tmp3A4[7]="PTR_OP";static char
_tmp3A5[7]="INC_OP";static char _tmp3A6[7]="DEC_OP";static char _tmp3A7[8]="LEFT_OP";
static char _tmp3A8[9]="RIGHT_OP";static char _tmp3A9[6]="LE_OP";static char _tmp3AA[6]="GE_OP";
static char _tmp3AB[6]="EQ_OP";static char _tmp3AC[6]="NE_OP";static char _tmp3AD[7]="AND_OP";
static char _tmp3AE[6]="OR_OP";static char _tmp3AF[11]="MUL_ASSIGN";static char
_tmp3B0[11]="DIV_ASSIGN";static char _tmp3B1[11]="MOD_ASSIGN";static char _tmp3B2[11]="ADD_ASSIGN";
static char _tmp3B3[11]="SUB_ASSIGN";static char _tmp3B4[12]="LEFT_ASSIGN";static
char _tmp3B5[13]="RIGHT_ASSIGN";static char _tmp3B6[11]="AND_ASSIGN";static char
_tmp3B7[11]="XOR_ASSIGN";static char _tmp3B8[10]="OR_ASSIGN";static char _tmp3B9[9]="ELLIPSIS";
static char _tmp3BA[11]="LEFT_RIGHT";static char _tmp3BB[12]="COLON_COLON";static
char _tmp3BC[11]="IDENTIFIER";static char _tmp3BD[17]="INTEGER_CONSTANT";static char
_tmp3BE[7]="STRING";static char _tmp3BF[8]="WSTRING";static char _tmp3C0[19]="CHARACTER_CONSTANT";
static char _tmp3C1[20]="WCHARACTER_CONSTANT";static char _tmp3C2[18]="FLOATING_CONSTANT";
static char _tmp3C3[9]="TYPE_VAR";static char _tmp3C4[13]="TYPEDEF_NAME";static char
_tmp3C5[16]="QUAL_IDENTIFIER";static char _tmp3C6[18]="QUAL_TYPEDEF_NAME";static
char _tmp3C7[10]="ATTRIBUTE";static char _tmp3C8[4]="ASM";static char _tmp3C9[4]="';'";
static char _tmp3CA[4]="'{'";static char _tmp3CB[4]="'}'";static char _tmp3CC[4]="','";
static char _tmp3CD[4]="'='";static char _tmp3CE[4]="'<'";static char _tmp3CF[4]="'>'";
static char _tmp3D0[4]="'('";static char _tmp3D1[4]="')'";static char _tmp3D2[4]="'_'";
static char _tmp3D3[4]="'$'";static char _tmp3D4[4]="':'";static char _tmp3D5[4]="'.'";
static char _tmp3D6[4]="'['";static char _tmp3D7[4]="']'";static char _tmp3D8[4]="'*'";
static char _tmp3D9[4]="'@'";static char _tmp3DA[4]="'?'";static char _tmp3DB[4]="'+'";
static char _tmp3DC[4]="'|'";static char _tmp3DD[4]="'^'";static char _tmp3DE[4]="'&'";
static char _tmp3DF[4]="'-'";static char _tmp3E0[4]="'/'";static char _tmp3E1[4]="'%'";
static char _tmp3E2[4]="'~'";static char _tmp3E3[4]="'!'";static char _tmp3E4[5]="prog";
static char _tmp3E5[17]="translation_unit";static char _tmp3E6[12]="export_list";
static char _tmp3E7[19]="export_list_values";static char _tmp3E8[21]="external_declaration";
static char _tmp3E9[20]="function_definition";static char _tmp3EA[21]="function_definition2";
static char _tmp3EB[13]="using_action";static char _tmp3EC[15]="unusing_action";
static char _tmp3ED[17]="namespace_action";static char _tmp3EE[19]="unnamespace_action";
static char _tmp3EF[12]="declaration";static char _tmp3F0[19]="resetable_qual_opt";
static char _tmp3F1[17]="declaration_list";static char _tmp3F2[23]="declaration_specifiers";
static char _tmp3F3[24]="storage_class_specifier";static char _tmp3F4[15]="attributes_opt";
static char _tmp3F5[11]="attributes";static char _tmp3F6[15]="attribute_list";static
char _tmp3F7[10]="attribute";static char _tmp3F8[15]="type_specifier";static char
_tmp3F9[25]="type_specifier_notypedef";static char _tmp3FA[5]="kind";static char
_tmp3FB[15]="type_qualifier";static char _tmp3FC[15]="enum_specifier";static char
_tmp3FD[11]="enum_field";static char _tmp3FE[22]="enum_declaration_list";static
char _tmp3FF[26]="struct_or_union_specifier";static char _tmp400[16]="type_params_opt";
static char _tmp401[16]="struct_or_union";static char _tmp402[24]="struct_declaration_list";
static char _tmp403[25]="struct_declaration_list0";static char _tmp404[21]="init_declarator_list";
static char _tmp405[22]="init_declarator_list0";static char _tmp406[16]="init_declarator";
static char _tmp407[19]="struct_declaration";static char _tmp408[25]="specifier_qualifier_list";
static char _tmp409[35]="notypedef_specifier_qualifier_list";static char _tmp40A[23]="struct_declarator_list";
static char _tmp40B[24]="struct_declarator_list0";static char _tmp40C[18]="struct_declarator";
static char _tmp40D[19]="datatype_specifier";static char _tmp40E[14]="qual_datatype";
static char _tmp40F[19]="datatypefield_list";static char _tmp410[20]="datatypefield_scope";
static char _tmp411[14]="datatypefield";static char _tmp412[11]="declarator";static
char _tmp413[23]="declarator_withtypedef";static char _tmp414[18]="direct_declarator";
static char _tmp415[30]="direct_declarator_withtypedef";static char _tmp416[8]="pointer";
static char _tmp417[12]="one_pointer";static char _tmp418[14]="pointer_quals";static
char _tmp419[13]="pointer_qual";static char _tmp41A[23]="pointer_null_and_bound";
static char _tmp41B[14]="pointer_bound";static char _tmp41C[18]="zeroterm_qual_opt";
static char _tmp41D[8]="rgn_opt";static char _tmp41E[11]="tqual_list";static char
_tmp41F[20]="parameter_type_list";static char _tmp420[9]="type_var";static char
_tmp421[16]="optional_effect";static char _tmp422[19]="optional_rgn_order";static
char _tmp423[10]="rgn_order";static char _tmp424[16]="optional_inject";static char
_tmp425[11]="effect_set";static char _tmp426[14]="atomic_effect";static char _tmp427[
11]="region_set";static char _tmp428[15]="parameter_list";static char _tmp429[22]="parameter_declaration";
static char _tmp42A[16]="identifier_list";static char _tmp42B[17]="identifier_list0";
static char _tmp42C[12]="initializer";static char _tmp42D[18]="array_initializer";
static char _tmp42E[17]="initializer_list";static char _tmp42F[12]="designation";
static char _tmp430[16]="designator_list";static char _tmp431[11]="designator";
static char _tmp432[10]="type_name";static char _tmp433[14]="any_type_name";static
char _tmp434[15]="type_name_list";static char _tmp435[20]="abstract_declarator";
static char _tmp436[27]="direct_abstract_declarator";static char _tmp437[10]="statement";
static char _tmp438[13]="open_exp_opt";static char _tmp439[18]="labeled_statement";
static char _tmp43A[21]="expression_statement";static char _tmp43B[19]="compound_statement";
static char _tmp43C[16]="block_item_list";static char _tmp43D[20]="selection_statement";
static char _tmp43E[15]="switch_clauses";static char _tmp43F[20]="iteration_statement";
static char _tmp440[15]="jump_statement";static char _tmp441[12]="exp_pattern";
static char _tmp442[20]="conditional_pattern";static char _tmp443[19]="logical_or_pattern";
static char _tmp444[20]="logical_and_pattern";static char _tmp445[21]="inclusive_or_pattern";
static char _tmp446[21]="exclusive_or_pattern";static char _tmp447[12]="and_pattern";
static char _tmp448[17]="equality_pattern";static char _tmp449[19]="relational_pattern";
static char _tmp44A[14]="shift_pattern";static char _tmp44B[17]="additive_pattern";
static char _tmp44C[23]="multiplicative_pattern";static char _tmp44D[13]="cast_pattern";
static char _tmp44E[14]="unary_pattern";static char _tmp44F[16]="postfix_pattern";
static char _tmp450[16]="primary_pattern";static char _tmp451[8]="pattern";static
char _tmp452[19]="tuple_pattern_list";static char _tmp453[20]="tuple_pattern_list0";
static char _tmp454[14]="field_pattern";static char _tmp455[19]="field_pattern_list";
static char _tmp456[20]="field_pattern_list0";static char _tmp457[11]="expression";
static char _tmp458[22]="assignment_expression";static char _tmp459[20]="assignment_operator";
static char _tmp45A[23]="conditional_expression";static char _tmp45B[20]="constant_expression";
static char _tmp45C[22]="logical_or_expression";static char _tmp45D[23]="logical_and_expression";
static char _tmp45E[24]="inclusive_or_expression";static char _tmp45F[24]="exclusive_or_expression";
static char _tmp460[15]="and_expression";static char _tmp461[20]="equality_expression";
static char _tmp462[22]="relational_expression";static char _tmp463[17]="shift_expression";
static char _tmp464[20]="additive_expression";static char _tmp465[26]="multiplicative_expression";
static char _tmp466[16]="cast_expression";static char _tmp467[17]="unary_expression";
static char _tmp468[15]="unary_operator";static char _tmp469[19]="postfix_expression";
static char _tmp46A[19]="primary_expression";static char _tmp46B[25]="argument_expression_list";
static char _tmp46C[26]="argument_expression_list0";static char _tmp46D[9]="constant";
static char _tmp46E[20]="qual_opt_identifier";static char _tmp46F[17]="qual_opt_typedef";
static char _tmp470[18]="struct_union_name";static char _tmp471[11]="field_name";
static char _tmp472[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[289]={{
_tmp352,_tmp352,_tmp352 + 2},{_tmp353,_tmp353,_tmp353 + 6},{_tmp354,_tmp354,
_tmp354 + 12},{_tmp355,_tmp355,_tmp355 + 5},{_tmp356,_tmp356,_tmp356 + 9},{_tmp357,
_tmp357,_tmp357 + 7},{_tmp358,_tmp358,_tmp358 + 7},{_tmp359,_tmp359,_tmp359 + 8},{
_tmp35A,_tmp35A,_tmp35A + 5},{_tmp35B,_tmp35B,_tmp35B + 5},{_tmp35C,_tmp35C,
_tmp35C + 6},{_tmp35D,_tmp35D,_tmp35D + 4},{_tmp35E,_tmp35E,_tmp35E + 5},{_tmp35F,
_tmp35F,_tmp35F + 6},{_tmp360,_tmp360,_tmp360 + 7},{_tmp361,_tmp361,_tmp361 + 7},{
_tmp362,_tmp362,_tmp362 + 9},{_tmp363,_tmp363,_tmp363 + 6},{_tmp364,_tmp364,
_tmp364 + 9},{_tmp365,_tmp365,_tmp365 + 9},{_tmp366,_tmp366,_tmp366 + 7},{_tmp367,
_tmp367,_tmp367 + 6},{_tmp368,_tmp368,_tmp368 + 5},{_tmp369,_tmp369,_tmp369 + 8},{
_tmp36A,_tmp36A,_tmp36A + 7},{_tmp36B,_tmp36B,_tmp36B + 7},{_tmp36C,_tmp36C,
_tmp36C + 9},{_tmp36D,_tmp36D,_tmp36D + 3},{_tmp36E,_tmp36E,_tmp36E + 5},{_tmp36F,
_tmp36F,_tmp36F + 7},{_tmp370,_tmp370,_tmp370 + 6},{_tmp371,_tmp371,_tmp371 + 3},{
_tmp372,_tmp372,_tmp372 + 4},{_tmp373,_tmp373,_tmp373 + 5},{_tmp374,_tmp374,
_tmp374 + 9},{_tmp375,_tmp375,_tmp375 + 6},{_tmp376,_tmp376,_tmp376 + 7},{_tmp377,
_tmp377,_tmp377 + 5},{_tmp378,_tmp378,_tmp378 + 8},{_tmp379,_tmp379,_tmp379 + 4},{
_tmp37A,_tmp37A,_tmp37A + 6},{_tmp37B,_tmp37B,_tmp37B + 4},{_tmp37C,_tmp37C,
_tmp37C + 6},{_tmp37D,_tmp37D,_tmp37D + 7},{_tmp37E,_tmp37E,_tmp37E + 4},{_tmp37F,
_tmp37F,_tmp37F + 9},{_tmp380,_tmp380,_tmp380 + 9},{_tmp381,_tmp381,_tmp381 + 6},{
_tmp382,_tmp382,_tmp382 + 10},{_tmp383,_tmp383,_tmp383 + 9},{_tmp384,_tmp384,
_tmp384 + 8},{_tmp385,_tmp385,_tmp385 + 7},{_tmp386,_tmp386,_tmp386 + 7},{_tmp387,
_tmp387,_tmp387 + 8},{_tmp388,_tmp388,_tmp388 + 7},{_tmp389,_tmp389,_tmp389 + 8},{
_tmp38A,_tmp38A,_tmp38A + 5},{_tmp38B,_tmp38B,_tmp38B + 9},{_tmp38C,_tmp38C,
_tmp38C + 6},{_tmp38D,_tmp38D,_tmp38D + 7},{_tmp38E,_tmp38E,_tmp38E + 5},{_tmp38F,
_tmp38F,_tmp38F + 8},{_tmp390,_tmp390,_tmp390 + 13},{_tmp391,_tmp391,_tmp391 + 16},{
_tmp392,_tmp392,_tmp392 + 14},{_tmp393,_tmp393,_tmp393 + 12},{_tmp394,_tmp394,
_tmp394 + 7},{_tmp395,_tmp395,_tmp395 + 8},{_tmp396,_tmp396,_tmp396 + 12},{_tmp397,
_tmp397,_tmp397 + 6},{_tmp398,_tmp398,_tmp398 + 8},{_tmp399,_tmp399,_tmp399 + 8},{
_tmp39A,_tmp39A,_tmp39A + 10},{_tmp39B,_tmp39B,_tmp39B + 9},{_tmp39C,_tmp39C,
_tmp39C + 13},{_tmp39D,_tmp39D,_tmp39D + 10},{_tmp39E,_tmp39E,_tmp39E + 9},{_tmp39F,
_tmp39F,_tmp39F + 13},{_tmp3A0,_tmp3A0,_tmp3A0 + 14},{_tmp3A1,_tmp3A1,_tmp3A1 + 12},{
_tmp3A2,_tmp3A2,_tmp3A2 + 16},{_tmp3A3,_tmp3A3,_tmp3A3 + 15},{_tmp3A4,_tmp3A4,
_tmp3A4 + 7},{_tmp3A5,_tmp3A5,_tmp3A5 + 7},{_tmp3A6,_tmp3A6,_tmp3A6 + 7},{_tmp3A7,
_tmp3A7,_tmp3A7 + 8},{_tmp3A8,_tmp3A8,_tmp3A8 + 9},{_tmp3A9,_tmp3A9,_tmp3A9 + 6},{
_tmp3AA,_tmp3AA,_tmp3AA + 6},{_tmp3AB,_tmp3AB,_tmp3AB + 6},{_tmp3AC,_tmp3AC,
_tmp3AC + 6},{_tmp3AD,_tmp3AD,_tmp3AD + 7},{_tmp3AE,_tmp3AE,_tmp3AE + 6},{_tmp3AF,
_tmp3AF,_tmp3AF + 11},{_tmp3B0,_tmp3B0,_tmp3B0 + 11},{_tmp3B1,_tmp3B1,_tmp3B1 + 11},{
_tmp3B2,_tmp3B2,_tmp3B2 + 11},{_tmp3B3,_tmp3B3,_tmp3B3 + 11},{_tmp3B4,_tmp3B4,
_tmp3B4 + 12},{_tmp3B5,_tmp3B5,_tmp3B5 + 13},{_tmp3B6,_tmp3B6,_tmp3B6 + 11},{
_tmp3B7,_tmp3B7,_tmp3B7 + 11},{_tmp3B8,_tmp3B8,_tmp3B8 + 10},{_tmp3B9,_tmp3B9,
_tmp3B9 + 9},{_tmp3BA,_tmp3BA,_tmp3BA + 11},{_tmp3BB,_tmp3BB,_tmp3BB + 12},{_tmp3BC,
_tmp3BC,_tmp3BC + 11},{_tmp3BD,_tmp3BD,_tmp3BD + 17},{_tmp3BE,_tmp3BE,_tmp3BE + 7},{
_tmp3BF,_tmp3BF,_tmp3BF + 8},{_tmp3C0,_tmp3C0,_tmp3C0 + 19},{_tmp3C1,_tmp3C1,
_tmp3C1 + 20},{_tmp3C2,_tmp3C2,_tmp3C2 + 18},{_tmp3C3,_tmp3C3,_tmp3C3 + 9},{_tmp3C4,
_tmp3C4,_tmp3C4 + 13},{_tmp3C5,_tmp3C5,_tmp3C5 + 16},{_tmp3C6,_tmp3C6,_tmp3C6 + 18},{
_tmp3C7,_tmp3C7,_tmp3C7 + 10},{_tmp3C8,_tmp3C8,_tmp3C8 + 4},{_tmp3C9,_tmp3C9,
_tmp3C9 + 4},{_tmp3CA,_tmp3CA,_tmp3CA + 4},{_tmp3CB,_tmp3CB,_tmp3CB + 4},{_tmp3CC,
_tmp3CC,_tmp3CC + 4},{_tmp3CD,_tmp3CD,_tmp3CD + 4},{_tmp3CE,_tmp3CE,_tmp3CE + 4},{
_tmp3CF,_tmp3CF,_tmp3CF + 4},{_tmp3D0,_tmp3D0,_tmp3D0 + 4},{_tmp3D1,_tmp3D1,
_tmp3D1 + 4},{_tmp3D2,_tmp3D2,_tmp3D2 + 4},{_tmp3D3,_tmp3D3,_tmp3D3 + 4},{_tmp3D4,
_tmp3D4,_tmp3D4 + 4},{_tmp3D5,_tmp3D5,_tmp3D5 + 4},{_tmp3D6,_tmp3D6,_tmp3D6 + 4},{
_tmp3D7,_tmp3D7,_tmp3D7 + 4},{_tmp3D8,_tmp3D8,_tmp3D8 + 4},{_tmp3D9,_tmp3D9,
_tmp3D9 + 4},{_tmp3DA,_tmp3DA,_tmp3DA + 4},{_tmp3DB,_tmp3DB,_tmp3DB + 4},{_tmp3DC,
_tmp3DC,_tmp3DC + 4},{_tmp3DD,_tmp3DD,_tmp3DD + 4},{_tmp3DE,_tmp3DE,_tmp3DE + 4},{
_tmp3DF,_tmp3DF,_tmp3DF + 4},{_tmp3E0,_tmp3E0,_tmp3E0 + 4},{_tmp3E1,_tmp3E1,
_tmp3E1 + 4},{_tmp3E2,_tmp3E2,_tmp3E2 + 4},{_tmp3E3,_tmp3E3,_tmp3E3 + 4},{_tmp3E4,
_tmp3E4,_tmp3E4 + 5},{_tmp3E5,_tmp3E5,_tmp3E5 + 17},{_tmp3E6,_tmp3E6,_tmp3E6 + 12},{
_tmp3E7,_tmp3E7,_tmp3E7 + 19},{_tmp3E8,_tmp3E8,_tmp3E8 + 21},{_tmp3E9,_tmp3E9,
_tmp3E9 + 20},{_tmp3EA,_tmp3EA,_tmp3EA + 21},{_tmp3EB,_tmp3EB,_tmp3EB + 13},{
_tmp3EC,_tmp3EC,_tmp3EC + 15},{_tmp3ED,_tmp3ED,_tmp3ED + 17},{_tmp3EE,_tmp3EE,
_tmp3EE + 19},{_tmp3EF,_tmp3EF,_tmp3EF + 12},{_tmp3F0,_tmp3F0,_tmp3F0 + 19},{
_tmp3F1,_tmp3F1,_tmp3F1 + 17},{_tmp3F2,_tmp3F2,_tmp3F2 + 23},{_tmp3F3,_tmp3F3,
_tmp3F3 + 24},{_tmp3F4,_tmp3F4,_tmp3F4 + 15},{_tmp3F5,_tmp3F5,_tmp3F5 + 11},{
_tmp3F6,_tmp3F6,_tmp3F6 + 15},{_tmp3F7,_tmp3F7,_tmp3F7 + 10},{_tmp3F8,_tmp3F8,
_tmp3F8 + 15},{_tmp3F9,_tmp3F9,_tmp3F9 + 25},{_tmp3FA,_tmp3FA,_tmp3FA + 5},{_tmp3FB,
_tmp3FB,_tmp3FB + 15},{_tmp3FC,_tmp3FC,_tmp3FC + 15},{_tmp3FD,_tmp3FD,_tmp3FD + 11},{
_tmp3FE,_tmp3FE,_tmp3FE + 22},{_tmp3FF,_tmp3FF,_tmp3FF + 26},{_tmp400,_tmp400,
_tmp400 + 16},{_tmp401,_tmp401,_tmp401 + 16},{_tmp402,_tmp402,_tmp402 + 24},{
_tmp403,_tmp403,_tmp403 + 25},{_tmp404,_tmp404,_tmp404 + 21},{_tmp405,_tmp405,
_tmp405 + 22},{_tmp406,_tmp406,_tmp406 + 16},{_tmp407,_tmp407,_tmp407 + 19},{
_tmp408,_tmp408,_tmp408 + 25},{_tmp409,_tmp409,_tmp409 + 35},{_tmp40A,_tmp40A,
_tmp40A + 23},{_tmp40B,_tmp40B,_tmp40B + 24},{_tmp40C,_tmp40C,_tmp40C + 18},{
_tmp40D,_tmp40D,_tmp40D + 19},{_tmp40E,_tmp40E,_tmp40E + 14},{_tmp40F,_tmp40F,
_tmp40F + 19},{_tmp410,_tmp410,_tmp410 + 20},{_tmp411,_tmp411,_tmp411 + 14},{
_tmp412,_tmp412,_tmp412 + 11},{_tmp413,_tmp413,_tmp413 + 23},{_tmp414,_tmp414,
_tmp414 + 18},{_tmp415,_tmp415,_tmp415 + 30},{_tmp416,_tmp416,_tmp416 + 8},{_tmp417,
_tmp417,_tmp417 + 12},{_tmp418,_tmp418,_tmp418 + 14},{_tmp419,_tmp419,_tmp419 + 13},{
_tmp41A,_tmp41A,_tmp41A + 23},{_tmp41B,_tmp41B,_tmp41B + 14},{_tmp41C,_tmp41C,
_tmp41C + 18},{_tmp41D,_tmp41D,_tmp41D + 8},{_tmp41E,_tmp41E,_tmp41E + 11},{_tmp41F,
_tmp41F,_tmp41F + 20},{_tmp420,_tmp420,_tmp420 + 9},{_tmp421,_tmp421,_tmp421 + 16},{
_tmp422,_tmp422,_tmp422 + 19},{_tmp423,_tmp423,_tmp423 + 10},{_tmp424,_tmp424,
_tmp424 + 16},{_tmp425,_tmp425,_tmp425 + 11},{_tmp426,_tmp426,_tmp426 + 14},{
_tmp427,_tmp427,_tmp427 + 11},{_tmp428,_tmp428,_tmp428 + 15},{_tmp429,_tmp429,
_tmp429 + 22},{_tmp42A,_tmp42A,_tmp42A + 16},{_tmp42B,_tmp42B,_tmp42B + 17},{
_tmp42C,_tmp42C,_tmp42C + 12},{_tmp42D,_tmp42D,_tmp42D + 18},{_tmp42E,_tmp42E,
_tmp42E + 17},{_tmp42F,_tmp42F,_tmp42F + 12},{_tmp430,_tmp430,_tmp430 + 16},{
_tmp431,_tmp431,_tmp431 + 11},{_tmp432,_tmp432,_tmp432 + 10},{_tmp433,_tmp433,
_tmp433 + 14},{_tmp434,_tmp434,_tmp434 + 15},{_tmp435,_tmp435,_tmp435 + 20},{
_tmp436,_tmp436,_tmp436 + 27},{_tmp437,_tmp437,_tmp437 + 10},{_tmp438,_tmp438,
_tmp438 + 13},{_tmp439,_tmp439,_tmp439 + 18},{_tmp43A,_tmp43A,_tmp43A + 21},{
_tmp43B,_tmp43B,_tmp43B + 19},{_tmp43C,_tmp43C,_tmp43C + 16},{_tmp43D,_tmp43D,
_tmp43D + 20},{_tmp43E,_tmp43E,_tmp43E + 15},{_tmp43F,_tmp43F,_tmp43F + 20},{
_tmp440,_tmp440,_tmp440 + 15},{_tmp441,_tmp441,_tmp441 + 12},{_tmp442,_tmp442,
_tmp442 + 20},{_tmp443,_tmp443,_tmp443 + 19},{_tmp444,_tmp444,_tmp444 + 20},{
_tmp445,_tmp445,_tmp445 + 21},{_tmp446,_tmp446,_tmp446 + 21},{_tmp447,_tmp447,
_tmp447 + 12},{_tmp448,_tmp448,_tmp448 + 17},{_tmp449,_tmp449,_tmp449 + 19},{
_tmp44A,_tmp44A,_tmp44A + 14},{_tmp44B,_tmp44B,_tmp44B + 17},{_tmp44C,_tmp44C,
_tmp44C + 23},{_tmp44D,_tmp44D,_tmp44D + 13},{_tmp44E,_tmp44E,_tmp44E + 14},{
_tmp44F,_tmp44F,_tmp44F + 16},{_tmp450,_tmp450,_tmp450 + 16},{_tmp451,_tmp451,
_tmp451 + 8},{_tmp452,_tmp452,_tmp452 + 19},{_tmp453,_tmp453,_tmp453 + 20},{_tmp454,
_tmp454,_tmp454 + 14},{_tmp455,_tmp455,_tmp455 + 19},{_tmp456,_tmp456,_tmp456 + 20},{
_tmp457,_tmp457,_tmp457 + 11},{_tmp458,_tmp458,_tmp458 + 22},{_tmp459,_tmp459,
_tmp459 + 20},{_tmp45A,_tmp45A,_tmp45A + 23},{_tmp45B,_tmp45B,_tmp45B + 20},{
_tmp45C,_tmp45C,_tmp45C + 22},{_tmp45D,_tmp45D,_tmp45D + 23},{_tmp45E,_tmp45E,
_tmp45E + 24},{_tmp45F,_tmp45F,_tmp45F + 24},{_tmp460,_tmp460,_tmp460 + 15},{
_tmp461,_tmp461,_tmp461 + 20},{_tmp462,_tmp462,_tmp462 + 22},{_tmp463,_tmp463,
_tmp463 + 17},{_tmp464,_tmp464,_tmp464 + 20},{_tmp465,_tmp465,_tmp465 + 26},{
_tmp466,_tmp466,_tmp466 + 16},{_tmp467,_tmp467,_tmp467 + 17},{_tmp468,_tmp468,
_tmp468 + 15},{_tmp469,_tmp469,_tmp469 + 19},{_tmp46A,_tmp46A,_tmp46A + 19},{
_tmp46B,_tmp46B,_tmp46B + 25},{_tmp46C,_tmp46C,_tmp46C + 26},{_tmp46D,_tmp46D,
_tmp46D + 9},{_tmp46E,_tmp46E,_tmp46E + 20},{_tmp46F,_tmp46F,_tmp46F + 17},{_tmp470,
_tmp470,_tmp470 + 18},{_tmp471,_tmp471,_tmp471 + 11},{_tmp472,_tmp472,_tmp472 + 12}};
static short Cyc_yyr1[515]={0,146,147,147,147,147,147,147,147,147,147,147,148,148,
149,149,149,150,150,150,151,151,151,151,152,152,153,154,155,156,157,157,157,157,
157,157,157,158,158,159,159,160,160,160,160,160,160,160,160,160,160,161,161,161,
161,161,161,161,162,162,163,164,164,165,165,165,165,165,165,166,166,167,167,167,
167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,
168,169,169,169,170,170,170,171,171,172,172,172,173,173,173,173,173,174,174,175,
175,176,176,177,177,178,179,179,180,180,181,182,182,182,182,182,182,183,183,183,
183,183,183,184,185,185,186,186,186,186,187,187,187,188,188,189,189,189,189,190,
190,190,191,191,192,192,193,193,194,194,194,194,194,194,194,194,194,194,195,195,
195,195,195,195,195,195,195,195,195,196,196,197,198,198,199,199,199,199,199,199,
199,199,200,200,200,201,201,202,202,202,203,203,203,204,204,205,205,205,205,206,
206,207,207,208,208,209,209,210,210,211,211,212,212,212,212,213,213,214,214,215,
215,215,216,217,217,218,218,219,219,219,219,220,220,220,220,221,222,222,223,223,
224,224,225,225,225,225,225,226,226,227,227,227,228,228,228,228,228,228,228,228,
228,228,228,229,229,229,229,229,229,229,230,230,231,232,232,233,233,234,234,234,
234,234,234,235,235,235,235,235,235,236,236,236,236,236,236,237,237,237,237,237,
237,237,237,237,237,237,237,237,237,238,238,238,238,238,238,238,238,239,240,240,
241,241,242,242,243,243,244,244,245,245,246,246,246,247,247,247,247,247,248,248,
248,249,249,249,250,250,250,250,251,251,252,252,252,252,252,252,252,253,254,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,256,256,256,257,257,258,
258,259,259,259,260,260,261,261,262,262,262,263,263,263,263,263,263,263,263,263,
263,263,264,264,264,264,264,264,264,265,266,266,267,267,268,268,269,269,270,270,
271,271,271,272,272,272,272,272,273,273,273,274,274,274,275,275,275,275,276,276,
277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,
278,278,278,279,279,279,279,279,279,279,279,279,279,280,280,280,280,280,280,280,
280,280,280,281,282,282,283,283,283,283,283,284,284,285,285,286,286,287,287,288,
288};static short Cyc_yyr2[515]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,1,2,3,1,1,1,2,3,3,4,3,
4,2,1,2,1,2,3,5,3,7,5,8,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,
4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,
3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,0,3,6,
3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,
5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,
2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,
2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,
4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,
3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,4,2,2,2,
4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,
3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,
4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[1030]={0,19,51,52,53,54,56,71,74,75,
76,77,78,79,80,81,95,96,97,113,114,47,0,0,57,0,0,147,88,92,0,0,0,0,0,0,0,38,505,
213,507,506,508,0,0,72,0,199,199,198,1,0,17,0,0,18,0,0,41,49,43,69,45,82,83,0,84,
0,37,158,0,183,186,85,162,111,55,54,48,0,99,504,0,505,500,501,502,503,111,0,373,
0,0,0,0,236,0,375,376,26,28,0,0,0,0,0,0,0,0,148,0,0,0,0,0,0,0,196,197,2,0,0,0,0,
0,30,0,119,120,122,42,50,44,46,115,509,510,111,111,37,39,37,0,20,0,215,0,171,159,
184,193,192,0,0,190,191,194,195,204,186,0,70,0,55,103,0,101,0,505,383,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,489,490,473,0,0,0,0,0,476,474,475,0,400,402,416,424,426,
428,430,432,434,437,442,445,448,452,0,454,477,488,487,0,384,382,33,0,0,111,0,0,0,
129,125,127,254,256,0,0,9,10,0,0,0,111,511,512,214,94,0,0,163,73,234,0,231,0,3,0,
5,0,284,0,31,0,0,37,21,0,116,117,142,110,0,145,0,0,0,0,0,0,0,0,0,0,0,0,505,287,
289,0,295,291,0,293,277,278,279,0,280,281,282,0,40,22,122,261,0,221,237,0,0,217,
215,0,201,0,0,0,206,58,205,187,0,0,104,100,0,0,377,0,0,397,0,0,0,0,250,393,398,0,
395,0,462,0,418,452,0,419,420,0,0,0,0,0,0,0,0,0,455,456,37,0,0,0,458,459,457,0,
374,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,406,407,408,409,410,411,412,413,414,
415,405,0,460,0,483,484,0,0,0,492,0,111,390,391,0,388,0,238,0,0,0,0,257,229,0,
130,135,131,133,126,128,215,0,263,255,264,514,513,0,87,91,0,89,0,93,109,64,63,0,
61,164,215,233,160,263,235,172,173,0,86,200,27,0,29,0,0,0,0,121,123,240,239,23,
106,118,0,0,0,137,138,140,0,111,111,153,0,0,0,0,0,0,37,0,324,325,326,0,0,328,0,0,
0,0,296,292,122,294,290,288,0,170,222,0,0,0,228,216,223,167,0,0,0,217,169,203,
202,165,201,0,0,207,59,112,0,105,423,102,98,386,387,253,0,394,249,251,381,0,0,0,
0,241,245,0,0,0,0,0,0,0,0,0,0,0,0,0,491,498,0,497,401,425,0,427,429,431,433,435,
436,440,441,438,439,443,444,446,447,449,450,451,404,403,482,479,0,481,0,0,0,378,
0,385,32,0,379,0,0,258,136,132,134,0,217,0,201,0,265,0,215,0,276,260,0,0,111,0,0,
0,129,0,111,0,215,0,182,161,232,4,6,0,35,0,0,141,124,142,0,217,146,155,154,0,0,
149,0,0,0,303,0,0,0,0,0,323,327,0,0,0,0,286,37,24,262,215,0,225,0,0,218,0,168,
221,209,166,189,188,207,185,0,7,0,252,396,399,461,0,0,242,0,246,465,0,0,0,0,469,
472,0,0,0,0,0,453,494,0,0,480,478,0,0,389,392,380,259,230,272,0,266,267,201,0,0,
217,201,0,90,0,217,0,0,0,60,62,0,174,0,0,217,0,201,0,0,0,139,143,115,144,156,153,
153,0,0,0,0,0,0,0,0,0,0,0,0,0,303,329,0,0,25,217,0,226,224,0,217,0,208,0,8,0,0,0,
243,247,0,0,0,0,421,422,482,481,496,0,499,417,493,495,0,271,269,275,274,0,268,
201,0,115,0,67,65,66,175,181,178,0,180,176,201,0,34,0,0,152,151,297,303,0,0,0,0,
0,0,331,332,334,336,338,340,342,344,347,352,355,358,362,364,371,372,0,37,300,309,
0,0,0,0,0,0,0,0,0,0,330,283,211,227,219,210,215,13,0,14,464,463,0,248,466,0,0,
471,470,485,0,273,270,36,0,0,179,177,285,108,0,0,0,303,0,368,0,0,365,37,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,366,304,0,311,0,0,0,319,0,0,0,0,0,0,302,0,217,12,
15,0,0,0,0,486,107,0,157,298,299,0,0,0,0,303,305,335,0,337,339,341,343,345,346,
350,351,348,349,353,354,356,357,359,360,361,0,310,312,313,0,321,320,0,315,0,0,0,
220,212,16,0,0,0,0,301,367,0,363,306,0,37,314,322,316,317,0,244,467,0,68,0,0,333,
303,307,318,0,370,369,308,468,0,0,0};static short Cyc_yydefgoto[143]={1027,50,685,
875,51,52,287,53,458,54,460,55,56,141,57,58,526,228,444,445,229,61,243,230,63,
164,165,64,161,65,264,265,126,127,128,266,231,425,473,474,475,66,67,646,647,648,
68,476,69,449,70,71,158,159,72,117,522,316,682,606,73,607,516,673,508,512,513,
420,309,251,94,95,547,466,548,331,332,333,232,302,303,608,431,290,462,291,292,
293,294,295,760,296,297,836,837,838,839,840,841,842,843,844,845,846,847,848,849,
850,851,334,410,411,335,336,337,298,199,397,200,532,201,202,203,204,205,206,207,
208,209,210,211,212,213,214,215,563,564,216,217,75,876,244,435};static short Cyc_yypact[
1030]={2748,- -32768,- -32768,- -32768,- -32768,116,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,3328,206,3101,- -32768,32,- 49,- -32768,- 5,136,145,165,162,189,191,340,279,
- -32768,- -32768,243,- -32768,- -32768,- -32768,250,487,285,286,312,312,- -32768,-
-32768,2611,- -32768,454,536,- -32768,295,732,3328,3328,3328,- -32768,3328,- -32768,
- -32768,252,- -32768,32,3247,381,37,684,651,- -32768,- -32768,333,349,359,- -32768,
32,371,- -32768,4365,228,- -32768,- -32768,- -32768,- -32768,333,5752,- -32768,367,313,
4365,383,389,425,- -32768,72,- -32768,- -32768,3474,3474,2611,2611,3474,429,5752,
337,- -32768,79,434,565,459,79,3693,5752,- -32768,- -32768,- -32768,2611,2884,2611,
2884,73,- -32768,469,474,- -32768,3165,- -32768,- -32768,- -32768,- -32768,3693,-
-32768,- -32768,333,139,1623,- -32768,3247,732,- -32768,3474,3401,4122,- -32768,381,
- -32768,- -32768,- -32768,484,486,- -32768,- -32768,- -32768,- -32768,- 28,651,3474,-
-32768,2884,- -32768,496,510,504,32,99,- -32768,4365,- 15,958,5957,551,5752,5801,
592,598,608,632,659,661,663,681,6006,6006,- -32768,- -32768,- -32768,2190,685,6055,
6055,6055,- -32768,- -32768,- -32768,299,- -32768,- -32768,- 6,641,545,682,631,606,130,
658,234,123,- -32768,1171,6055,193,- 24,- -32768,712,6196,735,- -32768,- -32768,740,
5752,333,6196,727,220,3693,1096,3693,605,- -32768,- 20,- 20,- -32768,- -32768,50,744,
323,333,- -32768,- -32768,- -32768,- -32768,71,730,- -32768,- -32768,647,373,- -32768,
753,- -32768,777,- -32768,784,768,797,- -32768,565,4220,3247,- -32768,791,3693,-
-32768,709,801,32,806,803,75,805,1324,816,811,809,824,4318,1324,218,820,818,-
-32768,- -32768,823,1766,1766,732,1766,- -32768,- -32768,- -32768,829,- -32768,-
-32768,- -32768,121,- -32768,- -32768,832,815,74,848,- -32768,11,836,826,284,837,701,
828,3474,5752,- -32768,842,- -32768,- -32768,74,852,32,- -32768,5752,854,- -32768,833,
851,- -32768,79,5752,4365,260,- -32768,- -32768,- -32768,856,857,2190,- -32768,3693,-
-32768,- -32768,3730,- -32768,886,5752,5752,5752,5752,5752,5752,3693,859,2190,-
-32768,- -32768,1909,853,377,5752,- -32768,- -32768,- -32768,5752,- -32768,6055,5752,
6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,
6055,5752,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,5752,- -32768,79,- -32768,- -32768,4413,79,5752,- -32768,868,
333,- -32768,- -32768,867,861,4365,- -32768,311,958,871,3474,- -32768,873,860,-
-32768,1096,1096,1096,- -32768,- -32768,2958,4508,157,- -32768,500,- -32768,- -32768,
11,- -32768,- -32768,3474,- -32768,894,- -32768,881,- -32768,876,877,883,- -32768,1173,
- -32768,535,654,- -32768,- -32768,- -32768,3693,- -32768,- -32768,- -32768,2611,-
-32768,2611,897,895,887,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
773,5752,898,891,- -32768,888,122,333,333,831,5752,5752,890,900,5752,991,2052,904,
- -32768,- -32768,- -32768,390,982,- -32768,4606,5752,1324,2328,- -32768,- -32768,3165,
- -32768,- -32768,- -32768,3474,- -32768,- -32768,3693,899,267,- -32768,- -32768,889,-
-32768,11,903,3620,826,- -32768,- -32768,- -32768,- -32768,701,34,905,863,- -32768,-
-32768,2474,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,901,- -32768,
- -32768,- -32768,- -32768,2994,906,909,929,- -32768,- -32768,728,4220,910,914,917,
919,393,915,920,562,921,923,5879,- -32768,- -32768,924,928,- -32768,641,272,545,682,
631,606,130,130,658,658,658,658,234,234,123,123,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,926,- -32768,- 26,3474,4024,- -32768,6235,- -32768,- -32768,
933,- -32768,98,943,- -32768,- -32768,- -32768,- -32768,930,826,931,701,927,500,3474,
3547,4701,- -32768,- -32768,- 20,936,333,770,940,71,3031,944,333,3474,3401,4799,-
-32768,535,- -32768,- -32768,- -32768,946,- -32768,968,893,- -32768,- -32768,709,5752,
826,- -32768,- -32768,- -32768,954,32,506,399,419,5752,841,436,950,4897,4992,520,-
-32768,- -32768,957,960,953,478,- -32768,3247,- -32768,815,962,3474,- -32768,967,11,
- -32768,966,- -32768,175,- -32768,- -32768,- -32768,- -32768,863,- -32768,963,- -32768,
2611,- -32768,- -32768,- -32768,973,208,970,- -32768,3828,- -32768,- -32768,5752,1070,
5752,5801,- -32768,- -32768,79,79,973,969,4024,- -32768,- -32768,5752,5752,- -32768,-
-32768,74,750,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,972,- -32768,-
-32768,701,74,974,826,701,985,- -32768,5752,826,516,975,976,- -32768,- -32768,992,-
-32768,74,993,826,995,701,990,5752,1006,- -32768,- -32768,3693,- -32768,1000,48,831,
1324,1007,1001,6160,999,1009,1324,5752,5087,547,5182,557,5277,841,- -32768,1012,
1015,- -32768,826,112,- -32768,- -32768,1022,826,3693,- -32768,249,- -32768,1010,1011,
5752,- -32768,- -32768,4220,1013,1016,1017,- -32768,886,1014,1019,- -32768,762,-
-32768,- -32768,- -32768,- -32768,4024,- -32768,- -32768,- -32768,- -32768,1020,- -32768,
701,588,3693,1036,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1021,- -32768,-
-32768,701,527,- -32768,1028,3693,- -32768,- -32768,1122,841,1032,6104,1029,2328,
6055,1026,- -32768,- 3,- -32768,1066,1023,756,825,134,834,268,347,- -32768,- -32768,-
-32768,- -32768,1067,6055,1909,- -32768,- -32768,531,1324,541,5372,1324,575,5467,
5562,589,1038,- -32768,- -32768,- -32768,- -32768,1040,- -32768,962,- -32768,1039,656,
- -32768,- -32768,324,- -32768,- -32768,3693,1138,- -32768,- -32768,- -32768,3926,-
-32768,- -32768,- -32768,1044,1045,- -32768,- -32768,- -32768,- -32768,620,1324,1048,
841,2190,- -32768,3693,1043,- -32768,1480,6055,5752,6055,6055,6055,6055,6055,6055,
6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,6055,5752,- -32768,- -32768,1047,
- -32768,1324,1324,645,- -32768,1324,1324,695,1324,706,5657,- -32768,11,826,- -32768,
- -32768,337,5752,1050,1046,- -32768,- -32768,1064,- -32768,- -32768,- -32768,1053,
1051,1057,6055,841,- -32768,641,334,545,682,682,606,130,130,658,658,658,658,234,
234,123,123,- -32768,- -32768,- -32768,356,- -32768,- -32768,- -32768,1324,- -32768,-
-32768,1324,- -32768,1324,1324,707,- -32768,- -32768,- -32768,802,1071,3693,1072,-
-32768,973,293,- -32768,- -32768,5752,1480,- -32768,- -32768,- -32768,- -32768,1324,-
-32768,- -32768,1073,- -32768,1075,1076,- -32768,841,- -32768,- -32768,1077,- -32768,-
-32768,- -32768,- -32768,1197,1206,- -32768};static short Cyc_yypgoto[143]={- -32768,
176,- -32768,261,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 46,-
-32768,- 84,16,- -32768,- -32768,0,586,- -32768,91,- 177,1097,5,- -32768,- -32768,- 90,-
-32768,194,1177,- 695,- -32768,- -32768,- -32768,956,952,697,480,- -32768,- -32768,578,
- -32768,- -32768,187,- -32768,- -32768,283,- 162,1149,- 371,69,- -32768,1061,- -32768,-
-32768,1175,- 87,- -32768,540,- 99,- 133,- 143,- 334,287,550,556,- 418,- 469,- 114,- 317,-
95,- -32768,- 258,- 149,- 532,- 319,- -32768,902,- 184,- 94,- 45,20,- 240,55,- -32768,-
-32768,- -32768,- 59,- 255,- -32768,- 650,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,8,1008,- -32768,690,821,- -32768,- 8,587,- -32768,- 165,- 296,-
140,- 327,- 348,- 346,865,- 341,- 344,- 362,- 336,- 332,- 175,422,479,896,- -32768,- 331,-
-32768,179,415,- 61,27,- 289,- 51};static short Cyc_yytable[6376]={59,250,308,465,136,
62,358,233,234,143,341,237,574,575,576,577,616,361,362,363,568,59,140,569,549,
317,62,344,572,573,571,96,499,500,537,502,345,78,398,566,536,671,578,579,112,262,
307,136,580,581,310,59,423,644,645,825,62,100,59,59,59,714,59,62,62,62,432,62,59,
147,263,589,509,62,130,131,132,324,133,630,405,198,300,140,142,39,366,448,442,
907,169,60,137,288,419,299,364,674,326,239,315,220,559,59,59,433,630,712,62,62,
587,406,60,327,590,319,891,434,866,101,59,59,59,59,39,62,62,62,62,59,367,510,719,
908,62,240,432,631,38,59,149,59,60,38,62,142,62,41,147,60,60,60,41,60,543,289,
544,142,531,60,432,679,59,44,662,531,518,62,556,- 150,558,434,437,511,797,433,638,
443,325,257,899,38,359,436,677,241,438,434,453,611,41,668,224,242,60,60,505,258,
225,433,631,482,97,468,483,170,453,582,583,584,611,60,60,60,60,414,299,374,375,
524,60,915,916,171,76,718,409,119,38,422,60,530,60,409,424,434,40,41,42,870,504,
288,288,364,288,423,423,423,471,434,956,430,506,60,376,377,961,382,917,918,102,
97,59,160,103,383,384,62,528,451,269,492,97,721,549,399,400,401,142,235,236,507,
172,427,104,624,105,59,59,428,59,694,62,62,306,62,253,254,255,256,429,511,- 215,
289,289,- 215,289,531,750,624,1004,288,38,106,241,783,527,107,450,402,757,41,242,
600,403,404,79,113,109,486,359,268,270,39,170,493,477,494,320,538,129,418,554,
617,495,749,359,- 237,110,97,- 237,97,171,60,124,38,1020,59,38,567,19,20,62,40,41,
42,40,41,42,1025,874,380,134,289,788,381,111,419,60,60,39,60,511,539,250,707,625,
549,670,453,114,329,330,807,364,246,591,97,811,241,1016,784,710,306,97,921,517,
242,819,922,453,667,115,794,795,74,453,665,415,219,596,364,422,422,422,301,365,
424,424,424,597,615,116,364,441,787,678,80,98,869,99,656,666,38,872,364,364,623,
60,629,440,40,41,42,947,140,364,160,59,74,59,738,873,62,1005,62,74,163,419,162,
727,637,74,649,650,738,531,653,364,657,135,923,138,788,742,74,1006,59,663,924,
925,167,62,218,166,454,429,98,43,364,455,59,221,142,561,144,62,145,98,659,97,222,
364,146,726,364,450,142,74,74,699,364,723,135,674,773,755,74,741,59,880,681,743,
778,62,74,74,74,74,511,477,364,238,301,787,799,756,713,223,60,792,60,664,970,971,
972,973,74,364,793,245,964,625,761,965,966,730,725,788,968,969,967,501,120,121,
774,453,74,60,453,962,740,166,974,975,98,247,98,259,976,977,97,60,38,97,339,260,
342,342,928,364,593,41,716,43,771,772,355,356,994,313,615,314,44,342,342,342,43,
321,299,60,47,48,49,612,753,613,323,754,787,629,322,614,98,632,342,633,804,812,
767,98,808,364,813,702,400,401,764,766,364,904,960,43,364,895,122,123,821,929,
626,905,627,800,364,452,59,860,628,931,364,62,38,642,643,805,74,863,340,927,364,
41,142,452,369,479,59,681,484,402,817,62,44,477,703,404,372,373,364,948,47,48,49,
935,252,74,684,477,890,940,477,364,364,897,1017,150,151,152,957,346,958,136,97,
889,810,347,153,154,155,156,157,943,427,368,312,348,894,166,428,823,47,48,49,454,
378,379,342,98,953,978,979,980,1019,342,38,857,859,60,862,349,865,38,40,41,42,
520,521,852,364,40,41,42,371,985,447,97,945,60,879,946,428,447,47,48,49,1003,350,
428,351,342,352,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,353,288,511,829,360,249,732,1014,38,856,364,47,48,49,370,988,40,41,42,198,98,
364,364,98,267,407,990,1011,471,644,645,38,472,1018,412,249,47,48,49,413,41,467,
692,693,125,933,417,59,937,939,446,44,62,288,782,452,758,759,452,47,48,49,439,
289,801,802,74,456,74,733,734,735,38,16,17,18,886,887,136,452,40,41,42,43,461,
452,359,342,911,912,81,457,471,963,525,603,604,605,459,59,47,48,49,463,62,469,
930,913,914,934,488,981,919,920,478,289,1012,364,421,480,426,489,481,467,485,992,
550,551,552,553,97,555,996,827,828,487,490,74,60,496,562,497,498,503,565,434,954,
507,261,515,98,534,43,288,523,267,514,519,38,84,187,188,85,86,87,585,529,41,533,
535,541,366,542,560,602,342,595,586,354,983,984,191,562,986,987,592,989,594,601,
81,60,599,38,618,619,620,634,621,622,59,40,41,42,98,62,636,640,635,610,651,639,
641,471,652,654,289,658,660,669,672,47,48,49,675,690,680,689,686,691,696,695,452,
697,1007,698,700,1008,705,1009,1010,701,704,82,709,708,452,711,717,452,39,720,
722,731,724,328,752,342,168,84,1021,736,85,86,87,739,746,41,747,751,762,768,88,
769,770,306,562,781,89,562,90,91,775,329,330,777,92,706,785,790,796,60,93,803,74,
806,814,815,7,8,9,10,11,12,13,14,15,16,17,18,19,20,809,816,818,342,820,822,249,
824,826,830,831,854,855,867,342,22,868,871,467,877,878,883,881,884,882,892,249,
27,885,888,893,896,898,249,900,28,29,903,906,909,926,941,944,910,942,949,32,951,
982,952,34,955,959,999,998,98,1000,35,36,997,1001,1002,467,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,249,135,1028,1013,1015,1022,729,1023,1024,1026,249,1029,995,
737,39,22,248,108,43,249,745,464,470,748,148,318,780,27,118,45,46,779,385,776,
993,28,29,688,416,540,570,598,853,562,0,0,32,0,0,0,34,0,0,0,557,0,0,35,36,902,0,
0,342,0,0,0,0,0,0,386,387,388,389,390,391,392,393,394,395,0,342,304,0,0,38,467,0,
0,789,0,791,39,40,41,42,43,0,306,467,396,0,798,0,0,447,0,45,46,0,0,428,0,47,48,
49,249,0,0,0,0,0,0,0,0,0,421,0,0,0,249,0,0,0,0,342,0,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,0,173,174,271,0,272,273,274,275,276,277,
278,279,135,81,0,175,280,0,0,176,0,281,0,0,0,0,467,177,178,179,180,0,342,0,0,181,
0,282,0,0,467,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,342,0,0,283,84,187,188,85,86,87,0,0,41,0,0,189,284,139,0,0,267,
0,0,190,0,0,191,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,467,0,249,0,
0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,758,759,21,173,174,
271,267,272,273,274,275,276,277,278,279,22,81,23,175,280,0,249,176,24,281,0,0,27,
0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,
35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,
86,87,39,40,41,42,43,189,284,139,- 303,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,
0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,
173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,
0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,
0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,
188,85,86,87,39,40,41,42,43,189,284,139,285,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,
193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,
281,0,0,27,0,0,177,178,179,180,0,28,29,- 37,181,0,282,0,0,0,0,0,32,33,182,183,34,
184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,
84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,
192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,
176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,
183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,
0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,173,174,0,0,0,0,0,0,0,0,0,0,22,81,23,175,0,0,0,176,24,0,0,0,27,0,
0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,
36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,
39,40,41,42,43,189,655,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,
0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,
0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,
0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,357,0,0,0,0,0,190,0,45,
286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,
178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,
185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,
41,42,43,189,0,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,
197,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,
0,0,0,0,0,0,22,0,23,0,0,0,683,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,
0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,
48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,
0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,
0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,
48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,
0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,
0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,
49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,
0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,
32,33,0,0,34,0,0,0,0,0,0,35,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,
0,0,0,0,0,0,38,0,0,0,0,22,0,39,40,41,42,43,0,0,0,- 11,0,27,0,0,44,0,45,46,0,28,29,
0,47,48,49,0,0,0,0,0,32,0,0,0,34,0,81,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,
0,0,427,82,45,46,28,29,428,0,47,48,49,0,0,687,0,32,168,84,0,34,85,86,87,0,0,41,
35,36,0,0,88,0,0,0,0,0,89,0,90,91,0,329,330,0,92,0,0,0,0,0,93,0,0,38,0,81,0,0,0,
0,39,40,41,42,43,0,0,0,0,0,0,0,0,471,0,45,46,0,0,0,0,47,48,49,2,3,4,77,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,82,0,0,0,0,0,0,0,0,0,22,0,23,0,0,83,
84,0,24,85,86,87,27,0,41,0,0,0,0,88,28,29,- 37,0,0,89,0,90,91,0,0,32,33,92,0,34,0,
0,0,93,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,261,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,
28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,2,3,4,77,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,0,139,0,0,
0,0,0,24,0,45,46,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,
0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,
0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,
0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,
305,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,0,0,45,46,28,29,0,0,226,0,0,0,
0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,227,0,27,0,0,0,0,0,45,46,28,29,
0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,0,
0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,676,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,
0,27,0,0,0,0,0,45,46,28,29,0,0,0,173,174,0,0,0,0,32,545,0,0,34,0,0,81,0,175,0,35,
36,176,0,0,0,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,
0,0,39,40,0,42,43,0,0,185,186,0,0,0,0,0,0,45,46,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
187,188,85,86,87,0,0,41,0,0,189,0,343,546,0,173,174,0,190,0,0,191,0,329,330,0,
192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,
0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,786,0,173,174,0,190,
0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,
180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,
950,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,
0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,
0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,
0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,
195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,
0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,311,192,0,81,
193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,
0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,
0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,
181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,81,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,491,173,174,0,0,0,
0,190,0,0,191,0,0,0,81,192,175,0,193,82,176,194,195,0,0,196,197,0,177,178,179,
180,0,0,168,84,181,0,85,86,87,0,0,41,0,0,182,183,88,184,0,0,0,0,89,0,90,91,0,185,
186,0,92,0,0,0,0,0,93,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,
0,189,0,173,174,0,0,0,0,190,588,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,
196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,
0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,
0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,609,192,0,81,193,175,0,194,
195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,
0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,661,0,191,0,0,0,81,192,175,
0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,
182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,728,
192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,
0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,
191,0,0,0,744,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,
0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,763,173,174,
0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,
179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,
765,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,
0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,
185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,
0,0,189,0,173,174,0,0,0,0,190,858,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,
196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,
0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,
0,0,41,0,0,189,0,173,174,0,0,0,0,190,861,0,191,0,0,0,81,192,175,0,193,0,176,194,
195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,
0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,
85,86,87,0,0,41,0,0,189,864,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,
176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,
0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,932,0,191,0,0,0,81,192,175,
0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,
182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,936,0,191,0,0,0,81,
192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,
0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,938,0,191,
0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,
0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,
991,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,
0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,173,174,0,0,0,0,0,0,0,185,186,0,0,81,0,0,
0,0,0,0,0,0,0,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,
182,183,0,184,0,0,0,190,0,0,191,0,0,185,186,192,0,0,193,0,0,194,195,0,0,196,197,
0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,343,0,0,0,0,0,190,
0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,
0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,
188,85,86,87,0,0,41,81,0,189,0,706,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,
193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,173,174,0,0,0,0,0,0,0,185,186,0,0,
81,0,0,0,0,0,0,0,0,0,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,
189,182,183,0,184,173,174,0,338,0,0,191,0,0,185,186,192,0,81,193,0,0,194,195,0,0,
196,197,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,
0,184,173,174,0,354,0,0,191,0,0,185,186,192,0,81,193,0,0,194,195,0,0,196,197,0,0,
0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,0,0,0,
190,0,0,191,832,833,185,186,192,0,0,193,0,0,194,195,0,81,196,197,0,0,0,0,0,0,0,0,
0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,0,0,0,901,0,0,191,81,0,0,0,192,0,
0,193,0,0,194,195,0,0,196,197,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,0,168,84,0,0,85,86,
87,81,0,41,0,0,0,0,88,0,0,0,0,0,834,82,90,91,0,0,0,0,92,0,0,835,0,408,93,195,168,
84,196,197,85,86,87,0,0,41,0,0,0,0,88,0,0,0,0,0,89,0,90,91,82,0,0,0,92,0,0,0,0,0,
93,0,715,0,0,168,84,0,0,85,86,87,0,0,41,0,0,0,0,88,0,0,0,0,0,89,0,90,91,0,0,0,0,
92,0,0,0,0,0,93};static short Cyc_yycheck[6376]={0,115,145,261,65,0,190,101,102,68,
175,105,374,375,376,377,434,192,193,194,368,21,68,369,343,158,21,176,372,373,371,
23,287,288,330,290,176,21,213,366,329,510,378,379,44,129,145,108,380,381,145,51,
229,5,6,750,51,106,58,59,60,593,62,58,59,60,86,62,68,69,129,402,61,68,58,59,60,
167,62,450,104,89,141,129,68,113,92,249,17,92,82,0,65,139,227,141,122,515,113,
107,128,93,357,103,104,125,477,133,103,104,399,135,21,128,403,160,811,137,768,
124,120,121,122,123,113,120,121,122,123,129,136,120,601,136,129,108,86,454,106,
139,71,141,51,106,139,129,141,115,148,58,59,60,115,62,338,139,340,141,323,68,86,
127,162,126,495,330,309,162,352,121,354,137,122,306,706,125,472,106,170,106,830,
106,190,234,518,106,237,137,249,429,115,508,120,114,103,104,122,124,126,125,517,
126,23,262,129,106,267,382,383,384,450,120,121,122,123,223,262,87,88,313,129,87,
88,124,108,127,218,51,106,229,139,321,141,225,229,137,114,115,116,127,119,287,
288,122,290,422,423,424,126,137,900,231,303,162,124,125,906,134,124,125,124,82,
262,124,119,142,143,262,319,249,131,279,93,607,593,82,83,84,262,103,104,106,88,
126,119,447,124,287,288,132,290,549,287,288,119,290,120,121,122,123,231,434,127,
287,288,130,290,472,642,471,960,357,106,124,106,107,316,126,249,126,651,115,114,
417,131,132,120,44,49,274,338,137,138,113,106,280,267,119,162,331,57,121,350,437,
126,641,354,119,105,170,122,172,124,262,59,106,1006,357,106,367,20,21,357,114,
115,116,114,115,116,1019,121,137,120,357,693,141,126,510,287,288,113,290,515,123,
498,560,447,706,121,450,105,131,132,727,122,112,404,218,732,106,107,690,130,119,
225,137,122,114,742,141,471,505,126,702,703,0,477,501,224,106,412,122,422,423,
424,142,127,422,423,424,119,431,120,122,240,693,523,22,23,773,25,487,501,106,778,
122,122,447,357,449,127,114,115,116,130,501,122,124,458,44,460,623,779,458,130,
460,51,108,601,120,613,471,57,481,482,637,641,485,122,487,65,134,67,802,627,70,
130,487,496,142,143,120,487,126,79,122,427,82,117,122,127,501,119,487,127,124,
501,126,93,119,331,122,122,132,613,122,447,501,103,104,127,122,609,108,942,668,
127,112,627,529,788,526,627,676,529,120,121,122,123,672,471,122,113,260,802,710,
127,592,123,458,699,460,497,915,916,917,918,142,122,699,126,909,623,127,910,911,
617,612,887,913,914,912,289,119,120,669,637,162,487,640,907,626,167,919,920,170,
127,172,119,921,922,412,501,106,415,173,122,175,176,854,122,407,115,595,117,127,
665,185,186,943,126,611,126,126,192,193,194,117,122,665,529,134,135,136,124,119,
126,123,122,887,630,121,132,218,458,213,460,724,122,119,225,728,122,127,82,83,84,
655,656,122,834,906,117,122,127,119,120,744,127,124,835,126,713,122,249,665,119,
132,127,122,665,106,478,479,725,260,119,126,853,122,115,665,267,138,269,685,681,
272,126,740,685,126,623,131,132,89,90,122,882,134,135,136,127,116,289,529,637,
119,119,640,122,122,826,1002,63,64,65,901,126,903,781,542,809,731,126,74,75,76,
77,78,873,126,91,146,126,822,321,132,746,134,135,136,122,85,86,323,331,127,923,
924,925,1006,330,106,762,763,665,765,126,767,106,114,115,116,63,64,758,122,114,
115,116,140,127,126,595,119,685,785,122,132,126,134,135,136,959,126,132,126,366,
126,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,126,854,
942,755,126,115,619,998,106,761,122,134,135,136,139,127,114,115,116,834,412,122,
122,415,134,120,127,127,126,5,6,106,130,1005,106,145,134,135,136,106,115,261,121,
122,119,860,126,854,863,864,127,126,854,906,685,447,22,23,450,134,135,136,125,
854,121,122,458,121,460,106,107,108,106,17,18,19,121,122,946,471,114,115,116,117,
123,477,901,472,139,140,38,121,126,908,314,422,423,424,121,906,134,135,136,113,
906,121,858,89,90,861,106,926,85,86,120,906,121,122,228,120,230,119,126,343,126,
940,346,347,348,349,758,351,947,753,754,126,119,529,854,126,360,130,126,121,364,
137,898,106,123,130,542,125,117,1006,133,265,127,127,106,107,108,109,110,111,112,
385,121,115,121,125,121,92,122,127,121,560,122,397,126,931,932,129,402,935,936,
124,938,127,122,38,906,127,106,106,120,126,106,127,122,1006,114,115,116,595,1006,
125,122,119,428,126,119,130,126,120,30,1006,119,42,126,137,134,135,136,127,122,
127,127,133,106,122,127,623,122,985,122,127,988,121,990,991,127,127,91,122,127,
637,127,121,640,113,127,127,123,133,103,647,641,106,107,1011,127,110,111,112,127,
126,115,106,121,126,120,120,119,127,119,495,120,126,498,128,129,121,131,132,125,
134,120,124,25,127,1006,140,127,685,127,127,127,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,133,127,127,699,127,133,427,119,126,120,127,130,121,119,710,37,119,113,
549,127,127,122,127,127,126,107,447,49,127,127,127,121,28,454,120,57,58,126,130,
91,91,121,121,138,122,25,68,121,119,122,72,121,127,107,126,758,121,79,80,127,127,
122,593,8,9,10,11,12,13,14,15,16,17,18,19,20,21,498,781,0,127,127,127,614,127,
127,127,508,0,946,622,113,37,114,35,117,517,628,260,265,640,70,159,681,49,48,128,
129,676,56,672,942,57,58,542,225,332,370,415,758,651,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,
- 1,353,- 1,- 1,79,80,832,- 1,- 1,835,- 1,- 1,- 1,- 1,- 1,- 1,93,94,95,96,97,98,99,100,101,
102,- 1,853,103,- 1,- 1,106,693,- 1,- 1,696,- 1,698,113,114,115,116,117,- 1,119,706,123,
- 1,709,- 1,- 1,126,- 1,128,129,- 1,- 1,132,- 1,134,135,136,613,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,623,- 1,- 1,- 1,627,- 1,- 1,- 1,- 1,907,- 1,909,910,911,912,913,914,915,916,917,918,
919,920,921,922,923,924,925,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,946,38,- 1,40,
41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,788,52,53,54,55,- 1,959,- 1,- 1,60,- 1,62,- 1,- 1,802,- 1,
- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1005,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,-
1,118,119,120,- 1,- 1,750,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,
141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,887,- 1,779,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,811,29,30,31,32,33,34,35,36,37,
38,39,40,41,- 1,826,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,-
1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,
115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,
- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,
- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,
- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,-
1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,
35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,
62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,
113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,
- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,
- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,
- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,
- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,
60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,
112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,
- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,
- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,
- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,
126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,
- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,
116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,
140,141,- 1,- 1,144,145,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,-
1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,
49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,
- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,
- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,
45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,
72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,-
1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,
- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,
68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,1,- 1,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,
- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,37,- 1,113,114,115,116,
117,- 1,- 1,- 1,121,- 1,49,- 1,- 1,126,- 1,128,129,- 1,57,58,- 1,134,135,136,- 1,- 1,- 1,- 1,
- 1,68,- 1,- 1,- 1,72,- 1,38,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,
119,- 1,- 1,49,- 1,- 1,- 1,126,91,128,129,57,58,132,- 1,134,135,136,- 1,- 1,103,- 1,68,
106,107,- 1,72,110,111,112,- 1,- 1,115,79,80,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,
129,- 1,131,132,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,106,- 1,38,- 1,- 1,- 1,- 1,113,114,115,
116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,37,- 1,39,- 1,- 1,106,107,- 1,45,110,111,112,49,- 1,115,- 1,- 1,- 1,- 1,120,57,58,59,
- 1,- 1,126,- 1,128,129,- 1,- 1,68,69,134,- 1,72,- 1,- 1,- 1,140,- 1,- 1,79,80,81,- 1,- 1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,113,
114,- 1,116,117,- 1,37,120,39,- 1,123,- 1,- 1,- 1,45,128,129,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,-
1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,
- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,113,114,- 1,116,117,37,- 1,120,- 1,- 1,- 1,- 1,- 1,45,- 1,128,129,49,- 1,- 1,- 1,- 1,- 1,-
1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,
114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,
- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,-
1,- 1,120,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,
- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,
- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,
- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,
128,129,57,58,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,68,32,- 1,- 1,72,- 1,- 1,38,- 1,40,- 1,79,80,
44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,
71,- 1,73,- 1,- 1,113,114,- 1,116,117,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,128,129,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,
120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,-
1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,
71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,
121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,
144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,
- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,
- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,
145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,
73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,
25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,-
1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,
126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,
52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,
129,- 1,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,83,84,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,
108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,-
1,- 1,38,134,40,- 1,137,91,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,106,107,
60,- 1,110,111,112,- 1,- 1,115,- 1,- 1,70,71,120,73,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,83,
84,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,
109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,
38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,-
1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,
- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,
- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,
141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,
- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,
25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,
- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,
126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,
52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,
129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,-
1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,
110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,
134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,
115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,
44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,
25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,
144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,-
1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,
54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,
107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,
- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,
60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,
112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,-
1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,
- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,
141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,
- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,
- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,
52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,25,26,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,83,84,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,
106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,- 1,- 1,- 1,126,- 1,- 1,
129,- 1,- 1,83,84,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,25,
26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,
- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,70,71,- 1,
73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,-
1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,-
1,70,71,- 1,73,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,
71,- 1,73,25,26,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,38,137,- 1,- 1,140,141,- 1,- 1,
144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,
118,70,71,- 1,73,25,26,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,38,137,- 1,- 1,140,141,-
1,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,
- 1,118,70,71,- 1,73,- 1,- 1,- 1,126,- 1,- 1,129,25,26,83,84,134,- 1,- 1,137,- 1,- 1,140,
141,- 1,38,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,
115,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,38,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,
140,141,- 1,- 1,144,145,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,- 1,
- 1,110,111,112,38,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,91,128,129,- 1,- 1,- 1,
- 1,134,- 1,- 1,137,- 1,103,140,141,106,107,144,145,110,111,112,- 1,- 1,115,- 1,- 1,- 1,-
1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,91,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,103,- 1,
- 1,106,107,- 1,- 1,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,
129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140};char Cyc_Yystack_overflow[17]="Yystack_overflow";
struct Cyc_Yystack_overflow_struct{char*tag;int f1;};struct Cyc_Yystack_overflow_struct
Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};void Cyc_yyerror(struct
_dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(int)'\000';
union Cyc_YYSTYPE Cyc_yylval={.YYINITIALSVAL={61,0}};static int Cyc_yynerrs=0;struct
_tuple25{unsigned int f1;struct _tuple1*f2;int f3;};struct _tuple26{struct
_dyneither_ptr f1;void*f2;};static char _tmp527[8]="stdcall";static char _tmp528[6]="cdecl";
static char _tmp529[9]="fastcall";static char _tmp52A[9]="noreturn";static char
_tmp52B[6]="const";static char _tmp52C[8]="aligned";static char _tmp52D[7]="packed";
static char _tmp52E[7]="shared";static char _tmp52F[7]="unused";static char _tmp530[5]="weak";
static char _tmp531[10]="dllimport";static char _tmp532[10]="dllexport";static char
_tmp533[23]="no_instrument_function";static char _tmp534[12]="constructor";static
char _tmp535[11]="destructor";static char _tmp536[22]="no_check_memory_usage";
static char _tmp537[5]="pure";struct _tuple27{void*f1;void*f2;};struct _tuple28{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*
yylex_buf);static void _tmpA2E(unsigned int*_tmpA2D,unsigned int*_tmpA2C,union Cyc_YYSTYPE**
_tmpA2B){for(*_tmpA2D=0;*_tmpA2D < *_tmpA2C;(*_tmpA2D)++){(*_tmpA2B)[*_tmpA2D]=
Cyc_yylval;}}static void _tmpA33(unsigned int*_tmpA32,unsigned int*_tmpA31,struct
Cyc_Yyltype**_tmpA30){for(*_tmpA32=0;*_tmpA32 < *_tmpA31;(*_tmpA32)++){(*_tmpA30)[*
_tmpA32]=Cyc_yynewloc();}}static void _tmpA3A(int*yyssp_offset,struct
_dyneither_ptr*yyss,unsigned int*_tmpA39,unsigned int*_tmpA38,short**_tmpA36){
for(*_tmpA39=0;*_tmpA39 < *_tmpA38;(*_tmpA39)++){(*_tmpA36)[*_tmpA39]=*_tmpA39 <= *
yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*
_tmpA39)): 0;}}static void _tmpA40(struct _dyneither_ptr*yyvs,int*yyssp_offset,
unsigned int*_tmpA3F,unsigned int*_tmpA3E,union Cyc_YYSTYPE**_tmpA3C){for(*
_tmpA3F=0;*_tmpA3F < *_tmpA3E;(*_tmpA3F)++){(*_tmpA3C)[*_tmpA3F]=*_tmpA3F <= *
yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union
Cyc_YYSTYPE),(int)*_tmpA3F)): Cyc_yylval;}}static void _tmpA46(int*yyssp_offset,
struct _dyneither_ptr*yyls,unsigned int*_tmpA45,unsigned int*_tmpA44,struct Cyc_Yyltype**
_tmpA42){for(*_tmpA45=0;*_tmpA45 < *_tmpA44;(*_tmpA45)++){(*_tmpA42)[*_tmpA45]=*
_tmpA45 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,
sizeof(struct Cyc_Yyltype),(int)*_tmpA45)): Cyc_yynewloc();}}static void _tmpF36(
unsigned int*_tmpF35,unsigned int*_tmpF34,char**_tmpF32){for(*_tmpF35=0;*_tmpF35
< *_tmpF34;(*_tmpF35)++){(*_tmpF32)[*_tmpF35]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*
yylex_buf){struct _RegionHandle _tmp474=_new_region("yyregion");struct
_RegionHandle*yyregion=& _tmp474;_push_region(yyregion);{int yystate;int yyn=0;int
yyerrstatus;int yychar1=0;int yyssp_offset;short*_tmpA29;unsigned int _tmpA28;
struct _dyneither_ptr yyss=(_tmpA28=200,((_tmpA29=_region_calloc(yyregion,sizeof(
short),_tmpA28),_tag_dyneither(_tmpA29,sizeof(short),_tmpA28))));int yyvsp_offset;
unsigned int _tmpA2D;unsigned int _tmpA2C;union Cyc_YYSTYPE*_tmpA2B;unsigned int
_tmpA2A;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmpA2A=(unsigned int)200,((
_tmpA2B=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),
_tmpA2A)),((((_tmpA2C=_tmpA2A,_tmpA2E(& _tmpA2D,& _tmpA2C,& _tmpA2B))),_tmpA2B)))))),
sizeof(union Cyc_YYSTYPE),(unsigned int)200);int yylsp_offset;unsigned int _tmpA32;
unsigned int _tmpA31;struct Cyc_Yyltype*_tmpA30;unsigned int _tmpA2F;struct
_dyneither_ptr yyls=_tag_dyneither(((_tmpA2F=(unsigned int)200,((_tmpA30=(struct
Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),
_tmpA2F)),((((_tmpA31=_tmpA2F,_tmpA33(& _tmpA32,& _tmpA31,& _tmpA30))),_tmpA30)))))),
sizeof(struct Cyc_Yyltype),(unsigned int)200);int yystacksize=200;union Cyc_YYSTYPE
yyval=Cyc_yylval;int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;
yyssp_offset=- 1;yyvsp_offset=0;yylsp_offset=0;yynewstate:*((short*)
_check_dyneither_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 10000){{const char*_tmpA34;Cyc_yyerror(((
_tmpA34="parser stack overflow",_tag_dyneither(_tmpA34,sizeof(char),22))));}(int)
_throw((void*)& Cyc_Yystack_overflow_val);}yystacksize *=2;if(yystacksize > 10000)
yystacksize=10000;{unsigned int _tmpA39;unsigned int _tmpA38;struct _dyneither_ptr
_tmpA37;short*_tmpA36;unsigned int _tmpA35;struct _dyneither_ptr yyss1=(_tmpA35=(
unsigned int)yystacksize,((_tmpA36=(short*)_region_malloc(yyregion,_check_times(
sizeof(short),_tmpA35)),((_tmpA37=_tag_dyneither(_tmpA36,sizeof(short),_tmpA35),((((
_tmpA38=_tmpA35,_tmpA3A(& yyssp_offset,& yyss,& _tmpA39,& _tmpA38,& _tmpA36))),
_tmpA37)))))));unsigned int _tmpA3F;unsigned int _tmpA3E;struct _dyneither_ptr
_tmpA3D;union Cyc_YYSTYPE*_tmpA3C;unsigned int _tmpA3B;struct _dyneither_ptr yyvs1=(
_tmpA3B=(unsigned int)yystacksize,((_tmpA3C=(union Cyc_YYSTYPE*)_region_malloc(
yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA3B)),((_tmpA3D=
_tag_dyneither(_tmpA3C,sizeof(union Cyc_YYSTYPE),_tmpA3B),((((_tmpA3E=_tmpA3B,
_tmpA40(& yyvs,& yyssp_offset,& _tmpA3F,& _tmpA3E,& _tmpA3C))),_tmpA3D)))))));
unsigned int _tmpA45;unsigned int _tmpA44;struct _dyneither_ptr _tmpA43;struct Cyc_Yyltype*
_tmpA42;unsigned int _tmpA41;struct _dyneither_ptr yyls1=(_tmpA41=(unsigned int)
yystacksize,((_tmpA42=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(
sizeof(struct Cyc_Yyltype),_tmpA41)),((_tmpA43=_tag_dyneither(_tmpA42,sizeof(
struct Cyc_Yyltype),_tmpA41),((((_tmpA44=_tmpA41,_tmpA46(& yyssp_offset,& yyls,&
_tmpA45,& _tmpA44,& _tmpA42))),_tmpA43)))))));yyss=yyss1;yyvs=yyvs1;yyls=yyls1;};}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1030,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 373?(int)Cyc_yytranslate[_check_known_subscript_notnull(374,Cyc_yychar)]:
289;}yyn +=yychar1;if((yyn < 0  || yyn > 6375) || Cyc_yycheck[
_check_known_subscript_notnull(6376,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6376,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1029){int
_tmp482=0;_npop_handler(0);return _tmp482;}if(Cyc_yychar != 0)Cyc_yychar=- 2;*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++
yyvsp_offset))=Cyc_yylval;*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,
sizeof(struct Cyc_Yyltype),++ yylsp_offset))=Cyc_yylloc;if(yyerrstatus != 0)--
yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1030,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(515,yyn)];if(yylen > 0)yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(
yyvsp_offset + 1)- yylen));switch(yyn){case 1: _LL215: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyyyvsp[0]);break;}case 2: _LL216: {union Cyc_YYSTYPE*yyyyvsp=(union
Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct Cyc_List_List*x;struct Cyc_List_List*
y;{struct _handler_cons _tmp483;_push_handler(& _tmp483);{int _tmp485=0;if(setjmp(
_tmp483.handler))_tmp485=1;if(!_tmp485){x=Cyc_yyget_YY18(yyyyvsp[0]);;
_pop_handler();}else{void*_tmp484=(void*)_exn_thrown;void*_tmp487=_tmp484;_LL219: {
struct Cyc_Core_Failure_struct*_tmp488=(struct Cyc_Core_Failure_struct*)_tmp487;
if(_tmp488->tag != Cyc_Core_Failure)goto _LL21B;}_LL21A: x=0;goto _LL218;_LL21B:;
_LL21C:(void)_throw(_tmp487);_LL218:;}};}{struct _handler_cons _tmp489;
_push_handler(& _tmp489);{int _tmp48B=0;if(setjmp(_tmp489.handler))_tmp48B=1;if(!
_tmp48B){y=Cyc_yyget_YY18(yyyyvsp[1]);;_pop_handler();}else{void*_tmp48A=(void*)
_exn_thrown;void*_tmp48D=_tmp48A;_LL21E: {struct Cyc_Core_Failure_struct*_tmp48E=(
struct Cyc_Core_Failure_struct*)_tmp48D;if(_tmp48E->tag != Cyc_Core_Failure)goto
_LL220;}_LL21F: y=0;goto _LL21D;_LL220:;_LL221:(void)_throw(_tmp48D);_LL21D:;}};}
yyval=Cyc_YY18(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(x,y));break;}case 3: _LL217: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_Absyn_Decl*_tmpA50;
struct Cyc_Absyn_Using_d_struct _tmpA4F;struct Cyc_Absyn_Using_d_struct*_tmpA4E;
struct Cyc_List_List*_tmpA4D;yyval=Cyc_YY18(((_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((
_tmpA4D->hd=((_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50->r=(void*)((_tmpA4E=
_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E[0]=((_tmpA4F.tag=11,((_tmpA4F.f1=Cyc_yyget_QualId_tok(
yyyyvsp[0]),((_tmpA4F.f2=Cyc_yyget_YY18(yyyyvsp[2]),_tmpA4F)))))),_tmpA4E)))),((
_tmpA50->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA50)))))),((_tmpA4D->tl=0,
_tmpA4D)))))));}Cyc_Lex_leave_using();break;}case 4: _LL222: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);{struct
Cyc_Absyn_Decl*_tmpA5A;struct Cyc_Absyn_Using_d_struct _tmpA59;struct Cyc_Absyn_Using_d_struct*
_tmpA58;struct Cyc_List_List*_tmpA57;yyval=Cyc_YY18(((_tmpA57=_cycalloc(sizeof(*
_tmpA57)),((_tmpA57->hd=((_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A->r=(void*)((
_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58[0]=((_tmpA59.tag=11,((_tmpA59.f1=
Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA59.f2=Cyc_yyget_YY18(yyyyvsp[2]),_tmpA59)))))),
_tmpA58)))),((_tmpA5A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA5A)))))),((
_tmpA57->tl=Cyc_yyget_YY18(yyyyvsp[4]),_tmpA57)))))));}break;}case 5: _LL223: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_Absyn_Decl*_tmpA69;
struct Cyc_Absyn_Namespace_d_struct _tmpA68;struct _dyneither_ptr*_tmpA67;struct Cyc_Absyn_Namespace_d_struct*
_tmpA66;struct Cyc_List_List*_tmpA65;yyval=Cyc_YY18(((_tmpA65=_cycalloc(sizeof(*
_tmpA65)),((_tmpA65->hd=((_tmpA69=_cycalloc(sizeof(*_tmpA69)),((_tmpA69->r=(void*)((
_tmpA66=_cycalloc(sizeof(*_tmpA66)),((_tmpA66[0]=((_tmpA68.tag=10,((_tmpA68.f1=((
_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67[0]=Cyc_yyget_String_tok(yyyyvsp[0]),
_tmpA67)))),((_tmpA68.f2=Cyc_yyget_YY18(yyyyvsp[2]),_tmpA68)))))),_tmpA66)))),((
_tmpA69->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA69)))))),((_tmpA65->tl=0,
_tmpA65)))))));}Cyc_Lex_leave_namespace();break;}case 6: _LL224: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);struct
_dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp49C;_push_handler(& _tmp49C);{int _tmp49E=0;if(setjmp(_tmp49C.handler))
_tmp49E=1;if(!_tmp49E){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}
else{void*_tmp49D=(void*)_exn_thrown;void*_tmp4A0=_tmp49D;_LL227: {struct Cyc_Core_Failure_struct*
_tmp4A1=(struct Cyc_Core_Failure_struct*)_tmp4A0;if(_tmp4A1->tag != Cyc_Core_Failure)
goto _LL229;}_LL228:{const char*_tmpA6A;nspace=((_tmpA6A="",_tag_dyneither(_tmpA6A,
sizeof(char),1)));}goto _LL226;_LL229:;_LL22A:(void)_throw(_tmp4A0);_LL226:;}};}{
struct _handler_cons _tmp4A3;_push_handler(& _tmp4A3);{int _tmp4A5=0;if(setjmp(
_tmp4A3.handler))_tmp4A5=1;if(!_tmp4A5){x=Cyc_yyget_YY18(yyyyvsp[2]);;
_pop_handler();}else{void*_tmp4A4=(void*)_exn_thrown;void*_tmp4A7=_tmp4A4;_LL22C: {
struct Cyc_Core_Failure_struct*_tmp4A8=(struct Cyc_Core_Failure_struct*)_tmp4A7;
if(_tmp4A8->tag != Cyc_Core_Failure)goto _LL22E;}_LL22D: x=0;goto _LL22B;_LL22E:;
_LL22F:(void)_throw(_tmp4A7);_LL22B:;}};}{struct _handler_cons _tmp4A9;
_push_handler(& _tmp4A9);{int _tmp4AB=0;if(setjmp(_tmp4A9.handler))_tmp4AB=1;if(!
_tmp4AB){y=Cyc_yyget_YY18(yyyyvsp[4]);;_pop_handler();}else{void*_tmp4AA=(void*)
_exn_thrown;void*_tmp4AD=_tmp4AA;_LL231: {struct Cyc_Core_Failure_struct*_tmp4AE=(
struct Cyc_Core_Failure_struct*)_tmp4AD;if(_tmp4AE->tag != Cyc_Core_Failure)goto
_LL233;}_LL232: y=0;goto _LL230;_LL233:;_LL234:(void)_throw(_tmp4AD);_LL230:;}};}{
struct Cyc_Absyn_Decl*_tmpA79;struct Cyc_Absyn_Namespace_d_struct _tmpA78;struct
_dyneither_ptr*_tmpA77;struct Cyc_Absyn_Namespace_d_struct*_tmpA76;struct Cyc_List_List*
_tmpA75;yyval=Cyc_YY18(((_tmpA75=_cycalloc(sizeof(*_tmpA75)),((_tmpA75->hd=((
_tmpA79=_cycalloc(sizeof(*_tmpA79)),((_tmpA79->r=(void*)((_tmpA76=_cycalloc(
sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA78.tag=10,((_tmpA78.f1=((_tmpA77=_cycalloc(
sizeof(*_tmpA77)),((_tmpA77[0]=nspace,_tmpA77)))),((_tmpA78.f2=x,_tmpA78)))))),
_tmpA76)))),((_tmpA79->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA79)))))),((
_tmpA75->tl=y,_tmpA75)))))));}break;}case 7: _LL225: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);struct _dyneither_ptr _tmp4B4=Cyc_yyget_String_tok(
yyyyvsp[1]);{const char*_tmpA7A;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4B4,((
_tmpA7A="C",_tag_dyneither(_tmpA7A,sizeof(char),2))))== 0){struct Cyc_Absyn_Decl*
_tmpA84;struct Cyc_Absyn_ExternC_d_struct _tmpA83;struct Cyc_Absyn_ExternC_d_struct*
_tmpA82;struct Cyc_List_List*_tmpA81;yyval=Cyc_YY18(((_tmpA81=_cycalloc(sizeof(*
_tmpA81)),((_tmpA81->hd=((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->r=(void*)((
_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82[0]=((_tmpA83.tag=12,((_tmpA83.f1=
Cyc_yyget_YY18(yyyyvsp[3]),_tmpA83)))),_tmpA82)))),((_tmpA84->loc=(unsigned int)(
yyyylsp[0]).first_line,_tmpA84)))))),((_tmpA81->tl=Cyc_yyget_YY18(yyyyvsp[5]),
_tmpA81)))))));}else{{const char*_tmpA85;if(Cyc_strcmp((struct _dyneither_ptr)
_tmp4B4,((_tmpA85="C include",_tag_dyneither(_tmpA85,sizeof(char),10))))!= 0){
const char*_tmpA86;Cyc_Parse_err(((_tmpA86="expecting \"C\" or \"C include\"",
_tag_dyneither(_tmpA86,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_Absyn_Decl*_tmpA90;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA8F;
struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA8E;struct Cyc_List_List*_tmpA8D;
yyval=Cyc_YY18(((_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D->hd=((_tmpA90=
_cycalloc(sizeof(*_tmpA90)),((_tmpA90->r=(void*)((_tmpA8E=_cycalloc(sizeof(*
_tmpA8E)),((_tmpA8E[0]=((_tmpA8F.tag=13,((_tmpA8F.f1=Cyc_yyget_YY18(yyyyvsp[3]),((
_tmpA8F.f2=0,_tmpA8F)))))),_tmpA8E)))),((_tmpA90->loc=(unsigned int)(yyyylsp[0]).first_line,
_tmpA90)))))),((_tmpA8D->tl=Cyc_yyget_YY18(yyyyvsp[5]),_tmpA8D)))))));};}}break;}
case 8: _LL235: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union
Cyc_YYSTYPE),7);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);{const char*_tmpA91;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA91="C include",
_tag_dyneither(_tmpA91,sizeof(char),10))))!= 0){const char*_tmpA92;Cyc_Parse_err(((
_tmpA92="expecting \"C include\"",_tag_dyneither(_tmpA92,sizeof(char),22))),(
unsigned int)(yyyylsp[0]).first_line);}}{struct Cyc_List_List*exs=Cyc_yyget_YY52(
yyyyvsp[5]);{struct Cyc_Absyn_Decl*_tmpA9C;struct Cyc_Absyn_ExternCinclude_d_struct
_tmpA9B;struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA9A;struct Cyc_List_List*
_tmpA99;yyval=Cyc_YY18(((_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99->hd=((
_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->r=(void*)((_tmpA9A=_cycalloc(
sizeof(*_tmpA9A)),((_tmpA9A[0]=((_tmpA9B.tag=13,((_tmpA9B.f1=Cyc_yyget_YY18(
yyyyvsp[3]),((_tmpA9B.f2=exs,_tmpA9B)))))),_tmpA9A)))),((_tmpA9C->loc=(
unsigned int)(yyyylsp[0]).first_line,_tmpA9C)))))),((_tmpA99->tl=Cyc_yyget_YY18(
yyyyvsp[6]),_tmpA99)))))));}break;};}case 9: _LL236: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_Absyn_Decl*_tmpA9F;
struct Cyc_List_List*_tmpA9E;yyval=Cyc_YY18(((_tmpA9E=_cycalloc(sizeof(*_tmpA9E)),((
_tmpA9E->hd=((_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F->r=(void*)& Cyc_Absyn_Porton_d_val,((
_tmpA9F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA9F)))))),((_tmpA9E->tl=
Cyc_yyget_YY18(yyyyvsp[2]),_tmpA9E)))))));}break;}case 10: _LL237: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct
Cyc_Absyn_Decl*_tmpAA2;struct Cyc_List_List*_tmpAA1;yyval=Cyc_YY18(((_tmpAA1=
_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->hd=((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((
_tmpAA2->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpAA2->loc=(unsigned int)(yyyylsp[
0]).first_line,_tmpAA2)))))),((_tmpAA1->tl=Cyc_yyget_YY18(yyyyvsp[2]),_tmpAA1)))))));}
break;}case 11: _LL238: yyval=Cyc_YY18(0);break;case 12: _LL239: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);yyval=
yyyyvsp[2];break;}case 13: _LL23A: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY52(0);break;}case 14:
_LL23B: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _tuple25*_tmpAA5;struct Cyc_List_List*
_tmpAA4;yyval=Cyc_YY52(((_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4->hd=((
_tmpAA5=_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5->f1=(unsigned int)(yyyylsp[0]).first_line,((
_tmpAA5->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAA5->f3=0,_tmpAA5)))))))),((
_tmpAA4->tl=0,_tmpAA4)))))));}break;}case 15: _LL23C: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct _tuple25*_tmpAA8;struct Cyc_List_List*
_tmpAA7;yyval=Cyc_YY52(((_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->hd=((
_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8->f1=(unsigned int)(yyyylsp[0]).first_line,((
_tmpAA8->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAA8->f3=0,_tmpAA8)))))))),((
_tmpAA7->tl=0,_tmpAA7)))))));}break;}case 16: _LL23D: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct _tuple25*_tmpAAB;struct Cyc_List_List*
_tmpAAA;yyval=Cyc_YY52(((_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA->hd=((
_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->f1=(unsigned int)(yyyylsp[0]).first_line,((
_tmpAAB->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAAB->f3=0,_tmpAAB)))))))),((
_tmpAAA->tl=Cyc_yyget_YY52(yyyyvsp[2]),_tmpAAA)))))));}break;}case 17: _LL23E: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Absyn_Fn_d_struct*
_tmpAB1;struct Cyc_Absyn_Fn_d_struct _tmpAB0;struct Cyc_List_List*_tmpAAF;yyval=Cyc_YY18(((
_tmpAAF=_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF->hd=Cyc_Absyn_new_decl((void*)((
_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1[0]=((_tmpAB0.tag=1,((_tmpAB0.f1=
Cyc_yyget_YY17(yyyyvsp[0]),_tmpAB0)))),_tmpAB1)))),(unsigned int)(yyyylsp[0]).first_line),((
_tmpAAF->tl=0,_tmpAAF)))))));}break;}case 18: _LL23F: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 19:
_LL240: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY18(0);break;}case 20:
_LL241: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY17(Cyc_Parse_make_function(
0,Cyc_yyget_YY29(yyyyvsp[0]),0,Cyc_yyget_YY9(yyyyvsp[1]),(unsigned int)(yyyylsp[
0]).first_line));break;}case 21: _LL242: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_Core_Opt*_tmpAB2;yyval=
Cyc_YY17(Cyc_Parse_make_function(((_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2->v=
Cyc_yyget_YY19(yyyyvsp[0]),_tmpAB2)))),Cyc_yyget_YY29(yyyyvsp[1]),0,Cyc_yyget_YY9(
yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));}break;}case 22: _LL243: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY17(Cyc_Parse_make_function(
0,Cyc_yyget_YY29(yyyyvsp[0]),Cyc_yyget_YY18(yyyyvsp[1]),Cyc_yyget_YY9(yyyyvsp[2]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 23: _LL244: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct
Cyc_Core_Opt*_tmpAB3;yyval=Cyc_YY17(Cyc_Parse_make_function(((_tmpAB3=_cycalloc(
sizeof(*_tmpAB3)),((_tmpAB3->v=Cyc_yyget_YY19(yyyyvsp[0]),_tmpAB3)))),Cyc_yyget_YY29(
yyyyvsp[1]),Cyc_yyget_YY18(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[3]),(unsigned int)(
yyyylsp[0]).first_line));}break;}case 24: _LL245: {union Cyc_YYSTYPE*yyyyvsp=(union
Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_Core_Opt*_tmpAB4;yyval=
Cyc_YY17(Cyc_Parse_make_function(((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4->v=
Cyc_yyget_YY19(yyyyvsp[0]),_tmpAB4)))),Cyc_yyget_YY29(yyyyvsp[1]),0,Cyc_yyget_YY9(
yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));}break;}case 25: _LL246: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_Core_Opt*_tmpAB5;yyval=
Cyc_YY17(Cyc_Parse_make_function(((_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5->v=
Cyc_yyget_YY19(yyyyvsp[0]),_tmpAB5)))),Cyc_yyget_YY29(yyyyvsp[1]),Cyc_yyget_YY18(
yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 26: _LL247: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(
yyyyvsp[1]));yyval=yyyyvsp[1];break;}case 27: _LL248: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);Cyc_Lex_leave_using();break;}case
28: _LL249: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct _dyneither_ptr*_tmpAB6;Cyc_Lex_enter_namespace(((
_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6[0]=Cyc_yyget_String_tok(yyyyvsp[1]),
_tmpAB6)))));}yyval=yyyyvsp[1];break;}case 29: _LL24A: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);Cyc_Lex_leave_namespace();break;}
case 30: _LL24B: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);int _tmp4D8=(yyyylsp[0]).first_line;
yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyyyvsp[0]),0,(
unsigned int)_tmp4D8,(unsigned int)_tmp4D8));break;}case 31: _LL24C: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=
Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY21(
yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)(yyyylsp[0]).first_line));
break;}case 32: _LL24D: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);{struct Cyc_List_List*_tmpAB7;yyval=
Cyc_YY18(((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->hd=Cyc_Absyn_let_decl(
Cyc_yyget_YY11(yyyyvsp[1]),Cyc_yyget_YY3(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((
_tmpAB7->tl=0,_tmpAB7)))))));}break;}case 33: _LL24E: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);struct Cyc_List_List*_tmp4DA=0;{
struct Cyc_List_List*_tmp4DB=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp4DB != 0;_tmp4DB=
_tmp4DB->tl){struct _dyneither_ptr*_tmp4DC=(struct _dyneither_ptr*)_tmp4DB->hd;
struct _tuple1*_tmpAB8;struct _tuple1*qv=(_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((
_tmpAB8->f1=Cyc_Absyn_Rel_n(0),((_tmpAB8->f2=_tmp4DC,_tmpAB8)))));struct Cyc_Absyn_Vardecl*
_tmp4DD=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*
_tmpAB9;_tmp4DA=((_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->hd=_tmp4DD,((
_tmpAB9->tl=_tmp4DA,_tmpAB9))))));}}_tmp4DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp4DA);{struct Cyc_List_List*_tmpABA;yyval=Cyc_YY18(((
_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA->hd=Cyc_Absyn_letv_decl(_tmp4DA,(
unsigned int)(yyyylsp[0]).first_line),((_tmpABA->tl=0,_tmpABA)))))));}break;}
case 34: _LL24F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union
Cyc_YYSTYPE),7);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);struct _dyneither_ptr _tmp4E1=Cyc_yyget_String_tok(
yyyyvsp[3]);{const char*_tmpABB;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E1,((
_tmpABB="`H",_tag_dyneither(_tmpABB,sizeof(char),3))))== 0){const char*_tmpABE;
void*_tmpABD;Cyc_Parse_err((struct _dyneither_ptr)((_tmpABD=0,Cyc_aprintf(((
_tmpABE="bad occurrence of heap region",_tag_dyneither(_tmpABE,sizeof(char),30))),
_tag_dyneither(_tmpABD,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
const char*_tmpABF;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E1,((_tmpABF="`U",
_tag_dyneither(_tmpABF,sizeof(char),3))))== 0){const char*_tmpAC2;void*_tmpAC1;
Cyc_Parse_err((struct _dyneither_ptr)((_tmpAC1=0,Cyc_aprintf(((_tmpAC2="bad occurrence of unique region",
_tag_dyneither(_tmpAC2,sizeof(char),32))),_tag_dyneither(_tmpAC1,sizeof(void*),0)))),(
unsigned int)(yyyylsp[3]).first_line);}}{struct _dyneither_ptr*_tmpAC5;struct Cyc_Absyn_Tvar*
_tmpAC4;struct Cyc_Absyn_Tvar*tv=(_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4->name=((
_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5[0]=_tmp4E1,_tmpAC5)))),((_tmpAC4->identity=
- 1,((_tmpAC4->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAC4)))))));
struct Cyc_Absyn_VarType_struct _tmpAC8;struct Cyc_Absyn_VarType_struct*_tmpAC7;
void*t=(void*)((_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7[0]=((_tmpAC8.tag=2,((
_tmpAC8.f1=tv,_tmpAC8)))),_tmpAC7))));struct Cyc_Absyn_RgnHandleType_struct
_tmpACE;struct Cyc_Absyn_RgnHandleType_struct*_tmpACD;struct _dyneither_ptr*
_tmpACB;struct _tuple1*_tmpACA;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA->f1=Cyc_Absyn_Loc_n,((_tmpACA->f2=((
_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB[0]=Cyc_yyget_String_tok(yyyyvsp[5]),
_tmpACB)))),_tmpACA)))))),(void*)((_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD[
0]=((_tmpACE.tag=15,((_tmpACE.f1=(void*)t,_tmpACE)))),_tmpACD)))),0);{struct Cyc_List_List*
_tmpACF;yyval=Cyc_YY18(((_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((
_tmpACF->tl=0,_tmpACF)))))));}break;};}case 35: _LL250: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);int _tmp4F1=Cyc_yyget_YY31(yyyyvsp[
0]);struct _dyneither_ptr _tmp4F2=Cyc_yyget_String_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*
_tmp4F3=Cyc_yyget_YY4(yyyyvsp[3]);{const char*_tmpAD0;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp4F2,((_tmpAD0="H",_tag_dyneither(_tmpAD0,sizeof(char),2))))== 
0){const char*_tmpAD1;Cyc_Parse_err(((_tmpAD1="bad occurrence of heap region `H",
_tag_dyneither(_tmpAD1,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}{
const char*_tmpAD2;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F2,((_tmpAD2="U",
_tag_dyneither(_tmpAD2,sizeof(char),2))))== 0){const char*_tmpAD3;Cyc_Parse_err(((
_tmpAD3="bad occurrence of unique region `U",_tag_dyneither(_tmpAD3,sizeof(char),
35))),(unsigned int)(yyyylsp[2]).first_line);}}if(_tmp4F1  && _tmp4F3 != 0){const
char*_tmpAD4;Cyc_Parse_err(((_tmpAD4="open regions cannot be @resetable",
_tag_dyneither(_tmpAD4,sizeof(char),34))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*_tmpAE1;const char*_tmpAE0;void*_tmpADF[1];struct Cyc_String_pa_struct
_tmpADE;struct Cyc_Absyn_Tvar*_tmpADD;struct Cyc_Absyn_Tvar*tv=(_tmpADD=_cycalloc(
sizeof(*_tmpADD)),((_tmpADD->name=((_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((
_tmpAE1[0]=(struct _dyneither_ptr)((_tmpADE.tag=0,((_tmpADE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp4F2),((_tmpADF[0]=& _tmpADE,Cyc_aprintf(((
_tmpAE0="`%s",_tag_dyneither(_tmpAE0,sizeof(char),4))),_tag_dyneither(_tmpADF,
sizeof(void*),1)))))))),_tmpAE1)))),((_tmpADD->identity=- 1,((_tmpADD->kind=Cyc_Tcutil_kind_to_bound(&
Cyc_Tcutil_rk),_tmpADD)))))));struct Cyc_Absyn_VarType_struct _tmpAE4;struct Cyc_Absyn_VarType_struct*
_tmpAE3;void*t=(void*)((_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3[0]=((
_tmpAE4.tag=2,((_tmpAE4.f1=tv,_tmpAE4)))),_tmpAE3))));struct Cyc_Absyn_RgnHandleType_struct
_tmpAEA;struct Cyc_Absyn_RgnHandleType_struct*_tmpAE9;struct _dyneither_ptr*
_tmpAE7;struct _tuple1*_tmpAE6;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6->f1=Cyc_Absyn_Loc_n,((_tmpAE6->f2=((
_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7[0]=_tmp4F2,_tmpAE7)))),_tmpAE6)))))),(
void*)((_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9[0]=((_tmpAEA.tag=15,((
_tmpAEA.f1=(void*)t,_tmpAEA)))),_tmpAE9)))),0);{struct Cyc_List_List*_tmpAEB;
yyval=Cyc_YY18(((_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB->hd=Cyc_Absyn_region_decl(
tv,vd,_tmp4F1,_tmp4F3,(unsigned int)(yyyylsp[0]).first_line),((_tmpAEB->tl=0,
_tmpAEB)))))));}break;};}case 36: _LL251: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);struct Cyc_Absyn_Eq_kb_struct*
_tmpAF3;struct Cyc_Absyn_Eq_kb_struct _tmpAF2;struct _dyneither_ptr*_tmpAF1;struct
Cyc_Absyn_Tvar*_tmpAF0;struct Cyc_Absyn_Tvar*tv=(_tmpAF0=_cycalloc(sizeof(*
_tmpAF0)),((_tmpAF0->name=((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1[0]=Cyc_yyget_String_tok(
yyyyvsp[2]),_tmpAF1)))),((_tmpAF0->identity=- 1,((_tmpAF0->kind=(void*)((_tmpAF3=
_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3[0]=((_tmpAF2.tag=0,((_tmpAF2.f1=& Cyc_Tcutil_rk,
_tmpAF2)))),_tmpAF3)))),_tmpAF0)))))));struct _dyneither_ptr*_tmpAF6;struct
_tuple1*_tmpAF5;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAF5=
_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5->f1=Cyc_Absyn_Loc_n,((_tmpAF5->f2=((
_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6[0]=Cyc_yyget_String_tok(yyyyvsp[4]),
_tmpAF6)))),_tmpAF5)))))),(void*)& Cyc_Absyn_VoidType_val,0);{struct Cyc_List_List*
_tmpAF7;yyval=Cyc_YY18(((_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7->hd=Cyc_Absyn_alias_decl(
Cyc_yyget_YY3(yyyyvsp[6]),tv,vd,(unsigned int)(yyyylsp[0]).first_line),((_tmpAF7->tl=
0,_tmpAF7)))))));}break;}case 37: _LL252: yyval=Cyc_YY31(0);break;case 38: _LL253: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY31(1);break;}case 39:
_LL254: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 40:
_LL255: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(
yyyyvsp[0]),Cyc_yyget_YY18(yyyyvsp[1])));break;}case 41: _LL256: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct
Cyc_Core_Opt*_tmpAFA;struct Cyc_Parse_Declaration_spec*_tmpAF9;yyval=Cyc_YY19(((
_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9->sc=((_tmpAFA=_cycalloc_atomic(
sizeof(*_tmpAFA)),((_tmpAFA->v=(void*)Cyc_yyget_YY22(yyyyvsp[0]),_tmpAFA)))),((
_tmpAF9->tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((
_tmpAF9->type_specs=0,((_tmpAF9->is_inline=0,((_tmpAF9->attributes=0,_tmpAF9)))))))))))));}
break;}case 42: _LL257: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct Cyc_Parse_Declaration_spec*
_tmp50E=Cyc_yyget_YY19(yyyyvsp[1]);if(_tmp50E->sc != 0){const char*_tmpAFD;void*
_tmpAFC;(_tmpAFC=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((
_tmpAFD="Only one storage class is allowed in a declaration",_tag_dyneither(
_tmpAFD,sizeof(char),51))),_tag_dyneither(_tmpAFC,sizeof(void*),0)));}{struct Cyc_Core_Opt*
_tmpB00;struct Cyc_Parse_Declaration_spec*_tmpAFF;yyval=Cyc_YY19(((_tmpAFF=
_cycalloc(sizeof(*_tmpAFF)),((_tmpAFF->sc=((_tmpB00=_cycalloc_atomic(sizeof(*
_tmpB00)),((_tmpB00->v=(void*)Cyc_yyget_YY22(yyyyvsp[0]),_tmpB00)))),((_tmpAFF->tq=
_tmp50E->tq,((_tmpAFF->type_specs=_tmp50E->type_specs,((_tmpAFF->is_inline=
_tmp50E->is_inline,((_tmpAFF->attributes=_tmp50E->attributes,_tmpAFF)))))))))))));}
break;}case 43: _LL258: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_List_List*_tmpB03;
struct Cyc_Parse_Declaration_spec*_tmpB02;yyval=Cyc_YY19(((_tmpB02=_cycalloc(
sizeof(*_tmpB02)),((_tmpB02->sc=0,((_tmpB02->tq=Cyc_Absyn_empty_tqual((
unsigned int)(yyyylsp[0]).first_line),((_tmpB02->type_specs=((_tmpB03=_cycalloc(
sizeof(*_tmpB03)),((_tmpB03->hd=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB03->tl=0,
_tmpB03)))))),((_tmpB02->is_inline=0,((_tmpB02->attributes=0,_tmpB02)))))))))))));}
break;}case 44: _LL259: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct Cyc_Parse_Declaration_spec*
_tmp515=Cyc_yyget_YY19(yyyyvsp[1]);{struct Cyc_List_List*_tmpB06;struct Cyc_Parse_Declaration_spec*
_tmpB05;yyval=Cyc_YY19(((_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05->sc=
_tmp515->sc,((_tmpB05->tq=_tmp515->tq,((_tmpB05->type_specs=((_tmpB06=_cycalloc(
sizeof(*_tmpB06)),((_tmpB06->hd=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB06->tl=_tmp515->type_specs,
_tmpB06)))))),((_tmpB05->is_inline=_tmp515->is_inline,((_tmpB05->attributes=
_tmp515->attributes,_tmpB05)))))))))))));}break;}case 45: _LL25A: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct
Cyc_Parse_Declaration_spec*_tmpB07;yyval=Cyc_YY19(((_tmpB07=_cycalloc(sizeof(*
_tmpB07)),((_tmpB07->sc=0,((_tmpB07->tq=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpB07->type_specs=
0,((_tmpB07->is_inline=0,((_tmpB07->attributes=0,_tmpB07)))))))))))));}break;}
case 46: _LL25B: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct Cyc_Parse_Declaration_spec*
_tmp519=Cyc_yyget_YY19(yyyyvsp[1]);{struct Cyc_Parse_Declaration_spec*_tmpB08;
yyval=Cyc_YY19(((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08->sc=_tmp519->sc,((
_tmpB08->tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyyyvsp[0]),_tmp519->tq),((
_tmpB08->type_specs=_tmp519->type_specs,((_tmpB08->is_inline=_tmp519->is_inline,((
_tmpB08->attributes=_tmp519->attributes,_tmpB08)))))))))))));}break;}case 47:
_LL25C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Parse_Declaration_spec*
_tmpB09;yyval=Cyc_YY19(((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09->sc=0,((
_tmpB09->tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((
_tmpB09->type_specs=0,((_tmpB09->is_inline=1,((_tmpB09->attributes=0,_tmpB09)))))))))))));}
break;}case 48: _LL25D: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct Cyc_Parse_Declaration_spec*
_tmp51C=Cyc_yyget_YY19(yyyyvsp[1]);{struct Cyc_Parse_Declaration_spec*_tmpB0A;
yyval=Cyc_YY19(((_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A->sc=_tmp51C->sc,((
_tmpB0A->tq=_tmp51C->tq,((_tmpB0A->type_specs=_tmp51C->type_specs,((_tmpB0A->is_inline=
1,((_tmpB0A->attributes=_tmp51C->attributes,_tmpB0A)))))))))))));}break;}case 49:
_LL25E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Parse_Declaration_spec*
_tmpB0B;yyval=Cyc_YY19(((_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B->sc=0,((
_tmpB0B->tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((
_tmpB0B->type_specs=0,((_tmpB0B->is_inline=0,((_tmpB0B->attributes=Cyc_yyget_YY45(
yyyyvsp[0]),_tmpB0B)))))))))))));}break;}case 50: _LL25F: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct
Cyc_Parse_Declaration_spec*_tmp51F=Cyc_yyget_YY19(yyyyvsp[1]);{struct Cyc_Parse_Declaration_spec*
_tmpB0C;yyval=Cyc_YY19(((_tmpB0C=_cycalloc(sizeof(*_tmpB0C)),((_tmpB0C->sc=
_tmp51F->sc,((_tmpB0C->tq=_tmp51F->tq,((_tmpB0C->type_specs=_tmp51F->type_specs,((
_tmpB0C->is_inline=_tmp51F->is_inline,((_tmpB0C->attributes=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(
yyyyvsp[0]),_tmp51F->attributes),_tmpB0C)))))))))))));}break;}case 51: _LL260: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY22(Cyc_Parse_Auto_sc);
break;}case 52: _LL261: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY22(Cyc_Parse_Register_sc);
break;}case 53: _LL262: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY22(Cyc_Parse_Static_sc);
break;}case 54: _LL263: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY22(Cyc_Parse_Extern_sc);
break;}case 55: _LL264: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{const char*_tmpB0D;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpB0D="C",
_tag_dyneither(_tmpB0D,sizeof(char),2))))!= 0){const char*_tmpB0E;Cyc_Parse_err(((
_tmpB0E="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpB0E,sizeof(
char),37))),(unsigned int)(yyyylsp[0]).first_line);}}yyval=Cyc_YY22(Cyc_Parse_ExternC_sc);
break;}case 56: _LL265: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY22(Cyc_Parse_Typedef_sc);
break;}case 57: _LL266: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY22(Cyc_Parse_Abstract_sc);
break;}case 58: _LL267: yyval=Cyc_YY45(0);break;case 59: _LL268: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
yyyyvsp[0];break;}case 60: _LL269: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);yyval=yyyyvsp[3];break;}case 61:
_LL26A: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_List_List*_tmpB0F;yyval=
Cyc_YY45(((_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F->hd=Cyc_yyget_YY46(
yyyyvsp[0]),((_tmpB0F->tl=0,_tmpB0F)))))));}break;}case 62: _LL26B: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct
Cyc_List_List*_tmpB10;yyval=Cyc_YY45(((_tmpB10=_cycalloc(sizeof(*_tmpB10)),((
_tmpB10->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpB10->tl=Cyc_yyget_YY45(yyyyvsp[2]),
_tmpB10)))))));}break;}case 63: _LL26C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);static struct Cyc_Absyn_Aligned_att_struct
att_aligned={6,- 1};static struct _tuple26 att_map[17]={{{_tmp527,_tmp527,_tmp527 + 8},(
void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp528,_tmp528,_tmp528 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{
_tmp529,_tmp529,_tmp529 + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp52A,
_tmp52A,_tmp52A + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp52B,_tmp52B,
_tmp52B + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp52C,_tmp52C,_tmp52C + 8},(void*)&
att_aligned},{{_tmp52D,_tmp52D,_tmp52D + 7},(void*)& Cyc_Absyn_Packed_att_val},{{
_tmp52E,_tmp52E,_tmp52E + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp52F,_tmp52F,
_tmp52F + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp530,_tmp530,_tmp530 + 5},(
void*)& Cyc_Absyn_Weak_att_val},{{_tmp531,_tmp531,_tmp531 + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{
_tmp532,_tmp532,_tmp532 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp533,
_tmp533,_tmp533 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp534,
_tmp534,_tmp534 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp535,_tmp535,
_tmp535 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp536,_tmp536,_tmp536 + 22},(
void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp537,_tmp537,_tmp537 + 5},(
void*)& Cyc_Absyn_Pure_att_val}};struct _dyneither_ptr _tmp525=Cyc_yyget_String_tok(
yyyyvsp[0]);if((((_get_dyneither_size(_tmp525,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp525,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp525,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp525,sizeof(char),(int)(_get_dyneither_size(
_tmp525,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp525,sizeof(char),(int)(_get_dyneither_size(_tmp525,sizeof(char))- 3)))== '_')
_tmp525=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp525,2,
_get_dyneither_size(_tmp525,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp525,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){{const char*_tmpB11;Cyc_Parse_err(((
_tmpB11="unrecognized attribute",_tag_dyneither(_tmpB11,sizeof(char),23))),(
unsigned int)(yyyylsp[0]).first_line);}yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
break;};}case 64: _LL26D: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
break;}case 65: _LL26E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct _dyneither_ptr _tmp539=Cyc_yyget_String_tok(
yyyyvsp[0]);int _tmp53B;struct _tuple6 _tmp53A=Cyc_yyget_Int_tok(yyyyvsp[2]);
_tmp53B=_tmp53A.f2;{void*a;{const char*_tmpB13;const char*_tmpB12;if(Cyc_zstrcmp((
struct _dyneither_ptr)_tmp539,((_tmpB13="regparm",_tag_dyneither(_tmpB13,sizeof(
char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp539,((_tmpB12="__regparm__",
_tag_dyneither(_tmpB12,sizeof(char),12))))== 0){if(_tmp53B < 0  || _tmp53B > 3){
const char*_tmpB14;Cyc_Parse_err(((_tmpB14="regparm requires value between 0 and 3",
_tag_dyneither(_tmpB14,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_struct _tmpB17;struct Cyc_Absyn_Regparm_att_struct*
_tmpB16;a=(void*)((_tmpB16=_cycalloc_atomic(sizeof(*_tmpB16)),((_tmpB16[0]=((
_tmpB17.tag=0,((_tmpB17.f1=_tmp53B,_tmpB17)))),_tmpB16))));};}else{const char*
_tmpB19;const char*_tmpB18;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp539,((_tmpB19="aligned",
_tag_dyneither(_tmpB19,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp539,((_tmpB18="__aligned__",_tag_dyneither(_tmpB18,sizeof(
char),12))))== 0){if(_tmp53B < 0){const char*_tmpB1A;Cyc_Parse_err(((_tmpB1A="aligned requires positive power of two",
_tag_dyneither(_tmpB1A,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp53B;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
const char*_tmpB1B;Cyc_Parse_err(((_tmpB1B="aligned requires positive power of two",
_tag_dyneither(_tmpB1B,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_struct _tmpB1E;struct Cyc_Absyn_Aligned_att_struct*
_tmpB1D;a=(void*)((_tmpB1D=_cycalloc_atomic(sizeof(*_tmpB1D)),((_tmpB1D[0]=((
_tmpB1E.tag=6,((_tmpB1E.f1=_tmp53B,_tmpB1E)))),_tmpB1D))));};};}else{const char*
_tmpB20;const char*_tmpB1F;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp539,((_tmpB20="initializes",
_tag_dyneither(_tmpB20,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp539,((_tmpB1F="__initializes__",_tag_dyneither(_tmpB1F,
sizeof(char),16))))== 0){struct Cyc_Absyn_Initializes_att_struct _tmpB23;struct Cyc_Absyn_Initializes_att_struct*
_tmpB22;a=(void*)((_tmpB22=_cycalloc_atomic(sizeof(*_tmpB22)),((_tmpB22[0]=((
_tmpB23.tag=20,((_tmpB23.f1=_tmp53B,_tmpB23)))),_tmpB22))));}else{const char*
_tmpB25;const char*_tmpB24;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp539,((_tmpB25="noliveunique",
_tag_dyneither(_tmpB25,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp539,((_tmpB24="__noliveunique__",_tag_dyneither(_tmpB24,
sizeof(char),17))))== 0){struct Cyc_Absyn_Noliveunique_att_struct _tmpB28;struct
Cyc_Absyn_Noliveunique_att_struct*_tmpB27;a=(void*)((_tmpB27=_cycalloc_atomic(
sizeof(*_tmpB27)),((_tmpB27[0]=((_tmpB28.tag=21,((_tmpB28.f1=_tmp53B,_tmpB28)))),
_tmpB27))));}else{{const char*_tmpB29;Cyc_Parse_err(((_tmpB29="unrecognized attribute",
_tag_dyneither(_tmpB29,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}yyval=Cyc_YY46(a);break;};}case 66: _LL26F: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct _dyneither_ptr _tmp550=Cyc_yyget_String_tok(
yyyyvsp[0]);struct _dyneither_ptr _tmp551=Cyc_yyget_String_tok(yyyyvsp[2]);void*a;{
const char*_tmpB2B;const char*_tmpB2A;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp550,((
_tmpB2B="section",_tag_dyneither(_tmpB2B,sizeof(char),8))))== 0  || Cyc_zstrcmp((
struct _dyneither_ptr)_tmp550,((_tmpB2A="__section__",_tag_dyneither(_tmpB2A,
sizeof(char),12))))== 0){struct Cyc_Absyn_Section_att_struct _tmpB2E;struct Cyc_Absyn_Section_att_struct*
_tmpB2D;a=(void*)((_tmpB2D=_cycalloc(sizeof(*_tmpB2D)),((_tmpB2D[0]=((_tmpB2E.tag=
8,((_tmpB2E.f1=_tmp551,_tmpB2E)))),_tmpB2D))));}else{{const char*_tmpB2F;Cyc_Parse_err(((
_tmpB2F="unrecognized attribute",_tag_dyneither(_tmpB2F,sizeof(char),23))),(
unsigned int)(yyyylsp[0]).first_line);}a=(void*)& Cyc_Absyn_Cdecl_att_val;}}yyval=
Cyc_YY46(a);break;}case 67: _LL270: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct _dyneither_ptr _tmp557=Cyc_yyget_String_tok(
yyyyvsp[0]);struct _dyneither_ptr _tmp558=Cyc_yyget_String_tok(yyyyvsp[2]);void*a;{
const char*_tmpB30;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp557,((_tmpB30="__mode__",
_tag_dyneither(_tmpB30,sizeof(char),9))))== 0){struct Cyc_Absyn_Mode_att_struct
_tmpB33;struct Cyc_Absyn_Mode_att_struct*_tmpB32;a=(void*)((_tmpB32=_cycalloc(
sizeof(*_tmpB32)),((_tmpB32[0]=((_tmpB33.tag=23,((_tmpB33.f1=_tmp558,_tmpB33)))),
_tmpB32))));}else{{const char*_tmpB34;Cyc_Parse_err(((_tmpB34="unrecognized attribute",
_tag_dyneither(_tmpB34,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 68: _LL271: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union
Cyc_YYSTYPE),8);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);struct _dyneither_ptr _tmp55D=Cyc_yyget_String_tok(
yyyyvsp[0]);struct _dyneither_ptr _tmp55E=Cyc_yyget_String_tok(yyyyvsp[2]);int
_tmp560;struct _tuple6 _tmp55F=Cyc_yyget_Int_tok(yyyyvsp[4]);_tmp560=_tmp55F.f2;{
int _tmp562;struct _tuple6 _tmp561=Cyc_yyget_Int_tok(yyyyvsp[6]);_tmp562=_tmp561.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;{const char*_tmpB36;const char*_tmpB35;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp55D,((_tmpB36="format",_tag_dyneither(
_tmpB36,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp55D,((
_tmpB35="__format__",_tag_dyneither(_tmpB35,sizeof(char),11))))== 0){const char*
_tmpB37;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp55E,((_tmpB37="printf",
_tag_dyneither(_tmpB37,sizeof(char),7))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpB3A;struct Cyc_Absyn_Format_att_struct*_tmpB39;a=(void*)((_tmpB39=
_cycalloc_atomic(sizeof(*_tmpB39)),((_tmpB39[0]=((_tmpB3A.tag=19,((_tmpB3A.f1=
Cyc_Absyn_Printf_ft,((_tmpB3A.f2=_tmp560,((_tmpB3A.f3=_tmp562,_tmpB3A)))))))),
_tmpB39))));}else{const char*_tmpB3B;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp55E,((
_tmpB3B="scanf",_tag_dyneither(_tmpB3B,sizeof(char),6))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpB3E;struct Cyc_Absyn_Format_att_struct*_tmpB3D;a=(void*)((_tmpB3D=
_cycalloc_atomic(sizeof(*_tmpB3D)),((_tmpB3D[0]=((_tmpB3E.tag=19,((_tmpB3E.f1=
Cyc_Absyn_Scanf_ft,((_tmpB3E.f2=_tmp560,((_tmpB3E.f3=_tmp562,_tmpB3E)))))))),
_tmpB3D))));}else{const char*_tmpB3F;Cyc_Parse_err(((_tmpB3F="unrecognized format type",
_tag_dyneither(_tmpB3F,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}
else{const char*_tmpB40;Cyc_Parse_err(((_tmpB40="unrecognized attribute",
_tag_dyneither(_tmpB40,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY46(a);break;};};}case 69: _LL272: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 70:
_LL273: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_Absyn_TypedefType_struct
_tmpB43;struct Cyc_Absyn_TypedefType_struct*_tmpB42;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB42=_cycalloc(sizeof(*_tmpB42)),((_tmpB42[0]=((_tmpB43.tag=17,((
_tmpB43.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB43.f2=Cyc_yyget_YY40(yyyyvsp[
1]),((_tmpB43.f3=0,((_tmpB43.f4=0,_tmpB43)))))))))),_tmpB42)))),(unsigned int)(
yyyylsp[0]).first_line));}break;}case 71: _LL274: {union Cyc_YYSTYPE*yyyyvsp=(union
Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));break;}
case 72: _LL275: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));break;}case 73:
_LL276: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0),(
unsigned int)(yyyylsp[0]).first_line));break;}case 74: _LL277: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL278: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Parse_Short_spec_struct
_tmpB46;struct Cyc_Parse_Short_spec_struct*_tmpB45;yyval=Cyc_YY23((void*)((
_tmpB45=_cycalloc_atomic(sizeof(*_tmpB45)),((_tmpB45[0]=((_tmpB46.tag=2,((
_tmpB46.f1=(unsigned int)(yyyylsp[0]).first_line,_tmpB46)))),_tmpB45)))));}
break;}case 76: _LL279: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));break;}case 77: _LL27A: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Parse_Long_spec_struct
_tmpB49;struct Cyc_Parse_Long_spec_struct*_tmpB48;yyval=Cyc_YY23((void*)((_tmpB48=
_cycalloc_atomic(sizeof(*_tmpB48)),((_tmpB48[0]=((_tmpB49.tag=3,((_tmpB49.f1=(
unsigned int)(yyyylsp[0]).first_line,_tmpB49)))),_tmpB48)))));}break;}case 78:
_LL27B: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));break;}case 79:
_LL27C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));break;}case 80:
_LL27D: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Parse_Signed_spec_struct
_tmpB4C;struct Cyc_Parse_Signed_spec_struct*_tmpB4B;yyval=Cyc_YY23((void*)((
_tmpB4B=_cycalloc_atomic(sizeof(*_tmpB4B)),((_tmpB4B[0]=((_tmpB4C.tag=0,((
_tmpB4C.f1=(unsigned int)(yyyylsp[0]).first_line,_tmpB4C)))),_tmpB4B)))));}
break;}case 81: _LL27E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Parse_Unsigned_spec_struct
_tmpB4F;struct Cyc_Parse_Unsigned_spec_struct*_tmpB4E;yyval=Cyc_YY23((void*)((
_tmpB4E=_cycalloc_atomic(sizeof(*_tmpB4E)),((_tmpB4E[0]=((_tmpB4F.tag=1,((
_tmpB4F.f1=(unsigned int)(yyyylsp[0]).first_line,_tmpB4F)))),_tmpB4E)))));}
break;}case 82: _LL27F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 83:
_LL280: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 84:
_LL281: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 85:
_LL282: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_yyget_YY44(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));break;}case 86:
_LL283: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_Absyn_TupleType_struct
_tmpB52;struct Cyc_Absyn_TupleType_struct*_tmpB51;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB51=_cycalloc(sizeof(*_tmpB51)),((_tmpB51[0]=((_tmpB52.tag=10,((
_tmpB52.f1=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(unsigned int,struct
_tuple9*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(
unsigned int)(yyyylsp[2]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2]))),_tmpB52)))),_tmpB51)))),(
unsigned int)(yyyylsp[0]).first_line));}break;}case 87: _LL284: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct
Cyc_Absyn_RgnHandleType_struct _tmpB55;struct Cyc_Absyn_RgnHandleType_struct*
_tmpB54;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB54=_cycalloc(sizeof(*
_tmpB54)),((_tmpB54[0]=((_tmpB55.tag=15,((_tmpB55.f1=(void*)Cyc_yyget_YY44(
yyyyvsp[2]),_tmpB55)))),_tmpB54)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 88: _LL285: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Absyn_RgnHandleType_struct
_tmpB58;struct Cyc_Absyn_RgnHandleType_struct*_tmpB57;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB57=_cycalloc(sizeof(*_tmpB57)),((_tmpB57[0]=((_tmpB58.tag=15,((
_tmpB58.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0),
_tmpB58)))),_tmpB57)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case 89:
_LL286: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);void*_tmp57D=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0);{struct Cyc_Absyn_DynRgnType_struct _tmpB5B;
struct Cyc_Absyn_DynRgnType_struct*_tmpB5A;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A[0]=((_tmpB5B.tag=16,((
_tmpB5B.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB5B.f2=(void*)_tmp57D,_tmpB5B)))))),
_tmpB5A)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case 90: _LL287: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union
Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);{struct Cyc_Absyn_DynRgnType_struct
_tmpB5E;struct Cyc_Absyn_DynRgnType_struct*_tmpB5D;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB5D=_cycalloc(sizeof(*_tmpB5D)),((_tmpB5D[0]=((_tmpB5E.tag=16,((
_tmpB5E.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB5E.f2=(void*)Cyc_yyget_YY44(
yyyyvsp[4]),_tmpB5E)))))),_tmpB5D)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 91: _LL288: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_Absyn_TagType_struct
_tmpB61;struct Cyc_Absyn_TagType_struct*_tmpB60;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB60=_cycalloc(sizeof(*_tmpB60)),((_tmpB60[0]=((_tmpB61.tag=19,((
_tmpB61.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),_tmpB61)))),_tmpB60)))),(
unsigned int)(yyyylsp[0]).first_line));}break;}case 92: _LL289: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct
Cyc_Absyn_TagType_struct _tmpB64;struct Cyc_Absyn_TagType_struct*_tmpB63;yyval=Cyc_YY23(
Cyc_Parse_type_spec((void*)((_tmpB63=_cycalloc(sizeof(*_tmpB63)),((_tmpB63[0]=((
_tmpB64.tag=19,((_tmpB64.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_iko,
0),_tmpB64)))),_tmpB63)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case
93: _LL28A: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_Absyn_ValueofType_struct
_tmpB67;struct Cyc_Absyn_ValueofType_struct*_tmpB66;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB66=_cycalloc(sizeof(*_tmpB66)),((_tmpB66[0]=((_tmpB67.tag=18,((
_tmpB67.f1=Cyc_yyget_YY3(yyyyvsp[2]),_tmpB67)))),_tmpB66)))),(unsigned int)(
yyyylsp[0]).first_line));}break;}case 94: _LL28B: {union Cyc_YYSTYPE*yyyyvsp=(union
Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY43(Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));break;}
case 95: _LL28C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(
yyyylsp[0]).first_line:(int)0);{struct Cyc_Absyn_Tqual _tmpB68;yyval=Cyc_YY25(((
_tmpB68.print_const=1,((_tmpB68.q_volatile=0,((_tmpB68.q_restrict=0,((_tmpB68.real_const=
1,((_tmpB68.loc=loc,_tmpB68)))))))))));}break;}case 96: _LL28D: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct
Cyc_Absyn_Tqual _tmpB69;yyval=Cyc_YY25(((_tmpB69.print_const=0,((_tmpB69.q_volatile=
1,((_tmpB69.q_restrict=0,((_tmpB69.real_const=0,((_tmpB69.loc=0,_tmpB69)))))))))));}
break;}case 97: _LL28E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Absyn_Tqual _tmpB6A;
yyval=Cyc_YY25(((_tmpB6A.print_const=0,((_tmpB6A.q_volatile=0,((_tmpB6A.q_restrict=
1,((_tmpB6A.real_const=0,((_tmpB6A.loc=0,_tmpB6A)))))))))));}break;}case 98:
_LL28F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union
Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);struct Cyc_Absyn_Enum_td_struct*
_tmpB79;struct Cyc_Absyn_Enumdecl*_tmpB78;struct Cyc_Core_Opt*_tmpB77;struct Cyc_Absyn_Enum_td_struct
_tmpB76;struct Cyc_Absyn_TypeDecl*_tmpB75;struct Cyc_Absyn_TypeDecl*_tmp58B=(
_tmpB75=_cycalloc(sizeof(*_tmpB75)),((_tmpB75->r=(void*)((_tmpB79=_cycalloc(
sizeof(*_tmpB79)),((_tmpB79[0]=((_tmpB76.tag=1,((_tmpB76.f1=((_tmpB78=_cycalloc(
sizeof(*_tmpB78)),((_tmpB78->sc=Cyc_Absyn_Public,((_tmpB78->name=Cyc_yyget_QualId_tok(
yyyyvsp[1]),((_tmpB78->fields=((_tmpB77=_cycalloc(sizeof(*_tmpB77)),((_tmpB77->v=
Cyc_yyget_YY48(yyyyvsp[3]),_tmpB77)))),_tmpB78)))))))),_tmpB76)))),_tmpB79)))),((
_tmpB75->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB75)))));{struct Cyc_Absyn_TypeDeclType_struct
_tmpB7C;struct Cyc_Absyn_TypeDeclType_struct*_tmpB7B;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB7B=_cycalloc(sizeof(*_tmpB7B)),((_tmpB7B[0]=((_tmpB7C.tag=26,((
_tmpB7C.f1=_tmp58B,((_tmpB7C.f2=0,_tmpB7C)))))),_tmpB7B)))),(unsigned int)(
yyyylsp[0]).first_line));}break;}case 99: _LL290: {union Cyc_YYSTYPE*yyyyvsp=(union
Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_Absyn_EnumType_struct
_tmpB7F;struct Cyc_Absyn_EnumType_struct*_tmpB7E;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E[0]=((_tmpB7F.tag=13,((
_tmpB7F.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB7F.f2=0,_tmpB7F)))))),_tmpB7E)))),(
unsigned int)(yyyylsp[0]).first_line));}break;}case 100: _LL291: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct
Cyc_Absyn_AnonEnumType_struct _tmpB82;struct Cyc_Absyn_AnonEnumType_struct*_tmpB81;
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB81=_cycalloc(sizeof(*_tmpB81)),((
_tmpB81[0]=((_tmpB82.tag=14,((_tmpB82.f1=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB82)))),
_tmpB81)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case 101: _LL292: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Absyn_Enumfield*_tmpB83;
yyval=Cyc_YY47(((_tmpB83=_cycalloc(sizeof(*_tmpB83)),((_tmpB83->name=Cyc_yyget_QualId_tok(
yyyyvsp[0]),((_tmpB83->tag=0,((_tmpB83->loc=(unsigned int)(yyyylsp[0]).first_line,
_tmpB83)))))))));}break;}case 102: _LL293: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_Absyn_Enumfield*_tmpB84;
yyval=Cyc_YY47(((_tmpB84=_cycalloc(sizeof(*_tmpB84)),((_tmpB84->name=Cyc_yyget_QualId_tok(
yyyyvsp[0]),((_tmpB84->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),((
_tmpB84->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB84)))))))));}break;}case
103: _LL294: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_List_List*_tmpB85;yyval=
Cyc_YY48(((_tmpB85=_cycalloc(sizeof(*_tmpB85)),((_tmpB85->hd=Cyc_yyget_YY47(
yyyyvsp[0]),((_tmpB85->tl=0,_tmpB85)))))));}break;}case 104: _LL295: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct
Cyc_List_List*_tmpB86;yyval=Cyc_YY48(((_tmpB86=_cycalloc(sizeof(*_tmpB86)),((
_tmpB86->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB86->tl=0,_tmpB86)))))));}break;}
case 105: _LL296: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_List_List*_tmpB87;yyval=
Cyc_YY48(((_tmpB87=_cycalloc(sizeof(*_tmpB87)),((_tmpB87->hd=Cyc_yyget_YY47(
yyyyvsp[0]),((_tmpB87->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB87)))))));}break;}case
106: _LL297: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_Absyn_AnonAggrType_struct
_tmpB8A;struct Cyc_Absyn_AnonAggrType_struct*_tmpB89;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB89=_cycalloc(sizeof(*_tmpB89)),((_tmpB89[0]=((_tmpB8A.tag=12,((
_tmpB8A.f1=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB8A.f2=Cyc_yyget_YY26(yyyyvsp[2]),
_tmpB8A)))))),_tmpB89)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case
107: _LL298: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union
Cyc_YYSTYPE),9);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);struct Cyc_List_List*_tmp59E=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),
unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(
unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY40(yyyyvsp[3]));struct Cyc_List_List*
_tmp59F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),
unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(
unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY40(yyyyvsp[5]));struct Cyc_Absyn_TypeDecl*
_tmp5A0=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(
yyyyvsp[2]),_tmp59E,(struct Cyc_Absyn_AggrdeclImpl*)Cyc_Absyn_aggrdecl_impl(
_tmp59F,Cyc_yyget_YY50(yyyyvsp[6]),Cyc_yyget_YY26(yyyyvsp[7]),1),0,(unsigned int)(
yyyylsp[0]).first_line);{struct Cyc_Absyn_TypeDeclType_struct _tmpB8D;struct Cyc_Absyn_TypeDeclType_struct*
_tmpB8C;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB8C=_cycalloc(sizeof(*
_tmpB8C)),((_tmpB8C[0]=((_tmpB8D.tag=26,((_tmpB8D.f1=_tmp5A0,((_tmpB8D.f2=0,
_tmpB8D)))))),_tmpB8C)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case
108: _LL299: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union
Cyc_YYSTYPE),8);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);struct Cyc_List_List*_tmp5A3=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),
unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(
unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));struct Cyc_List_List*
_tmp5A4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),
unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(
unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY40(yyyyvsp[4]));struct Cyc_Absyn_TypeDecl*
_tmp5A5=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(
yyyyvsp[1]),_tmp5A3,(struct Cyc_Absyn_AggrdeclImpl*)Cyc_Absyn_aggrdecl_impl(
_tmp5A4,Cyc_yyget_YY50(yyyyvsp[5]),Cyc_yyget_YY26(yyyyvsp[6]),0),0,(unsigned int)(
yyyylsp[0]).first_line);{struct Cyc_Absyn_TypeDeclType_struct _tmpB90;struct Cyc_Absyn_TypeDeclType_struct*
_tmpB8F;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB8F=_cycalloc(sizeof(*
_tmpB8F)),((_tmpB8F[0]=((_tmpB90.tag=26,((_tmpB90.f1=_tmp5A5,((_tmpB90.f2=0,
_tmpB90)))))),_tmpB8F)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case
109: _LL29A: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_Absyn_AggrType_struct
_tmpB9A;struct Cyc_Core_Opt*_tmpB99;struct Cyc_Absyn_AggrInfo _tmpB98;struct Cyc_Absyn_AggrType_struct*
_tmpB97;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB97=_cycalloc(sizeof(*
_tmpB97)),((_tmpB97[0]=((_tmpB9A.tag=11,((_tmpB9A.f1=((_tmpB98.aggr_info=Cyc_Absyn_UnknownAggr(
Cyc_yyget_YY24(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB99=
_cycalloc_atomic(sizeof(*_tmpB99)),((_tmpB99->v=(void*)1,_tmpB99))))),((_tmpB98.targs=
Cyc_yyget_YY40(yyyyvsp[3]),_tmpB98)))),_tmpB9A)))),_tmpB97)))),(unsigned int)(
yyyylsp[0]).first_line));}break;}case 110: _LL29B: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_Absyn_AggrType_struct
_tmpBA0;struct Cyc_Absyn_AggrInfo _tmpB9F;struct Cyc_Absyn_AggrType_struct*_tmpB9E;
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB9E=_cycalloc(sizeof(*_tmpB9E)),((
_tmpB9E[0]=((_tmpBA0.tag=11,((_tmpBA0.f1=((_tmpB9F.aggr_info=Cyc_Absyn_UnknownAggr(
Cyc_yyget_YY24(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB9F.targs=
Cyc_yyget_YY40(yyyyvsp[2]),_tmpB9F)))),_tmpBA0)))),_tmpB9E)))),(unsigned int)(
yyyylsp[0]).first_line));}break;}case 111: _LL29C: yyval=Cyc_YY40(0);break;case 112:
_LL29D: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));break;}
case 113: _LL29E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY24(Cyc_Absyn_StructA);
break;}case 114: _LL29F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY24(Cyc_Absyn_UnionA);
break;}case 115: _LL2A0: yyval=Cyc_YY26(0);break;case 116: _LL2A1: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);struct
Cyc_List_List*_tmp5AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyyyvsp[0])));struct Cyc_List_List*_tmp5B0=Cyc_Parse_get_argrfield_tags(_tmp5AF);
if(_tmp5B0 != 0)((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,
_tmp5B0,_tmp5AF);yyval=Cyc_YY26(_tmp5AF);break;}case 117: _LL2A2: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct
Cyc_List_List*_tmpBA1;yyval=Cyc_YY27(((_tmpBA1=_cycalloc(sizeof(*_tmpBA1)),((
_tmpBA1->hd=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpBA1->tl=0,_tmpBA1)))))));}break;}
case 118: _LL2A3: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_List_List*_tmpBA2;yyval=
Cyc_YY27(((_tmpBA2=_cycalloc(sizeof(*_tmpBA2)),((_tmpBA2->hd=Cyc_yyget_YY26(
yyyyvsp[1]),((_tmpBA2->tl=Cyc_yyget_YY27(yyyyvsp[0]),_tmpBA2)))))));}break;}case
119: _LL2A4: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY21(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyyyvsp[0])));break;}
case 120: _LL2A5: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_List_List*_tmpBA3;yyval=
Cyc_YY21(((_tmpBA3=_cycalloc(sizeof(*_tmpBA3)),((_tmpBA3->hd=Cyc_yyget_YY20(
yyyyvsp[0]),((_tmpBA3->tl=0,_tmpBA3)))))));}break;}case 121: _LL2A6: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct
Cyc_List_List*_tmpBA4;yyval=Cyc_YY21(((_tmpBA4=_cycalloc(sizeof(*_tmpBA4)),((
_tmpBA4->hd=Cyc_yyget_YY20(yyyyvsp[2]),((_tmpBA4->tl=Cyc_yyget_YY21(yyyyvsp[0]),
_tmpBA4)))))));}break;}case 122: _LL2A7: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _tuple22*_tmpBA5;yyval=Cyc_YY20(((
_tmpBA5=_cycalloc(sizeof(*_tmpBA5)),((_tmpBA5->f1=Cyc_yyget_YY29(yyyyvsp[0]),((
_tmpBA5->f2=0,_tmpBA5)))))));}break;}case 123: _LL2A8: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct _tuple22*_tmpBA6;yyval=Cyc_YY20(((
_tmpBA6=_cycalloc(sizeof(*_tmpBA6)),((_tmpBA6->f1=Cyc_yyget_YY29(yyyyvsp[0]),((
_tmpBA6->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),_tmpBA6)))))));}
break;}case 124: _LL2A9: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);struct _RegionHandle _tmp5B7=
_new_region("temp");struct _RegionHandle*temp=& _tmp5B7;_push_region(temp);{struct
_tuple23 _tmp5B9;struct Cyc_Absyn_Tqual _tmp5BA;struct Cyc_List_List*_tmp5BB;struct
Cyc_List_List*_tmp5BC;struct _tuple23*_tmp5B8=Cyc_yyget_YY35(yyyyvsp[0]);_tmp5B9=*
_tmp5B8;_tmp5BA=_tmp5B9.f1;_tmp5BB=_tmp5B9.f2;_tmp5BC=_tmp5B9.f3;if(_tmp5BA.loc
== 0)_tmp5BA.loc=(unsigned int)(yyyylsp[0]).first_line;{struct Cyc_List_List*
_tmp5BE;struct Cyc_List_List*_tmp5BF;struct _tuple0 _tmp5BD=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
temp,temp,Cyc_yyget_YY21(yyyyvsp[1]));_tmp5BE=_tmp5BD.f1;_tmp5BF=_tmp5BD.f2;{
void*_tmp5C0=Cyc_Parse_speclist2typ(_tmp5BB,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp5C1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
temp,temp,Cyc_Parse_apply_tmss(temp,_tmp5BA,_tmp5C0,_tmp5BE,_tmp5BC),_tmp5BF);
yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
unsigned int,struct _tuple14*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp5C1));};};}
_npop_handler(0);break;;_pop_region(temp);}case 125: _LL2AA: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct
Cyc_List_List*_tmpBA9;struct _tuple23*_tmpBA8;yyval=Cyc_YY35(((_tmpBA8=_cycalloc(
sizeof(*_tmpBA8)),((_tmpBA8->f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((
_tmpBA8->f2=((_tmpBA9=_cycalloc(sizeof(*_tmpBA9)),((_tmpBA9->hd=Cyc_yyget_YY23(
yyyyvsp[0]),((_tmpBA9->tl=0,_tmpBA9)))))),((_tmpBA8->f3=0,_tmpBA8)))))))));}
break;}case 126: _LL2AB: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct _tuple23*_tmp5C4=Cyc_yyget_YY35(
yyyyvsp[1]);{struct Cyc_List_List*_tmpBAC;struct _tuple23*_tmpBAB;yyval=Cyc_YY35(((
_tmpBAB=_cycalloc(sizeof(*_tmpBAB)),((_tmpBAB->f1=(*_tmp5C4).f1,((_tmpBAB->f2=((
_tmpBAC=_cycalloc(sizeof(*_tmpBAC)),((_tmpBAC->hd=Cyc_yyget_YY23(yyyyvsp[0]),((
_tmpBAC->tl=(*_tmp5C4).f2,_tmpBAC)))))),((_tmpBAB->f3=(*_tmp5C4).f3,_tmpBAB)))))))));}
break;}case 127: _LL2AC: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _tuple23*_tmpBAD;yyval=Cyc_YY35(((
_tmpBAD=_cycalloc(sizeof(*_tmpBAD)),((_tmpBAD->f1=Cyc_yyget_YY25(yyyyvsp[0]),((
_tmpBAD->f2=0,((_tmpBAD->f3=0,_tmpBAD)))))))));}break;}case 128: _LL2AD: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct
_tuple23*_tmp5C8=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple23*_tmpBAE;yyval=Cyc_YY35(((
_tmpBAE=_cycalloc(sizeof(*_tmpBAE)),((_tmpBAE->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyyyvsp[0]),(*_tmp5C8).f1),((_tmpBAE->f2=(*_tmp5C8).f2,((_tmpBAE->f3=(*_tmp5C8).f3,
_tmpBAE)))))))));}break;}case 129: _LL2AE: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _tuple23*_tmpBAF;yyval=Cyc_YY35(((
_tmpBAF=_cycalloc(sizeof(*_tmpBAF)),((_tmpBAF->f1=Cyc_Absyn_empty_tqual((
unsigned int)(yyyylsp[0]).first_line),((_tmpBAF->f2=0,((_tmpBAF->f3=Cyc_yyget_YY45(
yyyyvsp[0]),_tmpBAF)))))))));}break;}case 130: _LL2AF: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct _tuple23*_tmp5CB=Cyc_yyget_YY35(
yyyyvsp[1]);{struct _tuple23*_tmpBB0;yyval=Cyc_YY35(((_tmpBB0=_cycalloc(sizeof(*
_tmpBB0)),((_tmpBB0->f1=(*_tmp5CB).f1,((_tmpBB0->f2=(*_tmp5CB).f2,((_tmpBB0->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyyyvsp[0]),(*_tmp5CB).f3),_tmpBB0)))))))));}break;}case 131:
_LL2B0: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_List_List*_tmpBB3;
struct _tuple23*_tmpBB2;yyval=Cyc_YY35(((_tmpBB2=_cycalloc(sizeof(*_tmpBB2)),((
_tmpBB2->f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((
_tmpBB2->f2=((_tmpBB3=_cycalloc(sizeof(*_tmpBB3)),((_tmpBB3->hd=Cyc_yyget_YY23(
yyyyvsp[0]),((_tmpBB3->tl=0,_tmpBB3)))))),((_tmpBB2->f3=0,_tmpBB2)))))))));}
break;}case 132: _LL2B1: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct _tuple23*_tmp5CF=Cyc_yyget_YY35(
yyyyvsp[1]);{struct Cyc_List_List*_tmpBB6;struct _tuple23*_tmpBB5;yyval=Cyc_YY35(((
_tmpBB5=_cycalloc(sizeof(*_tmpBB5)),((_tmpBB5->f1=(*_tmp5CF).f1,((_tmpBB5->f2=((
_tmpBB6=_cycalloc(sizeof(*_tmpBB6)),((_tmpBB6->hd=Cyc_yyget_YY23(yyyyvsp[0]),((
_tmpBB6->tl=(*_tmp5CF).f2,_tmpBB6)))))),((_tmpBB5->f3=(*_tmp5CF).f3,_tmpBB5)))))))));}
break;}case 133: _LL2B2: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _tuple23*_tmpBB7;yyval=Cyc_YY35(((
_tmpBB7=_cycalloc(sizeof(*_tmpBB7)),((_tmpBB7->f1=Cyc_yyget_YY25(yyyyvsp[0]),((
_tmpBB7->f2=0,((_tmpBB7->f3=0,_tmpBB7)))))))));}break;}case 134: _LL2B3: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct
_tuple23*_tmp5D3=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple23*_tmpBB8;yyval=Cyc_YY35(((
_tmpBB8=_cycalloc(sizeof(*_tmpBB8)),((_tmpBB8->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyyyvsp[0]),(*_tmp5D3).f1),((_tmpBB8->f2=(*_tmp5D3).f2,((_tmpBB8->f3=(*_tmp5D3).f3,
_tmpBB8)))))))));}break;}case 135: _LL2B4: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _tuple23*_tmpBB9;yyval=Cyc_YY35(((
_tmpBB9=_cycalloc(sizeof(*_tmpBB9)),((_tmpBB9->f1=Cyc_Absyn_empty_tqual((
unsigned int)(yyyylsp[0]).first_line),((_tmpBB9->f2=0,((_tmpBB9->f3=Cyc_yyget_YY45(
yyyyvsp[0]),_tmpBB9)))))))));}break;}case 136: _LL2B5: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct _tuple23*_tmp5D6=Cyc_yyget_YY35(
yyyyvsp[1]);{struct _tuple23*_tmpBBA;yyval=Cyc_YY35(((_tmpBBA=_cycalloc(sizeof(*
_tmpBBA)),((_tmpBBA->f1=(*_tmp5D6).f1,((_tmpBBA->f2=(*_tmp5D6).f2,((_tmpBBA->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyyyvsp[0]),(*_tmp5D6).f3),_tmpBBA)))))))));}break;}case 137:
_LL2B6: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY21(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyyyvsp[0])));break;}
case 138: _LL2B7: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_List_List*_tmpBBB;yyval=
Cyc_YY21(((_tmpBBB=_cycalloc(sizeof(*_tmpBBB)),((_tmpBBB->hd=Cyc_yyget_YY20(
yyyyvsp[0]),((_tmpBBB->tl=0,_tmpBBB)))))));}break;}case 139: _LL2B8: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct
Cyc_List_List*_tmpBBC;yyval=Cyc_YY21(((_tmpBBC=_cycalloc(sizeof(*_tmpBBC)),((
_tmpBBC->hd=Cyc_yyget_YY20(yyyyvsp[2]),((_tmpBBC->tl=Cyc_yyget_YY21(yyyyvsp[0]),
_tmpBBC)))))));}break;}case 140: _LL2B9: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _tuple22*_tmpBBD;yyval=Cyc_YY20(((
_tmpBBD=_cycalloc(sizeof(*_tmpBBD)),((_tmpBBD->f1=Cyc_yyget_YY29(yyyyvsp[0]),((
_tmpBBD->f2=0,_tmpBBD)))))));}break;}case 141: _LL2BA: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_Parse_Declarator*
_tmpBC3;struct _tuple1*_tmpBC2;struct _tuple22*_tmpBC1;yyval=Cyc_YY20(((_tmpBC1=
_cycalloc(sizeof(*_tmpBC1)),((_tmpBC1->f1=((_tmpBC3=_cycalloc(sizeof(*_tmpBC3)),((
_tmpBC3->id=((_tmpBC2=_cycalloc(sizeof(*_tmpBC2)),((_tmpBC2->f1=Cyc_Absyn_Rel_n(
0),((_tmpBC2->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",
sizeof(char),1),_tmpBC2)))))),((_tmpBC3->tms=0,_tmpBC3)))))),((_tmpBC1->f2=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[1]),_tmpBC1)))))));}break;}case 142:
_LL2BB:{struct Cyc_Parse_Declarator*_tmpBC9;struct _tuple1*_tmpBC8;struct _tuple22*
_tmpBC7;yyval=Cyc_YY20(((_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7->f1=((
_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9->id=((_tmpBC8=_cycalloc(sizeof(*
_tmpBC8)),((_tmpBC8->f1=Cyc_Absyn_Rel_n(0),((_tmpBC8->f2=_init_dyneither_ptr(
_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpBC8)))))),((
_tmpBC9->tms=0,_tmpBC9)))))),((_tmpBC7->f2=0,_tmpBC7)))))));}break;case 143:
_LL2BC: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct _tuple22*_tmpBCA;yyval=Cyc_YY20(((
_tmpBCA=_cycalloc(sizeof(*_tmpBCA)),((_tmpBCA->f1=Cyc_yyget_YY29(yyyyvsp[0]),((
_tmpBCA->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),_tmpBCA)))))));}
break;}case 144: _LL2BD: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);int _tmp5E4=Cyc_yyget_YY31(yyyyvsp[
0]);struct Cyc_List_List*_tmp5E5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[
2]));struct Cyc_Core_Opt*_tmpBCB;struct Cyc_Absyn_TypeDecl*_tmp5E6=Cyc_Absyn_datatype_tdecl(
Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp5E5,((_tmpBCB=_cycalloc(
sizeof(*_tmpBCB)),((_tmpBCB->v=Cyc_yyget_YY34(yyyyvsp[4]),_tmpBCB)))),_tmp5E4,(
unsigned int)(yyyylsp[0]).first_line);{struct Cyc_Absyn_TypeDeclType_struct
_tmpBCE;struct Cyc_Absyn_TypeDeclType_struct*_tmpBCD;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpBCD=_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD[0]=((_tmpBCE.tag=26,((
_tmpBCE.f1=_tmp5E6,((_tmpBCE.f2=0,_tmpBCE)))))),_tmpBCD)))),(unsigned int)(
yyyylsp[0]).first_line));}break;}case 145: _LL2BE: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);int _tmp5EA=Cyc_yyget_YY31(yyyyvsp[
0]);{struct Cyc_Absyn_DatatypeType_struct _tmpBD8;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpBD7;struct Cyc_Absyn_DatatypeInfo _tmpBD6;struct Cyc_Absyn_DatatypeType_struct*
_tmpBD5;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpBD5=_cycalloc(sizeof(*
_tmpBD5)),((_tmpBD5[0]=((_tmpBD8.tag=3,((_tmpBD8.f1=((_tmpBD6.datatype_info=Cyc_Absyn_UnknownDatatype(((
_tmpBD7.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBD7.is_extensible=_tmp5EA,
_tmpBD7))))),((_tmpBD6.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpBD6)))),_tmpBD8)))),
_tmpBD5)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case 146: _LL2BF: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union
Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);int _tmp5EF=Cyc_yyget_YY31(yyyyvsp[
0]);{struct Cyc_Absyn_DatatypeFieldType_struct _tmpBE2;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpBE1;struct Cyc_Absyn_DatatypeFieldInfo _tmpBE0;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpBDF;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpBDF=_cycalloc(sizeof(*
_tmpBDF)),((_tmpBDF[0]=((_tmpBE2.tag=4,((_tmpBE2.f1=((_tmpBE0.field_info=Cyc_Absyn_UnknownDatatypefield(((
_tmpBE1.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBE1.field_name=Cyc_yyget_QualId_tok(
yyyyvsp[3]),((_tmpBE1.is_extensible=_tmp5EF,_tmpBE1))))))),((_tmpBE0.targs=Cyc_yyget_YY40(
yyyyvsp[4]),_tmpBE0)))),_tmpBE2)))),_tmpBDF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 147: _LL2C0: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY31(0);break;}case 148:
_LL2C1: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY31(1);break;}case 149:
_LL2C2: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_List_List*_tmpBE3;yyval=
Cyc_YY34(((_tmpBE3=_cycalloc(sizeof(*_tmpBE3)),((_tmpBE3->hd=Cyc_yyget_YY33(
yyyyvsp[0]),((_tmpBE3->tl=0,_tmpBE3)))))));}break;}case 150: _LL2C3: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct
Cyc_List_List*_tmpBE4;yyval=Cyc_YY34(((_tmpBE4=_cycalloc(sizeof(*_tmpBE4)),((
_tmpBE4->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBE4->tl=0,_tmpBE4)))))));}break;}
case 151: _LL2C4: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_List_List*_tmpBE5;yyval=
Cyc_YY34(((_tmpBE5=_cycalloc(sizeof(*_tmpBE5)),((_tmpBE5->hd=Cyc_yyget_YY33(
yyyyvsp[0]),((_tmpBE5->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpBE5)))))));}break;}case
152: _LL2C5: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_List_List*_tmpBE6;yyval=
Cyc_YY34(((_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6->hd=Cyc_yyget_YY33(
yyyyvsp[0]),((_tmpBE6->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpBE6)))))));}break;}case
153: _LL2C6: yyval=Cyc_YY32(Cyc_Absyn_Public);break;case 154: _LL2C7: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
Cyc_YY32(Cyc_Absyn_Extern);break;}case 155: _LL2C8: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY32(Cyc_Absyn_Static);
break;}case 156: _LL2C9: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_Absyn_Datatypefield*
_tmpBE7;yyval=Cyc_YY33(((_tmpBE7=_cycalloc(sizeof(*_tmpBE7)),((_tmpBE7->name=Cyc_yyget_QualId_tok(
yyyyvsp[1]),((_tmpBE7->typs=0,((_tmpBE7->loc=(unsigned int)(yyyylsp[0]).first_line,((
_tmpBE7->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpBE7)))))))))));}break;}case 157: _LL2CA: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union
Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);struct Cyc_List_List*_tmp5F9=((
struct Cyc_List_List*(*)(struct _tuple16*(*f)(unsigned int,struct _tuple9*),
unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(
unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));{struct Cyc_Absyn_Datatypefield*
_tmpBE8;yyval=Cyc_YY33(((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8->name=Cyc_yyget_QualId_tok(
yyyyvsp[1]),((_tmpBE8->typs=_tmp5F9,((_tmpBE8->loc=(unsigned int)(yyyylsp[0]).first_line,((
_tmpBE8->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpBE8)))))))))));}break;}case 158: _LL2CB: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 159:
_LL2CC: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_Parse_Declarator*
_tmpBE9;yyval=Cyc_YY29(((_tmpBE9=_cycalloc(sizeof(*_tmpBE9)),((_tmpBE9->id=(Cyc_yyget_YY29(
yyyyvsp[1]))->id,((_tmpBE9->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),(Cyc_yyget_YY29(
yyyyvsp[1]))->tms),_tmpBE9)))))));}break;}case 160: _LL2CD: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
yyyyvsp[0];break;}case 161: _LL2CE: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_Parse_Declarator*
_tmpBEA;yyval=Cyc_YY29(((_tmpBEA=_cycalloc(sizeof(*_tmpBEA)),((_tmpBEA->id=(Cyc_yyget_YY29(
yyyyvsp[1]))->id,((_tmpBEA->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),(Cyc_yyget_YY29(
yyyyvsp[1]))->tms),_tmpBEA)))))));}break;}case 162: _LL2CF: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct
Cyc_Parse_Declarator*_tmpBEB;yyval=Cyc_YY29(((_tmpBEB=_cycalloc(sizeof(*_tmpBEB)),((
_tmpBEB->id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBEB->tms=0,_tmpBEB)))))));}
break;}case 163: _LL2D0: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=yyyyvsp[1];break;}case 164:
_LL2D1: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct Cyc_Parse_Declarator*_tmp5FE=
Cyc_yyget_YY29(yyyyvsp[2]);{struct Cyc_Absyn_Attributes_mod_struct*_tmpBF1;struct
Cyc_Absyn_Attributes_mod_struct _tmpBF0;struct Cyc_List_List*_tmpBEF;_tmp5FE->tms=((
_tmpBEF=_cycalloc(sizeof(*_tmpBEF)),((_tmpBEF->hd=(void*)((_tmpBF1=_cycalloc(
sizeof(*_tmpBF1)),((_tmpBF1[0]=((_tmpBF0.tag=5,((_tmpBF0.f1=(unsigned int)(
yyyylsp[1]).first_line,((_tmpBF0.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpBF0)))))),
_tmpBF1)))),((_tmpBEF->tl=_tmp5FE->tms,_tmpBEF))))));}yyval=yyyyvsp[2];break;}
case 165: _LL2D2: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_List_List*_tmpBFB;
struct Cyc_Absyn_Carray_mod_struct _tmpBFA;struct Cyc_Absyn_Carray_mod_struct*
_tmpBF9;struct Cyc_Parse_Declarator*_tmpBF8;yyval=Cyc_YY29(((_tmpBF8=_cycalloc(
sizeof(*_tmpBF8)),((_tmpBF8->id=(Cyc_yyget_YY29(yyyyvsp[0]))->id,((_tmpBF8->tms=((
_tmpBFB=_cycalloc(sizeof(*_tmpBFB)),((_tmpBFB->hd=(void*)((_tmpBF9=_cycalloc(
sizeof(*_tmpBF9)),((_tmpBF9[0]=((_tmpBFA.tag=0,((_tmpBFA.f1=Cyc_yyget_YY51(
yyyyvsp[3]),((_tmpBFA.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBFA)))))),
_tmpBF9)))),((_tmpBFB->tl=(Cyc_yyget_YY29(yyyyvsp[0]))->tms,_tmpBFB)))))),
_tmpBF8)))))));}break;}case 166: _LL2D3: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);{struct Cyc_List_List*_tmpC05;
struct Cyc_Absyn_ConstArray_mod_struct _tmpC04;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpC03;struct Cyc_Parse_Declarator*_tmpC02;yyval=Cyc_YY29(((_tmpC02=_cycalloc(
sizeof(*_tmpC02)),((_tmpC02->id=(Cyc_yyget_YY29(yyyyvsp[0]))->id,((_tmpC02->tms=((
_tmpC05=_cycalloc(sizeof(*_tmpC05)),((_tmpC05->hd=(void*)((_tmpC03=_cycalloc(
sizeof(*_tmpC03)),((_tmpC03[0]=((_tmpC04.tag=1,((_tmpC04.f1=Cyc_yyget_YY3(
yyyyvsp[2]),((_tmpC04.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpC04.f3=(unsigned int)(
yyyylsp[4]).first_line,_tmpC04)))))))),_tmpC03)))),((_tmpC05->tl=(Cyc_yyget_YY29(
yyyyvsp[0]))->tms,_tmpC05)))))),_tmpC02)))))));}break;}case 167: _LL2D4: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct
_tuple24 _tmp60B;struct Cyc_List_List*_tmp60C;int _tmp60D;struct Cyc_Absyn_VarargInfo*
_tmp60E;struct Cyc_Core_Opt*_tmp60F;struct Cyc_List_List*_tmp610;struct _tuple24*
_tmp60A=Cyc_yyget_YY39(yyyyvsp[2]);_tmp60B=*_tmp60A;_tmp60C=_tmp60B.f1;_tmp60D=
_tmp60B.f2;_tmp60E=_tmp60B.f3;_tmp60F=_tmp60B.f4;_tmp610=_tmp60B.f5;{struct Cyc_List_List*
_tmpC1A;struct Cyc_Absyn_Function_mod_struct _tmpC19;struct Cyc_Absyn_WithTypes_struct
_tmpC18;struct Cyc_Absyn_WithTypes_struct*_tmpC17;struct Cyc_Absyn_Function_mod_struct*
_tmpC16;struct Cyc_Parse_Declarator*_tmpC15;yyval=Cyc_YY29(((_tmpC15=_cycalloc(
sizeof(*_tmpC15)),((_tmpC15->id=(Cyc_yyget_YY29(yyyyvsp[0]))->id,((_tmpC15->tms=((
_tmpC1A=_cycalloc(sizeof(*_tmpC1A)),((_tmpC1A->hd=(void*)((_tmpC16=_cycalloc(
sizeof(*_tmpC16)),((_tmpC16[0]=((_tmpC19.tag=3,((_tmpC19.f1=(void*)((void*)((
_tmpC17=_cycalloc(sizeof(*_tmpC17)),((_tmpC17[0]=((_tmpC18.tag=1,((_tmpC18.f1=
_tmp60C,((_tmpC18.f2=_tmp60D,((_tmpC18.f3=_tmp60E,((_tmpC18.f4=_tmp60F,((_tmpC18.f5=
_tmp610,_tmpC18)))))))))))),_tmpC17))))),_tmpC19)))),_tmpC16)))),((_tmpC1A->tl=(
Cyc_yyget_YY29(yyyyvsp[0]))->tms,_tmpC1A)))))),_tmpC15)))))));}break;}case 168:
_LL2D5: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union
Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);{struct Cyc_List_List*_tmpC2F;
struct Cyc_Absyn_Function_mod_struct _tmpC2E;struct Cyc_Absyn_WithTypes_struct
_tmpC2D;struct Cyc_Absyn_WithTypes_struct*_tmpC2C;struct Cyc_Absyn_Function_mod_struct*
_tmpC2B;struct Cyc_Parse_Declarator*_tmpC2A;yyval=Cyc_YY29(((_tmpC2A=_cycalloc(
sizeof(*_tmpC2A)),((_tmpC2A->id=(Cyc_yyget_YY29(yyyyvsp[0]))->id,((_tmpC2A->tms=((
_tmpC2F=_cycalloc(sizeof(*_tmpC2F)),((_tmpC2F->hd=(void*)((_tmpC2B=_cycalloc(
sizeof(*_tmpC2B)),((_tmpC2B[0]=((_tmpC2E.tag=3,((_tmpC2E.f1=(void*)((void*)((
_tmpC2C=_cycalloc(sizeof(*_tmpC2C)),((_tmpC2C[0]=((_tmpC2D.tag=1,((_tmpC2D.f1=0,((
_tmpC2D.f2=0,((_tmpC2D.f3=0,((_tmpC2D.f4=Cyc_yyget_YY49(yyyyvsp[2]),((_tmpC2D.f5=
Cyc_yyget_YY50(yyyyvsp[3]),_tmpC2D)))))))))))),_tmpC2C))))),_tmpC2E)))),_tmpC2B)))),((
_tmpC2F->tl=(Cyc_yyget_YY29(yyyyvsp[0]))->tms,_tmpC2F)))))),_tmpC2A)))))));}
break;}case 169: _LL2D6: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_List_List*_tmpC44;
struct Cyc_Absyn_Function_mod_struct _tmpC43;struct Cyc_Absyn_NoTypes_struct _tmpC42;
struct Cyc_Absyn_NoTypes_struct*_tmpC41;struct Cyc_Absyn_Function_mod_struct*
_tmpC40;struct Cyc_Parse_Declarator*_tmpC3F;yyval=Cyc_YY29(((_tmpC3F=_cycalloc(
sizeof(*_tmpC3F)),((_tmpC3F->id=(Cyc_yyget_YY29(yyyyvsp[0]))->id,((_tmpC3F->tms=((
_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44->hd=(void*)((_tmpC40=_cycalloc(
sizeof(*_tmpC40)),((_tmpC40[0]=((_tmpC43.tag=3,((_tmpC43.f1=(void*)((void*)((
_tmpC41=_cycalloc(sizeof(*_tmpC41)),((_tmpC41[0]=((_tmpC42.tag=0,((_tmpC42.f1=
Cyc_yyget_YY36(yyyyvsp[2]),((_tmpC42.f2=(unsigned int)(yyyylsp[0]).first_line,
_tmpC42)))))),_tmpC41))))),_tmpC43)))),_tmpC40)))),((_tmpC44->tl=(Cyc_yyget_YY29(
yyyyvsp[0]))->tms,_tmpC44)))))),_tmpC3F)))))));}break;}case 170: _LL2D7: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct
Cyc_List_List*_tmp623=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(
unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));{struct Cyc_List_List*_tmpC4E;
struct Cyc_Absyn_TypeParams_mod_struct _tmpC4D;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpC4C;struct Cyc_Parse_Declarator*_tmpC4B;yyval=Cyc_YY29(((_tmpC4B=_cycalloc(
sizeof(*_tmpC4B)),((_tmpC4B->id=(Cyc_yyget_YY29(yyyyvsp[0]))->id,((_tmpC4B->tms=((
_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E->hd=(void*)((_tmpC4C=_cycalloc(
sizeof(*_tmpC4C)),((_tmpC4C[0]=((_tmpC4D.tag=4,((_tmpC4D.f1=_tmp623,((_tmpC4D.f2=(
unsigned int)(yyyylsp[0]).first_line,((_tmpC4D.f3=0,_tmpC4D)))))))),_tmpC4C)))),((
_tmpC4E->tl=(Cyc_yyget_YY29(yyyyvsp[0]))->tms,_tmpC4E)))))),_tmpC4B)))))));}
break;}case 171: _LL2D8: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_List_List*_tmpC58;
struct Cyc_Absyn_Attributes_mod_struct _tmpC57;struct Cyc_Absyn_Attributes_mod_struct*
_tmpC56;struct Cyc_Parse_Declarator*_tmpC55;yyval=Cyc_YY29(((_tmpC55=_cycalloc(
sizeof(*_tmpC55)),((_tmpC55->id=(Cyc_yyget_YY29(yyyyvsp[0]))->id,((_tmpC55->tms=((
_tmpC58=_cycalloc(sizeof(*_tmpC58)),((_tmpC58->hd=(void*)((_tmpC56=_cycalloc(
sizeof(*_tmpC56)),((_tmpC56[0]=((_tmpC57.tag=5,((_tmpC57.f1=(unsigned int)(
yyyylsp[1]).first_line,((_tmpC57.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC57)))))),
_tmpC56)))),((_tmpC58->tl=(Cyc_yyget_YY29(yyyyvsp[0]))->tms,_tmpC58)))))),
_tmpC55)))))));}break;}case 172: _LL2D9: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Parse_Declarator*
_tmpC59;yyval=Cyc_YY29(((_tmpC59=_cycalloc(sizeof(*_tmpC59)),((_tmpC59->id=Cyc_yyget_QualId_tok(
yyyyvsp[0]),((_tmpC59->tms=0,_tmpC59)))))));}break;}case 173: _LL2DA: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct
Cyc_Parse_Declarator*_tmpC5A;yyval=Cyc_YY29(((_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((
_tmpC5A->id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpC5A->tms=0,_tmpC5A)))))));}
break;}case 174: _LL2DB: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=yyyyvsp[1];break;}case 175:
_LL2DC: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct Cyc_Parse_Declarator*_tmp62E=
Cyc_yyget_YY29(yyyyvsp[2]);{struct Cyc_Absyn_Attributes_mod_struct*_tmpC60;struct
Cyc_Absyn_Attributes_mod_struct _tmpC5F;struct Cyc_List_List*_tmpC5E;_tmp62E->tms=((
_tmpC5E=_cycalloc(sizeof(*_tmpC5E)),((_tmpC5E->hd=(void*)((_tmpC60=_cycalloc(
sizeof(*_tmpC60)),((_tmpC60[0]=((_tmpC5F.tag=5,((_tmpC5F.f1=(unsigned int)(
yyyylsp[1]).first_line,((_tmpC5F.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC5F)))))),
_tmpC60)))),((_tmpC5E->tl=_tmp62E->tms,_tmpC5E))))));}yyval=yyyyvsp[2];break;}
case 176: _LL2DD: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct Cyc_Parse_Declarator*_tmp632=
Cyc_yyget_YY29(yyyyvsp[0]);{struct Cyc_List_List*_tmpC6A;struct Cyc_Absyn_Carray_mod_struct
_tmpC69;struct Cyc_Absyn_Carray_mod_struct*_tmpC68;struct Cyc_Parse_Declarator*
_tmpC67;yyval=Cyc_YY29(((_tmpC67=_cycalloc(sizeof(*_tmpC67)),((_tmpC67->id=
_tmp632->id,((_tmpC67->tms=((_tmpC6A=_cycalloc(sizeof(*_tmpC6A)),((_tmpC6A->hd=(
void*)((_tmpC68=_cycalloc(sizeof(*_tmpC68)),((_tmpC68[0]=((_tmpC69.tag=0,((
_tmpC69.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpC69.f2=(unsigned int)(yyyylsp[3]).first_line,
_tmpC69)))))),_tmpC68)))),((_tmpC6A->tl=_tmp632->tms,_tmpC6A)))))),_tmpC67)))))));}
break;}case 177: _LL2DE: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);struct Cyc_Parse_Declarator*_tmp637=
Cyc_yyget_YY29(yyyyvsp[0]);{struct Cyc_List_List*_tmpC74;struct Cyc_Absyn_ConstArray_mod_struct
_tmpC73;struct Cyc_Absyn_ConstArray_mod_struct*_tmpC72;struct Cyc_Parse_Declarator*
_tmpC71;yyval=Cyc_YY29(((_tmpC71=_cycalloc(sizeof(*_tmpC71)),((_tmpC71->id=
_tmp637->id,((_tmpC71->tms=((_tmpC74=_cycalloc(sizeof(*_tmpC74)),((_tmpC74->hd=(
void*)((_tmpC72=_cycalloc(sizeof(*_tmpC72)),((_tmpC72[0]=((_tmpC73.tag=1,((
_tmpC73.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpC73.f2=Cyc_yyget_YY51(yyyyvsp[4]),((
_tmpC73.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpC73)))))))),_tmpC72)))),((
_tmpC74->tl=_tmp637->tms,_tmpC74)))))),_tmpC71)))))));}break;}case 178: _LL2DF: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct _tuple24 _tmp63D;struct Cyc_List_List*
_tmp63E;int _tmp63F;struct Cyc_Absyn_VarargInfo*_tmp640;struct Cyc_Core_Opt*_tmp641;
struct Cyc_List_List*_tmp642;struct _tuple24*_tmp63C=Cyc_yyget_YY39(yyyyvsp[2]);
_tmp63D=*_tmp63C;_tmp63E=_tmp63D.f1;_tmp63F=_tmp63D.f2;_tmp640=_tmp63D.f3;
_tmp641=_tmp63D.f4;_tmp642=_tmp63D.f5;{struct Cyc_Parse_Declarator*_tmp643=Cyc_yyget_YY29(
yyyyvsp[0]);{struct Cyc_List_List*_tmpC89;struct Cyc_Absyn_Function_mod_struct
_tmpC88;struct Cyc_Absyn_WithTypes_struct _tmpC87;struct Cyc_Absyn_WithTypes_struct*
_tmpC86;struct Cyc_Absyn_Function_mod_struct*_tmpC85;struct Cyc_Parse_Declarator*
_tmpC84;yyval=Cyc_YY29(((_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84->id=
_tmp643->id,((_tmpC84->tms=((_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89->hd=(
void*)((_tmpC85=_cycalloc(sizeof(*_tmpC85)),((_tmpC85[0]=((_tmpC88.tag=3,((
_tmpC88.f1=(void*)((void*)((_tmpC86=_cycalloc(sizeof(*_tmpC86)),((_tmpC86[0]=((
_tmpC87.tag=1,((_tmpC87.f1=_tmp63E,((_tmpC87.f2=_tmp63F,((_tmpC87.f3=_tmp640,((
_tmpC87.f4=_tmp641,((_tmpC87.f5=_tmp642,_tmpC87)))))))))))),_tmpC86))))),_tmpC88)))),
_tmpC85)))),((_tmpC89->tl=_tmp643->tms,_tmpC89)))))),_tmpC84)))))));}break;};}
case 179: _LL2E0: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);struct Cyc_Parse_Declarator*_tmp64A=
Cyc_yyget_YY29(yyyyvsp[0]);{struct Cyc_List_List*_tmpC9E;struct Cyc_Absyn_Function_mod_struct
_tmpC9D;struct Cyc_Absyn_WithTypes_struct _tmpC9C;struct Cyc_Absyn_WithTypes_struct*
_tmpC9B;struct Cyc_Absyn_Function_mod_struct*_tmpC9A;struct Cyc_Parse_Declarator*
_tmpC99;yyval=Cyc_YY29(((_tmpC99=_cycalloc(sizeof(*_tmpC99)),((_tmpC99->id=
_tmp64A->id,((_tmpC99->tms=((_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E->hd=(
void*)((_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((_tmpC9A[0]=((_tmpC9D.tag=3,((
_tmpC9D.f1=(void*)((void*)((_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B[0]=((
_tmpC9C.tag=1,((_tmpC9C.f1=0,((_tmpC9C.f2=0,((_tmpC9C.f3=0,((_tmpC9C.f4=Cyc_yyget_YY49(
yyyyvsp[2]),((_tmpC9C.f5=Cyc_yyget_YY50(yyyyvsp[3]),_tmpC9C)))))))))))),_tmpC9B))))),
_tmpC9D)))),_tmpC9A)))),((_tmpC9E->tl=_tmp64A->tms,_tmpC9E)))))),_tmpC99)))))));}
break;}case 180: _LL2E1: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct Cyc_Parse_Declarator*_tmp651=
Cyc_yyget_YY29(yyyyvsp[0]);{struct Cyc_List_List*_tmpCB3;struct Cyc_Absyn_Function_mod_struct
_tmpCB2;struct Cyc_Absyn_NoTypes_struct _tmpCB1;struct Cyc_Absyn_NoTypes_struct*
_tmpCB0;struct Cyc_Absyn_Function_mod_struct*_tmpCAF;struct Cyc_Parse_Declarator*
_tmpCAE;yyval=Cyc_YY29(((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE->id=
_tmp651->id,((_tmpCAE->tms=((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3->hd=(
void*)((_tmpCAF=_cycalloc(sizeof(*_tmpCAF)),((_tmpCAF[0]=((_tmpCB2.tag=3,((
_tmpCB2.f1=(void*)((void*)((_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0[0]=((
_tmpCB1.tag=0,((_tmpCB1.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpCB1.f2=(unsigned int)(
yyyylsp[0]).first_line,_tmpCB1)))))),_tmpCB0))))),_tmpCB2)))),_tmpCAF)))),((
_tmpCB3->tl=_tmp651->tms,_tmpCB3)))))),_tmpCAE)))))));}break;}case 181: _LL2E2: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct Cyc_List_List*_tmp658=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),
unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(
unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));struct Cyc_Parse_Declarator*
_tmp659=Cyc_yyget_YY29(yyyyvsp[0]);{struct Cyc_List_List*_tmpCBD;struct Cyc_Absyn_TypeParams_mod_struct
_tmpCBC;struct Cyc_Absyn_TypeParams_mod_struct*_tmpCBB;struct Cyc_Parse_Declarator*
_tmpCBA;yyval=Cyc_YY29(((_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA->id=
_tmp659->id,((_tmpCBA->tms=((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD->hd=(
void*)((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB[0]=((_tmpCBC.tag=4,((
_tmpCBC.f1=_tmp658,((_tmpCBC.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpCBC.f3=
0,_tmpCBC)))))))),_tmpCBB)))),((_tmpCBD->tl=_tmp659->tms,_tmpCBD)))))),_tmpCBA)))))));}
break;}case 182: _LL2E3: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct Cyc_Parse_Declarator*_tmp65E=
Cyc_yyget_YY29(yyyyvsp[0]);{struct Cyc_List_List*_tmpCC7;struct Cyc_Absyn_Attributes_mod_struct
_tmpCC6;struct Cyc_Absyn_Attributes_mod_struct*_tmpCC5;struct Cyc_Parse_Declarator*
_tmpCC4;yyval=Cyc_YY29(((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4->id=
_tmp65E->id,((_tmpCC4->tms=((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7->hd=(
void*)((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5[0]=((_tmpCC6.tag=5,((
_tmpCC6.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpCC6.f2=Cyc_yyget_YY45(
yyyyvsp[1]),_tmpCC6)))))),_tmpCC5)))),((_tmpCC7->tl=_tmp65E->tms,_tmpCC7)))))),
_tmpCC4)))))));}break;}case 183: _LL2E4: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 184:
_LL2E5: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY28(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(
yyyyvsp[0]),Cyc_yyget_YY28(yyyyvsp[1])));break;}case 185: _LL2E6: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);struct
Cyc_List_List*ans=0;if(Cyc_yyget_YY45(yyyyvsp[3])!= 0){struct Cyc_Absyn_Attributes_mod_struct*
_tmpCCD;struct Cyc_Absyn_Attributes_mod_struct _tmpCCC;struct Cyc_List_List*_tmpCCB;
ans=((_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB->hd=(void*)((_tmpCCD=
_cycalloc(sizeof(*_tmpCCD)),((_tmpCCD[0]=((_tmpCCC.tag=5,((_tmpCCC.f1=(
unsigned int)(yyyylsp[3]).first_line,((_tmpCCC.f2=Cyc_yyget_YY45(yyyyvsp[3]),
_tmpCCC)))))),_tmpCCD)))),((_tmpCCB->tl=ans,_tmpCCB))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;unsigned int _tmp667;union Cyc_Absyn_Constraint*_tmp668;union Cyc_Absyn_Constraint*
_tmp669;struct _tuple19 _tmp666=*Cyc_yyget_YY1(yyyyvsp[0]);_tmp667=_tmp666.f1;
_tmp668=_tmp666.f2;_tmp669=_tmp666.f3;if(Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*
_tmpCCE;ptrloc=((_tmpCCE=_cycalloc_atomic(sizeof(*_tmpCCE)),((_tmpCCE->ptr_loc=
_tmp667,((_tmpCCE->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpCCE->zt_loc=(
unsigned int)(yyyylsp[1]).first_line,_tmpCCE))))))));}{union Cyc_Absyn_Constraint*
_tmp66C;union Cyc_Absyn_Constraint*_tmp66D;union Cyc_Absyn_Constraint*_tmp66E;void*
_tmp66F;struct _tuple12 _tmp66B=Cyc_Parse_collapse_pointer_quals(_tmp667,_tmp668,
_tmp669,Cyc_yyget_YY44(yyyyvsp[2]),Cyc_yyget_YY54(yyyyvsp[1]));_tmp66C=_tmp66B.f1;
_tmp66D=_tmp66B.f2;_tmp66E=_tmp66B.f3;_tmp66F=_tmp66B.f4;{struct Cyc_Absyn_Pointer_mod_struct*
_tmpCD8;struct Cyc_Absyn_PtrAtts _tmpCD7;struct Cyc_Absyn_Pointer_mod_struct _tmpCD6;
struct Cyc_List_List*_tmpCD5;ans=((_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->hd=(
void*)((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8[0]=((_tmpCD6.tag=2,((
_tmpCD6.f1=((_tmpCD7.rgn=_tmp66F,((_tmpCD7.nullable=_tmp66C,((_tmpCD7.bounds=
_tmp66D,((_tmpCD7.zero_term=_tmp66E,((_tmpCD7.ptrloc=ptrloc,_tmpCD7)))))))))),((
_tmpCD6.f2=Cyc_yyget_YY25(yyyyvsp[4]),_tmpCD6)))))),_tmpCD8)))),((_tmpCD5->tl=
ans,_tmpCD5))))));}yyval=Cyc_YY28(ans);break;};};}case 186: _LL2E7: yyval=Cyc_YY54(
0);break;case 187: _LL2E8: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_List_List*_tmpCD9;yyval=
Cyc_YY54(((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9->hd=Cyc_yyget_YY53(
yyyyvsp[0]),((_tmpCD9->tl=Cyc_yyget_YY54(yyyyvsp[1]),_tmpCD9)))))));}break;}case
188: _LL2E9: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_Absyn_Numelts_ptrqual_struct
_tmpCDC;struct Cyc_Absyn_Numelts_ptrqual_struct*_tmpCDB;yyval=Cyc_YY53((void*)((
_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB[0]=((_tmpCDC.tag=0,((_tmpCDC.f1=
Cyc_yyget_YY3(yyyyvsp[2]),_tmpCDC)))),_tmpCDB)))));}break;}case 189: _LL2EA: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_Absyn_Region_ptrqual_struct
_tmpCDF;struct Cyc_Absyn_Region_ptrqual_struct*_tmpCDE;yyval=Cyc_YY53((void*)((
_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE[0]=((_tmpCDF.tag=1,((_tmpCDF.f1=(
void*)Cyc_yyget_YY44(yyyyvsp[2]),_tmpCDF)))),_tmpCDE)))));}break;}case 190: _LL2EB: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Absyn_Thin_ptrqual_struct
_tmpCE2;struct Cyc_Absyn_Thin_ptrqual_struct*_tmpCE1;yyval=Cyc_YY53((void*)((
_tmpCE1=_cycalloc_atomic(sizeof(*_tmpCE1)),((_tmpCE1[0]=((_tmpCE2.tag=2,_tmpCE2)),
_tmpCE1)))));}break;}case 191: _LL2EC: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Absyn_Fat_ptrqual_struct
_tmpCE5;struct Cyc_Absyn_Fat_ptrqual_struct*_tmpCE4;yyval=Cyc_YY53((void*)((
_tmpCE4=_cycalloc_atomic(sizeof(*_tmpCE4)),((_tmpCE4[0]=((_tmpCE5.tag=3,_tmpCE5)),
_tmpCE4)))));}break;}case 192: _LL2ED: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Absyn_Zeroterm_ptrqual_struct
_tmpCE8;struct Cyc_Absyn_Zeroterm_ptrqual_struct*_tmpCE7;yyval=Cyc_YY53((void*)((
_tmpCE7=_cycalloc_atomic(sizeof(*_tmpCE7)),((_tmpCE7[0]=((_tmpCE8.tag=4,_tmpCE8)),
_tmpCE7)))));}break;}case 193: _LL2EE: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Absyn_Nozeroterm_ptrqual_struct
_tmpCEB;struct Cyc_Absyn_Nozeroterm_ptrqual_struct*_tmpCEA;yyval=Cyc_YY53((void*)((
_tmpCEA=_cycalloc_atomic(sizeof(*_tmpCEA)),((_tmpCEA[0]=((_tmpCEB.tag=5,_tmpCEB)),
_tmpCEA)))));}break;}case 194: _LL2EF: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Absyn_Notnull_ptrqual_struct
_tmpCEE;struct Cyc_Absyn_Notnull_ptrqual_struct*_tmpCED;yyval=Cyc_YY53((void*)((
_tmpCED=_cycalloc_atomic(sizeof(*_tmpCED)),((_tmpCED[0]=((_tmpCEE.tag=6,_tmpCEE)),
_tmpCED)))));}break;}case 195: _LL2F0: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Absyn_Nullable_ptrqual_struct
_tmpCF1;struct Cyc_Absyn_Nullable_ptrqual_struct*_tmpCF0;yyval=Cyc_YY53((void*)((
_tmpCF0=_cycalloc_atomic(sizeof(*_tmpCF0)),((_tmpCF0[0]=((_tmpCF1.tag=7,_tmpCF1)),
_tmpCF0)))));}break;}case 196: _LL2F1: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);unsigned int loc=(unsigned int)(
yyyylsp[0]).first_line;{struct _tuple19*_tmpCF2;yyval=Cyc_YY1(((_tmpCF2=_cycalloc(
sizeof(*_tmpCF2)),((_tmpCF2->f1=loc,((_tmpCF2->f2=Cyc_Absyn_true_conref,((
_tmpCF2->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCF2)))))))));}break;}case 197: _LL2F2: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);unsigned int loc=(unsigned int)(
yyyylsp[0]).first_line;{struct _tuple19*_tmpCF3;yyval=Cyc_YY1(((_tmpCF3=_cycalloc(
sizeof(*_tmpCF3)),((_tmpCF3->f1=loc,((_tmpCF3->f2=Cyc_Absyn_false_conref,((
_tmpCF3->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCF3)))))))));}break;}case 198: _LL2F3: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);unsigned int loc=(unsigned int)(
yyyylsp[0]).first_line;{struct _tuple19*_tmpCF4;yyval=Cyc_YY1(((_tmpCF4=_cycalloc(
sizeof(*_tmpCF4)),((_tmpCF4->f1=loc,((_tmpCF4->f2=Cyc_Absyn_true_conref,((
_tmpCF4->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpCF4)))))))));}break;}case 199:
_LL2F4: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);break;case 200: _LL2F5: {union
Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(
struct Cyc_Yyltype),3);{struct Cyc_Absyn_Upper_b_struct _tmpCF7;struct Cyc_Absyn_Upper_b_struct*
_tmpCF6;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6[0]=((_tmpCF7.tag=1,((
_tmpCF7.f1=Cyc_yyget_YY3(yyyyvsp[1]),_tmpCF7)))),_tmpCF6))))));}break;}case 201:
_LL2F6: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 202: _LL2F7: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY51(Cyc_Absyn_true_conref);
break;}case 203: _LL2F8: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY51(Cyc_Absyn_false_conref);
break;}case 204: _LL2F9: yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,0));break;case 205: _LL2FA: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];break;}case 206: _LL2FB: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));break;}
case 207: _LL2FC: yyval=Cyc_YY25(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 208: _LL2FD: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_YY25(yyyyvsp[1])));break;}case 209: _LL2FE: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct _tuple24*_tmpCF8;yyval=Cyc_YY39(((
_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((_tmpCF8->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCF8->f2=
0,((_tmpCF8->f3=0,((_tmpCF8->f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpCF8->f5=Cyc_yyget_YY50(
yyyyvsp[2]),_tmpCF8)))))))))))));}break;}case 210: _LL2FF: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);{struct
_tuple24*_tmpCF9;yyval=Cyc_YY39(((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyyyvsp[0])),((_tmpCF9->f2=1,((_tmpCF9->f3=0,((_tmpCF9->f4=Cyc_yyget_YY49(
yyyyvsp[3]),((_tmpCF9->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCF9)))))))))))));}
break;}case 211: _LL300: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);struct _tuple9 _tmp68D;struct Cyc_Core_Opt*
_tmp68E;struct Cyc_Absyn_Tqual _tmp68F;void*_tmp690;struct _tuple9*_tmp68C=Cyc_yyget_YY37(
yyyyvsp[2]);_tmp68D=*_tmp68C;_tmp68E=_tmp68D.f1;_tmp68F=_tmp68D.f2;_tmp690=
_tmp68D.f3;{struct Cyc_Absyn_VarargInfo*_tmpCFA;struct Cyc_Absyn_VarargInfo*
_tmp691=(_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA->name=_tmp68E,((_tmpCFA->tq=
_tmp68F,((_tmpCFA->type=_tmp690,((_tmpCFA->inject=Cyc_yyget_YY31(yyyyvsp[1]),
_tmpCFA)))))))));{struct _tuple24*_tmpCFB;yyval=Cyc_YY39(((_tmpCFB=_cycalloc(
sizeof(*_tmpCFB)),((_tmpCFB->f1=0,((_tmpCFB->f2=0,((_tmpCFB->f3=_tmp691,((
_tmpCFB->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpCFB->f5=Cyc_yyget_YY50(yyyyvsp[4]),
_tmpCFB)))))))))))));}break;};}case 212: _LL301: {union Cyc_YYSTYPE*yyyyvsp=(union
Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);struct _tuple9 _tmp695;struct Cyc_Core_Opt*
_tmp696;struct Cyc_Absyn_Tqual _tmp697;void*_tmp698;struct _tuple9*_tmp694=Cyc_yyget_YY37(
yyyyvsp[4]);_tmp695=*_tmp694;_tmp696=_tmp695.f1;_tmp697=_tmp695.f2;_tmp698=
_tmp695.f3;{struct Cyc_Absyn_VarargInfo*_tmpCFC;struct Cyc_Absyn_VarargInfo*
_tmp699=(_tmpCFC=_cycalloc(sizeof(*_tmpCFC)),((_tmpCFC->name=_tmp696,((_tmpCFC->tq=
_tmp697,((_tmpCFC->type=_tmp698,((_tmpCFC->inject=Cyc_yyget_YY31(yyyyvsp[3]),
_tmpCFC)))))))));{struct _tuple24*_tmpCFD;yyval=Cyc_YY39(((_tmpCFD=_cycalloc(
sizeof(*_tmpCFD)),((_tmpCFD->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCFD->f2=0,((_tmpCFD->f3=
_tmp699,((_tmpCFD->f4=Cyc_yyget_YY49(yyyyvsp[5]),((_tmpCFD->f5=Cyc_yyget_YY50(
yyyyvsp[6]),_tmpCFD)))))))))))));}break;};}case 213: _LL302: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct
Cyc_Absyn_Unknown_kb_struct _tmpD00;struct Cyc_Absyn_Unknown_kb_struct*_tmpCFF;
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((
_tmpCFF=_cycalloc(sizeof(*_tmpCFF)),((_tmpCFF[0]=((_tmpD00.tag=1,((_tmpD00.f1=0,
_tmpD00)))),_tmpCFF))))));}break;}case 214: _LL303: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY44(Cyc_Parse_id2type(
Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[
2]))));break;}case 215: _LL304: yyval=Cyc_YY49(0);break;case 216: _LL305: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct
Cyc_Absyn_JoinEff_struct*_tmpD06;struct Cyc_Absyn_JoinEff_struct _tmpD05;struct Cyc_Core_Opt*
_tmpD04;yyval=Cyc_YY49(((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04->v=(void*)((
_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06[0]=((_tmpD05.tag=24,((_tmpD05.f1=
Cyc_yyget_YY40(yyyyvsp[1]),_tmpD05)))),_tmpD06)))),_tmpD04)))));}break;}case 217:
_LL306: yyval=Cyc_YY50(0);break;case 218: _LL307: {union Cyc_YYSTYPE*yyyyvsp=(union
Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=yyyyvsp[1];break;}case 219:
_LL308: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);struct Cyc_Absyn_Less_kb_struct
_tmpD09;struct Cyc_Absyn_Less_kb_struct*_tmpD08;struct Cyc_Absyn_Less_kb_struct*
_tmp6A1=(_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08[0]=((_tmpD09.tag=2,((
_tmpD09.f1=0,((_tmpD09.f2=& Cyc_Tcutil_trk,_tmpD09)))))),_tmpD08)));struct
_dyneither_ptr _tmp6A2=Cyc_yyget_String_tok(yyyyvsp[2]);void*_tmp6A3=Cyc_Parse_id2type(
_tmp6A2,(void*)_tmp6A1);{struct _tuple27*_tmpD13;struct Cyc_Absyn_JoinEff_struct
_tmpD12;struct Cyc_Absyn_JoinEff_struct*_tmpD11;struct Cyc_List_List*_tmpD10;yyval=
Cyc_YY50(((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10->hd=((_tmpD13=_cycalloc(
sizeof(*_tmpD13)),((_tmpD13->f1=(void*)((_tmpD11=_cycalloc(sizeof(*_tmpD11)),((
_tmpD11[0]=((_tmpD12.tag=24,((_tmpD12.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpD12)))),
_tmpD11)))),((_tmpD13->f2=_tmp6A3,_tmpD13)))))),((_tmpD10->tl=0,_tmpD10)))))));}
break;}case 220: _LL309: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);struct Cyc_Absyn_Less_kb_struct
_tmpD16;struct Cyc_Absyn_Less_kb_struct*_tmpD15;struct Cyc_Absyn_Less_kb_struct*
_tmp6AA=(_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15[0]=((_tmpD16.tag=2,((
_tmpD16.f1=0,((_tmpD16.f2=& Cyc_Tcutil_trk,_tmpD16)))))),_tmpD15)));struct
_dyneither_ptr _tmp6AB=Cyc_yyget_String_tok(yyyyvsp[2]);void*_tmp6AC=Cyc_Parse_id2type(
_tmp6AB,(void*)_tmp6AA);{struct _tuple27*_tmpD20;struct Cyc_Absyn_JoinEff_struct
_tmpD1F;struct Cyc_Absyn_JoinEff_struct*_tmpD1E;struct Cyc_List_List*_tmpD1D;yyval=
Cyc_YY50(((_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->hd=((_tmpD20=_cycalloc(
sizeof(*_tmpD20)),((_tmpD20->f1=(void*)((_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((
_tmpD1E[0]=((_tmpD1F.tag=24,((_tmpD1F.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpD1F)))),
_tmpD1E)))),((_tmpD20->f2=_tmp6AC,_tmpD20)))))),((_tmpD1D->tl=Cyc_yyget_YY50(
yyyyvsp[4]),_tmpD1D)))))));}break;}case 221: _LL30A: yyval=Cyc_YY31(0);break;case
222: _LL30B: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{const char*_tmpD21;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpD21="inject",
_tag_dyneither(_tmpD21,sizeof(char),7))))!= 0){const char*_tmpD22;Cyc_Parse_err(((
_tmpD22="missing type in function declaration",_tag_dyneither(_tmpD22,sizeof(
char),37))),(unsigned int)(yyyylsp[0]).first_line);}}yyval=Cyc_YY31(1);break;}
case 223: _LL30C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 224:
_LL30D: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(
yyyyvsp[0]),Cyc_yyget_YY40(yyyyvsp[2])));break;}case 225: _LL30E: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=
Cyc_YY40(0);break;}case 226: _LL30F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=yyyyvsp[1];break;}case 227:
_LL310: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_Absyn_RgnsEff_struct*
_tmpD28;struct Cyc_Absyn_RgnsEff_struct _tmpD27;struct Cyc_List_List*_tmpD26;yyval=
Cyc_YY40(((_tmpD26=_cycalloc(sizeof(*_tmpD26)),((_tmpD26->hd=(void*)((_tmpD28=
_cycalloc(sizeof(*_tmpD28)),((_tmpD28[0]=((_tmpD27.tag=25,((_tmpD27.f1=(void*)
Cyc_yyget_YY44(yyyyvsp[2]),_tmpD27)))),_tmpD28)))),((_tmpD26->tl=0,_tmpD26)))))));}
break;}case 228: _LL311: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyyyvsp[0]),& Cyc_Tcutil_ek,0);{struct Cyc_List_List*_tmpD29;yyval=Cyc_YY40(((
_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29->hd=Cyc_yyget_YY44(yyyyvsp[0]),((
_tmpD29->tl=0,_tmpD29)))))));}break;}case 229: _LL312: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyyyvsp[0]),& Cyc_Tcutil_trk,1);{struct Cyc_Absyn_AccessEff_struct*_tmpD2F;struct
Cyc_Absyn_AccessEff_struct _tmpD2E;struct Cyc_List_List*_tmpD2D;yyval=Cyc_YY40(((
_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D->hd=(void*)((_tmpD2F=_cycalloc(
sizeof(*_tmpD2F)),((_tmpD2F[0]=((_tmpD2E.tag=23,((_tmpD2E.f1=(void*)Cyc_yyget_YY44(
yyyyvsp[0]),_tmpD2E)))),_tmpD2F)))),((_tmpD2D->tl=0,_tmpD2D)))))));}break;}case
230: _LL313: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyyyvsp[0]),& Cyc_Tcutil_trk,1);{struct Cyc_Absyn_AccessEff_struct*_tmpD35;struct
Cyc_Absyn_AccessEff_struct _tmpD34;struct Cyc_List_List*_tmpD33;yyval=Cyc_YY40(((
_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33->hd=(void*)((_tmpD35=_cycalloc(
sizeof(*_tmpD35)),((_tmpD35[0]=((_tmpD34.tag=23,((_tmpD34.f1=(void*)Cyc_yyget_YY44(
yyyyvsp[0]),_tmpD34)))),_tmpD35)))),((_tmpD33->tl=Cyc_yyget_YY40(yyyyvsp[2]),
_tmpD33)))))));}break;}case 231: _LL314: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_List_List*_tmpD36;yyval=
Cyc_YY38(((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36->hd=Cyc_yyget_YY37(
yyyyvsp[0]),((_tmpD36->tl=0,_tmpD36)))))));}break;}case 232: _LL315: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct
Cyc_List_List*_tmpD37;yyval=Cyc_YY38(((_tmpD37=_cycalloc(sizeof(*_tmpD37)),((
_tmpD37->hd=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpD37->tl=Cyc_yyget_YY38(yyyyvsp[0]),
_tmpD37)))))));}break;}case 233: _LL316: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct _tuple23 _tmp6C2;struct Cyc_Absyn_Tqual
_tmp6C3;struct Cyc_List_List*_tmp6C4;struct Cyc_List_List*_tmp6C5;struct _tuple23*
_tmp6C1=Cyc_yyget_YY35(yyyyvsp[0]);_tmp6C2=*_tmp6C1;_tmp6C3=_tmp6C2.f1;_tmp6C4=
_tmp6C2.f2;_tmp6C5=_tmp6C2.f3;if(_tmp6C3.loc == 0)_tmp6C3.loc=(unsigned int)(
yyyylsp[0]).first_line;{struct Cyc_Parse_Declarator _tmp6C7;struct _tuple1*_tmp6C8;
struct Cyc_List_List*_tmp6C9;struct Cyc_Parse_Declarator*_tmp6C6=Cyc_yyget_YY29(
yyyyvsp[1]);_tmp6C7=*_tmp6C6;_tmp6C8=_tmp6C7.id;_tmp6C9=_tmp6C7.tms;{void*
_tmp6CA=Cyc_Parse_speclist2typ(_tmp6C4,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_Absyn_Tqual _tmp6CC;void*_tmp6CD;struct Cyc_List_List*_tmp6CE;struct Cyc_List_List*
_tmp6CF;struct _tuple11 _tmp6CB=Cyc_Parse_apply_tms(_tmp6C3,_tmp6CA,_tmp6C5,
_tmp6C9);_tmp6CC=_tmp6CB.f1;_tmp6CD=_tmp6CB.f2;_tmp6CE=_tmp6CB.f3;_tmp6CF=
_tmp6CB.f4;if(_tmp6CE != 0){const char*_tmpD38;Cyc_Parse_err(((_tmpD38="parameter with bad type params",
_tag_dyneither(_tmpD38,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp6C8)){const char*_tmpD39;Cyc_Parse_err(((
_tmpD39="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpD39,
sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{struct Cyc_Core_Opt*
_tmpD3A;struct Cyc_Core_Opt*_tmp6D2=(struct Cyc_Core_Opt*)((_tmpD3A=_cycalloc(
sizeof(*_tmpD3A)),((_tmpD3A->v=(*_tmp6C8).f2,_tmpD3A))));if(_tmp6CF != 0){const
char*_tmpD3D;void*_tmpD3C;(_tmpD3C=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((
_tmpD3D="extra attributes on parameter, ignoring",_tag_dyneither(_tmpD3D,sizeof(
char),40))),_tag_dyneither(_tmpD3C,sizeof(void*),0)));}{struct _tuple9*_tmpD3E;
yyval=Cyc_YY37(((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->f1=_tmp6D2,((
_tmpD3E->f2=_tmp6CC,((_tmpD3E->f3=_tmp6CD,_tmpD3E)))))))));}break;};};};}case 234:
_LL317: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);struct _tuple23 _tmp6D8;struct Cyc_Absyn_Tqual
_tmp6D9;struct Cyc_List_List*_tmp6DA;struct Cyc_List_List*_tmp6DB;struct _tuple23*
_tmp6D7=Cyc_yyget_YY35(yyyyvsp[0]);_tmp6D8=*_tmp6D7;_tmp6D9=_tmp6D8.f1;_tmp6DA=
_tmp6D8.f2;_tmp6DB=_tmp6D8.f3;if(_tmp6D9.loc == 0)_tmp6D9.loc=(unsigned int)(
yyyylsp[0]).first_line;{void*_tmp6DC=Cyc_Parse_speclist2typ(_tmp6DA,(
unsigned int)(yyyylsp[0]).first_line);if(_tmp6DB != 0){const char*_tmpD41;void*
_tmpD40;(_tmpD40=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((
_tmpD41="bad attributes on parameter, ignoring",_tag_dyneither(_tmpD41,sizeof(
char),38))),_tag_dyneither(_tmpD40,sizeof(void*),0)));}{struct _tuple9*_tmpD42;
yyval=Cyc_YY37(((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->f1=0,((_tmpD42->f2=
_tmp6D9,((_tmpD42->f3=_tmp6DC,_tmpD42)))))))));}break;};}case 235: _LL318: {union
Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(
struct Cyc_Yyltype),2);struct _tuple23 _tmp6E1;struct Cyc_Absyn_Tqual _tmp6E2;struct
Cyc_List_List*_tmp6E3;struct Cyc_List_List*_tmp6E4;struct _tuple23*_tmp6E0=Cyc_yyget_YY35(
yyyyvsp[0]);_tmp6E1=*_tmp6E0;_tmp6E2=_tmp6E1.f1;_tmp6E3=_tmp6E1.f2;_tmp6E4=
_tmp6E1.f3;if(_tmp6E2.loc == 0)_tmp6E2.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6E5=Cyc_Parse_speclist2typ(_tmp6E3,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6E6=(Cyc_yyget_YY30(yyyyvsp[1]))->tms;struct Cyc_Absyn_Tqual
_tmp6E8;void*_tmp6E9;struct Cyc_List_List*_tmp6EA;struct Cyc_List_List*_tmp6EB;
struct _tuple11 _tmp6E7=Cyc_Parse_apply_tms(_tmp6E2,_tmp6E5,_tmp6E4,_tmp6E6);
_tmp6E8=_tmp6E7.f1;_tmp6E9=_tmp6E7.f2;_tmp6EA=_tmp6E7.f3;_tmp6EB=_tmp6E7.f4;if(
_tmp6EA != 0){const char*_tmpD45;void*_tmpD44;(_tmpD44=0,Cyc_Tcutil_warn((
unsigned int)(yyyylsp[0]).first_line,((_tmpD45="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpD45,sizeof(char),49))),_tag_dyneither(_tmpD44,sizeof(void*),0)));}
if(_tmp6EB != 0){const char*_tmpD48;void*_tmpD47;(_tmpD47=0,Cyc_Tcutil_warn((
unsigned int)(yyyylsp[0]).first_line,((_tmpD48="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpD48,sizeof(char),38))),_tag_dyneither(_tmpD47,sizeof(void*),0)));}{
struct _tuple9*_tmpD49;yyval=Cyc_YY37(((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((
_tmpD49->f1=0,((_tmpD49->f2=_tmp6E8,((_tmpD49->f3=_tmp6E9,_tmpD49)))))))));}
break;};}case 236: _LL319: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY36(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));break;}
case 237: _LL31A: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _dyneither_ptr*_tmpD4C;
struct Cyc_List_List*_tmpD4B;yyval=Cyc_YY36(((_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((
_tmpD4B->hd=((_tmpD4C=_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C[0]=Cyc_yyget_String_tok(
yyyyvsp[0]),_tmpD4C)))),((_tmpD4B->tl=0,_tmpD4B)))))));}break;}case 238: _LL31B: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct _dyneither_ptr*_tmpD4F;
struct Cyc_List_List*_tmpD4E;yyval=Cyc_YY36(((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((
_tmpD4E->hd=((_tmpD4F=_cycalloc(sizeof(*_tmpD4F)),((_tmpD4F[0]=Cyc_yyget_String_tok(
yyyyvsp[2]),_tmpD4F)))),((_tmpD4E->tl=Cyc_yyget_YY36(yyyyvsp[0]),_tmpD4E)))))));}
break;}case 239: _LL31C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 240:
_LL31D: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 241:
_LL31E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD52;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD51;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD51=_cycalloc(sizeof(*_tmpD51)),((_tmpD51[0]=((_tmpD52.tag=36,((
_tmpD52.f1=0,((_tmpD52.f2=0,_tmpD52)))))),_tmpD51)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 242: _LL31F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD55;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD54;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD54=_cycalloc(sizeof(*_tmpD54)),((_tmpD54[0]=((_tmpD55.tag=36,((
_tmpD55.f1=0,((_tmpD55.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyyyvsp[1])),_tmpD55)))))),_tmpD54)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 243: _LL320: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD58;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD57;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD57=_cycalloc(sizeof(*_tmpD57)),((_tmpD57[0]=((_tmpD58.tag=36,((
_tmpD58.f1=0,((_tmpD58.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyyyvsp[1])),_tmpD58)))))),_tmpD57)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 244: _LL321: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);struct _dyneither_ptr*_tmpD5B;
struct _tuple1*_tmpD5A;struct Cyc_Absyn_Vardecl*_tmp6FB=Cyc_Absyn_new_vardecl(((
_tmpD5A=_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A->f1=Cyc_Absyn_Loc_n,((_tmpD5A->f2=((
_tmpD5B=_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B[0]=Cyc_yyget_String_tok(yyyyvsp[2]),
_tmpD5B)))),_tmpD5A)))))),Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
0,(unsigned int)(yyyylsp[2]).first_line));(_tmp6FB->tq).real_const=1;{struct Cyc_Absyn_Comprehension_e_struct
_tmpD5E;struct Cyc_Absyn_Comprehension_e_struct*_tmpD5D;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD5D=_cycalloc(sizeof(*_tmpD5D)),((_tmpD5D[0]=((_tmpD5E.tag=28,((
_tmpD5E.f1=_tmp6FB,((_tmpD5E.f2=Cyc_yyget_YY3(yyyyvsp[4]),((_tmpD5E.f3=Cyc_yyget_YY3(
yyyyvsp[6]),((_tmpD5E.f4=0,_tmpD5E)))))))))),_tmpD5D)))),(unsigned int)(yyyylsp[
0]).first_line));}break;}case 245: _LL322: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _tuple28*_tmpD61;struct Cyc_List_List*
_tmpD60;yyval=Cyc_YY6(((_tmpD60=_cycalloc(sizeof(*_tmpD60)),((_tmpD60->hd=((
_tmpD61=_cycalloc(sizeof(*_tmpD61)),((_tmpD61->f1=0,((_tmpD61->f2=Cyc_yyget_YY3(
yyyyvsp[0]),_tmpD61)))))),((_tmpD60->tl=0,_tmpD60)))))));}break;}case 246: _LL323: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct _tuple28*_tmpD64;struct Cyc_List_List*
_tmpD63;yyval=Cyc_YY6(((_tmpD63=_cycalloc(sizeof(*_tmpD63)),((_tmpD63->hd=((
_tmpD64=_cycalloc(sizeof(*_tmpD64)),((_tmpD64->f1=Cyc_yyget_YY41(yyyyvsp[0]),((
_tmpD64->f2=Cyc_yyget_YY3(yyyyvsp[1]),_tmpD64)))))),((_tmpD63->tl=0,_tmpD63)))))));}
break;}case 247: _LL324: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct _tuple28*_tmpD67;struct Cyc_List_List*
_tmpD66;yyval=Cyc_YY6(((_tmpD66=_cycalloc(sizeof(*_tmpD66)),((_tmpD66->hd=((
_tmpD67=_cycalloc(sizeof(*_tmpD67)),((_tmpD67->f1=0,((_tmpD67->f2=Cyc_yyget_YY3(
yyyyvsp[2]),_tmpD67)))))),((_tmpD66->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpD66)))))));}
break;}case 248: _LL325: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct _tuple28*_tmpD6A;struct Cyc_List_List*
_tmpD69;yyval=Cyc_YY6(((_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69->hd=((
_tmpD6A=_cycalloc(sizeof(*_tmpD6A)),((_tmpD6A->f1=Cyc_yyget_YY41(yyyyvsp[2]),((
_tmpD6A->f2=Cyc_yyget_YY3(yyyyvsp[3]),_tmpD6A)))))),((_tmpD69->tl=Cyc_yyget_YY6(
yyyyvsp[0]),_tmpD69)))))));}break;}case 249: _LL326: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY41(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));break;}
case 250: _LL327: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_List_List*_tmpD6B;yyval=
Cyc_YY41(((_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B->hd=Cyc_yyget_YY42(
yyyyvsp[0]),((_tmpD6B->tl=0,_tmpD6B)))))));}break;}case 251: _LL328: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct
Cyc_List_List*_tmpD6C;yyval=Cyc_YY41(((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((
_tmpD6C->hd=Cyc_yyget_YY42(yyyyvsp[1]),((_tmpD6C->tl=Cyc_yyget_YY41(yyyyvsp[0]),
_tmpD6C)))))));}break;}case 252: _LL329: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_Absyn_ArrayElement_struct
_tmpD6F;struct Cyc_Absyn_ArrayElement_struct*_tmpD6E;yyval=Cyc_YY42((void*)((
_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E[0]=((_tmpD6F.tag=0,((_tmpD6F.f1=
Cyc_yyget_YY3(yyyyvsp[1]),_tmpD6F)))),_tmpD6E)))));}break;}case 253: _LL32A: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_Absyn_FieldName_struct
_tmpD75;struct _dyneither_ptr*_tmpD74;struct Cyc_Absyn_FieldName_struct*_tmpD73;
yyval=Cyc_YY42((void*)((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73[0]=((
_tmpD75.tag=1,((_tmpD75.f1=((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74[0]=
Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD74)))),_tmpD75)))),_tmpD73)))));}break;}
case 254: _LL32B: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);struct _tuple23 _tmp710;struct Cyc_Absyn_Tqual
_tmp711;struct Cyc_List_List*_tmp712;struct Cyc_List_List*_tmp713;struct _tuple23*
_tmp70F=Cyc_yyget_YY35(yyyyvsp[0]);_tmp710=*_tmp70F;_tmp711=_tmp710.f1;_tmp712=
_tmp710.f2;_tmp713=_tmp710.f3;{void*_tmp714=Cyc_Parse_speclist2typ(_tmp712,(
unsigned int)(yyyylsp[0]).first_line);if(_tmp713 != 0){const char*_tmpD78;void*
_tmpD77;(_tmpD77=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((
_tmpD78="ignoring attributes in type",_tag_dyneither(_tmpD78,sizeof(char),28))),
_tag_dyneither(_tmpD77,sizeof(void*),0)));}{struct _tuple9*_tmpD79;yyval=Cyc_YY37(((
_tmpD79=_cycalloc(sizeof(*_tmpD79)),((_tmpD79->f1=0,((_tmpD79->f2=_tmp711,((
_tmpD79->f3=_tmp714,_tmpD79)))))))));}break;};}case 255: _LL32C: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);struct
_tuple23 _tmp719;struct Cyc_Absyn_Tqual _tmp71A;struct Cyc_List_List*_tmp71B;struct
Cyc_List_List*_tmp71C;struct _tuple23*_tmp718=Cyc_yyget_YY35(yyyyvsp[0]);_tmp719=*
_tmp718;_tmp71A=_tmp719.f1;_tmp71B=_tmp719.f2;_tmp71C=_tmp719.f3;{void*_tmp71D=
Cyc_Parse_speclist2typ(_tmp71B,(unsigned int)(yyyylsp[0]).first_line);struct Cyc_List_List*
_tmp71E=(Cyc_yyget_YY30(yyyyvsp[1]))->tms;struct _tuple11 _tmp71F=Cyc_Parse_apply_tms(
_tmp71A,_tmp71D,_tmp71C,_tmp71E);if(_tmp71F.f3 != 0){const char*_tmpD7C;void*
_tmpD7B;(_tmpD7B=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((
_tmpD7C="bad type params, ignoring",_tag_dyneither(_tmpD7C,sizeof(char),26))),
_tag_dyneither(_tmpD7B,sizeof(void*),0)));}if(_tmp71F.f4 != 0){const char*_tmpD7F;
void*_tmpD7E;(_tmpD7E=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((
_tmpD7F="bad specifiers, ignoring",_tag_dyneither(_tmpD7F,sizeof(char),25))),
_tag_dyneither(_tmpD7E,sizeof(void*),0)));}{struct _tuple9*_tmpD80;yyval=Cyc_YY37(((
_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80->f1=0,((_tmpD80->f2=_tmp71F.f1,((
_tmpD80->f3=_tmp71F.f2,_tmpD80)))))))));}break;};}case 256: _LL32D: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(
yyyylsp[0]).first_line));break;}case 257: _LL32E: {union Cyc_YYSTYPE*yyyyvsp=(union
Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_Absyn_JoinEff_struct
_tmpD83;struct Cyc_Absyn_JoinEff_struct*_tmpD82;yyval=Cyc_YY44((void*)((_tmpD82=
_cycalloc(sizeof(*_tmpD82)),((_tmpD82[0]=((_tmpD83.tag=24,((_tmpD83.f1=0,_tmpD83)))),
_tmpD82)))));}break;}case 258: _LL32F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_Absyn_JoinEff_struct
_tmpD86;struct Cyc_Absyn_JoinEff_struct*_tmpD85;yyval=Cyc_YY44((void*)((_tmpD85=
_cycalloc(sizeof(*_tmpD85)),((_tmpD85[0]=((_tmpD86.tag=24,((_tmpD86.f1=Cyc_yyget_YY40(
yyyyvsp[1]),_tmpD86)))),_tmpD85)))));}break;}case 259: _LL330: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct
Cyc_Absyn_RgnsEff_struct _tmpD89;struct Cyc_Absyn_RgnsEff_struct*_tmpD88;yyval=Cyc_YY44((
void*)((_tmpD88=_cycalloc(sizeof(*_tmpD88)),((_tmpD88[0]=((_tmpD89.tag=25,((
_tmpD89.f1=(void*)Cyc_yyget_YY44(yyyyvsp[2]),_tmpD89)))),_tmpD88)))));}break;}
case 260: _LL331: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_Absyn_JoinEff_struct
_tmpD8F;struct Cyc_List_List*_tmpD8E;struct Cyc_Absyn_JoinEff_struct*_tmpD8D;yyval=
Cyc_YY44((void*)((_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D[0]=((_tmpD8F.tag=
24,((_tmpD8F.f1=((_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E->hd=Cyc_yyget_YY44(
yyyyvsp[0]),((_tmpD8E->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpD8E)))))),_tmpD8F)))),
_tmpD8D)))));}break;}case 261: _LL332: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_List_List*_tmpD90;yyval=
Cyc_YY40(((_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90->hd=Cyc_yyget_YY44(
yyyyvsp[0]),((_tmpD90->tl=0,_tmpD90)))))));}break;}case 262: _LL333: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct
Cyc_List_List*_tmpD91;yyval=Cyc_YY40(((_tmpD91=_cycalloc(sizeof(*_tmpD91)),((
_tmpD91->hd=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpD91->tl=Cyc_yyget_YY40(yyyyvsp[0]),
_tmpD91)))))));}break;}case 263: _LL334: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Parse_Abstractdeclarator*
_tmpD92;yyval=Cyc_YY30(((_tmpD92=_cycalloc(sizeof(*_tmpD92)),((_tmpD92->tms=Cyc_yyget_YY28(
yyyyvsp[0]),_tmpD92)))));}break;}case 264: _LL335: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 265:
_LL336: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_Parse_Abstractdeclarator*
_tmpD93;yyval=Cyc_YY30(((_tmpD93=_cycalloc(sizeof(*_tmpD93)),((_tmpD93->tms=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY28(yyyyvsp[0]),(Cyc_yyget_YY30(yyyyvsp[1]))->tms),_tmpD93)))));}
break;}case 266: _LL337: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=yyyyvsp[1];break;}case 267:
_LL338: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_List_List*_tmpD9D;
struct Cyc_Absyn_Carray_mod_struct _tmpD9C;struct Cyc_Absyn_Carray_mod_struct*
_tmpD9B;struct Cyc_Parse_Abstractdeclarator*_tmpD9A;yyval=Cyc_YY30(((_tmpD9A=
_cycalloc(sizeof(*_tmpD9A)),((_tmpD9A->tms=((_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((
_tmpD9D->hd=(void*)((_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B[0]=((_tmpD9C.tag=
0,((_tmpD9C.f1=Cyc_yyget_YY51(yyyyvsp[2]),((_tmpD9C.f2=(unsigned int)(yyyylsp[2]).first_line,
_tmpD9C)))))),_tmpD9B)))),((_tmpD9D->tl=0,_tmpD9D)))))),_tmpD9A)))));}break;}
case 268: _LL339: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_List_List*_tmpDA7;
struct Cyc_Absyn_Carray_mod_struct _tmpDA6;struct Cyc_Absyn_Carray_mod_struct*
_tmpDA5;struct Cyc_Parse_Abstractdeclarator*_tmpDA4;yyval=Cyc_YY30(((_tmpDA4=
_cycalloc(sizeof(*_tmpDA4)),((_tmpDA4->tms=((_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((
_tmpDA7->hd=(void*)((_tmpDA5=_cycalloc(sizeof(*_tmpDA5)),((_tmpDA5[0]=((_tmpDA6.tag=
0,((_tmpDA6.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpDA6.f2=(unsigned int)(yyyylsp[3]).first_line,
_tmpDA6)))))),_tmpDA5)))),((_tmpDA7->tl=(Cyc_yyget_YY30(yyyyvsp[0]))->tms,
_tmpDA7)))))),_tmpDA4)))));}break;}case 269: _LL33A: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_List_List*_tmpDB1;
struct Cyc_Absyn_ConstArray_mod_struct _tmpDB0;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpDAF;struct Cyc_Parse_Abstractdeclarator*_tmpDAE;yyval=Cyc_YY30(((_tmpDAE=
_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->tms=((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((
_tmpDB1->hd=(void*)((_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF[0]=((_tmpDB0.tag=
1,((_tmpDB0.f1=Cyc_yyget_YY3(yyyyvsp[1]),((_tmpDB0.f2=Cyc_yyget_YY51(yyyyvsp[3]),((
_tmpDB0.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpDB0)))))))),_tmpDAF)))),((
_tmpDB1->tl=0,_tmpDB1)))))),_tmpDAE)))));}break;}case 270: _LL33B: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);{struct
Cyc_List_List*_tmpDBB;struct Cyc_Absyn_ConstArray_mod_struct _tmpDBA;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpDB9;struct Cyc_Parse_Abstractdeclarator*_tmpDB8;yyval=Cyc_YY30(((_tmpDB8=
_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8->tms=((_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((
_tmpDBB->hd=(void*)((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9[0]=((_tmpDBA.tag=
1,((_tmpDBA.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpDBA.f2=Cyc_yyget_YY51(yyyyvsp[4]),((
_tmpDBA.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpDBA)))))))),_tmpDB9)))),((
_tmpDBB->tl=(Cyc_yyget_YY30(yyyyvsp[0]))->tms,_tmpDBB)))))),_tmpDB8)))));}break;}
case 271: _LL33C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_List_List*_tmpDD0;
struct Cyc_Absyn_Function_mod_struct _tmpDCF;struct Cyc_Absyn_WithTypes_struct
_tmpDCE;struct Cyc_Absyn_WithTypes_struct*_tmpDCD;struct Cyc_Absyn_Function_mod_struct*
_tmpDCC;struct Cyc_Parse_Abstractdeclarator*_tmpDCB;yyval=Cyc_YY30(((_tmpDCB=
_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB->tms=((_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((
_tmpDD0->hd=(void*)((_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC[0]=((_tmpDCF.tag=
3,((_tmpDCF.f1=(void*)((void*)((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD[0]=((
_tmpDCE.tag=1,((_tmpDCE.f1=0,((_tmpDCE.f2=0,((_tmpDCE.f3=0,((_tmpDCE.f4=Cyc_yyget_YY49(
yyyyvsp[1]),((_tmpDCE.f5=Cyc_yyget_YY50(yyyyvsp[2]),_tmpDCE)))))))))))),_tmpDCD))))),
_tmpDCF)))),_tmpDCC)))),((_tmpDD0->tl=0,_tmpDD0)))))),_tmpDCB)))));}break;}case
272: _LL33D: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);struct _tuple24 _tmp749;struct Cyc_List_List*
_tmp74A;int _tmp74B;struct Cyc_Absyn_VarargInfo*_tmp74C;struct Cyc_Core_Opt*_tmp74D;
struct Cyc_List_List*_tmp74E;struct _tuple24*_tmp748=Cyc_yyget_YY39(yyyyvsp[1]);
_tmp749=*_tmp748;_tmp74A=_tmp749.f1;_tmp74B=_tmp749.f2;_tmp74C=_tmp749.f3;
_tmp74D=_tmp749.f4;_tmp74E=_tmp749.f5;{struct Cyc_List_List*_tmpDE5;struct Cyc_Absyn_Function_mod_struct
_tmpDE4;struct Cyc_Absyn_WithTypes_struct _tmpDE3;struct Cyc_Absyn_WithTypes_struct*
_tmpDE2;struct Cyc_Absyn_Function_mod_struct*_tmpDE1;struct Cyc_Parse_Abstractdeclarator*
_tmpDE0;yyval=Cyc_YY30(((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0->tms=((
_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->hd=(void*)((_tmpDE1=_cycalloc(
sizeof(*_tmpDE1)),((_tmpDE1[0]=((_tmpDE4.tag=3,((_tmpDE4.f1=(void*)((void*)((
_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE3.tag=1,((_tmpDE3.f1=
_tmp74A,((_tmpDE3.f2=_tmp74B,((_tmpDE3.f3=_tmp74C,((_tmpDE3.f4=_tmp74D,((_tmpDE3.f5=
_tmp74E,_tmpDE3)))))))))))),_tmpDE2))))),_tmpDE4)))),_tmpDE1)))),((_tmpDE5->tl=0,
_tmpDE5)))))),_tmpDE0)))));}break;}case 273: _LL33E: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);{struct Cyc_List_List*_tmpDFA;
struct Cyc_Absyn_Function_mod_struct _tmpDF9;struct Cyc_Absyn_WithTypes_struct
_tmpDF8;struct Cyc_Absyn_WithTypes_struct*_tmpDF7;struct Cyc_Absyn_Function_mod_struct*
_tmpDF6;struct Cyc_Parse_Abstractdeclarator*_tmpDF5;yyval=Cyc_YY30(((_tmpDF5=
_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5->tms=((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((
_tmpDFA->hd=(void*)((_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6[0]=((_tmpDF9.tag=
3,((_tmpDF9.f1=(void*)((void*)((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7[0]=((
_tmpDF8.tag=1,((_tmpDF8.f1=0,((_tmpDF8.f2=0,((_tmpDF8.f3=0,((_tmpDF8.f4=Cyc_yyget_YY49(
yyyyvsp[2]),((_tmpDF8.f5=Cyc_yyget_YY50(yyyyvsp[3]),_tmpDF8)))))))))))),_tmpDF7))))),
_tmpDF9)))),_tmpDF6)))),((_tmpDFA->tl=(Cyc_yyget_YY30(yyyyvsp[0]))->tms,_tmpDFA)))))),
_tmpDF5)))));}break;}case 274: _LL33F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct _tuple24 _tmp75C;struct Cyc_List_List*
_tmp75D;int _tmp75E;struct Cyc_Absyn_VarargInfo*_tmp75F;struct Cyc_Core_Opt*_tmp760;
struct Cyc_List_List*_tmp761;struct _tuple24*_tmp75B=Cyc_yyget_YY39(yyyyvsp[2]);
_tmp75C=*_tmp75B;_tmp75D=_tmp75C.f1;_tmp75E=_tmp75C.f2;_tmp75F=_tmp75C.f3;
_tmp760=_tmp75C.f4;_tmp761=_tmp75C.f5;{struct Cyc_List_List*_tmpE0F;struct Cyc_Absyn_Function_mod_struct
_tmpE0E;struct Cyc_Absyn_WithTypes_struct _tmpE0D;struct Cyc_Absyn_WithTypes_struct*
_tmpE0C;struct Cyc_Absyn_Function_mod_struct*_tmpE0B;struct Cyc_Parse_Abstractdeclarator*
_tmpE0A;yyval=Cyc_YY30(((_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A->tms=((
_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->hd=(void*)((_tmpE0B=_cycalloc(
sizeof(*_tmpE0B)),((_tmpE0B[0]=((_tmpE0E.tag=3,((_tmpE0E.f1=(void*)((void*)((
_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C[0]=((_tmpE0D.tag=1,((_tmpE0D.f1=
_tmp75D,((_tmpE0D.f2=_tmp75E,((_tmpE0D.f3=_tmp75F,((_tmpE0D.f4=_tmp760,((_tmpE0D.f5=
_tmp761,_tmpE0D)))))))))))),_tmpE0C))))),_tmpE0E)))),_tmpE0B)))),((_tmpE0F->tl=(
Cyc_yyget_YY30(yyyyvsp[0]))->tms,_tmpE0F)))))),_tmpE0A)))));}break;}case 275:
_LL340: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct Cyc_List_List*_tmp768=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),
unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(
unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));{struct Cyc_List_List*_tmpE19;
struct Cyc_Absyn_TypeParams_mod_struct _tmpE18;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpE17;struct Cyc_Parse_Abstractdeclarator*_tmpE16;yyval=Cyc_YY30(((_tmpE16=
_cycalloc(sizeof(*_tmpE16)),((_tmpE16->tms=((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((
_tmpE19->hd=(void*)((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17[0]=((_tmpE18.tag=
4,((_tmpE18.f1=_tmp768,((_tmpE18.f2=(unsigned int)(yyyylsp[1]).first_line,((
_tmpE18.f3=0,_tmpE18)))))))),_tmpE17)))),((_tmpE19->tl=(Cyc_yyget_YY30(yyyyvsp[0]))->tms,
_tmpE19)))))),_tmpE16)))));}break;}case 276: _LL341: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_List_List*_tmpE23;
struct Cyc_Absyn_Attributes_mod_struct _tmpE22;struct Cyc_Absyn_Attributes_mod_struct*
_tmpE21;struct Cyc_Parse_Abstractdeclarator*_tmpE20;yyval=Cyc_YY30(((_tmpE20=
_cycalloc(sizeof(*_tmpE20)),((_tmpE20->tms=((_tmpE23=_cycalloc(sizeof(*_tmpE23)),((
_tmpE23->hd=(void*)((_tmpE21=_cycalloc(sizeof(*_tmpE21)),((_tmpE21[0]=((_tmpE22.tag=
5,((_tmpE22.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpE22.f2=Cyc_yyget_YY45(
yyyyvsp[1]),_tmpE22)))))),_tmpE21)))),((_tmpE23->tl=(Cyc_yyget_YY30(yyyyvsp[0]))->tms,
_tmpE23)))))),_tmpE20)))));}break;}case 277: _LL342: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 278:
_LL343: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 279:
_LL344: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 280:
_LL345: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 281:
_LL346: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 282:
_LL347: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 283:
_LL348: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union
Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);{struct Cyc_Absyn_ResetRegion_s_struct
_tmpE26;struct Cyc_Absyn_ResetRegion_s_struct*_tmpE25;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((
void*)((_tmpE25=_cycalloc(sizeof(*_tmpE25)),((_tmpE25[0]=((_tmpE26.tag=16,((
_tmpE26.f1=Cyc_yyget_YY3(yyyyvsp[2]),_tmpE26)))),_tmpE25)))),(unsigned int)(
yyyylsp[0]).first_line));}break;}case 284: _LL349: yyval=Cyc_YY4(0);break;case 285:
_LL34A: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union
Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);{const char*_tmpE27;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpE27="open",
_tag_dyneither(_tmpE27,sizeof(char),5))))!= 0){const char*_tmpE28;Cyc_Parse_err(((
_tmpE28="expecting `open'",_tag_dyneither(_tmpE28,sizeof(char),17))),(
unsigned int)(yyyylsp[1]).first_line);}}yyval=Cyc_YY4((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyyyvsp[3]));break;}case 286: _LL34B: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct Cyc_Absyn_Label_s_struct
_tmpE2E;struct _dyneither_ptr*_tmpE2D;struct Cyc_Absyn_Label_s_struct*_tmpE2C;
yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)((_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((
_tmpE2C[0]=((_tmpE2E.tag=13,((_tmpE2E.f1=((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((
_tmpE2D[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE2D)))),((_tmpE2E.f2=Cyc_yyget_YY9(
yyyyvsp[2]),_tmpE2E)))))),_tmpE2C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 287: _LL34C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY9(Cyc_Absyn_skip_stmt((
unsigned int)(yyyylsp[0]).first_line));break;}case 288: _LL34D: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=
Cyc_YY9(Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 289: _LL34E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY9(Cyc_Absyn_skip_stmt((
unsigned int)(yyyylsp[0]).first_line));break;}case 290: _LL34F: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=
yyyyvsp[1];break;}case 291: _LL350: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(
Cyc_yyget_YY18(yyyyvsp[0]),Cyc_Absyn_skip_stmt(0)));break;}case 292: _LL351: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(
Cyc_yyget_YY18(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1])));break;}case 293: _LL352: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 294:
_LL353: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(
Cyc_yyget_YY9(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 295: _LL354: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Absyn_Fn_d_struct
_tmpE31;struct Cyc_Absyn_Fn_d_struct*_tmpE30;yyval=Cyc_YY9(Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl((void*)((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30[0]=((
_tmpE31.tag=1,((_tmpE31.f1=Cyc_yyget_YY17(yyyyvsp[0]),_tmpE31)))),_tmpE30)))),(
unsigned int)(yyyylsp[0]).first_line),Cyc_Absyn_skip_stmt(0)));}break;}case 296:
_LL355: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_Absyn_Fn_d_struct
_tmpE34;struct Cyc_Absyn_Fn_d_struct*_tmpE33;yyval=Cyc_YY9(Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl((void*)((_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33[0]=((
_tmpE34.tag=1,((_tmpE34.f1=Cyc_yyget_YY17(yyyyvsp[0]),_tmpE34)))),_tmpE33)))),(
unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY9(yyyyvsp[1])));}break;}case
297: _LL356: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union
Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(
unsigned int)(yyyylsp[0]).first_line));break;}case 298: _LL357: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);yyval=
Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY9(
yyyyvsp[4]),Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 299: _LL358: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(
Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 300: _LL359: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);struct Cyc_Absyn_UnknownId_e_struct
_tmpE37;struct Cyc_Absyn_UnknownId_e_struct*_tmpE36;struct Cyc_Absyn_Exp*_tmp77C=
Cyc_Absyn_new_exp((void*)((_tmpE36=_cycalloc(sizeof(*_tmpE36)),((_tmpE36[0]=((
_tmpE37.tag=2,((_tmpE37.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmpE37)))),_tmpE36)))),(
unsigned int)(yyyylsp[1]).first_line);yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(
_tmp77C,Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 301: _LL35A: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);struct Cyc_Absyn_Exp*_tmp77F=Cyc_Absyn_tuple_exp(
Cyc_yyget_YY5(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);yyval=Cyc_YY9(
Cyc_Absyn_switch_stmt(_tmp77F,Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[
0]).first_line));break;}case 302: _LL35B: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(
Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY10(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 303: _LL35C: yyval=Cyc_YY10(0);break;case 304: _LL35D: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct
Cyc_Absyn_Switch_clause*_tmpE3A;struct Cyc_List_List*_tmpE39;yyval=Cyc_YY10(((
_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39->hd=((_tmpE3A=_cycalloc(sizeof(*
_tmpE3A)),((_tmpE3A->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(
unsigned int)(yyyylsp[0]).first_line),((_tmpE3A->pat_vars=0,((_tmpE3A->where_clause=
0,((_tmpE3A->body=Cyc_yyget_YY9(yyyyvsp[2]),((_tmpE3A->loc=(unsigned int)(
yyyylsp[0]).first_line,_tmpE3A)))))))))))),((_tmpE39->tl=0,_tmpE39)))))));}
break;}case 305: _LL35E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_Absyn_Switch_clause*
_tmpE3D;struct Cyc_List_List*_tmpE3C;yyval=Cyc_YY10(((_tmpE3C=_cycalloc(sizeof(*
_tmpE3C)),((_tmpE3C->hd=((_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D->pattern=
Cyc_yyget_YY11(yyyyvsp[1]),((_tmpE3D->pat_vars=0,((_tmpE3D->where_clause=0,((
_tmpE3D->body=Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((
_tmpE3D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE3D)))))))))))),((_tmpE3C->tl=
Cyc_yyget_YY10(yyyyvsp[3]),_tmpE3C)))))));}break;}case 306: _LL35F: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);{struct
Cyc_Absyn_Switch_clause*_tmpE40;struct Cyc_List_List*_tmpE3F;yyval=Cyc_YY10(((
_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F->hd=((_tmpE40=_cycalloc(sizeof(*
_tmpE40)),((_tmpE40->pattern=Cyc_yyget_YY11(yyyyvsp[1]),((_tmpE40->pat_vars=0,((
_tmpE40->where_clause=0,((_tmpE40->body=Cyc_yyget_YY9(yyyyvsp[3]),((_tmpE40->loc=(
unsigned int)(yyyylsp[0]).first_line,_tmpE40)))))))))))),((_tmpE3F->tl=Cyc_yyget_YY10(
yyyyvsp[4]),_tmpE3F)))))));}break;}case 307: _LL360: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);{struct Cyc_Absyn_Switch_clause*
_tmpE43;struct Cyc_List_List*_tmpE42;yyval=Cyc_YY10(((_tmpE42=_cycalloc(sizeof(*
_tmpE42)),((_tmpE42->hd=((_tmpE43=_cycalloc(sizeof(*_tmpE43)),((_tmpE43->pattern=
Cyc_yyget_YY11(yyyyvsp[1]),((_tmpE43->pat_vars=0,((_tmpE43->where_clause=(struct
Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[3]),((_tmpE43->body=Cyc_Absyn_fallthru_stmt(
0,(unsigned int)(yyyylsp[4]).first_line),((_tmpE43->loc=(unsigned int)(yyyylsp[0]).first_line,
_tmpE43)))))))))))),((_tmpE42->tl=Cyc_yyget_YY10(yyyyvsp[5]),_tmpE42)))))));}
break;}case 308: _LL361: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);{struct Cyc_Absyn_Switch_clause*
_tmpE46;struct Cyc_List_List*_tmpE45;yyval=Cyc_YY10(((_tmpE45=_cycalloc(sizeof(*
_tmpE45)),((_tmpE45->hd=((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46->pattern=
Cyc_yyget_YY11(yyyyvsp[1]),((_tmpE46->pat_vars=0,((_tmpE46->where_clause=(struct
Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[3]),((_tmpE46->body=Cyc_yyget_YY9(yyyyvsp[5]),((
_tmpE46->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE46)))))))))))),((_tmpE45->tl=
Cyc_yyget_YY10(yyyyvsp[6]),_tmpE45)))))));}break;}case 309: _LL362: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);yyval=
Cyc_YY9(Cyc_Absyn_while_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[4]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 310: _LL363: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);yyval=
Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY3(yyyyvsp[4]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 311: _LL364: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);yyval=
Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));break;}case
312: _LL365: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union
Cyc_YYSTYPE),7);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyyyvsp[4]),Cyc_yyget_YY9(
yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));break;}case 313: _LL366: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union
Cyc_YYSTYPE),7);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(
yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));break;}case 314: _LL367: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union
Cyc_YYSTYPE),8);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_yyget_YY3(yyyyvsp[5]),Cyc_yyget_YY9(
yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));break;}case 315: _LL368: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union
Cyc_YYSTYPE),7);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_yyget_YY3(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(
yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));break;}case 316: _LL369: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union
Cyc_YYSTYPE),8);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_yyget_YY3(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyyyvsp[5]),Cyc_yyget_YY9(
yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));break;}case 317: _LL36A: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union
Cyc_YYSTYPE),8);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(
yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));break;}case 318: _LL36B: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union
Cyc_YYSTYPE),9);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),Cyc_yyget_YY3(yyyyvsp[6]),
Cyc_yyget_YY9(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));break;}case 319:
_LL36C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union
Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);struct Cyc_List_List*_tmp78A=Cyc_yyget_YY18(
yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp78B=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyyyvsp[5]),(
unsigned int)(yyyylsp[0]).first_line);yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(
_tmp78A,_tmp78B));break;}case 320: _LL36D: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);struct Cyc_List_List*_tmp78C=Cyc_yyget_YY18(
yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp78D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyyyvsp[6]),(
unsigned int)(yyyylsp[0]).first_line);yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(
_tmp78C,_tmp78D));break;}case 321: _LL36E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);struct Cyc_List_List*_tmp78E=Cyc_yyget_YY18(
yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp78F=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyyyvsp[4]),Cyc_yyget_YY9(yyyyvsp[6]),(
unsigned int)(yyyylsp[0]).first_line);yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(
_tmp78E,_tmp78F));break;}case 322: _LL36F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);struct Cyc_List_List*_tmp790=Cyc_yyget_YY18(
yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp791=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_yyget_YY3(yyyyvsp[5]),Cyc_yyget_YY9(yyyyvsp[7]),(
unsigned int)(yyyylsp[0]).first_line);yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(
_tmp790,_tmp791));break;}case 323: _LL370: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct _dyneither_ptr*_tmpE47;
yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((_tmpE47=_cycalloc(sizeof(*_tmpE47)),((
_tmpE47[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE47)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 324: _LL371: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY9(Cyc_Absyn_continue_stmt((
unsigned int)(yyyylsp[0]).first_line));break;}case 325: _LL372: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=
Cyc_YY9(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));break;}case
326: _LL373: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY9(Cyc_Absyn_return_stmt(
0,(unsigned int)(yyyylsp[0]).first_line));break;}case 327: _LL374: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=
Cyc_YY9(Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[1]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 328: _LL375: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=
Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));break;}
case 329: _LL376: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(
0,(unsigned int)(yyyylsp[0]).first_line));break;}case 330: _LL377: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);yyval=
Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(yyyyvsp[2]),(unsigned int)(yyyylsp[
0]).first_line));break;}case 331: _LL378: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 332:
_LL379: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 333:
_LL37A: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union
Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(
yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 334: _LL37B: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 335:
_LL37C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(
yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case 336: _LL37D: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 337:
_LL37E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(
yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case 338: _LL37F: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 339:
_LL380: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),
Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case
340: _LL381: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 341:
_LL382: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),
Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case
342: _LL383: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 343:
_LL384: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),
Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case
344: _LL385: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 345:
_LL386: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(
yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case 346: _LL387: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(
yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case 347: _LL388: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 348:
_LL389: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(
yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case 349: _LL38A: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(
yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case 350: _LL38B: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(
yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case 351: _LL38C: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(
yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case 352: _LL38D: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 353:
_LL38E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[
0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}
case 354: _LL38F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 355: _LL390: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 356:
_LL391: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),
Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case
357: _LL392: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),
Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case
358: _LL393: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 359:
_LL394: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),
Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case
360: _LL395: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),
Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case
361: _LL396: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),
Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));break;}case
362: _LL397: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 363:
_LL398: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);void*_tmp793=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);yyval=Cyc_YY11(
Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp793,Cyc_yyget_YY3(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(
unsigned int)(yyyylsp[0]).first_line)));break;}case 364: _LL399: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
yyyyvsp[0];break;}case 365: _LL39A: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_yyget_YY3(yyyyvsp[1])));break;}case 366: _LL39B: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[1]),(
unsigned int)(yyyylsp[0]).first_line)));break;}case 367: _LL39C: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);void*
_tmp794=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(
yyyylsp[2]).first_line);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(
_tmp794,(unsigned int)(yyyylsp[0]).first_line)));break;}case 368: _LL39D: {union
Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(
struct Cyc_Yyltype),2);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));break;}case
369: _LL39E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union
Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);{struct Cyc_Absyn_StructField_struct
_tmpE4D;struct _dyneither_ptr*_tmpE4C;struct Cyc_Absyn_StructField_struct*_tmpE4B;
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyyyvsp[
2])).f3,(void*)((_tmpE4B=_cycalloc(sizeof(*_tmpE4B)),((_tmpE4B[0]=((_tmpE4D.tag=
0,((_tmpE4D.f1=((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[0]=Cyc_yyget_String_tok(
yyyyvsp[4]),_tmpE4C)))),_tmpE4D)))),_tmpE4B)))),(unsigned int)(yyyylsp[0]).first_line)));}
break;}case 370: _LL39F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);void*_tmp798=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);{struct Cyc_Absyn_TupleIndex_struct
_tmpE50;struct Cyc_Absyn_TupleIndex_struct*_tmpE4F;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp(_tmp798,(void*)((_tmpE4F=_cycalloc_atomic(sizeof(*_tmpE4F)),((
_tmpE4F[0]=((_tmpE50.tag=1,((_tmpE50.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[
4])).f2,_tmpE50)))),_tmpE4F)))),(unsigned int)(yyyylsp[0]).first_line)));}break;}
case 371: _LL3A0: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 372:
_LL3A1: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 373:
_LL3A2: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));break;}case
374: _LL3A3: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_yyget_YY3(yyyyvsp[1])));break;}case 375: _LL3A4: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(
yyyyvsp[0]);{void*_tmp79B=e->r;union Cyc_Absyn_Cnst _tmp79D;struct _tuple4 _tmp79E;
enum Cyc_Absyn_Sign _tmp79F;char _tmp7A0;union Cyc_Absyn_Cnst _tmp7A2;struct _tuple5
_tmp7A3;enum Cyc_Absyn_Sign _tmp7A4;short _tmp7A5;union Cyc_Absyn_Cnst _tmp7A7;
struct _tuple6 _tmp7A8;enum Cyc_Absyn_Sign _tmp7A9;int _tmp7AA;union Cyc_Absyn_Cnst
_tmp7AC;struct _tuple8 _tmp7AD;struct _dyneither_ptr _tmp7AE;int _tmp7AF;union Cyc_Absyn_Cnst
_tmp7B1;int _tmp7B2;union Cyc_Absyn_Cnst _tmp7B4;struct _dyneither_ptr _tmp7B5;union
Cyc_Absyn_Cnst _tmp7B7;struct _dyneither_ptr _tmp7B8;union Cyc_Absyn_Cnst _tmp7BA;
struct _tuple7 _tmp7BB;_LL3A7: {struct Cyc_Absyn_Const_e_struct*_tmp79C=(struct Cyc_Absyn_Const_e_struct*)
_tmp79B;if(_tmp79C->tag != 0)goto _LL3A9;else{_tmp79D=_tmp79C->f1;if((_tmp79D.Char_c).tag
!= 2)goto _LL3A9;_tmp79E=(struct _tuple4)(_tmp79D.Char_c).val;_tmp79F=_tmp79E.f1;
_tmp7A0=_tmp79E.f2;}}_LL3A8:{struct Cyc_Absyn_Char_p_struct _tmpE53;struct Cyc_Absyn_Char_p_struct*
_tmpE52;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE52=_cycalloc_atomic(
sizeof(*_tmpE52)),((_tmpE52[0]=((_tmpE53.tag=10,((_tmpE53.f1=_tmp7A0,_tmpE53)))),
_tmpE52)))),e->loc));}goto _LL3A6;_LL3A9: {struct Cyc_Absyn_Const_e_struct*_tmp7A1=(
struct Cyc_Absyn_Const_e_struct*)_tmp79B;if(_tmp7A1->tag != 0)goto _LL3AB;else{
_tmp7A2=_tmp7A1->f1;if((_tmp7A2.Short_c).tag != 4)goto _LL3AB;_tmp7A3=(struct
_tuple5)(_tmp7A2.Short_c).val;_tmp7A4=_tmp7A3.f1;_tmp7A5=_tmp7A3.f2;}}_LL3AA:{
struct Cyc_Absyn_Int_p_struct _tmpE56;struct Cyc_Absyn_Int_p_struct*_tmpE55;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE55=_cycalloc_atomic(sizeof(*_tmpE55)),((
_tmpE55[0]=((_tmpE56.tag=9,((_tmpE56.f1=_tmp7A4,((_tmpE56.f2=(int)_tmp7A5,
_tmpE56)))))),_tmpE55)))),e->loc));}goto _LL3A6;_LL3AB: {struct Cyc_Absyn_Const_e_struct*
_tmp7A6=(struct Cyc_Absyn_Const_e_struct*)_tmp79B;if(_tmp7A6->tag != 0)goto _LL3AD;
else{_tmp7A7=_tmp7A6->f1;if((_tmp7A7.Int_c).tag != 5)goto _LL3AD;_tmp7A8=(struct
_tuple6)(_tmp7A7.Int_c).val;_tmp7A9=_tmp7A8.f1;_tmp7AA=_tmp7A8.f2;}}_LL3AC:{
struct Cyc_Absyn_Int_p_struct _tmpE59;struct Cyc_Absyn_Int_p_struct*_tmpE58;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE58=_cycalloc_atomic(sizeof(*_tmpE58)),((
_tmpE58[0]=((_tmpE59.tag=9,((_tmpE59.f1=_tmp7A9,((_tmpE59.f2=_tmp7AA,_tmpE59)))))),
_tmpE58)))),e->loc));}goto _LL3A6;_LL3AD: {struct Cyc_Absyn_Const_e_struct*_tmp7AB=(
struct Cyc_Absyn_Const_e_struct*)_tmp79B;if(_tmp7AB->tag != 0)goto _LL3AF;else{
_tmp7AC=_tmp7AB->f1;if((_tmp7AC.Float_c).tag != 7)goto _LL3AF;_tmp7AD=(struct
_tuple8)(_tmp7AC.Float_c).val;_tmp7AE=_tmp7AD.f1;_tmp7AF=_tmp7AD.f2;}}_LL3AE:{
struct Cyc_Absyn_Float_p_struct _tmpE5C;struct Cyc_Absyn_Float_p_struct*_tmpE5B;
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((
_tmpE5B[0]=((_tmpE5C.tag=11,((_tmpE5C.f1=_tmp7AE,((_tmpE5C.f2=_tmp7AF,_tmpE5C)))))),
_tmpE5B)))),e->loc));}goto _LL3A6;_LL3AF: {struct Cyc_Absyn_Const_e_struct*_tmp7B0=(
struct Cyc_Absyn_Const_e_struct*)_tmp79B;if(_tmp7B0->tag != 0)goto _LL3B1;else{
_tmp7B1=_tmp7B0->f1;if((_tmp7B1.Null_c).tag != 1)goto _LL3B1;_tmp7B2=(int)(_tmp7B1.Null_c).val;}}
_LL3B0: yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));
goto _LL3A6;_LL3B1: {struct Cyc_Absyn_Const_e_struct*_tmp7B3=(struct Cyc_Absyn_Const_e_struct*)
_tmp79B;if(_tmp7B3->tag != 0)goto _LL3B3;else{_tmp7B4=_tmp7B3->f1;if((_tmp7B4.String_c).tag
!= 8)goto _LL3B3;_tmp7B5=(struct _dyneither_ptr)(_tmp7B4.String_c).val;}}_LL3B2:{
const char*_tmpE5D;Cyc_Parse_err(((_tmpE5D="strings cannot occur within patterns",
_tag_dyneither(_tmpE5D,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}
goto _LL3A6;_LL3B3: {struct Cyc_Absyn_Const_e_struct*_tmp7B6=(struct Cyc_Absyn_Const_e_struct*)
_tmp79B;if(_tmp7B6->tag != 0)goto _LL3B5;else{_tmp7B7=_tmp7B6->f1;if((_tmp7B7.Wstring_c).tag
!= 9)goto _LL3B5;_tmp7B8=(struct _dyneither_ptr)(_tmp7B7.Wstring_c).val;}}_LL3B4:{
const char*_tmpE5E;Cyc_Parse_err(((_tmpE5E="strings cannot occur within patterns",
_tag_dyneither(_tmpE5E,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}
goto _LL3A6;_LL3B5: {struct Cyc_Absyn_Const_e_struct*_tmp7B9=(struct Cyc_Absyn_Const_e_struct*)
_tmp79B;if(_tmp7B9->tag != 0)goto _LL3B7;else{_tmp7BA=_tmp7B9->f1;if((_tmp7BA.LongLong_c).tag
!= 6)goto _LL3B7;_tmp7BB=(struct _tuple7)(_tmp7BA.LongLong_c).val;}}_LL3B6:{const
char*_tmpE5F;Cyc_Parse_unimp(((_tmpE5F="long long's in patterns",_tag_dyneither(
_tmpE5F,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A6;
_LL3B7:;_LL3B8: {const char*_tmpE60;Cyc_Parse_err(((_tmpE60="bad constant in case",
_tag_dyneither(_tmpE60,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}
_LL3A6:;}break;}case 376: _LL3A5: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Absyn_UnknownId_p_struct
_tmpE63;struct Cyc_Absyn_UnknownId_p_struct*_tmpE62;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE62=_cycalloc(sizeof(*_tmpE62)),((_tmpE62[0]=((_tmpE63.tag=14,((
_tmpE63.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpE63)))),_tmpE62)))),(
unsigned int)(yyyylsp[0]).first_line));}break;}case 377: _LL3B9: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{const
char*_tmpE64;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((
_tmpE64="as",_tag_dyneither(_tmpE64,sizeof(char),3))))!= 0){const char*_tmpE65;
Cyc_Parse_err(((_tmpE65="expecting `as'",_tag_dyneither(_tmpE65,sizeof(char),15))),(
unsigned int)(yyyylsp[1]).first_line);}}{struct Cyc_Absyn_Var_p_struct _tmpE6F;
struct _dyneither_ptr*_tmpE6E;struct _tuple1*_tmpE6D;struct Cyc_Absyn_Var_p_struct*
_tmpE6C;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE6C=_cycalloc(sizeof(*
_tmpE6C)),((_tmpE6C[0]=((_tmpE6F.tag=1,((_tmpE6F.f1=Cyc_Absyn_new_vardecl(((
_tmpE6D=_cycalloc(sizeof(*_tmpE6D)),((_tmpE6D->f1=Cyc_Absyn_Loc_n,((_tmpE6D->f2=((
_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E[0]=Cyc_yyget_String_tok(yyyyvsp[0]),
_tmpE6E)))),_tmpE6D)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE6F.f2=Cyc_yyget_YY11(
yyyyvsp[2]),_tmpE6F)))))),_tmpE6C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 378: _LL3BA: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct Cyc_List_List*_tmp7D1;int
_tmp7D2;struct _tuple20 _tmp7D0=*Cyc_yyget_YY12(yyyyvsp[2]);_tmp7D1=_tmp7D0.f1;
_tmp7D2=_tmp7D0.f2;{struct Cyc_Absyn_Tuple_p_struct _tmpE72;struct Cyc_Absyn_Tuple_p_struct*
_tmpE71;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE71=_cycalloc(sizeof(*
_tmpE71)),((_tmpE71[0]=((_tmpE72.tag=4,((_tmpE72.f1=_tmp7D1,((_tmpE72.f2=_tmp7D2,
_tmpE72)))))),_tmpE71)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case
379: _LL3BB: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct Cyc_List_List*_tmp7D6;int
_tmp7D7;struct _tuple20 _tmp7D5=*Cyc_yyget_YY12(yyyyvsp[2]);_tmp7D6=_tmp7D5.f1;
_tmp7D7=_tmp7D5.f2;{struct Cyc_Absyn_UnknownCall_p_struct _tmpE75;struct Cyc_Absyn_UnknownCall_p_struct*
_tmpE74;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE74=_cycalloc(sizeof(*
_tmpE74)),((_tmpE74[0]=((_tmpE75.tag=15,((_tmpE75.f1=Cyc_yyget_QualId_tok(
yyyyvsp[0]),((_tmpE75.f2=_tmp7D6,((_tmpE75.f3=_tmp7D7,_tmpE75)))))))),_tmpE74)))),(
unsigned int)(yyyylsp[0]).first_line));}break;}case 380: _LL3BC: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);struct
Cyc_List_List*_tmp7DB;int _tmp7DC;struct _tuple20 _tmp7DA=*Cyc_yyget_YY16(yyyyvsp[3]);
_tmp7DB=_tmp7DA.f1;_tmp7DC=_tmp7DA.f2;{struct Cyc_List_List*_tmp7DD=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(
yyyyvsp[2]));{struct Cyc_Absyn_Aggr_p_struct _tmpE7B;struct Cyc_Absyn_AggrInfo*
_tmpE7A;struct Cyc_Absyn_Aggr_p_struct*_tmpE79;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE79=_cycalloc(sizeof(*_tmpE79)),((_tmpE79[0]=((_tmpE7B.tag=6,((
_tmpE7B.f1=((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A->aggr_info=Cyc_Absyn_UnknownAggr(
Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpE7A->targs=0,_tmpE7A)))))),((
_tmpE7B.f2=_tmp7DD,((_tmpE7B.f3=_tmp7DB,((_tmpE7B.f4=_tmp7DC,_tmpE7B)))))))))),
_tmpE79)))),(unsigned int)(yyyylsp[0]).first_line));}break;};}case 381: _LL3BD: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct Cyc_List_List*_tmp7E2;int
_tmp7E3;struct _tuple20 _tmp7E1=*Cyc_yyget_YY16(yyyyvsp[2]);_tmp7E2=_tmp7E1.f1;
_tmp7E3=_tmp7E1.f2;{struct Cyc_List_List*_tmp7E4=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY40(
yyyyvsp[1]));{struct Cyc_Absyn_Aggr_p_struct _tmpE7E;struct Cyc_Absyn_Aggr_p_struct*
_tmpE7D;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE7D=_cycalloc(sizeof(*
_tmpE7D)),((_tmpE7D[0]=((_tmpE7E.tag=6,((_tmpE7E.f1=0,((_tmpE7E.f2=_tmp7E4,((
_tmpE7E.f3=_tmp7E2,((_tmpE7E.f4=_tmp7E3,_tmpE7E)))))))))),_tmpE7D)))),(
unsigned int)(yyyylsp[0]).first_line));}break;};}case 382: _LL3BE: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct
Cyc_Absyn_Pointer_p_struct _tmpE81;struct Cyc_Absyn_Pointer_p_struct*_tmpE80;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE80=_cycalloc(sizeof(*_tmpE80)),((
_tmpE80[0]=((_tmpE81.tag=5,((_tmpE81.f1=Cyc_yyget_YY11(yyyyvsp[1]),_tmpE81)))),
_tmpE80)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case 383: _LL3BF: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct Cyc_Absyn_Pointer_p_struct
_tmpE8B;struct Cyc_Absyn_Pointer_p_struct _tmpE8A;struct Cyc_Absyn_Pointer_p_struct*
_tmpE89;struct Cyc_Absyn_Pointer_p_struct*_tmpE88;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88[0]=((_tmpE8B.tag=5,((
_tmpE8B.f1=Cyc_Absyn_new_pat((void*)((_tmpE89=_cycalloc(sizeof(*_tmpE89)),((
_tmpE89[0]=((_tmpE8A.tag=5,((_tmpE8A.f1=Cyc_yyget_YY11(yyyyvsp[1]),_tmpE8A)))),
_tmpE89)))),(unsigned int)(yyyylsp[0]).first_line),_tmpE8B)))),_tmpE88)))),(
unsigned int)(yyyylsp[0]).first_line));}break;}case 384: _LL3C0: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct
Cyc_Absyn_Reference_p_struct _tmpE95;struct _dyneither_ptr*_tmpE94;struct _tuple1*
_tmpE93;struct Cyc_Absyn_Reference_p_struct*_tmpE92;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE92=_cycalloc(sizeof(*_tmpE92)),((_tmpE92[0]=((_tmpE95.tag=2,((
_tmpE95.f1=Cyc_Absyn_new_vardecl(((_tmpE93=_cycalloc(sizeof(*_tmpE93)),((_tmpE93->f1=
Cyc_Absyn_Loc_n,((_tmpE93->f2=((_tmpE94=_cycalloc(sizeof(*_tmpE94)),((_tmpE94[0]=
Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE94)))),_tmpE93)))))),(void*)& Cyc_Absyn_VoidType_val,
0),((_tmpE95.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(
yyyylsp[1]).first_line),_tmpE95)))))),_tmpE92)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 385: _LL3C1: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{const char*_tmpE96;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpE96="as",
_tag_dyneither(_tmpE96,sizeof(char),3))))!= 0){const char*_tmpE97;Cyc_Parse_err(((
_tmpE97="expecting `as'",_tag_dyneither(_tmpE97,sizeof(char),15))),(unsigned int)(
yyyylsp[2]).first_line);}}{struct Cyc_Absyn_Reference_p_struct _tmpEA1;struct
_dyneither_ptr*_tmpEA0;struct _tuple1*_tmpE9F;struct Cyc_Absyn_Reference_p_struct*
_tmpE9E;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE9E=_cycalloc(sizeof(*
_tmpE9E)),((_tmpE9E[0]=((_tmpEA1.tag=2,((_tmpEA1.f1=Cyc_Absyn_new_vardecl(((
_tmpE9F=_cycalloc(sizeof(*_tmpE9F)),((_tmpE9F->f1=Cyc_Absyn_Loc_n,((_tmpE9F->f2=((
_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0[0]=Cyc_yyget_String_tok(yyyyvsp[1]),
_tmpEA0)))),_tmpE9F)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpEA1.f2=Cyc_yyget_YY11(
yyyyvsp[3]),_tmpEA1)))))),_tmpE9E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 386: _LL3C2: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);void*_tmp7F7=Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct
_tmpEB2;struct Cyc_Absyn_TagType_struct _tmpEB1;struct Cyc_Absyn_TagType_struct*
_tmpEB0;struct _dyneither_ptr*_tmpEAF;struct _tuple1*_tmpEAE;struct Cyc_Absyn_TagInt_p_struct*
_tmpEAD;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpEAD=_cycalloc(sizeof(*
_tmpEAD)),((_tmpEAD[0]=((_tmpEB2.tag=3,((_tmpEB2.f1=Cyc_Parse_typ2tvar((
unsigned int)(yyyylsp[2]).first_line,_tmp7F7),((_tmpEB2.f2=Cyc_Absyn_new_vardecl(((
_tmpEAE=_cycalloc(sizeof(*_tmpEAE)),((_tmpEAE->f1=Cyc_Absyn_Loc_n,((_tmpEAE->f2=((
_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((_tmpEAF[0]=Cyc_yyget_String_tok(yyyyvsp[0]),
_tmpEAF)))),_tmpEAE)))))),(void*)((_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0[
0]=((_tmpEB1.tag=19,((_tmpEB1.f1=(void*)_tmp7F7,_tmpEB1)))),_tmpEB0)))),0),
_tmpEB2)))))),_tmpEAD)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case
387: _LL3C3: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);struct Cyc_Absyn_Tvar*_tmp7FE=Cyc_Tcutil_new_tvar(
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct
_tmpECE;struct Cyc_Absyn_TagType_struct _tmpECD;struct Cyc_Absyn_VarType_struct
_tmpECC;struct Cyc_Absyn_VarType_struct*_tmpECB;struct Cyc_Absyn_TagType_struct*
_tmpECA;struct _dyneither_ptr*_tmpEC9;struct _tuple1*_tmpEC8;struct Cyc_Absyn_TagInt_p_struct*
_tmpEC7;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpEC7=_cycalloc(sizeof(*
_tmpEC7)),((_tmpEC7[0]=((_tmpECE.tag=3,((_tmpECE.f1=_tmp7FE,((_tmpECE.f2=Cyc_Absyn_new_vardecl(((
_tmpEC8=_cycalloc(sizeof(*_tmpEC8)),((_tmpEC8->f1=Cyc_Absyn_Loc_n,((_tmpEC8->f2=((
_tmpEC9=_cycalloc(sizeof(*_tmpEC9)),((_tmpEC9[0]=Cyc_yyget_String_tok(yyyyvsp[0]),
_tmpEC9)))),_tmpEC8)))))),(void*)((_tmpECA=_cycalloc(sizeof(*_tmpECA)),((_tmpECA[
0]=((_tmpECD.tag=19,((_tmpECD.f1=(void*)((void*)((_tmpECB=_cycalloc(sizeof(*
_tmpECB)),((_tmpECB[0]=((_tmpECC.tag=2,((_tmpECC.f1=_tmp7FE,_tmpECC)))),_tmpECB))))),
_tmpECD)))),_tmpECA)))),0),_tmpECE)))))),_tmpEC7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 388: _LL3C4: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _tuple20*_tmpECF;yyval=Cyc_YY12(((
_tmpECF=_cycalloc(sizeof(*_tmpECF)),((_tmpECF->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpECF->f2=0,
_tmpECF)))))));}break;}case 389: _LL3C5: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct _tuple20*_tmpED0;yyval=Cyc_YY12(((
_tmpED0=_cycalloc(sizeof(*_tmpED0)),((_tmpED0->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpED0->f2=1,
_tmpED0)))))));}break;}case 390: _LL3C6: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _tuple20*_tmpED1;yyval=Cyc_YY12(((
_tmpED1=_cycalloc(sizeof(*_tmpED1)),((_tmpED1->f1=0,((_tmpED1->f2=1,_tmpED1)))))));}
break;}case 391: _LL3C7: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_List_List*_tmpED2;yyval=
Cyc_YY13(((_tmpED2=_cycalloc(sizeof(*_tmpED2)),((_tmpED2->hd=Cyc_yyget_YY11(
yyyyvsp[0]),((_tmpED2->tl=0,_tmpED2)))))));}break;}case 392: _LL3C8: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct
Cyc_List_List*_tmpED3;yyval=Cyc_YY13(((_tmpED3=_cycalloc(sizeof(*_tmpED3)),((
_tmpED3->hd=Cyc_yyget_YY11(yyyyvsp[2]),((_tmpED3->tl=Cyc_yyget_YY13(yyyyvsp[0]),
_tmpED3)))))));}break;}case 393: _LL3C9: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _tuple21*_tmpED4;yyval=Cyc_YY14(((
_tmpED4=_cycalloc(sizeof(*_tmpED4)),((_tmpED4->f1=0,((_tmpED4->f2=Cyc_yyget_YY11(
yyyyvsp[0]),_tmpED4)))))));}break;}case 394: _LL3CA: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);{struct _tuple21*_tmpED5;yyval=Cyc_YY14(((
_tmpED5=_cycalloc(sizeof(*_tmpED5)),((_tmpED5->f1=Cyc_yyget_YY41(yyyyvsp[0]),((
_tmpED5->f2=Cyc_yyget_YY11(yyyyvsp[1]),_tmpED5)))))));}break;}case 395: _LL3CB: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _tuple20*_tmpED6;yyval=Cyc_YY16(((
_tmpED6=_cycalloc(sizeof(*_tmpED6)),((_tmpED6->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyyyvsp[0])),((_tmpED6->f2=0,
_tmpED6)))))));}break;}case 396: _LL3CC: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct _tuple20*_tmpED7;yyval=Cyc_YY16(((
_tmpED7=_cycalloc(sizeof(*_tmpED7)),((_tmpED7->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyyyvsp[0])),((_tmpED7->f2=1,
_tmpED7)))))));}break;}case 397: _LL3CD: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _tuple20*_tmpED8;yyval=Cyc_YY16(((
_tmpED8=_cycalloc(sizeof(*_tmpED8)),((_tmpED8->f1=0,((_tmpED8->f2=1,_tmpED8)))))));}
break;}case 398: _LL3CE: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_List_List*_tmpED9;yyval=
Cyc_YY15(((_tmpED9=_cycalloc(sizeof(*_tmpED9)),((_tmpED9->hd=Cyc_yyget_YY14(
yyyyvsp[0]),((_tmpED9->tl=0,_tmpED9)))))));}break;}case 399: _LL3CF: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct
Cyc_List_List*_tmpEDA;yyval=Cyc_YY15(((_tmpEDA=_cycalloc(sizeof(*_tmpEDA)),((
_tmpEDA->hd=Cyc_yyget_YY14(yyyyvsp[2]),((_tmpEDA->tl=Cyc_yyget_YY15(yyyyvsp[0]),
_tmpEDA)))))));}break;}case 400: _LL3D0: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 401:
_LL3D1: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_seq_exp(
Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 402: _LL3D2: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 403:
_LL3D3: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(
Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY8(yyyyvsp[1]),Cyc_yyget_YY3(yyyyvsp[2]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 404: _LL3D4: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=
Cyc_YY3(Cyc_Absyn_swap_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 405: _LL3D5: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
Cyc_YY8(0);break;}case 406: _LL3D6: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Core_Opt*_tmpEDB;yyval=
Cyc_YY8(((_tmpEDB=_cycalloc_atomic(sizeof(*_tmpEDB)),((_tmpEDB->v=(void*)Cyc_Absyn_Times,
_tmpEDB)))));}break;}case 407: _LL3D7: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Core_Opt*_tmpEDC;yyval=
Cyc_YY8(((_tmpEDC=_cycalloc_atomic(sizeof(*_tmpEDC)),((_tmpEDC->v=(void*)Cyc_Absyn_Div,
_tmpEDC)))));}break;}case 408: _LL3D8: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Core_Opt*_tmpEDD;yyval=
Cyc_YY8(((_tmpEDD=_cycalloc_atomic(sizeof(*_tmpEDD)),((_tmpEDD->v=(void*)Cyc_Absyn_Mod,
_tmpEDD)))));}break;}case 409: _LL3D9: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Core_Opt*_tmpEDE;yyval=
Cyc_YY8(((_tmpEDE=_cycalloc_atomic(sizeof(*_tmpEDE)),((_tmpEDE->v=(void*)Cyc_Absyn_Plus,
_tmpEDE)))));}break;}case 410: _LL3DA: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Core_Opt*_tmpEDF;yyval=
Cyc_YY8(((_tmpEDF=_cycalloc_atomic(sizeof(*_tmpEDF)),((_tmpEDF->v=(void*)Cyc_Absyn_Minus,
_tmpEDF)))));}break;}case 411: _LL3DB: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Core_Opt*_tmpEE0;yyval=
Cyc_YY8(((_tmpEE0=_cycalloc_atomic(sizeof(*_tmpEE0)),((_tmpEE0->v=(void*)Cyc_Absyn_Bitlshift,
_tmpEE0)))));}break;}case 412: _LL3DC: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Core_Opt*_tmpEE1;yyval=
Cyc_YY8(((_tmpEE1=_cycalloc_atomic(sizeof(*_tmpEE1)),((_tmpEE1->v=(void*)Cyc_Absyn_Bitlrshift,
_tmpEE1)))));}break;}case 413: _LL3DD: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Core_Opt*_tmpEE2;yyval=
Cyc_YY8(((_tmpEE2=_cycalloc_atomic(sizeof(*_tmpEE2)),((_tmpEE2->v=(void*)Cyc_Absyn_Bitand,
_tmpEE2)))));}break;}case 414: _LL3DE: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Core_Opt*_tmpEE3;yyval=
Cyc_YY8(((_tmpEE3=_cycalloc_atomic(sizeof(*_tmpEE3)),((_tmpEE3->v=(void*)Cyc_Absyn_Bitxor,
_tmpEE3)))));}break;}case 415: _LL3DF: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Core_Opt*_tmpEE4;yyval=
Cyc_YY8(((_tmpEE4=_cycalloc_atomic(sizeof(*_tmpEE4)),((_tmpEE4->v=(void*)Cyc_Absyn_Bitor,
_tmpEE4)))));}break;}case 416: _LL3E0: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 417:
_LL3E1: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union
Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);yyval=Cyc_YY3(Cyc_Absyn_conditional_exp(
Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 418: _LL3E2: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=
Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 419: _LL3E3: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,
Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));break;}case 420:
_LL3E4: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,
Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));break;}case 421:
_LL3E5: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union
Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);yyval=Cyc_YY3(Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 422: _LL3E6: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);yyval=
Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(
yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));break;}case 423: _LL3E7: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 424:
_LL3E8: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 425:
_LL3E9: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_or_exp(Cyc_yyget_YY3(
yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 426: _LL3EA: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 427:
_LL3EB: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_and_exp(
Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3EC: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 429:
_LL3ED: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitor,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 430: _LL3EE: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
yyyyvsp[0];break;}case 431: _LL3EF: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitxor,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 432: _LL3F0: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
yyyyvsp[0];break;}case 433: _LL3F1: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitand,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 434: _LL3F2: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
yyyyvsp[0];break;}case 435: _LL3F3: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(
yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 436: _LL3F4: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_neq_exp(
Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 437: _LL3F5: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 438:
_LL3F6: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(
yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 439: _LL3F7: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_gt_exp(Cyc_yyget_YY3(
yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 440: _LL3F8: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_lte_exp(
Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 441: _LL3F9: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_gte_exp(
Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 442: _LL3FA: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 443:
_LL3FB: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitlshift,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 444: _LL3FC: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=
Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(
yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));break;}case 445: _LL3FD: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 446:
_LL3FE: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(
Cyc_Absyn_Plus,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(
yyyylsp[0]).first_line));break;}case 447: _LL3FF: {union Cyc_YYSTYPE*yyyyvsp=(union
Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(
Cyc_Absyn_Minus,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 448: _LL400: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
yyyyvsp[0];break;}case 449: _LL401: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(
Cyc_Absyn_Times,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(
unsigned int)(yyyylsp[0]).first_line));break;}case 450: _LL402: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=
Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(
yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));break;}case 451: _LL403: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(
Cyc_Absyn_Mod,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(
yyyylsp[0]).first_line));break;}case 452: _LL404: {union Cyc_YYSTYPE*yyyyvsp=(union
Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 453:
_LL405: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);void*_tmp81D=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);yyval=Cyc_YY3(
Cyc_Absyn_cast_exp(_tmp81D,Cyc_yyget_YY3(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(
unsigned int)(yyyylsp[0]).first_line));break;}case 454: _LL406: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
yyyyvsp[0];break;}case 455: _LL407: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY3(Cyc_Absyn_pre_inc_exp(
Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));break;}case 456:
_LL408: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(
Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));break;}case 457:
_LL409: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY3(Cyc_Absyn_address_exp(
Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));break;}case 458:
_LL40A: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY3(Cyc_Absyn_deref_exp(
Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));break;}case 459:
_LL40B: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=yyyyvsp[1];break;}case 460:
_LL40C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(
Cyc_yyget_YY7(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 461: _LL40D: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);void*_tmp81E=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);yyval=Cyc_YY3(
Cyc_Absyn_sizeoftyp_exp(_tmp81E,(unsigned int)(yyyylsp[0]).first_line));break;}
case 462: _LL40E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));break;}case 463:
_LL40F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union
Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);void*_tmp81F=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);{struct Cyc_Absyn_StructField_struct
_tmpEEA;struct _dyneither_ptr*_tmpEE9;struct Cyc_Absyn_StructField_struct*_tmpEE8;
yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(_tmp81F,(void*)((_tmpEE8=_cycalloc(sizeof(*
_tmpEE8)),((_tmpEE8[0]=((_tmpEEA.tag=0,((_tmpEEA.f1=((_tmpEE9=_cycalloc(sizeof(*
_tmpEE9)),((_tmpEE9[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEE9)))),_tmpEEA)))),
_tmpEE8)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case 464: _LL410: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union
Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);void*_tmp823=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);{struct Cyc_Absyn_TupleIndex_struct
_tmpEED;struct Cyc_Absyn_TupleIndex_struct*_tmpEEC;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(
_tmp823,(void*)((_tmpEEC=_cycalloc_atomic(sizeof(*_tmpEEC)),((_tmpEEC[0]=((
_tmpEED.tag=1,((_tmpEED.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[4])).f2,
_tmpEED)))),_tmpEEC)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case 465:
_LL411: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_Absyn_Malloc_e_struct
_tmpEF3;struct Cyc_Absyn_MallocInfo _tmpEF2;struct Cyc_Absyn_Malloc_e_struct*
_tmpEF1;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEF1=_cycalloc(sizeof(*
_tmpEF1)),((_tmpEF1[0]=((_tmpEF3.tag=34,((_tmpEF3.f1=((_tmpEF2.is_calloc=0,((
_tmpEF2.rgn=0,((_tmpEF2.elt_type=0,((_tmpEF2.num_elts=Cyc_yyget_YY3(yyyyvsp[2]),((
_tmpEF2.fat_result=0,_tmpEF2)))))))))),_tmpEF3)))),_tmpEF1)))),(unsigned int)(
yyyylsp[0]).first_line));}break;}case 466: _LL412: {union Cyc_YYSTYPE*yyyyvsp=(
union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);{struct Cyc_Absyn_Malloc_e_struct
_tmpEF9;struct Cyc_Absyn_MallocInfo _tmpEF8;struct Cyc_Absyn_Malloc_e_struct*
_tmpEF7;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEF7=_cycalloc(sizeof(*
_tmpEF7)),((_tmpEF7[0]=((_tmpEF9.tag=34,((_tmpEF9.f1=((_tmpEF8.is_calloc=0,((
_tmpEF8.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),((_tmpEF8.elt_type=0,((
_tmpEF8.num_elts=Cyc_yyget_YY3(yyyyvsp[4]),((_tmpEF8.fat_result=0,_tmpEF8)))))))))),
_tmpEF9)))),_tmpEF7)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case 467:
_LL413: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union
Cyc_YYSTYPE),9);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);void*_tmp82C=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);{struct Cyc_Absyn_Malloc_e_struct
_tmpF03;void**_tmpF02;struct Cyc_Absyn_MallocInfo _tmpF01;struct Cyc_Absyn_Malloc_e_struct*
_tmpF00;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpF00=_cycalloc(sizeof(*
_tmpF00)),((_tmpF00[0]=((_tmpF03.tag=34,((_tmpF03.f1=((_tmpF01.is_calloc=1,((
_tmpF01.rgn=0,((_tmpF01.elt_type=((_tmpF02=_cycalloc(sizeof(*_tmpF02)),((_tmpF02[
0]=_tmp82C,_tmpF02)))),((_tmpF01.num_elts=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpF01.fat_result=
0,_tmpF01)))))))))),_tmpF03)))),_tmpF00)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 468: _LL414: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);void*_tmp831=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);{struct Cyc_Absyn_Malloc_e_struct
_tmpF0D;void**_tmpF0C;struct Cyc_Absyn_MallocInfo _tmpF0B;struct Cyc_Absyn_Malloc_e_struct*
_tmpF0A;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpF0A=_cycalloc(sizeof(*
_tmpF0A)),((_tmpF0A[0]=((_tmpF0D.tag=34,((_tmpF0D.f1=((_tmpF0B.is_calloc=1,((
_tmpF0B.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyyyvsp[2]),((_tmpF0B.elt_type=((
_tmpF0C=_cycalloc(sizeof(*_tmpF0C)),((_tmpF0C[0]=_tmp831,_tmpF0C)))),((_tmpF0B.num_elts=
Cyc_yyget_YY3(yyyyvsp[4]),((_tmpF0B.fat_result=0,_tmpF0B)))))))))),_tmpF0D)))),
_tmpF0A)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case 469: _LL415: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);{struct Cyc_Absyn_Exp*_tmpF0E[1];
yyval=Cyc_YY3(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpF0E[0]=Cyc_yyget_YY3(
yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpF0E,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[
0]).first_line));}break;}case 470: _LL416: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);{struct Cyc_Absyn_Tagcheck_e_struct
_tmpF14;struct _dyneither_ptr*_tmpF13;struct Cyc_Absyn_Tagcheck_e_struct*_tmpF12;
yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpF12=_cycalloc(sizeof(*_tmpF12)),((
_tmpF12[0]=((_tmpF14.tag=38,((_tmpF14.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpF14.f2=((
_tmpF13=_cycalloc(sizeof(*_tmpF13)),((_tmpF13[0]=Cyc_yyget_String_tok(yyyyvsp[4]),
_tmpF13)))),_tmpF14)))))),_tmpF12)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 471: _LL417: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);{struct Cyc_Absyn_Tagcheck_e_struct
_tmpF1A;struct _dyneither_ptr*_tmpF19;struct Cyc_Absyn_Tagcheck_e_struct*_tmpF18;
yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpF18=_cycalloc(sizeof(*_tmpF18)),((
_tmpF18[0]=((_tmpF1A.tag=38,((_tmpF1A.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(
yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpF1A.f2=((_tmpF19=
_cycalloc(sizeof(*_tmpF19)),((_tmpF19[0]=Cyc_yyget_String_tok(yyyyvsp[4]),
_tmpF19)))),_tmpF1A)))))),_tmpF18)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 472: _LL418: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);void*_tmp83D=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);yyval=Cyc_YY3(
Cyc_Absyn_valueof_exp(_tmp83D,(unsigned int)(yyyylsp[0]).first_line));break;}
case 473: _LL419: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);int _tmp83F;struct _dyneither_ptr
_tmp840;struct _tuple18 _tmp83E=Cyc_yyget_Asm_tok(yyyyvsp[0]);_tmp83F=_tmp83E.f1;
_tmp840=_tmp83E.f2;yyval=Cyc_YY3(Cyc_Absyn_asm_exp(_tmp83F,_tmp840,(unsigned int)(
yyyylsp[0]).first_line));break;}case 474: _LL41A: {union Cyc_YYSTYPE*yyyyvsp=(union
Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY7(Cyc_Absyn_Bitnot);
break;}case 475: _LL41B: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY7(Cyc_Absyn_Not);
break;}case 476: _LL41C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY7(Cyc_Absyn_Minus);
break;}case 477: _LL41D: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 478:
_LL41E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);yyval=Cyc_YY3(Cyc_Absyn_subscript_exp(
Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 479: _LL41F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(
Cyc_yyget_YY3(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));break;}case
480: _LL420: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(
Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY5(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 481: _LL421: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct _dyneither_ptr*_tmpF1B;
yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(yyyyvsp[0]),((_tmpF1B=
_cycalloc(sizeof(*_tmpF1B)),((_tmpF1B[0]=Cyc_yyget_String_tok(yyyyvsp[2]),
_tmpF1B)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case 482: _LL422: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union
Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct _dyneither_ptr*_tmpF1C;
yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(yyyyvsp[0]),((_tmpF1C=
_cycalloc(sizeof(*_tmpF1C)),((_tmpF1C[0]=Cyc_yyget_String_tok(yyyyvsp[2]),
_tmpF1C)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case 483: _LL423: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(
Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));break;}case 484:
_LL424: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(
Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));break;}case 485:
_LL425: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union
Cyc_YYSTYPE),6);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);{struct Cyc_Absyn_CompoundLit_e_struct
_tmpF1F;struct Cyc_Absyn_CompoundLit_e_struct*_tmpF1E;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpF1E=_cycalloc(sizeof(*_tmpF1E)),((_tmpF1E[0]=((_tmpF1F.tag=26,((
_tmpF1F.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpF1F.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[4])),_tmpF1F)))))),
_tmpF1E)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case 486: _LL426: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union
Cyc_YYSTYPE),7);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);{struct Cyc_Absyn_CompoundLit_e_struct
_tmpF22;struct Cyc_Absyn_CompoundLit_e_struct*_tmpF21;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpF21=_cycalloc(sizeof(*_tmpF21)),((_tmpF21[0]=((_tmpF22.tag=26,((
_tmpF22.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpF22.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[4])),_tmpF22)))))),
_tmpF21)))),(unsigned int)(yyyylsp[0]).first_line));}break;}case 487: _LL427: {
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_Absyn_UnknownId_e_struct
_tmpF25;struct Cyc_Absyn_UnknownId_e_struct*_tmpF24;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpF24=_cycalloc(sizeof(*_tmpF24)),((_tmpF24[0]=((_tmpF25.tag=2,((
_tmpF25.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpF25)))),_tmpF24)))),(
unsigned int)(yyyylsp[0]).first_line));}break;}case 488: _LL428: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=
yyyyvsp[0];break;}case 489: _LL429: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY3(Cyc_Absyn_string_exp(
Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));break;}
case 490: _LL42A: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY3(Cyc_Absyn_wstring_exp(
Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));break;}
case 491: _LL42B: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);yyval=yyyyvsp[1];break;}case 492:
_LL42C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union
Cyc_YYSTYPE),2);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(
Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));break;}case 493:
_LL42D: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union
Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(
Cyc_yyget_YY3(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY40(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));break;}case
494: _LL42E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union
Cyc_YYSTYPE),4);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(
Cyc_yyget_YY5(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));break;}case 495:
_LL42F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union
Cyc_YYSTYPE),5);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);{struct Cyc_Absyn_Aggregate_e_struct
_tmpF28;struct Cyc_Absyn_Aggregate_e_struct*_tmpF27;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpF27=_cycalloc(sizeof(*_tmpF27)),((_tmpF27[0]=((_tmpF28.tag=29,((
_tmpF28.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpF28.f2=Cyc_yyget_YY40(yyyyvsp[
2]),((_tmpF28.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyyyvsp[3])),((_tmpF28.f4=0,_tmpF28)))))))))),_tmpF27)))),(
unsigned int)(yyyylsp[0]).first_line));}break;}case 496: _LL430: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);yyval=
Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 497: _LL431: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY5(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[0])));break;}case
498: _LL432: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct Cyc_List_List*_tmpF29;yyval=
Cyc_YY5(((_tmpF29=_cycalloc(sizeof(*_tmpF29)),((_tmpF29->hd=Cyc_yyget_YY3(
yyyyvsp[0]),((_tmpF29->tl=0,_tmpF29)))))));}break;}case 499: _LL433: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);{struct
Cyc_List_List*_tmpF2A;yyval=Cyc_YY5(((_tmpF2A=_cycalloc(sizeof(*_tmpF2A)),((
_tmpF2A->hd=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpF2A->tl=Cyc_yyget_YY5(yyyyvsp[0]),
_tmpF2A)))))));}break;}case 500: _LL434: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);struct _tuple6 _tmp84D=Cyc_yyget_Int_tok(
yyyyvsp[0]);yyval=Cyc_YY3(Cyc_Absyn_int_exp(_tmp84D.f1,_tmp84D.f2,(unsigned int)(
yyyylsp[0]).first_line));break;}case 501: _LL435: {union Cyc_YYSTYPE*yyyyvsp=(union
Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY3(Cyc_Absyn_char_exp(
Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));break;}
case 502: _LL436: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=Cyc_YY3(Cyc_Absyn_wchar_exp(
Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));break;}
case 503: _LL437: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);struct _dyneither_ptr _tmp84E=Cyc_yyget_String_tok(
yyyyvsp[0]);int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp84E);int i=1;if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp84E,sizeof(char),l - 1));if(c
== 'f'  || c == 'F')i=0;else{if(c == 'l'  || c == 'L')i=2;}}yyval=Cyc_YY3(Cyc_Absyn_float_exp(
_tmp84E,i,(unsigned int)(yyyylsp[0]).first_line));break;}case 504: _LL438: {union
Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(
struct Cyc_Yyltype),1);yyval=Cyc_YY3(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL439: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _dyneither_ptr*_tmpF2D;
struct _tuple1*_tmpF2C;yyval=Cyc_QualId_tok(((_tmpF2C=_cycalloc(sizeof(*_tmpF2C)),((
_tmpF2C->f1=Cyc_Absyn_Rel_n(0),((_tmpF2C->f2=((_tmpF2D=_cycalloc(sizeof(*_tmpF2D)),((
_tmpF2D[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF2D)))),_tmpF2C)))))));}break;}
case 506: _LL43A: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 507:
_LL43B: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);{struct _dyneither_ptr*_tmpF30;
struct _tuple1*_tmpF2F;yyval=Cyc_QualId_tok(((_tmpF2F=_cycalloc(sizeof(*_tmpF2F)),((
_tmpF2F->f1=Cyc_Absyn_Rel_n(0),((_tmpF2F->f2=((_tmpF30=_cycalloc(sizeof(*_tmpF30)),((
_tmpF30[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF30)))),_tmpF2F)))))));}break;}
case 508: _LL43C: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct
Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 509:
_LL43D: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 510:
_LL43E: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 511:
_LL43F: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 512:
_LL440: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);yyval=yyyyvsp[0];break;}case 513:
_LL441: {union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(
_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union
Cyc_YYSTYPE),1);struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)
_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);break;}case 514: _LL442: {union Cyc_YYSTYPE*
yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);struct Cyc_Yyltype*
yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,
sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yylex_buf->lex_curr_pos -=1;break;}default: _LL443: break;}yyvsp_offset -=yylen;
yyssp_offset -=yylen;yylsp_offset -=yylen;*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yyval;
++ yylsp_offset;if(yylen == 0){(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line=Cyc_yylloc.first_line;(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_column=Cyc_yylloc.first_column;(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_column;}
else{(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,
sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(
yylsp_offset + yylen)- 1))).last_column;}yyn=(int)Cyc_yyr1[
_check_known_subscript_notnull(515,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(143,yyn - 146)]+ *((short*)
_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));if((yystate >= 0  && 
yystate <= 6375) && Cyc_yycheck[_check_known_subscript_notnull(6376,yystate)]== *((
short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))yystate=(int)
Cyc_yytable[_check_known_subscript_notnull(6376,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(143,yyn - 146)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1030,yystate)];if(yyn > - 32768  && yyn < 6375){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 289 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6376,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(289,x)])+ 15,count ++);}{unsigned int _tmpF35;
unsigned int _tmpF34;struct _dyneither_ptr _tmpF33;char*_tmpF32;unsigned int _tmpF31;
msg=((_tmpF31=(unsigned int)(sze + 15),((_tmpF32=(char*)_region_malloc(yyregion,
_check_times(sizeof(char),_tmpF31 + 1)),((_tmpF33=_tag_dyneither(_tmpF32,sizeof(
char),_tmpF31 + 1),((((_tmpF34=_tmpF31,((_tmpF36(& _tmpF35,& _tmpF34,& _tmpF32),
_tmpF32[_tmpF34]=(char)0)))),_tmpF33))))))));}{const char*_tmpF37;Cyc_strcpy(msg,((
_tmpF37="parse error",_tag_dyneither(_tmpF37,sizeof(char),12))));}if(count < 5){
count=0;for(x=yyn < 0?- yyn: 0;x < 289 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6376,x + yyn)]== x){{const char*_tmpF39;const char*
_tmpF38;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)((
_tmpF39=", expecting `",_tag_dyneither(_tmpF39,sizeof(char),14))):(struct
_dyneither_ptr)((_tmpF38=" or `",_tag_dyneither(_tmpF38,sizeof(char),6)))));}Cyc_strcat(
msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(289,x)]);{
const char*_tmpF3A;Cyc_strcat(msg,((_tmpF3A="'",_tag_dyneither(_tmpF3A,sizeof(
char),2))));}++ count;}}}Cyc_yyerror((struct _dyneither_ptr)msg);}else{const char*
_tmpF3B;Cyc_yyerror(((_tmpF3B="parse error",_tag_dyneither(_tmpF3B,sizeof(char),
12))));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int
_tmp85C=1;_npop_handler(0);return _tmp85C;}Cyc_yychar=- 2;}yyerrstatus=3;goto
yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp85D=1;
_npop_handler(0);return _tmp85D;}-- yyvsp_offset;yystate=(int)*((short*)
_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));-- yylsp_offset;
yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1030,yystate)];
if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6375) || Cyc_yycheck[
_check_known_subscript_notnull(6376,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6376,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 1029){int
_tmp85E=0;_npop_handler(0);return _tmp85E;}*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=Cyc_yylval;*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++
yylsp_offset))=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE _tmp867=v;struct _tuple6 _tmp868;int
_tmp869;char _tmp86A;struct _dyneither_ptr _tmp86B;struct _tuple1*_tmp86C;struct
_tuple1 _tmp86D;union Cyc_Absyn_Nmspace _tmp86E;struct _dyneither_ptr*_tmp86F;_LL446:
if((_tmp867.Int_tok).tag != 1)goto _LL448;_tmp868=(struct _tuple6)(_tmp867.Int_tok).val;
_tmp869=_tmp868.f2;_LL447:{const char*_tmpF3F;void*_tmpF3E[1];struct Cyc_Int_pa_struct
_tmpF3D;(_tmpF3D.tag=1,((_tmpF3D.f1=(unsigned long)_tmp869,((_tmpF3E[0]=& _tmpF3D,
Cyc_fprintf(Cyc_stderr,((_tmpF3F="%d",_tag_dyneither(_tmpF3F,sizeof(char),3))),
_tag_dyneither(_tmpF3E,sizeof(void*),1)))))));}goto _LL445;_LL448: if((_tmp867.Char_tok).tag
!= 2)goto _LL44A;_tmp86A=(char)(_tmp867.Char_tok).val;_LL449:{const char*_tmpF43;
void*_tmpF42[1];struct Cyc_Int_pa_struct _tmpF41;(_tmpF41.tag=1,((_tmpF41.f1=(
unsigned long)((int)_tmp86A),((_tmpF42[0]=& _tmpF41,Cyc_fprintf(Cyc_stderr,((
_tmpF43="%c",_tag_dyneither(_tmpF43,sizeof(char),3))),_tag_dyneither(_tmpF42,
sizeof(void*),1)))))));}goto _LL445;_LL44A: if((_tmp867.String_tok).tag != 3)goto
_LL44C;_tmp86B=(struct _dyneither_ptr)(_tmp867.String_tok).val;_LL44B:{const char*
_tmpF47;void*_tmpF46[1];struct Cyc_String_pa_struct _tmpF45;(_tmpF45.tag=0,((
_tmpF45.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp86B),((_tmpF46[0]=&
_tmpF45,Cyc_fprintf(Cyc_stderr,((_tmpF47="\"%s\"",_tag_dyneither(_tmpF47,sizeof(
char),5))),_tag_dyneither(_tmpF46,sizeof(void*),1)))))));}goto _LL445;_LL44C: if((
_tmp867.QualId_tok).tag != 5)goto _LL44E;_tmp86C=(struct _tuple1*)(_tmp867.QualId_tok).val;
_tmp86D=*_tmp86C;_tmp86E=_tmp86D.f1;_tmp86F=_tmp86D.f2;_LL44D: {struct Cyc_List_List*
_tmp879=0;{union Cyc_Absyn_Nmspace _tmp87A=_tmp86E;struct Cyc_List_List*_tmp87B;
struct Cyc_List_List*_tmp87C;struct Cyc_List_List*_tmp87D;int _tmp87E;_LL451: if((
_tmp87A.Rel_n).tag != 1)goto _LL453;_tmp87B=(struct Cyc_List_List*)(_tmp87A.Rel_n).val;
_LL452: _tmp879=_tmp87B;goto _LL450;_LL453: if((_tmp87A.Abs_n).tag != 2)goto _LL455;
_tmp87C=(struct Cyc_List_List*)(_tmp87A.Abs_n).val;_LL454: _tmp879=_tmp87C;goto
_LL450;_LL455: if((_tmp87A.C_n).tag != 3)goto _LL457;_tmp87D=(struct Cyc_List_List*)(
_tmp87A.C_n).val;_LL456: _tmp879=_tmp87D;goto _LL450;_LL457: if((_tmp87A.Loc_n).tag
!= 4)goto _LL450;_tmp87E=(int)(_tmp87A.Loc_n).val;_LL458: goto _LL450;_LL450:;}for(
0;_tmp879 != 0;_tmp879=_tmp879->tl){const char*_tmpF4B;void*_tmpF4A[1];struct Cyc_String_pa_struct
_tmpF49;(_tmpF49.tag=0,((_tmpF49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp879->hd)),((_tmpF4A[0]=& _tmpF49,Cyc_fprintf(Cyc_stderr,((
_tmpF4B="%s::",_tag_dyneither(_tmpF4B,sizeof(char),5))),_tag_dyneither(_tmpF4A,
sizeof(void*),1)))))));}{const char*_tmpF4F;void*_tmpF4E[1];struct Cyc_String_pa_struct
_tmpF4D;(_tmpF4D.tag=0,((_tmpF4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp86F),((_tmpF4E[0]=& _tmpF4D,Cyc_fprintf(Cyc_stderr,((_tmpF4F="%s::",
_tag_dyneither(_tmpF4F,sizeof(char),5))),_tag_dyneither(_tmpF4E,sizeof(void*),1)))))));}
goto _LL445;}_LL44E:;_LL44F:{const char*_tmpF52;void*_tmpF51;(_tmpF51=0,Cyc_fprintf(
Cyc_stderr,((_tmpF52="?",_tag_dyneither(_tmpF52,sizeof(char),2))),_tag_dyneither(
_tmpF51,sizeof(void*),0)));}goto _LL445;_LL445:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
