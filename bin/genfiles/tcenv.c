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
 typedef char*Cyc_Cstring;typedef char*Cyc_CstringNN;typedef struct _dyneither_ptr Cyc_string_t;
typedef struct _dyneither_ptr Cyc_mstring_t;typedef struct _dyneither_ptr*Cyc_stringptr_t;
typedef struct _dyneither_ptr*Cyc_mstringptr_t;typedef char*Cyc_Cbuffer_t;typedef
char*Cyc_CbufferNN_t;typedef struct _dyneither_ptr Cyc_buffer_t;typedef struct
_dyneither_ptr Cyc_mbuffer_t;typedef int Cyc_bool;struct Cyc_Core_NewRegion{struct
_DynRegionHandle*dynregion;};typedef unsigned long Cyc_size_t;typedef
unsigned short Cyc_mode_t;struct Cyc___cycFILE;typedef struct Cyc___cycFILE Cyc_FILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};typedef void*Cyc_parg_t;struct _dyneither_ptr Cyc_aprintf(struct
_dyneither_ptr,struct _dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{
void*v;};typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[
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
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};typedef struct Cyc_List_List*
Cyc_List_list_t;typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;typedef const struct
Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};extern
char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict
Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_is_empty(
struct Cyc_Dict_Dict d);int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,
void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*Cyc_Set_set_t;struct Cyc_Set_Set*
Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rempty(struct
_RegionHandle*r,int(*cmp)(void*,void*));int Cyc_Set_member(struct Cyc_Set_Set*s,
void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};
struct Cyc_SlowDict_Dict;typedef struct Cyc_SlowDict_Dict*Cyc_SlowDict_dict_t;
extern char Cyc_SlowDict_Present[8];struct Cyc_SlowDict_Present_struct{char*tag;};
extern char Cyc_SlowDict_Absent[7];struct Cyc_SlowDict_Absent_struct{char*tag;};
struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;
int line_no;int col;};typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;typedef
struct Cyc_Position_Segment*Cyc_Position_seg_t;struct Cyc_Position_Lex_struct{int
tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern
char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*tag;};
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;typedef struct Cyc_PP_Out*
Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;typedef struct
_dyneither_ptr*Cyc_Absyn_field_name_t;typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;struct _union_Nmspace_Rel_n{int
tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};typedef union Cyc_Absyn_Nmspace
Cyc_Absyn_nmspace_t;union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*
Cyc_Absyn_qvar_opt_t;typedef struct _tuple0*Cyc_Absyn_typedef_name_t;typedef struct
_tuple0*Cyc_Absyn_typedef_name_opt_t;typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;typedef void*Cyc_Absyn_kindbound_t;
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;typedef void*Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;typedef struct Cyc_Absyn_PtrInfo
Cyc_Absyn_ptr_info_t;typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;typedef struct Cyc_Absyn_DatatypeInfo
Cyc_Absyn_datatype_info_t;typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;typedef struct Cyc_Absyn_ArrayInfo
Cyc_Absyn_array_info_t;typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;typedef struct Cyc_Absyn_VarargCallInfo
Cyc_Absyn_vararg_call_info_t;typedef void*Cyc_Absyn_raw_exp_t;typedef struct Cyc_Absyn_Exp*
Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;typedef void*Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
typedef void*Cyc_Absyn_raw_pat_t;typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
typedef void*Cyc_Absyn_binding_t;typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;typedef struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_aggrdecl_t;typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;typedef struct Cyc_Absyn_Typedefdecl*
Cyc_Absyn_typedefdecl_t;typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;typedef struct Cyc_Absyn_Vardecl*
Cyc_Absyn_vardecl_t;typedef void*Cyc_Absyn_raw_decl_t;typedef struct Cyc_Absyn_Decl*
Cyc_Absyn_decl_t;typedef void*Cyc_Absyn_designator_t;typedef void*Cyc_Absyn_absyn_annot_t;
typedef void*Cyc_Absyn_attribute_t;typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;typedef void*Cyc_Absyn_offsetof_field_t;
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;typedef enum Cyc_Absyn_Coercion
Cyc_Absyn_coercion_t;typedef struct Cyc_Absyn_PtrLoc*Cyc_Absyn_ptrloc_t;enum Cyc_Absyn_Scope{
Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern
 = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz
 = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};enum Cyc_Absyn_AliasQual{
Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};enum Cyc_Absyn_KindQual{
Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind
 = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual
kind;enum Cyc_Absyn_AliasQual aliasqual;};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 
0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA
 = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};
struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};
struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{
struct _union_Constraint_Eq_constr Eq_constr;struct
_union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr
No_constr;};typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_struct{
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
int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{int tag;};typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{
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
int tag;struct Cyc_Absyn_Datatypedecl*f1;};typedef void*Cyc_Absyn_raw_type_decl_t;
struct Cyc_Absyn_TypeDecl{void*r;struct Cyc_Position_Segment*loc;};typedef struct
Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_struct{int tag;
};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;
int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};
struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
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
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_struct Cyc_Absyn_RefCntRgn_val;struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};typedef void*Cyc_Absyn_funcparams_t;enum Cyc_Absyn_Format_Type{
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
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};typedef void*Cyc_Absyn_type_modifier_t;
struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;
char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct
_union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};
struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct
_union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;
struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct
_dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct
_union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};enum Cyc_Absyn_Primop{
Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod
 = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte
 = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand
 = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift
 = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{
Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec
 = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*
injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion
 = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_struct{int tag;union Cyc_Absyn_Cnst f1;};
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
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
char*tag;};int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);extern
union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int
add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;
int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct Cyc_RgnOrder_RgnPO;
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;struct Cyc_RgnOrder_RgnPO*
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
tl;};typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;typedef const struct Cyc_Tcenv_CList*
const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};typedef void*Cyc_Tcenv_resolved_t;
struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;
struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict
enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*
availables;};typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_NotLoop_j_struct{
int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{int tag;};struct Cyc_Tcenv_FnEnd_j_struct{
int tag;};struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};extern
struct Cyc_Tcenv_NotLoop_j_struct Cyc_Tcenv_NotLoop_j_val;extern struct Cyc_Tcenv_CaseEnd_j_struct
Cyc_Tcenv_CaseEnd_j_val;extern struct Cyc_Tcenv_FnEnd_j_struct Cyc_Tcenv_FnEnd_j_val;
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};typedef struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*);struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(
struct _RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Fenv*old_fenv,
struct Cyc_Absyn_Fndecl*new_fn);struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
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
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*
te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);struct
_tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*
f3;};const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**
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
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);struct _tuple11 Cyc_Tcutil_swap_kind(
void*t,void*kb);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*
e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t);char Cyc_Tcenv_Env_error[10]="Env_error";
struct Cyc_Tcenv_Env_error_struct{char*tag;};struct Cyc_Tcenv_Env_error_struct Cyc_Tcenv_Env_error_val={
Cyc_Tcenv_Env_error};void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){{const char*
_tmp358;void*_tmp357[1];struct Cyc_String_pa_struct _tmp356;(_tmp356.tag=0,((
_tmp356.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp357[0]=&
_tmp356,Cyc_fprintf(Cyc_stderr,((_tmp358="Internal error in tcenv: %s\n",
_tag_dyneither(_tmp358,sizeof(char),29))),_tag_dyneither(_tmp357,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}
struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;struct Cyc_Tcenv_NotLoop_j_struct Cyc_Tcenv_NotLoop_j_val={
0};struct Cyc_Tcenv_CaseEnd_j_struct Cyc_Tcenv_CaseEnd_j_val={1};struct Cyc_Tcenv_FnEnd_j_struct
Cyc_Tcenv_FnEnd_j_val={2};typedef const struct _tuple10*Cyc_Tcenv_ftclause_t;struct
Cyc_Tcenv_CtrlEnv{struct _RegionHandle*ctrl_rgn;void*continue_stmt;void*
break_stmt;const struct _tuple10*fallthru_clause;void*next_stmt;int try_depth;};
typedef struct Cyc_Tcenv_CtrlEnv*Cyc_Tcenv_ctrl_env_t;struct Cyc_Tcenv_SharedFenv{
struct _RegionHandle*frgn;void*return_typ;struct Cyc_Dict_Dict seen_labels;struct
Cyc_Dict_Dict needed_labels;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*
delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*
b;const struct Cyc_Tcenv_Bindings*tl;};typedef const struct Cyc_Tcenv_Bindings*const
Cyc_Tcenv_bindings_t;typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;
struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*
type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*
locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*
capability;void*curr_rgn;int in_notreadctxt;int in_lhs;struct _RegionHandle*fnrgn;};
char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_struct{char*
tag;};struct Cyc_Tcenv_NoBinding_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct
_dyneither_ptr*v){for(0;(unsigned int)bs;bs=bs->tl){if(Cyc_strptrcmp(v,bs->v)== 
0)return bs->b;}(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(struct _RegionHandle*r){struct Cyc_Tcenv_Genv*_tmp359;return(
_tmp359=_region_malloc(r,sizeof(*_tmp359)),((_tmp359->grgn=r,((_tmp359->namespaces=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp359->aggrdecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp359->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp359->enumdecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp359->typedefs=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp359->ordinaries=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp359->availables=
0,_tmp359)))))))))))))))));}struct _tuple12{void*f1;int f2;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*r){{struct Cyc_Core_Opt*_tmp35A;Cyc_Tcutil_empty_var_set=((
_tmp35A=_cycalloc(sizeof(*_tmp35A)),((_tmp35A->v=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp35A))));}{struct Cyc_Tcenv_Genv*_tmpA=Cyc_Tcenv_empty_genv(r);{struct Cyc_Absyn_Datatypedecl**
_tmp35B;_tmpA->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmpA->datatypedecls,(*
Cyc_Absyn_exn_name).f2,((_tmp35B=_cycalloc(sizeof(*_tmp35B)),((_tmp35B[0]=Cyc_Absyn_exn_tud,
_tmp35B)))));}{struct Cyc_List_List*_tmpC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmpC != 0;_tmpC=_tmpC->tl){
struct Cyc_Tcenv_DatatypeRes_struct*_tmp361;struct Cyc_Tcenv_DatatypeRes_struct
_tmp360;struct _tuple12*_tmp35F;_tmpA->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmpA->ordinaries,(*((
struct Cyc_Absyn_Datatypefield*)_tmpC->hd)->name).f2,((_tmp35F=_region_malloc(r,
sizeof(*_tmp35F)),((_tmp35F->f1=(void*)((_tmp361=_cycalloc(sizeof(*_tmp361)),((
_tmp361[0]=((_tmp360.tag=2,((_tmp360.f1=Cyc_Absyn_exn_tud,((_tmp360.f2=(struct
Cyc_Absyn_Datatypefield*)_tmpC->hd,_tmp360)))))),_tmp361)))),((_tmp35F->f2=1,
_tmp35F)))))));}}{struct Cyc_Dict_Dict ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_varlist_cmp),0,_tmpA);struct Cyc_Tcenv_Tenv*_tmp362;return(_tmp362=
_region_malloc(r,sizeof(*_tmp362)),((_tmp362->ns=0,((_tmp362->ae=ae,((_tmp362->le=
0,((_tmp362->allow_valueof=0,_tmp362)))))))));};};}static struct Cyc_Tcenv_Genv*
Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){return((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(
te,loc,n,ns));}static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*
ns){if(ns == 0){const char*_tmp363;return((struct Cyc_List_List*(*)(struct
_dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp363="outer_namespace",_tag_dyneither(
_tmp363,sizeof(char),16))));}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
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
_tmp14=1;if(!_tmp14){lookup(ge2,v);{const char*_tmp367;void*_tmp366[1];struct Cyc_String_pa_struct
_tmp365;(_tmp365.tag=0,((_tmp365.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
v),((_tmp366[0]=& _tmp365,Cyc_Tcutil_terr(loc,((_tmp367="%s is ambiguous",
_tag_dyneither(_tmp367,sizeof(char),16))),_tag_dyneither(_tmp366,sizeof(void*),1)))))));};;
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
_tmp28);_LLA:;}};}}if(cns == 0){struct Cyc_Dict_Absent_struct _tmp36A;struct Cyc_Dict_Absent_struct*
_tmp369;(int)_throw((void*)((_tmp369=_cycalloc_atomic(sizeof(*_tmp369)),((
_tmp369[0]=((_tmp36A.tag=Cyc_Dict_Absent,_tmp36A)),_tmp369)))));}cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp2C=ge->ordinaries;struct _tuple12*
ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
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
elt))Cyc_Set_member)(_tmp42,n)){struct Cyc_List_List*_tmp36D;struct Cyc_List_List*
_tmp36C;_tmp3D=((_tmp36C=_region_malloc(_tmp3C,sizeof(*_tmp36C)),((_tmp36C->hd=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp3B,((_tmp36D=_cycalloc(sizeof(*_tmp36D)),((_tmp36D->hd=n,((_tmp36D->tl=ns,
_tmp36D))))))),((_tmp36C->tl=_tmp3D,_tmp36C))))));}}}if(((int(*)(struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp40,n)){struct Cyc_List_List*
_tmp370;struct Cyc_List_List*_tmp36F;_tmp3D=((_tmp36F=_region_malloc(_tmp3C,
sizeof(*_tmp36F)),((_tmp36F->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmp3B,((_tmp370=_cycalloc(sizeof(*
_tmp370)),((_tmp370->hd=n,((_tmp370->tl=ns,_tmp370))))))),((_tmp36F->tl=_tmp3D,
_tmp36F))))));}if(_tmp3D != 0){if(_tmp3D->tl != 0){const char*_tmp374;void*_tmp373[
1];struct Cyc_String_pa_struct _tmp372;(_tmp372.tag=0,((_tmp372.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp373[0]=& _tmp372,Cyc_Tcutil_terr(
loc,((_tmp374="%s is ambiguous",_tag_dyneither(_tmp374,sizeof(char),16))),
_tag_dyneither(_tmp373,sizeof(void*),1)))))));}return(struct Cyc_List_List*)
_tmp3D->hd;}if(_tmp3B == 0){struct Cyc_Dict_Absent_struct _tmp377;struct Cyc_Dict_Absent_struct*
_tmp376;(int)_throw((void*)((_tmp376=_cycalloc_atomic(sizeof(*_tmp376)),((
_tmp376[0]=((_tmp377.tag=Cyc_Dict_Absent,_tmp377)),_tmp376)))));}_tmp3B=Cyc_Tcenv_outer_namespace(
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
_tmp7B=1;if(!_tmp7B){{struct Cyc_Absyn_Datatypedecl***_tmp378;struct Cyc_Absyn_Datatypedecl***
_tmp7D=(_tmp378=_region_malloc(r,sizeof(*_tmp378)),((_tmp378[0]=((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Datatypedecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp6F),_tmp378)));_npop_handler(0);
return _tmp7D;};_pop_handler();}else{void*_tmp7A=(void*)_exn_thrown;void*_tmp7F=
_tmp7A;_LL3C: {struct Cyc_Dict_Absent_struct*_tmp80=(struct Cyc_Dict_Absent_struct*)
_tmp7F;if(_tmp80->tag != Cyc_Dict_Absent)goto _LL3E;}_LL3D: return 0;_LL3E:;_LL3F:(
void)_throw(_tmp7F);_LL3B:;}};}_LL33: if((_tmp70.Loc_n).tag != 4)goto _LL35;_tmp72=(
int)(_tmp70.Loc_n).val;_LL34:{const char*_tmp37B;void*_tmp37A;(_tmp37A=0,Cyc_Tcutil_terr(
loc,((_tmp37B="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp37B,sizeof(
char),33))),_tag_dyneither(_tmp37A,sizeof(void*),0)));}return 0;_LL35: if((_tmp70.Rel_n).tag
!= 1)goto _LL37;_tmp73=(struct Cyc_List_List*)(_tmp70.Rel_n).val;if(_tmp73 == 0)
goto _LL37;_tmp74=*_tmp73;_tmp75=(struct _dyneither_ptr*)_tmp74.hd;_tmp76=_tmp74.tl;
_LL36: {struct Cyc_Tcenv_Genv*ge;{struct _handler_cons _tmp83;_push_handler(& _tmp83);{
int _tmp85=0;if(setjmp(_tmp83.handler))_tmp85=1;if(!_tmp85){ge=Cyc_Tcenv_lookup_namespace(
te,loc,_tmp75,_tmp76);;_pop_handler();}else{void*_tmp84=(void*)_exn_thrown;void*
_tmp87=_tmp84;_LL41: {struct Cyc_Dict_Absent_struct*_tmp88=(struct Cyc_Dict_Absent_struct*)
_tmp87;if(_tmp88->tag != Cyc_Dict_Absent)goto _LL43;}_LL42:{const char*_tmp37E;void*
_tmp37D;(_tmp37D=0,Cyc_Tcutil_terr(loc,((_tmp37E="bad qualified name for @extensible datatype",
_tag_dyneither(_tmp37E,sizeof(char),44))),_tag_dyneither(_tmp37D,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_struct _tmp381;struct Cyc_Dict_Absent_struct*_tmp380;(int)
_throw((void*)((_tmp380=_cycalloc_atomic(sizeof(*_tmp380)),((_tmp380[0]=((
_tmp381.tag=Cyc_Dict_Absent,_tmp381)),_tmp380)))));};_LL43:;_LL44:(void)_throw(
_tmp87);_LL40:;}};}{struct Cyc_Dict_Dict _tmp8D=ge->datatypedecls;struct Cyc_Absyn_Datatypedecl***
_tmp382;return(_tmp382=_region_malloc(r,sizeof(*_tmp382)),((_tmp382[0]=((struct
Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp8D,_tmp6F),_tmp382)));};}_LL37: if((_tmp70.C_n).tag != 3)goto _LL39;_tmp77=(
struct Cyc_List_List*)(_tmp70.C_n).val;_LL38: _tmp78=_tmp77;goto _LL3A;_LL39: if((
_tmp70.Abs_n).tag != 2)goto _LL30;_tmp78=(struct Cyc_List_List*)(_tmp70.Abs_n).val;
_LL3A: {struct Cyc_Dict_Dict _tmp8F=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp78))->datatypedecls;struct
Cyc_Absyn_Datatypedecl***_tmp383;return(_tmp383=_region_malloc(r,sizeof(*_tmp383)),((
_tmp383[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8F,_tmp6F),_tmp383)));}_LL30:;};}static
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
== 0){const char*_tmp384;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((
_tmp384="put_fenv",_tag_dyneither(_tmp384,sizeof(char),9))));}{struct Cyc_Tcenv_Tenv*
_tmp385;return(_tmp385=_region_malloc(l,sizeof(*_tmp385)),((_tmp385->ns=te->ns,((
_tmp385->ae=te->ae,((_tmp385->le=(struct Cyc_Tcenv_Fenv*)fe,((_tmp385->allow_valueof=
te->allow_valueof,_tmp385)))))))));};}static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(
struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_tmp386;
return(_tmp386=_region_malloc(l,sizeof(*_tmp386)),((_tmp386->ns=te->ns,((_tmp386->ae=
te->ae,((_tmp386->le=0,((_tmp386->allow_valueof=te->allow_valueof,_tmp386)))))))));}
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,
struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpBA;struct Cyc_List_List*
_tmpBB;struct Cyc_RgnOrder_RgnPO*_tmpBC;const struct Cyc_Tcenv_Bindings*_tmpBD;
struct Cyc_Absyn_Stmt*_tmpBE;struct Cyc_Tcenv_CtrlEnv*_tmpBF;void*_tmpC0;void*
_tmpC1;int _tmpC2;int _tmpC3;struct _RegionHandle*_tmpC4;struct Cyc_Tcenv_Fenv _tmpB9=*
f;_tmpBA=_tmpB9.shared;_tmpBB=_tmpB9.type_vars;_tmpBC=_tmpB9.region_order;_tmpBD=
_tmpB9.locals;_tmpBE=_tmpB9.encloser;_tmpBF=_tmpB9.ctrl_env;_tmpC0=_tmpB9.capability;
_tmpC1=_tmpB9.curr_rgn;_tmpC2=_tmpB9.in_notreadctxt;_tmpC3=_tmpB9.in_lhs;_tmpC4=
_tmpB9.fnrgn;{struct Cyc_Tcenv_Fenv*_tmp387;return(_tmp387=_region_malloc(l,
sizeof(*_tmp387)),((_tmp387->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpBA,((
_tmp387->type_vars=(struct Cyc_List_List*)_tmpBB,((_tmp387->region_order=(struct
Cyc_RgnOrder_RgnPO*)_tmpBC,((_tmp387->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmpBD),((_tmp387->encloser=(struct Cyc_Absyn_Stmt*)
_tmpBE,((_tmp387->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmpBF),((_tmp387->capability=(void*)_tmpC0,((_tmp387->curr_rgn=(void*)_tmpC1,((
_tmp387->in_notreadctxt=(int)_tmpC2,((_tmp387->in_lhs=(int)_tmpC3,((_tmp387->fnrgn=(
struct _RegionHandle*)l,(struct Cyc_Tcenv_Fenv*)_tmp387)))))))))))))))))))))));};}
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,
struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpC7;struct Cyc_List_List*
_tmpC8;struct Cyc_RgnOrder_RgnPO*_tmpC9;const struct Cyc_Tcenv_Bindings*_tmpCA;
struct Cyc_Absyn_Stmt*_tmpCB;struct Cyc_Tcenv_CtrlEnv*_tmpCC;void*_tmpCD;void*
_tmpCE;int _tmpCF;int _tmpD0;struct Cyc_Tcenv_Fenv _tmpC6=*f;_tmpC7=_tmpC6.shared;
_tmpC8=_tmpC6.type_vars;_tmpC9=_tmpC6.region_order;_tmpCA=_tmpC6.locals;_tmpCB=
_tmpC6.encloser;_tmpCC=_tmpC6.ctrl_env;_tmpCD=_tmpC6.capability;_tmpCE=_tmpC6.curr_rgn;
_tmpCF=_tmpC6.in_notreadctxt;_tmpD0=_tmpC6.in_lhs;{struct _RegionHandle*_tmpD2;
void*_tmpD3;void*_tmpD4;const struct _tuple10*_tmpD5;void*_tmpD6;int _tmpD7;struct
Cyc_Tcenv_CtrlEnv _tmpD1=*_tmpCC;_tmpD2=_tmpD1.ctrl_rgn;_tmpD3=_tmpD1.continue_stmt;
_tmpD4=_tmpD1.break_stmt;_tmpD5=_tmpD1.fallthru_clause;_tmpD6=_tmpD1.next_stmt;
_tmpD7=_tmpD1.try_depth;{struct Cyc_Tcenv_CtrlEnv*_tmp388;struct Cyc_Tcenv_CtrlEnv*
_tmpD8=(_tmp388=_region_malloc(l,sizeof(*_tmp388)),((_tmp388->ctrl_rgn=_tmpD2,((
_tmp388->continue_stmt=_tmpD3,((_tmp388->break_stmt=_tmpD4,((_tmp388->fallthru_clause=
_tmpD5,((_tmp388->next_stmt=_tmpD6,((_tmp388->try_depth=_tmpD7,_tmp388)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp389;return(_tmp389=_region_malloc(l,sizeof(*_tmp389)),((
_tmp389->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpC7,((_tmp389->type_vars=(struct
Cyc_List_List*)_tmpC8,((_tmp389->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC9,((
_tmp389->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)
_tmpCA),((_tmp389->encloser=(struct Cyc_Absyn_Stmt*)_tmpCB,((_tmp389->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)_tmpD8,((_tmp389->capability=(void*)_tmpCD,((_tmp389->curr_rgn=(
void*)_tmpCE,((_tmp389->in_notreadctxt=(int)_tmpCF,((_tmp389->in_lhs=(int)_tmpD0,((
_tmp389->fnrgn=(struct _RegionHandle*)l,(struct Cyc_Tcenv_Fenv*)_tmp389)))))))))))))))))))))));};};}
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmpDD;
struct Cyc_Tcenv_SharedFenv*_tmpDE;const char*_tmp38A;struct Cyc_Tcenv_Fenv*_tmpDC=
Cyc_Tcenv_get_fenv(te,((_tmp38A="return_typ",_tag_dyneither(_tmp38A,sizeof(char),
11))));_tmpDD=*_tmpDC;_tmpDE=_tmpDD.shared;{void*_tmpE0;struct Cyc_Tcenv_SharedFenv
_tmpDF=*_tmpDE;_tmpE0=_tmpDF.return_typ;return _tmpE0;};}struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpE1=te->le;if(te->le == 0)return
0;{struct Cyc_List_List*_tmpE3;struct Cyc_Tcenv_Fenv _tmpE2=*((struct Cyc_Tcenv_Fenv*)
_check_null(_tmpE1));_tmpE3=_tmpE2.type_vars;return _tmpE3;};}struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){struct Cyc_Core_Opt*
_tmp38B;return(_tmp38B=_cycalloc(sizeof(*_tmp38B)),((_tmp38B->v=Cyc_Tcenv_lookup_type_vars(
te),_tmp38B)));}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*
r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
struct Cyc_Tcenv_Fenv _tmpE7;struct Cyc_Tcenv_SharedFenv*_tmpE8;struct Cyc_List_List*
_tmpE9;struct Cyc_RgnOrder_RgnPO*_tmpEA;const struct Cyc_Tcenv_Bindings*_tmpEB;
struct Cyc_Absyn_Stmt*_tmpEC;struct Cyc_Tcenv_CtrlEnv*_tmpED;void*_tmpEE;void*
_tmpEF;int _tmpF0;int _tmpF1;const char*_tmp38C;struct Cyc_Tcenv_Fenv*_tmpE6=Cyc_Tcenv_get_fenv(
te,((_tmp38C="add_type_vars",_tag_dyneither(_tmp38C,sizeof(char),14))));_tmpE7=*
_tmpE6;_tmpE8=_tmpE7.shared;_tmpE9=_tmpE7.type_vars;_tmpEA=_tmpE7.region_order;
_tmpEB=_tmpE7.locals;_tmpEC=_tmpE7.encloser;_tmpED=_tmpE7.ctrl_env;_tmpEE=_tmpE7.capability;
_tmpEF=_tmpE7.curr_rgn;_tmpF0=_tmpE7.in_notreadctxt;_tmpF1=_tmpE7.in_lhs;Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_List_List*_tmpF2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpE9);Cyc_Tcutil_check_unique_tvars(
loc,_tmpF2);{struct Cyc_Tcenv_Fenv*_tmp38D;struct Cyc_Tcenv_Fenv*_tmpF3=(_tmp38D=
_region_malloc(r,sizeof(*_tmp38D)),((_tmp38D->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpE8,((_tmp38D->type_vars=(struct Cyc_List_List*)_tmpF2,((_tmp38D->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpEA,((_tmp38D->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmpEB),((_tmp38D->encloser=(struct Cyc_Absyn_Stmt*)
_tmpEC,((_tmp38D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmpED),((_tmp38D->capability=(void*)_tmpEE,((_tmp38D->curr_rgn=(void*)_tmpEF,((
_tmp38D->in_notreadctxt=(int)_tmpF0,((_tmp38D->in_lhs=(int)_tmpF1,((_tmp38D->fnrgn=(
struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp38D)))))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmpF3);};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpF5=te->le;
if(_tmpF5 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);else{struct Cyc_Tcenv_Fenv*fe=
Cyc_Tcenv_copy_fenv_old_ctrl(r,(struct Cyc_Tcenv_Fenv*)_tmpF5);return Cyc_Tcenv_put_fenv(
r,te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*
vd){struct _dyneither_ptr*_tmpF6=(*vd->name).f2;{union Cyc_Absyn_Nmspace _tmpF7=(*
vd->name).f1;int _tmpF8;_LL5C: if((_tmpF7.Loc_n).tag != 4)goto _LL5E;_tmpF8=(int)(
_tmpF7.Loc_n).val;_LL5D: goto _LL5B;_LL5E:;_LL5F: {struct Cyc_Core_Impossible_struct
_tmp393;const char*_tmp392;struct Cyc_Core_Impossible_struct*_tmp391;(int)_throw((
void*)((_tmp391=_cycalloc(sizeof(*_tmp391)),((_tmp391[0]=((_tmp393.tag=Cyc_Core_Impossible,((
_tmp393.f1=((_tmp392="add_local_var: called with Rel_n",_tag_dyneither(_tmp392,
sizeof(char),33))),_tmp393)))),_tmp391)))));}_LL5B:;}{struct Cyc_Tcenv_Fenv _tmpFE;
struct Cyc_Tcenv_SharedFenv*_tmpFF;struct Cyc_List_List*_tmp100;struct Cyc_RgnOrder_RgnPO*
_tmp101;const struct Cyc_Tcenv_Bindings*_tmp102;struct Cyc_Absyn_Stmt*_tmp103;
struct Cyc_Tcenv_CtrlEnv*_tmp104;void*_tmp105;void*_tmp106;int _tmp107;int _tmp108;
const char*_tmp394;struct Cyc_Tcenv_Fenv*_tmpFD=Cyc_Tcenv_get_fenv(te,((_tmp394="add_local_var",
_tag_dyneither(_tmp394,sizeof(char),14))));_tmpFE=*_tmpFD;_tmpFF=_tmpFE.shared;
_tmp100=_tmpFE.type_vars;_tmp101=_tmpFE.region_order;_tmp102=_tmpFE.locals;
_tmp103=_tmpFE.encloser;_tmp104=_tmpFE.ctrl_env;_tmp105=_tmpFE.capability;
_tmp106=_tmpFE.curr_rgn;_tmp107=_tmpFE.in_notreadctxt;_tmp108=_tmpFE.in_lhs;{
struct Cyc_Absyn_Local_b_struct*_tmp39A;struct Cyc_Absyn_Local_b_struct _tmp399;
struct Cyc_Tcenv_Bindings*_tmp398;struct Cyc_Tcenv_Bindings*_tmp109=(_tmp398=
_region_malloc(r,sizeof(*_tmp398)),((_tmp398->v=_tmpF6,((_tmp398->b=(void*)((
_tmp39A=_cycalloc(sizeof(*_tmp39A)),((_tmp39A[0]=((_tmp399.tag=4,((_tmp399.f1=vd,
_tmp399)))),_tmp39A)))),((_tmp398->tl=(const struct Cyc_Tcenv_Bindings*)_tmp102,
_tmp398)))))));struct Cyc_Tcenv_Fenv*_tmp39B;struct Cyc_Tcenv_Fenv*_tmp10A=(
_tmp39B=_region_malloc(r,sizeof(*_tmp39B)),((_tmp39B->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpFF,((_tmp39B->type_vars=(struct Cyc_List_List*)_tmp100,((_tmp39B->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp101,((_tmp39B->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp109),((_tmp39B->encloser=(struct Cyc_Absyn_Stmt*)
_tmp103,((_tmp39B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp104),((_tmp39B->capability=(void*)_tmp105,((_tmp39B->curr_rgn=(void*)_tmp106,((
_tmp39B->in_notreadctxt=(int)_tmp107,((_tmp39B->in_lhs=(int)_tmp108,((_tmp39B->fnrgn=(
struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp39B)))))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp10A);};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp10F=te->le;
if(_tmp10F == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmp111;struct Cyc_List_List*_tmp112;struct Cyc_RgnOrder_RgnPO*_tmp113;const struct
Cyc_Tcenv_Bindings*_tmp114;struct Cyc_Absyn_Stmt*_tmp115;struct Cyc_Tcenv_CtrlEnv*
_tmp116;void*_tmp117;void*_tmp118;int _tmp119;int _tmp11A;struct Cyc_Tcenv_Fenv
_tmp110=*_tmp10F;_tmp111=_tmp110.shared;_tmp112=_tmp110.type_vars;_tmp113=
_tmp110.region_order;_tmp114=_tmp110.locals;_tmp115=_tmp110.encloser;_tmp116=
_tmp110.ctrl_env;_tmp117=_tmp110.capability;_tmp118=_tmp110.curr_rgn;_tmp119=
_tmp110.in_notreadctxt;_tmp11A=_tmp110.in_lhs;{struct Cyc_Tcenv_Fenv*_tmp39C;
struct Cyc_Tcenv_Fenv*_tmp11B=(_tmp39C=_region_malloc(r,sizeof(*_tmp39C)),((
_tmp39C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp111,((_tmp39C->type_vars=(
struct Cyc_List_List*)_tmp112,((_tmp39C->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp113,((_tmp39C->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)
_tmp114),((_tmp39C->encloser=(struct Cyc_Absyn_Stmt*)_tmp115,((_tmp39C->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp116),((_tmp39C->capability=(
void*)_tmp117,((_tmp39C->curr_rgn=(void*)_tmp118,((_tmp39C->in_notreadctxt=(int)
1,((_tmp39C->in_lhs=(int)_tmp11A,((_tmp39C->fnrgn=(struct _RegionHandle*)r,(
struct Cyc_Tcenv_Fenv*)_tmp39C)))))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,
te,_tmp11B);};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp11D=te->le;if(
_tmp11D == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmp11F;struct Cyc_List_List*_tmp120;struct Cyc_RgnOrder_RgnPO*_tmp121;const struct
Cyc_Tcenv_Bindings*_tmp122;struct Cyc_Absyn_Stmt*_tmp123;struct Cyc_Tcenv_CtrlEnv*
_tmp124;void*_tmp125;void*_tmp126;int _tmp127;int _tmp128;struct Cyc_Tcenv_Fenv
_tmp11E=*_tmp11D;_tmp11F=_tmp11E.shared;_tmp120=_tmp11E.type_vars;_tmp121=
_tmp11E.region_order;_tmp122=_tmp11E.locals;_tmp123=_tmp11E.encloser;_tmp124=
_tmp11E.ctrl_env;_tmp125=_tmp11E.capability;_tmp126=_tmp11E.curr_rgn;_tmp127=
_tmp11E.in_notreadctxt;_tmp128=_tmp11E.in_lhs;{struct Cyc_Tcenv_Fenv*_tmp39D;
struct Cyc_Tcenv_Fenv*_tmp129=(_tmp39D=_region_malloc(r,sizeof(*_tmp39D)),((
_tmp39D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp11F,((_tmp39D->type_vars=(
struct Cyc_List_List*)_tmp120,((_tmp39D->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp121,((_tmp39D->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)
_tmp122),((_tmp39D->encloser=(struct Cyc_Absyn_Stmt*)_tmp123,((_tmp39D->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp124),((_tmp39D->capability=(
void*)_tmp125,((_tmp39D->curr_rgn=(void*)_tmp126,((_tmp39D->in_notreadctxt=(int)
0,((_tmp39D->in_lhs=(int)_tmp128,((_tmp39D->fnrgn=(struct _RegionHandle*)r,(
struct Cyc_Tcenv_Fenv*)_tmp39D)))))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,
te,_tmp129);};};}int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
_tmp12B=te->le;if(_tmp12B == 0)return 0;{struct Cyc_Tcenv_Fenv _tmp12D;int _tmp12E;
struct Cyc_Tcenv_Fenv*_tmp12C=(struct Cyc_Tcenv_Fenv*)_tmp12B;_tmp12D=*_tmp12C;
_tmp12E=_tmp12D.in_notreadctxt;return _tmp12E;};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp12F=te->le;
if(_tmp12F == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmp131;struct Cyc_List_List*_tmp132;struct Cyc_RgnOrder_RgnPO*_tmp133;const struct
Cyc_Tcenv_Bindings*_tmp134;struct Cyc_Absyn_Stmt*_tmp135;struct Cyc_Tcenv_CtrlEnv*
_tmp136;void*_tmp137;void*_tmp138;int _tmp139;int _tmp13A;struct Cyc_Tcenv_Fenv
_tmp130=*_tmp12F;_tmp131=_tmp130.shared;_tmp132=_tmp130.type_vars;_tmp133=
_tmp130.region_order;_tmp134=_tmp130.locals;_tmp135=_tmp130.encloser;_tmp136=
_tmp130.ctrl_env;_tmp137=_tmp130.capability;_tmp138=_tmp130.curr_rgn;_tmp139=
_tmp130.in_notreadctxt;_tmp13A=_tmp130.in_lhs;{struct Cyc_Tcenv_Fenv*_tmp39E;
struct Cyc_Tcenv_Fenv*_tmp13B=(_tmp39E=_region_malloc(r,sizeof(*_tmp39E)),((
_tmp39E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp131,((_tmp39E->type_vars=(
struct Cyc_List_List*)_tmp132,((_tmp39E->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp133,((_tmp39E->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)
_tmp134),((_tmp39E->encloser=(struct Cyc_Absyn_Stmt*)_tmp135,((_tmp39E->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp136),((_tmp39E->capability=(
void*)_tmp137,((_tmp39E->curr_rgn=(void*)_tmp138,((_tmp39E->in_notreadctxt=(int)
_tmp139,((_tmp39E->in_lhs=(int)1,((_tmp39E->fnrgn=(struct _RegionHandle*)r,(
struct Cyc_Tcenv_Fenv*)_tmp39E)))))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,
te,_tmp13B);};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp13D=te->le;if(_tmp13D == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*_tmp13F;struct
Cyc_List_List*_tmp140;struct Cyc_RgnOrder_RgnPO*_tmp141;const struct Cyc_Tcenv_Bindings*
_tmp142;struct Cyc_Absyn_Stmt*_tmp143;struct Cyc_Tcenv_CtrlEnv*_tmp144;void*
_tmp145;void*_tmp146;int _tmp147;int _tmp148;struct Cyc_Tcenv_Fenv _tmp13E=*_tmp13D;
_tmp13F=_tmp13E.shared;_tmp140=_tmp13E.type_vars;_tmp141=_tmp13E.region_order;
_tmp142=_tmp13E.locals;_tmp143=_tmp13E.encloser;_tmp144=_tmp13E.ctrl_env;_tmp145=
_tmp13E.capability;_tmp146=_tmp13E.curr_rgn;_tmp147=_tmp13E.in_notreadctxt;
_tmp148=_tmp13E.in_lhs;{struct Cyc_Tcenv_Fenv*_tmp39F;struct Cyc_Tcenv_Fenv*
_tmp149=(_tmp39F=_region_malloc(r,sizeof(*_tmp39F)),((_tmp39F->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp13F,((_tmp39F->type_vars=(struct Cyc_List_List*)_tmp140,((_tmp39F->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp141,((_tmp39F->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp142),((_tmp39F->encloser=(struct Cyc_Absyn_Stmt*)
_tmp143,((_tmp39F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp144),((_tmp39F->capability=(void*)_tmp145,((_tmp39F->curr_rgn=(void*)_tmp146,((
_tmp39F->in_notreadctxt=(int)_tmp147,((_tmp39F->in_lhs=(int)0,((_tmp39F->fnrgn=(
struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp39F)))))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp149);};};}int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmp14B=te->le;if(_tmp14B == 0)return 0;{struct Cyc_Tcenv_Fenv
_tmp14D;int _tmp14E;struct Cyc_Tcenv_Fenv*_tmp14C=(struct Cyc_Tcenv_Fenv*)_tmp14B;
_tmp14D=*_tmp14C;_tmp14E=_tmp14D.in_lhs;return _tmp14E;};}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){struct _dyneither_ptr*_tmp14F=(*
vd->name).f2;struct Cyc_Tcenv_Fenv _tmp152;struct Cyc_Tcenv_SharedFenv*_tmp153;
struct Cyc_List_List*_tmp154;struct Cyc_RgnOrder_RgnPO*_tmp155;const struct Cyc_Tcenv_Bindings*
_tmp156;struct Cyc_Absyn_Stmt*_tmp157;struct Cyc_Tcenv_CtrlEnv*_tmp158;void*
_tmp159;void*_tmp15A;int _tmp15B;int _tmp15C;const char*_tmp3A0;struct Cyc_Tcenv_Fenv*
_tmp151=Cyc_Tcenv_get_fenv(te,((_tmp3A0="add_pat_var",_tag_dyneither(_tmp3A0,
sizeof(char),12))));_tmp152=*_tmp151;_tmp153=_tmp152.shared;_tmp154=_tmp152.type_vars;
_tmp155=_tmp152.region_order;_tmp156=_tmp152.locals;_tmp157=_tmp152.encloser;
_tmp158=_tmp152.ctrl_env;_tmp159=_tmp152.capability;_tmp15A=_tmp152.curr_rgn;
_tmp15B=_tmp152.in_notreadctxt;_tmp15C=_tmp152.in_lhs;{struct Cyc_Absyn_Pat_b_struct*
_tmp3A6;struct Cyc_Absyn_Pat_b_struct _tmp3A5;struct Cyc_Tcenv_Bindings*_tmp3A4;
struct Cyc_Tcenv_Bindings*_tmp15D=(_tmp3A4=_region_malloc(r,sizeof(*_tmp3A4)),((
_tmp3A4->v=_tmp14F,((_tmp3A4->b=(void*)((_tmp3A6=_cycalloc(sizeof(*_tmp3A6)),((
_tmp3A6[0]=((_tmp3A5.tag=5,((_tmp3A5.f1=vd,_tmp3A5)))),_tmp3A6)))),((_tmp3A4->tl=(
const struct Cyc_Tcenv_Bindings*)_tmp156,_tmp3A4)))))));struct Cyc_Tcenv_Fenv*
_tmp3A7;struct Cyc_Tcenv_Fenv*_tmp15E=(_tmp3A7=_region_malloc(r,sizeof(*_tmp3A7)),((
_tmp3A7->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp153,((_tmp3A7->type_vars=(
struct Cyc_List_List*)_tmp154,((_tmp3A7->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp155,((_tmp3A7->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)
_tmp15D),((_tmp3A7->encloser=(struct Cyc_Absyn_Stmt*)_tmp157,((_tmp3A7->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp158),((_tmp3A7->capability=(
void*)_tmp159,((_tmp3A7->curr_rgn=(void*)_tmp15A,((_tmp3A7->in_notreadctxt=(int)
_tmp15B,((_tmp3A7->in_lhs=(int)_tmp15C,((_tmp3A7->fnrgn=(struct _RegionHandle*)r,(
struct Cyc_Tcenv_Fenv*)_tmp3A7)))))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,
te,_tmp15E);};}void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct Cyc_Tcenv_Fenv*_tmp163=
te->le;struct _tuple0 _tmp165;union Cyc_Absyn_Nmspace _tmp166;struct _dyneither_ptr*
_tmp167;struct _tuple0*_tmp164=q;_tmp165=*_tmp164;_tmp166=_tmp165.f1;_tmp167=
_tmp165.f2;{union Cyc_Absyn_Nmspace _tmp168=_tmp166;int _tmp169;struct Cyc_List_List*
_tmp16A;_LL61: if((_tmp168.Loc_n).tag != 4)goto _LL63;_tmp169=(int)(_tmp168.Loc_n).val;
_LL62: if(_tmp163 == 0){struct Cyc_Dict_Absent_struct _tmp3AA;struct Cyc_Dict_Absent_struct*
_tmp3A9;(int)_throw((void*)((_tmp3A9=_cycalloc_atomic(sizeof(*_tmp3A9)),((
_tmp3A9[0]=((_tmp3AA.tag=Cyc_Dict_Absent,_tmp3AA)),_tmp3A9)))));}goto _LL64;_LL63:
if((_tmp168.Rel_n).tag != 1)goto _LL65;_tmp16A=(struct Cyc_List_List*)(_tmp168.Rel_n).val;
if(_tmp16A != 0)goto _LL65;if(!(_tmp163 != 0))goto _LL65;_LL64: {struct Cyc_Tcenv_Fenv
_tmp16E;const struct Cyc_Tcenv_Bindings*_tmp16F;struct Cyc_Tcenv_Fenv*_tmp16D=(
struct Cyc_Tcenv_Fenv*)_tmp163;_tmp16E=*_tmp16D;_tmp16F=_tmp16E.locals;{struct
_handler_cons _tmp170;_push_handler(& _tmp170);{int _tmp172=0;if(setjmp(_tmp170.handler))
_tmp172=1;if(!_tmp172){{struct Cyc_Tcenv_VarRes_struct _tmp3AD;struct Cyc_Tcenv_VarRes_struct*
_tmp3AC;void*_tmp175=(void*)((_tmp3AC=_region_malloc(r,sizeof(*_tmp3AC)),((
_tmp3AC[0]=((_tmp3AD.tag=0,((_tmp3AD.f1=(void*)Cyc_Tcenv_lookup_binding(_tmp16F,
_tmp167),_tmp3AD)))),_tmp3AC))));_npop_handler(0);return _tmp175;};_pop_handler();}
else{void*_tmp171=(void*)_exn_thrown;void*_tmp177=_tmp171;_LL68: {struct Cyc_Tcenv_NoBinding_struct*
_tmp178=(struct Cyc_Tcenv_NoBinding_struct*)_tmp177;if(_tmp178->tag != Cyc_Tcenv_NoBinding)
goto _LL6A;}_LL69: return(void*)Cyc_Tcenv_lookup_ordinary_global(te,loc,q);_LL6A:;
_LL6B:(void)_throw(_tmp177);_LL67:;}};};}_LL65:;_LL66: {struct _handler_cons
_tmp179;_push_handler(& _tmp179);{int _tmp17B=0;if(setjmp(_tmp179.handler))_tmp17B=
1;if(!_tmp17B){{void*_tmp17C=(void*)Cyc_Tcenv_lookup_ordinary_global(te,loc,q);
_npop_handler(0);return _tmp17C;};_pop_handler();}else{void*_tmp17A=(void*)
_exn_thrown;void*_tmp17E=_tmp17A;_LL6D: {struct Cyc_Dict_Absent_struct*_tmp17F=(
struct Cyc_Dict_Absent_struct*)_tmp17E;if(_tmp17F->tag != Cyc_Dict_Absent)goto
_LL6F;}_LL6E: {struct Cyc_Tcenv_VarRes_struct _tmp3B0;struct Cyc_Tcenv_VarRes_struct*
_tmp3AF;return(void*)((_tmp3AF=_region_malloc(r,sizeof(*_tmp3AF)),((_tmp3AF[0]=((
_tmp3B0.tag=0,((_tmp3B0.f1=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),_tmp3B0)))),
_tmp3AF))));}_LL6F:;_LL70:(void)_throw(_tmp17E);_LL6C:;}};}_LL60:;};}void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_Fenv _tmp184;struct Cyc_Tcenv_CtrlEnv*_tmp185;const char*_tmp3B1;
struct Cyc_Tcenv_Fenv*_tmp183=Cyc_Tcenv_get_fenv(te,((_tmp3B1="process_continue",
_tag_dyneither(_tmp3B1,sizeof(char),17))));_tmp184=*_tmp183;_tmp185=_tmp184.ctrl_env;{
void*_tmp186=_tmp185->continue_stmt;struct Cyc_Absyn_Stmt*_tmp188;_LL72: {struct
Cyc_Tcenv_Stmt_j_struct*_tmp187=(struct Cyc_Tcenv_Stmt_j_struct*)_tmp186;if(
_tmp187->tag != 3)goto _LL74;else{_tmp188=_tmp187->f1;}}_LL73:{struct Cyc_List_List*
_tmp3B2;_tmp188->non_local_preds=((_tmp3B2=_cycalloc(sizeof(*_tmp3B2)),((_tmp3B2->hd=
s,((_tmp3B2->tl=_tmp188->non_local_preds,_tmp3B2))))));}*sopt=(struct Cyc_Absyn_Stmt*)
_tmp188;return;_LL74: {struct Cyc_Tcenv_NotLoop_j_struct*_tmp189=(struct Cyc_Tcenv_NotLoop_j_struct*)
_tmp186;if(_tmp189->tag != 0)goto _LL76;}_LL75:{const char*_tmp3B5;void*_tmp3B4;(
_tmp3B4=0,Cyc_Tcutil_terr(s->loc,((_tmp3B5="continue not in a loop",
_tag_dyneither(_tmp3B5,sizeof(char),23))),_tag_dyneither(_tmp3B4,sizeof(void*),0)));}
return;_LL76: {struct Cyc_Tcenv_CaseEnd_j_struct*_tmp18A=(struct Cyc_Tcenv_CaseEnd_j_struct*)
_tmp186;if(_tmp18A->tag != 1)goto _LL78;}_LL77: goto _LL79;_LL78: {struct Cyc_Tcenv_FnEnd_j_struct*
_tmp18B=(struct Cyc_Tcenv_FnEnd_j_struct*)_tmp186;if(_tmp18B->tag != 2)goto _LL71;}
_LL79: {const char*_tmp3B6;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((
_tmp3B6="bad continue destination",_tag_dyneither(_tmp3B6,sizeof(char),25))));}
_LL71:;};}void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv _tmp192;struct Cyc_Tcenv_CtrlEnv*
_tmp193;struct Cyc_Tcenv_SharedFenv*_tmp194;const char*_tmp3B7;struct Cyc_Tcenv_Fenv*
_tmp191=Cyc_Tcenv_get_fenv(te,((_tmp3B7="process_break",_tag_dyneither(_tmp3B7,
sizeof(char),14))));_tmp192=*_tmp191;_tmp193=_tmp192.ctrl_env;_tmp194=_tmp192.shared;{
void*_tmp195=_tmp193->break_stmt;struct Cyc_Absyn_Stmt*_tmp197;_LL7B: {struct Cyc_Tcenv_Stmt_j_struct*
_tmp196=(struct Cyc_Tcenv_Stmt_j_struct*)_tmp195;if(_tmp196->tag != 3)goto _LL7D;
else{_tmp197=_tmp196->f1;}}_LL7C:{struct Cyc_List_List*_tmp3B8;_tmp197->non_local_preds=((
_tmp3B8=_cycalloc(sizeof(*_tmp3B8)),((_tmp3B8->hd=s,((_tmp3B8->tl=_tmp197->non_local_preds,
_tmp3B8))))));}*sopt=(struct Cyc_Absyn_Stmt*)_tmp197;return;_LL7D: {struct Cyc_Tcenv_NotLoop_j_struct*
_tmp198=(struct Cyc_Tcenv_NotLoop_j_struct*)_tmp195;if(_tmp198->tag != 0)goto _LL7F;}
_LL7E:{const char*_tmp3BB;void*_tmp3BA;(_tmp3BA=0,Cyc_Tcutil_terr(s->loc,((
_tmp3BB="break not in a loop or switch",_tag_dyneither(_tmp3BB,sizeof(char),30))),
_tag_dyneither(_tmp3BA,sizeof(void*),0)));}return;_LL7F: {struct Cyc_Tcenv_FnEnd_j_struct*
_tmp199=(struct Cyc_Tcenv_FnEnd_j_struct*)_tmp195;if(_tmp199->tag != 2)goto _LL81;}
_LL80:{void*_tmp19E=Cyc_Tcutil_compress(_tmp194->return_typ);_LL84: {struct Cyc_Absyn_VoidType_struct*
_tmp19F=(struct Cyc_Absyn_VoidType_struct*)_tmp19E;if(_tmp19F->tag != 0)goto _LL86;}
_LL85: goto _LL83;_LL86: {struct Cyc_Absyn_FloatType_struct*_tmp1A0=(struct Cyc_Absyn_FloatType_struct*)
_tmp19E;if(_tmp1A0->tag != 7)goto _LL88;}_LL87: goto _LL89;_LL88: {struct Cyc_Absyn_IntType_struct*
_tmp1A1=(struct Cyc_Absyn_IntType_struct*)_tmp19E;if(_tmp1A1->tag != 6)goto _LL8A;}
_LL89:{const char*_tmp3BE;void*_tmp3BD;(_tmp3BD=0,Cyc_Tcutil_warn(s->loc,((
_tmp3BE="break may cause function not to return a value",_tag_dyneither(_tmp3BE,
sizeof(char),47))),_tag_dyneither(_tmp3BD,sizeof(void*),0)));}goto _LL83;_LL8A:;
_LL8B:{const char*_tmp3C1;void*_tmp3C0;(_tmp3C0=0,Cyc_Tcutil_terr(s->loc,((
_tmp3C1="break may cause function not to return a value",_tag_dyneither(_tmp3C1,
sizeof(char),47))),_tag_dyneither(_tmp3C0,sizeof(void*),0)));}goto _LL83;_LL83:;}
return;_LL81: {struct Cyc_Tcenv_CaseEnd_j_struct*_tmp19A=(struct Cyc_Tcenv_CaseEnd_j_struct*)
_tmp195;if(_tmp19A->tag != 1)goto _LL7A;}_LL82:{const char*_tmp3C4;void*_tmp3C3;(
_tmp3C3=0,Cyc_Tcutil_terr(s->loc,((_tmp3C4="break causes outer switch clause to implicitly fallthru",
_tag_dyneither(_tmp3C4,sizeof(char),56))),_tag_dyneither(_tmp3C3,sizeof(void*),0)));}
return;_LL7A:;};}void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv _tmp1AA;
struct Cyc_Tcenv_SharedFenv*_tmp1AB;const char*_tmp3C5;struct Cyc_Tcenv_Fenv*
_tmp1A9=Cyc_Tcenv_get_fenv(te,((_tmp3C5="process_goto",_tag_dyneither(_tmp3C5,
sizeof(char),13))));_tmp1AA=*_tmp1A9;_tmp1AB=_tmp1AA.shared;{struct Cyc_Absyn_Stmt**
sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup_opt)(_tmp1AB->seen_labels,l);if(sopt2 == 0){struct Cyc_Dict_Dict
_tmp1AC=_tmp1AB->needed_labels;struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1AC,l);
struct _RegionHandle*frgn=_tmp1AB->frgn;if(slopt == 0){struct Cyc_List_List**
_tmp3C6;slopt=((_tmp3C6=_region_malloc(frgn,sizeof(*_tmp3C6)),((_tmp3C6[0]=0,
_tmp3C6))));}{struct Cyc_List_List*_tmp3C7;struct Cyc_List_List*new_needed=(
_tmp3C7=_cycalloc(sizeof(*_tmp3C7)),((_tmp3C7->hd=s,((_tmp3C7->tl=*slopt,_tmp3C7)))));
_tmp1AB->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1AC,l,new_needed);};}
else{struct Cyc_Absyn_Stmt*s=*sopt2;{struct Cyc_List_List*_tmp3C8;s->non_local_preds=((
_tmp3C8=_cycalloc(sizeof(*_tmp3C8)),((_tmp3C8->hd=s,((_tmp3C8->tl=s->non_local_preds,
_tmp3C8))))));}*sopt=(struct Cyc_Absyn_Stmt*)s;}};}const struct _tuple10*const Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***
clauseopt){struct Cyc_Tcenv_Fenv _tmp1B2;struct Cyc_Tcenv_CtrlEnv*_tmp1B3;const char*
_tmp3C9;struct Cyc_Tcenv_Fenv*_tmp1B1=Cyc_Tcenv_get_fenv(te,((_tmp3C9="process_fallthru",
_tag_dyneither(_tmp3C9,sizeof(char),17))));_tmp1B2=*_tmp1B1;_tmp1B3=_tmp1B2.ctrl_env;{
const struct _tuple10*_tmp1B4=(const struct _tuple10*)_tmp1B3->fallthru_clause;if(
_tmp1B4 != (const struct _tuple10*)0){{struct Cyc_List_List*_tmp3CA;(((*_tmp1B4).f1)->body)->non_local_preds=((
_tmp3CA=_cycalloc(sizeof(*_tmp3CA)),((_tmp3CA->hd=s,((_tmp3CA->tl=(((*_tmp1B4).f1)->body)->non_local_preds,
_tmp3CA))))));}{struct Cyc_Absyn_Switch_clause**_tmp3CB;*clauseopt=(struct Cyc_Absyn_Switch_clause**)((
_tmp3CB=_cycalloc(sizeof(*_tmp3CB)),((_tmp3CB[0]=(*_tmp1B4).f1,_tmp3CB))));};}
return _tmp1B4;};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,
struct Cyc_Absyn_Switch_clause*clause){struct Cyc_Tcenv_Fenv _tmp1B9;struct Cyc_Tcenv_SharedFenv*
_tmp1BA;struct Cyc_List_List*_tmp1BB;struct Cyc_RgnOrder_RgnPO*_tmp1BC;const struct
Cyc_Tcenv_Bindings*_tmp1BD;struct Cyc_Absyn_Stmt*_tmp1BE;struct Cyc_Tcenv_CtrlEnv*
_tmp1BF;void*_tmp1C0;void*_tmp1C1;int _tmp1C2;int _tmp1C3;const char*_tmp3CC;struct
Cyc_Tcenv_Fenv*_tmp1B8=Cyc_Tcenv_get_fenv(te,((_tmp3CC="set_fallthru",
_tag_dyneither(_tmp3CC,sizeof(char),13))));_tmp1B9=*_tmp1B8;_tmp1BA=_tmp1B9.shared;
_tmp1BB=_tmp1B9.type_vars;_tmp1BC=_tmp1B9.region_order;_tmp1BD=_tmp1B9.locals;
_tmp1BE=_tmp1B9.encloser;_tmp1BF=_tmp1B9.ctrl_env;_tmp1C0=_tmp1B9.capability;
_tmp1C1=_tmp1B9.curr_rgn;_tmp1C2=_tmp1B9.in_notreadctxt;_tmp1C3=_tmp1B9.in_lhs;{
struct Cyc_Tcenv_CtrlEnv _tmp1C5;struct _RegionHandle*_tmp1C6;void*_tmp1C7;void*
_tmp1C8;const struct _tuple10*_tmp1C9;void*_tmp1CA;int _tmp1CB;struct Cyc_Tcenv_CtrlEnv*
_tmp1C4=_tmp1BF;_tmp1C5=*_tmp1C4;_tmp1C6=_tmp1C5.ctrl_rgn;_tmp1C7=_tmp1C5.continue_stmt;
_tmp1C8=_tmp1C5.break_stmt;_tmp1C9=_tmp1C5.fallthru_clause;_tmp1CA=_tmp1C5.next_stmt;
_tmp1CB=_tmp1C5.try_depth;{struct Cyc_List_List*ft_typ=0;for(0;vds != 0;vds=vds->tl){
struct Cyc_List_List*_tmp3CD;ft_typ=((_tmp3CD=_region_malloc(_tmp1C6,sizeof(*
_tmp3CD)),((_tmp3CD->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp3CD->tl=
ft_typ,_tmp3CD))))));}{const struct Cyc_Tcenv_CList*_tmp1CD=(const struct Cyc_Tcenv_CList*)((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);struct
_tuple10*_tmp3D0;struct Cyc_Tcenv_CtrlEnv*_tmp3CF;struct Cyc_Tcenv_CtrlEnv*_tmp1CE=(
_tmp3CF=_region_malloc(r,sizeof(*_tmp3CF)),((_tmp3CF->ctrl_rgn=_tmp1C6,((_tmp3CF->continue_stmt=
_tmp1C7,((_tmp3CF->break_stmt=_tmp1C8,((_tmp3CF->fallthru_clause=(const struct
_tuple10*)((_tmp3D0=_region_malloc(_tmp1C6,sizeof(*_tmp3D0)),((_tmp3D0->f1=
clause,((_tmp3D0->f2=new_tvs,((_tmp3D0->f3=_tmp1CD,_tmp3D0)))))))),((_tmp3CF->next_stmt=
_tmp1CA,((_tmp3CF->try_depth=_tmp1CB,_tmp3CF)))))))))))));struct Cyc_Tcenv_Fenv*
_tmp3D1;struct Cyc_Tcenv_Fenv*_tmp1CF=(_tmp3D1=_region_malloc(r,sizeof(*_tmp3D1)),((
_tmp3D1->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1BA,((_tmp3D1->type_vars=(
struct Cyc_List_List*)_tmp1BB,((_tmp3D1->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp1BC,((_tmp3D1->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)
_tmp1BD),((_tmp3D1->encloser=(struct Cyc_Absyn_Stmt*)_tmp1BE,((_tmp3D1->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)_tmp1CE,((_tmp3D1->capability=(void*)_tmp1C0,((_tmp3D1->curr_rgn=(
void*)_tmp1C1,((_tmp3D1->in_notreadctxt=(int)_tmp1C2,((_tmp3D1->in_lhs=(int)
_tmp1C3,((_tmp3D1->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3D1)))))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp1CF);};};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*_tmp3D2;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3D2="clear_fallthru",
_tag_dyneither(_tmp3D2,sizeof(char),15)))));struct Cyc_Tcenv_Fenv _tmp1D4;struct
Cyc_Tcenv_CtrlEnv*_tmp1D5;struct Cyc_Tcenv_Fenv*_tmp1D3=fe;_tmp1D4=*_tmp1D3;
_tmp1D5=_tmp1D4.ctrl_env;_tmp1D5->fallthru_clause=(const struct _tuple10*)0;return
Cyc_Tcenv_put_fenv(r,te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
struct Cyc_Tcenv_Fenv _tmp1D9;struct Cyc_Tcenv_SharedFenv*_tmp1DA;struct Cyc_List_List*
_tmp1DB;struct Cyc_RgnOrder_RgnPO*_tmp1DC;const struct Cyc_Tcenv_Bindings*_tmp1DD;
struct Cyc_Absyn_Stmt*_tmp1DE;struct Cyc_Tcenv_CtrlEnv*_tmp1DF;void*_tmp1E0;void*
_tmp1E1;int _tmp1E2;int _tmp1E3;const char*_tmp3D3;struct Cyc_Tcenv_Fenv*_tmp1D8=Cyc_Tcenv_get_fenv(
te,((_tmp3D3="set_in_loop",_tag_dyneither(_tmp3D3,sizeof(char),12))));_tmp1D9=*
_tmp1D8;_tmp1DA=_tmp1D9.shared;_tmp1DB=_tmp1D9.type_vars;_tmp1DC=_tmp1D9.region_order;
_tmp1DD=_tmp1D9.locals;_tmp1DE=_tmp1D9.encloser;_tmp1DF=_tmp1D9.ctrl_env;_tmp1E0=
_tmp1D9.capability;_tmp1E1=_tmp1D9.curr_rgn;_tmp1E2=_tmp1D9.in_notreadctxt;
_tmp1E3=_tmp1D9.in_lhs;{struct Cyc_Tcenv_Stmt_j_struct*_tmp3DE;struct Cyc_Tcenv_Stmt_j_struct
_tmp3DD;struct Cyc_Tcenv_Stmt_j_struct*_tmp3DC;struct Cyc_Tcenv_Stmt_j_struct
_tmp3DB;struct Cyc_Tcenv_CtrlEnv*_tmp3DA;struct Cyc_Tcenv_CtrlEnv*new_cenv=(
_tmp3DA=_region_malloc(r,sizeof(*_tmp3DA)),((_tmp3DA->ctrl_rgn=r,((_tmp3DA->continue_stmt=(
void*)((_tmp3DC=_region_malloc(r,sizeof(*_tmp3DC)),((_tmp3DC[0]=((_tmp3DB.tag=3,((
_tmp3DB.f1=continue_dest,_tmp3DB)))),_tmp3DC)))),((_tmp3DA->break_stmt=(void*)
_tmp1DF->next_stmt,((_tmp3DA->next_stmt=(void*)((_tmp3DE=_region_malloc(r,
sizeof(*_tmp3DE)),((_tmp3DE[0]=((_tmp3DD.tag=3,((_tmp3DD.f1=continue_dest,
_tmp3DD)))),_tmp3DE)))),((_tmp3DA->fallthru_clause=(const struct _tuple10*)_tmp1DF->fallthru_clause,((
_tmp3DA->try_depth=_tmp1DF->try_depth,_tmp3DA)))))))))))));struct Cyc_Tcenv_Fenv*
_tmp3DF;struct Cyc_Tcenv_Fenv*new_fenv=(_tmp3DF=_region_malloc(r,sizeof(*_tmp3DF)),((
_tmp3DF->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1DA,((_tmp3DF->type_vars=(
struct Cyc_List_List*)_tmp1DB,((_tmp3DF->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp1DC,((_tmp3DF->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)
_tmp1DD),((_tmp3DF->encloser=(struct Cyc_Absyn_Stmt*)_tmp1DE,((_tmp3DF->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3DF->capability=(void*)_tmp1E0,((_tmp3DF->curr_rgn=(
void*)_tmp1E1,((_tmp3DF->in_notreadctxt=(int)_tmp1E2,((_tmp3DF->in_lhs=(int)
_tmp1E3,((_tmp3DF->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3DF)))))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*_tmp3E0;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3E0="enter_try",
_tag_dyneither(_tmp3E0,sizeof(char),10)))));struct Cyc_Tcenv_Fenv _tmp1EB;struct
Cyc_Tcenv_CtrlEnv*_tmp1EC;struct Cyc_Tcenv_Fenv*_tmp1EA=fe;_tmp1EB=*_tmp1EA;
_tmp1EC=_tmp1EB.ctrl_env;++ _tmp1EC->try_depth;return Cyc_Tcenv_put_fenv(r,te,fe);}
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp1F0;
struct Cyc_Tcenv_CtrlEnv*_tmp1F1;const char*_tmp3E1;struct Cyc_Tcenv_Fenv*_tmp1EF=
Cyc_Tcenv_get_fenv(te,((_tmp3E1="get_try_depth",_tag_dyneither(_tmp3E1,sizeof(
char),14))));_tmp1F0=*_tmp1EF;_tmp1F1=_tmp1F0.ctrl_env;return _tmp1F1->try_depth;}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te){const char*_tmp3E2;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(
te,((_tmp3E2="set_in_switch",_tag_dyneither(_tmp3E2,sizeof(char),14)))));struct
Cyc_Tcenv_Fenv _tmp1F3;struct Cyc_Tcenv_CtrlEnv*_tmp1F4;struct Cyc_Tcenv_Fenv*
_tmp1F2=fe;_tmp1F3=*_tmp1F2;_tmp1F4=_tmp1F3.ctrl_env;_tmp1F4->break_stmt=_tmp1F4->next_stmt;
_tmp1F4->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;return Cyc_Tcenv_put_fenv(r,te,
fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,void*j){struct Cyc_Tcenv_SharedFenv*_tmp1F8;struct Cyc_List_List*_tmp1F9;struct
Cyc_RgnOrder_RgnPO*_tmp1FA;const struct Cyc_Tcenv_Bindings*_tmp1FB;struct Cyc_Absyn_Stmt*
_tmp1FC;struct Cyc_Tcenv_CtrlEnv*_tmp1FD;void*_tmp1FE;void*_tmp1FF;int _tmp200;int
_tmp201;const char*_tmp3E3;struct Cyc_Tcenv_Fenv _tmp1F7=*Cyc_Tcenv_get_fenv(te,((
_tmp3E3="set_next",_tag_dyneither(_tmp3E3,sizeof(char),9))));_tmp1F8=_tmp1F7.shared;
_tmp1F9=_tmp1F7.type_vars;_tmp1FA=_tmp1F7.region_order;_tmp1FB=_tmp1F7.locals;
_tmp1FC=_tmp1F7.encloser;_tmp1FD=_tmp1F7.ctrl_env;_tmp1FE=_tmp1F7.capability;
_tmp1FF=_tmp1F7.curr_rgn;_tmp200=_tmp1F7.in_notreadctxt;_tmp201=_tmp1F7.in_lhs;{
struct Cyc_Tcenv_CtrlEnv*_tmp3E4;struct Cyc_Tcenv_CtrlEnv*new_cenv=(_tmp3E4=
_region_malloc(r,sizeof(*_tmp3E4)),((_tmp3E4->ctrl_rgn=r,((_tmp3E4->continue_stmt=(
void*)_tmp1FD->continue_stmt,((_tmp3E4->break_stmt=(void*)_tmp1FD->break_stmt,((
_tmp3E4->next_stmt=j,((_tmp3E4->fallthru_clause=(const struct _tuple10*)_tmp1FD->fallthru_clause,((
_tmp3E4->try_depth=_tmp1FD->try_depth,_tmp3E4)))))))))))));struct Cyc_Tcenv_Fenv*
_tmp3E5;struct Cyc_Tcenv_Fenv*new_fenv=(_tmp3E5=_region_malloc(r,sizeof(*_tmp3E5)),((
_tmp3E5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1F8,((_tmp3E5->type_vars=(
struct Cyc_List_List*)_tmp1F9,((_tmp3E5->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp1FA,((_tmp3E5->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)
_tmp1FB),((_tmp3E5->encloser=(struct Cyc_Absyn_Stmt*)_tmp1FC,((_tmp3E5->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3E5->capability=(void*)_tmp1FE,((_tmp3E5->curr_rgn=(
void*)_tmp1FF,((_tmp3E5->in_notreadctxt=(int)_tmp200,((_tmp3E5->in_lhs=(int)
_tmp201,((_tmp3E5->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3E5)))))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_Fenv
_tmp206;struct Cyc_Tcenv_SharedFenv*_tmp207;const char*_tmp3E6;struct Cyc_Tcenv_Fenv*
_tmp205=Cyc_Tcenv_get_fenv(te,((_tmp3E6="add_label",_tag_dyneither(_tmp3E6,
sizeof(char),10))));_tmp206=*_tmp205;_tmp207=_tmp206.shared;{struct Cyc_Dict_Dict
needed=_tmp207->needed_labels;struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp207->frgn;if(sl_opt != 0){_tmp207->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct
_dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{struct Cyc_List_List*_tmp208=*
sl_opt;s->non_local_preds=_tmp208;for(0;_tmp208 != 0;_tmp208=_tmp208->tl){void*
_tmp209=((struct Cyc_Absyn_Stmt*)_tmp208->hd)->r;struct Cyc_Absyn_Stmt*_tmp20B;
struct Cyc_Absyn_Stmt**_tmp20C;_LL8D: {struct Cyc_Absyn_Goto_s_struct*_tmp20A=(
struct Cyc_Absyn_Goto_s_struct*)_tmp209;if(_tmp20A->tag != 8)goto _LL8F;else{
_tmp20B=_tmp20A->f2;_tmp20C=(struct Cyc_Absyn_Stmt**)& _tmp20A->f2;}}_LL8E:*
_tmp20C=(struct Cyc_Absyn_Stmt*)s;goto _LL8C;_LL8F:;_LL90:{const char*_tmp3E7;((int(*)(
struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3E7="Tcenv: add_label backpatching of non-goto",
_tag_dyneither(_tmp3E7,sizeof(char),42))));}goto _LL8C;_LL8C:;}};}if(((int(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp207->seen_labels,
v)){const char*_tmp3EB;void*_tmp3EA[1];struct Cyc_String_pa_struct _tmp3E9;(_tmp3E9.tag=
0,((_tmp3E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3EA[0]=&
_tmp3E9,Cyc_Tcutil_terr(s->loc,((_tmp3EB="Repeated label: %s",_tag_dyneither(
_tmp3EB,sizeof(char),19))),_tag_dyneither(_tmp3EA,sizeof(void*),1)))))));}
_tmp207->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp207->seen_labels,v,
s);return te;};}int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){struct
Cyc_Tcenv_Fenv _tmp213;struct Cyc_Tcenv_SharedFenv*_tmp214;const char*_tmp3EC;
struct Cyc_Tcenv_Fenv*_tmp212=Cyc_Tcenv_get_fenv(te,((_tmp3EC="all_labels_resolved",
_tag_dyneither(_tmp3EC,sizeof(char),20))));_tmp213=*_tmp212;_tmp214=_tmp213.shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp214->needed_labels);}
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv
_tmp217;struct Cyc_Absyn_Stmt*_tmp218;const char*_tmp3ED;struct Cyc_Tcenv_Fenv*
_tmp216=Cyc_Tcenv_get_fenv(te,((_tmp3ED="get_encloser",_tag_dyneither(_tmp3ED,
sizeof(char),13))));_tmp217=*_tmp216;_tmp218=_tmp217.encloser;return _tmp218;}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*
te){if(te->le == 0){struct Cyc_Tcenv_Tenv*_tmp3EE;return(_tmp3EE=_region_malloc(r2,
sizeof(*_tmp3EE)),((_tmp3EE->ns=te->ns,((_tmp3EE->ae=te->ae,((_tmp3EE->le=0,((
_tmp3EE->allow_valueof=1,_tmp3EE)))))))));}{struct Cyc_Tcenv_SharedFenv*_tmp21B;
struct Cyc_List_List*_tmp21C;struct Cyc_RgnOrder_RgnPO*_tmp21D;const struct Cyc_Tcenv_Bindings*
_tmp21E;struct Cyc_Absyn_Stmt*_tmp21F;struct Cyc_Tcenv_CtrlEnv*_tmp220;void*
_tmp221;void*_tmp222;int _tmp223;int _tmp224;struct Cyc_Tcenv_Fenv _tmp21A=*((struct
Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp21B=_tmp21A.shared;_tmp21C=_tmp21A.type_vars;
_tmp21D=_tmp21A.region_order;_tmp21E=_tmp21A.locals;_tmp21F=_tmp21A.encloser;
_tmp220=_tmp21A.ctrl_env;_tmp221=_tmp21A.capability;_tmp222=_tmp21A.curr_rgn;
_tmp223=_tmp21A.in_notreadctxt;_tmp224=_tmp21A.in_lhs;{struct Cyc_Tcenv_Fenv*
_tmp3EF;struct Cyc_Tcenv_Fenv*_tmp225=(_tmp3EF=_region_malloc(r2,sizeof(*_tmp3EF)),((
_tmp3EF->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp21B,((_tmp3EF->type_vars=(
struct Cyc_List_List*)_tmp21C,((_tmp3EF->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp21D,((_tmp3EF->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)
_tmp21E),((_tmp3EF->encloser=(struct Cyc_Absyn_Stmt*)_tmp21F,((_tmp3EF->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp220),((_tmp3EF->capability=(
void*)_tmp221,((_tmp3EF->curr_rgn=(void*)_tmp222,((_tmp3EF->in_notreadctxt=(int)
_tmp223,((_tmp3EF->in_lhs=(int)_tmp224,((_tmp3EF->fnrgn=(struct _RegionHandle*)r2,(
struct Cyc_Tcenv_Fenv*)_tmp3EF)))))))))))))))))))))));struct Cyc_Tcenv_Tenv*
_tmp3F0;return(_tmp3F0=_region_malloc(r2,sizeof(*_tmp3F0)),((_tmp3F0->ns=te->ns,((
_tmp3F0->ae=te->ae,((_tmp3F0->le=_tmp225,((_tmp3F0->allow_valueof=1,_tmp3F0)))))))));};};}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_SharedFenv*_tmp22A;struct Cyc_List_List*
_tmp22B;struct Cyc_RgnOrder_RgnPO*_tmp22C;const struct Cyc_Tcenv_Bindings*_tmp22D;
struct Cyc_Absyn_Stmt*_tmp22E;struct Cyc_Tcenv_CtrlEnv*_tmp22F;void*_tmp230;void*
_tmp231;int _tmp232;int _tmp233;const char*_tmp3F1;struct Cyc_Tcenv_Fenv _tmp229=*Cyc_Tcenv_get_fenv(
te,((_tmp3F1="set_encloser",_tag_dyneither(_tmp3F1,sizeof(char),13))));_tmp22A=
_tmp229.shared;_tmp22B=_tmp229.type_vars;_tmp22C=_tmp229.region_order;_tmp22D=
_tmp229.locals;_tmp22E=_tmp229.encloser;_tmp22F=_tmp229.ctrl_env;_tmp230=_tmp229.capability;
_tmp231=_tmp229.curr_rgn;_tmp232=_tmp229.in_notreadctxt;_tmp233=_tmp229.in_lhs;{
struct Cyc_Tcenv_Fenv*_tmp3F2;struct Cyc_Tcenv_Fenv*_tmp234=(_tmp3F2=
_region_malloc(r,sizeof(*_tmp3F2)),((_tmp3F2->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp22A,((_tmp3F2->type_vars=(struct Cyc_List_List*)_tmp22B,((_tmp3F2->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp22C,((_tmp3F2->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp22D),((_tmp3F2->encloser=(struct Cyc_Absyn_Stmt*)
s,((_tmp3F2->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp22F),((_tmp3F2->capability=(void*)_tmp230,((_tmp3F2->curr_rgn=(void*)_tmp231,((
_tmp3F2->in_notreadctxt=(int)_tmp232,((_tmp3F2->in_lhs=(int)_tmp233,((_tmp3F2->fnrgn=(
struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3F2)))))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp234);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
struct Cyc_Absyn_Tvar*tv;{void*_tmp236=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*
_tmp238;_LL92: {struct Cyc_Absyn_VarType_struct*_tmp237=(struct Cyc_Absyn_VarType_struct*)
_tmp236;if(_tmp237->tag != 2)goto _LL94;else{_tmp238=_tmp237->f1;}}_LL93: tv=
_tmp238;goto _LL91;_LL94:;_LL95:{const char*_tmp3F3;tv=((struct Cyc_Absyn_Tvar*(*)(
struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3F3="bad add region",
_tag_dyneither(_tmp3F3,sizeof(char),15))));}goto _LL91;_LL91:;}{struct Cyc_Tcenv_SharedFenv*
_tmp23C;struct Cyc_List_List*_tmp23D;struct Cyc_RgnOrder_RgnPO*_tmp23E;const struct
Cyc_Tcenv_Bindings*_tmp23F;struct Cyc_Absyn_Stmt*_tmp240;struct Cyc_Tcenv_CtrlEnv*
_tmp241;void*_tmp242;void*_tmp243;int _tmp244;int _tmp245;const char*_tmp3F4;struct
Cyc_Tcenv_Fenv _tmp23B=*Cyc_Tcenv_get_fenv(te,((_tmp3F4="add_region",
_tag_dyneither(_tmp3F4,sizeof(char),11))));_tmp23C=_tmp23B.shared;_tmp23D=
_tmp23B.type_vars;_tmp23E=_tmp23B.region_order;_tmp23F=_tmp23B.locals;_tmp240=
_tmp23B.encloser;_tmp241=_tmp23B.ctrl_env;_tmp242=_tmp23B.capability;_tmp243=
_tmp23B.curr_rgn;_tmp244=_tmp23B.in_notreadctxt;_tmp245=_tmp23B.in_lhs;_tmp23E=
Cyc_RgnOrder_add_youngest(_tmp23C->frgn,_tmp23E,tv,resetable,opened);{struct Cyc_Absyn_JoinEff_struct
_tmp407;struct Cyc_List_List*_tmp406;struct Cyc_Absyn_AccessEff_struct*_tmp405;
struct Cyc_Absyn_AccessEff_struct _tmp404;struct Cyc_List_List*_tmp403;struct Cyc_Absyn_JoinEff_struct*
_tmp402;_tmp242=(void*)((_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402[0]=((
_tmp407.tag=24,((_tmp407.f1=((_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403->hd=(
void*)((_tmp405=_cycalloc(sizeof(*_tmp405)),((_tmp405[0]=((_tmp404.tag=23,((
_tmp404.f1=(void*)rgn,_tmp404)))),_tmp405)))),((_tmp403->tl=((_tmp406=_cycalloc(
sizeof(*_tmp406)),((_tmp406->hd=_tmp242,((_tmp406->tl=0,_tmp406)))))),_tmp403)))))),
_tmp407)))),_tmp402))));}{struct Cyc_Tcenv_Fenv*_tmp408;struct Cyc_Tcenv_Fenv*
_tmp24C=(_tmp408=_region_malloc(r,sizeof(*_tmp408)),((_tmp408->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp23C,((_tmp408->type_vars=(struct Cyc_List_List*)_tmp23D,((_tmp408->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp23E,((_tmp408->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp23F),((_tmp408->encloser=(struct Cyc_Absyn_Stmt*)
_tmp240,((_tmp408->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp241),((_tmp408->capability=(void*)_tmp242,((_tmp408->curr_rgn=(void*)_tmp243,((
_tmp408->in_notreadctxt=(int)_tmp244,((_tmp408->in_lhs=(int)_tmp245,((_tmp408->fnrgn=(
struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp408)))))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp24C);};};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Tvar*block_rgn){struct Cyc_Tcenv_SharedFenv*_tmp250;struct Cyc_List_List*
_tmp251;struct Cyc_RgnOrder_RgnPO*_tmp252;const struct Cyc_Tcenv_Bindings*_tmp253;
struct Cyc_Absyn_Stmt*_tmp254;struct Cyc_Tcenv_CtrlEnv*_tmp255;void*_tmp256;void*
_tmp257;int _tmp258;int _tmp259;const char*_tmp409;struct Cyc_Tcenv_Fenv _tmp24F=*Cyc_Tcenv_get_fenv(
te,((_tmp409="new_named_block",_tag_dyneither(_tmp409,sizeof(char),16))));
_tmp250=_tmp24F.shared;_tmp251=_tmp24F.type_vars;_tmp252=_tmp24F.region_order;
_tmp253=_tmp24F.locals;_tmp254=_tmp24F.encloser;_tmp255=_tmp24F.ctrl_env;_tmp256=
_tmp24F.capability;_tmp257=_tmp24F.curr_rgn;_tmp258=_tmp24F.in_notreadctxt;
_tmp259=_tmp24F.in_lhs;{const char*_tmp40A;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(
r,Cyc_Tcenv_get_fenv(te,((_tmp40A="new_block",_tag_dyneither(_tmp40A,sizeof(char),
10)))));struct Cyc_Absyn_VarType_struct _tmp40D;struct Cyc_Absyn_VarType_struct*
_tmp40C;void*block_typ=(void*)((_tmp40C=_cycalloc(sizeof(*_tmp40C)),((_tmp40C[0]=((
_tmp40D.tag=2,((_tmp40D.f1=block_rgn,_tmp40D)))),_tmp40C))));{struct Cyc_List_List*
_tmp40E;_tmp251=((_tmp40E=_cycalloc(sizeof(*_tmp40E)),((_tmp40E->hd=block_rgn,((
_tmp40E->tl=_tmp251,_tmp40E))))));}Cyc_Tcutil_check_unique_tvars(loc,_tmp251);
_tmp252=Cyc_RgnOrder_add_youngest(_tmp250->frgn,_tmp252,block_rgn,0,0);{struct
Cyc_Absyn_JoinEff_struct _tmp421;struct Cyc_List_List*_tmp420;struct Cyc_Absyn_AccessEff_struct*
_tmp41F;struct Cyc_Absyn_AccessEff_struct _tmp41E;struct Cyc_List_List*_tmp41D;
struct Cyc_Absyn_JoinEff_struct*_tmp41C;_tmp256=(void*)((_tmp41C=_cycalloc(
sizeof(*_tmp41C)),((_tmp41C[0]=((_tmp421.tag=24,((_tmp421.f1=((_tmp41D=_cycalloc(
sizeof(*_tmp41D)),((_tmp41D->hd=(void*)((_tmp41F=_cycalloc(sizeof(*_tmp41F)),((
_tmp41F[0]=((_tmp41E.tag=23,((_tmp41E.f1=(void*)block_typ,_tmp41E)))),_tmp41F)))),((
_tmp41D->tl=((_tmp420=_cycalloc(sizeof(*_tmp420)),((_tmp420->hd=_tmp256,((
_tmp420->tl=0,_tmp420)))))),_tmp41D)))))),_tmp421)))),_tmp41C))));}_tmp257=
block_typ;{struct Cyc_Tcenv_Fenv*_tmp422;struct Cyc_Tcenv_Fenv*_tmp261=(_tmp422=
_region_malloc(r,sizeof(*_tmp422)),((_tmp422->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp250,((_tmp422->type_vars=(struct Cyc_List_List*)_tmp251,((_tmp422->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp252,((_tmp422->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp253),((_tmp422->encloser=(struct Cyc_Absyn_Stmt*)
_tmp254,((_tmp422->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp255),((_tmp422->capability=(void*)_tmp256,((_tmp422->curr_rgn=(void*)_tmp257,((
_tmp422->in_notreadctxt=(int)_tmp258,((_tmp422->in_lhs=(int)_tmp259,((_tmp422->fnrgn=(
struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp422)))))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp261);};};}static struct Cyc_Absyn_Eq_kb_struct
Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);Cyc_Tcutil_add_tvar_identity(
t);return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*loc){
struct Cyc_Tcenv_SharedFenv*_tmp269;struct Cyc_List_List*_tmp26A;struct Cyc_RgnOrder_RgnPO*
_tmp26B;const struct Cyc_Tcenv_Bindings*_tmp26C;struct Cyc_Absyn_Stmt*_tmp26D;
struct Cyc_Tcenv_CtrlEnv*_tmp26E;void*_tmp26F;void*_tmp270;int _tmp271;int _tmp272;
const char*_tmp423;struct Cyc_Tcenv_Fenv _tmp268=*Cyc_Tcenv_get_fenv(te,((_tmp423="new_outlives_constraints",
_tag_dyneither(_tmp423,sizeof(char),25))));_tmp269=_tmp268.shared;_tmp26A=
_tmp268.type_vars;_tmp26B=_tmp268.region_order;_tmp26C=_tmp268.locals;_tmp26D=
_tmp268.encloser;_tmp26E=_tmp268.ctrl_env;_tmp26F=_tmp268.capability;_tmp270=
_tmp268.curr_rgn;_tmp271=_tmp268.in_notreadctxt;_tmp272=_tmp268.in_lhs;for(0;cs
!= 0;cs=cs->tl){_tmp26B=Cyc_RgnOrder_add_outlives_constraint(_tmp269->frgn,
_tmp26B,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
struct Cyc_Tcenv_Fenv*_tmp424;struct Cyc_Tcenv_Fenv*_tmp273=(_tmp424=
_region_malloc(r,sizeof(*_tmp424)),((_tmp424->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp269,((_tmp424->type_vars=(struct Cyc_List_List*)_tmp26A,((_tmp424->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp26B,((_tmp424->locals=(const struct Cyc_Tcenv_Bindings*)((
const struct Cyc_Tcenv_Bindings*)_tmp26C),((_tmp424->encloser=(struct Cyc_Absyn_Stmt*)
_tmp26D,((_tmp424->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp26E),((_tmp424->capability=(void*)_tmp26F,((_tmp424->curr_rgn=(void*)_tmp270,((
_tmp424->in_notreadctxt=(int)_tmp271,((_tmp424->in_lhs=(int)_tmp272,((_tmp424->fnrgn=(
struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp424)))))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp273);};}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**
oldtv,struct Cyc_Position_Segment*loc){void*_tmp275=Cyc_Tcutil_compress(r1);void*
_tmp276=Cyc_Tcutil_compress(r2);struct Cyc_Absyn_Kind*_tmp277=Cyc_Tcutil_typ_kind(
_tmp275);struct Cyc_Absyn_Kind*_tmp278=Cyc_Tcutil_typ_kind(_tmp276);int r1_le_r2=
Cyc_Tcutil_kind_leq(_tmp277,_tmp278);int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp278,
_tmp277);if(!r1_le_r2  && !r2_le_r1){{const char*_tmp42B;void*_tmp42A[4];struct Cyc_String_pa_struct
_tmp429;struct Cyc_String_pa_struct _tmp428;struct Cyc_String_pa_struct _tmp427;
struct Cyc_String_pa_struct _tmp426;(_tmp426.tag=0,((_tmp426.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp278)),((
_tmp427.tag=0,((_tmp427.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp277)),((_tmp428.tag=0,((_tmp428.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp276)),((_tmp429.tag=0,((_tmp429.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp275)),((
_tmp42A[0]=& _tmp429,((_tmp42A[1]=& _tmp428,((_tmp42A[2]=& _tmp427,((_tmp42A[3]=&
_tmp426,Cyc_Tcutil_terr(loc,((_tmp42B="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",
_tag_dyneither(_tmp42B,sizeof(char),82))),_tag_dyneither(_tmp42A,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{if(r1_le_r2  && !
r2_le_r1){struct Cyc_Absyn_Tvar*_tmp280;void*_tmp281;struct _tuple11 _tmp27F=Cyc_Tcutil_swap_kind(
_tmp276,Cyc_Tcutil_kind_to_bound(_tmp277));_tmp280=_tmp27F.f1;_tmp281=_tmp27F.f2;{
struct _tuple11*_tmp42C;*oldtv=((_tmp42C=_region_malloc(r,sizeof(*_tmp42C)),((
_tmp42C->f1=_tmp280,((_tmp42C->f2=_tmp281,_tmp42C))))));};}else{if(!r1_le_r2  && 
r2_le_r1){struct Cyc_Absyn_Tvar*_tmp284;void*_tmp285;struct _tuple11 _tmp283=Cyc_Tcutil_swap_kind(
_tmp275,Cyc_Tcutil_kind_to_bound(_tmp278));_tmp284=_tmp283.f1;_tmp285=_tmp283.f2;{
struct _tuple11*_tmp42D;*oldtv=((_tmp42D=_region_malloc(r,sizeof(*_tmp42D)),((
_tmp42D->f1=_tmp284,((_tmp42D->f2=_tmp285,_tmp42D))))));};}}}{struct
_RegionHandle*_tmp287=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp288=0;if((
_tmp275 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp275 != (void*)& Cyc_Absyn_UniqueRgn_val)
 && _tmp275 != (void*)& Cyc_Absyn_RefCntRgn_val){struct Cyc_Absyn_AccessEff_struct
_tmp430;struct Cyc_Absyn_AccessEff_struct*_tmp42F;void*eff1=(void*)((_tmp42F=
_cycalloc(sizeof(*_tmp42F)),((_tmp42F[0]=((_tmp430.tag=23,((_tmp430.f1=(void*)
_tmp275,_tmp430)))),_tmp42F))));struct _tuple13*_tmp433;struct Cyc_List_List*
_tmp432;_tmp288=((_tmp432=_region_malloc(_tmp287,sizeof(*_tmp432)),((_tmp432->hd=((
_tmp433=_region_malloc(_tmp287,sizeof(*_tmp433)),((_tmp433->f1=eff1,((_tmp433->f2=
_tmp276,_tmp433)))))),((_tmp432->tl=_tmp288,_tmp432))))));}if((_tmp276 != (void*)&
Cyc_Absyn_HeapRgn_val  && _tmp276 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp276 != (
void*)& Cyc_Absyn_RefCntRgn_val){struct Cyc_Absyn_AccessEff_struct _tmp436;struct
Cyc_Absyn_AccessEff_struct*_tmp435;void*eff2=(void*)((_tmp435=_cycalloc(sizeof(*
_tmp435)),((_tmp435[0]=((_tmp436.tag=23,((_tmp436.f1=(void*)_tmp276,_tmp436)))),
_tmp435))));struct _tuple13*_tmp439;struct Cyc_List_List*_tmp438;_tmp288=((_tmp438=
_region_malloc(_tmp287,sizeof(*_tmp438)),((_tmp438->hd=((_tmp439=_region_malloc(
_tmp287,sizeof(*_tmp439)),((_tmp439->f1=eff2,((_tmp439->f2=_tmp275,_tmp439)))))),((
_tmp438->tl=_tmp288,_tmp438))))));}return Cyc_Tcenv_new_outlives_constraints((
struct _RegionHandle*)_tmp287,te,_tmp288,loc);};}void*Cyc_Tcenv_curr_rgn(struct
Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp291=te->le;if(_tmp291 == 0)return(
void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)
_tmp291;struct Cyc_Tcenv_Fenv _tmp293;void*_tmp294;struct Cyc_Tcenv_Fenv*_tmp292=fe;
_tmp293=*_tmp292;_tmp294=_tmp293.curr_rgn;return _tmp294;};}void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn){const char*
_tmp43A;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp43A="check_rgn_accessible",
_tag_dyneither(_tmp43A,sizeof(char),21))));struct Cyc_Tcenv_Fenv _tmp296;void*
_tmp297;struct Cyc_RgnOrder_RgnPO*_tmp298;struct Cyc_Tcenv_Fenv*_tmp295=fe;_tmp296=*
_tmp295;_tmp297=_tmp296.capability;_tmp298=_tmp296.region_order;if(Cyc_Tcutil_region_in_effect(
0,rgn,_tmp297) || Cyc_Tcutil_region_in_effect(1,rgn,_tmp297))return;{struct Cyc_Absyn_AccessEff_struct
_tmp43D;struct Cyc_Absyn_AccessEff_struct*_tmp43C;if(Cyc_RgnOrder_eff_outlives_eff(
_tmp298,(void*)((_tmp43C=_cycalloc(sizeof(*_tmp43C)),((_tmp43C[0]=((_tmp43D.tag=
23,((_tmp43D.f1=(void*)rgn,_tmp43D)))),_tmp43C)))),_tmp297))return;}{const char*
_tmp441;void*_tmp440[1];struct Cyc_String_pa_struct _tmp43F;(_tmp43F.tag=0,((
_tmp43F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
rgn)),((_tmp440[0]=& _tmp43F,Cyc_Tcutil_terr(loc,((_tmp441="Expression accesses unavailable region %s",
_tag_dyneither(_tmp441,sizeof(char),42))),_tag_dyneither(_tmp440,sizeof(void*),1)))))));};}
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*rgn){Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{struct Cyc_Tcenv_Fenv
_tmp2A1;struct Cyc_RgnOrder_RgnPO*_tmp2A2;const char*_tmp442;struct Cyc_Tcenv_Fenv*
_tmp2A0=Cyc_Tcenv_get_fenv(te,((_tmp442="check_rgn_resetable",_tag_dyneither(
_tmp442,sizeof(char),20))));_tmp2A1=*_tmp2A0;_tmp2A2=_tmp2A1.region_order;{void*
_tmp2A3=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp2A5;_LL97: {struct Cyc_Absyn_VarType_struct*
_tmp2A4=(struct Cyc_Absyn_VarType_struct*)_tmp2A3;if(_tmp2A4->tag != 2)goto _LL99;
else{_tmp2A5=_tmp2A4->f1;}}_LL98: if(!Cyc_RgnOrder_is_region_resetable(_tmp2A2,
_tmp2A5)){const char*_tmp446;void*_tmp445[1];struct Cyc_String_pa_struct _tmp444;(
_tmp444.tag=0,((_tmp444.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
rgn)),((_tmp445[0]=& _tmp444,Cyc_Tcutil_terr(loc,((_tmp446="Region %s is not resetable",
_tag_dyneither(_tmp446,sizeof(char),27))),_tag_dyneither(_tmp445,sizeof(void*),1)))))));}
return;_LL99:;_LL9A: {const char*_tmp449;void*_tmp448;(_tmp448=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp449="check_rgn_resetable",
_tag_dyneither(_tmp449,sizeof(char),20))),_tag_dyneither(_tmp448,sizeof(void*),0)));}
_LL96:;};};}int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*
rt_b){struct Cyc_Tcenv_Fenv*_tmp2AB=te->le;rt_a=Cyc_Tcutil_compress(rt_a);rt_b=
Cyc_Tcutil_compress(rt_b);if(_tmp2AB == 0){void*_tmp2AC=rt_a;_LL9C: {struct Cyc_Absyn_RefCntRgn_struct*
_tmp2AD=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp2AC;if(_tmp2AD->tag != 22)goto
_LL9E;}_LL9D: return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;_LL9E: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp2AE=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp2AC;if(_tmp2AE->tag != 21)goto
_LLA0;}_LL9F: return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;_LLA0: {struct Cyc_Absyn_HeapRgn_struct*
_tmp2AF=(struct Cyc_Absyn_HeapRgn_struct*)_tmp2AC;if(_tmp2AF->tag != 20)goto _LLA2;}
_LLA1: return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;_LLA2:;_LLA3: return 0;_LL9B:;}{
struct Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)_tmp2AB;struct Cyc_Tcenv_Fenv
_tmp2B1;struct Cyc_RgnOrder_RgnPO*_tmp2B2;struct Cyc_Tcenv_Fenv*_tmp2B0=fe;_tmp2B1=*
_tmp2B0;_tmp2B2=_tmp2B1.region_order;{struct Cyc_Absyn_AccessEff_struct _tmp44C;
struct Cyc_Absyn_AccessEff_struct*_tmp44B;int _tmp2B3=Cyc_RgnOrder_effect_outlives(
_tmp2B2,(void*)((_tmp44B=_cycalloc(sizeof(*_tmp44B)),((_tmp44B[0]=((_tmp44C.tag=
23,((_tmp44C.f1=(void*)rt_a,_tmp44C)))),_tmp44B)))),rt_b);return _tmp2B3;};};}
struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;struct Cyc_Position_Segment*
f4;};void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*eff){struct Cyc_Tcenv_Fenv _tmp2B8;void*_tmp2B9;struct Cyc_RgnOrder_RgnPO*
_tmp2BA;struct Cyc_Tcenv_SharedFenv*_tmp2BB;const char*_tmp44D;struct Cyc_Tcenv_Fenv*
_tmp2B7=Cyc_Tcenv_get_fenv(te,((_tmp44D="check_effect_accessible",_tag_dyneither(
_tmp44D,sizeof(char),24))));_tmp2B8=*_tmp2B7;_tmp2B9=_tmp2B8.capability;_tmp2BA=
_tmp2B8.region_order;_tmp2BB=_tmp2B8.shared;if(Cyc_Tcutil_subset_effect(0,eff,
_tmp2B9))return;if(Cyc_RgnOrder_eff_outlives_eff(_tmp2BA,eff,_tmp2B9))return;{
struct _RegionHandle*frgn=_tmp2BB->frgn;struct _tuple14*_tmp450;struct Cyc_List_List*
_tmp44F;_tmp2BB->delayed_effect_checks=((_tmp44F=_region_malloc(frgn,sizeof(*
_tmp44F)),((_tmp44F->hd=((_tmp450=_region_malloc(frgn,sizeof(*_tmp450)),((
_tmp450->f1=_tmp2B9,((_tmp450->f2=eff,((_tmp450->f3=_tmp2BA,((_tmp450->f4=loc,
_tmp450)))))))))),((_tmp44F->tl=_tmp2BB->delayed_effect_checks,_tmp44F))))));};}
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv
_tmp2C0;struct Cyc_Tcenv_SharedFenv*_tmp2C1;const char*_tmp451;struct Cyc_Tcenv_Fenv*
_tmp2BF=Cyc_Tcenv_get_fenv(te,((_tmp451="check_delayed_effects",_tag_dyneither(
_tmp451,sizeof(char),22))));_tmp2C0=*_tmp2BF;_tmp2C1=_tmp2C0.shared;{struct Cyc_List_List*
_tmp2C2=_tmp2C1->delayed_effect_checks;for(0;_tmp2C2 != 0;_tmp2C2=_tmp2C2->tl){
struct _tuple14 _tmp2C4;void*_tmp2C5;void*_tmp2C6;struct Cyc_RgnOrder_RgnPO*_tmp2C7;
struct Cyc_Position_Segment*_tmp2C8;struct _tuple14*_tmp2C3=(struct _tuple14*)
_tmp2C2->hd;_tmp2C4=*_tmp2C3;_tmp2C5=_tmp2C4.f1;_tmp2C6=_tmp2C4.f2;_tmp2C7=
_tmp2C4.f3;_tmp2C8=_tmp2C4.f4;if(Cyc_Tcutil_subset_effect(1,_tmp2C6,_tmp2C5))
continue;if(Cyc_RgnOrder_eff_outlives_eff(_tmp2C7,_tmp2C6,_tmp2C5))continue;{
const char*_tmp456;void*_tmp455[2];struct Cyc_String_pa_struct _tmp454;struct Cyc_String_pa_struct
_tmp453;(_tmp453.tag=0,((_tmp453.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2C6)),((_tmp454.tag=0,((_tmp454.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C5)),((_tmp455[
0]=& _tmp454,((_tmp455[1]=& _tmp453,Cyc_Tcutil_terr(_tmp2C8,((_tmp456="Capability \n%s\ndoes not cover function's effect\n%s",
_tag_dyneither(_tmp456,sizeof(char),51))),_tag_dyneither(_tmp455,sizeof(void*),2)))))))))))));};}};}
struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;struct Cyc_Position_Segment*
f3;};void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*po){struct Cyc_Tcenv_Fenv*_tmp2CD=te->le;if(_tmp2CD == 0){
for(0;po != 0;po=po->tl){struct Cyc_Absyn_AccessEff_struct _tmp459;struct Cyc_Absyn_AccessEff_struct*
_tmp458;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect)
 || !Cyc_Tcutil_subset_effect(1,(void*)((_tmp458=_cycalloc(sizeof(*_tmp458)),((
_tmp458[0]=((_tmp459.tag=23,((_tmp459.f1=(void*)(*((struct _tuple13*)po->hd)).f2,
_tmp459)))),_tmp458)))),Cyc_Absyn_empty_effect)){const char*_tmp45C;void*_tmp45B;(
_tmp45B=0,Cyc_Tcutil_terr(loc,((_tmp45C="the required region ordering is not satisfied here",
_tag_dyneither(_tmp45C,sizeof(char),51))),_tag_dyneither(_tmp45B,sizeof(void*),0)));}}
return;}{struct Cyc_Tcenv_Fenv _tmp2D3;struct Cyc_RgnOrder_RgnPO*_tmp2D4;struct Cyc_Tcenv_SharedFenv*
_tmp2D5;struct Cyc_Tcenv_Fenv*_tmp2D2=(struct Cyc_Tcenv_Fenv*)_tmp2CD;_tmp2D3=*
_tmp2D2;_tmp2D4=_tmp2D3.region_order;_tmp2D5=_tmp2D3.shared;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp2D4,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){struct _tuple15*_tmp45F;struct Cyc_List_List*
_tmp45E;_tmp2D5->delayed_constraint_checks=((_tmp45E=_region_malloc(_tmp2D5->frgn,
sizeof(*_tmp45E)),((_tmp45E->hd=((_tmp45F=_region_malloc(_tmp2D5->frgn,sizeof(*
_tmp45F)),((_tmp45F->f1=_tmp2D4,((_tmp45F->f2=po,((_tmp45F->f3=loc,_tmp45F)))))))),((
_tmp45E->tl=_tmp2D5->delayed_constraint_checks,_tmp45E))))));}};}void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp2DA;struct Cyc_Tcenv_SharedFenv*
_tmp2DB;const char*_tmp460;struct Cyc_Tcenv_Fenv*_tmp2D9=Cyc_Tcenv_get_fenv(te,((
_tmp460="check_delayed_constraints",_tag_dyneither(_tmp460,sizeof(char),26))));
_tmp2DA=*_tmp2D9;_tmp2DB=_tmp2DA.shared;{struct Cyc_List_List*_tmp2DC=_tmp2DB->delayed_constraint_checks;
for(0;_tmp2DC != 0;_tmp2DC=_tmp2DC->tl){struct _tuple15 _tmp2DE;struct Cyc_RgnOrder_RgnPO*
_tmp2DF;struct Cyc_List_List*_tmp2E0;struct Cyc_Position_Segment*_tmp2E1;struct
_tuple15*_tmp2DD=(struct _tuple15*)_tmp2DC->hd;_tmp2DE=*_tmp2DD;_tmp2DF=_tmp2DE.f1;
_tmp2E0=_tmp2DE.f2;_tmp2E1=_tmp2DE.f3;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp2DF,_tmp2E0,(void*)& Cyc_Absyn_HeapRgn_val,1)){const char*_tmp463;void*_tmp462;(
_tmp462=0,Cyc_Tcutil_terr(_tmp2E1,((_tmp463="the required region ordering is not satisfied here",
_tag_dyneither(_tmp463,sizeof(char),51))),_tag_dyneither(_tmp462,sizeof(void*),0)));}}};}
struct _tuple16{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct
Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Fndecl*fd){const struct Cyc_Tcenv_Bindings*locals=(const struct
Cyc_Tcenv_Bindings*)0;struct _dyneither_ptr*_tmp470;const char*_tmp46F;void*
_tmp46E[1];struct Cyc_String_pa_struct _tmp46D;struct Cyc_Absyn_Tvar*_tmp46C;struct
Cyc_Absyn_Tvar*rgn0=(_tmp46C=_cycalloc(sizeof(*_tmp46C)),((_tmp46C->name=((
_tmp470=_cycalloc(sizeof(*_tmp470)),((_tmp470[0]=(struct _dyneither_ptr)((_tmp46D.tag=
0,((_tmp46D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((
_tmp46E[0]=& _tmp46D,Cyc_aprintf(((_tmp46F="`%s",_tag_dyneither(_tmp46F,sizeof(
char),4))),_tag_dyneither(_tmp46E,sizeof(void*),1)))))))),_tmp470)))),((_tmp46C->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp46C->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp46C)))))));
struct Cyc_List_List*_tmp471;struct Cyc_List_List*_tmp2E4=(_tmp471=_cycalloc(
sizeof(*_tmp471)),((_tmp471->hd=rgn0,((_tmp471->tl=fd->tvs,_tmp471)))));Cyc_Tcutil_check_unique_tvars(
loc,_tmp2E4);{struct Cyc_RgnOrder_RgnPO*_tmp2E5=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,
fd->rgn_po,(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,rgn0,loc);
struct Cyc_Absyn_VarType_struct _tmp474;struct Cyc_Absyn_VarType_struct*_tmp473;
void*param_rgn=(void*)((_tmp473=_cycalloc(sizeof(*_tmp473)),((_tmp473[0]=((
_tmp474.tag=2,((_tmp474.f1=rgn0,_tmp474)))),_tmp473))));struct Cyc_Core_Opt*
_tmp475;struct Cyc_Core_Opt*param_rgn_opt=(_tmp475=_cycalloc(sizeof(*_tmp475)),((
_tmp475->v=param_rgn,_tmp475)));struct Cyc_List_List*_tmp2E6=0;{struct Cyc_List_List*
_tmp2E7=fd->args;for(0;_tmp2E7 != 0;_tmp2E7=_tmp2E7->tl){struct Cyc_Absyn_Vardecl
_tmp47B;struct _tuple0*_tmp47A;struct Cyc_Absyn_Vardecl*_tmp479;struct Cyc_Absyn_Vardecl*
_tmp2E8=(_tmp479=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp479[0]=((
_tmp47B.sc=Cyc_Absyn_Public,((_tmp47B.name=((_tmp47A=_cycalloc(sizeof(*_tmp47A)),((
_tmp47A->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp47A->f2=(*((struct
_tuple16*)_tmp2E7->hd)).f1,_tmp47A)))))),((_tmp47B.tq=(*((struct _tuple16*)
_tmp2E7->hd)).f2,((_tmp47B.type=(*((struct _tuple16*)_tmp2E7->hd)).f3,((_tmp47B.initializer=
0,((_tmp47B.rgn=param_rgn_opt,((_tmp47B.attributes=0,((_tmp47B.escapes=0,_tmp47B)))))))))))))))),
_tmp479)));{struct Cyc_List_List _tmp47E;struct Cyc_List_List*_tmp47D;_tmp2E6=((
_tmp47D=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp47D[0]=((_tmp47E.hd=
_tmp2E8,((_tmp47E.tl=_tmp2E6,_tmp47E)))),_tmp47D))));}{struct Cyc_Absyn_Param_b_struct*
_tmp484;struct Cyc_Absyn_Param_b_struct _tmp483;struct Cyc_Tcenv_Bindings*_tmp482;
locals=(const struct Cyc_Tcenv_Bindings*)((_tmp482=_region_malloc(r,sizeof(*
_tmp482)),((_tmp482->v=(*((struct _tuple16*)_tmp2E7->hd)).f1,((_tmp482->b=(void*)((
_tmp484=_cycalloc(sizeof(*_tmp484)),((_tmp484[0]=((_tmp483.tag=3,((_tmp483.f1=
_tmp2E8,_tmp483)))),_tmp484)))),((_tmp482->tl=locals,_tmp482))))))));};}}if(fd->cyc_varargs
!= 0){struct Cyc_Core_Opt*_tmp2F2;struct Cyc_Absyn_Tqual _tmp2F3;void*_tmp2F4;int
_tmp2F5;struct Cyc_Absyn_VarargInfo _tmp2F1=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp2F2=_tmp2F1.name;_tmp2F3=_tmp2F1.tq;_tmp2F4=
_tmp2F1.type;_tmp2F5=_tmp2F1.inject;if(_tmp2F2 != 0){void*_tmp2F6=Cyc_Absyn_dyneither_typ(
_tmp2F4,param_rgn,_tmp2F3,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl _tmp48A;
struct _tuple0*_tmp489;struct Cyc_Absyn_Vardecl*_tmp488;struct Cyc_Absyn_Vardecl*
_tmp2F7=(_tmp488=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp488[0]=((
_tmp48A.sc=Cyc_Absyn_Public,((_tmp48A.name=((_tmp489=_cycalloc(sizeof(*_tmp489)),((
_tmp489->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp489->f2=(struct
_dyneither_ptr*)_tmp2F2->v,_tmp489)))))),((_tmp48A.tq=Cyc_Absyn_empty_tqual(0),((
_tmp48A.type=_tmp2F6,((_tmp48A.initializer=0,((_tmp48A.rgn=param_rgn_opt,((
_tmp48A.attributes=0,((_tmp48A.escapes=0,_tmp48A)))))))))))))))),_tmp488)));{
struct Cyc_List_List*_tmp48B;_tmp2E6=((_tmp48B=_cycalloc(sizeof(*_tmp48B)),((
_tmp48B->hd=_tmp2F7,((_tmp48B->tl=_tmp2E6,_tmp48B))))));}{struct Cyc_Absyn_Param_b_struct*
_tmp491;struct Cyc_Absyn_Param_b_struct _tmp490;struct Cyc_Tcenv_Bindings*_tmp48F;
locals=(const struct Cyc_Tcenv_Bindings*)((_tmp48F=_region_malloc(r,sizeof(*
_tmp48F)),((_tmp48F->v=(struct _dyneither_ptr*)_tmp2F2->v,((_tmp48F->b=(void*)((
_tmp491=_cycalloc(sizeof(*_tmp491)),((_tmp491[0]=((_tmp490.tag=3,((_tmp490.f1=
_tmp2F7,_tmp490)))),_tmp491)))),((_tmp48F->tl=locals,_tmp48F))))))));};}else{
const char*_tmp494;void*_tmp493;(_tmp493=0,Cyc_Tcutil_terr(loc,((_tmp494="missing name for varargs",
_tag_dyneither(_tmp494,sizeof(char),25))),_tag_dyneither(_tmp493,sizeof(void*),0)));}}{
struct Cyc_Core_Opt _tmp497;struct Cyc_Core_Opt*_tmp496;fd->param_vardecls=((
_tmp496=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp496[0]=((_tmp497.v=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2E6),
_tmp497)),_tmp496))));}{struct Cyc_Absyn_JoinEff_struct*_tmp4B5;struct Cyc_List_List*
_tmp4B4;struct Cyc_Absyn_AccessEff_struct _tmp4B3;struct Cyc_Absyn_AccessEff_struct*
_tmp4B2;struct Cyc_List_List*_tmp4B1;struct Cyc_Absyn_JoinEff_struct _tmp4B0;struct
Cyc_Tcenv_CtrlEnv*_tmp4AF;struct Cyc_Tcenv_SharedFenv*_tmp4AE;struct Cyc_Tcenv_Fenv*
_tmp4AD;return(_tmp4AD=_region_malloc(r,sizeof(*_tmp4AD)),((_tmp4AD->shared=(
struct Cyc_Tcenv_SharedFenv*)((_tmp4AE=_region_malloc(r,sizeof(*_tmp4AE)),((
_tmp4AE->frgn=r,((_tmp4AE->return_typ=fd->ret_type,((_tmp4AE->seen_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp4AE->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp4AE->delayed_effect_checks=
0,((_tmp4AE->delayed_constraint_checks=0,_tmp4AE)))))))))))))),((_tmp4AD->type_vars=(
struct Cyc_List_List*)_tmp2E4,((_tmp4AD->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp2E5,((_tmp4AD->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp4AD->encloser=(
struct Cyc_Absyn_Stmt*)fd->body,((_tmp4AD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
_tmp4AF=_region_malloc(r,sizeof(*_tmp4AF)),((_tmp4AF->ctrl_rgn=r,((_tmp4AF->continue_stmt=(
void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4AF->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((
_tmp4AF->fallthru_clause=(const struct _tuple10*)0,((_tmp4AF->next_stmt=(void*)&
Cyc_Tcenv_FnEnd_j_val,((_tmp4AF->try_depth=0,_tmp4AF)))))))))))))),((_tmp4AD->capability=(
void*)((void*)((_tmp4B5=_cycalloc(sizeof(*_tmp4B5)),((_tmp4B5[0]=((_tmp4B0.tag=
24,((_tmp4B0.f1=((_tmp4B4=_cycalloc(sizeof(*_tmp4B4)),((_tmp4B4->hd=(void*)((
_tmp4B2=_cycalloc(sizeof(*_tmp4B2)),((_tmp4B2[0]=((_tmp4B3.tag=23,((_tmp4B3.f1=(
void*)param_rgn,_tmp4B3)))),_tmp4B2)))),((_tmp4B4->tl=((_tmp4B1=_cycalloc(
sizeof(*_tmp4B1)),((_tmp4B1->hd=(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,((
_tmp4B1->tl=0,_tmp4B1)))))),_tmp4B4)))))),_tmp4B0)))),_tmp4B5))))),((_tmp4AD->curr_rgn=(
void*)param_rgn,((_tmp4AD->in_notreadctxt=(int)0,((_tmp4AD->in_lhs=(int)0,((
_tmp4AD->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp4AD)))))))))))))))))))))));};};}
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Tcenv_Fenv _tmp316;
const struct Cyc_Tcenv_Bindings*_tmp317;struct Cyc_RgnOrder_RgnPO*_tmp318;struct Cyc_List_List*
_tmp319;struct Cyc_Tcenv_SharedFenv*_tmp31A;struct _RegionHandle*_tmp31B;struct Cyc_Tcenv_Fenv*
_tmp315=old_fenv;_tmp316=*_tmp315;_tmp317=_tmp316.locals;_tmp318=_tmp316.region_order;
_tmp319=_tmp316.type_vars;_tmp31A=_tmp316.shared;_tmp31B=_tmp316.fnrgn;{struct
_RegionHandle*_tmp31C=_tmp31A->frgn;const struct Cyc_Tcenv_Bindings*_tmp31D=(const
struct Cyc_Tcenv_Bindings*)_tmp317;struct _dyneither_ptr*_tmp4C2;const char*_tmp4C1;
void*_tmp4C0[1];struct Cyc_String_pa_struct _tmp4BF;struct Cyc_Absyn_Tvar*_tmp4BE;
struct Cyc_Absyn_Tvar*rgn0=(_tmp4BE=_cycalloc(sizeof(*_tmp4BE)),((_tmp4BE->name=((
_tmp4C2=_cycalloc(sizeof(*_tmp4C2)),((_tmp4C2[0]=(struct _dyneither_ptr)((_tmp4BF.tag=
0,((_tmp4BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((
_tmp4C0[0]=& _tmp4BF,Cyc_aprintf(((_tmp4C1="`%s",_tag_dyneither(_tmp4C1,sizeof(
char),4))),_tag_dyneither(_tmp4C0,sizeof(void*),1)))))))),_tmp4C2)))),((_tmp4BE->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp4BE->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp4BE)))))));{
struct Cyc_List_List*_tmp31E=fd->tvs;for(0;_tmp31E != 0;_tmp31E=_tmp31E->tl){
struct Cyc_Absyn_Kind _tmp320;enum Cyc_Absyn_KindQual _tmp321;enum Cyc_Absyn_AliasQual
_tmp322;struct Cyc_Absyn_Kind*_tmp31F=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp31E->hd,& Cyc_Tcutil_bk);_tmp320=*_tmp31F;_tmp321=_tmp320.kind;_tmp322=
_tmp320.aliasqual;if(_tmp321 == Cyc_Absyn_RgnKind){if(_tmp322 == Cyc_Absyn_Aliasable)
_tmp318=Cyc_RgnOrder_add_youngest(_tmp31C,_tmp318,(struct Cyc_Absyn_Tvar*)_tmp31E->hd,
0,0);else{const char*_tmp4C5;void*_tmp4C4;(_tmp4C4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4C5="non-intuitionistic tvar in nested_fenv",
_tag_dyneither(_tmp4C5,sizeof(char),39))),_tag_dyneither(_tmp4C4,sizeof(void*),0)));}}}}
_tmp318=Cyc_RgnOrder_add_youngest(_tmp31C,_tmp318,rgn0,0,0);{struct Cyc_List_List*
_tmp4C6;struct Cyc_List_List*_tmp325=(_tmp4C6=_cycalloc(sizeof(*_tmp4C6)),((
_tmp4C6->hd=rgn0,((_tmp4C6->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp319),_tmp4C6)))));Cyc_Tcutil_check_unique_tvars(
loc,_tmp325);{struct Cyc_Absyn_VarType_struct _tmp4C9;struct Cyc_Absyn_VarType_struct*
_tmp4C8;void*param_rgn=(void*)((_tmp4C8=_cycalloc(sizeof(*_tmp4C8)),((_tmp4C8[0]=((
_tmp4C9.tag=2,((_tmp4C9.f1=rgn0,_tmp4C9)))),_tmp4C8))));struct Cyc_Core_Opt*
_tmp4CA;struct Cyc_Core_Opt*param_rgn_opt=(_tmp4CA=_cycalloc(sizeof(*_tmp4CA)),((
_tmp4CA->v=param_rgn,_tmp4CA)));struct Cyc_List_List*_tmp326=0;{struct Cyc_List_List*
_tmp327=fd->args;for(0;_tmp327 != 0;_tmp327=_tmp327->tl){struct Cyc_Absyn_Vardecl
_tmp4D0;struct _tuple0*_tmp4CF;struct Cyc_Absyn_Vardecl*_tmp4CE;struct Cyc_Absyn_Vardecl*
_tmp328=(_tmp4CE=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4CE[0]=((
_tmp4D0.sc=Cyc_Absyn_Public,((_tmp4D0.name=((_tmp4CF=_cycalloc(sizeof(*_tmp4CF)),((
_tmp4CF->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp4CF->f2=(*((struct
_tuple16*)_tmp327->hd)).f1,_tmp4CF)))))),((_tmp4D0.tq=(*((struct _tuple16*)
_tmp327->hd)).f2,((_tmp4D0.type=(*((struct _tuple16*)_tmp327->hd)).f3,((_tmp4D0.initializer=
0,((_tmp4D0.rgn=param_rgn_opt,((_tmp4D0.attributes=0,((_tmp4D0.escapes=0,_tmp4D0)))))))))))))))),
_tmp4CE)));{struct Cyc_List_List _tmp4D3;struct Cyc_List_List*_tmp4D2;_tmp326=((
_tmp4D2=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp4D2[0]=((_tmp4D3.hd=
_tmp328,((_tmp4D3.tl=_tmp326,_tmp4D3)))),_tmp4D2))));}{struct Cyc_Absyn_Param_b_struct*
_tmp4D9;struct Cyc_Absyn_Param_b_struct _tmp4D8;struct Cyc_Tcenv_Bindings*_tmp4D7;
_tmp31D=(const struct Cyc_Tcenv_Bindings*)((_tmp4D7=_region_malloc(_tmp31C,sizeof(*
_tmp4D7)),((_tmp4D7->v=(*((struct _tuple16*)_tmp327->hd)).f1,((_tmp4D7->b=(void*)((
_tmp4D9=_cycalloc(sizeof(*_tmp4D9)),((_tmp4D9[0]=((_tmp4D8.tag=3,((_tmp4D8.f1=
_tmp328,_tmp4D8)))),_tmp4D9)))),((_tmp4D7->tl=_tmp31D,_tmp4D7))))))));};}}if(fd->cyc_varargs
!= 0){struct Cyc_Core_Opt*_tmp332;struct Cyc_Absyn_Tqual _tmp333;void*_tmp334;int
_tmp335;struct Cyc_Absyn_VarargInfo _tmp331=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp332=_tmp331.name;_tmp333=_tmp331.tq;_tmp334=
_tmp331.type;_tmp335=_tmp331.inject;if(_tmp332 != 0){void*_tmp336=Cyc_Absyn_dyneither_typ(
_tmp334,param_rgn,_tmp333,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl _tmp4DF;
struct _tuple0*_tmp4DE;struct Cyc_Absyn_Vardecl*_tmp4DD;struct Cyc_Absyn_Vardecl*
_tmp337=(_tmp4DD=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4DD[0]=((
_tmp4DF.sc=Cyc_Absyn_Public,((_tmp4DF.name=((_tmp4DE=_cycalloc(sizeof(*_tmp4DE)),((
_tmp4DE->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp4DE->f2=(struct
_dyneither_ptr*)_tmp332->v,_tmp4DE)))))),((_tmp4DF.tq=Cyc_Absyn_empty_tqual(0),((
_tmp4DF.type=_tmp336,((_tmp4DF.initializer=0,((_tmp4DF.rgn=param_rgn_opt,((
_tmp4DF.attributes=0,((_tmp4DF.escapes=0,_tmp4DF)))))))))))))))),_tmp4DD)));{
struct Cyc_List_List*_tmp4E0;_tmp326=((_tmp4E0=_cycalloc(sizeof(*_tmp4E0)),((
_tmp4E0->hd=_tmp337,((_tmp4E0->tl=_tmp326,_tmp4E0))))));}{struct Cyc_Absyn_Param_b_struct*
_tmp4E6;struct Cyc_Absyn_Param_b_struct _tmp4E5;struct Cyc_Tcenv_Bindings*_tmp4E4;
_tmp31D=(const struct Cyc_Tcenv_Bindings*)((_tmp4E4=_region_malloc(_tmp31C,sizeof(*
_tmp4E4)),((_tmp4E4->v=(struct _dyneither_ptr*)_tmp332->v,((_tmp4E4->b=(void*)((
_tmp4E6=_cycalloc(sizeof(*_tmp4E6)),((_tmp4E6[0]=((_tmp4E5.tag=3,((_tmp4E5.f1=
_tmp337,_tmp4E5)))),_tmp4E6)))),((_tmp4E4->tl=_tmp31D,_tmp4E4))))))));};}else{
const char*_tmp4E9;void*_tmp4E8;(_tmp4E8=0,Cyc_Tcutil_terr(loc,((_tmp4E9="missing name for varargs",
_tag_dyneither(_tmp4E9,sizeof(char),25))),_tag_dyneither(_tmp4E8,sizeof(void*),0)));}}{
struct Cyc_Core_Opt _tmp4EC;struct Cyc_Core_Opt*_tmp4EB;fd->param_vardecls=((
_tmp4EB=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4EB[0]=((_tmp4EC.v=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp326),
_tmp4EC)),_tmp4EB))));}{struct Cyc_Absyn_JoinEff_struct*_tmp50A;struct Cyc_List_List*
_tmp509;struct Cyc_Absyn_AccessEff_struct _tmp508;struct Cyc_Absyn_AccessEff_struct*
_tmp507;struct Cyc_List_List*_tmp506;struct Cyc_Absyn_JoinEff_struct _tmp505;struct
Cyc_Tcenv_CtrlEnv*_tmp504;struct Cyc_Tcenv_SharedFenv*_tmp503;struct Cyc_Tcenv_Fenv*
_tmp502;return(struct Cyc_Tcenv_Fenv*)((_tmp502=_region_malloc(_tmp31C,sizeof(*
_tmp502)),((_tmp502->shared=(struct Cyc_Tcenv_SharedFenv*)((_tmp503=
_region_malloc(_tmp31C,sizeof(*_tmp503)),((_tmp503->frgn=_tmp31C,((_tmp503->return_typ=
fd->ret_type,((_tmp503->seen_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp31C,
Cyc_strptrcmp),((_tmp503->needed_labels=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
_tmp31C,Cyc_strptrcmp),((_tmp503->delayed_effect_checks=0,((_tmp503->delayed_constraint_checks=
0,_tmp503)))))))))))))),((_tmp502->type_vars=(struct Cyc_List_List*)_tmp325,((
_tmp502->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp318,((_tmp502->locals=(
const struct Cyc_Tcenv_Bindings*)_tmp31D,((_tmp502->encloser=(struct Cyc_Absyn_Stmt*)
fd->body,((_tmp502->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)((
_tmp504=_region_malloc(_tmp31C,sizeof(*_tmp504)),((_tmp504->ctrl_rgn=_tmp31C,((
_tmp504->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp504->break_stmt=(
void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp504->fallthru_clause=(const struct _tuple10*)
0,((_tmp504->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp504->try_depth=0,
_tmp504))))))))))))))),((_tmp502->capability=(void*)((void*)((_tmp50A=_cycalloc(
sizeof(*_tmp50A)),((_tmp50A[0]=((_tmp505.tag=24,((_tmp505.f1=((_tmp509=_cycalloc(
sizeof(*_tmp509)),((_tmp509->hd=(void*)((_tmp507=_cycalloc(sizeof(*_tmp507)),((
_tmp507[0]=((_tmp508.tag=23,((_tmp508.f1=(void*)param_rgn,_tmp508)))),_tmp507)))),((
_tmp509->tl=((_tmp506=_cycalloc(sizeof(*_tmp506)),((_tmp506->hd=(void*)((struct
Cyc_Core_Opt*)_check_null(fd->effect))->v,((_tmp506->tl=0,_tmp506)))))),_tmp509)))))),
_tmp505)))),_tmp50A))))),((_tmp502->curr_rgn=(void*)param_rgn,((_tmp502->in_notreadctxt=(
int)0,((_tmp502->in_lhs=(int)0,((_tmp502->fnrgn=(struct _RegionHandle*)_tmp31B,(
struct Cyc_Tcenv_Fenv*)_tmp502))))))))))))))))))))))));};};};};}
