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
_tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct
_tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct
_tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{
int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};
struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _union_Cnst_Float_c{
int tag;struct _dyneither_ptr val;};struct _union_Cnst_String_c{int tag;struct
_dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};
union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c
Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct
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
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(
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
_tmp86C;struct Cyc_Position_Exit_struct*_tmp86B;(int)_throw((void*)((_tmp86B=
_cycalloc_atomic(sizeof(*_tmp86B)),((_tmp86B[0]=((_tmp86C.tag=Cyc_Position_Exit,
_tmp86C)),_tmp86B)))));};}static void Cyc_Parse_unimp(struct _dyneither_ptr msg,
struct Cyc_Position_Segment*sg){{const char*_tmp871;void*_tmp870[2];struct Cyc_String_pa_struct
_tmp86F;struct Cyc_String_pa_struct _tmp86E;(_tmp86E.tag=0,((_tmp86E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp86F.tag=0,((_tmp86F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(sg)),((
_tmp870[0]=& _tmp86F,((_tmp870[1]=& _tmp86E,Cyc_fprintf(Cyc_stderr,((_tmp871="%s: Warning: Cyclone does not yet support %s\n",
_tag_dyneither(_tmp871,sizeof(char),46))),_tag_dyneither(_tmp870,sizeof(void*),2)))))))))))));}
return;}static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,struct Cyc_Position_Segment*
loc){struct _tuple8 _tmp7;struct Cyc_Absyn_Tqual _tmp8;void*_tmp9;struct _tuple8*
_tmp6=tqt;_tmp7=*_tmp6;_tmp8=_tmp7.f2;_tmp9=_tmp7.f3;if((_tmp8.print_const  || 
_tmp8.q_volatile) || _tmp8.q_restrict){if(_tmp8.loc != 0)loc=_tmp8.loc;{const char*
_tmp874;void*_tmp873;(_tmp873=0,Cyc_Tcutil_warn(loc,((_tmp874="qualifier on type is ignored",
_tag_dyneither(_tmp874,sizeof(char),29))),_tag_dyneither(_tmp873,sizeof(void*),0)));};}
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
_tmp877;struct Cyc_Absyn_Upper_b_struct*_tmp876;bound=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp876=_cycalloc(sizeof(*_tmp876)),((
_tmp876[0]=((_tmp877.tag=1,((_tmp877.f1=_tmp14,_tmp877)))),_tmp876)))));}goto
_LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_struct*_tmp15=(struct Cyc_Absyn_Region_ptrqual_struct*)
_tmpC;if(_tmp15->tag != 1)goto _LL0;else{_tmp16=(void*)_tmp15->f1;}}_LL10: rgn=
_tmp16;goto _LL0;_LL0:;}{struct _tuple11 _tmp878;return(_tmp878.f1=nullable,((
_tmp878.f2=bound,((_tmp878.f3=zeroterm,((_tmp878.f4=rgn,_tmp878)))))));};}struct
_tuple1*Cyc_Parse_gensym_enum(){static int enum_counter=0;struct _dyneither_ptr*
_tmp885;const char*_tmp884;void*_tmp883[1];struct Cyc_Int_pa_struct _tmp882;struct
_tuple1*_tmp881;return(_tmp881=_cycalloc(sizeof(*_tmp881)),((_tmp881->f1=Cyc_Absyn_Rel_n(
0),((_tmp881->f2=((_tmp885=_cycalloc(sizeof(*_tmp885)),((_tmp885[0]=(struct
_dyneither_ptr)((_tmp882.tag=1,((_tmp882.f1=(unsigned long)enum_counter ++,((
_tmp883[0]=& _tmp882,Cyc_aprintf(((_tmp884="__anonymous_enum_%d__",_tag_dyneither(
_tmp884,sizeof(char),22))),_tag_dyneither(_tmp883,sizeof(void*),1)))))))),
_tmp885)))),_tmp881)))));}struct _tuple12{struct _tuple1*f1;struct Cyc_Absyn_Tqual
f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple13{struct
_tuple12*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple13*field_info){struct _tuple13 _tmp20;
struct _tuple12*_tmp21;struct _tuple12 _tmp22;struct _tuple1*_tmp23;struct Cyc_Absyn_Tqual
_tmp24;void*_tmp25;struct Cyc_List_List*_tmp26;struct Cyc_List_List*_tmp27;struct
Cyc_Absyn_Exp*_tmp28;struct _tuple13*_tmp1F=field_info;_tmp20=*_tmp1F;_tmp21=
_tmp20.f1;_tmp22=*_tmp21;_tmp23=_tmp22.f1;_tmp24=_tmp22.f2;_tmp25=_tmp22.f3;
_tmp26=_tmp22.f4;_tmp27=_tmp22.f5;_tmp28=_tmp20.f2;if(_tmp26 != 0){const char*
_tmp886;Cyc_Parse_err(((_tmp886="bad type params in struct field",_tag_dyneither(
_tmp886,sizeof(char),32))),loc);}if(Cyc_Absyn_is_qvar_qualified(_tmp23)){const
char*_tmp887;Cyc_Parse_err(((_tmp887="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp887,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp888;return(_tmp888=_cycalloc(sizeof(*_tmp888)),((_tmp888->name=(*_tmp23).f2,((
_tmp888->tq=_tmp24,((_tmp888->type=_tmp25,((_tmp888->width=_tmp28,((_tmp888->attributes=
_tmp27,_tmp888)))))))))));};}static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*
loc){struct Cyc_Parse_Type_spec_struct _tmp88B;struct Cyc_Parse_Type_spec_struct*
_tmp88A;return(void*)((_tmp88A=_cycalloc(sizeof(*_tmp88A)),((_tmp88A[0]=((
_tmp88B.tag=4,((_tmp88B.f1=(void*)t,((_tmp88B.f2=loc,_tmp88B)))))),_tmp88A))));}
static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp2E=t;struct Cyc_Absyn_ArrayInfo
_tmp30;void*_tmp31;struct Cyc_Absyn_Tqual _tmp32;struct Cyc_Absyn_Exp*_tmp33;union
Cyc_Absyn_Constraint*_tmp34;struct Cyc_Position_Segment*_tmp35;_LL12: {struct Cyc_Absyn_ArrayType_struct*
_tmp2F=(struct Cyc_Absyn_ArrayType_struct*)_tmp2E;if(_tmp2F->tag != 9)goto _LL14;
else{_tmp30=_tmp2F->f1;_tmp31=_tmp30.elt_type;_tmp32=_tmp30.tq;_tmp33=_tmp30.num_elts;
_tmp34=_tmp30.zero_term;_tmp35=_tmp30.zt_loc;}}_LL13: {struct Cyc_Absyn_Upper_b_struct
_tmp88E;struct Cyc_Absyn_Upper_b_struct*_tmp88D;return Cyc_Absyn_starb_typ(_tmp31,
argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,
_tmp32,_tmp33 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((void*)((_tmp88D=
_cycalloc(sizeof(*_tmp88D)),((_tmp88D[0]=((_tmp88E.tag=1,((_tmp88E.f1=(struct Cyc_Absyn_Exp*)
_tmp33,_tmp88E)))),_tmp88D))))),_tmp34);}_LL14:;_LL15: return t;_LL11:;}struct
_tuple14{struct _dyneither_ptr*f1;void*f2;};static struct Cyc_List_List*Cyc_Parse_get_arg_tags(
struct Cyc_List_List*x){struct Cyc_List_List*_tmp38=0;for(0;x != 0;x=x->tl){struct
_tuple8*_tmp39=(struct _tuple8*)x->hd;struct _tuple8 _tmp3A;struct Cyc_Core_Opt*
_tmp3B;struct Cyc_Core_Opt _tmp3C;struct _dyneither_ptr*_tmp3D;void*_tmp3E;void*
_tmp40;struct _tuple8 _tmp41;struct Cyc_Core_Opt*_tmp42;struct Cyc_Core_Opt _tmp43;
struct _dyneither_ptr*_tmp44;void*_tmp45;void*_tmp47;struct Cyc_Core_Opt*_tmp49;
struct Cyc_Core_Opt**_tmp4A;_LL17: _tmp3A=*_tmp39;_tmp3B=_tmp3A.f1;if(_tmp3B == 0)
goto _LL19;_tmp3C=*_tmp3B;_tmp3D=(struct _dyneither_ptr*)_tmp3C.v;_tmp3E=_tmp3A.f3;{
struct Cyc_Absyn_TagType_struct*_tmp3F=(struct Cyc_Absyn_TagType_struct*)_tmp3E;
if(_tmp3F->tag != 20)goto _LL19;else{_tmp40=(void*)_tmp3F->f1;}};_LL18:{void*
_tmp4B=_tmp40;struct Cyc_Core_Opt*_tmp4D;struct Cyc_Core_Opt**_tmp4E;_LL1E: {
struct Cyc_Absyn_Evar_struct*_tmp4C=(struct Cyc_Absyn_Evar_struct*)_tmp4B;if(
_tmp4C->tag != 1)goto _LL20;else{_tmp4D=_tmp4C->f2;_tmp4E=(struct Cyc_Core_Opt**)&
_tmp4C->f2;}}_LL1F: {struct Cyc_String_pa_struct _tmp896;void*_tmp895[1];const char*
_tmp894;struct _dyneither_ptr*_tmp893;struct _dyneither_ptr*nm=(_tmp893=_cycalloc(
sizeof(*_tmp893)),((_tmp893[0]=(struct _dyneither_ptr)((_tmp896.tag=0,((_tmp896.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3D),((_tmp895[0]=& _tmp896,Cyc_aprintf(((
_tmp894="`%s",_tag_dyneither(_tmp894,sizeof(char),4))),_tag_dyneither(_tmp895,
sizeof(void*),1)))))))),_tmp893)));{struct Cyc_Absyn_VarType_struct*_tmp8AB;
struct Cyc_Absyn_Tvar*_tmp8AA;struct Cyc_Absyn_Eq_kb_struct _tmp8A9;struct Cyc_Absyn_Eq_kb_struct*
_tmp8A8;struct Cyc_Absyn_VarType_struct _tmp8A7;struct Cyc_Core_Opt*_tmp8A6;*_tmp4E=((
_tmp8A6=_cycalloc(sizeof(*_tmp8A6)),((_tmp8A6->v=(void*)((_tmp8AB=_cycalloc(
sizeof(*_tmp8AB)),((_tmp8AB[0]=((_tmp8A7.tag=2,((_tmp8A7.f1=((_tmp8AA=_cycalloc(
sizeof(*_tmp8AA)),((_tmp8AA->name=nm,((_tmp8AA->identity=- 1,((_tmp8AA->kind=(
void*)((_tmp8A8=_cycalloc(sizeof(*_tmp8A8)),((_tmp8A8[0]=((_tmp8A9.tag=0,((
_tmp8A9.f1=& Cyc_Tcutil_ik,_tmp8A9)))),_tmp8A8)))),_tmp8AA)))))))),_tmp8A7)))),
_tmp8AB)))),_tmp8A6))));}goto _LL1D;}_LL20:;_LL21: goto _LL1D;_LL1D:;}{struct
_tuple14*_tmp8AE;struct Cyc_List_List*_tmp8AD;_tmp38=((_tmp8AD=_cycalloc(sizeof(*
_tmp8AD)),((_tmp8AD->hd=((_tmp8AE=_cycalloc(sizeof(*_tmp8AE)),((_tmp8AE->f1=
_tmp3D,((_tmp8AE->f2=_tmp40,_tmp8AE)))))),((_tmp8AD->tl=_tmp38,_tmp8AD))))));}
goto _LL16;_LL19: _tmp41=*_tmp39;_tmp42=_tmp41.f1;if(_tmp42 == 0)goto _LL1B;_tmp43=*
_tmp42;_tmp44=(struct _dyneither_ptr*)_tmp43.v;_tmp45=_tmp41.f3;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp46=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp45;if(_tmp46->tag != 16)goto
_LL1B;else{_tmp47=(void*)_tmp46->f1;{struct Cyc_Absyn_Evar_struct*_tmp48=(struct
Cyc_Absyn_Evar_struct*)_tmp47;if(_tmp48->tag != 1)goto _LL1B;else{_tmp49=_tmp48->f2;
_tmp4A=(struct Cyc_Core_Opt**)& _tmp48->f2;}};}};_LL1A: {struct Cyc_String_pa_struct
_tmp8B6;void*_tmp8B5[1];const char*_tmp8B4;struct _dyneither_ptr*_tmp8B3;struct
_dyneither_ptr*nm=(_tmp8B3=_cycalloc(sizeof(*_tmp8B3)),((_tmp8B3[0]=(struct
_dyneither_ptr)((_tmp8B6.tag=0,((_tmp8B6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp44),((_tmp8B5[0]=& _tmp8B6,Cyc_aprintf(((_tmp8B4="`%s",
_tag_dyneither(_tmp8B4,sizeof(char),4))),_tag_dyneither(_tmp8B5,sizeof(void*),1)))))))),
_tmp8B3)));{struct Cyc_Absyn_VarType_struct*_tmp8CB;struct Cyc_Absyn_Tvar*_tmp8CA;
struct Cyc_Absyn_Eq_kb_struct _tmp8C9;struct Cyc_Absyn_Eq_kb_struct*_tmp8C8;struct
Cyc_Absyn_VarType_struct _tmp8C7;struct Cyc_Core_Opt*_tmp8C6;*_tmp4A=((_tmp8C6=
_cycalloc(sizeof(*_tmp8C6)),((_tmp8C6->v=(void*)((_tmp8CB=_cycalloc(sizeof(*
_tmp8CB)),((_tmp8CB[0]=((_tmp8C7.tag=2,((_tmp8C7.f1=((_tmp8CA=_cycalloc(sizeof(*
_tmp8CA)),((_tmp8CA->name=nm,((_tmp8CA->identity=- 1,((_tmp8CA->kind=(void*)((
_tmp8C8=_cycalloc(sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8C9.tag=0,((_tmp8C9.f1=&
Cyc_Tcutil_rk,_tmp8C9)))),_tmp8C8)))),_tmp8CA)))))))),_tmp8C7)))),_tmp8CB)))),
_tmp8C6))));}goto _LL16;}_LL1B:;_LL1C: goto _LL16;_LL16:;}return _tmp38;}static
struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){struct
Cyc_List_List*_tmp65=0;for(0;x != 0;x=x->tl){void*_tmp66=((struct Cyc_Absyn_Aggrfield*)
x->hd)->type;void*_tmp68;_LL23: {struct Cyc_Absyn_TagType_struct*_tmp67=(struct
Cyc_Absyn_TagType_struct*)_tmp66;if(_tmp67->tag != 20)goto _LL25;else{_tmp68=(void*)
_tmp67->f1;}}_LL24:{struct _tuple14*_tmp8CE;struct Cyc_List_List*_tmp8CD;_tmp65=((
_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD->hd=((_tmp8CE=_cycalloc(sizeof(*
_tmp8CE)),((_tmp8CE->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8CE->f2=
_tmp68,_tmp8CE)))))),((_tmp8CD->tl=_tmp65,_tmp8CD))))));}goto _LL22;_LL25:;_LL26:
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
_tmp78=tags;for(0;_tmp78 != 0;_tmp78=_tmp78->tl){struct _tuple14 _tmp7A;struct
_dyneither_ptr*_tmp7B;void*_tmp7C;struct _tuple14*_tmp79=(struct _tuple14*)_tmp78->hd;
_tmp7A=*_tmp79;_tmp7B=_tmp7A.f1;_tmp7C=_tmp7A.f2;if(Cyc_strptrcmp(_tmp7B,_tmp77)
== 0){struct Cyc_Absyn_Valueof_e_struct _tmp8D1;struct Cyc_Absyn_Valueof_e_struct*
_tmp8D0;return Cyc_Absyn_new_exp((void*)((_tmp8D0=_cycalloc(sizeof(*_tmp8D0)),((
_tmp8D0[0]=((_tmp8D1.tag=39,((_tmp8D1.f1=(void*)Cyc_Tcutil_copy_type(_tmp7C),
_tmp8D1)))),_tmp8D0)))),e->loc);}}}goto _LL27;_LL2C:;_LL2D: goto _LL27;_LL27:;}
return e;}static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){{
void*_tmp7F=t;struct Cyc_Absyn_ArrayInfo _tmp81;void*_tmp82;struct Cyc_Absyn_Tqual
_tmp83;struct Cyc_Absyn_Exp*_tmp84;union Cyc_Absyn_Constraint*_tmp85;struct Cyc_Position_Segment*
_tmp86;struct Cyc_Absyn_PtrInfo _tmp88;void*_tmp89;struct Cyc_Absyn_Tqual _tmp8A;
struct Cyc_Absyn_PtrAtts _tmp8B;void*_tmp8C;union Cyc_Absyn_Constraint*_tmp8D;union
Cyc_Absyn_Constraint*_tmp8E;union Cyc_Absyn_Constraint*_tmp8F;struct Cyc_Absyn_PtrLoc*
_tmp90;_LL2F: {struct Cyc_Absyn_ArrayType_struct*_tmp80=(struct Cyc_Absyn_ArrayType_struct*)
_tmp7F;if(_tmp80->tag != 9)goto _LL31;else{_tmp81=_tmp80->f1;_tmp82=_tmp81.elt_type;
_tmp83=_tmp81.tq;_tmp84=_tmp81.num_elts;_tmp85=_tmp81.zero_term;_tmp86=_tmp81.zt_loc;}}
_LL30: {struct Cyc_Absyn_Exp*nelts2=_tmp84;if(_tmp84 != 0)nelts2=(struct Cyc_Absyn_Exp*)
Cyc_Parse_substitute_tags_exp(tags,(struct Cyc_Absyn_Exp*)_tmp84);{void*_tmp91=
Cyc_Parse_substitute_tags(tags,_tmp82);if(_tmp84 != nelts2  || _tmp82 != _tmp91){
struct Cyc_Absyn_ArrayType_struct _tmp8D7;struct Cyc_Absyn_ArrayInfo _tmp8D6;struct
Cyc_Absyn_ArrayType_struct*_tmp8D5;return(void*)((_tmp8D5=_cycalloc(sizeof(*
_tmp8D5)),((_tmp8D5[0]=((_tmp8D7.tag=9,((_tmp8D7.f1=((_tmp8D6.elt_type=_tmp91,((
_tmp8D6.tq=_tmp83,((_tmp8D6.num_elts=nelts2,((_tmp8D6.zero_term=_tmp85,((_tmp8D6.zt_loc=
_tmp86,_tmp8D6)))))))))),_tmp8D7)))),_tmp8D5))));}goto _LL2E;};}_LL31: {struct Cyc_Absyn_PointerType_struct*
_tmp87=(struct Cyc_Absyn_PointerType_struct*)_tmp7F;if(_tmp87->tag != 5)goto _LL33;
else{_tmp88=_tmp87->f1;_tmp89=_tmp88.elt_typ;_tmp8A=_tmp88.elt_tq;_tmp8B=_tmp88.ptr_atts;
_tmp8C=_tmp8B.rgn;_tmp8D=_tmp8B.nullable;_tmp8E=_tmp8B.bounds;_tmp8F=_tmp8B.zero_term;
_tmp90=_tmp8B.ptrloc;}}_LL32: {void*_tmp95=Cyc_Parse_substitute_tags(tags,_tmp89);
union Cyc_Absyn_Constraint*_tmp96=_tmp8E;{union Cyc_Absyn_Constraint _tmp97=*_tmp8E;
void*_tmp98;struct Cyc_Absyn_Exp*_tmp9A;_LL36: if((_tmp97.Eq_constr).tag != 1)goto
_LL38;_tmp98=(void*)(_tmp97.Eq_constr).val;{struct Cyc_Absyn_Upper_b_struct*
_tmp99=(struct Cyc_Absyn_Upper_b_struct*)_tmp98;if(_tmp99->tag != 1)goto _LL38;
else{_tmp9A=_tmp99->f1;}};_LL37: _tmp9A=Cyc_Parse_substitute_tags_exp(tags,_tmp9A);{
struct Cyc_Absyn_Upper_b_struct*_tmp8DD;struct Cyc_Absyn_Upper_b_struct _tmp8DC;
union Cyc_Absyn_Constraint*_tmp8DB;_tmp96=((_tmp8DB=_cycalloc(sizeof(*_tmp8DB)),(((
_tmp8DB->Eq_constr).val=(void*)((_tmp8DD=_cycalloc(sizeof(*_tmp8DD)),((_tmp8DD[0]=((
_tmp8DC.tag=1,((_tmp8DC.f1=_tmp9A,_tmp8DC)))),_tmp8DD)))),(((_tmp8DB->Eq_constr).tag=
1,_tmp8DB))))));}goto _LL35;_LL38:;_LL39: goto _LL35;_LL35:;}if(_tmp95 != _tmp89  || 
_tmp96 != _tmp8E){struct Cyc_Absyn_PointerType_struct _tmp8E7;struct Cyc_Absyn_PtrAtts
_tmp8E6;struct Cyc_Absyn_PtrInfo _tmp8E5;struct Cyc_Absyn_PointerType_struct*
_tmp8E4;return(void*)((_tmp8E4=_cycalloc(sizeof(*_tmp8E4)),((_tmp8E4[0]=((
_tmp8E7.tag=5,((_tmp8E7.f1=((_tmp8E5.elt_typ=_tmp95,((_tmp8E5.elt_tq=_tmp8A,((
_tmp8E5.ptr_atts=((_tmp8E6.rgn=_tmp8C,((_tmp8E6.nullable=_tmp8D,((_tmp8E6.bounds=
_tmp96,((_tmp8E6.zero_term=_tmp8F,((_tmp8E6.ptrloc=_tmp90,_tmp8E6)))))))))),
_tmp8E5)))))),_tmp8E7)))),_tmp8E4))));}goto _LL2E;}_LL33:;_LL34: goto _LL2E;_LL2E:;}
return t;}static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,
struct Cyc_Absyn_Aggrfield*x){x->type=Cyc_Parse_substitute_tags(tags,x->type);}
struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple15*Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment*loc,struct _tuple8*t){struct _tuple15*_tmp8E8;return(
_tmp8E8=_cycalloc(sizeof(*_tmp8E8)),((_tmp8E8->f1=(*t).f2,((_tmp8E8->f2=(*t).f3,
_tmp8E8)))));}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*
d){void*_tmpA3=d->r;struct Cyc_Absyn_Vardecl*_tmpA5;_LL3B: {struct Cyc_Absyn_Var_d_struct*
_tmpA4=(struct Cyc_Absyn_Var_d_struct*)_tmpA3;if(_tmpA4->tag != 0)goto _LL3D;else{
_tmpA5=_tmpA4->f1;}}_LL3C: return _tmpA5;_LL3D:;_LL3E: {const char*_tmp8EB;void*
_tmp8EA;(_tmp8EA=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((_tmp8EB="bad declaration in `forarray' statement",
_tag_dyneither(_tmp8EB,sizeof(char),40))),_tag_dyneither(_tmp8EA,sizeof(void*),0)));}
_LL3A:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmpA8=tm;_LL40: {struct
Cyc_Absyn_TypeParams_mod_struct*_tmpA9=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpA8;if(_tmpA9->tag != 4)goto _LL42;}_LL41: return 1;_LL42:;_LL43: return 0;_LL3F:;}
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){const char*_tmp8EC;if(
Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8EC="`H",_tag_dyneither(_tmp8EC,
sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_HeapRgn_val;else{const char*
_tmp8ED;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8ED="`U",_tag_dyneither(
_tmp8ED,sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_UniqueRgn_val;else{const
char*_tmp8EE;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8EE="`RC",
_tag_dyneither(_tmp8EE,sizeof(char),4))))== 0)return(void*)& Cyc_Absyn_RefCntRgn_val;
else{struct Cyc_Absyn_VarType_struct _tmp8F8;struct _dyneither_ptr*_tmp8F7;struct
Cyc_Absyn_Tvar*_tmp8F6;struct Cyc_Absyn_VarType_struct*_tmp8F5;return(void*)((
_tmp8F5=_cycalloc(sizeof(*_tmp8F5)),((_tmp8F5[0]=((_tmp8F8.tag=2,((_tmp8F8.f1=((
_tmp8F6=_cycalloc(sizeof(*_tmp8F6)),((_tmp8F6->name=((_tmp8F7=_cycalloc(sizeof(*
_tmp8F7)),((_tmp8F7[0]=s,_tmp8F7)))),((_tmp8F6->identity=- 1,((_tmp8F6->kind=k,
_tmp8F6)))))))),_tmp8F8)))),_tmp8F5))));}}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar*t){void*k;{void*_tmpB1=Cyc_Absyn_compress_kb(t->kind);
struct Cyc_Absyn_Kind*_tmpB4;_LL45: {struct Cyc_Absyn_Unknown_kb_struct*_tmpB2=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpB1;if(_tmpB2->tag != 1)goto _LL47;}_LL46:{
struct Cyc_Absyn_Unknown_kb_struct _tmp8FB;struct Cyc_Absyn_Unknown_kb_struct*
_tmp8FA;k=(void*)((_tmp8FA=_cycalloc(sizeof(*_tmp8FA)),((_tmp8FA[0]=((_tmp8FB.tag=
1,((_tmp8FB.f1=0,_tmp8FB)))),_tmp8FA))));}goto _LL44;_LL47: {struct Cyc_Absyn_Less_kb_struct*
_tmpB3=(struct Cyc_Absyn_Less_kb_struct*)_tmpB1;if(_tmpB3->tag != 2)goto _LL49;
else{_tmpB4=_tmpB3->f2;}}_LL48:{struct Cyc_Absyn_Less_kb_struct _tmp8FE;struct Cyc_Absyn_Less_kb_struct*
_tmp8FD;k=(void*)((_tmp8FD=_cycalloc(sizeof(*_tmp8FD)),((_tmp8FD[0]=((_tmp8FE.tag=
2,((_tmp8FE.f1=0,((_tmp8FE.f2=_tmpB4,_tmp8FE)))))),_tmp8FD))));}goto _LL44;_LL49:;
_LL4A: k=_tmpB1;goto _LL44;_LL44:;}{struct Cyc_Absyn_Tvar*_tmp8FF;return(_tmp8FF=
_cycalloc(sizeof(*_tmp8FF)),((_tmp8FF->name=t->name,((_tmp8FF->identity=- 1,((
_tmp8FF->kind=k,_tmp8FF)))))));};}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(
struct Cyc_Position_Segment*loc,void*t){void*_tmpBA=t;struct Cyc_Absyn_Tvar*_tmpBC;
_LL4C: {struct Cyc_Absyn_VarType_struct*_tmpBB=(struct Cyc_Absyn_VarType_struct*)
_tmpBA;if(_tmpBB->tag != 2)goto _LL4E;else{_tmpBC=_tmpBB->f1;}}_LL4D: return _tmpBC;
_LL4E:;_LL4F: {const char*_tmp902;void*_tmp901;(_tmp901=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp902="expecting a list of type variables, not types",_tag_dyneither(_tmp902,
sizeof(char),46))),_tag_dyneither(_tmp901,sizeof(void*),0)));}_LL4B:;}static void*
Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct
_tmp905;struct Cyc_Absyn_VarType_struct*_tmp904;return(void*)((_tmp904=_cycalloc(
sizeof(*_tmp904)),((_tmp904[0]=((_tmp905.tag=2,((_tmp905.f1=pr,_tmp905)))),
_tmp904))));}static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,
int leq){void*_tmpC1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpC3;struct Cyc_Absyn_Tvar
_tmpC4;void*_tmpC5;void**_tmpC6;_LL51: {struct Cyc_Absyn_VarType_struct*_tmpC2=(
struct Cyc_Absyn_VarType_struct*)_tmpC1;if(_tmpC2->tag != 2)goto _LL53;else{_tmpC3=
_tmpC2->f1;_tmpC4=*_tmpC3;_tmpC5=_tmpC4.kind;_tmpC6=(void**)&(*_tmpC2->f1).kind;}}
_LL52: {void*_tmpC7=Cyc_Absyn_compress_kb(*_tmpC6);_LL56: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpC8=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC7;if(_tmpC8->tag != 1)goto _LL58;}
_LL57: if(!leq)*_tmpC6=Cyc_Tcutil_kind_to_bound(k);else{struct Cyc_Absyn_Less_kb_struct
_tmp908;struct Cyc_Absyn_Less_kb_struct*_tmp907;*_tmpC6=(void*)((_tmp907=
_cycalloc(sizeof(*_tmp907)),((_tmp907[0]=((_tmp908.tag=2,((_tmp908.f1=0,((
_tmp908.f2=k,_tmp908)))))),_tmp907))));}return;_LL58:;_LL59: return;_LL55:;}_LL53:;
_LL54: return;_LL50:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmpCB=(void*)tms->hd;void*_tmpCD;
_LL5B: {struct Cyc_Absyn_Function_mod_struct*_tmpCC=(struct Cyc_Absyn_Function_mod_struct*)
_tmpCB;if(_tmpCC->tag != 3)goto _LL5D;else{_tmpCD=(void*)_tmpCC->f1;}}_LL5C: if(tms->tl
== 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)
 && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){void*_tmpCE=_tmpCD;
struct Cyc_List_List*_tmpD1;_LL60: {struct Cyc_Absyn_WithTypes_struct*_tmpCF=(
struct Cyc_Absyn_WithTypes_struct*)_tmpCE;if(_tmpCF->tag != 1)goto _LL62;}_LL61:{
const char*_tmp90B;void*_tmp90A;(_tmp90A=0,Cyc_Tcutil_warn(loc,((_tmp90B="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp90B,sizeof(char),93))),_tag_dyneither(_tmp90A,sizeof(void*),0)));}
return tms;_LL62: {struct Cyc_Absyn_NoTypes_struct*_tmpD0=(struct Cyc_Absyn_NoTypes_struct*)
_tmpCE;if(_tmpD0->tag != 0)goto _LL5F;else{_tmpD1=_tmpD0->f1;}}_LL63: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpD1)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds)){const char*_tmp90E;void*_tmp90D;(_tmp90D=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
loc,((_tmp90E="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp90E,sizeof(char),73))),_tag_dyneither(_tmp90D,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpD6=0;for(0;_tmpD1 != 0;_tmpD1=_tmpD1->tl){struct Cyc_List_List*
_tmpD7=tds;for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){struct Cyc_Absyn_Decl*_tmpD8=(
struct Cyc_Absyn_Decl*)_tmpD7->hd;void*_tmpD9=_tmpD8->r;struct Cyc_Absyn_Vardecl*
_tmpDB;_LL65: {struct Cyc_Absyn_Var_d_struct*_tmpDA=(struct Cyc_Absyn_Var_d_struct*)
_tmpD9;if(_tmpDA->tag != 0)goto _LL67;else{_tmpDB=_tmpDA->f1;}}_LL66: if(Cyc_zstrptrcmp((*
_tmpDB->name).f2,(struct _dyneither_ptr*)_tmpD1->hd)!= 0)continue;if(_tmpDB->initializer
!= 0){const char*_tmp911;void*_tmp910;(_tmp910=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD8->loc,((
_tmp911="initializer found in parameter declaration",_tag_dyneither(_tmp911,
sizeof(char),43))),_tag_dyneither(_tmp910,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmpDB->name)){const char*_tmp914;void*_tmp913;(_tmp913=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD8->loc,((
_tmp914="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp914,
sizeof(char),47))),_tag_dyneither(_tmp913,sizeof(void*),0)));}{struct _tuple8*
_tmp91A;struct Cyc_Core_Opt*_tmp919;struct Cyc_List_List*_tmp918;_tmpD6=((_tmp918=
_cycalloc(sizeof(*_tmp918)),((_tmp918->hd=((_tmp91A=_cycalloc(sizeof(*_tmp91A)),((
_tmp91A->f1=((_tmp919=_cycalloc(sizeof(*_tmp919)),((_tmp919->v=(*_tmpDB->name).f2,
_tmp919)))),((_tmp91A->f2=_tmpDB->tq,((_tmp91A->f3=_tmpDB->type,_tmp91A)))))))),((
_tmp918->tl=_tmpD6,_tmp918))))));}goto L;_LL67:;_LL68: {const char*_tmp91D;void*
_tmp91C;(_tmp91C=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD8->loc,((_tmp91D="nonvariable declaration in parameter type",
_tag_dyneither(_tmp91D,sizeof(char),42))),_tag_dyneither(_tmp91C,sizeof(void*),0)));}
_LL64:;}L: if(_tmpD7 == 0){const char*_tmp921;void*_tmp920[1];struct Cyc_String_pa_struct
_tmp91F;(_tmp91F.tag=0,((_tmp91F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmpD1->hd)),((_tmp920[0]=& _tmp91F,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp921="%s is not given a type",_tag_dyneither(_tmp921,sizeof(char),23))),
_tag_dyneither(_tmp920,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp930;struct Cyc_Absyn_WithTypes_struct*_tmp92F;struct Cyc_Absyn_WithTypes_struct
_tmp92E;struct Cyc_Absyn_Function_mod_struct _tmp92D;struct Cyc_List_List*_tmp92C;
return(_tmp92C=_cycalloc(sizeof(*_tmp92C)),((_tmp92C->hd=(void*)((_tmp930=
_cycalloc(sizeof(*_tmp930)),((_tmp930[0]=((_tmp92D.tag=3,((_tmp92D.f1=(void*)((
void*)((_tmp92F=_cycalloc(sizeof(*_tmp92F)),((_tmp92F[0]=((_tmp92E.tag=1,((
_tmp92E.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpD6),((_tmp92E.f2=0,((_tmp92E.f3=0,((_tmp92E.f4=0,((_tmp92E.f5=0,_tmp92E)))))))))))),
_tmp92F))))),_tmp92D)))),_tmp930)))),((_tmp92C->tl=0,_tmp92C)))));};};_LL5F:;}
goto _LL5E;_LL5D:;_LL5E: {struct Cyc_List_List*_tmp931;return(_tmp931=_cycalloc(
sizeof(*_tmp931)),((_tmp931->hd=(void*)tms->hd,((_tmp931->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp931)))));}_LL5A:;};}struct _tuple16{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){if(tds != 0){struct Cyc_Parse_Declarator*
_tmp932;d=((_tmp932=_cycalloc(sizeof(*_tmp932)),((_tmp932->id=d->id,((_tmp932->tms=
Cyc_Parse_oldstyle2newstyle(d->tms,tds,loc),_tmp932))))));}{enum Cyc_Absyn_Scope
sc=Cyc_Absyn_Public;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(
0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0)
switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v){case Cyc_Parse_Extern_sc: _LL69: sc=
Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL6A: sc=Cyc_Absyn_Static;break;
default: _LL6B:{const char*_tmp933;Cyc_Parse_err(((_tmp933="bad storage class on function",
_tag_dyneither(_tmp933,sizeof(char),30))),loc);}break;}}{void*_tmpF0=Cyc_Parse_collapse_type_specifiers(
tss,loc);struct Cyc_Absyn_Tqual _tmpF2;void*_tmpF3;struct Cyc_List_List*_tmpF4;
struct Cyc_List_List*_tmpF5;struct _tuple10 _tmpF1=Cyc_Parse_apply_tms(tq,_tmpF0,
atts,d->tms);_tmpF2=_tmpF1.f1;_tmpF3=_tmpF1.f2;_tmpF4=_tmpF1.f3;_tmpF5=_tmpF1.f4;
if(_tmpF4 != 0){const char*_tmp936;void*_tmp935;(_tmp935=0,Cyc_Tcutil_warn(loc,((
_tmp936="bad type params, ignoring",_tag_dyneither(_tmp936,sizeof(char),26))),
_tag_dyneither(_tmp935,sizeof(void*),0)));}{void*_tmpF8=_tmpF3;struct Cyc_Absyn_FnInfo
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
_tmp107.f3;if(_tmp108 == 0){{const char*_tmp937;Cyc_Parse_err(((_tmp937="missing argument variable in function prototype",
_tag_dyneither(_tmp937,sizeof(char),48))),loc);}{struct _tuple16*_tmp93A;struct
Cyc_List_List*_tmp939;_tmp104=((_tmp939=_cycalloc(sizeof(*_tmp939)),((_tmp939->hd=((
_tmp93A=_cycalloc(sizeof(*_tmp93A)),((_tmp93A->f1=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp93A->f2=_tmp109,((
_tmp93A->f3=_tmp10A,_tmp93A)))))))),((_tmp939->tl=_tmp104,_tmp939))))));};}else{
struct _tuple16*_tmp93D;struct Cyc_List_List*_tmp93C;_tmp104=((_tmp93C=_cycalloc(
sizeof(*_tmp93C)),((_tmp93C->hd=((_tmp93D=_cycalloc(sizeof(*_tmp93D)),((_tmp93D->f1=(
struct _dyneither_ptr*)_tmp108->v,((_tmp93D->f2=_tmp109,((_tmp93D->f3=_tmp10A,
_tmp93D)))))))),((_tmp93C->tl=_tmp104,_tmp93C))))));}}}{struct Cyc_Absyn_Fndecl*
_tmp93E;return(_tmp93E=_cycalloc(sizeof(*_tmp93E)),((_tmp93E->sc=sc,((_tmp93E->name=
d->id,((_tmp93E->tvs=_tmpFB,((_tmp93E->is_inline=is_inline,((_tmp93E->effect=
_tmpFC,((_tmp93E->ret_tqual=_tmpFD,((_tmp93E->ret_type=_tmpFE,((_tmp93E->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp104),((
_tmp93E->c_varargs=_tmp100,((_tmp93E->cyc_varargs=_tmp101,((_tmp93E->rgn_po=
_tmp102,((_tmp93E->body=body,((_tmp93E->cached_typ=0,((_tmp93E->param_vardecls=0,((
_tmp93E->fn_vardecl=0,((_tmp93E->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp103,_tmpF5),_tmp93E)))))))))))))))))))))))))))))))));};}
_LL70:;_LL71: {const char*_tmp941;void*_tmp940;(_tmp940=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp941="declarator is not a function prototype",_tag_dyneither(_tmp941,sizeof(
char),39))),_tag_dyneither(_tmp940,sizeof(void*),0)));}_LL6D:;};};};}static char
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
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp11D);if(seen_type){const char*_tmp942;
Cyc_Parse_err(((_tmp942="signed qualifier must come before type",_tag_dyneither(
_tmp942,sizeof(char),39))),_tmp11D);}last_loc=_tmp11D;seen_sign=1;sgn=Cyc_Absyn_Signed;
goto _LL72;_LL77: {struct Cyc_Parse_Unsigned_spec_struct*_tmp11E=(struct Cyc_Parse_Unsigned_spec_struct*)
_tmp118;if(_tmp11E->tag != 1)goto _LL79;else{_tmp11F=_tmp11E->f1;}}_LL78: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp11F);if(seen_type){const char*_tmp943;
Cyc_Parse_err(((_tmp943="signed qualifier must come before type",_tag_dyneither(
_tmp943,sizeof(char),39))),_tmp11F);}last_loc=_tmp11F;seen_sign=1;sgn=Cyc_Absyn_Unsigned;
goto _LL72;_LL79: {struct Cyc_Parse_Short_spec_struct*_tmp120=(struct Cyc_Parse_Short_spec_struct*)
_tmp118;if(_tmp120->tag != 2)goto _LL7B;else{_tmp121=_tmp120->f1;}}_LL7A: if(
seen_size){const char*_tmp944;Cyc_Parse_err(((_tmp944="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp944,sizeof(char),59))),_tmp121);}if(seen_type){const char*
_tmp945;Cyc_Parse_err(((_tmp945="short modifier must come before base type",
_tag_dyneither(_tmp945,sizeof(char),42))),_tmp121);}last_loc=_tmp121;seen_size=1;
sz=Cyc_Absyn_Short_sz;goto _LL72;_LL7B: {struct Cyc_Parse_Long_spec_struct*_tmp122=(
struct Cyc_Parse_Long_spec_struct*)_tmp118;if(_tmp122->tag != 3)goto _LL72;else{
_tmp123=_tmp122->f1;}}_LL7C: if(seen_type){const char*_tmp946;Cyc_Parse_err(((
_tmp946="long modifier must come before base type",_tag_dyneither(_tmp946,
sizeof(char),41))),_tmp123);}if(seen_size)switch(sz){case Cyc_Absyn_Long_sz: _LL7D:
sz=Cyc_Absyn_LongLong_sz;break;default: _LL7E:{const char*_tmp947;Cyc_Parse_err(((
_tmp947="extra long in type specifier",_tag_dyneither(_tmp947,sizeof(char),29))),
_tmp123);}break;}else{sz=Cyc_Absyn_Long_sz;}last_loc=_tmp123;seen_size=1;goto
_LL72;_LL72:;}if(!seen_type){if(!seen_sign  && !seen_size){const char*_tmp94A;void*
_tmp949;(_tmp949=0,Cyc_Tcutil_warn(last_loc,((_tmp94A="missing type within specifier",
_tag_dyneither(_tmp94A,sizeof(char),30))),_tag_dyneither(_tmp949,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{if(seen_sign){void*_tmp12C=t;enum Cyc_Absyn_Sign
_tmp12E;enum Cyc_Absyn_Size_of _tmp12F;_LL81: {struct Cyc_Absyn_IntType_struct*
_tmp12D=(struct Cyc_Absyn_IntType_struct*)_tmp12C;if(_tmp12D->tag != 6)goto _LL83;
else{_tmp12E=_tmp12D->f1;_tmp12F=_tmp12D->f2;}}_LL82: if(_tmp12E != sgn)t=Cyc_Absyn_int_typ(
sgn,_tmp12F);goto _LL80;_LL83:;_LL84:{const char*_tmp94B;Cyc_Parse_err(((_tmp94B="sign specification on non-integral type",
_tag_dyneither(_tmp94B,sizeof(char),40))),last_loc);}goto _LL80;_LL80:;}if(
seen_size){void*_tmp131=t;enum Cyc_Absyn_Sign _tmp133;enum Cyc_Absyn_Size_of
_tmp134;_LL86: {struct Cyc_Absyn_IntType_struct*_tmp132=(struct Cyc_Absyn_IntType_struct*)
_tmp131;if(_tmp132->tag != 6)goto _LL88;else{_tmp133=_tmp132->f1;_tmp134=_tmp132->f2;}}
_LL87: if(_tmp134 != sz)t=Cyc_Absyn_int_typ(_tmp133,sz);goto _LL85;_LL88: {struct
Cyc_Absyn_DoubleType_struct*_tmp135=(struct Cyc_Absyn_DoubleType_struct*)_tmp131;
if(_tmp135->tag != 8)goto _LL8A;}_LL89: t=Cyc_Absyn_double_typ(1);goto _LL85;_LL8A:;
_LL8B:{const char*_tmp94C;Cyc_Parse_err(((_tmp94C="size qualifier on non-integral type",
_tag_dyneither(_tmp94C,sizeof(char),36))),last_loc);}goto _LL85;_LL85:;}}return t;}
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)
return 0;{struct Cyc_Parse_Declarator*_tmp137=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmp138=_tmp137->id;struct Cyc_Absyn_Tqual _tmp13A;void*_tmp13B;
struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13D;struct _tuple10 _tmp139=
Cyc_Parse_apply_tms(tq,t,shared_atts,_tmp137->tms);_tmp13A=_tmp139.f1;_tmp13B=
_tmp139.f2;_tmp13C=_tmp139.f3;_tmp13D=_tmp139.f4;if(ds->tl == 0){struct _tuple12*
_tmp94F;struct Cyc_List_List*_tmp94E;return(_tmp94E=_region_malloc(r,sizeof(*
_tmp94E)),((_tmp94E->hd=((_tmp94F=_region_malloc(r,sizeof(*_tmp94F)),((_tmp94F->f1=
_tmp138,((_tmp94F->f2=_tmp13A,((_tmp94F->f3=_tmp13B,((_tmp94F->f4=_tmp13C,((
_tmp94F->f5=_tmp13D,_tmp94F)))))))))))),((_tmp94E->tl=0,_tmp94E)))));}else{
struct _tuple12*_tmp952;struct Cyc_List_List*_tmp951;return(_tmp951=_region_malloc(
r,sizeof(*_tmp951)),((_tmp951->hd=((_tmp952=_region_malloc(r,sizeof(*_tmp952)),((
_tmp952->f1=_tmp138,((_tmp952->f2=_tmp13A,((_tmp952->f3=_tmp13B,((_tmp952->f4=
_tmp13C,((_tmp952->f5=_tmp13D,_tmp952)))))))))))),((_tmp951->tl=Cyc_Parse_apply_tmss(
r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp951)))));}};}static struct
_tuple10 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0){struct _tuple10 _tmp953;return(_tmp953.f1=
tq,((_tmp953.f2=t,((_tmp953.f3=0,((_tmp953.f4=atts,_tmp953)))))));}{void*_tmp143=(
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
struct Cyc_List_List*_tmp954;fn_atts=((_tmp954=_cycalloc(sizeof(*_tmp954)),((
_tmp954->hd=(void*)as->hd,((_tmp954->tl=fn_atts,_tmp954))))));}else{struct Cyc_List_List*
_tmp955;new_atts=((_tmp955=_cycalloc(sizeof(*_tmp955)),((_tmp955->hd=(void*)as->hd,((
_tmp955->tl=new_atts,_tmp955))))));}}}if(tms->tl != 0){void*_tmp161=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;struct Cyc_List_List*_tmp163;_LL9F: {
struct Cyc_Absyn_TypeParams_mod_struct*_tmp162=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp161;if(_tmp162->tag != 4)goto _LLA1;else{_tmp163=_tmp162->f1;}}_LLA0: typvars=
_tmp163;tms=tms->tl;goto _LL9E;_LLA1:;_LLA2: goto _LL9E;_LL9E:;}if(((((!_tmp159  && 
_tmp15A == 0) && _tmp158 != 0) && _tmp158->tl == 0) && (*((struct _tuple8*)_tmp158->hd)).f1
== 0) && (*((struct _tuple8*)_tmp158->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
_tmp158=0;{struct Cyc_List_List*_tmp164=Cyc_Parse_get_arg_tags(_tmp158);if(
_tmp164 != 0)t=Cyc_Parse_substitute_tags(_tmp164,t);t=Cyc_Parse_array2ptr(t,0);{
struct Cyc_List_List*_tmp165=_tmp158;for(0;_tmp165 != 0;_tmp165=_tmp165->tl){
struct _tuple8 _tmp167;struct Cyc_Core_Opt*_tmp168;struct Cyc_Absyn_Tqual _tmp169;
void*_tmp16A;void**_tmp16B;struct _tuple8*_tmp166=(struct _tuple8*)_tmp165->hd;
_tmp167=*_tmp166;_tmp168=_tmp167.f1;_tmp169=_tmp167.f2;_tmp16A=_tmp167.f3;
_tmp16B=(void**)&(*_tmp166).f3;if(_tmp164 != 0)*_tmp16B=Cyc_Parse_substitute_tags(
_tmp164,*_tmp16B);*_tmp16B=Cyc_Parse_array2ptr(*_tmp16B,1);}}return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(tq.loc),Cyc_Absyn_function_typ(typvars,_tmp15B,tq,t,
_tmp158,_tmp159,_tmp15A,_tmp15C,fn_atts),new_atts,((struct Cyc_List_List*)
_check_null(tms))->tl);};}_LL9C: {struct Cyc_Absyn_NoTypes_struct*_tmp15D=(struct
Cyc_Absyn_NoTypes_struct*)_tmp156;if(_tmp15D->tag != 0)goto _LL99;else{_tmp15E=
_tmp15D->f2;}}_LL9D: {const char*_tmp958;void*_tmp957;(_tmp957=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
_tmp15E,((_tmp958="function declaration without parameter types",_tag_dyneither(
_tmp958,sizeof(char),45))),_tag_dyneither(_tmp957,sizeof(void*),0)));}_LL99:;}
_LL93: {struct Cyc_Absyn_TypeParams_mod_struct*_tmp14D=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp143;if(_tmp14D->tag != 4)goto _LL95;else{_tmp14E=_tmp14D->f1;_tmp14F=_tmp14D->f2;}}
_LL94: if(tms->tl == 0){struct _tuple10 _tmp959;return(_tmp959.f1=tq,((_tmp959.f2=t,((
_tmp959.f3=_tmp14E,((_tmp959.f4=atts,_tmp959)))))));}{const char*_tmp95C;void*
_tmp95B;(_tmp95B=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp14F,((_tmp95C="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp95C,sizeof(char),68))),_tag_dyneither(_tmp95B,sizeof(void*),0)));};
_LL95: {struct Cyc_Absyn_Pointer_mod_struct*_tmp150=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp143;if(_tmp150->tag != 2)goto _LL97;else{_tmp151=_tmp150->f1;_tmp152=_tmp150->f2;}}
_LL96: {struct Cyc_Absyn_PointerType_struct _tmp962;struct Cyc_Absyn_PtrInfo _tmp961;
struct Cyc_Absyn_PointerType_struct*_tmp960;return Cyc_Parse_apply_tms(_tmp152,(
void*)((_tmp960=_cycalloc(sizeof(*_tmp960)),((_tmp960[0]=((_tmp962.tag=5,((
_tmp962.f1=((_tmp961.elt_typ=t,((_tmp961.elt_tq=tq,((_tmp961.ptr_atts=_tmp151,
_tmp961)))))),_tmp962)))),_tmp960)))),atts,tms->tl);}_LL97: {struct Cyc_Absyn_Attributes_mod_struct*
_tmp153=(struct Cyc_Absyn_Attributes_mod_struct*)_tmp143;if(_tmp153->tag != 5)goto
_LL8C;else{_tmp154=_tmp153->f1;_tmp155=_tmp153->f2;}}_LL98: return Cyc_Parse_apply_tms(
tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
atts,_tmp155),tms->tl);_LL8C:;};}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*
tss,struct Cyc_Position_Segment*loc){return Cyc_Parse_collapse_type_specifiers(tss,
loc);}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple12*t){struct _tuple12 _tmp175;struct _tuple1*_tmp176;struct Cyc_Absyn_Tqual
_tmp177;void*_tmp178;struct Cyc_List_List*_tmp179;struct Cyc_List_List*_tmp17A;
struct _tuple12*_tmp174=t;_tmp175=*_tmp174;_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;
_tmp178=_tmp175.f3;_tmp179=_tmp175.f4;_tmp17A=_tmp175.f5;Cyc_Lex_register_typedef(
_tmp176);{struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*type;{void*_tmp17B=_tmp178;
struct Cyc_Core_Opt*_tmp17D;_LLA4: {struct Cyc_Absyn_Evar_struct*_tmp17C=(struct
Cyc_Absyn_Evar_struct*)_tmp17B;if(_tmp17C->tag != 1)goto _LLA6;else{_tmp17D=
_tmp17C->f1;}}_LLA5: type=0;if(_tmp17D == 0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;
else{kind=_tmp17D;}goto _LLA3;_LLA6:;_LLA7: kind=0;{struct Cyc_Core_Opt*_tmp963;
type=((_tmp963=_cycalloc(sizeof(*_tmp963)),((_tmp963->v=_tmp178,_tmp963))));}
goto _LLA3;_LLA3:;}{struct Cyc_Absyn_Typedef_d_struct _tmp969;struct Cyc_Absyn_Typedefdecl*
_tmp968;struct Cyc_Absyn_Typedef_d_struct*_tmp967;return Cyc_Absyn_new_decl((void*)((
_tmp967=_cycalloc(sizeof(*_tmp967)),((_tmp967[0]=((_tmp969.tag=9,((_tmp969.f1=((
_tmp968=_cycalloc(sizeof(*_tmp968)),((_tmp968->name=_tmp176,((_tmp968->tvs=
_tmp179,((_tmp968->kind=kind,((_tmp968->defn=type,((_tmp968->atts=_tmp17A,((
_tmp968->tq=_tmp177,_tmp968)))))))))))))),_tmp969)))),_tmp967)))),loc);};};}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct
Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Decl_s_struct _tmp96C;struct Cyc_Absyn_Decl_s_struct*
_tmp96B;return Cyc_Absyn_new_stmt((void*)((_tmp96B=_cycalloc(sizeof(*_tmp96B)),((
_tmp96B[0]=((_tmp96C.tag=12,((_tmp96C.f1=d,((_tmp96C.f2=s,_tmp96C)))))),_tmp96B)))),
d->loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*
ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*
f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*
accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*
Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*
ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*loc){struct
_RegionHandle _tmp184=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp184;
_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp186;struct Cyc_Absyn_Tqual
_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_List_List*_tmp189;struct Cyc_Parse_Declaration_spec*
_tmp185=ds;_tmp186=*_tmp185;_tmp187=_tmp186.tq;_tmp188=_tmp186.type_specs;
_tmp189=_tmp186.attributes;if(_tmp187.loc == 0)_tmp187.loc=tqual_loc;if(ds->is_inline){
const char*_tmp96D;Cyc_Parse_err(((_tmp96D="inline is allowed only on function definitions",
_tag_dyneither(_tmp96D,sizeof(char),47))),loc);}if(_tmp188 == 0){{const char*
_tmp970;void*_tmp96F;(_tmp96F=0,Cyc_Tcutil_warn(loc,((_tmp970="missing type specifiers in declaration, assuming int",
_tag_dyneither(_tmp970,sizeof(char),53))),_tag_dyneither(_tmp96F,sizeof(void*),0)));}{
void*_tmp971[1];_tmp188=((_tmp971[0]=Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,loc),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp971,sizeof(void*),1))));};}{enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;int
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
_tmp189);{struct Cyc_Absyn_Aggr_d_struct*_tmp977;struct Cyc_Absyn_Aggr_d_struct
_tmp976;struct Cyc_List_List*_tmp975;struct Cyc_List_List*_tmp1BE=(_tmp975=
_cycalloc(sizeof(*_tmp975)),((_tmp975->hd=Cyc_Absyn_new_decl((void*)((_tmp977=
_cycalloc(sizeof(*_tmp977)),((_tmp977[0]=((_tmp976.tag=6,((_tmp976.f1=_tmp198,
_tmp976)))),_tmp977)))),loc),((_tmp975->tl=0,_tmp975)))));_npop_handler(0);
return _tmp1BE;};_LLB3: {struct Cyc_Absyn_TypeDeclType_struct*_tmp199=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp192;if(_tmp199->tag != 27)goto _LLB5;else{_tmp19A=_tmp199->f1;_tmp19B=*_tmp19A;
_tmp19C=_tmp19B.r;{struct Cyc_Absyn_Enum_td_struct*_tmp19D=(struct Cyc_Absyn_Enum_td_struct*)
_tmp19C;if(_tmp19D->tag != 1)goto _LLB5;else{_tmp19E=_tmp19D->f1;}};}}_LLB4: if(
_tmp189 != 0){const char*_tmp978;Cyc_Parse_err(((_tmp978="attributes on enum not supported",
_tag_dyneither(_tmp978,sizeof(char),33))),loc);}{struct Cyc_Absyn_Enum_d_struct*
_tmp97E;struct Cyc_Absyn_Enum_d_struct _tmp97D;struct Cyc_List_List*_tmp97C;struct
Cyc_List_List*_tmp1C3=(_tmp97C=_cycalloc(sizeof(*_tmp97C)),((_tmp97C->hd=Cyc_Absyn_new_decl((
void*)((_tmp97E=_cycalloc(sizeof(*_tmp97E)),((_tmp97E[0]=((_tmp97D.tag=8,((
_tmp97D.f1=_tmp19E,_tmp97D)))),_tmp97E)))),loc),((_tmp97C->tl=0,_tmp97C)))));
_npop_handler(0);return _tmp1C3;};_LLB5: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp19F=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp192;if(_tmp19F->tag != 27)goto
_LLB7;else{_tmp1A0=_tmp19F->f1;_tmp1A1=*_tmp1A0;_tmp1A2=_tmp1A1.r;{struct Cyc_Absyn_Datatype_td_struct*
_tmp1A3=(struct Cyc_Absyn_Datatype_td_struct*)_tmp1A2;if(_tmp1A3->tag != 2)goto
_LLB7;else{_tmp1A4=_tmp1A3->f1;}};}}_LLB6: if(_tmp189 != 0){const char*_tmp97F;Cyc_Parse_err(((
_tmp97F="attributes on datatypes not supported",_tag_dyneither(_tmp97F,sizeof(
char),38))),loc);}{struct Cyc_Absyn_Datatype_d_struct*_tmp985;struct Cyc_Absyn_Datatype_d_struct
_tmp984;struct Cyc_List_List*_tmp983;struct Cyc_List_List*_tmp1C8=(_tmp983=
_cycalloc(sizeof(*_tmp983)),((_tmp983->hd=Cyc_Absyn_new_decl((void*)((_tmp985=
_cycalloc(sizeof(*_tmp985)),((_tmp985[0]=((_tmp984.tag=7,((_tmp984.f1=_tmp1A4,
_tmp984)))),_tmp985)))),loc),((_tmp983->tl=0,_tmp983)))));_npop_handler(0);
return _tmp1C8;};_LLB7: {struct Cyc_Absyn_AggrType_struct*_tmp1A5=(struct Cyc_Absyn_AggrType_struct*)
_tmp192;if(_tmp1A5->tag != 12)goto _LLB9;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A6.aggr_info;
if((_tmp1A7.UnknownAggr).tag != 1)goto _LLB9;_tmp1A8=(struct _tuple3)(_tmp1A7.UnknownAggr).val;
_tmp1A9=_tmp1A8.f1;_tmp1AA=_tmp1A8.f2;_tmp1AB=_tmp1A6.targs;}}_LLB8: {struct Cyc_List_List*
_tmp1C9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp1AB);struct Cyc_Absyn_Aggrdecl*_tmp986;struct Cyc_Absyn_Aggrdecl*_tmp1CA=(
_tmp986=_cycalloc(sizeof(*_tmp986)),((_tmp986->kind=_tmp1A9,((_tmp986->sc=s,((
_tmp986->name=_tmp1AA,((_tmp986->tvs=_tmp1C9,((_tmp986->impl=0,((_tmp986->attributes=
0,_tmp986)))))))))))));if(_tmp189 != 0){const char*_tmp987;Cyc_Parse_err(((_tmp987="bad attributes on type declaration",
_tag_dyneither(_tmp987,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp98D;struct Cyc_Absyn_Aggr_d_struct _tmp98C;struct Cyc_List_List*_tmp98B;struct
Cyc_List_List*_tmp1CF=(_tmp98B=_cycalloc(sizeof(*_tmp98B)),((_tmp98B->hd=Cyc_Absyn_new_decl((
void*)((_tmp98D=_cycalloc(sizeof(*_tmp98D)),((_tmp98D[0]=((_tmp98C.tag=6,((
_tmp98C.f1=_tmp1CA,_tmp98C)))),_tmp98D)))),loc),((_tmp98B->tl=0,_tmp98B)))));
_npop_handler(0);return _tmp1CF;};}_LLB9: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1AC=(struct Cyc_Absyn_DatatypeType_struct*)_tmp192;if(_tmp1AC->tag != 3)goto
_LLBB;else{_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AD.datatype_info;if((_tmp1AE.KnownDatatype).tag
!= 2)goto _LLBB;_tmp1AF=(struct Cyc_Absyn_Datatypedecl**)(_tmp1AE.KnownDatatype).val;}}
_LLBA: if(_tmp189 != 0){const char*_tmp98E;Cyc_Parse_err(((_tmp98E="bad attributes on datatype",
_tag_dyneither(_tmp98E,sizeof(char),27))),loc);}{struct Cyc_Absyn_Datatype_d_struct*
_tmp994;struct Cyc_Absyn_Datatype_d_struct _tmp993;struct Cyc_List_List*_tmp992;
struct Cyc_List_List*_tmp1D5=(_tmp992=_cycalloc(sizeof(*_tmp992)),((_tmp992->hd=
Cyc_Absyn_new_decl((void*)((_tmp994=_cycalloc(sizeof(*_tmp994)),((_tmp994[0]=((
_tmp993.tag=7,((_tmp993.f1=*_tmp1AF,_tmp993)))),_tmp994)))),loc),((_tmp992->tl=0,
_tmp992)))));_npop_handler(0);return _tmp1D5;};_LLBB: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1B0=(struct Cyc_Absyn_DatatypeType_struct*)_tmp192;if(_tmp1B0->tag != 3)goto
_LLBD;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B1.datatype_info;if((_tmp1B2.UnknownDatatype).tag
!= 1)goto _LLBD;_tmp1B3=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B2.UnknownDatatype).val;
_tmp1B4=_tmp1B3.name;_tmp1B5=_tmp1B3.is_extensible;_tmp1B6=_tmp1B1.targs;}}_LLBC: {
struct Cyc_List_List*_tmp1D6=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp1B6);struct Cyc_Absyn_Decl*_tmp1D7=
Cyc_Absyn_datatype_decl(s,_tmp1B4,_tmp1D6,0,_tmp1B5,loc);if(_tmp189 != 0){const
char*_tmp995;Cyc_Parse_err(((_tmp995="bad attributes on datatype",_tag_dyneither(
_tmp995,sizeof(char),27))),loc);}{struct Cyc_List_List*_tmp996;struct Cyc_List_List*
_tmp1DA=(_tmp996=_cycalloc(sizeof(*_tmp996)),((_tmp996->hd=_tmp1D7,((_tmp996->tl=
0,_tmp996)))));_npop_handler(0);return _tmp1DA;};}_LLBD: {struct Cyc_Absyn_EnumType_struct*
_tmp1B7=(struct Cyc_Absyn_EnumType_struct*)_tmp192;if(_tmp1B7->tag != 14)goto _LLBF;
else{_tmp1B8=_tmp1B7->f1;}}_LLBE: {struct Cyc_Absyn_Enumdecl*_tmp997;struct Cyc_Absyn_Enumdecl*
_tmp1DB=(_tmp997=_cycalloc(sizeof(*_tmp997)),((_tmp997->sc=s,((_tmp997->name=
_tmp1B8,((_tmp997->fields=0,_tmp997)))))));if(_tmp189 != 0){const char*_tmp998;Cyc_Parse_err(((
_tmp998="bad attributes on enum",_tag_dyneither(_tmp998,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp9A2;struct Cyc_Absyn_Enum_d_struct _tmp9A1;struct Cyc_Absyn_Enum_d_struct*
_tmp9A0;struct Cyc_List_List*_tmp99F;struct Cyc_List_List*_tmp1E1=(_tmp99F=
_cycalloc(sizeof(*_tmp99F)),((_tmp99F->hd=((_tmp9A2=_cycalloc(sizeof(*_tmp9A2)),((
_tmp9A2->r=(void*)((_tmp9A0=_cycalloc(sizeof(*_tmp9A0)),((_tmp9A0[0]=((_tmp9A1.tag=
8,((_tmp9A1.f1=_tmp1DB,_tmp9A1)))),_tmp9A0)))),((_tmp9A2->loc=loc,_tmp9A2)))))),((
_tmp99F->tl=0,_tmp99F)))));_npop_handler(0);return _tmp1E1;};}_LLBF: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1B9=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp192;if(_tmp1B9->tag != 15)goto
_LLC1;else{_tmp1BA=_tmp1B9->f1;}}_LLC0: {struct Cyc_Core_Opt*_tmp9A5;struct Cyc_Absyn_Enumdecl*
_tmp9A4;struct Cyc_Absyn_Enumdecl*_tmp1E3=(_tmp9A4=_cycalloc(sizeof(*_tmp9A4)),((
_tmp9A4->sc=s,((_tmp9A4->name=Cyc_Parse_gensym_enum(),((_tmp9A4->fields=((
_tmp9A5=_cycalloc(sizeof(*_tmp9A5)),((_tmp9A5->v=_tmp1BA,_tmp9A5)))),_tmp9A4)))))));
if(_tmp189 != 0){const char*_tmp9A6;Cyc_Parse_err(((_tmp9A6="bad attributes on enum",
_tag_dyneither(_tmp9A6,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp9B0;
struct Cyc_Absyn_Enum_d_struct _tmp9AF;struct Cyc_Absyn_Enum_d_struct*_tmp9AE;
struct Cyc_List_List*_tmp9AD;struct Cyc_List_List*_tmp1E9=(_tmp9AD=_cycalloc(
sizeof(*_tmp9AD)),((_tmp9AD->hd=((_tmp9B0=_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0->r=(
void*)((_tmp9AE=_cycalloc(sizeof(*_tmp9AE)),((_tmp9AE[0]=((_tmp9AF.tag=8,((
_tmp9AF.f1=_tmp1E3,_tmp9AF)))),_tmp9AE)))),((_tmp9B0->loc=loc,_tmp9B0)))))),((
_tmp9AD->tl=0,_tmp9AD)))));_npop_handler(0);return _tmp1E9;};}_LLC1:;_LLC2:{const
char*_tmp9B1;Cyc_Parse_err(((_tmp9B1="missing declarator",_tag_dyneither(_tmp9B1,
sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1ED=0;_npop_handler(0);return
_tmp1ED;};_LLB0:;}else{struct Cyc_List_List*_tmp1EE=Cyc_Parse_apply_tmss(mkrgn,
_tmp187,_tmp191,_tmp18F,_tmp189);if(istypedef){if(!exps_empty){const char*_tmp9B2;
Cyc_Parse_err(((_tmp9B2="initializer in typedef declaration",_tag_dyneither(
_tmp9B2,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp1EE);struct Cyc_List_List*_tmp1F0=decls;_npop_handler(0);return _tmp1F0;};}
else{struct Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp1F1=_tmp1EE;for(0;
_tmp1F1 != 0;(_tmp1F1=_tmp1F1->tl,_tmp190=_tmp190->tl)){struct _tuple12 _tmp1F3;
struct _tuple1*_tmp1F4;struct Cyc_Absyn_Tqual _tmp1F5;void*_tmp1F6;struct Cyc_List_List*
_tmp1F7;struct Cyc_List_List*_tmp1F8;struct _tuple12*_tmp1F2=(struct _tuple12*)
_tmp1F1->hd;_tmp1F3=*_tmp1F2;_tmp1F4=_tmp1F3.f1;_tmp1F5=_tmp1F3.f2;_tmp1F6=
_tmp1F3.f3;_tmp1F7=_tmp1F3.f4;_tmp1F8=_tmp1F3.f5;if(_tmp1F7 != 0){const char*
_tmp9B5;void*_tmp9B4;(_tmp9B4=0,Cyc_Tcutil_warn(loc,((_tmp9B5="bad type params, ignoring",
_tag_dyneither(_tmp9B5,sizeof(char),26))),_tag_dyneither(_tmp9B4,sizeof(void*),0)));}
if(_tmp190 == 0){const char*_tmp9B8;void*_tmp9B7;(_tmp9B7=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp9B8="unexpected NULL in parse!",_tag_dyneither(_tmp9B8,sizeof(char),26))),
_tag_dyneither(_tmp9B7,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*_tmp1FD=(struct
Cyc_Absyn_Exp*)_tmp190->hd;struct Cyc_Absyn_Vardecl*_tmp1FE=Cyc_Absyn_new_vardecl(
_tmp1F4,_tmp1F6,_tmp1FD);_tmp1FE->tq=_tmp1F5;_tmp1FE->sc=s;_tmp1FE->attributes=
_tmp1F8;{struct Cyc_Absyn_Var_d_struct*_tmp9BE;struct Cyc_Absyn_Var_d_struct
_tmp9BD;struct Cyc_Absyn_Decl*_tmp9BC;struct Cyc_Absyn_Decl*_tmp1FF=(_tmp9BC=
_cycalloc(sizeof(*_tmp9BC)),((_tmp9BC->r=(void*)((_tmp9BE=_cycalloc(sizeof(*
_tmp9BE)),((_tmp9BE[0]=((_tmp9BD.tag=0,((_tmp9BD.f1=_tmp1FE,_tmp9BD)))),_tmp9BE)))),((
_tmp9BC->loc=loc,_tmp9BC)))));struct Cyc_List_List*_tmp9BF;decls=((_tmp9BF=
_cycalloc(sizeof(*_tmp9BF)),((_tmp9BF->hd=_tmp1FF,((_tmp9BF->tl=decls,_tmp9BF))))));};};}}{
struct Cyc_List_List*_tmp204=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
decls);_npop_handler(0);return _tmp204;};}}};};};};};_pop_region(mkrgn);}static
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
_LLD6: break;}break;default: _LLD1: break;}{const char*_tmp9C4;void*_tmp9C3[2];
struct Cyc_String_pa_struct _tmp9C2;struct Cyc_Int_pa_struct _tmp9C1;Cyc_Parse_err((
struct _dyneither_ptr)((_tmp9C1.tag=1,((_tmp9C1.f1=(unsigned long)((int)Cyc_strlen((
struct _dyneither_ptr)s)),((_tmp9C2.tag=0,((_tmp9C2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s),((_tmp9C3[0]=& _tmp9C2,((_tmp9C3[1]=& _tmp9C1,Cyc_aprintf(((
_tmp9C4="bad kind: %s; strlen=%d",_tag_dyneither(_tmp9C4,sizeof(char),24))),
_tag_dyneither(_tmp9C3,sizeof(void*),2)))))))))))))),loc);}return& Cyc_Tcutil_bk;}
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*_tmp209=
p->r;struct _tuple1*_tmp20B;struct Cyc_Absyn_Vardecl*_tmp20D;struct Cyc_Absyn_Pat*
_tmp20E;struct Cyc_Absyn_Pat _tmp20F;void*_tmp210;struct Cyc_Absyn_Pat*_tmp213;
enum Cyc_Absyn_Sign _tmp216;int _tmp217;char _tmp219;struct _dyneither_ptr _tmp21B;
struct _tuple1*_tmp21D;struct Cyc_List_List*_tmp21E;int _tmp21F;struct Cyc_Absyn_Exp*
_tmp221;_LLDA: {struct Cyc_Absyn_UnknownId_p_struct*_tmp20A=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp209;if(_tmp20A->tag != 14)goto _LLDC;else{_tmp20B=_tmp20A->f1;}}_LLDB: {struct
Cyc_Absyn_UnknownId_e_struct _tmp9C7;struct Cyc_Absyn_UnknownId_e_struct*_tmp9C6;
return Cyc_Absyn_new_exp((void*)((_tmp9C6=_cycalloc(sizeof(*_tmp9C6)),((_tmp9C6[0]=((
_tmp9C7.tag=2,((_tmp9C7.f1=_tmp20B,_tmp9C7)))),_tmp9C6)))),p->loc);}_LLDC: {
struct Cyc_Absyn_Reference_p_struct*_tmp20C=(struct Cyc_Absyn_Reference_p_struct*)
_tmp209;if(_tmp20C->tag != 2)goto _LLDE;else{_tmp20D=_tmp20C->f1;_tmp20E=_tmp20C->f2;
_tmp20F=*_tmp20E;_tmp210=_tmp20F.r;{struct Cyc_Absyn_Wild_p_struct*_tmp211=(
struct Cyc_Absyn_Wild_p_struct*)_tmp210;if(_tmp211->tag != 0)goto _LLDE;};}}_LLDD: {
struct Cyc_Absyn_UnknownId_e_struct _tmp9CA;struct Cyc_Absyn_UnknownId_e_struct*
_tmp9C9;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmp9C9=_cycalloc(
sizeof(*_tmp9C9)),((_tmp9C9[0]=((_tmp9CA.tag=2,((_tmp9CA.f1=_tmp20D->name,
_tmp9CA)))),_tmp9C9)))),p->loc),p->loc);}_LLDE: {struct Cyc_Absyn_Pointer_p_struct*
_tmp212=(struct Cyc_Absyn_Pointer_p_struct*)_tmp209;if(_tmp212->tag != 5)goto _LLE0;
else{_tmp213=_tmp212->f1;}}_LLDF: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(
_tmp213),p->loc);_LLE0: {struct Cyc_Absyn_Null_p_struct*_tmp214=(struct Cyc_Absyn_Null_p_struct*)
_tmp209;if(_tmp214->tag != 8)goto _LLE2;}_LLE1: return Cyc_Absyn_null_exp(p->loc);
_LLE2: {struct Cyc_Absyn_Int_p_struct*_tmp215=(struct Cyc_Absyn_Int_p_struct*)
_tmp209;if(_tmp215->tag != 9)goto _LLE4;else{_tmp216=_tmp215->f1;_tmp217=_tmp215->f2;}}
_LLE3: return Cyc_Absyn_int_exp(_tmp216,_tmp217,p->loc);_LLE4: {struct Cyc_Absyn_Char_p_struct*
_tmp218=(struct Cyc_Absyn_Char_p_struct*)_tmp209;if(_tmp218->tag != 10)goto _LLE6;
else{_tmp219=_tmp218->f1;}}_LLE5: return Cyc_Absyn_char_exp(_tmp219,p->loc);_LLE6: {
struct Cyc_Absyn_Float_p_struct*_tmp21A=(struct Cyc_Absyn_Float_p_struct*)_tmp209;
if(_tmp21A->tag != 11)goto _LLE8;else{_tmp21B=_tmp21A->f1;}}_LLE7: return Cyc_Absyn_float_exp(
_tmp21B,p->loc);_LLE8: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp21C=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp209;if(_tmp21C->tag != 15)goto _LLEA;else{_tmp21D=_tmp21C->f1;_tmp21E=_tmp21C->f2;
_tmp21F=_tmp21C->f3;if(_tmp21F != 0)goto _LLEA;}}_LLE9: {struct Cyc_Absyn_UnknownId_e_struct
_tmp9CD;struct Cyc_Absyn_UnknownId_e_struct*_tmp9CC;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmp9CC=_cycalloc(sizeof(*_tmp9CC)),((_tmp9CC[0]=((_tmp9CD.tag=2,((
_tmp9CD.f1=_tmp21D,_tmp9CD)))),_tmp9CC)))),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp21E);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLEA: {struct Cyc_Absyn_Exp_p_struct*_tmp220=(struct Cyc_Absyn_Exp_p_struct*)
_tmp209;if(_tmp220->tag != 16)goto _LLEC;else{_tmp221=_tmp220->f1;}}_LLEB: return
_tmp221;_LLEC:;_LLED:{const char*_tmp9CE;Cyc_Parse_err(((_tmp9CE="cannot mix patterns and expressions in case",
_tag_dyneither(_tmp9CE,sizeof(char),44))),p->loc);}return Cyc_Absyn_null_exp(p->loc);
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
yyzzz;}union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){union Cyc_YYSTYPE _tmp9CF;
return((_tmp9CF.Int_tok).val=yy1,(((_tmp9CF.Int_tok).tag=1,_tmp9CF)));}static
char _tmp22F[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp22F,_tmp22F,_tmp22F + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE
yy1){char yyzzz;{union Cyc_YYSTYPE _tmp230=yy1;char _tmp231;_LLF4: if((_tmp230.Char_tok).tag
!= 2)goto _LLF6;_tmp231=(char)(_tmp230.Char_tok).val;_LLF5: yyzzz=_tmp231;goto
_LLF3;_LLF6:;_LLF7:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLF3:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9D0;return((_tmp9D0.Char_tok).val=
yy1,(((_tmp9D0.Char_tok).tag=2,_tmp9D0)));}static char _tmp234[13]="string_t<`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{
_tmp234,_tmp234,_tmp234 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp235=yy1;struct _dyneither_ptr
_tmp236;_LLF9: if((_tmp235.String_tok).tag != 3)goto _LLFB;_tmp236=(struct
_dyneither_ptr)(_tmp235.String_tok).val;_LLFA: yyzzz=_tmp236;goto _LLF8;_LLFB:;
_LLFC:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLF8:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9D1;return((_tmp9D1.String_tok).val=
yy1,(((_tmp9D1.String_tok).tag=3,_tmp9D1)));}static char _tmp239[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp239,
_tmp239,_tmp239 + 56}};struct _tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct
_tuple18*yyzzz;{union Cyc_YYSTYPE _tmp23A=yy1;struct _tuple18*_tmp23B;_LLFE: if((
_tmp23A.YY1).tag != 7)goto _LL100;_tmp23B=(struct _tuple18*)(_tmp23A.YY1).val;_LLFF:
yyzzz=_tmp23B;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)& Cyc_yyfail_YY1);_LLFD:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY1(struct _tuple18*yy1){union Cyc_YYSTYPE _tmp9D2;
return((_tmp9D2.YY1).val=yy1,(((_tmp9D2.YY1).tag=7,_tmp9D2)));}static char _tmp23E[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp23E,_tmp23E,_tmp23E + 19}};union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE
yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp23F=yy1;union Cyc_Absyn_Constraint*
_tmp240;_LL103: if((_tmp23F.YY2).tag != 8)goto _LL105;_tmp240=(union Cyc_Absyn_Constraint*)(
_tmp23F.YY2).val;_LL104: yyzzz=_tmp240;goto _LL102;_LL105:;_LL106:(int)_throw((
void*)& Cyc_yyfail_YY2);_LL102:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*
yy1){union Cyc_YYSTYPE _tmp9D3;return((_tmp9D3.YY2).val=yy1,(((_tmp9D3.YY2).tag=8,
_tmp9D3)));}static char _tmp243[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE _tmp244=yy1;
struct Cyc_Absyn_Exp*_tmp245;_LL108: if((_tmp244.YY3).tag != 9)goto _LL10A;_tmp245=(
struct Cyc_Absyn_Exp*)(_tmp244.YY3).val;_LL109: yyzzz=_tmp245;goto _LL107;_LL10A:;
_LL10B:(int)_throw((void*)& Cyc_yyfail_YY3);_LL107:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY3(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9D4;return((_tmp9D4.YY3).val=
yy1,(((_tmp9D4.YY3).tag=9,_tmp9D4)));}static char _tmp248[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp248,_tmp248,
_tmp248 + 10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE _tmp249=yy1;struct Cyc_Absyn_Exp*_tmp24A;_LL10D: if((
_tmp249.YY4).tag != 10)goto _LL10F;_tmp24A=(struct Cyc_Absyn_Exp*)(_tmp249.YY4).val;
_LL10E: yyzzz=_tmp24A;goto _LL10C;_LL10F:;_LL110:(int)_throw((void*)& Cyc_yyfail_YY4);
_LL10C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE
_tmp9D5;return((_tmp9D5.YY4).val=yy1,(((_tmp9D5.YY4).tag=10,_tmp9D5)));}static
char _tmp24D[17]="list_t<exp_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={
Cyc_Core_Failure,{_tmp24D,_tmp24D,_tmp24D + 17}};struct Cyc_List_List*Cyc_yyget_YY5(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp24E=yy1;
struct Cyc_List_List*_tmp24F;_LL112: if((_tmp24E.YY5).tag != 11)goto _LL114;_tmp24F=(
struct Cyc_List_List*)(_tmp24E.YY5).val;_LL113: yyzzz=_tmp24F;goto _LL111;_LL114:;
_LL115:(int)_throw((void*)& Cyc_yyfail_YY5);_LL111:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D6;return((_tmp9D6.YY5).val=
yy1,(((_tmp9D6.YY5).tag=11,_tmp9D6)));}static char _tmp252[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp252,
_tmp252,_tmp252 + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp253=yy1;struct Cyc_List_List*
_tmp254;_LL117: if((_tmp253.YY6).tag != 12)goto _LL119;_tmp254=(struct Cyc_List_List*)(
_tmp253.YY6).val;_LL118: yyzzz=_tmp254;goto _LL116;_LL119:;_LL11A:(int)_throw((
void*)& Cyc_yyfail_YY6);_LL116:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9D7;return((_tmp9D7.YY6).val=yy1,(((_tmp9D7.YY6).tag=12,
_tmp9D7)));}static char _tmp257[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp257,_tmp257,_tmp257 + 9}};enum Cyc_Absyn_Primop
Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Primop yyzzz;{union Cyc_YYSTYPE
_tmp258=yy1;enum Cyc_Absyn_Primop _tmp259;_LL11C: if((_tmp258.YY7).tag != 13)goto
_LL11E;_tmp259=(enum Cyc_Absyn_Primop)(_tmp258.YY7).val;_LL11D: yyzzz=_tmp259;
goto _LL11B;_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY7);_LL11B:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9D8;
return((_tmp9D8.YY7).val=yy1,(((_tmp9D8.YY7).tag=13,_tmp9D8)));}static char
_tmp25C[19]="opt_t<primop_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={
Cyc_Core_Failure,{_tmp25C,_tmp25C,_tmp25C + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(
union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp25D=yy1;
struct Cyc_Core_Opt*_tmp25E;_LL121: if((_tmp25D.YY8).tag != 14)goto _LL123;_tmp25E=(
struct Cyc_Core_Opt*)(_tmp25D.YY8).val;_LL122: yyzzz=_tmp25E;goto _LL120;_LL123:;
_LL124:(int)_throw((void*)& Cyc_yyfail_YY8);_LL120:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY8(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9D9;return((_tmp9D9.YY8).val=
yy1,(((_tmp9D9.YY8).tag=14,_tmp9D9)));}static char _tmp261[7]="qvar_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp261,
_tmp261,_tmp261 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp262=yy1;struct _tuple1*_tmp263;_LL126:
if((_tmp262.QualId_tok).tag != 5)goto _LL128;_tmp263=(struct _tuple1*)(_tmp262.QualId_tok).val;
_LL127: yyzzz=_tmp263;goto _LL125;_LL128:;_LL129:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL125:;}return yyzzz;}union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple1*yy1){union Cyc_YYSTYPE
_tmp9DA;return((_tmp9DA.QualId_tok).val=yy1,(((_tmp9DA.QualId_tok).tag=5,_tmp9DA)));}
static char _tmp266[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY9={
Cyc_Core_Failure,{_tmp266,_tmp266,_tmp266 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE _tmp267=yy1;
struct Cyc_Absyn_Stmt*_tmp268;_LL12B: if((_tmp267.YY9).tag != 15)goto _LL12D;_tmp268=(
struct Cyc_Absyn_Stmt*)(_tmp267.YY9).val;_LL12C: yyzzz=_tmp268;goto _LL12A;_LL12D:;
_LL12E:(int)_throw((void*)& Cyc_yyfail_YY9);_LL12A:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9DB;return((_tmp9DB.YY9).val=
yy1,(((_tmp9DB.YY9).tag=15,_tmp9DB)));}static char _tmp26B[27]="list_t<switch_clause_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp26B,
_tmp26B,_tmp26B + 27}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp26C=yy1;struct Cyc_List_List*
_tmp26D;_LL130: if((_tmp26C.YY10).tag != 16)goto _LL132;_tmp26D=(struct Cyc_List_List*)(
_tmp26C.YY10).val;_LL131: yyzzz=_tmp26D;goto _LL12F;_LL132:;_LL133:(int)_throw((
void*)& Cyc_yyfail_YY10);_LL12F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9DC;return((_tmp9DC.YY10).val=yy1,(((_tmp9DC.YY10).tag=
16,_tmp9DC)));}static char _tmp270[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp270,_tmp270,_tmp270 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE
_tmp271=yy1;struct Cyc_Absyn_Pat*_tmp272;_LL135: if((_tmp271.YY11).tag != 17)goto
_LL137;_tmp272=(struct Cyc_Absyn_Pat*)(_tmp271.YY11).val;_LL136: yyzzz=_tmp272;
goto _LL134;_LL137:;_LL138:(int)_throw((void*)& Cyc_yyfail_YY11);_LL134:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9DD;
return((_tmp9DD.YY11).val=yy1,(((_tmp9DD.YY11).tag=17,_tmp9DD)));}static char
_tmp275[28]="$(list_t<pat_t,`H>,bool)@`H";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={
Cyc_Core_Failure,{_tmp275,_tmp275,_tmp275 + 28}};struct _tuple19*Cyc_yyget_YY12(
union Cyc_YYSTYPE yy1){struct _tuple19*yyzzz;{union Cyc_YYSTYPE _tmp276=yy1;struct
_tuple19*_tmp277;_LL13A: if((_tmp276.YY12).tag != 18)goto _LL13C;_tmp277=(struct
_tuple19*)(_tmp276.YY12).val;_LL13B: yyzzz=_tmp277;goto _LL139;_LL13C:;_LL13D:(int)
_throw((void*)& Cyc_yyfail_YY12);_LL139:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY12(
struct _tuple19*yy1){union Cyc_YYSTYPE _tmp9DE;return((_tmp9DE.YY12).val=yy1,(((
_tmp9DE.YY12).tag=18,_tmp9DE)));}static char _tmp27A[17]="list_t<pat_t,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp27A,_tmp27A,
_tmp27A + 17}};struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp27B=yy1;struct Cyc_List_List*_tmp27C;_LL13F: if((
_tmp27B.YY13).tag != 19)goto _LL141;_tmp27C=(struct Cyc_List_List*)(_tmp27B.YY13).val;
_LL140: yyzzz=_tmp27C;goto _LL13E;_LL141:;_LL142:(int)_throw((void*)& Cyc_yyfail_YY13);
_LL13E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9DF;return((_tmp9DF.YY13).val=yy1,(((_tmp9DF.YY13).tag=19,_tmp9DF)));}static
char _tmp27F[36]="$(list_t<designator_t,`H>,pat_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp27F,_tmp27F,_tmp27F + 36}};struct _tuple20*
Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){struct _tuple20*yyzzz;{union Cyc_YYSTYPE
_tmp280=yy1;struct _tuple20*_tmp281;_LL144: if((_tmp280.YY14).tag != 20)goto _LL146;
_tmp281=(struct _tuple20*)(_tmp280.YY14).val;_LL145: yyzzz=_tmp281;goto _LL143;
_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY14);_LL143:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY14(struct _tuple20*yy1){union Cyc_YYSTYPE _tmp9E0;return((_tmp9E0.YY14).val=
yy1,(((_tmp9E0.YY14).tag=20,_tmp9E0)));}static char _tmp284[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp284,
_tmp284,_tmp284 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp285=yy1;struct Cyc_List_List*
_tmp286;_LL149: if((_tmp285.YY15).tag != 21)goto _LL14B;_tmp286=(struct Cyc_List_List*)(
_tmp285.YY15).val;_LL14A: yyzzz=_tmp286;goto _LL148;_LL14B:;_LL14C:(int)_throw((
void*)& Cyc_yyfail_YY15);_LL148:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9E1;return((_tmp9E1.YY15).val=yy1,(((_tmp9E1.YY15).tag=
21,_tmp9E1)));}static char _tmp289[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp289,
_tmp289,_tmp289 + 58}};struct _tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct
_tuple19*yyzzz;{union Cyc_YYSTYPE _tmp28A=yy1;struct _tuple19*_tmp28B;_LL14E: if((
_tmp28A.YY16).tag != 22)goto _LL150;_tmp28B=(struct _tuple19*)(_tmp28A.YY16).val;
_LL14F: yyzzz=_tmp28B;goto _LL14D;_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY16);
_LL14D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY16(struct _tuple19*yy1){union Cyc_YYSTYPE
_tmp9E2;return((_tmp9E2.YY16).val=yy1,(((_tmp9E2.YY16).tag=22,_tmp9E2)));}static
char _tmp28E[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp28E,_tmp28E,_tmp28E + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp28F=yy1;
struct Cyc_Absyn_Fndecl*_tmp290;_LL153: if((_tmp28F.YY17).tag != 23)goto _LL155;
_tmp290=(struct Cyc_Absyn_Fndecl*)(_tmp28F.YY17).val;_LL154: yyzzz=_tmp290;goto
_LL152;_LL155:;_LL156:(int)_throw((void*)& Cyc_yyfail_YY17);_LL152:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9E3;
return((_tmp9E3.YY17).val=yy1,(((_tmp9E3.YY17).tag=23,_tmp9E3)));}static char
_tmp293[18]="list_t<decl_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp293,_tmp293,_tmp293 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp294=yy1;
struct Cyc_List_List*_tmp295;_LL158: if((_tmp294.YY18).tag != 24)goto _LL15A;_tmp295=(
struct Cyc_List_List*)(_tmp294.YY18).val;_LL159: yyzzz=_tmp295;goto _LL157;_LL15A:;
_LL15B:(int)_throw((void*)& Cyc_yyfail_YY18);_LL157:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E4;return((_tmp9E4.YY18).val=
yy1,(((_tmp9E4.YY18).tag=24,_tmp9E4)));}static char _tmp298[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp298,
_tmp298,_tmp298 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp299=yy1;struct
Cyc_Parse_Declaration_spec*_tmp29A;_LL15D: if((_tmp299.YY19).tag != 25)goto _LL15F;
_tmp29A=(struct Cyc_Parse_Declaration_spec*)(_tmp299.YY19).val;_LL15E: yyzzz=
_tmp29A;goto _LL15C;_LL15F:;_LL160:(int)_throw((void*)& Cyc_yyfail_YY19);_LL15C:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){
union Cyc_YYSTYPE _tmp9E5;return((_tmp9E5.YY19).val=yy1,(((_tmp9E5.YY19).tag=25,
_tmp9E5)));}static char _tmp29D[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp29D,_tmp29D,_tmp29D + 29}};struct _tuple21*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE
_tmp29E=yy1;struct _tuple21*_tmp29F;_LL162: if((_tmp29E.YY20).tag != 26)goto _LL164;
_tmp29F=(struct _tuple21*)(_tmp29E.YY20).val;_LL163: yyzzz=_tmp29F;goto _LL161;
_LL164:;_LL165:(int)_throw((void*)& Cyc_yyfail_YY20);_LL161:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY20(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9E6;return((_tmp9E6.YY20).val=
yy1,(((_tmp9E6.YY20).tag=26,_tmp9E6)));}static char _tmp2A2[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp2A2,
_tmp2A2,_tmp2A2 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2A3=yy1;struct Cyc_List_List*
_tmp2A4;_LL167: if((_tmp2A3.YY21).tag != 27)goto _LL169;_tmp2A4=(struct Cyc_List_List*)(
_tmp2A3.YY21).val;_LL168: yyzzz=_tmp2A4;goto _LL166;_LL169:;_LL16A:(int)_throw((
void*)& Cyc_yyfail_YY21);_LL166:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9E7;return((_tmp9E7.YY21).val=yy1,(((_tmp9E7.YY21).tag=
27,_tmp9E7)));}static char _tmp2A7[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp2A7,_tmp2A7,_tmp2A7 + 16}};enum Cyc_Parse_Storage_class
Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){enum Cyc_Parse_Storage_class yyzzz;{union Cyc_YYSTYPE
_tmp2A8=yy1;enum Cyc_Parse_Storage_class _tmp2A9;_LL16C: if((_tmp2A8.YY22).tag != 
28)goto _LL16E;_tmp2A9=(enum Cyc_Parse_Storage_class)(_tmp2A8.YY22).val;_LL16D:
yyzzz=_tmp2A9;goto _LL16B;_LL16E:;_LL16F:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL16B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class yy1){
union Cyc_YYSTYPE _tmp9E8;return((_tmp9E8.YY22).val=yy1,(((_tmp9E8.YY22).tag=28,
_tmp9E8)));}static char _tmp2AC[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp2AC,_tmp2AC,_tmp2AC + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2AD=yy1;void*_tmp2AE;_LL171:
if((_tmp2AD.YY23).tag != 29)goto _LL173;_tmp2AE=(void*)(_tmp2AD.YY23).val;_LL172:
yyzzz=_tmp2AE;goto _LL170;_LL173:;_LL174:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL170:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(void*yy1){union Cyc_YYSTYPE _tmp9E9;
return((_tmp9E9.YY23).val=yy1,(((_tmp9E9.YY23).tag=29,_tmp9E9)));}static char
_tmp2B1[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={
Cyc_Core_Failure,{_tmp2B1,_tmp2B1,_tmp2B1 + 12}};enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(
union Cyc_YYSTYPE yy1){enum Cyc_Absyn_AggrKind yyzzz;{union Cyc_YYSTYPE _tmp2B2=yy1;
enum Cyc_Absyn_AggrKind _tmp2B3;_LL176: if((_tmp2B2.YY24).tag != 30)goto _LL178;
_tmp2B3=(enum Cyc_Absyn_AggrKind)(_tmp2B2.YY24).val;_LL177: yyzzz=_tmp2B3;goto
_LL175;_LL178:;_LL179:(int)_throw((void*)& Cyc_yyfail_YY24);_LL175:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9EA;
return((_tmp9EA.YY24).val=yy1,(((_tmp9EA.YY24).tag=30,_tmp9EA)));}static char
_tmp2B6[8]="tqual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{
_tmp2B6,_tmp2B6,_tmp2B6 + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE _tmp2B7=yy1;struct Cyc_Absyn_Tqual
_tmp2B8;_LL17B: if((_tmp2B7.YY25).tag != 31)goto _LL17D;_tmp2B8=(struct Cyc_Absyn_Tqual)(
_tmp2B7.YY25).val;_LL17C: yyzzz=_tmp2B8;goto _LL17A;_LL17D:;_LL17E:(int)_throw((
void*)& Cyc_yyfail_YY25);_LL17A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual
yy1){union Cyc_YYSTYPE _tmp9EB;return((_tmp9EB.YY25).val=yy1,(((_tmp9EB.YY25).tag=
31,_tmp9EB)));}static char _tmp2BB[23]="list_t<aggrfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp2BB,_tmp2BB,_tmp2BB + 23}};struct Cyc_List_List*
Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2BC=yy1;struct Cyc_List_List*_tmp2BD;_LL180: if((_tmp2BC.YY26).tag != 32)goto
_LL182;_tmp2BD=(struct Cyc_List_List*)(_tmp2BC.YY26).val;_LL181: yyzzz=_tmp2BD;
goto _LL17F;_LL182:;_LL183:(int)_throw((void*)& Cyc_yyfail_YY26);_LL17F:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EC;
return((_tmp9EC.YY26).val=yy1,(((_tmp9EC.YY26).tag=32,_tmp9EC)));}static char
_tmp2C0[34]="list_t<list_t<aggrfield_t,`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2C0,_tmp2C0,_tmp2C0 + 34}};struct Cyc_List_List*
Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2C1=yy1;struct Cyc_List_List*_tmp2C2;_LL185: if((_tmp2C1.YY27).tag != 33)goto
_LL187;_tmp2C2=(struct Cyc_List_List*)(_tmp2C1.YY27).val;_LL186: yyzzz=_tmp2C2;
goto _LL184;_LL187:;_LL188:(int)_throw((void*)& Cyc_yyfail_YY27);_LL184:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9ED;
return((_tmp9ED.YY27).val=yy1,(((_tmp9ED.YY27).tag=33,_tmp9ED)));}static char
_tmp2C5[31]="list_t<type_modifier_t<`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2C5,_tmp2C5,_tmp2C5 + 31}};struct Cyc_List_List*
Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2C6=yy1;struct Cyc_List_List*_tmp2C7;_LL18A: if((_tmp2C6.YY28).tag != 34)goto
_LL18C;_tmp2C7=(struct Cyc_List_List*)(_tmp2C6.YY28).val;_LL18B: yyzzz=_tmp2C7;
goto _LL189;_LL18C:;_LL18D:(int)_throw((void*)& Cyc_yyfail_YY28);_LL189:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EE;
return((_tmp9EE.YY28).val=yy1,(((_tmp9EE.YY28).tag=34,_tmp9EE)));}static char
_tmp2CA[13]="declarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={
Cyc_Core_Failure,{_tmp2CA,_tmp2CA,_tmp2CA + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE _tmp2CB=
yy1;struct Cyc_Parse_Declarator*_tmp2CC;_LL18F: if((_tmp2CB.YY29).tag != 35)goto
_LL191;_tmp2CC=(struct Cyc_Parse_Declarator*)(_tmp2CB.YY29).val;_LL190: yyzzz=
_tmp2CC;goto _LL18E;_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY29);_LL18E:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){union Cyc_YYSTYPE
_tmp9EF;return((_tmp9EF.YY29).val=yy1,(((_tmp9EF.YY29).tag=35,_tmp9EF)));}static
char _tmp2CF[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={
Cyc_Core_Failure,{_tmp2CF,_tmp2CF,_tmp2CF + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{
union Cyc_YYSTYPE _tmp2D0=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp2D1;_LL194:
if((_tmp2D0.YY30).tag != 36)goto _LL196;_tmp2D1=(struct Cyc_Parse_Abstractdeclarator*)(
_tmp2D0.YY30).val;_LL195: yyzzz=_tmp2D1;goto _LL193;_LL196:;_LL197:(int)_throw((
void*)& Cyc_yyfail_YY30);_LL193:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1){union Cyc_YYSTYPE _tmp9F0;return((_tmp9F0.YY30).val=yy1,(((_tmp9F0.YY30).tag=
36,_tmp9F0)));}static char _tmp2D4[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2D4,_tmp2D4,_tmp2D4 + 5}};int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp2D5=yy1;int _tmp2D6;_LL199: if((
_tmp2D5.YY31).tag != 37)goto _LL19B;_tmp2D6=(int)(_tmp2D5.YY31).val;_LL19A: yyzzz=
_tmp2D6;goto _LL198;_LL19B:;_LL19C:(int)_throw((void*)& Cyc_yyfail_YY31);_LL198:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp9F1;return((
_tmp9F1.YY31).val=yy1,(((_tmp9F1.YY31).tag=37,_tmp9F1)));}static char _tmp2D9[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2D9,
_tmp2D9,_tmp2D9 + 8}};enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;{union Cyc_YYSTYPE _tmp2DA=yy1;enum Cyc_Absyn_Scope
_tmp2DB;_LL19E: if((_tmp2DA.YY32).tag != 38)goto _LL1A0;_tmp2DB=(enum Cyc_Absyn_Scope)(
_tmp2DA.YY32).val;_LL19F: yyzzz=_tmp2DB;goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((
void*)& Cyc_yyfail_YY32);_LL19D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope
yy1){union Cyc_YYSTYPE _tmp9F2;return((_tmp9F2.YY32).val=yy1,(((_tmp9F2.YY32).tag=
38,_tmp9F2)));}static char _tmp2DE[16]="datatypefield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2DE,_tmp2DE,_tmp2DE + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union
Cyc_YYSTYPE _tmp2DF=yy1;struct Cyc_Absyn_Datatypefield*_tmp2E0;_LL1A3: if((_tmp2DF.YY33).tag
!= 39)goto _LL1A5;_tmp2E0=(struct Cyc_Absyn_Datatypefield*)(_tmp2DF.YY33).val;
_LL1A4: yyzzz=_tmp2E0;goto _LL1A2;_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL1A2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){
union Cyc_YYSTYPE _tmp9F3;return((_tmp9F3.YY33).val=yy1,(((_tmp9F3.YY33).tag=39,
_tmp9F3)));}static char _tmp2E3[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2E3,_tmp2E3,_tmp2E3 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2E4=yy1;struct Cyc_List_List*_tmp2E5;_LL1A8: if((_tmp2E4.YY34).tag != 40)goto
_LL1AA;_tmp2E5=(struct Cyc_List_List*)(_tmp2E4.YY34).val;_LL1A9: yyzzz=_tmp2E5;
goto _LL1A7;_LL1AA:;_LL1AB:(int)_throw((void*)& Cyc_yyfail_YY34);_LL1A7:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F4;
return((_tmp9F4.YY34).val=yy1,(((_tmp9F4.YY34).tag=40,_tmp9F4)));}static char
_tmp2E8[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2E8,_tmp2E8,
_tmp2E8 + 55}};struct _tuple22*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple22*
yyzzz;{union Cyc_YYSTYPE _tmp2E9=yy1;struct _tuple22*_tmp2EA;_LL1AD: if((_tmp2E9.YY35).tag
!= 41)goto _LL1AF;_tmp2EA=(struct _tuple22*)(_tmp2E9.YY35).val;_LL1AE: yyzzz=
_tmp2EA;goto _LL1AC;_LL1AF:;_LL1B0:(int)_throw((void*)& Cyc_yyfail_YY35);_LL1AC:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY35(struct _tuple22*yy1){union Cyc_YYSTYPE
_tmp9F5;return((_tmp9F5.YY35).val=yy1,(((_tmp9F5.YY35).tag=41,_tmp9F5)));}static
char _tmp2ED[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={
Cyc_Core_Failure,{_tmp2ED,_tmp2ED,_tmp2ED + 17}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2EE=yy1;
struct Cyc_List_List*_tmp2EF;_LL1B2: if((_tmp2EE.YY36).tag != 42)goto _LL1B4;_tmp2EF=(
struct Cyc_List_List*)(_tmp2EE.YY36).val;_LL1B3: yyzzz=_tmp2EF;goto _LL1B1;_LL1B4:;
_LL1B5:(int)_throw((void*)& Cyc_yyfail_YY36);_LL1B1:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F6;return((_tmp9F6.YY36).val=
yy1,(((_tmp9F6.YY36).tag=42,_tmp9F6)));}static char _tmp2F2[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2F2,
_tmp2F2,_tmp2F2 + 37}};struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){struct
_tuple8*yyzzz;{union Cyc_YYSTYPE _tmp2F3=yy1;struct _tuple8*_tmp2F4;_LL1B7: if((
_tmp2F3.YY37).tag != 43)goto _LL1B9;_tmp2F4=(struct _tuple8*)(_tmp2F3.YY37).val;
_LL1B8: yyzzz=_tmp2F4;goto _LL1B6;_LL1B9:;_LL1BA:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL1B6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE
_tmp9F7;return((_tmp9F7.YY37).val=yy1,(((_tmp9F7.YY37).tag=43,_tmp9F7)));}static
char _tmp2F7[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2F7,_tmp2F7,
_tmp2F7 + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2F8=yy1;struct Cyc_List_List*_tmp2F9;_LL1BC: if((
_tmp2F8.YY38).tag != 44)goto _LL1BE;_tmp2F9=(struct Cyc_List_List*)(_tmp2F8.YY38).val;
_LL1BD: yyzzz=_tmp2F9;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1BB:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9F8;return((_tmp9F8.YY38).val=yy1,(((_tmp9F8.YY38).tag=44,_tmp9F8)));}static
char _tmp2FC[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2FC,
_tmp2FC,_tmp2FC + 127}};struct _tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct
_tuple23*yyzzz;{union Cyc_YYSTYPE _tmp2FD=yy1;struct _tuple23*_tmp2FE;_LL1C1: if((
_tmp2FD.YY39).tag != 45)goto _LL1C3;_tmp2FE=(struct _tuple23*)(_tmp2FD.YY39).val;
_LL1C2: yyzzz=_tmp2FE;goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY39);
_LL1C0:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY39(struct _tuple23*yy1){union Cyc_YYSTYPE
_tmp9F9;return((_tmp9F9.YY39).val=yy1,(((_tmp9F9.YY39).tag=45,_tmp9F9)));}static
char _tmp301[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={
Cyc_Core_Failure,{_tmp301,_tmp301,_tmp301 + 18}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp302=yy1;
struct Cyc_List_List*_tmp303;_LL1C6: if((_tmp302.YY40).tag != 46)goto _LL1C8;_tmp303=(
struct Cyc_List_List*)(_tmp302.YY40).val;_LL1C7: yyzzz=_tmp303;goto _LL1C5;_LL1C8:;
_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY40);_LL1C5:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FA;return((_tmp9FA.YY40).val=
yy1,(((_tmp9FA.YY40).tag=46,_tmp9FA)));}static char _tmp306[24]="list_t<designator_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp306,
_tmp306,_tmp306 + 24}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp307=yy1;struct Cyc_List_List*
_tmp308;_LL1CB: if((_tmp307.YY41).tag != 47)goto _LL1CD;_tmp308=(struct Cyc_List_List*)(
_tmp307.YY41).val;_LL1CC: yyzzz=_tmp308;goto _LL1CA;_LL1CD:;_LL1CE:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1CA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9FB;return((_tmp9FB.YY41).val=yy1,(((_tmp9FB.YY41).tag=
47,_tmp9FB)));}static char _tmp30B[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp30B,_tmp30B,_tmp30B + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp30C=yy1;void*_tmp30D;_LL1D0:
if((_tmp30C.YY42).tag != 48)goto _LL1D2;_tmp30D=(void*)(_tmp30C.YY42).val;_LL1D1:
yyzzz=_tmp30D;goto _LL1CF;_LL1D2:;_LL1D3:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1CF:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp9FC;
return((_tmp9FC.YY42).val=yy1,(((_tmp9FC.YY42).tag=48,_tmp9FC)));}static char
_tmp310[7]="kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{
_tmp310,_tmp310,_tmp310 + 7}};struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Kind*yyzzz;{union Cyc_YYSTYPE _tmp311=yy1;struct Cyc_Absyn_Kind*
_tmp312;_LL1D5: if((_tmp311.YY43).tag != 49)goto _LL1D7;_tmp312=(struct Cyc_Absyn_Kind*)(
_tmp311.YY43).val;_LL1D6: yyzzz=_tmp312;goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((
void*)& Cyc_yyfail_YY43);_LL1D4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*
yy1){union Cyc_YYSTYPE _tmp9FD;return((_tmp9FD.YY43).val=yy1,(((_tmp9FD.YY43).tag=
49,_tmp9FD)));}static char _tmp315[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp315,_tmp315,_tmp315 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp316=yy1;void*_tmp317;_LL1DA:
if((_tmp316.YY44).tag != 50)goto _LL1DC;_tmp317=(void*)(_tmp316.YY44).val;_LL1DB:
yyzzz=_tmp317;goto _LL1D9;_LL1DC:;_LL1DD:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1D9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmp9FE;
return((_tmp9FE.YY44).val=yy1,(((_tmp9FE.YY44).tag=50,_tmp9FE)));}static char
_tmp31A[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY45={
Cyc_Core_Failure,{_tmp31A,_tmp31A,_tmp31A + 23}};struct Cyc_List_List*Cyc_yyget_YY45(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp31B=yy1;
struct Cyc_List_List*_tmp31C;_LL1DF: if((_tmp31B.YY45).tag != 51)goto _LL1E1;_tmp31C=(
struct Cyc_List_List*)(_tmp31B.YY45).val;_LL1E0: yyzzz=_tmp31C;goto _LL1DE;_LL1E1:;
_LL1E2:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1DE:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FF;return((_tmp9FF.YY45).val=
yy1,(((_tmp9FF.YY45).tag=51,_tmp9FF)));}static char _tmp31F[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp31F,
_tmp31F,_tmp31F + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union
Cyc_YYSTYPE _tmp320=yy1;void*_tmp321;_LL1E4: if((_tmp320.YY46).tag != 52)goto _LL1E6;
_tmp321=(void*)(_tmp320.YY46).val;_LL1E5: yyzzz=_tmp321;goto _LL1E3;_LL1E6:;_LL1E7:(
int)_throw((void*)& Cyc_yyfail_YY46);_LL1E3:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1){union Cyc_YYSTYPE _tmpA00;return((_tmpA00.YY46).val=yy1,(((_tmpA00.YY46).tag=
52,_tmpA00)));}static char _tmp324[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp324,_tmp324,_tmp324 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE
_tmp325=yy1;struct Cyc_Absyn_Enumfield*_tmp326;_LL1E9: if((_tmp325.YY47).tag != 53)
goto _LL1EB;_tmp326=(struct Cyc_Absyn_Enumfield*)(_tmp325.YY47).val;_LL1EA: yyzzz=
_tmp326;goto _LL1E8;_LL1EB:;_LL1EC:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1E8:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE
_tmpA01;return((_tmpA01.YY47).val=yy1,(((_tmpA01.YY47).tag=53,_tmpA01)));}static
char _tmp329[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={
Cyc_Core_Failure,{_tmp329,_tmp329,_tmp329 + 23}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp32A=yy1;
struct Cyc_List_List*_tmp32B;_LL1EE: if((_tmp32A.YY48).tag != 54)goto _LL1F0;_tmp32B=(
struct Cyc_List_List*)(_tmp32A.YY48).val;_LL1EF: yyzzz=_tmp32B;goto _LL1ED;_LL1F0:;
_LL1F1:(int)_throw((void*)& Cyc_yyfail_YY48);_LL1ED:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA02;return((_tmpA02.YY48).val=
yy1,(((_tmpA02.YY48).tag=54,_tmpA02)));}static char _tmp32E[17]="opt_t<type_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp32E,
_tmp32E,_tmp32E + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp32F=yy1;struct Cyc_Core_Opt*_tmp330;
_LL1F3: if((_tmp32F.YY49).tag != 55)goto _LL1F5;_tmp330=(struct Cyc_Core_Opt*)(
_tmp32F.YY49).val;_LL1F4: yyzzz=_tmp330;goto _LL1F2;_LL1F5:;_LL1F6:(int)_throw((
void*)& Cyc_yyfail_YY49);_LL1F2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*
yy1){union Cyc_YYSTYPE _tmpA03;return((_tmpA03.YY49).val=yy1,(((_tmpA03.YY49).tag=
55,_tmpA03)));}static char _tmp333[31]="list_t<$(type_t,type_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp333,_tmp333,
_tmp333 + 31}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp334=yy1;struct Cyc_List_List*_tmp335;_LL1F8: if((
_tmp334.YY50).tag != 56)goto _LL1FA;_tmp335=(struct Cyc_List_List*)(_tmp334.YY50).val;
_LL1F9: yyzzz=_tmp335;goto _LL1F7;_LL1FA:;_LL1FB:(int)_throw((void*)& Cyc_yyfail_YY50);
_LL1F7:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmpA04;return((_tmpA04.YY50).val=yy1,(((_tmpA04.YY50).tag=56,_tmpA04)));}static
char _tmp338[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY51={
Cyc_Core_Failure,{_tmp338,_tmp338,_tmp338 + 15}};union Cyc_Absyn_Constraint*Cyc_yyget_YY51(
union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp339=
yy1;union Cyc_Absyn_Constraint*_tmp33A;_LL1FD: if((_tmp339.YY51).tag != 57)goto
_LL1FF;_tmp33A=(union Cyc_Absyn_Constraint*)(_tmp339.YY51).val;_LL1FE: yyzzz=
_tmp33A;goto _LL1FC;_LL1FF:;_LL200:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1FC:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE
_tmpA05;return((_tmpA05.YY51).val=yy1,(((_tmpA05.YY51).tag=57,_tmpA05)));}static
char _tmp33D[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp33D,_tmp33D,_tmp33D + 45}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp33E=yy1;struct Cyc_List_List*_tmp33F;_LL202: if((_tmp33E.YY52).tag != 58)goto
_LL204;_tmp33F=(struct Cyc_List_List*)(_tmp33E.YY52).val;_LL203: yyzzz=_tmp33F;
goto _LL201;_LL204:;_LL205:(int)_throw((void*)& Cyc_yyfail_YY52);_LL201:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA06;
return((_tmpA06.YY52).val=yy1,(((_tmpA06.YY52).tag=58,_tmpA06)));}static char
_tmp342[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp342,_tmp342,_tmp342 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp343=yy1;void*_tmp344;_LL207: if((_tmp343.YY53).tag
!= 59)goto _LL209;_tmp344=(void*)(_tmp343.YY53).val;_LL208: yyzzz=_tmp344;goto
_LL206;_LL209:;_LL20A:(int)_throw((void*)& Cyc_yyfail_YY53);_LL206:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmpA07;return((_tmpA07.YY53).val=
yy1,(((_tmpA07.YY53).tag=59,_tmpA07)));}static char _tmp347[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp347,
_tmp347,_tmp347 + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp348=yy1;struct Cyc_List_List*
_tmp349;_LL20C: if((_tmp348.YY54).tag != 60)goto _LL20E;_tmp349=(struct Cyc_List_List*)(
_tmp348.YY54).val;_LL20D: yyzzz=_tmp349;goto _LL20B;_LL20E:;_LL20F:(int)_throw((
void*)& Cyc_yyfail_YY54);_LL20B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmpA08;return((_tmpA08.YY54).val=yy1,(((_tmpA08.YY54).tag=
60,_tmpA08)));}static char _tmp34C[21]="$(bool,string_t<`H>)";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp34C,_tmp34C,_tmp34C + 21}};struct
_tuple17 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){struct _tuple17 yyzzz;{union Cyc_YYSTYPE
_tmp34D=yy1;struct _tuple17 _tmp34E;_LL211: if((_tmp34D.Asm_tok).tag != 6)goto _LL213;
_tmp34E=(struct _tuple17)(_tmp34D.Asm_tok).val;_LL212: yyzzz=_tmp34E;goto _LL210;
_LL213:;_LL214:(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL210:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple17 yy1){union Cyc_YYSTYPE _tmpA09;return((
_tmpA09.Asm_tok).val=yy1,(((_tmpA09.Asm_tok).tag=6,_tmpA09)));}struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_Yyltype Cyc_yynewloc(){struct Cyc_Yyltype _tmpA0A;return(_tmpA0A.timestamp=0,((
_tmpA0A.first_line=0,((_tmpA0A.first_column=0,((_tmpA0A.last_line=0,((_tmpA0A.last_column=
0,_tmpA0A)))))))));}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[
374]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,145,2,2,
129,143,140,2,126,127,134,137,122,141,131,142,2,2,2,2,2,2,2,2,2,2,130,119,124,
123,125,136,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,133,
139,128,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,120,138,121,144,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,
79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,
104,105,106,107,108,109,110,111,112,113,114,115,116,117,118};static char _tmp351[2]="$";
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
_tmp3BE[8]="WSTRING";static char _tmp3BF[19]="CHARACTER_CONSTANT";static char
_tmp3C0[20]="WCHARACTER_CONSTANT";static char _tmp3C1[18]="FLOATING_CONSTANT";
static char _tmp3C2[9]="TYPE_VAR";static char _tmp3C3[13]="TYPEDEF_NAME";static char
_tmp3C4[16]="QUAL_IDENTIFIER";static char _tmp3C5[18]="QUAL_TYPEDEF_NAME";static
char _tmp3C6[10]="ATTRIBUTE";static char _tmp3C7[4]="ASM";static char _tmp3C8[4]="';'";
static char _tmp3C9[4]="'{'";static char _tmp3CA[4]="'}'";static char _tmp3CB[4]="','";
static char _tmp3CC[4]="'='";static char _tmp3CD[4]="'<'";static char _tmp3CE[4]="'>'";
static char _tmp3CF[4]="'('";static char _tmp3D0[4]="')'";static char _tmp3D1[4]="'_'";
static char _tmp3D2[4]="'$'";static char _tmp3D3[4]="':'";static char _tmp3D4[4]="'.'";
static char _tmp3D5[4]="'['";static char _tmp3D6[4]="']'";static char _tmp3D7[4]="'*'";
static char _tmp3D8[4]="'@'";static char _tmp3D9[4]="'?'";static char _tmp3DA[4]="'+'";
static char _tmp3DB[4]="'|'";static char _tmp3DC[4]="'^'";static char _tmp3DD[4]="'&'";
static char _tmp3DE[4]="'-'";static char _tmp3DF[4]="'/'";static char _tmp3E0[4]="'%'";
static char _tmp3E1[4]="'~'";static char _tmp3E2[4]="'!'";static char _tmp3E3[5]="prog";
static char _tmp3E4[17]="translation_unit";static char _tmp3E5[12]="export_list";
static char _tmp3E6[19]="export_list_values";static char _tmp3E7[21]="external_declaration";
static char _tmp3E8[20]="function_definition";static char _tmp3E9[21]="function_definition2";
static char _tmp3EA[13]="using_action";static char _tmp3EB[15]="unusing_action";
static char _tmp3EC[17]="namespace_action";static char _tmp3ED[19]="unnamespace_action";
static char _tmp3EE[12]="declaration";static char _tmp3EF[19]="resetable_qual_opt";
static char _tmp3F0[17]="declaration_list";static char _tmp3F1[23]="declaration_specifiers";
static char _tmp3F2[24]="storage_class_specifier";static char _tmp3F3[15]="attributes_opt";
static char _tmp3F4[11]="attributes";static char _tmp3F5[15]="attribute_list";static
char _tmp3F6[10]="attribute";static char _tmp3F7[15]="type_specifier";static char
_tmp3F8[25]="type_specifier_notypedef";static char _tmp3F9[5]="kind";static char
_tmp3FA[15]="type_qualifier";static char _tmp3FB[15]="enum_specifier";static char
_tmp3FC[11]="enum_field";static char _tmp3FD[22]="enum_declaration_list";static
char _tmp3FE[26]="struct_or_union_specifier";static char _tmp3FF[16]="type_params_opt";
static char _tmp400[16]="struct_or_union";static char _tmp401[24]="struct_declaration_list";
static char _tmp402[25]="struct_declaration_list0";static char _tmp403[21]="init_declarator_list";
static char _tmp404[22]="init_declarator_list0";static char _tmp405[16]="init_declarator";
static char _tmp406[19]="struct_declaration";static char _tmp407[25]="specifier_qualifier_list";
static char _tmp408[35]="notypedef_specifier_qualifier_list";static char _tmp409[23]="struct_declarator_list";
static char _tmp40A[24]="struct_declarator_list0";static char _tmp40B[18]="struct_declarator";
static char _tmp40C[19]="datatype_specifier";static char _tmp40D[14]="qual_datatype";
static char _tmp40E[19]="datatypefield_list";static char _tmp40F[20]="datatypefield_scope";
static char _tmp410[14]="datatypefield";static char _tmp411[11]="declarator";static
char _tmp412[23]="declarator_withtypedef";static char _tmp413[18]="direct_declarator";
static char _tmp414[30]="direct_declarator_withtypedef";static char _tmp415[8]="pointer";
static char _tmp416[12]="one_pointer";static char _tmp417[14]="pointer_quals";static
char _tmp418[13]="pointer_qual";static char _tmp419[23]="pointer_null_and_bound";
static char _tmp41A[14]="pointer_bound";static char _tmp41B[18]="zeroterm_qual_opt";
static char _tmp41C[8]="rgn_opt";static char _tmp41D[11]="tqual_list";static char
_tmp41E[20]="parameter_type_list";static char _tmp41F[9]="type_var";static char
_tmp420[16]="optional_effect";static char _tmp421[19]="optional_rgn_order";static
char _tmp422[10]="rgn_order";static char _tmp423[16]="optional_inject";static char
_tmp424[11]="effect_set";static char _tmp425[14]="atomic_effect";static char _tmp426[
11]="region_set";static char _tmp427[15]="parameter_list";static char _tmp428[22]="parameter_declaration";
static char _tmp429[16]="identifier_list";static char _tmp42A[17]="identifier_list0";
static char _tmp42B[12]="initializer";static char _tmp42C[18]="array_initializer";
static char _tmp42D[17]="initializer_list";static char _tmp42E[12]="designation";
static char _tmp42F[16]="designator_list";static char _tmp430[11]="designator";
static char _tmp431[10]="type_name";static char _tmp432[14]="any_type_name";static
char _tmp433[15]="type_name_list";static char _tmp434[20]="abstract_declarator";
static char _tmp435[27]="direct_abstract_declarator";static char _tmp436[10]="statement";
static char _tmp437[13]="open_exp_opt";static char _tmp438[18]="labeled_statement";
static char _tmp439[21]="expression_statement";static char _tmp43A[19]="compound_statement";
static char _tmp43B[16]="block_item_list";static char _tmp43C[20]="selection_statement";
static char _tmp43D[15]="switch_clauses";static char _tmp43E[20]="iteration_statement";
static char _tmp43F[15]="jump_statement";static char _tmp440[12]="exp_pattern";
static char _tmp441[20]="conditional_pattern";static char _tmp442[19]="logical_or_pattern";
static char _tmp443[20]="logical_and_pattern";static char _tmp444[21]="inclusive_or_pattern";
static char _tmp445[21]="exclusive_or_pattern";static char _tmp446[12]="and_pattern";
static char _tmp447[17]="equality_pattern";static char _tmp448[19]="relational_pattern";
static char _tmp449[14]="shift_pattern";static char _tmp44A[17]="additive_pattern";
static char _tmp44B[23]="multiplicative_pattern";static char _tmp44C[13]="cast_pattern";
static char _tmp44D[14]="unary_pattern";static char _tmp44E[16]="postfix_pattern";
static char _tmp44F[16]="primary_pattern";static char _tmp450[8]="pattern";static
char _tmp451[19]="tuple_pattern_list";static char _tmp452[20]="tuple_pattern_list0";
static char _tmp453[14]="field_pattern";static char _tmp454[19]="field_pattern_list";
static char _tmp455[20]="field_pattern_list0";static char _tmp456[11]="expression";
static char _tmp457[22]="assignment_expression";static char _tmp458[20]="assignment_operator";
static char _tmp459[23]="conditional_expression";static char _tmp45A[20]="constant_expression";
static char _tmp45B[22]="logical_or_expression";static char _tmp45C[23]="logical_and_expression";
static char _tmp45D[24]="inclusive_or_expression";static char _tmp45E[24]="exclusive_or_expression";
static char _tmp45F[15]="and_expression";static char _tmp460[20]="equality_expression";
static char _tmp461[22]="relational_expression";static char _tmp462[17]="shift_expression";
static char _tmp463[20]="additive_expression";static char _tmp464[26]="multiplicative_expression";
static char _tmp465[16]="cast_expression";static char _tmp466[17]="unary_expression";
static char _tmp467[15]="unary_operator";static char _tmp468[19]="postfix_expression";
static char _tmp469[19]="primary_expression";static char _tmp46A[25]="argument_expression_list";
static char _tmp46B[26]="argument_expression_list0";static char _tmp46C[9]="constant";
static char _tmp46D[20]="qual_opt_identifier";static char _tmp46E[17]="qual_opt_typedef";
static char _tmp46F[18]="struct_union_name";static char _tmp470[11]="field_name";
static char _tmp471[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[289]={{
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
_tmp3BE,_tmp3BE,_tmp3BE + 8},{_tmp3BF,_tmp3BF,_tmp3BF + 19},{_tmp3C0,_tmp3C0,
_tmp3C0 + 20},{_tmp3C1,_tmp3C1,_tmp3C1 + 18},{_tmp3C2,_tmp3C2,_tmp3C2 + 9},{_tmp3C3,
_tmp3C3,_tmp3C3 + 13},{_tmp3C4,_tmp3C4,_tmp3C4 + 16},{_tmp3C5,_tmp3C5,_tmp3C5 + 18},{
_tmp3C6,_tmp3C6,_tmp3C6 + 10},{_tmp3C7,_tmp3C7,_tmp3C7 + 4},{_tmp3C8,_tmp3C8,
_tmp3C8 + 4},{_tmp3C9,_tmp3C9,_tmp3C9 + 4},{_tmp3CA,_tmp3CA,_tmp3CA + 4},{_tmp3CB,
_tmp3CB,_tmp3CB + 4},{_tmp3CC,_tmp3CC,_tmp3CC + 4},{_tmp3CD,_tmp3CD,_tmp3CD + 4},{
_tmp3CE,_tmp3CE,_tmp3CE + 4},{_tmp3CF,_tmp3CF,_tmp3CF + 4},{_tmp3D0,_tmp3D0,
_tmp3D0 + 4},{_tmp3D1,_tmp3D1,_tmp3D1 + 4},{_tmp3D2,_tmp3D2,_tmp3D2 + 4},{_tmp3D3,
_tmp3D3,_tmp3D3 + 4},{_tmp3D4,_tmp3D4,_tmp3D4 + 4},{_tmp3D5,_tmp3D5,_tmp3D5 + 4},{
_tmp3D6,_tmp3D6,_tmp3D6 + 4},{_tmp3D7,_tmp3D7,_tmp3D7 + 4},{_tmp3D8,_tmp3D8,
_tmp3D8 + 4},{_tmp3D9,_tmp3D9,_tmp3D9 + 4},{_tmp3DA,_tmp3DA,_tmp3DA + 4},{_tmp3DB,
_tmp3DB,_tmp3DB + 4},{_tmp3DC,_tmp3DC,_tmp3DC + 4},{_tmp3DD,_tmp3DD,_tmp3DD + 4},{
_tmp3DE,_tmp3DE,_tmp3DE + 4},{_tmp3DF,_tmp3DF,_tmp3DF + 4},{_tmp3E0,_tmp3E0,
_tmp3E0 + 4},{_tmp3E1,_tmp3E1,_tmp3E1 + 4},{_tmp3E2,_tmp3E2,_tmp3E2 + 4},{_tmp3E3,
_tmp3E3,_tmp3E3 + 5},{_tmp3E4,_tmp3E4,_tmp3E4 + 17},{_tmp3E5,_tmp3E5,_tmp3E5 + 12},{
_tmp3E6,_tmp3E6,_tmp3E6 + 19},{_tmp3E7,_tmp3E7,_tmp3E7 + 21},{_tmp3E8,_tmp3E8,
_tmp3E8 + 20},{_tmp3E9,_tmp3E9,_tmp3E9 + 21},{_tmp3EA,_tmp3EA,_tmp3EA + 13},{
_tmp3EB,_tmp3EB,_tmp3EB + 15},{_tmp3EC,_tmp3EC,_tmp3EC + 17},{_tmp3ED,_tmp3ED,
_tmp3ED + 19},{_tmp3EE,_tmp3EE,_tmp3EE + 12},{_tmp3EF,_tmp3EF,_tmp3EF + 19},{
_tmp3F0,_tmp3F0,_tmp3F0 + 17},{_tmp3F1,_tmp3F1,_tmp3F1 + 23},{_tmp3F2,_tmp3F2,
_tmp3F2 + 24},{_tmp3F3,_tmp3F3,_tmp3F3 + 15},{_tmp3F4,_tmp3F4,_tmp3F4 + 11},{
_tmp3F5,_tmp3F5,_tmp3F5 + 15},{_tmp3F6,_tmp3F6,_tmp3F6 + 10},{_tmp3F7,_tmp3F7,
_tmp3F7 + 15},{_tmp3F8,_tmp3F8,_tmp3F8 + 25},{_tmp3F9,_tmp3F9,_tmp3F9 + 5},{_tmp3FA,
_tmp3FA,_tmp3FA + 15},{_tmp3FB,_tmp3FB,_tmp3FB + 15},{_tmp3FC,_tmp3FC,_tmp3FC + 11},{
_tmp3FD,_tmp3FD,_tmp3FD + 22},{_tmp3FE,_tmp3FE,_tmp3FE + 26},{_tmp3FF,_tmp3FF,
_tmp3FF + 16},{_tmp400,_tmp400,_tmp400 + 16},{_tmp401,_tmp401,_tmp401 + 24},{
_tmp402,_tmp402,_tmp402 + 25},{_tmp403,_tmp403,_tmp403 + 21},{_tmp404,_tmp404,
_tmp404 + 22},{_tmp405,_tmp405,_tmp405 + 16},{_tmp406,_tmp406,_tmp406 + 19},{
_tmp407,_tmp407,_tmp407 + 25},{_tmp408,_tmp408,_tmp408 + 35},{_tmp409,_tmp409,
_tmp409 + 23},{_tmp40A,_tmp40A,_tmp40A + 24},{_tmp40B,_tmp40B,_tmp40B + 18},{
_tmp40C,_tmp40C,_tmp40C + 19},{_tmp40D,_tmp40D,_tmp40D + 14},{_tmp40E,_tmp40E,
_tmp40E + 19},{_tmp40F,_tmp40F,_tmp40F + 20},{_tmp410,_tmp410,_tmp410 + 14},{
_tmp411,_tmp411,_tmp411 + 11},{_tmp412,_tmp412,_tmp412 + 23},{_tmp413,_tmp413,
_tmp413 + 18},{_tmp414,_tmp414,_tmp414 + 30},{_tmp415,_tmp415,_tmp415 + 8},{_tmp416,
_tmp416,_tmp416 + 12},{_tmp417,_tmp417,_tmp417 + 14},{_tmp418,_tmp418,_tmp418 + 13},{
_tmp419,_tmp419,_tmp419 + 23},{_tmp41A,_tmp41A,_tmp41A + 14},{_tmp41B,_tmp41B,
_tmp41B + 18},{_tmp41C,_tmp41C,_tmp41C + 8},{_tmp41D,_tmp41D,_tmp41D + 11},{_tmp41E,
_tmp41E,_tmp41E + 20},{_tmp41F,_tmp41F,_tmp41F + 9},{_tmp420,_tmp420,_tmp420 + 16},{
_tmp421,_tmp421,_tmp421 + 19},{_tmp422,_tmp422,_tmp422 + 10},{_tmp423,_tmp423,
_tmp423 + 16},{_tmp424,_tmp424,_tmp424 + 11},{_tmp425,_tmp425,_tmp425 + 14},{
_tmp426,_tmp426,_tmp426 + 11},{_tmp427,_tmp427,_tmp427 + 15},{_tmp428,_tmp428,
_tmp428 + 22},{_tmp429,_tmp429,_tmp429 + 16},{_tmp42A,_tmp42A,_tmp42A + 17},{
_tmp42B,_tmp42B,_tmp42B + 12},{_tmp42C,_tmp42C,_tmp42C + 18},{_tmp42D,_tmp42D,
_tmp42D + 17},{_tmp42E,_tmp42E,_tmp42E + 12},{_tmp42F,_tmp42F,_tmp42F + 16},{
_tmp430,_tmp430,_tmp430 + 11},{_tmp431,_tmp431,_tmp431 + 10},{_tmp432,_tmp432,
_tmp432 + 14},{_tmp433,_tmp433,_tmp433 + 15},{_tmp434,_tmp434,_tmp434 + 20},{
_tmp435,_tmp435,_tmp435 + 27},{_tmp436,_tmp436,_tmp436 + 10},{_tmp437,_tmp437,
_tmp437 + 13},{_tmp438,_tmp438,_tmp438 + 18},{_tmp439,_tmp439,_tmp439 + 21},{
_tmp43A,_tmp43A,_tmp43A + 19},{_tmp43B,_tmp43B,_tmp43B + 16},{_tmp43C,_tmp43C,
_tmp43C + 20},{_tmp43D,_tmp43D,_tmp43D + 15},{_tmp43E,_tmp43E,_tmp43E + 20},{
_tmp43F,_tmp43F,_tmp43F + 15},{_tmp440,_tmp440,_tmp440 + 12},{_tmp441,_tmp441,
_tmp441 + 20},{_tmp442,_tmp442,_tmp442 + 19},{_tmp443,_tmp443,_tmp443 + 20},{
_tmp444,_tmp444,_tmp444 + 21},{_tmp445,_tmp445,_tmp445 + 21},{_tmp446,_tmp446,
_tmp446 + 12},{_tmp447,_tmp447,_tmp447 + 17},{_tmp448,_tmp448,_tmp448 + 19},{
_tmp449,_tmp449,_tmp449 + 14},{_tmp44A,_tmp44A,_tmp44A + 17},{_tmp44B,_tmp44B,
_tmp44B + 23},{_tmp44C,_tmp44C,_tmp44C + 13},{_tmp44D,_tmp44D,_tmp44D + 14},{
_tmp44E,_tmp44E,_tmp44E + 16},{_tmp44F,_tmp44F,_tmp44F + 16},{_tmp450,_tmp450,
_tmp450 + 8},{_tmp451,_tmp451,_tmp451 + 19},{_tmp452,_tmp452,_tmp452 + 20},{_tmp453,
_tmp453,_tmp453 + 14},{_tmp454,_tmp454,_tmp454 + 19},{_tmp455,_tmp455,_tmp455 + 20},{
_tmp456,_tmp456,_tmp456 + 11},{_tmp457,_tmp457,_tmp457 + 22},{_tmp458,_tmp458,
_tmp458 + 20},{_tmp459,_tmp459,_tmp459 + 23},{_tmp45A,_tmp45A,_tmp45A + 20},{
_tmp45B,_tmp45B,_tmp45B + 22},{_tmp45C,_tmp45C,_tmp45C + 23},{_tmp45D,_tmp45D,
_tmp45D + 24},{_tmp45E,_tmp45E,_tmp45E + 24},{_tmp45F,_tmp45F,_tmp45F + 15},{
_tmp460,_tmp460,_tmp460 + 20},{_tmp461,_tmp461,_tmp461 + 22},{_tmp462,_tmp462,
_tmp462 + 17},{_tmp463,_tmp463,_tmp463 + 20},{_tmp464,_tmp464,_tmp464 + 26},{
_tmp465,_tmp465,_tmp465 + 16},{_tmp466,_tmp466,_tmp466 + 17},{_tmp467,_tmp467,
_tmp467 + 15},{_tmp468,_tmp468,_tmp468 + 19},{_tmp469,_tmp469,_tmp469 + 19},{
_tmp46A,_tmp46A,_tmp46A + 25},{_tmp46B,_tmp46B,_tmp46B + 26},{_tmp46C,_tmp46C,
_tmp46C + 9},{_tmp46D,_tmp46D,_tmp46D + 20},{_tmp46E,_tmp46E,_tmp46E + 17},{_tmp46F,
_tmp46F,_tmp46F + 18},{_tmp470,_tmp470,_tmp470 + 11},{_tmp471,_tmp471,_tmp471 + 12}};
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
1030]={2729,- -32768,- -32768,- -32768,- -32768,- 21,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,3309,144,1289,- -32768,216,46,- -32768,81,107,164,166,190,202,269,115,356,
- -32768,- -32768,302,- -32768,- -32768,- -32768,286,519,314,304,312,312,- -32768,-
-32768,2592,- -32768,98,236,- -32768,380,590,3309,3309,3309,- -32768,3309,- -32768,-
-32768,512,- -32768,216,3228,285,- 49,710,803,- -32768,- -32768,332,349,359,- -32768,
216,358,- -32768,3082,241,- -32768,- -32768,- -32768,- -32768,332,5937,- -32768,360,
387,3082,405,421,435,- -32768,- 34,- -32768,- -32768,3455,3455,2592,2592,3455,471,
5937,426,- -32768,215,460,565,477,215,3674,5937,- -32768,- -32768,- -32768,2592,2865,
2592,2865,65,- -32768,522,509,- -32768,3146,- -32768,- -32768,- -32768,- -32768,3674,-
-32768,- -32768,332,230,1604,- -32768,3228,590,- -32768,3455,3382,4307,- -32768,285,
- -32768,- -32768,- -32768,526,535,- -32768,- -32768,- -32768,- -32768,47,803,3455,-
-32768,2865,- -32768,545,556,560,216,75,- -32768,3082,97,1282,6064,553,5937,1242,
569,571,592,603,635,642,657,661,6113,6113,- -32768,- -32768,- -32768,2171,663,6162,
6162,6162,- -32768,- -32768,- -32768,- 16,- -32768,- -32768,25,623,641,676,680,95,99,
170,309,291,- -32768,1066,6162,114,100,- -32768,709,914,725,- -32768,- -32768,728,
5937,332,914,712,220,3674,3747,3674,637,- -32768,- 7,- 7,- -32768,- -32768,- 6,723,205,
332,- -32768,- -32768,- -32768,- -32768,68,726,- -32768,- -32768,815,244,- -32768,740,-
-32768,748,- -32768,753,768,785,- -32768,565,4405,3228,- -32768,780,3674,- -32768,
865,792,216,795,794,194,802,3817,809,832,823,824,4503,3817,249,820,778,- -32768,-
-32768,828,1747,1747,590,1747,- -32768,- -32768,- -32768,834,- -32768,- -32768,-
-32768,162,- -32768,- -32768,833,821,- 25,853,- -32768,27,835,837,299,838,235,831,
3455,5937,- -32768,855,- -32768,- -32768,- 25,849,216,- -32768,5937,854,- -32768,851,
852,- -32768,215,5937,3082,440,- -32768,- -32768,- -32768,862,863,2171,- -32768,3674,
- -32768,- -32768,3915,- -32768,882,5937,5937,5937,5937,5937,5937,3674,1109,2171,-
-32768,- -32768,1890,857,281,5937,- -32768,- -32768,- -32768,5937,- -32768,6162,5937,
6162,6162,6162,6162,6162,6162,6162,6162,6162,6162,6162,6162,6162,6162,6162,6162,
6162,5937,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,5937,- -32768,215,- -32768,- -32768,4598,215,5937,- -32768,
864,332,- -32768,- -32768,859,868,3082,- -32768,383,1282,860,3455,- -32768,871,873,-
-32768,3747,3747,3747,- -32768,- -32768,3012,4693,125,- -32768,319,- -32768,- -32768,
27,- -32768,- -32768,3455,- -32768,886,- -32768,876,- -32768,872,875,881,- -32768,2939,
- -32768,396,154,- -32768,- -32768,- -32768,3674,- -32768,- -32768,- -32768,2592,-
-32768,2592,901,889,884,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
707,5937,893,891,- -32768,885,543,332,332,568,5937,5937,888,896,5937,988,2033,900,
- -32768,- -32768,- -32768,392,981,- -32768,4791,5937,3817,2309,- -32768,- -32768,3146,
- -32768,- -32768,- -32768,3455,- -32768,- -32768,3674,902,231,- -32768,- -32768,890,-
-32768,27,903,3601,837,- -32768,- -32768,- -32768,- -32768,235,112,904,870,- -32768,-
-32768,2455,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,899,- -32768,
- -32768,- -32768,- -32768,6247,906,913,930,- -32768,- -32768,502,4405,330,915,917,
919,382,453,918,193,920,923,5986,- -32768,- -32768,922,928,- -32768,623,228,641,676,
680,95,99,99,170,170,170,170,309,309,291,291,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,924,- -32768,121,3455,4209,- -32768,6286,- -32768,- -32768,
932,- -32768,136,943,- -32768,- -32768,- -32768,- -32768,931,837,933,235,926,319,3455,
3528,4886,- -32768,- -32768,- 7,934,332,789,937,68,1071,938,332,3455,3382,4984,-
-32768,396,- -32768,- -32768,- -32768,935,- -32768,961,1077,- -32768,- -32768,865,5937,
837,- -32768,- -32768,- -32768,947,216,524,466,478,5937,788,517,944,5082,5177,554,-
-32768,- -32768,949,953,948,541,- -32768,3228,- -32768,821,957,3455,- -32768,956,27,
- -32768,968,- -32768,183,- -32768,- -32768,- -32768,- -32768,870,- -32768,958,- -32768,
2592,- -32768,- -32768,- -32768,974,347,971,- -32768,4013,- -32768,- -32768,5937,1073,
5937,1242,- -32768,- -32768,215,215,974,969,4209,- -32768,- -32768,5937,5937,- -32768,
- -32768,- 25,729,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,972,- -32768,-
-32768,235,- 25,975,837,235,970,- -32768,5937,837,567,977,978,- -32768,- -32768,979,
- -32768,- 25,980,837,982,235,983,5937,991,- -32768,- -32768,3674,- -32768,985,48,568,
3817,992,986,1119,984,994,3817,5937,5272,588,5367,609,5462,788,- -32768,1000,1004,
- -32768,837,178,- -32768,- -32768,1011,837,3674,- -32768,375,- -32768,999,1003,5937,
- -32768,- -32768,4405,581,1005,1012,- -32768,882,1006,1008,- -32768,736,- -32768,-
-32768,- -32768,- -32768,4209,- -32768,- -32768,- -32768,- -32768,1010,- -32768,235,622,
3674,1031,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1013,- -32768,- -32768,
235,584,- -32768,1020,3674,- -32768,- -32768,1114,788,1026,6211,1022,2309,6162,1019,
- -32768,41,- -32768,1061,963,724,782,103,797,480,467,- -32768,- -32768,- -32768,-
-32768,1062,6162,1890,- -32768,- -32768,605,3817,613,5557,3817,615,5652,5747,697,
1033,- -32768,- -32768,- -32768,- -32768,1034,- -32768,957,- -32768,1037,706,- -32768,-
-32768,250,- -32768,- -32768,3674,1130,- -32768,- -32768,- -32768,4111,- -32768,-
-32768,- -32768,1049,1050,- -32768,- -32768,- -32768,- -32768,638,3817,1053,788,2171,
- -32768,3674,1044,- -32768,1461,6162,5937,6162,6162,6162,6162,6162,6162,6162,6162,
6162,6162,6162,6162,6162,6162,6162,6162,6162,5937,- -32768,- -32768,1056,- -32768,
3817,3817,640,- -32768,3817,3817,653,3817,659,5842,- -32768,27,837,- -32768,- -32768,
426,5937,1051,1054,- -32768,- -32768,1069,- -32768,- -32768,- -32768,1058,1055,1059,
6162,788,- -32768,623,268,641,676,676,95,99,99,170,170,170,170,309,309,291,291,-
-32768,- -32768,- -32768,270,- -32768,- -32768,- -32768,3817,- -32768,- -32768,3817,-
-32768,3817,3817,685,- -32768,- -32768,- -32768,805,1067,3674,1068,- -32768,974,393,
- -32768,- -32768,5937,1461,- -32768,- -32768,- -32768,- -32768,3817,- -32768,- -32768,
1074,- -32768,1075,1081,- -32768,788,- -32768,- -32768,1082,- -32768,- -32768,- -32768,
- -32768,1198,1214,- -32768};static short Cyc_yypgoto[143]={- -32768,214,- -32768,276,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 46,- -32768,- 113,16,-
-32768,- -32768,0,601,- -32768,91,- 186,1122,5,- -32768,- -32768,- 121,- -32768,159,
1193,- 698,- -32768,- -32768,- -32768,973,976,619,481,- -32768,- -32768,597,- -32768,-
-32768,180,- -32768,- -32768,80,- 77,1170,- 229,195,- -32768,1084,- -32768,- -32768,
1196,- 416,- -32768,570,- 115,- 133,- 143,175,307,574,580,- 426,- 469,- 114,- 412,- 106,-
-32768,- 258,- 149,- 543,- 319,- -32768,929,- 184,78,- 45,- 30,- 286,- 22,- -32768,- -32768,
- -32768,- 59,- 255,- -32768,- 304,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,8,1030,- -32768,716,847,- -32768,- 8,576,- -32768,- 165,- 296,- 140,- 359,
- 353,- 346,895,- 351,- 344,- 363,- 330,- 309,- 175,422,508,925,- -32768,- 357,- -32768,179,
415,- 61,- 10,- 289,45};static short Cyc_yytable[6427]={59,250,308,465,136,62,358,566,
616,143,341,574,575,576,577,568,262,361,362,363,571,59,140,569,549,317,62,344,
572,573,307,96,499,500,537,502,345,78,398,310,536,671,631,423,112,589,324,136,
578,579,714,59,825,644,645,137,62,38,59,59,59,432,59,62,62,62,41,62,59,147,263,
580,581,62,130,131,132,44,133,432,432,198,300,140,142,442,224,76,509,674,169,60,
225,288,419,299,668,505,240,239,433,220,559,59,59,631,364,678,62,62,587,365,60,
891,590,319,437,366,433,433,59,59,59,59,113,62,62,62,62,59,434,434,719,907,62,19,
20,129,662,59,39,59,60,611,62,142,62,510,147,60,60,60,100,60,543,289,544,142,531,
60,39,367,59,797,611,531,518,62,556,- 150,558,257,448,511,443,315,638,908,325,233,
234,170,359,237,372,373,374,375,453,258,915,916,246,723,60,60,399,400,401,171,
530,430,97,468,405,101,453,582,583,584,326,60,60,60,60,414,299,120,121,451,60,
630,301,376,377,327,409,917,918,422,60,102,60,409,424,406,423,423,423,679,402,
288,288,364,288,403,404,172,630,434,38,427,486,60,712,378,379,428,493,41,38,97,
59,718,79,119,149,62,40,41,42,492,97,434,549,702,400,401,142,436,447,504,438,103,
364,104,428,59,59,507,59,694,62,62,757,62,268,270,520,521,38,511,306,289,289,870,
289,531,804,41,- 215,288,808,- 215,105,434,527,235,236,402,482,241,38,483,703,404,
106,364,821,242,359,41,440,39,253,254,255,256,550,538,301,418,554,555,39,749,359,
170,506,97,364,97,670,60,160,122,123,59,710,567,- 237,269,62,- 237,528,171,454,873,
494,501,624,455,364,289,788,495,320,419,60,60,947,60,511,415,250,707,625,549,665,
453,364,524,364,889,624,107,591,97,1005,441,1006,784,43,364,97,109,894,110,561,
144,453,145,111,794,795,74,453,146,306,114,596,517,422,422,422,382,429,424,424,
424,115,615,116,383,384,787,43,80,98,124,99,656,666,612,450,613,380,623,60,629,
381,614,364,241,783,140,160,695,59,74,59,242,477,62,866,62,74,163,419,162,727,
637,74,649,650,664,531,653,167,657,135,38,138,788,742,74,218,59,663,40,41,42,62,
219,166,600,874,98,726,241,1016,59,597,142,364,364,62,242,98,699,97,659,741,43,
364,617,674,142,74,74,626,743,627,135,221,773,899,74,628,59,880,681,38,778,62,74,
74,74,74,511,40,41,42,222,787,799,738,713,962,60,792,60,970,971,972,973,964,74,
223,793,738,967,625,539,965,966,593,725,788,968,969,329,330,644,645,364,453,74,
60,453,700,740,166,667,238,98,245,98,364,974,975,97,60,755,97,339,956,342,342,
928,364,923,961,716,247,756,772,355,356,924,925,615,976,977,342,342,342,921,38,
299,60,922,429,692,693,38,40,41,42,787,629,260,134,98,41,342,43,642,643,364,98,
259,450,753,761,44,754,764,766,38,904,960,313,47,48,49,1004,40,41,42,905,314,730,
364,452,59,477,321,771,471,62,38,632,767,633,74,364,322,927,340,41,142,452,323,
479,59,681,484,789,812,62,44,252,677,813,346,38,347,948,47,48,49,1020,364,74,41,
364,860,881,125,364,895,897,1017,368,1025,44,957,348,958,136,97,312,810,47,48,49,
364,863,349,943,364,929,829,249,364,166,364,823,856,931,890,935,684,364,342,98,
774,978,979,980,1019,342,267,857,859,60,862,800,865,454,350,364,427,249,953,852,
985,351,428,805,47,48,49,97,364,60,879,732,369,988,364,721,352,1003,817,990,353,
342,360,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,364,
288,511,758,759,1011,38,1014,370,940,750,477,364,371,40,41,42,43,945,198,98,946,
407,98,412,477,471,413,477,930,467,417,934,1018,47,48,49,47,48,49,421,439,426,
801,802,933,446,59,937,939,886,887,62,288,456,452,911,912,452,150,151,152,457,
289,913,914,74,459,74,954,153,154,155,156,157,919,920,267,136,452,16,17,18,525,
461,452,359,342,733,734,735,463,782,963,469,807,603,604,605,59,811,497,983,984,
62,478,986,987,480,989,819,981,467,481,38,289,551,552,553,1012,364,485,40,41,42,
992,827,828,487,562,97,488,996,565,447,489,490,74,60,496,428,869,47,48,49,81,872,
498,503,261,98,434,507,288,585,514,1007,523,519,1008,515,1009,1010,529,38,43,586,
366,533,534,535,562,40,41,42,342,541,560,542,594,599,592,1021,595,471,618,601,
602,472,619,60,620,47,48,49,621,622,610,82,59,634,635,636,98,62,639,640,651,641,
652,408,654,658,168,84,289,660,85,86,87,672,669,41,675,680,686,689,88,690,691,
696,452,697,89,698,90,91,705,701,249,704,92,708,709,711,452,717,93,452,39,731,
720,724,722,746,752,342,736,739,249,747,751,768,762,562,769,249,562,770,306,775,
781,7,8,9,10,11,12,13,14,15,16,17,18,19,20,777,706,785,796,60,790,803,74,910,806,
809,814,815,816,818,22,820,824,826,830,831,854,855,822,249,994,867,27,342,385,
868,871,467,877,249,28,29,878,882,342,884,883,885,249,888,892,32,893,896,898,34,
832,833,900,81,903,906,35,36,909,926,941,949,942,81,944,386,387,388,389,390,391,
392,393,394,395,467,951,959,952,98,955,982,999,38,997,1000,998,1002,1001,38,39,
40,41,42,43,396,729,40,41,42,1013,1015,135,471,1028,45,46,1022,1023,471,745,47,
48,49,1024,1026,82,47,48,49,1029,38,84,187,188,85,86,87,995,737,41,168,84,562,
108,85,86,87,249,464,41,354,248,748,191,88,148,470,421,318,118,834,249,90,91,993,
779,780,776,92,902,416,835,342,688,93,195,540,598,196,197,570,853,173,174,467,0,
0,0,0,791,342,0,0,557,0,81,0,467,0,0,798,0,0,0,0,0,0,0,0,177,178,179,180,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,81,0,0,0,0,185,186,81,0,342,0,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,38,84,187,188,85,86,
87,0,0,41,0,0,189,135,343,0,467,0,0,0,190,267,0,191,0,82,0,0,192,0,467,193,82,
342,194,195,0,328,196,197,168,84,0,0,85,86,87,83,84,41,249,85,86,87,88,0,41,0,0,
0,89,88,90,91,0,329,330,89,92,90,91,0,0,0,93,92,0,0,0,342,0,93,267,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,249,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,467,2,3,4,77,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,758,759,21,173,174,271,0,272,273,274,275,276,277,278,
279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,
282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,-
303,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,
277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,
181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,
139,285,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,
77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,
276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,
29,- 37,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,
284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,
77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,
276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,
29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,
284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,
77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,0,0,0,0,0,0,0,0,0,0,
22,81,23,175,0,0,0,176,24,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,
0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,655,0,0,0,0,0,0,190,0,45,
286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,
178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,
185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,
41,42,43,189,0,357,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,
196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,
22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,
32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,0,0,0,0,0,0,190,0,45,286,0,
0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,683,0,24,0,25,26,
27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,
0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,
0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,
- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,
0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,
0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,
0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,22,0,39,40,41,42,43,
0,0,0,- 11,0,27,0,0,44,0,45,46,0,28,29,0,47,48,49,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,
0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,38,0,0,0,
22,0,0,39,40,41,42,43,0,306,0,0,27,0,0,0,447,0,45,46,28,29,428,0,47,48,49,0,0,0,
0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,304,
0,0,0,0,81,0,0,0,0,39,40,0,42,43,0,306,0,0,0,0,0,0,427,0,45,46,0,0,428,0,47,48,
49,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,82,0,0,0,0,0,0,0,
0,0,22,0,23,0,0,168,84,0,24,85,86,87,27,0,41,0,0,0,0,88,28,29,- 37,0,0,89,0,90,91,
0,0,32,33,92,0,34,0,0,0,93,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,261,0,0,0,24,45,46,
0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,
2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,
42,43,22,0,139,0,0,0,0,0,24,0,45,46,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,
0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,
0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
0,0,0,0,0,0,304,0,0,305,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,0,0,45,46,
28,29,0,0,226,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,227,0,27,0,
0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,
0,306,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,
36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,676,0,0,0,0,0,0,22,0,0,
39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,
0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,
0,34,0,0,0,0,0,0,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,271,0,272,273,274,275,
276,277,278,279,0,81,0,175,280,0,39,176,0,281,43,0,0,0,0,177,178,179,180,0,0,45,
46,181,0,282,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,0,0,41,0,0,189,284,139,0,0,
173,174,0,190,0,0,191,545,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,
177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,
185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,
0,0,189,0,343,546,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,
176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,
184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
187,188,85,86,87,0,0,41,0,0,189,0,343,786,0,173,174,0,190,0,0,191,0,329,330,0,
192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,
0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,950,0,173,174,0,190,
0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,
180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,
0,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,
0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,
0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,
41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,311,192,0,81,193,175,0,194,195,
176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,
184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,0,0,0,192,0,81,
193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,
0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,491,173,174,0,0,0,0,190,0,0,191,0,
0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,
0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,588,
0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,
181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,
190,0,0,191,0,0,0,609,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,
179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,
173,174,0,0,0,0,190,661,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,
177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,
185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,
0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,728,192,0,81,193,175,0,194,195,176,0,
196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,
0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,
86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,744,192,0,81,193,175,0,
194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,
183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,187,188,85,86,87,0,0,41,0,0,189,763,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,
175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,
0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,765,173,174,0,0,0,0,190,0,0,191,0,0,
0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,
0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,858,0,
191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,
181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,
190,861,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,
0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,864,173,
174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,
178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,
186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,
189,0,173,174,0,0,0,0,190,932,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,
197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,
0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,
41,0,0,189,0,173,174,0,0,0,0,190,936,0,191,0,0,0,81,192,175,0,193,0,176,194,195,
0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,
0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,
86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,938,0,191,0,0,0,81,192,175,0,193,0,
176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,
0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,991,0,191,0,0,0,81,192,175,
0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,
182,183,0,184,173,174,0,0,0,0,0,0,0,185,186,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,177,
178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,0,0,0,190,0,0,
191,0,0,185,186,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,0,0,173,174,0,38,84,
187,188,85,86,87,0,0,41,81,0,189,0,706,0,0,0,0,0,190,0,0,191,177,178,179,180,192,
0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,173,174,0,0,0,0,0,0,0,185,186,
0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,
0,0,189,182,183,0,184,173,174,0,338,0,0,191,0,0,185,186,192,0,81,193,0,0,194,195,
0,0,196,197,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,
183,0,184,173,174,0,354,0,0,191,0,0,185,186,192,0,81,193,0,0,194,195,0,0,196,197,
0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,81,
0,0,190,0,0,191,0,0,185,186,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,0,0,0,0,
0,38,84,187,188,85,86,87,81,0,41,0,0,189,0,0,0,0,0,0,0,901,82,0,191,0,0,0,0,192,
0,0,193,0,687,194,195,168,84,196,197,85,86,87,0,0,41,0,0,0,0,88,0,0,0,0,0,89,0,
90,91,82,329,330,0,92,0,0,0,0,0,93,0,715,0,0,168,84,0,0,85,86,87,0,0,41,0,0,0,0,
88,0,0,0,0,0,89,0,90,91,0,0,0,0,92,0,0,0,0,0,93};static short Cyc_yycheck[6427]={0,
115,145,261,65,0,190,366,434,68,175,374,375,376,377,368,129,192,193,194,371,21,
68,369,343,158,21,176,372,373,145,23,287,288,330,290,176,21,213,145,329,510,454,
229,44,402,167,108,378,379,593,51,750,5,6,65,51,106,58,59,60,86,62,58,59,60,115,
62,68,69,129,380,381,68,58,59,60,126,62,86,86,89,141,129,68,17,120,108,61,515,82,
0,126,139,227,141,508,122,108,107,125,93,357,103,104,517,122,523,103,104,399,127,
21,811,403,160,122,92,125,125,120,121,122,123,44,120,121,122,123,129,137,137,601,
92,129,20,21,57,495,139,113,141,51,429,139,129,141,120,148,58,59,60,106,62,338,
139,340,141,323,68,113,136,162,706,450,330,309,162,352,121,354,106,249,306,106,
128,472,136,170,101,102,106,190,105,89,90,87,88,249,124,87,88,112,609,103,104,82,
83,84,124,321,231,23,262,104,124,267,382,383,384,113,120,121,122,123,223,262,119,
120,249,129,450,142,124,125,128,218,124,125,229,139,124,141,225,229,135,422,423,
424,127,126,287,288,122,290,131,132,88,477,137,106,126,274,162,133,85,86,132,280,
115,106,82,262,127,120,51,71,262,114,115,116,279,93,137,593,82,83,84,262,234,126,
119,237,119,122,119,132,287,288,106,290,549,287,288,651,290,137,138,63,64,106,
434,119,287,288,127,290,472,724,115,127,357,728,130,124,137,316,103,104,126,126,
106,106,129,131,132,124,122,744,114,338,115,127,113,120,121,122,123,346,331,260,
121,350,351,113,641,354,106,303,170,122,172,121,262,124,119,120,357,130,367,119,
131,357,122,319,124,122,779,119,289,447,127,122,357,693,126,162,510,287,288,130,
290,515,224,498,560,447,706,501,450,122,313,122,809,471,126,404,218,130,240,130,
690,117,122,225,49,822,105,127,124,471,126,126,702,703,0,477,132,119,105,412,122,
422,423,424,134,231,422,423,424,126,431,120,142,143,693,117,22,23,59,25,487,501,
124,249,126,137,447,357,449,141,132,122,106,107,501,124,127,458,44,460,114,267,
458,768,460,51,108,601,120,613,471,57,481,482,497,641,485,120,487,65,106,67,802,
627,70,126,487,496,114,115,116,487,106,79,417,121,82,613,106,107,501,119,487,122,
122,501,114,93,127,331,119,627,117,122,437,942,501,103,104,124,627,126,108,119,
668,830,112,132,529,788,526,106,676,529,120,121,122,123,672,114,115,116,122,802,
710,623,592,907,458,699,460,915,916,917,918,909,142,123,699,637,912,623,123,910,
911,407,612,887,913,914,131,132,5,6,122,637,162,487,640,127,626,167,505,113,170,
126,172,122,919,920,412,501,127,415,173,900,175,176,854,122,134,906,595,127,127,
665,185,186,142,143,611,921,922,192,193,194,137,106,665,529,141,427,121,122,106,
114,115,116,887,630,122,120,218,115,213,117,478,479,122,225,119,447,119,127,126,
122,655,656,106,834,906,126,134,135,136,960,114,115,116,835,126,617,122,249,665,
471,122,127,126,665,106,458,119,460,260,122,121,853,126,115,665,267,123,269,685,
681,272,696,122,685,126,116,518,127,126,106,126,882,134,135,136,1006,122,289,115,
122,119,127,119,122,127,826,1002,91,1019,126,901,126,903,781,542,146,731,134,135,
136,122,119,126,873,122,127,755,115,122,321,122,746,761,127,119,127,529,122,323,
331,669,923,924,925,1006,330,134,762,763,665,765,713,767,122,126,122,126,145,127,
758,127,126,132,725,134,135,136,595,122,685,785,619,138,127,122,607,126,959,740,
127,126,366,126,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,
384,122,854,942,22,23,127,106,998,139,119,642,623,122,140,114,115,116,117,119,
834,412,122,120,415,106,637,126,106,640,858,261,126,861,1005,134,135,136,134,135,
136,228,125,230,121,122,860,127,854,863,864,121,122,854,906,121,447,139,140,450,
63,64,65,121,854,89,90,458,121,460,898,74,75,76,77,78,85,86,265,946,471,17,18,19,
314,123,477,901,472,106,107,108,113,685,908,121,727,422,423,424,906,732,130,931,
932,906,120,935,936,120,938,742,926,343,126,106,906,347,348,349,121,122,126,114,
115,116,940,753,754,126,360,758,106,947,364,126,119,119,529,854,126,132,773,134,
135,136,38,778,126,121,123,542,137,106,1006,385,127,985,133,127,988,130,990,991,
121,106,117,397,92,121,125,125,402,114,115,116,560,121,127,122,127,127,124,1011,
122,126,106,122,121,130,120,906,126,134,135,136,127,122,428,91,1006,106,119,125,
595,1006,119,122,126,130,120,103,30,119,106,107,1006,42,110,111,112,137,126,115,
127,127,133,127,120,122,106,122,623,122,126,122,128,129,121,127,427,127,134,127,
122,127,637,121,140,640,113,123,127,133,127,126,647,641,127,127,447,106,121,120,
126,495,119,454,498,127,119,121,120,8,9,10,11,12,13,14,15,16,17,18,19,20,21,125,
120,124,127,1006,25,127,685,138,127,133,127,127,127,127,37,127,119,126,120,127,
130,121,133,498,943,119,49,699,56,119,113,549,127,508,57,58,127,126,710,127,122,
127,517,127,107,68,127,121,28,72,25,26,120,38,126,130,79,80,91,91,121,25,122,38,
121,93,94,95,96,97,98,99,100,101,102,593,121,127,122,758,121,119,107,106,127,121,
126,122,127,106,113,114,115,116,117,123,614,114,115,116,127,127,781,126,0,128,
129,127,127,126,628,134,135,136,127,127,91,134,135,136,0,106,107,108,109,110,111,
112,946,622,115,106,107,651,35,110,111,112,613,260,115,126,114,640,129,120,70,
265,623,159,48,126,627,128,129,942,676,681,672,134,832,225,137,835,542,140,141,
332,415,144,145,370,758,25,26,693,- 1,- 1,- 1,- 1,698,853,- 1,- 1,353,- 1,38,- 1,706,- 1,
- 1,709,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,83,84,38,- 1,907,- 1,909,910,911,912,
913,914,915,916,917,918,919,920,921,922,923,924,925,106,107,108,109,110,111,112,
- 1,- 1,115,- 1,- 1,118,946,120,- 1,788,- 1,- 1,- 1,126,750,- 1,129,- 1,91,- 1,- 1,134,- 1,
802,137,91,959,140,141,- 1,103,144,145,106,107,- 1,- 1,110,111,112,106,107,115,779,
110,111,112,120,- 1,115,- 1,- 1,- 1,126,120,128,129,- 1,131,132,126,134,128,129,- 1,- 1,
- 1,140,134,- 1,- 1,- 1,1005,- 1,140,811,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
826,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,887,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,
39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,
- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,
116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,
- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,
- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,
- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,
126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,
36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,
- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,
114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,
137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,
- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,
- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,
- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,
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
- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,115,116,117,-
1,119,- 1,- 1,49,- 1,- 1,- 1,126,- 1,128,129,57,58,132,- 1,134,135,136,- 1,- 1,- 1,- 1,68,-
1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,
- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,132,- 1,134,135,136,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,
- 1,106,107,- 1,45,110,111,112,49,- 1,115,- 1,- 1,- 1,- 1,120,57,58,59,- 1,- 1,126,- 1,128,
129,- 1,- 1,68,69,134,- 1,72,- 1,- 1,- 1,140,- 1,- 1,79,80,81,- 1,- 1,- 1,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,-
1,37,120,39,- 1,123,- 1,- 1,- 1,45,128,129,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,
117,37,- 1,120,- 1,- 1,- 1,- 1,- 1,45,- 1,128,129,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,
- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,
119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,
72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,120,- 1,49,- 1,-
1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,-
1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,-
1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,
129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,-
1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,
114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,113,44,- 1,46,117,- 1,
- 1,- 1,- 1,52,53,54,55,- 1,- 1,128,129,60,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,120,- 1,- 1,25,26,
- 1,126,- 1,- 1,129,32,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,
- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,
- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,
54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,
107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,
- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,
- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,
109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,
132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,-
1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,
110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,-
1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,
60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,
112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,
38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,-
1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,38,137,40,
- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,
- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,
141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,
- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,
- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,
52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,
129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,
108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,-
1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,
112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,
38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,-
1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,
- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,
- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,
141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,
- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,-
1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,-
1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,
- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,-
1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,
84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,
109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,
38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,-
1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,
44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,
25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,
144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,
126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,
108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,-
1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,
112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,-
1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,70,71,- 1,73,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,
- 1,118,70,71,- 1,73,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,- 1,137,- 1,- 1,140,
141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,
115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,-
1,140,141,- 1,- 1,144,145,- 1,- 1,70,71,- 1,73,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,
38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,
112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,25,26,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,38,
137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,
111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,25,26,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,-
1,38,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,
110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,38,- 1,- 1,126,- 1,- 1,129,- 1,- 1,83,84,
134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,
109,110,111,112,38,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,91,- 1,129,- 1,- 1,- 1,
- 1,134,- 1,- 1,137,- 1,103,140,141,106,107,144,145,110,111,112,- 1,- 1,115,- 1,- 1,- 1,-
1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,91,131,132,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,103,
- 1,- 1,106,107,- 1,- 1,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,
128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140};char Cyc_Yystack_overflow[17]="Yystack_overflow";
struct Cyc_Yystack_overflow_struct{char*tag;int f1;};struct Cyc_Yystack_overflow_struct
Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};void Cyc_yyerror(struct
_dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(int)'\000';
union Cyc_YYSTYPE Cyc_yylval={.YYINITIALSVAL={61,0}};static int Cyc_yynerrs=0;struct
_tuple24{struct Cyc_Position_Segment*f1;struct _tuple1*f2;int f3;};struct _tuple25{
struct _dyneither_ptr f1;void*f2;};static char _tmp51B[8]="stdcall";static char
_tmp51C[6]="cdecl";static char _tmp51D[9]="fastcall";static char _tmp51E[9]="noreturn";
static char _tmp51F[6]="const";static char _tmp520[8]="aligned";static char _tmp521[7]="packed";
static char _tmp522[7]="shared";static char _tmp523[7]="unused";static char _tmp524[5]="weak";
static char _tmp525[10]="dllimport";static char _tmp526[10]="dllexport";static char
_tmp527[23]="no_instrument_function";static char _tmp528[12]="constructor";static
char _tmp529[11]="destructor";static char _tmp52A[22]="no_check_memory_usage";
static char _tmp52B[5]="pure";struct _tuple26{void*f1;void*f2;};struct _tuple27{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*
yylex_buf);static void _tmpA11(unsigned int*_tmpA10,unsigned int*_tmpA0F,union Cyc_YYSTYPE**
_tmpA0E){for(*_tmpA10=0;*_tmpA10 < *_tmpA0F;(*_tmpA10)++){(*_tmpA0E)[*_tmpA10]=
Cyc_yylval;}}static void _tmpA16(unsigned int*_tmpA15,unsigned int*_tmpA14,struct
Cyc_Yyltype**_tmpA13){for(*_tmpA15=0;*_tmpA15 < *_tmpA14;(*_tmpA15)++){(*_tmpA13)[*
_tmpA15]=Cyc_yynewloc();}}static void _tmpA1D(int*yyssp_offset,struct
_dyneither_ptr*yyss,unsigned int*_tmpA1C,unsigned int*_tmpA1B,short**_tmpA19){
for(*_tmpA1C=0;*_tmpA1C < *_tmpA1B;(*_tmpA1C)++){(*_tmpA19)[*_tmpA1C]=*_tmpA1C <= *
yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*
_tmpA1C)): 0;}}static void _tmpA23(struct _dyneither_ptr*yyvs,int*yyssp_offset,
unsigned int*_tmpA22,unsigned int*_tmpA21,union Cyc_YYSTYPE**_tmpA1F){for(*
_tmpA22=0;*_tmpA22 < *_tmpA21;(*_tmpA22)++){(*_tmpA1F)[*_tmpA22]=*_tmpA22 <= *
yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union
Cyc_YYSTYPE),(int)*_tmpA22)): Cyc_yylval;}}static void _tmpA29(int*yyssp_offset,
struct _dyneither_ptr*yyls,unsigned int*_tmpA28,unsigned int*_tmpA27,struct Cyc_Yyltype**
_tmpA25){for(*_tmpA28=0;*_tmpA28 < *_tmpA27;(*_tmpA28)++){(*_tmpA25)[*_tmpA28]=*
_tmpA28 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,
sizeof(struct Cyc_Yyltype),(int)*_tmpA28)): Cyc_yynewloc();}}static void _tmpF19(
unsigned int*_tmpF18,unsigned int*_tmpF17,char**_tmpF15){for(*_tmpF18=0;*_tmpF18
< *_tmpF17;(*_tmpF18)++){(*_tmpF15)[*_tmpF18]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*
yylex_buf){struct _RegionHandle _tmp473=_new_region("yyregion");struct
_RegionHandle*yyregion=& _tmp473;_push_region(yyregion);{int yystate;int yyn=0;int
yyerrstatus;int yychar1=0;int yyssp_offset;short*_tmpA0C;unsigned int _tmpA0B;
struct _dyneither_ptr yyss=(_tmpA0B=200,((_tmpA0C=_region_calloc(yyregion,sizeof(
short),_tmpA0B),_tag_dyneither(_tmpA0C,sizeof(short),_tmpA0B))));int yyvsp_offset;
unsigned int _tmpA10;unsigned int _tmpA0F;union Cyc_YYSTYPE*_tmpA0E;unsigned int
_tmpA0D;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmpA0D=(unsigned int)200,((
_tmpA0E=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),
_tmpA0D)),((((_tmpA0F=_tmpA0D,_tmpA11(& _tmpA10,& _tmpA0F,& _tmpA0E))),_tmpA0E)))))),
sizeof(union Cyc_YYSTYPE),(unsigned int)200);int yylsp_offset;unsigned int _tmpA15;
unsigned int _tmpA14;struct Cyc_Yyltype*_tmpA13;unsigned int _tmpA12;struct
_dyneither_ptr yyls=_tag_dyneither(((_tmpA12=(unsigned int)200,((_tmpA13=(struct
Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),
_tmpA12)),((((_tmpA14=_tmpA12,_tmpA16(& _tmpA15,& _tmpA14,& _tmpA13))),_tmpA13)))))),
sizeof(struct Cyc_Yyltype),(unsigned int)200);int yystacksize=200;union Cyc_YYSTYPE
yyval=Cyc_yylval;int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;
yyssp_offset=- 1;yyvsp_offset=0;yylsp_offset=0;yynewstate:*((short*)
_check_dyneither_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 10000){{const char*_tmpA17;Cyc_yyerror(((
_tmpA17="parser stack overflow",_tag_dyneither(_tmpA17,sizeof(char),22))));}(int)
_throw((void*)& Cyc_Yystack_overflow_val);}yystacksize *=2;if(yystacksize > 10000)
yystacksize=10000;{unsigned int _tmpA1C;unsigned int _tmpA1B;struct _dyneither_ptr
_tmpA1A;short*_tmpA19;unsigned int _tmpA18;struct _dyneither_ptr yyss1=(_tmpA18=(
unsigned int)yystacksize,((_tmpA19=(short*)_region_malloc(yyregion,_check_times(
sizeof(short),_tmpA18)),((_tmpA1A=_tag_dyneither(_tmpA19,sizeof(short),_tmpA18),((((
_tmpA1B=_tmpA18,_tmpA1D(& yyssp_offset,& yyss,& _tmpA1C,& _tmpA1B,& _tmpA19))),
_tmpA1A)))))));unsigned int _tmpA22;unsigned int _tmpA21;struct _dyneither_ptr
_tmpA20;union Cyc_YYSTYPE*_tmpA1F;unsigned int _tmpA1E;struct _dyneither_ptr yyvs1=(
_tmpA1E=(unsigned int)yystacksize,((_tmpA1F=(union Cyc_YYSTYPE*)_region_malloc(
yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA1E)),((_tmpA20=
_tag_dyneither(_tmpA1F,sizeof(union Cyc_YYSTYPE),_tmpA1E),((((_tmpA21=_tmpA1E,
_tmpA23(& yyvs,& yyssp_offset,& _tmpA22,& _tmpA21,& _tmpA1F))),_tmpA20)))))));
unsigned int _tmpA28;unsigned int _tmpA27;struct _dyneither_ptr _tmpA26;struct Cyc_Yyltype*
_tmpA25;unsigned int _tmpA24;struct _dyneither_ptr yyls1=(_tmpA24=(unsigned int)
yystacksize,((_tmpA25=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(
sizeof(struct Cyc_Yyltype),_tmpA24)),((_tmpA26=_tag_dyneither(_tmpA25,sizeof(
struct Cyc_Yyltype),_tmpA24),((((_tmpA27=_tmpA24,_tmpA29(& yyssp_offset,& yyls,&
_tmpA28,& _tmpA27,& _tmpA25))),_tmpA26)))))));yyss=yyss1;yyvs=yyvs1;yyls=yyls1;};}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1030,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 373?(int)Cyc_yytranslate[_check_known_subscript_notnull(374,Cyc_yychar)]:
289;}yyn +=yychar1;if((yyn < 0  || yyn > 6426) || Cyc_yycheck[
_check_known_subscript_notnull(6427,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6427,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1029){int
_tmp481=0;_npop_handler(0);return _tmp481;}if(Cyc_yychar != 0)Cyc_yychar=- 2;*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++
yyvsp_offset))=Cyc_yylval;*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,
sizeof(struct Cyc_Yyltype),++ yylsp_offset))=Cyc_yylloc;if(yyerrstatus != 0)--
yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1030,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(515,yyn)];if(yylen > 0)yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(
yyvsp_offset + 1)- yylen));switch(yyn){case 1: _LL215: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));Cyc_Parse_parse_result=
Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)));break;case 2: _LL216: {struct Cyc_List_List*x;struct
Cyc_List_List*y;{struct _handler_cons _tmp482;_push_handler(& _tmp482);{int _tmp484=
0;if(setjmp(_tmp482.handler))_tmp484=1;if(!_tmp484){x=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));;
_pop_handler();}else{void*_tmp483=(void*)_exn_thrown;void*_tmp486=_tmp483;_LL219: {
struct Cyc_Core_Failure_struct*_tmp487=(struct Cyc_Core_Failure_struct*)_tmp486;
if(_tmp487->tag != Cyc_Core_Failure)goto _LL21B;}_LL21A: x=0;goto _LL218;_LL21B:;
_LL21C:(void)_throw(_tmp486);_LL218:;}};}{struct _handler_cons _tmp488;
_push_handler(& _tmp488);{int _tmp48A=0;if(setjmp(_tmp488.handler))_tmp48A=1;if(!
_tmp48A){y=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset)));;_pop_handler();}else{void*_tmp489=(
void*)_exn_thrown;void*_tmp48C=_tmp489;_LL21E: {struct Cyc_Core_Failure_struct*
_tmp48D=(struct Cyc_Core_Failure_struct*)_tmp48C;if(_tmp48D->tag != Cyc_Core_Failure)
goto _LL220;}_LL21F: y=0;goto _LL21D;_LL220:;_LL221:(void)_throw(_tmp48C);_LL21D:;}};}
yyval=Cyc_YY18(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(x,y));break;}case 3: _LL217:{struct Cyc_Absyn_Decl*_tmpA33;
struct Cyc_Absyn_Using_d_struct _tmpA32;struct Cyc_Absyn_Using_d_struct*_tmpA31;
struct Cyc_List_List*_tmpA30;yyval=Cyc_YY18(((_tmpA30=_cycalloc(sizeof(*_tmpA30)),((
_tmpA30->hd=((_tmpA33=_cycalloc(sizeof(*_tmpA33)),((_tmpA33->r=(void*)((_tmpA31=
_cycalloc(sizeof(*_tmpA31)),((_tmpA31[0]=((_tmpA32.tag=11,((_tmpA32.f1=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpA32.f2=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA32)))))),
_tmpA31)))),((_tmpA33->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpA33)))))),((_tmpA30->tl=0,_tmpA30)))))));}Cyc_Lex_leave_using();
break;case 4: _LL222:{struct Cyc_Absyn_Decl*_tmpA3D;struct Cyc_Absyn_Using_d_struct
_tmpA3C;struct Cyc_Absyn_Using_d_struct*_tmpA3B;struct Cyc_List_List*_tmpA3A;yyval=
Cyc_YY18(((_tmpA3A=_cycalloc(sizeof(*_tmpA3A)),((_tmpA3A->hd=((_tmpA3D=_cycalloc(
sizeof(*_tmpA3D)),((_tmpA3D->r=(void*)((_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((
_tmpA3B[0]=((_tmpA3C.tag=11,((_tmpA3C.f1=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),((
_tmpA3C.f2=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpA3C)))))),_tmpA3B)))),((_tmpA3D->loc=
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line),
_tmpA3D)))))),((_tmpA3A->tl=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA3A)))))));}
break;case 5: _LL223:{struct Cyc_Absyn_Decl*_tmpA4C;struct Cyc_Absyn_Namespace_d_struct
_tmpA4B;struct _dyneither_ptr*_tmpA4A;struct Cyc_Absyn_Namespace_d_struct*_tmpA49;
struct Cyc_List_List*_tmpA48;yyval=Cyc_YY18(((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((
_tmpA48->hd=((_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C->r=(void*)((_tmpA49=
_cycalloc(sizeof(*_tmpA49)),((_tmpA49[0]=((_tmpA4B.tag=10,((_tmpA4B.f1=((_tmpA4A=
_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpA4A)))),((_tmpA4B.f2=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA4B)))))),
_tmpA49)))),((_tmpA4C->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpA4C)))))),((_tmpA48->tl=0,_tmpA48)))))));}Cyc_Lex_leave_namespace();
break;case 6: _LL224: {struct _dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*
y;{struct _handler_cons _tmp49B;_push_handler(& _tmp49B);{int _tmp49D=0;if(setjmp(
_tmp49B.handler))_tmp49D=1;if(!_tmp49D){nspace=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4)));;
_pop_handler();}else{void*_tmp49C=(void*)_exn_thrown;void*_tmp49F=_tmp49C;_LL227: {
struct Cyc_Core_Failure_struct*_tmp4A0=(struct Cyc_Core_Failure_struct*)_tmp49F;
if(_tmp4A0->tag != Cyc_Core_Failure)goto _LL229;}_LL228:{const char*_tmpA4D;nspace=((
_tmpA4D="",_tag_dyneither(_tmpA4D,sizeof(char),1)));}goto _LL226;_LL229:;_LL22A:(
void)_throw(_tmp49F);_LL226:;}};}{struct _handler_cons _tmp4A2;_push_handler(&
_tmp4A2);{int _tmp4A4=0;if(setjmp(_tmp4A2.handler))_tmp4A4=1;if(!_tmp4A4){x=Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2)));;_pop_handler();}else{void*_tmp4A3=(void*)_exn_thrown;void*
_tmp4A6=_tmp4A3;_LL22C: {struct Cyc_Core_Failure_struct*_tmp4A7=(struct Cyc_Core_Failure_struct*)
_tmp4A6;if(_tmp4A7->tag != Cyc_Core_Failure)goto _LL22E;}_LL22D: x=0;goto _LL22B;
_LL22E:;_LL22F:(void)_throw(_tmp4A6);_LL22B:;}};}{struct _handler_cons _tmp4A8;
_push_handler(& _tmp4A8);{int _tmp4AA=0;if(setjmp(_tmp4A8.handler))_tmp4AA=1;if(!
_tmp4AA){y=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset)));;_pop_handler();}else{void*_tmp4A9=(
void*)_exn_thrown;void*_tmp4AC=_tmp4A9;_LL231: {struct Cyc_Core_Failure_struct*
_tmp4AD=(struct Cyc_Core_Failure_struct*)_tmp4AC;if(_tmp4AD->tag != Cyc_Core_Failure)
goto _LL233;}_LL232: y=0;goto _LL230;_LL233:;_LL234:(void)_throw(_tmp4AC);_LL230:;}};}{
struct Cyc_Absyn_Decl*_tmpA5C;struct Cyc_Absyn_Namespace_d_struct _tmpA5B;struct
_dyneither_ptr*_tmpA5A;struct Cyc_Absyn_Namespace_d_struct*_tmpA59;struct Cyc_List_List*
_tmpA58;yyval=Cyc_YY18(((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58->hd=((
_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C->r=(void*)((_tmpA59=_cycalloc(
sizeof(*_tmpA59)),((_tmpA59[0]=((_tmpA5B.tag=10,((_tmpA5B.f1=((_tmpA5A=_cycalloc(
sizeof(*_tmpA5A)),((_tmpA5A[0]=nspace,_tmpA5A)))),((_tmpA5B.f2=x,_tmpA5B)))))),
_tmpA59)))),((_tmpA5C->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),_tmpA5C)))))),((_tmpA58->tl=y,_tmpA58)))))));}
break;}case 7: _LL225:{const char*_tmpA5D;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))),((_tmpA5D="C",_tag_dyneither(_tmpA5D,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmpA67;struct Cyc_Absyn_ExternC_d_struct _tmpA66;struct Cyc_Absyn_ExternC_d_struct*
_tmpA65;struct Cyc_List_List*_tmpA64;yyval=Cyc_YY18(((_tmpA64=_cycalloc(sizeof(*
_tmpA64)),((_tmpA64->hd=((_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67->r=(void*)((
_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65[0]=((_tmpA66.tag=12,((_tmpA66.f1=
Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpA66)))),_tmpA65)))),((_tmpA67->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line),_tmpA67)))))),((
_tmpA64->tl=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA64)))))));}else{{const char*_tmpA68;
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),((
_tmpA68="C include",_tag_dyneither(_tmpA68,sizeof(char),10))))!= 0){const char*
_tmpA69;Cyc_Parse_err(((_tmpA69="expecting \"C\" or \"C include\"",
_tag_dyneither(_tmpA69,sizeof(char),29))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).last_line));}}{struct Cyc_Absyn_Decl*
_tmpA73;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA72;struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpA71;struct Cyc_List_List*_tmpA70;yyval=Cyc_YY18(((_tmpA70=_cycalloc(sizeof(*
_tmpA70)),((_tmpA70->hd=((_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73->r=(void*)((
_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71[0]=((_tmpA72.tag=13,((_tmpA72.f1=
Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),((_tmpA72.f2=0,_tmpA72)))))),_tmpA71)))),((
_tmpA73->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),_tmpA73)))))),((_tmpA70->tl=Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpA70)))))));};}}break;case 8: _LL235:{const char*_tmpA74;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))),((
_tmpA74="C include",_tag_dyneither(_tmpA74,sizeof(char),10))))!= 0){const char*
_tmpA75;Cyc_Parse_err(((_tmpA75="expecting \"C include\"",_tag_dyneither(_tmpA75,
sizeof(char),22))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).last_line));}}{struct Cyc_List_List*exs=Cyc_yyget_YY52(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));{struct Cyc_Absyn_Decl*_tmpA7F;struct Cyc_Absyn_ExternCinclude_d_struct
_tmpA7E;struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA7D;struct Cyc_List_List*
_tmpA7C;yyval=Cyc_YY18(((_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C->hd=((
_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F->r=(void*)((_tmpA7D=_cycalloc(
sizeof(*_tmpA7D)),((_tmpA7D[0]=((_tmpA7E.tag=13,((_tmpA7E.f1=Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpA7E.f2=exs,_tmpA7E)))))),_tmpA7D)))),((_tmpA7F->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line),_tmpA7F)))))),((
_tmpA7C->tl=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA7C)))))));}break;};case 9: _LL236:{
struct Cyc_Absyn_Decl*_tmpA82;struct Cyc_List_List*_tmpA81;yyval=Cyc_YY18(((
_tmpA81=_cycalloc(sizeof(*_tmpA81)),((_tmpA81->hd=((_tmpA82=_cycalloc(sizeof(*
_tmpA82)),((_tmpA82->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA82->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line),_tmpA82)))))),((
_tmpA81->tl=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA81)))))));}break;case 10: _LL237:{
struct Cyc_Absyn_Decl*_tmpA85;struct Cyc_List_List*_tmpA84;yyval=Cyc_YY18(((
_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->hd=((_tmpA85=_cycalloc(sizeof(*
_tmpA85)),((_tmpA85->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA85->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line),_tmpA85)))))),((
_tmpA84->tl=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA84)))))));}break;case 11: _LL238:
yyval=Cyc_YY18(0);break;case 12: _LL239: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1));
break;case 13: _LL23A: yyval=Cyc_YY52(0);break;case 14: _LL23B:{struct _tuple24*
_tmpA88;struct Cyc_List_List*_tmpA87;yyval=Cyc_YY52(((_tmpA87=_cycalloc(sizeof(*
_tmpA87)),((_tmpA87->hd=((_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88->f1=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpA88->f2=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpA88->f3=0,_tmpA88)))))))),((_tmpA87->tl=0,_tmpA87)))))));}
break;case 15: _LL23C:{struct _tuple24*_tmpA8B;struct Cyc_List_List*_tmpA8A;yyval=
Cyc_YY52(((_tmpA8A=_cycalloc(sizeof(*_tmpA8A)),((_tmpA8A->hd=((_tmpA8B=_cycalloc(
sizeof(*_tmpA8B)),((_tmpA8B->f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),((_tmpA8B->f2=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpA8B->f3=0,_tmpA8B)))))))),((_tmpA8A->tl=0,_tmpA8A)))))));}break;case 16:
_LL23D:{struct _tuple24*_tmpA8E;struct Cyc_List_List*_tmpA8D;yyval=Cyc_YY52(((
_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D->hd=((_tmpA8E=_cycalloc(sizeof(*
_tmpA8E)),((_tmpA8E->f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line),((_tmpA8E->f2=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((
_tmpA8E->f3=0,_tmpA8E)))))))),((_tmpA8D->tl=Cyc_yyget_YY52(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA8D)))))));}
break;case 17: _LL23E:{struct Cyc_Absyn_Fn_d_struct*_tmpA94;struct Cyc_Absyn_Fn_d_struct
_tmpA93;struct Cyc_List_List*_tmpA92;yyval=Cyc_YY18(((_tmpA92=_cycalloc(sizeof(*
_tmpA92)),((_tmpA92->hd=Cyc_Absyn_new_decl((void*)((_tmpA94=_cycalloc(sizeof(*
_tmpA94)),((_tmpA94[0]=((_tmpA93.tag=1,((_tmpA93.f1=Cyc_yyget_YY17(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA93)))),
_tmpA94)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),((_tmpA92->tl=0,_tmpA92)))))));}break;case 18: _LL23F:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 19: _LL240: yyval=Cyc_YY18(0);break;case 20: _LL241: yyval=
Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),0,
Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 21: _LL242:{struct Cyc_Core_Opt*_tmpA95;
yyval=Cyc_YY17(Cyc_Parse_make_function(((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((
_tmpA95->v=Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpA95)))),Cyc_yyget_YY29(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),0,Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 22: _LL243:
yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 23: _LL244:{
struct Cyc_Core_Opt*_tmpA96;yyval=Cyc_YY17(Cyc_Parse_make_function(((_tmpA96=
_cycalloc(sizeof(*_tmpA96)),((_tmpA96->v=Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpA96)))),Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 24: _LL245:{struct Cyc_Core_Opt*_tmpA97;
yyval=Cyc_YY17(Cyc_Parse_make_function(((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((
_tmpA97->v=Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpA97)))),Cyc_yyget_YY29(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),0,Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 25: _LL246:{
struct Cyc_Core_Opt*_tmpA98;yyval=Cyc_YY17(Cyc_Parse_make_function(((_tmpA98=
_cycalloc(sizeof(*_tmpA98)),((_tmpA98->v=Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpA98)))),Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 26: _LL247: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))));yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 27: _LL248: Cyc_Lex_leave_using();
break;case 28: _LL249:{struct _dyneither_ptr*_tmpA99;Cyc_Lex_enter_namespace(((
_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpA99)))));}yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 29: _LL24A: Cyc_Lex_leave_namespace();break;case 30: _LL24B: yyval=Cyc_YY18(Cyc_Parse_make_declarations(
Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),0,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)));break;case 31: _LL24C: yyval=Cyc_YY18(Cyc_Parse_make_declarations(
Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY21(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 32: _LL24D:{
struct Cyc_List_List*_tmpA9A;yyval=Cyc_YY18(((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((
_tmpA9A->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),((_tmpA9A->tl=0,_tmpA9A)))))));}break;case 33: _LL24E: {
struct Cyc_List_List*_tmp4D7=0;{struct Cyc_List_List*_tmp4D8=Cyc_yyget_YY36(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));for(0;_tmp4D8 != 0;_tmp4D8=_tmp4D8->tl){struct _dyneither_ptr*
_tmp4D9=(struct _dyneither_ptr*)_tmp4D8->hd;struct _tuple1*_tmpA9B;struct _tuple1*
qv=(_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((_tmpA9B->f1=Cyc_Absyn_Rel_n(0),((
_tmpA9B->f2=_tmp4D9,_tmpA9B)))));struct Cyc_Absyn_Vardecl*_tmp4DA=Cyc_Absyn_new_vardecl(
qv,Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*_tmpA9C;_tmp4D7=((_tmpA9C=
_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->hd=_tmp4DA,((_tmpA9C->tl=_tmp4D7,_tmpA9C))))));}}
_tmp4D7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4D7);{struct Cyc_List_List*_tmpA9D;yyval=Cyc_YY18(((_tmpA9D=_cycalloc(sizeof(*
_tmpA9D)),((_tmpA9D->hd=Cyc_Absyn_letv_decl(_tmp4D7,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpA9D->tl=0,
_tmpA9D)))))));}break;}case 34: _LL24F:{const char*_tmpA9E;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((
_tmpA9E="`H",_tag_dyneither(_tmpA9E,sizeof(char),3))))== 0){const char*_tmpAA1;
void*_tmpAA0;Cyc_Parse_err((struct _dyneither_ptr)((_tmpAA0=0,Cyc_aprintf(((
_tmpAA1="bad occurrence of heap region",_tag_dyneither(_tmpAA1,sizeof(char),30))),
_tag_dyneither(_tmpAA0,sizeof(void*),0)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));}}{const char*
_tmpAA2;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((
_tmpAA2="`U",_tag_dyneither(_tmpAA2,sizeof(char),3))))== 0){const char*_tmpAA5;
void*_tmpAA4;Cyc_Parse_err((struct _dyneither_ptr)((_tmpAA4=0,Cyc_aprintf(((
_tmpAA5="bad occurrence of unique region",_tag_dyneither(_tmpAA5,sizeof(char),32))),
_tag_dyneither(_tmpAA4,sizeof(void*),0)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));}}{struct
_dyneither_ptr*_tmpAA8;struct Cyc_Absyn_Tvar*_tmpAA7;struct Cyc_Absyn_Tvar*tv=(
_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->name=((_tmpAA8=_cycalloc(sizeof(*
_tmpAA8)),((_tmpAA8[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpAA8)))),((_tmpAA7->identity=- 1,((_tmpAA7->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),
_tmpAA7)))))));struct Cyc_Absyn_VarType_struct _tmpAAB;struct Cyc_Absyn_VarType_struct*
_tmpAAA;void*t=(void*)((_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA[0]=((
_tmpAAB.tag=2,((_tmpAAB.f1=tv,_tmpAAB)))),_tmpAAA))));struct Cyc_Absyn_RgnHandleType_struct
_tmpAB1;struct Cyc_Absyn_RgnHandleType_struct*_tmpAB0;struct _dyneither_ptr*
_tmpAAE;struct _tuple1*_tmpAAD;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD->f1=Cyc_Absyn_Loc_n,((_tmpAAD->f2=((
_tmpAAE=_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpAAE)))),_tmpAAD)))))),(void*)((_tmpAB0=_cycalloc(sizeof(*
_tmpAB0)),((_tmpAB0[0]=((_tmpAB1.tag=16,((_tmpAB1.f1=(void*)t,_tmpAB1)))),
_tmpAB0)))),0);{struct Cyc_List_List*_tmpAB2;yyval=Cyc_YY18(((_tmpAB2=_cycalloc(
sizeof(*_tmpAB2)),((_tmpAB2->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 6))),0,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),((_tmpAB2->tl=0,_tmpAB2)))))));}break;};case 35:
_LL250:{const char*_tmpAB3;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpAB3="H",_tag_dyneither(_tmpAB3,sizeof(char),2))))== 0){
const char*_tmpAB4;Cyc_Parse_err(((_tmpAB4="bad occurrence of heap region `H",
_tag_dyneither(_tmpAB4,sizeof(char),33))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line));}}{const char*
_tmpAB5;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((
_tmpAB5="U",_tag_dyneither(_tmpAB5,sizeof(char),2))))== 0){const char*_tmpAB6;Cyc_Parse_err(((
_tmpAB6="bad occurrence of unique region `U",_tag_dyneither(_tmpAB6,sizeof(char),
35))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line));}}if(Cyc_yyget_YY31(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))) && 
Cyc_yyget_YY4(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1)))!= 0){const char*_tmpAB7;Cyc_Parse_err(((_tmpAB7="open regions cannot be @resetable",
_tag_dyneither(_tmpAB7,sizeof(char),34))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));}{struct
_dyneither_ptr*_tmpAC4;const char*_tmpAC3;void*_tmpAC2[1];struct Cyc_String_pa_struct
_tmpAC1;struct Cyc_Absyn_Tvar*_tmpAC0;struct Cyc_Absyn_Tvar*tv=(_tmpAC0=_cycalloc(
sizeof(*_tmpAC0)),((_tmpAC0->name=((_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((
_tmpAC4[0]=(struct _dyneither_ptr)((_tmpAC1.tag=0,((_tmpAC1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),((
_tmpAC2[0]=& _tmpAC1,Cyc_aprintf(((_tmpAC3="`%s",_tag_dyneither(_tmpAC3,sizeof(
char),4))),_tag_dyneither(_tmpAC2,sizeof(void*),1)))))))),_tmpAC4)))),((_tmpAC0->identity=
- 1,((_tmpAC0->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAC0)))))));
struct Cyc_Absyn_VarType_struct _tmpAC7;struct Cyc_Absyn_VarType_struct*_tmpAC6;
void*t=(void*)((_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6[0]=((_tmpAC7.tag=2,((
_tmpAC7.f1=tv,_tmpAC7)))),_tmpAC6))));struct Cyc_Absyn_RgnHandleType_struct
_tmpACD;struct Cyc_Absyn_RgnHandleType_struct*_tmpACC;struct _dyneither_ptr*
_tmpACA;struct _tuple1*_tmpAC9;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9->f1=Cyc_Absyn_Loc_n,((_tmpAC9->f2=((
_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),_tmpACA)))),_tmpAC9)))))),(void*)((_tmpACC=_cycalloc(sizeof(*
_tmpACC)),((_tmpACC[0]=((_tmpACD.tag=16,((_tmpACD.f1=(void*)t,_tmpACD)))),
_tmpACC)))),0);{struct Cyc_List_List*_tmpACE;yyval=Cyc_YY18(((_tmpACE=_cycalloc(
sizeof(*_tmpACE)),((_tmpACE->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))),Cyc_yyget_YY4(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpACE->tl=0,
_tmpACE)))))));}break;};case 36: _LL251: {struct Cyc_Absyn_Eq_kb_struct*_tmpAD6;
struct Cyc_Absyn_Eq_kb_struct _tmpAD5;struct _dyneither_ptr*_tmpAD4;struct Cyc_Absyn_Tvar*
_tmpAD3;struct Cyc_Absyn_Tvar*tv=(_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3->name=((
_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 5))),_tmpAD4)))),((_tmpAD3->identity=- 1,((_tmpAD3->kind=(void*)((
_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6[0]=((_tmpAD5.tag=0,((_tmpAD5.f1=&
Cyc_Tcutil_rk,_tmpAD5)))),_tmpAD6)))),_tmpAD3)))))));struct _dyneither_ptr*
_tmpAD9;struct _tuple1*_tmpAD8;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpAD8=_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8->f1=Cyc_Absyn_Loc_n,((_tmpAD8->f2=((
_tmpAD9=_cycalloc(sizeof(*_tmpAD9)),((_tmpAD9[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),_tmpAD9)))),_tmpAD8)))))),(void*)& Cyc_Absyn_VoidType_val,0);{
struct Cyc_List_List*_tmpADA;yyval=Cyc_YY18(((_tmpADA=_cycalloc(sizeof(*_tmpADA)),((
_tmpADA->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),tv,
vd,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((
_tmpADA->tl=0,_tmpADA)))))));}break;}case 37: _LL252: yyval=Cyc_YY31(0);break;case
38: _LL253: yyval=Cyc_YY31(1);break;case 39: _LL254: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 40: _LL255: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))));break;case 41: _LL256:{struct Cyc_Core_Opt*_tmpADD;struct Cyc_Parse_Declaration_spec*
_tmpADC;yyval=Cyc_YY19(((_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC->sc=((
_tmpADD=_cycalloc_atomic(sizeof(*_tmpADD)),((_tmpADD->v=(void*)Cyc_yyget_YY22(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpADD)))),((_tmpADC->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpADC->type_specs=
0,((_tmpADC->is_inline=0,((_tmpADC->attributes=0,_tmpADC)))))))))))));}break;
case 42: _LL257: if((Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->sc != 0){const char*_tmpAE0;void*
_tmpADF;(_tmpADF=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpAE0="Only one storage class is allowed in a declaration",
_tag_dyneither(_tmpAE0,sizeof(char),51))),_tag_dyneither(_tmpADF,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmpAE3;struct Cyc_Parse_Declaration_spec*_tmpAE2;yyval=Cyc_YY19(((
_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2->sc=((_tmpAE3=_cycalloc_atomic(
sizeof(*_tmpAE3)),((_tmpAE3->v=(void*)Cyc_yyget_YY22(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpAE3)))),((_tmpAE2->tq=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tq,((
_tmpAE2->type_specs=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->type_specs,((
_tmpAE2->is_inline=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->is_inline,((
_tmpAE2->attributes=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->attributes,
_tmpAE2)))))))))))));}break;case 43: _LL258:{struct Cyc_List_List*_tmpAE6;struct Cyc_Parse_Declaration_spec*
_tmpAE5;yyval=Cyc_YY19(((_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5->sc=0,((
_tmpAE5->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),((_tmpAE5->type_specs=((_tmpAE6=_cycalloc(sizeof(*
_tmpAE6)),((_tmpAE6->hd=Cyc_yyget_YY23(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpAE6->tl=0,_tmpAE6)))))),((_tmpAE5->is_inline=0,((_tmpAE5->attributes=0,
_tmpAE5)))))))))))));}break;case 44: _LL259:{struct Cyc_List_List*_tmpAE9;struct Cyc_Parse_Declaration_spec*
_tmpAE8;yyval=Cyc_YY19(((_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8->sc=(Cyc_yyget_YY19(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->sc,((_tmpAE8->tq=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tq,((
_tmpAE8->type_specs=((_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9->hd=Cyc_yyget_YY23(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpAE9->tl=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->type_specs,
_tmpAE9)))))),((_tmpAE8->is_inline=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->is_inline,((
_tmpAE8->attributes=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->attributes,
_tmpAE8)))))))))))));}break;case 45: _LL25A:{struct Cyc_Parse_Declaration_spec*
_tmpAEA;yyval=Cyc_YY19(((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA->sc=0,((
_tmpAEA->tq=Cyc_yyget_YY25(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((_tmpAEA->type_specs=0,((_tmpAEA->is_inline=
0,((_tmpAEA->attributes=0,_tmpAEA)))))))))))));}break;case 46: _LL25B:{struct Cyc_Parse_Declaration_spec*
_tmpAEB;yyval=Cyc_YY19(((_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB->sc=(Cyc_yyget_YY19(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->sc,((_tmpAEB->tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tq),((
_tmpAEB->type_specs=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->type_specs,((
_tmpAEB->is_inline=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->is_inline,((
_tmpAEB->attributes=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->attributes,
_tmpAEB)))))))))))));}break;case 47: _LL25C:{struct Cyc_Parse_Declaration_spec*
_tmpAEC;yyval=Cyc_YY19(((_tmpAEC=_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC->sc=0,((
_tmpAEC->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),((_tmpAEC->type_specs=0,((_tmpAEC->is_inline=1,((
_tmpAEC->attributes=0,_tmpAEC)))))))))))));}break;case 48: _LL25D:{struct Cyc_Parse_Declaration_spec*
_tmpAED;yyval=Cyc_YY19(((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED->sc=(Cyc_yyget_YY19(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->sc,((_tmpAED->tq=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tq,((
_tmpAED->type_specs=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->type_specs,((
_tmpAED->is_inline=1,((_tmpAED->attributes=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->attributes,
_tmpAED)))))))))))));}break;case 49: _LL25E:{struct Cyc_Parse_Declaration_spec*
_tmpAEE;yyval=Cyc_YY19(((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->sc=0,((
_tmpAEE->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),((_tmpAEE->type_specs=0,((_tmpAEE->is_inline=0,((
_tmpAEE->attributes=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpAEE)))))))))))));}
break;case 50: _LL25F:{struct Cyc_Parse_Declaration_spec*_tmpAEF;yyval=Cyc_YY19(((
_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF->sc=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->sc,((
_tmpAEF->tq=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tq,((_tmpAEF->type_specs=(Cyc_yyget_YY19(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->type_specs,((_tmpAEF->is_inline=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->is_inline,((
_tmpAEF->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(Cyc_yyget_YY19(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->attributes),_tmpAEF)))))))))))));}break;case 51: _LL260: yyval=
Cyc_YY22(Cyc_Parse_Auto_sc);break;case 52: _LL261: yyval=Cyc_YY22(Cyc_Parse_Register_sc);
break;case 53: _LL262: yyval=Cyc_YY22(Cyc_Parse_Static_sc);break;case 54: _LL263:
yyval=Cyc_YY22(Cyc_Parse_Extern_sc);break;case 55: _LL264:{const char*_tmpAF0;if(
Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpAF0="C",_tag_dyneither(_tmpAF0,sizeof(char),2))))!= 0){const char*_tmpAF1;Cyc_Parse_err(((
_tmpAF1="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAF1,sizeof(
char),37))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));}}yyval=Cyc_YY22(Cyc_Parse_ExternC_sc);break;case 56:
_LL265: yyval=Cyc_YY22(Cyc_Parse_Typedef_sc);break;case 57: _LL266: yyval=Cyc_YY22(
Cyc_Parse_Abstract_sc);break;case 58: _LL267: yyval=Cyc_YY45(0);break;case 59: _LL268:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 60: _LL269: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2));
break;case 61: _LL26A:{struct Cyc_List_List*_tmpAF2;yyval=Cyc_YY45(((_tmpAF2=
_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2->hd=Cyc_yyget_YY46(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpAF2->tl=0,_tmpAF2)))))));}break;case 62: _LL26B:{struct Cyc_List_List*_tmpAF3;
yyval=Cyc_YY45(((_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3->hd=Cyc_yyget_YY46(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpAF3->tl=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpAF3)))))));}
break;case 63: _LL26C: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={6,- 1};
static struct _tuple25 att_map[17]={{{_tmp51B,_tmp51B,_tmp51B + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{
_tmp51C,_tmp51C,_tmp51C + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp51D,_tmp51D,
_tmp51D + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp51E,_tmp51E,_tmp51E + 9},(
void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp51F,_tmp51F,_tmp51F + 6},(void*)& Cyc_Absyn_Const_att_val},{{
_tmp520,_tmp520,_tmp520 + 8},(void*)& att_aligned},{{_tmp521,_tmp521,_tmp521 + 7},(
void*)& Cyc_Absyn_Packed_att_val},{{_tmp522,_tmp522,_tmp522 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{
_tmp523,_tmp523,_tmp523 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp524,_tmp524,
_tmp524 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp525,_tmp525,_tmp525 + 10},(void*)&
Cyc_Absyn_Dllimport_att_val},{{_tmp526,_tmp526,_tmp526 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{
_tmp527,_tmp527,_tmp527 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{
_tmp528,_tmp528,_tmp528 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp529,
_tmp529,_tmp529 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp52A,_tmp52A,
_tmp52A + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp52B,_tmp52B,
_tmp52B + 5},(void*)& Cyc_Absyn_Pure_att_val}};struct _dyneither_ptr _tmp519=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)));if((((_get_dyneither_size(_tmp519,sizeof(char))> 4  && *((const
char*)_check_dyneither_subscript(_tmp519,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp519,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp519,sizeof(char),(int)(_get_dyneither_size(
_tmp519,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp519,sizeof(char),(int)(_get_dyneither_size(_tmp519,sizeof(char))- 3)))== '_')
_tmp519=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp519,2,
_get_dyneither_size(_tmp519,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp519,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){{const char*_tmpAF4;Cyc_Parse_err(((
_tmpAF4="unrecognized attribute",_tag_dyneither(_tmpAF4,sizeof(char),23))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));}yyval=Cyc_YY46((void*)&
Cyc_Absyn_Cdecl_att_val);}break;};}case 64: _LL26D: yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
break;case 65: _LL26E: {struct _dyneither_ptr _tmp52D=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3)));int _tmp52F;struct _tuple6 _tmp52E=Cyc_yyget_Int_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));
_tmp52F=_tmp52E.f2;{void*a;{const char*_tmpAF6;const char*_tmpAF5;if(Cyc_zstrcmp((
struct _dyneither_ptr)_tmp52D,((_tmpAF6="regparm",_tag_dyneither(_tmpAF6,sizeof(
char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpAF5="__regparm__",
_tag_dyneither(_tmpAF5,sizeof(char),12))))== 0){if(_tmp52F < 0  || _tmp52F > 3){
const char*_tmpAF7;Cyc_Parse_err(((_tmpAF7="regparm requires value between 0 and 3",
_tag_dyneither(_tmpAF7,sizeof(char),39))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));}{struct Cyc_Absyn_Regparm_att_struct
_tmpAFA;struct Cyc_Absyn_Regparm_att_struct*_tmpAF9;a=(void*)((_tmpAF9=
_cycalloc_atomic(sizeof(*_tmpAF9)),((_tmpAF9[0]=((_tmpAFA.tag=0,((_tmpAFA.f1=
_tmp52F,_tmpAFA)))),_tmpAF9))));};}else{const char*_tmpAFC;const char*_tmpAFB;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpAFC="aligned",_tag_dyneither(
_tmpAFC,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((
_tmpAFB="__aligned__",_tag_dyneither(_tmpAFB,sizeof(char),12))))== 0){if(_tmp52F
< 0){const char*_tmpAFD;Cyc_Parse_err(((_tmpAFD="aligned requires positive power of two",
_tag_dyneither(_tmpAFD,sizeof(char),39))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));}{unsigned int j=(
unsigned int)_tmp52F;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){const char*
_tmpAFE;Cyc_Parse_err(((_tmpAFE="aligned requires positive power of two",
_tag_dyneither(_tmpAFE,sizeof(char),39))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmpB01;struct Cyc_Absyn_Aligned_att_struct*_tmpB00;a=(void*)((_tmpB00=
_cycalloc_atomic(sizeof(*_tmpB00)),((_tmpB00[0]=((_tmpB01.tag=6,((_tmpB01.f1=
_tmp52F,_tmpB01)))),_tmpB00))));};};}else{const char*_tmpB03;const char*_tmpB02;
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB03="initializes",
_tag_dyneither(_tmpB03,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp52D,((_tmpB02="__initializes__",_tag_dyneither(_tmpB02,
sizeof(char),16))))== 0){struct Cyc_Absyn_Initializes_att_struct _tmpB06;struct Cyc_Absyn_Initializes_att_struct*
_tmpB05;a=(void*)((_tmpB05=_cycalloc_atomic(sizeof(*_tmpB05)),((_tmpB05[0]=((
_tmpB06.tag=20,((_tmpB06.f1=_tmp52F,_tmpB06)))),_tmpB05))));}else{const char*
_tmpB08;const char*_tmpB07;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB08="noliveunique",
_tag_dyneither(_tmpB08,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp52D,((_tmpB07="__noliveunique__",_tag_dyneither(_tmpB07,
sizeof(char),17))))== 0){struct Cyc_Absyn_Noliveunique_att_struct _tmpB0B;struct
Cyc_Absyn_Noliveunique_att_struct*_tmpB0A;a=(void*)((_tmpB0A=_cycalloc_atomic(
sizeof(*_tmpB0A)),((_tmpB0A[0]=((_tmpB0B.tag=21,((_tmpB0B.f1=_tmp52F,_tmpB0B)))),
_tmpB0A))));}else{{const char*_tmpB0C;Cyc_Parse_err(((_tmpB0C="unrecognized attribute",
_tag_dyneither(_tmpB0C,sizeof(char),23))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));}a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}
yyval=Cyc_YY46(a);break;};}case 66: _LL26F: {struct _dyneither_ptr _tmp544=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3)));struct _dyneither_ptr _tmp545=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));void*
a;{const char*_tmpB0E;const char*_tmpB0D;if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp544,((_tmpB0E="section",_tag_dyneither(_tmpB0E,sizeof(char),8))))== 0  || Cyc_zstrcmp((
struct _dyneither_ptr)_tmp544,((_tmpB0D="__section__",_tag_dyneither(_tmpB0D,
sizeof(char),12))))== 0){struct Cyc_Absyn_Section_att_struct _tmpB11;struct Cyc_Absyn_Section_att_struct*
_tmpB10;a=(void*)((_tmpB10=_cycalloc(sizeof(*_tmpB10)),((_tmpB10[0]=((_tmpB11.tag=
8,((_tmpB11.f1=_tmp545,_tmpB11)))),_tmpB10))));}else{{const char*_tmpB12;Cyc_Parse_err(((
_tmpB12="unrecognized attribute",_tag_dyneither(_tmpB12,sizeof(char),23))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));}a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
yyval=Cyc_YY46(a);break;}case 67: _LL270: {struct _dyneither_ptr _tmp54B=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3)));struct _dyneither_ptr _tmp54C=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));void*
a;{const char*_tmpB13;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54B,((_tmpB13="__mode__",
_tag_dyneither(_tmpB13,sizeof(char),9))))== 0){struct Cyc_Absyn_Mode_att_struct
_tmpB16;struct Cyc_Absyn_Mode_att_struct*_tmpB15;a=(void*)((_tmpB15=_cycalloc(
sizeof(*_tmpB15)),((_tmpB15[0]=((_tmpB16.tag=23,((_tmpB16.f1=_tmp54C,_tmpB16)))),
_tmpB15))));}else{{const char*_tmpB17;Cyc_Parse_err(((_tmpB17="unrecognized attribute",
_tag_dyneither(_tmpB17,sizeof(char),23))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));}a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
yyval=Cyc_YY46(a);break;}case 68: _LL271: {struct _dyneither_ptr _tmp551=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 7)));struct _dyneither_ptr _tmp552=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5)));int
_tmp554;struct _tuple6 _tmp553=Cyc_yyget_Int_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3)));
_tmp554=_tmp553.f2;{int _tmp556;struct _tuple6 _tmp555=Cyc_yyget_Int_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));_tmp556=_tmp555.f2;{void*a=(void*)& Cyc_Absyn_Cdecl_att_val;{
const char*_tmpB19;const char*_tmpB18;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp551,((
_tmpB19="format",_tag_dyneither(_tmpB19,sizeof(char),7))))== 0  || Cyc_zstrcmp((
struct _dyneither_ptr)_tmp551,((_tmpB18="__format__",_tag_dyneither(_tmpB18,
sizeof(char),11))))== 0){const char*_tmpB1A;if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp552,((_tmpB1A="printf",_tag_dyneither(_tmpB1A,sizeof(char),7))))== 0){struct
Cyc_Absyn_Format_att_struct _tmpB1D;struct Cyc_Absyn_Format_att_struct*_tmpB1C;a=(
void*)((_tmpB1C=_cycalloc_atomic(sizeof(*_tmpB1C)),((_tmpB1C[0]=((_tmpB1D.tag=19,((
_tmpB1D.f1=Cyc_Absyn_Printf_ft,((_tmpB1D.f2=_tmp554,((_tmpB1D.f3=_tmp556,_tmpB1D)))))))),
_tmpB1C))));}else{const char*_tmpB1E;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp552,((
_tmpB1E="scanf",_tag_dyneither(_tmpB1E,sizeof(char),6))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpB21;struct Cyc_Absyn_Format_att_struct*_tmpB20;a=(void*)((_tmpB20=
_cycalloc_atomic(sizeof(*_tmpB20)),((_tmpB20[0]=((_tmpB21.tag=19,((_tmpB21.f1=
Cyc_Absyn_Scanf_ft,((_tmpB21.f2=_tmp554,((_tmpB21.f3=_tmp556,_tmpB21)))))))),
_tmpB20))));}else{const char*_tmpB22;Cyc_Parse_err(((_tmpB22="unrecognized format type",
_tag_dyneither(_tmpB22,sizeof(char),25))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).last_line));}}}else{const char*
_tmpB23;Cyc_Parse_err(((_tmpB23="unrecognized attribute",_tag_dyneither(_tmpB23,
sizeof(char),23))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));}}yyval=Cyc_YY46(a);break;};};}case 69: _LL272: yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 70: _LL273:{struct Cyc_Absyn_TypedefType_struct _tmpB26;
struct Cyc_Absyn_TypedefType_struct*_tmpB25;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB25=_cycalloc(sizeof(*_tmpB25)),((_tmpB25[0]=((_tmpB26.tag=18,((
_tmpB26.f1=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((_tmpB26.f2=Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB26.f3=0,((_tmpB26.f4=0,_tmpB26)))))))))),_tmpB25)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 71: _LL274:
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 72: _LL275:
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 73: _LL276:
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))),0),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 74: _LL277: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 75: _LL278:{struct Cyc_Parse_Short_spec_struct
_tmpB29;struct Cyc_Parse_Short_spec_struct*_tmpB28;yyval=Cyc_YY23((void*)((
_tmpB28=_cycalloc(sizeof(*_tmpB28)),((_tmpB28[0]=((_tmpB29.tag=2,((_tmpB29.f1=
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),
_tmpB29)))),_tmpB28)))));}break;case 76: _LL279: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 77: _LL27A:{struct Cyc_Parse_Long_spec_struct
_tmpB2C;struct Cyc_Parse_Long_spec_struct*_tmpB2B;yyval=Cyc_YY23((void*)((_tmpB2B=
_cycalloc(sizeof(*_tmpB2B)),((_tmpB2B[0]=((_tmpB2C.tag=3,((_tmpB2C.f1=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpB2C)))),_tmpB2B)))));}
break;case 78: _LL27B: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 79: _LL27C:
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 80: _LL27D:{
struct Cyc_Parse_Signed_spec_struct _tmpB2F;struct Cyc_Parse_Signed_spec_struct*
_tmpB2E;yyval=Cyc_YY23((void*)((_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E[0]=((
_tmpB2F.tag=0,((_tmpB2F.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpB2F)))),_tmpB2E)))));}break;case 81: _LL27E:{struct
Cyc_Parse_Unsigned_spec_struct _tmpB32;struct Cyc_Parse_Unsigned_spec_struct*
_tmpB31;yyval=Cyc_YY23((void*)((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31[0]=((
_tmpB32.tag=1,((_tmpB32.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpB32)))),_tmpB31)))));}break;case 82: _LL27F: yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 83: _LL280: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 84: _LL281: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset));break;case 85: _LL282: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 86: _LL283:{struct Cyc_Absyn_TupleType_struct
_tmpB35;struct Cyc_Absyn_TupleType_struct*_tmpB34;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB34=_cycalloc(sizeof(*_tmpB34)),((_tmpB34[0]=((_tmpB35.tag=11,((
_tmpB35.f1=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))))),_tmpB35)))),_tmpB34)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 87: _LL284:{
struct Cyc_Absyn_RgnHandleType_struct _tmpB38;struct Cyc_Absyn_RgnHandleType_struct*
_tmpB37;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB37=_cycalloc(sizeof(*
_tmpB37)),((_tmpB37[0]=((_tmpB38.tag=16,((_tmpB38.f1=(void*)Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpB38)))),_tmpB37)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 88: _LL285:{
struct Cyc_Absyn_RgnHandleType_struct _tmpB3B;struct Cyc_Absyn_RgnHandleType_struct*
_tmpB3A;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB3A=_cycalloc(sizeof(*
_tmpB3A)),((_tmpB3A[0]=((_tmpB3B.tag=16,((_tmpB3B.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0),_tmpB3B)))),_tmpB3A)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 89: _LL286: {
void*_tmp571=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0);{struct
Cyc_Absyn_DynRgnType_struct _tmpB3E;struct Cyc_Absyn_DynRgnType_struct*_tmpB3D;
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB3D=_cycalloc(sizeof(*_tmpB3D)),((
_tmpB3D[0]=((_tmpB3E.tag=17,((_tmpB3E.f1=(void*)Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpB3E.f2=(void*)_tmp571,_tmpB3E)))))),_tmpB3D)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 90:
_LL287:{struct Cyc_Absyn_DynRgnType_struct _tmpB41;struct Cyc_Absyn_DynRgnType_struct*
_tmpB40;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB40=_cycalloc(sizeof(*
_tmpB40)),((_tmpB40[0]=((_tmpB41.tag=17,((_tmpB41.f1=(void*)Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpB41.f2=(void*)Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpB41)))))),_tmpB40)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 91: _LL288:{struct Cyc_Absyn_TagType_struct
_tmpB44;struct Cyc_Absyn_TagType_struct*_tmpB43;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB43=_cycalloc(sizeof(*_tmpB43)),((_tmpB43[0]=((_tmpB44.tag=20,((
_tmpB44.f1=(void*)Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),_tmpB44)))),_tmpB43)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 92: _LL289:{
struct Cyc_Absyn_TagType_struct _tmpB47;struct Cyc_Absyn_TagType_struct*_tmpB46;
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB46=_cycalloc(sizeof(*_tmpB46)),((
_tmpB46[0]=((_tmpB47.tag=20,((_tmpB47.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_iko,0),_tmpB47)))),_tmpB46)))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 93: _LL28A:{
struct Cyc_Absyn_ValueofType_struct _tmpB4A;struct Cyc_Absyn_ValueofType_struct*
_tmpB49;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB49=_cycalloc(sizeof(*
_tmpB49)),((_tmpB49[0]=((_tmpB4A.tag=19,((_tmpB4A.f1=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpB4A)))),_tmpB49)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 94: _LL28B: yyval=Cyc_YY43(Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 95: _LL28C: {
struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line): 0;{struct Cyc_Absyn_Tqual
_tmpB4B;yyval=Cyc_YY25(((_tmpB4B.print_const=1,((_tmpB4B.q_volatile=0,((_tmpB4B.q_restrict=
0,((_tmpB4B.real_const=1,((_tmpB4B.loc=loc,_tmpB4B)))))))))));}break;}case 96:
_LL28D:{struct Cyc_Absyn_Tqual _tmpB4C;yyval=Cyc_YY25(((_tmpB4C.print_const=0,((
_tmpB4C.q_volatile=1,((_tmpB4C.q_restrict=0,((_tmpB4C.real_const=0,((_tmpB4C.loc=
0,_tmpB4C)))))))))));}break;case 97: _LL28E:{struct Cyc_Absyn_Tqual _tmpB4D;yyval=
Cyc_YY25(((_tmpB4D.print_const=0,((_tmpB4D.q_volatile=0,((_tmpB4D.q_restrict=1,((
_tmpB4D.real_const=0,((_tmpB4D.loc=0,_tmpB4D)))))))))));}break;case 98: _LL28F: {
struct Cyc_Absyn_Enum_td_struct*_tmpB5C;struct Cyc_Absyn_Enumdecl*_tmpB5B;struct
Cyc_Core_Opt*_tmpB5A;struct Cyc_Absyn_Enum_td_struct _tmpB59;struct Cyc_Absyn_TypeDecl*
_tmpB58;struct Cyc_Absyn_TypeDecl*_tmp57F=(_tmpB58=_cycalloc(sizeof(*_tmpB58)),((
_tmpB58->r=(void*)((_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C[0]=((_tmpB59.tag=
1,((_tmpB59.f1=((_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((_tmpB5B->sc=Cyc_Absyn_Public,((
_tmpB5B->name=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((
_tmpB5B->fields=((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A->v=Cyc_yyget_YY48(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpB5A)))),_tmpB5B)))))))),_tmpB59)))),_tmpB5C)))),((_tmpB58->loc=
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),
_tmpB58)))));{struct Cyc_Absyn_TypeDeclType_struct _tmpB5F;struct Cyc_Absyn_TypeDeclType_struct*
_tmpB5E;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB5E=_cycalloc(sizeof(*
_tmpB5E)),((_tmpB5E[0]=((_tmpB5F.tag=27,((_tmpB5F.f1=_tmp57F,((_tmpB5F.f2=0,
_tmpB5F)))))),_tmpB5E)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;}case 99: _LL290:{struct Cyc_Absyn_EnumType_struct
_tmpB62;struct Cyc_Absyn_EnumType_struct*_tmpB61;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB61=_cycalloc(sizeof(*_tmpB61)),((_tmpB61[0]=((_tmpB62.tag=14,((
_tmpB62.f1=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((_tmpB62.f2=0,_tmpB62)))))),
_tmpB61)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 100: _LL291:{struct Cyc_Absyn_AnonEnumType_struct
_tmpB65;struct Cyc_Absyn_AnonEnumType_struct*_tmpB64;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB64=_cycalloc(sizeof(*_tmpB64)),((_tmpB64[0]=((_tmpB65.tag=15,((
_tmpB65.f1=Cyc_yyget_YY48(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),_tmpB65)))),_tmpB64)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 101:
_LL292:{struct Cyc_Absyn_Enumfield*_tmpB66;yyval=Cyc_YY47(((_tmpB66=_cycalloc(
sizeof(*_tmpB66)),((_tmpB66->name=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB66->tag=0,((_tmpB66->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpB66)))))))));}break;case 102: _LL293:{struct Cyc_Absyn_Enumfield*
_tmpB67;yyval=Cyc_YY47(((_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->name=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpB67->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB67->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpB67)))))))));}break;case 103: _LL294:{struct Cyc_List_List*
_tmpB68;yyval=Cyc_YY48(((_tmpB68=_cycalloc(sizeof(*_tmpB68)),((_tmpB68->hd=Cyc_yyget_YY47(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB68->tl=0,_tmpB68)))))));}break;case 104: _LL295:{struct Cyc_List_List*
_tmpB69;yyval=Cyc_YY48(((_tmpB69=_cycalloc(sizeof(*_tmpB69)),((_tmpB69->hd=Cyc_yyget_YY47(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpB69->tl=0,_tmpB69)))))));}break;case 105: _LL296:{struct
Cyc_List_List*_tmpB6A;yyval=Cyc_YY48(((_tmpB6A=_cycalloc(sizeof(*_tmpB6A)),((
_tmpB6A->hd=Cyc_yyget_YY47(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((_tmpB6A->tl=Cyc_yyget_YY48(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpB6A)))))));}break;case 106: _LL297:{struct Cyc_Absyn_AnonAggrType_struct
_tmpB6D;struct Cyc_Absyn_AnonAggrType_struct*_tmpB6C;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C[0]=((_tmpB6D.tag=13,((
_tmpB6D.f1=Cyc_yyget_YY24(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((_tmpB6D.f2=Cyc_yyget_YY26(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpB6D)))))),_tmpB6C)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 107:
_LL298: {struct Cyc_List_List*_tmp592=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).last_line),Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))));
struct Cyc_List_List*_tmp593=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line),Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))));
struct Cyc_Absyn_TypeDecl*_tmp594=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7))),Cyc_Absyn_Public,
Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6))),_tmp592,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp593,Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY26(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),1),0,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));{struct Cyc_Absyn_TypeDeclType_struct _tmpB70;struct
Cyc_Absyn_TypeDeclType_struct*_tmpB6F;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((
_tmpB6F=_cycalloc(sizeof(*_tmpB6F)),((_tmpB6F[0]=((_tmpB70.tag=27,((_tmpB70.f1=
_tmp594,((_tmpB70.f2=0,_tmpB70)))))),_tmpB6F)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 8))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 108:
_LL299: {struct Cyc_List_List*_tmp597=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).last_line),Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))));
struct Cyc_List_List*_tmp598=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line),Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))));
struct Cyc_Absyn_TypeDecl*_tmp599=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7))),Cyc_Absyn_Public,
Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6))),_tmp597,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp598,Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY26(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),0),0,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));{struct Cyc_Absyn_TypeDeclType_struct _tmpB73;struct
Cyc_Absyn_TypeDeclType_struct*_tmpB72;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((
_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72[0]=((_tmpB73.tag=27,((_tmpB73.f1=
_tmp599,((_tmpB73.f2=0,_tmpB73)))))),_tmpB72)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 109:
_LL29A:{struct Cyc_Absyn_AggrType_struct _tmpB7D;struct Cyc_Core_Opt*_tmpB7C;struct
Cyc_Absyn_AggrInfo _tmpB7B;struct Cyc_Absyn_AggrType_struct*_tmpB7A;yyval=Cyc_YY23(
Cyc_Parse_type_spec((void*)((_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((_tmpB7A[0]=((
_tmpB7D.tag=12,((_tmpB7D.f1=((_tmpB7B.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpB7C=_cycalloc_atomic(sizeof(*_tmpB7C)),((_tmpB7C->v=(void*)1,_tmpB7C))))),((
_tmpB7B.targs=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpB7B)))),_tmpB7D)))),_tmpB7A)))),
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}
break;case 110: _LL29B:{struct Cyc_Absyn_AggrType_struct _tmpB83;struct Cyc_Absyn_AggrInfo
_tmpB82;struct Cyc_Absyn_AggrType_struct*_tmpB81;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB81=_cycalloc(sizeof(*_tmpB81)),((_tmpB81[0]=((_tmpB83.tag=12,((
_tmpB83.f1=((_tmpB82.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),0),((_tmpB82.targs=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpB82)))),
_tmpB83)))),_tmpB81)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 111: _LL29C: yyval=Cyc_YY40(0);break;case
112: _LL29D: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1)))));break;case 113: _LL29E: yyval=Cyc_YY24(Cyc_Absyn_StructA);
break;case 114: _LL29F: yyval=Cyc_YY24(Cyc_Absyn_UnionA);break;case 115: _LL2A0: yyval=
Cyc_YY26(0);break;case 116: _LL2A1: {struct Cyc_List_List*_tmp5A3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY27(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))));
struct Cyc_List_List*_tmp5A4=Cyc_Parse_get_argrfield_tags(_tmp5A3);if(_tmp5A4 != 0)((
void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,
_tmp5A4,_tmp5A3);yyval=Cyc_YY26(_tmp5A3);break;}case 117: _LL2A2:{struct Cyc_List_List*
_tmpB84;yyval=Cyc_YY27(((_tmpB84=_cycalloc(sizeof(*_tmpB84)),((_tmpB84->hd=Cyc_yyget_YY26(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB84->tl=0,_tmpB84)))))));}break;case 118: _LL2A3:{struct Cyc_List_List*
_tmpB85;yyval=Cyc_YY27(((_tmpB85=_cycalloc(sizeof(*_tmpB85)),((_tmpB85->hd=Cyc_yyget_YY26(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB85->tl=Cyc_yyget_YY27(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpB85)))))));}break;case 119: _LL2A4: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))));
break;case 120: _LL2A5:{struct Cyc_List_List*_tmpB86;yyval=Cyc_YY21(((_tmpB86=
_cycalloc(sizeof(*_tmpB86)),((_tmpB86->hd=Cyc_yyget_YY20(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB86->tl=0,_tmpB86)))))));}break;case 121: _LL2A6:{struct Cyc_List_List*_tmpB87;
yyval=Cyc_YY21(((_tmpB87=_cycalloc(sizeof(*_tmpB87)),((_tmpB87->hd=Cyc_yyget_YY20(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB87->tl=Cyc_yyget_YY21(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpB87)))))));}break;case 122: _LL2A7:{struct _tuple21*_tmpB88;yyval=Cyc_YY20(((
_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->f1=Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB88->f2=0,_tmpB88)))))));}break;case 123: _LL2A8:{struct _tuple21*_tmpB89;yyval=
Cyc_YY20(((_tmpB89=_cycalloc(sizeof(*_tmpB89)),((_tmpB89->f1=Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpB89->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpB89)))))));}
break;case 124: _LL2A9: {struct _RegionHandle _tmp5AB=_new_region("temp");struct
_RegionHandle*temp=& _tmp5AB;_push_region(temp);{struct _tuple22 _tmp5AD;struct Cyc_Absyn_Tqual
_tmp5AE;struct Cyc_List_List*_tmp5AF;struct Cyc_List_List*_tmp5B0;struct _tuple22*
_tmp5AC=Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)));_tmp5AD=*_tmp5AC;_tmp5AE=_tmp5AD.f1;
_tmp5AF=_tmp5AD.f2;_tmp5B0=_tmp5AD.f3;if(_tmp5AE.loc == 0)_tmp5AE.loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line);{struct Cyc_List_List*
_tmp5B2;struct Cyc_List_List*_tmp5B3;struct _tuple0 _tmp5B1=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
temp,temp,Cyc_yyget_YY21(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))));_tmp5B2=_tmp5B1.f1;_tmp5B3=_tmp5B1.f2;{
void*_tmp5B4=Cyc_Parse_speclist2typ(_tmp5AF,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line));struct Cyc_List_List*
_tmp5B5=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp5AE,_tmp5B4,_tmp5B2,_tmp5B0),_tmp5B3);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,struct _tuple13*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line),
_tmp5B5));};};}_npop_handler(0);break;;_pop_region(temp);}case 125: _LL2AA:{struct
Cyc_List_List*_tmpB8C;struct _tuple22*_tmpB8B;yyval=Cyc_YY35(((_tmpB8B=_cycalloc(
sizeof(*_tmpB8B)),((_tmpB8B->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpB8B->f2=((
_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C->hd=Cyc_yyget_YY23(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB8C->tl=0,_tmpB8C)))))),((_tmpB8B->f3=0,_tmpB8B)))))))));}break;case 126:
_LL2AB:{struct Cyc_List_List*_tmpB8F;struct _tuple22*_tmpB8E;yyval=Cyc_YY35(((
_tmpB8E=_cycalloc(sizeof(*_tmpB8E)),((_tmpB8E->f1=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f1,((
_tmpB8E->f2=((_tmpB8F=_cycalloc(sizeof(*_tmpB8F)),((_tmpB8F->hd=Cyc_yyget_YY23(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpB8F->tl=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f2,
_tmpB8F)))))),((_tmpB8E->f3=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f3,
_tmpB8E)))))))));}break;case 127: _LL2AC:{struct _tuple22*_tmpB90;yyval=Cyc_YY35(((
_tmpB90=_cycalloc(sizeof(*_tmpB90)),((_tmpB90->f1=Cyc_yyget_YY25(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB90->f2=0,((_tmpB90->f3=0,_tmpB90)))))))));}break;case 128: _LL2AD:{struct
_tuple22*_tmpB91;yyval=Cyc_YY35(((_tmpB91=_cycalloc(sizeof(*_tmpB91)),((_tmpB91->f1=
Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(*
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)))).f1),((_tmpB91->f2=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f2,((
_tmpB91->f3=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f3,_tmpB91)))))))));}break;case
129: _LL2AE:{struct _tuple22*_tmpB92;yyval=Cyc_YY35(((_tmpB92=_cycalloc(sizeof(*
_tmpB92)),((_tmpB92->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpB92->f2=0,((
_tmpB92->f3=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpB92)))))))));}break;case 130: _LL2AF:{
struct _tuple22*_tmpB93;yyval=Cyc_YY35(((_tmpB93=_cycalloc(sizeof(*_tmpB93)),((
_tmpB93->f1=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f1,((_tmpB93->f2=(*Cyc_yyget_YY35(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))).f2,((_tmpB93->f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(*
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)))).f3),_tmpB93)))))))));}break;case 131: _LL2B0:{struct
Cyc_List_List*_tmpB96;struct _tuple22*_tmpB95;yyval=Cyc_YY35(((_tmpB95=_cycalloc(
sizeof(*_tmpB95)),((_tmpB95->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpB95->f2=((
_tmpB96=_cycalloc(sizeof(*_tmpB96)),((_tmpB96->hd=Cyc_yyget_YY23(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB96->tl=0,_tmpB96)))))),((_tmpB95->f3=0,_tmpB95)))))))));}break;case 132:
_LL2B1:{struct Cyc_List_List*_tmpB99;struct _tuple22*_tmpB98;yyval=Cyc_YY35(((
_tmpB98=_cycalloc(sizeof(*_tmpB98)),((_tmpB98->f1=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f1,((
_tmpB98->f2=((_tmpB99=_cycalloc(sizeof(*_tmpB99)),((_tmpB99->hd=Cyc_yyget_YY23(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpB99->tl=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f2,
_tmpB99)))))),((_tmpB98->f3=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f3,
_tmpB98)))))))));}break;case 133: _LL2B2:{struct _tuple22*_tmpB9A;yyval=Cyc_YY35(((
_tmpB9A=_cycalloc(sizeof(*_tmpB9A)),((_tmpB9A->f1=Cyc_yyget_YY25(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB9A->f2=0,((_tmpB9A->f3=0,_tmpB9A)))))))));}break;case 134: _LL2B3:{struct
_tuple22*_tmpB9B;yyval=Cyc_YY35(((_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((_tmpB9B->f1=
Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(*
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)))).f1),((_tmpB9B->f2=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f2,((
_tmpB9B->f3=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f3,_tmpB9B)))))))));}break;case
135: _LL2B4:{struct _tuple22*_tmpB9C;yyval=Cyc_YY35(((_tmpB9C=_cycalloc(sizeof(*
_tmpB9C)),((_tmpB9C->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpB9C->f2=0,((
_tmpB9C->f3=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpB9C)))))))));}break;case 136: _LL2B5:{
struct _tuple22*_tmpB9D;yyval=Cyc_YY35(((_tmpB9D=_cycalloc(sizeof(*_tmpB9D)),((
_tmpB9D->f1=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f1,((_tmpB9D->f2=(*Cyc_yyget_YY35(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))).f2,((_tmpB9D->f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(*
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)))).f3),_tmpB9D)))))))));}break;case 137: _LL2B6: yyval=
Cyc_YY21(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))));break;case 138: _LL2B7:{struct Cyc_List_List*_tmpB9E;yyval=Cyc_YY21(((
_tmpB9E=_cycalloc(sizeof(*_tmpB9E)),((_tmpB9E->hd=Cyc_yyget_YY20(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB9E->tl=0,_tmpB9E)))))));}break;case 139: _LL2B8:{struct Cyc_List_List*_tmpB9F;
yyval=Cyc_YY21(((_tmpB9F=_cycalloc(sizeof(*_tmpB9F)),((_tmpB9F->hd=Cyc_yyget_YY20(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB9F->tl=Cyc_yyget_YY21(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpB9F)))))));}break;case 140: _LL2B9:{struct _tuple21*_tmpBA0;yyval=Cyc_YY20(((
_tmpBA0=_cycalloc(sizeof(*_tmpBA0)),((_tmpBA0->f1=Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpBA0->f2=0,_tmpBA0)))))));}break;case 141: _LL2BA:{struct Cyc_Parse_Declarator*
_tmpBA6;struct _tuple1*_tmpBA5;struct _tuple21*_tmpBA4;yyval=Cyc_YY20(((_tmpBA4=
_cycalloc(sizeof(*_tmpBA4)),((_tmpBA4->f1=((_tmpBA6=_cycalloc(sizeof(*_tmpBA6)),((
_tmpBA6->id=((_tmpBA5=_cycalloc(sizeof(*_tmpBA5)),((_tmpBA5->f1=Cyc_Absyn_Rel_n(
0),((_tmpBA5->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",
sizeof(char),1),_tmpBA5)))))),((_tmpBA6->tms=0,_tmpBA6)))))),((_tmpBA4->f2=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpBA4)))))));}
break;case 142: _LL2BB:{struct Cyc_Parse_Declarator*_tmpBAC;struct _tuple1*_tmpBAB;
struct _tuple21*_tmpBAA;yyval=Cyc_YY20(((_tmpBAA=_cycalloc(sizeof(*_tmpBAA)),((
_tmpBAA->f1=((_tmpBAC=_cycalloc(sizeof(*_tmpBAC)),((_tmpBAC->id=((_tmpBAB=
_cycalloc(sizeof(*_tmpBAB)),((_tmpBAB->f1=Cyc_Absyn_Rel_n(0),((_tmpBAB->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),
_tmpBAB)))))),((_tmpBAC->tms=0,_tmpBAC)))))),((_tmpBAA->f2=0,_tmpBAA)))))));}
break;case 143: _LL2BC:{struct _tuple21*_tmpBAD;yyval=Cyc_YY20(((_tmpBAD=_cycalloc(
sizeof(*_tmpBAD)),((_tmpBAD->f1=Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((
_tmpBAD->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpBAD)))))));}
break;case 144: _LL2BD: {int _tmp5D2=Cyc_yyget_YY31(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5)));
struct Cyc_List_List*_tmp5D3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line),Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))));
struct Cyc_Core_Opt*_tmpBAE;struct Cyc_Absyn_TypeDecl*_tmp5D4=Cyc_Absyn_datatype_tdecl(
Cyc_Absyn_Public,Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),
_tmp5D3,((_tmpBAE=_cycalloc(sizeof(*_tmpBAE)),((_tmpBAE->v=Cyc_yyget_YY34(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpBAE)))),_tmp5D2,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));{struct Cyc_Absyn_TypeDeclType_struct _tmpBB1;struct
Cyc_Absyn_TypeDeclType_struct*_tmpBB0;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((
_tmpBB0=_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0[0]=((_tmpBB1.tag=27,((_tmpBB1.f1=
_tmp5D4,((_tmpBB1.f2=0,_tmpBB1)))))),_tmpBB0)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 145:
_LL2BE: {int _tmp5D8=Cyc_yyget_YY31(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)));{
struct Cyc_Absyn_DatatypeType_struct _tmpBBB;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpBBA;struct Cyc_Absyn_DatatypeInfo _tmpBB9;struct Cyc_Absyn_DatatypeType_struct*
_tmpBB8;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpBB8=_cycalloc(sizeof(*
_tmpBB8)),((_tmpBB8[0]=((_tmpBBB.tag=3,((_tmpBBB.f1=((_tmpBB9.datatype_info=Cyc_Absyn_UnknownDatatype(((
_tmpBBA.name=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpBBA.is_extensible=_tmp5D8,_tmpBBA))))),((_tmpBB9.targs=Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpBB9)))),_tmpBBB)))),_tmpBB8)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 146:
_LL2BF: {int _tmp5DD=Cyc_yyget_YY31(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4)));{
struct Cyc_Absyn_DatatypeFieldType_struct _tmpBC5;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpBC4;struct Cyc_Absyn_DatatypeFieldInfo _tmpBC3;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpBC2;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpBC2=_cycalloc(sizeof(*
_tmpBC2)),((_tmpBC2[0]=((_tmpBC5.tag=4,((_tmpBC5.f1=((_tmpBC3.field_info=Cyc_Absyn_UnknownDatatypefield(((
_tmpBC4.datatype_name=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((
_tmpBC4.field_name=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpBC4.is_extensible=_tmp5DD,_tmpBC4))))))),((_tmpBC3.targs=Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpBC3)))),_tmpBC5)))),_tmpBC2)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 147:
_LL2C0: yyval=Cyc_YY31(0);break;case 148: _LL2C1: yyval=Cyc_YY31(1);break;case 149:
_LL2C2:{struct Cyc_List_List*_tmpBC6;yyval=Cyc_YY34(((_tmpBC6=_cycalloc(sizeof(*
_tmpBC6)),((_tmpBC6->hd=Cyc_yyget_YY33(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpBC6->tl=0,_tmpBC6)))))));}break;case 150: _LL2C3:{struct Cyc_List_List*_tmpBC7;
yyval=Cyc_YY34(((_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7->hd=Cyc_yyget_YY33(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpBC7->tl=0,_tmpBC7)))))));}break;case 151: _LL2C4:{struct
Cyc_List_List*_tmpBC8;yyval=Cyc_YY34(((_tmpBC8=_cycalloc(sizeof(*_tmpBC8)),((
_tmpBC8->hd=Cyc_yyget_YY33(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((_tmpBC8->tl=Cyc_yyget_YY34(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpBC8)))))));}break;case 152: _LL2C5:{struct Cyc_List_List*
_tmpBC9;yyval=Cyc_YY34(((_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9->hd=Cyc_yyget_YY33(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpBC9->tl=Cyc_yyget_YY34(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpBC9)))))));}
break;case 153: _LL2C6: yyval=Cyc_YY32(Cyc_Absyn_Public);break;case 154: _LL2C7: yyval=
Cyc_YY32(Cyc_Absyn_Extern);break;case 155: _LL2C8: yyval=Cyc_YY32(Cyc_Absyn_Static);
break;case 156: _LL2C9:{struct Cyc_Absyn_Datatypefield*_tmpBCA;yyval=Cyc_YY33(((
_tmpBCA=_cycalloc(sizeof(*_tmpBCA)),((_tmpBCA->name=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpBCA->typs=0,((_tmpBCA->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpBCA->sc=Cyc_yyget_YY32(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpBCA)))))))))));}break;case 157: _LL2CA: {struct Cyc_List_List*
_tmp5E7=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)))));{struct Cyc_Absyn_Datatypefield*
_tmpBCB;yyval=Cyc_YY33(((_tmpBCB=_cycalloc(sizeof(*_tmpBCB)),((_tmpBCB->name=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpBCB->typs=_tmp5E7,((_tmpBCB->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpBCB->sc=Cyc_yyget_YY32(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))),_tmpBCB)))))))))));}break;}case 158: _LL2CB: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 159: _LL2CC:{struct Cyc_Parse_Declarator*_tmpBCC;yyval=Cyc_YY29(((_tmpBCC=
_cycalloc(sizeof(*_tmpBCC)),((_tmpBCC->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->id,((
_tmpBCC->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY28(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->tms),_tmpBCC)))))));}break;case 160: _LL2CD: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 161: _LL2CE:{struct Cyc_Parse_Declarator*_tmpBCD;yyval=Cyc_YY29(((_tmpBCD=
_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->id,((
_tmpBCD->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY28(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->tms),_tmpBCD)))))));}break;case 162: _LL2CF:{struct Cyc_Parse_Declarator*
_tmpBCE;yyval=Cyc_YY29(((_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE->id=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpBCE->tms=0,_tmpBCE)))))));}break;case 163: _LL2D0: yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1));break;case 164: _LL2D1: {struct Cyc_Parse_Declarator*_tmp5EC=Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));{struct Cyc_Absyn_Attributes_mod_struct*_tmpBD4;struct Cyc_Absyn_Attributes_mod_struct
_tmpBD3;struct Cyc_List_List*_tmpBD2;_tmp5EC->tms=((_tmpBD2=_cycalloc(sizeof(*
_tmpBD2)),((_tmpBD2->hd=(void*)((_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4[0]=((
_tmpBD3.tag=5,((_tmpBD3.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line),((_tmpBD3.f2=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpBD3)))))),_tmpBD4)))),((_tmpBD2->tl=_tmp5EC->tms,_tmpBD2))))));}yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1));break;}case 165: _LL2D2:{struct Cyc_List_List*_tmpBDE;struct Cyc_Absyn_Carray_mod_struct
_tmpBDD;struct Cyc_Absyn_Carray_mod_struct*_tmpBDC;struct Cyc_Parse_Declarator*
_tmpBDB;yyval=Cyc_YY29(((_tmpBDB=_cycalloc(sizeof(*_tmpBDB)),((_tmpBDB->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))))->id,((_tmpBDB->tms=((_tmpBDE=_cycalloc(sizeof(*_tmpBDE)),((
_tmpBDE->hd=(void*)((_tmpBDC=_cycalloc(sizeof(*_tmpBDC)),((_tmpBDC[0]=((_tmpBDD.tag=
0,((_tmpBDD.f1=Cyc_yyget_YY51(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((_tmpBDD.f2=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpBDD)))))),_tmpBDC)))),((
_tmpBDE->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,_tmpBDE)))))),_tmpBDB)))))));}
break;case 166: _LL2D3:{struct Cyc_List_List*_tmpBE8;struct Cyc_Absyn_ConstArray_mod_struct
_tmpBE7;struct Cyc_Absyn_ConstArray_mod_struct*_tmpBE6;struct Cyc_Parse_Declarator*
_tmpBE5;yyval=Cyc_YY29(((_tmpBE5=_cycalloc(sizeof(*_tmpBE5)),((_tmpBE5->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->id,((_tmpBE5->tms=((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((
_tmpBE8->hd=(void*)((_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6[0]=((_tmpBE7.tag=
1,((_tmpBE7.f1=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((_tmpBE7.f2=Cyc_yyget_YY51(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpBE7.f3=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpBE7)))))))),_tmpBE6)))),((_tmpBE8->tl=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->tms,_tmpBE8)))))),_tmpBE5)))))));}break;case 167: _LL2D4: {
struct _tuple23 _tmp5F9;struct Cyc_List_List*_tmp5FA;int _tmp5FB;struct Cyc_Absyn_VarargInfo*
_tmp5FC;struct Cyc_Core_Opt*_tmp5FD;struct Cyc_List_List*_tmp5FE;struct _tuple23*
_tmp5F8=Cyc_yyget_YY39(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));_tmp5F9=*_tmp5F8;_tmp5FA=_tmp5F9.f1;
_tmp5FB=_tmp5F9.f2;_tmp5FC=_tmp5F9.f3;_tmp5FD=_tmp5F9.f4;_tmp5FE=_tmp5F9.f5;{
struct Cyc_List_List*_tmpBFD;struct Cyc_Absyn_Function_mod_struct _tmpBFC;struct Cyc_Absyn_WithTypes_struct
_tmpBFB;struct Cyc_Absyn_WithTypes_struct*_tmpBFA;struct Cyc_Absyn_Function_mod_struct*
_tmpBF9;struct Cyc_Parse_Declarator*_tmpBF8;yyval=Cyc_YY29(((_tmpBF8=_cycalloc(
sizeof(*_tmpBF8)),((_tmpBF8->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->id,((
_tmpBF8->tms=((_tmpBFD=_cycalloc(sizeof(*_tmpBFD)),((_tmpBFD->hd=(void*)((
_tmpBF9=_cycalloc(sizeof(*_tmpBF9)),((_tmpBF9[0]=((_tmpBFC.tag=3,((_tmpBFC.f1=(
void*)((void*)((_tmpBFA=_cycalloc(sizeof(*_tmpBFA)),((_tmpBFA[0]=((_tmpBFB.tag=1,((
_tmpBFB.f1=_tmp5FA,((_tmpBFB.f2=_tmp5FB,((_tmpBFB.f3=_tmp5FC,((_tmpBFB.f4=
_tmp5FD,((_tmpBFB.f5=_tmp5FE,_tmpBFB)))))))))))),_tmpBFA))))),_tmpBFC)))),
_tmpBF9)))),((_tmpBFD->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,
_tmpBFD)))))),_tmpBF8)))))));}break;}case 168: _LL2D5:{struct Cyc_List_List*_tmpC12;
struct Cyc_Absyn_Function_mod_struct _tmpC11;struct Cyc_Absyn_WithTypes_struct
_tmpC10;struct Cyc_Absyn_WithTypes_struct*_tmpC0F;struct Cyc_Absyn_Function_mod_struct*
_tmpC0E;struct Cyc_Parse_Declarator*_tmpC0D;yyval=Cyc_YY29(((_tmpC0D=_cycalloc(
sizeof(*_tmpC0D)),((_tmpC0D->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))))->id,((
_tmpC0D->tms=((_tmpC12=_cycalloc(sizeof(*_tmpC12)),((_tmpC12->hd=(void*)((
_tmpC0E=_cycalloc(sizeof(*_tmpC0E)),((_tmpC0E[0]=((_tmpC11.tag=3,((_tmpC11.f1=(
void*)((void*)((_tmpC0F=_cycalloc(sizeof(*_tmpC0F)),((_tmpC0F[0]=((_tmpC10.tag=1,((
_tmpC10.f1=0,((_tmpC10.f2=0,((_tmpC10.f3=0,((_tmpC10.f4=Cyc_yyget_YY49(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpC10.f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpC10)))))))))))),_tmpC0F))))),_tmpC11)))),_tmpC0E)))),((_tmpC12->tl=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->tms,_tmpC12)))))),_tmpC0D)))))));}break;case 169: _LL2D6:{
struct Cyc_List_List*_tmpC27;struct Cyc_Absyn_Function_mod_struct _tmpC26;struct Cyc_Absyn_NoTypes_struct
_tmpC25;struct Cyc_Absyn_NoTypes_struct*_tmpC24;struct Cyc_Absyn_Function_mod_struct*
_tmpC23;struct Cyc_Parse_Declarator*_tmpC22;yyval=Cyc_YY29(((_tmpC22=_cycalloc(
sizeof(*_tmpC22)),((_tmpC22->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->id,((
_tmpC22->tms=((_tmpC27=_cycalloc(sizeof(*_tmpC27)),((_tmpC27->hd=(void*)((
_tmpC23=_cycalloc(sizeof(*_tmpC23)),((_tmpC23[0]=((_tmpC26.tag=3,((_tmpC26.f1=(
void*)((void*)((_tmpC24=_cycalloc(sizeof(*_tmpC24)),((_tmpC24[0]=((_tmpC25.tag=0,((
_tmpC25.f1=Cyc_yyget_YY36(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((_tmpC25.f2=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpC25)))))),_tmpC24))))),
_tmpC26)))),_tmpC23)))),((_tmpC27->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,
_tmpC27)))))),_tmpC22)))))));}break;case 170: _LL2D7: {struct Cyc_List_List*_tmp611=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)))));{struct Cyc_List_List*_tmpC31;struct Cyc_Absyn_TypeParams_mod_struct
_tmpC30;struct Cyc_Absyn_TypeParams_mod_struct*_tmpC2F;struct Cyc_Parse_Declarator*
_tmpC2E;yyval=Cyc_YY29(((_tmpC2E=_cycalloc(sizeof(*_tmpC2E)),((_tmpC2E->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))))->id,((_tmpC2E->tms=((_tmpC31=_cycalloc(sizeof(*_tmpC31)),((
_tmpC31->hd=(void*)((_tmpC2F=_cycalloc(sizeof(*_tmpC2F)),((_tmpC2F[0]=((_tmpC30.tag=
4,((_tmpC30.f1=_tmp611,((_tmpC30.f2=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpC30.f3=0,_tmpC30)))))))),_tmpC2F)))),((_tmpC31->tl=(
Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,_tmpC31)))))),_tmpC2E)))))));}break;}case
171: _LL2D8:{struct Cyc_List_List*_tmpC3B;struct Cyc_Absyn_Attributes_mod_struct
_tmpC3A;struct Cyc_Absyn_Attributes_mod_struct*_tmpC39;struct Cyc_Parse_Declarator*
_tmpC38;yyval=Cyc_YY29(((_tmpC38=_cycalloc(sizeof(*_tmpC38)),((_tmpC38->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))))->id,((_tmpC38->tms=((_tmpC3B=_cycalloc(sizeof(*_tmpC3B)),((
_tmpC3B->hd=(void*)((_tmpC39=_cycalloc(sizeof(*_tmpC39)),((_tmpC39[0]=((_tmpC3A.tag=
5,((_tmpC3A.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpC3A.f2=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpC3A)))))),
_tmpC39)))),((_tmpC3B->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))))->tms,
_tmpC3B)))))),_tmpC38)))))));}break;case 172: _LL2D9:{struct Cyc_Parse_Declarator*
_tmpC3C;yyval=Cyc_YY29(((_tmpC3C=_cycalloc(sizeof(*_tmpC3C)),((_tmpC3C->id=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpC3C->tms=0,_tmpC3C)))))));}break;case 173: _LL2DA:{struct Cyc_Parse_Declarator*
_tmpC3D;yyval=Cyc_YY29(((_tmpC3D=_cycalloc(sizeof(*_tmpC3D)),((_tmpC3D->id=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpC3D->tms=0,_tmpC3D)))))));}break;case 174: _LL2DB: yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1));break;case 175: _LL2DC: {struct Cyc_Parse_Declarator*_tmp61C=Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));{struct Cyc_Absyn_Attributes_mod_struct*_tmpC43;struct Cyc_Absyn_Attributes_mod_struct
_tmpC42;struct Cyc_List_List*_tmpC41;_tmp61C->tms=((_tmpC41=_cycalloc(sizeof(*
_tmpC41)),((_tmpC41->hd=(void*)((_tmpC43=_cycalloc(sizeof(*_tmpC43)),((_tmpC43[0]=((
_tmpC42.tag=5,((_tmpC42.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line),((_tmpC42.f2=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpC42)))))),_tmpC43)))),((_tmpC41->tl=_tmp61C->tms,_tmpC41))))));}yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1));break;}case 176: _LL2DD:{struct Cyc_List_List*_tmpC4D;struct Cyc_Absyn_Carray_mod_struct
_tmpC4C;struct Cyc_Absyn_Carray_mod_struct*_tmpC4B;struct Cyc_Parse_Declarator*
_tmpC4A;yyval=Cyc_YY29(((_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))))->id,((_tmpC4A->tms=((_tmpC4D=_cycalloc(sizeof(*_tmpC4D)),((
_tmpC4D->hd=(void*)((_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B[0]=((_tmpC4C.tag=
0,((_tmpC4C.f1=Cyc_yyget_YY51(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((_tmpC4C.f2=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpC4C)))))),_tmpC4B)))),((
_tmpC4D->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,_tmpC4D)))))),_tmpC4A)))))));}
break;case 177: _LL2DE:{struct Cyc_List_List*_tmpC57;struct Cyc_Absyn_ConstArray_mod_struct
_tmpC56;struct Cyc_Absyn_ConstArray_mod_struct*_tmpC55;struct Cyc_Parse_Declarator*
_tmpC54;yyval=Cyc_YY29(((_tmpC54=_cycalloc(sizeof(*_tmpC54)),((_tmpC54->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->id,((_tmpC54->tms=((_tmpC57=_cycalloc(sizeof(*_tmpC57)),((
_tmpC57->hd=(void*)((_tmpC55=_cycalloc(sizeof(*_tmpC55)),((_tmpC55[0]=((_tmpC56.tag=
1,((_tmpC56.f1=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((_tmpC56.f2=Cyc_yyget_YY51(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpC56.f3=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpC56)))))))),_tmpC55)))),((_tmpC57->tl=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->tms,_tmpC57)))))),_tmpC54)))))));}break;case 178: _LL2DF: {
struct _tuple23 _tmp629;struct Cyc_List_List*_tmp62A;int _tmp62B;struct Cyc_Absyn_VarargInfo*
_tmp62C;struct Cyc_Core_Opt*_tmp62D;struct Cyc_List_List*_tmp62E;struct _tuple23*
_tmp628=Cyc_yyget_YY39(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));_tmp629=*_tmp628;_tmp62A=_tmp629.f1;
_tmp62B=_tmp629.f2;_tmp62C=_tmp629.f3;_tmp62D=_tmp629.f4;_tmp62E=_tmp629.f5;{
struct Cyc_List_List*_tmpC6C;struct Cyc_Absyn_Function_mod_struct _tmpC6B;struct Cyc_Absyn_WithTypes_struct
_tmpC6A;struct Cyc_Absyn_WithTypes_struct*_tmpC69;struct Cyc_Absyn_Function_mod_struct*
_tmpC68;struct Cyc_Parse_Declarator*_tmpC67;yyval=Cyc_YY29(((_tmpC67=_cycalloc(
sizeof(*_tmpC67)),((_tmpC67->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->id,((
_tmpC67->tms=((_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C->hd=(void*)((
_tmpC68=_cycalloc(sizeof(*_tmpC68)),((_tmpC68[0]=((_tmpC6B.tag=3,((_tmpC6B.f1=(
void*)((void*)((_tmpC69=_cycalloc(sizeof(*_tmpC69)),((_tmpC69[0]=((_tmpC6A.tag=1,((
_tmpC6A.f1=_tmp62A,((_tmpC6A.f2=_tmp62B,((_tmpC6A.f3=_tmp62C,((_tmpC6A.f4=
_tmp62D,((_tmpC6A.f5=_tmp62E,_tmpC6A)))))))))))),_tmpC69))))),_tmpC6B)))),
_tmpC68)))),((_tmpC6C->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,
_tmpC6C)))))),_tmpC67)))))));}break;}case 179: _LL2E0:{struct Cyc_List_List*_tmpC81;
struct Cyc_Absyn_Function_mod_struct _tmpC80;struct Cyc_Absyn_WithTypes_struct
_tmpC7F;struct Cyc_Absyn_WithTypes_struct*_tmpC7E;struct Cyc_Absyn_Function_mod_struct*
_tmpC7D;struct Cyc_Parse_Declarator*_tmpC7C;yyval=Cyc_YY29(((_tmpC7C=_cycalloc(
sizeof(*_tmpC7C)),((_tmpC7C->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))))->id,((
_tmpC7C->tms=((_tmpC81=_cycalloc(sizeof(*_tmpC81)),((_tmpC81->hd=(void*)((
_tmpC7D=_cycalloc(sizeof(*_tmpC7D)),((_tmpC7D[0]=((_tmpC80.tag=3,((_tmpC80.f1=(
void*)((void*)((_tmpC7E=_cycalloc(sizeof(*_tmpC7E)),((_tmpC7E[0]=((_tmpC7F.tag=1,((
_tmpC7F.f1=0,((_tmpC7F.f2=0,((_tmpC7F.f3=0,((_tmpC7F.f4=Cyc_yyget_YY49(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpC7F.f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpC7F)))))))))))),_tmpC7E))))),_tmpC80)))),_tmpC7D)))),((_tmpC81->tl=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->tms,_tmpC81)))))),_tmpC7C)))))));}break;case 180: _LL2E1:{
struct Cyc_List_List*_tmpC96;struct Cyc_Absyn_Function_mod_struct _tmpC95;struct Cyc_Absyn_NoTypes_struct
_tmpC94;struct Cyc_Absyn_NoTypes_struct*_tmpC93;struct Cyc_Absyn_Function_mod_struct*
_tmpC92;struct Cyc_Parse_Declarator*_tmpC91;yyval=Cyc_YY29(((_tmpC91=_cycalloc(
sizeof(*_tmpC91)),((_tmpC91->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->id,((
_tmpC91->tms=((_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96->hd=(void*)((
_tmpC92=_cycalloc(sizeof(*_tmpC92)),((_tmpC92[0]=((_tmpC95.tag=3,((_tmpC95.f1=(
void*)((void*)((_tmpC93=_cycalloc(sizeof(*_tmpC93)),((_tmpC93[0]=((_tmpC94.tag=0,((
_tmpC94.f1=Cyc_yyget_YY36(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((_tmpC94.f2=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpC94)))))),_tmpC93))))),
_tmpC95)))),_tmpC92)))),((_tmpC96->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,
_tmpC96)))))),_tmpC91)))))));}break;case 181: _LL2E2: {struct Cyc_List_List*_tmp641=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)))));{struct Cyc_List_List*_tmpCA0;struct Cyc_Absyn_TypeParams_mod_struct
_tmpC9F;struct Cyc_Absyn_TypeParams_mod_struct*_tmpC9E;struct Cyc_Parse_Declarator*
_tmpC9D;yyval=Cyc_YY29(((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))))->id,((_tmpC9D->tms=((_tmpCA0=_cycalloc(sizeof(*_tmpCA0)),((
_tmpCA0->hd=(void*)((_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E[0]=((_tmpC9F.tag=
4,((_tmpC9F.f1=_tmp641,((_tmpC9F.f2=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpC9F.f3=0,_tmpC9F)))))))),_tmpC9E)))),((_tmpCA0->tl=(
Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,_tmpCA0)))))),_tmpC9D)))))));}break;}case
182: _LL2E3:{struct Cyc_List_List*_tmpCAA;struct Cyc_Absyn_Attributes_mod_struct
_tmpCA9;struct Cyc_Absyn_Attributes_mod_struct*_tmpCA8;struct Cyc_Parse_Declarator*
_tmpCA7;yyval=Cyc_YY29(((_tmpCA7=_cycalloc(sizeof(*_tmpCA7)),((_tmpCA7->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))))->id,((_tmpCA7->tms=((_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((
_tmpCAA->hd=(void*)((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8[0]=((_tmpCA9.tag=
5,((_tmpCA9.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpCA9.f2=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCA9)))))),
_tmpCA8)))),((_tmpCAA->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))))->tms,
_tmpCAA)))))),_tmpCA7)))))));}break;case 183: _LL2E4: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 184: _LL2E5: yyval=Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_yyget_YY28(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))));break;case 185: _LL2E6: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)))!= 0){struct Cyc_Absyn_Attributes_mod_struct*_tmpCB0;struct Cyc_Absyn_Attributes_mod_struct
_tmpCAF;struct Cyc_List_List*_tmpCAE;ans=((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((
_tmpCAE->hd=(void*)((_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0[0]=((_tmpCAF.tag=
5,((_tmpCAF.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),((_tmpCAF.f2=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpCAF)))))),_tmpCB0)))),((_tmpCAE->tl=ans,_tmpCAE))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;struct Cyc_Position_Segment*_tmp64E;union Cyc_Absyn_Constraint*_tmp64F;
union Cyc_Absyn_Constraint*_tmp650;struct _tuple18 _tmp64D=*Cyc_yyget_YY1(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4)));_tmp64E=_tmp64D.f1;_tmp64F=_tmp64D.f2;_tmp650=_tmp64D.f3;if(
Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*_tmpCB1;ptrloc=((_tmpCB1=
_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1->ptr_loc=_tmp64E,((_tmpCB1->rgn_loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line),((_tmpCB1->zt_loc=
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line),
_tmpCB1))))))));}{union Cyc_Absyn_Constraint*_tmp653;union Cyc_Absyn_Constraint*
_tmp654;union Cyc_Absyn_Constraint*_tmp655;void*_tmp656;struct _tuple11 _tmp652=Cyc_Parse_collapse_pointer_quals(
_tmp64E,_tmp64F,_tmp650,Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY54(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))));_tmp653=_tmp652.f1;_tmp654=_tmp652.f2;_tmp655=_tmp652.f3;
_tmp656=_tmp652.f4;{struct Cyc_Absyn_Pointer_mod_struct*_tmpCBB;struct Cyc_Absyn_PtrAtts
_tmpCBA;struct Cyc_Absyn_Pointer_mod_struct _tmpCB9;struct Cyc_List_List*_tmpCB8;
ans=((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8->hd=(void*)((_tmpCBB=
_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB[0]=((_tmpCB9.tag=2,((_tmpCB9.f1=((_tmpCBA.rgn=
_tmp656,((_tmpCBA.nullable=_tmp653,((_tmpCBA.bounds=_tmp654,((_tmpCBA.zero_term=
_tmp655,((_tmpCBA.ptrloc=ptrloc,_tmpCBA)))))))))),((_tmpCB9.f2=Cyc_yyget_YY25(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpCB9)))))),_tmpCBB)))),((_tmpCB8->tl=ans,_tmpCB8))))));}yyval=
Cyc_YY28(ans);break;};};}case 186: _LL2E7: yyval=Cyc_YY54(0);break;case 187: _LL2E8:{
struct Cyc_List_List*_tmpCBC;yyval=Cyc_YY54(((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((
_tmpCBC->hd=Cyc_yyget_YY53(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((_tmpCBC->tl=Cyc_yyget_YY54(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpCBC)))))));}break;case 188: _LL2E9:{struct Cyc_Absyn_Numelts_ptrqual_struct
_tmpCBF;struct Cyc_Absyn_Numelts_ptrqual_struct*_tmpCBE;yyval=Cyc_YY53((void*)((
_tmpCBE=_cycalloc(sizeof(*_tmpCBE)),((_tmpCBE[0]=((_tmpCBF.tag=0,((_tmpCBF.f1=
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),_tmpCBF)))),_tmpCBE)))));}break;case 189: _LL2EA:{
struct Cyc_Absyn_Region_ptrqual_struct _tmpCC2;struct Cyc_Absyn_Region_ptrqual_struct*
_tmpCC1;yyval=Cyc_YY53((void*)((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1[0]=((
_tmpCC2.tag=1,((_tmpCC2.f1=(void*)Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpCC2)))),_tmpCC1)))));}break;case 190: _LL2EB:{struct Cyc_Absyn_Thin_ptrqual_struct
_tmpCC5;struct Cyc_Absyn_Thin_ptrqual_struct*_tmpCC4;yyval=Cyc_YY53((void*)((
_tmpCC4=_cycalloc_atomic(sizeof(*_tmpCC4)),((_tmpCC4[0]=((_tmpCC5.tag=2,_tmpCC5)),
_tmpCC4)))));}break;case 191: _LL2EC:{struct Cyc_Absyn_Fat_ptrqual_struct _tmpCC8;
struct Cyc_Absyn_Fat_ptrqual_struct*_tmpCC7;yyval=Cyc_YY53((void*)((_tmpCC7=
_cycalloc_atomic(sizeof(*_tmpCC7)),((_tmpCC7[0]=((_tmpCC8.tag=3,_tmpCC8)),
_tmpCC7)))));}break;case 192: _LL2ED:{struct Cyc_Absyn_Zeroterm_ptrqual_struct
_tmpCCB;struct Cyc_Absyn_Zeroterm_ptrqual_struct*_tmpCCA;yyval=Cyc_YY53((void*)((
_tmpCCA=_cycalloc_atomic(sizeof(*_tmpCCA)),((_tmpCCA[0]=((_tmpCCB.tag=4,_tmpCCB)),
_tmpCCA)))));}break;case 193: _LL2EE:{struct Cyc_Absyn_Nozeroterm_ptrqual_struct
_tmpCCE;struct Cyc_Absyn_Nozeroterm_ptrqual_struct*_tmpCCD;yyval=Cyc_YY53((void*)((
_tmpCCD=_cycalloc_atomic(sizeof(*_tmpCCD)),((_tmpCCD[0]=((_tmpCCE.tag=5,_tmpCCE)),
_tmpCCD)))));}break;case 194: _LL2EF:{struct Cyc_Absyn_Notnull_ptrqual_struct
_tmpCD1;struct Cyc_Absyn_Notnull_ptrqual_struct*_tmpCD0;yyval=Cyc_YY53((void*)((
_tmpCD0=_cycalloc_atomic(sizeof(*_tmpCD0)),((_tmpCD0[0]=((_tmpCD1.tag=6,_tmpCD1)),
_tmpCD0)))));}break;case 195: _LL2F0:{struct Cyc_Absyn_Nullable_ptrqual_struct
_tmpCD4;struct Cyc_Absyn_Nullable_ptrqual_struct*_tmpCD3;yyval=Cyc_YY53((void*)((
_tmpCD3=_cycalloc_atomic(sizeof(*_tmpCD3)),((_tmpCD3[0]=((_tmpCD4.tag=7,_tmpCD4)),
_tmpCD3)))));}break;case 196: _LL2F1: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line);{struct _tuple18*
_tmpCD5;yyval=Cyc_YY1(((_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->f1=loc,((
_tmpCD5->f2=Cyc_Absyn_true_conref,((_tmpCD5->f3=Cyc_yyget_YY2(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCD5)))))))));}
break;}case 197: _LL2F2: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line);{struct _tuple18*
_tmpCD6;yyval=Cyc_YY1(((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->f1=loc,((
_tmpCD6->f2=Cyc_Absyn_false_conref,((_tmpCD6->f3=Cyc_yyget_YY2(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCD6)))))))));}
break;}case 198: _LL2F3: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line);{struct _tuple18*
_tmpCD7;yyval=Cyc_YY1(((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->f1=loc,((
_tmpCD7->f2=Cyc_Absyn_true_conref,((_tmpCD7->f3=Cyc_Absyn_bounds_dyneither_conref,
_tmpCD7)))))))));}break;}case 199: _LL2F4: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 200: _LL2F5:{struct Cyc_Absyn_Upper_b_struct _tmpCDA;struct Cyc_Absyn_Upper_b_struct*
_tmpCD9;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9[0]=((_tmpCDA.tag=1,((
_tmpCDA.f1=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),_tmpCDA)))),_tmpCD9))))));}break;
case 201: _LL2F6: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 202: _LL2F7: yyval=Cyc_YY51(Cyc_Absyn_true_conref);break;case 203: _LL2F8:
yyval=Cyc_YY51(Cyc_Absyn_false_conref);break;case 204: _LL2F9: yyval=Cyc_YY44(Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));break;case 205: _LL2FA: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),& Cyc_Tcutil_trk,1);yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 206: _LL2FB: yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,
0));break;case 207: _LL2FC: yyval=Cyc_YY25(Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset + 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).last_line)));break;case 208:
_LL2FD: yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_yyget_YY25(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))));break;case 209: _LL2FE:{struct _tuple23*_tmpCDB;yyval=Cyc_YY39(((
_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),((
_tmpCDB->f2=0,((_tmpCDB->f3=0,((_tmpCDB->f4=Cyc_yyget_YY49(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpCDB->f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCDB)))))))))))));}break;case 210:
_LL2FF:{struct _tuple23*_tmpCDC;yyval=Cyc_YY39(((_tmpCDC=_cycalloc(sizeof(*
_tmpCDC)),((_tmpCDC->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 4)))),((_tmpCDC->f2=1,((_tmpCDC->f3=0,((_tmpCDC->f4=
Cyc_yyget_YY49(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),((_tmpCDC->f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCDC)))))))))))));}
break;case 211: _LL300: {struct _tuple8 _tmp674;struct Cyc_Core_Opt*_tmp675;struct Cyc_Absyn_Tqual
_tmp676;void*_tmp677;struct _tuple8*_tmp673=Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)));
_tmp674=*_tmp673;_tmp675=_tmp674.f1;_tmp676=_tmp674.f2;_tmp677=_tmp674.f3;{
struct Cyc_Absyn_VarargInfo*_tmpCDD;struct Cyc_Absyn_VarargInfo*_tmp678=(_tmpCDD=
_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->name=_tmp675,((_tmpCDD->tq=_tmp676,((
_tmpCDD->type=_tmp677,((_tmpCDD->inject=Cyc_yyget_YY31(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpCDD)))))))));{struct _tuple23*_tmpCDE;yyval=Cyc_YY39(((_tmpCDE=_cycalloc(
sizeof(*_tmpCDE)),((_tmpCDE->f1=0,((_tmpCDE->f2=0,((_tmpCDE->f3=_tmp678,((
_tmpCDE->f4=Cyc_yyget_YY49(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((_tmpCDE->f5=Cyc_yyget_YY50(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpCDE)))))))))))));}break;};}case 212: _LL301: {struct _tuple8
_tmp67C;struct Cyc_Core_Opt*_tmp67D;struct Cyc_Absyn_Tqual _tmp67E;void*_tmp67F;
struct _tuple8*_tmp67B=Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)));
_tmp67C=*_tmp67B;_tmp67D=_tmp67C.f1;_tmp67E=_tmp67C.f2;_tmp67F=_tmp67C.f3;{
struct Cyc_Absyn_VarargInfo*_tmpCDF;struct Cyc_Absyn_VarargInfo*_tmp680=(_tmpCDF=
_cycalloc(sizeof(*_tmpCDF)),((_tmpCDF->name=_tmp67D,((_tmpCDF->tq=_tmp67E,((
_tmpCDF->type=_tmp67F,((_tmpCDF->inject=Cyc_yyget_YY31(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpCDF)))))))));{struct _tuple23*_tmpCE0;yyval=Cyc_YY39(((_tmpCE0=_cycalloc(
sizeof(*_tmpCE0)),((_tmpCE0->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6)))),((_tmpCE0->f2=0,((_tmpCE0->f3=
_tmp680,((_tmpCE0->f4=Cyc_yyget_YY49(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpCE0->f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCE0)))))))))))));}break;};}case 213:
_LL302:{struct Cyc_Absyn_Unknown_kb_struct _tmpCE3;struct Cyc_Absyn_Unknown_kb_struct*
_tmpCE2;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),(void*)((
_tmpCE2=_cycalloc(sizeof(*_tmpCE2)),((_tmpCE2[0]=((_tmpCE3.tag=1,((_tmpCE3.f1=0,
_tmpCE3)))),_tmpCE2))))));}break;case 214: _LL303: yyval=Cyc_YY44(Cyc_Parse_id2type(
Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))));break;case 215: _LL304: yyval=Cyc_YY49(0);break;case 216: _LL305:{
struct Cyc_Absyn_JoinEff_struct*_tmpCE9;struct Cyc_Absyn_JoinEff_struct _tmpCE8;
struct Cyc_Core_Opt*_tmpCE7;yyval=Cyc_YY49(((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((
_tmpCE7->v=(void*)((_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9[0]=((_tmpCE8.tag=
25,((_tmpCE8.f1=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCE8)))),_tmpCE9)))),_tmpCE7)))));}
break;case 217: _LL306: yyval=Cyc_YY50(0);break;case 218: _LL307: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 219: _LL308: {struct Cyc_Absyn_Less_kb_struct _tmpCEC;struct Cyc_Absyn_Less_kb_struct*
_tmpCEB;struct Cyc_Absyn_Less_kb_struct*_tmp688=(_tmpCEB=_cycalloc(sizeof(*
_tmpCEB)),((_tmpCEB[0]=((_tmpCEC.tag=2,((_tmpCEC.f1=0,((_tmpCEC.f2=& Cyc_Tcutil_trk,
_tmpCEC)))))),_tmpCEB)));struct _dyneither_ptr _tmp689=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)));void*_tmp68A=Cyc_Parse_id2type(_tmp689,(void*)_tmp688);{struct
_tuple26*_tmpCF6;struct Cyc_Absyn_JoinEff_struct _tmpCF5;struct Cyc_Absyn_JoinEff_struct*
_tmpCF4;struct Cyc_List_List*_tmpCF3;yyval=Cyc_YY50(((_tmpCF3=_cycalloc(sizeof(*
_tmpCF3)),((_tmpCF3->hd=((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->f1=(
void*)((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((_tmpCF4[0]=((_tmpCF5.tag=25,((
_tmpCF5.f1=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpCF5)))),_tmpCF4)))),((_tmpCF6->f2=
_tmp68A,_tmpCF6)))))),((_tmpCF3->tl=0,_tmpCF3)))))));}break;}case 220: _LL309: {
struct Cyc_Absyn_Less_kb_struct _tmpCF9;struct Cyc_Absyn_Less_kb_struct*_tmpCF8;
struct Cyc_Absyn_Less_kb_struct*_tmp691=(_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((
_tmpCF8[0]=((_tmpCF9.tag=2,((_tmpCF9.f1=0,((_tmpCF9.f2=& Cyc_Tcutil_trk,_tmpCF9)))))),
_tmpCF8)));struct _dyneither_ptr _tmp692=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)));void*
_tmp693=Cyc_Parse_id2type(_tmp692,(void*)_tmp691);{struct _tuple26*_tmpD03;struct
Cyc_Absyn_JoinEff_struct _tmpD02;struct Cyc_Absyn_JoinEff_struct*_tmpD01;struct Cyc_List_List*
_tmpD00;yyval=Cyc_YY50(((_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00->hd=((
_tmpD03=_cycalloc(sizeof(*_tmpD03)),((_tmpD03->f1=(void*)((_tmpD01=_cycalloc(
sizeof(*_tmpD01)),((_tmpD01[0]=((_tmpD02.tag=25,((_tmpD02.f1=Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))),_tmpD02)))),_tmpD01)))),((_tmpD03->f2=_tmp693,_tmpD03)))))),((
_tmpD00->tl=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD00)))))));}break;}case 221: _LL30A:
yyval=Cyc_YY31(0);break;case 222: _LL30B:{const char*_tmpD04;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpD04="inject",_tag_dyneither(_tmpD04,sizeof(char),7))))!= 0){const char*
_tmpD05;Cyc_Parse_err(((_tmpD05="missing type in function declaration",
_tag_dyneither(_tmpD05,sizeof(char),37))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));}}yyval=Cyc_YY31(1);
break;case 223: _LL30C: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 224: _LL30D: yyval=Cyc_YY40(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))));
break;case 225: _LL30E: yyval=Cyc_YY40(0);break;case 226: _LL30F: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1));
break;case 227: _LL310:{struct Cyc_Absyn_RgnsEff_struct*_tmpD0B;struct Cyc_Absyn_RgnsEff_struct
_tmpD0A;struct Cyc_List_List*_tmpD09;yyval=Cyc_YY40(((_tmpD09=_cycalloc(sizeof(*
_tmpD09)),((_tmpD09->hd=(void*)((_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B[0]=((
_tmpD0A.tag=26,((_tmpD0A.f1=(void*)Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpD0A)))),_tmpD0B)))),((_tmpD09->tl=0,_tmpD09)))))));}break;case 228: _LL311: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),& Cyc_Tcutil_ek,0);{struct Cyc_List_List*_tmpD0C;
yyval=Cyc_YY40(((_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C->hd=Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD0C->tl=0,_tmpD0C)))))));}break;case 229: _LL312: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),& Cyc_Tcutil_trk,1);{struct Cyc_Absyn_AccessEff_struct*
_tmpD12;struct Cyc_Absyn_AccessEff_struct _tmpD11;struct Cyc_List_List*_tmpD10;
yyval=Cyc_YY40(((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10->hd=(void*)((
_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12[0]=((_tmpD11.tag=24,((_tmpD11.f1=(
void*)Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD11)))),_tmpD12)))),((_tmpD10->tl=0,
_tmpD10)))))));}break;case 230: _LL313: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),& Cyc_Tcutil_trk,1);{struct Cyc_Absyn_AccessEff_struct*_tmpD18;
struct Cyc_Absyn_AccessEff_struct _tmpD17;struct Cyc_List_List*_tmpD16;yyval=Cyc_YY40(((
_tmpD16=_cycalloc(sizeof(*_tmpD16)),((_tmpD16->hd=(void*)((_tmpD18=_cycalloc(
sizeof(*_tmpD18)),((_tmpD18[0]=((_tmpD17.tag=24,((_tmpD17.f1=(void*)Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),_tmpD17)))),_tmpD18)))),((_tmpD16->tl=Cyc_yyget_YY40(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpD16)))))));}break;case 231: _LL314:{struct Cyc_List_List*
_tmpD19;yyval=Cyc_YY38(((_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19->hd=Cyc_yyget_YY37(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD19->tl=0,_tmpD19)))))));}break;case 232: _LL315:{struct Cyc_List_List*
_tmpD1A;yyval=Cyc_YY38(((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((_tmpD1A->hd=Cyc_yyget_YY37(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD1A->tl=Cyc_yyget_YY38(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpD1A)))))));}break;case 233: _LL316: {struct _tuple22 _tmp6A9;struct Cyc_Absyn_Tqual
_tmp6AA;struct Cyc_List_List*_tmp6AB;struct Cyc_List_List*_tmp6AC;struct _tuple22*
_tmp6A8=Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));_tmp6A9=*_tmp6A8;_tmp6AA=_tmp6A9.f1;
_tmp6AB=_tmp6A9.f2;_tmp6AC=_tmp6A9.f3;if(_tmp6AA.loc == 0)_tmp6AA.loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line);{struct Cyc_Parse_Declarator
_tmp6AE;struct _tuple1*_tmp6AF;struct Cyc_List_List*_tmp6B0;struct Cyc_Parse_Declarator*
_tmp6AD=Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset)));_tmp6AE=*_tmp6AD;_tmp6AF=_tmp6AE.id;
_tmp6B0=_tmp6AE.tms;{void*_tmp6B1=Cyc_Parse_speclist2typ(_tmp6AB,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));struct Cyc_Absyn_Tqual
_tmp6B3;void*_tmp6B4;struct Cyc_List_List*_tmp6B5;struct Cyc_List_List*_tmp6B6;
struct _tuple10 _tmp6B2=Cyc_Parse_apply_tms(_tmp6AA,_tmp6B1,_tmp6AC,_tmp6B0);
_tmp6B3=_tmp6B2.f1;_tmp6B4=_tmp6B2.f2;_tmp6B5=_tmp6B2.f3;_tmp6B6=_tmp6B2.f4;if(
_tmp6B5 != 0){const char*_tmpD1B;Cyc_Parse_err(((_tmpD1B="parameter with bad type params",
_tag_dyneither(_tmpD1B,sizeof(char),31))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));}if(Cyc_Absyn_is_qvar_qualified(
_tmp6AF)){const char*_tmpD1C;Cyc_Parse_err(((_tmpD1C="parameter cannot be qualified with a namespace",
_tag_dyneither(_tmpD1C,sizeof(char),47))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));}{struct Cyc_Core_Opt*
_tmpD1D;struct Cyc_Core_Opt*_tmp6B9=(struct Cyc_Core_Opt*)((_tmpD1D=_cycalloc(
sizeof(*_tmpD1D)),((_tmpD1D->v=(*_tmp6AF).f2,_tmpD1D))));if(_tmp6B6 != 0){const
char*_tmpD20;void*_tmpD1F;(_tmpD1F=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpD20="extra attributes on parameter, ignoring",
_tag_dyneither(_tmpD20,sizeof(char),40))),_tag_dyneither(_tmpD1F,sizeof(void*),0)));}{
struct _tuple8*_tmpD21;yyval=Cyc_YY37(((_tmpD21=_cycalloc(sizeof(*_tmpD21)),((
_tmpD21->f1=_tmp6B9,((_tmpD21->f2=_tmp6B3,((_tmpD21->f3=_tmp6B4,_tmpD21)))))))));}
break;};};};}case 234: _LL317: {struct _tuple22 _tmp6BF;struct Cyc_Absyn_Tqual _tmp6C0;
struct Cyc_List_List*_tmp6C1;struct Cyc_List_List*_tmp6C2;struct _tuple22*_tmp6BE=
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)));_tmp6BF=*_tmp6BE;_tmp6C0=_tmp6BF.f1;_tmp6C1=_tmp6BF.f2;
_tmp6C2=_tmp6BF.f3;if(_tmp6C0.loc == 0)_tmp6C0.loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line);{void*_tmp6C3=Cyc_Parse_speclist2typ(
_tmp6C1,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));if(_tmp6C2 != 0){const char*_tmpD24;void*_tmpD23;(
_tmpD23=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpD24="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpD24,sizeof(char),38))),_tag_dyneither(_tmpD23,sizeof(void*),0)));}{
struct _tuple8*_tmpD25;yyval=Cyc_YY37(((_tmpD25=_cycalloc(sizeof(*_tmpD25)),((
_tmpD25->f1=0,((_tmpD25->f2=_tmp6C0,((_tmpD25->f3=_tmp6C3,_tmpD25)))))))));}
break;};}case 235: _LL318: {struct _tuple22 _tmp6C8;struct Cyc_Absyn_Tqual _tmp6C9;
struct Cyc_List_List*_tmp6CA;struct Cyc_List_List*_tmp6CB;struct _tuple22*_tmp6C7=
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1)));_tmp6C8=*_tmp6C7;_tmp6C9=_tmp6C8.f1;_tmp6CA=
_tmp6C8.f2;_tmp6CB=_tmp6C8.f3;if(_tmp6C9.loc == 0)_tmp6C9.loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line);{void*_tmp6CC=Cyc_Parse_speclist2typ(
_tmp6CA,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));struct Cyc_List_List*_tmp6CD=(Cyc_yyget_YY30(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->tms;struct Cyc_Absyn_Tqual _tmp6CF;void*_tmp6D0;struct Cyc_List_List*
_tmp6D1;struct Cyc_List_List*_tmp6D2;struct _tuple10 _tmp6CE=Cyc_Parse_apply_tms(
_tmp6C9,_tmp6CC,_tmp6CB,_tmp6CD);_tmp6CF=_tmp6CE.f1;_tmp6D0=_tmp6CE.f2;_tmp6D1=
_tmp6CE.f3;_tmp6D2=_tmp6CE.f4;if(_tmp6D1 != 0){const char*_tmpD28;void*_tmpD27;(
_tmpD27=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpD28="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpD28,sizeof(char),49))),_tag_dyneither(_tmpD27,sizeof(void*),0)));}
if(_tmp6D2 != 0){const char*_tmpD2B;void*_tmpD2A;(_tmpD2A=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpD2B="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpD2B,sizeof(char),38))),_tag_dyneither(_tmpD2A,sizeof(void*),0)));}{
struct _tuple8*_tmpD2C;yyval=Cyc_YY37(((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((
_tmpD2C->f1=0,((_tmpD2C->f2=_tmp6CF,((_tmpD2C->f3=_tmp6D0,_tmpD2C)))))))));}
break;};}case 236: _LL319: yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY36(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))));
break;case 237: _LL31A:{struct _dyneither_ptr*_tmpD2F;struct Cyc_List_List*_tmpD2E;
yyval=Cyc_YY36(((_tmpD2E=_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E->hd=((_tmpD2F=
_cycalloc(sizeof(*_tmpD2F)),((_tmpD2F[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD2F)))),((
_tmpD2E->tl=0,_tmpD2E)))))));}break;case 238: _LL31B:{struct _dyneither_ptr*_tmpD32;
struct Cyc_List_List*_tmpD31;yyval=Cyc_YY36(((_tmpD31=_cycalloc(sizeof(*_tmpD31)),((
_tmpD31->hd=((_tmpD32=_cycalloc(sizeof(*_tmpD32)),((_tmpD32[0]=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpD32)))),((_tmpD31->tl=Cyc_yyget_YY36(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpD31)))))));}break;case 239: _LL31C: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 240: _LL31D: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 241: _LL31E:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD35;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD34;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD34=_cycalloc(sizeof(*_tmpD34)),((_tmpD34[0]=((_tmpD35.tag=36,((
_tmpD35.f1=0,((_tmpD35.f2=0,_tmpD35)))))),_tmpD34)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 242:
_LL31F:{struct Cyc_Absyn_UnresolvedMem_e_struct _tmpD38;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpD37;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD37=_cycalloc(sizeof(*
_tmpD37)),((_tmpD37[0]=((_tmpD38.tag=36,((_tmpD38.f1=0,((_tmpD38.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)))),
_tmpD38)))))),_tmpD37)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 243: _LL320:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD3B;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD3A;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD3A=_cycalloc(sizeof(*_tmpD3A)),((_tmpD3A[0]=((_tmpD3B.tag=36,((
_tmpD3B.f1=0,((_tmpD3B.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2)))),_tmpD3B)))))),_tmpD3A)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 244:
_LL321: {struct _dyneither_ptr*_tmpD3E;struct _tuple1*_tmpD3D;struct Cyc_Absyn_Vardecl*
_tmp6E2=Cyc_Absyn_new_vardecl(((_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D->f1=
Cyc_Absyn_Loc_n,((_tmpD3D->f2=((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E[0]=
Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))),_tmpD3E)))),_tmpD3D)))))),Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).last_line)));(_tmp6E2->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct _tmpD41;struct Cyc_Absyn_Comprehension_e_struct*
_tmpD40;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD40=_cycalloc(sizeof(*
_tmpD40)),((_tmpD40[0]=((_tmpD41.tag=28,((_tmpD41.f1=_tmp6E2,((_tmpD41.f2=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpD41.f3=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpD41.f4=0,_tmpD41)))))))))),_tmpD40)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 245:
_LL322:{struct _tuple27*_tmpD44;struct Cyc_List_List*_tmpD43;yyval=Cyc_YY6(((
_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43->hd=((_tmpD44=_cycalloc(sizeof(*
_tmpD44)),((_tmpD44->f1=0,((_tmpD44->f2=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD44)))))),((
_tmpD43->tl=0,_tmpD43)))))));}break;case 246: _LL323:{struct _tuple27*_tmpD47;
struct Cyc_List_List*_tmpD46;yyval=Cyc_YY6(((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((
_tmpD46->hd=((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47->f1=Cyc_yyget_YY41(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpD47->f2=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD47)))))),((
_tmpD46->tl=0,_tmpD46)))))));}break;case 247: _LL324:{struct _tuple27*_tmpD4A;
struct Cyc_List_List*_tmpD49;yyval=Cyc_YY6(((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((
_tmpD49->hd=((_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A->f1=0,((_tmpD4A->f2=
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),_tmpD4A)))))),((_tmpD49->tl=Cyc_yyget_YY6(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),_tmpD49)))))));}break;case 248: _LL325:{struct _tuple27*_tmpD4D;
struct Cyc_List_List*_tmpD4C;yyval=Cyc_YY6(((_tmpD4C=_cycalloc(sizeof(*_tmpD4C)),((
_tmpD4C->hd=((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D->f1=Cyc_yyget_YY41(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpD4D->f2=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD4D)))))),((
_tmpD4C->tl=Cyc_yyget_YY6(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),_tmpD4C)))))));}break;case 249: _LL326:
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY41(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1)))));break;case 250: _LL327:{struct Cyc_List_List*
_tmpD4E;yyval=Cyc_YY41(((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E->hd=Cyc_yyget_YY42(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD4E->tl=0,_tmpD4E)))))));}break;case 251: _LL328:{struct Cyc_List_List*
_tmpD4F;yyval=Cyc_YY41(((_tmpD4F=_cycalloc(sizeof(*_tmpD4F)),((_tmpD4F->hd=Cyc_yyget_YY42(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD4F->tl=Cyc_yyget_YY41(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpD4F)))))));}break;case 252: _LL329:{struct Cyc_Absyn_ArrayElement_struct _tmpD52;
struct Cyc_Absyn_ArrayElement_struct*_tmpD51;yyval=Cyc_YY42((void*)((_tmpD51=
_cycalloc(sizeof(*_tmpD51)),((_tmpD51[0]=((_tmpD52.tag=0,((_tmpD52.f1=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpD52)))),_tmpD51)))));}break;case 253: _LL32A:{struct Cyc_Absyn_FieldName_struct
_tmpD58;struct _dyneither_ptr*_tmpD57;struct Cyc_Absyn_FieldName_struct*_tmpD56;
yyval=Cyc_YY42((void*)((_tmpD56=_cycalloc(sizeof(*_tmpD56)),((_tmpD56[0]=((
_tmpD58.tag=1,((_tmpD58.f1=((_tmpD57=_cycalloc(sizeof(*_tmpD57)),((_tmpD57[0]=
Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD57)))),_tmpD58)))),_tmpD56)))));}
break;case 254: _LL32B: {struct _tuple22 _tmp6F7;struct Cyc_Absyn_Tqual _tmp6F8;struct
Cyc_List_List*_tmp6F9;struct Cyc_List_List*_tmp6FA;struct _tuple22*_tmp6F6=Cyc_yyget_YY35(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)));_tmp6F7=*_tmp6F6;_tmp6F8=_tmp6F7.f1;_tmp6F9=_tmp6F7.f2;_tmp6FA=
_tmp6F7.f3;{void*_tmp6FB=Cyc_Parse_speclist2typ(_tmp6F9,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));if(_tmp6FA != 0){const
char*_tmpD5B;void*_tmpD5A;(_tmpD5A=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpD5B="ignoring attributes in type",
_tag_dyneither(_tmpD5B,sizeof(char),28))),_tag_dyneither(_tmpD5A,sizeof(void*),0)));}{
struct _tuple8*_tmpD5C;yyval=Cyc_YY37(((_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((
_tmpD5C->f1=0,((_tmpD5C->f2=_tmp6F8,((_tmpD5C->f3=_tmp6FB,_tmpD5C)))))))));}
break;};}case 255: _LL32C: {struct _tuple22 _tmp700;struct Cyc_Absyn_Tqual _tmp701;
struct Cyc_List_List*_tmp702;struct Cyc_List_List*_tmp703;struct _tuple22*_tmp6FF=
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1)));_tmp700=*_tmp6FF;_tmp701=_tmp700.f1;_tmp702=
_tmp700.f2;_tmp703=_tmp700.f3;{void*_tmp704=Cyc_Parse_speclist2typ(_tmp702,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));struct Cyc_List_List*
_tmp705=(Cyc_yyget_YY30(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tms;struct _tuple10 _tmp706=Cyc_Parse_apply_tms(
_tmp701,_tmp704,_tmp703,_tmp705);if(_tmp706.f3 != 0){const char*_tmpD5F;void*
_tmpD5E;(_tmpD5E=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpD5F="bad type params, ignoring",_tag_dyneither(
_tmpD5F,sizeof(char),26))),_tag_dyneither(_tmpD5E,sizeof(void*),0)));}if(_tmp706.f4
!= 0){const char*_tmpD62;void*_tmpD61;(_tmpD61=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpD62="bad specifiers, ignoring",
_tag_dyneither(_tmpD62,sizeof(char),25))),_tag_dyneither(_tmpD61,sizeof(void*),0)));}{
struct _tuple8*_tmpD63;yyval=Cyc_YY37(((_tmpD63=_cycalloc(sizeof(*_tmpD63)),((
_tmpD63->f1=0,((_tmpD63->f2=_tmp706.f1,((_tmpD63->f3=_tmp706.f2,_tmpD63)))))))));}
break;};}case 256: _LL32D: yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 257: _LL32E:{struct Cyc_Absyn_JoinEff_struct
_tmpD66;struct Cyc_Absyn_JoinEff_struct*_tmpD65;yyval=Cyc_YY44((void*)((_tmpD65=
_cycalloc(sizeof(*_tmpD65)),((_tmpD65[0]=((_tmpD66.tag=25,((_tmpD66.f1=0,_tmpD66)))),
_tmpD65)))));}break;case 258: _LL32F:{struct Cyc_Absyn_JoinEff_struct _tmpD69;struct
Cyc_Absyn_JoinEff_struct*_tmpD68;yyval=Cyc_YY44((void*)((_tmpD68=_cycalloc(
sizeof(*_tmpD68)),((_tmpD68[0]=((_tmpD69.tag=25,((_tmpD69.f1=Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpD69)))),_tmpD68)))));}break;case 259: _LL330:{struct Cyc_Absyn_RgnsEff_struct
_tmpD6C;struct Cyc_Absyn_RgnsEff_struct*_tmpD6B;yyval=Cyc_YY44((void*)((_tmpD6B=
_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B[0]=((_tmpD6C.tag=26,((_tmpD6C.f1=(void*)
Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),_tmpD6C)))),_tmpD6B)))));}break;case 260: _LL331:{
struct Cyc_Absyn_JoinEff_struct _tmpD72;struct Cyc_List_List*_tmpD71;struct Cyc_Absyn_JoinEff_struct*
_tmpD70;yyval=Cyc_YY44((void*)((_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70[0]=((
_tmpD72.tag=25,((_tmpD72.f1=((_tmpD71=_cycalloc(sizeof(*_tmpD71)),((_tmpD71->hd=
Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),((_tmpD71->tl=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD71)))))),
_tmpD72)))),_tmpD70)))));}break;case 261: _LL332:{struct Cyc_List_List*_tmpD73;
yyval=Cyc_YY40(((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73->hd=Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD73->tl=0,_tmpD73)))))));}break;case 262: _LL333:{struct Cyc_List_List*
_tmpD74;yyval=Cyc_YY40(((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74->hd=Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD74->tl=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpD74)))))));}break;case 263: _LL334:{struct Cyc_Parse_Abstractdeclarator*_tmpD75;
yyval=Cyc_YY30(((_tmpD75=_cycalloc(sizeof(*_tmpD75)),((_tmpD75->tms=Cyc_yyget_YY28(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpD75)))));}break;case 264: _LL335: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 265: _LL336:{struct Cyc_Parse_Abstractdeclarator*_tmpD76;yyval=Cyc_YY30(((
_tmpD76=_cycalloc(sizeof(*_tmpD76)),((_tmpD76->tms=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),(Cyc_yyget_YY30(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tms),
_tmpD76)))));}break;case 266: _LL337: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1));
break;case 267: _LL338:{struct Cyc_List_List*_tmpD80;struct Cyc_Absyn_Carray_mod_struct
_tmpD7F;struct Cyc_Absyn_Carray_mod_struct*_tmpD7E;struct Cyc_Parse_Abstractdeclarator*
_tmpD7D;yyval=Cyc_YY30(((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D->tms=((
_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80->hd=(void*)((_tmpD7E=_cycalloc(
sizeof(*_tmpD7E)),((_tmpD7E[0]=((_tmpD7F.tag=0,((_tmpD7F.f1=Cyc_yyget_YY51(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD7F.f2=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpD7F)))))),_tmpD7E)))),((_tmpD80->tl=0,_tmpD80)))))),
_tmpD7D)))));}break;case 268: _LL339:{struct Cyc_List_List*_tmpD8A;struct Cyc_Absyn_Carray_mod_struct
_tmpD89;struct Cyc_Absyn_Carray_mod_struct*_tmpD88;struct Cyc_Parse_Abstractdeclarator*
_tmpD87;yyval=Cyc_YY30(((_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->tms=((
_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A->hd=(void*)((_tmpD88=_cycalloc(
sizeof(*_tmpD88)),((_tmpD88[0]=((_tmpD89.tag=0,((_tmpD89.f1=Cyc_yyget_YY51(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD89.f2=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpD89)))))),_tmpD88)))),((_tmpD8A->tl=(Cyc_yyget_YY30(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))))->tms,_tmpD8A)))))),_tmpD87)))));}break;case 269: _LL33A:{
struct Cyc_List_List*_tmpD94;struct Cyc_Absyn_ConstArray_mod_struct _tmpD93;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD92;struct Cyc_Parse_Abstractdeclarator*
_tmpD91;yyval=Cyc_YY30(((_tmpD91=_cycalloc(sizeof(*_tmpD91)),((_tmpD91->tms=((
_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94->hd=(void*)((_tmpD92=_cycalloc(
sizeof(*_tmpD92)),((_tmpD92[0]=((_tmpD93.tag=1,((_tmpD93.f1=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpD93.f2=Cyc_yyget_YY51(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpD93.f3=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpD93)))))))),_tmpD92)))),((_tmpD94->tl=0,_tmpD94)))))),
_tmpD91)))));}break;case 270: _LL33B:{struct Cyc_List_List*_tmpD9E;struct Cyc_Absyn_ConstArray_mod_struct
_tmpD9D;struct Cyc_Absyn_ConstArray_mod_struct*_tmpD9C;struct Cyc_Parse_Abstractdeclarator*
_tmpD9B;yyval=Cyc_YY30(((_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B->tms=((
_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->hd=(void*)((_tmpD9C=_cycalloc(
sizeof(*_tmpD9C)),((_tmpD9C[0]=((_tmpD9D.tag=1,((_tmpD9D.f1=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpD9D.f2=Cyc_yyget_YY51(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpD9D.f3=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpD9D)))))))),_tmpD9C)))),((_tmpD9E->tl=(Cyc_yyget_YY30(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->tms,_tmpD9E)))))),_tmpD9B)))));}break;case 271: _LL33C:{
struct Cyc_List_List*_tmpDB3;struct Cyc_Absyn_Function_mod_struct _tmpDB2;struct Cyc_Absyn_WithTypes_struct
_tmpDB1;struct Cyc_Absyn_WithTypes_struct*_tmpDB0;struct Cyc_Absyn_Function_mod_struct*
_tmpDAF;struct Cyc_Parse_Abstractdeclarator*_tmpDAE;yyval=Cyc_YY30(((_tmpDAE=
_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->tms=((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((
_tmpDB3->hd=(void*)((_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF[0]=((_tmpDB2.tag=
3,((_tmpDB2.f1=(void*)((void*)((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0[0]=((
_tmpDB1.tag=1,((_tmpDB1.f1=0,((_tmpDB1.f2=0,((_tmpDB1.f3=0,((_tmpDB1.f4=Cyc_yyget_YY49(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpDB1.f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpDB1)))))))))))),_tmpDB0))))),_tmpDB2)))),_tmpDAF)))),((_tmpDB3->tl=0,_tmpDB3)))))),
_tmpDAE)))));}break;case 272: _LL33D: {struct _tuple23 _tmp730;struct Cyc_List_List*
_tmp731;int _tmp732;struct Cyc_Absyn_VarargInfo*_tmp733;struct Cyc_Core_Opt*_tmp734;
struct Cyc_List_List*_tmp735;struct _tuple23*_tmp72F=Cyc_yyget_YY39(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));
_tmp730=*_tmp72F;_tmp731=_tmp730.f1;_tmp732=_tmp730.f2;_tmp733=_tmp730.f3;
_tmp734=_tmp730.f4;_tmp735=_tmp730.f5;{struct Cyc_List_List*_tmpDC8;struct Cyc_Absyn_Function_mod_struct
_tmpDC7;struct Cyc_Absyn_WithTypes_struct _tmpDC6;struct Cyc_Absyn_WithTypes_struct*
_tmpDC5;struct Cyc_Absyn_Function_mod_struct*_tmpDC4;struct Cyc_Parse_Abstractdeclarator*
_tmpDC3;yyval=Cyc_YY30(((_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3->tms=((
_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8->hd=(void*)((_tmpDC4=_cycalloc(
sizeof(*_tmpDC4)),((_tmpDC4[0]=((_tmpDC7.tag=3,((_tmpDC7.f1=(void*)((void*)((
_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5[0]=((_tmpDC6.tag=1,((_tmpDC6.f1=
_tmp731,((_tmpDC6.f2=_tmp732,((_tmpDC6.f3=_tmp733,((_tmpDC6.f4=_tmp734,((_tmpDC6.f5=
_tmp735,_tmpDC6)))))))))))),_tmpDC5))))),_tmpDC7)))),_tmpDC4)))),((_tmpDC8->tl=0,
_tmpDC8)))))),_tmpDC3)))));}break;}case 273: _LL33E:{struct Cyc_List_List*_tmpDDD;
struct Cyc_Absyn_Function_mod_struct _tmpDDC;struct Cyc_Absyn_WithTypes_struct
_tmpDDB;struct Cyc_Absyn_WithTypes_struct*_tmpDDA;struct Cyc_Absyn_Function_mod_struct*
_tmpDD9;struct Cyc_Parse_Abstractdeclarator*_tmpDD8;yyval=Cyc_YY30(((_tmpDD8=
_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8->tms=((_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((
_tmpDDD->hd=(void*)((_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9[0]=((_tmpDDC.tag=
3,((_tmpDDC.f1=(void*)((void*)((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA[0]=((
_tmpDDB.tag=1,((_tmpDDB.f1=0,((_tmpDDB.f2=0,((_tmpDDB.f3=0,((_tmpDDB.f4=Cyc_yyget_YY49(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpDDB.f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpDDB)))))))))))),_tmpDDA))))),_tmpDDC)))),_tmpDD9)))),((_tmpDDD->tl=(Cyc_yyget_YY30(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->tms,_tmpDDD)))))),_tmpDD8)))));}break;case 274: _LL33F: {
struct _tuple23 _tmp743;struct Cyc_List_List*_tmp744;int _tmp745;struct Cyc_Absyn_VarargInfo*
_tmp746;struct Cyc_Core_Opt*_tmp747;struct Cyc_List_List*_tmp748;struct _tuple23*
_tmp742=Cyc_yyget_YY39(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));_tmp743=*_tmp742;_tmp744=_tmp743.f1;
_tmp745=_tmp743.f2;_tmp746=_tmp743.f3;_tmp747=_tmp743.f4;_tmp748=_tmp743.f5;{
struct Cyc_List_List*_tmpDF2;struct Cyc_Absyn_Function_mod_struct _tmpDF1;struct Cyc_Absyn_WithTypes_struct
_tmpDF0;struct Cyc_Absyn_WithTypes_struct*_tmpDEF;struct Cyc_Absyn_Function_mod_struct*
_tmpDEE;struct Cyc_Parse_Abstractdeclarator*_tmpDED;yyval=Cyc_YY30(((_tmpDED=
_cycalloc(sizeof(*_tmpDED)),((_tmpDED->tms=((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((
_tmpDF2->hd=(void*)((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE[0]=((_tmpDF1.tag=
3,((_tmpDF1.f1=(void*)((void*)((_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF[0]=((
_tmpDF0.tag=1,((_tmpDF0.f1=_tmp744,((_tmpDF0.f2=_tmp745,((_tmpDF0.f3=_tmp746,((
_tmpDF0.f4=_tmp747,((_tmpDF0.f5=_tmp748,_tmpDF0)))))))))))),_tmpDEF))))),_tmpDF1)))),
_tmpDEE)))),((_tmpDF2->tl=(Cyc_yyget_YY30(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,
_tmpDF2)))))),_tmpDED)))));}break;}case 275: _LL340: {struct Cyc_List_List*_tmp74F=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)))));{struct Cyc_List_List*_tmpDFC;struct Cyc_Absyn_TypeParams_mod_struct
_tmpDFB;struct Cyc_Absyn_TypeParams_mod_struct*_tmpDFA;struct Cyc_Parse_Abstractdeclarator*
_tmpDF9;yyval=Cyc_YY30(((_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9->tms=((
_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC->hd=(void*)((_tmpDFA=_cycalloc(
sizeof(*_tmpDFA)),((_tmpDFA[0]=((_tmpDFB.tag=4,((_tmpDFB.f1=_tmp74F,((_tmpDFB.f2=
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((
_tmpDFB.f3=0,_tmpDFB)))))))),_tmpDFA)))),((_tmpDFC->tl=(Cyc_yyget_YY30(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))))->tms,_tmpDFC)))))),_tmpDF9)))));}break;}case 276: _LL341:{
struct Cyc_List_List*_tmpE06;struct Cyc_Absyn_Attributes_mod_struct _tmpE05;struct
Cyc_Absyn_Attributes_mod_struct*_tmpE04;struct Cyc_Parse_Abstractdeclarator*
_tmpE03;yyval=Cyc_YY30(((_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03->tms=((
_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06->hd=(void*)((_tmpE04=_cycalloc(
sizeof(*_tmpE04)),((_tmpE04[0]=((_tmpE05.tag=5,((_tmpE05.f1=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpE05.f2=Cyc_yyget_YY45(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpE05)))))),_tmpE04)))),((_tmpE06->tl=(Cyc_yyget_YY30(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))))->tms,_tmpE06)))))),_tmpE03)))));}break;case 277: _LL342: yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 278: _LL343: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 279: _LL344: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 280: _LL345: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 281: _LL346: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 282: _LL347: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 283: _LL348:{struct Cyc_Absyn_ResetRegion_s_struct _tmpE09;struct Cyc_Absyn_ResetRegion_s_struct*
_tmpE08;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)((_tmpE08=_cycalloc(sizeof(*
_tmpE08)),((_tmpE08[0]=((_tmpE09.tag=16,((_tmpE09.f1=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpE09)))),_tmpE08)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 284: _LL349: yyval=Cyc_YY4(0);break;case
285: _LL34A:{const char*_tmpE0A;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpE0A="open",_tag_dyneither(_tmpE0A,sizeof(char),5))))!= 
0){const char*_tmpE0B;Cyc_Parse_err(((_tmpE0B="expecting `open'",_tag_dyneither(
_tmpE0B,sizeof(char),17))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line));}}yyval=Cyc_YY4((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))));break;case 286: _LL34B:{struct Cyc_Absyn_Label_s_struct _tmpE11;
struct _dyneither_ptr*_tmpE10;struct Cyc_Absyn_Label_s_struct*_tmpE0F;yyval=Cyc_YY9(
Cyc_Absyn_new_stmt((void*)((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F[0]=((
_tmpE11.tag=13,((_tmpE11.f1=((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10[0]=
Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpE10)))),((_tmpE11.f2=Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpE11)))))),_tmpE0F)))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 287:
_LL34C: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 288: _LL34D:
yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 289: _LL34E:
yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 290: _LL34F: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1));
break;case 291: _LL350: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Absyn_skip_stmt(0)));break;case 292: _LL351: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(
Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))));
break;case 293: _LL352: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 294: _LL353: yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(
Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 295: _LL354:{
struct Cyc_Absyn_Fn_d_struct _tmpE14;struct Cyc_Absyn_Fn_d_struct*_tmpE13;yyval=Cyc_YY9(
Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE13=_cycalloc(sizeof(*
_tmpE13)),((_tmpE13[0]=((_tmpE14.tag=1,((_tmpE14.f1=Cyc_yyget_YY17(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE14)))),
_tmpE13)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),Cyc_Absyn_skip_stmt(0)));}break;case 296: _LL355:{
struct Cyc_Absyn_Fn_d_struct _tmpE17;struct Cyc_Absyn_Fn_d_struct*_tmpE16;yyval=Cyc_YY9(
Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE16=_cycalloc(sizeof(*
_tmpE16)),((_tmpE16[0]=((_tmpE17.tag=1,((_tmpE17.f1=Cyc_yyget_YY17(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpE17)))),_tmpE16)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))));}
break;case 297: _LL356: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 298: _LL357:
yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 299: _LL358:
yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),Cyc_yyget_YY10(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 300: _LL359: {struct Cyc_Absyn_UnknownId_e_struct
_tmpE1A;struct Cyc_Absyn_UnknownId_e_struct*_tmpE19;struct Cyc_Absyn_Exp*_tmp763=
Cyc_Absyn_new_exp((void*)((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19[0]=((
_tmpE1A.tag=2,((_tmpE1A.f1=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpE1A)))),_tmpE19)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp763,Cyc_yyget_YY10(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;}case 301: _LL35A: {struct Cyc_Absyn_Exp*_tmp766=
Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(
_tmp766,Cyc_yyget_YY10(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;}case 302:
_LL35B: yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),Cyc_yyget_YY10(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 303: _LL35C: yyval=Cyc_YY10(0);break;case
304: _LL35D:{struct Cyc_Absyn_Switch_clause*_tmpE1D;struct Cyc_List_List*_tmpE1C;
yyval=Cyc_YY10(((_tmpE1C=_cycalloc(sizeof(*_tmpE1C)),((_tmpE1C->hd=((_tmpE1D=
_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line)),((
_tmpE1D->pat_vars=0,((_tmpE1D->where_clause=0,((_tmpE1D->body=Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpE1D->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpE1D)))))))))))),((_tmpE1C->tl=0,_tmpE1C)))))));}
break;case 305: _LL35E:{struct Cyc_Absyn_Switch_clause*_tmpE20;struct Cyc_List_List*
_tmpE1F;yyval=Cyc_YY10(((_tmpE1F=_cycalloc(sizeof(*_tmpE1F)),((_tmpE1F->hd=((
_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20->pattern=Cyc_yyget_YY11(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpE20->pat_vars=0,((_tmpE20->where_clause=0,((_tmpE20->body=
Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)),((_tmpE20->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpE20)))))))))))),((
_tmpE1F->tl=Cyc_yyget_YY10(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE1F)))))));}break;case 306: _LL35F:{
struct Cyc_Absyn_Switch_clause*_tmpE23;struct Cyc_List_List*_tmpE22;yyval=Cyc_YY10(((
_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22->hd=((_tmpE23=_cycalloc(sizeof(*
_tmpE23)),((_tmpE23->pattern=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((
_tmpE23->pat_vars=0,((_tmpE23->where_clause=0,((_tmpE23->body=Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpE23->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),_tmpE23)))))))))))),((_tmpE22->tl=Cyc_yyget_YY10(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpE22)))))));}break;case 307: _LL360:{struct Cyc_Absyn_Switch_clause*
_tmpE26;struct Cyc_List_List*_tmpE25;yyval=Cyc_YY10(((_tmpE25=_cycalloc(sizeof(*
_tmpE25)),((_tmpE25->hd=((_tmpE26=_cycalloc(sizeof(*_tmpE26)),((_tmpE26->pattern=
Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 4))),((_tmpE26->pat_vars=0,((_tmpE26->where_clause=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((
_tmpE26->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line)),((_tmpE26->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpE26)))))))))))),((
_tmpE25->tl=Cyc_yyget_YY10(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE25)))))));}break;case 308: _LL361:{
struct Cyc_Absyn_Switch_clause*_tmpE29;struct Cyc_List_List*_tmpE28;yyval=Cyc_YY10(((
_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28->hd=((_tmpE29=_cycalloc(sizeof(*
_tmpE29)),((_tmpE29->pattern=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))),((
_tmpE29->pat_vars=0,((_tmpE29->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpE29->body=Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpE29->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpE29)))))))))))),((_tmpE28->tl=Cyc_yyget_YY10(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpE28)))))));}break;case 309: _LL362: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 310: _LL363:
yyval=Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 311: _LL364: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 312: _LL365: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 313: _LL366: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 314: _LL367: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line)));break;case 315:
_LL368: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 316: _LL369:
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 317: _LL36A:
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 318: _LL36B:
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 8))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 319: _LL36C: {
struct Cyc_List_List*_tmp771=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3)));
struct Cyc_Absyn_Stmt*_tmp772=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(
_tmp771,_tmp772));break;}case 320: _LL36D: {struct Cyc_List_List*_tmp773=Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4)));struct Cyc_Absyn_Stmt*_tmp774=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 3))),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp773,
_tmp774));break;}case 321: _LL36E: {struct Cyc_List_List*_tmp775=Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4)));struct Cyc_Absyn_Stmt*_tmp776=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp775,
_tmp776));break;}case 322: _LL36F: {struct Cyc_List_List*_tmp777=Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 5)));struct Cyc_Absyn_Stmt*_tmp778=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 4))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp777,
_tmp778));break;}case 323: _LL370:{struct _dyneither_ptr*_tmpE2A;yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((
_tmpE2A=_cycalloc(sizeof(*_tmpE2A)),((_tmpE2A[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpE2A)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)));}break;case 324: _LL371: yyval=Cyc_YY9(Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line)));
break;case 325: _LL372: yyval=Cyc_YY9(Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line)));break;case 326:
_LL373: yyval=Cyc_YY9(Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line)));break;case 327:
_LL374: yyval=Cyc_YY9(Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)));break;case 328: _LL375: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line)));
break;case 329: _LL376: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line)));break;case 330:
_LL377: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).last_line)));break;case 331:
_LL378: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset));break;case 332: _LL379: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 333: _LL37A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 4)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line))));break;case 334: _LL37B: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 335: _LL37C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 336:
_LL37D: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset));break;case 337: _LL37E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line))));break;case 338: _LL37F: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 339: _LL380: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 340:
_LL381: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset));break;case 341: _LL382: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 342:
_LL383: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset));break;case 343: _LL384: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 344:
_LL385: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset));break;case 345: _LL386: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line))));break;case 346: _LL387: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line))));break;case 347: _LL388: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 348: _LL389: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 349:
_LL38A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 350:
_LL38B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 351:
_LL38C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 352:
_LL38D: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset));break;case 353: _LL38E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 354:
_LL38F: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 355:
_LL390: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset));break;case 356: _LL391: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line))));break;case 357: _LL392: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line))));break;case 358: _LL393: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 359: _LL394: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 360:
_LL395: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 361:
_LL396: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2)))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 362:
_LL397: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset));break;case 363: _LL398: {void*_tmp77A=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line));yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(
_tmp77A,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),1,Cyc_Absyn_Unknown_coercion,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;}case 364:
_LL399: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset));break;case 365: _LL39A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)))));break;case 366: _LL39B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line))));break;case 367: _LL39C: {void*_tmp77B=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(
_tmp77B,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line))));break;}case 368: _LL39D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 369:
_LL39E:{struct Cyc_Absyn_StructField_struct _tmpE30;struct _dyneither_ptr*_tmpE2F;
struct Cyc_Absyn_StructField_struct*_tmpE2E;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 3)))).f3,(void*)((_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((
_tmpE2E[0]=((_tmpE30.tag=0,((_tmpE30.f1=((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((
_tmpE2F[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),_tmpE2F)))),_tmpE30)))),_tmpE2E)))),
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));}
break;case 370: _LL39F: {void*_tmp77F=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line));{struct Cyc_Absyn_TupleIndex_struct _tmpE33;struct
Cyc_Absyn_TupleIndex_struct*_tmpE32;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp(
_tmp77F,(void*)((_tmpE32=_cycalloc_atomic(sizeof(*_tmpE32)),((_tmpE32[0]=((
_tmpE33.tag=1,((_tmpE33.f1=(unsigned int)(Cyc_yyget_Int_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)))).f2,
_tmpE33)))),_tmpE32)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line))));}break;}case 371: _LL3A0: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 372: _LL3A1: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 373: _LL3A2: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 374: _LL3A3: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1)))));break;case 375: _LL3A4: {struct Cyc_Absyn_Exp*e=
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)));{void*_tmp782=e->r;union Cyc_Absyn_Cnst _tmp784;
struct _tuple4 _tmp785;enum Cyc_Absyn_Sign _tmp786;char _tmp787;union Cyc_Absyn_Cnst
_tmp789;struct _tuple5 _tmp78A;enum Cyc_Absyn_Sign _tmp78B;short _tmp78C;union Cyc_Absyn_Cnst
_tmp78E;struct _tuple6 _tmp78F;enum Cyc_Absyn_Sign _tmp790;int _tmp791;union Cyc_Absyn_Cnst
_tmp793;struct _dyneither_ptr _tmp794;union Cyc_Absyn_Cnst _tmp796;int _tmp797;union
Cyc_Absyn_Cnst _tmp799;struct _dyneither_ptr _tmp79A;union Cyc_Absyn_Cnst _tmp79C;
struct _dyneither_ptr _tmp79D;union Cyc_Absyn_Cnst _tmp79F;struct _tuple7 _tmp7A0;
_LL3A7: {struct Cyc_Absyn_Const_e_struct*_tmp783=(struct Cyc_Absyn_Const_e_struct*)
_tmp782;if(_tmp783->tag != 0)goto _LL3A9;else{_tmp784=_tmp783->f1;if((_tmp784.Char_c).tag
!= 2)goto _LL3A9;_tmp785=(struct _tuple4)(_tmp784.Char_c).val;_tmp786=_tmp785.f1;
_tmp787=_tmp785.f2;}}_LL3A8:{struct Cyc_Absyn_Char_p_struct _tmpE36;struct Cyc_Absyn_Char_p_struct*
_tmpE35;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE35=_cycalloc_atomic(
sizeof(*_tmpE35)),((_tmpE35[0]=((_tmpE36.tag=10,((_tmpE36.f1=_tmp787,_tmpE36)))),
_tmpE35)))),e->loc));}goto _LL3A6;_LL3A9: {struct Cyc_Absyn_Const_e_struct*_tmp788=(
struct Cyc_Absyn_Const_e_struct*)_tmp782;if(_tmp788->tag != 0)goto _LL3AB;else{
_tmp789=_tmp788->f1;if((_tmp789.Short_c).tag != 4)goto _LL3AB;_tmp78A=(struct
_tuple5)(_tmp789.Short_c).val;_tmp78B=_tmp78A.f1;_tmp78C=_tmp78A.f2;}}_LL3AA:{
struct Cyc_Absyn_Int_p_struct _tmpE39;struct Cyc_Absyn_Int_p_struct*_tmpE38;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE38=_cycalloc_atomic(sizeof(*_tmpE38)),((
_tmpE38[0]=((_tmpE39.tag=9,((_tmpE39.f1=_tmp78B,((_tmpE39.f2=(int)_tmp78C,
_tmpE39)))))),_tmpE38)))),e->loc));}goto _LL3A6;_LL3AB: {struct Cyc_Absyn_Const_e_struct*
_tmp78D=(struct Cyc_Absyn_Const_e_struct*)_tmp782;if(_tmp78D->tag != 0)goto _LL3AD;
else{_tmp78E=_tmp78D->f1;if((_tmp78E.Int_c).tag != 5)goto _LL3AD;_tmp78F=(struct
_tuple6)(_tmp78E.Int_c).val;_tmp790=_tmp78F.f1;_tmp791=_tmp78F.f2;}}_LL3AC:{
struct Cyc_Absyn_Int_p_struct _tmpE3C;struct Cyc_Absyn_Int_p_struct*_tmpE3B;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE3B=_cycalloc_atomic(sizeof(*_tmpE3B)),((
_tmpE3B[0]=((_tmpE3C.tag=9,((_tmpE3C.f1=_tmp790,((_tmpE3C.f2=_tmp791,_tmpE3C)))))),
_tmpE3B)))),e->loc));}goto _LL3A6;_LL3AD: {struct Cyc_Absyn_Const_e_struct*_tmp792=(
struct Cyc_Absyn_Const_e_struct*)_tmp782;if(_tmp792->tag != 0)goto _LL3AF;else{
_tmp793=_tmp792->f1;if((_tmp793.Float_c).tag != 7)goto _LL3AF;_tmp794=(struct
_dyneither_ptr)(_tmp793.Float_c).val;}}_LL3AE:{struct Cyc_Absyn_Float_p_struct
_tmpE3F;struct Cyc_Absyn_Float_p_struct*_tmpE3E;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E[0]=((_tmpE3F.tag=11,((
_tmpE3F.f1=_tmp794,_tmpE3F)))),_tmpE3E)))),e->loc));}goto _LL3A6;_LL3AF: {struct
Cyc_Absyn_Const_e_struct*_tmp795=(struct Cyc_Absyn_Const_e_struct*)_tmp782;if(
_tmp795->tag != 0)goto _LL3B1;else{_tmp796=_tmp795->f1;if((_tmp796.Null_c).tag != 1)
goto _LL3B1;_tmp797=(int)(_tmp796.Null_c).val;}}_LL3B0: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A6;_LL3B1: {struct Cyc_Absyn_Const_e_struct*
_tmp798=(struct Cyc_Absyn_Const_e_struct*)_tmp782;if(_tmp798->tag != 0)goto _LL3B3;
else{_tmp799=_tmp798->f1;if((_tmp799.String_c).tag != 8)goto _LL3B3;_tmp79A=(
struct _dyneither_ptr)(_tmp799.String_c).val;}}_LL3B2:{const char*_tmpE40;Cyc_Parse_err(((
_tmpE40="strings cannot occur within patterns",_tag_dyneither(_tmpE40,sizeof(
char),37))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));}goto _LL3A6;_LL3B3: {struct Cyc_Absyn_Const_e_struct*
_tmp79B=(struct Cyc_Absyn_Const_e_struct*)_tmp782;if(_tmp79B->tag != 0)goto _LL3B5;
else{_tmp79C=_tmp79B->f1;if((_tmp79C.Wstring_c).tag != 9)goto _LL3B5;_tmp79D=(
struct _dyneither_ptr)(_tmp79C.Wstring_c).val;}}_LL3B4:{const char*_tmpE41;Cyc_Parse_err(((
_tmpE41="strings cannot occur within patterns",_tag_dyneither(_tmpE41,sizeof(
char),37))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));}goto _LL3A6;_LL3B5: {struct Cyc_Absyn_Const_e_struct*
_tmp79E=(struct Cyc_Absyn_Const_e_struct*)_tmp782;if(_tmp79E->tag != 0)goto _LL3B7;
else{_tmp79F=_tmp79E->f1;if((_tmp79F.LongLong_c).tag != 6)goto _LL3B7;_tmp7A0=(
struct _tuple7)(_tmp79F.LongLong_c).val;}}_LL3B6:{const char*_tmpE42;Cyc_Parse_unimp(((
_tmpE42="long long's in patterns",_tag_dyneither(_tmpE42,sizeof(char),24))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));}goto _LL3A6;_LL3B7:;
_LL3B8: {const char*_tmpE43;Cyc_Parse_err(((_tmpE43="bad constant in case",
_tag_dyneither(_tmpE43,sizeof(char),21))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));}_LL3A6:;}break;}case
376: _LL3A5:{struct Cyc_Absyn_UnknownId_p_struct _tmpE46;struct Cyc_Absyn_UnknownId_p_struct*
_tmpE45;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE45=_cycalloc(sizeof(*
_tmpE45)),((_tmpE45[0]=((_tmpE46.tag=14,((_tmpE46.f1=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpE46)))),_tmpE45)))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 377:
_LL3B9:{const char*_tmpE47;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpE47="as",_tag_dyneither(_tmpE47,sizeof(char),3))))!= 0){
const char*_tmpE48;Cyc_Parse_err(((_tmpE48="expecting `as'",_tag_dyneither(
_tmpE48,sizeof(char),15))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));}}{struct Cyc_Absyn_Var_p_struct _tmpE52;struct
_dyneither_ptr*_tmpE51;struct _tuple1*_tmpE50;struct Cyc_Absyn_Var_p_struct*
_tmpE4F;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE4F=_cycalloc(sizeof(*
_tmpE4F)),((_tmpE4F[0]=((_tmpE52.tag=1,((_tmpE52.f1=Cyc_Absyn_new_vardecl(((
_tmpE50=_cycalloc(sizeof(*_tmpE50)),((_tmpE50->f1=Cyc_Absyn_Loc_n,((_tmpE50->f2=((
_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),_tmpE51)))),_tmpE50)))))),(void*)& Cyc_Absyn_VoidType_val,0),((
_tmpE52.f2=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE52)))))),_tmpE4F)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line)));}break;case 378:
_LL3BA: {struct Cyc_List_List*_tmp7B6;int _tmp7B7;struct _tuple19 _tmp7B5=*Cyc_yyget_YY12(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));_tmp7B6=_tmp7B5.f1;_tmp7B7=_tmp7B5.f2;{struct Cyc_Absyn_Tuple_p_struct
_tmpE55;struct Cyc_Absyn_Tuple_p_struct*_tmpE54;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE54=_cycalloc(sizeof(*_tmpE54)),((_tmpE54[0]=((_tmpE55.tag=4,((
_tmpE55.f1=_tmp7B6,((_tmpE55.f2=_tmp7B7,_tmpE55)))))),_tmpE54)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 379:
_LL3BB: {struct Cyc_List_List*_tmp7BB;int _tmp7BC;struct _tuple19 _tmp7BA=*Cyc_yyget_YY12(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));_tmp7BB=_tmp7BA.f1;_tmp7BC=_tmp7BA.f2;{struct Cyc_Absyn_UnknownCall_p_struct
_tmpE58;struct Cyc_Absyn_UnknownCall_p_struct*_tmpE57;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57[0]=((_tmpE58.tag=15,((
_tmpE58.f1=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((_tmpE58.f2=_tmp7BB,((_tmpE58.f3=
_tmp7BC,_tmpE58)))))))),_tmpE57)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;}case 380: _LL3BC: {struct Cyc_List_List*
_tmp7C0;int _tmp7C1;struct _tuple19 _tmp7BF=*Cyc_yyget_YY16(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));
_tmp7C0=_tmp7BF.f1;_tmp7C1=_tmp7BF.f2;{struct Cyc_List_List*_tmp7C2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line),Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))));{struct Cyc_Absyn_Aggr_p_struct _tmpE5E;struct Cyc_Absyn_AggrInfo*
_tmpE5D;struct Cyc_Absyn_Aggr_p_struct*_tmpE5C;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE5C=_cycalloc(sizeof(*_tmpE5C)),((_tmpE5C[0]=((_tmpE5E.tag=6,((
_tmpE5E.f1=((_tmpE5D=_cycalloc(sizeof(*_tmpE5D)),((_tmpE5D->aggr_info=Cyc_Absyn_UnknownAggr(
Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),0),((
_tmpE5D->targs=0,_tmpE5D)))))),((_tmpE5E.f2=_tmp7C2,((_tmpE5E.f3=_tmp7C0,((
_tmpE5E.f4=_tmp7C1,_tmpE5E)))))))))),_tmpE5C)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;};}case 381:
_LL3BD: {struct Cyc_List_List*_tmp7C7;int _tmp7C8;struct _tuple19 _tmp7C6=*Cyc_yyget_YY16(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));_tmp7C7=_tmp7C6.f1;_tmp7C8=_tmp7C6.f2;{struct Cyc_List_List*
_tmp7C9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line),
Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))));{struct Cyc_Absyn_Aggr_p_struct _tmpE61;struct Cyc_Absyn_Aggr_p_struct*
_tmpE60;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE60=_cycalloc(sizeof(*
_tmpE60)),((_tmpE60[0]=((_tmpE61.tag=6,((_tmpE61.f1=0,((_tmpE61.f2=_tmp7C9,((
_tmpE61.f3=_tmp7C7,((_tmpE61.f4=_tmp7C8,_tmpE61)))))))))),_tmpE60)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;};}case 382:
_LL3BE:{struct Cyc_Absyn_Pointer_p_struct _tmpE64;struct Cyc_Absyn_Pointer_p_struct*
_tmpE63;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE63=_cycalloc(sizeof(*
_tmpE63)),((_tmpE63[0]=((_tmpE64.tag=5,((_tmpE64.f1=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE64)))),
_tmpE63)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 383: _LL3BF:{struct Cyc_Absyn_Pointer_p_struct
_tmpE6E;struct Cyc_Absyn_Pointer_p_struct _tmpE6D;struct Cyc_Absyn_Pointer_p_struct*
_tmpE6C;struct Cyc_Absyn_Pointer_p_struct*_tmpE6B;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B[0]=((_tmpE6E.tag=5,((
_tmpE6E.f1=Cyc_Absyn_new_pat((void*)((_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((
_tmpE6C[0]=((_tmpE6D.tag=5,((_tmpE6D.f1=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE6D)))),
_tmpE6C)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),_tmpE6E)))),_tmpE6B)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 384:
_LL3C0:{struct Cyc_Absyn_Reference_p_struct _tmpE78;struct _dyneither_ptr*_tmpE77;
struct _tuple1*_tmpE76;struct Cyc_Absyn_Reference_p_struct*_tmpE75;yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)((_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75[0]=((
_tmpE78.tag=2,((_tmpE78.f1=Cyc_Absyn_new_vardecl(((_tmpE76=_cycalloc(sizeof(*
_tmpE76)),((_tmpE76->f1=Cyc_Absyn_Loc_n,((_tmpE76->f2=((_tmpE77=_cycalloc(
sizeof(*_tmpE77)),((_tmpE77[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE77)))),
_tmpE76)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE78.f2=Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),_tmpE78)))))),_tmpE75)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 385:
_LL3C1:{const char*_tmpE79;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpE79="as",_tag_dyneither(_tmpE79,sizeof(char),3))))!= 0){
const char*_tmpE7A;Cyc_Parse_err(((_tmpE7A="expecting `as'",_tag_dyneither(
_tmpE7A,sizeof(char),15))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));}}{struct Cyc_Absyn_Reference_p_struct _tmpE84;
struct _dyneither_ptr*_tmpE83;struct _tuple1*_tmpE82;struct Cyc_Absyn_Reference_p_struct*
_tmpE81;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE81=_cycalloc(sizeof(*
_tmpE81)),((_tmpE81[0]=((_tmpE84.tag=2,((_tmpE84.f1=Cyc_Absyn_new_vardecl(((
_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82->f1=Cyc_Absyn_Loc_n,((_tmpE82->f2=((
_tmpE83=_cycalloc(sizeof(*_tmpE83)),((_tmpE83[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),_tmpE83)))),_tmpE82)))))),(void*)& Cyc_Absyn_VoidType_val,0),((
_tmpE84.f2=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE84)))))),_tmpE81)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line)));}break;case 386:
_LL3C2: {void*_tmp7DC=Cyc_Parse_id2type(Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Tcutil_kind_to_bound(&
Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct _tmpE95;struct Cyc_Absyn_TagType_struct
_tmpE94;struct Cyc_Absyn_TagType_struct*_tmpE93;struct _dyneither_ptr*_tmpE92;
struct _tuple1*_tmpE91;struct Cyc_Absyn_TagInt_p_struct*_tmpE90;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE90=_cycalloc(sizeof(*_tmpE90)),((_tmpE90[0]=((_tmpE95.tag=3,((
_tmpE95.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),_tmp7DC),((_tmpE95.f2=Cyc_Absyn_new_vardecl(((
_tmpE91=_cycalloc(sizeof(*_tmpE91)),((_tmpE91->f1=Cyc_Absyn_Loc_n,((_tmpE91->f2=((
_tmpE92=_cycalloc(sizeof(*_tmpE92)),((_tmpE92[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),_tmpE92)))),_tmpE91)))))),(void*)((_tmpE93=_cycalloc(sizeof(*
_tmpE93)),((_tmpE93[0]=((_tmpE94.tag=20,((_tmpE94.f1=(void*)_tmp7DC,_tmpE94)))),
_tmpE93)))),0),_tmpE95)))))),_tmpE90)))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 387:
_LL3C3: {struct Cyc_Absyn_Tvar*_tmp7E3=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(&
Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct _tmpEB1;struct Cyc_Absyn_TagType_struct
_tmpEB0;struct Cyc_Absyn_VarType_struct _tmpEAF;struct Cyc_Absyn_VarType_struct*
_tmpEAE;struct Cyc_Absyn_TagType_struct*_tmpEAD;struct _dyneither_ptr*_tmpEAC;
struct _tuple1*_tmpEAB;struct Cyc_Absyn_TagInt_p_struct*_tmpEAA;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA[0]=((_tmpEB1.tag=3,((
_tmpEB1.f1=_tmp7E3,((_tmpEB1.f2=Cyc_Absyn_new_vardecl(((_tmpEAB=_cycalloc(
sizeof(*_tmpEAB)),((_tmpEAB->f1=Cyc_Absyn_Loc_n,((_tmpEAB->f2=((_tmpEAC=
_cycalloc(sizeof(*_tmpEAC)),((_tmpEAC[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpEAC)))),_tmpEAB)))))),(void*)((_tmpEAD=_cycalloc(sizeof(*_tmpEAD)),((_tmpEAD[
0]=((_tmpEB0.tag=20,((_tmpEB0.f1=(void*)((void*)((_tmpEAE=_cycalloc(sizeof(*
_tmpEAE)),((_tmpEAE[0]=((_tmpEAF.tag=2,((_tmpEAF.f1=_tmp7E3,_tmpEAF)))),_tmpEAE))))),
_tmpEB0)))),_tmpEAD)))),0),_tmpEB1)))))),_tmpEAA)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 388:
_LL3C4:{struct _tuple19*_tmpEB2;yyval=Cyc_YY12(((_tmpEB2=_cycalloc(sizeof(*
_tmpEB2)),((_tmpEB2->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)))),((_tmpEB2->f2=0,_tmpEB2)))))));}break;case 389:
_LL3C5:{struct _tuple19*_tmpEB3;yyval=Cyc_YY12(((_tmpEB3=_cycalloc(sizeof(*
_tmpEB3)),((_tmpEB3->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2)))),((_tmpEB3->f2=1,_tmpEB3)))))));}break;case 390:
_LL3C6:{struct _tuple19*_tmpEB4;yyval=Cyc_YY12(((_tmpEB4=_cycalloc(sizeof(*
_tmpEB4)),((_tmpEB4->f1=0,((_tmpEB4->f2=1,_tmpEB4)))))));}break;case 391: _LL3C7:{
struct Cyc_List_List*_tmpEB5;yyval=Cyc_YY13(((_tmpEB5=_cycalloc(sizeof(*_tmpEB5)),((
_tmpEB5->hd=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((_tmpEB5->tl=0,_tmpEB5)))))));}break;
case 392: _LL3C8:{struct Cyc_List_List*_tmpEB6;yyval=Cyc_YY13(((_tmpEB6=_cycalloc(
sizeof(*_tmpEB6)),((_tmpEB6->hd=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpEB6->tl=Cyc_yyget_YY13(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpEB6)))))));}break;case 393: _LL3C9:{
struct _tuple20*_tmpEB7;yyval=Cyc_YY14(((_tmpEB7=_cycalloc(sizeof(*_tmpEB7)),((
_tmpEB7->f1=0,((_tmpEB7->f2=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpEB7)))))));}
break;case 394: _LL3CA:{struct _tuple20*_tmpEB8;yyval=Cyc_YY14(((_tmpEB8=_cycalloc(
sizeof(*_tmpEB8)),((_tmpEB8->f1=Cyc_yyget_YY41(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpEB8->f2=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpEB8)))))));}break;case 395: _LL3CB:{
struct _tuple19*_tmpEB9;yyval=Cyc_YY16(((_tmpEB9=_cycalloc(sizeof(*_tmpEB9)),((
_tmpEB9->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))),((_tmpEB9->f2=0,_tmpEB9)))))));}break;case 396: _LL3CC:{struct
_tuple19*_tmpEBA;yyval=Cyc_YY16(((_tmpEBA=_cycalloc(sizeof(*_tmpEBA)),((_tmpEBA->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2)))),((_tmpEBA->f2=1,_tmpEBA)))))));}break;case 397: _LL3CD:{struct
_tuple19*_tmpEBB;yyval=Cyc_YY16(((_tmpEBB=_cycalloc(sizeof(*_tmpEBB)),((_tmpEBB->f1=
0,((_tmpEBB->f2=1,_tmpEBB)))))));}break;case 398: _LL3CE:{struct Cyc_List_List*
_tmpEBC;yyval=Cyc_YY15(((_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC->hd=Cyc_yyget_YY14(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpEBC->tl=0,_tmpEBC)))))));}break;case 399: _LL3CF:{struct Cyc_List_List*
_tmpEBD;yyval=Cyc_YY15(((_tmpEBD=_cycalloc(sizeof(*_tmpEBD)),((_tmpEBD->hd=Cyc_yyget_YY14(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpEBD->tl=Cyc_yyget_YY15(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpEBD)))))));}break;case 400: _LL3D0: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 401: _LL3D1: yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 402: _LL3D2: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 403: _LL3D3: yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY8(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 404: _LL3D4:
yyval=Cyc_YY3(Cyc_Absyn_swap_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 405: _LL3D5: yyval=Cyc_YY8(0);break;case 406:
_LL3D6:{struct Cyc_Core_Opt*_tmpEBE;yyval=Cyc_YY8(((_tmpEBE=_cycalloc_atomic(
sizeof(*_tmpEBE)),((_tmpEBE->v=(void*)Cyc_Absyn_Times,_tmpEBE)))));}break;case
407: _LL3D7:{struct Cyc_Core_Opt*_tmpEBF;yyval=Cyc_YY8(((_tmpEBF=_cycalloc_atomic(
sizeof(*_tmpEBF)),((_tmpEBF->v=(void*)Cyc_Absyn_Div,_tmpEBF)))));}break;case 408:
_LL3D8:{struct Cyc_Core_Opt*_tmpEC0;yyval=Cyc_YY8(((_tmpEC0=_cycalloc_atomic(
sizeof(*_tmpEC0)),((_tmpEC0->v=(void*)Cyc_Absyn_Mod,_tmpEC0)))));}break;case 409:
_LL3D9:{struct Cyc_Core_Opt*_tmpEC1;yyval=Cyc_YY8(((_tmpEC1=_cycalloc_atomic(
sizeof(*_tmpEC1)),((_tmpEC1->v=(void*)Cyc_Absyn_Plus,_tmpEC1)))));}break;case 410:
_LL3DA:{struct Cyc_Core_Opt*_tmpEC2;yyval=Cyc_YY8(((_tmpEC2=_cycalloc_atomic(
sizeof(*_tmpEC2)),((_tmpEC2->v=(void*)Cyc_Absyn_Minus,_tmpEC2)))));}break;case
411: _LL3DB:{struct Cyc_Core_Opt*_tmpEC3;yyval=Cyc_YY8(((_tmpEC3=_cycalloc_atomic(
sizeof(*_tmpEC3)),((_tmpEC3->v=(void*)Cyc_Absyn_Bitlshift,_tmpEC3)))));}break;
case 412: _LL3DC:{struct Cyc_Core_Opt*_tmpEC4;yyval=Cyc_YY8(((_tmpEC4=
_cycalloc_atomic(sizeof(*_tmpEC4)),((_tmpEC4->v=(void*)Cyc_Absyn_Bitlrshift,
_tmpEC4)))));}break;case 413: _LL3DD:{struct Cyc_Core_Opt*_tmpEC5;yyval=Cyc_YY8(((
_tmpEC5=_cycalloc_atomic(sizeof(*_tmpEC5)),((_tmpEC5->v=(void*)Cyc_Absyn_Bitand,
_tmpEC5)))));}break;case 414: _LL3DE:{struct Cyc_Core_Opt*_tmpEC6;yyval=Cyc_YY8(((
_tmpEC6=_cycalloc_atomic(sizeof(*_tmpEC6)),((_tmpEC6->v=(void*)Cyc_Absyn_Bitxor,
_tmpEC6)))));}break;case 415: _LL3DF:{struct Cyc_Core_Opt*_tmpEC7;yyval=Cyc_YY8(((
_tmpEC7=_cycalloc_atomic(sizeof(*_tmpEC7)),((_tmpEC7->v=(void*)Cyc_Absyn_Bitor,
_tmpEC7)))));}break;case 416: _LL3E0: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 417: _LL3E1: yyval=Cyc_YY3(Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 418: _LL3E2:
yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 419: _LL3E3:
yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 420: _LL3E4:
yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 421: _LL3E5:
yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 422: _LL3E6: yyval=Cyc_YY3(Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 423: _LL3E7: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 424: _LL3E8: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 425: _LL3E9: yyval=Cyc_YY3(Cyc_Absyn_or_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 426: _LL3EA:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 427: _LL3EB: yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 428: _LL3EC:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 429: _LL3ED: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 430: _LL3EE:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 431: _LL3EF: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 432: _LL3F0:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 433: _LL3F1: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 434: _LL3F2:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 435: _LL3F3: yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 436: _LL3F4:
yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 437: _LL3F5: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 438: _LL3F6: yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 439: _LL3F7: yyval=Cyc_YY3(Cyc_Absyn_gt_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 440: _LL3F8:
yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 441: _LL3F9: yyval=Cyc_YY3(Cyc_Absyn_gte_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 442: _LL3FA:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 443: _LL3FB: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 444: _LL3FC:
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 445: _LL3FD: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 446: _LL3FE: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 447: _LL3FF:
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 448: _LL400: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 449: _LL401: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 450: _LL402:
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 451: _LL403: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(
Cyc_Absyn_Mod,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 452: _LL404:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 453: _LL405: {void*_tmp802=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line));yyval=Cyc_YY3(Cyc_Absyn_cast_exp(_tmp802,Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),1,Cyc_Absyn_Unknown_coercion,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;}case 454:
_LL406: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset));break;case 455: _LL407: yyval=Cyc_YY3(Cyc_Absyn_pre_inc_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 456: _LL408: yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 457: _LL409: yyval=Cyc_YY3(Cyc_Absyn_address_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 458: _LL40A: yyval=Cyc_YY3(Cyc_Absyn_deref_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 459: _LL40B: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 460: _LL40C: yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 461: _LL40D: {void*_tmp803=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp(_tmp803,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;}case 462:
_LL40E: yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 463: _LL40F: {
void*_tmp804=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));{struct Cyc_Absyn_StructField_struct
_tmpECD;struct _dyneither_ptr*_tmpECC;struct Cyc_Absyn_StructField_struct*_tmpECB;
yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(_tmp804,(void*)((_tmpECB=_cycalloc(sizeof(*
_tmpECB)),((_tmpECB[0]=((_tmpECD.tag=0,((_tmpECD.f1=((_tmpECC=_cycalloc(sizeof(*
_tmpECC)),((_tmpECC[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpECC)))),_tmpECD)))),_tmpECB)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;}case 464: _LL410: {void*_tmp808=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 3))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line));{struct Cyc_Absyn_TupleIndex_struct _tmpED0;struct
Cyc_Absyn_TupleIndex_struct*_tmpECF;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(_tmp808,(
void*)((_tmpECF=_cycalloc_atomic(sizeof(*_tmpECF)),((_tmpECF[0]=((_tmpED0.tag=1,((
_tmpED0.f1=(unsigned int)(Cyc_yyget_Int_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)))).f2,
_tmpED0)))),_tmpECF)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;}case 465: _LL411:{struct Cyc_Absyn_Malloc_e_struct
_tmpED6;struct Cyc_Absyn_MallocInfo _tmpED5;struct Cyc_Absyn_Malloc_e_struct*
_tmpED4;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpED4=_cycalloc(sizeof(*
_tmpED4)),((_tmpED4[0]=((_tmpED6.tag=34,((_tmpED6.f1=((_tmpED5.is_calloc=0,((
_tmpED5.rgn=0,((_tmpED5.elt_type=0,((_tmpED5.num_elts=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpED5.fat_result=0,_tmpED5)))))))))),_tmpED6)))),_tmpED4)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 466:
_LL412:{struct Cyc_Absyn_Malloc_e_struct _tmpEDC;struct Cyc_Absyn_MallocInfo _tmpEDB;
struct Cyc_Absyn_Malloc_e_struct*_tmpEDA;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpEDA=_cycalloc(sizeof(*_tmpEDA)),((_tmpEDA[0]=((_tmpEDC.tag=34,((_tmpEDC.f1=((
_tmpEDB.is_calloc=0,((_tmpEDB.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpEDB.elt_type=0,((_tmpEDB.num_elts=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpEDB.fat_result=0,_tmpEDB)))))))))),_tmpEDC)))),_tmpEDA)))),
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}
break;case 467: _LL413: {void*_tmp811=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line));{struct Cyc_Absyn_Malloc_e_struct _tmpEE6;void**
_tmpEE5;struct Cyc_Absyn_MallocInfo _tmpEE4;struct Cyc_Absyn_Malloc_e_struct*
_tmpEE3;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEE3=_cycalloc(sizeof(*
_tmpEE3)),((_tmpEE3[0]=((_tmpEE6.tag=34,((_tmpEE6.f1=((_tmpEE4.is_calloc=1,((
_tmpEE4.rgn=0,((_tmpEE4.elt_type=((_tmpEE5=_cycalloc(sizeof(*_tmpEE5)),((_tmpEE5[
0]=_tmp811,_tmpEE5)))),((_tmpEE4.num_elts=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6))),((
_tmpEE4.fat_result=0,_tmpEE4)))))))))),_tmpEE6)))),_tmpEE3)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 8))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 468:
_LL414: {void*_tmp816=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line));{struct Cyc_Absyn_Malloc_e_struct
_tmpEF0;void**_tmpEEF;struct Cyc_Absyn_MallocInfo _tmpEEE;struct Cyc_Absyn_Malloc_e_struct*
_tmpEED;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEED=_cycalloc(sizeof(*
_tmpEED)),((_tmpEED[0]=((_tmpEF0.tag=34,((_tmpEF0.f1=((_tmpEEE.is_calloc=1,((
_tmpEEE.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8))),((
_tmpEEE.elt_type=((_tmpEEF=_cycalloc(sizeof(*_tmpEEF)),((_tmpEEF[0]=_tmp816,
_tmpEEF)))),((_tmpEEE.num_elts=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6))),((
_tmpEEE.fat_result=0,_tmpEEE)))))))))),_tmpEF0)))),_tmpEED)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 10))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 469:
_LL415:{struct Cyc_Absyn_Exp*_tmpEF1[1];yyval=Cyc_YY3(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((
_tmpEF1[0]=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEF1,sizeof(struct Cyc_Absyn_Exp*),
1)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 470: _LL416:{struct Cyc_Absyn_Tagcheck_e_struct
_tmpEF7;struct _dyneither_ptr*_tmpEF6;struct Cyc_Absyn_Tagcheck_e_struct*_tmpEF5;
yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEF5=_cycalloc(sizeof(*_tmpEF5)),((
_tmpEF5[0]=((_tmpEF7.tag=38,((_tmpEF7.f1=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((
_tmpEF7.f2=((_tmpEF6=_cycalloc(sizeof(*_tmpEF6)),((_tmpEF6[0]=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpEF6)))),_tmpEF7)))))),_tmpEF5)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 471:
_LL417:{struct Cyc_Absyn_Tagcheck_e_struct _tmpEFD;struct _dyneither_ptr*_tmpEFC;
struct Cyc_Absyn_Tagcheck_e_struct*_tmpEFB;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpEFB=_cycalloc(sizeof(*_tmpEFB)),((_tmpEFB[0]=((_tmpEFD.tag=38,((_tmpEFD.f1=
Cyc_Absyn_deref_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line)),((_tmpEFD.f2=((
_tmpEFC=_cycalloc(sizeof(*_tmpEFC)),((_tmpEFC[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpEFC)))),_tmpEFD)))))),_tmpEFB)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 472:
_LL418: {void*_tmp822=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(
_tmp822,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;}case 473: _LL419: {int _tmp824;struct
_dyneither_ptr _tmp825;struct _tuple17 _tmp823=Cyc_yyget_Asm_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)));_tmp824=
_tmp823.f1;_tmp825=_tmp823.f2;yyval=Cyc_YY3(Cyc_Absyn_asm_exp(_tmp824,_tmp825,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));
break;}case 474: _LL41A: yyval=Cyc_YY7(Cyc_Absyn_Bitnot);break;case 475: _LL41B: yyval=
Cyc_YY7(Cyc_Absyn_Not);break;case 476: _LL41C: yyval=Cyc_YY7(Cyc_Absyn_Minus);
break;case 477: _LL41D: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 478: _LL41E: yyval=Cyc_YY3(Cyc_Absyn_subscript_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 3))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 479: _LL41F:
yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),0,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));
break;case 480: _LL420: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),Cyc_yyget_YY5(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 481: _LL421:{
struct _dyneither_ptr*_tmpEFE;yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpEFE=_cycalloc(sizeof(*_tmpEFE)),((_tmpEFE[0]=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpEFE)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 482: _LL422:{struct _dyneither_ptr*_tmpEFF;
yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((
_tmpEFF=_cycalloc(sizeof(*_tmpEFF)),((_tmpEFF[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpEFF)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 483: _LL423: yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 484: _LL424: yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 485: _LL425:{struct Cyc_Absyn_CompoundLit_e_struct
_tmpF02;struct Cyc_Absyn_CompoundLit_e_struct*_tmpF01;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpF01=_cycalloc(sizeof(*_tmpF01)),((_tmpF01[0]=((_tmpF02.tag=26,((
_tmpF02.f1=Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),((_tmpF02.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)))),
_tmpF02)))))),_tmpF01)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 486: _LL426:{struct Cyc_Absyn_CompoundLit_e_struct
_tmpF05;struct Cyc_Absyn_CompoundLit_e_struct*_tmpF04;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpF04=_cycalloc(sizeof(*_tmpF04)),((_tmpF04[0]=((_tmpF05.tag=26,((
_tmpF05.f1=Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))),((_tmpF05.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),
_tmpF05)))))),_tmpF04)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 487: _LL427:{struct Cyc_Absyn_UnknownId_e_struct
_tmpF08;struct Cyc_Absyn_UnknownId_e_struct*_tmpF07;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpF07=_cycalloc(sizeof(*_tmpF07)),((_tmpF07[0]=((_tmpF08.tag=2,((
_tmpF08.f1=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpF08)))),_tmpF07)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 488:
_LL428: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset));break;case 489: _LL429: yyval=Cyc_YY3(Cyc_Absyn_string_exp(
Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 490: _LL42A:
yyval=Cyc_YY3(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 491: _LL42B:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1));break;case 492: _LL42C: yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 493: _LL42D: yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 4))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 494: _LL42E:
yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 495: _LL42F:{
struct Cyc_Absyn_Aggregate_e_struct _tmpF0B;struct Cyc_Absyn_Aggregate_e_struct*
_tmpF0A;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpF0A=_cycalloc(sizeof(*
_tmpF0A)),((_tmpF0A[0]=((_tmpF0B.tag=29,((_tmpF0B.f1=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))),((_tmpF0B.f2=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((
_tmpF0B.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1)))),((_tmpF0B.f4=0,_tmpF0B)))))))))),_tmpF0A)))),
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}
break;case 496: _LL430: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 497: _LL431:
yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)))));break;case 498: _LL432:{struct Cyc_List_List*_tmpF0C;
yyval=Cyc_YY5(((_tmpF0C=_cycalloc(sizeof(*_tmpF0C)),((_tmpF0C->hd=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpF0C->tl=0,_tmpF0C)))))));}break;case 499: _LL433:{struct Cyc_List_List*
_tmpF0D;yyval=Cyc_YY5(((_tmpF0D=_cycalloc(sizeof(*_tmpF0D)),((_tmpF0D->hd=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpF0D->tl=Cyc_yyget_YY5(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpF0D)))))));}break;case 500: _LL434: yyval=Cyc_YY3(Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))).f1,(Cyc_yyget_Int_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f2,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 501: _LL435:
yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 502: _LL436:
yyval=Cyc_YY3(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 503: _LL437:
yyval=Cyc_YY3(Cyc_Absyn_float_exp(Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 504: _LL438:
yyval=Cyc_YY3(Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 505: _LL439:{struct _dyneither_ptr*_tmpF10;
struct _tuple1*_tmpF0F;yyval=Cyc_QualId_tok(((_tmpF0F=_cycalloc(sizeof(*_tmpF0F)),((
_tmpF0F->f1=Cyc_Absyn_Rel_n(0),((_tmpF0F->f2=((_tmpF10=_cycalloc(sizeof(*_tmpF10)),((
_tmpF10[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpF10)))),_tmpF0F)))))));}break;
case 506: _LL43A: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 507: _LL43B:{struct
_dyneither_ptr*_tmpF13;struct _tuple1*_tmpF12;yyval=Cyc_QualId_tok(((_tmpF12=
_cycalloc(sizeof(*_tmpF12)),((_tmpF12->f1=Cyc_Absyn_Rel_n(0),((_tmpF12->f2=((
_tmpF13=_cycalloc(sizeof(*_tmpF13)),((_tmpF13[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpF13)))),_tmpF12)))))));}break;case 508: _LL43C: yyval=*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 509: _LL43D: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 510: _LL43E: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 511: _LL43F: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 512: _LL440: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 513: _LL441: break;case 514:
_LL442: yylex_buf->lex_curr_pos -=1;break;default: _LL443: break;}yyvsp_offset -=
yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;*((union Cyc_YYSTYPE*)
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
yystate <= 6426) && Cyc_yycheck[_check_known_subscript_notnull(6427,yystate)]== *((
short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))yystate=(int)
Cyc_yytable[_check_known_subscript_notnull(6427,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(143,yyn - 146)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1030,yystate)];if(yyn > - 32768  && yyn < 6426){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 289 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6427,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(289,x)])+ 15,count ++);}{unsigned int _tmpF18;
unsigned int _tmpF17;struct _dyneither_ptr _tmpF16;char*_tmpF15;unsigned int _tmpF14;
msg=((_tmpF14=(unsigned int)(sze + 15),((_tmpF15=(char*)_region_malloc(yyregion,
_check_times(sizeof(char),_tmpF14 + 1)),((_tmpF16=_tag_dyneither(_tmpF15,sizeof(
char),_tmpF14 + 1),((((_tmpF17=_tmpF14,((_tmpF19(& _tmpF18,& _tmpF17,& _tmpF15),
_tmpF15[_tmpF17]=(char)0)))),_tmpF16))))))));}{const char*_tmpF1A;Cyc_strcpy(msg,((
_tmpF1A="parse error",_tag_dyneither(_tmpF1A,sizeof(char),12))));}if(count < 5){
count=0;for(x=yyn < 0?- yyn: 0;x < 289 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6427,x + yyn)]== x){{const char*_tmpF1C;const char*
_tmpF1B;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)((
_tmpF1C=", expecting `",_tag_dyneither(_tmpF1C,sizeof(char),14))):(struct
_dyneither_ptr)((_tmpF1B=" or `",_tag_dyneither(_tmpF1B,sizeof(char),6)))));}Cyc_strcat(
msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(289,x)]);{
const char*_tmpF1D;Cyc_strcat(msg,((_tmpF1D="'",_tag_dyneither(_tmpF1D,sizeof(
char),2))));}++ count;}}}Cyc_yyerror((struct _dyneither_ptr)msg);}else{const char*
_tmpF1E;Cyc_yyerror(((_tmpF1E="parse error",_tag_dyneither(_tmpF1E,sizeof(char),
12))));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int
_tmp83F=1;_npop_handler(0);return _tmp83F;}Cyc_yychar=- 2;}yyerrstatus=3;goto
yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp840=1;
_npop_handler(0);return _tmp840;}-- yyvsp_offset;yystate=(int)*((short*)
_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));-- yylsp_offset;
yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1030,yystate)];
if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6426) || Cyc_yycheck[
_check_known_subscript_notnull(6427,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6427,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 1029){int
_tmp841=0;_npop_handler(0);return _tmp841;}*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=Cyc_yylval;*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++
yylsp_offset))=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE _tmp84A=v;struct _tuple6 _tmp84B;int
_tmp84C;char _tmp84D;struct _dyneither_ptr _tmp84E;struct _tuple1*_tmp84F;struct
_tuple1 _tmp850;union Cyc_Absyn_Nmspace _tmp851;struct _dyneither_ptr*_tmp852;_LL446:
if((_tmp84A.Int_tok).tag != 1)goto _LL448;_tmp84B=(struct _tuple6)(_tmp84A.Int_tok).val;
_tmp84C=_tmp84B.f2;_LL447:{const char*_tmpF22;void*_tmpF21[1];struct Cyc_Int_pa_struct
_tmpF20;(_tmpF20.tag=1,((_tmpF20.f1=(unsigned long)_tmp84C,((_tmpF21[0]=& _tmpF20,
Cyc_fprintf(Cyc_stderr,((_tmpF22="%d",_tag_dyneither(_tmpF22,sizeof(char),3))),
_tag_dyneither(_tmpF21,sizeof(void*),1)))))));}goto _LL445;_LL448: if((_tmp84A.Char_tok).tag
!= 2)goto _LL44A;_tmp84D=(char)(_tmp84A.Char_tok).val;_LL449:{const char*_tmpF26;
void*_tmpF25[1];struct Cyc_Int_pa_struct _tmpF24;(_tmpF24.tag=1,((_tmpF24.f1=(
unsigned long)((int)_tmp84D),((_tmpF25[0]=& _tmpF24,Cyc_fprintf(Cyc_stderr,((
_tmpF26="%c",_tag_dyneither(_tmpF26,sizeof(char),3))),_tag_dyneither(_tmpF25,
sizeof(void*),1)))))));}goto _LL445;_LL44A: if((_tmp84A.String_tok).tag != 3)goto
_LL44C;_tmp84E=(struct _dyneither_ptr)(_tmp84A.String_tok).val;_LL44B:{const char*
_tmpF2A;void*_tmpF29[1];struct Cyc_String_pa_struct _tmpF28;(_tmpF28.tag=0,((
_tmpF28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp84E),((_tmpF29[0]=&
_tmpF28,Cyc_fprintf(Cyc_stderr,((_tmpF2A="\"%s\"",_tag_dyneither(_tmpF2A,sizeof(
char),5))),_tag_dyneither(_tmpF29,sizeof(void*),1)))))));}goto _LL445;_LL44C: if((
_tmp84A.QualId_tok).tag != 5)goto _LL44E;_tmp84F=(struct _tuple1*)(_tmp84A.QualId_tok).val;
_tmp850=*_tmp84F;_tmp851=_tmp850.f1;_tmp852=_tmp850.f2;_LL44D: {struct Cyc_List_List*
_tmp85C=0;{union Cyc_Absyn_Nmspace _tmp85D=_tmp851;struct Cyc_List_List*_tmp85E;
struct Cyc_List_List*_tmp85F;struct Cyc_List_List*_tmp860;int _tmp861;_LL451: if((
_tmp85D.Rel_n).tag != 1)goto _LL453;_tmp85E=(struct Cyc_List_List*)(_tmp85D.Rel_n).val;
_LL452: _tmp85C=_tmp85E;goto _LL450;_LL453: if((_tmp85D.Abs_n).tag != 2)goto _LL455;
_tmp85F=(struct Cyc_List_List*)(_tmp85D.Abs_n).val;_LL454: _tmp85C=_tmp85F;goto
_LL450;_LL455: if((_tmp85D.C_n).tag != 3)goto _LL457;_tmp860=(struct Cyc_List_List*)(
_tmp85D.C_n).val;_LL456: _tmp85C=_tmp860;goto _LL450;_LL457: if((_tmp85D.Loc_n).tag
!= 4)goto _LL450;_tmp861=(int)(_tmp85D.Loc_n).val;_LL458: goto _LL450;_LL450:;}for(
0;_tmp85C != 0;_tmp85C=_tmp85C->tl){const char*_tmpF2E;void*_tmpF2D[1];struct Cyc_String_pa_struct
_tmpF2C;(_tmpF2C.tag=0,((_tmpF2C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp85C->hd)),((_tmpF2D[0]=& _tmpF2C,Cyc_fprintf(Cyc_stderr,((
_tmpF2E="%s::",_tag_dyneither(_tmpF2E,sizeof(char),5))),_tag_dyneither(_tmpF2D,
sizeof(void*),1)))))));}{const char*_tmpF32;void*_tmpF31[1];struct Cyc_String_pa_struct
_tmpF30;(_tmpF30.tag=0,((_tmpF30.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp852),((_tmpF31[0]=& _tmpF30,Cyc_fprintf(Cyc_stderr,((_tmpF32="%s::",
_tag_dyneither(_tmpF32,sizeof(char),5))),_tag_dyneither(_tmpF31,sizeof(void*),1)))))));}
goto _LL445;}_LL44E:;_LL44F:{const char*_tmpF35;void*_tmpF34;(_tmpF34=0,Cyc_fprintf(
Cyc_stderr,((_tmpF35="?",_tag_dyneither(_tmpF35,sizeof(char),2))),_tag_dyneither(
_tmpF34,sizeof(void*),0)));}goto _LL445;_LL445:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
