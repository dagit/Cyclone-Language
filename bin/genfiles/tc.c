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
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
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
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];
struct Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter_c(void(*f)(void*,
void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*
x);struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*
x);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
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
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*
ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};
struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*
bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};
struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts
ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{
int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{
int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{int tag;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
zt_loc;};struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;}
;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
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
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
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
struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _tuple4{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};
struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple6 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
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
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
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
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern
struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_const_string_typ(
void*rgn);void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_array_typ(void*elt_type,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct _dyneither_ptr Cyc_Absyn_attribute2string(
void*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(
struct Cyc_Absyn_Kind*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(
struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(
struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*
s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};
struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*
r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);int Cyc_Dict_member(struct Cyc_Dict_Dict
d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);struct
_tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*,
int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct
_RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(struct _RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct
Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*,struct Cyc_List_List*);struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple0*);int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int
Cyc_Tcutil_is_function_type(void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*
k);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,
struct Cyc_Absyn_Kind*k,int allow_evars,void*);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_supports_default(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple9*eq;int isTrue;
};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple10{unsigned int f1;int f2;};
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);void Cyc_Tc_tc(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_struct{
char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;
struct Cyc_Absyn_Datatypefield*field;};struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(
struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(
struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);static char _tmp0[1]="";static struct _dyneither_ptr
Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};static struct _dyneither_ptr*Cyc_Tc_tc_msg=(
struct _dyneither_ptr*)& Cyc_Tc_tc_msg_c;struct _tuple11{struct Cyc_Position_Segment*
f1;struct _tuple0*f2;int f3;};static int Cyc_Tc_export_member(struct _tuple0*x,struct
Cyc_List_List*exports){for(0;exports != 0;exports=exports->tl){struct _tuple11*
_tmp1=(struct _tuple11*)exports->hd;if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){(*
_tmp1).f3=1;return 1;}}return 0;}struct _tuple12{void*f1;int f2;};static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**
exports){struct Cyc_Absyn_Vardecl _tmp3;enum Cyc_Absyn_Scope _tmp4;struct _tuple0*
_tmp5;struct _tuple0 _tmp6;union Cyc_Absyn_Nmspace _tmp7;struct _dyneither_ptr*_tmp8;
void*_tmp9;struct Cyc_Absyn_Exp*_tmpA;struct Cyc_List_List*_tmpB;struct Cyc_Absyn_Vardecl*
_tmp2=vd;_tmp3=*_tmp2;_tmp4=_tmp3.sc;_tmp5=_tmp3.name;_tmp6=*_tmp5;_tmp7=_tmp6.f1;
_tmp8=_tmp6.f2;_tmp9=_tmp3.type;_tmpA=_tmp3.initializer;_tmpB=_tmp3.attributes;{
union Cyc_Absyn_Nmspace _tmpC=_tmp7;struct Cyc_List_List*_tmpD;struct Cyc_List_List*
_tmpE;struct Cyc_List_List*_tmpF;_LL1: if((_tmpC.Rel_n).tag != 1)goto _LL3;_tmpD=(
struct Cyc_List_List*)(_tmpC.Rel_n).val;if(_tmpD != 0)goto _LL3;_LL2: goto _LL4;_LL3:
if((_tmpC.C_n).tag != 3)goto _LL5;_tmpE=(struct Cyc_List_List*)(_tmpC.C_n).val;if(
_tmpE != 0)goto _LL5;_LL4: goto _LL6;_LL5: if((_tmpC.Abs_n).tag != 2)goto _LL7;_tmpF=(
struct Cyc_List_List*)(_tmpC.Abs_n).val;if(_tmpF != 0)goto _LL7;_LL6: goto _LL0;_LL7:;
_LL8:{const char*_tmp2AC;void*_tmp2AB[1];struct Cyc_String_pa_struct _tmp2AA;(
_tmp2AA.tag=0,((_tmp2AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
vd->name)),((_tmp2AB[0]=& _tmp2AA,Cyc_Tcutil_terr(loc,((_tmp2AC="qualified variable declarations are not implemented (%s)",
_tag_dyneither(_tmp2AC,sizeof(char),57))),_tag_dyneither(_tmp2AB,sizeof(void*),1)))))));}
return;_LL0:;}(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns,vd->sc == Cyc_Absyn_ExternC
 || in_cinclude);{void*_tmp13=Cyc_Tcutil_compress(_tmp9);struct Cyc_Absyn_ArrayInfo
_tmp15;void*_tmp16;struct Cyc_Absyn_Tqual _tmp17;struct Cyc_Absyn_Exp*_tmp18;union
Cyc_Absyn_Constraint*_tmp19;struct Cyc_Position_Segment*_tmp1A;_LLA:{struct Cyc_Absyn_ArrayType_struct*
_tmp14=(struct Cyc_Absyn_ArrayType_struct*)_tmp13;if(_tmp14->tag != 9)goto _LLC;
else{_tmp15=_tmp14->f1;_tmp16=_tmp15.elt_type;_tmp17=_tmp15.tq;_tmp18=_tmp15.num_elts;
if(_tmp18 != 0)goto _LLC;_tmp19=_tmp15.zero_term;_tmp1A=_tmp15.zt_loc;}}if(!(_tmpA
!= 0))goto _LLC;_LLB:{void*_tmp1B=_tmpA->r;union Cyc_Absyn_Cnst _tmp1D;struct
_dyneither_ptr _tmp1E;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_List_List*_tmp22;
struct Cyc_List_List*_tmp24;_LLF: {struct Cyc_Absyn_Const_e_struct*_tmp1C=(struct
Cyc_Absyn_Const_e_struct*)_tmp1B;if(_tmp1C->tag != 0)goto _LL11;else{_tmp1D=_tmp1C->f1;
if((_tmp1D.String_c).tag != 7)goto _LL11;_tmp1E=(struct _dyneither_ptr)(_tmp1D.String_c).val;}}
_LL10: _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp16,_tmp17,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp1E,sizeof(char)),0),_tmp19,_tmp1A));
goto _LLE;_LL11: {struct Cyc_Absyn_Comprehension_e_struct*_tmp1F=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1B;if(_tmp1F->tag != 28)goto _LL13;else{_tmp20=_tmp1F->f2;}}_LL12: _tmp9=(vd->type=
Cyc_Absyn_array_typ(_tmp16,_tmp17,(struct Cyc_Absyn_Exp*)_tmp20,_tmp19,_tmp1A));
goto _LLE;_LL13: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp21=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp1B;if(_tmp21->tag != 36)goto _LL15;else{_tmp22=_tmp21->f2;}}_LL14: _tmp24=
_tmp22;goto _LL16;_LL15: {struct Cyc_Absyn_Array_e_struct*_tmp23=(struct Cyc_Absyn_Array_e_struct*)
_tmp1B;if(_tmp23->tag != 27)goto _LL17;else{_tmp24=_tmp23->f1;}}_LL16: _tmp9=(vd->type=
Cyc_Absyn_array_typ(_tmp16,_tmp17,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp24),0),_tmp19,
_tmp1A));goto _LLE;_LL17:;_LL18: goto _LLE;_LLE:;}goto _LL9;_LLC:;_LLD: goto _LL9;_LL9:;}
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp9);(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(vd->tq).print_const,_tmp9);{void*_tmp25=Cyc_Tcutil_compress(_tmp9);_LL1A: {
struct Cyc_Absyn_ArrayType_struct*_tmp26=(struct Cyc_Absyn_ArrayType_struct*)
_tmp25;if(_tmp26->tag != 9)goto _LL1C;}_LL1B: vd->escapes=0;goto _LL19;_LL1C:;_LL1D:
vd->escapes=1;goto _LL19;_LL19:;}if(Cyc_Tcutil_is_function_type(_tmp9))_tmpB=Cyc_Tcutil_transfer_fn_type_atts(
_tmp9,_tmpB);if(_tmp4 == Cyc_Absyn_Extern  || _tmp4 == Cyc_Absyn_ExternC){if(_tmpA
!= 0){const char*_tmp2AF;void*_tmp2AE;(_tmp2AE=0,Cyc_Tcutil_terr(loc,((_tmp2AF="extern declaration should not have initializer",
_tag_dyneither(_tmp2AF,sizeof(char),47))),_tag_dyneither(_tmp2AE,sizeof(void*),0)));}}
else{if(!Cyc_Tcutil_is_function_type(_tmp9)){for(0;_tmpB != 0;_tmpB=_tmpB->tl){
void*_tmp29=(void*)_tmpB->hd;_LL1F: {struct Cyc_Absyn_Aligned_att_struct*_tmp2A=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp29;if(_tmp2A->tag != 6)goto _LL21;}_LL20:
goto _LL22;_LL21: {struct Cyc_Absyn_Section_att_struct*_tmp2B=(struct Cyc_Absyn_Section_att_struct*)
_tmp29;if(_tmp2B->tag != 8)goto _LL23;}_LL22: goto _LL24;_LL23: {struct Cyc_Absyn_Nocommon_att_struct*
_tmp2C=(struct Cyc_Absyn_Nocommon_att_struct*)_tmp29;if(_tmp2C->tag != 9)goto _LL25;}
_LL24: goto _LL26;_LL25: {struct Cyc_Absyn_Shared_att_struct*_tmp2D=(struct Cyc_Absyn_Shared_att_struct*)
_tmp29;if(_tmp2D->tag != 10)goto _LL27;}_LL26: goto _LL28;_LL27: {struct Cyc_Absyn_Unused_att_struct*
_tmp2E=(struct Cyc_Absyn_Unused_att_struct*)_tmp29;if(_tmp2E->tag != 11)goto _LL29;}
_LL28: goto _LL2A;_LL29: {struct Cyc_Absyn_Weak_att_struct*_tmp2F=(struct Cyc_Absyn_Weak_att_struct*)
_tmp29;if(_tmp2F->tag != 12)goto _LL2B;}_LL2A: goto _LL2C;_LL2B: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp30=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp29;if(_tmp30->tag != 13)goto
_LL2D;}_LL2C: goto _LL2E;_LL2D: {struct Cyc_Absyn_Dllexport_att_struct*_tmp31=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp29;if(_tmp31->tag != 14)goto _LL2F;}
_LL2E: continue;_LL2F:;_LL30:{const char*_tmp2B4;void*_tmp2B3[2];struct Cyc_String_pa_struct
_tmp2B2;struct Cyc_String_pa_struct _tmp2B1;(_tmp2B1.tag=0,((_tmp2B1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((
_tmp2B2.tag=0,((_tmp2B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB->hd)),((_tmp2B3[0]=& _tmp2B2,((_tmp2B3[1]=& _tmp2B1,Cyc_Tcutil_terr(
loc,((_tmp2B4="bad attribute %s for variable %s",_tag_dyneither(_tmp2B4,sizeof(
char),33))),_tag_dyneither(_tmp2B3,sizeof(void*),2)))))))))))));}goto _LL1E;_LL1E:;}
if(_tmpA == 0  || in_cinclude){if(check_var_init  && !Cyc_Tcutil_supports_default(
_tmp9)){const char*_tmp2B9;void*_tmp2B8[2];struct Cyc_String_pa_struct _tmp2B7;
struct Cyc_String_pa_struct _tmp2B6;(_tmp2B6.tag=0,((_tmp2B6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9)),((_tmp2B7.tag=
0,((_tmp2B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
vd->name)),((_tmp2B8[0]=& _tmp2B7,((_tmp2B8[1]=& _tmp2B6,Cyc_Tcutil_terr(loc,((
_tmp2B9="initializer required for variable %s of type %s",_tag_dyneither(_tmp2B9,
sizeof(char),48))),_tag_dyneither(_tmp2B8,sizeof(void*),2)))))))))))));}}else{
struct Cyc_Absyn_Exp*_tmp3A=(struct Cyc_Absyn_Exp*)_tmpA;void*_tmp3B=Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp9,_tmp3A);if(!Cyc_Tcutil_coerce_assign(te,_tmp3A,_tmp9)){{const
char*_tmp2BF;void*_tmp2BE[3];struct Cyc_String_pa_struct _tmp2BD;struct Cyc_String_pa_struct
_tmp2BC;struct Cyc_String_pa_struct _tmp2BB;(_tmp2BB.tag=0,((_tmp2BB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3B)),((_tmp2BC.tag=
0,((_tmp2BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp9)),((_tmp2BD.tag=0,((_tmp2BD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp2BE[0]=& _tmp2BD,((
_tmp2BE[1]=& _tmp2BC,((_tmp2BE[2]=& _tmp2BB,Cyc_Tcutil_terr(loc,((_tmp2BF="%s declared with type \n%s\n but initialized with type \n%s",
_tag_dyneither(_tmp2BF,sizeof(char),57))),_tag_dyneither(_tmp2BE,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}if(!Cyc_Tcutil_is_const_exp(te,_tmp3A)){const char*
_tmp2C2;void*_tmp2C1;(_tmp2C1=0,Cyc_Tcutil_terr(loc,((_tmp2C2="initializer is not a constant expression",
_tag_dyneither(_tmp2C2,sizeof(char),41))),_tag_dyneither(_tmp2C1,sizeof(void*),0)));}}}
else{for(0;_tmpB != 0;_tmpB=_tmpB->tl){void*_tmp43=(void*)_tmpB->hd;_LL32: {
struct Cyc_Absyn_Regparm_att_struct*_tmp44=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp43;if(_tmp44->tag != 0)goto _LL34;}_LL33: goto _LL35;_LL34: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp45=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp43;if(_tmp45->tag != 1)goto _LL36;}
_LL35: goto _LL37;_LL36: {struct Cyc_Absyn_Cdecl_att_struct*_tmp46=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp43;if(_tmp46->tag != 2)goto _LL38;}_LL37: goto _LL39;_LL38: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp47=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp43;if(_tmp47->tag != 3)goto _LL3A;}
_LL39: goto _LL3B;_LL3A: {struct Cyc_Absyn_Noreturn_att_struct*_tmp48=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp43;if(_tmp48->tag != 4)goto _LL3C;}_LL3B: goto _LL3D;_LL3C: {struct Cyc_Absyn_Format_att_struct*
_tmp49=(struct Cyc_Absyn_Format_att_struct*)_tmp43;if(_tmp49->tag != 19)goto _LL3E;}
_LL3D: goto _LL3F;_LL3E: {struct Cyc_Absyn_Initializes_att_struct*_tmp4A=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp43;if(_tmp4A->tag != 20)goto _LL40;}_LL3F: goto _LL41;_LL40: {struct Cyc_Absyn_Pure_att_struct*
_tmp4B=(struct Cyc_Absyn_Pure_att_struct*)_tmp43;if(_tmp4B->tag != 22)goto _LL42;}
_LL41: goto _LL43;_LL42: {struct Cyc_Absyn_Const_att_struct*_tmp4C=(struct Cyc_Absyn_Const_att_struct*)
_tmp43;if(_tmp4C->tag != 5)goto _LL44;}_LL43: {const char*_tmp2C5;void*_tmp2C4;(
_tmp2C4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp2C5="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp2C5,
sizeof(char),45))),_tag_dyneither(_tmp2C4,sizeof(void*),0)));}_LL44: {struct Cyc_Absyn_Aligned_att_struct*
_tmp4D=(struct Cyc_Absyn_Aligned_att_struct*)_tmp43;if(_tmp4D->tag != 6)goto _LL46;}
_LL45: goto _LL47;_LL46: {struct Cyc_Absyn_Packed_att_struct*_tmp4E=(struct Cyc_Absyn_Packed_att_struct*)
_tmp43;if(_tmp4E->tag != 7)goto _LL48;}_LL47:{const char*_tmp2C9;void*_tmp2C8[1];
struct Cyc_String_pa_struct _tmp2C7;(_tmp2C7.tag=0,((_tmp2C7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpB->hd)),((
_tmp2C8[0]=& _tmp2C7,Cyc_Tcutil_terr(loc,((_tmp2C9="bad attribute %s in function declaration",
_tag_dyneither(_tmp2C9,sizeof(char),41))),_tag_dyneither(_tmp2C8,sizeof(void*),1)))))));}
goto _LL31;_LL48:;_LL49: continue;_LL31:;}}}{struct _handler_cons _tmp54;
_push_handler(& _tmp54);{int _tmp56=0;if(setjmp(_tmp54.handler))_tmp56=1;if(!
_tmp56){{struct _tuple12*_tmp57=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp8);void*_tmp58=(*_tmp57).f1;
void*_tmp5A;_LL4B: {struct Cyc_Tcenv_VarRes_struct*_tmp59=(struct Cyc_Tcenv_VarRes_struct*)
_tmp58;if(_tmp59->tag != 0)goto _LL4D;else{_tmp5A=(void*)_tmp59->f1;}}_LL4C: {
struct Cyc_Absyn_Global_b_struct _tmp2CC;struct Cyc_Absyn_Global_b_struct*_tmp2CB;
struct Cyc_Absyn_Global_b_struct*_tmp5F=(_tmp2CB=_cycalloc(sizeof(*_tmp2CB)),((
_tmp2CB[0]=((_tmp2CC.tag=1,((_tmp2CC.f1=vd,_tmp2CC)))),_tmp2CB)));void*_tmp60=
Cyc_Tcdecl_merge_binding(_tmp5A,(void*)_tmp5F,loc,Cyc_Tc_tc_msg);if(_tmp60 == (
void*)& Cyc_Absyn_Unresolved_b_val){_npop_handler(0);return;}if(exports == 0  || 
Cyc_Tc_export_member(vd->name,*exports)){if(!(_tmp60 == _tmp5A  && (*_tmp57).f2)){
struct Cyc_Tcenv_VarRes_struct*_tmp2D2;struct Cyc_Tcenv_VarRes_struct _tmp2D1;
struct _tuple12*_tmp2D0;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(
struct _tuple12*)((_tmp2D0=_cycalloc(sizeof(*_tmp2D0)),((_tmp2D0->f1=(void*)((
_tmp2D2=_cycalloc(sizeof(*_tmp2D2)),((_tmp2D2[0]=((_tmp2D1.tag=0,((_tmp2D1.f1=(
void*)_tmp60,_tmp2D1)))),_tmp2D2)))),((_tmp2D0->f2=1,_tmp2D0)))))));}}
_npop_handler(0);return;}_LL4D: {struct Cyc_Tcenv_AggrRes_struct*_tmp5B=(struct
Cyc_Tcenv_AggrRes_struct*)_tmp58;if(_tmp5B->tag != 1)goto _LL4F;}_LL4E:{const char*
_tmp2D5;void*_tmp2D4;(_tmp2D4=0,Cyc_Tcutil_warn(loc,((_tmp2D5="variable declaration shadows previous struct declaration",
_tag_dyneither(_tmp2D5,sizeof(char),57))),_tag_dyneither(_tmp2D4,sizeof(void*),0)));}
goto _LL4A;_LL4F: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp5C=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp58;if(_tmp5C->tag != 2)goto _LL51;}_LL50:{const char*_tmp2D8;void*_tmp2D7;(
_tmp2D7=0,Cyc_Tcutil_warn(loc,((_tmp2D8="variable declaration shadows previous datatype constructor",
_tag_dyneither(_tmp2D8,sizeof(char),59))),_tag_dyneither(_tmp2D7,sizeof(void*),0)));}
goto _LL4A;_LL51: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp5D=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp58;if(_tmp5D->tag != 4)goto _LL53;}_LL52: goto _LL54;_LL53: {struct Cyc_Tcenv_EnumRes_struct*
_tmp5E=(struct Cyc_Tcenv_EnumRes_struct*)_tmp58;if(_tmp5E->tag != 3)goto _LL4A;}
_LL54:{const char*_tmp2DB;void*_tmp2DA;(_tmp2DA=0,Cyc_Tcutil_warn(loc,((_tmp2DB="variable declaration shadows previous enum tag",
_tag_dyneither(_tmp2DB,sizeof(char),47))),_tag_dyneither(_tmp2DA,sizeof(void*),0)));}
goto _LL4A;_LL4A:;};_pop_handler();}else{void*_tmp55=(void*)_exn_thrown;void*
_tmp6D=_tmp55;_LL56: {struct Cyc_Dict_Absent_struct*_tmp6E=(struct Cyc_Dict_Absent_struct*)
_tmp6D;if(_tmp6E->tag != Cyc_Dict_Absent)goto _LL58;}_LL57: goto _LL55;_LL58:;_LL59:(
void)_throw(_tmp6D);_LL55:;}};}if(exports == 0  || Cyc_Tc_export_member(vd->name,*
exports)){struct Cyc_Tcenv_VarRes_struct*_tmp2EA;struct Cyc_Absyn_Global_b_struct*
_tmp2E9;struct Cyc_Absyn_Global_b_struct _tmp2E8;struct Cyc_Tcenv_VarRes_struct
_tmp2E7;struct _tuple12*_tmp2E6;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(
struct _tuple12*)((_tmp2E6=_cycalloc(sizeof(*_tmp2E6)),((_tmp2E6->f1=(void*)((
_tmp2EA=_cycalloc(sizeof(*_tmp2EA)),((_tmp2EA[0]=((_tmp2E7.tag=0,((_tmp2E7.f1=(
void*)((void*)((_tmp2E9=_cycalloc(sizeof(*_tmp2E9)),((_tmp2E9[0]=((_tmp2E8.tag=1,((
_tmp2E8.f1=vd,_tmp2E8)))),_tmp2E9))))),_tmp2E7)))),_tmp2EA)))),((_tmp2E6->f2=0,
_tmp2E6)))))));}}static int Cyc_Tc_is_main(struct _tuple0*n){struct _tuple0 _tmp75;
union Cyc_Absyn_Nmspace _tmp76;struct _dyneither_ptr*_tmp77;struct _tuple0*_tmp74=n;
_tmp75=*_tmp74;_tmp76=_tmp75.f1;_tmp77=_tmp75.f2;{union Cyc_Absyn_Nmspace _tmp78=
_tmp76;struct Cyc_List_List*_tmp79;_LL5B: if((_tmp78.Abs_n).tag != 2)goto _LL5D;
_tmp79=(struct Cyc_List_List*)(_tmp78.Abs_n).val;if(_tmp79 != 0)goto _LL5D;_LL5C: {
const char*_tmp2EB;return Cyc_strcmp((struct _dyneither_ptr)*_tmp77,((_tmp2EB="main",
_tag_dyneither(_tmp2EB,sizeof(char),5))))== 0;}_LL5D:;_LL5E: return 0;_LL5A:;};}
struct _tuple13{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Fndecl*fd,int in_cinclude,struct Cyc_List_List**exports){
struct _dyneither_ptr*v=(*fd->name).f2;if(fd->sc == Cyc_Absyn_ExternC  && !
in_cinclude){const char*_tmp2EE;void*_tmp2ED;(_tmp2ED=0,Cyc_Tcutil_terr(loc,((
_tmp2EE="extern \"C\" functions cannot be implemented in Cyclone",_tag_dyneither(
_tmp2EE,sizeof(char),54))),_tag_dyneither(_tmp2ED,sizeof(void*),0)));}{union Cyc_Absyn_Nmspace
_tmp7D=(*fd->name).f1;struct Cyc_List_List*_tmp7E;struct Cyc_List_List*_tmp7F;
struct Cyc_List_List*_tmp80;_LL60: if((_tmp7D.Rel_n).tag != 1)goto _LL62;_tmp7E=(
struct Cyc_List_List*)(_tmp7D.Rel_n).val;if(_tmp7E != 0)goto _LL62;_LL61: goto _LL5F;
_LL62: if((_tmp7D.C_n).tag != 3)goto _LL64;_tmp7F=(struct Cyc_List_List*)(_tmp7D.C_n).val;
if(_tmp7F != 0)goto _LL64;_LL63: {const char*_tmp2F1;void*_tmp2F0;(_tmp2F0=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2F1="tc: C_n in tcFndecl",
_tag_dyneither(_tmp2F1,sizeof(char),20))),_tag_dyneither(_tmp2F0,sizeof(void*),0)));}
_LL64: if((_tmp7D.Abs_n).tag != 2)goto _LL66;_tmp80=(struct Cyc_List_List*)(_tmp7D.Abs_n).val;
_LL65: {const char*_tmp2F4;void*_tmp2F3;(_tmp2F3=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2F4="tc: Abs_n in tcFndecl",
_tag_dyneither(_tmp2F4,sizeof(char),22))),_tag_dyneither(_tmp2F3,sizeof(void*),0)));}
_LL66:;_LL67:{const char*_tmp2F8;void*_tmp2F7[1];struct Cyc_String_pa_struct
_tmp2F6;(_tmp2F6.tag=0,((_tmp2F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(fd->name)),((_tmp2F7[0]=& _tmp2F6,Cyc_Tcutil_terr(loc,((
_tmp2F8="qualified function declarations are not implemented (%s)",
_tag_dyneither(_tmp2F8,sizeof(char),57))),_tag_dyneither(_tmp2F7,sizeof(void*),1)))))));}
return;_LL5F:;}(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns,fd->sc == Cyc_Absyn_ExternC
 || in_cinclude);Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{void*t=Cyc_Tcutil_fndecl2typ(
fd);fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);{struct Cyc_List_List*
atts=fd->attributes;for(0;atts != 0;atts=atts->tl){void*_tmp88=(void*)atts->hd;
_LL69: {struct Cyc_Absyn_Packed_att_struct*_tmp89=(struct Cyc_Absyn_Packed_att_struct*)
_tmp88;if(_tmp89->tag != 7)goto _LL6B;}_LL6A: goto _LL6C;_LL6B: {struct Cyc_Absyn_Aligned_att_struct*
_tmp8A=(struct Cyc_Absyn_Aligned_att_struct*)_tmp88;if(_tmp8A->tag != 6)goto _LL6D;}
_LL6C:{const char*_tmp2FC;void*_tmp2FB[1];struct Cyc_String_pa_struct _tmp2FA;(
_tmp2FA.tag=0,((_tmp2FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)atts->hd)),((_tmp2FB[0]=& _tmp2FA,Cyc_Tcutil_terr(loc,((_tmp2FC="bad attribute %s for function",
_tag_dyneither(_tmp2FC,sizeof(char),30))),_tag_dyneither(_tmp2FB,sizeof(void*),1)))))));}
goto _LL68;_LL6D:;_LL6E: goto _LL68;_LL68:;}}{struct _handler_cons _tmp8E;
_push_handler(& _tmp8E);{int _tmp90=0;if(setjmp(_tmp8E.handler))_tmp90=1;if(!
_tmp90){{struct _tuple12*_tmp91=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);void*_tmp92=(*_tmp91).f1;
void*_tmp94;_LL70: {struct Cyc_Tcenv_VarRes_struct*_tmp93=(struct Cyc_Tcenv_VarRes_struct*)
_tmp92;if(_tmp93->tag != 0)goto _LL72;else{_tmp94=(void*)_tmp93->f1;}}_LL71: {
struct Cyc_Absyn_Funname_b_struct _tmp2FF;struct Cyc_Absyn_Funname_b_struct*_tmp2FE;
struct Cyc_Absyn_Funname_b_struct*_tmp99=(_tmp2FE=_cycalloc(sizeof(*_tmp2FE)),((
_tmp2FE[0]=((_tmp2FF.tag=2,((_tmp2FF.f1=fd,_tmp2FF)))),_tmp2FE)));void*_tmp9A=
Cyc_Tcdecl_merge_binding(_tmp94,(void*)_tmp99,loc,Cyc_Tc_tc_msg);if(_tmp9A == (
void*)& Cyc_Absyn_Unresolved_b_val)goto _LL6F;if(exports == 0  || Cyc_Tc_export_member(
fd->name,*exports)){if(!(_tmp9A == _tmp94  && (*_tmp91).f2)){struct Cyc_Tcenv_VarRes_struct*
_tmp305;struct Cyc_Tcenv_VarRes_struct _tmp304;struct _tuple12*_tmp303;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple12*v))Cyc_Dict_insert)(ge->ordinaries,v,(struct _tuple12*)((_tmp303=
_cycalloc(sizeof(*_tmp303)),((_tmp303->f1=(void*)((_tmp305=_cycalloc(sizeof(*
_tmp305)),((_tmp305[0]=((_tmp304.tag=0,((_tmp304.f1=(void*)_tmp9A,_tmp304)))),
_tmp305)))),((_tmp303->f2=1,_tmp303)))))));}}goto _LL6F;}_LL72: {struct Cyc_Tcenv_AggrRes_struct*
_tmp95=(struct Cyc_Tcenv_AggrRes_struct*)_tmp92;if(_tmp95->tag != 1)goto _LL74;}
_LL73:{const char*_tmp308;void*_tmp307;(_tmp307=0,Cyc_Tcutil_warn(loc,((_tmp308="function declaration shadows previous type declaration",
_tag_dyneither(_tmp308,sizeof(char),55))),_tag_dyneither(_tmp307,sizeof(void*),0)));}
goto _LL6F;_LL74: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp96=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp92;if(_tmp96->tag != 2)goto _LL76;}_LL75:{const char*_tmp30B;void*_tmp30A;(
_tmp30A=0,Cyc_Tcutil_warn(loc,((_tmp30B="function declaration shadows previous datatype constructor",
_tag_dyneither(_tmp30B,sizeof(char),59))),_tag_dyneither(_tmp30A,sizeof(void*),0)));}
goto _LL6F;_LL76: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp97=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp92;if(_tmp97->tag != 4)goto _LL78;}_LL77: goto _LL79;_LL78: {struct Cyc_Tcenv_EnumRes_struct*
_tmp98=(struct Cyc_Tcenv_EnumRes_struct*)_tmp92;if(_tmp98->tag != 3)goto _LL6F;}
_LL79:{const char*_tmp30E;void*_tmp30D;(_tmp30D=0,Cyc_Tcutil_warn(loc,((_tmp30E="function declaration shadows previous enum tag",
_tag_dyneither(_tmp30E,sizeof(char),47))),_tag_dyneither(_tmp30D,sizeof(void*),0)));}
goto _LL6F;_LL6F:;};_pop_handler();}else{void*_tmp8F=(void*)_exn_thrown;void*
_tmpA7=_tmp8F;_LL7B: {struct Cyc_Dict_Absent_struct*_tmpA8=(struct Cyc_Dict_Absent_struct*)
_tmpA7;if(_tmpA8->tag != Cyc_Dict_Absent)goto _LL7D;}_LL7C: if(exports == 0  || Cyc_Tc_export_member(
fd->name,*exports)){struct Cyc_Tcenv_VarRes_struct*_tmp31D;struct Cyc_Absyn_Funname_b_struct*
_tmp31C;struct Cyc_Absyn_Funname_b_struct _tmp31B;struct Cyc_Tcenv_VarRes_struct
_tmp31A;struct _tuple12*_tmp319;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,v,(
struct _tuple12*)((_tmp319=_cycalloc(sizeof(*_tmp319)),((_tmp319->f1=(void*)((
_tmp31D=_cycalloc(sizeof(*_tmp31D)),((_tmp31D[0]=((_tmp31A.tag=0,((_tmp31A.f1=(
void*)((void*)((_tmp31C=_cycalloc(sizeof(*_tmp31C)),((_tmp31C[0]=((_tmp31B.tag=2,((
_tmp31B.f1=fd,_tmp31B)))),_tmp31C))))),_tmp31A)))),_tmp31D)))),((_tmp319->f2=0,
_tmp319)))))));}goto _LL7A;_LL7D:;_LL7E:(void)_throw(_tmpA7);_LL7A:;}};}if(
in_cinclude)return;{struct _RegionHandle _tmpAE=_new_region("fnrgn");struct
_RegionHandle*fnrgn=& _tmpAE;_push_region(fnrgn);{struct Cyc_Tcenv_Fenv*_tmpAF=Cyc_Tcenv_new_fenv(
fnrgn,loc,fd);struct Cyc_Tcenv_Tenv*_tmp31E;struct Cyc_Tcenv_Tenv*_tmpB0=(_tmp31E=
_region_malloc(fnrgn,sizeof(*_tmp31E)),((_tmp31E->ns=te->ns,((_tmp31E->ae=te->ae,((
_tmp31E->le=(struct Cyc_Tcenv_Fenv*)_tmpAF,((_tmp31E->allow_valueof=0,_tmp31E)))))))));
Cyc_Tcstmt_tcStmt(_tmpB0,fd->body,0);Cyc_Tcenv_check_delayed_effects(_tmpB0);Cyc_Tcenv_check_delayed_constraints(
_tmpB0);if(!Cyc_Tcenv_all_labels_resolved(_tmpB0)){const char*_tmp321;void*
_tmp320;(_tmp320=0,Cyc_Tcutil_terr(loc,((_tmp321="function has goto statements to undefined labels",
_tag_dyneither(_tmp321,sizeof(char),49))),_tag_dyneither(_tmp320,sizeof(void*),0)));}}
if(Cyc_Tc_is_main(fd->name)){{void*_tmpB4=Cyc_Tcutil_compress(fd->ret_type);
enum Cyc_Absyn_Size_of _tmpB7;_LL80: {struct Cyc_Absyn_VoidType_struct*_tmpB5=(
struct Cyc_Absyn_VoidType_struct*)_tmpB4;if(_tmpB5->tag != 0)goto _LL82;}_LL81:{
const char*_tmp324;void*_tmp323;(_tmp323=0,Cyc_Tcutil_warn(loc,((_tmp324="main declared with return type void",
_tag_dyneither(_tmp324,sizeof(char),36))),_tag_dyneither(_tmp323,sizeof(void*),0)));}
goto _LL7F;_LL82: {struct Cyc_Absyn_IntType_struct*_tmpB6=(struct Cyc_Absyn_IntType_struct*)
_tmpB4;if(_tmpB6->tag != 6)goto _LL84;else{_tmpB7=_tmpB6->f2;}}_LL83: goto _LL7F;
_LL84:;_LL85:{const char*_tmp328;void*_tmp327[1];struct Cyc_String_pa_struct
_tmp326;(_tmp326.tag=0,((_tmp326.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type)),((_tmp327[0]=& _tmp326,Cyc_Tcutil_terr(loc,((
_tmp328="main declared with return type %s instead of int or void",
_tag_dyneither(_tmp328,sizeof(char),57))),_tag_dyneither(_tmp327,sizeof(void*),1)))))));}
goto _LL7F;_LL7F:;}if(fd->c_varargs  || fd->cyc_varargs != 0){const char*_tmp32B;
void*_tmp32A;(_tmp32A=0,Cyc_Tcutil_terr(loc,((_tmp32B="main declared with varargs",
_tag_dyneither(_tmp32B,sizeof(char),27))),_tag_dyneither(_tmp32A,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpBF=fd->args;if(_tmpBF != 0){struct _tuple13 _tmpC1;void*
_tmpC2;struct _tuple13*_tmpC0=(struct _tuple13*)_tmpBF->hd;_tmpC1=*_tmpC0;_tmpC2=
_tmpC1.f3;{void*_tmpC3=Cyc_Tcutil_compress(_tmpC2);enum Cyc_Absyn_Size_of _tmpC5;
_LL87: {struct Cyc_Absyn_IntType_struct*_tmpC4=(struct Cyc_Absyn_IntType_struct*)
_tmpC3;if(_tmpC4->tag != 6)goto _LL89;else{_tmpC5=_tmpC4->f2;}}_LL88: goto _LL86;
_LL89:;_LL8A:{const char*_tmp32F;void*_tmp32E[1];struct Cyc_String_pa_struct
_tmp32D;(_tmp32D.tag=0,((_tmp32D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpC2)),((_tmp32E[0]=& _tmp32D,Cyc_Tcutil_terr(loc,((
_tmp32F="main declared with first argument of type %s instead of int",
_tag_dyneither(_tmp32F,sizeof(char),60))),_tag_dyneither(_tmp32E,sizeof(void*),1)))))));}
goto _LL86;_LL86:;}_tmpBF=_tmpBF->tl;if(_tmpBF != 0){struct _tuple13 _tmpCA;void*
_tmpCB;struct _tuple13*_tmpC9=(struct _tuple13*)_tmpBF->hd;_tmpCA=*_tmpC9;_tmpCB=
_tmpCA.f3;_tmpBF=_tmpBF->tl;if(_tmpBF != 0){const char*_tmp332;void*_tmp331;(
_tmp331=0,Cyc_Tcutil_terr(loc,((_tmp332="main declared with too many arguments",
_tag_dyneither(_tmp332,sizeof(char),38))),_tag_dyneither(_tmp331,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp333;struct Cyc_Core_Opt*tvs=(_tmp333=_cycalloc(sizeof(*
_tmp333)),((_tmp333->v=fd->tvs,_tmp333)));struct Cyc_Core_Opt*_tmp33B;struct Cyc_Core_Opt*
_tmp33A;struct Cyc_Core_Opt*_tmp339;struct Cyc_Core_Opt*_tmp338;struct Cyc_Core_Opt*
_tmp337;struct Cyc_Core_Opt*_tmp336;struct Cyc_Core_Opt*_tmp335;struct Cyc_Core_Opt*
_tmp334;if(((!Cyc_Tcutil_unify(_tmpCB,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(
Cyc_Absyn_new_evar(((_tmp33A=_cycalloc(sizeof(*_tmp33A)),((_tmp33A->v=& Cyc_Tcutil_rk,
_tmp33A)))),tvs)),Cyc_Absyn_new_evar(((_tmp33B=_cycalloc(sizeof(*_tmp33B)),((
_tmp33B->v=& Cyc_Tcutil_rk,_tmp33B)))),tvs),Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)())) && !Cyc_Tcutil_unify(_tmpCB,Cyc_Absyn_dyneither_typ(
Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp338=_cycalloc(sizeof(*
_tmp338)),((_tmp338->v=& Cyc_Tcutil_rk,_tmp338)))),tvs)),Cyc_Absyn_new_evar(((
_tmp339=_cycalloc(sizeof(*_tmp339)),((_tmp339->v=& Cyc_Tcutil_rk,_tmp339)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
 && !Cyc_Tcutil_unify(_tmpCB,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((
_tmp336=_cycalloc(sizeof(*_tmp336)),((_tmp336->v=& Cyc_Tcutil_rk,_tmp336)))),tvs)),
Cyc_Absyn_new_evar(((_tmp337=_cycalloc(sizeof(*_tmp337)),((_tmp337->v=& Cyc_Tcutil_rk,
_tmp337)))),tvs),Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
 && !Cyc_Tcutil_unify(_tmpCB,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(
Cyc_Absyn_new_evar(((_tmp334=_cycalloc(sizeof(*_tmp334)),((_tmp334->v=& Cyc_Tcutil_rk,
_tmp334)))),tvs)),Cyc_Absyn_new_evar(((_tmp335=_cycalloc(sizeof(*_tmp335)),((
_tmp335->v=& Cyc_Tcutil_rk,_tmp335)))),tvs),Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)()))){const char*_tmp33F;void*_tmp33E[1];struct Cyc_String_pa_struct
_tmp33D;(_tmp33D.tag=0,((_tmp33D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpCB)),((_tmp33E[0]=& _tmp33D,Cyc_Tcutil_terr(loc,((
_tmp33F="second argument of main has type %s instead of char??",_tag_dyneither(
_tmp33F,sizeof(char),54))),_tag_dyneither(_tmp33E,sizeof(void*),1)))))));}};}}};};
_pop_region(fnrgn);};};}static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*
td){struct _dyneither_ptr*v=(*td->name).f2;{union Cyc_Absyn_Nmspace _tmpDA=(*td->name).f1;
struct Cyc_List_List*_tmpDB;struct Cyc_List_List*_tmpDC;struct Cyc_List_List*_tmpDD;
_LL8C: if((_tmpDA.Rel_n).tag != 1)goto _LL8E;_tmpDB=(struct Cyc_List_List*)(_tmpDA.Rel_n).val;
if(_tmpDB != 0)goto _LL8E;_LL8D: goto _LL8F;_LL8E: if((_tmpDA.C_n).tag != 3)goto _LL90;
_tmpDC=(struct Cyc_List_List*)(_tmpDA.C_n).val;if(_tmpDC != 0)goto _LL90;_LL8F: goto
_LL91;_LL90: if((_tmpDA.Abs_n).tag != 2)goto _LL92;_tmpDD=(struct Cyc_List_List*)(
_tmpDA.Abs_n).val;if(_tmpDD != 0)goto _LL92;_LL91: goto _LL8B;_LL92:;_LL93:{const
char*_tmp343;void*_tmp342[1];struct Cyc_String_pa_struct _tmp341;(_tmp341.tag=0,((
_tmp341.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
td->name)),((_tmp342[0]=& _tmp341,Cyc_Tcutil_terr(loc,((_tmp343="qualified typedef declarations are not implemented (%s)",
_tag_dyneither(_tmp343,sizeof(char),56))),_tag_dyneither(_tmp342,sizeof(void*),1)))))));}
return;_LL8B:;}if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(
ge->typedefs,v)){{const char*_tmp347;void*_tmp346[1];struct Cyc_String_pa_struct
_tmp345;(_tmp345.tag=0,((_tmp345.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
v),((_tmp346[0]=& _tmp345,Cyc_Tcutil_terr(loc,((_tmp347="redeclaration of typedef %s",
_tag_dyneither(_tmp347,sizeof(char),28))),_tag_dyneither(_tmp346,sizeof(void*),1)))))));}
return;}(*td->name).f1=Cyc_Absyn_Abs_n(te->ns,0);Cyc_Tcutil_check_unique_tvars(
loc,td->tvs);Cyc_Tcutil_add_tvar_identities(td->tvs);if(td->defn != 0){Cyc_Tcutil_check_type(
loc,te,td->tvs,& Cyc_Tcutil_tak,0,(void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);(
td->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(
void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);}{struct Cyc_List_List*tvs=
td->tvs;for(0;tvs != 0;tvs=tvs->tl){void*_tmpE4=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmpE6;struct Cyc_Core_Opt**_tmpE7;struct Cyc_Core_Opt*
_tmpE9;struct Cyc_Core_Opt**_tmpEA;struct Cyc_Absyn_Kind*_tmpEB;_LL95: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpE5=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpE4;if(_tmpE5->tag != 1)goto _LL97;
else{_tmpE6=_tmpE5->f1;_tmpE7=(struct Cyc_Core_Opt**)& _tmpE5->f1;}}_LL96: if(td->defn
!= 0){const char*_tmp34B;void*_tmp34A[1];struct Cyc_String_pa_struct _tmp349;(
_tmp349.tag=0,((_tmp349.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp34A[0]=& _tmp349,Cyc_Tcutil_warn(loc,((
_tmp34B="type variable %s is not used in typedef definition",_tag_dyneither(
_tmp34B,sizeof(char),51))),_tag_dyneither(_tmp34A,sizeof(void*),1)))))));}{
struct Cyc_Core_Opt*_tmp34C;*_tmpE7=((_tmp34C=_cycalloc(sizeof(*_tmp34C)),((
_tmp34C->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk),_tmp34C))));}goto _LL94;
_LL97: {struct Cyc_Absyn_Less_kb_struct*_tmpE8=(struct Cyc_Absyn_Less_kb_struct*)
_tmpE4;if(_tmpE8->tag != 2)goto _LL99;else{_tmpE9=_tmpE8->f1;_tmpEA=(struct Cyc_Core_Opt**)&
_tmpE8->f1;_tmpEB=_tmpE8->f2;}}_LL98:{struct Cyc_Core_Opt*_tmp34D;*_tmpEA=((
_tmp34D=_cycalloc(sizeof(*_tmp34D)),((_tmp34D->v=Cyc_Tcutil_kind_to_bound(_tmpEB),
_tmp34D))));}goto _LL94;_LL99:;_LL9A: continue;_LL94:;}}ge->typedefs=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(
ge->typedefs,v,td);}static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct
Cyc_List_List*rpo,struct Cyc_List_List*fields){struct _RegionHandle _tmpF1=
_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpF1;_push_region(
uprev_rgn);for(0;rpo != 0;rpo=rpo->tl){struct _tuple9 _tmpF3;void*_tmpF4;void*
_tmpF5;struct _tuple9*_tmpF2=(struct _tuple9*)rpo->hd;_tmpF3=*_tmpF2;_tmpF4=_tmpF3.f1;
_tmpF5=_tmpF3.f2;Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,_tmpF4);Cyc_Tcutil_check_type(
loc,te,tvs,& Cyc_Tcutil_trk,0,_tmpF5);}{struct Cyc_List_List*prev_fields=0;struct
Cyc_List_List*_tmpF6=fields;for(0;_tmpF6 != 0;_tmpF6=_tmpF6->tl){struct Cyc_Absyn_Aggrfield
_tmpF8;struct _dyneither_ptr*_tmpF9;struct Cyc_Absyn_Tqual _tmpFA;void*_tmpFB;
struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_List_List*_tmpFD;struct Cyc_Absyn_Aggrfield*
_tmpF7=(struct Cyc_Absyn_Aggrfield*)_tmpF6->hd;_tmpF8=*_tmpF7;_tmpF9=_tmpF8.name;
_tmpFA=_tmpF8.tq;_tmpFB=_tmpF8.type;_tmpFC=_tmpF8.width;_tmpFD=_tmpF8.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpF9)){const
char*_tmp351;void*_tmp350[1];struct Cyc_String_pa_struct _tmp34F;(_tmp34F.tag=0,((
_tmp34F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpF9),((_tmp350[0]=&
_tmp34F,Cyc_Tcutil_terr(loc,((_tmp351="duplicate field %s",_tag_dyneither(
_tmp351,sizeof(char),19))),_tag_dyneither(_tmp350,sizeof(void*),1)))))));}{const
char*_tmp352;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpF9,((_tmp352="",
_tag_dyneither(_tmp352,sizeof(char),1))))!= 0){struct Cyc_List_List*_tmp353;
prev_fields=((_tmp353=_region_malloc(uprev_rgn,sizeof(*_tmp353)),((_tmp353->hd=
_tmpF9,((_tmp353->tl=prev_fields,_tmp353))))));}}Cyc_Tcutil_check_type(loc,te,
tvs,& Cyc_Tcutil_tmk,0,_tmpFB);(((struct Cyc_Absyn_Aggrfield*)_tmpF6->hd)->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmpF6->hd)->tq).print_const,
_tmpFB);Cyc_Tcutil_check_bitfield(loc,te,_tmpFB,_tmpFC,_tmpF9);}};;_pop_region(
uprev_rgn);}static void Cyc_Tc_rule_out_memkind(struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int constrain_top_kind){struct Cyc_List_List*
tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){void*_tmp103=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp105;struct Cyc_Core_Opt**
_tmp106;struct Cyc_Core_Opt*_tmp108;struct Cyc_Core_Opt**_tmp109;struct Cyc_Absyn_Kind*
_tmp10A;struct Cyc_Absyn_Kind _tmp10B;enum Cyc_Absyn_KindQual _tmp10C;enum Cyc_Absyn_AliasQual
_tmp10D;struct Cyc_Core_Opt*_tmp10F;struct Cyc_Core_Opt**_tmp110;struct Cyc_Absyn_Kind*
_tmp111;struct Cyc_Absyn_Kind _tmp112;enum Cyc_Absyn_KindQual _tmp113;enum Cyc_Absyn_AliasQual
_tmp114;struct Cyc_Core_Opt*_tmp116;struct Cyc_Core_Opt**_tmp117;struct Cyc_Absyn_Kind*
_tmp118;struct Cyc_Absyn_Kind*_tmp11A;struct Cyc_Absyn_Kind _tmp11B;enum Cyc_Absyn_KindQual
_tmp11C;enum Cyc_Absyn_AliasQual _tmp11D;_LL9C: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp104=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp103;if(_tmp104->tag != 1)goto
_LL9E;else{_tmp105=_tmp104->f1;_tmp106=(struct Cyc_Core_Opt**)& _tmp104->f1;}}
_LL9D:{struct Cyc_Core_Opt*_tmp354;*_tmp106=((_tmp354=_cycalloc(sizeof(*_tmp354)),((
_tmp354->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp354))));}continue;_LL9E: {
struct Cyc_Absyn_Less_kb_struct*_tmp107=(struct Cyc_Absyn_Less_kb_struct*)_tmp103;
if(_tmp107->tag != 2)goto _LLA0;else{_tmp108=_tmp107->f1;_tmp109=(struct Cyc_Core_Opt**)&
_tmp107->f1;_tmp10A=_tmp107->f2;_tmp10B=*_tmp10A;_tmp10C=_tmp10B.kind;if(_tmp10C
!= Cyc_Absyn_MemKind)goto _LLA0;_tmp10D=_tmp10B.aliasqual;}}_LL9F: if(
constrain_top_kind  && _tmp10D == Cyc_Absyn_Top){struct Cyc_Absyn_Kind*_tmp357;
struct Cyc_Core_Opt*_tmp356;*_tmp109=((_tmp356=_cycalloc(sizeof(*_tmp356)),((
_tmp356->v=Cyc_Tcutil_kind_to_bound(((_tmp357=_cycalloc_atomic(sizeof(*_tmp357)),((
_tmp357->kind=Cyc_Absyn_BoxKind,((_tmp357->aliasqual=Cyc_Absyn_Aliasable,_tmp357))))))),
_tmp356))));}else{struct Cyc_Absyn_Kind*_tmp35A;struct Cyc_Core_Opt*_tmp359;*
_tmp109=((_tmp359=_cycalloc(sizeof(*_tmp359)),((_tmp359->v=Cyc_Tcutil_kind_to_bound(((
_tmp35A=_cycalloc_atomic(sizeof(*_tmp35A)),((_tmp35A->kind=Cyc_Absyn_BoxKind,((
_tmp35A->aliasqual=_tmp10D,_tmp35A))))))),_tmp359))));}continue;_LLA0:{struct Cyc_Absyn_Less_kb_struct*
_tmp10E=(struct Cyc_Absyn_Less_kb_struct*)_tmp103;if(_tmp10E->tag != 2)goto _LLA2;
else{_tmp10F=_tmp10E->f1;_tmp110=(struct Cyc_Core_Opt**)& _tmp10E->f1;_tmp111=
_tmp10E->f2;_tmp112=*_tmp111;_tmp113=_tmp112.kind;if(_tmp113 != Cyc_Absyn_BoxKind)
goto _LLA2;_tmp114=_tmp112.aliasqual;if(_tmp114 != Cyc_Absyn_Top)goto _LLA2;}}if(!
constrain_top_kind)goto _LLA2;_LLA1:{struct Cyc_Absyn_Kind*_tmp35D;struct Cyc_Core_Opt*
_tmp35C;*_tmp110=((_tmp35C=_cycalloc(sizeof(*_tmp35C)),((_tmp35C->v=Cyc_Tcutil_kind_to_bound(((
_tmp35D=_cycalloc_atomic(sizeof(*_tmp35D)),((_tmp35D->kind=Cyc_Absyn_BoxKind,((
_tmp35D->aliasqual=Cyc_Absyn_Aliasable,_tmp35D))))))),_tmp35C))));}continue;
_LLA2: {struct Cyc_Absyn_Less_kb_struct*_tmp115=(struct Cyc_Absyn_Less_kb_struct*)
_tmp103;if(_tmp115->tag != 2)goto _LLA4;else{_tmp116=_tmp115->f1;_tmp117=(struct
Cyc_Core_Opt**)& _tmp115->f1;_tmp118=_tmp115->f2;}}_LLA3:{struct Cyc_Core_Opt*
_tmp35E;*_tmp117=((_tmp35E=_cycalloc(sizeof(*_tmp35E)),((_tmp35E->v=Cyc_Tcutil_kind_to_bound(
_tmp118),_tmp35E))));}continue;_LLA4: {struct Cyc_Absyn_Eq_kb_struct*_tmp119=(
struct Cyc_Absyn_Eq_kb_struct*)_tmp103;if(_tmp119->tag != 0)goto _LLA6;else{_tmp11A=
_tmp119->f1;_tmp11B=*_tmp11A;_tmp11C=_tmp11B.kind;if(_tmp11C != Cyc_Absyn_MemKind)
goto _LLA6;_tmp11D=_tmp11B.aliasqual;}}_LLA5:{const char*_tmp366;void*_tmp365[3];
struct Cyc_String_pa_struct _tmp364;struct Cyc_String_pa_struct _tmp363;struct Cyc_Absyn_Kind*
_tmp362;struct Cyc_String_pa_struct _tmp361;(_tmp361.tag=0,((_tmp361.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((_tmp362=
_cycalloc_atomic(sizeof(*_tmp362)),((_tmp362->kind=Cyc_Absyn_MemKind,((_tmp362->aliasqual=
_tmp11D,_tmp362)))))))),((_tmp363.tag=0,((_tmp363.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp364.tag=0,((
_tmp364.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp365[0]=&
_tmp364,((_tmp365[1]=& _tmp363,((_tmp365[2]=& _tmp361,Cyc_Tcutil_terr(loc,((
_tmp366="type %s attempts to abstract type variable %s of kind %s",
_tag_dyneither(_tmp366,sizeof(char),57))),_tag_dyneither(_tmp365,sizeof(void*),3)))))))))))))))))));}
continue;_LLA6:;_LLA7: continue;_LL9B:;}}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*
f1;struct Cyc_Absyn_Aggrdecl***f2;};void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl*
ad){struct _dyneither_ptr*_tmp12C=(*ad->name).f2;{struct Cyc_List_List*atts=ad->attributes;
for(0;atts != 0;atts=atts->tl){void*_tmp12D=(void*)atts->hd;_LLA9: {struct Cyc_Absyn_Packed_att_struct*
_tmp12E=(struct Cyc_Absyn_Packed_att_struct*)_tmp12D;if(_tmp12E->tag != 7)goto
_LLAB;}_LLAA: goto _LLAC;_LLAB: {struct Cyc_Absyn_Aligned_att_struct*_tmp12F=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp12D;if(_tmp12F->tag != 6)goto _LLAD;}_LLAC:
continue;_LLAD:;_LLAE:{const char*_tmp36B;void*_tmp36A[2];struct Cyc_String_pa_struct
_tmp369;struct Cyc_String_pa_struct _tmp368;(_tmp368.tag=0,((_tmp368.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp12C),((_tmp369.tag=0,((_tmp369.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp36A[0]=& _tmp369,((_tmp36A[1]=& _tmp368,Cyc_Tcutil_terr(loc,((
_tmp36B="bad attribute %s in  %s definition",_tag_dyneither(_tmp36B,sizeof(char),
35))),_tag_dyneither(_tmp36A,sizeof(void*),2)))))))))))));}goto _LLA8;_LLA8:;}}{
struct Cyc_List_List*_tmp134=ad->tvs;{union Cyc_Absyn_Nmspace _tmp135=(*ad->name).f1;
struct Cyc_List_List*_tmp136;struct Cyc_List_List*_tmp137;struct Cyc_List_List*
_tmp138;_LLB0: if((_tmp135.Rel_n).tag != 1)goto _LLB2;_tmp136=(struct Cyc_List_List*)(
_tmp135.Rel_n).val;if(_tmp136 != 0)goto _LLB2;_LLB1: goto _LLB3;_LLB2: if((_tmp135.C_n).tag
!= 3)goto _LLB4;_tmp137=(struct Cyc_List_List*)(_tmp135.C_n).val;if(_tmp137 != 0)
goto _LLB4;_LLB3: goto _LLB5;_LLB4: if((_tmp135.Abs_n).tag != 2)goto _LLB6;_tmp138=(
struct Cyc_List_List*)(_tmp135.Abs_n).val;if(_tmp138 != 0)goto _LLB6;_LLB5: goto
_LLAF;_LLB6:;_LLB7:{const char*_tmp36F;void*_tmp36E[1];struct Cyc_String_pa_struct
_tmp36D;(_tmp36D.tag=0,((_tmp36D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(ad->name)),((_tmp36E[0]=& _tmp36D,Cyc_Tcutil_terr(loc,((
_tmp36F="qualified struct declarations are not implemented (%s)",_tag_dyneither(
_tmp36F,sizeof(char),55))),_tag_dyneither(_tmp36E,sizeof(void*),1)))))));}
return;_LLAF:;}(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns,ad->sc == Cyc_Absyn_ExternC);
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);Cyc_Tcutil_add_tvar_identities(ad->tvs);{
struct _tuple14 _tmp370;struct _tuple14 _tmp13D=(_tmp370.f1=ad->impl,((_tmp370.f2=((
struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
ge->aggrdecls,_tmp12C),_tmp370)));struct Cyc_Absyn_AggrdeclImpl*_tmp13E;struct Cyc_Absyn_Aggrdecl***
_tmp13F;struct Cyc_Absyn_AggrdeclImpl*_tmp140;struct Cyc_Absyn_AggrdeclImpl _tmp141;
struct Cyc_List_List*_tmp142;struct Cyc_List_List*_tmp143;struct Cyc_List_List*
_tmp144;int _tmp145;struct Cyc_Absyn_Aggrdecl***_tmp146;struct Cyc_Absyn_AggrdeclImpl*
_tmp147;struct Cyc_Absyn_AggrdeclImpl _tmp148;struct Cyc_List_List*_tmp149;struct
Cyc_List_List*_tmp14A;struct Cyc_List_List*_tmp14B;int _tmp14C;struct Cyc_Absyn_Aggrdecl***
_tmp14D;struct Cyc_Absyn_Aggrdecl**_tmp14E;struct Cyc_Absyn_AggrdeclImpl*_tmp14F;
struct Cyc_Absyn_Aggrdecl***_tmp150;struct Cyc_Absyn_Aggrdecl**_tmp151;_LLB9:
_tmp13E=_tmp13D.f1;if(_tmp13E != 0)goto _LLBB;_tmp13F=_tmp13D.f2;if(_tmp13F != 0)
goto _LLBB;_LLBA: Cyc_Tc_rule_out_memkind(loc,_tmp12C,_tmp134,0);{struct Cyc_Absyn_Aggrdecl**
_tmp371;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,
_tmp12C,((_tmp371=_cycalloc(sizeof(*_tmp371)),((_tmp371[0]=ad,_tmp371)))));}goto
_LLB8;_LLBB: _tmp140=_tmp13D.f1;if(_tmp140 == 0)goto _LLBD;_tmp141=*_tmp140;_tmp142=
_tmp141.exist_vars;_tmp143=_tmp141.rgn_po;_tmp144=_tmp141.fields;_tmp145=_tmp141.tagged;
_tmp146=_tmp13D.f2;if(_tmp146 != 0)goto _LLBD;_LLBC: {struct Cyc_Absyn_Aggrdecl*
_tmp374;struct Cyc_Absyn_Aggrdecl**_tmp373;struct Cyc_Absyn_Aggrdecl**_tmp153=(
_tmp373=_cycalloc(sizeof(*_tmp373)),((_tmp373[0]=((_tmp374=_cycalloc(sizeof(*
_tmp374)),((_tmp374->kind=ad->kind,((_tmp374->sc=Cyc_Absyn_Extern,((_tmp374->name=
ad->name,((_tmp374->tvs=_tmp134,((_tmp374->impl=0,((_tmp374->attributes=ad->attributes,
_tmp374)))))))))))))),_tmp373)));ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(
ge->aggrdecls,_tmp12C,_tmp153);Cyc_Tcutil_check_unique_tvars(loc,_tmp142);Cyc_Tcutil_add_tvar_identities(
_tmp142);if(_tmp145  && ad->kind == Cyc_Absyn_StructA){const char*_tmp377;void*
_tmp376;(_tmp376=0,Cyc_Tcutil_terr(loc,((_tmp377="@tagged qualifier is only allowed on union declarations",
_tag_dyneither(_tmp377,sizeof(char),56))),_tag_dyneither(_tmp376,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmp134,_tmp142),_tmp143,_tmp144);Cyc_Tc_rule_out_memkind(
loc,_tmp12C,_tmp134,0);Cyc_Tc_rule_out_memkind(loc,_tmp12C,_tmp142,1);if(ad->kind
== Cyc_Absyn_UnionA  && !_tmp145){struct Cyc_List_List*f=_tmp144;for(0;f != 0;f=f->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)){const char*
_tmp37D;void*_tmp37C[3];struct Cyc_String_pa_struct _tmp37B;struct Cyc_String_pa_struct
_tmp37A;struct Cyc_String_pa_struct _tmp379;(_tmp379.tag=0,((_tmp379.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)
f->hd)->type)),((_tmp37A.tag=0,((_tmp37A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp12C),((_tmp37B.tag=0,((_tmp37B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp37C[0]=&
_tmp37B,((_tmp37C[1]=& _tmp37A,((_tmp37C[2]=& _tmp379,Cyc_Tcutil_warn(loc,((
_tmp37D="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp37D,sizeof(char),98))),_tag_dyneither(_tmp37C,sizeof(void*),3)))))))))))))))))));}}}*
_tmp153=ad;goto _LLB8;}_LLBD: _tmp147=_tmp13D.f1;if(_tmp147 == 0)goto _LLBF;_tmp148=*
_tmp147;_tmp149=_tmp148.exist_vars;_tmp14A=_tmp148.rgn_po;_tmp14B=_tmp148.fields;
_tmp14C=_tmp148.tagged;_tmp14D=_tmp13D.f2;if(_tmp14D == 0)goto _LLBF;_tmp14E=*
_tmp14D;_LLBE: if(ad->kind != (*_tmp14E)->kind){const char*_tmp380;void*_tmp37F;(
_tmp37F=0,Cyc_Tcutil_terr(loc,((_tmp380="cannot reuse struct names for unions and vice-versa",
_tag_dyneither(_tmp380,sizeof(char),52))),_tag_dyneither(_tmp37F,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*_tmp15F=*_tmp14E;{struct Cyc_Absyn_Aggrdecl*_tmp381;*
_tmp14E=((_tmp381=_cycalloc(sizeof(*_tmp381)),((_tmp381->kind=ad->kind,((_tmp381->sc=
Cyc_Absyn_Extern,((_tmp381->name=ad->name,((_tmp381->tvs=_tmp134,((_tmp381->impl=
0,((_tmp381->attributes=ad->attributes,_tmp381))))))))))))));}Cyc_Tcutil_check_unique_tvars(
loc,_tmp149);Cyc_Tcutil_add_tvar_identities(_tmp149);if(_tmp14C  && ad->kind == 
Cyc_Absyn_StructA){const char*_tmp384;void*_tmp383;(_tmp383=0,Cyc_Tcutil_terr(loc,((
_tmp384="@tagged qualifier is only allowed on union declarations",_tag_dyneither(
_tmp384,sizeof(char),56))),_tag_dyneither(_tmp383,sizeof(void*),0)));}Cyc_Tc_tcAggrImpl(
te,ge,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp134,_tmp149),_tmp14A,_tmp14B);*_tmp14E=_tmp15F;_tmp151=
_tmp14E;goto _LLC0;};_LLBF: _tmp14F=_tmp13D.f1;if(_tmp14F != 0)goto _LLB8;_tmp150=
_tmp13D.f2;if(_tmp150 == 0)goto _LLB8;_tmp151=*_tmp150;_LLC0: {struct Cyc_Absyn_Aggrdecl*
_tmp163=Cyc_Tcdecl_merge_aggrdecl(*_tmp151,ad,loc,Cyc_Tc_tc_msg);if(_tmp163 == 0)
return;else{Cyc_Tc_rule_out_memkind(loc,_tmp12C,_tmp134,0);if(ad->impl != 0)Cyc_Tc_rule_out_memkind(
loc,_tmp12C,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);*
_tmp151=(struct Cyc_Absyn_Aggrdecl*)_tmp163;ad=(struct Cyc_Absyn_Aggrdecl*)_tmp163;
goto _LLB8;}}_LLB8:;}{struct Cyc_Tcenv_AggrRes_struct*_tmp38A;struct Cyc_Tcenv_AggrRes_struct
_tmp389;struct _tuple12*_tmp388;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,
_tmp12C,(struct _tuple12*)((_tmp388=_cycalloc(sizeof(*_tmp388)),((_tmp388->f1=(
void*)((_tmp38A=_cycalloc(sizeof(*_tmp38A)),((_tmp38A[0]=((_tmp389.tag=1,((
_tmp389.f1=ad,_tmp389)))),_tmp38A)))),((_tmp388->f2=1,_tmp388)))))));};};}static
void Cyc_Tc_rule_out_mem_and_unique(struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*v,struct Cyc_List_List*tvs){struct Cyc_List_List*tvs2=tvs;for(0;
tvs2 != 0;tvs2=tvs2->tl){void*_tmp167=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp169;struct Cyc_Core_Opt**_tmp16A;struct
Cyc_Core_Opt*_tmp16C;struct Cyc_Core_Opt**_tmp16D;struct Cyc_Absyn_Kind*_tmp16E;
struct Cyc_Absyn_Kind _tmp16F;enum Cyc_Absyn_KindQual _tmp170;enum Cyc_Absyn_AliasQual
_tmp171;struct Cyc_Core_Opt*_tmp173;struct Cyc_Core_Opt**_tmp174;struct Cyc_Absyn_Kind*
_tmp175;struct Cyc_Absyn_Kind _tmp176;enum Cyc_Absyn_KindQual _tmp177;enum Cyc_Absyn_AliasQual
_tmp178;struct Cyc_Core_Opt*_tmp17A;struct Cyc_Core_Opt**_tmp17B;struct Cyc_Absyn_Kind*
_tmp17C;struct Cyc_Absyn_Kind _tmp17D;enum Cyc_Absyn_KindQual _tmp17E;enum Cyc_Absyn_AliasQual
_tmp17F;struct Cyc_Core_Opt*_tmp181;struct Cyc_Core_Opt**_tmp182;struct Cyc_Absyn_Kind*
_tmp183;struct Cyc_Absyn_Kind _tmp184;enum Cyc_Absyn_KindQual _tmp185;enum Cyc_Absyn_AliasQual
_tmp186;struct Cyc_Core_Opt*_tmp188;struct Cyc_Core_Opt**_tmp189;struct Cyc_Absyn_Kind*
_tmp18A;struct Cyc_Absyn_Kind _tmp18B;enum Cyc_Absyn_KindQual _tmp18C;enum Cyc_Absyn_AliasQual
_tmp18D;struct Cyc_Absyn_Kind*_tmp18F;struct Cyc_Absyn_Kind _tmp190;enum Cyc_Absyn_KindQual
_tmp191;enum Cyc_Absyn_AliasQual _tmp192;struct Cyc_Core_Opt*_tmp194;struct Cyc_Core_Opt**
_tmp195;struct Cyc_Absyn_Kind*_tmp196;struct Cyc_Absyn_Kind _tmp197;enum Cyc_Absyn_KindQual
_tmp198;enum Cyc_Absyn_AliasQual _tmp199;struct Cyc_Absyn_Kind*_tmp19B;struct Cyc_Absyn_Kind
_tmp19C;enum Cyc_Absyn_KindQual _tmp19D;enum Cyc_Absyn_AliasQual _tmp19E;struct Cyc_Absyn_Kind*
_tmp1A0;struct Cyc_Absyn_Kind _tmp1A1;enum Cyc_Absyn_KindQual _tmp1A2;enum Cyc_Absyn_AliasQual
_tmp1A3;_LLC2: {struct Cyc_Absyn_Unknown_kb_struct*_tmp168=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp167;if(_tmp168->tag != 1)goto _LLC4;else{_tmp169=_tmp168->f1;_tmp16A=(struct
Cyc_Core_Opt**)& _tmp168->f1;}}_LLC3: _tmp16D=_tmp16A;goto _LLC5;_LLC4: {struct Cyc_Absyn_Less_kb_struct*
_tmp16B=(struct Cyc_Absyn_Less_kb_struct*)_tmp167;if(_tmp16B->tag != 2)goto _LLC6;
else{_tmp16C=_tmp16B->f1;_tmp16D=(struct Cyc_Core_Opt**)& _tmp16B->f1;_tmp16E=
_tmp16B->f2;_tmp16F=*_tmp16E;_tmp170=_tmp16F.kind;if(_tmp170 != Cyc_Absyn_MemKind)
goto _LLC6;_tmp171=_tmp16F.aliasqual;if(_tmp171 != Cyc_Absyn_Top)goto _LLC6;}}_LLC5:
_tmp174=_tmp16D;goto _LLC7;_LLC6: {struct Cyc_Absyn_Less_kb_struct*_tmp172=(struct
Cyc_Absyn_Less_kb_struct*)_tmp167;if(_tmp172->tag != 2)goto _LLC8;else{_tmp173=
_tmp172->f1;_tmp174=(struct Cyc_Core_Opt**)& _tmp172->f1;_tmp175=_tmp172->f2;
_tmp176=*_tmp175;_tmp177=_tmp176.kind;if(_tmp177 != Cyc_Absyn_MemKind)goto _LLC8;
_tmp178=_tmp176.aliasqual;if(_tmp178 != Cyc_Absyn_Aliasable)goto _LLC8;}}_LLC7:{
struct Cyc_Core_Opt*_tmp38B;*_tmp174=((_tmp38B=_cycalloc(sizeof(*_tmp38B)),((
_tmp38B->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp38B))));}goto _LLC1;_LLC8: {
struct Cyc_Absyn_Less_kb_struct*_tmp179=(struct Cyc_Absyn_Less_kb_struct*)_tmp167;
if(_tmp179->tag != 2)goto _LLCA;else{_tmp17A=_tmp179->f1;_tmp17B=(struct Cyc_Core_Opt**)&
_tmp179->f1;_tmp17C=_tmp179->f2;_tmp17D=*_tmp17C;_tmp17E=_tmp17D.kind;if(_tmp17E
!= Cyc_Absyn_AnyKind)goto _LLCA;_tmp17F=_tmp17D.aliasqual;if(_tmp17F != Cyc_Absyn_Top)
goto _LLCA;}}_LLC9: _tmp182=_tmp17B;goto _LLCB;_LLCA: {struct Cyc_Absyn_Less_kb_struct*
_tmp180=(struct Cyc_Absyn_Less_kb_struct*)_tmp167;if(_tmp180->tag != 2)goto _LLCC;
else{_tmp181=_tmp180->f1;_tmp182=(struct Cyc_Core_Opt**)& _tmp180->f1;_tmp183=
_tmp180->f2;_tmp184=*_tmp183;_tmp185=_tmp184.kind;if(_tmp185 != Cyc_Absyn_AnyKind)
goto _LLCC;_tmp186=_tmp184.aliasqual;if(_tmp186 != Cyc_Absyn_Aliasable)goto _LLCC;}}
_LLCB:{struct Cyc_Core_Opt*_tmp38C;*_tmp182=((_tmp38C=_cycalloc(sizeof(*_tmp38C)),((
_tmp38C->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak),_tmp38C))));}goto _LLC1;_LLCC: {
struct Cyc_Absyn_Less_kb_struct*_tmp187=(struct Cyc_Absyn_Less_kb_struct*)_tmp167;
if(_tmp187->tag != 2)goto _LLCE;else{_tmp188=_tmp187->f1;_tmp189=(struct Cyc_Core_Opt**)&
_tmp187->f1;_tmp18A=_tmp187->f2;_tmp18B=*_tmp18A;_tmp18C=_tmp18B.kind;if(_tmp18C
!= Cyc_Absyn_RgnKind)goto _LLCE;_tmp18D=_tmp18B.aliasqual;if(_tmp18D != Cyc_Absyn_Top)
goto _LLCE;}}_LLCD:{struct Cyc_Core_Opt*_tmp38D;*_tmp189=((_tmp38D=_cycalloc(
sizeof(*_tmp38D)),((_tmp38D->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp38D))));}
goto _LLC1;_LLCE: {struct Cyc_Absyn_Eq_kb_struct*_tmp18E=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp167;if(_tmp18E->tag != 0)goto _LLD0;else{_tmp18F=_tmp18E->f1;_tmp190=*_tmp18F;
_tmp191=_tmp190.kind;if(_tmp191 != Cyc_Absyn_RgnKind)goto _LLD0;_tmp192=_tmp190.aliasqual;
if(_tmp192 != Cyc_Absyn_Top)goto _LLD0;}}_LLCF:{const char*_tmp392;void*_tmp391[2];
struct Cyc_String_pa_struct _tmp390;struct Cyc_String_pa_struct _tmp38F;(_tmp38F.tag=
0,((_tmp38F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
tvs2->hd)->name),((_tmp390.tag=0,((_tmp390.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*v),((_tmp391[0]=& _tmp390,((_tmp391[1]=& _tmp38F,Cyc_Tcutil_terr(
loc,((_tmp392="type %s attempts to abstract type variable %s of kind TR",
_tag_dyneither(_tmp392,sizeof(char),57))),_tag_dyneither(_tmp391,sizeof(void*),2)))))))))))));}
goto _LLC1;_LLD0: {struct Cyc_Absyn_Less_kb_struct*_tmp193=(struct Cyc_Absyn_Less_kb_struct*)
_tmp167;if(_tmp193->tag != 2)goto _LLD2;else{_tmp194=_tmp193->f1;_tmp195=(struct
Cyc_Core_Opt**)& _tmp193->f1;_tmp196=_tmp193->f2;_tmp197=*_tmp196;_tmp198=_tmp197.kind;
_tmp199=_tmp197.aliasqual;if(_tmp199 != Cyc_Absyn_Unique)goto _LLD2;}}_LLD1:
_tmp19D=_tmp198;goto _LLD3;_LLD2: {struct Cyc_Absyn_Eq_kb_struct*_tmp19A=(struct
Cyc_Absyn_Eq_kb_struct*)_tmp167;if(_tmp19A->tag != 0)goto _LLD4;else{_tmp19B=
_tmp19A->f1;_tmp19C=*_tmp19B;_tmp19D=_tmp19C.kind;_tmp19E=_tmp19C.aliasqual;if(
_tmp19E != Cyc_Absyn_Unique)goto _LLD4;}}_LLD3:{const char*_tmp39A;void*_tmp399[3];
struct Cyc_String_pa_struct _tmp398;struct Cyc_String_pa_struct _tmp397;struct Cyc_Absyn_Kind*
_tmp396;struct Cyc_String_pa_struct _tmp395;(_tmp395.tag=0,((_tmp395.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((_tmp396=
_cycalloc_atomic(sizeof(*_tmp396)),((_tmp396->kind=_tmp19D,((_tmp396->aliasqual=
Cyc_Absyn_Unique,_tmp396)))))))),((_tmp397.tag=0,((_tmp397.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((
_tmp398.tag=0,((_tmp398.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((
_tmp399[0]=& _tmp398,((_tmp399[1]=& _tmp397,((_tmp399[2]=& _tmp395,Cyc_Tcutil_terr(
loc,((_tmp39A="type %s attempts to abstract type variable %s of kind %s",
_tag_dyneither(_tmp39A,sizeof(char),57))),_tag_dyneither(_tmp399,sizeof(void*),3)))))))))))))))))));}
goto _LLC1;_LLD4: {struct Cyc_Absyn_Eq_kb_struct*_tmp19F=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp167;if(_tmp19F->tag != 0)goto _LLD6;else{_tmp1A0=_tmp19F->f1;_tmp1A1=*_tmp1A0;
_tmp1A2=_tmp1A1.kind;if(_tmp1A2 != Cyc_Absyn_MemKind)goto _LLD6;_tmp1A3=_tmp1A1.aliasqual;}}
_LLD5:{const char*_tmp3A2;void*_tmp3A1[3];struct Cyc_String_pa_struct _tmp3A0;
struct Cyc_String_pa_struct _tmp39F;struct Cyc_Absyn_Kind*_tmp39E;struct Cyc_String_pa_struct
_tmp39D;(_tmp39D.tag=0,((_tmp39D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp39E=_cycalloc_atomic(sizeof(*_tmp39E)),((_tmp39E->kind=
Cyc_Absyn_MemKind,((_tmp39E->aliasqual=_tmp1A3,_tmp39E)))))))),((_tmp39F.tag=0,((
_tmp39F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
tvs2->hd)->name),((_tmp3A0.tag=0,((_tmp3A0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*v),((_tmp3A1[0]=& _tmp3A0,((_tmp3A1[1]=& _tmp39F,((_tmp3A1[2]=&
_tmp39D,Cyc_Tcutil_terr(loc,((_tmp3A2="type %s attempts to abstract type variable %s of kind %s",
_tag_dyneither(_tmp3A2,sizeof(char),57))),_tag_dyneither(_tmp3A1,sizeof(void*),3)))))))))))))))))));}
goto _LLC1;_LLD6:;_LLD7: goto _LLC1;_LLC1:;}}struct _tuple15{struct Cyc_Absyn_Tqual f1;
void*f2;};static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct _dyneither_ptr
obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*
tvs,struct Cyc_Absyn_Datatypedecl*tudres){{struct Cyc_List_List*_tmp1B7=fields;
for(0;_tmp1B7 != 0;_tmp1B7=_tmp1B7->tl){struct Cyc_Absyn_Datatypefield*_tmp1B8=(
struct Cyc_Absyn_Datatypefield*)_tmp1B7->hd;{struct Cyc_List_List*typs=_tmp1B8->typs;
for(0;typs != 0;typs=typs->tl){Cyc_Tcutil_check_type(_tmp1B8->loc,te,tvs,& Cyc_Tcutil_tmk,
0,(*((struct _tuple15*)typs->hd)).f2);{struct _RegionHandle*_tmp1B9=Cyc_Tcenv_get_fnrgn(
te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1B9,(*((struct _tuple15*)typs->hd)).f2)){
const char*_tmp3A6;void*_tmp3A5[1];struct Cyc_String_pa_struct _tmp3A4;(_tmp3A4.tag=
0,((_tmp3A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp1B8->name)),((_tmp3A5[0]=& _tmp3A4,Cyc_Tcutil_terr(_tmp1B8->loc,((_tmp3A6="noalias pointers in datatypes are not allowed (%s)",
_tag_dyneither(_tmp3A6,sizeof(char),51))),_tag_dyneither(_tmp3A5,sizeof(void*),1)))))));}((*((
struct _tuple15*)typs->hd)).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
_tmp1B8->loc,((*((struct _tuple15*)typs->hd)).f1).print_const,(*((struct _tuple15*)
typs->hd)).f2);};}}{union Cyc_Absyn_Nmspace _tmp1BD=(*_tmp1B8->name).f1;struct Cyc_List_List*
_tmp1BE;struct Cyc_List_List*_tmp1BF;struct Cyc_List_List*_tmp1C0;struct Cyc_List_List*
_tmp1C1;int _tmp1C2;_LLD9: if((_tmp1BD.Rel_n).tag != 1)goto _LLDB;_tmp1BE=(struct Cyc_List_List*)(
_tmp1BD.Rel_n).val;if(_tmp1BE != 0)goto _LLDB;_LLDA: if(is_extensible)(*_tmp1B8->name).f1=
Cyc_Absyn_Abs_n(te->ns,0);else{(*_tmp1B8->name).f1=(*name).f1;}goto _LLD8;_LLDB:
if((_tmp1BD.Rel_n).tag != 1)goto _LLDD;_tmp1BF=(struct Cyc_List_List*)(_tmp1BD.Rel_n).val;
_LLDC:{const char*_tmp3AA;void*_tmp3A9[1];struct Cyc_String_pa_struct _tmp3A8;(
_tmp3A8.tag=0,((_tmp3A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp1B8->name)),((_tmp3A9[0]=& _tmp3A8,Cyc_Tcutil_terr(_tmp1B8->loc,((_tmp3AA="qualified datatypefield declarations are not allowed (%s)",
_tag_dyneither(_tmp3AA,sizeof(char),58))),_tag_dyneither(_tmp3A9,sizeof(void*),1)))))));}
goto _LLD8;_LLDD: if((_tmp1BD.C_n).tag != 3)goto _LLDF;_tmp1C0=(struct Cyc_List_List*)(
_tmp1BD.C_n).val;_LLDE: goto _LLD8;_LLDF: if((_tmp1BD.Abs_n).tag != 2)goto _LLE1;
_tmp1C1=(struct Cyc_List_List*)(_tmp1BD.Abs_n).val;_LLE0: goto _LLD8;_LLE1: if((
_tmp1BD.Loc_n).tag != 4)goto _LLD8;_tmp1C2=(int)(_tmp1BD.Loc_n).val;_LLE2: {const
char*_tmp3AD;void*_tmp3AC;(_tmp3AC=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3AD="tcDatatypeFields: Loc_n",
_tag_dyneither(_tmp3AD,sizeof(char),24))),_tag_dyneither(_tmp3AC,sizeof(void*),0)));}
_LLD8:;};}}{struct Cyc_List_List*fields2;if(is_extensible){int _tmp1C8=1;struct Cyc_List_List*
_tmp1C9=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp1C8,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp1C8)fields2=_tmp1C9;else{fields2=0;}}else{struct _RegionHandle _tmp1CA=
_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1CA;_push_region(
uprev_rgn);{struct Cyc_List_List*prev_fields=0;{struct Cyc_List_List*fs=fields;
for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Datatypefield*_tmp1CB=(struct Cyc_Absyn_Datatypefield*)
fs->hd;if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),
struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,
prev_fields,(*_tmp1CB->name).f2)){const char*_tmp3B2;void*_tmp3B1[2];struct Cyc_String_pa_struct
_tmp3B0;struct Cyc_String_pa_struct _tmp3AF;(_tmp3AF.tag=0,((_tmp3AF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp3B0.tag=0,((_tmp3B0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1CB->name).f2),((_tmp3B1[0]=&
_tmp3B0,((_tmp3B1[1]=& _tmp3AF,Cyc_Tcutil_terr(_tmp1CB->loc,((_tmp3B2="duplicate field name %s in %s",
_tag_dyneither(_tmp3B2,sizeof(char),30))),_tag_dyneither(_tmp3B1,sizeof(void*),2)))))))))))));}
else{struct Cyc_List_List*_tmp3B3;prev_fields=((_tmp3B3=_region_malloc(uprev_rgn,
sizeof(*_tmp3B3)),((_tmp3B3->hd=(*_tmp1CB->name).f2,((_tmp3B3->tl=prev_fields,
_tmp3B3))))));}if(_tmp1CB->sc != Cyc_Absyn_Public){{const char*_tmp3B7;void*
_tmp3B6[1];struct Cyc_String_pa_struct _tmp3B5;(_tmp3B5.tag=0,((_tmp3B5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1CB->name).f2),((_tmp3B6[0]=&
_tmp3B5,Cyc_Tcutil_warn(loc,((_tmp3B7="ignoring scope of field %s",
_tag_dyneither(_tmp3B7,sizeof(char),27))),_tag_dyneither(_tmp3B6,sizeof(void*),1)))))));}
_tmp1CB->sc=Cyc_Absyn_Public;}}}fields2=fields;};_pop_region(uprev_rgn);}{struct
Cyc_List_List*_tmp1D4=fields;for(0;_tmp1D4 != 0;_tmp1D4=_tmp1D4->tl){struct Cyc_Absyn_Datatypefield*
_tmp1D5=(struct Cyc_Absyn_Datatypefield*)_tmp1D4->hd;struct Cyc_Tcenv_DatatypeRes_struct*
_tmp3BD;struct Cyc_Tcenv_DatatypeRes_struct _tmp3BC;struct _tuple12*_tmp3BB;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple12*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1D5->name).f2,(struct _tuple12*)((
_tmp3BB=_cycalloc(sizeof(*_tmp3BB)),((_tmp3BB->f1=(void*)((_tmp3BD=_cycalloc(
sizeof(*_tmp3BD)),((_tmp3BD[0]=((_tmp3BC.tag=2,((_tmp3BC.f1=tudres,((_tmp3BC.f2=
_tmp1D5,_tmp3BC)))))),_tmp3BD)))),((_tmp3BB->f2=1,_tmp3BB)))))));}}return fields2;};}
struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};void Cyc_Tc_tcDatatypedecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Datatypedecl*tud){struct _dyneither_ptr*v=(*tud->name).f2;const
char*_tmp3BF;const char*_tmp3BE;struct _dyneither_ptr obj=tud->is_extensible?(
_tmp3BF="@extensible datatype",_tag_dyneither(_tmp3BF,sizeof(char),21)):((
_tmp3BE="datatype",_tag_dyneither(_tmp3BE,sizeof(char),9)));struct Cyc_List_List*
tvs=tud->tvs;Cyc_Tcutil_check_unique_tvars(loc,tvs);Cyc_Tcutil_add_tvar_identities(
tvs);{struct _RegionHandle _tmp1D9=_new_region("temp");struct _RegionHandle*temp=&
_tmp1D9;_push_region(temp);{struct Cyc_Absyn_Datatypedecl***tud_opt;{struct
_handler_cons _tmp1DA;_push_handler(& _tmp1DA);{int _tmp1DC=0;if(setjmp(_tmp1DA.handler))
_tmp1DC=1;if(!_tmp1DC){tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible){struct Cyc_Dict_Absent_struct _tmp3C2;
struct Cyc_Dict_Absent_struct*_tmp3C1;(int)_throw((void*)((_tmp3C1=
_cycalloc_atomic(sizeof(*_tmp3C1)),((_tmp3C1[0]=((_tmp3C2.tag=Cyc_Dict_Absent,
_tmp3C2)),_tmp3C1)))));}if(tud_opt != 0)tud->name=(*(*tud_opt))->name;else{(*tud->name).f1=
Cyc_Absyn_Abs_n(te->ns,0);};_pop_handler();}else{void*_tmp1DB=(void*)_exn_thrown;
void*_tmp1E0=_tmp1DB;_LLE4: {struct Cyc_Dict_Absent_struct*_tmp1E1=(struct Cyc_Dict_Absent_struct*)
_tmp1E0;if(_tmp1E1->tag != Cyc_Dict_Absent)goto _LLE6;}_LLE5:{union Cyc_Absyn_Nmspace
_tmp1E2=(*tud->name).f1;struct Cyc_List_List*_tmp1E3;struct Cyc_List_List*_tmp1E4;
_LLE9: if((_tmp1E2.Rel_n).tag != 1)goto _LLEB;_tmp1E3=(struct Cyc_List_List*)(
_tmp1E2.Rel_n).val;if(_tmp1E3 != 0)goto _LLEB;_LLEA:(*tud->name).f1=Cyc_Absyn_Abs_n(
te->ns,0);goto _LLE8;_LLEB: if((_tmp1E2.Abs_n).tag != 2)goto _LLED;_tmp1E4=(struct
Cyc_List_List*)(_tmp1E2.Abs_n).val;_LLEC: goto _LLEE;_LLED:;_LLEE:{const char*
_tmp3C6;void*_tmp3C5[1];struct Cyc_String_pa_struct _tmp3C4;(_tmp3C4.tag=0,((
_tmp3C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tud->name)),((_tmp3C5[0]=& _tmp3C4,Cyc_Tcutil_terr(loc,((_tmp3C6="qualified (non-extensible) datatype declarations are not implemented (%s)",
_tag_dyneither(_tmp3C6,sizeof(char),74))),_tag_dyneither(_tmp3C5,sizeof(void*),1)))))));}
_npop_handler(0);return;_LLE8:;}{struct Cyc_Absyn_Datatypedecl***_tmp1E8=((struct
Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
ge->datatypedecls,v);{struct Cyc_Absyn_Datatypedecl***_tmp3C7;tud_opt=(
unsigned int)_tmp1E8?(_tmp3C7=_region_malloc(temp,sizeof(*_tmp3C7)),((_tmp3C7[0]=*
_tmp1E8,_tmp3C7))): 0;}goto _LLE3;};_LLE6:;_LLE7:(void)_throw(_tmp1E0);_LLE3:;}};}{
struct _tuple16 _tmp3C8;struct _tuple16 _tmp1EB=(_tmp3C8.f1=tud->fields,((_tmp3C8.f2=
tud_opt,_tmp3C8)));struct Cyc_Core_Opt*_tmp1EC;struct Cyc_Absyn_Datatypedecl***
_tmp1ED;struct Cyc_Core_Opt*_tmp1EE;struct Cyc_Core_Opt _tmp1EF;struct Cyc_List_List*
_tmp1F0;struct Cyc_List_List**_tmp1F1;struct Cyc_Absyn_Datatypedecl***_tmp1F2;
struct Cyc_Core_Opt*_tmp1F3;struct Cyc_Core_Opt _tmp1F4;struct Cyc_List_List*_tmp1F5;
struct Cyc_List_List**_tmp1F6;struct Cyc_Absyn_Datatypedecl***_tmp1F7;struct Cyc_Absyn_Datatypedecl**
_tmp1F8;struct Cyc_Core_Opt*_tmp1F9;struct Cyc_Absyn_Datatypedecl***_tmp1FA;struct
Cyc_Absyn_Datatypedecl**_tmp1FB;_LLF0: _tmp1EC=_tmp1EB.f1;if(_tmp1EC != 0)goto
_LLF2;_tmp1ED=_tmp1EB.f2;if(_tmp1ED != 0)goto _LLF2;_LLF1: Cyc_Tc_rule_out_mem_and_unique(
loc,v,tvs);{struct Cyc_Absyn_Datatypedecl**_tmp3C9;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(
ge->datatypedecls,v,((_tmp3C9=_cycalloc(sizeof(*_tmp3C9)),((_tmp3C9[0]=tud,
_tmp3C9)))));}goto _LLEF;_LLF2: _tmp1EE=_tmp1EB.f1;if(_tmp1EE == 0)goto _LLF4;
_tmp1EF=*_tmp1EE;_tmp1F0=(struct Cyc_List_List*)_tmp1EF.v;_tmp1F1=(struct Cyc_List_List**)&(*
_tmp1EB.f1).v;_tmp1F2=_tmp1EB.f2;if(_tmp1F2 != 0)goto _LLF4;_LLF3: {struct Cyc_Absyn_Datatypedecl*
_tmp3CC;struct Cyc_Absyn_Datatypedecl**_tmp3CB;struct Cyc_Absyn_Datatypedecl**
_tmp1FD=(_tmp3CB=_cycalloc(sizeof(*_tmp3CB)),((_tmp3CB[0]=((_tmp3CC=_cycalloc(
sizeof(*_tmp3CC)),((_tmp3CC->sc=Cyc_Absyn_Extern,((_tmp3CC->name=tud->name,((
_tmp3CC->tvs=tvs,((_tmp3CC->fields=0,((_tmp3CC->is_extensible=tud->is_extensible,
_tmp3CC)))))))))))),_tmp3CB)));ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(
ge->datatypedecls,v,_tmp1FD);*_tmp1F1=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,
tud->name,*_tmp1F1,tvs,tud);Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);*_tmp1FD=
tud;goto _LLEF;}_LLF4: _tmp1F3=_tmp1EB.f1;if(_tmp1F3 == 0)goto _LLF6;_tmp1F4=*
_tmp1F3;_tmp1F5=(struct Cyc_List_List*)_tmp1F4.v;_tmp1F6=(struct Cyc_List_List**)&(*
_tmp1EB.f1).v;_tmp1F7=_tmp1EB.f2;if(_tmp1F7 == 0)goto _LLF6;_tmp1F8=*_tmp1F7;_LLF5: {
struct Cyc_Absyn_Datatypedecl*_tmp200=*_tmp1F8;if((!tud->is_extensible  && (
unsigned int)_tmp200) && _tmp200->is_extensible)tud->is_extensible=1;{struct Cyc_Absyn_Datatypedecl*
_tmp3CD;*_tmp1F8=((_tmp3CD=_cycalloc(sizeof(*_tmp3CD)),((_tmp3CD->sc=Cyc_Absyn_Extern,((
_tmp3CD->name=tud->name,((_tmp3CD->tvs=tvs,((_tmp3CD->fields=0,((_tmp3CD->is_extensible=
tud->is_extensible,_tmp3CD))))))))))));}*_tmp1F6=Cyc_Tc_tcDatatypeFields(te,ge,
loc,obj,tud->is_extensible,tud->name,*_tmp1F6,tvs,tud);*_tmp1F8=_tmp200;_tmp1FB=
_tmp1F8;goto _LLF7;}_LLF6: _tmp1F9=_tmp1EB.f1;if(_tmp1F9 != 0)goto _LLEF;_tmp1FA=
_tmp1EB.f2;if(_tmp1FA == 0)goto _LLEF;_tmp1FB=*_tmp1FA;_LLF7: {struct Cyc_Absyn_Datatypedecl*
_tmp202=Cyc_Tcdecl_merge_datatypedecl(*_tmp1FB,tud,loc,Cyc_Tc_tc_msg);Cyc_Tc_rule_out_mem_and_unique(
loc,v,tvs);if(_tmp202 == 0){_npop_handler(0);return;}else{*_tmp1FB=(struct Cyc_Absyn_Datatypedecl*)
_tmp202;goto _LLEF;}}_LLEF:;};};_pop_region(temp);};}void Cyc_Tc_tcEnumdecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct
Cyc_Absyn_Enumdecl*ed){struct _dyneither_ptr*v=(*ed->name).f2;{union Cyc_Absyn_Nmspace
_tmp205=(*ed->name).f1;struct Cyc_List_List*_tmp206;struct Cyc_List_List*_tmp207;
_LLF9: if((_tmp205.Rel_n).tag != 1)goto _LLFB;_tmp206=(struct Cyc_List_List*)(
_tmp205.Rel_n).val;if(_tmp206 != 0)goto _LLFB;_LLFA: goto _LLFC;_LLFB: if((_tmp205.Abs_n).tag
!= 2)goto _LLFD;_tmp207=(struct Cyc_List_List*)(_tmp205.Abs_n).val;if(_tmp207 != 0)
goto _LLFD;_LLFC: goto _LLF8;_LLFD:;_LLFE:{const char*_tmp3D1;void*_tmp3D0[1];struct
Cyc_String_pa_struct _tmp3CF;(_tmp3CF.tag=0,((_tmp3CF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(ed->name)),((_tmp3D0[0]=& _tmp3CF,
Cyc_Tcutil_terr(loc,((_tmp3D1="qualified enum declarations are not implemented (%s)",
_tag_dyneither(_tmp3D1,sizeof(char),53))),_tag_dyneither(_tmp3D0,sizeof(void*),1)))))));}
return;_LLF8:;}(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);
if(ed->fields != 0){struct _RegionHandle _tmp20B=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp20B;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;struct Cyc_List_List*fs=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct
Cyc_Absyn_Enumfield*_tmp20C=(struct Cyc_Absyn_Enumfield*)fs->hd;if(((int(*)(int(*
compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,
struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp20C->name).f2)){
const char*_tmp3D5;void*_tmp3D4[1];struct Cyc_String_pa_struct _tmp3D3;(_tmp3D3.tag=
0,((_tmp3D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp20C->name).f2),((
_tmp3D4[0]=& _tmp3D3,Cyc_Tcutil_terr(_tmp20C->loc,((_tmp3D5="duplicate field name %s",
_tag_dyneither(_tmp3D5,sizeof(char),24))),_tag_dyneither(_tmp3D4,sizeof(void*),1)))))));}
else{struct Cyc_List_List*_tmp3D6;prev_fields=((_tmp3D6=_region_malloc(uprev_rgn,
sizeof(*_tmp3D6)),((_tmp3D6->hd=(*_tmp20C->name).f2,((_tmp3D6->tl=prev_fields,
_tmp3D6))))));}if(_tmp20C->tag == 0)_tmp20C->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp20C->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp20C->tag))){const char*_tmp3DB;void*_tmp3DA[2];struct Cyc_String_pa_struct
_tmp3D9;struct Cyc_String_pa_struct _tmp3D8;(_tmp3D8.tag=0,((_tmp3D8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp20C->name).f2),((_tmp3D9.tag=0,((
_tmp3D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3DA[0]=&
_tmp3D9,((_tmp3DA[1]=& _tmp3D8,Cyc_Tcutil_terr(loc,((_tmp3DB="enum %s, field %s: expression is not constant",
_tag_dyneither(_tmp3DB,sizeof(char),46))),_tag_dyneither(_tmp3DA,sizeof(void*),2)))))))))))));}}{
unsigned int _tmp216;int _tmp217;struct _tuple10 _tmp215=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp20C->tag));_tmp216=_tmp215.f1;_tmp217=
_tmp215.f2;if(!_tmp217){const char*_tmp3DE;void*_tmp3DD;(_tmp3DD=0,Cyc_Tcutil_terr(
loc,((_tmp3DE="Cyclone enum tags cannot use sizeof or offsetof",_tag_dyneither(
_tmp3DE,sizeof(char),48))),_tag_dyneither(_tmp3DD,sizeof(void*),0)));}tag_count=
_tmp216 + 1;(*_tmp20C->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);};}};
_pop_region(uprev_rgn);}{struct _handler_cons _tmp21A;_push_handler(& _tmp21A);{int
_tmp21C=0;if(setjmp(_tmp21A.handler))_tmp21C=1;if(!_tmp21C){{struct Cyc_Absyn_Enumdecl**
_tmp21D=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);struct Cyc_Absyn_Enumdecl*
_tmp21E=Cyc_Tcdecl_merge_enumdecl(*_tmp21D,ed,loc,Cyc_Tc_tc_msg);if(_tmp21E == 0){
_npop_handler(0);return;}*_tmp21D=(struct Cyc_Absyn_Enumdecl*)_tmp21E;};
_pop_handler();}else{void*_tmp21B=(void*)_exn_thrown;void*_tmp220=_tmp21B;_LL100: {
struct Cyc_Dict_Absent_struct*_tmp221=(struct Cyc_Dict_Absent_struct*)_tmp220;if(
_tmp221->tag != Cyc_Dict_Absent)goto _LL102;}_LL101: {struct Cyc_Absyn_Enumdecl**
_tmp3DF;struct Cyc_Absyn_Enumdecl**_tmp222=(_tmp3DF=_cycalloc(sizeof(*_tmp3DF)),((
_tmp3DF[0]=ed,_tmp3DF)));ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,
v,_tmp222);goto _LLFF;}_LL102:;_LL103:(void)_throw(_tmp220);_LLFF:;}};}if(ed->fields
!= 0){struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp224=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Tcenv_EnumRes_struct*
_tmp3E5;struct Cyc_Tcenv_EnumRes_struct _tmp3E4;struct _tuple12*_tmp3E3;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple12*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp224->name).f2,(struct _tuple12*)((
_tmp3E3=_cycalloc(sizeof(*_tmp3E3)),((_tmp3E3->f1=(void*)((_tmp3E5=_cycalloc(
sizeof(*_tmp3E5)),((_tmp3E5[0]=((_tmp3E4.tag=3,((_tmp3E4.f1=ed,((_tmp3E4.f2=
_tmp224,_tmp3E4)))))),_tmp3E5)))),((_tmp3E3->f2=1,_tmp3E3)))))));}}}static int Cyc_Tc_okay_externC(
struct Cyc_Position_Segment*loc,enum Cyc_Absyn_Scope sc,int in_include){switch(sc){
case Cyc_Absyn_Static: _LL104: if(!in_include){const char*_tmp3E8;void*_tmp3E7;(
_tmp3E7=0,Cyc_Tcutil_warn(loc,((_tmp3E8="static declaration nested within extern \"C\"",
_tag_dyneither(_tmp3E8,sizeof(char),44))),_tag_dyneither(_tmp3E7,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Abstract: _LL105:{const char*_tmp3EB;void*_tmp3EA;(_tmp3EA=0,
Cyc_Tcutil_warn(loc,((_tmp3EB="abstract declaration nested within extern \"C\"",
_tag_dyneither(_tmp3EB,sizeof(char),46))),_tag_dyneither(_tmp3EA,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Public: _LL106: goto _LL107;case Cyc_Absyn_Register: _LL107:
goto _LL108;case Cyc_Absyn_Extern: _LL108: return 1;case Cyc_Absyn_ExternC: _LL109:{
const char*_tmp3EE;void*_tmp3ED;(_tmp3ED=0,Cyc_Tcutil_warn(loc,((_tmp3EE="nested extern \"C\" declaration",
_tag_dyneither(_tmp3EE,sizeof(char),30))),_tag_dyneither(_tmp3ED,sizeof(void*),0)));}
return 1;}}static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,
struct _tuple11*exp){struct Cyc_Position_Segment*_tmp22F;struct _tuple0*_tmp230;
struct _tuple11 _tmp22E=*exp;_tmp22F=_tmp22E.f1;_tmp230=_tmp22E.f2;{struct _tuple0
_tmp232;union Cyc_Absyn_Nmspace _tmp233;struct _dyneither_ptr*_tmp234;struct _tuple0*
_tmp231=_tmp230;_tmp232=*_tmp231;_tmp233=_tmp232.f1;_tmp234=_tmp232.f2;{union Cyc_Absyn_Nmspace
_tmp235=_tmp233;struct Cyc_List_List*_tmp236;struct Cyc_List_List*_tmp237;struct
Cyc_List_List*_tmp238;_LL10C: if((_tmp235.Rel_n).tag != 1)goto _LL10E;_tmp236=(
struct Cyc_List_List*)(_tmp235.Rel_n).val;if(_tmp236 != 0)goto _LL10E;_LL10D: goto
_LL10F;_LL10E: if((_tmp235.C_n).tag != 3)goto _LL110;_tmp237=(struct Cyc_List_List*)(
_tmp235.C_n).val;if(_tmp237 != 0)goto _LL110;_LL10F: goto _LL10B;_LL110: if((_tmp235.Abs_n).tag
!= 2)goto _LL112;_tmp238=(struct Cyc_List_List*)(_tmp235.Abs_n).val;if(_tmp238 != 0)
goto _LL112;_LL111: goto _LL10B;_LL112:;_LL113:{const char*_tmp3F2;void*_tmp3F1[1];
struct Cyc_String_pa_struct _tmp3F0;(_tmp3F0.tag=0,((_tmp3F0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp230)),((
_tmp3F1[0]=& _tmp3F0,Cyc_Tcutil_terr(_tmp22F,((_tmp3F2="qualified export variables are not implemented (%s)",
_tag_dyneither(_tmp3F2,sizeof(char),52))),_tag_dyneither(_tmp3F1,sizeof(void*),1)))))));}
return;_LL10B:;}(*_tmp230).f1=Cyc_Absyn_Abs_n(te->ns,1);};}static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int
in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**
exports){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_List_List*last=
0;struct Cyc_List_List*_tmp23C=ds0;for(0;_tmp23C != 0;(last=_tmp23C,_tmp23C=
_tmp23C->tl)){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp23C->hd;struct
Cyc_Position_Segment*loc=d->loc;void*_tmp23D=d->r;struct Cyc_Absyn_Vardecl*
_tmp243;struct Cyc_Absyn_Fndecl*_tmp245;struct Cyc_Absyn_Typedefdecl*_tmp247;
struct Cyc_Absyn_Aggrdecl*_tmp249;struct Cyc_Absyn_Datatypedecl*_tmp24B;struct Cyc_Absyn_Enumdecl*
_tmp24D;struct _dyneither_ptr*_tmp251;struct Cyc_List_List*_tmp252;struct _tuple0*
_tmp254;struct _tuple0 _tmp255;union Cyc_Absyn_Nmspace _tmp256;struct _dyneither_ptr*
_tmp257;struct Cyc_List_List*_tmp258;struct Cyc_List_List*_tmp25A;struct Cyc_List_List*
_tmp25C;struct Cyc_List_List*_tmp25D;_LL115: {struct Cyc_Absyn_Let_d_struct*
_tmp23E=(struct Cyc_Absyn_Let_d_struct*)_tmp23D;if(_tmp23E->tag != 2)goto _LL117;}
_LL116: goto _LL118;_LL117: {struct Cyc_Absyn_Letv_d_struct*_tmp23F=(struct Cyc_Absyn_Letv_d_struct*)
_tmp23D;if(_tmp23F->tag != 3)goto _LL119;}_LL118:{const char*_tmp3F5;void*_tmp3F4;(
_tmp3F4=0,Cyc_Tcutil_terr(loc,((_tmp3F5="top level let-declarations are not implemented",
_tag_dyneither(_tmp3F5,sizeof(char),47))),_tag_dyneither(_tmp3F4,sizeof(void*),0)));}
goto _LL114;_LL119: {struct Cyc_Absyn_Region_d_struct*_tmp240=(struct Cyc_Absyn_Region_d_struct*)
_tmp23D;if(_tmp240->tag != 4)goto _LL11B;}_LL11A:{const char*_tmp3F8;void*_tmp3F7;(
_tmp3F7=0,Cyc_Tcutil_terr(loc,((_tmp3F8="top level region declarations are not implemented",
_tag_dyneither(_tmp3F8,sizeof(char),50))),_tag_dyneither(_tmp3F7,sizeof(void*),0)));}
goto _LL114;_LL11B: {struct Cyc_Absyn_Alias_d_struct*_tmp241=(struct Cyc_Absyn_Alias_d_struct*)
_tmp23D;if(_tmp241->tag != 5)goto _LL11D;}_LL11C:{const char*_tmp3FB;void*_tmp3FA;(
_tmp3FA=0,Cyc_Tcutil_terr(loc,((_tmp3FB="top level alias declarations are not implemented",
_tag_dyneither(_tmp3FB,sizeof(char),49))),_tag_dyneither(_tmp3FA,sizeof(void*),0)));}
goto _LL114;_LL11D: {struct Cyc_Absyn_Var_d_struct*_tmp242=(struct Cyc_Absyn_Var_d_struct*)
_tmp23D;if(_tmp242->tag != 0)goto _LL11F;else{_tmp243=_tmp242->f1;}}_LL11E: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp243->sc,in_externCinclude))_tmp243->sc=
Cyc_Absyn_ExternC;Cyc_Tc_tcVardecl(te,ge,loc,_tmp243,check_var_init,
in_externCinclude,exports);goto _LL114;_LL11F: {struct Cyc_Absyn_Fn_d_struct*
_tmp244=(struct Cyc_Absyn_Fn_d_struct*)_tmp23D;if(_tmp244->tag != 1)goto _LL121;
else{_tmp245=_tmp244->f1;}}_LL120: if(in_externC  && Cyc_Tc_okay_externC(d->loc,
_tmp245->sc,in_externCinclude))_tmp245->sc=Cyc_Absyn_ExternC;Cyc_Tc_tcFndecl(te,
ge,loc,_tmp245,in_externCinclude,exports);goto _LL114;_LL121: {struct Cyc_Absyn_Typedef_d_struct*
_tmp246=(struct Cyc_Absyn_Typedef_d_struct*)_tmp23D;if(_tmp246->tag != 9)goto
_LL123;else{_tmp247=_tmp246->f1;}}_LL122: Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp247);
goto _LL114;_LL123: {struct Cyc_Absyn_Aggr_d_struct*_tmp248=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp23D;if(_tmp248->tag != 6)goto _LL125;else{_tmp249=_tmp248->f1;}}_LL124: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp249->sc,in_externCinclude))_tmp249->sc=
Cyc_Absyn_ExternC;Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp249);goto _LL114;_LL125: {
struct Cyc_Absyn_Datatype_d_struct*_tmp24A=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp23D;if(_tmp24A->tag != 7)goto _LL127;else{_tmp24B=_tmp24A->f1;}}_LL126: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp24B->sc,in_externCinclude))_tmp24B->sc=
Cyc_Absyn_ExternC;Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp24B);goto _LL114;_LL127: {
struct Cyc_Absyn_Enum_d_struct*_tmp24C=(struct Cyc_Absyn_Enum_d_struct*)_tmp23D;
if(_tmp24C->tag != 8)goto _LL129;else{_tmp24D=_tmp24C->f1;}}_LL128: if(in_externC
 && Cyc_Tc_okay_externC(d->loc,_tmp24D->sc,in_externCinclude))_tmp24D->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp24D);goto _LL114;_LL129: {struct Cyc_Absyn_Porton_d_struct*
_tmp24E=(struct Cyc_Absyn_Porton_d_struct*)_tmp23D;if(_tmp24E->tag != 14)goto
_LL12B;}_LL12A:{const char*_tmp3FE;void*_tmp3FD;(_tmp3FD=0,Cyc_Tcutil_warn(d->loc,((
_tmp3FE="spurious __cyclone_port_on__",_tag_dyneither(_tmp3FE,sizeof(char),29))),
_tag_dyneither(_tmp3FD,sizeof(void*),0)));}goto _LL114;_LL12B: {struct Cyc_Absyn_Portoff_d_struct*
_tmp24F=(struct Cyc_Absyn_Portoff_d_struct*)_tmp23D;if(_tmp24F->tag != 15)goto
_LL12D;}_LL12C: goto _LL114;_LL12D: {struct Cyc_Absyn_Namespace_d_struct*_tmp250=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp23D;if(_tmp250->tag != 10)goto _LL12F;
else{_tmp251=_tmp250->f1;_tmp252=_tmp250->f2;}}_LL12E: {struct Cyc_List_List*
_tmp266=te->ns;struct Cyc_List_List*_tmp3FF;struct Cyc_List_List*_tmp267=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp266,((_tmp3FF=_cycalloc(sizeof(*_tmp3FF)),((_tmp3FF->hd=_tmp251,((_tmp3FF->tl=
0,_tmp3FF)))))));if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
ge->namespaces,_tmp251)){ge->namespaces=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(
grgn,ge->namespaces,_tmp251);te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp267,
Cyc_Tcenv_empty_genv(grgn));}te->ns=_tmp267;Cyc_Tc_tc_decls(te,_tmp252,
in_externC,in_externCinclude,check_var_init,grgn,exports);te->ns=_tmp266;goto
_LL114;}_LL12F: {struct Cyc_Absyn_Using_d_struct*_tmp253=(struct Cyc_Absyn_Using_d_struct*)
_tmp23D;if(_tmp253->tag != 11)goto _LL131;else{_tmp254=_tmp253->f1;_tmp255=*
_tmp254;_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;_tmp258=_tmp253->f2;}}_LL130: {
struct _dyneither_ptr*first;struct Cyc_List_List*rest;{union Cyc_Absyn_Nmspace
_tmp269=_tmp256;int _tmp26A;struct Cyc_List_List*_tmp26B;struct Cyc_List_List*
_tmp26C;struct Cyc_List_List*_tmp26D;struct Cyc_List_List*_tmp26E;struct Cyc_List_List
_tmp26F;struct _dyneither_ptr*_tmp270;struct Cyc_List_List*_tmp271;struct Cyc_List_List*
_tmp272;struct Cyc_List_List _tmp273;struct _dyneither_ptr*_tmp274;struct Cyc_List_List*
_tmp275;struct Cyc_List_List*_tmp276;struct Cyc_List_List _tmp277;struct
_dyneither_ptr*_tmp278;struct Cyc_List_List*_tmp279;_LL136: if((_tmp269.Loc_n).tag
!= 4)goto _LL138;_tmp26A=(int)(_tmp269.Loc_n).val;_LL137: goto _LL139;_LL138: if((
_tmp269.Rel_n).tag != 1)goto _LL13A;_tmp26B=(struct Cyc_List_List*)(_tmp269.Rel_n).val;
if(_tmp26B != 0)goto _LL13A;_LL139: goto _LL13B;_LL13A: if((_tmp269.C_n).tag != 3)goto
_LL13C;_tmp26C=(struct Cyc_List_List*)(_tmp269.C_n).val;if(_tmp26C != 0)goto _LL13C;
_LL13B: goto _LL13D;_LL13C: if((_tmp269.Abs_n).tag != 2)goto _LL13E;_tmp26D=(struct
Cyc_List_List*)(_tmp269.Abs_n).val;if(_tmp26D != 0)goto _LL13E;_LL13D: first=
_tmp257;rest=0;goto _LL135;_LL13E: if((_tmp269.Rel_n).tag != 1)goto _LL140;_tmp26E=(
struct Cyc_List_List*)(_tmp269.Rel_n).val;if(_tmp26E == 0)goto _LL140;_tmp26F=*
_tmp26E;_tmp270=(struct _dyneither_ptr*)_tmp26F.hd;_tmp271=_tmp26F.tl;_LL13F:
_tmp274=_tmp270;_tmp275=_tmp271;goto _LL141;_LL140: if((_tmp269.C_n).tag != 3)goto
_LL142;_tmp272=(struct Cyc_List_List*)(_tmp269.C_n).val;if(_tmp272 == 0)goto _LL142;
_tmp273=*_tmp272;_tmp274=(struct _dyneither_ptr*)_tmp273.hd;_tmp275=_tmp273.tl;
_LL141: _tmp278=_tmp274;_tmp279=_tmp275;goto _LL143;_LL142: if((_tmp269.Abs_n).tag
!= 2)goto _LL135;_tmp276=(struct Cyc_List_List*)(_tmp269.Abs_n).val;if(_tmp276 == 0)
goto _LL135;_tmp277=*_tmp276;_tmp278=(struct _dyneither_ptr*)_tmp277.hd;_tmp279=
_tmp277.tl;_LL143: first=_tmp278;{struct Cyc_List_List*_tmp400;rest=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp279,((_tmp400=
_cycalloc(sizeof(*_tmp400)),((_tmp400->hd=_tmp257,((_tmp400->tl=0,_tmp400)))))));}
goto _LL135;_LL135:;}{struct Cyc_List_List*_tmp27B=Cyc_Tcenv_resolve_namespace(te,
loc,first,rest);{struct Cyc_List_List*_tmp401;ge->availables=(struct Cyc_List_List*)((
_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401->hd=_tmp27B,((_tmp401->tl=ge->availables,
_tmp401))))));}Cyc_Tc_tc_decls(te,_tmp258,in_externC,in_externCinclude,
check_var_init,grgn,exports);ge->availables=((struct Cyc_List_List*)_check_null(
ge->availables))->tl;goto _LL114;};}_LL131: {struct Cyc_Absyn_ExternC_d_struct*
_tmp259=(struct Cyc_Absyn_ExternC_d_struct*)_tmp23D;if(_tmp259->tag != 12)goto
_LL133;else{_tmp25A=_tmp259->f1;}}_LL132: Cyc_Tc_tc_decls(te,_tmp25A,1,
in_externCinclude,check_var_init,grgn,exports);goto _LL114;_LL133: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp25B=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp23D;if(_tmp25B->tag != 13)
goto _LL114;else{_tmp25C=_tmp25B->f1;_tmp25D=_tmp25B->f2;}}_LL134:((void(*)(void(*
f)(struct Cyc_Tcenv_Tenv*,struct _tuple11*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp25D);{struct Cyc_List_List*
newexs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp25D,(unsigned int)exports?*exports: 0);Cyc_Tc_tc_decls(te,
_tmp25C,1,1,check_var_init,grgn,(struct Cyc_List_List**)& newexs);for(0;_tmp25D != 
0;_tmp25D=_tmp25D->tl){struct _tuple11*_tmp27D=(struct _tuple11*)_tmp25D->hd;if(!(*
_tmp27D).f3){const char*_tmp405;void*_tmp404[1];struct Cyc_String_pa_struct _tmp403;(
_tmp403.tag=0,((_tmp403.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((*
_tmp27D).f2)),((_tmp404[0]=& _tmp403,Cyc_Tcutil_warn((*_tmp27D).f1,((_tmp405="%s is exported but not defined",
_tag_dyneither(_tmp405,sizeof(char),31))),_tag_dyneither(_tmp404,sizeof(void*),1)))))));}}
goto _LL114;};_LL114:;}}void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*
te,int check_var_init,struct Cyc_List_List*ds){Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct
_RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};static int Cyc_Tc_vardecl_needed(
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);static struct Cyc_List_List*
Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){return((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,
env,ds);}struct _tuple17{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};static int
Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){void*
_tmp281=d->r;struct Cyc_Absyn_Vardecl*_tmp283;struct Cyc_List_List*_tmp285;struct
Cyc_List_List**_tmp286;struct Cyc_List_List*_tmp288;struct Cyc_List_List**_tmp289;
struct Cyc_List_List*_tmp28B;struct Cyc_List_List**_tmp28C;struct Cyc_List_List*
_tmp28E;struct Cyc_List_List**_tmp28F;_LL145: {struct Cyc_Absyn_Var_d_struct*
_tmp282=(struct Cyc_Absyn_Var_d_struct*)_tmp281;if(_tmp282->tag != 0)goto _LL147;
else{_tmp283=_tmp282->f1;}}_LL146: if(env->in_cinclude  || _tmp283->sc != Cyc_Absyn_Extern
 && _tmp283->sc != Cyc_Absyn_ExternC)return 1;{struct _tuple0 _tmp291;union Cyc_Absyn_Nmspace
_tmp292;struct _dyneither_ptr*_tmp293;struct _tuple0*_tmp290=_tmp283->name;_tmp291=*
_tmp290;_tmp292=_tmp291.f1;_tmp293=_tmp291.f2;{struct Cyc_List_List*ns;{union Cyc_Absyn_Nmspace
_tmp294=_tmp292;int _tmp295;struct Cyc_List_List*_tmp296;struct Cyc_List_List*
_tmp297;struct Cyc_List_List*_tmp298;_LL152: if((_tmp294.Loc_n).tag != 4)goto _LL154;
_tmp295=(int)(_tmp294.Loc_n).val;_LL153: ns=0;goto _LL151;_LL154: if((_tmp294.Rel_n).tag
!= 1)goto _LL156;_tmp296=(struct Cyc_List_List*)(_tmp294.Rel_n).val;_LL155: ns=
_tmp296;goto _LL151;_LL156: if((_tmp294.C_n).tag != 3)goto _LL158;_tmp297=(struct Cyc_List_List*)(
_tmp294.C_n).val;_LL157: ns=_tmp297;goto _LL151;_LL158: if((_tmp294.Abs_n).tag != 2)
goto _LL151;_tmp298=(struct Cyc_List_List*)(_tmp294.Abs_n).val;_LL159: ns=_tmp298;
goto _LL151;_LL151:;}{struct _tuple17*_tmp299=((struct _tuple17*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);struct Cyc_Tcenv_Genv*
_tmp29A=(*_tmp299).f1;int _tmp29B=(*((struct _tuple12*(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp29A->ordinaries,_tmp293)).f2;if(!
_tmp29B)(*_tmp299).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp299).f2,_tmp293);
return _tmp29B;};};};_LL147: {struct Cyc_Absyn_ExternC_d_struct*_tmp284=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp281;if(_tmp284->tag != 12)goto _LL149;else{_tmp285=_tmp284->f1;_tmp286=(struct
Cyc_List_List**)& _tmp284->f1;}}_LL148: _tmp289=_tmp286;goto _LL14A;_LL149: {struct
Cyc_Absyn_Using_d_struct*_tmp287=(struct Cyc_Absyn_Using_d_struct*)_tmp281;if(
_tmp287->tag != 11)goto _LL14B;else{_tmp288=_tmp287->f2;_tmp289=(struct Cyc_List_List**)&
_tmp287->f2;}}_LL14A: _tmp28C=_tmp289;goto _LL14C;_LL14B: {struct Cyc_Absyn_Namespace_d_struct*
_tmp28A=(struct Cyc_Absyn_Namespace_d_struct*)_tmp281;if(_tmp28A->tag != 10)goto
_LL14D;else{_tmp28B=_tmp28A->f2;_tmp28C=(struct Cyc_List_List**)& _tmp28A->f2;}}
_LL14C:*_tmp28C=Cyc_Tc_treeshake_f(env,*_tmp28C);return 1;_LL14D: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp28D=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp281;if(_tmp28D->tag != 13)
goto _LL14F;else{_tmp28E=_tmp28D->f1;_tmp28F=(struct Cyc_List_List**)& _tmp28D->f1;}}
_LL14E: {int in_cinclude=env->in_cinclude;env->in_cinclude=1;*_tmp28F=Cyc_Tc_treeshake_f(
env,*_tmp28F);env->in_cinclude=in_cinclude;return 1;}_LL14F:;_LL150: return 1;
_LL144:;}static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct
_dyneither_ptr*x,void*y){return !((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(set,x);}static struct _tuple17*Cyc_Tc_treeshake_make_env_f(
struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){struct _tuple17*_tmp406;return(
_tmp406=_region_malloc(rgn,sizeof(*_tmp406)),((_tmp406->f1=ge,((_tmp406->f2=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp406)))));}struct
_tuple18{struct Cyc_List_List*f1;struct _tuple17*f2;};struct Cyc_List_List*Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){struct _RegionHandle _tmp29D=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp29D;_push_region(rgn);{struct Cyc_Tc_TreeshakeEnv
_tmp407;struct Cyc_Tc_TreeshakeEnv _tmp29E=(_tmp407.rgn=rgn,((_tmp407.in_cinclude=
0,((_tmp407.nsdict=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple17*(*
f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict
d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp407)))));
struct Cyc_List_List*_tmp29F=Cyc_Tc_treeshake_f(& _tmp29E,ds);if(((int(*)(struct
Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp29E.nsdict)){struct Cyc_List_List*_tmp2A0=
_tmp29F;_npop_handler(0);return _tmp2A0;}{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,
_tmp29E.nsdict);struct _tuple18 _tmp2A1=*((struct _tuple18*(*)(struct _RegionHandle*,
struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp29E.nsdict);while(((int(*)(
struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& _tmp2A1)){struct
_tuple17*_tmp2A3;struct _tuple17 _tmp2A4;struct Cyc_Tcenv_Genv*_tmp2A5;struct Cyc_Set_Set*
_tmp2A6;struct _tuple18 _tmp2A2=_tmp2A1;_tmp2A3=_tmp2A2.f2;_tmp2A4=*_tmp2A3;
_tmp2A5=_tmp2A4.f1;_tmp2A6=_tmp2A4.f2;_tmp2A5->ordinaries=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct
_tuple12*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(
_tmp2A5->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct
_tuple12*y))Cyc_Tc_treeshake_remove_f,_tmp2A6,_tmp2A5->ordinaries);}{struct Cyc_List_List*
_tmp2A7=_tmp29F;_npop_handler(0);return _tmp2A7;};};};_pop_region(rgn);}
