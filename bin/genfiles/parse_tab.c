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
_tmp857;struct Cyc_Position_Exit_struct*_tmp856;(int)_throw((void*)((_tmp856=
_cycalloc_atomic(sizeof(*_tmp856)),((_tmp856[0]=((_tmp857.tag=Cyc_Position_Exit,
_tmp857)),_tmp856)))));};}static void Cyc_Parse_unimp(struct _dyneither_ptr msg,
struct Cyc_Position_Segment*sg){{const char*_tmp85C;void*_tmp85B[2];struct Cyc_String_pa_struct
_tmp85A;struct Cyc_String_pa_struct _tmp859;(_tmp859.tag=0,((_tmp859.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp85A.tag=0,((_tmp85A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(sg)),((
_tmp85B[0]=& _tmp85A,((_tmp85B[1]=& _tmp859,Cyc_fprintf(Cyc_stderr,((_tmp85C="%s: Warning: Cyclone does not yet support %s\n",
_tag_dyneither(_tmp85C,sizeof(char),46))),_tag_dyneither(_tmp85B,sizeof(void*),2)))))))))))));}
return;}static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,struct Cyc_Position_Segment*
loc){struct _tuple8 _tmp7;struct Cyc_Absyn_Tqual _tmp8;void*_tmp9;struct _tuple8*
_tmp6=tqt;_tmp7=*_tmp6;_tmp8=_tmp7.f2;_tmp9=_tmp7.f3;if((_tmp8.print_const  || 
_tmp8.q_volatile) || _tmp8.q_restrict){if(_tmp8.loc != 0)loc=_tmp8.loc;{const char*
_tmp85F;void*_tmp85E;(_tmp85E=0,Cyc_Tcutil_warn(loc,((_tmp85F="qualifier on type is ignored",
_tag_dyneither(_tmp85F,sizeof(char),29))),_tag_dyneither(_tmp85E,sizeof(void*),0)));};}
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
_tmp862;struct Cyc_Absyn_Upper_b_struct*_tmp861;bound=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp861=_cycalloc(sizeof(*_tmp861)),((
_tmp861[0]=((_tmp862.tag=1,((_tmp862.f1=_tmp14,_tmp862)))),_tmp861)))));}goto
_LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_struct*_tmp15=(struct Cyc_Absyn_Region_ptrqual_struct*)
_tmpC;if(_tmp15->tag != 1)goto _LL0;else{_tmp16=(void*)_tmp15->f1;}}_LL10: rgn=
_tmp16;goto _LL0;_LL0:;}{struct _tuple11 _tmp863;return(_tmp863.f1=nullable,((
_tmp863.f2=bound,((_tmp863.f3=zeroterm,((_tmp863.f4=rgn,_tmp863)))))));};}struct
_tuple1*Cyc_Parse_gensym_enum(){static int enum_counter=0;struct _dyneither_ptr*
_tmp870;const char*_tmp86F;void*_tmp86E[1];struct Cyc_Int_pa_struct _tmp86D;struct
_tuple1*_tmp86C;return(_tmp86C=_cycalloc(sizeof(*_tmp86C)),((_tmp86C->f1=Cyc_Absyn_Rel_n(
0),((_tmp86C->f2=((_tmp870=_cycalloc(sizeof(*_tmp870)),((_tmp870[0]=(struct
_dyneither_ptr)((_tmp86D.tag=1,((_tmp86D.f1=(unsigned long)enum_counter ++,((
_tmp86E[0]=& _tmp86D,Cyc_aprintf(((_tmp86F="__anonymous_enum_%d__",_tag_dyneither(
_tmp86F,sizeof(char),22))),_tag_dyneither(_tmp86E,sizeof(void*),1)))))))),
_tmp870)))),_tmp86C)))));}struct _tuple12{struct _tuple1*f1;struct Cyc_Absyn_Tqual
f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple13{struct
_tuple12*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple13*field_info){struct _tuple13 _tmp20;
struct _tuple12*_tmp21;struct _tuple12 _tmp22;struct _tuple1*_tmp23;struct Cyc_Absyn_Tqual
_tmp24;void*_tmp25;struct Cyc_List_List*_tmp26;struct Cyc_List_List*_tmp27;struct
Cyc_Absyn_Exp*_tmp28;struct _tuple13*_tmp1F=field_info;_tmp20=*_tmp1F;_tmp21=
_tmp20.f1;_tmp22=*_tmp21;_tmp23=_tmp22.f1;_tmp24=_tmp22.f2;_tmp25=_tmp22.f3;
_tmp26=_tmp22.f4;_tmp27=_tmp22.f5;_tmp28=_tmp20.f2;if(_tmp26 != 0){const char*
_tmp871;Cyc_Parse_err(((_tmp871="bad type params in struct field",_tag_dyneither(
_tmp871,sizeof(char),32))),loc);}if(Cyc_Absyn_is_qvar_qualified(_tmp23)){const
char*_tmp872;Cyc_Parse_err(((_tmp872="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp872,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp873;return(_tmp873=_cycalloc(sizeof(*_tmp873)),((_tmp873->name=(*_tmp23).f2,((
_tmp873->tq=_tmp24,((_tmp873->type=_tmp25,((_tmp873->width=_tmp28,((_tmp873->attributes=
_tmp27,_tmp873)))))))))));};}static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*
loc){struct Cyc_Parse_Type_spec_struct _tmp876;struct Cyc_Parse_Type_spec_struct*
_tmp875;return(void*)((_tmp875=_cycalloc(sizeof(*_tmp875)),((_tmp875[0]=((
_tmp876.tag=4,((_tmp876.f1=(void*)t,((_tmp876.f2=loc,_tmp876)))))),_tmp875))));}
static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp2E=t;struct Cyc_Absyn_ArrayInfo
_tmp30;void*_tmp31;struct Cyc_Absyn_Tqual _tmp32;struct Cyc_Absyn_Exp*_tmp33;union
Cyc_Absyn_Constraint*_tmp34;struct Cyc_Position_Segment*_tmp35;_LL12: {struct Cyc_Absyn_ArrayType_struct*
_tmp2F=(struct Cyc_Absyn_ArrayType_struct*)_tmp2E;if(_tmp2F->tag != 9)goto _LL14;
else{_tmp30=_tmp2F->f1;_tmp31=_tmp30.elt_type;_tmp32=_tmp30.tq;_tmp33=_tmp30.num_elts;
_tmp34=_tmp30.zero_term;_tmp35=_tmp30.zt_loc;}}_LL13: {struct Cyc_Absyn_Upper_b_struct
_tmp879;struct Cyc_Absyn_Upper_b_struct*_tmp878;return Cyc_Absyn_starb_typ(_tmp31,
argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,
_tmp32,_tmp33 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((void*)((_tmp878=
_cycalloc(sizeof(*_tmp878)),((_tmp878[0]=((_tmp879.tag=1,((_tmp879.f1=(struct Cyc_Absyn_Exp*)
_tmp33,_tmp879)))),_tmp878))))),_tmp34);}_LL14:;_LL15: return t;_LL11:;}struct
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
_tmp4C->f2;}}_LL1F: {struct Cyc_String_pa_struct _tmp881;void*_tmp880[1];const char*
_tmp87F;struct _dyneither_ptr*_tmp87E;struct _dyneither_ptr*nm=(_tmp87E=_cycalloc(
sizeof(*_tmp87E)),((_tmp87E[0]=(struct _dyneither_ptr)((_tmp881.tag=0,((_tmp881.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3D),((_tmp880[0]=& _tmp881,Cyc_aprintf(((
_tmp87F="`%s",_tag_dyneither(_tmp87F,sizeof(char),4))),_tag_dyneither(_tmp880,
sizeof(void*),1)))))))),_tmp87E)));{struct Cyc_Absyn_VarType_struct*_tmp896;
struct Cyc_Absyn_Tvar*_tmp895;struct Cyc_Absyn_Eq_kb_struct _tmp894;struct Cyc_Absyn_Eq_kb_struct*
_tmp893;struct Cyc_Absyn_VarType_struct _tmp892;struct Cyc_Core_Opt*_tmp891;*_tmp4E=((
_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891->v=(void*)((_tmp896=_cycalloc(
sizeof(*_tmp896)),((_tmp896[0]=((_tmp892.tag=2,((_tmp892.f1=((_tmp895=_cycalloc(
sizeof(*_tmp895)),((_tmp895->name=nm,((_tmp895->identity=- 1,((_tmp895->kind=(
void*)((_tmp893=_cycalloc(sizeof(*_tmp893)),((_tmp893[0]=((_tmp894.tag=0,((
_tmp894.f1=& Cyc_Tcutil_ik,_tmp894)))),_tmp893)))),_tmp895)))))))),_tmp892)))),
_tmp896)))),_tmp891))));}goto _LL1D;}_LL20:;_LL21: goto _LL1D;_LL1D:;}{struct
_tuple14*_tmp899;struct Cyc_List_List*_tmp898;_tmp38=((_tmp898=_cycalloc(sizeof(*
_tmp898)),((_tmp898->hd=((_tmp899=_cycalloc(sizeof(*_tmp899)),((_tmp899->f1=
_tmp3D,((_tmp899->f2=_tmp40,_tmp899)))))),((_tmp898->tl=_tmp38,_tmp898))))));}
goto _LL16;_LL19: _tmp41=*_tmp39;_tmp42=_tmp41.f1;if(_tmp42 == 0)goto _LL1B;_tmp43=*
_tmp42;_tmp44=(struct _dyneither_ptr*)_tmp43.v;_tmp45=_tmp41.f3;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp46=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp45;if(_tmp46->tag != 16)goto
_LL1B;else{_tmp47=(void*)_tmp46->f1;{struct Cyc_Absyn_Evar_struct*_tmp48=(struct
Cyc_Absyn_Evar_struct*)_tmp47;if(_tmp48->tag != 1)goto _LL1B;else{_tmp49=_tmp48->f2;
_tmp4A=(struct Cyc_Core_Opt**)& _tmp48->f2;}};}};_LL1A: {struct Cyc_String_pa_struct
_tmp8A1;void*_tmp8A0[1];const char*_tmp89F;struct _dyneither_ptr*_tmp89E;struct
_dyneither_ptr*nm=(_tmp89E=_cycalloc(sizeof(*_tmp89E)),((_tmp89E[0]=(struct
_dyneither_ptr)((_tmp8A1.tag=0,((_tmp8A1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp44),((_tmp8A0[0]=& _tmp8A1,Cyc_aprintf(((_tmp89F="`%s",
_tag_dyneither(_tmp89F,sizeof(char),4))),_tag_dyneither(_tmp8A0,sizeof(void*),1)))))))),
_tmp89E)));{struct Cyc_Absyn_VarType_struct*_tmp8B6;struct Cyc_Absyn_Tvar*_tmp8B5;
struct Cyc_Absyn_Eq_kb_struct _tmp8B4;struct Cyc_Absyn_Eq_kb_struct*_tmp8B3;struct
Cyc_Absyn_VarType_struct _tmp8B2;struct Cyc_Core_Opt*_tmp8B1;*_tmp4A=((_tmp8B1=
_cycalloc(sizeof(*_tmp8B1)),((_tmp8B1->v=(void*)((_tmp8B6=_cycalloc(sizeof(*
_tmp8B6)),((_tmp8B6[0]=((_tmp8B2.tag=2,((_tmp8B2.f1=((_tmp8B5=_cycalloc(sizeof(*
_tmp8B5)),((_tmp8B5->name=nm,((_tmp8B5->identity=- 1,((_tmp8B5->kind=(void*)((
_tmp8B3=_cycalloc(sizeof(*_tmp8B3)),((_tmp8B3[0]=((_tmp8B4.tag=0,((_tmp8B4.f1=&
Cyc_Tcutil_rk,_tmp8B4)))),_tmp8B3)))),_tmp8B5)))))))),_tmp8B2)))),_tmp8B6)))),
_tmp8B1))));}goto _LL16;}_LL1B:;_LL1C: goto _LL16;_LL16:;}return _tmp38;}static
struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){struct
Cyc_List_List*_tmp65=0;for(0;x != 0;x=x->tl){void*_tmp66=((struct Cyc_Absyn_Aggrfield*)
x->hd)->type;void*_tmp68;_LL23: {struct Cyc_Absyn_TagType_struct*_tmp67=(struct
Cyc_Absyn_TagType_struct*)_tmp66;if(_tmp67->tag != 20)goto _LL25;else{_tmp68=(void*)
_tmp67->f1;}}_LL24:{struct _tuple14*_tmp8B9;struct Cyc_List_List*_tmp8B8;_tmp65=((
_tmp8B8=_cycalloc(sizeof(*_tmp8B8)),((_tmp8B8->hd=((_tmp8B9=_cycalloc(sizeof(*
_tmp8B9)),((_tmp8B9->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8B9->f2=
_tmp68,_tmp8B9)))))),((_tmp8B8->tl=_tmp65,_tmp8B8))))));}goto _LL22;_LL25:;_LL26:
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
== 0){struct Cyc_Absyn_Valueof_e_struct _tmp8BC;struct Cyc_Absyn_Valueof_e_struct*
_tmp8BB;return Cyc_Absyn_new_exp((void*)((_tmp8BB=_cycalloc(sizeof(*_tmp8BB)),((
_tmp8BB[0]=((_tmp8BC.tag=39,((_tmp8BC.f1=(void*)Cyc_Tcutil_copy_type(_tmp7C),
_tmp8BC)))),_tmp8BB)))),e->loc);}}}goto _LL27;_LL2C:;_LL2D: goto _LL27;_LL27:;}
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
struct Cyc_Absyn_ArrayType_struct _tmp8C2;struct Cyc_Absyn_ArrayInfo _tmp8C1;struct
Cyc_Absyn_ArrayType_struct*_tmp8C0;return(void*)((_tmp8C0=_cycalloc(sizeof(*
_tmp8C0)),((_tmp8C0[0]=((_tmp8C2.tag=9,((_tmp8C2.f1=((_tmp8C1.elt_type=_tmp91,((
_tmp8C1.tq=_tmp83,((_tmp8C1.num_elts=nelts2,((_tmp8C1.zero_term=_tmp85,((_tmp8C1.zt_loc=
_tmp86,_tmp8C1)))))))))),_tmp8C2)))),_tmp8C0))));}goto _LL2E;};}_LL31: {struct Cyc_Absyn_PointerType_struct*
_tmp87=(struct Cyc_Absyn_PointerType_struct*)_tmp7F;if(_tmp87->tag != 5)goto _LL33;
else{_tmp88=_tmp87->f1;_tmp89=_tmp88.elt_typ;_tmp8A=_tmp88.elt_tq;_tmp8B=_tmp88.ptr_atts;
_tmp8C=_tmp8B.rgn;_tmp8D=_tmp8B.nullable;_tmp8E=_tmp8B.bounds;_tmp8F=_tmp8B.zero_term;
_tmp90=_tmp8B.ptrloc;}}_LL32: {void*_tmp95=Cyc_Parse_substitute_tags(tags,_tmp89);
union Cyc_Absyn_Constraint*_tmp96=_tmp8E;{union Cyc_Absyn_Constraint _tmp97=*_tmp8E;
void*_tmp98;struct Cyc_Absyn_Exp*_tmp9A;_LL36: if((_tmp97.Eq_constr).tag != 1)goto
_LL38;_tmp98=(void*)(_tmp97.Eq_constr).val;{struct Cyc_Absyn_Upper_b_struct*
_tmp99=(struct Cyc_Absyn_Upper_b_struct*)_tmp98;if(_tmp99->tag != 1)goto _LL38;
else{_tmp9A=_tmp99->f1;}};_LL37: _tmp9A=Cyc_Parse_substitute_tags_exp(tags,_tmp9A);{
struct Cyc_Absyn_Upper_b_struct*_tmp8C8;struct Cyc_Absyn_Upper_b_struct _tmp8C7;
union Cyc_Absyn_Constraint*_tmp8C6;_tmp96=((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),(((
_tmp8C6->Eq_constr).val=(void*)((_tmp8C8=_cycalloc(sizeof(*_tmp8C8)),((_tmp8C8[0]=((
_tmp8C7.tag=1,((_tmp8C7.f1=_tmp9A,_tmp8C7)))),_tmp8C8)))),(((_tmp8C6->Eq_constr).tag=
1,_tmp8C6))))));}goto _LL35;_LL38:;_LL39: goto _LL35;_LL35:;}if(_tmp95 != _tmp89  || 
_tmp96 != _tmp8E){struct Cyc_Absyn_PointerType_struct _tmp8D2;struct Cyc_Absyn_PtrAtts
_tmp8D1;struct Cyc_Absyn_PtrInfo _tmp8D0;struct Cyc_Absyn_PointerType_struct*
_tmp8CF;return(void*)((_tmp8CF=_cycalloc(sizeof(*_tmp8CF)),((_tmp8CF[0]=((
_tmp8D2.tag=5,((_tmp8D2.f1=((_tmp8D0.elt_typ=_tmp95,((_tmp8D0.elt_tq=_tmp8A,((
_tmp8D0.ptr_atts=((_tmp8D1.rgn=_tmp8C,((_tmp8D1.nullable=_tmp8D,((_tmp8D1.bounds=
_tmp96,((_tmp8D1.zero_term=_tmp8F,((_tmp8D1.ptrloc=_tmp90,_tmp8D1)))))))))),
_tmp8D0)))))),_tmp8D2)))),_tmp8CF))));}goto _LL2E;}_LL33:;_LL34: goto _LL2E;_LL2E:;}
return t;}static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,
struct Cyc_Absyn_Aggrfield*x){x->type=Cyc_Parse_substitute_tags(tags,x->type);}
struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple15*Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment*loc,struct _tuple8*t){struct _tuple15*_tmp8D3;return(
_tmp8D3=_cycalloc(sizeof(*_tmp8D3)),((_tmp8D3->f1=(*t).f2,((_tmp8D3->f2=(*t).f3,
_tmp8D3)))));}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*
d){void*_tmpA3=d->r;struct Cyc_Absyn_Vardecl*_tmpA5;_LL3B: {struct Cyc_Absyn_Var_d_struct*
_tmpA4=(struct Cyc_Absyn_Var_d_struct*)_tmpA3;if(_tmpA4->tag != 0)goto _LL3D;else{
_tmpA5=_tmpA4->f1;}}_LL3C: return _tmpA5;_LL3D:;_LL3E: {const char*_tmp8D6;void*
_tmp8D5;(_tmp8D5=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((_tmp8D6="bad declaration in `forarray' statement",
_tag_dyneither(_tmp8D6,sizeof(char),40))),_tag_dyneither(_tmp8D5,sizeof(void*),0)));}
_LL3A:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmpA8=tm;_LL40: {struct
Cyc_Absyn_TypeParams_mod_struct*_tmpA9=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpA8;if(_tmpA9->tag != 4)goto _LL42;}_LL41: return 1;_LL42:;_LL43: return 0;_LL3F:;}
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){const char*_tmp8D7;if(
Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8D7="`H",_tag_dyneither(_tmp8D7,
sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_HeapRgn_val;else{const char*
_tmp8D8;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8D8="`U",_tag_dyneither(
_tmp8D8,sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_UniqueRgn_val;else{struct
Cyc_Absyn_VarType_struct _tmp8E2;struct _dyneither_ptr*_tmp8E1;struct Cyc_Absyn_Tvar*
_tmp8E0;struct Cyc_Absyn_VarType_struct*_tmp8DF;return(void*)((_tmp8DF=_cycalloc(
sizeof(*_tmp8DF)),((_tmp8DF[0]=((_tmp8E2.tag=2,((_tmp8E2.f1=((_tmp8E0=_cycalloc(
sizeof(*_tmp8E0)),((_tmp8E0->name=((_tmp8E1=_cycalloc(sizeof(*_tmp8E1)),((
_tmp8E1[0]=s,_tmp8E1)))),((_tmp8E0->identity=- 1,((_tmp8E0->kind=k,_tmp8E0)))))))),
_tmp8E2)))),_tmp8DF))));}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct
Cyc_Absyn_Tvar*t){void*k;{void*_tmpB0=Cyc_Absyn_compress_kb(t->kind);struct Cyc_Absyn_Kind*
_tmpB3;_LL45: {struct Cyc_Absyn_Unknown_kb_struct*_tmpB1=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB0;if(_tmpB1->tag != 1)goto _LL47;}_LL46:{struct Cyc_Absyn_Unknown_kb_struct
_tmp8E5;struct Cyc_Absyn_Unknown_kb_struct*_tmp8E4;k=(void*)((_tmp8E4=_cycalloc(
sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E5.tag=1,((_tmp8E5.f1=0,_tmp8E5)))),
_tmp8E4))));}goto _LL44;_LL47: {struct Cyc_Absyn_Less_kb_struct*_tmpB2=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB0;if(_tmpB2->tag != 2)goto _LL49;else{_tmpB3=_tmpB2->f2;}}_LL48:{struct Cyc_Absyn_Less_kb_struct
_tmp8E8;struct Cyc_Absyn_Less_kb_struct*_tmp8E7;k=(void*)((_tmp8E7=_cycalloc(
sizeof(*_tmp8E7)),((_tmp8E7[0]=((_tmp8E8.tag=2,((_tmp8E8.f1=0,((_tmp8E8.f2=
_tmpB3,_tmp8E8)))))),_tmp8E7))));}goto _LL44;_LL49:;_LL4A: k=_tmpB0;goto _LL44;
_LL44:;}{struct Cyc_Absyn_Tvar*_tmp8E9;return(_tmp8E9=_cycalloc(sizeof(*_tmp8E9)),((
_tmp8E9->name=t->name,((_tmp8E9->identity=- 1,((_tmp8E9->kind=k,_tmp8E9)))))));};}
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,
void*t){void*_tmpB9=t;struct Cyc_Absyn_Tvar*_tmpBB;_LL4C: {struct Cyc_Absyn_VarType_struct*
_tmpBA=(struct Cyc_Absyn_VarType_struct*)_tmpB9;if(_tmpBA->tag != 2)goto _LL4E;
else{_tmpBB=_tmpBA->f1;}}_LL4D: return _tmpBB;_LL4E:;_LL4F: {const char*_tmp8EC;
void*_tmp8EB;(_tmp8EB=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp8EC="expecting a list of type variables, not types",
_tag_dyneither(_tmp8EC,sizeof(char),46))),_tag_dyneither(_tmp8EB,sizeof(void*),0)));}
_LL4B:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct
_tmp8EF;struct Cyc_Absyn_VarType_struct*_tmp8EE;return(void*)((_tmp8EE=_cycalloc(
sizeof(*_tmp8EE)),((_tmp8EE[0]=((_tmp8EF.tag=2,((_tmp8EF.f1=pr,_tmp8EF)))),
_tmp8EE))));}static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,
int leq){void*_tmpC0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpC2;struct Cyc_Absyn_Tvar
_tmpC3;void*_tmpC4;void**_tmpC5;_LL51: {struct Cyc_Absyn_VarType_struct*_tmpC1=(
struct Cyc_Absyn_VarType_struct*)_tmpC0;if(_tmpC1->tag != 2)goto _LL53;else{_tmpC2=
_tmpC1->f1;_tmpC3=*_tmpC2;_tmpC4=_tmpC3.kind;_tmpC5=(void**)&(*_tmpC1->f1).kind;}}
_LL52: {void*_tmpC6=Cyc_Absyn_compress_kb(*_tmpC5);_LL56: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpC7=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC6;if(_tmpC7->tag != 1)goto _LL58;}
_LL57: if(!leq)*_tmpC5=Cyc_Tcutil_kind_to_bound(k);else{struct Cyc_Absyn_Less_kb_struct
_tmp8F2;struct Cyc_Absyn_Less_kb_struct*_tmp8F1;*_tmpC5=(void*)((_tmp8F1=
_cycalloc(sizeof(*_tmp8F1)),((_tmp8F1[0]=((_tmp8F2.tag=2,((_tmp8F2.f1=0,((
_tmp8F2.f2=k,_tmp8F2)))))),_tmp8F1))));}return;_LL58:;_LL59: return;_LL55:;}_LL53:;
_LL54: return;_LL50:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmpCA=(void*)tms->hd;void*_tmpCC;
_LL5B: {struct Cyc_Absyn_Function_mod_struct*_tmpCB=(struct Cyc_Absyn_Function_mod_struct*)
_tmpCA;if(_tmpCB->tag != 3)goto _LL5D;else{_tmpCC=(void*)_tmpCB->f1;}}_LL5C: if(tms->tl
== 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)
 && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){void*_tmpCD=_tmpCC;
struct Cyc_List_List*_tmpD0;_LL60: {struct Cyc_Absyn_WithTypes_struct*_tmpCE=(
struct Cyc_Absyn_WithTypes_struct*)_tmpCD;if(_tmpCE->tag != 1)goto _LL62;}_LL61:{
const char*_tmp8F5;void*_tmp8F4;(_tmp8F4=0,Cyc_Tcutil_warn(loc,((_tmp8F5="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp8F5,sizeof(char),93))),_tag_dyneither(_tmp8F4,sizeof(void*),0)));}
return tms;_LL62: {struct Cyc_Absyn_NoTypes_struct*_tmpCF=(struct Cyc_Absyn_NoTypes_struct*)
_tmpCD;if(_tmpCF->tag != 0)goto _LL5F;else{_tmpD0=_tmpCF->f1;}}_LL63: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpD0)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds)){const char*_tmp8F8;void*_tmp8F7;(_tmp8F7=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
loc,((_tmp8F8="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp8F8,sizeof(char),73))),_tag_dyneither(_tmp8F7,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpD5=0;for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){struct Cyc_List_List*
_tmpD6=tds;for(0;_tmpD6 != 0;_tmpD6=_tmpD6->tl){struct Cyc_Absyn_Decl*_tmpD7=(
struct Cyc_Absyn_Decl*)_tmpD6->hd;void*_tmpD8=_tmpD7->r;struct Cyc_Absyn_Vardecl*
_tmpDA;_LL65: {struct Cyc_Absyn_Var_d_struct*_tmpD9=(struct Cyc_Absyn_Var_d_struct*)
_tmpD8;if(_tmpD9->tag != 0)goto _LL67;else{_tmpDA=_tmpD9->f1;}}_LL66: if(Cyc_zstrptrcmp((*
_tmpDA->name).f2,(struct _dyneither_ptr*)_tmpD0->hd)!= 0)continue;if(_tmpDA->initializer
!= 0){const char*_tmp8FB;void*_tmp8FA;(_tmp8FA=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD7->loc,((
_tmp8FB="initializer found in parameter declaration",_tag_dyneither(_tmp8FB,
sizeof(char),43))),_tag_dyneither(_tmp8FA,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmpDA->name)){const char*_tmp8FE;void*_tmp8FD;(_tmp8FD=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD7->loc,((
_tmp8FE="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8FE,
sizeof(char),47))),_tag_dyneither(_tmp8FD,sizeof(void*),0)));}{struct _tuple8*
_tmp904;struct Cyc_Core_Opt*_tmp903;struct Cyc_List_List*_tmp902;_tmpD5=((_tmp902=
_cycalloc(sizeof(*_tmp902)),((_tmp902->hd=((_tmp904=_cycalloc(sizeof(*_tmp904)),((
_tmp904->f1=((_tmp903=_cycalloc(sizeof(*_tmp903)),((_tmp903->v=(*_tmpDA->name).f2,
_tmp903)))),((_tmp904->f2=_tmpDA->tq,((_tmp904->f3=_tmpDA->type,_tmp904)))))))),((
_tmp902->tl=_tmpD5,_tmp902))))));}goto L;_LL67:;_LL68: {const char*_tmp907;void*
_tmp906;(_tmp906=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD7->loc,((_tmp907="nonvariable declaration in parameter type",
_tag_dyneither(_tmp907,sizeof(char),42))),_tag_dyneither(_tmp906,sizeof(void*),0)));}
_LL64:;}L: if(_tmpD6 == 0){const char*_tmp90B;void*_tmp90A[1];struct Cyc_String_pa_struct
_tmp909;(_tmp909.tag=0,((_tmp909.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmpD0->hd)),((_tmp90A[0]=& _tmp909,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp90B="%s is not given a type",_tag_dyneither(_tmp90B,sizeof(char),23))),
_tag_dyneither(_tmp90A,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp91A;struct Cyc_Absyn_WithTypes_struct*_tmp919;struct Cyc_Absyn_WithTypes_struct
_tmp918;struct Cyc_Absyn_Function_mod_struct _tmp917;struct Cyc_List_List*_tmp916;
return(_tmp916=_cycalloc(sizeof(*_tmp916)),((_tmp916->hd=(void*)((_tmp91A=
_cycalloc(sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp917.tag=3,((_tmp917.f1=(void*)((
void*)((_tmp919=_cycalloc(sizeof(*_tmp919)),((_tmp919[0]=((_tmp918.tag=1,((
_tmp918.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpD5),((_tmp918.f2=0,((_tmp918.f3=0,((_tmp918.f4=0,((_tmp918.f5=0,_tmp918)))))))))))),
_tmp919))))),_tmp917)))),_tmp91A)))),((_tmp916->tl=0,_tmp916)))));};};_LL5F:;}
goto _LL5E;_LL5D:;_LL5E: {struct Cyc_List_List*_tmp91B;return(_tmp91B=_cycalloc(
sizeof(*_tmp91B)),((_tmp91B->hd=(void*)tms->hd,((_tmp91B->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp91B)))));}_LL5A:;};}struct _tuple16{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){if(tds != 0){struct Cyc_Parse_Declarator*
_tmp91C;d=((_tmp91C=_cycalloc(sizeof(*_tmp91C)),((_tmp91C->id=d->id,((_tmp91C->tms=
Cyc_Parse_oldstyle2newstyle(d->tms,tds,loc),_tmp91C))))));}{enum Cyc_Absyn_Scope
sc=Cyc_Absyn_Public;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(
0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0)
switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v){case Cyc_Parse_Extern_sc: _LL69: sc=
Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL6A: sc=Cyc_Absyn_Static;break;
default: _LL6B:{const char*_tmp91D;Cyc_Parse_err(((_tmp91D="bad storage class on function",
_tag_dyneither(_tmp91D,sizeof(char),30))),loc);}break;}}{void*_tmpEF=Cyc_Parse_collapse_type_specifiers(
tss,loc);struct Cyc_Absyn_Tqual _tmpF1;void*_tmpF2;struct Cyc_List_List*_tmpF3;
struct Cyc_List_List*_tmpF4;struct _tuple10 _tmpF0=Cyc_Parse_apply_tms(tq,_tmpEF,
atts,d->tms);_tmpF1=_tmpF0.f1;_tmpF2=_tmpF0.f2;_tmpF3=_tmpF0.f3;_tmpF4=_tmpF0.f4;
if(_tmpF3 != 0){const char*_tmp920;void*_tmp91F;(_tmp91F=0,Cyc_Tcutil_warn(loc,((
_tmp920="bad type params, ignoring",_tag_dyneither(_tmp920,sizeof(char),26))),
_tag_dyneither(_tmp91F,sizeof(void*),0)));}{void*_tmpF7=_tmpF2;struct Cyc_Absyn_FnInfo
_tmpF9;struct Cyc_List_List*_tmpFA;struct Cyc_Core_Opt*_tmpFB;struct Cyc_Absyn_Tqual
_tmpFC;void*_tmpFD;struct Cyc_List_List*_tmpFE;int _tmpFF;struct Cyc_Absyn_VarargInfo*
_tmp100;struct Cyc_List_List*_tmp101;struct Cyc_List_List*_tmp102;_LL6E: {struct
Cyc_Absyn_FnType_struct*_tmpF8=(struct Cyc_Absyn_FnType_struct*)_tmpF7;if(_tmpF8->tag
!= 10)goto _LL70;else{_tmpF9=_tmpF8->f1;_tmpFA=_tmpF9.tvars;_tmpFB=_tmpF9.effect;
_tmpFC=_tmpF9.ret_tqual;_tmpFD=_tmpF9.ret_typ;_tmpFE=_tmpF9.args;_tmpFF=_tmpF9.c_varargs;
_tmp100=_tmpF9.cyc_varargs;_tmp101=_tmpF9.rgn_po;_tmp102=_tmpF9.attributes;}}
_LL6F: {struct Cyc_List_List*_tmp103=0;{struct Cyc_List_List*_tmp104=_tmpFE;for(0;
_tmp104 != 0;_tmp104=_tmp104->tl){struct _tuple8 _tmp106;struct Cyc_Core_Opt*_tmp107;
struct Cyc_Absyn_Tqual _tmp108;void*_tmp109;struct _tuple8*_tmp105=(struct _tuple8*)
_tmp104->hd;_tmp106=*_tmp105;_tmp107=_tmp106.f1;_tmp108=_tmp106.f2;_tmp109=
_tmp106.f3;if(_tmp107 == 0){{const char*_tmp921;Cyc_Parse_err(((_tmp921="missing argument variable in function prototype",
_tag_dyneither(_tmp921,sizeof(char),48))),loc);}{struct _tuple16*_tmp924;struct
Cyc_List_List*_tmp923;_tmp103=((_tmp923=_cycalloc(sizeof(*_tmp923)),((_tmp923->hd=((
_tmp924=_cycalloc(sizeof(*_tmp924)),((_tmp924->f1=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp924->f2=_tmp108,((
_tmp924->f3=_tmp109,_tmp924)))))))),((_tmp923->tl=_tmp103,_tmp923))))));};}else{
struct _tuple16*_tmp927;struct Cyc_List_List*_tmp926;_tmp103=((_tmp926=_cycalloc(
sizeof(*_tmp926)),((_tmp926->hd=((_tmp927=_cycalloc(sizeof(*_tmp927)),((_tmp927->f1=(
struct _dyneither_ptr*)_tmp107->v,((_tmp927->f2=_tmp108,((_tmp927->f3=_tmp109,
_tmp927)))))))),((_tmp926->tl=_tmp103,_tmp926))))));}}}{struct Cyc_Absyn_Fndecl*
_tmp928;return(_tmp928=_cycalloc(sizeof(*_tmp928)),((_tmp928->sc=sc,((_tmp928->name=
d->id,((_tmp928->tvs=_tmpFA,((_tmp928->is_inline=is_inline,((_tmp928->effect=
_tmpFB,((_tmp928->ret_tqual=_tmpFC,((_tmp928->ret_type=_tmpFD,((_tmp928->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp103),((
_tmp928->c_varargs=_tmpFF,((_tmp928->cyc_varargs=_tmp100,((_tmp928->rgn_po=
_tmp101,((_tmp928->body=body,((_tmp928->cached_typ=0,((_tmp928->param_vardecls=0,((
_tmp928->fn_vardecl=0,((_tmp928->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp102,_tmpF4),_tmp928)))))))))))))))))))))))))))))))));};}
_LL70:;_LL71: {const char*_tmp92B;void*_tmp92A;(_tmp92A=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp92B="declarator is not a function prototype",_tag_dyneither(_tmp92B,sizeof(
char),39))),_tag_dyneither(_tmp92A,sizeof(void*),0)));}_LL6D:;};};};}static char
_tmp113[52]="at most one type may appear within a type specifier";static struct
_dyneither_ptr Cyc_Parse_msg1={_tmp113,_tmp113,_tmp113 + 52};static char _tmp114[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp114,_tmp114,_tmp114 + 63};static
char _tmp115[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp115,_tmp115,_tmp115 + 50};static char _tmp116[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp116,_tmp116,_tmp116 + 60};static
void*Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){int seen_type=0;int seen_sign=0;int seen_size=0;void*t=(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp117=(
void*)ts->hd;void*_tmp119;struct Cyc_Position_Segment*_tmp11A;struct Cyc_Position_Segment*
_tmp11C;struct Cyc_Position_Segment*_tmp11E;struct Cyc_Position_Segment*_tmp120;
struct Cyc_Position_Segment*_tmp122;_LL73: {struct Cyc_Parse_Type_spec_struct*
_tmp118=(struct Cyc_Parse_Type_spec_struct*)_tmp117;if(_tmp118->tag != 4)goto _LL75;
else{_tmp119=(void*)_tmp118->f1;_tmp11A=_tmp118->f2;}}_LL74: if(seen_type)Cyc_Parse_err(
Cyc_Parse_msg1,_tmp11A);last_loc=_tmp11A;seen_type=1;t=_tmp119;goto _LL72;_LL75: {
struct Cyc_Parse_Signed_spec_struct*_tmp11B=(struct Cyc_Parse_Signed_spec_struct*)
_tmp117;if(_tmp11B->tag != 0)goto _LL77;else{_tmp11C=_tmp11B->f1;}}_LL76: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp11C);if(seen_type){const char*_tmp92C;
Cyc_Parse_err(((_tmp92C="signed qualifier must come before type",_tag_dyneither(
_tmp92C,sizeof(char),39))),_tmp11C);}last_loc=_tmp11C;seen_sign=1;sgn=Cyc_Absyn_Signed;
goto _LL72;_LL77: {struct Cyc_Parse_Unsigned_spec_struct*_tmp11D=(struct Cyc_Parse_Unsigned_spec_struct*)
_tmp117;if(_tmp11D->tag != 1)goto _LL79;else{_tmp11E=_tmp11D->f1;}}_LL78: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp11E);if(seen_type){const char*_tmp92D;
Cyc_Parse_err(((_tmp92D="signed qualifier must come before type",_tag_dyneither(
_tmp92D,sizeof(char),39))),_tmp11E);}last_loc=_tmp11E;seen_sign=1;sgn=Cyc_Absyn_Unsigned;
goto _LL72;_LL79: {struct Cyc_Parse_Short_spec_struct*_tmp11F=(struct Cyc_Parse_Short_spec_struct*)
_tmp117;if(_tmp11F->tag != 2)goto _LL7B;else{_tmp120=_tmp11F->f1;}}_LL7A: if(
seen_size){const char*_tmp92E;Cyc_Parse_err(((_tmp92E="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp92E,sizeof(char),59))),_tmp120);}if(seen_type){const char*
_tmp92F;Cyc_Parse_err(((_tmp92F="short modifier must come before base type",
_tag_dyneither(_tmp92F,sizeof(char),42))),_tmp120);}last_loc=_tmp120;seen_size=1;
sz=Cyc_Absyn_Short_sz;goto _LL72;_LL7B: {struct Cyc_Parse_Long_spec_struct*_tmp121=(
struct Cyc_Parse_Long_spec_struct*)_tmp117;if(_tmp121->tag != 3)goto _LL72;else{
_tmp122=_tmp121->f1;}}_LL7C: if(seen_type){const char*_tmp930;Cyc_Parse_err(((
_tmp930="long modifier must come before base type",_tag_dyneither(_tmp930,
sizeof(char),41))),_tmp122);}if(seen_size)switch(sz){case Cyc_Absyn_Long_sz: _LL7D:
sz=Cyc_Absyn_LongLong_sz;break;default: _LL7E:{const char*_tmp931;Cyc_Parse_err(((
_tmp931="extra long in type specifier",_tag_dyneither(_tmp931,sizeof(char),29))),
_tmp122);}break;}else{sz=Cyc_Absyn_Long_sz;}last_loc=_tmp122;seen_size=1;goto
_LL72;_LL72:;}if(!seen_type){if(!seen_sign  && !seen_size){const char*_tmp934;void*
_tmp933;(_tmp933=0,Cyc_Tcutil_warn(last_loc,((_tmp934="missing type within specifier",
_tag_dyneither(_tmp934,sizeof(char),30))),_tag_dyneither(_tmp933,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{if(seen_sign){void*_tmp12B=t;enum Cyc_Absyn_Sign
_tmp12D;enum Cyc_Absyn_Size_of _tmp12E;_LL81: {struct Cyc_Absyn_IntType_struct*
_tmp12C=(struct Cyc_Absyn_IntType_struct*)_tmp12B;if(_tmp12C->tag != 6)goto _LL83;
else{_tmp12D=_tmp12C->f1;_tmp12E=_tmp12C->f2;}}_LL82: if(_tmp12D != sgn)t=Cyc_Absyn_int_typ(
sgn,_tmp12E);goto _LL80;_LL83:;_LL84:{const char*_tmp935;Cyc_Parse_err(((_tmp935="sign specification on non-integral type",
_tag_dyneither(_tmp935,sizeof(char),40))),last_loc);}goto _LL80;_LL80:;}if(
seen_size){void*_tmp130=t;enum Cyc_Absyn_Sign _tmp132;enum Cyc_Absyn_Size_of
_tmp133;_LL86: {struct Cyc_Absyn_IntType_struct*_tmp131=(struct Cyc_Absyn_IntType_struct*)
_tmp130;if(_tmp131->tag != 6)goto _LL88;else{_tmp132=_tmp131->f1;_tmp133=_tmp131->f2;}}
_LL87: if(_tmp133 != sz)t=Cyc_Absyn_int_typ(_tmp132,sz);goto _LL85;_LL88: {struct
Cyc_Absyn_DoubleType_struct*_tmp134=(struct Cyc_Absyn_DoubleType_struct*)_tmp130;
if(_tmp134->tag != 8)goto _LL8A;}_LL89: t=Cyc_Absyn_double_typ(1);goto _LL85;_LL8A:;
_LL8B:{const char*_tmp936;Cyc_Parse_err(((_tmp936="size qualifier on non-integral type",
_tag_dyneither(_tmp936,sizeof(char),36))),last_loc);}goto _LL85;_LL85:;}}return t;}
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)
return 0;{struct Cyc_Parse_Declarator*_tmp136=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmp137=_tmp136->id;struct Cyc_Absyn_Tqual _tmp139;void*_tmp13A;
struct Cyc_List_List*_tmp13B;struct Cyc_List_List*_tmp13C;struct _tuple10 _tmp138=
Cyc_Parse_apply_tms(tq,t,shared_atts,_tmp136->tms);_tmp139=_tmp138.f1;_tmp13A=
_tmp138.f2;_tmp13B=_tmp138.f3;_tmp13C=_tmp138.f4;if(ds->tl == 0){struct _tuple12*
_tmp939;struct Cyc_List_List*_tmp938;return(_tmp938=_region_malloc(r,sizeof(*
_tmp938)),((_tmp938->hd=((_tmp939=_region_malloc(r,sizeof(*_tmp939)),((_tmp939->f1=
_tmp137,((_tmp939->f2=_tmp139,((_tmp939->f3=_tmp13A,((_tmp939->f4=_tmp13B,((
_tmp939->f5=_tmp13C,_tmp939)))))))))))),((_tmp938->tl=0,_tmp938)))));}else{
struct _tuple12*_tmp93C;struct Cyc_List_List*_tmp93B;return(_tmp93B=_region_malloc(
r,sizeof(*_tmp93B)),((_tmp93B->hd=((_tmp93C=_region_malloc(r,sizeof(*_tmp93C)),((
_tmp93C->f1=_tmp137,((_tmp93C->f2=_tmp139,((_tmp93C->f3=_tmp13A,((_tmp93C->f4=
_tmp13B,((_tmp93C->f5=_tmp13C,_tmp93C)))))))))))),((_tmp93B->tl=Cyc_Parse_apply_tmss(
r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp93B)))));}};}static struct
_tuple10 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0){struct _tuple10 _tmp93D;return(_tmp93D.f1=
tq,((_tmp93D.f2=t,((_tmp93D.f3=0,((_tmp93D.f4=atts,_tmp93D)))))));}{void*_tmp142=(
void*)tms->hd;union Cyc_Absyn_Constraint*_tmp144;struct Cyc_Position_Segment*
_tmp145;struct Cyc_Absyn_Exp*_tmp147;union Cyc_Absyn_Constraint*_tmp148;struct Cyc_Position_Segment*
_tmp149;void*_tmp14B;struct Cyc_List_List*_tmp14D;struct Cyc_Position_Segment*
_tmp14E;struct Cyc_Absyn_PtrAtts _tmp150;struct Cyc_Absyn_Tqual _tmp151;struct Cyc_Position_Segment*
_tmp153;struct Cyc_List_List*_tmp154;_LL8D: {struct Cyc_Absyn_Carray_mod_struct*
_tmp143=(struct Cyc_Absyn_Carray_mod_struct*)_tmp142;if(_tmp143->tag != 0)goto
_LL8F;else{_tmp144=_tmp143->f1;_tmp145=_tmp143->f2;}}_LL8E: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmp144,_tmp145),atts,tms->tl);
_LL8F: {struct Cyc_Absyn_ConstArray_mod_struct*_tmp146=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp142;if(_tmp146->tag != 1)goto _LL91;else{_tmp147=_tmp146->f1;_tmp148=_tmp146->f2;
_tmp149=_tmp146->f3;}}_LL90: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmp147,_tmp148,_tmp149),atts,tms->tl);
_LL91: {struct Cyc_Absyn_Function_mod_struct*_tmp14A=(struct Cyc_Absyn_Function_mod_struct*)
_tmp142;if(_tmp14A->tag != 3)goto _LL93;else{_tmp14B=(void*)_tmp14A->f1;}}_LL92: {
void*_tmp155=_tmp14B;struct Cyc_List_List*_tmp157;int _tmp158;struct Cyc_Absyn_VarargInfo*
_tmp159;struct Cyc_Core_Opt*_tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_Position_Segment*
_tmp15D;_LL9A: {struct Cyc_Absyn_WithTypes_struct*_tmp156=(struct Cyc_Absyn_WithTypes_struct*)
_tmp155;if(_tmp156->tag != 1)goto _LL9C;else{_tmp157=_tmp156->f1;_tmp158=_tmp156->f2;
_tmp159=_tmp156->f3;_tmp15A=_tmp156->f4;_tmp15B=_tmp156->f5;}}_LL9B: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp93E;fn_atts=((_tmp93E=_cycalloc(sizeof(*_tmp93E)),((
_tmp93E->hd=(void*)as->hd,((_tmp93E->tl=fn_atts,_tmp93E))))));}else{struct Cyc_List_List*
_tmp93F;new_atts=((_tmp93F=_cycalloc(sizeof(*_tmp93F)),((_tmp93F->hd=(void*)as->hd,((
_tmp93F->tl=new_atts,_tmp93F))))));}}}if(tms->tl != 0){void*_tmp160=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;struct Cyc_List_List*_tmp162;_LL9F: {
struct Cyc_Absyn_TypeParams_mod_struct*_tmp161=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp160;if(_tmp161->tag != 4)goto _LLA1;else{_tmp162=_tmp161->f1;}}_LLA0: typvars=
_tmp162;tms=tms->tl;goto _LL9E;_LLA1:;_LLA2: goto _LL9E;_LL9E:;}if(((((!_tmp158  && 
_tmp159 == 0) && _tmp157 != 0) && _tmp157->tl == 0) && (*((struct _tuple8*)_tmp157->hd)).f1
== 0) && (*((struct _tuple8*)_tmp157->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
_tmp157=0;{struct Cyc_List_List*_tmp163=Cyc_Parse_get_arg_tags(_tmp157);if(
_tmp163 != 0)t=Cyc_Parse_substitute_tags(_tmp163,t);t=Cyc_Parse_array2ptr(t,0);{
struct Cyc_List_List*_tmp164=_tmp157;for(0;_tmp164 != 0;_tmp164=_tmp164->tl){
struct _tuple8 _tmp166;struct Cyc_Core_Opt*_tmp167;struct Cyc_Absyn_Tqual _tmp168;
void*_tmp169;void**_tmp16A;struct _tuple8*_tmp165=(struct _tuple8*)_tmp164->hd;
_tmp166=*_tmp165;_tmp167=_tmp166.f1;_tmp168=_tmp166.f2;_tmp169=_tmp166.f3;
_tmp16A=(void**)&(*_tmp165).f3;if(_tmp163 != 0)*_tmp16A=Cyc_Parse_substitute_tags(
_tmp163,*_tmp16A);*_tmp16A=Cyc_Parse_array2ptr(*_tmp16A,1);}}return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(tq.loc),Cyc_Absyn_function_typ(typvars,_tmp15A,tq,t,
_tmp157,_tmp158,_tmp159,_tmp15B,fn_atts),new_atts,((struct Cyc_List_List*)
_check_null(tms))->tl);};}_LL9C: {struct Cyc_Absyn_NoTypes_struct*_tmp15C=(struct
Cyc_Absyn_NoTypes_struct*)_tmp155;if(_tmp15C->tag != 0)goto _LL99;else{_tmp15D=
_tmp15C->f2;}}_LL9D: {const char*_tmp942;void*_tmp941;(_tmp941=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
_tmp15D,((_tmp942="function declaration without parameter types",_tag_dyneither(
_tmp942,sizeof(char),45))),_tag_dyneither(_tmp941,sizeof(void*),0)));}_LL99:;}
_LL93: {struct Cyc_Absyn_TypeParams_mod_struct*_tmp14C=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp142;if(_tmp14C->tag != 4)goto _LL95;else{_tmp14D=_tmp14C->f1;_tmp14E=_tmp14C->f2;}}
_LL94: if(tms->tl == 0){struct _tuple10 _tmp943;return(_tmp943.f1=tq,((_tmp943.f2=t,((
_tmp943.f3=_tmp14D,((_tmp943.f4=atts,_tmp943)))))));}{const char*_tmp946;void*
_tmp945;(_tmp945=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp14E,((_tmp946="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp946,sizeof(char),68))),_tag_dyneither(_tmp945,sizeof(void*),0)));};
_LL95: {struct Cyc_Absyn_Pointer_mod_struct*_tmp14F=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp142;if(_tmp14F->tag != 2)goto _LL97;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;}}
_LL96: {struct Cyc_Absyn_PointerType_struct _tmp94C;struct Cyc_Absyn_PtrInfo _tmp94B;
struct Cyc_Absyn_PointerType_struct*_tmp94A;return Cyc_Parse_apply_tms(_tmp151,(
void*)((_tmp94A=_cycalloc(sizeof(*_tmp94A)),((_tmp94A[0]=((_tmp94C.tag=5,((
_tmp94C.f1=((_tmp94B.elt_typ=t,((_tmp94B.elt_tq=tq,((_tmp94B.ptr_atts=_tmp150,
_tmp94B)))))),_tmp94C)))),_tmp94A)))),atts,tms->tl);}_LL97: {struct Cyc_Absyn_Attributes_mod_struct*
_tmp152=(struct Cyc_Absyn_Attributes_mod_struct*)_tmp142;if(_tmp152->tag != 5)goto
_LL8C;else{_tmp153=_tmp152->f1;_tmp154=_tmp152->f2;}}_LL98: return Cyc_Parse_apply_tms(
tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
atts,_tmp154),tms->tl);_LL8C:;};}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*
tss,struct Cyc_Position_Segment*loc){return Cyc_Parse_collapse_type_specifiers(tss,
loc);}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple12*t){struct _tuple12 _tmp174;struct _tuple1*_tmp175;struct Cyc_Absyn_Tqual
_tmp176;void*_tmp177;struct Cyc_List_List*_tmp178;struct Cyc_List_List*_tmp179;
struct _tuple12*_tmp173=t;_tmp174=*_tmp173;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;
_tmp177=_tmp174.f3;_tmp178=_tmp174.f4;_tmp179=_tmp174.f5;Cyc_Lex_register_typedef(
_tmp175);{struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*type;{void*_tmp17A=_tmp177;
struct Cyc_Core_Opt*_tmp17C;_LLA4: {struct Cyc_Absyn_Evar_struct*_tmp17B=(struct
Cyc_Absyn_Evar_struct*)_tmp17A;if(_tmp17B->tag != 1)goto _LLA6;else{_tmp17C=
_tmp17B->f1;}}_LLA5: type=0;if(_tmp17C == 0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;
else{kind=_tmp17C;}goto _LLA3;_LLA6:;_LLA7: kind=0;{struct Cyc_Core_Opt*_tmp94D;
type=((_tmp94D=_cycalloc(sizeof(*_tmp94D)),((_tmp94D->v=_tmp177,_tmp94D))));}
goto _LLA3;_LLA3:;}{struct Cyc_Absyn_Typedef_d_struct _tmp953;struct Cyc_Absyn_Typedefdecl*
_tmp952;struct Cyc_Absyn_Typedef_d_struct*_tmp951;return Cyc_Absyn_new_decl((void*)((
_tmp951=_cycalloc(sizeof(*_tmp951)),((_tmp951[0]=((_tmp953.tag=9,((_tmp953.f1=((
_tmp952=_cycalloc(sizeof(*_tmp952)),((_tmp952->name=_tmp175,((_tmp952->tvs=
_tmp178,((_tmp952->kind=kind,((_tmp952->defn=type,((_tmp952->atts=_tmp179,((
_tmp952->tq=_tmp176,_tmp952)))))))))))))),_tmp953)))),_tmp951)))),loc);};};}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct
Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Decl_s_struct _tmp956;struct Cyc_Absyn_Decl_s_struct*
_tmp955;return Cyc_Absyn_new_stmt((void*)((_tmp955=_cycalloc(sizeof(*_tmp955)),((
_tmp955[0]=((_tmp956.tag=12,((_tmp956.f1=d,((_tmp956.f2=s,_tmp956)))))),_tmp955)))),
d->loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*
ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*
f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*
accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*
Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*
ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*loc){struct
_RegionHandle _tmp183=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp183;
_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp185;struct Cyc_Absyn_Tqual
_tmp186;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_Parse_Declaration_spec*
_tmp184=ds;_tmp185=*_tmp184;_tmp186=_tmp185.tq;_tmp187=_tmp185.type_specs;
_tmp188=_tmp185.attributes;if(_tmp186.loc == 0)_tmp186.loc=tqual_loc;if(ds->is_inline){
const char*_tmp957;Cyc_Parse_err(((_tmp957="inline is allowed only on function definitions",
_tag_dyneither(_tmp957,sizeof(char),47))),loc);}if(_tmp187 == 0){{const char*
_tmp95A;void*_tmp959;(_tmp959=0,Cyc_Tcutil_warn(loc,((_tmp95A="missing type specifiers in declaration, assuming int",
_tag_dyneither(_tmp95A,sizeof(char),53))),_tag_dyneither(_tmp959,sizeof(void*),0)));}{
void*_tmp95B[1];_tmp187=((_tmp95B[0]=Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,loc),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp95B,sizeof(void*),1))));};}{enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;int
istypedef=0;if(ds->sc != 0)switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v){case Cyc_Parse_Typedef_sc: _LLA8: istypedef=1;break;case
Cyc_Parse_Extern_sc: _LLA9: s=Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc:
_LLAA: s=Cyc_Absyn_ExternC;break;case Cyc_Parse_Static_sc: _LLAB: s=Cyc_Absyn_Static;
break;case Cyc_Parse_Auto_sc: _LLAC: s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc:
_LLAD: if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}
break;case Cyc_Parse_Abstract_sc: _LLAE: s=Cyc_Absyn_Abstract;break;}{struct Cyc_List_List*
_tmp18E;struct Cyc_List_List*_tmp18F;struct _tuple0 _tmp18D=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
mkrgn,mkrgn,ids);_tmp18E=_tmp18D.f1;_tmp18F=_tmp18D.f2;{int exps_empty=1;{struct
Cyc_List_List*es=_tmp18F;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd
!= 0){exps_empty=0;break;}}}{void*_tmp190=Cyc_Parse_collapse_type_specifiers(
_tmp187,loc);if(_tmp18E == 0){void*_tmp191=_tmp190;struct Cyc_Absyn_TypeDecl*
_tmp193;struct Cyc_Absyn_TypeDecl _tmp194;void*_tmp195;struct Cyc_Absyn_Aggrdecl*
_tmp197;struct Cyc_Absyn_TypeDecl*_tmp199;struct Cyc_Absyn_TypeDecl _tmp19A;void*
_tmp19B;struct Cyc_Absyn_Enumdecl*_tmp19D;struct Cyc_Absyn_TypeDecl*_tmp19F;struct
Cyc_Absyn_TypeDecl _tmp1A0;void*_tmp1A1;struct Cyc_Absyn_Datatypedecl*_tmp1A3;
struct Cyc_Absyn_AggrInfo _tmp1A5;union Cyc_Absyn_AggrInfoU _tmp1A6;struct _tuple3
_tmp1A7;enum Cyc_Absyn_AggrKind _tmp1A8;struct _tuple1*_tmp1A9;struct Cyc_List_List*
_tmp1AA;struct Cyc_Absyn_DatatypeInfo _tmp1AC;union Cyc_Absyn_DatatypeInfoU _tmp1AD;
struct Cyc_Absyn_Datatypedecl**_tmp1AE;struct Cyc_Absyn_DatatypeInfo _tmp1B0;union
Cyc_Absyn_DatatypeInfoU _tmp1B1;struct Cyc_Absyn_UnknownDatatypeInfo _tmp1B2;struct
_tuple1*_tmp1B3;int _tmp1B4;struct Cyc_List_List*_tmp1B5;struct _tuple1*_tmp1B7;
struct Cyc_List_List*_tmp1B9;_LLB1: {struct Cyc_Absyn_TypeDeclType_struct*_tmp192=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp191;if(_tmp192->tag != 26)goto _LLB3;
else{_tmp193=_tmp192->f1;_tmp194=*_tmp193;_tmp195=_tmp194.r;{struct Cyc_Absyn_Aggr_td_struct*
_tmp196=(struct Cyc_Absyn_Aggr_td_struct*)_tmp195;if(_tmp196->tag != 0)goto _LLB3;
else{_tmp197=_tmp196->f1;}};}}_LLB2: _tmp197->attributes=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp197->attributes,
_tmp188);{struct Cyc_Absyn_Aggr_d_struct*_tmp961;struct Cyc_Absyn_Aggr_d_struct
_tmp960;struct Cyc_List_List*_tmp95F;struct Cyc_List_List*_tmp1BD=(_tmp95F=
_cycalloc(sizeof(*_tmp95F)),((_tmp95F->hd=Cyc_Absyn_new_decl((void*)((_tmp961=
_cycalloc(sizeof(*_tmp961)),((_tmp961[0]=((_tmp960.tag=6,((_tmp960.f1=_tmp197,
_tmp960)))),_tmp961)))),loc),((_tmp95F->tl=0,_tmp95F)))));_npop_handler(0);
return _tmp1BD;};_LLB3: {struct Cyc_Absyn_TypeDeclType_struct*_tmp198=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp191;if(_tmp198->tag != 26)goto _LLB5;else{_tmp199=_tmp198->f1;_tmp19A=*_tmp199;
_tmp19B=_tmp19A.r;{struct Cyc_Absyn_Enum_td_struct*_tmp19C=(struct Cyc_Absyn_Enum_td_struct*)
_tmp19B;if(_tmp19C->tag != 1)goto _LLB5;else{_tmp19D=_tmp19C->f1;}};}}_LLB4: if(
_tmp188 != 0){const char*_tmp962;Cyc_Parse_err(((_tmp962="attributes on enum not supported",
_tag_dyneither(_tmp962,sizeof(char),33))),loc);}{struct Cyc_Absyn_Enum_d_struct*
_tmp968;struct Cyc_Absyn_Enum_d_struct _tmp967;struct Cyc_List_List*_tmp966;struct
Cyc_List_List*_tmp1C2=(_tmp966=_cycalloc(sizeof(*_tmp966)),((_tmp966->hd=Cyc_Absyn_new_decl((
void*)((_tmp968=_cycalloc(sizeof(*_tmp968)),((_tmp968[0]=((_tmp967.tag=8,((
_tmp967.f1=_tmp19D,_tmp967)))),_tmp968)))),loc),((_tmp966->tl=0,_tmp966)))));
_npop_handler(0);return _tmp1C2;};_LLB5: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp19E=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp191;if(_tmp19E->tag != 26)goto
_LLB7;else{_tmp19F=_tmp19E->f1;_tmp1A0=*_tmp19F;_tmp1A1=_tmp1A0.r;{struct Cyc_Absyn_Datatype_td_struct*
_tmp1A2=(struct Cyc_Absyn_Datatype_td_struct*)_tmp1A1;if(_tmp1A2->tag != 2)goto
_LLB7;else{_tmp1A3=_tmp1A2->f1;}};}}_LLB6: if(_tmp188 != 0){const char*_tmp969;Cyc_Parse_err(((
_tmp969="attributes on datatypes not supported",_tag_dyneither(_tmp969,sizeof(
char),38))),loc);}{struct Cyc_Absyn_Datatype_d_struct*_tmp96F;struct Cyc_Absyn_Datatype_d_struct
_tmp96E;struct Cyc_List_List*_tmp96D;struct Cyc_List_List*_tmp1C7=(_tmp96D=
_cycalloc(sizeof(*_tmp96D)),((_tmp96D->hd=Cyc_Absyn_new_decl((void*)((_tmp96F=
_cycalloc(sizeof(*_tmp96F)),((_tmp96F[0]=((_tmp96E.tag=7,((_tmp96E.f1=_tmp1A3,
_tmp96E)))),_tmp96F)))),loc),((_tmp96D->tl=0,_tmp96D)))));_npop_handler(0);
return _tmp1C7;};_LLB7: {struct Cyc_Absyn_AggrType_struct*_tmp1A4=(struct Cyc_Absyn_AggrType_struct*)
_tmp191;if(_tmp1A4->tag != 12)goto _LLB9;else{_tmp1A5=_tmp1A4->f1;_tmp1A6=_tmp1A5.aggr_info;
if((_tmp1A6.UnknownAggr).tag != 1)goto _LLB9;_tmp1A7=(struct _tuple3)(_tmp1A6.UnknownAggr).val;
_tmp1A8=_tmp1A7.f1;_tmp1A9=_tmp1A7.f2;_tmp1AA=_tmp1A5.targs;}}_LLB8: {struct Cyc_List_List*
_tmp1C8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp1AA);struct Cyc_Absyn_Aggrdecl*_tmp970;struct Cyc_Absyn_Aggrdecl*_tmp1C9=(
_tmp970=_cycalloc(sizeof(*_tmp970)),((_tmp970->kind=_tmp1A8,((_tmp970->sc=s,((
_tmp970->name=_tmp1A9,((_tmp970->tvs=_tmp1C8,((_tmp970->impl=0,((_tmp970->attributes=
0,_tmp970)))))))))))));if(_tmp188 != 0){const char*_tmp971;Cyc_Parse_err(((_tmp971="bad attributes on type declaration",
_tag_dyneither(_tmp971,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp977;struct Cyc_Absyn_Aggr_d_struct _tmp976;struct Cyc_List_List*_tmp975;struct
Cyc_List_List*_tmp1CE=(_tmp975=_cycalloc(sizeof(*_tmp975)),((_tmp975->hd=Cyc_Absyn_new_decl((
void*)((_tmp977=_cycalloc(sizeof(*_tmp977)),((_tmp977[0]=((_tmp976.tag=6,((
_tmp976.f1=_tmp1C9,_tmp976)))),_tmp977)))),loc),((_tmp975->tl=0,_tmp975)))));
_npop_handler(0);return _tmp1CE;};}_LLB9: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1AB=(struct Cyc_Absyn_DatatypeType_struct*)_tmp191;if(_tmp1AB->tag != 3)goto
_LLBB;else{_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AC.datatype_info;if((_tmp1AD.KnownDatatype).tag
!= 2)goto _LLBB;_tmp1AE=(struct Cyc_Absyn_Datatypedecl**)(_tmp1AD.KnownDatatype).val;}}
_LLBA: if(_tmp188 != 0){const char*_tmp978;Cyc_Parse_err(((_tmp978="bad attributes on datatype",
_tag_dyneither(_tmp978,sizeof(char),27))),loc);}{struct Cyc_Absyn_Datatype_d_struct*
_tmp97E;struct Cyc_Absyn_Datatype_d_struct _tmp97D;struct Cyc_List_List*_tmp97C;
struct Cyc_List_List*_tmp1D4=(_tmp97C=_cycalloc(sizeof(*_tmp97C)),((_tmp97C->hd=
Cyc_Absyn_new_decl((void*)((_tmp97E=_cycalloc(sizeof(*_tmp97E)),((_tmp97E[0]=((
_tmp97D.tag=7,((_tmp97D.f1=*_tmp1AE,_tmp97D)))),_tmp97E)))),loc),((_tmp97C->tl=0,
_tmp97C)))));_npop_handler(0);return _tmp1D4;};_LLBB: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1AF=(struct Cyc_Absyn_DatatypeType_struct*)_tmp191;if(_tmp1AF->tag != 3)goto
_LLBD;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1B0.datatype_info;if((_tmp1B1.UnknownDatatype).tag
!= 1)goto _LLBD;_tmp1B2=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B1.UnknownDatatype).val;
_tmp1B3=_tmp1B2.name;_tmp1B4=_tmp1B2.is_extensible;_tmp1B5=_tmp1B0.targs;}}_LLBC: {
struct Cyc_List_List*_tmp1D5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp1B5);struct Cyc_Absyn_Decl*_tmp1D6=
Cyc_Absyn_datatype_decl(s,_tmp1B3,_tmp1D5,0,_tmp1B4,loc);if(_tmp188 != 0){const
char*_tmp97F;Cyc_Parse_err(((_tmp97F="bad attributes on datatype",_tag_dyneither(
_tmp97F,sizeof(char),27))),loc);}{struct Cyc_List_List*_tmp980;struct Cyc_List_List*
_tmp1D9=(_tmp980=_cycalloc(sizeof(*_tmp980)),((_tmp980->hd=_tmp1D6,((_tmp980->tl=
0,_tmp980)))));_npop_handler(0);return _tmp1D9;};}_LLBD: {struct Cyc_Absyn_EnumType_struct*
_tmp1B6=(struct Cyc_Absyn_EnumType_struct*)_tmp191;if(_tmp1B6->tag != 14)goto _LLBF;
else{_tmp1B7=_tmp1B6->f1;}}_LLBE: {struct Cyc_Absyn_Enumdecl*_tmp981;struct Cyc_Absyn_Enumdecl*
_tmp1DA=(_tmp981=_cycalloc(sizeof(*_tmp981)),((_tmp981->sc=s,((_tmp981->name=
_tmp1B7,((_tmp981->fields=0,_tmp981)))))));if(_tmp188 != 0){const char*_tmp982;Cyc_Parse_err(((
_tmp982="bad attributes on enum",_tag_dyneither(_tmp982,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp98C;struct Cyc_Absyn_Enum_d_struct _tmp98B;struct Cyc_Absyn_Enum_d_struct*
_tmp98A;struct Cyc_List_List*_tmp989;struct Cyc_List_List*_tmp1E0=(_tmp989=
_cycalloc(sizeof(*_tmp989)),((_tmp989->hd=((_tmp98C=_cycalloc(sizeof(*_tmp98C)),((
_tmp98C->r=(void*)((_tmp98A=_cycalloc(sizeof(*_tmp98A)),((_tmp98A[0]=((_tmp98B.tag=
8,((_tmp98B.f1=_tmp1DA,_tmp98B)))),_tmp98A)))),((_tmp98C->loc=loc,_tmp98C)))))),((
_tmp989->tl=0,_tmp989)))));_npop_handler(0);return _tmp1E0;};}_LLBF: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1B8=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp191;if(_tmp1B8->tag != 15)goto
_LLC1;else{_tmp1B9=_tmp1B8->f1;}}_LLC0: {struct Cyc_Core_Opt*_tmp98F;struct Cyc_Absyn_Enumdecl*
_tmp98E;struct Cyc_Absyn_Enumdecl*_tmp1E2=(_tmp98E=_cycalloc(sizeof(*_tmp98E)),((
_tmp98E->sc=s,((_tmp98E->name=Cyc_Parse_gensym_enum(),((_tmp98E->fields=((
_tmp98F=_cycalloc(sizeof(*_tmp98F)),((_tmp98F->v=_tmp1B9,_tmp98F)))),_tmp98E)))))));
if(_tmp188 != 0){const char*_tmp990;Cyc_Parse_err(((_tmp990="bad attributes on enum",
_tag_dyneither(_tmp990,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp99A;
struct Cyc_Absyn_Enum_d_struct _tmp999;struct Cyc_Absyn_Enum_d_struct*_tmp998;
struct Cyc_List_List*_tmp997;struct Cyc_List_List*_tmp1E8=(_tmp997=_cycalloc(
sizeof(*_tmp997)),((_tmp997->hd=((_tmp99A=_cycalloc(sizeof(*_tmp99A)),((_tmp99A->r=(
void*)((_tmp998=_cycalloc(sizeof(*_tmp998)),((_tmp998[0]=((_tmp999.tag=8,((
_tmp999.f1=_tmp1E2,_tmp999)))),_tmp998)))),((_tmp99A->loc=loc,_tmp99A)))))),((
_tmp997->tl=0,_tmp997)))));_npop_handler(0);return _tmp1E8;};}_LLC1:;_LLC2:{const
char*_tmp99B;Cyc_Parse_err(((_tmp99B="missing declarator",_tag_dyneither(_tmp99B,
sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1EC=0;_npop_handler(0);return
_tmp1EC;};_LLB0:;}else{struct Cyc_List_List*_tmp1ED=Cyc_Parse_apply_tmss(mkrgn,
_tmp186,_tmp190,_tmp18E,_tmp188);if(istypedef){if(!exps_empty){const char*_tmp99C;
Cyc_Parse_err(((_tmp99C="initializer in typedef declaration",_tag_dyneither(
_tmp99C,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp1ED);struct Cyc_List_List*_tmp1EF=decls;_npop_handler(0);return _tmp1EF;};}
else{struct Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp1F0=_tmp1ED;for(0;
_tmp1F0 != 0;(_tmp1F0=_tmp1F0->tl,_tmp18F=_tmp18F->tl)){struct _tuple12 _tmp1F2;
struct _tuple1*_tmp1F3;struct Cyc_Absyn_Tqual _tmp1F4;void*_tmp1F5;struct Cyc_List_List*
_tmp1F6;struct Cyc_List_List*_tmp1F7;struct _tuple12*_tmp1F1=(struct _tuple12*)
_tmp1F0->hd;_tmp1F2=*_tmp1F1;_tmp1F3=_tmp1F2.f1;_tmp1F4=_tmp1F2.f2;_tmp1F5=
_tmp1F2.f3;_tmp1F6=_tmp1F2.f4;_tmp1F7=_tmp1F2.f5;if(_tmp1F6 != 0){const char*
_tmp99F;void*_tmp99E;(_tmp99E=0,Cyc_Tcutil_warn(loc,((_tmp99F="bad type params, ignoring",
_tag_dyneither(_tmp99F,sizeof(char),26))),_tag_dyneither(_tmp99E,sizeof(void*),0)));}
if(_tmp18F == 0){const char*_tmp9A2;void*_tmp9A1;(_tmp9A1=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp9A2="unexpected NULL in parse!",_tag_dyneither(_tmp9A2,sizeof(char),26))),
_tag_dyneither(_tmp9A1,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*_tmp1FC=(struct
Cyc_Absyn_Exp*)_tmp18F->hd;struct Cyc_Absyn_Vardecl*_tmp1FD=Cyc_Absyn_new_vardecl(
_tmp1F3,_tmp1F5,_tmp1FC);_tmp1FD->tq=_tmp1F4;_tmp1FD->sc=s;_tmp1FD->attributes=
_tmp1F7;{struct Cyc_Absyn_Var_d_struct*_tmp9A8;struct Cyc_Absyn_Var_d_struct
_tmp9A7;struct Cyc_Absyn_Decl*_tmp9A6;struct Cyc_Absyn_Decl*_tmp1FE=(_tmp9A6=
_cycalloc(sizeof(*_tmp9A6)),((_tmp9A6->r=(void*)((_tmp9A8=_cycalloc(sizeof(*
_tmp9A8)),((_tmp9A8[0]=((_tmp9A7.tag=0,((_tmp9A7.f1=_tmp1FD,_tmp9A7)))),_tmp9A8)))),((
_tmp9A6->loc=loc,_tmp9A6)))));struct Cyc_List_List*_tmp9A9;decls=((_tmp9A9=
_cycalloc(sizeof(*_tmp9A9)),((_tmp9A9->hd=_tmp1FE,((_tmp9A9->tl=decls,_tmp9A9))))));};};}}{
struct Cyc_List_List*_tmp203=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
decls);_npop_handler(0);return _tmp203;};}}};};};};};_pop_region(mkrgn);}static
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
_LLD6: break;}break;default: _LLD1: break;}{const char*_tmp9AE;void*_tmp9AD[2];
struct Cyc_String_pa_struct _tmp9AC;struct Cyc_Int_pa_struct _tmp9AB;Cyc_Parse_err((
struct _dyneither_ptr)((_tmp9AB.tag=1,((_tmp9AB.f1=(unsigned long)((int)Cyc_strlen((
struct _dyneither_ptr)s)),((_tmp9AC.tag=0,((_tmp9AC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s),((_tmp9AD[0]=& _tmp9AC,((_tmp9AD[1]=& _tmp9AB,Cyc_aprintf(((
_tmp9AE="bad kind: %s; strlen=%d",_tag_dyneither(_tmp9AE,sizeof(char),24))),
_tag_dyneither(_tmp9AD,sizeof(void*),2)))))))))))))),loc);}return& Cyc_Tcutil_bk;}
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*_tmp208=
p->r;struct _tuple1*_tmp20A;struct Cyc_Absyn_Vardecl*_tmp20C;struct Cyc_Absyn_Pat*
_tmp20D;struct Cyc_Absyn_Pat _tmp20E;void*_tmp20F;struct Cyc_Absyn_Pat*_tmp212;
enum Cyc_Absyn_Sign _tmp215;int _tmp216;char _tmp218;struct _dyneither_ptr _tmp21A;
struct _tuple1*_tmp21C;struct Cyc_List_List*_tmp21D;int _tmp21E;struct Cyc_Absyn_Exp*
_tmp220;_LLDA: {struct Cyc_Absyn_UnknownId_p_struct*_tmp209=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp208;if(_tmp209->tag != 14)goto _LLDC;else{_tmp20A=_tmp209->f1;}}_LLDB: {struct
Cyc_Absyn_UnknownId_e_struct _tmp9B1;struct Cyc_Absyn_UnknownId_e_struct*_tmp9B0;
return Cyc_Absyn_new_exp((void*)((_tmp9B0=_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0[0]=((
_tmp9B1.tag=2,((_tmp9B1.f1=_tmp20A,_tmp9B1)))),_tmp9B0)))),p->loc);}_LLDC: {
struct Cyc_Absyn_Reference_p_struct*_tmp20B=(struct Cyc_Absyn_Reference_p_struct*)
_tmp208;if(_tmp20B->tag != 2)goto _LLDE;else{_tmp20C=_tmp20B->f1;_tmp20D=_tmp20B->f2;
_tmp20E=*_tmp20D;_tmp20F=_tmp20E.r;{struct Cyc_Absyn_Wild_p_struct*_tmp210=(
struct Cyc_Absyn_Wild_p_struct*)_tmp20F;if(_tmp210->tag != 0)goto _LLDE;};}}_LLDD: {
struct Cyc_Absyn_UnknownId_e_struct _tmp9B4;struct Cyc_Absyn_UnknownId_e_struct*
_tmp9B3;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmp9B3=_cycalloc(
sizeof(*_tmp9B3)),((_tmp9B3[0]=((_tmp9B4.tag=2,((_tmp9B4.f1=_tmp20C->name,
_tmp9B4)))),_tmp9B3)))),p->loc),p->loc);}_LLDE: {struct Cyc_Absyn_Pointer_p_struct*
_tmp211=(struct Cyc_Absyn_Pointer_p_struct*)_tmp208;if(_tmp211->tag != 5)goto _LLE0;
else{_tmp212=_tmp211->f1;}}_LLDF: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(
_tmp212),p->loc);_LLE0: {struct Cyc_Absyn_Null_p_struct*_tmp213=(struct Cyc_Absyn_Null_p_struct*)
_tmp208;if(_tmp213->tag != 8)goto _LLE2;}_LLE1: return Cyc_Absyn_null_exp(p->loc);
_LLE2: {struct Cyc_Absyn_Int_p_struct*_tmp214=(struct Cyc_Absyn_Int_p_struct*)
_tmp208;if(_tmp214->tag != 9)goto _LLE4;else{_tmp215=_tmp214->f1;_tmp216=_tmp214->f2;}}
_LLE3: return Cyc_Absyn_int_exp(_tmp215,_tmp216,p->loc);_LLE4: {struct Cyc_Absyn_Char_p_struct*
_tmp217=(struct Cyc_Absyn_Char_p_struct*)_tmp208;if(_tmp217->tag != 10)goto _LLE6;
else{_tmp218=_tmp217->f1;}}_LLE5: return Cyc_Absyn_char_exp(_tmp218,p->loc);_LLE6: {
struct Cyc_Absyn_Float_p_struct*_tmp219=(struct Cyc_Absyn_Float_p_struct*)_tmp208;
if(_tmp219->tag != 11)goto _LLE8;else{_tmp21A=_tmp219->f1;}}_LLE7: return Cyc_Absyn_float_exp(
_tmp21A,p->loc);_LLE8: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp21B=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp208;if(_tmp21B->tag != 15)goto _LLEA;else{_tmp21C=_tmp21B->f1;_tmp21D=_tmp21B->f2;
_tmp21E=_tmp21B->f3;if(_tmp21E != 0)goto _LLEA;}}_LLE9: {struct Cyc_Absyn_UnknownId_e_struct
_tmp9B7;struct Cyc_Absyn_UnknownId_e_struct*_tmp9B6;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmp9B6=_cycalloc(sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B7.tag=2,((
_tmp9B7.f1=_tmp21C,_tmp9B7)))),_tmp9B6)))),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp21D);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLEA: {struct Cyc_Absyn_Exp_p_struct*_tmp21F=(struct Cyc_Absyn_Exp_p_struct*)
_tmp208;if(_tmp21F->tag != 16)goto _LLEC;else{_tmp220=_tmp21F->f1;}}_LLEB: return
_tmp220;_LLEC:;_LLED:{const char*_tmp9B8;Cyc_Parse_err(((_tmp9B8="cannot mix patterns and expressions in case",
_tag_dyneither(_tmp9B8,sizeof(char),44))),p->loc);}return Cyc_Absyn_null_exp(p->loc);
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
_union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp229[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp229,
_tmp229,_tmp229 + 14}};struct _tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){struct
_tuple6 yyzzz;{union Cyc_YYSTYPE _tmp22A=yy1;struct _tuple6 _tmp22B;_LLEF: if((_tmp22A.Int_tok).tag
!= 1)goto _LLF1;_tmp22B=(struct _tuple6)(_tmp22A.Int_tok).val;_LLF0: yyzzz=_tmp22B;
goto _LLEE;_LLF1:;_LLF2:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLEE:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){union Cyc_YYSTYPE _tmp9B9;
return((_tmp9B9.Int_tok).val=yy1,(((_tmp9B9.Int_tok).tag=1,_tmp9B9)));}static
char _tmp22E[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp22E,_tmp22E,_tmp22E + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE
yy1){char yyzzz;{union Cyc_YYSTYPE _tmp22F=yy1;char _tmp230;_LLF4: if((_tmp22F.Char_tok).tag
!= 2)goto _LLF6;_tmp230=(char)(_tmp22F.Char_tok).val;_LLF5: yyzzz=_tmp230;goto
_LLF3;_LLF6:;_LLF7:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLF3:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9BA;return((_tmp9BA.Char_tok).val=
yy1,(((_tmp9BA.Char_tok).tag=2,_tmp9BA)));}static char _tmp233[13]="string_t<`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{
_tmp233,_tmp233,_tmp233 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp234=yy1;struct _dyneither_ptr
_tmp235;_LLF9: if((_tmp234.String_tok).tag != 3)goto _LLFB;_tmp235=(struct
_dyneither_ptr)(_tmp234.String_tok).val;_LLFA: yyzzz=_tmp235;goto _LLF8;_LLFB:;
_LLFC:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLF8:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9BB;return((_tmp9BB.String_tok).val=
yy1,(((_tmp9BB.String_tok).tag=3,_tmp9BB)));}static char _tmp238[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp238,
_tmp238,_tmp238 + 56}};struct _tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct
_tuple18*yyzzz;{union Cyc_YYSTYPE _tmp239=yy1;struct _tuple18*_tmp23A;_LLFE: if((
_tmp239.YY1).tag != 7)goto _LL100;_tmp23A=(struct _tuple18*)(_tmp239.YY1).val;_LLFF:
yyzzz=_tmp23A;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)& Cyc_yyfail_YY1);_LLFD:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY1(struct _tuple18*yy1){union Cyc_YYSTYPE _tmp9BC;
return((_tmp9BC.YY1).val=yy1,(((_tmp9BC.YY1).tag=7,_tmp9BC)));}static char _tmp23D[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp23D,_tmp23D,_tmp23D + 19}};union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE
yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp23E=yy1;union Cyc_Absyn_Constraint*
_tmp23F;_LL103: if((_tmp23E.YY2).tag != 8)goto _LL105;_tmp23F=(union Cyc_Absyn_Constraint*)(
_tmp23E.YY2).val;_LL104: yyzzz=_tmp23F;goto _LL102;_LL105:;_LL106:(int)_throw((
void*)& Cyc_yyfail_YY2);_LL102:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*
yy1){union Cyc_YYSTYPE _tmp9BD;return((_tmp9BD.YY2).val=yy1,(((_tmp9BD.YY2).tag=8,
_tmp9BD)));}static char _tmp242[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp242,_tmp242,_tmp242 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE _tmp243=yy1;
struct Cyc_Absyn_Exp*_tmp244;_LL108: if((_tmp243.YY3).tag != 9)goto _LL10A;_tmp244=(
struct Cyc_Absyn_Exp*)(_tmp243.YY3).val;_LL109: yyzzz=_tmp244;goto _LL107;_LL10A:;
_LL10B:(int)_throw((void*)& Cyc_yyfail_YY3);_LL107:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY3(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9BE;return((_tmp9BE.YY3).val=
yy1,(((_tmp9BE.YY3).tag=9,_tmp9BE)));}static char _tmp247[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp247,_tmp247,
_tmp247 + 10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE _tmp248=yy1;struct Cyc_Absyn_Exp*_tmp249;_LL10D: if((
_tmp248.YY4).tag != 10)goto _LL10F;_tmp249=(struct Cyc_Absyn_Exp*)(_tmp248.YY4).val;
_LL10E: yyzzz=_tmp249;goto _LL10C;_LL10F:;_LL110:(int)_throw((void*)& Cyc_yyfail_YY4);
_LL10C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE
_tmp9BF;return((_tmp9BF.YY4).val=yy1,(((_tmp9BF.YY4).tag=10,_tmp9BF)));}static
char _tmp24C[17]="list_t<exp_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={
Cyc_Core_Failure,{_tmp24C,_tmp24C,_tmp24C + 17}};struct Cyc_List_List*Cyc_yyget_YY5(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp24D=yy1;
struct Cyc_List_List*_tmp24E;_LL112: if((_tmp24D.YY5).tag != 11)goto _LL114;_tmp24E=(
struct Cyc_List_List*)(_tmp24D.YY5).val;_LL113: yyzzz=_tmp24E;goto _LL111;_LL114:;
_LL115:(int)_throw((void*)& Cyc_yyfail_YY5);_LL111:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C0;return((_tmp9C0.YY5).val=
yy1,(((_tmp9C0.YY5).tag=11,_tmp9C0)));}static char _tmp251[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp251,
_tmp251,_tmp251 + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp252=yy1;struct Cyc_List_List*
_tmp253;_LL117: if((_tmp252.YY6).tag != 12)goto _LL119;_tmp253=(struct Cyc_List_List*)(
_tmp252.YY6).val;_LL118: yyzzz=_tmp253;goto _LL116;_LL119:;_LL11A:(int)_throw((
void*)& Cyc_yyfail_YY6);_LL116:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9C1;return((_tmp9C1.YY6).val=yy1,(((_tmp9C1.YY6).tag=12,
_tmp9C1)));}static char _tmp256[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp256,_tmp256,_tmp256 + 9}};enum Cyc_Absyn_Primop
Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Primop yyzzz;{union Cyc_YYSTYPE
_tmp257=yy1;enum Cyc_Absyn_Primop _tmp258;_LL11C: if((_tmp257.YY7).tag != 13)goto
_LL11E;_tmp258=(enum Cyc_Absyn_Primop)(_tmp257.YY7).val;_LL11D: yyzzz=_tmp258;
goto _LL11B;_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY7);_LL11B:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9C2;
return((_tmp9C2.YY7).val=yy1,(((_tmp9C2.YY7).tag=13,_tmp9C2)));}static char
_tmp25B[19]="opt_t<primop_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={
Cyc_Core_Failure,{_tmp25B,_tmp25B,_tmp25B + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(
union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp25C=yy1;
struct Cyc_Core_Opt*_tmp25D;_LL121: if((_tmp25C.YY8).tag != 14)goto _LL123;_tmp25D=(
struct Cyc_Core_Opt*)(_tmp25C.YY8).val;_LL122: yyzzz=_tmp25D;goto _LL120;_LL123:;
_LL124:(int)_throw((void*)& Cyc_yyfail_YY8);_LL120:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY8(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9C3;return((_tmp9C3.YY8).val=
yy1,(((_tmp9C3.YY8).tag=14,_tmp9C3)));}static char _tmp260[7]="qvar_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp260,
_tmp260,_tmp260 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp261=yy1;struct _tuple1*_tmp262;_LL126:
if((_tmp261.QualId_tok).tag != 5)goto _LL128;_tmp262=(struct _tuple1*)(_tmp261.QualId_tok).val;
_LL127: yyzzz=_tmp262;goto _LL125;_LL128:;_LL129:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL125:;}return yyzzz;}union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple1*yy1){union Cyc_YYSTYPE
_tmp9C4;return((_tmp9C4.QualId_tok).val=yy1,(((_tmp9C4.QualId_tok).tag=5,_tmp9C4)));}
static char _tmp265[7]="stmt_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY9={
Cyc_Core_Failure,{_tmp265,_tmp265,_tmp265 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE _tmp266=yy1;
struct Cyc_Absyn_Stmt*_tmp267;_LL12B: if((_tmp266.YY9).tag != 15)goto _LL12D;_tmp267=(
struct Cyc_Absyn_Stmt*)(_tmp266.YY9).val;_LL12C: yyzzz=_tmp267;goto _LL12A;_LL12D:;
_LL12E:(int)_throw((void*)& Cyc_yyfail_YY9);_LL12A:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9C5;return((_tmp9C5.YY9).val=
yy1,(((_tmp9C5.YY9).tag=15,_tmp9C5)));}static char _tmp26A[27]="list_t<switch_clause_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp26A,
_tmp26A,_tmp26A + 27}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp26B=yy1;struct Cyc_List_List*
_tmp26C;_LL130: if((_tmp26B.YY10).tag != 16)goto _LL132;_tmp26C=(struct Cyc_List_List*)(
_tmp26B.YY10).val;_LL131: yyzzz=_tmp26C;goto _LL12F;_LL132:;_LL133:(int)_throw((
void*)& Cyc_yyfail_YY10);_LL12F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9C6;return((_tmp9C6.YY10).val=yy1,(((_tmp9C6.YY10).tag=
16,_tmp9C6)));}static char _tmp26F[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp26F,_tmp26F,_tmp26F + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE
_tmp270=yy1;struct Cyc_Absyn_Pat*_tmp271;_LL135: if((_tmp270.YY11).tag != 17)goto
_LL137;_tmp271=(struct Cyc_Absyn_Pat*)(_tmp270.YY11).val;_LL136: yyzzz=_tmp271;
goto _LL134;_LL137:;_LL138:(int)_throw((void*)& Cyc_yyfail_YY11);_LL134:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9C7;
return((_tmp9C7.YY11).val=yy1,(((_tmp9C7.YY11).tag=17,_tmp9C7)));}static char
_tmp274[28]="$(list_t<pat_t,`H>,bool)@`H";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={
Cyc_Core_Failure,{_tmp274,_tmp274,_tmp274 + 28}};struct _tuple19*Cyc_yyget_YY12(
union Cyc_YYSTYPE yy1){struct _tuple19*yyzzz;{union Cyc_YYSTYPE _tmp275=yy1;struct
_tuple19*_tmp276;_LL13A: if((_tmp275.YY12).tag != 18)goto _LL13C;_tmp276=(struct
_tuple19*)(_tmp275.YY12).val;_LL13B: yyzzz=_tmp276;goto _LL139;_LL13C:;_LL13D:(int)
_throw((void*)& Cyc_yyfail_YY12);_LL139:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY12(
struct _tuple19*yy1){union Cyc_YYSTYPE _tmp9C8;return((_tmp9C8.YY12).val=yy1,(((
_tmp9C8.YY12).tag=18,_tmp9C8)));}static char _tmp279[17]="list_t<pat_t,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp279,_tmp279,
_tmp279 + 17}};struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp27A=yy1;struct Cyc_List_List*_tmp27B;_LL13F: if((
_tmp27A.YY13).tag != 19)goto _LL141;_tmp27B=(struct Cyc_List_List*)(_tmp27A.YY13).val;
_LL140: yyzzz=_tmp27B;goto _LL13E;_LL141:;_LL142:(int)_throw((void*)& Cyc_yyfail_YY13);
_LL13E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9C9;return((_tmp9C9.YY13).val=yy1,(((_tmp9C9.YY13).tag=19,_tmp9C9)));}static
char _tmp27E[36]="$(list_t<designator_t,`H>,pat_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp27E,_tmp27E,_tmp27E + 36}};struct _tuple20*
Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){struct _tuple20*yyzzz;{union Cyc_YYSTYPE
_tmp27F=yy1;struct _tuple20*_tmp280;_LL144: if((_tmp27F.YY14).tag != 20)goto _LL146;
_tmp280=(struct _tuple20*)(_tmp27F.YY14).val;_LL145: yyzzz=_tmp280;goto _LL143;
_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY14);_LL143:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY14(struct _tuple20*yy1){union Cyc_YYSTYPE _tmp9CA;return((_tmp9CA.YY14).val=
yy1,(((_tmp9CA.YY14).tag=20,_tmp9CA)));}static char _tmp283[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp283,
_tmp283,_tmp283 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp284=yy1;struct Cyc_List_List*
_tmp285;_LL149: if((_tmp284.YY15).tag != 21)goto _LL14B;_tmp285=(struct Cyc_List_List*)(
_tmp284.YY15).val;_LL14A: yyzzz=_tmp285;goto _LL148;_LL14B:;_LL14C:(int)_throw((
void*)& Cyc_yyfail_YY15);_LL148:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9CB;return((_tmp9CB.YY15).val=yy1,(((_tmp9CB.YY15).tag=
21,_tmp9CB)));}static char _tmp288[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp288,
_tmp288,_tmp288 + 58}};struct _tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct
_tuple19*yyzzz;{union Cyc_YYSTYPE _tmp289=yy1;struct _tuple19*_tmp28A;_LL14E: if((
_tmp289.YY16).tag != 22)goto _LL150;_tmp28A=(struct _tuple19*)(_tmp289.YY16).val;
_LL14F: yyzzz=_tmp28A;goto _LL14D;_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY16);
_LL14D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY16(struct _tuple19*yy1){union Cyc_YYSTYPE
_tmp9CC;return((_tmp9CC.YY16).val=yy1,(((_tmp9CC.YY16).tag=22,_tmp9CC)));}static
char _tmp28D[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp28D,_tmp28D,_tmp28D + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp28E=yy1;
struct Cyc_Absyn_Fndecl*_tmp28F;_LL153: if((_tmp28E.YY17).tag != 23)goto _LL155;
_tmp28F=(struct Cyc_Absyn_Fndecl*)(_tmp28E.YY17).val;_LL154: yyzzz=_tmp28F;goto
_LL152;_LL155:;_LL156:(int)_throw((void*)& Cyc_yyfail_YY17);_LL152:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9CD;
return((_tmp9CD.YY17).val=yy1,(((_tmp9CD.YY17).tag=23,_tmp9CD)));}static char
_tmp292[18]="list_t<decl_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp292,_tmp292,_tmp292 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp293=yy1;
struct Cyc_List_List*_tmp294;_LL158: if((_tmp293.YY18).tag != 24)goto _LL15A;_tmp294=(
struct Cyc_List_List*)(_tmp293.YY18).val;_LL159: yyzzz=_tmp294;goto _LL157;_LL15A:;
_LL15B:(int)_throw((void*)& Cyc_yyfail_YY18);_LL157:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CE;return((_tmp9CE.YY18).val=
yy1,(((_tmp9CE.YY18).tag=24,_tmp9CE)));}static char _tmp297[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp297,
_tmp297,_tmp297 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp298=yy1;struct
Cyc_Parse_Declaration_spec*_tmp299;_LL15D: if((_tmp298.YY19).tag != 25)goto _LL15F;
_tmp299=(struct Cyc_Parse_Declaration_spec*)(_tmp298.YY19).val;_LL15E: yyzzz=
_tmp299;goto _LL15C;_LL15F:;_LL160:(int)_throw((void*)& Cyc_yyfail_YY19);_LL15C:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){
union Cyc_YYSTYPE _tmp9CF;return((_tmp9CF.YY19).val=yy1,(((_tmp9CF.YY19).tag=25,
_tmp9CF)));}static char _tmp29C[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp29C,_tmp29C,_tmp29C + 29}};struct _tuple21*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE
_tmp29D=yy1;struct _tuple21*_tmp29E;_LL162: if((_tmp29D.YY20).tag != 26)goto _LL164;
_tmp29E=(struct _tuple21*)(_tmp29D.YY20).val;_LL163: yyzzz=_tmp29E;goto _LL161;
_LL164:;_LL165:(int)_throw((void*)& Cyc_yyfail_YY20);_LL161:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY20(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9D0;return((_tmp9D0.YY20).val=
yy1,(((_tmp9D0.YY20).tag=26,_tmp9D0)));}static char _tmp2A1[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp2A1,
_tmp2A1,_tmp2A1 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2A2=yy1;struct Cyc_List_List*
_tmp2A3;_LL167: if((_tmp2A2.YY21).tag != 27)goto _LL169;_tmp2A3=(struct Cyc_List_List*)(
_tmp2A2.YY21).val;_LL168: yyzzz=_tmp2A3;goto _LL166;_LL169:;_LL16A:(int)_throw((
void*)& Cyc_yyfail_YY21);_LL166:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9D1;return((_tmp9D1.YY21).val=yy1,(((_tmp9D1.YY21).tag=
27,_tmp9D1)));}static char _tmp2A6[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp2A6,_tmp2A6,_tmp2A6 + 16}};enum Cyc_Parse_Storage_class
Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){enum Cyc_Parse_Storage_class yyzzz;{union Cyc_YYSTYPE
_tmp2A7=yy1;enum Cyc_Parse_Storage_class _tmp2A8;_LL16C: if((_tmp2A7.YY22).tag != 
28)goto _LL16E;_tmp2A8=(enum Cyc_Parse_Storage_class)(_tmp2A7.YY22).val;_LL16D:
yyzzz=_tmp2A8;goto _LL16B;_LL16E:;_LL16F:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL16B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class yy1){
union Cyc_YYSTYPE _tmp9D2;return((_tmp9D2.YY22).val=yy1,(((_tmp9D2.YY22).tag=28,
_tmp9D2)));}static char _tmp2AB[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp2AB,_tmp2AB,_tmp2AB + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2AC=yy1;void*_tmp2AD;_LL171:
if((_tmp2AC.YY23).tag != 29)goto _LL173;_tmp2AD=(void*)(_tmp2AC.YY23).val;_LL172:
yyzzz=_tmp2AD;goto _LL170;_LL173:;_LL174:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL170:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(void*yy1){union Cyc_YYSTYPE _tmp9D3;
return((_tmp9D3.YY23).val=yy1,(((_tmp9D3.YY23).tag=29,_tmp9D3)));}static char
_tmp2B0[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={
Cyc_Core_Failure,{_tmp2B0,_tmp2B0,_tmp2B0 + 12}};enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(
union Cyc_YYSTYPE yy1){enum Cyc_Absyn_AggrKind yyzzz;{union Cyc_YYSTYPE _tmp2B1=yy1;
enum Cyc_Absyn_AggrKind _tmp2B2;_LL176: if((_tmp2B1.YY24).tag != 30)goto _LL178;
_tmp2B2=(enum Cyc_Absyn_AggrKind)(_tmp2B1.YY24).val;_LL177: yyzzz=_tmp2B2;goto
_LL175;_LL178:;_LL179:(int)_throw((void*)& Cyc_yyfail_YY24);_LL175:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9D4;
return((_tmp9D4.YY24).val=yy1,(((_tmp9D4.YY24).tag=30,_tmp9D4)));}static char
_tmp2B5[8]="tqual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{
_tmp2B5,_tmp2B5,_tmp2B5 + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE _tmp2B6=yy1;struct Cyc_Absyn_Tqual
_tmp2B7;_LL17B: if((_tmp2B6.YY25).tag != 31)goto _LL17D;_tmp2B7=(struct Cyc_Absyn_Tqual)(
_tmp2B6.YY25).val;_LL17C: yyzzz=_tmp2B7;goto _LL17A;_LL17D:;_LL17E:(int)_throw((
void*)& Cyc_yyfail_YY25);_LL17A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual
yy1){union Cyc_YYSTYPE _tmp9D5;return((_tmp9D5.YY25).val=yy1,(((_tmp9D5.YY25).tag=
31,_tmp9D5)));}static char _tmp2BA[23]="list_t<aggrfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp2BA,_tmp2BA,_tmp2BA + 23}};struct Cyc_List_List*
Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2BB=yy1;struct Cyc_List_List*_tmp2BC;_LL180: if((_tmp2BB.YY26).tag != 32)goto
_LL182;_tmp2BC=(struct Cyc_List_List*)(_tmp2BB.YY26).val;_LL181: yyzzz=_tmp2BC;
goto _LL17F;_LL182:;_LL183:(int)_throw((void*)& Cyc_yyfail_YY26);_LL17F:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D6;
return((_tmp9D6.YY26).val=yy1,(((_tmp9D6.YY26).tag=32,_tmp9D6)));}static char
_tmp2BF[34]="list_t<list_t<aggrfield_t,`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2BF,_tmp2BF,_tmp2BF + 34}};struct Cyc_List_List*
Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2C0=yy1;struct Cyc_List_List*_tmp2C1;_LL185: if((_tmp2C0.YY27).tag != 33)goto
_LL187;_tmp2C1=(struct Cyc_List_List*)(_tmp2C0.YY27).val;_LL186: yyzzz=_tmp2C1;
goto _LL184;_LL187:;_LL188:(int)_throw((void*)& Cyc_yyfail_YY27);_LL184:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D7;
return((_tmp9D7.YY27).val=yy1,(((_tmp9D7.YY27).tag=33,_tmp9D7)));}static char
_tmp2C4[31]="list_t<type_modifier_t<`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2C4,_tmp2C4,_tmp2C4 + 31}};struct Cyc_List_List*
Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2C5=yy1;struct Cyc_List_List*_tmp2C6;_LL18A: if((_tmp2C5.YY28).tag != 34)goto
_LL18C;_tmp2C6=(struct Cyc_List_List*)(_tmp2C5.YY28).val;_LL18B: yyzzz=_tmp2C6;
goto _LL189;_LL18C:;_LL18D:(int)_throw((void*)& Cyc_yyfail_YY28);_LL189:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D8;
return((_tmp9D8.YY28).val=yy1,(((_tmp9D8.YY28).tag=34,_tmp9D8)));}static char
_tmp2C9[13]="declarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={
Cyc_Core_Failure,{_tmp2C9,_tmp2C9,_tmp2C9 + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE _tmp2CA=
yy1;struct Cyc_Parse_Declarator*_tmp2CB;_LL18F: if((_tmp2CA.YY29).tag != 35)goto
_LL191;_tmp2CB=(struct Cyc_Parse_Declarator*)(_tmp2CA.YY29).val;_LL190: yyzzz=
_tmp2CB;goto _LL18E;_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY29);_LL18E:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){union Cyc_YYSTYPE
_tmp9D9;return((_tmp9D9.YY29).val=yy1,(((_tmp9D9.YY29).tag=35,_tmp9D9)));}static
char _tmp2CE[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={
Cyc_Core_Failure,{_tmp2CE,_tmp2CE,_tmp2CE + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{
union Cyc_YYSTYPE _tmp2CF=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp2D0;_LL194:
if((_tmp2CF.YY30).tag != 36)goto _LL196;_tmp2D0=(struct Cyc_Parse_Abstractdeclarator*)(
_tmp2CF.YY30).val;_LL195: yyzzz=_tmp2D0;goto _LL193;_LL196:;_LL197:(int)_throw((
void*)& Cyc_yyfail_YY30);_LL193:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1){union Cyc_YYSTYPE _tmp9DA;return((_tmp9DA.YY30).val=yy1,(((_tmp9DA.YY30).tag=
36,_tmp9DA)));}static char _tmp2D3[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2D3,_tmp2D3,_tmp2D3 + 5}};int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp2D4=yy1;int _tmp2D5;_LL199: if((
_tmp2D4.YY31).tag != 37)goto _LL19B;_tmp2D5=(int)(_tmp2D4.YY31).val;_LL19A: yyzzz=
_tmp2D5;goto _LL198;_LL19B:;_LL19C:(int)_throw((void*)& Cyc_yyfail_YY31);_LL198:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp9DB;return((
_tmp9DB.YY31).val=yy1,(((_tmp9DB.YY31).tag=37,_tmp9DB)));}static char _tmp2D8[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2D8,
_tmp2D8,_tmp2D8 + 8}};enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;{union Cyc_YYSTYPE _tmp2D9=yy1;enum Cyc_Absyn_Scope
_tmp2DA;_LL19E: if((_tmp2D9.YY32).tag != 38)goto _LL1A0;_tmp2DA=(enum Cyc_Absyn_Scope)(
_tmp2D9.YY32).val;_LL19F: yyzzz=_tmp2DA;goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((
void*)& Cyc_yyfail_YY32);_LL19D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope
yy1){union Cyc_YYSTYPE _tmp9DC;return((_tmp9DC.YY32).val=yy1,(((_tmp9DC.YY32).tag=
38,_tmp9DC)));}static char _tmp2DD[16]="datatypefield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2DD,_tmp2DD,_tmp2DD + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union
Cyc_YYSTYPE _tmp2DE=yy1;struct Cyc_Absyn_Datatypefield*_tmp2DF;_LL1A3: if((_tmp2DE.YY33).tag
!= 39)goto _LL1A5;_tmp2DF=(struct Cyc_Absyn_Datatypefield*)(_tmp2DE.YY33).val;
_LL1A4: yyzzz=_tmp2DF;goto _LL1A2;_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL1A2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){
union Cyc_YYSTYPE _tmp9DD;return((_tmp9DD.YY33).val=yy1,(((_tmp9DD.YY33).tag=39,
_tmp9DD)));}static char _tmp2E2[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2E2,_tmp2E2,_tmp2E2 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2E3=yy1;struct Cyc_List_List*_tmp2E4;_LL1A8: if((_tmp2E3.YY34).tag != 40)goto
_LL1AA;_tmp2E4=(struct Cyc_List_List*)(_tmp2E3.YY34).val;_LL1A9: yyzzz=_tmp2E4;
goto _LL1A7;_LL1AA:;_LL1AB:(int)_throw((void*)& Cyc_yyfail_YY34);_LL1A7:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DE;
return((_tmp9DE.YY34).val=yy1,(((_tmp9DE.YY34).tag=40,_tmp9DE)));}static char
_tmp2E7[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2E7,_tmp2E7,
_tmp2E7 + 55}};struct _tuple22*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple22*
yyzzz;{union Cyc_YYSTYPE _tmp2E8=yy1;struct _tuple22*_tmp2E9;_LL1AD: if((_tmp2E8.YY35).tag
!= 41)goto _LL1AF;_tmp2E9=(struct _tuple22*)(_tmp2E8.YY35).val;_LL1AE: yyzzz=
_tmp2E9;goto _LL1AC;_LL1AF:;_LL1B0:(int)_throw((void*)& Cyc_yyfail_YY35);_LL1AC:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY35(struct _tuple22*yy1){union Cyc_YYSTYPE
_tmp9DF;return((_tmp9DF.YY35).val=yy1,(((_tmp9DF.YY35).tag=41,_tmp9DF)));}static
char _tmp2EC[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={
Cyc_Core_Failure,{_tmp2EC,_tmp2EC,_tmp2EC + 17}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2ED=yy1;
struct Cyc_List_List*_tmp2EE;_LL1B2: if((_tmp2ED.YY36).tag != 42)goto _LL1B4;_tmp2EE=(
struct Cyc_List_List*)(_tmp2ED.YY36).val;_LL1B3: yyzzz=_tmp2EE;goto _LL1B1;_LL1B4:;
_LL1B5:(int)_throw((void*)& Cyc_yyfail_YY36);_LL1B1:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E0;return((_tmp9E0.YY36).val=
yy1,(((_tmp9E0.YY36).tag=42,_tmp9E0)));}static char _tmp2F1[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2F1,
_tmp2F1,_tmp2F1 + 37}};struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){struct
_tuple8*yyzzz;{union Cyc_YYSTYPE _tmp2F2=yy1;struct _tuple8*_tmp2F3;_LL1B7: if((
_tmp2F2.YY37).tag != 43)goto _LL1B9;_tmp2F3=(struct _tuple8*)(_tmp2F2.YY37).val;
_LL1B8: yyzzz=_tmp2F3;goto _LL1B6;_LL1B9:;_LL1BA:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL1B6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE
_tmp9E1;return((_tmp9E1.YY37).val=yy1,(((_tmp9E1.YY37).tag=43,_tmp9E1)));}static
char _tmp2F6[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2F6,_tmp2F6,
_tmp2F6 + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2F7=yy1;struct Cyc_List_List*_tmp2F8;_LL1BC: if((
_tmp2F7.YY38).tag != 44)goto _LL1BE;_tmp2F8=(struct Cyc_List_List*)(_tmp2F7.YY38).val;
_LL1BD: yyzzz=_tmp2F8;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1BB:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9E2;return((_tmp9E2.YY38).val=yy1,(((_tmp9E2.YY38).tag=44,_tmp9E2)));}static
char _tmp2FB[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2FB,
_tmp2FB,_tmp2FB + 127}};struct _tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct
_tuple23*yyzzz;{union Cyc_YYSTYPE _tmp2FC=yy1;struct _tuple23*_tmp2FD;_LL1C1: if((
_tmp2FC.YY39).tag != 45)goto _LL1C3;_tmp2FD=(struct _tuple23*)(_tmp2FC.YY39).val;
_LL1C2: yyzzz=_tmp2FD;goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY39);
_LL1C0:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY39(struct _tuple23*yy1){union Cyc_YYSTYPE
_tmp9E3;return((_tmp9E3.YY39).val=yy1,(((_tmp9E3.YY39).tag=45,_tmp9E3)));}static
char _tmp300[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={
Cyc_Core_Failure,{_tmp300,_tmp300,_tmp300 + 18}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp301=yy1;
struct Cyc_List_List*_tmp302;_LL1C6: if((_tmp301.YY40).tag != 46)goto _LL1C8;_tmp302=(
struct Cyc_List_List*)(_tmp301.YY40).val;_LL1C7: yyzzz=_tmp302;goto _LL1C5;_LL1C8:;
_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY40);_LL1C5:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E4;return((_tmp9E4.YY40).val=
yy1,(((_tmp9E4.YY40).tag=46,_tmp9E4)));}static char _tmp305[24]="list_t<designator_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp305,
_tmp305,_tmp305 + 24}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp306=yy1;struct Cyc_List_List*
_tmp307;_LL1CB: if((_tmp306.YY41).tag != 47)goto _LL1CD;_tmp307=(struct Cyc_List_List*)(
_tmp306.YY41).val;_LL1CC: yyzzz=_tmp307;goto _LL1CA;_LL1CD:;_LL1CE:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1CA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9E5;return((_tmp9E5.YY41).val=yy1,(((_tmp9E5.YY41).tag=
47,_tmp9E5)));}static char _tmp30A[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp30A,_tmp30A,_tmp30A + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp30B=yy1;void*_tmp30C;_LL1D0:
if((_tmp30B.YY42).tag != 48)goto _LL1D2;_tmp30C=(void*)(_tmp30B.YY42).val;_LL1D1:
yyzzz=_tmp30C;goto _LL1CF;_LL1D2:;_LL1D3:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1CF:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp9E6;
return((_tmp9E6.YY42).val=yy1,(((_tmp9E6.YY42).tag=48,_tmp9E6)));}static char
_tmp30F[7]="kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{
_tmp30F,_tmp30F,_tmp30F + 7}};struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Kind*yyzzz;{union Cyc_YYSTYPE _tmp310=yy1;struct Cyc_Absyn_Kind*
_tmp311;_LL1D5: if((_tmp310.YY43).tag != 49)goto _LL1D7;_tmp311=(struct Cyc_Absyn_Kind*)(
_tmp310.YY43).val;_LL1D6: yyzzz=_tmp311;goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((
void*)& Cyc_yyfail_YY43);_LL1D4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*
yy1){union Cyc_YYSTYPE _tmp9E7;return((_tmp9E7.YY43).val=yy1,(((_tmp9E7.YY43).tag=
49,_tmp9E7)));}static char _tmp314[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp314,_tmp314,_tmp314 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp315=yy1;void*_tmp316;_LL1DA:
if((_tmp315.YY44).tag != 50)goto _LL1DC;_tmp316=(void*)(_tmp315.YY44).val;_LL1DB:
yyzzz=_tmp316;goto _LL1D9;_LL1DC:;_LL1DD:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1D9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmp9E8;
return((_tmp9E8.YY44).val=yy1,(((_tmp9E8.YY44).tag=50,_tmp9E8)));}static char
_tmp319[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY45={
Cyc_Core_Failure,{_tmp319,_tmp319,_tmp319 + 23}};struct Cyc_List_List*Cyc_yyget_YY45(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp31A=yy1;
struct Cyc_List_List*_tmp31B;_LL1DF: if((_tmp31A.YY45).tag != 51)goto _LL1E1;_tmp31B=(
struct Cyc_List_List*)(_tmp31A.YY45).val;_LL1E0: yyzzz=_tmp31B;goto _LL1DE;_LL1E1:;
_LL1E2:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1DE:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E9;return((_tmp9E9.YY45).val=
yy1,(((_tmp9E9.YY45).tag=51,_tmp9E9)));}static char _tmp31E[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp31E,
_tmp31E,_tmp31E + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union
Cyc_YYSTYPE _tmp31F=yy1;void*_tmp320;_LL1E4: if((_tmp31F.YY46).tag != 52)goto _LL1E6;
_tmp320=(void*)(_tmp31F.YY46).val;_LL1E5: yyzzz=_tmp320;goto _LL1E3;_LL1E6:;_LL1E7:(
int)_throw((void*)& Cyc_yyfail_YY46);_LL1E3:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1){union Cyc_YYSTYPE _tmp9EA;return((_tmp9EA.YY46).val=yy1,(((_tmp9EA.YY46).tag=
52,_tmp9EA)));}static char _tmp323[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp323,_tmp323,_tmp323 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE
_tmp324=yy1;struct Cyc_Absyn_Enumfield*_tmp325;_LL1E9: if((_tmp324.YY47).tag != 53)
goto _LL1EB;_tmp325=(struct Cyc_Absyn_Enumfield*)(_tmp324.YY47).val;_LL1EA: yyzzz=
_tmp325;goto _LL1E8;_LL1EB:;_LL1EC:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1E8:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE
_tmp9EB;return((_tmp9EB.YY47).val=yy1,(((_tmp9EB.YY47).tag=53,_tmp9EB)));}static
char _tmp328[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={
Cyc_Core_Failure,{_tmp328,_tmp328,_tmp328 + 23}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp329=yy1;
struct Cyc_List_List*_tmp32A;_LL1EE: if((_tmp329.YY48).tag != 54)goto _LL1F0;_tmp32A=(
struct Cyc_List_List*)(_tmp329.YY48).val;_LL1EF: yyzzz=_tmp32A;goto _LL1ED;_LL1F0:;
_LL1F1:(int)_throw((void*)& Cyc_yyfail_YY48);_LL1ED:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EC;return((_tmp9EC.YY48).val=
yy1,(((_tmp9EC.YY48).tag=54,_tmp9EC)));}static char _tmp32D[17]="opt_t<type_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp32D,
_tmp32D,_tmp32D + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp32E=yy1;struct Cyc_Core_Opt*_tmp32F;
_LL1F3: if((_tmp32E.YY49).tag != 55)goto _LL1F5;_tmp32F=(struct Cyc_Core_Opt*)(
_tmp32E.YY49).val;_LL1F4: yyzzz=_tmp32F;goto _LL1F2;_LL1F5:;_LL1F6:(int)_throw((
void*)& Cyc_yyfail_YY49);_LL1F2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*
yy1){union Cyc_YYSTYPE _tmp9ED;return((_tmp9ED.YY49).val=yy1,(((_tmp9ED.YY49).tag=
55,_tmp9ED)));}static char _tmp332[31]="list_t<$(type_t,type_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp332,_tmp332,
_tmp332 + 31}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp333=yy1;struct Cyc_List_List*_tmp334;_LL1F8: if((
_tmp333.YY50).tag != 56)goto _LL1FA;_tmp334=(struct Cyc_List_List*)(_tmp333.YY50).val;
_LL1F9: yyzzz=_tmp334;goto _LL1F7;_LL1FA:;_LL1FB:(int)_throw((void*)& Cyc_yyfail_YY50);
_LL1F7:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE
_tmp9EE;return((_tmp9EE.YY50).val=yy1,(((_tmp9EE.YY50).tag=56,_tmp9EE)));}static
char _tmp337[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY51={
Cyc_Core_Failure,{_tmp337,_tmp337,_tmp337 + 15}};union Cyc_Absyn_Constraint*Cyc_yyget_YY51(
union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp338=
yy1;union Cyc_Absyn_Constraint*_tmp339;_LL1FD: if((_tmp338.YY51).tag != 57)goto
_LL1FF;_tmp339=(union Cyc_Absyn_Constraint*)(_tmp338.YY51).val;_LL1FE: yyzzz=
_tmp339;goto _LL1FC;_LL1FF:;_LL200:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1FC:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE
_tmp9EF;return((_tmp9EF.YY51).val=yy1,(((_tmp9EF.YY51).tag=57,_tmp9EF)));}static
char _tmp33C[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp33C,_tmp33C,_tmp33C + 45}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp33D=yy1;struct Cyc_List_List*_tmp33E;_LL202: if((_tmp33D.YY52).tag != 58)goto
_LL204;_tmp33E=(struct Cyc_List_List*)(_tmp33D.YY52).val;_LL203: yyzzz=_tmp33E;
goto _LL201;_LL204:;_LL205:(int)_throw((void*)& Cyc_yyfail_YY52);_LL201:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F0;
return((_tmp9F0.YY52).val=yy1,(((_tmp9F0.YY52).tag=58,_tmp9F0)));}static char
_tmp341[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp341,_tmp341,_tmp341 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp342=yy1;void*_tmp343;_LL207: if((_tmp342.YY53).tag
!= 59)goto _LL209;_tmp343=(void*)(_tmp342.YY53).val;_LL208: yyzzz=_tmp343;goto
_LL206;_LL209:;_LL20A:(int)_throw((void*)& Cyc_yyfail_YY53);_LL206:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmp9F1;return((_tmp9F1.YY53).val=
yy1,(((_tmp9F1.YY53).tag=59,_tmp9F1)));}static char _tmp346[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp346,
_tmp346,_tmp346 + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp347=yy1;struct Cyc_List_List*
_tmp348;_LL20C: if((_tmp347.YY54).tag != 60)goto _LL20E;_tmp348=(struct Cyc_List_List*)(
_tmp347.YY54).val;_LL20D: yyzzz=_tmp348;goto _LL20B;_LL20E:;_LL20F:(int)_throw((
void*)& Cyc_yyfail_YY54);_LL20B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*
yy1){union Cyc_YYSTYPE _tmp9F2;return((_tmp9F2.YY54).val=yy1,(((_tmp9F2.YY54).tag=
60,_tmp9F2)));}static char _tmp34B[21]="$(bool,string_t<`H>)";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp34B,_tmp34B,_tmp34B + 21}};struct
_tuple17 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){struct _tuple17 yyzzz;{union Cyc_YYSTYPE
_tmp34C=yy1;struct _tuple17 _tmp34D;_LL211: if((_tmp34C.Asm_tok).tag != 6)goto _LL213;
_tmp34D=(struct _tuple17)(_tmp34C.Asm_tok).val;_LL212: yyzzz=_tmp34D;goto _LL210;
_LL213:;_LL214:(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL210:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple17 yy1){union Cyc_YYSTYPE _tmp9F3;return((
_tmp9F3.Asm_tok).val=yy1,(((_tmp9F3.Asm_tok).tag=6,_tmp9F3)));}struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_Yyltype Cyc_yynewloc(){struct Cyc_Yyltype _tmp9F4;return(_tmp9F4.timestamp=0,((
_tmp9F4.first_line=0,((_tmp9F4.first_column=0,((_tmp9F4.last_line=0,((_tmp9F4.last_column=
0,_tmp9F4)))))))));}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[
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
104,105,106,107,108,109,110,111,112,113,114,115,116};static char _tmp350[2]="$";
static char _tmp351[6]="error";static char _tmp352[12]="$undefined.";static char
_tmp353[5]="AUTO";static char _tmp354[9]="REGISTER";static char _tmp355[7]="STATIC";
static char _tmp356[7]="EXTERN";static char _tmp357[8]="TYPEDEF";static char _tmp358[5]="VOID";
static char _tmp359[5]="CHAR";static char _tmp35A[6]="SHORT";static char _tmp35B[4]="INT";
static char _tmp35C[5]="LONG";static char _tmp35D[6]="FLOAT";static char _tmp35E[7]="DOUBLE";
static char _tmp35F[7]="SIGNED";static char _tmp360[9]="UNSIGNED";static char _tmp361[
6]="CONST";static char _tmp362[9]="VOLATILE";static char _tmp363[9]="RESTRICT";
static char _tmp364[7]="STRUCT";static char _tmp365[6]="UNION";static char _tmp366[5]="CASE";
static char _tmp367[8]="DEFAULT";static char _tmp368[7]="INLINE";static char _tmp369[7]="SIZEOF";
static char _tmp36A[9]="OFFSETOF";static char _tmp36B[3]="IF";static char _tmp36C[5]="ELSE";
static char _tmp36D[7]="SWITCH";static char _tmp36E[6]="WHILE";static char _tmp36F[3]="DO";
static char _tmp370[4]="FOR";static char _tmp371[5]="GOTO";static char _tmp372[9]="CONTINUE";
static char _tmp373[6]="BREAK";static char _tmp374[7]="RETURN";static char _tmp375[5]="ENUM";
static char _tmp376[8]="NULL_kw";static char _tmp377[4]="LET";static char _tmp378[6]="THROW";
static char _tmp379[4]="TRY";static char _tmp37A[6]="CATCH";static char _tmp37B[7]="EXPORT";
static char _tmp37C[4]="NEW";static char _tmp37D[9]="ABSTRACT";static char _tmp37E[9]="FALLTHRU";
static char _tmp37F[6]="USING";static char _tmp380[10]="NAMESPACE";static char _tmp381[
9]="DATATYPE";static char _tmp382[8]="XTUNION";static char _tmp383[7]="TUNION";
static char _tmp384[7]="MALLOC";static char _tmp385[8]="RMALLOC";static char _tmp386[7]="CALLOC";
static char _tmp387[8]="RCALLOC";static char _tmp388[5]="SWAP";static char _tmp389[9]="REGION_T";
static char _tmp38A[6]="TAG_T";static char _tmp38B[7]="REGION";static char _tmp38C[5]="RNEW";
static char _tmp38D[8]="REGIONS";static char _tmp38E[13]="RESET_REGION";static char
_tmp38F[16]="NOZEROTERM_QUAL";static char _tmp390[14]="ZEROTERM_QUAL";static char
_tmp391[12]="REGION_QUAL";static char _tmp392[7]="PORTON";static char _tmp393[8]="PORTOFF";
static char _tmp394[12]="DYNREGION_T";static char _tmp395[6]="ALIAS";static char
_tmp396[8]="NUMELTS";static char _tmp397[8]="VALUEOF";static char _tmp398[10]="VALUEOF_T";
static char _tmp399[9]="TAGCHECK";static char _tmp39A[13]="NUMELTS_QUAL";static char
_tmp39B[10]="THIN_QUAL";static char _tmp39C[9]="FAT_QUAL";static char _tmp39D[13]="NOTNULL_QUAL";
static char _tmp39E[14]="NULLABLE_QUAL";static char _tmp39F[12]="TAGGED_QUAL";static
char _tmp3A0[16]="EXTENSIBLE_QUAL";static char _tmp3A1[15]="RESETABLE_QUAL";static
char _tmp3A2[7]="PTR_OP";static char _tmp3A3[7]="INC_OP";static char _tmp3A4[7]="DEC_OP";
static char _tmp3A5[8]="LEFT_OP";static char _tmp3A6[9]="RIGHT_OP";static char _tmp3A7[
6]="LE_OP";static char _tmp3A8[6]="GE_OP";static char _tmp3A9[6]="EQ_OP";static char
_tmp3AA[6]="NE_OP";static char _tmp3AB[7]="AND_OP";static char _tmp3AC[6]="OR_OP";
static char _tmp3AD[11]="MUL_ASSIGN";static char _tmp3AE[11]="DIV_ASSIGN";static char
_tmp3AF[11]="MOD_ASSIGN";static char _tmp3B0[11]="ADD_ASSIGN";static char _tmp3B1[11]="SUB_ASSIGN";
static char _tmp3B2[12]="LEFT_ASSIGN";static char _tmp3B3[13]="RIGHT_ASSIGN";static
char _tmp3B4[11]="AND_ASSIGN";static char _tmp3B5[11]="XOR_ASSIGN";static char
_tmp3B6[10]="OR_ASSIGN";static char _tmp3B7[9]="ELLIPSIS";static char _tmp3B8[11]="LEFT_RIGHT";
static char _tmp3B9[12]="COLON_COLON";static char _tmp3BA[11]="IDENTIFIER";static
char _tmp3BB[17]="INTEGER_CONSTANT";static char _tmp3BC[7]="STRING";static char
_tmp3BD[19]="CHARACTER_CONSTANT";static char _tmp3BE[18]="FLOATING_CONSTANT";
static char _tmp3BF[9]="TYPE_VAR";static char _tmp3C0[13]="TYPEDEF_NAME";static char
_tmp3C1[16]="QUAL_IDENTIFIER";static char _tmp3C2[18]="QUAL_TYPEDEF_NAME";static
char _tmp3C3[10]="ATTRIBUTE";static char _tmp3C4[4]="ASM";static char _tmp3C5[4]="';'";
static char _tmp3C6[4]="'{'";static char _tmp3C7[4]="'}'";static char _tmp3C8[4]="','";
static char _tmp3C9[4]="'='";static char _tmp3CA[4]="'<'";static char _tmp3CB[4]="'>'";
static char _tmp3CC[4]="'('";static char _tmp3CD[4]="')'";static char _tmp3CE[4]="'_'";
static char _tmp3CF[4]="'$'";static char _tmp3D0[4]="':'";static char _tmp3D1[4]="'.'";
static char _tmp3D2[4]="'['";static char _tmp3D3[4]="']'";static char _tmp3D4[4]="'*'";
static char _tmp3D5[4]="'@'";static char _tmp3D6[4]="'?'";static char _tmp3D7[4]="'+'";
static char _tmp3D8[4]="'|'";static char _tmp3D9[4]="'^'";static char _tmp3DA[4]="'&'";
static char _tmp3DB[4]="'-'";static char _tmp3DC[4]="'/'";static char _tmp3DD[4]="'%'";
static char _tmp3DE[4]="'~'";static char _tmp3DF[4]="'!'";static char _tmp3E0[5]="prog";
static char _tmp3E1[17]="translation_unit";static char _tmp3E2[12]="export_list";
static char _tmp3E3[19]="export_list_values";static char _tmp3E4[21]="external_declaration";
static char _tmp3E5[20]="function_definition";static char _tmp3E6[21]="function_definition2";
static char _tmp3E7[13]="using_action";static char _tmp3E8[15]="unusing_action";
static char _tmp3E9[17]="namespace_action";static char _tmp3EA[19]="unnamespace_action";
static char _tmp3EB[12]="declaration";static char _tmp3EC[19]="resetable_qual_opt";
static char _tmp3ED[17]="declaration_list";static char _tmp3EE[23]="declaration_specifiers";
static char _tmp3EF[24]="storage_class_specifier";static char _tmp3F0[15]="attributes_opt";
static char _tmp3F1[11]="attributes";static char _tmp3F2[15]="attribute_list";static
char _tmp3F3[10]="attribute";static char _tmp3F4[15]="type_specifier";static char
_tmp3F5[25]="type_specifier_notypedef";static char _tmp3F6[5]="kind";static char
_tmp3F7[15]="type_qualifier";static char _tmp3F8[15]="enum_specifier";static char
_tmp3F9[11]="enum_field";static char _tmp3FA[22]="enum_declaration_list";static
char _tmp3FB[26]="struct_or_union_specifier";static char _tmp3FC[16]="type_params_opt";
static char _tmp3FD[16]="struct_or_union";static char _tmp3FE[24]="struct_declaration_list";
static char _tmp3FF[25]="struct_declaration_list0";static char _tmp400[21]="init_declarator_list";
static char _tmp401[22]="init_declarator_list0";static char _tmp402[16]="init_declarator";
static char _tmp403[19]="struct_declaration";static char _tmp404[25]="specifier_qualifier_list";
static char _tmp405[35]="notypedef_specifier_qualifier_list";static char _tmp406[23]="struct_declarator_list";
static char _tmp407[24]="struct_declarator_list0";static char _tmp408[18]="struct_declarator";
static char _tmp409[19]="datatype_specifier";static char _tmp40A[14]="qual_datatype";
static char _tmp40B[19]="datatypefield_list";static char _tmp40C[20]="datatypefield_scope";
static char _tmp40D[14]="datatypefield";static char _tmp40E[11]="declarator";static
char _tmp40F[23]="declarator_withtypedef";static char _tmp410[18]="direct_declarator";
static char _tmp411[30]="direct_declarator_withtypedef";static char _tmp412[8]="pointer";
static char _tmp413[12]="one_pointer";static char _tmp414[14]="pointer_quals";static
char _tmp415[13]="pointer_qual";static char _tmp416[23]="pointer_null_and_bound";
static char _tmp417[14]="pointer_bound";static char _tmp418[18]="zeroterm_qual_opt";
static char _tmp419[8]="rgn_opt";static char _tmp41A[11]="tqual_list";static char
_tmp41B[20]="parameter_type_list";static char _tmp41C[9]="type_var";static char
_tmp41D[16]="optional_effect";static char _tmp41E[19]="optional_rgn_order";static
char _tmp41F[10]="rgn_order";static char _tmp420[16]="optional_inject";static char
_tmp421[11]="effect_set";static char _tmp422[14]="atomic_effect";static char _tmp423[
11]="region_set";static char _tmp424[15]="parameter_list";static char _tmp425[22]="parameter_declaration";
static char _tmp426[16]="identifier_list";static char _tmp427[17]="identifier_list0";
static char _tmp428[12]="initializer";static char _tmp429[18]="array_initializer";
static char _tmp42A[17]="initializer_list";static char _tmp42B[12]="designation";
static char _tmp42C[16]="designator_list";static char _tmp42D[11]="designator";
static char _tmp42E[10]="type_name";static char _tmp42F[14]="any_type_name";static
char _tmp430[15]="type_name_list";static char _tmp431[20]="abstract_declarator";
static char _tmp432[27]="direct_abstract_declarator";static char _tmp433[10]="statement";
static char _tmp434[13]="open_exp_opt";static char _tmp435[18]="labeled_statement";
static char _tmp436[21]="expression_statement";static char _tmp437[19]="compound_statement";
static char _tmp438[16]="block_item_list";static char _tmp439[20]="selection_statement";
static char _tmp43A[15]="switch_clauses";static char _tmp43B[20]="iteration_statement";
static char _tmp43C[15]="jump_statement";static char _tmp43D[12]="exp_pattern";
static char _tmp43E[20]="conditional_pattern";static char _tmp43F[19]="logical_or_pattern";
static char _tmp440[20]="logical_and_pattern";static char _tmp441[21]="inclusive_or_pattern";
static char _tmp442[21]="exclusive_or_pattern";static char _tmp443[12]="and_pattern";
static char _tmp444[17]="equality_pattern";static char _tmp445[19]="relational_pattern";
static char _tmp446[14]="shift_pattern";static char _tmp447[17]="additive_pattern";
static char _tmp448[23]="multiplicative_pattern";static char _tmp449[13]="cast_pattern";
static char _tmp44A[14]="unary_pattern";static char _tmp44B[16]="postfix_pattern";
static char _tmp44C[16]="primary_pattern";static char _tmp44D[8]="pattern";static
char _tmp44E[19]="tuple_pattern_list";static char _tmp44F[20]="tuple_pattern_list0";
static char _tmp450[14]="field_pattern";static char _tmp451[19]="field_pattern_list";
static char _tmp452[20]="field_pattern_list0";static char _tmp453[11]="expression";
static char _tmp454[22]="assignment_expression";static char _tmp455[20]="assignment_operator";
static char _tmp456[23]="conditional_expression";static char _tmp457[20]="constant_expression";
static char _tmp458[22]="logical_or_expression";static char _tmp459[23]="logical_and_expression";
static char _tmp45A[24]="inclusive_or_expression";static char _tmp45B[24]="exclusive_or_expression";
static char _tmp45C[15]="and_expression";static char _tmp45D[20]="equality_expression";
static char _tmp45E[22]="relational_expression";static char _tmp45F[17]="shift_expression";
static char _tmp460[20]="additive_expression";static char _tmp461[26]="multiplicative_expression";
static char _tmp462[16]="cast_expression";static char _tmp463[17]="unary_expression";
static char _tmp464[15]="unary_operator";static char _tmp465[19]="postfix_expression";
static char _tmp466[19]="primary_expression";static char _tmp467[25]="argument_expression_list";
static char _tmp468[26]="argument_expression_list0";static char _tmp469[9]="constant";
static char _tmp46A[20]="qual_opt_identifier";static char _tmp46B[17]="qual_opt_typedef";
static char _tmp46C[18]="struct_union_name";static char _tmp46D[11]="field_name";
static char _tmp46E[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[287]={{
_tmp350,_tmp350,_tmp350 + 2},{_tmp351,_tmp351,_tmp351 + 6},{_tmp352,_tmp352,
_tmp352 + 12},{_tmp353,_tmp353,_tmp353 + 5},{_tmp354,_tmp354,_tmp354 + 9},{_tmp355,
_tmp355,_tmp355 + 7},{_tmp356,_tmp356,_tmp356 + 7},{_tmp357,_tmp357,_tmp357 + 8},{
_tmp358,_tmp358,_tmp358 + 5},{_tmp359,_tmp359,_tmp359 + 5},{_tmp35A,_tmp35A,
_tmp35A + 6},{_tmp35B,_tmp35B,_tmp35B + 4},{_tmp35C,_tmp35C,_tmp35C + 5},{_tmp35D,
_tmp35D,_tmp35D + 6},{_tmp35E,_tmp35E,_tmp35E + 7},{_tmp35F,_tmp35F,_tmp35F + 7},{
_tmp360,_tmp360,_tmp360 + 9},{_tmp361,_tmp361,_tmp361 + 6},{_tmp362,_tmp362,
_tmp362 + 9},{_tmp363,_tmp363,_tmp363 + 9},{_tmp364,_tmp364,_tmp364 + 7},{_tmp365,
_tmp365,_tmp365 + 6},{_tmp366,_tmp366,_tmp366 + 5},{_tmp367,_tmp367,_tmp367 + 8},{
_tmp368,_tmp368,_tmp368 + 7},{_tmp369,_tmp369,_tmp369 + 7},{_tmp36A,_tmp36A,
_tmp36A + 9},{_tmp36B,_tmp36B,_tmp36B + 3},{_tmp36C,_tmp36C,_tmp36C + 5},{_tmp36D,
_tmp36D,_tmp36D + 7},{_tmp36E,_tmp36E,_tmp36E + 6},{_tmp36F,_tmp36F,_tmp36F + 3},{
_tmp370,_tmp370,_tmp370 + 4},{_tmp371,_tmp371,_tmp371 + 5},{_tmp372,_tmp372,
_tmp372 + 9},{_tmp373,_tmp373,_tmp373 + 6},{_tmp374,_tmp374,_tmp374 + 7},{_tmp375,
_tmp375,_tmp375 + 5},{_tmp376,_tmp376,_tmp376 + 8},{_tmp377,_tmp377,_tmp377 + 4},{
_tmp378,_tmp378,_tmp378 + 6},{_tmp379,_tmp379,_tmp379 + 4},{_tmp37A,_tmp37A,
_tmp37A + 6},{_tmp37B,_tmp37B,_tmp37B + 7},{_tmp37C,_tmp37C,_tmp37C + 4},{_tmp37D,
_tmp37D,_tmp37D + 9},{_tmp37E,_tmp37E,_tmp37E + 9},{_tmp37F,_tmp37F,_tmp37F + 6},{
_tmp380,_tmp380,_tmp380 + 10},{_tmp381,_tmp381,_tmp381 + 9},{_tmp382,_tmp382,
_tmp382 + 8},{_tmp383,_tmp383,_tmp383 + 7},{_tmp384,_tmp384,_tmp384 + 7},{_tmp385,
_tmp385,_tmp385 + 8},{_tmp386,_tmp386,_tmp386 + 7},{_tmp387,_tmp387,_tmp387 + 8},{
_tmp388,_tmp388,_tmp388 + 5},{_tmp389,_tmp389,_tmp389 + 9},{_tmp38A,_tmp38A,
_tmp38A + 6},{_tmp38B,_tmp38B,_tmp38B + 7},{_tmp38C,_tmp38C,_tmp38C + 5},{_tmp38D,
_tmp38D,_tmp38D + 8},{_tmp38E,_tmp38E,_tmp38E + 13},{_tmp38F,_tmp38F,_tmp38F + 16},{
_tmp390,_tmp390,_tmp390 + 14},{_tmp391,_tmp391,_tmp391 + 12},{_tmp392,_tmp392,
_tmp392 + 7},{_tmp393,_tmp393,_tmp393 + 8},{_tmp394,_tmp394,_tmp394 + 12},{_tmp395,
_tmp395,_tmp395 + 6},{_tmp396,_tmp396,_tmp396 + 8},{_tmp397,_tmp397,_tmp397 + 8},{
_tmp398,_tmp398,_tmp398 + 10},{_tmp399,_tmp399,_tmp399 + 9},{_tmp39A,_tmp39A,
_tmp39A + 13},{_tmp39B,_tmp39B,_tmp39B + 10},{_tmp39C,_tmp39C,_tmp39C + 9},{_tmp39D,
_tmp39D,_tmp39D + 13},{_tmp39E,_tmp39E,_tmp39E + 14},{_tmp39F,_tmp39F,_tmp39F + 12},{
_tmp3A0,_tmp3A0,_tmp3A0 + 16},{_tmp3A1,_tmp3A1,_tmp3A1 + 15},{_tmp3A2,_tmp3A2,
_tmp3A2 + 7},{_tmp3A3,_tmp3A3,_tmp3A3 + 7},{_tmp3A4,_tmp3A4,_tmp3A4 + 7},{_tmp3A5,
_tmp3A5,_tmp3A5 + 8},{_tmp3A6,_tmp3A6,_tmp3A6 + 9},{_tmp3A7,_tmp3A7,_tmp3A7 + 6},{
_tmp3A8,_tmp3A8,_tmp3A8 + 6},{_tmp3A9,_tmp3A9,_tmp3A9 + 6},{_tmp3AA,_tmp3AA,
_tmp3AA + 6},{_tmp3AB,_tmp3AB,_tmp3AB + 7},{_tmp3AC,_tmp3AC,_tmp3AC + 6},{_tmp3AD,
_tmp3AD,_tmp3AD + 11},{_tmp3AE,_tmp3AE,_tmp3AE + 11},{_tmp3AF,_tmp3AF,_tmp3AF + 11},{
_tmp3B0,_tmp3B0,_tmp3B0 + 11},{_tmp3B1,_tmp3B1,_tmp3B1 + 11},{_tmp3B2,_tmp3B2,
_tmp3B2 + 12},{_tmp3B3,_tmp3B3,_tmp3B3 + 13},{_tmp3B4,_tmp3B4,_tmp3B4 + 11},{
_tmp3B5,_tmp3B5,_tmp3B5 + 11},{_tmp3B6,_tmp3B6,_tmp3B6 + 10},{_tmp3B7,_tmp3B7,
_tmp3B7 + 9},{_tmp3B8,_tmp3B8,_tmp3B8 + 11},{_tmp3B9,_tmp3B9,_tmp3B9 + 12},{_tmp3BA,
_tmp3BA,_tmp3BA + 11},{_tmp3BB,_tmp3BB,_tmp3BB + 17},{_tmp3BC,_tmp3BC,_tmp3BC + 7},{
_tmp3BD,_tmp3BD,_tmp3BD + 19},{_tmp3BE,_tmp3BE,_tmp3BE + 18},{_tmp3BF,_tmp3BF,
_tmp3BF + 9},{_tmp3C0,_tmp3C0,_tmp3C0 + 13},{_tmp3C1,_tmp3C1,_tmp3C1 + 16},{_tmp3C2,
_tmp3C2,_tmp3C2 + 18},{_tmp3C3,_tmp3C3,_tmp3C3 + 10},{_tmp3C4,_tmp3C4,_tmp3C4 + 4},{
_tmp3C5,_tmp3C5,_tmp3C5 + 4},{_tmp3C6,_tmp3C6,_tmp3C6 + 4},{_tmp3C7,_tmp3C7,
_tmp3C7 + 4},{_tmp3C8,_tmp3C8,_tmp3C8 + 4},{_tmp3C9,_tmp3C9,_tmp3C9 + 4},{_tmp3CA,
_tmp3CA,_tmp3CA + 4},{_tmp3CB,_tmp3CB,_tmp3CB + 4},{_tmp3CC,_tmp3CC,_tmp3CC + 4},{
_tmp3CD,_tmp3CD,_tmp3CD + 4},{_tmp3CE,_tmp3CE,_tmp3CE + 4},{_tmp3CF,_tmp3CF,
_tmp3CF + 4},{_tmp3D0,_tmp3D0,_tmp3D0 + 4},{_tmp3D1,_tmp3D1,_tmp3D1 + 4},{_tmp3D2,
_tmp3D2,_tmp3D2 + 4},{_tmp3D3,_tmp3D3,_tmp3D3 + 4},{_tmp3D4,_tmp3D4,_tmp3D4 + 4},{
_tmp3D5,_tmp3D5,_tmp3D5 + 4},{_tmp3D6,_tmp3D6,_tmp3D6 + 4},{_tmp3D7,_tmp3D7,
_tmp3D7 + 4},{_tmp3D8,_tmp3D8,_tmp3D8 + 4},{_tmp3D9,_tmp3D9,_tmp3D9 + 4},{_tmp3DA,
_tmp3DA,_tmp3DA + 4},{_tmp3DB,_tmp3DB,_tmp3DB + 4},{_tmp3DC,_tmp3DC,_tmp3DC + 4},{
_tmp3DD,_tmp3DD,_tmp3DD + 4},{_tmp3DE,_tmp3DE,_tmp3DE + 4},{_tmp3DF,_tmp3DF,
_tmp3DF + 4},{_tmp3E0,_tmp3E0,_tmp3E0 + 5},{_tmp3E1,_tmp3E1,_tmp3E1 + 17},{_tmp3E2,
_tmp3E2,_tmp3E2 + 12},{_tmp3E3,_tmp3E3,_tmp3E3 + 19},{_tmp3E4,_tmp3E4,_tmp3E4 + 21},{
_tmp3E5,_tmp3E5,_tmp3E5 + 20},{_tmp3E6,_tmp3E6,_tmp3E6 + 21},{_tmp3E7,_tmp3E7,
_tmp3E7 + 13},{_tmp3E8,_tmp3E8,_tmp3E8 + 15},{_tmp3E9,_tmp3E9,_tmp3E9 + 17},{
_tmp3EA,_tmp3EA,_tmp3EA + 19},{_tmp3EB,_tmp3EB,_tmp3EB + 12},{_tmp3EC,_tmp3EC,
_tmp3EC + 19},{_tmp3ED,_tmp3ED,_tmp3ED + 17},{_tmp3EE,_tmp3EE,_tmp3EE + 23},{
_tmp3EF,_tmp3EF,_tmp3EF + 24},{_tmp3F0,_tmp3F0,_tmp3F0 + 15},{_tmp3F1,_tmp3F1,
_tmp3F1 + 11},{_tmp3F2,_tmp3F2,_tmp3F2 + 15},{_tmp3F3,_tmp3F3,_tmp3F3 + 10},{
_tmp3F4,_tmp3F4,_tmp3F4 + 15},{_tmp3F5,_tmp3F5,_tmp3F5 + 25},{_tmp3F6,_tmp3F6,
_tmp3F6 + 5},{_tmp3F7,_tmp3F7,_tmp3F7 + 15},{_tmp3F8,_tmp3F8,_tmp3F8 + 15},{_tmp3F9,
_tmp3F9,_tmp3F9 + 11},{_tmp3FA,_tmp3FA,_tmp3FA + 22},{_tmp3FB,_tmp3FB,_tmp3FB + 26},{
_tmp3FC,_tmp3FC,_tmp3FC + 16},{_tmp3FD,_tmp3FD,_tmp3FD + 16},{_tmp3FE,_tmp3FE,
_tmp3FE + 24},{_tmp3FF,_tmp3FF,_tmp3FF + 25},{_tmp400,_tmp400,_tmp400 + 21},{
_tmp401,_tmp401,_tmp401 + 22},{_tmp402,_tmp402,_tmp402 + 16},{_tmp403,_tmp403,
_tmp403 + 19},{_tmp404,_tmp404,_tmp404 + 25},{_tmp405,_tmp405,_tmp405 + 35},{
_tmp406,_tmp406,_tmp406 + 23},{_tmp407,_tmp407,_tmp407 + 24},{_tmp408,_tmp408,
_tmp408 + 18},{_tmp409,_tmp409,_tmp409 + 19},{_tmp40A,_tmp40A,_tmp40A + 14},{
_tmp40B,_tmp40B,_tmp40B + 19},{_tmp40C,_tmp40C,_tmp40C + 20},{_tmp40D,_tmp40D,
_tmp40D + 14},{_tmp40E,_tmp40E,_tmp40E + 11},{_tmp40F,_tmp40F,_tmp40F + 23},{
_tmp410,_tmp410,_tmp410 + 18},{_tmp411,_tmp411,_tmp411 + 30},{_tmp412,_tmp412,
_tmp412 + 8},{_tmp413,_tmp413,_tmp413 + 12},{_tmp414,_tmp414,_tmp414 + 14},{_tmp415,
_tmp415,_tmp415 + 13},{_tmp416,_tmp416,_tmp416 + 23},{_tmp417,_tmp417,_tmp417 + 14},{
_tmp418,_tmp418,_tmp418 + 18},{_tmp419,_tmp419,_tmp419 + 8},{_tmp41A,_tmp41A,
_tmp41A + 11},{_tmp41B,_tmp41B,_tmp41B + 20},{_tmp41C,_tmp41C,_tmp41C + 9},{_tmp41D,
_tmp41D,_tmp41D + 16},{_tmp41E,_tmp41E,_tmp41E + 19},{_tmp41F,_tmp41F,_tmp41F + 10},{
_tmp420,_tmp420,_tmp420 + 16},{_tmp421,_tmp421,_tmp421 + 11},{_tmp422,_tmp422,
_tmp422 + 14},{_tmp423,_tmp423,_tmp423 + 11},{_tmp424,_tmp424,_tmp424 + 15},{
_tmp425,_tmp425,_tmp425 + 22},{_tmp426,_tmp426,_tmp426 + 16},{_tmp427,_tmp427,
_tmp427 + 17},{_tmp428,_tmp428,_tmp428 + 12},{_tmp429,_tmp429,_tmp429 + 18},{
_tmp42A,_tmp42A,_tmp42A + 17},{_tmp42B,_tmp42B,_tmp42B + 12},{_tmp42C,_tmp42C,
_tmp42C + 16},{_tmp42D,_tmp42D,_tmp42D + 11},{_tmp42E,_tmp42E,_tmp42E + 10},{
_tmp42F,_tmp42F,_tmp42F + 14},{_tmp430,_tmp430,_tmp430 + 15},{_tmp431,_tmp431,
_tmp431 + 20},{_tmp432,_tmp432,_tmp432 + 27},{_tmp433,_tmp433,_tmp433 + 10},{
_tmp434,_tmp434,_tmp434 + 13},{_tmp435,_tmp435,_tmp435 + 18},{_tmp436,_tmp436,
_tmp436 + 21},{_tmp437,_tmp437,_tmp437 + 19},{_tmp438,_tmp438,_tmp438 + 16},{
_tmp439,_tmp439,_tmp439 + 20},{_tmp43A,_tmp43A,_tmp43A + 15},{_tmp43B,_tmp43B,
_tmp43B + 20},{_tmp43C,_tmp43C,_tmp43C + 15},{_tmp43D,_tmp43D,_tmp43D + 12},{
_tmp43E,_tmp43E,_tmp43E + 20},{_tmp43F,_tmp43F,_tmp43F + 19},{_tmp440,_tmp440,
_tmp440 + 20},{_tmp441,_tmp441,_tmp441 + 21},{_tmp442,_tmp442,_tmp442 + 21},{
_tmp443,_tmp443,_tmp443 + 12},{_tmp444,_tmp444,_tmp444 + 17},{_tmp445,_tmp445,
_tmp445 + 19},{_tmp446,_tmp446,_tmp446 + 14},{_tmp447,_tmp447,_tmp447 + 17},{
_tmp448,_tmp448,_tmp448 + 23},{_tmp449,_tmp449,_tmp449 + 13},{_tmp44A,_tmp44A,
_tmp44A + 14},{_tmp44B,_tmp44B,_tmp44B + 16},{_tmp44C,_tmp44C,_tmp44C + 16},{
_tmp44D,_tmp44D,_tmp44D + 8},{_tmp44E,_tmp44E,_tmp44E + 19},{_tmp44F,_tmp44F,
_tmp44F + 20},{_tmp450,_tmp450,_tmp450 + 14},{_tmp451,_tmp451,_tmp451 + 19},{
_tmp452,_tmp452,_tmp452 + 20},{_tmp453,_tmp453,_tmp453 + 11},{_tmp454,_tmp454,
_tmp454 + 22},{_tmp455,_tmp455,_tmp455 + 20},{_tmp456,_tmp456,_tmp456 + 23},{
_tmp457,_tmp457,_tmp457 + 20},{_tmp458,_tmp458,_tmp458 + 22},{_tmp459,_tmp459,
_tmp459 + 23},{_tmp45A,_tmp45A,_tmp45A + 24},{_tmp45B,_tmp45B,_tmp45B + 24},{
_tmp45C,_tmp45C,_tmp45C + 15},{_tmp45D,_tmp45D,_tmp45D + 20},{_tmp45E,_tmp45E,
_tmp45E + 22},{_tmp45F,_tmp45F,_tmp45F + 17},{_tmp460,_tmp460,_tmp460 + 20},{
_tmp461,_tmp461,_tmp461 + 26},{_tmp462,_tmp462,_tmp462 + 16},{_tmp463,_tmp463,
_tmp463 + 17},{_tmp464,_tmp464,_tmp464 + 15},{_tmp465,_tmp465,_tmp465 + 19},{
_tmp466,_tmp466,_tmp466 + 19},{_tmp467,_tmp467,_tmp467 + 25},{_tmp468,_tmp468,
_tmp468 + 26},{_tmp469,_tmp469,_tmp469 + 9},{_tmp46A,_tmp46A,_tmp46A + 20},{_tmp46B,
_tmp46B,_tmp46B + 17},{_tmp46C,_tmp46C,_tmp46C + 18},{_tmp46D,_tmp46D,_tmp46D + 11},{
_tmp46E,_tmp46E,_tmp46E + 12}};static short Cyc_yyr1[513]={0,144,145,145,145,145,
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
struct _dyneither_ptr f1;void*f2;};static char _tmp50C[8]="stdcall";static char
_tmp50D[6]="cdecl";static char _tmp50E[9]="fastcall";static char _tmp50F[9]="noreturn";
static char _tmp510[6]="const";static char _tmp511[8]="aligned";static char _tmp512[7]="packed";
static char _tmp513[7]="shared";static char _tmp514[7]="unused";static char _tmp515[5]="weak";
static char _tmp516[10]="dllimport";static char _tmp517[10]="dllexport";static char
_tmp518[23]="no_instrument_function";static char _tmp519[12]="constructor";static
char _tmp51A[11]="destructor";static char _tmp51B[22]="no_check_memory_usage";
static char _tmp51C[5]="pure";struct _tuple26{void*f1;void*f2;};struct _tuple27{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*
yylex_buf);static void _tmp9F9(unsigned int*_tmp9F8,unsigned int*_tmp9F7,union Cyc_YYSTYPE**
_tmp9F6){for(*_tmp9F8=0;*_tmp9F8 < *_tmp9F7;(*_tmp9F8)++){(*_tmp9F6)[*_tmp9F8]=
Cyc_yylval;}}static void _tmp9FE(unsigned int*_tmp9FD,unsigned int*_tmp9FC,struct
Cyc_Yyltype**_tmp9FB){for(*_tmp9FD=0;*_tmp9FD < *_tmp9FC;(*_tmp9FD)++){(*_tmp9FB)[*
_tmp9FD]=Cyc_yynewloc();}}static void _tmpEEE(unsigned int*_tmpEED,unsigned int*
_tmpEEC,char**_tmpEEA){for(*_tmpEED=0;*_tmpEED < *_tmpEEC;(*_tmpEED)++){(*_tmpEEA)[*
_tmpEED]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct
_RegionHandle _tmp470=_new_region("yyregion");struct _RegionHandle*yyregion=&
_tmp470;_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;
int yyssp_offset;short*yyss=(short*)_region_malloc(yyregion,sizeof(short)* 10000);
int yyvsp_offset;unsigned int _tmp9F8;unsigned int _tmp9F7;union Cyc_YYSTYPE*_tmp9F6;
unsigned int _tmp9F5;union Cyc_YYSTYPE*yyvs=(union Cyc_YYSTYPE*)((_tmp9F5=10000,((
_tmp9F6=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),
_tmp9F5)),((((_tmp9F7=_tmp9F5,_tmp9F9(& _tmp9F8,& _tmp9F7,& _tmp9F6))),_tmp9F6))))));
int yylsp_offset;unsigned int _tmp9FD;unsigned int _tmp9FC;struct Cyc_Yyltype*
_tmp9FB;unsigned int _tmp9FA;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)((_tmp9FA=
10000,((_tmp9FB=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(
struct Cyc_Yyltype),_tmp9FA)),((((_tmp9FC=_tmp9FA,_tmp9FE(& _tmp9FD,& _tmp9FC,&
_tmp9FB))),_tmp9FB))))));int yystacksize=10000;union Cyc_YYSTYPE yyval=Cyc_yylval;
int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;
yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(
10000,++ yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(
yystacksize >= 10000){{const char*_tmp9FF;Cyc_yyerror(((_tmp9FF="parser stack overflow",
_tag_dyneither(_tmp9FF,sizeof(char),22))));}(int)_throw((void*)& Cyc_Yystack_overflow_val);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1028,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 371?(int)Cyc_yytranslate[_check_known_subscript_notnull(372,Cyc_yychar)]:
287;}yyn +=yychar1;if((yyn < 0  || yyn > 6244) || Cyc_yycheck[
_check_known_subscript_notnull(6245,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6245,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1027){int
_tmp472=0;_npop_handler(0);return _tmp472;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1028,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(513,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL215: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL216: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp473;_push_handler(& _tmp473);{int _tmp475=0;if(setjmp(_tmp473.handler))_tmp475=
1;if(!_tmp475){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp474=(void*)_exn_thrown;void*
_tmp477=_tmp474;_LL219: {struct Cyc_Core_Failure_struct*_tmp478=(struct Cyc_Core_Failure_struct*)
_tmp477;if(_tmp478->tag != Cyc_Core_Failure)goto _LL21B;}_LL21A: x=0;goto _LL218;
_LL21B:;_LL21C:(void)_throw(_tmp477);_LL218:;}};}{struct _handler_cons _tmp479;
_push_handler(& _tmp479);{int _tmp47B=0;if(setjmp(_tmp479.handler))_tmp47B=1;if(!
_tmp47B){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;
_pop_handler();}else{void*_tmp47A=(void*)_exn_thrown;void*_tmp47D=_tmp47A;_LL21E: {
struct Cyc_Core_Failure_struct*_tmp47E=(struct Cyc_Core_Failure_struct*)_tmp47D;
if(_tmp47E->tag != Cyc_Core_Failure)goto _LL220;}_LL21F: y=0;goto _LL21D;_LL220:;
_LL221:(void)_throw(_tmp47D);_LL21D:;}};}yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));break;}
case 3: _LL217:{struct Cyc_Absyn_Decl*_tmpA09;struct Cyc_Absyn_Using_d_struct _tmpA08;
struct Cyc_Absyn_Using_d_struct*_tmpA07;struct Cyc_List_List*_tmpA06;yyval=Cyc_YY18(((
_tmpA06=_cycalloc(sizeof(*_tmpA06)),((_tmpA06->hd=((_tmpA09=_cycalloc(sizeof(*
_tmpA09)),((_tmpA09->r=(void*)((_tmpA07=_cycalloc(sizeof(*_tmpA07)),((_tmpA07[0]=((
_tmpA08.tag=11,((_tmpA08.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA08.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA08)))))),_tmpA07)))),((
_tmpA09->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA09)))))),((_tmpA06->tl=0,_tmpA06)))))));}Cyc_Lex_leave_using();
break;case 4: _LL222:{struct Cyc_Absyn_Decl*_tmpA13;struct Cyc_Absyn_Using_d_struct
_tmpA12;struct Cyc_Absyn_Using_d_struct*_tmpA11;struct Cyc_List_List*_tmpA10;yyval=
Cyc_YY18(((_tmpA10=_cycalloc(sizeof(*_tmpA10)),((_tmpA10->hd=((_tmpA13=_cycalloc(
sizeof(*_tmpA13)),((_tmpA13->r=(void*)((_tmpA11=_cycalloc(sizeof(*_tmpA11)),((
_tmpA11[0]=((_tmpA12.tag=11,((_tmpA12.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpA12.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA12)))))),
_tmpA11)))),((_tmpA13->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA13)))))),((
_tmpA10->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA10)))))));}break;case 5: _LL223:{struct Cyc_Absyn_Decl*_tmpA22;
struct Cyc_Absyn_Namespace_d_struct _tmpA21;struct _dyneither_ptr*_tmpA20;struct Cyc_Absyn_Namespace_d_struct*
_tmpA1F;struct Cyc_List_List*_tmpA1E;yyval=Cyc_YY18(((_tmpA1E=_cycalloc(sizeof(*
_tmpA1E)),((_tmpA1E->hd=((_tmpA22=_cycalloc(sizeof(*_tmpA22)),((_tmpA22->r=(void*)((
_tmpA1F=_cycalloc(sizeof(*_tmpA1F)),((_tmpA1F[0]=((_tmpA21.tag=10,((_tmpA21.f1=((
_tmpA20=_cycalloc(sizeof(*_tmpA20)),((_tmpA20[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA20)))),((_tmpA21.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA21)))))),
_tmpA1F)))),((_tmpA22->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA22)))))),((
_tmpA1E->tl=0,_tmpA1E)))))));}Cyc_Lex_leave_namespace();break;case 6: _LL224: {
struct _dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp48C;_push_handler(& _tmp48C);{int _tmp48E=0;if(setjmp(_tmp48C.handler))
_tmp48E=1;if(!_tmp48E){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp48D=(void*)_exn_thrown;void*_tmp490=_tmp48D;_LL227: {struct Cyc_Core_Failure_struct*
_tmp491=(struct Cyc_Core_Failure_struct*)_tmp490;if(_tmp491->tag != Cyc_Core_Failure)
goto _LL229;}_LL228:{const char*_tmpA23;nspace=((_tmpA23="",_tag_dyneither(_tmpA23,
sizeof(char),1)));}goto _LL226;_LL229:;_LL22A:(void)_throw(_tmp490);_LL226:;}};}{
struct _handler_cons _tmp493;_push_handler(& _tmp493);{int _tmp495=0;if(setjmp(
_tmp493.handler))_tmp495=1;if(!_tmp495){x=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp494=(void*)_exn_thrown;void*_tmp497=_tmp494;_LL22C: {struct Cyc_Core_Failure_struct*
_tmp498=(struct Cyc_Core_Failure_struct*)_tmp497;if(_tmp498->tag != Cyc_Core_Failure)
goto _LL22E;}_LL22D: x=0;goto _LL22B;_LL22E:;_LL22F:(void)_throw(_tmp497);_LL22B:;}};}{
struct _handler_cons _tmp499;_push_handler(& _tmp499);{int _tmp49B=0;if(setjmp(
_tmp499.handler))_tmp49B=1;if(!_tmp49B){y=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{void*
_tmp49A=(void*)_exn_thrown;void*_tmp49D=_tmp49A;_LL231: {struct Cyc_Core_Failure_struct*
_tmp49E=(struct Cyc_Core_Failure_struct*)_tmp49D;if(_tmp49E->tag != Cyc_Core_Failure)
goto _LL233;}_LL232: y=0;goto _LL230;_LL233:;_LL234:(void)_throw(_tmp49D);_LL230:;}};}{
struct Cyc_Absyn_Decl*_tmpA32;struct Cyc_Absyn_Namespace_d_struct _tmpA31;struct
_dyneither_ptr*_tmpA30;struct Cyc_Absyn_Namespace_d_struct*_tmpA2F;struct Cyc_List_List*
_tmpA2E;yyval=Cyc_YY18(((_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((_tmpA2E->hd=((
_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32->r=(void*)((_tmpA2F=_cycalloc(
sizeof(*_tmpA2F)),((_tmpA2F[0]=((_tmpA31.tag=10,((_tmpA31.f1=((_tmpA30=_cycalloc(
sizeof(*_tmpA30)),((_tmpA30[0]=nspace,_tmpA30)))),((_tmpA31.f2=x,_tmpA31)))))),
_tmpA2F)))),((_tmpA32->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA32)))))),((
_tmpA2E->tl=y,_tmpA2E)))))));}break;}case 7: _LL225:{const char*_tmpA33;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),((_tmpA33="C",_tag_dyneither(_tmpA33,sizeof(char),2))))
== 0){struct Cyc_Absyn_Decl*_tmpA3D;struct Cyc_Absyn_ExternC_d_struct _tmpA3C;
struct Cyc_Absyn_ExternC_d_struct*_tmpA3B;struct Cyc_List_List*_tmpA3A;yyval=Cyc_YY18(((
_tmpA3A=_cycalloc(sizeof(*_tmpA3A)),((_tmpA3A->hd=((_tmpA3D=_cycalloc(sizeof(*
_tmpA3D)),((_tmpA3D->r=(void*)((_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B[0]=((
_tmpA3C.tag=12,((_tmpA3C.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpA3C)))),_tmpA3B)))),((_tmpA3D->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA3D)))))),((
_tmpA3A->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA3A)))))));}else{{const char*_tmpA3E;if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),((_tmpA3E="C include",_tag_dyneither(_tmpA3E,sizeof(char),10))))
!= 0){const char*_tmpA3F;Cyc_Parse_err(((_tmpA3F="expecting \"C\" or \"C include\"",
_tag_dyneither(_tmpA3F,sizeof(char),29))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}}{struct Cyc_Absyn_Decl*
_tmpA49;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA48;struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpA47;struct Cyc_List_List*_tmpA46;yyval=Cyc_YY18(((_tmpA46=_cycalloc(sizeof(*
_tmpA46)),((_tmpA46->hd=((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49->r=(void*)((
_tmpA47=_cycalloc(sizeof(*_tmpA47)),((_tmpA47[0]=((_tmpA48.tag=13,((_tmpA48.f1=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpA48.f2=0,_tmpA48)))))),_tmpA47)))),((_tmpA49->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA49)))))),((
_tmpA46->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA46)))))));};}}break;case 8: _LL235:{const char*_tmpA4A;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]),((_tmpA4A="C include",_tag_dyneither(_tmpA4A,sizeof(
char),10))))!= 0){const char*_tmpA4B;Cyc_Parse_err(((_tmpA4B="expecting \"C include\"",
_tag_dyneither(_tmpA4B,sizeof(char),22))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_Absyn_Decl*_tmpA55;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA54;
struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA53;struct Cyc_List_List*_tmpA52;
yyval=Cyc_YY18(((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->hd=((_tmpA55=
_cycalloc(sizeof(*_tmpA55)),((_tmpA55->r=(void*)((_tmpA53=_cycalloc(sizeof(*
_tmpA53)),((_tmpA53[0]=((_tmpA54.tag=13,((_tmpA54.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA54.f2=exs,_tmpA54)))))),
_tmpA53)))),((_tmpA55->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA55)))))),((
_tmpA52->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA52)))))));}break;};case 9: _LL236:{struct Cyc_Absyn_Decl*
_tmpA58;struct Cyc_List_List*_tmpA57;yyval=Cyc_YY18(((_tmpA57=_cycalloc(sizeof(*
_tmpA57)),((_tmpA57->hd=((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58->r=(void*)&
Cyc_Absyn_Porton_d_val,((_tmpA58->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA58)))))),((
_tmpA57->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA57)))))));}break;case 10: _LL237:{struct Cyc_Absyn_Decl*
_tmpA5B;struct Cyc_List_List*_tmpA5A;yyval=Cyc_YY18(((_tmpA5A=_cycalloc(sizeof(*
_tmpA5A)),((_tmpA5A->hd=((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B->r=(void*)&
Cyc_Absyn_Portoff_d_val,((_tmpA5B->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA5B)))))),((
_tmpA5A->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA5A)))))));}break;case 11: _LL238: yyval=Cyc_YY18(0);break;case
12: _LL239: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 13: _LL23A: yyval=Cyc_YY52(0);break;case 14: _LL23B:{struct _tuple24*
_tmpA5E;struct Cyc_List_List*_tmpA5D;yyval=Cyc_YY52(((_tmpA5D=_cycalloc(sizeof(*
_tmpA5D)),((_tmpA5D->hd=((_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpA5E->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpA5E->f3=0,_tmpA5E)))))))),((_tmpA5D->tl=0,_tmpA5D)))))));}break;case 15:
_LL23C:{struct _tuple24*_tmpA61;struct Cyc_List_List*_tmpA60;yyval=Cyc_YY52(((
_tmpA60=_cycalloc(sizeof(*_tmpA60)),((_tmpA60->hd=((_tmpA61=_cycalloc(sizeof(*
_tmpA61)),((_tmpA61->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpA61->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpA61->f3=0,_tmpA61)))))))),((_tmpA60->tl=0,_tmpA60)))))));}break;case 16:
_LL23D:{struct _tuple24*_tmpA64;struct Cyc_List_List*_tmpA63;yyval=Cyc_YY52(((
_tmpA63=_cycalloc(sizeof(*_tmpA63)),((_tmpA63->hd=((_tmpA64=_cycalloc(sizeof(*
_tmpA64)),((_tmpA64->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpA64->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpA64->f3=0,_tmpA64)))))))),((_tmpA63->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA63)))))));}break;case
17: _LL23E:{struct Cyc_Absyn_Fn_d_struct*_tmpA6A;struct Cyc_Absyn_Fn_d_struct
_tmpA69;struct Cyc_List_List*_tmpA68;yyval=Cyc_YY18(((_tmpA68=_cycalloc(sizeof(*
_tmpA68)),((_tmpA68->hd=Cyc_Absyn_new_decl((void*)((_tmpA6A=_cycalloc(sizeof(*
_tmpA6A)),((_tmpA6A[0]=((_tmpA69.tag=1,((_tmpA69.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA69)))),_tmpA6A)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA68->tl=0,
_tmpA68)))))));}break;case 18: _LL23F: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 19: _LL240: yyval=Cyc_YY18(0);break;case 20: _LL241:
yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 21:
_LL242:{struct Cyc_Core_Opt*_tmpA6B;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA6B)))),Cyc_yyget_YY29(
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
_LL244:{struct Cyc_Core_Opt*_tmpA6C;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA6C)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 24:
_LL245:{struct Cyc_Core_Opt*_tmpA6D;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA6D)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 25:
_LL246:{struct Cyc_Core_Opt*_tmpA6E;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA6E)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 26:
_LL247: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 27: _LL248: Cyc_Lex_leave_using();
break;case 28: _LL249:{struct _dyneither_ptr*_tmpA6F;Cyc_Lex_enter_namespace(((
_tmpA6F=_cycalloc(sizeof(*_tmpA6F)),((_tmpA6F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA6F)))));}yyval=yyvs[
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
_LL24D:{struct Cyc_List_List*_tmpA70;yyval=Cyc_YY18(((_tmpA70=_cycalloc(sizeof(*
_tmpA70)),((_tmpA70->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA70->tl=0,
_tmpA70)))))));}break;case 33: _LL24E: {struct Cyc_List_List*_tmp4C8=0;{struct Cyc_List_List*
_tmp4C9=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
for(0;_tmp4C9 != 0;_tmp4C9=_tmp4C9->tl){struct _dyneither_ptr*_tmp4CA=(struct
_dyneither_ptr*)_tmp4C9->hd;struct _tuple1*_tmpA71;struct _tuple1*qv=(_tmpA71=
_cycalloc(sizeof(*_tmpA71)),((_tmpA71->f1=Cyc_Absyn_Rel_n(0),((_tmpA71->f2=
_tmp4CA,_tmpA71)))));struct Cyc_Absyn_Vardecl*_tmp4CB=Cyc_Absyn_new_vardecl(qv,
Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*_tmpA72;_tmp4C8=((_tmpA72=_cycalloc(
sizeof(*_tmpA72)),((_tmpA72->hd=_tmp4CB,((_tmpA72->tl=_tmp4C8,_tmpA72))))));}}
_tmp4C8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4C8);{struct Cyc_List_List*_tmpA73;yyval=Cyc_YY18(((_tmpA73=_cycalloc(sizeof(*
_tmpA73)),((_tmpA73->hd=Cyc_Absyn_letv_decl(_tmp4C8,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA73->tl=0,
_tmpA73)))))));}break;}case 34: _LL24F:{const char*_tmpA74;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpA74="`H",_tag_dyneither(_tmpA74,sizeof(char),3))))== 0){
const char*_tmpA77;void*_tmpA76;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA76=0,
Cyc_aprintf(((_tmpA77="bad occurrence of heap region",_tag_dyneither(_tmpA77,
sizeof(char),30))),_tag_dyneither(_tmpA76,sizeof(void*),0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{const char*
_tmpA78;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA78="`U",
_tag_dyneither(_tmpA78,sizeof(char),3))))== 0){const char*_tmpA7B;void*_tmpA7A;
Cyc_Parse_err((struct _dyneither_ptr)((_tmpA7A=0,Cyc_aprintf(((_tmpA7B="bad occurrence of unique region",
_tag_dyneither(_tmpA7B,sizeof(char),32))),_tag_dyneither(_tmpA7A,sizeof(void*),0)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));}}{struct _dyneither_ptr*_tmpA7E;struct Cyc_Absyn_Tvar*
_tmpA7D;struct Cyc_Absyn_Tvar*tv=(_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D->name=((
_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA7E)))),((_tmpA7D->identity=
- 1,((_tmpA7D->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA7D)))))));
struct Cyc_Absyn_VarType_struct _tmpA81;struct Cyc_Absyn_VarType_struct*_tmpA80;
void*t=(void*)((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80[0]=((_tmpA81.tag=2,((
_tmpA81.f1=tv,_tmpA81)))),_tmpA80))));struct Cyc_Absyn_RgnHandleType_struct
_tmpA87;struct Cyc_Absyn_RgnHandleType_struct*_tmpA86;struct _dyneither_ptr*
_tmpA84;struct _tuple1*_tmpA83;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83->f1=Cyc_Absyn_Loc_n,((_tmpA83->f2=((
_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA84)))),_tmpA83)))))),(
void*)((_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86[0]=((_tmpA87.tag=16,((
_tmpA87.f1=(void*)t,_tmpA87)))),_tmpA86)))),0);{struct Cyc_List_List*_tmpA88;
yyval=Cyc_YY18(((_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpA88->tl=0,_tmpA88)))))));}break;};case 35:
_LL250:{const char*_tmpA89;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA89="H",
_tag_dyneither(_tmpA89,sizeof(char),2))))== 0){const char*_tmpA8A;Cyc_Parse_err(((
_tmpA8A="bad occurrence of heap region `H",_tag_dyneither(_tmpA8A,sizeof(char),
33))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));}}{const char*_tmpA8B;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmpA8B="U",_tag_dyneither(_tmpA8B,sizeof(char),2))))== 0){
const char*_tmpA8C;Cyc_Parse_err(((_tmpA8C="bad occurrence of unique region `U",
_tag_dyneither(_tmpA8C,sizeof(char),35))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));}}if(Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){const char*
_tmpA8D;Cyc_Parse_err(((_tmpA8D="open regions cannot be @resetable",
_tag_dyneither(_tmpA8D,sizeof(char),34))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}{struct
_dyneither_ptr*_tmpA9A;const char*_tmpA99;void*_tmpA98[1];struct Cyc_String_pa_struct
_tmpA97;struct Cyc_Absyn_Tvar*_tmpA96;struct Cyc_Absyn_Tvar*tv=(_tmpA96=_cycalloc(
sizeof(*_tmpA96)),((_tmpA96->name=((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((
_tmpA9A[0]=(struct _dyneither_ptr)((_tmpA97.tag=0,((_tmpA97.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpA98[0]=& _tmpA97,
Cyc_aprintf(((_tmpA99="`%s",_tag_dyneither(_tmpA99,sizeof(char),4))),
_tag_dyneither(_tmpA98,sizeof(void*),1)))))))),_tmpA9A)))),((_tmpA96->identity=-
1,((_tmpA96->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA96)))))));struct
Cyc_Absyn_VarType_struct _tmpA9D;struct Cyc_Absyn_VarType_struct*_tmpA9C;void*t=(
void*)((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C[0]=((_tmpA9D.tag=2,((
_tmpA9D.f1=tv,_tmpA9D)))),_tmpA9C))));struct Cyc_Absyn_RgnHandleType_struct
_tmpAA3;struct Cyc_Absyn_RgnHandleType_struct*_tmpAA2;struct _dyneither_ptr*
_tmpAA0;struct _tuple1*_tmpA9F;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F->f1=Cyc_Absyn_Loc_n,((_tmpA9F->f2=((
_tmpAA0=_cycalloc(sizeof(*_tmpAA0)),((_tmpAA0[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAA0)))),_tmpA9F)))))),(
void*)((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2[0]=((_tmpAA3.tag=16,((
_tmpAA3.f1=(void*)t,_tmpAA3)))),_tmpAA2)))),0);{struct Cyc_List_List*_tmpAA4;
yyval=Cyc_YY18(((_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAA4->tl=0,
_tmpAA4)))))));}break;};case 36: _LL251: {struct Cyc_Absyn_Eq_kb_struct*_tmpAAC;
struct Cyc_Absyn_Eq_kb_struct _tmpAAB;struct _dyneither_ptr*_tmpAAA;struct Cyc_Absyn_Tvar*
_tmpAA9;struct Cyc_Absyn_Tvar*tv=(_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9->name=((
_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpAAA)))),((_tmpAA9->identity=
- 1,((_tmpAA9->kind=(void*)((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC[0]=((
_tmpAAB.tag=0,((_tmpAAB.f1=& Cyc_Tcutil_rk,_tmpAAB)))),_tmpAAC)))),_tmpAA9)))))));
struct _dyneither_ptr*_tmpAAF;struct _tuple1*_tmpAAE;struct Cyc_Absyn_Vardecl*vd=
Cyc_Absyn_new_vardecl(((_tmpAAE=_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE->f1=Cyc_Absyn_Loc_n,((
_tmpAAE->f2=((_tmpAAF=_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpAAF)))),_tmpAAE)))))),(
void*)& Cyc_Absyn_VoidType_val,0);{struct Cyc_List_List*_tmpAB0;yyval=Cyc_YY18(((
_tmpAB0=_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAB0->tl=0,
_tmpAB0)))))));}break;}case 37: _LL252: yyval=Cyc_YY31(0);break;case 38: _LL253: yyval=
Cyc_YY31(1);break;case 39: _LL254: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 40: _LL255: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 41: _LL256:{
struct Cyc_Core_Opt*_tmpAB3;struct Cyc_Parse_Declaration_spec*_tmpAB2;yyval=Cyc_YY19(((
_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2->sc=((_tmpAB3=_cycalloc_atomic(
sizeof(*_tmpAB3)),((_tmpAB3->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAB3)))),((_tmpAB2->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAB2->type_specs=
0,((_tmpAB2->is_inline=0,((_tmpAB2->attributes=0,_tmpAB2)))))))))))));}break;
case 42: _LL257: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc != 0){const char*_tmpAB6;void*_tmpAB5;(_tmpAB5=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpAB6="Only one storage class is allowed in a declaration",
_tag_dyneither(_tmpAB6,sizeof(char),51))),_tag_dyneither(_tmpAB5,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmpAB9;struct Cyc_Parse_Declaration_spec*_tmpAB8;yyval=Cyc_YY19(((
_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8->sc=((_tmpAB9=_cycalloc_atomic(
sizeof(*_tmpAB9)),((_tmpAB9->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpAB9)))),((_tmpAB8->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAB8->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAB8->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpAB8->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,
_tmpAB8)))))))))))));}break;case 43: _LL258:{struct Cyc_List_List*_tmpABC;struct Cyc_Parse_Declaration_spec*
_tmpABB;yyval=Cyc_YY19(((_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB->sc=0,((
_tmpABB->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpABB->type_specs=((
_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpABC->tl=0,_tmpABC)))))),((
_tmpABB->is_inline=0,((_tmpABB->attributes=0,_tmpABB)))))))))))));}break;case 44:
_LL259:{struct Cyc_List_List*_tmpABF;struct Cyc_Parse_Declaration_spec*_tmpABE;
yyval=Cyc_YY19(((_tmpABE=_cycalloc(sizeof(*_tmpABE)),((_tmpABE->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpABE->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpABE->type_specs=((_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpABF->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,_tmpABF)))))),((
_tmpABE->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpABE->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpABE)))))))))))));}
break;case 45: _LL25A:{struct Cyc_Parse_Declaration_spec*_tmpAC0;yyval=Cyc_YY19(((
_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0->sc=0,((_tmpAC0->tq=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAC0->type_specs=
0,((_tmpAC0->is_inline=0,((_tmpAC0->attributes=0,_tmpAC0)))))))))))));}break;
case 46: _LL25B:{struct Cyc_Parse_Declaration_spec*_tmpAC1;yyval=Cyc_YY19(((_tmpAC1=
_cycalloc(sizeof(*_tmpAC1)),((_tmpAC1->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAC1->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq),((_tmpAC1->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpAC1->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpAC1->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAC1)))))))))))));}
break;case 47: _LL25C:{struct Cyc_Parse_Declaration_spec*_tmpAC2;yyval=Cyc_YY19(((
_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2->sc=0,((_tmpAC2->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAC2->type_specs=0,((_tmpAC2->is_inline=1,((
_tmpAC2->attributes=0,_tmpAC2)))))))))))));}break;case 48: _LL25D:{struct Cyc_Parse_Declaration_spec*
_tmpAC3;yyval=Cyc_YY19(((_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAC3->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAC3->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAC3->is_inline=1,((_tmpAC3->attributes=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAC3)))))))))))));}
break;case 49: _LL25E:{struct Cyc_Parse_Declaration_spec*_tmpAC4;yyval=Cyc_YY19(((
_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4->sc=0,((_tmpAC4->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAC4->type_specs=0,((_tmpAC4->is_inline=0,((
_tmpAC4->attributes=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpAC4)))))))))))));}break;case 50: _LL25F:{struct Cyc_Parse_Declaration_spec*
_tmpAC5;yyval=Cyc_YY19(((_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAC5->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAC5->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAC5->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpAC5->attributes=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes),_tmpAC5)))))))))))));}
break;case 51: _LL260: yyval=Cyc_YY22(Cyc_Parse_Auto_sc);break;case 52: _LL261: yyval=
Cyc_YY22(Cyc_Parse_Register_sc);break;case 53: _LL262: yyval=Cyc_YY22(Cyc_Parse_Static_sc);
break;case 54: _LL263: yyval=Cyc_YY22(Cyc_Parse_Extern_sc);break;case 55: _LL264:{
const char*_tmpAC6;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAC6="C",
_tag_dyneither(_tmpAC6,sizeof(char),2))))!= 0){const char*_tmpAC7;Cyc_Parse_err(((
_tmpAC7="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAC7,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}}yyval=Cyc_YY22(Cyc_Parse_ExternC_sc);break;case 56:
_LL265: yyval=Cyc_YY22(Cyc_Parse_Typedef_sc);break;case 57: _LL266: yyval=Cyc_YY22(
Cyc_Parse_Abstract_sc);break;case 58: _LL267: yyval=Cyc_YY45(0);break;case 59: _LL268:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 60:
_LL269: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 61: _LL26A:{struct Cyc_List_List*_tmpAC8;yyval=Cyc_YY45(((_tmpAC8=_cycalloc(
sizeof(*_tmpAC8)),((_tmpAC8->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAC8->tl=0,_tmpAC8)))))));}
break;case 62: _LL26B:{struct Cyc_List_List*_tmpAC9;yyval=Cyc_YY45(((_tmpAC9=
_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpAC9->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAC9)))))));}break;
case 63: _LL26C: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={6,- 1};
static struct _tuple25 att_map[17]={{{_tmp50C,_tmp50C,_tmp50C + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{
_tmp50D,_tmp50D,_tmp50D + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp50E,_tmp50E,
_tmp50E + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp50F,_tmp50F,_tmp50F + 9},(
void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp510,_tmp510,_tmp510 + 6},(void*)& Cyc_Absyn_Const_att_val},{{
_tmp511,_tmp511,_tmp511 + 8},(void*)& att_aligned},{{_tmp512,_tmp512,_tmp512 + 7},(
void*)& Cyc_Absyn_Packed_att_val},{{_tmp513,_tmp513,_tmp513 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{
_tmp514,_tmp514,_tmp514 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp515,_tmp515,
_tmp515 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp516,_tmp516,_tmp516 + 10},(void*)&
Cyc_Absyn_Dllimport_att_val},{{_tmp517,_tmp517,_tmp517 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{
_tmp518,_tmp518,_tmp518 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{
_tmp519,_tmp519,_tmp519 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp51A,
_tmp51A,_tmp51A + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp51B,_tmp51B,
_tmp51B + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp51C,_tmp51C,
_tmp51C + 5},(void*)& Cyc_Absyn_Pure_att_val}};struct _dyneither_ptr _tmp50A=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp50A,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp50A,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp50A,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp50A,sizeof(char),(int)(_get_dyneither_size(
_tmp50A,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp50A,sizeof(char),(int)(_get_dyneither_size(_tmp50A,sizeof(char))- 3)))== '_')
_tmp50A=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp50A,2,
_get_dyneither_size(_tmp50A,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp50A,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){{const char*_tmpACA;Cyc_Parse_err(((
_tmpACA="unrecognized attribute",_tag_dyneither(_tmpACA,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=Cyc_YY46((
void*)& Cyc_Absyn_Cdecl_att_val);}break;};}case 64: _LL26D: yyval=Cyc_YY46((void*)&
Cyc_Absyn_Const_att_val);break;case 65: _LL26E: {struct _dyneither_ptr _tmp51E=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp520;struct
_tuple6 _tmp51F=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp520=_tmp51F.f2;{void*a;{const char*_tmpACC;const char*
_tmpACB;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51E,((_tmpACC="regparm",
_tag_dyneither(_tmpACC,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp51E,((_tmpACB="__regparm__",_tag_dyneither(_tmpACB,sizeof(
char),12))))== 0){if(_tmp520 < 0  || _tmp520 > 3){const char*_tmpACD;Cyc_Parse_err(((
_tmpACD="regparm requires value between 0 and 3",_tag_dyneither(_tmpACD,sizeof(
char),39))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Regparm_att_struct _tmpAD0;
struct Cyc_Absyn_Regparm_att_struct*_tmpACF;a=(void*)((_tmpACF=_cycalloc_atomic(
sizeof(*_tmpACF)),((_tmpACF[0]=((_tmpAD0.tag=0,((_tmpAD0.f1=_tmp520,_tmpAD0)))),
_tmpACF))));};}else{const char*_tmpAD2;const char*_tmpAD1;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp51E,((_tmpAD2="aligned",_tag_dyneither(_tmpAD2,sizeof(char),8))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp51E,((_tmpAD1="__aligned__",
_tag_dyneither(_tmpAD1,sizeof(char),12))))== 0){if(_tmp520 < 0){const char*_tmpAD3;
Cyc_Parse_err(((_tmpAD3="aligned requires positive power of two",_tag_dyneither(
_tmpAD3,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp520;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
const char*_tmpAD4;Cyc_Parse_err(((_tmpAD4="aligned requires positive power of two",
_tag_dyneither(_tmpAD4,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmpAD7;struct Cyc_Absyn_Aligned_att_struct*_tmpAD6;a=(void*)((_tmpAD6=
_cycalloc_atomic(sizeof(*_tmpAD6)),((_tmpAD6[0]=((_tmpAD7.tag=6,((_tmpAD7.f1=
_tmp520,_tmpAD7)))),_tmpAD6))));};};}else{const char*_tmpAD9;const char*_tmpAD8;
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51E,((_tmpAD9="initializes",
_tag_dyneither(_tmpAD9,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp51E,((_tmpAD8="__initializes__",_tag_dyneither(_tmpAD8,
sizeof(char),16))))== 0){struct Cyc_Absyn_Initializes_att_struct _tmpADC;struct Cyc_Absyn_Initializes_att_struct*
_tmpADB;a=(void*)((_tmpADB=_cycalloc_atomic(sizeof(*_tmpADB)),((_tmpADB[0]=((
_tmpADC.tag=20,((_tmpADC.f1=_tmp520,_tmpADC)))),_tmpADB))));}else{const char*
_tmpADE;const char*_tmpADD;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51E,((_tmpADE="noliveunique",
_tag_dyneither(_tmpADE,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp51E,((_tmpADD="__noliveunique__",_tag_dyneither(_tmpADD,
sizeof(char),17))))== 0){struct Cyc_Absyn_Noliveunique_att_struct _tmpAE1;struct
Cyc_Absyn_Noliveunique_att_struct*_tmpAE0;a=(void*)((_tmpAE0=_cycalloc_atomic(
sizeof(*_tmpAE0)),((_tmpAE0[0]=((_tmpAE1.tag=21,((_tmpAE1.f1=_tmp520,_tmpAE1)))),
_tmpAE0))));}else{{const char*_tmpAE2;Cyc_Parse_err(((_tmpAE2="unrecognized attribute",
_tag_dyneither(_tmpAE2,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}}}}yyval=Cyc_YY46(a);break;};}case 66: _LL26F: {struct
_dyneither_ptr _tmp535=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp536=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpAE4;const char*_tmpAE3;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp535,((_tmpAE4="section",
_tag_dyneither(_tmpAE4,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp535,((_tmpAE3="__section__",_tag_dyneither(_tmpAE3,sizeof(
char),12))))== 0){struct Cyc_Absyn_Section_att_struct _tmpAE7;struct Cyc_Absyn_Section_att_struct*
_tmpAE6;a=(void*)((_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6[0]=((_tmpAE7.tag=
8,((_tmpAE7.f1=_tmp536,_tmpAE7)))),_tmpAE6))));}else{{const char*_tmpAE8;Cyc_Parse_err(((
_tmpAE8="unrecognized attribute",_tag_dyneither(_tmpAE8,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 67: _LL270: {struct
_dyneither_ptr _tmp53C=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp53D=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpAE9;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp53C,((_tmpAE9="__mode__",
_tag_dyneither(_tmpAE9,sizeof(char),9))))== 0){struct Cyc_Absyn_Mode_att_struct
_tmpAEC;struct Cyc_Absyn_Mode_att_struct*_tmpAEB;a=(void*)((_tmpAEB=_cycalloc(
sizeof(*_tmpAEB)),((_tmpAEB[0]=((_tmpAEC.tag=23,((_tmpAEC.f1=_tmp53D,_tmpAEC)))),
_tmpAEB))));}else{{const char*_tmpAED;Cyc_Parse_err(((_tmpAED="unrecognized attribute",
_tag_dyneither(_tmpAED,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 68: _LL271: {struct
_dyneither_ptr _tmp542=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp543=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp545;struct _tuple6
_tmp544=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp545=_tmp544.f2;{int _tmp547;struct _tuple6 _tmp546=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp547=_tmp546.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;{const char*_tmpAEF;const char*_tmpAEE;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp542,((_tmpAEF="format",_tag_dyneither(
_tmpAEF,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp542,((
_tmpAEE="__format__",_tag_dyneither(_tmpAEE,sizeof(char),11))))== 0){const char*
_tmpAF0;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp543,((_tmpAF0="printf",
_tag_dyneither(_tmpAF0,sizeof(char),7))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpAF3;struct Cyc_Absyn_Format_att_struct*_tmpAF2;a=(void*)((_tmpAF2=
_cycalloc_atomic(sizeof(*_tmpAF2)),((_tmpAF2[0]=((_tmpAF3.tag=19,((_tmpAF3.f1=
Cyc_Absyn_Printf_ft,((_tmpAF3.f2=_tmp545,((_tmpAF3.f3=_tmp547,_tmpAF3)))))))),
_tmpAF2))));}else{const char*_tmpAF4;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp543,((
_tmpAF4="scanf",_tag_dyneither(_tmpAF4,sizeof(char),6))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpAF7;struct Cyc_Absyn_Format_att_struct*_tmpAF6;a=(void*)((_tmpAF6=
_cycalloc_atomic(sizeof(*_tmpAF6)),((_tmpAF6[0]=((_tmpAF7.tag=19,((_tmpAF7.f1=
Cyc_Absyn_Scanf_ft,((_tmpAF7.f2=_tmp545,((_tmpAF7.f3=_tmp547,_tmpAF7)))))))),
_tmpAF6))));}else{const char*_tmpAF8;Cyc_Parse_err(((_tmpAF8="unrecognized format type",
_tag_dyneither(_tmpAF8,sizeof(char),25))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{
const char*_tmpAF9;Cyc_Parse_err(((_tmpAF9="unrecognized attribute",
_tag_dyneither(_tmpAF9,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY46(
a);break;};};}case 69: _LL272: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 70: _LL273:{struct Cyc_Absyn_TypedefType_struct _tmpAFC;
struct Cyc_Absyn_TypedefType_struct*_tmpAFB;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB[0]=((_tmpAFC.tag=18,((
_tmpAFC.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpAFC.f2=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAFC.f3=0,((_tmpAFC.f4=
0,_tmpAFC)))))))))),_tmpAFB)))),Cyc_Position_segment_of_abs((yyls[
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
_LL278:{struct Cyc_Parse_Short_spec_struct _tmpAFF;struct Cyc_Parse_Short_spec_struct*
_tmpAFE;yyval=Cyc_YY23((void*)((_tmpAFE=_cycalloc(sizeof(*_tmpAFE)),((_tmpAFE[0]=((
_tmpAFF.tag=2,((_tmpAFF.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpAFF)))),
_tmpAFE)))));}break;case 76: _LL279: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 77: _LL27A:{struct Cyc_Parse_Long_spec_struct
_tmpB02;struct Cyc_Parse_Long_spec_struct*_tmpB01;yyval=Cyc_YY23((void*)((_tmpB01=
_cycalloc(sizeof(*_tmpB01)),((_tmpB01[0]=((_tmpB02.tag=3,((_tmpB02.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB02)))),
_tmpB01)))));}break;case 78: _LL27B: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 79: _LL27C: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 80:
_LL27D:{struct Cyc_Parse_Signed_spec_struct _tmpB05;struct Cyc_Parse_Signed_spec_struct*
_tmpB04;yyval=Cyc_YY23((void*)((_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04[0]=((
_tmpB05.tag=0,((_tmpB05.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB05)))),
_tmpB04)))));}break;case 81: _LL27E:{struct Cyc_Parse_Unsigned_spec_struct _tmpB08;
struct Cyc_Parse_Unsigned_spec_struct*_tmpB07;yyval=Cyc_YY23((void*)((_tmpB07=
_cycalloc(sizeof(*_tmpB07)),((_tmpB07[0]=((_tmpB08.tag=1,((_tmpB08.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB08)))),
_tmpB07)))));}break;case 82: _LL27F: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 83: _LL280: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84: _LL281: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 85: _LL282:
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 86:
_LL283:{struct Cyc_Absyn_TupleType_struct _tmpB0B;struct Cyc_Absyn_TupleType_struct*
_tmpB0A;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB0A=_cycalloc(sizeof(*
_tmpB0A)),((_tmpB0A[0]=((_tmpB0B.tag=11,((_tmpB0B.f1=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))),_tmpB0B)))),_tmpB0A)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 87: _LL284:{struct Cyc_Absyn_RgnHandleType_struct
_tmpB0E;struct Cyc_Absyn_RgnHandleType_struct*_tmpB0D;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D[0]=((_tmpB0E.tag=16,((
_tmpB0E.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpB0E)))),_tmpB0D)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 88:
_LL285:{struct Cyc_Absyn_RgnHandleType_struct _tmpB11;struct Cyc_Absyn_RgnHandleType_struct*
_tmpB10;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB10=_cycalloc(sizeof(*
_tmpB10)),((_tmpB10[0]=((_tmpB11.tag=16,((_tmpB11.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0),_tmpB11)))),_tmpB10)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 89:
_LL286: {void*_tmp562=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,0);{
struct Cyc_Absyn_DynRgnType_struct _tmpB14;struct Cyc_Absyn_DynRgnType_struct*
_tmpB13;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB13=_cycalloc(sizeof(*
_tmpB13)),((_tmpB13[0]=((_tmpB14.tag=17,((_tmpB14.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB14.f2=(void*)
_tmp562,_tmpB14)))))),_tmpB13)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case 90:
_LL287:{struct Cyc_Absyn_DynRgnType_struct _tmpB17;struct Cyc_Absyn_DynRgnType_struct*
_tmpB16;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB16=_cycalloc(sizeof(*
_tmpB16)),((_tmpB16[0]=((_tmpB17.tag=17,((_tmpB17.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB17.f2=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB17)))))),
_tmpB16)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 91: _LL288:{struct Cyc_Absyn_TagType_struct
_tmpB1A;struct Cyc_Absyn_TagType_struct*_tmpB19;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB19=_cycalloc(sizeof(*_tmpB19)),((_tmpB19[0]=((_tmpB1A.tag=20,((
_tmpB1A.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpB1A)))),_tmpB19)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 92:
_LL289:{struct Cyc_Absyn_TagType_struct _tmpB1D;struct Cyc_Absyn_TagType_struct*
_tmpB1C;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB1C=_cycalloc(sizeof(*
_tmpB1C)),((_tmpB1C[0]=((_tmpB1D.tag=20,((_tmpB1D.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_iko,0),_tmpB1D)))),_tmpB1C)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 93:
_LL28A:{struct Cyc_Absyn_ValueofType_struct _tmpB20;struct Cyc_Absyn_ValueofType_struct*
_tmpB1F;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB1F=_cycalloc(sizeof(*
_tmpB1F)),((_tmpB1F[0]=((_tmpB20.tag=19,((_tmpB20.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB20)))),_tmpB1F)))),
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
_tmpB21;yyval=Cyc_YY25(((_tmpB21.print_const=1,((_tmpB21.q_volatile=0,((_tmpB21.q_restrict=
0,((_tmpB21.real_const=1,((_tmpB21.loc=loc,_tmpB21)))))))))));}break;}case 96:
_LL28D:{struct Cyc_Absyn_Tqual _tmpB22;yyval=Cyc_YY25(((_tmpB22.print_const=0,((
_tmpB22.q_volatile=1,((_tmpB22.q_restrict=0,((_tmpB22.real_const=0,((_tmpB22.loc=
0,_tmpB22)))))))))));}break;case 97: _LL28E:{struct Cyc_Absyn_Tqual _tmpB23;yyval=
Cyc_YY25(((_tmpB23.print_const=0,((_tmpB23.q_volatile=0,((_tmpB23.q_restrict=1,((
_tmpB23.real_const=0,((_tmpB23.loc=0,_tmpB23)))))))))));}break;case 98: _LL28F: {
struct Cyc_Absyn_Enum_td_struct*_tmpB32;struct Cyc_Absyn_Enumdecl*_tmpB31;struct
Cyc_Core_Opt*_tmpB30;struct Cyc_Absyn_Enum_td_struct _tmpB2F;struct Cyc_Absyn_TypeDecl*
_tmpB2E;struct Cyc_Absyn_TypeDecl*_tmp570=(_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((
_tmpB2E->r=(void*)((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32[0]=((_tmpB2F.tag=
1,((_tmpB2F.f1=((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31->sc=Cyc_Absyn_Public,((
_tmpB31->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpB31->fields=((_tmpB30=_cycalloc(sizeof(*_tmpB30)),((
_tmpB30->v=Cyc_yyget_YY48(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),_tmpB30)))),_tmpB31)))))))),_tmpB2F)))),_tmpB32)))),((_tmpB2E->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB2E)))));{
struct Cyc_Absyn_TypeDeclType_struct _tmpB35;struct Cyc_Absyn_TypeDeclType_struct*
_tmpB34;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB34=_cycalloc(sizeof(*
_tmpB34)),((_tmpB34[0]=((_tmpB35.tag=26,((_tmpB35.f1=_tmp570,((_tmpB35.f2=0,
_tmpB35)))))),_tmpB34)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case 99:
_LL290:{struct Cyc_Absyn_EnumType_struct _tmpB38;struct Cyc_Absyn_EnumType_struct*
_tmpB37;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB37=_cycalloc(sizeof(*
_tmpB37)),((_tmpB37[0]=((_tmpB38.tag=14,((_tmpB38.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB38.f2=0,_tmpB38)))))),
_tmpB37)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 100: _LL291:{struct Cyc_Absyn_AnonEnumType_struct
_tmpB3B;struct Cyc_Absyn_AnonEnumType_struct*_tmpB3A;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB3A=_cycalloc(sizeof(*_tmpB3A)),((_tmpB3A[0]=((_tmpB3B.tag=15,((
_tmpB3B.f1=Cyc_yyget_YY48(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),_tmpB3B)))),_tmpB3A)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 101:
_LL292:{struct Cyc_Absyn_Enumfield*_tmpB3C;yyval=Cyc_YY47(((_tmpB3C=_cycalloc(
sizeof(*_tmpB3C)),((_tmpB3C->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB3C->tag=0,((_tmpB3C->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB3C)))))))));}break;case 102: _LL293:{struct Cyc_Absyn_Enumfield*
_tmpB3D;yyval=Cyc_YY47(((_tmpB3D=_cycalloc(sizeof(*_tmpB3D)),((_tmpB3D->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB3D->tag=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB3D->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB3D)))))))));}
break;case 103: _LL294:{struct Cyc_List_List*_tmpB3E;yyval=Cyc_YY48(((_tmpB3E=
_cycalloc(sizeof(*_tmpB3E)),((_tmpB3E->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB3E->tl=0,_tmpB3E)))))));}
break;case 104: _LL295:{struct Cyc_List_List*_tmpB3F;yyval=Cyc_YY48(((_tmpB3F=
_cycalloc(sizeof(*_tmpB3F)),((_tmpB3F->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB3F->tl=0,_tmpB3F)))))));}
break;case 105: _LL296:{struct Cyc_List_List*_tmpB40;yyval=Cyc_YY48(((_tmpB40=
_cycalloc(sizeof(*_tmpB40)),((_tmpB40->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB40->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB40)))))));}break;
case 106: _LL297:{struct Cyc_Absyn_AnonAggrType_struct _tmpB43;struct Cyc_Absyn_AnonAggrType_struct*
_tmpB42;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB42=_cycalloc(sizeof(*
_tmpB42)),((_tmpB42[0]=((_tmpB43.tag=13,((_tmpB43.f1=Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB43.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB43)))))),
_tmpB42)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 107: _LL298: {struct Cyc_List_List*_tmp583=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp584=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));struct Cyc_Absyn_TypeDecl*
_tmp585=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp583,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp584,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_TypeDeclType_struct
_tmpB46;struct Cyc_Absyn_TypeDeclType_struct*_tmpB45;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB45=_cycalloc(sizeof(*_tmpB45)),((_tmpB45[0]=((_tmpB46.tag=26,((
_tmpB46.f1=_tmp585,((_tmpB46.f2=0,_tmpB46)))))),_tmpB45)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
108: _LL299: {struct Cyc_List_List*_tmp588=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp589=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));struct Cyc_Absyn_TypeDecl*_tmp58A=Cyc_Absyn_aggr_tdecl(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tmp588,(struct Cyc_Absyn_AggrdeclImpl*)Cyc_Absyn_aggrdecl_impl(_tmp589,Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_TypeDeclType_struct
_tmpB49;struct Cyc_Absyn_TypeDeclType_struct*_tmpB48;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB48=_cycalloc(sizeof(*_tmpB48)),((_tmpB48[0]=((_tmpB49.tag=26,((
_tmpB49.f1=_tmp58A,((_tmpB49.f2=0,_tmpB49)))))),_tmpB48)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
109: _LL29A:{struct Cyc_Absyn_AggrType_struct _tmpB53;struct Cyc_Core_Opt*_tmpB52;
struct Cyc_Absyn_AggrInfo _tmpB51;struct Cyc_Absyn_AggrType_struct*_tmpB50;yyval=
Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB50=_cycalloc(sizeof(*_tmpB50)),((
_tmpB50[0]=((_tmpB53.tag=12,((_tmpB53.f1=((_tmpB51.aggr_info=Cyc_Absyn_UnknownAggr(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB52=
_cycalloc_atomic(sizeof(*_tmpB52)),((_tmpB52->v=(void*)1,_tmpB52))))),((_tmpB51.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB51)))),
_tmpB53)))),_tmpB50)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 110:
_LL29B:{struct Cyc_Absyn_AggrType_struct _tmpB59;struct Cyc_Absyn_AggrInfo _tmpB58;
struct Cyc_Absyn_AggrType_struct*_tmpB57;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((
_tmpB57=_cycalloc(sizeof(*_tmpB57)),((_tmpB57[0]=((_tmpB59.tag=12,((_tmpB59.f1=((
_tmpB58.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),((_tmpB58.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB58)))),
_tmpB59)))),_tmpB57)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 111:
_LL29C: yyval=Cyc_YY40(0);break;case 112: _LL29D: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 113: _LL29E:
yyval=Cyc_YY24(Cyc_Absyn_StructA);break;case 114: _LL29F: yyval=Cyc_YY24(Cyc_Absyn_UnionA);
break;case 115: _LL2A0: yyval=Cyc_YY26(0);break;case 116: _LL2A1: {struct Cyc_List_List*
_tmp594=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));struct Cyc_List_List*
_tmp595=Cyc_Parse_get_argrfield_tags(_tmp594);if(_tmp595 != 0)((void(*)(void(*f)(
struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct
Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp595,
_tmp594);yyval=Cyc_YY26(_tmp594);break;}case 117: _LL2A2:{struct Cyc_List_List*
_tmpB5A;yyval=Cyc_YY27(((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A->hd=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB5A->tl=0,
_tmpB5A)))))));}break;case 118: _LL2A3:{struct Cyc_List_List*_tmpB5B;yyval=Cyc_YY27(((
_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((_tmpB5B->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB5B->tl=Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB5B)))))));}
break;case 119: _LL2A4: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 120: _LL2A5:{struct Cyc_List_List*_tmpB5C;yyval=Cyc_YY21(((
_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB5C->tl=0,_tmpB5C)))))));}
break;case 121: _LL2A6:{struct Cyc_List_List*_tmpB5D;yyval=Cyc_YY21(((_tmpB5D=
_cycalloc(sizeof(*_tmpB5D)),((_tmpB5D->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB5D->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB5D)))))));}
break;case 122: _LL2A7:{struct _tuple21*_tmpB5E;yyval=Cyc_YY20(((_tmpB5E=_cycalloc(
sizeof(*_tmpB5E)),((_tmpB5E->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB5E->f2=0,_tmpB5E)))))));}
break;case 123: _LL2A8:{struct _tuple21*_tmpB5F;yyval=Cyc_YY20(((_tmpB5F=_cycalloc(
sizeof(*_tmpB5F)),((_tmpB5F->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB5F->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB5F)))))));}
break;case 124: _LL2A9: {struct _RegionHandle _tmp59C=_new_region("temp");struct
_RegionHandle*temp=& _tmp59C;_push_region(temp);{struct _tuple22 _tmp59E;struct Cyc_Absyn_Tqual
_tmp59F;struct Cyc_List_List*_tmp5A0;struct Cyc_List_List*_tmp5A1;struct _tuple22*
_tmp59D=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp59E=*_tmp59D;_tmp59F=_tmp59E.f1;_tmp5A0=_tmp59E.f2;_tmp5A1=_tmp59E.f3;if(
_tmp59F.loc == 0)_tmp59F.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);{struct Cyc_List_List*
_tmp5A3;struct Cyc_List_List*_tmp5A4;struct _tuple0 _tmp5A2=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
temp,temp,Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]));_tmp5A3=_tmp5A2.f1;_tmp5A4=_tmp5A2.f2;{void*_tmp5A5=Cyc_Parse_speclist2typ(
_tmp5A0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp5A6=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(temp,_tmp59F,_tmp5A5,_tmp5A3,
_tmp5A1),_tmp5A4);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple13*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp5A6));};};}
_npop_handler(0);break;;_pop_region(temp);}case 125: _LL2AA:{struct Cyc_List_List*
_tmpB62;struct _tuple22*_tmpB61;yyval=Cyc_YY35(((_tmpB61=_cycalloc(sizeof(*
_tmpB61)),((_tmpB61->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB61->f2=((
_tmpB62=_cycalloc(sizeof(*_tmpB62)),((_tmpB62->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB62->tl=0,_tmpB62)))))),((
_tmpB61->f3=0,_tmpB61)))))))));}break;case 126: _LL2AB:{struct Cyc_List_List*
_tmpB65;struct _tuple22*_tmpB64;yyval=Cyc_YY35(((_tmpB64=_cycalloc(sizeof(*
_tmpB64)),((_tmpB64->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpB64->f2=((_tmpB65=_cycalloc(sizeof(*_tmpB65)),((
_tmpB65->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB65->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpB65)))))),((_tmpB64->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB64)))))))));}break;case 127: _LL2AC:{struct _tuple22*_tmpB66;yyval=Cyc_YY35(((
_tmpB66=_cycalloc(sizeof(*_tmpB66)),((_tmpB66->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB66->f2=0,((_tmpB66->f3=
0,_tmpB66)))))))));}break;case 128: _LL2AD:{struct _tuple22*_tmpB67;yyval=Cyc_YY35(((
_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpB67->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpB67->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpB67)))))))));}break;case 129: _LL2AE:{struct _tuple22*
_tmpB68;yyval=Cyc_YY35(((_tmpB68=_cycalloc(sizeof(*_tmpB68)),((_tmpB68->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB68->f2=0,((_tmpB68->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB68)))))))));}break;
case 130: _LL2AF:{struct _tuple22*_tmpB69;yyval=Cyc_YY35(((_tmpB69=_cycalloc(
sizeof(*_tmpB69)),((_tmpB69->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpB69->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpB69->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpB69)))))))));}break;case 131: _LL2B0:{struct Cyc_List_List*_tmpB6C;struct
_tuple22*_tmpB6B;yyval=Cyc_YY35(((_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((_tmpB6B->f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB6B->f2=((
_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB6C->tl=0,_tmpB6C)))))),((
_tmpB6B->f3=0,_tmpB6B)))))))));}break;case 132: _LL2B1:{struct Cyc_List_List*
_tmpB6F;struct _tuple22*_tmpB6E;yyval=Cyc_YY35(((_tmpB6E=_cycalloc(sizeof(*
_tmpB6E)),((_tmpB6E->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpB6E->f2=((_tmpB6F=_cycalloc(sizeof(*_tmpB6F)),((
_tmpB6F->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB6F->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpB6F)))))),((_tmpB6E->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB6E)))))))));}break;case 133: _LL2B2:{struct _tuple22*_tmpB70;yyval=Cyc_YY35(((
_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB70->f2=0,((_tmpB70->f3=
0,_tmpB70)))))))));}break;case 134: _LL2B3:{struct _tuple22*_tmpB71;yyval=Cyc_YY35(((
_tmpB71=_cycalloc(sizeof(*_tmpB71)),((_tmpB71->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpB71->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpB71->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpB71)))))))));}break;case 135: _LL2B4:{struct _tuple22*
_tmpB72;yyval=Cyc_YY35(((_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB72->f2=0,((_tmpB72->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB72)))))))));}break;
case 136: _LL2B5:{struct _tuple22*_tmpB73;yyval=Cyc_YY35(((_tmpB73=_cycalloc(
sizeof(*_tmpB73)),((_tmpB73->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpB73->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpB73->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpB73)))))))));}break;case 137: _LL2B6: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 138: _LL2B7:{
struct Cyc_List_List*_tmpB74;yyval=Cyc_YY21(((_tmpB74=_cycalloc(sizeof(*_tmpB74)),((
_tmpB74->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB74->tl=0,_tmpB74)))))));}break;case 139: _LL2B8:{struct Cyc_List_List*
_tmpB75;yyval=Cyc_YY21(((_tmpB75=_cycalloc(sizeof(*_tmpB75)),((_tmpB75->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB75->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB75)))))));}
break;case 140: _LL2B9:{struct _tuple21*_tmpB76;yyval=Cyc_YY20(((_tmpB76=_cycalloc(
sizeof(*_tmpB76)),((_tmpB76->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB76->f2=0,_tmpB76)))))));}
break;case 141: _LL2BA:{struct Cyc_Parse_Declarator*_tmpB7C;struct _tuple1*_tmpB7B;
struct _tuple21*_tmpB7A;yyval=Cyc_YY20(((_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((
_tmpB7A->f1=((_tmpB7C=_cycalloc(sizeof(*_tmpB7C)),((_tmpB7C->id=((_tmpB7B=
_cycalloc(sizeof(*_tmpB7B)),((_tmpB7B->f1=Cyc_Absyn_Rel_n(0),((_tmpB7B->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),
_tmpB7B)))))),((_tmpB7C->tms=0,_tmpB7C)))))),((_tmpB7A->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB7A)))))));}
break;case 142: _LL2BB:{struct Cyc_Parse_Declarator*_tmpB82;struct _tuple1*_tmpB81;
struct _tuple21*_tmpB80;yyval=Cyc_YY20(((_tmpB80=_cycalloc(sizeof(*_tmpB80)),((
_tmpB80->f1=((_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82->id=((_tmpB81=
_cycalloc(sizeof(*_tmpB81)),((_tmpB81->f1=Cyc_Absyn_Rel_n(0),((_tmpB81->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),
_tmpB81)))))),((_tmpB82->tms=0,_tmpB82)))))),((_tmpB80->f2=0,_tmpB80)))))));}
break;case 143: _LL2BC:{struct _tuple21*_tmpB83;yyval=Cyc_YY20(((_tmpB83=_cycalloc(
sizeof(*_tmpB83)),((_tmpB83->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB83->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB83)))))));}
break;case 144: _LL2BD: {int _tmp5C3=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);struct Cyc_List_List*
_tmp5C4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));struct Cyc_Core_Opt*_tmpB84;struct Cyc_Absyn_TypeDecl*
_tmp5C5=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp5C4,((_tmpB84=
_cycalloc(sizeof(*_tmpB84)),((_tmpB84->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB84)))),_tmp5C3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_TypeDeclType_struct
_tmpB87;struct Cyc_Absyn_TypeDeclType_struct*_tmpB86;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB86=_cycalloc(sizeof(*_tmpB86)),((_tmpB86[0]=((_tmpB87.tag=26,((
_tmpB87.f1=_tmp5C5,((_tmpB87.f2=0,_tmpB87)))))),_tmpB86)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
145: _LL2BE: {int _tmp5C9=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);{struct Cyc_Absyn_DatatypeType_struct _tmpB91;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpB90;struct Cyc_Absyn_DatatypeInfo _tmpB8F;struct Cyc_Absyn_DatatypeType_struct*
_tmpB8E;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB8E=_cycalloc(sizeof(*
_tmpB8E)),((_tmpB8E[0]=((_tmpB91.tag=3,((_tmpB91.f1=((_tmpB8F.datatype_info=Cyc_Absyn_UnknownDatatype(((
_tmpB90.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB90.is_extensible=_tmp5C9,_tmpB90))))),((_tmpB8F.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB8F)))),
_tmpB91)))),_tmpB8E)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
146: _LL2BF: {int _tmp5CE=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);{struct Cyc_Absyn_DatatypeFieldType_struct _tmpB9B;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpB9A;struct Cyc_Absyn_DatatypeFieldInfo _tmpB99;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpB98;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB98=_cycalloc(sizeof(*
_tmpB98)),((_tmpB98[0]=((_tmpB9B.tag=4,((_tmpB9B.f1=((_tmpB99.field_info=Cyc_Absyn_UnknownDatatypefield(((
_tmpB9A.datatype_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmpB9A.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB9A.is_extensible=
_tmp5CE,_tmpB9A))))))),((_tmpB99.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB99)))),_tmpB9B)))),
_tmpB98)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 147: _LL2C0: yyval=Cyc_YY31(0);break;case
148: _LL2C1: yyval=Cyc_YY31(1);break;case 149: _LL2C2:{struct Cyc_List_List*_tmpB9C;
yyval=Cyc_YY34(((_tmpB9C=_cycalloc(sizeof(*_tmpB9C)),((_tmpB9C->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB9C->tl=0,
_tmpB9C)))))));}break;case 150: _LL2C3:{struct Cyc_List_List*_tmpB9D;yyval=Cyc_YY34(((
_tmpB9D=_cycalloc(sizeof(*_tmpB9D)),((_tmpB9D->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB9D->tl=0,_tmpB9D)))))));}
break;case 151: _LL2C4:{struct Cyc_List_List*_tmpB9E;yyval=Cyc_YY34(((_tmpB9E=
_cycalloc(sizeof(*_tmpB9E)),((_tmpB9E->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB9E->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB9E)))))));}break;
case 152: _LL2C5:{struct Cyc_List_List*_tmpB9F;yyval=Cyc_YY34(((_tmpB9F=_cycalloc(
sizeof(*_tmpB9F)),((_tmpB9F->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB9F->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB9F)))))));}break;
case 153: _LL2C6: yyval=Cyc_YY32(Cyc_Absyn_Public);break;case 154: _LL2C7: yyval=Cyc_YY32(
Cyc_Absyn_Extern);break;case 155: _LL2C8: yyval=Cyc_YY32(Cyc_Absyn_Static);break;
case 156: _LL2C9:{struct Cyc_Absyn_Datatypefield*_tmpBA0;yyval=Cyc_YY33(((_tmpBA0=
_cycalloc(sizeof(*_tmpBA0)),((_tmpBA0->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBA0->typs=0,((_tmpBA0->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpBA0->sc=Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBA0)))))))))));}
break;case 157: _LL2CA: {struct Cyc_List_List*_tmp5D8=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Absyn_Datatypefield*
_tmpBA1;yyval=Cyc_YY33(((_tmpBA1=_cycalloc(sizeof(*_tmpBA1)),((_tmpBA1->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpBA1->typs=
_tmp5D8,((_tmpBA1->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpBA1->sc=
Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmpBA1)))))))))));}break;}case 158: _LL2CB: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 159: _LL2CC:{struct
Cyc_Parse_Declarator*_tmpBA2;yyval=Cyc_YY29(((_tmpBA2=_cycalloc(sizeof(*_tmpBA2)),((
_tmpBA2->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id,((_tmpBA2->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpBA2)))))));}
break;case 160: _LL2CD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 161: _LL2CE:{struct Cyc_Parse_Declarator*_tmpBA3;yyval=
Cyc_YY29(((_tmpBA3=_cycalloc(sizeof(*_tmpBA3)),((_tmpBA3->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->id,((_tmpBA3->tms=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpBA3)))))));}
break;case 162: _LL2CF:{struct Cyc_Parse_Declarator*_tmpBA4;yyval=Cyc_YY29(((
_tmpBA4=_cycalloc(sizeof(*_tmpBA4)),((_tmpBA4->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBA4->tms=0,_tmpBA4)))))));}
break;case 163: _LL2D0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 164: _LL2D1: {struct Cyc_Parse_Declarator*_tmp5DD=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpBAA;struct Cyc_Absyn_Attributes_mod_struct _tmpBA9;struct Cyc_List_List*_tmpBA8;
_tmp5DD->tms=((_tmpBA8=_cycalloc(sizeof(*_tmpBA8)),((_tmpBA8->hd=(void*)((
_tmpBAA=_cycalloc(sizeof(*_tmpBAA)),((_tmpBAA[0]=((_tmpBA9.tag=5,((_tmpBA9.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpBA9.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpBA9)))))),_tmpBAA)))),((
_tmpBA8->tl=_tmp5DD->tms,_tmpBA8))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 165: _LL2D2:{
struct Cyc_List_List*_tmpBB4;struct Cyc_Absyn_Carray_mod_struct _tmpBB3;struct Cyc_Absyn_Carray_mod_struct*
_tmpBB2;struct Cyc_Parse_Declarator*_tmpBB1;yyval=Cyc_YY29(((_tmpBB1=_cycalloc(
sizeof(*_tmpBB1)),((_tmpBB1->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBB1->tms=((
_tmpBB4=_cycalloc(sizeof(*_tmpBB4)),((_tmpBB4->hd=(void*)((_tmpBB2=_cycalloc(
sizeof(*_tmpBB2)),((_tmpBB2[0]=((_tmpBB3.tag=0,((_tmpBB3.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBB3.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBB3)))))),
_tmpBB2)))),((_tmpBB4->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpBB4)))))),_tmpBB1)))))));}break;case 166:
_LL2D3:{struct Cyc_List_List*_tmpBBE;struct Cyc_Absyn_ConstArray_mod_struct _tmpBBD;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpBBC;struct Cyc_Parse_Declarator*_tmpBBB;
yyval=Cyc_YY29(((_tmpBBB=_cycalloc(sizeof(*_tmpBBB)),((_tmpBBB->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpBBB->tms=((
_tmpBBE=_cycalloc(sizeof(*_tmpBBE)),((_tmpBBE->hd=(void*)((_tmpBBC=_cycalloc(
sizeof(*_tmpBBC)),((_tmpBBC[0]=((_tmpBBD.tag=1,((_tmpBBD.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBBD.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBBD.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBBD)))))))),
_tmpBBC)))),((_tmpBBE->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpBBE)))))),_tmpBBB)))))));}break;case 167:
_LL2D4: {struct _tuple23 _tmp5EA;struct Cyc_List_List*_tmp5EB;int _tmp5EC;struct Cyc_Absyn_VarargInfo*
_tmp5ED;struct Cyc_Core_Opt*_tmp5EE;struct Cyc_List_List*_tmp5EF;struct _tuple23*
_tmp5E9=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5EA=*_tmp5E9;_tmp5EB=_tmp5EA.f1;_tmp5EC=_tmp5EA.f2;_tmp5ED=_tmp5EA.f3;
_tmp5EE=_tmp5EA.f4;_tmp5EF=_tmp5EA.f5;{struct Cyc_List_List*_tmpBD3;struct Cyc_Absyn_Function_mod_struct
_tmpBD2;struct Cyc_Absyn_WithTypes_struct _tmpBD1;struct Cyc_Absyn_WithTypes_struct*
_tmpBD0;struct Cyc_Absyn_Function_mod_struct*_tmpBCF;struct Cyc_Parse_Declarator*
_tmpBCE;yyval=Cyc_YY29(((_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBCE->tms=((
_tmpBD3=_cycalloc(sizeof(*_tmpBD3)),((_tmpBD3->hd=(void*)((_tmpBCF=_cycalloc(
sizeof(*_tmpBCF)),((_tmpBCF[0]=((_tmpBD2.tag=3,((_tmpBD2.f1=(void*)((void*)((
_tmpBD0=_cycalloc(sizeof(*_tmpBD0)),((_tmpBD0[0]=((_tmpBD1.tag=1,((_tmpBD1.f1=
_tmp5EB,((_tmpBD1.f2=_tmp5EC,((_tmpBD1.f3=_tmp5ED,((_tmpBD1.f4=_tmp5EE,((_tmpBD1.f5=
_tmp5EF,_tmpBD1)))))))))))),_tmpBD0))))),_tmpBD2)))),_tmpBCF)))),((_tmpBD3->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpBD3)))))),_tmpBCE)))))));}break;}case 168: _LL2D5:{struct Cyc_List_List*_tmpBE8;
struct Cyc_Absyn_Function_mod_struct _tmpBE7;struct Cyc_Absyn_WithTypes_struct
_tmpBE6;struct Cyc_Absyn_WithTypes_struct*_tmpBE5;struct Cyc_Absyn_Function_mod_struct*
_tmpBE4;struct Cyc_Parse_Declarator*_tmpBE3;yyval=Cyc_YY29(((_tmpBE3=_cycalloc(
sizeof(*_tmpBE3)),((_tmpBE3->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpBE3->tms=((
_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8->hd=(void*)((_tmpBE4=_cycalloc(
sizeof(*_tmpBE4)),((_tmpBE4[0]=((_tmpBE7.tag=3,((_tmpBE7.f1=(void*)((void*)((
_tmpBE5=_cycalloc(sizeof(*_tmpBE5)),((_tmpBE5[0]=((_tmpBE6.tag=1,((_tmpBE6.f1=0,((
_tmpBE6.f2=0,((_tmpBE6.f3=0,((_tmpBE6.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBE6.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBE6)))))))))))),
_tmpBE5))))),_tmpBE7)))),_tmpBE4)))),((_tmpBE8->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpBE8)))))),
_tmpBE3)))))));}break;case 169: _LL2D6:{struct Cyc_List_List*_tmpBFD;struct Cyc_Absyn_Function_mod_struct
_tmpBFC;struct Cyc_Absyn_NoTypes_struct _tmpBFB;struct Cyc_Absyn_NoTypes_struct*
_tmpBFA;struct Cyc_Absyn_Function_mod_struct*_tmpBF9;struct Cyc_Parse_Declarator*
_tmpBF8;yyval=Cyc_YY29(((_tmpBF8=_cycalloc(sizeof(*_tmpBF8)),((_tmpBF8->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBF8->tms=((
_tmpBFD=_cycalloc(sizeof(*_tmpBFD)),((_tmpBFD->hd=(void*)((_tmpBF9=_cycalloc(
sizeof(*_tmpBF9)),((_tmpBF9[0]=((_tmpBFC.tag=3,((_tmpBFC.f1=(void*)((void*)((
_tmpBFA=_cycalloc(sizeof(*_tmpBFA)),((_tmpBFA[0]=((_tmpBFB.tag=0,((_tmpBFB.f1=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpBFB.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpBFB)))))),_tmpBFA))))),_tmpBFC)))),_tmpBF9)))),((
_tmpBFD->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms,_tmpBFD)))))),_tmpBF8)))))));}break;case 170: _LL2D7: {
struct Cyc_List_List*_tmp602=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpC07;struct Cyc_Absyn_TypeParams_mod_struct _tmpC06;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpC05;struct Cyc_Parse_Declarator*_tmpC04;yyval=Cyc_YY29(((_tmpC04=_cycalloc(
sizeof(*_tmpC04)),((_tmpC04->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC04->tms=((
_tmpC07=_cycalloc(sizeof(*_tmpC07)),((_tmpC07->hd=(void*)((_tmpC05=_cycalloc(
sizeof(*_tmpC05)),((_tmpC05[0]=((_tmpC06.tag=4,((_tmpC06.f1=_tmp602,((_tmpC06.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpC06.f3=0,_tmpC06)))))))),_tmpC05)))),((_tmpC07->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC07)))))),_tmpC04)))))));}break;}case 171: _LL2D8:{struct Cyc_List_List*_tmpC11;
struct Cyc_Absyn_Attributes_mod_struct _tmpC10;struct Cyc_Absyn_Attributes_mod_struct*
_tmpC0F;struct Cyc_Parse_Declarator*_tmpC0E;yyval=Cyc_YY29(((_tmpC0E=_cycalloc(
sizeof(*_tmpC0E)),((_tmpC0E->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpC0E->tms=((
_tmpC11=_cycalloc(sizeof(*_tmpC11)),((_tmpC11->hd=(void*)((_tmpC0F=_cycalloc(
sizeof(*_tmpC0F)),((_tmpC0F[0]=((_tmpC10.tag=5,((_tmpC10.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC10.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC10)))))),_tmpC0F)))),((
_tmpC11->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpC11)))))),_tmpC0E)))))));}break;case 172: _LL2D9:{
struct Cyc_Parse_Declarator*_tmpC12;yyval=Cyc_YY29(((_tmpC12=_cycalloc(sizeof(*
_tmpC12)),((_tmpC12->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC12->tms=0,_tmpC12)))))));}break;case 173: _LL2DA:{
struct Cyc_Parse_Declarator*_tmpC13;yyval=Cyc_YY29(((_tmpC13=_cycalloc(sizeof(*
_tmpC13)),((_tmpC13->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC13->tms=0,_tmpC13)))))));}break;case 174: _LL2DB:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 175:
_LL2DC: {struct Cyc_Parse_Declarator*_tmp60D=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpC19;struct Cyc_Absyn_Attributes_mod_struct _tmpC18;struct Cyc_List_List*_tmpC17;
_tmp60D->tms=((_tmpC17=_cycalloc(sizeof(*_tmpC17)),((_tmpC17->hd=(void*)((
_tmpC19=_cycalloc(sizeof(*_tmpC19)),((_tmpC19[0]=((_tmpC18.tag=5,((_tmpC18.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpC18.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC18)))))),_tmpC19)))),((
_tmpC17->tl=_tmp60D->tms,_tmpC17))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 176: _LL2DD:{
struct Cyc_List_List*_tmpC23;struct Cyc_Absyn_Carray_mod_struct _tmpC22;struct Cyc_Absyn_Carray_mod_struct*
_tmpC21;struct Cyc_Parse_Declarator*_tmpC20;yyval=Cyc_YY29(((_tmpC20=_cycalloc(
sizeof(*_tmpC20)),((_tmpC20->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC20->tms=((
_tmpC23=_cycalloc(sizeof(*_tmpC23)),((_tmpC23->hd=(void*)((_tmpC21=_cycalloc(
sizeof(*_tmpC21)),((_tmpC21[0]=((_tmpC22.tag=0,((_tmpC22.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC22.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC22)))))),
_tmpC21)))),((_tmpC23->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpC23)))))),_tmpC20)))))));}break;case 177:
_LL2DE:{struct Cyc_List_List*_tmpC2D;struct Cyc_Absyn_ConstArray_mod_struct _tmpC2C;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpC2B;struct Cyc_Parse_Declarator*_tmpC2A;
yyval=Cyc_YY29(((_tmpC2A=_cycalloc(sizeof(*_tmpC2A)),((_tmpC2A->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpC2A->tms=((
_tmpC2D=_cycalloc(sizeof(*_tmpC2D)),((_tmpC2D->hd=(void*)((_tmpC2B=_cycalloc(
sizeof(*_tmpC2B)),((_tmpC2B[0]=((_tmpC2C.tag=1,((_tmpC2C.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC2C.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC2C.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC2C)))))))),
_tmpC2B)))),((_tmpC2D->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpC2D)))))),_tmpC2A)))))));}break;case 178:
_LL2DF: {struct _tuple23 _tmp61A;struct Cyc_List_List*_tmp61B;int _tmp61C;struct Cyc_Absyn_VarargInfo*
_tmp61D;struct Cyc_Core_Opt*_tmp61E;struct Cyc_List_List*_tmp61F;struct _tuple23*
_tmp619=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp61A=*_tmp619;_tmp61B=_tmp61A.f1;_tmp61C=_tmp61A.f2;_tmp61D=_tmp61A.f3;
_tmp61E=_tmp61A.f4;_tmp61F=_tmp61A.f5;{struct Cyc_List_List*_tmpC42;struct Cyc_Absyn_Function_mod_struct
_tmpC41;struct Cyc_Absyn_WithTypes_struct _tmpC40;struct Cyc_Absyn_WithTypes_struct*
_tmpC3F;struct Cyc_Absyn_Function_mod_struct*_tmpC3E;struct Cyc_Parse_Declarator*
_tmpC3D;yyval=Cyc_YY29(((_tmpC3D=_cycalloc(sizeof(*_tmpC3D)),((_tmpC3D->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC3D->tms=((
_tmpC42=_cycalloc(sizeof(*_tmpC42)),((_tmpC42->hd=(void*)((_tmpC3E=_cycalloc(
sizeof(*_tmpC3E)),((_tmpC3E[0]=((_tmpC41.tag=3,((_tmpC41.f1=(void*)((void*)((
_tmpC3F=_cycalloc(sizeof(*_tmpC3F)),((_tmpC3F[0]=((_tmpC40.tag=1,((_tmpC40.f1=
_tmp61B,((_tmpC40.f2=_tmp61C,((_tmpC40.f3=_tmp61D,((_tmpC40.f4=_tmp61E,((_tmpC40.f5=
_tmp61F,_tmpC40)))))))))))),_tmpC3F))))),_tmpC41)))),_tmpC3E)))),((_tmpC42->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC42)))))),_tmpC3D)))))));}break;}case 179: _LL2E0:{struct Cyc_List_List*_tmpC57;
struct Cyc_Absyn_Function_mod_struct _tmpC56;struct Cyc_Absyn_WithTypes_struct
_tmpC55;struct Cyc_Absyn_WithTypes_struct*_tmpC54;struct Cyc_Absyn_Function_mod_struct*
_tmpC53;struct Cyc_Parse_Declarator*_tmpC52;yyval=Cyc_YY29(((_tmpC52=_cycalloc(
sizeof(*_tmpC52)),((_tmpC52->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpC52->tms=((
_tmpC57=_cycalloc(sizeof(*_tmpC57)),((_tmpC57->hd=(void*)((_tmpC53=_cycalloc(
sizeof(*_tmpC53)),((_tmpC53[0]=((_tmpC56.tag=3,((_tmpC56.f1=(void*)((void*)((
_tmpC54=_cycalloc(sizeof(*_tmpC54)),((_tmpC54[0]=((_tmpC55.tag=1,((_tmpC55.f1=0,((
_tmpC55.f2=0,((_tmpC55.f3=0,((_tmpC55.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC55.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC55)))))))))))),
_tmpC54))))),_tmpC56)))),_tmpC53)))),((_tmpC57->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpC57)))))),
_tmpC52)))))));}break;case 180: _LL2E1:{struct Cyc_List_List*_tmpC6C;struct Cyc_Absyn_Function_mod_struct
_tmpC6B;struct Cyc_Absyn_NoTypes_struct _tmpC6A;struct Cyc_Absyn_NoTypes_struct*
_tmpC69;struct Cyc_Absyn_Function_mod_struct*_tmpC68;struct Cyc_Parse_Declarator*
_tmpC67;yyval=Cyc_YY29(((_tmpC67=_cycalloc(sizeof(*_tmpC67)),((_tmpC67->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC67->tms=((
_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C->hd=(void*)((_tmpC68=_cycalloc(
sizeof(*_tmpC68)),((_tmpC68[0]=((_tmpC6B.tag=3,((_tmpC6B.f1=(void*)((void*)((
_tmpC69=_cycalloc(sizeof(*_tmpC69)),((_tmpC69[0]=((_tmpC6A.tag=0,((_tmpC6A.f1=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpC6A.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpC6A)))))),_tmpC69))))),_tmpC6B)))),_tmpC68)))),((
_tmpC6C->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms,_tmpC6C)))))),_tmpC67)))))));}break;case 181: _LL2E2: {
struct Cyc_List_List*_tmp632=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpC76;struct Cyc_Absyn_TypeParams_mod_struct _tmpC75;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpC74;struct Cyc_Parse_Declarator*_tmpC73;yyval=Cyc_YY29(((_tmpC73=_cycalloc(
sizeof(*_tmpC73)),((_tmpC73->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC73->tms=((
_tmpC76=_cycalloc(sizeof(*_tmpC76)),((_tmpC76->hd=(void*)((_tmpC74=_cycalloc(
sizeof(*_tmpC74)),((_tmpC74[0]=((_tmpC75.tag=4,((_tmpC75.f1=_tmp632,((_tmpC75.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpC75.f3=0,_tmpC75)))))))),_tmpC74)))),((_tmpC76->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC76)))))),_tmpC73)))))));}break;}case 182: _LL2E3:{struct Cyc_List_List*_tmpC80;
struct Cyc_Absyn_Attributes_mod_struct _tmpC7F;struct Cyc_Absyn_Attributes_mod_struct*
_tmpC7E;struct Cyc_Parse_Declarator*_tmpC7D;yyval=Cyc_YY29(((_tmpC7D=_cycalloc(
sizeof(*_tmpC7D)),((_tmpC7D->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpC7D->tms=((
_tmpC80=_cycalloc(sizeof(*_tmpC80)),((_tmpC80->hd=(void*)((_tmpC7E=_cycalloc(
sizeof(*_tmpC7E)),((_tmpC7E[0]=((_tmpC7F.tag=5,((_tmpC7F.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC7F.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC7F)))))),_tmpC7E)))),((
_tmpC80->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpC80)))))),_tmpC7D)))))));}break;case 183: _LL2E4:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 184:
_LL2E5: yyval=Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 185: _LL2E6: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){struct Cyc_Absyn_Attributes_mod_struct*
_tmpC86;struct Cyc_Absyn_Attributes_mod_struct _tmpC85;struct Cyc_List_List*_tmpC84;
ans=((_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84->hd=(void*)((_tmpC86=
_cycalloc(sizeof(*_tmpC86)),((_tmpC86[0]=((_tmpC85.tag=5,((_tmpC85.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpC85.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpC85)))))),_tmpC86)))),((_tmpC84->tl=ans,_tmpC84))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;struct Cyc_Position_Segment*_tmp63F;union Cyc_Absyn_Constraint*_tmp640;
union Cyc_Absyn_Constraint*_tmp641;struct _tuple18 _tmp63E=*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp63F=_tmp63E.f1;
_tmp640=_tmp63E.f2;_tmp641=_tmp63E.f3;if(Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*
_tmpC87;ptrloc=((_tmpC87=_cycalloc(sizeof(*_tmpC87)),((_tmpC87->ptr_loc=_tmp63F,((
_tmpC87->rgn_loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpC87->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),_tmpC87))))))));}{union Cyc_Absyn_Constraint*
_tmp644;union Cyc_Absyn_Constraint*_tmp645;union Cyc_Absyn_Constraint*_tmp646;void*
_tmp647;struct _tuple11 _tmp643=Cyc_Parse_collapse_pointer_quals(_tmp63F,_tmp640,
_tmp641,Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY54(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
_tmp644=_tmp643.f1;_tmp645=_tmp643.f2;_tmp646=_tmp643.f3;_tmp647=_tmp643.f4;{
struct Cyc_Absyn_Pointer_mod_struct*_tmpC91;struct Cyc_Absyn_PtrAtts _tmpC90;struct
Cyc_Absyn_Pointer_mod_struct _tmpC8F;struct Cyc_List_List*_tmpC8E;ans=((_tmpC8E=
_cycalloc(sizeof(*_tmpC8E)),((_tmpC8E->hd=(void*)((_tmpC91=_cycalloc(sizeof(*
_tmpC91)),((_tmpC91[0]=((_tmpC8F.tag=2,((_tmpC8F.f1=((_tmpC90.rgn=_tmp647,((
_tmpC90.nullable=_tmp644,((_tmpC90.bounds=_tmp645,((_tmpC90.zero_term=_tmp646,((
_tmpC90.ptrloc=ptrloc,_tmpC90)))))))))),((_tmpC8F.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC8F)))))),_tmpC91)))),((
_tmpC8E->tl=ans,_tmpC8E))))));}yyval=Cyc_YY28(ans);break;};};}case 186: _LL2E7:
yyval=Cyc_YY54(0);break;case 187: _LL2E8:{struct Cyc_List_List*_tmpC92;yyval=Cyc_YY54(((
_tmpC92=_cycalloc(sizeof(*_tmpC92)),((_tmpC92->hd=Cyc_yyget_YY53(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC92->tl=Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC92)))))));}break;
case 188: _LL2E9:{struct Cyc_Absyn_Numelts_ptrqual_struct _tmpC95;struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmpC94;yyval=Cyc_YY53((void*)((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94[0]=((
_tmpC95.tag=0,((_tmpC95.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpC95)))),_tmpC94)))));}break;case 189: _LL2EA:{struct
Cyc_Absyn_Region_ptrqual_struct _tmpC98;struct Cyc_Absyn_Region_ptrqual_struct*
_tmpC97;yyval=Cyc_YY53((void*)((_tmpC97=_cycalloc(sizeof(*_tmpC97)),((_tmpC97[0]=((
_tmpC98.tag=1,((_tmpC98.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC98)))),_tmpC97)))));}
break;case 190: _LL2EB:{struct Cyc_Absyn_Thin_ptrqual_struct _tmpC9B;struct Cyc_Absyn_Thin_ptrqual_struct*
_tmpC9A;yyval=Cyc_YY53((void*)((_tmpC9A=_cycalloc_atomic(sizeof(*_tmpC9A)),((
_tmpC9A[0]=((_tmpC9B.tag=2,_tmpC9B)),_tmpC9A)))));}break;case 191: _LL2EC:{struct
Cyc_Absyn_Fat_ptrqual_struct _tmpC9E;struct Cyc_Absyn_Fat_ptrqual_struct*_tmpC9D;
yyval=Cyc_YY53((void*)((_tmpC9D=_cycalloc_atomic(sizeof(*_tmpC9D)),((_tmpC9D[0]=((
_tmpC9E.tag=3,_tmpC9E)),_tmpC9D)))));}break;case 192: _LL2ED:{struct Cyc_Absyn_Zeroterm_ptrqual_struct
_tmpCA1;struct Cyc_Absyn_Zeroterm_ptrqual_struct*_tmpCA0;yyval=Cyc_YY53((void*)((
_tmpCA0=_cycalloc_atomic(sizeof(*_tmpCA0)),((_tmpCA0[0]=((_tmpCA1.tag=4,_tmpCA1)),
_tmpCA0)))));}break;case 193: _LL2EE:{struct Cyc_Absyn_Nozeroterm_ptrqual_struct
_tmpCA4;struct Cyc_Absyn_Nozeroterm_ptrqual_struct*_tmpCA3;yyval=Cyc_YY53((void*)((
_tmpCA3=_cycalloc_atomic(sizeof(*_tmpCA3)),((_tmpCA3[0]=((_tmpCA4.tag=5,_tmpCA4)),
_tmpCA3)))));}break;case 194: _LL2EF:{struct Cyc_Absyn_Notnull_ptrqual_struct
_tmpCA7;struct Cyc_Absyn_Notnull_ptrqual_struct*_tmpCA6;yyval=Cyc_YY53((void*)((
_tmpCA6=_cycalloc_atomic(sizeof(*_tmpCA6)),((_tmpCA6[0]=((_tmpCA7.tag=6,_tmpCA7)),
_tmpCA6)))));}break;case 195: _LL2F0:{struct Cyc_Absyn_Nullable_ptrqual_struct
_tmpCAA;struct Cyc_Absyn_Nullable_ptrqual_struct*_tmpCA9;yyval=Cyc_YY53((void*)((
_tmpCA9=_cycalloc_atomic(sizeof(*_tmpCA9)),((_tmpCA9[0]=((_tmpCAA.tag=7,_tmpCAA)),
_tmpCA9)))));}break;case 196: _LL2F1: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple18*_tmpCAB;yyval=Cyc_YY1(((_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->f1=
loc,((_tmpCAB->f2=Cyc_Absyn_true_conref,((_tmpCAB->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCAB)))))))));}break;}
case 197: _LL2F2: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple18*_tmpCAC;yyval=Cyc_YY1(((_tmpCAC=_cycalloc(sizeof(*_tmpCAC)),((_tmpCAC->f1=
loc,((_tmpCAC->f2=Cyc_Absyn_false_conref,((_tmpCAC->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCAC)))))))));}break;}
case 198: _LL2F3: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{struct _tuple18*
_tmpCAD;yyval=Cyc_YY1(((_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD->f1=loc,((
_tmpCAD->f2=Cyc_Absyn_true_conref,((_tmpCAD->f3=Cyc_Absyn_bounds_dyneither_conref,
_tmpCAD)))))))));}break;}case 199: _LL2F4: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 200: _LL2F5:{struct Cyc_Absyn_Upper_b_struct _tmpCB0;struct Cyc_Absyn_Upper_b_struct*
_tmpCAF;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmpCAF=_cycalloc(sizeof(*_tmpCAF)),((_tmpCAF[0]=((_tmpCB0.tag=1,((
_tmpCB0.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpCB0)))),_tmpCAF))))));}break;case 201: _LL2F6: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())
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
struct _tuple23*_tmpCB1;yyval=Cyc_YY39(((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((
_tmpCB1->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((
_tmpCB1->f2=0,((_tmpCB1->f3=0,((_tmpCB1->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCB1->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCB1)))))))))))));}
break;case 210: _LL2FF:{struct _tuple23*_tmpCB2;yyval=Cyc_YY39(((_tmpCB2=_cycalloc(
sizeof(*_tmpCB2)),((_tmpCB2->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),((_tmpCB2->f2=1,((_tmpCB2->f3=0,((_tmpCB2->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCB2->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCB2)))))))))))));}
break;case 211: _LL300: {struct _tuple8 _tmp665;struct Cyc_Core_Opt*_tmp666;struct Cyc_Absyn_Tqual
_tmp667;void*_tmp668;struct _tuple8*_tmp664=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp665=*_tmp664;_tmp666=
_tmp665.f1;_tmp667=_tmp665.f2;_tmp668=_tmp665.f3;{struct Cyc_Absyn_VarargInfo*
_tmpCB3;struct Cyc_Absyn_VarargInfo*_tmp669=(_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((
_tmpCB3->name=_tmp666,((_tmpCB3->tq=_tmp667,((_tmpCB3->type=_tmp668,((_tmpCB3->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpCB3)))))))));{struct _tuple23*_tmpCB4;yyval=Cyc_YY39(((_tmpCB4=_cycalloc(
sizeof(*_tmpCB4)),((_tmpCB4->f1=0,((_tmpCB4->f2=0,((_tmpCB4->f3=_tmp669,((
_tmpCB4->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpCB4->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCB4)))))))))))));}
break;};}case 212: _LL301: {struct _tuple8 _tmp66D;struct Cyc_Core_Opt*_tmp66E;struct
Cyc_Absyn_Tqual _tmp66F;void*_tmp670;struct _tuple8*_tmp66C=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp66D=*_tmp66C;_tmp66E=
_tmp66D.f1;_tmp66F=_tmp66D.f2;_tmp670=_tmp66D.f3;{struct Cyc_Absyn_VarargInfo*
_tmpCB5;struct Cyc_Absyn_VarargInfo*_tmp671=(_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((
_tmpCB5->name=_tmp66E,((_tmpCB5->tq=_tmp66F,((_tmpCB5->type=_tmp670,((_tmpCB5->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpCB5)))))))));{struct _tuple23*_tmpCB6;yyval=Cyc_YY39(((_tmpCB6=_cycalloc(
sizeof(*_tmpCB6)),((_tmpCB6->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)])),((_tmpCB6->f2=0,((_tmpCB6->f3=_tmp671,((_tmpCB6->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCB6->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCB6)))))))))))));}
break;};}case 213: _LL302:{struct Cyc_Absyn_Unknown_kb_struct _tmpCB9;struct Cyc_Absyn_Unknown_kb_struct*
_tmpCB8;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)((_tmpCB8=_cycalloc(
sizeof(*_tmpCB8)),((_tmpCB8[0]=((_tmpCB9.tag=1,((_tmpCB9.f1=0,_tmpCB9)))),
_tmpCB8))))));}break;case 214: _LL303: yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))));
break;case 215: _LL304: yyval=Cyc_YY49(0);break;case 216: _LL305:{struct Cyc_Absyn_JoinEff_struct*
_tmpCBF;struct Cyc_Absyn_JoinEff_struct _tmpCBE;struct Cyc_Core_Opt*_tmpCBD;yyval=
Cyc_YY49(((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD->v=(void*)((_tmpCBF=
_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF[0]=((_tmpCBE.tag=24,((_tmpCBE.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCBE)))),_tmpCBF)))),
_tmpCBD)))));}break;case 217: _LL306: yyval=Cyc_YY50(0);break;case 218: _LL307: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 219: _LL308: {
struct Cyc_Absyn_Less_kb_struct _tmpCC2;struct Cyc_Absyn_Less_kb_struct*_tmpCC1;
struct Cyc_Absyn_Less_kb_struct*_tmp679=(_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((
_tmpCC1[0]=((_tmpCC2.tag=2,((_tmpCC2.f1=0,((_tmpCC2.f2=& Cyc_Tcutil_trk,_tmpCC2)))))),
_tmpCC1)));struct _dyneither_ptr _tmp67A=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp67B=Cyc_Parse_id2type(
_tmp67A,(void*)_tmp679);{struct _tuple26*_tmpCCC;struct Cyc_Absyn_JoinEff_struct
_tmpCCB;struct Cyc_Absyn_JoinEff_struct*_tmpCCA;struct Cyc_List_List*_tmpCC9;yyval=
Cyc_YY50(((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9->hd=((_tmpCCC=_cycalloc(
sizeof(*_tmpCCC)),((_tmpCCC->f1=(void*)((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((
_tmpCCA[0]=((_tmpCCB.tag=24,((_tmpCCB.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCCB)))),_tmpCCA)))),((
_tmpCCC->f2=_tmp67B,_tmpCCC)))))),((_tmpCC9->tl=0,_tmpCC9)))))));}break;}case 220:
_LL309: {struct Cyc_Absyn_Less_kb_struct _tmpCCF;struct Cyc_Absyn_Less_kb_struct*
_tmpCCE;struct Cyc_Absyn_Less_kb_struct*_tmp682=(_tmpCCE=_cycalloc(sizeof(*
_tmpCCE)),((_tmpCCE[0]=((_tmpCCF.tag=2,((_tmpCCF.f1=0,((_tmpCCF.f2=& Cyc_Tcutil_trk,
_tmpCCF)))))),_tmpCCE)));struct _dyneither_ptr _tmp683=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp684=Cyc_Parse_id2type(
_tmp683,(void*)_tmp682);{struct _tuple26*_tmpCD9;struct Cyc_Absyn_JoinEff_struct
_tmpCD8;struct Cyc_Absyn_JoinEff_struct*_tmpCD7;struct Cyc_List_List*_tmpCD6;yyval=
Cyc_YY50(((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->hd=((_tmpCD9=_cycalloc(
sizeof(*_tmpCD9)),((_tmpCD9->f1=(void*)((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((
_tmpCD7[0]=((_tmpCD8.tag=24,((_tmpCD8.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmpCD8)))),_tmpCD7)))),((
_tmpCD9->f2=_tmp684,_tmpCD9)))))),((_tmpCD6->tl=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCD6)))))));}break;}case
221: _LL30A: yyval=Cyc_YY31(0);break;case 222: _LL30B:{const char*_tmpCDA;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpCDA="inject",_tag_dyneither(_tmpCDA,sizeof(char),7))))
!= 0){const char*_tmpCDB;Cyc_Parse_err(((_tmpCDB="missing type in function declaration",
_tag_dyneither(_tmpCDB,sizeof(char),37))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY31(
1);break;case 223: _LL30C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 224: _LL30D: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 225: _LL30E:
yyval=Cyc_YY40(0);break;case 226: _LL30F: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 227: _LL310:{struct Cyc_Absyn_RgnsEff_struct*
_tmpCE1;struct Cyc_Absyn_RgnsEff_struct _tmpCE0;struct Cyc_List_List*_tmpCDF;yyval=
Cyc_YY40(((_tmpCDF=_cycalloc(sizeof(*_tmpCDF)),((_tmpCDF->hd=(void*)((_tmpCE1=
_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1[0]=((_tmpCE0.tag=25,((_tmpCE0.f1=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpCE0)))),_tmpCE1)))),((_tmpCDF->tl=0,_tmpCDF)))))));}break;case 228: _LL311: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_ek,
0);{struct Cyc_List_List*_tmpCE2;yyval=Cyc_YY40(((_tmpCE2=_cycalloc(sizeof(*
_tmpCE2)),((_tmpCE2->hd=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpCE2->tl=0,_tmpCE2)))))));}break;case 229: _LL312: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),& Cyc_Tcutil_trk,
1);{struct Cyc_Absyn_AccessEff_struct*_tmpCE8;struct Cyc_Absyn_AccessEff_struct
_tmpCE7;struct Cyc_List_List*_tmpCE6;yyval=Cyc_YY40(((_tmpCE6=_cycalloc(sizeof(*
_tmpCE6)),((_tmpCE6->hd=(void*)((_tmpCE8=_cycalloc(sizeof(*_tmpCE8)),((_tmpCE8[0]=((
_tmpCE7.tag=23,((_tmpCE7.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCE7)))),_tmpCE8)))),((
_tmpCE6->tl=0,_tmpCE6)))))));}break;case 230: _LL313: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),& Cyc_Tcutil_trk,1);{
struct Cyc_Absyn_AccessEff_struct*_tmpCEE;struct Cyc_Absyn_AccessEff_struct _tmpCED;
struct Cyc_List_List*_tmpCEC;yyval=Cyc_YY40(((_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((
_tmpCEC->hd=(void*)((_tmpCEE=_cycalloc(sizeof(*_tmpCEE)),((_tmpCEE[0]=((_tmpCED.tag=
23,((_tmpCED.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),_tmpCED)))),_tmpCEE)))),((_tmpCEC->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCEC)))))));}break;case
231: _LL314:{struct Cyc_List_List*_tmpCEF;yyval=Cyc_YY38(((_tmpCEF=_cycalloc(
sizeof(*_tmpCEF)),((_tmpCEF->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCEF->tl=0,_tmpCEF)))))));}
break;case 232: _LL315:{struct Cyc_List_List*_tmpCF0;yyval=Cyc_YY38(((_tmpCF0=
_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCF0->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCF0)))))));}
break;case 233: _LL316: {struct _tuple22 _tmp69A;struct Cyc_Absyn_Tqual _tmp69B;struct
Cyc_List_List*_tmp69C;struct Cyc_List_List*_tmp69D;struct _tuple22*_tmp699=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp69A=*_tmp699;
_tmp69B=_tmp69A.f1;_tmp69C=_tmp69A.f2;_tmp69D=_tmp69A.f3;if(_tmp69B.loc == 0)
_tmp69B.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp69F;struct _tuple1*
_tmp6A0;struct Cyc_List_List*_tmp6A1;struct Cyc_Parse_Declarator*_tmp69E=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp69F=*_tmp69E;
_tmp6A0=_tmp69F.id;_tmp6A1=_tmp69F.tms;{void*_tmp6A2=Cyc_Parse_speclist2typ(
_tmp69C,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp6A4;void*_tmp6A5;struct
Cyc_List_List*_tmp6A6;struct Cyc_List_List*_tmp6A7;struct _tuple10 _tmp6A3=Cyc_Parse_apply_tms(
_tmp69B,_tmp6A2,_tmp69D,_tmp6A1);_tmp6A4=_tmp6A3.f1;_tmp6A5=_tmp6A3.f2;_tmp6A6=
_tmp6A3.f3;_tmp6A7=_tmp6A3.f4;if(_tmp6A6 != 0){const char*_tmpCF1;Cyc_Parse_err(((
_tmpCF1="parameter with bad type params",_tag_dyneither(_tmpCF1,sizeof(char),31))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp6A0)){const char*
_tmpCF2;Cyc_Parse_err(((_tmpCF2="parameter cannot be qualified with a namespace",
_tag_dyneither(_tmpCF2,sizeof(char),47))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*
_tmpCF3;struct Cyc_Core_Opt*_tmp6AA=(struct Cyc_Core_Opt*)((_tmpCF3=_cycalloc(
sizeof(*_tmpCF3)),((_tmpCF3->v=(*_tmp6A0).f2,_tmpCF3))));if(_tmp6A7 != 0){const
char*_tmpCF6;void*_tmpCF5;(_tmpCF5=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpCF6="extra attributes on parameter, ignoring",
_tag_dyneither(_tmpCF6,sizeof(char),40))),_tag_dyneither(_tmpCF5,sizeof(void*),0)));}{
struct _tuple8*_tmpCF7;yyval=Cyc_YY37(((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((
_tmpCF7->f1=_tmp6AA,((_tmpCF7->f2=_tmp6A4,((_tmpCF7->f3=_tmp6A5,_tmpCF7)))))))));}
break;};};};}case 234: _LL317: {struct _tuple22 _tmp6B0;struct Cyc_Absyn_Tqual _tmp6B1;
struct Cyc_List_List*_tmp6B2;struct Cyc_List_List*_tmp6B3;struct _tuple22*_tmp6AF=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B0=*
_tmp6AF;_tmp6B1=_tmp6B0.f1;_tmp6B2=_tmp6B0.f2;_tmp6B3=_tmp6B0.f3;if(_tmp6B1.loc
== 0)_tmp6B1.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{void*_tmp6B4=
Cyc_Parse_speclist2typ(_tmp6B2,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6B3 != 0){
const char*_tmpCFA;void*_tmpCF9;(_tmpCF9=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpCFA="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpCFA,sizeof(char),38))),_tag_dyneither(_tmpCF9,sizeof(void*),0)));}{
struct _tuple8*_tmpCFB;yyval=Cyc_YY37(((_tmpCFB=_cycalloc(sizeof(*_tmpCFB)),((
_tmpCFB->f1=0,((_tmpCFB->f2=_tmp6B1,((_tmpCFB->f3=_tmp6B4,_tmpCFB)))))))));}
break;};}case 235: _LL318: {struct _tuple22 _tmp6B9;struct Cyc_Absyn_Tqual _tmp6BA;
struct Cyc_List_List*_tmp6BB;struct Cyc_List_List*_tmp6BC;struct _tuple22*_tmp6B8=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6B9=*_tmp6B8;_tmp6BA=_tmp6B9.f1;_tmp6BB=_tmp6B9.f2;_tmp6BC=_tmp6B9.f3;if(
_tmp6BA.loc == 0)_tmp6BA.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{void*_tmp6BD=
Cyc_Parse_speclist2typ(_tmp6BB,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6BE=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp6C0;void*_tmp6C1;struct Cyc_List_List*_tmp6C2;struct Cyc_List_List*
_tmp6C3;struct _tuple10 _tmp6BF=Cyc_Parse_apply_tms(_tmp6BA,_tmp6BD,_tmp6BC,
_tmp6BE);_tmp6C0=_tmp6BF.f1;_tmp6C1=_tmp6BF.f2;_tmp6C2=_tmp6BF.f3;_tmp6C3=
_tmp6BF.f4;if(_tmp6C2 != 0){const char*_tmpCFE;void*_tmpCFD;(_tmpCFD=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpCFE="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpCFE,sizeof(char),49))),_tag_dyneither(_tmpCFD,sizeof(void*),0)));}
if(_tmp6C3 != 0){const char*_tmpD01;void*_tmpD00;(_tmpD00=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD01="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpD01,sizeof(char),38))),_tag_dyneither(_tmpD00,sizeof(void*),0)));}{
struct _tuple8*_tmpD02;yyval=Cyc_YY37(((_tmpD02=_cycalloc(sizeof(*_tmpD02)),((
_tmpD02->f1=0,((_tmpD02->f2=_tmp6C0,((_tmpD02->f3=_tmp6C1,_tmpD02)))))))));}
break;};}case 236: _LL319: yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 237: _LL31A:{struct _dyneither_ptr*_tmpD05;struct Cyc_List_List*
_tmpD04;yyval=Cyc_YY36(((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04->hd=((
_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD05)))),((_tmpD04->tl=0,
_tmpD04)))))));}break;case 238: _LL31B:{struct _dyneither_ptr*_tmpD08;struct Cyc_List_List*
_tmpD07;yyval=Cyc_YY36(((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07->hd=((
_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD08)))),((_tmpD07->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpD07)))))));}break;case 239: _LL31C: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 240: _LL31D: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 241: _LL31E:{struct
Cyc_Absyn_UnresolvedMem_e_struct _tmpD0B;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpD0A;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD0A=_cycalloc(sizeof(*
_tmpD0A)),((_tmpD0A[0]=((_tmpD0B.tag=36,((_tmpD0B.f1=0,((_tmpD0B.f2=0,_tmpD0B)))))),
_tmpD0A)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 242: _LL31F:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD0E;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD0D;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD0D=_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D[0]=((_tmpD0E.tag=36,((
_tmpD0E.f1=0,((_tmpD0E.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),
_tmpD0E)))))),_tmpD0D)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 243:
_LL320:{struct Cyc_Absyn_UnresolvedMem_e_struct _tmpD11;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpD10;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD10=_cycalloc(sizeof(*
_tmpD10)),((_tmpD10[0]=((_tmpD11.tag=36,((_tmpD11.f1=0,((_tmpD11.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),_tmpD11)))))),_tmpD10)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 244: _LL321: {struct _dyneither_ptr*
_tmpD14;struct _tuple1*_tmpD13;struct Cyc_Absyn_Vardecl*_tmp6D3=Cyc_Absyn_new_vardecl(((
_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13->f1=Cyc_Absyn_Loc_n,((_tmpD13->f2=((
_tmpD14=_cycalloc(sizeof(*_tmpD14)),((_tmpD14[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpD14)))),_tmpD13)))))),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp6D3->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct _tmpD17;struct Cyc_Absyn_Comprehension_e_struct*
_tmpD16;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD16=_cycalloc(sizeof(*
_tmpD16)),((_tmpD16[0]=((_tmpD17.tag=28,((_tmpD17.f1=_tmp6D3,((_tmpD17.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpD17.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD17.f4=0,
_tmpD17)))))))))),_tmpD16)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
245: _LL322:{struct _tuple27*_tmpD1A;struct Cyc_List_List*_tmpD19;yyval=Cyc_YY6(((
_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19->hd=((_tmpD1A=_cycalloc(sizeof(*
_tmpD1A)),((_tmpD1A->f1=0,((_tmpD1A->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD1A)))))),((_tmpD19->tl=
0,_tmpD19)))))));}break;case 246: _LL323:{struct _tuple27*_tmpD1D;struct Cyc_List_List*
_tmpD1C;yyval=Cyc_YY6(((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C->hd=((
_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD1D->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD1D)))))),((
_tmpD1C->tl=0,_tmpD1C)))))));}break;case 247: _LL324:{struct _tuple27*_tmpD20;
struct Cyc_List_List*_tmpD1F;yyval=Cyc_YY6(((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((
_tmpD1F->hd=((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->f1=0,((_tmpD20->f2=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD20)))))),((
_tmpD1F->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),_tmpD1F)))))));}break;case 248: _LL325:{struct _tuple27*_tmpD23;struct Cyc_List_List*
_tmpD22;yyval=Cyc_YY6(((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22->hd=((
_tmpD23=_cycalloc(sizeof(*_tmpD23)),((_tmpD23->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD23->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD23)))))),((
_tmpD22->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]),_tmpD22)))))));}break;case 249: _LL326: yyval=Cyc_YY41(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 250: _LL327:{
struct Cyc_List_List*_tmpD24;yyval=Cyc_YY41(((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((
_tmpD24->hd=Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD24->tl=0,_tmpD24)))))));}break;case 251: _LL328:{struct Cyc_List_List*
_tmpD25;yyval=Cyc_YY41(((_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25->hd=Cyc_yyget_YY42(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD25->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD25)))))));}
break;case 252: _LL329:{struct Cyc_Absyn_ArrayElement_struct _tmpD28;struct Cyc_Absyn_ArrayElement_struct*
_tmpD27;yyval=Cyc_YY42((void*)((_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27[0]=((
_tmpD28.tag=0,((_tmpD28.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpD28)))),_tmpD27)))));}break;case 253: _LL32A:{struct
Cyc_Absyn_FieldName_struct _tmpD2E;struct _dyneither_ptr*_tmpD2D;struct Cyc_Absyn_FieldName_struct*
_tmpD2C;yyval=Cyc_YY42((void*)((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C[0]=((
_tmpD2E.tag=1,((_tmpD2E.f1=((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpD2D)))),_tmpD2E)))),_tmpD2C)))));}break;case 254: _LL32B: {struct _tuple22
_tmp6E8;struct Cyc_Absyn_Tqual _tmp6E9;struct Cyc_List_List*_tmp6EA;struct Cyc_List_List*
_tmp6EB;struct _tuple22*_tmp6E7=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E8=*_tmp6E7;_tmp6E9=
_tmp6E8.f1;_tmp6EA=_tmp6E8.f2;_tmp6EB=_tmp6E8.f3;{void*_tmp6EC=Cyc_Parse_speclist2typ(
_tmp6EA,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp6EB != 0){const char*_tmpD31;void*_tmpD30;(
_tmpD30=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD31="ignoring attributes in type",
_tag_dyneither(_tmpD31,sizeof(char),28))),_tag_dyneither(_tmpD30,sizeof(void*),0)));}{
struct _tuple8*_tmpD32;yyval=Cyc_YY37(((_tmpD32=_cycalloc(sizeof(*_tmpD32)),((
_tmpD32->f1=0,((_tmpD32->f2=_tmp6E9,((_tmpD32->f3=_tmp6EC,_tmpD32)))))))));}
break;};}case 255: _LL32C: {struct _tuple22 _tmp6F1;struct Cyc_Absyn_Tqual _tmp6F2;
struct Cyc_List_List*_tmp6F3;struct Cyc_List_List*_tmp6F4;struct _tuple22*_tmp6F0=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6F1=*_tmp6F0;_tmp6F2=_tmp6F1.f1;_tmp6F3=_tmp6F1.f2;_tmp6F4=_tmp6F1.f3;{void*
_tmp6F5=Cyc_Parse_speclist2typ(_tmp6F3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6F6=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple10 _tmp6F7=Cyc_Parse_apply_tms(_tmp6F2,_tmp6F5,_tmp6F4,_tmp6F6);if(
_tmp6F7.f3 != 0){const char*_tmpD35;void*_tmpD34;(_tmpD34=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD35="bad type params, ignoring",
_tag_dyneither(_tmpD35,sizeof(char),26))),_tag_dyneither(_tmpD34,sizeof(void*),0)));}
if(_tmp6F7.f4 != 0){const char*_tmpD38;void*_tmpD37;(_tmpD37=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD38="bad specifiers, ignoring",
_tag_dyneither(_tmpD38,sizeof(char),25))),_tag_dyneither(_tmpD37,sizeof(void*),0)));}{
struct _tuple8*_tmpD39;yyval=Cyc_YY37(((_tmpD39=_cycalloc(sizeof(*_tmpD39)),((
_tmpD39->f1=0,((_tmpD39->f2=_tmp6F7.f1,((_tmpD39->f3=_tmp6F7.f2,_tmpD39)))))))));}
break;};}case 256: _LL32D: yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 257:
_LL32E:{struct Cyc_Absyn_JoinEff_struct _tmpD3C;struct Cyc_Absyn_JoinEff_struct*
_tmpD3B;yyval=Cyc_YY44((void*)((_tmpD3B=_cycalloc(sizeof(*_tmpD3B)),((_tmpD3B[0]=((
_tmpD3C.tag=24,((_tmpD3C.f1=0,_tmpD3C)))),_tmpD3B)))));}break;case 258: _LL32F:{
struct Cyc_Absyn_JoinEff_struct _tmpD3F;struct Cyc_Absyn_JoinEff_struct*_tmpD3E;
yyval=Cyc_YY44((void*)((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E[0]=((
_tmpD3F.tag=24,((_tmpD3F.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpD3F)))),_tmpD3E)))));}break;case 259: _LL330:{struct
Cyc_Absyn_RgnsEff_struct _tmpD42;struct Cyc_Absyn_RgnsEff_struct*_tmpD41;yyval=Cyc_YY44((
void*)((_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41[0]=((_tmpD42.tag=25,((
_tmpD42.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpD42)))),_tmpD41)))));}break;case 260: _LL331:{struct Cyc_Absyn_JoinEff_struct
_tmpD48;struct Cyc_List_List*_tmpD47;struct Cyc_Absyn_JoinEff_struct*_tmpD46;yyval=
Cyc_YY44((void*)((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46[0]=((_tmpD48.tag=
24,((_tmpD48.f1=((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47->hd=Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD47->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD47)))))),_tmpD48)))),
_tmpD46)))));}break;case 261: _LL332:{struct Cyc_List_List*_tmpD49;yyval=Cyc_YY40(((
_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD49->tl=0,_tmpD49)))))));}
break;case 262: _LL333:{struct Cyc_List_List*_tmpD4A;yyval=Cyc_YY40(((_tmpD4A=
_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD4A->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD4A)))))));}
break;case 263: _LL334:{struct Cyc_Parse_Abstractdeclarator*_tmpD4B;yyval=Cyc_YY30(((
_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD4B)))));}break;case 264:
_LL335: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
265: _LL336:{struct Cyc_Parse_Abstractdeclarator*_tmpD4C;yyval=Cyc_YY30(((_tmpD4C=
_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpD4C)))));}break;
case 266: _LL337: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 267: _LL338:{struct Cyc_List_List*_tmpD56;struct Cyc_Absyn_Carray_mod_struct
_tmpD55;struct Cyc_Absyn_Carray_mod_struct*_tmpD54;struct Cyc_Parse_Abstractdeclarator*
_tmpD53;yyval=Cyc_YY30(((_tmpD53=_cycalloc(sizeof(*_tmpD53)),((_tmpD53->tms=((
_tmpD56=_cycalloc(sizeof(*_tmpD56)),((_tmpD56->hd=(void*)((_tmpD54=_cycalloc(
sizeof(*_tmpD54)),((_tmpD54[0]=((_tmpD55.tag=0,((_tmpD55.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD55.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD55)))))),
_tmpD54)))),((_tmpD56->tl=0,_tmpD56)))))),_tmpD53)))));}break;case 268: _LL339:{
struct Cyc_List_List*_tmpD60;struct Cyc_Absyn_Carray_mod_struct _tmpD5F;struct Cyc_Absyn_Carray_mod_struct*
_tmpD5E;struct Cyc_Parse_Abstractdeclarator*_tmpD5D;yyval=Cyc_YY30(((_tmpD5D=
_cycalloc(sizeof(*_tmpD5D)),((_tmpD5D->tms=((_tmpD60=_cycalloc(sizeof(*_tmpD60)),((
_tmpD60->hd=(void*)((_tmpD5E=_cycalloc(sizeof(*_tmpD5E)),((_tmpD5E[0]=((_tmpD5F.tag=
0,((_tmpD5F.f1=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD5F.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD5F)))))),
_tmpD5E)))),((_tmpD60->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD60)))))),_tmpD5D)))));}break;case 269: _LL33A:{
struct Cyc_List_List*_tmpD6A;struct Cyc_Absyn_ConstArray_mod_struct _tmpD69;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD68;struct Cyc_Parse_Abstractdeclarator*
_tmpD67;yyval=Cyc_YY30(((_tmpD67=_cycalloc(sizeof(*_tmpD67)),((_tmpD67->tms=((
_tmpD6A=_cycalloc(sizeof(*_tmpD6A)),((_tmpD6A->hd=(void*)((_tmpD68=_cycalloc(
sizeof(*_tmpD68)),((_tmpD68[0]=((_tmpD69.tag=1,((_tmpD69.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD69.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD69.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD69)))))))),
_tmpD68)))),((_tmpD6A->tl=0,_tmpD6A)))))),_tmpD67)))));}break;case 270: _LL33B:{
struct Cyc_List_List*_tmpD74;struct Cyc_Absyn_ConstArray_mod_struct _tmpD73;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD72;struct Cyc_Parse_Abstractdeclarator*
_tmpD71;yyval=Cyc_YY30(((_tmpD71=_cycalloc(sizeof(*_tmpD71)),((_tmpD71->tms=((
_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74->hd=(void*)((_tmpD72=_cycalloc(
sizeof(*_tmpD72)),((_tmpD72[0]=((_tmpD73.tag=1,((_tmpD73.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD73.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD73.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD73)))))))),
_tmpD72)))),((_tmpD74->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpD74)))))),_tmpD71)))));}break;case 271: _LL33C:{
struct Cyc_List_List*_tmpD89;struct Cyc_Absyn_Function_mod_struct _tmpD88;struct Cyc_Absyn_WithTypes_struct
_tmpD87;struct Cyc_Absyn_WithTypes_struct*_tmpD86;struct Cyc_Absyn_Function_mod_struct*
_tmpD85;struct Cyc_Parse_Abstractdeclarator*_tmpD84;yyval=Cyc_YY30(((_tmpD84=
_cycalloc(sizeof(*_tmpD84)),((_tmpD84->tms=((_tmpD89=_cycalloc(sizeof(*_tmpD89)),((
_tmpD89->hd=(void*)((_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85[0]=((_tmpD88.tag=
3,((_tmpD88.f1=(void*)((void*)((_tmpD86=_cycalloc(sizeof(*_tmpD86)),((_tmpD86[0]=((
_tmpD87.tag=1,((_tmpD87.f1=0,((_tmpD87.f2=0,((_tmpD87.f3=0,((_tmpD87.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD87.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD87)))))))))))),
_tmpD86))))),_tmpD88)))),_tmpD85)))),((_tmpD89->tl=0,_tmpD89)))))),_tmpD84)))));}
break;case 272: _LL33D: {struct _tuple23 _tmp721;struct Cyc_List_List*_tmp722;int
_tmp723;struct Cyc_Absyn_VarargInfo*_tmp724;struct Cyc_Core_Opt*_tmp725;struct Cyc_List_List*
_tmp726;struct _tuple23*_tmp720=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp721=*_tmp720;_tmp722=
_tmp721.f1;_tmp723=_tmp721.f2;_tmp724=_tmp721.f3;_tmp725=_tmp721.f4;_tmp726=
_tmp721.f5;{struct Cyc_List_List*_tmpD9E;struct Cyc_Absyn_Function_mod_struct
_tmpD9D;struct Cyc_Absyn_WithTypes_struct _tmpD9C;struct Cyc_Absyn_WithTypes_struct*
_tmpD9B;struct Cyc_Absyn_Function_mod_struct*_tmpD9A;struct Cyc_Parse_Abstractdeclarator*
_tmpD99;yyval=Cyc_YY30(((_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99->tms=((
_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->hd=(void*)((_tmpD9A=_cycalloc(
sizeof(*_tmpD9A)),((_tmpD9A[0]=((_tmpD9D.tag=3,((_tmpD9D.f1=(void*)((void*)((
_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B[0]=((_tmpD9C.tag=1,((_tmpD9C.f1=
_tmp722,((_tmpD9C.f2=_tmp723,((_tmpD9C.f3=_tmp724,((_tmpD9C.f4=_tmp725,((_tmpD9C.f5=
_tmp726,_tmpD9C)))))))))))),_tmpD9B))))),_tmpD9D)))),_tmpD9A)))),((_tmpD9E->tl=0,
_tmpD9E)))))),_tmpD99)))));}break;}case 273: _LL33E:{struct Cyc_List_List*_tmpDB3;
struct Cyc_Absyn_Function_mod_struct _tmpDB2;struct Cyc_Absyn_WithTypes_struct
_tmpDB1;struct Cyc_Absyn_WithTypes_struct*_tmpDB0;struct Cyc_Absyn_Function_mod_struct*
_tmpDAF;struct Cyc_Parse_Abstractdeclarator*_tmpDAE;yyval=Cyc_YY30(((_tmpDAE=
_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->tms=((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((
_tmpDB3->hd=(void*)((_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF[0]=((_tmpDB2.tag=
3,((_tmpDB2.f1=(void*)((void*)((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0[0]=((
_tmpDB1.tag=1,((_tmpDB1.f1=0,((_tmpDB1.f2=0,((_tmpDB1.f3=0,((_tmpDB1.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDB1.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDB1)))))))))))),
_tmpDB0))))),_tmpDB2)))),_tmpDAF)))),((_tmpDB3->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpDB3)))))),
_tmpDAE)))));}break;case 274: _LL33F: {struct _tuple23 _tmp734;struct Cyc_List_List*
_tmp735;int _tmp736;struct Cyc_Absyn_VarargInfo*_tmp737;struct Cyc_Core_Opt*_tmp738;
struct Cyc_List_List*_tmp739;struct _tuple23*_tmp733=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp734=*_tmp733;_tmp735=
_tmp734.f1;_tmp736=_tmp734.f2;_tmp737=_tmp734.f3;_tmp738=_tmp734.f4;_tmp739=
_tmp734.f5;{struct Cyc_List_List*_tmpDC8;struct Cyc_Absyn_Function_mod_struct
_tmpDC7;struct Cyc_Absyn_WithTypes_struct _tmpDC6;struct Cyc_Absyn_WithTypes_struct*
_tmpDC5;struct Cyc_Absyn_Function_mod_struct*_tmpDC4;struct Cyc_Parse_Abstractdeclarator*
_tmpDC3;yyval=Cyc_YY30(((_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3->tms=((
_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8->hd=(void*)((_tmpDC4=_cycalloc(
sizeof(*_tmpDC4)),((_tmpDC4[0]=((_tmpDC7.tag=3,((_tmpDC7.f1=(void*)((void*)((
_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5[0]=((_tmpDC6.tag=1,((_tmpDC6.f1=
_tmp735,((_tmpDC6.f2=_tmp736,((_tmpDC6.f3=_tmp737,((_tmpDC6.f4=_tmp738,((_tmpDC6.f5=
_tmp739,_tmpDC6)))))))))))),_tmpDC5))))),_tmpDC7)))),_tmpDC4)))),((_tmpDC8->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpDC8)))))),_tmpDC3)))));}break;}case 275: _LL340: {struct Cyc_List_List*_tmp740=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{
struct Cyc_List_List*_tmpDD2;struct Cyc_Absyn_TypeParams_mod_struct _tmpDD1;struct
Cyc_Absyn_TypeParams_mod_struct*_tmpDD0;struct Cyc_Parse_Abstractdeclarator*
_tmpDCF;yyval=Cyc_YY30(((_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF->tms=((
_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->hd=(void*)((_tmpDD0=_cycalloc(
sizeof(*_tmpDD0)),((_tmpDD0[0]=((_tmpDD1.tag=4,((_tmpDD1.f1=_tmp740,((_tmpDD1.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpDD1.f3=0,_tmpDD1)))))))),_tmpDD0)))),((_tmpDD2->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpDD2)))))),_tmpDCF)))));}break;}case 276: _LL341:{struct Cyc_List_List*_tmpDDC;
struct Cyc_Absyn_Attributes_mod_struct _tmpDDB;struct Cyc_Absyn_Attributes_mod_struct*
_tmpDDA;struct Cyc_Parse_Abstractdeclarator*_tmpDD9;yyval=Cyc_YY30(((_tmpDD9=
_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9->tms=((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((
_tmpDDC->hd=(void*)((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA[0]=((_tmpDDB.tag=
5,((_tmpDDB.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpDDB.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDDB)))))),_tmpDDA)))),((
_tmpDDC->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpDDC)))))),_tmpDD9)))));}break;case 277: _LL342: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 278: _LL343:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 279:
_LL344: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
280: _LL345: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 281: _LL346: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 282: _LL347: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 283: _LL348:{struct Cyc_Absyn_ResetRegion_s_struct _tmpDDF;
struct Cyc_Absyn_ResetRegion_s_struct*_tmpDDE;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((
void*)((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE[0]=((_tmpDDF.tag=16,((
_tmpDDF.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),_tmpDDF)))),_tmpDDE)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 284:
_LL349: yyval=Cyc_YY4(0);break;case 285: _LL34A:{const char*_tmpDE0;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmpDE0="open",_tag_dyneither(_tmpDE0,sizeof(char),5))))
!= 0){const char*_tmpDE1;Cyc_Parse_err(((_tmpDE1="expecting `open'",
_tag_dyneither(_tmpDE1,sizeof(char),17))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}yyval=Cyc_YY4((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));break;case 286: _LL34B:{struct Cyc_Absyn_Label_s_struct _tmpDE7;
struct _dyneither_ptr*_tmpDE6;struct Cyc_Absyn_Label_s_struct*_tmpDE5;yyval=Cyc_YY9(
Cyc_Absyn_new_stmt((void*)((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5[0]=((
_tmpDE7.tag=13,((_tmpDE7.f1=((_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpDE6)))),((_tmpDE7.f2=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpDE7)))))),_tmpDE5)))),Cyc_Position_segment_of_abs((yyls[
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
_LL354:{struct Cyc_Absyn_Fn_d_struct _tmpDEA;struct Cyc_Absyn_Fn_d_struct*_tmpDE9;
yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDE9=
_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9[0]=((_tmpDEA.tag=1,((_tmpDEA.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDEA)))),_tmpDE9)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0)));}break;case 296: _LL355:{
struct Cyc_Absyn_Fn_d_struct _tmpDED;struct Cyc_Absyn_Fn_d_struct*_tmpDEC;yyval=Cyc_YY9(
Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDEC=_cycalloc(sizeof(*
_tmpDEC)),((_tmpDEC[0]=((_tmpDED.tag=1,((_tmpDED.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDED)))),_tmpDEC)))),
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
_LL359: {struct Cyc_Absyn_UnknownId_e_struct _tmpDF0;struct Cyc_Absyn_UnknownId_e_struct*
_tmpDEF;struct Cyc_Absyn_Exp*_tmp754=Cyc_Absyn_new_exp((void*)((_tmpDEF=_cycalloc(
sizeof(*_tmpDEF)),((_tmpDEF[0]=((_tmpDF0.tag=2,((_tmpDF0.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpDF0)))),_tmpDEF)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp754,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 301:
_LL35A: {struct Cyc_Absyn_Exp*_tmp757=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(
Cyc_Absyn_switch_stmt(_tmp757,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 302:
_LL35B: yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 303:
_LL35C: yyval=Cyc_YY10(0);break;case 304: _LL35D:{struct Cyc_Absyn_Switch_clause*
_tmpDF3;struct Cyc_List_List*_tmpDF2;yyval=Cyc_YY10(((_tmpDF2=_cycalloc(sizeof(*
_tmpDF2)),((_tmpDF2->hd=((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3->pattern=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)),((_tmpDF3->pat_vars=
0,((_tmpDF3->where_clause=0,((_tmpDF3->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDF3->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDF3)))))))))))),((
_tmpDF2->tl=0,_tmpDF2)))))));}break;case 305: _LL35E:{struct Cyc_Absyn_Switch_clause*
_tmpDF6;struct Cyc_List_List*_tmpDF5;yyval=Cyc_YY10(((_tmpDF5=_cycalloc(sizeof(*
_tmpDF5)),((_tmpDF5->hd=((_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpDF6->pat_vars=0,((_tmpDF6->where_clause=0,((_tmpDF6->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpDF6->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDF6)))))))))))),((
_tmpDF5->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpDF5)))))));}break;case 306: _LL35F:{struct Cyc_Absyn_Switch_clause*
_tmpDF9;struct Cyc_List_List*_tmpDF8;yyval=Cyc_YY10(((_tmpDF8=_cycalloc(sizeof(*
_tmpDF8)),((_tmpDF8->hd=((_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpDF9->pat_vars=0,((_tmpDF9->where_clause=0,((_tmpDF9->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpDF9->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpDF9)))))))))))),((
_tmpDF8->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpDF8)))))));}break;case 307: _LL360:{struct Cyc_Absyn_Switch_clause*
_tmpDFC;struct Cyc_List_List*_tmpDFB;yyval=Cyc_YY10(((_tmpDFB=_cycalloc(sizeof(*
_tmpDFB)),((_tmpDFB->hd=((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((
_tmpDFC->pat_vars=0,((_tmpDFC->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDFC->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpDFC->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDFC)))))))))))),((
_tmpDFB->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpDFB)))))));}break;case 308: _LL361:{struct Cyc_Absyn_Switch_clause*
_tmpDFF;struct Cyc_List_List*_tmpDFE;yyval=Cyc_YY10(((_tmpDFE=_cycalloc(sizeof(*
_tmpDFE)),((_tmpDFE->hd=((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((
_tmpDFF->pat_vars=0,((_tmpDFF->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpDFF->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpDFF->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDFF)))))))))))),((
_tmpDFE->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpDFE)))))));}break;case 309: _LL362: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
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
_LL36C: {struct Cyc_List_List*_tmp762=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp763=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp762,_tmp763));break;}case 320: _LL36D: {struct
Cyc_List_List*_tmp764=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp765=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp764,
_tmp765));break;}case 321: _LL36E: {struct Cyc_List_List*_tmp766=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp767=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp766,_tmp767));break;}case 322: _LL36F: {struct
Cyc_List_List*_tmp768=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp769=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp768,_tmp769));break;}case 323: _LL370:{struct
_dyneither_ptr*_tmpE00;yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((_tmpE00=_cycalloc(
sizeof(*_tmpE00)),((_tmpE00[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE00)))),Cyc_Position_segment_of_abs((
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
363: _LL398: {void*_tmp76B=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));yyval=Cyc_YY11(
Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp76B,Cyc_yyget_YY3(yyvs[
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
yylsp_offset)]).last_line))));break;case 367: _LL39C: {void*_tmp76C=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY11(
Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp76C,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;}case
368: _LL39D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 369:
_LL39E:{struct Cyc_Absyn_StructField_struct _tmpE06;struct _dyneither_ptr*_tmpE05;
struct Cyc_Absyn_StructField_struct*_tmpE04;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpE04=_cycalloc(sizeof(*_tmpE04)),((_tmpE04[0]=((_tmpE06.tag=0,((
_tmpE06.f1=((_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE05)))),_tmpE06)))),
_tmpE04)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 370: _LL39F: {void*_tmp770=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_TupleIndex_struct
_tmpE09;struct Cyc_Absyn_TupleIndex_struct*_tmpE08;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp(_tmp770,(void*)((_tmpE08=_cycalloc_atomic(sizeof(*_tmpE08)),((
_tmpE08[0]=((_tmpE09.tag=1,((_tmpE09.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,_tmpE09)))),_tmpE08)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;}case 371: _LL3A0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 372: _LL3A1: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 373: _LL3A2:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 374:
_LL3A3: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 375: _LL3A4: {
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);{void*_tmp773=e->r;union Cyc_Absyn_Cnst _tmp775;struct _tuple4
_tmp776;enum Cyc_Absyn_Sign _tmp777;char _tmp778;union Cyc_Absyn_Cnst _tmp77A;struct
_tuple5 _tmp77B;enum Cyc_Absyn_Sign _tmp77C;short _tmp77D;union Cyc_Absyn_Cnst
_tmp77F;struct _tuple6 _tmp780;enum Cyc_Absyn_Sign _tmp781;int _tmp782;union Cyc_Absyn_Cnst
_tmp784;struct _dyneither_ptr _tmp785;union Cyc_Absyn_Cnst _tmp787;int _tmp788;union
Cyc_Absyn_Cnst _tmp78A;struct _dyneither_ptr _tmp78B;union Cyc_Absyn_Cnst _tmp78D;
struct _tuple7 _tmp78E;_LL3A7: {struct Cyc_Absyn_Const_e_struct*_tmp774=(struct Cyc_Absyn_Const_e_struct*)
_tmp773;if(_tmp774->tag != 0)goto _LL3A9;else{_tmp775=_tmp774->f1;if((_tmp775.Char_c).tag
!= 2)goto _LL3A9;_tmp776=(struct _tuple4)(_tmp775.Char_c).val;_tmp777=_tmp776.f1;
_tmp778=_tmp776.f2;}}_LL3A8:{struct Cyc_Absyn_Char_p_struct _tmpE0C;struct Cyc_Absyn_Char_p_struct*
_tmpE0B;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE0B=_cycalloc_atomic(
sizeof(*_tmpE0B)),((_tmpE0B[0]=((_tmpE0C.tag=10,((_tmpE0C.f1=_tmp778,_tmpE0C)))),
_tmpE0B)))),e->loc));}goto _LL3A6;_LL3A9: {struct Cyc_Absyn_Const_e_struct*_tmp779=(
struct Cyc_Absyn_Const_e_struct*)_tmp773;if(_tmp779->tag != 0)goto _LL3AB;else{
_tmp77A=_tmp779->f1;if((_tmp77A.Short_c).tag != 3)goto _LL3AB;_tmp77B=(struct
_tuple5)(_tmp77A.Short_c).val;_tmp77C=_tmp77B.f1;_tmp77D=_tmp77B.f2;}}_LL3AA:{
struct Cyc_Absyn_Int_p_struct _tmpE0F;struct Cyc_Absyn_Int_p_struct*_tmpE0E;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE0E=_cycalloc_atomic(sizeof(*_tmpE0E)),((
_tmpE0E[0]=((_tmpE0F.tag=9,((_tmpE0F.f1=_tmp77C,((_tmpE0F.f2=(int)_tmp77D,
_tmpE0F)))))),_tmpE0E)))),e->loc));}goto _LL3A6;_LL3AB: {struct Cyc_Absyn_Const_e_struct*
_tmp77E=(struct Cyc_Absyn_Const_e_struct*)_tmp773;if(_tmp77E->tag != 0)goto _LL3AD;
else{_tmp77F=_tmp77E->f1;if((_tmp77F.Int_c).tag != 4)goto _LL3AD;_tmp780=(struct
_tuple6)(_tmp77F.Int_c).val;_tmp781=_tmp780.f1;_tmp782=_tmp780.f2;}}_LL3AC:{
struct Cyc_Absyn_Int_p_struct _tmpE12;struct Cyc_Absyn_Int_p_struct*_tmpE11;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE11=_cycalloc_atomic(sizeof(*_tmpE11)),((
_tmpE11[0]=((_tmpE12.tag=9,((_tmpE12.f1=_tmp781,((_tmpE12.f2=_tmp782,_tmpE12)))))),
_tmpE11)))),e->loc));}goto _LL3A6;_LL3AD: {struct Cyc_Absyn_Const_e_struct*_tmp783=(
struct Cyc_Absyn_Const_e_struct*)_tmp773;if(_tmp783->tag != 0)goto _LL3AF;else{
_tmp784=_tmp783->f1;if((_tmp784.Float_c).tag != 6)goto _LL3AF;_tmp785=(struct
_dyneither_ptr)(_tmp784.Float_c).val;}}_LL3AE:{struct Cyc_Absyn_Float_p_struct
_tmpE15;struct Cyc_Absyn_Float_p_struct*_tmpE14;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14[0]=((_tmpE15.tag=11,((
_tmpE15.f1=_tmp785,_tmpE15)))),_tmpE14)))),e->loc));}goto _LL3A6;_LL3AF: {struct
Cyc_Absyn_Const_e_struct*_tmp786=(struct Cyc_Absyn_Const_e_struct*)_tmp773;if(
_tmp786->tag != 0)goto _LL3B1;else{_tmp787=_tmp786->f1;if((_tmp787.Null_c).tag != 1)
goto _LL3B1;_tmp788=(int)(_tmp787.Null_c).val;}}_LL3B0: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A6;_LL3B1: {struct Cyc_Absyn_Const_e_struct*
_tmp789=(struct Cyc_Absyn_Const_e_struct*)_tmp773;if(_tmp789->tag != 0)goto _LL3B3;
else{_tmp78A=_tmp789->f1;if((_tmp78A.String_c).tag != 7)goto _LL3B3;_tmp78B=(
struct _dyneither_ptr)(_tmp78A.String_c).val;}}_LL3B2:{const char*_tmpE16;Cyc_Parse_err(((
_tmpE16="strings cannot occur within patterns",_tag_dyneither(_tmpE16,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL3A6;_LL3B3: {struct Cyc_Absyn_Const_e_struct*
_tmp78C=(struct Cyc_Absyn_Const_e_struct*)_tmp773;if(_tmp78C->tag != 0)goto _LL3B5;
else{_tmp78D=_tmp78C->f1;if((_tmp78D.LongLong_c).tag != 5)goto _LL3B5;_tmp78E=(
struct _tuple7)(_tmp78D.LongLong_c).val;}}_LL3B4:{const char*_tmpE17;Cyc_Parse_unimp(((
_tmpE17="long long's in patterns",_tag_dyneither(_tmpE17,sizeof(char),24))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}goto _LL3A6;
_LL3B5:;_LL3B6: {const char*_tmpE18;Cyc_Parse_err(((_tmpE18="bad constant in case",
_tag_dyneither(_tmpE18,sizeof(char),21))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}_LL3A6:;}
break;}case 376: _LL3A5:{struct Cyc_Absyn_UnknownId_p_struct _tmpE1B;struct Cyc_Absyn_UnknownId_p_struct*
_tmpE1A;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE1A=_cycalloc(sizeof(*
_tmpE1A)),((_tmpE1A[0]=((_tmpE1B.tag=14,((_tmpE1B.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE1B)))),_tmpE1A)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 377:
_LL3B7:{const char*_tmpE1C;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE1C="as",
_tag_dyneither(_tmpE1C,sizeof(char),3))))!= 0){const char*_tmpE1D;Cyc_Parse_err(((
_tmpE1D="expecting `as'",_tag_dyneither(_tmpE1D,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Var_p_struct
_tmpE27;struct _dyneither_ptr*_tmpE26;struct _tuple1*_tmpE25;struct Cyc_Absyn_Var_p_struct*
_tmpE24;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE24=_cycalloc(sizeof(*
_tmpE24)),((_tmpE24[0]=((_tmpE27.tag=1,((_tmpE27.f1=Cyc_Absyn_new_vardecl(((
_tmpE25=_cycalloc(sizeof(*_tmpE25)),((_tmpE25->f1=Cyc_Absyn_Loc_n,((_tmpE25->f2=((
_tmpE26=_cycalloc(sizeof(*_tmpE26)),((_tmpE26[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE26)))),_tmpE25)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE27.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE27)))))),_tmpE24)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 378: _LL3B8: {struct Cyc_List_List*
_tmp7A3;int _tmp7A4;struct _tuple19 _tmp7A2=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A3=_tmp7A2.f1;
_tmp7A4=_tmp7A2.f2;{struct Cyc_Absyn_Tuple_p_struct _tmpE2A;struct Cyc_Absyn_Tuple_p_struct*
_tmpE29;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE29=_cycalloc(sizeof(*
_tmpE29)),((_tmpE29[0]=((_tmpE2A.tag=4,((_tmpE2A.f1=_tmp7A3,((_tmpE2A.f2=_tmp7A4,
_tmpE2A)))))),_tmpE29)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
379: _LL3B9: {struct Cyc_List_List*_tmp7A8;int _tmp7A9;struct _tuple19 _tmp7A7=*Cyc_yyget_YY12(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A8=_tmp7A7.f1;
_tmp7A9=_tmp7A7.f2;{struct Cyc_Absyn_UnknownCall_p_struct _tmpE2D;struct Cyc_Absyn_UnknownCall_p_struct*
_tmpE2C;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE2C=_cycalloc(sizeof(*
_tmpE2C)),((_tmpE2C[0]=((_tmpE2D.tag=15,((_tmpE2D.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpE2D.f2=_tmp7A8,((
_tmpE2D.f3=_tmp7A9,_tmpE2D)))))))),_tmpE2C)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
380: _LL3BA: {struct Cyc_List_List*_tmp7AD;int _tmp7AE;struct _tuple19 _tmp7AC=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7AD=_tmp7AC.f1;
_tmp7AE=_tmp7AC.f2;{struct Cyc_List_List*_tmp7AF=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpE33;struct Cyc_Absyn_AggrInfo*_tmpE32;struct Cyc_Absyn_Aggr_p_struct*_tmpE31;
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((
_tmpE31[0]=((_tmpE33.tag=6,((_tmpE33.f1=((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((
_tmpE32->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),0),((_tmpE32->targs=
0,_tmpE32)))))),((_tmpE33.f2=_tmp7AF,((_tmpE33.f3=_tmp7AD,((_tmpE33.f4=_tmp7AE,
_tmpE33)))))))))),_tmpE31)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;};}case
381: _LL3BB: {struct Cyc_List_List*_tmp7B4;int _tmp7B5;struct _tuple19 _tmp7B3=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7B4=_tmp7B3.f1;
_tmp7B5=_tmp7B3.f2;{struct Cyc_List_List*_tmp7B6=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpE36;struct Cyc_Absyn_Aggr_p_struct*_tmpE35;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35[0]=((_tmpE36.tag=6,((
_tmpE36.f1=0,((_tmpE36.f2=_tmp7B6,((_tmpE36.f3=_tmp7B4,((_tmpE36.f4=_tmp7B5,
_tmpE36)))))))))),_tmpE35)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;};}case
382: _LL3BC:{struct Cyc_Absyn_Pointer_p_struct _tmpE39;struct Cyc_Absyn_Pointer_p_struct*
_tmpE38;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE38=_cycalloc(sizeof(*
_tmpE38)),((_tmpE38[0]=((_tmpE39.tag=5,((_tmpE39.f1=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE39)))),_tmpE38)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 383:
_LL3BD:{struct Cyc_Absyn_Pointer_p_struct _tmpE43;struct Cyc_Absyn_Pointer_p_struct
_tmpE42;struct Cyc_Absyn_Pointer_p_struct*_tmpE41;struct Cyc_Absyn_Pointer_p_struct*
_tmpE40;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE40=_cycalloc(sizeof(*
_tmpE40)),((_tmpE40[0]=((_tmpE43.tag=5,((_tmpE43.f1=Cyc_Absyn_new_pat((void*)((
_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41[0]=((_tmpE42.tag=5,((_tmpE42.f1=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE42)))),
_tmpE41)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpE43)))),_tmpE40)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 384:
_LL3BE:{struct Cyc_Absyn_Reference_p_struct _tmpE4D;struct _dyneither_ptr*_tmpE4C;
struct _tuple1*_tmpE4B;struct Cyc_Absyn_Reference_p_struct*_tmpE4A;yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)((_tmpE4A=_cycalloc(sizeof(*_tmpE4A)),((_tmpE4A[0]=((
_tmpE4D.tag=2,((_tmpE4D.f1=Cyc_Absyn_new_vardecl(((_tmpE4B=_cycalloc(sizeof(*
_tmpE4B)),((_tmpE4B->f1=Cyc_Absyn_Loc_n,((_tmpE4B->f2=((_tmpE4C=_cycalloc(
sizeof(*_tmpE4C)),((_tmpE4C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE4C)))),_tmpE4B)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE4D.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpE4D)))))),_tmpE4A)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 385:
_LL3BF:{const char*_tmpE4E;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE4E="as",
_tag_dyneither(_tmpE4E,sizeof(char),3))))!= 0){const char*_tmpE4F;Cyc_Parse_err(((
_tmpE4F="expecting `as'",_tag_dyneither(_tmpE4F,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Reference_p_struct
_tmpE59;struct _dyneither_ptr*_tmpE58;struct _tuple1*_tmpE57;struct Cyc_Absyn_Reference_p_struct*
_tmpE56;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE56=_cycalloc(sizeof(*
_tmpE56)),((_tmpE56[0]=((_tmpE59.tag=2,((_tmpE59.f1=Cyc_Absyn_new_vardecl(((
_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57->f1=Cyc_Absyn_Loc_n,((_tmpE57->f2=((
_tmpE58=_cycalloc(sizeof(*_tmpE58)),((_tmpE58[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE58)))),_tmpE57)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE59.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE59)))))),_tmpE56)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 386: _LL3C0: {void*_tmp7C9=Cyc_Parse_id2type(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct
_tmpE6A;struct Cyc_Absyn_TagType_struct _tmpE69;struct Cyc_Absyn_TagType_struct*
_tmpE68;struct _dyneither_ptr*_tmpE67;struct _tuple1*_tmpE66;struct Cyc_Absyn_TagInt_p_struct*
_tmpE65;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE65=_cycalloc(sizeof(*
_tmpE65)),((_tmpE65[0]=((_tmpE6A.tag=3,((_tmpE6A.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp7C9),((
_tmpE6A.f2=Cyc_Absyn_new_vardecl(((_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66->f1=
Cyc_Absyn_Loc_n,((_tmpE66->f2=((_tmpE67=_cycalloc(sizeof(*_tmpE67)),((_tmpE67[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpE67)))),_tmpE66)))))),(void*)((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68[
0]=((_tmpE69.tag=20,((_tmpE69.f1=(void*)_tmp7C9,_tmpE69)))),_tmpE68)))),0),
_tmpE6A)))))),_tmpE65)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
387: _LL3C1: {struct Cyc_Absyn_Tvar*_tmp7D0=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(&
Cyc_Tcutil_ik));{struct Cyc_Absyn_TagInt_p_struct _tmpE86;struct Cyc_Absyn_TagType_struct
_tmpE85;struct Cyc_Absyn_VarType_struct _tmpE84;struct Cyc_Absyn_VarType_struct*
_tmpE83;struct Cyc_Absyn_TagType_struct*_tmpE82;struct _dyneither_ptr*_tmpE81;
struct _tuple1*_tmpE80;struct Cyc_Absyn_TagInt_p_struct*_tmpE7F;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE7F=_cycalloc(sizeof(*_tmpE7F)),((_tmpE7F[0]=((_tmpE86.tag=3,((
_tmpE86.f1=_tmp7D0,((_tmpE86.f2=Cyc_Absyn_new_vardecl(((_tmpE80=_cycalloc(
sizeof(*_tmpE80)),((_tmpE80->f1=Cyc_Absyn_Loc_n,((_tmpE80->f2=((_tmpE81=
_cycalloc(sizeof(*_tmpE81)),((_tmpE81[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpE81)))),_tmpE80)))))),(
void*)((_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82[0]=((_tmpE85.tag=20,((
_tmpE85.f1=(void*)((void*)((_tmpE83=_cycalloc(sizeof(*_tmpE83)),((_tmpE83[0]=((
_tmpE84.tag=2,((_tmpE84.f1=_tmp7D0,_tmpE84)))),_tmpE83))))),_tmpE85)))),_tmpE82)))),
0),_tmpE86)))))),_tmpE7F)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
388: _LL3C2:{struct _tuple19*_tmpE87;yyval=Cyc_YY12(((_tmpE87=_cycalloc(sizeof(*
_tmpE87)),((_tmpE87->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpE87->f2=0,_tmpE87)))))));}break;case 389: _LL3C3:{struct _tuple19*_tmpE88;yyval=
Cyc_YY12(((_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpE88->f2=1,_tmpE88)))))));}
break;case 390: _LL3C4:{struct _tuple19*_tmpE89;yyval=Cyc_YY12(((_tmpE89=_cycalloc(
sizeof(*_tmpE89)),((_tmpE89->f1=0,((_tmpE89->f2=1,_tmpE89)))))));}break;case 391:
_LL3C5:{struct Cyc_List_List*_tmpE8A;yyval=Cyc_YY13(((_tmpE8A=_cycalloc(sizeof(*
_tmpE8A)),((_tmpE8A->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpE8A->tl=0,_tmpE8A)))))));}break;case 392: _LL3C6:{struct Cyc_List_List*
_tmpE8B;yyval=Cyc_YY13(((_tmpE8B=_cycalloc(sizeof(*_tmpE8B)),((_tmpE8B->hd=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpE8B->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE8B)))))));}
break;case 393: _LL3C7:{struct _tuple20*_tmpE8C;yyval=Cyc_YY14(((_tmpE8C=_cycalloc(
sizeof(*_tmpE8C)),((_tmpE8C->f1=0,((_tmpE8C->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE8C)))))));}break;case
394: _LL3C8:{struct _tuple20*_tmpE8D;yyval=Cyc_YY14(((_tmpE8D=_cycalloc(sizeof(*
_tmpE8D)),((_tmpE8D->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpE8D->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE8D)))))));}break;case
395: _LL3C9:{struct _tuple19*_tmpE8E;yyval=Cyc_YY16(((_tmpE8E=_cycalloc(sizeof(*
_tmpE8E)),((_tmpE8E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpE8E->f2=0,_tmpE8E)))))));}break;case 396: _LL3CA:{struct _tuple19*_tmpE8F;yyval=
Cyc_YY16(((_tmpE8F=_cycalloc(sizeof(*_tmpE8F)),((_tmpE8F->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpE8F->f2=1,_tmpE8F)))))));}
break;case 397: _LL3CB:{struct _tuple19*_tmpE90;yyval=Cyc_YY16(((_tmpE90=_cycalloc(
sizeof(*_tmpE90)),((_tmpE90->f1=0,((_tmpE90->f2=1,_tmpE90)))))));}break;case 398:
_LL3CC:{struct Cyc_List_List*_tmpE91;yyval=Cyc_YY15(((_tmpE91=_cycalloc(sizeof(*
_tmpE91)),((_tmpE91->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpE91->tl=0,_tmpE91)))))));}break;case 399: _LL3CD:{struct Cyc_List_List*
_tmpE92;yyval=Cyc_YY15(((_tmpE92=_cycalloc(sizeof(*_tmpE92)),((_tmpE92->hd=Cyc_yyget_YY14(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpE92->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE92)))))));}
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
_LL3D3: yyval=Cyc_YY8(0);break;case 406: _LL3D4:{struct Cyc_Core_Opt*_tmpE93;yyval=
Cyc_YY8(((_tmpE93=_cycalloc_atomic(sizeof(*_tmpE93)),((_tmpE93->v=(void*)Cyc_Absyn_Times,
_tmpE93)))));}break;case 407: _LL3D5:{struct Cyc_Core_Opt*_tmpE94;yyval=Cyc_YY8(((
_tmpE94=_cycalloc_atomic(sizeof(*_tmpE94)),((_tmpE94->v=(void*)Cyc_Absyn_Div,
_tmpE94)))));}break;case 408: _LL3D6:{struct Cyc_Core_Opt*_tmpE95;yyval=Cyc_YY8(((
_tmpE95=_cycalloc_atomic(sizeof(*_tmpE95)),((_tmpE95->v=(void*)Cyc_Absyn_Mod,
_tmpE95)))));}break;case 409: _LL3D7:{struct Cyc_Core_Opt*_tmpE96;yyval=Cyc_YY8(((
_tmpE96=_cycalloc_atomic(sizeof(*_tmpE96)),((_tmpE96->v=(void*)Cyc_Absyn_Plus,
_tmpE96)))));}break;case 410: _LL3D8:{struct Cyc_Core_Opt*_tmpE97;yyval=Cyc_YY8(((
_tmpE97=_cycalloc_atomic(sizeof(*_tmpE97)),((_tmpE97->v=(void*)Cyc_Absyn_Minus,
_tmpE97)))));}break;case 411: _LL3D9:{struct Cyc_Core_Opt*_tmpE98;yyval=Cyc_YY8(((
_tmpE98=_cycalloc_atomic(sizeof(*_tmpE98)),((_tmpE98->v=(void*)Cyc_Absyn_Bitlshift,
_tmpE98)))));}break;case 412: _LL3DA:{struct Cyc_Core_Opt*_tmpE99;yyval=Cyc_YY8(((
_tmpE99=_cycalloc_atomic(sizeof(*_tmpE99)),((_tmpE99->v=(void*)Cyc_Absyn_Bitlrshift,
_tmpE99)))));}break;case 413: _LL3DB:{struct Cyc_Core_Opt*_tmpE9A;yyval=Cyc_YY8(((
_tmpE9A=_cycalloc_atomic(sizeof(*_tmpE9A)),((_tmpE9A->v=(void*)Cyc_Absyn_Bitand,
_tmpE9A)))));}break;case 414: _LL3DC:{struct Cyc_Core_Opt*_tmpE9B;yyval=Cyc_YY8(((
_tmpE9B=_cycalloc_atomic(sizeof(*_tmpE9B)),((_tmpE9B->v=(void*)Cyc_Absyn_Bitxor,
_tmpE9B)))));}break;case 415: _LL3DD:{struct Cyc_Core_Opt*_tmpE9C;yyval=Cyc_YY8(((
_tmpE9C=_cycalloc_atomic(sizeof(*_tmpE9C)),((_tmpE9C->v=(void*)Cyc_Absyn_Bitor,
_tmpE9C)))));}break;case 416: _LL3DE: yyval=yyvs[_check_known_subscript_notnull(
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
453: _LL403: {void*_tmp7EF=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));yyval=Cyc_YY3(
Cyc_Absyn_cast_exp(_tmp7EF,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
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
_LL40B: {void*_tmp7F0=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY3(
Cyc_Absyn_sizeoftyp_exp(_tmp7F0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 462:
_LL40C: yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 463:
_LL40D: {void*_tmp7F1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_StructField_struct
_tmpEA2;struct _dyneither_ptr*_tmpEA1;struct Cyc_Absyn_StructField_struct*_tmpEA0;
yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(_tmp7F1,(void*)((_tmpEA0=_cycalloc(sizeof(*
_tmpEA0)),((_tmpEA0[0]=((_tmpEA2.tag=0,((_tmpEA2.f1=((_tmpEA1=_cycalloc(sizeof(*
_tmpEA1)),((_tmpEA1[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpEA1)))),_tmpEA2)))),_tmpEA0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
464: _LL40E: {void*_tmp7F5=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_TupleIndex_struct
_tmpEA5;struct Cyc_Absyn_TupleIndex_struct*_tmpEA4;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(
_tmp7F5,(void*)((_tmpEA4=_cycalloc_atomic(sizeof(*_tmpEA4)),((_tmpEA4[0]=((
_tmpEA5.tag=1,((_tmpEA5.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,_tmpEA5)))),_tmpEA4)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 465: _LL40F:{struct Cyc_Absyn_Malloc_e_struct
_tmpEAB;struct Cyc_Absyn_MallocInfo _tmpEAA;struct Cyc_Absyn_Malloc_e_struct*
_tmpEA9;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEA9=_cycalloc(sizeof(*
_tmpEA9)),((_tmpEA9[0]=((_tmpEAB.tag=34,((_tmpEAB.f1=((_tmpEAA.is_calloc=0,((
_tmpEAA.rgn=0,((_tmpEAA.elt_type=0,((_tmpEAA.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpEAA.fat_result=0,
_tmpEAA)))))))))),_tmpEAB)))),_tmpEA9)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 466:
_LL410:{struct Cyc_Absyn_Malloc_e_struct _tmpEB1;struct Cyc_Absyn_MallocInfo _tmpEB0;
struct Cyc_Absyn_Malloc_e_struct*_tmpEAF;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((_tmpEAF[0]=((_tmpEB1.tag=34,((_tmpEB1.f1=((
_tmpEB0.is_calloc=0,((_tmpEB0.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpEB0.elt_type=0,((
_tmpEB0.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpEB0.fat_result=0,_tmpEB0)))))))))),_tmpEB1)))),_tmpEAF)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 467: _LL411: {void*_tmp7FE=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));{struct Cyc_Absyn_Malloc_e_struct
_tmpEBB;void**_tmpEBA;struct Cyc_Absyn_MallocInfo _tmpEB9;struct Cyc_Absyn_Malloc_e_struct*
_tmpEB8;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEB8=_cycalloc(sizeof(*
_tmpEB8)),((_tmpEB8[0]=((_tmpEBB.tag=34,((_tmpEBB.f1=((_tmpEB9.is_calloc=1,((
_tmpEB9.rgn=0,((_tmpEB9.elt_type=((_tmpEBA=_cycalloc(sizeof(*_tmpEBA)),((_tmpEBA[
0]=_tmp7FE,_tmpEBA)))),((_tmpEB9.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((_tmpEB9.fat_result=0,
_tmpEB9)))))))))),_tmpEBB)))),_tmpEB8)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
468: _LL412: {void*_tmp803=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));{struct Cyc_Absyn_Malloc_e_struct
_tmpEC5;void**_tmpEC4;struct Cyc_Absyn_MallocInfo _tmpEC3;struct Cyc_Absyn_Malloc_e_struct*
_tmpEC2;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEC2=_cycalloc(sizeof(*
_tmpEC2)),((_tmpEC2[0]=((_tmpEC5.tag=34,((_tmpEC5.f1=((_tmpEC3.is_calloc=1,((
_tmpEC3.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),((_tmpEC3.elt_type=((
_tmpEC4=_cycalloc(sizeof(*_tmpEC4)),((_tmpEC4[0]=_tmp803,_tmpEC4)))),((_tmpEC3.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((
_tmpEC3.fat_result=0,_tmpEC3)))))))))),_tmpEC5)))),_tmpEC2)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
469: _LL413:{struct Cyc_Absyn_Exp*_tmpEC6[1];yyval=Cyc_YY3(Cyc_Absyn_primop_exp(
Cyc_Absyn_Numelts,((_tmpEC6[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpEC6,sizeof(struct Cyc_Absyn_Exp*),1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 470:
_LL414:{struct Cyc_Absyn_Tagcheck_e_struct _tmpECC;struct _dyneither_ptr*_tmpECB;
struct Cyc_Absyn_Tagcheck_e_struct*_tmpECA;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpECA=_cycalloc(sizeof(*_tmpECA)),((_tmpECA[0]=((_tmpECC.tag=38,((_tmpECC.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpECC.f2=((_tmpECB=_cycalloc(sizeof(*_tmpECB)),((_tmpECB[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpECB)))),_tmpECC)))))),
_tmpECA)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 471: _LL415:{struct Cyc_Absyn_Tagcheck_e_struct
_tmpED2;struct _dyneither_ptr*_tmpED1;struct Cyc_Absyn_Tagcheck_e_struct*_tmpED0;
yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpED0=_cycalloc(sizeof(*_tmpED0)),((
_tmpED0[0]=((_tmpED2.tag=38,((_tmpED2.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)),((_tmpED2.f2=((
_tmpED1=_cycalloc(sizeof(*_tmpED1)),((_tmpED1[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpED1)))),_tmpED2)))))),
_tmpED0)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 472: _LL416: {void*_tmp80F=Cyc_Parse_type_name_to_type(
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY3(
Cyc_Absyn_valueof_exp(_tmp80F,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 473:
_LL417: {int _tmp811;struct _dyneither_ptr _tmp812;struct _tuple17 _tmp810=Cyc_yyget_Asm_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp811=_tmp810.f1;
_tmp812=_tmp810.f2;yyval=Cyc_YY3(Cyc_Absyn_asm_exp(_tmp811,_tmp812,Cyc_Position_segment_of_abs((
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
_LL41F:{struct _dyneither_ptr*_tmpED3;yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpED3=_cycalloc(
sizeof(*_tmpED3)),((_tmpED3[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpED3)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 482:
_LL420:{struct _dyneither_ptr*_tmpED4;yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpED4=_cycalloc(
sizeof(*_tmpED4)),((_tmpED4[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpED4)))),Cyc_Position_segment_of_abs((
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
_LL423:{struct Cyc_Absyn_CompoundLit_e_struct _tmpED7;struct Cyc_Absyn_CompoundLit_e_struct*
_tmpED6;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpED6=_cycalloc(sizeof(*
_tmpED6)),((_tmpED6[0]=((_tmpED7.tag=26,((_tmpED7.f1=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpED7.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmpED7)))))),_tmpED6)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 486: _LL424:{struct Cyc_Absyn_CompoundLit_e_struct
_tmpEDA;struct Cyc_Absyn_CompoundLit_e_struct*_tmpED9;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpED9=_cycalloc(sizeof(*_tmpED9)),((_tmpED9[0]=((_tmpEDA.tag=26,((
_tmpEDA.f1=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 5)]),((_tmpEDA.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),
_tmpEDA)))))),_tmpED9)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 487:
_LL425:{struct Cyc_Absyn_UnknownId_e_struct _tmpEDD;struct Cyc_Absyn_UnknownId_e_struct*
_tmpEDC;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEDC=_cycalloc(sizeof(*
_tmpEDC)),((_tmpEDC[0]=((_tmpEDD.tag=2,((_tmpEDD.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEDD)))),_tmpEDC)))),Cyc_Position_segment_of_abs((
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
_LL42C:{struct Cyc_Absyn_Aggregate_e_struct _tmpEE0;struct Cyc_Absyn_Aggregate_e_struct*
_tmpEDF;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEDF=_cycalloc(sizeof(*
_tmpEDF)),((_tmpEDF[0]=((_tmpEE0.tag=29,((_tmpEE0.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpEE0.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpEE0.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmpEE0.f4=0,
_tmpEE0)))))))))),_tmpEDF)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 495:
_LL42D: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 496:
_LL42E: yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));break;
case 497: _LL42F:{struct Cyc_List_List*_tmpEE1;yyval=Cyc_YY5(((_tmpEE1=_cycalloc(
sizeof(*_tmpEE1)),((_tmpEE1->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEE1->tl=0,_tmpEE1)))))));}
break;case 498: _LL430:{struct Cyc_List_List*_tmpEE2;yyval=Cyc_YY5(((_tmpEE2=
_cycalloc(sizeof(*_tmpEE2)),((_tmpEE2->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEE2->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpEE2)))))));}
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
_LL435:{struct _dyneither_ptr*_tmpEE5;struct _tuple1*_tmpEE4;yyval=Cyc_QualId_tok(((
_tmpEE4=_cycalloc(sizeof(*_tmpEE4)),((_tmpEE4->f1=Cyc_Absyn_Rel_n(0),((_tmpEE4->f2=((
_tmpEE5=_cycalloc(sizeof(*_tmpEE5)),((_tmpEE5[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEE5)))),_tmpEE4)))))));}
break;case 504: _LL436: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 505: _LL437:{struct _dyneither_ptr*_tmpEE8;struct _tuple1*
_tmpEE7;yyval=Cyc_QualId_tok(((_tmpEE7=_cycalloc(sizeof(*_tmpEE7)),((_tmpEE7->f1=
Cyc_Absyn_Rel_n(0),((_tmpEE7->f2=((_tmpEE8=_cycalloc(sizeof(*_tmpEE8)),((_tmpEE8[
0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpEE8)))),_tmpEE7)))))));}break;case 506: _LL438: yyval=yyvs[
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
_check_known_subscript_notnull(287,x)])+ 15,count ++);}{unsigned int _tmpEED;
unsigned int _tmpEEC;struct _dyneither_ptr _tmpEEB;char*_tmpEEA;unsigned int _tmpEE9;
msg=((_tmpEE9=(unsigned int)(sze + 15),((_tmpEEA=(char*)_region_malloc(yyregion,
_check_times(sizeof(char),_tmpEE9 + 1)),((_tmpEEB=_tag_dyneither(_tmpEEA,sizeof(
char),_tmpEE9 + 1),((((_tmpEEC=_tmpEE9,((_tmpEEE(& _tmpEED,& _tmpEEC,& _tmpEEA),
_tmpEEA[_tmpEEC]=(char)0)))),_tmpEEB))))))));}{const char*_tmpEEF;Cyc_strcpy(msg,((
_tmpEEF="parse error",_tag_dyneither(_tmpEEF,sizeof(char),12))));}if(count < 5){
count=0;for(x=yyn < 0?- yyn: 0;x < 287 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6245,x + yyn)]== x){{const char*_tmpEF1;const char*
_tmpEF0;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)((
_tmpEF1=", expecting `",_tag_dyneither(_tmpEF1,sizeof(char),14))):(struct
_dyneither_ptr)((_tmpEF0=" or `",_tag_dyneither(_tmpEF0,sizeof(char),6)))));}Cyc_strcat(
msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(287,x)]);{
const char*_tmpEF2;Cyc_strcat(msg,((_tmpEF2="'",_tag_dyneither(_tmpEF2,sizeof(
char),2))));}++ count;}}}Cyc_yyerror((struct _dyneither_ptr)msg);}else{const char*
_tmpEF3;Cyc_yyerror(((_tmpEF3="parse error",_tag_dyneither(_tmpEF3,sizeof(char),
12))));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int
_tmp82C=1;_npop_handler(0);return _tmp82C;}Cyc_yychar=- 2;}yyerrstatus=3;goto
yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp82D=1;
_npop_handler(0);return _tmp82D;}-- yyvsp_offset;yystate=(int)yyss[
_check_known_subscript_notnull(10000,-- yyssp_offset)];-- yylsp_offset;yyerrhandle:
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1028,yystate)];if(yyn == -
32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6244) || Cyc_yycheck[
_check_known_subscript_notnull(6245,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6245,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 1027){int
_tmp82E=0;_npop_handler(0);return _tmp82E;}yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE _tmp835=v;struct _tuple6 _tmp836;int
_tmp837;char _tmp838;struct _dyneither_ptr _tmp839;struct _tuple1*_tmp83A;struct
_tuple1 _tmp83B;union Cyc_Absyn_Nmspace _tmp83C;struct _dyneither_ptr*_tmp83D;_LL442:
if((_tmp835.Int_tok).tag != 1)goto _LL444;_tmp836=(struct _tuple6)(_tmp835.Int_tok).val;
_tmp837=_tmp836.f2;_LL443:{const char*_tmpEF7;void*_tmpEF6[1];struct Cyc_Int_pa_struct
_tmpEF5;(_tmpEF5.tag=1,((_tmpEF5.f1=(unsigned long)_tmp837,((_tmpEF6[0]=& _tmpEF5,
Cyc_fprintf(Cyc_stderr,((_tmpEF7="%d",_tag_dyneither(_tmpEF7,sizeof(char),3))),
_tag_dyneither(_tmpEF6,sizeof(void*),1)))))));}goto _LL441;_LL444: if((_tmp835.Char_tok).tag
!= 2)goto _LL446;_tmp838=(char)(_tmp835.Char_tok).val;_LL445:{const char*_tmpEFB;
void*_tmpEFA[1];struct Cyc_Int_pa_struct _tmpEF9;(_tmpEF9.tag=1,((_tmpEF9.f1=(
unsigned long)((int)_tmp838),((_tmpEFA[0]=& _tmpEF9,Cyc_fprintf(Cyc_stderr,((
_tmpEFB="%c",_tag_dyneither(_tmpEFB,sizeof(char),3))),_tag_dyneither(_tmpEFA,
sizeof(void*),1)))))));}goto _LL441;_LL446: if((_tmp835.String_tok).tag != 3)goto
_LL448;_tmp839=(struct _dyneither_ptr)(_tmp835.String_tok).val;_LL447:{const char*
_tmpEFF;void*_tmpEFE[1];struct Cyc_String_pa_struct _tmpEFD;(_tmpEFD.tag=0,((
_tmpEFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp839),((_tmpEFE[0]=&
_tmpEFD,Cyc_fprintf(Cyc_stderr,((_tmpEFF="\"%s\"",_tag_dyneither(_tmpEFF,sizeof(
char),5))),_tag_dyneither(_tmpEFE,sizeof(void*),1)))))));}goto _LL441;_LL448: if((
_tmp835.QualId_tok).tag != 5)goto _LL44A;_tmp83A=(struct _tuple1*)(_tmp835.QualId_tok).val;
_tmp83B=*_tmp83A;_tmp83C=_tmp83B.f1;_tmp83D=_tmp83B.f2;_LL449: {struct Cyc_List_List*
_tmp847=0;{union Cyc_Absyn_Nmspace _tmp848=_tmp83C;struct Cyc_List_List*_tmp849;
struct Cyc_List_List*_tmp84A;struct Cyc_List_List*_tmp84B;int _tmp84C;_LL44D: if((
_tmp848.Rel_n).tag != 1)goto _LL44F;_tmp849=(struct Cyc_List_List*)(_tmp848.Rel_n).val;
_LL44E: _tmp847=_tmp849;goto _LL44C;_LL44F: if((_tmp848.Abs_n).tag != 2)goto _LL451;
_tmp84A=(struct Cyc_List_List*)(_tmp848.Abs_n).val;_LL450: _tmp847=_tmp84A;goto
_LL44C;_LL451: if((_tmp848.C_n).tag != 3)goto _LL453;_tmp84B=(struct Cyc_List_List*)(
_tmp848.C_n).val;_LL452: _tmp847=_tmp84B;goto _LL44C;_LL453: if((_tmp848.Loc_n).tag
!= 4)goto _LL44C;_tmp84C=(int)(_tmp848.Loc_n).val;_LL454: goto _LL44C;_LL44C:;}for(
0;_tmp847 != 0;_tmp847=_tmp847->tl){const char*_tmpF03;void*_tmpF02[1];struct Cyc_String_pa_struct
_tmpF01;(_tmpF01.tag=0,((_tmpF01.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp847->hd)),((_tmpF02[0]=& _tmpF01,Cyc_fprintf(Cyc_stderr,((
_tmpF03="%s::",_tag_dyneither(_tmpF03,sizeof(char),5))),_tag_dyneither(_tmpF02,
sizeof(void*),1)))))));}{const char*_tmpF07;void*_tmpF06[1];struct Cyc_String_pa_struct
_tmpF05;(_tmpF05.tag=0,((_tmpF05.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp83D),((_tmpF06[0]=& _tmpF05,Cyc_fprintf(Cyc_stderr,((_tmpF07="%s::",
_tag_dyneither(_tmpF07,sizeof(char),5))),_tag_dyneither(_tmpF06,sizeof(void*),1)))))));}
goto _LL441;}_LL44A:;_LL44B:{const char*_tmpF0A;void*_tmpF09;(_tmpF09=0,Cyc_fprintf(
Cyc_stderr,((_tmpF0A="?",_tag_dyneither(_tmpF0A,sizeof(char),2))),_tag_dyneither(
_tmpF09,sizeof(void*),0)));}goto _LL441;_LL441:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
