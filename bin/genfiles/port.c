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
tag;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_RefCntRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{int
tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
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
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
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
int tag;struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
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
_tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Position_Segment*loc,void*t);struct _tuple9{void*f1;void*f2;};struct Cyc_Tcexp_TestEnv{
struct _tuple9*eq;int isTrue;};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);extern char Cyc_Tcdecl_Incompatible[
13];struct Cyc_Tcdecl_Incompatible_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{
struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};struct
Cyc_Position_Segment{int start;int end;};struct Cyc_Port_Edit{struct Cyc_Position_Segment*
loc;struct _dyneither_ptr old_string;struct _dyneither_ptr new_string;};struct Cyc_Port_Edit;
struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;int Cyc_Port_cmp_edit(
struct Cyc_Port_Edit*e1,struct Cyc_Port_Edit*e2){if((unsigned int)e1->loc  && (
unsigned int)e2->loc)return((struct Cyc_Position_Segment*)_check_null(e1->loc))->start
- ((struct Cyc_Position_Segment*)_check_null(e2->loc))->start;else{if(e1->loc == 
e2->loc)return 0;else{if((unsigned int)e1->loc)return 1;else{return - 1;}}}}struct
Cyc_Port_Cvar{int id;void**eq;struct Cyc_List_List*uppers;struct Cyc_List_List*
lowers;};struct Cyc_Port_Cfield{void*qual;struct _dyneither_ptr*f;void*type;};
struct Cyc_Port_Const_ct_struct{int tag;};struct Cyc_Port_Notconst_ct_struct{int tag;
};struct Cyc_Port_Thin_ct_struct{int tag;};struct Cyc_Port_Fat_ct_struct{int tag;};
struct Cyc_Port_Void_ct_struct{int tag;};struct Cyc_Port_Zero_ct_struct{int tag;};
struct Cyc_Port_Arith_ct_struct{int tag;};struct Cyc_Port_Heap_ct_struct{int tag;};
struct Cyc_Port_Zterm_ct_struct{int tag;};struct Cyc_Port_Nozterm_ct_struct{int tag;
};struct Cyc_Port_RgnVar_ct_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Port_Ptr_ct_struct{
int tag;void*f1;void*f2;void*f3;void*f4;void*f5;};struct Cyc_Port_Array_ct_struct{
int tag;void*f1;void*f2;void*f3;};struct _tuple10{struct Cyc_Absyn_Aggrdecl*f1;
struct Cyc_List_List*f2;};struct Cyc_Port_KnownAggr_ct_struct{int tag;struct
_tuple10*f1;};struct Cyc_Port_UnknownAggr_ct_struct{int tag;struct Cyc_List_List*f1;
void**f2;};struct Cyc_Port_Fn_ct_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Port_Cvar;struct Cyc_Port_Var_ct_struct{int tag;struct Cyc_Port_Cvar*f1;};
struct Cyc_Port_Const_ct_struct Cyc_Port_Const_ct_val={0};struct Cyc_Port_Notconst_ct_struct
Cyc_Port_Notconst_ct_val={1};struct Cyc_Port_Thin_ct_struct Cyc_Port_Thin_ct_val={
2};struct Cyc_Port_Fat_ct_struct Cyc_Port_Fat_ct_val={3};struct Cyc_Port_Void_ct_struct
Cyc_Port_Void_ct_val={4};struct Cyc_Port_Zero_ct_struct Cyc_Port_Zero_ct_val={5};
struct Cyc_Port_Arith_ct_struct Cyc_Port_Arith_ct_val={6};struct Cyc_Port_Heap_ct_struct
Cyc_Port_Heap_ct_val={7};struct Cyc_Port_Zterm_ct_struct Cyc_Port_Zterm_ct_val={8};
struct Cyc_Port_Nozterm_ct_struct Cyc_Port_Nozterm_ct_val={9};static struct
_dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts);static
struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*ts);
struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;static struct _dyneither_ptr Cyc_Port_ctype2string(
int deep,void*t){void*_tmpA=t;struct _dyneither_ptr*_tmp16;void*_tmp18;void*_tmp19;
void*_tmp1A;void*_tmp1B;void*_tmp1C;void*_tmp1E;void*_tmp1F;void*_tmp20;struct
_tuple10*_tmp22;struct _tuple10 _tmp23;struct Cyc_Absyn_Aggrdecl*_tmp24;struct Cyc_List_List*
_tmp25;struct Cyc_List_List*_tmp27;void**_tmp28;void*_tmp29;struct Cyc_List_List*
_tmp2B;void*_tmp2D;struct Cyc_List_List*_tmp2E;struct Cyc_Port_Cvar*_tmp30;_LL1: {
struct Cyc_Port_Const_ct_struct*_tmpB=(struct Cyc_Port_Const_ct_struct*)_tmpA;if(
_tmpB->tag != 0)goto _LL3;}_LL2: {const char*_tmp53D;return(_tmp53D="const",
_tag_dyneither(_tmp53D,sizeof(char),6));}_LL3: {struct Cyc_Port_Notconst_ct_struct*
_tmpC=(struct Cyc_Port_Notconst_ct_struct*)_tmpA;if(_tmpC->tag != 1)goto _LL5;}_LL4: {
const char*_tmp53E;return(_tmp53E="notconst",_tag_dyneither(_tmp53E,sizeof(char),
9));}_LL5: {struct Cyc_Port_Thin_ct_struct*_tmpD=(struct Cyc_Port_Thin_ct_struct*)
_tmpA;if(_tmpD->tag != 2)goto _LL7;}_LL6: {const char*_tmp53F;return(_tmp53F="thin",
_tag_dyneither(_tmp53F,sizeof(char),5));}_LL7: {struct Cyc_Port_Fat_ct_struct*
_tmpE=(struct Cyc_Port_Fat_ct_struct*)_tmpA;if(_tmpE->tag != 3)goto _LL9;}_LL8: {
const char*_tmp540;return(_tmp540="fat",_tag_dyneither(_tmp540,sizeof(char),4));}
_LL9: {struct Cyc_Port_Void_ct_struct*_tmpF=(struct Cyc_Port_Void_ct_struct*)_tmpA;
if(_tmpF->tag != 4)goto _LLB;}_LLA: {const char*_tmp541;return(_tmp541="void",
_tag_dyneither(_tmp541,sizeof(char),5));}_LLB: {struct Cyc_Port_Zero_ct_struct*
_tmp10=(struct Cyc_Port_Zero_ct_struct*)_tmpA;if(_tmp10->tag != 5)goto _LLD;}_LLC: {
const char*_tmp542;return(_tmp542="zero",_tag_dyneither(_tmp542,sizeof(char),5));}
_LLD: {struct Cyc_Port_Arith_ct_struct*_tmp11=(struct Cyc_Port_Arith_ct_struct*)
_tmpA;if(_tmp11->tag != 6)goto _LLF;}_LLE: {const char*_tmp543;return(_tmp543="arith",
_tag_dyneither(_tmp543,sizeof(char),6));}_LLF: {struct Cyc_Port_Heap_ct_struct*
_tmp12=(struct Cyc_Port_Heap_ct_struct*)_tmpA;if(_tmp12->tag != 7)goto _LL11;}_LL10: {
const char*_tmp544;return(_tmp544="heap",_tag_dyneither(_tmp544,sizeof(char),5));}
_LL11: {struct Cyc_Port_Zterm_ct_struct*_tmp13=(struct Cyc_Port_Zterm_ct_struct*)
_tmpA;if(_tmp13->tag != 8)goto _LL13;}_LL12: {const char*_tmp545;return(_tmp545="ZT",
_tag_dyneither(_tmp545,sizeof(char),3));}_LL13: {struct Cyc_Port_Nozterm_ct_struct*
_tmp14=(struct Cyc_Port_Nozterm_ct_struct*)_tmpA;if(_tmp14->tag != 9)goto _LL15;}
_LL14: {const char*_tmp546;return(_tmp546="NZT",_tag_dyneither(_tmp546,sizeof(
char),4));}_LL15: {struct Cyc_Port_RgnVar_ct_struct*_tmp15=(struct Cyc_Port_RgnVar_ct_struct*)
_tmpA;if(_tmp15->tag != 10)goto _LL17;else{_tmp16=_tmp15->f1;}}_LL16: {const char*
_tmp54A;void*_tmp549[1];struct Cyc_String_pa_struct _tmp548;return(struct
_dyneither_ptr)((_tmp548.tag=0,((_tmp548.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp16),((_tmp549[0]=& _tmp548,Cyc_aprintf(((_tmp54A="%s",
_tag_dyneither(_tmp54A,sizeof(char),3))),_tag_dyneither(_tmp549,sizeof(void*),1))))))));}
_LL17: {struct Cyc_Port_Ptr_ct_struct*_tmp17=(struct Cyc_Port_Ptr_ct_struct*)_tmpA;
if(_tmp17->tag != 11)goto _LL19;else{_tmp18=(void*)_tmp17->f1;_tmp19=(void*)_tmp17->f2;
_tmp1A=(void*)_tmp17->f3;_tmp1B=(void*)_tmp17->f4;_tmp1C=(void*)_tmp17->f5;}}
_LL18: {const char*_tmp552;void*_tmp551[5];struct Cyc_String_pa_struct _tmp550;
struct Cyc_String_pa_struct _tmp54F;struct Cyc_String_pa_struct _tmp54E;struct Cyc_String_pa_struct
_tmp54D;struct Cyc_String_pa_struct _tmp54C;return(struct _dyneither_ptr)((_tmp54C.tag=
0,((_tmp54C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,_tmp1C)),((_tmp54D.tag=0,((_tmp54D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp1B)),((_tmp54E.tag=0,((_tmp54E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp1A)),((
_tmp54F.tag=0,((_tmp54F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,_tmp19)),((_tmp550.tag=0,((_tmp550.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp18)),((_tmp551[0]=& _tmp550,((
_tmp551[1]=& _tmp54F,((_tmp551[2]=& _tmp54E,((_tmp551[3]=& _tmp54D,((_tmp551[4]=&
_tmp54C,Cyc_aprintf(((_tmp552="ptr(%s,%s,%s,%s,%s)",_tag_dyneither(_tmp552,
sizeof(char),20))),_tag_dyneither(_tmp551,sizeof(void*),5))))))))))))))))))))))))))))))));}
_LL19: {struct Cyc_Port_Array_ct_struct*_tmp1D=(struct Cyc_Port_Array_ct_struct*)
_tmpA;if(_tmp1D->tag != 12)goto _LL1B;else{_tmp1E=(void*)_tmp1D->f1;_tmp1F=(void*)
_tmp1D->f2;_tmp20=(void*)_tmp1D->f3;}}_LL1A: {const char*_tmp558;void*_tmp557[3];
struct Cyc_String_pa_struct _tmp556;struct Cyc_String_pa_struct _tmp555;struct Cyc_String_pa_struct
_tmp554;return(struct _dyneither_ptr)((_tmp554.tag=0,((_tmp554.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp20)),((
_tmp555.tag=0,((_tmp555.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,_tmp1F)),((_tmp556.tag=0,((_tmp556.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp1E)),((_tmp557[0]=& _tmp556,((
_tmp557[1]=& _tmp555,((_tmp557[2]=& _tmp554,Cyc_aprintf(((_tmp558="array(%s,%s,%s)",
_tag_dyneither(_tmp558,sizeof(char),16))),_tag_dyneither(_tmp557,sizeof(void*),3))))))))))))))))))));}
_LL1B: {struct Cyc_Port_KnownAggr_ct_struct*_tmp21=(struct Cyc_Port_KnownAggr_ct_struct*)
_tmpA;if(_tmp21->tag != 13)goto _LL1D;else{_tmp22=_tmp21->f1;_tmp23=*_tmp22;_tmp24=
_tmp23.f1;_tmp25=_tmp23.f2;}}_LL1C: {const char*_tmp55A;const char*_tmp559;struct
_dyneither_ptr s=_tmp24->kind == Cyc_Absyn_StructA?(_tmp55A="struct",
_tag_dyneither(_tmp55A,sizeof(char),7)):((_tmp559="union",_tag_dyneither(_tmp559,
sizeof(char),6)));if(!deep){const char*_tmp55F;void*_tmp55E[2];struct Cyc_String_pa_struct
_tmp55D;struct Cyc_String_pa_struct _tmp55C;return(struct _dyneither_ptr)((_tmp55C.tag=
0,((_tmp55C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp24->name)),((_tmp55D.tag=0,((_tmp55D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp55E[0]=& _tmp55D,((_tmp55E[1]=& _tmp55C,Cyc_aprintf(((
_tmp55F="%s %s",_tag_dyneither(_tmp55F,sizeof(char),6))),_tag_dyneither(_tmp55E,
sizeof(void*),2))))))))))))));}else{const char*_tmp565;void*_tmp564[3];struct Cyc_String_pa_struct
_tmp563;struct Cyc_String_pa_struct _tmp562;struct Cyc_String_pa_struct _tmp561;
return(struct _dyneither_ptr)((_tmp561.tag=0,((_tmp561.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Port_cfields2string(0,_tmp25)),((_tmp562.tag=0,((
_tmp562.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp24->name)),((_tmp563.tag=0,((_tmp563.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp564[0]=& _tmp563,((_tmp564[1]=& _tmp562,((_tmp564[2]=&
_tmp561,Cyc_aprintf(((_tmp565="%s %s {%s}",_tag_dyneither(_tmp565,sizeof(char),
11))),_tag_dyneither(_tmp564,sizeof(void*),3))))))))))))))))))));}}_LL1D: {
struct Cyc_Port_UnknownAggr_ct_struct*_tmp26=(struct Cyc_Port_UnknownAggr_ct_struct*)
_tmpA;if(_tmp26->tag != 14)goto _LL1F;else{_tmp27=_tmp26->f1;_tmp28=_tmp26->f2;if(
_tmp28 == 0)goto _LL1F;_tmp29=*_tmp28;}}_LL1E: return Cyc_Port_ctype2string(deep,
_tmp29);_LL1F: {struct Cyc_Port_UnknownAggr_ct_struct*_tmp2A=(struct Cyc_Port_UnknownAggr_ct_struct*)
_tmpA;if(_tmp2A->tag != 14)goto _LL21;else{_tmp2B=_tmp2A->f1;}}_LL20: {const char*
_tmp569;void*_tmp568[1];struct Cyc_String_pa_struct _tmp567;return(struct
_dyneither_ptr)((_tmp567.tag=0,((_tmp567.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_cfields2string(deep,_tmp2B)),((_tmp568[0]=& _tmp567,Cyc_aprintf(((
_tmp569="aggr {%s}",_tag_dyneither(_tmp569,sizeof(char),10))),_tag_dyneither(
_tmp568,sizeof(void*),1))))))));}_LL21: {struct Cyc_Port_Fn_ct_struct*_tmp2C=(
struct Cyc_Port_Fn_ct_struct*)_tmpA;if(_tmp2C->tag != 15)goto _LL23;else{_tmp2D=(
void*)_tmp2C->f1;_tmp2E=_tmp2C->f2;}}_LL22: {const char*_tmp56E;void*_tmp56D[2];
struct Cyc_String_pa_struct _tmp56C;struct Cyc_String_pa_struct _tmp56B;return(
struct _dyneither_ptr)((_tmp56B.tag=0,((_tmp56B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp2D)),((_tmp56C.tag=0,((_tmp56C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(deep,_tmp2E)),((
_tmp56D[0]=& _tmp56C,((_tmp56D[1]=& _tmp56B,Cyc_aprintf(((_tmp56E="fn(%s)->%s",
_tag_dyneither(_tmp56E,sizeof(char),11))),_tag_dyneither(_tmp56D,sizeof(void*),2))))))))))))));}
_LL23: {struct Cyc_Port_Var_ct_struct*_tmp2F=(struct Cyc_Port_Var_ct_struct*)_tmpA;
if(_tmp2F->tag != 16)goto _LL0;else{_tmp30=_tmp2F->f1;}}_LL24: if((unsigned int)
_tmp30->eq)return Cyc_Port_ctype2string(deep,*((void**)_check_null(_tmp30->eq)));
else{if(!deep  || _tmp30->uppers == 0  && _tmp30->lowers == 0){const char*_tmp572;
void*_tmp571[1];struct Cyc_Int_pa_struct _tmp570;return(struct _dyneither_ptr)((
_tmp570.tag=1,((_tmp570.f1=(unsigned long)_tmp30->id,((_tmp571[0]=& _tmp570,Cyc_aprintf(((
_tmp572="var(%d)",_tag_dyneither(_tmp572,sizeof(char),8))),_tag_dyneither(
_tmp571,sizeof(void*),1))))))));}else{const char*_tmp578;void*_tmp577[3];struct
Cyc_String_pa_struct _tmp576;struct Cyc_Int_pa_struct _tmp575;struct Cyc_String_pa_struct
_tmp574;return(struct _dyneither_ptr)((_tmp574.tag=0,((_tmp574.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(0,_tmp30->uppers)),((
_tmp575.tag=1,((_tmp575.f1=(unsigned long)_tmp30->id,((_tmp576.tag=0,((_tmp576.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(0,_tmp30->lowers)),((
_tmp577[0]=& _tmp576,((_tmp577[1]=& _tmp575,((_tmp577[2]=& _tmp574,Cyc_aprintf(((
_tmp578="var([%s]<=%d<=[%s])",_tag_dyneither(_tmp578,sizeof(char),20))),
_tag_dyneither(_tmp577,sizeof(void*),3))))))))))))))))))));}}_LL0:;}static struct
_dyneither_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){struct _dyneither_ptr*
_tmp579;return(_tmp579=_cycalloc(sizeof(*_tmp579)),((_tmp579[0]=Cyc_Port_ctype2string(
deep,t),_tmp579)));}struct Cyc_List_List*Cyc_Port_sep(struct _dyneither_ptr s,
struct Cyc_List_List*xs){struct _dyneither_ptr*_tmp57A;struct _dyneither_ptr*_tmp65=(
_tmp57A=_cycalloc(sizeof(*_tmp57A)),((_tmp57A[0]=s,_tmp57A)));if(xs == 0)return xs;{
struct Cyc_List_List*_tmp66=xs;struct Cyc_List_List*_tmp67=xs->tl;for(0;_tmp67 != 0;(
_tmp66=_tmp67,_tmp67=_tmp67->tl)){struct Cyc_List_List*_tmp57B;((struct Cyc_List_List*)
_check_null(_tmp66))->tl=((_tmp57B=_cycalloc(sizeof(*_tmp57B)),((_tmp57B->hd=
_tmp65,((_tmp57B->tl=_tmp67,_tmp57B))))));}return xs;};}struct Cyc_Port_Cfield;
struct Cyc_Port_Cfield;static struct _dyneither_ptr*Cyc_Port_cfield2stringptr(int
deep,struct Cyc_Port_Cfield*f){const char*_tmp581;void*_tmp580[3];struct Cyc_String_pa_struct
_tmp57F;struct Cyc_String_pa_struct _tmp57E;struct Cyc_String_pa_struct _tmp57D;
struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp57D.tag=0,((_tmp57D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,f->type)),((
_tmp57E.tag=0,((_tmp57E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f->f),((
_tmp57F.tag=0,((_tmp57F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,f->qual)),((_tmp580[0]=& _tmp57F,((_tmp580[1]=& _tmp57E,((_tmp580[2]=& _tmp57D,
Cyc_aprintf(((_tmp581="%s %s: %s",_tag_dyneither(_tmp581,sizeof(char),10))),
_tag_dyneither(_tmp580,sizeof(void*),3))))))))))))))))))));struct _dyneither_ptr*
_tmp582;return(_tmp582=_cycalloc(sizeof(*_tmp582)),((_tmp582[0]=s,_tmp582)));}
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){
const char*_tmp583;return(struct _dyneither_ptr)Cyc_strconcat_l(Cyc_Port_sep(((
_tmp583=",",_tag_dyneither(_tmp583,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Port_ctype2stringptr,deep,ts)));}struct Cyc_Port_Cfield;static struct
_dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){const char*
_tmp584;return(struct _dyneither_ptr)Cyc_strconcat_l(Cyc_Port_sep(((_tmp584=";",
_tag_dyneither(_tmp584,sizeof(char),2))),((struct Cyc_List_List*(*)(struct
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
_dyneither_ptr*n){struct Cyc_Port_RgnVar_ct_struct _tmp587;struct Cyc_Port_RgnVar_ct_struct*
_tmp586;return(void*)((_tmp586=_cycalloc(sizeof(*_tmp586)),((_tmp586[0]=((
_tmp587.tag=10,((_tmp587.f1=n,_tmp587)))),_tmp586))));}static void*Cyc_Port_unknown_aggr_ct(
struct Cyc_List_List*fs){struct Cyc_Port_UnknownAggr_ct_struct _tmp58A;struct Cyc_Port_UnknownAggr_ct_struct*
_tmp589;return(void*)((_tmp589=_cycalloc(sizeof(*_tmp589)),((_tmp589[0]=((
_tmp58A.tag=14,((_tmp58A.f1=fs,((_tmp58A.f2=0,_tmp58A)))))),_tmp589))));}static
void*Cyc_Port_known_aggr_ct(struct _tuple10*p){struct Cyc_Port_KnownAggr_ct_struct
_tmp58D;struct Cyc_Port_KnownAggr_ct_struct*_tmp58C;return(void*)((_tmp58C=
_cycalloc(sizeof(*_tmp58C)),((_tmp58C[0]=((_tmp58D.tag=13,((_tmp58D.f1=p,_tmp58D)))),
_tmp58C))));}static void*Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind,void*r,
void*zt){struct Cyc_Port_Ptr_ct_struct _tmp590;struct Cyc_Port_Ptr_ct_struct*
_tmp58F;return(void*)((_tmp58F=_cycalloc(sizeof(*_tmp58F)),((_tmp58F[0]=((
_tmp590.tag=11,((_tmp590.f1=(void*)elt,((_tmp590.f2=(void*)qual,((_tmp590.f3=(
void*)ptr_kind,((_tmp590.f4=(void*)r,((_tmp590.f5=(void*)zt,_tmp590)))))))))))),
_tmp58F))));}static void*Cyc_Port_array_ct(void*elt,void*qual,void*zt){struct Cyc_Port_Array_ct_struct
_tmp593;struct Cyc_Port_Array_ct_struct*_tmp592;return(void*)((_tmp592=_cycalloc(
sizeof(*_tmp592)),((_tmp592[0]=((_tmp593.tag=12,((_tmp593.f1=(void*)elt,((
_tmp593.f2=(void*)qual,((_tmp593.f3=(void*)zt,_tmp593)))))))),_tmp592))));}
static void*Cyc_Port_fn_ct(void*return_type,struct Cyc_List_List*args){struct Cyc_Port_Fn_ct_struct
_tmp596;struct Cyc_Port_Fn_ct_struct*_tmp595;return(void*)((_tmp595=_cycalloc(
sizeof(*_tmp595)),((_tmp595[0]=((_tmp596.tag=15,((_tmp596.f1=(void*)return_type,((
_tmp596.f2=args,_tmp596)))))),_tmp595))));}struct Cyc_Port_Cvar;static void*Cyc_Port_var(){
static int counter=0;struct Cyc_Port_Var_ct_struct _tmp59C;struct Cyc_Port_Cvar*
_tmp59B;struct Cyc_Port_Var_ct_struct*_tmp59A;return(void*)((_tmp59A=_cycalloc(
sizeof(*_tmp59A)),((_tmp59A[0]=((_tmp59C.tag=16,((_tmp59C.f1=((_tmp59B=_cycalloc(
sizeof(*_tmp59B)),((_tmp59B->id=counter ++,((_tmp59B->eq=0,((_tmp59B->uppers=0,((
_tmp59B->lowers=0,_tmp59B)))))))))),_tmp59C)))),_tmp59A))));}static void*Cyc_Port_new_var(
void*x){return Cyc_Port_var();}static struct _dyneither_ptr*Cyc_Port_new_region_var(){
static int counter=0;const char*_tmp5A0;void*_tmp59F[1];struct Cyc_Int_pa_struct
_tmp59E;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp59E.tag=1,((_tmp59E.f1=(
unsigned long)counter ++,((_tmp59F[0]=& _tmp59E,Cyc_aprintf(((_tmp5A0="`R%d",
_tag_dyneither(_tmp5A0,sizeof(char),5))),_tag_dyneither(_tmp59F,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp5A1;return(_tmp5A1=_cycalloc(sizeof(*_tmp5A1)),((
_tmp5A1[0]=s,_tmp5A1)));}static int Cyc_Port_unifies(void*t1,void*t2);struct Cyc_Port_Cvar;
static void*Cyc_Port_compress_ct(void*t){void*_tmp85=t;struct Cyc_Port_Cvar*_tmp87;
struct Cyc_Port_Cvar _tmp88;void**_tmp89;void***_tmp8A;struct Cyc_List_List*_tmp8B;
struct Cyc_List_List*_tmp8C;void**_tmp8E;_LL26: {struct Cyc_Port_Var_ct_struct*
_tmp86=(struct Cyc_Port_Var_ct_struct*)_tmp85;if(_tmp86->tag != 16)goto _LL28;else{
_tmp87=_tmp86->f1;_tmp88=*_tmp87;_tmp89=_tmp88.eq;_tmp8A=(void***)&(*_tmp86->f1).eq;
_tmp8B=_tmp88.uppers;_tmp8C=_tmp88.lowers;}}_LL27: {void**_tmp8F=*_tmp8A;if((
unsigned int)_tmp8F){void*r=Cyc_Port_compress_ct(*_tmp8F);if(*_tmp8F != r){void**
_tmp5A2;*_tmp8A=((_tmp5A2=_cycalloc(sizeof(*_tmp5A2)),((_tmp5A2[0]=r,_tmp5A2))));}
return r;}for(0;_tmp8C != 0;_tmp8C=_tmp8C->tl){void*_tmp91=(void*)_tmp8C->hd;_LL2D: {
struct Cyc_Port_Const_ct_struct*_tmp92=(struct Cyc_Port_Const_ct_struct*)_tmp91;
if(_tmp92->tag != 0)goto _LL2F;}_LL2E: goto _LL30;_LL2F: {struct Cyc_Port_Nozterm_ct_struct*
_tmp93=(struct Cyc_Port_Nozterm_ct_struct*)_tmp91;if(_tmp93->tag != 9)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Port_Void_ct_struct*_tmp94=(struct Cyc_Port_Void_ct_struct*)
_tmp91;if(_tmp94->tag != 4)goto _LL33;}_LL32: goto _LL34;_LL33: {struct Cyc_Port_KnownAggr_ct_struct*
_tmp95=(struct Cyc_Port_KnownAggr_ct_struct*)_tmp91;if(_tmp95->tag != 13)goto _LL35;}
_LL34: goto _LL36;_LL35: {struct Cyc_Port_Fn_ct_struct*_tmp96=(struct Cyc_Port_Fn_ct_struct*)
_tmp91;if(_tmp96->tag != 15)goto _LL37;}_LL36:{void**_tmp5A3;*_tmp8A=((_tmp5A3=
_cycalloc(sizeof(*_tmp5A3)),((_tmp5A3[0]=(void*)_tmp8C->hd,_tmp5A3))));}return(
void*)_tmp8C->hd;_LL37:;_LL38: goto _LL2C;_LL2C:;}for(0;_tmp8B != 0;_tmp8B=_tmp8B->tl){
void*_tmp98=(void*)_tmp8B->hd;_LL3A: {struct Cyc_Port_Notconst_ct_struct*_tmp99=(
struct Cyc_Port_Notconst_ct_struct*)_tmp98;if(_tmp99->tag != 1)goto _LL3C;}_LL3B:
goto _LL3D;_LL3C: {struct Cyc_Port_Zterm_ct_struct*_tmp9A=(struct Cyc_Port_Zterm_ct_struct*)
_tmp98;if(_tmp9A->tag != 8)goto _LL3E;}_LL3D: goto _LL3F;_LL3E: {struct Cyc_Port_Zero_ct_struct*
_tmp9B=(struct Cyc_Port_Zero_ct_struct*)_tmp98;if(_tmp9B->tag != 5)goto _LL40;}
_LL3F: goto _LL41;_LL40: {struct Cyc_Port_KnownAggr_ct_struct*_tmp9C=(struct Cyc_Port_KnownAggr_ct_struct*)
_tmp98;if(_tmp9C->tag != 13)goto _LL42;}_LL41: goto _LL43;_LL42: {struct Cyc_Port_Fn_ct_struct*
_tmp9D=(struct Cyc_Port_Fn_ct_struct*)_tmp98;if(_tmp9D->tag != 15)goto _LL44;}_LL43:{
void**_tmp5A4;*_tmp8A=((_tmp5A4=_cycalloc(sizeof(*_tmp5A4)),((_tmp5A4[0]=(void*)
_tmp8B->hd,_tmp5A4))));}return(void*)_tmp8B->hd;_LL44:;_LL45: goto _LL39;_LL39:;}
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
_LL64: {void*_tmpBE;void*_tmpBF;struct _tuple9 _tmp5A5;struct _tuple9 _tmpBD=(
_tmp5A5.f1=Cyc_Port_inst(instenv,_tmpB0),((_tmp5A5.f2=Cyc_Port_inst(instenv,
_tmpB3),_tmp5A5)));_tmpBE=_tmpBD.f1;_tmpBF=_tmpBD.f2;if(_tmpBE == _tmpB0  && 
_tmpBF == _tmpB3)return t;{struct Cyc_Port_Ptr_ct_struct _tmp5A8;struct Cyc_Port_Ptr_ct_struct*
_tmp5A7;return(void*)((_tmp5A7=_cycalloc(sizeof(*_tmp5A7)),((_tmp5A7[0]=((
_tmp5A8.tag=11,((_tmp5A8.f1=(void*)_tmpBE,((_tmp5A8.f2=(void*)_tmpB1,((_tmp5A8.f3=(
void*)_tmpB2,((_tmp5A8.f4=(void*)_tmpBF,((_tmp5A8.f5=(void*)_tmpB4,_tmp5A8)))))))))))),
_tmp5A7))));};}_LL65: {struct Cyc_Port_Array_ct_struct*_tmpB5=(struct Cyc_Port_Array_ct_struct*)
_tmp9F;if(_tmpB5->tag != 12)goto _LL67;else{_tmpB6=(void*)_tmpB5->f1;_tmpB7=(void*)
_tmpB5->f2;_tmpB8=(void*)_tmpB5->f3;}}_LL66: {void*_tmpC2=Cyc_Port_inst(instenv,
_tmpB6);if(_tmpC2 == _tmpB6)return t;{struct Cyc_Port_Array_ct_struct _tmp5AB;struct
Cyc_Port_Array_ct_struct*_tmp5AA;return(void*)((_tmp5AA=_cycalloc(sizeof(*
_tmp5AA)),((_tmp5AA[0]=((_tmp5AB.tag=12,((_tmp5AB.f1=(void*)_tmpC2,((_tmp5AB.f2=(
void*)_tmpB7,((_tmp5AB.f3=(void*)_tmpB8,_tmp5AB)))))))),_tmp5AA))));};}_LL67: {
struct Cyc_Port_Fn_ct_struct*_tmpB9=(struct Cyc_Port_Fn_ct_struct*)_tmp9F;if(
_tmpB9->tag != 15)goto _LL46;else{_tmpBA=(void*)_tmpB9->f1;_tmpBB=_tmpB9->f2;}}
_LL68: {struct Cyc_Port_Fn_ct_struct _tmp5AE;struct Cyc_Port_Fn_ct_struct*_tmp5AD;
return(void*)((_tmp5AD=_cycalloc(sizeof(*_tmp5AD)),((_tmp5AD[0]=((_tmp5AE.tag=15,((
_tmp5AE.f1=(void*)Cyc_Port_inst(instenv,_tmpBA),((_tmp5AE.f2=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Port_inst,instenv,_tmpBB),_tmp5AE)))))),_tmp5AD))));}
_LL46:;}void*Cyc_Port_instantiate(void*t){struct Cyc_Dict_Dict*_tmp5AF;return Cyc_Port_inst(((
_tmp5AF=_cycalloc(sizeof(*_tmp5AF)),((_tmp5AF[0]=((struct Cyc_Dict_Dict(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),
_tmp5AF)))),t);}struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*
ts){int found=0;{struct Cyc_List_List*_tmpC8=ts;for(0;(unsigned int)_tmpC8;_tmpC8=
_tmpC8->tl){void*_tmpC9=Cyc_Port_compress_ct((void*)_tmpC8->hd);if(t == _tmpC9)
found=1;}}if(!found)return ts;{struct Cyc_List_List*res=0;for(0;ts != 0;ts=ts->tl){
if(t == Cyc_Port_compress_ct((void*)ts->hd))continue;{struct Cyc_List_List*_tmp5B0;
res=((_tmp5B0=_cycalloc(sizeof(*_tmp5B0)),((_tmp5B0->hd=(void*)ts->hd,((_tmp5B0->tl=
res,_tmp5B0))))));};}return res;};}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;void
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
void*_tmpF4;struct Cyc_List_List*_tmpF5;struct _tuple10*_tmpF7;struct _tuple10
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
t2)return;{struct _tuple9 _tmp5B1;struct _tuple9 _tmpFE=(_tmp5B1.f1=t1,((_tmp5B1.f2=
t2,_tmp5B1)));void*_tmpFF;struct Cyc_Port_Cvar*_tmp101;void*_tmp102;struct Cyc_Port_Cvar*
_tmp104;void*_tmp105;void*_tmp107;void*_tmp108;void*_tmp109;void*_tmp10A;void*
_tmp10B;void*_tmp10C;void*_tmp10E;void*_tmp10F;void*_tmp110;void*_tmp111;void*
_tmp112;void*_tmp113;struct _dyneither_ptr*_tmp115;struct _dyneither_ptr _tmp116;
void*_tmp117;struct _dyneither_ptr*_tmp119;struct _dyneither_ptr _tmp11A;void*
_tmp11B;void*_tmp11D;void*_tmp11E;void*_tmp11F;void*_tmp120;void*_tmp122;void*
_tmp123;void*_tmp124;void*_tmp125;void*_tmp127;struct Cyc_List_List*_tmp128;void*
_tmp129;void*_tmp12B;struct Cyc_List_List*_tmp12C;void*_tmp12D;struct _tuple10*
_tmp12F;void*_tmp130;struct _tuple10*_tmp132;void*_tmp133;struct Cyc_List_List*
_tmp135;void**_tmp136;void***_tmp137;void*_tmp138;struct Cyc_List_List*_tmp13A;
void**_tmp13B;void***_tmp13C;void*_tmp13D;struct Cyc_List_List*_tmp13F;void**
_tmp140;void***_tmp141;void*_tmp142;struct _tuple10*_tmp144;struct _tuple10 _tmp145;
struct Cyc_Absyn_Aggrdecl*_tmp146;struct Cyc_List_List*_tmp147;void*_tmp148;struct
_tuple10*_tmp14A;struct _tuple10 _tmp14B;struct Cyc_Absyn_Aggrdecl*_tmp14C;struct
Cyc_List_List*_tmp14D;void*_tmp14E;struct Cyc_List_List*_tmp150;void**_tmp151;
void***_tmp152;_LL9A: _tmpFF=_tmpFE.f1;{struct Cyc_Port_Var_ct_struct*_tmp100=(
struct Cyc_Port_Var_ct_struct*)_tmpFF;if(_tmp100->tag != 16)goto _LL9C;else{_tmp101=
_tmp100->f1;}};_LL9B: if(!Cyc_Port_occurs(t1,t2)){{struct Cyc_List_List*_tmp153=
Cyc_Port_filter_self(t1,_tmp101->uppers);for(0;(unsigned int)_tmp153;_tmp153=
_tmp153->tl){if(!Cyc_Port_leq(t2,(void*)_tmp153->hd))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}{
struct Cyc_List_List*_tmp154=Cyc_Port_filter_self(t1,_tmp101->lowers);for(0;(
unsigned int)_tmp154;_tmp154=_tmp154->tl){if(!Cyc_Port_leq((void*)_tmp154->hd,t2))(
int)_throw((void*)& Cyc_Port_Unify_ct_val);}}{void**_tmp5B2;_tmp101->eq=((_tmp5B2=
_cycalloc(sizeof(*_tmp5B2)),((_tmp5B2[0]=t2,_tmp5B2))));}return;}else{(int)
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
_tmp13A,1));{void**_tmp5B3;*_tmp137=(*_tmp13C=((_tmp5B3=_cycalloc(sizeof(*
_tmp5B3)),((_tmp5B3[0]=_tmp156,_tmp5B3)))));}return;}_LLAA: _tmp13D=_tmpFE.f1;{
struct Cyc_Port_UnknownAggr_ct_struct*_tmp13E=(struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp13D;if(_tmp13E->tag != 14)goto _LLAC;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;
_tmp141=(void***)& _tmp13E->f2;}};_tmp142=_tmpFE.f2;{struct Cyc_Port_KnownAggr_ct_struct*
_tmp143=(struct Cyc_Port_KnownAggr_ct_struct*)_tmp142;if(_tmp143->tag != 13)goto
_LLAC;else{_tmp144=_tmp143->f1;_tmp145=*_tmp144;_tmp146=_tmp145.f1;_tmp147=
_tmp145.f2;}};_LLAB: Cyc_Port_merge_fields(_tmp147,_tmp13F,0);{void**_tmp5B4;*
_tmp141=((_tmp5B4=_cycalloc(sizeof(*_tmp5B4)),((_tmp5B4[0]=t2,_tmp5B4))));}
return;_LLAC: _tmp148=_tmpFE.f1;{struct Cyc_Port_KnownAggr_ct_struct*_tmp149=(
struct Cyc_Port_KnownAggr_ct_struct*)_tmp148;if(_tmp149->tag != 13)goto _LLAE;else{
_tmp14A=_tmp149->f1;_tmp14B=*_tmp14A;_tmp14C=_tmp14B.f1;_tmp14D=_tmp14B.f2;}};
_tmp14E=_tmpFE.f2;{struct Cyc_Port_UnknownAggr_ct_struct*_tmp14F=(struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp14E;if(_tmp14F->tag != 14)goto _LLAE;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;
_tmp152=(void***)& _tmp14F->f2;}};_LLAD: Cyc_Port_merge_fields(_tmp14D,_tmp150,0);{
void**_tmp5B5;*_tmp152=((_tmp5B5=_cycalloc(sizeof(*_tmp5B5)),((_tmp5B5[0]=t1,
_tmp5B5))));}return;_LLAE:;_LLAF:(int)_throw((void*)& Cyc_Port_Unify_ct_val);
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
== 0){{struct Cyc_List_List*_tmp5B6;common=((_tmp5B6=_cycalloc(sizeof(*_tmp5B6)),((
_tmp5B6->hd=_tmp15D,((_tmp5B6->tl=common,_tmp5B6))))));}Cyc_Port_unify_ct(
_tmp15D->qual,_tmp15B->qual);Cyc_Port_unify_ct(_tmp15D->type,_tmp15B->type);
found=1;break;}}}if(!found){if(allow_f1_subset_f2){struct Cyc_List_List*_tmp5B7;
common=((_tmp5B7=_cycalloc(sizeof(*_tmp5B7)),((_tmp5B7->hd=_tmp15B,((_tmp5B7->tl=
common,_tmp5B7))))));}else{(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}}{
struct Cyc_List_List*_tmp160=fs1;for(0;(unsigned int)_tmp160;_tmp160=_tmp160->tl){
struct Cyc_Port_Cfield*_tmp161=(struct Cyc_Port_Cfield*)_tmp160->hd;int found=0;{
struct Cyc_List_List*_tmp162=fs2;for(0;(unsigned int)_tmp162;_tmp162=_tmp162->tl){
struct Cyc_Port_Cfield*_tmp163=(struct Cyc_Port_Cfield*)_tmp162->hd;if(Cyc_strptrcmp(
_tmp161->f,_tmp163->f))found=1;}}if(!found){struct Cyc_List_List*_tmp5B8;common=((
_tmp5B8=_cycalloc(sizeof(*_tmp5B8)),((_tmp5B8->hd=_tmp161,((_tmp5B8->tl=common,
_tmp5B8))))));}}}return common;}static int Cyc_Port_unifies(void*t1,void*t2){{
struct _handler_cons _tmp165;_push_handler(& _tmp165);{int _tmp167=0;if(setjmp(
_tmp165.handler))_tmp167=1;if(!_tmp167){Cyc_Port_unify_ct(t1,t2);;_pop_handler();}
else{void*_tmp166=(void*)_exn_thrown;void*_tmp169=_tmp166;_LLB1: {struct Cyc_Port_Unify_ct_struct*
_tmp16A=(struct Cyc_Port_Unify_ct_struct*)_tmp169;if(_tmp16A->tag != Cyc_Port_Unify_ct)
goto _LLB3;}_LLB2: return 0;_LLB3:;_LLB4:(void)_throw(_tmp169);_LLB0:;}};}return 1;}
struct _tuple11{void*f1;void*f2;void*f3;void*f4;void*f5;};static struct Cyc_List_List*
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
== _tmp179)return*uppers;{struct _tuple9 _tmp5B9;struct _tuple9 _tmp17B=(_tmp5B9.f1=
t,((_tmp5B9.f2=_tmp179,_tmp5B9)));void*_tmp17C;void*_tmp17E;void*_tmp180;void*
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
void*_tmp199;void*_tmp19A;void*_tmp19B;void*_tmp19C;struct _tuple11 _tmp5BA;struct
_tuple11 _tmp197=(_tmp5BA.f1=Cyc_Port_var(),((_tmp5BA.f2=Cyc_Port_var(),((_tmp5BA.f3=
Cyc_Port_var(),((_tmp5BA.f4=Cyc_Port_var(),((_tmp5BA.f5=Cyc_Port_var(),_tmp5BA)))))))));
_tmp198=_tmp197.f1;_tmp199=_tmp197.f2;_tmp19A=_tmp197.f3;_tmp19B=_tmp197.f4;
_tmp19C=_tmp197.f5;{struct Cyc_Port_Ptr_ct_struct _tmp5BD;struct Cyc_Port_Ptr_ct_struct*
_tmp5BC;struct Cyc_Port_Ptr_ct_struct*_tmp19D=(_tmp5BC=_cycalloc(sizeof(*_tmp5BC)),((
_tmp5BC[0]=((_tmp5BD.tag=11,((_tmp5BD.f1=(void*)_tmp198,((_tmp5BD.f2=(void*)
_tmp199,((_tmp5BD.f3=(void*)_tmp19A,((_tmp5BD.f4=(void*)_tmp19B,((_tmp5BD.f5=(
void*)_tmp19C,_tmp5BD)))))))))))),_tmp5BC)));Cyc_Port_leq(_tmp198,_tmp18A);Cyc_Port_leq(
_tmp198,_tmp191);Cyc_Port_leq(_tmp199,_tmp18B);Cyc_Port_leq(_tmp199,_tmp192);Cyc_Port_leq(
_tmp19A,_tmp18C);Cyc_Port_leq(_tmp19A,_tmp192);Cyc_Port_leq(_tmp19B,_tmp18D);Cyc_Port_leq(
_tmp19B,_tmp194);Cyc_Port_leq(_tmp19C,_tmp18E);Cyc_Port_leq(_tmp19C,_tmp195);
_tmp178->hd=(void*)((void*)_tmp19D);return*uppers;};}_LLD9:;_LLDA: goto _LLD0;
_LLD0:;};}}{struct Cyc_List_List*_tmp5BE;return(_tmp5BE=_cycalloc(sizeof(*_tmp5BE)),((
_tmp5BE->hd=t,((_tmp5BE->tl=*uppers,_tmp5BE)))));};}static struct Cyc_List_List*
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
void*)_tmp1AD->hd);if(t == _tmp1AE)return*lowers;{struct _tuple9 _tmp5BF;struct
_tuple9 _tmp1B0=(_tmp5BF.f1=t,((_tmp5BF.f2=_tmp1AE,_tmp5BF)));void*_tmp1B1;void*
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
_tmp1D6;void*_tmp1D7;struct _tuple11 _tmp5C0;struct _tuple11 _tmp1D2=(_tmp5C0.f1=Cyc_Port_var(),((
_tmp5C0.f2=Cyc_Port_var(),((_tmp5C0.f3=Cyc_Port_var(),((_tmp5C0.f4=Cyc_Port_var(),((
_tmp5C0.f5=Cyc_Port_var(),_tmp5C0)))))))));_tmp1D3=_tmp1D2.f1;_tmp1D4=_tmp1D2.f2;
_tmp1D5=_tmp1D2.f3;_tmp1D6=_tmp1D2.f4;_tmp1D7=_tmp1D2.f5;{struct Cyc_Port_Ptr_ct_struct
_tmp5C3;struct Cyc_Port_Ptr_ct_struct*_tmp5C2;struct Cyc_Port_Ptr_ct_struct*
_tmp1D8=(_tmp5C2=_cycalloc(sizeof(*_tmp5C2)),((_tmp5C2[0]=((_tmp5C3.tag=11,((
_tmp5C3.f1=(void*)_tmp1D3,((_tmp5C3.f2=(void*)_tmp1D4,((_tmp5C3.f3=(void*)
_tmp1D5,((_tmp5C3.f4=(void*)_tmp1D6,((_tmp5C3.f5=(void*)_tmp1D7,_tmp5C3)))))))))))),
_tmp5C2)));Cyc_Port_leq(_tmp1C5,_tmp1D3);Cyc_Port_leq(_tmp1CC,_tmp1D3);Cyc_Port_leq(
_tmp1C6,_tmp1D4);Cyc_Port_leq(_tmp1CD,_tmp1D4);Cyc_Port_leq(_tmp1C7,_tmp1D5);Cyc_Port_leq(
_tmp1CD,_tmp1D5);Cyc_Port_leq(_tmp1C8,_tmp1D6);Cyc_Port_leq(_tmp1CF,_tmp1D6);Cyc_Port_leq(
_tmp1C9,_tmp1D7);Cyc_Port_leq(_tmp1D0,_tmp1D7);_tmp1AD->hd=(void*)((void*)
_tmp1D8);return*lowers;};}_LL101:;_LL102: goto _LLF4;_LLF4:;};}}{struct Cyc_List_List*
_tmp5C4;return(_tmp5C4=_cycalloc(sizeof(*_tmp5C4)),((_tmp5C4->hd=t,((_tmp5C4->tl=*
lowers,_tmp5C4)))));};}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
struct Cyc_Port_Cvar;static int Cyc_Port_leq(void*t1,void*t2){if(t1 == t2)return 1;t1=
Cyc_Port_compress_ct(t1);t2=Cyc_Port_compress_ct(t2);{struct _tuple9 _tmp5C5;
struct _tuple9 _tmp1DD=(_tmp5C5.f1=t1,((_tmp5C5.f2=t2,_tmp5C5)));void*_tmp1DE;void*
_tmp1E0;struct _dyneither_ptr*_tmp1E2;struct _dyneither_ptr _tmp1E3;void*_tmp1E4;
struct _dyneither_ptr*_tmp1E6;struct _dyneither_ptr _tmp1E7;void*_tmp1E8;struct
_dyneither_ptr*_tmp1EA;struct _dyneither_ptr _tmp1EB;void*_tmp1EC;void*_tmp1EE;
void*_tmp1F0;void*_tmp1F2;void*_tmp1F4;void*_tmp1F6;void*_tmp1F8;void*_tmp1FA;
void*_tmp1FC;void*_tmp1FE;void*_tmp200;void*_tmp202;void*_tmp204;void*_tmp206;
void*_tmp208;void*_tmp20A;void*_tmp20C;void*_tmp20E;void*_tmp210;void*_tmp212;
void*_tmp214;void*_tmp216;void*_tmp218;void*_tmp21A;void*_tmp21C;void*_tmp21E;
void*_tmp220;void*_tmp222;void*_tmp224;void*_tmp226;void*_tmp227;void*_tmp228;
void*_tmp229;void*_tmp22A;void*_tmp22B;void*_tmp22D;void*_tmp22E;void*_tmp22F;
void*_tmp230;void*_tmp231;void*_tmp232;void*_tmp234;void*_tmp235;void*_tmp236;
void*_tmp237;void*_tmp239;void*_tmp23A;void*_tmp23B;void*_tmp23C;void*_tmp23E;
void*_tmp23F;void*_tmp240;void*_tmp241;void*_tmp243;void*_tmp244;void*_tmp245;
void*_tmp246;void*_tmp247;struct Cyc_Port_Cvar*_tmp249;void*_tmp24A;struct Cyc_Port_Cvar*
_tmp24C;void*_tmp24D;struct Cyc_Port_Cvar*_tmp24F;void*_tmp250;struct Cyc_Port_Cvar*
_tmp252;_LL104: _tmp1DE=_tmp1DD.f1;{struct Cyc_Port_Heap_ct_struct*_tmp1DF=(struct
Cyc_Port_Heap_ct_struct*)_tmp1DE;if(_tmp1DF->tag != 7)goto _LL106;};_LL105: return 1;
_LL106: _tmp1E0=_tmp1DD.f1;{struct Cyc_Port_RgnVar_ct_struct*_tmp1E1=(struct Cyc_Port_RgnVar_ct_struct*)
_tmp1E0;if(_tmp1E1->tag != 10)goto _LL108;else{_tmp1E2=_tmp1E1->f1;_tmp1E3=*
_tmp1E2;}};_tmp1E4=_tmp1DD.f2;{struct Cyc_Port_RgnVar_ct_struct*_tmp1E5=(struct
Cyc_Port_RgnVar_ct_struct*)_tmp1E4;if(_tmp1E5->tag != 10)goto _LL108;else{_tmp1E6=
_tmp1E5->f1;_tmp1E7=*_tmp1E6;}};_LL107: return Cyc_strcmp((struct _dyneither_ptr)
_tmp1E3,(struct _dyneither_ptr)_tmp1E7)== 0;_LL108: _tmp1E8=_tmp1DD.f1;{struct Cyc_Port_RgnVar_ct_struct*
_tmp1E9=(struct Cyc_Port_RgnVar_ct_struct*)_tmp1E8;if(_tmp1E9->tag != 10)goto
_LL10A;else{_tmp1EA=_tmp1E9->f1;_tmp1EB=*_tmp1EA;}};_tmp1EC=_tmp1DD.f2;{struct
Cyc_Port_Heap_ct_struct*_tmp1ED=(struct Cyc_Port_Heap_ct_struct*)_tmp1EC;if(
_tmp1ED->tag != 7)goto _LL10A;};_LL109: return 0;_LL10A: _tmp1EE=_tmp1DD.f1;{struct
Cyc_Port_Notconst_ct_struct*_tmp1EF=(struct Cyc_Port_Notconst_ct_struct*)_tmp1EE;
if(_tmp1EF->tag != 1)goto _LL10C;};_tmp1F0=_tmp1DD.f2;{struct Cyc_Port_Const_ct_struct*
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
Cyc_Port_empty_cenv(){struct Cyc_Port_GlobalCenv*_tmp5C6;struct Cyc_Port_GlobalCenv*
g=(_tmp5C6=_cycalloc(sizeof(*_tmp5C6)),((_tmp5C6->typedef_dict=((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((
_tmp5C6->struct_dict=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((_tmp5C6->union_dict=((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((
_tmp5C6->qualifier_edits=0,((_tmp5C6->pointer_edits=0,((_tmp5C6->zeroterm_edits=
0,((_tmp5C6->edits=0,((_tmp5C6->porting=0,((_tmp5C6->return_type=Cyc_Port_void_ct(),
_tmp5C6)))))))))))))))))));struct Cyc_Port_Cenv*_tmp5C7;return(_tmp5C7=_cycalloc(
sizeof(*_tmp5C7)),((_tmp5C7->gcenv=g,((_tmp5C7->cpos=Cyc_Port_Toplevel_pos,((
_tmp5C7->var_dict=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),_tmp5C7)))))));}struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static int Cyc_Port_in_fn_arg(struct Cyc_Port_Cenv*env){return
env->cpos == Cyc_Port_FnArg_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static
int Cyc_Port_in_fn_res(struct Cyc_Port_Cenv*env){return env->cpos == Cyc_Port_FnRes_pos;}
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*
env){return env->cpos == Cyc_Port_Toplevel_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
static void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env){return(env->gcenv)->return_type;}
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static void*Cyc_Port_lookup_typedef(
struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _handler_cons _tmp255;
_push_handler(& _tmp255);{int _tmp257=0;if(setjmp(_tmp255.handler))_tmp257=1;if(!
_tmp257){{void*_tmp259;struct _tuple9 _tmp258=*((struct _tuple9*(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);_tmp259=
_tmp258.f1;{void*_tmp25A=_tmp259;_npop_handler(0);return _tmp25A;};};_pop_handler();}
else{void*_tmp256=(void*)_exn_thrown;void*_tmp25C=_tmp256;_LL135: {struct Cyc_Dict_Absent_struct*
_tmp25D=(struct Cyc_Dict_Absent_struct*)_tmp25C;if(_tmp25D->tag != Cyc_Dict_Absent)
goto _LL137;}_LL136: return Cyc_Absyn_sint_typ;_LL137:;_LL138:(void)_throw(_tmp25C);
_LL134:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static void*Cyc_Port_lookup_typedef_ctype(
struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _handler_cons _tmp25E;
_push_handler(& _tmp25E);{int _tmp260=0;if(setjmp(_tmp25E.handler))_tmp260=1;if(!
_tmp260){{void*_tmp262;struct _tuple9 _tmp261=*((struct _tuple9*(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);_tmp262=
_tmp261.f2;{void*_tmp263=_tmp262;_npop_handler(0);return _tmp263;};};_pop_handler();}
else{void*_tmp25F=(void*)_exn_thrown;void*_tmp265=_tmp25F;_LL13A: {struct Cyc_Dict_Absent_struct*
_tmp266=(struct Cyc_Dict_Absent_struct*)_tmp265;if(_tmp266->tag != Cyc_Dict_Absent)
goto _LL13C;}_LL13B: return Cyc_Port_var();_LL13C:;_LL13D:(void)_throw(_tmp265);
_LL139:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static struct _tuple10*Cyc_Port_lookup_struct_decl(
struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _tuple10**_tmp267=((struct
_tuple10**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->struct_dict,
n);if((unsigned int)_tmp267)return*_tmp267;else{struct Cyc_Absyn_Aggrdecl*_tmp5C8;
struct Cyc_Absyn_Aggrdecl*_tmp268=(_tmp5C8=_cycalloc(sizeof(*_tmp5C8)),((_tmp5C8->kind=
Cyc_Absyn_StructA,((_tmp5C8->sc=Cyc_Absyn_Public,((_tmp5C8->name=n,((_tmp5C8->tvs=
0,((_tmp5C8->impl=0,((_tmp5C8->attributes=0,_tmp5C8)))))))))))));struct _tuple10*
_tmp5C9;struct _tuple10*p=(_tmp5C9=_cycalloc(sizeof(*_tmp5C9)),((_tmp5C9->f1=
_tmp268,((_tmp5C9->f2=0,_tmp5C9)))));(env->gcenv)->struct_dict=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple10*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,
n,p);return p;}}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static struct _tuple10*Cyc_Port_lookup_union_decl(
struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _tuple10**_tmp26B=((struct
_tuple10**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->union_dict,
n);if((unsigned int)_tmp26B)return*_tmp26B;else{struct Cyc_Absyn_Aggrdecl*_tmp5CA;
struct Cyc_Absyn_Aggrdecl*_tmp26C=(_tmp5CA=_cycalloc(sizeof(*_tmp5CA)),((_tmp5CA->kind=
Cyc_Absyn_UnionA,((_tmp5CA->sc=Cyc_Absyn_Public,((_tmp5CA->name=n,((_tmp5CA->tvs=
0,((_tmp5CA->impl=0,((_tmp5CA->attributes=0,_tmp5CA)))))))))))));struct _tuple10*
_tmp5CB;struct _tuple10*p=(_tmp5CB=_cycalloc(sizeof(*_tmp5CB)),((_tmp5CB->f1=
_tmp26C,((_tmp5CB->f2=0,_tmp5CB)))));(env->gcenv)->union_dict=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple10*v))Cyc_Dict_insert)((env->gcenv)->union_dict,
n,p);return p;}}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple12{void*f1;
struct _tuple9*f2;};static struct _tuple9 Cyc_Port_lookup_var(struct Cyc_Port_Cenv*
env,struct _tuple0*x){struct _handler_cons _tmp26F;_push_handler(& _tmp26F);{int
_tmp271=0;if(setjmp(_tmp26F.handler))_tmp271=1;if(!_tmp271){{struct _tuple9*
_tmp273;struct _tuple12 _tmp272=*((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp273=_tmp272.f2;{struct _tuple9
_tmp274=*_tmp273;_npop_handler(0);return _tmp274;};};_pop_handler();}else{void*
_tmp270=(void*)_exn_thrown;void*_tmp276=_tmp270;_LL13F: {struct Cyc_Dict_Absent_struct*
_tmp277=(struct Cyc_Dict_Absent_struct*)_tmp276;if(_tmp277->tag != Cyc_Dict_Absent)
goto _LL141;}_LL140: {struct _tuple9 _tmp5CC;return(_tmp5CC.f1=Cyc_Port_var(),((
_tmp5CC.f2=Cyc_Port_var(),_tmp5CC)));}_LL141:;_LL142:(void)_throw(_tmp276);
_LL13E:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static struct _tuple12*Cyc_Port_lookup_full_var(
struct Cyc_Port_Cenv*env,struct _tuple0*x){return((struct _tuple12*(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);}struct Cyc_Port_Cenv;struct
Cyc_Port_Cenv;static int Cyc_Port_declared_var(struct Cyc_Port_Cenv*env,struct
_tuple0*x){return((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)(
env->var_dict,x);}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static int Cyc_Port_isfn(
struct Cyc_Port_Cenv*env,struct _tuple0*x){struct _handler_cons _tmp279;
_push_handler(& _tmp279);{int _tmp27B=0;if(setjmp(_tmp279.handler))_tmp27B=1;if(!
_tmp27B){{void*_tmp27D;struct _tuple12 _tmp27C=*((struct _tuple12*(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp27D=_tmp27C.f1;LOOP: {
void*_tmp27E=_tmp27D;struct _tuple0*_tmp280;_LL144: {struct Cyc_Absyn_TypedefType_struct*
_tmp27F=(struct Cyc_Absyn_TypedefType_struct*)_tmp27E;if(_tmp27F->tag != 18)goto
_LL146;else{_tmp280=_tmp27F->f1;}}_LL145: _tmp27D=Cyc_Port_lookup_typedef(env,
_tmp280);goto LOOP;_LL146: {struct Cyc_Absyn_FnType_struct*_tmp281=(struct Cyc_Absyn_FnType_struct*)
_tmp27E;if(_tmp281->tag != 10)goto _LL148;}_LL147: {int _tmp282=1;_npop_handler(0);
return _tmp282;}_LL148:;_LL149: {int _tmp283=0;_npop_handler(0);return _tmp283;}
_LL143:;}};_pop_handler();}else{void*_tmp27A=(void*)_exn_thrown;void*_tmp285=
_tmp27A;_LL14B: {struct Cyc_Dict_Absent_struct*_tmp286=(struct Cyc_Dict_Absent_struct*)
_tmp285;if(_tmp286->tag != Cyc_Dict_Absent)goto _LL14D;}_LL14C: return 0;_LL14D:;
_LL14E:(void)_throw(_tmp285);_LL14A:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
static int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x){struct
_handler_cons _tmp287;_push_handler(& _tmp287);{int _tmp289=0;if(setjmp(_tmp287.handler))
_tmp289=1;if(!_tmp289){{void*_tmp28B;struct _tuple12 _tmp28A=*((struct _tuple12*(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp28B=
_tmp28A.f1;LOOP: {void*_tmp28C=_tmp28B;struct _tuple0*_tmp28E;_LL150: {struct Cyc_Absyn_TypedefType_struct*
_tmp28D=(struct Cyc_Absyn_TypedefType_struct*)_tmp28C;if(_tmp28D->tag != 18)goto
_LL152;else{_tmp28E=_tmp28D->f1;}}_LL151: _tmp28B=Cyc_Port_lookup_typedef(env,
_tmp28E);goto LOOP;_LL152: {struct Cyc_Absyn_ArrayType_struct*_tmp28F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp28C;if(_tmp28F->tag != 9)goto _LL154;}_LL153: {int _tmp290=1;_npop_handler(0);
return _tmp290;}_LL154:;_LL155: {int _tmp291=0;_npop_handler(0);return _tmp291;}
_LL14F:;}};_pop_handler();}else{void*_tmp288=(void*)_exn_thrown;void*_tmp293=
_tmp288;_LL157: {struct Cyc_Dict_Absent_struct*_tmp294=(struct Cyc_Dict_Absent_struct*)
_tmp293;if(_tmp294->tag != Cyc_Dict_Absent)goto _LL159;}_LL158: return 0;_LL159:;
_LL15A:(void)_throw(_tmp293);_LL156:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*Cyc_Port_set_cpos(struct Cyc_Port_Cenv*
env,enum Cyc_Port_CPos cpos){struct Cyc_Port_Cenv*_tmp5CD;return(_tmp5CD=_cycalloc(
sizeof(*_tmp5CD)),((_tmp5CD->gcenv=env->gcenv,((_tmp5CD->var_dict=env->var_dict,((
_tmp5CD->cpos=cpos,_tmp5CD)))))));}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
static void Cyc_Port_add_return_type(struct Cyc_Port_Cenv*env,void*t){(env->gcenv)->return_type=
t;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*
Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct _tuple0*x,void*t,void*qual,void*
ctype){struct _tuple12*_tmp5D3;struct _tuple9*_tmp5D2;struct Cyc_Port_Cenv*_tmp5D1;
return(_tmp5D1=_cycalloc(sizeof(*_tmp5D1)),((_tmp5D1->gcenv=env->gcenv,((_tmp5D1->var_dict=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple12*v))
Cyc_Dict_insert)(env->var_dict,x,((_tmp5D3=_cycalloc(sizeof(*_tmp5D3)),((_tmp5D3->f1=
t,((_tmp5D3->f2=((_tmp5D2=_cycalloc(sizeof(*_tmp5D2)),((_tmp5D2->f1=qual,((
_tmp5D2->f2=ctype,_tmp5D2)))))),_tmp5D3))))))),((_tmp5D1->cpos=env->cpos,_tmp5D1)))))));}
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*
env,struct _tuple0*n,void*t,void*ct){struct _tuple9*_tmp5D4;(env->gcenv)->typedef_dict=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple9*v))Cyc_Dict_insert)((
env->gcenv)->typedef_dict,n,((_tmp5D4=_cycalloc(sizeof(*_tmp5D4)),((_tmp5D4->f1=
t,((_tmp5D4->f2=ct,_tmp5D4)))))));}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct _tuple13{void*f1;void*f2;struct Cyc_Position_Segment*f3;};static void Cyc_Port_register_const_cvar(
struct Cyc_Port_Cenv*env,void*new_qual,void*orig_qual,struct Cyc_Position_Segment*
loc){struct _tuple13*_tmp5D7;struct Cyc_List_List*_tmp5D6;(env->gcenv)->qualifier_edits=((
_tmp5D6=_cycalloc(sizeof(*_tmp5D6)),((_tmp5D6->hd=((_tmp5D7=_cycalloc(sizeof(*
_tmp5D7)),((_tmp5D7->f1=new_qual,((_tmp5D7->f2=orig_qual,((_tmp5D7->f3=loc,
_tmp5D7)))))))),((_tmp5D6->tl=(env->gcenv)->qualifier_edits,_tmp5D6))))));}
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static void Cyc_Port_register_ptr_cvars(
struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,struct Cyc_Position_Segment*
loc){struct _tuple13*_tmp5DA;struct Cyc_List_List*_tmp5D9;(env->gcenv)->pointer_edits=((
_tmp5D9=_cycalloc(sizeof(*_tmp5D9)),((_tmp5D9->hd=((_tmp5DA=_cycalloc(sizeof(*
_tmp5DA)),((_tmp5DA->f1=new_ptr,((_tmp5DA->f2=orig_ptr,((_tmp5DA->f3=loc,_tmp5DA)))))))),((
_tmp5D9->tl=(env->gcenv)->pointer_edits,_tmp5D9))))));}struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*
env,void*new_zt,void*orig_zt,struct Cyc_Position_Segment*loc){struct _tuple13*
_tmp5DD;struct Cyc_List_List*_tmp5DC;(env->gcenv)->zeroterm_edits=((_tmp5DC=
_cycalloc(sizeof(*_tmp5DC)),((_tmp5DC->hd=((_tmp5DD=_cycalloc(sizeof(*_tmp5DD)),((
_tmp5DD->f1=new_zt,((_tmp5DD->f2=orig_zt,((_tmp5DD->f3=loc,_tmp5DD)))))))),((
_tmp5DC->tl=(env->gcenv)->zeroterm_edits,_tmp5DC))))));}static void*Cyc_Port_main_type(){
struct _tuple7*_tmp5DE;struct _tuple7*arg1=(_tmp5DE=_cycalloc(sizeof(*_tmp5DE)),((
_tmp5DE->f1=0,((_tmp5DE->f2=Cyc_Absyn_empty_tqual(0),((_tmp5DE->f3=Cyc_Absyn_sint_typ,
_tmp5DE)))))));struct _tuple7*_tmp5DF;struct _tuple7*arg2=(_tmp5DF=_cycalloc(
sizeof(*_tmp5DF)),((_tmp5DF->f1=0,((_tmp5DF->f2=Cyc_Absyn_empty_tqual(0),((
_tmp5DF->f3=Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_wildtyp(0)),
Cyc_Absyn_wildtyp(0),Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)()),_tmp5DF)))))));struct Cyc_Absyn_FnType_struct _tmp5E9;
struct _tuple7*_tmp5E8[2];struct Cyc_Absyn_FnInfo _tmp5E7;struct Cyc_Absyn_FnType_struct*
_tmp5E6;return(void*)((_tmp5E6=_cycalloc(sizeof(*_tmp5E6)),((_tmp5E6[0]=((
_tmp5E9.tag=10,((_tmp5E9.f1=((_tmp5E7.tvars=0,((_tmp5E7.effect=0,((_tmp5E7.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmp5E7.ret_typ=Cyc_Absyn_sint_typ,((_tmp5E7.args=((
_tmp5E8[1]=arg2,((_tmp5E8[0]=arg1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp5E8,sizeof(struct _tuple7*),2)))))),((_tmp5E7.c_varargs=
0,((_tmp5E7.cyc_varargs=0,((_tmp5E7.rgn_po=0,((_tmp5E7.attributes=0,_tmp5E7)))))))))))))))))),
_tmp5E9)))),_tmp5E6))));}struct Cyc_Port_Cenv;static void*Cyc_Port_type_to_ctype(
struct Cyc_Port_Cenv*env,void*t);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct
Cyc_Port_Cenv;static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv(){struct Cyc_Port_Cenv*
_tmp2A6=Cyc_Port_empty_cenv();{struct _tuple0*_tmp5EA;_tmp2A6=Cyc_Port_add_var(
_tmp2A6,((_tmp5EA=_cycalloc(sizeof(*_tmp5EA)),((_tmp5EA->f1=Cyc_Absyn_Loc_n,((
_tmp5EA->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"main",
sizeof(char),5),_tmp5EA)))))),Cyc_Port_main_type(),Cyc_Port_const_ct(),Cyc_Port_type_to_ctype(
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
_tmp2B8)){int*_tmp5EB;*cts=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,int*v))Cyc_Dict_insert)(*cts,_tmp2B8,(int*)((_tmp5EB=
_cycalloc_atomic(sizeof(*_tmp5EB)),((_tmp5EB[0]=0,_tmp5EB)))));}{int*_tmp2C7=((
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
struct _tuple9 _tmp5EC;struct _tuple9 _tmp2C9=(_tmp5EC.f1=t,((_tmp5EC.f2=c,_tmp5EC)));
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
struct _tuple9 _tmp5ED;struct _tuple9 _tmp2E7=(_tmp5ED.f1=_tmp2CF,((_tmp5ED.f2=
_tmp2D4,_tmp5ED)));void*_tmp2E8;void*_tmp2EA;void*_tmp2EC;void*_tmp2EE;struct
_dyneither_ptr*_tmp2F0;_LL188: _tmp2E8=_tmp2E7.f1;{struct Cyc_Absyn_Evar_struct*
_tmp2E9=(struct Cyc_Absyn_Evar_struct*)_tmp2E8;if(_tmp2E9->tag != 1)goto _LL18A;};
_tmp2EA=_tmp2E7.f2;{struct Cyc_Port_Heap_ct_struct*_tmp2EB=(struct Cyc_Port_Heap_ct_struct*)
_tmp2EA;if(_tmp2EB->tag != 7)goto _LL18A;};if(!(!fn_res))goto _LL18A;_LL189:{struct
Cyc_Port_Edit*_tmp5F6;const char*_tmp5F5;const char*_tmp5F4;struct Cyc_List_List*
_tmp5F3;(env->gcenv)->edits=((_tmp5F3=_cycalloc(sizeof(*_tmp5F3)),((_tmp5F3->hd=((
_tmp5F6=_cycalloc(sizeof(*_tmp5F6)),((_tmp5F6->loc=_tmp2E5,((_tmp5F6->old_string=((
_tmp5F5="`H ",_tag_dyneither(_tmp5F5,sizeof(char),4))),((_tmp5F6->new_string=((
_tmp5F4="",_tag_dyneither(_tmp5F4,sizeof(char),1))),_tmp5F6)))))))),((_tmp5F3->tl=(
env->gcenv)->edits,_tmp5F3))))));}goto _LL187;_LL18A: _tmp2EC=_tmp2E7.f1;{struct
Cyc_Absyn_Evar_struct*_tmp2ED=(struct Cyc_Absyn_Evar_struct*)_tmp2EC;if(_tmp2ED->tag
!= 1)goto _LL18C;};_tmp2EE=_tmp2E7.f2;{struct Cyc_Port_RgnVar_ct_struct*_tmp2EF=(
struct Cyc_Port_RgnVar_ct_struct*)_tmp2EE;if(_tmp2EF->tag != 10)goto _LL18C;else{
_tmp2F0=_tmp2EF->f1;}};_LL18B: {int _tmp2F5=*((int*(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_lookup)(counts,_tmp2F0);if(_tmp2F5 > 1){struct
Cyc_Port_Edit*_tmp606;const char*_tmp605;void*_tmp604[1];struct Cyc_String_pa_struct
_tmp603;const char*_tmp602;struct Cyc_List_List*_tmp601;(env->gcenv)->edits=((
_tmp601=_cycalloc(sizeof(*_tmp601)),((_tmp601->hd=((_tmp606=_cycalloc(sizeof(*
_tmp606)),((_tmp606->loc=_tmp2E5,((_tmp606->old_string=(struct _dyneither_ptr)((
_tmp603.tag=0,((_tmp603.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2F0),((
_tmp604[0]=& _tmp603,Cyc_aprintf(((_tmp605="%s ",_tag_dyneither(_tmp605,sizeof(
char),4))),_tag_dyneither(_tmp604,sizeof(void*),1)))))))),((_tmp606->new_string=((
_tmp602="",_tag_dyneither(_tmp602,sizeof(char),1))),_tmp606)))))))),((_tmp601->tl=(
env->gcenv)->edits,_tmp601))))));}goto _LL187;}_LL18C:;_LL18D: goto _LL187;_LL187:;}
goto _LL17E;};_LL181: _tmp2D5=_tmp2C9.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp2D6=(
struct Cyc_Absyn_ArrayType_struct*)_tmp2D5;if(_tmp2D6->tag != 9)goto _LL183;else{
_tmp2D7=_tmp2D6->f1;_tmp2D8=_tmp2D7.elt_type;}};_tmp2D9=_tmp2C9.f2;{struct Cyc_Port_Array_ct_struct*
_tmp2DA=(struct Cyc_Port_Array_ct_struct*)_tmp2D9;if(_tmp2DA->tag != 12)goto _LL183;
else{_tmp2DB=(void*)_tmp2DA->f1;}};_LL182: Cyc_Port_register_rgns(env,counts,
fn_res,_tmp2D8,_tmp2DB);goto _LL17E;_LL183: _tmp2DC=_tmp2C9.f1;{struct Cyc_Absyn_FnType_struct*
_tmp2DD=(struct Cyc_Absyn_FnType_struct*)_tmp2DC;if(_tmp2DD->tag != 10)goto _LL185;
else{_tmp2DE=_tmp2DD->f1;_tmp2DF=_tmp2DE.ret_typ;_tmp2E0=_tmp2DE.args;}};_tmp2E1=
_tmp2C9.f2;{struct Cyc_Port_Fn_ct_struct*_tmp2E2=(struct Cyc_Port_Fn_ct_struct*)
_tmp2E1;if(_tmp2E2->tag != 15)goto _LL185;else{_tmp2E3=(void*)_tmp2E2->f1;_tmp2E4=
_tmp2E2->f2;}};_LL184: Cyc_Port_register_rgns(env,counts,1,_tmp2DF,_tmp2E3);for(0;
_tmp2E0 != 0  && _tmp2E4 != 0;(_tmp2E0=_tmp2E0->tl,_tmp2E4=_tmp2E4->tl)){void*
_tmp2FD;struct _tuple7 _tmp2FC=*((struct _tuple7*)_tmp2E0->hd);_tmp2FD=_tmp2FC.f3;{
void*_tmp2FE=(void*)_tmp2E4->hd;Cyc_Port_register_rgns(env,counts,0,_tmp2FD,
_tmp2FE);};}goto _LL17E;_LL185:;_LL186: goto _LL17E;_LL17E:;};}struct Cyc_Port_Cenv;
static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e);
struct Cyc_Port_Cenv;static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct
Cyc_Absyn_Stmt*s);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*
Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d);struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t){
void*_tmp2FF=t;struct _tuple0*_tmp301;enum Cyc_Absyn_Size_of _tmp303;_LL18F: {
struct Cyc_Absyn_TypedefType_struct*_tmp300=(struct Cyc_Absyn_TypedefType_struct*)
_tmp2FF;if(_tmp300->tag != 18)goto _LL191;else{_tmp301=_tmp300->f1;}}_LL190:(*
_tmp301).f1=Cyc_Absyn_Loc_n;return Cyc_Port_is_char(env,Cyc_Port_lookup_typedef(
env,_tmp301));_LL191: {struct Cyc_Absyn_IntType_struct*_tmp302=(struct Cyc_Absyn_IntType_struct*)
_tmp2FF;if(_tmp302->tag != 6)goto _LL193;else{_tmp303=_tmp302->f2;}}_LL192: return 1;
_LL193:;_LL194: return 0;_LL18E:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct
Cyc_Port_Cenv;static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){
void*_tmp304=t;struct _tuple0*_tmp306;struct Cyc_Absyn_PtrInfo _tmp309;void*_tmp30A;
struct Cyc_Absyn_Tqual _tmp30B;struct Cyc_Absyn_PtrAtts _tmp30C;void*_tmp30D;union
Cyc_Absyn_Constraint*_tmp30E;union Cyc_Absyn_Constraint*_tmp30F;union Cyc_Absyn_Constraint*
_tmp310;struct Cyc_Absyn_PtrLoc*_tmp311;struct Cyc_Absyn_ArrayInfo _tmp316;void*
_tmp317;struct Cyc_Absyn_Tqual _tmp318;union Cyc_Absyn_Constraint*_tmp319;struct Cyc_Position_Segment*
_tmp31A;struct Cyc_Absyn_FnInfo _tmp31C;void*_tmp31D;struct Cyc_List_List*_tmp31E;
struct Cyc_Absyn_AggrInfo _tmp320;union Cyc_Absyn_AggrInfoU _tmp321;struct Cyc_List_List*
_tmp324;_LL196: {struct Cyc_Absyn_TypedefType_struct*_tmp305=(struct Cyc_Absyn_TypedefType_struct*)
_tmp304;if(_tmp305->tag != 18)goto _LL198;else{_tmp306=_tmp305->f1;}}_LL197:(*
_tmp306).f1=Cyc_Absyn_Loc_n;return Cyc_Port_lookup_typedef_ctype(env,_tmp306);
_LL198: {struct Cyc_Absyn_VoidType_struct*_tmp307=(struct Cyc_Absyn_VoidType_struct*)
_tmp304;if(_tmp307->tag != 0)goto _LL19A;}_LL199: return Cyc_Port_void_ct();_LL19A: {
struct Cyc_Absyn_PointerType_struct*_tmp308=(struct Cyc_Absyn_PointerType_struct*)
_tmp304;if(_tmp308->tag != 5)goto _LL19C;else{_tmp309=_tmp308->f1;_tmp30A=_tmp309.elt_typ;
_tmp30B=_tmp309.elt_tq;_tmp30C=_tmp309.ptr_atts;_tmp30D=_tmp30C.rgn;_tmp30E=
_tmp30C.nullable;_tmp30F=_tmp30C.bounds;_tmp310=_tmp30C.zero_term;_tmp311=
_tmp30C.ptrloc;}}_LL19B: {struct Cyc_Position_Segment*_tmp325=(unsigned int)
_tmp311?_tmp311->ptr_loc: 0;struct Cyc_Position_Segment*_tmp326=(unsigned int)
_tmp311?_tmp311->rgn_loc: 0;struct Cyc_Position_Segment*_tmp327=(unsigned int)
_tmp311?_tmp311->zt_loc: 0;void*_tmp328=Cyc_Port_type_to_ctype(env,_tmp30A);void*
new_rgn;{void*_tmp329=_tmp30D;struct Cyc_Absyn_Tvar*_tmp32C;struct Cyc_Absyn_Tvar
_tmp32D;struct _dyneither_ptr*_tmp32E;_LL1AF: {struct Cyc_Absyn_HeapRgn_struct*
_tmp32A=(struct Cyc_Absyn_HeapRgn_struct*)_tmp329;if(_tmp32A->tag != 21)goto _LL1B1;}
_LL1B0: new_rgn=Cyc_Port_heap_ct();goto _LL1AE;_LL1B1: {struct Cyc_Absyn_VarType_struct*
_tmp32B=(struct Cyc_Absyn_VarType_struct*)_tmp329;if(_tmp32B->tag != 2)goto _LL1B3;
else{_tmp32C=_tmp32B->f1;_tmp32D=*_tmp32C;_tmp32E=_tmp32D.name;}}_LL1B2: new_rgn=
Cyc_Port_rgnvar_ct(_tmp32E);goto _LL1AE;_LL1B3: {struct Cyc_Absyn_Evar_struct*
_tmp32F=(struct Cyc_Absyn_Evar_struct*)_tmp329;if(_tmp32F->tag != 1)goto _LL1B5;}
_LL1B4: if(Cyc_Port_in_fn_arg(env))new_rgn=Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var());
else{if(Cyc_Port_in_fn_res(env) || Cyc_Port_in_toplevel(env))new_rgn=Cyc_Port_heap_ct();
else{new_rgn=Cyc_Port_var();}}goto _LL1AE;_LL1B5:;_LL1B6: new_rgn=Cyc_Port_heap_ct();
goto _LL1AE;_LL1AE:;}{int _tmp330=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp30F)== (void*)& Cyc_Absyn_DynEither_b_val;int orig_zt;{union Cyc_Absyn_Constraint
_tmp331=*_tmp310;int _tmp332;_LL1B8: if((_tmp331.No_constr).tag != 3)goto _LL1BA;
_tmp332=(int)(_tmp331.No_constr).val;_LL1B9: orig_zt=Cyc_Port_is_char(env,_tmp30A);
goto _LL1B7;_LL1BA:;_LL1BB: orig_zt=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp310);goto _LL1B7;_LL1B7:;}if((env->gcenv)->porting){void*_tmp333=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,_tmp333,_tmp30B.print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct(),_tmp30B.loc);if(_tmp30B.print_const)Cyc_Port_unify_ct(
_tmp333,Cyc_Port_const_ct());{void*_tmp334=Cyc_Port_var();Cyc_Port_register_ptr_cvars(
env,_tmp334,_tmp330?Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),_tmp325);if(_tmp330)Cyc_Port_unify_ct(
_tmp334,Cyc_Port_fat_ct());{void*_tmp335=Cyc_Port_var();Cyc_Port_register_zeroterm_cvars(
env,_tmp335,orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),_tmp327);{union Cyc_Absyn_Constraint
_tmp336=*_tmp310;int _tmp337;_LL1BD: if((_tmp336.No_constr).tag != 3)goto _LL1BF;
_tmp337=(int)(_tmp336.No_constr).val;_LL1BE: goto _LL1BC;_LL1BF:;_LL1C0: Cyc_Port_unify_ct(
_tmp335,((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp310)?Cyc_Port_zterm_ct():
Cyc_Port_nozterm_ct());goto _LL1BC;_LL1BC:;}return Cyc_Port_ptr_ct(_tmp328,_tmp333,
_tmp334,new_rgn,_tmp335);};};}else{return Cyc_Port_ptr_ct(_tmp328,_tmp30B.print_const?
Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp330?Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),
new_rgn,orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}};}_LL19C: {struct
Cyc_Absyn_IntType_struct*_tmp312=(struct Cyc_Absyn_IntType_struct*)_tmp304;if(
_tmp312->tag != 6)goto _LL19E;}_LL19D: goto _LL19F;_LL19E: {struct Cyc_Absyn_FloatType_struct*
_tmp313=(struct Cyc_Absyn_FloatType_struct*)_tmp304;if(_tmp313->tag != 7)goto
_LL1A0;}_LL19F: goto _LL1A1;_LL1A0: {struct Cyc_Absyn_DoubleType_struct*_tmp314=(
struct Cyc_Absyn_DoubleType_struct*)_tmp304;if(_tmp314->tag != 8)goto _LL1A2;}
_LL1A1: return Cyc_Port_arith_ct();_LL1A2: {struct Cyc_Absyn_ArrayType_struct*
_tmp315=(struct Cyc_Absyn_ArrayType_struct*)_tmp304;if(_tmp315->tag != 9)goto
_LL1A4;else{_tmp316=_tmp315->f1;_tmp317=_tmp316.elt_type;_tmp318=_tmp316.tq;
_tmp319=_tmp316.zero_term;_tmp31A=_tmp316.zt_loc;}}_LL1A3: {void*_tmp338=Cyc_Port_type_to_ctype(
env,_tmp317);int orig_zt;{union Cyc_Absyn_Constraint _tmp339=*_tmp319;int _tmp33A;
_LL1C2: if((_tmp339.No_constr).tag != 3)goto _LL1C4;_tmp33A=(int)(_tmp339.No_constr).val;
_LL1C3: orig_zt=0;goto _LL1C1;_LL1C4:;_LL1C5: orig_zt=((int(*)(union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_val)(_tmp319);goto _LL1C1;_LL1C1:;}if((env->gcenv)->porting){
void*_tmp33B=Cyc_Port_var();Cyc_Port_register_const_cvar(env,_tmp33B,_tmp318.print_const?
Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp318.loc);{void*_tmp33C=Cyc_Port_var();
Cyc_Port_register_zeroterm_cvars(env,_tmp33C,orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),
_tmp31A);{union Cyc_Absyn_Constraint _tmp33D=*_tmp319;int _tmp33E;_LL1C7: if((
_tmp33D.No_constr).tag != 3)goto _LL1C9;_tmp33E=(int)(_tmp33D.No_constr).val;
_LL1C8: goto _LL1C6;_LL1C9:;_LL1CA: Cyc_Port_unify_ct(_tmp33C,((int(*)(union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_val)(_tmp319)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());
goto _LL1C6;_LL1C6:;}return Cyc_Port_array_ct(_tmp338,_tmp33B,_tmp33C);};}else{
return Cyc_Port_array_ct(_tmp338,_tmp318.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}}_LL1A4: {struct Cyc_Absyn_FnType_struct*
_tmp31B=(struct Cyc_Absyn_FnType_struct*)_tmp304;if(_tmp31B->tag != 10)goto _LL1A6;
else{_tmp31C=_tmp31B->f1;_tmp31D=_tmp31C.ret_typ;_tmp31E=_tmp31C.args;}}_LL1A5: {
void*_tmp33F=Cyc_Port_type_to_ctype(Cyc_Port_set_cpos(env,Cyc_Port_FnRes_pos),
_tmp31D);struct Cyc_Port_Cenv*_tmp340=Cyc_Port_set_cpos(env,Cyc_Port_FnArg_pos);
struct Cyc_List_List*cargs=0;for(0;_tmp31E != 0;_tmp31E=_tmp31E->tl){struct Cyc_Absyn_Tqual
_tmp342;void*_tmp343;struct _tuple7 _tmp341=*((struct _tuple7*)_tmp31E->hd);_tmp342=
_tmp341.f2;_tmp343=_tmp341.f3;{struct Cyc_List_List*_tmp607;cargs=((_tmp607=
_cycalloc(sizeof(*_tmp607)),((_tmp607->hd=Cyc_Port_type_to_ctype(_tmp340,_tmp343),((
_tmp607->tl=cargs,_tmp607))))));};}return Cyc_Port_fn_ct(_tmp33F,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(cargs));}_LL1A6: {struct Cyc_Absyn_AggrType_struct*
_tmp31F=(struct Cyc_Absyn_AggrType_struct*)_tmp304;if(_tmp31F->tag != 12)goto
_LL1A8;else{_tmp320=_tmp31F->f1;_tmp321=_tmp320.aggr_info;}}_LL1A7: {union Cyc_Absyn_AggrInfoU
_tmp345=_tmp321;struct _tuple2 _tmp346;enum Cyc_Absyn_AggrKind _tmp347;struct
_tuple0*_tmp348;struct _tuple2 _tmp349;enum Cyc_Absyn_AggrKind _tmp34A;struct
_tuple0*_tmp34B;struct Cyc_Absyn_Aggrdecl**_tmp34C;_LL1CC: if((_tmp345.UnknownAggr).tag
!= 1)goto _LL1CE;_tmp346=(struct _tuple2)(_tmp345.UnknownAggr).val;_tmp347=_tmp346.f1;
if(_tmp347 != Cyc_Absyn_StructA)goto _LL1CE;_tmp348=_tmp346.f2;_LL1CD:(*_tmp348).f1=
Cyc_Absyn_Loc_n;{struct _tuple10*_tmp34D=Cyc_Port_lookup_struct_decl(env,_tmp348);
return Cyc_Port_known_aggr_ct(_tmp34D);};_LL1CE: if((_tmp345.UnknownAggr).tag != 1)
goto _LL1D0;_tmp349=(struct _tuple2)(_tmp345.UnknownAggr).val;_tmp34A=_tmp349.f1;
if(_tmp34A != Cyc_Absyn_UnionA)goto _LL1D0;_tmp34B=_tmp349.f2;_LL1CF:(*_tmp34B).f1=
Cyc_Absyn_Loc_n;{struct _tuple10*_tmp34E=Cyc_Port_lookup_union_decl(env,_tmp34B);
return Cyc_Port_known_aggr_ct(_tmp34E);};_LL1D0: if((_tmp345.KnownAggr).tag != 2)
goto _LL1CB;_tmp34C=(struct Cyc_Absyn_Aggrdecl**)(_tmp345.KnownAggr).val;_LL1D1: {
struct Cyc_Absyn_Aggrdecl*_tmp34F=*_tmp34C;switch(_tmp34F->kind){case Cyc_Absyn_StructA:
_LL1D2: {struct _tuple10*_tmp350=Cyc_Port_lookup_struct_decl(env,_tmp34F->name);
return Cyc_Port_known_aggr_ct(_tmp350);}case Cyc_Absyn_UnionA: _LL1D3: {struct
_tuple10*_tmp351=Cyc_Port_lookup_union_decl(env,_tmp34F->name);return Cyc_Port_known_aggr_ct(
_tmp351);}}}_LL1CB:;}_LL1A8: {struct Cyc_Absyn_EnumType_struct*_tmp322=(struct Cyc_Absyn_EnumType_struct*)
_tmp304;if(_tmp322->tag != 14)goto _LL1AA;}_LL1A9: return Cyc_Port_arith_ct();_LL1AA: {
struct Cyc_Absyn_AnonEnumType_struct*_tmp323=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp304;if(_tmp323->tag != 15)goto _LL1AC;else{_tmp324=_tmp323->f1;}}_LL1AB: for(0;(
unsigned int)_tmp324;_tmp324=_tmp324->tl){(*((struct Cyc_Absyn_Enumfield*)_tmp324->hd)->name).f1=
Cyc_Absyn_Loc_n;env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)_tmp324->hd)->name,
Cyc_Absyn_sint_typ,Cyc_Port_const_ct(),Cyc_Port_arith_ct());}return Cyc_Port_arith_ct();
_LL1AC:;_LL1AD: return Cyc_Port_arith_ct();_LL195:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,enum Cyc_Absyn_Primop p,
struct Cyc_List_List*args){void*_tmp352=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)
_check_null(args))->hd);struct Cyc_List_List*_tmp353=args->tl;enum Cyc_Absyn_Primop
_tmp354=p;_LL1D6: if(_tmp354 != Cyc_Absyn_Plus)goto _LL1D8;_LL1D7: {void*_tmp355=
Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(_tmp353))->hd);
if(Cyc_Port_leq(_tmp352,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()))){Cyc_Port_leq(_tmp355,Cyc_Port_arith_ct());
return _tmp352;}else{if(Cyc_Port_leq(_tmp355,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),
Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()))){Cyc_Port_leq(_tmp352,Cyc_Port_arith_ct());
return _tmp355;}else{Cyc_Port_leq(_tmp352,Cyc_Port_arith_ct());Cyc_Port_leq(
_tmp355,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();}}}_LL1D8: if(_tmp354 != 
Cyc_Absyn_Minus)goto _LL1DA;_LL1D9: if(_tmp353 == 0){Cyc_Port_leq(_tmp352,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();}else{void*_tmp356=Cyc_Port_compress_ct((void*)_tmp353->hd);
if(Cyc_Port_leq(_tmp352,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()))){if(Cyc_Port_leq(_tmp356,Cyc_Port_ptr_ct(Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var())))return Cyc_Port_arith_ct();
else{Cyc_Port_leq(_tmp356,Cyc_Port_arith_ct());return _tmp352;}}else{Cyc_Port_leq(
_tmp352,Cyc_Port_arith_ct());Cyc_Port_leq(_tmp352,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();}}
_LL1DA: if(_tmp354 != Cyc_Absyn_Times)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if(
_tmp354 != Cyc_Absyn_Div)goto _LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(_tmp354 != Cyc_Absyn_Mod)
goto _LL1E0;_LL1DF: goto _LL1E1;_LL1E0: if(_tmp354 != Cyc_Absyn_Eq)goto _LL1E2;_LL1E1:
goto _LL1E3;_LL1E2: if(_tmp354 != Cyc_Absyn_Neq)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4:
if(_tmp354 != Cyc_Absyn_Lt)goto _LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp354 != Cyc_Absyn_Gt)
goto _LL1E8;_LL1E7: goto _LL1E9;_LL1E8: if(_tmp354 != Cyc_Absyn_Lte)goto _LL1EA;_LL1E9:
goto _LL1EB;_LL1EA: if(_tmp354 != Cyc_Absyn_Gte)goto _LL1EC;_LL1EB: goto _LL1ED;_LL1EC:
if(_tmp354 != Cyc_Absyn_Bitand)goto _LL1EE;_LL1ED: goto _LL1EF;_LL1EE: if(_tmp354 != 
Cyc_Absyn_Bitor)goto _LL1F0;_LL1EF: goto _LL1F1;_LL1F0: if(_tmp354 != Cyc_Absyn_Bitxor)
goto _LL1F2;_LL1F1: goto _LL1F3;_LL1F2: if(_tmp354 != Cyc_Absyn_Bitlshift)goto _LL1F4;
_LL1F3: goto _LL1F5;_LL1F4: if(_tmp354 != Cyc_Absyn_Bitlrshift)goto _LL1F6;_LL1F5:
goto _LL1F7;_LL1F6: if(_tmp354 != Cyc_Absyn_Bitarshift)goto _LL1F8;_LL1F7: Cyc_Port_leq(
_tmp352,Cyc_Port_arith_ct());Cyc_Port_leq((void*)((struct Cyc_List_List*)
_check_null(_tmp353))->hd,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();_LL1F8:
if(_tmp354 != Cyc_Absyn_Not)goto _LL1FA;_LL1F9: goto _LL1FB;_LL1FA: if(_tmp354 != Cyc_Absyn_Bitnot)
goto _LL1FC;_LL1FB: Cyc_Port_leq(_tmp352,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();
_LL1FC:;_LL1FD: {const char*_tmp60A;void*_tmp609;(_tmp609=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp60A=".size primop",
_tag_dyneither(_tmp60A,sizeof(char),13))),_tag_dyneither(_tmp609,sizeof(void*),0)));}
_LL1D5:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;static struct _tuple9 Cyc_Port_gen_lhs(
struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){void*_tmp359=e->r;struct _tuple0*
_tmp35B;struct _tuple0*_tmp35D;struct Cyc_Absyn_Exp*_tmp35F;struct Cyc_Absyn_Exp*
_tmp360;struct Cyc_Absyn_Exp*_tmp362;struct Cyc_Absyn_Exp*_tmp364;struct
_dyneither_ptr*_tmp365;struct Cyc_Absyn_Exp*_tmp367;struct _dyneither_ptr*_tmp368;
_LL1FF: {struct Cyc_Absyn_Var_e_struct*_tmp35A=(struct Cyc_Absyn_Var_e_struct*)
_tmp359;if(_tmp35A->tag != 1)goto _LL201;else{_tmp35B=_tmp35A->f1;}}_LL200: _tmp35D=
_tmp35B;goto _LL202;_LL201: {struct Cyc_Absyn_UnknownId_e_struct*_tmp35C=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp359;if(_tmp35C->tag != 2)goto _LL203;else{
_tmp35D=_tmp35C->f1;}}_LL202:(*_tmp35D).f1=Cyc_Absyn_Loc_n;return Cyc_Port_lookup_var(
env,_tmp35D);_LL203: {struct Cyc_Absyn_Subscript_e_struct*_tmp35E=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp359;if(_tmp35E->tag != 24)goto _LL205;else{_tmp35F=_tmp35E->f1;_tmp360=_tmp35E->f2;}}
_LL204: {void*_tmp369=Cyc_Port_var();void*_tmp36A=Cyc_Port_var();void*_tmp36B=
Cyc_Port_gen_exp(env,_tmp35F);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp360),Cyc_Port_arith_ct());{
void*_tmp36C=Cyc_Port_ptr_ct(_tmp369,_tmp36A,Cyc_Port_fat_ct(),Cyc_Port_var(),
Cyc_Port_var());Cyc_Port_leq(_tmp36B,_tmp36C);{struct _tuple9 _tmp60B;return(
_tmp60B.f1=_tmp36A,((_tmp60B.f2=_tmp369,_tmp60B)));};};}_LL205: {struct Cyc_Absyn_Deref_e_struct*
_tmp361=(struct Cyc_Absyn_Deref_e_struct*)_tmp359;if(_tmp361->tag != 21)goto _LL207;
else{_tmp362=_tmp361->f1;}}_LL206: {void*_tmp36E=Cyc_Port_var();void*_tmp36F=Cyc_Port_var();
void*_tmp370=Cyc_Port_ptr_ct(_tmp36E,_tmp36F,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var());
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp362),_tmp370);{struct _tuple9 _tmp60C;return(
_tmp60C.f1=_tmp36F,((_tmp60C.f2=_tmp36E,_tmp60C)));};}_LL207: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp363=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp359;if(_tmp363->tag != 22)goto
_LL209;else{_tmp364=_tmp363->f1;_tmp365=_tmp363->f2;}}_LL208: {void*_tmp372=Cyc_Port_var();
void*_tmp373=Cyc_Port_var();void*_tmp374=Cyc_Port_gen_exp(env,_tmp364);{struct
Cyc_Port_Cfield*_tmp60F;struct Cyc_Port_Cfield*_tmp60E[1];Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp364),Cyc_Port_unknown_aggr_ct(((_tmp60E[0]=((_tmp60F=_cycalloc(sizeof(*
_tmp60F)),((_tmp60F->qual=_tmp373,((_tmp60F->f=_tmp365,((_tmp60F->type=_tmp372,
_tmp60F)))))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp60E,sizeof(struct Cyc_Port_Cfield*),1))))));}{struct _tuple9
_tmp610;return(_tmp610.f1=_tmp373,((_tmp610.f2=_tmp372,_tmp610)));};}_LL209: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp366=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp359;if(_tmp366->tag != 23)goto _LL20B;else{_tmp367=_tmp366->f1;_tmp368=_tmp366->f2;}}
_LL20A: {void*_tmp378=Cyc_Port_var();void*_tmp379=Cyc_Port_var();void*_tmp37A=
Cyc_Port_gen_exp(env,_tmp367);{struct Cyc_Port_Cfield*_tmp613;struct Cyc_Port_Cfield*
_tmp612[1];Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp367),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(((
_tmp612[0]=((_tmp613=_cycalloc(sizeof(*_tmp613)),((_tmp613->qual=_tmp379,((
_tmp613->f=_tmp368,((_tmp613->type=_tmp378,_tmp613)))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp612,sizeof(struct Cyc_Port_Cfield*),
1))))),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));}{struct
_tuple9 _tmp614;return(_tmp614.f1=_tmp379,((_tmp614.f2=_tmp378,_tmp614)));};}
_LL20B:;_LL20C: {struct Cyc_String_pa_struct _tmp61C;void*_tmp61B[1];const char*
_tmp61A;void*_tmp619;(_tmp619=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp61C.tag=0,((
_tmp61C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp61B[0]=& _tmp61C,Cyc_aprintf(((_tmp61A="gen_lhs: %s",_tag_dyneither(_tmp61A,
sizeof(char),12))),_tag_dyneither(_tmp61B,sizeof(void*),1)))))))),_tag_dyneither(
_tmp619,sizeof(void*),0)));}_LL1FE:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){void*
_tmp382=e->r;union Cyc_Absyn_Cnst _tmp384;struct _tuple3 _tmp385;union Cyc_Absyn_Cnst
_tmp387;struct _dyneither_ptr _tmp388;union Cyc_Absyn_Cnst _tmp38A;struct _tuple4
_tmp38B;union Cyc_Absyn_Cnst _tmp38D;struct _tuple6 _tmp38E;union Cyc_Absyn_Cnst
_tmp395;struct _dyneither_ptr _tmp396;union Cyc_Absyn_Cnst _tmp398;struct _tuple5
_tmp399;int _tmp39A;union Cyc_Absyn_Cnst _tmp39C;struct _tuple5 _tmp39D;union Cyc_Absyn_Cnst
_tmp39F;struct _dyneither_ptr _tmp3A0;union Cyc_Absyn_Cnst _tmp3A2;struct
_dyneither_ptr _tmp3A3;union Cyc_Absyn_Cnst _tmp3A5;int _tmp3A6;struct _tuple0*
_tmp3A8;struct _tuple0*_tmp3AA;enum Cyc_Absyn_Primop _tmp3AC;struct Cyc_List_List*
_tmp3AD;struct Cyc_Absyn_Exp*_tmp3AF;struct Cyc_Core_Opt*_tmp3B0;struct Cyc_Absyn_Exp*
_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B3;enum Cyc_Absyn_Incrementor _tmp3B4;struct Cyc_Absyn_Exp*
_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B8;struct Cyc_Absyn_Exp*
_tmp3BA;struct Cyc_Absyn_Exp*_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Exp*
_tmp3BE;struct Cyc_Absyn_Exp*_tmp3C0;struct Cyc_Absyn_Exp*_tmp3C1;struct Cyc_Absyn_Exp*
_tmp3C3;struct Cyc_List_List*_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C6;struct Cyc_List_List*
_tmp3C7;struct Cyc_Absyn_Exp*_tmp3CA;void*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CE;
struct Cyc_Absyn_Exp*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D2;struct Cyc_Absyn_Exp*
_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D7;struct
_dyneither_ptr*_tmp3D8;struct Cyc_Absyn_Exp*_tmp3DA;struct _dyneither_ptr*_tmp3DB;
struct Cyc_Absyn_MallocInfo _tmp3DD;void**_tmp3DE;struct Cyc_Absyn_Exp*_tmp3DF;
struct Cyc_Absyn_Exp*_tmp3E1;struct Cyc_Absyn_Exp*_tmp3E2;struct Cyc_Absyn_Stmt*
_tmp3ED;_LL20E: {struct Cyc_Absyn_Const_e_struct*_tmp383=(struct Cyc_Absyn_Const_e_struct*)
_tmp382;if(_tmp383->tag != 0)goto _LL210;else{_tmp384=_tmp383->f1;if((_tmp384.Char_c).tag
!= 2)goto _LL210;_tmp385=(struct _tuple3)(_tmp384.Char_c).val;}}_LL20F: goto _LL211;
_LL210: {struct Cyc_Absyn_Const_e_struct*_tmp386=(struct Cyc_Absyn_Const_e_struct*)
_tmp382;if(_tmp386->tag != 0)goto _LL212;else{_tmp387=_tmp386->f1;if((_tmp387.Wchar_c).tag
!= 3)goto _LL212;_tmp388=(struct _dyneither_ptr)(_tmp387.Wchar_c).val;}}_LL211:
goto _LL213;_LL212: {struct Cyc_Absyn_Const_e_struct*_tmp389=(struct Cyc_Absyn_Const_e_struct*)
_tmp382;if(_tmp389->tag != 0)goto _LL214;else{_tmp38A=_tmp389->f1;if((_tmp38A.Short_c).tag
!= 4)goto _LL214;_tmp38B=(struct _tuple4)(_tmp38A.Short_c).val;}}_LL213: goto _LL215;
_LL214: {struct Cyc_Absyn_Const_e_struct*_tmp38C=(struct Cyc_Absyn_Const_e_struct*)
_tmp382;if(_tmp38C->tag != 0)goto _LL216;else{_tmp38D=_tmp38C->f1;if((_tmp38D.LongLong_c).tag
!= 6)goto _LL216;_tmp38E=(struct _tuple6)(_tmp38D.LongLong_c).val;}}_LL215: goto
_LL217;_LL216: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp38F=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp382;if(_tmp38F->tag != 18)goto _LL218;}_LL217: goto _LL219;_LL218: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp390=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp382;if(_tmp390->tag != 19)goto
_LL21A;}_LL219: goto _LL21B;_LL21A: {struct Cyc_Absyn_Offsetof_e_struct*_tmp391=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp382;if(_tmp391->tag != 20)goto _LL21C;}
_LL21B: goto _LL21D;_LL21C: {struct Cyc_Absyn_Enum_e_struct*_tmp392=(struct Cyc_Absyn_Enum_e_struct*)
_tmp382;if(_tmp392->tag != 32)goto _LL21E;}_LL21D: goto _LL21F;_LL21E: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp393=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp382;if(_tmp393->tag != 33)goto
_LL220;}_LL21F: goto _LL221;_LL220: {struct Cyc_Absyn_Const_e_struct*_tmp394=(
struct Cyc_Absyn_Const_e_struct*)_tmp382;if(_tmp394->tag != 0)goto _LL222;else{
_tmp395=_tmp394->f1;if((_tmp395.Float_c).tag != 7)goto _LL222;_tmp396=(struct
_dyneither_ptr)(_tmp395.Float_c).val;}}_LL221: return Cyc_Port_arith_ct();_LL222: {
struct Cyc_Absyn_Const_e_struct*_tmp397=(struct Cyc_Absyn_Const_e_struct*)_tmp382;
if(_tmp397->tag != 0)goto _LL224;else{_tmp398=_tmp397->f1;if((_tmp398.Int_c).tag != 
5)goto _LL224;_tmp399=(struct _tuple5)(_tmp398.Int_c).val;_tmp39A=_tmp399.f2;if(
_tmp39A != 0)goto _LL224;}}_LL223: return Cyc_Port_zero_ct();_LL224: {struct Cyc_Absyn_Const_e_struct*
_tmp39B=(struct Cyc_Absyn_Const_e_struct*)_tmp382;if(_tmp39B->tag != 0)goto _LL226;
else{_tmp39C=_tmp39B->f1;if((_tmp39C.Int_c).tag != 5)goto _LL226;_tmp39D=(struct
_tuple5)(_tmp39C.Int_c).val;}}_LL225: return Cyc_Port_arith_ct();_LL226: {struct
Cyc_Absyn_Const_e_struct*_tmp39E=(struct Cyc_Absyn_Const_e_struct*)_tmp382;if(
_tmp39E->tag != 0)goto _LL228;else{_tmp39F=_tmp39E->f1;if((_tmp39F.String_c).tag != 
8)goto _LL228;_tmp3A0=(struct _dyneither_ptr)(_tmp39F.String_c).val;}}_LL227:
return Cyc_Port_ptr_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_fat_ct(),
Cyc_Port_heap_ct(),Cyc_Port_zterm_ct());_LL228: {struct Cyc_Absyn_Const_e_struct*
_tmp3A1=(struct Cyc_Absyn_Const_e_struct*)_tmp382;if(_tmp3A1->tag != 0)goto _LL22A;
else{_tmp3A2=_tmp3A1->f1;if((_tmp3A2.Wstring_c).tag != 9)goto _LL22A;_tmp3A3=(
struct _dyneither_ptr)(_tmp3A2.Wstring_c).val;}}_LL229: return Cyc_Port_ptr_ct(Cyc_Port_arith_ct(),
Cyc_Port_const_ct(),Cyc_Port_fat_ct(),Cyc_Port_heap_ct(),Cyc_Port_zterm_ct());
_LL22A: {struct Cyc_Absyn_Const_e_struct*_tmp3A4=(struct Cyc_Absyn_Const_e_struct*)
_tmp382;if(_tmp3A4->tag != 0)goto _LL22C;else{_tmp3A5=_tmp3A4->f1;if((_tmp3A5.Null_c).tag
!= 1)goto _LL22C;_tmp3A6=(int)(_tmp3A5.Null_c).val;}}_LL22B: return Cyc_Port_zero_ct();
_LL22C: {struct Cyc_Absyn_Var_e_struct*_tmp3A7=(struct Cyc_Absyn_Var_e_struct*)
_tmp382;if(_tmp3A7->tag != 1)goto _LL22E;else{_tmp3A8=_tmp3A7->f1;}}_LL22D: _tmp3AA=
_tmp3A8;goto _LL22F;_LL22E: {struct Cyc_Absyn_UnknownId_e_struct*_tmp3A9=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp382;if(_tmp3A9->tag != 2)goto _LL230;else{
_tmp3AA=_tmp3A9->f1;}}_LL22F:(*_tmp3AA).f1=Cyc_Absyn_Loc_n;{void*_tmp3F2;struct
_tuple9 _tmp3F1=Cyc_Port_lookup_var(env,_tmp3AA);_tmp3F2=_tmp3F1.f2;if(Cyc_Port_isfn(
env,_tmp3AA)){_tmp3F2=Cyc_Port_instantiate(_tmp3F2);return Cyc_Port_ptr_ct(
_tmp3F2,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_heap_ct(),Cyc_Port_nozterm_ct());}
else{if(Cyc_Port_isarray(env,_tmp3AA)){void*_tmp3F3=Cyc_Port_var();void*_tmp3F4=
Cyc_Port_var();void*_tmp3F5=Cyc_Port_var();void*_tmp3F6=Cyc_Port_array_ct(
_tmp3F3,_tmp3F4,_tmp3F5);Cyc_Port_unifies(_tmp3F2,_tmp3F6);{void*_tmp3F7=Cyc_Port_ptr_ct(
_tmp3F3,_tmp3F4,Cyc_Port_fat_ct(),Cyc_Port_var(),_tmp3F5);return _tmp3F7;};}else{
return _tmp3F2;}}};_LL230: {struct Cyc_Absyn_Primop_e_struct*_tmp3AB=(struct Cyc_Absyn_Primop_e_struct*)
_tmp382;if(_tmp3AB->tag != 3)goto _LL232;else{_tmp3AC=_tmp3AB->f1;_tmp3AD=_tmp3AB->f2;}}
_LL231: return Cyc_Port_gen_primop(env,_tmp3AC,((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp3AD));_LL232: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp3AE=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp382;if(_tmp3AE->tag != 4)goto
_LL234;else{_tmp3AF=_tmp3AE->f1;_tmp3B0=_tmp3AE->f2;_tmp3B1=_tmp3AE->f3;}}_LL233: {
void*_tmp3F9;void*_tmp3FA;struct _tuple9 _tmp3F8=Cyc_Port_gen_lhs(env,_tmp3AF);
_tmp3F9=_tmp3F8.f1;_tmp3FA=_tmp3F8.f2;Cyc_Port_unifies(_tmp3F9,Cyc_Port_notconst_ct());{
void*_tmp3FB=Cyc_Port_gen_exp(env,_tmp3B1);if((unsigned int)_tmp3B0){struct Cyc_List_List
_tmp61D;struct Cyc_List_List x2=(_tmp61D.hd=_tmp3FB,((_tmp61D.tl=0,_tmp61D)));
struct Cyc_List_List _tmp61E;struct Cyc_List_List x1=(_tmp61E.hd=_tmp3FA,((_tmp61E.tl=(
struct Cyc_List_List*)& x2,_tmp61E)));void*_tmp3FC=Cyc_Port_gen_primop(env,(enum 
Cyc_Absyn_Primop)_tmp3B0->v,(struct Cyc_List_List*)& x1);Cyc_Port_leq(_tmp3FC,
_tmp3FA);return _tmp3FA;}else{Cyc_Port_leq(_tmp3FB,_tmp3FA);return _tmp3FA;}};}
_LL234: {struct Cyc_Absyn_Increment_e_struct*_tmp3B2=(struct Cyc_Absyn_Increment_e_struct*)
_tmp382;if(_tmp3B2->tag != 5)goto _LL236;else{_tmp3B3=_tmp3B2->f1;_tmp3B4=_tmp3B2->f2;}}
_LL235: {void*_tmp400;void*_tmp401;struct _tuple9 _tmp3FF=Cyc_Port_gen_lhs(env,
_tmp3B3);_tmp400=_tmp3FF.f1;_tmp401=_tmp3FF.f2;Cyc_Port_unifies(_tmp400,Cyc_Port_notconst_ct());
Cyc_Port_leq(_tmp401,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()));Cyc_Port_leq(_tmp401,Cyc_Port_arith_ct());return
_tmp401;}_LL236: {struct Cyc_Absyn_Conditional_e_struct*_tmp3B5=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp382;if(_tmp3B5->tag != 6)goto _LL238;else{_tmp3B6=_tmp3B5->f1;_tmp3B7=_tmp3B5->f2;
_tmp3B8=_tmp3B5->f3;}}_LL237: {void*_tmp402=Cyc_Port_var();Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp3B6),Cyc_Port_arith_ct());Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3B7),
_tmp402);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3B8),_tmp402);return _tmp402;}
_LL238: {struct Cyc_Absyn_And_e_struct*_tmp3B9=(struct Cyc_Absyn_And_e_struct*)
_tmp382;if(_tmp3B9->tag != 7)goto _LL23A;else{_tmp3BA=_tmp3B9->f1;_tmp3BB=_tmp3B9->f2;}}
_LL239: _tmp3BD=_tmp3BA;_tmp3BE=_tmp3BB;goto _LL23B;_LL23A: {struct Cyc_Absyn_Or_e_struct*
_tmp3BC=(struct Cyc_Absyn_Or_e_struct*)_tmp382;if(_tmp3BC->tag != 8)goto _LL23C;
else{_tmp3BD=_tmp3BC->f1;_tmp3BE=_tmp3BC->f2;}}_LL23B: Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp3BD),Cyc_Port_arith_ct());Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3BE),Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();_LL23C: {struct Cyc_Absyn_SeqExp_e_struct*_tmp3BF=(
struct Cyc_Absyn_SeqExp_e_struct*)_tmp382;if(_tmp3BF->tag != 9)goto _LL23E;else{
_tmp3C0=_tmp3BF->f1;_tmp3C1=_tmp3BF->f2;}}_LL23D: Cyc_Port_gen_exp(env,_tmp3C0);
return Cyc_Port_gen_exp(env,_tmp3C1);_LL23E: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp3C2=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp382;if(_tmp3C2->tag != 10)goto
_LL240;else{_tmp3C3=_tmp3C2->f1;_tmp3C4=_tmp3C2->f2;}}_LL23F: _tmp3C6=_tmp3C3;
_tmp3C7=_tmp3C4;goto _LL241;_LL240: {struct Cyc_Absyn_FnCall_e_struct*_tmp3C5=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp382;if(_tmp3C5->tag != 11)goto _LL242;else{
_tmp3C6=_tmp3C5->f1;_tmp3C7=_tmp3C5->f2;}}_LL241: {void*_tmp403=Cyc_Port_var();
void*_tmp404=Cyc_Port_gen_exp(env,_tmp3C6);struct Cyc_List_List*_tmp405=((struct
Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp3C7);struct
Cyc_List_List*_tmp406=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)((void*(*)(void*x))Cyc_Port_new_var,_tmp405);Cyc_Port_unifies(
_tmp404,Cyc_Port_ptr_ct(Cyc_Port_fn_ct(_tmp403,_tmp406),Cyc_Port_var(),Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_nozterm_ct()));for(0;_tmp405 != 0;(_tmp405=_tmp405->tl,
_tmp406=_tmp406->tl)){Cyc_Port_leq((void*)_tmp405->hd,(void*)((struct Cyc_List_List*)
_check_null(_tmp406))->hd);}return _tmp403;}_LL242: {struct Cyc_Absyn_Throw_e_struct*
_tmp3C8=(struct Cyc_Absyn_Throw_e_struct*)_tmp382;if(_tmp3C8->tag != 12)goto _LL244;}
_LL243: {const char*_tmp621;void*_tmp620;(_tmp620=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp621="throw",_tag_dyneither(
_tmp621,sizeof(char),6))),_tag_dyneither(_tmp620,sizeof(void*),0)));}_LL244: {
struct Cyc_Absyn_NoInstantiate_e_struct*_tmp3C9=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp382;if(_tmp3C9->tag != 13)goto _LL246;else{_tmp3CA=_tmp3C9->f1;}}_LL245: return
Cyc_Port_gen_exp(env,_tmp3CA);_LL246: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp3CB=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp382;if(_tmp3CB->tag != 14)goto
_LL248;}_LL247: {const char*_tmp624;void*_tmp623;(_tmp623=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp624="instantiate",
_tag_dyneither(_tmp624,sizeof(char),12))),_tag_dyneither(_tmp623,sizeof(void*),0)));}
_LL248: {struct Cyc_Absyn_Cast_e_struct*_tmp3CC=(struct Cyc_Absyn_Cast_e_struct*)
_tmp382;if(_tmp3CC->tag != 15)goto _LL24A;else{_tmp3CD=(void*)_tmp3CC->f1;_tmp3CE=
_tmp3CC->f2;}}_LL249: {void*_tmp40B=Cyc_Port_type_to_ctype(env,_tmp3CD);void*
_tmp40C=Cyc_Port_gen_exp(env,_tmp3CE);Cyc_Port_leq(_tmp40B,_tmp40C);return
_tmp40C;}_LL24A: {struct Cyc_Absyn_Address_e_struct*_tmp3CF=(struct Cyc_Absyn_Address_e_struct*)
_tmp382;if(_tmp3CF->tag != 16)goto _LL24C;else{_tmp3D0=_tmp3CF->f1;}}_LL24B: {void*
_tmp40E;void*_tmp40F;struct _tuple9 _tmp40D=Cyc_Port_gen_lhs(env,_tmp3D0);_tmp40E=
_tmp40D.f1;_tmp40F=_tmp40D.f2;return Cyc_Port_ptr_ct(_tmp40F,_tmp40E,Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_var());}_LL24C: {struct Cyc_Absyn_Subscript_e_struct*
_tmp3D1=(struct Cyc_Absyn_Subscript_e_struct*)_tmp382;if(_tmp3D1->tag != 24)goto
_LL24E;else{_tmp3D2=_tmp3D1->f1;_tmp3D3=_tmp3D1->f2;}}_LL24D: {void*_tmp410=Cyc_Port_var();
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3D3),Cyc_Port_arith_ct());{void*_tmp411=
Cyc_Port_gen_exp(env,_tmp3D2);Cyc_Port_leq(_tmp411,Cyc_Port_ptr_ct(_tmp410,Cyc_Port_var(),
Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()));return _tmp410;};}_LL24E: {
struct Cyc_Absyn_Deref_e_struct*_tmp3D4=(struct Cyc_Absyn_Deref_e_struct*)_tmp382;
if(_tmp3D4->tag != 21)goto _LL250;else{_tmp3D5=_tmp3D4->f1;}}_LL24F: {void*_tmp412=
Cyc_Port_var();Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3D5),Cyc_Port_ptr_ct(
_tmp412,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));return
_tmp412;}_LL250: {struct Cyc_Absyn_AggrMember_e_struct*_tmp3D6=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp382;if(_tmp3D6->tag != 22)goto _LL252;else{_tmp3D7=_tmp3D6->f1;_tmp3D8=_tmp3D6->f2;}}
_LL251: {void*_tmp413=Cyc_Port_var();void*_tmp414=Cyc_Port_gen_exp(env,_tmp3D7);{
struct Cyc_Port_Cfield*_tmp627;struct Cyc_Port_Cfield*_tmp626[1];Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp3D7),Cyc_Port_unknown_aggr_ct(((_tmp626[0]=((_tmp627=_cycalloc(sizeof(*
_tmp627)),((_tmp627->qual=Cyc_Port_var(),((_tmp627->f=_tmp3D8,((_tmp627->type=
_tmp413,_tmp627)))))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp626,sizeof(struct Cyc_Port_Cfield*),1))))));}return _tmp413;}
_LL252: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp3D9=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp382;if(_tmp3D9->tag != 23)goto _LL254;else{_tmp3DA=_tmp3D9->f1;_tmp3DB=_tmp3D9->f2;}}
_LL253: {void*_tmp417=Cyc_Port_var();void*_tmp418=Cyc_Port_gen_exp(env,_tmp3DA);{
struct Cyc_Port_Cfield*_tmp62A;struct Cyc_Port_Cfield*_tmp629[1];Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp3DA),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(((_tmp629[0]=((_tmp62A=
_cycalloc(sizeof(*_tmp62A)),((_tmp62A->qual=Cyc_Port_var(),((_tmp62A->f=_tmp3DB,((
_tmp62A->type=_tmp417,_tmp62A)))))))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp629,sizeof(struct Cyc_Port_Cfield*),
1))))),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));}return
_tmp417;}_LL254: {struct Cyc_Absyn_Malloc_e_struct*_tmp3DC=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp382;if(_tmp3DC->tag != 34)goto _LL256;else{_tmp3DD=_tmp3DC->f1;_tmp3DE=_tmp3DD.elt_type;
_tmp3DF=_tmp3DD.num_elts;}}_LL255: Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3DF),Cyc_Port_arith_ct());{
void*_tmp41B=(unsigned int)_tmp3DE?Cyc_Port_type_to_ctype(env,*_tmp3DE): Cyc_Port_var();
return Cyc_Port_ptr_ct(_tmp41B,Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_heap_ct(),
Cyc_Port_var());};_LL256: {struct Cyc_Absyn_Swap_e_struct*_tmp3E0=(struct Cyc_Absyn_Swap_e_struct*)
_tmp382;if(_tmp3E0->tag != 35)goto _LL258;else{_tmp3E1=_tmp3E0->f1;_tmp3E2=_tmp3E0->f2;}}
_LL257: {const char*_tmp62D;void*_tmp62C;(_tmp62C=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp62D="Swap_e",_tag_dyneither(
_tmp62D,sizeof(char),7))),_tag_dyneither(_tmp62C,sizeof(void*),0)));}_LL258: {
struct Cyc_Absyn_New_e_struct*_tmp3E3=(struct Cyc_Absyn_New_e_struct*)_tmp382;if(
_tmp3E3->tag != 17)goto _LL25A;}_LL259: {const char*_tmp630;void*_tmp62F;(_tmp62F=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp630="New_e",_tag_dyneither(_tmp630,sizeof(char),6))),_tag_dyneither(_tmp62F,
sizeof(void*),0)));}_LL25A: {struct Cyc_Absyn_Tuple_e_struct*_tmp3E4=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp382;if(_tmp3E4->tag != 25)goto _LL25C;}_LL25B: {const char*_tmp633;void*_tmp632;(
_tmp632=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp633="Tuple_e",_tag_dyneither(_tmp633,sizeof(char),8))),_tag_dyneither(
_tmp632,sizeof(void*),0)));}_LL25C: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp3E5=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp382;if(_tmp3E5->tag != 26)goto
_LL25E;}_LL25D: {const char*_tmp636;void*_tmp635;(_tmp635=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp636="CompoundLit_e",
_tag_dyneither(_tmp636,sizeof(char),14))),_tag_dyneither(_tmp635,sizeof(void*),0)));}
_LL25E: {struct Cyc_Absyn_Array_e_struct*_tmp3E6=(struct Cyc_Absyn_Array_e_struct*)
_tmp382;if(_tmp3E6->tag != 27)goto _LL260;}_LL25F: {const char*_tmp639;void*_tmp638;(
_tmp638=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp639="Array_e",_tag_dyneither(_tmp639,sizeof(char),8))),_tag_dyneither(
_tmp638,sizeof(void*),0)));}_LL260: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp3E7=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp382;if(_tmp3E7->tag != 28)
goto _LL262;}_LL261: {const char*_tmp63C;void*_tmp63B;(_tmp63B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp63C="Comprehension_e",
_tag_dyneither(_tmp63C,sizeof(char),16))),_tag_dyneither(_tmp63B,sizeof(void*),0)));}
_LL262: {struct Cyc_Absyn_Aggregate_e_struct*_tmp3E8=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp382;if(_tmp3E8->tag != 29)goto _LL264;}_LL263: {const char*_tmp63F;void*_tmp63E;(
_tmp63E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp63F="Aggregate_e",_tag_dyneither(_tmp63F,sizeof(char),12))),_tag_dyneither(
_tmp63E,sizeof(void*),0)));}_LL264: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp3E9=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp382;if(_tmp3E9->tag != 30)goto _LL266;}
_LL265: {const char*_tmp642;void*_tmp641;(_tmp641=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp642="AnonStruct_e",
_tag_dyneither(_tmp642,sizeof(char),13))),_tag_dyneither(_tmp641,sizeof(void*),0)));}
_LL266: {struct Cyc_Absyn_Datatype_e_struct*_tmp3EA=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp382;if(_tmp3EA->tag != 31)goto _LL268;}_LL267: {const char*_tmp645;void*_tmp644;(
_tmp644=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp645="Datatype_e",_tag_dyneither(_tmp645,sizeof(char),11))),_tag_dyneither(
_tmp644,sizeof(void*),0)));}_LL268: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp3EB=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp382;if(_tmp3EB->tag != 36)
goto _LL26A;}_LL269: {const char*_tmp648;void*_tmp647;(_tmp647=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp648="UnresolvedMem_e",
_tag_dyneither(_tmp648,sizeof(char),16))),_tag_dyneither(_tmp647,sizeof(void*),0)));}
_LL26A: {struct Cyc_Absyn_StmtExp_e_struct*_tmp3EC=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp382;if(_tmp3EC->tag != 37)goto _LL26C;else{_tmp3ED=_tmp3EC->f1;}}_LL26B: {
const char*_tmp64B;void*_tmp64A;(_tmp64A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp64B="StmtExp_e",_tag_dyneither(
_tmp64B,sizeof(char),10))),_tag_dyneither(_tmp64A,sizeof(void*),0)));}_LL26C: {
struct Cyc_Absyn_Valueof_e_struct*_tmp3EE=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp382;if(_tmp3EE->tag != 39)goto _LL26E;}_LL26D: {const char*_tmp64E;void*_tmp64D;(
_tmp64D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp64E="Valueof_e",_tag_dyneither(_tmp64E,sizeof(char),10))),_tag_dyneither(
_tmp64D,sizeof(void*),0)));}_LL26E: {struct Cyc_Absyn_Asm_e_struct*_tmp3EF=(
struct Cyc_Absyn_Asm_e_struct*)_tmp382;if(_tmp3EF->tag != 40)goto _LL270;}_LL26F: {
const char*_tmp651;void*_tmp650;(_tmp650=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp651="Asm_e",_tag_dyneither(
_tmp651,sizeof(char),6))),_tag_dyneither(_tmp650,sizeof(void*),0)));}_LL270: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp3F0=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp382;if(_tmp3F0->tag != 38)goto _LL20D;}_LL271: {const char*_tmp654;void*_tmp653;(
_tmp653=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp654="Tagcheck_e",_tag_dyneither(_tmp654,sizeof(char),11))),_tag_dyneither(
_tmp653,sizeof(void*),0)));}_LL20D:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s){void*
_tmp438=s->r;struct Cyc_Absyn_Exp*_tmp43B;struct Cyc_Absyn_Stmt*_tmp43D;struct Cyc_Absyn_Stmt*
_tmp43E;struct Cyc_Absyn_Exp*_tmp440;struct Cyc_Absyn_Exp*_tmp442;struct Cyc_Absyn_Stmt*
_tmp443;struct Cyc_Absyn_Stmt*_tmp444;struct _tuple8 _tmp446;struct Cyc_Absyn_Exp*
_tmp447;struct Cyc_Absyn_Stmt*_tmp448;struct Cyc_Absyn_Exp*_tmp44D;struct _tuple8
_tmp44E;struct Cyc_Absyn_Exp*_tmp44F;struct _tuple8 _tmp450;struct Cyc_Absyn_Exp*
_tmp451;struct Cyc_Absyn_Stmt*_tmp452;struct Cyc_Absyn_Exp*_tmp454;struct Cyc_List_List*
_tmp455;struct Cyc_Absyn_Decl*_tmp458;struct Cyc_Absyn_Stmt*_tmp459;struct Cyc_Absyn_Stmt*
_tmp45B;struct Cyc_Absyn_Stmt*_tmp45D;struct _tuple8 _tmp45E;struct Cyc_Absyn_Exp*
_tmp45F;_LL273: {struct Cyc_Absyn_Skip_s_struct*_tmp439=(struct Cyc_Absyn_Skip_s_struct*)
_tmp438;if(_tmp439->tag != 0)goto _LL275;}_LL274: return;_LL275: {struct Cyc_Absyn_Exp_s_struct*
_tmp43A=(struct Cyc_Absyn_Exp_s_struct*)_tmp438;if(_tmp43A->tag != 1)goto _LL277;
else{_tmp43B=_tmp43A->f1;}}_LL276: Cyc_Port_gen_exp(env,_tmp43B);return;_LL277: {
struct Cyc_Absyn_Seq_s_struct*_tmp43C=(struct Cyc_Absyn_Seq_s_struct*)_tmp438;if(
_tmp43C->tag != 2)goto _LL279;else{_tmp43D=_tmp43C->f1;_tmp43E=_tmp43C->f2;}}
_LL278: Cyc_Port_gen_stmt(env,_tmp43D);Cyc_Port_gen_stmt(env,_tmp43E);return;
_LL279: {struct Cyc_Absyn_Return_s_struct*_tmp43F=(struct Cyc_Absyn_Return_s_struct*)
_tmp438;if(_tmp43F->tag != 3)goto _LL27B;else{_tmp440=_tmp43F->f1;}}_LL27A: {void*
_tmp462=Cyc_Port_lookup_return_type(env);void*_tmp463=(unsigned int)_tmp440?Cyc_Port_gen_exp(
env,(struct Cyc_Absyn_Exp*)_tmp440): Cyc_Port_void_ct();Cyc_Port_leq(_tmp463,
_tmp462);return;}_LL27B: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp441=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp438;if(_tmp441->tag != 4)goto _LL27D;else{_tmp442=_tmp441->f1;_tmp443=_tmp441->f2;
_tmp444=_tmp441->f3;}}_LL27C: Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp442),Cyc_Port_arith_ct());
Cyc_Port_gen_stmt(env,_tmp443);Cyc_Port_gen_stmt(env,_tmp444);return;_LL27D: {
struct Cyc_Absyn_While_s_struct*_tmp445=(struct Cyc_Absyn_While_s_struct*)_tmp438;
if(_tmp445->tag != 5)goto _LL27F;else{_tmp446=_tmp445->f1;_tmp447=_tmp446.f1;
_tmp448=_tmp445->f2;}}_LL27E: Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp447),Cyc_Port_arith_ct());
Cyc_Port_gen_stmt(env,_tmp448);return;_LL27F: {struct Cyc_Absyn_Break_s_struct*
_tmp449=(struct Cyc_Absyn_Break_s_struct*)_tmp438;if(_tmp449->tag != 6)goto _LL281;}
_LL280: goto _LL282;_LL281: {struct Cyc_Absyn_Continue_s_struct*_tmp44A=(struct Cyc_Absyn_Continue_s_struct*)
_tmp438;if(_tmp44A->tag != 7)goto _LL283;}_LL282: goto _LL284;_LL283: {struct Cyc_Absyn_Goto_s_struct*
_tmp44B=(struct Cyc_Absyn_Goto_s_struct*)_tmp438;if(_tmp44B->tag != 8)goto _LL285;}
_LL284: return;_LL285: {struct Cyc_Absyn_For_s_struct*_tmp44C=(struct Cyc_Absyn_For_s_struct*)
_tmp438;if(_tmp44C->tag != 9)goto _LL287;else{_tmp44D=_tmp44C->f1;_tmp44E=_tmp44C->f2;
_tmp44F=_tmp44E.f1;_tmp450=_tmp44C->f3;_tmp451=_tmp450.f1;_tmp452=_tmp44C->f4;}}
_LL286: Cyc_Port_gen_exp(env,_tmp44D);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp44F),
Cyc_Port_arith_ct());Cyc_Port_gen_exp(env,_tmp451);Cyc_Port_gen_stmt(env,_tmp452);
return;_LL287: {struct Cyc_Absyn_Switch_s_struct*_tmp453=(struct Cyc_Absyn_Switch_s_struct*)
_tmp438;if(_tmp453->tag != 10)goto _LL289;else{_tmp454=_tmp453->f1;_tmp455=_tmp453->f2;}}
_LL288: Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp454),Cyc_Port_arith_ct());for(0;(
unsigned int)_tmp455;_tmp455=_tmp455->tl){Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)
_tmp455->hd)->body);}return;_LL289: {struct Cyc_Absyn_Fallthru_s_struct*_tmp456=(
struct Cyc_Absyn_Fallthru_s_struct*)_tmp438;if(_tmp456->tag != 11)goto _LL28B;}
_LL28A: {const char*_tmp657;void*_tmp656;(_tmp656=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp657="fallthru",
_tag_dyneither(_tmp657,sizeof(char),9))),_tag_dyneither(_tmp656,sizeof(void*),0)));}
_LL28B: {struct Cyc_Absyn_Decl_s_struct*_tmp457=(struct Cyc_Absyn_Decl_s_struct*)
_tmp438;if(_tmp457->tag != 12)goto _LL28D;else{_tmp458=_tmp457->f1;_tmp459=_tmp457->f2;}}
_LL28C: env=Cyc_Port_gen_localdecl(env,_tmp458);Cyc_Port_gen_stmt(env,_tmp459);
return;_LL28D: {struct Cyc_Absyn_Label_s_struct*_tmp45A=(struct Cyc_Absyn_Label_s_struct*)
_tmp438;if(_tmp45A->tag != 13)goto _LL28F;else{_tmp45B=_tmp45A->f2;}}_LL28E: Cyc_Port_gen_stmt(
env,_tmp45B);return;_LL28F: {struct Cyc_Absyn_Do_s_struct*_tmp45C=(struct Cyc_Absyn_Do_s_struct*)
_tmp438;if(_tmp45C->tag != 14)goto _LL291;else{_tmp45D=_tmp45C->f1;_tmp45E=_tmp45C->f2;
_tmp45F=_tmp45E.f1;}}_LL290: Cyc_Port_gen_stmt(env,_tmp45D);Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp45F),Cyc_Port_arith_ct());return;_LL291: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp460=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp438;if(_tmp460->tag != 15)goto
_LL293;}_LL292: {const char*_tmp65A;void*_tmp659;(_tmp659=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp65A="try/catch",
_tag_dyneither(_tmp65A,sizeof(char),10))),_tag_dyneither(_tmp659,sizeof(void*),0)));}
_LL293: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp461=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp438;if(_tmp461->tag != 16)goto _LL272;}_LL294: {const char*_tmp65D;void*_tmp65C;(
_tmp65C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp65D="reset region",_tag_dyneither(_tmp65D,sizeof(char),13))),_tag_dyneither(
_tmp65C,sizeof(void*),0)));}_LL272:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Port_gen_initializer(
struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){void*_tmp46A=e->r;struct
Cyc_List_List*_tmp46C;struct Cyc_List_List*_tmp46E;struct Cyc_List_List*_tmp470;
union Cyc_Absyn_Cnst _tmp472;struct _dyneither_ptr _tmp473;union Cyc_Absyn_Cnst
_tmp475;struct _dyneither_ptr _tmp476;_LL296: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp46B=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp46A;if(_tmp46B->tag != 36)
goto _LL298;else{_tmp46C=_tmp46B->f2;}}_LL297: _tmp46E=_tmp46C;goto _LL299;_LL298: {
struct Cyc_Absyn_Array_e_struct*_tmp46D=(struct Cyc_Absyn_Array_e_struct*)_tmp46A;
if(_tmp46D->tag != 27)goto _LL29A;else{_tmp46E=_tmp46D->f1;}}_LL299: _tmp470=
_tmp46E;goto _LL29B;_LL29A: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp46F=(struct
Cyc_Absyn_CompoundLit_e_struct*)_tmp46A;if(_tmp46F->tag != 26)goto _LL29C;else{
_tmp470=_tmp46F->f2;}}_LL29B: LOOP: {void*_tmp477=t;struct _tuple0*_tmp479;struct
Cyc_Absyn_ArrayInfo _tmp47B;void*_tmp47C;union Cyc_Absyn_Constraint*_tmp47D;struct
Cyc_Position_Segment*_tmp47E;struct Cyc_Absyn_AggrInfo _tmp480;union Cyc_Absyn_AggrInfoU
_tmp481;struct _tuple2 _tmp482;enum Cyc_Absyn_AggrKind _tmp483;struct _tuple0*
_tmp484;struct Cyc_Absyn_AggrInfo _tmp486;union Cyc_Absyn_AggrInfoU _tmp487;struct
Cyc_Absyn_Aggrdecl**_tmp488;struct Cyc_Absyn_Aggrdecl*_tmp489;_LL2A3: {struct Cyc_Absyn_TypedefType_struct*
_tmp478=(struct Cyc_Absyn_TypedefType_struct*)_tmp477;if(_tmp478->tag != 18)goto
_LL2A5;else{_tmp479=_tmp478->f1;}}_LL2A4:(*_tmp479).f1=Cyc_Absyn_Loc_n;t=Cyc_Port_lookup_typedef(
env,_tmp479);goto LOOP;_LL2A5: {struct Cyc_Absyn_ArrayType_struct*_tmp47A=(struct
Cyc_Absyn_ArrayType_struct*)_tmp477;if(_tmp47A->tag != 9)goto _LL2A7;else{_tmp47B=
_tmp47A->f1;_tmp47C=_tmp47B.elt_type;_tmp47D=_tmp47B.zero_term;_tmp47E=_tmp47B.zt_loc;}}
_LL2A6: {void*_tmp48A=Cyc_Port_var();void*_tmp48B=Cyc_Port_var();void*_tmp48C=
Cyc_Port_var();void*_tmp48D=Cyc_Port_type_to_ctype(env,_tmp47C);for(0;_tmp470 != 
0;_tmp470=_tmp470->tl){struct Cyc_List_List*_tmp48F;struct Cyc_Absyn_Exp*_tmp490;
struct _tuple14 _tmp48E=*((struct _tuple14*)_tmp470->hd);_tmp48F=_tmp48E.f1;_tmp490=
_tmp48E.f2;if((unsigned int)_tmp48F){const char*_tmp660;void*_tmp65F;(_tmp65F=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp660="designators in initializers",_tag_dyneither(_tmp660,sizeof(char),28))),
_tag_dyneither(_tmp65F,sizeof(void*),0)));}{void*_tmp493=Cyc_Port_gen_initializer(
env,_tmp47C,_tmp490);Cyc_Port_leq(_tmp493,_tmp48A);};}return Cyc_Port_array_ct(
_tmp48A,_tmp48B,_tmp48C);}_LL2A7: {struct Cyc_Absyn_AggrType_struct*_tmp47F=(
struct Cyc_Absyn_AggrType_struct*)_tmp477;if(_tmp47F->tag != 12)goto _LL2A9;else{
_tmp480=_tmp47F->f1;_tmp481=_tmp480.aggr_info;if((_tmp481.UnknownAggr).tag != 1)
goto _LL2A9;_tmp482=(struct _tuple2)(_tmp481.UnknownAggr).val;_tmp483=_tmp482.f1;
if(_tmp483 != Cyc_Absyn_StructA)goto _LL2A9;_tmp484=_tmp482.f2;}}_LL2A8:(*_tmp484).f1=
Cyc_Absyn_Loc_n;{struct Cyc_Absyn_Aggrdecl*_tmp495;struct _tuple10 _tmp494=*Cyc_Port_lookup_struct_decl(
env,_tmp484);_tmp495=_tmp494.f1;if((struct Cyc_Absyn_Aggrdecl*)_tmp495 == 0){const
char*_tmp663;void*_tmp662;(_tmp662=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp663="struct is not yet defined",
_tag_dyneither(_tmp663,sizeof(char),26))),_tag_dyneither(_tmp662,sizeof(void*),0)));}
_tmp489=_tmp495;goto _LL2AA;};_LL2A9: {struct Cyc_Absyn_AggrType_struct*_tmp485=(
struct Cyc_Absyn_AggrType_struct*)_tmp477;if(_tmp485->tag != 12)goto _LL2AB;else{
_tmp486=_tmp485->f1;_tmp487=_tmp486.aggr_info;if((_tmp487.KnownAggr).tag != 2)
goto _LL2AB;_tmp488=(struct Cyc_Absyn_Aggrdecl**)(_tmp487.KnownAggr).val;_tmp489=*
_tmp488;}}_LL2AA: {struct Cyc_List_List*_tmp498=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp489->impl))->fields;for(0;_tmp470 != 0;_tmp470=_tmp470->tl){
struct Cyc_List_List*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;struct _tuple14 _tmp499=*((
struct _tuple14*)_tmp470->hd);_tmp49A=_tmp499.f1;_tmp49B=_tmp499.f2;if((
unsigned int)_tmp49A){const char*_tmp666;void*_tmp665;(_tmp665=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp666="designators in initializers",
_tag_dyneither(_tmp666,sizeof(char),28))),_tag_dyneither(_tmp665,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrfield*_tmp49E=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)
_check_null(_tmp498))->hd;_tmp498=_tmp498->tl;{void*_tmp49F=Cyc_Port_gen_initializer(
env,_tmp49E->type,_tmp49B);;};};}return Cyc_Port_type_to_ctype(env,t);}_LL2AB:;
_LL2AC: {const char*_tmp669;void*_tmp668;(_tmp668=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp669="bad type for aggregate initializer",
_tag_dyneither(_tmp669,sizeof(char),35))),_tag_dyneither(_tmp668,sizeof(void*),0)));}
_LL2A2:;}_LL29C: {struct Cyc_Absyn_Const_e_struct*_tmp471=(struct Cyc_Absyn_Const_e_struct*)
_tmp46A;if(_tmp471->tag != 0)goto _LL29E;else{_tmp472=_tmp471->f1;if((_tmp472.String_c).tag
!= 8)goto _LL29E;_tmp473=(struct _dyneither_ptr)(_tmp472.String_c).val;}}_LL29D:
goto _LL29F;_LL29E: {struct Cyc_Absyn_Const_e_struct*_tmp474=(struct Cyc_Absyn_Const_e_struct*)
_tmp46A;if(_tmp474->tag != 0)goto _LL2A0;else{_tmp475=_tmp474->f1;if((_tmp475.Wstring_c).tag
!= 9)goto _LL2A0;_tmp476=(struct _dyneither_ptr)(_tmp475.Wstring_c).val;}}_LL29F:
LOOP2: {void*_tmp4A2=t;struct _tuple0*_tmp4A4;_LL2AE: {struct Cyc_Absyn_TypedefType_struct*
_tmp4A3=(struct Cyc_Absyn_TypedefType_struct*)_tmp4A2;if(_tmp4A3->tag != 18)goto
_LL2B0;else{_tmp4A4=_tmp4A3->f1;}}_LL2AF:(*_tmp4A4).f1=Cyc_Absyn_Loc_n;t=Cyc_Port_lookup_typedef(
env,_tmp4A4);goto LOOP2;_LL2B0: {struct Cyc_Absyn_ArrayType_struct*_tmp4A5=(struct
Cyc_Absyn_ArrayType_struct*)_tmp4A2;if(_tmp4A5->tag != 9)goto _LL2B2;}_LL2B1:
return Cyc_Port_array_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_zterm_ct());
_LL2B2:;_LL2B3: return Cyc_Port_gen_exp(env,e);_LL2AD:;}_LL2A0:;_LL2A1: return Cyc_Port_gen_exp(
env,e);_LL295:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*
env,struct Cyc_Absyn_Decl*d){void*_tmp4A6=d->r;struct Cyc_Absyn_Vardecl*_tmp4A8;
_LL2B5: {struct Cyc_Absyn_Var_d_struct*_tmp4A7=(struct Cyc_Absyn_Var_d_struct*)
_tmp4A6;if(_tmp4A7->tag != 0)goto _LL2B7;else{_tmp4A8=_tmp4A7->f1;}}_LL2B6: {void*
_tmp4A9=Cyc_Port_var();void*q;if((env->gcenv)->porting){q=Cyc_Port_var();Cyc_Port_register_const_cvar(
env,q,(_tmp4A8->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(
_tmp4A8->tq).loc);}else{q=(_tmp4A8->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}(*
_tmp4A8->name).f1=Cyc_Absyn_Loc_n;env=Cyc_Port_add_var(env,_tmp4A8->name,_tmp4A8->type,
q,_tmp4A9);Cyc_Port_unifies(_tmp4A9,Cyc_Port_type_to_ctype(env,_tmp4A8->type));
if((unsigned int)_tmp4A8->initializer){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_check_null(_tmp4A8->initializer);void*t2=Cyc_Port_gen_initializer(env,_tmp4A8->type,
e);Cyc_Port_leq(t2,_tmp4A9);}return env;}_LL2B7:;_LL2B8: {const char*_tmp66C;void*
_tmp66B;(_tmp66B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp66C="non-local decl that isn't a variable",_tag_dyneither(_tmp66C,sizeof(
char),37))),_tag_dyneither(_tmp66B,sizeof(void*),0)));}_LL2B4:;}struct _tuple15{
struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple7*
Cyc_Port_make_targ(struct _tuple15*arg){struct _dyneither_ptr*_tmp4AD;struct Cyc_Absyn_Tqual
_tmp4AE;void*_tmp4AF;struct _tuple15 _tmp4AC=*arg;_tmp4AD=_tmp4AC.f1;_tmp4AE=
_tmp4AC.f2;_tmp4AF=_tmp4AC.f3;{struct _tuple7*_tmp66D;return(_tmp66D=_cycalloc(
sizeof(*_tmp66D)),((_tmp66D->f1=0,((_tmp66D->f2=_tmp4AE,((_tmp66D->f3=_tmp4AF,
_tmp66D)))))));};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*
Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds);struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple16{
struct _dyneither_ptr*f1;void*f2;void*f3;void*f4;};struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*
env,struct Cyc_Absyn_Decl*d){void*_tmp4B1=d->r;struct Cyc_Absyn_Vardecl*_tmp4B5;
struct Cyc_Absyn_Fndecl*_tmp4B7;struct Cyc_Absyn_Typedefdecl*_tmp4B9;struct Cyc_Absyn_Aggrdecl*
_tmp4BB;struct Cyc_Absyn_Enumdecl*_tmp4BD;_LL2BA: {struct Cyc_Absyn_Porton_d_struct*
_tmp4B2=(struct Cyc_Absyn_Porton_d_struct*)_tmp4B1;if(_tmp4B2->tag != 14)goto
_LL2BC;}_LL2BB:(env->gcenv)->porting=1;return env;_LL2BC: {struct Cyc_Absyn_Portoff_d_struct*
_tmp4B3=(struct Cyc_Absyn_Portoff_d_struct*)_tmp4B1;if(_tmp4B3->tag != 15)goto
_LL2BE;}_LL2BD:(env->gcenv)->porting=0;return env;_LL2BE: {struct Cyc_Absyn_Var_d_struct*
_tmp4B4=(struct Cyc_Absyn_Var_d_struct*)_tmp4B1;if(_tmp4B4->tag != 0)goto _LL2C0;
else{_tmp4B5=_tmp4B4->f1;}}_LL2BF:(*_tmp4B5->name).f1=Cyc_Absyn_Loc_n;if(Cyc_Port_declared_var(
env,_tmp4B5->name)){void*_tmp4BF;void*_tmp4C0;struct _tuple9 _tmp4BE=Cyc_Port_lookup_var(
env,_tmp4B5->name);_tmp4BF=_tmp4BE.f1;_tmp4C0=_tmp4BE.f2;{void*q2;if((env->gcenv)->porting
 && !Cyc_Port_isfn(env,_tmp4B5->name)){q2=Cyc_Port_var();Cyc_Port_register_const_cvar(
env,q2,(_tmp4B5->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(
_tmp4B5->tq).loc);}else{q2=(_tmp4B5->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
Cyc_Port_unifies(_tmp4BF,q2);Cyc_Port_unifies(_tmp4C0,Cyc_Port_type_to_ctype(env,
_tmp4B5->type));if((unsigned int)_tmp4B5->initializer){struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)_check_null(_tmp4B5->initializer);Cyc_Port_leq(Cyc_Port_gen_initializer(
env,_tmp4B5->type,e),_tmp4C0);}return env;};}else{return Cyc_Port_gen_localdecl(
env,d);}_LL2C0: {struct Cyc_Absyn_Fn_d_struct*_tmp4B6=(struct Cyc_Absyn_Fn_d_struct*)
_tmp4B1;if(_tmp4B6->tag != 1)goto _LL2C2;else{_tmp4B7=_tmp4B6->f1;}}_LL2C1:(*
_tmp4B7->name).f1=Cyc_Absyn_Loc_n;{struct _tuple12*predeclared=0;if(Cyc_Port_declared_var(
env,_tmp4B7->name))predeclared=(struct _tuple12*)Cyc_Port_lookup_full_var(env,
_tmp4B7->name);{void*_tmp4C1=_tmp4B7->ret_type;struct Cyc_List_List*_tmp4C2=
_tmp4B7->args;struct Cyc_List_List*_tmp4C3=((struct Cyc_List_List*(*)(struct
_tuple7*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_make_targ,
_tmp4C2);struct Cyc_Absyn_FnType_struct _tmp673;struct Cyc_Absyn_FnInfo _tmp672;
struct Cyc_Absyn_FnType_struct*_tmp671;struct Cyc_Absyn_FnType_struct*_tmp4C4=(
_tmp671=_cycalloc(sizeof(*_tmp671)),((_tmp671[0]=((_tmp673.tag=10,((_tmp673.f1=((
_tmp672.tvars=0,((_tmp672.effect=0,((_tmp672.ret_tqual=Cyc_Absyn_empty_tqual(0),((
_tmp672.ret_typ=_tmp4C1,((_tmp672.args=_tmp4C3,((_tmp672.c_varargs=0,((_tmp672.cyc_varargs=
0,((_tmp672.rgn_po=0,((_tmp672.attributes=0,_tmp672)))))))))))))))))),_tmp673)))),
_tmp671)));struct Cyc_Port_Cenv*_tmp4C5=Cyc_Port_set_cpos(env,Cyc_Port_FnBody_pos);
void*_tmp4C6=Cyc_Port_type_to_ctype(_tmp4C5,_tmp4C1);struct Cyc_List_List*c_args=
0;struct Cyc_List_List*c_arg_types=0;{struct Cyc_List_List*_tmp4C7=_tmp4C2;for(0;(
unsigned int)_tmp4C7;_tmp4C7=_tmp4C7->tl){struct _dyneither_ptr*_tmp4C9;struct Cyc_Absyn_Tqual
_tmp4CA;void*_tmp4CB;struct _tuple15 _tmp4C8=*((struct _tuple15*)_tmp4C7->hd);
_tmp4C9=_tmp4C8.f1;_tmp4CA=_tmp4C8.f2;_tmp4CB=_tmp4C8.f3;{void*_tmp4CC=Cyc_Port_type_to_ctype(
_tmp4C5,_tmp4CB);void*tqv;if((env->gcenv)->porting){tqv=Cyc_Port_var();Cyc_Port_register_const_cvar(
env,tqv,_tmp4CA.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp4CA.loc);}
else{tqv=_tmp4CA.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}{struct
_tuple16*_tmp676;struct Cyc_List_List*_tmp675;c_args=((_tmp675=_cycalloc(sizeof(*
_tmp675)),((_tmp675->hd=((_tmp676=_cycalloc(sizeof(*_tmp676)),((_tmp676->f1=
_tmp4C9,((_tmp676->f2=_tmp4CB,((_tmp676->f3=tqv,((_tmp676->f4=_tmp4CC,_tmp676)))))))))),((
_tmp675->tl=c_args,_tmp675))))));}{struct Cyc_List_List*_tmp677;c_arg_types=((
_tmp677=_cycalloc(sizeof(*_tmp677)),((_tmp677->hd=_tmp4CC,((_tmp677->tl=
c_arg_types,_tmp677))))));};};}}c_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(c_args);c_arg_types=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(c_arg_types);{void*_tmp4D0=Cyc_Port_fn_ct(_tmp4C6,
c_arg_types);(*_tmp4B7->name).f1=Cyc_Absyn_Loc_n;_tmp4C5=Cyc_Port_add_var(
_tmp4C5,_tmp4B7->name,(void*)_tmp4C4,Cyc_Port_const_ct(),_tmp4D0);Cyc_Port_add_return_type(
_tmp4C5,_tmp4C6);{struct Cyc_List_List*_tmp4D1=c_args;for(0;(unsigned int)_tmp4D1;
_tmp4D1=_tmp4D1->tl){struct _dyneither_ptr*_tmp4D3;void*_tmp4D4;void*_tmp4D5;void*
_tmp4D6;struct _tuple16 _tmp4D2=*((struct _tuple16*)_tmp4D1->hd);_tmp4D3=_tmp4D2.f1;
_tmp4D4=_tmp4D2.f2;_tmp4D5=_tmp4D2.f3;_tmp4D6=_tmp4D2.f4;{struct _tuple0*_tmp678;
_tmp4C5=Cyc_Port_add_var(_tmp4C5,((_tmp678=_cycalloc(sizeof(*_tmp678)),((_tmp678->f1=
Cyc_Absyn_Loc_n,((_tmp678->f2=_tmp4D3,_tmp678)))))),_tmp4D4,_tmp4D5,_tmp4D6);};}}
Cyc_Port_gen_stmt(_tmp4C5,_tmp4B7->body);Cyc_Port_generalize(0,_tmp4D0);{struct
Cyc_Dict_Dict counts=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);Cyc_Port_region_counts(&
counts,_tmp4D0);Cyc_Port_register_rgns(env,counts,1,(void*)_tmp4C4,_tmp4D0);env=
Cyc_Port_add_var(_tmp4C5,_tmp4B7->name,(void*)_tmp4C4,Cyc_Port_const_ct(),
_tmp4D0);if((unsigned int)predeclared){void*_tmp4D9;struct _tuple9*_tmp4DA;struct
_tuple9 _tmp4DB;void*_tmp4DC;void*_tmp4DD;struct _tuple12 _tmp4D8=*predeclared;
_tmp4D9=_tmp4D8.f1;_tmp4DA=_tmp4D8.f2;_tmp4DB=*_tmp4DA;_tmp4DC=_tmp4DB.f1;
_tmp4DD=_tmp4DB.f2;Cyc_Port_unifies(_tmp4DC,Cyc_Port_const_ct());Cyc_Port_unifies(
_tmp4DD,Cyc_Port_instantiate(_tmp4D0));Cyc_Port_register_rgns(env,counts,1,
_tmp4D9,_tmp4D0);}return env;};};};};_LL2C2: {struct Cyc_Absyn_Typedef_d_struct*
_tmp4B8=(struct Cyc_Absyn_Typedef_d_struct*)_tmp4B1;if(_tmp4B8->tag != 9)goto
_LL2C4;else{_tmp4B9=_tmp4B8->f1;}}_LL2C3: {void*_tmp4E1=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4B9->defn))->v;void*_tmp4E2=Cyc_Port_type_to_ctype(env,_tmp4E1);(*
_tmp4B9->name).f1=Cyc_Absyn_Loc_n;Cyc_Port_add_typedef(env,_tmp4B9->name,_tmp4E1,
_tmp4E2);return env;}_LL2C4: {struct Cyc_Absyn_Aggr_d_struct*_tmp4BA=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4B1;if(_tmp4BA->tag != 6)goto _LL2C6;else{_tmp4BB=_tmp4BA->f1;}}_LL2C5: {
struct _tuple0*_tmp4E3=_tmp4BB->name;(*_tmp4BB->name).f1=Cyc_Absyn_Loc_n;{struct
_tuple10*previous;struct Cyc_List_List*curr=0;switch(_tmp4BB->kind){case Cyc_Absyn_StructA:
_LL2CA: previous=Cyc_Port_lookup_struct_decl(env,_tmp4E3);break;case Cyc_Absyn_UnionA:
_LL2CB: previous=Cyc_Port_lookup_union_decl(env,_tmp4E3);break;}if((unsigned int)
_tmp4BB->impl){struct Cyc_List_List*cf=(*previous).f2;{struct Cyc_List_List*
_tmp4E4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BB->impl))->fields;for(
0;(unsigned int)_tmp4E4;_tmp4E4=_tmp4E4->tl){struct Cyc_Absyn_Aggrfield*_tmp4E5=(
struct Cyc_Absyn_Aggrfield*)_tmp4E4->hd;void*qv;if((env->gcenv)->porting){qv=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,qv,(_tmp4E5->tq).print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct(),(_tmp4E5->tq).loc);}else{qv=(_tmp4E5->tq).print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct();}{void*_tmp4E6=Cyc_Port_type_to_ctype(env,_tmp4E5->type);
if(cf != 0){void*_tmp4E8;void*_tmp4E9;struct Cyc_Port_Cfield _tmp4E7=*((struct Cyc_Port_Cfield*)
cf->hd);_tmp4E8=_tmp4E7.qual;_tmp4E9=_tmp4E7.type;cf=cf->tl;Cyc_Port_unifies(qv,
_tmp4E8);Cyc_Port_unifies(_tmp4E6,_tmp4E9);}{struct Cyc_Port_Cfield*_tmp67B;
struct Cyc_List_List*_tmp67A;curr=((_tmp67A=_cycalloc(sizeof(*_tmp67A)),((_tmp67A->hd=((
_tmp67B=_cycalloc(sizeof(*_tmp67B)),((_tmp67B->qual=qv,((_tmp67B->f=_tmp4E5->name,((
_tmp67B->type=_tmp4E6,_tmp67B)))))))),((_tmp67A->tl=curr,_tmp67A))))));};};}}
curr=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(curr);
if((*previous).f2 == 0)(*previous).f2=curr;}return env;};}_LL2C6: {struct Cyc_Absyn_Enum_d_struct*
_tmp4BC=(struct Cyc_Absyn_Enum_d_struct*)_tmp4B1;if(_tmp4BC->tag != 8)goto _LL2C8;
else{_tmp4BD=_tmp4BC->f1;}}_LL2C7:(*_tmp4BD->name).f1=Cyc_Absyn_Loc_n;if((
unsigned int)_tmp4BD->fields){struct Cyc_List_List*_tmp4EC=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4BD->fields))->v;for(0;(unsigned int)_tmp4EC;
_tmp4EC=_tmp4EC->tl){(*((struct Cyc_Absyn_Enumfield*)_tmp4EC->hd)->name).f1=Cyc_Absyn_Loc_n;{
struct Cyc_Absyn_EnumType_struct _tmp67E;struct Cyc_Absyn_EnumType_struct*_tmp67D;
env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)_tmp4EC->hd)->name,(void*)((
_tmp67D=_cycalloc(sizeof(*_tmp67D)),((_tmp67D[0]=((_tmp67E.tag=14,((_tmp67E.f1=
_tmp4BD->name,((_tmp67E.f2=(struct Cyc_Absyn_Enumdecl*)_tmp4BD,_tmp67E)))))),
_tmp67D)))),Cyc_Port_const_ct(),Cyc_Port_arith_ct());};}}return env;_LL2C8:;
_LL2C9: if((env->gcenv)->porting){const char*_tmp681;void*_tmp680;(_tmp680=0,Cyc_fprintf(
Cyc_stderr,((_tmp681="Warning: Cyclone declaration found in code to be ported -- skipping.",
_tag_dyneither(_tmp681,sizeof(char),69))),_tag_dyneither(_tmp680,sizeof(void*),0)));}
return env;_LL2B9:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*
env,struct Cyc_List_List*ds){for(0;(unsigned int)ds;ds=ds->tl){env=Cyc_Port_gen_topdecl(
env,(struct Cyc_Absyn_Decl*)ds->hd);}return env;}struct Cyc_Port_Cenv;struct Cyc_Port_Edit;
struct Cyc_Port_Edit;struct Cyc_Port_Edit;static struct Cyc_List_List*Cyc_Port_gen_edits(
struct Cyc_List_List*ds){struct Cyc_Port_Cenv*env=Cyc_Port_gen_topdecls(Cyc_Port_initial_cenv(),
ds);struct Cyc_List_List*_tmp4F1=(env->gcenv)->pointer_edits;struct Cyc_List_List*
_tmp4F2=(env->gcenv)->qualifier_edits;struct Cyc_List_List*_tmp4F3=(env->gcenv)->zeroterm_edits;
struct Cyc_List_List*_tmp4F4=(env->gcenv)->edits;{struct Cyc_List_List*_tmp4F5=
_tmp4F1;for(0;(unsigned int)_tmp4F5;_tmp4F5=_tmp4F5->tl){void*_tmp4F7;void*
_tmp4F8;struct Cyc_Position_Segment*_tmp4F9;struct _tuple13 _tmp4F6=*((struct
_tuple13*)_tmp4F5->hd);_tmp4F7=_tmp4F6.f1;_tmp4F8=_tmp4F6.f2;_tmp4F9=_tmp4F6.f3;
Cyc_Port_unifies(_tmp4F7,_tmp4F8);}}{struct Cyc_List_List*_tmp4FA=_tmp4F2;for(0;(
unsigned int)_tmp4FA;_tmp4FA=_tmp4FA->tl){void*_tmp4FC;void*_tmp4FD;struct Cyc_Position_Segment*
_tmp4FE;struct _tuple13 _tmp4FB=*((struct _tuple13*)_tmp4FA->hd);_tmp4FC=_tmp4FB.f1;
_tmp4FD=_tmp4FB.f2;_tmp4FE=_tmp4FB.f3;Cyc_Port_unifies(_tmp4FC,_tmp4FD);}}{
struct Cyc_List_List*_tmp4FF=_tmp4F3;for(0;(unsigned int)_tmp4FF;_tmp4FF=_tmp4FF->tl){
void*_tmp501;void*_tmp502;struct Cyc_Position_Segment*_tmp503;struct _tuple13
_tmp500=*((struct _tuple13*)_tmp4FF->hd);_tmp501=_tmp500.f1;_tmp502=_tmp500.f2;
_tmp503=_tmp500.f3;Cyc_Port_unifies(_tmp501,_tmp502);}}for(0;(unsigned int)
_tmp4F1;_tmp4F1=_tmp4F1->tl){void*_tmp505;void*_tmp506;struct Cyc_Position_Segment*
_tmp507;struct _tuple13 _tmp504=*((struct _tuple13*)_tmp4F1->hd);_tmp505=_tmp504.f1;
_tmp506=_tmp504.f2;_tmp507=_tmp504.f3;if(!Cyc_Port_unifies(_tmp505,_tmp506) && (
unsigned int)_tmp507){void*_tmp508=Cyc_Port_compress_ct(_tmp506);_LL2CE: {struct
Cyc_Port_Thin_ct_struct*_tmp509=(struct Cyc_Port_Thin_ct_struct*)_tmp508;if(
_tmp509->tag != 2)goto _LL2D0;}_LL2CF:{struct Cyc_Port_Edit*_tmp68A;const char*
_tmp689;const char*_tmp688;struct Cyc_List_List*_tmp687;_tmp4F4=((_tmp687=
_cycalloc(sizeof(*_tmp687)),((_tmp687->hd=((_tmp68A=_cycalloc(sizeof(*_tmp68A)),((
_tmp68A->loc=_tmp507,((_tmp68A->old_string=((_tmp689="?",_tag_dyneither(_tmp689,
sizeof(char),2))),((_tmp68A->new_string=((_tmp688="*",_tag_dyneither(_tmp688,
sizeof(char),2))),_tmp68A)))))))),((_tmp687->tl=_tmp4F4,_tmp687))))));}goto
_LL2CD;_LL2D0: {struct Cyc_Port_Fat_ct_struct*_tmp50A=(struct Cyc_Port_Fat_ct_struct*)
_tmp508;if(_tmp50A->tag != 3)goto _LL2D2;}_LL2D1:{struct Cyc_Port_Edit*_tmp693;
const char*_tmp692;const char*_tmp691;struct Cyc_List_List*_tmp690;_tmp4F4=((
_tmp690=_cycalloc(sizeof(*_tmp690)),((_tmp690->hd=((_tmp693=_cycalloc(sizeof(*
_tmp693)),((_tmp693->loc=_tmp507,((_tmp693->old_string=((_tmp692="*",
_tag_dyneither(_tmp692,sizeof(char),2))),((_tmp693->new_string=((_tmp691="?",
_tag_dyneither(_tmp691,sizeof(char),2))),_tmp693)))))))),((_tmp690->tl=_tmp4F4,
_tmp690))))));}goto _LL2CD;_LL2D2:;_LL2D3: goto _LL2CD;_LL2CD:;}}for(0;(
unsigned int)_tmp4F2;_tmp4F2=_tmp4F2->tl){void*_tmp514;void*_tmp515;struct Cyc_Position_Segment*
_tmp516;struct _tuple13 _tmp513=*((struct _tuple13*)_tmp4F2->hd);_tmp514=_tmp513.f1;
_tmp515=_tmp513.f2;_tmp516=_tmp513.f3;if(!Cyc_Port_unifies(_tmp514,_tmp515) && (
unsigned int)_tmp516){void*_tmp517=Cyc_Port_compress_ct(_tmp515);_LL2D5: {struct
Cyc_Port_Notconst_ct_struct*_tmp518=(struct Cyc_Port_Notconst_ct_struct*)_tmp517;
if(_tmp518->tag != 1)goto _LL2D7;}_LL2D6:{struct Cyc_Port_Edit*_tmp69C;const char*
_tmp69B;const char*_tmp69A;struct Cyc_List_List*_tmp699;_tmp4F4=((_tmp699=
_cycalloc(sizeof(*_tmp699)),((_tmp699->hd=((_tmp69C=_cycalloc(sizeof(*_tmp69C)),((
_tmp69C->loc=_tmp516,((_tmp69C->old_string=((_tmp69B="const ",_tag_dyneither(
_tmp69B,sizeof(char),7))),((_tmp69C->new_string=((_tmp69A="",_tag_dyneither(
_tmp69A,sizeof(char),1))),_tmp69C)))))))),((_tmp699->tl=_tmp4F4,_tmp699))))));}
goto _LL2D4;_LL2D7: {struct Cyc_Port_Const_ct_struct*_tmp519=(struct Cyc_Port_Const_ct_struct*)
_tmp517;if(_tmp519->tag != 0)goto _LL2D9;}_LL2D8:{struct Cyc_Port_Edit*_tmp6A5;
const char*_tmp6A4;const char*_tmp6A3;struct Cyc_List_List*_tmp6A2;_tmp4F4=((
_tmp6A2=_cycalloc(sizeof(*_tmp6A2)),((_tmp6A2->hd=((_tmp6A5=_cycalloc(sizeof(*
_tmp6A5)),((_tmp6A5->loc=_tmp516,((_tmp6A5->old_string=((_tmp6A4="",
_tag_dyneither(_tmp6A4,sizeof(char),1))),((_tmp6A5->new_string=((_tmp6A3="const ",
_tag_dyneither(_tmp6A3,sizeof(char),7))),_tmp6A5)))))))),((_tmp6A2->tl=_tmp4F4,
_tmp6A2))))));}goto _LL2D4;_LL2D9:;_LL2DA: goto _LL2D4;_LL2D4:;}}for(0;(
unsigned int)_tmp4F3;_tmp4F3=_tmp4F3->tl){void*_tmp523;void*_tmp524;struct Cyc_Position_Segment*
_tmp525;struct _tuple13 _tmp522=*((struct _tuple13*)_tmp4F3->hd);_tmp523=_tmp522.f1;
_tmp524=_tmp522.f2;_tmp525=_tmp522.f3;if(!Cyc_Port_unifies(_tmp523,_tmp524) && (
unsigned int)_tmp525){void*_tmp526=Cyc_Port_compress_ct(_tmp524);_LL2DC: {struct
Cyc_Port_Zterm_ct_struct*_tmp527=(struct Cyc_Port_Zterm_ct_struct*)_tmp526;if(
_tmp527->tag != 8)goto _LL2DE;}_LL2DD:{struct Cyc_Port_Edit*_tmp6AE;const char*
_tmp6AD;const char*_tmp6AC;struct Cyc_List_List*_tmp6AB;_tmp4F4=((_tmp6AB=
_cycalloc(sizeof(*_tmp6AB)),((_tmp6AB->hd=((_tmp6AE=_cycalloc(sizeof(*_tmp6AE)),((
_tmp6AE->loc=_tmp525,((_tmp6AE->old_string=((_tmp6AD="NOZEROTERM ",
_tag_dyneither(_tmp6AD,sizeof(char),12))),((_tmp6AE->new_string=((_tmp6AC="",
_tag_dyneither(_tmp6AC,sizeof(char),1))),_tmp6AE)))))))),((_tmp6AB->tl=_tmp4F4,
_tmp6AB))))));}goto _LL2DB;_LL2DE: {struct Cyc_Port_Nozterm_ct_struct*_tmp528=(
struct Cyc_Port_Nozterm_ct_struct*)_tmp526;if(_tmp528->tag != 9)goto _LL2E0;}_LL2DF:{
struct Cyc_Port_Edit*_tmp6B7;const char*_tmp6B6;const char*_tmp6B5;struct Cyc_List_List*
_tmp6B4;_tmp4F4=((_tmp6B4=_cycalloc(sizeof(*_tmp6B4)),((_tmp6B4->hd=((_tmp6B7=
_cycalloc(sizeof(*_tmp6B7)),((_tmp6B7->loc=_tmp525,((_tmp6B7->old_string=((
_tmp6B6="ZEROTERM ",_tag_dyneither(_tmp6B6,sizeof(char),10))),((_tmp6B7->new_string=((
_tmp6B5="",_tag_dyneither(_tmp6B5,sizeof(char),1))),_tmp6B7)))))))),((_tmp6B4->tl=
_tmp4F4,_tmp6B4))))));}goto _LL2DB;_LL2E0:;_LL2E1: goto _LL2DB;_LL2DB:;}}_tmp4F4=((
struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),
struct Cyc_List_List*x))Cyc_List_merge_sort)(Cyc_Port_cmp_edit,_tmp4F4);while((
unsigned int)_tmp4F4  && ((struct Cyc_Position_Segment*)_check_null(((struct Cyc_Port_Edit*)
_tmp4F4->hd)->loc))->start == 0){_tmp4F4=_tmp4F4->tl;}return _tmp4F4;}struct Cyc_Port_Edit;
struct Cyc_Port_Edit;static struct Cyc_Position_Segment*Cyc_Port_get_seg(struct Cyc_Port_Edit*
e){return e->loc;}struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
void Cyc_Port_port(struct Cyc_List_List*ds){struct Cyc_List_List*_tmp531=Cyc_Port_gen_edits(
ds);struct Cyc_List_List*_tmp532=((struct Cyc_List_List*(*)(struct Cyc_Position_Segment*(*
f)(struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_get_seg,
_tmp531);Cyc_Position_use_gcc_style_location=0;{struct Cyc_List_List*_tmp533=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Position_strings_of_segments(
_tmp532));for(0;(unsigned int)_tmp531;(_tmp531=_tmp531->tl,_tmp533=_tmp533->tl)){
struct Cyc_Position_Segment*_tmp535;struct _dyneither_ptr _tmp536;struct
_dyneither_ptr _tmp537;struct Cyc_Port_Edit _tmp534=*((struct Cyc_Port_Edit*)_tmp531->hd);
_tmp535=_tmp534.loc;_tmp536=_tmp534.old_string;_tmp537=_tmp534.new_string;{
struct _dyneither_ptr sloc=(struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct
Cyc_List_List*)_check_null(_tmp533))->hd);const char*_tmp6BD;void*_tmp6BC[3];
struct Cyc_String_pa_struct _tmp6BB;struct Cyc_String_pa_struct _tmp6BA;struct Cyc_String_pa_struct
_tmp6B9;(_tmp6B9.tag=0,((_tmp6B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp537),((_tmp6BA.tag=0,((_tmp6BA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp536),((_tmp6BB.tag=0,((_tmp6BB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)sloc),((_tmp6BC[0]=& _tmp6BB,((_tmp6BC[1]=& _tmp6BA,((_tmp6BC[
2]=& _tmp6B9,Cyc_printf(((_tmp6BD="%s: insert `%s' for `%s'\n",_tag_dyneither(
_tmp6BD,sizeof(char),26))),_tag_dyneither(_tmp6BC,sizeof(void*),3)))))))))))))))))));};}};}
