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
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{
void*v;};extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_unique_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < 
dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,
void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat_l(
struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;
struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];
struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;struct Cyc_List_List*
Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};extern int Cyc_Position_use_gcc_style_location;
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n
Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(
struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*
ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;
struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct
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
tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;
};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_RefCntRgn_struct{
int tag;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
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
_union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int
tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};
struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6
val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int
tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;
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
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum Cyc_Absyn_Scope
sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*
fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*);extern void*Cyc_Absyn_sint_typ;void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
union Cyc_Absyn_Constraint*zero_term);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int
add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;
int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct
Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{
char*tag;};struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));int Cyc_Dict_member(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t);struct _tuple10{void*f1;void*f2;};struct
Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_struct{char*
tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;
struct Cyc_Absyn_Datatypefield*field;};struct Cyc_Position_Segment{int start;int end;
};struct Cyc_Port_Edit{struct Cyc_Position_Segment*loc;struct _dyneither_ptr
old_string;struct _dyneither_ptr new_string;};struct Cyc_Port_Edit;struct Cyc_Port_Edit;
struct Cyc_Port_Edit;struct Cyc_Port_Edit;int Cyc_Port_cmp_edit(struct Cyc_Port_Edit*
e1,struct Cyc_Port_Edit*e2){if((unsigned int)e1->loc  && (unsigned int)e2->loc)
return((struct Cyc_Position_Segment*)_check_null(e1->loc))->start - ((struct Cyc_Position_Segment*)
_check_null(e2->loc))->start;else{if(e1->loc == e2->loc)return 0;else{if((
unsigned int)e1->loc)return 1;else{return - 1;}}}}struct Cyc_Port_Cvar{int id;void**
eq;struct Cyc_List_List*uppers;struct Cyc_List_List*lowers;};struct Cyc_Port_Cfield{
void*qual;struct _dyneither_ptr*f;void*type;};struct Cyc_Port_Const_ct_struct{int
tag;};struct Cyc_Port_Notconst_ct_struct{int tag;};struct Cyc_Port_Thin_ct_struct{
int tag;};struct Cyc_Port_Fat_ct_struct{int tag;};struct Cyc_Port_Void_ct_struct{int
tag;};struct Cyc_Port_Zero_ct_struct{int tag;};struct Cyc_Port_Arith_ct_struct{int
tag;};struct Cyc_Port_Heap_ct_struct{int tag;};struct Cyc_Port_Zterm_ct_struct{int
tag;};struct Cyc_Port_Nozterm_ct_struct{int tag;};struct Cyc_Port_RgnVar_ct_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Port_Ptr_ct_struct{int tag;void*f1;
void*f2;void*f3;void*f4;void*f5;};struct Cyc_Port_Array_ct_struct{int tag;void*f1;
void*f2;void*f3;};struct _tuple11{struct Cyc_Absyn_Aggrdecl*f1;struct Cyc_List_List*
f2;};struct Cyc_Port_KnownAggr_ct_struct{int tag;struct _tuple11*f1;};struct Cyc_Port_UnknownAggr_ct_struct{
int tag;struct Cyc_List_List*f1;void**f2;};struct Cyc_Port_Fn_ct_struct{int tag;void*
f1;struct Cyc_List_List*f2;};struct Cyc_Port_Cvar;struct Cyc_Port_Var_ct_struct{int
tag;struct Cyc_Port_Cvar*f1;};struct Cyc_Port_Const_ct_struct Cyc_Port_Const_ct_val={
0};struct Cyc_Port_Notconst_ct_struct Cyc_Port_Notconst_ct_val={1};struct Cyc_Port_Thin_ct_struct
Cyc_Port_Thin_ct_val={2};struct Cyc_Port_Fat_ct_struct Cyc_Port_Fat_ct_val={3};
struct Cyc_Port_Void_ct_struct Cyc_Port_Void_ct_val={4};struct Cyc_Port_Zero_ct_struct
Cyc_Port_Zero_ct_val={5};struct Cyc_Port_Arith_ct_struct Cyc_Port_Arith_ct_val={6};
struct Cyc_Port_Heap_ct_struct Cyc_Port_Heap_ct_val={7};struct Cyc_Port_Zterm_ct_struct
Cyc_Port_Zterm_ct_val={8};struct Cyc_Port_Nozterm_ct_struct Cyc_Port_Nozterm_ct_val={
9};static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*
ts);static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*
ts);struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;static struct _dyneither_ptr Cyc_Port_ctype2string(
int deep,void*t){void*_tmpA=t;struct _dyneither_ptr*_tmp16;void*_tmp18;void*_tmp19;
void*_tmp1A;void*_tmp1B;void*_tmp1C;void*_tmp1E;void*_tmp1F;void*_tmp20;struct
_tuple11*_tmp22;struct _tuple11 _tmp23;struct Cyc_Absyn_Aggrdecl*_tmp24;struct Cyc_List_List*
_tmp25;struct Cyc_List_List*_tmp27;void**_tmp28;void*_tmp29;struct Cyc_List_List*
_tmp2B;void*_tmp2D;struct Cyc_List_List*_tmp2E;struct Cyc_Port_Cvar*_tmp30;_LL1: {
struct Cyc_Port_Const_ct_struct*_tmpB=(struct Cyc_Port_Const_ct_struct*)_tmpA;if(
_tmpB->tag != 0)goto _LL3;}_LL2: {const char*_tmp53C;return(_tmp53C="const",
_tag_dyneither(_tmp53C,sizeof(char),6));}_LL3: {struct Cyc_Port_Notconst_ct_struct*
_tmpC=(struct Cyc_Port_Notconst_ct_struct*)_tmpA;if(_tmpC->tag != 1)goto _LL5;}_LL4: {
const char*_tmp53D;return(_tmp53D="notconst",_tag_dyneither(_tmp53D,sizeof(char),
9));}_LL5: {struct Cyc_Port_Thin_ct_struct*_tmpD=(struct Cyc_Port_Thin_ct_struct*)
_tmpA;if(_tmpD->tag != 2)goto _LL7;}_LL6: {const char*_tmp53E;return(_tmp53E="thin",
_tag_dyneither(_tmp53E,sizeof(char),5));}_LL7: {struct Cyc_Port_Fat_ct_struct*
_tmpE=(struct Cyc_Port_Fat_ct_struct*)_tmpA;if(_tmpE->tag != 3)goto _LL9;}_LL8: {
const char*_tmp53F;return(_tmp53F="fat",_tag_dyneither(_tmp53F,sizeof(char),4));}
_LL9: {struct Cyc_Port_Void_ct_struct*_tmpF=(struct Cyc_Port_Void_ct_struct*)_tmpA;
if(_tmpF->tag != 4)goto _LLB;}_LLA: {const char*_tmp540;return(_tmp540="void",
_tag_dyneither(_tmp540,sizeof(char),5));}_LLB: {struct Cyc_Port_Zero_ct_struct*
_tmp10=(struct Cyc_Port_Zero_ct_struct*)_tmpA;if(_tmp10->tag != 5)goto _LLD;}_LLC: {
const char*_tmp541;return(_tmp541="zero",_tag_dyneither(_tmp541,sizeof(char),5));}
_LLD: {struct Cyc_Port_Arith_ct_struct*_tmp11=(struct Cyc_Port_Arith_ct_struct*)
_tmpA;if(_tmp11->tag != 6)goto _LLF;}_LLE: {const char*_tmp542;return(_tmp542="arith",
_tag_dyneither(_tmp542,sizeof(char),6));}_LLF: {struct Cyc_Port_Heap_ct_struct*
_tmp12=(struct Cyc_Port_Heap_ct_struct*)_tmpA;if(_tmp12->tag != 7)goto _LL11;}_LL10: {
const char*_tmp543;return(_tmp543="heap",_tag_dyneither(_tmp543,sizeof(char),5));}
_LL11: {struct Cyc_Port_Zterm_ct_struct*_tmp13=(struct Cyc_Port_Zterm_ct_struct*)
_tmpA;if(_tmp13->tag != 8)goto _LL13;}_LL12: {const char*_tmp544;return(_tmp544="ZT",
_tag_dyneither(_tmp544,sizeof(char),3));}_LL13: {struct Cyc_Port_Nozterm_ct_struct*
_tmp14=(struct Cyc_Port_Nozterm_ct_struct*)_tmpA;if(_tmp14->tag != 9)goto _LL15;}
_LL14: {const char*_tmp545;return(_tmp545="NZT",_tag_dyneither(_tmp545,sizeof(
char),4));}_LL15: {struct Cyc_Port_RgnVar_ct_struct*_tmp15=(struct Cyc_Port_RgnVar_ct_struct*)
_tmpA;if(_tmp15->tag != 10)goto _LL17;else{_tmp16=_tmp15->f1;}}_LL16: {const char*
_tmp549;void*_tmp548[1];struct Cyc_String_pa_struct _tmp547;return(struct
_dyneither_ptr)((_tmp547.tag=0,((_tmp547.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp16),((_tmp548[0]=& _tmp547,Cyc_aprintf(((_tmp549="%s",
_tag_dyneither(_tmp549,sizeof(char),3))),_tag_dyneither(_tmp548,sizeof(void*),1))))))));}
_LL17: {struct Cyc_Port_Ptr_ct_struct*_tmp17=(struct Cyc_Port_Ptr_ct_struct*)_tmpA;
if(_tmp17->tag != 11)goto _LL19;else{_tmp18=(void*)_tmp17->f1;_tmp19=(void*)_tmp17->f2;
_tmp1A=(void*)_tmp17->f3;_tmp1B=(void*)_tmp17->f4;_tmp1C=(void*)_tmp17->f5;}}
_LL18: {const char*_tmp551;void*_tmp550[5];struct Cyc_String_pa_struct _tmp54F;
struct Cyc_String_pa_struct _tmp54E;struct Cyc_String_pa_struct _tmp54D;struct Cyc_String_pa_struct
_tmp54C;struct Cyc_String_pa_struct _tmp54B;return(struct _dyneither_ptr)((_tmp54B.tag=
0,((_tmp54B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,_tmp1C)),((_tmp54C.tag=0,((_tmp54C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp1B)),((_tmp54D.tag=0,((_tmp54D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp1A)),((
_tmp54E.tag=0,((_tmp54E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,_tmp19)),((_tmp54F.tag=0,((_tmp54F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp18)),((_tmp550[0]=& _tmp54F,((
_tmp550[1]=& _tmp54E,((_tmp550[2]=& _tmp54D,((_tmp550[3]=& _tmp54C,((_tmp550[4]=&
_tmp54B,Cyc_aprintf(((_tmp551="ptr(%s,%s,%s,%s,%s)",_tag_dyneither(_tmp551,
sizeof(char),20))),_tag_dyneither(_tmp550,sizeof(void*),5))))))))))))))))))))))))))))))));}
_LL19: {struct Cyc_Port_Array_ct_struct*_tmp1D=(struct Cyc_Port_Array_ct_struct*)
_tmpA;if(_tmp1D->tag != 12)goto _LL1B;else{_tmp1E=(void*)_tmp1D->f1;_tmp1F=(void*)
_tmp1D->f2;_tmp20=(void*)_tmp1D->f3;}}_LL1A: {const char*_tmp557;void*_tmp556[3];
struct Cyc_String_pa_struct _tmp555;struct Cyc_String_pa_struct _tmp554;struct Cyc_String_pa_struct
_tmp553;return(struct _dyneither_ptr)((_tmp553.tag=0,((_tmp553.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp20)),((
_tmp554.tag=0,((_tmp554.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,_tmp1F)),((_tmp555.tag=0,((_tmp555.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp1E)),((_tmp556[0]=& _tmp555,((
_tmp556[1]=& _tmp554,((_tmp556[2]=& _tmp553,Cyc_aprintf(((_tmp557="array(%s,%s,%s)",
_tag_dyneither(_tmp557,sizeof(char),16))),_tag_dyneither(_tmp556,sizeof(void*),3))))))))))))))))))));}
_LL1B: {struct Cyc_Port_KnownAggr_ct_struct*_tmp21=(struct Cyc_Port_KnownAggr_ct_struct*)
_tmpA;if(_tmp21->tag != 13)goto _LL1D;else{_tmp22=_tmp21->f1;_tmp23=*_tmp22;_tmp24=
_tmp23.f1;_tmp25=_tmp23.f2;}}_LL1C: {const char*_tmp559;const char*_tmp558;struct
_dyneither_ptr s=_tmp24->kind == Cyc_Absyn_StructA?(_tmp559="struct",
_tag_dyneither(_tmp559,sizeof(char),7)):((_tmp558="union",_tag_dyneither(_tmp558,
sizeof(char),6)));if(!deep){const char*_tmp55E;void*_tmp55D[2];struct Cyc_String_pa_struct
_tmp55C;struct Cyc_String_pa_struct _tmp55B;return(struct _dyneither_ptr)((_tmp55B.tag=
0,((_tmp55B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp24->name)),((_tmp55C.tag=0,((_tmp55C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp55D[0]=& _tmp55C,((_tmp55D[1]=& _tmp55B,Cyc_aprintf(((
_tmp55E="%s %s",_tag_dyneither(_tmp55E,sizeof(char),6))),_tag_dyneither(_tmp55D,
sizeof(void*),2))))))))))))));}else{const char*_tmp564;void*_tmp563[3];struct Cyc_String_pa_struct
_tmp562;struct Cyc_String_pa_struct _tmp561;struct Cyc_String_pa_struct _tmp560;
return(struct _dyneither_ptr)((_tmp560.tag=0,((_tmp560.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Port_cfields2string(0,_tmp25)),((_tmp561.tag=0,((
_tmp561.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp24->name)),((_tmp562.tag=0,((_tmp562.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp563[0]=& _tmp562,((_tmp563[1]=& _tmp561,((_tmp563[2]=&
_tmp560,Cyc_aprintf(((_tmp564="%s %s {%s}",_tag_dyneither(_tmp564,sizeof(char),
11))),_tag_dyneither(_tmp563,sizeof(void*),3))))))))))))))))))));}}_LL1D: {
struct Cyc_Port_UnknownAggr_ct_struct*_tmp26=(struct Cyc_Port_UnknownAggr_ct_struct*)
_tmpA;if(_tmp26->tag != 14)goto _LL1F;else{_tmp27=_tmp26->f1;_tmp28=_tmp26->f2;if(
_tmp28 == 0)goto _LL1F;_tmp29=*_tmp28;}}_LL1E: return Cyc_Port_ctype2string(deep,
_tmp29);_LL1F: {struct Cyc_Port_UnknownAggr_ct_struct*_tmp2A=(struct Cyc_Port_UnknownAggr_ct_struct*)
_tmpA;if(_tmp2A->tag != 14)goto _LL21;else{_tmp2B=_tmp2A->f1;}}_LL20: {const char*
_tmp568;void*_tmp567[1];struct Cyc_String_pa_struct _tmp566;return(struct
_dyneither_ptr)((_tmp566.tag=0,((_tmp566.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_cfields2string(deep,_tmp2B)),((_tmp567[0]=& _tmp566,Cyc_aprintf(((
_tmp568="aggr {%s}",_tag_dyneither(_tmp568,sizeof(char),10))),_tag_dyneither(
_tmp567,sizeof(void*),1))))))));}_LL21: {struct Cyc_Port_Fn_ct_struct*_tmp2C=(
struct Cyc_Port_Fn_ct_struct*)_tmpA;if(_tmp2C->tag != 15)goto _LL23;else{_tmp2D=(
void*)_tmp2C->f1;_tmp2E=_tmp2C->f2;}}_LL22: {const char*_tmp56D;void*_tmp56C[2];
struct Cyc_String_pa_struct _tmp56B;struct Cyc_String_pa_struct _tmp56A;return(
struct _dyneither_ptr)((_tmp56A.tag=0,((_tmp56A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp2D)),((_tmp56B.tag=0,((_tmp56B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(deep,_tmp2E)),((
_tmp56C[0]=& _tmp56B,((_tmp56C[1]=& _tmp56A,Cyc_aprintf(((_tmp56D="fn(%s)->%s",
_tag_dyneither(_tmp56D,sizeof(char),11))),_tag_dyneither(_tmp56C,sizeof(void*),2))))))))))))));}
_LL23: {struct Cyc_Port_Var_ct_struct*_tmp2F=(struct Cyc_Port_Var_ct_struct*)_tmpA;
if(_tmp2F->tag != 16)goto _LL0;else{_tmp30=_tmp2F->f1;}}_LL24: if((unsigned int)
_tmp30->eq)return Cyc_Port_ctype2string(deep,*((void**)_check_null(_tmp30->eq)));
else{if(!deep  || _tmp30->uppers == 0  && _tmp30->lowers == 0){const char*_tmp571;
void*_tmp570[1];struct Cyc_Int_pa_struct _tmp56F;return(struct _dyneither_ptr)((
_tmp56F.tag=1,((_tmp56F.f1=(unsigned long)_tmp30->id,((_tmp570[0]=& _tmp56F,Cyc_aprintf(((
_tmp571="var(%d)",_tag_dyneither(_tmp571,sizeof(char),8))),_tag_dyneither(
_tmp570,sizeof(void*),1))))))));}else{const char*_tmp577;void*_tmp576[3];struct
Cyc_String_pa_struct _tmp575;struct Cyc_Int_pa_struct _tmp574;struct Cyc_String_pa_struct
_tmp573;return(struct _dyneither_ptr)((_tmp573.tag=0,((_tmp573.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(0,_tmp30->uppers)),((
_tmp574.tag=1,((_tmp574.f1=(unsigned long)_tmp30->id,((_tmp575.tag=0,((_tmp575.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(0,_tmp30->lowers)),((
_tmp576[0]=& _tmp575,((_tmp576[1]=& _tmp574,((_tmp576[2]=& _tmp573,Cyc_aprintf(((
_tmp577="var([%s]<=%d<=[%s])",_tag_dyneither(_tmp577,sizeof(char),20))),
_tag_dyneither(_tmp576,sizeof(void*),3))))))))))))))))))));}}_LL0:;}static struct
_dyneither_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){struct _dyneither_ptr*
_tmp578;return(_tmp578=_cycalloc(sizeof(*_tmp578)),((_tmp578[0]=Cyc_Port_ctype2string(
deep,t),_tmp578)));}struct Cyc_List_List*Cyc_Port_sep(struct _dyneither_ptr s,
struct Cyc_List_List*xs){struct _dyneither_ptr*_tmp579;struct _dyneither_ptr*_tmp65=(
_tmp579=_cycalloc(sizeof(*_tmp579)),((_tmp579[0]=s,_tmp579)));if(xs == 0)return xs;{
struct Cyc_List_List*_tmp66=xs;struct Cyc_List_List*_tmp67=xs->tl;for(0;_tmp67 != 0;(
_tmp66=_tmp67,_tmp67=_tmp67->tl)){struct Cyc_List_List*_tmp57A;((struct Cyc_List_List*)
_check_null(_tmp66))->tl=((_tmp57A=_cycalloc(sizeof(*_tmp57A)),((_tmp57A->hd=
_tmp65,((_tmp57A->tl=_tmp67,_tmp57A))))));}return xs;};}struct Cyc_Port_Cfield;
struct Cyc_Port_Cfield;static struct _dyneither_ptr*Cyc_Port_cfield2stringptr(int
deep,struct Cyc_Port_Cfield*f){const char*_tmp580;void*_tmp57F[3];struct Cyc_String_pa_struct
_tmp57E;struct Cyc_String_pa_struct _tmp57D;struct Cyc_String_pa_struct _tmp57C;
struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp57C.tag=0,((_tmp57C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,f->type)),((
_tmp57D.tag=0,((_tmp57D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f->f),((
_tmp57E.tag=0,((_tmp57E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,f->qual)),((_tmp57F[0]=& _tmp57E,((_tmp57F[1]=& _tmp57D,((_tmp57F[2]=& _tmp57C,
Cyc_aprintf(((_tmp580="%s %s: %s",_tag_dyneither(_tmp580,sizeof(char),10))),
_tag_dyneither(_tmp57F,sizeof(void*),3))))))))))))))))))));struct _dyneither_ptr*
_tmp581;return(_tmp581=_cycalloc(sizeof(*_tmp581)),((_tmp581[0]=s,_tmp581)));}
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){
const char*_tmp582;return(struct _dyneither_ptr)Cyc_strconcat_l(Cyc_Port_sep(((
_tmp582=",",_tag_dyneither(_tmp582,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Port_ctype2stringptr,deep,ts)));}struct Cyc_Port_Cfield;static struct
_dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){const char*
_tmp583;return(struct _dyneither_ptr)Cyc_strconcat_l(Cyc_Port_sep(((_tmp583=";",
_tag_dyneither(_tmp583,sizeof(char),2))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr*(*f)(int,struct Cyc_Port_Cfield*),int env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Port_cfield2stringptr,deep,fs)));}static void*Cyc_Port_notconst_ct(){
return(void*)& Cyc_Port_Notconst_ct_val;}static void*Cyc_Port_const_ct(){return(
void*)& Cyc_Port_Const_ct_val;}static void*Cyc_Port_thin_ct(){return(void*)& Cyc_Port_Thin_ct_val;}
static void*Cyc_Port_fat_ct(){return(void*)& Cyc_Port_Fat_ct_val;}static void*Cyc_Port_void_ct(){
return(void*)& Cyc_Port_Void_ct_val;}static void*Cyc_Port_zero_ct(){return(void*)&
Cyc_Port_Zero_ct_val;}static void*Cyc_Port_arith_ct(){return(void*)& Cyc_Port_Arith_ct_val;}
static void*Cyc_Port_heap_ct(){return(void*)& Cyc_Port_Heap_ct_val;}static void*Cyc_Port_zterm_ct(){
return(void*)& Cyc_Port_Zterm_ct_val;}static void*Cyc_Port_nozterm_ct(){return(
void*)& Cyc_Port_Nozterm_ct_val;}static void*Cyc_Port_rgnvar_ct(struct
_dyneither_ptr*n){struct Cyc_Port_RgnVar_ct_struct _tmp586;struct Cyc_Port_RgnVar_ct_struct*
_tmp585;return(void*)((_tmp585=_cycalloc(sizeof(*_tmp585)),((_tmp585[0]=((
_tmp586.tag=10,((_tmp586.f1=n,_tmp586)))),_tmp585))));}static void*Cyc_Port_unknown_aggr_ct(
struct Cyc_List_List*fs){struct Cyc_Port_UnknownAggr_ct_struct _tmp589;struct Cyc_Port_UnknownAggr_ct_struct*
_tmp588;return(void*)((_tmp588=_cycalloc(sizeof(*_tmp588)),((_tmp588[0]=((
_tmp589.tag=14,((_tmp589.f1=fs,((_tmp589.f2=0,_tmp589)))))),_tmp588))));}static
void*Cyc_Port_known_aggr_ct(struct _tuple11*p){struct Cyc_Port_KnownAggr_ct_struct
_tmp58C;struct Cyc_Port_KnownAggr_ct_struct*_tmp58B;return(void*)((_tmp58B=
_cycalloc(sizeof(*_tmp58B)),((_tmp58B[0]=((_tmp58C.tag=13,((_tmp58C.f1=p,_tmp58C)))),
_tmp58B))));}static void*Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind,void*r,
void*zt){struct Cyc_Port_Ptr_ct_struct _tmp58F;struct Cyc_Port_Ptr_ct_struct*
_tmp58E;return(void*)((_tmp58E=_cycalloc(sizeof(*_tmp58E)),((_tmp58E[0]=((
_tmp58F.tag=11,((_tmp58F.f1=(void*)elt,((_tmp58F.f2=(void*)qual,((_tmp58F.f3=(
void*)ptr_kind,((_tmp58F.f4=(void*)r,((_tmp58F.f5=(void*)zt,_tmp58F)))))))))))),
_tmp58E))));}static void*Cyc_Port_array_ct(void*elt,void*qual,void*zt){struct Cyc_Port_Array_ct_struct
_tmp592;struct Cyc_Port_Array_ct_struct*_tmp591;return(void*)((_tmp591=_cycalloc(
sizeof(*_tmp591)),((_tmp591[0]=((_tmp592.tag=12,((_tmp592.f1=(void*)elt,((
_tmp592.f2=(void*)qual,((_tmp592.f3=(void*)zt,_tmp592)))))))),_tmp591))));}
static void*Cyc_Port_fn_ct(void*return_type,struct Cyc_List_List*args){struct Cyc_Port_Fn_ct_struct
_tmp595;struct Cyc_Port_Fn_ct_struct*_tmp594;return(void*)((_tmp594=_cycalloc(
sizeof(*_tmp594)),((_tmp594[0]=((_tmp595.tag=15,((_tmp595.f1=(void*)return_type,((
_tmp595.f2=args,_tmp595)))))),_tmp594))));}struct Cyc_Port_Cvar;static void*Cyc_Port_var(){
static int counter=0;struct Cyc_Port_Var_ct_struct _tmp59B;struct Cyc_Port_Cvar*
_tmp59A;struct Cyc_Port_Var_ct_struct*_tmp599;return(void*)((_tmp599=_cycalloc(
sizeof(*_tmp599)),((_tmp599[0]=((_tmp59B.tag=16,((_tmp59B.f1=((_tmp59A=_cycalloc(
sizeof(*_tmp59A)),((_tmp59A->id=counter ++,((_tmp59A->eq=0,((_tmp59A->uppers=0,((
_tmp59A->lowers=0,_tmp59A)))))))))),_tmp59B)))),_tmp599))));}static void*Cyc_Port_new_var(
void*x){return Cyc_Port_var();}static struct _dyneither_ptr*Cyc_Port_new_region_var(){
static int counter=0;const char*_tmp59F;void*_tmp59E[1];struct Cyc_Int_pa_struct
_tmp59D;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp59D.tag=1,((_tmp59D.f1=(
unsigned long)counter ++,((_tmp59E[0]=& _tmp59D,Cyc_aprintf(((_tmp59F="`R%d",
_tag_dyneither(_tmp59F,sizeof(char),5))),_tag_dyneither(_tmp59E,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp5A0;return(_tmp5A0=_cycalloc(sizeof(*_tmp5A0)),((
_tmp5A0[0]=s,_tmp5A0)));}static int Cyc_Port_unifies(void*t1,void*t2);struct Cyc_Port_Cvar;
static void*Cyc_Port_compress_ct(void*t){void*_tmp85=t;struct Cyc_Port_Cvar*_tmp87;
struct Cyc_Port_Cvar _tmp88;void**_tmp89;void***_tmp8A;struct Cyc_List_List*_tmp8B;
struct Cyc_List_List*_tmp8C;void**_tmp8E;_LL26: {struct Cyc_Port_Var_ct_struct*
_tmp86=(struct Cyc_Port_Var_ct_struct*)_tmp85;if(_tmp86->tag != 16)goto _LL28;else{
_tmp87=_tmp86->f1;_tmp88=*_tmp87;_tmp89=_tmp88.eq;_tmp8A=(void***)&(*_tmp86->f1).eq;
_tmp8B=_tmp88.uppers;_tmp8C=_tmp88.lowers;}}_LL27: {void**_tmp8F=*_tmp8A;if((
unsigned int)_tmp8F){void*r=Cyc_Port_compress_ct(*_tmp8F);if(*_tmp8F != r){void**
_tmp5A1;*_tmp8A=((_tmp5A1=_cycalloc(sizeof(*_tmp5A1)),((_tmp5A1[0]=r,_tmp5A1))));}
return r;}for(0;_tmp8C != 0;_tmp8C=_tmp8C->tl){void*_tmp91=(void*)_tmp8C->hd;_LL2D: {
struct Cyc_Port_Const_ct_struct*_tmp92=(struct Cyc_Port_Const_ct_struct*)_tmp91;
if(_tmp92->tag != 0)goto _LL2F;}_LL2E: goto _LL30;_LL2F: {struct Cyc_Port_Nozterm_ct_struct*
_tmp93=(struct Cyc_Port_Nozterm_ct_struct*)_tmp91;if(_tmp93->tag != 9)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Port_Void_ct_struct*_tmp94=(struct Cyc_Port_Void_ct_struct*)
_tmp91;if(_tmp94->tag != 4)goto _LL33;}_LL32: goto _LL34;_LL33: {struct Cyc_Port_KnownAggr_ct_struct*
_tmp95=(struct Cyc_Port_KnownAggr_ct_struct*)_tmp91;if(_tmp95->tag != 13)goto _LL35;}
_LL34: goto _LL36;_LL35: {struct Cyc_Port_Fn_ct_struct*_tmp96=(struct Cyc_Port_Fn_ct_struct*)
_tmp91;if(_tmp96->tag != 15)goto _LL37;}_LL36:{void**_tmp5A2;*_tmp8A=((_tmp5A2=
_cycalloc(sizeof(*_tmp5A2)),((_tmp5A2[0]=(void*)_tmp8C->hd,_tmp5A2))));}return(
void*)_tmp8C->hd;_LL37:;_LL38: goto _LL2C;_LL2C:;}for(0;_tmp8B != 0;_tmp8B=_tmp8B->tl){
void*_tmp98=(void*)_tmp8B->hd;_LL3A: {struct Cyc_Port_Notconst_ct_struct*_tmp99=(
struct Cyc_Port_Notconst_ct_struct*)_tmp98;if(_tmp99->tag != 1)goto _LL3C;}_LL3B:
goto _LL3D;_LL3C: {struct Cyc_Port_Zterm_ct_struct*_tmp9A=(struct Cyc_Port_Zterm_ct_struct*)
_tmp98;if(_tmp9A->tag != 8)goto _LL3E;}_LL3D: goto _LL3F;_LL3E: {struct Cyc_Port_Zero_ct_struct*
_tmp9B=(struct Cyc_Port_Zero_ct_struct*)_tmp98;if(_tmp9B->tag != 5)goto _LL40;}
_LL3F: goto _LL41;_LL40: {struct Cyc_Port_KnownAggr_ct_struct*_tmp9C=(struct Cyc_Port_KnownAggr_ct_struct*)
_tmp98;if(_tmp9C->tag != 13)goto _LL42;}_LL41: goto _LL43;_LL42: {struct Cyc_Port_Fn_ct_struct*
_tmp9D=(struct Cyc_Port_Fn_ct_struct*)_tmp98;if(_tmp9D->tag != 15)goto _LL44;}_LL43:{
void**_tmp5A3;*_tmp8A=((_tmp5A3=_cycalloc(sizeof(*_tmp5A3)),((_tmp5A3[0]=(void*)
_tmp8B->hd,_tmp5A3))));}return(void*)_tmp8B->hd;_LL44:;_LL45: goto _LL39;_LL39:;}
return t;}_LL28: {struct Cyc_Port_UnknownAggr_ct_struct*_tmp8D=(struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp85;if(_tmp8D->tag != 14)goto _LL2A;else{_tmp8E=_tmp8D->f2;}}_LL29: if((
unsigned int)_tmp8E)return Cyc_Port_compress_ct(*_tmp8E);else{return t;}_LL2A:;
_LL2B: return t;_LL25:;}static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*
t){void*_tmp9F=Cyc_Port_compress_ct(t);struct _dyneither_ptr*_tmpAE;void*_tmpB0;
void*_tmpB1;void*_tmpB2;void*_tmpB3;void*_tmpB4;void*_tmpB6;void*_tmpB7;void*
_tmpB8;void*_tmpBA;struct Cyc_List_List*_tmpBB;_LL47: {struct Cyc_Port_Const_ct_struct*
_tmpA0=(struct Cyc_Port_Const_ct_struct*)_tmp9F;if(_tmpA0->tag != 0)goto _LL49;}
_LL48: goto _LL4A;_LL49: {struct Cyc_Port_Notconst_ct_struct*_tmpA1=(struct Cyc_Port_Notconst_ct_struct*)
_tmp9F;if(_tmpA1->tag != 1)goto _LL4B;}_LL4A: goto _LL4C;_LL4B: {struct Cyc_Port_Thin_ct_struct*
_tmpA2=(struct Cyc_Port_Thin_ct_struct*)_tmp9F;if(_tmpA2->tag != 2)goto _LL4D;}
_LL4C: goto _LL4E;_LL4D: {struct Cyc_Port_Fat_ct_struct*_tmpA3=(struct Cyc_Port_Fat_ct_struct*)
_tmp9F;if(_tmpA3->tag != 3)goto _LL4F;}_LL4E: goto _LL50;_LL4F: {struct Cyc_Port_Void_ct_struct*
_tmpA4=(struct Cyc_Port_Void_ct_struct*)_tmp9F;if(_tmpA4->tag != 4)goto _LL51;}
_LL50: goto _LL52;_LL51: {struct Cyc_Port_Zero_ct_struct*_tmpA5=(struct Cyc_Port_Zero_ct_struct*)
_tmp9F;if(_tmpA5->tag != 5)goto _LL53;}_LL52: goto _LL54;_LL53: {struct Cyc_Port_Arith_ct_struct*
_tmpA6=(struct Cyc_Port_Arith_ct_struct*)_tmp9F;if(_tmpA6->tag != 6)goto _LL55;}
_LL54: goto _LL56;_LL55: {struct Cyc_Port_Zterm_ct_struct*_tmpA7=(struct Cyc_Port_Zterm_ct_struct*)
_tmp9F;if(_tmpA7->tag != 8)goto _LL57;}_LL56: goto _LL58;_LL57: {struct Cyc_Port_Nozterm_ct_struct*
_tmpA8=(struct Cyc_Port_Nozterm_ct_struct*)_tmp9F;if(_tmpA8->tag != 9)goto _LL59;}
_LL58: goto _LL5A;_LL59: {struct Cyc_Port_UnknownAggr_ct_struct*_tmpA9=(struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp9F;if(_tmpA9->tag != 14)goto _LL5B;}_LL5A: goto _LL5C;_LL5B: {struct Cyc_Port_KnownAggr_ct_struct*
_tmpAA=(struct Cyc_Port_KnownAggr_ct_struct*)_tmp9F;if(_tmpAA->tag != 13)goto _LL5D;}
_LL5C: goto _LL5E;_LL5D: {struct Cyc_Port_Var_ct_struct*_tmpAB=(struct Cyc_Port_Var_ct_struct*)
_tmp9F;if(_tmpAB->tag != 16)goto _LL5F;}_LL5E: goto _LL60;_LL5F: {struct Cyc_Port_Heap_ct_struct*
_tmpAC=(struct Cyc_Port_Heap_ct_struct*)_tmp9F;if(_tmpAC->tag != 7)goto _LL61;}
_LL60: return t;_LL61: {struct Cyc_Port_RgnVar_ct_struct*_tmpAD=(struct Cyc_Port_RgnVar_ct_struct*)
_tmp9F;if(_tmpAD->tag != 10)goto _LL63;else{_tmpAE=_tmpAD->f1;}}_LL62: if(!((int(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*instenv,_tmpAE))*
instenv=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,
void*v))Cyc_Dict_insert)(*instenv,_tmpAE,Cyc_Port_var());return((void*(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*instenv,_tmpAE);_LL63: {
struct Cyc_Port_Ptr_ct_struct*_tmpAF=(struct Cyc_Port_Ptr_ct_struct*)_tmp9F;if(
_tmpAF->tag != 11)goto _LL65;else{_tmpB0=(void*)_tmpAF->f1;_tmpB1=(void*)_tmpAF->f2;
_tmpB2=(void*)_tmpAF->f3;_tmpB3=(void*)_tmpAF->f4;_tmpB4=(void*)_tmpAF->f5;}}
_LL64: {void*_tmpBE;void*_tmpBF;struct _tuple10 _tmp5A4;struct _tuple10 _tmpBD=(
_tmp5A4.f1=Cyc_Port_inst(instenv,_tmpB0),((_tmp5A4.f2=Cyc_Port_inst(instenv,
_tmpB3),_tmp5A4)));_tmpBE=_tmpBD.f1;_tmpBF=_tmpBD.f2;if(_tmpBE == _tmpB0  && 
_tmpBF == _tmpB3)return t;{struct Cyc_Port_Ptr_ct_struct _tmp5A7;struct Cyc_Port_Ptr_ct_struct*
_tmp5A6;return(void*)((_tmp5A6=_cycalloc(sizeof(*_tmp5A6)),((_tmp5A6[0]=((
_tmp5A7.tag=11,((_tmp5A7.f1=(void*)_tmpBE,((_tmp5A7.f2=(void*)_tmpB1,((_tmp5A7.f3=(
void*)_tmpB2,((_tmp5A7.f4=(void*)_tmpBF,((_tmp5A7.f5=(void*)_tmpB4,_tmp5A7)))))))))))),
_tmp5A6))));};}_LL65: {struct Cyc_Port_Array_ct_struct*_tmpB5=(struct Cyc_Port_Array_ct_struct*)
_tmp9F;if(_tmpB5->tag != 12)goto _LL67;else{_tmpB6=(void*)_tmpB5->f1;_tmpB7=(void*)
_tmpB5->f2;_tmpB8=(void*)_tmpB5->f3;}}_LL66: {void*_tmpC2=Cyc_Port_inst(instenv,
_tmpB6);if(_tmpC2 == _tmpB6)return t;{struct Cyc_Port_Array_ct_struct _tmp5AA;struct
Cyc_Port_Array_ct_struct*_tmp5A9;return(void*)((_tmp5A9=_cycalloc(sizeof(*
_tmp5A9)),((_tmp5A9[0]=((_tmp5AA.tag=12,((_tmp5AA.f1=(void*)_tmpC2,((_tmp5AA.f2=(
void*)_tmpB7,((_tmp5AA.f3=(void*)_tmpB8,_tmp5AA)))))))),_tmp5A9))));};}_LL67: {
struct Cyc_Port_Fn_ct_struct*_tmpB9=(struct Cyc_Port_Fn_ct_struct*)_tmp9F;if(
_tmpB9->tag != 15)goto _LL46;else{_tmpBA=(void*)_tmpB9->f1;_tmpBB=_tmpB9->f2;}}
_LL68: {struct Cyc_Port_Fn_ct_struct _tmp5AD;struct Cyc_Port_Fn_ct_struct*_tmp5AC;
return(void*)((_tmp5AC=_cycalloc(sizeof(*_tmp5AC)),((_tmp5AC[0]=((_tmp5AD.tag=15,((
_tmp5AD.f1=(void*)Cyc_Port_inst(instenv,_tmpBA),((_tmp5AD.f2=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Port_inst,instenv,_tmpBB),_tmp5AD)))))),_tmp5AC))));}
_LL46:;}void*Cyc_Port_instantiate(void*t){struct Cyc_Dict_Dict*_tmp5AE;return Cyc_Port_inst(((
_tmp5AE=_cycalloc(sizeof(*_tmp5AE)),((_tmp5AE[0]=((struct Cyc_Dict_Dict(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),
_tmp5AE)))),t);}struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*
ts){int found=0;{struct Cyc_List_List*_tmpC8=ts;for(0;(unsigned int)_tmpC8;_tmpC8=
_tmpC8->tl){void*_tmpC9=Cyc_Port_compress_ct((void*)_tmpC8->hd);if(t == _tmpC9)
found=1;}}if(!found)return ts;{struct Cyc_List_List*res=0;for(0;ts != 0;ts=ts->tl){
if(t == Cyc_Port_compress_ct((void*)ts->hd))continue;{struct Cyc_List_List*_tmp5AF;
res=((_tmp5AF=_cycalloc(sizeof(*_tmp5AF)),((_tmp5AF->hd=(void*)ts->hd,((_tmp5AF->tl=
res,_tmp5AF))))));};}return res;};}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;void
Cyc_Port_generalize(int is_rgn,void*t){t=Cyc_Port_compress_ct(t);{void*_tmpCB=t;
struct Cyc_Port_Cvar*_tmpCD;void*_tmpDC;void*_tmpDD;void*_tmpDE;void*_tmpDF;void*
_tmpE0;void*_tmpE2;void*_tmpE3;void*_tmpE4;void*_tmpE6;struct Cyc_List_List*
_tmpE7;_LL6A: {struct Cyc_Port_Var_ct_struct*_tmpCC=(struct Cyc_Port_Var_ct_struct*)
_tmpCB;if(_tmpCC->tag != 16)goto _LL6C;else{_tmpCD=_tmpCC->f1;}}_LL6B: _tmpCD->uppers=
Cyc_Port_filter_self(t,_tmpCD->uppers);_tmpCD->lowers=Cyc_Port_filter_self(t,
_tmpCD->lowers);if(is_rgn){if(_tmpCD->uppers == 0  && _tmpCD->lowers == 0){Cyc_Port_unifies(
t,Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var()));return;}if((unsigned int)_tmpCD->uppers){
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmpCD->uppers))->hd);
Cyc_Port_generalize(1,t);}else{Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)
_check_null(_tmpCD->lowers))->hd);Cyc_Port_generalize(1,t);}}return;_LL6C: {
struct Cyc_Port_Const_ct_struct*_tmpCE=(struct Cyc_Port_Const_ct_struct*)_tmpCB;
if(_tmpCE->tag != 0)goto _LL6E;}_LL6D: goto _LL6F;_LL6E: {struct Cyc_Port_Notconst_ct_struct*
_tmpCF=(struct Cyc_Port_Notconst_ct_struct*)_tmpCB;if(_tmpCF->tag != 1)goto _LL70;}
_LL6F: goto _LL71;_LL70: {struct Cyc_Port_Thin_ct_struct*_tmpD0=(struct Cyc_Port_Thin_ct_struct*)
_tmpCB;if(_tmpD0->tag != 2)goto _LL72;}_LL71: goto _LL73;_LL72: {struct Cyc_Port_Fat_ct_struct*
_tmpD1=(struct Cyc_Port_Fat_ct_struct*)_tmpCB;if(_tmpD1->tag != 3)goto _LL74;}_LL73:
goto _LL75;_LL74: {struct Cyc_Port_Void_ct_struct*_tmpD2=(struct Cyc_Port_Void_ct_struct*)
_tmpCB;if(_tmpD2->tag != 4)goto _LL76;}_LL75: goto _LL77;_LL76: {struct Cyc_Port_Zero_ct_struct*
_tmpD3=(struct Cyc_Port_Zero_ct_struct*)_tmpCB;if(_tmpD3->tag != 5)goto _LL78;}
_LL77: goto _LL79;_LL78: {struct Cyc_Port_Arith_ct_struct*_tmpD4=(struct Cyc_Port_Arith_ct_struct*)
_tmpCB;if(_tmpD4->tag != 6)goto _LL7A;}_LL79: goto _LL7B;_LL7A: {struct Cyc_Port_UnknownAggr_ct_struct*
_tmpD5=(struct Cyc_Port_UnknownAggr_ct_struct*)_tmpCB;if(_tmpD5->tag != 14)goto
_LL7C;}_LL7B: goto _LL7D;_LL7C: {struct Cyc_Port_KnownAggr_ct_struct*_tmpD6=(struct
Cyc_Port_KnownAggr_ct_struct*)_tmpCB;if(_tmpD6->tag != 13)goto _LL7E;}_LL7D: goto
_LL7F;_LL7E: {struct Cyc_Port_RgnVar_ct_struct*_tmpD7=(struct Cyc_Port_RgnVar_ct_struct*)
_tmpCB;if(_tmpD7->tag != 10)goto _LL80;}_LL7F: goto _LL81;_LL80: {struct Cyc_Port_Nozterm_ct_struct*
_tmpD8=(struct Cyc_Port_Nozterm_ct_struct*)_tmpCB;if(_tmpD8->tag != 9)goto _LL82;}
_LL81: goto _LL83;_LL82: {struct Cyc_Port_Zterm_ct_struct*_tmpD9=(struct Cyc_Port_Zterm_ct_struct*)
_tmpCB;if(_tmpD9->tag != 8)goto _LL84;}_LL83: goto _LL85;_LL84: {struct Cyc_Port_Heap_ct_struct*
_tmpDA=(struct Cyc_Port_Heap_ct_struct*)_tmpCB;if(_tmpDA->tag != 7)goto _LL86;}
_LL85: return;_LL86: {struct Cyc_Port_Ptr_ct_struct*_tmpDB=(struct Cyc_Port_Ptr_ct_struct*)
_tmpCB;if(_tmpDB->tag != 11)goto _LL88;else{_tmpDC=(void*)_tmpDB->f1;_tmpDD=(void*)
_tmpDB->f2;_tmpDE=(void*)_tmpDB->f3;_tmpDF=(void*)_tmpDB->f4;_tmpE0=(void*)
_tmpDB->f5;}}_LL87: Cyc_Port_generalize(0,_tmpDC);Cyc_Port_generalize(1,_tmpDF);
goto _LL69;_LL88: {struct Cyc_Port_Array_ct_struct*_tmpE1=(struct Cyc_Port_Array_ct_struct*)
_tmpCB;if(_tmpE1->tag != 12)goto _LL8A;else{_tmpE2=(void*)_tmpE1->f1;_tmpE3=(void*)
_tmpE1->f2;_tmpE4=(void*)_tmpE1->f3;}}_LL89: Cyc_Port_generalize(0,_tmpE2);Cyc_Port_generalize(
0,_tmpE3);goto _LL69;_LL8A: {struct Cyc_Port_Fn_ct_struct*_tmpE5=(struct Cyc_Port_Fn_ct_struct*)
_tmpCB;if(_tmpE5->tag != 15)goto _LL69;else{_tmpE6=(void*)_tmpE5->f1;_tmpE7=_tmpE5->f2;}}
_LL8B: Cyc_Port_generalize(0,_tmpE6);((void(*)(void(*f)(int,void*),int env,struct
Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Port_generalize,0,_tmpE7);goto _LL69;_LL69:;};}
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;static int Cyc_Port_occurs(void*v,void*
t){t=Cyc_Port_compress_ct(t);if(t == v)return 1;{void*_tmpE8=t;void*_tmpEA;void*
_tmpEB;void*_tmpEC;void*_tmpED;void*_tmpEE;void*_tmpF0;void*_tmpF1;void*_tmpF2;
void*_tmpF4;struct Cyc_List_List*_tmpF5;struct _tuple11*_tmpF7;struct _tuple11
_tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_List_List*_tmpFB;_LL8D: {struct Cyc_Port_Ptr_ct_struct*
_tmpE9=(struct Cyc_Port_Ptr_ct_struct*)_tmpE8;if(_tmpE9->tag != 11)goto _LL8F;else{
_tmpEA=(void*)_tmpE9->f1;_tmpEB=(void*)_tmpE9->f2;_tmpEC=(void*)_tmpE9->f3;
_tmpED=(void*)_tmpE9->f4;_tmpEE=(void*)_tmpE9->f5;}}_LL8E: return(((Cyc_Port_occurs(
v,_tmpEA) || Cyc_Port_occurs(v,_tmpEB)) || Cyc_Port_occurs(v,_tmpEC)) || Cyc_Port_occurs(
v,_tmpED)) || Cyc_Port_occurs(v,_tmpEE);_LL8F: {struct Cyc_Port_Array_ct_struct*
_tmpEF=(struct Cyc_Port_Array_ct_struct*)_tmpE8;if(_tmpEF->tag != 12)goto _LL91;
else{_tmpF0=(void*)_tmpEF->f1;_tmpF1=(void*)_tmpEF->f2;_tmpF2=(void*)_tmpEF->f3;}}
_LL90: return(Cyc_Port_occurs(v,_tmpF0) || Cyc_Port_occurs(v,_tmpF1)) || Cyc_Port_occurs(
v,_tmpF2);_LL91: {struct Cyc_Port_Fn_ct_struct*_tmpF3=(struct Cyc_Port_Fn_ct_struct*)
_tmpE8;if(_tmpF3->tag != 15)goto _LL93;else{_tmpF4=(void*)_tmpF3->f1;_tmpF5=_tmpF3->f2;}}
_LL92: if(Cyc_Port_occurs(v,_tmpF4))return 1;for(0;(unsigned int)_tmpF5;_tmpF5=
_tmpF5->tl){if(Cyc_Port_occurs(v,(void*)_tmpF5->hd))return 1;}return 0;_LL93: {
struct Cyc_Port_KnownAggr_ct_struct*_tmpF6=(struct Cyc_Port_KnownAggr_ct_struct*)
_tmpE8;if(_tmpF6->tag != 13)goto _LL95;else{_tmpF7=_tmpF6->f1;_tmpF8=*_tmpF7;
_tmpF9=_tmpF8.f2;}}_LL94: return 0;_LL95: {struct Cyc_Port_UnknownAggr_ct_struct*
_tmpFA=(struct Cyc_Port_UnknownAggr_ct_struct*)_tmpE8;if(_tmpFA->tag != 14)goto
_LL97;else{_tmpFB=_tmpFA->f1;}}_LL96: for(0;(unsigned int)_tmpFB;_tmpFB=_tmpFB->tl){
if(Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmpFB->hd)->qual) || Cyc_Port_occurs(
v,((struct Cyc_Port_Cfield*)_tmpFB->hd)->type))return 1;}return 0;_LL97:;_LL98:
return 0;_LL8C:;};}char Cyc_Port_Unify_ct[9]="Unify_ct";struct Cyc_Port_Unify_ct_struct{
char*tag;};struct Cyc_Port_Unify_ct_struct Cyc_Port_Unify_ct_val={Cyc_Port_Unify_ct};
static int Cyc_Port_leq(void*t1,void*t2);static void Cyc_Port_unify_cts(struct Cyc_List_List*
t1,struct Cyc_List_List*t2);static struct Cyc_List_List*Cyc_Port_merge_fields(
struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_subset);struct Cyc_Port_Cvar;
struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;static void Cyc_Port_unify_ct(
void*t1,void*t2){t1=Cyc_Port_compress_ct(t1);t2=Cyc_Port_compress_ct(t2);if(t1 == 
t2)return;{struct _tuple10 _tmp5B0;struct _tuple10 _tmpFE=(_tmp5B0.f1=t1,((_tmp5B0.f2=
t2,_tmp5B0)));void*_tmpFF;struct Cyc_Port_Cvar*_tmp101;void*_tmp102;struct Cyc_Port_Cvar*
_tmp104;void*_tmp105;void*_tmp107;void*_tmp108;void*_tmp109;void*_tmp10A;void*
_tmp10B;void*_tmp10C;void*_tmp10E;void*_tmp10F;void*_tmp110;void*_tmp111;void*
_tmp112;void*_tmp113;struct _dyneither_ptr*_tmp115;struct _dyneither_ptr _tmp116;
void*_tmp117;struct _dyneither_ptr*_tmp119;struct _dyneither_ptr _tmp11A;void*
_tmp11B;void*_tmp11D;void*_tmp11E;void*_tmp11F;void*_tmp120;void*_tmp122;void*
_tmp123;void*_tmp124;void*_tmp125;void*_tmp127;struct Cyc_List_List*_tmp128;void*
_tmp129;void*_tmp12B;struct Cyc_List_List*_tmp12C;void*_tmp12D;struct _tuple11*
_tmp12F;void*_tmp130;struct _tuple11*_tmp132;void*_tmp133;struct Cyc_List_List*
_tmp135;void**_tmp136;void***_tmp137;void*_tmp138;struct Cyc_List_List*_tmp13A;
void**_tmp13B;void***_tmp13C;void*_tmp13D;struct Cyc_List_List*_tmp13F;void**
_tmp140;void***_tmp141;void*_tmp142;struct _tuple11*_tmp144;struct _tuple11 _tmp145;
struct Cyc_Absyn_Aggrdecl*_tmp146;struct Cyc_List_List*_tmp147;void*_tmp148;struct
_tuple11*_tmp14A;struct _tuple11 _tmp14B;struct Cyc_Absyn_Aggrdecl*_tmp14C;struct
Cyc_List_List*_tmp14D;void*_tmp14E;struct Cyc_List_List*_tmp150;void**_tmp151;
void***_tmp152;_LL9A: _tmpFF=_tmpFE.f1;{struct Cyc_Port_Var_ct_struct*_tmp100=(
struct Cyc_Port_Var_ct_struct*)_tmpFF;if(_tmp100->tag != 16)goto _LL9C;else{_tmp101=
_tmp100->f1;}};_LL9B: if(!Cyc_Port_occurs(t1,t2)){{struct Cyc_List_List*_tmp153=
Cyc_Port_filter_self(t1,_tmp101->uppers);for(0;(unsigned int)_tmp153;_tmp153=
_tmp153->tl){if(!Cyc_Port_leq(t2,(void*)_tmp153->hd))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}{
struct Cyc_List_List*_tmp154=Cyc_Port_filter_self(t1,_tmp101->lowers);for(0;(
unsigned int)_tmp154;_tmp154=_tmp154->tl){if(!Cyc_Port_leq((void*)_tmp154->hd,t2))(
int)_throw((void*)& Cyc_Port_Unify_ct_val);}}{void**_tmp5B1;_tmp101->eq=((_tmp5B1=
_cycalloc(sizeof(*_tmp5B1)),((_tmp5B1[0]=t2,_tmp5B1))));}return;}else{(int)
_throw((void*)& Cyc_Port_Unify_ct_val);}_LL9C: _tmp102=_tmpFE.f2;{struct Cyc_Port_Var_ct_struct*
_tmp103=(struct Cyc_Port_Var_ct_struct*)_tmp102;if(_tmp103->tag != 16)goto _LL9E;
else{_tmp104=_tmp103->f1;}};_LL9D: Cyc_Port_unify_ct(t2,t1);return;_LL9E: _tmp105=
_tmpFE.f1;{struct Cyc_Port_Ptr_ct_struct*_tmp106=(struct Cyc_Port_Ptr_ct_struct*)
_tmp105;if(_tmp106->tag != 11)goto _LLA0;else{_tmp107=(void*)_tmp106->f1;_tmp108=(
void*)_tmp106->f2;_tmp109=(void*)_tmp106->f3;_tmp10A=(void*)_tmp106->f4;_tmp10B=(
void*)_tmp106->f5;}};_tmp10C=_tmpFE.f2;{struct Cyc_Port_Ptr_ct_struct*_tmp10D=(
struct Cyc_Port_Ptr_ct_struct*)_tmp10C;if(_tmp10D->tag != 11)goto _LLA0;else{
_tmp10E=(void*)_tmp10D->f1;_tmp10F=(void*)_tmp10D->f2;_tmp110=(void*)_tmp10D->f3;
_tmp111=(void*)_tmp10D->f4;_tmp112=(void*)_tmp10D->f5;}};_LL9F: Cyc_Port_unify_ct(
_tmp107,_tmp10E);Cyc_Port_unify_ct(_tmp108,_tmp10F);Cyc_Port_unify_ct(_tmp109,
_tmp110);Cyc_Port_unify_ct(_tmp10A,_tmp111);Cyc_Port_unify_ct(_tmp10B,_tmp112);
return;_LLA0: _tmp113=_tmpFE.f1;{struct Cyc_Port_RgnVar_ct_struct*_tmp114=(struct
Cyc_Port_RgnVar_ct_struct*)_tmp113;if(_tmp114->tag != 10)goto _LLA2;else{_tmp115=
_tmp114->f1;_tmp116=*_tmp115;}};_tmp117=_tmpFE.f2;{struct Cyc_Port_RgnVar_ct_struct*
_tmp118=(struct Cyc_Port_RgnVar_ct_struct*)_tmp117;if(_tmp118->tag != 10)goto _LLA2;
else{_tmp119=_tmp118->f1;_tmp11A=*_tmp119;}};_LLA1: if(Cyc_strcmp((struct
_dyneither_ptr)_tmp116,(struct _dyneither_ptr)_tmp11A)!= 0)(int)_throw((void*)&
Cyc_Port_Unify_ct_val);return;_LLA2: _tmp11B=_tmpFE.f1;{struct Cyc_Port_Array_ct_struct*
_tmp11C=(struct Cyc_Port_Array_ct_struct*)_tmp11B;if(_tmp11C->tag != 12)goto _LLA4;
else{_tmp11D=(void*)_tmp11C->f1;_tmp11E=(void*)_tmp11C->f2;_tmp11F=(void*)
_tmp11C->f3;}};_tmp120=_tmpFE.f2;{struct Cyc_Port_Array_ct_struct*_tmp121=(struct
Cyc_Port_Array_ct_struct*)_tmp120;if(_tmp121->tag != 12)goto _LLA4;else{_tmp122=(
void*)_tmp121->f1;_tmp123=(void*)_tmp121->f2;_tmp124=(void*)_tmp121->f3;}};_LLA3:
Cyc_Port_unify_ct(_tmp11D,_tmp122);Cyc_Port_unify_ct(_tmp11E,_tmp123);Cyc_Port_unify_ct(
_tmp11F,_tmp124);return;_LLA4: _tmp125=_tmpFE.f1;{struct Cyc_Port_Fn_ct_struct*
_tmp126=(struct Cyc_Port_Fn_ct_struct*)_tmp125;if(_tmp126->tag != 15)goto _LLA6;
else{_tmp127=(void*)_tmp126->f1;_tmp128=_tmp126->f2;}};_tmp129=_tmpFE.f2;{struct
Cyc_Port_Fn_ct_struct*_tmp12A=(struct Cyc_Port_Fn_ct_struct*)_tmp129;if(_tmp12A->tag
!= 15)goto _LLA6;else{_tmp12B=(void*)_tmp12A->f1;_tmp12C=_tmp12A->f2;}};_LLA5: Cyc_Port_unify_ct(
_tmp127,_tmp12B);Cyc_Port_unify_cts(_tmp128,_tmp12C);return;_LLA6: _tmp12D=_tmpFE.f1;{
struct Cyc_Port_KnownAggr_ct_struct*_tmp12E=(struct Cyc_Port_KnownAggr_ct_struct*)
_tmp12D;if(_tmp12E->tag != 13)goto _LLA8;else{_tmp12F=_tmp12E->f1;}};_tmp130=
_tmpFE.f2;{struct Cyc_Port_KnownAggr_ct_struct*_tmp131=(struct Cyc_Port_KnownAggr_ct_struct*)
_tmp130;if(_tmp131->tag != 13)goto _LLA8;else{_tmp132=_tmp131->f1;}};_LLA7: if(
_tmp12F == _tmp132)return;else{(int)_throw((void*)& Cyc_Port_Unify_ct_val);}_LLA8:
_tmp133=_tmpFE.f1;{struct Cyc_Port_UnknownAggr_ct_struct*_tmp134=(struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp133;if(_tmp134->tag != 14)goto _LLAA;else{_tmp135=_tmp134->f1;_tmp136=_tmp134->f2;
_tmp137=(void***)& _tmp134->f2;}};_tmp138=_tmpFE.f2;{struct Cyc_Port_UnknownAggr_ct_struct*
_tmp139=(struct Cyc_Port_UnknownAggr_ct_struct*)_tmp138;if(_tmp139->tag != 14)goto
_LLAA;else{_tmp13A=_tmp139->f1;_tmp13B=_tmp139->f2;_tmp13C=(void***)& _tmp139->f2;}};
_LLA9: {void*_tmp156=Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(_tmp135,
_tmp13A,1));{void**_tmp5B2;*_tmp137=(*_tmp13C=((_tmp5B2=_cycalloc(sizeof(*
_tmp5B2)),((_tmp5B2[0]=_tmp156,_tmp5B2)))));}return;}_LLAA: _tmp13D=_tmpFE.f1;{
struct Cyc_Port_UnknownAggr_ct_struct*_tmp13E=(struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp13D;if(_tmp13E->tag != 14)goto _LLAC;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;
_tmp141=(void***)& _tmp13E->f2;}};_tmp142=_tmpFE.f2;{struct Cyc_Port_KnownAggr_ct_struct*
_tmp143=(struct Cyc_Port_KnownAggr_ct_struct*)_tmp142;if(_tmp143->tag != 13)goto
_LLAC;else{_tmp144=_tmp143->f1;_tmp145=*_tmp144;_tmp146=_tmp145.f1;_tmp147=
_tmp145.f2;}};_LLAB: Cyc_Port_merge_fields(_tmp147,_tmp13F,0);{void**_tmp5B3;*
_tmp141=((_tmp5B3=_cycalloc(sizeof(*_tmp5B3)),((_tmp5B3[0]=t2,_tmp5B3))));}
return;_LLAC: _tmp148=_tmpFE.f1;{struct Cyc_Port_KnownAggr_ct_struct*_tmp149=(
struct Cyc_Port_KnownAggr_ct_struct*)_tmp148;if(_tmp149->tag != 13)goto _LLAE;else{
_tmp14A=_tmp149->f1;_tmp14B=*_tmp14A;_tmp14C=_tmp14B.f1;_tmp14D=_tmp14B.f2;}};
_tmp14E=_tmpFE.f2;{struct Cyc_Port_UnknownAggr_ct_struct*_tmp14F=(struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp14E;if(_tmp14F->tag != 14)goto _LLAE;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;
_tmp152=(void***)& _tmp14F->f2;}};_LLAD: Cyc_Port_merge_fields(_tmp14D,_tmp150,0);{
void**_tmp5B4;*_tmp152=((_tmp5B4=_cycalloc(sizeof(*_tmp5B4)),((_tmp5B4[0]=t1,
_tmp5B4))));}return;_LLAE:;_LLAF:(int)_throw((void*)& Cyc_Port_Unify_ct_val);
_LL99:;};}static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*
t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Port_unify_ct((void*)t1->hd,(
void*)t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)& Cyc_Port_Unify_ct_val);}
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct
Cyc_List_List*fs2,int allow_f1_subset_f2){struct Cyc_List_List*common=0;{struct Cyc_List_List*
_tmp15A=fs2;for(0;(unsigned int)_tmp15A;_tmp15A=_tmp15A->tl){struct Cyc_Port_Cfield*
_tmp15B=(struct Cyc_Port_Cfield*)_tmp15A->hd;int found=0;{struct Cyc_List_List*
_tmp15C=fs1;for(0;(unsigned int)_tmp15C;_tmp15C=_tmp15C->tl){struct Cyc_Port_Cfield*
_tmp15D=(struct Cyc_Port_Cfield*)_tmp15C->hd;if(Cyc_strptrcmp(_tmp15D->f,_tmp15B->f)
== 0){{struct Cyc_List_List*_tmp5B5;common=((_tmp5B5=_cycalloc(sizeof(*_tmp5B5)),((
_tmp5B5->hd=_tmp15D,((_tmp5B5->tl=common,_tmp5B5))))));}Cyc_Port_unify_ct(
_tmp15D->qual,_tmp15B->qual);Cyc_Port_unify_ct(_tmp15D->type,_tmp15B->type);
found=1;break;}}}if(!found){if(allow_f1_subset_f2){struct Cyc_List_List*_tmp5B6;
common=((_tmp5B6=_cycalloc(sizeof(*_tmp5B6)),((_tmp5B6->hd=_tmp15B,((_tmp5B6->tl=
common,_tmp5B6))))));}else{(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}}{
struct Cyc_List_List*_tmp160=fs1;for(0;(unsigned int)_tmp160;_tmp160=_tmp160->tl){
struct Cyc_Port_Cfield*_tmp161=(struct Cyc_Port_Cfield*)_tmp160->hd;int found=0;{
struct Cyc_List_List*_tmp162=fs2;for(0;(unsigned int)_tmp162;_tmp162=_tmp162->tl){
struct Cyc_Port_Cfield*_tmp163=(struct Cyc_Port_Cfield*)_tmp162->hd;if(Cyc_strptrcmp(
_tmp161->f,_tmp163->f))found=1;}}if(!found){struct Cyc_List_List*_tmp5B7;common=((
_tmp5B7=_cycalloc(sizeof(*_tmp5B7)),((_tmp5B7->hd=_tmp161,((_tmp5B7->tl=common,
_tmp5B7))))));}}}return common;}static int Cyc_Port_unifies(void*t1,void*t2){{
struct _handler_cons _tmp165;_push_handler(& _tmp165);{int _tmp167=0;if(setjmp(
_tmp165.handler))_tmp167=1;if(!_tmp167){Cyc_Port_unify_ct(t1,t2);;_pop_handler();}
else{void*_tmp166=(void*)_exn_thrown;void*_tmp169=_tmp166;_LLB1: {struct Cyc_Port_Unify_ct_struct*
_tmp16A=(struct Cyc_Port_Unify_ct_struct*)_tmp169;if(_tmp16A->tag != Cyc_Port_Unify_ct)
goto _LLB3;}_LLB2: return 0;_LLB3:;_LLB4:(void)_throw(_tmp169);_LLB0:;}};}return 1;}
struct _tuple12{void*f1;void*f2;void*f3;void*f4;void*f5;};static struct Cyc_List_List*
Cyc_Port_insert_upper(void*v,void*t,struct Cyc_List_List**uppers){t=Cyc_Port_compress_ct(
t);{void*_tmp16B=t;_LLB6: {struct Cyc_Port_Notconst_ct_struct*_tmp16C=(struct Cyc_Port_Notconst_ct_struct*)
_tmp16B;if(_tmp16C->tag != 1)goto _LLB8;}_LLB7: goto _LLB9;_LLB8: {struct Cyc_Port_Zterm_ct_struct*
_tmp16D=(struct Cyc_Port_Zterm_ct_struct*)_tmp16B;if(_tmp16D->tag != 8)goto _LLBA;}
_LLB9: goto _LLBB;_LLBA: {struct Cyc_Port_Zero_ct_struct*_tmp16E=(struct Cyc_Port_Zero_ct_struct*)
_tmp16B;if(_tmp16E->tag != 5)goto _LLBC;}_LLBB: goto _LLBD;_LLBC: {struct Cyc_Port_Thin_ct_struct*
_tmp16F=(struct Cyc_Port_Thin_ct_struct*)_tmp16B;if(_tmp16F->tag != 2)goto _LLBE;}
_LLBD: goto _LLBF;_LLBE: {struct Cyc_Port_Fat_ct_struct*_tmp170=(struct Cyc_Port_Fat_ct_struct*)
_tmp16B;if(_tmp170->tag != 3)goto _LLC0;}_LLBF: goto _LLC1;_LLC0: {struct Cyc_Port_Array_ct_struct*
_tmp171=(struct Cyc_Port_Array_ct_struct*)_tmp16B;if(_tmp171->tag != 12)goto _LLC2;}
_LLC1: goto _LLC3;_LLC2: {struct Cyc_Port_KnownAggr_ct_struct*_tmp172=(struct Cyc_Port_KnownAggr_ct_struct*)
_tmp16B;if(_tmp172->tag != 13)goto _LLC4;}_LLC3: goto _LLC5;_LLC4: {struct Cyc_Port_Fn_ct_struct*
_tmp173=(struct Cyc_Port_Fn_ct_struct*)_tmp16B;if(_tmp173->tag != 15)goto _LLC6;}
_LLC5: goto _LLC7;_LLC6: {struct Cyc_Port_Heap_ct_struct*_tmp174=(struct Cyc_Port_Heap_ct_struct*)
_tmp16B;if(_tmp174->tag != 7)goto _LLC8;}_LLC7:*uppers=0;Cyc_Port_unifies(v,t);
return*uppers;_LLC8: {struct Cyc_Port_Void_ct_struct*_tmp175=(struct Cyc_Port_Void_ct_struct*)
_tmp16B;if(_tmp175->tag != 4)goto _LLCA;}_LLC9: goto _LLCB;_LLCA: {struct Cyc_Port_Const_ct_struct*
_tmp176=(struct Cyc_Port_Const_ct_struct*)_tmp16B;if(_tmp176->tag != 0)goto _LLCC;}
_LLCB: goto _LLCD;_LLCC: {struct Cyc_Port_Nozterm_ct_struct*_tmp177=(struct Cyc_Port_Nozterm_ct_struct*)
_tmp16B;if(_tmp177->tag != 9)goto _LLCE;}_LLCD: return*uppers;_LLCE:;_LLCF: goto
_LLB5;_LLB5:;}{struct Cyc_List_List*_tmp178=*uppers;for(0;(unsigned int)_tmp178;
_tmp178=_tmp178->tl){void*_tmp179=Cyc_Port_compress_ct((void*)_tmp178->hd);if(t
== _tmp179)return*uppers;{struct _tuple10 _tmp5B8;struct _tuple10 _tmp17B=(_tmp5B8.f1=
t,((_tmp5B8.f2=_tmp179,_tmp5B8)));void*_tmp17C;void*_tmp17E;void*_tmp180;void*
_tmp182;void*_tmp184;void*_tmp186;void*_tmp188;void*_tmp18A;void*_tmp18B;void*
_tmp18C;void*_tmp18D;void*_tmp18E;void*_tmp18F;void*_tmp191;void*_tmp192;void*
_tmp193;void*_tmp194;void*_tmp195;_LLD1: _tmp17C=_tmp17B.f1;{struct Cyc_Port_Arith_ct_struct*
_tmp17D=(struct Cyc_Port_Arith_ct_struct*)_tmp17C;if(_tmp17D->tag != 6)goto _LLD3;};
_tmp17E=_tmp17B.f2;{struct Cyc_Port_Ptr_ct_struct*_tmp17F=(struct Cyc_Port_Ptr_ct_struct*)
_tmp17E;if(_tmp17F->tag != 11)goto _LLD3;};_LLD2: goto _LLD4;_LLD3: _tmp180=_tmp17B.f1;{
struct Cyc_Port_Arith_ct_struct*_tmp181=(struct Cyc_Port_Arith_ct_struct*)_tmp180;
if(_tmp181->tag != 6)goto _LLD5;};_tmp182=_tmp17B.f2;{struct Cyc_Port_Zero_ct_struct*
_tmp183=(struct Cyc_Port_Zero_ct_struct*)_tmp182;if(_tmp183->tag != 5)goto _LLD5;};
_LLD4: goto _LLD6;_LLD5: _tmp184=_tmp17B.f1;{struct Cyc_Port_Arith_ct_struct*_tmp185=(
struct Cyc_Port_Arith_ct_struct*)_tmp184;if(_tmp185->tag != 6)goto _LLD7;};_tmp186=
_tmp17B.f2;{struct Cyc_Port_Array_ct_struct*_tmp187=(struct Cyc_Port_Array_ct_struct*)
_tmp186;if(_tmp187->tag != 12)goto _LLD7;};_LLD6: return*uppers;_LLD7: _tmp188=
_tmp17B.f1;{struct Cyc_Port_Ptr_ct_struct*_tmp189=(struct Cyc_Port_Ptr_ct_struct*)
_tmp188;if(_tmp189->tag != 11)goto _LLD9;else{_tmp18A=(void*)_tmp189->f1;_tmp18B=(
void*)_tmp189->f2;_tmp18C=(void*)_tmp189->f3;_tmp18D=(void*)_tmp189->f4;_tmp18E=(
void*)_tmp189->f5;}};_tmp18F=_tmp17B.f2;{struct Cyc_Port_Ptr_ct_struct*_tmp190=(
struct Cyc_Port_Ptr_ct_struct*)_tmp18F;if(_tmp190->tag != 11)goto _LLD9;else{
_tmp191=(void*)_tmp190->f1;_tmp192=(void*)_tmp190->f2;_tmp193=(void*)_tmp190->f3;
_tmp194=(void*)_tmp190->f4;_tmp195=(void*)_tmp190->f5;}};_LLD8: {void*_tmp198;
void*_tmp199;void*_tmp19A;void*_tmp19B;void*_tmp19C;struct _tuple12 _tmp5B9;struct
_tuple12 _tmp197=(_tmp5B9.f1=Cyc_Port_var(),((_tmp5B9.f2=Cyc_Port_var(),((_tmp5B9.f3=
Cyc_Port_var(),((_tmp5B9.f4=Cyc_Port_var(),((_tmp5B9.f5=Cyc_Port_var(),_tmp5B9)))))))));
_tmp198=_tmp197.f1;_tmp199=_tmp197.f2;_tmp19A=_tmp197.f3;_tmp19B=_tmp197.f4;
_tmp19C=_tmp197.f5;{struct Cyc_Port_Ptr_ct_struct _tmp5BC;struct Cyc_Port_Ptr_ct_struct*
_tmp5BB;struct Cyc_Port_Ptr_ct_struct*_tmp19D=(_tmp5BB=_cycalloc(sizeof(*_tmp5BB)),((
_tmp5BB[0]=((_tmp5BC.tag=11,((_tmp5BC.f1=(void*)_tmp198,((_tmp5BC.f2=(void*)
_tmp199,((_tmp5BC.f3=(void*)_tmp19A,((_tmp5BC.f4=(void*)_tmp19B,((_tmp5BC.f5=(
void*)_tmp19C,_tmp5BC)))))))))))),_tmp5BB)));Cyc_Port_leq(_tmp198,_tmp18A);Cyc_Port_leq(
_tmp198,_tmp191);Cyc_Port_leq(_tmp199,_tmp18B);Cyc_Port_leq(_tmp199,_tmp192);Cyc_Port_leq(
_tmp19A,_tmp18C);Cyc_Port_leq(_tmp19A,_tmp192);Cyc_Port_leq(_tmp19B,_tmp18D);Cyc_Port_leq(
_tmp19B,_tmp194);Cyc_Port_leq(_tmp19C,_tmp18E);Cyc_Port_leq(_tmp19C,_tmp195);
_tmp178->hd=(void*)((void*)_tmp19D);return*uppers;};}_LLD9:;_LLDA: goto _LLD0;
_LLD0:;};}}{struct Cyc_List_List*_tmp5BD;return(_tmp5BD=_cycalloc(sizeof(*_tmp5BD)),((
_tmp5BD->hd=t,((_tmp5BD->tl=*uppers,_tmp5BD)))));};}static struct Cyc_List_List*
Cyc_Port_insert_lower(void*v,void*t,struct Cyc_List_List**lowers){t=Cyc_Port_compress_ct(
t);{void*_tmp1A1=t;_LLDC: {struct Cyc_Port_Const_ct_struct*_tmp1A2=(struct Cyc_Port_Const_ct_struct*)
_tmp1A1;if(_tmp1A2->tag != 0)goto _LLDE;}_LLDD: goto _LLDF;_LLDE: {struct Cyc_Port_Zterm_ct_struct*
_tmp1A3=(struct Cyc_Port_Zterm_ct_struct*)_tmp1A1;if(_tmp1A3->tag != 8)goto _LLE0;}
_LLDF: goto _LLE1;_LLE0: {struct Cyc_Port_Thin_ct_struct*_tmp1A4=(struct Cyc_Port_Thin_ct_struct*)
_tmp1A1;if(_tmp1A4->tag != 2)goto _LLE2;}_LLE1: goto _LLE3;_LLE2: {struct Cyc_Port_Fat_ct_struct*
_tmp1A5=(struct Cyc_Port_Fat_ct_struct*)_tmp1A1;if(_tmp1A5->tag != 3)goto _LLE4;}
_LLE3: goto _LLE5;_LLE4: {struct Cyc_Port_Void_ct_struct*_tmp1A6=(struct Cyc_Port_Void_ct_struct*)
_tmp1A1;if(_tmp1A6->tag != 4)goto _LLE6;}_LLE5: goto _LLE7;_LLE6: {struct Cyc_Port_KnownAggr_ct_struct*
_tmp1A7=(struct Cyc_Port_KnownAggr_ct_struct*)_tmp1A1;if(_tmp1A7->tag != 13)goto
_LLE8;}_LLE7: goto _LLE9;_LLE8: {struct Cyc_Port_Fn_ct_struct*_tmp1A8=(struct Cyc_Port_Fn_ct_struct*)
_tmp1A1;if(_tmp1A8->tag != 15)goto _LLEA;}_LLE9: goto _LLEB;_LLEA: {struct Cyc_Port_RgnVar_ct_struct*
_tmp1A9=(struct Cyc_Port_RgnVar_ct_struct*)_tmp1A1;if(_tmp1A9->tag != 10)goto _LLEC;}
_LLEB:*lowers=0;Cyc_Port_unifies(v,t);return*lowers;_LLEC: {struct Cyc_Port_Heap_ct_struct*
_tmp1AA=(struct Cyc_Port_Heap_ct_struct*)_tmp1A1;if(_tmp1AA->tag != 7)goto _LLEE;}
_LLED: goto _LLEF;_LLEE: {struct Cyc_Port_Notconst_ct_struct*_tmp1AB=(struct Cyc_Port_Notconst_ct_struct*)
_tmp1A1;if(_tmp1AB->tag != 1)goto _LLF0;}_LLEF: goto _LLF1;_LLF0: {struct Cyc_Port_Nozterm_ct_struct*
_tmp1AC=(struct Cyc_Port_Nozterm_ct_struct*)_tmp1A1;if(_tmp1AC->tag != 9)goto _LLF2;}
_LLF1: return*lowers;_LLF2:;_LLF3: goto _LLDB;_LLDB:;}{struct Cyc_List_List*_tmp1AD=*
lowers;for(0;(unsigned int)_tmp1AD;_tmp1AD=_tmp1AD->tl){void*_tmp1AE=Cyc_Port_compress_ct((
void*)_tmp1AD->hd);if(t == _tmp1AE)return*lowers;{struct _tuple10 _tmp5BE;struct
_tuple10 _tmp1B0=(_tmp5BE.f1=t,((_tmp5BE.f2=_tmp1AE,_tmp5BE)));void*_tmp1B1;void*
_tmp1B3;void*_tmp1B5;void*_tmp1B7;void*_tmp1B9;void*_tmp1BB;void*_tmp1BD;void*
_tmp1BF;void*_tmp1C1;void*_tmp1C3;void*_tmp1C5;void*_tmp1C6;void*_tmp1C7;void*
_tmp1C8;void*_tmp1C9;void*_tmp1CA;void*_tmp1CC;void*_tmp1CD;void*_tmp1CE;void*
_tmp1CF;void*_tmp1D0;_LLF5: _tmp1B1=_tmp1B0.f2;{struct Cyc_Port_Void_ct_struct*
_tmp1B2=(struct Cyc_Port_Void_ct_struct*)_tmp1B1;if(_tmp1B2->tag != 4)goto _LLF7;};
_LLF6: goto _LLF8;_LLF7: _tmp1B3=_tmp1B0.f1;{struct Cyc_Port_Zero_ct_struct*_tmp1B4=(
struct Cyc_Port_Zero_ct_struct*)_tmp1B3;if(_tmp1B4->tag != 5)goto _LLF9;};_tmp1B5=
_tmp1B0.f2;{struct Cyc_Port_Arith_ct_struct*_tmp1B6=(struct Cyc_Port_Arith_ct_struct*)
_tmp1B5;if(_tmp1B6->tag != 6)goto _LLF9;};_LLF8: goto _LLFA;_LLF9: _tmp1B7=_tmp1B0.f1;{
struct Cyc_Port_Zero_ct_struct*_tmp1B8=(struct Cyc_Port_Zero_ct_struct*)_tmp1B7;
if(_tmp1B8->tag != 5)goto _LLFB;};_tmp1B9=_tmp1B0.f2;{struct Cyc_Port_Ptr_ct_struct*
_tmp1BA=(struct Cyc_Port_Ptr_ct_struct*)_tmp1B9;if(_tmp1BA->tag != 11)goto _LLFB;};
_LLFA: goto _LLFC;_LLFB: _tmp1BB=_tmp1B0.f1;{struct Cyc_Port_Ptr_ct_struct*_tmp1BC=(
struct Cyc_Port_Ptr_ct_struct*)_tmp1BB;if(_tmp1BC->tag != 11)goto _LLFD;};_tmp1BD=
_tmp1B0.f2;{struct Cyc_Port_Arith_ct_struct*_tmp1BE=(struct Cyc_Port_Arith_ct_struct*)
_tmp1BD;if(_tmp1BE->tag != 6)goto _LLFD;};_LLFC: goto _LLFE;_LLFD: _tmp1BF=_tmp1B0.f1;{
struct Cyc_Port_Array_ct_struct*_tmp1C0=(struct Cyc_Port_Array_ct_struct*)_tmp1BF;
if(_tmp1C0->tag != 12)goto _LLFF;};_tmp1C1=_tmp1B0.f2;{struct Cyc_Port_Arith_ct_struct*
_tmp1C2=(struct Cyc_Port_Arith_ct_struct*)_tmp1C1;if(_tmp1C2->tag != 6)goto _LLFF;};
_LLFE: return*lowers;_LLFF: _tmp1C3=_tmp1B0.f1;{struct Cyc_Port_Ptr_ct_struct*
_tmp1C4=(struct Cyc_Port_Ptr_ct_struct*)_tmp1C3;if(_tmp1C4->tag != 11)goto _LL101;
else{_tmp1C5=(void*)_tmp1C4->f1;_tmp1C6=(void*)_tmp1C4->f2;_tmp1C7=(void*)
_tmp1C4->f3;_tmp1C8=(void*)_tmp1C4->f4;_tmp1C9=(void*)_tmp1C4->f5;}};_tmp1CA=
_tmp1B0.f2;{struct Cyc_Port_Ptr_ct_struct*_tmp1CB=(struct Cyc_Port_Ptr_ct_struct*)
_tmp1CA;if(_tmp1CB->tag != 11)goto _LL101;else{_tmp1CC=(void*)_tmp1CB->f1;_tmp1CD=(
void*)_tmp1CB->f2;_tmp1CE=(void*)_tmp1CB->f3;_tmp1CF=(void*)_tmp1CB->f4;_tmp1D0=(
void*)_tmp1CB->f5;}};_LL100: {void*_tmp1D3;void*_tmp1D4;void*_tmp1D5;void*
_tmp1D6;void*_tmp1D7;struct _tuple12 _tmp5BF;struct _tuple12 _tmp1D2=(_tmp5BF.f1=Cyc_Port_var(),((
_tmp5BF.f2=Cyc_Port_var(),((_tmp5BF.f3=Cyc_Port_var(),((_tmp5BF.f4=Cyc_Port_var(),((
_tmp5BF.f5=Cyc_Port_var(),_tmp5BF)))))))));_tmp1D3=_tmp1D2.f1;_tmp1D4=_tmp1D2.f2;
_tmp1D5=_tmp1D2.f3;_tmp1D6=_tmp1D2.f4;_tmp1D7=_tmp1D2.f5;{struct Cyc_Port_Ptr_ct_struct
_tmp5C2;struct Cyc_Port_Ptr_ct_struct*_tmp5C1;struct Cyc_Port_Ptr_ct_struct*
_tmp1D8=(_tmp5C1=_cycalloc(sizeof(*_tmp5C1)),((_tmp5C1[0]=((_tmp5C2.tag=11,((
_tmp5C2.f1=(void*)_tmp1D3,((_tmp5C2.f2=(void*)_tmp1D4,((_tmp5C2.f3=(void*)
_tmp1D5,((_tmp5C2.f4=(void*)_tmp1D6,((_tmp5C2.f5=(void*)_tmp1D7,_tmp5C2)))))))))))),
_tmp5C1)));Cyc_Port_leq(_tmp1C5,_tmp1D3);Cyc_Port_leq(_tmp1CC,_tmp1D3);Cyc_Port_leq(
_tmp1C6,_tmp1D4);Cyc_Port_leq(_tmp1CD,_tmp1D4);Cyc_Port_leq(_tmp1C7,_tmp1D5);Cyc_Port_leq(
_tmp1CD,_tmp1D5);Cyc_Port_leq(_tmp1C8,_tmp1D6);Cyc_Port_leq(_tmp1CF,_tmp1D6);Cyc_Port_leq(
_tmp1C9,_tmp1D7);Cyc_Port_leq(_tmp1D0,_tmp1D7);_tmp1AD->hd=(void*)((void*)
_tmp1D8);return*lowers;};}_LL101:;_LL102: goto _LLF4;_LLF4:;};}}{struct Cyc_List_List*
_tmp5C3;return(_tmp5C3=_cycalloc(sizeof(*_tmp5C3)),((_tmp5C3->hd=t,((_tmp5C3->tl=*
lowers,_tmp5C3)))));};}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
struct Cyc_Port_Cvar;static int Cyc_Port_leq(void*t1,void*t2){if(t1 == t2)return 1;t1=
Cyc_Port_compress_ct(t1);t2=Cyc_Port_compress_ct(t2);{struct _tuple10 _tmp5C4;
struct _tuple10 _tmp1DD=(_tmp5C4.f1=t1,((_tmp5C4.f2=t2,_tmp5C4)));void*_tmp1DE;
void*_tmp1E0;struct _dyneither_ptr*_tmp1E2;struct _dyneither_ptr _tmp1E3;void*
_tmp1E4;struct _dyneither_ptr*_tmp1E6;struct _dyneither_ptr _tmp1E7;void*_tmp1E8;
struct _dyneither_ptr*_tmp1EA;struct _dyneither_ptr _tmp1EB;void*_tmp1EC;void*
_tmp1EE;void*_tmp1F0;void*_tmp1F2;void*_tmp1F4;void*_tmp1F6;void*_tmp1F8;void*
_tmp1FA;void*_tmp1FC;void*_tmp1FE;void*_tmp200;void*_tmp202;void*_tmp204;void*
_tmp206;void*_tmp208;void*_tmp20A;void*_tmp20C;void*_tmp20E;void*_tmp210;void*
_tmp212;void*_tmp214;void*_tmp216;void*_tmp218;void*_tmp21A;void*_tmp21C;void*
_tmp21E;void*_tmp220;void*_tmp222;void*_tmp224;void*_tmp226;void*_tmp227;void*
_tmp228;void*_tmp229;void*_tmp22A;void*_tmp22B;void*_tmp22D;void*_tmp22E;void*
_tmp22F;void*_tmp230;void*_tmp231;void*_tmp232;void*_tmp234;void*_tmp235;void*
_tmp236;void*_tmp237;void*_tmp239;void*_tmp23A;void*_tmp23B;void*_tmp23C;void*
_tmp23E;void*_tmp23F;void*_tmp240;void*_tmp241;void*_tmp243;void*_tmp244;void*
_tmp245;void*_tmp246;void*_tmp247;struct Cyc_Port_Cvar*_tmp249;void*_tmp24A;
struct Cyc_Port_Cvar*_tmp24C;void*_tmp24D;struct Cyc_Port_Cvar*_tmp24F;void*
_tmp250;struct Cyc_Port_Cvar*_tmp252;_LL104: _tmp1DE=_tmp1DD.f1;{struct Cyc_Port_Heap_ct_struct*
_tmp1DF=(struct Cyc_Port_Heap_ct_struct*)_tmp1DE;if(_tmp1DF->tag != 7)goto _LL106;};
_LL105: return 1;_LL106: _tmp1E0=_tmp1DD.f1;{struct Cyc_Port_RgnVar_ct_struct*
_tmp1E1=(struct Cyc_Port_RgnVar_ct_struct*)_tmp1E0;if(_tmp1E1->tag != 10)goto
_LL108;else{_tmp1E2=_tmp1E1->f1;_tmp1E3=*_tmp1E2;}};_tmp1E4=_tmp1DD.f2;{struct
Cyc_Port_RgnVar_ct_struct*_tmp1E5=(struct Cyc_Port_RgnVar_ct_struct*)_tmp1E4;if(
_tmp1E5->tag != 10)goto _LL108;else{_tmp1E6=_tmp1E5->f1;_tmp1E7=*_tmp1E6;}};_LL107:
return Cyc_strcmp((struct _dyneither_ptr)_tmp1E3,(struct _dyneither_ptr)_tmp1E7)== 
0;_LL108: _tmp1E8=_tmp1DD.f1;{struct Cyc_Port_RgnVar_ct_struct*_tmp1E9=(struct Cyc_Port_RgnVar_ct_struct*)
_tmp1E8;if(_tmp1E9->tag != 10)goto _LL10A;else{_tmp1EA=_tmp1E9->f1;_tmp1EB=*
_tmp1EA;}};_tmp1EC=_tmp1DD.f2;{struct Cyc_Port_Heap_ct_struct*_tmp1ED=(struct Cyc_Port_Heap_ct_struct*)
_tmp1EC;if(_tmp1ED->tag != 7)goto _LL10A;};_LL109: return 0;_LL10A: _tmp1EE=_tmp1DD.f1;{
struct Cyc_Port_Notconst_ct_struct*_tmp1EF=(struct Cyc_Port_Notconst_ct_struct*)
_tmp1EE;if(_tmp1EF->tag != 1)goto _LL10C;};_tmp1F0=_tmp1DD.f2;{struct Cyc_Port_Const_ct_struct*
_tmp1F1=(struct Cyc_Port_Const_ct_struct*)_tmp1F0;if(_tmp1F1->tag != 0)goto _LL10C;};
_LL10B: return 1;_LL10C: _tmp1F2=_tmp1DD.f1;{struct Cyc_Port_Const_ct_struct*_tmp1F3=(
struct Cyc_Port_Const_ct_struct*)_tmp1F2;if(_tmp1F3->tag != 0)goto _LL10E;};_tmp1F4=
_tmp1DD.f2;{struct Cyc_Port_Notconst_ct_struct*_tmp1F5=(struct Cyc_Port_Notconst_ct_struct*)
_tmp1F4;if(_tmp1F5->tag != 1)goto _LL10E;};_LL10D: return 0;_LL10E: _tmp1F6=_tmp1DD.f1;{
struct Cyc_Port_Nozterm_ct_struct*_tmp1F7=(struct Cyc_Port_Nozterm_ct_struct*)
_tmp1F6;if(_tmp1F7->tag != 9)goto _LL110;};_tmp1F8=_tmp1DD.f2;{struct Cyc_Port_Zterm_ct_struct*
_tmp1F9=(struct Cyc_Port_Zterm_ct_struct*)_tmp1F8;if(_tmp1F9->tag != 8)goto _LL110;};
_LL10F: return 0;_LL110: _tmp1FA=_tmp1DD.f1;{struct Cyc_Port_Zterm_ct_struct*_tmp1FB=(
struct Cyc_Port_Zterm_ct_struct*)_tmp1FA;if(_tmp1FB->tag != 8)goto _LL112;};_tmp1FC=
_tmp1DD.f2;{struct Cyc_Port_Nozterm_ct_struct*_tmp1FD=(struct Cyc_Port_Nozterm_ct_struct*)
_tmp1FC;if(_tmp1FD->tag != 9)goto _LL112;};_LL111: return 1;_LL112: _tmp1FE=_tmp1DD.f1;{
struct Cyc_Port_Var_ct_struct*_tmp1FF=(struct Cyc_Port_Var_ct_struct*)_tmp1FE;if(
_tmp1FF->tag != 16)goto _LL114;};_tmp200=_tmp1DD.f2;{struct Cyc_Port_Const_ct_struct*
_tmp201=(struct Cyc_Port_Const_ct_struct*)_tmp200;if(_tmp201->tag != 0)goto _LL114;};
_LL113: return 1;_LL114: _tmp202=_tmp1DD.f1;{struct Cyc_Port_Var_ct_struct*_tmp203=(
struct Cyc_Port_Var_ct_struct*)_tmp202;if(_tmp203->tag != 16)goto _LL116;};_tmp204=
_tmp1DD.f2;{struct Cyc_Port_Void_ct_struct*_tmp205=(struct Cyc_Port_Void_ct_struct*)
_tmp204;if(_tmp205->tag != 4)goto _LL116;};_LL115: return 1;_LL116: _tmp206=_tmp1DD.f1;{
struct Cyc_Port_Void_ct_struct*_tmp207=(struct Cyc_Port_Void_ct_struct*)_tmp206;
if(_tmp207->tag != 4)goto _LL118;};_LL117: return 0;_LL118: _tmp208=_tmp1DD.f1;{
struct Cyc_Port_Zero_ct_struct*_tmp209=(struct Cyc_Port_Zero_ct_struct*)_tmp208;
if(_tmp209->tag != 5)goto _LL11A;};_tmp20A=_tmp1DD.f2;{struct Cyc_Port_Arith_ct_struct*
_tmp20B=(struct Cyc_Port_Arith_ct_struct*)_tmp20A;if(_tmp20B->tag != 6)goto _LL11A;};
_LL119: return 1;_LL11A: _tmp20C=_tmp1DD.f1;{struct Cyc_Port_Zero_ct_struct*_tmp20D=(
struct Cyc_Port_Zero_ct_struct*)_tmp20C;if(_tmp20D->tag != 5)goto _LL11C;};_tmp20E=
_tmp1DD.f2;{struct Cyc_Port_Ptr_ct_struct*_tmp20F=(struct Cyc_Port_Ptr_ct_struct*)
_tmp20E;if(_tmp20F->tag != 11)goto _LL11C;};_LL11B: return 1;_LL11C: _tmp210=_tmp1DD.f1;{
struct Cyc_Port_Zero_ct_struct*_tmp211=(struct Cyc_Port_Zero_ct_struct*)_tmp210;
if(_tmp211->tag != 5)goto _LL11E;};_tmp212=_tmp1DD.f2;{struct Cyc_Port_Void_ct_struct*
_tmp213=(struct Cyc_Port_Void_ct_struct*)_tmp212;if(_tmp213->tag != 4)goto _LL11E;};
_LL11D: return 1;_LL11E: _tmp214=_tmp1DD.f1;{struct Cyc_Port_Ptr_ct_struct*_tmp215=(
struct Cyc_Port_Ptr_ct_struct*)_tmp214;if(_tmp215->tag != 11)goto _LL120;};_tmp216=
_tmp1DD.f2;{struct Cyc_Port_Arith_ct_struct*_tmp217=(struct Cyc_Port_Arith_ct_struct*)
_tmp216;if(_tmp217->tag != 6)goto _LL120;};_LL11F: return 1;_LL120: _tmp218=_tmp1DD.f1;{
struct Cyc_Port_Ptr_ct_struct*_tmp219=(struct Cyc_Port_Ptr_ct_struct*)_tmp218;if(
_tmp219->tag != 11)goto _LL122;};_tmp21A=_tmp1DD.f2;{struct Cyc_Port_Void_ct_struct*
_tmp21B=(struct Cyc_Port_Void_ct_struct*)_tmp21A;if(_tmp21B->tag != 4)goto _LL122;};
_LL121: return 1;_LL122: _tmp21C=_tmp1DD.f1;{struct Cyc_Port_Array_ct_struct*_tmp21D=(
struct Cyc_Port_Array_ct_struct*)_tmp21C;if(_tmp21D->tag != 12)goto _LL124;};
_tmp21E=_tmp1DD.f2;{struct Cyc_Port_Arith_ct_struct*_tmp21F=(struct Cyc_Port_Arith_ct_struct*)
_tmp21E;if(_tmp21F->tag != 6)goto _LL124;};_LL123: return 1;_LL124: _tmp220=_tmp1DD.f1;{
struct Cyc_Port_Array_ct_struct*_tmp221=(struct Cyc_Port_Array_ct_struct*)_tmp220;
if(_tmp221->tag != 12)goto _LL126;};_tmp222=_tmp1DD.f2;{struct Cyc_Port_Void_ct_struct*
_tmp223=(struct Cyc_Port_Void_ct_struct*)_tmp222;if(_tmp223->tag != 4)goto _LL126;};
_LL125: return 1;_LL126: _tmp224=_tmp1DD.f1;{struct Cyc_Port_Ptr_ct_struct*_tmp225=(
struct Cyc_Port_Ptr_ct_struct*)_tmp224;if(_tmp225->tag != 11)goto _LL128;else{
_tmp226=(void*)_tmp225->f1;_tmp227=(void*)_tmp225->f2;_tmp228=(void*)_tmp225->f3;
_tmp229=(void*)_tmp225->f4;_tmp22A=(void*)_tmp225->f5;}};_tmp22B=_tmp1DD.f2;{
struct Cyc_Port_Ptr_ct_struct*_tmp22C=(struct Cyc_Port_Ptr_ct_struct*)_tmp22B;if(
_tmp22C->tag != 11)goto _LL128;else{_tmp22D=(void*)_tmp22C->f1;_tmp22E=(void*)
_tmp22C->f2;_tmp22F=(void*)_tmp22C->f3;_tmp230=(void*)_tmp22C->f4;_tmp231=(void*)
_tmp22C->f5;}};_LL127: return(((Cyc_Port_leq(_tmp226,_tmp22D) && Cyc_Port_leq(
_tmp227,_tmp22E)) && Cyc_Port_unifies(_tmp228,_tmp22F)) && Cyc_Port_leq(_tmp229,
_tmp230)) && Cyc_Port_leq(_tmp22A,_tmp231);_LL128: _tmp232=_tmp1DD.f1;{struct Cyc_Port_Array_ct_struct*
_tmp233=(struct Cyc_Port_Array_ct_struct*)_tmp232;if(_tmp233->tag != 12)goto _LL12A;
else{_tmp234=(void*)_tmp233->f1;_tmp235=(void*)_tmp233->f2;_tmp236=(void*)
_tmp233->f3;}};_tmp237=_tmp1DD.f2;{struct Cyc_Port_Array_ct_struct*_tmp238=(
struct Cyc_Port_Array_ct_struct*)_tmp237;if(_tmp238->tag != 12)goto _LL12A;else{
_tmp239=(void*)_tmp238->f1;_tmp23A=(void*)_tmp238->f2;_tmp23B=(void*)_tmp238->f3;}};
_LL129: return(Cyc_Port_leq(_tmp234,_tmp239) && Cyc_Port_leq(_tmp235,_tmp23A))
 && Cyc_Port_leq(_tmp236,_tmp23B);_LL12A: _tmp23C=_tmp1DD.f1;{struct Cyc_Port_Array_ct_struct*
_tmp23D=(struct Cyc_Port_Array_ct_struct*)_tmp23C;if(_tmp23D->tag != 12)goto _LL12C;
else{_tmp23E=(void*)_tmp23D->f1;_tmp23F=(void*)_tmp23D->f2;_tmp240=(void*)
_tmp23D->f3;}};_tmp241=_tmp1DD.f2;{struct Cyc_Port_Ptr_ct_struct*_tmp242=(struct
Cyc_Port_Ptr_ct_struct*)_tmp241;if(_tmp242->tag != 11)goto _LL12C;else{_tmp243=(
void*)_tmp242->f1;_tmp244=(void*)_tmp242->f2;_tmp245=(void*)_tmp242->f3;_tmp246=(
void*)_tmp242->f5;}};_LL12B: return((Cyc_Port_leq(_tmp23E,_tmp243) && Cyc_Port_leq(
_tmp23F,_tmp244)) && Cyc_Port_unifies((void*)& Cyc_Port_Fat_ct_val,_tmp245)) && 
Cyc_Port_leq(_tmp240,_tmp246);_LL12C: _tmp247=_tmp1DD.f1;{struct Cyc_Port_Var_ct_struct*
_tmp248=(struct Cyc_Port_Var_ct_struct*)_tmp247;if(_tmp248->tag != 16)goto _LL12E;
else{_tmp249=_tmp248->f1;}};_tmp24A=_tmp1DD.f2;{struct Cyc_Port_Var_ct_struct*
_tmp24B=(struct Cyc_Port_Var_ct_struct*)_tmp24A;if(_tmp24B->tag != 16)goto _LL12E;
else{_tmp24C=_tmp24B->f1;}};_LL12D: _tmp249->uppers=Cyc_Port_filter_self(t1,
_tmp249->uppers);_tmp24C->lowers=Cyc_Port_filter_self(t2,_tmp24C->lowers);
_tmp249->uppers=Cyc_Port_insert_upper(t1,t2,& _tmp249->uppers);_tmp24C->lowers=
Cyc_Port_insert_lower(t2,t1,& _tmp24C->lowers);return 1;_LL12E: _tmp24D=_tmp1DD.f1;{
struct Cyc_Port_Var_ct_struct*_tmp24E=(struct Cyc_Port_Var_ct_struct*)_tmp24D;if(
_tmp24E->tag != 16)goto _LL130;else{_tmp24F=_tmp24E->f1;}};_LL12F: _tmp24F->uppers=
Cyc_Port_filter_self(t1,_tmp24F->uppers);_tmp24F->uppers=Cyc_Port_insert_upper(
t1,t2,& _tmp24F->uppers);return 1;_LL130: _tmp250=_tmp1DD.f2;{struct Cyc_Port_Var_ct_struct*
_tmp251=(struct Cyc_Port_Var_ct_struct*)_tmp250;if(_tmp251->tag != 16)goto _LL132;
else{_tmp252=_tmp251->f1;}};_LL131: _tmp252->lowers=Cyc_Port_filter_self(t2,
_tmp252->lowers);_tmp252->lowers=Cyc_Port_insert_lower(t2,t1,& _tmp252->lowers);
return 1;_LL132:;_LL133: return Cyc_Port_unifies(t1,t2);_LL103:;};}struct Cyc_Port_GlobalCenv{
struct Cyc_Dict_Dict typedef_dict;struct Cyc_Dict_Dict struct_dict;struct Cyc_Dict_Dict
union_dict;void*return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*
pointer_edits;struct Cyc_List_List*zeroterm_edits;struct Cyc_List_List*edits;int
porting;};enum Cyc_Port_CPos{Cyc_Port_FnRes_pos  = 0,Cyc_Port_FnArg_pos  = 1,Cyc_Port_FnBody_pos
 = 2,Cyc_Port_Toplevel_pos  = 3};struct Cyc_Port_GlobalCenv;struct Cyc_Port_Cenv{
struct Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict var_dict;enum Cyc_Port_CPos
cpos;};struct Cyc_Port_Cenv;struct Cyc_Port_GlobalCenv;static struct Cyc_Port_Cenv*
Cyc_Port_empty_cenv(){struct Cyc_Port_GlobalCenv*_tmp5C5;struct Cyc_Port_GlobalCenv*
g=(_tmp5C5=_cycalloc(sizeof(*_tmp5C5)),((_tmp5C5->typedef_dict=((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((
_tmp5C5->struct_dict=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((_tmp5C5->union_dict=((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((
_tmp5C5->qualifier_edits=0,((_tmp5C5->pointer_edits=0,((_tmp5C5->zeroterm_edits=
0,((_tmp5C5->edits=0,((_tmp5C5->porting=0,((_tmp5C5->return_type=Cyc_Port_void_ct(),
_tmp5C5)))))))))))))))))));struct Cyc_Port_Cenv*_tmp5C6;return(_tmp5C6=_cycalloc(
sizeof(*_tmp5C6)),((_tmp5C6->gcenv=g,((_tmp5C6->cpos=Cyc_Port_Toplevel_pos,((
_tmp5C6->var_dict=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),_tmp5C6)))))));}struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static int Cyc_Port_in_fn_arg(struct Cyc_Port_Cenv*env){return
env->cpos == Cyc_Port_FnArg_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static
int Cyc_Port_in_fn_res(struct Cyc_Port_Cenv*env){return env->cpos == Cyc_Port_FnRes_pos;}
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*
env){return env->cpos == Cyc_Port_Toplevel_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
static void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env){return(env->gcenv)->return_type;}
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static void*Cyc_Port_lookup_typedef(
struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _handler_cons _tmp255;
_push_handler(& _tmp255);{int _tmp257=0;if(setjmp(_tmp255.handler))_tmp257=1;if(!
_tmp257){{void*_tmp259;struct _tuple10 _tmp258=*((struct _tuple10*(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);_tmp259=
_tmp258.f1;{void*_tmp25A=_tmp259;_npop_handler(0);return _tmp25A;};};_pop_handler();}
else{void*_tmp256=(void*)_exn_thrown;void*_tmp25C=_tmp256;_LL135: {struct Cyc_Dict_Absent_struct*
_tmp25D=(struct Cyc_Dict_Absent_struct*)_tmp25C;if(_tmp25D->tag != Cyc_Dict_Absent)
goto _LL137;}_LL136: return Cyc_Absyn_sint_typ;_LL137:;_LL138:(void)_throw(_tmp25C);
_LL134:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static void*Cyc_Port_lookup_typedef_ctype(
struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _handler_cons _tmp25E;
_push_handler(& _tmp25E);{int _tmp260=0;if(setjmp(_tmp25E.handler))_tmp260=1;if(!
_tmp260){{void*_tmp262;struct _tuple10 _tmp261=*((struct _tuple10*(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);_tmp262=
_tmp261.f2;{void*_tmp263=_tmp262;_npop_handler(0);return _tmp263;};};_pop_handler();}
else{void*_tmp25F=(void*)_exn_thrown;void*_tmp265=_tmp25F;_LL13A: {struct Cyc_Dict_Absent_struct*
_tmp266=(struct Cyc_Dict_Absent_struct*)_tmp265;if(_tmp266->tag != Cyc_Dict_Absent)
goto _LL13C;}_LL13B: return Cyc_Port_var();_LL13C:;_LL13D:(void)_throw(_tmp265);
_LL139:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static struct _tuple11*Cyc_Port_lookup_struct_decl(
struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _tuple11**_tmp267=((struct
_tuple11**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->struct_dict,
n);if((unsigned int)_tmp267)return*_tmp267;else{struct Cyc_Absyn_Aggrdecl*_tmp5C7;
struct Cyc_Absyn_Aggrdecl*_tmp268=(_tmp5C7=_cycalloc(sizeof(*_tmp5C7)),((_tmp5C7->kind=
Cyc_Absyn_StructA,((_tmp5C7->sc=Cyc_Absyn_Public,((_tmp5C7->name=n,((_tmp5C7->tvs=
0,((_tmp5C7->impl=0,((_tmp5C7->attributes=0,_tmp5C7)))))))))))));struct _tuple11*
_tmp5C8;struct _tuple11*p=(_tmp5C8=_cycalloc(sizeof(*_tmp5C8)),((_tmp5C8->f1=
_tmp268,((_tmp5C8->f2=0,_tmp5C8)))));(env->gcenv)->struct_dict=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,
n,p);return p;}}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static struct _tuple11*Cyc_Port_lookup_union_decl(
struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _tuple11**_tmp26B=((struct
_tuple11**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->union_dict,
n);if((unsigned int)_tmp26B)return*_tmp26B;else{struct Cyc_Absyn_Aggrdecl*_tmp5C9;
struct Cyc_Absyn_Aggrdecl*_tmp26C=(_tmp5C9=_cycalloc(sizeof(*_tmp5C9)),((_tmp5C9->kind=
Cyc_Absyn_UnionA,((_tmp5C9->sc=Cyc_Absyn_Public,((_tmp5C9->name=n,((_tmp5C9->tvs=
0,((_tmp5C9->impl=0,((_tmp5C9->attributes=0,_tmp5C9)))))))))))));struct _tuple11*
_tmp5CA;struct _tuple11*p=(_tmp5CA=_cycalloc(sizeof(*_tmp5CA)),((_tmp5CA->f1=
_tmp26C,((_tmp5CA->f2=0,_tmp5CA)))));(env->gcenv)->union_dict=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->union_dict,
n,p);return p;}}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple13{void*f1;
struct _tuple10*f2;};static struct _tuple10 Cyc_Port_lookup_var(struct Cyc_Port_Cenv*
env,struct _tuple0*x){struct _handler_cons _tmp26F;_push_handler(& _tmp26F);{int
_tmp271=0;if(setjmp(_tmp26F.handler))_tmp271=1;if(!_tmp271){{struct _tuple10*
_tmp273;struct _tuple13 _tmp272=*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp273=_tmp272.f2;{struct _tuple10
_tmp274=*_tmp273;_npop_handler(0);return _tmp274;};};_pop_handler();}else{void*
_tmp270=(void*)_exn_thrown;void*_tmp276=_tmp270;_LL13F: {struct Cyc_Dict_Absent_struct*
_tmp277=(struct Cyc_Dict_Absent_struct*)_tmp276;if(_tmp277->tag != Cyc_Dict_Absent)
goto _LL141;}_LL140: {struct _tuple10 _tmp5CB;return(_tmp5CB.f1=Cyc_Port_var(),((
_tmp5CB.f2=Cyc_Port_var(),_tmp5CB)));}_LL141:;_LL142:(void)_throw(_tmp276);
_LL13E:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static struct _tuple13*Cyc_Port_lookup_full_var(
struct Cyc_Port_Cenv*env,struct _tuple0*x){return((struct _tuple13*(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);}struct Cyc_Port_Cenv;struct
Cyc_Port_Cenv;static int Cyc_Port_declared_var(struct Cyc_Port_Cenv*env,struct
_tuple0*x){return((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)(
env->var_dict,x);}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static int Cyc_Port_isfn(
struct Cyc_Port_Cenv*env,struct _tuple0*x){struct _handler_cons _tmp279;
_push_handler(& _tmp279);{int _tmp27B=0;if(setjmp(_tmp279.handler))_tmp27B=1;if(!
_tmp27B){{void*_tmp27D;struct _tuple13 _tmp27C=*((struct _tuple13*(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp27D=_tmp27C.f1;LOOP: {
void*_tmp27E=_tmp27D;struct _tuple0*_tmp280;_LL144: {struct Cyc_Absyn_TypedefType_struct*
_tmp27F=(struct Cyc_Absyn_TypedefType_struct*)_tmp27E;if(_tmp27F->tag != 17)goto
_LL146;else{_tmp280=_tmp27F->f1;}}_LL145: _tmp27D=Cyc_Port_lookup_typedef(env,
_tmp280);goto LOOP;_LL146: {struct Cyc_Absyn_FnType_struct*_tmp281=(struct Cyc_Absyn_FnType_struct*)
_tmp27E;if(_tmp281->tag != 9)goto _LL148;}_LL147: {int _tmp282=1;_npop_handler(0);
return _tmp282;}_LL148:;_LL149: {int _tmp283=0;_npop_handler(0);return _tmp283;}
_LL143:;}};_pop_handler();}else{void*_tmp27A=(void*)_exn_thrown;void*_tmp285=
_tmp27A;_LL14B: {struct Cyc_Dict_Absent_struct*_tmp286=(struct Cyc_Dict_Absent_struct*)
_tmp285;if(_tmp286->tag != Cyc_Dict_Absent)goto _LL14D;}_LL14C: return 0;_LL14D:;
_LL14E:(void)_throw(_tmp285);_LL14A:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
static int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x){struct
_handler_cons _tmp287;_push_handler(& _tmp287);{int _tmp289=0;if(setjmp(_tmp287.handler))
_tmp289=1;if(!_tmp289){{void*_tmp28B;struct _tuple13 _tmp28A=*((struct _tuple13*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp28B=
_tmp28A.f1;LOOP: {void*_tmp28C=_tmp28B;struct _tuple0*_tmp28E;_LL150: {struct Cyc_Absyn_TypedefType_struct*
_tmp28D=(struct Cyc_Absyn_TypedefType_struct*)_tmp28C;if(_tmp28D->tag != 17)goto
_LL152;else{_tmp28E=_tmp28D->f1;}}_LL151: _tmp28B=Cyc_Port_lookup_typedef(env,
_tmp28E);goto LOOP;_LL152: {struct Cyc_Absyn_ArrayType_struct*_tmp28F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp28C;if(_tmp28F->tag != 8)goto _LL154;}_LL153: {int _tmp290=1;_npop_handler(0);
return _tmp290;}_LL154:;_LL155: {int _tmp291=0;_npop_handler(0);return _tmp291;}
_LL14F:;}};_pop_handler();}else{void*_tmp288=(void*)_exn_thrown;void*_tmp293=
_tmp288;_LL157: {struct Cyc_Dict_Absent_struct*_tmp294=(struct Cyc_Dict_Absent_struct*)
_tmp293;if(_tmp294->tag != Cyc_Dict_Absent)goto _LL159;}_LL158: return 0;_LL159:;
_LL15A:(void)_throw(_tmp293);_LL156:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*Cyc_Port_set_cpos(struct Cyc_Port_Cenv*
env,enum Cyc_Port_CPos cpos){struct Cyc_Port_Cenv*_tmp5CC;return(_tmp5CC=_cycalloc(
sizeof(*_tmp5CC)),((_tmp5CC->gcenv=env->gcenv,((_tmp5CC->var_dict=env->var_dict,((
_tmp5CC->cpos=cpos,_tmp5CC)))))));}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
static void Cyc_Port_add_return_type(struct Cyc_Port_Cenv*env,void*t){(env->gcenv)->return_type=
t;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*
Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct _tuple0*x,void*t,void*qual,void*
ctype){struct _tuple13*_tmp5D2;struct _tuple10*_tmp5D1;struct Cyc_Port_Cenv*_tmp5D0;
return(_tmp5D0=_cycalloc(sizeof(*_tmp5D0)),((_tmp5D0->gcenv=env->gcenv,((_tmp5D0->var_dict=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))
Cyc_Dict_insert)(env->var_dict,x,((_tmp5D2=_cycalloc(sizeof(*_tmp5D2)),((_tmp5D2->f1=
t,((_tmp5D2->f2=((_tmp5D1=_cycalloc(sizeof(*_tmp5D1)),((_tmp5D1->f1=qual,((
_tmp5D1->f2=ctype,_tmp5D1)))))),_tmp5D2))))))),((_tmp5D0->cpos=env->cpos,_tmp5D0)))))));}
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*
env,struct _tuple0*n,void*t,void*ct){struct _tuple10*_tmp5D3;(env->gcenv)->typedef_dict=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple10*v))
Cyc_Dict_insert)((env->gcenv)->typedef_dict,n,((_tmp5D3=_cycalloc(sizeof(*
_tmp5D3)),((_tmp5D3->f1=t,((_tmp5D3->f2=ct,_tmp5D3)))))));}struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct _tuple14{void*f1;void*f2;struct Cyc_Position_Segment*f3;
};static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*new_qual,
void*orig_qual,struct Cyc_Position_Segment*loc){struct _tuple14*_tmp5D6;struct Cyc_List_List*
_tmp5D5;(env->gcenv)->qualifier_edits=((_tmp5D5=_cycalloc(sizeof(*_tmp5D5)),((
_tmp5D5->hd=((_tmp5D6=_cycalloc(sizeof(*_tmp5D6)),((_tmp5D6->f1=new_qual,((
_tmp5D6->f2=orig_qual,((_tmp5D6->f3=loc,_tmp5D6)))))))),((_tmp5D5->tl=(env->gcenv)->qualifier_edits,
_tmp5D5))))));}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static void Cyc_Port_register_ptr_cvars(
struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,struct Cyc_Position_Segment*
loc){struct _tuple14*_tmp5D9;struct Cyc_List_List*_tmp5D8;(env->gcenv)->pointer_edits=((
_tmp5D8=_cycalloc(sizeof(*_tmp5D8)),((_tmp5D8->hd=((_tmp5D9=_cycalloc(sizeof(*
_tmp5D9)),((_tmp5D9->f1=new_ptr,((_tmp5D9->f2=orig_ptr,((_tmp5D9->f3=loc,_tmp5D9)))))))),((
_tmp5D8->tl=(env->gcenv)->pointer_edits,_tmp5D8))))));}struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*
env,void*new_zt,void*orig_zt,struct Cyc_Position_Segment*loc){struct _tuple14*
_tmp5DC;struct Cyc_List_List*_tmp5DB;(env->gcenv)->zeroterm_edits=((_tmp5DB=
_cycalloc(sizeof(*_tmp5DB)),((_tmp5DB->hd=((_tmp5DC=_cycalloc(sizeof(*_tmp5DC)),((
_tmp5DC->f1=new_zt,((_tmp5DC->f2=orig_zt,((_tmp5DC->f3=loc,_tmp5DC)))))))),((
_tmp5DB->tl=(env->gcenv)->zeroterm_edits,_tmp5DB))))));}static void*Cyc_Port_main_type(){
struct _tuple8*_tmp5DD;struct _tuple8*arg1=(_tmp5DD=_cycalloc(sizeof(*_tmp5DD)),((
_tmp5DD->f1=0,((_tmp5DD->f2=Cyc_Absyn_empty_tqual(0),((_tmp5DD->f3=Cyc_Absyn_sint_typ,
_tmp5DD)))))));struct _tuple8*_tmp5DE;struct _tuple8*arg2=(_tmp5DE=_cycalloc(
sizeof(*_tmp5DE)),((_tmp5DE->f1=0,((_tmp5DE->f2=Cyc_Absyn_empty_tqual(0),((
_tmp5DE->f3=Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_wildtyp(0)),
Cyc_Absyn_wildtyp(0),Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)()),_tmp5DE)))))));struct Cyc_Absyn_FnType_struct _tmp5E8;
struct _tuple8*_tmp5E7[2];struct Cyc_Absyn_FnInfo _tmp5E6;struct Cyc_Absyn_FnType_struct*
_tmp5E5;return(void*)((_tmp5E5=_cycalloc(sizeof(*_tmp5E5)),((_tmp5E5[0]=((
_tmp5E8.tag=9,((_tmp5E8.f1=((_tmp5E6.tvars=0,((_tmp5E6.effect=0,((_tmp5E6.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmp5E6.ret_typ=Cyc_Absyn_sint_typ,((_tmp5E6.args=((
_tmp5E7[1]=arg2,((_tmp5E7[0]=arg1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp5E7,sizeof(struct _tuple8*),2)))))),((_tmp5E6.c_varargs=
0,((_tmp5E6.cyc_varargs=0,((_tmp5E6.rgn_po=0,((_tmp5E6.attributes=0,_tmp5E6)))))))))))))))))),
_tmp5E8)))),_tmp5E5))));}struct Cyc_Port_Cenv;static void*Cyc_Port_type_to_ctype(
struct Cyc_Port_Cenv*env,void*t);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct
Cyc_Port_Cenv;static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv(){struct Cyc_Port_Cenv*
_tmp2A6=Cyc_Port_empty_cenv();{struct _tuple0*_tmp5E9;_tmp2A6=Cyc_Port_add_var(
_tmp2A6,((_tmp5E9=_cycalloc(sizeof(*_tmp5E9)),((_tmp5E9->f1=Cyc_Absyn_Loc_n,((
_tmp5E9->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"main",
sizeof(char),5),_tmp5E9)))))),Cyc_Port_main_type(),Cyc_Port_const_ct(),Cyc_Port_type_to_ctype(
_tmp2A6,Cyc_Port_main_type()));}return _tmp2A6;}static void Cyc_Port_region_counts(
struct Cyc_Dict_Dict*cts,void*t){void*_tmp2A9=Cyc_Port_compress_ct(t);struct
_dyneither_ptr*_tmp2B8;void*_tmp2BA;void*_tmp2BB;void*_tmp2BC;void*_tmp2BD;void*
_tmp2BE;void*_tmp2C0;void*_tmp2C1;void*_tmp2C2;void*_tmp2C4;struct Cyc_List_List*
_tmp2C5;_LL15C: {struct Cyc_Port_Const_ct_struct*_tmp2AA=(struct Cyc_Port_Const_ct_struct*)
_tmp2A9;if(_tmp2AA->tag != 0)goto _LL15E;}_LL15D: goto _LL15F;_LL15E: {struct Cyc_Port_Notconst_ct_struct*
_tmp2AB=(struct Cyc_Port_Notconst_ct_struct*)_tmp2A9;if(_tmp2AB->tag != 1)goto
_LL160;}_LL15F: goto _LL161;_LL160: {struct Cyc_Port_Thin_ct_struct*_tmp2AC=(struct
Cyc_Port_Thin_ct_struct*)_tmp2A9;if(_tmp2AC->tag != 2)goto _LL162;}_LL161: goto
_LL163;_LL162: {struct Cyc_Port_Fat_ct_struct*_tmp2AD=(struct Cyc_Port_Fat_ct_struct*)
_tmp2A9;if(_tmp2AD->tag != 3)goto _LL164;}_LL163: goto _LL165;_LL164: {struct Cyc_Port_Void_ct_struct*
_tmp2AE=(struct Cyc_Port_Void_ct_struct*)_tmp2A9;if(_tmp2AE->tag != 4)goto _LL166;}
_LL165: goto _LL167;_LL166: {struct Cyc_Port_Zero_ct_struct*_tmp2AF=(struct Cyc_Port_Zero_ct_struct*)
_tmp2A9;if(_tmp2AF->tag != 5)goto _LL168;}_LL167: goto _LL169;_LL168: {struct Cyc_Port_Arith_ct_struct*
_tmp2B0=(struct Cyc_Port_Arith_ct_struct*)_tmp2A9;if(_tmp2B0->tag != 6)goto _LL16A;}
_LL169: goto _LL16B;_LL16A: {struct Cyc_Port_UnknownAggr_ct_struct*_tmp2B1=(struct
Cyc_Port_UnknownAggr_ct_struct*)_tmp2A9;if(_tmp2B1->tag != 14)goto _LL16C;}_LL16B:
goto _LL16D;_LL16C: {struct Cyc_Port_KnownAggr_ct_struct*_tmp2B2=(struct Cyc_Port_KnownAggr_ct_struct*)
_tmp2A9;if(_tmp2B2->tag != 13)goto _LL16E;}_LL16D: goto _LL16F;_LL16E: {struct Cyc_Port_Var_ct_struct*
_tmp2B3=(struct Cyc_Port_Var_ct_struct*)_tmp2A9;if(_tmp2B3->tag != 16)goto _LL170;}
_LL16F: goto _LL171;_LL170: {struct Cyc_Port_Zterm_ct_struct*_tmp2B4=(struct Cyc_Port_Zterm_ct_struct*)
_tmp2A9;if(_tmp2B4->tag != 8)goto _LL172;}_LL171: goto _LL173;_LL172: {struct Cyc_Port_Nozterm_ct_struct*
_tmp2B5=(struct Cyc_Port_Nozterm_ct_struct*)_tmp2A9;if(_tmp2B5->tag != 9)goto
_LL174;}_LL173: goto _LL175;_LL174: {struct Cyc_Port_Heap_ct_struct*_tmp2B6=(struct
Cyc_Port_Heap_ct_struct*)_tmp2A9;if(_tmp2B6->tag != 7)goto _LL176;}_LL175: return;
_LL176: {struct Cyc_Port_RgnVar_ct_struct*_tmp2B7=(struct Cyc_Port_RgnVar_ct_struct*)
_tmp2A9;if(_tmp2B7->tag != 10)goto _LL178;else{_tmp2B8=_tmp2B7->f1;}}_LL177: if(!((
int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*cts,
_tmp2B8)){int*_tmp5EA;*cts=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,int*v))Cyc_Dict_insert)(*cts,_tmp2B8,(int*)((_tmp5EA=
_cycalloc_atomic(sizeof(*_tmp5EA)),((_tmp5EA[0]=0,_tmp5EA)))));}{int*_tmp2C7=((
int*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*cts,
_tmp2B8);*_tmp2C7=*_tmp2C7 + 1;return;};_LL178: {struct Cyc_Port_Ptr_ct_struct*
_tmp2B9=(struct Cyc_Port_Ptr_ct_struct*)_tmp2A9;if(_tmp2B9->tag != 11)goto _LL17A;
else{_tmp2BA=(void*)_tmp2B9->f1;_tmp2BB=(void*)_tmp2B9->f2;_tmp2BC=(void*)
_tmp2B9->f3;_tmp2BD=(void*)_tmp2B9->f4;_tmp2BE=(void*)_tmp2B9->f5;}}_LL179: Cyc_Port_region_counts(
cts,_tmp2BA);Cyc_Port_region_counts(cts,_tmp2BD);return;_LL17A: {struct Cyc_Port_Array_ct_struct*
_tmp2BF=(struct Cyc_Port_Array_ct_struct*)_tmp2A9;if(_tmp2BF->tag != 12)goto _LL17C;
else{_tmp2C0=(void*)_tmp2BF->f1;_tmp2C1=(void*)_tmp2BF->f2;_tmp2C2=(void*)
_tmp2BF->f3;}}_LL17B: Cyc_Port_region_counts(cts,_tmp2C0);return;_LL17C: {struct
Cyc_Port_Fn_ct_struct*_tmp2C3=(struct Cyc_Port_Fn_ct_struct*)_tmp2A9;if(_tmp2C3->tag
!= 15)goto _LL15B;else{_tmp2C4=(void*)_tmp2C3->f1;_tmp2C5=_tmp2C3->f2;}}_LL17D:
Cyc_Port_region_counts(cts,_tmp2C4);for(0;(unsigned int)_tmp2C5;_tmp2C5=_tmp2C5->tl){
Cyc_Port_region_counts(cts,(void*)_tmp2C5->hd);}return;_LL15B:;}struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static void Cyc_Port_register_rgns(struct Cyc_Port_Cenv*env,
struct Cyc_Dict_Dict counts,int fn_res,void*t,void*c){c=Cyc_Port_compress_ct(c);{
struct _tuple10 _tmp5EB;struct _tuple10 _tmp2C9=(_tmp5EB.f1=t,((_tmp5EB.f2=c,_tmp5EB)));
void*_tmp2CA;struct Cyc_Absyn_PtrInfo _tmp2CC;void*_tmp2CD;struct Cyc_Absyn_PtrAtts
_tmp2CE;void*_tmp2CF;struct Cyc_Absyn_PtrLoc*_tmp2D0;void*_tmp2D1;void*_tmp2D3;
void*_tmp2D4;void*_tmp2D5;struct Cyc_Absyn_ArrayInfo _tmp2D7;void*_tmp2D8;void*
_tmp2D9;void*_tmp2DB;void*_tmp2DC;struct Cyc_Absyn_FnInfo _tmp2DE;void*_tmp2DF;
struct Cyc_List_List*_tmp2E0;void*_tmp2E1;void*_tmp2E3;struct Cyc_List_List*
_tmp2E4;_LL17F: _tmp2CA=_tmp2C9.f1;{struct Cyc_Absyn_PointerType_struct*_tmp2CB=(
struct Cyc_Absyn_PointerType_struct*)_tmp2CA;if(_tmp2CB->tag != 5)goto _LL181;else{
_tmp2CC=_tmp2CB->f1;_tmp2CD=_tmp2CC.elt_typ;_tmp2CE=_tmp2CC.ptr_atts;_tmp2CF=
_tmp2CE.rgn;_tmp2D0=_tmp2CE.ptrloc;}};_tmp2D1=_tmp2C9.f2;{struct Cyc_Port_Ptr_ct_struct*
_tmp2D2=(struct Cyc_Port_Ptr_ct_struct*)_tmp2D1;if(_tmp2D2->tag != 11)goto _LL181;
else{_tmp2D3=(void*)_tmp2D2->f1;_tmp2D4=(void*)_tmp2D2->f4;}};_LL180: Cyc_Port_register_rgns(
env,counts,fn_res,_tmp2CD,_tmp2D3);{struct Cyc_Position_Segment*_tmp2E5=(
unsigned int)_tmp2D0?_tmp2D0->rgn_loc: 0;_tmp2D4=Cyc_Port_compress_ct(_tmp2D4);{
struct _tuple10 _tmp5EC;struct _tuple10 _tmp2E7=(_tmp5EC.f1=_tmp2CF,((_tmp5EC.f2=
_tmp2D4,_tmp5EC)));void*_tmp2E8;void*_tmp2EA;void*_tmp2EC;void*_tmp2EE;struct
_dyneither_ptr*_tmp2F0;_LL188: _tmp2E8=_tmp2E7.f1;{struct Cyc_Absyn_Evar_struct*
_tmp2E9=(struct Cyc_Absyn_Evar_struct*)_tmp2E8;if(_tmp2E9->tag != 1)goto _LL18A;};
_tmp2EA=_tmp2E7.f2;{struct Cyc_Port_Heap_ct_struct*_tmp2EB=(struct Cyc_Port_Heap_ct_struct*)
_tmp2EA;if(_tmp2EB->tag != 7)goto _LL18A;};if(!(!fn_res))goto _LL18A;_LL189:{struct
Cyc_Port_Edit*_tmp5F5;const char*_tmp5F4;const char*_tmp5F3;struct Cyc_List_List*
_tmp5F2;(env->gcenv)->edits=((_tmp5F2=_cycalloc(sizeof(*_tmp5F2)),((_tmp5F2->hd=((
_tmp5F5=_cycalloc(sizeof(*_tmp5F5)),((_tmp5F5->loc=_tmp2E5,((_tmp5F5->old_string=((
_tmp5F4="`H ",_tag_dyneither(_tmp5F4,sizeof(char),4))),((_tmp5F5->new_string=((
_tmp5F3="",_tag_dyneither(_tmp5F3,sizeof(char),1))),_tmp5F5)))))))),((_tmp5F2->tl=(
env->gcenv)->edits,_tmp5F2))))));}goto _LL187;_LL18A: _tmp2EC=_tmp2E7.f1;{struct
Cyc_Absyn_Evar_struct*_tmp2ED=(struct Cyc_Absyn_Evar_struct*)_tmp2EC;if(_tmp2ED->tag
!= 1)goto _LL18C;};_tmp2EE=_tmp2E7.f2;{struct Cyc_Port_RgnVar_ct_struct*_tmp2EF=(
struct Cyc_Port_RgnVar_ct_struct*)_tmp2EE;if(_tmp2EF->tag != 10)goto _LL18C;else{
_tmp2F0=_tmp2EF->f1;}};_LL18B: {int _tmp2F5=*((int*(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_lookup)(counts,_tmp2F0);if(_tmp2F5 > 1){struct
Cyc_Port_Edit*_tmp605;const char*_tmp604;void*_tmp603[1];struct Cyc_String_pa_struct
_tmp602;const char*_tmp601;struct Cyc_List_List*_tmp600;(env->gcenv)->edits=((
_tmp600=_cycalloc(sizeof(*_tmp600)),((_tmp600->hd=((_tmp605=_cycalloc(sizeof(*
_tmp605)),((_tmp605->loc=_tmp2E5,((_tmp605->old_string=(struct _dyneither_ptr)((
_tmp602.tag=0,((_tmp602.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2F0),((
_tmp603[0]=& _tmp602,Cyc_aprintf(((_tmp604="%s ",_tag_dyneither(_tmp604,sizeof(
char),4))),_tag_dyneither(_tmp603,sizeof(void*),1)))))))),((_tmp605->new_string=((
_tmp601="",_tag_dyneither(_tmp601,sizeof(char),1))),_tmp605)))))))),((_tmp600->tl=(
env->gcenv)->edits,_tmp600))))));}goto _LL187;}_LL18C:;_LL18D: goto _LL187;_LL187:;}
goto _LL17E;};_LL181: _tmp2D5=_tmp2C9.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp2D6=(
struct Cyc_Absyn_ArrayType_struct*)_tmp2D5;if(_tmp2D6->tag != 8)goto _LL183;else{
_tmp2D7=_tmp2D6->f1;_tmp2D8=_tmp2D7.elt_type;}};_tmp2D9=_tmp2C9.f2;{struct Cyc_Port_Array_ct_struct*
_tmp2DA=(struct Cyc_Port_Array_ct_struct*)_tmp2D9;if(_tmp2DA->tag != 12)goto _LL183;
else{_tmp2DB=(void*)_tmp2DA->f1;}};_LL182: Cyc_Port_register_rgns(env,counts,
fn_res,_tmp2D8,_tmp2DB);goto _LL17E;_LL183: _tmp2DC=_tmp2C9.f1;{struct Cyc_Absyn_FnType_struct*
_tmp2DD=(struct Cyc_Absyn_FnType_struct*)_tmp2DC;if(_tmp2DD->tag != 9)goto _LL185;
else{_tmp2DE=_tmp2DD->f1;_tmp2DF=_tmp2DE.ret_typ;_tmp2E0=_tmp2DE.args;}};_tmp2E1=
_tmp2C9.f2;{struct Cyc_Port_Fn_ct_struct*_tmp2E2=(struct Cyc_Port_Fn_ct_struct*)
_tmp2E1;if(_tmp2E2->tag != 15)goto _LL185;else{_tmp2E3=(void*)_tmp2E2->f1;_tmp2E4=
_tmp2E2->f2;}};_LL184: Cyc_Port_register_rgns(env,counts,1,_tmp2DF,_tmp2E3);for(0;
_tmp2E0 != 0  && _tmp2E4 != 0;(_tmp2E0=_tmp2E0->tl,_tmp2E4=_tmp2E4->tl)){void*
_tmp2FD;struct _tuple8 _tmp2FC=*((struct _tuple8*)_tmp2E0->hd);_tmp2FD=_tmp2FC.f3;{
void*_tmp2FE=(void*)_tmp2E4->hd;Cyc_Port_register_rgns(env,counts,0,_tmp2FD,
_tmp2FE);};}goto _LL17E;_LL185:;_LL186: goto _LL17E;_LL17E:;};}struct Cyc_Port_Cenv;
static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e);
struct Cyc_Port_Cenv;static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct
Cyc_Absyn_Stmt*s);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*
Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d);struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t){
void*_tmp2FF=t;struct _tuple0*_tmp301;enum Cyc_Absyn_Size_of _tmp303;_LL18F: {
struct Cyc_Absyn_TypedefType_struct*_tmp300=(struct Cyc_Absyn_TypedefType_struct*)
_tmp2FF;if(_tmp300->tag != 17)goto _LL191;else{_tmp301=_tmp300->f1;}}_LL190:(*
_tmp301).f1=Cyc_Absyn_Loc_n;return Cyc_Port_is_char(env,Cyc_Port_lookup_typedef(
env,_tmp301));_LL191: {struct Cyc_Absyn_IntType_struct*_tmp302=(struct Cyc_Absyn_IntType_struct*)
_tmp2FF;if(_tmp302->tag != 6)goto _LL193;else{_tmp303=_tmp302->f2;}}_LL192: return 1;
_LL193:;_LL194: return 0;_LL18E:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct
Cyc_Port_Cenv;static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){
void*_tmp304=t;struct _tuple0*_tmp306;struct Cyc_Absyn_PtrInfo _tmp309;void*_tmp30A;
struct Cyc_Absyn_Tqual _tmp30B;struct Cyc_Absyn_PtrAtts _tmp30C;void*_tmp30D;union
Cyc_Absyn_Constraint*_tmp30E;union Cyc_Absyn_Constraint*_tmp30F;union Cyc_Absyn_Constraint*
_tmp310;struct Cyc_Absyn_PtrLoc*_tmp311;struct Cyc_Absyn_ArrayInfo _tmp315;void*
_tmp316;struct Cyc_Absyn_Tqual _tmp317;union Cyc_Absyn_Constraint*_tmp318;struct Cyc_Position_Segment*
_tmp319;struct Cyc_Absyn_FnInfo _tmp31B;void*_tmp31C;struct Cyc_List_List*_tmp31D;
struct Cyc_Absyn_AggrInfo _tmp31F;union Cyc_Absyn_AggrInfoU _tmp320;struct Cyc_List_List*
_tmp323;_LL196: {struct Cyc_Absyn_TypedefType_struct*_tmp305=(struct Cyc_Absyn_TypedefType_struct*)
_tmp304;if(_tmp305->tag != 17)goto _LL198;else{_tmp306=_tmp305->f1;}}_LL197:(*
_tmp306).f1=Cyc_Absyn_Loc_n;return Cyc_Port_lookup_typedef_ctype(env,_tmp306);
_LL198: {struct Cyc_Absyn_VoidType_struct*_tmp307=(struct Cyc_Absyn_VoidType_struct*)
_tmp304;if(_tmp307->tag != 0)goto _LL19A;}_LL199: return Cyc_Port_void_ct();_LL19A: {
struct Cyc_Absyn_PointerType_struct*_tmp308=(struct Cyc_Absyn_PointerType_struct*)
_tmp304;if(_tmp308->tag != 5)goto _LL19C;else{_tmp309=_tmp308->f1;_tmp30A=_tmp309.elt_typ;
_tmp30B=_tmp309.elt_tq;_tmp30C=_tmp309.ptr_atts;_tmp30D=_tmp30C.rgn;_tmp30E=
_tmp30C.nullable;_tmp30F=_tmp30C.bounds;_tmp310=_tmp30C.zero_term;_tmp311=
_tmp30C.ptrloc;}}_LL19B: {struct Cyc_Position_Segment*_tmp324=(unsigned int)
_tmp311?_tmp311->ptr_loc: 0;struct Cyc_Position_Segment*_tmp325=(unsigned int)
_tmp311?_tmp311->rgn_loc: 0;struct Cyc_Position_Segment*_tmp326=(unsigned int)
_tmp311?_tmp311->zt_loc: 0;void*_tmp327=Cyc_Port_type_to_ctype(env,_tmp30A);void*
new_rgn;{void*_tmp328=_tmp30D;struct Cyc_Absyn_Tvar*_tmp32B;struct Cyc_Absyn_Tvar
_tmp32C;struct _dyneither_ptr*_tmp32D;_LL1AD: {struct Cyc_Absyn_HeapRgn_struct*
_tmp329=(struct Cyc_Absyn_HeapRgn_struct*)_tmp328;if(_tmp329->tag != 20)goto _LL1AF;}
_LL1AE: new_rgn=Cyc_Port_heap_ct();goto _LL1AC;_LL1AF: {struct Cyc_Absyn_VarType_struct*
_tmp32A=(struct Cyc_Absyn_VarType_struct*)_tmp328;if(_tmp32A->tag != 2)goto _LL1B1;
else{_tmp32B=_tmp32A->f1;_tmp32C=*_tmp32B;_tmp32D=_tmp32C.name;}}_LL1B0: new_rgn=
Cyc_Port_rgnvar_ct(_tmp32D);goto _LL1AC;_LL1B1: {struct Cyc_Absyn_Evar_struct*
_tmp32E=(struct Cyc_Absyn_Evar_struct*)_tmp328;if(_tmp32E->tag != 1)goto _LL1B3;}
_LL1B2: if(Cyc_Port_in_fn_arg(env))new_rgn=Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var());
else{if(Cyc_Port_in_fn_res(env) || Cyc_Port_in_toplevel(env))new_rgn=Cyc_Port_heap_ct();
else{new_rgn=Cyc_Port_var();}}goto _LL1AC;_LL1B3:;_LL1B4: new_rgn=Cyc_Port_heap_ct();
goto _LL1AC;_LL1AC:;}{int _tmp32F=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp30F)== (void*)& Cyc_Absyn_DynEither_b_val;int orig_zt;{union Cyc_Absyn_Constraint
_tmp330=*_tmp310;int _tmp331;_LL1B6: if((_tmp330.No_constr).tag != 3)goto _LL1B8;
_tmp331=(int)(_tmp330.No_constr).val;_LL1B7: orig_zt=Cyc_Port_is_char(env,_tmp30A);
goto _LL1B5;_LL1B8:;_LL1B9: orig_zt=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp310);goto _LL1B5;_LL1B5:;}if((env->gcenv)->porting){void*_tmp332=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,_tmp332,_tmp30B.print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct(),_tmp30B.loc);if(_tmp30B.print_const)Cyc_Port_unify_ct(
_tmp332,Cyc_Port_const_ct());{void*_tmp333=Cyc_Port_var();Cyc_Port_register_ptr_cvars(
env,_tmp333,_tmp32F?Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),_tmp324);if(_tmp32F)Cyc_Port_unify_ct(
_tmp333,Cyc_Port_fat_ct());{void*_tmp334=Cyc_Port_var();Cyc_Port_register_zeroterm_cvars(
env,_tmp334,orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),_tmp326);{union Cyc_Absyn_Constraint
_tmp335=*_tmp310;int _tmp336;_LL1BB: if((_tmp335.No_constr).tag != 3)goto _LL1BD;
_tmp336=(int)(_tmp335.No_constr).val;_LL1BC: goto _LL1BA;_LL1BD:;_LL1BE: Cyc_Port_unify_ct(
_tmp334,((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp310)?Cyc_Port_zterm_ct():
Cyc_Port_nozterm_ct());goto _LL1BA;_LL1BA:;}return Cyc_Port_ptr_ct(_tmp327,_tmp332,
_tmp333,new_rgn,_tmp334);};};}else{return Cyc_Port_ptr_ct(_tmp327,_tmp30B.print_const?
Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp32F?Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),
new_rgn,orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}};}_LL19C: {struct
Cyc_Absyn_IntType_struct*_tmp312=(struct Cyc_Absyn_IntType_struct*)_tmp304;if(
_tmp312->tag != 6)goto _LL19E;}_LL19D: goto _LL19F;_LL19E: {struct Cyc_Absyn_FloatType_struct*
_tmp313=(struct Cyc_Absyn_FloatType_struct*)_tmp304;if(_tmp313->tag != 7)goto
_LL1A0;}_LL19F: return Cyc_Port_arith_ct();_LL1A0: {struct Cyc_Absyn_ArrayType_struct*
_tmp314=(struct Cyc_Absyn_ArrayType_struct*)_tmp304;if(_tmp314->tag != 8)goto
_LL1A2;else{_tmp315=_tmp314->f1;_tmp316=_tmp315.elt_type;_tmp317=_tmp315.tq;
_tmp318=_tmp315.zero_term;_tmp319=_tmp315.zt_loc;}}_LL1A1: {void*_tmp337=Cyc_Port_type_to_ctype(
env,_tmp316);int orig_zt;{union Cyc_Absyn_Constraint _tmp338=*_tmp318;int _tmp339;
_LL1C0: if((_tmp338.No_constr).tag != 3)goto _LL1C2;_tmp339=(int)(_tmp338.No_constr).val;
_LL1C1: orig_zt=0;goto _LL1BF;_LL1C2:;_LL1C3: orig_zt=((int(*)(union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_val)(_tmp318);goto _LL1BF;_LL1BF:;}if((env->gcenv)->porting){
void*_tmp33A=Cyc_Port_var();Cyc_Port_register_const_cvar(env,_tmp33A,_tmp317.print_const?
Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp317.loc);{void*_tmp33B=Cyc_Port_var();
Cyc_Port_register_zeroterm_cvars(env,_tmp33B,orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),
_tmp319);{union Cyc_Absyn_Constraint _tmp33C=*_tmp318;int _tmp33D;_LL1C5: if((
_tmp33C.No_constr).tag != 3)goto _LL1C7;_tmp33D=(int)(_tmp33C.No_constr).val;
_LL1C6: goto _LL1C4;_LL1C7:;_LL1C8: Cyc_Port_unify_ct(_tmp33B,((int(*)(union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_val)(_tmp318)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());
goto _LL1C4;_LL1C4:;}return Cyc_Port_array_ct(_tmp337,_tmp33A,_tmp33B);};}else{
return Cyc_Port_array_ct(_tmp337,_tmp317.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}}_LL1A2: {struct Cyc_Absyn_FnType_struct*
_tmp31A=(struct Cyc_Absyn_FnType_struct*)_tmp304;if(_tmp31A->tag != 9)goto _LL1A4;
else{_tmp31B=_tmp31A->f1;_tmp31C=_tmp31B.ret_typ;_tmp31D=_tmp31B.args;}}_LL1A3: {
void*_tmp33E=Cyc_Port_type_to_ctype(Cyc_Port_set_cpos(env,Cyc_Port_FnRes_pos),
_tmp31C);struct Cyc_Port_Cenv*_tmp33F=Cyc_Port_set_cpos(env,Cyc_Port_FnArg_pos);
struct Cyc_List_List*cargs=0;for(0;_tmp31D != 0;_tmp31D=_tmp31D->tl){struct Cyc_Absyn_Tqual
_tmp341;void*_tmp342;struct _tuple8 _tmp340=*((struct _tuple8*)_tmp31D->hd);_tmp341=
_tmp340.f2;_tmp342=_tmp340.f3;{struct Cyc_List_List*_tmp606;cargs=((_tmp606=
_cycalloc(sizeof(*_tmp606)),((_tmp606->hd=Cyc_Port_type_to_ctype(_tmp33F,_tmp342),((
_tmp606->tl=cargs,_tmp606))))));};}return Cyc_Port_fn_ct(_tmp33E,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(cargs));}_LL1A4: {struct Cyc_Absyn_AggrType_struct*
_tmp31E=(struct Cyc_Absyn_AggrType_struct*)_tmp304;if(_tmp31E->tag != 11)goto
_LL1A6;else{_tmp31F=_tmp31E->f1;_tmp320=_tmp31F.aggr_info;}}_LL1A5: {union Cyc_Absyn_AggrInfoU
_tmp344=_tmp320;struct _tuple2 _tmp345;enum Cyc_Absyn_AggrKind _tmp346;struct
_tuple0*_tmp347;struct _tuple2 _tmp348;enum Cyc_Absyn_AggrKind _tmp349;struct
_tuple0*_tmp34A;struct Cyc_Absyn_Aggrdecl**_tmp34B;_LL1CA: if((_tmp344.UnknownAggr).tag
!= 1)goto _LL1CC;_tmp345=(struct _tuple2)(_tmp344.UnknownAggr).val;_tmp346=_tmp345.f1;
if(_tmp346 != Cyc_Absyn_StructA)goto _LL1CC;_tmp347=_tmp345.f2;_LL1CB:(*_tmp347).f1=
Cyc_Absyn_Loc_n;{struct _tuple11*_tmp34C=Cyc_Port_lookup_struct_decl(env,_tmp347);
return Cyc_Port_known_aggr_ct(_tmp34C);};_LL1CC: if((_tmp344.UnknownAggr).tag != 1)
goto _LL1CE;_tmp348=(struct _tuple2)(_tmp344.UnknownAggr).val;_tmp349=_tmp348.f1;
if(_tmp349 != Cyc_Absyn_UnionA)goto _LL1CE;_tmp34A=_tmp348.f2;_LL1CD:(*_tmp34A).f1=
Cyc_Absyn_Loc_n;{struct _tuple11*_tmp34D=Cyc_Port_lookup_union_decl(env,_tmp34A);
return Cyc_Port_known_aggr_ct(_tmp34D);};_LL1CE: if((_tmp344.KnownAggr).tag != 2)
goto _LL1C9;_tmp34B=(struct Cyc_Absyn_Aggrdecl**)(_tmp344.KnownAggr).val;_LL1CF: {
struct Cyc_Absyn_Aggrdecl*_tmp34E=*_tmp34B;switch(_tmp34E->kind){case Cyc_Absyn_StructA:
_LL1D0: {struct _tuple11*_tmp34F=Cyc_Port_lookup_struct_decl(env,_tmp34E->name);
return Cyc_Port_known_aggr_ct(_tmp34F);}case Cyc_Absyn_UnionA: _LL1D1: {struct
_tuple11*_tmp350=Cyc_Port_lookup_union_decl(env,_tmp34E->name);return Cyc_Port_known_aggr_ct(
_tmp350);}}}_LL1C9:;}_LL1A6: {struct Cyc_Absyn_EnumType_struct*_tmp321=(struct Cyc_Absyn_EnumType_struct*)
_tmp304;if(_tmp321->tag != 13)goto _LL1A8;}_LL1A7: return Cyc_Port_arith_ct();_LL1A8: {
struct Cyc_Absyn_AnonEnumType_struct*_tmp322=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp304;if(_tmp322->tag != 14)goto _LL1AA;else{_tmp323=_tmp322->f1;}}_LL1A9: for(0;(
unsigned int)_tmp323;_tmp323=_tmp323->tl){(*((struct Cyc_Absyn_Enumfield*)_tmp323->hd)->name).f1=
Cyc_Absyn_Loc_n;env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)_tmp323->hd)->name,
Cyc_Absyn_sint_typ,Cyc_Port_const_ct(),Cyc_Port_arith_ct());}return Cyc_Port_arith_ct();
_LL1AA:;_LL1AB: return Cyc_Port_arith_ct();_LL195:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,enum Cyc_Absyn_Primop p,
struct Cyc_List_List*args){void*_tmp351=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)
_check_null(args))->hd);struct Cyc_List_List*_tmp352=args->tl;enum Cyc_Absyn_Primop
_tmp353=p;_LL1D4: if(_tmp353 != Cyc_Absyn_Plus)goto _LL1D6;_LL1D5: {void*_tmp354=
Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(_tmp352))->hd);
if(Cyc_Port_leq(_tmp351,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()))){Cyc_Port_leq(_tmp354,Cyc_Port_arith_ct());
return _tmp351;}else{if(Cyc_Port_leq(_tmp354,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),
Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()))){Cyc_Port_leq(_tmp351,Cyc_Port_arith_ct());
return _tmp354;}else{Cyc_Port_leq(_tmp351,Cyc_Port_arith_ct());Cyc_Port_leq(
_tmp354,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();}}}_LL1D6: if(_tmp353 != 
Cyc_Absyn_Minus)goto _LL1D8;_LL1D7: if(_tmp352 == 0){Cyc_Port_leq(_tmp351,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();}else{void*_tmp355=Cyc_Port_compress_ct((void*)_tmp352->hd);
if(Cyc_Port_leq(_tmp351,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()))){if(Cyc_Port_leq(_tmp355,Cyc_Port_ptr_ct(Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var())))return Cyc_Port_arith_ct();
else{Cyc_Port_leq(_tmp355,Cyc_Port_arith_ct());return _tmp351;}}else{Cyc_Port_leq(
_tmp351,Cyc_Port_arith_ct());Cyc_Port_leq(_tmp351,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();}}
_LL1D8: if(_tmp353 != Cyc_Absyn_Times)goto _LL1DA;_LL1D9: goto _LL1DB;_LL1DA: if(
_tmp353 != Cyc_Absyn_Div)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if(_tmp353 != Cyc_Absyn_Mod)
goto _LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(_tmp353 != Cyc_Absyn_Eq)goto _LL1E0;_LL1DF:
goto _LL1E1;_LL1E0: if(_tmp353 != Cyc_Absyn_Neq)goto _LL1E2;_LL1E1: goto _LL1E3;_LL1E2:
if(_tmp353 != Cyc_Absyn_Lt)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4: if(_tmp353 != Cyc_Absyn_Gt)
goto _LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp353 != Cyc_Absyn_Lte)goto _LL1E8;_LL1E7:
goto _LL1E9;_LL1E8: if(_tmp353 != Cyc_Absyn_Gte)goto _LL1EA;_LL1E9: goto _LL1EB;_LL1EA:
if(_tmp353 != Cyc_Absyn_Bitand)goto _LL1EC;_LL1EB: goto _LL1ED;_LL1EC: if(_tmp353 != 
Cyc_Absyn_Bitor)goto _LL1EE;_LL1ED: goto _LL1EF;_LL1EE: if(_tmp353 != Cyc_Absyn_Bitxor)
goto _LL1F0;_LL1EF: goto _LL1F1;_LL1F0: if(_tmp353 != Cyc_Absyn_Bitlshift)goto _LL1F2;
_LL1F1: goto _LL1F3;_LL1F2: if(_tmp353 != Cyc_Absyn_Bitlrshift)goto _LL1F4;_LL1F3:
goto _LL1F5;_LL1F4: if(_tmp353 != Cyc_Absyn_Bitarshift)goto _LL1F6;_LL1F5: Cyc_Port_leq(
_tmp351,Cyc_Port_arith_ct());Cyc_Port_leq((void*)((struct Cyc_List_List*)
_check_null(_tmp352))->hd,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();_LL1F6:
if(_tmp353 != Cyc_Absyn_Not)goto _LL1F8;_LL1F7: goto _LL1F9;_LL1F8: if(_tmp353 != Cyc_Absyn_Bitnot)
goto _LL1FA;_LL1F9: Cyc_Port_leq(_tmp351,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();
_LL1FA:;_LL1FB: {const char*_tmp609;void*_tmp608;(_tmp608=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp609=".size primop",
_tag_dyneither(_tmp609,sizeof(char),13))),_tag_dyneither(_tmp608,sizeof(void*),0)));}
_LL1D3:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;static struct _tuple10 Cyc_Port_gen_lhs(
struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){void*_tmp358=e->r;struct _tuple0*
_tmp35A;struct _tuple0*_tmp35C;struct Cyc_Absyn_Exp*_tmp35E;struct Cyc_Absyn_Exp*
_tmp35F;struct Cyc_Absyn_Exp*_tmp361;struct Cyc_Absyn_Exp*_tmp363;struct
_dyneither_ptr*_tmp364;struct Cyc_Absyn_Exp*_tmp366;struct _dyneither_ptr*_tmp367;
_LL1FD: {struct Cyc_Absyn_Var_e_struct*_tmp359=(struct Cyc_Absyn_Var_e_struct*)
_tmp358;if(_tmp359->tag != 1)goto _LL1FF;else{_tmp35A=_tmp359->f1;}}_LL1FE: _tmp35C=
_tmp35A;goto _LL200;_LL1FF: {struct Cyc_Absyn_UnknownId_e_struct*_tmp35B=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp358;if(_tmp35B->tag != 2)goto _LL201;else{
_tmp35C=_tmp35B->f1;}}_LL200:(*_tmp35C).f1=Cyc_Absyn_Loc_n;return Cyc_Port_lookup_var(
env,_tmp35C);_LL201: {struct Cyc_Absyn_Subscript_e_struct*_tmp35D=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp358;if(_tmp35D->tag != 24)goto _LL203;else{_tmp35E=_tmp35D->f1;_tmp35F=_tmp35D->f2;}}
_LL202: {void*_tmp368=Cyc_Port_var();void*_tmp369=Cyc_Port_var();void*_tmp36A=
Cyc_Port_gen_exp(env,_tmp35E);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp35F),Cyc_Port_arith_ct());{
void*_tmp36B=Cyc_Port_ptr_ct(_tmp368,_tmp369,Cyc_Port_fat_ct(),Cyc_Port_var(),
Cyc_Port_var());Cyc_Port_leq(_tmp36A,_tmp36B);{struct _tuple10 _tmp60A;return(
_tmp60A.f1=_tmp369,((_tmp60A.f2=_tmp368,_tmp60A)));};};}_LL203: {struct Cyc_Absyn_Deref_e_struct*
_tmp360=(struct Cyc_Absyn_Deref_e_struct*)_tmp358;if(_tmp360->tag != 21)goto _LL205;
else{_tmp361=_tmp360->f1;}}_LL204: {void*_tmp36D=Cyc_Port_var();void*_tmp36E=Cyc_Port_var();
void*_tmp36F=Cyc_Port_ptr_ct(_tmp36D,_tmp36E,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var());
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp361),_tmp36F);{struct _tuple10 _tmp60B;
return(_tmp60B.f1=_tmp36E,((_tmp60B.f2=_tmp36D,_tmp60B)));};}_LL205: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp362=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp358;if(_tmp362->tag != 22)goto
_LL207;else{_tmp363=_tmp362->f1;_tmp364=_tmp362->f2;}}_LL206: {void*_tmp371=Cyc_Port_var();
void*_tmp372=Cyc_Port_var();void*_tmp373=Cyc_Port_gen_exp(env,_tmp363);{struct
Cyc_Port_Cfield*_tmp60E;struct Cyc_Port_Cfield*_tmp60D[1];Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp363),Cyc_Port_unknown_aggr_ct(((_tmp60D[0]=((_tmp60E=_cycalloc(sizeof(*
_tmp60E)),((_tmp60E->qual=_tmp372,((_tmp60E->f=_tmp364,((_tmp60E->type=_tmp371,
_tmp60E)))))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp60D,sizeof(struct Cyc_Port_Cfield*),1))))));}{struct _tuple10
_tmp60F;return(_tmp60F.f1=_tmp372,((_tmp60F.f2=_tmp371,_tmp60F)));};}_LL207: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp365=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp358;if(_tmp365->tag != 23)goto _LL209;else{_tmp366=_tmp365->f1;_tmp367=_tmp365->f2;}}
_LL208: {void*_tmp377=Cyc_Port_var();void*_tmp378=Cyc_Port_var();void*_tmp379=
Cyc_Port_gen_exp(env,_tmp366);{struct Cyc_Port_Cfield*_tmp612;struct Cyc_Port_Cfield*
_tmp611[1];Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp366),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(((
_tmp611[0]=((_tmp612=_cycalloc(sizeof(*_tmp612)),((_tmp612->qual=_tmp378,((
_tmp612->f=_tmp367,((_tmp612->type=_tmp377,_tmp612)))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp611,sizeof(struct Cyc_Port_Cfield*),
1))))),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));}{struct
_tuple10 _tmp613;return(_tmp613.f1=_tmp378,((_tmp613.f2=_tmp377,_tmp613)));};}
_LL209:;_LL20A: {struct Cyc_String_pa_struct _tmp61B;void*_tmp61A[1];const char*
_tmp619;void*_tmp618;(_tmp618=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp61B.tag=0,((
_tmp61B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp61A[0]=& _tmp61B,Cyc_aprintf(((_tmp619="gen_lhs: %s",_tag_dyneither(_tmp619,
sizeof(char),12))),_tag_dyneither(_tmp61A,sizeof(void*),1)))))))),_tag_dyneither(
_tmp618,sizeof(void*),0)));}_LL1FC:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){void*
_tmp381=e->r;union Cyc_Absyn_Cnst _tmp383;struct _tuple3 _tmp384;union Cyc_Absyn_Cnst
_tmp386;struct _dyneither_ptr _tmp387;union Cyc_Absyn_Cnst _tmp389;struct _tuple4
_tmp38A;union Cyc_Absyn_Cnst _tmp38C;struct _tuple6 _tmp38D;union Cyc_Absyn_Cnst
_tmp394;struct _tuple7 _tmp395;union Cyc_Absyn_Cnst _tmp397;struct _tuple5 _tmp398;int
_tmp399;union Cyc_Absyn_Cnst _tmp39B;struct _tuple5 _tmp39C;union Cyc_Absyn_Cnst
_tmp39E;struct _dyneither_ptr _tmp39F;union Cyc_Absyn_Cnst _tmp3A1;struct
_dyneither_ptr _tmp3A2;union Cyc_Absyn_Cnst _tmp3A4;int _tmp3A5;struct _tuple0*
_tmp3A7;struct _tuple0*_tmp3A9;enum Cyc_Absyn_Primop _tmp3AB;struct Cyc_List_List*
_tmp3AC;struct Cyc_Absyn_Exp*_tmp3AE;struct Cyc_Core_Opt*_tmp3AF;struct Cyc_Absyn_Exp*
_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B2;enum Cyc_Absyn_Incrementor _tmp3B3;struct Cyc_Absyn_Exp*
_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B7;struct Cyc_Absyn_Exp*
_tmp3B9;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Exp*
_tmp3BD;struct Cyc_Absyn_Exp*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C0;struct Cyc_Absyn_Exp*
_tmp3C2;struct Cyc_List_List*_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C5;struct Cyc_List_List*
_tmp3C6;struct Cyc_Absyn_Exp*_tmp3C9;void*_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CD;
struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D1;struct Cyc_Absyn_Exp*
_tmp3D2;struct Cyc_Absyn_Exp*_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D6;struct
_dyneither_ptr*_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D9;struct _dyneither_ptr*_tmp3DA;
struct Cyc_Absyn_MallocInfo _tmp3DC;void**_tmp3DD;struct Cyc_Absyn_Exp*_tmp3DE;
struct Cyc_Absyn_Exp*_tmp3E0;struct Cyc_Absyn_Exp*_tmp3E1;struct Cyc_Absyn_Stmt*
_tmp3EC;_LL20C: {struct Cyc_Absyn_Const_e_struct*_tmp382=(struct Cyc_Absyn_Const_e_struct*)
_tmp381;if(_tmp382->tag != 0)goto _LL20E;else{_tmp383=_tmp382->f1;if((_tmp383.Char_c).tag
!= 2)goto _LL20E;_tmp384=(struct _tuple3)(_tmp383.Char_c).val;}}_LL20D: goto _LL20F;
_LL20E: {struct Cyc_Absyn_Const_e_struct*_tmp385=(struct Cyc_Absyn_Const_e_struct*)
_tmp381;if(_tmp385->tag != 0)goto _LL210;else{_tmp386=_tmp385->f1;if((_tmp386.Wchar_c).tag
!= 3)goto _LL210;_tmp387=(struct _dyneither_ptr)(_tmp386.Wchar_c).val;}}_LL20F:
goto _LL211;_LL210: {struct Cyc_Absyn_Const_e_struct*_tmp388=(struct Cyc_Absyn_Const_e_struct*)
_tmp381;if(_tmp388->tag != 0)goto _LL212;else{_tmp389=_tmp388->f1;if((_tmp389.Short_c).tag
!= 4)goto _LL212;_tmp38A=(struct _tuple4)(_tmp389.Short_c).val;}}_LL211: goto _LL213;
_LL212: {struct Cyc_Absyn_Const_e_struct*_tmp38B=(struct Cyc_Absyn_Const_e_struct*)
_tmp381;if(_tmp38B->tag != 0)goto _LL214;else{_tmp38C=_tmp38B->f1;if((_tmp38C.LongLong_c).tag
!= 6)goto _LL214;_tmp38D=(struct _tuple6)(_tmp38C.LongLong_c).val;}}_LL213: goto
_LL215;_LL214: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp38E=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp381;if(_tmp38E->tag != 18)goto _LL216;}_LL215: goto _LL217;_LL216: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp38F=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp381;if(_tmp38F->tag != 19)goto
_LL218;}_LL217: goto _LL219;_LL218: {struct Cyc_Absyn_Offsetof_e_struct*_tmp390=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp381;if(_tmp390->tag != 20)goto _LL21A;}
_LL219: goto _LL21B;_LL21A: {struct Cyc_Absyn_Enum_e_struct*_tmp391=(struct Cyc_Absyn_Enum_e_struct*)
_tmp381;if(_tmp391->tag != 32)goto _LL21C;}_LL21B: goto _LL21D;_LL21C: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp392=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp381;if(_tmp392->tag != 33)goto
_LL21E;}_LL21D: goto _LL21F;_LL21E: {struct Cyc_Absyn_Const_e_struct*_tmp393=(
struct Cyc_Absyn_Const_e_struct*)_tmp381;if(_tmp393->tag != 0)goto _LL220;else{
_tmp394=_tmp393->f1;if((_tmp394.Float_c).tag != 7)goto _LL220;_tmp395=(struct
_tuple7)(_tmp394.Float_c).val;}}_LL21F: return Cyc_Port_arith_ct();_LL220: {struct
Cyc_Absyn_Const_e_struct*_tmp396=(struct Cyc_Absyn_Const_e_struct*)_tmp381;if(
_tmp396->tag != 0)goto _LL222;else{_tmp397=_tmp396->f1;if((_tmp397.Int_c).tag != 5)
goto _LL222;_tmp398=(struct _tuple5)(_tmp397.Int_c).val;_tmp399=_tmp398.f2;if(
_tmp399 != 0)goto _LL222;}}_LL221: return Cyc_Port_zero_ct();_LL222: {struct Cyc_Absyn_Const_e_struct*
_tmp39A=(struct Cyc_Absyn_Const_e_struct*)_tmp381;if(_tmp39A->tag != 0)goto _LL224;
else{_tmp39B=_tmp39A->f1;if((_tmp39B.Int_c).tag != 5)goto _LL224;_tmp39C=(struct
_tuple5)(_tmp39B.Int_c).val;}}_LL223: return Cyc_Port_arith_ct();_LL224: {struct
Cyc_Absyn_Const_e_struct*_tmp39D=(struct Cyc_Absyn_Const_e_struct*)_tmp381;if(
_tmp39D->tag != 0)goto _LL226;else{_tmp39E=_tmp39D->f1;if((_tmp39E.String_c).tag != 
8)goto _LL226;_tmp39F=(struct _dyneither_ptr)(_tmp39E.String_c).val;}}_LL225:
return Cyc_Port_ptr_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_fat_ct(),
Cyc_Port_heap_ct(),Cyc_Port_zterm_ct());_LL226: {struct Cyc_Absyn_Const_e_struct*
_tmp3A0=(struct Cyc_Absyn_Const_e_struct*)_tmp381;if(_tmp3A0->tag != 0)goto _LL228;
else{_tmp3A1=_tmp3A0->f1;if((_tmp3A1.Wstring_c).tag != 9)goto _LL228;_tmp3A2=(
struct _dyneither_ptr)(_tmp3A1.Wstring_c).val;}}_LL227: return Cyc_Port_ptr_ct(Cyc_Port_arith_ct(),
Cyc_Port_const_ct(),Cyc_Port_fat_ct(),Cyc_Port_heap_ct(),Cyc_Port_zterm_ct());
_LL228: {struct Cyc_Absyn_Const_e_struct*_tmp3A3=(struct Cyc_Absyn_Const_e_struct*)
_tmp381;if(_tmp3A3->tag != 0)goto _LL22A;else{_tmp3A4=_tmp3A3->f1;if((_tmp3A4.Null_c).tag
!= 1)goto _LL22A;_tmp3A5=(int)(_tmp3A4.Null_c).val;}}_LL229: return Cyc_Port_zero_ct();
_LL22A: {struct Cyc_Absyn_Var_e_struct*_tmp3A6=(struct Cyc_Absyn_Var_e_struct*)
_tmp381;if(_tmp3A6->tag != 1)goto _LL22C;else{_tmp3A7=_tmp3A6->f1;}}_LL22B: _tmp3A9=
_tmp3A7;goto _LL22D;_LL22C: {struct Cyc_Absyn_UnknownId_e_struct*_tmp3A8=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp381;if(_tmp3A8->tag != 2)goto _LL22E;else{
_tmp3A9=_tmp3A8->f1;}}_LL22D:(*_tmp3A9).f1=Cyc_Absyn_Loc_n;{void*_tmp3F1;struct
_tuple10 _tmp3F0=Cyc_Port_lookup_var(env,_tmp3A9);_tmp3F1=_tmp3F0.f2;if(Cyc_Port_isfn(
env,_tmp3A9)){_tmp3F1=Cyc_Port_instantiate(_tmp3F1);return Cyc_Port_ptr_ct(
_tmp3F1,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_heap_ct(),Cyc_Port_nozterm_ct());}
else{if(Cyc_Port_isarray(env,_tmp3A9)){void*_tmp3F2=Cyc_Port_var();void*_tmp3F3=
Cyc_Port_var();void*_tmp3F4=Cyc_Port_var();void*_tmp3F5=Cyc_Port_array_ct(
_tmp3F2,_tmp3F3,_tmp3F4);Cyc_Port_unifies(_tmp3F1,_tmp3F5);{void*_tmp3F6=Cyc_Port_ptr_ct(
_tmp3F2,_tmp3F3,Cyc_Port_fat_ct(),Cyc_Port_var(),_tmp3F4);return _tmp3F6;};}else{
return _tmp3F1;}}};_LL22E: {struct Cyc_Absyn_Primop_e_struct*_tmp3AA=(struct Cyc_Absyn_Primop_e_struct*)
_tmp381;if(_tmp3AA->tag != 3)goto _LL230;else{_tmp3AB=_tmp3AA->f1;_tmp3AC=_tmp3AA->f2;}}
_LL22F: return Cyc_Port_gen_primop(env,_tmp3AB,((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp3AC));_LL230: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp3AD=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp381;if(_tmp3AD->tag != 4)goto
_LL232;else{_tmp3AE=_tmp3AD->f1;_tmp3AF=_tmp3AD->f2;_tmp3B0=_tmp3AD->f3;}}_LL231: {
void*_tmp3F8;void*_tmp3F9;struct _tuple10 _tmp3F7=Cyc_Port_gen_lhs(env,_tmp3AE);
_tmp3F8=_tmp3F7.f1;_tmp3F9=_tmp3F7.f2;Cyc_Port_unifies(_tmp3F8,Cyc_Port_notconst_ct());{
void*_tmp3FA=Cyc_Port_gen_exp(env,_tmp3B0);if((unsigned int)_tmp3AF){struct Cyc_List_List
_tmp61C;struct Cyc_List_List x2=(_tmp61C.hd=_tmp3FA,((_tmp61C.tl=0,_tmp61C)));
struct Cyc_List_List _tmp61D;struct Cyc_List_List x1=(_tmp61D.hd=_tmp3F9,((_tmp61D.tl=(
struct Cyc_List_List*)& x2,_tmp61D)));void*_tmp3FB=Cyc_Port_gen_primop(env,(enum 
Cyc_Absyn_Primop)_tmp3AF->v,(struct Cyc_List_List*)& x1);Cyc_Port_leq(_tmp3FB,
_tmp3F9);return _tmp3F9;}else{Cyc_Port_leq(_tmp3FA,_tmp3F9);return _tmp3F9;}};}
_LL232: {struct Cyc_Absyn_Increment_e_struct*_tmp3B1=(struct Cyc_Absyn_Increment_e_struct*)
_tmp381;if(_tmp3B1->tag != 5)goto _LL234;else{_tmp3B2=_tmp3B1->f1;_tmp3B3=_tmp3B1->f2;}}
_LL233: {void*_tmp3FF;void*_tmp400;struct _tuple10 _tmp3FE=Cyc_Port_gen_lhs(env,
_tmp3B2);_tmp3FF=_tmp3FE.f1;_tmp400=_tmp3FE.f2;Cyc_Port_unifies(_tmp3FF,Cyc_Port_notconst_ct());
Cyc_Port_leq(_tmp400,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()));Cyc_Port_leq(_tmp400,Cyc_Port_arith_ct());return
_tmp400;}_LL234: {struct Cyc_Absyn_Conditional_e_struct*_tmp3B4=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp381;if(_tmp3B4->tag != 6)goto _LL236;else{_tmp3B5=_tmp3B4->f1;_tmp3B6=_tmp3B4->f2;
_tmp3B7=_tmp3B4->f3;}}_LL235: {void*_tmp401=Cyc_Port_var();Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp3B5),Cyc_Port_arith_ct());Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3B6),
_tmp401);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3B7),_tmp401);return _tmp401;}
_LL236: {struct Cyc_Absyn_And_e_struct*_tmp3B8=(struct Cyc_Absyn_And_e_struct*)
_tmp381;if(_tmp3B8->tag != 7)goto _LL238;else{_tmp3B9=_tmp3B8->f1;_tmp3BA=_tmp3B8->f2;}}
_LL237: _tmp3BC=_tmp3B9;_tmp3BD=_tmp3BA;goto _LL239;_LL238: {struct Cyc_Absyn_Or_e_struct*
_tmp3BB=(struct Cyc_Absyn_Or_e_struct*)_tmp381;if(_tmp3BB->tag != 8)goto _LL23A;
else{_tmp3BC=_tmp3BB->f1;_tmp3BD=_tmp3BB->f2;}}_LL239: Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp3BC),Cyc_Port_arith_ct());Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3BD),Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();_LL23A: {struct Cyc_Absyn_SeqExp_e_struct*_tmp3BE=(
struct Cyc_Absyn_SeqExp_e_struct*)_tmp381;if(_tmp3BE->tag != 9)goto _LL23C;else{
_tmp3BF=_tmp3BE->f1;_tmp3C0=_tmp3BE->f2;}}_LL23B: Cyc_Port_gen_exp(env,_tmp3BF);
return Cyc_Port_gen_exp(env,_tmp3C0);_LL23C: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp3C1=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp381;if(_tmp3C1->tag != 10)goto
_LL23E;else{_tmp3C2=_tmp3C1->f1;_tmp3C3=_tmp3C1->f2;}}_LL23D: _tmp3C5=_tmp3C2;
_tmp3C6=_tmp3C3;goto _LL23F;_LL23E: {struct Cyc_Absyn_FnCall_e_struct*_tmp3C4=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp381;if(_tmp3C4->tag != 11)goto _LL240;else{
_tmp3C5=_tmp3C4->f1;_tmp3C6=_tmp3C4->f2;}}_LL23F: {void*_tmp402=Cyc_Port_var();
void*_tmp403=Cyc_Port_gen_exp(env,_tmp3C5);struct Cyc_List_List*_tmp404=((struct
Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp3C6);struct
Cyc_List_List*_tmp405=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)((void*(*)(void*x))Cyc_Port_new_var,_tmp404);Cyc_Port_unifies(
_tmp403,Cyc_Port_ptr_ct(Cyc_Port_fn_ct(_tmp402,_tmp405),Cyc_Port_var(),Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_nozterm_ct()));for(0;_tmp404 != 0;(_tmp404=_tmp404->tl,
_tmp405=_tmp405->tl)){Cyc_Port_leq((void*)_tmp404->hd,(void*)((struct Cyc_List_List*)
_check_null(_tmp405))->hd);}return _tmp402;}_LL240: {struct Cyc_Absyn_Throw_e_struct*
_tmp3C7=(struct Cyc_Absyn_Throw_e_struct*)_tmp381;if(_tmp3C7->tag != 12)goto _LL242;}
_LL241: {const char*_tmp620;void*_tmp61F;(_tmp61F=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp620="throw",_tag_dyneither(
_tmp620,sizeof(char),6))),_tag_dyneither(_tmp61F,sizeof(void*),0)));}_LL242: {
struct Cyc_Absyn_NoInstantiate_e_struct*_tmp3C8=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp381;if(_tmp3C8->tag != 13)goto _LL244;else{_tmp3C9=_tmp3C8->f1;}}_LL243: return
Cyc_Port_gen_exp(env,_tmp3C9);_LL244: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp3CA=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp381;if(_tmp3CA->tag != 14)goto
_LL246;}_LL245: {const char*_tmp623;void*_tmp622;(_tmp622=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp623="instantiate",
_tag_dyneither(_tmp623,sizeof(char),12))),_tag_dyneither(_tmp622,sizeof(void*),0)));}
_LL246: {struct Cyc_Absyn_Cast_e_struct*_tmp3CB=(struct Cyc_Absyn_Cast_e_struct*)
_tmp381;if(_tmp3CB->tag != 15)goto _LL248;else{_tmp3CC=(void*)_tmp3CB->f1;_tmp3CD=
_tmp3CB->f2;}}_LL247: {void*_tmp40A=Cyc_Port_type_to_ctype(env,_tmp3CC);void*
_tmp40B=Cyc_Port_gen_exp(env,_tmp3CD);Cyc_Port_leq(_tmp40A,_tmp40B);return
_tmp40B;}_LL248: {struct Cyc_Absyn_Address_e_struct*_tmp3CE=(struct Cyc_Absyn_Address_e_struct*)
_tmp381;if(_tmp3CE->tag != 16)goto _LL24A;else{_tmp3CF=_tmp3CE->f1;}}_LL249: {void*
_tmp40D;void*_tmp40E;struct _tuple10 _tmp40C=Cyc_Port_gen_lhs(env,_tmp3CF);_tmp40D=
_tmp40C.f1;_tmp40E=_tmp40C.f2;return Cyc_Port_ptr_ct(_tmp40E,_tmp40D,Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_var());}_LL24A: {struct Cyc_Absyn_Subscript_e_struct*
_tmp3D0=(struct Cyc_Absyn_Subscript_e_struct*)_tmp381;if(_tmp3D0->tag != 24)goto
_LL24C;else{_tmp3D1=_tmp3D0->f1;_tmp3D2=_tmp3D0->f2;}}_LL24B: {void*_tmp40F=Cyc_Port_var();
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3D2),Cyc_Port_arith_ct());{void*_tmp410=
Cyc_Port_gen_exp(env,_tmp3D1);Cyc_Port_leq(_tmp410,Cyc_Port_ptr_ct(_tmp40F,Cyc_Port_var(),
Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()));return _tmp40F;};}_LL24C: {
struct Cyc_Absyn_Deref_e_struct*_tmp3D3=(struct Cyc_Absyn_Deref_e_struct*)_tmp381;
if(_tmp3D3->tag != 21)goto _LL24E;else{_tmp3D4=_tmp3D3->f1;}}_LL24D: {void*_tmp411=
Cyc_Port_var();Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3D4),Cyc_Port_ptr_ct(
_tmp411,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));return
_tmp411;}_LL24E: {struct Cyc_Absyn_AggrMember_e_struct*_tmp3D5=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp381;if(_tmp3D5->tag != 22)goto _LL250;else{_tmp3D6=_tmp3D5->f1;_tmp3D7=_tmp3D5->f2;}}
_LL24F: {void*_tmp412=Cyc_Port_var();void*_tmp413=Cyc_Port_gen_exp(env,_tmp3D6);{
struct Cyc_Port_Cfield*_tmp626;struct Cyc_Port_Cfield*_tmp625[1];Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp3D6),Cyc_Port_unknown_aggr_ct(((_tmp625[0]=((_tmp626=_cycalloc(sizeof(*
_tmp626)),((_tmp626->qual=Cyc_Port_var(),((_tmp626->f=_tmp3D7,((_tmp626->type=
_tmp412,_tmp626)))))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp625,sizeof(struct Cyc_Port_Cfield*),1))))));}return _tmp412;}
_LL250: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp3D8=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp381;if(_tmp3D8->tag != 23)goto _LL252;else{_tmp3D9=_tmp3D8->f1;_tmp3DA=_tmp3D8->f2;}}
_LL251: {void*_tmp416=Cyc_Port_var();void*_tmp417=Cyc_Port_gen_exp(env,_tmp3D9);{
struct Cyc_Port_Cfield*_tmp629;struct Cyc_Port_Cfield*_tmp628[1];Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp3D9),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(((_tmp628[0]=((_tmp629=
_cycalloc(sizeof(*_tmp629)),((_tmp629->qual=Cyc_Port_var(),((_tmp629->f=_tmp3DA,((
_tmp629->type=_tmp416,_tmp629)))))))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp628,sizeof(struct Cyc_Port_Cfield*),
1))))),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));}return
_tmp416;}_LL252: {struct Cyc_Absyn_Malloc_e_struct*_tmp3DB=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp381;if(_tmp3DB->tag != 34)goto _LL254;else{_tmp3DC=_tmp3DB->f1;_tmp3DD=_tmp3DC.elt_type;
_tmp3DE=_tmp3DC.num_elts;}}_LL253: Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3DE),Cyc_Port_arith_ct());{
void*_tmp41A=(unsigned int)_tmp3DD?Cyc_Port_type_to_ctype(env,*_tmp3DD): Cyc_Port_var();
return Cyc_Port_ptr_ct(_tmp41A,Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_heap_ct(),
Cyc_Port_var());};_LL254: {struct Cyc_Absyn_Swap_e_struct*_tmp3DF=(struct Cyc_Absyn_Swap_e_struct*)
_tmp381;if(_tmp3DF->tag != 35)goto _LL256;else{_tmp3E0=_tmp3DF->f1;_tmp3E1=_tmp3DF->f2;}}
_LL255: {const char*_tmp62C;void*_tmp62B;(_tmp62B=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp62C="Swap_e",_tag_dyneither(
_tmp62C,sizeof(char),7))),_tag_dyneither(_tmp62B,sizeof(void*),0)));}_LL256: {
struct Cyc_Absyn_New_e_struct*_tmp3E2=(struct Cyc_Absyn_New_e_struct*)_tmp381;if(
_tmp3E2->tag != 17)goto _LL258;}_LL257: {const char*_tmp62F;void*_tmp62E;(_tmp62E=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp62F="New_e",_tag_dyneither(_tmp62F,sizeof(char),6))),_tag_dyneither(_tmp62E,
sizeof(void*),0)));}_LL258: {struct Cyc_Absyn_Tuple_e_struct*_tmp3E3=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp381;if(_tmp3E3->tag != 25)goto _LL25A;}_LL259: {const char*_tmp632;void*_tmp631;(
_tmp631=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp632="Tuple_e",_tag_dyneither(_tmp632,sizeof(char),8))),_tag_dyneither(
_tmp631,sizeof(void*),0)));}_LL25A: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp3E4=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp381;if(_tmp3E4->tag != 26)goto
_LL25C;}_LL25B: {const char*_tmp635;void*_tmp634;(_tmp634=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp635="CompoundLit_e",
_tag_dyneither(_tmp635,sizeof(char),14))),_tag_dyneither(_tmp634,sizeof(void*),0)));}
_LL25C: {struct Cyc_Absyn_Array_e_struct*_tmp3E5=(struct Cyc_Absyn_Array_e_struct*)
_tmp381;if(_tmp3E5->tag != 27)goto _LL25E;}_LL25D: {const char*_tmp638;void*_tmp637;(
_tmp637=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp638="Array_e",_tag_dyneither(_tmp638,sizeof(char),8))),_tag_dyneither(
_tmp637,sizeof(void*),0)));}_LL25E: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp3E6=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp381;if(_tmp3E6->tag != 28)
goto _LL260;}_LL25F: {const char*_tmp63B;void*_tmp63A;(_tmp63A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp63B="Comprehension_e",
_tag_dyneither(_tmp63B,sizeof(char),16))),_tag_dyneither(_tmp63A,sizeof(void*),0)));}
_LL260: {struct Cyc_Absyn_Aggregate_e_struct*_tmp3E7=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp381;if(_tmp3E7->tag != 29)goto _LL262;}_LL261: {const char*_tmp63E;void*_tmp63D;(
_tmp63D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp63E="Aggregate_e",_tag_dyneither(_tmp63E,sizeof(char),12))),_tag_dyneither(
_tmp63D,sizeof(void*),0)));}_LL262: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp3E8=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp381;if(_tmp3E8->tag != 30)goto _LL264;}
_LL263: {const char*_tmp641;void*_tmp640;(_tmp640=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp641="AnonStruct_e",
_tag_dyneither(_tmp641,sizeof(char),13))),_tag_dyneither(_tmp640,sizeof(void*),0)));}
_LL264: {struct Cyc_Absyn_Datatype_e_struct*_tmp3E9=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp381;if(_tmp3E9->tag != 31)goto _LL266;}_LL265: {const char*_tmp644;void*_tmp643;(
_tmp643=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp644="Datatype_e",_tag_dyneither(_tmp644,sizeof(char),11))),_tag_dyneither(
_tmp643,sizeof(void*),0)));}_LL266: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp3EA=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp381;if(_tmp3EA->tag != 36)
goto _LL268;}_LL267: {const char*_tmp647;void*_tmp646;(_tmp646=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp647="UnresolvedMem_e",
_tag_dyneither(_tmp647,sizeof(char),16))),_tag_dyneither(_tmp646,sizeof(void*),0)));}
_LL268: {struct Cyc_Absyn_StmtExp_e_struct*_tmp3EB=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp381;if(_tmp3EB->tag != 37)goto _LL26A;else{_tmp3EC=_tmp3EB->f1;}}_LL269: {
const char*_tmp64A;void*_tmp649;(_tmp649=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp64A="StmtExp_e",_tag_dyneither(
_tmp64A,sizeof(char),10))),_tag_dyneither(_tmp649,sizeof(void*),0)));}_LL26A: {
struct Cyc_Absyn_Valueof_e_struct*_tmp3ED=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp381;if(_tmp3ED->tag != 39)goto _LL26C;}_LL26B: {const char*_tmp64D;void*_tmp64C;(
_tmp64C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp64D="Valueof_e",_tag_dyneither(_tmp64D,sizeof(char),10))),_tag_dyneither(
_tmp64C,sizeof(void*),0)));}_LL26C: {struct Cyc_Absyn_Asm_e_struct*_tmp3EE=(
struct Cyc_Absyn_Asm_e_struct*)_tmp381;if(_tmp3EE->tag != 40)goto _LL26E;}_LL26D: {
const char*_tmp650;void*_tmp64F;(_tmp64F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp650="Asm_e",_tag_dyneither(
_tmp650,sizeof(char),6))),_tag_dyneither(_tmp64F,sizeof(void*),0)));}_LL26E: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp3EF=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp381;if(_tmp3EF->tag != 38)goto _LL20B;}_LL26F: {const char*_tmp653;void*_tmp652;(
_tmp652=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp653="Tagcheck_e",_tag_dyneither(_tmp653,sizeof(char),11))),_tag_dyneither(
_tmp652,sizeof(void*),0)));}_LL20B:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s){void*
_tmp437=s->r;struct Cyc_Absyn_Exp*_tmp43A;struct Cyc_Absyn_Stmt*_tmp43C;struct Cyc_Absyn_Stmt*
_tmp43D;struct Cyc_Absyn_Exp*_tmp43F;struct Cyc_Absyn_Exp*_tmp441;struct Cyc_Absyn_Stmt*
_tmp442;struct Cyc_Absyn_Stmt*_tmp443;struct _tuple9 _tmp445;struct Cyc_Absyn_Exp*
_tmp446;struct Cyc_Absyn_Stmt*_tmp447;struct Cyc_Absyn_Exp*_tmp44C;struct _tuple9
_tmp44D;struct Cyc_Absyn_Exp*_tmp44E;struct _tuple9 _tmp44F;struct Cyc_Absyn_Exp*
_tmp450;struct Cyc_Absyn_Stmt*_tmp451;struct Cyc_Absyn_Exp*_tmp453;struct Cyc_List_List*
_tmp454;struct Cyc_Absyn_Decl*_tmp457;struct Cyc_Absyn_Stmt*_tmp458;struct Cyc_Absyn_Stmt*
_tmp45A;struct Cyc_Absyn_Stmt*_tmp45C;struct _tuple9 _tmp45D;struct Cyc_Absyn_Exp*
_tmp45E;_LL271: {struct Cyc_Absyn_Skip_s_struct*_tmp438=(struct Cyc_Absyn_Skip_s_struct*)
_tmp437;if(_tmp438->tag != 0)goto _LL273;}_LL272: return;_LL273: {struct Cyc_Absyn_Exp_s_struct*
_tmp439=(struct Cyc_Absyn_Exp_s_struct*)_tmp437;if(_tmp439->tag != 1)goto _LL275;
else{_tmp43A=_tmp439->f1;}}_LL274: Cyc_Port_gen_exp(env,_tmp43A);return;_LL275: {
struct Cyc_Absyn_Seq_s_struct*_tmp43B=(struct Cyc_Absyn_Seq_s_struct*)_tmp437;if(
_tmp43B->tag != 2)goto _LL277;else{_tmp43C=_tmp43B->f1;_tmp43D=_tmp43B->f2;}}
_LL276: Cyc_Port_gen_stmt(env,_tmp43C);Cyc_Port_gen_stmt(env,_tmp43D);return;
_LL277: {struct Cyc_Absyn_Return_s_struct*_tmp43E=(struct Cyc_Absyn_Return_s_struct*)
_tmp437;if(_tmp43E->tag != 3)goto _LL279;else{_tmp43F=_tmp43E->f1;}}_LL278: {void*
_tmp461=Cyc_Port_lookup_return_type(env);void*_tmp462=(unsigned int)_tmp43F?Cyc_Port_gen_exp(
env,(struct Cyc_Absyn_Exp*)_tmp43F): Cyc_Port_void_ct();Cyc_Port_leq(_tmp462,
_tmp461);return;}_LL279: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp440=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp437;if(_tmp440->tag != 4)goto _LL27B;else{_tmp441=_tmp440->f1;_tmp442=_tmp440->f2;
_tmp443=_tmp440->f3;}}_LL27A: Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp441),Cyc_Port_arith_ct());
Cyc_Port_gen_stmt(env,_tmp442);Cyc_Port_gen_stmt(env,_tmp443);return;_LL27B: {
struct Cyc_Absyn_While_s_struct*_tmp444=(struct Cyc_Absyn_While_s_struct*)_tmp437;
if(_tmp444->tag != 5)goto _LL27D;else{_tmp445=_tmp444->f1;_tmp446=_tmp445.f1;
_tmp447=_tmp444->f2;}}_LL27C: Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp446),Cyc_Port_arith_ct());
Cyc_Port_gen_stmt(env,_tmp447);return;_LL27D: {struct Cyc_Absyn_Break_s_struct*
_tmp448=(struct Cyc_Absyn_Break_s_struct*)_tmp437;if(_tmp448->tag != 6)goto _LL27F;}
_LL27E: goto _LL280;_LL27F: {struct Cyc_Absyn_Continue_s_struct*_tmp449=(struct Cyc_Absyn_Continue_s_struct*)
_tmp437;if(_tmp449->tag != 7)goto _LL281;}_LL280: goto _LL282;_LL281: {struct Cyc_Absyn_Goto_s_struct*
_tmp44A=(struct Cyc_Absyn_Goto_s_struct*)_tmp437;if(_tmp44A->tag != 8)goto _LL283;}
_LL282: return;_LL283: {struct Cyc_Absyn_For_s_struct*_tmp44B=(struct Cyc_Absyn_For_s_struct*)
_tmp437;if(_tmp44B->tag != 9)goto _LL285;else{_tmp44C=_tmp44B->f1;_tmp44D=_tmp44B->f2;
_tmp44E=_tmp44D.f1;_tmp44F=_tmp44B->f3;_tmp450=_tmp44F.f1;_tmp451=_tmp44B->f4;}}
_LL284: Cyc_Port_gen_exp(env,_tmp44C);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp44E),
Cyc_Port_arith_ct());Cyc_Port_gen_exp(env,_tmp450);Cyc_Port_gen_stmt(env,_tmp451);
return;_LL285: {struct Cyc_Absyn_Switch_s_struct*_tmp452=(struct Cyc_Absyn_Switch_s_struct*)
_tmp437;if(_tmp452->tag != 10)goto _LL287;else{_tmp453=_tmp452->f1;_tmp454=_tmp452->f2;}}
_LL286: Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp453),Cyc_Port_arith_ct());for(0;(
unsigned int)_tmp454;_tmp454=_tmp454->tl){Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)
_tmp454->hd)->body);}return;_LL287: {struct Cyc_Absyn_Fallthru_s_struct*_tmp455=(
struct Cyc_Absyn_Fallthru_s_struct*)_tmp437;if(_tmp455->tag != 11)goto _LL289;}
_LL288: {const char*_tmp656;void*_tmp655;(_tmp655=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp656="fallthru",
_tag_dyneither(_tmp656,sizeof(char),9))),_tag_dyneither(_tmp655,sizeof(void*),0)));}
_LL289: {struct Cyc_Absyn_Decl_s_struct*_tmp456=(struct Cyc_Absyn_Decl_s_struct*)
_tmp437;if(_tmp456->tag != 12)goto _LL28B;else{_tmp457=_tmp456->f1;_tmp458=_tmp456->f2;}}
_LL28A: env=Cyc_Port_gen_localdecl(env,_tmp457);Cyc_Port_gen_stmt(env,_tmp458);
return;_LL28B: {struct Cyc_Absyn_Label_s_struct*_tmp459=(struct Cyc_Absyn_Label_s_struct*)
_tmp437;if(_tmp459->tag != 13)goto _LL28D;else{_tmp45A=_tmp459->f2;}}_LL28C: Cyc_Port_gen_stmt(
env,_tmp45A);return;_LL28D: {struct Cyc_Absyn_Do_s_struct*_tmp45B=(struct Cyc_Absyn_Do_s_struct*)
_tmp437;if(_tmp45B->tag != 14)goto _LL28F;else{_tmp45C=_tmp45B->f1;_tmp45D=_tmp45B->f2;
_tmp45E=_tmp45D.f1;}}_LL28E: Cyc_Port_gen_stmt(env,_tmp45C);Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp45E),Cyc_Port_arith_ct());return;_LL28F: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp45F=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp437;if(_tmp45F->tag != 15)goto
_LL291;}_LL290: {const char*_tmp659;void*_tmp658;(_tmp658=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp659="try/catch",
_tag_dyneither(_tmp659,sizeof(char),10))),_tag_dyneither(_tmp658,sizeof(void*),0)));}
_LL291: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp460=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp437;if(_tmp460->tag != 16)goto _LL270;}_LL292: {const char*_tmp65C;void*_tmp65B;(
_tmp65B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp65C="reset region",_tag_dyneither(_tmp65C,sizeof(char),13))),_tag_dyneither(
_tmp65B,sizeof(void*),0)));}_LL270:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Port_gen_initializer(
struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){void*_tmp469=e->r;struct
Cyc_List_List*_tmp46B;struct Cyc_List_List*_tmp46D;struct Cyc_List_List*_tmp46F;
union Cyc_Absyn_Cnst _tmp471;struct _dyneither_ptr _tmp472;union Cyc_Absyn_Cnst
_tmp474;struct _dyneither_ptr _tmp475;_LL294: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp46A=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp469;if(_tmp46A->tag != 36)
goto _LL296;else{_tmp46B=_tmp46A->f2;}}_LL295: _tmp46D=_tmp46B;goto _LL297;_LL296: {
struct Cyc_Absyn_Array_e_struct*_tmp46C=(struct Cyc_Absyn_Array_e_struct*)_tmp469;
if(_tmp46C->tag != 27)goto _LL298;else{_tmp46D=_tmp46C->f1;}}_LL297: _tmp46F=
_tmp46D;goto _LL299;_LL298: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp46E=(struct
Cyc_Absyn_CompoundLit_e_struct*)_tmp469;if(_tmp46E->tag != 26)goto _LL29A;else{
_tmp46F=_tmp46E->f2;}}_LL299: LOOP: {void*_tmp476=t;struct _tuple0*_tmp478;struct
Cyc_Absyn_ArrayInfo _tmp47A;void*_tmp47B;union Cyc_Absyn_Constraint*_tmp47C;struct
Cyc_Position_Segment*_tmp47D;struct Cyc_Absyn_AggrInfo _tmp47F;union Cyc_Absyn_AggrInfoU
_tmp480;struct _tuple2 _tmp481;enum Cyc_Absyn_AggrKind _tmp482;struct _tuple0*
_tmp483;struct Cyc_Absyn_AggrInfo _tmp485;union Cyc_Absyn_AggrInfoU _tmp486;struct
Cyc_Absyn_Aggrdecl**_tmp487;struct Cyc_Absyn_Aggrdecl*_tmp488;_LL2A1: {struct Cyc_Absyn_TypedefType_struct*
_tmp477=(struct Cyc_Absyn_TypedefType_struct*)_tmp476;if(_tmp477->tag != 17)goto
_LL2A3;else{_tmp478=_tmp477->f1;}}_LL2A2:(*_tmp478).f1=Cyc_Absyn_Loc_n;t=Cyc_Port_lookup_typedef(
env,_tmp478);goto LOOP;_LL2A3: {struct Cyc_Absyn_ArrayType_struct*_tmp479=(struct
Cyc_Absyn_ArrayType_struct*)_tmp476;if(_tmp479->tag != 8)goto _LL2A5;else{_tmp47A=
_tmp479->f1;_tmp47B=_tmp47A.elt_type;_tmp47C=_tmp47A.zero_term;_tmp47D=_tmp47A.zt_loc;}}
_LL2A4: {void*_tmp489=Cyc_Port_var();void*_tmp48A=Cyc_Port_var();void*_tmp48B=
Cyc_Port_var();void*_tmp48C=Cyc_Port_type_to_ctype(env,_tmp47B);for(0;_tmp46F != 
0;_tmp46F=_tmp46F->tl){struct Cyc_List_List*_tmp48E;struct Cyc_Absyn_Exp*_tmp48F;
struct _tuple15 _tmp48D=*((struct _tuple15*)_tmp46F->hd);_tmp48E=_tmp48D.f1;_tmp48F=
_tmp48D.f2;if((unsigned int)_tmp48E){const char*_tmp65F;void*_tmp65E;(_tmp65E=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp65F="designators in initializers",_tag_dyneither(_tmp65F,sizeof(char),28))),
_tag_dyneither(_tmp65E,sizeof(void*),0)));}{void*_tmp492=Cyc_Port_gen_initializer(
env,_tmp47B,_tmp48F);Cyc_Port_leq(_tmp492,_tmp489);};}return Cyc_Port_array_ct(
_tmp489,_tmp48A,_tmp48B);}_LL2A5: {struct Cyc_Absyn_AggrType_struct*_tmp47E=(
struct Cyc_Absyn_AggrType_struct*)_tmp476;if(_tmp47E->tag != 11)goto _LL2A7;else{
_tmp47F=_tmp47E->f1;_tmp480=_tmp47F.aggr_info;if((_tmp480.UnknownAggr).tag != 1)
goto _LL2A7;_tmp481=(struct _tuple2)(_tmp480.UnknownAggr).val;_tmp482=_tmp481.f1;
if(_tmp482 != Cyc_Absyn_StructA)goto _LL2A7;_tmp483=_tmp481.f2;}}_LL2A6:(*_tmp483).f1=
Cyc_Absyn_Loc_n;{struct Cyc_Absyn_Aggrdecl*_tmp494;struct _tuple11 _tmp493=*Cyc_Port_lookup_struct_decl(
env,_tmp483);_tmp494=_tmp493.f1;if((struct Cyc_Absyn_Aggrdecl*)_tmp494 == 0){const
char*_tmp662;void*_tmp661;(_tmp661=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp662="struct is not yet defined",
_tag_dyneither(_tmp662,sizeof(char),26))),_tag_dyneither(_tmp661,sizeof(void*),0)));}
_tmp488=_tmp494;goto _LL2A8;};_LL2A7: {struct Cyc_Absyn_AggrType_struct*_tmp484=(
struct Cyc_Absyn_AggrType_struct*)_tmp476;if(_tmp484->tag != 11)goto _LL2A9;else{
_tmp485=_tmp484->f1;_tmp486=_tmp485.aggr_info;if((_tmp486.KnownAggr).tag != 2)
goto _LL2A9;_tmp487=(struct Cyc_Absyn_Aggrdecl**)(_tmp486.KnownAggr).val;_tmp488=*
_tmp487;}}_LL2A8: {struct Cyc_List_List*_tmp497=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp488->impl))->fields;for(0;_tmp46F != 0;_tmp46F=_tmp46F->tl){
struct Cyc_List_List*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct _tuple15 _tmp498=*((
struct _tuple15*)_tmp46F->hd);_tmp499=_tmp498.f1;_tmp49A=_tmp498.f2;if((
unsigned int)_tmp499){const char*_tmp665;void*_tmp664;(_tmp664=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp665="designators in initializers",
_tag_dyneither(_tmp665,sizeof(char),28))),_tag_dyneither(_tmp664,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrfield*_tmp49D=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)
_check_null(_tmp497))->hd;_tmp497=_tmp497->tl;{void*_tmp49E=Cyc_Port_gen_initializer(
env,_tmp49D->type,_tmp49A);;};};}return Cyc_Port_type_to_ctype(env,t);}_LL2A9:;
_LL2AA: {const char*_tmp668;void*_tmp667;(_tmp667=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp668="bad type for aggregate initializer",
_tag_dyneither(_tmp668,sizeof(char),35))),_tag_dyneither(_tmp667,sizeof(void*),0)));}
_LL2A0:;}_LL29A: {struct Cyc_Absyn_Const_e_struct*_tmp470=(struct Cyc_Absyn_Const_e_struct*)
_tmp469;if(_tmp470->tag != 0)goto _LL29C;else{_tmp471=_tmp470->f1;if((_tmp471.String_c).tag
!= 8)goto _LL29C;_tmp472=(struct _dyneither_ptr)(_tmp471.String_c).val;}}_LL29B:
goto _LL29D;_LL29C: {struct Cyc_Absyn_Const_e_struct*_tmp473=(struct Cyc_Absyn_Const_e_struct*)
_tmp469;if(_tmp473->tag != 0)goto _LL29E;else{_tmp474=_tmp473->f1;if((_tmp474.Wstring_c).tag
!= 9)goto _LL29E;_tmp475=(struct _dyneither_ptr)(_tmp474.Wstring_c).val;}}_LL29D:
LOOP2: {void*_tmp4A1=t;struct _tuple0*_tmp4A3;_LL2AC: {struct Cyc_Absyn_TypedefType_struct*
_tmp4A2=(struct Cyc_Absyn_TypedefType_struct*)_tmp4A1;if(_tmp4A2->tag != 17)goto
_LL2AE;else{_tmp4A3=_tmp4A2->f1;}}_LL2AD:(*_tmp4A3).f1=Cyc_Absyn_Loc_n;t=Cyc_Port_lookup_typedef(
env,_tmp4A3);goto LOOP2;_LL2AE: {struct Cyc_Absyn_ArrayType_struct*_tmp4A4=(struct
Cyc_Absyn_ArrayType_struct*)_tmp4A1;if(_tmp4A4->tag != 8)goto _LL2B0;}_LL2AF:
return Cyc_Port_array_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_zterm_ct());
_LL2B0:;_LL2B1: return Cyc_Port_gen_exp(env,e);_LL2AB:;}_LL29E:;_LL29F: return Cyc_Port_gen_exp(
env,e);_LL293:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*
env,struct Cyc_Absyn_Decl*d){void*_tmp4A5=d->r;struct Cyc_Absyn_Vardecl*_tmp4A7;
_LL2B3: {struct Cyc_Absyn_Var_d_struct*_tmp4A6=(struct Cyc_Absyn_Var_d_struct*)
_tmp4A5;if(_tmp4A6->tag != 0)goto _LL2B5;else{_tmp4A7=_tmp4A6->f1;}}_LL2B4: {void*
_tmp4A8=Cyc_Port_var();void*q;if((env->gcenv)->porting){q=Cyc_Port_var();Cyc_Port_register_const_cvar(
env,q,(_tmp4A7->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(
_tmp4A7->tq).loc);}else{q=(_tmp4A7->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}(*
_tmp4A7->name).f1=Cyc_Absyn_Loc_n;env=Cyc_Port_add_var(env,_tmp4A7->name,_tmp4A7->type,
q,_tmp4A8);Cyc_Port_unifies(_tmp4A8,Cyc_Port_type_to_ctype(env,_tmp4A7->type));
if((unsigned int)_tmp4A7->initializer){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_check_null(_tmp4A7->initializer);void*t2=Cyc_Port_gen_initializer(env,_tmp4A7->type,
e);Cyc_Port_leq(t2,_tmp4A8);}return env;}_LL2B5:;_LL2B6: {const char*_tmp66B;void*
_tmp66A;(_tmp66A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp66B="non-local decl that isn't a variable",_tag_dyneither(_tmp66B,sizeof(
char),37))),_tag_dyneither(_tmp66A,sizeof(void*),0)));}_LL2B2:;}struct _tuple16{
struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple8*
Cyc_Port_make_targ(struct _tuple16*arg){struct _dyneither_ptr*_tmp4AC;struct Cyc_Absyn_Tqual
_tmp4AD;void*_tmp4AE;struct _tuple16 _tmp4AB=*arg;_tmp4AC=_tmp4AB.f1;_tmp4AD=
_tmp4AB.f2;_tmp4AE=_tmp4AB.f3;{struct _tuple8*_tmp66C;return(_tmp66C=_cycalloc(
sizeof(*_tmp66C)),((_tmp66C->f1=0,((_tmp66C->f2=_tmp4AD,((_tmp66C->f3=_tmp4AE,
_tmp66C)))))));};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*
Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds);struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple17{
struct _dyneither_ptr*f1;void*f2;void*f3;void*f4;};struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*
env,struct Cyc_Absyn_Decl*d){void*_tmp4B0=d->r;struct Cyc_Absyn_Vardecl*_tmp4B4;
struct Cyc_Absyn_Fndecl*_tmp4B6;struct Cyc_Absyn_Typedefdecl*_tmp4B8;struct Cyc_Absyn_Aggrdecl*
_tmp4BA;struct Cyc_Absyn_Enumdecl*_tmp4BC;_LL2B8: {struct Cyc_Absyn_Porton_d_struct*
_tmp4B1=(struct Cyc_Absyn_Porton_d_struct*)_tmp4B0;if(_tmp4B1->tag != 14)goto
_LL2BA;}_LL2B9:(env->gcenv)->porting=1;return env;_LL2BA: {struct Cyc_Absyn_Portoff_d_struct*
_tmp4B2=(struct Cyc_Absyn_Portoff_d_struct*)_tmp4B0;if(_tmp4B2->tag != 15)goto
_LL2BC;}_LL2BB:(env->gcenv)->porting=0;return env;_LL2BC: {struct Cyc_Absyn_Var_d_struct*
_tmp4B3=(struct Cyc_Absyn_Var_d_struct*)_tmp4B0;if(_tmp4B3->tag != 0)goto _LL2BE;
else{_tmp4B4=_tmp4B3->f1;}}_LL2BD:(*_tmp4B4->name).f1=Cyc_Absyn_Loc_n;if(Cyc_Port_declared_var(
env,_tmp4B4->name)){void*_tmp4BE;void*_tmp4BF;struct _tuple10 _tmp4BD=Cyc_Port_lookup_var(
env,_tmp4B4->name);_tmp4BE=_tmp4BD.f1;_tmp4BF=_tmp4BD.f2;{void*q2;if((env->gcenv)->porting
 && !Cyc_Port_isfn(env,_tmp4B4->name)){q2=Cyc_Port_var();Cyc_Port_register_const_cvar(
env,q2,(_tmp4B4->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(
_tmp4B4->tq).loc);}else{q2=(_tmp4B4->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
Cyc_Port_unifies(_tmp4BE,q2);Cyc_Port_unifies(_tmp4BF,Cyc_Port_type_to_ctype(env,
_tmp4B4->type));if((unsigned int)_tmp4B4->initializer){struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)_check_null(_tmp4B4->initializer);Cyc_Port_leq(Cyc_Port_gen_initializer(
env,_tmp4B4->type,e),_tmp4BF);}return env;};}else{return Cyc_Port_gen_localdecl(
env,d);}_LL2BE: {struct Cyc_Absyn_Fn_d_struct*_tmp4B5=(struct Cyc_Absyn_Fn_d_struct*)
_tmp4B0;if(_tmp4B5->tag != 1)goto _LL2C0;else{_tmp4B6=_tmp4B5->f1;}}_LL2BF:(*
_tmp4B6->name).f1=Cyc_Absyn_Loc_n;{struct _tuple13*predeclared=0;if(Cyc_Port_declared_var(
env,_tmp4B6->name))predeclared=(struct _tuple13*)Cyc_Port_lookup_full_var(env,
_tmp4B6->name);{void*_tmp4C0=_tmp4B6->ret_type;struct Cyc_List_List*_tmp4C1=
_tmp4B6->args;struct Cyc_List_List*_tmp4C2=((struct Cyc_List_List*(*)(struct
_tuple8*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_make_targ,
_tmp4C1);struct Cyc_Absyn_FnType_struct _tmp672;struct Cyc_Absyn_FnInfo _tmp671;
struct Cyc_Absyn_FnType_struct*_tmp670;struct Cyc_Absyn_FnType_struct*_tmp4C3=(
_tmp670=_cycalloc(sizeof(*_tmp670)),((_tmp670[0]=((_tmp672.tag=9,((_tmp672.f1=((
_tmp671.tvars=0,((_tmp671.effect=0,((_tmp671.ret_tqual=Cyc_Absyn_empty_tqual(0),((
_tmp671.ret_typ=_tmp4C0,((_tmp671.args=_tmp4C2,((_tmp671.c_varargs=0,((_tmp671.cyc_varargs=
0,((_tmp671.rgn_po=0,((_tmp671.attributes=0,_tmp671)))))))))))))))))),_tmp672)))),
_tmp670)));struct Cyc_Port_Cenv*_tmp4C4=Cyc_Port_set_cpos(env,Cyc_Port_FnBody_pos);
void*_tmp4C5=Cyc_Port_type_to_ctype(_tmp4C4,_tmp4C0);struct Cyc_List_List*c_args=
0;struct Cyc_List_List*c_arg_types=0;{struct Cyc_List_List*_tmp4C6=_tmp4C1;for(0;(
unsigned int)_tmp4C6;_tmp4C6=_tmp4C6->tl){struct _dyneither_ptr*_tmp4C8;struct Cyc_Absyn_Tqual
_tmp4C9;void*_tmp4CA;struct _tuple16 _tmp4C7=*((struct _tuple16*)_tmp4C6->hd);
_tmp4C8=_tmp4C7.f1;_tmp4C9=_tmp4C7.f2;_tmp4CA=_tmp4C7.f3;{void*_tmp4CB=Cyc_Port_type_to_ctype(
_tmp4C4,_tmp4CA);void*tqv;if((env->gcenv)->porting){tqv=Cyc_Port_var();Cyc_Port_register_const_cvar(
env,tqv,_tmp4C9.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp4C9.loc);}
else{tqv=_tmp4C9.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}{struct
_tuple17*_tmp675;struct Cyc_List_List*_tmp674;c_args=((_tmp674=_cycalloc(sizeof(*
_tmp674)),((_tmp674->hd=((_tmp675=_cycalloc(sizeof(*_tmp675)),((_tmp675->f1=
_tmp4C8,((_tmp675->f2=_tmp4CA,((_tmp675->f3=tqv,((_tmp675->f4=_tmp4CB,_tmp675)))))))))),((
_tmp674->tl=c_args,_tmp674))))));}{struct Cyc_List_List*_tmp676;c_arg_types=((
_tmp676=_cycalloc(sizeof(*_tmp676)),((_tmp676->hd=_tmp4CB,((_tmp676->tl=
c_arg_types,_tmp676))))));};};}}c_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(c_args);c_arg_types=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(c_arg_types);{void*_tmp4CF=Cyc_Port_fn_ct(_tmp4C5,
c_arg_types);(*_tmp4B6->name).f1=Cyc_Absyn_Loc_n;_tmp4C4=Cyc_Port_add_var(
_tmp4C4,_tmp4B6->name,(void*)_tmp4C3,Cyc_Port_const_ct(),_tmp4CF);Cyc_Port_add_return_type(
_tmp4C4,_tmp4C5);{struct Cyc_List_List*_tmp4D0=c_args;for(0;(unsigned int)_tmp4D0;
_tmp4D0=_tmp4D0->tl){struct _dyneither_ptr*_tmp4D2;void*_tmp4D3;void*_tmp4D4;void*
_tmp4D5;struct _tuple17 _tmp4D1=*((struct _tuple17*)_tmp4D0->hd);_tmp4D2=_tmp4D1.f1;
_tmp4D3=_tmp4D1.f2;_tmp4D4=_tmp4D1.f3;_tmp4D5=_tmp4D1.f4;{struct _tuple0*_tmp677;
_tmp4C4=Cyc_Port_add_var(_tmp4C4,((_tmp677=_cycalloc(sizeof(*_tmp677)),((_tmp677->f1=
Cyc_Absyn_Loc_n,((_tmp677->f2=_tmp4D2,_tmp677)))))),_tmp4D3,_tmp4D4,_tmp4D5);};}}
Cyc_Port_gen_stmt(_tmp4C4,_tmp4B6->body);Cyc_Port_generalize(0,_tmp4CF);{struct
Cyc_Dict_Dict counts=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);Cyc_Port_region_counts(&
counts,_tmp4CF);Cyc_Port_register_rgns(env,counts,1,(void*)_tmp4C3,_tmp4CF);env=
Cyc_Port_add_var(_tmp4C4,_tmp4B6->name,(void*)_tmp4C3,Cyc_Port_const_ct(),
_tmp4CF);if((unsigned int)predeclared){void*_tmp4D8;struct _tuple10*_tmp4D9;
struct _tuple10 _tmp4DA;void*_tmp4DB;void*_tmp4DC;struct _tuple13 _tmp4D7=*
predeclared;_tmp4D8=_tmp4D7.f1;_tmp4D9=_tmp4D7.f2;_tmp4DA=*_tmp4D9;_tmp4DB=
_tmp4DA.f1;_tmp4DC=_tmp4DA.f2;Cyc_Port_unifies(_tmp4DB,Cyc_Port_const_ct());Cyc_Port_unifies(
_tmp4DC,Cyc_Port_instantiate(_tmp4CF));Cyc_Port_register_rgns(env,counts,1,
_tmp4D8,_tmp4CF);}return env;};};};};_LL2C0: {struct Cyc_Absyn_Typedef_d_struct*
_tmp4B7=(struct Cyc_Absyn_Typedef_d_struct*)_tmp4B0;if(_tmp4B7->tag != 9)goto
_LL2C2;else{_tmp4B8=_tmp4B7->f1;}}_LL2C1: {void*_tmp4E0=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4B8->defn))->v;void*_tmp4E1=Cyc_Port_type_to_ctype(env,_tmp4E0);(*
_tmp4B8->name).f1=Cyc_Absyn_Loc_n;Cyc_Port_add_typedef(env,_tmp4B8->name,_tmp4E0,
_tmp4E1);return env;}_LL2C2: {struct Cyc_Absyn_Aggr_d_struct*_tmp4B9=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4B0;if(_tmp4B9->tag != 6)goto _LL2C4;else{_tmp4BA=_tmp4B9->f1;}}_LL2C3: {
struct _tuple0*_tmp4E2=_tmp4BA->name;(*_tmp4BA->name).f1=Cyc_Absyn_Loc_n;{struct
_tuple11*previous;struct Cyc_List_List*curr=0;switch(_tmp4BA->kind){case Cyc_Absyn_StructA:
_LL2C8: previous=Cyc_Port_lookup_struct_decl(env,_tmp4E2);break;case Cyc_Absyn_UnionA:
_LL2C9: previous=Cyc_Port_lookup_union_decl(env,_tmp4E2);break;}if((unsigned int)
_tmp4BA->impl){struct Cyc_List_List*cf=(*previous).f2;{struct Cyc_List_List*
_tmp4E3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BA->impl))->fields;for(
0;(unsigned int)_tmp4E3;_tmp4E3=_tmp4E3->tl){struct Cyc_Absyn_Aggrfield*_tmp4E4=(
struct Cyc_Absyn_Aggrfield*)_tmp4E3->hd;void*qv;if((env->gcenv)->porting){qv=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,qv,(_tmp4E4->tq).print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct(),(_tmp4E4->tq).loc);}else{qv=(_tmp4E4->tq).print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct();}{void*_tmp4E5=Cyc_Port_type_to_ctype(env,_tmp4E4->type);
if(cf != 0){void*_tmp4E7;void*_tmp4E8;struct Cyc_Port_Cfield _tmp4E6=*((struct Cyc_Port_Cfield*)
cf->hd);_tmp4E7=_tmp4E6.qual;_tmp4E8=_tmp4E6.type;cf=cf->tl;Cyc_Port_unifies(qv,
_tmp4E7);Cyc_Port_unifies(_tmp4E5,_tmp4E8);}{struct Cyc_Port_Cfield*_tmp67A;
struct Cyc_List_List*_tmp679;curr=((_tmp679=_cycalloc(sizeof(*_tmp679)),((_tmp679->hd=((
_tmp67A=_cycalloc(sizeof(*_tmp67A)),((_tmp67A->qual=qv,((_tmp67A->f=_tmp4E4->name,((
_tmp67A->type=_tmp4E5,_tmp67A)))))))),((_tmp679->tl=curr,_tmp679))))));};};}}
curr=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(curr);
if((*previous).f2 == 0)(*previous).f2=curr;}return env;};}_LL2C4: {struct Cyc_Absyn_Enum_d_struct*
_tmp4BB=(struct Cyc_Absyn_Enum_d_struct*)_tmp4B0;if(_tmp4BB->tag != 8)goto _LL2C6;
else{_tmp4BC=_tmp4BB->f1;}}_LL2C5:(*_tmp4BC->name).f1=Cyc_Absyn_Loc_n;if((
unsigned int)_tmp4BC->fields){struct Cyc_List_List*_tmp4EB=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4BC->fields))->v;for(0;(unsigned int)_tmp4EB;
_tmp4EB=_tmp4EB->tl){(*((struct Cyc_Absyn_Enumfield*)_tmp4EB->hd)->name).f1=Cyc_Absyn_Loc_n;{
struct Cyc_Absyn_EnumType_struct _tmp67D;struct Cyc_Absyn_EnumType_struct*_tmp67C;
env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)_tmp4EB->hd)->name,(void*)((
_tmp67C=_cycalloc(sizeof(*_tmp67C)),((_tmp67C[0]=((_tmp67D.tag=13,((_tmp67D.f1=
_tmp4BC->name,((_tmp67D.f2=(struct Cyc_Absyn_Enumdecl*)_tmp4BC,_tmp67D)))))),
_tmp67C)))),Cyc_Port_const_ct(),Cyc_Port_arith_ct());};}}return env;_LL2C6:;
_LL2C7: if((env->gcenv)->porting){const char*_tmp680;void*_tmp67F;(_tmp67F=0,Cyc_fprintf(
Cyc_stderr,((_tmp680="Warning: Cyclone declaration found in code to be ported -- skipping.",
_tag_dyneither(_tmp680,sizeof(char),69))),_tag_dyneither(_tmp67F,sizeof(void*),0)));}
return env;_LL2B7:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*
env,struct Cyc_List_List*ds){for(0;(unsigned int)ds;ds=ds->tl){env=Cyc_Port_gen_topdecl(
env,(struct Cyc_Absyn_Decl*)ds->hd);}return env;}struct Cyc_Port_Cenv;struct Cyc_Port_Edit;
struct Cyc_Port_Edit;struct Cyc_Port_Edit;static struct Cyc_List_List*Cyc_Port_gen_edits(
struct Cyc_List_List*ds){struct Cyc_Port_Cenv*env=Cyc_Port_gen_topdecls(Cyc_Port_initial_cenv(),
ds);struct Cyc_List_List*_tmp4F0=(env->gcenv)->pointer_edits;struct Cyc_List_List*
_tmp4F1=(env->gcenv)->qualifier_edits;struct Cyc_List_List*_tmp4F2=(env->gcenv)->zeroterm_edits;
struct Cyc_List_List*_tmp4F3=(env->gcenv)->edits;{struct Cyc_List_List*_tmp4F4=
_tmp4F0;for(0;(unsigned int)_tmp4F4;_tmp4F4=_tmp4F4->tl){void*_tmp4F6;void*
_tmp4F7;struct Cyc_Position_Segment*_tmp4F8;struct _tuple14 _tmp4F5=*((struct
_tuple14*)_tmp4F4->hd);_tmp4F6=_tmp4F5.f1;_tmp4F7=_tmp4F5.f2;_tmp4F8=_tmp4F5.f3;
Cyc_Port_unifies(_tmp4F6,_tmp4F7);}}{struct Cyc_List_List*_tmp4F9=_tmp4F1;for(0;(
unsigned int)_tmp4F9;_tmp4F9=_tmp4F9->tl){void*_tmp4FB;void*_tmp4FC;struct Cyc_Position_Segment*
_tmp4FD;struct _tuple14 _tmp4FA=*((struct _tuple14*)_tmp4F9->hd);_tmp4FB=_tmp4FA.f1;
_tmp4FC=_tmp4FA.f2;_tmp4FD=_tmp4FA.f3;Cyc_Port_unifies(_tmp4FB,_tmp4FC);}}{
struct Cyc_List_List*_tmp4FE=_tmp4F2;for(0;(unsigned int)_tmp4FE;_tmp4FE=_tmp4FE->tl){
void*_tmp500;void*_tmp501;struct Cyc_Position_Segment*_tmp502;struct _tuple14
_tmp4FF=*((struct _tuple14*)_tmp4FE->hd);_tmp500=_tmp4FF.f1;_tmp501=_tmp4FF.f2;
_tmp502=_tmp4FF.f3;Cyc_Port_unifies(_tmp500,_tmp501);}}for(0;(unsigned int)
_tmp4F0;_tmp4F0=_tmp4F0->tl){void*_tmp504;void*_tmp505;struct Cyc_Position_Segment*
_tmp506;struct _tuple14 _tmp503=*((struct _tuple14*)_tmp4F0->hd);_tmp504=_tmp503.f1;
_tmp505=_tmp503.f2;_tmp506=_tmp503.f3;if(!Cyc_Port_unifies(_tmp504,_tmp505) && (
unsigned int)_tmp506){void*_tmp507=Cyc_Port_compress_ct(_tmp505);_LL2CC: {struct
Cyc_Port_Thin_ct_struct*_tmp508=(struct Cyc_Port_Thin_ct_struct*)_tmp507;if(
_tmp508->tag != 2)goto _LL2CE;}_LL2CD:{struct Cyc_Port_Edit*_tmp689;const char*
_tmp688;const char*_tmp687;struct Cyc_List_List*_tmp686;_tmp4F3=((_tmp686=
_cycalloc(sizeof(*_tmp686)),((_tmp686->hd=((_tmp689=_cycalloc(sizeof(*_tmp689)),((
_tmp689->loc=_tmp506,((_tmp689->old_string=((_tmp688="?",_tag_dyneither(_tmp688,
sizeof(char),2))),((_tmp689->new_string=((_tmp687="*",_tag_dyneither(_tmp687,
sizeof(char),2))),_tmp689)))))))),((_tmp686->tl=_tmp4F3,_tmp686))))));}goto
_LL2CB;_LL2CE: {struct Cyc_Port_Fat_ct_struct*_tmp509=(struct Cyc_Port_Fat_ct_struct*)
_tmp507;if(_tmp509->tag != 3)goto _LL2D0;}_LL2CF:{struct Cyc_Port_Edit*_tmp692;
const char*_tmp691;const char*_tmp690;struct Cyc_List_List*_tmp68F;_tmp4F3=((
_tmp68F=_cycalloc(sizeof(*_tmp68F)),((_tmp68F->hd=((_tmp692=_cycalloc(sizeof(*
_tmp692)),((_tmp692->loc=_tmp506,((_tmp692->old_string=((_tmp691="*",
_tag_dyneither(_tmp691,sizeof(char),2))),((_tmp692->new_string=((_tmp690="?",
_tag_dyneither(_tmp690,sizeof(char),2))),_tmp692)))))))),((_tmp68F->tl=_tmp4F3,
_tmp68F))))));}goto _LL2CB;_LL2D0:;_LL2D1: goto _LL2CB;_LL2CB:;}}for(0;(
unsigned int)_tmp4F1;_tmp4F1=_tmp4F1->tl){void*_tmp513;void*_tmp514;struct Cyc_Position_Segment*
_tmp515;struct _tuple14 _tmp512=*((struct _tuple14*)_tmp4F1->hd);_tmp513=_tmp512.f1;
_tmp514=_tmp512.f2;_tmp515=_tmp512.f3;if(!Cyc_Port_unifies(_tmp513,_tmp514) && (
unsigned int)_tmp515){void*_tmp516=Cyc_Port_compress_ct(_tmp514);_LL2D3: {struct
Cyc_Port_Notconst_ct_struct*_tmp517=(struct Cyc_Port_Notconst_ct_struct*)_tmp516;
if(_tmp517->tag != 1)goto _LL2D5;}_LL2D4:{struct Cyc_Port_Edit*_tmp69B;const char*
_tmp69A;const char*_tmp699;struct Cyc_List_List*_tmp698;_tmp4F3=((_tmp698=
_cycalloc(sizeof(*_tmp698)),((_tmp698->hd=((_tmp69B=_cycalloc(sizeof(*_tmp69B)),((
_tmp69B->loc=_tmp515,((_tmp69B->old_string=((_tmp69A="const ",_tag_dyneither(
_tmp69A,sizeof(char),7))),((_tmp69B->new_string=((_tmp699="",_tag_dyneither(
_tmp699,sizeof(char),1))),_tmp69B)))))))),((_tmp698->tl=_tmp4F3,_tmp698))))));}
goto _LL2D2;_LL2D5: {struct Cyc_Port_Const_ct_struct*_tmp518=(struct Cyc_Port_Const_ct_struct*)
_tmp516;if(_tmp518->tag != 0)goto _LL2D7;}_LL2D6:{struct Cyc_Port_Edit*_tmp6A4;
const char*_tmp6A3;const char*_tmp6A2;struct Cyc_List_List*_tmp6A1;_tmp4F3=((
_tmp6A1=_cycalloc(sizeof(*_tmp6A1)),((_tmp6A1->hd=((_tmp6A4=_cycalloc(sizeof(*
_tmp6A4)),((_tmp6A4->loc=_tmp515,((_tmp6A4->old_string=((_tmp6A3="",
_tag_dyneither(_tmp6A3,sizeof(char),1))),((_tmp6A4->new_string=((_tmp6A2="const ",
_tag_dyneither(_tmp6A2,sizeof(char),7))),_tmp6A4)))))))),((_tmp6A1->tl=_tmp4F3,
_tmp6A1))))));}goto _LL2D2;_LL2D7:;_LL2D8: goto _LL2D2;_LL2D2:;}}for(0;(
unsigned int)_tmp4F2;_tmp4F2=_tmp4F2->tl){void*_tmp522;void*_tmp523;struct Cyc_Position_Segment*
_tmp524;struct _tuple14 _tmp521=*((struct _tuple14*)_tmp4F2->hd);_tmp522=_tmp521.f1;
_tmp523=_tmp521.f2;_tmp524=_tmp521.f3;if(!Cyc_Port_unifies(_tmp522,_tmp523) && (
unsigned int)_tmp524){void*_tmp525=Cyc_Port_compress_ct(_tmp523);_LL2DA: {struct
Cyc_Port_Zterm_ct_struct*_tmp526=(struct Cyc_Port_Zterm_ct_struct*)_tmp525;if(
_tmp526->tag != 8)goto _LL2DC;}_LL2DB:{struct Cyc_Port_Edit*_tmp6AD;const char*
_tmp6AC;const char*_tmp6AB;struct Cyc_List_List*_tmp6AA;_tmp4F3=((_tmp6AA=
_cycalloc(sizeof(*_tmp6AA)),((_tmp6AA->hd=((_tmp6AD=_cycalloc(sizeof(*_tmp6AD)),((
_tmp6AD->loc=_tmp524,((_tmp6AD->old_string=((_tmp6AC="NOZEROTERM ",
_tag_dyneither(_tmp6AC,sizeof(char),12))),((_tmp6AD->new_string=((_tmp6AB="",
_tag_dyneither(_tmp6AB,sizeof(char),1))),_tmp6AD)))))))),((_tmp6AA->tl=_tmp4F3,
_tmp6AA))))));}goto _LL2D9;_LL2DC: {struct Cyc_Port_Nozterm_ct_struct*_tmp527=(
struct Cyc_Port_Nozterm_ct_struct*)_tmp525;if(_tmp527->tag != 9)goto _LL2DE;}_LL2DD:{
struct Cyc_Port_Edit*_tmp6B6;const char*_tmp6B5;const char*_tmp6B4;struct Cyc_List_List*
_tmp6B3;_tmp4F3=((_tmp6B3=_cycalloc(sizeof(*_tmp6B3)),((_tmp6B3->hd=((_tmp6B6=
_cycalloc(sizeof(*_tmp6B6)),((_tmp6B6->loc=_tmp524,((_tmp6B6->old_string=((
_tmp6B5="ZEROTERM ",_tag_dyneither(_tmp6B5,sizeof(char),10))),((_tmp6B6->new_string=((
_tmp6B4="",_tag_dyneither(_tmp6B4,sizeof(char),1))),_tmp6B6)))))))),((_tmp6B3->tl=
_tmp4F3,_tmp6B3))))));}goto _LL2D9;_LL2DE:;_LL2DF: goto _LL2D9;_LL2D9:;}}_tmp4F3=((
struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),
struct Cyc_List_List*x))Cyc_List_merge_sort)(Cyc_Port_cmp_edit,_tmp4F3);while((
unsigned int)_tmp4F3  && ((struct Cyc_Position_Segment*)_check_null(((struct Cyc_Port_Edit*)
_tmp4F3->hd)->loc))->start == 0){_tmp4F3=_tmp4F3->tl;}return _tmp4F3;}struct Cyc_Port_Edit;
struct Cyc_Port_Edit;static struct Cyc_Position_Segment*Cyc_Port_get_seg(struct Cyc_Port_Edit*
e){return e->loc;}struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
void Cyc_Port_port(struct Cyc_List_List*ds){struct Cyc_List_List*_tmp530=Cyc_Port_gen_edits(
ds);struct Cyc_List_List*_tmp531=((struct Cyc_List_List*(*)(struct Cyc_Position_Segment*(*
f)(struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_get_seg,
_tmp530);Cyc_Position_use_gcc_style_location=0;{struct Cyc_List_List*_tmp532=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Position_strings_of_segments(
_tmp531));for(0;(unsigned int)_tmp530;(_tmp530=_tmp530->tl,_tmp532=_tmp532->tl)){
struct Cyc_Position_Segment*_tmp534;struct _dyneither_ptr _tmp535;struct
_dyneither_ptr _tmp536;struct Cyc_Port_Edit _tmp533=*((struct Cyc_Port_Edit*)_tmp530->hd);
_tmp534=_tmp533.loc;_tmp535=_tmp533.old_string;_tmp536=_tmp533.new_string;{
struct _dyneither_ptr sloc=(struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct
Cyc_List_List*)_check_null(_tmp532))->hd);const char*_tmp6BC;void*_tmp6BB[3];
struct Cyc_String_pa_struct _tmp6BA;struct Cyc_String_pa_struct _tmp6B9;struct Cyc_String_pa_struct
_tmp6B8;(_tmp6B8.tag=0,((_tmp6B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp536),((_tmp6B9.tag=0,((_tmp6B9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp535),((_tmp6BA.tag=0,((_tmp6BA.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)sloc),((_tmp6BB[0]=& _tmp6BA,((_tmp6BB[1]=& _tmp6B9,((_tmp6BB[
2]=& _tmp6B8,Cyc_printf(((_tmp6BC="%s: insert `%s' for `%s'\n",_tag_dyneither(
_tmp6BC,sizeof(char),26))),_tag_dyneither(_tmp6BB,sizeof(void*),3)))))))))))))))))));};}};}
