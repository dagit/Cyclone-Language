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
_tmp866;struct Cyc_Position_Exit_struct*_tmp865;(int)_throw((void*)((_tmp865=
_cycalloc_atomic(sizeof(*_tmp865)),((_tmp865[0]=((_tmp866.tag=Cyc_Position_Exit,
_tmp866)),_tmp865)))));};}static void Cyc_Parse_unimp(struct _dyneither_ptr msg,
struct Cyc_Position_Segment*sg){{const char*_tmp86B;void*_tmp86A[2];struct Cyc_String_pa_struct
_tmp869;struct Cyc_String_pa_struct _tmp868;(_tmp868.tag=0,((_tmp868.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp869.tag=0,((_tmp869.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(sg)),((
_tmp86A[0]=& _tmp869,((_tmp86A[1]=& _tmp868,Cyc_fprintf(Cyc_stderr,((_tmp86B="%s: Warning: Cyclone does not yet support %s\n",
_tag_dyneither(_tmp86B,sizeof(char),46))),_tag_dyneither(_tmp86A,sizeof(void*),2)))))))))))));}
return;}static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,struct Cyc_Position_Segment*
loc){struct _tuple8 _tmp7;struct Cyc_Absyn_Tqual _tmp8;void*_tmp9;struct _tuple8*
_tmp6=tqt;_tmp7=*_tmp6;_tmp8=_tmp7.f2;_tmp9=_tmp7.f3;if((_tmp8.print_const  || 
_tmp8.q_volatile) || _tmp8.q_restrict){if(_tmp8.loc != 0)loc=_tmp8.loc;{const char*
_tmp86E;void*_tmp86D;(_tmp86D=0,Cyc_Tcutil_warn(loc,((_tmp86E="qualifier on type is ignored",
_tag_dyneither(_tmp86E,sizeof(char),29))),_tag_dyneither(_tmp86D,sizeof(void*),0)));};}
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
_tmp871;struct Cyc_Absyn_Upper_b_struct*_tmp870;bound=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp870=_cycalloc(sizeof(*_tmp870)),((
_tmp870[0]=((_tmp871.tag=1,((_tmp871.f1=_tmp14,_tmp871)))),_tmp870)))));}goto
_LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_struct*_tmp15=(struct Cyc_Absyn_Region_ptrqual_struct*)
_tmpC;if(_tmp15->tag != 1)goto _LL0;else{_tmp16=(void*)_tmp15->f1;}}_LL10: rgn=
_tmp16;goto _LL0;_LL0:;}{struct _tuple11 _tmp872;return(_tmp872.f1=nullable,((
_tmp872.f2=bound,((_tmp872.f3=zeroterm,((_tmp872.f4=rgn,_tmp872)))))));};}struct
_tuple1*Cyc_Parse_gensym_enum(){static int enum_counter=0;struct _dyneither_ptr*
_tmp87F;const char*_tmp87E;void*_tmp87D[1];struct Cyc_Int_pa_struct _tmp87C;struct
_tuple1*_tmp87B;return(_tmp87B=_cycalloc(sizeof(*_tmp87B)),((_tmp87B->f1=Cyc_Absyn_Rel_n(
0),((_tmp87B->f2=((_tmp87F=_cycalloc(sizeof(*_tmp87F)),((_tmp87F[0]=(struct
_dyneither_ptr)((_tmp87C.tag=1,((_tmp87C.f1=(unsigned long)enum_counter ++,((
_tmp87D[0]=& _tmp87C,Cyc_aprintf(((_tmp87E="__anonymous_enum_%d__",_tag_dyneither(
_tmp87E,sizeof(char),22))),_tag_dyneither(_tmp87D,sizeof(void*),1)))))))),
_tmp87F)))),_tmp87B)))));}struct _tuple12{struct _tuple1*f1;struct Cyc_Absyn_Tqual
f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple13{struct
_tuple12*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple13*field_info){struct _tuple13 _tmp20;
struct _tuple12*_tmp21;struct _tuple12 _tmp22;struct _tuple1*_tmp23;struct Cyc_Absyn_Tqual
_tmp24;void*_tmp25;struct Cyc_List_List*_tmp26;struct Cyc_List_List*_tmp27;struct
Cyc_Absyn_Exp*_tmp28;struct _tuple13*_tmp1F=field_info;_tmp20=*_tmp1F;_tmp21=
_tmp20.f1;_tmp22=*_tmp21;_tmp23=_tmp22.f1;_tmp24=_tmp22.f2;_tmp25=_tmp22.f3;
_tmp26=_tmp22.f4;_tmp27=_tmp22.f5;_tmp28=_tmp20.f2;if(_tmp26 != 0){const char*
_tmp880;Cyc_Parse_err(((_tmp880="bad type params in struct field",_tag_dyneither(
_tmp880,sizeof(char),32))),loc);}if(Cyc_Absyn_is_qvar_qualified(_tmp23)){const
char*_tmp881;Cyc_Parse_err(((_tmp881="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp881,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp882;return(_tmp882=_cycalloc(sizeof(*_tmp882)),((_tmp882->name=(*_tmp23).f2,((
_tmp882->tq=_tmp24,((_tmp882->type=_tmp25,((_tmp882->width=_tmp28,((_tmp882->attributes=
_tmp27,_tmp882)))))))))));};}static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*
loc){struct Cyc_Parse_Type_spec_struct _tmp885;struct Cyc_Parse_Type_spec_struct*
_tmp884;return(void*)((_tmp884=_cycalloc(sizeof(*_tmp884)),((_tmp884[0]=((
_tmp885.tag=4,((_tmp885.f1=(void*)t,((_tmp885.f2=loc,_tmp885)))))),_tmp884))));}
static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp2E=t;struct Cyc_Absyn_ArrayInfo
_tmp30;void*_tmp31;struct Cyc_Absyn_Tqual _tmp32;struct Cyc_Absyn_Exp*_tmp33;union
Cyc_Absyn_Constraint*_tmp34;struct Cyc_Position_Segment*_tmp35;_LL12: {struct Cyc_Absyn_ArrayType_struct*
_tmp2F=(struct Cyc_Absyn_ArrayType_struct*)_tmp2E;if(_tmp2F->tag != 9)goto _LL14;
else{_tmp30=_tmp2F->f1;_tmp31=_tmp30.elt_type;_tmp32=_tmp30.tq;_tmp33=_tmp30.num_elts;
_tmp34=_tmp30.zero_term;_tmp35=_tmp30.zt_loc;}}_LL13: {struct Cyc_Absyn_Upper_b_struct
_tmp888;struct Cyc_Absyn_Upper_b_struct*_tmp887;return Cyc_Absyn_starb_typ(_tmp31,
argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,
_tmp32,_tmp33 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((void*)((_tmp887=
_cycalloc(sizeof(*_tmp887)),((_tmp887[0]=((_tmp888.tag=1,((_tmp888.f1=(struct Cyc_Absyn_Exp*)
_tmp33,_tmp888)))),_tmp887))))),_tmp34);}_LL14:;_LL15: return t;_LL11:;}struct
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
_tmp4C->f2;}}_LL1F: {struct Cyc_String_pa_struct _tmp890;void*_tmp88F[1];const char*
_tmp88E;struct _dyneither_ptr*_tmp88D;struct _dyneither_ptr*nm=(_tmp88D=_cycalloc(
sizeof(*_tmp88D)),((_tmp88D[0]=(struct _dyneither_ptr)((_tmp890.tag=0,((_tmp890.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3D),((_tmp88F[0]=& _tmp890,Cyc_aprintf(((
_tmp88E="`%s",_tag_dyneither(_tmp88E,sizeof(char),4))),_tag_dyneither(_tmp88F,
sizeof(void*),1)))))))),_tmp88D)));{struct Cyc_Absyn_VarType_struct*_tmp8A5;
struct Cyc_Absyn_Tvar*_tmp8A4;struct Cyc_Absyn_Eq_kb_struct _tmp8A3;struct Cyc_Absyn_Eq_kb_struct*
_tmp8A2;struct Cyc_Absyn_VarType_struct _tmp8A1;struct Cyc_Core_Opt*_tmp8A0;*_tmp4E=((
_tmp8A0=_cycalloc(sizeof(*_tmp8A0)),((_tmp8A0->v=(void*)((_tmp8A5=_cycalloc(
sizeof(*_tmp8A5)),((_tmp8A5[0]=((_tmp8A1.tag=2,((_tmp8A1.f1=((_tmp8A4=_cycalloc(
sizeof(*_tmp8A4)),((_tmp8A4->name=nm,((_tmp8A4->identity=- 1,((_tmp8A4->kind=(
void*)((_tmp8A2=_cycalloc(sizeof(*_tmp8A2)),((_tmp8A2[0]=((_tmp8A3.tag=0,((
_tmp8A3.f1=& Cyc_Tcutil_ik,_tmp8A3)))),_tmp8A2)))),_tmp8A4)))))))),_tmp8A1)))),
_tmp8A5)))),_tmp8A0))));}goto _LL1D;}_LL20:;_LL21: goto _LL1D;_LL1D:;}{struct
_tuple14*_tmp8A8;struct Cyc_List_List*_tmp8A7;_tmp38=((_tmp8A7=_cycalloc(sizeof(*
_tmp8A7)),((_tmp8A7->hd=((_tmp8A8=_cycalloc(sizeof(*_tmp8A8)),((_tmp8A8->f1=
_tmp3D,((_tmp8A8->f2=_tmp40,_tmp8A8)))))),((_tmp8A7->tl=_tmp38,_tmp8A7))))));}
goto _LL16;_LL19: _tmp41=*_tmp39;_tmp42=_tmp41.f1;if(_tmp42 == 0)goto _LL1B;_tmp43=*
_tmp42;_tmp44=(struct _dyneither_ptr*)_tmp43.v;_tmp45=_tmp41.f3;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp46=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp45;if(_tmp46->tag != 16)goto
_LL1B;else{_tmp47=(void*)_tmp46->f1;{struct Cyc_Absyn_Evar_struct*_tmp48=(struct
Cyc_Absyn_Evar_struct*)_tmp47;if(_tmp48->tag != 1)goto _LL1B;else{_tmp49=_tmp48->f2;
_tmp4A=(struct Cyc_Core_Opt**)& _tmp48->f2;}};}};_LL1A: {struct Cyc_String_pa_struct
_tmp8B0;void*_tmp8AF[1];const char*_tmp8AE;struct _dyneither_ptr*_tmp8AD;struct
_dyneither_ptr*nm=(_tmp8AD=_cycalloc(sizeof(*_tmp8AD)),((_tmp8AD[0]=(struct
_dyneither_ptr)((_tmp8B0.tag=0,((_tmp8B0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp44),((_tmp8AF[0]=& _tmp8B0,Cyc_aprintf(((_tmp8AE="`%s",
_tag_dyneither(_tmp8AE,sizeof(char),4))),_tag_dyneither(_tmp8AF,sizeof(void*),1)))))))),
_tmp8AD)));{struct Cyc_Absyn_VarType_struct*_tmp8C5;struct Cyc_Absyn_Tvar*_tmp8C4;
struct Cyc_Absyn_Eq_kb_struct _tmp8C3;struct Cyc_Absyn_Eq_kb_struct*_tmp8C2;struct
Cyc_Absyn_VarType_struct _tmp8C1;struct Cyc_Core_Opt*_tmp8C0;*_tmp4A=((_tmp8C0=
_cycalloc(sizeof(*_tmp8C0)),((_tmp8C0->v=(void*)((_tmp8C5=_cycalloc(sizeof(*
_tmp8C5)),((_tmp8C5[0]=((_tmp8C1.tag=2,((_tmp8C1.f1=((_tmp8C4=_cycalloc(sizeof(*
_tmp8C4)),((_tmp8C4->name=nm,((_tmp8C4->identity=- 1,((_tmp8C4->kind=(void*)((
_tmp8C2=_cycalloc(sizeof(*_tmp8C2)),((_tmp8C2[0]=((_tmp8C3.tag=0,((_tmp8C3.f1=&
Cyc_Tcutil_rk,_tmp8C3)))),_tmp8C2)))),_tmp8C4)))))))),_tmp8C1)))),_tmp8C5)))),
_tmp8C0))));}goto _LL16;}_LL1B:;_LL1C: goto _LL16;_LL16:;}return _tmp38;}static
struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){struct
Cyc_List_List*_tmp65=0;for(0;x != 0;x=x->tl){void*_tmp66=((struct Cyc_Absyn_Aggrfield*)
x->hd)->type;void*_tmp68;_LL23: {struct Cyc_Absyn_TagType_struct*_tmp67=(struct
Cyc_Absyn_TagType_struct*)_tmp66;if(_tmp67->tag != 20)goto _LL25;else{_tmp68=(void*)
_tmp67->f1;}}_LL24:{struct _tuple14*_tmp8C8;struct Cyc_List_List*_tmp8C7;_tmp65=((
_tmp8C7=_cycalloc(sizeof(*_tmp8C7)),((_tmp8C7->hd=((_tmp8C8=_cycalloc(sizeof(*
_tmp8C8)),((_tmp8C8->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8C8->f2=
_tmp68,_tmp8C8)))))),((_tmp8C7->tl=_tmp65,_tmp8C7))))));}goto _LL22;_LL25:;_LL26:
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
== 0){struct Cyc_Absyn_Valueof_e_struct _tmp8CB;struct Cyc_Absyn_Valueof_e_struct*
_tmp8CA;return Cyc_Absyn_new_exp((void*)((_tmp8CA=_cycalloc(sizeof(*_tmp8CA)),((
_tmp8CA[0]=((_tmp8CB.tag=39,((_tmp8CB.f1=(void*)Cyc_Tcutil_copy_type(_tmp7C),
_tmp8CB)))),_tmp8CA)))),e->loc);}}}goto _LL27;_LL2C:;_LL2D: goto _LL27;_LL27:;}
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
struct Cyc_Absyn_ArrayType_struct _tmp8D1;struct Cyc_Absyn_ArrayInfo _tmp8D0;struct
Cyc_Absyn_ArrayType_struct*_tmp8CF;return(void*)((_tmp8CF=_cycalloc(sizeof(*
_tmp8CF)),((_tmp8CF[0]=((_tmp8D1.tag=9,((_tmp8D1.f1=((_tmp8D0.elt_type=_tmp91,((
_tmp8D0.tq=_tmp83,((_tmp8D0.num_elts=nelts2,((_tmp8D0.zero_term=_tmp85,((_tmp8D0.zt_loc=
_tmp86,_tmp8D0)))))))))),_tmp8D1)))),_tmp8CF))));}goto _LL2E;};}_LL31: {struct Cyc_Absyn_PointerType_struct*
_tmp87=(struct Cyc_Absyn_PointerType_struct*)_tmp7F;if(_tmp87->tag != 5)goto _LL33;
else{_tmp88=_tmp87->f1;_tmp89=_tmp88.elt_typ;_tmp8A=_tmp88.elt_tq;_tmp8B=_tmp88.ptr_atts;
_tmp8C=_tmp8B.rgn;_tmp8D=_tmp8B.nullable;_tmp8E=_tmp8B.bounds;_tmp8F=_tmp8B.zero_term;
_tmp90=_tmp8B.ptrloc;}}_LL32: {void*_tmp95=Cyc_Parse_substitute_tags(tags,_tmp89);
union Cyc_Absyn_Constraint*_tmp96=_tmp8E;{union Cyc_Absyn_Constraint _tmp97=*_tmp8E;
void*_tmp98;struct Cyc_Absyn_Exp*_tmp9A;_LL36: if((_tmp97.Eq_constr).tag != 1)goto
_LL38;_tmp98=(void*)(_tmp97.Eq_constr).val;{struct Cyc_Absyn_Upper_b_struct*
_tmp99=(struct Cyc_Absyn_Upper_b_struct*)_tmp98;if(_tmp99->tag != 1)goto _LL38;
else{_tmp9A=_tmp99->f1;}};_LL37: _tmp9A=Cyc_Parse_substitute_tags_exp(tags,_tmp9A);{
struct Cyc_Absyn_Upper_b_struct*_tmp8D7;struct Cyc_Absyn_Upper_b_struct _tmp8D6;
union Cyc_Absyn_Constraint*_tmp8D5;_tmp96=((_tmp8D5=_cycalloc(sizeof(*_tmp8D5)),(((
_tmp8D5->Eq_constr).val=(void*)((_tmp8D7=_cycalloc(sizeof(*_tmp8D7)),((_tmp8D7[0]=((
_tmp8D6.tag=1,((_tmp8D6.f1=_tmp9A,_tmp8D6)))),_tmp8D7)))),(((_tmp8D5->Eq_constr).tag=
1,_tmp8D5))))));}goto _LL35;_LL38:;_LL39: goto _LL35;_LL35:;}if(_tmp95 != _tmp89  || 
_tmp96 != _tmp8E){struct Cyc_Absyn_PointerType_struct _tmp8E1;struct Cyc_Absyn_PtrAtts
_tmp8E0;struct Cyc_Absyn_PtrInfo _tmp8DF;struct Cyc_Absyn_PointerType_struct*
_tmp8DE;return(void*)((_tmp8DE=_cycalloc(sizeof(*_tmp8DE)),((_tmp8DE[0]=((
_tmp8E1.tag=5,((_tmp8E1.f1=((_tmp8DF.elt_typ=_tmp95,((_tmp8DF.elt_tq=_tmp8A,((
_tmp8DF.ptr_atts=((_tmp8E0.rgn=_tmp8C,((_tmp8E0.nullable=_tmp8D,((_tmp8E0.bounds=
_tmp96,((_tmp8E0.zero_term=_tmp8F,((_tmp8E0.ptrloc=_tmp90,_tmp8E0)))))))))),
_tmp8DF)))))),_tmp8E1)))),_tmp8DE))));}goto _LL2E;}_LL33:;_LL34: goto _LL2E;_LL2E:;}
return t;}static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,
struct Cyc_Absyn_Aggrfield*x){x->type=Cyc_Parse_substitute_tags(tags,x->type);}
struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple15*Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment*loc,struct _tuple8*t){struct _tuple15*_tmp8E2;return(
_tmp8E2=_cycalloc(sizeof(*_tmp8E2)),((_tmp8E2->f1=(*t).f2,((_tmp8E2->f2=(*t).f3,
_tmp8E2)))));}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*
d){void*_tmpA3=d->r;struct Cyc_Absyn_Vardecl*_tmpA5;_LL3B: {struct Cyc_Absyn_Var_d_struct*
_tmpA4=(struct Cyc_Absyn_Var_d_struct*)_tmpA3;if(_tmpA4->tag != 0)goto _LL3D;else{
_tmpA5=_tmpA4->f1;}}_LL3C: return _tmpA5;_LL3D:;_LL3E: {const char*_tmp8E5;void*
_tmp8E4;(_tmp8E4=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((_tmp8E5="bad declaration in `forarray' statement",
_tag_dyneither(_tmp8E5,sizeof(char),40))),_tag_dyneither(_tmp8E4,sizeof(void*),0)));}
_LL3A:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmpA8=tm;_LL40: {struct
Cyc_Absyn_TypeParams_mod_struct*_tmpA9=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpA8;if(_tmpA9->tag != 4)goto _LL42;}_LL41: return 1;_LL42:;_LL43: return 0;_LL3F:;}
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){const char*_tmp8E6;if(
Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8E6="`H",_tag_dyneither(_tmp8E6,
sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_HeapRgn_val;else{const char*
_tmp8E7;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8E7="`U",_tag_dyneither(
_tmp8E7,sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_UniqueRgn_val;else{const
char*_tmp8E8;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8E8="`RC",
_tag_dyneither(_tmp8E8,sizeof(char),4))))== 0)return(void*)& Cyc_Absyn_RefCntRgn_val;
else{struct Cyc_Absyn_VarType_struct _tmp8F2;struct _dyneither_ptr*_tmp8F1;struct
Cyc_Absyn_Tvar*_tmp8F0;struct Cyc_Absyn_VarType_struct*_tmp8EF;return(void*)((
_tmp8EF=_cycalloc(sizeof(*_tmp8EF)),((_tmp8EF[0]=((_tmp8F2.tag=2,((_tmp8F2.f1=((
_tmp8F0=_cycalloc(sizeof(*_tmp8F0)),((_tmp8F0->name=((_tmp8F1=_cycalloc(sizeof(*
_tmp8F1)),((_tmp8F1[0]=s,_tmp8F1)))),((_tmp8F0->identity=- 1,((_tmp8F0->kind=k,
_tmp8F0)))))))),_tmp8F2)))),_tmp8EF))));}}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar*t){void*k;{void*_tmpB1=Cyc_Absyn_compress_kb(t->kind);
struct Cyc_Absyn_Kind*_tmpB4;_LL45: {struct Cyc_Absyn_Unknown_kb_struct*_tmpB2=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpB1;if(_tmpB2->tag != 1)goto _LL47;}_LL46:{
struct Cyc_Absyn_Unknown_kb_struct _tmp8F5;struct Cyc_Absyn_Unknown_kb_struct*
_tmp8F4;k=(void*)((_tmp8F4=_cycalloc(sizeof(*_tmp8F4)),((_tmp8F4[0]=((_tmp8F5.tag=
1,((_tmp8F5.f1=0,_tmp8F5)))),_tmp8F4))));}goto _LL44;_LL47: {struct Cyc_Absyn_Less_kb_struct*
_tmpB3=(struct Cyc_Absyn_Less_kb_struct*)_tmpB1;if(_tmpB3->tag != 2)goto _LL49;
else{_tmpB4=_tmpB3->f2;}}_LL48:{struct Cyc_Absyn_Less_kb_struct _tmp8F8;struct Cyc_Absyn_Less_kb_struct*
_tmp8F7;k=(void*)((_tmp8F7=_cycalloc(sizeof(*_tmp8F7)),((_tmp8F7[0]=((_tmp8F8.tag=
2,((_tmp8F8.f1=0,((_tmp8F8.f2=_tmpB4,_tmp8F8)))))),_tmp8F7))));}goto _LL44;_LL49:;
_LL4A: k=_tmpB1;goto _LL44;_LL44:;}{struct Cyc_Absyn_Tvar*_tmp8F9;return(_tmp8F9=
_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9->name=t->name,((_tmp8F9->identity=- 1,((
_tmp8F9->kind=k,_tmp8F9)))))));};}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(
struct Cyc_Position_Segment*loc,void*t){void*_tmpBA=t;struct Cyc_Absyn_Tvar*_tmpBC;
_LL4C: {struct Cyc_Absyn_VarType_struct*_tmpBB=(struct Cyc_Absyn_VarType_struct*)
_tmpBA;if(_tmpBB->tag != 2)goto _LL4E;else{_tmpBC=_tmpBB->f1;}}_LL4D: return _tmpBC;
_LL4E:;_LL4F: {const char*_tmp8FC;void*_tmp8FB;(_tmp8FB=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp8FC="expecting a list of type variables, not types",_tag_dyneither(_tmp8FC,
sizeof(char),46))),_tag_dyneither(_tmp8FB,sizeof(void*),0)));}_LL4B:;}static void*
Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct
_tmp8FF;struct Cyc_Absyn_VarType_struct*_tmp8FE;return(void*)((_tmp8FE=_cycalloc(
sizeof(*_tmp8FE)),((_tmp8FE[0]=((_tmp8FF.tag=2,((_tmp8FF.f1=pr,_tmp8FF)))),
_tmp8FE))));}static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,
int leq){void*_tmpC1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpC3;struct Cyc_Absyn_Tvar
_tmpC4;void*_tmpC5;void**_tmpC6;_LL51: {struct Cyc_Absyn_VarType_struct*_tmpC2=(
struct Cyc_Absyn_VarType_struct*)_tmpC1;if(_tmpC2->tag != 2)goto _LL53;else{_tmpC3=
_tmpC2->f1;_tmpC4=*_tmpC3;_tmpC5=_tmpC4.kind;_tmpC6=(void**)&(*_tmpC2->f1).kind;}}
_LL52: {void*_tmpC7=Cyc_Absyn_compress_kb(*_tmpC6);_LL56: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpC8=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC7;if(_tmpC8->tag != 1)goto _LL58;}
_LL57: if(!leq)*_tmpC6=Cyc_Tcutil_kind_to_bound(k);else{struct Cyc_Absyn_Less_kb_struct
_tmp902;struct Cyc_Absyn_Less_kb_struct*_tmp901;*_tmpC6=(void*)((_tmp901=
_cycalloc(sizeof(*_tmp901)),((_tmp901[0]=((_tmp902.tag=2,((_tmp902.f1=0,((
_tmp902.f2=k,_tmp902)))))),_tmp901))));}return;_LL58:;_LL59: return;_LL55:;}_LL53:;
_LL54: return;_LL50:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmpCB=(void*)tms->hd;void*_tmpCD;
_LL5B: {struct Cyc_Absyn_Function_mod_struct*_tmpCC=(struct Cyc_Absyn_Function_mod_struct*)
_tmpCB;if(_tmpCC->tag != 3)goto _LL5D;else{_tmpCD=(void*)_tmpCC->f1;}}_LL5C: if(tms->tl
== 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)
 && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){void*_tmpCE=_tmpCD;
struct Cyc_List_List*_tmpD1;_LL60: {struct Cyc_Absyn_WithTypes_struct*_tmpCF=(
struct Cyc_Absyn_WithTypes_struct*)_tmpCE;if(_tmpCF->tag != 1)goto _LL62;}_LL61:{
const char*_tmp905;void*_tmp904;(_tmp904=0,Cyc_Tcutil_warn(loc,((_tmp905="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp905,sizeof(char),93))),_tag_dyneither(_tmp904,sizeof(void*),0)));}
return tms;_LL62: {struct Cyc_Absyn_NoTypes_struct*_tmpD0=(struct Cyc_Absyn_NoTypes_struct*)
_tmpCE;if(_tmpD0->tag != 0)goto _LL5F;else{_tmpD1=_tmpD0->f1;}}_LL63: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpD1)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds)){const char*_tmp908;void*_tmp907;(_tmp907=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
loc,((_tmp908="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp908,sizeof(char),73))),_tag_dyneither(_tmp907,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpD6=0;for(0;_tmpD1 != 0;_tmpD1=_tmpD1->tl){struct Cyc_List_List*
_tmpD7=tds;for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){struct Cyc_Absyn_Decl*_tmpD8=(
struct Cyc_Absyn_Decl*)_tmpD7->hd;void*_tmpD9=_tmpD8->r;struct Cyc_Absyn_Vardecl*
_tmpDB;_LL65: {struct Cyc_Absyn_Var_d_struct*_tmpDA=(struct Cyc_Absyn_Var_d_struct*)
_tmpD9;if(_tmpDA->tag != 0)goto _LL67;else{_tmpDB=_tmpDA->f1;}}_LL66: if(Cyc_zstrptrcmp((*
_tmpDB->name).f2,(struct _dyneither_ptr*)_tmpD1->hd)!= 0)continue;if(_tmpDB->initializer
!= 0){const char*_tmp90B;void*_tmp90A;(_tmp90A=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD8->loc,((
_tmp90B="initializer found in parameter declaration",_tag_dyneither(_tmp90B,
sizeof(char),43))),_tag_dyneither(_tmp90A,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmpDB->name)){const char*_tmp90E;void*_tmp90D;(_tmp90D=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD8->loc,((
_tmp90E="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp90E,
sizeof(char),47))),_tag_dyneither(_tmp90D,sizeof(void*),0)));}{struct _tuple8*
_tmp914;struct Cyc_Core_Opt*_tmp913;struct Cyc_List_List*_tmp912;_tmpD6=((_tmp912=
_cycalloc(sizeof(*_tmp912)),((_tmp912->hd=((_tmp914=_cycalloc(sizeof(*_tmp914)),((
_tmp914->f1=((_tmp913=_cycalloc(sizeof(*_tmp913)),((_tmp913->v=(*_tmpDB->name).f2,
_tmp913)))),((_tmp914->f2=_tmpDB->tq,((_tmp914->f3=_tmpDB->type,_tmp914)))))))),((
_tmp912->tl=_tmpD6,_tmp912))))));}goto L;_LL67:;_LL68: {const char*_tmp917;void*
_tmp916;(_tmp916=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD8->loc,((_tmp917="nonvariable declaration in parameter type",
_tag_dyneither(_tmp917,sizeof(char),42))),_tag_dyneither(_tmp916,sizeof(void*),0)));}
_LL64:;}L: if(_tmpD7 == 0){const char*_tmp91B;void*_tmp91A[1];struct Cyc_String_pa_struct
_tmp919;(_tmp919.tag=0,((_tmp919.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmpD1->hd)),((_tmp91A[0]=& _tmp919,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp91B="%s is not given a type",_tag_dyneither(_tmp91B,sizeof(char),23))),
_tag_dyneither(_tmp91A,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp92A;struct Cyc_Absyn_WithTypes_struct*_tmp929;struct Cyc_Absyn_WithTypes_struct
_tmp928;struct Cyc_Absyn_Function_mod_struct _tmp927;struct Cyc_List_List*_tmp926;
return(_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926->hd=(void*)((_tmp92A=
_cycalloc(sizeof(*_tmp92A)),((_tmp92A[0]=((_tmp927.tag=3,((_tmp927.f1=(void*)((
void*)((_tmp929=_cycalloc(sizeof(*_tmp929)),((_tmp929[0]=((_tmp928.tag=1,((
_tmp928.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpD6),((_tmp928.f2=0,((_tmp928.f3=0,((_tmp928.f4=0,((_tmp928.f5=0,_tmp928)))))))))))),
_tmp929))))),_tmp927)))),_tmp92A)))),((_tmp926->tl=0,_tmp926)))));};};_LL5F:;}
goto _LL5E;_LL5D:;_LL5E: {struct Cyc_List_List*_tmp92B;return(_tmp92B=_cycalloc(
sizeof(*_tmp92B)),((_tmp92B->hd=(void*)tms->hd,((_tmp92B->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp92B)))));}_LL5A:;};}struct _tuple16{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){if(tds != 0){struct Cyc_Parse_Declarator*
_tmp92C;d=((_tmp92C=_cycalloc(sizeof(*_tmp92C)),((_tmp92C->id=d->id,((_tmp92C->tms=
Cyc_Parse_oldstyle2newstyle(d->tms,tds,loc),_tmp92C))))));}{enum Cyc_Absyn_Scope
sc=Cyc_Absyn_Public;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(
0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0)
switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v){case Cyc_Parse_Extern_sc: _LL69: sc=
Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL6A: sc=Cyc_Absyn_Static;break;
default: _LL6B:{const char*_tmp92D;Cyc_Parse_err(((_tmp92D="bad storage class on function",
_tag_dyneither(_tmp92D,sizeof(char),30))),loc);}break;}}{void*_tmpF0=Cyc_Parse_collapse_type_specifiers(
tss,loc);struct Cyc_Absyn_Tqual _tmpF2;void*_tmpF3;struct Cyc_List_List*_tmpF4;
struct Cyc_List_List*_tmpF5;struct _tuple10 _tmpF1=Cyc_Parse_apply_tms(tq,_tmpF0,
atts,d->tms);_tmpF2=_tmpF1.f1;_tmpF3=_tmpF1.f2;_tmpF4=_tmpF1.f3;_tmpF5=_tmpF1.f4;
if(_tmpF4 != 0){const char*_tmp930;void*_tmp92F;(_tmp92F=0,Cyc_Tcutil_warn(loc,((
_tmp930="bad type params, ignoring",_tag_dyneither(_tmp930,sizeof(char),26))),
_tag_dyneither(_tmp92F,sizeof(void*),0)));}{void*_tmpF8=_tmpF3;struct Cyc_Absyn_FnInfo
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
_tmp107.f3;if(_tmp108 == 0){{const char*_tmp931;Cyc_Parse_err(((_tmp931="missing argument variable in function prototype",
_tag_dyneither(_tmp931,sizeof(char),48))),loc);}{struct _tuple16*_tmp934;struct
Cyc_List_List*_tmp933;_tmp104=((_tmp933=_cycalloc(sizeof(*_tmp933)),((_tmp933->hd=((
_tmp934=_cycalloc(sizeof(*_tmp934)),((_tmp934->f1=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp934->f2=_tmp109,((
_tmp934->f3=_tmp10A,_tmp934)))))))),((_tmp933->tl=_tmp104,_tmp933))))));};}else{
struct _tuple16*_tmp937;struct Cyc_List_List*_tmp936;_tmp104=((_tmp936=_cycalloc(
sizeof(*_tmp936)),((_tmp936->hd=((_tmp937=_cycalloc(sizeof(*_tmp937)),((_tmp937->f1=(
struct _dyneither_ptr*)_tmp108->v,((_tmp937->f2=_tmp109,((_tmp937->f3=_tmp10A,
_tmp937)))))))),((_tmp936->tl=_tmp104,_tmp936))))));}}}{struct Cyc_Absyn_Fndecl*
_tmp938;return(_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938->sc=sc,((_tmp938->name=
d->id,((_tmp938->tvs=_tmpFB,((_tmp938->is_inline=is_inline,((_tmp938->effect=
_tmpFC,((_tmp938->ret_tqual=_tmpFD,((_tmp938->ret_type=_tmpFE,((_tmp938->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp104),((
_tmp938->c_varargs=_tmp100,((_tmp938->cyc_varargs=_tmp101,((_tmp938->rgn_po=
_tmp102,((_tmp938->body=body,((_tmp938->cached_typ=0,((_tmp938->param_vardecls=0,((
_tmp938->fn_vardecl=0,((_tmp938->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp103,_tmpF5),_tmp938)))))))))))))))))))))))))))))))));};}
_LL70:;_LL71: {const char*_tmp93B;void*_tmp93A;(_tmp93A=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp93B="declarator is not a function prototype",_tag_dyneither(_tmp93B,sizeof(
char),39))),_tag_dyneither(_tmp93A,sizeof(void*),0)));}_LL6D:;};};};}static char
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
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp11D);if(seen_type){const char*_tmp93C;
Cyc_Parse_err(((_tmp93C="signed qualifier must come before type",_tag_dyneither(
_tmp93C,sizeof(char),39))),_tmp11D);}last_loc=_tmp11D;seen_sign=1;sgn=Cyc_Absyn_Signed;
goto _LL72;_LL77: {struct Cyc_Parse_Unsigned_spec_struct*_tmp11E=(struct Cyc_Parse_Unsigned_spec_struct*)
_tmp118;if(_tmp11E->tag != 1)goto _LL79;else{_tmp11F=_tmp11E->f1;}}_LL78: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp11F);if(seen_type){const char*_tmp93D;
Cyc_Parse_err(((_tmp93D="signed qualifier must come before type",_tag_dyneither(
_tmp93D,sizeof(char),39))),_tmp11F);}last_loc=_tmp11F;seen_sign=1;sgn=Cyc_Absyn_Unsigned;
goto _LL72;_LL79: {struct Cyc_Parse_Short_spec_struct*_tmp120=(struct Cyc_Parse_Short_spec_struct*)
_tmp118;if(_tmp120->tag != 2)goto _LL7B;else{_tmp121=_tmp120->f1;}}_LL7A: if(
seen_size){const char*_tmp93E;Cyc_Parse_err(((_tmp93E="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp93E,sizeof(char),59))),_tmp121);}if(seen_type){const char*
_tmp93F;Cyc_Parse_err(((_tmp93F="short modifier must come before base type",
_tag_dyneither(_tmp93F,sizeof(char),42))),_tmp121);}last_loc=_tmp121;seen_size=1;
sz=Cyc_Absyn_Short_sz;goto _LL72;_LL7B: {struct Cyc_Parse_Long_spec_struct*_tmp122=(
struct Cyc_Parse_Long_spec_struct*)_tmp118;if(_tmp122->tag != 3)goto _LL72;else{
_tmp123=_tmp122->f1;}}_LL7C: if(seen_type){const char*_tmp940;Cyc_Parse_err(((
_tmp940="long modifier must come before base type",_tag_dyneither(_tmp940,
sizeof(char),41))),_tmp123);}if(seen_size)switch(sz){case Cyc_Absyn_Long_sz: _LL7D:
sz=Cyc_Absyn_LongLong_sz;break;default: _LL7E:{const char*_tmp941;Cyc_Parse_err(((
_tmp941="extra long in type specifier",_tag_dyneither(_tmp941,sizeof(char),29))),
_tmp123);}break;}else{sz=Cyc_Absyn_Long_sz;}last_loc=_tmp123;seen_size=1;goto
_LL72;_LL72:;}if(!seen_type){if(!seen_sign  && !seen_size){const char*_tmp944;void*
_tmp943;(_tmp943=0,Cyc_Tcutil_warn(last_loc,((_tmp944="missing type within specifier",
_tag_dyneither(_tmp944,sizeof(char),30))),_tag_dyneither(_tmp943,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{if(seen_sign){void*_tmp12C=t;enum Cyc_Absyn_Sign
_tmp12E;enum Cyc_Absyn_Size_of _tmp12F;_LL81: {struct Cyc_Absyn_IntType_struct*
_tmp12D=(struct Cyc_Absyn_IntType_struct*)_tmp12C;if(_tmp12D->tag != 6)goto _LL83;
else{_tmp12E=_tmp12D->f1;_tmp12F=_tmp12D->f2;}}_LL82: if(_tmp12E != sgn)t=Cyc_Absyn_int_typ(
sgn,_tmp12F);goto _LL80;_LL83:;_LL84:{const char*_tmp945;Cyc_Parse_err(((_tmp945="sign specification on non-integral type",
_tag_dyneither(_tmp945,sizeof(char),40))),last_loc);}goto _LL80;_LL80:;}if(
seen_size){void*_tmp131=t;enum Cyc_Absyn_Sign _tmp133;enum Cyc_Absyn_Size_of
_tmp134;_LL86: {struct Cyc_Absyn_IntType_struct*_tmp132=(struct Cyc_Absyn_IntType_struct*)
_tmp131;if(_tmp132->tag != 6)goto _LL88;else{_tmp133=_tmp132->f1;_tmp134=_tmp132->f2;}}
_LL87: if(_tmp134 != sz)t=Cyc_Absyn_int_typ(_tmp133,sz);goto _LL85;_LL88: {struct
Cyc_Absyn_DoubleType_struct*_tmp135=(struct Cyc_Absyn_DoubleType_struct*)_tmp131;
if(_tmp135->tag != 8)goto _LL8A;}_LL89: t=Cyc_Absyn_double_typ(1);goto _LL85;_LL8A:;
_LL8B:{const char*_tmp946;Cyc_Parse_err(((_tmp946="size qualifier on non-integral type",
_tag_dyneither(_tmp946,sizeof(char),36))),last_loc);}goto _LL85;_LL85:;}}return t;}
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)
return 0;{struct Cyc_Parse_Declarator*_tmp137=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmp138=_tmp137->id;struct Cyc_Absyn_Tqual _tmp13A;void*_tmp13B;
struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13D;struct _tuple10 _tmp139=
Cyc_Parse_apply_tms(tq,t,shared_atts,_tmp137->tms);_tmp13A=_tmp139.f1;_tmp13B=
_tmp139.f2;_tmp13C=_tmp139.f3;_tmp13D=_tmp139.f4;if(ds->tl == 0){struct _tuple12*
_tmp949;struct Cyc_List_List*_tmp948;return(_tmp948=_region_malloc(r,sizeof(*
_tmp948)),((_tmp948->hd=((_tmp949=_region_malloc(r,sizeof(*_tmp949)),((_tmp949->f1=
_tmp138,((_tmp949->f2=_tmp13A,((_tmp949->f3=_tmp13B,((_tmp949->f4=_tmp13C,((
_tmp949->f5=_tmp13D,_tmp949)))))))))))),((_tmp948->tl=0,_tmp948)))));}else{
struct _tuple12*_tmp94C;struct Cyc_List_List*_tmp94B;return(_tmp94B=_region_malloc(
r,sizeof(*_tmp94B)),((_tmp94B->hd=((_tmp94C=_region_malloc(r,sizeof(*_tmp94C)),((
_tmp94C->f1=_tmp138,((_tmp94C->f2=_tmp13A,((_tmp94C->f3=_tmp13B,((_tmp94C->f4=
_tmp13C,((_tmp94C->f5=_tmp13D,_tmp94C)))))))))))),((_tmp94B->tl=Cyc_Parse_apply_tmss(
r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp94B)))));}};}static struct
_tuple10 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0){struct _tuple10 _tmp94D;return(_tmp94D.f1=
tq,((_tmp94D.f2=t,((_tmp94D.f3=0,((_tmp94D.f4=atts,_tmp94D)))))));}{void*_tmp143=(
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
struct Cyc_List_List*_tmp94E;fn_atts=((_tmp94E=_cycalloc(sizeof(*_tmp94E)),((
_tmp94E->hd=(void*)as->hd,((_tmp94E->tl=fn_atts,_tmp94E))))));}else{struct Cyc_List_List*
_tmp94F;new_atts=((_tmp94F=_cycalloc(sizeof(*_tmp94F)),((_tmp94F->hd=(void*)as->hd,((
_tmp94F->tl=new_atts,_tmp94F))))));}}}if(tms->tl != 0){void*_tmp161=(void*)((
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
_tmp15D->f2;}}_LL9D: {const char*_tmp952;void*_tmp951;(_tmp951=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
_tmp15E,((_tmp952="function declaration without parameter types",_tag_dyneither(
_tmp952,sizeof(char),45))),_tag_dyneither(_tmp951,sizeof(void*),0)));}_LL99:;}
_LL93: {struct Cyc_Absyn_TypeParams_mod_struct*_tmp14D=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp143;if(_tmp14D->tag != 4)goto _LL95;else{_tmp14E=_tmp14D->f1;_tmp14F=_tmp14D->f2;}}
_LL94: if(tms->tl == 0){struct _tuple10 _tmp953;return(_tmp953.f1=tq,((_tmp953.f2=t,((
_tmp953.f3=_tmp14E,((_tmp953.f4=atts,_tmp953)))))));}{const char*_tmp956;void*
_tmp955;(_tmp955=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp14F,((_tmp956="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp956,sizeof(char),68))),_tag_dyneither(_tmp955,sizeof(void*),0)));};
_LL95: {struct Cyc_Absyn_Pointer_mod_struct*_tmp150=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp143;if(_tmp150->tag != 2)goto _LL97;else{_tmp151=_tmp150->f1;_tmp152=_tmp150->f2;}}
_LL96: {struct Cyc_Absyn_PointerType_struct _tmp95C;struct Cyc_Absyn_PtrInfo _tmp95B;
struct Cyc_Absyn_PointerType_struct*_tmp95A;return Cyc_Parse_apply_tms(_tmp152,(
void*)((_tmp95A=_cycalloc(sizeof(*_tmp95A)),((_tmp95A[0]=((_tmp95C.tag=5,((
_tmp95C.f1=((_tmp95B.elt_typ=t,((_tmp95B.elt_tq=tq,((_tmp95B.ptr_atts=_tmp151,
_tmp95B)))))),_tmp95C)))),_tmp95A)))),atts,tms->tl);}_LL97: {struct Cyc_Absyn_Attributes_mod_struct*
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
else{kind=_tmp17D;}goto _LLA3;_LLA6:;_LLA7: kind=0;{struct Cyc_Core_Opt*_tmp95D;
type=((_tmp95D=_cycalloc(sizeof(*_tmp95D)),((_tmp95D->v=_tmp178,_tmp95D))));}
goto _LLA3;_LLA3:;}{struct Cyc_Absyn_Typedef_d_struct _tmp963;struct Cyc_Absyn_Typedefdecl*
_tmp962;struct Cyc_Absyn_Typedef_d_struct*_tmp961;return Cyc_Absyn_new_decl((void*)((
_tmp961=_cycalloc(sizeof(*_tmp961)),((_tmp961[0]=((_tmp963.tag=9,((_tmp963.f1=((
_tmp962=_cycalloc(sizeof(*_tmp962)),((_tmp962->name=_tmp176,((_tmp962->tvs=
_tmp179,((_tmp962->kind=kind,((_tmp962->defn=type,((_tmp962->atts=_tmp17A,((
_tmp962->tq=_tmp177,_tmp962)))))))))))))),_tmp963)))),_tmp961)))),loc);};};}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct
Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Decl_s_struct _tmp966;struct Cyc_Absyn_Decl_s_struct*
_tmp965;return Cyc_Absyn_new_stmt((void*)((_tmp965=_cycalloc(sizeof(*_tmp965)),((
_tmp965[0]=((_tmp966.tag=12,((_tmp966.f1=d,((_tmp966.f2=s,_tmp966)))))),_tmp965)))),
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
const char*_tmp967;Cyc_Parse_err(((_tmp967="inline is allowed only on function definitions",
_tag_dyneither(_tmp967,sizeof(char),47))),loc);}if(_tmp188 == 0){{const char*
_tmp96A;void*_tmp969;(_tmp969=0,Cyc_Tcutil_warn(loc,((_tmp96A="missing type specifiers in declaration, assuming int",
_tag_dyneither(_tmp96A,sizeof(char),53))),_tag_dyneither(_tmp969,sizeof(void*),0)));}{
void*_tmp96B[1];_tmp188=((_tmp96B[0]=Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,loc),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp96B,sizeof(void*),1))));};}{enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;int
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
_tmp189);{struct Cyc_Absyn_Aggr_d_struct*_tmp971;struct Cyc_Absyn_Aggr_d_struct
_tmp970;struct Cyc_List_List*_tmp96F;struct Cyc_List_List*_tmp1BE=(_tmp96F=
_cycalloc(sizeof(*_tmp96F)),((_tmp96F->hd=Cyc_Absyn_new_decl((void*)((_tmp971=
_cycalloc(sizeof(*_tmp971)),((_tmp971[0]=((_tmp970.tag=6,((_tmp970.f1=_tmp198,
_tmp970)))),_tmp971)))),loc),((_tmp96F->tl=0,_tmp96F)))));_npop_handler(0);
return _tmp1BE;};_LLB3: {struct Cyc_Absyn_TypeDeclType_struct*_tmp199=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp192;if(_tmp199->tag != 27)goto _LLB5;else{_tmp19A=_tmp199->f1;_tmp19B=*_tmp19A;
_tmp19C=_tmp19B.r;{struct Cyc_Absyn_Enum_td_struct*_tmp19D=(struct Cyc_Absyn_Enum_td_struct*)
_tmp19C;if(_tmp19D->tag != 1)goto _LLB5;else{_tmp19E=_tmp19D->f1;}};}}_LLB4: if(
_tmp189 != 0){const char*_tmp972;Cyc_Parse_err(((_tmp972="attributes on enum not supported",
_tag_dyneither(_tmp972,sizeof(char),33))),loc);}{struct Cyc_Absyn_Enum_d_struct*
_tmp978;struct Cyc_Absyn_Enum_d_struct _tmp977;struct Cyc_List_List*_tmp976;struct
Cyc_List_List*_tmp1C3=(_tmp976=_cycalloc(sizeof(*_tmp976)),((_tmp976->hd=Cyc_Absyn_new_decl((
void*)((_tmp978=_cycalloc(sizeof(*_tmp978)),((_tmp978[0]=((_tmp977.tag=8,((
_tmp977.f1=_tmp19E,_tmp977)))),_tmp978)))),loc),((_tmp976->tl=0,_tmp976)))));
_npop_handler(0);return _tmp1C3;};_LLB5: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp19F=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp192;if(_tmp19F->tag != 27)goto
_LLB7;else{_tmp1A0=_tmp19F->f1;_tmp1A1=*_tmp1A0;_tmp1A2=_tmp1A1.r;{struct Cyc_Absyn_Datatype_td_struct*
_tmp1A3=(struct Cyc_Absyn_Datatype_td_struct*)_tmp1A2;if(_tmp1A3->tag != 2)goto
_LLB7;else{_tmp1A4=_tmp1A3->f1;}};}}_LLB6: if(_tmp189 != 0){const char*_tmp979;Cyc_Parse_err(((
_tmp979="attributes on datatypes not supported",_tag_dyneither(_tmp979,sizeof(
char),38))),loc);}{struct Cyc_Absyn_Datatype_d_struct*_tmp97F;struct Cyc_Absyn_Datatype_d_struct
_tmp97E;struct Cyc_List_List*_tmp97D;struct Cyc_List_List*_tmp1C8=(_tmp97D=
_cycalloc(sizeof(*_tmp97D)),((_tmp97D->hd=Cyc_Absyn_new_decl((void*)((_tmp97F=
_cycalloc(sizeof(*_tmp97F)),((_tmp97F[0]=((_tmp97E.tag=7,((_tmp97E.f1=_tmp1A4,
_tmp97E)))),_tmp97F)))),loc),((_tmp97D->tl=0,_tmp97D)))));_npop_handler(0);
return _tmp1C8;};_LLB7: {struct Cyc_Absyn_AggrType_struct*_tmp1A5=(struct Cyc_Absyn_AggrType_struct*)
_tmp192;if(_tmp1A5->tag != 12)goto _LLB9;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A6.aggr_info;
if((_tmp1A7.UnknownAggr).tag != 1)goto _LLB9;_tmp1A8=(struct _tuple3)(_tmp1A7.UnknownAggr).val;
_tmp1A9=_tmp1A8.f1;_tmp1AA=_tmp1A8.f2;_tmp1AB=_tmp1A6.targs;}}_LLB8: {struct Cyc_List_List*
_tmp1C9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp1AB);struct Cyc_Absyn_Aggrdecl*_tmp980;struct Cyc_Absyn_Aggrdecl*_tmp1CA=(
_tmp980=_cycalloc(sizeof(*_tmp980)),((_tmp980->kind=_tmp1A9,((_tmp980->sc=s,((
_tmp980->name=_tmp1AA,((_tmp980->tvs=_tmp1C9,((_tmp980->impl=0,((_tmp980->attributes=
0,_tmp980)))))))))))));if(_tmp189 != 0){const char*_tmp981;Cyc_Parse_err(((_tmp981="bad attributes on type declaration",
_tag_dyneither(_tmp981,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp987;struct Cyc_Absyn_Aggr_d_struct _tmp986;struct Cyc_List_List*_tmp985;struct
Cyc_List_List*_tmp1CF=(_tmp985=_cycalloc(sizeof(*_tmp985)),((_tmp985->hd=Cyc_Absyn_new_decl((
void*)((_tmp987=_cycalloc(sizeof(*_tmp987)),((_tmp987[0]=((_tmp986.tag=6,((
_tmp986.f1=_tmp1CA,_tmp986)))),_tmp987)))),loc),((_tmp985->tl=0,_tmp985)))));
_npop_handler(0);return _tmp1CF;};}_LLB9: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1AC=(struct Cyc_Absyn_DatatypeType_struct*)_tmp192;if(_tmp1AC->tag != 3)goto
_LLBB;else{_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AD.datatype_info;if((_tmp1AE.KnownDatatype).tag
!= 2)goto _LLBB;_tmp1AF=(struct Cyc_Absyn_Datatypedecl**)(_tmp1AE.KnownDatatype).val;}}
_LLBA: if(_tmp189 != 0){const char*_tmp988;Cyc_Parse_err(((_tmp988="bad attributes on datatype",
_tag_dyneither(_tmp988,sizeof(char),27))),loc);}{struct Cyc_Absyn_Datatype_d_struct*
_tmp98E;struct Cyc_Absyn_Datatype_d_struct _tmp98D;struct Cyc_List_List*_tmp98C;
struct Cyc_List_List*_tmp1D5=(_tmp98C=_cycalloc(sizeof(*_tmp98C)),((_tmp98C->hd=
Cyc_Absyn_new_decl((void*)((_tmp98E=_cycalloc(sizeof(*_tmp98E)),((_tmp98E[0]=((
_tmp98D.tag=7,((_tmp98D.f1=*_tmp1AF,_tmp98D)))),_tmp98E)))),loc),((_tmp98C->tl=0,
_tmp98C)))));_npop_handler(0);return _tmp1D5;};_LLBB: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1B0=(struct Cyc_Absyn_DatatypeType_struct*)_tmp192;if(_tmp1B0->tag != 3)goto
_LLBD;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B1.datatype_info;if((_tmp1B2.UnknownDatatype).tag
!= 1)goto _LLBD;_tmp1B3=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B2.UnknownDatatype).val;
_tmp1B4=_tmp1B3.name;_tmp1B5=_tmp1B3.is_extensible;_tmp1B6=_tmp1B1.targs;}}_LLBC: {
struct Cyc_List_List*_tmp1D6=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp1B6);struct Cyc_Absyn_Decl*_tmp1D7=
Cyc_Absyn_datatype_decl(s,_tmp1B4,_tmp1D6,0,_tmp1B5,loc);if(_tmp189 != 0){const
char*_tmp98F;Cyc_Parse_err(((_tmp98F="bad attributes on datatype",_tag_dyneither(
_tmp98F,sizeof(char),27))),loc);}{struct Cyc_List_List*_tmp990;struct Cyc_List_List*
_tmp1DA=(_tmp990=_cycalloc(sizeof(*_tmp990)),((_tmp990->hd=_tmp1D7,((_tmp990->tl=
0,_tmp990)))));_npop_handler(0);return _tmp1DA;};}_LLBD: {struct Cyc_Absyn_EnumType_struct*
_tmp1B7=(struct Cyc_Absyn_EnumType_struct*)_tmp192;if(_tmp1B7->tag != 14)goto _LLBF;
else{_tmp1B8=_tmp1B7->f1;}}_LLBE: {struct Cyc_Absyn_Enumdecl*_tmp991;struct Cyc_Absyn_Enumdecl*
_tmp1DB=(_tmp991=_cycalloc(sizeof(*_tmp991)),((_tmp991->sc=s,((_tmp991->name=
_tmp1B8,((_tmp991->fields=0,_tmp991)))))));if(_tmp189 != 0){const char*_tmp992;Cyc_Parse_err(((
_tmp992="bad attributes on enum",_tag_dyneither(_tmp992,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp99C;struct Cyc_Absyn_Enum_d_struct _tmp99B;struct Cyc_Absyn_Enum_d_struct*
_tmp99A;struct Cyc_List_List*_tmp999;struct Cyc_List_List*_tmp1E1=(_tmp999=
_cycalloc(sizeof(*_tmp999)),((_tmp999->hd=((_tmp99C=_cycalloc(sizeof(*_tmp99C)),((
_tmp99C->r=(void*)((_tmp99A=_cycalloc(sizeof(*_tmp99A)),((_tmp99A[0]=((_tmp99B.tag=
8,((_tmp99B.f1=_tmp1DB,_tmp99B)))),_tmp99A)))),((_tmp99C->loc=loc,_tmp99C)))))),((
_tmp999->tl=0,_tmp999)))));_npop_handler(0);return _tmp1E1;};}_LLBF: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1B9=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp192;if(_tmp1B9->tag != 15)goto
_LLC1;else{_tmp1BA=_tmp1B9->f1;}}_LLC0: {struct Cyc_Core_Opt*_tmp99F;struct Cyc_Absyn_Enumdecl*
_tmp99E;struct Cyc_Absyn_Enumdecl*_tmp1E3=(_tmp99E=_cycalloc(sizeof(*_tmp99E)),((
_tmp99E->sc=s,((_tmp99E->name=Cyc_Parse_gensym_enum(),((_tmp99E->fields=((
_tmp99F=_cycalloc(sizeof(*_tmp99F)),((_tmp99F->v=_tmp1BA,_tmp99F)))),_tmp99E)))))));
if(_tmp189 != 0){const char*_tmp9A0;Cyc_Parse_err(((_tmp9A0="bad attributes on enum",
_tag_dyneither(_tmp9A0,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp9AA;
struct Cyc_Absyn_Enum_d_struct _tmp9A9;struct Cyc_Absyn_Enum_d_struct*_tmp9A8;
struct Cyc_List_List*_tmp9A7;struct Cyc_List_List*_tmp1E9=(_tmp9A7=_cycalloc(
sizeof(*_tmp9A7)),((_tmp9A7->hd=((_tmp9AA=_cycalloc(sizeof(*_tmp9AA)),((_tmp9AA->r=(
void*)((_tmp9A8=_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8[0]=((_tmp9A9.tag=8,((
_tmp9A9.f1=_tmp1E3,_tmp9A9)))),_tmp9A8)))),((_tmp9AA->loc=loc,_tmp9AA)))))),((
_tmp9A7->tl=0,_tmp9A7)))));_npop_handler(0);return _tmp1E9;};}_LLC1:;_LLC2:{const
char*_tmp9AB;Cyc_Parse_err(((_tmp9AB="missing declarator",_tag_dyneither(_tmp9AB,
sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1ED=0;_npop_handler(0);return
_tmp1ED;};_LLB0:;}else{struct Cyc_List_List*_tmp1EE=Cyc_Parse_apply_tmss(mkrgn,
_tmp187,_tmp191,_tmp18F,_tmp189);if(istypedef){if(!exps_empty){const char*_tmp9AC;
Cyc_Parse_err(((_tmp9AC="initializer in typedef declaration",_tag_dyneither(
_tmp9AC,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp1EE);struct Cyc_List_List*_tmp1F0=decls;_npop_handler(0);return _tmp1F0;};}
else{struct Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp1F1=_tmp1EE;for(0;
_tmp1F1 != 0;(_tmp1F1=_tmp1F1->tl,_tmp190=_tmp190->tl)){struct _tuple12 _tmp1F3;
struct _tuple1*_tmp1F4;struct Cyc_Absyn_Tqual _tmp1F5;void*_tmp1F6;struct Cyc_List_List*
_tmp1F7;struct Cyc_List_List*_tmp1F8;struct _tuple12*_tmp1F2=(struct _tuple12*)
_tmp1F1->hd;_tmp1F3=*_tmp1F2;_tmp1F4=_tmp1F3.f1;_tmp1F5=_tmp1F3.f2;_tmp1F6=
_tmp1F3.f3;_tmp1F7=_tmp1F3.f4;_tmp1F8=_tmp1F3.f5;if(_tmp1F7 != 0){const char*
_tmp9AF;void*_tmp9AE;(_tmp9AE=0,Cyc_Tcutil_warn(loc,((_tmp9AF="bad type params, ignoring",
_tag_dyneither(_tmp9AF,sizeof(char),26))),_tag_dyneither(_tmp9AE,sizeof(void*),0)));}
if(_tmp190 == 0){const char*_tmp9B2;void*_tmp9B1;(_tmp9B1=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp9B2="unexpected NULL in parse!",_tag_dyneither(_tmp9B2,sizeof(char),26))),
_tag_dyneither(_tmp9B1,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*_tmp1FD=(struct
Cyc_Absyn_Exp*)_tmp190->hd;struct Cyc_Absyn_Vardecl*_tmp1FE=Cyc_Absyn_new_vardecl(
_tmp1F4,_tmp1F6,_tmp1FD);_tmp1FE->tq=_tmp1F5;_tmp1FE->sc=s;_tmp1FE->attributes=
_tmp1F8;{struct Cyc_Absyn_Var_d_struct*_tmp9B8;struct Cyc_Absyn_Var_d_struct
_tmp9B7;struct Cyc_Absyn_Decl*_tmp9B6;struct Cyc_Absyn_Decl*_tmp1FF=(_tmp9B6=
_cycalloc(sizeof(*_tmp9B6)),((_tmp9B6->r=(void*)((_tmp9B8=_cycalloc(sizeof(*
_tmp9B8)),((_tmp9B8[0]=((_tmp9B7.tag=0,((_tmp9B7.f1=_tmp1FE,_tmp9B7)))),_tmp9B8)))),((
_tmp9B6->loc=loc,_tmp9B6)))));struct Cyc_List_List*_tmp9B9;decls=((_tmp9B9=
_cycalloc(sizeof(*_tmp9B9)),((_tmp9B9->hd=_tmp1FF,((_tmp9B9->tl=decls,_tmp9B9))))));};};}}{
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
_LLD6: break;}break;default: _LLD1: break;}{const char*_tmp9BE;void*_tmp9BD[2];
struct Cyc_String_pa_struct _tmp9BC;struct Cyc_Int_pa_struct _tmp9BB;Cyc_Parse_err((
struct _dyneither_ptr)((_tmp9BB.tag=1,((_tmp9BB.f1=(unsigned long)((int)Cyc_strlen((
struct _dyneither_ptr)s)),((_tmp9BC.tag=0,((_tmp9BC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s),((_tmp9BD[0]=& _tmp9BC,((_tmp9BD[1]=& _tmp9BB,Cyc_aprintf(((
_tmp9BE="bad kind: %s; strlen=%d",_tag_dyneither(_tmp9BE,sizeof(char),24))),
_tag_dyneither(_tmp9BD,sizeof(void*),2)))))))))))))),loc);}return& Cyc_Tcutil_bk;}
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*_tmp209=
p->r;struct _tuple1*_tmp20B;struct Cyc_Absyn_Vardecl*_tmp20D;struct Cyc_Absyn_Pat*
_tmp20E;struct Cyc_Absyn_Pat _tmp20F;void*_tmp210;struct Cyc_Absyn_Pat*_tmp213;
enum Cyc_Absyn_Sign _tmp216;int _tmp217;char _tmp219;struct _dyneither_ptr _tmp21B;
struct _tuple1*_tmp21D;struct Cyc_List_List*_tmp21E;int _tmp21F;struct Cyc_Absyn_Exp*
_tmp221;_LLDA: {struct Cyc_Absyn_UnknownId_p_struct*_tmp20A=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp209;if(_tmp20A->tag != 14)goto _LLDC;else{_tmp20B=_tmp20A->f1;}}_LLDB: {struct
Cyc_Absyn_UnknownId_e_struct _tmp9C1;struct Cyc_Absyn_UnknownId_e_struct*_tmp9C0;
return Cyc_Absyn_new_exp((void*)((_tmp9C0=_cycalloc(sizeof(*_tmp9C0)),((_tmp9C0[0]=((
_tmp9C1.tag=2,((_tmp9C1.f1=_tmp20B,_tmp9C1)))),_tmp9C0)))),p->loc);}_LLDC: {
struct Cyc_Absyn_Reference_p_struct*_tmp20C=(struct Cyc_Absyn_Reference_p_struct*)
_tmp209;if(_tmp20C->tag != 2)goto _LLDE;else{_tmp20D=_tmp20C->f1;_tmp20E=_tmp20C->f2;
_tmp20F=*_tmp20E;_tmp210=_tmp20F.r;{struct Cyc_Absyn_Wild_p_struct*_tmp211=(
struct Cyc_Absyn_Wild_p_struct*)_tmp210;if(_tmp211->tag != 0)goto _LLDE;};}}_LLDD: {
struct Cyc_Absyn_UnknownId_e_struct _tmp9C4;struct Cyc_Absyn_UnknownId_e_struct*
_tmp9C3;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmp9C3=_cycalloc(
sizeof(*_tmp9C3)),((_tmp9C3[0]=((_tmp9C4.tag=2,((_tmp9C4.f1=_tmp20D->name,
_tmp9C4)))),_tmp9C3)))),p->loc),p->loc);}_LLDE: {struct Cyc_Absyn_Pointer_p_struct*
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
_tmp9C7;struct Cyc_Absyn_UnknownId_e_struct*_tmp9C6;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmp9C6=_cycalloc(sizeof(*_tmp9C6)),((_tmp9C6[0]=((_tmp9C7.tag=2,((
_tmp9C7.f1=_tmp21D,_tmp9C7)))),_tmp9C6)))),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp21E);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLEA: {struct Cyc_Absyn_Exp_p_struct*_tmp220=(struct Cyc_Absyn_Exp_p_struct*)
_tmp209;if(_tmp220->tag != 16)goto _LLEC;else{_tmp221=_tmp220->f1;}}_LLEB: return
_tmp221;_LLEC:;_LLED:{const char*_tmp9C8;Cyc_Parse_err(((_tmp9C8="cannot mix patterns and expressions in case",
_tag_dyneither(_tmp9C8,sizeof(char),44))),p->loc);}return Cyc_Absyn_null_exp(p->loc);
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
yyzzz;}union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){union Cyc_YYSTYPE _tmp9C9;
return((_tmp9C9.Int_tok).val=yy1,(((_tmp9C9.Int_tok).tag=1,_tmp9C9)));}static
char _tmp22F[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp22F,_tmp22F,_tmp22F + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE
yy1){char yyzzz;{union Cyc_YYSTYPE _tmp230=yy1;char _tmp231;_LLF4: if((_tmp230.Char_tok).tag
!= 2)goto _LLF6;_tmp231=(char)(_tmp230.Char_tok).val;_LLF5: yyzzz=_tmp231;goto
_LLF3;_LLF6:;_LLF7:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLF3:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9CA;return((_tmp9CA.Char_tok).val=
yy1,(((_tmp9CA.Char_tok).tag=2,_tmp9CA)));}static char _tmp234[13]="string_t<`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{
_tmp234,_tmp234,_tmp234 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp235=yy1;struct _dyneither_ptr
_tmp236;_LLF9: if((_tmp235.String_tok).tag != 3)goto _LLFB;_tmp236=(struct
_dyneither_ptr)(_tmp235.String_tok).val;_LLFA: yyzzz=_tmp236;goto _LLF8;_LLFB:;
_LLFC:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLF8:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9CB;return((_tmp9CB.String_tok).val=
yy1,(((_tmp9CB.String_tok).tag=3,_tmp9CB)));}static char _tmp239[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp239,
_tmp239,_tmp239 + 56}};struct _tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct
_tuple18*yyzzz;{union Cyc_YYSTYPE _tmp23A=yy1;struct _tuple18*_tmp23B;_LLFE: if((
_tmp23A.YY1).tag != 7)goto _LL100;_tmp23B=(struct _tuple18*)(_tmp23A.YY1).val;_LLFF:
yyzzz=_tmp23B;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)& Cyc_yyfail_YY1);_LLFD:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY1(struct _tuple18*yy1){union Cyc_YYSTYPE _tmp9CC;
return((_tmp9CC.YY1).val=yy1,(((_tmp9CC.YY1).tag=7,_tmp9CC)));}static char _tmp23E[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp23E,_tmp23E,_tmp23E + 19}};union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE
yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp23F=yy1;union Cyc_Absyn_Constraint*
_tmp240;_LL103: if((_tmp23F.YY2).tag != 8)goto _LL105;_tmp240=(union Cyc_Absyn_Constraint*)(
_tmp23F.YY2).val;_LL104: yyzzz=_tmp240;goto _LL102;_LL105:;_LL106:(int)_throw((
void*)& Cyc_yyfail_YY2);_LL102:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*
yy1){union Cyc_YYSTYPE _tmp9CD;return((_tmp9CD.YY2).val=yy1,(((_tmp9CD.YY2).tag=8,
_tmp9CD)));}static char _tmp243[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE _tmp244=yy1;
struct Cyc_Absyn_Exp*_tmp245;_LL108: if((_tmp244.YY3).tag != 9)goto _LL10A;_tmp245=(
struct Cyc_Absyn_Exp*)(_tmp244.YY3).val;_LL109: yyzzz=_tmp245;goto _LL107;_LL10A:;
_LL10B:(int)_throw((void*)& Cyc_yyfail_YY3);_LL107:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY3(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9CE;return((_tmp9CE.YY3).val=
yy1,(((_tmp9CE.YY3).tag=9,_tmp9CE)));}static char _tmp248[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp248,_tmp248,
_tmp248 + 10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE _tmp249=yy1;struct Cyc_Absyn_Exp*_tmp24A;_LL10D: if((
_tmp249.YY4).tag != 10)goto _LL10F;_tmp24A=(struct Cyc_Absyn_Exp*)(_tmp249.YY4).val;
_LL10E: yyzzz=_tmp24A;goto _LL10C;_LL10F:;_LL110:(int)_throw((void*)& Cyc_yyfail_YY4);
_LL10C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE
_tmp9CF;return((_tmp9CF.YY4).val=yy1,(((_tmp9CF.YY4).tag=10,_tmp9CF)));}static
char _tmp24D[17]="list_t<exp_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={
Cyc_Core_Failure,{_tmp24D,_tmp24D,_tmp24D + 17}};struct Cyc_List_List*Cyc_yyget_YY5(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp24E=yy1;
struct Cyc_List_List*_tmp24F;_LL112: if((_tmp24E.YY5).tag != 11)goto _LL114;_tmp24F=(
struct Cyc_List_List*)(_tmp24E.YY5).val;_LL113: yyzzz=_tmp24F;goto _LL111;_LL114:;
_LL115:(int)_throw((void*)& Cyc_yyfail_YY5);_LL111:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D0;return((_tmp9D0.YY5).val=
yy1,(((_tmp9D0.YY5).tag=11,_tmp9D0)));}static char _tmp252[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp252,
_tmp252,_tmp252 + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp253=yy1;struct Cyc_List_List*
_tmp254;_LL117: if((_tmp253.YY6).tag != 12)goto _LL119;_tmp254=(struct Cyc_List_List*)(
_tmp253.YY6).val;_LL118: yyzzz=_tmp254;goto _LL116;_LL119:;_LL11A:(int)_throw((
void*)& Cyc_yyfail_YY6);_LL116:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9D1;return((_tmp9D1.YY6).val=yy1,(((_tmp9D1.YY6).tag=12,
_tmp9D1)));}static char _tmp257[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp257,_tmp257,_tmp257 + 9}};enum Cyc_Absyn_Primop
Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Primop yyzzz;{union Cyc_YYSTYPE
_tmp258=yy1;enum Cyc_Absyn_Primop _tmp259;_LL11C: if((_tmp258.YY7).tag != 13)goto
_LL11E;_tmp259=(enum Cyc_Absyn_Primop)(_tmp258.YY7).val;_LL11D: yyzzz=_tmp259;
goto _LL11B;_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY7);_LL11B:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9D2;
return((_tmp9D2.YY7).val=yy1,(((_tmp9D2.YY7).tag=13,_tmp9D2)));}static char
_tmp25C[19]="opt_t<primop_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={
Cyc_Core_Failure,{_tmp25C,_tmp25C,_tmp25C + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(
union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp25D=yy1;
struct Cyc_Core_Opt*_tmp25E;_LL121: if((_tmp25D.YY8).tag != 14)goto _LL123;_tmp25E=(
struct Cyc_Core_Opt*)(_tmp25D.YY8).val;_LL122: yyzzz=_tmp25E;goto _LL120;_LL123:;
_LL124:(int)_throw((void*)& Cyc_yyfail_YY8);_LL120:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY8(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9D3;return((_tmp9D3.YY8).val=
yy1,(((_tmp9D3.YY8).tag=14,_tmp9D3)));}static char _tmp261[7]="qvar_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp261,
_tmp261,_tmp261 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp262=yy1;struct _tuple1*_tmp263;_LL126:
if((_tmp262.QualId_tok).tag != 5)goto _LL128;_tmp263=(struct _tuple1*)(_tmp262.QualId_tok).val;
_LL127: yyzzz=_tmp263;goto _LL125;_LL128:;_LL129:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL125:;}return yyzzz;}union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple1*yy1){union Cyc_YYSTYPE
_tmp9D4;return((_tmp9D4.QualId_tok).val=yy1,(((_tmp9D4.QualId_tok).tag=5,_tmp9D4)));}
static char _tmp266[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY9={
Cyc_Core_Failure,{_tmp266,_tmp266,_tmp266 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE _tmp267=yy1;
struct Cyc_Absyn_Stmt*_tmp268;_LL12B: if((_tmp267.YY9).tag != 15)goto _LL12D;_tmp268=(
struct Cyc_Absyn_Stmt*)(_tmp267.YY9).val;_LL12C: yyzzz=_tmp268;goto _LL12A;_LL12D:;
_LL12E:(int)_throw((void*)& Cyc_yyfail_YY9);_LL12A:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9D5;return((_tmp9D5.YY9).val=
yy1,(((_tmp9D5.YY9).tag=15,_tmp9D5)));}static char _tmp26B[27]="list_t<switch_clause_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp26B,
_tmp26B,_tmp26B + 27}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp26C=yy1;struct Cyc_List_List*
_tmp26D;_LL130: if((_tmp26C.YY10).tag != 16)goto _LL132;_tmp26D=(struct Cyc_List_List*)(
_tmp26C.YY10).val;_LL131: yyzzz=_tmp26D;goto _LL12F;_LL132:;_LL133:(int)_throw((
void*)& Cyc_yyfail_YY10);_LL12F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9D6;return((_tmp9D6.YY10).val=yy1,(((_tmp9D6.YY10).tag=
16,_tmp9D6)));}static char _tmp270[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp270,_tmp270,_tmp270 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE
_tmp271=yy1;struct Cyc_Absyn_Pat*_tmp272;_LL135: if((_tmp271.YY11).tag != 17)goto
_LL137;_tmp272=(struct Cyc_Absyn_Pat*)(_tmp271.YY11).val;_LL136: yyzzz=_tmp272;
goto _LL134;_LL137:;_LL138:(int)_throw((void*)& Cyc_yyfail_YY11);_LL134:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9D7;
return((_tmp9D7.YY11).val=yy1,(((_tmp9D7.YY11).tag=17,_tmp9D7)));}static char
_tmp275[28]="$(list_t<pat_t,`H>,bool)@`H";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={
Cyc_Core_Failure,{_tmp275,_tmp275,_tmp275 + 28}};struct _tuple19*Cyc_yyget_YY12(
union Cyc_YYSTYPE yy1){struct _tuple19*yyzzz;{union Cyc_YYSTYPE _tmp276=yy1;struct
_tuple19*_tmp277;_LL13A: if((_tmp276.YY12).tag != 18)goto _LL13C;_tmp277=(struct
_tuple19*)(_tmp276.YY12).val;_LL13B: yyzzz=_tmp277;goto _LL139;_LL13C:;_LL13D:(int)
_throw((void*)& Cyc_yyfail_YY12);_LL139:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY12(
struct _tuple19*yy1){union Cyc_YYSTYPE _tmp9D8;return((_tmp9D8.YY12).val=yy1,(((
_tmp9D8.YY12).tag=18,_tmp9D8)));}static char _tmp27A[17]="list_t<pat_t,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp27A,_tmp27A,
_tmp27A + 17}};struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp27B=yy1;struct Cyc_List_List*_tmp27C;_LL13F: if((
_tmp27B.YY13).tag != 19)goto _LL141;_tmp27C=(struct Cyc_List_List*)(_tmp27B.YY13).val;
_LL140: yyzzz=_tmp27C;goto _LL13E;_LL141:;_LL142:(int)_throw((void*)& Cyc_yyfail_YY13);
_LL13E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9D9;return((_tmp9D9.YY13).val=yy1,(((_tmp9D9.YY13).tag=19,_tmp9D9)));}static
char _tmp27F[36]="$(list_t<designator_t,`H>,pat_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp27F,_tmp27F,_tmp27F + 36}};struct _tuple20*
Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){struct _tuple20*yyzzz;{union Cyc_YYSTYPE
_tmp280=yy1;struct _tuple20*_tmp281;_LL144: if((_tmp280.YY14).tag != 20)goto _LL146;
_tmp281=(struct _tuple20*)(_tmp280.YY14).val;_LL145: yyzzz=_tmp281;goto _LL143;
_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY14);_LL143:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY14(struct _tuple20*yy1){union Cyc_YYSTYPE _tmp9DA;return((_tmp9DA.YY14).val=
yy1,(((_tmp9DA.YY14).tag=20,_tmp9DA)));}static char _tmp284[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp284,
_tmp284,_tmp284 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp285=yy1;struct Cyc_List_List*
_tmp286;_LL149: if((_tmp285.YY15).tag != 21)goto _LL14B;_tmp286=(struct Cyc_List_List*)(
_tmp285.YY15).val;_LL14A: yyzzz=_tmp286;goto _LL148;_LL14B:;_LL14C:(int)_throw((
void*)& Cyc_yyfail_YY15);_LL148:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9DB;return((_tmp9DB.YY15).val=yy1,(((_tmp9DB.YY15).tag=
21,_tmp9DB)));}static char _tmp289[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp289,
_tmp289,_tmp289 + 58}};struct _tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct
_tuple19*yyzzz;{union Cyc_YYSTYPE _tmp28A=yy1;struct _tuple19*_tmp28B;_LL14E: if((
_tmp28A.YY16).tag != 22)goto _LL150;_tmp28B=(struct _tuple19*)(_tmp28A.YY16).val;
_LL14F: yyzzz=_tmp28B;goto _LL14D;_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY16);
_LL14D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY16(struct _tuple19*yy1){union Cyc_YYSTYPE
_tmp9DC;return((_tmp9DC.YY16).val=yy1,(((_tmp9DC.YY16).tag=22,_tmp9DC)));}static
char _tmp28E[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp28E,_tmp28E,_tmp28E + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp28F=yy1;
struct Cyc_Absyn_Fndecl*_tmp290;_LL153: if((_tmp28F.YY17).tag != 23)goto _LL155;
_tmp290=(struct Cyc_Absyn_Fndecl*)(_tmp28F.YY17).val;_LL154: yyzzz=_tmp290;goto
_LL152;_LL155:;_LL156:(int)_throw((void*)& Cyc_yyfail_YY17);_LL152:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9DD;
return((_tmp9DD.YY17).val=yy1,(((_tmp9DD.YY17).tag=23,_tmp9DD)));}static char
_tmp293[18]="list_t<decl_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp293,_tmp293,_tmp293 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp294=yy1;
struct Cyc_List_List*_tmp295;_LL158: if((_tmp294.YY18).tag != 24)goto _LL15A;_tmp295=(
struct Cyc_List_List*)(_tmp294.YY18).val;_LL159: yyzzz=_tmp295;goto _LL157;_LL15A:;
_LL15B:(int)_throw((void*)& Cyc_yyfail_YY18);_LL157:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DE;return((_tmp9DE.YY18).val=
yy1,(((_tmp9DE.YY18).tag=24,_tmp9DE)));}static char _tmp298[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp298,
_tmp298,_tmp298 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp299=yy1;struct
Cyc_Parse_Declaration_spec*_tmp29A;_LL15D: if((_tmp299.YY19).tag != 25)goto _LL15F;
_tmp29A=(struct Cyc_Parse_Declaration_spec*)(_tmp299.YY19).val;_LL15E: yyzzz=
_tmp29A;goto _LL15C;_LL15F:;_LL160:(int)_throw((void*)& Cyc_yyfail_YY19);_LL15C:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){
union Cyc_YYSTYPE _tmp9DF;return((_tmp9DF.YY19).val=yy1,(((_tmp9DF.YY19).tag=25,
_tmp9DF)));}static char _tmp29D[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp29D,_tmp29D,_tmp29D + 29}};struct _tuple21*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE
_tmp29E=yy1;struct _tuple21*_tmp29F;_LL162: if((_tmp29E.YY20).tag != 26)goto _LL164;
_tmp29F=(struct _tuple21*)(_tmp29E.YY20).val;_LL163: yyzzz=_tmp29F;goto _LL161;
_LL164:;_LL165:(int)_throw((void*)& Cyc_yyfail_YY20);_LL161:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY20(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9E0;return((_tmp9E0.YY20).val=
yy1,(((_tmp9E0.YY20).tag=26,_tmp9E0)));}static char _tmp2A2[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp2A2,
_tmp2A2,_tmp2A2 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2A3=yy1;struct Cyc_List_List*
_tmp2A4;_LL167: if((_tmp2A3.YY21).tag != 27)goto _LL169;_tmp2A4=(struct Cyc_List_List*)(
_tmp2A3.YY21).val;_LL168: yyzzz=_tmp2A4;goto _LL166;_LL169:;_LL16A:(int)_throw((
void*)& Cyc_yyfail_YY21);_LL166:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9E1;return((_tmp9E1.YY21).val=yy1,(((_tmp9E1.YY21).tag=
27,_tmp9E1)));}static char _tmp2A7[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp2A7,_tmp2A7,_tmp2A7 + 16}};enum Cyc_Parse_Storage_class
Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){enum Cyc_Parse_Storage_class yyzzz;{union Cyc_YYSTYPE
_tmp2A8=yy1;enum Cyc_Parse_Storage_class _tmp2A9;_LL16C: if((_tmp2A8.YY22).tag != 
28)goto _LL16E;_tmp2A9=(enum Cyc_Parse_Storage_class)(_tmp2A8.YY22).val;_LL16D:
yyzzz=_tmp2A9;goto _LL16B;_LL16E:;_LL16F:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL16B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class yy1){
union Cyc_YYSTYPE _tmp9E2;return((_tmp9E2.YY22).val=yy1,(((_tmp9E2.YY22).tag=28,
_tmp9E2)));}static char _tmp2AC[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp2AC,_tmp2AC,_tmp2AC + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2AD=yy1;void*_tmp2AE;_LL171:
if((_tmp2AD.YY23).tag != 29)goto _LL173;_tmp2AE=(void*)(_tmp2AD.YY23).val;_LL172:
yyzzz=_tmp2AE;goto _LL170;_LL173:;_LL174:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL170:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(void*yy1){union Cyc_YYSTYPE _tmp9E3;
return((_tmp9E3.YY23).val=yy1,(((_tmp9E3.YY23).tag=29,_tmp9E3)));}static char
_tmp2B1[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={
Cyc_Core_Failure,{_tmp2B1,_tmp2B1,_tmp2B1 + 12}};enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(
union Cyc_YYSTYPE yy1){enum Cyc_Absyn_AggrKind yyzzz;{union Cyc_YYSTYPE _tmp2B2=yy1;
enum Cyc_Absyn_AggrKind _tmp2B3;_LL176: if((_tmp2B2.YY24).tag != 30)goto _LL178;
_tmp2B3=(enum Cyc_Absyn_AggrKind)(_tmp2B2.YY24).val;_LL177: yyzzz=_tmp2B3;goto
_LL175;_LL178:;_LL179:(int)_throw((void*)& Cyc_yyfail_YY24);_LL175:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9E4;
return((_tmp9E4.YY24).val=yy1,(((_tmp9E4.YY24).tag=30,_tmp9E4)));}static char
_tmp2B6[8]="tqual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{
_tmp2B6,_tmp2B6,_tmp2B6 + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE _tmp2B7=yy1;struct Cyc_Absyn_Tqual
_tmp2B8;_LL17B: if((_tmp2B7.YY25).tag != 31)goto _LL17D;_tmp2B8=(struct Cyc_Absyn_Tqual)(
_tmp2B7.YY25).val;_LL17C: yyzzz=_tmp2B8;goto _LL17A;_LL17D:;_LL17E:(int)_throw((
void*)& Cyc_yyfail_YY25);_LL17A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual
yy1){union Cyc_YYSTYPE _tmp9E5;return((_tmp9E5.YY25).val=yy1,(((_tmp9E5.YY25).tag=
31,_tmp9E5)));}static char _tmp2BB[23]="list_t<aggrfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp2BB,_tmp2BB,_tmp2BB + 23}};struct Cyc_List_List*
Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2BC=yy1;struct Cyc_List_List*_tmp2BD;_LL180: if((_tmp2BC.YY26).tag != 32)goto
_LL182;_tmp2BD=(struct Cyc_List_List*)(_tmp2BC.YY26).val;_LL181: yyzzz=_tmp2BD;
goto _LL17F;_LL182:;_LL183:(int)_throw((void*)& Cyc_yyfail_YY26);_LL17F:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E6;
return((_tmp9E6.YY26).val=yy1,(((_tmp9E6.YY26).tag=32,_tmp9E6)));}static char
_tmp2C0[34]="list_t<list_t<aggrfield_t,`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2C0,_tmp2C0,_tmp2C0 + 34}};struct Cyc_List_List*
Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2C1=yy1;struct Cyc_List_List*_tmp2C2;_LL185: if((_tmp2C1.YY27).tag != 33)goto
_LL187;_tmp2C2=(struct Cyc_List_List*)(_tmp2C1.YY27).val;_LL186: yyzzz=_tmp2C2;
goto _LL184;_LL187:;_LL188:(int)_throw((void*)& Cyc_yyfail_YY27);_LL184:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E7;
return((_tmp9E7.YY27).val=yy1,(((_tmp9E7.YY27).tag=33,_tmp9E7)));}static char
_tmp2C5[31]="list_t<type_modifier_t<`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2C5,_tmp2C5,_tmp2C5 + 31}};struct Cyc_List_List*
Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2C6=yy1;struct Cyc_List_List*_tmp2C7;_LL18A: if((_tmp2C6.YY28).tag != 34)goto
_LL18C;_tmp2C7=(struct Cyc_List_List*)(_tmp2C6.YY28).val;_LL18B: yyzzz=_tmp2C7;
goto _LL189;_LL18C:;_LL18D:(int)_throw((void*)& Cyc_yyfail_YY28);_LL189:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E8;
return((_tmp9E8.YY28).val=yy1,(((_tmp9E8.YY28).tag=34,_tmp9E8)));}static char
_tmp2CA[13]="declarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={
Cyc_Core_Failure,{_tmp2CA,_tmp2CA,_tmp2CA + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE _tmp2CB=
yy1;struct Cyc_Parse_Declarator*_tmp2CC;_LL18F: if((_tmp2CB.YY29).tag != 35)goto
_LL191;_tmp2CC=(struct Cyc_Parse_Declarator*)(_tmp2CB.YY29).val;_LL190: yyzzz=
_tmp2CC;goto _LL18E;_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY29);_LL18E:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){union Cyc_YYSTYPE
_tmp9E9;return((_tmp9E9.YY29).val=yy1,(((_tmp9E9.YY29).tag=35,_tmp9E9)));}static
char _tmp2CF[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={
Cyc_Core_Failure,{_tmp2CF,_tmp2CF,_tmp2CF + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{
union Cyc_YYSTYPE _tmp2D0=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp2D1;_LL194:
if((_tmp2D0.YY30).tag != 36)goto _LL196;_tmp2D1=(struct Cyc_Parse_Abstractdeclarator*)(
_tmp2D0.YY30).val;_LL195: yyzzz=_tmp2D1;goto _LL193;_LL196:;_LL197:(int)_throw((
void*)& Cyc_yyfail_YY30);_LL193:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1){union Cyc_YYSTYPE _tmp9EA;return((_tmp9EA.YY30).val=yy1,(((_tmp9EA.YY30).tag=
36,_tmp9EA)));}static char _tmp2D4[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2D4,_tmp2D4,_tmp2D4 + 5}};int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp2D5=yy1;int _tmp2D6;_LL199: if((
_tmp2D5.YY31).tag != 37)goto _LL19B;_tmp2D6=(int)(_tmp2D5.YY31).val;_LL19A: yyzzz=
_tmp2D6;goto _LL198;_LL19B:;_LL19C:(int)_throw((void*)& Cyc_yyfail_YY31);_LL198:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp9EB;return((
_tmp9EB.YY31).val=yy1,(((_tmp9EB.YY31).tag=37,_tmp9EB)));}static char _tmp2D9[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2D9,
_tmp2D9,_tmp2D9 + 8}};enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;{union Cyc_YYSTYPE _tmp2DA=yy1;enum Cyc_Absyn_Scope
_tmp2DB;_LL19E: if((_tmp2DA.YY32).tag != 38)goto _LL1A0;_tmp2DB=(enum Cyc_Absyn_Scope)(
_tmp2DA.YY32).val;_LL19F: yyzzz=_tmp2DB;goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((
void*)& Cyc_yyfail_YY32);_LL19D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope
yy1){union Cyc_YYSTYPE _tmp9EC;return((_tmp9EC.YY32).val=yy1,(((_tmp9EC.YY32).tag=
38,_tmp9EC)));}static char _tmp2DE[16]="datatypefield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2DE,_tmp2DE,_tmp2DE + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union
Cyc_YYSTYPE _tmp2DF=yy1;struct Cyc_Absyn_Datatypefield*_tmp2E0;_LL1A3: if((_tmp2DF.YY33).tag
!= 39)goto _LL1A5;_tmp2E0=(struct Cyc_Absyn_Datatypefield*)(_tmp2DF.YY33).val;
_LL1A4: yyzzz=_tmp2E0;goto _LL1A2;_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL1A2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){
union Cyc_YYSTYPE _tmp9ED;return((_tmp9ED.YY33).val=yy1,(((_tmp9ED.YY33).tag=39,
_tmp9ED)));}static char _tmp2E3[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2E3,_tmp2E3,_tmp2E3 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2E4=yy1;struct Cyc_List_List*_tmp2E5;_LL1A8: if((_tmp2E4.YY34).tag != 40)goto
_LL1AA;_tmp2E5=(struct Cyc_List_List*)(_tmp2E4.YY34).val;_LL1A9: yyzzz=_tmp2E5;
goto _LL1A7;_LL1AA:;_LL1AB:(int)_throw((void*)& Cyc_yyfail_YY34);_LL1A7:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EE;
return((_tmp9EE.YY34).val=yy1,(((_tmp9EE.YY34).tag=40,_tmp9EE)));}static char
_tmp2E8[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2E8,_tmp2E8,
_tmp2E8 + 55}};struct _tuple22*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple22*
yyzzz;{union Cyc_YYSTYPE _tmp2E9=yy1;struct _tuple22*_tmp2EA;_LL1AD: if((_tmp2E9.YY35).tag
!= 41)goto _LL1AF;_tmp2EA=(struct _tuple22*)(_tmp2E9.YY35).val;_LL1AE: yyzzz=
_tmp2EA;goto _LL1AC;_LL1AF:;_LL1B0:(int)_throw((void*)& Cyc_yyfail_YY35);_LL1AC:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY35(struct _tuple22*yy1){union Cyc_YYSTYPE
_tmp9EF;return((_tmp9EF.YY35).val=yy1,(((_tmp9EF.YY35).tag=41,_tmp9EF)));}static
char _tmp2ED[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={
Cyc_Core_Failure,{_tmp2ED,_tmp2ED,_tmp2ED + 17}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2EE=yy1;
struct Cyc_List_List*_tmp2EF;_LL1B2: if((_tmp2EE.YY36).tag != 42)goto _LL1B4;_tmp2EF=(
struct Cyc_List_List*)(_tmp2EE.YY36).val;_LL1B3: yyzzz=_tmp2EF;goto _LL1B1;_LL1B4:;
_LL1B5:(int)_throw((void*)& Cyc_yyfail_YY36);_LL1B1:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F0;return((_tmp9F0.YY36).val=
yy1,(((_tmp9F0.YY36).tag=42,_tmp9F0)));}static char _tmp2F2[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2F2,
_tmp2F2,_tmp2F2 + 37}};struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){struct
_tuple8*yyzzz;{union Cyc_YYSTYPE _tmp2F3=yy1;struct _tuple8*_tmp2F4;_LL1B7: if((
_tmp2F3.YY37).tag != 43)goto _LL1B9;_tmp2F4=(struct _tuple8*)(_tmp2F3.YY37).val;
_LL1B8: yyzzz=_tmp2F4;goto _LL1B6;_LL1B9:;_LL1BA:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL1B6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE
_tmp9F1;return((_tmp9F1.YY37).val=yy1,(((_tmp9F1.YY37).tag=43,_tmp9F1)));}static
char _tmp2F7[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2F7,_tmp2F7,
_tmp2F7 + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2F8=yy1;struct Cyc_List_List*_tmp2F9;_LL1BC: if((
_tmp2F8.YY38).tag != 44)goto _LL1BE;_tmp2F9=(struct Cyc_List_List*)(_tmp2F8.YY38).val;
_LL1BD: yyzzz=_tmp2F9;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1BB:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9F2;return((_tmp9F2.YY38).val=yy1,(((_tmp9F2.YY38).tag=44,_tmp9F2)));}static
char _tmp2FC[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2FC,
_tmp2FC,_tmp2FC + 127}};struct _tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct
_tuple23*yyzzz;{union Cyc_YYSTYPE _tmp2FD=yy1;struct _tuple23*_tmp2FE;_LL1C1: if((
_tmp2FD.YY39).tag != 45)goto _LL1C3;_tmp2FE=(struct _tuple23*)(_tmp2FD.YY39).val;
_LL1C2: yyzzz=_tmp2FE;goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY39);
_LL1C0:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY39(struct _tuple23*yy1){union Cyc_YYSTYPE
_tmp9F3;return((_tmp9F3.YY39).val=yy1,(((_tmp9F3.YY39).tag=45,_tmp9F3)));}static
char _tmp301[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={
Cyc_Core_Failure,{_tmp301,_tmp301,_tmp301 + 18}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp302=yy1;
struct Cyc_List_List*_tmp303;_LL1C6: if((_tmp302.YY40).tag != 46)goto _LL1C8;_tmp303=(
struct Cyc_List_List*)(_tmp302.YY40).val;_LL1C7: yyzzz=_tmp303;goto _LL1C5;_LL1C8:;
_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY40);_LL1C5:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F4;return((_tmp9F4.YY40).val=
yy1,(((_tmp9F4.YY40).tag=46,_tmp9F4)));}static char _tmp306[24]="list_t<designator_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp306,
_tmp306,_tmp306 + 24}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp307=yy1;struct Cyc_List_List*
_tmp308;_LL1CB: if((_tmp307.YY41).tag != 47)goto _LL1CD;_tmp308=(struct Cyc_List_List*)(
_tmp307.YY41).val;_LL1CC: yyzzz=_tmp308;goto _LL1CA;_LL1CD:;_LL1CE:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1CA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9F5;return((_tmp9F5.YY41).val=yy1,(((_tmp9F5.YY41).tag=
47,_tmp9F5)));}static char _tmp30B[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp30B,_tmp30B,_tmp30B + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp30C=yy1;void*_tmp30D;_LL1D0:
if((_tmp30C.YY42).tag != 48)goto _LL1D2;_tmp30D=(void*)(_tmp30C.YY42).val;_LL1D1:
yyzzz=_tmp30D;goto _LL1CF;_LL1D2:;_LL1D3:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1CF:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp9F6;
return((_tmp9F6.YY42).val=yy1,(((_tmp9F6.YY42).tag=48,_tmp9F6)));}static char
_tmp310[7]="kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{
_tmp310,_tmp310,_tmp310 + 7}};struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Kind*yyzzz;{union Cyc_YYSTYPE _tmp311=yy1;struct Cyc_Absyn_Kind*
_tmp312;_LL1D5: if((_tmp311.YY43).tag != 49)goto _LL1D7;_tmp312=(struct Cyc_Absyn_Kind*)(
_tmp311.YY43).val;_LL1D6: yyzzz=_tmp312;goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((
void*)& Cyc_yyfail_YY43);_LL1D4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*
yy1){union Cyc_YYSTYPE _tmp9F7;return((_tmp9F7.YY43).val=yy1,(((_tmp9F7.YY43).tag=
49,_tmp9F7)));}static char _tmp315[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp315,_tmp315,_tmp315 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp316=yy1;void*_tmp317;_LL1DA:
if((_tmp316.YY44).tag != 50)goto _LL1DC;_tmp317=(void*)(_tmp316.YY44).val;_LL1DB:
yyzzz=_tmp317;goto _LL1D9;_LL1DC:;_LL1DD:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1D9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmp9F8;
return((_tmp9F8.YY44).val=yy1,(((_tmp9F8.YY44).tag=50,_tmp9F8)));}static char
_tmp31A[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY45={
Cyc_Core_Failure,{_tmp31A,_tmp31A,_tmp31A + 23}};struct Cyc_List_List*Cyc_yyget_YY45(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp31B=yy1;
struct Cyc_List_List*_tmp31C;_LL1DF: if((_tmp31B.YY45).tag != 51)goto _LL1E1;_tmp31C=(
struct Cyc_List_List*)(_tmp31B.YY45).val;_LL1E0: yyzzz=_tmp31C;goto _LL1DE;_LL1E1:;
_LL1E2:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1DE:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F9;return((_tmp9F9.YY45).val=
yy1,(((_tmp9F9.YY45).tag=51,_tmp9F9)));}static char _tmp31F[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp31F,
_tmp31F,_tmp31F + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union
Cyc_YYSTYPE _tmp320=yy1;void*_tmp321;_LL1E4: if((_tmp320.YY46).tag != 52)goto _LL1E6;
_tmp321=(void*)(_tmp320.YY46).val;_LL1E5: yyzzz=_tmp321;goto _LL1E3;_LL1E6:;_LL1E7:(
int)_throw((void*)& Cyc_yyfail_YY46);_LL1E3:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1){union Cyc_YYSTYPE _tmp9FA;return((_tmp9FA.YY46).val=yy1,(((_tmp9FA.YY46).tag=
52,_tmp9FA)));}static char _tmp324[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp324,_tmp324,_tmp324 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE
_tmp325=yy1;struct Cyc_Absyn_Enumfield*_tmp326;_LL1E9: if((_tmp325.YY47).tag != 53)
goto _LL1EB;_tmp326=(struct Cyc_Absyn_Enumfield*)(_tmp325.YY47).val;_LL1EA: yyzzz=
_tmp326;goto _LL1E8;_LL1EB:;_LL1EC:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1E8:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE
_tmp9FB;return((_tmp9FB.YY47).val=yy1,(((_tmp9FB.YY47).tag=53,_tmp9FB)));}static
char _tmp329[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={
Cyc_Core_Failure,{_tmp329,_tmp329,_tmp329 + 23}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp32A=yy1;
struct Cyc_List_List*_tmp32B;_LL1EE: if((_tmp32A.YY48).tag != 54)goto _LL1F0;_tmp32B=(
struct Cyc_List_List*)(_tmp32A.YY48).val;_LL1EF: yyzzz=_tmp32B;goto _LL1ED;_LL1F0:;
_LL1F1:(int)_throw((void*)& Cyc_yyfail_YY48);_LL1ED:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FC;return((_tmp9FC.YY48).val=
yy1,(((_tmp9FC.YY48).tag=54,_tmp9FC)));}static char _tmp32E[17]="opt_t<type_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp32E,
_tmp32E,_tmp32E + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp32F=yy1;struct Cyc_Core_Opt*_tmp330;
_LL1F3: if((_tmp32F.YY49).tag != 55)goto _LL1F5;_tmp330=(struct Cyc_Core_Opt*)(
_tmp32F.YY49).val;_LL1F4: yyzzz=_tmp330;goto _LL1F2;_LL1F5:;_LL1F6:(int)_throw((
void*)& Cyc_yyfail_YY49);_LL1F2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*
yy1){union Cyc_YYSTYPE _tmp9FD;return((_tmp9FD.YY49).val=yy1,(((_tmp9FD.YY49).tag=
55,_tmp9FD)));}static char _tmp333[31]="list_t<$(type_t,type_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp333,_tmp333,
_tmp333 + 31}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp334=yy1;struct Cyc_List_List*_tmp335;_LL1F8: if((
_tmp334.YY50).tag != 56)goto _LL1FA;_tmp335=(struct Cyc_List_List*)(_tmp334.YY50).val;
_LL1F9: yyzzz=_tmp335;goto _LL1F7;_LL1FA:;_LL1FB:(int)_throw((void*)& Cyc_yyfail_YY50);
_LL1F7:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9FE;return((_tmp9FE.YY50).val=yy1,(((_tmp9FE.YY50).tag=56,_tmp9FE)));}static
char _tmp338[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY51={
Cyc_Core_Failure,{_tmp338,_tmp338,_tmp338 + 15}};union Cyc_Absyn_Constraint*Cyc_yyget_YY51(
union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp339=
yy1;union Cyc_Absyn_Constraint*_tmp33A;_LL1FD: if((_tmp339.YY51).tag != 57)goto
_LL1FF;_tmp33A=(union Cyc_Absyn_Constraint*)(_tmp339.YY51).val;_LL1FE: yyzzz=
_tmp33A;goto _LL1FC;_LL1FF:;_LL200:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1FC:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE
_tmp9FF;return((_tmp9FF.YY51).val=yy1,(((_tmp9FF.YY51).tag=57,_tmp9FF)));}static
char _tmp33D[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp33D,_tmp33D,_tmp33D + 45}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp33E=yy1;struct Cyc_List_List*_tmp33F;_LL202: if((_tmp33E.YY52).tag != 58)goto
_LL204;_tmp33F=(struct Cyc_List_List*)(_tmp33E.YY52).val;_LL203: yyzzz=_tmp33F;
goto _LL201;_LL204:;_LL205:(int)_throw((void*)& Cyc_yyfail_YY52);_LL201:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA00;
return((_tmpA00.YY52).val=yy1,(((_tmpA00.YY52).tag=58,_tmpA00)));}static char
_tmp342[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp342,_tmp342,_tmp342 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp343=yy1;void*_tmp344;_LL207: if((_tmp343.YY53).tag
!= 59)goto _LL209;_tmp344=(void*)(_tmp343.YY53).val;_LL208: yyzzz=_tmp344;goto
_LL206;_LL209:;_LL20A:(int)_throw((void*)& Cyc_yyfail_YY53);_LL206:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmpA01;return((_tmpA01.YY53).val=
yy1,(((_tmpA01.YY53).tag=59,_tmpA01)));}static char _tmp347[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp347,
_tmp347,_tmp347 + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp348=yy1;struct Cyc_List_List*
_tmp349;_LL20C: if((_tmp348.YY54).tag != 60)goto _LL20E;_tmp349=(struct Cyc_List_List*)(
_tmp348.YY54).val;_LL20D: yyzzz=_tmp349;goto _LL20B;_LL20E:;_LL20F:(int)_throw((
void*)& Cyc_yyfail_YY54);_LL20B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmpA02;return((_tmpA02.YY54).val=yy1,(((_tmpA02.YY54).tag=
60,_tmpA02)));}static char _tmp34C[21]="$(bool,string_t<`H>)";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp34C,_tmp34C,_tmp34C + 21}};struct
_tuple17 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){struct _tuple17 yyzzz;{union Cyc_YYSTYPE
_tmp34D=yy1;struct _tuple17 _tmp34E;_LL211: if((_tmp34D.Asm_tok).tag != 6)goto _LL213;
_tmp34E=(struct _tuple17)(_tmp34D.Asm_tok).val;_LL212: yyzzz=_tmp34E;goto _LL210;
_LL213:;_LL214:(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL210:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple17 yy1){union Cyc_YYSTYPE _tmpA03;return((
_tmpA03.Asm_tok).val=yy1,(((_tmpA03.Asm_tok).tag=6,_tmpA03)));}struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_Yyltype Cyc_yynewloc(){struct Cyc_Yyltype _tmpA04;return(_tmpA04.timestamp=0,((
_tmpA04.first_line=0,((_tmpA04.first_column=0,((_tmpA04.last_line=0,((_tmpA04.last_column=
0,_tmpA04)))))))));}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[
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
struct _dyneither_ptr f1;void*f2;};static char _tmp519[8]="stdcall";static char
_tmp51A[6]="cdecl";static char _tmp51B[9]="fastcall";static char _tmp51C[9]="noreturn";
static char _tmp51D[6]="const";static char _tmp51E[8]="aligned";static char _tmp51F[7]="packed";
static char _tmp520[7]="shared";static char _tmp521[7]="unused";static char _tmp522[5]="weak";
static char _tmp523[10]="dllimport";static char _tmp524[10]="dllexport";static char
_tmp525[23]="no_instrument_function";static char _tmp526[12]="constructor";static
char _tmp527[11]="destructor";static char _tmp528[22]="no_check_memory_usage";
static char _tmp529[5]="pure";struct _tuple26{void*f1;void*f2;};struct _tuple27{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*
yylex_buf);static void _tmpA0B(unsigned int*_tmpA0A,unsigned int*_tmpA09,union Cyc_YYSTYPE**
_tmpA08){for(*_tmpA0A=0;*_tmpA0A < *_tmpA09;(*_tmpA0A)++){(*_tmpA08)[*_tmpA0A]=
Cyc_yylval;}}static void _tmpA10(unsigned int*_tmpA0F,unsigned int*_tmpA0E,struct
Cyc_Yyltype**_tmpA0D){for(*_tmpA0F=0;*_tmpA0F < *_tmpA0E;(*_tmpA0F)++){(*_tmpA0D)[*
_tmpA0F]=Cyc_yynewloc();}}static void _tmpA17(int*yyssp_offset,struct
_dyneither_ptr*yyss,unsigned int*_tmpA16,unsigned int*_tmpA15,short**_tmpA13){
for(*_tmpA16=0;*_tmpA16 < *_tmpA15;(*_tmpA16)++){(*_tmpA13)[*_tmpA16]=*_tmpA16 <= *
yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*
_tmpA16)): 0;}}static void _tmpA1D(struct _dyneither_ptr*yyvs,int*yyssp_offset,
unsigned int*_tmpA1C,unsigned int*_tmpA1B,union Cyc_YYSTYPE**_tmpA19){for(*
_tmpA1C=0;*_tmpA1C < *_tmpA1B;(*_tmpA1C)++){(*_tmpA19)[*_tmpA1C]=*_tmpA1C <= *
yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union
Cyc_YYSTYPE),(int)*_tmpA1C)): Cyc_yylval;}}static void _tmpA23(int*yyssp_offset,
struct _dyneither_ptr*yyls,unsigned int*_tmpA22,unsigned int*_tmpA21,struct Cyc_Yyltype**
_tmpA1F){for(*_tmpA22=0;*_tmpA22 < *_tmpA21;(*_tmpA22)++){(*_tmpA1F)[*_tmpA22]=*
_tmpA22 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,
sizeof(struct Cyc_Yyltype),(int)*_tmpA22)): Cyc_yynewloc();}}static void _tmpF12(
unsigned int*_tmpF11,unsigned int*_tmpF10,char**_tmpF0E){for(*_tmpF11=0;*_tmpF11
< *_tmpF10;(*_tmpF11)++){(*_tmpF0E)[*_tmpF11]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*
yylex_buf){struct _RegionHandle _tmp471=_new_region("yyregion");struct
_RegionHandle*yyregion=& _tmp471;_push_region(yyregion);{int yystate;int yyn=0;int
yyerrstatus;int yychar1=0;int yyssp_offset;short*_tmpA06;unsigned int _tmpA05;
struct _dyneither_ptr yyss=(_tmpA05=200,((_tmpA06=_region_calloc(yyregion,sizeof(
short),_tmpA05),_tag_dyneither(_tmpA06,sizeof(short),_tmpA05))));int yyvsp_offset;
unsigned int _tmpA0A;unsigned int _tmpA09;union Cyc_YYSTYPE*_tmpA08;unsigned int
_tmpA07;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmpA07=(unsigned int)200,((
_tmpA08=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),
_tmpA07)),((((_tmpA09=_tmpA07,_tmpA0B(& _tmpA0A,& _tmpA09,& _tmpA08))),_tmpA08)))))),
sizeof(union Cyc_YYSTYPE),(unsigned int)200);int yylsp_offset;unsigned int _tmpA0F;
unsigned int _tmpA0E;struct Cyc_Yyltype*_tmpA0D;unsigned int _tmpA0C;struct
_dyneither_ptr yyls=_tag_dyneither(((_tmpA0C=(unsigned int)200,((_tmpA0D=(struct
Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),
_tmpA0C)),((((_tmpA0E=_tmpA0C,_tmpA10(& _tmpA0F,& _tmpA0E,& _tmpA0D))),_tmpA0D)))))),
sizeof(struct Cyc_Yyltype),(unsigned int)200);int yystacksize=200;union Cyc_YYSTYPE
yyval=Cyc_yylval;int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;
yyssp_offset=- 1;yyvsp_offset=0;yylsp_offset=0;yynewstate:*((short*)
_check_dyneither_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 10000){{const char*_tmpA11;Cyc_yyerror(((
_tmpA11="parser stack overflow",_tag_dyneither(_tmpA11,sizeof(char),22))));}(int)
_throw((void*)& Cyc_Yystack_overflow_val);}yystacksize *=2;if(yystacksize > 10000)
yystacksize=10000;{unsigned int _tmpA16;unsigned int _tmpA15;struct _dyneither_ptr
_tmpA14;short*_tmpA13;unsigned int _tmpA12;struct _dyneither_ptr yyss1=(_tmpA12=(
unsigned int)yystacksize,((_tmpA13=(short*)_region_malloc(yyregion,_check_times(
sizeof(short),_tmpA12)),((_tmpA14=_tag_dyneither(_tmpA13,sizeof(short),_tmpA12),((((
_tmpA15=_tmpA12,_tmpA17(& yyssp_offset,& yyss,& _tmpA16,& _tmpA15,& _tmpA13))),
_tmpA14)))))));unsigned int _tmpA1C;unsigned int _tmpA1B;struct _dyneither_ptr
_tmpA1A;union Cyc_YYSTYPE*_tmpA19;unsigned int _tmpA18;struct _dyneither_ptr yyvs1=(
_tmpA18=(unsigned int)yystacksize,((_tmpA19=(union Cyc_YYSTYPE*)_region_malloc(
yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA18)),((_tmpA1A=
_tag_dyneither(_tmpA19,sizeof(union Cyc_YYSTYPE),_tmpA18),((((_tmpA1B=_tmpA18,
_tmpA1D(& yyvs,& yyssp_offset,& _tmpA1C,& _tmpA1B,& _tmpA19))),_tmpA1A)))))));
unsigned int _tmpA22;unsigned int _tmpA21;struct _dyneither_ptr _tmpA20;struct Cyc_Yyltype*
_tmpA1F;unsigned int _tmpA1E;struct _dyneither_ptr yyls1=(_tmpA1E=(unsigned int)
yystacksize,((_tmpA1F=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(
sizeof(struct Cyc_Yyltype),_tmpA1E)),((_tmpA20=_tag_dyneither(_tmpA1F,sizeof(
struct Cyc_Yyltype),_tmpA1E),((((_tmpA21=_tmpA1E,_tmpA23(& yyssp_offset,& yyls,&
_tmpA22,& _tmpA21,& _tmpA1F))),_tmpA20)))))));yyss=yyss1;yyvs=yyvs1;yyls=yyls1;};}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1028,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 371?(int)Cyc_yytranslate[_check_known_subscript_notnull(372,Cyc_yychar)]:
287;}yyn +=yychar1;if((yyn < 0  || yyn > 6244) || Cyc_yycheck[
_check_known_subscript_notnull(6245,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6245,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1027){int
_tmp47F=0;_npop_handler(0);return _tmp47F;}if(Cyc_yychar != 0)Cyc_yychar=- 2;*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++
yyvsp_offset))=Cyc_yylval;*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,
sizeof(struct Cyc_Yyltype),++ yylsp_offset))=Cyc_yylloc;if(yyerrstatus != 0)--
yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1028,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(513,yyn)];if(yylen > 0)yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(
yyvsp_offset + 1)- yylen));switch(yyn){case 1: _LL215: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));Cyc_Parse_parse_result=
Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)));break;case 2: _LL216: {struct Cyc_List_List*x;struct
Cyc_List_List*y;{struct _handler_cons _tmp480;_push_handler(& _tmp480);{int _tmp482=
0;if(setjmp(_tmp480.handler))_tmp482=1;if(!_tmp482){x=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));;
_pop_handler();}else{void*_tmp481=(void*)_exn_thrown;void*_tmp484=_tmp481;_LL219: {
struct Cyc_Core_Failure_struct*_tmp485=(struct Cyc_Core_Failure_struct*)_tmp484;
if(_tmp485->tag != Cyc_Core_Failure)goto _LL21B;}_LL21A: x=0;goto _LL218;_LL21B:;
_LL21C:(void)_throw(_tmp484);_LL218:;}};}{struct _handler_cons _tmp486;
_push_handler(& _tmp486);{int _tmp488=0;if(setjmp(_tmp486.handler))_tmp488=1;if(!
_tmp488){y=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset)));;_pop_handler();}else{void*_tmp487=(
void*)_exn_thrown;void*_tmp48A=_tmp487;_LL21E: {struct Cyc_Core_Failure_struct*
_tmp48B=(struct Cyc_Core_Failure_struct*)_tmp48A;if(_tmp48B->tag != Cyc_Core_Failure)
goto _LL220;}_LL21F: y=0;goto _LL21D;_LL220:;_LL221:(void)_throw(_tmp48A);_LL21D:;}};}
yyval=Cyc_YY18(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(x,y));break;}case 3: _LL217:{struct Cyc_Absyn_Decl*_tmpA2D;
struct Cyc_Absyn_Using_d_struct _tmpA2C;struct Cyc_Absyn_Using_d_struct*_tmpA2B;
struct Cyc_List_List*_tmpA2A;yyval=Cyc_YY18(((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((
_tmpA2A->hd=((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D->r=(void*)((_tmpA2B=
_cycalloc(sizeof(*_tmpA2B)),((_tmpA2B[0]=((_tmpA2C.tag=11,((_tmpA2C.f1=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpA2C.f2=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA2C)))))),
_tmpA2B)))),((_tmpA2D->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpA2D)))))),((_tmpA2A->tl=0,_tmpA2A)))))));}Cyc_Lex_leave_using();
break;case 4: _LL222:{struct Cyc_Absyn_Decl*_tmpA37;struct Cyc_Absyn_Using_d_struct
_tmpA36;struct Cyc_Absyn_Using_d_struct*_tmpA35;struct Cyc_List_List*_tmpA34;yyval=
Cyc_YY18(((_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34->hd=((_tmpA37=_cycalloc(
sizeof(*_tmpA37)),((_tmpA37->r=(void*)((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((
_tmpA35[0]=((_tmpA36.tag=11,((_tmpA36.f1=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),((
_tmpA36.f2=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpA36)))))),_tmpA35)))),((_tmpA37->loc=
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line),
_tmpA37)))))),((_tmpA34->tl=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA34)))))));}
break;case 5: _LL223:{struct Cyc_Absyn_Decl*_tmpA46;struct Cyc_Absyn_Namespace_d_struct
_tmpA45;struct _dyneither_ptr*_tmpA44;struct Cyc_Absyn_Namespace_d_struct*_tmpA43;
struct Cyc_List_List*_tmpA42;yyval=Cyc_YY18(((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((
_tmpA42->hd=((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46->r=(void*)((_tmpA43=
_cycalloc(sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA45.tag=10,((_tmpA45.f1=((_tmpA44=
_cycalloc(sizeof(*_tmpA44)),((_tmpA44[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpA44)))),((_tmpA45.f2=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA45)))))),
_tmpA43)))),((_tmpA46->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpA46)))))),((_tmpA42->tl=0,_tmpA42)))))));}Cyc_Lex_leave_namespace();
break;case 6: _LL224: {struct _dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*
y;{struct _handler_cons _tmp499;_push_handler(& _tmp499);{int _tmp49B=0;if(setjmp(
_tmp499.handler))_tmp49B=1;if(!_tmp49B){nspace=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4)));;
_pop_handler();}else{void*_tmp49A=(void*)_exn_thrown;void*_tmp49D=_tmp49A;_LL227: {
struct Cyc_Core_Failure_struct*_tmp49E=(struct Cyc_Core_Failure_struct*)_tmp49D;
if(_tmp49E->tag != Cyc_Core_Failure)goto _LL229;}_LL228:{const char*_tmpA47;nspace=((
_tmpA47="",_tag_dyneither(_tmpA47,sizeof(char),1)));}goto _LL226;_LL229:;_LL22A:(
void)_throw(_tmp49D);_LL226:;}};}{struct _handler_cons _tmp4A0;_push_handler(&
_tmp4A0);{int _tmp4A2=0;if(setjmp(_tmp4A0.handler))_tmp4A2=1;if(!_tmp4A2){x=Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2)));;_pop_handler();}else{void*_tmp4A1=(void*)_exn_thrown;void*
_tmp4A4=_tmp4A1;_LL22C: {struct Cyc_Core_Failure_struct*_tmp4A5=(struct Cyc_Core_Failure_struct*)
_tmp4A4;if(_tmp4A5->tag != Cyc_Core_Failure)goto _LL22E;}_LL22D: x=0;goto _LL22B;
_LL22E:;_LL22F:(void)_throw(_tmp4A4);_LL22B:;}};}{struct _handler_cons _tmp4A6;
_push_handler(& _tmp4A6);{int _tmp4A8=0;if(setjmp(_tmp4A6.handler))_tmp4A8=1;if(!
_tmp4A8){y=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset)));;_pop_handler();}else{void*_tmp4A7=(
void*)_exn_thrown;void*_tmp4AA=_tmp4A7;_LL231: {struct Cyc_Core_Failure_struct*
_tmp4AB=(struct Cyc_Core_Failure_struct*)_tmp4AA;if(_tmp4AB->tag != Cyc_Core_Failure)
goto _LL233;}_LL232: y=0;goto _LL230;_LL233:;_LL234:(void)_throw(_tmp4AA);_LL230:;}};}{
struct Cyc_Absyn_Decl*_tmpA56;struct Cyc_Absyn_Namespace_d_struct _tmpA55;struct
_dyneither_ptr*_tmpA54;struct Cyc_Absyn_Namespace_d_struct*_tmpA53;struct Cyc_List_List*
_tmpA52;yyval=Cyc_YY18(((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->hd=((
_tmpA56=_cycalloc(sizeof(*_tmpA56)),((_tmpA56->r=(void*)((_tmpA53=_cycalloc(
sizeof(*_tmpA53)),((_tmpA53[0]=((_tmpA55.tag=10,((_tmpA55.f1=((_tmpA54=_cycalloc(
sizeof(*_tmpA54)),((_tmpA54[0]=nspace,_tmpA54)))),((_tmpA55.f2=x,_tmpA55)))))),
_tmpA53)))),((_tmpA56->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),_tmpA56)))))),((_tmpA52->tl=y,_tmpA52)))))));}
break;}case 7: _LL225:{const char*_tmpA57;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))),((_tmpA57="C",_tag_dyneither(_tmpA57,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmpA61;struct Cyc_Absyn_ExternC_d_struct _tmpA60;struct Cyc_Absyn_ExternC_d_struct*
_tmpA5F;struct Cyc_List_List*_tmpA5E;yyval=Cyc_YY18(((_tmpA5E=_cycalloc(sizeof(*
_tmpA5E)),((_tmpA5E->hd=((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61->r=(void*)((
_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((_tmpA5F[0]=((_tmpA60.tag=12,((_tmpA60.f1=
Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpA60)))),_tmpA5F)))),((_tmpA61->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line),_tmpA61)))))),((
_tmpA5E->tl=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA5E)))))));}else{{const char*_tmpA62;
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),((
_tmpA62="C include",_tag_dyneither(_tmpA62,sizeof(char),10))))!= 0){const char*
_tmpA63;Cyc_Parse_err(((_tmpA63="expecting \"C\" or \"C include\"",
_tag_dyneither(_tmpA63,sizeof(char),29))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).last_line));}}{struct Cyc_Absyn_Decl*
_tmpA6D;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA6C;struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpA6B;struct Cyc_List_List*_tmpA6A;yyval=Cyc_YY18(((_tmpA6A=_cycalloc(sizeof(*
_tmpA6A)),((_tmpA6A->hd=((_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D->r=(void*)((
_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B[0]=((_tmpA6C.tag=13,((_tmpA6C.f1=
Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),((_tmpA6C.f2=0,_tmpA6C)))))),_tmpA6B)))),((
_tmpA6D->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),_tmpA6D)))))),((_tmpA6A->tl=Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpA6A)))))));};}}break;case 8: _LL235:{const char*_tmpA6E;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))),((
_tmpA6E="C include",_tag_dyneither(_tmpA6E,sizeof(char),10))))!= 0){const char*
_tmpA6F;Cyc_Parse_err(((_tmpA6F="expecting \"C include\"",_tag_dyneither(_tmpA6F,
sizeof(char),22))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).last_line));}}{struct Cyc_List_List*exs=Cyc_yyget_YY52(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));{struct Cyc_Absyn_Decl*_tmpA79;struct Cyc_Absyn_ExternCinclude_d_struct
_tmpA78;struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA77;struct Cyc_List_List*
_tmpA76;yyval=Cyc_YY18(((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76->hd=((
_tmpA79=_cycalloc(sizeof(*_tmpA79)),((_tmpA79->r=(void*)((_tmpA77=_cycalloc(
sizeof(*_tmpA77)),((_tmpA77[0]=((_tmpA78.tag=13,((_tmpA78.f1=Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpA78.f2=exs,_tmpA78)))))),_tmpA77)))),((_tmpA79->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line),_tmpA79)))))),((
_tmpA76->tl=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA76)))))));}break;};case 9: _LL236:{
struct Cyc_Absyn_Decl*_tmpA7C;struct Cyc_List_List*_tmpA7B;yyval=Cyc_YY18(((
_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B->hd=((_tmpA7C=_cycalloc(sizeof(*
_tmpA7C)),((_tmpA7C->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA7C->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line),_tmpA7C)))))),((
_tmpA7B->tl=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA7B)))))));}break;case 10: _LL237:{
struct Cyc_Absyn_Decl*_tmpA7F;struct Cyc_List_List*_tmpA7E;yyval=Cyc_YY18(((
_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E->hd=((_tmpA7F=_cycalloc(sizeof(*
_tmpA7F)),((_tmpA7F->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA7F->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line),_tmpA7F)))))),((
_tmpA7E->tl=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA7E)))))));}break;case 11: _LL238:
yyval=Cyc_YY18(0);break;case 12: _LL239: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1));
break;case 13: _LL23A: yyval=Cyc_YY52(0);break;case 14: _LL23B:{struct _tuple24*
_tmpA82;struct Cyc_List_List*_tmpA81;yyval=Cyc_YY52(((_tmpA81=_cycalloc(sizeof(*
_tmpA81)),((_tmpA81->hd=((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82->f1=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpA82->f2=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpA82->f3=0,_tmpA82)))))))),((_tmpA81->tl=0,_tmpA81)))))));}
break;case 15: _LL23C:{struct _tuple24*_tmpA85;struct Cyc_List_List*_tmpA84;yyval=
Cyc_YY52(((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->hd=((_tmpA85=_cycalloc(
sizeof(*_tmpA85)),((_tmpA85->f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),((_tmpA85->f2=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpA85->f3=0,_tmpA85)))))))),((_tmpA84->tl=0,_tmpA84)))))));}break;case 16:
_LL23D:{struct _tuple24*_tmpA88;struct Cyc_List_List*_tmpA87;yyval=Cyc_YY52(((
_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87->hd=((_tmpA88=_cycalloc(sizeof(*
_tmpA88)),((_tmpA88->f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line),((_tmpA88->f2=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((
_tmpA88->f3=0,_tmpA88)))))))),((_tmpA87->tl=Cyc_yyget_YY52(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA87)))))));}
break;case 17: _LL23E:{struct Cyc_Absyn_Fn_d_struct*_tmpA8E;struct Cyc_Absyn_Fn_d_struct
_tmpA8D;struct Cyc_List_List*_tmpA8C;yyval=Cyc_YY18(((_tmpA8C=_cycalloc(sizeof(*
_tmpA8C)),((_tmpA8C->hd=Cyc_Absyn_new_decl((void*)((_tmpA8E=_cycalloc(sizeof(*
_tmpA8E)),((_tmpA8E[0]=((_tmpA8D.tag=1,((_tmpA8D.f1=Cyc_yyget_YY17(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpA8D)))),
_tmpA8E)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),((_tmpA8C->tl=0,_tmpA8C)))))));}break;case 18: _LL23F:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 19: _LL240: yyval=Cyc_YY18(0);break;case 20: _LL241: yyval=
Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),0,
Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 21: _LL242:{struct Cyc_Core_Opt*_tmpA8F;
yyval=Cyc_YY17(Cyc_Parse_make_function(((_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((
_tmpA8F->v=Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpA8F)))),Cyc_yyget_YY29(*((union
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
struct Cyc_Core_Opt*_tmpA90;yyval=Cyc_YY17(Cyc_Parse_make_function(((_tmpA90=
_cycalloc(sizeof(*_tmpA90)),((_tmpA90->v=Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpA90)))),Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 24: _LL245:{struct Cyc_Core_Opt*_tmpA91;
yyval=Cyc_YY17(Cyc_Parse_make_function(((_tmpA91=_cycalloc(sizeof(*_tmpA91)),((
_tmpA91->v=Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpA91)))),Cyc_yyget_YY29(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),0,Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 25: _LL246:{
struct Cyc_Core_Opt*_tmpA92;yyval=Cyc_YY17(Cyc_Parse_make_function(((_tmpA92=
_cycalloc(sizeof(*_tmpA92)),((_tmpA92->v=Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpA92)))),Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
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
break;case 28: _LL249:{struct _dyneither_ptr*_tmpA93;Cyc_Lex_enter_namespace(((
_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpA93)))));}yyval=*((union Cyc_YYSTYPE*)
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
struct Cyc_List_List*_tmpA94;yyval=Cyc_YY18(((_tmpA94=_cycalloc(sizeof(*_tmpA94)),((
_tmpA94->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),((_tmpA94->tl=0,_tmpA94)))))));}break;case 33: _LL24E: {
struct Cyc_List_List*_tmp4D5=0;{struct Cyc_List_List*_tmp4D6=Cyc_yyget_YY36(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));for(0;_tmp4D6 != 0;_tmp4D6=_tmp4D6->tl){struct _dyneither_ptr*
_tmp4D7=(struct _dyneither_ptr*)_tmp4D6->hd;struct _tuple1*_tmpA95;struct _tuple1*
qv=(_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95->f1=Cyc_Absyn_Rel_n(0),((
_tmpA95->f2=_tmp4D7,_tmpA95)))));struct Cyc_Absyn_Vardecl*_tmp4D8=Cyc_Absyn_new_vardecl(
qv,Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*_tmpA96;_tmp4D5=((_tmpA96=
_cycalloc(sizeof(*_tmpA96)),((_tmpA96->hd=_tmp4D8,((_tmpA96->tl=_tmp4D5,_tmpA96))))));}}
_tmp4D5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4D5);{struct Cyc_List_List*_tmpA97;yyval=Cyc_YY18(((_tmpA97=_cycalloc(sizeof(*
_tmpA97)),((_tmpA97->hd=Cyc_Absyn_letv_decl(_tmp4D5,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpA97->tl=0,
_tmpA97)))))));}break;}case 34: _LL24F:{const char*_tmpA98;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((
_tmpA98="`H",_tag_dyneither(_tmpA98,sizeof(char),3))))== 0){const char*_tmpA9B;
void*_tmpA9A;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA9A=0,Cyc_aprintf(((
_tmpA9B="bad occurrence of heap region",_tag_dyneither(_tmpA9B,sizeof(char),30))),
_tag_dyneither(_tmpA9A,sizeof(void*),0)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));}}{const char*
_tmpA9C;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((
_tmpA9C="`U",_tag_dyneither(_tmpA9C,sizeof(char),3))))== 0){const char*_tmpA9F;
void*_tmpA9E;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA9E=0,Cyc_aprintf(((
_tmpA9F="bad occurrence of unique region",_tag_dyneither(_tmpA9F,sizeof(char),32))),
_tag_dyneither(_tmpA9E,sizeof(void*),0)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));}}{struct
_dyneither_ptr*_tmpAA2;struct Cyc_Absyn_Tvar*_tmpAA1;struct Cyc_Absyn_Tvar*tv=(
_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->name=((_tmpAA2=_cycalloc(sizeof(*
_tmpAA2)),((_tmpAA2[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpAA2)))),((_tmpAA1->identity=- 1,((_tmpAA1->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),
_tmpAA1)))))));struct Cyc_Absyn_VarType_struct _tmpAA5;struct Cyc_Absyn_VarType_struct*
_tmpAA4;void*t=(void*)((_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4[0]=((
_tmpAA5.tag=2,((_tmpAA5.f1=tv,_tmpAA5)))),_tmpAA4))));struct Cyc_Absyn_RgnHandleType_struct
_tmpAAB;struct Cyc_Absyn_RgnHandleType_struct*_tmpAAA;struct _dyneither_ptr*
_tmpAA8;struct _tuple1*_tmpAA7;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->f1=Cyc_Absyn_Loc_n,((_tmpAA7->f2=((
_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpAA8)))),_tmpAA7)))))),(void*)((_tmpAAA=_cycalloc(sizeof(*
_tmpAAA)),((_tmpAAA[0]=((_tmpAAB.tag=16,((_tmpAAB.f1=(void*)t,_tmpAAB)))),
_tmpAAA)))),0);{struct Cyc_List_List*_tmpAAC;yyval=Cyc_YY18(((_tmpAAC=_cycalloc(
sizeof(*_tmpAAC)),((_tmpAAC->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 6))),0,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),((_tmpAAC->tl=0,_tmpAAC)))))));}break;};case 35:
_LL250:{const char*_tmpAAD;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpAAD="H",_tag_dyneither(_tmpAAD,sizeof(char),2))))== 0){
const char*_tmpAAE;Cyc_Parse_err(((_tmpAAE="bad occurrence of heap region `H",
_tag_dyneither(_tmpAAE,sizeof(char),33))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line));}}{const char*
_tmpAAF;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((
_tmpAAF="U",_tag_dyneither(_tmpAAF,sizeof(char),2))))== 0){const char*_tmpAB0;Cyc_Parse_err(((
_tmpAB0="bad occurrence of unique region `U",_tag_dyneither(_tmpAB0,sizeof(char),
35))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line));}}if(Cyc_yyget_YY31(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))) && 
Cyc_yyget_YY4(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1)))!= 0){const char*_tmpAB1;Cyc_Parse_err(((_tmpAB1="open regions cannot be @resetable",
_tag_dyneither(_tmpAB1,sizeof(char),34))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));}{struct
_dyneither_ptr*_tmpABE;const char*_tmpABD;void*_tmpABC[1];struct Cyc_String_pa_struct
_tmpABB;struct Cyc_Absyn_Tvar*_tmpABA;struct Cyc_Absyn_Tvar*tv=(_tmpABA=_cycalloc(
sizeof(*_tmpABA)),((_tmpABA->name=((_tmpABE=_cycalloc(sizeof(*_tmpABE)),((
_tmpABE[0]=(struct _dyneither_ptr)((_tmpABB.tag=0,((_tmpABB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),((
_tmpABC[0]=& _tmpABB,Cyc_aprintf(((_tmpABD="`%s",_tag_dyneither(_tmpABD,sizeof(
char),4))),_tag_dyneither(_tmpABC,sizeof(void*),1)))))))),_tmpABE)))),((_tmpABA->identity=
- 1,((_tmpABA->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpABA)))))));
struct Cyc_Absyn_VarType_struct _tmpAC1;struct Cyc_Absyn_VarType_struct*_tmpAC0;
void*t=(void*)((_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0[0]=((_tmpAC1.tag=2,((
_tmpAC1.f1=tv,_tmpAC1)))),_tmpAC0))));struct Cyc_Absyn_RgnHandleType_struct
_tmpAC7;struct Cyc_Absyn_RgnHandleType_struct*_tmpAC6;struct _dyneither_ptr*
_tmpAC4;struct _tuple1*_tmpAC3;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3->f1=Cyc_Absyn_Loc_n,((_tmpAC3->f2=((
_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),_tmpAC4)))),_tmpAC3)))))),(void*)((_tmpAC6=_cycalloc(sizeof(*
_tmpAC6)),((_tmpAC6[0]=((_tmpAC7.tag=16,((_tmpAC7.f1=(void*)t,_tmpAC7)))),
_tmpAC6)))),0);{struct Cyc_List_List*_tmpAC8;yyval=Cyc_YY18(((_tmpAC8=_cycalloc(
sizeof(*_tmpAC8)),((_tmpAC8->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))),Cyc_yyget_YY4(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpAC8->tl=0,
_tmpAC8)))))));}break;};case 36: _LL251: {struct Cyc_Absyn_Eq_kb_struct*_tmpAD0;
struct Cyc_Absyn_Eq_kb_struct _tmpACF;struct _dyneither_ptr*_tmpACE;struct Cyc_Absyn_Tvar*
_tmpACD;struct Cyc_Absyn_Tvar*tv=(_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD->name=((
_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 5))),_tmpACE)))),((_tmpACD->identity=- 1,((_tmpACD->kind=(void*)((
_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0[0]=((_tmpACF.tag=0,((_tmpACF.f1=&
Cyc_Tcutil_rk,_tmpACF)))),_tmpAD0)))),_tmpACD)))))));struct _dyneither_ptr*
_tmpAD3;struct _tuple1*_tmpAD2;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2->f1=Cyc_Absyn_Loc_n,((_tmpAD2->f2=((
_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),_tmpAD3)))),_tmpAD2)))))),(void*)& Cyc_Absyn_VoidType_val,0);{
struct Cyc_List_List*_tmpAD4;yyval=Cyc_YY18(((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((
_tmpAD4->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),tv,
vd,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((
_tmpAD4->tl=0,_tmpAD4)))))));}break;}case 37: _LL252: yyval=Cyc_YY31(0);break;case
38: _LL253: yyval=Cyc_YY31(1);break;case 39: _LL254: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 40: _LL255: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))));break;case 41: _LL256:{struct Cyc_Core_Opt*_tmpAD7;struct Cyc_Parse_Declaration_spec*
_tmpAD6;yyval=Cyc_YY19(((_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6->sc=((
_tmpAD7=_cycalloc_atomic(sizeof(*_tmpAD7)),((_tmpAD7->v=(void*)Cyc_yyget_YY22(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpAD7)))),((_tmpAD6->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpAD6->type_specs=
0,((_tmpAD6->is_inline=0,((_tmpAD6->attributes=0,_tmpAD6)))))))))))));}break;
case 42: _LL257: if((Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->sc != 0){const char*_tmpADA;void*
_tmpAD9;(_tmpAD9=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpADA="Only one storage class is allowed in a declaration",
_tag_dyneither(_tmpADA,sizeof(char),51))),_tag_dyneither(_tmpAD9,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmpADD;struct Cyc_Parse_Declaration_spec*_tmpADC;yyval=Cyc_YY19(((
_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC->sc=((_tmpADD=_cycalloc_atomic(
sizeof(*_tmpADD)),((_tmpADD->v=(void*)Cyc_yyget_YY22(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpADD)))),((_tmpADC->tq=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tq,((
_tmpADC->type_specs=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->type_specs,((
_tmpADC->is_inline=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->is_inline,((
_tmpADC->attributes=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->attributes,
_tmpADC)))))))))))));}break;case 43: _LL258:{struct Cyc_List_List*_tmpAE0;struct Cyc_Parse_Declaration_spec*
_tmpADF;yyval=Cyc_YY19(((_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF->sc=0,((
_tmpADF->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),((_tmpADF->type_specs=((_tmpAE0=_cycalloc(sizeof(*
_tmpAE0)),((_tmpAE0->hd=Cyc_yyget_YY23(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpAE0->tl=0,_tmpAE0)))))),((_tmpADF->is_inline=0,((_tmpADF->attributes=0,
_tmpADF)))))))))))));}break;case 44: _LL259:{struct Cyc_List_List*_tmpAE3;struct Cyc_Parse_Declaration_spec*
_tmpAE2;yyval=Cyc_YY19(((_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2->sc=(Cyc_yyget_YY19(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->sc,((_tmpAE2->tq=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tq,((
_tmpAE2->type_specs=((_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3->hd=Cyc_yyget_YY23(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpAE3->tl=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->type_specs,
_tmpAE3)))))),((_tmpAE2->is_inline=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->is_inline,((
_tmpAE2->attributes=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->attributes,
_tmpAE2)))))))))))));}break;case 45: _LL25A:{struct Cyc_Parse_Declaration_spec*
_tmpAE4;yyval=Cyc_YY19(((_tmpAE4=_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4->sc=0,((
_tmpAE4->tq=Cyc_yyget_YY25(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((_tmpAE4->type_specs=0,((_tmpAE4->is_inline=
0,((_tmpAE4->attributes=0,_tmpAE4)))))))))))));}break;case 46: _LL25B:{struct Cyc_Parse_Declaration_spec*
_tmpAE5;yyval=Cyc_YY19(((_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5->sc=(Cyc_yyget_YY19(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->sc,((_tmpAE5->tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tq),((
_tmpAE5->type_specs=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->type_specs,((
_tmpAE5->is_inline=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->is_inline,((
_tmpAE5->attributes=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->attributes,
_tmpAE5)))))))))))));}break;case 47: _LL25C:{struct Cyc_Parse_Declaration_spec*
_tmpAE6;yyval=Cyc_YY19(((_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6->sc=0,((
_tmpAE6->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),((_tmpAE6->type_specs=0,((_tmpAE6->is_inline=1,((
_tmpAE6->attributes=0,_tmpAE6)))))))))))));}break;case 48: _LL25D:{struct Cyc_Parse_Declaration_spec*
_tmpAE7;yyval=Cyc_YY19(((_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7->sc=(Cyc_yyget_YY19(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->sc,((_tmpAE7->tq=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tq,((
_tmpAE7->type_specs=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->type_specs,((
_tmpAE7->is_inline=1,((_tmpAE7->attributes=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->attributes,
_tmpAE7)))))))))))));}break;case 49: _LL25E:{struct Cyc_Parse_Declaration_spec*
_tmpAE8;yyval=Cyc_YY19(((_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8->sc=0,((
_tmpAE8->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),((_tmpAE8->type_specs=0,((_tmpAE8->is_inline=0,((
_tmpAE8->attributes=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpAE8)))))))))))));}
break;case 50: _LL25F:{struct Cyc_Parse_Declaration_spec*_tmpAE9;yyval=Cyc_YY19(((
_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9->sc=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->sc,((
_tmpAE9->tq=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tq,((_tmpAE9->type_specs=(Cyc_yyget_YY19(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->type_specs,((_tmpAE9->is_inline=(Cyc_yyget_YY19(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->is_inline,((
_tmpAE9->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(Cyc_yyget_YY19(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->attributes),_tmpAE9)))))))))))));}break;case 51: _LL260: yyval=
Cyc_YY22(Cyc_Parse_Auto_sc);break;case 52: _LL261: yyval=Cyc_YY22(Cyc_Parse_Register_sc);
break;case 53: _LL262: yyval=Cyc_YY22(Cyc_Parse_Static_sc);break;case 54: _LL263:
yyval=Cyc_YY22(Cyc_Parse_Extern_sc);break;case 55: _LL264:{const char*_tmpAEA;if(
Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpAEA="C",_tag_dyneither(_tmpAEA,sizeof(char),2))))!= 0){const char*_tmpAEB;Cyc_Parse_err(((
_tmpAEB="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAEB,sizeof(
char),37))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));}}yyval=Cyc_YY22(Cyc_Parse_ExternC_sc);break;case 56:
_LL265: yyval=Cyc_YY22(Cyc_Parse_Typedef_sc);break;case 57: _LL266: yyval=Cyc_YY22(
Cyc_Parse_Abstract_sc);break;case 58: _LL267: yyval=Cyc_YY45(0);break;case 59: _LL268:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 60: _LL269: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2));
break;case 61: _LL26A:{struct Cyc_List_List*_tmpAEC;yyval=Cyc_YY45(((_tmpAEC=
_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC->hd=Cyc_yyget_YY46(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpAEC->tl=0,_tmpAEC)))))));}break;case 62: _LL26B:{struct Cyc_List_List*_tmpAED;
yyval=Cyc_YY45(((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED->hd=Cyc_yyget_YY46(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpAED->tl=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpAED)))))));}
break;case 63: _LL26C: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={6,- 1};
static struct _tuple25 att_map[17]={{{_tmp519,_tmp519,_tmp519 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{
_tmp51A,_tmp51A,_tmp51A + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp51B,_tmp51B,
_tmp51B + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp51C,_tmp51C,_tmp51C + 9},(
void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp51D,_tmp51D,_tmp51D + 6},(void*)& Cyc_Absyn_Const_att_val},{{
_tmp51E,_tmp51E,_tmp51E + 8},(void*)& att_aligned},{{_tmp51F,_tmp51F,_tmp51F + 7},(
void*)& Cyc_Absyn_Packed_att_val},{{_tmp520,_tmp520,_tmp520 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{
_tmp521,_tmp521,_tmp521 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp522,_tmp522,
_tmp522 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp523,_tmp523,_tmp523 + 10},(void*)&
Cyc_Absyn_Dllimport_att_val},{{_tmp524,_tmp524,_tmp524 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{
_tmp525,_tmp525,_tmp525 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{
_tmp526,_tmp526,_tmp526 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp527,
_tmp527,_tmp527 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp528,_tmp528,
_tmp528 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp529,_tmp529,
_tmp529 + 5},(void*)& Cyc_Absyn_Pure_att_val}};struct _dyneither_ptr _tmp517=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)));if((((_get_dyneither_size(_tmp517,sizeof(char))> 4  && *((const
char*)_check_dyneither_subscript(_tmp517,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp517,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp517,sizeof(char),(int)(_get_dyneither_size(
_tmp517,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp517,sizeof(char),(int)(_get_dyneither_size(_tmp517,sizeof(char))- 3)))== '_')
_tmp517=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp517,2,
_get_dyneither_size(_tmp517,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp517,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){{const char*_tmpAEE;Cyc_Parse_err(((
_tmpAEE="unrecognized attribute",_tag_dyneither(_tmpAEE,sizeof(char),23))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));}yyval=Cyc_YY46((void*)&
Cyc_Absyn_Cdecl_att_val);}break;};}case 64: _LL26D: yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
break;case 65: _LL26E: {struct _dyneither_ptr _tmp52B=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3)));int _tmp52D;struct _tuple6 _tmp52C=Cyc_yyget_Int_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));
_tmp52D=_tmp52C.f2;{void*a;{const char*_tmpAF0;const char*_tmpAEF;if(Cyc_zstrcmp((
struct _dyneither_ptr)_tmp52B,((_tmpAF0="regparm",_tag_dyneither(_tmpAF0,sizeof(
char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAEF="__regparm__",
_tag_dyneither(_tmpAEF,sizeof(char),12))))== 0){if(_tmp52D < 0  || _tmp52D > 3){
const char*_tmpAF1;Cyc_Parse_err(((_tmpAF1="regparm requires value between 0 and 3",
_tag_dyneither(_tmpAF1,sizeof(char),39))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));}{struct Cyc_Absyn_Regparm_att_struct
_tmpAF4;struct Cyc_Absyn_Regparm_att_struct*_tmpAF3;a=(void*)((_tmpAF3=
_cycalloc_atomic(sizeof(*_tmpAF3)),((_tmpAF3[0]=((_tmpAF4.tag=0,((_tmpAF4.f1=
_tmp52D,_tmpAF4)))),_tmpAF3))));};}else{const char*_tmpAF6;const char*_tmpAF5;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAF6="aligned",_tag_dyneither(
_tmpAF6,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((
_tmpAF5="__aligned__",_tag_dyneither(_tmpAF5,sizeof(char),12))))== 0){if(_tmp52D
< 0){const char*_tmpAF7;Cyc_Parse_err(((_tmpAF7="aligned requires positive power of two",
_tag_dyneither(_tmpAF7,sizeof(char),39))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));}{unsigned int j=(
unsigned int)_tmp52D;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){const char*
_tmpAF8;Cyc_Parse_err(((_tmpAF8="aligned requires positive power of two",
_tag_dyneither(_tmpAF8,sizeof(char),39))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmpAFB;struct Cyc_Absyn_Aligned_att_struct*_tmpAFA;a=(void*)((_tmpAFA=
_cycalloc_atomic(sizeof(*_tmpAFA)),((_tmpAFA[0]=((_tmpAFB.tag=6,((_tmpAFB.f1=
_tmp52D,_tmpAFB)))),_tmpAFA))));};};}else{const char*_tmpAFD;const char*_tmpAFC;
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAFD="initializes",
_tag_dyneither(_tmpAFD,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp52B,((_tmpAFC="__initializes__",_tag_dyneither(_tmpAFC,
sizeof(char),16))))== 0){struct Cyc_Absyn_Initializes_att_struct _tmpB00;struct Cyc_Absyn_Initializes_att_struct*
_tmpAFF;a=(void*)((_tmpAFF=_cycalloc_atomic(sizeof(*_tmpAFF)),((_tmpAFF[0]=((
_tmpB00.tag=20,((_tmpB00.f1=_tmp52D,_tmpB00)))),_tmpAFF))));}else{const char*
_tmpB02;const char*_tmpB01;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB02="noliveunique",
_tag_dyneither(_tmpB02,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp52B,((_tmpB01="__noliveunique__",_tag_dyneither(_tmpB01,
sizeof(char),17))))== 0){struct Cyc_Absyn_Noliveunique_att_struct _tmpB05;struct
Cyc_Absyn_Noliveunique_att_struct*_tmpB04;a=(void*)((_tmpB04=_cycalloc_atomic(
sizeof(*_tmpB04)),((_tmpB04[0]=((_tmpB05.tag=21,((_tmpB05.f1=_tmp52D,_tmpB05)))),
_tmpB04))));}else{{const char*_tmpB06;Cyc_Parse_err(((_tmpB06="unrecognized attribute",
_tag_dyneither(_tmpB06,sizeof(char),23))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));}a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}
yyval=Cyc_YY46(a);break;};}case 66: _LL26F: {struct _dyneither_ptr _tmp542=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3)));struct _dyneither_ptr _tmp543=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));void*
a;{const char*_tmpB08;const char*_tmpB07;if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp542,((_tmpB08="section",_tag_dyneither(_tmpB08,sizeof(char),8))))== 0  || Cyc_zstrcmp((
struct _dyneither_ptr)_tmp542,((_tmpB07="__section__",_tag_dyneither(_tmpB07,
sizeof(char),12))))== 0){struct Cyc_Absyn_Section_att_struct _tmpB0B;struct Cyc_Absyn_Section_att_struct*
_tmpB0A;a=(void*)((_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A[0]=((_tmpB0B.tag=
8,((_tmpB0B.f1=_tmp543,_tmpB0B)))),_tmpB0A))));}else{{const char*_tmpB0C;Cyc_Parse_err(((
_tmpB0C="unrecognized attribute",_tag_dyneither(_tmpB0C,sizeof(char),23))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));}a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
yyval=Cyc_YY46(a);break;}case 67: _LL270: {struct _dyneither_ptr _tmp549=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3)));struct _dyneither_ptr _tmp54A=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));void*
a;{const char*_tmpB0D;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp549,((_tmpB0D="__mode__",
_tag_dyneither(_tmpB0D,sizeof(char),9))))== 0){struct Cyc_Absyn_Mode_att_struct
_tmpB10;struct Cyc_Absyn_Mode_att_struct*_tmpB0F;a=(void*)((_tmpB0F=_cycalloc(
sizeof(*_tmpB0F)),((_tmpB0F[0]=((_tmpB10.tag=23,((_tmpB10.f1=_tmp54A,_tmpB10)))),
_tmpB0F))));}else{{const char*_tmpB11;Cyc_Parse_err(((_tmpB11="unrecognized attribute",
_tag_dyneither(_tmpB11,sizeof(char),23))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));}a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
yyval=Cyc_YY46(a);break;}case 68: _LL271: {struct _dyneither_ptr _tmp54F=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 7)));struct _dyneither_ptr _tmp550=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5)));int
_tmp552;struct _tuple6 _tmp551=Cyc_yyget_Int_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3)));
_tmp552=_tmp551.f2;{int _tmp554;struct _tuple6 _tmp553=Cyc_yyget_Int_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));_tmp554=_tmp553.f2;{void*a=(void*)& Cyc_Absyn_Cdecl_att_val;{
const char*_tmpB13;const char*_tmpB12;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54F,((
_tmpB13="format",_tag_dyneither(_tmpB13,sizeof(char),7))))== 0  || Cyc_zstrcmp((
struct _dyneither_ptr)_tmp54F,((_tmpB12="__format__",_tag_dyneither(_tmpB12,
sizeof(char),11))))== 0){const char*_tmpB14;if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp550,((_tmpB14="printf",_tag_dyneither(_tmpB14,sizeof(char),7))))== 0){struct
Cyc_Absyn_Format_att_struct _tmpB17;struct Cyc_Absyn_Format_att_struct*_tmpB16;a=(
void*)((_tmpB16=_cycalloc_atomic(sizeof(*_tmpB16)),((_tmpB16[0]=((_tmpB17.tag=19,((
_tmpB17.f1=Cyc_Absyn_Printf_ft,((_tmpB17.f2=_tmp552,((_tmpB17.f3=_tmp554,_tmpB17)))))))),
_tmpB16))));}else{const char*_tmpB18;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp550,((
_tmpB18="scanf",_tag_dyneither(_tmpB18,sizeof(char),6))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpB1B;struct Cyc_Absyn_Format_att_struct*_tmpB1A;a=(void*)((_tmpB1A=
_cycalloc_atomic(sizeof(*_tmpB1A)),((_tmpB1A[0]=((_tmpB1B.tag=19,((_tmpB1B.f1=
Cyc_Absyn_Scanf_ft,((_tmpB1B.f2=_tmp552,((_tmpB1B.f3=_tmp554,_tmpB1B)))))))),
_tmpB1A))));}else{const char*_tmpB1C;Cyc_Parse_err(((_tmpB1C="unrecognized format type",
_tag_dyneither(_tmpB1C,sizeof(char),25))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).last_line));}}}else{const char*
_tmpB1D;Cyc_Parse_err(((_tmpB1D="unrecognized attribute",_tag_dyneither(_tmpB1D,
sizeof(char),23))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));}}yyval=Cyc_YY46(a);break;};};}case 69: _LL272: yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 70: _LL273:{struct Cyc_Absyn_TypedefType_struct _tmpB20;
struct Cyc_Absyn_TypedefType_struct*_tmpB1F;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F[0]=((_tmpB20.tag=18,((
_tmpB20.f1=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((_tmpB20.f2=Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB20.f3=0,((_tmpB20.f4=0,_tmpB20)))))))))),_tmpB1F)))),Cyc_Position_segment_of_abs((*((
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
_tmpB23;struct Cyc_Parse_Short_spec_struct*_tmpB22;yyval=Cyc_YY23((void*)((
_tmpB22=_cycalloc(sizeof(*_tmpB22)),((_tmpB22[0]=((_tmpB23.tag=2,((_tmpB23.f1=
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),
_tmpB23)))),_tmpB22)))));}break;case 76: _LL279: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 77: _LL27A:{struct Cyc_Parse_Long_spec_struct
_tmpB26;struct Cyc_Parse_Long_spec_struct*_tmpB25;yyval=Cyc_YY23((void*)((_tmpB25=
_cycalloc(sizeof(*_tmpB25)),((_tmpB25[0]=((_tmpB26.tag=3,((_tmpB26.f1=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpB26)))),_tmpB25)))));}
break;case 78: _LL27B: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 79: _LL27C:
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 80: _LL27D:{
struct Cyc_Parse_Signed_spec_struct _tmpB29;struct Cyc_Parse_Signed_spec_struct*
_tmpB28;yyval=Cyc_YY23((void*)((_tmpB28=_cycalloc(sizeof(*_tmpB28)),((_tmpB28[0]=((
_tmpB29.tag=0,((_tmpB29.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpB29)))),_tmpB28)))));}break;case 81: _LL27E:{struct
Cyc_Parse_Unsigned_spec_struct _tmpB2C;struct Cyc_Parse_Unsigned_spec_struct*
_tmpB2B;yyval=Cyc_YY23((void*)((_tmpB2B=_cycalloc(sizeof(*_tmpB2B)),((_tmpB2B[0]=((
_tmpB2C.tag=1,((_tmpB2C.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpB2C)))),_tmpB2B)))));}break;case 82: _LL27F: yyval=*((
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
_tmpB2F;struct Cyc_Absyn_TupleType_struct*_tmpB2E;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E[0]=((_tmpB2F.tag=11,((
_tmpB2F.f1=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))))),_tmpB2F)))),_tmpB2E)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 87: _LL284:{
struct Cyc_Absyn_RgnHandleType_struct _tmpB32;struct Cyc_Absyn_RgnHandleType_struct*
_tmpB31;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB31=_cycalloc(sizeof(*
_tmpB31)),((_tmpB31[0]=((_tmpB32.tag=16,((_tmpB32.f1=(void*)Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpB32)))),_tmpB31)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 88: _LL285:{
struct Cyc_Absyn_RgnHandleType_struct _tmpB35;struct Cyc_Absyn_RgnHandleType_struct*
_tmpB34;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB34=_cycalloc(sizeof(*
_tmpB34)),((_tmpB34[0]=((_tmpB35.tag=16,((_tmpB35.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0),_tmpB35)))),_tmpB34)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 89: _LL286: {
void*_tmp56F=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0);{struct
Cyc_Absyn_DynRgnType_struct _tmpB38;struct Cyc_Absyn_DynRgnType_struct*_tmpB37;
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB37=_cycalloc(sizeof(*_tmpB37)),((
_tmpB37[0]=((_tmpB38.tag=17,((_tmpB38.f1=(void*)Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpB38.f2=(void*)_tmp56F,_tmpB38)))))),_tmpB37)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 90:
_LL287:{struct Cyc_Absyn_DynRgnType_struct _tmpB3B;struct Cyc_Absyn_DynRgnType_struct*
_tmpB3A;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB3A=_cycalloc(sizeof(*
_tmpB3A)),((_tmpB3A[0]=((_tmpB3B.tag=17,((_tmpB3B.f1=(void*)Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpB3B.f2=(void*)Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpB3B)))))),_tmpB3A)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 91: _LL288:{struct Cyc_Absyn_TagType_struct
_tmpB3E;struct Cyc_Absyn_TagType_struct*_tmpB3D;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB3D=_cycalloc(sizeof(*_tmpB3D)),((_tmpB3D[0]=((_tmpB3E.tag=20,((
_tmpB3E.f1=(void*)Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),_tmpB3E)))),_tmpB3D)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 92: _LL289:{
struct Cyc_Absyn_TagType_struct _tmpB41;struct Cyc_Absyn_TagType_struct*_tmpB40;
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB40=_cycalloc(sizeof(*_tmpB40)),((
_tmpB40[0]=((_tmpB41.tag=20,((_tmpB41.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_iko,0),_tmpB41)))),_tmpB40)))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 93: _LL28A:{
struct Cyc_Absyn_ValueofType_struct _tmpB44;struct Cyc_Absyn_ValueofType_struct*
_tmpB43;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB43=_cycalloc(sizeof(*
_tmpB43)),((_tmpB43[0]=((_tmpB44.tag=19,((_tmpB44.f1=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpB44)))),_tmpB43)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
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
_tmpB45;yyval=Cyc_YY25(((_tmpB45.print_const=1,((_tmpB45.q_volatile=0,((_tmpB45.q_restrict=
0,((_tmpB45.real_const=1,((_tmpB45.loc=loc,_tmpB45)))))))))));}break;}case 96:
_LL28D:{struct Cyc_Absyn_Tqual _tmpB46;yyval=Cyc_YY25(((_tmpB46.print_const=0,((
_tmpB46.q_volatile=1,((_tmpB46.q_restrict=0,((_tmpB46.real_const=0,((_tmpB46.loc=
0,_tmpB46)))))))))));}break;case 97: _LL28E:{struct Cyc_Absyn_Tqual _tmpB47;yyval=
Cyc_YY25(((_tmpB47.print_const=0,((_tmpB47.q_volatile=0,((_tmpB47.q_restrict=1,((
_tmpB47.real_const=0,((_tmpB47.loc=0,_tmpB47)))))))))));}break;case 98: _LL28F: {
struct Cyc_Absyn_Enum_td_struct*_tmpB56;struct Cyc_Absyn_Enumdecl*_tmpB55;struct
Cyc_Core_Opt*_tmpB54;struct Cyc_Absyn_Enum_td_struct _tmpB53;struct Cyc_Absyn_TypeDecl*
_tmpB52;struct Cyc_Absyn_TypeDecl*_tmp57D=(_tmpB52=_cycalloc(sizeof(*_tmpB52)),((
_tmpB52->r=(void*)((_tmpB56=_cycalloc(sizeof(*_tmpB56)),((_tmpB56[0]=((_tmpB53.tag=
1,((_tmpB53.f1=((_tmpB55=_cycalloc(sizeof(*_tmpB55)),((_tmpB55->sc=Cyc_Absyn_Public,((
_tmpB55->name=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((
_tmpB55->fields=((_tmpB54=_cycalloc(sizeof(*_tmpB54)),((_tmpB54->v=Cyc_yyget_YY48(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpB54)))),_tmpB55)))))))),_tmpB53)))),_tmpB56)))),((_tmpB52->loc=
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),
_tmpB52)))));{struct Cyc_Absyn_TypeDeclType_struct _tmpB59;struct Cyc_Absyn_TypeDeclType_struct*
_tmpB58;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB58=_cycalloc(sizeof(*
_tmpB58)),((_tmpB58[0]=((_tmpB59.tag=27,((_tmpB59.f1=_tmp57D,((_tmpB59.f2=0,
_tmpB59)))))),_tmpB58)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;}case 99: _LL290:{struct Cyc_Absyn_EnumType_struct
_tmpB5C;struct Cyc_Absyn_EnumType_struct*_tmpB5B;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((_tmpB5B[0]=((_tmpB5C.tag=14,((
_tmpB5C.f1=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((_tmpB5C.f2=0,_tmpB5C)))))),
_tmpB5B)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 100: _LL291:{struct Cyc_Absyn_AnonEnumType_struct
_tmpB5F;struct Cyc_Absyn_AnonEnumType_struct*_tmpB5E;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((_tmpB5E[0]=((_tmpB5F.tag=15,((
_tmpB5F.f1=Cyc_yyget_YY48(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),_tmpB5F)))),_tmpB5E)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 101:
_LL292:{struct Cyc_Absyn_Enumfield*_tmpB60;yyval=Cyc_YY47(((_tmpB60=_cycalloc(
sizeof(*_tmpB60)),((_tmpB60->name=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB60->tag=0,((_tmpB60->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpB60)))))))));}break;case 102: _LL293:{struct Cyc_Absyn_Enumfield*
_tmpB61;yyval=Cyc_YY47(((_tmpB61=_cycalloc(sizeof(*_tmpB61)),((_tmpB61->name=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpB61->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB61->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpB61)))))))));}break;case 103: _LL294:{struct Cyc_List_List*
_tmpB62;yyval=Cyc_YY48(((_tmpB62=_cycalloc(sizeof(*_tmpB62)),((_tmpB62->hd=Cyc_yyget_YY47(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB62->tl=0,_tmpB62)))))));}break;case 104: _LL295:{struct Cyc_List_List*
_tmpB63;yyval=Cyc_YY48(((_tmpB63=_cycalloc(sizeof(*_tmpB63)),((_tmpB63->hd=Cyc_yyget_YY47(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpB63->tl=0,_tmpB63)))))));}break;case 105: _LL296:{struct
Cyc_List_List*_tmpB64;yyval=Cyc_YY48(((_tmpB64=_cycalloc(sizeof(*_tmpB64)),((
_tmpB64->hd=Cyc_yyget_YY47(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((_tmpB64->tl=Cyc_yyget_YY48(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpB64)))))));}break;case 106: _LL297:{struct Cyc_Absyn_AnonAggrType_struct
_tmpB67;struct Cyc_Absyn_AnonAggrType_struct*_tmpB66;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB66=_cycalloc(sizeof(*_tmpB66)),((_tmpB66[0]=((_tmpB67.tag=13,((
_tmpB67.f1=Cyc_yyget_YY24(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((_tmpB67.f2=Cyc_yyget_YY26(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpB67)))))),_tmpB66)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 107:
_LL298: {struct Cyc_List_List*_tmp590=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).last_line),Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))));
struct Cyc_List_List*_tmp591=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line),Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))));
struct Cyc_Absyn_TypeDecl*_tmp592=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7))),Cyc_Absyn_Public,
Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6))),_tmp590,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp591,Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY26(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),1),0,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));{struct Cyc_Absyn_TypeDeclType_struct _tmpB6A;struct
Cyc_Absyn_TypeDeclType_struct*_tmpB69;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((
_tmpB69=_cycalloc(sizeof(*_tmpB69)),((_tmpB69[0]=((_tmpB6A.tag=27,((_tmpB6A.f1=
_tmp592,((_tmpB6A.f2=0,_tmpB6A)))))),_tmpB69)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 8))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 108:
_LL299: {struct Cyc_List_List*_tmp595=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).last_line),Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))));
struct Cyc_List_List*_tmp596=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line),Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))));
struct Cyc_Absyn_TypeDecl*_tmp597=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7))),Cyc_Absyn_Public,
Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6))),_tmp595,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp596,Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY26(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),0),0,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));{struct Cyc_Absyn_TypeDeclType_struct _tmpB6D;struct
Cyc_Absyn_TypeDeclType_struct*_tmpB6C;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((
_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C[0]=((_tmpB6D.tag=27,((_tmpB6D.f1=
_tmp597,((_tmpB6D.f2=0,_tmpB6D)))))),_tmpB6C)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 109:
_LL29A:{struct Cyc_Absyn_AggrType_struct _tmpB77;struct Cyc_Core_Opt*_tmpB76;struct
Cyc_Absyn_AggrInfo _tmpB75;struct Cyc_Absyn_AggrType_struct*_tmpB74;yyval=Cyc_YY23(
Cyc_Parse_type_spec((void*)((_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74[0]=((
_tmpB77.tag=12,((_tmpB77.f1=((_tmpB75.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpB76=_cycalloc_atomic(sizeof(*_tmpB76)),((_tmpB76->v=(void*)1,_tmpB76))))),((
_tmpB75.targs=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpB75)))),_tmpB77)))),_tmpB74)))),
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}
break;case 110: _LL29B:{struct Cyc_Absyn_AggrType_struct _tmpB7D;struct Cyc_Absyn_AggrInfo
_tmpB7C;struct Cyc_Absyn_AggrType_struct*_tmpB7B;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB7B=_cycalloc(sizeof(*_tmpB7B)),((_tmpB7B[0]=((_tmpB7D.tag=12,((
_tmpB7D.f1=((_tmpB7C.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),0),((_tmpB7C.targs=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpB7C)))),
_tmpB7D)))),_tmpB7B)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 111: _LL29C: yyval=Cyc_YY40(0);break;case
112: _LL29D: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1)))));break;case 113: _LL29E: yyval=Cyc_YY24(Cyc_Absyn_StructA);
break;case 114: _LL29F: yyval=Cyc_YY24(Cyc_Absyn_UnionA);break;case 115: _LL2A0: yyval=
Cyc_YY26(0);break;case 116: _LL2A1: {struct Cyc_List_List*_tmp5A1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY27(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))));
struct Cyc_List_List*_tmp5A2=Cyc_Parse_get_argrfield_tags(_tmp5A1);if(_tmp5A2 != 0)((
void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,
_tmp5A2,_tmp5A1);yyval=Cyc_YY26(_tmp5A1);break;}case 117: _LL2A2:{struct Cyc_List_List*
_tmpB7E;yyval=Cyc_YY27(((_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E->hd=Cyc_yyget_YY26(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB7E->tl=0,_tmpB7E)))))));}break;case 118: _LL2A3:{struct Cyc_List_List*
_tmpB7F;yyval=Cyc_YY27(((_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F->hd=Cyc_yyget_YY26(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB7F->tl=Cyc_yyget_YY27(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpB7F)))))));}break;case 119: _LL2A4: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))));
break;case 120: _LL2A5:{struct Cyc_List_List*_tmpB80;yyval=Cyc_YY21(((_tmpB80=
_cycalloc(sizeof(*_tmpB80)),((_tmpB80->hd=Cyc_yyget_YY20(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB80->tl=0,_tmpB80)))))));}break;case 121: _LL2A6:{struct Cyc_List_List*_tmpB81;
yyval=Cyc_YY21(((_tmpB81=_cycalloc(sizeof(*_tmpB81)),((_tmpB81->hd=Cyc_yyget_YY20(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB81->tl=Cyc_yyget_YY21(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpB81)))))));}break;case 122: _LL2A7:{struct _tuple21*_tmpB82;yyval=Cyc_YY20(((
_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82->f1=Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB82->f2=0,_tmpB82)))))));}break;case 123: _LL2A8:{struct _tuple21*_tmpB83;yyval=
Cyc_YY20(((_tmpB83=_cycalloc(sizeof(*_tmpB83)),((_tmpB83->f1=Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpB83->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpB83)))))));}
break;case 124: _LL2A9: {struct _RegionHandle _tmp5A9=_new_region("temp");struct
_RegionHandle*temp=& _tmp5A9;_push_region(temp);{struct _tuple22 _tmp5AB;struct Cyc_Absyn_Tqual
_tmp5AC;struct Cyc_List_List*_tmp5AD;struct Cyc_List_List*_tmp5AE;struct _tuple22*
_tmp5AA=Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)));_tmp5AB=*_tmp5AA;_tmp5AC=_tmp5AB.f1;
_tmp5AD=_tmp5AB.f2;_tmp5AE=_tmp5AB.f3;if(_tmp5AC.loc == 0)_tmp5AC.loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line);{struct Cyc_List_List*
_tmp5B0;struct Cyc_List_List*_tmp5B1;struct _tuple0 _tmp5AF=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
temp,temp,Cyc_yyget_YY21(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))));_tmp5B0=_tmp5AF.f1;_tmp5B1=_tmp5AF.f2;{
void*_tmp5B2=Cyc_Parse_speclist2typ(_tmp5AD,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line));struct Cyc_List_List*
_tmp5B3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp5AC,_tmp5B2,_tmp5B0,_tmp5AE),_tmp5B1);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,struct _tuple13*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line),
_tmp5B3));};};}_npop_handler(0);break;;_pop_region(temp);}case 125: _LL2AA:{struct
Cyc_List_List*_tmpB86;struct _tuple22*_tmpB85;yyval=Cyc_YY35(((_tmpB85=_cycalloc(
sizeof(*_tmpB85)),((_tmpB85->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpB85->f2=((
_tmpB86=_cycalloc(sizeof(*_tmpB86)),((_tmpB86->hd=Cyc_yyget_YY23(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB86->tl=0,_tmpB86)))))),((_tmpB85->f3=0,_tmpB85)))))))));}break;case 126:
_LL2AB:{struct Cyc_List_List*_tmpB89;struct _tuple22*_tmpB88;yyval=Cyc_YY35(((
_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->f1=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f1,((
_tmpB88->f2=((_tmpB89=_cycalloc(sizeof(*_tmpB89)),((_tmpB89->hd=Cyc_yyget_YY23(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpB89->tl=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f2,
_tmpB89)))))),((_tmpB88->f3=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f3,
_tmpB88)))))))));}break;case 127: _LL2AC:{struct _tuple22*_tmpB8A;yyval=Cyc_YY35(((
_tmpB8A=_cycalloc(sizeof(*_tmpB8A)),((_tmpB8A->f1=Cyc_yyget_YY25(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB8A->f2=0,((_tmpB8A->f3=0,_tmpB8A)))))))));}break;case 128: _LL2AD:{struct
_tuple22*_tmpB8B;yyval=Cyc_YY35(((_tmpB8B=_cycalloc(sizeof(*_tmpB8B)),((_tmpB8B->f1=
Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(*
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)))).f1),((_tmpB8B->f2=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f2,((
_tmpB8B->f3=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f3,_tmpB8B)))))))));}break;case
129: _LL2AE:{struct _tuple22*_tmpB8C;yyval=Cyc_YY35(((_tmpB8C=_cycalloc(sizeof(*
_tmpB8C)),((_tmpB8C->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpB8C->f2=0,((
_tmpB8C->f3=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpB8C)))))))));}break;case 130: _LL2AF:{
struct _tuple22*_tmpB8D;yyval=Cyc_YY35(((_tmpB8D=_cycalloc(sizeof(*_tmpB8D)),((
_tmpB8D->f1=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f1,((_tmpB8D->f2=(*Cyc_yyget_YY35(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))).f2,((_tmpB8D->f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(*
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)))).f3),_tmpB8D)))))))));}break;case 131: _LL2B0:{struct
Cyc_List_List*_tmpB90;struct _tuple22*_tmpB8F;yyval=Cyc_YY35(((_tmpB8F=_cycalloc(
sizeof(*_tmpB8F)),((_tmpB8F->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpB8F->f2=((
_tmpB90=_cycalloc(sizeof(*_tmpB90)),((_tmpB90->hd=Cyc_yyget_YY23(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB90->tl=0,_tmpB90)))))),((_tmpB8F->f3=0,_tmpB8F)))))))));}break;case 132:
_LL2B1:{struct Cyc_List_List*_tmpB93;struct _tuple22*_tmpB92;yyval=Cyc_YY35(((
_tmpB92=_cycalloc(sizeof(*_tmpB92)),((_tmpB92->f1=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f1,((
_tmpB92->f2=((_tmpB93=_cycalloc(sizeof(*_tmpB93)),((_tmpB93->hd=Cyc_yyget_YY23(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpB93->tl=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f2,
_tmpB93)))))),((_tmpB92->f3=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f3,
_tmpB92)))))))));}break;case 133: _LL2B2:{struct _tuple22*_tmpB94;yyval=Cyc_YY35(((
_tmpB94=_cycalloc(sizeof(*_tmpB94)),((_tmpB94->f1=Cyc_yyget_YY25(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB94->f2=0,((_tmpB94->f3=0,_tmpB94)))))))));}break;case 134: _LL2B3:{struct
_tuple22*_tmpB95;yyval=Cyc_YY35(((_tmpB95=_cycalloc(sizeof(*_tmpB95)),((_tmpB95->f1=
Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(*
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)))).f1),((_tmpB95->f2=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f2,((
_tmpB95->f3=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f3,_tmpB95)))))))));}break;case
135: _LL2B4:{struct _tuple22*_tmpB96;yyval=Cyc_YY35(((_tmpB96=_cycalloc(sizeof(*
_tmpB96)),((_tmpB96->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)),((_tmpB96->f2=0,((
_tmpB96->f3=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpB96)))))))));}break;case 136: _LL2B5:{
struct _tuple22*_tmpB97;yyval=Cyc_YY35(((_tmpB97=_cycalloc(sizeof(*_tmpB97)),((
_tmpB97->f1=(*Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f1,((_tmpB97->f2=(*Cyc_yyget_YY35(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))).f2,((_tmpB97->f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(*
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)))).f3),_tmpB97)))))))));}break;case 137: _LL2B6: yyval=
Cyc_YY21(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))));break;case 138: _LL2B7:{struct Cyc_List_List*_tmpB98;yyval=Cyc_YY21(((
_tmpB98=_cycalloc(sizeof(*_tmpB98)),((_tmpB98->hd=Cyc_yyget_YY20(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB98->tl=0,_tmpB98)))))));}break;case 139: _LL2B8:{struct Cyc_List_List*_tmpB99;
yyval=Cyc_YY21(((_tmpB99=_cycalloc(sizeof(*_tmpB99)),((_tmpB99->hd=Cyc_yyget_YY20(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpB99->tl=Cyc_yyget_YY21(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpB99)))))));}break;case 140: _LL2B9:{struct _tuple21*_tmpB9A;yyval=Cyc_YY20(((
_tmpB9A=_cycalloc(sizeof(*_tmpB9A)),((_tmpB9A->f1=Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpB9A->f2=0,_tmpB9A)))))));}break;case 141: _LL2BA:{struct Cyc_Parse_Declarator*
_tmpBA0;struct _tuple1*_tmpB9F;struct _tuple21*_tmpB9E;yyval=Cyc_YY20(((_tmpB9E=
_cycalloc(sizeof(*_tmpB9E)),((_tmpB9E->f1=((_tmpBA0=_cycalloc(sizeof(*_tmpBA0)),((
_tmpBA0->id=((_tmpB9F=_cycalloc(sizeof(*_tmpB9F)),((_tmpB9F->f1=Cyc_Absyn_Rel_n(
0),((_tmpB9F->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",
sizeof(char),1),_tmpB9F)))))),((_tmpBA0->tms=0,_tmpBA0)))))),((_tmpB9E->f2=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpB9E)))))));}
break;case 142: _LL2BB:{struct Cyc_Parse_Declarator*_tmpBA6;struct _tuple1*_tmpBA5;
struct _tuple21*_tmpBA4;yyval=Cyc_YY20(((_tmpBA4=_cycalloc(sizeof(*_tmpBA4)),((
_tmpBA4->f1=((_tmpBA6=_cycalloc(sizeof(*_tmpBA6)),((_tmpBA6->id=((_tmpBA5=
_cycalloc(sizeof(*_tmpBA5)),((_tmpBA5->f1=Cyc_Absyn_Rel_n(0),((_tmpBA5->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),
_tmpBA5)))))),((_tmpBA6->tms=0,_tmpBA6)))))),((_tmpBA4->f2=0,_tmpBA4)))))));}
break;case 143: _LL2BC:{struct _tuple21*_tmpBA7;yyval=Cyc_YY20(((_tmpBA7=_cycalloc(
sizeof(*_tmpBA7)),((_tmpBA7->f1=Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((
_tmpBA7->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpBA7)))))));}
break;case 144: _LL2BD: {int _tmp5D0=Cyc_yyget_YY31(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5)));
struct Cyc_List_List*_tmp5D1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line),Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))));
struct Cyc_Core_Opt*_tmpBA8;struct Cyc_Absyn_TypeDecl*_tmp5D2=Cyc_Absyn_datatype_tdecl(
Cyc_Absyn_Public,Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),
_tmp5D1,((_tmpBA8=_cycalloc(sizeof(*_tmpBA8)),((_tmpBA8->v=Cyc_yyget_YY34(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpBA8)))),_tmp5D0,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));{struct Cyc_Absyn_TypeDeclType_struct _tmpBAB;struct
Cyc_Absyn_TypeDeclType_struct*_tmpBAA;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((
_tmpBAA=_cycalloc(sizeof(*_tmpBAA)),((_tmpBAA[0]=((_tmpBAB.tag=27,((_tmpBAB.f1=
_tmp5D2,((_tmpBAB.f2=0,_tmpBAB)))))),_tmpBAA)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 145:
_LL2BE: {int _tmp5D6=Cyc_yyget_YY31(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)));{
struct Cyc_Absyn_DatatypeType_struct _tmpBB5;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpBB4;struct Cyc_Absyn_DatatypeInfo _tmpBB3;struct Cyc_Absyn_DatatypeType_struct*
_tmpBB2;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpBB2=_cycalloc(sizeof(*
_tmpBB2)),((_tmpBB2[0]=((_tmpBB5.tag=3,((_tmpBB5.f1=((_tmpBB3.datatype_info=Cyc_Absyn_UnknownDatatype(((
_tmpBB4.name=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpBB4.is_extensible=_tmp5D6,_tmpBB4))))),((_tmpBB3.targs=Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpBB3)))),_tmpBB5)))),_tmpBB2)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 146:
_LL2BF: {int _tmp5DB=Cyc_yyget_YY31(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4)));{
struct Cyc_Absyn_DatatypeFieldType_struct _tmpBBF;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpBBE;struct Cyc_Absyn_DatatypeFieldInfo _tmpBBD;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpBBC;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpBBC=_cycalloc(sizeof(*
_tmpBBC)),((_tmpBBC[0]=((_tmpBBF.tag=4,((_tmpBBF.f1=((_tmpBBD.field_info=Cyc_Absyn_UnknownDatatypefield(((
_tmpBBE.datatype_name=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((
_tmpBBE.field_name=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpBBE.is_extensible=_tmp5DB,_tmpBBE))))))),((_tmpBBD.targs=Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpBBD)))),_tmpBBF)))),_tmpBBC)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 147:
_LL2C0: yyval=Cyc_YY31(0);break;case 148: _LL2C1: yyval=Cyc_YY31(1);break;case 149:
_LL2C2:{struct Cyc_List_List*_tmpBC0;yyval=Cyc_YY34(((_tmpBC0=_cycalloc(sizeof(*
_tmpBC0)),((_tmpBC0->hd=Cyc_yyget_YY33(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpBC0->tl=0,_tmpBC0)))))));}break;case 150: _LL2C3:{struct Cyc_List_List*_tmpBC1;
yyval=Cyc_YY34(((_tmpBC1=_cycalloc(sizeof(*_tmpBC1)),((_tmpBC1->hd=Cyc_yyget_YY33(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpBC1->tl=0,_tmpBC1)))))));}break;case 151: _LL2C4:{struct
Cyc_List_List*_tmpBC2;yyval=Cyc_YY34(((_tmpBC2=_cycalloc(sizeof(*_tmpBC2)),((
_tmpBC2->hd=Cyc_yyget_YY33(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((_tmpBC2->tl=Cyc_yyget_YY34(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpBC2)))))));}break;case 152: _LL2C5:{struct Cyc_List_List*
_tmpBC3;yyval=Cyc_YY34(((_tmpBC3=_cycalloc(sizeof(*_tmpBC3)),((_tmpBC3->hd=Cyc_yyget_YY33(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpBC3->tl=Cyc_yyget_YY34(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpBC3)))))));}
break;case 153: _LL2C6: yyval=Cyc_YY32(Cyc_Absyn_Public);break;case 154: _LL2C7: yyval=
Cyc_YY32(Cyc_Absyn_Extern);break;case 155: _LL2C8: yyval=Cyc_YY32(Cyc_Absyn_Static);
break;case 156: _LL2C9:{struct Cyc_Absyn_Datatypefield*_tmpBC4;yyval=Cyc_YY33(((
_tmpBC4=_cycalloc(sizeof(*_tmpBC4)),((_tmpBC4->name=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpBC4->typs=0,((_tmpBC4->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpBC4->sc=Cyc_yyget_YY32(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpBC4)))))))))));}break;case 157: _LL2CA: {struct Cyc_List_List*
_tmp5E5=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)))));{struct Cyc_Absyn_Datatypefield*
_tmpBC5;yyval=Cyc_YY33(((_tmpBC5=_cycalloc(sizeof(*_tmpBC5)),((_tmpBC5->name=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpBC5->typs=_tmp5E5,((_tmpBC5->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpBC5->sc=Cyc_yyget_YY32(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))),_tmpBC5)))))))))));}break;}case 158: _LL2CB: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 159: _LL2CC:{struct Cyc_Parse_Declarator*_tmpBC6;yyval=Cyc_YY29(((_tmpBC6=
_cycalloc(sizeof(*_tmpBC6)),((_tmpBC6->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->id,((
_tmpBC6->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY28(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->tms),_tmpBC6)))))));}break;case 160: _LL2CD: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 161: _LL2CE:{struct Cyc_Parse_Declarator*_tmpBC7;yyval=Cyc_YY29(((_tmpBC7=
_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->id,((
_tmpBC7->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY28(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->tms),_tmpBC7)))))));}break;case 162: _LL2CF:{struct Cyc_Parse_Declarator*
_tmpBC8;yyval=Cyc_YY29(((_tmpBC8=_cycalloc(sizeof(*_tmpBC8)),((_tmpBC8->id=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpBC8->tms=0,_tmpBC8)))))));}break;case 163: _LL2D0: yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1));break;case 164: _LL2D1: {struct Cyc_Parse_Declarator*_tmp5EA=Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));{struct Cyc_Absyn_Attributes_mod_struct*_tmpBCE;struct Cyc_Absyn_Attributes_mod_struct
_tmpBCD;struct Cyc_List_List*_tmpBCC;_tmp5EA->tms=((_tmpBCC=_cycalloc(sizeof(*
_tmpBCC)),((_tmpBCC->hd=(void*)((_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE[0]=((
_tmpBCD.tag=5,((_tmpBCD.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line),((_tmpBCD.f2=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpBCD)))))),_tmpBCE)))),((_tmpBCC->tl=_tmp5EA->tms,_tmpBCC))))));}yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1));break;}case 165: _LL2D2:{struct Cyc_List_List*_tmpBD8;struct Cyc_Absyn_Carray_mod_struct
_tmpBD7;struct Cyc_Absyn_Carray_mod_struct*_tmpBD6;struct Cyc_Parse_Declarator*
_tmpBD5;yyval=Cyc_YY29(((_tmpBD5=_cycalloc(sizeof(*_tmpBD5)),((_tmpBD5->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))))->id,((_tmpBD5->tms=((_tmpBD8=_cycalloc(sizeof(*_tmpBD8)),((
_tmpBD8->hd=(void*)((_tmpBD6=_cycalloc(sizeof(*_tmpBD6)),((_tmpBD6[0]=((_tmpBD7.tag=
0,((_tmpBD7.f1=Cyc_yyget_YY51(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((_tmpBD7.f2=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpBD7)))))),_tmpBD6)))),((
_tmpBD8->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,_tmpBD8)))))),_tmpBD5)))))));}
break;case 166: _LL2D3:{struct Cyc_List_List*_tmpBE2;struct Cyc_Absyn_ConstArray_mod_struct
_tmpBE1;struct Cyc_Absyn_ConstArray_mod_struct*_tmpBE0;struct Cyc_Parse_Declarator*
_tmpBDF;yyval=Cyc_YY29(((_tmpBDF=_cycalloc(sizeof(*_tmpBDF)),((_tmpBDF->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->id,((_tmpBDF->tms=((_tmpBE2=_cycalloc(sizeof(*_tmpBE2)),((
_tmpBE2->hd=(void*)((_tmpBE0=_cycalloc(sizeof(*_tmpBE0)),((_tmpBE0[0]=((_tmpBE1.tag=
1,((_tmpBE1.f1=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((_tmpBE1.f2=Cyc_yyget_YY51(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpBE1.f3=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpBE1)))))))),_tmpBE0)))),((_tmpBE2->tl=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->tms,_tmpBE2)))))),_tmpBDF)))))));}break;case 167: _LL2D4: {
struct _tuple23 _tmp5F7;struct Cyc_List_List*_tmp5F8;int _tmp5F9;struct Cyc_Absyn_VarargInfo*
_tmp5FA;struct Cyc_Core_Opt*_tmp5FB;struct Cyc_List_List*_tmp5FC;struct _tuple23*
_tmp5F6=Cyc_yyget_YY39(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));_tmp5F7=*_tmp5F6;_tmp5F8=_tmp5F7.f1;
_tmp5F9=_tmp5F7.f2;_tmp5FA=_tmp5F7.f3;_tmp5FB=_tmp5F7.f4;_tmp5FC=_tmp5F7.f5;{
struct Cyc_List_List*_tmpBF7;struct Cyc_Absyn_Function_mod_struct _tmpBF6;struct Cyc_Absyn_WithTypes_struct
_tmpBF5;struct Cyc_Absyn_WithTypes_struct*_tmpBF4;struct Cyc_Absyn_Function_mod_struct*
_tmpBF3;struct Cyc_Parse_Declarator*_tmpBF2;yyval=Cyc_YY29(((_tmpBF2=_cycalloc(
sizeof(*_tmpBF2)),((_tmpBF2->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->id,((
_tmpBF2->tms=((_tmpBF7=_cycalloc(sizeof(*_tmpBF7)),((_tmpBF7->hd=(void*)((
_tmpBF3=_cycalloc(sizeof(*_tmpBF3)),((_tmpBF3[0]=((_tmpBF6.tag=3,((_tmpBF6.f1=(
void*)((void*)((_tmpBF4=_cycalloc(sizeof(*_tmpBF4)),((_tmpBF4[0]=((_tmpBF5.tag=1,((
_tmpBF5.f1=_tmp5F8,((_tmpBF5.f2=_tmp5F9,((_tmpBF5.f3=_tmp5FA,((_tmpBF5.f4=
_tmp5FB,((_tmpBF5.f5=_tmp5FC,_tmpBF5)))))))))))),_tmpBF4))))),_tmpBF6)))),
_tmpBF3)))),((_tmpBF7->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,
_tmpBF7)))))),_tmpBF2)))))));}break;}case 168: _LL2D5:{struct Cyc_List_List*_tmpC0C;
struct Cyc_Absyn_Function_mod_struct _tmpC0B;struct Cyc_Absyn_WithTypes_struct
_tmpC0A;struct Cyc_Absyn_WithTypes_struct*_tmpC09;struct Cyc_Absyn_Function_mod_struct*
_tmpC08;struct Cyc_Parse_Declarator*_tmpC07;yyval=Cyc_YY29(((_tmpC07=_cycalloc(
sizeof(*_tmpC07)),((_tmpC07->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))))->id,((
_tmpC07->tms=((_tmpC0C=_cycalloc(sizeof(*_tmpC0C)),((_tmpC0C->hd=(void*)((
_tmpC08=_cycalloc(sizeof(*_tmpC08)),((_tmpC08[0]=((_tmpC0B.tag=3,((_tmpC0B.f1=(
void*)((void*)((_tmpC09=_cycalloc(sizeof(*_tmpC09)),((_tmpC09[0]=((_tmpC0A.tag=1,((
_tmpC0A.f1=0,((_tmpC0A.f2=0,((_tmpC0A.f3=0,((_tmpC0A.f4=Cyc_yyget_YY49(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpC0A.f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpC0A)))))))))))),_tmpC09))))),_tmpC0B)))),_tmpC08)))),((_tmpC0C->tl=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->tms,_tmpC0C)))))),_tmpC07)))))));}break;case 169: _LL2D6:{
struct Cyc_List_List*_tmpC21;struct Cyc_Absyn_Function_mod_struct _tmpC20;struct Cyc_Absyn_NoTypes_struct
_tmpC1F;struct Cyc_Absyn_NoTypes_struct*_tmpC1E;struct Cyc_Absyn_Function_mod_struct*
_tmpC1D;struct Cyc_Parse_Declarator*_tmpC1C;yyval=Cyc_YY29(((_tmpC1C=_cycalloc(
sizeof(*_tmpC1C)),((_tmpC1C->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->id,((
_tmpC1C->tms=((_tmpC21=_cycalloc(sizeof(*_tmpC21)),((_tmpC21->hd=(void*)((
_tmpC1D=_cycalloc(sizeof(*_tmpC1D)),((_tmpC1D[0]=((_tmpC20.tag=3,((_tmpC20.f1=(
void*)((void*)((_tmpC1E=_cycalloc(sizeof(*_tmpC1E)),((_tmpC1E[0]=((_tmpC1F.tag=0,((
_tmpC1F.f1=Cyc_yyget_YY36(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((_tmpC1F.f2=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpC1F)))))),_tmpC1E))))),
_tmpC20)))),_tmpC1D)))),((_tmpC21->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,
_tmpC21)))))),_tmpC1C)))))));}break;case 170: _LL2D7: {struct Cyc_List_List*_tmp60F=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)))));{struct Cyc_List_List*_tmpC2B;struct Cyc_Absyn_TypeParams_mod_struct
_tmpC2A;struct Cyc_Absyn_TypeParams_mod_struct*_tmpC29;struct Cyc_Parse_Declarator*
_tmpC28;yyval=Cyc_YY29(((_tmpC28=_cycalloc(sizeof(*_tmpC28)),((_tmpC28->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))))->id,((_tmpC28->tms=((_tmpC2B=_cycalloc(sizeof(*_tmpC2B)),((
_tmpC2B->hd=(void*)((_tmpC29=_cycalloc(sizeof(*_tmpC29)),((_tmpC29[0]=((_tmpC2A.tag=
4,((_tmpC2A.f1=_tmp60F,((_tmpC2A.f2=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpC2A.f3=0,_tmpC2A)))))))),_tmpC29)))),((_tmpC2B->tl=(
Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,_tmpC2B)))))),_tmpC28)))))));}break;}case
171: _LL2D8:{struct Cyc_List_List*_tmpC35;struct Cyc_Absyn_Attributes_mod_struct
_tmpC34;struct Cyc_Absyn_Attributes_mod_struct*_tmpC33;struct Cyc_Parse_Declarator*
_tmpC32;yyval=Cyc_YY29(((_tmpC32=_cycalloc(sizeof(*_tmpC32)),((_tmpC32->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))))->id,((_tmpC32->tms=((_tmpC35=_cycalloc(sizeof(*_tmpC35)),((
_tmpC35->hd=(void*)((_tmpC33=_cycalloc(sizeof(*_tmpC33)),((_tmpC33[0]=((_tmpC34.tag=
5,((_tmpC34.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpC34.f2=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpC34)))))),
_tmpC33)))),((_tmpC35->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))))->tms,
_tmpC35)))))),_tmpC32)))))));}break;case 172: _LL2D9:{struct Cyc_Parse_Declarator*
_tmpC36;yyval=Cyc_YY29(((_tmpC36=_cycalloc(sizeof(*_tmpC36)),((_tmpC36->id=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpC36->tms=0,_tmpC36)))))));}break;case 173: _LL2DA:{struct Cyc_Parse_Declarator*
_tmpC37;yyval=Cyc_YY29(((_tmpC37=_cycalloc(sizeof(*_tmpC37)),((_tmpC37->id=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpC37->tms=0,_tmpC37)))))));}break;case 174: _LL2DB: yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1));break;case 175: _LL2DC: {struct Cyc_Parse_Declarator*_tmp61A=Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));{struct Cyc_Absyn_Attributes_mod_struct*_tmpC3D;struct Cyc_Absyn_Attributes_mod_struct
_tmpC3C;struct Cyc_List_List*_tmpC3B;_tmp61A->tms=((_tmpC3B=_cycalloc(sizeof(*
_tmpC3B)),((_tmpC3B->hd=(void*)((_tmpC3D=_cycalloc(sizeof(*_tmpC3D)),((_tmpC3D[0]=((
_tmpC3C.tag=5,((_tmpC3C.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line),((_tmpC3C.f2=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpC3C)))))),_tmpC3D)))),((_tmpC3B->tl=_tmp61A->tms,_tmpC3B))))));}yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1));break;}case 176: _LL2DD:{struct Cyc_List_List*_tmpC47;struct Cyc_Absyn_Carray_mod_struct
_tmpC46;struct Cyc_Absyn_Carray_mod_struct*_tmpC45;struct Cyc_Parse_Declarator*
_tmpC44;yyval=Cyc_YY29(((_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))))->id,((_tmpC44->tms=((_tmpC47=_cycalloc(sizeof(*_tmpC47)),((
_tmpC47->hd=(void*)((_tmpC45=_cycalloc(sizeof(*_tmpC45)),((_tmpC45[0]=((_tmpC46.tag=
0,((_tmpC46.f1=Cyc_yyget_YY51(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((_tmpC46.f2=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpC46)))))),_tmpC45)))),((
_tmpC47->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,_tmpC47)))))),_tmpC44)))))));}
break;case 177: _LL2DE:{struct Cyc_List_List*_tmpC51;struct Cyc_Absyn_ConstArray_mod_struct
_tmpC50;struct Cyc_Absyn_ConstArray_mod_struct*_tmpC4F;struct Cyc_Parse_Declarator*
_tmpC4E;yyval=Cyc_YY29(((_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->id,((_tmpC4E->tms=((_tmpC51=_cycalloc(sizeof(*_tmpC51)),((
_tmpC51->hd=(void*)((_tmpC4F=_cycalloc(sizeof(*_tmpC4F)),((_tmpC4F[0]=((_tmpC50.tag=
1,((_tmpC50.f1=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((_tmpC50.f2=Cyc_yyget_YY51(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpC50.f3=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpC50)))))))),_tmpC4F)))),((_tmpC51->tl=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->tms,_tmpC51)))))),_tmpC4E)))))));}break;case 178: _LL2DF: {
struct _tuple23 _tmp627;struct Cyc_List_List*_tmp628;int _tmp629;struct Cyc_Absyn_VarargInfo*
_tmp62A;struct Cyc_Core_Opt*_tmp62B;struct Cyc_List_List*_tmp62C;struct _tuple23*
_tmp626=Cyc_yyget_YY39(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));_tmp627=*_tmp626;_tmp628=_tmp627.f1;
_tmp629=_tmp627.f2;_tmp62A=_tmp627.f3;_tmp62B=_tmp627.f4;_tmp62C=_tmp627.f5;{
struct Cyc_List_List*_tmpC66;struct Cyc_Absyn_Function_mod_struct _tmpC65;struct Cyc_Absyn_WithTypes_struct
_tmpC64;struct Cyc_Absyn_WithTypes_struct*_tmpC63;struct Cyc_Absyn_Function_mod_struct*
_tmpC62;struct Cyc_Parse_Declarator*_tmpC61;yyval=Cyc_YY29(((_tmpC61=_cycalloc(
sizeof(*_tmpC61)),((_tmpC61->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->id,((
_tmpC61->tms=((_tmpC66=_cycalloc(sizeof(*_tmpC66)),((_tmpC66->hd=(void*)((
_tmpC62=_cycalloc(sizeof(*_tmpC62)),((_tmpC62[0]=((_tmpC65.tag=3,((_tmpC65.f1=(
void*)((void*)((_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63[0]=((_tmpC64.tag=1,((
_tmpC64.f1=_tmp628,((_tmpC64.f2=_tmp629,((_tmpC64.f3=_tmp62A,((_tmpC64.f4=
_tmp62B,((_tmpC64.f5=_tmp62C,_tmpC64)))))))))))),_tmpC63))))),_tmpC65)))),
_tmpC62)))),((_tmpC66->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,
_tmpC66)))))),_tmpC61)))))));}break;}case 179: _LL2E0:{struct Cyc_List_List*_tmpC7B;
struct Cyc_Absyn_Function_mod_struct _tmpC7A;struct Cyc_Absyn_WithTypes_struct
_tmpC79;struct Cyc_Absyn_WithTypes_struct*_tmpC78;struct Cyc_Absyn_Function_mod_struct*
_tmpC77;struct Cyc_Parse_Declarator*_tmpC76;yyval=Cyc_YY29(((_tmpC76=_cycalloc(
sizeof(*_tmpC76)),((_tmpC76->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))))->id,((
_tmpC76->tms=((_tmpC7B=_cycalloc(sizeof(*_tmpC7B)),((_tmpC7B->hd=(void*)((
_tmpC77=_cycalloc(sizeof(*_tmpC77)),((_tmpC77[0]=((_tmpC7A.tag=3,((_tmpC7A.f1=(
void*)((void*)((_tmpC78=_cycalloc(sizeof(*_tmpC78)),((_tmpC78[0]=((_tmpC79.tag=1,((
_tmpC79.f1=0,((_tmpC79.f2=0,((_tmpC79.f3=0,((_tmpC79.f4=Cyc_yyget_YY49(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpC79.f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpC79)))))))))))),_tmpC78))))),_tmpC7A)))),_tmpC77)))),((_tmpC7B->tl=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->tms,_tmpC7B)))))),_tmpC76)))))));}break;case 180: _LL2E1:{
struct Cyc_List_List*_tmpC90;struct Cyc_Absyn_Function_mod_struct _tmpC8F;struct Cyc_Absyn_NoTypes_struct
_tmpC8E;struct Cyc_Absyn_NoTypes_struct*_tmpC8D;struct Cyc_Absyn_Function_mod_struct*
_tmpC8C;struct Cyc_Parse_Declarator*_tmpC8B;yyval=Cyc_YY29(((_tmpC8B=_cycalloc(
sizeof(*_tmpC8B)),((_tmpC8B->id=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->id,((
_tmpC8B->tms=((_tmpC90=_cycalloc(sizeof(*_tmpC90)),((_tmpC90->hd=(void*)((
_tmpC8C=_cycalloc(sizeof(*_tmpC8C)),((_tmpC8C[0]=((_tmpC8F.tag=3,((_tmpC8F.f1=(
void*)((void*)((_tmpC8D=_cycalloc(sizeof(*_tmpC8D)),((_tmpC8D[0]=((_tmpC8E.tag=0,((
_tmpC8E.f1=Cyc_yyget_YY36(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((_tmpC8E.f2=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpC8E)))))),_tmpC8D))))),
_tmpC8F)))),_tmpC8C)))),((_tmpC90->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,
_tmpC90)))))),_tmpC8B)))))));}break;case 181: _LL2E2: {struct Cyc_List_List*_tmp63F=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)))));{struct Cyc_List_List*_tmpC9A;struct Cyc_Absyn_TypeParams_mod_struct
_tmpC99;struct Cyc_Absyn_TypeParams_mod_struct*_tmpC98;struct Cyc_Parse_Declarator*
_tmpC97;yyval=Cyc_YY29(((_tmpC97=_cycalloc(sizeof(*_tmpC97)),((_tmpC97->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))))->id,((_tmpC97->tms=((_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((
_tmpC9A->hd=(void*)((_tmpC98=_cycalloc(sizeof(*_tmpC98)),((_tmpC98[0]=((_tmpC99.tag=
4,((_tmpC99.f1=_tmp63F,((_tmpC99.f2=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpC99.f3=0,_tmpC99)))))))),_tmpC98)))),((_tmpC9A->tl=(
Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,_tmpC9A)))))),_tmpC97)))))));}break;}case
182: _LL2E3:{struct Cyc_List_List*_tmpCA4;struct Cyc_Absyn_Attributes_mod_struct
_tmpCA3;struct Cyc_Absyn_Attributes_mod_struct*_tmpCA2;struct Cyc_Parse_Declarator*
_tmpCA1;yyval=Cyc_YY29(((_tmpCA1=_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1->id=(Cyc_yyget_YY29(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))))->id,((_tmpCA1->tms=((_tmpCA4=_cycalloc(sizeof(*_tmpCA4)),((
_tmpCA4->hd=(void*)((_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2[0]=((_tmpCA3.tag=
5,((_tmpCA3.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpCA3.f2=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCA3)))))),
_tmpCA2)))),((_tmpCA4->tl=(Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))))->tms,
_tmpCA4)))))),_tmpCA1)))))));}break;case 183: _LL2E4: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 184: _LL2E5: yyval=Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_yyget_YY28(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))));break;case 185: _LL2E6: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)))!= 0){struct Cyc_Absyn_Attributes_mod_struct*_tmpCAA;struct Cyc_Absyn_Attributes_mod_struct
_tmpCA9;struct Cyc_List_List*_tmpCA8;ans=((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((
_tmpCA8->hd=(void*)((_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((_tmpCAA[0]=((_tmpCA9.tag=
5,((_tmpCA9.f1=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),((_tmpCA9.f2=Cyc_yyget_YY45(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpCA9)))))),_tmpCAA)))),((_tmpCA8->tl=ans,_tmpCA8))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;struct Cyc_Position_Segment*_tmp64C;union Cyc_Absyn_Constraint*_tmp64D;
union Cyc_Absyn_Constraint*_tmp64E;struct _tuple18 _tmp64B=*Cyc_yyget_YY1(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4)));_tmp64C=_tmp64B.f1;_tmp64D=_tmp64B.f2;_tmp64E=_tmp64B.f3;if(
Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*_tmpCAB;ptrloc=((_tmpCAB=
_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->ptr_loc=_tmp64C,((_tmpCAB->rgn_loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line),((_tmpCAB->zt_loc=
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line),
_tmpCAB))))))));}{union Cyc_Absyn_Constraint*_tmp651;union Cyc_Absyn_Constraint*
_tmp652;union Cyc_Absyn_Constraint*_tmp653;void*_tmp654;struct _tuple11 _tmp650=Cyc_Parse_collapse_pointer_quals(
_tmp64C,_tmp64D,_tmp64E,Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY54(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))));_tmp651=_tmp650.f1;_tmp652=_tmp650.f2;_tmp653=_tmp650.f3;
_tmp654=_tmp650.f4;{struct Cyc_Absyn_Pointer_mod_struct*_tmpCB5;struct Cyc_Absyn_PtrAtts
_tmpCB4;struct Cyc_Absyn_Pointer_mod_struct _tmpCB3;struct Cyc_List_List*_tmpCB2;
ans=((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2->hd=(void*)((_tmpCB5=
_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5[0]=((_tmpCB3.tag=2,((_tmpCB3.f1=((_tmpCB4.rgn=
_tmp654,((_tmpCB4.nullable=_tmp651,((_tmpCB4.bounds=_tmp652,((_tmpCB4.zero_term=
_tmp653,((_tmpCB4.ptrloc=ptrloc,_tmpCB4)))))))))),((_tmpCB3.f2=Cyc_yyget_YY25(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpCB3)))))),_tmpCB5)))),((_tmpCB2->tl=ans,_tmpCB2))))));}yyval=
Cyc_YY28(ans);break;};};}case 186: _LL2E7: yyval=Cyc_YY54(0);break;case 187: _LL2E8:{
struct Cyc_List_List*_tmpCB6;yyval=Cyc_YY54(((_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((
_tmpCB6->hd=Cyc_yyget_YY53(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((_tmpCB6->tl=Cyc_yyget_YY54(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpCB6)))))));}break;case 188: _LL2E9:{struct Cyc_Absyn_Numelts_ptrqual_struct
_tmpCB9;struct Cyc_Absyn_Numelts_ptrqual_struct*_tmpCB8;yyval=Cyc_YY53((void*)((
_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8[0]=((_tmpCB9.tag=0,((_tmpCB9.f1=
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),_tmpCB9)))),_tmpCB8)))));}break;case 189: _LL2EA:{
struct Cyc_Absyn_Region_ptrqual_struct _tmpCBC;struct Cyc_Absyn_Region_ptrqual_struct*
_tmpCBB;yyval=Cyc_YY53((void*)((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB[0]=((
_tmpCBC.tag=1,((_tmpCBC.f1=(void*)Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpCBC)))),_tmpCBB)))));}break;case 190: _LL2EB:{struct Cyc_Absyn_Thin_ptrqual_struct
_tmpCBF;struct Cyc_Absyn_Thin_ptrqual_struct*_tmpCBE;yyval=Cyc_YY53((void*)((
_tmpCBE=_cycalloc_atomic(sizeof(*_tmpCBE)),((_tmpCBE[0]=((_tmpCBF.tag=2,_tmpCBF)),
_tmpCBE)))));}break;case 191: _LL2EC:{struct Cyc_Absyn_Fat_ptrqual_struct _tmpCC2;
struct Cyc_Absyn_Fat_ptrqual_struct*_tmpCC1;yyval=Cyc_YY53((void*)((_tmpCC1=
_cycalloc_atomic(sizeof(*_tmpCC1)),((_tmpCC1[0]=((_tmpCC2.tag=3,_tmpCC2)),
_tmpCC1)))));}break;case 192: _LL2ED:{struct Cyc_Absyn_Zeroterm_ptrqual_struct
_tmpCC5;struct Cyc_Absyn_Zeroterm_ptrqual_struct*_tmpCC4;yyval=Cyc_YY53((void*)((
_tmpCC4=_cycalloc_atomic(sizeof(*_tmpCC4)),((_tmpCC4[0]=((_tmpCC5.tag=4,_tmpCC5)),
_tmpCC4)))));}break;case 193: _LL2EE:{struct Cyc_Absyn_Nozeroterm_ptrqual_struct
_tmpCC8;struct Cyc_Absyn_Nozeroterm_ptrqual_struct*_tmpCC7;yyval=Cyc_YY53((void*)((
_tmpCC7=_cycalloc_atomic(sizeof(*_tmpCC7)),((_tmpCC7[0]=((_tmpCC8.tag=5,_tmpCC8)),
_tmpCC7)))));}break;case 194: _LL2EF:{struct Cyc_Absyn_Notnull_ptrqual_struct
_tmpCCB;struct Cyc_Absyn_Notnull_ptrqual_struct*_tmpCCA;yyval=Cyc_YY53((void*)((
_tmpCCA=_cycalloc_atomic(sizeof(*_tmpCCA)),((_tmpCCA[0]=((_tmpCCB.tag=6,_tmpCCB)),
_tmpCCA)))));}break;case 195: _LL2F0:{struct Cyc_Absyn_Nullable_ptrqual_struct
_tmpCCE;struct Cyc_Absyn_Nullable_ptrqual_struct*_tmpCCD;yyval=Cyc_YY53((void*)((
_tmpCCD=_cycalloc_atomic(sizeof(*_tmpCCD)),((_tmpCCD[0]=((_tmpCCE.tag=7,_tmpCCE)),
_tmpCCD)))));}break;case 196: _LL2F1: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line);{struct _tuple18*
_tmpCCF;yyval=Cyc_YY1(((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF->f1=loc,((
_tmpCCF->f2=Cyc_Absyn_true_conref,((_tmpCCF->f3=Cyc_yyget_YY2(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCCF)))))))));}
break;}case 197: _LL2F2: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line);{struct _tuple18*
_tmpCD0;yyval=Cyc_YY1(((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0->f1=loc,((
_tmpCD0->f2=Cyc_Absyn_false_conref,((_tmpCD0->f3=Cyc_yyget_YY2(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCD0)))))))));}
break;}case 198: _LL2F3: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line);{struct _tuple18*
_tmpCD1;yyval=Cyc_YY1(((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->f1=loc,((
_tmpCD1->f2=Cyc_Absyn_true_conref,((_tmpCD1->f3=Cyc_Absyn_bounds_dyneither_conref,
_tmpCD1)))))))));}break;}case 199: _LL2F4: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 200: _LL2F5:{struct Cyc_Absyn_Upper_b_struct _tmpCD4;struct Cyc_Absyn_Upper_b_struct*
_tmpCD3;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3[0]=((_tmpCD4.tag=1,((
_tmpCD4.f1=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),_tmpCD4)))),_tmpCD3))))));}break;
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
yyvsp_offset)))));break;case 209: _LL2FE:{struct _tuple23*_tmpCD5;yyval=Cyc_YY39(((
_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),((
_tmpCD5->f2=0,((_tmpCD5->f3=0,((_tmpCD5->f4=Cyc_yyget_YY49(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpCD5->f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCD5)))))))))))));}break;case 210:
_LL2FF:{struct _tuple23*_tmpCD6;yyval=Cyc_YY39(((_tmpCD6=_cycalloc(sizeof(*
_tmpCD6)),((_tmpCD6->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 4)))),((_tmpCD6->f2=1,((_tmpCD6->f3=0,((_tmpCD6->f4=
Cyc_yyget_YY49(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),((_tmpCD6->f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCD6)))))))))))));}
break;case 211: _LL300: {struct _tuple8 _tmp672;struct Cyc_Core_Opt*_tmp673;struct Cyc_Absyn_Tqual
_tmp674;void*_tmp675;struct _tuple8*_tmp671=Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)));
_tmp672=*_tmp671;_tmp673=_tmp672.f1;_tmp674=_tmp672.f2;_tmp675=_tmp672.f3;{
struct Cyc_Absyn_VarargInfo*_tmpCD7;struct Cyc_Absyn_VarargInfo*_tmp676=(_tmpCD7=
_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->name=_tmp673,((_tmpCD7->tq=_tmp674,((
_tmpCD7->type=_tmp675,((_tmpCD7->inject=Cyc_yyget_YY31(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpCD7)))))))));{struct _tuple23*_tmpCD8;yyval=Cyc_YY39(((_tmpCD8=_cycalloc(
sizeof(*_tmpCD8)),((_tmpCD8->f1=0,((_tmpCD8->f2=0,((_tmpCD8->f3=_tmp676,((
_tmpCD8->f4=Cyc_yyget_YY49(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((_tmpCD8->f5=Cyc_yyget_YY50(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpCD8)))))))))))));}break;};}case 212: _LL301: {struct _tuple8
_tmp67A;struct Cyc_Core_Opt*_tmp67B;struct Cyc_Absyn_Tqual _tmp67C;void*_tmp67D;
struct _tuple8*_tmp679=Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)));
_tmp67A=*_tmp679;_tmp67B=_tmp67A.f1;_tmp67C=_tmp67A.f2;_tmp67D=_tmp67A.f3;{
struct Cyc_Absyn_VarargInfo*_tmpCD9;struct Cyc_Absyn_VarargInfo*_tmp67E=(_tmpCD9=
_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9->name=_tmp67B,((_tmpCD9->tq=_tmp67C,((
_tmpCD9->type=_tmp67D,((_tmpCD9->inject=Cyc_yyget_YY31(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpCD9)))))))));{struct _tuple23*_tmpCDA;yyval=Cyc_YY39(((_tmpCDA=_cycalloc(
sizeof(*_tmpCDA)),((_tmpCDA->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6)))),((_tmpCDA->f2=0,((_tmpCDA->f3=
_tmp67E,((_tmpCDA->f4=Cyc_yyget_YY49(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpCDA->f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCDA)))))))))))));}break;};}case 213:
_LL302:{struct Cyc_Absyn_Unknown_kb_struct _tmpCDD;struct Cyc_Absyn_Unknown_kb_struct*
_tmpCDC;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),(void*)((
_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC[0]=((_tmpCDD.tag=1,((_tmpCDD.f1=0,
_tmpCDD)))),_tmpCDC))))));}break;case 214: _LL303: yyval=Cyc_YY44(Cyc_Parse_id2type(
Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))));break;case 215: _LL304: yyval=Cyc_YY49(0);break;case 216: _LL305:{
struct Cyc_Absyn_JoinEff_struct*_tmpCE3;struct Cyc_Absyn_JoinEff_struct _tmpCE2;
struct Cyc_Core_Opt*_tmpCE1;yyval=Cyc_YY49(((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((
_tmpCE1->v=(void*)((_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3[0]=((_tmpCE2.tag=
25,((_tmpCE2.f1=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCE2)))),_tmpCE3)))),_tmpCE1)))));}
break;case 217: _LL306: yyval=Cyc_YY50(0);break;case 218: _LL307: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 219: _LL308: {struct Cyc_Absyn_Less_kb_struct _tmpCE6;struct Cyc_Absyn_Less_kb_struct*
_tmpCE5;struct Cyc_Absyn_Less_kb_struct*_tmp686=(_tmpCE5=_cycalloc(sizeof(*
_tmpCE5)),((_tmpCE5[0]=((_tmpCE6.tag=2,((_tmpCE6.f1=0,((_tmpCE6.f2=& Cyc_Tcutil_trk,
_tmpCE6)))))),_tmpCE5)));struct _dyneither_ptr _tmp687=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)));void*_tmp688=Cyc_Parse_id2type(_tmp687,(void*)_tmp686);{struct
_tuple26*_tmpCF0;struct Cyc_Absyn_JoinEff_struct _tmpCEF;struct Cyc_Absyn_JoinEff_struct*
_tmpCEE;struct Cyc_List_List*_tmpCED;yyval=Cyc_YY50(((_tmpCED=_cycalloc(sizeof(*
_tmpCED)),((_tmpCED->hd=((_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0->f1=(
void*)((_tmpCEE=_cycalloc(sizeof(*_tmpCEE)),((_tmpCEE[0]=((_tmpCEF.tag=25,((
_tmpCEF.f1=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpCEF)))),_tmpCEE)))),((_tmpCF0->f2=
_tmp688,_tmpCF0)))))),((_tmpCED->tl=0,_tmpCED)))))));}break;}case 220: _LL309: {
struct Cyc_Absyn_Less_kb_struct _tmpCF3;struct Cyc_Absyn_Less_kb_struct*_tmpCF2;
struct Cyc_Absyn_Less_kb_struct*_tmp68F=(_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((
_tmpCF2[0]=((_tmpCF3.tag=2,((_tmpCF3.f1=0,((_tmpCF3.f2=& Cyc_Tcutil_trk,_tmpCF3)))))),
_tmpCF2)));struct _dyneither_ptr _tmp690=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)));void*
_tmp691=Cyc_Parse_id2type(_tmp690,(void*)_tmp68F);{struct _tuple26*_tmpCFD;struct
Cyc_Absyn_JoinEff_struct _tmpCFC;struct Cyc_Absyn_JoinEff_struct*_tmpCFB;struct Cyc_List_List*
_tmpCFA;yyval=Cyc_YY50(((_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA->hd=((
_tmpCFD=_cycalloc(sizeof(*_tmpCFD)),((_tmpCFD->f1=(void*)((_tmpCFB=_cycalloc(
sizeof(*_tmpCFB)),((_tmpCFB[0]=((_tmpCFC.tag=25,((_tmpCFC.f1=Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))),_tmpCFC)))),_tmpCFB)))),((_tmpCFD->f2=_tmp691,_tmpCFD)))))),((
_tmpCFA->tl=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpCFA)))))));}break;}case 221: _LL30A:
yyval=Cyc_YY31(0);break;case 222: _LL30B:{const char*_tmpCFE;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpCFE="inject",_tag_dyneither(_tmpCFE,sizeof(char),7))))!= 0){const char*
_tmpCFF;Cyc_Parse_err(((_tmpCFF="missing type in function declaration",
_tag_dyneither(_tmpCFF,sizeof(char),37))),Cyc_Position_segment_of_abs((*((struct
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
break;case 227: _LL310:{struct Cyc_Absyn_RgnsEff_struct*_tmpD05;struct Cyc_Absyn_RgnsEff_struct
_tmpD04;struct Cyc_List_List*_tmpD03;yyval=Cyc_YY40(((_tmpD03=_cycalloc(sizeof(*
_tmpD03)),((_tmpD03->hd=(void*)((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05[0]=((
_tmpD04.tag=26,((_tmpD04.f1=(void*)Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpD04)))),_tmpD05)))),((_tmpD03->tl=0,_tmpD03)))))));}break;case 228: _LL311: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),& Cyc_Tcutil_ek,0);{struct Cyc_List_List*_tmpD06;
yyval=Cyc_YY40(((_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06->hd=Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD06->tl=0,_tmpD06)))))));}break;case 229: _LL312: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),& Cyc_Tcutil_trk,1);{struct Cyc_Absyn_AccessEff_struct*
_tmpD0C;struct Cyc_Absyn_AccessEff_struct _tmpD0B;struct Cyc_List_List*_tmpD0A;
yyval=Cyc_YY40(((_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A->hd=(void*)((
_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C[0]=((_tmpD0B.tag=24,((_tmpD0B.f1=(
void*)Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD0B)))),_tmpD0C)))),((_tmpD0A->tl=0,
_tmpD0A)))))));}break;case 230: _LL313: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),& Cyc_Tcutil_trk,1);{struct Cyc_Absyn_AccessEff_struct*_tmpD12;
struct Cyc_Absyn_AccessEff_struct _tmpD11;struct Cyc_List_List*_tmpD10;yyval=Cyc_YY40(((
_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10->hd=(void*)((_tmpD12=_cycalloc(
sizeof(*_tmpD12)),((_tmpD12[0]=((_tmpD11.tag=24,((_tmpD11.f1=(void*)Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),_tmpD11)))),_tmpD12)))),((_tmpD10->tl=Cyc_yyget_YY40(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpD10)))))));}break;case 231: _LL314:{struct Cyc_List_List*
_tmpD13;yyval=Cyc_YY38(((_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13->hd=Cyc_yyget_YY37(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD13->tl=0,_tmpD13)))))));}break;case 232: _LL315:{struct Cyc_List_List*
_tmpD14;yyval=Cyc_YY38(((_tmpD14=_cycalloc(sizeof(*_tmpD14)),((_tmpD14->hd=Cyc_yyget_YY37(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD14->tl=Cyc_yyget_YY38(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpD14)))))));}break;case 233: _LL316: {struct _tuple22 _tmp6A7;struct Cyc_Absyn_Tqual
_tmp6A8;struct Cyc_List_List*_tmp6A9;struct Cyc_List_List*_tmp6AA;struct _tuple22*
_tmp6A6=Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));_tmp6A7=*_tmp6A6;_tmp6A8=_tmp6A7.f1;
_tmp6A9=_tmp6A7.f2;_tmp6AA=_tmp6A7.f3;if(_tmp6A8.loc == 0)_tmp6A8.loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line);{struct Cyc_Parse_Declarator
_tmp6AC;struct _tuple1*_tmp6AD;struct Cyc_List_List*_tmp6AE;struct Cyc_Parse_Declarator*
_tmp6AB=Cyc_yyget_YY29(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset)));_tmp6AC=*_tmp6AB;_tmp6AD=_tmp6AC.id;
_tmp6AE=_tmp6AC.tms;{void*_tmp6AF=Cyc_Parse_speclist2typ(_tmp6A9,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));struct Cyc_Absyn_Tqual
_tmp6B1;void*_tmp6B2;struct Cyc_List_List*_tmp6B3;struct Cyc_List_List*_tmp6B4;
struct _tuple10 _tmp6B0=Cyc_Parse_apply_tms(_tmp6A8,_tmp6AF,_tmp6AA,_tmp6AE);
_tmp6B1=_tmp6B0.f1;_tmp6B2=_tmp6B0.f2;_tmp6B3=_tmp6B0.f3;_tmp6B4=_tmp6B0.f4;if(
_tmp6B3 != 0){const char*_tmpD15;Cyc_Parse_err(((_tmpD15="parameter with bad type params",
_tag_dyneither(_tmpD15,sizeof(char),31))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));}if(Cyc_Absyn_is_qvar_qualified(
_tmp6AD)){const char*_tmpD16;Cyc_Parse_err(((_tmpD16="parameter cannot be qualified with a namespace",
_tag_dyneither(_tmpD16,sizeof(char),47))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));}{struct Cyc_Core_Opt*
_tmpD17;struct Cyc_Core_Opt*_tmp6B7=(struct Cyc_Core_Opt*)((_tmpD17=_cycalloc(
sizeof(*_tmpD17)),((_tmpD17->v=(*_tmp6AD).f2,_tmpD17))));if(_tmp6B4 != 0){const
char*_tmpD1A;void*_tmpD19;(_tmpD19=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpD1A="extra attributes on parameter, ignoring",
_tag_dyneither(_tmpD1A,sizeof(char),40))),_tag_dyneither(_tmpD19,sizeof(void*),0)));}{
struct _tuple8*_tmpD1B;yyval=Cyc_YY37(((_tmpD1B=_cycalloc(sizeof(*_tmpD1B)),((
_tmpD1B->f1=_tmp6B7,((_tmpD1B->f2=_tmp6B1,((_tmpD1B->f3=_tmp6B2,_tmpD1B)))))))));}
break;};};};}case 234: _LL317: {struct _tuple22 _tmp6BD;struct Cyc_Absyn_Tqual _tmp6BE;
struct Cyc_List_List*_tmp6BF;struct Cyc_List_List*_tmp6C0;struct _tuple22*_tmp6BC=
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)));_tmp6BD=*_tmp6BC;_tmp6BE=_tmp6BD.f1;_tmp6BF=_tmp6BD.f2;
_tmp6C0=_tmp6BD.f3;if(_tmp6BE.loc == 0)_tmp6BE.loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line);{void*_tmp6C1=Cyc_Parse_speclist2typ(
_tmp6BF,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));if(_tmp6C0 != 0){const char*_tmpD1E;void*_tmpD1D;(
_tmpD1D=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpD1E="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpD1E,sizeof(char),38))),_tag_dyneither(_tmpD1D,sizeof(void*),0)));}{
struct _tuple8*_tmpD1F;yyval=Cyc_YY37(((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((
_tmpD1F->f1=0,((_tmpD1F->f2=_tmp6BE,((_tmpD1F->f3=_tmp6C1,_tmpD1F)))))))));}
break;};}case 235: _LL318: {struct _tuple22 _tmp6C6;struct Cyc_Absyn_Tqual _tmp6C7;
struct Cyc_List_List*_tmp6C8;struct Cyc_List_List*_tmp6C9;struct _tuple22*_tmp6C5=
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1)));_tmp6C6=*_tmp6C5;_tmp6C7=_tmp6C6.f1;_tmp6C8=
_tmp6C6.f2;_tmp6C9=_tmp6C6.f3;if(_tmp6C7.loc == 0)_tmp6C7.loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line);{void*_tmp6CA=Cyc_Parse_speclist2typ(
_tmp6C8,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));struct Cyc_List_List*_tmp6CB=(Cyc_yyget_YY30(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))))->tms;struct Cyc_Absyn_Tqual _tmp6CD;void*_tmp6CE;struct Cyc_List_List*
_tmp6CF;struct Cyc_List_List*_tmp6D0;struct _tuple10 _tmp6CC=Cyc_Parse_apply_tms(
_tmp6C7,_tmp6CA,_tmp6C9,_tmp6CB);_tmp6CD=_tmp6CC.f1;_tmp6CE=_tmp6CC.f2;_tmp6CF=
_tmp6CC.f3;_tmp6D0=_tmp6CC.f4;if(_tmp6CF != 0){const char*_tmpD22;void*_tmpD21;(
_tmpD21=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpD22="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpD22,sizeof(char),49))),_tag_dyneither(_tmpD21,sizeof(void*),0)));}
if(_tmp6D0 != 0){const char*_tmpD25;void*_tmpD24;(_tmpD24=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpD25="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpD25,sizeof(char),38))),_tag_dyneither(_tmpD24,sizeof(void*),0)));}{
struct _tuple8*_tmpD26;yyval=Cyc_YY37(((_tmpD26=_cycalloc(sizeof(*_tmpD26)),((
_tmpD26->f1=0,((_tmpD26->f2=_tmp6CD,((_tmpD26->f3=_tmp6CE,_tmpD26)))))))));}
break;};}case 236: _LL319: yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY36(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))));
break;case 237: _LL31A:{struct _dyneither_ptr*_tmpD29;struct Cyc_List_List*_tmpD28;
yyval=Cyc_YY36(((_tmpD28=_cycalloc(sizeof(*_tmpD28)),((_tmpD28->hd=((_tmpD29=
_cycalloc(sizeof(*_tmpD29)),((_tmpD29[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD29)))),((
_tmpD28->tl=0,_tmpD28)))))));}break;case 238: _LL31B:{struct _dyneither_ptr*_tmpD2C;
struct Cyc_List_List*_tmpD2B;yyval=Cyc_YY36(((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((
_tmpD2B->hd=((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C[0]=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpD2C)))),((_tmpD2B->tl=Cyc_yyget_YY36(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpD2B)))))));}break;case 239: _LL31C: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 240: _LL31D: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 241: _LL31E:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD2F;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD2E;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD2E=_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E[0]=((_tmpD2F.tag=36,((
_tmpD2F.f1=0,((_tmpD2F.f2=0,_tmpD2F)))))),_tmpD2E)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 242:
_LL31F:{struct Cyc_Absyn_UnresolvedMem_e_struct _tmpD32;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpD31;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD31=_cycalloc(sizeof(*
_tmpD31)),((_tmpD31[0]=((_tmpD32.tag=36,((_tmpD32.f1=0,((_tmpD32.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)))),
_tmpD32)))))),_tmpD31)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 243: _LL320:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD35;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD34;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD34=_cycalloc(sizeof(*_tmpD34)),((_tmpD34[0]=((_tmpD35.tag=36,((
_tmpD35.f1=0,((_tmpD35.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2)))),_tmpD35)))))),_tmpD34)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 244:
_LL321: {struct _dyneither_ptr*_tmpD38;struct _tuple1*_tmpD37;struct Cyc_Absyn_Vardecl*
_tmp6E0=Cyc_Absyn_new_vardecl(((_tmpD37=_cycalloc(sizeof(*_tmpD37)),((_tmpD37->f1=
Cyc_Absyn_Loc_n,((_tmpD37->f2=((_tmpD38=_cycalloc(sizeof(*_tmpD38)),((_tmpD38[0]=
Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))),_tmpD38)))),_tmpD37)))))),Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).last_line)));(_tmp6E0->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct _tmpD3B;struct Cyc_Absyn_Comprehension_e_struct*
_tmpD3A;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD3A=_cycalloc(sizeof(*
_tmpD3A)),((_tmpD3A[0]=((_tmpD3B.tag=28,((_tmpD3B.f1=_tmp6E0,((_tmpD3B.f2=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpD3B.f3=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpD3B.f4=0,_tmpD3B)))))))))),_tmpD3A)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 7))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 245:
_LL322:{struct _tuple27*_tmpD3E;struct Cyc_List_List*_tmpD3D;yyval=Cyc_YY6(((
_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D->hd=((_tmpD3E=_cycalloc(sizeof(*
_tmpD3E)),((_tmpD3E->f1=0,((_tmpD3E->f2=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD3E)))))),((
_tmpD3D->tl=0,_tmpD3D)))))));}break;case 246: _LL323:{struct _tuple27*_tmpD41;
struct Cyc_List_List*_tmpD40;yyval=Cyc_YY6(((_tmpD40=_cycalloc(sizeof(*_tmpD40)),((
_tmpD40->hd=((_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41->f1=Cyc_yyget_YY41(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpD41->f2=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD41)))))),((
_tmpD40->tl=0,_tmpD40)))))));}break;case 247: _LL324:{struct _tuple27*_tmpD44;
struct Cyc_List_List*_tmpD43;yyval=Cyc_YY6(((_tmpD43=_cycalloc(sizeof(*_tmpD43)),((
_tmpD43->hd=((_tmpD44=_cycalloc(sizeof(*_tmpD44)),((_tmpD44->f1=0,((_tmpD44->f2=
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),_tmpD44)))))),((_tmpD43->tl=Cyc_yyget_YY6(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),_tmpD43)))))));}break;case 248: _LL325:{struct _tuple27*_tmpD47;
struct Cyc_List_List*_tmpD46;yyval=Cyc_YY6(((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((
_tmpD46->hd=((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47->f1=Cyc_yyget_YY41(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpD47->f2=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD47)))))),((
_tmpD46->tl=Cyc_yyget_YY6(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),_tmpD46)))))));}break;case 249: _LL326:
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY41(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1)))));break;case 250: _LL327:{struct Cyc_List_List*
_tmpD48;yyval=Cyc_YY41(((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->hd=Cyc_yyget_YY42(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD48->tl=0,_tmpD48)))))));}break;case 251: _LL328:{struct Cyc_List_List*
_tmpD49;yyval=Cyc_YY41(((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->hd=Cyc_yyget_YY42(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD49->tl=Cyc_yyget_YY41(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpD49)))))));}break;case 252: _LL329:{struct Cyc_Absyn_ArrayElement_struct _tmpD4C;
struct Cyc_Absyn_ArrayElement_struct*_tmpD4B;yyval=Cyc_YY42((void*)((_tmpD4B=
_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B[0]=((_tmpD4C.tag=0,((_tmpD4C.f1=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpD4C)))),_tmpD4B)))));}break;case 253: _LL32A:{struct Cyc_Absyn_FieldName_struct
_tmpD52;struct _dyneither_ptr*_tmpD51;struct Cyc_Absyn_FieldName_struct*_tmpD50;
yyval=Cyc_YY42((void*)((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50[0]=((
_tmpD52.tag=1,((_tmpD52.f1=((_tmpD51=_cycalloc(sizeof(*_tmpD51)),((_tmpD51[0]=
Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD51)))),_tmpD52)))),_tmpD50)))));}
break;case 254: _LL32B: {struct _tuple22 _tmp6F5;struct Cyc_Absyn_Tqual _tmp6F6;struct
Cyc_List_List*_tmp6F7;struct Cyc_List_List*_tmp6F8;struct _tuple22*_tmp6F4=Cyc_yyget_YY35(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)));_tmp6F5=*_tmp6F4;_tmp6F6=_tmp6F5.f1;_tmp6F7=_tmp6F5.f2;_tmp6F8=
_tmp6F5.f3;{void*_tmp6F9=Cyc_Parse_speclist2typ(_tmp6F7,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));if(_tmp6F8 != 0){const
char*_tmpD55;void*_tmpD54;(_tmpD54=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpD55="ignoring attributes in type",
_tag_dyneither(_tmpD55,sizeof(char),28))),_tag_dyneither(_tmpD54,sizeof(void*),0)));}{
struct _tuple8*_tmpD56;yyval=Cyc_YY37(((_tmpD56=_cycalloc(sizeof(*_tmpD56)),((
_tmpD56->f1=0,((_tmpD56->f2=_tmp6F6,((_tmpD56->f3=_tmp6F9,_tmpD56)))))))));}
break;};}case 255: _LL32C: {struct _tuple22 _tmp6FE;struct Cyc_Absyn_Tqual _tmp6FF;
struct Cyc_List_List*_tmp700;struct Cyc_List_List*_tmp701;struct _tuple22*_tmp6FD=
Cyc_yyget_YY35(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1)));_tmp6FE=*_tmp6FD;_tmp6FF=_tmp6FE.f1;_tmp700=
_tmp6FE.f2;_tmp701=_tmp6FE.f3;{void*_tmp702=Cyc_Parse_speclist2typ(_tmp700,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));struct Cyc_List_List*
_tmp703=(Cyc_yyget_YY30(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tms;struct _tuple10 _tmp704=Cyc_Parse_apply_tms(
_tmp6FF,_tmp702,_tmp701,_tmp703);if(_tmp704.f3 != 0){const char*_tmpD59;void*
_tmpD58;(_tmpD58=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),((_tmpD59="bad type params, ignoring",_tag_dyneither(
_tmpD59,sizeof(char),26))),_tag_dyneither(_tmpD58,sizeof(void*),0)));}if(_tmp704.f4
!= 0){const char*_tmpD5C;void*_tmpD5B;(_tmpD5B=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpD5C="bad specifiers, ignoring",
_tag_dyneither(_tmpD5C,sizeof(char),25))),_tag_dyneither(_tmpD5B,sizeof(void*),0)));}{
struct _tuple8*_tmpD5D;yyval=Cyc_YY37(((_tmpD5D=_cycalloc(sizeof(*_tmpD5D)),((
_tmpD5D->f1=0,((_tmpD5D->f2=_tmp704.f1,((_tmpD5D->f3=_tmp704.f2,_tmpD5D)))))))));}
break;};}case 256: _LL32D: yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 257: _LL32E:{struct Cyc_Absyn_JoinEff_struct
_tmpD60;struct Cyc_Absyn_JoinEff_struct*_tmpD5F;yyval=Cyc_YY44((void*)((_tmpD5F=
_cycalloc(sizeof(*_tmpD5F)),((_tmpD5F[0]=((_tmpD60.tag=25,((_tmpD60.f1=0,_tmpD60)))),
_tmpD5F)))));}break;case 258: _LL32F:{struct Cyc_Absyn_JoinEff_struct _tmpD63;struct
Cyc_Absyn_JoinEff_struct*_tmpD62;yyval=Cyc_YY44((void*)((_tmpD62=_cycalloc(
sizeof(*_tmpD62)),((_tmpD62[0]=((_tmpD63.tag=25,((_tmpD63.f1=Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpD63)))),_tmpD62)))));}break;case 259: _LL330:{struct Cyc_Absyn_RgnsEff_struct
_tmpD66;struct Cyc_Absyn_RgnsEff_struct*_tmpD65;yyval=Cyc_YY44((void*)((_tmpD65=
_cycalloc(sizeof(*_tmpD65)),((_tmpD65[0]=((_tmpD66.tag=26,((_tmpD66.f1=(void*)
Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),_tmpD66)))),_tmpD65)))));}break;case 260: _LL331:{
struct Cyc_Absyn_JoinEff_struct _tmpD6C;struct Cyc_List_List*_tmpD6B;struct Cyc_Absyn_JoinEff_struct*
_tmpD6A;yyval=Cyc_YY44((void*)((_tmpD6A=_cycalloc(sizeof(*_tmpD6A)),((_tmpD6A[0]=((
_tmpD6C.tag=25,((_tmpD6C.f1=((_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B->hd=
Cyc_yyget_YY44(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),((_tmpD6B->tl=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpD6B)))))),
_tmpD6C)))),_tmpD6A)))));}break;case 261: _LL332:{struct Cyc_List_List*_tmpD6D;
yyval=Cyc_YY40(((_tmpD6D=_cycalloc(sizeof(*_tmpD6D)),((_tmpD6D->hd=Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD6D->tl=0,_tmpD6D)))))));}break;case 262: _LL333:{struct Cyc_List_List*
_tmpD6E;yyval=Cyc_YY40(((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E->hd=Cyc_yyget_YY44(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD6E->tl=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpD6E)))))));}break;case 263: _LL334:{struct Cyc_Parse_Abstractdeclarator*_tmpD6F;
yyval=Cyc_YY30(((_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F->tms=Cyc_yyget_YY28(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpD6F)))));}break;case 264: _LL335: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 265: _LL336:{struct Cyc_Parse_Abstractdeclarator*_tmpD70;yyval=Cyc_YY30(((
_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70->tms=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),(Cyc_yyget_YY30(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))))->tms),
_tmpD70)))));}break;case 266: _LL337: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1));
break;case 267: _LL338:{struct Cyc_List_List*_tmpD7A;struct Cyc_Absyn_Carray_mod_struct
_tmpD79;struct Cyc_Absyn_Carray_mod_struct*_tmpD78;struct Cyc_Parse_Abstractdeclarator*
_tmpD77;yyval=Cyc_YY30(((_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77->tms=((
_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((_tmpD7A->hd=(void*)((_tmpD78=_cycalloc(
sizeof(*_tmpD78)),((_tmpD78[0]=((_tmpD79.tag=0,((_tmpD79.f1=Cyc_yyget_YY51(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD79.f2=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpD79)))))),_tmpD78)))),((_tmpD7A->tl=0,_tmpD7A)))))),
_tmpD77)))));}break;case 268: _LL339:{struct Cyc_List_List*_tmpD84;struct Cyc_Absyn_Carray_mod_struct
_tmpD83;struct Cyc_Absyn_Carray_mod_struct*_tmpD82;struct Cyc_Parse_Abstractdeclarator*
_tmpD81;yyval=Cyc_YY30(((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81->tms=((
_tmpD84=_cycalloc(sizeof(*_tmpD84)),((_tmpD84->hd=(void*)((_tmpD82=_cycalloc(
sizeof(*_tmpD82)),((_tmpD82[0]=((_tmpD83.tag=0,((_tmpD83.f1=Cyc_yyget_YY51(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpD83.f2=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpD83)))))),_tmpD82)))),((_tmpD84->tl=(Cyc_yyget_YY30(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))))->tms,_tmpD84)))))),_tmpD81)))));}break;case 269: _LL33A:{
struct Cyc_List_List*_tmpD8E;struct Cyc_Absyn_ConstArray_mod_struct _tmpD8D;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD8C;struct Cyc_Parse_Abstractdeclarator*
_tmpD8B;yyval=Cyc_YY30(((_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B->tms=((
_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E->hd=(void*)((_tmpD8C=_cycalloc(
sizeof(*_tmpD8C)),((_tmpD8C[0]=((_tmpD8D.tag=1,((_tmpD8D.f1=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpD8D.f2=Cyc_yyget_YY51(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpD8D.f3=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpD8D)))))))),_tmpD8C)))),((_tmpD8E->tl=0,_tmpD8E)))))),
_tmpD8B)))));}break;case 270: _LL33B:{struct Cyc_List_List*_tmpD98;struct Cyc_Absyn_ConstArray_mod_struct
_tmpD97;struct Cyc_Absyn_ConstArray_mod_struct*_tmpD96;struct Cyc_Parse_Abstractdeclarator*
_tmpD95;yyval=Cyc_YY30(((_tmpD95=_cycalloc(sizeof(*_tmpD95)),((_tmpD95->tms=((
_tmpD98=_cycalloc(sizeof(*_tmpD98)),((_tmpD98->hd=(void*)((_tmpD96=_cycalloc(
sizeof(*_tmpD96)),((_tmpD96[0]=((_tmpD97.tag=1,((_tmpD97.f1=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpD97.f2=Cyc_yyget_YY51(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpD97.f3=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpD97)))))))),_tmpD96)))),((_tmpD98->tl=(Cyc_yyget_YY30(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->tms,_tmpD98)))))),_tmpD95)))));}break;case 271: _LL33C:{
struct Cyc_List_List*_tmpDAD;struct Cyc_Absyn_Function_mod_struct _tmpDAC;struct Cyc_Absyn_WithTypes_struct
_tmpDAB;struct Cyc_Absyn_WithTypes_struct*_tmpDAA;struct Cyc_Absyn_Function_mod_struct*
_tmpDA9;struct Cyc_Parse_Abstractdeclarator*_tmpDA8;yyval=Cyc_YY30(((_tmpDA8=
_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8->tms=((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((
_tmpDAD->hd=(void*)((_tmpDA9=_cycalloc(sizeof(*_tmpDA9)),((_tmpDA9[0]=((_tmpDAC.tag=
3,((_tmpDAC.f1=(void*)((void*)((_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA[0]=((
_tmpDAB.tag=1,((_tmpDAB.f1=0,((_tmpDAB.f2=0,((_tmpDAB.f3=0,((_tmpDAB.f4=Cyc_yyget_YY49(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpDAB.f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpDAB)))))))))))),_tmpDAA))))),_tmpDAC)))),_tmpDA9)))),((_tmpDAD->tl=0,_tmpDAD)))))),
_tmpDA8)))));}break;case 272: _LL33D: {struct _tuple23 _tmp72E;struct Cyc_List_List*
_tmp72F;int _tmp730;struct Cyc_Absyn_VarargInfo*_tmp731;struct Cyc_Core_Opt*_tmp732;
struct Cyc_List_List*_tmp733;struct _tuple23*_tmp72D=Cyc_yyget_YY39(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));
_tmp72E=*_tmp72D;_tmp72F=_tmp72E.f1;_tmp730=_tmp72E.f2;_tmp731=_tmp72E.f3;
_tmp732=_tmp72E.f4;_tmp733=_tmp72E.f5;{struct Cyc_List_List*_tmpDC2;struct Cyc_Absyn_Function_mod_struct
_tmpDC1;struct Cyc_Absyn_WithTypes_struct _tmpDC0;struct Cyc_Absyn_WithTypes_struct*
_tmpDBF;struct Cyc_Absyn_Function_mod_struct*_tmpDBE;struct Cyc_Parse_Abstractdeclarator*
_tmpDBD;yyval=Cyc_YY30(((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->tms=((
_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2->hd=(void*)((_tmpDBE=_cycalloc(
sizeof(*_tmpDBE)),((_tmpDBE[0]=((_tmpDC1.tag=3,((_tmpDC1.f1=(void*)((void*)((
_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF[0]=((_tmpDC0.tag=1,((_tmpDC0.f1=
_tmp72F,((_tmpDC0.f2=_tmp730,((_tmpDC0.f3=_tmp731,((_tmpDC0.f4=_tmp732,((_tmpDC0.f5=
_tmp733,_tmpDC0)))))))))))),_tmpDBF))))),_tmpDC1)))),_tmpDBE)))),((_tmpDC2->tl=0,
_tmpDC2)))))),_tmpDBD)))));}break;}case 273: _LL33E:{struct Cyc_List_List*_tmpDD7;
struct Cyc_Absyn_Function_mod_struct _tmpDD6;struct Cyc_Absyn_WithTypes_struct
_tmpDD5;struct Cyc_Absyn_WithTypes_struct*_tmpDD4;struct Cyc_Absyn_Function_mod_struct*
_tmpDD3;struct Cyc_Parse_Abstractdeclarator*_tmpDD2;yyval=Cyc_YY30(((_tmpDD2=
_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->tms=((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((
_tmpDD7->hd=(void*)((_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3[0]=((_tmpDD6.tag=
3,((_tmpDD6.f1=(void*)((void*)((_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4[0]=((
_tmpDD5.tag=1,((_tmpDD5.f1=0,((_tmpDD5.f2=0,((_tmpDD5.f3=0,((_tmpDD5.f4=Cyc_yyget_YY49(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpDD5.f5=Cyc_yyget_YY50(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpDD5)))))))))))),_tmpDD4))))),_tmpDD6)))),_tmpDD3)))),((_tmpDD7->tl=(Cyc_yyget_YY30(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))))->tms,_tmpDD7)))))),_tmpDD2)))));}break;case 274: _LL33F: {
struct _tuple23 _tmp741;struct Cyc_List_List*_tmp742;int _tmp743;struct Cyc_Absyn_VarargInfo*
_tmp744;struct Cyc_Core_Opt*_tmp745;struct Cyc_List_List*_tmp746;struct _tuple23*
_tmp740=Cyc_yyget_YY39(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));_tmp741=*_tmp740;_tmp742=_tmp741.f1;
_tmp743=_tmp741.f2;_tmp744=_tmp741.f3;_tmp745=_tmp741.f4;_tmp746=_tmp741.f5;{
struct Cyc_List_List*_tmpDEC;struct Cyc_Absyn_Function_mod_struct _tmpDEB;struct Cyc_Absyn_WithTypes_struct
_tmpDEA;struct Cyc_Absyn_WithTypes_struct*_tmpDE9;struct Cyc_Absyn_Function_mod_struct*
_tmpDE8;struct Cyc_Parse_Abstractdeclarator*_tmpDE7;yyval=Cyc_YY30(((_tmpDE7=
_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->tms=((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((
_tmpDEC->hd=(void*)((_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8[0]=((_tmpDEB.tag=
3,((_tmpDEB.f1=(void*)((void*)((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9[0]=((
_tmpDEA.tag=1,((_tmpDEA.f1=_tmp742,((_tmpDEA.f2=_tmp743,((_tmpDEA.f3=_tmp744,((
_tmpDEA.f4=_tmp745,((_tmpDEA.f5=_tmp746,_tmpDEA)))))))))))),_tmpDE9))))),_tmpDEB)))),
_tmpDE8)))),((_tmpDEC->tl=(Cyc_yyget_YY30(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))))->tms,
_tmpDEC)))))),_tmpDE7)))));}break;}case 275: _LL340: {struct Cyc_List_List*_tmp74D=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)))));{struct Cyc_List_List*_tmpDF6;struct Cyc_Absyn_TypeParams_mod_struct
_tmpDF5;struct Cyc_Absyn_TypeParams_mod_struct*_tmpDF4;struct Cyc_Parse_Abstractdeclarator*
_tmpDF3;yyval=Cyc_YY30(((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3->tms=((
_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6->hd=(void*)((_tmpDF4=_cycalloc(
sizeof(*_tmpDF4)),((_tmpDF4[0]=((_tmpDF5.tag=4,((_tmpDF5.f1=_tmp74D,((_tmpDF5.f2=
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((
_tmpDF5.f3=0,_tmpDF5)))))))),_tmpDF4)))),((_tmpDF6->tl=(Cyc_yyget_YY30(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))))->tms,_tmpDF6)))))),_tmpDF3)))));}break;}case 276: _LL341:{
struct Cyc_List_List*_tmpE00;struct Cyc_Absyn_Attributes_mod_struct _tmpDFF;struct
Cyc_Absyn_Attributes_mod_struct*_tmpDFE;struct Cyc_Parse_Abstractdeclarator*
_tmpDFD;yyval=Cyc_YY30(((_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD->tms=((
_tmpE00=_cycalloc(sizeof(*_tmpE00)),((_tmpE00->hd=(void*)((_tmpDFE=_cycalloc(
sizeof(*_tmpDFE)),((_tmpDFE[0]=((_tmpDFF.tag=5,((_tmpDFF.f1=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),((_tmpDFF.f2=Cyc_yyget_YY45(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpDFF)))))),_tmpDFE)))),((_tmpE00->tl=(Cyc_yyget_YY30(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))))->tms,_tmpE00)))))),_tmpDFD)))));}break;case 277: _LL342: yyval=*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 278: _LL343: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 279: _LL344: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 280: _LL345: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 281: _LL346: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 282: _LL347: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 283: _LL348:{struct Cyc_Absyn_ResetRegion_s_struct _tmpE03;struct Cyc_Absyn_ResetRegion_s_struct*
_tmpE02;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)((_tmpE02=_cycalloc(sizeof(*
_tmpE02)),((_tmpE02[0]=((_tmpE03.tag=16,((_tmpE03.f1=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpE03)))),_tmpE02)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 284: _LL349: yyval=Cyc_YY4(0);break;case
285: _LL34A:{const char*_tmpE04;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpE04="open",_tag_dyneither(_tmpE04,sizeof(char),5))))!= 
0){const char*_tmpE05;Cyc_Parse_err(((_tmpE05="expecting `open'",_tag_dyneither(
_tmpE05,sizeof(char),17))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line));}}yyval=Cyc_YY4((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))));break;case 286: _LL34B:{struct Cyc_Absyn_Label_s_struct _tmpE0B;
struct _dyneither_ptr*_tmpE0A;struct Cyc_Absyn_Label_s_struct*_tmpE09;yyval=Cyc_YY9(
Cyc_Absyn_new_stmt((void*)((_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09[0]=((
_tmpE0B.tag=13,((_tmpE0B.f1=((_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A[0]=
Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpE0A)))),((_tmpE0B.f2=Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpE0B)))))),_tmpE09)))),Cyc_Position_segment_of_abs((*((struct
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
struct Cyc_Absyn_Fn_d_struct _tmpE0E;struct Cyc_Absyn_Fn_d_struct*_tmpE0D;yyval=Cyc_YY9(
Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE0D=_cycalloc(sizeof(*
_tmpE0D)),((_tmpE0D[0]=((_tmpE0E.tag=1,((_tmpE0E.f1=Cyc_yyget_YY17(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE0E)))),
_tmpE0D)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),Cyc_Absyn_skip_stmt(0)));}break;case 296: _LL355:{
struct Cyc_Absyn_Fn_d_struct _tmpE11;struct Cyc_Absyn_Fn_d_struct*_tmpE10;yyval=Cyc_YY9(
Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE10=_cycalloc(sizeof(*
_tmpE10)),((_tmpE10[0]=((_tmpE11.tag=1,((_tmpE11.f1=Cyc_yyget_YY17(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpE11)))),_tmpE10)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
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
_tmpE14;struct Cyc_Absyn_UnknownId_e_struct*_tmpE13;struct Cyc_Absyn_Exp*_tmp761=
Cyc_Absyn_new_exp((void*)((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13[0]=((
_tmpE14.tag=2,((_tmpE14.f1=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpE14)))),_tmpE13)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp761,Cyc_yyget_YY10(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;}case 301: _LL35A: {struct Cyc_Absyn_Exp*_tmp764=
Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 6))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(
_tmp764,Cyc_yyget_YY10(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
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
304: _LL35D:{struct Cyc_Absyn_Switch_clause*_tmpE17;struct Cyc_List_List*_tmpE16;
yyval=Cyc_YY10(((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16->hd=((_tmpE17=
_cycalloc(sizeof(*_tmpE17)),((_tmpE17->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line)),((
_tmpE17->pat_vars=0,((_tmpE17->where_clause=0,((_tmpE17->body=Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpE17->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpE17)))))))))))),((_tmpE16->tl=0,_tmpE16)))))));}
break;case 305: _LL35E:{struct Cyc_Absyn_Switch_clause*_tmpE1A;struct Cyc_List_List*
_tmpE19;yyval=Cyc_YY10(((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19->hd=((
_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A->pattern=Cyc_yyget_YY11(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpE1A->pat_vars=0,((_tmpE1A->where_clause=0,((_tmpE1A->body=
Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line)),((_tmpE1A->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpE1A)))))))))))),((
_tmpE19->tl=Cyc_yyget_YY10(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE19)))))));}break;case 306: _LL35F:{
struct Cyc_Absyn_Switch_clause*_tmpE1D;struct Cyc_List_List*_tmpE1C;yyval=Cyc_YY10(((
_tmpE1C=_cycalloc(sizeof(*_tmpE1C)),((_tmpE1C->hd=((_tmpE1D=_cycalloc(sizeof(*
_tmpE1D)),((_tmpE1D->pattern=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((
_tmpE1D->pat_vars=0,((_tmpE1D->where_clause=0,((_tmpE1D->body=Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpE1D->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),_tmpE1D)))))))))))),((_tmpE1C->tl=Cyc_yyget_YY10(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpE1C)))))));}break;case 307: _LL360:{struct Cyc_Absyn_Switch_clause*
_tmpE20;struct Cyc_List_List*_tmpE1F;yyval=Cyc_YY10(((_tmpE1F=_cycalloc(sizeof(*
_tmpE1F)),((_tmpE1F->hd=((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20->pattern=
Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 4))),((_tmpE20->pat_vars=0,((_tmpE20->where_clause=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((
_tmpE20->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line)),((_tmpE20->loc=Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line),_tmpE20)))))))))))),((
_tmpE1F->tl=Cyc_yyget_YY10(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE1F)))))));}break;case 308: _LL361:{
struct Cyc_Absyn_Switch_clause*_tmpE23;struct Cyc_List_List*_tmpE22;yyval=Cyc_YY10(((
_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22->hd=((_tmpE23=_cycalloc(sizeof(*
_tmpE23)),((_tmpE23->pattern=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))),((
_tmpE23->pat_vars=0,((_tmpE23->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpE23->body=Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpE23->loc=Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line),_tmpE23)))))))))))),((_tmpE22->tl=Cyc_yyget_YY10(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpE22)))))));}break;case 309: _LL362: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
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
struct Cyc_List_List*_tmp76F=Cyc_yyget_YY18(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3)));
struct Cyc_Absyn_Stmt*_tmp770=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(
_tmp76F,_tmp770));break;}case 320: _LL36D: {struct Cyc_List_List*_tmp771=Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4)));struct Cyc_Absyn_Stmt*_tmp772=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 3))),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp771,
_tmp772));break;}case 321: _LL36E: {struct Cyc_List_List*_tmp773=Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4)));struct Cyc_Absyn_Stmt*_tmp774=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp773,
_tmp774));break;}case 322: _LL36F: {struct Cyc_List_List*_tmp775=Cyc_yyget_YY18(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 5)));struct Cyc_Absyn_Stmt*_tmp776=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(
union Cyc_YYSTYPE),yyvsp_offset - 4))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY9(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp775,
_tmp776));break;}case 323: _LL370:{struct _dyneither_ptr*_tmpE24;yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((
_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpE24)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
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
Cyc_YYSTYPE),yyvsp_offset));break;case 363: _LL398: {void*_tmp778=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line));yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(
_tmp778,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
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
yylsp_offset))).last_line))));break;case 367: _LL39C: {void*_tmp779=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(
_tmp779,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line))));break;}case 368: _LL39D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));break;case 369:
_LL39E:{struct Cyc_Absyn_StructField_struct _tmpE2A;struct _dyneither_ptr*_tmpE29;
struct Cyc_Absyn_StructField_struct*_tmpE28;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 3)))).f3,(void*)((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((
_tmpE28[0]=((_tmpE2A.tag=0,((_tmpE2A.f1=((_tmpE29=_cycalloc(sizeof(*_tmpE29)),((
_tmpE29[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),_tmpE29)))),_tmpE2A)))),_tmpE28)))),
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line))));}
break;case 370: _LL39F: {void*_tmp77D=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line));{struct Cyc_Absyn_TupleIndex_struct _tmpE2D;struct
Cyc_Absyn_TupleIndex_struct*_tmpE2C;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp(
_tmp77D,(void*)((_tmpE2C=_cycalloc_atomic(sizeof(*_tmpE2C)),((_tmpE2C[0]=((
_tmpE2D.tag=1,((_tmpE2D.f1=(unsigned int)(Cyc_yyget_Int_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)))).f2,
_tmpE2D)))),_tmpE2C)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
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
Cyc_YYSTYPE),yyvsp_offset)));{void*_tmp780=e->r;union Cyc_Absyn_Cnst _tmp782;
struct _tuple4 _tmp783;enum Cyc_Absyn_Sign _tmp784;char _tmp785;union Cyc_Absyn_Cnst
_tmp787;struct _tuple5 _tmp788;enum Cyc_Absyn_Sign _tmp789;short _tmp78A;union Cyc_Absyn_Cnst
_tmp78C;struct _tuple6 _tmp78D;enum Cyc_Absyn_Sign _tmp78E;int _tmp78F;union Cyc_Absyn_Cnst
_tmp791;struct _dyneither_ptr _tmp792;union Cyc_Absyn_Cnst _tmp794;int _tmp795;union
Cyc_Absyn_Cnst _tmp797;struct _dyneither_ptr _tmp798;union Cyc_Absyn_Cnst _tmp79A;
struct _tuple7 _tmp79B;_LL3A7: {struct Cyc_Absyn_Const_e_struct*_tmp781=(struct Cyc_Absyn_Const_e_struct*)
_tmp780;if(_tmp781->tag != 0)goto _LL3A9;else{_tmp782=_tmp781->f1;if((_tmp782.Char_c).tag
!= 2)goto _LL3A9;_tmp783=(struct _tuple4)(_tmp782.Char_c).val;_tmp784=_tmp783.f1;
_tmp785=_tmp783.f2;}}_LL3A8:{struct Cyc_Absyn_Char_p_struct _tmpE30;struct Cyc_Absyn_Char_p_struct*
_tmpE2F;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE2F=_cycalloc_atomic(
sizeof(*_tmpE2F)),((_tmpE2F[0]=((_tmpE30.tag=10,((_tmpE30.f1=_tmp785,_tmpE30)))),
_tmpE2F)))),e->loc));}goto _LL3A6;_LL3A9: {struct Cyc_Absyn_Const_e_struct*_tmp786=(
struct Cyc_Absyn_Const_e_struct*)_tmp780;if(_tmp786->tag != 0)goto _LL3AB;else{
_tmp787=_tmp786->f1;if((_tmp787.Short_c).tag != 3)goto _LL3AB;_tmp788=(struct
_tuple5)(_tmp787.Short_c).val;_tmp789=_tmp788.f1;_tmp78A=_tmp788.f2;}}_LL3AA:{
struct Cyc_Absyn_Int_p_struct _tmpE33;struct Cyc_Absyn_Int_p_struct*_tmpE32;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE32=_cycalloc_atomic(sizeof(*_tmpE32)),((
_tmpE32[0]=((_tmpE33.tag=9,((_tmpE33.f1=_tmp789,((_tmpE33.f2=(int)_tmp78A,
_tmpE33)))))),_tmpE32)))),e->loc));}goto _LL3A6;_LL3AB: {struct Cyc_Absyn_Const_e_struct*
_tmp78B=(struct Cyc_Absyn_Const_e_struct*)_tmp780;if(_tmp78B->tag != 0)goto _LL3AD;
else{_tmp78C=_tmp78B->f1;if((_tmp78C.Int_c).tag != 4)goto _LL3AD;_tmp78D=(struct
_tuple6)(_tmp78C.Int_c).val;_tmp78E=_tmp78D.f1;_tmp78F=_tmp78D.f2;}}_LL3AC:{
struct Cyc_Absyn_Int_p_struct _tmpE36;struct Cyc_Absyn_Int_p_struct*_tmpE35;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE35=_cycalloc_atomic(sizeof(*_tmpE35)),((
_tmpE35[0]=((_tmpE36.tag=9,((_tmpE36.f1=_tmp78E,((_tmpE36.f2=_tmp78F,_tmpE36)))))),
_tmpE35)))),e->loc));}goto _LL3A6;_LL3AD: {struct Cyc_Absyn_Const_e_struct*_tmp790=(
struct Cyc_Absyn_Const_e_struct*)_tmp780;if(_tmp790->tag != 0)goto _LL3AF;else{
_tmp791=_tmp790->f1;if((_tmp791.Float_c).tag != 6)goto _LL3AF;_tmp792=(struct
_dyneither_ptr)(_tmp791.Float_c).val;}}_LL3AE:{struct Cyc_Absyn_Float_p_struct
_tmpE39;struct Cyc_Absyn_Float_p_struct*_tmpE38;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38[0]=((_tmpE39.tag=11,((
_tmpE39.f1=_tmp792,_tmpE39)))),_tmpE38)))),e->loc));}goto _LL3A6;_LL3AF: {struct
Cyc_Absyn_Const_e_struct*_tmp793=(struct Cyc_Absyn_Const_e_struct*)_tmp780;if(
_tmp793->tag != 0)goto _LL3B1;else{_tmp794=_tmp793->f1;if((_tmp794.Null_c).tag != 1)
goto _LL3B1;_tmp795=(int)(_tmp794.Null_c).val;}}_LL3B0: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A6;_LL3B1: {struct Cyc_Absyn_Const_e_struct*
_tmp796=(struct Cyc_Absyn_Const_e_struct*)_tmp780;if(_tmp796->tag != 0)goto _LL3B3;
else{_tmp797=_tmp796->f1;if((_tmp797.String_c).tag != 7)goto _LL3B3;_tmp798=(
struct _dyneither_ptr)(_tmp797.String_c).val;}}_LL3B2:{const char*_tmpE3A;Cyc_Parse_err(((
_tmpE3A="strings cannot occur within patterns",_tag_dyneither(_tmpE3A,sizeof(
char),37))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line));}goto _LL3A6;_LL3B3: {struct Cyc_Absyn_Const_e_struct*
_tmp799=(struct Cyc_Absyn_Const_e_struct*)_tmp780;if(_tmp799->tag != 0)goto _LL3B5;
else{_tmp79A=_tmp799->f1;if((_tmp79A.LongLong_c).tag != 5)goto _LL3B5;_tmp79B=(
struct _tuple7)(_tmp79A.LongLong_c).val;}}_LL3B4:{const char*_tmpE3B;Cyc_Parse_unimp(((
_tmpE3B="long long's in patterns",_tag_dyneither(_tmpE3B,sizeof(char),24))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));}goto _LL3A6;_LL3B5:;
_LL3B6: {const char*_tmpE3C;Cyc_Parse_err(((_tmpE3C="bad constant in case",
_tag_dyneither(_tmpE3C,sizeof(char),21))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line));}_LL3A6:;}break;}case
376: _LL3A5:{struct Cyc_Absyn_UnknownId_p_struct _tmpE3F;struct Cyc_Absyn_UnknownId_p_struct*
_tmpE3E;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE3E=_cycalloc(sizeof(*
_tmpE3E)),((_tmpE3E[0]=((_tmpE3F.tag=14,((_tmpE3F.f1=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpE3F)))),_tmpE3E)))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 377:
_LL3B7:{const char*_tmpE40;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpE40="as",_tag_dyneither(_tmpE40,sizeof(char),3))))!= 0){
const char*_tmpE41;Cyc_Parse_err(((_tmpE41="expecting `as'",_tag_dyneither(
_tmpE41,sizeof(char),15))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));}}{struct Cyc_Absyn_Var_p_struct _tmpE4B;struct
_dyneither_ptr*_tmpE4A;struct _tuple1*_tmpE49;struct Cyc_Absyn_Var_p_struct*
_tmpE48;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE48=_cycalloc(sizeof(*
_tmpE48)),((_tmpE48[0]=((_tmpE4B.tag=1,((_tmpE4B.f1=Cyc_Absyn_new_vardecl(((
_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49->f1=Cyc_Absyn_Loc_n,((_tmpE49->f2=((
_tmpE4A=_cycalloc(sizeof(*_tmpE4A)),((_tmpE4A[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),_tmpE4A)))),_tmpE49)))))),(void*)& Cyc_Absyn_VoidType_val,0),((
_tmpE4B.f2=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE4B)))))),_tmpE48)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line)));}break;case 378:
_LL3B8: {struct Cyc_List_List*_tmp7B0;int _tmp7B1;struct _tuple19 _tmp7AF=*Cyc_yyget_YY12(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));_tmp7B0=_tmp7AF.f1;_tmp7B1=_tmp7AF.f2;{struct Cyc_Absyn_Tuple_p_struct
_tmpE4E;struct Cyc_Absyn_Tuple_p_struct*_tmpE4D;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE4D=_cycalloc(sizeof(*_tmpE4D)),((_tmpE4D[0]=((_tmpE4E.tag=4,((
_tmpE4E.f1=_tmp7B0,((_tmpE4E.f2=_tmp7B1,_tmpE4E)))))),_tmpE4D)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 379:
_LL3B9: {struct Cyc_List_List*_tmp7B5;int _tmp7B6;struct _tuple19 _tmp7B4=*Cyc_yyget_YY12(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));_tmp7B5=_tmp7B4.f1;_tmp7B6=_tmp7B4.f2;{struct Cyc_Absyn_UnknownCall_p_struct
_tmpE51;struct Cyc_Absyn_UnknownCall_p_struct*_tmpE50;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE50=_cycalloc(sizeof(*_tmpE50)),((_tmpE50[0]=((_tmpE51.tag=15,((
_tmpE51.f1=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((_tmpE51.f2=_tmp7B5,((_tmpE51.f3=
_tmp7B6,_tmpE51)))))))),_tmpE50)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;}case 380: _LL3BA: {struct Cyc_List_List*
_tmp7BA;int _tmp7BB;struct _tuple19 _tmp7B9=*Cyc_yyget_YY16(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)));
_tmp7BA=_tmp7B9.f1;_tmp7BB=_tmp7B9.f2;{struct Cyc_List_List*_tmp7BC=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line),Cyc_yyget_YY40(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))));{struct Cyc_Absyn_Aggr_p_struct _tmpE57;struct Cyc_Absyn_AggrInfo*
_tmpE56;struct Cyc_Absyn_Aggr_p_struct*_tmpE55;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE55=_cycalloc(sizeof(*_tmpE55)),((_tmpE55[0]=((_tmpE57.tag=6,((
_tmpE57.f1=((_tmpE56=_cycalloc(sizeof(*_tmpE56)),((_tmpE56->aggr_info=Cyc_Absyn_UnknownAggr(
Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),0),((
_tmpE56->targs=0,_tmpE56)))))),((_tmpE57.f2=_tmp7BC,((_tmpE57.f3=_tmp7BA,((
_tmpE57.f4=_tmp7BB,_tmpE57)))))))))),_tmpE55)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;};}case 381:
_LL3BB: {struct Cyc_List_List*_tmp7C1;int _tmp7C2;struct _tuple19 _tmp7C0=*Cyc_yyget_YY16(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1)));_tmp7C1=_tmp7C0.f1;_tmp7C2=_tmp7C0.f2;{struct Cyc_List_List*
_tmp7C3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line),
Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))));{struct Cyc_Absyn_Aggr_p_struct _tmpE5A;struct Cyc_Absyn_Aggr_p_struct*
_tmpE59;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE59=_cycalloc(sizeof(*
_tmpE59)),((_tmpE59[0]=((_tmpE5A.tag=6,((_tmpE5A.f1=0,((_tmpE5A.f2=_tmp7C3,((
_tmpE5A.f3=_tmp7C1,((_tmpE5A.f4=_tmp7C2,_tmpE5A)))))))))),_tmpE59)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;};}case 382:
_LL3BC:{struct Cyc_Absyn_Pointer_p_struct _tmpE5D;struct Cyc_Absyn_Pointer_p_struct*
_tmpE5C;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE5C=_cycalloc(sizeof(*
_tmpE5C)),((_tmpE5C[0]=((_tmpE5D.tag=5,((_tmpE5D.f1=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE5D)))),
_tmpE5C)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 383: _LL3BD:{struct Cyc_Absyn_Pointer_p_struct
_tmpE67;struct Cyc_Absyn_Pointer_p_struct _tmpE66;struct Cyc_Absyn_Pointer_p_struct*
_tmpE65;struct Cyc_Absyn_Pointer_p_struct*_tmpE64;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE64=_cycalloc(sizeof(*_tmpE64)),((_tmpE64[0]=((_tmpE67.tag=5,((
_tmpE67.f1=Cyc_Absyn_new_pat((void*)((_tmpE65=_cycalloc(sizeof(*_tmpE65)),((
_tmpE65[0]=((_tmpE66.tag=5,((_tmpE66.f1=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE66)))),
_tmpE65)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),_tmpE67)))),_tmpE64)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 384:
_LL3BE:{struct Cyc_Absyn_Reference_p_struct _tmpE71;struct _dyneither_ptr*_tmpE70;
struct _tuple1*_tmpE6F;struct Cyc_Absyn_Reference_p_struct*_tmpE6E;yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)((_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E[0]=((
_tmpE71.tag=2,((_tmpE71.f1=Cyc_Absyn_new_vardecl(((_tmpE6F=_cycalloc(sizeof(*
_tmpE6F)),((_tmpE6F->f1=Cyc_Absyn_Loc_n,((_tmpE6F->f2=((_tmpE70=_cycalloc(
sizeof(*_tmpE70)),((_tmpE70[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE70)))),
_tmpE6F)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE71.f2=Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)),_tmpE71)))))),_tmpE6E)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 385:
_LL3BF:{const char*_tmpE72;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpE72="as",_tag_dyneither(_tmpE72,sizeof(char),3))))!= 0){
const char*_tmpE73;Cyc_Parse_err(((_tmpE73="expecting `as'",_tag_dyneither(
_tmpE73,sizeof(char),15))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));}}{struct Cyc_Absyn_Reference_p_struct _tmpE7D;
struct _dyneither_ptr*_tmpE7C;struct _tuple1*_tmpE7B;struct Cyc_Absyn_Reference_p_struct*
_tmpE7A;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE7A=_cycalloc(sizeof(*
_tmpE7A)),((_tmpE7A[0]=((_tmpE7D.tag=2,((_tmpE7D.f1=Cyc_Absyn_new_vardecl(((
_tmpE7B=_cycalloc(sizeof(*_tmpE7B)),((_tmpE7B->f1=Cyc_Absyn_Loc_n,((_tmpE7B->f2=((
_tmpE7C=_cycalloc(sizeof(*_tmpE7C)),((_tmpE7C[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),_tmpE7C)))),_tmpE7B)))))),(void*)& Cyc_Absyn_VoidType_val,0),((
_tmpE7D.f2=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpE7D)))))),_tmpE7A)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line)));}break;case 386:
_LL3C0: {void*_tmp7D6=Cyc_Parse_id2type(Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Tcutil_kind_to_bound(&
Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct _tmpE8E;struct Cyc_Absyn_TagType_struct
_tmpE8D;struct Cyc_Absyn_TagType_struct*_tmpE8C;struct _dyneither_ptr*_tmpE8B;
struct _tuple1*_tmpE8A;struct Cyc_Absyn_TagInt_p_struct*_tmpE89;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE89=_cycalloc(sizeof(*_tmpE89)),((_tmpE89[0]=((_tmpE8E.tag=3,((
_tmpE8E.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line),_tmp7D6),((_tmpE8E.f2=Cyc_Absyn_new_vardecl(((
_tmpE8A=_cycalloc(sizeof(*_tmpE8A)),((_tmpE8A->f1=Cyc_Absyn_Loc_n,((_tmpE8A->f2=((
_tmpE8B=_cycalloc(sizeof(*_tmpE8B)),((_tmpE8B[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),_tmpE8B)))),_tmpE8A)))))),(void*)((_tmpE8C=_cycalloc(sizeof(*
_tmpE8C)),((_tmpE8C[0]=((_tmpE8D.tag=20,((_tmpE8D.f1=(void*)_tmp7D6,_tmpE8D)))),
_tmpE8C)))),0),_tmpE8E)))))),_tmpE89)))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 387:
_LL3C1: {struct Cyc_Absyn_Tvar*_tmp7DD=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(&
Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct _tmpEAA;struct Cyc_Absyn_TagType_struct
_tmpEA9;struct Cyc_Absyn_VarType_struct _tmpEA8;struct Cyc_Absyn_VarType_struct*
_tmpEA7;struct Cyc_Absyn_TagType_struct*_tmpEA6;struct _dyneither_ptr*_tmpEA5;
struct _tuple1*_tmpEA4;struct Cyc_Absyn_TagInt_p_struct*_tmpEA3;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpEA3=_cycalloc(sizeof(*_tmpEA3)),((_tmpEA3[0]=((_tmpEAA.tag=3,((
_tmpEAA.f1=_tmp7DD,((_tmpEAA.f2=Cyc_Absyn_new_vardecl(((_tmpEA4=_cycalloc(
sizeof(*_tmpEA4)),((_tmpEA4->f1=Cyc_Absyn_Loc_n,((_tmpEA4->f2=((_tmpEA5=
_cycalloc(sizeof(*_tmpEA5)),((_tmpEA5[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),
_tmpEA5)))),_tmpEA4)))))),(void*)((_tmpEA6=_cycalloc(sizeof(*_tmpEA6)),((_tmpEA6[
0]=((_tmpEA9.tag=20,((_tmpEA9.f1=(void*)((void*)((_tmpEA7=_cycalloc(sizeof(*
_tmpEA7)),((_tmpEA7[0]=((_tmpEA8.tag=2,((_tmpEA8.f1=_tmp7DD,_tmpEA8)))),_tmpEA7))))),
_tmpEA9)))),_tmpEA6)))),0),_tmpEAA)))))),_tmpEA3)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 388:
_LL3C2:{struct _tuple19*_tmpEAB;yyval=Cyc_YY12(((_tmpEAB=_cycalloc(sizeof(*
_tmpEAB)),((_tmpEAB->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)))),((_tmpEAB->f2=0,_tmpEAB)))))));}break;case 389:
_LL3C3:{struct _tuple19*_tmpEAC;yyval=Cyc_YY12(((_tmpEAC=_cycalloc(sizeof(*
_tmpEAC)),((_tmpEAC->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2)))),((_tmpEAC->f2=1,_tmpEAC)))))));}break;case 390:
_LL3C4:{struct _tuple19*_tmpEAD;yyval=Cyc_YY12(((_tmpEAD=_cycalloc(sizeof(*
_tmpEAD)),((_tmpEAD->f1=0,((_tmpEAD->f2=1,_tmpEAD)))))));}break;case 391: _LL3C5:{
struct Cyc_List_List*_tmpEAE;yyval=Cyc_YY13(((_tmpEAE=_cycalloc(sizeof(*_tmpEAE)),((
_tmpEAE->hd=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((_tmpEAE->tl=0,_tmpEAE)))))));}break;
case 392: _LL3C6:{struct Cyc_List_List*_tmpEAF;yyval=Cyc_YY13(((_tmpEAF=_cycalloc(
sizeof(*_tmpEAF)),((_tmpEAF->hd=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),((
_tmpEAF->tl=Cyc_yyget_YY13(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),_tmpEAF)))))));}break;case 393: _LL3C7:{
struct _tuple20*_tmpEB0;yyval=Cyc_YY14(((_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((
_tmpEB0->f1=0,((_tmpEB0->f2=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpEB0)))))));}
break;case 394: _LL3C8:{struct _tuple20*_tmpEB1;yyval=Cyc_YY14(((_tmpEB1=_cycalloc(
sizeof(*_tmpEB1)),((_tmpEB1->f1=Cyc_yyget_YY41(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpEB1->f2=Cyc_yyget_YY11(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpEB1)))))));}break;case 395: _LL3C9:{
struct _tuple19*_tmpEB2;yyval=Cyc_YY16(((_tmpEB2=_cycalloc(sizeof(*_tmpEB2)),((
_tmpEB2->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))),((_tmpEB2->f2=0,_tmpEB2)))))));}break;case 396: _LL3CA:{struct
_tuple19*_tmpEB3;yyval=Cyc_YY16(((_tmpEB3=_cycalloc(sizeof(*_tmpEB3)),((_tmpEB3->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2)))),((_tmpEB3->f2=1,_tmpEB3)))))));}break;case 397: _LL3CB:{struct
_tuple19*_tmpEB4;yyval=Cyc_YY16(((_tmpEB4=_cycalloc(sizeof(*_tmpEB4)),((_tmpEB4->f1=
0,((_tmpEB4->f2=1,_tmpEB4)))))));}break;case 398: _LL3CC:{struct Cyc_List_List*
_tmpEB5;yyval=Cyc_YY15(((_tmpEB5=_cycalloc(sizeof(*_tmpEB5)),((_tmpEB5->hd=Cyc_yyget_YY14(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpEB5->tl=0,_tmpEB5)))))));}break;case 399: _LL3CD:{struct Cyc_List_List*
_tmpEB6;yyval=Cyc_YY15(((_tmpEB6=_cycalloc(sizeof(*_tmpEB6)),((_tmpEB6->hd=Cyc_yyget_YY14(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpEB6->tl=Cyc_yyget_YY15(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpEB6)))))));}break;case 400: _LL3CE: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 401: _LL3CF: yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 402: _LL3D0: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 403: _LL3D1: yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY8(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 404: _LL3D2:
yyval=Cyc_YY3(Cyc_Absyn_swap_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 405: _LL3D3: yyval=Cyc_YY8(0);break;case 406:
_LL3D4:{struct Cyc_Core_Opt*_tmpEB7;yyval=Cyc_YY8(((_tmpEB7=_cycalloc_atomic(
sizeof(*_tmpEB7)),((_tmpEB7->v=(void*)Cyc_Absyn_Times,_tmpEB7)))));}break;case
407: _LL3D5:{struct Cyc_Core_Opt*_tmpEB8;yyval=Cyc_YY8(((_tmpEB8=_cycalloc_atomic(
sizeof(*_tmpEB8)),((_tmpEB8->v=(void*)Cyc_Absyn_Div,_tmpEB8)))));}break;case 408:
_LL3D6:{struct Cyc_Core_Opt*_tmpEB9;yyval=Cyc_YY8(((_tmpEB9=_cycalloc_atomic(
sizeof(*_tmpEB9)),((_tmpEB9->v=(void*)Cyc_Absyn_Mod,_tmpEB9)))));}break;case 409:
_LL3D7:{struct Cyc_Core_Opt*_tmpEBA;yyval=Cyc_YY8(((_tmpEBA=_cycalloc_atomic(
sizeof(*_tmpEBA)),((_tmpEBA->v=(void*)Cyc_Absyn_Plus,_tmpEBA)))));}break;case 410:
_LL3D8:{struct Cyc_Core_Opt*_tmpEBB;yyval=Cyc_YY8(((_tmpEBB=_cycalloc_atomic(
sizeof(*_tmpEBB)),((_tmpEBB->v=(void*)Cyc_Absyn_Minus,_tmpEBB)))));}break;case
411: _LL3D9:{struct Cyc_Core_Opt*_tmpEBC;yyval=Cyc_YY8(((_tmpEBC=_cycalloc_atomic(
sizeof(*_tmpEBC)),((_tmpEBC->v=(void*)Cyc_Absyn_Bitlshift,_tmpEBC)))));}break;
case 412: _LL3DA:{struct Cyc_Core_Opt*_tmpEBD;yyval=Cyc_YY8(((_tmpEBD=
_cycalloc_atomic(sizeof(*_tmpEBD)),((_tmpEBD->v=(void*)Cyc_Absyn_Bitlrshift,
_tmpEBD)))));}break;case 413: _LL3DB:{struct Cyc_Core_Opt*_tmpEBE;yyval=Cyc_YY8(((
_tmpEBE=_cycalloc_atomic(sizeof(*_tmpEBE)),((_tmpEBE->v=(void*)Cyc_Absyn_Bitand,
_tmpEBE)))));}break;case 414: _LL3DC:{struct Cyc_Core_Opt*_tmpEBF;yyval=Cyc_YY8(((
_tmpEBF=_cycalloc_atomic(sizeof(*_tmpEBF)),((_tmpEBF->v=(void*)Cyc_Absyn_Bitxor,
_tmpEBF)))));}break;case 415: _LL3DD:{struct Cyc_Core_Opt*_tmpEC0;yyval=Cyc_YY8(((
_tmpEC0=_cycalloc_atomic(sizeof(*_tmpEC0)),((_tmpEC0->v=(void*)Cyc_Absyn_Bitor,
_tmpEC0)))));}break;case 416: _LL3DE: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 417: _LL3DF: yyval=Cyc_YY3(Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 418: _LL3E0:
yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 419: _LL3E1:
yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 420: _LL3E2:
yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 421: _LL3E3:
yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 422: _LL3E4: yyval=Cyc_YY3(Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 423: _LL3E5: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 424: _LL3E6: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 425: _LL3E7: yyval=Cyc_YY3(Cyc_Absyn_or_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 426: _LL3E8:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 427: _LL3E9: yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 428: _LL3EA:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 429: _LL3EB: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 430: _LL3EC:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 431: _LL3ED: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 432: _LL3EE:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 433: _LL3EF: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 434: _LL3F0:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 435: _LL3F1: yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 436: _LL3F2:
yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 437: _LL3F3: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 438: _LL3F4: yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 439: _LL3F5: yyval=Cyc_YY3(Cyc_Absyn_gt_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 440: _LL3F6:
yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 441: _LL3F7: yyval=Cyc_YY3(Cyc_Absyn_gte_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 442: _LL3F8:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 443: _LL3F9: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 444: _LL3FA:
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 445: _LL3FB: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 446: _LL3FC: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 447: _LL3FD:
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 448: _LL3FE: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 449: _LL3FF: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 450: _LL400:
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 451: _LL401: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(
Cyc_Absyn_Mod,Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 452: _LL402:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 453: _LL403: {void*_tmp7FC=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line));yyval=Cyc_YY3(Cyc_Absyn_cast_exp(_tmp7FC,Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),1,Cyc_Absyn_Unknown_coercion,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;}case 454:
_LL404: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset));break;case 455: _LL405: yyval=Cyc_YY3(Cyc_Absyn_pre_inc_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 456: _LL406: yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 457: _LL407: yyval=Cyc_YY3(Cyc_Absyn_address_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 458: _LL408: yyval=Cyc_YY3(Cyc_Absyn_deref_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 459: _LL409: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 460: _LL40A: yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 461: _LL40B: {void*_tmp7FD=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).last_line));yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp(_tmp7FD,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;}case 462:
_LL40C: yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 463: _LL40D: {
void*_tmp7FE=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line));{struct Cyc_Absyn_StructField_struct
_tmpEC6;struct _dyneither_ptr*_tmpEC5;struct Cyc_Absyn_StructField_struct*_tmpEC4;
yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(_tmp7FE,(void*)((_tmpEC4=_cycalloc(sizeof(*
_tmpEC4)),((_tmpEC4[0]=((_tmpEC6.tag=0,((_tmpEC6.f1=((_tmpEC5=_cycalloc(sizeof(*
_tmpEC5)),((_tmpEC5[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),
_tmpEC5)))),_tmpEC6)))),_tmpEC4)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;}case 464: _LL40E: {void*_tmp802=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 3))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).last_line));{struct Cyc_Absyn_TupleIndex_struct _tmpEC9;struct
Cyc_Absyn_TupleIndex_struct*_tmpEC8;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(_tmp802,(
void*)((_tmpEC8=_cycalloc_atomic(sizeof(*_tmpEC8)),((_tmpEC8[0]=((_tmpEC9.tag=1,((
_tmpEC9.f1=(unsigned int)(Cyc_yyget_Int_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)))).f2,
_tmpEC9)))),_tmpEC8)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;}case 465: _LL40F:{struct Cyc_Absyn_Malloc_e_struct
_tmpECF;struct Cyc_Absyn_MallocInfo _tmpECE;struct Cyc_Absyn_Malloc_e_struct*
_tmpECD;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpECD=_cycalloc(sizeof(*
_tmpECD)),((_tmpECD[0]=((_tmpECF.tag=34,((_tmpECF.f1=((_tmpECE.is_calloc=0,((
_tmpECE.rgn=0,((_tmpECE.elt_type=0,((_tmpECE.num_elts=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((
_tmpECE.fat_result=0,_tmpECE)))))))))),_tmpECF)))),_tmpECD)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 466:
_LL410:{struct Cyc_Absyn_Malloc_e_struct _tmpED5;struct Cyc_Absyn_MallocInfo _tmpED4;
struct Cyc_Absyn_Malloc_e_struct*_tmpED3;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpED3=_cycalloc(sizeof(*_tmpED3)),((_tmpED3[0]=((_tmpED5.tag=34,((_tmpED5.f1=((
_tmpED4.is_calloc=0,((_tmpED4.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),((_tmpED4.elt_type=0,((_tmpED4.num_elts=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),((_tmpED4.fat_result=0,_tmpED4)))))))))),_tmpED5)))),_tmpED3)))),
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}
break;case 467: _LL411: {void*_tmp80B=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).last_line));{struct Cyc_Absyn_Malloc_e_struct _tmpEDF;void**
_tmpEDE;struct Cyc_Absyn_MallocInfo _tmpEDD;struct Cyc_Absyn_Malloc_e_struct*
_tmpEDC;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEDC=_cycalloc(sizeof(*
_tmpEDC)),((_tmpEDC[0]=((_tmpEDF.tag=34,((_tmpEDF.f1=((_tmpEDD.is_calloc=1,((
_tmpEDD.rgn=0,((_tmpEDD.elt_type=((_tmpEDE=_cycalloc(sizeof(*_tmpEDE)),((_tmpEDE[
0]=_tmp80B,_tmpEDE)))),((_tmpEDD.num_elts=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6))),((
_tmpEDD.fat_result=0,_tmpEDD)))))))))),_tmpEDF)))),_tmpEDC)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 8))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 468:
_LL412: {void*_tmp810=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).last_line));{struct Cyc_Absyn_Malloc_e_struct
_tmpEE9;void**_tmpEE8;struct Cyc_Absyn_MallocInfo _tmpEE7;struct Cyc_Absyn_Malloc_e_struct*
_tmpEE6;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEE6=_cycalloc(sizeof(*
_tmpEE6)),((_tmpEE6[0]=((_tmpEE9.tag=34,((_tmpEE9.f1=((_tmpEE7.is_calloc=1,((
_tmpEE7.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8))),((
_tmpEE7.elt_type=((_tmpEE8=_cycalloc(sizeof(*_tmpEE8)),((_tmpEE8[0]=_tmp810,
_tmpEE8)))),((_tmpEE7.num_elts=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6))),((
_tmpEE7.fat_result=0,_tmpEE7)))))))))),_tmpEE9)))),_tmpEE6)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 10))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;}case 469:
_LL413:{struct Cyc_Absyn_Exp*_tmpEEA[1];yyval=Cyc_YY3(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((
_tmpEEA[0]=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEEA,sizeof(struct Cyc_Absyn_Exp*),
1)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 470: _LL414:{struct Cyc_Absyn_Tagcheck_e_struct
_tmpEF0;struct _dyneither_ptr*_tmpEEF;struct Cyc_Absyn_Tagcheck_e_struct*_tmpEEE;
yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEEE=_cycalloc(sizeof(*_tmpEEE)),((
_tmpEEE[0]=((_tmpEF0.tag=38,((_tmpEF0.f1=Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),((
_tmpEF0.f2=((_tmpEEF=_cycalloc(sizeof(*_tmpEEF)),((_tmpEEF[0]=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpEEF)))),_tmpEF0)))))),_tmpEEE)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 471:
_LL415:{struct Cyc_Absyn_Tagcheck_e_struct _tmpEF6;struct _dyneither_ptr*_tmpEF5;
struct Cyc_Absyn_Tagcheck_e_struct*_tmpEF4;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpEF4=_cycalloc(sizeof(*_tmpEF4)),((_tmpEF4[0]=((_tmpEF6.tag=38,((_tmpEF6.f1=
Cyc_Absyn_deref_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).last_line)),((_tmpEF6.f2=((
_tmpEF5=_cycalloc(sizeof(*_tmpEF5)),((_tmpEF5[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1))),_tmpEF5)))),_tmpEF6)))))),_tmpEF4)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 5))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 472:
_LL416: {void*_tmp81C=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 1))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line));yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(
_tmp81C,Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;}case 473: _LL417: {int _tmp81E;struct
_dyneither_ptr _tmp81F;struct _tuple17 _tmp81D=Cyc_yyget_Asm_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)));_tmp81E=
_tmp81D.f1;_tmp81F=_tmp81D.f2;yyval=Cyc_YY3(Cyc_Absyn_asm_exp(_tmp81E,_tmp81F,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));
break;}case 474: _LL418: yyval=Cyc_YY7(Cyc_Absyn_Bitnot);break;case 475: _LL419: yyval=
Cyc_YY7(Cyc_Absyn_Not);break;case 476: _LL41A: yyval=Cyc_YY7(Cyc_Absyn_Minus);
break;case 477: _LL41B: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 478: _LL41C: yyval=Cyc_YY3(Cyc_Absyn_subscript_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 3))),Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 479: _LL41D:
yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),0,
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));
break;case 480: _LL41E: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 3))),Cyc_yyget_YY5(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 481: _LL41F:{
struct _dyneither_ptr*_tmpEF7;yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 2))),((_tmpEF7=_cycalloc(sizeof(*_tmpEF7)),((_tmpEF7[0]=Cyc_yyget_String_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpEF7)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 482: _LL420:{struct _dyneither_ptr*_tmpEF8;
yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((
_tmpEF8=_cycalloc(sizeof(*_tmpEF8)),((_tmpEF8[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpEF8)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 483: _LL421: yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 484: _LL422: yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 485: _LL423:{struct Cyc_Absyn_CompoundLit_e_struct
_tmpEFB;struct Cyc_Absyn_CompoundLit_e_struct*_tmpEFA;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpEFA=_cycalloc(sizeof(*_tmpEFA)),((_tmpEFA[0]=((_tmpEFB.tag=26,((
_tmpEFB.f1=Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4))),((_tmpEFB.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)))),
_tmpEFB)))))),_tmpEFA)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 486: _LL424:{struct Cyc_Absyn_CompoundLit_e_struct
_tmpEFE;struct Cyc_Absyn_CompoundLit_e_struct*_tmpEFD;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpEFD=_cycalloc(sizeof(*_tmpEFD)),((_tmpEFD[0]=((_tmpEFE.tag=26,((
_tmpEFE.f1=Cyc_yyget_YY37(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5))),((_tmpEFE.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2)))),
_tmpEFE)))))),_tmpEFD)))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));}break;case 487: _LL425:{struct Cyc_Absyn_UnknownId_e_struct
_tmpF01;struct Cyc_Absyn_UnknownId_e_struct*_tmpF00;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpF00=_cycalloc(sizeof(*_tmpF00)),((_tmpF00[0]=((_tmpF01.tag=2,((
_tmpF01.f1=Cyc_yyget_QualId_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpF01)))),_tmpF00)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}break;case 488:
_LL426: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset));break;case 489: _LL427: yyval=Cyc_YY3(Cyc_Absyn_string_exp(
Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((struct
Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 490: _LL428:
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 1));break;case 491: _LL429: yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 492: _LL42A: yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(
Cyc_yyget_YY3(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 4))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1)))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 493: _LL42B:
yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 3))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 494: _LL42C:{
struct Cyc_Absyn_Aggregate_e_struct _tmpF04;struct Cyc_Absyn_Aggregate_e_struct*
_tmpF03;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpF03=_cycalloc(sizeof(*
_tmpF03)),((_tmpF03[0]=((_tmpF04.tag=29,((_tmpF04.f1=Cyc_yyget_QualId_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset - 4))),((_tmpF04.f2=Cyc_yyget_YY40(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),((
_tmpF04.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset - 1)))),((_tmpF04.f4=0,_tmpF04)))))))))),_tmpF03)))),
Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));}
break;case 495: _LL42D: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset - 4))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 496: _LL42E:
yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union
Cyc_YYSTYPE),yyvsp_offset)))));break;case 497: _LL42F:{struct Cyc_List_List*_tmpF05;
yyval=Cyc_YY5(((_tmpF05=_cycalloc(sizeof(*_tmpF05)),((_tmpF05->hd=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpF05->tl=0,_tmpF05)))))));}break;case 498: _LL430:{struct Cyc_List_List*
_tmpF06;yyval=Cyc_YY5(((_tmpF06=_cycalloc(sizeof(*_tmpF06)),((_tmpF06->hd=Cyc_yyget_YY3(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),((_tmpF06->tl=Cyc_yyget_YY5(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2))),
_tmpF06)))))));}break;case 499: _LL431: yyval=Cyc_YY3(Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(*((
union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset)))).f1,(Cyc_yyget_Int_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset)))).f2,Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 500: _LL432:
yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 501: _LL433:
yyval=Cyc_YY3(Cyc_Absyn_float_exp(Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).first_line,(*((struct Cyc_Yyltype*)_check_dyneither_subscript(
yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line)));break;case 502: _LL434:
yyval=Cyc_YY3(Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((*((struct Cyc_Yyltype*)
_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line,(*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),
yylsp_offset))).last_line)));break;case 503: _LL435:{struct _dyneither_ptr*_tmpF09;
struct _tuple1*_tmpF08;yyval=Cyc_QualId_tok(((_tmpF08=_cycalloc(sizeof(*_tmpF08)),((
_tmpF08->f1=Cyc_Absyn_Rel_n(0),((_tmpF08->f2=((_tmpF09=_cycalloc(sizeof(*_tmpF09)),((
_tmpF09[0]=Cyc_yyget_String_tok(*((union Cyc_YYSTYPE*)_check_dyneither_subscript(
yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset))),_tmpF09)))),_tmpF08)))))));}break;
case 504: _LL436: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 505: _LL437:{struct
_dyneither_ptr*_tmpF0C;struct _tuple1*_tmpF0B;yyval=Cyc_QualId_tok(((_tmpF0B=
_cycalloc(sizeof(*_tmpF0B)),((_tmpF0B->f1=Cyc_Absyn_Rel_n(0),((_tmpF0B->f2=((
_tmpF0C=_cycalloc(sizeof(*_tmpF0C)),((_tmpF0C[0]=Cyc_yyget_String_tok(*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset))),_tmpF0C)))),_tmpF0B)))))));}break;case 506: _LL438: yyval=*((union
Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),
yyvsp_offset));break;case 507: _LL439: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 508: _LL43A: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 509: _LL43B: yyval=*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;
case 510: _LL43C: yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,
sizeof(union Cyc_YYSTYPE),yyvsp_offset));break;case 511: _LL43D: break;case 512:
_LL43E: yylex_buf->lex_curr_pos -=1;break;default: _LL43F: break;}yyvsp_offset -=
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
_check_known_subscript_notnull(513,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(143,yyn - 144)]+ *((short*)
_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));if((yystate >= 0  && 
yystate <= 6244) && Cyc_yycheck[_check_known_subscript_notnull(6245,yystate)]== *((
short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))yystate=(int)
Cyc_yytable[_check_known_subscript_notnull(6245,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(143,yyn - 144)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1028,yystate)];if(yyn > - 32768  && yyn < 6244){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 287 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6245,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(287,x)])+ 15,count ++);}{unsigned int _tmpF11;
unsigned int _tmpF10;struct _dyneither_ptr _tmpF0F;char*_tmpF0E;unsigned int _tmpF0D;
msg=((_tmpF0D=(unsigned int)(sze + 15),((_tmpF0E=(char*)_region_malloc(yyregion,
_check_times(sizeof(char),_tmpF0D + 1)),((_tmpF0F=_tag_dyneither(_tmpF0E,sizeof(
char),_tmpF0D + 1),((((_tmpF10=_tmpF0D,((_tmpF12(& _tmpF11,& _tmpF10,& _tmpF0E),
_tmpF0E[_tmpF10]=(char)0)))),_tmpF0F))))))));}{const char*_tmpF13;Cyc_strcpy(msg,((
_tmpF13="parse error",_tag_dyneither(_tmpF13,sizeof(char),12))));}if(count < 5){
count=0;for(x=yyn < 0?- yyn: 0;x < 287 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6245,x + yyn)]== x){{const char*_tmpF15;const char*
_tmpF14;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)((
_tmpF15=", expecting `",_tag_dyneither(_tmpF15,sizeof(char),14))):(struct
_dyneither_ptr)((_tmpF14=" or `",_tag_dyneither(_tmpF14,sizeof(char),6)))));}Cyc_strcat(
msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(287,x)]);{
const char*_tmpF16;Cyc_strcat(msg,((_tmpF16="'",_tag_dyneither(_tmpF16,sizeof(
char),2))));}++ count;}}}Cyc_yyerror((struct _dyneither_ptr)msg);}else{const char*
_tmpF17;Cyc_yyerror(((_tmpF17="parse error",_tag_dyneither(_tmpF17,sizeof(char),
12))));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int
_tmp839=1;_npop_handler(0);return _tmp839;}Cyc_yychar=- 2;}yyerrstatus=3;goto
yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp83A=1;
_npop_handler(0);return _tmp83A;}-- yyvsp_offset;yystate=(int)*((short*)
_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));-- yylsp_offset;
yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1028,yystate)];
if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6244) || Cyc_yycheck[
_check_known_subscript_notnull(6245,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6245,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 1027){int
_tmp83B=0;_npop_handler(0);return _tmp83B;}*((union Cyc_YYSTYPE*)
_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=Cyc_yylval;*((
struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++
yylsp_offset))=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE _tmp844=v;struct _tuple6 _tmp845;int
_tmp846;char _tmp847;struct _dyneither_ptr _tmp848;struct _tuple1*_tmp849;struct
_tuple1 _tmp84A;union Cyc_Absyn_Nmspace _tmp84B;struct _dyneither_ptr*_tmp84C;_LL442:
if((_tmp844.Int_tok).tag != 1)goto _LL444;_tmp845=(struct _tuple6)(_tmp844.Int_tok).val;
_tmp846=_tmp845.f2;_LL443:{const char*_tmpF1B;void*_tmpF1A[1];struct Cyc_Int_pa_struct
_tmpF19;(_tmpF19.tag=1,((_tmpF19.f1=(unsigned long)_tmp846,((_tmpF1A[0]=& _tmpF19,
Cyc_fprintf(Cyc_stderr,((_tmpF1B="%d",_tag_dyneither(_tmpF1B,sizeof(char),3))),
_tag_dyneither(_tmpF1A,sizeof(void*),1)))))));}goto _LL441;_LL444: if((_tmp844.Char_tok).tag
!= 2)goto _LL446;_tmp847=(char)(_tmp844.Char_tok).val;_LL445:{const char*_tmpF1F;
void*_tmpF1E[1];struct Cyc_Int_pa_struct _tmpF1D;(_tmpF1D.tag=1,((_tmpF1D.f1=(
unsigned long)((int)_tmp847),((_tmpF1E[0]=& _tmpF1D,Cyc_fprintf(Cyc_stderr,((
_tmpF1F="%c",_tag_dyneither(_tmpF1F,sizeof(char),3))),_tag_dyneither(_tmpF1E,
sizeof(void*),1)))))));}goto _LL441;_LL446: if((_tmp844.String_tok).tag != 3)goto
_LL448;_tmp848=(struct _dyneither_ptr)(_tmp844.String_tok).val;_LL447:{const char*
_tmpF23;void*_tmpF22[1];struct Cyc_String_pa_struct _tmpF21;(_tmpF21.tag=0,((
_tmpF21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp848),((_tmpF22[0]=&
_tmpF21,Cyc_fprintf(Cyc_stderr,((_tmpF23="\"%s\"",_tag_dyneither(_tmpF23,sizeof(
char),5))),_tag_dyneither(_tmpF22,sizeof(void*),1)))))));}goto _LL441;_LL448: if((
_tmp844.QualId_tok).tag != 5)goto _LL44A;_tmp849=(struct _tuple1*)(_tmp844.QualId_tok).val;
_tmp84A=*_tmp849;_tmp84B=_tmp84A.f1;_tmp84C=_tmp84A.f2;_LL449: {struct Cyc_List_List*
_tmp856=0;{union Cyc_Absyn_Nmspace _tmp857=_tmp84B;struct Cyc_List_List*_tmp858;
struct Cyc_List_List*_tmp859;struct Cyc_List_List*_tmp85A;int _tmp85B;_LL44D: if((
_tmp857.Rel_n).tag != 1)goto _LL44F;_tmp858=(struct Cyc_List_List*)(_tmp857.Rel_n).val;
_LL44E: _tmp856=_tmp858;goto _LL44C;_LL44F: if((_tmp857.Abs_n).tag != 2)goto _LL451;
_tmp859=(struct Cyc_List_List*)(_tmp857.Abs_n).val;_LL450: _tmp856=_tmp859;goto
_LL44C;_LL451: if((_tmp857.C_n).tag != 3)goto _LL453;_tmp85A=(struct Cyc_List_List*)(
_tmp857.C_n).val;_LL452: _tmp856=_tmp85A;goto _LL44C;_LL453: if((_tmp857.Loc_n).tag
!= 4)goto _LL44C;_tmp85B=(int)(_tmp857.Loc_n).val;_LL454: goto _LL44C;_LL44C:;}for(
0;_tmp856 != 0;_tmp856=_tmp856->tl){const char*_tmpF27;void*_tmpF26[1];struct Cyc_String_pa_struct
_tmpF25;(_tmpF25.tag=0,((_tmpF25.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp856->hd)),((_tmpF26[0]=& _tmpF25,Cyc_fprintf(Cyc_stderr,((
_tmpF27="%s::",_tag_dyneither(_tmpF27,sizeof(char),5))),_tag_dyneither(_tmpF26,
sizeof(void*),1)))))));}{const char*_tmpF2B;void*_tmpF2A[1];struct Cyc_String_pa_struct
_tmpF29;(_tmpF29.tag=0,((_tmpF29.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp84C),((_tmpF2A[0]=& _tmpF29,Cyc_fprintf(Cyc_stderr,((_tmpF2B="%s::",
_tag_dyneither(_tmpF2B,sizeof(char),5))),_tag_dyneither(_tmpF2A,sizeof(void*),1)))))));}
goto _LL441;}_LL44A:;_LL44B:{const char*_tmpF2E;void*_tmpF2D;(_tmpF2D=0,Cyc_fprintf(
Cyc_stderr,((_tmpF2E="?",_tag_dyneither(_tmpF2E,sizeof(char),2))),_tag_dyneither(
_tmpF2D,sizeof(void*),0)));}goto _LL441;_LL441:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
