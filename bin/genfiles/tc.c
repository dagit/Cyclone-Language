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
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
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
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;struct
Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};struct
Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
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
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;struct Cyc_Absyn_Tqual
ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
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
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern
struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{
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
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_const_string_typ(
void*rgn);void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_array_typ(void*elt_type,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;
int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;
int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;
int print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;struct _dyneither_ptr Cyc_Absynpp_typ2string(
void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct
_dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*
cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,
struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};int Cyc_Dict_is_empty(
struct Cyc_Dict_Dict d);int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict d);struct _tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(
struct _RegionHandle*,int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,
struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn,struct Cyc_Position_Segment*loc);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);
struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;
struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct
Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
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
void*t,struct Cyc_List_List*atts);void Cyc_Tcutil_check_no_qual(struct Cyc_Position_Segment*
loc,void*t);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple9*eq;int isTrue;};struct Cyc_Tcexp_TestEnv
Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr msg_part);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,int new_block);struct _tuple10{unsigned int f1;int f2;};struct _tuple10 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,int var_default_init,struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Datatypedecl*);void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Enumdecl*);
extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_struct{char*
tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;
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
_LL8:{const char*_tmp2AF;void*_tmp2AE[1];struct Cyc_String_pa_struct _tmp2AD;(
_tmp2AD.tag=0,((_tmp2AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
vd->name)),((_tmp2AE[0]=& _tmp2AD,Cyc_Tcutil_terr(loc,((_tmp2AF="qualified variable declarations are not implemented (%s)",
_tag_dyneither(_tmp2AF,sizeof(char),57))),_tag_dyneither(_tmp2AE,sizeof(void*),1)))))));}
return;_LL0:;}(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns,vd->sc == Cyc_Absyn_ExternC
 || in_cinclude);{void*_tmp13=Cyc_Tcutil_compress(_tmp9);struct Cyc_Absyn_ArrayInfo
_tmp15;void*_tmp16;struct Cyc_Absyn_Tqual _tmp17;struct Cyc_Absyn_Exp*_tmp18;union
Cyc_Absyn_Constraint*_tmp19;struct Cyc_Position_Segment*_tmp1A;_LLA:{struct Cyc_Absyn_ArrayType_struct*
_tmp14=(struct Cyc_Absyn_ArrayType_struct*)_tmp13;if(_tmp14->tag != 9)goto _LLC;
else{_tmp15=_tmp14->f1;_tmp16=_tmp15.elt_type;_tmp17=_tmp15.tq;_tmp18=_tmp15.num_elts;
if(_tmp18 != 0)goto _LLC;_tmp19=_tmp15.zero_term;_tmp1A=_tmp15.zt_loc;}}if(!(_tmpA
!= 0))goto _LLC;_LLB:{void*_tmp1B=_tmpA->r;union Cyc_Absyn_Cnst _tmp1D;struct
_dyneither_ptr _tmp1E;union Cyc_Absyn_Cnst _tmp20;struct _dyneither_ptr _tmp21;struct
Cyc_Absyn_Exp*_tmp23;struct Cyc_List_List*_tmp25;struct Cyc_List_List*_tmp27;_LLF: {
struct Cyc_Absyn_Const_e_struct*_tmp1C=(struct Cyc_Absyn_Const_e_struct*)_tmp1B;
if(_tmp1C->tag != 0)goto _LL11;else{_tmp1D=_tmp1C->f1;if((_tmp1D.String_c).tag != 8)
goto _LL11;_tmp1E=(struct _dyneither_ptr)(_tmp1D.String_c).val;}}_LL10: _tmp9=(vd->type=
Cyc_Absyn_array_typ(_tmp16,_tmp17,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
_get_dyneither_size(_tmp1E,sizeof(char)),0),_tmp19,_tmp1A));goto _LLE;_LL11: {
struct Cyc_Absyn_Const_e_struct*_tmp1F=(struct Cyc_Absyn_Const_e_struct*)_tmp1B;
if(_tmp1F->tag != 0)goto _LL13;else{_tmp20=_tmp1F->f1;if((_tmp20.Wstring_c).tag != 
9)goto _LL13;_tmp21=(struct _dyneither_ptr)(_tmp20.Wstring_c).val;}}_LL12: _tmp9=(
vd->type=Cyc_Absyn_array_typ(_tmp16,_tmp17,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
1,0),_tmp19,_tmp1A));goto _LLE;_LL13: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp22=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp1B;if(_tmp22->tag != 28)goto
_LL15;else{_tmp23=_tmp22->f2;}}_LL14: _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp16,
_tmp17,(struct Cyc_Absyn_Exp*)_tmp23,_tmp19,_tmp1A));goto _LLE;_LL15: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp24=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1B;if(_tmp24->tag != 36)goto
_LL17;else{_tmp25=_tmp24->f2;}}_LL16: _tmp27=_tmp25;goto _LL18;_LL17: {struct Cyc_Absyn_Array_e_struct*
_tmp26=(struct Cyc_Absyn_Array_e_struct*)_tmp1B;if(_tmp26->tag != 27)goto _LL19;
else{_tmp27=_tmp26->f1;}}_LL18: _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp16,_tmp17,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp27),0),_tmp19,_tmp1A));goto _LLE;_LL19:;_LL1A: goto _LLE;
_LLE:;}goto _LL9;_LLC:;_LLD: goto _LL9;_LL9:;}Cyc_Tcutil_check_valid_toplevel_type(
loc,te,_tmp9);(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,
_tmp9);{void*_tmp28=Cyc_Tcutil_compress(_tmp9);_LL1C: {struct Cyc_Absyn_ArrayType_struct*
_tmp29=(struct Cyc_Absyn_ArrayType_struct*)_tmp28;if(_tmp29->tag != 9)goto _LL1E;}
_LL1D: vd->escapes=0;goto _LL1B;_LL1E:;_LL1F: vd->escapes=1;goto _LL1B;_LL1B:;}if(
Cyc_Tcutil_is_function_type(_tmp9))_tmpB=Cyc_Tcutil_transfer_fn_type_atts(_tmp9,
_tmpB);if(_tmp4 == Cyc_Absyn_Extern  || _tmp4 == Cyc_Absyn_ExternC){if(_tmpA != 0){
const char*_tmp2B2;void*_tmp2B1;(_tmp2B1=0,Cyc_Tcutil_terr(loc,((_tmp2B2="extern declaration should not have initializer",
_tag_dyneither(_tmp2B2,sizeof(char),47))),_tag_dyneither(_tmp2B1,sizeof(void*),0)));}}
else{if(!Cyc_Tcutil_is_function_type(_tmp9)){for(0;_tmpB != 0;_tmpB=_tmpB->tl){
void*_tmp2C=(void*)_tmpB->hd;_LL21: {struct Cyc_Absyn_Aligned_att_struct*_tmp2D=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp2C;if(_tmp2D->tag != 6)goto _LL23;}_LL22:
goto _LL24;_LL23: {struct Cyc_Absyn_Section_att_struct*_tmp2E=(struct Cyc_Absyn_Section_att_struct*)
_tmp2C;if(_tmp2E->tag != 8)goto _LL25;}_LL24: goto _LL26;_LL25: {struct Cyc_Absyn_Nocommon_att_struct*
_tmp2F=(struct Cyc_Absyn_Nocommon_att_struct*)_tmp2C;if(_tmp2F->tag != 9)goto _LL27;}
_LL26: goto _LL28;_LL27: {struct Cyc_Absyn_Shared_att_struct*_tmp30=(struct Cyc_Absyn_Shared_att_struct*)
_tmp2C;if(_tmp30->tag != 10)goto _LL29;}_LL28: goto _LL2A;_LL29: {struct Cyc_Absyn_Unused_att_struct*
_tmp31=(struct Cyc_Absyn_Unused_att_struct*)_tmp2C;if(_tmp31->tag != 11)goto _LL2B;}
_LL2A: goto _LL2C;_LL2B: {struct Cyc_Absyn_Weak_att_struct*_tmp32=(struct Cyc_Absyn_Weak_att_struct*)
_tmp2C;if(_tmp32->tag != 12)goto _LL2D;}_LL2C: goto _LL2E;_LL2D: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp33=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp2C;if(_tmp33->tag != 13)goto
_LL2F;}_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_Dllexport_att_struct*_tmp34=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp2C;if(_tmp34->tag != 14)goto _LL31;}
_LL30: continue;_LL31:;_LL32:{const char*_tmp2B7;void*_tmp2B6[2];struct Cyc_String_pa_struct
_tmp2B5;struct Cyc_String_pa_struct _tmp2B4;(_tmp2B4.tag=0,((_tmp2B4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((
_tmp2B5.tag=0,((_tmp2B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB->hd)),((_tmp2B6[0]=& _tmp2B5,((_tmp2B6[1]=& _tmp2B4,Cyc_Tcutil_terr(
loc,((_tmp2B7="bad attribute %s for variable %s",_tag_dyneither(_tmp2B7,sizeof(
char),33))),_tag_dyneither(_tmp2B6,sizeof(void*),2)))))))))))));}goto _LL20;_LL20:;}
if(_tmpA == 0  || in_cinclude){if(check_var_init  && !Cyc_Tcutil_supports_default(
_tmp9)){const char*_tmp2BC;void*_tmp2BB[2];struct Cyc_String_pa_struct _tmp2BA;
struct Cyc_String_pa_struct _tmp2B9;(_tmp2B9.tag=0,((_tmp2B9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9)),((_tmp2BA.tag=
0,((_tmp2BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
vd->name)),((_tmp2BB[0]=& _tmp2BA,((_tmp2BB[1]=& _tmp2B9,Cyc_Tcutil_terr(loc,((
_tmp2BC="initializer required for variable %s of type %s",_tag_dyneither(_tmp2BC,
sizeof(char),48))),_tag_dyneither(_tmp2BB,sizeof(void*),2)))))))))))));}}else{
struct Cyc_Absyn_Exp*_tmp3D=(struct Cyc_Absyn_Exp*)_tmpA;void*_tmp3E=Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp9,_tmp3D);if(!Cyc_Tcutil_coerce_assign(te,_tmp3D,_tmp9)){{const
char*_tmp2C2;void*_tmp2C1[3];struct Cyc_String_pa_struct _tmp2C0;struct Cyc_String_pa_struct
_tmp2BF;struct Cyc_String_pa_struct _tmp2BE;(_tmp2BE.tag=0,((_tmp2BE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3E)),((_tmp2BF.tag=
0,((_tmp2BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp9)),((_tmp2C0.tag=0,((_tmp2C0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp2C1[0]=& _tmp2C0,((
_tmp2C1[1]=& _tmp2BF,((_tmp2C1[2]=& _tmp2BE,Cyc_Tcutil_terr(loc,((_tmp2C2="%s declared with type \n%s\n but initialized with type \n%s",
_tag_dyneither(_tmp2C2,sizeof(char),57))),_tag_dyneither(_tmp2C1,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}if(!Cyc_Tcutil_is_const_exp(te,_tmp3D)){const char*
_tmp2C5;void*_tmp2C4;(_tmp2C4=0,Cyc_Tcutil_terr(loc,((_tmp2C5="initializer is not a constant expression",
_tag_dyneither(_tmp2C5,sizeof(char),41))),_tag_dyneither(_tmp2C4,sizeof(void*),0)));}}}
else{for(0;_tmpB != 0;_tmpB=_tmpB->tl){void*_tmp46=(void*)_tmpB->hd;_LL34: {
struct Cyc_Absyn_Regparm_att_struct*_tmp47=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp46;if(_tmp47->tag != 0)goto _LL36;}_LL35: goto _LL37;_LL36: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp48=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp46;if(_tmp48->tag != 1)goto _LL38;}
_LL37: goto _LL39;_LL38: {struct Cyc_Absyn_Cdecl_att_struct*_tmp49=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp46;if(_tmp49->tag != 2)goto _LL3A;}_LL39: goto _LL3B;_LL3A: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp4A=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp46;if(_tmp4A->tag != 3)goto _LL3C;}
_LL3B: goto _LL3D;_LL3C: {struct Cyc_Absyn_Noreturn_att_struct*_tmp4B=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp46;if(_tmp4B->tag != 4)goto _LL3E;}_LL3D: goto _LL3F;_LL3E: {struct Cyc_Absyn_Format_att_struct*
_tmp4C=(struct Cyc_Absyn_Format_att_struct*)_tmp46;if(_tmp4C->tag != 19)goto _LL40;}
_LL3F: goto _LL41;_LL40: {struct Cyc_Absyn_Initializes_att_struct*_tmp4D=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp46;if(_tmp4D->tag != 20)goto _LL42;}_LL41: goto _LL43;_LL42: {struct Cyc_Absyn_Pure_att_struct*
_tmp4E=(struct Cyc_Absyn_Pure_att_struct*)_tmp46;if(_tmp4E->tag != 22)goto _LL44;}
_LL43: goto _LL45;_LL44: {struct Cyc_Absyn_Const_att_struct*_tmp4F=(struct Cyc_Absyn_Const_att_struct*)
_tmp46;if(_tmp4F->tag != 5)goto _LL46;}_LL45: {const char*_tmp2C8;void*_tmp2C7;(
_tmp2C7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp2C8="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp2C8,
sizeof(char),45))),_tag_dyneither(_tmp2C7,sizeof(void*),0)));}_LL46: {struct Cyc_Absyn_Aligned_att_struct*
_tmp50=(struct Cyc_Absyn_Aligned_att_struct*)_tmp46;if(_tmp50->tag != 6)goto _LL48;}
_LL47: goto _LL49;_LL48: {struct Cyc_Absyn_Packed_att_struct*_tmp51=(struct Cyc_Absyn_Packed_att_struct*)
_tmp46;if(_tmp51->tag != 7)goto _LL4A;}_LL49:{const char*_tmp2CC;void*_tmp2CB[1];
struct Cyc_String_pa_struct _tmp2CA;(_tmp2CA.tag=0,((_tmp2CA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpB->hd)),((
_tmp2CB[0]=& _tmp2CA,Cyc_Tcutil_terr(loc,((_tmp2CC="bad attribute %s in function declaration",
_tag_dyneither(_tmp2CC,sizeof(char),41))),_tag_dyneither(_tmp2CB,sizeof(void*),1)))))));}
goto _LL33;_LL4A:;_LL4B: continue;_LL33:;}}}{struct _handler_cons _tmp57;
_push_handler(& _tmp57);{int _tmp59=0;if(setjmp(_tmp57.handler))_tmp59=1;if(!
_tmp59){{struct _tuple12*_tmp5A=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp8);void*_tmp5B=(*_tmp5A).f1;
void*_tmp5D;_LL4D: {struct Cyc_Tcenv_VarRes_struct*_tmp5C=(struct Cyc_Tcenv_VarRes_struct*)
_tmp5B;if(_tmp5C->tag != 0)goto _LL4F;else{_tmp5D=(void*)_tmp5C->f1;}}_LL4E: {
struct Cyc_Absyn_Global_b_struct _tmp2CF;struct Cyc_Absyn_Global_b_struct*_tmp2CE;
struct Cyc_Absyn_Global_b_struct*_tmp62=(_tmp2CE=_cycalloc(sizeof(*_tmp2CE)),((
_tmp2CE[0]=((_tmp2CF.tag=1,((_tmp2CF.f1=vd,_tmp2CF)))),_tmp2CE)));void*_tmp63=
Cyc_Tcdecl_merge_binding(_tmp5D,(void*)_tmp62,loc,Cyc_Tc_tc_msg);if(_tmp63 == (
void*)& Cyc_Absyn_Unresolved_b_val){_npop_handler(0);return;}if(exports == 0  || 
Cyc_Tc_export_member(vd->name,*exports)){if(!(_tmp63 == _tmp5D  && (*_tmp5A).f2)){
struct Cyc_Tcenv_VarRes_struct*_tmp2D5;struct Cyc_Tcenv_VarRes_struct _tmp2D4;
struct _tuple12*_tmp2D3;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(
struct _tuple12*)((_tmp2D3=_cycalloc(sizeof(*_tmp2D3)),((_tmp2D3->f1=(void*)((
_tmp2D5=_cycalloc(sizeof(*_tmp2D5)),((_tmp2D5[0]=((_tmp2D4.tag=0,((_tmp2D4.f1=(
void*)_tmp63,_tmp2D4)))),_tmp2D5)))),((_tmp2D3->f2=1,_tmp2D3)))))));}}
_npop_handler(0);return;}_LL4F: {struct Cyc_Tcenv_AggrRes_struct*_tmp5E=(struct
Cyc_Tcenv_AggrRes_struct*)_tmp5B;if(_tmp5E->tag != 1)goto _LL51;}_LL50:{const char*
_tmp2D8;void*_tmp2D7;(_tmp2D7=0,Cyc_Tcutil_warn(loc,((_tmp2D8="variable declaration shadows previous struct declaration",
_tag_dyneither(_tmp2D8,sizeof(char),57))),_tag_dyneither(_tmp2D7,sizeof(void*),0)));}
goto _LL4C;_LL51: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp5F=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp5B;if(_tmp5F->tag != 2)goto _LL53;}_LL52:{const char*_tmp2DB;void*_tmp2DA;(
_tmp2DA=0,Cyc_Tcutil_warn(loc,((_tmp2DB="variable declaration shadows previous datatype constructor",
_tag_dyneither(_tmp2DB,sizeof(char),59))),_tag_dyneither(_tmp2DA,sizeof(void*),0)));}
goto _LL4C;_LL53: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp60=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp5B;if(_tmp60->tag != 4)goto _LL55;}_LL54: goto _LL56;_LL55: {struct Cyc_Tcenv_EnumRes_struct*
_tmp61=(struct Cyc_Tcenv_EnumRes_struct*)_tmp5B;if(_tmp61->tag != 3)goto _LL4C;}
_LL56:{const char*_tmp2DE;void*_tmp2DD;(_tmp2DD=0,Cyc_Tcutil_warn(loc,((_tmp2DE="variable declaration shadows previous enum tag",
_tag_dyneither(_tmp2DE,sizeof(char),47))),_tag_dyneither(_tmp2DD,sizeof(void*),0)));}
goto _LL4C;_LL4C:;};_pop_handler();}else{void*_tmp58=(void*)_exn_thrown;void*
_tmp70=_tmp58;_LL58: {struct Cyc_Dict_Absent_struct*_tmp71=(struct Cyc_Dict_Absent_struct*)
_tmp70;if(_tmp71->tag != Cyc_Dict_Absent)goto _LL5A;}_LL59: goto _LL57;_LL5A:;_LL5B:(
void)_throw(_tmp70);_LL57:;}};}if(exports == 0  || Cyc_Tc_export_member(vd->name,*
exports)){struct Cyc_Tcenv_VarRes_struct*_tmp2ED;struct Cyc_Absyn_Global_b_struct*
_tmp2EC;struct Cyc_Absyn_Global_b_struct _tmp2EB;struct Cyc_Tcenv_VarRes_struct
_tmp2EA;struct _tuple12*_tmp2E9;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(
struct _tuple12*)((_tmp2E9=_cycalloc(sizeof(*_tmp2E9)),((_tmp2E9->f1=(void*)((
_tmp2ED=_cycalloc(sizeof(*_tmp2ED)),((_tmp2ED[0]=((_tmp2EA.tag=0,((_tmp2EA.f1=(
void*)((void*)((_tmp2EC=_cycalloc(sizeof(*_tmp2EC)),((_tmp2EC[0]=((_tmp2EB.tag=1,((
_tmp2EB.f1=vd,_tmp2EB)))),_tmp2EC))))),_tmp2EA)))),_tmp2ED)))),((_tmp2E9->f2=0,
_tmp2E9)))))));}}static int Cyc_Tc_is_main(struct _tuple0*n){struct _tuple0 _tmp78;
union Cyc_Absyn_Nmspace _tmp79;struct _dyneither_ptr*_tmp7A;struct _tuple0*_tmp77=n;
_tmp78=*_tmp77;_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;{union Cyc_Absyn_Nmspace _tmp7B=
_tmp79;struct Cyc_List_List*_tmp7C;_LL5D: if((_tmp7B.Abs_n).tag != 2)goto _LL5F;
_tmp7C=(struct Cyc_List_List*)(_tmp7B.Abs_n).val;if(_tmp7C != 0)goto _LL5F;_LL5E: {
const char*_tmp2EE;return Cyc_strcmp((struct _dyneither_ptr)*_tmp7A,((_tmp2EE="main",
_tag_dyneither(_tmp2EE,sizeof(char),5))))== 0;}_LL5F:;_LL60: return 0;_LL5C:;};}
struct _tuple13{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Fndecl*fd,int in_cinclude,struct Cyc_List_List**exports){
struct _dyneither_ptr*v=(*fd->name).f2;if(fd->sc == Cyc_Absyn_ExternC  && !
in_cinclude){const char*_tmp2F1;void*_tmp2F0;(_tmp2F0=0,Cyc_Tcutil_terr(loc,((
_tmp2F1="extern \"C\" functions cannot be implemented in Cyclone",_tag_dyneither(
_tmp2F1,sizeof(char),54))),_tag_dyneither(_tmp2F0,sizeof(void*),0)));}{union Cyc_Absyn_Nmspace
_tmp80=(*fd->name).f1;struct Cyc_List_List*_tmp81;struct Cyc_List_List*_tmp82;
struct Cyc_List_List*_tmp83;_LL62: if((_tmp80.Rel_n).tag != 1)goto _LL64;_tmp81=(
struct Cyc_List_List*)(_tmp80.Rel_n).val;if(_tmp81 != 0)goto _LL64;_LL63: goto _LL61;
_LL64: if((_tmp80.C_n).tag != 3)goto _LL66;_tmp82=(struct Cyc_List_List*)(_tmp80.C_n).val;
if(_tmp82 != 0)goto _LL66;_LL65: {const char*_tmp2F4;void*_tmp2F3;(_tmp2F3=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2F4="tc: C_n in tcFndecl",
_tag_dyneither(_tmp2F4,sizeof(char),20))),_tag_dyneither(_tmp2F3,sizeof(void*),0)));}
_LL66: if((_tmp80.Abs_n).tag != 2)goto _LL68;_tmp83=(struct Cyc_List_List*)(_tmp80.Abs_n).val;
_LL67: {const char*_tmp2F7;void*_tmp2F6;(_tmp2F6=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2F7="tc: Abs_n in tcFndecl",
_tag_dyneither(_tmp2F7,sizeof(char),22))),_tag_dyneither(_tmp2F6,sizeof(void*),0)));}
_LL68:;_LL69:{const char*_tmp2FB;void*_tmp2FA[1];struct Cyc_String_pa_struct
_tmp2F9;(_tmp2F9.tag=0,((_tmp2F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(fd->name)),((_tmp2FA[0]=& _tmp2F9,Cyc_Tcutil_terr(loc,((
_tmp2FB="qualified function declarations are not implemented (%s)",
_tag_dyneither(_tmp2FB,sizeof(char),57))),_tag_dyneither(_tmp2FA,sizeof(void*),1)))))));}
return;_LL61:;}(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns,fd->sc == Cyc_Absyn_ExternC
 || in_cinclude);Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{void*t=Cyc_Tcutil_fndecl2typ(
fd);fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);{struct Cyc_List_List*
atts=fd->attributes;for(0;atts != 0;atts=atts->tl){void*_tmp8B=(void*)atts->hd;
_LL6B: {struct Cyc_Absyn_Packed_att_struct*_tmp8C=(struct Cyc_Absyn_Packed_att_struct*)
_tmp8B;if(_tmp8C->tag != 7)goto _LL6D;}_LL6C: goto _LL6E;_LL6D: {struct Cyc_Absyn_Aligned_att_struct*
_tmp8D=(struct Cyc_Absyn_Aligned_att_struct*)_tmp8B;if(_tmp8D->tag != 6)goto _LL6F;}
_LL6E:{const char*_tmp2FF;void*_tmp2FE[1];struct Cyc_String_pa_struct _tmp2FD;(
_tmp2FD.tag=0,((_tmp2FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)atts->hd)),((_tmp2FE[0]=& _tmp2FD,Cyc_Tcutil_terr(loc,((_tmp2FF="bad attribute %s for function",
_tag_dyneither(_tmp2FF,sizeof(char),30))),_tag_dyneither(_tmp2FE,sizeof(void*),1)))))));}
goto _LL6A;_LL6F:;_LL70: goto _LL6A;_LL6A:;}}{struct _handler_cons _tmp91;
_push_handler(& _tmp91);{int _tmp93=0;if(setjmp(_tmp91.handler))_tmp93=1;if(!
_tmp93){{struct _tuple12*_tmp94=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);void*_tmp95=(*_tmp94).f1;
void*_tmp97;_LL72: {struct Cyc_Tcenv_VarRes_struct*_tmp96=(struct Cyc_Tcenv_VarRes_struct*)
_tmp95;if(_tmp96->tag != 0)goto _LL74;else{_tmp97=(void*)_tmp96->f1;}}_LL73: {
struct Cyc_Absyn_Funname_b_struct _tmp302;struct Cyc_Absyn_Funname_b_struct*_tmp301;
struct Cyc_Absyn_Funname_b_struct*_tmp9C=(_tmp301=_cycalloc(sizeof(*_tmp301)),((
_tmp301[0]=((_tmp302.tag=2,((_tmp302.f1=fd,_tmp302)))),_tmp301)));void*_tmp9D=
Cyc_Tcdecl_merge_binding(_tmp97,(void*)_tmp9C,loc,Cyc_Tc_tc_msg);if(_tmp9D == (
void*)& Cyc_Absyn_Unresolved_b_val)goto _LL71;if(exports == 0  || Cyc_Tc_export_member(
fd->name,*exports)){if(!(_tmp9D == _tmp97  && (*_tmp94).f2)){struct Cyc_Tcenv_VarRes_struct*
_tmp308;struct Cyc_Tcenv_VarRes_struct _tmp307;struct _tuple12*_tmp306;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple12*v))Cyc_Dict_insert)(ge->ordinaries,v,(struct _tuple12*)((_tmp306=
_cycalloc(sizeof(*_tmp306)),((_tmp306->f1=(void*)((_tmp308=_cycalloc(sizeof(*
_tmp308)),((_tmp308[0]=((_tmp307.tag=0,((_tmp307.f1=(void*)_tmp9D,_tmp307)))),
_tmp308)))),((_tmp306->f2=1,_tmp306)))))));}}goto _LL71;}_LL74: {struct Cyc_Tcenv_AggrRes_struct*
_tmp98=(struct Cyc_Tcenv_AggrRes_struct*)_tmp95;if(_tmp98->tag != 1)goto _LL76;}
_LL75:{const char*_tmp30B;void*_tmp30A;(_tmp30A=0,Cyc_Tcutil_warn(loc,((_tmp30B="function declaration shadows previous type declaration",
_tag_dyneither(_tmp30B,sizeof(char),55))),_tag_dyneither(_tmp30A,sizeof(void*),0)));}
goto _LL71;_LL76: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp99=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp95;if(_tmp99->tag != 2)goto _LL78;}_LL77:{const char*_tmp30E;void*_tmp30D;(
_tmp30D=0,Cyc_Tcutil_warn(loc,((_tmp30E="function declaration shadows previous datatype constructor",
_tag_dyneither(_tmp30E,sizeof(char),59))),_tag_dyneither(_tmp30D,sizeof(void*),0)));}
goto _LL71;_LL78: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp9A=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp95;if(_tmp9A->tag != 4)goto _LL7A;}_LL79: goto _LL7B;_LL7A: {struct Cyc_Tcenv_EnumRes_struct*
_tmp9B=(struct Cyc_Tcenv_EnumRes_struct*)_tmp95;if(_tmp9B->tag != 3)goto _LL71;}
_LL7B:{const char*_tmp311;void*_tmp310;(_tmp310=0,Cyc_Tcutil_warn(loc,((_tmp311="function declaration shadows previous enum tag",
_tag_dyneither(_tmp311,sizeof(char),47))),_tag_dyneither(_tmp310,sizeof(void*),0)));}
goto _LL71;_LL71:;};_pop_handler();}else{void*_tmp92=(void*)_exn_thrown;void*
_tmpAA=_tmp92;_LL7D: {struct Cyc_Dict_Absent_struct*_tmpAB=(struct Cyc_Dict_Absent_struct*)
_tmpAA;if(_tmpAB->tag != Cyc_Dict_Absent)goto _LL7F;}_LL7E: if(exports == 0  || Cyc_Tc_export_member(
fd->name,*exports)){struct Cyc_Tcenv_VarRes_struct*_tmp320;struct Cyc_Absyn_Funname_b_struct*
_tmp31F;struct Cyc_Absyn_Funname_b_struct _tmp31E;struct Cyc_Tcenv_VarRes_struct
_tmp31D;struct _tuple12*_tmp31C;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,v,(
struct _tuple12*)((_tmp31C=_cycalloc(sizeof(*_tmp31C)),((_tmp31C->f1=(void*)((
_tmp320=_cycalloc(sizeof(*_tmp320)),((_tmp320[0]=((_tmp31D.tag=0,((_tmp31D.f1=(
void*)((void*)((_tmp31F=_cycalloc(sizeof(*_tmp31F)),((_tmp31F[0]=((_tmp31E.tag=2,((
_tmp31E.f1=fd,_tmp31E)))),_tmp31F))))),_tmp31D)))),_tmp320)))),((_tmp31C->f2=0,
_tmp31C)))))));}goto _LL7C;_LL7F:;_LL80:(void)_throw(_tmpAA);_LL7C:;}};}if(
in_cinclude)return;{struct _RegionHandle _tmpB1=_new_region("fnrgn");struct
_RegionHandle*fnrgn=& _tmpB1;_push_region(fnrgn);{struct Cyc_Tcenv_Fenv*_tmpB2=Cyc_Tcenv_new_fenv(
fnrgn,loc,fd);struct Cyc_Tcenv_Tenv*_tmp321;struct Cyc_Tcenv_Tenv*_tmpB3=(_tmp321=
_region_malloc(fnrgn,sizeof(*_tmp321)),((_tmp321->ns=te->ns,((_tmp321->ae=te->ae,((
_tmp321->le=(struct Cyc_Tcenv_Fenv*)_tmpB2,((_tmp321->allow_valueof=0,_tmp321)))))))));
Cyc_Tcstmt_tcStmt(_tmpB3,fd->body,0);Cyc_Tcenv_check_delayed_effects(_tmpB3);Cyc_Tcenv_check_delayed_constraints(
_tmpB3);if(!Cyc_Tcenv_all_labels_resolved(_tmpB3)){const char*_tmp324;void*
_tmp323;(_tmp323=0,Cyc_Tcutil_terr(loc,((_tmp324="function has goto statements to undefined labels",
_tag_dyneither(_tmp324,sizeof(char),49))),_tag_dyneither(_tmp323,sizeof(void*),0)));}}
if(Cyc_Tc_is_main(fd->name)){{void*_tmpB7=Cyc_Tcutil_compress(fd->ret_type);
enum Cyc_Absyn_Size_of _tmpBA;_LL82: {struct Cyc_Absyn_VoidType_struct*_tmpB8=(
struct Cyc_Absyn_VoidType_struct*)_tmpB7;if(_tmpB8->tag != 0)goto _LL84;}_LL83:{
const char*_tmp327;void*_tmp326;(_tmp326=0,Cyc_Tcutil_warn(loc,((_tmp327="main declared with return type void",
_tag_dyneither(_tmp327,sizeof(char),36))),_tag_dyneither(_tmp326,sizeof(void*),0)));}
goto _LL81;_LL84: {struct Cyc_Absyn_IntType_struct*_tmpB9=(struct Cyc_Absyn_IntType_struct*)
_tmpB7;if(_tmpB9->tag != 6)goto _LL86;else{_tmpBA=_tmpB9->f2;}}_LL85: goto _LL81;
_LL86:;_LL87:{const char*_tmp32B;void*_tmp32A[1];struct Cyc_String_pa_struct
_tmp329;(_tmp329.tag=0,((_tmp329.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type)),((_tmp32A[0]=& _tmp329,Cyc_Tcutil_terr(loc,((
_tmp32B="main declared with return type %s instead of int or void",
_tag_dyneither(_tmp32B,sizeof(char),57))),_tag_dyneither(_tmp32A,sizeof(void*),1)))))));}
goto _LL81;_LL81:;}if(fd->c_varargs  || fd->cyc_varargs != 0){const char*_tmp32E;
void*_tmp32D;(_tmp32D=0,Cyc_Tcutil_terr(loc,((_tmp32E="main declared with varargs",
_tag_dyneither(_tmp32E,sizeof(char),27))),_tag_dyneither(_tmp32D,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC2=fd->args;if(_tmpC2 != 0){struct _tuple13 _tmpC4;void*
_tmpC5;struct _tuple13*_tmpC3=(struct _tuple13*)_tmpC2->hd;_tmpC4=*_tmpC3;_tmpC5=
_tmpC4.f3;{void*_tmpC6=Cyc_Tcutil_compress(_tmpC5);enum Cyc_Absyn_Size_of _tmpC8;
_LL89: {struct Cyc_Absyn_IntType_struct*_tmpC7=(struct Cyc_Absyn_IntType_struct*)
_tmpC6;if(_tmpC7->tag != 6)goto _LL8B;else{_tmpC8=_tmpC7->f2;}}_LL8A: goto _LL88;
_LL8B:;_LL8C:{const char*_tmp332;void*_tmp331[1];struct Cyc_String_pa_struct
_tmp330;(_tmp330.tag=0,((_tmp330.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpC5)),((_tmp331[0]=& _tmp330,Cyc_Tcutil_terr(loc,((
_tmp332="main declared with first argument of type %s instead of int",
_tag_dyneither(_tmp332,sizeof(char),60))),_tag_dyneither(_tmp331,sizeof(void*),1)))))));}
goto _LL88;_LL88:;}_tmpC2=_tmpC2->tl;if(_tmpC2 != 0){struct _tuple13 _tmpCD;void*
_tmpCE;struct _tuple13*_tmpCC=(struct _tuple13*)_tmpC2->hd;_tmpCD=*_tmpCC;_tmpCE=
_tmpCD.f3;_tmpC2=_tmpC2->tl;if(_tmpC2 != 0){const char*_tmp335;void*_tmp334;(
_tmp334=0,Cyc_Tcutil_terr(loc,((_tmp335="main declared with too many arguments",
_tag_dyneither(_tmp335,sizeof(char),38))),_tag_dyneither(_tmp334,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp336;struct Cyc_Core_Opt*tvs=(_tmp336=_cycalloc(sizeof(*
_tmp336)),((_tmp336->v=fd->tvs,_tmp336)));struct Cyc_Core_Opt*_tmp33E;struct Cyc_Core_Opt*
_tmp33D;struct Cyc_Core_Opt*_tmp33C;struct Cyc_Core_Opt*_tmp33B;struct Cyc_Core_Opt*
_tmp33A;struct Cyc_Core_Opt*_tmp339;struct Cyc_Core_Opt*_tmp338;struct Cyc_Core_Opt*
_tmp337;if(((!Cyc_Tcutil_unify(_tmpCE,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(
Cyc_Absyn_new_evar(((_tmp33D=_cycalloc(sizeof(*_tmp33D)),((_tmp33D->v=& Cyc_Tcutil_rk,
_tmp33D)))),tvs)),Cyc_Absyn_new_evar(((_tmp33E=_cycalloc(sizeof(*_tmp33E)),((
_tmp33E->v=& Cyc_Tcutil_rk,_tmp33E)))),tvs),Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)())) && !Cyc_Tcutil_unify(_tmpCE,Cyc_Absyn_dyneither_typ(
Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp33B=_cycalloc(sizeof(*
_tmp33B)),((_tmp33B->v=& Cyc_Tcutil_rk,_tmp33B)))),tvs)),Cyc_Absyn_new_evar(((
_tmp33C=_cycalloc(sizeof(*_tmp33C)),((_tmp33C->v=& Cyc_Tcutil_rk,_tmp33C)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
 && !Cyc_Tcutil_unify(_tmpCE,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((
_tmp339=_cycalloc(sizeof(*_tmp339)),((_tmp339->v=& Cyc_Tcutil_rk,_tmp339)))),tvs)),
Cyc_Absyn_new_evar(((_tmp33A=_cycalloc(sizeof(*_tmp33A)),((_tmp33A->v=& Cyc_Tcutil_rk,
_tmp33A)))),tvs),Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
 && !Cyc_Tcutil_unify(_tmpCE,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(
Cyc_Absyn_new_evar(((_tmp337=_cycalloc(sizeof(*_tmp337)),((_tmp337->v=& Cyc_Tcutil_rk,
_tmp337)))),tvs)),Cyc_Absyn_new_evar(((_tmp338=_cycalloc(sizeof(*_tmp338)),((
_tmp338->v=& Cyc_Tcutil_rk,_tmp338)))),tvs),Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)()))){const char*_tmp342;void*_tmp341[1];struct Cyc_String_pa_struct
_tmp340;(_tmp340.tag=0,((_tmp340.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpCE)),((_tmp341[0]=& _tmp340,Cyc_Tcutil_terr(loc,((
_tmp342="second argument of main has type %s instead of char??",_tag_dyneither(
_tmp342,sizeof(char),54))),_tag_dyneither(_tmp341,sizeof(void*),1)))))));}};}}};};
_pop_region(fnrgn);};};}static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*
td){struct _dyneither_ptr*v=(*td->name).f2;{union Cyc_Absyn_Nmspace _tmpDD=(*td->name).f1;
struct Cyc_List_List*_tmpDE;struct Cyc_List_List*_tmpDF;struct Cyc_List_List*_tmpE0;
_LL8E: if((_tmpDD.Rel_n).tag != 1)goto _LL90;_tmpDE=(struct Cyc_List_List*)(_tmpDD.Rel_n).val;
if(_tmpDE != 0)goto _LL90;_LL8F: goto _LL91;_LL90: if((_tmpDD.C_n).tag != 3)goto _LL92;
_tmpDF=(struct Cyc_List_List*)(_tmpDD.C_n).val;if(_tmpDF != 0)goto _LL92;_LL91: goto
_LL93;_LL92: if((_tmpDD.Abs_n).tag != 2)goto _LL94;_tmpE0=(struct Cyc_List_List*)(
_tmpDD.Abs_n).val;if(_tmpE0 != 0)goto _LL94;_LL93: goto _LL8D;_LL94:;_LL95:{const
char*_tmp346;void*_tmp345[1];struct Cyc_String_pa_struct _tmp344;(_tmp344.tag=0,((
_tmp344.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
td->name)),((_tmp345[0]=& _tmp344,Cyc_Tcutil_terr(loc,((_tmp346="qualified typedef declarations are not implemented (%s)",
_tag_dyneither(_tmp346,sizeof(char),56))),_tag_dyneither(_tmp345,sizeof(void*),1)))))));}
return;_LL8D:;}if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(
ge->typedefs,v)){{const char*_tmp34A;void*_tmp349[1];struct Cyc_String_pa_struct
_tmp348;(_tmp348.tag=0,((_tmp348.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
v),((_tmp349[0]=& _tmp348,Cyc_Tcutil_terr(loc,((_tmp34A="redeclaration of typedef %s",
_tag_dyneither(_tmp34A,sizeof(char),28))),_tag_dyneither(_tmp349,sizeof(void*),1)))))));}
return;}(*td->name).f1=Cyc_Absyn_Abs_n(te->ns,0);Cyc_Tcutil_check_unique_tvars(
loc,td->tvs);Cyc_Tcutil_add_tvar_identities(td->tvs);if(td->defn != 0){Cyc_Tcutil_check_type(
loc,te,td->tvs,& Cyc_Tcutil_tak,0,(void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);(
td->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(
void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);}{struct Cyc_List_List*tvs=
td->tvs;for(0;tvs != 0;tvs=tvs->tl){void*_tmpE7=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmpE9;struct Cyc_Core_Opt**_tmpEA;struct Cyc_Core_Opt*
_tmpEC;struct Cyc_Core_Opt**_tmpED;struct Cyc_Absyn_Kind*_tmpEE;_LL97: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpE8=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpE7;if(_tmpE8->tag != 1)goto _LL99;
else{_tmpE9=_tmpE8->f1;_tmpEA=(struct Cyc_Core_Opt**)& _tmpE8->f1;}}_LL98: if(td->defn
!= 0){const char*_tmp34E;void*_tmp34D[1];struct Cyc_String_pa_struct _tmp34C;(
_tmp34C.tag=0,((_tmp34C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp34D[0]=& _tmp34C,Cyc_Tcutil_warn(loc,((
_tmp34E="type variable %s is not used in typedef definition",_tag_dyneither(
_tmp34E,sizeof(char),51))),_tag_dyneither(_tmp34D,sizeof(void*),1)))))));}{
struct Cyc_Core_Opt*_tmp34F;*_tmpEA=((_tmp34F=_cycalloc(sizeof(*_tmp34F)),((
_tmp34F->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk),_tmp34F))));}goto _LL96;
_LL99: {struct Cyc_Absyn_Less_kb_struct*_tmpEB=(struct Cyc_Absyn_Less_kb_struct*)
_tmpE7;if(_tmpEB->tag != 2)goto _LL9B;else{_tmpEC=_tmpEB->f1;_tmpED=(struct Cyc_Core_Opt**)&
_tmpEB->f1;_tmpEE=_tmpEB->f2;}}_LL9A:{struct Cyc_Core_Opt*_tmp350;*_tmpED=((
_tmp350=_cycalloc(sizeof(*_tmp350)),((_tmp350->v=Cyc_Tcutil_kind_to_bound(_tmpEE),
_tmp350))));}goto _LL96;_LL9B:;_LL9C: continue;_LL96:;}}ge->typedefs=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(
ge->typedefs,v,td);}static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,enum Cyc_Absyn_AggrKind
str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*
fields){struct _RegionHandle _tmpF4=_new_region("uprev_rgn");struct _RegionHandle*
uprev_rgn=& _tmpF4;_push_region(uprev_rgn);for(0;rpo != 0;rpo=rpo->tl){struct
_tuple9 _tmpF6;void*_tmpF7;void*_tmpF8;struct _tuple9*_tmpF5=(struct _tuple9*)rpo->hd;
_tmpF6=*_tmpF5;_tmpF7=_tmpF6.f1;_tmpF8=_tmpF6.f2;Cyc_Tcutil_check_type(loc,te,
tvs,& Cyc_Tcutil_ek,0,_tmpF7);Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,
_tmpF8);}{struct Cyc_List_List*prev_fields=0;struct Cyc_List_List*_tmpF9=fields;
for(0;_tmpF9 != 0;_tmpF9=_tmpF9->tl){struct Cyc_Absyn_Aggrfield _tmpFB;struct
_dyneither_ptr*_tmpFC;struct Cyc_Absyn_Tqual _tmpFD;void*_tmpFE;struct Cyc_Absyn_Exp*
_tmpFF;struct Cyc_List_List*_tmp100;struct Cyc_Absyn_Aggrfield*_tmpFA=(struct Cyc_Absyn_Aggrfield*)
_tmpF9->hd;_tmpFB=*_tmpFA;_tmpFC=_tmpFB.name;_tmpFD=_tmpFB.tq;_tmpFE=_tmpFB.type;
_tmpFF=_tmpFB.width;_tmp100=_tmpFB.attributes;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpFC)){const char*
_tmp354;void*_tmp353[1];struct Cyc_String_pa_struct _tmp352;(_tmp352.tag=0,((
_tmp352.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpFC),((_tmp353[0]=&
_tmp352,Cyc_Tcutil_terr(loc,((_tmp354="duplicate member %s",_tag_dyneither(
_tmp354,sizeof(char),20))),_tag_dyneither(_tmp353,sizeof(void*),1)))))));}{const
char*_tmp355;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpFC,((_tmp355="",
_tag_dyneither(_tmp355,sizeof(char),1))))!= 0){struct Cyc_List_List*_tmp356;
prev_fields=((_tmp356=_region_malloc(uprev_rgn,sizeof(*_tmp356)),((_tmp356->hd=
_tmpFC,((_tmp356->tl=prev_fields,_tmp356))))));}}{struct Cyc_Absyn_Kind*
field_kind=& Cyc_Tcutil_tmk;if(_tmpF9->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,_tmpFE);(((
struct Cyc_Absyn_Aggrfield*)_tmpF9->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(((struct Cyc_Absyn_Aggrfield*)_tmpF9->hd)->tq).print_const,_tmpFE);Cyc_Tcutil_check_bitfield(
loc,te,_tmpFE,_tmpFF,_tmpFC);};}};;_pop_region(uprev_rgn);}static void Cyc_Tc_rule_out_memkind(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs,
int constrain_top_kind){struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp106=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);
struct Cyc_Core_Opt*_tmp108;struct Cyc_Core_Opt**_tmp109;struct Cyc_Core_Opt*
_tmp10B;struct Cyc_Core_Opt**_tmp10C;struct Cyc_Absyn_Kind*_tmp10D;struct Cyc_Absyn_Kind
_tmp10E;enum Cyc_Absyn_KindQual _tmp10F;enum Cyc_Absyn_AliasQual _tmp110;struct Cyc_Core_Opt*
_tmp112;struct Cyc_Core_Opt**_tmp113;struct Cyc_Absyn_Kind*_tmp114;struct Cyc_Absyn_Kind
_tmp115;enum Cyc_Absyn_KindQual _tmp116;enum Cyc_Absyn_AliasQual _tmp117;struct Cyc_Core_Opt*
_tmp119;struct Cyc_Core_Opt**_tmp11A;struct Cyc_Absyn_Kind*_tmp11B;struct Cyc_Absyn_Kind*
_tmp11D;struct Cyc_Absyn_Kind _tmp11E;enum Cyc_Absyn_KindQual _tmp11F;enum Cyc_Absyn_AliasQual
_tmp120;_LL9E: {struct Cyc_Absyn_Unknown_kb_struct*_tmp107=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp106;if(_tmp107->tag != 1)goto _LLA0;else{_tmp108=_tmp107->f1;_tmp109=(struct
Cyc_Core_Opt**)& _tmp107->f1;}}_LL9F:{struct Cyc_Core_Opt*_tmp357;*_tmp109=((
_tmp357=_cycalloc(sizeof(*_tmp357)),((_tmp357->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),
_tmp357))));}continue;_LLA0: {struct Cyc_Absyn_Less_kb_struct*_tmp10A=(struct Cyc_Absyn_Less_kb_struct*)
_tmp106;if(_tmp10A->tag != 2)goto _LLA2;else{_tmp10B=_tmp10A->f1;_tmp10C=(struct
Cyc_Core_Opt**)& _tmp10A->f1;_tmp10D=_tmp10A->f2;_tmp10E=*_tmp10D;_tmp10F=_tmp10E.kind;
if(_tmp10F != Cyc_Absyn_MemKind)goto _LLA2;_tmp110=_tmp10E.aliasqual;}}_LLA1: if(
constrain_top_kind  && _tmp110 == Cyc_Absyn_Top){struct Cyc_Absyn_Kind*_tmp35A;
struct Cyc_Core_Opt*_tmp359;*_tmp10C=((_tmp359=_cycalloc(sizeof(*_tmp359)),((
_tmp359->v=Cyc_Tcutil_kind_to_bound(((_tmp35A=_cycalloc_atomic(sizeof(*_tmp35A)),((
_tmp35A->kind=Cyc_Absyn_BoxKind,((_tmp35A->aliasqual=Cyc_Absyn_Aliasable,_tmp35A))))))),
_tmp359))));}else{struct Cyc_Absyn_Kind*_tmp35D;struct Cyc_Core_Opt*_tmp35C;*
_tmp10C=((_tmp35C=_cycalloc(sizeof(*_tmp35C)),((_tmp35C->v=Cyc_Tcutil_kind_to_bound(((
_tmp35D=_cycalloc_atomic(sizeof(*_tmp35D)),((_tmp35D->kind=Cyc_Absyn_BoxKind,((
_tmp35D->aliasqual=_tmp110,_tmp35D))))))),_tmp35C))));}continue;_LLA2:{struct Cyc_Absyn_Less_kb_struct*
_tmp111=(struct Cyc_Absyn_Less_kb_struct*)_tmp106;if(_tmp111->tag != 2)goto _LLA4;
else{_tmp112=_tmp111->f1;_tmp113=(struct Cyc_Core_Opt**)& _tmp111->f1;_tmp114=
_tmp111->f2;_tmp115=*_tmp114;_tmp116=_tmp115.kind;if(_tmp116 != Cyc_Absyn_BoxKind)
goto _LLA4;_tmp117=_tmp115.aliasqual;if(_tmp117 != Cyc_Absyn_Top)goto _LLA4;}}if(!
constrain_top_kind)goto _LLA4;_LLA3:{struct Cyc_Absyn_Kind*_tmp360;struct Cyc_Core_Opt*
_tmp35F;*_tmp113=((_tmp35F=_cycalloc(sizeof(*_tmp35F)),((_tmp35F->v=Cyc_Tcutil_kind_to_bound(((
_tmp360=_cycalloc_atomic(sizeof(*_tmp360)),((_tmp360->kind=Cyc_Absyn_BoxKind,((
_tmp360->aliasqual=Cyc_Absyn_Aliasable,_tmp360))))))),_tmp35F))));}continue;
_LLA4: {struct Cyc_Absyn_Less_kb_struct*_tmp118=(struct Cyc_Absyn_Less_kb_struct*)
_tmp106;if(_tmp118->tag != 2)goto _LLA6;else{_tmp119=_tmp118->f1;_tmp11A=(struct
Cyc_Core_Opt**)& _tmp118->f1;_tmp11B=_tmp118->f2;}}_LLA5:{struct Cyc_Core_Opt*
_tmp361;*_tmp11A=((_tmp361=_cycalloc(sizeof(*_tmp361)),((_tmp361->v=Cyc_Tcutil_kind_to_bound(
_tmp11B),_tmp361))));}continue;_LLA6: {struct Cyc_Absyn_Eq_kb_struct*_tmp11C=(
struct Cyc_Absyn_Eq_kb_struct*)_tmp106;if(_tmp11C->tag != 0)goto _LLA8;else{_tmp11D=
_tmp11C->f1;_tmp11E=*_tmp11D;_tmp11F=_tmp11E.kind;if(_tmp11F != Cyc_Absyn_MemKind)
goto _LLA8;_tmp120=_tmp11E.aliasqual;}}_LLA7:{const char*_tmp369;void*_tmp368[3];
struct Cyc_String_pa_struct _tmp367;struct Cyc_String_pa_struct _tmp366;struct Cyc_Absyn_Kind*
_tmp365;struct Cyc_String_pa_struct _tmp364;(_tmp364.tag=0,((_tmp364.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((_tmp365=
_cycalloc_atomic(sizeof(*_tmp365)),((_tmp365->kind=Cyc_Absyn_MemKind,((_tmp365->aliasqual=
_tmp120,_tmp365)))))))),((_tmp366.tag=0,((_tmp366.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp367.tag=0,((
_tmp367.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp368[0]=&
_tmp367,((_tmp368[1]=& _tmp366,((_tmp368[2]=& _tmp364,Cyc_Tcutil_terr(loc,((
_tmp369="type %s attempts to abstract type variable %s of kind %s",
_tag_dyneither(_tmp369,sizeof(char),57))),_tag_dyneither(_tmp368,sizeof(void*),3)))))))))))))))))));}
continue;_LLA8:;_LLA9: continue;_LL9D:;}}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*
f1;struct Cyc_Absyn_Aggrdecl***f2;};void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl*
ad){struct _dyneither_ptr*_tmp12F=(*ad->name).f2;{struct Cyc_List_List*atts=ad->attributes;
for(0;atts != 0;atts=atts->tl){void*_tmp130=(void*)atts->hd;_LLAB: {struct Cyc_Absyn_Packed_att_struct*
_tmp131=(struct Cyc_Absyn_Packed_att_struct*)_tmp130;if(_tmp131->tag != 7)goto
_LLAD;}_LLAC: goto _LLAE;_LLAD: {struct Cyc_Absyn_Aligned_att_struct*_tmp132=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp130;if(_tmp132->tag != 6)goto _LLAF;}_LLAE:
continue;_LLAF:;_LLB0:{const char*_tmp36E;void*_tmp36D[2];struct Cyc_String_pa_struct
_tmp36C;struct Cyc_String_pa_struct _tmp36B;(_tmp36B.tag=0,((_tmp36B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp12F),((_tmp36C.tag=0,((_tmp36C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp36D[0]=& _tmp36C,((_tmp36D[1]=& _tmp36B,Cyc_Tcutil_terr(loc,((
_tmp36E="bad attribute %s in  %s definition",_tag_dyneither(_tmp36E,sizeof(char),
35))),_tag_dyneither(_tmp36D,sizeof(void*),2)))))))))))));}goto _LLAA;_LLAA:;}}{
struct Cyc_List_List*_tmp137=ad->tvs;{union Cyc_Absyn_Nmspace _tmp138=(*ad->name).f1;
struct Cyc_List_List*_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_List_List*
_tmp13B;_LLB2: if((_tmp138.Rel_n).tag != 1)goto _LLB4;_tmp139=(struct Cyc_List_List*)(
_tmp138.Rel_n).val;if(_tmp139 != 0)goto _LLB4;_LLB3: goto _LLB5;_LLB4: if((_tmp138.C_n).tag
!= 3)goto _LLB6;_tmp13A=(struct Cyc_List_List*)(_tmp138.C_n).val;if(_tmp13A != 0)
goto _LLB6;_LLB5: goto _LLB7;_LLB6: if((_tmp138.Abs_n).tag != 2)goto _LLB8;_tmp13B=(
struct Cyc_List_List*)(_tmp138.Abs_n).val;if(_tmp13B != 0)goto _LLB8;_LLB7: goto
_LLB1;_LLB8:;_LLB9:{const char*_tmp372;void*_tmp371[1];struct Cyc_String_pa_struct
_tmp370;(_tmp370.tag=0,((_tmp370.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(ad->name)),((_tmp371[0]=& _tmp370,Cyc_Tcutil_terr(loc,((
_tmp372="qualified struct declarations are not implemented (%s)",_tag_dyneither(
_tmp372,sizeof(char),55))),_tag_dyneither(_tmp371,sizeof(void*),1)))))));}
return;_LLB1:;}(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns,ad->sc == Cyc_Absyn_ExternC);
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);Cyc_Tcutil_add_tvar_identities(ad->tvs);{
struct _tuple14 _tmp373;struct _tuple14 _tmp140=(_tmp373.f1=ad->impl,((_tmp373.f2=((
struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
ge->aggrdecls,_tmp12F),_tmp373)));struct Cyc_Absyn_AggrdeclImpl*_tmp141;struct Cyc_Absyn_Aggrdecl***
_tmp142;struct Cyc_Absyn_AggrdeclImpl*_tmp143;struct Cyc_Absyn_AggrdeclImpl _tmp144;
struct Cyc_List_List*_tmp145;struct Cyc_List_List*_tmp146;struct Cyc_List_List*
_tmp147;int _tmp148;struct Cyc_Absyn_Aggrdecl***_tmp149;struct Cyc_Absyn_AggrdeclImpl*
_tmp14A;struct Cyc_Absyn_AggrdeclImpl _tmp14B;struct Cyc_List_List*_tmp14C;struct
Cyc_List_List*_tmp14D;struct Cyc_List_List*_tmp14E;int _tmp14F;struct Cyc_Absyn_Aggrdecl***
_tmp150;struct Cyc_Absyn_Aggrdecl**_tmp151;struct Cyc_Absyn_AggrdeclImpl*_tmp152;
struct Cyc_Absyn_Aggrdecl***_tmp153;struct Cyc_Absyn_Aggrdecl**_tmp154;_LLBB:
_tmp141=_tmp140.f1;if(_tmp141 != 0)goto _LLBD;_tmp142=_tmp140.f2;if(_tmp142 != 0)
goto _LLBD;_LLBC: Cyc_Tc_rule_out_memkind(loc,_tmp12F,_tmp137,0);{struct Cyc_Absyn_Aggrdecl**
_tmp374;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,
_tmp12F,((_tmp374=_cycalloc(sizeof(*_tmp374)),((_tmp374[0]=ad,_tmp374)))));}goto
_LLBA;_LLBD: _tmp143=_tmp140.f1;if(_tmp143 == 0)goto _LLBF;_tmp144=*_tmp143;_tmp145=
_tmp144.exist_vars;_tmp146=_tmp144.rgn_po;_tmp147=_tmp144.fields;_tmp148=_tmp144.tagged;
_tmp149=_tmp140.f2;if(_tmp149 != 0)goto _LLBF;_LLBE: {struct Cyc_Absyn_Aggrdecl*
_tmp377;struct Cyc_Absyn_Aggrdecl**_tmp376;struct Cyc_Absyn_Aggrdecl**_tmp156=(
_tmp376=_cycalloc(sizeof(*_tmp376)),((_tmp376[0]=((_tmp377=_cycalloc(sizeof(*
_tmp377)),((_tmp377->kind=ad->kind,((_tmp377->sc=Cyc_Absyn_Extern,((_tmp377->name=
ad->name,((_tmp377->tvs=_tmp137,((_tmp377->impl=0,((_tmp377->attributes=ad->attributes,
_tmp377)))))))))))))),_tmp376)));ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(
ge->aggrdecls,_tmp12F,_tmp156);Cyc_Tcutil_check_unique_tvars(loc,_tmp145);Cyc_Tcutil_add_tvar_identities(
_tmp145);if(_tmp148  && ad->kind == Cyc_Absyn_StructA){const char*_tmp37A;void*
_tmp379;(_tmp379=0,Cyc_Tcutil_terr(loc,((_tmp37A="@tagged qualifier is only allowed on union declarations",
_tag_dyneither(_tmp37A,sizeof(char),56))),_tag_dyneither(_tmp379,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp137,_tmp145),_tmp146,_tmp147);Cyc_Tc_rule_out_memkind(
loc,_tmp12F,_tmp137,0);Cyc_Tc_rule_out_memkind(loc,_tmp12F,_tmp145,1);if(ad->kind
== Cyc_Absyn_UnionA  && !_tmp148){struct Cyc_List_List*f=_tmp147;for(0;f != 0;f=f->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)){const char*
_tmp380;void*_tmp37F[3];struct Cyc_String_pa_struct _tmp37E;struct Cyc_String_pa_struct
_tmp37D;struct Cyc_String_pa_struct _tmp37C;(_tmp37C.tag=0,((_tmp37C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)
f->hd)->type)),((_tmp37D.tag=0,((_tmp37D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp12F),((_tmp37E.tag=0,((_tmp37E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp37F[0]=&
_tmp37E,((_tmp37F[1]=& _tmp37D,((_tmp37F[2]=& _tmp37C,Cyc_Tcutil_warn(loc,((
_tmp380="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp380,sizeof(char),98))),_tag_dyneither(_tmp37F,sizeof(void*),3)))))))))))))))))));}}}*
_tmp156=ad;goto _LLBA;}_LLBF: _tmp14A=_tmp140.f1;if(_tmp14A == 0)goto _LLC1;_tmp14B=*
_tmp14A;_tmp14C=_tmp14B.exist_vars;_tmp14D=_tmp14B.rgn_po;_tmp14E=_tmp14B.fields;
_tmp14F=_tmp14B.tagged;_tmp150=_tmp140.f2;if(_tmp150 == 0)goto _LLC1;_tmp151=*
_tmp150;_LLC0: if(ad->kind != (*_tmp151)->kind){const char*_tmp383;void*_tmp382;(
_tmp382=0,Cyc_Tcutil_terr(loc,((_tmp383="cannot reuse struct names for unions and vice-versa",
_tag_dyneither(_tmp383,sizeof(char),52))),_tag_dyneither(_tmp382,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*_tmp162=*_tmp151;{struct Cyc_Absyn_Aggrdecl*_tmp384;*
_tmp151=((_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384->kind=ad->kind,((_tmp384->sc=
Cyc_Absyn_Extern,((_tmp384->name=ad->name,((_tmp384->tvs=_tmp137,((_tmp384->impl=
0,((_tmp384->attributes=ad->attributes,_tmp384))))))))))))));}Cyc_Tcutil_check_unique_tvars(
loc,_tmp14C);Cyc_Tcutil_add_tvar_identities(_tmp14C);if(_tmp14F  && ad->kind == 
Cyc_Absyn_StructA){const char*_tmp387;void*_tmp386;(_tmp386=0,Cyc_Tcutil_terr(loc,((
_tmp387="@tagged qualifier is only allowed on union declarations",_tag_dyneither(
_tmp387,sizeof(char),56))),_tag_dyneither(_tmp386,sizeof(void*),0)));}Cyc_Tc_tcAggrImpl(
te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp137,_tmp14C),_tmp14D,_tmp14E);*_tmp151=_tmp162;_tmp154=
_tmp151;goto _LLC2;};_LLC1: _tmp152=_tmp140.f1;if(_tmp152 != 0)goto _LLBA;_tmp153=
_tmp140.f2;if(_tmp153 == 0)goto _LLBA;_tmp154=*_tmp153;_LLC2: {struct Cyc_Absyn_Aggrdecl*
_tmp166=Cyc_Tcdecl_merge_aggrdecl(*_tmp154,ad,loc,Cyc_Tc_tc_msg);if(_tmp166 == 0)
return;else{Cyc_Tc_rule_out_memkind(loc,_tmp12F,_tmp137,0);if(ad->impl != 0)Cyc_Tc_rule_out_memkind(
loc,_tmp12F,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);*
_tmp154=(struct Cyc_Absyn_Aggrdecl*)_tmp166;ad=(struct Cyc_Absyn_Aggrdecl*)_tmp166;
goto _LLBA;}}_LLBA:;}{struct Cyc_Tcenv_AggrRes_struct*_tmp38D;struct Cyc_Tcenv_AggrRes_struct
_tmp38C;struct _tuple12*_tmp38B;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(ge->ordinaries,
_tmp12F,(struct _tuple12*)((_tmp38B=_cycalloc(sizeof(*_tmp38B)),((_tmp38B->f1=(
void*)((_tmp38D=_cycalloc(sizeof(*_tmp38D)),((_tmp38D[0]=((_tmp38C.tag=1,((
_tmp38C.f1=ad,_tmp38C)))),_tmp38D)))),((_tmp38B->f2=1,_tmp38B)))))));};};}static
void Cyc_Tc_rule_out_mem_and_unique(struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*v,struct Cyc_List_List*tvs){struct Cyc_List_List*tvs2=tvs;for(0;
tvs2 != 0;tvs2=tvs2->tl){void*_tmp16A=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp16C;struct Cyc_Core_Opt**_tmp16D;struct
Cyc_Core_Opt*_tmp16F;struct Cyc_Core_Opt**_tmp170;struct Cyc_Absyn_Kind*_tmp171;
struct Cyc_Absyn_Kind _tmp172;enum Cyc_Absyn_KindQual _tmp173;enum Cyc_Absyn_AliasQual
_tmp174;struct Cyc_Core_Opt*_tmp176;struct Cyc_Core_Opt**_tmp177;struct Cyc_Absyn_Kind*
_tmp178;struct Cyc_Absyn_Kind _tmp179;enum Cyc_Absyn_KindQual _tmp17A;enum Cyc_Absyn_AliasQual
_tmp17B;struct Cyc_Core_Opt*_tmp17D;struct Cyc_Core_Opt**_tmp17E;struct Cyc_Absyn_Kind*
_tmp17F;struct Cyc_Absyn_Kind _tmp180;enum Cyc_Absyn_KindQual _tmp181;enum Cyc_Absyn_AliasQual
_tmp182;struct Cyc_Core_Opt*_tmp184;struct Cyc_Core_Opt**_tmp185;struct Cyc_Absyn_Kind*
_tmp186;struct Cyc_Absyn_Kind _tmp187;enum Cyc_Absyn_KindQual _tmp188;enum Cyc_Absyn_AliasQual
_tmp189;struct Cyc_Core_Opt*_tmp18B;struct Cyc_Core_Opt**_tmp18C;struct Cyc_Absyn_Kind*
_tmp18D;struct Cyc_Absyn_Kind _tmp18E;enum Cyc_Absyn_KindQual _tmp18F;enum Cyc_Absyn_AliasQual
_tmp190;struct Cyc_Absyn_Kind*_tmp192;struct Cyc_Absyn_Kind _tmp193;enum Cyc_Absyn_KindQual
_tmp194;enum Cyc_Absyn_AliasQual _tmp195;struct Cyc_Core_Opt*_tmp197;struct Cyc_Core_Opt**
_tmp198;struct Cyc_Absyn_Kind*_tmp199;struct Cyc_Absyn_Kind _tmp19A;enum Cyc_Absyn_KindQual
_tmp19B;enum Cyc_Absyn_AliasQual _tmp19C;struct Cyc_Absyn_Kind*_tmp19E;struct Cyc_Absyn_Kind
_tmp19F;enum Cyc_Absyn_KindQual _tmp1A0;enum Cyc_Absyn_AliasQual _tmp1A1;struct Cyc_Absyn_Kind*
_tmp1A3;struct Cyc_Absyn_Kind _tmp1A4;enum Cyc_Absyn_KindQual _tmp1A5;enum Cyc_Absyn_AliasQual
_tmp1A6;_LLC4: {struct Cyc_Absyn_Unknown_kb_struct*_tmp16B=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp16A;if(_tmp16B->tag != 1)goto _LLC6;else{_tmp16C=_tmp16B->f1;_tmp16D=(struct
Cyc_Core_Opt**)& _tmp16B->f1;}}_LLC5: _tmp170=_tmp16D;goto _LLC7;_LLC6: {struct Cyc_Absyn_Less_kb_struct*
_tmp16E=(struct Cyc_Absyn_Less_kb_struct*)_tmp16A;if(_tmp16E->tag != 2)goto _LLC8;
else{_tmp16F=_tmp16E->f1;_tmp170=(struct Cyc_Core_Opt**)& _tmp16E->f1;_tmp171=
_tmp16E->f2;_tmp172=*_tmp171;_tmp173=_tmp172.kind;if(_tmp173 != Cyc_Absyn_MemKind)
goto _LLC8;_tmp174=_tmp172.aliasqual;if(_tmp174 != Cyc_Absyn_Top)goto _LLC8;}}_LLC7:
_tmp177=_tmp170;goto _LLC9;_LLC8: {struct Cyc_Absyn_Less_kb_struct*_tmp175=(struct
Cyc_Absyn_Less_kb_struct*)_tmp16A;if(_tmp175->tag != 2)goto _LLCA;else{_tmp176=
_tmp175->f1;_tmp177=(struct Cyc_Core_Opt**)& _tmp175->f1;_tmp178=_tmp175->f2;
_tmp179=*_tmp178;_tmp17A=_tmp179.kind;if(_tmp17A != Cyc_Absyn_MemKind)goto _LLCA;
_tmp17B=_tmp179.aliasqual;if(_tmp17B != Cyc_Absyn_Aliasable)goto _LLCA;}}_LLC9:{
struct Cyc_Core_Opt*_tmp38E;*_tmp177=((_tmp38E=_cycalloc(sizeof(*_tmp38E)),((
_tmp38E->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp38E))));}goto _LLC3;_LLCA: {
struct Cyc_Absyn_Less_kb_struct*_tmp17C=(struct Cyc_Absyn_Less_kb_struct*)_tmp16A;
if(_tmp17C->tag != 2)goto _LLCC;else{_tmp17D=_tmp17C->f1;_tmp17E=(struct Cyc_Core_Opt**)&
_tmp17C->f1;_tmp17F=_tmp17C->f2;_tmp180=*_tmp17F;_tmp181=_tmp180.kind;if(_tmp181
!= Cyc_Absyn_AnyKind)goto _LLCC;_tmp182=_tmp180.aliasqual;if(_tmp182 != Cyc_Absyn_Top)
goto _LLCC;}}_LLCB: _tmp185=_tmp17E;goto _LLCD;_LLCC: {struct Cyc_Absyn_Less_kb_struct*
_tmp183=(struct Cyc_Absyn_Less_kb_struct*)_tmp16A;if(_tmp183->tag != 2)goto _LLCE;
else{_tmp184=_tmp183->f1;_tmp185=(struct Cyc_Core_Opt**)& _tmp183->f1;_tmp186=
_tmp183->f2;_tmp187=*_tmp186;_tmp188=_tmp187.kind;if(_tmp188 != Cyc_Absyn_AnyKind)
goto _LLCE;_tmp189=_tmp187.aliasqual;if(_tmp189 != Cyc_Absyn_Aliasable)goto _LLCE;}}
_LLCD:{struct Cyc_Core_Opt*_tmp38F;*_tmp185=((_tmp38F=_cycalloc(sizeof(*_tmp38F)),((
_tmp38F->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak),_tmp38F))));}goto _LLC3;_LLCE: {
struct Cyc_Absyn_Less_kb_struct*_tmp18A=(struct Cyc_Absyn_Less_kb_struct*)_tmp16A;
if(_tmp18A->tag != 2)goto _LLD0;else{_tmp18B=_tmp18A->f1;_tmp18C=(struct Cyc_Core_Opt**)&
_tmp18A->f1;_tmp18D=_tmp18A->f2;_tmp18E=*_tmp18D;_tmp18F=_tmp18E.kind;if(_tmp18F
!= Cyc_Absyn_RgnKind)goto _LLD0;_tmp190=_tmp18E.aliasqual;if(_tmp190 != Cyc_Absyn_Top)
goto _LLD0;}}_LLCF:{struct Cyc_Core_Opt*_tmp390;*_tmp18C=((_tmp390=_cycalloc(
sizeof(*_tmp390)),((_tmp390->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp390))));}
goto _LLC3;_LLD0: {struct Cyc_Absyn_Eq_kb_struct*_tmp191=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp16A;if(_tmp191->tag != 0)goto _LLD2;else{_tmp192=_tmp191->f1;_tmp193=*_tmp192;
_tmp194=_tmp193.kind;if(_tmp194 != Cyc_Absyn_RgnKind)goto _LLD2;_tmp195=_tmp193.aliasqual;
if(_tmp195 != Cyc_Absyn_Top)goto _LLD2;}}_LLD1:{const char*_tmp395;void*_tmp394[2];
struct Cyc_String_pa_struct _tmp393;struct Cyc_String_pa_struct _tmp392;(_tmp392.tag=
0,((_tmp392.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
tvs2->hd)->name),((_tmp393.tag=0,((_tmp393.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*v),((_tmp394[0]=& _tmp393,((_tmp394[1]=& _tmp392,Cyc_Tcutil_terr(
loc,((_tmp395="type %s attempts to abstract type variable %s of kind TR",
_tag_dyneither(_tmp395,sizeof(char),57))),_tag_dyneither(_tmp394,sizeof(void*),2)))))))))))));}
goto _LLC3;_LLD2: {struct Cyc_Absyn_Less_kb_struct*_tmp196=(struct Cyc_Absyn_Less_kb_struct*)
_tmp16A;if(_tmp196->tag != 2)goto _LLD4;else{_tmp197=_tmp196->f1;_tmp198=(struct
Cyc_Core_Opt**)& _tmp196->f1;_tmp199=_tmp196->f2;_tmp19A=*_tmp199;_tmp19B=_tmp19A.kind;
_tmp19C=_tmp19A.aliasqual;if(_tmp19C != Cyc_Absyn_Unique)goto _LLD4;}}_LLD3:
_tmp1A0=_tmp19B;goto _LLD5;_LLD4: {struct Cyc_Absyn_Eq_kb_struct*_tmp19D=(struct
Cyc_Absyn_Eq_kb_struct*)_tmp16A;if(_tmp19D->tag != 0)goto _LLD6;else{_tmp19E=
_tmp19D->f1;_tmp19F=*_tmp19E;_tmp1A0=_tmp19F.kind;_tmp1A1=_tmp19F.aliasqual;if(
_tmp1A1 != Cyc_Absyn_Unique)goto _LLD6;}}_LLD5:{const char*_tmp39D;void*_tmp39C[3];
struct Cyc_String_pa_struct _tmp39B;struct Cyc_String_pa_struct _tmp39A;struct Cyc_Absyn_Kind*
_tmp399;struct Cyc_String_pa_struct _tmp398;(_tmp398.tag=0,((_tmp398.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((_tmp399=
_cycalloc_atomic(sizeof(*_tmp399)),((_tmp399->kind=_tmp1A0,((_tmp399->aliasqual=
Cyc_Absyn_Unique,_tmp399)))))))),((_tmp39A.tag=0,((_tmp39A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((
_tmp39B.tag=0,((_tmp39B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((
_tmp39C[0]=& _tmp39B,((_tmp39C[1]=& _tmp39A,((_tmp39C[2]=& _tmp398,Cyc_Tcutil_terr(
loc,((_tmp39D="type %s attempts to abstract type variable %s of kind %s",
_tag_dyneither(_tmp39D,sizeof(char),57))),_tag_dyneither(_tmp39C,sizeof(void*),3)))))))))))))))))));}
goto _LLC3;_LLD6: {struct Cyc_Absyn_Eq_kb_struct*_tmp1A2=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp16A;if(_tmp1A2->tag != 0)goto _LLD8;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=*_tmp1A3;
_tmp1A5=_tmp1A4.kind;if(_tmp1A5 != Cyc_Absyn_MemKind)goto _LLD8;_tmp1A6=_tmp1A4.aliasqual;}}
_LLD7:{const char*_tmp3A5;void*_tmp3A4[3];struct Cyc_String_pa_struct _tmp3A3;
struct Cyc_String_pa_struct _tmp3A2;struct Cyc_Absyn_Kind*_tmp3A1;struct Cyc_String_pa_struct
_tmp3A0;(_tmp3A0.tag=0,((_tmp3A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp3A1=_cycalloc_atomic(sizeof(*_tmp3A1)),((_tmp3A1->kind=
Cyc_Absyn_MemKind,((_tmp3A1->aliasqual=_tmp1A6,_tmp3A1)))))))),((_tmp3A2.tag=0,((
_tmp3A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
tvs2->hd)->name),((_tmp3A3.tag=0,((_tmp3A3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*v),((_tmp3A4[0]=& _tmp3A3,((_tmp3A4[1]=& _tmp3A2,((_tmp3A4[2]=&
_tmp3A0,Cyc_Tcutil_terr(loc,((_tmp3A5="type %s attempts to abstract type variable %s of kind %s",
_tag_dyneither(_tmp3A5,sizeof(char),57))),_tag_dyneither(_tmp3A4,sizeof(void*),3)))))))))))))))))));}
goto _LLC3;_LLD8:;_LLD9: goto _LLC3;_LLC3:;}}struct _tuple15{struct Cyc_Absyn_Tqual f1;
void*f2;};static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct _dyneither_ptr
obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*
tvs,struct Cyc_Absyn_Datatypedecl*tudres){{struct Cyc_List_List*_tmp1BA=fields;
for(0;_tmp1BA != 0;_tmp1BA=_tmp1BA->tl){struct Cyc_Absyn_Datatypefield*_tmp1BB=(
struct Cyc_Absyn_Datatypefield*)_tmp1BA->hd;{struct Cyc_List_List*typs=_tmp1BB->typs;
for(0;typs != 0;typs=typs->tl){Cyc_Tcutil_check_type(_tmp1BB->loc,te,tvs,& Cyc_Tcutil_tmk,
0,(*((struct _tuple15*)typs->hd)).f2);{struct _RegionHandle*_tmp1BC=Cyc_Tcenv_get_fnrgn(
te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1BC,(*((struct _tuple15*)typs->hd)).f2)){
const char*_tmp3A9;void*_tmp3A8[1];struct Cyc_String_pa_struct _tmp3A7;(_tmp3A7.tag=
0,((_tmp3A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp1BB->name)),((_tmp3A8[0]=& _tmp3A7,Cyc_Tcutil_terr(_tmp1BB->loc,((_tmp3A9="noalias pointers in datatypes are not allowed (%s)",
_tag_dyneither(_tmp3A9,sizeof(char),51))),_tag_dyneither(_tmp3A8,sizeof(void*),1)))))));}((*((
struct _tuple15*)typs->hd)).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
_tmp1BB->loc,((*((struct _tuple15*)typs->hd)).f1).print_const,(*((struct _tuple15*)
typs->hd)).f2);};}}{union Cyc_Absyn_Nmspace _tmp1C0=(*_tmp1BB->name).f1;struct Cyc_List_List*
_tmp1C1;struct Cyc_List_List*_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_List_List*
_tmp1C4;int _tmp1C5;_LLDB: if((_tmp1C0.Rel_n).tag != 1)goto _LLDD;_tmp1C1=(struct Cyc_List_List*)(
_tmp1C0.Rel_n).val;if(_tmp1C1 != 0)goto _LLDD;_LLDC: if(is_extensible)(*_tmp1BB->name).f1=
Cyc_Absyn_Abs_n(te->ns,0);else{(*_tmp1BB->name).f1=(*name).f1;}goto _LLDA;_LLDD:
if((_tmp1C0.Rel_n).tag != 1)goto _LLDF;_tmp1C2=(struct Cyc_List_List*)(_tmp1C0.Rel_n).val;
_LLDE:{const char*_tmp3AD;void*_tmp3AC[1];struct Cyc_String_pa_struct _tmp3AB;(
_tmp3AB.tag=0,((_tmp3AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp1BB->name)),((_tmp3AC[0]=& _tmp3AB,Cyc_Tcutil_terr(_tmp1BB->loc,((_tmp3AD="qualified datatypefield declarations are not allowed (%s)",
_tag_dyneither(_tmp3AD,sizeof(char),58))),_tag_dyneither(_tmp3AC,sizeof(void*),1)))))));}
goto _LLDA;_LLDF: if((_tmp1C0.C_n).tag != 3)goto _LLE1;_tmp1C3=(struct Cyc_List_List*)(
_tmp1C0.C_n).val;_LLE0: goto _LLDA;_LLE1: if((_tmp1C0.Abs_n).tag != 2)goto _LLE3;
_tmp1C4=(struct Cyc_List_List*)(_tmp1C0.Abs_n).val;_LLE2: goto _LLDA;_LLE3: if((
_tmp1C0.Loc_n).tag != 4)goto _LLDA;_tmp1C5=(int)(_tmp1C0.Loc_n).val;_LLE4: {const
char*_tmp3B0;void*_tmp3AF;(_tmp3AF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3B0="tcDatatypeFields: Loc_n",
_tag_dyneither(_tmp3B0,sizeof(char),24))),_tag_dyneither(_tmp3AF,sizeof(void*),0)));}
_LLDA:;};}}{struct Cyc_List_List*fields2;if(is_extensible){int _tmp1CB=1;struct Cyc_List_List*
_tmp1CC=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp1CB,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp1CB)fields2=_tmp1CC;else{fields2=0;}}else{struct _RegionHandle _tmp1CD=
_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1CD;_push_region(
uprev_rgn);{struct Cyc_List_List*prev_fields=0;{struct Cyc_List_List*fs=fields;
for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Datatypefield*_tmp1CE=(struct Cyc_Absyn_Datatypefield*)
fs->hd;if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),
struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,
prev_fields,(*_tmp1CE->name).f2)){const char*_tmp3B5;void*_tmp3B4[2];struct Cyc_String_pa_struct
_tmp3B3;struct Cyc_String_pa_struct _tmp3B2;(_tmp3B2.tag=0,((_tmp3B2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp3B3.tag=0,((_tmp3B3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1CE->name).f2),((_tmp3B4[0]=&
_tmp3B3,((_tmp3B4[1]=& _tmp3B2,Cyc_Tcutil_terr(_tmp1CE->loc,((_tmp3B5="duplicate field name %s in %s",
_tag_dyneither(_tmp3B5,sizeof(char),30))),_tag_dyneither(_tmp3B4,sizeof(void*),2)))))))))))));}
else{struct Cyc_List_List*_tmp3B6;prev_fields=((_tmp3B6=_region_malloc(uprev_rgn,
sizeof(*_tmp3B6)),((_tmp3B6->hd=(*_tmp1CE->name).f2,((_tmp3B6->tl=prev_fields,
_tmp3B6))))));}if(_tmp1CE->sc != Cyc_Absyn_Public){{const char*_tmp3BA;void*
_tmp3B9[1];struct Cyc_String_pa_struct _tmp3B8;(_tmp3B8.tag=0,((_tmp3B8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1CE->name).f2),((_tmp3B9[0]=&
_tmp3B8,Cyc_Tcutil_warn(loc,((_tmp3BA="ignoring scope of field %s",
_tag_dyneither(_tmp3BA,sizeof(char),27))),_tag_dyneither(_tmp3B9,sizeof(void*),1)))))));}
_tmp1CE->sc=Cyc_Absyn_Public;}}}fields2=fields;};_pop_region(uprev_rgn);}{struct
Cyc_List_List*_tmp1D7=fields;for(0;_tmp1D7 != 0;_tmp1D7=_tmp1D7->tl){struct Cyc_Absyn_Datatypefield*
_tmp1D8=(struct Cyc_Absyn_Datatypefield*)_tmp1D7->hd;struct Cyc_Tcenv_DatatypeRes_struct*
_tmp3C0;struct Cyc_Tcenv_DatatypeRes_struct _tmp3BF;struct _tuple12*_tmp3BE;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple12*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1D8->name).f2,(struct _tuple12*)((
_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE->f1=(void*)((_tmp3C0=_cycalloc(
sizeof(*_tmp3C0)),((_tmp3C0[0]=((_tmp3BF.tag=2,((_tmp3BF.f1=tudres,((_tmp3BF.f2=
_tmp1D8,_tmp3BF)))))),_tmp3C0)))),((_tmp3BE->f2=1,_tmp3BE)))))));}}return fields2;};}
struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};void Cyc_Tc_tcDatatypedecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Datatypedecl*tud){struct _dyneither_ptr*v=(*tud->name).f2;const
char*_tmp3C2;const char*_tmp3C1;struct _dyneither_ptr obj=tud->is_extensible?(
_tmp3C2="@extensible datatype",_tag_dyneither(_tmp3C2,sizeof(char),21)):((
_tmp3C1="datatype",_tag_dyneither(_tmp3C1,sizeof(char),9)));struct Cyc_List_List*
tvs=tud->tvs;Cyc_Tcutil_check_unique_tvars(loc,tvs);Cyc_Tcutil_add_tvar_identities(
tvs);{struct _RegionHandle _tmp1DC=_new_region("temp");struct _RegionHandle*temp=&
_tmp1DC;_push_region(temp);{struct Cyc_Absyn_Datatypedecl***tud_opt;{struct
_handler_cons _tmp1DD;_push_handler(& _tmp1DD);{int _tmp1DF=0;if(setjmp(_tmp1DD.handler))
_tmp1DF=1;if(!_tmp1DF){tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible){struct Cyc_Dict_Absent_struct _tmp3C5;
struct Cyc_Dict_Absent_struct*_tmp3C4;(int)_throw((void*)((_tmp3C4=
_cycalloc_atomic(sizeof(*_tmp3C4)),((_tmp3C4[0]=((_tmp3C5.tag=Cyc_Dict_Absent,
_tmp3C5)),_tmp3C4)))));}if(tud_opt != 0)tud->name=(*(*tud_opt))->name;else{(*tud->name).f1=
Cyc_Absyn_Abs_n(te->ns,0);};_pop_handler();}else{void*_tmp1DE=(void*)_exn_thrown;
void*_tmp1E3=_tmp1DE;_LLE6: {struct Cyc_Dict_Absent_struct*_tmp1E4=(struct Cyc_Dict_Absent_struct*)
_tmp1E3;if(_tmp1E4->tag != Cyc_Dict_Absent)goto _LLE8;}_LLE7:{union Cyc_Absyn_Nmspace
_tmp1E5=(*tud->name).f1;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*_tmp1E7;
_LLEB: if((_tmp1E5.Rel_n).tag != 1)goto _LLED;_tmp1E6=(struct Cyc_List_List*)(
_tmp1E5.Rel_n).val;if(_tmp1E6 != 0)goto _LLED;_LLEC:(*tud->name).f1=Cyc_Absyn_Abs_n(
te->ns,0);goto _LLEA;_LLED: if((_tmp1E5.Abs_n).tag != 2)goto _LLEF;_tmp1E7=(struct
Cyc_List_List*)(_tmp1E5.Abs_n).val;_LLEE: goto _LLF0;_LLEF:;_LLF0:{const char*
_tmp3C9;void*_tmp3C8[1];struct Cyc_String_pa_struct _tmp3C7;(_tmp3C7.tag=0,((
_tmp3C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tud->name)),((_tmp3C8[0]=& _tmp3C7,Cyc_Tcutil_terr(loc,((_tmp3C9="qualified (non-extensible) datatype declarations are not implemented (%s)",
_tag_dyneither(_tmp3C9,sizeof(char),74))),_tag_dyneither(_tmp3C8,sizeof(void*),1)))))));}
_npop_handler(0);return;_LLEA:;}{struct Cyc_Absyn_Datatypedecl***_tmp1EB=((struct
Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
ge->datatypedecls,v);{struct Cyc_Absyn_Datatypedecl***_tmp3CA;tud_opt=(
unsigned int)_tmp1EB?(_tmp3CA=_region_malloc(temp,sizeof(*_tmp3CA)),((_tmp3CA[0]=*
_tmp1EB,_tmp3CA))): 0;}goto _LLE5;};_LLE8:;_LLE9:(void)_throw(_tmp1E3);_LLE5:;}};}{
struct _tuple16 _tmp3CB;struct _tuple16 _tmp1EE=(_tmp3CB.f1=tud->fields,((_tmp3CB.f2=
tud_opt,_tmp3CB)));struct Cyc_Core_Opt*_tmp1EF;struct Cyc_Absyn_Datatypedecl***
_tmp1F0;struct Cyc_Core_Opt*_tmp1F1;struct Cyc_Core_Opt _tmp1F2;struct Cyc_List_List*
_tmp1F3;struct Cyc_List_List**_tmp1F4;struct Cyc_Absyn_Datatypedecl***_tmp1F5;
struct Cyc_Core_Opt*_tmp1F6;struct Cyc_Core_Opt _tmp1F7;struct Cyc_List_List*_tmp1F8;
struct Cyc_List_List**_tmp1F9;struct Cyc_Absyn_Datatypedecl***_tmp1FA;struct Cyc_Absyn_Datatypedecl**
_tmp1FB;struct Cyc_Core_Opt*_tmp1FC;struct Cyc_Absyn_Datatypedecl***_tmp1FD;struct
Cyc_Absyn_Datatypedecl**_tmp1FE;_LLF2: _tmp1EF=_tmp1EE.f1;if(_tmp1EF != 0)goto
_LLF4;_tmp1F0=_tmp1EE.f2;if(_tmp1F0 != 0)goto _LLF4;_LLF3: Cyc_Tc_rule_out_mem_and_unique(
loc,v,tvs);{struct Cyc_Absyn_Datatypedecl**_tmp3CC;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(
ge->datatypedecls,v,((_tmp3CC=_cycalloc(sizeof(*_tmp3CC)),((_tmp3CC[0]=tud,
_tmp3CC)))));}goto _LLF1;_LLF4: _tmp1F1=_tmp1EE.f1;if(_tmp1F1 == 0)goto _LLF6;
_tmp1F2=*_tmp1F1;_tmp1F3=(struct Cyc_List_List*)_tmp1F2.v;_tmp1F4=(struct Cyc_List_List**)&(*
_tmp1EE.f1).v;_tmp1F5=_tmp1EE.f2;if(_tmp1F5 != 0)goto _LLF6;_LLF5: {struct Cyc_Absyn_Datatypedecl*
_tmp3CF;struct Cyc_Absyn_Datatypedecl**_tmp3CE;struct Cyc_Absyn_Datatypedecl**
_tmp200=(_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE[0]=((_tmp3CF=_cycalloc(
sizeof(*_tmp3CF)),((_tmp3CF->sc=Cyc_Absyn_Extern,((_tmp3CF->name=tud->name,((
_tmp3CF->tvs=tvs,((_tmp3CF->fields=0,((_tmp3CF->is_extensible=tud->is_extensible,
_tmp3CF)))))))))))),_tmp3CE)));ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(
ge->datatypedecls,v,_tmp200);*_tmp1F4=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,
tud->name,*_tmp1F4,tvs,tud);Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);*_tmp200=
tud;goto _LLF1;}_LLF6: _tmp1F6=_tmp1EE.f1;if(_tmp1F6 == 0)goto _LLF8;_tmp1F7=*
_tmp1F6;_tmp1F8=(struct Cyc_List_List*)_tmp1F7.v;_tmp1F9=(struct Cyc_List_List**)&(*
_tmp1EE.f1).v;_tmp1FA=_tmp1EE.f2;if(_tmp1FA == 0)goto _LLF8;_tmp1FB=*_tmp1FA;_LLF7: {
struct Cyc_Absyn_Datatypedecl*_tmp203=*_tmp1FB;if((!tud->is_extensible  && (
unsigned int)_tmp203) && _tmp203->is_extensible)tud->is_extensible=1;{struct Cyc_Absyn_Datatypedecl*
_tmp3D0;*_tmp1FB=((_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0->sc=Cyc_Absyn_Extern,((
_tmp3D0->name=tud->name,((_tmp3D0->tvs=tvs,((_tmp3D0->fields=0,((_tmp3D0->is_extensible=
tud->is_extensible,_tmp3D0))))))))))));}*_tmp1F9=Cyc_Tc_tcDatatypeFields(te,ge,
loc,obj,tud->is_extensible,tud->name,*_tmp1F9,tvs,tud);*_tmp1FB=_tmp203;_tmp1FE=
_tmp1FB;goto _LLF9;}_LLF8: _tmp1FC=_tmp1EE.f1;if(_tmp1FC != 0)goto _LLF1;_tmp1FD=
_tmp1EE.f2;if(_tmp1FD == 0)goto _LLF1;_tmp1FE=*_tmp1FD;_LLF9: {struct Cyc_Absyn_Datatypedecl*
_tmp205=Cyc_Tcdecl_merge_datatypedecl(*_tmp1FE,tud,loc,Cyc_Tc_tc_msg);Cyc_Tc_rule_out_mem_and_unique(
loc,v,tvs);if(_tmp205 == 0){_npop_handler(0);return;}else{*_tmp1FE=(struct Cyc_Absyn_Datatypedecl*)
_tmp205;goto _LLF1;}}_LLF1:;};};_pop_region(temp);};}void Cyc_Tc_tcEnumdecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct
Cyc_Absyn_Enumdecl*ed){struct _dyneither_ptr*v=(*ed->name).f2;{union Cyc_Absyn_Nmspace
_tmp208=(*ed->name).f1;struct Cyc_List_List*_tmp209;struct Cyc_List_List*_tmp20A;
_LLFB: if((_tmp208.Rel_n).tag != 1)goto _LLFD;_tmp209=(struct Cyc_List_List*)(
_tmp208.Rel_n).val;if(_tmp209 != 0)goto _LLFD;_LLFC: goto _LLFE;_LLFD: if((_tmp208.Abs_n).tag
!= 2)goto _LLFF;_tmp20A=(struct Cyc_List_List*)(_tmp208.Abs_n).val;if(_tmp20A != 0)
goto _LLFF;_LLFE: goto _LLFA;_LLFF:;_LL100:{const char*_tmp3D4;void*_tmp3D3[1];
struct Cyc_String_pa_struct _tmp3D2;(_tmp3D2.tag=0,((_tmp3D2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(ed->name)),((
_tmp3D3[0]=& _tmp3D2,Cyc_Tcutil_terr(loc,((_tmp3D4="qualified enum declarations are not implemented (%s)",
_tag_dyneither(_tmp3D4,sizeof(char),53))),_tag_dyneither(_tmp3D3,sizeof(void*),1)))))));}
return;_LLFA:;}(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);
if(ed->fields != 0){struct _RegionHandle _tmp20E=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp20E;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;struct Cyc_List_List*fs=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct
Cyc_Absyn_Enumfield*_tmp20F=(struct Cyc_Absyn_Enumfield*)fs->hd;if(((int(*)(int(*
compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,
struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp20F->name).f2)){
const char*_tmp3D8;void*_tmp3D7[1];struct Cyc_String_pa_struct _tmp3D6;(_tmp3D6.tag=
0,((_tmp3D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp20F->name).f2),((
_tmp3D7[0]=& _tmp3D6,Cyc_Tcutil_terr(_tmp20F->loc,((_tmp3D8="duplicate field name %s",
_tag_dyneither(_tmp3D8,sizeof(char),24))),_tag_dyneither(_tmp3D7,sizeof(void*),1)))))));}
else{struct Cyc_List_List*_tmp3D9;prev_fields=((_tmp3D9=_region_malloc(uprev_rgn,
sizeof(*_tmp3D9)),((_tmp3D9->hd=(*_tmp20F->name).f2,((_tmp3D9->tl=prev_fields,
_tmp3D9))))));}if(_tmp20F->tag == 0)_tmp20F->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp20F->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp20F->tag))){const char*_tmp3DE;void*_tmp3DD[2];struct Cyc_String_pa_struct
_tmp3DC;struct Cyc_String_pa_struct _tmp3DB;(_tmp3DB.tag=0,((_tmp3DB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp20F->name).f2),((_tmp3DC.tag=0,((
_tmp3DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3DD[0]=&
_tmp3DC,((_tmp3DD[1]=& _tmp3DB,Cyc_Tcutil_terr(loc,((_tmp3DE="enum %s, field %s: expression is not constant",
_tag_dyneither(_tmp3DE,sizeof(char),46))),_tag_dyneither(_tmp3DD,sizeof(void*),2)))))))))))));}}{
unsigned int _tmp219;int _tmp21A;struct _tuple10 _tmp218=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp20F->tag));_tmp219=_tmp218.f1;_tmp21A=
_tmp218.f2;if(!_tmp21A){const char*_tmp3E1;void*_tmp3E0;(_tmp3E0=0,Cyc_Tcutil_terr(
loc,((_tmp3E1="Cyclone enum tags cannot use sizeof or offsetof or other enum tags",
_tag_dyneither(_tmp3E1,sizeof(char),67))),_tag_dyneither(_tmp3E0,sizeof(void*),0)));}
tag_count=_tmp219 + 1;(*_tmp20F->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);};}};
_pop_region(uprev_rgn);}{struct _handler_cons _tmp21D;_push_handler(& _tmp21D);{int
_tmp21F=0;if(setjmp(_tmp21D.handler))_tmp21F=1;if(!_tmp21F){{struct Cyc_Absyn_Enumdecl**
_tmp220=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);struct Cyc_Absyn_Enumdecl*
_tmp221=Cyc_Tcdecl_merge_enumdecl(*_tmp220,ed,loc,Cyc_Tc_tc_msg);if(_tmp221 == 0){
_npop_handler(0);return;}*_tmp220=(struct Cyc_Absyn_Enumdecl*)_tmp221;};
_pop_handler();}else{void*_tmp21E=(void*)_exn_thrown;void*_tmp223=_tmp21E;_LL102: {
struct Cyc_Dict_Absent_struct*_tmp224=(struct Cyc_Dict_Absent_struct*)_tmp223;if(
_tmp224->tag != Cyc_Dict_Absent)goto _LL104;}_LL103: {struct Cyc_Absyn_Enumdecl**
_tmp3E2;struct Cyc_Absyn_Enumdecl**_tmp225=(_tmp3E2=_cycalloc(sizeof(*_tmp3E2)),((
_tmp3E2[0]=ed,_tmp3E2)));ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,
v,_tmp225);goto _LL101;}_LL104:;_LL105:(void)_throw(_tmp223);_LL101:;}};}if(ed->fields
!= 0){struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp227=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Tcenv_EnumRes_struct*
_tmp3E8;struct Cyc_Tcenv_EnumRes_struct _tmp3E7;struct _tuple12*_tmp3E6;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple12*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp227->name).f2,(struct _tuple12*)((
_tmp3E6=_cycalloc(sizeof(*_tmp3E6)),((_tmp3E6->f1=(void*)((_tmp3E8=_cycalloc(
sizeof(*_tmp3E8)),((_tmp3E8[0]=((_tmp3E7.tag=3,((_tmp3E7.f1=ed,((_tmp3E7.f2=
_tmp227,_tmp3E7)))))),_tmp3E8)))),((_tmp3E6->f2=1,_tmp3E6)))))));}}}static int Cyc_Tc_okay_externC(
struct Cyc_Position_Segment*loc,enum Cyc_Absyn_Scope sc,int in_include){switch(sc){
case Cyc_Absyn_Static: _LL106: if(!in_include){const char*_tmp3EB;void*_tmp3EA;(
_tmp3EA=0,Cyc_Tcutil_warn(loc,((_tmp3EB="static declaration nested within extern \"C\"",
_tag_dyneither(_tmp3EB,sizeof(char),44))),_tag_dyneither(_tmp3EA,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Abstract: _LL107:{const char*_tmp3EE;void*_tmp3ED;(_tmp3ED=0,
Cyc_Tcutil_warn(loc,((_tmp3EE="abstract declaration nested within extern \"C\"",
_tag_dyneither(_tmp3EE,sizeof(char),46))),_tag_dyneither(_tmp3ED,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Public: _LL108: goto _LL109;case Cyc_Absyn_Register: _LL109:
goto _LL10A;case Cyc_Absyn_Extern: _LL10A: return 1;case Cyc_Absyn_ExternC: _LL10B:{
const char*_tmp3F1;void*_tmp3F0;(_tmp3F0=0,Cyc_Tcutil_warn(loc,((_tmp3F1="nested extern \"C\" declaration",
_tag_dyneither(_tmp3F1,sizeof(char),30))),_tag_dyneither(_tmp3F0,sizeof(void*),0)));}
return 1;}}static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,
struct _tuple11*exp){struct Cyc_Position_Segment*_tmp232;struct _tuple0*_tmp233;
struct _tuple11 _tmp231=*exp;_tmp232=_tmp231.f1;_tmp233=_tmp231.f2;{struct _tuple0
_tmp235;union Cyc_Absyn_Nmspace _tmp236;struct _dyneither_ptr*_tmp237;struct _tuple0*
_tmp234=_tmp233;_tmp235=*_tmp234;_tmp236=_tmp235.f1;_tmp237=_tmp235.f2;{union Cyc_Absyn_Nmspace
_tmp238=_tmp236;struct Cyc_List_List*_tmp239;struct Cyc_List_List*_tmp23A;struct
Cyc_List_List*_tmp23B;_LL10E: if((_tmp238.Rel_n).tag != 1)goto _LL110;_tmp239=(
struct Cyc_List_List*)(_tmp238.Rel_n).val;if(_tmp239 != 0)goto _LL110;_LL10F: goto
_LL111;_LL110: if((_tmp238.C_n).tag != 3)goto _LL112;_tmp23A=(struct Cyc_List_List*)(
_tmp238.C_n).val;if(_tmp23A != 0)goto _LL112;_LL111: goto _LL10D;_LL112: if((_tmp238.Abs_n).tag
!= 2)goto _LL114;_tmp23B=(struct Cyc_List_List*)(_tmp238.Abs_n).val;if(_tmp23B != 0)
goto _LL114;_LL113: goto _LL10D;_LL114:;_LL115:{const char*_tmp3F5;void*_tmp3F4[1];
struct Cyc_String_pa_struct _tmp3F3;(_tmp3F3.tag=0,((_tmp3F3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp233)),((
_tmp3F4[0]=& _tmp3F3,Cyc_Tcutil_terr(_tmp232,((_tmp3F5="qualified export variables are not implemented (%s)",
_tag_dyneither(_tmp3F5,sizeof(char),52))),_tag_dyneither(_tmp3F4,sizeof(void*),1)))))));}
return;_LL10D:;}(*_tmp233).f1=Cyc_Absyn_Abs_n(te->ns,1);};}static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int
in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**
exports){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_List_List*last=
0;struct Cyc_List_List*_tmp23F=ds0;for(0;_tmp23F != 0;(last=_tmp23F,_tmp23F=
_tmp23F->tl)){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp23F->hd;struct
Cyc_Position_Segment*loc=d->loc;void*_tmp240=d->r;struct Cyc_Absyn_Vardecl*
_tmp246;struct Cyc_Absyn_Fndecl*_tmp248;struct Cyc_Absyn_Typedefdecl*_tmp24A;
struct Cyc_Absyn_Aggrdecl*_tmp24C;struct Cyc_Absyn_Datatypedecl*_tmp24E;struct Cyc_Absyn_Enumdecl*
_tmp250;struct _dyneither_ptr*_tmp254;struct Cyc_List_List*_tmp255;struct _tuple0*
_tmp257;struct _tuple0 _tmp258;union Cyc_Absyn_Nmspace _tmp259;struct _dyneither_ptr*
_tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*
_tmp25F;struct Cyc_List_List*_tmp260;_LL117: {struct Cyc_Absyn_Let_d_struct*
_tmp241=(struct Cyc_Absyn_Let_d_struct*)_tmp240;if(_tmp241->tag != 2)goto _LL119;}
_LL118: goto _LL11A;_LL119: {struct Cyc_Absyn_Letv_d_struct*_tmp242=(struct Cyc_Absyn_Letv_d_struct*)
_tmp240;if(_tmp242->tag != 3)goto _LL11B;}_LL11A:{const char*_tmp3F8;void*_tmp3F7;(
_tmp3F7=0,Cyc_Tcutil_terr(loc,((_tmp3F8="top level let-declarations are not implemented",
_tag_dyneither(_tmp3F8,sizeof(char),47))),_tag_dyneither(_tmp3F7,sizeof(void*),0)));}
goto _LL116;_LL11B: {struct Cyc_Absyn_Region_d_struct*_tmp243=(struct Cyc_Absyn_Region_d_struct*)
_tmp240;if(_tmp243->tag != 4)goto _LL11D;}_LL11C:{const char*_tmp3FB;void*_tmp3FA;(
_tmp3FA=0,Cyc_Tcutil_terr(loc,((_tmp3FB="top level region declarations are not implemented",
_tag_dyneither(_tmp3FB,sizeof(char),50))),_tag_dyneither(_tmp3FA,sizeof(void*),0)));}
goto _LL116;_LL11D: {struct Cyc_Absyn_Alias_d_struct*_tmp244=(struct Cyc_Absyn_Alias_d_struct*)
_tmp240;if(_tmp244->tag != 5)goto _LL11F;}_LL11E:{const char*_tmp3FE;void*_tmp3FD;(
_tmp3FD=0,Cyc_Tcutil_terr(loc,((_tmp3FE="top level alias declarations are not implemented",
_tag_dyneither(_tmp3FE,sizeof(char),49))),_tag_dyneither(_tmp3FD,sizeof(void*),0)));}
goto _LL116;_LL11F: {struct Cyc_Absyn_Var_d_struct*_tmp245=(struct Cyc_Absyn_Var_d_struct*)
_tmp240;if(_tmp245->tag != 0)goto _LL121;else{_tmp246=_tmp245->f1;}}_LL120: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp246->sc,in_externCinclude))_tmp246->sc=
Cyc_Absyn_ExternC;Cyc_Tc_tcVardecl(te,ge,loc,_tmp246,check_var_init,
in_externCinclude,exports);goto _LL116;_LL121: {struct Cyc_Absyn_Fn_d_struct*
_tmp247=(struct Cyc_Absyn_Fn_d_struct*)_tmp240;if(_tmp247->tag != 1)goto _LL123;
else{_tmp248=_tmp247->f1;}}_LL122: if(in_externC  && Cyc_Tc_okay_externC(d->loc,
_tmp248->sc,in_externCinclude))_tmp248->sc=Cyc_Absyn_ExternC;Cyc_Tc_tcFndecl(te,
ge,loc,_tmp248,in_externCinclude,exports);goto _LL116;_LL123: {struct Cyc_Absyn_Typedef_d_struct*
_tmp249=(struct Cyc_Absyn_Typedef_d_struct*)_tmp240;if(_tmp249->tag != 9)goto
_LL125;else{_tmp24A=_tmp249->f1;}}_LL124: Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp24A);
goto _LL116;_LL125: {struct Cyc_Absyn_Aggr_d_struct*_tmp24B=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp240;if(_tmp24B->tag != 6)goto _LL127;else{_tmp24C=_tmp24B->f1;}}_LL126: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp24C->sc,in_externCinclude))_tmp24C->sc=
Cyc_Absyn_ExternC;Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp24C);goto _LL116;_LL127: {
struct Cyc_Absyn_Datatype_d_struct*_tmp24D=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp240;if(_tmp24D->tag != 7)goto _LL129;else{_tmp24E=_tmp24D->f1;}}_LL128: if(
in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp24E->sc,in_externCinclude))_tmp24E->sc=
Cyc_Absyn_ExternC;Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp24E);goto _LL116;_LL129: {
struct Cyc_Absyn_Enum_d_struct*_tmp24F=(struct Cyc_Absyn_Enum_d_struct*)_tmp240;
if(_tmp24F->tag != 8)goto _LL12B;else{_tmp250=_tmp24F->f1;}}_LL12A: if(in_externC
 && Cyc_Tc_okay_externC(d->loc,_tmp250->sc,in_externCinclude))_tmp250->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp250);goto _LL116;_LL12B: {struct Cyc_Absyn_Porton_d_struct*
_tmp251=(struct Cyc_Absyn_Porton_d_struct*)_tmp240;if(_tmp251->tag != 14)goto
_LL12D;}_LL12C:{const char*_tmp401;void*_tmp400;(_tmp400=0,Cyc_Tcutil_warn(d->loc,((
_tmp401="spurious __cyclone_port_on__",_tag_dyneither(_tmp401,sizeof(char),29))),
_tag_dyneither(_tmp400,sizeof(void*),0)));}goto _LL116;_LL12D: {struct Cyc_Absyn_Portoff_d_struct*
_tmp252=(struct Cyc_Absyn_Portoff_d_struct*)_tmp240;if(_tmp252->tag != 15)goto
_LL12F;}_LL12E: goto _LL116;_LL12F: {struct Cyc_Absyn_Namespace_d_struct*_tmp253=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp240;if(_tmp253->tag != 10)goto _LL131;
else{_tmp254=_tmp253->f1;_tmp255=_tmp253->f2;}}_LL130: {struct Cyc_List_List*
_tmp269=te->ns;struct Cyc_List_List*_tmp402;struct Cyc_List_List*_tmp26A=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp269,((_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402->hd=_tmp254,((_tmp402->tl=
0,_tmp402)))))));if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
ge->namespaces,_tmp254)){ge->namespaces=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(
grgn,ge->namespaces,_tmp254);te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp26A,
Cyc_Tcenv_empty_genv(grgn));}te->ns=_tmp26A;Cyc_Tc_tc_decls(te,_tmp255,
in_externC,in_externCinclude,check_var_init,grgn,exports);te->ns=_tmp269;goto
_LL116;}_LL131: {struct Cyc_Absyn_Using_d_struct*_tmp256=(struct Cyc_Absyn_Using_d_struct*)
_tmp240;if(_tmp256->tag != 11)goto _LL133;else{_tmp257=_tmp256->f1;_tmp258=*
_tmp257;_tmp259=_tmp258.f1;_tmp25A=_tmp258.f2;_tmp25B=_tmp256->f2;}}_LL132: {
struct _dyneither_ptr*first;struct Cyc_List_List*rest;{union Cyc_Absyn_Nmspace
_tmp26C=_tmp259;int _tmp26D;struct Cyc_List_List*_tmp26E;struct Cyc_List_List*
_tmp26F;struct Cyc_List_List*_tmp270;struct Cyc_List_List*_tmp271;struct Cyc_List_List
_tmp272;struct _dyneither_ptr*_tmp273;struct Cyc_List_List*_tmp274;struct Cyc_List_List*
_tmp275;struct Cyc_List_List _tmp276;struct _dyneither_ptr*_tmp277;struct Cyc_List_List*
_tmp278;struct Cyc_List_List*_tmp279;struct Cyc_List_List _tmp27A;struct
_dyneither_ptr*_tmp27B;struct Cyc_List_List*_tmp27C;_LL138: if((_tmp26C.Loc_n).tag
!= 4)goto _LL13A;_tmp26D=(int)(_tmp26C.Loc_n).val;_LL139: goto _LL13B;_LL13A: if((
_tmp26C.Rel_n).tag != 1)goto _LL13C;_tmp26E=(struct Cyc_List_List*)(_tmp26C.Rel_n).val;
if(_tmp26E != 0)goto _LL13C;_LL13B: goto _LL13D;_LL13C: if((_tmp26C.C_n).tag != 3)goto
_LL13E;_tmp26F=(struct Cyc_List_List*)(_tmp26C.C_n).val;if(_tmp26F != 0)goto _LL13E;
_LL13D: goto _LL13F;_LL13E: if((_tmp26C.Abs_n).tag != 2)goto _LL140;_tmp270=(struct
Cyc_List_List*)(_tmp26C.Abs_n).val;if(_tmp270 != 0)goto _LL140;_LL13F: first=
_tmp25A;rest=0;goto _LL137;_LL140: if((_tmp26C.Rel_n).tag != 1)goto _LL142;_tmp271=(
struct Cyc_List_List*)(_tmp26C.Rel_n).val;if(_tmp271 == 0)goto _LL142;_tmp272=*
_tmp271;_tmp273=(struct _dyneither_ptr*)_tmp272.hd;_tmp274=_tmp272.tl;_LL141:
_tmp277=_tmp273;_tmp278=_tmp274;goto _LL143;_LL142: if((_tmp26C.C_n).tag != 3)goto
_LL144;_tmp275=(struct Cyc_List_List*)(_tmp26C.C_n).val;if(_tmp275 == 0)goto _LL144;
_tmp276=*_tmp275;_tmp277=(struct _dyneither_ptr*)_tmp276.hd;_tmp278=_tmp276.tl;
_LL143: _tmp27B=_tmp277;_tmp27C=_tmp278;goto _LL145;_LL144: if((_tmp26C.Abs_n).tag
!= 2)goto _LL137;_tmp279=(struct Cyc_List_List*)(_tmp26C.Abs_n).val;if(_tmp279 == 0)
goto _LL137;_tmp27A=*_tmp279;_tmp27B=(struct _dyneither_ptr*)_tmp27A.hd;_tmp27C=
_tmp27A.tl;_LL145: first=_tmp27B;{struct Cyc_List_List*_tmp403;rest=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp27C,((_tmp403=
_cycalloc(sizeof(*_tmp403)),((_tmp403->hd=_tmp25A,((_tmp403->tl=0,_tmp403)))))));}
goto _LL137;_LL137:;}{struct Cyc_List_List*_tmp27E=Cyc_Tcenv_resolve_namespace(te,
loc,first,rest);{struct Cyc_List_List*_tmp404;ge->availables=(struct Cyc_List_List*)((
_tmp404=_cycalloc(sizeof(*_tmp404)),((_tmp404->hd=_tmp27E,((_tmp404->tl=ge->availables,
_tmp404))))));}Cyc_Tc_tc_decls(te,_tmp25B,in_externC,in_externCinclude,
check_var_init,grgn,exports);ge->availables=((struct Cyc_List_List*)_check_null(
ge->availables))->tl;goto _LL116;};}_LL133: {struct Cyc_Absyn_ExternC_d_struct*
_tmp25C=(struct Cyc_Absyn_ExternC_d_struct*)_tmp240;if(_tmp25C->tag != 12)goto
_LL135;else{_tmp25D=_tmp25C->f1;}}_LL134: Cyc_Tc_tc_decls(te,_tmp25D,1,
in_externCinclude,check_var_init,grgn,exports);goto _LL116;_LL135: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp25E=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp240;if(_tmp25E->tag != 13)
goto _LL116;else{_tmp25F=_tmp25E->f1;_tmp260=_tmp25E->f2;}}_LL136:((void(*)(void(*
f)(struct Cyc_Tcenv_Tenv*,struct _tuple11*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp260);{struct Cyc_List_List*
newexs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp260,(unsigned int)exports?*exports: 0);Cyc_Tc_tc_decls(te,
_tmp25F,1,1,check_var_init,grgn,(struct Cyc_List_List**)& newexs);for(0;_tmp260 != 
0;_tmp260=_tmp260->tl){struct _tuple11*_tmp280=(struct _tuple11*)_tmp260->hd;if(!(*
_tmp280).f3){const char*_tmp408;void*_tmp407[1];struct Cyc_String_pa_struct _tmp406;(
_tmp406.tag=0,((_tmp406.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((*
_tmp280).f2)),((_tmp407[0]=& _tmp406,Cyc_Tcutil_warn((*_tmp280).f1,((_tmp408="%s is exported but not defined",
_tag_dyneither(_tmp408,sizeof(char),31))),_tag_dyneither(_tmp407,sizeof(void*),1)))))));}}
goto _LL116;};_LL116:;}}void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*
te,int check_var_init,struct Cyc_List_List*ds){Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct
_RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};static int Cyc_Tc_vardecl_needed(
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);static struct Cyc_List_List*
Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){return((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,
env,ds);}struct _tuple17{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};static int
Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){void*
_tmp284=d->r;struct Cyc_Absyn_Vardecl*_tmp286;struct Cyc_List_List*_tmp288;struct
Cyc_List_List**_tmp289;struct Cyc_List_List*_tmp28B;struct Cyc_List_List**_tmp28C;
struct Cyc_List_List*_tmp28E;struct Cyc_List_List**_tmp28F;struct Cyc_List_List*
_tmp291;struct Cyc_List_List**_tmp292;_LL147: {struct Cyc_Absyn_Var_d_struct*
_tmp285=(struct Cyc_Absyn_Var_d_struct*)_tmp284;if(_tmp285->tag != 0)goto _LL149;
else{_tmp286=_tmp285->f1;}}_LL148: if((env->in_cinclude  || _tmp286->sc != Cyc_Absyn_Extern
 && _tmp286->sc != Cyc_Absyn_ExternC) || !Cyc_Absyn_qvar_cmp(_tmp286->name,Cyc_Absyn_uniquergn_qvar))
return 1;{struct _tuple0 _tmp294;union Cyc_Absyn_Nmspace _tmp295;struct _dyneither_ptr*
_tmp296;struct _tuple0*_tmp293=_tmp286->name;_tmp294=*_tmp293;_tmp295=_tmp294.f1;
_tmp296=_tmp294.f2;{struct Cyc_List_List*ns;{union Cyc_Absyn_Nmspace _tmp297=
_tmp295;int _tmp298;struct Cyc_List_List*_tmp299;struct Cyc_List_List*_tmp29A;
struct Cyc_List_List*_tmp29B;_LL154: if((_tmp297.Loc_n).tag != 4)goto _LL156;_tmp298=(
int)(_tmp297.Loc_n).val;_LL155: ns=0;goto _LL153;_LL156: if((_tmp297.Rel_n).tag != 1)
goto _LL158;_tmp299=(struct Cyc_List_List*)(_tmp297.Rel_n).val;_LL157: ns=_tmp299;
goto _LL153;_LL158: if((_tmp297.C_n).tag != 3)goto _LL15A;_tmp29A=(struct Cyc_List_List*)(
_tmp297.C_n).val;_LL159: ns=_tmp29A;goto _LL153;_LL15A: if((_tmp297.Abs_n).tag != 2)
goto _LL153;_tmp29B=(struct Cyc_List_List*)(_tmp297.Abs_n).val;_LL15B: ns=_tmp29B;
goto _LL153;_LL153:;}{struct _tuple17*_tmp29C=((struct _tuple17*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);struct Cyc_Tcenv_Genv*
_tmp29D=(*_tmp29C).f1;int _tmp29E=(*((struct _tuple12*(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp29D->ordinaries,_tmp296)).f2;if(!
_tmp29E)(*_tmp29C).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp29C).f2,_tmp296);
return _tmp29E;};};};_LL149: {struct Cyc_Absyn_ExternC_d_struct*_tmp287=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp284;if(_tmp287->tag != 12)goto _LL14B;else{_tmp288=_tmp287->f1;_tmp289=(struct
Cyc_List_List**)& _tmp287->f1;}}_LL14A: _tmp28C=_tmp289;goto _LL14C;_LL14B: {struct
Cyc_Absyn_Using_d_struct*_tmp28A=(struct Cyc_Absyn_Using_d_struct*)_tmp284;if(
_tmp28A->tag != 11)goto _LL14D;else{_tmp28B=_tmp28A->f2;_tmp28C=(struct Cyc_List_List**)&
_tmp28A->f2;}}_LL14C: _tmp28F=_tmp28C;goto _LL14E;_LL14D: {struct Cyc_Absyn_Namespace_d_struct*
_tmp28D=(struct Cyc_Absyn_Namespace_d_struct*)_tmp284;if(_tmp28D->tag != 10)goto
_LL14F;else{_tmp28E=_tmp28D->f2;_tmp28F=(struct Cyc_List_List**)& _tmp28D->f2;}}
_LL14E:*_tmp28F=Cyc_Tc_treeshake_f(env,*_tmp28F);return 1;_LL14F: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp290=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp284;if(_tmp290->tag != 13)
goto _LL151;else{_tmp291=_tmp290->f1;_tmp292=(struct Cyc_List_List**)& _tmp290->f1;}}
_LL150: {int in_cinclude=env->in_cinclude;env->in_cinclude=1;*_tmp292=Cyc_Tc_treeshake_f(
env,*_tmp292);env->in_cinclude=in_cinclude;return 1;}_LL151:;_LL152: return 1;
_LL146:;}static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct
_dyneither_ptr*x,void*y){return !((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(set,x);}static struct _tuple17*Cyc_Tc_treeshake_make_env_f(
struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){struct _tuple17*_tmp409;return(
_tmp409=_region_malloc(rgn,sizeof(*_tmp409)),((_tmp409->f1=ge,((_tmp409->f2=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp409)))));}struct
_tuple18{struct Cyc_List_List*f1;struct _tuple17*f2;};struct Cyc_List_List*Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){struct _RegionHandle _tmp2A0=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp2A0;_push_region(rgn);{struct Cyc_Tc_TreeshakeEnv
_tmp40A;struct Cyc_Tc_TreeshakeEnv _tmp2A1=(_tmp40A.rgn=rgn,((_tmp40A.in_cinclude=
0,((_tmp40A.nsdict=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple17*(*
f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict
d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp40A)))));
struct Cyc_List_List*_tmp2A2=Cyc_Tc_treeshake_f(& _tmp2A1,ds);if(((int(*)(struct
Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp2A1.nsdict)){struct Cyc_List_List*_tmp2A3=
_tmp2A2;_npop_handler(0);return _tmp2A3;}{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,
_tmp2A1.nsdict);struct _tuple18 _tmp2A4=*((struct _tuple18*(*)(struct _RegionHandle*,
struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp2A1.nsdict);while(((int(*)(
struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& _tmp2A4)){struct
_tuple17*_tmp2A6;struct _tuple17 _tmp2A7;struct Cyc_Tcenv_Genv*_tmp2A8;struct Cyc_Set_Set*
_tmp2A9;struct _tuple18 _tmp2A5=_tmp2A4;_tmp2A6=_tmp2A5.f2;_tmp2A7=*_tmp2A6;
_tmp2A8=_tmp2A7.f1;_tmp2A9=_tmp2A7.f2;_tmp2A8->ordinaries=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct
_tuple12*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(
_tmp2A8->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct
_tuple12*y))Cyc_Tc_treeshake_remove_f,_tmp2A9,_tmp2A8->ordinaries);}{struct Cyc_List_List*
_tmp2AA=_tmp2A2;_npop_handler(0);return _tmp2AA;};};};_pop_region(rgn);}
