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
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < 
dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
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
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
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
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;}
;struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
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
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
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
exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple1*
n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(
enum Cyc_Absyn_Scope s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
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
Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{int tag;struct Cyc_Absyn_Decl*
f1;};enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0,Cyc_Parse_Extern_sc
 = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc  = 3,Cyc_Parse_Auto_sc  = 4,Cyc_Parse_Register_sc
 = 5,Cyc_Parse_Abstract_sc  = 6};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple10{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple10 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple11{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple11
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Parse_err((struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);{struct Cyc_Position_Exit_struct
_tmp865;struct Cyc_Position_Exit_struct*_tmp864;(int)_throw((void*)((_tmp864=
_cycalloc_atomic(sizeof(*_tmp864)),((_tmp864[0]=((_tmp865.tag=Cyc_Position_Exit,
_tmp865)),_tmp864)))));};}static void Cyc_Parse_unimp(struct _dyneither_ptr msg,
struct Cyc_Position_Segment*sg){{const char*_tmp86A;void*_tmp869[2];struct Cyc_String_pa_struct
_tmp868;struct Cyc_String_pa_struct _tmp867;(_tmp867.tag=0,((_tmp867.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp868.tag=0,((_tmp868.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(sg)),((
_tmp869[0]=& _tmp868,((_tmp869[1]=& _tmp867,Cyc_fprintf(Cyc_stderr,((_tmp86A="%s: Warning: Cyclone does not yet support %s\n",
_tag_dyneither(_tmp86A,sizeof(char),46))),_tag_dyneither(_tmp869,sizeof(void*),2)))))))))))));}
return;}struct _tuple12{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;
union Cyc_Absyn_Constraint*f3;void*f4;};static struct _tuple12 Cyc_Parse_collapse_pointer_quals(
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
else{_tmpE=_tmpD->f1;}}_LLE:{struct Cyc_Absyn_Upper_b_struct _tmp86D;struct Cyc_Absyn_Upper_b_struct*
_tmp86C;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmp86C=_cycalloc(sizeof(*_tmp86C)),((_tmp86C[0]=((_tmp86D.tag=1,((
_tmp86D.f1=_tmpE,_tmp86D)))),_tmp86C)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_struct*
_tmpF=(struct Cyc_Absyn_Region_ptrqual_struct*)_tmp6;if(_tmpF->tag != 1)goto _LL0;
else{_tmp10=(void*)_tmpF->f1;}}_LL10: rgn=_tmp10;goto _LL0;_LL0:;}{struct _tuple12
_tmp86E;return(_tmp86E.f1=nullable,((_tmp86E.f2=bound,((_tmp86E.f3=zeroterm,((
_tmp86E.f4=rgn,_tmp86E)))))));};}struct _tuple1*Cyc_Parse_gensym_enum(){static int
enum_counter=0;struct _dyneither_ptr*_tmp87B;const char*_tmp87A;void*_tmp879[1];
struct Cyc_Int_pa_struct _tmp878;struct _tuple1*_tmp877;return(_tmp877=_cycalloc(
sizeof(*_tmp877)),((_tmp877->f1=Cyc_Absyn_Rel_n(0),((_tmp877->f2=((_tmp87B=
_cycalloc(sizeof(*_tmp87B)),((_tmp87B[0]=(struct _dyneither_ptr)((_tmp878.tag=1,((
_tmp878.f1=(unsigned long)enum_counter ++,((_tmp879[0]=& _tmp878,Cyc_aprintf(((
_tmp87A="__anonymous_enum_%d__",_tag_dyneither(_tmp87A,sizeof(char),22))),
_tag_dyneither(_tmp879,sizeof(void*),1)))))))),_tmp87B)))),_tmp877)))));}struct
_tuple13{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;
struct Cyc_List_List*f5;};struct _tuple14{struct _tuple13*f1;struct Cyc_Absyn_Exp*f2;
};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*
loc,struct _tuple14*field_info){struct _tuple14 _tmp1A;struct _tuple13*_tmp1B;struct
_tuple13 _tmp1C;struct _tuple1*_tmp1D;struct Cyc_Absyn_Tqual _tmp1E;void*_tmp1F;
struct Cyc_List_List*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_Absyn_Exp*_tmp22;
struct _tuple14*_tmp19=field_info;_tmp1A=*_tmp19;_tmp1B=_tmp1A.f1;_tmp1C=*_tmp1B;
_tmp1D=_tmp1C.f1;_tmp1E=_tmp1C.f2;_tmp1F=_tmp1C.f3;_tmp20=_tmp1C.f4;_tmp21=
_tmp1C.f5;_tmp22=_tmp1A.f2;if(_tmp20 != 0){const char*_tmp87C;Cyc_Parse_err(((
_tmp87C="bad type params in struct field",_tag_dyneither(_tmp87C,sizeof(char),32))),
loc);}if(Cyc_Absyn_is_qvar_qualified(_tmp1D)){const char*_tmp87D;Cyc_Parse_err(((
_tmp87D="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp87D,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp87E;return(_tmp87E=_cycalloc(sizeof(*_tmp87E)),((_tmp87E->name=(*_tmp1D).f2,((
_tmp87E->tq=_tmp1E,((_tmp87E->type=_tmp1F,((_tmp87E->width=_tmp22,((_tmp87E->attributes=
_tmp21,_tmp87E)))))))))));};}static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*
loc){struct Cyc_Parse_Type_spec_struct _tmp881;struct Cyc_Parse_Type_spec_struct*
_tmp880;return(void*)((_tmp880=_cycalloc(sizeof(*_tmp880)),((_tmp880[0]=((
_tmp881.tag=4,((_tmp881.f1=(void*)t,((_tmp881.f2=loc,_tmp881)))))),_tmp880))));}
static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp28=t;struct Cyc_Absyn_ArrayInfo
_tmp2A;void*_tmp2B;struct Cyc_Absyn_Tqual _tmp2C;struct Cyc_Absyn_Exp*_tmp2D;union
Cyc_Absyn_Constraint*_tmp2E;struct Cyc_Position_Segment*_tmp2F;_LL12: {struct Cyc_Absyn_ArrayType_struct*
_tmp29=(struct Cyc_Absyn_ArrayType_struct*)_tmp28;if(_tmp29->tag != 9)goto _LL14;
else{_tmp2A=_tmp29->f1;_tmp2B=_tmp2A.elt_type;_tmp2C=_tmp2A.tq;_tmp2D=_tmp2A.num_elts;
_tmp2E=_tmp2A.zero_term;_tmp2F=_tmp2A.zt_loc;}}_LL13: {struct Cyc_Absyn_Upper_b_struct
_tmp884;struct Cyc_Absyn_Upper_b_struct*_tmp883;return Cyc_Absyn_starb_typ(_tmp2B,
argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,
_tmp2C,_tmp2D == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((void*)((_tmp883=
_cycalloc(sizeof(*_tmp883)),((_tmp883[0]=((_tmp884.tag=1,((_tmp884.f1=(struct Cyc_Absyn_Exp*)
_tmp2D,_tmp884)))),_tmp883))))),_tmp2E);}_LL14:;_LL15: return t;_LL11:;}struct
_tuple15{struct _dyneither_ptr*f1;void*f2;};static struct Cyc_List_List*Cyc_Parse_get_arg_tags(
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
_tmp46->f2;}}_LL1F: {struct Cyc_String_pa_struct _tmp88C;void*_tmp88B[1];const char*
_tmp88A;struct _dyneither_ptr*_tmp889;struct _dyneither_ptr*nm=(struct
_dyneither_ptr*)((_tmp889=_cycalloc(sizeof(*_tmp889)),((_tmp889[0]=((_tmp88C.tag=
0,((_tmp88C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp37),((_tmp88B[0]=&
_tmp88C,Cyc_aprintf(((_tmp88A="`%s",_tag_dyneither(_tmp88A,sizeof(char),4))),
_tag_dyneither(_tmp88B,sizeof(void*),1)))))))),_tmp889))));{struct Cyc_Absyn_VarType_struct*
_tmp8A1;struct Cyc_Absyn_Tvar*_tmp8A0;struct Cyc_Absyn_Eq_kb_struct _tmp89F;struct
Cyc_Absyn_Eq_kb_struct*_tmp89E;struct Cyc_Absyn_VarType_struct _tmp89D;struct Cyc_Core_Opt*
_tmp89C;*_tmp48=((_tmp89C=_cycalloc(sizeof(*_tmp89C)),((_tmp89C->v=(void*)((
_tmp8A1=_cycalloc(sizeof(*_tmp8A1)),((_tmp8A1[0]=((_tmp89D.tag=2,((_tmp89D.f1=((
_tmp8A0=_cycalloc(sizeof(*_tmp8A0)),((_tmp8A0->name=nm,((_tmp8A0->identity=- 1,((
_tmp8A0->kind=(void*)((_tmp89E=_cycalloc(sizeof(*_tmp89E)),((_tmp89E[0]=((
_tmp89F.tag=0,((_tmp89F.f1=& Cyc_Tcutil_ik,_tmp89F)))),_tmp89E)))),_tmp8A0)))))))),
_tmp89D)))),_tmp8A1)))),_tmp89C))));}goto _LL1D;}_LL20:;_LL21: goto _LL1D;_LL1D:;}{
struct _tuple15*_tmp8A4;struct Cyc_List_List*_tmp8A3;_tmp32=((_tmp8A3=_cycalloc(
sizeof(*_tmp8A3)),((_tmp8A3->hd=((_tmp8A4=_cycalloc(sizeof(*_tmp8A4)),((_tmp8A4->f1=
_tmp37,((_tmp8A4->f2=_tmp3A,_tmp8A4)))))),((_tmp8A3->tl=_tmp32,_tmp8A3))))));}
goto _LL16;_LL19: _tmp3B=*_tmp33;_tmp3C=_tmp3B.f1;if(_tmp3C == 0)goto _LL1B;_tmp3D=*
_tmp3C;_tmp3E=(struct _dyneither_ptr*)_tmp3D.v;_tmp3F=_tmp3B.f3;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp40=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp3F;if(_tmp40->tag != 16)goto
_LL1B;else{_tmp41=(void*)_tmp40->f1;{struct Cyc_Absyn_Evar_struct*_tmp42=(struct
Cyc_Absyn_Evar_struct*)_tmp41;if(_tmp42->tag != 1)goto _LL1B;else{_tmp43=_tmp42->f2;
_tmp44=(struct Cyc_Core_Opt**)& _tmp42->f2;}};}};_LL1A: {struct Cyc_String_pa_struct
_tmp8AC;void*_tmp8AB[1];const char*_tmp8AA;struct _dyneither_ptr*_tmp8A9;struct
_dyneither_ptr*nm=(struct _dyneither_ptr*)((_tmp8A9=_cycalloc(sizeof(*_tmp8A9)),((
_tmp8A9[0]=((_tmp8AC.tag=0,((_tmp8AC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp3E),((_tmp8AB[0]=& _tmp8AC,Cyc_aprintf(((_tmp8AA="`%s",
_tag_dyneither(_tmp8AA,sizeof(char),4))),_tag_dyneither(_tmp8AB,sizeof(void*),1)))))))),
_tmp8A9))));{struct Cyc_Absyn_VarType_struct*_tmp8C1;struct Cyc_Absyn_Tvar*_tmp8C0;
struct Cyc_Absyn_Eq_kb_struct _tmp8BF;struct Cyc_Absyn_Eq_kb_struct*_tmp8BE;struct
Cyc_Absyn_VarType_struct _tmp8BD;struct Cyc_Core_Opt*_tmp8BC;*_tmp44=((_tmp8BC=
_cycalloc(sizeof(*_tmp8BC)),((_tmp8BC->v=(void*)((_tmp8C1=_cycalloc(sizeof(*
_tmp8C1)),((_tmp8C1[0]=((_tmp8BD.tag=2,((_tmp8BD.f1=((_tmp8C0=_cycalloc(sizeof(*
_tmp8C0)),((_tmp8C0->name=nm,((_tmp8C0->identity=- 1,((_tmp8C0->kind=(void*)((
_tmp8BE=_cycalloc(sizeof(*_tmp8BE)),((_tmp8BE[0]=((_tmp8BF.tag=0,((_tmp8BF.f1=&
Cyc_Tcutil_rk,_tmp8BF)))),_tmp8BE)))),_tmp8C0)))))))),_tmp8BD)))),_tmp8C1)))),
_tmp8BC))));}goto _LL16;}_LL1B:;_LL1C: goto _LL16;_LL16:;}return _tmp32;}static
struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){struct
Cyc_List_List*_tmp5F=0;for(0;x != 0;x=x->tl){void*_tmp60=((struct Cyc_Absyn_Aggrfield*)
x->hd)->type;void*_tmp62;_LL23: {struct Cyc_Absyn_TagType_struct*_tmp61=(struct
Cyc_Absyn_TagType_struct*)_tmp60;if(_tmp61->tag != 20)goto _LL25;else{_tmp62=(void*)
_tmp61->f1;}}_LL24:{struct _tuple15*_tmp8C4;struct Cyc_List_List*_tmp8C3;_tmp5F=((
_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3->hd=((_tmp8C4=_cycalloc(sizeof(*
_tmp8C4)),((_tmp8C4->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8C4->f2=
_tmp62,_tmp8C4)))))),((_tmp8C3->tl=_tmp5F,_tmp8C3))))));}goto _LL22;_LL25:;_LL26:
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
_tmp72=tags;for(0;_tmp72 != 0;_tmp72=_tmp72->tl){struct _tuple15 _tmp74;struct
_dyneither_ptr*_tmp75;void*_tmp76;struct _tuple15*_tmp73=(struct _tuple15*)_tmp72->hd;
_tmp74=*_tmp73;_tmp75=_tmp74.f1;_tmp76=_tmp74.f2;if(Cyc_strptrcmp(_tmp75,_tmp71)
== 0){struct Cyc_Absyn_Valueof_e_struct _tmp8C7;struct Cyc_Absyn_Valueof_e_struct*
_tmp8C6;return Cyc_Absyn_new_exp((void*)((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),((
_tmp8C6[0]=((_tmp8C7.tag=39,((_tmp8C7.f1=(void*)Cyc_Tcutil_copy_type(_tmp76),
_tmp8C7)))),_tmp8C6)))),e->loc);}}}goto _LL27;_LL2C:;_LL2D: goto _LL27;_LL27:;}
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
struct Cyc_Absyn_ArrayType_struct _tmp8CD;struct Cyc_Absyn_ArrayInfo _tmp8CC;struct
Cyc_Absyn_ArrayType_struct*_tmp8CB;return(void*)((_tmp8CB=_cycalloc(sizeof(*
_tmp8CB)),((_tmp8CB[0]=((_tmp8CD.tag=9,((_tmp8CD.f1=((_tmp8CC.elt_type=_tmp8B,((
_tmp8CC.tq=_tmp7D,((_tmp8CC.num_elts=nelts2,((_tmp8CC.zero_term=_tmp7F,((_tmp8CC.zt_loc=
_tmp80,_tmp8CC)))))))))),_tmp8CD)))),_tmp8CB))));}goto _LL2E;};}_LL31: {struct Cyc_Absyn_PointerType_struct*
_tmp81=(struct Cyc_Absyn_PointerType_struct*)_tmp79;if(_tmp81->tag != 5)goto _LL33;
else{_tmp82=_tmp81->f1;_tmp83=_tmp82.elt_typ;_tmp84=_tmp82.elt_tq;_tmp85=_tmp82.ptr_atts;
_tmp86=_tmp85.rgn;_tmp87=_tmp85.nullable;_tmp88=_tmp85.bounds;_tmp89=_tmp85.zero_term;
_tmp8A=_tmp85.ptrloc;}}_LL32: {void*_tmp8F=Cyc_Parse_substitute_tags(tags,_tmp83);
union Cyc_Absyn_Constraint*_tmp90=_tmp88;{union Cyc_Absyn_Constraint _tmp91=*_tmp88;
void*_tmp92;struct Cyc_Absyn_Exp*_tmp94;_LL36: if((_tmp91.Eq_constr).tag != 1)goto
_LL38;_tmp92=(void*)(_tmp91.Eq_constr).val;{struct Cyc_Absyn_Upper_b_struct*
_tmp93=(struct Cyc_Absyn_Upper_b_struct*)_tmp92;if(_tmp93->tag != 1)goto _LL38;
else{_tmp94=_tmp93->f1;}};_LL37: _tmp94=Cyc_Parse_substitute_tags_exp(tags,_tmp94);{
struct Cyc_Absyn_Upper_b_struct*_tmp8D3;struct Cyc_Absyn_Upper_b_struct _tmp8D2;
union Cyc_Absyn_Constraint*_tmp8D1;_tmp90=((_tmp8D1=_cycalloc(sizeof(*_tmp8D1)),(((
_tmp8D1->Eq_constr).val=(void*)((_tmp8D3=_cycalloc(sizeof(*_tmp8D3)),((_tmp8D3[0]=((
_tmp8D2.tag=1,((_tmp8D2.f1=_tmp94,_tmp8D2)))),_tmp8D3)))),(((_tmp8D1->Eq_constr).tag=
1,_tmp8D1))))));}goto _LL35;_LL38:;_LL39: goto _LL35;_LL35:;}if(_tmp8F != _tmp83  || 
_tmp90 != _tmp88){struct Cyc_Absyn_PointerType_struct _tmp8DD;struct Cyc_Absyn_PtrAtts
_tmp8DC;struct Cyc_Absyn_PtrInfo _tmp8DB;struct Cyc_Absyn_PointerType_struct*
_tmp8DA;return(void*)((_tmp8DA=_cycalloc(sizeof(*_tmp8DA)),((_tmp8DA[0]=((
_tmp8DD.tag=5,((_tmp8DD.f1=((_tmp8DB.elt_typ=_tmp8F,((_tmp8DB.elt_tq=_tmp84,((
_tmp8DB.ptr_atts=((_tmp8DC.rgn=_tmp86,((_tmp8DC.nullable=_tmp87,((_tmp8DC.bounds=
_tmp90,((_tmp8DC.zero_term=_tmp89,((_tmp8DC.ptrloc=_tmp8A,_tmp8DC)))))))))),
_tmp8DB)))))),_tmp8DD)))),_tmp8DA))));}goto _LL2E;}_LL33:;_LL34: goto _LL2E;_LL2E:;}
return t;}static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,
struct Cyc_Absyn_Aggrfield*x){x->type=Cyc_Parse_substitute_tags(tags,x->type);}
struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple16*Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment*loc,struct _tuple8*t){struct _tuple16*_tmp8DE;return(
_tmp8DE=_cycalloc(sizeof(*_tmp8DE)),((_tmp8DE->f1=(*t).f2,((_tmp8DE->f2=(*t).f3,
_tmp8DE)))));}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*
d){void*_tmp9D=d->r;struct Cyc_Absyn_Vardecl*_tmp9F;_LL3B: {struct Cyc_Absyn_Var_d_struct*
_tmp9E=(struct Cyc_Absyn_Var_d_struct*)_tmp9D;if(_tmp9E->tag != 0)goto _LL3D;else{
_tmp9F=_tmp9E->f1;}}_LL3C: return _tmp9F;_LL3D:;_LL3E: {const char*_tmp8E1;void*
_tmp8E0;(_tmp8E0=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((_tmp8E1="bad declaration in `forarray' statement",
_tag_dyneither(_tmp8E1,sizeof(char),40))),_tag_dyneither(_tmp8E0,sizeof(void*),0)));}
_LL3A:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmpA2=tm;_LL40: {struct
Cyc_Absyn_TypeParams_mod_struct*_tmpA3=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpA2;if(_tmpA3->tag != 4)goto _LL42;}_LL41: return 1;_LL42:;_LL43: return 0;_LL3F:;}
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){const char*_tmp8E2;if(
Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8E2="`H",_tag_dyneither(_tmp8E2,
sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_HeapRgn_val;else{const char*
_tmp8E3;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8E3="`U",_tag_dyneither(
_tmp8E3,sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_UniqueRgn_val;else{struct
Cyc_Absyn_VarType_struct _tmp8ED;struct _dyneither_ptr*_tmp8EC;struct Cyc_Absyn_Tvar*
_tmp8EB;struct Cyc_Absyn_VarType_struct*_tmp8EA;return(void*)((_tmp8EA=_cycalloc(
sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8ED.tag=2,((_tmp8ED.f1=((_tmp8EB=_cycalloc(
sizeof(*_tmp8EB)),((_tmp8EB->name=((_tmp8EC=_cycalloc(sizeof(*_tmp8EC)),((
_tmp8EC[0]=s,_tmp8EC)))),((_tmp8EB->identity=- 1,((_tmp8EB->kind=k,_tmp8EB)))))))),
_tmp8ED)))),_tmp8EA))));}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct
Cyc_Absyn_Tvar*t){void*k;{void*_tmpAA=Cyc_Absyn_compress_kb(t->kind);struct Cyc_Absyn_Kind*
_tmpAD;_LL45: {struct Cyc_Absyn_Unknown_kb_struct*_tmpAB=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpAA;if(_tmpAB->tag != 1)goto _LL47;}_LL46:{struct Cyc_Absyn_Unknown_kb_struct
_tmp8F0;struct Cyc_Absyn_Unknown_kb_struct*_tmp8EF;k=(void*)((_tmp8EF=_cycalloc(
sizeof(*_tmp8EF)),((_tmp8EF[0]=((_tmp8F0.tag=1,((_tmp8F0.f1=0,_tmp8F0)))),
_tmp8EF))));}goto _LL44;_LL47: {struct Cyc_Absyn_Less_kb_struct*_tmpAC=(struct Cyc_Absyn_Less_kb_struct*)
_tmpAA;if(_tmpAC->tag != 2)goto _LL49;else{_tmpAD=_tmpAC->f2;}}_LL48:{struct Cyc_Absyn_Less_kb_struct
_tmp8F3;struct Cyc_Absyn_Less_kb_struct*_tmp8F2;k=(void*)((_tmp8F2=_cycalloc(
sizeof(*_tmp8F2)),((_tmp8F2[0]=((_tmp8F3.tag=2,((_tmp8F3.f1=0,((_tmp8F3.f2=
_tmpAD,_tmp8F3)))))),_tmp8F2))));}goto _LL44;_LL49:;_LL4A: k=_tmpAA;goto _LL44;
_LL44:;}{struct Cyc_Absyn_Tvar*_tmp8F4;return(_tmp8F4=_cycalloc(sizeof(*_tmp8F4)),((
_tmp8F4->name=t->name,((_tmp8F4->identity=- 1,((_tmp8F4->kind=k,_tmp8F4)))))));};}
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,
void*t){void*_tmpB3=t;struct Cyc_Absyn_Tvar*_tmpB5;_LL4C: {struct Cyc_Absyn_VarType_struct*
_tmpB4=(struct Cyc_Absyn_VarType_struct*)_tmpB3;if(_tmpB4->tag != 2)goto _LL4E;
else{_tmpB5=_tmpB4->f1;}}_LL4D: return _tmpB5;_LL4E:;_LL4F: {const char*_tmp8F7;
void*_tmp8F6;(_tmp8F6=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp8F7="expecting a list of type variables, not types",
_tag_dyneither(_tmp8F7,sizeof(char),46))),_tag_dyneither(_tmp8F6,sizeof(void*),0)));}
_LL4B:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct
_tmp8FA;struct Cyc_Absyn_VarType_struct*_tmp8F9;return(void*)((_tmp8F9=_cycalloc(
sizeof(*_tmp8F9)),((_tmp8F9[0]=((_tmp8FA.tag=2,((_tmp8FA.f1=pr,_tmp8FA)))),
_tmp8F9))));}static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,
int leq){void*_tmpBA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpBC;struct Cyc_Absyn_Tvar
_tmpBD;void*_tmpBE;void**_tmpBF;_LL51: {struct Cyc_Absyn_VarType_struct*_tmpBB=(
struct Cyc_Absyn_VarType_struct*)_tmpBA;if(_tmpBB->tag != 2)goto _LL53;else{_tmpBC=
_tmpBB->f1;_tmpBD=*_tmpBC;_tmpBE=_tmpBD.kind;_tmpBF=(void**)&(*_tmpBB->f1).kind;}}
_LL52: {void*_tmpC0=Cyc_Absyn_compress_kb(*_tmpBF);_LL56: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpC1=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC0;if(_tmpC1->tag != 1)goto _LL58;}
_LL57: if(!leq)*_tmpBF=Cyc_Tcutil_kind_to_bound(k);else{struct Cyc_Absyn_Less_kb_struct
_tmp8FD;struct Cyc_Absyn_Less_kb_struct*_tmp8FC;*_tmpBF=(void*)((_tmp8FC=
_cycalloc(sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FD.tag=2,((_tmp8FD.f1=0,((
_tmp8FD.f2=k,_tmp8FD)))))),_tmp8FC))));}return;_LL58:;_LL59: return;_LL55:;}_LL53:;
_LL54: return;_LL50:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmpC4=(void*)tms->hd;void*_tmpC6;
_LL5B: {struct Cyc_Absyn_Function_mod_struct*_tmpC5=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC4;if(_tmpC5->tag != 3)goto _LL5D;else{_tmpC6=(void*)_tmpC5->f1;}}_LL5C: if(tms->tl
== 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)
 && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){void*_tmpC7=_tmpC6;
struct Cyc_List_List*_tmpCA;_LL60: {struct Cyc_Absyn_WithTypes_struct*_tmpC8=(
struct Cyc_Absyn_WithTypes_struct*)_tmpC7;if(_tmpC8->tag != 1)goto _LL62;}_LL61:{
const char*_tmp900;void*_tmp8FF;(_tmp8FF=0,Cyc_Tcutil_warn(loc,((_tmp900="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp900,sizeof(char),93))),_tag_dyneither(_tmp8FF,sizeof(void*),0)));}
return tms;_LL62: {struct Cyc_Absyn_NoTypes_struct*_tmpC9=(struct Cyc_Absyn_NoTypes_struct*)
_tmpC7;if(_tmpC9->tag != 0)goto _LL5F;else{_tmpCA=_tmpC9->f1;}}_LL63: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpCA)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds)){const char*_tmp903;void*_tmp902;(_tmp902=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
loc,((_tmp903="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp903,sizeof(char),73))),_tag_dyneither(_tmp902,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpCF=0;for(0;_tmpCA != 0;_tmpCA=_tmpCA->tl){struct Cyc_List_List*
_tmpD0=tds;for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){struct Cyc_Absyn_Decl*_tmpD1=(
struct Cyc_Absyn_Decl*)_tmpD0->hd;void*_tmpD2=_tmpD1->r;struct Cyc_Absyn_Vardecl*
_tmpD4;_LL65: {struct Cyc_Absyn_Var_d_struct*_tmpD3=(struct Cyc_Absyn_Var_d_struct*)
_tmpD2;if(_tmpD3->tag != 0)goto _LL67;else{_tmpD4=_tmpD3->f1;}}_LL66: if(Cyc_zstrptrcmp((*
_tmpD4->name).f2,(struct _dyneither_ptr*)_tmpCA->hd)!= 0)continue;if(_tmpD4->initializer
!= 0){const char*_tmp906;void*_tmp905;(_tmp905=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,((
_tmp906="initializer found in parameter declaration",_tag_dyneither(_tmp906,
sizeof(char),43))),_tag_dyneither(_tmp905,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmpD4->name)){const char*_tmp909;void*_tmp908;(_tmp908=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,((
_tmp909="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp909,
sizeof(char),47))),_tag_dyneither(_tmp908,sizeof(void*),0)));}{struct _tuple8*
_tmp90F;struct Cyc_Core_Opt*_tmp90E;struct Cyc_List_List*_tmp90D;_tmpCF=((_tmp90D=
_cycalloc(sizeof(*_tmp90D)),((_tmp90D->hd=((_tmp90F=_cycalloc(sizeof(*_tmp90F)),((
_tmp90F->f1=((_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E->v=(*_tmpD4->name).f2,
_tmp90E)))),((_tmp90F->f2=_tmpD4->tq,((_tmp90F->f3=_tmpD4->type,_tmp90F)))))))),((
_tmp90D->tl=_tmpCF,_tmp90D))))));}goto L;_LL67:;_LL68: {const char*_tmp912;void*
_tmp911;(_tmp911=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,((_tmp912="nonvariable declaration in parameter type",
_tag_dyneither(_tmp912,sizeof(char),42))),_tag_dyneither(_tmp911,sizeof(void*),0)));}
_LL64:;}L: if(_tmpD0 == 0){const char*_tmp916;void*_tmp915[1];struct Cyc_String_pa_struct
_tmp914;(_tmp914.tag=0,((_tmp914.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmpCA->hd)),((_tmp915[0]=& _tmp914,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp916="%s is not given a type",_tag_dyneither(_tmp916,sizeof(char),23))),
_tag_dyneither(_tmp915,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp925;struct Cyc_Absyn_WithTypes_struct*_tmp924;struct Cyc_Absyn_WithTypes_struct
_tmp923;struct Cyc_Absyn_Function_mod_struct _tmp922;struct Cyc_List_List*_tmp921;
return(_tmp921=_cycalloc(sizeof(*_tmp921)),((_tmp921->hd=(void*)((_tmp925=
_cycalloc(sizeof(*_tmp925)),((_tmp925[0]=((_tmp922.tag=3,((_tmp922.f1=(void*)((
void*)((_tmp924=_cycalloc(sizeof(*_tmp924)),((_tmp924[0]=((_tmp923.tag=1,((
_tmp923.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpCF),((_tmp923.f2=0,((_tmp923.f3=0,((_tmp923.f4=0,((_tmp923.f5=0,_tmp923)))))))))))),
_tmp924))))),_tmp922)))),_tmp925)))),((_tmp921->tl=0,_tmp921)))));};};_LL5F:;}
goto _LL5E;_LL5D:;_LL5E: {struct Cyc_List_List*_tmp926;return(_tmp926=_cycalloc(
sizeof(*_tmp926)),((_tmp926->hd=(void*)tms->hd,((_tmp926->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp926)))));}_LL5A:;};}struct _tuple17{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){if(tds != 0){struct Cyc_Parse_Declarator*
_tmp927;d=((_tmp927=_cycalloc(sizeof(*_tmp927)),((_tmp927->id=d->id,((_tmp927->tms=
Cyc_Parse_oldstyle2newstyle(d->tms,tds,loc),_tmp927))))));}{enum Cyc_Absyn_Scope
sc=Cyc_Absyn_Public;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(
0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0)
switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v){case Cyc_Parse_Extern_sc: _LL69: sc=
Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL6A: sc=Cyc_Absyn_Static;break;
default: _LL6B:{const char*_tmp928;Cyc_Parse_err(((_tmp928="bad storage class on function",
_tag_dyneither(_tmp928,sizeof(char),30))),loc);}break;}}{void*_tmpEA;struct Cyc_Core_Opt*
_tmpEB;struct _tuple10 _tmpE9=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmpEA=
_tmpE9.f1;_tmpEB=_tmpE9.f2;{struct Cyc_Absyn_Tqual _tmpED;void*_tmpEE;struct Cyc_List_List*
_tmpEF;struct Cyc_List_List*_tmpF0;struct _tuple11 _tmpEC=Cyc_Parse_apply_tms(tq,
_tmpEA,atts,d->tms);_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;_tmpEF=_tmpEC.f3;_tmpF0=
_tmpEC.f4;if(_tmpEB != 0){const char*_tmp92B;void*_tmp92A;(_tmp92A=0,Cyc_Tcutil_warn(
loc,((_tmp92B="nested type declaration within function prototype",_tag_dyneither(
_tmp92B,sizeof(char),50))),_tag_dyneither(_tmp92A,sizeof(void*),0)));}if(_tmpEF
!= 0){const char*_tmp92E;void*_tmp92D;(_tmp92D=0,Cyc_Tcutil_warn(loc,((_tmp92E="bad type params, ignoring",
_tag_dyneither(_tmp92E,sizeof(char),26))),_tag_dyneither(_tmp92D,sizeof(void*),0)));}{
void*_tmpF5=_tmpEE;struct Cyc_Absyn_FnInfo _tmpF7;struct Cyc_List_List*_tmpF8;
struct Cyc_Core_Opt*_tmpF9;void*_tmpFA;struct Cyc_List_List*_tmpFB;int _tmpFC;
struct Cyc_Absyn_VarargInfo*_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*
_tmpFF;_LL6E: {struct Cyc_Absyn_FnType_struct*_tmpF6=(struct Cyc_Absyn_FnType_struct*)
_tmpF5;if(_tmpF6->tag != 10)goto _LL70;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF7.tvars;
_tmpF9=_tmpF7.effect;_tmpFA=_tmpF7.ret_typ;_tmpFB=_tmpF7.args;_tmpFC=_tmpF7.c_varargs;
_tmpFD=_tmpF7.cyc_varargs;_tmpFE=_tmpF7.rgn_po;_tmpFF=_tmpF7.attributes;}}_LL6F: {
struct Cyc_List_List*_tmp100=0;{struct Cyc_List_List*_tmp101=_tmpFB;for(0;_tmp101
!= 0;_tmp101=_tmp101->tl){struct _tuple8 _tmp103;struct Cyc_Core_Opt*_tmp104;struct
Cyc_Absyn_Tqual _tmp105;void*_tmp106;struct _tuple8*_tmp102=(struct _tuple8*)
_tmp101->hd;_tmp103=*_tmp102;_tmp104=_tmp103.f1;_tmp105=_tmp103.f2;_tmp106=
_tmp103.f3;if(_tmp104 == 0){{const char*_tmp92F;Cyc_Parse_err(((_tmp92F="missing argument variable in function prototype",
_tag_dyneither(_tmp92F,sizeof(char),48))),loc);}{struct _tuple17*_tmp932;struct
Cyc_List_List*_tmp931;_tmp100=((_tmp931=_cycalloc(sizeof(*_tmp931)),((_tmp931->hd=((
_tmp932=_cycalloc(sizeof(*_tmp932)),((_tmp932->f1=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp932->f2=_tmp105,((
_tmp932->f3=_tmp106,_tmp932)))))))),((_tmp931->tl=_tmp100,_tmp931))))));};}else{
struct _tuple17*_tmp935;struct Cyc_List_List*_tmp934;_tmp100=((_tmp934=_cycalloc(
sizeof(*_tmp934)),((_tmp934->hd=((_tmp935=_cycalloc(sizeof(*_tmp935)),((_tmp935->f1=(
struct _dyneither_ptr*)_tmp104->v,((_tmp935->f2=_tmp105,((_tmp935->f3=_tmp106,
_tmp935)))))))),((_tmp934->tl=_tmp100,_tmp934))))));}}}{struct Cyc_Absyn_Fndecl*
_tmp936;return(_tmp936=_cycalloc(sizeof(*_tmp936)),((_tmp936->sc=sc,((_tmp936->name=
d->id,((_tmp936->tvs=_tmpF8,((_tmp936->is_inline=is_inline,((_tmp936->effect=
_tmpF9,((_tmp936->ret_type=_tmpFA,((_tmp936->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp100),((_tmp936->c_varargs=_tmpFC,((
_tmp936->cyc_varargs=_tmpFD,((_tmp936->rgn_po=_tmpFE,((_tmp936->body=body,((
_tmp936->cached_typ=0,((_tmp936->param_vardecls=0,((_tmp936->fn_vardecl=0,((
_tmp936->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmpFF,_tmpF0),_tmp936)))))))))))))))))))))))))))))));};}
_LL70:;_LL71: {const char*_tmp939;void*_tmp938;(_tmp938=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp939="declarator is not a function prototype",_tag_dyneither(_tmp939,sizeof(
char),39))),_tag_dyneither(_tmp938,sizeof(void*),0)));}_LL6D:;};};};};}static
char _tmp110[52]="at most one type may appear within a type specifier";static
struct _dyneither_ptr Cyc_Parse_msg1={_tmp110,_tmp110,_tmp110 + 52};static char
_tmp111[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp111,_tmp111,_tmp111 + 63};static
char _tmp112[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp112,_tmp112,_tmp112 + 50};static char _tmp113[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp113,_tmp113,_tmp113 + 60};static
struct _tuple10 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct
Cyc_Position_Segment*loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int
seen_sign=0;int seen_size=0;void*t=(void*)& Cyc_Absyn_VoidType_val;enum Cyc_Absyn_Size_of
sz=Cyc_Absyn_Int_sz;enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp114=(void*)ts->hd;void*_tmp116;
struct Cyc_Position_Segment*_tmp117;struct Cyc_Position_Segment*_tmp119;struct Cyc_Position_Segment*
_tmp11B;struct Cyc_Position_Segment*_tmp11D;struct Cyc_Position_Segment*_tmp11F;
struct Cyc_Absyn_Decl*_tmp121;_LL73: {struct Cyc_Parse_Type_spec_struct*_tmp115=(
struct Cyc_Parse_Type_spec_struct*)_tmp114;if(_tmp115->tag != 4)goto _LL75;else{
_tmp116=(void*)_tmp115->f1;_tmp117=_tmp115->f2;}}_LL74: if(seen_type)Cyc_Parse_err(
Cyc_Parse_msg1,_tmp117);last_loc=_tmp117;seen_type=1;t=_tmp116;goto _LL72;_LL75: {
struct Cyc_Parse_Signed_spec_struct*_tmp118=(struct Cyc_Parse_Signed_spec_struct*)
_tmp114;if(_tmp118->tag != 0)goto _LL77;else{_tmp119=_tmp118->f1;}}_LL76: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp119);if(seen_type){const char*_tmp93A;
Cyc_Parse_err(((_tmp93A="signed qualifier must come before type",_tag_dyneither(
_tmp93A,sizeof(char),39))),_tmp119);}last_loc=_tmp119;seen_sign=1;sgn=Cyc_Absyn_Signed;
goto _LL72;_LL77: {struct Cyc_Parse_Unsigned_spec_struct*_tmp11A=(struct Cyc_Parse_Unsigned_spec_struct*)
_tmp114;if(_tmp11A->tag != 1)goto _LL79;else{_tmp11B=_tmp11A->f1;}}_LL78: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp11B);if(seen_type){const char*_tmp93B;
Cyc_Parse_err(((_tmp93B="signed qualifier must come before type",_tag_dyneither(
_tmp93B,sizeof(char),39))),_tmp11B);}last_loc=_tmp11B;seen_sign=1;sgn=Cyc_Absyn_Unsigned;
goto _LL72;_LL79: {struct Cyc_Parse_Short_spec_struct*_tmp11C=(struct Cyc_Parse_Short_spec_struct*)
_tmp114;if(_tmp11C->tag != 2)goto _LL7B;else{_tmp11D=_tmp11C->f1;}}_LL7A: if(
seen_size){const char*_tmp93C;Cyc_Parse_err(((_tmp93C="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp93C,sizeof(char),59))),_tmp11D);}if(seen_type){const char*
_tmp93D;Cyc_Parse_err(((_tmp93D="short modifier must come before base type",
_tag_dyneither(_tmp93D,sizeof(char),42))),_tmp11D);}last_loc=_tmp11D;seen_size=1;
sz=Cyc_Absyn_Short_sz;goto _LL72;_LL7B: {struct Cyc_Parse_Long_spec_struct*_tmp11E=(
struct Cyc_Parse_Long_spec_struct*)_tmp114;if(_tmp11E->tag != 3)goto _LL7D;else{
_tmp11F=_tmp11E->f1;}}_LL7C: if(seen_type){const char*_tmp93E;Cyc_Parse_err(((
_tmp93E="long modifier must come before base type",_tag_dyneither(_tmp93E,
sizeof(char),41))),_tmp11F);}if(seen_size)switch(sz){case Cyc_Absyn_Long_sz: _LL7F:
sz=Cyc_Absyn_LongLong_sz;break;default: _LL80:{const char*_tmp93F;Cyc_Parse_err(((
_tmp93F="extra long in type specifier",_tag_dyneither(_tmp93F,sizeof(char),29))),
_tmp11F);}break;}else{sz=Cyc_Absyn_Long_sz;}last_loc=_tmp11F;seen_size=1;goto
_LL72;_LL7D: {struct Cyc_Parse_Decl_spec_struct*_tmp120=(struct Cyc_Parse_Decl_spec_struct*)
_tmp114;if(_tmp120->tag != 5)goto _LL72;else{_tmp121=_tmp120->f1;}}_LL7E: last_loc=
_tmp121->loc;if(declopt == 0  && !seen_type){seen_type=1;{void*_tmp128=_tmp121->r;
struct Cyc_Absyn_Aggrdecl*_tmp12A;struct Cyc_Absyn_Datatypedecl*_tmp12C;struct Cyc_Absyn_Enumdecl*
_tmp12E;_LL83: {struct Cyc_Absyn_Aggr_d_struct*_tmp129=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp128;if(_tmp129->tag != 6)goto _LL85;else{_tmp12A=_tmp129->f1;}}_LL84: {struct
Cyc_List_List*_tmp12F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmp12A->tvs));{struct Cyc_Absyn_AggrType_struct _tmp945;
struct Cyc_Absyn_AggrInfo _tmp944;struct Cyc_Absyn_AggrType_struct*_tmp943;t=(void*)((
_tmp943=_cycalloc(sizeof(*_tmp943)),((_tmp943[0]=((_tmp945.tag=12,((_tmp945.f1=((
_tmp944.aggr_info=Cyc_Absyn_UnknownAggr(_tmp12A->kind,_tmp12A->name,0),((_tmp944.targs=
_tmp12F,_tmp944)))),_tmp945)))),_tmp943))));}if(_tmp12A->impl != 0){struct Cyc_Core_Opt*
_tmp946;declopt=((_tmp946=_cycalloc(sizeof(*_tmp946)),((_tmp946->v=_tmp121,
_tmp946))));}goto _LL82;}_LL85: {struct Cyc_Absyn_Datatype_d_struct*_tmp12B=(
struct Cyc_Absyn_Datatype_d_struct*)_tmp128;if(_tmp12B->tag != 7)goto _LL87;else{
_tmp12C=_tmp12B->f1;}}_LL86: {struct Cyc_List_List*_tmp134=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmp12C->tvs));{struct Cyc_Absyn_DatatypeType_struct
_tmp950;struct Cyc_Absyn_Datatypedecl**_tmp94F;struct Cyc_Absyn_DatatypeInfo
_tmp94E;struct Cyc_Absyn_DatatypeType_struct*_tmp94D;t=(void*)((_tmp94D=_cycalloc(
sizeof(*_tmp94D)),((_tmp94D[0]=((_tmp950.tag=3,((_tmp950.f1=((_tmp94E.datatype_info=
Cyc_Absyn_KnownDatatype(((_tmp94F=_cycalloc(sizeof(*_tmp94F)),((_tmp94F[0]=
_tmp12C,_tmp94F))))),((_tmp94E.targs=_tmp134,_tmp94E)))),_tmp950)))),_tmp94D))));}
if(_tmp12C->fields != 0){struct Cyc_Core_Opt*_tmp951;declopt=((_tmp951=_cycalloc(
sizeof(*_tmp951)),((_tmp951->v=_tmp121,_tmp951))));}goto _LL82;}_LL87: {struct Cyc_Absyn_Enum_d_struct*
_tmp12D=(struct Cyc_Absyn_Enum_d_struct*)_tmp128;if(_tmp12D->tag != 8)goto _LL89;
else{_tmp12E=_tmp12D->f1;}}_LL88:{struct Cyc_Absyn_EnumType_struct _tmp954;struct
Cyc_Absyn_EnumType_struct*_tmp953;t=(void*)((_tmp953=_cycalloc(sizeof(*_tmp953)),((
_tmp953[0]=((_tmp954.tag=14,((_tmp954.f1=_tmp12E->name,((_tmp954.f2=0,_tmp954)))))),
_tmp953))));}{struct Cyc_Core_Opt*_tmp955;declopt=((_tmp955=_cycalloc(sizeof(*
_tmp955)),((_tmp955->v=_tmp121,_tmp955))));}goto _LL82;_LL89:;_LL8A: {const char*
_tmp958;void*_tmp957;(_tmp957=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp121->loc,((
_tmp958="bad declaration within type specifier",_tag_dyneither(_tmp958,sizeof(
char),38))),_tag_dyneither(_tmp957,sizeof(void*),0)));}_LL82:;};}else{Cyc_Parse_err(
Cyc_Parse_msg3,_tmp121->loc);}goto _LL72;_LL72:;}if(!seen_type){if(!seen_sign  && 
!seen_size){const char*_tmp95B;void*_tmp95A;(_tmp95A=0,Cyc_Tcutil_warn(last_loc,((
_tmp95B="missing type within specifier",_tag_dyneither(_tmp95B,sizeof(char),30))),
_tag_dyneither(_tmp95A,sizeof(void*),0)));}t=Cyc_Absyn_int_typ(sgn,sz);}else{if(
seen_sign){void*_tmp141=t;enum Cyc_Absyn_Sign _tmp143;enum Cyc_Absyn_Size_of
_tmp144;_LL8C: {struct Cyc_Absyn_IntType_struct*_tmp142=(struct Cyc_Absyn_IntType_struct*)
_tmp141;if(_tmp142->tag != 6)goto _LL8E;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;}}
_LL8D: if(_tmp143 != sgn)t=Cyc_Absyn_int_typ(sgn,_tmp144);goto _LL8B;_LL8E:;_LL8F:{
const char*_tmp95C;Cyc_Parse_err(((_tmp95C="sign specification on non-integral type",
_tag_dyneither(_tmp95C,sizeof(char),40))),last_loc);}goto _LL8B;_LL8B:;}if(
seen_size){void*_tmp146=t;enum Cyc_Absyn_Sign _tmp148;enum Cyc_Absyn_Size_of
_tmp149;_LL91: {struct Cyc_Absyn_IntType_struct*_tmp147=(struct Cyc_Absyn_IntType_struct*)
_tmp146;if(_tmp147->tag != 6)goto _LL93;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}
_LL92: if(_tmp149 != sz)t=Cyc_Absyn_int_typ(_tmp148,sz);goto _LL90;_LL93: {struct
Cyc_Absyn_DoubleType_struct*_tmp14A=(struct Cyc_Absyn_DoubleType_struct*)_tmp146;
if(_tmp14A->tag != 8)goto _LL95;}_LL94: t=Cyc_Absyn_double_typ(1);goto _LL90;_LL95:;
_LL96:{const char*_tmp95D;Cyc_Parse_err(((_tmp95D="size qualifier on non-integral type",
_tag_dyneither(_tmp95D,sizeof(char),36))),last_loc);}goto _LL90;_LL90:;}}{struct
_tuple10 _tmp95E;return(_tmp95E.f1=t,((_tmp95E.f2=declopt,_tmp95E)));};}static
struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)
return 0;{struct Cyc_Parse_Declarator*_tmp14D=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmp14E=_tmp14D->id;struct Cyc_Absyn_Tqual _tmp150;void*_tmp151;
struct Cyc_List_List*_tmp152;struct Cyc_List_List*_tmp153;struct _tuple11 _tmp14F=
Cyc_Parse_apply_tms(tq,t,shared_atts,_tmp14D->tms);_tmp150=_tmp14F.f1;_tmp151=
_tmp14F.f2;_tmp152=_tmp14F.f3;_tmp153=_tmp14F.f4;if(ds->tl == 0){struct _tuple13*
_tmp961;struct Cyc_List_List*_tmp960;return(_tmp960=_region_malloc(r,sizeof(*
_tmp960)),((_tmp960->hd=((_tmp961=_region_malloc(r,sizeof(*_tmp961)),((_tmp961->f1=
_tmp14E,((_tmp961->f2=_tmp150,((_tmp961->f3=_tmp151,((_tmp961->f4=_tmp152,((
_tmp961->f5=_tmp153,_tmp961)))))))))))),((_tmp960->tl=0,_tmp960)))));}else{
struct _tuple13*_tmp964;struct Cyc_List_List*_tmp963;return(_tmp963=_region_malloc(
r,sizeof(*_tmp963)),((_tmp963->hd=((_tmp964=_region_malloc(r,sizeof(*_tmp964)),((
_tmp964->f1=_tmp14E,((_tmp964->f2=_tmp150,((_tmp964->f3=_tmp151,((_tmp964->f4=
_tmp152,((_tmp964->f5=_tmp153,_tmp964)))))))))))),((_tmp963->tl=Cyc_Parse_apply_tmss(
r,_tmp150,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp963)))));}};}static
struct _tuple11 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0){struct _tuple11 _tmp965;return(_tmp965.f1=
tq,((_tmp965.f2=t,((_tmp965.f3=0,((_tmp965.f4=atts,_tmp965)))))));}{void*_tmp159=(
void*)tms->hd;union Cyc_Absyn_Constraint*_tmp15B;struct Cyc_Position_Segment*
_tmp15C;struct Cyc_Absyn_Exp*_tmp15E;union Cyc_Absyn_Constraint*_tmp15F;struct Cyc_Position_Segment*
_tmp160;void*_tmp162;struct Cyc_List_List*_tmp164;struct Cyc_Position_Segment*
_tmp165;struct Cyc_Absyn_PtrAtts _tmp167;struct Cyc_Absyn_Tqual _tmp168;struct Cyc_Position_Segment*
_tmp16A;struct Cyc_List_List*_tmp16B;_LL98: {struct Cyc_Absyn_Carray_mod_struct*
_tmp15A=(struct Cyc_Absyn_Carray_mod_struct*)_tmp159;if(_tmp15A->tag != 0)goto
_LL9A;else{_tmp15B=_tmp15A->f1;_tmp15C=_tmp15A->f2;}}_LL99: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmp15B,_tmp15C),atts,tms->tl);
_LL9A: {struct Cyc_Absyn_ConstArray_mod_struct*_tmp15D=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp159;if(_tmp15D->tag != 1)goto _LL9C;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15D->f2;
_tmp160=_tmp15D->f3;}}_LL9B: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmp15E,_tmp15F,_tmp160),atts,tms->tl);
_LL9C: {struct Cyc_Absyn_Function_mod_struct*_tmp161=(struct Cyc_Absyn_Function_mod_struct*)
_tmp159;if(_tmp161->tag != 3)goto _LL9E;else{_tmp162=(void*)_tmp161->f1;}}_LL9D: {
void*_tmp16C=_tmp162;struct Cyc_List_List*_tmp16E;int _tmp16F;struct Cyc_Absyn_VarargInfo*
_tmp170;struct Cyc_Core_Opt*_tmp171;struct Cyc_List_List*_tmp172;struct Cyc_Position_Segment*
_tmp174;_LLA5: {struct Cyc_Absyn_WithTypes_struct*_tmp16D=(struct Cyc_Absyn_WithTypes_struct*)
_tmp16C;if(_tmp16D->tag != 1)goto _LLA7;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;
_tmp170=_tmp16D->f3;_tmp171=_tmp16D->f4;_tmp172=_tmp16D->f5;}}_LLA6: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp966;fn_atts=((_tmp966=_cycalloc(sizeof(*_tmp966)),((
_tmp966->hd=(void*)as->hd,((_tmp966->tl=fn_atts,_tmp966))))));}else{struct Cyc_List_List*
_tmp967;new_atts=((_tmp967=_cycalloc(sizeof(*_tmp967)),((_tmp967->hd=(void*)as->hd,((
_tmp967->tl=new_atts,_tmp967))))));}}}if(tms->tl != 0){void*_tmp177=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;struct Cyc_List_List*_tmp179;_LLAA: {
struct Cyc_Absyn_TypeParams_mod_struct*_tmp178=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp177;if(_tmp178->tag != 4)goto _LLAC;else{_tmp179=_tmp178->f1;}}_LLAB: typvars=
_tmp179;tms=tms->tl;goto _LLA9;_LLAC:;_LLAD: goto _LLA9;_LLA9:;}if(((((!_tmp16F  && 
_tmp170 == 0) && _tmp16E != 0) && _tmp16E->tl == 0) && (*((struct _tuple8*)_tmp16E->hd)).f1
== 0) && (*((struct _tuple8*)_tmp16E->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
_tmp16E=0;{struct Cyc_List_List*_tmp17A=Cyc_Parse_get_arg_tags(_tmp16E);if(
_tmp17A != 0)t=Cyc_Parse_substitute_tags(_tmp17A,t);t=Cyc_Parse_array2ptr(t,0);{
struct Cyc_List_List*_tmp17B=_tmp16E;for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){
struct _tuple8 _tmp17D;struct Cyc_Core_Opt*_tmp17E;struct Cyc_Absyn_Tqual _tmp17F;
void*_tmp180;void**_tmp181;struct _tuple8*_tmp17C=(struct _tuple8*)_tmp17B->hd;
_tmp17D=*_tmp17C;_tmp17E=_tmp17D.f1;_tmp17F=_tmp17D.f2;_tmp180=_tmp17D.f3;
_tmp181=(void**)&(*_tmp17C).f3;if(_tmp17A != 0)*_tmp181=Cyc_Parse_substitute_tags(
_tmp17A,*_tmp181);*_tmp181=Cyc_Parse_array2ptr(*_tmp181,1);}}return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(tq.loc),Cyc_Absyn_function_typ(typvars,_tmp171,t,_tmp16E,
_tmp16F,_tmp170,_tmp172,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}
_LLA7: {struct Cyc_Absyn_NoTypes_struct*_tmp173=(struct Cyc_Absyn_NoTypes_struct*)
_tmp16C;if(_tmp173->tag != 0)goto _LLA4;else{_tmp174=_tmp173->f2;}}_LLA8: {const
char*_tmp96A;void*_tmp969;(_tmp969=0,((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp174,((
_tmp96A="function declaration without parameter types",_tag_dyneither(_tmp96A,
sizeof(char),45))),_tag_dyneither(_tmp969,sizeof(void*),0)));}_LLA4:;}_LL9E: {
struct Cyc_Absyn_TypeParams_mod_struct*_tmp163=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp159;if(_tmp163->tag != 4)goto _LLA0;else{_tmp164=_tmp163->f1;_tmp165=_tmp163->f2;}}
_LL9F: if(tms->tl == 0){struct _tuple11 _tmp96B;return(_tmp96B.f1=tq,((_tmp96B.f2=t,((
_tmp96B.f3=_tmp164,((_tmp96B.f4=atts,_tmp96B)))))));}{const char*_tmp96E;void*
_tmp96D;(_tmp96D=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp165,((_tmp96E="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp96E,sizeof(char),68))),_tag_dyneither(_tmp96D,sizeof(void*),0)));};
_LLA0: {struct Cyc_Absyn_Pointer_mod_struct*_tmp166=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp159;if(_tmp166->tag != 2)goto _LLA2;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}
_LLA1: {struct Cyc_Absyn_PointerType_struct _tmp974;struct Cyc_Absyn_PtrInfo _tmp973;
struct Cyc_Absyn_PointerType_struct*_tmp972;return Cyc_Parse_apply_tms(_tmp168,(
void*)((_tmp972=_cycalloc(sizeof(*_tmp972)),((_tmp972[0]=((_tmp974.tag=5,((
_tmp974.f1=((_tmp973.elt_typ=t,((_tmp973.elt_tq=tq,((_tmp973.ptr_atts=_tmp167,
_tmp973)))))),_tmp974)))),_tmp972)))),atts,tms->tl);}_LLA2: {struct Cyc_Absyn_Attributes_mod_struct*
_tmp169=(struct Cyc_Absyn_Attributes_mod_struct*)_tmp159;if(_tmp169->tag != 5)goto
_LL97;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;}}_LLA3: return Cyc_Parse_apply_tms(
tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
atts,_tmp16B),tms->tl);_LL97:;};}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*
tss,struct Cyc_Position_Segment*loc){void*_tmp18B;struct Cyc_Core_Opt*_tmp18C;
struct _tuple10 _tmp18A=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmp18B=
_tmp18A.f1;_tmp18C=_tmp18A.f2;if(_tmp18C != 0){const char*_tmp977;void*_tmp976;(
_tmp976=0,Cyc_Tcutil_warn(loc,((_tmp977="ignoring nested type declaration(s) in specifier list",
_tag_dyneither(_tmp977,sizeof(char),54))),_tag_dyneither(_tmp976,sizeof(void*),0)));}
return _tmp18B;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple13*t){struct _tuple13 _tmp190;struct _tuple1*_tmp191;struct Cyc_Absyn_Tqual
_tmp192;void*_tmp193;struct Cyc_List_List*_tmp194;struct Cyc_List_List*_tmp195;
struct _tuple13*_tmp18F=t;_tmp190=*_tmp18F;_tmp191=_tmp190.f1;_tmp192=_tmp190.f2;
_tmp193=_tmp190.f3;_tmp194=_tmp190.f4;_tmp195=_tmp190.f5;Cyc_Lex_register_typedef(
_tmp191);{struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*type;{void*_tmp196=_tmp193;
struct Cyc_Core_Opt*_tmp198;_LLAF: {struct Cyc_Absyn_Evar_struct*_tmp197=(struct
Cyc_Absyn_Evar_struct*)_tmp196;if(_tmp197->tag != 1)goto _LLB1;else{_tmp198=
_tmp197->f1;}}_LLB0: type=0;if(_tmp198 == 0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;
else{kind=_tmp198;}goto _LLAE;_LLB1:;_LLB2: kind=0;{struct Cyc_Core_Opt*_tmp978;
type=((_tmp978=_cycalloc(sizeof(*_tmp978)),((_tmp978->v=_tmp193,_tmp978))));}
goto _LLAE;_LLAE:;}{struct Cyc_Absyn_Typedef_d_struct _tmp97E;struct Cyc_Absyn_Typedefdecl*
_tmp97D;struct Cyc_Absyn_Typedef_d_struct*_tmp97C;return Cyc_Absyn_new_decl((void*)((
_tmp97C=_cycalloc(sizeof(*_tmp97C)),((_tmp97C[0]=((_tmp97E.tag=9,((_tmp97E.f1=((
_tmp97D=_cycalloc(sizeof(*_tmp97D)),((_tmp97D->name=_tmp191,((_tmp97D->tvs=
_tmp194,((_tmp97D->kind=kind,((_tmp97D->defn=type,((_tmp97D->atts=_tmp195,((
_tmp97D->tq=_tmp192,_tmp97D)))))))))))))),_tmp97E)))),_tmp97C)))),loc);};};}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct
Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Decl_s_struct _tmp981;struct Cyc_Absyn_Decl_s_struct*
_tmp980;return Cyc_Absyn_new_stmt((void*)((_tmp980=_cycalloc(sizeof(*_tmp980)),((
_tmp980[0]=((_tmp981.tag=12,((_tmp981.f1=d,((_tmp981.f2=s,_tmp981)))))),_tmp980)))),
d->loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*
ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*
f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*
accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*
Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*
ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*loc){struct
_RegionHandle _tmp19F=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp19F;
_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp1A1;struct Cyc_Absyn_Tqual
_tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_List_List*_tmp1A4;struct Cyc_Parse_Declaration_spec*
_tmp1A0=ds;_tmp1A1=*_tmp1A0;_tmp1A2=_tmp1A1.tq;_tmp1A3=_tmp1A1.type_specs;
_tmp1A4=_tmp1A1.attributes;if(_tmp1A2.loc == 0)_tmp1A2.loc=tqual_loc;if(ds->is_inline){
const char*_tmp982;Cyc_Parse_err(((_tmp982="inline is allowed only on function definitions",
_tag_dyneither(_tmp982,sizeof(char),47))),loc);}if(_tmp1A3 == 0){{const char*
_tmp983;Cyc_Parse_err(((_tmp983="missing type specifiers in declaration",
_tag_dyneither(_tmp983,sizeof(char),39))),loc);}{struct Cyc_List_List*_tmp1A7=0;
_npop_handler(0);return _tmp1A7;};}{enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;int
istypedef=0;if(ds->sc != 0)switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v){case Cyc_Parse_Typedef_sc: _LLB3: istypedef=1;break;case
Cyc_Parse_Extern_sc: _LLB4: s=Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc:
_LLB5: s=Cyc_Absyn_ExternC;break;case Cyc_Parse_Static_sc: _LLB6: s=Cyc_Absyn_Static;
break;case Cyc_Parse_Auto_sc: _LLB7: s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc:
_LLB8: if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}
break;case Cyc_Parse_Abstract_sc: _LLB9: s=Cyc_Absyn_Abstract;break;}{struct Cyc_List_List*
_tmp1A9;struct Cyc_List_List*_tmp1AA;struct _tuple0 _tmp1A8=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
mkrgn,mkrgn,ids);_tmp1A9=_tmp1A8.f1;_tmp1AA=_tmp1A8.f2;{int exps_empty=1;{struct
Cyc_List_List*es=_tmp1AA;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd
!= 0){exps_empty=0;break;}}}{struct _tuple10 _tmp1AB=Cyc_Parse_collapse_type_specifiers(
_tmp1A3,loc);if(_tmp1A9 == 0){void*_tmp1AD;struct Cyc_Core_Opt*_tmp1AE;struct
_tuple10 _tmp1AC=_tmp1AB;_tmp1AD=_tmp1AC.f1;_tmp1AE=_tmp1AC.f2;if(_tmp1AE != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1AE->v;{void*_tmp1AF=d->r;
struct Cyc_Absyn_Enumdecl*_tmp1B1;struct Cyc_Absyn_Aggrdecl*_tmp1B3;struct Cyc_Absyn_Datatypedecl*
_tmp1B5;_LLBC: {struct Cyc_Absyn_Enum_d_struct*_tmp1B0=(struct Cyc_Absyn_Enum_d_struct*)
_tmp1AF;if(_tmp1B0->tag != 8)goto _LLBE;else{_tmp1B1=_tmp1B0->f1;}}_LLBD: _tmp1B1->sc=
s;if(_tmp1A4 != 0){const char*_tmp984;Cyc_Parse_err(((_tmp984="bad attributes on enum",
_tag_dyneither(_tmp984,sizeof(char),23))),loc);}goto _LLBB;_LLBE: {struct Cyc_Absyn_Aggr_d_struct*
_tmp1B2=(struct Cyc_Absyn_Aggr_d_struct*)_tmp1AF;if(_tmp1B2->tag != 6)goto _LLC0;
else{_tmp1B3=_tmp1B2->f1;}}_LLBF: _tmp1B3->sc=s;_tmp1B3->attributes=_tmp1A4;goto
_LLBB;_LLC0: {struct Cyc_Absyn_Datatype_d_struct*_tmp1B4=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp1AF;if(_tmp1B4->tag != 7)goto _LLC2;else{_tmp1B5=_tmp1B4->f1;}}_LLC1: _tmp1B5->sc=
s;if(_tmp1A4 != 0){const char*_tmp985;Cyc_Parse_err(((_tmp985="bad attributes on datatype",
_tag_dyneither(_tmp985,sizeof(char),27))),loc);}goto _LLBB;_LLC2:;_LLC3:{const
char*_tmp986;Cyc_Parse_err(((_tmp986="bad declaration",_tag_dyneither(_tmp986,
sizeof(char),16))),loc);}{struct Cyc_List_List*_tmp1B9=0;_npop_handler(0);return
_tmp1B9;};_LLBB:;}{struct Cyc_List_List*_tmp987;struct Cyc_List_List*_tmp1BB=(
_tmp987=_cycalloc(sizeof(*_tmp987)),((_tmp987->hd=d,((_tmp987->tl=0,_tmp987)))));
_npop_handler(0);return _tmp1BB;};}else{void*_tmp1BC=_tmp1AD;struct Cyc_Absyn_AggrInfo
_tmp1BE;union Cyc_Absyn_AggrInfoU _tmp1BF;struct _tuple3 _tmp1C0;enum Cyc_Absyn_AggrKind
_tmp1C1;struct _tuple1*_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_Absyn_DatatypeInfo
_tmp1C5;union Cyc_Absyn_DatatypeInfoU _tmp1C6;struct Cyc_Absyn_Datatypedecl**
_tmp1C7;struct Cyc_Absyn_DatatypeInfo _tmp1C9;union Cyc_Absyn_DatatypeInfoU _tmp1CA;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp1CB;struct _tuple1*_tmp1CC;int _tmp1CD;
struct Cyc_List_List*_tmp1CE;struct _tuple1*_tmp1D0;struct Cyc_List_List*_tmp1D2;
_LLC5: {struct Cyc_Absyn_AggrType_struct*_tmp1BD=(struct Cyc_Absyn_AggrType_struct*)
_tmp1BC;if(_tmp1BD->tag != 12)goto _LLC7;else{_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BE.aggr_info;
if((_tmp1BF.UnknownAggr).tag != 1)goto _LLC7;_tmp1C0=(struct _tuple3)(_tmp1BF.UnknownAggr).val;
_tmp1C1=_tmp1C0.f1;_tmp1C2=_tmp1C0.f2;_tmp1C3=_tmp1BE.targs;}}_LLC6: {struct Cyc_List_List*
_tmp1D3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp1C3);struct Cyc_Absyn_Aggrdecl*_tmp988;struct Cyc_Absyn_Aggrdecl*_tmp1D4=(
_tmp988=_cycalloc(sizeof(*_tmp988)),((_tmp988->kind=_tmp1C1,((_tmp988->sc=s,((
_tmp988->name=_tmp1C2,((_tmp988->tvs=_tmp1D3,((_tmp988->impl=0,((_tmp988->attributes=
0,_tmp988)))))))))))));if(_tmp1A4 != 0){const char*_tmp989;Cyc_Parse_err(((_tmp989="bad attributes on type declaration",
_tag_dyneither(_tmp989,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp98F;struct Cyc_Absyn_Aggr_d_struct _tmp98E;struct Cyc_List_List*_tmp98D;struct
Cyc_List_List*_tmp1D9=(_tmp98D=_cycalloc(sizeof(*_tmp98D)),((_tmp98D->hd=Cyc_Absyn_new_decl((
void*)((_tmp98F=_cycalloc(sizeof(*_tmp98F)),((_tmp98F[0]=((_tmp98E.tag=6,((
_tmp98E.f1=_tmp1D4,_tmp98E)))),_tmp98F)))),loc),((_tmp98D->tl=0,_tmp98D)))));
_npop_handler(0);return _tmp1D9;};}_LLC7: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1C4=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1BC;if(_tmp1C4->tag != 3)goto
_LLC9;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C5.datatype_info;if((_tmp1C6.KnownDatatype).tag
!= 2)goto _LLC9;_tmp1C7=(struct Cyc_Absyn_Datatypedecl**)(_tmp1C6.KnownDatatype).val;}}
_LLC8: if(_tmp1A4 != 0){const char*_tmp990;Cyc_Parse_err(((_tmp990="bad attributes on datatype",
_tag_dyneither(_tmp990,sizeof(char),27))),loc);}{struct Cyc_Absyn_Datatype_d_struct*
_tmp996;struct Cyc_Absyn_Datatype_d_struct _tmp995;struct Cyc_List_List*_tmp994;
struct Cyc_List_List*_tmp1DF=(_tmp994=_cycalloc(sizeof(*_tmp994)),((_tmp994->hd=
Cyc_Absyn_new_decl((void*)((_tmp996=_cycalloc(sizeof(*_tmp996)),((_tmp996[0]=((
_tmp995.tag=7,((_tmp995.f1=*_tmp1C7,_tmp995)))),_tmp996)))),loc),((_tmp994->tl=0,
_tmp994)))));_npop_handler(0);return _tmp1DF;};_LLC9: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1C8=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1BC;if(_tmp1C8->tag != 3)goto
_LLCB;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=_tmp1C9.datatype_info;if((_tmp1CA.UnknownDatatype).tag
!= 1)goto _LLCB;_tmp1CB=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1CA.UnknownDatatype).val;
_tmp1CC=_tmp1CB.name;_tmp1CD=_tmp1CB.is_extensible;_tmp1CE=_tmp1C9.targs;}}_LLCA: {
struct Cyc_List_List*_tmp1E0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp1CE);struct Cyc_Absyn_Decl*_tmp1E1=
Cyc_Absyn_datatype_decl(s,_tmp1CC,_tmp1E0,0,_tmp1CD,loc);if(_tmp1A4 != 0){const
char*_tmp997;Cyc_Parse_err(((_tmp997="bad attributes on datatype",_tag_dyneither(
_tmp997,sizeof(char),27))),loc);}{struct Cyc_List_List*_tmp998;struct Cyc_List_List*
_tmp1E4=(_tmp998=_cycalloc(sizeof(*_tmp998)),((_tmp998->hd=_tmp1E1,((_tmp998->tl=
0,_tmp998)))));_npop_handler(0);return _tmp1E4;};}_LLCB: {struct Cyc_Absyn_EnumType_struct*
_tmp1CF=(struct Cyc_Absyn_EnumType_struct*)_tmp1BC;if(_tmp1CF->tag != 14)goto _LLCD;
else{_tmp1D0=_tmp1CF->f1;}}_LLCC: {struct Cyc_Absyn_Enumdecl*_tmp999;struct Cyc_Absyn_Enumdecl*
_tmp1E5=(_tmp999=_cycalloc(sizeof(*_tmp999)),((_tmp999->sc=s,((_tmp999->name=
_tmp1D0,((_tmp999->fields=0,_tmp999)))))));if(_tmp1A4 != 0){const char*_tmp99A;Cyc_Parse_err(((
_tmp99A="bad attributes on enum",_tag_dyneither(_tmp99A,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp9A4;struct Cyc_Absyn_Enum_d_struct _tmp9A3;struct Cyc_Absyn_Enum_d_struct*
_tmp9A2;struct Cyc_List_List*_tmp9A1;struct Cyc_List_List*_tmp1EB=(_tmp9A1=
_cycalloc(sizeof(*_tmp9A1)),((_tmp9A1->hd=((_tmp9A4=_cycalloc(sizeof(*_tmp9A4)),((
_tmp9A4->r=(void*)((_tmp9A2=_cycalloc(sizeof(*_tmp9A2)),((_tmp9A2[0]=((_tmp9A3.tag=
8,((_tmp9A3.f1=_tmp1E5,_tmp9A3)))),_tmp9A2)))),((_tmp9A4->loc=loc,_tmp9A4)))))),((
_tmp9A1->tl=0,_tmp9A1)))));_npop_handler(0);return _tmp1EB;};}_LLCD: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1D1=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp1BC;if(_tmp1D1->tag != 15)goto
_LLCF;else{_tmp1D2=_tmp1D1->f1;}}_LLCE: {struct Cyc_Core_Opt*_tmp9A7;struct Cyc_Absyn_Enumdecl*
_tmp9A6;struct Cyc_Absyn_Enumdecl*_tmp1ED=(_tmp9A6=_cycalloc(sizeof(*_tmp9A6)),((
_tmp9A6->sc=s,((_tmp9A6->name=Cyc_Parse_gensym_enum(),((_tmp9A6->fields=((
_tmp9A7=_cycalloc(sizeof(*_tmp9A7)),((_tmp9A7->v=_tmp1D2,_tmp9A7)))),_tmp9A6)))))));
if(_tmp1A4 != 0){const char*_tmp9A8;Cyc_Parse_err(((_tmp9A8="bad attributes on enum",
_tag_dyneither(_tmp9A8,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp9B2;
struct Cyc_Absyn_Enum_d_struct _tmp9B1;struct Cyc_Absyn_Enum_d_struct*_tmp9B0;
struct Cyc_List_List*_tmp9AF;struct Cyc_List_List*_tmp1F3=(_tmp9AF=_cycalloc(
sizeof(*_tmp9AF)),((_tmp9AF->hd=((_tmp9B2=_cycalloc(sizeof(*_tmp9B2)),((_tmp9B2->r=(
void*)((_tmp9B0=_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=8,((
_tmp9B1.f1=_tmp1ED,_tmp9B1)))),_tmp9B0)))),((_tmp9B2->loc=loc,_tmp9B2)))))),((
_tmp9AF->tl=0,_tmp9AF)))));_npop_handler(0);return _tmp1F3;};}_LLCF:;_LLD0:{const
char*_tmp9B3;Cyc_Parse_err(((_tmp9B3="missing declarator",_tag_dyneither(_tmp9B3,
sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1F7=0;_npop_handler(0);return
_tmp1F7;};_LLC4:;}}else{void*t=_tmp1AB.f1;struct Cyc_List_List*_tmp1F8=Cyc_Parse_apply_tmss(
mkrgn,_tmp1A2,t,_tmp1A9,_tmp1A4);if(istypedef){if(!exps_empty){const char*_tmp9B4;
Cyc_Parse_err(((_tmp9B4="initializer in typedef declaration",_tag_dyneither(
_tmp9B4,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple13*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp1F8);if(_tmp1AB.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp1AB.f2)->v;{void*_tmp1FA=d->r;struct Cyc_Absyn_Aggrdecl*_tmp1FC;struct Cyc_Absyn_Datatypedecl*
_tmp1FE;struct Cyc_Absyn_Enumdecl*_tmp200;_LLD2: {struct Cyc_Absyn_Aggr_d_struct*
_tmp1FB=(struct Cyc_Absyn_Aggr_d_struct*)_tmp1FA;if(_tmp1FB->tag != 6)goto _LLD4;
else{_tmp1FC=_tmp1FB->f1;}}_LLD3: _tmp1FC->sc=s;_tmp1FC->attributes=_tmp1A4;
_tmp1A4=0;goto _LLD1;_LLD4: {struct Cyc_Absyn_Datatype_d_struct*_tmp1FD=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp1FA;if(_tmp1FD->tag != 7)goto _LLD6;else{_tmp1FE=_tmp1FD->f1;}}_LLD5: _tmp1FE->sc=
s;goto _LLD1;_LLD6: {struct Cyc_Absyn_Enum_d_struct*_tmp1FF=(struct Cyc_Absyn_Enum_d_struct*)
_tmp1FA;if(_tmp1FF->tag != 8)goto _LLD8;else{_tmp200=_tmp1FF->f1;}}_LLD7: _tmp200->sc=
s;goto _LLD1;_LLD8:;_LLD9:{const char*_tmp9B5;Cyc_Parse_err(((_tmp9B5="declaration within typedef is not a struct, union, or datatype",
_tag_dyneither(_tmp9B5,sizeof(char),63))),loc);}goto _LLD1;_LLD1:;}{struct Cyc_List_List*
_tmp9B6;decls=((_tmp9B6=_cycalloc(sizeof(*_tmp9B6)),((_tmp9B6->hd=d,((_tmp9B6->tl=
decls,_tmp9B6))))));};}{struct Cyc_List_List*_tmp203=decls;_npop_handler(0);
return _tmp203;};};}else{if(_tmp1AB.f2 != 0){const char*_tmp9B7;Cyc_Parse_unimp(((
_tmp9B7="nested type declaration within declarator",_tag_dyneither(_tmp9B7,
sizeof(char),42))),loc);}{struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp205=_tmp1F8;for(0;_tmp205 != 0;(_tmp205=_tmp205->tl,_tmp1AA=_tmp1AA->tl)){
struct _tuple13 _tmp207;struct _tuple1*_tmp208;struct Cyc_Absyn_Tqual _tmp209;void*
_tmp20A;struct Cyc_List_List*_tmp20B;struct Cyc_List_List*_tmp20C;struct _tuple13*
_tmp206=(struct _tuple13*)_tmp205->hd;_tmp207=*_tmp206;_tmp208=_tmp207.f1;_tmp209=
_tmp207.f2;_tmp20A=_tmp207.f3;_tmp20B=_tmp207.f4;_tmp20C=_tmp207.f5;if(_tmp20B != 
0){const char*_tmp9BA;void*_tmp9B9;(_tmp9B9=0,Cyc_Tcutil_warn(loc,((_tmp9BA="bad type params, ignoring",
_tag_dyneither(_tmp9BA,sizeof(char),26))),_tag_dyneither(_tmp9B9,sizeof(void*),0)));}
if(_tmp1AA == 0){const char*_tmp9BD;void*_tmp9BC;(_tmp9BC=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp9BD="unexpected NULL in parse!",_tag_dyneither(_tmp9BD,sizeof(char),26))),
_tag_dyneither(_tmp9BC,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*_tmp211=(struct
Cyc_Absyn_Exp*)_tmp1AA->hd;struct Cyc_Absyn_Vardecl*_tmp212=Cyc_Absyn_new_vardecl(
_tmp208,_tmp20A,_tmp211);_tmp212->tq=_tmp209;_tmp212->sc=s;_tmp212->attributes=
_tmp20C;{struct Cyc_Absyn_Var_d_struct*_tmp9C3;struct Cyc_Absyn_Var_d_struct
_tmp9C2;struct Cyc_Absyn_Decl*_tmp9C1;struct Cyc_Absyn_Decl*_tmp213=(_tmp9C1=
_cycalloc(sizeof(*_tmp9C1)),((_tmp9C1->r=(void*)((_tmp9C3=_cycalloc(sizeof(*
_tmp9C3)),((_tmp9C3[0]=((_tmp9C2.tag=0,((_tmp9C2.f1=_tmp212,_tmp9C2)))),_tmp9C3)))),((
_tmp9C1->loc=loc,_tmp9C1)))));struct Cyc_List_List*_tmp9C4;decls=((_tmp9C4=
_cycalloc(sizeof(*_tmp9C4)),((_tmp9C4->hd=_tmp213,((_tmp9C4->tl=decls,_tmp9C4))))));};};}}{
struct Cyc_List_List*_tmp218=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
decls);_npop_handler(0);return _tmp218;};};}}};};};};};_pop_region(mkrgn);}static
struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,struct Cyc_Position_Segment*
loc){if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct
_dyneither_ptr)s)== 2)switch(*((const char*)_check_dyneither_subscript(s,sizeof(
char),0))){case 'A': _LLDA: return& Cyc_Tcutil_ak;case 'M': _LLDB: return& Cyc_Tcutil_mk;
case 'B': _LLDC: return& Cyc_Tcutil_bk;case 'R': _LLDD: return& Cyc_Tcutil_rk;case 'E':
_LLDE: return& Cyc_Tcutil_ek;case 'I': _LLDF: return& Cyc_Tcutil_ik;case 'U': _LLE0:
switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))){case 'R':
_LLE2: return& Cyc_Tcutil_urk;case 'A': _LLE3: return& Cyc_Tcutil_uak;case 'M': _LLE4:
return& Cyc_Tcutil_umk;case 'B': _LLE5: return& Cyc_Tcutil_ubk;default: _LLE6: break;}
break;case 'T': _LLE1: switch(*((const char*)_check_dyneither_subscript(s,sizeof(
char),1))){case 'R': _LLE9: return& Cyc_Tcutil_trk;case 'A': _LLEA: return& Cyc_Tcutil_tak;
case 'M': _LLEB: return& Cyc_Tcutil_tmk;case 'B': _LLEC: return& Cyc_Tcutil_tbk;default:
_LLED: break;}break;default: _LLE8: break;}{const char*_tmp9C9;void*_tmp9C8[2];
struct Cyc_String_pa_struct _tmp9C7;struct Cyc_Int_pa_struct _tmp9C6;Cyc_Parse_err((
struct _dyneither_ptr)((_tmp9C6.tag=1,((_tmp9C6.f1=(unsigned long)((int)Cyc_strlen((
struct _dyneither_ptr)s)),((_tmp9C7.tag=0,((_tmp9C7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s),((_tmp9C8[0]=& _tmp9C7,((_tmp9C8[1]=& _tmp9C6,Cyc_aprintf(((
_tmp9C9="bad kind: %s; strlen=%d",_tag_dyneither(_tmp9C9,sizeof(char),24))),
_tag_dyneither(_tmp9C8,sizeof(void*),2)))))))))))))),loc);}return& Cyc_Tcutil_bk;}
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*_tmp21D=
p->r;struct _tuple1*_tmp21F;struct Cyc_Absyn_Vardecl*_tmp221;struct Cyc_Absyn_Pat*
_tmp222;struct Cyc_Absyn_Pat _tmp223;void*_tmp224;struct Cyc_Absyn_Pat*_tmp227;
enum Cyc_Absyn_Sign _tmp22A;int _tmp22B;char _tmp22D;struct _dyneither_ptr _tmp22F;
struct _tuple1*_tmp231;struct Cyc_List_List*_tmp232;int _tmp233;struct Cyc_Absyn_Exp*
_tmp235;_LLF1: {struct Cyc_Absyn_UnknownId_p_struct*_tmp21E=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp21D;if(_tmp21E->tag != 14)goto _LLF3;else{_tmp21F=_tmp21E->f1;}}_LLF2: {struct
Cyc_Absyn_UnknownId_e_struct _tmp9CC;struct Cyc_Absyn_UnknownId_e_struct*_tmp9CB;
return Cyc_Absyn_new_exp((void*)((_tmp9CB=_cycalloc(sizeof(*_tmp9CB)),((_tmp9CB[0]=((
_tmp9CC.tag=2,((_tmp9CC.f1=_tmp21F,_tmp9CC)))),_tmp9CB)))),p->loc);}_LLF3: {
struct Cyc_Absyn_Reference_p_struct*_tmp220=(struct Cyc_Absyn_Reference_p_struct*)
_tmp21D;if(_tmp220->tag != 2)goto _LLF5;else{_tmp221=_tmp220->f1;_tmp222=_tmp220->f2;
_tmp223=*_tmp222;_tmp224=_tmp223.r;{struct Cyc_Absyn_Wild_p_struct*_tmp225=(
struct Cyc_Absyn_Wild_p_struct*)_tmp224;if(_tmp225->tag != 0)goto _LLF5;};}}_LLF4: {
struct Cyc_Absyn_UnknownId_e_struct _tmp9CF;struct Cyc_Absyn_UnknownId_e_struct*
_tmp9CE;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmp9CE=_cycalloc(
sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9CF.tag=2,((_tmp9CF.f1=_tmp221->name,
_tmp9CF)))),_tmp9CE)))),p->loc),p->loc);}_LLF5: {struct Cyc_Absyn_Pointer_p_struct*
_tmp226=(struct Cyc_Absyn_Pointer_p_struct*)_tmp21D;if(_tmp226->tag != 5)goto _LLF7;
else{_tmp227=_tmp226->f1;}}_LLF6: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(
_tmp227),p->loc);_LLF7: {struct Cyc_Absyn_Null_p_struct*_tmp228=(struct Cyc_Absyn_Null_p_struct*)
_tmp21D;if(_tmp228->tag != 8)goto _LLF9;}_LLF8: return Cyc_Absyn_null_exp(p->loc);
_LLF9: {struct Cyc_Absyn_Int_p_struct*_tmp229=(struct Cyc_Absyn_Int_p_struct*)
_tmp21D;if(_tmp229->tag != 9)goto _LLFB;else{_tmp22A=_tmp229->f1;_tmp22B=_tmp229->f2;}}
_LLFA: return Cyc_Absyn_int_exp(_tmp22A,_tmp22B,p->loc);_LLFB: {struct Cyc_Absyn_Char_p_struct*
_tmp22C=(struct Cyc_Absyn_Char_p_struct*)_tmp21D;if(_tmp22C->tag != 10)goto _LLFD;
else{_tmp22D=_tmp22C->f1;}}_LLFC: return Cyc_Absyn_char_exp(_tmp22D,p->loc);_LLFD: {
struct Cyc_Absyn_Float_p_struct*_tmp22E=(struct Cyc_Absyn_Float_p_struct*)_tmp21D;
if(_tmp22E->tag != 11)goto _LLFF;else{_tmp22F=_tmp22E->f1;}}_LLFE: return Cyc_Absyn_float_exp(
_tmp22F,p->loc);_LLFF: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp230=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp21D;if(_tmp230->tag != 15)goto _LL101;else{_tmp231=_tmp230->f1;_tmp232=_tmp230->f2;
_tmp233=_tmp230->f3;if(_tmp233 != 0)goto _LL101;}}_LL100: {struct Cyc_Absyn_UnknownId_e_struct
_tmp9D2;struct Cyc_Absyn_UnknownId_e_struct*_tmp9D1;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmp9D1=_cycalloc(sizeof(*_tmp9D1)),((_tmp9D1[0]=((_tmp9D2.tag=2,((
_tmp9D2.f1=_tmp231,_tmp9D2)))),_tmp9D1)))),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp232);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LL101: {struct Cyc_Absyn_Exp_p_struct*_tmp234=(struct Cyc_Absyn_Exp_p_struct*)
_tmp21D;if(_tmp234->tag != 16)goto _LL103;else{_tmp235=_tmp234->f1;}}_LL102: return
_tmp235;_LL103:;_LL104:{const char*_tmp9D3;Cyc_Parse_err(((_tmp9D3="cannot mix patterns and expressions in case",
_tag_dyneither(_tmp9D3,sizeof(char),44))),p->loc);}return Cyc_Absyn_null_exp(p->loc);
_LLF0:;}union Cyc_YYSTYPE;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple6 val;};
struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{
int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;
struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*
val;};struct _tuple18{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{
int tag;struct _tuple18 val;};struct _tuple19{struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*
f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple19*
val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
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
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};
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
_union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp23E[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp23E,
_tmp23E,_tmp23E + 14}};struct _tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){struct
_tuple6 yyzzz;{union Cyc_YYSTYPE _tmp23F=yy1;struct _tuple6 _tmp240;_LL106: if((
_tmp23F.Int_tok).tag != 1)goto _LL108;_tmp240=(struct _tuple6)(_tmp23F.Int_tok).val;
_LL107: yyzzz=_tmp240;goto _LL105;_LL108:;_LL109:(int)_throw((void*)& Cyc_yyfail_Int_tok);
_LL105:;}return yyzzz;}union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){union Cyc_YYSTYPE
_tmp9D4;return((_tmp9D4.Int_tok).val=yy1,(((_tmp9D4.Int_tok).tag=1,_tmp9D4)));}
static char _tmp243[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE
yy1){char yyzzz;{union Cyc_YYSTYPE _tmp244=yy1;char _tmp245;_LL10B: if((_tmp244.Char_tok).tag
!= 2)goto _LL10D;_tmp245=(char)(_tmp244.Char_tok).val;_LL10C: yyzzz=_tmp245;goto
_LL10A;_LL10D:;_LL10E:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LL10A:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9D5;return((
_tmp9D5.Char_tok).val=yy1,(((_tmp9D5.Char_tok).tag=2,_tmp9D5)));}static char
_tmp248[13]="string_t<`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp248,_tmp248,_tmp248 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(
union Cyc_YYSTYPE yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp249=yy1;
struct _dyneither_ptr _tmp24A;_LL110: if((_tmp249.String_tok).tag != 3)goto _LL112;
_tmp24A=(struct _dyneither_ptr)(_tmp249.String_tok).val;_LL111: yyzzz=_tmp24A;goto
_LL10F;_LL112:;_LL113:(int)_throw((void*)& Cyc_yyfail_String_tok);_LL10F:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE
_tmp9D6;return((_tmp9D6.String_tok).val=yy1,(((_tmp9D6.String_tok).tag=3,_tmp9D6)));}
static char _tmp24D[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp24D,
_tmp24D,_tmp24D + 56}};struct _tuple19*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct
_tuple19*yyzzz;{union Cyc_YYSTYPE _tmp24E=yy1;struct _tuple19*_tmp24F;_LL115: if((
_tmp24E.YY1).tag != 7)goto _LL117;_tmp24F=(struct _tuple19*)(_tmp24E.YY1).val;
_LL116: yyzzz=_tmp24F;goto _LL114;_LL117:;_LL118:(int)_throw((void*)& Cyc_yyfail_YY1);
_LL114:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY1(struct _tuple19*yy1){union Cyc_YYSTYPE
_tmp9D7;return((_tmp9D7.YY1).val=yy1,(((_tmp9D7.YY1).tag=7,_tmp9D7)));}static
char _tmp252[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={
Cyc_Core_Failure,{_tmp252,_tmp252,_tmp252 + 19}};union Cyc_Absyn_Constraint*Cyc_yyget_YY2(
union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp253=
yy1;union Cyc_Absyn_Constraint*_tmp254;_LL11A: if((_tmp253.YY2).tag != 8)goto _LL11C;
_tmp254=(union Cyc_Absyn_Constraint*)(_tmp253.YY2).val;_LL11B: yyzzz=_tmp254;goto
_LL119;_LL11C:;_LL11D:(int)_throw((void*)& Cyc_yyfail_YY2);_LL119:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9D8;
return((_tmp9D8.YY2).val=yy1,(((_tmp9D8.YY2).tag=8,_tmp9D8)));}static char _tmp257[
6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{
_tmp257,_tmp257,_tmp257 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE _tmp258=yy1;struct Cyc_Absyn_Exp*
_tmp259;_LL11F: if((_tmp258.YY3).tag != 9)goto _LL121;_tmp259=(struct Cyc_Absyn_Exp*)(
_tmp258.YY3).val;_LL120: yyzzz=_tmp259;goto _LL11E;_LL121:;_LL122:(int)_throw((
void*)& Cyc_yyfail_YY3);_LL11E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY3(struct Cyc_Absyn_Exp*
yy1){union Cyc_YYSTYPE _tmp9D9;return((_tmp9D9.YY3).val=yy1,(((_tmp9D9.YY3).tag=9,
_tmp9D9)));}static char _tmp25C[10]="exp_opt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp25C,_tmp25C,_tmp25C + 10}};struct Cyc_Absyn_Exp*
Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE
_tmp25D=yy1;struct Cyc_Absyn_Exp*_tmp25E;_LL124: if((_tmp25D.YY4).tag != 10)goto
_LL126;_tmp25E=(struct Cyc_Absyn_Exp*)(_tmp25D.YY4).val;_LL125: yyzzz=_tmp25E;goto
_LL123;_LL126:;_LL127:(int)_throw((void*)& Cyc_yyfail_YY4);_LL123:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9DA;return((
_tmp9DA.YY4).val=yy1,(((_tmp9DA.YY4).tag=10,_tmp9DA)));}static char _tmp261[17]="list_t<exp_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp261,
_tmp261,_tmp261 + 17}};struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp262=yy1;struct Cyc_List_List*
_tmp263;_LL129: if((_tmp262.YY5).tag != 11)goto _LL12B;_tmp263=(struct Cyc_List_List*)(
_tmp262.YY5).val;_LL12A: yyzzz=_tmp263;goto _LL128;_LL12B:;_LL12C:(int)_throw((
void*)& Cyc_yyfail_YY5);_LL128:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9DB;return((_tmp9DB.YY5).val=yy1,(((_tmp9DB.YY5).tag=11,
_tmp9DB)));}static char _tmp266[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp266,
_tmp266,_tmp266 + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp267=yy1;struct Cyc_List_List*
_tmp268;_LL12E: if((_tmp267.YY6).tag != 12)goto _LL130;_tmp268=(struct Cyc_List_List*)(
_tmp267.YY6).val;_LL12F: yyzzz=_tmp268;goto _LL12D;_LL130:;_LL131:(int)_throw((
void*)& Cyc_yyfail_YY6);_LL12D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9DC;return((_tmp9DC.YY6).val=yy1,(((_tmp9DC.YY6).tag=12,
_tmp9DC)));}static char _tmp26B[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp26B,_tmp26B,_tmp26B + 9}};enum Cyc_Absyn_Primop
Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Primop yyzzz;{union Cyc_YYSTYPE
_tmp26C=yy1;enum Cyc_Absyn_Primop _tmp26D;_LL133: if((_tmp26C.YY7).tag != 13)goto
_LL135;_tmp26D=(enum Cyc_Absyn_Primop)(_tmp26C.YY7).val;_LL134: yyzzz=_tmp26D;
goto _LL132;_LL135:;_LL136:(int)_throw((void*)& Cyc_yyfail_YY7);_LL132:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9DD;
return((_tmp9DD.YY7).val=yy1,(((_tmp9DD.YY7).tag=13,_tmp9DD)));}static char
_tmp270[19]="opt_t<primop_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={
Cyc_Core_Failure,{_tmp270,_tmp270,_tmp270 + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(
union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp271=yy1;
struct Cyc_Core_Opt*_tmp272;_LL138: if((_tmp271.YY8).tag != 14)goto _LL13A;_tmp272=(
struct Cyc_Core_Opt*)(_tmp271.YY8).val;_LL139: yyzzz=_tmp272;goto _LL137;_LL13A:;
_LL13B:(int)_throw((void*)& Cyc_yyfail_YY8);_LL137:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY8(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9DE;return((_tmp9DE.YY8).val=
yy1,(((_tmp9DE.YY8).tag=14,_tmp9DE)));}static char _tmp275[7]="qvar_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp275,
_tmp275,_tmp275 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp276=yy1;struct _tuple1*_tmp277;_LL13D:
if((_tmp276.QualId_tok).tag != 5)goto _LL13F;_tmp277=(struct _tuple1*)(_tmp276.QualId_tok).val;
_LL13E: yyzzz=_tmp277;goto _LL13C;_LL13F:;_LL140:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL13C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple1*yy1){union Cyc_YYSTYPE
_tmp9DF;return((_tmp9DF.QualId_tok).val=yy1,(((_tmp9DF.QualId_tok).tag=5,_tmp9DF)));}
static char _tmp27A[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY9={
Cyc_Core_Failure,{_tmp27A,_tmp27A,_tmp27A + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE _tmp27B=yy1;
struct Cyc_Absyn_Stmt*_tmp27C;_LL142: if((_tmp27B.YY9).tag != 15)goto _LL144;_tmp27C=(
struct Cyc_Absyn_Stmt*)(_tmp27B.YY9).val;_LL143: yyzzz=_tmp27C;goto _LL141;_LL144:;
_LL145:(int)_throw((void*)& Cyc_yyfail_YY9);_LL141:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9E0;return((_tmp9E0.YY9).val=
yy1,(((_tmp9E0.YY9).tag=15,_tmp9E0)));}static char _tmp27F[27]="list_t<switch_clause_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp27F,
_tmp27F,_tmp27F + 27}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp280=yy1;struct Cyc_List_List*
_tmp281;_LL147: if((_tmp280.YY10).tag != 16)goto _LL149;_tmp281=(struct Cyc_List_List*)(
_tmp280.YY10).val;_LL148: yyzzz=_tmp281;goto _LL146;_LL149:;_LL14A:(int)_throw((
void*)& Cyc_yyfail_YY10);_LL146:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9E1;return((_tmp9E1.YY10).val=yy1,(((_tmp9E1.YY10).tag=
16,_tmp9E1)));}static char _tmp284[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp284,_tmp284,_tmp284 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE
_tmp285=yy1;struct Cyc_Absyn_Pat*_tmp286;_LL14C: if((_tmp285.YY11).tag != 17)goto
_LL14E;_tmp286=(struct Cyc_Absyn_Pat*)(_tmp285.YY11).val;_LL14D: yyzzz=_tmp286;
goto _LL14B;_LL14E:;_LL14F:(int)_throw((void*)& Cyc_yyfail_YY11);_LL14B:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9E2;
return((_tmp9E2.YY11).val=yy1,(((_tmp9E2.YY11).tag=17,_tmp9E2)));}static char
_tmp289[28]="$(list_t<pat_t,`H>,bool)@`H";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={
Cyc_Core_Failure,{_tmp289,_tmp289,_tmp289 + 28}};struct _tuple20*Cyc_yyget_YY12(
union Cyc_YYSTYPE yy1){struct _tuple20*yyzzz;{union Cyc_YYSTYPE _tmp28A=yy1;struct
_tuple20*_tmp28B;_LL151: if((_tmp28A.YY12).tag != 18)goto _LL153;_tmp28B=(struct
_tuple20*)(_tmp28A.YY12).val;_LL152: yyzzz=_tmp28B;goto _LL150;_LL153:;_LL154:(int)
_throw((void*)& Cyc_yyfail_YY12);_LL150:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY12(
struct _tuple20*yy1){union Cyc_YYSTYPE _tmp9E3;return((_tmp9E3.YY12).val=yy1,(((
_tmp9E3.YY12).tag=18,_tmp9E3)));}static char _tmp28E[17]="list_t<pat_t,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp28E,_tmp28E,
_tmp28E + 17}};struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp28F=yy1;struct Cyc_List_List*_tmp290;_LL156: if((
_tmp28F.YY13).tag != 19)goto _LL158;_tmp290=(struct Cyc_List_List*)(_tmp28F.YY13).val;
_LL157: yyzzz=_tmp290;goto _LL155;_LL158:;_LL159:(int)_throw((void*)& Cyc_yyfail_YY13);
_LL155:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9E4;return((_tmp9E4.YY13).val=yy1,(((_tmp9E4.YY13).tag=19,_tmp9E4)));}static
char _tmp293[36]="$(list_t<designator_t,`H>,pat_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp293,_tmp293,_tmp293 + 36}};struct _tuple21*
Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE
_tmp294=yy1;struct _tuple21*_tmp295;_LL15B: if((_tmp294.YY14).tag != 20)goto _LL15D;
_tmp295=(struct _tuple21*)(_tmp294.YY14).val;_LL15C: yyzzz=_tmp295;goto _LL15A;
_LL15D:;_LL15E:(int)_throw((void*)& Cyc_yyfail_YY14);_LL15A:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY14(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9E5;return((_tmp9E5.YY14).val=
yy1,(((_tmp9E5.YY14).tag=20,_tmp9E5)));}static char _tmp298[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp298,
_tmp298,_tmp298 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp299=yy1;struct Cyc_List_List*
_tmp29A;_LL160: if((_tmp299.YY15).tag != 21)goto _LL162;_tmp29A=(struct Cyc_List_List*)(
_tmp299.YY15).val;_LL161: yyzzz=_tmp29A;goto _LL15F;_LL162:;_LL163:(int)_throw((
void*)& Cyc_yyfail_YY15);_LL15F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9E6;return((_tmp9E6.YY15).val=yy1,(((_tmp9E6.YY15).tag=
21,_tmp9E6)));}static char _tmp29D[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp29D,
_tmp29D,_tmp29D + 58}};struct _tuple20*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct
_tuple20*yyzzz;{union Cyc_YYSTYPE _tmp29E=yy1;struct _tuple20*_tmp29F;_LL165: if((
_tmp29E.YY16).tag != 22)goto _LL167;_tmp29F=(struct _tuple20*)(_tmp29E.YY16).val;
_LL166: yyzzz=_tmp29F;goto _LL164;_LL167:;_LL168:(int)_throw((void*)& Cyc_yyfail_YY16);
_LL164:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY16(struct _tuple20*yy1){union Cyc_YYSTYPE
_tmp9E7;return((_tmp9E7.YY16).val=yy1,(((_tmp9E7.YY16).tag=22,_tmp9E7)));}static
char _tmp2A2[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp2A2,_tmp2A2,_tmp2A2 + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp2A3=yy1;
struct Cyc_Absyn_Fndecl*_tmp2A4;_LL16A: if((_tmp2A3.YY17).tag != 23)goto _LL16C;
_tmp2A4=(struct Cyc_Absyn_Fndecl*)(_tmp2A3.YY17).val;_LL16B: yyzzz=_tmp2A4;goto
_LL169;_LL16C:;_LL16D:(int)_throw((void*)& Cyc_yyfail_YY17);_LL169:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9E8;
return((_tmp9E8.YY17).val=yy1,(((_tmp9E8.YY17).tag=23,_tmp9E8)));}static char
_tmp2A7[18]="list_t<decl_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp2A7,_tmp2A7,_tmp2A7 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2A8=yy1;
struct Cyc_List_List*_tmp2A9;_LL16F: if((_tmp2A8.YY18).tag != 24)goto _LL171;_tmp2A9=(
struct Cyc_List_List*)(_tmp2A8.YY18).val;_LL170: yyzzz=_tmp2A9;goto _LL16E;_LL171:;
_LL172:(int)_throw((void*)& Cyc_yyfail_YY18);_LL16E:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E9;return((_tmp9E9.YY18).val=
yy1,(((_tmp9E9.YY18).tag=24,_tmp9E9)));}static char _tmp2AC[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp2AC,
_tmp2AC,_tmp2AC + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp2AD=yy1;struct
Cyc_Parse_Declaration_spec*_tmp2AE;_LL174: if((_tmp2AD.YY19).tag != 25)goto _LL176;
_tmp2AE=(struct Cyc_Parse_Declaration_spec*)(_tmp2AD.YY19).val;_LL175: yyzzz=
_tmp2AE;goto _LL173;_LL176:;_LL177:(int)_throw((void*)& Cyc_yyfail_YY19);_LL173:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){
union Cyc_YYSTYPE _tmp9EA;return((_tmp9EA.YY19).val=yy1,(((_tmp9EA.YY19).tag=25,
_tmp9EA)));}static char _tmp2B1[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp2B1,_tmp2B1,_tmp2B1 + 29}};struct _tuple22*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){struct _tuple22*yyzzz;{union Cyc_YYSTYPE
_tmp2B2=yy1;struct _tuple22*_tmp2B3;_LL179: if((_tmp2B2.YY20).tag != 26)goto _LL17B;
_tmp2B3=(struct _tuple22*)(_tmp2B2.YY20).val;_LL17A: yyzzz=_tmp2B3;goto _LL178;
_LL17B:;_LL17C:(int)_throw((void*)& Cyc_yyfail_YY20);_LL178:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY20(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9EB;return((_tmp9EB.YY20).val=
yy1,(((_tmp9EB.YY20).tag=26,_tmp9EB)));}static char _tmp2B6[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp2B6,
_tmp2B6,_tmp2B6 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2B7=yy1;struct Cyc_List_List*
_tmp2B8;_LL17E: if((_tmp2B7.YY21).tag != 27)goto _LL180;_tmp2B8=(struct Cyc_List_List*)(
_tmp2B7.YY21).val;_LL17F: yyzzz=_tmp2B8;goto _LL17D;_LL180:;_LL181:(int)_throw((
void*)& Cyc_yyfail_YY21);_LL17D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9EC;return((_tmp9EC.YY21).val=yy1,(((_tmp9EC.YY21).tag=
27,_tmp9EC)));}static char _tmp2BB[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp2BB,_tmp2BB,_tmp2BB + 16}};enum Cyc_Parse_Storage_class
Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){enum Cyc_Parse_Storage_class yyzzz;{union Cyc_YYSTYPE
_tmp2BC=yy1;enum Cyc_Parse_Storage_class _tmp2BD;_LL183: if((_tmp2BC.YY22).tag != 
28)goto _LL185;_tmp2BD=(enum Cyc_Parse_Storage_class)(_tmp2BC.YY22).val;_LL184:
yyzzz=_tmp2BD;goto _LL182;_LL185:;_LL186:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL182:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class yy1){
union Cyc_YYSTYPE _tmp9ED;return((_tmp9ED.YY22).val=yy1,(((_tmp9ED.YY22).tag=28,
_tmp9ED)));}static char _tmp2C0[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp2C0,_tmp2C0,_tmp2C0 + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2C1=yy1;void*_tmp2C2;_LL188:
if((_tmp2C1.YY23).tag != 29)goto _LL18A;_tmp2C2=(void*)(_tmp2C1.YY23).val;_LL189:
yyzzz=_tmp2C2;goto _LL187;_LL18A:;_LL18B:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL187:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(void*yy1){union Cyc_YYSTYPE _tmp9EE;
return((_tmp9EE.YY23).val=yy1,(((_tmp9EE.YY23).tag=29,_tmp9EE)));}static char
_tmp2C5[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={
Cyc_Core_Failure,{_tmp2C5,_tmp2C5,_tmp2C5 + 12}};enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(
union Cyc_YYSTYPE yy1){enum Cyc_Absyn_AggrKind yyzzz;{union Cyc_YYSTYPE _tmp2C6=yy1;
enum Cyc_Absyn_AggrKind _tmp2C7;_LL18D: if((_tmp2C6.YY24).tag != 30)goto _LL18F;
_tmp2C7=(enum Cyc_Absyn_AggrKind)(_tmp2C6.YY24).val;_LL18E: yyzzz=_tmp2C7;goto
_LL18C;_LL18F:;_LL190:(int)_throw((void*)& Cyc_yyfail_YY24);_LL18C:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9EF;
return((_tmp9EF.YY24).val=yy1,(((_tmp9EF.YY24).tag=30,_tmp9EF)));}static char
_tmp2CA[8]="tqual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{
_tmp2CA,_tmp2CA,_tmp2CA + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE _tmp2CB=yy1;struct Cyc_Absyn_Tqual
_tmp2CC;_LL192: if((_tmp2CB.YY25).tag != 31)goto _LL194;_tmp2CC=(struct Cyc_Absyn_Tqual)(
_tmp2CB.YY25).val;_LL193: yyzzz=_tmp2CC;goto _LL191;_LL194:;_LL195:(int)_throw((
void*)& Cyc_yyfail_YY25);_LL191:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual
yy1){union Cyc_YYSTYPE _tmp9F0;return((_tmp9F0.YY25).val=yy1,(((_tmp9F0.YY25).tag=
31,_tmp9F0)));}static char _tmp2CF[23]="list_t<aggrfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp2CF,_tmp2CF,_tmp2CF + 23}};struct Cyc_List_List*
Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2D0=yy1;struct Cyc_List_List*_tmp2D1;_LL197: if((_tmp2D0.YY26).tag != 32)goto
_LL199;_tmp2D1=(struct Cyc_List_List*)(_tmp2D0.YY26).val;_LL198: yyzzz=_tmp2D1;
goto _LL196;_LL199:;_LL19A:(int)_throw((void*)& Cyc_yyfail_YY26);_LL196:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F1;
return((_tmp9F1.YY26).val=yy1,(((_tmp9F1.YY26).tag=32,_tmp9F1)));}static char
_tmp2D4[34]="list_t<list_t<aggrfield_t,`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2D4,_tmp2D4,_tmp2D4 + 34}};struct Cyc_List_List*
Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2D5=yy1;struct Cyc_List_List*_tmp2D6;_LL19C: if((_tmp2D5.YY27).tag != 33)goto
_LL19E;_tmp2D6=(struct Cyc_List_List*)(_tmp2D5.YY27).val;_LL19D: yyzzz=_tmp2D6;
goto _LL19B;_LL19E:;_LL19F:(int)_throw((void*)& Cyc_yyfail_YY27);_LL19B:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F2;
return((_tmp9F2.YY27).val=yy1,(((_tmp9F2.YY27).tag=33,_tmp9F2)));}static char
_tmp2D9[31]="list_t<type_modifier_t<`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2D9,_tmp2D9,_tmp2D9 + 31}};struct Cyc_List_List*
Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2DA=yy1;struct Cyc_List_List*_tmp2DB;_LL1A1: if((_tmp2DA.YY28).tag != 34)goto
_LL1A3;_tmp2DB=(struct Cyc_List_List*)(_tmp2DA.YY28).val;_LL1A2: yyzzz=_tmp2DB;
goto _LL1A0;_LL1A3:;_LL1A4:(int)_throw((void*)& Cyc_yyfail_YY28);_LL1A0:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F3;
return((_tmp9F3.YY28).val=yy1,(((_tmp9F3.YY28).tag=34,_tmp9F3)));}static char
_tmp2DE[13]="declarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={
Cyc_Core_Failure,{_tmp2DE,_tmp2DE,_tmp2DE + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE _tmp2DF=
yy1;struct Cyc_Parse_Declarator*_tmp2E0;_LL1A6: if((_tmp2DF.YY29).tag != 35)goto
_LL1A8;_tmp2E0=(struct Cyc_Parse_Declarator*)(_tmp2DF.YY29).val;_LL1A7: yyzzz=
_tmp2E0;goto _LL1A5;_LL1A8:;_LL1A9:(int)_throw((void*)& Cyc_yyfail_YY29);_LL1A5:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){union Cyc_YYSTYPE
_tmp9F4;return((_tmp9F4.YY29).val=yy1,(((_tmp9F4.YY29).tag=35,_tmp9F4)));}static
char _tmp2E3[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={
Cyc_Core_Failure,{_tmp2E3,_tmp2E3,_tmp2E3 + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{
union Cyc_YYSTYPE _tmp2E4=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp2E5;_LL1AB:
if((_tmp2E4.YY30).tag != 36)goto _LL1AD;_tmp2E5=(struct Cyc_Parse_Abstractdeclarator*)(
_tmp2E4.YY30).val;_LL1AC: yyzzz=_tmp2E5;goto _LL1AA;_LL1AD:;_LL1AE:(int)_throw((
void*)& Cyc_yyfail_YY30);_LL1AA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1){union Cyc_YYSTYPE _tmp9F5;return((_tmp9F5.YY30).val=yy1,(((_tmp9F5.YY30).tag=
36,_tmp9F5)));}static char _tmp2E8[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2E8,_tmp2E8,_tmp2E8 + 5}};int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp2E9=yy1;int _tmp2EA;_LL1B0: if((
_tmp2E9.YY31).tag != 37)goto _LL1B2;_tmp2EA=(int)(_tmp2E9.YY31).val;_LL1B1: yyzzz=
_tmp2EA;goto _LL1AF;_LL1B2:;_LL1B3:(int)_throw((void*)& Cyc_yyfail_YY31);_LL1AF:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp9F6;return((
_tmp9F6.YY31).val=yy1,(((_tmp9F6.YY31).tag=37,_tmp9F6)));}static char _tmp2ED[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2ED,
_tmp2ED,_tmp2ED + 8}};enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;{union Cyc_YYSTYPE _tmp2EE=yy1;enum Cyc_Absyn_Scope
_tmp2EF;_LL1B5: if((_tmp2EE.YY32).tag != 38)goto _LL1B7;_tmp2EF=(enum Cyc_Absyn_Scope)(
_tmp2EE.YY32).val;_LL1B6: yyzzz=_tmp2EF;goto _LL1B4;_LL1B7:;_LL1B8:(int)_throw((
void*)& Cyc_yyfail_YY32);_LL1B4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope
yy1){union Cyc_YYSTYPE _tmp9F7;return((_tmp9F7.YY32).val=yy1,(((_tmp9F7.YY32).tag=
38,_tmp9F7)));}static char _tmp2F2[16]="datatypefield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2F2,_tmp2F2,_tmp2F2 + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union
Cyc_YYSTYPE _tmp2F3=yy1;struct Cyc_Absyn_Datatypefield*_tmp2F4;_LL1BA: if((_tmp2F3.YY33).tag
!= 39)goto _LL1BC;_tmp2F4=(struct Cyc_Absyn_Datatypefield*)(_tmp2F3.YY33).val;
_LL1BB: yyzzz=_tmp2F4;goto _LL1B9;_LL1BC:;_LL1BD:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL1B9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){
union Cyc_YYSTYPE _tmp9F8;return((_tmp9F8.YY33).val=yy1,(((_tmp9F8.YY33).tag=39,
_tmp9F8)));}static char _tmp2F7[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2F7,_tmp2F7,_tmp2F7 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2F8=yy1;struct Cyc_List_List*_tmp2F9;_LL1BF: if((_tmp2F8.YY34).tag != 40)goto
_LL1C1;_tmp2F9=(struct Cyc_List_List*)(_tmp2F8.YY34).val;_LL1C0: yyzzz=_tmp2F9;
goto _LL1BE;_LL1C1:;_LL1C2:(int)_throw((void*)& Cyc_yyfail_YY34);_LL1BE:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F9;
return((_tmp9F9.YY34).val=yy1,(((_tmp9F9.YY34).tag=40,_tmp9F9)));}static char
_tmp2FC[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2FC,_tmp2FC,
_tmp2FC + 55}};struct _tuple23*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple23*
yyzzz;{union Cyc_YYSTYPE _tmp2FD=yy1;struct _tuple23*_tmp2FE;_LL1C4: if((_tmp2FD.YY35).tag
!= 41)goto _LL1C6;_tmp2FE=(struct _tuple23*)(_tmp2FD.YY35).val;_LL1C5: yyzzz=
_tmp2FE;goto _LL1C3;_LL1C6:;_LL1C7:(int)_throw((void*)& Cyc_yyfail_YY35);_LL1C3:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY35(struct _tuple23*yy1){union Cyc_YYSTYPE
_tmp9FA;return((_tmp9FA.YY35).val=yy1,(((_tmp9FA.YY35).tag=41,_tmp9FA)));}static
char _tmp301[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={
Cyc_Core_Failure,{_tmp301,_tmp301,_tmp301 + 17}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp302=yy1;
struct Cyc_List_List*_tmp303;_LL1C9: if((_tmp302.YY36).tag != 42)goto _LL1CB;_tmp303=(
struct Cyc_List_List*)(_tmp302.YY36).val;_LL1CA: yyzzz=_tmp303;goto _LL1C8;_LL1CB:;
_LL1CC:(int)_throw((void*)& Cyc_yyfail_YY36);_LL1C8:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FB;return((_tmp9FB.YY36).val=
yy1,(((_tmp9FB.YY36).tag=42,_tmp9FB)));}static char _tmp306[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp306,
_tmp306,_tmp306 + 37}};struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){struct
_tuple8*yyzzz;{union Cyc_YYSTYPE _tmp307=yy1;struct _tuple8*_tmp308;_LL1CE: if((
_tmp307.YY37).tag != 43)goto _LL1D0;_tmp308=(struct _tuple8*)(_tmp307.YY37).val;
_LL1CF: yyzzz=_tmp308;goto _LL1CD;_LL1D0:;_LL1D1:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL1CD:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE
_tmp9FC;return((_tmp9FC.YY37).val=yy1,(((_tmp9FC.YY37).tag=43,_tmp9FC)));}static
char _tmp30B[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp30B,_tmp30B,
_tmp30B + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp30C=yy1;struct Cyc_List_List*_tmp30D;_LL1D3: if((
_tmp30C.YY38).tag != 44)goto _LL1D5;_tmp30D=(struct Cyc_List_List*)(_tmp30C.YY38).val;
_LL1D4: yyzzz=_tmp30D;goto _LL1D2;_LL1D5:;_LL1D6:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1D2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9FD;return((_tmp9FD.YY38).val=yy1,(((_tmp9FD.YY38).tag=44,_tmp9FD)));}static
char _tmp310[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp310,
_tmp310,_tmp310 + 127}};struct _tuple24*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct
_tuple24*yyzzz;{union Cyc_YYSTYPE _tmp311=yy1;struct _tuple24*_tmp312;_LL1D8: if((
_tmp311.YY39).tag != 45)goto _LL1DA;_tmp312=(struct _tuple24*)(_tmp311.YY39).val;
_LL1D9: yyzzz=_tmp312;goto _LL1D7;_LL1DA:;_LL1DB:(int)_throw((void*)& Cyc_yyfail_YY39);
_LL1D7:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY39(struct _tuple24*yy1){union Cyc_YYSTYPE
_tmp9FE;return((_tmp9FE.YY39).val=yy1,(((_tmp9FE.YY39).tag=45,_tmp9FE)));}static
char _tmp315[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={
Cyc_Core_Failure,{_tmp315,_tmp315,_tmp315 + 18}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp316=yy1;
struct Cyc_List_List*_tmp317;_LL1DD: if((_tmp316.YY40).tag != 46)goto _LL1DF;_tmp317=(
struct Cyc_List_List*)(_tmp316.YY40).val;_LL1DE: yyzzz=_tmp317;goto _LL1DC;_LL1DF:;
_LL1E0:(int)_throw((void*)& Cyc_yyfail_YY40);_LL1DC:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FF;return((_tmp9FF.YY40).val=
yy1,(((_tmp9FF.YY40).tag=46,_tmp9FF)));}static char _tmp31A[24]="list_t<designator_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp31A,
_tmp31A,_tmp31A + 24}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp31B=yy1;struct Cyc_List_List*
_tmp31C;_LL1E2: if((_tmp31B.YY41).tag != 47)goto _LL1E4;_tmp31C=(struct Cyc_List_List*)(
_tmp31B.YY41).val;_LL1E3: yyzzz=_tmp31C;goto _LL1E1;_LL1E4:;_LL1E5:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1E1:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmpA00;return((_tmpA00.YY41).val=yy1,(((_tmpA00.YY41).tag=
47,_tmpA00)));}static char _tmp31F[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp31F,_tmp31F,_tmp31F + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp320=yy1;void*_tmp321;_LL1E7:
if((_tmp320.YY42).tag != 48)goto _LL1E9;_tmp321=(void*)(_tmp320.YY42).val;_LL1E8:
yyzzz=_tmp321;goto _LL1E6;_LL1E9:;_LL1EA:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1E6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmpA01;
return((_tmpA01.YY42).val=yy1,(((_tmpA01.YY42).tag=48,_tmpA01)));}static char
_tmp324[7]="kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{
_tmp324,_tmp324,_tmp324 + 7}};struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Kind*yyzzz;{union Cyc_YYSTYPE _tmp325=yy1;struct Cyc_Absyn_Kind*
_tmp326;_LL1EC: if((_tmp325.YY43).tag != 49)goto _LL1EE;_tmp326=(struct Cyc_Absyn_Kind*)(
_tmp325.YY43).val;_LL1ED: yyzzz=_tmp326;goto _LL1EB;_LL1EE:;_LL1EF:(int)_throw((
void*)& Cyc_yyfail_YY43);_LL1EB:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*
yy1){union Cyc_YYSTYPE _tmpA02;return((_tmpA02.YY43).val=yy1,(((_tmpA02.YY43).tag=
49,_tmpA02)));}static char _tmp329[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp329,_tmp329,_tmp329 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp32A=yy1;void*_tmp32B;_LL1F1:
if((_tmp32A.YY44).tag != 50)goto _LL1F3;_tmp32B=(void*)(_tmp32A.YY44).val;_LL1F2:
yyzzz=_tmp32B;goto _LL1F0;_LL1F3:;_LL1F4:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1F0:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmpA03;
return((_tmpA03.YY44).val=yy1,(((_tmpA03.YY44).tag=50,_tmpA03)));}static char
_tmp32E[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY45={
Cyc_Core_Failure,{_tmp32E,_tmp32E,_tmp32E + 23}};struct Cyc_List_List*Cyc_yyget_YY45(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp32F=yy1;
struct Cyc_List_List*_tmp330;_LL1F6: if((_tmp32F.YY45).tag != 51)goto _LL1F8;_tmp330=(
struct Cyc_List_List*)(_tmp32F.YY45).val;_LL1F7: yyzzz=_tmp330;goto _LL1F5;_LL1F8:;
_LL1F9:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1F5:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA04;return((_tmpA04.YY45).val=
yy1,(((_tmpA04.YY45).tag=51,_tmpA04)));}static char _tmp333[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp333,
_tmp333,_tmp333 + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union
Cyc_YYSTYPE _tmp334=yy1;void*_tmp335;_LL1FB: if((_tmp334.YY46).tag != 52)goto _LL1FD;
_tmp335=(void*)(_tmp334.YY46).val;_LL1FC: yyzzz=_tmp335;goto _LL1FA;_LL1FD:;_LL1FE:(
int)_throw((void*)& Cyc_yyfail_YY46);_LL1FA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1){union Cyc_YYSTYPE _tmpA05;return((_tmpA05.YY46).val=yy1,(((_tmpA05.YY46).tag=
52,_tmpA05)));}static char _tmp338[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp338,_tmp338,_tmp338 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE
_tmp339=yy1;struct Cyc_Absyn_Enumfield*_tmp33A;_LL200: if((_tmp339.YY47).tag != 53)
goto _LL202;_tmp33A=(struct Cyc_Absyn_Enumfield*)(_tmp339.YY47).val;_LL201: yyzzz=
_tmp33A;goto _LL1FF;_LL202:;_LL203:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1FF:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE
_tmpA06;return((_tmpA06.YY47).val=yy1,(((_tmpA06.YY47).tag=53,_tmpA06)));}static
char _tmp33D[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={
Cyc_Core_Failure,{_tmp33D,_tmp33D,_tmp33D + 23}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp33E=yy1;
struct Cyc_List_List*_tmp33F;_LL205: if((_tmp33E.YY48).tag != 54)goto _LL207;_tmp33F=(
struct Cyc_List_List*)(_tmp33E.YY48).val;_LL206: yyzzz=_tmp33F;goto _LL204;_LL207:;
_LL208:(int)_throw((void*)& Cyc_yyfail_YY48);_LL204:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA07;return((_tmpA07.YY48).val=
yy1,(((_tmpA07.YY48).tag=54,_tmpA07)));}static char _tmp342[17]="opt_t<type_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp342,
_tmp342,_tmp342 + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp343=yy1;struct Cyc_Core_Opt*_tmp344;
_LL20A: if((_tmp343.YY49).tag != 55)goto _LL20C;_tmp344=(struct Cyc_Core_Opt*)(
_tmp343.YY49).val;_LL20B: yyzzz=_tmp344;goto _LL209;_LL20C:;_LL20D:(int)_throw((
void*)& Cyc_yyfail_YY49);_LL209:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*
yy1){union Cyc_YYSTYPE _tmpA08;return((_tmpA08.YY49).val=yy1,(((_tmpA08.YY49).tag=
55,_tmpA08)));}static char _tmp347[31]="list_t<$(type_t,type_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp347,_tmp347,
_tmp347 + 31}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp348=yy1;struct Cyc_List_List*_tmp349;_LL20F: if((
_tmp348.YY50).tag != 56)goto _LL211;_tmp349=(struct Cyc_List_List*)(_tmp348.YY50).val;
_LL210: yyzzz=_tmp349;goto _LL20E;_LL211:;_LL212:(int)_throw((void*)& Cyc_yyfail_YY50);
_LL20E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmpA09;return((_tmpA09.YY50).val=yy1,(((_tmpA09.YY50).tag=56,_tmpA09)));}static
char _tmp34C[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY51={
Cyc_Core_Failure,{_tmp34C,_tmp34C,_tmp34C + 15}};union Cyc_Absyn_Constraint*Cyc_yyget_YY51(
union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp34D=
yy1;union Cyc_Absyn_Constraint*_tmp34E;_LL214: if((_tmp34D.YY51).tag != 57)goto
_LL216;_tmp34E=(union Cyc_Absyn_Constraint*)(_tmp34D.YY51).val;_LL215: yyzzz=
_tmp34E;goto _LL213;_LL216:;_LL217:(int)_throw((void*)& Cyc_yyfail_YY51);_LL213:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE
_tmpA0A;return((_tmpA0A.YY51).val=yy1,(((_tmpA0A.YY51).tag=57,_tmpA0A)));}static
char _tmp351[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp351,_tmp351,_tmp351 + 45}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp352=yy1;struct Cyc_List_List*_tmp353;_LL219: if((_tmp352.YY52).tag != 58)goto
_LL21B;_tmp353=(struct Cyc_List_List*)(_tmp352.YY52).val;_LL21A: yyzzz=_tmp353;
goto _LL218;_LL21B:;_LL21C:(int)_throw((void*)& Cyc_yyfail_YY52);_LL218:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA0B;
return((_tmpA0B.YY52).val=yy1,(((_tmpA0B.YY52).tag=58,_tmpA0B)));}static char
_tmp356[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp356,_tmp356,_tmp356 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp357=yy1;void*_tmp358;_LL21E: if((_tmp357.YY53).tag
!= 59)goto _LL220;_tmp358=(void*)(_tmp357.YY53).val;_LL21F: yyzzz=_tmp358;goto
_LL21D;_LL220:;_LL221:(int)_throw((void*)& Cyc_yyfail_YY53);_LL21D:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmpA0C;return((_tmpA0C.YY53).val=
yy1,(((_tmpA0C.YY53).tag=59,_tmpA0C)));}static char _tmp35B[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp35B,
_tmp35B,_tmp35B + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp35C=yy1;struct Cyc_List_List*
_tmp35D;_LL223: if((_tmp35C.YY54).tag != 60)goto _LL225;_tmp35D=(struct Cyc_List_List*)(
_tmp35C.YY54).val;_LL224: yyzzz=_tmp35D;goto _LL222;_LL225:;_LL226:(int)_throw((
void*)& Cyc_yyfail_YY54);_LL222:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmpA0D;return((_tmpA0D.YY54).val=yy1,(((_tmpA0D.YY54).tag=
60,_tmpA0D)));}static char _tmp360[21]="$(bool,string_t<`H>)";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp360,_tmp360,_tmp360 + 21}};struct
_tuple18 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){struct _tuple18 yyzzz;{union Cyc_YYSTYPE
_tmp361=yy1;struct _tuple18 _tmp362;_LL228: if((_tmp361.Asm_tok).tag != 6)goto _LL22A;
_tmp362=(struct _tuple18)(_tmp361.Asm_tok).val;_LL229: yyzzz=_tmp362;goto _LL227;
_LL22A:;_LL22B:(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL227:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple18 yy1){union Cyc_YYSTYPE _tmpA0E;return((
_tmpA0E.Asm_tok).val=yy1,(((_tmpA0E.Asm_tok).tag=6,_tmpA0E)));}struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_Yyltype Cyc_yynewloc(){struct Cyc_Yyltype _tmpA0F;return(_tmpA0F.timestamp=0,((
_tmpA0F.first_line=0,((_tmpA0F.first_column=0,((_tmpA0F.last_line=0,((_tmpA0F.last_column=
0,_tmpA0F)))))))));}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[
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
104,105,106,107,108,109,110,111,112,113,114,115,116};static char _tmp365[2]="$";
static char _tmp366[6]="error";static char _tmp367[12]="$undefined.";static char
_tmp368[5]="AUTO";static char _tmp369[9]="REGISTER";static char _tmp36A[7]="STATIC";
static char _tmp36B[7]="EXTERN";static char _tmp36C[8]="TYPEDEF";static char _tmp36D[5]="VOID";
static char _tmp36E[5]="CHAR";static char _tmp36F[6]="SHORT";static char _tmp370[4]="INT";
static char _tmp371[5]="LONG";static char _tmp372[6]="FLOAT";static char _tmp373[7]="DOUBLE";
static char _tmp374[7]="SIGNED";static char _tmp375[9]="UNSIGNED";static char _tmp376[
6]="CONST";static char _tmp377[9]="VOLATILE";static char _tmp378[9]="RESTRICT";
static char _tmp379[7]="STRUCT";static char _tmp37A[6]="UNION";static char _tmp37B[5]="CASE";
static char _tmp37C[8]="DEFAULT";static char _tmp37D[7]="INLINE";static char _tmp37E[7]="SIZEOF";
static char _tmp37F[9]="OFFSETOF";static char _tmp380[3]="IF";static char _tmp381[5]="ELSE";
static char _tmp382[7]="SWITCH";static char _tmp383[6]="WHILE";static char _tmp384[3]="DO";
static char _tmp385[4]="FOR";static char _tmp386[5]="GOTO";static char _tmp387[9]="CONTINUE";
static char _tmp388[6]="BREAK";static char _tmp389[7]="RETURN";static char _tmp38A[5]="ENUM";
static char _tmp38B[8]="NULL_kw";static char _tmp38C[4]="LET";static char _tmp38D[6]="THROW";
static char _tmp38E[4]="TRY";static char _tmp38F[6]="CATCH";static char _tmp390[7]="EXPORT";
static char _tmp391[4]="NEW";static char _tmp392[9]="ABSTRACT";static char _tmp393[9]="FALLTHRU";
static char _tmp394[6]="USING";static char _tmp395[10]="NAMESPACE";static char _tmp396[
9]="DATATYPE";static char _tmp397[8]="XTUNION";static char _tmp398[7]="TUNION";
static char _tmp399[7]="MALLOC";static char _tmp39A[8]="RMALLOC";static char _tmp39B[7]="CALLOC";
static char _tmp39C[8]="RCALLOC";static char _tmp39D[5]="SWAP";static char _tmp39E[9]="REGION_T";
static char _tmp39F[6]="TAG_T";static char _tmp3A0[7]="REGION";static char _tmp3A1[5]="RNEW";
static char _tmp3A2[8]="REGIONS";static char _tmp3A3[13]="RESET_REGION";static char
_tmp3A4[16]="NOZEROTERM_QUAL";static char _tmp3A5[14]="ZEROTERM_QUAL";static char
_tmp3A6[12]="REGION_QUAL";static char _tmp3A7[7]="PORTON";static char _tmp3A8[8]="PORTOFF";
static char _tmp3A9[12]="DYNREGION_T";static char _tmp3AA[6]="ALIAS";static char
_tmp3AB[8]="NUMELTS";static char _tmp3AC[8]="VALUEOF";static char _tmp3AD[10]="VALUEOF_T";
static char _tmp3AE[9]="TAGCHECK";static char _tmp3AF[13]="NUMELTS_QUAL";static char
_tmp3B0[10]="THIN_QUAL";static char _tmp3B1[9]="FAT_QUAL";static char _tmp3B2[13]="NOTNULL_QUAL";
static char _tmp3B3[14]="NULLABLE_QUAL";static char _tmp3B4[12]="TAGGED_QUAL";static
char _tmp3B5[16]="EXTENSIBLE_QUAL";static char _tmp3B6[15]="RESETABLE_QUAL";static
char _tmp3B7[7]="PTR_OP";static char _tmp3B8[7]="INC_OP";static char _tmp3B9[7]="DEC_OP";
static char _tmp3BA[8]="LEFT_OP";static char _tmp3BB[9]="RIGHT_OP";static char _tmp3BC[
6]="LE_OP";static char _tmp3BD[6]="GE_OP";static char _tmp3BE[6]="EQ_OP";static char
_tmp3BF[6]="NE_OP";static char _tmp3C0[7]="AND_OP";static char _tmp3C1[6]="OR_OP";
static char _tmp3C2[11]="MUL_ASSIGN";static char _tmp3C3[11]="DIV_ASSIGN";static char
_tmp3C4[11]="MOD_ASSIGN";static char _tmp3C5[11]="ADD_ASSIGN";static char _tmp3C6[11]="SUB_ASSIGN";
static char _tmp3C7[12]="LEFT_ASSIGN";static char _tmp3C8[13]="RIGHT_ASSIGN";static
char _tmp3C9[11]="AND_ASSIGN";static char _tmp3CA[11]="XOR_ASSIGN";static char
_tmp3CB[10]="OR_ASSIGN";static char _tmp3CC[9]="ELLIPSIS";static char _tmp3CD[11]="LEFT_RIGHT";
static char _tmp3CE[12]="COLON_COLON";static char _tmp3CF[11]="IDENTIFIER";static
char _tmp3D0[17]="INTEGER_CONSTANT";static char _tmp3D1[7]="STRING";static char
_tmp3D2[19]="CHARACTER_CONSTANT";static char _tmp3D3[18]="FLOATING_CONSTANT";
static char _tmp3D4[9]="TYPE_VAR";static char _tmp3D5[13]="TYPEDEF_NAME";static char
_tmp3D6[16]="QUAL_IDENTIFIER";static char _tmp3D7[18]="QUAL_TYPEDEF_NAME";static
char _tmp3D8[10]="ATTRIBUTE";static char _tmp3D9[4]="ASM";static char _tmp3DA[4]="';'";
static char _tmp3DB[4]="'{'";static char _tmp3DC[4]="'}'";static char _tmp3DD[4]="','";
static char _tmp3DE[4]="'='";static char _tmp3DF[4]="'<'";static char _tmp3E0[4]="'>'";
static char _tmp3E1[4]="'('";static char _tmp3E2[4]="')'";static char _tmp3E3[4]="'_'";
static char _tmp3E4[4]="'$'";static char _tmp3E5[4]="':'";static char _tmp3E6[4]="'.'";
static char _tmp3E7[4]="'['";static char _tmp3E8[4]="']'";static char _tmp3E9[4]="'*'";
static char _tmp3EA[4]="'@'";static char _tmp3EB[4]="'?'";static char _tmp3EC[4]="'+'";
static char _tmp3ED[4]="'|'";static char _tmp3EE[4]="'^'";static char _tmp3EF[4]="'&'";
static char _tmp3F0[4]="'-'";static char _tmp3F1[4]="'/'";static char _tmp3F2[4]="'%'";
static char _tmp3F3[4]="'~'";static char _tmp3F4[4]="'!'";static char _tmp3F5[5]="prog";
static char _tmp3F6[17]="translation_unit";static char _tmp3F7[12]="export_list";
static char _tmp3F8[19]="export_list_values";static char _tmp3F9[21]="external_declaration";
static char _tmp3FA[20]="function_definition";static char _tmp3FB[21]="function_definition2";
static char _tmp3FC[13]="using_action";static char _tmp3FD[15]="unusing_action";
static char _tmp3FE[17]="namespace_action";static char _tmp3FF[19]="unnamespace_action";
static char _tmp400[12]="declaration";static char _tmp401[19]="resetable_qual_opt";
static char _tmp402[17]="declaration_list";static char _tmp403[23]="declaration_specifiers";
static char _tmp404[24]="storage_class_specifier";static char _tmp405[15]="attributes_opt";
static char _tmp406[11]="attributes";static char _tmp407[15]="attribute_list";static
char _tmp408[10]="attribute";static char _tmp409[15]="type_specifier";static char
_tmp40A[25]="type_specifier_notypedef";static char _tmp40B[5]="kind";static char
_tmp40C[15]="type_qualifier";static char _tmp40D[15]="enum_specifier";static char
_tmp40E[11]="enum_field";static char _tmp40F[22]="enum_declaration_list";static
char _tmp410[26]="struct_or_union_specifier";static char _tmp411[16]="type_params_opt";
static char _tmp412[16]="struct_or_union";static char _tmp413[24]="struct_declaration_list";
static char _tmp414[25]="struct_declaration_list0";static char _tmp415[21]="init_declarator_list";
static char _tmp416[22]="init_declarator_list0";static char _tmp417[16]="init_declarator";
static char _tmp418[19]="struct_declaration";static char _tmp419[25]="specifier_qualifier_list";
static char _tmp41A[35]="notypedef_specifier_qualifier_list";static char _tmp41B[23]="struct_declarator_list";
static char _tmp41C[24]="struct_declarator_list0";static char _tmp41D[18]="struct_declarator";
static char _tmp41E[19]="datatype_specifier";static char _tmp41F[14]="qual_datatype";
static char _tmp420[19]="datatypefield_list";static char _tmp421[20]="datatypefield_scope";
static char _tmp422[14]="datatypefield";static char _tmp423[11]="declarator";static
char _tmp424[23]="declarator_withtypedef";static char _tmp425[18]="direct_declarator";
static char _tmp426[30]="direct_declarator_withtypedef";static char _tmp427[8]="pointer";
static char _tmp428[12]="one_pointer";static char _tmp429[14]="pointer_quals";static
char _tmp42A[13]="pointer_qual";static char _tmp42B[23]="pointer_null_and_bound";
static char _tmp42C[14]="pointer_bound";static char _tmp42D[18]="zeroterm_qual_opt";
static char _tmp42E[8]="rgn_opt";static char _tmp42F[11]="tqual_list";static char
_tmp430[20]="parameter_type_list";static char _tmp431[9]="type_var";static char
_tmp432[16]="optional_effect";static char _tmp433[19]="optional_rgn_order";static
char _tmp434[10]="rgn_order";static char _tmp435[16]="optional_inject";static char
_tmp436[11]="effect_set";static char _tmp437[14]="atomic_effect";static char _tmp438[
11]="region_set";static char _tmp439[15]="parameter_list";static char _tmp43A[22]="parameter_declaration";
static char _tmp43B[16]="identifier_list";static char _tmp43C[17]="identifier_list0";
static char _tmp43D[12]="initializer";static char _tmp43E[18]="array_initializer";
static char _tmp43F[17]="initializer_list";static char _tmp440[12]="designation";
static char _tmp441[16]="designator_list";static char _tmp442[11]="designator";
static char _tmp443[10]="type_name";static char _tmp444[14]="any_type_name";static
char _tmp445[15]="type_name_list";static char _tmp446[20]="abstract_declarator";
static char _tmp447[27]="direct_abstract_declarator";static char _tmp448[10]="statement";
static char _tmp449[13]="open_exp_opt";static char _tmp44A[18]="labeled_statement";
static char _tmp44B[21]="expression_statement";static char _tmp44C[19]="compound_statement";
static char _tmp44D[16]="block_item_list";static char _tmp44E[20]="selection_statement";
static char _tmp44F[15]="switch_clauses";static char _tmp450[20]="iteration_statement";
static char _tmp451[15]="jump_statement";static char _tmp452[12]="exp_pattern";
static char _tmp453[20]="conditional_pattern";static char _tmp454[19]="logical_or_pattern";
static char _tmp455[20]="logical_and_pattern";static char _tmp456[21]="inclusive_or_pattern";
static char _tmp457[21]="exclusive_or_pattern";static char _tmp458[12]="and_pattern";
static char _tmp459[17]="equality_pattern";static char _tmp45A[19]="relational_pattern";
static char _tmp45B[14]="shift_pattern";static char _tmp45C[17]="additive_pattern";
static char _tmp45D[23]="multiplicative_pattern";static char _tmp45E[13]="cast_pattern";
static char _tmp45F[14]="unary_pattern";static char _tmp460[16]="postfix_pattern";
static char _tmp461[16]="primary_pattern";static char _tmp462[8]="pattern";static
char _tmp463[19]="tuple_pattern_list";static char _tmp464[20]="tuple_pattern_list0";
static char _tmp465[14]="field_pattern";static char _tmp466[19]="field_pattern_list";
static char _tmp467[20]="field_pattern_list0";static char _tmp468[11]="expression";
static char _tmp469[22]="assignment_expression";static char _tmp46A[20]="assignment_operator";
static char _tmp46B[23]="conditional_expression";static char _tmp46C[20]="constant_expression";
static char _tmp46D[22]="logical_or_expression";static char _tmp46E[23]="logical_and_expression";
static char _tmp46F[24]="inclusive_or_expression";static char _tmp470[24]="exclusive_or_expression";
static char _tmp471[15]="and_expression";static char _tmp472[20]="equality_expression";
static char _tmp473[22]="relational_expression";static char _tmp474[17]="shift_expression";
static char _tmp475[20]="additive_expression";static char _tmp476[26]="multiplicative_expression";
static char _tmp477[16]="cast_expression";static char _tmp478[17]="unary_expression";
static char _tmp479[15]="unary_operator";static char _tmp47A[19]="postfix_expression";
static char _tmp47B[19]="primary_expression";static char _tmp47C[25]="argument_expression_list";
static char _tmp47D[26]="argument_expression_list0";static char _tmp47E[9]="constant";
static char _tmp47F[20]="qual_opt_identifier";static char _tmp480[17]="qual_opt_typedef";
static char _tmp481[18]="struct_union_name";static char _tmp482[11]="field_name";
static char _tmp483[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[287]={{
_tmp365,_tmp365,_tmp365 + 2},{_tmp366,_tmp366,_tmp366 + 6},{_tmp367,_tmp367,
_tmp367 + 12},{_tmp368,_tmp368,_tmp368 + 5},{_tmp369,_tmp369,_tmp369 + 9},{_tmp36A,
_tmp36A,_tmp36A + 7},{_tmp36B,_tmp36B,_tmp36B + 7},{_tmp36C,_tmp36C,_tmp36C + 8},{
_tmp36D,_tmp36D,_tmp36D + 5},{_tmp36E,_tmp36E,_tmp36E + 5},{_tmp36F,_tmp36F,
_tmp36F + 6},{_tmp370,_tmp370,_tmp370 + 4},{_tmp371,_tmp371,_tmp371 + 5},{_tmp372,
_tmp372,_tmp372 + 6},{_tmp373,_tmp373,_tmp373 + 7},{_tmp374,_tmp374,_tmp374 + 7},{
_tmp375,_tmp375,_tmp375 + 9},{_tmp376,_tmp376,_tmp376 + 6},{_tmp377,_tmp377,
_tmp377 + 9},{_tmp378,_tmp378,_tmp378 + 9},{_tmp379,_tmp379,_tmp379 + 7},{_tmp37A,
_tmp37A,_tmp37A + 6},{_tmp37B,_tmp37B,_tmp37B + 5},{_tmp37C,_tmp37C,_tmp37C + 8},{
_tmp37D,_tmp37D,_tmp37D + 7},{_tmp37E,_tmp37E,_tmp37E + 7},{_tmp37F,_tmp37F,
_tmp37F + 9},{_tmp380,_tmp380,_tmp380 + 3},{_tmp381,_tmp381,_tmp381 + 5},{_tmp382,
_tmp382,_tmp382 + 7},{_tmp383,_tmp383,_tmp383 + 6},{_tmp384,_tmp384,_tmp384 + 3},{
_tmp385,_tmp385,_tmp385 + 4},{_tmp386,_tmp386,_tmp386 + 5},{_tmp387,_tmp387,
_tmp387 + 9},{_tmp388,_tmp388,_tmp388 + 6},{_tmp389,_tmp389,_tmp389 + 7},{_tmp38A,
_tmp38A,_tmp38A + 5},{_tmp38B,_tmp38B,_tmp38B + 8},{_tmp38C,_tmp38C,_tmp38C + 4},{
_tmp38D,_tmp38D,_tmp38D + 6},{_tmp38E,_tmp38E,_tmp38E + 4},{_tmp38F,_tmp38F,
_tmp38F + 6},{_tmp390,_tmp390,_tmp390 + 7},{_tmp391,_tmp391,_tmp391 + 4},{_tmp392,
_tmp392,_tmp392 + 9},{_tmp393,_tmp393,_tmp393 + 9},{_tmp394,_tmp394,_tmp394 + 6},{
_tmp395,_tmp395,_tmp395 + 10},{_tmp396,_tmp396,_tmp396 + 9},{_tmp397,_tmp397,
_tmp397 + 8},{_tmp398,_tmp398,_tmp398 + 7},{_tmp399,_tmp399,_tmp399 + 7},{_tmp39A,
_tmp39A,_tmp39A + 8},{_tmp39B,_tmp39B,_tmp39B + 7},{_tmp39C,_tmp39C,_tmp39C + 8},{
_tmp39D,_tmp39D,_tmp39D + 5},{_tmp39E,_tmp39E,_tmp39E + 9},{_tmp39F,_tmp39F,
_tmp39F + 6},{_tmp3A0,_tmp3A0,_tmp3A0 + 7},{_tmp3A1,_tmp3A1,_tmp3A1 + 5},{_tmp3A2,
_tmp3A2,_tmp3A2 + 8},{_tmp3A3,_tmp3A3,_tmp3A3 + 13},{_tmp3A4,_tmp3A4,_tmp3A4 + 16},{
_tmp3A5,_tmp3A5,_tmp3A5 + 14},{_tmp3A6,_tmp3A6,_tmp3A6 + 12},{_tmp3A7,_tmp3A7,
_tmp3A7 + 7},{_tmp3A8,_tmp3A8,_tmp3A8 + 8},{_tmp3A9,_tmp3A9,_tmp3A9 + 12},{_tmp3AA,
_tmp3AA,_tmp3AA + 6},{_tmp3AB,_tmp3AB,_tmp3AB + 8},{_tmp3AC,_tmp3AC,_tmp3AC + 8},{
_tmp3AD,_tmp3AD,_tmp3AD + 10},{_tmp3AE,_tmp3AE,_tmp3AE + 9},{_tmp3AF,_tmp3AF,
_tmp3AF + 13},{_tmp3B0,_tmp3B0,_tmp3B0 + 10},{_tmp3B1,_tmp3B1,_tmp3B1 + 9},{_tmp3B2,
_tmp3B2,_tmp3B2 + 13},{_tmp3B3,_tmp3B3,_tmp3B3 + 14},{_tmp3B4,_tmp3B4,_tmp3B4 + 12},{
_tmp3B5,_tmp3B5,_tmp3B5 + 16},{_tmp3B6,_tmp3B6,_tmp3B6 + 15},{_tmp3B7,_tmp3B7,
_tmp3B7 + 7},{_tmp3B8,_tmp3B8,_tmp3B8 + 7},{_tmp3B9,_tmp3B9,_tmp3B9 + 7},{_tmp3BA,
_tmp3BA,_tmp3BA + 8},{_tmp3BB,_tmp3BB,_tmp3BB + 9},{_tmp3BC,_tmp3BC,_tmp3BC + 6},{
_tmp3BD,_tmp3BD,_tmp3BD + 6},{_tmp3BE,_tmp3BE,_tmp3BE + 6},{_tmp3BF,_tmp3BF,
_tmp3BF + 6},{_tmp3C0,_tmp3C0,_tmp3C0 + 7},{_tmp3C1,_tmp3C1,_tmp3C1 + 6},{_tmp3C2,
_tmp3C2,_tmp3C2 + 11},{_tmp3C3,_tmp3C3,_tmp3C3 + 11},{_tmp3C4,_tmp3C4,_tmp3C4 + 11},{
_tmp3C5,_tmp3C5,_tmp3C5 + 11},{_tmp3C6,_tmp3C6,_tmp3C6 + 11},{_tmp3C7,_tmp3C7,
_tmp3C7 + 12},{_tmp3C8,_tmp3C8,_tmp3C8 + 13},{_tmp3C9,_tmp3C9,_tmp3C9 + 11},{
_tmp3CA,_tmp3CA,_tmp3CA + 11},{_tmp3CB,_tmp3CB,_tmp3CB + 10},{_tmp3CC,_tmp3CC,
_tmp3CC + 9},{_tmp3CD,_tmp3CD,_tmp3CD + 11},{_tmp3CE,_tmp3CE,_tmp3CE + 12},{_tmp3CF,
_tmp3CF,_tmp3CF + 11},{_tmp3D0,_tmp3D0,_tmp3D0 + 17},{_tmp3D1,_tmp3D1,_tmp3D1 + 7},{
_tmp3D2,_tmp3D2,_tmp3D2 + 19},{_tmp3D3,_tmp3D3,_tmp3D3 + 18},{_tmp3D4,_tmp3D4,
_tmp3D4 + 9},{_tmp3D5,_tmp3D5,_tmp3D5 + 13},{_tmp3D6,_tmp3D6,_tmp3D6 + 16},{_tmp3D7,
_tmp3D7,_tmp3D7 + 18},{_tmp3D8,_tmp3D8,_tmp3D8 + 10},{_tmp3D9,_tmp3D9,_tmp3D9 + 4},{
_tmp3DA,_tmp3DA,_tmp3DA + 4},{_tmp3DB,_tmp3DB,_tmp3DB + 4},{_tmp3DC,_tmp3DC,
_tmp3DC + 4},{_tmp3DD,_tmp3DD,_tmp3DD + 4},{_tmp3DE,_tmp3DE,_tmp3DE + 4},{_tmp3DF,
_tmp3DF,_tmp3DF + 4},{_tmp3E0,_tmp3E0,_tmp3E0 + 4},{_tmp3E1,_tmp3E1,_tmp3E1 + 4},{
_tmp3E2,_tmp3E2,_tmp3E2 + 4},{_tmp3E3,_tmp3E3,_tmp3E3 + 4},{_tmp3E4,_tmp3E4,
_tmp3E4 + 4},{_tmp3E5,_tmp3E5,_tmp3E5 + 4},{_tmp3E6,_tmp3E6,_tmp3E6 + 4},{_tmp3E7,
_tmp3E7,_tmp3E7 + 4},{_tmp3E8,_tmp3E8,_tmp3E8 + 4},{_tmp3E9,_tmp3E9,_tmp3E9 + 4},{
_tmp3EA,_tmp3EA,_tmp3EA + 4},{_tmp3EB,_tmp3EB,_tmp3EB + 4},{_tmp3EC,_tmp3EC,
_tmp3EC + 4},{_tmp3ED,_tmp3ED,_tmp3ED + 4},{_tmp3EE,_tmp3EE,_tmp3EE + 4},{_tmp3EF,
_tmp3EF,_tmp3EF + 4},{_tmp3F0,_tmp3F0,_tmp3F0 + 4},{_tmp3F1,_tmp3F1,_tmp3F1 + 4},{
_tmp3F2,_tmp3F2,_tmp3F2 + 4},{_tmp3F3,_tmp3F3,_tmp3F3 + 4},{_tmp3F4,_tmp3F4,
_tmp3F4 + 4},{_tmp3F5,_tmp3F5,_tmp3F5 + 5},{_tmp3F6,_tmp3F6,_tmp3F6 + 17},{_tmp3F7,
_tmp3F7,_tmp3F7 + 12},{_tmp3F8,_tmp3F8,_tmp3F8 + 19},{_tmp3F9,_tmp3F9,_tmp3F9 + 21},{
_tmp3FA,_tmp3FA,_tmp3FA + 20},{_tmp3FB,_tmp3FB,_tmp3FB + 21},{_tmp3FC,_tmp3FC,
_tmp3FC + 13},{_tmp3FD,_tmp3FD,_tmp3FD + 15},{_tmp3FE,_tmp3FE,_tmp3FE + 17},{
_tmp3FF,_tmp3FF,_tmp3FF + 19},{_tmp400,_tmp400,_tmp400 + 12},{_tmp401,_tmp401,
_tmp401 + 19},{_tmp402,_tmp402,_tmp402 + 17},{_tmp403,_tmp403,_tmp403 + 23},{
_tmp404,_tmp404,_tmp404 + 24},{_tmp405,_tmp405,_tmp405 + 15},{_tmp406,_tmp406,
_tmp406 + 11},{_tmp407,_tmp407,_tmp407 + 15},{_tmp408,_tmp408,_tmp408 + 10},{
_tmp409,_tmp409,_tmp409 + 15},{_tmp40A,_tmp40A,_tmp40A + 25},{_tmp40B,_tmp40B,
_tmp40B + 5},{_tmp40C,_tmp40C,_tmp40C + 15},{_tmp40D,_tmp40D,_tmp40D + 15},{_tmp40E,
_tmp40E,_tmp40E + 11},{_tmp40F,_tmp40F,_tmp40F + 22},{_tmp410,_tmp410,_tmp410 + 26},{
_tmp411,_tmp411,_tmp411 + 16},{_tmp412,_tmp412,_tmp412 + 16},{_tmp413,_tmp413,
_tmp413 + 24},{_tmp414,_tmp414,_tmp414 + 25},{_tmp415,_tmp415,_tmp415 + 21},{
_tmp416,_tmp416,_tmp416 + 22},{_tmp417,_tmp417,_tmp417 + 16},{_tmp418,_tmp418,
_tmp418 + 19},{_tmp419,_tmp419,_tmp419 + 25},{_tmp41A,_tmp41A,_tmp41A + 35},{
_tmp41B,_tmp41B,_tmp41B + 23},{_tmp41C,_tmp41C,_tmp41C + 24},{_tmp41D,_tmp41D,
_tmp41D + 18},{_tmp41E,_tmp41E,_tmp41E + 19},{_tmp41F,_tmp41F,_tmp41F + 14},{
_tmp420,_tmp420,_tmp420 + 19},{_tmp421,_tmp421,_tmp421 + 20},{_tmp422,_tmp422,
_tmp422 + 14},{_tmp423,_tmp423,_tmp423 + 11},{_tmp424,_tmp424,_tmp424 + 23},{
_tmp425,_tmp425,_tmp425 + 18},{_tmp426,_tmp426,_tmp426 + 30},{_tmp427,_tmp427,
_tmp427 + 8},{_tmp428,_tmp428,_tmp428 + 12},{_tmp429,_tmp429,_tmp429 + 14},{_tmp42A,
_tmp42A,_tmp42A + 13},{_tmp42B,_tmp42B,_tmp42B + 23},{_tmp42C,_tmp42C,_tmp42C + 14},{
_tmp42D,_tmp42D,_tmp42D + 18},{_tmp42E,_tmp42E,_tmp42E + 8},{_tmp42F,_tmp42F,
_tmp42F + 11},{_tmp430,_tmp430,_tmp430 + 20},{_tmp431,_tmp431,_tmp431 + 9},{_tmp432,
_tmp432,_tmp432 + 16},{_tmp433,_tmp433,_tmp433 + 19},{_tmp434,_tmp434,_tmp434 + 10},{
_tmp435,_tmp435,_tmp435 + 16},{_tmp436,_tmp436,_tmp436 + 11},{_tmp437,_tmp437,
_tmp437 + 14},{_tmp438,_tmp438,_tmp438 + 11},{_tmp439,_tmp439,_tmp439 + 15},{
_tmp43A,_tmp43A,_tmp43A + 22},{_tmp43B,_tmp43B,_tmp43B + 16},{_tmp43C,_tmp43C,
_tmp43C + 17},{_tmp43D,_tmp43D,_tmp43D + 12},{_tmp43E,_tmp43E,_tmp43E + 18},{
_tmp43F,_tmp43F,_tmp43F + 17},{_tmp440,_tmp440,_tmp440 + 12},{_tmp441,_tmp441,
_tmp441 + 16},{_tmp442,_tmp442,_tmp442 + 11},{_tmp443,_tmp443,_tmp443 + 10},{
_tmp444,_tmp444,_tmp444 + 14},{_tmp445,_tmp445,_tmp445 + 15},{_tmp446,_tmp446,
_tmp446 + 20},{_tmp447,_tmp447,_tmp447 + 27},{_tmp448,_tmp448,_tmp448 + 10},{
_tmp449,_tmp449,_tmp449 + 13},{_tmp44A,_tmp44A,_tmp44A + 18},{_tmp44B,_tmp44B,
_tmp44B + 21},{_tmp44C,_tmp44C,_tmp44C + 19},{_tmp44D,_tmp44D,_tmp44D + 16},{
_tmp44E,_tmp44E,_tmp44E + 20},{_tmp44F,_tmp44F,_tmp44F + 15},{_tmp450,_tmp450,
_tmp450 + 20},{_tmp451,_tmp451,_tmp451 + 15},{_tmp452,_tmp452,_tmp452 + 12},{
_tmp453,_tmp453,_tmp453 + 20},{_tmp454,_tmp454,_tmp454 + 19},{_tmp455,_tmp455,
_tmp455 + 20},{_tmp456,_tmp456,_tmp456 + 21},{_tmp457,_tmp457,_tmp457 + 21},{
_tmp458,_tmp458,_tmp458 + 12},{_tmp459,_tmp459,_tmp459 + 17},{_tmp45A,_tmp45A,
_tmp45A + 19},{_tmp45B,_tmp45B,_tmp45B + 14},{_tmp45C,_tmp45C,_tmp45C + 17},{
_tmp45D,_tmp45D,_tmp45D + 23},{_tmp45E,_tmp45E,_tmp45E + 13},{_tmp45F,_tmp45F,
_tmp45F + 14},{_tmp460,_tmp460,_tmp460 + 16},{_tmp461,_tmp461,_tmp461 + 16},{
_tmp462,_tmp462,_tmp462 + 8},{_tmp463,_tmp463,_tmp463 + 19},{_tmp464,_tmp464,
_tmp464 + 20},{_tmp465,_tmp465,_tmp465 + 14},{_tmp466,_tmp466,_tmp466 + 19},{
_tmp467,_tmp467,_tmp467 + 20},{_tmp468,_tmp468,_tmp468 + 11},{_tmp469,_tmp469,
_tmp469 + 22},{_tmp46A,_tmp46A,_tmp46A + 20},{_tmp46B,_tmp46B,_tmp46B + 23},{
_tmp46C,_tmp46C,_tmp46C + 20},{_tmp46D,_tmp46D,_tmp46D + 22},{_tmp46E,_tmp46E,
_tmp46E + 23},{_tmp46F,_tmp46F,_tmp46F + 24},{_tmp470,_tmp470,_tmp470 + 24},{
_tmp471,_tmp471,_tmp471 + 15},{_tmp472,_tmp472,_tmp472 + 20},{_tmp473,_tmp473,
_tmp473 + 22},{_tmp474,_tmp474,_tmp474 + 17},{_tmp475,_tmp475,_tmp475 + 20},{
_tmp476,_tmp476,_tmp476 + 26},{_tmp477,_tmp477,_tmp477 + 16},{_tmp478,_tmp478,
_tmp478 + 17},{_tmp479,_tmp479,_tmp479 + 15},{_tmp47A,_tmp47A,_tmp47A + 19},{
_tmp47B,_tmp47B,_tmp47B + 19},{_tmp47C,_tmp47C,_tmp47C + 25},{_tmp47D,_tmp47D,
_tmp47D + 26},{_tmp47E,_tmp47E,_tmp47E + 9},{_tmp47F,_tmp47F,_tmp47F + 20},{_tmp480,
_tmp480,_tmp480 + 17},{_tmp481,_tmp481,_tmp481 + 18},{_tmp482,_tmp482,_tmp482 + 11},{
_tmp483,_tmp483,_tmp483 + 12}};static short Cyc_yyr1[512]={0,144,145,145,145,145,
145,145,145,145,145,145,146,146,147,147,147,148,148,148,149,149,149,149,150,150,
151,152,153,154,155,155,155,155,155,155,155,156,156,157,157,158,158,158,158,158,
158,158,158,158,158,159,159,159,159,159,159,159,160,160,161,162,162,163,163,163,
163,163,163,164,164,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,
165,165,165,165,165,165,165,165,166,167,167,167,168,168,168,169,169,170,170,170,
171,171,171,171,171,172,172,173,173,174,174,175,175,176,177,177,178,178,179,180,
180,180,180,180,180,181,181,181,181,181,181,182,183,183,184,184,184,185,185,185,
186,186,187,187,187,187,188,188,188,189,189,190,190,191,191,192,192,192,192,192,
192,192,192,192,192,193,193,193,193,193,193,193,193,193,193,193,194,194,195,196,
196,197,197,197,197,197,197,197,197,198,198,198,199,199,200,200,200,201,201,201,
202,202,203,203,203,203,204,204,205,205,206,206,207,207,208,208,209,209,210,210,
210,210,211,211,212,212,213,213,213,214,215,215,216,216,217,217,217,217,218,218,
218,218,219,220,220,221,221,222,222,223,223,223,223,223,224,224,225,225,225,226,
226,226,226,226,226,226,226,226,226,226,227,227,227,227,227,227,227,228,228,229,
230,230,231,231,232,232,232,232,232,232,233,233,233,233,233,233,234,234,234,234,
234,234,235,235,235,235,235,235,235,235,235,235,235,235,235,235,236,236,236,236,
236,236,236,236,237,238,238,239,239,240,240,241,241,242,242,243,243,244,244,244,
245,245,245,245,245,246,246,246,247,247,247,248,248,248,248,249,249,250,250,250,
250,250,250,250,251,252,253,253,253,253,253,253,253,253,253,253,253,253,253,253,
253,254,254,254,255,255,256,256,257,257,257,258,258,259,259,260,260,260,261,261,
261,261,261,261,261,261,261,261,261,262,262,262,262,262,262,262,263,264,264,265,
265,266,266,267,267,268,268,269,269,269,270,270,270,270,270,271,271,271,272,272,
272,273,273,273,273,274,274,275,275,275,275,275,275,275,275,275,275,275,275,275,
275,275,275,275,275,275,275,276,276,276,277,277,277,277,277,277,277,277,277,277,
278,278,278,278,278,278,278,278,278,279,280,280,281,281,281,281,282,282,283,283,
284,284,285,285,286,286};static short Cyc_yyr2[512]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,1,
2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,8,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,
2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,
1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,
1,2,1,1,3,1,2,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,
4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,
2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,
1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,
2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,
1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,
1,3,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,
1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,
2,2,2,2,2,4,2,6,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,
1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[1028]={0,19,51,52,53,
54,56,71,74,75,76,77,78,79,80,81,95,96,97,113,114,47,0,0,57,0,0,146,88,92,0,0,0,
0,0,0,0,38,502,212,504,503,505,0,0,72,0,198,198,197,1,0,17,0,0,18,0,0,41,49,43,
69,45,82,83,0,84,0,37,157,0,182,185,85,161,111,55,54,48,0,99,501,0,502,498,499,
500,111,0,372,0,0,0,0,235,0,374,375,26,28,0,0,0,0,0,0,0,0,147,0,0,0,0,0,0,0,195,
196,2,0,0,0,0,0,30,0,119,120,122,42,50,44,46,115,506,507,111,111,37,39,37,0,20,0,
214,0,170,158,183,192,191,0,0,189,190,193,194,203,185,0,70,0,55,103,0,101,0,502,
382,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,488,472,0,0,0,0,0,475,473,474,0,399,401,
415,423,425,427,429,431,433,436,441,444,447,451,0,453,476,487,486,0,383,381,33,0,
0,111,0,0,0,129,125,127,253,255,0,0,9,10,0,0,0,111,508,509,213,94,0,0,162,73,233,
0,230,0,3,0,5,0,283,0,31,0,0,37,21,0,116,117,0,110,0,144,0,0,0,0,0,0,0,0,0,0,0,0,
502,286,288,0,294,290,0,292,276,277,278,0,279,280,281,0,40,22,122,260,0,220,236,
0,0,216,214,0,200,0,0,0,205,58,204,186,0,0,104,100,0,0,376,0,0,396,0,0,0,0,249,
392,397,0,394,0,461,0,417,451,0,418,419,0,0,0,0,0,0,0,0,0,454,455,37,0,0,0,457,
458,456,0,373,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,405,406,407,408,409,410,
411,412,413,414,404,0,459,0,482,483,0,0,0,490,0,111,389,390,0,387,0,237,0,0,0,0,
256,228,0,130,135,131,133,126,128,214,0,262,254,263,511,510,0,87,91,0,89,0,93,
109,64,63,0,61,163,214,232,159,262,234,171,172,0,86,199,27,0,29,0,0,0,0,121,123,
239,238,23,106,118,0,0,0,137,138,140,0,111,111,152,0,0,0,0,0,0,37,0,323,324,325,
0,0,327,0,0,0,0,295,291,122,293,289,287,0,169,221,0,0,0,227,215,222,166,0,0,0,
216,168,202,201,164,200,0,0,206,59,112,0,105,422,102,98,385,386,252,0,393,248,
250,380,0,0,0,0,240,244,0,0,0,0,0,0,0,0,0,0,0,0,0,489,496,0,495,400,424,0,426,
428,430,432,434,435,439,440,437,438,442,443,445,446,448,449,450,403,402,481,478,
0,480,0,0,0,377,0,384,32,0,378,0,0,257,136,132,134,0,216,0,200,0,264,0,214,0,275,
259,0,0,111,0,0,0,129,0,111,0,214,0,181,160,231,4,6,0,35,0,0,141,124,0,0,216,145,
154,153,0,0,148,0,0,0,302,0,0,0,0,0,322,326,0,0,0,0,285,37,24,261,214,0,224,0,0,
217,0,167,220,208,165,188,187,206,184,0,7,0,251,395,398,460,0,0,241,0,245,464,0,
0,0,0,468,471,0,0,0,0,0,452,492,0,0,479,477,0,0,388,391,379,258,229,271,0,265,
266,200,0,0,216,200,0,90,0,216,0,0,0,60,62,0,173,0,0,216,0,200,0,0,0,139,142,115,
143,155,152,152,0,0,0,0,0,0,0,0,0,0,0,0,0,302,328,0,0,25,216,0,225,223,0,216,0,
207,0,8,0,0,0,242,246,0,0,0,0,420,421,481,480,494,0,497,416,491,493,0,270,268,
274,273,0,267,200,0,115,0,67,65,66,174,180,177,0,179,175,200,0,34,0,0,151,150,
296,302,0,0,0,0,0,0,330,331,333,335,337,339,341,343,346,351,354,357,361,363,370,
371,0,37,299,308,0,0,0,0,0,0,0,0,0,0,329,282,210,226,218,209,214,13,0,14,463,462,
0,247,465,0,0,470,469,484,0,272,269,36,0,0,178,176,284,108,0,0,0,302,0,367,0,0,
364,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,365,303,0,310,0,0,0,318,0,0,0,0,0,
0,301,0,216,12,15,0,0,0,0,485,107,0,156,297,298,0,0,0,0,302,304,334,0,336,338,
340,342,344,345,349,350,347,348,352,353,355,356,358,359,360,0,309,311,312,0,320,
319,0,314,0,0,0,219,211,16,0,0,0,0,300,366,0,362,305,0,37,313,321,315,316,0,243,
466,0,68,0,0,332,302,306,317,0,369,368,307,467,0,0,0};static short Cyc_yydefgoto[
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
238,96,62,298,62,795,146,362,529,141,- 149,96,168,428,396,529,60,60,710,287,59,
141,218,509,434,62,222,436,636,403,112,449,223,60,60,60,60,516,39,541,677,542,60,
128,451,508,39,430,528,716,432,239,60,554,60,556,171,240,404,432,102,313,451,721,
38,84,186,85,86,118,297,41,372,373,427,60,430,622,255,868,430,503,352,378,431,
189,420,379,96,432,96,422,448,504,256,286,286,244,286,323,522,622,421,421,421,
266,268,374,375,526,475,159,919,431,435,169,920,431,267,59,233,234,484,466,62,
432,755,299,491,432,103,170,105,864,108,96,74,251,252,253,254,141,96,362,59,59,
407,59,363,62,62,509,62,407,913,914,324,529,80,97,39,98,106,38,109,286,287,287,
668,287,41,325,692,525,60,397,398,399,802,318,74,44,806,580,581,582,425,74,362,
915,916,413,426,74,505,897,708,747,819,60,60,134,60,137,598,304,74,439,502,113,
547,362,362,- 214,59,165,- 214,400,97,62,945,782,401,402,615,239,781,417,97,362,
871,240,509,792,793,169,287,1003,74,74,19,20,623,134,299,451,- 236,74,96,- 236,248,
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
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,281,84,186,85,86,39,40,41,42,43,187,282,138,- 302,
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
_tuple25{struct Cyc_Position_Segment*f1;struct _tuple1*f2;int f3;};struct _tuple26{
struct _dyneither_ptr f1;void*f2;};static char _tmp521[8]="stdcall";static char
_tmp522[6]="cdecl";static char _tmp523[9]="fastcall";static char _tmp524[9]="noreturn";
static char _tmp525[6]="const";static char _tmp526[8]="aligned";static char _tmp527[7]="packed";
static char _tmp528[7]="shared";static char _tmp529[7]="unused";static char _tmp52A[5]="weak";
static char _tmp52B[10]="dllimport";static char _tmp52C[10]="dllexport";static char
_tmp52D[23]="no_instrument_function";static char _tmp52E[12]="constructor";static
char _tmp52F[11]="destructor";static char _tmp530[22]="no_check_memory_usage";
static char _tmp531[5]="pure";struct _tuple27{void*f1;void*f2;};struct _tuple28{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*
yylex_buf);static void _tmpA14(unsigned int*_tmpA13,unsigned int*_tmpA12,short**
_tmpA11){for(*_tmpA13=0;*_tmpA13 < *_tmpA12;(*_tmpA13)++){(*_tmpA11)[*_tmpA13]=(
short)0;}}static void _tmpA19(unsigned int*_tmpA18,unsigned int*_tmpA17,union Cyc_YYSTYPE**
_tmpA16){for(*_tmpA18=0;*_tmpA18 < *_tmpA17;(*_tmpA18)++){(*_tmpA16)[*_tmpA18]=
Cyc_yylval;}}static void _tmpA1E(unsigned int*_tmpA1D,unsigned int*_tmpA1C,struct
Cyc_Yyltype**_tmpA1B){for(*_tmpA1D=0;*_tmpA1D < *_tmpA1C;(*_tmpA1D)++){(*_tmpA1B)[*
_tmpA1D]=Cyc_yynewloc();}}static void _tmpF1B(unsigned int*_tmpF1A,unsigned int*
_tmpF19,char**_tmpF17){for(*_tmpF1A=0;*_tmpF1A < *_tmpF19;(*_tmpF1A)++){(*_tmpF17)[*
_tmpF1A]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct
_RegionHandle _tmp485=_new_region("yyregion");struct _RegionHandle*yyregion=&
_tmp485;_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;
int yyssp_offset;unsigned int _tmpA13;unsigned int _tmpA12;short*_tmpA11;
unsigned int _tmpA10;short*yyss=(short*)((_tmpA10=10000,((_tmpA11=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmpA10)),((((_tmpA12=_tmpA10,
_tmpA14(& _tmpA13,& _tmpA12,& _tmpA11))),_tmpA11))))));int yyvsp_offset;unsigned int
_tmpA18;unsigned int _tmpA17;union Cyc_YYSTYPE*_tmpA16;unsigned int _tmpA15;union
Cyc_YYSTYPE*yyvs=(union Cyc_YYSTYPE*)((_tmpA15=10000,((_tmpA16=(union Cyc_YYSTYPE*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA15)),((((
_tmpA17=_tmpA15,_tmpA19(& _tmpA18,& _tmpA17,& _tmpA16))),_tmpA16))))));int
yylsp_offset;unsigned int _tmpA1D;unsigned int _tmpA1C;struct Cyc_Yyltype*_tmpA1B;
unsigned int _tmpA1A;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)((_tmpA1A=10000,((
_tmpA1B=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct
Cyc_Yyltype),_tmpA1A)),((((_tmpA1C=_tmpA1A,_tmpA1E(& _tmpA1D,& _tmpA1C,& _tmpA1B))),
_tmpA1B))))));int yystacksize=10000;union Cyc_YYSTYPE yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){{const char*_tmpA1F;Cyc_yyerror(((_tmpA1F="parser stack overflow",
_tag_dyneither(_tmpA1F,sizeof(char),22))));}(int)_throw((void*)& Cyc_Yystack_overflow_val);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1028,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 371?(int)Cyc_yytranslate[_check_known_subscript_notnull(372,Cyc_yychar)]:
287;}yyn +=yychar1;if((yyn < 0  || yyn > 6244) || Cyc_yycheck[
_check_known_subscript_notnull(6245,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6245,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1027){int
_tmp487=0;_npop_handler(0);return _tmp487;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1028,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(512,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL22C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL22D: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp488;_push_handler(& _tmp488);{int _tmp48A=0;if(setjmp(_tmp488.handler))_tmp48A=
1;if(!_tmp48A){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp489=(void*)_exn_thrown;void*
_tmp48C=_tmp489;_LL230: {struct Cyc_Core_Failure_struct*_tmp48D=(struct Cyc_Core_Failure_struct*)
_tmp48C;if(_tmp48D->tag != Cyc_Core_Failure)goto _LL232;}_LL231: x=0;goto _LL22F;
_LL232:;_LL233:(void)_throw(_tmp48C);_LL22F:;}};}{struct _handler_cons _tmp48E;
_push_handler(& _tmp48E);{int _tmp490=0;if(setjmp(_tmp48E.handler))_tmp490=1;if(!
_tmp490){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;
_pop_handler();}else{void*_tmp48F=(void*)_exn_thrown;void*_tmp492=_tmp48F;_LL235: {
struct Cyc_Core_Failure_struct*_tmp493=(struct Cyc_Core_Failure_struct*)_tmp492;
if(_tmp493->tag != Cyc_Core_Failure)goto _LL237;}_LL236: y=0;goto _LL234;_LL237:;
_LL238:(void)_throw(_tmp492);_LL234:;}};}yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));break;}
case 3: _LL22E:{struct Cyc_Absyn_Decl*_tmpA29;struct Cyc_Absyn_Using_d_struct _tmpA28;
struct Cyc_Absyn_Using_d_struct*_tmpA27;struct Cyc_List_List*_tmpA26;yyval=Cyc_YY18(((
_tmpA26=_cycalloc(sizeof(*_tmpA26)),((_tmpA26->hd=((_tmpA29=_cycalloc(sizeof(*
_tmpA29)),((_tmpA29->r=(void*)((_tmpA27=_cycalloc(sizeof(*_tmpA27)),((_tmpA27[0]=((
_tmpA28.tag=11,((_tmpA28.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA28.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA28)))))),_tmpA27)))),((
_tmpA29->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA29)))))),((_tmpA26->tl=0,_tmpA26)))))));}Cyc_Lex_leave_using();
break;case 4: _LL239:{struct Cyc_Absyn_Decl*_tmpA33;struct Cyc_Absyn_Using_d_struct
_tmpA32;struct Cyc_Absyn_Using_d_struct*_tmpA31;struct Cyc_List_List*_tmpA30;yyval=
Cyc_YY18(((_tmpA30=_cycalloc(sizeof(*_tmpA30)),((_tmpA30->hd=((_tmpA33=_cycalloc(
sizeof(*_tmpA33)),((_tmpA33->r=(void*)((_tmpA31=_cycalloc(sizeof(*_tmpA31)),((
_tmpA31[0]=((_tmpA32.tag=11,((_tmpA32.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpA32.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA32)))))),
_tmpA31)))),((_tmpA33->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA33)))))),((
_tmpA30->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA30)))))));}break;case 5: _LL23A:{struct Cyc_Absyn_Decl*_tmpA42;
struct Cyc_Absyn_Namespace_d_struct _tmpA41;struct _dyneither_ptr*_tmpA40;struct Cyc_Absyn_Namespace_d_struct*
_tmpA3F;struct Cyc_List_List*_tmpA3E;yyval=Cyc_YY18(((_tmpA3E=_cycalloc(sizeof(*
_tmpA3E)),((_tmpA3E->hd=((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42->r=(void*)((
_tmpA3F=_cycalloc(sizeof(*_tmpA3F)),((_tmpA3F[0]=((_tmpA41.tag=10,((_tmpA41.f1=((
_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA40)))),((_tmpA41.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA41)))))),
_tmpA3F)))),((_tmpA42->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA42)))))),((
_tmpA3E->tl=0,_tmpA3E)))))));}Cyc_Lex_leave_namespace();break;case 6: _LL23B: {
struct _dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp4A1;_push_handler(& _tmp4A1);{int _tmp4A3=0;if(setjmp(_tmp4A1.handler))
_tmp4A3=1;if(!_tmp4A3){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp4A2=(void*)_exn_thrown;void*_tmp4A5=_tmp4A2;_LL23E: {struct Cyc_Core_Failure_struct*
_tmp4A6=(struct Cyc_Core_Failure_struct*)_tmp4A5;if(_tmp4A6->tag != Cyc_Core_Failure)
goto _LL240;}_LL23F:{const char*_tmpA43;nspace=((_tmpA43="",_tag_dyneither(_tmpA43,
sizeof(char),1)));}goto _LL23D;_LL240:;_LL241:(void)_throw(_tmp4A5);_LL23D:;}};}{
struct _handler_cons _tmp4A8;_push_handler(& _tmp4A8);{int _tmp4AA=0;if(setjmp(
_tmp4A8.handler))_tmp4AA=1;if(!_tmp4AA){x=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp4A9=(void*)_exn_thrown;void*_tmp4AC=_tmp4A9;_LL243: {struct Cyc_Core_Failure_struct*
_tmp4AD=(struct Cyc_Core_Failure_struct*)_tmp4AC;if(_tmp4AD->tag != Cyc_Core_Failure)
goto _LL245;}_LL244: x=0;goto _LL242;_LL245:;_LL246:(void)_throw(_tmp4AC);_LL242:;}};}{
struct _handler_cons _tmp4AE;_push_handler(& _tmp4AE);{int _tmp4B0=0;if(setjmp(
_tmp4AE.handler))_tmp4B0=1;if(!_tmp4B0){y=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{void*
_tmp4AF=(void*)_exn_thrown;void*_tmp4B2=_tmp4AF;_LL248: {struct Cyc_Core_Failure_struct*
_tmp4B3=(struct Cyc_Core_Failure_struct*)_tmp4B2;if(_tmp4B3->tag != Cyc_Core_Failure)
goto _LL24A;}_LL249: y=0;goto _LL247;_LL24A:;_LL24B:(void)_throw(_tmp4B2);_LL247:;}};}{
struct Cyc_Absyn_Decl*_tmpA52;struct Cyc_Absyn_Namespace_d_struct _tmpA51;struct
_dyneither_ptr*_tmpA50;struct Cyc_Absyn_Namespace_d_struct*_tmpA4F;struct Cyc_List_List*
_tmpA4E;yyval=Cyc_YY18(((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E->hd=((
_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->r=(void*)((_tmpA4F=_cycalloc(
sizeof(*_tmpA4F)),((_tmpA4F[0]=((_tmpA51.tag=10,((_tmpA51.f1=((_tmpA50=_cycalloc(
sizeof(*_tmpA50)),((_tmpA50[0]=nspace,_tmpA50)))),((_tmpA51.f2=x,_tmpA51)))))),
_tmpA4F)))),((_tmpA52->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA52)))))),((
_tmpA4E->tl=y,_tmpA4E)))))));}break;}case 7: _LL23C:{const char*_tmpA53;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),((_tmpA53="C",_tag_dyneither(_tmpA53,sizeof(char),2))))
== 0){struct Cyc_Absyn_Decl*_tmpA5D;struct Cyc_Absyn_ExternC_d_struct _tmpA5C;
struct Cyc_Absyn_ExternC_d_struct*_tmpA5B;struct Cyc_List_List*_tmpA5A;yyval=Cyc_YY18(((
_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A->hd=((_tmpA5D=_cycalloc(sizeof(*
_tmpA5D)),((_tmpA5D->r=(void*)((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=((
_tmpA5C.tag=12,((_tmpA5C.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpA5C)))),_tmpA5B)))),((_tmpA5D->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA5D)))))),((
_tmpA5A->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA5A)))))));}else{{const char*_tmpA5E;if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),((_tmpA5E="C include",_tag_dyneither(_tmpA5E,sizeof(char),10))))
!= 0){const char*_tmpA5F;Cyc_Parse_err(((_tmpA5F="expecting \"C\" or \"C include\"",
_tag_dyneither(_tmpA5F,sizeof(char),29))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}}{struct Cyc_Absyn_Decl*
_tmpA69;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA68;struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpA67;struct Cyc_List_List*_tmpA66;yyval=Cyc_YY18(((_tmpA66=_cycalloc(sizeof(*
_tmpA66)),((_tmpA66->hd=((_tmpA69=_cycalloc(sizeof(*_tmpA69)),((_tmpA69->r=(void*)((
_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67[0]=((_tmpA68.tag=13,((_tmpA68.f1=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpA68.f2=0,_tmpA68)))))),_tmpA67)))),((_tmpA69->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA69)))))),((
_tmpA66->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA66)))))));};}}break;case 8: _LL24C:{const char*_tmpA6A;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]),((_tmpA6A="C include",_tag_dyneither(_tmpA6A,sizeof(
char),10))))!= 0){const char*_tmpA6B;Cyc_Parse_err(((_tmpA6B="expecting \"C include\"",
_tag_dyneither(_tmpA6B,sizeof(char),22))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_Absyn_Decl*_tmpA75;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA74;
struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA73;struct Cyc_List_List*_tmpA72;
yyval=Cyc_YY18(((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72->hd=((_tmpA75=
_cycalloc(sizeof(*_tmpA75)),((_tmpA75->r=(void*)((_tmpA73=_cycalloc(sizeof(*
_tmpA73)),((_tmpA73[0]=((_tmpA74.tag=13,((_tmpA74.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA74.f2=exs,_tmpA74)))))),
_tmpA73)))),((_tmpA75->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA75)))))),((
_tmpA72->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA72)))))));}break;};case 9: _LL24D:{struct Cyc_Absyn_Decl*
_tmpA78;struct Cyc_List_List*_tmpA77;yyval=Cyc_YY18(((_tmpA77=_cycalloc(sizeof(*
_tmpA77)),((_tmpA77->hd=((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78->r=(void*)&
Cyc_Absyn_Porton_d_val,((_tmpA78->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA78)))))),((
_tmpA77->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA77)))))));}break;case 10: _LL24E:{struct Cyc_Absyn_Decl*
_tmpA7B;struct Cyc_List_List*_tmpA7A;yyval=Cyc_YY18(((_tmpA7A=_cycalloc(sizeof(*
_tmpA7A)),((_tmpA7A->hd=((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B->r=(void*)&
Cyc_Absyn_Portoff_d_val,((_tmpA7B->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA7B)))))),((
_tmpA7A->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA7A)))))));}break;case 11: _LL24F: yyval=Cyc_YY18(0);break;case
12: _LL250: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 13: _LL251: yyval=Cyc_YY52(0);break;case 14: _LL252:{struct _tuple25*
_tmpA7E;struct Cyc_List_List*_tmpA7D;yyval=Cyc_YY52(((_tmpA7D=_cycalloc(sizeof(*
_tmpA7D)),((_tmpA7D->hd=((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpA7E->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpA7E->f3=0,_tmpA7E)))))))),((_tmpA7D->tl=0,_tmpA7D)))))));}break;case 15:
_LL253:{struct _tuple25*_tmpA81;struct Cyc_List_List*_tmpA80;yyval=Cyc_YY52(((
_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80->hd=((_tmpA81=_cycalloc(sizeof(*
_tmpA81)),((_tmpA81->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpA81->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpA81->f3=0,_tmpA81)))))))),((_tmpA80->tl=0,_tmpA80)))))));}break;case 16:
_LL254:{struct _tuple25*_tmpA84;struct Cyc_List_List*_tmpA83;yyval=Cyc_YY52(((
_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83->hd=((_tmpA84=_cycalloc(sizeof(*
_tmpA84)),((_tmpA84->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpA84->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpA84->f3=0,_tmpA84)))))))),((_tmpA83->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA83)))))));}break;case
17: _LL255:{struct Cyc_Absyn_Fn_d_struct*_tmpA8A;struct Cyc_Absyn_Fn_d_struct
_tmpA89;struct Cyc_List_List*_tmpA88;yyval=Cyc_YY18(((_tmpA88=_cycalloc(sizeof(*
_tmpA88)),((_tmpA88->hd=Cyc_Absyn_new_decl((void*)((_tmpA8A=_cycalloc(sizeof(*
_tmpA8A)),((_tmpA8A[0]=((_tmpA89.tag=1,((_tmpA89.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA89)))),_tmpA8A)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA88->tl=0,
_tmpA88)))))));}break;case 18: _LL256: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 19: _LL257: yyval=Cyc_YY18(0);break;case 20: _LL258:
yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 21:
_LL259:{struct Cyc_Core_Opt*_tmpA8B;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA8B)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 22:
_LL25A: yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 23:
_LL25B:{struct Cyc_Core_Opt*_tmpA8C;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA8C)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 24:
_LL25C:{struct Cyc_Core_Opt*_tmpA8D;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA8D)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 25:
_LL25D:{struct Cyc_Core_Opt*_tmpA8E;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA8E)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 26:
_LL25E: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 27: _LL25F: Cyc_Lex_leave_using();
break;case 28: _LL260:{struct _dyneither_ptr*_tmpA8F;Cyc_Lex_enter_namespace(((
_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA8F)))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 29: _LL261: Cyc_Lex_leave_namespace();
break;case 30: _LL262: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
31: _LL263: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 32:
_LL264:{struct Cyc_List_List*_tmpA90;yyval=Cyc_YY18(((_tmpA90=_cycalloc(sizeof(*
_tmpA90)),((_tmpA90->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA90->tl=0,
_tmpA90)))))));}break;case 33: _LL265: {struct Cyc_List_List*_tmp4DD=0;{struct Cyc_List_List*
_tmp4DE=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
for(0;_tmp4DE != 0;_tmp4DE=_tmp4DE->tl){struct _dyneither_ptr*_tmp4DF=(struct
_dyneither_ptr*)_tmp4DE->hd;struct _tuple1*_tmpA91;struct _tuple1*qv=(_tmpA91=
_cycalloc(sizeof(*_tmpA91)),((_tmpA91->f1=Cyc_Absyn_Rel_n(0),((_tmpA91->f2=
_tmp4DF,_tmpA91)))));struct Cyc_Absyn_Vardecl*_tmp4E0=Cyc_Absyn_new_vardecl(qv,
Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*_tmpA92;_tmp4DD=((_tmpA92=_cycalloc(
sizeof(*_tmpA92)),((_tmpA92->hd=_tmp4E0,((_tmpA92->tl=_tmp4DD,_tmpA92))))));}}
_tmp4DD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4DD);{struct Cyc_List_List*_tmpA93;yyval=Cyc_YY18(((_tmpA93=_cycalloc(sizeof(*
_tmpA93)),((_tmpA93->hd=Cyc_Absyn_letv_decl(_tmp4DD,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA93->tl=0,
_tmpA93)))))));}break;}case 34: _LL266:{const char*_tmpA94;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpA94="`H",_tag_dyneither(_tmpA94,sizeof(char),3))))== 0){
const char*_tmpA97;void*_tmpA96;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA96=0,
Cyc_aprintf(((_tmpA97="bad occurrence of heap region",_tag_dyneither(_tmpA97,
sizeof(char),30))),_tag_dyneither(_tmpA96,sizeof(void*),0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{const char*
_tmpA98;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA98="`U",
_tag_dyneither(_tmpA98,sizeof(char),3))))== 0){const char*_tmpA9B;void*_tmpA9A;
Cyc_Parse_err((struct _dyneither_ptr)((_tmpA9A=0,Cyc_aprintf(((_tmpA9B="bad occurrence of unique region",
_tag_dyneither(_tmpA9B,sizeof(char),32))),_tag_dyneither(_tmpA9A,sizeof(void*),0)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));}}{struct _dyneither_ptr*_tmpA9E;struct Cyc_Absyn_Tvar*
_tmpA9D;struct Cyc_Absyn_Tvar*tv=(_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D->name=((
_tmpA9E=_cycalloc(sizeof(*_tmpA9E)),((_tmpA9E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA9E)))),((_tmpA9D->identity=
- 1,((_tmpA9D->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA9D)))))));
struct Cyc_Absyn_VarType_struct _tmpAA1;struct Cyc_Absyn_VarType_struct*_tmpAA0;
void*t=(void*)((_tmpAA0=_cycalloc(sizeof(*_tmpAA0)),((_tmpAA0[0]=((_tmpAA1.tag=2,((
_tmpAA1.f1=tv,_tmpAA1)))),_tmpAA0))));struct Cyc_Absyn_RgnHandleType_struct
_tmpAA7;struct Cyc_Absyn_RgnHandleType_struct*_tmpAA6;struct _dyneither_ptr*
_tmpAA4;struct _tuple1*_tmpAA3;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3->f1=Cyc_Absyn_Loc_n,((_tmpAA3->f2=((
_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpAA4)))),_tmpAA3)))))),(
void*)((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6[0]=((_tmpAA7.tag=16,((
_tmpAA7.f1=(void*)t,_tmpAA7)))),_tmpAA6)))),0);{struct Cyc_List_List*_tmpAA8;
yyval=Cyc_YY18(((_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAA8->tl=0,_tmpAA8)))))));}break;};case 35:
_LL267:{const char*_tmpAA9;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpAA9="H",
_tag_dyneither(_tmpAA9,sizeof(char),2))))== 0){const char*_tmpAAA;Cyc_Parse_err(((
_tmpAAA="bad occurrence of heap region `H",_tag_dyneither(_tmpAAA,sizeof(char),
33))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));}}{const char*_tmpAAB;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmpAAB="U",_tag_dyneither(_tmpAAB,sizeof(char),2))))== 0){
const char*_tmpAAC;Cyc_Parse_err(((_tmpAAC="bad occurrence of unique region `U",
_tag_dyneither(_tmpAAC,sizeof(char),35))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));}}if(Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){const char*
_tmpAAD;Cyc_Parse_err(((_tmpAAD="open regions cannot be @resetable",
_tag_dyneither(_tmpAAD,sizeof(char),34))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}{struct
_dyneither_ptr*_tmpABA;const char*_tmpAB9;void*_tmpAB8[1];struct Cyc_String_pa_struct
_tmpAB7;struct Cyc_Absyn_Tvar*_tmpAB6;struct Cyc_Absyn_Tvar*tv=(_tmpAB6=_cycalloc(
sizeof(*_tmpAB6)),((_tmpAB6->name=((_tmpABA=_cycalloc(sizeof(*_tmpABA)),((
_tmpABA[0]=(struct _dyneither_ptr)((_tmpAB7.tag=0,((_tmpAB7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpAB8[0]=& _tmpAB7,
Cyc_aprintf(((_tmpAB9="`%s",_tag_dyneither(_tmpAB9,sizeof(char),4))),
_tag_dyneither(_tmpAB8,sizeof(void*),1)))))))),_tmpABA)))),((_tmpAB6->identity=-
1,((_tmpAB6->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAB6)))))));struct
Cyc_Absyn_VarType_struct _tmpABD;struct Cyc_Absyn_VarType_struct*_tmpABC;void*t=(
void*)((_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC[0]=((_tmpABD.tag=2,((
_tmpABD.f1=tv,_tmpABD)))),_tmpABC))));struct Cyc_Absyn_RgnHandleType_struct
_tmpAC3;struct Cyc_Absyn_RgnHandleType_struct*_tmpAC2;struct _dyneither_ptr*
_tmpAC0;struct _tuple1*_tmpABF;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF->f1=Cyc_Absyn_Loc_n,((_tmpABF->f2=((
_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAC0)))),_tmpABF)))))),(
void*)((_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2[0]=((_tmpAC3.tag=16,((
_tmpAC3.f1=(void*)t,_tmpAC3)))),_tmpAC2)))),0);{struct Cyc_List_List*_tmpAC4;
yyval=Cyc_YY18(((_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAC4->tl=0,
_tmpAC4)))))));}break;};case 36: _LL268: {struct Cyc_Absyn_Eq_kb_struct*_tmpACC;
struct Cyc_Absyn_Eq_kb_struct _tmpACB;struct _dyneither_ptr*_tmpACA;struct Cyc_Absyn_Tvar*
_tmpAC9;struct Cyc_Absyn_Tvar*tv=(_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9->name=((
_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpACA)))),((_tmpAC9->identity=
- 1,((_tmpAC9->kind=(void*)((_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC[0]=((
_tmpACB.tag=0,((_tmpACB.f1=& Cyc_Tcutil_rk,_tmpACB)))),_tmpACC)))),_tmpAC9)))))));
struct _dyneither_ptr*_tmpACF;struct _tuple1*_tmpACE;struct Cyc_Absyn_Vardecl*vd=
Cyc_Absyn_new_vardecl(((_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE->f1=Cyc_Absyn_Loc_n,((
_tmpACE->f2=((_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpACF)))),_tmpACE)))))),(
void*)& Cyc_Absyn_VoidType_val,0);{struct Cyc_List_List*_tmpAD0;yyval=Cyc_YY18(((
_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAD0->tl=0,
_tmpAD0)))))));}break;}case 37: _LL269: yyval=Cyc_YY31(0);break;case 38: _LL26A: yyval=
Cyc_YY31(1);break;case 39: _LL26B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 40: _LL26C: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 41: _LL26D:{
struct Cyc_Core_Opt*_tmpAD3;struct Cyc_Parse_Declaration_spec*_tmpAD2;yyval=Cyc_YY19(((
_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2->sc=((_tmpAD3=_cycalloc_atomic(
sizeof(*_tmpAD3)),((_tmpAD3->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAD3)))),((_tmpAD2->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAD2->type_specs=
0,((_tmpAD2->is_inline=0,((_tmpAD2->attributes=0,_tmpAD2)))))))))))));}break;
case 42: _LL26E: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc != 0){const char*_tmpAD6;void*_tmpAD5;(_tmpAD5=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpAD6="Only one storage class is allowed in a declaration",
_tag_dyneither(_tmpAD6,sizeof(char),51))),_tag_dyneither(_tmpAD5,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmpAD9;struct Cyc_Parse_Declaration_spec*_tmpAD8;yyval=Cyc_YY19(((
_tmpAD8=_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8->sc=((_tmpAD9=_cycalloc_atomic(
sizeof(*_tmpAD9)),((_tmpAD9->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpAD9)))),((_tmpAD8->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAD8->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAD8->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpAD8->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,
_tmpAD8)))))))))))));}break;case 43: _LL26F:{struct Cyc_List_List*_tmpADC;struct Cyc_Parse_Declaration_spec*
_tmpADB;yyval=Cyc_YY19(((_tmpADB=_cycalloc(sizeof(*_tmpADB)),((_tmpADB->sc=0,((
_tmpADB->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpADB->type_specs=((
_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpADC->tl=0,_tmpADC)))))),((
_tmpADB->is_inline=0,((_tmpADB->attributes=0,_tmpADB)))))))))))));}break;case 44:
_LL270:{struct Cyc_List_List*_tmpADF;struct Cyc_Parse_Declaration_spec*_tmpADE;
yyval=Cyc_YY19(((_tmpADE=_cycalloc(sizeof(*_tmpADE)),((_tmpADE->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpADE->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpADE->type_specs=((_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpADF->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,_tmpADF)))))),((
_tmpADE->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpADE->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpADE)))))))))))));}
break;case 45: _LL271:{struct Cyc_Parse_Declaration_spec*_tmpAE0;yyval=Cyc_YY19(((
_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0->sc=0,((_tmpAE0->tq=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAE0->type_specs=
0,((_tmpAE0->is_inline=0,((_tmpAE0->attributes=0,_tmpAE0)))))))))))));}break;
case 46: _LL272:{struct Cyc_Parse_Declaration_spec*_tmpAE1;yyval=Cyc_YY19(((_tmpAE1=
_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAE1->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq),((_tmpAE1->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpAE1->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpAE1->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAE1)))))))))))));}
break;case 47: _LL273:{struct Cyc_Parse_Declaration_spec*_tmpAE2;yyval=Cyc_YY19(((
_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2->sc=0,((_tmpAE2->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAE2->type_specs=0,((_tmpAE2->is_inline=1,((
_tmpAE2->attributes=0,_tmpAE2)))))))))))));}break;case 48: _LL274:{struct Cyc_Parse_Declaration_spec*
_tmpAE3;yyval=Cyc_YY19(((_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAE3->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAE3->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAE3->is_inline=1,((_tmpAE3->attributes=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAE3)))))))))))));}
break;case 49: _LL275:{struct Cyc_Parse_Declaration_spec*_tmpAE4;yyval=Cyc_YY19(((
_tmpAE4=_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4->sc=0,((_tmpAE4->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAE4->type_specs=0,((_tmpAE4->is_inline=0,((
_tmpAE4->attributes=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpAE4)))))))))))));}break;case 50: _LL276:{struct Cyc_Parse_Declaration_spec*
_tmpAE5;yyval=Cyc_YY19(((_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAE5->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAE5->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAE5->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpAE5->attributes=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes),_tmpAE5)))))))))))));}
break;case 51: _LL277: yyval=Cyc_YY22(Cyc_Parse_Auto_sc);break;case 52: _LL278: yyval=
Cyc_YY22(Cyc_Parse_Register_sc);break;case 53: _LL279: yyval=Cyc_YY22(Cyc_Parse_Static_sc);
break;case 54: _LL27A: yyval=Cyc_YY22(Cyc_Parse_Extern_sc);break;case 55: _LL27B:{
const char*_tmpAE6;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAE6="C",
_tag_dyneither(_tmpAE6,sizeof(char),2))))!= 0){const char*_tmpAE7;Cyc_Parse_err(((
_tmpAE7="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAE7,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}}yyval=Cyc_YY22(Cyc_Parse_ExternC_sc);break;case 56:
_LL27C: yyval=Cyc_YY22(Cyc_Parse_Typedef_sc);break;case 57: _LL27D: yyval=Cyc_YY22(
Cyc_Parse_Abstract_sc);break;case 58: _LL27E: yyval=Cyc_YY45(0);break;case 59: _LL27F:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 60:
_LL280: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 61: _LL281:{struct Cyc_List_List*_tmpAE8;yyval=Cyc_YY45(((_tmpAE8=_cycalloc(
sizeof(*_tmpAE8)),((_tmpAE8->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAE8->tl=0,_tmpAE8)))))));}
break;case 62: _LL282:{struct Cyc_List_List*_tmpAE9;yyval=Cyc_YY45(((_tmpAE9=
_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpAE9->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAE9)))))));}break;
case 63: _LL283: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={6,- 1};
static struct _tuple26 att_map[17]={{{_tmp521,_tmp521,_tmp521 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{
_tmp522,_tmp522,_tmp522 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp523,_tmp523,
_tmp523 + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp524,_tmp524,_tmp524 + 9},(
void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp525,_tmp525,_tmp525 + 6},(void*)& Cyc_Absyn_Const_att_val},{{
_tmp526,_tmp526,_tmp526 + 8},(void*)& att_aligned},{{_tmp527,_tmp527,_tmp527 + 7},(
void*)& Cyc_Absyn_Packed_att_val},{{_tmp528,_tmp528,_tmp528 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{
_tmp529,_tmp529,_tmp529 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp52A,_tmp52A,
_tmp52A + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp52B,_tmp52B,_tmp52B + 10},(void*)&
Cyc_Absyn_Dllimport_att_val},{{_tmp52C,_tmp52C,_tmp52C + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{
_tmp52D,_tmp52D,_tmp52D + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{
_tmp52E,_tmp52E,_tmp52E + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp52F,
_tmp52F,_tmp52F + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp530,_tmp530,
_tmp530 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp531,_tmp531,
_tmp531 + 5},(void*)& Cyc_Absyn_Pure_att_val}};struct _dyneither_ptr _tmp51F=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp51F,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp51F,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp51F,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp51F,sizeof(char),(int)(_get_dyneither_size(
_tmp51F,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp51F,sizeof(char),(int)(_get_dyneither_size(_tmp51F,sizeof(char))- 3)))== '_')
_tmp51F=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp51F,2,
_get_dyneither_size(_tmp51F,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp51F,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){{const char*_tmpAEA;Cyc_Parse_err(((
_tmpAEA="unrecognized attribute",_tag_dyneither(_tmpAEA,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=Cyc_YY46((
void*)& Cyc_Absyn_Cdecl_att_val);}break;};}case 64: _LL284: yyval=Cyc_YY46((void*)&
Cyc_Absyn_Const_att_val);break;case 65: _LL285: {struct _dyneither_ptr _tmp533=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp535;struct
_tuple6 _tmp534=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp535=_tmp534.f2;{void*a;{const char*_tmpAEC;const char*
_tmpAEB;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp533,((_tmpAEC="regparm",
_tag_dyneither(_tmpAEC,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp533,((_tmpAEB="__regparm__",_tag_dyneither(_tmpAEB,sizeof(
char),12))))== 0){if(_tmp535 < 0  || _tmp535 > 3){const char*_tmpAED;Cyc_Parse_err(((
_tmpAED="regparm requires value between 0 and 3",_tag_dyneither(_tmpAED,sizeof(
char),39))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Regparm_att_struct _tmpAF0;
struct Cyc_Absyn_Regparm_att_struct*_tmpAEF;a=(void*)((_tmpAEF=_cycalloc_atomic(
sizeof(*_tmpAEF)),((_tmpAEF[0]=((_tmpAF0.tag=0,((_tmpAF0.f1=_tmp535,_tmpAF0)))),
_tmpAEF))));};}else{const char*_tmpAF2;const char*_tmpAF1;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp533,((_tmpAF2="aligned",_tag_dyneither(_tmpAF2,sizeof(char),8))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp533,((_tmpAF1="__aligned__",
_tag_dyneither(_tmpAF1,sizeof(char),12))))== 0){if(_tmp535 < 0){const char*_tmpAF3;
Cyc_Parse_err(((_tmpAF3="aligned requires positive power of two",_tag_dyneither(
_tmpAF3,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp535;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
const char*_tmpAF4;Cyc_Parse_err(((_tmpAF4="aligned requires positive power of two",
_tag_dyneither(_tmpAF4,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmpAF7;struct Cyc_Absyn_Aligned_att_struct*_tmpAF6;a=(void*)((_tmpAF6=
_cycalloc_atomic(sizeof(*_tmpAF6)),((_tmpAF6[0]=((_tmpAF7.tag=6,((_tmpAF7.f1=
_tmp535,_tmpAF7)))),_tmpAF6))));};};}else{const char*_tmpAF9;const char*_tmpAF8;
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp533,((_tmpAF9="initializes",
_tag_dyneither(_tmpAF9,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp533,((_tmpAF8="__initializes__",_tag_dyneither(_tmpAF8,
sizeof(char),16))))== 0){struct Cyc_Absyn_Initializes_att_struct _tmpAFC;struct Cyc_Absyn_Initializes_att_struct*
_tmpAFB;a=(void*)((_tmpAFB=_cycalloc_atomic(sizeof(*_tmpAFB)),((_tmpAFB[0]=((
_tmpAFC.tag=20,((_tmpAFC.f1=_tmp535,_tmpAFC)))),_tmpAFB))));}else{const char*
_tmpAFE;const char*_tmpAFD;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp533,((_tmpAFE="noliveunique",
_tag_dyneither(_tmpAFE,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp533,((_tmpAFD="__noliveunique__",_tag_dyneither(_tmpAFD,
sizeof(char),17))))== 0){struct Cyc_Absyn_Noliveunique_att_struct _tmpB01;struct
Cyc_Absyn_Noliveunique_att_struct*_tmpB00;a=(void*)((_tmpB00=_cycalloc_atomic(
sizeof(*_tmpB00)),((_tmpB00[0]=((_tmpB01.tag=21,((_tmpB01.f1=_tmp535,_tmpB01)))),
_tmpB00))));}else{{const char*_tmpB02;Cyc_Parse_err(((_tmpB02="unrecognized attribute",
_tag_dyneither(_tmpB02,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}}}}yyval=Cyc_YY46(a);break;};}case 66: _LL286: {struct
_dyneither_ptr _tmp54A=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp54B=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpB04;const char*_tmpB03;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54A,((_tmpB04="section",
_tag_dyneither(_tmpB04,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp54A,((_tmpB03="__section__",_tag_dyneither(_tmpB03,sizeof(
char),12))))== 0){struct Cyc_Absyn_Section_att_struct _tmpB07;struct Cyc_Absyn_Section_att_struct*
_tmpB06;a=(void*)((_tmpB06=_cycalloc(sizeof(*_tmpB06)),((_tmpB06[0]=((_tmpB07.tag=
8,((_tmpB07.f1=_tmp54B,_tmpB07)))),_tmpB06))));}else{{const char*_tmpB08;Cyc_Parse_err(((
_tmpB08="unrecognized attribute",_tag_dyneither(_tmpB08,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 67: _LL287: {struct
_dyneither_ptr _tmp551=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp552=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpB09;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp551,((_tmpB09="__mode__",
_tag_dyneither(_tmpB09,sizeof(char),9))))== 0){struct Cyc_Absyn_Mode_att_struct
_tmpB0C;struct Cyc_Absyn_Mode_att_struct*_tmpB0B;a=(void*)((_tmpB0B=_cycalloc(
sizeof(*_tmpB0B)),((_tmpB0B[0]=((_tmpB0C.tag=23,((_tmpB0C.f1=_tmp552,_tmpB0C)))),
_tmpB0B))));}else{{const char*_tmpB0D;Cyc_Parse_err(((_tmpB0D="unrecognized attribute",
_tag_dyneither(_tmpB0D,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 68: _LL288: {struct
_dyneither_ptr _tmp557=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp558=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp55A;struct _tuple6
_tmp559=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp55A=_tmp559.f2;{int _tmp55C;struct _tuple6 _tmp55B=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp55C=_tmp55B.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;{const char*_tmpB0F;const char*_tmpB0E;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp557,((_tmpB0F="format",_tag_dyneither(
_tmpB0F,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp557,((
_tmpB0E="__format__",_tag_dyneither(_tmpB0E,sizeof(char),11))))== 0){const char*
_tmpB10;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp558,((_tmpB10="printf",
_tag_dyneither(_tmpB10,sizeof(char),7))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpB13;struct Cyc_Absyn_Format_att_struct*_tmpB12;a=(void*)((_tmpB12=
_cycalloc_atomic(sizeof(*_tmpB12)),((_tmpB12[0]=((_tmpB13.tag=19,((_tmpB13.f1=
Cyc_Absyn_Printf_ft,((_tmpB13.f2=_tmp55A,((_tmpB13.f3=_tmp55C,_tmpB13)))))))),
_tmpB12))));}else{const char*_tmpB14;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp558,((
_tmpB14="scanf",_tag_dyneither(_tmpB14,sizeof(char),6))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpB17;struct Cyc_Absyn_Format_att_struct*_tmpB16;a=(void*)((_tmpB16=
_cycalloc_atomic(sizeof(*_tmpB16)),((_tmpB16[0]=((_tmpB17.tag=19,((_tmpB17.f1=
Cyc_Absyn_Scanf_ft,((_tmpB17.f2=_tmp55A,((_tmpB17.f3=_tmp55C,_tmpB17)))))))),
_tmpB16))));}else{const char*_tmpB18;Cyc_Parse_err(((_tmpB18="unrecognized format type",
_tag_dyneither(_tmpB18,sizeof(char),25))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{
const char*_tmpB19;Cyc_Parse_err(((_tmpB19="unrecognized attribute",
_tag_dyneither(_tmpB19,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY46(
a);break;};};}case 69: _LL289: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 70: _LL28A:{struct Cyc_Absyn_TypedefType_struct _tmpB1C;
struct Cyc_Absyn_TypedefType_struct*_tmpB1B;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B[0]=((_tmpB1C.tag=18,((
_tmpB1C.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB1C.f2=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB1C.f3=0,((_tmpB1C.f4=
0,_tmpB1C)))))))))),_tmpB1B)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 71:
_LL28B: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 72:
_LL28C: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 73:
_LL28D: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 74:
_LL28E: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 75:
_LL28F:{struct Cyc_Parse_Short_spec_struct _tmpB1F;struct Cyc_Parse_Short_spec_struct*
_tmpB1E;yyval=Cyc_YY23((void*)((_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E[0]=((
_tmpB1F.tag=2,((_tmpB1F.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB1F)))),
_tmpB1E)))));}break;case 76: _LL290: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 77: _LL291:{struct Cyc_Parse_Long_spec_struct
_tmpB22;struct Cyc_Parse_Long_spec_struct*_tmpB21;yyval=Cyc_YY23((void*)((_tmpB21=
_cycalloc(sizeof(*_tmpB21)),((_tmpB21[0]=((_tmpB22.tag=3,((_tmpB22.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB22)))),
_tmpB21)))));}break;case 78: _LL292: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 79: _LL293: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 80:
_LL294:{struct Cyc_Parse_Signed_spec_struct _tmpB25;struct Cyc_Parse_Signed_spec_struct*
_tmpB24;yyval=Cyc_YY23((void*)((_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24[0]=((
_tmpB25.tag=0,((_tmpB25.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB25)))),
_tmpB24)))));}break;case 81: _LL295:{struct Cyc_Parse_Unsigned_spec_struct _tmpB28;
struct Cyc_Parse_Unsigned_spec_struct*_tmpB27;yyval=Cyc_YY23((void*)((_tmpB27=
_cycalloc(sizeof(*_tmpB27)),((_tmpB27[0]=((_tmpB28.tag=1,((_tmpB28.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB28)))),
_tmpB27)))));}break;case 82: _LL296: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 83: _LL297: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84: _LL298: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 85: _LL299:
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 86:
_LL29A:{struct Cyc_Absyn_TupleType_struct _tmpB2B;struct Cyc_Absyn_TupleType_struct*
_tmpB2A;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB2A=_cycalloc(sizeof(*
_tmpB2A)),((_tmpB2A[0]=((_tmpB2B.tag=11,((_tmpB2B.f1=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))),_tmpB2B)))),_tmpB2A)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 87: _LL29B:{struct Cyc_Absyn_RgnHandleType_struct
_tmpB2E;struct Cyc_Absyn_RgnHandleType_struct*_tmpB2D;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB2D=_cycalloc(sizeof(*_tmpB2D)),((_tmpB2D[0]=((_tmpB2E.tag=16,((
_tmpB2E.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpB2E)))),_tmpB2D)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 88:
_LL29C:{struct Cyc_Absyn_RgnHandleType_struct _tmpB31;struct Cyc_Absyn_RgnHandleType_struct*
_tmpB30;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB30=_cycalloc(sizeof(*
_tmpB30)),((_tmpB30[0]=((_tmpB31.tag=16,((_tmpB31.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0),_tmpB31)))),_tmpB30)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 89:
_LL29D: {void*_tmp577=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0);{
struct Cyc_Absyn_DynRgnType_struct _tmpB34;struct Cyc_Absyn_DynRgnType_struct*
_tmpB33;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB33=_cycalloc(sizeof(*
_tmpB33)),((_tmpB33[0]=((_tmpB34.tag=17,((_tmpB34.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB34.f2=(void*)
_tmp577,_tmpB34)))))),_tmpB33)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case 90:
_LL29E:{struct Cyc_Absyn_DynRgnType_struct _tmpB37;struct Cyc_Absyn_DynRgnType_struct*
_tmpB36;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB36=_cycalloc(sizeof(*
_tmpB36)),((_tmpB36[0]=((_tmpB37.tag=17,((_tmpB37.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB37.f2=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB37)))))),
_tmpB36)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 91: _LL29F:{struct Cyc_Absyn_TagType_struct
_tmpB3A;struct Cyc_Absyn_TagType_struct*_tmpB39;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB39=_cycalloc(sizeof(*_tmpB39)),((_tmpB39[0]=((_tmpB3A.tag=20,((
_tmpB3A.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpB3A)))),_tmpB39)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 92:
_LL2A0:{struct Cyc_Absyn_TagType_struct _tmpB3D;struct Cyc_Absyn_TagType_struct*
_tmpB3C;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB3C=_cycalloc(sizeof(*
_tmpB3C)),((_tmpB3C[0]=((_tmpB3D.tag=20,((_tmpB3D.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_iko,0),_tmpB3D)))),_tmpB3C)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 93:
_LL2A1:{struct Cyc_Absyn_ValueofType_struct _tmpB40;struct Cyc_Absyn_ValueofType_struct*
_tmpB3F;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB3F=_cycalloc(sizeof(*
_tmpB3F)),((_tmpB3F[0]=((_tmpB40.tag=19,((_tmpB40.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB40)))),_tmpB3F)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 94: _LL2A2: yyval=Cyc_YY43(Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 95: _LL2A3: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;{struct Cyc_Absyn_Tqual
_tmpB41;yyval=Cyc_YY25(((_tmpB41.print_const=1,((_tmpB41.q_volatile=0,((_tmpB41.q_restrict=
0,((_tmpB41.real_const=1,((_tmpB41.loc=loc,_tmpB41)))))))))));}break;}case 96:
_LL2A4:{struct Cyc_Absyn_Tqual _tmpB42;yyval=Cyc_YY25(((_tmpB42.print_const=0,((
_tmpB42.q_volatile=1,((_tmpB42.q_restrict=0,((_tmpB42.real_const=0,((_tmpB42.loc=
0,_tmpB42)))))))))));}break;case 97: _LL2A5:{struct Cyc_Absyn_Tqual _tmpB43;yyval=
Cyc_YY25(((_tmpB43.print_const=0,((_tmpB43.q_volatile=0,((_tmpB43.q_restrict=1,((
_tmpB43.real_const=0,((_tmpB43.loc=0,_tmpB43)))))))))));}break;case 98: _LL2A6:{
struct Cyc_Parse_Decl_spec_struct _tmpB5F;struct Cyc_Absyn_Enum_d_struct*_tmpB5E;
struct Cyc_Absyn_Enumdecl*_tmpB5D;struct Cyc_Core_Opt*_tmpB5C;struct Cyc_Absyn_Enum_d_struct
_tmpB5B;struct Cyc_Absyn_Decl*_tmpB5A;struct Cyc_Parse_Decl_spec_struct*_tmpB59;
yyval=Cyc_YY23((void*)((_tmpB59=_cycalloc(sizeof(*_tmpB59)),((_tmpB59[0]=((
_tmpB5F.tag=5,((_tmpB5F.f1=((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A->r=(
void*)((_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((_tmpB5E[0]=((_tmpB5B.tag=8,((
_tmpB5B.f1=((_tmpB5D=_cycalloc(sizeof(*_tmpB5D)),((_tmpB5D->sc=Cyc_Absyn_Public,((
_tmpB5D->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpB5D->fields=((_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((
_tmpB5C->v=Cyc_yyget_YY48(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),_tmpB5C)))),_tmpB5D)))))))),_tmpB5B)))),_tmpB5E)))),((_tmpB5A->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB5A)))))),
_tmpB5F)))),_tmpB59)))));}break;case 99: _LL2A7:{struct Cyc_Absyn_EnumType_struct
_tmpB62;struct Cyc_Absyn_EnumType_struct*_tmpB61;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB61=_cycalloc(sizeof(*_tmpB61)),((_tmpB61[0]=((_tmpB62.tag=14,((
_tmpB62.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB62.f2=0,_tmpB62)))))),_tmpB61)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 100:
_LL2A8:{struct Cyc_Parse_Type_spec_struct _tmpB6C;struct Cyc_Absyn_AnonEnumType_struct
_tmpB6B;struct Cyc_Absyn_AnonEnumType_struct*_tmpB6A;struct Cyc_Parse_Type_spec_struct*
_tmpB69;yyval=Cyc_YY23((void*)((_tmpB69=_cycalloc(sizeof(*_tmpB69)),((_tmpB69[0]=((
_tmpB6C.tag=4,((_tmpB6C.f1=(void*)((void*)((_tmpB6A=_cycalloc(sizeof(*_tmpB6A)),((
_tmpB6A[0]=((_tmpB6B.tag=15,((_tmpB6B.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB6B)))),_tmpB6A))))),((
_tmpB6C.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB6C)))))),_tmpB69)))));}break;case 101: _LL2A9:{
struct Cyc_Absyn_Enumfield*_tmpB6D;yyval=Cyc_YY47(((_tmpB6D=_cycalloc(sizeof(*
_tmpB6D)),((_tmpB6D->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB6D->tag=0,((_tmpB6D->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB6D)))))))));}break;case 102: _LL2AA:{struct Cyc_Absyn_Enumfield*
_tmpB6E;yyval=Cyc_YY47(((_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((_tmpB6E->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB6E->tag=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB6E->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB6E)))))))));}
break;case 103: _LL2AB:{struct Cyc_List_List*_tmpB6F;yyval=Cyc_YY48(((_tmpB6F=
_cycalloc(sizeof(*_tmpB6F)),((_tmpB6F->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB6F->tl=0,_tmpB6F)))))));}
break;case 104: _LL2AC:{struct Cyc_List_List*_tmpB70;yyval=Cyc_YY48(((_tmpB70=
_cycalloc(sizeof(*_tmpB70)),((_tmpB70->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB70->tl=0,_tmpB70)))))));}
break;case 105: _LL2AD:{struct Cyc_List_List*_tmpB71;yyval=Cyc_YY48(((_tmpB71=
_cycalloc(sizeof(*_tmpB71)),((_tmpB71->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB71->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB71)))))));}break;
case 106: _LL2AE:{struct Cyc_Absyn_AnonAggrType_struct _tmpB74;struct Cyc_Absyn_AnonAggrType_struct*
_tmpB73;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB73=_cycalloc(sizeof(*
_tmpB73)),((_tmpB73[0]=((_tmpB74.tag=13,((_tmpB74.f1=Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB74.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB74)))))),
_tmpB73)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 107: _LL2AF: {struct Cyc_List_List*_tmp599=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp59A=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct
_tmpB77;struct Cyc_Parse_Decl_spec_struct*_tmpB76;yyval=Cyc_YY23((void*)((_tmpB76=
_cycalloc(sizeof(*_tmpB76)),((_tmpB76[0]=((_tmpB77.tag=5,((_tmpB77.f1=Cyc_Absyn_aggr_decl(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tmp599,(struct Cyc_Absyn_AggrdeclImpl*)Cyc_Absyn_aggrdecl_impl(_tmp59A,Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB77)))),
_tmpB76)))));}break;}case 108: _LL2B0: {struct Cyc_List_List*_tmp59D=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp59E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct _tmpB7A;struct Cyc_Parse_Decl_spec_struct*
_tmpB79;yyval=Cyc_YY23((void*)((_tmpB79=_cycalloc(sizeof(*_tmpB79)),((_tmpB79[0]=((
_tmpB7A.tag=5,((_tmpB7A.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp59D,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp59E,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB7A)))),
_tmpB79)))));}break;}case 109: _LL2B1:{struct Cyc_Absyn_AggrType_struct _tmpB84;
struct Cyc_Core_Opt*_tmpB83;struct Cyc_Absyn_AggrInfo _tmpB82;struct Cyc_Absyn_AggrType_struct*
_tmpB81;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB81=_cycalloc(sizeof(*
_tmpB81)),((_tmpB81[0]=((_tmpB84.tag=12,((_tmpB84.f1=((_tmpB82.aggr_info=Cyc_Absyn_UnknownAggr(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB83=
_cycalloc_atomic(sizeof(*_tmpB83)),((_tmpB83->v=(void*)1,_tmpB83))))),((_tmpB82.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB82)))),
_tmpB84)))),_tmpB81)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 110:
_LL2B2:{struct Cyc_Absyn_AggrType_struct _tmpB8A;struct Cyc_Absyn_AggrInfo _tmpB89;
struct Cyc_Absyn_AggrType_struct*_tmpB88;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((
_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88[0]=((_tmpB8A.tag=12,((_tmpB8A.f1=((
_tmpB89.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),((_tmpB89.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB89)))),
_tmpB8A)))),_tmpB88)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 111:
_LL2B3: yyval=Cyc_YY40(0);break;case 112: _LL2B4: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 113: _LL2B5:
yyval=Cyc_YY24(Cyc_Absyn_StructA);break;case 114: _LL2B6: yyval=Cyc_YY24(Cyc_Absyn_UnionA);
break;case 115: _LL2B7: yyval=Cyc_YY26(0);break;case 116: _LL2B8: {struct Cyc_List_List*
_tmp5A8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));struct Cyc_List_List*
_tmp5A9=Cyc_Parse_get_argrfield_tags(_tmp5A8);if(_tmp5A9 != 0)((void(*)(void(*f)(
struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct
Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp5A9,
_tmp5A8);yyval=Cyc_YY26(_tmp5A8);break;}case 117: _LL2B9:{struct Cyc_List_List*
_tmpB8B;yyval=Cyc_YY27(((_tmpB8B=_cycalloc(sizeof(*_tmpB8B)),((_tmpB8B->hd=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB8B->tl=0,
_tmpB8B)))))));}break;case 118: _LL2BA:{struct Cyc_List_List*_tmpB8C;yyval=Cyc_YY27(((
_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB8C->tl=Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB8C)))))));}
break;case 119: _LL2BB: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 120: _LL2BC:{struct Cyc_List_List*_tmpB8D;yyval=Cyc_YY21(((
_tmpB8D=_cycalloc(sizeof(*_tmpB8D)),((_tmpB8D->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB8D->tl=0,_tmpB8D)))))));}
break;case 121: _LL2BD:{struct Cyc_List_List*_tmpB8E;yyval=Cyc_YY21(((_tmpB8E=
_cycalloc(sizeof(*_tmpB8E)),((_tmpB8E->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB8E->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB8E)))))));}
break;case 122: _LL2BE:{struct _tuple22*_tmpB8F;yyval=Cyc_YY20(((_tmpB8F=_cycalloc(
sizeof(*_tmpB8F)),((_tmpB8F->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB8F->f2=0,_tmpB8F)))))));}
break;case 123: _LL2BF:{struct _tuple22*_tmpB90;yyval=Cyc_YY20(((_tmpB90=_cycalloc(
sizeof(*_tmpB90)),((_tmpB90->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB90->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB90)))))));}
break;case 124: _LL2C0: {struct _RegionHandle _tmp5B0=_new_region("temp");struct
_RegionHandle*temp=& _tmp5B0;_push_region(temp);{struct _tuple23 _tmp5B2;struct Cyc_Absyn_Tqual
_tmp5B3;struct Cyc_List_List*_tmp5B4;struct Cyc_List_List*_tmp5B5;struct _tuple23*
_tmp5B1=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5B2=*_tmp5B1;_tmp5B3=_tmp5B2.f1;_tmp5B4=_tmp5B2.f2;_tmp5B5=_tmp5B2.f3;if(
_tmp5B3.loc == 0)_tmp5B3.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);{struct Cyc_List_List*
_tmp5B7;struct Cyc_List_List*_tmp5B8;struct _tuple0 _tmp5B6=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
temp,temp,Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]));_tmp5B7=_tmp5B6.f1;_tmp5B8=_tmp5B6.f2;{void*_tmp5B9=Cyc_Parse_speclist2typ(
_tmp5B4,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp5BA=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(temp,_tmp5B3,_tmp5B9,_tmp5B7,
_tmp5B5),_tmp5B8);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple14*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp5BA));};};}
_npop_handler(0);break;;_pop_region(temp);}case 125: _LL2C1:{struct Cyc_List_List*
_tmpB93;struct _tuple23*_tmpB92;yyval=Cyc_YY35(((_tmpB92=_cycalloc(sizeof(*
_tmpB92)),((_tmpB92->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB92->f2=((
_tmpB93=_cycalloc(sizeof(*_tmpB93)),((_tmpB93->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB93->tl=0,_tmpB93)))))),((
_tmpB92->f3=0,_tmpB92)))))))));}break;case 126: _LL2C2:{struct Cyc_List_List*
_tmpB96;struct _tuple23*_tmpB95;yyval=Cyc_YY35(((_tmpB95=_cycalloc(sizeof(*
_tmpB95)),((_tmpB95->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpB95->f2=((_tmpB96=_cycalloc(sizeof(*_tmpB96)),((
_tmpB96->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB96->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpB96)))))),((_tmpB95->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB95)))))))));}break;case 127: _LL2C3:{struct _tuple23*_tmpB97;yyval=Cyc_YY35(((
_tmpB97=_cycalloc(sizeof(*_tmpB97)),((_tmpB97->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB97->f2=0,((_tmpB97->f3=
0,_tmpB97)))))))));}break;case 128: _LL2C4:{struct _tuple23*_tmpB98;yyval=Cyc_YY35(((
_tmpB98=_cycalloc(sizeof(*_tmpB98)),((_tmpB98->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpB98->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpB98->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpB98)))))))));}break;case 129: _LL2C5:{struct _tuple23*
_tmpB99;yyval=Cyc_YY35(((_tmpB99=_cycalloc(sizeof(*_tmpB99)),((_tmpB99->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB99->f2=0,((_tmpB99->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB99)))))))));}break;
case 130: _LL2C6:{struct _tuple23*_tmpB9A;yyval=Cyc_YY35(((_tmpB9A=_cycalloc(
sizeof(*_tmpB9A)),((_tmpB9A->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpB9A->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpB9A->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpB9A)))))))));}break;case 131: _LL2C7:{struct Cyc_List_List*_tmpB9D;struct
_tuple23*_tmpB9C;yyval=Cyc_YY35(((_tmpB9C=_cycalloc(sizeof(*_tmpB9C)),((_tmpB9C->f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB9C->f2=((
_tmpB9D=_cycalloc(sizeof(*_tmpB9D)),((_tmpB9D->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB9D->tl=0,_tmpB9D)))))),((
_tmpB9C->f3=0,_tmpB9C)))))))));}break;case 132: _LL2C8:{struct Cyc_List_List*
_tmpBA0;struct _tuple23*_tmpB9F;yyval=Cyc_YY35(((_tmpB9F=_cycalloc(sizeof(*
_tmpB9F)),((_tmpB9F->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpB9F->f2=((_tmpBA0=_cycalloc(sizeof(*_tmpBA0)),((
_tmpBA0->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpBA0->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpBA0)))))),((_tmpB9F->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB9F)))))))));}break;case 133: _LL2C9:{struct _tuple23*_tmpBA1;yyval=Cyc_YY35(((
_tmpBA1=_cycalloc(sizeof(*_tmpBA1)),((_tmpBA1->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBA1->f2=0,((_tmpBA1->f3=
0,_tmpBA1)))))))));}break;case 134: _LL2CA:{struct _tuple23*_tmpBA2;yyval=Cyc_YY35(((
_tmpBA2=_cycalloc(sizeof(*_tmpBA2)),((_tmpBA2->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpBA2->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpBA2->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpBA2)))))))));}break;case 135: _LL2CB:{struct _tuple23*
_tmpBA3;yyval=Cyc_YY35(((_tmpBA3=_cycalloc(sizeof(*_tmpBA3)),((_tmpBA3->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpBA3->f2=0,((_tmpBA3->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBA3)))))))));}break;
case 136: _LL2CC:{struct _tuple23*_tmpBA4;yyval=Cyc_YY35(((_tmpBA4=_cycalloc(
sizeof(*_tmpBA4)),((_tmpBA4->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpBA4->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpBA4->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpBA4)))))))));}break;case 137: _LL2CD: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 138: _LL2CE:{
struct Cyc_List_List*_tmpBA5;yyval=Cyc_YY21(((_tmpBA5=_cycalloc(sizeof(*_tmpBA5)),((
_tmpBA5->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpBA5->tl=0,_tmpBA5)))))));}break;case 139: _LL2CF:{struct Cyc_List_List*
_tmpBA6;yyval=Cyc_YY21(((_tmpBA6=_cycalloc(sizeof(*_tmpBA6)),((_tmpBA6->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBA6->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpBA6)))))));}
break;case 140: _LL2D0:{struct _tuple22*_tmpBA7;yyval=Cyc_YY20(((_tmpBA7=_cycalloc(
sizeof(*_tmpBA7)),((_tmpBA7->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBA7->f2=0,_tmpBA7)))))));}
break;case 141: _LL2D1:{struct Cyc_Parse_Declarator*_tmpBAD;struct _tuple1*_tmpBAC;
struct _tuple22*_tmpBAB;yyval=Cyc_YY20(((_tmpBAB=_cycalloc(sizeof(*_tmpBAB)),((
_tmpBAB->f1=((_tmpBAD=_cycalloc(sizeof(*_tmpBAD)),((_tmpBAD->id=((_tmpBAC=
_cycalloc(sizeof(*_tmpBAC)),((_tmpBAC->f1=Cyc_Absyn_Rel_n(0),((_tmpBAC->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),
_tmpBAC)))))),((_tmpBAD->tms=0,_tmpBAD)))))),((_tmpBAB->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBAB)))))));}
break;case 142: _LL2D2:{struct _tuple22*_tmpBAE;yyval=Cyc_YY20(((_tmpBAE=_cycalloc(
sizeof(*_tmpBAE)),((_tmpBAE->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBAE->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBAE)))))));}
break;case 143: _LL2D3: {int _tmp5D3=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);struct Cyc_List_List*
_tmp5D4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct _tmpBB4;struct Cyc_Core_Opt*
_tmpBB3;struct Cyc_Parse_Decl_spec_struct*_tmpBB2;yyval=Cyc_YY23((void*)((_tmpBB2=
_cycalloc(sizeof(*_tmpBB2)),((_tmpBB2[0]=((_tmpBB4.tag=5,((_tmpBB4.f1=Cyc_Absyn_datatype_decl(
Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),_tmp5D4,((_tmpBB3=_cycalloc(sizeof(*_tmpBB3)),((_tmpBB3->v=
Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpBB3)))),_tmp5D3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBB4)))),
_tmpBB2)))));}break;}case 144: _LL2D4: {int _tmp5D8=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);{struct Cyc_Absyn_DatatypeType_struct
_tmpBBE;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBBD;struct Cyc_Absyn_DatatypeInfo
_tmpBBC;struct Cyc_Absyn_DatatypeType_struct*_tmpBBB;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpBBB=_cycalloc(sizeof(*_tmpBBB)),((_tmpBBB[0]=((_tmpBBE.tag=3,((
_tmpBBE.f1=((_tmpBBC.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpBBD.name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBBD.is_extensible=
_tmp5D8,_tmpBBD))))),((_tmpBBC.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBBC)))),_tmpBBE)))),
_tmpBBB)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 145: _LL2D5: {int _tmp5DD=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);{struct Cyc_Absyn_DatatypeFieldType_struct
_tmpBC8;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBC7;struct Cyc_Absyn_DatatypeFieldInfo
_tmpBC6;struct Cyc_Absyn_DatatypeFieldType_struct*_tmpBC5;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpBC5=_cycalloc(sizeof(*_tmpBC5)),((_tmpBC5[0]=((_tmpBC8.tag=4,((
_tmpBC8.f1=((_tmpBC6.field_info=Cyc_Absyn_UnknownDatatypefield(((_tmpBC7.datatype_name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpBC7.field_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpBC7.is_extensible=_tmp5DD,_tmpBC7))))))),((
_tmpBC6.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpBC6)))),_tmpBC8)))),_tmpBC5)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
146: _LL2D6: yyval=Cyc_YY31(0);break;case 147: _LL2D7: yyval=Cyc_YY31(1);break;case
148: _LL2D8:{struct Cyc_List_List*_tmpBC9;yyval=Cyc_YY34(((_tmpBC9=_cycalloc(
sizeof(*_tmpBC9)),((_tmpBC9->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBC9->tl=0,_tmpBC9)))))));}
break;case 149: _LL2D9:{struct Cyc_List_List*_tmpBCA;yyval=Cyc_YY34(((_tmpBCA=
_cycalloc(sizeof(*_tmpBCA)),((_tmpBCA->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBCA->tl=0,_tmpBCA)))))));}
break;case 150: _LL2DA:{struct Cyc_List_List*_tmpBCB;yyval=Cyc_YY34(((_tmpBCB=
_cycalloc(sizeof(*_tmpBCB)),((_tmpBCB->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBCB->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBCB)))))));}break;
case 151: _LL2DB:{struct Cyc_List_List*_tmpBCC;yyval=Cyc_YY34(((_tmpBCC=_cycalloc(
sizeof(*_tmpBCC)),((_tmpBCC->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBCC->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBCC)))))));}break;
case 152: _LL2DC: yyval=Cyc_YY32(Cyc_Absyn_Public);break;case 153: _LL2DD: yyval=Cyc_YY32(
Cyc_Absyn_Extern);break;case 154: _LL2DE: yyval=Cyc_YY32(Cyc_Absyn_Static);break;
case 155: _LL2DF:{struct Cyc_Absyn_Datatypefield*_tmpBCD;yyval=Cyc_YY33(((_tmpBCD=
_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBCD->typs=0,((_tmpBCD->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpBCD->sc=Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBCD)))))))))));}
break;case 156: _LL2E0: {struct Cyc_List_List*_tmp5E7=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Absyn_Datatypefield*
_tmpBCE;yyval=Cyc_YY33(((_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpBCE->typs=
_tmp5E7,((_tmpBCE->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpBCE->sc=
Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmpBCE)))))))))));}break;}case 157: _LL2E1: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 158: _LL2E2:{struct
Cyc_Parse_Declarator*_tmpBCF;yyval=Cyc_YY29(((_tmpBCF=_cycalloc(sizeof(*_tmpBCF)),((
_tmpBCF->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id,((_tmpBCF->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpBCF)))))));}
break;case 159: _LL2E3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 160: _LL2E4:{struct Cyc_Parse_Declarator*_tmpBD0;yyval=
Cyc_YY29(((_tmpBD0=_cycalloc(sizeof(*_tmpBD0)),((_tmpBD0->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->id,((_tmpBD0->tms=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpBD0)))))));}
break;case 161: _LL2E5:{struct Cyc_Parse_Declarator*_tmpBD1;yyval=Cyc_YY29(((
_tmpBD1=_cycalloc(sizeof(*_tmpBD1)),((_tmpBD1->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBD1->tms=0,_tmpBD1)))))));}
break;case 162: _LL2E6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 163: _LL2E7: {struct Cyc_Parse_Declarator*_tmp5EC=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpBD7;struct Cyc_Absyn_Attributes_mod_struct _tmpBD6;struct Cyc_List_List*_tmpBD5;
_tmp5EC->tms=((_tmpBD5=_cycalloc(sizeof(*_tmpBD5)),((_tmpBD5->hd=(void*)((
_tmpBD7=_cycalloc(sizeof(*_tmpBD7)),((_tmpBD7[0]=((_tmpBD6.tag=5,((_tmpBD6.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpBD6.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpBD6)))))),_tmpBD7)))),((
_tmpBD5->tl=_tmp5EC->tms,_tmpBD5))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 164: _LL2E8:{
struct Cyc_List_List*_tmpBE1;struct Cyc_Absyn_Carray_mod_struct _tmpBE0;struct Cyc_Absyn_Carray_mod_struct*
_tmpBDF;struct Cyc_Parse_Declarator*_tmpBDE;yyval=Cyc_YY29(((_tmpBDE=_cycalloc(
sizeof(*_tmpBDE)),((_tmpBDE->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBDE->tms=((
_tmpBE1=_cycalloc(sizeof(*_tmpBE1)),((_tmpBE1->hd=(void*)((_tmpBDF=_cycalloc(
sizeof(*_tmpBDF)),((_tmpBDF[0]=((_tmpBE0.tag=0,((_tmpBE0.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBE0.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBE0)))))),
_tmpBDF)))),((_tmpBE1->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpBE1)))))),_tmpBDE)))))));}break;case 165:
_LL2E9:{struct Cyc_List_List*_tmpBEB;struct Cyc_Absyn_ConstArray_mod_struct _tmpBEA;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpBE9;struct Cyc_Parse_Declarator*_tmpBE8;
yyval=Cyc_YY29(((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpBE8->tms=((
_tmpBEB=_cycalloc(sizeof(*_tmpBEB)),((_tmpBEB->hd=(void*)((_tmpBE9=_cycalloc(
sizeof(*_tmpBE9)),((_tmpBE9[0]=((_tmpBEA.tag=1,((_tmpBEA.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBEA.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBEA.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBEA)))))))),
_tmpBE9)))),((_tmpBEB->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpBEB)))))),_tmpBE8)))))));}break;case 166:
_LL2EA: {struct _tuple24 _tmp5F9;struct Cyc_List_List*_tmp5FA;int _tmp5FB;struct Cyc_Absyn_VarargInfo*
_tmp5FC;struct Cyc_Core_Opt*_tmp5FD;struct Cyc_List_List*_tmp5FE;struct _tuple24*
_tmp5F8=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5F9=*_tmp5F8;_tmp5FA=_tmp5F9.f1;_tmp5FB=_tmp5F9.f2;_tmp5FC=_tmp5F9.f3;
_tmp5FD=_tmp5F9.f4;_tmp5FE=_tmp5F9.f5;{struct Cyc_List_List*_tmpC00;struct Cyc_Absyn_Function_mod_struct
_tmpBFF;struct Cyc_Absyn_WithTypes_struct _tmpBFE;struct Cyc_Absyn_WithTypes_struct*
_tmpBFD;struct Cyc_Absyn_Function_mod_struct*_tmpBFC;struct Cyc_Parse_Declarator*
_tmpBFB;yyval=Cyc_YY29(((_tmpBFB=_cycalloc(sizeof(*_tmpBFB)),((_tmpBFB->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBFB->tms=((
_tmpC00=_cycalloc(sizeof(*_tmpC00)),((_tmpC00->hd=(void*)((_tmpBFC=_cycalloc(
sizeof(*_tmpBFC)),((_tmpBFC[0]=((_tmpBFF.tag=3,((_tmpBFF.f1=(void*)((void*)((
_tmpBFD=_cycalloc(sizeof(*_tmpBFD)),((_tmpBFD[0]=((_tmpBFE.tag=1,((_tmpBFE.f1=
_tmp5FA,((_tmpBFE.f2=_tmp5FB,((_tmpBFE.f3=_tmp5FC,((_tmpBFE.f4=_tmp5FD,((_tmpBFE.f5=
_tmp5FE,_tmpBFE)))))))))))),_tmpBFD))))),_tmpBFF)))),_tmpBFC)))),((_tmpC00->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC00)))))),_tmpBFB)))))));}break;}case 167: _LL2EB:{struct Cyc_List_List*_tmpC15;
struct Cyc_Absyn_Function_mod_struct _tmpC14;struct Cyc_Absyn_WithTypes_struct
_tmpC13;struct Cyc_Absyn_WithTypes_struct*_tmpC12;struct Cyc_Absyn_Function_mod_struct*
_tmpC11;struct Cyc_Parse_Declarator*_tmpC10;yyval=Cyc_YY29(((_tmpC10=_cycalloc(
sizeof(*_tmpC10)),((_tmpC10->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpC10->tms=((
_tmpC15=_cycalloc(sizeof(*_tmpC15)),((_tmpC15->hd=(void*)((_tmpC11=_cycalloc(
sizeof(*_tmpC11)),((_tmpC11[0]=((_tmpC14.tag=3,((_tmpC14.f1=(void*)((void*)((
_tmpC12=_cycalloc(sizeof(*_tmpC12)),((_tmpC12[0]=((_tmpC13.tag=1,((_tmpC13.f1=0,((
_tmpC13.f2=0,((_tmpC13.f3=0,((_tmpC13.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC13.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC13)))))))))))),
_tmpC12))))),_tmpC14)))),_tmpC11)))),((_tmpC15->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpC15)))))),
_tmpC10)))))));}break;case 168: _LL2EC:{struct Cyc_List_List*_tmpC2A;struct Cyc_Absyn_Function_mod_struct
_tmpC29;struct Cyc_Absyn_NoTypes_struct _tmpC28;struct Cyc_Absyn_NoTypes_struct*
_tmpC27;struct Cyc_Absyn_Function_mod_struct*_tmpC26;struct Cyc_Parse_Declarator*
_tmpC25;yyval=Cyc_YY29(((_tmpC25=_cycalloc(sizeof(*_tmpC25)),((_tmpC25->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC25->tms=((
_tmpC2A=_cycalloc(sizeof(*_tmpC2A)),((_tmpC2A->hd=(void*)((_tmpC26=_cycalloc(
sizeof(*_tmpC26)),((_tmpC26[0]=((_tmpC29.tag=3,((_tmpC29.f1=(void*)((void*)((
_tmpC27=_cycalloc(sizeof(*_tmpC27)),((_tmpC27[0]=((_tmpC28.tag=0,((_tmpC28.f1=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpC28.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpC28)))))),_tmpC27))))),_tmpC29)))),_tmpC26)))),((
_tmpC2A->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms,_tmpC2A)))))),_tmpC25)))))));}break;case 169: _LL2ED: {
struct Cyc_List_List*_tmp611=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpC34;struct Cyc_Absyn_TypeParams_mod_struct _tmpC33;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpC32;struct Cyc_Parse_Declarator*_tmpC31;yyval=Cyc_YY29(((_tmpC31=_cycalloc(
sizeof(*_tmpC31)),((_tmpC31->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC31->tms=((
_tmpC34=_cycalloc(sizeof(*_tmpC34)),((_tmpC34->hd=(void*)((_tmpC32=_cycalloc(
sizeof(*_tmpC32)),((_tmpC32[0]=((_tmpC33.tag=4,((_tmpC33.f1=_tmp611,((_tmpC33.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpC33.f3=0,_tmpC33)))))))),_tmpC32)))),((_tmpC34->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC34)))))),_tmpC31)))))));}break;}case 170: _LL2EE:{struct Cyc_List_List*_tmpC3E;
struct Cyc_Absyn_Attributes_mod_struct _tmpC3D;struct Cyc_Absyn_Attributes_mod_struct*
_tmpC3C;struct Cyc_Parse_Declarator*_tmpC3B;yyval=Cyc_YY29(((_tmpC3B=_cycalloc(
sizeof(*_tmpC3B)),((_tmpC3B->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpC3B->tms=((
_tmpC3E=_cycalloc(sizeof(*_tmpC3E)),((_tmpC3E->hd=(void*)((_tmpC3C=_cycalloc(
sizeof(*_tmpC3C)),((_tmpC3C[0]=((_tmpC3D.tag=5,((_tmpC3D.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC3D.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC3D)))))),_tmpC3C)))),((
_tmpC3E->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpC3E)))))),_tmpC3B)))))));}break;case 171: _LL2EF:{
struct Cyc_Parse_Declarator*_tmpC3F;yyval=Cyc_YY29(((_tmpC3F=_cycalloc(sizeof(*
_tmpC3F)),((_tmpC3F->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC3F->tms=0,_tmpC3F)))))));}break;case 172: _LL2F0:{
struct Cyc_Parse_Declarator*_tmpC40;yyval=Cyc_YY29(((_tmpC40=_cycalloc(sizeof(*
_tmpC40)),((_tmpC40->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC40->tms=0,_tmpC40)))))));}break;case 173: _LL2F1:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 174:
_LL2F2: {struct Cyc_Parse_Declarator*_tmp61C=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpC46;struct Cyc_Absyn_Attributes_mod_struct _tmpC45;struct Cyc_List_List*_tmpC44;
_tmp61C->tms=((_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44->hd=(void*)((
_tmpC46=_cycalloc(sizeof(*_tmpC46)),((_tmpC46[0]=((_tmpC45.tag=5,((_tmpC45.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpC45.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC45)))))),_tmpC46)))),((
_tmpC44->tl=_tmp61C->tms,_tmpC44))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 175: _LL2F3:{
struct Cyc_List_List*_tmpC50;struct Cyc_Absyn_Carray_mod_struct _tmpC4F;struct Cyc_Absyn_Carray_mod_struct*
_tmpC4E;struct Cyc_Parse_Declarator*_tmpC4D;yyval=Cyc_YY29(((_tmpC4D=_cycalloc(
sizeof(*_tmpC4D)),((_tmpC4D->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC4D->tms=((
_tmpC50=_cycalloc(sizeof(*_tmpC50)),((_tmpC50->hd=(void*)((_tmpC4E=_cycalloc(
sizeof(*_tmpC4E)),((_tmpC4E[0]=((_tmpC4F.tag=0,((_tmpC4F.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC4F.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC4F)))))),
_tmpC4E)))),((_tmpC50->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpC50)))))),_tmpC4D)))))));}break;case 176:
_LL2F4:{struct Cyc_List_List*_tmpC5A;struct Cyc_Absyn_ConstArray_mod_struct _tmpC59;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpC58;struct Cyc_Parse_Declarator*_tmpC57;
yyval=Cyc_YY29(((_tmpC57=_cycalloc(sizeof(*_tmpC57)),((_tmpC57->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpC57->tms=((
_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((_tmpC5A->hd=(void*)((_tmpC58=_cycalloc(
sizeof(*_tmpC58)),((_tmpC58[0]=((_tmpC59.tag=1,((_tmpC59.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC59.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC59.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC59)))))))),
_tmpC58)))),((_tmpC5A->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpC5A)))))),_tmpC57)))))));}break;case 177:
_LL2F5: {struct _tuple24 _tmp629;struct Cyc_List_List*_tmp62A;int _tmp62B;struct Cyc_Absyn_VarargInfo*
_tmp62C;struct Cyc_Core_Opt*_tmp62D;struct Cyc_List_List*_tmp62E;struct _tuple24*
_tmp628=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp629=*_tmp628;_tmp62A=_tmp629.f1;_tmp62B=_tmp629.f2;_tmp62C=_tmp629.f3;
_tmp62D=_tmp629.f4;_tmp62E=_tmp629.f5;{struct Cyc_List_List*_tmpC6F;struct Cyc_Absyn_Function_mod_struct
_tmpC6E;struct Cyc_Absyn_WithTypes_struct _tmpC6D;struct Cyc_Absyn_WithTypes_struct*
_tmpC6C;struct Cyc_Absyn_Function_mod_struct*_tmpC6B;struct Cyc_Parse_Declarator*
_tmpC6A;yyval=Cyc_YY29(((_tmpC6A=_cycalloc(sizeof(*_tmpC6A)),((_tmpC6A->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC6A->tms=((
_tmpC6F=_cycalloc(sizeof(*_tmpC6F)),((_tmpC6F->hd=(void*)((_tmpC6B=_cycalloc(
sizeof(*_tmpC6B)),((_tmpC6B[0]=((_tmpC6E.tag=3,((_tmpC6E.f1=(void*)((void*)((
_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C[0]=((_tmpC6D.tag=1,((_tmpC6D.f1=
_tmp62A,((_tmpC6D.f2=_tmp62B,((_tmpC6D.f3=_tmp62C,((_tmpC6D.f4=_tmp62D,((_tmpC6D.f5=
_tmp62E,_tmpC6D)))))))))))),_tmpC6C))))),_tmpC6E)))),_tmpC6B)))),((_tmpC6F->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC6F)))))),_tmpC6A)))))));}break;}case 178: _LL2F6:{struct Cyc_List_List*_tmpC84;
struct Cyc_Absyn_Function_mod_struct _tmpC83;struct Cyc_Absyn_WithTypes_struct
_tmpC82;struct Cyc_Absyn_WithTypes_struct*_tmpC81;struct Cyc_Absyn_Function_mod_struct*
_tmpC80;struct Cyc_Parse_Declarator*_tmpC7F;yyval=Cyc_YY29(((_tmpC7F=_cycalloc(
sizeof(*_tmpC7F)),((_tmpC7F->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpC7F->tms=((
_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84->hd=(void*)((_tmpC80=_cycalloc(
sizeof(*_tmpC80)),((_tmpC80[0]=((_tmpC83.tag=3,((_tmpC83.f1=(void*)((void*)((
_tmpC81=_cycalloc(sizeof(*_tmpC81)),((_tmpC81[0]=((_tmpC82.tag=1,((_tmpC82.f1=0,((
_tmpC82.f2=0,((_tmpC82.f3=0,((_tmpC82.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC82.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC82)))))))))))),
_tmpC81))))),_tmpC83)))),_tmpC80)))),((_tmpC84->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpC84)))))),
_tmpC7F)))))));}break;case 179: _LL2F7:{struct Cyc_List_List*_tmpC99;struct Cyc_Absyn_Function_mod_struct
_tmpC98;struct Cyc_Absyn_NoTypes_struct _tmpC97;struct Cyc_Absyn_NoTypes_struct*
_tmpC96;struct Cyc_Absyn_Function_mod_struct*_tmpC95;struct Cyc_Parse_Declarator*
_tmpC94;yyval=Cyc_YY29(((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC94->tms=((
_tmpC99=_cycalloc(sizeof(*_tmpC99)),((_tmpC99->hd=(void*)((_tmpC95=_cycalloc(
sizeof(*_tmpC95)),((_tmpC95[0]=((_tmpC98.tag=3,((_tmpC98.f1=(void*)((void*)((
_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96[0]=((_tmpC97.tag=0,((_tmpC97.f1=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpC97.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpC97)))))),_tmpC96))))),_tmpC98)))),_tmpC95)))),((
_tmpC99->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms,_tmpC99)))))),_tmpC94)))))));}break;case 180: _LL2F8: {
struct Cyc_List_List*_tmp641=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpCA3;struct Cyc_Absyn_TypeParams_mod_struct _tmpCA2;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpCA1;struct Cyc_Parse_Declarator*_tmpCA0;yyval=Cyc_YY29(((_tmpCA0=_cycalloc(
sizeof(*_tmpCA0)),((_tmpCA0->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpCA0->tms=((
_tmpCA3=_cycalloc(sizeof(*_tmpCA3)),((_tmpCA3->hd=(void*)((_tmpCA1=_cycalloc(
sizeof(*_tmpCA1)),((_tmpCA1[0]=((_tmpCA2.tag=4,((_tmpCA2.f1=_tmp641,((_tmpCA2.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpCA2.f3=0,_tmpCA2)))))))),_tmpCA1)))),((_tmpCA3->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpCA3)))))),_tmpCA0)))))));}break;}case 181: _LL2F9:{struct Cyc_List_List*_tmpCAD;
struct Cyc_Absyn_Attributes_mod_struct _tmpCAC;struct Cyc_Absyn_Attributes_mod_struct*
_tmpCAB;struct Cyc_Parse_Declarator*_tmpCAA;yyval=Cyc_YY29(((_tmpCAA=_cycalloc(
sizeof(*_tmpCAA)),((_tmpCAA->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpCAA->tms=((
_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD->hd=(void*)((_tmpCAB=_cycalloc(
sizeof(*_tmpCAB)),((_tmpCAB[0]=((_tmpCAC.tag=5,((_tmpCAC.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpCAC.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCAC)))))),_tmpCAB)))),((
_tmpCAD->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpCAD)))))),_tmpCAA)))))));}break;case 182: _LL2FA:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 183:
_LL2FB: yyval=Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 184: _LL2FC: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){struct Cyc_Absyn_Attributes_mod_struct*
_tmpCB3;struct Cyc_Absyn_Attributes_mod_struct _tmpCB2;struct Cyc_List_List*_tmpCB1;
ans=((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1->hd=(void*)((_tmpCB3=
_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3[0]=((_tmpCB2.tag=5,((_tmpCB2.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpCB2.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpCB2)))))),_tmpCB3)))),((_tmpCB1->tl=ans,_tmpCB1))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;struct Cyc_Position_Segment*_tmp64E;union Cyc_Absyn_Constraint*_tmp64F;
union Cyc_Absyn_Constraint*_tmp650;struct _tuple19 _tmp64D=*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp64E=_tmp64D.f1;
_tmp64F=_tmp64D.f2;_tmp650=_tmp64D.f3;if(Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*
_tmpCB4;ptrloc=((_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4->ptr_loc=_tmp64E,((
_tmpCB4->rgn_loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpCB4->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),_tmpCB4))))))));}{union Cyc_Absyn_Constraint*
_tmp653;union Cyc_Absyn_Constraint*_tmp654;union Cyc_Absyn_Constraint*_tmp655;void*
_tmp656;struct _tuple12 _tmp652=Cyc_Parse_collapse_pointer_quals(_tmp64E,_tmp64F,
_tmp650,Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY54(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
_tmp653=_tmp652.f1;_tmp654=_tmp652.f2;_tmp655=_tmp652.f3;_tmp656=_tmp652.f4;{
struct Cyc_Absyn_Pointer_mod_struct*_tmpCBE;struct Cyc_Absyn_PtrAtts _tmpCBD;struct
Cyc_Absyn_Pointer_mod_struct _tmpCBC;struct Cyc_List_List*_tmpCBB;ans=((_tmpCBB=
_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB->hd=(void*)((_tmpCBE=_cycalloc(sizeof(*
_tmpCBE)),((_tmpCBE[0]=((_tmpCBC.tag=2,((_tmpCBC.f1=((_tmpCBD.rgn=_tmp656,((
_tmpCBD.nullable=_tmp653,((_tmpCBD.bounds=_tmp654,((_tmpCBD.zero_term=_tmp655,((
_tmpCBD.ptrloc=ptrloc,_tmpCBD)))))))))),((_tmpCBC.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCBC)))))),_tmpCBE)))),((
_tmpCBB->tl=ans,_tmpCBB))))));}yyval=Cyc_YY28(ans);break;};};}case 185: _LL2FD:
yyval=Cyc_YY54(0);break;case 186: _LL2FE:{struct Cyc_List_List*_tmpCBF;yyval=Cyc_YY54(((
_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->hd=Cyc_yyget_YY53(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCBF->tl=Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCBF)))))));}break;
case 187: _LL2FF:{struct Cyc_Absyn_Numelts_ptrqual_struct _tmpCC2;struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmpCC1;yyval=Cyc_YY53((void*)((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1[0]=((
_tmpCC2.tag=0,((_tmpCC2.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpCC2)))),_tmpCC1)))));}break;case 188: _LL300:{struct
Cyc_Absyn_Region_ptrqual_struct _tmpCC5;struct Cyc_Absyn_Region_ptrqual_struct*
_tmpCC4;yyval=Cyc_YY53((void*)((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4[0]=((
_tmpCC5.tag=1,((_tmpCC5.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCC5)))),_tmpCC4)))));}
break;case 189: _LL301:{struct Cyc_Absyn_Thin_ptrqual_struct _tmpCC8;struct Cyc_Absyn_Thin_ptrqual_struct*
_tmpCC7;yyval=Cyc_YY53((void*)((_tmpCC7=_cycalloc_atomic(sizeof(*_tmpCC7)),((
_tmpCC7[0]=((_tmpCC8.tag=2,_tmpCC8)),_tmpCC7)))));}break;case 190: _LL302:{struct
Cyc_Absyn_Fat_ptrqual_struct _tmpCCB;struct Cyc_Absyn_Fat_ptrqual_struct*_tmpCCA;
yyval=Cyc_YY53((void*)((_tmpCCA=_cycalloc_atomic(sizeof(*_tmpCCA)),((_tmpCCA[0]=((
_tmpCCB.tag=3,_tmpCCB)),_tmpCCA)))));}break;case 191: _LL303:{struct Cyc_Absyn_Zeroterm_ptrqual_struct
_tmpCCE;struct Cyc_Absyn_Zeroterm_ptrqual_struct*_tmpCCD;yyval=Cyc_YY53((void*)((
_tmpCCD=_cycalloc_atomic(sizeof(*_tmpCCD)),((_tmpCCD[0]=((_tmpCCE.tag=4,_tmpCCE)),
_tmpCCD)))));}break;case 192: _LL304:{struct Cyc_Absyn_Nozeroterm_ptrqual_struct
_tmpCD1;struct Cyc_Absyn_Nozeroterm_ptrqual_struct*_tmpCD0;yyval=Cyc_YY53((void*)((
_tmpCD0=_cycalloc_atomic(sizeof(*_tmpCD0)),((_tmpCD0[0]=((_tmpCD1.tag=5,_tmpCD1)),
_tmpCD0)))));}break;case 193: _LL305:{struct Cyc_Absyn_Notnull_ptrqual_struct
_tmpCD4;struct Cyc_Absyn_Notnull_ptrqual_struct*_tmpCD3;yyval=Cyc_YY53((void*)((
_tmpCD3=_cycalloc_atomic(sizeof(*_tmpCD3)),((_tmpCD3[0]=((_tmpCD4.tag=6,_tmpCD4)),
_tmpCD3)))));}break;case 194: _LL306:{struct Cyc_Absyn_Nullable_ptrqual_struct
_tmpCD7;struct Cyc_Absyn_Nullable_ptrqual_struct*_tmpCD6;yyval=Cyc_YY53((void*)((
_tmpCD6=_cycalloc_atomic(sizeof(*_tmpCD6)),((_tmpCD6[0]=((_tmpCD7.tag=7,_tmpCD7)),
_tmpCD6)))));}break;case 195: _LL307: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple19*_tmpCD8;yyval=Cyc_YY1(((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->f1=
loc,((_tmpCD8->f2=Cyc_Absyn_true_conref,((_tmpCD8->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCD8)))))))));}break;}
case 196: _LL308: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple19*_tmpCD9;yyval=Cyc_YY1(((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9->f1=
loc,((_tmpCD9->f2=Cyc_Absyn_false_conref,((_tmpCD9->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCD9)))))))));}break;}
case 197: _LL309: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{struct _tuple19*
_tmpCDA;yyval=Cyc_YY1(((_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA->f1=loc,((
_tmpCDA->f2=Cyc_Absyn_true_conref,((_tmpCDA->f3=Cyc_Absyn_bounds_dyneither_conref,
_tmpCDA)))))))));}break;}case 198: _LL30A: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 199: _LL30B:{struct Cyc_Absyn_Upper_b_struct _tmpCDD;struct Cyc_Absyn_Upper_b_struct*
_tmpCDC;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC[0]=((_tmpCDD.tag=1,((
_tmpCDD.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpCDD)))),_tmpCDC))))));}break;case 200: _LL30C: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)());break;case 201: _LL30D: yyval=Cyc_YY51(Cyc_Absyn_true_conref);
break;case 202: _LL30E: yyval=Cyc_YY51(Cyc_Absyn_false_conref);break;case 203: _LL30F:
yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));
break;case 204: _LL310: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_trk,1);yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 205: _LL311:
yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,0));
break;case 206: _LL312: yyval=Cyc_YY25(Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)));break;case
207: _LL313: yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 208: _LL314:{
struct _tuple24*_tmpCDE;yyval=Cyc_YY39(((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((
_tmpCDE->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((
_tmpCDE->f2=0,((_tmpCDE->f3=0,((_tmpCDE->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCDE->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCDE)))))))))))));}
break;case 209: _LL315:{struct _tuple24*_tmpCDF;yyval=Cyc_YY39(((_tmpCDF=_cycalloc(
sizeof(*_tmpCDF)),((_tmpCDF->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),((_tmpCDF->f2=1,((_tmpCDF->f3=0,((_tmpCDF->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCDF->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCDF)))))))))))));}
break;case 210: _LL316: {struct _tuple8 _tmp674;struct Cyc_Core_Opt*_tmp675;struct Cyc_Absyn_Tqual
_tmp676;void*_tmp677;struct _tuple8*_tmp673=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp674=*_tmp673;_tmp675=
_tmp674.f1;_tmp676=_tmp674.f2;_tmp677=_tmp674.f3;{struct Cyc_Absyn_VarargInfo*
_tmpCE0;struct Cyc_Absyn_VarargInfo*_tmp678=(_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((
_tmpCE0->name=_tmp675,((_tmpCE0->tq=_tmp676,((_tmpCE0->type=_tmp677,((_tmpCE0->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpCE0)))))))));{struct _tuple24*_tmpCE1;yyval=Cyc_YY39(((_tmpCE1=_cycalloc(
sizeof(*_tmpCE1)),((_tmpCE1->f1=0,((_tmpCE1->f2=0,((_tmpCE1->f3=_tmp678,((
_tmpCE1->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpCE1->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCE1)))))))))))));}
break;};}case 211: _LL317: {struct _tuple8 _tmp67C;struct Cyc_Core_Opt*_tmp67D;struct
Cyc_Absyn_Tqual _tmp67E;void*_tmp67F;struct _tuple8*_tmp67B=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp67C=*_tmp67B;_tmp67D=
_tmp67C.f1;_tmp67E=_tmp67C.f2;_tmp67F=_tmp67C.f3;{struct Cyc_Absyn_VarargInfo*
_tmpCE2;struct Cyc_Absyn_VarargInfo*_tmp680=(_tmpCE2=_cycalloc(sizeof(*_tmpCE2)),((
_tmpCE2->name=_tmp67D,((_tmpCE2->tq=_tmp67E,((_tmpCE2->type=_tmp67F,((_tmpCE2->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpCE2)))))))));{struct _tuple24*_tmpCE3;yyval=Cyc_YY39(((_tmpCE3=_cycalloc(
sizeof(*_tmpCE3)),((_tmpCE3->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)])),((_tmpCE3->f2=0,((_tmpCE3->f3=_tmp680,((_tmpCE3->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCE3->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCE3)))))))))))));}
break;};}case 212: _LL318:{struct Cyc_Absyn_Unknown_kb_struct _tmpCE6;struct Cyc_Absyn_Unknown_kb_struct*
_tmpCE5;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)((_tmpCE5=_cycalloc(
sizeof(*_tmpCE5)),((_tmpCE5[0]=((_tmpCE6.tag=1,((_tmpCE6.f1=0,_tmpCE6)))),
_tmpCE5))))));}break;case 213: _LL319: yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))));
break;case 214: _LL31A: yyval=Cyc_YY49(0);break;case 215: _LL31B:{struct Cyc_Absyn_JoinEff_struct*
_tmpCEC;struct Cyc_Absyn_JoinEff_struct _tmpCEB;struct Cyc_Core_Opt*_tmpCEA;yyval=
Cyc_YY49(((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->v=(void*)((_tmpCEC=
_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC[0]=((_tmpCEB.tag=24,((_tmpCEB.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCEB)))),_tmpCEC)))),
_tmpCEA)))));}break;case 216: _LL31C: yyval=Cyc_YY50(0);break;case 217: _LL31D: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 218: _LL31E: {
struct Cyc_Absyn_Less_kb_struct _tmpCEF;struct Cyc_Absyn_Less_kb_struct*_tmpCEE;
struct Cyc_Absyn_Less_kb_struct*_tmp688=(_tmpCEE=_cycalloc(sizeof(*_tmpCEE)),((
_tmpCEE[0]=((_tmpCEF.tag=2,((_tmpCEF.f1=0,((_tmpCEF.f2=& Cyc_Tcutil_trk,_tmpCEF)))))),
_tmpCEE)));struct _dyneither_ptr _tmp689=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp68A=Cyc_Parse_id2type(
_tmp689,(void*)_tmp688);{struct _tuple27*_tmpCF9;struct Cyc_Absyn_JoinEff_struct
_tmpCF8;struct Cyc_Absyn_JoinEff_struct*_tmpCF7;struct Cyc_List_List*_tmpCF6;yyval=
Cyc_YY50(((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->hd=((_tmpCF9=_cycalloc(
sizeof(*_tmpCF9)),((_tmpCF9->f1=(void*)((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((
_tmpCF7[0]=((_tmpCF8.tag=24,((_tmpCF8.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCF8)))),_tmpCF7)))),((
_tmpCF9->f2=_tmp68A,_tmpCF9)))))),((_tmpCF6->tl=0,_tmpCF6)))))));}break;}case 219:
_LL31F: {struct Cyc_Absyn_Less_kb_struct _tmpCFC;struct Cyc_Absyn_Less_kb_struct*
_tmpCFB;struct Cyc_Absyn_Less_kb_struct*_tmp691=(_tmpCFB=_cycalloc(sizeof(*
_tmpCFB)),((_tmpCFB[0]=((_tmpCFC.tag=2,((_tmpCFC.f1=0,((_tmpCFC.f2=& Cyc_Tcutil_trk,
_tmpCFC)))))),_tmpCFB)));struct _dyneither_ptr _tmp692=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp693=Cyc_Parse_id2type(
_tmp692,(void*)_tmp691);{struct _tuple27*_tmpD06;struct Cyc_Absyn_JoinEff_struct
_tmpD05;struct Cyc_Absyn_JoinEff_struct*_tmpD04;struct Cyc_List_List*_tmpD03;yyval=
Cyc_YY50(((_tmpD03=_cycalloc(sizeof(*_tmpD03)),((_tmpD03->hd=((_tmpD06=_cycalloc(
sizeof(*_tmpD06)),((_tmpD06->f1=(void*)((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((
_tmpD04[0]=((_tmpD05.tag=24,((_tmpD05.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmpD05)))),_tmpD04)))),((
_tmpD06->f2=_tmp693,_tmpD06)))))),((_tmpD03->tl=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD03)))))));}break;}case
220: _LL320: yyval=Cyc_YY31(0);break;case 221: _LL321:{const char*_tmpD07;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpD07="inject",_tag_dyneither(_tmpD07,sizeof(char),7))))
!= 0){const char*_tmpD08;Cyc_Parse_err(((_tmpD08="missing type in function declaration",
_tag_dyneither(_tmpD08,sizeof(char),37))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY31(
1);break;case 222: _LL322: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 223: _LL323: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 224: _LL324:
yyval=Cyc_YY40(0);break;case 225: _LL325: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 226: _LL326:{struct Cyc_Absyn_RgnsEff_struct*
_tmpD0E;struct Cyc_Absyn_RgnsEff_struct _tmpD0D;struct Cyc_List_List*_tmpD0C;yyval=
Cyc_YY40(((_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C->hd=(void*)((_tmpD0E=
_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E[0]=((_tmpD0D.tag=25,((_tmpD0D.f1=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpD0D)))),_tmpD0E)))),((_tmpD0C->tl=0,_tmpD0C)))))));}break;case 227: _LL327: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_ek,
0);{struct Cyc_List_List*_tmpD0F;yyval=Cyc_YY40(((_tmpD0F=_cycalloc(sizeof(*
_tmpD0F)),((_tmpD0F->hd=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD0F->tl=0,_tmpD0F)))))));}break;case 228: _LL328: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_trk,
1);{struct Cyc_Absyn_AccessEff_struct*_tmpD15;struct Cyc_Absyn_AccessEff_struct
_tmpD14;struct Cyc_List_List*_tmpD13;yyval=Cyc_YY40(((_tmpD13=_cycalloc(sizeof(*
_tmpD13)),((_tmpD13->hd=(void*)((_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15[0]=((
_tmpD14.tag=23,((_tmpD14.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD14)))),_tmpD15)))),((
_tmpD13->tl=0,_tmpD13)))))));}break;case 229: _LL329: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),& Cyc_Tcutil_trk,1);{
struct Cyc_Absyn_AccessEff_struct*_tmpD1B;struct Cyc_Absyn_AccessEff_struct _tmpD1A;
struct Cyc_List_List*_tmpD19;yyval=Cyc_YY40(((_tmpD19=_cycalloc(sizeof(*_tmpD19)),((
_tmpD19->hd=(void*)((_tmpD1B=_cycalloc(sizeof(*_tmpD1B)),((_tmpD1B[0]=((_tmpD1A.tag=
23,((_tmpD1A.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),_tmpD1A)))),_tmpD1B)))),((_tmpD19->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD19)))))));}break;case
230: _LL32A:{struct Cyc_List_List*_tmpD1C;yyval=Cyc_YY38(((_tmpD1C=_cycalloc(
sizeof(*_tmpD1C)),((_tmpD1C->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD1C->tl=0,_tmpD1C)))))));}
break;case 231: _LL32B:{struct Cyc_List_List*_tmpD1D;yyval=Cyc_YY38(((_tmpD1D=
_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD1D->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD1D)))))));}
break;case 232: _LL32C: {struct _tuple23 _tmp6A9;struct Cyc_Absyn_Tqual _tmp6AA;struct
Cyc_List_List*_tmp6AB;struct Cyc_List_List*_tmp6AC;struct _tuple23*_tmp6A8=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6A9=*_tmp6A8;
_tmp6AA=_tmp6A9.f1;_tmp6AB=_tmp6A9.f2;_tmp6AC=_tmp6A9.f3;if(_tmp6AA.loc == 0)
_tmp6AA.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp6AE;struct _tuple1*
_tmp6AF;struct Cyc_List_List*_tmp6B0;struct Cyc_Parse_Declarator*_tmp6AD=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AE=*_tmp6AD;
_tmp6AF=_tmp6AE.id;_tmp6B0=_tmp6AE.tms;{void*_tmp6B1=Cyc_Parse_speclist2typ(
_tmp6AB,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp6B3;void*_tmp6B4;struct
Cyc_List_List*_tmp6B5;struct Cyc_List_List*_tmp6B6;struct _tuple11 _tmp6B2=Cyc_Parse_apply_tms(
_tmp6AA,_tmp6B1,_tmp6AC,_tmp6B0);_tmp6B3=_tmp6B2.f1;_tmp6B4=_tmp6B2.f2;_tmp6B5=
_tmp6B2.f3;_tmp6B6=_tmp6B2.f4;if(_tmp6B5 != 0){const char*_tmpD1E;Cyc_Parse_err(((
_tmpD1E="parameter with bad type params",_tag_dyneither(_tmpD1E,sizeof(char),31))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp6AF)){const char*
_tmpD1F;Cyc_Parse_err(((_tmpD1F="parameter cannot be qualified with a namespace",
_tag_dyneither(_tmpD1F,sizeof(char),47))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*
_tmpD20;struct Cyc_Core_Opt*_tmp6B9=(struct Cyc_Core_Opt*)((_tmpD20=_cycalloc(
sizeof(*_tmpD20)),((_tmpD20->v=(*_tmp6AF).f2,_tmpD20))));if(_tmp6B6 != 0){const
char*_tmpD23;void*_tmpD22;(_tmpD22=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD23="extra attributes on parameter, ignoring",
_tag_dyneither(_tmpD23,sizeof(char),40))),_tag_dyneither(_tmpD22,sizeof(void*),0)));}{
struct _tuple8*_tmpD24;yyval=Cyc_YY37(((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((
_tmpD24->f1=_tmp6B9,((_tmpD24->f2=_tmp6B3,((_tmpD24->f3=_tmp6B4,_tmpD24)))))))));}
break;};};};}case 233: _LL32D: {struct _tuple23 _tmp6BF;struct Cyc_Absyn_Tqual _tmp6C0;
struct Cyc_List_List*_tmp6C1;struct Cyc_List_List*_tmp6C2;struct _tuple23*_tmp6BE=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6BF=*
_tmp6BE;_tmp6C0=_tmp6BF.f1;_tmp6C1=_tmp6BF.f2;_tmp6C2=_tmp6BF.f3;if(_tmp6C0.loc
== 0)_tmp6C0.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{void*_tmp6C3=
Cyc_Parse_speclist2typ(_tmp6C1,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6C2 != 0){
const char*_tmpD27;void*_tmpD26;(_tmpD26=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD27="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpD27,sizeof(char),38))),_tag_dyneither(_tmpD26,sizeof(void*),0)));}{
struct _tuple8*_tmpD28;yyval=Cyc_YY37(((_tmpD28=_cycalloc(sizeof(*_tmpD28)),((
_tmpD28->f1=0,((_tmpD28->f2=_tmp6C0,((_tmpD28->f3=_tmp6C3,_tmpD28)))))))));}
break;};}case 234: _LL32E: {struct _tuple23 _tmp6C8;struct Cyc_Absyn_Tqual _tmp6C9;
struct Cyc_List_List*_tmp6CA;struct Cyc_List_List*_tmp6CB;struct _tuple23*_tmp6C7=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6C8=*_tmp6C7;_tmp6C9=_tmp6C8.f1;_tmp6CA=_tmp6C8.f2;_tmp6CB=_tmp6C8.f3;if(
_tmp6C9.loc == 0)_tmp6C9.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{void*_tmp6CC=
Cyc_Parse_speclist2typ(_tmp6CA,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6CD=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp6CF;void*_tmp6D0;struct Cyc_List_List*_tmp6D1;struct Cyc_List_List*
_tmp6D2;struct _tuple11 _tmp6CE=Cyc_Parse_apply_tms(_tmp6C9,_tmp6CC,_tmp6CB,
_tmp6CD);_tmp6CF=_tmp6CE.f1;_tmp6D0=_tmp6CE.f2;_tmp6D1=_tmp6CE.f3;_tmp6D2=
_tmp6CE.f4;if(_tmp6D1 != 0){const char*_tmpD2B;void*_tmpD2A;(_tmpD2A=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpD2B="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpD2B,sizeof(char),49))),_tag_dyneither(_tmpD2A,sizeof(void*),0)));}
if(_tmp6D2 != 0){const char*_tmpD2E;void*_tmpD2D;(_tmpD2D=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD2E="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpD2E,sizeof(char),38))),_tag_dyneither(_tmpD2D,sizeof(void*),0)));}{
struct _tuple8*_tmpD2F;yyval=Cyc_YY37(((_tmpD2F=_cycalloc(sizeof(*_tmpD2F)),((
_tmpD2F->f1=0,((_tmpD2F->f2=_tmp6CF,((_tmpD2F->f3=_tmp6D0,_tmpD2F)))))))));}
break;};}case 235: _LL32F: yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 236: _LL330:{struct _dyneither_ptr*_tmpD32;struct Cyc_List_List*
_tmpD31;yyval=Cyc_YY36(((_tmpD31=_cycalloc(sizeof(*_tmpD31)),((_tmpD31->hd=((
_tmpD32=_cycalloc(sizeof(*_tmpD32)),((_tmpD32[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD32)))),((_tmpD31->tl=0,
_tmpD31)))))));}break;case 237: _LL331:{struct _dyneither_ptr*_tmpD35;struct Cyc_List_List*
_tmpD34;yyval=Cyc_YY36(((_tmpD34=_cycalloc(sizeof(*_tmpD34)),((_tmpD34->hd=((
_tmpD35=_cycalloc(sizeof(*_tmpD35)),((_tmpD35[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD35)))),((_tmpD34->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpD34)))))));}break;case 238: _LL332: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 239: _LL333: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 240: _LL334:{struct
Cyc_Absyn_UnresolvedMem_e_struct _tmpD38;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpD37;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD37=_cycalloc(sizeof(*
_tmpD37)),((_tmpD37[0]=((_tmpD38.tag=36,((_tmpD38.f1=0,((_tmpD38.f2=0,_tmpD38)))))),
_tmpD37)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 241: _LL335:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD3B;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD3A;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD3A=_cycalloc(sizeof(*_tmpD3A)),((_tmpD3A[0]=((_tmpD3B.tag=36,((
_tmpD3B.f1=0,((_tmpD3B.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),
_tmpD3B)))))),_tmpD3A)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 242:
_LL336:{struct Cyc_Absyn_UnresolvedMem_e_struct _tmpD3E;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpD3D;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD3D=_cycalloc(sizeof(*
_tmpD3D)),((_tmpD3D[0]=((_tmpD3E.tag=36,((_tmpD3E.f1=0,((_tmpD3E.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),_tmpD3E)))))),_tmpD3D)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 243: _LL337: {struct _dyneither_ptr*
_tmpD41;struct _tuple1*_tmpD40;struct Cyc_Absyn_Vardecl*_tmp6E2=Cyc_Absyn_new_vardecl(((
_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40->f1=Cyc_Absyn_Loc_n,((_tmpD40->f2=((
_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpD41)))),_tmpD40)))))),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp6E2->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct _tmpD44;struct Cyc_Absyn_Comprehension_e_struct*
_tmpD43;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD43=_cycalloc(sizeof(*
_tmpD43)),((_tmpD43[0]=((_tmpD44.tag=28,((_tmpD44.f1=_tmp6E2,((_tmpD44.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpD44.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD44.f4=0,
_tmpD44)))))))))),_tmpD43)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
244: _LL338:{struct _tuple28*_tmpD47;struct Cyc_List_List*_tmpD46;yyval=Cyc_YY6(((
_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->hd=((_tmpD47=_cycalloc(sizeof(*
_tmpD47)),((_tmpD47->f1=0,((_tmpD47->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD47)))))),((_tmpD46->tl=
0,_tmpD46)))))));}break;case 245: _LL339:{struct _tuple28*_tmpD4A;struct Cyc_List_List*
_tmpD49;yyval=Cyc_YY6(((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->hd=((
_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD4A->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD4A)))))),((
_tmpD49->tl=0,_tmpD49)))))));}break;case 246: _LL33A:{struct _tuple28*_tmpD4D;
struct Cyc_List_List*_tmpD4C;yyval=Cyc_YY6(((_tmpD4C=_cycalloc(sizeof(*_tmpD4C)),((
_tmpD4C->hd=((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D->f1=0,((_tmpD4D->f2=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD4D)))))),((
_tmpD4C->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),_tmpD4C)))))));}break;case 247: _LL33B:{struct _tuple28*_tmpD50;struct Cyc_List_List*
_tmpD4F;yyval=Cyc_YY6(((_tmpD4F=_cycalloc(sizeof(*_tmpD4F)),((_tmpD4F->hd=((
_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD50->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD50)))))),((
_tmpD4F->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]),_tmpD4F)))))));}break;case 248: _LL33C: yyval=Cyc_YY41(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 249: _LL33D:{
struct Cyc_List_List*_tmpD51;yyval=Cyc_YY41(((_tmpD51=_cycalloc(sizeof(*_tmpD51)),((
_tmpD51->hd=Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD51->tl=0,_tmpD51)))))));}break;case 250: _LL33E:{struct Cyc_List_List*
_tmpD52;yyval=Cyc_YY41(((_tmpD52=_cycalloc(sizeof(*_tmpD52)),((_tmpD52->hd=Cyc_yyget_YY42(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD52->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD52)))))));}
break;case 251: _LL33F:{struct Cyc_Absyn_ArrayElement_struct _tmpD55;struct Cyc_Absyn_ArrayElement_struct*
_tmpD54;yyval=Cyc_YY42((void*)((_tmpD54=_cycalloc(sizeof(*_tmpD54)),((_tmpD54[0]=((
_tmpD55.tag=0,((_tmpD55.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpD55)))),_tmpD54)))));}break;case 252: _LL340:{struct
Cyc_Absyn_FieldName_struct _tmpD5B;struct _dyneither_ptr*_tmpD5A;struct Cyc_Absyn_FieldName_struct*
_tmpD59;yyval=Cyc_YY42((void*)((_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59[0]=((
_tmpD5B.tag=1,((_tmpD5B.f1=((_tmpD5A=_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpD5A)))),_tmpD5B)))),_tmpD59)))));}break;case 253: _LL341: {struct _tuple23
_tmp6F7;struct Cyc_Absyn_Tqual _tmp6F8;struct Cyc_List_List*_tmp6F9;struct Cyc_List_List*
_tmp6FA;struct _tuple23*_tmp6F6=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6F7=*_tmp6F6;_tmp6F8=
_tmp6F7.f1;_tmp6F9=_tmp6F7.f2;_tmp6FA=_tmp6F7.f3;{void*_tmp6FB=Cyc_Parse_speclist2typ(
_tmp6F9,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp6FA != 0){const char*_tmpD5E;void*_tmpD5D;(
_tmpD5D=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD5E="ignoring attributes in type",
_tag_dyneither(_tmpD5E,sizeof(char),28))),_tag_dyneither(_tmpD5D,sizeof(void*),0)));}{
struct _tuple8*_tmpD5F;yyval=Cyc_YY37(((_tmpD5F=_cycalloc(sizeof(*_tmpD5F)),((
_tmpD5F->f1=0,((_tmpD5F->f2=_tmp6F8,((_tmpD5F->f3=_tmp6FB,_tmpD5F)))))))));}
break;};}case 254: _LL342: {struct _tuple23 _tmp700;struct Cyc_Absyn_Tqual _tmp701;
struct Cyc_List_List*_tmp702;struct Cyc_List_List*_tmp703;struct _tuple23*_tmp6FF=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp700=*_tmp6FF;_tmp701=_tmp700.f1;_tmp702=_tmp700.f2;_tmp703=_tmp700.f3;{void*
_tmp704=Cyc_Parse_speclist2typ(_tmp702,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp705=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple11 _tmp706=Cyc_Parse_apply_tms(_tmp701,_tmp704,_tmp703,_tmp705);if(
_tmp706.f3 != 0){const char*_tmpD62;void*_tmpD61;(_tmpD61=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD62="bad type params, ignoring",
_tag_dyneither(_tmpD62,sizeof(char),26))),_tag_dyneither(_tmpD61,sizeof(void*),0)));}
if(_tmp706.f4 != 0){const char*_tmpD65;void*_tmpD64;(_tmpD64=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD65="bad specifiers, ignoring",
_tag_dyneither(_tmpD65,sizeof(char),25))),_tag_dyneither(_tmpD64,sizeof(void*),0)));}{
struct _tuple8*_tmpD66;yyval=Cyc_YY37(((_tmpD66=_cycalloc(sizeof(*_tmpD66)),((
_tmpD66->f1=0,((_tmpD66->f2=_tmp706.f1,((_tmpD66->f3=_tmp706.f2,_tmpD66)))))))));}
break;};}case 255: _LL343: yyval=Cyc_YY44((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);break;case 256: _LL344:{
struct Cyc_Absyn_JoinEff_struct _tmpD69;struct Cyc_Absyn_JoinEff_struct*_tmpD68;
yyval=Cyc_YY44((void*)((_tmpD68=_cycalloc(sizeof(*_tmpD68)),((_tmpD68[0]=((
_tmpD69.tag=24,((_tmpD69.f1=0,_tmpD69)))),_tmpD68)))));}break;case 257: _LL345:{
struct Cyc_Absyn_JoinEff_struct _tmpD6C;struct Cyc_Absyn_JoinEff_struct*_tmpD6B;
yyval=Cyc_YY44((void*)((_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B[0]=((
_tmpD6C.tag=24,((_tmpD6C.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpD6C)))),_tmpD6B)))));}break;case 258: _LL346:{struct
Cyc_Absyn_RgnsEff_struct _tmpD6F;struct Cyc_Absyn_RgnsEff_struct*_tmpD6E;yyval=Cyc_YY44((
void*)((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E[0]=((_tmpD6F.tag=25,((
_tmpD6F.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpD6F)))),_tmpD6E)))));}break;case 259: _LL347:{struct Cyc_Absyn_JoinEff_struct
_tmpD75;struct Cyc_List_List*_tmpD74;struct Cyc_Absyn_JoinEff_struct*_tmpD73;yyval=
Cyc_YY44((void*)((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73[0]=((_tmpD75.tag=
24,((_tmpD75.f1=((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74->hd=Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD74->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD74)))))),_tmpD75)))),
_tmpD73)))));}break;case 260: _LL348:{struct Cyc_List_List*_tmpD76;yyval=Cyc_YY40(((
_tmpD76=_cycalloc(sizeof(*_tmpD76)),((_tmpD76->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD76->tl=0,_tmpD76)))))));}
break;case 261: _LL349:{struct Cyc_List_List*_tmpD77;yyval=Cyc_YY40(((_tmpD77=
_cycalloc(sizeof(*_tmpD77)),((_tmpD77->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD77->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD77)))))));}
break;case 262: _LL34A:{struct Cyc_Parse_Abstractdeclarator*_tmpD78;yyval=Cyc_YY30(((
_tmpD78=_cycalloc(sizeof(*_tmpD78)),((_tmpD78->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD78)))));}break;case 263:
_LL34B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
264: _LL34C:{struct Cyc_Parse_Abstractdeclarator*_tmpD79;yyval=Cyc_YY30(((_tmpD79=
_cycalloc(sizeof(*_tmpD79)),((_tmpD79->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpD79)))));}break;
case 265: _LL34D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 266: _LL34E:{struct Cyc_List_List*_tmpD83;struct Cyc_Absyn_Carray_mod_struct
_tmpD82;struct Cyc_Absyn_Carray_mod_struct*_tmpD81;struct Cyc_Parse_Abstractdeclarator*
_tmpD80;yyval=Cyc_YY30(((_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80->tms=((
_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83->hd=(void*)((_tmpD81=_cycalloc(
sizeof(*_tmpD81)),((_tmpD81[0]=((_tmpD82.tag=0,((_tmpD82.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD82.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD82)))))),
_tmpD81)))),((_tmpD83->tl=0,_tmpD83)))))),_tmpD80)))));}break;case 267: _LL34F:{
struct Cyc_List_List*_tmpD8D;struct Cyc_Absyn_Carray_mod_struct _tmpD8C;struct Cyc_Absyn_Carray_mod_struct*
_tmpD8B;struct Cyc_Parse_Abstractdeclarator*_tmpD8A;yyval=Cyc_YY30(((_tmpD8A=
_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A->tms=((_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((
_tmpD8D->hd=(void*)((_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B[0]=((_tmpD8C.tag=
0,((_tmpD8C.f1=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD8C.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD8C)))))),
_tmpD8B)))),((_tmpD8D->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD8D)))))),_tmpD8A)))));}break;case 268: _LL350:{
struct Cyc_List_List*_tmpD97;struct Cyc_Absyn_ConstArray_mod_struct _tmpD96;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD95;struct Cyc_Parse_Abstractdeclarator*
_tmpD94;yyval=Cyc_YY30(((_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94->tms=((
_tmpD97=_cycalloc(sizeof(*_tmpD97)),((_tmpD97->hd=(void*)((_tmpD95=_cycalloc(
sizeof(*_tmpD95)),((_tmpD95[0]=((_tmpD96.tag=1,((_tmpD96.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD96.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD96.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD96)))))))),
_tmpD95)))),((_tmpD97->tl=0,_tmpD97)))))),_tmpD94)))));}break;case 269: _LL351:{
struct Cyc_List_List*_tmpDA1;struct Cyc_Absyn_ConstArray_mod_struct _tmpDA0;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD9F;struct Cyc_Parse_Abstractdeclarator*
_tmpD9E;yyval=Cyc_YY30(((_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->tms=((
_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1->hd=(void*)((_tmpD9F=_cycalloc(
sizeof(*_tmpD9F)),((_tmpD9F[0]=((_tmpDA0.tag=1,((_tmpDA0.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDA0.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDA0.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDA0)))))))),
_tmpD9F)))),((_tmpDA1->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpDA1)))))),_tmpD9E)))));}break;case 270: _LL352:{
struct Cyc_List_List*_tmpDB6;struct Cyc_Absyn_Function_mod_struct _tmpDB5;struct Cyc_Absyn_WithTypes_struct
_tmpDB4;struct Cyc_Absyn_WithTypes_struct*_tmpDB3;struct Cyc_Absyn_Function_mod_struct*
_tmpDB2;struct Cyc_Parse_Abstractdeclarator*_tmpDB1;yyval=Cyc_YY30(((_tmpDB1=
_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1->tms=((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((
_tmpDB6->hd=(void*)((_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2[0]=((_tmpDB5.tag=
3,((_tmpDB5.f1=(void*)((void*)((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3[0]=((
_tmpDB4.tag=1,((_tmpDB4.f1=0,((_tmpDB4.f2=0,((_tmpDB4.f3=0,((_tmpDB4.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDB4.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDB4)))))))))))),
_tmpDB3))))),_tmpDB5)))),_tmpDB2)))),((_tmpDB6->tl=0,_tmpDB6)))))),_tmpDB1)))));}
break;case 271: _LL353: {struct _tuple24 _tmp730;struct Cyc_List_List*_tmp731;int
_tmp732;struct Cyc_Absyn_VarargInfo*_tmp733;struct Cyc_Core_Opt*_tmp734;struct Cyc_List_List*
_tmp735;struct _tuple24*_tmp72F=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp730=*_tmp72F;_tmp731=
_tmp730.f1;_tmp732=_tmp730.f2;_tmp733=_tmp730.f3;_tmp734=_tmp730.f4;_tmp735=
_tmp730.f5;{struct Cyc_List_List*_tmpDCB;struct Cyc_Absyn_Function_mod_struct
_tmpDCA;struct Cyc_Absyn_WithTypes_struct _tmpDC9;struct Cyc_Absyn_WithTypes_struct*
_tmpDC8;struct Cyc_Absyn_Function_mod_struct*_tmpDC7;struct Cyc_Parse_Abstractdeclarator*
_tmpDC6;yyval=Cyc_YY30(((_tmpDC6=_cycalloc(sizeof(*_tmpDC6)),((_tmpDC6->tms=((
_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB->hd=(void*)((_tmpDC7=_cycalloc(
sizeof(*_tmpDC7)),((_tmpDC7[0]=((_tmpDCA.tag=3,((_tmpDCA.f1=(void*)((void*)((
_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8[0]=((_tmpDC9.tag=1,((_tmpDC9.f1=
_tmp731,((_tmpDC9.f2=_tmp732,((_tmpDC9.f3=_tmp733,((_tmpDC9.f4=_tmp734,((_tmpDC9.f5=
_tmp735,_tmpDC9)))))))))))),_tmpDC8))))),_tmpDCA)))),_tmpDC7)))),((_tmpDCB->tl=0,
_tmpDCB)))))),_tmpDC6)))));}break;}case 272: _LL354:{struct Cyc_List_List*_tmpDE0;
struct Cyc_Absyn_Function_mod_struct _tmpDDF;struct Cyc_Absyn_WithTypes_struct
_tmpDDE;struct Cyc_Absyn_WithTypes_struct*_tmpDDD;struct Cyc_Absyn_Function_mod_struct*
_tmpDDC;struct Cyc_Parse_Abstractdeclarator*_tmpDDB;yyval=Cyc_YY30(((_tmpDDB=
_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB->tms=((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((
_tmpDE0->hd=(void*)((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC[0]=((_tmpDDF.tag=
3,((_tmpDDF.f1=(void*)((void*)((_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD[0]=((
_tmpDDE.tag=1,((_tmpDDE.f1=0,((_tmpDDE.f2=0,((_tmpDDE.f3=0,((_tmpDDE.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDDE.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDDE)))))))))))),
_tmpDDD))))),_tmpDDF)))),_tmpDDC)))),((_tmpDE0->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpDE0)))))),
_tmpDDB)))));}break;case 273: _LL355: {struct _tuple24 _tmp743;struct Cyc_List_List*
_tmp744;int _tmp745;struct Cyc_Absyn_VarargInfo*_tmp746;struct Cyc_Core_Opt*_tmp747;
struct Cyc_List_List*_tmp748;struct _tuple24*_tmp742=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp743=*_tmp742;_tmp744=
_tmp743.f1;_tmp745=_tmp743.f2;_tmp746=_tmp743.f3;_tmp747=_tmp743.f4;_tmp748=
_tmp743.f5;{struct Cyc_List_List*_tmpDF5;struct Cyc_Absyn_Function_mod_struct
_tmpDF4;struct Cyc_Absyn_WithTypes_struct _tmpDF3;struct Cyc_Absyn_WithTypes_struct*
_tmpDF2;struct Cyc_Absyn_Function_mod_struct*_tmpDF1;struct Cyc_Parse_Abstractdeclarator*
_tmpDF0;yyval=Cyc_YY30(((_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((_tmpDF0->tms=((
_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5->hd=(void*)((_tmpDF1=_cycalloc(
sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF4.tag=3,((_tmpDF4.f1=(void*)((void*)((
_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2[0]=((_tmpDF3.tag=1,((_tmpDF3.f1=
_tmp744,((_tmpDF3.f2=_tmp745,((_tmpDF3.f3=_tmp746,((_tmpDF3.f4=_tmp747,((_tmpDF3.f5=
_tmp748,_tmpDF3)))))))))))),_tmpDF2))))),_tmpDF4)))),_tmpDF1)))),((_tmpDF5->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpDF5)))))),_tmpDF0)))));}break;}case 274: _LL356: {struct Cyc_List_List*_tmp74F=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{
struct Cyc_List_List*_tmpDFF;struct Cyc_Absyn_TypeParams_mod_struct _tmpDFE;struct
Cyc_Absyn_TypeParams_mod_struct*_tmpDFD;struct Cyc_Parse_Abstractdeclarator*
_tmpDFC;yyval=Cyc_YY30(((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC->tms=((
_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF->hd=(void*)((_tmpDFD=_cycalloc(
sizeof(*_tmpDFD)),((_tmpDFD[0]=((_tmpDFE.tag=4,((_tmpDFE.f1=_tmp74F,((_tmpDFE.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpDFE.f3=0,_tmpDFE)))))))),_tmpDFD)))),((_tmpDFF->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpDFF)))))),_tmpDFC)))));}break;}case 275: _LL357:{struct Cyc_List_List*_tmpE09;
struct Cyc_Absyn_Attributes_mod_struct _tmpE08;struct Cyc_Absyn_Attributes_mod_struct*
_tmpE07;struct Cyc_Parse_Abstractdeclarator*_tmpE06;yyval=Cyc_YY30(((_tmpE06=
_cycalloc(sizeof(*_tmpE06)),((_tmpE06->tms=((_tmpE09=_cycalloc(sizeof(*_tmpE09)),((
_tmpE09->hd=(void*)((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07[0]=((_tmpE08.tag=
5,((_tmpE08.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpE08.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE08)))))),_tmpE07)))),((
_tmpE09->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpE09)))))),_tmpE06)))));}break;case 276: _LL358: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 277: _LL359:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 278:
_LL35A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
279: _LL35B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 280: _LL35C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 281: _LL35D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 282: _LL35E:{struct Cyc_Absyn_ResetRegion_s_struct _tmpE0C;
struct Cyc_Absyn_ResetRegion_s_struct*_tmpE0B;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((
void*)((_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B[0]=((_tmpE0C.tag=16,((
_tmpE0C.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),_tmpE0C)))),_tmpE0B)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 283:
_LL35F: yyval=Cyc_YY4(0);break;case 284: _LL360:{const char*_tmpE0D;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmpE0D="open",_tag_dyneither(_tmpE0D,sizeof(char),5))))
!= 0){const char*_tmpE0E;Cyc_Parse_err(((_tmpE0E="expecting `open'",
_tag_dyneither(_tmpE0E,sizeof(char),17))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}yyval=Cyc_YY4((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));break;case 285: _LL361:{struct Cyc_Absyn_Label_s_struct _tmpE14;
struct _dyneither_ptr*_tmpE13;struct Cyc_Absyn_Label_s_struct*_tmpE12;yyval=Cyc_YY9(
Cyc_Absyn_new_stmt((void*)((_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12[0]=((
_tmpE14.tag=13,((_tmpE14.f1=((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpE13)))),((_tmpE14.f2=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE14)))))),_tmpE12)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 286:
_LL362: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 287:
_LL363: yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 288:
_LL364: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 289:
_LL365: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 290: _LL366: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0)));
break;case 291: _LL367: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 292: _LL368:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 293:
_LL369: yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 294:
_LL36A:{struct Cyc_Absyn_Fn_d_struct _tmpE17;struct Cyc_Absyn_Fn_d_struct*_tmpE16;
yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE16=
_cycalloc(sizeof(*_tmpE16)),((_tmpE16[0]=((_tmpE17.tag=1,((_tmpE17.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE17)))),_tmpE16)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0)));}break;case 295: _LL36B:{
struct Cyc_Absyn_Fn_d_struct _tmpE1A;struct Cyc_Absyn_Fn_d_struct*_tmpE19;yyval=Cyc_YY9(
Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE19=_cycalloc(sizeof(*
_tmpE19)),((_tmpE19[0]=((_tmpE1A.tag=1,((_tmpE1A.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE1A)))),_tmpE19)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));}break;case 296: _LL36C: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 297: _LL36D: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 298:
_LL36E: yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 299:
_LL36F: {struct Cyc_Absyn_UnknownId_e_struct _tmpE1D;struct Cyc_Absyn_UnknownId_e_struct*
_tmpE1C;struct Cyc_Absyn_Exp*_tmp763=Cyc_Absyn_new_exp((void*)((_tmpE1C=_cycalloc(
sizeof(*_tmpE1C)),((_tmpE1C[0]=((_tmpE1D.tag=2,((_tmpE1D.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpE1D)))),_tmpE1C)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp763,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 300:
_LL370: {struct Cyc_Absyn_Exp*_tmp766=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(
Cyc_Absyn_switch_stmt(_tmp766,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 301:
_LL371: yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 302:
_LL372: yyval=Cyc_YY10(0);break;case 303: _LL373:{struct Cyc_Absyn_Switch_clause*
_tmpE20;struct Cyc_List_List*_tmpE1F;yyval=Cyc_YY10(((_tmpE1F=_cycalloc(sizeof(*
_tmpE1F)),((_tmpE1F->hd=((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20->pattern=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)),((_tmpE20->pat_vars=
0,((_tmpE20->where_clause=0,((_tmpE20->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpE20->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE20)))))))))))),((
_tmpE1F->tl=0,_tmpE1F)))))));}break;case 304: _LL374:{struct Cyc_Absyn_Switch_clause*
_tmpE23;struct Cyc_List_List*_tmpE22;yyval=Cyc_YY10(((_tmpE22=_cycalloc(sizeof(*
_tmpE22)),((_tmpE22->hd=((_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpE23->pat_vars=0,((_tmpE23->where_clause=0,((_tmpE23->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpE23->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE23)))))))))))),((
_tmpE22->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE22)))))));}break;case 305: _LL375:{struct Cyc_Absyn_Switch_clause*
_tmpE26;struct Cyc_List_List*_tmpE25;yyval=Cyc_YY10(((_tmpE25=_cycalloc(sizeof(*
_tmpE25)),((_tmpE25->hd=((_tmpE26=_cycalloc(sizeof(*_tmpE26)),((_tmpE26->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpE26->pat_vars=0,((_tmpE26->where_clause=0,((_tmpE26->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE26->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpE26)))))))))))),((
_tmpE25->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE25)))))));}break;case 306: _LL376:{struct Cyc_Absyn_Switch_clause*
_tmpE29;struct Cyc_List_List*_tmpE28;yyval=Cyc_YY10(((_tmpE28=_cycalloc(sizeof(*
_tmpE28)),((_tmpE28->hd=((_tmpE29=_cycalloc(sizeof(*_tmpE29)),((_tmpE29->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((
_tmpE29->pat_vars=0,((_tmpE29->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpE29->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpE29->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE29)))))))))))),((
_tmpE28->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE28)))))));}break;case 307: _LL377:{struct Cyc_Absyn_Switch_clause*
_tmpE2C;struct Cyc_List_List*_tmpE2B;yyval=Cyc_YY10(((_tmpE2B=_cycalloc(sizeof(*
_tmpE2B)),((_tmpE2B->hd=((_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((
_tmpE2C->pat_vars=0,((_tmpE2C->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpE2C->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE2C->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE2C)))))))))))),((
_tmpE2B->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE2B)))))));}break;case 308: _LL378: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 309:
_LL379: yyval=Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 310:
_LL37A: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 311:
_LL37B: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 312:
_LL37C: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 313:
_LL37D: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
314: _LL37E: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 315: _LL37F: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 316:
_LL380: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 317:
_LL381: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 318:
_LL382: {struct Cyc_List_List*_tmp771=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp772=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp771,_tmp772));break;}case 319: _LL383: {struct
Cyc_List_List*_tmp773=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp774=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp773,
_tmp774));break;}case 320: _LL384: {struct Cyc_List_List*_tmp775=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp776=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp775,_tmp776));break;}case 321: _LL385: {struct
Cyc_List_List*_tmp777=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp778=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp777,_tmp778));break;}case 322: _LL386:{struct
_dyneither_ptr*_tmpE2D;yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((_tmpE2D=_cycalloc(
sizeof(*_tmpE2D)),((_tmpE2D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE2D)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));}break;case
323: _LL387: yyval=Cyc_YY9(Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
324: _LL388: yyval=Cyc_YY9(Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
325: _LL389: yyval=Cyc_YY9(Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
326: _LL38A: yyval=Cyc_YY9(Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
327: _LL38B: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
328: _LL38C: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)));break;case
329: _LL38D: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)));break;case
330: _LL38E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 331: _LL38F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 332: _LL390: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 333: _LL391: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 334: _LL392: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 335:
_LL393: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
336: _LL394: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 337:
_LL395: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
338: _LL396: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 339: _LL397: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 340: _LL398: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 341:
_LL399: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
342: _LL39A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 343: _LL39B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 344: _LL39C: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 345:
_LL39D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 346:
_LL39E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
347: _LL39F: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 348:
_LL3A0: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 349:
_LL3A1: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 350:
_LL3A2: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 351:
_LL3A3: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
352: _LL3A4: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 353: _LL3A5: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 354:
_LL3A6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
355: _LL3A7: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 356: _LL3A8: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 357:
_LL3A9: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
358: _LL3AA: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 359: _LL3AB: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 360:
_LL3AC: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 361:
_LL3AD: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
362: _LL3AE: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 363: _LL3AF: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 364: _LL3B0: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));break;case 365: _LL3B1: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 366: _LL3B2: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 367:
_LL3B3: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 368:
_LL3B4:{struct Cyc_Absyn_StructField_struct _tmpE33;struct _dyneither_ptr*_tmpE32;
struct Cyc_Absyn_StructField_struct*_tmpE31;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31[0]=((_tmpE33.tag=0,((
_tmpE33.f1=((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE32)))),_tmpE33)))),
_tmpE31)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 369: _LL3B5:{struct Cyc_Absyn_TupleIndex_struct
_tmpE36;struct Cyc_Absyn_TupleIndex_struct*_tmpE35;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)((_tmpE35=_cycalloc_atomic(sizeof(*_tmpE35)),((
_tmpE35[0]=((_tmpE36.tag=1,((_tmpE36.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,_tmpE36)))),_tmpE35)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 370: _LL3B6: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 371: _LL3B7: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 372: _LL3B8:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 373:
_LL3B9: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 374: _LL3BA: {
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);{void*_tmp77F=e->r;union Cyc_Absyn_Cnst _tmp781;struct _tuple4
_tmp782;enum Cyc_Absyn_Sign _tmp783;char _tmp784;union Cyc_Absyn_Cnst _tmp786;struct
_tuple5 _tmp787;enum Cyc_Absyn_Sign _tmp788;short _tmp789;union Cyc_Absyn_Cnst
_tmp78B;struct _tuple6 _tmp78C;enum Cyc_Absyn_Sign _tmp78D;int _tmp78E;union Cyc_Absyn_Cnst
_tmp790;struct _dyneither_ptr _tmp791;union Cyc_Absyn_Cnst _tmp793;int _tmp794;union
Cyc_Absyn_Cnst _tmp796;struct _dyneither_ptr _tmp797;union Cyc_Absyn_Cnst _tmp799;
struct _tuple7 _tmp79A;_LL3BD: {struct Cyc_Absyn_Const_e_struct*_tmp780=(struct Cyc_Absyn_Const_e_struct*)
_tmp77F;if(_tmp780->tag != 0)goto _LL3BF;else{_tmp781=_tmp780->f1;if((_tmp781.Char_c).tag
!= 2)goto _LL3BF;_tmp782=(struct _tuple4)(_tmp781.Char_c).val;_tmp783=_tmp782.f1;
_tmp784=_tmp782.f2;}}_LL3BE:{struct Cyc_Absyn_Char_p_struct _tmpE39;struct Cyc_Absyn_Char_p_struct*
_tmpE38;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE38=_cycalloc_atomic(
sizeof(*_tmpE38)),((_tmpE38[0]=((_tmpE39.tag=10,((_tmpE39.f1=_tmp784,_tmpE39)))),
_tmpE38)))),e->loc));}goto _LL3BC;_LL3BF: {struct Cyc_Absyn_Const_e_struct*_tmp785=(
struct Cyc_Absyn_Const_e_struct*)_tmp77F;if(_tmp785->tag != 0)goto _LL3C1;else{
_tmp786=_tmp785->f1;if((_tmp786.Short_c).tag != 3)goto _LL3C1;_tmp787=(struct
_tuple5)(_tmp786.Short_c).val;_tmp788=_tmp787.f1;_tmp789=_tmp787.f2;}}_LL3C0:{
struct Cyc_Absyn_Int_p_struct _tmpE3C;struct Cyc_Absyn_Int_p_struct*_tmpE3B;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE3B=_cycalloc_atomic(sizeof(*_tmpE3B)),((
_tmpE3B[0]=((_tmpE3C.tag=9,((_tmpE3C.f1=_tmp788,((_tmpE3C.f2=(int)_tmp789,
_tmpE3C)))))),_tmpE3B)))),e->loc));}goto _LL3BC;_LL3C1: {struct Cyc_Absyn_Const_e_struct*
_tmp78A=(struct Cyc_Absyn_Const_e_struct*)_tmp77F;if(_tmp78A->tag != 0)goto _LL3C3;
else{_tmp78B=_tmp78A->f1;if((_tmp78B.Int_c).tag != 4)goto _LL3C3;_tmp78C=(struct
_tuple6)(_tmp78B.Int_c).val;_tmp78D=_tmp78C.f1;_tmp78E=_tmp78C.f2;}}_LL3C2:{
struct Cyc_Absyn_Int_p_struct _tmpE3F;struct Cyc_Absyn_Int_p_struct*_tmpE3E;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE3E=_cycalloc_atomic(sizeof(*_tmpE3E)),((
_tmpE3E[0]=((_tmpE3F.tag=9,((_tmpE3F.f1=_tmp78D,((_tmpE3F.f2=_tmp78E,_tmpE3F)))))),
_tmpE3E)))),e->loc));}goto _LL3BC;_LL3C3: {struct Cyc_Absyn_Const_e_struct*_tmp78F=(
struct Cyc_Absyn_Const_e_struct*)_tmp77F;if(_tmp78F->tag != 0)goto _LL3C5;else{
_tmp790=_tmp78F->f1;if((_tmp790.Float_c).tag != 6)goto _LL3C5;_tmp791=(struct
_dyneither_ptr)(_tmp790.Float_c).val;}}_LL3C4:{struct Cyc_Absyn_Float_p_struct
_tmpE42;struct Cyc_Absyn_Float_p_struct*_tmpE41;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41[0]=((_tmpE42.tag=11,((
_tmpE42.f1=_tmp791,_tmpE42)))),_tmpE41)))),e->loc));}goto _LL3BC;_LL3C5: {struct
Cyc_Absyn_Const_e_struct*_tmp792=(struct Cyc_Absyn_Const_e_struct*)_tmp77F;if(
_tmp792->tag != 0)goto _LL3C7;else{_tmp793=_tmp792->f1;if((_tmp793.Null_c).tag != 1)
goto _LL3C7;_tmp794=(int)(_tmp793.Null_c).val;}}_LL3C6: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3BC;_LL3C7: {struct Cyc_Absyn_Const_e_struct*
_tmp795=(struct Cyc_Absyn_Const_e_struct*)_tmp77F;if(_tmp795->tag != 0)goto _LL3C9;
else{_tmp796=_tmp795->f1;if((_tmp796.String_c).tag != 7)goto _LL3C9;_tmp797=(
struct _dyneither_ptr)(_tmp796.String_c).val;}}_LL3C8:{const char*_tmpE43;Cyc_Parse_err(((
_tmpE43="strings cannot occur within patterns",_tag_dyneither(_tmpE43,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL3BC;_LL3C9: {struct Cyc_Absyn_Const_e_struct*
_tmp798=(struct Cyc_Absyn_Const_e_struct*)_tmp77F;if(_tmp798->tag != 0)goto _LL3CB;
else{_tmp799=_tmp798->f1;if((_tmp799.LongLong_c).tag != 5)goto _LL3CB;_tmp79A=(
struct _tuple7)(_tmp799.LongLong_c).val;}}_LL3CA:{const char*_tmpE44;Cyc_Parse_unimp(((
_tmpE44="long long's in patterns",_tag_dyneither(_tmpE44,sizeof(char),24))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}goto _LL3BC;
_LL3CB:;_LL3CC: {const char*_tmpE45;Cyc_Parse_err(((_tmpE45="bad constant in case",
_tag_dyneither(_tmpE45,sizeof(char),21))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}_LL3BC:;}
break;}case 375: _LL3BB:{struct Cyc_Absyn_UnknownId_p_struct _tmpE48;struct Cyc_Absyn_UnknownId_p_struct*
_tmpE47;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE47=_cycalloc(sizeof(*
_tmpE47)),((_tmpE47[0]=((_tmpE48.tag=14,((_tmpE48.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE48)))),_tmpE47)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 376:
_LL3CD:{const char*_tmpE49;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE49="as",
_tag_dyneither(_tmpE49,sizeof(char),3))))!= 0){const char*_tmpE4A;Cyc_Parse_err(((
_tmpE4A="expecting `as'",_tag_dyneither(_tmpE4A,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Var_p_struct
_tmpE54;struct _dyneither_ptr*_tmpE53;struct _tuple1*_tmpE52;struct Cyc_Absyn_Var_p_struct*
_tmpE51;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE51=_cycalloc(sizeof(*
_tmpE51)),((_tmpE51[0]=((_tmpE54.tag=1,((_tmpE54.f1=Cyc_Absyn_new_vardecl(((
_tmpE52=_cycalloc(sizeof(*_tmpE52)),((_tmpE52->f1=Cyc_Absyn_Loc_n,((_tmpE52->f2=((
_tmpE53=_cycalloc(sizeof(*_tmpE53)),((_tmpE53[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE53)))),_tmpE52)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE54.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE54)))))),_tmpE51)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 377: _LL3CE: {struct Cyc_List_List*
_tmp7AF;int _tmp7B0;struct _tuple20 _tmp7AE=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7AF=_tmp7AE.f1;
_tmp7B0=_tmp7AE.f2;{struct Cyc_Absyn_Tuple_p_struct _tmpE57;struct Cyc_Absyn_Tuple_p_struct*
_tmpE56;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE56=_cycalloc(sizeof(*
_tmpE56)),((_tmpE56[0]=((_tmpE57.tag=4,((_tmpE57.f1=_tmp7AF,((_tmpE57.f2=_tmp7B0,
_tmpE57)))))),_tmpE56)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
378: _LL3CF: {struct Cyc_List_List*_tmp7B4;int _tmp7B5;struct _tuple20 _tmp7B3=*Cyc_yyget_YY12(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7B4=_tmp7B3.f1;
_tmp7B5=_tmp7B3.f2;{struct Cyc_Absyn_UnknownCall_p_struct _tmpE5A;struct Cyc_Absyn_UnknownCall_p_struct*
_tmpE59;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE59=_cycalloc(sizeof(*
_tmpE59)),((_tmpE59[0]=((_tmpE5A.tag=15,((_tmpE5A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpE5A.f2=_tmp7B4,((
_tmpE5A.f3=_tmp7B5,_tmpE5A)))))))),_tmpE59)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
379: _LL3D0: {struct Cyc_List_List*_tmp7B9;int _tmp7BA;struct _tuple20 _tmp7B8=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7B9=_tmp7B8.f1;
_tmp7BA=_tmp7B8.f2;{struct Cyc_List_List*_tmp7BB=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpE60;struct Cyc_Absyn_AggrInfo*_tmpE5F;struct Cyc_Absyn_Aggr_p_struct*_tmpE5E;
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE5E=_cycalloc(sizeof(*_tmpE5E)),((
_tmpE5E[0]=((_tmpE60.tag=6,((_tmpE60.f1=((_tmpE5F=_cycalloc(sizeof(*_tmpE5F)),((
_tmpE5F->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),0),((_tmpE5F->targs=
0,_tmpE5F)))))),((_tmpE60.f2=_tmp7BB,((_tmpE60.f3=_tmp7B9,((_tmpE60.f4=_tmp7BA,
_tmpE60)))))))))),_tmpE5E)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;};}case
380: _LL3D1: {struct Cyc_List_List*_tmp7C0;int _tmp7C1;struct _tuple20 _tmp7BF=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7C0=_tmp7BF.f1;
_tmp7C1=_tmp7BF.f2;{struct Cyc_List_List*_tmp7C2=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpE63;struct Cyc_Absyn_Aggr_p_struct*_tmpE62;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE62=_cycalloc(sizeof(*_tmpE62)),((_tmpE62[0]=((_tmpE63.tag=6,((
_tmpE63.f1=0,((_tmpE63.f2=_tmp7C2,((_tmpE63.f3=_tmp7C0,((_tmpE63.f4=_tmp7C1,
_tmpE63)))))))))),_tmpE62)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;};}case
381: _LL3D2:{struct Cyc_Absyn_Pointer_p_struct _tmpE66;struct Cyc_Absyn_Pointer_p_struct*
_tmpE65;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE65=_cycalloc(sizeof(*
_tmpE65)),((_tmpE65[0]=((_tmpE66.tag=5,((_tmpE66.f1=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE66)))),_tmpE65)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 382:
_LL3D3:{struct Cyc_Absyn_Pointer_p_struct _tmpE70;struct Cyc_Absyn_Pointer_p_struct
_tmpE6F;struct Cyc_Absyn_Pointer_p_struct*_tmpE6E;struct Cyc_Absyn_Pointer_p_struct*
_tmpE6D;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE6D=_cycalloc(sizeof(*
_tmpE6D)),((_tmpE6D[0]=((_tmpE70.tag=5,((_tmpE70.f1=Cyc_Absyn_new_pat((void*)((
_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E[0]=((_tmpE6F.tag=5,((_tmpE6F.f1=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE6F)))),
_tmpE6E)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpE70)))),_tmpE6D)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 383:
_LL3D4:{struct Cyc_Absyn_Reference_p_struct _tmpE7A;struct _dyneither_ptr*_tmpE79;
struct _tuple1*_tmpE78;struct Cyc_Absyn_Reference_p_struct*_tmpE77;yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)((_tmpE77=_cycalloc(sizeof(*_tmpE77)),((_tmpE77[0]=((
_tmpE7A.tag=2,((_tmpE7A.f1=Cyc_Absyn_new_vardecl(((_tmpE78=_cycalloc(sizeof(*
_tmpE78)),((_tmpE78->f1=Cyc_Absyn_Loc_n,((_tmpE78->f2=((_tmpE79=_cycalloc(
sizeof(*_tmpE79)),((_tmpE79[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE79)))),_tmpE78)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE7A.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpE7A)))))),_tmpE77)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 384:
_LL3D5:{const char*_tmpE7B;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE7B="as",
_tag_dyneither(_tmpE7B,sizeof(char),3))))!= 0){const char*_tmpE7C;Cyc_Parse_err(((
_tmpE7C="expecting `as'",_tag_dyneither(_tmpE7C,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Reference_p_struct
_tmpE86;struct _dyneither_ptr*_tmpE85;struct _tuple1*_tmpE84;struct Cyc_Absyn_Reference_p_struct*
_tmpE83;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE83=_cycalloc(sizeof(*
_tmpE83)),((_tmpE83[0]=((_tmpE86.tag=2,((_tmpE86.f1=Cyc_Absyn_new_vardecl(((
_tmpE84=_cycalloc(sizeof(*_tmpE84)),((_tmpE84->f1=Cyc_Absyn_Loc_n,((_tmpE84->f2=((
_tmpE85=_cycalloc(sizeof(*_tmpE85)),((_tmpE85[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE85)))),_tmpE84)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE86.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE86)))))),_tmpE83)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 385: _LL3D6: {void*_tmp7D5=Cyc_Parse_id2type(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct
_tmpE97;struct Cyc_Absyn_TagType_struct _tmpE96;struct Cyc_Absyn_TagType_struct*
_tmpE95;struct _dyneither_ptr*_tmpE94;struct _tuple1*_tmpE93;struct Cyc_Absyn_TagInt_p_struct*
_tmpE92;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE92=_cycalloc(sizeof(*
_tmpE92)),((_tmpE92[0]=((_tmpE97.tag=3,((_tmpE97.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp7D5),((
_tmpE97.f2=Cyc_Absyn_new_vardecl(((_tmpE93=_cycalloc(sizeof(*_tmpE93)),((_tmpE93->f1=
Cyc_Absyn_Loc_n,((_tmpE93->f2=((_tmpE94=_cycalloc(sizeof(*_tmpE94)),((_tmpE94[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpE94)))),_tmpE93)))))),(void*)((_tmpE95=_cycalloc(sizeof(*_tmpE95)),((_tmpE95[
0]=((_tmpE96.tag=20,((_tmpE96.f1=(void*)_tmp7D5,_tmpE96)))),_tmpE95)))),0),
_tmpE97)))))),_tmpE92)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
386: _LL3D7: {struct Cyc_Absyn_Tvar*_tmp7DC=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(&
Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct _tmpEB3;struct Cyc_Absyn_TagType_struct
_tmpEB2;struct Cyc_Absyn_VarType_struct _tmpEB1;struct Cyc_Absyn_VarType_struct*
_tmpEB0;struct Cyc_Absyn_TagType_struct*_tmpEAF;struct _dyneither_ptr*_tmpEAE;
struct _tuple1*_tmpEAD;struct Cyc_Absyn_TagInt_p_struct*_tmpEAC;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpEAC=_cycalloc(sizeof(*_tmpEAC)),((_tmpEAC[0]=((_tmpEB3.tag=3,((
_tmpEB3.f1=_tmp7DC,((_tmpEB3.f2=Cyc_Absyn_new_vardecl(((_tmpEAD=_cycalloc(
sizeof(*_tmpEAD)),((_tmpEAD->f1=Cyc_Absyn_Loc_n,((_tmpEAD->f2=((_tmpEAE=
_cycalloc(sizeof(*_tmpEAE)),((_tmpEAE[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpEAE)))),_tmpEAD)))))),(
void*)((_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((_tmpEAF[0]=((_tmpEB2.tag=20,((
_tmpEB2.f1=(void*)((void*)((_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0[0]=((
_tmpEB1.tag=2,((_tmpEB1.f1=_tmp7DC,_tmpEB1)))),_tmpEB0))))),_tmpEB2)))),_tmpEAF)))),
0),_tmpEB3)))))),_tmpEAC)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
387: _LL3D8:{struct _tuple20*_tmpEB4;yyval=Cyc_YY12(((_tmpEB4=_cycalloc(sizeof(*
_tmpEB4)),((_tmpEB4->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpEB4->f2=0,_tmpEB4)))))));}break;case 388: _LL3D9:{struct _tuple20*_tmpEB5;yyval=
Cyc_YY12(((_tmpEB5=_cycalloc(sizeof(*_tmpEB5)),((_tmpEB5->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpEB5->f2=1,_tmpEB5)))))));}
break;case 389: _LL3DA:{struct _tuple20*_tmpEB6;yyval=Cyc_YY12(((_tmpEB6=_cycalloc(
sizeof(*_tmpEB6)),((_tmpEB6->f1=0,((_tmpEB6->f2=1,_tmpEB6)))))));}break;case 390:
_LL3DB:{struct Cyc_List_List*_tmpEB7;yyval=Cyc_YY13(((_tmpEB7=_cycalloc(sizeof(*
_tmpEB7)),((_tmpEB7->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpEB7->tl=0,_tmpEB7)))))));}break;case 391: _LL3DC:{struct Cyc_List_List*
_tmpEB8;yyval=Cyc_YY13(((_tmpEB8=_cycalloc(sizeof(*_tmpEB8)),((_tmpEB8->hd=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEB8->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpEB8)))))));}
break;case 392: _LL3DD:{struct _tuple21*_tmpEB9;yyval=Cyc_YY14(((_tmpEB9=_cycalloc(
sizeof(*_tmpEB9)),((_tmpEB9->f1=0,((_tmpEB9->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEB9)))))));}break;case
393: _LL3DE:{struct _tuple21*_tmpEBA;yyval=Cyc_YY14(((_tmpEBA=_cycalloc(sizeof(*
_tmpEBA)),((_tmpEBA->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpEBA->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEBA)))))));}break;case
394: _LL3DF:{struct _tuple20*_tmpEBB;yyval=Cyc_YY16(((_tmpEBB=_cycalloc(sizeof(*
_tmpEBB)),((_tmpEBB->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpEBB->f2=0,_tmpEBB)))))));}break;case 395: _LL3E0:{struct _tuple20*_tmpEBC;yyval=
Cyc_YY16(((_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpEBC->f2=1,_tmpEBC)))))));}
break;case 396: _LL3E1:{struct _tuple20*_tmpEBD;yyval=Cyc_YY16(((_tmpEBD=_cycalloc(
sizeof(*_tmpEBD)),((_tmpEBD->f1=0,((_tmpEBD->f2=1,_tmpEBD)))))));}break;case 397:
_LL3E2:{struct Cyc_List_List*_tmpEBE;yyval=Cyc_YY15(((_tmpEBE=_cycalloc(sizeof(*
_tmpEBE)),((_tmpEBE->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpEBE->tl=0,_tmpEBE)))))));}break;case 398: _LL3E3:{struct Cyc_List_List*
_tmpEBF;yyval=Cyc_YY15(((_tmpEBF=_cycalloc(sizeof(*_tmpEBF)),((_tmpEBF->hd=Cyc_yyget_YY14(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEBF->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpEBF)))))));}
break;case 399: _LL3E4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 400: _LL3E5: yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 401:
_LL3E6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
402: _LL3E7: yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 403:
_LL3E8: yyval=Cyc_YY3(Cyc_Absyn_swap_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 404:
_LL3E9: yyval=Cyc_YY8(0);break;case 405: _LL3EA:{struct Cyc_Core_Opt*_tmpEC0;yyval=
Cyc_YY8(((_tmpEC0=_cycalloc_atomic(sizeof(*_tmpEC0)),((_tmpEC0->v=(void*)Cyc_Absyn_Times,
_tmpEC0)))));}break;case 406: _LL3EB:{struct Cyc_Core_Opt*_tmpEC1;yyval=Cyc_YY8(((
_tmpEC1=_cycalloc_atomic(sizeof(*_tmpEC1)),((_tmpEC1->v=(void*)Cyc_Absyn_Div,
_tmpEC1)))));}break;case 407: _LL3EC:{struct Cyc_Core_Opt*_tmpEC2;yyval=Cyc_YY8(((
_tmpEC2=_cycalloc_atomic(sizeof(*_tmpEC2)),((_tmpEC2->v=(void*)Cyc_Absyn_Mod,
_tmpEC2)))));}break;case 408: _LL3ED:{struct Cyc_Core_Opt*_tmpEC3;yyval=Cyc_YY8(((
_tmpEC3=_cycalloc_atomic(sizeof(*_tmpEC3)),((_tmpEC3->v=(void*)Cyc_Absyn_Plus,
_tmpEC3)))));}break;case 409: _LL3EE:{struct Cyc_Core_Opt*_tmpEC4;yyval=Cyc_YY8(((
_tmpEC4=_cycalloc_atomic(sizeof(*_tmpEC4)),((_tmpEC4->v=(void*)Cyc_Absyn_Minus,
_tmpEC4)))));}break;case 410: _LL3EF:{struct Cyc_Core_Opt*_tmpEC5;yyval=Cyc_YY8(((
_tmpEC5=_cycalloc_atomic(sizeof(*_tmpEC5)),((_tmpEC5->v=(void*)Cyc_Absyn_Bitlshift,
_tmpEC5)))));}break;case 411: _LL3F0:{struct Cyc_Core_Opt*_tmpEC6;yyval=Cyc_YY8(((
_tmpEC6=_cycalloc_atomic(sizeof(*_tmpEC6)),((_tmpEC6->v=(void*)Cyc_Absyn_Bitlrshift,
_tmpEC6)))));}break;case 412: _LL3F1:{struct Cyc_Core_Opt*_tmpEC7;yyval=Cyc_YY8(((
_tmpEC7=_cycalloc_atomic(sizeof(*_tmpEC7)),((_tmpEC7->v=(void*)Cyc_Absyn_Bitand,
_tmpEC7)))));}break;case 413: _LL3F2:{struct Cyc_Core_Opt*_tmpEC8;yyval=Cyc_YY8(((
_tmpEC8=_cycalloc_atomic(sizeof(*_tmpEC8)),((_tmpEC8->v=(void*)Cyc_Absyn_Bitxor,
_tmpEC8)))));}break;case 414: _LL3F3:{struct Cyc_Core_Opt*_tmpEC9;yyval=Cyc_YY8(((
_tmpEC9=_cycalloc_atomic(sizeof(*_tmpEC9)),((_tmpEC9->v=(void*)Cyc_Absyn_Bitor,
_tmpEC9)))));}break;case 415: _LL3F4: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 416: _LL3F5: yyval=Cyc_YY3(Cyc_Absyn_conditional_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 417:
_LL3F6: yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 418:
_LL3F7: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 419:
_LL3F8: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 420:
_LL3F9: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 421:
_LL3FA: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 422:
_LL3FB: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
423: _LL3FC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 424: _LL3FD: yyval=Cyc_YY3(Cyc_Absyn_or_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 425:
_LL3FE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
426: _LL3FF: yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 427:
_LL400: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
428: _LL401: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 429:
_LL402: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
430: _LL403: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 431:
_LL404: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
432: _LL405: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 433:
_LL406: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
434: _LL407: yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 435:
_LL408: yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 436:
_LL409: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
437: _LL40A: yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 438:
_LL40B: yyval=Cyc_YY3(Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 439:
_LL40C: yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 440:
_LL40D: yyval=Cyc_YY3(Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 441:
_LL40E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
442: _LL40F: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 443:
_LL410: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 444:
_LL411: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
445: _LL412: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 446:
_LL413: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 447:
_LL414: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
448: _LL415: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 449:
_LL416: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 450:
_LL417: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 451:
_LL418: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
452: _LL419: yyval=Cyc_YY3(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 453: _LL41A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 454: _LL41B: yyval=
Cyc_YY3(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 455:
_LL41C: yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 456:
_LL41D: yyval=Cyc_YY3(Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 457:
_LL41E: yyval=Cyc_YY3(Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 458:
_LL41F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
459: _LL420: yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 460:
_LL421: yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 461:
_LL422: yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 462:
_LL423:{struct Cyc_Absyn_StructField_struct _tmpECF;struct _dyneither_ptr*_tmpECE;
struct Cyc_Absyn_StructField_struct*_tmpECD;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpECD=_cycalloc(sizeof(*_tmpECD)),((_tmpECD[0]=((_tmpECF.tag=0,((
_tmpECF.f1=((_tmpECE=_cycalloc(sizeof(*_tmpECE)),((_tmpECE[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpECE)))),_tmpECF)))),
_tmpECD)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 463: _LL424:{struct Cyc_Absyn_TupleIndex_struct
_tmpED2;struct Cyc_Absyn_TupleIndex_struct*_tmpED1;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpED1=_cycalloc_atomic(sizeof(*_tmpED1)),((_tmpED1[0]=((_tmpED2.tag=1,((
_tmpED2.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2,_tmpED2)))),_tmpED1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 464:
_LL425:{struct Cyc_Absyn_Malloc_e_struct _tmpED8;struct Cyc_Absyn_MallocInfo _tmpED7;
struct Cyc_Absyn_Malloc_e_struct*_tmpED6;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpED6=_cycalloc(sizeof(*_tmpED6)),((_tmpED6[0]=((_tmpED8.tag=34,((_tmpED8.f1=((
_tmpED7.is_calloc=0,((_tmpED7.rgn=0,((_tmpED7.elt_type=0,((_tmpED7.num_elts=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpED7.fat_result=
0,_tmpED7)))))))))),_tmpED8)))),_tmpED6)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 465:
_LL426:{struct Cyc_Absyn_Malloc_e_struct _tmpEDE;struct Cyc_Absyn_MallocInfo _tmpEDD;
struct Cyc_Absyn_Malloc_e_struct*_tmpEDC;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpEDC=_cycalloc(sizeof(*_tmpEDC)),((_tmpEDC[0]=((_tmpEDE.tag=34,((_tmpEDE.f1=((
_tmpEDD.is_calloc=0,((_tmpEDD.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpEDD.elt_type=0,((
_tmpEDD.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpEDD.fat_result=0,_tmpEDD)))))))))),_tmpEDE)))),_tmpEDC)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 466: _LL427: {void*_tmp807;struct _tuple8
_tmp806=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp807=_tmp806.f3;{struct Cyc_Absyn_Malloc_e_struct _tmpEE8;void**_tmpEE7;
struct Cyc_Absyn_MallocInfo _tmpEE6;struct Cyc_Absyn_Malloc_e_struct*_tmpEE5;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEE5=_cycalloc(sizeof(*_tmpEE5)),((_tmpEE5[
0]=((_tmpEE8.tag=34,((_tmpEE8.f1=((_tmpEE6.is_calloc=1,((_tmpEE6.rgn=0,((_tmpEE6.elt_type=((
_tmpEE7=_cycalloc(sizeof(*_tmpEE7)),((_tmpEE7[0]=_tmp807,_tmpEE7)))),((_tmpEE6.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((
_tmpEE6.fat_result=0,_tmpEE6)))))))))),_tmpEE8)))),_tmpEE5)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
467: _LL428: {void*_tmp80D;struct _tuple8 _tmp80C=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp80D=_tmp80C.f3;{
struct Cyc_Absyn_Malloc_e_struct _tmpEF2;void**_tmpEF1;struct Cyc_Absyn_MallocInfo
_tmpEF0;struct Cyc_Absyn_Malloc_e_struct*_tmpEEF;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpEEF=_cycalloc(sizeof(*_tmpEEF)),((_tmpEEF[0]=((_tmpEF2.tag=34,((
_tmpEF2.f1=((_tmpEF0.is_calloc=1,((_tmpEF0.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),((_tmpEF0.elt_type=((
_tmpEF1=_cycalloc(sizeof(*_tmpEF1)),((_tmpEF1[0]=_tmp80D,_tmpEF1)))),((_tmpEF0.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((
_tmpEF0.fat_result=0,_tmpEF0)))))))))),_tmpEF2)))),_tmpEEF)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
468: _LL429:{struct Cyc_Absyn_Exp*_tmpEF3[1];yyval=Cyc_YY3(Cyc_Absyn_primop_exp(
Cyc_Absyn_Numelts,((_tmpEF3[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpEF3,sizeof(struct Cyc_Absyn_Exp*),1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 469:
_LL42A:{struct Cyc_Absyn_Tagcheck_e_struct _tmpEF9;struct _dyneither_ptr*_tmpEF8;
struct Cyc_Absyn_Tagcheck_e_struct*_tmpEF7;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpEF7=_cycalloc(sizeof(*_tmpEF7)),((_tmpEF7[0]=((_tmpEF9.tag=38,((_tmpEF9.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpEF9.f2=((_tmpEF8=_cycalloc(sizeof(*_tmpEF8)),((_tmpEF8[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpEF8)))),_tmpEF9)))))),
_tmpEF7)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 470: _LL42B:{struct Cyc_Absyn_Tagcheck_e_struct
_tmpEFF;struct _dyneither_ptr*_tmpEFE;struct Cyc_Absyn_Tagcheck_e_struct*_tmpEFD;
yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEFD=_cycalloc(sizeof(*_tmpEFD)),((
_tmpEFD[0]=((_tmpEFF.tag=38,((_tmpEFF.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)),((_tmpEFF.f2=((
_tmpEFE=_cycalloc(sizeof(*_tmpEFE)),((_tmpEFE[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpEFE)))),_tmpEFF)))))),
_tmpEFD)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 471: _LL42C: {void*_tmp81A;struct _tuple8
_tmp819=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp81A=_tmp819.f3;yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(_tmp81A,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 472:
_LL42D: {int _tmp81C;struct _dyneither_ptr _tmp81D;struct _tuple18 _tmp81B=Cyc_yyget_Asm_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp81C=_tmp81B.f1;
_tmp81D=_tmp81B.f2;yyval=Cyc_YY3(Cyc_Absyn_asm_exp(_tmp81C,_tmp81D,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 473:
_LL42E: yyval=Cyc_YY7(Cyc_Absyn_Bitnot);break;case 474: _LL42F: yyval=Cyc_YY7(Cyc_Absyn_Not);
break;case 475: _LL430: yyval=Cyc_YY7(Cyc_Absyn_Minus);break;case 476: _LL431: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 477: _LL432:
yyval=Cyc_YY3(Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 478:
_LL433: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 479:
_LL434: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 480:
_LL435:{struct _dyneither_ptr*_tmpF00;yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpF00=_cycalloc(
sizeof(*_tmpF00)),((_tmpF00[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF00)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 481:
_LL436:{struct _dyneither_ptr*_tmpF01;yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpF01=_cycalloc(
sizeof(*_tmpF01)),((_tmpF01[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF01)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 482:
_LL437: yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 483:
_LL438: yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 484:
_LL439:{struct Cyc_Absyn_CompoundLit_e_struct _tmpF04;struct Cyc_Absyn_CompoundLit_e_struct*
_tmpF03;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpF03=_cycalloc(sizeof(*
_tmpF03)),((_tmpF03[0]=((_tmpF04.tag=26,((_tmpF04.f1=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpF04.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmpF04)))))),_tmpF03)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 485: _LL43A:{struct Cyc_Absyn_CompoundLit_e_struct
_tmpF07;struct Cyc_Absyn_CompoundLit_e_struct*_tmpF06;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpF06=_cycalloc(sizeof(*_tmpF06)),((_tmpF06[0]=((_tmpF07.tag=26,((
_tmpF07.f1=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 5)]),((_tmpF07.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),
_tmpF07)))))),_tmpF06)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 486:
_LL43B:{struct Cyc_Absyn_UnknownId_e_struct _tmpF0A;struct Cyc_Absyn_UnknownId_e_struct*
_tmpF09;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpF09=_cycalloc(sizeof(*
_tmpF09)),((_tmpF09[0]=((_tmpF0A.tag=2,((_tmpF0A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF0A)))),_tmpF09)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 487:
_LL43C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
488: _LL43D: yyval=Cyc_YY3(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 489:
_LL43E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 490: _LL43F: yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 491:
_LL440: yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 492:
_LL441: yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 493:
_LL442:{struct Cyc_Absyn_Aggregate_e_struct _tmpF0D;struct Cyc_Absyn_Aggregate_e_struct*
_tmpF0C;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpF0C=_cycalloc(sizeof(*
_tmpF0C)),((_tmpF0C[0]=((_tmpF0D.tag=29,((_tmpF0D.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpF0D.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpF0D.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmpF0D.f4=0,
_tmpF0D)))))))))),_tmpF0C)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 494:
_LL443: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 495:
_LL444: yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));break;
case 496: _LL445:{struct Cyc_List_List*_tmpF0E;yyval=Cyc_YY5(((_tmpF0E=_cycalloc(
sizeof(*_tmpF0E)),((_tmpF0E->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpF0E->tl=0,_tmpF0E)))))));}
break;case 497: _LL446:{struct Cyc_List_List*_tmpF0F;yyval=Cyc_YY5(((_tmpF0F=
_cycalloc(sizeof(*_tmpF0F)),((_tmpF0F->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpF0F->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpF0F)))))));}
break;case 498: _LL447: yyval=Cyc_YY3(Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 499:
_LL448: yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 500:
_LL449: yyval=Cyc_YY3(Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 501:
_LL44A: yyval=Cyc_YY3(Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 502:
_LL44B:{struct _dyneither_ptr*_tmpF12;struct _tuple1*_tmpF11;yyval=Cyc_QualId_tok(((
_tmpF11=_cycalloc(sizeof(*_tmpF11)),((_tmpF11->f1=Cyc_Absyn_Rel_n(0),((_tmpF11->f2=((
_tmpF12=_cycalloc(sizeof(*_tmpF12)),((_tmpF12[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF12)))),_tmpF11)))))));}
break;case 503: _LL44C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 504: _LL44D:{struct _dyneither_ptr*_tmpF15;struct _tuple1*
_tmpF14;yyval=Cyc_QualId_tok(((_tmpF14=_cycalloc(sizeof(*_tmpF14)),((_tmpF14->f1=
Cyc_Absyn_Rel_n(0),((_tmpF14->f2=((_tmpF15=_cycalloc(sizeof(*_tmpF15)),((_tmpF15[
0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpF15)))),_tmpF14)))))));}break;case 505: _LL44E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 506: _LL44F: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 507: _LL450:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 508:
_LL451: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
509: _LL452: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 510: _LL453: break;case 511: _LL454: yylex_buf->lex_curr_pos -=1;break;default:
_LL455: break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(512,yyn)];yystate=Cyc_yypgoto[
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
_check_known_subscript_notnull(287,x)])+ 15,count ++);}{unsigned int _tmpF1A;
unsigned int _tmpF19;struct _dyneither_ptr _tmpF18;char*_tmpF17;unsigned int _tmpF16;
msg=((_tmpF16=(unsigned int)(sze + 15),((_tmpF17=(char*)_region_malloc(yyregion,
_check_times(sizeof(char),_tmpF16 + 1)),((_tmpF18=_tag_dyneither(_tmpF17,sizeof(
char),_tmpF16 + 1),((((_tmpF19=_tmpF16,((_tmpF1B(& _tmpF1A,& _tmpF19,& _tmpF17),
_tmpF17[_tmpF19]=(char)0)))),_tmpF18))))))));}{const char*_tmpF1C;Cyc_strcpy(msg,((
_tmpF1C="parse error",_tag_dyneither(_tmpF1C,sizeof(char),12))));}if(count < 5){
count=0;for(x=yyn < 0?- yyn: 0;x < 287 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6245,x + yyn)]== x){{const char*_tmpF1E;const char*
_tmpF1D;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)((
_tmpF1E=", expecting `",_tag_dyneither(_tmpF1E,sizeof(char),14))):(struct
_dyneither_ptr)((_tmpF1D=" or `",_tag_dyneither(_tmpF1D,sizeof(char),6)))));}Cyc_strcat(
msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(287,x)]);{
const char*_tmpF1F;Cyc_strcat(msg,((_tmpF1F="'",_tag_dyneither(_tmpF1F,sizeof(
char),2))));}++ count;}}}Cyc_yyerror((struct _dyneither_ptr)msg);}else{const char*
_tmpF20;Cyc_yyerror(((_tmpF20="parse error",_tag_dyneither(_tmpF20,sizeof(char),
12))));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int
_tmp837=1;_npop_handler(0);return _tmp837;}Cyc_yychar=- 2;}yyerrstatus=3;goto
yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp838=1;
_npop_handler(0);return _tmp838;}-- yyvsp_offset;yystate=(int)yyss[
_check_known_subscript_notnull(10000,-- yyssp_offset)];-- yylsp_offset;yyerrhandle:
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1028,yystate)];if(yyn == -
32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6244) || Cyc_yycheck[
_check_known_subscript_notnull(6245,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6245,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 1027){int
_tmp839=0;_npop_handler(0);return _tmp839;}yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE _tmp843=v;struct _tuple6 _tmp844;int
_tmp845;char _tmp846;struct _dyneither_ptr _tmp847;struct _tuple1*_tmp848;struct
_tuple1 _tmp849;union Cyc_Absyn_Nmspace _tmp84A;struct _dyneither_ptr*_tmp84B;_LL458:
if((_tmp843.Int_tok).tag != 1)goto _LL45A;_tmp844=(struct _tuple6)(_tmp843.Int_tok).val;
_tmp845=_tmp844.f2;_LL459:{const char*_tmpF24;void*_tmpF23[1];struct Cyc_Int_pa_struct
_tmpF22;(_tmpF22.tag=1,((_tmpF22.f1=(unsigned long)_tmp845,((_tmpF23[0]=& _tmpF22,
Cyc_fprintf(Cyc_stderr,((_tmpF24="%d",_tag_dyneither(_tmpF24,sizeof(char),3))),
_tag_dyneither(_tmpF23,sizeof(void*),1)))))));}goto _LL457;_LL45A: if((_tmp843.Char_tok).tag
!= 2)goto _LL45C;_tmp846=(char)(_tmp843.Char_tok).val;_LL45B:{const char*_tmpF28;
void*_tmpF27[1];struct Cyc_Int_pa_struct _tmpF26;(_tmpF26.tag=1,((_tmpF26.f1=(
unsigned long)((int)_tmp846),((_tmpF27[0]=& _tmpF26,Cyc_fprintf(Cyc_stderr,((
_tmpF28="%c",_tag_dyneither(_tmpF28,sizeof(char),3))),_tag_dyneither(_tmpF27,
sizeof(void*),1)))))));}goto _LL457;_LL45C: if((_tmp843.String_tok).tag != 3)goto
_LL45E;_tmp847=(struct _dyneither_ptr)(_tmp843.String_tok).val;_LL45D:{const char*
_tmpF2C;void*_tmpF2B[1];struct Cyc_String_pa_struct _tmpF2A;(_tmpF2A.tag=0,((
_tmpF2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp847),((_tmpF2B[0]=&
_tmpF2A,Cyc_fprintf(Cyc_stderr,((_tmpF2C="\"%s\"",_tag_dyneither(_tmpF2C,sizeof(
char),5))),_tag_dyneither(_tmpF2B,sizeof(void*),1)))))));}goto _LL457;_LL45E: if((
_tmp843.QualId_tok).tag != 5)goto _LL460;_tmp848=(struct _tuple1*)(_tmp843.QualId_tok).val;
_tmp849=*_tmp848;_tmp84A=_tmp849.f1;_tmp84B=_tmp849.f2;_LL45F: {struct Cyc_List_List*
_tmp855=0;{union Cyc_Absyn_Nmspace _tmp856=_tmp84A;struct Cyc_List_List*_tmp857;
struct Cyc_List_List*_tmp858;struct Cyc_List_List*_tmp859;int _tmp85A;_LL463: if((
_tmp856.Rel_n).tag != 1)goto _LL465;_tmp857=(struct Cyc_List_List*)(_tmp856.Rel_n).val;
_LL464: _tmp855=_tmp857;goto _LL462;_LL465: if((_tmp856.Abs_n).tag != 2)goto _LL467;
_tmp858=(struct Cyc_List_List*)(_tmp856.Abs_n).val;_LL466: _tmp855=_tmp858;goto
_LL462;_LL467: if((_tmp856.C_n).tag != 3)goto _LL469;_tmp859=(struct Cyc_List_List*)(
_tmp856.C_n).val;_LL468: _tmp855=_tmp859;goto _LL462;_LL469: if((_tmp856.Loc_n).tag
!= 4)goto _LL462;_tmp85A=(int)(_tmp856.Loc_n).val;_LL46A: goto _LL462;_LL462:;}for(
0;_tmp855 != 0;_tmp855=_tmp855->tl){const char*_tmpF30;void*_tmpF2F[1];struct Cyc_String_pa_struct
_tmpF2E;(_tmpF2E.tag=0,((_tmpF2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp855->hd)),((_tmpF2F[0]=& _tmpF2E,Cyc_fprintf(Cyc_stderr,((
_tmpF30="%s::",_tag_dyneither(_tmpF30,sizeof(char),5))),_tag_dyneither(_tmpF2F,
sizeof(void*),1)))))));}{const char*_tmpF34;void*_tmpF33[1];struct Cyc_String_pa_struct
_tmpF32;(_tmpF32.tag=0,((_tmpF32.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp84B),((_tmpF33[0]=& _tmpF32,Cyc_fprintf(Cyc_stderr,((_tmpF34="%s::",
_tag_dyneither(_tmpF34,sizeof(char),5))),_tag_dyneither(_tmpF33,sizeof(void*),1)))))));}
goto _LL457;}_LL460:;_LL461:{const char*_tmpF37;void*_tmpF36;(_tmpF36=0,Cyc_fprintf(
Cyc_stderr,((_tmpF37="?",_tag_dyneither(_tmpF37,sizeof(char),2))),_tag_dyneither(
_tmpF36,sizeof(void*),0)));}goto _LL457;_LL457:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
