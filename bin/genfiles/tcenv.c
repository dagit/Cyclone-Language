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
_dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
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
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < 
dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_rempty(struct
_RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(
struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,
void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,void*);struct Cyc_Set_Set;
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rempty(
struct _RegionHandle*r,int(*cmp)(void*,void*));int Cyc_Set_member(struct Cyc_Set_Set*
s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};
struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[8];struct Cyc_SlowDict_Present_struct{
char*tag;};extern char Cyc_SlowDict_Absent[7];struct Cyc_SlowDict_Absent_struct{
char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;};
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};int Cyc_strptrcmp(struct
_dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};
struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
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
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_TypeDeclType_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;
};extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct
Cyc_Absyn_UniqueRgn_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{int tag;
struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft
 = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_struct{
int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};struct Cyc_Absyn_Fastcall_att_struct{
int tag;};struct Cyc_Absyn_Noreturn_att_struct{int tag;};struct Cyc_Absyn_Const_att_struct{
int tag;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{
int tag;};struct Cyc_Absyn_Section_att_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_Nocommon_att_struct{int tag;};struct Cyc_Absyn_Shared_att_struct{
int tag;};struct Cyc_Absyn_Unused_att_struct{int tag;};struct Cyc_Absyn_Weak_att_struct{
int tag;};struct Cyc_Absyn_Dllimport_att_struct{int tag;};struct Cyc_Absyn_Dllexport_att_struct{
int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{int tag;};struct Cyc_Absyn_Constructor_att_struct{
int tag;};struct Cyc_Absyn_Destructor_att_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{
int tag;};struct Cyc_Absyn_Format_att_struct{int tag;enum Cyc_Absyn_Format_Type f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_struct{
int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{int tag;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple3 val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6
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
char*tag;};int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);extern
union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct Cyc_RgnOrder_RgnPO;
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
int tag;struct Cyc_Absyn_Stmt*f1;};extern struct Cyc_Tcenv_NotLoop_j_struct Cyc_Tcenv_NotLoop_j_val;
extern struct Cyc_Tcenv_CaseEnd_j_struct Cyc_Tcenv_CaseEnd_j_val;extern struct Cyc_Tcenv_FnEnd_j_struct
Cyc_Tcenv_FnEnd_j_val;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict
ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcenv_env_err(struct
_dyneither_ptr msg);struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(struct _RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct
Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Fenv*
old_fenv,struct Cyc_Absyn_Fndecl*new_fn);struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _dyneither_ptr*,struct
Cyc_List_List*);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple0*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple0*);struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple0*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple0*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);
struct _tuple9{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const
struct Cyc_Tcenv_CList*f3;};const struct _tuple9*Cyc_Tcenv_process_fallthru(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);struct
Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct
_dyneither_ptr*,struct Cyc_Absyn_Stmt*);int Cyc_Tcenv_all_labels_resolved(struct
Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*
name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc);struct _tuple10{struct Cyc_Absyn_Tvar*f1;void*f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple10**
oldtv,struct Cyc_Position_Segment*loc);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,void*r,int resetable,int opened);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);struct
Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*
def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);struct _tuple10 Cyc_Tcutil_swap_kind(
void*t,void*kb);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*
e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);char Cyc_Tcenv_Env_error[
10]="Env_error";struct Cyc_Tcenv_Env_error_struct{char*tag;};struct Cyc_Tcenv_Env_error_struct
Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};void*Cyc_Tcenv_env_err(struct
_dyneither_ptr msg){{const char*_tmp326;void*_tmp325[1];struct Cyc_String_pa_struct
_tmp324;(_tmp324.tag=0,((_tmp324.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
msg),((_tmp325[0]=& _tmp324,Cyc_fprintf(Cyc_stderr,((_tmp326="Internal error in tcenv: %s\n",
_tag_dyneither(_tmp326,sizeof(char),29))),_tag_dyneither(_tmp325,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}
struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;struct Cyc_Tcenv_NotLoop_j_struct Cyc_Tcenv_NotLoop_j_val={
0};struct Cyc_Tcenv_CaseEnd_j_struct Cyc_Tcenv_CaseEnd_j_val={1};struct Cyc_Tcenv_FnEnd_j_struct
Cyc_Tcenv_FnEnd_j_val={2};struct Cyc_Tcenv_CtrlEnv{struct _RegionHandle*ctrl_rgn;
void*continue_stmt;void*break_stmt;const struct _tuple9*fallthru_clause;void*
next_stmt;int try_depth;};struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;
void*return_typ;struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict needed_labels;
struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*
delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*
b;const struct Cyc_Tcenv_Bindings*tl;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*
shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const
struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*
ctrl_env;void*capability;void*curr_rgn;int in_notreadctxt;struct _RegionHandle*
fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_struct{
char*tag;};struct Cyc_Tcenv_NoBinding_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct
_dyneither_ptr*v){for(0;(unsigned int)bs;bs=bs->tl){if(Cyc_strptrcmp(v,bs->v)== 
0)return bs->b;}(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(struct _RegionHandle*r){struct Cyc_Tcenv_Genv*_tmp327;return(
_tmp327=_region_malloc(r,sizeof(*_tmp327)),((_tmp327->grgn=r,((_tmp327->namespaces=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp327->aggrdecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp327->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp327->enumdecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp327->typedefs=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp327->ordinaries=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp327->availables=
0,_tmp327)))))))))))))))));}struct _tuple11{void*f1;int f2;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*r){{struct Cyc_Core_Opt*_tmp328;Cyc_Tcutil_empty_var_set=((
_tmp328=_cycalloc(sizeof(*_tmp328)),((_tmp328->v=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp328))));}{struct Cyc_Tcenv_Genv*_tmpA=Cyc_Tcenv_empty_genv(r);{struct Cyc_Absyn_Datatypedecl**
_tmp329;_tmpA->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmpA->datatypedecls,(*
Cyc_Absyn_exn_name).f2,((_tmp329=_cycalloc(sizeof(*_tmp329)),((_tmp329[0]=Cyc_Absyn_exn_tud,
_tmp329)))));}{struct Cyc_List_List*_tmpC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmpC != 0;_tmpC=_tmpC->tl){
struct Cyc_Tcenv_DatatypeRes_struct*_tmp32F;struct Cyc_Tcenv_DatatypeRes_struct
_tmp32E;struct _tuple11*_tmp32D;_tmpA->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple11*v))Cyc_Dict_insert)(_tmpA->ordinaries,(*((
struct Cyc_Absyn_Datatypefield*)_tmpC->hd)->name).f2,((_tmp32D=_region_malloc(r,
sizeof(*_tmp32D)),((_tmp32D->f1=(void*)((_tmp32F=_cycalloc(sizeof(*_tmp32F)),((
_tmp32F[0]=((_tmp32E.tag=2,((_tmp32E.f1=Cyc_Absyn_exn_tud,((_tmp32E.f2=(struct
Cyc_Absyn_Datatypefield*)_tmpC->hd,_tmp32E)))))),_tmp32F)))),((_tmp32D->f2=1,
_tmp32D)))))));}}{struct Cyc_Dict_Dict ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_varlist_cmp),0,_tmpA);struct Cyc_Tcenv_Tenv*_tmp330;return(_tmp330=
_region_malloc(r,sizeof(*_tmp330)),((_tmp330->ns=0,((_tmp330->ae=ae,((_tmp330->le=
0,((_tmp330->allow_valueof=0,_tmp330)))))))));};};}static struct Cyc_Tcenv_Genv*
Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){return((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(
te,loc,n,ns));}static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*
ns){if(ns == 0){const char*_tmp331;return((struct Cyc_List_List*(*)(struct
_dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp331="outer_namespace",_tag_dyneither(
_tmp331,sizeof(char),16))));}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2){if(n1 == n2)return 1;for(0;n1 != 0;
n1=n1->tl){if(n2 == 0)return 0;if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(
struct _dyneither_ptr*)n2->hd)!= 0)return 0;n2=n2->tl;}if(n2 != 0)return 0;return 1;}
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct
_dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){for(0;nss != 0;
nss=nss->tl){if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);struct
_handler_cons _tmp12;_push_handler(& _tmp12);{int _tmp14=0;if(setjmp(_tmp12.handler))
_tmp14=1;if(!_tmp14){lookup(ge2,v);{const char*_tmp335;void*_tmp334[1];struct Cyc_String_pa_struct
_tmp333;(_tmp333.tag=0,((_tmp333.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
v),((_tmp334[0]=& _tmp333,Cyc_Tcutil_terr(loc,((_tmp335="%s is ambiguous",
_tag_dyneither(_tmp335,sizeof(char),16))),_tag_dyneither(_tmp334,sizeof(void*),1)))))));};;
_pop_handler();}else{void*_tmp13=(void*)_exn_thrown;void*_tmp19=_tmp13;_LL1: {
struct Cyc_Dict_Absent_struct*_tmp1A=(struct Cyc_Dict_Absent_struct*)_tmp19;if(
_tmp1A->tag != Cyc_Dict_Absent)goto _LL3;}_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(
_tmp19);_LL0:;}};}}return;}static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v){struct Cyc_List_List*cns=te->ns;while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);{struct _handler_cons _tmp1B;
_push_handler(& _tmp1B);{int _tmp1D=0;if(setjmp(_tmp1B.handler))_tmp1D=1;if(!
_tmp1D){{void*result=lookup(ge,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1E=result;_npop_handler(0);return _tmp1E;};};_pop_handler();}else{void*
_tmp1C=(void*)_exn_thrown;void*_tmp20=_tmp1C;_LL6: {struct Cyc_Dict_Absent_struct*
_tmp21=(struct Cyc_Dict_Absent_struct*)_tmp20;if(_tmp21->tag != Cyc_Dict_Absent)
goto _LL8;}_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp20);_LL5:;}};}{struct Cyc_List_List*
_tmp22=ge->availables;for(0;_tmp22 != 0;_tmp22=_tmp22->tl){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)_tmp22->hd);struct _handler_cons _tmp23;_push_handler(&
_tmp23);{int _tmp25=0;if(setjmp(_tmp23.handler))_tmp25=1;if(!_tmp25){{void*result=
lookup(ge2,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)
_tmp22->hd,_tmp22->tl);{void*_tmp26=result;_npop_handler(0);return _tmp26;};};
_pop_handler();}else{void*_tmp24=(void*)_exn_thrown;void*_tmp28=_tmp24;_LLB: {
struct Cyc_Dict_Absent_struct*_tmp29=(struct Cyc_Dict_Absent_struct*)_tmp28;if(
_tmp29->tag != Cyc_Dict_Absent)goto _LLD;}_LLC: goto _LLA;_LLD:;_LLE:(void)_throw(
_tmp28);_LLA:;}};}}if(cns == 0){struct Cyc_Dict_Absent_struct _tmp338;struct Cyc_Dict_Absent_struct*
_tmp337;(int)_throw((void*)((_tmp337=_cycalloc_atomic(sizeof(*_tmp337)),((
_tmp337[0]=((_tmp338.tag=Cyc_Dict_Absent,_tmp338)),_tmp337)))));}cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp2C=ge->ordinaries;struct _tuple11*
ans=((struct _tuple11*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp2C,v);(*ans).f2=1;return(*ans).f1;}static void*Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct
_tuple0 _tmp2E;union Cyc_Absyn_Nmspace _tmp2F;struct _dyneither_ptr*_tmp30;struct
_tuple0*_tmp2D=q;_tmp2E=*_tmp2D;_tmp2F=_tmp2E.f1;_tmp30=_tmp2E.f2;{union Cyc_Absyn_Nmspace
_tmp31=_tmp2F;int _tmp32;struct Cyc_List_List*_tmp33;struct Cyc_List_List*_tmp34;
struct Cyc_List_List _tmp35;struct _dyneither_ptr*_tmp36;struct Cyc_List_List*_tmp37;
struct Cyc_List_List*_tmp38;struct Cyc_List_List*_tmp39;_LL10: if((_tmp31.Loc_n).tag
!= 4)goto _LL12;_tmp32=(int)(_tmp31.Loc_n).val;_LL11: goto _LL13;_LL12: if((_tmp31.Rel_n).tag
!= 1)goto _LL14;_tmp33=(struct Cyc_List_List*)(_tmp31.Rel_n).val;if(_tmp33 != 0)
goto _LL14;_LL13: return((void*(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct
_dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,
_tmp30);_LL14: if((_tmp31.Rel_n).tag != 1)goto _LL16;_tmp34=(struct Cyc_List_List*)(
_tmp31.Rel_n).val;if(_tmp34 == 0)goto _LL16;_tmp35=*_tmp34;_tmp36=(struct
_dyneither_ptr*)_tmp35.hd;_tmp37=_tmp35.tl;_LL15: {struct Cyc_Tcenv_Genv*_tmp3A=
Cyc_Tcenv_lookup_namespace(te,loc,_tmp36,_tmp37);return Cyc_Tcenv_lookup_ordinary_global_f(
_tmp3A,_tmp30);}_LL16: if((_tmp31.C_n).tag != 3)goto _LL18;_tmp38=(struct Cyc_List_List*)(
_tmp31.C_n).val;_LL17: _tmp39=_tmp38;goto _LL19;_LL18: if((_tmp31.Abs_n).tag != 2)
goto _LLF;_tmp39=(struct Cyc_List_List*)(_tmp31.Abs_n).val;_LL19: return Cyc_Tcenv_lookup_ordinary_global_f(((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp39),_tmp30);_LLF:;};}struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,
struct Cyc_List_List*ns){struct Cyc_List_List*_tmp3B=te->ns;struct _RegionHandle*
_tmp3C=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp3D=0;while(1){struct Cyc_Tcenv_Genv*
_tmp3E=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp3B);struct Cyc_List_List*_tmp3F=_tmp3E->availables;
struct Cyc_Set_Set*_tmp40=_tmp3E->namespaces;{struct Cyc_List_List*_tmp41=_tmp3F;
for(0;_tmp41 != 0;_tmp41=_tmp41->tl){struct Cyc_Set_Set*_tmp42=(((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)
_tmp41->hd))->namespaces;if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_member)(_tmp42,n)){struct Cyc_List_List*_tmp33B;struct Cyc_List_List*
_tmp33A;_tmp3D=((_tmp33A=_region_malloc(_tmp3C,sizeof(*_tmp33A)),((_tmp33A->hd=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp3B,((_tmp33B=_cycalloc(sizeof(*_tmp33B)),((_tmp33B->hd=n,((_tmp33B->tl=ns,
_tmp33B))))))),((_tmp33A->tl=_tmp3D,_tmp33A))))));}}}if(((int(*)(struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp40,n)){struct Cyc_List_List*
_tmp33E;struct Cyc_List_List*_tmp33D;_tmp3D=((_tmp33D=_region_malloc(_tmp3C,
sizeof(*_tmp33D)),((_tmp33D->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmp3B,((_tmp33E=_cycalloc(sizeof(*
_tmp33E)),((_tmp33E->hd=n,((_tmp33E->tl=ns,_tmp33E))))))),((_tmp33D->tl=_tmp3D,
_tmp33D))))));}if(_tmp3D != 0){if(_tmp3D->tl != 0){const char*_tmp342;void*_tmp341[
1];struct Cyc_String_pa_struct _tmp340;(_tmp340.tag=0,((_tmp340.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp341[0]=& _tmp340,Cyc_Tcutil_terr(
loc,((_tmp342="%s is ambiguous",_tag_dyneither(_tmp342,sizeof(char),16))),
_tag_dyneither(_tmp341,sizeof(void*),1)))))));}return(struct Cyc_List_List*)
_tmp3D->hd;}if(_tmp3B == 0){struct Cyc_Dict_Absent_struct _tmp345;struct Cyc_Dict_Absent_struct*
_tmp344;(int)_throw((void*)((_tmp344=_cycalloc_atomic(sizeof(*_tmp344)),((
_tmp344[0]=((_tmp345.tag=Cyc_Dict_Absent,_tmp345)),_tmp344)))));}_tmp3B=Cyc_Tcenv_outer_namespace(
_tmp3B);}}static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*
ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp4C=ge->aggrdecls;return((
struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp4C,v);}struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0 _tmp4E;union Cyc_Absyn_Nmspace
_tmp4F;struct _dyneither_ptr*_tmp50;struct _tuple0*_tmp4D=q;_tmp4E=*_tmp4D;_tmp4F=
_tmp4E.f1;_tmp50=_tmp4E.f2;{union Cyc_Absyn_Nmspace _tmp51=_tmp4F;int _tmp52;struct
Cyc_List_List*_tmp53;struct Cyc_List_List*_tmp54;struct Cyc_List_List*_tmp55;
struct Cyc_List_List*_tmp56;struct Cyc_List_List _tmp57;struct _dyneither_ptr*_tmp58;
struct Cyc_List_List*_tmp59;_LL1B: if((_tmp51.Loc_n).tag != 4)goto _LL1D;_tmp52=(int)(
_tmp51.Loc_n).val;_LL1C: goto _LL1E;_LL1D: if((_tmp51.Rel_n).tag != 1)goto _LL1F;
_tmp53=(struct Cyc_List_List*)(_tmp51.Rel_n).val;if(_tmp53 != 0)goto _LL1F;_LL1E:
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,
_tmp50);_LL1F: if((_tmp51.C_n).tag != 3)goto _LL21;_tmp54=(struct Cyc_List_List*)(
_tmp51.C_n).val;_LL20: _tmp55=_tmp54;goto _LL22;_LL21: if((_tmp51.Abs_n).tag != 2)
goto _LL23;_tmp55=(struct Cyc_List_List*)(_tmp51.Abs_n).val;_LL22: {struct Cyc_Dict_Dict
_tmp5A=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp55))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5A,_tmp50);}
_LL23: if((_tmp51.Rel_n).tag != 1)goto _LL1A;_tmp56=(struct Cyc_List_List*)(_tmp51.Rel_n).val;
if(_tmp56 == 0)goto _LL1A;_tmp57=*_tmp56;_tmp58=(struct _dyneither_ptr*)_tmp57.hd;
_tmp59=_tmp57.tl;_LL24: {struct Cyc_Dict_Dict _tmp5B=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp58,_tmp59))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5B,_tmp50);}_LL1A:;};}
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*
ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp5C=ge->datatypedecls;return((
struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup)(_tmp5C,v);}struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct
_tuple0 _tmp5E;union Cyc_Absyn_Nmspace _tmp5F;struct _dyneither_ptr*_tmp60;struct
_tuple0*_tmp5D=q;_tmp5E=*_tmp5D;_tmp5F=_tmp5E.f1;_tmp60=_tmp5E.f2;{union Cyc_Absyn_Nmspace
_tmp61=_tmp5F;int _tmp62;struct Cyc_List_List*_tmp63;struct Cyc_List_List*_tmp64;
struct Cyc_List_List _tmp65;struct _dyneither_ptr*_tmp66;struct Cyc_List_List*_tmp67;
struct Cyc_List_List*_tmp68;struct Cyc_List_List*_tmp69;_LL26: if((_tmp61.Loc_n).tag
!= 4)goto _LL28;_tmp62=(int)(_tmp61.Loc_n).val;_LL27: goto _LL29;_LL28: if((_tmp61.Rel_n).tag
!= 1)goto _LL2A;_tmp63=(struct Cyc_List_List*)(_tmp61.Rel_n).val;if(_tmp63 != 0)
goto _LL2A;_LL29: return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct
Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp60);_LL2A: if((_tmp61.Rel_n).tag != 1)
goto _LL2C;_tmp64=(struct Cyc_List_List*)(_tmp61.Rel_n).val;if(_tmp64 == 0)goto
_LL2C;_tmp65=*_tmp64;_tmp66=(struct _dyneither_ptr*)_tmp65.hd;_tmp67=_tmp65.tl;
_LL2B: {struct Cyc_Dict_Dict _tmp6A=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp66,
_tmp67))->datatypedecls;return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp6A,_tmp60);}_LL2C: if((_tmp61.C_n).tag
!= 3)goto _LL2E;_tmp68=(struct Cyc_List_List*)(_tmp61.C_n).val;_LL2D: _tmp69=_tmp68;
goto _LL2F;_LL2E: if((_tmp61.Abs_n).tag != 2)goto _LL25;_tmp69=(struct Cyc_List_List*)(
_tmp61.Abs_n).val;_LL2F: {struct Cyc_Dict_Dict _tmp6B=(((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp69))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp6B,_tmp60);}_LL25:;};}static struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}struct Cyc_Absyn_Datatypedecl***
Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0 _tmp6D;union Cyc_Absyn_Nmspace
_tmp6E;struct _dyneither_ptr*_tmp6F;struct _tuple0*_tmp6C=q;_tmp6D=*_tmp6C;_tmp6E=
_tmp6D.f1;_tmp6F=_tmp6D.f2;{union Cyc_Absyn_Nmspace _tmp70=_tmp6E;struct Cyc_List_List*
_tmp71;int _tmp72;struct Cyc_List_List*_tmp73;struct Cyc_List_List _tmp74;struct
_dyneither_ptr*_tmp75;struct Cyc_List_List*_tmp76;struct Cyc_List_List*_tmp77;
struct Cyc_List_List*_tmp78;_LL31: if((_tmp70.Rel_n).tag != 1)goto _LL33;_tmp71=(
struct Cyc_List_List*)(_tmp70.Rel_n).val;if(_tmp71 != 0)goto _LL33;_LL32: {struct
_handler_cons _tmp79;_push_handler(& _tmp79);{int _tmp7B=0;if(setjmp(_tmp79.handler))
_tmp7B=1;if(!_tmp7B){{struct Cyc_Absyn_Datatypedecl***_tmp346;struct Cyc_Absyn_Datatypedecl***
_tmp7D=(_tmp346=_region_malloc(r,sizeof(*_tmp346)),((_tmp346[0]=((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Datatypedecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp6F),_tmp346)));_npop_handler(0);
return _tmp7D;};_pop_handler();}else{void*_tmp7A=(void*)_exn_thrown;void*_tmp7F=
_tmp7A;_LL3C: {struct Cyc_Dict_Absent_struct*_tmp80=(struct Cyc_Dict_Absent_struct*)
_tmp7F;if(_tmp80->tag != Cyc_Dict_Absent)goto _LL3E;}_LL3D: return 0;_LL3E:;_LL3F:(
void)_throw(_tmp7F);_LL3B:;}};}_LL33: if((_tmp70.Loc_n).tag != 4)goto _LL35;_tmp72=(
int)(_tmp70.Loc_n).val;_LL34:{const char*_tmp349;void*_tmp348;(_tmp348=0,Cyc_Tcutil_terr(
loc,((_tmp349="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp349,sizeof(
char),33))),_tag_dyneither(_tmp348,sizeof(void*),0)));}return 0;_LL35: if((_tmp70.Rel_n).tag
!= 1)goto _LL37;_tmp73=(struct Cyc_List_List*)(_tmp70.Rel_n).val;if(_tmp73 == 0)
goto _LL37;_tmp74=*_tmp73;_tmp75=(struct _dyneither_ptr*)_tmp74.hd;_tmp76=_tmp74.tl;
_LL36: {struct Cyc_Tcenv_Genv*ge;{struct _handler_cons _tmp83;_push_handler(& _tmp83);{
int _tmp85=0;if(setjmp(_tmp83.handler))_tmp85=1;if(!_tmp85){ge=Cyc_Tcenv_lookup_namespace(
te,loc,_tmp75,_tmp76);;_pop_handler();}else{void*_tmp84=(void*)_exn_thrown;void*
_tmp87=_tmp84;_LL41: {struct Cyc_Dict_Absent_struct*_tmp88=(struct Cyc_Dict_Absent_struct*)
_tmp87;if(_tmp88->tag != Cyc_Dict_Absent)goto _LL43;}_LL42:{const char*_tmp34C;void*
_tmp34B;(_tmp34B=0,Cyc_Tcutil_terr(loc,((_tmp34C="bad qualified name for @extensible datatype",
_tag_dyneither(_tmp34C,sizeof(char),44))),_tag_dyneither(_tmp34B,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_struct _tmp34F;struct Cyc_Dict_Absent_struct*_tmp34E;(int)
_throw((void*)((_tmp34E=_cycalloc_atomic(sizeof(*_tmp34E)),((_tmp34E[0]=((
_tmp34F.tag=Cyc_Dict_Absent,_tmp34F)),_tmp34E)))));};_LL43:;_LL44:(void)_throw(
_tmp87);_LL40:;}};}{struct Cyc_Dict_Dict _tmp8D=ge->datatypedecls;struct Cyc_Absyn_Datatypedecl***
_tmp350;return(_tmp350=_region_malloc(r,sizeof(*_tmp350)),((_tmp350[0]=((struct
Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp8D,_tmp6F),_tmp350)));};}_LL37: if((_tmp70.C_n).tag != 3)goto _LL39;_tmp77=(
struct Cyc_List_List*)(_tmp70.C_n).val;_LL38: _tmp78=_tmp77;goto _LL3A;_LL39: if((
_tmp70.Abs_n).tag != 2)goto _LL30;_tmp78=(struct Cyc_List_List*)(_tmp70.Abs_n).val;
_LL3A: {struct Cyc_Dict_Dict _tmp8F=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp78))->datatypedecls;struct
Cyc_Absyn_Datatypedecl***_tmp351;return(_tmp351=_region_malloc(r,sizeof(*_tmp351)),((
_tmp351[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8F,_tmp6F),_tmp351)));}_LL30:;};}static
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,
struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp91=ge->enumdecls;return((struct
Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp91,v);}struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0 _tmp93;union Cyc_Absyn_Nmspace
_tmp94;struct _dyneither_ptr*_tmp95;struct _tuple0*_tmp92=q;_tmp93=*_tmp92;_tmp94=
_tmp93.f1;_tmp95=_tmp93.f2;{union Cyc_Absyn_Nmspace _tmp96=_tmp94;int _tmp97;struct
Cyc_List_List*_tmp98;struct Cyc_List_List*_tmp99;struct Cyc_List_List _tmp9A;struct
_dyneither_ptr*_tmp9B;struct Cyc_List_List*_tmp9C;struct Cyc_List_List*_tmp9D;
struct Cyc_List_List*_tmp9E;_LL46: if((_tmp96.Loc_n).tag != 4)goto _LL48;_tmp97=(int)(
_tmp96.Loc_n).val;_LL47: goto _LL49;_LL48: if((_tmp96.Rel_n).tag != 1)goto _LL4A;
_tmp98=(struct Cyc_List_List*)(_tmp96.Rel_n).val;if(_tmp98 != 0)goto _LL4A;_LL49:
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,
_tmp95);_LL4A: if((_tmp96.Rel_n).tag != 1)goto _LL4C;_tmp99=(struct Cyc_List_List*)(
_tmp96.Rel_n).val;if(_tmp99 == 0)goto _LL4C;_tmp9A=*_tmp99;_tmp9B=(struct
_dyneither_ptr*)_tmp9A.hd;_tmp9C=_tmp9A.tl;_LL4B: {struct Cyc_Dict_Dict _tmp9F=(
Cyc_Tcenv_lookup_namespace(te,loc,_tmp9B,_tmp9C))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp9F,_tmp95);}
_LL4C: if((_tmp96.C_n).tag != 3)goto _LL4E;_tmp9D=(struct Cyc_List_List*)(_tmp96.C_n).val;
_LL4D: _tmp9E=_tmp9D;goto _LL4F;_LL4E: if((_tmp96.Abs_n).tag != 2)goto _LL45;_tmp9E=(
struct Cyc_List_List*)(_tmp96.Abs_n).val;_LL4F: {struct Cyc_Dict_Dict _tmpA0=(((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp9E))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmpA0,_tmp95);}_LL45:;};}static
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*
ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmpA1=ge->typedefs;return((struct
Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmpA1,v);}struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0 _tmpA3;union Cyc_Absyn_Nmspace
_tmpA4;struct _dyneither_ptr*_tmpA5;struct _tuple0*_tmpA2=q;_tmpA3=*_tmpA2;_tmpA4=
_tmpA3.f1;_tmpA5=_tmpA3.f2;{union Cyc_Absyn_Nmspace _tmpA6=_tmpA4;int _tmpA7;struct
Cyc_List_List*_tmpA8;struct Cyc_List_List*_tmpA9;struct Cyc_List_List _tmpAA;struct
_dyneither_ptr*_tmpAB;struct Cyc_List_List*_tmpAC;struct Cyc_List_List*_tmpAD;
struct Cyc_List_List*_tmpAE;_LL51: if((_tmpA6.Loc_n).tag != 4)goto _LL53;_tmpA7=(int)(
_tmpA6.Loc_n).val;_LL52: goto _LL54;_LL53: if((_tmpA6.Rel_n).tag != 1)goto _LL55;
_tmpA8=(struct Cyc_List_List*)(_tmpA6.Rel_n).val;if(_tmpA8 != 0)goto _LL55;_LL54:
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,
_tmpA5);_LL55: if((_tmpA6.Rel_n).tag != 1)goto _LL57;_tmpA9=(struct Cyc_List_List*)(
_tmpA6.Rel_n).val;if(_tmpA9 == 0)goto _LL57;_tmpAA=*_tmpA9;_tmpAB=(struct
_dyneither_ptr*)_tmpAA.hd;_tmpAC=_tmpAA.tl;_LL56: {struct Cyc_Dict_Dict _tmpAF=(
Cyc_Tcenv_lookup_namespace(te,loc,_tmpAB,_tmpAC))->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmpAF,_tmpA5);}
_LL57: if((_tmpA6.C_n).tag != 3)goto _LL59;_tmpAD=(struct Cyc_List_List*)(_tmpA6.C_n).val;
_LL58: _tmpAE=_tmpAD;goto _LL5A;_LL59: if((_tmpA6.Abs_n).tag != 2)goto _LL50;_tmpAE=(
struct Cyc_List_List*)(_tmpA6.Abs_n).val;_LL5A: {struct Cyc_Dict_Dict _tmpB0=(((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmpAE))->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmpB0,_tmpA5);}_LL50:;};}static
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct
_dyneither_ptr err_msg){struct Cyc_Tcenv_Fenv*_tmpB1=te->le;if((struct Cyc_Tcenv_Tenv*)
te == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);return(
struct Cyc_Tcenv_Fenv*)_check_null(_tmpB1);}struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return(struct _RegionHandle*)Cyc_Core_heap_region;}struct _RegionHandle*Cyc_Tcenv_get_fnrgn(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpB2=te->le;if(_tmpB2 != 0){
struct Cyc_Tcenv_Fenv _tmpB4;struct _RegionHandle*_tmpB5;struct Cyc_Tcenv_Fenv*
_tmpB3=(struct Cyc_Tcenv_Fenv*)_tmpB2;_tmpB4=*_tmpB3;_tmpB5=_tmpB4.fnrgn;return
_tmpB5;}return Cyc_Tcenv_coerce_heap_region();}static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(
struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){if(te->le
== 0){const char*_tmp352;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((
_tmp352="put_fenv",_tag_dyneither(_tmp352,sizeof(char),9))));}{struct Cyc_Tcenv_Tenv*
_tmp353;return(_tmp353=_region_malloc(l,sizeof(*_tmp353)),((_tmp353->ns=te->ns,((
_tmp353->ae=te->ae,((_tmp353->le=(struct Cyc_Tcenv_Fenv*)fe,((_tmp353->allow_valueof=
te->allow_valueof,_tmp353)))))))));};}static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(
struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_tmp354;
return(_tmp354=_region_malloc(l,sizeof(*_tmp354)),((_tmp354->ns=te->ns,((_tmp354->ae=
te->ae,((_tmp354->le=0,((_tmp354->allow_valueof=te->allow_valueof,_tmp354)))))))));}
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,
struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpBA;struct Cyc_List_List*
_tmpBB;struct Cyc_RgnOrder_RgnPO*_tmpBC;const struct Cyc_Tcenv_Bindings*_tmpBD;
struct Cyc_Absyn_Stmt*_tmpBE;struct Cyc_Tcenv_CtrlEnv*_tmpBF;void*_tmpC0;void*
_tmpC1;int _tmpC2;struct _RegionHandle*_tmpC3;struct Cyc_Tcenv_Fenv _tmpB9=*f;_tmpBA=
_tmpB9.shared;_tmpBB=_tmpB9.type_vars;_tmpBC=_tmpB9.region_order;_tmpBD=_tmpB9.locals;
_tmpBE=_tmpB9.encloser;_tmpBF=_tmpB9.ctrl_env;_tmpC0=_tmpB9.capability;_tmpC1=
_tmpB9.curr_rgn;_tmpC2=_tmpB9.in_notreadctxt;_tmpC3=_tmpB9.fnrgn;{struct Cyc_Tcenv_Fenv*
_tmp355;return(_tmp355=_region_malloc(l,sizeof(*_tmp355)),((_tmp355->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmpBA,((_tmp355->type_vars=(struct Cyc_List_List*)
_tmpBB,((_tmp355->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpBC,((_tmp355->locals=(
const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpBD),((
_tmp355->encloser=(struct Cyc_Absyn_Stmt*)_tmpBE,((_tmp355->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmpBF),((_tmp355->capability=(void*)_tmpC0,((_tmp355->curr_rgn=(
void*)_tmpC1,((_tmp355->in_notreadctxt=(int)_tmpC2,((_tmp355->fnrgn=(struct
_RegionHandle*)l,_tmp355)))))))))))))))))))));};}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpC6;
struct Cyc_List_List*_tmpC7;struct Cyc_RgnOrder_RgnPO*_tmpC8;const struct Cyc_Tcenv_Bindings*
_tmpC9;struct Cyc_Absyn_Stmt*_tmpCA;struct Cyc_Tcenv_CtrlEnv*_tmpCB;void*_tmpCC;
void*_tmpCD;int _tmpCE;struct Cyc_Tcenv_Fenv _tmpC5=*f;_tmpC6=_tmpC5.shared;_tmpC7=
_tmpC5.type_vars;_tmpC8=_tmpC5.region_order;_tmpC9=_tmpC5.locals;_tmpCA=_tmpC5.encloser;
_tmpCB=_tmpC5.ctrl_env;_tmpCC=_tmpC5.capability;_tmpCD=_tmpC5.curr_rgn;_tmpCE=
_tmpC5.in_notreadctxt;{struct _RegionHandle*_tmpD0;void*_tmpD1;void*_tmpD2;const
struct _tuple9*_tmpD3;void*_tmpD4;int _tmpD5;struct Cyc_Tcenv_CtrlEnv _tmpCF=*_tmpCB;
_tmpD0=_tmpCF.ctrl_rgn;_tmpD1=_tmpCF.continue_stmt;_tmpD2=_tmpCF.break_stmt;
_tmpD3=_tmpCF.fallthru_clause;_tmpD4=_tmpCF.next_stmt;_tmpD5=_tmpCF.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp356;struct Cyc_Tcenv_CtrlEnv*_tmpD6=(_tmp356=
_region_malloc(l,sizeof(*_tmp356)),((_tmp356->ctrl_rgn=_tmpD0,((_tmp356->continue_stmt=
_tmpD1,((_tmp356->break_stmt=_tmpD2,((_tmp356->fallthru_clause=_tmpD3,((_tmp356->next_stmt=
_tmpD4,((_tmp356->try_depth=_tmpD5,_tmp356)))))))))))));struct Cyc_Tcenv_Fenv*
_tmp357;return(_tmp357=_region_malloc(l,sizeof(*_tmp357)),((_tmp357->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmpC6,((_tmp357->type_vars=(struct Cyc_List_List*)
_tmpC7,((_tmp357->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC8,((_tmp357->locals=(
const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpC9),((
_tmp357->encloser=(struct Cyc_Absyn_Stmt*)_tmpCA,((_tmp357->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)
_tmpD6,((_tmp357->capability=(void*)_tmpCC,((_tmp357->curr_rgn=(void*)_tmpCD,((
_tmp357->in_notreadctxt=(int)_tmpCE,((_tmp357->fnrgn=(struct _RegionHandle*)l,
_tmp357)))))))))))))))))))));};};}void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv _tmpDB;struct Cyc_Tcenv_SharedFenv*_tmpDC;const char*
_tmp358;struct Cyc_Tcenv_Fenv*_tmpDA=Cyc_Tcenv_get_fenv(te,((_tmp358="return_typ",
_tag_dyneither(_tmp358,sizeof(char),11))));_tmpDB=*_tmpDA;_tmpDC=_tmpDB.shared;{
void*_tmpDE;struct Cyc_Tcenv_SharedFenv _tmpDD=*_tmpDC;_tmpDE=_tmpDD.return_typ;
return _tmpDE;};}struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmpDF=te->le;if(te->le == 0)return 0;{struct Cyc_List_List*
_tmpE1;struct Cyc_Tcenv_Fenv _tmpE0=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpDF));
_tmpE1=_tmpE0.type_vars;return _tmpE1;};}struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Core_Opt*_tmp359;return(_tmp359=_cycalloc(
sizeof(*_tmp359)),((_tmp359->v=Cyc_Tcenv_lookup_type_vars(te),_tmp359)));}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){struct Cyc_Tcenv_Fenv _tmpE5;
struct Cyc_Tcenv_SharedFenv*_tmpE6;struct Cyc_List_List*_tmpE7;struct Cyc_RgnOrder_RgnPO*
_tmpE8;const struct Cyc_Tcenv_Bindings*_tmpE9;struct Cyc_Absyn_Stmt*_tmpEA;struct
Cyc_Tcenv_CtrlEnv*_tmpEB;void*_tmpEC;void*_tmpED;int _tmpEE;const char*_tmp35A;
struct Cyc_Tcenv_Fenv*_tmpE4=Cyc_Tcenv_get_fenv(te,((_tmp35A="add_type_vars",
_tag_dyneither(_tmp35A,sizeof(char),14))));_tmpE5=*_tmpE4;_tmpE6=_tmpE5.shared;
_tmpE7=_tmpE5.type_vars;_tmpE8=_tmpE5.region_order;_tmpE9=_tmpE5.locals;_tmpEA=
_tmpE5.encloser;_tmpEB=_tmpE5.ctrl_env;_tmpEC=_tmpE5.capability;_tmpED=_tmpE5.curr_rgn;
_tmpEE=_tmpE5.in_notreadctxt;Cyc_Tcutil_add_tvar_identities(tvs);{struct Cyc_List_List*
_tmpEF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(tvs,_tmpE7);Cyc_Tcutil_check_unique_tvars(loc,_tmpEF);{struct
Cyc_Tcenv_Fenv*_tmp35B;struct Cyc_Tcenv_Fenv*_tmpF0=(_tmp35B=_region_malloc(r,
sizeof(*_tmp35B)),((_tmp35B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpE6,((
_tmp35B->type_vars=(struct Cyc_List_List*)_tmpEF,((_tmp35B->region_order=(struct
Cyc_RgnOrder_RgnPO*)_tmpE8,((_tmp35B->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmpE9),((_tmp35B->encloser=(struct Cyc_Absyn_Stmt*)
_tmpEA,((_tmp35B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmpEB),((_tmp35B->capability=(void*)_tmpEC,((_tmp35B->curr_rgn=(void*)_tmpED,((
_tmp35B->in_notreadctxt=(int)_tmpEE,((_tmp35B->fnrgn=(struct _RegionHandle*)r,
_tmp35B)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmpF0);};};}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpF2=te->le;if(_tmpF2 == 0)return Cyc_Tcenv_put_emptyfenv(r,
te);else{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,(struct Cyc_Tcenv_Fenv*)
_tmpF2);return Cyc_Tcenv_put_fenv(r,te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Vardecl*vd){struct _dyneither_ptr*_tmpF3=(*vd->name).f2;{union Cyc_Absyn_Nmspace
_tmpF4=(*vd->name).f1;int _tmpF5;_LL5C: if((_tmpF4.Loc_n).tag != 4)goto _LL5E;_tmpF5=(
int)(_tmpF4.Loc_n).val;_LL5D: goto _LL5B;_LL5E:;_LL5F: {struct Cyc_Core_Impossible_struct
_tmp361;const char*_tmp360;struct Cyc_Core_Impossible_struct*_tmp35F;(int)_throw((
void*)((_tmp35F=_cycalloc(sizeof(*_tmp35F)),((_tmp35F[0]=((_tmp361.tag=Cyc_Core_Impossible,((
_tmp361.f1=((_tmp360="add_local_var: called with Rel_n",_tag_dyneither(_tmp360,
sizeof(char),33))),_tmp361)))),_tmp35F)))));}_LL5B:;}{struct Cyc_Tcenv_Fenv _tmpFB;
struct Cyc_Tcenv_SharedFenv*_tmpFC;struct Cyc_List_List*_tmpFD;struct Cyc_RgnOrder_RgnPO*
_tmpFE;const struct Cyc_Tcenv_Bindings*_tmpFF;struct Cyc_Absyn_Stmt*_tmp100;struct
Cyc_Tcenv_CtrlEnv*_tmp101;void*_tmp102;void*_tmp103;int _tmp104;const char*_tmp362;
struct Cyc_Tcenv_Fenv*_tmpFA=Cyc_Tcenv_get_fenv(te,((_tmp362="add_local_var",
_tag_dyneither(_tmp362,sizeof(char),14))));_tmpFB=*_tmpFA;_tmpFC=_tmpFB.shared;
_tmpFD=_tmpFB.type_vars;_tmpFE=_tmpFB.region_order;_tmpFF=_tmpFB.locals;_tmp100=
_tmpFB.encloser;_tmp101=_tmpFB.ctrl_env;_tmp102=_tmpFB.capability;_tmp103=_tmpFB.curr_rgn;
_tmp104=_tmpFB.in_notreadctxt;{struct Cyc_Absyn_Local_b_struct*_tmp368;struct Cyc_Absyn_Local_b_struct
_tmp367;struct Cyc_Tcenv_Bindings*_tmp366;struct Cyc_Tcenv_Bindings*_tmp105=(
_tmp366=_region_malloc(r,sizeof(*_tmp366)),((_tmp366->v=_tmpF3,((_tmp366->b=(
void*)((_tmp368=_cycalloc(sizeof(*_tmp368)),((_tmp368[0]=((_tmp367.tag=4,((
_tmp367.f1=vd,_tmp367)))),_tmp368)))),((_tmp366->tl=(const struct Cyc_Tcenv_Bindings*)
_tmpFF,_tmp366)))))));struct Cyc_Tcenv_Fenv*_tmp369;struct Cyc_Tcenv_Fenv*_tmp106=(
_tmp369=_region_malloc(r,sizeof(*_tmp369)),((_tmp369->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpFC,((_tmp369->type_vars=(struct Cyc_List_List*)_tmpFD,((_tmp369->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpFE,((_tmp369->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp105),((_tmp369->encloser=(struct Cyc_Absyn_Stmt*)
_tmp100,((_tmp369->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp101),((_tmp369->capability=(void*)_tmp102,((_tmp369->curr_rgn=(void*)_tmp103,((
_tmp369->in_notreadctxt=(int)_tmp104,((_tmp369->fnrgn=(struct _RegionHandle*)r,
_tmp369)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmp106);};};}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmp10B=te->le;if(_tmp10B == 0)return Cyc_Tcenv_put_emptyfenv(
r,te);{struct Cyc_Tcenv_SharedFenv*_tmp10D;struct Cyc_List_List*_tmp10E;struct Cyc_RgnOrder_RgnPO*
_tmp10F;const struct Cyc_Tcenv_Bindings*_tmp110;struct Cyc_Absyn_Stmt*_tmp111;
struct Cyc_Tcenv_CtrlEnv*_tmp112;void*_tmp113;void*_tmp114;int _tmp115;struct Cyc_Tcenv_Fenv
_tmp10C=*_tmp10B;_tmp10D=_tmp10C.shared;_tmp10E=_tmp10C.type_vars;_tmp10F=
_tmp10C.region_order;_tmp110=_tmp10C.locals;_tmp111=_tmp10C.encloser;_tmp112=
_tmp10C.ctrl_env;_tmp113=_tmp10C.capability;_tmp114=_tmp10C.curr_rgn;_tmp115=
_tmp10C.in_notreadctxt;{struct Cyc_Tcenv_Fenv*_tmp36A;struct Cyc_Tcenv_Fenv*
_tmp116=(_tmp36A=_region_malloc(r,sizeof(*_tmp36A)),((_tmp36A->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp10D,((_tmp36A->type_vars=(struct Cyc_List_List*)_tmp10E,((_tmp36A->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp10F,((_tmp36A->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp110),((_tmp36A->encloser=(struct Cyc_Absyn_Stmt*)
_tmp111,((_tmp36A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp112),((_tmp36A->capability=(void*)_tmp113,((_tmp36A->curr_rgn=(void*)_tmp114,((
_tmp36A->in_notreadctxt=(int)1,((_tmp36A->fnrgn=(struct _RegionHandle*)r,_tmp36A)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp116);};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp118=te->le;
if(_tmp118 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmp11A;struct Cyc_List_List*_tmp11B;struct Cyc_RgnOrder_RgnPO*_tmp11C;const struct
Cyc_Tcenv_Bindings*_tmp11D;struct Cyc_Absyn_Stmt*_tmp11E;struct Cyc_Tcenv_CtrlEnv*
_tmp11F;void*_tmp120;void*_tmp121;int _tmp122;struct Cyc_Tcenv_Fenv _tmp119=*
_tmp118;_tmp11A=_tmp119.shared;_tmp11B=_tmp119.type_vars;_tmp11C=_tmp119.region_order;
_tmp11D=_tmp119.locals;_tmp11E=_tmp119.encloser;_tmp11F=_tmp119.ctrl_env;_tmp120=
_tmp119.capability;_tmp121=_tmp119.curr_rgn;_tmp122=_tmp119.in_notreadctxt;{
struct Cyc_Tcenv_Fenv*_tmp36B;struct Cyc_Tcenv_Fenv*_tmp123=(_tmp36B=
_region_malloc(r,sizeof(*_tmp36B)),((_tmp36B->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp11A,((_tmp36B->type_vars=(struct Cyc_List_List*)_tmp11B,((_tmp36B->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp11C,((_tmp36B->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp11D),((_tmp36B->encloser=(struct Cyc_Absyn_Stmt*)
_tmp11E,((_tmp36B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp11F),((_tmp36B->capability=(void*)_tmp120,((_tmp36B->curr_rgn=(void*)_tmp121,((
_tmp36B->in_notreadctxt=(int)0,((_tmp36B->fnrgn=(struct _RegionHandle*)r,_tmp36B)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp123);};};}int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp125=te->le;if(_tmp125 == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmp127;int _tmp128;struct Cyc_Tcenv_Fenv*_tmp126=(struct Cyc_Tcenv_Fenv*)
_tmp125;_tmp127=*_tmp126;_tmp128=_tmp127.in_notreadctxt;return _tmp128;};}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){struct _dyneither_ptr*
_tmp129=(*vd->name).f2;struct Cyc_Tcenv_Fenv _tmp12C;struct Cyc_Tcenv_SharedFenv*
_tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_RgnOrder_RgnPO*_tmp12F;const struct
Cyc_Tcenv_Bindings*_tmp130;struct Cyc_Absyn_Stmt*_tmp131;struct Cyc_Tcenv_CtrlEnv*
_tmp132;void*_tmp133;void*_tmp134;int _tmp135;const char*_tmp36C;struct Cyc_Tcenv_Fenv*
_tmp12B=Cyc_Tcenv_get_fenv(te,((_tmp36C="add_pat_var",_tag_dyneither(_tmp36C,
sizeof(char),12))));_tmp12C=*_tmp12B;_tmp12D=_tmp12C.shared;_tmp12E=_tmp12C.type_vars;
_tmp12F=_tmp12C.region_order;_tmp130=_tmp12C.locals;_tmp131=_tmp12C.encloser;
_tmp132=_tmp12C.ctrl_env;_tmp133=_tmp12C.capability;_tmp134=_tmp12C.curr_rgn;
_tmp135=_tmp12C.in_notreadctxt;{struct Cyc_Absyn_Pat_b_struct*_tmp372;struct Cyc_Absyn_Pat_b_struct
_tmp371;struct Cyc_Tcenv_Bindings*_tmp370;struct Cyc_Tcenv_Bindings*_tmp136=(
_tmp370=_region_malloc(r,sizeof(*_tmp370)),((_tmp370->v=_tmp129,((_tmp370->b=(
void*)((_tmp372=_cycalloc(sizeof(*_tmp372)),((_tmp372[0]=((_tmp371.tag=5,((
_tmp371.f1=vd,_tmp371)))),_tmp372)))),((_tmp370->tl=(const struct Cyc_Tcenv_Bindings*)
_tmp130,_tmp370)))))));struct Cyc_Tcenv_Fenv*_tmp373;struct Cyc_Tcenv_Fenv*_tmp137=(
_tmp373=_region_malloc(r,sizeof(*_tmp373)),((_tmp373->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp12D,((_tmp373->type_vars=(struct Cyc_List_List*)_tmp12E,((_tmp373->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp12F,((_tmp373->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp136),((_tmp373->encloser=(struct Cyc_Absyn_Stmt*)
_tmp131,((_tmp373->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp132),((_tmp373->capability=(void*)_tmp133,((_tmp373->curr_rgn=(void*)_tmp134,((
_tmp373->in_notreadctxt=(int)_tmp135,((_tmp373->fnrgn=(struct _RegionHandle*)r,
_tmp373)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmp137);};}void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct _tuple0*q){struct Cyc_Tcenv_Fenv*_tmp13C=te->le;struct _tuple0 _tmp13E;union
Cyc_Absyn_Nmspace _tmp13F;struct _dyneither_ptr*_tmp140;struct _tuple0*_tmp13D=q;
_tmp13E=*_tmp13D;_tmp13F=_tmp13E.f1;_tmp140=_tmp13E.f2;{union Cyc_Absyn_Nmspace
_tmp141=_tmp13F;int _tmp142;struct Cyc_List_List*_tmp143;_LL61: if((_tmp141.Loc_n).tag
!= 4)goto _LL63;_tmp142=(int)(_tmp141.Loc_n).val;_LL62: if(_tmp13C == 0){struct Cyc_Dict_Absent_struct
_tmp376;struct Cyc_Dict_Absent_struct*_tmp375;(int)_throw((void*)((_tmp375=
_cycalloc_atomic(sizeof(*_tmp375)),((_tmp375[0]=((_tmp376.tag=Cyc_Dict_Absent,
_tmp376)),_tmp375)))));}goto _LL64;_LL63: if((_tmp141.Rel_n).tag != 1)goto _LL65;
_tmp143=(struct Cyc_List_List*)(_tmp141.Rel_n).val;if(_tmp143 != 0)goto _LL65;if(!(
_tmp13C != 0))goto _LL65;_LL64: {struct Cyc_Tcenv_Fenv _tmp147;const struct Cyc_Tcenv_Bindings*
_tmp148;struct Cyc_Tcenv_Fenv*_tmp146=(struct Cyc_Tcenv_Fenv*)_tmp13C;_tmp147=*
_tmp146;_tmp148=_tmp147.locals;{struct _handler_cons _tmp149;_push_handler(&
_tmp149);{int _tmp14B=0;if(setjmp(_tmp149.handler))_tmp14B=1;if(!_tmp14B){{struct
Cyc_Tcenv_VarRes_struct _tmp379;struct Cyc_Tcenv_VarRes_struct*_tmp378;void*
_tmp14E=(void*)((_tmp378=_region_malloc(r,sizeof(*_tmp378)),((_tmp378[0]=((
_tmp379.tag=0,((_tmp379.f1=(void*)Cyc_Tcenv_lookup_binding(_tmp148,_tmp140),
_tmp379)))),_tmp378))));_npop_handler(0);return _tmp14E;};_pop_handler();}else{
void*_tmp14A=(void*)_exn_thrown;void*_tmp150=_tmp14A;_LL68: {struct Cyc_Tcenv_NoBinding_struct*
_tmp151=(struct Cyc_Tcenv_NoBinding_struct*)_tmp150;if(_tmp151->tag != Cyc_Tcenv_NoBinding)
goto _LL6A;}_LL69: return(void*)Cyc_Tcenv_lookup_ordinary_global(te,loc,q);_LL6A:;
_LL6B:(void)_throw(_tmp150);_LL67:;}};};}_LL65:;_LL66: {struct _handler_cons
_tmp152;_push_handler(& _tmp152);{int _tmp154=0;if(setjmp(_tmp152.handler))_tmp154=
1;if(!_tmp154){{void*_tmp155=(void*)Cyc_Tcenv_lookup_ordinary_global(te,loc,q);
_npop_handler(0);return _tmp155;};_pop_handler();}else{void*_tmp153=(void*)
_exn_thrown;void*_tmp157=_tmp153;_LL6D: {struct Cyc_Dict_Absent_struct*_tmp158=(
struct Cyc_Dict_Absent_struct*)_tmp157;if(_tmp158->tag != Cyc_Dict_Absent)goto
_LL6F;}_LL6E: {struct Cyc_Tcenv_VarRes_struct _tmp37C;struct Cyc_Tcenv_VarRes_struct*
_tmp37B;return(void*)((_tmp37B=_region_malloc(r,sizeof(*_tmp37B)),((_tmp37B[0]=((
_tmp37C.tag=0,((_tmp37C.f1=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),_tmp37C)))),
_tmp37B))));}_LL6F:;_LL70:(void)_throw(_tmp157);_LL6C:;}};}_LL60:;};}void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_Fenv _tmp15D;struct Cyc_Tcenv_CtrlEnv*_tmp15E;const char*_tmp37D;
struct Cyc_Tcenv_Fenv*_tmp15C=Cyc_Tcenv_get_fenv(te,((_tmp37D="process_continue",
_tag_dyneither(_tmp37D,sizeof(char),17))));_tmp15D=*_tmp15C;_tmp15E=_tmp15D.ctrl_env;{
void*_tmp15F=_tmp15E->continue_stmt;struct Cyc_Absyn_Stmt*_tmp161;_LL72: {struct
Cyc_Tcenv_Stmt_j_struct*_tmp160=(struct Cyc_Tcenv_Stmt_j_struct*)_tmp15F;if(
_tmp160->tag != 3)goto _LL74;else{_tmp161=_tmp160->f1;}}_LL73:{struct Cyc_List_List*
_tmp37E;_tmp161->non_local_preds=((_tmp37E=_cycalloc(sizeof(*_tmp37E)),((_tmp37E->hd=
s,((_tmp37E->tl=_tmp161->non_local_preds,_tmp37E))))));}*sopt=(struct Cyc_Absyn_Stmt*)
_tmp161;return;_LL74: {struct Cyc_Tcenv_NotLoop_j_struct*_tmp162=(struct Cyc_Tcenv_NotLoop_j_struct*)
_tmp15F;if(_tmp162->tag != 0)goto _LL76;}_LL75:{const char*_tmp381;void*_tmp380;(
_tmp380=0,Cyc_Tcutil_terr(s->loc,((_tmp381="continue not in a loop",
_tag_dyneither(_tmp381,sizeof(char),23))),_tag_dyneither(_tmp380,sizeof(void*),0)));}
return;_LL76: {struct Cyc_Tcenv_CaseEnd_j_struct*_tmp163=(struct Cyc_Tcenv_CaseEnd_j_struct*)
_tmp15F;if(_tmp163->tag != 1)goto _LL78;}_LL77: goto _LL79;_LL78: {struct Cyc_Tcenv_FnEnd_j_struct*
_tmp164=(struct Cyc_Tcenv_FnEnd_j_struct*)_tmp15F;if(_tmp164->tag != 2)goto _LL71;}
_LL79: {const char*_tmp382;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((
_tmp382="bad continue destination",_tag_dyneither(_tmp382,sizeof(char),25))));}
_LL71:;};}void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv _tmp16B;struct Cyc_Tcenv_CtrlEnv*
_tmp16C;struct Cyc_Tcenv_SharedFenv*_tmp16D;const char*_tmp383;struct Cyc_Tcenv_Fenv*
_tmp16A=Cyc_Tcenv_get_fenv(te,((_tmp383="process_break",_tag_dyneither(_tmp383,
sizeof(char),14))));_tmp16B=*_tmp16A;_tmp16C=_tmp16B.ctrl_env;_tmp16D=_tmp16B.shared;{
void*_tmp16E=_tmp16C->break_stmt;struct Cyc_Absyn_Stmt*_tmp170;_LL7B: {struct Cyc_Tcenv_Stmt_j_struct*
_tmp16F=(struct Cyc_Tcenv_Stmt_j_struct*)_tmp16E;if(_tmp16F->tag != 3)goto _LL7D;
else{_tmp170=_tmp16F->f1;}}_LL7C:{struct Cyc_List_List*_tmp384;_tmp170->non_local_preds=((
_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384->hd=s,((_tmp384->tl=_tmp170->non_local_preds,
_tmp384))))));}*sopt=(struct Cyc_Absyn_Stmt*)_tmp170;return;_LL7D: {struct Cyc_Tcenv_NotLoop_j_struct*
_tmp171=(struct Cyc_Tcenv_NotLoop_j_struct*)_tmp16E;if(_tmp171->tag != 0)goto _LL7F;}
_LL7E:{const char*_tmp387;void*_tmp386;(_tmp386=0,Cyc_Tcutil_terr(s->loc,((
_tmp387="break not in a loop or switch",_tag_dyneither(_tmp387,sizeof(char),30))),
_tag_dyneither(_tmp386,sizeof(void*),0)));}return;_LL7F: {struct Cyc_Tcenv_FnEnd_j_struct*
_tmp172=(struct Cyc_Tcenv_FnEnd_j_struct*)_tmp16E;if(_tmp172->tag != 2)goto _LL81;}
_LL80:{void*_tmp177=Cyc_Tcutil_compress(_tmp16D->return_typ);_LL84: {struct Cyc_Absyn_VoidType_struct*
_tmp178=(struct Cyc_Absyn_VoidType_struct*)_tmp177;if(_tmp178->tag != 0)goto _LL86;}
_LL85: goto _LL83;_LL86: {struct Cyc_Absyn_DoubleType_struct*_tmp179=(struct Cyc_Absyn_DoubleType_struct*)
_tmp177;if(_tmp179->tag != 8)goto _LL88;}_LL87: goto _LL89;_LL88: {struct Cyc_Absyn_FloatType_struct*
_tmp17A=(struct Cyc_Absyn_FloatType_struct*)_tmp177;if(_tmp17A->tag != 7)goto _LL8A;}
_LL89: goto _LL8B;_LL8A: {struct Cyc_Absyn_IntType_struct*_tmp17B=(struct Cyc_Absyn_IntType_struct*)
_tmp177;if(_tmp17B->tag != 6)goto _LL8C;}_LL8B:{const char*_tmp38A;void*_tmp389;(
_tmp389=0,Cyc_Tcutil_warn(s->loc,((_tmp38A="break may cause function not to return a value",
_tag_dyneither(_tmp38A,sizeof(char),47))),_tag_dyneither(_tmp389,sizeof(void*),0)));}
goto _LL83;_LL8C:;_LL8D:{const char*_tmp38D;void*_tmp38C;(_tmp38C=0,Cyc_Tcutil_terr(
s->loc,((_tmp38D="break may cause function not to return a value",_tag_dyneither(
_tmp38D,sizeof(char),47))),_tag_dyneither(_tmp38C,sizeof(void*),0)));}goto _LL83;
_LL83:;}return;_LL81: {struct Cyc_Tcenv_CaseEnd_j_struct*_tmp173=(struct Cyc_Tcenv_CaseEnd_j_struct*)
_tmp16E;if(_tmp173->tag != 1)goto _LL7A;}_LL82:{const char*_tmp390;void*_tmp38F;(
_tmp38F=0,Cyc_Tcutil_terr(s->loc,((_tmp390="break causes outer switch clause to implicitly fallthru",
_tag_dyneither(_tmp390,sizeof(char),56))),_tag_dyneither(_tmp38F,sizeof(void*),0)));}
return;_LL7A:;};}void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv _tmp184;
struct Cyc_Tcenv_SharedFenv*_tmp185;const char*_tmp391;struct Cyc_Tcenv_Fenv*
_tmp183=Cyc_Tcenv_get_fenv(te,((_tmp391="process_goto",_tag_dyneither(_tmp391,
sizeof(char),13))));_tmp184=*_tmp183;_tmp185=_tmp184.shared;{struct Cyc_Absyn_Stmt**
sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup_opt)(_tmp185->seen_labels,l);if(sopt2 == 0){struct Cyc_Dict_Dict
_tmp186=_tmp185->needed_labels;struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp186,l);
struct _RegionHandle*frgn=_tmp185->frgn;if(slopt == 0){struct Cyc_List_List**
_tmp392;slopt=((_tmp392=_region_malloc(frgn,sizeof(*_tmp392)),((_tmp392[0]=0,
_tmp392))));}{struct Cyc_List_List*_tmp393;struct Cyc_List_List*new_needed=(
_tmp393=_cycalloc(sizeof(*_tmp393)),((_tmp393->hd=s,((_tmp393->tl=*slopt,_tmp393)))));
_tmp185->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp186,l,new_needed);};}
else{struct Cyc_Absyn_Stmt*s=*sopt2;{struct Cyc_List_List*_tmp394;s->non_local_preds=((
_tmp394=_cycalloc(sizeof(*_tmp394)),((_tmp394->hd=s,((_tmp394->tl=s->non_local_preds,
_tmp394))))));}*sopt=(struct Cyc_Absyn_Stmt*)s;}};}const struct _tuple9*Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***
clauseopt){struct Cyc_Tcenv_Fenv _tmp18C;struct Cyc_Tcenv_CtrlEnv*_tmp18D;const char*
_tmp395;struct Cyc_Tcenv_Fenv*_tmp18B=Cyc_Tcenv_get_fenv(te,((_tmp395="process_fallthru",
_tag_dyneither(_tmp395,sizeof(char),17))));_tmp18C=*_tmp18B;_tmp18D=_tmp18C.ctrl_env;{
const struct _tuple9*_tmp18E=(const struct _tuple9*)_tmp18D->fallthru_clause;if(
_tmp18E != (const struct _tuple9*)0){{struct Cyc_List_List*_tmp396;(((*_tmp18E).f1)->body)->non_local_preds=((
_tmp396=_cycalloc(sizeof(*_tmp396)),((_tmp396->hd=s,((_tmp396->tl=(((*_tmp18E).f1)->body)->non_local_preds,
_tmp396))))));}{struct Cyc_Absyn_Switch_clause**_tmp397;*clauseopt=(struct Cyc_Absyn_Switch_clause**)((
_tmp397=_cycalloc(sizeof(*_tmp397)),((_tmp397[0]=(*_tmp18E).f1,_tmp397))));};}
return _tmp18E;};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,
struct Cyc_Absyn_Switch_clause*clause){struct Cyc_Tcenv_Fenv _tmp193;struct Cyc_Tcenv_SharedFenv*
_tmp194;struct Cyc_List_List*_tmp195;struct Cyc_RgnOrder_RgnPO*_tmp196;const struct
Cyc_Tcenv_Bindings*_tmp197;struct Cyc_Absyn_Stmt*_tmp198;struct Cyc_Tcenv_CtrlEnv*
_tmp199;void*_tmp19A;void*_tmp19B;int _tmp19C;const char*_tmp398;struct Cyc_Tcenv_Fenv*
_tmp192=Cyc_Tcenv_get_fenv(te,((_tmp398="set_fallthru",_tag_dyneither(_tmp398,
sizeof(char),13))));_tmp193=*_tmp192;_tmp194=_tmp193.shared;_tmp195=_tmp193.type_vars;
_tmp196=_tmp193.region_order;_tmp197=_tmp193.locals;_tmp198=_tmp193.encloser;
_tmp199=_tmp193.ctrl_env;_tmp19A=_tmp193.capability;_tmp19B=_tmp193.curr_rgn;
_tmp19C=_tmp193.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv _tmp19E;struct
_RegionHandle*_tmp19F;void*_tmp1A0;void*_tmp1A1;const struct _tuple9*_tmp1A2;void*
_tmp1A3;int _tmp1A4;struct Cyc_Tcenv_CtrlEnv*_tmp19D=_tmp199;_tmp19E=*_tmp19D;
_tmp19F=_tmp19E.ctrl_rgn;_tmp1A0=_tmp19E.continue_stmt;_tmp1A1=_tmp19E.break_stmt;
_tmp1A2=_tmp19E.fallthru_clause;_tmp1A3=_tmp19E.next_stmt;_tmp1A4=_tmp19E.try_depth;{
struct Cyc_List_List*ft_typ=0;for(0;vds != 0;vds=vds->tl){struct Cyc_List_List*
_tmp399;ft_typ=((_tmp399=_region_malloc(_tmp19F,sizeof(*_tmp399)),((_tmp399->hd=((
struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp399->tl=ft_typ,_tmp399))))));}{
const struct Cyc_Tcenv_CList*_tmp1A6=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);struct _tuple9*_tmp39C;struct Cyc_Tcenv_CtrlEnv*
_tmp39B;struct Cyc_Tcenv_CtrlEnv*_tmp1A7=(_tmp39B=_region_malloc(r,sizeof(*
_tmp39B)),((_tmp39B->ctrl_rgn=_tmp19F,((_tmp39B->continue_stmt=_tmp1A0,((_tmp39B->break_stmt=
_tmp1A1,((_tmp39B->fallthru_clause=(const struct _tuple9*)((_tmp39C=_region_malloc(
_tmp19F,sizeof(*_tmp39C)),((_tmp39C->f1=clause,((_tmp39C->f2=new_tvs,((_tmp39C->f3=
_tmp1A6,_tmp39C)))))))),((_tmp39B->next_stmt=_tmp1A3,((_tmp39B->try_depth=
_tmp1A4,_tmp39B)))))))))))));struct Cyc_Tcenv_Fenv*_tmp39D;struct Cyc_Tcenv_Fenv*
_tmp1A8=(_tmp39D=_region_malloc(r,sizeof(*_tmp39D)),((_tmp39D->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp194,((_tmp39D->type_vars=(struct Cyc_List_List*)_tmp195,((_tmp39D->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp196,((_tmp39D->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp197),((_tmp39D->encloser=(struct Cyc_Absyn_Stmt*)
_tmp198,((_tmp39D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1A7,((_tmp39D->capability=(
void*)_tmp19A,((_tmp39D->curr_rgn=(void*)_tmp19B,((_tmp39D->in_notreadctxt=(int)
_tmp19C,((_tmp39D->fnrgn=(struct _RegionHandle*)r,_tmp39D)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp1A8);};};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*_tmp39E;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp39E="clear_fallthru",
_tag_dyneither(_tmp39E,sizeof(char),15)))));struct Cyc_Tcenv_Fenv _tmp1AD;struct
Cyc_Tcenv_CtrlEnv*_tmp1AE;struct Cyc_Tcenv_Fenv*_tmp1AC=fe;_tmp1AD=*_tmp1AC;
_tmp1AE=_tmp1AD.ctrl_env;_tmp1AE->fallthru_clause=(const struct _tuple9*)0;return
Cyc_Tcenv_put_fenv(r,te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
struct Cyc_Tcenv_Fenv _tmp1B2;struct Cyc_Tcenv_SharedFenv*_tmp1B3;struct Cyc_List_List*
_tmp1B4;struct Cyc_RgnOrder_RgnPO*_tmp1B5;const struct Cyc_Tcenv_Bindings*_tmp1B6;
struct Cyc_Absyn_Stmt*_tmp1B7;struct Cyc_Tcenv_CtrlEnv*_tmp1B8;void*_tmp1B9;void*
_tmp1BA;int _tmp1BB;const char*_tmp39F;struct Cyc_Tcenv_Fenv*_tmp1B1=Cyc_Tcenv_get_fenv(
te,((_tmp39F="set_in_loop",_tag_dyneither(_tmp39F,sizeof(char),12))));_tmp1B2=*
_tmp1B1;_tmp1B3=_tmp1B2.shared;_tmp1B4=_tmp1B2.type_vars;_tmp1B5=_tmp1B2.region_order;
_tmp1B6=_tmp1B2.locals;_tmp1B7=_tmp1B2.encloser;_tmp1B8=_tmp1B2.ctrl_env;_tmp1B9=
_tmp1B2.capability;_tmp1BA=_tmp1B2.curr_rgn;_tmp1BB=_tmp1B2.in_notreadctxt;{
struct Cyc_Tcenv_Stmt_j_struct*_tmp3AA;struct Cyc_Tcenv_Stmt_j_struct _tmp3A9;
struct Cyc_Tcenv_Stmt_j_struct*_tmp3A8;struct Cyc_Tcenv_Stmt_j_struct _tmp3A7;
struct Cyc_Tcenv_CtrlEnv*_tmp3A6;struct Cyc_Tcenv_CtrlEnv*new_cenv=(_tmp3A6=
_region_malloc(r,sizeof(*_tmp3A6)),((_tmp3A6->ctrl_rgn=r,((_tmp3A6->continue_stmt=(
void*)((_tmp3A8=_region_malloc(r,sizeof(*_tmp3A8)),((_tmp3A8[0]=((_tmp3A7.tag=3,((
_tmp3A7.f1=continue_dest,_tmp3A7)))),_tmp3A8)))),((_tmp3A6->break_stmt=(void*)
_tmp1B8->next_stmt,((_tmp3A6->next_stmt=(void*)((_tmp3AA=_region_malloc(r,
sizeof(*_tmp3AA)),((_tmp3AA[0]=((_tmp3A9.tag=3,((_tmp3A9.f1=continue_dest,
_tmp3A9)))),_tmp3AA)))),((_tmp3A6->fallthru_clause=(const struct _tuple9*)_tmp1B8->fallthru_clause,((
_tmp3A6->try_depth=_tmp1B8->try_depth,_tmp3A6)))))))))))));struct Cyc_Tcenv_Fenv*
_tmp3AB;struct Cyc_Tcenv_Fenv*new_fenv=(_tmp3AB=_region_malloc(r,sizeof(*_tmp3AB)),((
_tmp3AB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1B3,((_tmp3AB->type_vars=(
struct Cyc_List_List*)_tmp1B4,((_tmp3AB->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp1B5,((_tmp3AB->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)
_tmp1B6),((_tmp3AB->encloser=(struct Cyc_Absyn_Stmt*)_tmp1B7,((_tmp3AB->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3AB->capability=(void*)_tmp1B9,((_tmp3AB->curr_rgn=(
void*)_tmp1BA,((_tmp3AB->in_notreadctxt=(int)_tmp1BB,((_tmp3AB->fnrgn=(struct
_RegionHandle*)r,_tmp3AB)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,
new_fenv);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te){const char*_tmp3AC;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
r,Cyc_Tcenv_get_fenv(te,((_tmp3AC="enter_try",_tag_dyneither(_tmp3AC,sizeof(char),
10)))));struct Cyc_Tcenv_Fenv _tmp1C3;struct Cyc_Tcenv_CtrlEnv*_tmp1C4;struct Cyc_Tcenv_Fenv*
_tmp1C2=fe;_tmp1C3=*_tmp1C2;_tmp1C4=_tmp1C3.ctrl_env;++ _tmp1C4->try_depth;return
Cyc_Tcenv_put_fenv(r,te,fe);}int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv _tmp1C8;struct Cyc_Tcenv_CtrlEnv*_tmp1C9;const char*_tmp3AD;
struct Cyc_Tcenv_Fenv*_tmp1C7=Cyc_Tcenv_get_fenv(te,((_tmp3AD="get_try_depth",
_tag_dyneither(_tmp3AD,sizeof(char),14))));_tmp1C8=*_tmp1C7;_tmp1C9=_tmp1C8.ctrl_env;
return _tmp1C9->try_depth;}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*_tmp3AE;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3AE="set_in_switch",
_tag_dyneither(_tmp3AE,sizeof(char),14)))));struct Cyc_Tcenv_Fenv _tmp1CB;struct
Cyc_Tcenv_CtrlEnv*_tmp1CC;struct Cyc_Tcenv_Fenv*_tmp1CA=fe;_tmp1CB=*_tmp1CA;
_tmp1CC=_tmp1CB.ctrl_env;_tmp1CC->break_stmt=_tmp1CC->next_stmt;_tmp1CC->next_stmt=(
void*)& Cyc_Tcenv_CaseEnd_j_val;return Cyc_Tcenv_put_fenv(r,te,fe);}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){struct
Cyc_Tcenv_SharedFenv*_tmp1D0;struct Cyc_List_List*_tmp1D1;struct Cyc_RgnOrder_RgnPO*
_tmp1D2;const struct Cyc_Tcenv_Bindings*_tmp1D3;struct Cyc_Absyn_Stmt*_tmp1D4;
struct Cyc_Tcenv_CtrlEnv*_tmp1D5;void*_tmp1D6;void*_tmp1D7;int _tmp1D8;const char*
_tmp3AF;struct Cyc_Tcenv_Fenv _tmp1CF=*Cyc_Tcenv_get_fenv(te,((_tmp3AF="set_next",
_tag_dyneither(_tmp3AF,sizeof(char),9))));_tmp1D0=_tmp1CF.shared;_tmp1D1=_tmp1CF.type_vars;
_tmp1D2=_tmp1CF.region_order;_tmp1D3=_tmp1CF.locals;_tmp1D4=_tmp1CF.encloser;
_tmp1D5=_tmp1CF.ctrl_env;_tmp1D6=_tmp1CF.capability;_tmp1D7=_tmp1CF.curr_rgn;
_tmp1D8=_tmp1CF.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv*_tmp3B0;struct Cyc_Tcenv_CtrlEnv*
new_cenv=(_tmp3B0=_region_malloc(r,sizeof(*_tmp3B0)),((_tmp3B0->ctrl_rgn=r,((
_tmp3B0->continue_stmt=(void*)_tmp1D5->continue_stmt,((_tmp3B0->break_stmt=(void*)
_tmp1D5->break_stmt,((_tmp3B0->next_stmt=j,((_tmp3B0->fallthru_clause=(const
struct _tuple9*)_tmp1D5->fallthru_clause,((_tmp3B0->try_depth=_tmp1D5->try_depth,
_tmp3B0)))))))))))));struct Cyc_Tcenv_Fenv*_tmp3B1;struct Cyc_Tcenv_Fenv*new_fenv=(
_tmp3B1=_region_malloc(r,sizeof(*_tmp3B1)),((_tmp3B1->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1D0,((_tmp3B1->type_vars=(struct Cyc_List_List*)_tmp1D1,((_tmp3B1->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1D2,((_tmp3B1->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp1D3),((_tmp3B1->encloser=(struct Cyc_Absyn_Stmt*)
_tmp1D4,((_tmp3B1->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3B1->capability=(
void*)_tmp1D6,((_tmp3B1->curr_rgn=(void*)_tmp1D7,((_tmp3B1->in_notreadctxt=(int)
_tmp1D8,((_tmp3B1->fnrgn=(struct _RegionHandle*)r,_tmp3B1)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_Fenv
_tmp1DD;struct Cyc_Tcenv_SharedFenv*_tmp1DE;const char*_tmp3B2;struct Cyc_Tcenv_Fenv*
_tmp1DC=Cyc_Tcenv_get_fenv(te,((_tmp3B2="add_label",_tag_dyneither(_tmp3B2,
sizeof(char),10))));_tmp1DD=*_tmp1DC;_tmp1DE=_tmp1DD.shared;{struct Cyc_Dict_Dict
needed=_tmp1DE->needed_labels;struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp1DE->frgn;if(sl_opt != 0){_tmp1DE->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct
_dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{struct Cyc_List_List*_tmp1DF=*
sl_opt;s->non_local_preds=_tmp1DF;for(0;_tmp1DF != 0;_tmp1DF=_tmp1DF->tl){void*
_tmp1E0=((struct Cyc_Absyn_Stmt*)_tmp1DF->hd)->r;struct Cyc_Absyn_Stmt*_tmp1E2;
struct Cyc_Absyn_Stmt**_tmp1E3;_LL8F: {struct Cyc_Absyn_Goto_s_struct*_tmp1E1=(
struct Cyc_Absyn_Goto_s_struct*)_tmp1E0;if(_tmp1E1->tag != 8)goto _LL91;else{
_tmp1E2=_tmp1E1->f2;_tmp1E3=(struct Cyc_Absyn_Stmt**)& _tmp1E1->f2;}}_LL90:*
_tmp1E3=(struct Cyc_Absyn_Stmt*)s;goto _LL8E;_LL91:;_LL92:{const char*_tmp3B3;((int(*)(
struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3B3="Tcenv: add_label backpatching of non-goto",
_tag_dyneither(_tmp3B3,sizeof(char),42))));}goto _LL8E;_LL8E:;}};}if(((int(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp1DE->seen_labels,
v)){const char*_tmp3B7;void*_tmp3B6[1];struct Cyc_String_pa_struct _tmp3B5;(_tmp3B5.tag=
0,((_tmp3B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3B6[0]=&
_tmp3B5,Cyc_Tcutil_terr(s->loc,((_tmp3B7="Repeated label: %s",_tag_dyneither(
_tmp3B7,sizeof(char),19))),_tag_dyneither(_tmp3B6,sizeof(void*),1)))))));}
_tmp1DE->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp1DE->seen_labels,v,
s);return te;};}int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){struct
Cyc_Tcenv_Fenv _tmp1EA;struct Cyc_Tcenv_SharedFenv*_tmp1EB;const char*_tmp3B8;
struct Cyc_Tcenv_Fenv*_tmp1E9=Cyc_Tcenv_get_fenv(te,((_tmp3B8="all_labels_resolved",
_tag_dyneither(_tmp3B8,sizeof(char),20))));_tmp1EA=*_tmp1E9;_tmp1EB=_tmp1EA.shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp1EB->needed_labels);}
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv
_tmp1EE;struct Cyc_Absyn_Stmt*_tmp1EF;const char*_tmp3B9;struct Cyc_Tcenv_Fenv*
_tmp1ED=Cyc_Tcenv_get_fenv(te,((_tmp3B9="get_encloser",_tag_dyneither(_tmp3B9,
sizeof(char),13))));_tmp1EE=*_tmp1ED;_tmp1EF=_tmp1EE.encloser;return _tmp1EF;}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*
te){if(te->le == 0){struct Cyc_Tcenv_Tenv*_tmp3BA;return(_tmp3BA=_region_malloc(r2,
sizeof(*_tmp3BA)),((_tmp3BA->ns=te->ns,((_tmp3BA->ae=te->ae,((_tmp3BA->le=0,((
_tmp3BA->allow_valueof=1,_tmp3BA)))))))));}{struct Cyc_Tcenv_SharedFenv*_tmp1F2;
struct Cyc_List_List*_tmp1F3;struct Cyc_RgnOrder_RgnPO*_tmp1F4;const struct Cyc_Tcenv_Bindings*
_tmp1F5;struct Cyc_Absyn_Stmt*_tmp1F6;struct Cyc_Tcenv_CtrlEnv*_tmp1F7;void*
_tmp1F8;void*_tmp1F9;int _tmp1FA;struct Cyc_Tcenv_Fenv _tmp1F1=*((struct Cyc_Tcenv_Fenv*)
_check_null(te->le));_tmp1F2=_tmp1F1.shared;_tmp1F3=_tmp1F1.type_vars;_tmp1F4=
_tmp1F1.region_order;_tmp1F5=_tmp1F1.locals;_tmp1F6=_tmp1F1.encloser;_tmp1F7=
_tmp1F1.ctrl_env;_tmp1F8=_tmp1F1.capability;_tmp1F9=_tmp1F1.curr_rgn;_tmp1FA=
_tmp1F1.in_notreadctxt;{struct Cyc_Tcenv_Fenv*_tmp3BB;struct Cyc_Tcenv_Fenv*
_tmp1FB=(_tmp3BB=_region_malloc(r2,sizeof(*_tmp3BB)),((_tmp3BB->shared=(struct
Cyc_Tcenv_SharedFenv*)_tmp1F2,((_tmp3BB->type_vars=(struct Cyc_List_List*)_tmp1F3,((
_tmp3BB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1F4,((_tmp3BB->locals=(
const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp1F5),((
_tmp3BB->encloser=(struct Cyc_Absyn_Stmt*)_tmp1F6,((_tmp3BB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp1F7),((_tmp3BB->capability=(void*)_tmp1F8,((_tmp3BB->curr_rgn=(
void*)_tmp1F9,((_tmp3BB->in_notreadctxt=(int)_tmp1FA,((_tmp3BB->fnrgn=(struct
_RegionHandle*)r2,_tmp3BB)))))))))))))))))))));struct Cyc_Tcenv_Tenv*_tmp3BC;
return(_tmp3BC=_region_malloc(r2,sizeof(*_tmp3BC)),((_tmp3BC->ns=te->ns,((
_tmp3BC->ae=te->ae,((_tmp3BC->le=_tmp1FB,((_tmp3BC->allow_valueof=1,_tmp3BC)))))))));};};}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_SharedFenv*_tmp200;struct Cyc_List_List*
_tmp201;struct Cyc_RgnOrder_RgnPO*_tmp202;const struct Cyc_Tcenv_Bindings*_tmp203;
struct Cyc_Absyn_Stmt*_tmp204;struct Cyc_Tcenv_CtrlEnv*_tmp205;void*_tmp206;void*
_tmp207;int _tmp208;const char*_tmp3BD;struct Cyc_Tcenv_Fenv _tmp1FF=*Cyc_Tcenv_get_fenv(
te,((_tmp3BD="set_encloser",_tag_dyneither(_tmp3BD,sizeof(char),13))));_tmp200=
_tmp1FF.shared;_tmp201=_tmp1FF.type_vars;_tmp202=_tmp1FF.region_order;_tmp203=
_tmp1FF.locals;_tmp204=_tmp1FF.encloser;_tmp205=_tmp1FF.ctrl_env;_tmp206=_tmp1FF.capability;
_tmp207=_tmp1FF.curr_rgn;_tmp208=_tmp1FF.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmp3BE;struct Cyc_Tcenv_Fenv*_tmp209=(_tmp3BE=_region_malloc(r,sizeof(*_tmp3BE)),((
_tmp3BE->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp200,((_tmp3BE->type_vars=(
struct Cyc_List_List*)_tmp201,((_tmp3BE->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp202,((_tmp3BE->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)
_tmp203),((_tmp3BE->encloser=(struct Cyc_Absyn_Stmt*)s,((_tmp3BE->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp205),((_tmp3BE->capability=(
void*)_tmp206,((_tmp3BE->curr_rgn=(void*)_tmp207,((_tmp3BE->in_notreadctxt=(int)
_tmp208,((_tmp3BE->fnrgn=(struct _RegionHandle*)r,_tmp3BE)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp209);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
struct Cyc_Absyn_Tvar*tv;{void*_tmp20B=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*
_tmp20D;_LL94: {struct Cyc_Absyn_VarType_struct*_tmp20C=(struct Cyc_Absyn_VarType_struct*)
_tmp20B;if(_tmp20C->tag != 2)goto _LL96;else{_tmp20D=_tmp20C->f1;}}_LL95: tv=
_tmp20D;goto _LL93;_LL96:;_LL97:{const char*_tmp3BF;tv=((struct Cyc_Absyn_Tvar*(*)(
struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3BF="bad add region",
_tag_dyneither(_tmp3BF,sizeof(char),15))));}goto _LL93;_LL93:;}{struct Cyc_Tcenv_SharedFenv*
_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_RgnOrder_RgnPO*_tmp213;const struct
Cyc_Tcenv_Bindings*_tmp214;struct Cyc_Absyn_Stmt*_tmp215;struct Cyc_Tcenv_CtrlEnv*
_tmp216;void*_tmp217;void*_tmp218;int _tmp219;const char*_tmp3C0;struct Cyc_Tcenv_Fenv
_tmp210=*Cyc_Tcenv_get_fenv(te,((_tmp3C0="add_region",_tag_dyneither(_tmp3C0,
sizeof(char),11))));_tmp211=_tmp210.shared;_tmp212=_tmp210.type_vars;_tmp213=
_tmp210.region_order;_tmp214=_tmp210.locals;_tmp215=_tmp210.encloser;_tmp216=
_tmp210.ctrl_env;_tmp217=_tmp210.capability;_tmp218=_tmp210.curr_rgn;_tmp219=
_tmp210.in_notreadctxt;_tmp213=Cyc_RgnOrder_add_youngest(_tmp211->frgn,_tmp213,
tv,resetable,opened);{struct Cyc_Absyn_JoinEff_struct _tmp3D3;struct Cyc_List_List*
_tmp3D2;struct Cyc_Absyn_AccessEff_struct*_tmp3D1;struct Cyc_Absyn_AccessEff_struct
_tmp3D0;struct Cyc_List_List*_tmp3CF;struct Cyc_Absyn_JoinEff_struct*_tmp3CE;
_tmp217=(void*)((_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE[0]=((_tmp3D3.tag=
24,((_tmp3D3.f1=((_tmp3CF=_cycalloc(sizeof(*_tmp3CF)),((_tmp3CF->hd=(void*)((
_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1[0]=((_tmp3D0.tag=23,((_tmp3D0.f1=(
void*)rgn,_tmp3D0)))),_tmp3D1)))),((_tmp3CF->tl=((_tmp3D2=_cycalloc(sizeof(*
_tmp3D2)),((_tmp3D2->hd=_tmp217,((_tmp3D2->tl=0,_tmp3D2)))))),_tmp3CF)))))),
_tmp3D3)))),_tmp3CE))));}{struct Cyc_Tcenv_Fenv*_tmp3D4;struct Cyc_Tcenv_Fenv*
_tmp220=(_tmp3D4=_region_malloc(r,sizeof(*_tmp3D4)),((_tmp3D4->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp211,((_tmp3D4->type_vars=(struct Cyc_List_List*)_tmp212,((_tmp3D4->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp213,((_tmp3D4->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp214),((_tmp3D4->encloser=(struct Cyc_Absyn_Stmt*)
_tmp215,((_tmp3D4->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp216),((_tmp3D4->capability=(void*)_tmp217,((_tmp3D4->curr_rgn=(void*)_tmp218,((
_tmp3D4->in_notreadctxt=(int)_tmp219,((_tmp3D4->fnrgn=(struct _RegionHandle*)r,
_tmp3D4)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmp220);};};}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){struct Cyc_Tcenv_SharedFenv*
_tmp224;struct Cyc_List_List*_tmp225;struct Cyc_RgnOrder_RgnPO*_tmp226;const struct
Cyc_Tcenv_Bindings*_tmp227;struct Cyc_Absyn_Stmt*_tmp228;struct Cyc_Tcenv_CtrlEnv*
_tmp229;void*_tmp22A;void*_tmp22B;int _tmp22C;const char*_tmp3D5;struct Cyc_Tcenv_Fenv
_tmp223=*Cyc_Tcenv_get_fenv(te,((_tmp3D5="new_named_block",_tag_dyneither(
_tmp3D5,sizeof(char),16))));_tmp224=_tmp223.shared;_tmp225=_tmp223.type_vars;
_tmp226=_tmp223.region_order;_tmp227=_tmp223.locals;_tmp228=_tmp223.encloser;
_tmp229=_tmp223.ctrl_env;_tmp22A=_tmp223.capability;_tmp22B=_tmp223.curr_rgn;
_tmp22C=_tmp223.in_notreadctxt;{const char*_tmp3D6;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(
r,Cyc_Tcenv_get_fenv(te,((_tmp3D6="new_block",_tag_dyneither(_tmp3D6,sizeof(char),
10)))));struct Cyc_Absyn_VarType_struct _tmp3D9;struct Cyc_Absyn_VarType_struct*
_tmp3D8;void*block_typ=(void*)((_tmp3D8=_cycalloc(sizeof(*_tmp3D8)),((_tmp3D8[0]=((
_tmp3D9.tag=2,((_tmp3D9.f1=block_rgn,_tmp3D9)))),_tmp3D8))));{struct Cyc_List_List*
_tmp3DA;_tmp225=((_tmp3DA=_cycalloc(sizeof(*_tmp3DA)),((_tmp3DA->hd=block_rgn,((
_tmp3DA->tl=_tmp225,_tmp3DA))))));}Cyc_Tcutil_check_unique_tvars(loc,_tmp225);
_tmp226=Cyc_RgnOrder_add_youngest(_tmp224->frgn,_tmp226,block_rgn,0,0);{struct
Cyc_Absyn_JoinEff_struct _tmp3ED;struct Cyc_List_List*_tmp3EC;struct Cyc_Absyn_AccessEff_struct*
_tmp3EB;struct Cyc_Absyn_AccessEff_struct _tmp3EA;struct Cyc_List_List*_tmp3E9;
struct Cyc_Absyn_JoinEff_struct*_tmp3E8;_tmp22A=(void*)((_tmp3E8=_cycalloc(
sizeof(*_tmp3E8)),((_tmp3E8[0]=((_tmp3ED.tag=24,((_tmp3ED.f1=((_tmp3E9=_cycalloc(
sizeof(*_tmp3E9)),((_tmp3E9->hd=(void*)((_tmp3EB=_cycalloc(sizeof(*_tmp3EB)),((
_tmp3EB[0]=((_tmp3EA.tag=23,((_tmp3EA.f1=(void*)block_typ,_tmp3EA)))),_tmp3EB)))),((
_tmp3E9->tl=((_tmp3EC=_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC->hd=_tmp22A,((
_tmp3EC->tl=0,_tmp3EC)))))),_tmp3E9)))))),_tmp3ED)))),_tmp3E8))));}_tmp22B=
block_typ;{struct Cyc_Tcenv_Fenv*_tmp3EE;struct Cyc_Tcenv_Fenv*_tmp234=(_tmp3EE=
_region_malloc(r,sizeof(*_tmp3EE)),((_tmp3EE->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp224,((_tmp3EE->type_vars=(struct Cyc_List_List*)_tmp225,((_tmp3EE->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp226,((_tmp3EE->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp227),((_tmp3EE->encloser=(struct Cyc_Absyn_Stmt*)
_tmp228,((_tmp3EE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp229),((_tmp3EE->capability=(void*)_tmp22A,((_tmp3EE->curr_rgn=(void*)_tmp22B,((
_tmp3EE->in_notreadctxt=(int)_tmp22C,((_tmp3EE->fnrgn=(struct _RegionHandle*)r,
_tmp3EE)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmp234);};};}static
struct Cyc_Absyn_Eq_kb_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te){struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);return Cyc_Tcenv_new_named_block(r,loc,te,t);}
struct _tuple12{void*f1;void*f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc){struct Cyc_Tcenv_SharedFenv*_tmp23C;struct Cyc_List_List*_tmp23D;struct Cyc_RgnOrder_RgnPO*
_tmp23E;const struct Cyc_Tcenv_Bindings*_tmp23F;struct Cyc_Absyn_Stmt*_tmp240;
struct Cyc_Tcenv_CtrlEnv*_tmp241;void*_tmp242;void*_tmp243;int _tmp244;const char*
_tmp3EF;struct Cyc_Tcenv_Fenv _tmp23B=*Cyc_Tcenv_get_fenv(te,((_tmp3EF="new_outlives_constraints",
_tag_dyneither(_tmp3EF,sizeof(char),25))));_tmp23C=_tmp23B.shared;_tmp23D=
_tmp23B.type_vars;_tmp23E=_tmp23B.region_order;_tmp23F=_tmp23B.locals;_tmp240=
_tmp23B.encloser;_tmp241=_tmp23B.ctrl_env;_tmp242=_tmp23B.capability;_tmp243=
_tmp23B.curr_rgn;_tmp244=_tmp23B.in_notreadctxt;for(0;cs != 0;cs=cs->tl){_tmp23E=
Cyc_RgnOrder_add_outlives_constraint(_tmp23C->frgn,_tmp23E,(*((struct _tuple12*)
cs->hd)).f1,(*((struct _tuple12*)cs->hd)).f2,loc);}{struct Cyc_Tcenv_Fenv*_tmp3F0;
struct Cyc_Tcenv_Fenv*_tmp245=(_tmp3F0=_region_malloc(r,sizeof(*_tmp3F0)),((
_tmp3F0->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp23C,((_tmp3F0->type_vars=(
struct Cyc_List_List*)_tmp23D,((_tmp3F0->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp23E,((_tmp3F0->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)
_tmp23F),((_tmp3F0->encloser=(struct Cyc_Absyn_Stmt*)_tmp240,((_tmp3F0->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp241),((_tmp3F0->capability=(
void*)_tmp242,((_tmp3F0->curr_rgn=(void*)_tmp243,((_tmp3F0->in_notreadctxt=(int)
_tmp244,((_tmp3F0->fnrgn=(struct _RegionHandle*)r,_tmp3F0)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp245);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple10**
oldtv,struct Cyc_Position_Segment*loc){void*_tmp247=Cyc_Tcutil_compress(r1);void*
_tmp248=Cyc_Tcutil_compress(r2);struct Cyc_Absyn_Kind*_tmp249=Cyc_Tcutil_typ_kind(
_tmp247);struct Cyc_Absyn_Kind*_tmp24A=Cyc_Tcutil_typ_kind(_tmp248);int r1_le_r2=
Cyc_Tcutil_kind_leq(_tmp249,_tmp24A);int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp24A,
_tmp249);if(!r1_le_r2  && !r2_le_r1){{const char*_tmp3F7;void*_tmp3F6[4];struct Cyc_String_pa_struct
_tmp3F5;struct Cyc_String_pa_struct _tmp3F4;struct Cyc_String_pa_struct _tmp3F3;
struct Cyc_String_pa_struct _tmp3F2;(_tmp3F2.tag=0,((_tmp3F2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp24A)),((
_tmp3F3.tag=0,((_tmp3F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp249)),((_tmp3F4.tag=0,((_tmp3F4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp248)),((_tmp3F5.tag=0,((_tmp3F5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp247)),((
_tmp3F6[0]=& _tmp3F5,((_tmp3F6[1]=& _tmp3F4,((_tmp3F6[2]=& _tmp3F3,((_tmp3F6[3]=&
_tmp3F2,Cyc_Tcutil_terr(loc,((_tmp3F7="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",
_tag_dyneither(_tmp3F7,sizeof(char),82))),_tag_dyneither(_tmp3F6,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{if(r1_le_r2  && !
r2_le_r1){struct Cyc_Absyn_Tvar*_tmp252;void*_tmp253;struct _tuple10 _tmp251=Cyc_Tcutil_swap_kind(
_tmp248,Cyc_Tcutil_kind_to_bound(_tmp249));_tmp252=_tmp251.f1;_tmp253=_tmp251.f2;{
struct _tuple10*_tmp3F8;*oldtv=((_tmp3F8=_region_malloc(r,sizeof(*_tmp3F8)),((
_tmp3F8->f1=_tmp252,((_tmp3F8->f2=_tmp253,_tmp3F8))))));};}else{if(!r1_le_r2  && 
r2_le_r1){struct Cyc_Absyn_Tvar*_tmp256;void*_tmp257;struct _tuple10 _tmp255=Cyc_Tcutil_swap_kind(
_tmp247,Cyc_Tcutil_kind_to_bound(_tmp24A));_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;{
struct _tuple10*_tmp3F9;*oldtv=((_tmp3F9=_region_malloc(r,sizeof(*_tmp3F9)),((
_tmp3F9->f1=_tmp256,((_tmp3F9->f2=_tmp257,_tmp3F9))))));};}}}{struct
_RegionHandle*_tmp259=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp25A=0;if(
_tmp247 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp247 != (void*)& Cyc_Absyn_UniqueRgn_val){
struct Cyc_Absyn_AccessEff_struct _tmp3FC;struct Cyc_Absyn_AccessEff_struct*_tmp3FB;
void*eff1=(void*)((_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB[0]=((_tmp3FC.tag=
23,((_tmp3FC.f1=(void*)_tmp247,_tmp3FC)))),_tmp3FB))));struct _tuple12*_tmp3FF;
struct Cyc_List_List*_tmp3FE;_tmp25A=((_tmp3FE=_region_malloc(_tmp259,sizeof(*
_tmp3FE)),((_tmp3FE->hd=((_tmp3FF=_region_malloc(_tmp259,sizeof(*_tmp3FF)),((
_tmp3FF->f1=eff1,((_tmp3FF->f2=_tmp248,_tmp3FF)))))),((_tmp3FE->tl=_tmp25A,
_tmp3FE))))));}if(_tmp248 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp248 != (void*)&
Cyc_Absyn_UniqueRgn_val){struct Cyc_Absyn_AccessEff_struct _tmp402;struct Cyc_Absyn_AccessEff_struct*
_tmp401;void*eff2=(void*)((_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401[0]=((
_tmp402.tag=23,((_tmp402.f1=(void*)_tmp248,_tmp402)))),_tmp401))));struct
_tuple12*_tmp405;struct Cyc_List_List*_tmp404;_tmp25A=((_tmp404=_region_malloc(
_tmp259,sizeof(*_tmp404)),((_tmp404->hd=((_tmp405=_region_malloc(_tmp259,sizeof(*
_tmp405)),((_tmp405->f1=eff2,((_tmp405->f2=_tmp247,_tmp405)))))),((_tmp404->tl=
_tmp25A,_tmp404))))));}return Cyc_Tcenv_new_outlives_constraints((struct
_RegionHandle*)_tmp259,te,_tmp25A,loc);};}void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmp263=te->le;if(_tmp263 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)_tmp263;struct Cyc_Tcenv_Fenv
_tmp265;void*_tmp266;struct Cyc_Tcenv_Fenv*_tmp264=fe;_tmp265=*_tmp264;_tmp266=
_tmp265.curr_rgn;return _tmp266;};}void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*rgn){const char*_tmp406;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_get_fenv(te,((_tmp406="check_rgn_accessible",_tag_dyneither(_tmp406,
sizeof(char),21))));struct Cyc_Tcenv_Fenv _tmp268;void*_tmp269;struct Cyc_RgnOrder_RgnPO*
_tmp26A;struct Cyc_Tcenv_Fenv*_tmp267=fe;_tmp268=*_tmp267;_tmp269=_tmp268.capability;
_tmp26A=_tmp268.region_order;if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp269) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp269))return;{struct Cyc_Absyn_AccessEff_struct
_tmp409;struct Cyc_Absyn_AccessEff_struct*_tmp408;if(Cyc_RgnOrder_eff_outlives_eff(
_tmp26A,(void*)((_tmp408=_cycalloc(sizeof(*_tmp408)),((_tmp408[0]=((_tmp409.tag=
23,((_tmp409.f1=(void*)rgn,_tmp409)))),_tmp408)))),_tmp269))return;}{const char*
_tmp40D;void*_tmp40C[1];struct Cyc_String_pa_struct _tmp40B;(_tmp40B.tag=0,((
_tmp40B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
rgn)),((_tmp40C[0]=& _tmp40B,Cyc_Tcutil_terr(loc,((_tmp40D="Expression accesses unavailable region %s",
_tag_dyneither(_tmp40D,sizeof(char),42))),_tag_dyneither(_tmp40C,sizeof(void*),1)))))));};}
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*rgn){Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{struct Cyc_Tcenv_Fenv
_tmp273;struct Cyc_RgnOrder_RgnPO*_tmp274;const char*_tmp40E;struct Cyc_Tcenv_Fenv*
_tmp272=Cyc_Tcenv_get_fenv(te,((_tmp40E="check_rgn_resetable",_tag_dyneither(
_tmp40E,sizeof(char),20))));_tmp273=*_tmp272;_tmp274=_tmp273.region_order;{void*
_tmp275=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp277;_LL99: {struct Cyc_Absyn_VarType_struct*
_tmp276=(struct Cyc_Absyn_VarType_struct*)_tmp275;if(_tmp276->tag != 2)goto _LL9B;
else{_tmp277=_tmp276->f1;}}_LL9A: if(!Cyc_RgnOrder_is_region_resetable(_tmp274,
_tmp277)){const char*_tmp412;void*_tmp411[1];struct Cyc_String_pa_struct _tmp410;(
_tmp410.tag=0,((_tmp410.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
rgn)),((_tmp411[0]=& _tmp410,Cyc_Tcutil_terr(loc,((_tmp412="Region %s is not resetable",
_tag_dyneither(_tmp412,sizeof(char),27))),_tag_dyneither(_tmp411,sizeof(void*),1)))))));}
return;_LL9B:;_LL9C: {const char*_tmp415;void*_tmp414;(_tmp414=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp415="check_rgn_resetable",
_tag_dyneither(_tmp415,sizeof(char),20))),_tag_dyneither(_tmp414,sizeof(void*),0)));}
_LL98:;};};}int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*
rt_b){struct Cyc_Tcenv_Fenv*_tmp27D=te->le;rt_a=Cyc_Tcutil_compress(rt_a);rt_b=
Cyc_Tcutil_compress(rt_b);if(_tmp27D == 0)return rt_a == (void*)& Cyc_Absyn_UniqueRgn_val
 || rt_a == (void*)& Cyc_Absyn_HeapRgn_val  && rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;{
struct Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)_tmp27D;struct Cyc_Tcenv_Fenv
_tmp27F;struct Cyc_RgnOrder_RgnPO*_tmp280;struct Cyc_Tcenv_Fenv*_tmp27E=fe;_tmp27F=*
_tmp27E;_tmp280=_tmp27F.region_order;{struct Cyc_Absyn_AccessEff_struct _tmp418;
struct Cyc_Absyn_AccessEff_struct*_tmp417;int _tmp281=Cyc_RgnOrder_effect_outlives(
_tmp280,(void*)((_tmp417=_cycalloc(sizeof(*_tmp417)),((_tmp417[0]=((_tmp418.tag=
23,((_tmp418.f1=(void*)rt_a,_tmp418)))),_tmp417)))),rt_b);return _tmp281;};};}
struct _tuple13{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;struct Cyc_Position_Segment*
f4;};void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*eff){struct Cyc_Tcenv_Fenv _tmp286;void*_tmp287;struct Cyc_RgnOrder_RgnPO*
_tmp288;struct Cyc_Tcenv_SharedFenv*_tmp289;const char*_tmp419;struct Cyc_Tcenv_Fenv*
_tmp285=Cyc_Tcenv_get_fenv(te,((_tmp419="check_effect_accessible",_tag_dyneither(
_tmp419,sizeof(char),24))));_tmp286=*_tmp285;_tmp287=_tmp286.capability;_tmp288=
_tmp286.region_order;_tmp289=_tmp286.shared;if(Cyc_Tcutil_subset_effect(0,eff,
_tmp287))return;if(Cyc_RgnOrder_eff_outlives_eff(_tmp288,eff,_tmp287))return;{
struct _RegionHandle*frgn=_tmp289->frgn;struct _tuple13*_tmp41C;struct Cyc_List_List*
_tmp41B;_tmp289->delayed_effect_checks=((_tmp41B=_region_malloc(frgn,sizeof(*
_tmp41B)),((_tmp41B->hd=((_tmp41C=_region_malloc(frgn,sizeof(*_tmp41C)),((
_tmp41C->f1=_tmp287,((_tmp41C->f2=eff,((_tmp41C->f3=_tmp288,((_tmp41C->f4=loc,
_tmp41C)))))))))),((_tmp41B->tl=_tmp289->delayed_effect_checks,_tmp41B))))));};}
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv
_tmp28E;struct Cyc_Tcenv_SharedFenv*_tmp28F;const char*_tmp41D;struct Cyc_Tcenv_Fenv*
_tmp28D=Cyc_Tcenv_get_fenv(te,((_tmp41D="check_delayed_effects",_tag_dyneither(
_tmp41D,sizeof(char),22))));_tmp28E=*_tmp28D;_tmp28F=_tmp28E.shared;{struct Cyc_List_List*
_tmp290=_tmp28F->delayed_effect_checks;for(0;_tmp290 != 0;_tmp290=_tmp290->tl){
struct _tuple13 _tmp292;void*_tmp293;void*_tmp294;struct Cyc_RgnOrder_RgnPO*_tmp295;
struct Cyc_Position_Segment*_tmp296;struct _tuple13*_tmp291=(struct _tuple13*)
_tmp290->hd;_tmp292=*_tmp291;_tmp293=_tmp292.f1;_tmp294=_tmp292.f2;_tmp295=
_tmp292.f3;_tmp296=_tmp292.f4;if(Cyc_Tcutil_subset_effect(1,_tmp294,_tmp293))
continue;if(Cyc_RgnOrder_eff_outlives_eff(_tmp295,_tmp294,_tmp293))continue;{
const char*_tmp422;void*_tmp421[2];struct Cyc_String_pa_struct _tmp420;struct Cyc_String_pa_struct
_tmp41F;(_tmp41F.tag=0,((_tmp41F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp294)),((_tmp420.tag=0,((_tmp420.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp293)),((_tmp421[
0]=& _tmp420,((_tmp421[1]=& _tmp41F,Cyc_Tcutil_terr(_tmp296,((_tmp422="Capability \n%s\ndoes not cover function's effect\n%s",
_tag_dyneither(_tmp422,sizeof(char),51))),_tag_dyneither(_tmp421,sizeof(void*),2)))))))))))));};}};}
struct _tuple14{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;struct Cyc_Position_Segment*
f3;};void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*po){struct Cyc_Tcenv_Fenv*_tmp29B=te->le;if(_tmp29B == 0){
for(0;po != 0;po=po->tl){struct Cyc_Absyn_AccessEff_struct _tmp425;struct Cyc_Absyn_AccessEff_struct*
_tmp424;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple12*)po->hd)).f1,Cyc_Absyn_empty_effect)
 || !Cyc_Tcutil_subset_effect(1,(void*)((_tmp424=_cycalloc(sizeof(*_tmp424)),((
_tmp424[0]=((_tmp425.tag=23,((_tmp425.f1=(void*)(*((struct _tuple12*)po->hd)).f2,
_tmp425)))),_tmp424)))),Cyc_Absyn_empty_effect)){const char*_tmp428;void*_tmp427;(
_tmp427=0,Cyc_Tcutil_terr(loc,((_tmp428="the required region ordering is not satisfied here",
_tag_dyneither(_tmp428,sizeof(char),51))),_tag_dyneither(_tmp427,sizeof(void*),0)));}}
return;}{struct Cyc_Tcenv_Fenv _tmp2A1;struct Cyc_RgnOrder_RgnPO*_tmp2A2;struct Cyc_Tcenv_SharedFenv*
_tmp2A3;struct Cyc_Tcenv_Fenv*_tmp2A0=(struct Cyc_Tcenv_Fenv*)_tmp29B;_tmp2A1=*
_tmp2A0;_tmp2A2=_tmp2A1.region_order;_tmp2A3=_tmp2A1.shared;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp2A2,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){struct _tuple14*_tmp42B;struct Cyc_List_List*
_tmp42A;_tmp2A3->delayed_constraint_checks=((_tmp42A=_region_malloc(_tmp2A3->frgn,
sizeof(*_tmp42A)),((_tmp42A->hd=((_tmp42B=_region_malloc(_tmp2A3->frgn,sizeof(*
_tmp42B)),((_tmp42B->f1=_tmp2A2,((_tmp42B->f2=po,((_tmp42B->f3=loc,_tmp42B)))))))),((
_tmp42A->tl=_tmp2A3->delayed_constraint_checks,_tmp42A))))));}};}void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp2A8;struct Cyc_Tcenv_SharedFenv*
_tmp2A9;const char*_tmp42C;struct Cyc_Tcenv_Fenv*_tmp2A7=Cyc_Tcenv_get_fenv(te,((
_tmp42C="check_delayed_constraints",_tag_dyneither(_tmp42C,sizeof(char),26))));
_tmp2A8=*_tmp2A7;_tmp2A9=_tmp2A8.shared;{struct Cyc_List_List*_tmp2AA=_tmp2A9->delayed_constraint_checks;
for(0;_tmp2AA != 0;_tmp2AA=_tmp2AA->tl){struct _tuple14 _tmp2AC;struct Cyc_RgnOrder_RgnPO*
_tmp2AD;struct Cyc_List_List*_tmp2AE;struct Cyc_Position_Segment*_tmp2AF;struct
_tuple14*_tmp2AB=(struct _tuple14*)_tmp2AA->hd;_tmp2AC=*_tmp2AB;_tmp2AD=_tmp2AC.f1;
_tmp2AE=_tmp2AC.f2;_tmp2AF=_tmp2AC.f3;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp2AD,_tmp2AE,(void*)& Cyc_Absyn_HeapRgn_val,1)){const char*_tmp42F;void*_tmp42E;(
_tmp42E=0,Cyc_Tcutil_terr(_tmp2AF,((_tmp42F="the required region ordering is not satisfied here",
_tag_dyneither(_tmp42F,sizeof(char),51))),_tag_dyneither(_tmp42E,sizeof(void*),0)));}}};}
struct _tuple15{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct
Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Fndecl*fd){const struct Cyc_Tcenv_Bindings*locals=(const struct
Cyc_Tcenv_Bindings*)0;struct _dyneither_ptr*_tmp43C;const char*_tmp43B;void*
_tmp43A[1];struct Cyc_String_pa_struct _tmp439;struct Cyc_Absyn_Tvar*_tmp438;struct
Cyc_Absyn_Tvar*rgn0=(_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438->name=((
_tmp43C=_cycalloc(sizeof(*_tmp43C)),((_tmp43C[0]=(struct _dyneither_ptr)((_tmp439.tag=
0,((_tmp439.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((
_tmp43A[0]=& _tmp439,Cyc_aprintf(((_tmp43B="`%s",_tag_dyneither(_tmp43B,sizeof(
char),4))),_tag_dyneither(_tmp43A,sizeof(void*),1)))))))),_tmp43C)))),((_tmp438->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp438->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp438)))))));
struct Cyc_List_List*_tmp43D;struct Cyc_List_List*_tmp2B2=(_tmp43D=_cycalloc(
sizeof(*_tmp43D)),((_tmp43D->hd=rgn0,((_tmp43D->tl=fd->tvs,_tmp43D)))));Cyc_Tcutil_check_unique_tvars(
loc,_tmp2B2);{struct Cyc_RgnOrder_RgnPO*_tmp2B3=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,
fd->rgn_po,(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,rgn0,loc);
struct Cyc_Absyn_VarType_struct _tmp440;struct Cyc_Absyn_VarType_struct*_tmp43F;
void*param_rgn=(void*)((_tmp43F=_cycalloc(sizeof(*_tmp43F)),((_tmp43F[0]=((
_tmp440.tag=2,((_tmp440.f1=rgn0,_tmp440)))),_tmp43F))));struct Cyc_Core_Opt*
_tmp441;struct Cyc_Core_Opt*param_rgn_opt=(_tmp441=_cycalloc(sizeof(*_tmp441)),((
_tmp441->v=param_rgn,_tmp441)));struct Cyc_List_List*_tmp2B4=0;{struct Cyc_List_List*
_tmp2B5=fd->args;for(0;_tmp2B5 != 0;_tmp2B5=_tmp2B5->tl){struct Cyc_Absyn_Vardecl
_tmp447;struct _tuple0*_tmp446;struct Cyc_Absyn_Vardecl*_tmp445;struct Cyc_Absyn_Vardecl*
_tmp2B6=(_tmp445=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp445[0]=((
_tmp447.sc=Cyc_Absyn_Public,((_tmp447.name=((_tmp446=_cycalloc(sizeof(*_tmp446)),((
_tmp446->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp446->f2=(*((struct
_tuple15*)_tmp2B5->hd)).f1,_tmp446)))))),((_tmp447.tq=(*((struct _tuple15*)
_tmp2B5->hd)).f2,((_tmp447.type=(*((struct _tuple15*)_tmp2B5->hd)).f3,((_tmp447.initializer=
0,((_tmp447.rgn=param_rgn_opt,((_tmp447.attributes=0,((_tmp447.escapes=0,_tmp447)))))))))))))))),
_tmp445)));{struct Cyc_List_List _tmp44A;struct Cyc_List_List*_tmp449;_tmp2B4=((
_tmp449=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp449[0]=((_tmp44A.hd=
_tmp2B6,((_tmp44A.tl=_tmp2B4,_tmp44A)))),_tmp449))));}{struct Cyc_Absyn_Param_b_struct*
_tmp450;struct Cyc_Absyn_Param_b_struct _tmp44F;struct Cyc_Tcenv_Bindings*_tmp44E;
locals=(const struct Cyc_Tcenv_Bindings*)((_tmp44E=_region_malloc(r,sizeof(*
_tmp44E)),((_tmp44E->v=(*((struct _tuple15*)_tmp2B5->hd)).f1,((_tmp44E->b=(void*)((
_tmp450=_cycalloc(sizeof(*_tmp450)),((_tmp450[0]=((_tmp44F.tag=3,((_tmp44F.f1=
_tmp2B6,_tmp44F)))),_tmp450)))),((_tmp44E->tl=locals,_tmp44E))))))));};}}if(fd->cyc_varargs
!= 0){struct Cyc_Core_Opt*_tmp2C0;struct Cyc_Absyn_Tqual _tmp2C1;void*_tmp2C2;int
_tmp2C3;struct Cyc_Absyn_VarargInfo _tmp2BF=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp2C0=_tmp2BF.name;_tmp2C1=_tmp2BF.tq;_tmp2C2=
_tmp2BF.type;_tmp2C3=_tmp2BF.inject;if(_tmp2C0 != 0){void*_tmp2C4=Cyc_Absyn_dyneither_typ(
_tmp2C2,param_rgn,_tmp2C1,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl _tmp456;
struct _tuple0*_tmp455;struct Cyc_Absyn_Vardecl*_tmp454;struct Cyc_Absyn_Vardecl*
_tmp2C5=(_tmp454=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp454[0]=((
_tmp456.sc=Cyc_Absyn_Public,((_tmp456.name=((_tmp455=_cycalloc(sizeof(*_tmp455)),((
_tmp455->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp455->f2=(struct
_dyneither_ptr*)_tmp2C0->v,_tmp455)))))),((_tmp456.tq=Cyc_Absyn_empty_tqual(0),((
_tmp456.type=_tmp2C4,((_tmp456.initializer=0,((_tmp456.rgn=param_rgn_opt,((
_tmp456.attributes=0,((_tmp456.escapes=0,_tmp456)))))))))))))))),_tmp454)));{
struct Cyc_List_List*_tmp457;_tmp2B4=((_tmp457=_cycalloc(sizeof(*_tmp457)),((
_tmp457->hd=_tmp2C5,((_tmp457->tl=_tmp2B4,_tmp457))))));}{struct Cyc_Absyn_Param_b_struct*
_tmp45D;struct Cyc_Absyn_Param_b_struct _tmp45C;struct Cyc_Tcenv_Bindings*_tmp45B;
locals=(const struct Cyc_Tcenv_Bindings*)((_tmp45B=_region_malloc(r,sizeof(*
_tmp45B)),((_tmp45B->v=(struct _dyneither_ptr*)_tmp2C0->v,((_tmp45B->b=(void*)((
_tmp45D=_cycalloc(sizeof(*_tmp45D)),((_tmp45D[0]=((_tmp45C.tag=3,((_tmp45C.f1=
_tmp2C5,_tmp45C)))),_tmp45D)))),((_tmp45B->tl=locals,_tmp45B))))))));};}else{
const char*_tmp460;void*_tmp45F;(_tmp45F=0,Cyc_Tcutil_terr(loc,((_tmp460="missing name for varargs",
_tag_dyneither(_tmp460,sizeof(char),25))),_tag_dyneither(_tmp45F,sizeof(void*),0)));}}{
struct Cyc_Core_Opt _tmp463;struct Cyc_Core_Opt*_tmp462;fd->param_vardecls=((
_tmp462=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp462[0]=((_tmp463.v=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2B4),
_tmp463)),_tmp462))));}{struct Cyc_Absyn_JoinEff_struct*_tmp481;struct Cyc_List_List*
_tmp480;struct Cyc_Absyn_AccessEff_struct _tmp47F;struct Cyc_Absyn_AccessEff_struct*
_tmp47E;struct Cyc_List_List*_tmp47D;struct Cyc_Absyn_JoinEff_struct _tmp47C;struct
Cyc_Tcenv_CtrlEnv*_tmp47B;struct Cyc_Tcenv_SharedFenv*_tmp47A;struct Cyc_Tcenv_Fenv*
_tmp479;return(_tmp479=_region_malloc(r,sizeof(*_tmp479)),((_tmp479->shared=(
struct Cyc_Tcenv_SharedFenv*)((_tmp47A=_region_malloc(r,sizeof(*_tmp47A)),((
_tmp47A->frgn=r,((_tmp47A->return_typ=fd->ret_type,((_tmp47A->seen_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp47A->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp47A->delayed_effect_checks=
0,((_tmp47A->delayed_constraint_checks=0,_tmp47A)))))))))))))),((_tmp479->type_vars=(
struct Cyc_List_List*)_tmp2B2,((_tmp479->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp2B3,((_tmp479->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp479->encloser=(
struct Cyc_Absyn_Stmt*)fd->body,((_tmp479->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
_tmp47B=_region_malloc(r,sizeof(*_tmp47B)),((_tmp47B->ctrl_rgn=r,((_tmp47B->continue_stmt=(
void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp47B->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((
_tmp47B->fallthru_clause=(const struct _tuple9*)0,((_tmp47B->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((
_tmp47B->try_depth=0,_tmp47B)))))))))))))),((_tmp479->capability=(void*)((void*)((
_tmp481=_cycalloc(sizeof(*_tmp481)),((_tmp481[0]=((_tmp47C.tag=24,((_tmp47C.f1=((
_tmp480=_cycalloc(sizeof(*_tmp480)),((_tmp480->hd=(void*)((_tmp47E=_cycalloc(
sizeof(*_tmp47E)),((_tmp47E[0]=((_tmp47F.tag=23,((_tmp47F.f1=(void*)param_rgn,
_tmp47F)))),_tmp47E)))),((_tmp480->tl=((_tmp47D=_cycalloc(sizeof(*_tmp47D)),((
_tmp47D->hd=(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,((_tmp47D->tl=
0,_tmp47D)))))),_tmp480)))))),_tmp47C)))),_tmp481))))),((_tmp479->curr_rgn=(void*)
param_rgn,((_tmp479->in_notreadctxt=(int)0,((_tmp479->fnrgn=(struct _RegionHandle*)
r,_tmp479)))))))))))))))))))));};};}struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*
fd){struct Cyc_Tcenv_Fenv _tmp2E4;const struct Cyc_Tcenv_Bindings*_tmp2E5;struct Cyc_RgnOrder_RgnPO*
_tmp2E6;struct Cyc_List_List*_tmp2E7;struct Cyc_Tcenv_SharedFenv*_tmp2E8;struct
_RegionHandle*_tmp2E9;struct Cyc_Tcenv_Fenv*_tmp2E3=old_fenv;_tmp2E4=*_tmp2E3;
_tmp2E5=_tmp2E4.locals;_tmp2E6=_tmp2E4.region_order;_tmp2E7=_tmp2E4.type_vars;
_tmp2E8=_tmp2E4.shared;_tmp2E9=_tmp2E4.fnrgn;{struct _RegionHandle*_tmp2EA=
_tmp2E8->frgn;const struct Cyc_Tcenv_Bindings*_tmp2EB=(const struct Cyc_Tcenv_Bindings*)
_tmp2E5;struct _dyneither_ptr*_tmp48E;const char*_tmp48D;void*_tmp48C[1];struct Cyc_String_pa_struct
_tmp48B;struct Cyc_Absyn_Tvar*_tmp48A;struct Cyc_Absyn_Tvar*rgn0=(_tmp48A=
_cycalloc(sizeof(*_tmp48A)),((_tmp48A->name=((_tmp48E=_cycalloc(sizeof(*_tmp48E)),((
_tmp48E[0]=(struct _dyneither_ptr)((_tmp48B.tag=0,((_tmp48B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp48C[0]=& _tmp48B,Cyc_aprintf(((
_tmp48D="`%s",_tag_dyneither(_tmp48D,sizeof(char),4))),_tag_dyneither(_tmp48C,
sizeof(void*),1)))))))),_tmp48E)))),((_tmp48A->identity=Cyc_Tcutil_new_tvar_id(),((
_tmp48A->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp48A)))))));{struct Cyc_List_List*
_tmp2EC=fd->tvs;for(0;_tmp2EC != 0;_tmp2EC=_tmp2EC->tl){struct Cyc_Absyn_Kind
_tmp2EE;enum Cyc_Absyn_KindQual _tmp2EF;enum Cyc_Absyn_AliasQual _tmp2F0;struct Cyc_Absyn_Kind*
_tmp2ED=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2EC->hd,& Cyc_Tcutil_bk);
_tmp2EE=*_tmp2ED;_tmp2EF=_tmp2EE.kind;_tmp2F0=_tmp2EE.aliasqual;if(_tmp2EF == Cyc_Absyn_RgnKind){
if(_tmp2F0 == Cyc_Absyn_Aliasable)_tmp2E6=Cyc_RgnOrder_add_youngest(_tmp2EA,
_tmp2E6,(struct Cyc_Absyn_Tvar*)_tmp2EC->hd,0,0);else{const char*_tmp491;void*
_tmp490;(_tmp490=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp491="non-intuitionistic tvar in nested_fenv",_tag_dyneither(_tmp491,sizeof(
char),39))),_tag_dyneither(_tmp490,sizeof(void*),0)));}}}}_tmp2E6=Cyc_RgnOrder_add_youngest(
_tmp2EA,_tmp2E6,rgn0,0,0);{struct Cyc_List_List*_tmp492;struct Cyc_List_List*
_tmp2F3=(_tmp492=_cycalloc(sizeof(*_tmp492)),((_tmp492->hd=rgn0,((_tmp492->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
fd->tvs,_tmp2E7),_tmp492)))));Cyc_Tcutil_check_unique_tvars(loc,_tmp2F3);{struct
Cyc_Absyn_VarType_struct _tmp495;struct Cyc_Absyn_VarType_struct*_tmp494;void*
param_rgn=(void*)((_tmp494=_cycalloc(sizeof(*_tmp494)),((_tmp494[0]=((_tmp495.tag=
2,((_tmp495.f1=rgn0,_tmp495)))),_tmp494))));struct Cyc_Core_Opt*_tmp496;struct Cyc_Core_Opt*
param_rgn_opt=(_tmp496=_cycalloc(sizeof(*_tmp496)),((_tmp496->v=param_rgn,
_tmp496)));struct Cyc_List_List*_tmp2F4=0;{struct Cyc_List_List*_tmp2F5=fd->args;
for(0;_tmp2F5 != 0;_tmp2F5=_tmp2F5->tl){struct Cyc_Absyn_Vardecl _tmp49C;struct
_tuple0*_tmp49B;struct Cyc_Absyn_Vardecl*_tmp49A;struct Cyc_Absyn_Vardecl*_tmp2F6=(
_tmp49A=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp49A[0]=((_tmp49C.sc=
Cyc_Absyn_Public,((_tmp49C.name=((_tmp49B=_cycalloc(sizeof(*_tmp49B)),((_tmp49B->f1=(
union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp49B->f2=(*((struct _tuple15*)_tmp2F5->hd)).f1,
_tmp49B)))))),((_tmp49C.tq=(*((struct _tuple15*)_tmp2F5->hd)).f2,((_tmp49C.type=(*((
struct _tuple15*)_tmp2F5->hd)).f3,((_tmp49C.initializer=0,((_tmp49C.rgn=
param_rgn_opt,((_tmp49C.attributes=0,((_tmp49C.escapes=0,_tmp49C)))))))))))))))),
_tmp49A)));{struct Cyc_List_List _tmp49F;struct Cyc_List_List*_tmp49E;_tmp2F4=((
_tmp49E=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp49E[0]=((_tmp49F.hd=
_tmp2F6,((_tmp49F.tl=_tmp2F4,_tmp49F)))),_tmp49E))));}{struct Cyc_Absyn_Param_b_struct*
_tmp4A5;struct Cyc_Absyn_Param_b_struct _tmp4A4;struct Cyc_Tcenv_Bindings*_tmp4A3;
_tmp2EB=(const struct Cyc_Tcenv_Bindings*)((_tmp4A3=_region_malloc(_tmp2EA,sizeof(*
_tmp4A3)),((_tmp4A3->v=(*((struct _tuple15*)_tmp2F5->hd)).f1,((_tmp4A3->b=(void*)((
_tmp4A5=_cycalloc(sizeof(*_tmp4A5)),((_tmp4A5[0]=((_tmp4A4.tag=3,((_tmp4A4.f1=
_tmp2F6,_tmp4A4)))),_tmp4A5)))),((_tmp4A3->tl=_tmp2EB,_tmp4A3))))))));};}}if(fd->cyc_varargs
!= 0){struct Cyc_Core_Opt*_tmp300;struct Cyc_Absyn_Tqual _tmp301;void*_tmp302;int
_tmp303;struct Cyc_Absyn_VarargInfo _tmp2FF=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp300=_tmp2FF.name;_tmp301=_tmp2FF.tq;_tmp302=
_tmp2FF.type;_tmp303=_tmp2FF.inject;if(_tmp300 != 0){void*_tmp304=Cyc_Absyn_dyneither_typ(
_tmp302,param_rgn,_tmp301,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl _tmp4AB;
struct _tuple0*_tmp4AA;struct Cyc_Absyn_Vardecl*_tmp4A9;struct Cyc_Absyn_Vardecl*
_tmp305=(_tmp4A9=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4A9[0]=((
_tmp4AB.sc=Cyc_Absyn_Public,((_tmp4AB.name=((_tmp4AA=_cycalloc(sizeof(*_tmp4AA)),((
_tmp4AA->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp4AA->f2=(struct
_dyneither_ptr*)_tmp300->v,_tmp4AA)))))),((_tmp4AB.tq=Cyc_Absyn_empty_tqual(0),((
_tmp4AB.type=_tmp304,((_tmp4AB.initializer=0,((_tmp4AB.rgn=param_rgn_opt,((
_tmp4AB.attributes=0,((_tmp4AB.escapes=0,_tmp4AB)))))))))))))))),_tmp4A9)));{
struct Cyc_List_List*_tmp4AC;_tmp2F4=((_tmp4AC=_cycalloc(sizeof(*_tmp4AC)),((
_tmp4AC->hd=_tmp305,((_tmp4AC->tl=_tmp2F4,_tmp4AC))))));}{struct Cyc_Absyn_Param_b_struct*
_tmp4B2;struct Cyc_Absyn_Param_b_struct _tmp4B1;struct Cyc_Tcenv_Bindings*_tmp4B0;
_tmp2EB=(const struct Cyc_Tcenv_Bindings*)((_tmp4B0=_region_malloc(_tmp2EA,sizeof(*
_tmp4B0)),((_tmp4B0->v=(struct _dyneither_ptr*)_tmp300->v,((_tmp4B0->b=(void*)((
_tmp4B2=_cycalloc(sizeof(*_tmp4B2)),((_tmp4B2[0]=((_tmp4B1.tag=3,((_tmp4B1.f1=
_tmp305,_tmp4B1)))),_tmp4B2)))),((_tmp4B0->tl=_tmp2EB,_tmp4B0))))))));};}else{
const char*_tmp4B5;void*_tmp4B4;(_tmp4B4=0,Cyc_Tcutil_terr(loc,((_tmp4B5="missing name for varargs",
_tag_dyneither(_tmp4B5,sizeof(char),25))),_tag_dyneither(_tmp4B4,sizeof(void*),0)));}}{
struct Cyc_Core_Opt _tmp4B8;struct Cyc_Core_Opt*_tmp4B7;fd->param_vardecls=((
_tmp4B7=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4B7[0]=((_tmp4B8.v=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2F4),
_tmp4B8)),_tmp4B7))));}{struct Cyc_Absyn_JoinEff_struct*_tmp4D6;struct Cyc_List_List*
_tmp4D5;struct Cyc_Absyn_AccessEff_struct _tmp4D4;struct Cyc_Absyn_AccessEff_struct*
_tmp4D3;struct Cyc_List_List*_tmp4D2;struct Cyc_Absyn_JoinEff_struct _tmp4D1;struct
Cyc_Tcenv_CtrlEnv*_tmp4D0;struct Cyc_Tcenv_SharedFenv*_tmp4CF;struct Cyc_Tcenv_Fenv*
_tmp4CE;return(struct Cyc_Tcenv_Fenv*)((_tmp4CE=_region_malloc(_tmp2EA,sizeof(*
_tmp4CE)),((_tmp4CE->shared=(struct Cyc_Tcenv_SharedFenv*)((_tmp4CF=
_region_malloc(_tmp2EA,sizeof(*_tmp4CF)),((_tmp4CF->frgn=_tmp2EA,((_tmp4CF->return_typ=
fd->ret_type,((_tmp4CF->seen_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp2EA,
Cyc_strptrcmp),((_tmp4CF->needed_labels=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
_tmp2EA,Cyc_strptrcmp),((_tmp4CF->delayed_effect_checks=0,((_tmp4CF->delayed_constraint_checks=
0,_tmp4CF)))))))))))))),((_tmp4CE->type_vars=(struct Cyc_List_List*)_tmp2F3,((
_tmp4CE->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2E6,((_tmp4CE->locals=(
const struct Cyc_Tcenv_Bindings*)_tmp2EB,((_tmp4CE->encloser=(struct Cyc_Absyn_Stmt*)
fd->body,((_tmp4CE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)((
_tmp4D0=_region_malloc(_tmp2EA,sizeof(*_tmp4D0)),((_tmp4D0->ctrl_rgn=_tmp2EA,((
_tmp4D0->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4D0->break_stmt=(
void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4D0->fallthru_clause=(const struct _tuple9*)0,((
_tmp4D0->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp4D0->try_depth=0,_tmp4D0))))))))))))))),((
_tmp4CE->capability=(void*)((void*)((_tmp4D6=_cycalloc(sizeof(*_tmp4D6)),((
_tmp4D6[0]=((_tmp4D1.tag=24,((_tmp4D1.f1=((_tmp4D5=_cycalloc(sizeof(*_tmp4D5)),((
_tmp4D5->hd=(void*)((_tmp4D3=_cycalloc(sizeof(*_tmp4D3)),((_tmp4D3[0]=((_tmp4D4.tag=
23,((_tmp4D4.f1=(void*)param_rgn,_tmp4D4)))),_tmp4D3)))),((_tmp4D5->tl=((_tmp4D2=
_cycalloc(sizeof(*_tmp4D2)),((_tmp4D2->hd=(void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v,((_tmp4D2->tl=0,_tmp4D2)))))),_tmp4D5)))))),_tmp4D1)))),
_tmp4D6))))),((_tmp4CE->curr_rgn=(void*)param_rgn,((_tmp4CE->in_notreadctxt=(int)
0,((_tmp4CE->fnrgn=(struct _RegionHandle*)_tmp2E9,_tmp4CE))))))))))))))))))))));};};};};}
