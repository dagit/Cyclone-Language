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
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*Cyc_stderr;struct
Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int
tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _dyneither_ptr Cyc_Core_new_string(
unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
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
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};unsigned long Cyc_strlen(struct
_dyneither_ptr s);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
struct _dyneither_ptr Cyc_strncpy(struct _dyneither_ptr,struct _dyneither_ptr,
unsigned long);struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,
unsigned long n);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};struct Cyc_Lineno_Pos*Cyc_Lineno_pos_of_abs(
struct _dyneither_ptr,int);void Cyc_Lineno_poss_of_abss(struct _dyneither_ptr
filename,struct Cyc_List_List*places);extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};void Cyc_Position_reset_position(struct _dyneither_ptr);void Cyc_Position_set_position_file(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Position_get_position_file();
struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);int Cyc_Position_segment_equals(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);struct _dyneither_ptr Cyc_Position_string_of_loc(int);
struct _dyneither_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_lex(struct Cyc_Position_Segment*,
struct _dyneither_ptr);struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct
Cyc_Position_Segment*,struct _dyneither_ptr);struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[
10];struct Cyc_Position_Nocontext_struct{char*tag;};extern int Cyc_Position_use_gcc_style_location;
extern int Cyc_Position_print_context;extern int Cyc_Position_num_errors;extern int
Cyc_Position_max_errors;void Cyc_Position_post_error(struct Cyc_Position_Error*);
int Cyc_Position_error_p();struct _dyneither_ptr Cyc_Position_get_line_directive(
struct Cyc_Position_Segment*loc);int Cyc_Position_use_gcc_style_location=1;char Cyc_Position_Exit[
5]="Exit";static char _tmp0[1]="";static struct _dyneither_ptr Cyc_Position_source={
_tmp0,_tmp0,_tmp0 + 1};struct Cyc_Position_Segment{int start;int end;};struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs(int start,int end){struct Cyc_Position_Segment*_tmp74;
return(_tmp74=_cycalloc_atomic(sizeof(*_tmp74)),((_tmp74->start=start,((_tmp74->end=
end,_tmp74)))));}struct Cyc_Position_Segment*Cyc_Position_segment_join(struct Cyc_Position_Segment*
s1,struct Cyc_Position_Segment*s2){if(s1 == 0)return s2;if(s2 == 0)return s1;{struct
Cyc_Position_Segment*_tmp75;return(_tmp75=_cycalloc_atomic(sizeof(*_tmp75)),((
_tmp75->start=s1->start,((_tmp75->end=s2->end,_tmp75)))));};}int Cyc_Position_segment_equals(
struct Cyc_Position_Segment*s1,struct Cyc_Position_Segment*s2){if(s1 == s2)return 1;
if(s1 == 0)return s2 == 0;return s2 != 0  && (s1->start == s2->start  && s1->end == s2->end);}
struct _dyneither_ptr Cyc_Position_string_of_loc(int loc){struct Cyc_Lineno_Pos*pos=
Cyc_Lineno_pos_of_abs(Cyc_Position_source,loc);if(Cyc_Position_use_gcc_style_location){
const char*_tmp7A;void*_tmp79[2];struct Cyc_String_pa_struct _tmp78;struct Cyc_Int_pa_struct
_tmp77;return(_tmp77.tag=1,((_tmp77.f1=(unsigned long)pos->line_no,((_tmp78.tag=
0,((_tmp78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)pos->logical_file),((
_tmp79[0]=& _tmp78,((_tmp79[1]=& _tmp77,Cyc_aprintf(((_tmp7A="%s:%d",
_tag_dyneither(_tmp7A,sizeof(char),6))),_tag_dyneither(_tmp79,sizeof(void*),2)))))))))))));}
else{const char*_tmp80;void*_tmp7F[3];struct Cyc_String_pa_struct _tmp7E;struct Cyc_Int_pa_struct
_tmp7D;struct Cyc_Int_pa_struct _tmp7C;return(_tmp7C.tag=1,((_tmp7C.f1=(
unsigned long)pos->col,((_tmp7D.tag=1,((_tmp7D.f1=(unsigned long)pos->line_no,((
_tmp7E.tag=0,((_tmp7E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)pos->logical_file),((
_tmp7F[0]=& _tmp7E,((_tmp7F[1]=& _tmp7D,((_tmp7F[2]=& _tmp7C,Cyc_aprintf(((_tmp80="%s:(%d:%d)",
_tag_dyneither(_tmp80,sizeof(char),11))),_tag_dyneither(_tmp7F,sizeof(void*),3)))))))))))))))))));}}
static struct _dyneither_ptr Cyc_Position_string_of_pos_pr(struct Cyc_Lineno_Pos*
pos_s,struct Cyc_Lineno_Pos*pos_e){if(Cyc_Position_use_gcc_style_location){const
char*_tmp85;void*_tmp84[2];struct Cyc_String_pa_struct _tmp83;struct Cyc_Int_pa_struct
_tmp82;return(_tmp82.tag=1,((_tmp82.f1=(unsigned long)pos_s->line_no,((_tmp83.tag=
0,((_tmp83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)pos_s->logical_file),((
_tmp84[0]=& _tmp83,((_tmp84[1]=& _tmp82,Cyc_aprintf(((_tmp85="%s:%d",
_tag_dyneither(_tmp85,sizeof(char),6))),_tag_dyneither(_tmp84,sizeof(void*),2)))))))))))));}
else{if(Cyc_strcmp((struct _dyneither_ptr)pos_s->logical_file,(struct
_dyneither_ptr)pos_e->logical_file)== 0){const char*_tmp8D;void*_tmp8C[5];struct
Cyc_String_pa_struct _tmp8B;struct Cyc_Int_pa_struct _tmp8A;struct Cyc_Int_pa_struct
_tmp89;struct Cyc_Int_pa_struct _tmp88;struct Cyc_Int_pa_struct _tmp87;return(_tmp87.tag=
1,((_tmp87.f1=(unsigned long)pos_e->col,((_tmp88.tag=1,((_tmp88.f1=(
unsigned long)pos_e->line_no,((_tmp89.tag=1,((_tmp89.f1=(unsigned long)pos_s->col,((
_tmp8A.tag=1,((_tmp8A.f1=(unsigned long)pos_s->line_no,((_tmp8B.tag=0,((_tmp8B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)pos_s->logical_file),((_tmp8C[0]=&
_tmp8B,((_tmp8C[1]=& _tmp8A,((_tmp8C[2]=& _tmp89,((_tmp8C[3]=& _tmp88,((_tmp8C[4]=&
_tmp87,Cyc_aprintf(((_tmp8D="%s(%d:%d-%d:%d)",_tag_dyneither(_tmp8D,sizeof(char),
16))),_tag_dyneither(_tmp8C,sizeof(void*),5)))))))))))))))))))))))))))))));}
else{const char*_tmp96;void*_tmp95[6];struct Cyc_String_pa_struct _tmp94;struct Cyc_Int_pa_struct
_tmp93;struct Cyc_Int_pa_struct _tmp92;struct Cyc_String_pa_struct _tmp91;struct Cyc_Int_pa_struct
_tmp90;struct Cyc_Int_pa_struct _tmp8F;return(_tmp8F.tag=1,((_tmp8F.f1=(
unsigned long)pos_e->col,((_tmp90.tag=1,((_tmp90.f1=(unsigned long)pos_e->line_no,((
_tmp91.tag=0,((_tmp91.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)pos_e->logical_file),((
_tmp92.tag=1,((_tmp92.f1=(unsigned long)pos_s->col,((_tmp93.tag=1,((_tmp93.f1=(
unsigned long)pos_s->line_no,((_tmp94.tag=0,((_tmp94.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)pos_s->logical_file),((_tmp95[0]=& _tmp94,((_tmp95[1]=&
_tmp93,((_tmp95[2]=& _tmp92,((_tmp95[3]=& _tmp91,((_tmp95[4]=& _tmp90,((_tmp95[5]=&
_tmp8F,Cyc_aprintf(((_tmp96="%s(%d:%d)-%s(%d:%d)",_tag_dyneither(_tmp96,sizeof(
char),20))),_tag_dyneither(_tmp95,sizeof(void*),6)))))))))))))))))))))))))))))))))))));}}}
struct _dyneither_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*s){
if(s == 0){const char*_tmp9A;void*_tmp99[1];struct Cyc_String_pa_struct _tmp98;
return(_tmp98.tag=0,((_tmp98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_source),((
_tmp99[0]=& _tmp98,Cyc_aprintf(((_tmp9A="%s",_tag_dyneither(_tmp9A,sizeof(char),3))),
_tag_dyneither(_tmp99,sizeof(void*),1)))))));}{struct Cyc_Lineno_Pos*pos_s=Cyc_Lineno_pos_of_abs(
Cyc_Position_source,s->start);struct Cyc_Lineno_Pos*pos_e=Cyc_Lineno_pos_of_abs(
Cyc_Position_source,s->end);return Cyc_Position_string_of_pos_pr(pos_s,pos_e);};}
static struct Cyc_Lineno_Pos*Cyc_Position_new_pos(){const char*_tmp9D;struct Cyc_Lineno_Pos*
_tmp9C;return(_tmp9C=_cycalloc(sizeof(*_tmp9C)),((_tmp9C->logical_file=((_tmp9D="",
_tag_dyneither(_tmp9D,sizeof(char),1))),((_tmp9C->line=Cyc_Core_new_string(0),((
_tmp9C->line_no=0,((_tmp9C->col=0,_tmp9C)))))))));}struct _tuple0{int f1;struct Cyc_Lineno_Pos*
f2;};struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*
segs){struct Cyc_List_List*places=0;{struct Cyc_List_List*_tmp24=segs;for(0;_tmp24
!= 0;_tmp24=_tmp24->tl){if((struct Cyc_Position_Segment*)_tmp24->hd == 0)continue;{
struct Cyc_List_List*_tmpA5;struct _tuple0*_tmpA4;struct _tuple0*_tmpA3;struct Cyc_List_List*
_tmpA2;places=((_tmpA2=_cycalloc(sizeof(*_tmpA2)),((_tmpA2->hd=((_tmpA3=
_cycalloc(sizeof(*_tmpA3)),((_tmpA3->f1=((struct Cyc_Position_Segment*)
_check_null((struct Cyc_Position_Segment*)_tmp24->hd))->end,((_tmpA3->f2=Cyc_Position_new_pos(),
_tmpA3)))))),((_tmpA2->tl=((_tmpA5=_cycalloc(sizeof(*_tmpA5)),((_tmpA5->hd=((
_tmpA4=_cycalloc(sizeof(*_tmpA4)),((_tmpA4->f1=((struct Cyc_Position_Segment*)
_check_null((struct Cyc_Position_Segment*)_tmp24->hd))->start,((_tmpA4->f2=Cyc_Position_new_pos(),
_tmpA4)))))),((_tmpA5->tl=places,_tmpA5)))))),_tmpA2))))));};}}Cyc_Lineno_poss_of_abss(
Cyc_Position_source,places);{struct Cyc_List_List*ans=0;places=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(places);for(0;segs != 0;segs=segs->tl){
if((struct Cyc_Position_Segment*)segs->hd == 0){struct _dyneither_ptr*_tmpB2;const
char*_tmpB1;void*_tmpB0[1];struct Cyc_String_pa_struct _tmpAF;struct Cyc_List_List*
_tmpAE;ans=((_tmpAE=_cycalloc(sizeof(*_tmpAE)),((_tmpAE->hd=((_tmpB2=_cycalloc(
sizeof(*_tmpB2)),((_tmpB2[0]=((_tmpAF.tag=0,((_tmpAF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Position_source),((_tmpB0[0]=& _tmpAF,Cyc_aprintf(((
_tmpB1="%s(unknown)",_tag_dyneither(_tmpB1,sizeof(char),12))),_tag_dyneither(
_tmpB0,sizeof(void*),1)))))))),_tmpB2)))),((_tmpAE->tl=ans,_tmpAE))))));}else{{
struct _dyneither_ptr*_tmpB5;struct Cyc_List_List*_tmpB4;ans=((_tmpB4=_cycalloc(
sizeof(*_tmpB4)),((_tmpB4->hd=((_tmpB5=_cycalloc(sizeof(*_tmpB5)),((_tmpB5[0]=
Cyc_Position_string_of_pos_pr((*((struct _tuple0*)((struct Cyc_List_List*)
_check_null(places))->hd)).f2,(*((struct _tuple0*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(places))->tl))->hd)).f2),_tmpB5)))),((
_tmpB4->tl=ans,_tmpB4))))));}places=((struct Cyc_List_List*)_check_null(places->tl))->tl;}}
return ans;};}struct Cyc_Position_Lex_struct Cyc_Position_Lex_val={0};struct Cyc_Position_Parse_struct
Cyc_Position_Parse_val={1};struct Cyc_Position_Elab_struct Cyc_Position_Elab_val={
2};struct Cyc_Position_Error;struct Cyc_Position_Error*Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment*l,struct _dyneither_ptr desc){struct Cyc_Position_Error*
_tmpB6;return(_tmpB6=_cycalloc(sizeof(*_tmpB6)),((_tmpB6->source=Cyc_Position_source,((
_tmpB6->seg=l,((_tmpB6->kind=(void*)& Cyc_Position_Lex_val,((_tmpB6->desc=desc,
_tmpB6)))))))));}struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*
l,struct _dyneither_ptr desc){struct Cyc_Position_Error*_tmpB7;return(_tmpB7=
_cycalloc(sizeof(*_tmpB7)),((_tmpB7->source=Cyc_Position_source,((_tmpB7->seg=l,((
_tmpB7->kind=(void*)& Cyc_Position_Parse_val,((_tmpB7->desc=desc,_tmpB7)))))))));}
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*l,
struct _dyneither_ptr desc){struct Cyc_Position_Error*_tmpB8;return(_tmpB8=
_cycalloc(sizeof(*_tmpB8)),((_tmpB8->source=Cyc_Position_source,((_tmpB8->seg=l,((
_tmpB8->kind=(void*)& Cyc_Position_Elab_val,((_tmpB8->desc=desc,_tmpB8)))))))));}
char Cyc_Position_Nocontext[10]="Nocontext";struct Cyc_Position_Nocontext_struct
Cyc_Position_Nocontext_val={Cyc_Position_Nocontext};static struct _dyneither_ptr
Cyc_Position_trunc(int n,struct _dyneither_ptr s){int len=(int)Cyc_strlen((struct
_dyneither_ptr)s);if(len < n)return s;{int len_one=(n - 3)/ 2;int len_two=(n - 3)- 
len_one;struct _dyneither_ptr mans=Cyc_Core_new_string((unsigned int)(n + 1));
struct _dyneither_ptr ans=_dyneither_ptr_decrease_size(mans,sizeof(char),1);Cyc_strncpy(
ans,(struct _dyneither_ptr)s,(unsigned long)len_one);{const char*_tmpB9;Cyc_strncpy(
_dyneither_ptr_plus(ans,sizeof(char),len_one),((_tmpB9="...",_tag_dyneither(
_tmpB9,sizeof(char),4))),3);}Cyc_strncpy(_dyneither_ptr_plus(ans,sizeof(char),
len_one + 3),(struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),len - 
len_two),(unsigned long)len_two);return mans;};}static int Cyc_Position_line_length=
76;struct _tuple1{struct _dyneither_ptr f1;int f2;int f3;};static struct _tuple1*Cyc_Position_get_context(
struct Cyc_Position_Segment*seg){if(seg == 0)(int)_throw((void*)& Cyc_Position_Nocontext_val);{
struct Cyc_Lineno_Pos*pos_s;struct Cyc_Lineno_Pos*pos_e;{struct _handler_cons _tmp38;
_push_handler(& _tmp38);{int _tmp3A=0;if(setjmp(_tmp38.handler))_tmp3A=1;if(!
_tmp3A){pos_s=Cyc_Lineno_pos_of_abs(Cyc_Position_source,seg->start);pos_e=Cyc_Lineno_pos_of_abs(
Cyc_Position_source,seg->end);;_pop_handler();}else{void*_tmp39=(void*)
_exn_thrown;void*_tmp3C=_tmp39;_LL1:;_LL2:(int)_throw((void*)& Cyc_Position_Nocontext_val);
_LL3:;_LL4:(void)_throw(_tmp3C);_LL0:;}};}{struct Cyc_Lineno_Pos _tmp3E;struct
_dyneither_ptr _tmp3F;int _tmp40;int _tmp41;struct Cyc_Lineno_Pos*_tmp3D=pos_s;
_tmp3E=*_tmp3D;_tmp3F=_tmp3E.line;_tmp40=_tmp3E.line_no;_tmp41=_tmp3E.col;{
struct Cyc_Lineno_Pos _tmp43;struct _dyneither_ptr _tmp44;int _tmp45;int _tmp46;struct
Cyc_Lineno_Pos*_tmp42=pos_e;_tmp43=*_tmp42;_tmp44=_tmp43.line;_tmp45=_tmp43.line_no;
_tmp46=_tmp43.col;if(_tmp40 == _tmp45){int n=Cyc_Position_line_length / 3;struct
_dyneither_ptr sec_one=Cyc_Position_trunc(n,Cyc_substring((struct _dyneither_ptr)
_tmp3F,0,(unsigned long)_tmp41));struct _dyneither_ptr sec_two=Cyc_Position_trunc(
n,Cyc_substring((struct _dyneither_ptr)_tmp3F,_tmp41,(unsigned long)(_tmp46 - 
_tmp41)));struct _dyneither_ptr sec_three=Cyc_Position_trunc(n,Cyc_substring((
struct _dyneither_ptr)_tmp3F,_tmp41,Cyc_strlen((struct _dyneither_ptr)_tmp3F)- 
_tmp46));struct Cyc_String_pa_struct _tmpC5;struct Cyc_String_pa_struct _tmpC4;
struct Cyc_String_pa_struct _tmpC3;void*_tmpC2[3];const char*_tmpC1;struct _tuple1*
_tmpC0;return(_tmpC0=_cycalloc(sizeof(*_tmpC0)),((_tmpC0->f1=((_tmpC5.tag=0,((
_tmpC5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sec_three),((_tmpC4.tag=0,((
_tmpC4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sec_two),((_tmpC3.tag=0,((
_tmpC3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sec_one),((_tmpC2[0]=&
_tmpC3,((_tmpC2[1]=& _tmpC4,((_tmpC2[2]=& _tmpC5,Cyc_aprintf(((_tmpC1="%s%s%s",
_tag_dyneither(_tmpC1,sizeof(char),7))),_tag_dyneither(_tmpC2,sizeof(void*),3)))))))))))))))))))),((
_tmpC0->f2=(int)Cyc_strlen((struct _dyneither_ptr)sec_one),((_tmpC0->f3=(int)(Cyc_strlen((
struct _dyneither_ptr)sec_one)+ Cyc_strlen((struct _dyneither_ptr)sec_two)),_tmpC0)))))));}
else{int n=(Cyc_Position_line_length - 3)/ 4;struct _dyneither_ptr sec_one=Cyc_Position_trunc(
n,Cyc_substring((struct _dyneither_ptr)_tmp3F,0,(unsigned long)_tmp41));struct
_dyneither_ptr sec_two=Cyc_Position_trunc(n,Cyc_substring((struct _dyneither_ptr)
_tmp3F,_tmp41,Cyc_strlen((struct _dyneither_ptr)_tmp3F)- _tmp41));struct
_dyneither_ptr sec_three=Cyc_Position_trunc(n,Cyc_substring((struct _dyneither_ptr)
_tmp44,0,(unsigned long)_tmp46));struct _dyneither_ptr sec_four=Cyc_Position_trunc(
n,Cyc_substring((struct _dyneither_ptr)_tmp44,_tmp46,Cyc_strlen((struct
_dyneither_ptr)_tmp44)- _tmp46));struct Cyc_String_pa_struct _tmpD3;struct Cyc_String_pa_struct
_tmpD2;struct Cyc_String_pa_struct _tmpD1;struct Cyc_String_pa_struct _tmpD0;void*
_tmpCF[4];const char*_tmpCE;struct _tuple1*_tmpCD;return(_tmpCD=_cycalloc(sizeof(*
_tmpCD)),((_tmpCD->f1=((_tmpD3.tag=0,((_tmpD3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sec_four),((_tmpD2.tag=0,((_tmpD2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)sec_three),((_tmpD1.tag=0,((_tmpD1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)sec_two),((_tmpD0.tag=0,((_tmpD0.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)sec_one),((_tmpCF[0]=& _tmpD0,((_tmpCF[1]=& _tmpD1,((_tmpCF[2]=&
_tmpD2,((_tmpCF[3]=& _tmpD3,Cyc_aprintf(((_tmpCE="%s%s.\\.%s%s",_tag_dyneither(
_tmpCE,sizeof(char),12))),_tag_dyneither(_tmpCF,sizeof(void*),4)))))))))))))))))))))))))),((
_tmpCD->f2=(int)Cyc_strlen((struct _dyneither_ptr)sec_one),((_tmpCD->f3=(int)(((
Cyc_strlen((struct _dyneither_ptr)sec_one)+ Cyc_strlen((struct _dyneither_ptr)
sec_two))+ 3)+ Cyc_strlen((struct _dyneither_ptr)sec_three)),_tmpCD)))))));}};};};}
static int Cyc_Position_error_b=0;int Cyc_Position_error_p(){return Cyc_Position_error_b;}
char Cyc_Position_Error[6]="Error";struct Cyc_Position_Error_struct{char*tag;
struct Cyc_Position_Error*f1;};int Cyc_Position_print_context=0;int Cyc_Position_first_error=
1;int Cyc_Position_num_errors=0;int Cyc_Position_max_errors=10;void Cyc_Position_post_error(
struct Cyc_Position_Error*e);static void _tmpE1(unsigned int*_tmpE0,unsigned int*
_tmpDF,char**_tmpDD){for(*_tmpE0=0;*_tmpE0 < *_tmpDF;(*_tmpE0)++){(*_tmpDD)[*
_tmpE0]='\000';}}void Cyc_Position_post_error(struct Cyc_Position_Error*e){Cyc_Position_error_b=
1;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stdout);if(Cyc_Position_first_error){{
const char*_tmpD6;void*_tmpD5;(_tmpD5=0,Cyc_fprintf(Cyc_stderr,((_tmpD6="\n",
_tag_dyneither(_tmpD6,sizeof(char),2))),_tag_dyneither(_tmpD5,sizeof(void*),0)));}
Cyc_Position_first_error=0;}if(Cyc_Position_num_errors <= Cyc_Position_max_errors){{
const char*_tmpDB;void*_tmpDA[2];struct Cyc_String_pa_struct _tmpD9;struct Cyc_String_pa_struct
_tmpD8;(_tmpD8.tag=0,((_tmpD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)e->desc),((
_tmpD9.tag=0,((_tmpD9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
e->seg)),((_tmpDA[0]=& _tmpD9,((_tmpDA[1]=& _tmpD8,Cyc_fprintf(Cyc_stderr,((_tmpDB="%s: %s\n",
_tag_dyneither(_tmpDB,sizeof(char),8))),_tag_dyneither(_tmpDA,sizeof(void*),2)))))))))))));}
if(Cyc_Position_print_context){struct _handler_cons _tmp5A;_push_handler(& _tmp5A);{
int _tmp5C=0;if(setjmp(_tmp5A.handler))_tmp5C=1;if(!_tmp5C){{struct _tuple1*x=Cyc_Position_get_context(
e->seg);unsigned int _tmpE0;unsigned int _tmpDF;struct _dyneither_ptr _tmpDE;char*
_tmpDD;unsigned int _tmpDC;struct _dyneither_ptr marker_str=(_tmpDC=(unsigned int)((*
x).f3 + 1),((_tmpDD=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpDC + 1)),((
_tmpDE=_tag_dyneither(_tmpDD,sizeof(char),_tmpDC + 1),((((_tmpDF=_tmpDC,((_tmpE1(&
_tmpE0,& _tmpDF,& _tmpDD),_tmpDD[_tmpDF]=(char)0)))),_tmpDE)))))));int i=- 1;while(
++ i < (*x).f2){char _tmpE4;char _tmpE3;struct _dyneither_ptr _tmpE2;(_tmpE2=
_dyneither_ptr_plus(marker_str,sizeof(char),i),((_tmpE3=*((char*)
_check_dyneither_subscript(_tmpE2,sizeof(char),0)),((_tmpE4=' ',((
_get_dyneither_size(_tmpE2,sizeof(char))== 1  && (_tmpE3 == '\000'  && _tmpE4 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpE2.curr)=_tmpE4)))))));}while(++ i < (*x).f3){
char _tmpE7;char _tmpE6;struct _dyneither_ptr _tmpE5;(_tmpE5=_dyneither_ptr_plus(
marker_str,sizeof(char),i),((_tmpE6=*((char*)_check_dyneither_subscript(_tmpE5,
sizeof(char),0)),((_tmpE7='^',((_get_dyneither_size(_tmpE5,sizeof(char))== 1  && (
_tmpE6 == '\000'  && _tmpE7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpE5.curr)=
_tmpE7)))))));}{const char*_tmpEC;void*_tmpEB[2];struct Cyc_String_pa_struct _tmpEA;
struct Cyc_String_pa_struct _tmpE9;(_tmpE9.tag=0,((_tmpE9.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)marker_str),((_tmpEA.tag=0,((_tmpEA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)(*x).f1),((_tmpEB[0]=& _tmpEA,((_tmpEB[1]=&
_tmpE9,Cyc_fprintf(Cyc_stderr,((_tmpEC="  %s\n  %s\n",_tag_dyneither(_tmpEC,
sizeof(char),11))),_tag_dyneither(_tmpEB,sizeof(void*),2)))))))))))));};};
_pop_handler();}else{void*_tmp5B=(void*)_exn_thrown;void*_tmp6C=_tmp5B;_LL6: {
struct Cyc_Position_Nocontext_struct*_tmp6D=(struct Cyc_Position_Nocontext_struct*)
_tmp6C;if(_tmp6D->tag != Cyc_Position_Nocontext)goto _LL8;}_LL7: goto _LL5;_LL8:;
_LL9:(void)_throw(_tmp6C);_LL5:;}};}}if(Cyc_Position_num_errors == Cyc_Position_max_errors){
const char*_tmpEF;void*_tmpEE;(_tmpEE=0,Cyc_fprintf(Cyc_stderr,((_tmpEF="Too many error messages!\n",
_tag_dyneither(_tmpEF,sizeof(char),26))),_tag_dyneither(_tmpEE,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);++ Cyc_Position_num_errors;}void Cyc_Position_reset_position(
struct _dyneither_ptr s){Cyc_Position_source=s;Cyc_Position_error_b=0;}void Cyc_Position_set_position_file(
struct _dyneither_ptr s){Cyc_Position_source=s;Cyc_Position_error_b=0;}struct
_dyneither_ptr Cyc_Position_get_position_file(){return Cyc_Position_source;}struct
_dyneither_ptr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*s){
struct Cyc_Lineno_Pos*pos_s=Cyc_Lineno_pos_of_abs(Cyc_Position_source,((struct Cyc_Position_Segment*)
_check_null(s))->start);if((struct Cyc_Lineno_Pos*)pos_s != 0){const char*_tmpF4;
void*_tmpF3[2];struct Cyc_Int_pa_struct _tmpF2;struct Cyc_String_pa_struct _tmpF1;
return(struct _dyneither_ptr)((_tmpF1.tag=0,((_tmpF1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)pos_s->logical_file),((_tmpF2.tag=1,((_tmpF2.f1=(
unsigned long)pos_s->line_no,((_tmpF3[0]=& _tmpF2,((_tmpF3[1]=& _tmpF1,Cyc_aprintf(((
_tmpF4="\n#line %d \"%s\"\n",_tag_dyneither(_tmpF4,sizeof(char),16))),
_tag_dyneither(_tmpF3,sizeof(void*),2))))))))))))));}else{return(struct
_dyneither_ptr)_tag_dyneither(0,0,0);}}
